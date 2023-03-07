/***************************************************************************//**
 * @file
 * @brief Kernel - Message Queue Management
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_KERNEL_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  MICRIUM_SOURCE
#include "../include/os.h"
#include "os_priv.h"

#include  <em_core.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const CPU_CHAR *os_q__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_CFG_Q_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                               OSQCreate()
 *
 * @brief    Called by your application to create a message queue. Message queues
 *           MUST be created before they can be used.
 *
 * @param    p_q         Pointer to the message queue.
 *
 * @param    p_name      Pointer to an ASCII string used to name the message queue.
 *
 * @param    max_qty     Indicates the maximum size of the message queue (must be non-zero). Note
 *                       that it is not possible to have a size higher than the maximum number of
 *                       OS_MSGs available.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *******************************************************************************************************/
void OSQCreate(OS_Q       *p_q,
               CPU_CHAR   *p_name,
               OS_MSG_QTY max_qty,
               RTOS_ERR   *p_err)

{
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#ifdef OS_SAFETY_CRITICAL_IEC61508
  if (OSSafetyCriticalStartFlag == DEF_TRUE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_ILLEGAL_RUN_TIME);
    return;
  }
#endif

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               Validate 'p_q'
  OS_ASSERT_DBG_ERR_SET((p_q != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               Cannot specify a zero size queue
  OS_ASSERT_DBG_ERR_SET((max_qty != 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Q size must be within 1 and OSCfg_MsgPoolSize
  OS_ASSERT_DBG_ERR_SET((max_qty <= OSCfg_MsgPoolSize), *p_err, RTOS_ERR_INVALID_ARG,; );

  CORE_ENTER_ATOMIC();

  *p_q = (OS_Q){ 0 };
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  p_q->Type = OS_OBJ_TYPE_Q;                                    // Mark the data structure as a message queue
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_q->NamePtr = p_name;
#else
  (void)&p_name;
#endif
  OS_MsgQInit(&p_q->MsgQ,                                       // Initialize the queue
              max_qty);
  OS_PendListInit(&p_q->PendList);                              // Initialize the waiting list

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_QDbgListAdd(p_q);
  OSQQty++;                                                     // One more queue created
#endif
  OS_TRACE_Q_CREATE(p_q, p_name);
  CORE_EXIT_ATOMIC();
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                                   OSQDel()
 *
 * @brief    This function deletes a message queue and readies all tasks pending on the queue.
 *
 * @param    p_q     Pointer to the message queue to delete.
 *
 * @param    opt     Determines delete options as follows:
 *                       - OS_OPT_DEL_NO_PEND  Deletes the queue ONLY if no task is pending.
 *                       - OS_OPT_DEL_ALWAYS   Deletes the queue even if tasks are waiting.
 *                   In this case, all pending tasks will be
 *                   readied.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_OS_TASK_WAITING
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   == 0    If no tasks were waiting on the queue, or upon error.
 *           >  0    If one or more tasks waiting on the queue are now readied and informed.
 *
 * @note     (1) Use this function with care. Tasks that would normally expect the presence of the
 *               queue MUST check the return code of OSQPend().
 *
 * @note     (2) Because ALL tasks pending on the queue will be readied, you MUST be careful
 *               handling resources in applications where the queue is used for mutual exclusion
 *               because these resource will no longer be guarded by the queue.
 *******************************************************************************************************/
OS_OBJ_QTY OSQDel(OS_Q     *p_q,
                  OS_OPT   opt,
                  RTOS_ERR *p_err)
{
  OS_OBJ_QTY   nbr_tasks;
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb;
  CPU_TS       ts;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  OS_TRACE_Q_DEL_ENTER(p_q, opt);

#ifdef OS_SAFETY_CRITICAL_IEC61508
  if (OSSafetyCriticalStartFlag == DEF_TRUE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_ILLEGAL_RUN_TIME);
    return (0u);
  }
#endif

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, 0u);

  //                                                               Validate 'p_q'
  OS_ASSERT_DBG_ERR_SET((p_q != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_q->Type == OS_OBJ_TYPE_Q), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  CORE_ENTER_ATOMIC();
  p_pend_list = &p_q->PendList;
  nbr_tasks = 0u;
  switch (opt) {
    case OS_OPT_DEL_NO_PEND:                                    // Delete message queue only if no task waiting
      if (p_pend_list->HeadPtr == DEF_NULL) {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
        OS_QDbgListRemove(p_q);
        OSQQty--;
#endif
        OS_TRACE_Q_DEL(p_q);
        OS_QClr(p_q);
        CORE_EXIT_ATOMIC();
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      } else {
        CORE_EXIT_ATOMIC();
        RTOS_ERR_SET(*p_err, RTOS_ERR_OS_TASK_WAITING);
      }
      break;

    case OS_OPT_DEL_ALWAYS:                                     // Always delete the message queue
#if (OS_CFG_TS_EN == DEF_ENABLED)
      ts = OS_TS_GET();                                         // Get local time stamp so all tasks get the same time
#else
      ts = 0u;
#endif
      while (p_pend_list->HeadPtr != DEF_NULL) {                // Remove all tasks from the pend list
        p_tcb = p_pend_list->HeadPtr;
        OS_PendAbort(p_tcb,
                     ts,
                     OS_STATUS_PEND_DEL);
        nbr_tasks++;
      }
#if (OS_CFG_DBG_EN == DEF_ENABLED)
      OS_QDbgListRemove(p_q);
      OSQQty--;
#endif
      OS_TRACE_Q_DEL(p_q);
      OS_QClr(p_q);
      CORE_EXIT_ATOMIC();
      OSSched();                                                // Find highest priority task ready to run
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    default:
      CORE_EXIT_ATOMIC();
      OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_INVALID_ARG, 0u);
  }
  OS_TRACE_Q_DEL_EXIT(RTOS_ERR_CODE_GET(*p_err));
  return (nbr_tasks);
}

/****************************************************************************************************//**
 *                                               OSQFlush()
 *
 * @brief    Flushes the contents of the message queue.
 *
 * @param    p_q     Pointer to the message queue to flush.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   == 0    If no entries were freed, or upon error.
 *           >  0    The number of freed entries.
 *
 * @note     (1) Use great care with this function because when you flush the queue, you
 *               LOSE the references to what the queue entries are pointing, potentially causing
 *               'memory leaks'. In other words, the data to which you are pointing that are being
 *               referenced by the queue entries should, most likely, be de-allocated (i.e. freed).
 *******************************************************************************************************/
OS_MSG_QTY OSQFlush(OS_Q     *p_q,
                    RTOS_ERR *p_err)
{
  OS_MSG_QTY entries;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, 0u);

  //                                                               Validate 'p_q'
  OS_ASSERT_DBG_ERR_SET((p_q != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_q->Type == OS_OBJ_TYPE_Q), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  CORE_ENTER_ATOMIC();
  entries = OS_MsgQFreeAll(&p_q->MsgQ);                         // Return all OS_MSGs to the OS_MSG pool
  CORE_EXIT_ATOMIC();
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (entries);
}

/****************************************************************************************************//**
 *                                                   OSQPend()
 *
 * @brief    Waits for a message to be sent to a queue.
 *
 * @param    p_q         Pointer to the message queue.
 *
 * @param    timeout     Optional timeout period (in clock ticks). If non-zero, your task waits for a
 *                       message to arrive at the queue up to the amount of time specified by this
 *                       argument. However, if you specify 0, your task will wait forever at
 *                       the specified queue or until a message arrives.
 *
 * @param    opt         Determines whether the user wants to block if the queue is empty or not:
 *                           - OS_OPT_PEND_BLOCKING        Task will     block.
 *                           - OS_OPT_PEND_NON_BLOCKING    Task will NOT block.
 *
 * @param    p_msg_size  Pointer to a variable that receives the size of the message.
 *
 * @param    p_ts        Pointer to a variable that receives the timestamp of when the message was
 *                       received, pend aborted, or the message queue was deleted, If you pass
 *                       a NULL pointer (i.e. (CPU_TS *)0), you will not get the timestamp. In other
 *                       words, passing a NULL pointer is valid and indicates that you do not need
 *                       the timestamp.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   != (void *)0    Pointer to the message received.
 *           == (void *)0 :
 *               - If you received a NULL pointer message, or
 *               - if no message was received, or
 *               - if 'p_q' is a NULL pointer, or
 *               - if you didn't pass a pointer to a queue.
 *******************************************************************************************************/
void *OSQPend(OS_Q        *p_q,
              OS_TICK     timeout,
              OS_OPT      opt,
              OS_MSG_SIZE *p_msg_size,
              CPU_TS      *p_ts,
              RTOS_ERR    *p_err)
{
  void *p_void;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  OS_TRACE_Q_PEND_ENTER(p_q, timeout, opt, p_msg_size, p_ts);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, DEF_NULL);

  //                                                               Validate 'p_q'
  OS_ASSERT_DBG_ERR_SET((p_q != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  //                                                               Validate 'p_msg_size'
  OS_ASSERT_DBG_ERR_SET((p_msg_size != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_PEND_BLOCKING)
                         || (opt == OS_OPT_PEND_NON_BLOCKING)), *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_q->Type == OS_OBJ_TYPE_Q), *p_err, RTOS_ERR_INVALID_TYPE, DEF_NULL);

  //                                                               Not allowed to pend in atomic/critical sections
  OS_ASSERT_DBG_ERR_SET(( (opt & OS_OPT_PEND_NON_BLOCKING)
                          || !CORE_IrqIsDisabled()), *p_err, RTOS_ERR_INVALID_STATE, DEF_NULL);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (DEF_NULL);
  }

  if (p_ts != DEF_NULL) {
    *p_ts = 0u;                                                 // Initialize the returned timestamp
  }

  CORE_ENTER_ATOMIC();
  p_void = OS_MsgQGet(&p_q->MsgQ,                               // Any message waiting in the message queue?
                      p_msg_size,
                      p_ts,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    CORE_EXIT_ATOMIC();
    OS_TRACE_Q_PEND(p_q);
    OS_TRACE_Q_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return (p_void);                                            // Yes, Return message received
  }

  if ((opt & OS_OPT_PEND_NON_BLOCKING) != 0u) {                 // Caller wants to block if not available?
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);                 // No
    OS_TRACE_Q_PEND_FAILED(p_q);
    OS_TRACE_Q_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return (DEF_NULL);
  } else {
    if (OSSchedLockNestingCtr > 0u) {                           // Can't pend when the scheduler is locked
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
      OS_TRACE_Q_PEND_FAILED(p_q);
      OS_TRACE_Q_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
      return (DEF_NULL);
    }
  }

  OS_Pend((OS_PEND_OBJ *)((void *)p_q),                         // Block task pending on Message Queue
          OS_TASK_PEND_ON_Q,
          timeout);
  CORE_EXIT_ATOMIC();
  OS_TRACE_Q_PEND_BLOCK(p_q);
  OSSched();                                                    // Find the next highest priority task ready to run

  CORE_ENTER_ATOMIC();
  switch (OSTCBCurPtr->PendStatus) {
    case OS_STATUS_PEND_OK:                                     // Extract message from TCB (Put there by Post)
      p_void = OSTCBCurPtr->MsgPtr;
      *p_msg_size = OSTCBCurPtr->MsgSize;
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
      }
