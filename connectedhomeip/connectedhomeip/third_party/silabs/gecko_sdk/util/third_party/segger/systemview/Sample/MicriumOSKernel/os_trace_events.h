/***************************************************************************//**
 * @brief Kernel Trace events
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software. 
 *
 ******************************************************************************/
/*
*********************************************************************************************************
*
*                                         KERNEL TRACE EVENTS
*
* File : os_trace_events.h
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                                MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _OS_TRACE_EVENTS_H_
#define  _OS_TRACE_EVENTS_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <SEGGER_SYSVIEW.h>
#include  <os_cfg_trace.h>
#include  <kernel/include/os.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          TRACE EVENT MACROS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           RECORDER CONTROL
*********************************************************************************************************
*/

// DEPRECATION NOTICE: OS_TRACE_INIT() will be deprecated and replaced by SEGGER_SYSVIEW_Conf().

#define  OS_TRACE_INIT()                             SEGGER_SYSVIEW_Conf()
#define  OS_TRACE_START()                            SEGGER_SYSVIEW_Start()
#define  OS_TRACE_STOP()                             SEGGER_SYSVIEW_Stop()


/*
*********************************************************************************************************
*                                           TRACE EVENT IDS
*********************************************************************************************************
*/

#define  OS_TRACE_ID_OFFSET                          (32u)

#define  OS_TRACE_ID_TICK_INCREMENT                  ( 1u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_ISR_REGISTER                    ( 2u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_TASK_MSG_Q_CREATE               ( 3u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_TASK_MSG_Q_POST                 ( 4u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_TASK_MSG_Q_PEND                 ( 5u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_TASK_SEM_CREATE                 ( 6u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_TASK_SEM_POST                   ( 7u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_TASK_SEM_PEND                   ( 8u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_MUTEX_CREATE                    ( 9u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_MUTEX_DEL                       (10u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_MUTEX_POST                      (11u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_MUTEX_PEND                      (12u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_MUTEX_TASK_PRIO_INHERIT         (13u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_MUTEX_TASK_PRIO_DISINHERIT      (14u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_SEM_CREATE                      (15u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_SEM_DEL                         (16u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_SEM_POST                        (17u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_SEM_PEND                        (18u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_Q_CREATE                        (19u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_Q_DEL                           (20u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_Q_POST                          (21u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_Q_PEND                          (22u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_FLAG_CREATE                     (23u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_FLAG_DEL                        (24u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_FLAG_POST                       (25u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_FLAG_PEND                       (26u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_MEM_CREATE                      (27u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_MEM_PUT                         (28u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_MEM_GET                         (29u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_TASK_SUSPEND                    (30u + OS_TRACE_ID_OFFSET)
#define  OS_TRACE_ID_TASK_RESUME                     (31u + OS_TRACE_ID_OFFSET)

/*
*********************************************************************************************************
*                                          KERNEL RELATED MACROS
*********************************************************************************************************
*/

#define  OS_TRACE_TASK_CREATE(p_tcb)                if (p_tcb != 0) {                            \
                                                      SEGGER_SYSVIEW_OnTaskCreate((U32)p_tcb);   \
                                                      SYSVIEW_AddTask((U32)p_tcb,                \
                                                                      &(p_tcb->NamePtr[0]),      \
                                                                      p_tcb->Prio,               \
                                                                      (U32 *)p_tcb->StkBasePtr,  \
                                                                      p_tcb->StkSize             \
                                                                      );                         \
                                                    }

#define  OS_TRACE_TASK_READY(p_tcb)                 SYSVIEW_TaskReady((U32)p_tcb)
#define  OS_TRACE_TASK_SWITCHED_IN(p_tcb)           SYSVIEW_TaskSwitchedIn((U32)p_tcb)
#define  OS_TRACE_TASK_DLY(dly_ticks)
#define  OS_TRACE_TASK_SUSPEND(p_tcb)               SEGGER_SYSVIEW_OnTaskStopReady((U32)p_tcb, 1);
#define  OS_TRACE_TASK_SUSPENDED(p_tcb)             SYSVIEW_TaskSuspend((U32)p_tcb)
#define  OS_TRACE_TASK_RESUME(p_tcb)                SYSVIEW_TaskReady((U32)p_tcb)
#define  OS_TRACE_TASK_PREEMPT(p_tcb)               if (OSTCBCurPtr->TaskState == OS_TASK_STATE_RDY) { \
                                                        SYSVIEW_TaskPreempt((U32)p_tcb);               \
                                                    }
