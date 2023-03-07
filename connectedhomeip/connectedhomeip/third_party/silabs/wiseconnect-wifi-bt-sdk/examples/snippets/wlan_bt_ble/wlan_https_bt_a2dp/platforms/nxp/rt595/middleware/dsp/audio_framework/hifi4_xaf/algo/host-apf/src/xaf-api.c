/*******************************************************************************
* Copyright (c) 2015-2019 Cadence Design Systems, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and 
* not with any other processors and platforms, subject to
* the following conditions:
* 
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/
#include "xos-msgq-if.h"
#include "xf.h"
#include "xaf-api.h"
#include "xaf-structs.h"
#include "xaf-version.h"

#define MODULE_TAG                      DEVAPI

/***** GLOBALS *****/
/* Info table */
/*******************************************************************************
 * Tracing configuration
 ******************************************************************************/

TRACE_TAG(INIT, 1);
TRACE_TAG(DEBUG, 1);
TRACE_TAG(INFO, 1);

#define XAF_4BYTE_ALIGN    4
#define XAF_8BYTE_ALIGN    8
#define XAF_32BYTE_ALIGN    32
#define XAF_DEV_AND_AP_STRUCT_MEM_SIZE \
   (sizeof(xf_ap_t) + (XAF_8BYTE_ALIGN-1) + \
   (sizeof(xaf_adev_t) + (XAF_4BYTE_ALIGN-1)))

/* ...check null pointer */ 
#define XAF_CHK_PTR(ptr)                                     \
({                                                          \
    int __ret;                                              \
                                                            \
    if ((__ret = (int)(ptr)) == 0)                          \
    {                                                       \
        TRACE(ERROR, _x("Null pointer error: %d"), __ret);  \
        return XAF_PTR_ERROR;                               \
    }                                                       \
    __ret;                                                  \
})

/* ...check range */
#define XAF_CHK_RANGE(val, min, max)                         \
({                                                          \
    int __ret = val;                                        \
                                                            \
    if ((__ret < (int)min) || (__ret > (int)max))           \
    {                                                       \
        TRACE(ERROR, _x("Invalid value: %d"), __ret);       \
        return XAF_INVALID_VALUE;                           \
    }                                                       \
    __ret;                                                  \
})

#define XAF_ADEV_STATE_CHK(ptr, adev_st)                            \
({                                                                  \
    XAF_CHK_RANGE(adev_st, XAF_ADEV_RESET, XAF_ADEV_INIT);          \
    if ((ptr)->adev_state == adev_st)                               \
    {                                                               \
        TRACE(ERROR, _x("Device API Error"));                       \
        return XAF_API_ERR;                                         \
    };                                                              \
})

#define XAF_COMP_STATE_CHK(ptr, comp_st)                            \
({                                                                  \
    XAF_CHK_RANGE(comp_st, XAF_COMP_RESET, XAF_COMP_GET_STATUS);    \
    if (comp_st == XAF_COMP_CREATE)                                 \
    {                                                               \
        if ((ptr)->comp_state != XAF_COMP_RESET)                    \
        {                                                           \
            TRACE(ERROR, _x("Component API Error"));                \
            return XAF_API_ERR;                                     \
        };                                                          \
    }                                                               \
    else {                                                          \
        if ((ptr)->comp_state <= XAF_COMP_RESET)                    \
        {                                                           \
            TRACE(ERROR, _x("Component API Error"));                \
            return XAF_API_ERR;                                     \
        };                                                          \
    }                                                               \
    (ptr)->comp_state = comp_st;                                    \
})

xf_ap_t    *xf_g_ap;
extern xf_dsp_t *xf_g_dsp;

extern int dsp_thread_entry(void  *arg, int wake_value);
extern const int xf_io_ports[XAF_MAX_COMPTYPE][2];

XAF_ERR_CODE xaf_malloc(void **buf_ptr, int size, int id)
{

    XAF_CHK_PTR(buf_ptr);

    *buf_ptr = xf_g_ap->xf_mem_malloc_fxn(size, id);
    XAF_CHK_PTR(*buf_ptr);
    memset(*buf_ptr, 0, size);

    if((UWORD32)*buf_ptr & (XAF_4BYTE_ALIGN - 1))
    {
       TRACE(INFO, _b("Memory alocatation failed : %p is not 4-byte aligned"), *buf_ptr);
       return XAF_PTR_ERROR;
    }

    xf_g_ap->xaf_memory_used += size;

    return XAF_NO_ERROR;
}


