/***************************************************************************//**
 * @file
 * @brief Kernel Trace Events
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _OS_TRACE_H_
#define  _OS_TRACE_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 *
 * Note(s) : (1) The header file os_trace_events.h is the interface between Micrium OS Kernel
 *               and your trace recorder of choice.
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <os_cfg.h>

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include  <sl_component_catalog.h>
#endif

#if defined(SL_CATALOG_SYSTEMVIEW_TRACE_PRESENT) || (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN == DEF_ENABLED))
#include  "os_trace_events.h"                                   // See Note #1.
#endif

/********************************************************************************************************
 *                                   DEFAULT TRACE EVENT MACROS (EMPTY)
 *******************************************************************************************************/

#ifndef  OS_TRACE_INIT
#define  OS_TRACE_INIT()
#endif
#ifndef  OS_TRACE_START
#define  OS_TRACE_START()
#endif
#ifndef  OS_TRACE_STOP
#define  OS_TRACE_STOP()
#endif

#ifndef  OS_TRACE_ISR_ENTER
#define  OS_TRACE_ISR_ENTER()
#endif
#ifndef  OS_TRACE_ISR_EXIT
#define  OS_TRACE_ISR_EXIT()
#endif

#ifndef  OS_TRACE_TICK_INCREMENT
#define  OS_TRACE_TICK_INCREMENT(OSTickCtr)
#endif

#ifndef  OS_TRACE_TASK_CREATE
#define  OS_TRACE_TASK_CREATE(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_DEL
#define  OS_TRACE_TASK_DEL(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_READY
#define  OS_TRACE_TASK_READY(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_SWITCHED_IN
#define  OS_TRACE_TASK_SWITCHED_IN(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_DLY
#define  OS_TRACE_TASK_DLY(dly_ticks)
#endif

#ifndef  OS_TRACE_TASK_SUSPEND
#define  OS_TRACE_TASK_SUSPEND(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_SUSPENDED
#define  OS_TRACE_TASK_SUSPENDED(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_RESUME
#define  OS_TRACE_TASK_RESUME(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_PREEMPT
#define  OS_TRACE_TASK_PREEMPT(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_PRIO_CHANGE
#define  OS_TRACE_TASK_PRIO_CHANGE(p_tcb, prio)
#endif

#ifndef  OS_TRACE_ISR_REGISTER
#define  OS_TRACE_ISR_REGISTER(isr_id, isr_name, isr_prio)
#endif

#ifndef  OS_TRACE_ISR_BEGIN
#define  OS_TRACE_ISR_BEGIN(isr_id)
#endif

#ifndef  OS_TRACE_ISR_END
#define  OS_TRACE_ISR_END()
#endif

#ifndef  OS_TRACE_ON_IDLE_ENTER
#define  OS_TRACE_ON_IDLE_ENTER()
#endif

#ifndef  OS_TRACE_TASK_MSG_Q_CREATE
#define  OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name)
#endif

#ifndef  OS_TRACE_TASK_MSG_Q_POST
#define  OS_TRACE_TASK_MSG_Q_POST(p_msg_q)
#endif

#ifndef  OS_TRACE_TASK_MSG_Q_POST_FAILED
#define  OS_TRACE_TASK_MSG_Q_POST_FAILED(p_msg_q)
#endif

#ifndef  OS_TRACE_TASK_MSG_Q_PEND
#define  OS_TRACE_TASK_MSG_Q_PEND(p_msg_q)
#endif

#ifndef  OS_TRACE_TASK_MSG_Q_PEND_FAILED
#define  OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q)
#endif

#ifndef  OS_TRACE_TASK_MSG_Q_PEND_BLOCK
#define  OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q)
#endif

#ifndef  OS_TRACE_TASK_SEM_CREATE
#define  OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name)
#endif

#ifndef  OS_TRACE_TASK_SEM_POST
#define  OS_TRACE_TASK_SEM_POST(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_SEM_POST_FAILED
#define  OS_TRACE_TASK_SEM_POST_FAILED(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_SEM_PEND
#define  OS_TRACE_TASK_SEM_PEND(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_SEM_PEND_FAILED
#define  OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_SEM_PEND_BLOCK
#define  OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb)
#endif

#ifndef  OS_TRACE_MUTEX_CREATE
#define  OS_TRACE_MUTEX_CREATE(p_mutex, p_name)
#endif

#ifndef  OS_TRACE_MUTEX_DEL
#define  OS_TRACE_MUTEX_DEL(p_mutex)
#endif

#ifndef  OS_TRACE_MUTEX_POST
#define  OS_TRACE_MUTEX_POST(p_mutex)
#endif

#ifndef  OS_TRACE_MUTEX_POST_FAILED
#define  OS_TRACE_MUTEX_POST_FAILED(p_mutex)
#endif

