/***************************************************************************//**
 * @file
 * @brief Kernel - Task Management
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
const CPU_CHAR *os_task__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if  (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
OS_EXT sl_sleeptimer_timer_handle_t OSRoundRobinTimer;
#endif

/****************************************************************************************************//**
 *                                           OSTaskChangePrio()
 *
 * @brief    Allows you to dynamically change the priority of a task. Note that the new
 *           priority MUST be available.
 *
 * @param    p_tcb       Pointer to the TCB of the task for which to change the priority.
 *
 * @param    prio_new    The new priority.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void OSTaskChangePrio(OS_TCB   *p_tcb,
                      OS_PRIO  prio_new,
                      RTOS_ERR *p_err)
{
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  OS_PRIO prio_high;
#endif
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  OS_ASSERT_DBG_ERR_SET(((p_tcb == DEF_NULL)
                         || (p_tcb->TaskState != OS_TASK_STATE_DEL)), *p_err, RTOS_ERR_INVALID_STATE,; );

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  if (prio_new >= OS_CFG_PRIO_MAX) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  CORE_ENTER_ATOMIC();
  if (p_tcb == DEF_NULL) {                                      // Are we changing the priority of 'self'?
    if (OSRunning != OS_STATE_OS_RUNNING) {
      CORE_EXIT_ATOMIC();
      OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_NOT_READY,; )
    }
    p_tcb = OSTCBCurPtr;
  }

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  p_tcb->BasePrio = prio_new;                                   // Update base priority

  if (p_tcb->MutexGrpHeadPtr != DEF_NULL) {                     // Owning a mutex?
    if (prio_new > p_tcb->Prio) {
      prio_high = OS_MutexGrpPrioFindHighest(p_tcb);
      if (prio_new > prio_high) {
        prio_new = prio_high;
      }
    }
  }
#endif

  OS_TaskChangePrio(p_tcb, prio_new);

  OS_TRACE_TASK_PRIO_CHANGE(p_tcb, prio_new);

  CORE_EXIT_ATOMIC();

  if (OSRunning == OS_STATE_OS_RUNNING) {
    OSSched();                                                  // Run highest priority task ready
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               OSTaskCreate()
 *
 * @brief    Allows the Kernel to manage the execution of a task. Tasks can either
 *           be created prior to the start of multitasking or by a running task. A task cannot be
 *           created by an ISR.
 *
 * @param    p_tcb           Pointer to the task's TCB.
 *
 * @param    p_name          Pointer to an ASCII string that provides a name for the task.
 *
 * @param    p_task          Pointer to the task's code.
 *
 * @param    p_arg           Pointer to an optional data area which can pass parameters to the task
 *                           when the task executes. For the task, it believes it was invoked and
 *                           passed the argument 'p_arg' as follows:
 *                           @verbatim
 *                           void Task (void *p_arg)
 *                           {
 *                               for (;;) {
 *                                   Task code;
 *                               }
 *                           }
 *                           @endverbatim
 *
 * @param    prio            The task's priority. A unique priority MUST be assigned to each task.
 *                           The lower the number, the higher the priority.
 *
 * @param    p_stk_base      Pointer to the base address of the stack (i.e. low address).
 *
 * @param    stk_limit       The number of stack elements to set as 'watermark' limits for the stack.
 *                           This value represents the number of CPU_STK entries left before the stack
 *                           is full. For example, specifying 10% of the 'stk_size' value indicates that
 *                           the stack limit will be reached when the stack reaches 90% full.
 *
 * @param    stk_size        The size of the stack in number of elements. If CPU_STK is set to
 *                           CPU_INT08U, the 'stk_size' corresponds to the number of bytes available.
 *                           If CPU_STK is set to CPU_INT16U, the 'stk_size' contains the number of
 *                           16-bit entries available. Finally, if CPU_STK is set to CPU_INT32U, the
 *                           'stk_size' contains the number of 32-bit entries available on the stack.
 *
 * @param    q_size          The maximum number of messages that can be sent to the task.
 *
 * @param    time_quanta     Amount of time (in ticks) for a time slice when the round-robin between
 *                           tasks. Set to 0 to use the default.
 *
 * @param    p_ext           Pointer to a user-supplied memory location which is used as a TCB
 *                           extension. For example, this user memory can hold the contents of
 *                           floating-point registers during a context switch, the time each task takes
 *                           to execute, the number of times the task has been switched-in, etc.
 *
 * @param    opt             Contains additional information (or options) about the behavior of the task.
 *                           See OS_OPT_TASK_xxx in OS.H. Current choices are:
 *                               - OS_OPT_TASK_NONE        No option selected.
 *                               - OS_OPT_TASK_STK_CHK     Stack checking to be allowed for the task.
 *                               - OS_OPT_TASK_STK_CLR     Clear the stack when the task is created.
 *                               - OS_OPT_TASK_SAVE_FP     If the CPU has floating-point registers,
 *                                                         save them during a context switch.
 *                               - OS_OPT_TASK_NO_TLS      If the caller does not want or need TLS
 *                           (Thread Local Storage) support for the task.
 *                           If you do not include this option, TLS will
 *                           be supported by default.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *
 * @note     (1) OSTaskCreate() triggers a critical assert when a stack overflow is detected during
 *               stack initialization. In this case, some memory may have been corrupted and should
 *               be treated as a fatal error.
 *******************************************************************************************************/