XAF_ERR_CODE xaf_get_verinfo(pUWORD8 ver_info[3])
{
    static const xa_api_info_t xa_af_info_tbl = 
    {
        "Copyright (c) 2016 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED", /* Copyright string */
        LIBNAME " (Hostless)",            /* Library name */
        LIBVERSION,         /* Library version */
        LIB_APIVERSION      /* API version */
    };

    XAF_CHK_PTR(ver_info);

    ver_info[0] = (pUWORD8)xa_af_info_tbl.p_lib_name;
    ver_info[1] = (pUWORD8)xa_af_info_tbl.p_lib_ver;
    ver_info[2] = (pUWORD8)xa_af_info_tbl.p_api_ver;

    XAF_CHK_PTR(*ver_info[0]);
    XAF_CHK_PTR(*ver_info[1]);
    XAF_CHK_PTR(*ver_info[2]);

    return XAF_NO_ERROR;
}

static void xaf_comp_response(xf_handle_t *h, xf_user_msg_t *msg)
{
#if 0
    if (msg->opcode == XF_UNREGISTER)
#else
    if (0)    //for Invalid set/get config param ID
#endif
    {
        /* ...component execution failed unexpectedly; die */
        BUG(1, _x("[%p] Abnormal termination"), h);
    }
    else
    {
        /* ...submit response to asynchronous delivery queue */
        xf_response_put(h, msg);
    }
}

static XAF_ERR_CODE xaf_comp_add(xaf_comp_t **pp_comp_chain, xaf_comp_t *p_comp)
{
    XAF_CHK_PTR(pp_comp_chain);
    XAF_CHK_PTR(p_comp);

    p_comp->next   = *pp_comp_chain;
    *pp_comp_chain = p_comp;

    return XAF_NO_ERROR;
}

static XAF_ERR_CODE xaf_comp_post_init_config(xaf_adev_t *p_adev, xaf_comp_t *p_comp, void *p_msg)
{
    xf_proxy_t *p_proxy = &p_adev->proxy; 
    xf_start_msg_t *smsg = p_msg;
    int i;

    p_comp->out_format.sample_rate   = smsg->sample_rate;
    p_comp->out_format.channels      = smsg->channels;
    p_comp->out_format.pcm_width     = smsg->pcm_width;

    TRACE(INFO, _b("Component[%x] Params: f=%d, c=%d, w=%d"), p_comp->handle.id, smsg->sample_rate, smsg->channels, smsg->pcm_width);
    for (i=0; i<p_comp->inp_ports; i++)
    {
        p_comp->out_format.input_length[i]  = smsg->input_length[i];
        TRACE(INFO, _b("Component[%x] Params: i[%d]=%d"), p_comp->handle.id, i, smsg->input_length[i]);
    }
    for (i=0; i<p_comp->out_ports; i++)
    {
        p_comp->out_format.output_length[i] = smsg->output_length[i];
        TRACE(INFO, _b("Component[%x] Params: o[%d]=%d"), p_comp->handle.id, i, smsg->output_length[i]);
    }

    if (p_comp->noutbuf)
    { 
        XF_CHK_API(xf_pool_alloc(p_proxy, p_comp->noutbuf, smsg->output_length[0], XF_POOL_OUTPUT, &p_comp->outpool, XAF_MEM_ID_COMP)); 
    }

    p_comp->init_done   = 1;
    p_comp->comp_status = XAF_INIT_DONE;

    return XAF_NO_ERROR;
}

