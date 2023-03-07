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
 * xf-main.c
 *
 * DSP processing framework main file
 ******************************************************************************/

#define MODULE_TAG                      MAIN

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xf-dp.h"

/*******************************************************************************
 * Tracing configuration
 ******************************************************************************/

TRACE_TAG(INFO, 1);

/*******************************************************************************
 * Global data definition
 ******************************************************************************/

xf_dsp_t *xf_g_dsp;

/*******************************************************************************
 * Timer interrupt - tbd
 ******************************************************************************/

/*******************************************************************************
 * IPC layer initialization
 ******************************************************************************/

extern int ipc_msgq_init(void **cmdq, void **respq, void **msgq_event);

/* ...system-specific IPC layer initialization */
int xf_ipc_init(UWORD32 core)
{
    xf_core_data_t  *cd = XF_CORE_DATA(core);
    xf_core_ro_data_t  *ro = XF_CORE_RO_DATA(core);

    xf_shmem_data_t *shmem = (xf_shmem_data_t *) xf_g_dsp->xf_ap_shmem_buffer;
    //xf_shmem_data_t *shmem = (xf_shmem_data_t *) malloc(XF_CFG_REMOTE_IPC_POOL_SIZE + 4095);
    //shmem = (xf_shmem_data_t *)((int) (shmem+4095) & 0xfffff000);

    /* ...initialize pointer to shared memory */
    cd->shmem = (xf_shmem_handle_t *)shmem;

    /* ...global memory pool initialization */

    XF_CHK_API(xf_mm_init(&cd->shared_pool, (shmem->buffer), (xf_g_dsp->xf_ap_shmem_buffer_size-(sizeof(xf_shmem_data_t)-XF_CFG_REMOTE_IPC_POOL_SIZE))));

    /* ...open xos-msgq interface */
    XF_CHK_API(ipc_msgq_init(&ro->ipc.cmd_msgq, &ro->ipc.resp_msgq, &ro->ipc.msgq_event));

    return 0;
}

/*******************************************************************************
 * Core executive loop
 ******************************************************************************/

static void xf_core_loop(UWORD32 core)
{
    /* ...initialize internal core structures */
    xf_core_init(core);
    
    for(;;)
    {
        /* ...wait in a low-power mode until event is triggered */
        xf_ipi_wait(core);
        
        /* ...service core event */
        xf_core_service(core);
        
        /* ...check exit flag */
        if (xf_g_dsp->dsp_thread_exit_flag)
            break;
    }
}

/*******************************************************************************
 * DSP entry point
 ******************************************************************************/

int dsp_thread_entry(void *arg, int unused)
{
#if XF_CFG_CORES_NUM > 1
    UWORD32     i;
#endif
    UWORD32     size;
    
    /* ...validation of parameters shared with ARM */
    size = XF_CFG_CORES_NUM;
    TRACE(INFO, _b("XF_CFG_CORES_NUM_DSP: %d %d"),              XF_CFG_CORES_NUM_DSP, size);
    size =  offset_of(xf_dsp_t, xf_dsp_local_pool) - offset_of(xf_dsp_t, xf_core_data);
    TRACE(INFO, _b("XF_DSP_OBJ_SIZE_CORE_DATA: %d %d"),         XF_DSP_OBJ_SIZE_CORE_DATA, size);
    size =  offset_of(xf_dsp_t, xf_core_ro_data) - offset_of(xf_dsp_t, xf_dsp_local_pool);
    TRACE(INFO, _b("XF_DSP_OBJ_SIZE_DSP_LOCAL_POOL: %d %d"),    XF_DSP_OBJ_SIZE_DSP_LOCAL_POOL, size);
    size =  offset_of(xf_dsp_t, xf_core_rw_data) - offset_of(xf_dsp_t, xf_core_ro_data);
    TRACE(INFO, _b("XF_DSP_OBJ_SIZE_CORE_RO_DATA: %d %d"),      XF_DSP_OBJ_SIZE_CORE_RO_DATA, size);
    size =  offset_of(xf_dsp_t, xf_ap_shmem_buffer) - offset_of(xf_dsp_t, xf_core_rw_data);
    TRACE(INFO, _b("XF_DSP_OBJ_SIZE_CORE_RW_DATA: %d %d"),      XF_DSP_OBJ_SIZE_CORE_RW_DATA, size);
    size = offset_of(xf_dsp_t, xf_ap_shmem_buffer) - offset_of(xf_dsp_t, xf_core_data);
    size -= (XF_DSP_OBJ_SIZE_CORE_DATA + XF_DSP_OBJ_SIZE_DSP_LOCAL_POOL + XF_DSP_OBJ_SIZE_CORE_RO_DATA + XF_DSP_OBJ_SIZE_CORE_RW_DATA);
    XF_CHK_ERR(!size, size);
    
    /* ...reset ro/rw core data - tbd */
    memset(xf_g_dsp->xf_core_rw_data, 0, sizeof(xf_g_dsp->xf_core_rw_data));
    memset(xf_g_dsp->xf_core_ro_data, 0, sizeof(xf_g_dsp->xf_core_ro_data));

    //TRACE_INIT("Xtensa Audio DSP Codec Server");
    TRACE(INFO, _b("Xtensa Audio DSP Codec Server"));

    /* ...initialize board */
    xf_board_init();

    /* ...global framework data initialization */
    xf_global_init();


#if XF_CFG_CORES_NUM > 1
    /* ...DSP shared memory pool initialization */
    XF_CHK_API(xf_mm_init(&xf_dsp_shmem_pool, xf_dsp_shmem_buffer, XF_CFG_LOCAL_IPC_POOL_SIZE));
#endif    // #if XF_CFG_CORES_NUM > 1

    /* ...initialize per-core memory loop */

    XF_CHK_API(xf_mm_init(&(xf_g_dsp->xf_core_data[0]).local_pool, xf_g_dsp->xf_dsp_local_buffer, xf_g_dsp->xf_dsp_local_buffer_size));

#if XF_CFG_CORES_NUM > 1    
    /* ...bring up all cores */
    for (i = 1; i < XF_CFG_CORES_NUM; i++)
    {
        /* ...wake-up secondary core somehow and make it execute xf_core_loop */
        xf_core_secondary_startup(i, xf_core_loop, i);
        
    }
#endif

    /* ...enter execution loop on master core #0 */
    xf_core_loop(0);

    /* ...not reachable */
    return 0;
}