void OSTaskCreate(OS_TCB       *p_tcb,
                  CPU_CHAR     *p_name,
                  OS_TASK_PTR  p_task,
                  void         *p_arg,
                  OS_PRIO      prio,
                  CPU_STK      *p_stk_base,
                  CPU_STK_SIZE stk_limit,
                  CPU_STK_SIZE stk_size,
                  OS_MSG_QTY   q_size,
                  OS_TICK      time_quanta,
                  void         *p_ext,
                  OS_OPT       opt,
                  RTOS_ERR     *p_err)
{
  CPU_STK_SIZE i;
#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
  OS_REG_ID reg_nbr;
#endif
#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
  OS_TLS_ID id;
#endif

  CPU_STK *p_sp;
  CPU_STK *p_stk_limit;
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

  //                                                               User must supply a valid OS_TCB
  OS_ASSERT_DBG_ERR_SET((p_tcb != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               User must supply a valid task
  OS_ASSERT_DBG_ERR_SET((p_task != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               User must supply a valid stack base address
  OS_ASSERT_DBG_ERR_SET((p_stk_base != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               User must supply a valid minimum stack size
  OS_ASSERT_DBG_ERR_SET((stk_size >= OSCfg_StkSizeMin), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               User must supply a valid stack limit
  OS_ASSERT_DBG_ERR_SET((stk_limit < stk_size), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Priority must be within 0 and OS_CFG_PRIO_MAX-1
  OS_ASSERT_DBG_ERR_SET((prio < OS_CFG_PRIO_MAX), *p_err, RTOS_ERR_INVALID_ARG,; );

#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
  //                                                               Q size must be within 0 and OSCfg_MsgPoolSize
  OS_ASSERT_DBG_ERR_SET((q_size <= OSCfg_MsgPoolSize), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  OS_TaskInitTCB(p_tcb);                                        // Initialize the TCB to default values

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               -------------- CLEAR THE TASK'S STACK --------------
  if (((opt & OS_OPT_TASK_STK_CHK) != 0u)                       // See if stack checking has been enabled
      || ((opt & OS_OPT_TASK_STK_CLR) != 0u)) {                 // See if stack needs to be cleared
    if ((opt & OS_OPT_TASK_STK_CLR) != 0u) {
      p_sp = p_stk_base;
      for (i = 0u; i < stk_size; i++) {                         // Stack grows from HIGH to LOW memory
        *p_sp = 0u;                                             // Clear from bottom of stack and up!
        p_sp++;
      }
    }
  }
  //                                                               ------ INITIALIZE THE STACK FRAME OF THE TASK ------
  stk_limit &= ~(CPU_CFG_STK_ALIGN_BYTES - 1u);                 // Align stack limit.
#if (CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO)
  p_stk_limit = p_stk_base + stk_limit;
#else
  p_stk_limit = p_stk_base + (stk_size - 1u) - stk_limit;
#endif

  p_sp = OSTaskStkInit(p_task,
                       p_arg,
                       p_stk_base,
                       p_stk_limit,
                       stk_size,
                       opt);

#if (CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO)             // Check if we overflown the stack during init
  RTOS_ASSERT_CRITICAL_ERR_SET((p_sp >= p_stk_base), *p_err, RTOS_ERR_OS,; );
#else
  RTOS_ASSERT_CRITICAL_ERR_SET((p_sp <= (p_stk_base + stk_size)), *p_err, RTOS_ERR_OS,; );
#endif

#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)                 // Initialize Redzoned stack
  OS_TaskStkRedzoneInit(p_stk_base, stk_size);
#endif

  //                                                               ------------ INITIALIZE THE TCB FIELDS -------------
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_tcb->TaskEntryAddr = p_task;                                // Save task entry point address
  p_tcb->TaskEntryArg = p_arg;                                  // Save task entry argument
#endif

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_tcb->NamePtr = p_name;                                      // Save task name
#else
  (void)&p_name;
#endif

  p_tcb->Prio = prio;                                           // Save the task's priority

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  p_tcb->BasePrio = prio;                                       // Set the base priority
#endif

  p_tcb->StkPtr = p_sp;                                         // Save the new top-of-stack pointer
#if ((OS_CFG_DBG_EN == DEF_ENABLED) || (OS_CFG_STAT_TASK_STK_CHK_EN == DEF_ENABLED))
  p_tcb->StkLimitPtr = p_stk_limit;                             // Save the stack limit pointer
#endif

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
  p_tcb->TimeQuanta = time_quanta;                              // Save the #ticks for time slice (0 means not sliced)
  if (time_quanta == 0u) {
    p_tcb->TimeQuantaCtr = (uint64_t)(((uint64_t)OSSchedRoundRobinDfltTimeQuanta * (uint64_t)sl_sleeptimer_get_timer_frequency()) + (OSCfg_TickRate_Hz - 1u)) / OSCfg_TickRate_Hz;
  } else {
    p_tcb->TimeQuantaCtr = (uint64_t)(((uint64_t)p_tcb->TimeQuanta * (uint64_t)sl_sleeptimer_get_timer_frequency()) + (OSCfg_TickRate_Hz - 1u)) / OSCfg_TickRate_Hz;
  }
#else
  (void)&time_quanta;
#endif

  p_tcb->ExtPtr = p_ext;                                        // Save pointer to TCB extension
  p_tcb->StkBasePtr = p_stk_base;                               // Save pointer to the base address of the stack
  p_tcb->StkSize = stk_size;                                    // Save the stack size (in number of CPU_STK elements)
  p_tcb->Opt = opt;                                             // Save task options

#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
  for (reg_nbr = 0u; reg_nbr < OS_CFG_TASK_REG_TBL_SIZE; reg_nbr++) {
    p_tcb->RegTbl[reg_nbr] = 0u;
  }
#endif

#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
  OS_MsgQInit(&p_tcb->MsgQ,                                     // Initialize the task's message queue
              q_size);
#else
  (void)&q_size;
#endif

  OSTaskCreateHook(p_tcb);                                      // Call user defined hook

  OS_TRACE_TASK_CREATE(p_tcb);
  OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name);
#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
  OS_TRACE_TASK_MSG_Q_CREATE(&p_tcb->MsgQ, p_name);
#endif

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
  for (id = 0u; id < OS_CFG_TLS_TBL_SIZE; id++) {
    p_tcb->TLS_Tbl[id] = 0u;
  }
  OS_TLS_TaskCreate(p_tcb);                                     // Call TLS hook
#endif
  //                                                               -------------- ADD TASK TO READY LIST --------------
  CORE_ENTER_ATOMIC();
  OS_PrioInsert(p_tcb->Prio);
  OS_RdyListInsertTail(p_tcb);

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_TaskDbgListAdd(p_tcb);
#endif

  OSTaskQty++;                                                  // Increment the #tasks counter

  if (OSRunning != OS_STATE_OS_RUNNING) {                       // Return if multitasking has not started
    CORE_EXIT_ATOMIC();
    return;
  }

  CORE_EXIT_ATOMIC();

  OSSched();
}

/****************************************************************************************************//**
 *                                               OSTaskDel()
 *
 * @brief    Allows you to delete a task. The calling task can delete itself by specifying a NULL
 *           pointer for 'p_tcb'. The deleted task is returned to the dormant state and can
 *           be re-activated by creating the deleted task again.
 *
 * @param    p_tcb   Pointer to the TCB of the task to delete.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_INVALID_ARG
 *                       - RTOS_ERR_INVALID_STATE
 *                       - RTOS_ERR_NOT_READY
 *
 * @note     (1) 'p_err' is set to RTOS_ERR_NONE before OSSched() to allow the returned err or code
 *               to be monitored even for a task that is deleting itself. In this case, 'p_err' MUST
 *               point to a global variable that can be accessed by another task.
 *******************************************************************************************************/
#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
void OSTaskDel(OS_TCB   *p_tcb,
               RTOS_ERR *p_err)
{
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  OS_TCB  *p_tcb_owner;
  OS_PRIO prio_new;
#endif
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  sl_status_t status;
#endif
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

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return;
  }

  if (p_tcb == DEF_NULL) {                                      // Delete 'Self'?
    CORE_ENTER_ATOMIC();
    p_tcb = OSTCBCurPtr;                                        // Yes.
    CORE_EXIT_ATOMIC();
  }

  CORE_ENTER_ATOMIC();
  switch (p_tcb->TaskState) {
    case OS_TASK_STATE_RDY:
      OS_RdyListRemove(p_tcb);
      break;

    case OS_TASK_STATE_SUSPENDED:
      break;

    case OS_TASK_STATE_DLY:                                     // Task is only delayed, not on any wait list
    case OS_TASK_STATE_DLY_SUSPENDED:
#if (OS_CFG_TICK_EN == DEF_ENABLED)
      status = sl_sleeptimer_stop_timer(&p_tcb->TimerHandle);
      RTOS_ASSERT_CRITICAL((status == SL_STATUS_OK), RTOS_ERR_FAIL,; );
#endif
      break;

    case OS_TASK_STATE_PEND:
    case OS_TASK_STATE_PEND_SUSPENDED:
    case OS_TASK_STATE_PEND_TIMEOUT:
    case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
      switch (p_tcb->PendOn) {                                  // See what we are pending on
        case OS_TASK_PEND_ON_NOTHING:
        case OS_TASK_PEND_ON_TASK_Q:                            // There is no wait list for these two
        case OS_TASK_PEND_ON_TASK_SEM:
          break;

        case OS_TASK_PEND_ON_FLAG:                              // Remove from pend list
        case OS_TASK_PEND_ON_Q:
        case OS_TASK_PEND_ON_SEM:
        case OS_TASK_PEND_ON_COND_VAR:
          OS_PendListRemove(p_tcb);
          break;

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
        case OS_TASK_PEND_ON_MUTEX:
          p_tcb_owner = ((OS_MUTEX *)p_tcb->PendObjPtr)->OwnerTCBPtr;
          prio_new = p_tcb_owner->Prio;
          OS_PendListRemove(p_tcb);
          if ((p_tcb_owner->Prio != p_tcb_owner->BasePrio)
              && (p_tcb_owner->Prio == p_tcb->Prio)) {          // Has the owner inherited a priority?
            prio_new = OS_MutexGrpPrioFindHighest(p_tcb_owner);
            prio_new = prio_new > p_tcb_owner->BasePrio ? p_tcb_owner->BasePrio : prio_new;
          }
          p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING;

          if (prio_new != p_tcb_owner->Prio) {
            OS_TaskChangePrio(p_tcb_owner, prio_new);
            OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb_owner, p_tcb_owner->Prio);
          }
          break;
#endif

        default:
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );
          break;
      }
#if (OS_CFG_TICK_EN == DEF_ENABLED)
      if ((p_tcb->TaskState == OS_TASK_STATE_PEND_TIMEOUT)
          || (p_tcb->TaskState == OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED)) {
        status = sl_sleeptimer_stop_timer(&p_tcb->TimerHandle);
        RTOS_ASSERT_CRITICAL((status == SL_STATUS_OK), RTOS_ERR_FAIL,; );
      }
#endif
      break;

    case OS_TASK_STATE_DEL:
#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      return;
      break;                                                    // The break helps avoid fall-through false errors
#endif

    default:
      CORE_EXIT_ATOMIC();
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
      return;
  }

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  if (p_tcb->MutexGrpHeadPtr != DEF_NULL) {
    OS_MutexGrpPostAll(p_tcb);
  }
#endif

#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
  (void)OS_MsgQFreeAll(&p_tcb->MsgQ);                           // Free task's message queue messages
#endif

  OSTaskDelHook(p_tcb);                                         // Call user defined hook

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
  OS_TLS_TaskDel(p_tcb);                                        // Call TLS hook
#endif

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_TaskDbgListRemove(p_tcb);
#endif

  OSTaskQty--;                                                  // One less task being managed

  OS_TRACE_TASK_DEL(p_tcb);

#if (OS_CFG_TASK_STK_REDZONE_EN != DEF_ENABLED)                 // Don't clear the TCB before checking the red-zone
  OS_TaskInitTCB(p_tcb);                                        // Initialize the TCB to default values
#endif
  p_tcb->TaskState = (OS_STATE)OS_TASK_STATE_DEL;               // Indicate that the task was deleted

  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                          // See Note #1.

  OSSched();                                                    // Find new highest priority task
}
#endif

/****************************************************************************************************//**
 *                                               OSTaskQFlush()
 *
 * @brief    Flushes the task's internal message queue.
 *
 * @param    p_tcb   Pointer to the task's TCB. Specifying a NULL pointer indicates that
 *                   you wish to flush the message queue of the calling task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   The number of entries freed from the queue.
 *
 * @note     (1) Use this function with great care. When you flush the queue, you lose the references
 *               to what the queue entries are pointing, which can cause 'memory leaks'.
 *               In other words, the data being pointed to that is referenced by the queue entries
 *               should, most likely, need to be de-allocated (i.e. freed).
 *******************************************************************************************************/
#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
OS_MSG_QTY OSTaskQFlush(OS_TCB   *p_tcb,
                        RTOS_ERR *p_err)
{
  OS_MSG_QTY entries;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  if (p_tcb == DEF_NULL) {                                      // Flush message queue of calling task?
    CORE_ENTER_ATOMIC();
    p_tcb = OSTCBCurPtr;
    CORE_EXIT_ATOMIC();
  }

  CORE_ENTER_ATOMIC();
  entries = OS_MsgQFreeAll(&p_tcb->MsgQ);                       // Return all OS_MSGs to the OS_MSG pool
  CORE_EXIT_ATOMIC();
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (entries);
}
#endif

/****************************************************************************************************//**
 *                                               OSTaskQPend()
 *
 * @brief    This function causes the current task to wait for a message to be posted to it.
 *
 * @param    timeout     Optional timeout period (in clock ticks). If non-zero, your task will
 *                       wait for a message to arrive up to the amount of time specified by this
 *                       argument. If you specify 0, your task will wait forever, or until a
 *                       message arrives.
 *
 * @param    opt         Determines if the user wants to block if the task's queue is empty or
 *                       not:
 *                           - OS_OPT_PEND_BLOCKING        Task will     block.
 *                           - OS_OPT_PEND_NON_BLOCKING    Task will NOT block.
 *
 * @param    p_msg_size  Pointer to a variable that will receive the size of the message.
 *
 * @param    p_ts        Pointer to a variable that will receive the timestamp of when the
 *                       message was received. If you pass a NULL pointer (i.e. (CPU_TS *)0), you
 *                       will not get the timestamp. In other words, passing a NULL pointer is valid
 *                       and indicates that you don't need the timestamp.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_NOT_READY
 *
 * @return   A pointer to the message received or a NULL pointer upon error.
 *******************************************************************************************************/
#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
void *OSTaskQPend(OS_TICK     timeout,
                  OS_OPT      opt,
                  OS_MSG_SIZE *p_msg_size,
                  CPU_TS      *p_ts,
                  RTOS_ERR    *p_err)
{
  OS_MSG_Q *p_msg_q;
  void     *p_void;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  OS_TRACE_TASK_MSG_Q_PEND_ENTER(&OSTCBCurPtr->MsgQ, timeout, opt, p_msg_size, p_ts);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, DEF_NULL);

  //                                                               User must supply a valid destination for msg size
  OS_ASSERT_DBG_ERR_SET((p_msg_size != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_PEND_BLOCKING)
                         || (opt == OS_OPT_PEND_NON_BLOCKING)), *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (DEF_NULL);
  }

  if (p_ts != DEF_NULL) {
    *p_ts = 0u;                                                 // Initialize the returned timestamp
  }

  CORE_ENTER_ATOMIC();
  p_msg_q = &OSTCBCurPtr->MsgQ;                                 // Any message waiting in the message queue?
  p_void = OS_MsgQGet(p_msg_q,
                      p_msg_size,
                      p_ts,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
#if (OS_CFG_TS_EN == DEF_ENABLED)
    if (p_ts != DEF_NULL) {
      OSTCBCurPtr->MsgQPendTime = OS_TS_GET() - *p_ts;
      if (OSTCBCurPtr->MsgQPendTimeMax < OSTCBCurPtr->MsgQPendTime) {
        OSTCBCurPtr->MsgQPendTimeMax = OSTCBCurPtr->MsgQPendTime;
      }
    }
#endif
#endif
    CORE_EXIT_ATOMIC();
    OS_TRACE_TASK_MSG_Q_PEND(p_msg_q);
    OS_TRACE_TASK_MSG_Q_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return (p_void);                                            // Yes, Return oldest message received
  }

  if ((opt & OS_OPT_PEND_NON_BLOCKING) != 0u) {                 // Caller wants to block if not available?
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);                 // No
    CORE_EXIT_ATOMIC();
    OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q);
    OS_TRACE_TASK_MSG_Q_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return (DEF_NULL);
  } else {                                                      // Yes
    if (OSSchedLockNestingCtr > 0u) {                           // Can't block when the scheduler is locked
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
      OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q);
      OS_TRACE_TASK_MSG_Q_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
      return (DEF_NULL);
    }
  }

  OS_Pend(DEF_NULL,                                             // Block task pending on Message
          OS_TASK_PEND_ON_TASK_Q,
          timeout);
  CORE_EXIT_ATOMIC();
  OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q);
  OSSched();                                                    // Find the next highest priority task ready to run

  CORE_ENTER_ATOMIC();
  switch (OSTCBCurPtr->PendStatus) {
    case OS_STATUS_PEND_OK:                                     // Extract message from TCB (Put there by Post)
      p_void = OSTCBCurPtr->MsgPtr;
      *p_msg_size = OSTCBCurPtr->MsgSize;
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
        OSTCBCurPtr->MsgQPendTime = OS_TS_GET() - OSTCBCurPtr->TS;
        if (OSTCBCurPtr->MsgQPendTimeMax < OSTCBCurPtr->MsgQPendTime) {
          OSTCBCurPtr->MsgQPendTimeMax = OSTCBCurPtr->MsgQPendTime;
        }
#endif
      }