XAF_ERR_CODE xaf_adev_open(pVOID *pp_adev, WORD32 audio_frmwk_buf_size, WORD32 audio_comp_buf_size, xaf_mem_malloc_fxn_t mem_malloc, xaf_mem_free_fxn_t mem_free)
{
    int ret, size;
    xaf_adev_t *p_adev;
    void * pTmp;
    xf_proxy_t *p_proxy; 

    XAF_CHK_PTR(pp_adev);
    XF_CHK_ERR((xf_g_ap == NULL), XAF_PTR_ERROR);
    XAF_CHK_PTR(mem_malloc);
    XAF_CHK_PTR(mem_free);

    //Memory allocation for adev struct pointer
    size = (sizeof(xaf_adev_t) +(XAF_4BYTE_ALIGN-1));
    pTmp = mem_malloc(size, XAF_MEM_ID_DEV);
    XAF_CHK_PTR(pTmp);
    memset(pTmp, 0, size);

    p_adev = (xaf_adev_t *) (((UWORD32)pTmp + (XAF_4BYTE_ALIGN-1))& ~(XAF_4BYTE_ALIGN-1));
    p_adev->adev_ptr = pTmp;
    *pp_adev = (void *)p_adev;
    
    p_adev->adev_state = XAF_ADEV_INIT;

    // Host side Memory allocation (BSS)
    size = sizeof(xf_ap_t)+(XAF_8BYTE_ALIGN-1);
    p_adev->p_apMem = mem_malloc(size, XAF_MEM_ID_DEV);
    XAF_CHK_PTR(p_adev->p_apMem);
    memset(p_adev->p_apMem, 0, size);

    xf_g_ap = (xf_ap_t *) (((UWORD32)p_adev->p_apMem + (XAF_8BYTE_ALIGN-1))& ~(XAF_8BYTE_ALIGN-1));

    xf_g_ap->xf_mem_malloc_fxn = mem_malloc;
    xf_g_ap->xf_mem_free_fxn = mem_free;

    // DSP Memory allocation (BSS)
    size = sizeof(xf_dsp_t)+(XAF_8BYTE_ALIGN-1);
    ret = xaf_malloc(&(p_adev->p_dspMem), size, XAF_MEM_ID_DEV);
    if(ret != XAF_NO_ERROR)
        return ret;

    xf_g_dsp = (xf_dsp_t *) (((UWORD32)p_adev->p_dspMem + (XAF_8BYTE_ALIGN-1)) & ~(XAF_8BYTE_ALIGN-1));


    size = audio_frmwk_buf_size + (XAF_32BYTE_ALIGN-1); 
    ret = xaf_malloc(&(p_adev->p_apSharedMem), size, XAF_MEM_ID_DEV);
    if(ret != XAF_NO_ERROR)
        return ret;
    xf_g_dsp->xf_ap_shmem_buffer = (UWORD8 *) (((UWORD32)p_adev->p_apSharedMem + (XAF_32BYTE_ALIGN-1)) & ~(XAF_32BYTE_ALIGN-1));
    xf_g_dsp->xf_ap_shmem_buffer_size = audio_frmwk_buf_size;


    size = (audio_comp_buf_size*XF_CFG_CORES_NUM_DSP) + (XAF_32BYTE_ALIGN-1); 
    ret = xaf_malloc(&(p_adev->p_dspLocalBuff), size, XAF_MEM_ID_DEV);
    if(ret != XAF_NO_ERROR)
        return ret;
    xf_g_dsp->xf_dsp_local_buffer = (UWORD8 *) (((UWORD32)p_adev->p_dspLocalBuff + (XAF_32BYTE_ALIGN-1)) & ~(XAF_32BYTE_ALIGN-1));
    xf_g_dsp->xf_dsp_local_buffer_size = audio_comp_buf_size*XF_CFG_CORES_NUM_DSP;
    

#if XF_CFG_CORES_NUM_DSP > 1
    ret = xaf_malloc(&(xf_g_dsp->xf_dsp_shmem_buffer), (audio_comp_buf_size), XAF_MEM_ID_DEV);
    if(ret != XAF_NO_ERROR)
        return ret;    
    xf_g_dsp->xf_dsp_shmem_buffer_size = audio_comp_buf_size;
#endif    // #if XF_CFG_CORES_NUM_DSP > 1

    ret = xos_mutex_create(&(xf_g_ap->g_msgq_lock), 0, 0);
    if (ret != XOS_OK)
        return XAF_XOS_ERROR;
    ret = xos_thread_create(&(xf_g_ap->dsp_thread), 0, dsp_thread_entry, (void *) 1, "DSP-thread", xf_g_ap->dsp_stack, STACK_SIZE, 5, 0, 0);
    if (ret != XOS_OK)
        return XAF_XOS_ERROR;
    
    p_proxy = &p_adev->proxy;
    
    /* ...open DSP proxy - specify "DSP#0" */
    XF_CHK_API(xf_proxy_init(p_proxy, 0));

    /* ...create auxiliary buffers pool for control commands */
    XF_CHK_API(xf_pool_alloc(p_proxy, XAF_AUX_POOL_SIZE, XAF_AUX_POOL_MSG_LENGTH, XF_POOL_AUX, &p_proxy->aux, XAF_MEM_ID_DEV));

    return XAF_NO_ERROR;
}

