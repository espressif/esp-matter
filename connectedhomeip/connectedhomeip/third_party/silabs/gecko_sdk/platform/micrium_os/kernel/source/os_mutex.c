/***************************************************************************//**
 * @file
 * @brief Kernel - Mutex Management
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
const CPU_CHAR *os_mutex__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                               OSMutexCreate()
 *
 * @brief    Creates a mutex so that multiple program threads can take turns sharing the same resource.
 *
 * @param    p_mutex     Pointer to the mutex to initialize. Your application is responsible for
 *                       allocating storage for the mutex.
 *
 * @param    p_name      Pointer to the name you would like to give the mutex.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *******************************************************************************************************/
void OSMutexCreate(OS_MUTEX *p_mutex,
                   CPU_CHAR *p_name,
                   RTOS_ERR *p_err)
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

  //                                                               Validate 'p_mutex'
  OS_ASSERT_DBG_ERR_SET((p_mutex != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CORE_ENTER_ATOMIC();

  *p_mutex = (OS_MUTEX){ 0 };
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  p_mutex->Type = OS_OBJ_TYPE_MUTEX;                            // Mark the data structure as a mutex
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_mutex->NamePtr = p_name;
#else
  (void)&p_name;
#endif
  OS_PendListInit(&p_mutex->PendList);                          // Initialize the waiting list

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_MutexDbgListAdd(p_mutex);
  OSMutexQty++;
#endif

  OS_TRACE_MUTEX_CREATE(p_mutex, p_name);

  CORE_EXIT_ATOMIC();
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               OSMutexDel()
 *
 * @brief    Deletes a mutex and readies all tasks pending on the mutex.
 *
 * @param    p_mutex     Pointer to the mutex to delete.
 *
 * @param    opt         Determines delete options as follows:
 *                           - OS_OPT_DEL_NO_PEND  Deletes the mutex ONLY if no tasks are pending.
 *                           - OS_OPT_DEL_ALWAYS   Deletes the mutex even if tasks are waiting.
 *                       In this case, all pending tasks will be readied.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_OS_TASK_WAITING
 *                           - RTOS_ERR_NOT_READY
 *
 * @return   == 0    If there are no tasks waiting on the mutex, or upon error.
 *           >  0    If there are one or more tasks waiting on the mutex that are now readied
 *                   and informed.
 *
 * @note     (1) Use this function with care. Tasks that would normally expect the presence of the
 *               mutex MUST check the return code of OSMutexPend().
 *
 * @note     (2) Because ALL tasks pending on the mutex will be readied, be careful in applications
 *               where the mutex is used for mutual exclusion because the resource(s) will no longer
 *               be guarded by the mutex.
 *******************************************************************************************************/
OS_OBJ_QTY OSMutexDel(OS_MUTEX *p_mutex,
                      OS_OPT   opt,
                      RTOS_ERR *p_err)
{
  OS_OBJ_QTY   nbr_tasks;
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb;
  OS_TCB       *p_tcb_owner;
  CPU_TS       ts;
  OS_PRIO      prio_new;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  OS_TRACE_MUTEX_DEL_ENTER(p_mutex, opt);

#ifdef OS_SAFETY_CRITICAL_IEC61508
  if (OSSafetyCriticalStartFlag == DEF_TRUE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_ILLEGAL_RUN_TIME);
    return (0u);
  }
#endif

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, 0u);

  //                                                               Validate 'p_mutex'
  OS_ASSERT_DBG_ERR_SET((p_mutex != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_mutex->Type == OS_OBJ_TYPE_MUTEX), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  CORE_ENTER_ATOMIC();
  p_pend_list = &p_mutex->PendList;
  nbr_tasks = 0u;
  switch (opt) {
    case OS_OPT_DEL_NO_PEND:                                    // Delete mutex only if no task waiting
      if (p_pend_list->HeadPtr == DEF_NULL) {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
        OS_MutexDbgListRemove(p_mutex);
        OSMutexQty--;
#endif
        OS_TRACE_MUTEX_DEL(p_mutex);
        if (p_mutex->OwnerTCBPtr != DEF_NULL) {                 // Does the mutex belong to a task?
          OS_MutexGrpRemove(p_mutex->OwnerTCBPtr, p_mutex);     // yes, remove it from the task group.
        }
        OS_MutexClr(p_mutex);
        CORE_EXIT_ATOMIC();
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      } else {
        CORE_EXIT_ATOMIC();
        RTOS_ERR_SET(*p_err, RTOS_ERR_OS_TASK_WAITING);
      }
      break;

    case OS_OPT_DEL_ALWAYS:                                     // Always delete the mutex
#if (OS_CFG_TS_EN == DEF_ENABLED)
      ts = OS_TS_GET();                                         // Get timestamp
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
      OS_MutexDbgListRemove(p_mutex);
      OSMutexQty--;
#endif
      OS_TRACE_MUTEX_DEL(p_mutex);

      p_tcb_owner = p_mutex->OwnerTCBPtr;
      if (p_tcb_owner != DEF_NULL) {                            // Does the mutex belong to a task?
        OS_MutexGrpRemove(p_tcb_owner, p_mutex);                // yes, remove it from the task group.
      }

      if (p_tcb_owner != DEF_NULL) {                            // Did we had to change the prio of owner?
        if (p_tcb_owner->Prio != p_tcb_owner->BasePrio) {
          prio_new = OS_MutexGrpPrioFindHighest(p_tcb_owner);
          prio_new = prio_new > p_tcb_owner->BasePrio ? p_tcb_owner->BasePrio : prio_new;
          OS_TaskChangePrio(p_tcb_owner, prio_new);
          OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb_owner, p_tcb_owner->Prio);
        }
      }

      OS_MutexClr(p_mutex);
      CORE_EXIT_ATOMIC();
      OSSched();                                                // Find highest priority task ready to run
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    default:
      CORE_EXIT_ATOMIC();
      OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_INVALID_ARG, 0u);
  }
  OS_TRACE_MUTEX_DEL_EXIT(RTOS_ERR_CODE_GET(*p_err));

  return (nbr_tasks);
}