#endif
      OS_TRACE_TASK_MSG_Q_PEND(p_msg_q);
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case OS_STATUS_PEND_ABORT:                                  // Indicate that we aborted
      p_void = DEF_NULL;
      *p_msg_size = 0u;
      if (p_ts != DEF_NULL) {
        *p_ts = 0u;
      }
      OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q);
      RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);
      break;

    case OS_STATUS_PEND_TIMEOUT:                                // Indicate that we didn't get event within TO
      p_void = DEF_NULL;
      *p_msg_size = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
      }
#endif
      OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
      break;

    case OS_STATUS_PEND_DEL:
    default:
      OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, DEF_NULL);
  }
  CORE_EXIT_ATOMIC();
  OS_TRACE_TASK_MSG_Q_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
  return (p_void);                                              // Return received message
}
#endif

/****************************************************************************************************//**
 *                                           OSTaskQPendAbort()
 *
 * @brief    Aborts and readies the task specified. Use this function to fault-abort the wait for a
 *           message, rather than to normally post the message to the task via OSTaskQPost().
 *
 * @param    p_tcb   Pointer to the TCB of the task to pend abort.
 *
 * @param    opt     Provides options for this function:
 *                       - OS_OPT_POST_NONE        No option specified.
 *                       - OS_OPT_POST_NO_SCHED    Indicates that the scheduler will not be called.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NONE_WAITING
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   == DEF_FALSE    If task was not waiting for a message, or upon error.
 *           == DEF_TRUE     If task was waiting for a message and was readied and informed.
 *******************************************************************************************************/
#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
CPU_BOOLEAN OSTaskQPendAbort(OS_TCB   *p_tcb,
                             OS_OPT   opt,
                             RTOS_ERR *p_err)
{
  CPU_TS ts;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FALSE);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, DEF_FALSE);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_POST_NONE)
                         || (opt == OS_OPT_POST_NO_SCHED)), *p_err, RTOS_ERR_INVALID_ARG, DEF_FALSE);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (DEF_FALSE);
  }

  CORE_ENTER_ATOMIC();
#if (OS_ARG_CHK_EN == DEF_ENABLED)
  if ((p_tcb == DEF_NULL)                                       // Pend abort self?
      || (p_tcb == OSTCBCurPtr)) {
    CORE_EXIT_ATOMIC();                                         // ... doesn't make sense
    OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_INVALID_ARG, DEF_FALSE);
  }
#endif

  if (p_tcb->PendOn != OS_TASK_PEND_ON_TASK_Q) {                // Is task waiting for a message?
    CORE_EXIT_ATOMIC();                                         // No
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE_WAITING);
    return (DEF_FALSE);
  }

#if (OS_CFG_TS_EN == DEF_ENABLED)
  ts = OS_TS_GET();                                             // Get timestamp of when the abort occurred
#else
  ts = 0u;
#endif
  OS_PendAbort(p_tcb,                                           // Abort the pend
               ts,
               OS_STATUS_PEND_ABORT);
  CORE_EXIT_ATOMIC();
  if ((opt & OS_OPT_POST_NO_SCHED) == 0u) {
    OSSched();                                                  // Run the scheduler
  }
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (DEF_TRUE);
}
#endif

/****************************************************************************************************//**
 *                                               OSTaskQPost()
 *
 * @brief    Sends a message to a task.
 *
 * @param    p_tcb       Pointer to the TCB of the task receiving a message. If you specify a NULL
 *                       pointer, the message will be posted to the task's queue of the calling task.
 *                       In other words, you'd be posting a message to yourself.
 *
 * @param    p_void      Pointer to the message to send.
 *
 * @param    msg_size    The size of the message sent (in bytes).
 *
 * @param    opt         Specifies whether the post will be FIFO or LIFO:
 *                           - OS_OPT_POST_FIFO        Post at the end   of the queue.
 *                           - OS_OPT_POST_LIFO        Post at the front of the queue.
 *                           - OS_OPT_POST_NO_SCHED    Do not run the scheduler after the post.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_NO_MORE_RSRC
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_NOT_READY
 *
 * @note     (1) OS_OPT_POST_NO_SCHED can be OR'ed with one of the other two options to prevent the
 *               scheduler from being called.
 *
 * @note     (2) This function may be called from an ISR.
 *******************************************************************************************************/