#ifndef  OS_TRACE_MUTEX_PEND
#define  OS_TRACE_MUTEX_PEND(p_mutex)
#endif

#ifndef  OS_TRACE_MUTEX_PEND_FAILED
#define  OS_TRACE_MUTEX_PEND_FAILED(p_mutex)
#endif

#ifndef  OS_TRACE_MUTEX_PEND_BLOCK
#define  OS_TRACE_MUTEX_PEND_BLOCK(p_mutex)
#endif

#ifndef  OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define  OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio)
#endif

#ifndef  OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT
#define  OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio)
#endif

#ifndef  OS_TRACE_SEM_CREATE
#define  OS_TRACE_SEM_CREATE(p_sem, p_name)
#endif

#ifndef  OS_TRACE_SEM_DEL
#define  OS_TRACE_SEM_DEL(p_sem)
#endif

#ifndef  OS_TRACE_SEM_POST
#define  OS_TRACE_SEM_POST(p_sem)
#endif

#ifndef  OS_TRACE_SEM_POST_FAILED
#define  OS_TRACE_SEM_POST_FAILED(p_sem)
#endif

#ifndef  OS_TRACE_SEM_PEND
#define  OS_TRACE_SEM_PEND(p_sem)
#endif

#ifndef  OS_TRACE_SEM_PEND_FAILED
#define  OS_TRACE_SEM_PEND_FAILED(p_sem)
#endif

#ifndef  OS_TRACE_SEM_PEND_BLOCK
#define  OS_TRACE_SEM_PEND_BLOCK(p_sem)
#endif

#ifndef  OS_TRACE_Q_CREATE
#define  OS_TRACE_Q_CREATE(p_q, p_name)
#endif

#ifndef  OS_TRACE_Q_DEL
#define  OS_TRACE_Q_DEL(p_q)
#endif

#ifndef  OS_TRACE_Q_POST
#define  OS_TRACE_Q_POST(p_q)
#endif

#ifndef  OS_TRACE_Q_POST_FAILED
#define  OS_TRACE_Q_POST_FAILED(p_q)
#endif

#ifndef  OS_TRACE_Q_PEND
#define  OS_TRACE_Q_PEND(p_q)
#endif

#ifndef  OS_TRACE_Q_PEND_FAILED
#define  OS_TRACE_Q_PEND_FAILED(p_q)
#endif

#ifndef  OS_TRACE_Q_PEND_BLOCK
#define  OS_TRACE_Q_PEND_BLOCK(p_q)
#endif

#ifndef  OS_TRACE_FLAG_CREATE
#define  OS_TRACE_FLAG_CREATE(p_grp, p_name)
#endif

#ifndef  OS_TRACE_FLAG_DEL
#define  OS_TRACE_FLAG_DEL(p_grp)
#endif

#ifndef  OS_TRACE_FLAG_POST
#define  OS_TRACE_FLAG_POST(p_grp)
#endif

#ifndef  OS_TRACE_FLAG_POST_FAILED
#define  OS_TRACE_FLAG_POST_FAILED(p_grp)
#endif

#ifndef  OS_TRACE_FLAG_PEND
#define  OS_TRACE_FLAG_PEND(p_grp)
#endif

#ifndef  OS_TRACE_FLAG_PEND_FAILED
#define  OS_TRACE_FLAG_PEND_FAILED(p_grp)
#endif

#ifndef  OS_TRACE_FLAG_PEND_BLOCK
#define  OS_TRACE_FLAG_PEND_BLOCK(p_grp)
#endif

#ifndef  OS_TRACE_MEM_CREATE
#define  OS_TRACE_MEM_CREATE(p_mem, p_name)
#endif

#ifndef  OS_TRACE_MEM_PUT
#define  OS_TRACE_MEM_PUT(p_mem)
#endif

#ifndef  OS_TRACE_MEM_PUT_FAILED
#define  OS_TRACE_MEM_PUT_FAILED(p_mem)
#endif

#ifndef  OS_TRACE_MEM_GET
#define  OS_TRACE_MEM_GET(p_mem)
#endif

#ifndef  OS_TRACE_MEM_GET_FAILED
#define  OS_TRACE_MEM_GET_FAILED(p_mem)
#endif

#ifndef  OS_TRACE_MUTEX_DEL_ENTER
#define  OS_TRACE_MUTEX_DEL_ENTER(p_mutex, opt)
#endif

#ifndef  OS_TRACE_MUTEX_POST_ENTER
#define  OS_TRACE_MUTEX_POST_ENTER(p_mutex, opt)
#endif

#ifndef  OS_TRACE_MUTEX_PEND_ENTER
#define  OS_TRACE_MUTEX_PEND_ENTER(p_mutex, timeout, opt, p_ts)
#endif

#ifndef  OS_TRACE_TASK_MSG_Q_POST_ENTER
#define  OS_TRACE_TASK_MSG_Q_POST_ENTER(p_msg_q, p_void, msg_size, opt)
#endif