#endif
      OS_TRACE_Q_PEND(p_q);
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case OS_STATUS_PEND_ABORT:                                  // Indicate that we aborted
      p_void = DEF_NULL;
      *p_msg_size = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
      }
#endif
      OS_TRACE_Q_PEND_FAILED(p_q);
      RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);
      break;

    case OS_STATUS_PEND_TIMEOUT:                                // Indicate that we didn't get event within TO
      p_void = DEF_NULL;
      *p_msg_size = 0u;
      if (p_ts != DEF_NULL) {
        *p_ts = 0u;
      }
      OS_TRACE_Q_PEND_FAILED(p_q);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
      break;

    case OS_STATUS_PEND_DEL:                                    // Indicate that object pended on has been deleted
      p_void = DEF_NULL;
      *p_msg_size = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
      }
#endif
      OS_TRACE_Q_PEND_FAILED(p_q);
      RTOS_ERR_SET(*p_err, RTOS_ERR_OS_OBJ_DEL);
      break;

    default:
      p_void = DEF_NULL;
      *p_msg_size = 0u;
      OS_TRACE_Q_PEND_FAILED(p_q);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, DEF_NULL);
  }
  CORE_EXIT_ATOMIC();
  OS_TRACE_Q_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
  return (p_void);
}

/****************************************************************************************************//**
 *                                               OSQPendAbort()
 *
 * @brief    Aborts and readies any tasks currently waiting on a queue. Use this function to
 *           fault-abort the wait on the queue, rather than the normal signaling of the queue via
 *           OSQPost().
 *
 * @param    p_q     Pointer to the message queue.
 *
 * @param    opt     Determines the type of ABORT performed:
 *                       - OS_OPT_PEND_ABORT_1     ABORT wait for a single task (HPT) waiting on the
 *                                                 message queue.
 *                       - OS_OPT_PEND_ABORT_ALL   ABORT wait for ALL tasks that are  waiting on the
 *                                                 message queue.
 *                       - OS_OPT_POST_NO_SCHED    Do not call the scheduler.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NONE_WAITING
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   == 0    If no tasks were waiting on the queue, or upon error.
 *           >  0    If one or more tasks waiting on the queue are now readied and informed.
 *******************************************************************************************************/