#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
void OSTaskQPost(OS_TCB      *p_tcb,
                 void        *p_void,
                 OS_MSG_SIZE msg_size,
                 OS_OPT      opt,
                 RTOS_ERR    *p_err)
{
  CPU_TS ts;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  OS_TRACE_TASK_MSG_Q_POST_ENTER(&p_tcb->MsgQ, p_void, msg_size, opt);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_POST_FIFO)
                         || (opt == OS_OPT_POST_LIFO)
                         || (opt == (OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED))
                         || (opt == (OS_OPT_POST_LIFO | OS_OPT_POST_NO_SCHED))), *p_err, RTOS_ERR_INVALID_ARG,; );

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

  OS_TRACE_TASK_MSG_Q_POST(&p_tcb->MsgQ);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                          // Assume we won't have any errors
  CORE_ENTER_ATOMIC();
  if (p_tcb == DEF_NULL) {                                      // Post msg to 'self'?
    p_tcb = OSTCBCurPtr;
  }
  switch (p_tcb->TaskState) {
    case OS_TASK_STATE_RDY:
    case OS_TASK_STATE_DLY:
    case OS_TASK_STATE_SUSPENDED:
    case OS_TASK_STATE_DLY_SUSPENDED:
      OS_MsgQPut(&p_tcb->MsgQ,                                  // Deposit the message in the queue
                 p_void,
                 msg_size,
                 opt,
                 ts,
                 p_err);
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_PEND:
    case OS_TASK_STATE_PEND_TIMEOUT:
    case OS_TASK_STATE_PEND_SUSPENDED:
    case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
      if (p_tcb->PendOn == OS_TASK_PEND_ON_TASK_Q) {            // Is task waiting for a message to be sent to it?
        OS_Post(DEF_NULL,
                p_tcb,
                p_void,
                msg_size,
                ts);
        CORE_EXIT_ATOMIC();
        if ((opt & OS_OPT_POST_NO_SCHED) == 0u) {
          OSSched();                                            // Run the scheduler
        }
      } else {
        OS_MsgQPut(&p_tcb->MsgQ,                                // No,  Task is pending on something else ...
                   p_void,                                      // ... Deposit the message in the task's queue
                   msg_size,
                   opt,
                   ts,
                   p_err);
        CORE_EXIT_ATOMIC();
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          OS_TRACE_TASK_MSG_Q_POST_FAILED(&p_tcb->MsgQ);
        }
      }
      break;

    case OS_TASK_STATE_DEL:
#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      OS_TRACE_TASK_MSG_Q_POST_FAILED(&p_tcb->MsgQ);
      break;
#endif

    default:
      CORE_EXIT_ATOMIC();
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );
  }

  OS_TRACE_TASK_MSG_Q_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
}
#endif

/****************************************************************************************************//**
 *                                               OSTaskRegGet()
 *
 * @brief    Obtains the current value of a task register. Task registers are application specific
 *           and can be used to store task specific values such as 'error numbers' (i.e. errno),
 *           statistics, etc.
 *
 * @param    p_tcb   Pointer to the TCB of the task from which you want to read the register.
 *                   If 'p_tcb' is a NULL pointer, you will get the register of the current task.
 *
 * @param    id      The 'id' of the desired task variable. Note that the 'id' must be less than
 *                   OS_CFG_TASK_REG_TBL_SIZE.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *
 * @return   The current value of the task's register, or 0 if an error is detected.
 *******************************************************************************************************/
#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
OS_REG OSTaskRegGet(OS_TCB    *p_tcb,
                    OS_REG_ID id,
                    RTOS_ERR  *p_err)
{
  OS_REG value;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  OS_ASSERT_DBG_ERR_SET((id < OS_CFG_TASK_REG_TBL_SIZE), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  CORE_ENTER_ATOMIC();
  if (p_tcb == DEF_NULL) {
    p_tcb = OSTCBCurPtr;
  }
  value = p_tcb->RegTbl[id];
  CORE_EXIT_ATOMIC();
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (value);
}
#endif

/****************************************************************************************************//**
 *                                               OSTaskRegGetID()
 *
 * @brief    This function obtains a task register ID. This function allows task register IDs
 *           to be allocated dynamically instead of statically.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NO_MORE_RSRC
 *
 * @return   The next available task register 'id' or OS_CFG_TASK_REG_TBL_SIZE if an error is detected.
 *******************************************************************************************************/
#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
OS_REG_ID OSTaskRegGetID(RTOS_ERR *p_err)
{
  OS_REG_ID id;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, OS_CFG_TASK_REG_TBL_SIZE);

  CORE_ENTER_ATOMIC();
  if (OSTaskRegNextAvailID >= OS_CFG_TASK_REG_TBL_SIZE) {       // See if we exceeded the number of IDs available
    CORE_EXIT_ATOMIC();                                         // Yes, cannot allocate more task register IDs
    RTOS_ERR_SET(*p_err, RTOS_ERR_NO_MORE_RSRC);
    return (OS_CFG_TASK_REG_TBL_SIZE);
  }

  id = OSTaskRegNextAvailID;                                    // Assign the next available ID
  OSTaskRegNextAvailID++;                                       // Increment available ID for next request
  CORE_EXIT_ATOMIC();
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (id);
}
#endif

/****************************************************************************************************//**
 *                                               OSTaskRegSet()
 *
 * @brief    Changes the current value of a task register. Task registers are application specific
 *           and can be used to store task specific values such as error numbers (i.e. errno),
 *           statistics, etc.
 *
 * @param    p_tcb   Pointer to the TCB of the task for which you want to set the register.
 *                   If 'p_tcb' is a NULL pointer, change the register of the current task.
 *
 * @param    id      The 'id' of the desired task register. Note that the 'id' must be less than
 *                   OS_CFG_TASK_REG_TBL_SIZE.
 *
 * @param    value   The desired value for the task register.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *******************************************************************************************************/
#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
void OSTaskRegSet(OS_TCB    *p_tcb,
                  OS_REG_ID id,
                  OS_REG    value,
                  RTOS_ERR  *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  OS_ASSERT_DBG_ERR_SET((id < OS_CFG_TASK_REG_TBL_SIZE), *p_err, RTOS_ERR_INVALID_ARG,; );

  CORE_ENTER_ATOMIC();
  if (p_tcb == DEF_NULL) {
    p_tcb = OSTCBCurPtr;
  }
  p_tcb->RegTbl[id] = value;
  CORE_EXIT_ATOMIC();
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               OSTaskResume()
 *
 * @brief    Resumes a previously suspended task. This is the only call that removes an explicit
 *           task suspension.
 *
 * @param    p_tcb   Pointer to the TCB of the task to resume.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/
#if (OS_CFG_TASK_SUSPEND_EN == DEF_ENABLED)
void OSTaskResume(OS_TCB   *p_tcb,
                  RTOS_ERR *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  OS_TRACE_TASK_RESUME_ENTER(p_tcb);

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

#if (OS_ARG_CHK_EN == DEF_ENABLED)
  if (OSRunning != OS_STATE_OS_RUNNING) {                       // Make sure kernel is running
    OS_TRACE_TASK_RESUME_EXIT(RTOS_ERR_NOT_READY);
    OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_NOT_READY,; );
  }
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  CORE_ENTER_ATOMIC();
#if (OS_ARG_CHK_EN == DEF_ENABLED)
  if ((p_tcb == DEF_NULL)                                       // We cannot resume 'self'
      || (p_tcb == OSTCBCurPtr)) {
    CORE_EXIT_ATOMIC();
    OS_TRACE_TASK_RESUME_EXIT(RTOS_ERR_INVALID_ARG);
    OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_INVALID_ARG,; );
  }
#endif

  switch (p_tcb->TaskState) {
    case OS_TASK_STATE_RDY:
    case OS_TASK_STATE_DLY:
    case OS_TASK_STATE_PEND:
    case OS_TASK_STATE_PEND_TIMEOUT:
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;

    case OS_TASK_STATE_SUSPENDED:
      p_tcb->SuspendCtr--;
      if (p_tcb->SuspendCtr == 0u) {
        p_tcb->TaskState = OS_TASK_STATE_RDY;
        OS_RdyListInsert(p_tcb);                                // Insert the task in the ready list
        OS_TRACE_TASK_RESUME(p_tcb);
      }
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_DLY_SUSPENDED:
      p_tcb->SuspendCtr--;
      if (p_tcb->SuspendCtr == 0u) {
        p_tcb->TaskState = OS_TASK_STATE_DLY;
      }
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_PEND_SUSPENDED:
      p_tcb->SuspendCtr--;
      if (p_tcb->SuspendCtr == 0u) {
        p_tcb->TaskState = OS_TASK_STATE_PEND;
      }
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
      p_tcb->SuspendCtr--;
      if (p_tcb->SuspendCtr == 0u) {
        p_tcb->TaskState = OS_TASK_STATE_PEND_TIMEOUT;
      }
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_DEL:
#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
      CORE_EXIT_ATOMIC();
      OS_TRACE_TASK_RESUME_EXIT(RTOS_ERR_INVALID_STATE);
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      return;
#endif

    default:
      CORE_EXIT_ATOMIC();
      OS_TRACE_TASK_RESUME_EXIT(RTOS_ERR_OS);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );
  }

  OSSched();

  OS_TRACE_TASK_RESUME_EXIT(RTOS_ERR_CODE_GET(*p_err));
}
#endif