#define  OS_TRACE_ISR_BEGIN(isr_id)
#define  OS_TRACE_ISR_END()

#define  OS_TRACE_ON_IDLE_ENTER()                   SEGGER_SYSVIEW_OnIdle()

#define  OS_TRACE_ISR_ENTER()                       SEGGER_SYSVIEW_RecordEnterISR()
#define  OS_TRACE_ISR_EXIT()                        SEGGER_SYSVIEW_RecordExitISR()


/*
*********************************************************************************************************
*                                        SIMPLE RECORDER FUNCTIONS
*********************************************************************************************************
*/

#define  OS_TRACE_TICK_INCREMENT(OSTickCtr)                SEGGER_SYSVIEW_RecordU32  (OS_TRACE_ID_TICK_INCREMENT,             (U32)OSTickCtr)
#define  OS_TRACE_ISR_REGISTER(isr_id, isr_name, isr_prio) SEGGER_SYSVIEW_RecordU32x2(OS_TRACE_ID_ISR_REGISTER,               (U32)isr_id,                         (U32)isr_prio)
#define  OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio)     SEGGER_SYSVIEW_RecordU32x2(OS_TRACE_ID_MUTEX_TASK_PRIO_INHERIT,    SEGGER_SYSVIEW_ShrinkId((U32)p_tcb), (U32)prio)
#define  OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio)  SEGGER_SYSVIEW_RecordU32x2(OS_TRACE_ID_MUTEX_TASK_PRIO_DISINHERIT, SEGGER_SYSVIEW_ShrinkId((U32)p_tcb), (U32)prio)
#define  OS_TRACE_TASK_DEL(p_tcb)


/*
*********************************************************************************************************
*                                        COMPLEX RECORDER FUNCTIONS
*********************************************************************************************************
*/

#define  OS_TRACE_MUTEX_CREATE(p_mutex, p_name)      SYSVIEW_RecordU32Register(OS_TRACE_ID_MUTEX_CREATE,      ((U32)p_mutex), p_name)
#define  OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name) SYSVIEW_RecordU32Register(OS_TRACE_ID_TASK_MSG_Q_CREATE, ((U32)p_msg_q), p_name)
#define  OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name)     SYSVIEW_RecordU32Register(OS_TRACE_ID_TASK_SEM_CREATE,   ((U32)p_tcb),   p_name)
#define  OS_TRACE_SEM_CREATE(p_sem, p_name)          SYSVIEW_RecordU32Register(OS_TRACE_ID_SEM_CREATE,        ((U32)p_sem),   p_name)
#define  OS_TRACE_Q_CREATE(p_q, p_name)              SYSVIEW_RecordU32Register(OS_TRACE_ID_Q_CREATE,          ((U32)p_q),     p_name)
#define  OS_TRACE_FLAG_CREATE(p_grp, p_name)         SYSVIEW_RecordU32Register(OS_TRACE_ID_FLAG_CREATE,       ((U32)p_grp),   p_name)
#define  OS_TRACE_MEM_CREATE(p_mem, p_name)          SYSVIEW_RecordU32Register(OS_TRACE_ID_MEM_CREATE,        ((U32)p_mem),   p_name)


/*
*********************************************************************************************************
*                                        KERNEL API ENTER FUNCTIONS
*********************************************************************************************************
*/