XAF_ERR_CODE xaf_adev_close(pVOID adev_ptr, xaf_adev_close_flag flag)
{
    xaf_adev_t *p_adev;
    xf_proxy_t *p_proxy;
    xaf_comp_t *p_comp, *p_next;
    WORD32 i, ncomp;

    XF_CHK_ERR((adev_ptr != NULL), XAF_PTR_ERROR);

    p_adev = (xaf_adev_t *)adev_ptr;
    ncomp = p_adev->n_comp;

    XAF_ADEV_STATE_CHK(p_adev, XAF_ADEV_RESET);
    p_adev->adev_state = XAF_ADEV_RESET;

    if(flag == XAF_ADEV_FORCE_CLOSE)
    {
        if((ncomp > 0)&&(p_adev->comp_chain != NULL))
        {
            p_comp = p_adev->comp_chain;
            p_next = p_comp->next;

            for(i = 0; i < ncomp; i++)
            {
                XF_CHK_API(xaf_comp_delete(p_comp));
                
                if(p_next == NULL)
                    break;
                
                p_comp = p_next;
                p_next = p_comp->next;
            }
        }
    }

    XF_CHK_ERR((p_adev->n_comp == 0), XAF_API_ERR);

    p_proxy = &p_adev->proxy;

    if(p_proxy->aux != NULL)
    {
        xf_pool_free(p_proxy->aux, XAF_MEM_ID_DEV);
    }

    TRACE(INFO, _b("dsp buffer usage(bytes): component=%d, framework=%d xaf=%d"),\
            xf_g_dsp->dsp_comp_buf_size_used, xf_g_dsp->dsp_frmwk_buf_size_used, \
            (xf_g_ap->xaf_memory_used + XAF_DEV_AND_AP_STRUCT_MEM_SIZE -  (xf_g_dsp->xf_dsp_local_buffer_size + xf_g_dsp->xf_ap_shmem_buffer_size)));

    if(xf_g_dsp != NULL)
    {
        xf_g_dsp->dsp_thread_exit_flag = 1;
        XF_CHK_API(xos_thread_abort(&(xf_g_ap->dsp_thread), 0));
        XF_CHK_API(xos_thread_join(&(xf_g_ap->dsp_thread), NULL));
        XF_CHK_API(xos_thread_delete(&(xf_g_ap->dsp_thread)));
        xf_proxy_close(p_proxy);

        xf_g_ap->xf_mem_free_fxn(p_adev->p_apSharedMem, XAF_MEM_ID_DEV);
        p_adev->p_apSharedMem = NULL;
#if XF_CFG_CORES_NUM_DSP > 1
        xf_g_ap->xf_mem_free_fxn(p_adev->xf_dsp_shmem_buffer, XAF_MEM_ID_DEV);
        p_adev->xf_dsp_shmem_buffer = NULL;
#endif    // #if XF_CFG_CORES_NUM_DSP > 1
        xf_g_ap->xf_mem_free_fxn(p_adev->p_dspLocalBuff, XAF_MEM_ID_DEV);
        p_adev->p_dspLocalBuff = NULL;
        xf_g_ap->xf_mem_free_fxn(p_adev->p_dspMem, XAF_MEM_ID_DEV);
        p_adev->p_dspMem = NULL;
        xf_g_ap->xf_mem_free_fxn(p_adev->p_apMem, XAF_MEM_ID_DEV);
        p_adev->p_apMem = NULL;
        xf_g_ap->xf_mem_free_fxn(p_adev->adev_ptr, XAF_MEM_ID_DEV);
        p_adev->adev_ptr = NULL;
        xf_g_ap = NULL;
    }

    return XAF_NO_ERROR;
}

