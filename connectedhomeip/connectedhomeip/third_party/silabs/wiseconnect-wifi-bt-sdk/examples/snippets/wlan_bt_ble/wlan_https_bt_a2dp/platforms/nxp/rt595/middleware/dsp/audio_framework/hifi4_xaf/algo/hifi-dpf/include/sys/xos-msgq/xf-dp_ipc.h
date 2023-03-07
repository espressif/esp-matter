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
 * xf-ipc.h
 *
 * Xtensa IPC mechanism
 *******************************************************************************/

#ifndef __XF_H
#error "xf-ipc.h mustn't be included directly"
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

/* ...system-specific shared memory configuration */
#include "xf-shmem.h"

/*******************************************************************************
 * Macros definitions (should better go to some other header)
 ******************************************************************************/

/*
 *  Execute WAITI 0 (enabling interrupts) only if *(ptr) is zero.
 *  The decision to execute WAITI is done atomically by disabling
 *  interrupts at level 'level' (level must be a constant)
 *  before checking the pointer.  Interrupts are always re-enabled
 *  on exit from this macro.
 */
#define _WAITI_ON_PTR(ptr, level)                       \
do {                                                    \
    int __tmp;                                          \
    __asm__ ("  rsil  %0, " #level " \n"                \
             "  l32i  %0, %1, 0 \n"                     \
             "  bnez  %0, 1f    \n"                     \
             "  waiti 0         \n"                     \
             "1:rsil  %0, 0     \n"                     \
             : "=a" (__tmp) : "a" (ptr) : "memory");    \
} while(0)

/* ...enable gdbstub */
//#define XF_CFG_USE_GDBSTUB              0

#ifndef XF_CFG_USE_GDBSTUB
/* ...maybe "level" should be hidden here - we always magically set 15 */
#define WAITI_ON_PTR(ptr, level)    _WAITI_ON_PTR(ptr, level)
#else
/* ...if debugger is enabled, do polling instead of waiting */
static inline void WAITI_ON_PTR(volatile UWORD32 *ptr, UWORD32 level)
{
    extern void poll_debug_ring(void);
    
    while (*ptr == 0)
    {
        /* ...should be called with interrupts disabled - tbd */
        poll_debug_ring();
    }
}
#endif

/*******************************************************************************
 * Remote IPI interrupt mode
 ******************************************************************************/

/* ...enable/disable IPI interrupt */
static inline void xf_ipi_enable(UWORD32 core, int on)
{
#ifdef __TOOLS_RF2__
    if (on)
        xos_enable_ints(1 << XF_PROXY_IPI_NUM(core));
    else
        xos_disable_ints(1 << XF_PROXY_IPI_NUM(core));
#else   // #ifdef __TOOLS_RF2__
    if (on)
        xos_interrupt_enable(XF_PROXY_IPI_NUM(core));
    else
        xos_interrupt_disable(XF_PROXY_IPI_NUM(core));
#endif  // #ifdef __TOOLS_RF2__
}

/* ...wait in low-power mode for interrupt arrival if "ptr" is 0 */
static inline void xf_ipi_wait(UWORD32 core)
{
    xf_core_ro_data_t  *ro = XF_CORE_RO_DATA(core);
    XosMsgQueue *cmd_msgq = ro->ipc.cmd_msgq;

#if 1
    XosEvent *msgq_event = ro->ipc.msgq_event;
    xos_event_wait_all(msgq_event, CMD_MSGQ_READY);
    xos_event_clear(msgq_event, CMD_MSGQ_READY);
#else
    xf_proxy_message_t msg;
    
    /* ...get message from xos message queue - blocking call */
    xos_msgq_get(cmd_msgq, (UWORD32 *) &msg);

    /* ...put message back in xos message queue - wait over */
    xos_msgq_put(cmd_msgq, (UWORD32 *) &msg);
#endif

    /* ...reset waiting object upon leaving */
    ro->ipc.wait = 0;
}

/* ...complete IPI waiting (may be called from any context on local core) */
static inline void xf_ipi_resume(UWORD32 core)
{
    xf_core_ro_data_t  *ro = XF_CORE_RO_DATA(core);
    
    /* ...single instruction is written atomically; no need to mask interrupts */
    ro->ipc.wait = 1;
}
/*complete IPI waiting and resume dsp thread */
static inline void xf_ipi_resume_dsp(UWORD32 core)
{
    xf_core_ro_data_t  *ro = XF_CORE_RO_DATA(core);
     
    XosEvent *msgq_event = ro->ipc.msgq_event;
	xf_ipi_resume(core);
    xos_event_set(msgq_event, CMD_MSGQ_READY);
   
  
}

/* ...assert IPI interrupt on remote core - board-specific */
static inline void xf_ipi_assert(UWORD32 core)
{
    XF_PROXY_NOTIFY_PEER(core);
}

/* ...initialize IPI subsystem */
static inline int xf_ipi_init(UWORD32 core)
{
    int ret;

    xf_core_ro_data_t  *ro = XF_CORE_RO_DATA(core);
    extern void (* const xf_ipi_handlers[])(void *arg);
    
    /* ...reset IPC data - no interrupt yet */
    ro->ipc.wait = 0;

    /* ...reset xos variables */
    ro->ipc.cmd_msgq   = NULL;
    ro->ipc.resp_msgq  = NULL;
    ro->ipc.msgq_event = NULL;

    /* ...install interrupt handler */
    ret = xos_register_interrupt_handler(XF_PROXY_IPI_NUM(core), xf_ipi_handlers[core], NULL);
    if (ret != XOS_OK)
        return ret;

    return 0;
}

/*******************************************************************************
 * Shared memory operations
 ******************************************************************************/

/* ...translate buffer address to shared proxy address */
static inline UWORD32 xf_ipc_b2a(UWORD32 core, void *b)
{
    return (UWORD32) b;
}

/* ...translate shared proxy address to local pointer */
static inline void * xf_ipc_a2b(UWORD32 core, UWORD32 address)
{
    return (void *) address;
}

/* ...component association with remote IPC client */
static inline void xf_ipc_component_addref(UWORD32 session)
{
}

/* ...delete record about component association with remote IPC client */
static inline void xf_ipc_component_rmref(UWORD32 id)
{
}

/* ...system-specific IPC layer initialization */
extern int xf_ipc_init(UWORD32 core);

/*******************************************************************************
 * Mutex definitions
 ******************************************************************************/

/* ...export shared memory access macros */
#define MUTEX_SHARED_READ(core)         \
    ({  xf_core_ro_data_t  *__ro = XF_CORE_RO_DATA(core); __ro->lock[0]; })

#define MUTEX_SHARED_WRITE(core, val)   \
    ({  xf_core_ro_data_t  *__ro = XF_CORE_RO_DATA(core); __ro->lock[0] = (val); })

/* ...include library header */
#ifdef __TOOLS_RF2__
#include <xtensa/xos/xos_mutex.h>
#else   // #ifdef __TOOLS_RF2__
#include <xtensa/xos_mutex.h>
#endif  // #ifdef __TOOLS_RF2__

#if XF_CFG_CORES_NUM > 1
/* ...rename API functions */
static inline void xf_mutex_lock(UWORD32 core)
{
    xos_mutex_lock(core);
}

static inline void xf_mutex_unlock(UWORD32 core)
{
    xos_mutex_unlock(core);
}

#else
/* ...for single-core setting no locking is actually needed */
static inline void xf_mutex_lock(UWORD32 core)
{
}

static inline void xf_mutex_unlock(UWORD32 core)
{
}

#endif  /* XF_CFG_CORES_NUM > 1 */