OS_OBJ_QTY OSQPendAbort(OS_Q     *p_q,
                        OS_OPT   opt,
                        RTOS_ERR *p_err)
{
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb;
  CPU_TS       ts;
  OS_OBJ_QTY   nbr_tasks;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, 0u);

  //                                                               Validate 'p_q'
  OS_ASSERT_DBG_ERR_SET((p_q != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_PEND_ABORT_1)
                         || (opt == OS_OPT_PEND_ABORT_ALL)
                         || (opt == (OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED))
                         || (opt == (OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED))), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_q->Type == OS_OBJ_TYPE_Q), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  CORE_ENTER_ATOMIC();
  p_pend_list = &p_q->PendList;
  if (p_pend_list->HeadPtr == DEF_NULL) {                       // Any task waiting on queue?
    CORE_EXIT_ATOMIC();                                         // No
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE_WAITING);
    return (0u);
  }

  nbr_tasks = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
  ts = OS_TS_GET();                                             // Get local time stamp so all tasks get the same time
#else
  ts = 0u;
#endif
  while (p_pend_list->HeadPtr != DEF_NULL) {
    p_tcb = p_pend_list->HeadPtr;
    OS_PendAbort(p_tcb,
                 ts,
                 OS_STATUS_PEND_ABORT);
    nbr_tasks++;
    if (opt != OS_OPT_PEND_ABORT_ALL) {                         // Pend abort all tasks waiting?
      break;                                                    // No
    }
  }
  CORE_EXIT_ATOMIC();

  if ((opt & OS_OPT_POST_NO_SCHED) == 0u) {
    OSSched();                                                  // Run the scheduler
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (nbr_tasks);
}