/****************************************************************************************************//**
 *                                               OSTaskSemPend()
 *
 * @brief    Blocks the current task until a signal is sent by another task or ISR.
 *
 * @param    timeout     The amount of time you will wait for the signal.
 *
 * @param    opt         Determines if the user wants to block if a semaphore post was not received:
 *                           - OS_OPT_PEND_BLOCKING        Task will     block.
 *                           - OS_OPT_PEND_NON_BLOCKING    Task will NOT block.
 *
 * @param    p_ts        Pointer to a variable that will receive the timestamp of when the
 *                       semaphore was posted or pend aborted.
 *                       If you pass a NULL pointer (i.e. (CPU_TS *)0), you will not get the
 *                       timestamp. In other words, passing a NULL pointer is valid and indicates
 *                       that you don't need the timestamp.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_NOT_READY
 *
 * @return   The current count of signals the task received, 0 if none.
 *******************************************************************************************************/
OS_SEM_CTR OSTaskSemPend(OS_TICK  timeout,
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

  OS_TRACE_TASK_SEM_PEND_ENTER(OSTCBCurPtr, timeout, opt, p_ts);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, 0u);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_PEND_BLOCKING)
                         || (opt == OS_OPT_PEND_NON_BLOCKING)), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  CORE_ENTER_ATOMIC();
  if (OSTCBCurPtr->SemCtr > 0u) {                               // See if task already been signaled
    OSTCBCurPtr->SemCtr--;
    ctr = OSTCBCurPtr->SemCtr;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    if (p_ts != DEF_NULL) {
      *p_ts = OSTCBCurPtr->TS;
    }
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
#if (OS_CFG_TS_EN == DEF_ENABLED)
    OSTCBCurPtr->SemPendTime = OS_TS_GET() - OSTCBCurPtr->TS;
    if (OSTCBCurPtr->SemPendTimeMax < OSTCBCurPtr->SemPendTime) {
      OSTCBCurPtr->SemPendTimeMax = OSTCBCurPtr->SemPendTime;
    }
#endif
#endif
#endif
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    OS_TRACE_TASK_SEM_PEND(OSTCBCurPtr);
    OS_TRACE_TASK_SEM_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return (ctr);
  }

  if ((opt & OS_OPT_PEND_NON_BLOCKING) != 0u) {                 // Caller wants to block if not available?
    CORE_EXIT_ATOMIC();
#if (OS_CFG_TS_EN == DEF_ENABLED)
    if (p_ts != DEF_NULL) {
      *p_ts = 0u;
    }
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);                 // No
    OS_TRACE_TASK_SEM_PEND_FAILED(OSTCBCurPtr);
    OS_TRACE_TASK_SEM_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return (0u);
  } else {                                                      // Yes
    if (OSSchedLockNestingCtr > 0u) {                           // Can't pend when the scheduler is locked
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = 0u;
      }
#endif
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
      OS_TRACE_TASK_SEM_PEND_FAILED(OSTCBCurPtr);
      OS_TRACE_TASK_SEM_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
      return (0u);
    }
  }

  OS_Pend(DEF_NULL,                                             // Block task pending on Signal
          OS_TASK_PEND_ON_TASK_SEM,
          timeout);
  CORE_EXIT_ATOMIC();
  OS_TRACE_TASK_SEM_PEND_BLOCK(OSTCBCurPtr);
  OSSched();                                                    // Find next highest priority task ready to run

  CORE_ENTER_ATOMIC();
  switch (OSTCBCurPtr->PendStatus) {                            // See if we timed-out or aborted
    case OS_STATUS_PEND_OK:
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
#if (OS_CFG_TS_EN == DEF_ENABLED)
        OSTCBCurPtr->SemPendTime = OS_TS_GET() - OSTCBCurPtr->TS;
        if (OSTCBCurPtr->SemPendTimeMax < OSTCBCurPtr->SemPendTime) {
          OSTCBCurPtr->SemPendTimeMax = OSTCBCurPtr->SemPendTime;
        }
#endif
#endif
      }
#endif
      OS_TRACE_TASK_SEM_PEND(OSTCBCurPtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case OS_STATUS_PEND_ABORT:
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = OSTCBCurPtr->TS;
      }
#endif
      OS_TRACE_TASK_SEM_PEND_FAILED(OSTCBCurPtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);                     // Indicate that we aborted
      break;

    case OS_STATUS_PEND_TIMEOUT:
#if (OS_CFG_TS_EN == DEF_ENABLED)
      if (p_ts != DEF_NULL) {
        *p_ts = 0u;
      }
#endif
      OS_TRACE_TASK_SEM_PEND_FAILED(OSTCBCurPtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);                   // Indicate that we didn't get event within TO
      break;

    case OS_STATUS_PEND_DEL:
    default:
      OS_TRACE_TASK_SEM_PEND_FAILED(OSTCBCurPtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, 0u);
  }
  ctr = OSTCBCurPtr->SemCtr;
  CORE_EXIT_ATOMIC();
  OS_TRACE_TASK_SEM_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
  return (ctr);
}

/****************************************************************************************************//**
 *                                           OSTaskSemPendAbort()
 *
 * @brief    Aborts and readies the task specified. This function should be used to
 *           fault-abort the wait for a signal, rather than to normally post the signal to the task via
 *           OSTaskSemPost().
 *
 * @param    p_tcb   Pointer to the TCB of the task to pend abort.
 *
 * @param    opt     Provides options for this function:
 *                       - OS_OPT_POST_NONE        No option selected.
 *                       - OS_OPT_POST_NO_SCHED    Indicates that the scheduler will not be called.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NONE_WAITING
 *
 * @return   == DEF_FALSE    If task was not waiting for a message, or upon error.
 *           == DEF_TRUE     If task was waiting for a message and was readied and informed.
 *******************************************************************************************************/
CPU_BOOLEAN OSTaskSemPendAbort(OS_TCB   *p_tcb,
                               OS_OPT   opt,
                               RTOS_ERR *p_err)
{
  CPU_TS ts;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FALSE);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, DEF_FALSE);

  //                                                               Make sure kernel is running.
  OS_ASSERT_DBG_ERR_SET((OSRunning == OS_STATE_OS_RUNNING), *p_err, RTOS_ERR_NOT_READY, DEF_FALSE);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_POST_NONE)
                         || (opt == OS_OPT_POST_NO_SCHED)), *p_err, RTOS_ERR_INVALID_ARG, DEF_FALSE);

  CORE_ENTER_ATOMIC();
#if (OS_ARG_CHK_EN == DEF_ENABLED)
  if ((p_tcb == DEF_NULL)                                       // Pend abort self?
      || (p_tcb == OSTCBCurPtr)) {
    CORE_EXIT_ATOMIC();                                         // ... doesn't make sense!
    OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_INVALID_ARG, DEF_FALSE);
  }
#endif

  if (p_tcb->PendOn != OS_TASK_PEND_ON_TASK_SEM) {              // Is task waiting for a signal?
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE_WAITING);
    return (DEF_FALSE);
  }
  CORE_EXIT_ATOMIC();

  CORE_ENTER_ATOMIC();
#if (OS_CFG_TS_EN == DEF_ENABLED)
  ts = OS_TS_GET();
#else
  ts = 0u;
#endif
  OS_PendAbort(p_tcb,                                           // Abort the pend
               ts,
               OS_STATUS_PEND_ABORT);
  CORE_EXIT_ATOMIC();
  if ((opt & OS_OPT_POST_NO_SCHED) == 0u) {
    OSSched();                                                  // Run the scheduler
  }
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (DEF_TRUE);
}

/****************************************************************************************************//**
 *                                               OSTaskSemPost()
 *
 * @brief    Signals a task waiting for a signal.
 *
 * @param    p_tcb   The pointer to the TCB of the task to signal. A NULL pointer indicates that
 *                   you are sending a signal to yourself.
 *
 * @param    opt     Determines the type of POST performed:
 *                       - OS_OPT_POST_NONE        No option.
 *                       - OS_OPT_POST_NO_SCHED    Do not call the scheduler.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_INVALID_STATE
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   The current value of the task's signal counter, or 0 if called from an ISR.
 *
 * @note     (1) This function may be called from an ISR.
 *******************************************************************************************************/
OS_SEM_CTR OSTaskSemPost(OS_TCB   *p_tcb,
                         OS_OPT   opt,
                         RTOS_ERR *p_err)
{
  OS_SEM_CTR ctr;
  CPU_TS     ts;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  OS_TRACE_TASK_SEM_POST_ENTER(p_tcb, opt);

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_POST_NONE)
                         || (opt == OS_OPT_POST_NO_SCHED)), *p_err, RTOS_ERR_INVALID_ARG, 0u);

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

  OS_TRACE_TASK_SEM_POST(p_tcb);

  CORE_ENTER_ATOMIC();
  if (p_tcb == DEF_NULL) {                                      // Post signal to 'self'?
    p_tcb = OSTCBCurPtr;
  }
#if (OS_CFG_TS_EN == DEF_ENABLED)
  p_tcb->TS = ts;