/****************************************************************************************************//**
 *                                               OSMutexPend()
 *
 * @brief    This function waits for a mutex.
 *
 * @param    p_mutex     Pointer to the mutex.
 *
 * @param    timeout     Optional timeout period (in clock ticks). If non-zero, the task will wait
 *                       for the resource up to the amount of time (in 'ticks') specified by this
 *                       argument. If you specify 0,  the task will wait forever at the specified
 *                       mutex, or until the resource becomes available.
 *
 * @param    opt         Determines whether the feature to block if the mutex is available or not:
 *                           - OS_OPT_PEND_BLOCKING        Task will     block.
 *                           - OS_OPT_PEND_NON_BLOCKING    Task will NOT block.
 *
 * @param    p_ts        Pointer to a variable that will receive the timestamp of when the mutex
 *                       was posted or pend aborted or the mutex deleted. If you pass a NULL pointer
 *                       (i.e. (CPU_TS *)0), you will not get the timestamp. In other words,
 *                       passing a NULL pointer is valid and indicates that you don't need the
 *                       timestamp.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_IS_OWNER (see note (1))
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @note     (1) A mutex can be nested, so RTOS_ERR_IS_OWNER error can be used as an indicator that you
 *               are nesting the mutex. If the correct number of OSMutexPost() is done the mutex will
 *               be released.
 *******************************************************************************************************/