XAF_ERR_CODE xaf_comp_create(pVOID adev_ptr, pVOID *pp_comp, xf_id_t comp_id, UWORD32 ninbuf, UWORD32 noutbuf, pVOID pp_inbuf[], xaf_comp_type comp_type)
{
    xaf_adev_t *p_adev;
    xaf_comp_t *p_comp;
    xf_handle_t *p_handle;    
    void * pTmp;
    int ret, size;

    p_adev = (xaf_adev_t *)adev_ptr;

    XAF_CHK_PTR(p_adev);
    XAF_CHK_PTR(pp_comp);
    XAF_CHK_PTR(comp_id);
    if (ninbuf) XAF_CHK_PTR(pp_inbuf);

    XAF_CHK_RANGE(ninbuf, 0, XAF_MAX_INBUFS);
    XAF_CHK_RANGE(noutbuf, 0, 1);
    XAF_CHK_RANGE(comp_type, XAF_DECODER, XAF_MAX_COMPTYPE); 

    XAF_ADEV_STATE_CHK(p_adev, XAF_ADEV_RESET);

    
    //Memory allocation for component struct pointer
    size = (sizeof(xaf_comp_t) + (XAF_4BYTE_ALIGN-1));
    ret = xaf_malloc(&pTmp, size, XAF_MEM_ID_COMP);
    if(ret != XAF_NO_ERROR)
        return ret;
    p_comp = (xaf_comp_t *) (((UWORD32)pTmp + (XAF_4BYTE_ALIGN-1))& ~(XAF_4BYTE_ALIGN-1));

    p_comp->comp_ptr = pTmp;
    *pp_comp = (void*)p_comp;

    p_comp->comp_state = XAF_COMP_CREATE;
    p_handle = &p_comp->handle;

    /* ...create component instance (select core-0) */
    XF_CHK_API(xf_open(&p_adev->proxy, p_handle, comp_id, 0, xaf_comp_response));

    XF_CHK_API(xaf_comp_add(&p_adev->comp_chain, p_comp));

    // Temporary solution in place of component chain handling
    p_comp->p_adev = p_adev;
    p_adev->n_comp += 1;

    p_comp->ninbuf = ninbuf;

    /* ...allocate input buffer */
    if (ninbuf) 
    {
        xf_buffer_t *buf;
        WORD32 i;
        XF_CHK_API(xf_pool_alloc(&p_adev->proxy, ninbuf, XAF_INBUF_SIZE, XF_POOL_INPUT, &p_comp->inpool, XAF_MEM_ID_COMP));
        
        for (i=0; i<ninbuf; i++)
        {
            buf         = xf_buffer_get(p_comp->inpool);
            pp_inbuf[i] = xf_buffer_data(buf); 
        }

    }
    p_comp->noutbuf = noutbuf;

    p_comp->comp_type   = comp_type;
    p_comp->comp_status = XAF_STARTING;

    switch (comp_type)
    {
    case XAF_DECODER:
    case XAF_ENCODER:
    case XAF_PRE_PROC:
    case XAF_POST_PROC:
	case XAF_RENDERER:
	case XAF_CAPTURER:
        p_comp->inp_ports = 1; p_comp->out_ports = 1;
        break;
    case XAF_MIXER:
        p_comp->inp_ports = 4; p_comp->out_ports = 1;
        break;
    case XAF_MIMO_PROC_12 ... (XAF_MAX_COMPTYPE-1):
    	p_comp->inp_ports  = xf_io_ports[comp_type][0];
    	p_comp->out_ports  = xf_io_ports[comp_type][1];
        break;
    default :
        break;
    }

    return XAF_NO_ERROR;
}

XAF_ERR_CODE xaf_comp_delete(pVOID comp_ptr)
{
    xaf_comp_t *p_comp;
    xaf_adev_t *p_adev;

    p_comp = (xaf_comp_t *)comp_ptr;

    XAF_CHK_PTR(p_comp);

    XAF_COMP_STATE_CHK(p_comp, XAF_COMP_RESET);

    // Temporary solution in place of component chain handling
    p_adev = (xaf_adev_t *)(p_comp->p_adev);
    XF_CHK_ERR((p_adev->n_comp > 0), XAF_API_ERR);
    p_adev->n_comp -= 1;

    if (p_comp->inpool)  xf_pool_free(p_comp->inpool, XAF_MEM_ID_COMP);
    if (p_comp->outpool) xf_pool_free(p_comp->outpool, XAF_MEM_ID_COMP);

    xf_close(&p_comp->handle);

    /* ...tbd - remove from chain */

    xf_g_ap->xf_mem_free_fxn(p_comp->comp_ptr, XAF_MEM_ID_COMP);
    p_comp->comp_ptr = NULL;

    return XAF_NO_ERROR;
}

XAF_ERR_CODE xaf_comp_set_config(pVOID comp_ptr, WORD32 num_param, pWORD32 p_param)
{
    xaf_comp_t              *p_comp;
    xf_user_msg_t           rmsg;
    xf_set_param_msg_t     *smsg;
    xf_handle_t            *p_handle;
    WORD32                     i, j;
    
    p_comp = (xaf_comp_t *)comp_ptr;    

    XAF_CHK_PTR(p_comp);
    XAF_CHK_PTR(p_param);
    XAF_CHK_RANGE(num_param, 1, XAF_MAX_CONFIG_PARAMS); 
    
    XAF_COMP_STATE_CHK(p_comp, XAF_COMP_SET_CONFIG);

    p_handle = &p_comp->handle;
    XAF_CHK_PTR(p_handle);

    /* ...set persistent stream characteristics */
    smsg = xf_buffer_data(p_handle->aux);

    j = 0;
    for (i=0; i<num_param; i++)
    {
        smsg->item[i].id    = p_param[j++];
        smsg->item[i].value = p_param[j++];
    }
    
    /* ...pass command to the component */
    /* ...tbd - command goes port 0 always, check if okay */
    XF_CHK_API(xf_command(p_handle, 0, XF_SET_PARAM, smsg, sizeof(xf_set_param_item_t)*num_param));

    /* ...wait until result is delivered */
    XF_CHK_API(xf_response_get(p_handle, &rmsg));

    /* ...make sure response is expected */
    XF_CHK_ERR((rmsg.opcode == XF_SET_PARAM && rmsg.buffer == smsg), XAF_API_ERR);

    return XAF_NO_ERROR;
}