#ifndef  OS_TRACE_TASK_MSG_Q_PEND_ENTER
#define  OS_TRACE_TASK_MSG_Q_PEND_ENTER(p_msg_q, timeout, opt, p_msg_size, p_ts)
#endif

#ifndef  OS_TRACE_TASK_SEM_POST_ENTER
#define  OS_TRACE_TASK_SEM_POST_ENTER(p_tcb, opt)
#endif

#ifndef  OS_TRACE_TASK_SEM_PEND_ENTER
#define  OS_TRACE_TASK_SEM_PEND_ENTER(p_tcb, timeout, opt, p_ts)
#endif

#ifndef  OS_TRACE_TASK_RESUME_ENTER
#define  OS_TRACE_TASK_RESUME_ENTER(p_tcb)
#endif

#ifndef  OS_TRACE_TASK_SUSPEND_ENTER
#define  OS_TRACE_TASK_SUSPEND_ENTER(p_tcb)
#endif

#ifndef  OS_TRACE_SEM_DEL_ENTER
#define  OS_TRACE_SEM_DEL_ENTER(p_sem, opt)
#endif

#ifndef  OS_TRACE_SEM_POST_ENTER
#define  OS_TRACE_SEM_POST_ENTER(p_sem, opt)
#endif

#ifndef  OS_TRACE_SEM_PEND_ENTER
#define  OS_TRACE_SEM_PEND_ENTER(p_sem, timeout, opt, p_ts)
#endif

#ifndef  OS_TRACE_Q_DEL_ENTER
#define  OS_TRACE_Q_DEL_ENTER(p_q, opt)
#endif

#ifndef  OS_TRACE_Q_POST_ENTER
#define  OS_TRACE_Q_POST_ENTER(p_q, p_void, msg_size, opt)
#endif

#ifndef  OS_TRACE_Q_PEND_ENTER
#define  OS_TRACE_Q_PEND_ENTER(p_q, timeout, opt, p_msg_size, p_ts)
#endif

#ifndef  OS_TRACE_FLAG_DEL_ENTER
#define  OS_TRACE_FLAG_DEL_ENTER(p_grp, opt)
#endif

#ifndef  OS_TRACE_FLAG_POST_ENTER
#define  OS_TRACE_FLAG_POST_ENTER(p_grp, flags, opt)
#endif

#ifndef  OS_TRACE_FLAG_PEND_ENTER
#define  OS_TRACE_FLAG_PEND_ENTER(p_grp, flags, timeout, opt, p_ts)
#endif

#ifndef  OS_TRACE_MEM_PUT_ENTER
#define  OS_TRACE_MEM_PUT_ENTER(p_mem, p_blk)
#endif

#ifndef  OS_TRACE_MEM_GET_ENTER
#define  OS_TRACE_MEM_GET_ENTER(p_mem)
#endif

#ifndef  OS_TRACE_MUTEX_DEL_EXIT
#define  OS_TRACE_MUTEX_DEL_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_MUTEX_POST_EXIT
#define  OS_TRACE_MUTEX_POST_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_MUTEX_PEND_EXIT
#define  OS_TRACE_MUTEX_PEND_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_TASK_MSG_Q_POST_EXIT
#define  OS_TRACE_TASK_MSG_Q_POST_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_TASK_MSG_Q_PEND_EXIT
#define  OS_TRACE_TASK_MSG_Q_PEND_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_TASK_SEM_POST_EXIT
#define  OS_TRACE_TASK_SEM_POST_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_TASK_SEM_PEND_EXIT
#define  OS_TRACE_TASK_SEM_PEND_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_TASK_RESUME_EXIT
#define  OS_TRACE_TASK_RESUME_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_TASK_SUSPEND_EXIT
#define  OS_TRACE_TASK_SUSPEND_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_SEM_DEL_EXIT
#define  OS_TRACE_SEM_DEL_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_SEM_POST_EXIT
#define  OS_TRACE_SEM_POST_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_SEM_PEND_EXIT
#define  OS_TRACE_SEM_PEND_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_Q_DEL_EXIT
#define  OS_TRACE_Q_DEL_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_Q_POST_EXIT
#define  OS_TRACE_Q_POST_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_Q_PEND_EXIT
#define  OS_TRACE_Q_PEND_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_FLAG_DEL_EXIT
#define  OS_TRACE_FLAG_DEL_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_FLAG_POST_EXIT
#define  OS_TRACE_FLAG_POST_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_FLAG_PEND_EXIT
#define  OS_TRACE_FLAG_PEND_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_MEM_PUT_EXIT
#define  OS_TRACE_MEM_PUT_EXIT(RetVal)
#endif

#ifndef  OS_TRACE_MEM_GET_EXIT
#define  OS_TRACE_MEM_GET_EXIT(RetVal)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of Kernel trace events module include.