void OSMutexPend(OS_MUTEX *p_mutex,
                 OS_TICK  timeout,
                 OS_OPT   opt,
                 CPU_TS   *p_ts,
                 RTOS_ERR *p_err)
{
  OS_TCB *p_tcb;
  CORE_DECLARE_IRQ_STATE;

#if (OS_CFG_TS_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_ts);                                        // Prevent compiler warning for not using 'ts'
#endif

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  OS_TRACE_MUTEX_PEND_ENTER(p_mutex, timeout, opt, p_ts);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               Validate 'p_mutex'
  OS_ASSERT_DBG_ERR_SET((p_mutex != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_PEND_BLOCKING)
                         || (opt == OS_OPT_PEND_NON_BLOCKING)), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_mutex->Type == OS_OBJ_TYPE_MUTEX), *p_err, RTOS_ERR_INVALID_TYPE,; );

  //                                                               Not allowed to pend in atomic/critical sections
  OS_ASSERT_DBG_ERR_SET(( (opt & OS_OPT_PEND_NON_BLOCKING)
                          || !CORE_IrqIsDisabled()), *p_err, RTOS_ERR_INVALID_STATE,; );

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return;
  }

  CORE_ENTER_ATOMIC();
  if (p_mutex->OwnerNestingCtr == 0u) {                         // Resource available?
    p_mutex->OwnerTCBPtr = OSTCBCurPtr;                         // Yes, caller may proceed
    p_mutex->OwnerNestingCtr = 1u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    if (p_ts != DEF_NULL) {
      *p_ts = p_mutex->TS;
    }
#endif
    OS_MutexGrpAdd(OSTCBCurPtr, p_mutex);                       // Add mutex to owner's group
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    OS_TRACE_MUTEX_PEND(p_mutex);
    OS_TRACE_MUTEX_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return;
  }

  if (OSTCBCurPtr == p_mutex->OwnerTCBPtr) {                    // See if current task is already the owner of the mutex
    if (p_mutex->OwnerNestingCtr == (OS_NESTING_CTR)-1) {
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
      OS_TRACE_MUTEX_PEND_FAILED(p_mutex);
      OS_TRACE_MUTEX_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
      return;
    }
    p_mutex->OwnerNestingCtr++;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    if (p_ts != DEF_NULL) {
      *p_ts = p_mutex->TS;
    }
#endif
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_IS_OWNER);                    // Indicate that current task already owns the mutex
    OS_TRACE_MUTEX_PEND_FAILED(p_mutex);
    OS_TRACE_MUTEX_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return;
  }

  if ((opt & OS_OPT_PEND_NON_BLOCKING) != 0u) {                 // Caller wants to block if not available?
    CORE_EXIT_ATOMIC();
#if (OS_CFG_TS_EN == DEF_ENABLED)
    if (p_ts != DEF_NULL) {
      *p_ts = 0u;
    }
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);                 // No
    OS_TRACE_MUTEX_PEND_FAILED(p_mutex);
    OS_TRACE_MUTEX_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return;
  } else {
    if (OSSchedLockNestingCtr > 0u) {                           // Can't pend when the scheduler is locked
      CORE_EXIT_ATOMIC();
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = 0u;
      }
#endif
      RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
      OS_TRACE_MUTEX_PEND_FAILED(p_mutex);
      OS_TRACE_MUTEX_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
      return;
    }
  }

  p_tcb = p_mutex->OwnerTCBPtr;                                 // Point to the TCB of the Mutex owner
  if (p_tcb->Prio > OSTCBCurPtr->Prio) {                        // See if mutex owner has a lower priority than current
    OS_TaskChangePrio(p_tcb, OSTCBCurPtr->Prio);
    OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, p_tcb->Prio);
  }

  OS_Pend((OS_PEND_OBJ *)((void *)p_mutex),                     // Block task pending on Mutex
          OS_TASK_PEND_ON_MUTEX,
          timeout);

  CORE_EXIT_ATOMIC();
  OS_TRACE_MUTEX_PEND_BLOCK(p_mutex);
  OSSched();                                                    // Find the next highest priority task ready to run

  CORE_ENTER_ATOMIC();
  switch (OSTCBCurPtr->PendStatus) {
    case OS_STATUS_PEND_OK:                                     // We got the mutex
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
      }