XAF_ERR_CODE xaf_comp_get_config(pVOID comp_ptr, WORD32 num_param, pWORD32 p_param)
{
    xaf_comp_t             *p_comp;
    xf_user_msg_t           rmsg;
    xf_get_param_msg_t     *smsg;
    xf_handle_t            *p_handle;
    WORD32                     i,j;

    p_comp = (xaf_comp_t *)comp_ptr;

    XAF_CHK_PTR(p_comp);
    XAF_CHK_PTR(p_param);
    XAF_CHK_RANGE(num_param, 1, XAF_MAX_CONFIG_PARAMS); 
    
    XAF_COMP_STATE_CHK(p_comp, XAF_COMP_GET_CONFIG);

    p_handle = &p_comp->handle;
    XAF_CHK_PTR(p_handle);

    /* ...set persistent stream characteristics */
    smsg = xf_buffer_data(p_handle->aux);

    for (i=0,j=0; i<num_param; i++,j+=2)
    {
        smsg->c.id[i] = p_param[j];
        p_param[j+1] = 0;
    }
    
    /* ...pass command to the component */
    /* ...tbd - command goes port 0 always, check if okay */
    XF_CHK_API(xf_command(p_handle, 0, XF_GET_PARAM, smsg, XF_GET_PARAM_CMD_LEN(num_param)));

    /* ...wait until result is delivered */
    XF_CHK_API(xf_response_get(p_handle, &rmsg));

    /* ...make sure response is expected */
    XF_CHK_ERR((rmsg.opcode == XF_GET_PARAM && rmsg.buffer == smsg), XAF_API_ERR);

    for (i=0,j=1; i<num_param; i++,j+=2)
    {
        p_param[j] = smsg->r.value[i];
    }

    return XAF_NO_ERROR;
}


XAF_ERR_CODE xaf_comp_get_status(pVOID adev_ptr, pVOID comp_ptr, xaf_comp_status *p_status, pVOID p_info)
{
    xaf_adev_t *p_adev;
    xaf_comp_t *p_comp;
    xf_handle_t *p_handle;

    p_adev = (xaf_adev_t *)adev_ptr;
    p_comp = (xaf_comp_t *)comp_ptr;

    XAF_CHK_PTR(p_comp);
    XAF_CHK_PTR(p_status);
    XAF_CHK_PTR(p_info);

    XAF_COMP_STATE_CHK(p_comp, XAF_COMP_GET_STATUS);

    if (!p_comp->init_done) XAF_CHK_PTR(p_adev);

    p_handle = &p_comp->handle;

    if (p_comp->pending_resp)
    {
        xf_user_msg_t rmsg;
        /* ...wait until result is delivered */
        XF_CHK_API(xf_response_get(p_handle, &rmsg)); 

        if (rmsg.opcode == XF_FILL_THIS_BUFFER) 
        {
            if (rmsg.buffer == p_comp->start_buf)
            {
                XF_CHK_API(xaf_comp_post_init_config(p_adev, p_comp, p_comp->start_buf));
            }
            else 
            {
                WORD32 *p_buf = (WORD32 *) p_info;
                p_buf[0] = (WORD32) rmsg.buffer;
                p_buf[1] = (WORD32) rmsg.length;
                p_comp->comp_status = XAF_OUTPUT_READY;
                p_comp->expect_out_cmd++;

                if (!p_comp->inpool && p_comp->outpool) p_comp->pending_resp--;
            }
        }
        else
        {
            /* ...make sure response is expected */
            XF_CHK_ERR((rmsg.opcode == XF_EMPTY_THIS_BUFFER), XAF_API_ERR);
            WORD32 *p_buf = (WORD32 *) p_info;
            p_buf[0] = (WORD32) rmsg.buffer;
            p_buf[1] = (WORD32) rmsg.length;
            
            p_comp->pending_resp--;
            
            if (p_comp->input_over && rmsg.buffer == NULL) p_comp->comp_status = XAF_EXEC_DONE;
            else p_comp->comp_status = XAF_NEED_INPUT;
        }
    }
    else if ((p_comp->comp_status == XAF_STARTING && p_comp->start_cmd_issued) ||
             (p_comp->comp_status == XAF_INIT_DONE && p_comp->exec_cmd_issued))
    {
        if (p_comp->inpool) p_comp->comp_status = XAF_NEED_INPUT;
    }
    
    *p_status = p_comp->comp_status;
    
    return XAF_NO_ERROR;
}