#endif
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                          // Assume we won't have any errors
  ctr = 0u;
  switch (p_tcb->TaskState) {
    case OS_TASK_STATE_RDY:
    case OS_TASK_STATE_DLY:
    case OS_TASK_STATE_SUSPENDED:
    case OS_TASK_STATE_DLY_SUSPENDED:
      switch (sizeof(OS_SEM_CTR)) {
        case 1u:
          if (p_tcb->SemCtr == DEF_INT_08U_MAX_VAL) {
            CORE_EXIT_ATOMIC();
            RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
            OS_TRACE_TASK_SEM_POST_FAILED(p_tcb);
            OS_TRACE_TASK_SEM_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
            return (0u);
          }
          break;

        case 2u:
          if (p_tcb->SemCtr == DEF_INT_16U_MAX_VAL) {
            CORE_EXIT_ATOMIC();
            RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
            OS_TRACE_TASK_SEM_POST_FAILED(p_tcb);
            OS_TRACE_TASK_SEM_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
            return (0u);
          }
          break;

        case 4u:
          if (p_tcb->SemCtr == DEF_INT_32U_MAX_VAL) {
            CORE_EXIT_ATOMIC();
            RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
            OS_TRACE_TASK_SEM_POST_FAILED(p_tcb);
            OS_TRACE_TASK_SEM_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
            return (0u);
          }
          break;

        default:
          break;
      }
      p_tcb->SemCtr++;                                          // Task signaled is not pending on anything
      ctr = p_tcb->SemCtr;
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_PEND:
    case OS_TASK_STATE_PEND_TIMEOUT:
    case OS_TASK_STATE_PEND_SUSPENDED:
    case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
      if (p_tcb->PendOn == OS_TASK_PEND_ON_TASK_SEM) {          // Is task signaled waiting for a signal?
        OS_Post(DEF_NULL,                                       // Task is pending on signal
                p_tcb,
                DEF_NULL,
                0u,
                ts);
        ctr = p_tcb->SemCtr;
        CORE_EXIT_ATOMIC();
        if ((opt & OS_OPT_POST_NO_SCHED) == 0u) {
          OSSched();                                            // Run the scheduler
        }
      } else {
        switch (sizeof(OS_SEM_CTR)) {
          case 1u:
            if (p_tcb->SemCtr == DEF_INT_08U_MAX_VAL) {
              CORE_EXIT_ATOMIC();
              RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
              OS_TRACE_TASK_SEM_POST_FAILED(p_tcb);
              OS_TRACE_TASK_SEM_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
              return (0u);
            }
            break;

          case 2u:
            if (p_tcb->SemCtr == DEF_INT_16U_MAX_VAL) {
              CORE_EXIT_ATOMIC();
              RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
              OS_TRACE_TASK_SEM_POST_FAILED(p_tcb);
              OS_TRACE_TASK_SEM_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
              return (0u);
            }
            break;

          case 4u:
            if (p_tcb->SemCtr == DEF_INT_32U_MAX_VAL) {
              CORE_EXIT_ATOMIC();
              RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
              OS_TRACE_TASK_SEM_POST_FAILED(p_tcb);
              OS_TRACE_TASK_SEM_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
              return (0u);
            }
            break;

          default:
            break;
        }
        p_tcb->SemCtr++;                                        // No,  Task signaled is NOT pending on semaphore ...
        ctr = p_tcb->SemCtr;                                    // ... it must be waiting on something else
        CORE_EXIT_ATOMIC();
      }
      break;

    case OS_TASK_STATE_DEL:
#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      OS_TRACE_TASK_SEM_POST_FAILED(p_tcb);
      ctr = 0u;
      break;
#endif

    default:
      CORE_EXIT_ATOMIC();
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, 0u);
  }

  OS_TRACE_TASK_SEM_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));

  return (ctr);
}

/****************************************************************************************************//**
 *                                               OSTaskSemSet()
 *
 * @brief    Clears the signal counter.
 *
 * @param    p_tcb   Pointer to the TCB of the task to clear the counter. If you specify a
 *                   NULL pointer, the signal counter of the current task will be cleared.
 *
 * @param    cnt     The desired value of the semaphore counter.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_TASK_WAITING
 *
 * @return   The value of the signal counter before being set, or 0 on error.
 *******************************************************************************************************/
OS_SEM_CTR OSTaskSemSet(OS_TCB     *p_tcb,
                        OS_SEM_CTR cnt,
                        RTOS_ERR   *p_err)
{
  OS_SEM_CTR ctr;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, 0u);

  CORE_ENTER_ATOMIC();
  if (p_tcb == DEF_NULL) {
    p_tcb = OSTCBCurPtr;
  }

  if (((p_tcb->TaskState   & OS_TASK_STATE_PEND) != 0u)         // Not allowed when a task is waiting.
      && (p_tcb->PendOn == OS_TASK_PEND_ON_TASK_SEM)) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_TASK_WAITING);
    return (0u);
  }

  ctr = p_tcb->SemCtr;
  p_tcb->SemCtr = (OS_SEM_CTR)cnt;
  CORE_EXIT_ATOMIC();
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (ctr);
}

/****************************************************************************************************//**
 *                                               OSTaskStkChk()
 *
 * @brief    Calculates the amount of free memory left on the specified task's stack.
 *
 * @param    p_tcb   Pointer to the TCB of the task to check. If you specify a NULL pointer,
 *                   you are specifying that you want to check the stack of the current task.
 *
 * @param    p_free  Pointer to a variable that will receive the number of free 'entries' on
 *                   the task's stack.
 *
 * @param    p_used  Pointer to a variable that will receive the number of used 'entries' on
 *                   the task's stack.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *                       - RTOS_ERR_NOT_SUPPORTED
 *
 * @note     (1) Options OS_OPT_TASK_STK_CHK and OS_OPT_TASK_STK_CLR should be set in OSTaskCreate() call
 *           in order to use OSTaskStkChk(). See OS_OPT_TASK_xxx in os.h.
 *******************************************************************************************************/