#endif
      OS_TRACE_MUTEX_PEND(p_mutex);
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case OS_STATUS_PEND_ABORT:                                  // Indicate that we aborted
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
      }
#endif
      OS_TRACE_MUTEX_PEND_FAILED(p_mutex);
      RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);
      break;

    case OS_STATUS_PEND_TIMEOUT:                                // Indicate that we didn't get mutex within timeout
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = 0u;
      }
#endif
      OS_TRACE_MUTEX_PEND_FAILED(p_mutex);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
      break;

    case OS_STATUS_PEND_DEL:                                    // Indicate that object pended on has been deleted
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
      }
#endif
      OS_TRACE_MUTEX_PEND_FAILED(p_mutex);
      RTOS_ERR_SET(*p_err, RTOS_ERR_OS_OBJ_DEL);
      break;

    default:
      OS_TRACE_MUTEX_PEND_FAILED(p_mutex);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );
      break;
  }
  CORE_EXIT_ATOMIC();
  OS_TRACE_MUTEX_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
}

/****************************************************************************************************//**
 *                                           OSMutexPendAbort()
 *
 * @brief    Aborts and readies any tasks currently waiting on a mutex. Rather than signal the mutex
 *           via OSMutexPost(), use this function to fault-abort the wait on the mutex.
 *
 * @param    p_mutex     Pointer to the mutex.
 *
 * @param    opt         Determines the type of ABORT performed:
 *                           - OS_OPT_PEND_ABORT_1     ABORT wait for a single task (HPT) waiting
 *                                                     on the mutex.
 *                           - OS_OPT_PEND_ABORT_ALL   ABORT wait for ALL tasks that are  waiting
 *                                                     on the mutex.
 *                           - OS_OPT_POST_NO_SCHED    Do not call the scheduler.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NONE_WAITING
 *                           - RTOS_ERR_NOT_READY
 *
 * @return   == 0    If there were no tasks waiting on the mutex, or upon error.
 *           >  0    If there were one or more tasks waiting on the mutex are now ready and informed.
 *******************************************************************************************************/
OS_OBJ_QTY OSMutexPendAbort(OS_MUTEX *p_mutex,
                            OS_OPT   opt,
                            RTOS_ERR *p_err)
{
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb;
  OS_TCB       *p_tcb_owner;
  CPU_TS       ts;
  OS_OBJ_QTY   nbr_tasks;
  OS_PRIO      prio_new;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, 0u);

  //                                                               Validate 'p_mutex'
  OS_ASSERT_DBG_ERR_SET((p_mutex != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_PEND_ABORT_1)
                         || (opt == OS_OPT_PEND_ABORT_ALL)
                         || (opt == (OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED))
                         || (opt == (OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED))), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_mutex->Type == OS_OBJ_TYPE_MUTEX), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  CORE_ENTER_ATOMIC();
  p_pend_list = &p_mutex->PendList;
  if (p_pend_list->HeadPtr == DEF_NULL) {                       // Any task waiting on mutex?
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
    p_tcb_owner = p_mutex->OwnerTCBPtr;
    prio_new = p_tcb_owner->Prio;
    if ((p_tcb_owner->Prio != p_tcb_owner->BasePrio)
        && (p_tcb_owner->Prio == p_tcb->Prio)) {                // Has the owner inherited a priority?
      prio_new = OS_MutexGrpPrioFindHighest(p_tcb_owner);
      prio_new = prio_new > p_tcb_owner->BasePrio ? p_tcb_owner->BasePrio : prio_new;
    }

    if (prio_new != p_tcb_owner->Prio) {
      OS_TaskChangePrio(p_tcb_owner, prio_new);
      OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb_owner, p_tcb_owner->Prio);
    }

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
 *                                               OSMutexPost()
 *
 * @brief    Signals a mutex.
 *
 * @param    p_mutex     Pointer to the mutex.
 *
 * @param    opt         Option that alters the behavior of the post. The choices are:
 *                           - OS_OPT_POST_NONE        No special option selected.
 *                           - OS_OPT_POST_NO_SCHED    If you don't want the scheduler to be called
 *                                                     after the post.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OWNERSHIP
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_NOT_READY
 *******************************************************************************************************/