XAF_ERR_CODE xaf_comp_process(pVOID adev_ptr, pVOID comp_ptr, pVOID p_buf, UWORD32 length, xaf_comp_flag flag)
{
    xaf_adev_t *p_adev;
    xaf_comp_t *p_comp;
    xf_handle_t *p_handle;

    p_adev = (xaf_adev_t *)adev_ptr;
    p_comp = (xaf_comp_t *)comp_ptr;    

    XAF_CHK_PTR(p_comp);

    XAF_COMP_STATE_CHK(p_comp, XAF_COMP_PROCESS);

    if (!p_comp->init_done) XAF_CHK_PTR(p_adev);
    XAF_CHK_RANGE(flag, XAF_START_FLAG, XAF_NEED_OUTPUT_FLAG);
    if (flag == XAF_INPUT_READY_FLAG) XAF_CHK_RANGE(length, 0, XAF_INBUF_SIZE);

    p_handle = &p_comp->handle;
    
    switch (flag)
    {
    case XAF_START_FLAG:
        if (p_comp->start_cmd_issued)
            break;
        else
        {
            p_comp->start_buf = xf_buffer_data(p_handle->aux);
#if 1
	    int out_port_idx=0;
	    for(out_port_idx=0;out_port_idx<p_comp->out_ports;out_port_idx++)
#endif
	    {
              XF_CHK_API(xf_command(p_handle, (p_comp->inp_ports + out_port_idx), XF_FILL_THIS_BUFFER, p_comp->start_buf, 0));
	    }
            p_comp->start_cmd_issued = 1;

            //if(p_comp->comp_type != XAF_DECODER) 
            if (p_comp->ninbuf == 0)    //don't check the response for first components in the chain (i/p side components)
            {
                xf_user_msg_t rmsg;
                int num_out_ports = p_comp->out_ports;
                
                while (num_out_ports)
                {
                    /* ...wait until result is delivered */
                    XF_CHK_API(xf_response_get(p_handle, &rmsg)); 
            
                    /* ...make sure response is expected */
                    XF_CHK_ERR((rmsg.opcode == XF_FILL_THIS_BUFFER && rmsg.buffer == p_comp->start_buf), XAF_API_ERR);
            
                    num_out_ports--;
                }

                XF_CHK_API(xaf_comp_post_init_config(p_adev, p_comp, p_comp->start_buf));
            }            
        }
        break;
    
    case XAF_EXEC_FLAG:
        if (!p_comp->init_done || p_comp->exec_cmd_issued)
            break;
        p_comp->exec_cmd_issued = 1;
        if (p_comp->outpool)
        {
            WORD32 i;
            xf_buffer_t *p_buf;
            void *p_data;

            for (i=0; i<p_comp->noutbuf; i++)
            {
                p_buf = xf_buffer_get(p_comp->outpool);
                p_data = xf_buffer_data(p_buf);

                XF_CHK_API(xf_command(&p_comp->handle, (p_comp->inp_ports), XF_FILL_THIS_BUFFER, p_data, p_comp->out_format.output_length[0]));
            }
            
            if (!p_comp->inpool) p_comp->pending_resp = p_comp->noutbuf;
        }
        break;
 
    case XAF_INPUT_OVER_FLAG:
        if (!p_comp->input_over)
        {
            XF_CHK_API(xf_command(p_handle, 0, XF_EMPTY_THIS_BUFFER, NULL, 0));
            p_comp->input_over = 1;
            p_comp->pending_resp++;
        }
        break;

    case XAF_INPUT_READY_FLAG:
        if (!p_comp->input_over)
        {
            XAF_CHK_PTR(p_buf);
            XF_CHK_API(xf_command(p_handle, 0, XF_EMPTY_THIS_BUFFER, p_buf, length));
            p_comp->pending_resp++;
        }
        break;

    case XAF_NEED_OUTPUT_FLAG:
        if (p_comp->expect_out_cmd)
        {
            XAF_CHK_PTR(p_buf);
            XF_CHK_API(xf_command(p_handle, (p_comp->inp_ports), XF_FILL_THIS_BUFFER, p_buf, length));
            p_comp->expect_out_cmd--;

            if (!p_comp->inpool && p_comp->outpool) p_comp->pending_resp++;
        }
        break;
    }
    
    return XAF_NO_ERROR;
}

