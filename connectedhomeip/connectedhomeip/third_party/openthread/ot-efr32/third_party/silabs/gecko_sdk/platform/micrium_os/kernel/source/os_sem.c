/***************************************************************************//**
 * @file
 * @brief Kernel - Semaphore Management
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
const CPU_CHAR *os_sem__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_CFG_SEM_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                               OSSemCreate()
 *
 * @brief    Creates a semaphore.
 *
 * @param    p_sem   Pointer to the semaphore to initialize. Your application is responsible
 *                   for allocating storage for the semaphore.
 *
 * @param    p_name  Pointer to the name to assign to the semaphore.
 *
 * @param    cnt     The initial value for the semaphore.
 *                   If used to share resources, you should initialize to the number of resources
 *                   available.
 *                   If used to signal the occurrence of event(s), you should initialize to 0.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *******************************************************************************************************/
void OSSemCreate(OS_SEM     *p_sem,
                 CPU_CHAR   *p_name,
                 OS_SEM_CTR cnt,
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

  //                                                               Validate 'p_sem'
  OS_ASSERT_DBG_ERR_SET((p_sem != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CORE_ENTER_ATOMIC();

  *p_sem = (OS_SEM){ 0 };
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  p_sem->Type = OS_OBJ_TYPE_SEM;                                // Mark the data structure as a semaphore
#endif
  p_sem->Ctr = cnt;                                             // Set semaphore value
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_sem->NamePtr = p_name;                                      // Save the name of the semaphore
#else
  (void)&p_name;
#endif
  OS_PendListInit(&p_sem->PendList);                            // Initialize the waiting list

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_SemDbgListAdd(p_sem);
  OSSemQty++;
#endif

  OS_TRACE_SEM_CREATE(p_sem, p_name);

  CORE_EXIT_ATOMIC();
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               OSSemDel()
 *
 * @brief    Deletes a semaphore.
 *
 * @param    p_sem   Pointer to the semaphore to delete.
 *
 * @param    opt     Determines delete options as follows:
 *                       - OS_OPT_DEL_NO_PEND  Deletes the semaphore ONLY if there are no pending tasks.
 *                       - OS_OPT_DEL_ALWAYS   Deletes the semaphore even if tasks are waiting.
 *                   In this case, all the pending tasks will be readied.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_OS_TASK_WAITING
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   == 0    If there were no tasks waiting on the semaphore, or upon error.
 *           >  0    If one or more tasks waiting on the semaphore that are now readied and informed.
 *
 * @note     (1) Use this function with care. Tasks that would normally expect the presence of
 *               the semaphore MUST check the return code of OSSemPend().
 *
 * @note     (2) Because ALL tasks pending on the semaphore will be readied, be careful with
 *               applications where the semaphore is used for mutual exclusion because the resource(s)
 *               will no longer be guarded by the semaphore.
 *******************************************************************************************************/
OS_OBJ_QTY OSSemDel(OS_SEM   *p_sem,
                    OS_OPT   opt,
                    RTOS_ERR *p_err)
{
  OS_OBJ_QTY   nbr_tasks;
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb;
  CPU_TS       ts;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  OS_TRACE_SEM_DEL_ENTER(p_sem, opt);

#ifdef OS_SAFETY_CRITICAL_IEC61508
  if (OSSafetyCriticalStartFlag == DEF_TRUE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_ILLEGAL_RUN_TIME);
    return (0u);
  }
#endif

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, 0u);

  //                                                               Validate 'p_sem'
  OS_ASSERT_DBG_ERR_SET((p_sem != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_sem->Type == OS_OBJ_TYPE_SEM), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  CORE_ENTER_ATOMIC();
  p_pend_list = &p_sem->PendList;
  nbr_tasks = 0u;
  switch (opt) {
    case OS_OPT_DEL_NO_PEND:                                    // Delete semaphore only if no task waiting
      if (p_pend_list->HeadPtr == DEF_NULL) {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
        OS_SemDbgListRemove(p_sem);
        OSSemQty--;
#endif
        OS_TRACE_SEM_DEL(p_sem);
        OS_SemClr(p_sem);
        CORE_EXIT_ATOMIC();
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      } else {
        CORE_EXIT_ATOMIC();
        RTOS_ERR_SET(*p_err, RTOS_ERR_OS_TASK_WAITING);
      }
      break;

    case OS_OPT_DEL_ALWAYS:                                     // Always delete the semaphore
#if (OS_CFG_TS_EN == DEF_ENABLED)
      ts = OS_TS_GET();                                         // Get local time stamp so all tasks get the same time
#else
      ts = 0u;
#endif
      while (p_pend_list->HeadPtr != DEF_NULL) {                // Remove all tasks on the pend list
        p_tcb = p_pend_list->HeadPtr;
        OS_PendAbort(p_tcb,
                     ts,
                     OS_STATUS_PEND_DEL);
        nbr_tasks++;
      }
#if (OS_CFG_DBG_EN == DEF_ENABLED)
      OS_SemDbgListRemove(p_sem);
      OSSemQty--;
#endif
      OS_TRACE_SEM_DEL(p_sem);
      OS_SemClr(p_sem);
      CORE_EXIT_ATOMIC();
      OSSched();                                                // Find highest priority task ready to run
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    default:
      CORE_EXIT_ATOMIC();
      OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_INVALID_ARG, 0u);
  }

  OS_TRACE_SEM_DEL_EXIT(RTOS_ERR_CODE_GET(*p_err));

  return (nbr_tasks);
}

/****************************************************************************************************//**
 *                                               OSSemPend()
 *
 * @brief    Waits for a semaphore.
 *
 * @param    p_sem       Pointer to the semaphore.
 *
 * @param    timeout     Optional timeout period (in clock ticks). If non-zero, your task will
 *                       wait for the resource up to the amount of time (in 'ticks') specified by this
 *                       argument. If you enter 0, your task will wait forever at the specified
 *                       semaphore, or until the resource becomes available (or the event occurs).
 *
 * @param    opt         Determines whether the user wants to block if the semaphore is available or not:
 *                           - OS_OPT_PEND_BLOCKING        Task will     block.
 *                           - OS_OPT_PEND_NON_BLOCKING    Task will NOT block.
 *
 * @param    p_ts        Pointer to a variable that receives the timestamp of when the semaphore was
 *                       posted or pending aborted or the semaphore deleted. If you pass a
 *                       NULL pointer (i.e. (CPU_TS*)0), you will not get the timestamp. In other
 *                       words, passing a NULL pointer is valid and indicates that you don't need the
 *                       timestamp.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @return   The current value of the semaphore counter, or 0 if not available.
 *******************************************************************************************************/
OS_SEM_CTR OSSemPend(OS_SEM   *p_sem,
                     OS_TICK  timeout,
                     OS_OPT   opt,
                     CPU_TS   *p_ts,
                     RTOS_ERR *p_err)
{
  OS_SEM_CTR ctr;
  CORE_DECLARE_IRQ_STATE;

#if (OS_CFG_TS_EN == DEF_DISABLED)
  (void)&p_ts;                                                  // Prevent compiler warning for not using 'ts'
#endif

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  OS_TRACE_SEM_PEND_ENTER(p_sem, timeout, opt, p_ts);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET(( (opt & OS_OPT_PEND_NON_BLOCKING)
                          || !(CORE_InIrqContext())), *p_err, RTOS_ERR_ISR, 0u);

  //                                                               Validate 'p_sem'
  OS_ASSERT_DBG_ERR_SET((p_sem != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_PEND_BLOCKING)
                         || (opt == OS_OPT_PEND_NON_BLOCKING)), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_sem->Type == OS_OBJ_TYPE_SEM), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

  //                                                               Not allowed to pend in atomic/critical sections
  OS_ASSERT_DBG_ERR_SET(( (opt & OS_OPT_PEND_NON_BLOCKING)
                          || !CORE_IrqIsDisabled()), *p_err, RTOS_ERR_INVALID_STATE, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  CORE_ENTER_ATOMIC();
  if (p_sem->Ctr > 0u) {                                        // Resource available?
    p_sem->Ctr--;                                               // Yes, caller may proceed
#if (OS_CFG_TS_EN == DEF_ENABLED)
    if (p_ts != DEF_NULL) {
      *p_ts = p_sem->TS;                                        // get timestamp of last post
    }
#endif
    ctr = p_sem->Ctr;
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    OS_TRACE_SEM_PEND(p_sem);
    OS_TRACE_SEM_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return (ctr);
  }

  if ((opt & OS_OPT_PEND_NON_BLOCKING) != 0u) {                 // Caller wants to block if not available?
#if (OS_CFG_TS_EN == DEF_ENABLED)
    if (p_ts != DEF_NULL) {
      *p_ts = 0u;
    }
#endif
    ctr = p_sem->Ctr;                                           // No
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);
    OS_TRACE_SEM_PEND_FAILED(p_sem);
    OS_TRACE_SEM_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return (ctr);
  } else {                                                      // Yes
    if (OSSchedLockNestingCtr > 0u) {                           // Can't pend when the scheduler is locked
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = 0u;
      }
#endif
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
      OS_TRACE_SEM_PEND_FAILED(p_sem);
      OS_TRACE_SEM_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
      return (0u);
    }
  }

  OS_Pend((OS_PEND_OBJ *)((void *)p_sem),                       // Block task pending on Semaphore
          OS_TASK_PEND_ON_SEM,
          timeout);
  CORE_EXIT_ATOMIC();
  OS_TRACE_SEM_PEND_BLOCK(p_sem);
  OSSched();                                                    // Find the next highest priority task ready to run

  CORE_ENTER_ATOMIC();
  switch (OSTCBCurPtr->PendStatus) {
    case OS_STATUS_PEND_OK:                                     // We got the semaphore
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
      }