#if (OS_CFG_TRACE_API_ENTER_EN == DEF_ENABLED)
#define  OS_TRACE_MUTEX_DEL_ENTER(p_mutex, opt)                                    SEGGER_SYSVIEW_RecordU32x2(OS_TRACE_ID_MUTEX_DEL,       SEGGER_SYSVIEW_ShrinkId((U32)p_mutex), (U32)opt)
#define  OS_TRACE_MUTEX_POST_ENTER(p_mutex, opt)                                   SEGGER_SYSVIEW_RecordU32x2(OS_TRACE_ID_MUTEX_POST,      SEGGER_SYSVIEW_ShrinkId((U32)p_mutex), (U32)opt)
#define  OS_TRACE_MUTEX_PEND_ENTER(p_mutex, timeout, opt, p_ts)                    SEGGER_SYSVIEW_RecordU32x3(OS_TRACE_ID_MUTEX_PEND,      SEGGER_SYSVIEW_ShrinkId((U32)p_mutex), (U32)timeout,  (U32)opt)
#define  OS_TRACE_TASK_MSG_Q_POST_ENTER(p_msg_q, p_void, msg_size, opt)            SEGGER_SYSVIEW_RecordU32x3(OS_TRACE_ID_TASK_MSG_Q_POST, SEGGER_SYSVIEW_ShrinkId((U32)p_msg_q), (U32)msg_size, (U32)opt)
#define  OS_TRACE_TASK_MSG_Q_PEND_ENTER(p_msg_q, timeout, opt, p_msg_size, p_ts)   SEGGER_SYSVIEW_RecordU32x3(OS_TRACE_ID_TASK_MSG_Q_PEND, SEGGER_SYSVIEW_ShrinkId((U32)p_msg_q), (U32)timeout,  (U32)opt)
#define  OS_TRACE_TASK_RESUME_ENTER(p_tcb)                                         SEGGER_SYSVIEW_RecordU32  (OS_TRACE_ID_TASK_RESUME,     SEGGER_SYSVIEW_ShrinkId((U32)p_tcb))
#define  OS_TRACE_TASK_SEM_POST_ENTER(p_tcb, opt)                                  SEGGER_SYSVIEW_RecordU32x2(OS_TRACE_ID_TASK_SEM_POST,   SEGGER_SYSVIEW_ShrinkId((U32)p_tcb),   (U32)opt)
#define  OS_TRACE_TASK_SEM_PEND_ENTER(p_tcb, timeout, opt, p_ts)                   SEGGER_SYSVIEW_RecordU32x3(OS_TRACE_ID_TASK_SEM_PEND,   SEGGER_SYSVIEW_ShrinkId((U32)p_tcb),   (U32)timeout,  (U32)opt)
#define  OS_TRACE_TASK_SUSPEND_ENTER(p_tcb)                                        SEGGER_SYSVIEW_RecordU32  (OS_TRACE_ID_TASK_SUSPEND,    SEGGER_SYSVIEW_ShrinkId((U32)p_tcb))
#define  OS_TRACE_SEM_DEL_ENTER(p_sem, opt)                                        SEGGER_SYSVIEW_RecordU32x2(OS_TRACE_ID_SEM_DEL,         SEGGER_SYSVIEW_ShrinkId((U32)p_sem),   (U32)opt)
#define  OS_TRACE_SEM_POST_ENTER(p_sem, opt)                                       SEGGER_SYSVIEW_RecordU32x2(OS_TRACE_ID_SEM_POST,        SEGGER_SYSVIEW_ShrinkId((U32)p_sem),   (U32)opt)
#define  OS_TRACE_SEM_PEND_ENTER(p_sem, timeout, opt, p_ts)                        SEGGER_SYSVIEW_RecordU32x3(OS_TRACE_ID_SEM_PEND,        SEGGER_SYSVIEW_ShrinkId((U32)p_sem),   (U32)timeout,  (U32)opt)
#define  OS_TRACE_Q_DEL_ENTER(p_q, opt)                                            SEGGER_SYSVIEW_RecordU32x2(OS_TRACE_ID_Q_DEL,           SEGGER_SYSVIEW_ShrinkId((U32)p_q),     (U32)opt)
#define  OS_TRACE_Q_POST_ENTER(p_q, p_void, msg_size, opt)                         SEGGER_SYSVIEW_RecordU32x3(OS_TRACE_ID_Q_POST,          SEGGER_SYSVIEW_ShrinkId((U32)p_q),     (U32)msg_size, (U32)opt)
#define  OS_TRACE_Q_PEND_ENTER(p_q, timeout, opt, p_msg_size, p_ts)                SEGGER_SYSVIEW_RecordU32x3(OS_TRACE_ID_Q_PEND,          SEGGER_SYSVIEW_ShrinkId((U32)p_q),     (U32)timeout,  (U32)opt)
#define  OS_TRACE_FLAG_DEL_ENTER(p_grp, opt)                                       SEGGER_SYSVIEW_RecordU32x2(OS_TRACE_ID_FLAG_DEL,        SEGGER_SYSVIEW_ShrinkId((U32)p_grp),   (U32)opt)
#define  OS_TRACE_FLAG_POST_ENTER(p_grp, flags, opt)                               SEGGER_SYSVIEW_RecordU32x3(OS_TRACE_ID_FLAG_POST,       SEGGER_SYSVIEW_ShrinkId((U32)p_grp),   (U32)flags,    (U32)opt)
#define  OS_TRACE_FLAG_PEND_ENTER(p_grp, flags, timeout, opt, p_ts)                SYSVIEW_RecordU32x4       (OS_TRACE_ID_FLAG_PEND,       SEGGER_SYSVIEW_ShrinkId((U32)p_grp),   (U32)flags,    (U32)timeout, (U32)opt)
#define  OS_TRACE_MEM_PUT_ENTER(p_mem, p_blk)                                      SEGGER_SYSVIEW_RecordU32x2(OS_TRACE_ID_MEM_PUT,         SEGGER_SYSVIEW_ShrinkId((U32)p_mem),   (U32)p_blk)
#define  OS_TRACE_MEM_GET_ENTER(p_mem)                                             SEGGER_SYSVIEW_RecordU32  (OS_TRACE_ID_MEM_GET,         SEGGER_SYSVIEW_ShrinkId((U32)p_mem))
#else
#define  OS_TRACE_MUTEX_DEL_ENTER(p_mutex, opt)
#define  OS_TRACE_MUTEX_POST_ENTER(p_mutex, opt)
#define  OS_TRACE_MUTEX_PEND_ENTER(p_mutex, timeout, opt, p_ts)
#define  OS_TRACE_TASK_MSG_Q_POST_ENTER(p_msg_q, p_void, msg_size, opt)
#define  OS_TRACE_TASK_MSG_Q_PEND_ENTER(p_msg_q, timeout, opt, p_msg_size, p_ts)
#define  OS_TRACE_TASK_RESUME_ENTER(p_tcb)
#define  OS_TRACE_TASK_SEM_POST_ENTER(p_tcb, opt)
#define  OS_TRACE_TASK_SEM_PEND_ENTER(p_tcb, timeout, opt, p_ts)
#define  OS_TRACE_TASK_SUSPEND_ENTER(p_tcb)
#define  OS_TRACE_SEM_DEL_ENTER(p_sem, opt)
#define  OS_TRACE_SEM_POST_ENTER(p_sem, opt)
#define  OS_TRACE_SEM_PEND_ENTER(p_sem, timeout, opt, p_ts)
#define  OS_TRACE_Q_DEL_ENTER(p_q, opt)
#define  OS_TRACE_Q_POST_ENTER(p_q, p_void, msg_size, opt)
#define  OS_TRACE_Q_PEND_ENTER(p_q, timeout, opt, p_msg_size, p_ts)
#define  OS_TRACE_FLAG_DEL_ENTER(p_grp, opt)
#define  OS_TRACE_FLAG_POST_ENTER(p_grp, flags, opt)
#define  OS_TRACE_FLAG_PEND_ENTER(p_grp, flags, timeout, opt, p_ts)
#define  OS_TRACE_MEM_PUT_ENTER(p_mem, p_blk)
#define  OS_TRACE_MEM_GET_ENTER(p_mem)
#endif