XAF_ERR_CODE xaf_connect(pVOID comp_ptr1, pVOID comp_ptr2, WORD32 num_buf)
{
    xaf_comp_t *p_src;
    xaf_comp_t *p_dest;

    p_src = (xaf_comp_t *)comp_ptr1;
    p_dest = (xaf_comp_t *)comp_ptr2;
    
    XAF_CHK_PTR(p_src);
    XAF_CHK_PTR(p_dest);
    XAF_CHK_RANGE(num_buf, 2, 4);    

    XAF_COMP_STATE_CHK(p_src, XAF_COMP_CONNECT);
    XAF_COMP_STATE_CHK(p_dest, XAF_COMP_CONNECT);

    if (!p_src->init_done || p_src->out_routed == p_src->out_ports || p_dest->inp_routed == p_dest->inp_ports)
        return XAF_ROUTING_ERROR;
                   
    XF_CHK_API(xf_route(&p_src->handle, (p_src->inp_ports + p_src->out_routed), &p_dest->handle, (p_dest->inp_routed), num_buf, p_src->out_format.output_length[p_src->out_routed], 8));
    
    p_src->out_routed++;
    p_dest->inp_routed++;

    return XAF_NO_ERROR;
}

XAF_ERR_CODE xaf_disconnect(xaf_comp_t *p_comp)
{
    XAF_CHK_PTR(p_comp);
    
    // Currently we do not have XAF_COMP_DISCONNECT defined
    // To be defined when xaf_disconnected is released in API.
    // XAF_COMP_STATE_CHK(p_comp, XAF_COMP_DISCONNECT);

    /* ...tbd - support for multiple output ports */
    if (!p_comp->init_done || p_comp->out_routed != p_comp->out_ports)
        return XAF_ROUTING_ERROR;

    XF_CHK_API(xf_unroute(&p_comp->handle, (p_comp->inp_ports)));

    return XAF_NO_ERROR;
}

XAF_ERR_CODE xaf_get_mem_stats(pVOID adev_ptr, WORD32 *pmem_info)
{
    xaf_adev_t *p_adev;
    xaf_comp_t *p_comp, *p_next;

    XAF_CHK_PTR(pmem_info);
    XAF_CHK_PTR(adev_ptr);

    p_adev = (xaf_adev_t *)adev_ptr;

    if((p_adev->adev_state < XAF_ADEV_INIT))
    {
        return XAF_API_ERR;
    }

#if 0
    if(p_adev->n_comp)
    {
        WORD32 i, ncomp;
        WORD32 init_done = 0, comp_state_gt_setcfg = 0;

        /* check if init has been done for all the components, return error if not */
        ncomp = p_adev->n_comp;
        p_comp = p_adev->comp_chain;
        p_next = p_comp->next;
        for(i = 0; i < ncomp; i++)
        {
            p_comp->comp_state;
            if(p_comp->comp_status >= XAF_INIT_DONE) /* this fails for renderer */
                init_done++;
            if(p_comp->comp_state > XAF_COMP_SET_CONFIG)
                comp_state_gt_setcfg++;

            if(p_next == NULL)
                break;

            p_comp = p_next;
            p_next = p_comp->next;
        }
        /* renderer doesnt have p_comp->comp_status. It only has p_comp->comp_state. 
         * Based on these states together, decide if it is appropriate to return mem stats */
        if(!(init_done && (comp_state_gt_setcfg >= ncomp)))
                return XAF_API_ERR;
    }
#endif

    /* mem stats info is complete only after components are initialzed. 
     * Recommended to capture stats before device is closed. */
    *((WORD32 *)pmem_info + 0) = xf_g_dsp->dsp_comp_buf_size_used;
    *((WORD32 *)pmem_info + 1) = xf_g_dsp->dsp_frmwk_buf_size_used;
    *((WORD32 *)pmem_info + 2) = xf_g_ap->xaf_memory_used + XAF_DEV_AND_AP_STRUCT_MEM_SIZE -  (xf_g_dsp->xf_dsp_local_buffer_size + xf_g_dsp->xf_ap_shmem_buffer_size);

    return XAF_NO_ERROR;
}
