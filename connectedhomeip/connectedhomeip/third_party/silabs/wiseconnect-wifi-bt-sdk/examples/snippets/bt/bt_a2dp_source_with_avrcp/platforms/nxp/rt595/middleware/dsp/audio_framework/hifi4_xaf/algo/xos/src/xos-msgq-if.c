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
/*******************************************************************************
 * xos-msgq.c
 *
 * XOS Message Queue Interface
 *******************************************************************************/


/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xos-msgq-if.h"
#include "xf.h"
#include "xaf-api.h"

/*******************************************************************************
 * Global Definitions
 ******************************************************************************/

extern xf_ap_t *xf_g_ap;
extern XAF_ERR_CODE xaf_malloc(void **buf_ptr, int size, int id); 

/*******************************************************************************
 * IPC Message Queue APIs
 ******************************************************************************/

int ipc_msgq_init(void **cmdq, void **respq, void **msgq_event)
{
    int ret;

    *cmdq = *respq = *msgq_event = NULL;

    ret = xos_mutex_lock(&(xf_g_ap->g_msgq_lock));
    if (ret != XOS_OK)
        return XAF_XOS_ERROR;
    
    if (xf_g_ap->g_ipc_msgq.init_done) 
    {
        *cmdq       = (void *) xf_g_ap->g_ipc_msgq.cmd_msgq;
        *respq      = (void *) xf_g_ap->g_ipc_msgq.resp_msgq;
        *msgq_event = (void *) xf_g_ap->g_ipc_msgq.msgq_event;
        
        return 0;
    }

    ret = xaf_malloc((void**)&(xf_g_ap->g_ipc_msgq.cmd_msgq), (XOS_MSGQ_SIZE(SEND_MSGQ_ENTRIES, sizeof(xf_proxy_msg_t))), XAF_MEM_ID_DEV);
    if(ret != XAF_NO_ERROR)
        return ret;

    /* ...allocation mustn't fail on host */
    BUG(xf_g_ap->g_ipc_msgq.cmd_msgq == NULL, _x("Out-of-memeory"));

    XF_CHK_API(xos_msgq_create(xf_g_ap->g_ipc_msgq.cmd_msgq, SEND_MSGQ_ENTRIES, sizeof(xf_proxy_msg_t), 0));

    ret = xaf_malloc((void**)&(xf_g_ap->g_ipc_msgq.resp_msgq), (XOS_MSGQ_SIZE(RECV_MSGQ_ENTRIES, sizeof(xf_proxy_msg_t))), XAF_MEM_ID_DEV);
    if(ret != XAF_NO_ERROR)
        return ret;

    /* ...allocation mustn't fail on host */
    BUG(xf_g_ap->g_ipc_msgq.resp_msgq == NULL, _x("Out-of-memeory"));

    XF_CHK_API(xos_msgq_create(xf_g_ap->g_ipc_msgq.resp_msgq, RECV_MSGQ_ENTRIES, sizeof(xf_proxy_msg_t), 0));

    ret = xaf_malloc((void**)&(xf_g_ap->g_ipc_msgq.msgq_event), sizeof(XosEvent), XAF_MEM_ID_DEV);
    if(ret != XAF_NO_ERROR)
        return ret;

    /* ...allocation mustn't fail on host */
    BUG(xf_g_ap->g_ipc_msgq.msgq_event == NULL, _x("Out-of-memeory"));
    
    /* ...create event object with 16 bits */
    xos_event_create(xf_g_ap->g_ipc_msgq.msgq_event, 0x0000ffff, 0);

    *cmdq       = (void *) xf_g_ap->g_ipc_msgq.cmd_msgq;
    *respq      = (void *) xf_g_ap->g_ipc_msgq.resp_msgq;
    *msgq_event = (void *) xf_g_ap->g_ipc_msgq.msgq_event;

    xf_g_ap->g_ipc_msgq.init_done = 1;

    ret = xos_mutex_unlock(&(xf_g_ap->g_msgq_lock));
    if (ret != XOS_OK)
        return XAF_XOS_ERROR;

    return 0;
}

int ipc_msgq_delete(void **cmdq, void **respq, void **msgq_event)
{
    int ret;

    ret = xos_msgq_delete(xf_g_ap->g_ipc_msgq.cmd_msgq);
    if (ret != XOS_OK)
        return XAF_XOS_ERROR;
    ret = xos_msgq_delete(xf_g_ap->g_ipc_msgq.resp_msgq);
    if (ret != XOS_OK)
        return XAF_XOS_ERROR;
    xos_event_delete(xf_g_ap->g_ipc_msgq.msgq_event);

    xf_g_ap->xf_mem_free_fxn(xf_g_ap->g_ipc_msgq.cmd_msgq, XAF_MEM_ID_DEV);
    xf_g_ap->xf_mem_free_fxn(xf_g_ap->g_ipc_msgq.resp_msgq, XAF_MEM_ID_DEV);
    xf_g_ap->xf_mem_free_fxn(xf_g_ap->g_ipc_msgq.msgq_event, XAF_MEM_ID_DEV);

    xf_g_ap->g_ipc_msgq.cmd_msgq   = NULL;
    xf_g_ap->g_ipc_msgq.resp_msgq  = NULL;
    xf_g_ap->g_ipc_msgq.msgq_event = NULL;
    xf_g_ap->g_ipc_msgq.init_done  = 0;

    *cmdq = *respq = *msgq_event = NULL;

    return 0;
}