/*
*********************************************************************************************************
*                                         KERNEL API EXIT FUNCTIONS
*********************************************************************************************************
*/

#if (OS_CFG_TRACE_API_EXIT_EN == DEF_ENABLED)
#define  OS_TRACE_MUTEX_DEL_EXIT(RetVal)                                                  SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_MUTEX_DEL,       RetVal)
#define  OS_TRACE_MUTEX_POST_EXIT(RetVal)                                                 SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_MUTEX_POST,      RetVal)
#define  OS_TRACE_MUTEX_PEND_EXIT(RetVal)                                                 SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_MUTEX_PEND,      RetVal)
#define  OS_TRACE_TASK_MSG_Q_POST_EXIT(RetVal)                                            SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_TASK_MSG_Q_POST, RetVal)
#define  OS_TRACE_TASK_MSG_Q_PEND_EXIT(RetVal)                                            SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_TASK_MSG_Q_PEND, RetVal)
#define  OS_TRACE_TASK_RESUME_EXIT(RetVal)                                                SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_TASK_RESUME,     RetVal)
#define  OS_TRACE_TASK_SEM_POST_EXIT(RetVal)                                              SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_TASK_SEM_POST,   RetVal)
#define  OS_TRACE_TASK_SEM_PEND_EXIT(RetVal)                                              SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_TASK_SEM_PEND,   RetVal)
#define  OS_TRACE_TASK_SUSPEND_EXIT(RetVal)                                               SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_TASK_SUSPEND,    RetVal)
#define  OS_TRACE_SEM_DEL_EXIT(RetVal)                                                    SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_SEM_DEL,         RetVal)
#define  OS_TRACE_SEM_POST_EXIT(RetVal)                                                   SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_SEM_POST,        RetVal)
#define  OS_TRACE_SEM_PEND_EXIT(RetVal)                                                   SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_SEM_PEND,        RetVal)
#define  OS_TRACE_Q_DEL_EXIT(RetVal)                                                      SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_Q_DEL,           RetVal)
#define  OS_TRACE_Q_POST_EXIT(RetVal)                                                     SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_Q_POST,          RetVal)
#define  OS_TRACE_Q_PEND_EXIT(RetVal)                                                     SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_Q_PEND,          RetVal)
#define  OS_TRACE_FLAG_DEL_EXIT(RetVal)                                                   SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_FLAG_DEL,        RetVal)
#define  OS_TRACE_FLAG_POST_EXIT(RetVal)                                                  SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_FLAG_POST,       RetVal)
#define  OS_TRACE_FLAG_PEND_EXIT(RetVal)                                                  SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_FLAG_PEND,       RetVal)
#define  OS_TRACE_MEM_PUT_EXIT(RetVal)                                                    SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_MEM_PUT,         RetVal)
#define  OS_TRACE_MEM_GET_EXIT(RetVal)                                                    SEGGER_SYSVIEW_RecordEndCallU32 (OS_TRACE_ID_MEM_GET,         RetVal)
#else
#define  OS_TRACE_MUTEX_DEL_EXIT(RetVal)
#define  OS_TRACE_MUTEX_POST_EXIT(RetVal)
#define  OS_TRACE_MUTEX_PEND_EXIT(RetVal)
#define  OS_TRACE_TASK_MSG_Q_POST_EXIT(RetVal)
#define  OS_TRACE_TASK_MSG_Q_PEND_EXIT(RetVal)
#define  OS_TRACE_TASK_RESUME_EXIT(RetVal)
#define  OS_TRACE_TASK_SEM_POST_EXIT(RetVal)
#define  OS_TRACE_TASK_SEM_PEND_EXIT(RetVal)
#define  OS_TRACE_TASK_SUSPEND_EXIT(RetVal)
#define  OS_TRACE_SEM_DEL_EXIT(RetVal)
#define  OS_TRACE_SEM_POST_EXIT(RetVal)
#define  OS_TRACE_SEM_PEND_EXIT(RetVal)
#define  OS_TRACE_Q_DEL_EXIT(RetVal)
#define  OS_TRACE_Q_POST_EXIT(RetVal)
#define  OS_TRACE_Q_PEND_EXIT(RetVal)
#define  OS_TRACE_FLAG_DEL_EXIT(RetVal)
#define  OS_TRACE_FLAG_POST_EXIT(RetVal)
#define  OS_TRACE_FLAG_PEND_EXIT(RetVal)
#define  OS_TRACE_MEM_PUT_EXIT(RetVal)
#define  OS_TRACE_MEM_GET_EXIT(RetVal)
#endif