#if (OS_CFG_STAT_TASK_STK_CHK_EN == DEF_ENABLED)
void OSTaskStkChk(OS_TCB       *p_tcb,
                  CPU_STK_SIZE *p_free,
                  CPU_STK_SIZE *p_used,
                  RTOS_ERR     *p_err)
{
  CPU_STK_SIZE free_stk;
  CPU_STK      *p_stk;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               User must specify valid destinations for the sizes
  OS_ASSERT_DBG_ERR_SET((p_free != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  OS_ASSERT_DBG_ERR_SET((p_used != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CORE_ENTER_ATOMIC();
  if (p_tcb == DEF_NULL) {                                      // Check the stack of the current task?
    p_tcb = OSTCBCurPtr;                                        // Yes
  }

  if (p_tcb->StkPtr == DEF_NULL) {                              // Make sure task exist
    CORE_EXIT_ATOMIC();
    *p_free = 0u;
    *p_used = 0u;
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  if ((p_tcb->Opt & OS_OPT_TASK_STK_CHK) == 0u) {               // Make sure stack checking option is set
    CORE_EXIT_ATOMIC();
    *p_free = 0u;
    *p_used = 0u;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    return;
  }
  CORE_EXIT_ATOMIC();

  free_stk = 0u;
#if (CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO)
  p_stk = p_tcb->StkBasePtr;                                    // Start at the lowest memory and go up
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
  p_stk += OS_CFG_TASK_STK_REDZONE_DEPTH;
#endif
  while (*p_stk == 0u) {                                        // Compute the number of zero entries on the stk
    p_stk++;
    free_stk++;
  }
#else
  p_stk = p_tcb->StkBasePtr + p_tcb->StkSize - 1u;              // Start at the highest memory and go down
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
  p_stk -= OS_CFG_TASK_STK_REDZONE_DEPTH;
#endif
  while (*p_stk == 0u) {
    free_stk++;
    p_stk--;
  }
#endif
  *p_free = free_stk;
  *p_used = (p_tcb->StkSize - free_stk);                        // Compute number of entries used on the stack
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                           OSTaskStkRedzoneChk()
 *
 * @brief    Verifies a task's stack redzone.
 *
 * @param    p_tcb   Pointer to the TCB of the task to check or null for the current task.
 *
 * @return   == DEF_FAIL     If the stack is     corrupted.
 *           == DEF_OK       If the stack is NOT corrupted.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
CPU_BOOLEAN OSTaskStkRedzoneChk(OS_TCB *p_tcb)
{
  CPU_BOOLEAN stk_status;

  if (p_tcb == DEF_NULL) {
    p_tcb = OSTCBCurPtr;
  }
  //                                                               Check if SP is valid:
  //                                                               StkBase <= SP < (StkBase + StkSize)
  if ((p_tcb->StkPtr < p_tcb->StkBasePtr)
      || (p_tcb->StkPtr >= (p_tcb->StkBasePtr + p_tcb->StkSize))) {
    return (DEF_FAIL);
  }

  stk_status = OS_TaskStkRedzoneChk(p_tcb->StkBasePtr, p_tcb->StkSize);

  return (stk_status);
}
#endif

/****************************************************************************************************//**
 *                                               OSTaskSuspend()
 *
 * @brief    This function is called to suspend a task. The task can be the calling task if 'p_tcb' is
 *           a NULL pointer or the pointer to the TCB of the calling task.
 *
 * @param    p_tcb   Pointer to the TCB of the task to resume.
 *                   If p_tcb is a NULL pointer, suspend the current task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_OS
 *                       - RTOS_ERR_NOT_READY
 *                       - RTOS_ERR_INVALID_STATE
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *******************************************************************************************************/
#if (OS_CFG_TASK_SUSPEND_EN == DEF_ENABLED)
void OSTaskSuspend(OS_TCB   *p_tcb,
                   RTOS_ERR *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  OS_TRACE_TASK_SUSPEND_ENTER(p_tcb);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  OS_TRACE_TASK_SUSPEND(p_tcb);

  CORE_ENTER_ATOMIC();
  if (p_tcb == DEF_NULL) {                                      // See if specified to suspend self
    if (OSRunning != OS_STATE_OS_RUNNING) {                     // Can't suspend self when the kernel isn't running
      CORE_EXIT_ATOMIC();
      OS_TRACE_TASK_RESUME_EXIT(RTOS_ERR_NOT_READY);
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
      return;
    }
    p_tcb = OSTCBCurPtr;
  }

  if (p_tcb == OSTCBCurPtr) {
    if (OSSchedLockNestingCtr > 0u) {                           // Can't suspend when the scheduler is locked
      CORE_EXIT_ATOMIC();
      OS_TRACE_TASK_RESUME_EXIT(RTOS_ERR_OS_SCHED_LOCKED);
      RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
      return;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  switch (p_tcb->TaskState) {
    case OS_TASK_STATE_RDY:
      p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;
      p_tcb->SuspendCtr = 1u;
      OS_RdyListRemove(p_tcb);
      OS_TRACE_TASK_SUSPEND(p_tcb);
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_DLY:
      p_tcb->TaskState = OS_TASK_STATE_DLY_SUSPENDED;
      p_tcb->SuspendCtr = 1u;
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_PEND:
      p_tcb->TaskState = OS_TASK_STATE_PEND_SUSPENDED;
      p_tcb->SuspendCtr = 1u;
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_PEND_TIMEOUT:
      p_tcb->TaskState = OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED;
      p_tcb->SuspendCtr = 1u;
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_SUSPENDED:
    case OS_TASK_STATE_DLY_SUSPENDED:
    case OS_TASK_STATE_PEND_SUSPENDED:
    case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
      if (p_tcb->SuspendCtr == (OS_NESTING_CTR)-1) {
        CORE_EXIT_ATOMIC();
        OS_TRACE_TASK_SUSPEND_EXIT(RTOS_ERR_WOULD_OVF);
        RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
        return;
      }
      p_tcb->SuspendCtr++;
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_DEL:
#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
      CORE_EXIT_ATOMIC();
      OS_TRACE_TASK_SUSPEND_EXIT(RTOS_ERR_INVALID_STATE);
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      return;
#endif

    default:
      CORE_EXIT_ATOMIC();
      OS_TRACE_TASK_SUSPEND_EXIT(RTOS_ERR_OS);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );
  }

  if (OSRunning == OS_STATE_OS_RUNNING) {                       // Only schedule when the kernel is running
    OSSched();
    OS_TRACE_TASK_SUSPEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
  }
}
#endif

/****************************************************************************************************//**
 *                                           OSTaskTimeQuantaSet()
 *
 * @brief    Changes the value of the task's specific time slice.
 *
 * @param    p_tcb           Pointer to the TCB of the task to change. If you specify an NULL
 *                           pointer, the current task is assumed.
 *
 * @param    time_quanta     The number of ticks before the CPU is taken away when round-robin
 *                           scheduling is enabled.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *
 * @note     (1) If the task referenced by p_tcb is currently running, then the remaining time until
 *               the quanta expires is updated with the new time_quanta only if the new time_quanta
 *               is greater than the remaining time of the previous quanta.
 *******************************************************************************************************/
#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
void OSTaskTimeQuantaSet(OS_TCB   *p_tcb,
                         OS_TICK  time_quanta,
                         RTOS_ERR *p_err)
{
  uint32_t time_remain;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  if (OSSchedRoundRobinEn != DEF_TRUE) {                        // Make sure round-robin has been enabled
    return;
  }

  CORE_ENTER_ATOMIC();
  if (p_tcb == DEF_NULL) {
    p_tcb = OSTCBCurPtr;
  }

  if (time_quanta == 0u) {
    p_tcb->TimeQuanta = (uint64_t)(((uint64_t)OSSchedRoundRobinDfltTimeQuanta * (uint64_t)sl_sleeptimer_get_timer_frequency()) + (OSCfg_TickRate_Hz - 1u)) / OSCfg_TickRate_Hz;
  } else {
    p_tcb->TimeQuanta = (uint64_t)(((uint64_t)(time_quanta) * (uint64_t)sl_sleeptimer_get_timer_frequency()) + (OSCfg_TickRate_Hz - 1u)) / OSCfg_TickRate_Hz;
  }

  if (p_tcb == OSTCBCurPtr) {
    //                                                             p_tcb belongs to a task that is in running state.
    sl_status_t status;
    status = sl_sleeptimer_get_timer_time_remaining(&OSRoundRobinTimer, &time_remain);
    if (status == SL_STATUS_OK) {
      time_remain = DEF_MAX(time_remain, p_tcb->TimeQuanta);
      //                                                           Only stop/restart the sleep timer if needed
      if (p_tcb->TimeQuantaCtr != time_remain) {
        p_tcb->TimeQuantaCtr = time_remain;
        sl_sleeptimer_stop_timer(&OSRoundRobinTimer);
        OS_SchedRoundRobinRestartTimer(p_tcb);
      }
    }
  } else {
    //                                                             p_tcb is for a task that is NOT in running state.
    p_tcb->TimeQuantaCtr = DEF_MAX(p_tcb->TimeQuantaCtr, p_tcb->TimeQuanta);
  }

  CORE_EXIT_ATOMIC();
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           OS_TaskDbgListAdd()
 *
 * @brief    Add a task to the task debug list.
 *
 * @param    p_tcb   Pointer to the TCB to add.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_TaskDbgListAdd(OS_TCB *p_tcb)
{
  p_tcb->DbgPrevPtr = DEF_NULL;
  if (OSTaskDbgListPtr == DEF_NULL) {
    p_tcb->DbgNextPtr = DEF_NULL;
  } else {
    p_tcb->DbgNextPtr = OSTaskDbgListPtr;
    OSTaskDbgListPtr->DbgPrevPtr = p_tcb;
  }
  OSTaskDbgListPtr = p_tcb;
}

/****************************************************************************************************//**
 *                                           OS_TaskDbgListRemove()
 *
 * @brief    Remove a task from the task debug list.
 *
 * @param    p_tcb   Pointer to the TCB to remove.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TaskDbgListRemove(OS_TCB *p_tcb)
{
  OS_TCB *p_tcb_next;
  OS_TCB *p_tcb_prev;

  p_tcb_prev = p_tcb->DbgPrevPtr;
  p_tcb_next = p_tcb->DbgNextPtr;

  if (p_tcb_prev == DEF_NULL) {
    OSTaskDbgListPtr = p_tcb_next;
    if (p_tcb_next != DEF_NULL) {
      p_tcb_next->DbgPrevPtr = DEF_NULL;
    }
    p_tcb->DbgNextPtr = DEF_NULL;
  } else if (p_tcb_next == DEF_NULL) {
    p_tcb_prev->DbgNextPtr = DEF_NULL;
    p_tcb->DbgPrevPtr = DEF_NULL;
  } else {
    p_tcb_prev->DbgNextPtr = p_tcb_next;
    p_tcb_next->DbgPrevPtr = p_tcb_prev;
    p_tcb->DbgNextPtr = DEF_NULL;
    p_tcb->DbgPrevPtr = DEF_NULL;
  }
}
#endif

/****************************************************************************************************//**
 *                                               OS_TaskInit()
 *
 * @brief    This function is called by OSInit() to initialize the task management.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TaskInit(RTOS_ERR *p_err)
{
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OSTaskDbgListPtr = DEF_NULL;
#endif

  OSTaskQty = 0u;                                               // Clear the number of tasks

#if ((OS_CFG_TASK_PROFILE_EN == DEF_ENABLED) || (OS_CFG_DBG_EN == DEF_ENABLED))
  OSTaskCtxSwCtr = 0u;                                          // Clear the context switch counter
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               OS_TaskInitTCB()
 *
 * @brief    This function is called to initialize a TCB to default values.
 *
 * @param    p_tcb   Pointer to the TCB to initialize.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TaskInitTCB(OS_TCB *p_tcb)
{
  *p_tcb = (OS_TCB){ 0 };

#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
  OS_REG_ID reg_id;
#endif
#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
  OS_TLS_ID id;
#endif

  p_tcb->StkPtr = DEF_NULL;

  p_tcb->StkLimitPtr = DEF_NULL;

  p_tcb->ExtPtr = DEF_NULL;

  p_tcb->NextPtr = DEF_NULL;
  p_tcb->PrevPtr = DEF_NULL;

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_tcb->NamePtr = (CPU_CHAR *)((void *)"?Task");
#endif

#if ((OS_CFG_DBG_EN == DEF_ENABLED) || (OS_CFG_STAT_TASK_STK_CHK_EN == DEF_ENABLED))
  p_tcb->StkBasePtr = DEF_NULL;
#endif

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_tcb->TaskEntryAddr = 0u;
  p_tcb->TaskEntryArg = DEF_NULL;
#endif

#if (OS_CFG_TS_EN == DEF_ENABLED)
  p_tcb->TS = 0u;
#endif

#if (OS_MSG_EN == DEF_ENABLED)
  p_tcb->MsgPtr = DEF_NULL;
  p_tcb->MsgSize = 0u;
#endif

#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
  OS_MsgQInit(&p_tcb->MsgQ,
              0u);
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
  p_tcb->MsgQPendTime = 0u;
  p_tcb->MsgQPendTimeMax = 0u;
#endif
#endif

#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  p_tcb->FlagsPend = 0u;
  p_tcb->FlagsOpt = 0u;
  p_tcb->FlagsRdy = 0u;
#endif

#if (OS_CFG_MON_EN == DEF_ENABLED)
  p_tcb->MonData.p_eval_data = DEF_NULL;
  p_tcb->MonData.p_on_eval = DEF_NULL;
#endif

#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
  for (reg_id = 0u; reg_id < OS_CFG_TASK_REG_TBL_SIZE; reg_id++) {
    p_tcb->RegTbl[reg_id] = 0u;
  }
#endif

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
  for (id = 0u; id < OS_CFG_TLS_TBL_SIZE; id++) {
    p_tcb->TLS_Tbl[id] = 0u;
  }
#endif

  p_tcb->SemCtr = 0u;
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
  p_tcb->SemPendTime = 0u;
  p_tcb->SemPendTimeMax = 0u;
#endif

  p_tcb->StkSize = 0u;

#if (OS_CFG_TASK_SUSPEND_EN == DEF_ENABLED)
  p_tcb->SuspendCtr = 0u;
#endif

#if (OS_CFG_STAT_TASK_STK_CHK_EN == DEF_ENABLED)
  p_tcb->StkFree = 0u;
  p_tcb->StkUsed = 0u;
#endif

  p_tcb->Opt = 0u;

#if (OS_CFG_TICK_EN == DEF_ENABLED)
  p_tcb->TickCtrPrev = 0u;
  p_tcb->IsTickCtrPrevValid = DEF_NO;
#endif

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
  p_tcb->TimeQuanta = 0u;
  p_tcb->TimeQuantaCtr = 0u;
#endif

#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
  p_tcb->CPUUsage = 0u;
  p_tcb->CPUUsageMax = 0u;
  p_tcb->CtxSwCtr = 0u;
  p_tcb->CyclesDelta = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
  p_tcb->CyclesStart = OS_TS_GET();                             // Read the current timestamp and save
#else
  p_tcb->CyclesStart = 0u;
#endif
  p_tcb->CyclesTotal = 0u;
#endif

  p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING;
  p_tcb->PendStatus = OS_STATUS_PEND_OK;
  p_tcb->TaskState = OS_TASK_STATE_RDY;

  p_tcb->Prio = OS_PRIO_INIT;
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  p_tcb->BasePrio = OS_PRIO_INIT;
  p_tcb->MutexGrpHeadPtr = DEF_NULL;
#endif

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_tcb->DbgPrevPtr = DEF_NULL;
  p_tcb->DbgNextPtr = DEF_NULL;
  p_tcb->DbgNamePtr = (CPU_CHAR *)((void *)" ");
#endif

#if (OS_CFG_ERRNO_EN == 1)
  p_tcb->local_errno = 0;
#endif
}

/****************************************************************************************************//**
 *                                               OS_TaskReturn()
 *
 * @brief    This function is called if a task accidentally returns without deleting itself. In other
 *           words, a task should either be an infinite loop or delete itself if it's done.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TaskReturn(void)
{
#if ((OS_CFG_TASK_DEL_EN == DEF_ENABLED) || (OS_CFG_TICK_EN == DEF_ENABLED))
  RTOS_ERR err;
#endif

  OSTaskReturnHook(OSTCBCurPtr);                                // Call hook to let user decide on what to do
#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
  OSTaskDel(DEF_NULL,                                           // Delete task if it accidentally returns!
            &err);
#else
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  while (DEF_TRUE) {
    OSTimeDly(DEF_INT_16U_MAX_VAL,
              OS_OPT_TIME_DLY,
              &err);
    (void)err;
  }
#else
  CPU_SW_EXCEPTION(; );
#endif
#endif
}

/****************************************************************************************************//**
 *                                           OS_TaskStkRedzoneChk()
 *
 * @brief    Verify a task's stack redzone.
 *
 * @param    p_base      Pointer to the base of the stack.
 *
 * @param    stk_size    The size of the stack.
 *
 * @return   == DEF_FAIL     If the stack is     corrupted.
 *           == DEF_OK       If the stack is NOT corrupted.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
CPU_BOOLEAN OS_TaskStkRedzoneChk(CPU_STK      *p_base,
                                 CPU_STK_SIZE stk_size)
{
  CPU_INT32U i;

#if (CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO)
  (void)&stk_size;                                              // Prevent compiler warning for not using 'stk_size'

  for (i = 0u; i < OS_CFG_TASK_STK_REDZONE_DEPTH; i++) {
    if (*p_base != (CPU_STK)OS_STACK_CHECK_VAL) {
      return (DEF_FAIL);
    }
    p_base++;
  }
#else
  p_base = p_base + stk_size - 1u;
  for (i = 0u; i < OS_CFG_TASK_STK_REDZONE_DEPTH; i++) {
    if (*p_base != (CPU_STK)OS_STACK_CHECK_VAL) {
      return (DEF_FAIL);
    }
    p_base--;
  }
#endif

  return (DEF_OK);
}
#endif

/****************************************************************************************************//**
 *                                           OS_TaskStkRedzoneInit()
 *
 * @brief    This functions is used to initialize a stack with Redzone checking.
 *
 * @param    p_base      Pointer to the base of the stack.
 *
 * @param    stk_size    The size of the stack.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
void OS_TaskStkRedzoneInit(CPU_STK      *p_base,
                           CPU_STK_SIZE stk_size)
{
  CPU_STK_SIZE i;

#if (CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO)
  (void)&stk_size;                                              // Prevent compiler warning for not using 'stk_size'

  for (i = 0u; i < OS_CFG_TASK_STK_REDZONE_DEPTH; i++) {
    *(p_base + i) = (CPU_STK)OS_STACK_CHECK_VAL;
  }
#else
  for (i = 0u; i < OS_CFG_TASK_STK_REDZONE_DEPTH; i++) {
    *(p_base + stk_size - 1u - i) = (CPU_STK)OS_STACK_CHECK_VAL;
  }
#endif
}
#endif

/****************************************************************************************************//**
 *                                           OS_TaskChangePrio()
 *
 * @brief    This function is called by the Kernel to perform the actual operation of changing a task's
 *           priority. Priority inheritance is updated if necessary.
 *
 * @param    p_tcb       Pointer to the TCB of the task to change the priority.
 *
 * @param    prio_new    The new priority to give to the task.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TaskChangePrio(OS_TCB  *p_tcb,
                       OS_PRIO prio_new)
{
  OS_TCB *p_tcb_owner;
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  OS_PRIO prio_cur;
#endif

  do {
    p_tcb_owner = DEF_NULL;
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
    prio_cur = p_tcb->Prio;
#endif
    switch (p_tcb->TaskState) {
      case OS_TASK_STATE_RDY:
        OS_RdyListRemove(p_tcb);                                // Remove from current priority
        p_tcb->Prio = prio_new;                                 // Set new task priority
        OS_PrioInsert(p_tcb->Prio);
        if (p_tcb == OSTCBCurPtr) {
          OS_RdyListInsertHead(p_tcb);
        } else {
          OS_RdyListInsertTail(p_tcb);
        }
        break;

      case OS_TASK_STATE_DLY:                                   // Nothing to do except change the priority in the OS_TCB
      case OS_TASK_STATE_SUSPENDED:
      case OS_TASK_STATE_DLY_SUSPENDED:
        p_tcb->Prio = prio_new;                                 // Set new task priority
        break;

      case OS_TASK_STATE_PEND:
      case OS_TASK_STATE_PEND_TIMEOUT:
      case OS_TASK_STATE_PEND_SUSPENDED:
      case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
        p_tcb->Prio = prio_new;                                 // Set new task priority
        switch (p_tcb->PendOn) {                                // What to do depends on what we are pending on
          case OS_TASK_PEND_ON_FLAG:
          case OS_TASK_PEND_ON_Q:
          case OS_TASK_PEND_ON_SEM:
          case OS_TASK_PEND_ON_COND_VAR:
            OS_PendListChangePrio(p_tcb);
            break;

          case OS_TASK_PEND_ON_MUTEX:
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
            OS_PendListChangePrio(p_tcb);
            p_tcb_owner = ((OS_MUTEX *)p_tcb->PendObjPtr)->OwnerTCBPtr;
            if (prio_cur > prio_new) {                          // Are we increasing the priority?
              if (p_tcb_owner->Prio <= prio_new) {              // Yes, do we need to give this prio to the owner?
                p_tcb_owner = DEF_NULL;
              } else {
                OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb_owner, prio_new);
              }
            } else {
              if (p_tcb_owner->Prio == prio_cur) {              // No, is it required to check for a lower prio?
                prio_new = OS_MutexGrpPrioFindHighest(p_tcb_owner);
                prio_new = prio_new > p_tcb_owner->BasePrio ? p_tcb_owner->BasePrio : prio_new;
                if (prio_new == p_tcb_owner->Prio) {
                  p_tcb_owner = DEF_NULL;
                } else {
                  OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb_owner, prio_new);
                }
              }
            }
#endif
            break;

          case OS_TASK_PEND_ON_TASK_Q:
          case OS_TASK_PEND_ON_TASK_SEM:
            break;

          default:
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );
        }
        break;

      case OS_TASK_STATE_DEL:
#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
        return;
#endif

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );
    }
    p_tcb = p_tcb_owner;
  } while (p_tcb != DEF_NULL);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