#endif
      OS_TRACE_SEM_PEND(p_sem);
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case OS_STATUS_PEND_ABORT:                                  // Indicate that we aborted
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
      }
#endif
      OS_TRACE_SEM_PEND_FAILED(p_sem);
      RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);
      break;

    case OS_STATUS_PEND_TIMEOUT:                                // Indicate that we didn't get semaphore within timeout
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = 0u;
      }
#endif
      OS_TRACE_SEM_PEND_FAILED(p_sem);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
      break;

    case OS_STATUS_PEND_DEL:                                    // Indicate that object pended on has been deleted
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
      }
#endif
      OS_TRACE_SEM_PEND_FAILED(p_sem);
      RTOS_ERR_SET(*p_err, RTOS_ERR_OS_OBJ_DEL);
      break;

    default:
      CORE_EXIT_ATOMIC();
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, 0u);
      OS_TRACE_SEM_PEND_FAILED(p_sem);
      OS_TRACE_SEM_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
  }
  ctr = p_sem->Ctr;
  CORE_EXIT_ATOMIC();
  OS_TRACE_SEM_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
  return (ctr);
}

/****************************************************************************************************//**
 *                                               OSSemPendAbort()
 *
 * @brief    Aborts and readies any tasks currently waiting on a semaphore. Rather than signal the
 *           semaphore via OSSemPost(), use this function to fault-abort the wait on the semaphore.
 *
 * @param    p_sem   Pointer to the semaphore.
 *
 * @param    opt     Determines the type of ABORT performed:
 *                       - OS_OPT_PEND_ABORT_1     ABORT waits for a single task (HPT) waiting on
 *                                                 the semaphore.
 *                       - OS_OPT_PEND_ABORT_ALL   ABORT waits for ALL tasks that are waiting on
 *                                                 the semaphore.
 *                       - OS_OPT_POST_NO_SCHED    Do not call the scheduler.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NONE_WAITING
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   == 0    If no tasks were waiting on the semaphore, or upon error.
 *           >  0    If one or more tasks waiting on the semaphore are now readied and informed.
 *******************************************************************************************************/