/****************************************************************************************************//**
 *                                                   OSQPost()
 *
 * @brief    Sends a message to a queue. With the 'opt' argument, you can specify
 *           if the message is broadcast to all waiting tasks and/or if you post the message
 *           to the front of the queue (LIFO) or normally (FIFO) at the end of the queue.
 *
 * @param    p_q         Pointer to a message queue.
 *
 * @param    p_void      Pointer to the message to send.
 *
 * @param    msg_size    Specifies the size of the message (in bytes).
 *
 * @param    opt         Determines the type of POST performed
 *                           - Types of POST :
 *                           - OS_OPT_POST_ALL         POST to ALL tasks that are waiting on the queue.
 *                                                     This option can be added to either
 *                                                     OS_OPT_POST_FIFO or OS_OPT_POST_LIFO.
 *                           - OS_OPT_POST_FIFO        POST message to end of queue (FIFO) and wake up
 *                                                     a single waiting task.
 *                           - OS_OPT_POST_LIFO        POST message to the front of the queue (LIFO) and
 *                                                     wake up a single waiting task.
 *                           - OS_OPT_POST_NO_SCHED    Do not call the scheduler.
 *                               - OS_OPT_POST_NO_SCHED can be added (OR'd) with other options.
 *                               - OS_OPT_POST_ALL      can be added (OR'd) with other options.
 *                               - The possible combinations of options are:
 *                                   - OS_OPT_POST_FIFO
 *                                   - OS_OPT_POST_LIFO
 *                                   - OS_OPT_POST_FIFO + OS_OPT_POST_ALL
 *                                   - OS_OPT_POST_LIFO + OS_OPT_POST_ALL
 *                                   - OS_OPT_POST_FIFO + OS_OPT_POST_NO_SCHED
 *                                   - OS_OPT_POST_LIFO + OS_OPT_POST_NO_SCHED
 *                                   - OS_OPT_POST_FIFO + OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED
 *                                   - OS_OPT_POST_LIFO + OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_NO_MORE_RSRC
 *                           - RTOS_ERR_NOT_READY
 *
 * @note     (1) This function may be called from an ISR.
 *******************************************************************************************************/
void OSQPost(OS_Q        *p_q,
             void        *p_void,
             OS_MSG_SIZE msg_size,
             OS_OPT      opt,
             RTOS_ERR    *p_err)
{
  OS_OPT       post_type;
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb;
  OS_TCB       *p_tcb_next;
  CPU_TS       ts;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  OS_TRACE_Q_POST_ENTER(p_q, p_void, msg_size, opt);

  //                                                               Validate 'p_q'
  OS_ASSERT_DBG_ERR_SET((p_q != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_POST_FIFO)
                         || (opt == OS_OPT_POST_LIFO)
                         || (opt == (OS_OPT_POST_FIFO | OS_OPT_POST_ALL))
                         || (opt == (OS_OPT_POST_LIFO | OS_OPT_POST_ALL))
                         || (opt == (OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED))
                         || (opt == (OS_OPT_POST_LIFO | OS_OPT_POST_NO_SCHED))
                         || (opt == (OS_OPT_POST_FIFO | OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED))
                         || (opt == (OS_OPT_POST_LIFO | OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED))), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_q->Type == OS_OBJ_TYPE_Q), *p_err, RTOS_ERR_INVALID_TYPE,; );

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return;
  }