void OSMutexPost(OS_MUTEX *p_mutex,
                 OS_OPT   opt,
                 RTOS_ERR *p_err)
{
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb;
  CPU_TS       ts;
  OS_PRIO      prio_new;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  OS_TRACE_MUTEX_POST_ENTER(p_mutex, opt);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               Validate 'p_mutex'
  OS_ASSERT_DBG_ERR_SET((p_mutex != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_POST_NONE)
                         || (opt == OS_OPT_POST_NO_SCHED)), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_mutex->Type == OS_OBJ_TYPE_MUTEX), *p_err, RTOS_ERR_INVALID_TYPE,; );

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return;
  }

  CORE_ENTER_ATOMIC();
  if (OSTCBCurPtr != p_mutex->OwnerTCBPtr) {                    // Make sure the mutex owner is releasing the mutex
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_OWNERSHIP);
    OS_TRACE_MUTEX_POST_FAILED(p_mutex);
    OS_TRACE_MUTEX_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return;
  }

  OS_TRACE_MUTEX_POST(p_mutex);

#if (OS_CFG_TS_EN == DEF_ENABLED)
  ts = OS_TS_GET();                                             // Get timestamp
  p_mutex->TS = ts;
#else
  ts = 0u;
#endif
  p_mutex->OwnerNestingCtr--;                                   // Decrement owner's nesting counter
  if (p_mutex->OwnerNestingCtr > 0u) {                          // Are we done with all nestings?
    CORE_EXIT_ATOMIC();                                         // No
    RTOS_ERR_SET(*p_err, RTOS_ERR_IS_OWNER);
    OS_TRACE_MUTEX_POST_FAILED(p_mutex);
    OS_TRACE_MUTEX_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return;
  }

  OS_MutexGrpRemove(OSTCBCurPtr, p_mutex);                      // Remove mutex from owner's group

  p_pend_list = &p_mutex->PendList;
  if (p_pend_list->HeadPtr == DEF_NULL) {                       // Any task waiting on mutex?
    p_mutex->OwnerTCBPtr = DEF_NULL;                            // No
    p_mutex->OwnerNestingCtr = 0u;
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    OS_TRACE_MUTEX_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return;
  }
  //                                                               Yes
  if (OSTCBCurPtr->Prio != OSTCBCurPtr->BasePrio) {             // Has owner inherited a priority?
    prio_new = OS_MutexGrpPrioFindHighest(OSTCBCurPtr);         // Yes, find highest priority pending
    prio_new = prio_new > OSTCBCurPtr->BasePrio ? OSTCBCurPtr->BasePrio : prio_new;
    if (prio_new > OSTCBCurPtr->Prio) {
      OS_RdyListRemove(OSTCBCurPtr);
      OSTCBCurPtr->Prio = prio_new;                             // Lower owner's priority back to its original one
      OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(OSTCBCurPtr, prio_new);
      OS_PrioInsert(prio_new);
      OS_RdyListInsertTail(OSTCBCurPtr);                        // Insert owner in ready list at new priority
      OSPrioCur = prio_new;
    }
  }
  //                                                               Get TCB from head of pend list
  p_tcb = p_pend_list->HeadPtr;
  p_mutex->OwnerTCBPtr = p_tcb;                                 // Give mutex to new owner
  p_mutex->OwnerNestingCtr = 1u;
  OS_MutexGrpAdd(p_tcb, p_mutex);
  //                                                               Post to mutex
  OS_Post((OS_PEND_OBJ *)((void *)p_mutex),
          p_tcb,
          DEF_NULL,
          0u,
          ts);

  CORE_EXIT_ATOMIC();

  if ((opt & OS_OPT_POST_NO_SCHED) == 0u) {
    OSSched();                                                  // Run the scheduler
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  OS_TRACE_MUTEX_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               OS_MutexClr()
 *
 * @brief    This function is called by OSMutexDel() to clear the contents of a mutex.
 *
 * @param    p_mutex     Pointer to the mutex to clear.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_MutexClr(OS_MUTEX *p_mutex)
{
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  p_mutex->Type = OS_OBJ_TYPE_NONE;                             // Mark the data structure as a NONE
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_mutex->NamePtr = (CPU_CHAR *)((void *)"?MUTEX");
#endif
  p_mutex->MutexGrpNextPtr = DEF_NULL;
  p_mutex->OwnerTCBPtr = DEF_NULL;
  p_mutex->OwnerNestingCtr = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
  p_mutex->TS = 0u;
#endif
  OS_PendListInit(&p_mutex->PendList);                          // Initialize the waiting list
}

/****************************************************************************************************//**
 *                                           OS_MutexDbgListAdd()
 *
 * @brief    Add a mutex to the mutex debug list.
 *
 * @param    p_mutex     Pointer to the mutex to add.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_MutexDbgListAdd(OS_MUTEX *p_mutex)
{
  p_mutex->DbgNamePtr = (CPU_CHAR *)((void *)" ");
  p_mutex->DbgPrevPtr = DEF_NULL;
  if (OSMutexDbgListPtr == DEF_NULL) {
    p_mutex->DbgNextPtr = DEF_NULL;
  } else {
    p_mutex->DbgNextPtr = OSMutexDbgListPtr;
    OSMutexDbgListPtr->DbgPrevPtr = p_mutex;
  }
  OSMutexDbgListPtr = p_mutex;
}

/****************************************************************************************************//**
 *                                           OS_MutexDbgListRemove()
 *
 * @brief    Remove a mutex from the mutex debug list.
 *
 * @param    p_mutex     Pointer to the mutex to remove.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_MutexDbgListRemove(OS_MUTEX *p_mutex)
{
  OS_MUTEX *p_mutex_next;
  OS_MUTEX *p_mutex_prev;

  p_mutex_prev = p_mutex->DbgPrevPtr;
  p_mutex_next = p_mutex->DbgNextPtr;

  if (p_mutex_prev == DEF_NULL) {
    OSMutexDbgListPtr = p_mutex_next;
    if (p_mutex_next != DEF_NULL) {
      p_mutex_next->DbgPrevPtr = DEF_NULL;
    }
    p_mutex->DbgNextPtr = DEF_NULL;
  } else if (p_mutex_next == DEF_NULL) {
    p_mutex_prev->DbgNextPtr = DEF_NULL;
    p_mutex->DbgPrevPtr = DEF_NULL;
  } else {
    p_mutex_prev->DbgNextPtr = p_mutex_next;
    p_mutex_next->DbgPrevPtr = p_mutex_prev;
    p_mutex->DbgNextPtr = DEF_NULL;
    p_mutex->DbgPrevPtr = DEF_NULL;
  }
}
#endif

/****************************************************************************************************//**
 *                                               OS_MutexGrpAdd()
 *
 * @brief    This function is called by the Kernel to add a mutex to a task's mutex group.
 *
 * @param    p_tcb       Pointer to the tcb of the task to give the mutex to.
 *
 * @param    p_mutex     Pointer to the mutex to add to the group.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_MutexGrpAdd(OS_TCB   *p_tcb,
                    OS_MUTEX *p_mutex)
{
  p_mutex->MutexGrpNextPtr = p_tcb->MutexGrpHeadPtr;            // The mutex grp is not sorted add to head of list.
  p_tcb->MutexGrpHeadPtr = p_mutex;
}

/****************************************************************************************************//**
 *                                           OS_MutexGrpRemove()
 *
 * @brief    This function is called by the Kernel to remove a mutex to a task's mutex group.
 *
 * @param    p_tcb       Pointer to the tcb of the task to remove the mutex from.
 *
 * @param    p_mutex     Pointer to the mutex to remove from the group.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_MutexGrpRemove(OS_TCB   *p_tcb,
                       OS_MUTEX *p_mutex)
{
  OS_MUTEX **pp_mutex;

  pp_mutex = &p_tcb->MutexGrpHeadPtr;

  while (*pp_mutex != p_mutex) {
    pp_mutex = &(*pp_mutex)->MutexGrpNextPtr;
  }

  *pp_mutex = (*pp_mutex)->MutexGrpNextPtr;
}

/****************************************************************************************************//**
 *                                       OS_MutexGrpPrioFindHighest()
 *
 * @brief    This function is called by the Kernel to find the highest task pending on any mutex from
 *           a group.
 *
 * @param    p_tcb   Pointer to the TCB of the task to process.
 *
 * @return   Highest priority pending or OS_CFG_PRIO_MAX - 1u if none found.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
OS_PRIO OS_MutexGrpPrioFindHighest(OS_TCB *p_tcb)
{
  OS_MUTEX **pp_mutex;
  OS_PRIO  highest_prio;
  OS_PRIO  prio;
  OS_TCB   *p_head;

  highest_prio = OS_CFG_PRIO_MAX - 1u;
  pp_mutex = &p_tcb->MutexGrpHeadPtr;

  while (*pp_mutex != DEF_NULL) {
    p_head = (*pp_mutex)->PendList.HeadPtr;
    if (p_head != DEF_NULL) {
      prio = p_head->Prio;
      if (prio < highest_prio) {
        highest_prio = prio;
      }
    }
    pp_mutex = &(*pp_mutex)->MutexGrpNextPtr;
  }

  return (highest_prio);
}

/****************************************************************************************************//**
 *                                           OS_MutexGrpPostAll()
 *
 * @brief    This function is called by the Kernel to post (release) all the mutex from a group. Used
 *           when deleting a task.
 *
 * @param    p_tcb   Pointer to the TCB of the task to process.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_MutexGrpPostAll(OS_TCB *p_tcb)
{
  OS_MUTEX     *p_mutex;
  OS_MUTEX     *p_mutex_next;
  CPU_TS       ts;
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb_new;

  p_mutex = p_tcb->MutexGrpHeadPtr;

  while (p_mutex != DEF_NULL) {
    OS_TRACE_MUTEX_POST(p_mutex);

    p_mutex_next = p_mutex->MutexGrpNextPtr;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    ts = OS_TS_GET();                                           // Get timestamp
    p_mutex->TS = ts;
#else
    ts = 0u;
#endif
    OS_MutexGrpRemove(p_tcb, p_mutex);                          // Remove mutex from owner's group

    p_pend_list = &p_mutex->PendList;
    if (p_pend_list->HeadPtr == DEF_NULL) {                     // Any task waiting on mutex?
      p_mutex->OwnerNestingCtr = 0u;                            // Decrement owner's nesting counter
      p_mutex->OwnerTCBPtr = DEF_NULL;                          // No
    } else {
      //                                                           Get TCB from head of pend list
      p_tcb_new = p_pend_list->HeadPtr;
      p_mutex->OwnerTCBPtr = p_tcb;                             // Give mutex to new owner
      p_mutex->OwnerNestingCtr = 1u;
      OS_MutexGrpAdd(p_tcb_new, p_mutex);
      //                                                           Post to mutex
      OS_Post((OS_PEND_OBJ *)((void *)p_mutex),
              p_tcb_new,
              DEF_NULL,
              0u,
              ts);
    }

    p_mutex = p_mutex_next;
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