OS_OBJ_QTY OSSemPendAbort(OS_SEM   *p_sem,
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

  //                                                               Validate 'p_sem'
  OS_ASSERT_DBG_ERR_SET((p_sem != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_PEND_ABORT_1)
                         || (opt == OS_OPT_PEND_ABORT_ALL)
                         || (opt == (OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED))
                         || (opt == (OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED))), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_sem->Type == OS_OBJ_TYPE_SEM), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  CORE_ENTER_ATOMIC();
  p_pend_list = &p_sem->PendList;
  if (p_pend_list->HeadPtr == DEF_NULL) {                       // Any task waiting on semaphore?
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
 *                                               OSSemPost()
 *
 * @brief    Signals a semaphore.
 *
 * @param    p_sem   Pointer to the semaphore.
 *
 * @param    opt     Determines the type of POST performed:
 *                       - OS_OPT_POST_1           POST and ready only the highest priority task waiting
 *                                                 on semaphore (if tasks are waiting).
 *                       - OS_OPT_POST_ALL         POST to ALL tasks that are waiting on the semaphore.
 *                       - OS_OPT_POST_NO_SCHED    Do not call the scheduler.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   The current value of the semaphore counter or 0 upon error.
 *
 * @note     (1) OS_OPT_POST_NO_SCHED can be OR'ed with one of the other two options to prevent the
 *               scheduler from being called.
 *
 * @note     (2) This function may be called from an ISR.
 *******************************************************************************************************/
OS_SEM_CTR OSSemPost(OS_SEM   *p_sem,
                     OS_OPT   opt,
                     RTOS_ERR *p_err)
{
  OS_SEM_CTR   ctr;
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb;
  OS_TCB       *p_tcb_next;
  CPU_TS       ts;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  OS_TRACE_SEM_POST_ENTER(p_sem, opt);

  //                                                               Validate 'p_sem'
  OS_ASSERT_DBG_ERR_SET((p_sem != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_POST_1)
                         || (opt == OS_OPT_POST_ALL)
                         || (opt == (OS_OPT_POST_1   | OS_OPT_POST_NO_SCHED))
                         || (opt == (OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED))), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_sem->Type == OS_OBJ_TYPE_SEM), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

#if (OS_CFG_TS_EN == DEF_ENABLED)
  ts = OS_TS_GET();                                             // Get timestamp
#else
  ts = 0u;
#endif

  OS_TRACE_SEM_POST(p_sem);

  CORE_ENTER_ATOMIC();
  p_pend_list = &p_sem->PendList;
  if (p_pend_list->HeadPtr == DEF_NULL) {                       // Any task waiting on semaphore?
    if (p_sem->Ctr == (OS_SEM_CTR)-1) {
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
      OS_TRACE_SEM_POST_FAILED(p_sem);
      OS_TRACE_SEM_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
      return (0u);
    }
    p_sem->Ctr++;                                               // No
    ctr = p_sem->Ctr;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    p_sem->TS = ts;                                             // Save timestamp in semaphore control block
#endif
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    OS_TRACE_SEM_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return (ctr);
  }

  p_tcb = p_pend_list->HeadPtr;
  while (p_tcb != DEF_NULL) {
    p_tcb_next = p_tcb->PendNextPtr;
    OS_Post((OS_PEND_OBJ *)((void *)p_sem),
            p_tcb,
            DEF_NULL,
            0u,
            ts);
    if ((opt & OS_OPT_POST_ALL) == 0) {                         // Post to all tasks waiting?
      break;                                                    // No
    }
    p_tcb = p_tcb_next;
  }
  CORE_EXIT_ATOMIC();
  if ((opt & OS_OPT_POST_NO_SCHED) == 0u) {
    OSSched();                                                  // Run the scheduler
  }
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  OS_TRACE_SEM_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));

  return (0u);
}

