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
 * xf-runtime.h
 *
 * Platform-specific runtime data definitions
 *******************************************************************************/

#ifndef __XF_H
#error "xf-runtime.h mustn't be included directly"
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

/* ...platform HAL layer */
#include "xf-dp_hal.h"

/*******************************************************************************
 * Mutex (opaque) data definition
 ******************************************************************************/

typedef volatile UWORD32 xf_mutex_t[XF_CFG_CORES_NUM > 1 ? 1 : 0];

/*******************************************************************************
 * Local/remote/ISR IPC (opaque) data
 ******************************************************************************/

typedef struct xf_ipc_handle
{
    /* ...variable we are waiting on */
    UWORD32                 wait;

    void                *cmd_msgq;
    void                *resp_msgq;
    void                *msgq_event;
    
}   xf_ipc_handle_t;    

/*******************************************************************************
 * IPC events
 ******************************************************************************/

/* ...core resumption flag */
#define XF_IPC_EVENT_CORE_ASSERT        (1 << 0)

/* ...core waiting flag */
#define XF_IPC_EVENT_CORE_WAIT          (1 << 1)

/* ...shared memory assertion flag */
#define XF_IPC_EVENT_SHMEM_ASSERT       (1 << 2)

/* ...shared memory waiting flag */
#define XF_IPC_EVENT_SHMEM_WAIT         (1 << 3)

/* ...disabled interrupts status */
#define XF_IPC_EVENT_ISR_OFF            (1 << 4)

/*******************************************************************************
 * Shared memory interface (opaque) data
 ******************************************************************************/

typedef void * xf_shmem_handle_t;