/*
*********************************************************************************************************
*                                        RECORDER API FUNCTIONS
*********************************************************************************************************
*/

#ifdef __cplusplus
extern "C" {
#endif
void  SYSVIEW_TaskReady      (U32 TaskID);
void  SYSVIEW_TaskSwitchedIn (U32 TaskID);
void  SYSVIEW_TaskSuspend    (U32 TaskID);
void  SYSVIEW_AddTask        (U32 TaskID, const char* NamePtr, U32 Prio, U32* StkBasePtr, U32 StkSize);
void  SYSVIEW_UpdateTask     (U32 TaskID, const char* NamePtr, U32 Prio, U32* StkBasePtr, U32 StkSize);
void  SYSVIEW_SendTaskInfo   (U32 TaskID, const char* sName, unsigned Prio, U32 StackBase, unsigned StackSize);
void  SYSVIEW_RecordU32x4    (unsigned Id, U32 Para0, U32 Para1, U32 Para2, U32 Para3);
void  SYSVIEW_RecordU32x5    (unsigned Id, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4);

void  SYSVIEW_RecordU32Register(unsigned EventId, U32 ResourceId, const char* sResource);
void  SYSVIEW_SendResourceList (void);
void  SYSVIEW_TaskPreempt      (U32 TaskId);

#ifdef __cplusplus
}
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of Kernel trace events module include.           */