/****************************************************************************************************//**
 *                                               OSSemSet()
 *
 * @brief    Sets the semaphore count to the value specified as an argument. Typically
 *           this value would be 0, but you can set the semaphore to any value.
 *           Typically, you would use this function when a semaphore acts as a signaling mechanism
 *           and you want to reset the count value.
 *
 * @param    p_sem   Pointer to the semaphore.
 *
 * @param    cnt     The new value for the semaphore count. You would pass 0 to reset the semaphore
 *                   count.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_TASK_WAITING
 *******************************************************************************************************/
void OSSemSet(OS_SEM     *p_sem,
              OS_SEM_CTR cnt,
              RTOS_ERR   *p_err)
{
  OS_PEND_LIST *p_pend_list;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               Validate 'p_sem'
  OS_ASSERT_DBG_ERR_SET((p_sem != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_sem->Type == OS_OBJ_TYPE_SEM), *p_err, RTOS_ERR_INVALID_TYPE,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  CORE_ENTER_ATOMIC();
  if (p_sem->Ctr > 0u) {                                        // See if semaphore already has a count
    p_sem->Ctr = cnt;                                           // Yes, set it to the new value specified.
  } else {
    p_pend_list = &p_sem->PendList;                             // No
    if (p_pend_list->HeadPtr == DEF_NULL) {                     // See if task(s) waiting?
      p_sem->Ctr = cnt;                                         // No, OK to set the value
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_OS_TASK_WAITING);
    }
  }
  CORE_EXIT_ATOMIC();
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               OS_SemClr()
 *
 * @brief    This function is called by OSSemDel() to clear the contents of a semaphore.
 *
 * @param    p_sem   Pointer to the semaphore to clear.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_SemClr(OS_SEM *p_sem)
{
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  p_sem->Type = OS_OBJ_TYPE_NONE;                               // Mark the data structure as a NONE
#endif
  p_sem->Ctr = 0u;                                              // Set semaphore value
#if (OS_CFG_TS_EN == DEF_ENABLED)
  p_sem->TS = 0u;                                               // Clear the time stamp
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_sem->NamePtr = (CPU_CHAR *)((void *)"?SEM");
#endif
  OS_PendListInit(&p_sem->PendList);                            // Initialize the waiting list
}

/****************************************************************************************************//**
 *                                           OS_SemDbgListAdd()
 *
 * @brief    Add a semaphore to the semaphore debug list.
 *
 * @param    p_sem   Pointer to the semaphore to add.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_SemDbgListAdd(OS_SEM *p_sem)
{
  p_sem->DbgNamePtr = (CPU_CHAR *)((void *)" ");
  p_sem->DbgPrevPtr = DEF_NULL;
  if (OSSemDbgListPtr == DEF_NULL) {
    p_sem->DbgNextPtr = DEF_NULL;
  } else {
    p_sem->DbgNextPtr = OSSemDbgListPtr;
    OSSemDbgListPtr->DbgPrevPtr = p_sem;
  }
  OSSemDbgListPtr = p_sem;
}

/****************************************************************************************************//**
 *                                           OS_SemDbgListRemove()
 *
 * @brief    Remove a semaphore from the semaphore debug list.
 *
 * @param    p_sem   Pointer to the semaphore to remove.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_SemDbgListRemove(OS_SEM *p_sem)
{
  OS_SEM *p_sem_next;
  OS_SEM *p_sem_prev;

  p_sem_prev = p_sem->DbgPrevPtr;
  p_sem_next = p_sem->DbgNextPtr;

  if (p_sem_prev == DEF_NULL) {
    OSSemDbgListPtr = p_sem_next;
    if (p_sem_next != DEF_NULL) {
      p_sem_next->DbgPrevPtr = DEF_NULL;
    }
    p_sem->DbgNextPtr = DEF_NULL;
  } else if (p_sem_next == DEF_NULL) {
    p_sem_prev->DbgNextPtr = DEF_NULL;
    p_sem->DbgPrevPtr = DEF_NULL;
  } else {
    p_sem_prev->DbgNextPtr = p_sem_next;
    p_sem_next->DbgPrevPtr = p_sem_prev;
    p_sem->DbgNextPtr = DEF_NULL;
    p_sem->DbgPrevPtr = DEF_NULL;
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