#if (OS_CFG_TS_EN == DEF_ENABLED)
  ts = OS_TS_GET();                                             // Get timestamp
#else
  ts = 0u;
#endif

  OS_TRACE_Q_POST(p_q);

  CORE_ENTER_ATOMIC();
  p_pend_list = &p_q->PendList;
  if (p_pend_list->HeadPtr == DEF_NULL) {                       // Any task waiting on message queue?
    if ((opt & OS_OPT_POST_LIFO) == 0u) {                       // Determine whether we post FIFO or LIFO
      post_type = OS_OPT_POST_FIFO;
    } else {
      post_type = OS_OPT_POST_LIFO;
    }
    OS_MsgQPut(&p_q->MsgQ,                                      // Place message in the message queue
               p_void,
               msg_size,
               post_type,
               ts,
               p_err);
    CORE_EXIT_ATOMIC();
    OS_TRACE_Q_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      OS_TRACE_Q_POST_FAILED(p_q);
    }
    return;
  }

  p_tcb = p_pend_list->HeadPtr;
  while (p_tcb != DEF_NULL) {
    p_tcb_next = p_tcb->PendNextPtr;
    OS_Post((OS_PEND_OBJ *)((void *)p_q),
            p_tcb,
            p_void,
            msg_size,
            ts);
    if ((opt & OS_OPT_POST_ALL) == 0) {                         // Post message to all tasks waiting?
      break;                                                    // No
    }
    p_tcb = p_tcb_next;
  }

  CORE_EXIT_ATOMIC();

  if ((opt & OS_OPT_POST_NO_SCHED) == 0u) {
    OSSched();                                                  // Run the scheduler
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  OS_TRACE_Q_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                                   OS_QClr()
 *
 * @brief    This function is called by OSQDel() to clear the contents of a message queue.
 *
 * @param    p_q     Pointer to the queue to clear.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_QClr(OS_Q *p_q)
{
  (void)OS_MsgQFreeAll(&p_q->MsgQ);                             // Return all OS_MSGs to the free list
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  p_q->Type = OS_OBJ_TYPE_NONE;                                 // Mark the data structure as a NONE
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_q->NamePtr = (CPU_CHAR *)((void *)"?Q");
#endif
  OS_MsgQInit(&p_q->MsgQ,                                       // Initialize the list of OS_MSGs
              0u);
  OS_PendListInit(&p_q->PendList);                              // Initialize the waiting list
}

/****************************************************************************************************//**
 *                                               OS_QDbgListAdd()
 *
 * @brief    Add a message queue to the message queue debug list.
 *
 * @param    p_q     Pointer to the message queue to add.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_QDbgListAdd(OS_Q *p_q)
{
  p_q->DbgNamePtr = (CPU_CHAR *)((void *)" ");
  p_q->DbgPrevPtr = DEF_NULL;
  if (OSQDbgListPtr == DEF_NULL) {
    p_q->DbgNextPtr = DEF_NULL;
  } else {
    p_q->DbgNextPtr = OSQDbgListPtr;
    OSQDbgListPtr->DbgPrevPtr = p_q;
  }
  OSQDbgListPtr = p_q;
}

/****************************************************************************************************//**
 *                                           OS_QDbgListRemove()
 *
 * @brief    Remove a message queue from the message queue debug list.
 *
 * @param    p_q     Pointer to the message queue to remove.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_QDbgListRemove(OS_Q *p_q)
{
  OS_Q *p_q_next;
  OS_Q *p_q_prev;

  p_q_prev = p_q->DbgPrevPtr;
  p_q_next = p_q->DbgNextPtr;

  if (p_q_prev == DEF_NULL) {
    OSQDbgListPtr = p_q_next;
    if (p_q_next != DEF_NULL) {
      p_q_next->DbgPrevPtr = DEF_NULL;
    }
    p_q->DbgNextPtr = DEF_NULL;
  } else if (p_q_next == DEF_NULL) {
    p_q_prev->DbgNextPtr = DEF_NULL;
    p_q->DbgPrevPtr = DEF_NULL;
  } else {
    p_q_prev->DbgNextPtr = p_q_next;
    p_q_next->DbgPrevPtr = p_q_prev;
    p_q->DbgNextPtr = DEF_NULL;
    p_q->DbgPrevPtr = DEF_NULL;
  }
}
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
