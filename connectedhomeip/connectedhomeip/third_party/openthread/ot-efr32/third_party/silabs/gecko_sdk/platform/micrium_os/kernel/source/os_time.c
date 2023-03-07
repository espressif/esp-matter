/***************************************************************************//**
 * @file
 * @brief Kernel - Time Management
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

#if (OS_CFG_TICK_EN == DEF_ENABLED)

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const CPU_CHAR *os_time__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/*****************************************************************************************************//**
 *                                         OSTimeTickRateHzGet()
 *
 * @brief    Gets kernel tick rate, in hertz.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *
 * @return   Kernel tick rate, in hertz.
 *******************************************************************************************************/
OS_RATE_HZ OSTimeTickRateHzGet(RTOS_ERR *p_err)
{
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (OSCfg_TickRate_Hz);
}

/*****************************************************************************************************//**
 *                                               OSTimeDly()
 *
 * @brief    Delays the execution of the currently running task until the specified number of system
 *           ticks expires. This directly equates to delaying the current task for some time to
 *           expire. No delay will result if the specified delay is 0. If the specified delay is
 *           greater than 0, this results in a context switch.
 *
 * @param    dly     Value in 'clock ticks' that the task for which will delay. A value of 0 means that
 *                   the CPU is yield to another ready task that runs at the same priority (if any).
 *                   Depending on the option argument, the task will wake up in:
 *                       - OS_OPT_TIME_DLY         dly
 *                       - OS_OPT_TIME_TIMEOUT     dly
 *                       - OS_OPT_TIME_PERIODIC    OSTCBCurPtr.TickCtrPrev + dly
 *
 * @param    opt     Specifies whether 'dly' represents absolute or relative time; default option
 *                   is OS_OPT_TIME_DLY:
 *                       - OS_OPT_TIME_DLY         Specifies delay relative to current time.
 *                       - OS_OPT_TIME_TIMEOUT     Same as OS_OPT_TIME_DLY.
 *                       - OS_OPT_TIME_PERIODIC    Indicates that 'dly' specifies the periodic value
 *                                                 that current time must be a multiple of before the task
 *                                                 will be resumed.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_READY
 *                       - RTOS_ERR_INVALID_STATE
 *                       - RTOS_ERR_NOT_INIT
 *******************************************************************************************************/
void OSTimeDly(OS_TICK  dly,
               OS_OPT   opt,
               RTOS_ERR *p_err)
{
  sl_status_t status;
  CPU_INT32U delay_ticks;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               Not allowed in atomic/critical sections
  OS_ASSERT_DBG_ERR_SET((!CORE_IrqIsDisabled()), *p_err, RTOS_ERR_INVALID_STATE,; );

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return;
  }

  if (OSSchedLockNestingCtr > 0u) {                             // Can't delay when the scheduler is locked
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
    return;
  }

  if (OS_SleeptimerFrequency_Hz == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    return;
  }

  //                                                               Validate 'opt'
  OS_ASSERT_DBG_ERR_SET(((opt == OS_OPT_TIME_DLY)
                         || (opt == OS_OPT_TIME_TIMEOUT)
                         || (opt == OS_OPT_TIME_PERIODIC)), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               Make sure we didn't specify a 0 delay
  OS_ASSERT_DBG_ERR_SET((dly != 0u) || (opt != OS_OPT_TIME_PERIODIC), *p_err, RTOS_ERR_INVALID_ARG,; );

  if ((opt == OS_OPT_TIME_DLY)
      || (opt == OS_OPT_TIME_TIMEOUT)) {
    delay_ticks = dly;
  } else {
    OS_TICK tick_ctr;
    OS_TICK tick_os_temp;

    tick_ctr = sl_sleeptimer_get_tick_count();
    tick_os_temp = (uint64_t)((uint64_t)tick_ctr * (uint64_t)OSCfg_TickRate_Hz) / OS_SleeptimerFrequency_Hz;

    if (OSTCBCurPtr->IsTickCtrPrevValid) {
      CPU_INT32S diff;

      OSTCBCurPtr->TickCtrPrev += dly;

      diff        = OSTCBCurPtr->TickCtrPrev - tick_os_temp;
      delay_ticks = (diff > 0) ? (CPU_INT32U)diff : 0u;
    } else {
      delay_ticks                     = dly;
      OSTCBCurPtr->TickCtrPrev        = tick_os_temp + dly;     // ... first time we load .TickCtrPrev
      OSTCBCurPtr->IsTickCtrPrevValid = DEF_YES;
    }

    if (OSDelayMaxTick != 0 && OSTCBCurPtr->TickCtrPrev >= OSDelayMaxTick) {
      OSTCBCurPtr->TickCtrPrev -= OSDelayMaxTick;               // Adjust in case of sleep timer overflow
    }
  }

  if (delay_ticks > 0u) {
    uint32_t delay;

    // Check for potential overflow
    if (OSDelayMaxTick != 0 && delay_ticks >= OSDelayMaxTick) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
      return;
    }

    delay = (uint64_t)(((uint64_t)delay_ticks * (uint64_t)OS_SleeptimerFrequency_Hz) + (OSCfg_TickRate_Hz - 1u)) / OSCfg_TickRate_Hz;

    CORE_ENTER_ATOMIC();

    status = sl_sleeptimer_start_timer(&OSTCBCurPtr->TimerHandle,
                                       delay,
                                       OS_TimerCallback,
                                       (void *)OSTCBCurPtr,
                                       0u,
                                       0u);
    if (status != SL_STATUS_OK) {
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      return;
    }

    OSTCBCurPtr->TaskState = OS_TASK_STATE_DLY;
    OS_TRACE_TASK_DLY(dly);
    OS_RdyListRemove(OSTCBCurPtr);                              // Remove current task from ready list
    CORE_EXIT_ATOMIC();
  } else {
    OS_RDY_LIST *p_rdy_list;

    CORE_ENTER_ATOMIC();
    p_rdy_list = &OSRdyList[OSPrioCur];                         // Can't yield if it's the only task at that priority
    if (p_rdy_list->HeadPtr == p_rdy_list->TailPtr) {
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      return;
    }

    OS_RdyListMoveHeadToTail(p_rdy_list);
    CORE_EXIT_ATOMIC();
  }

  OSSched();                                                    // Find next task to run!

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               OSTimeDlyHMSM()
 *
 * @brief    Delay execution of the currently running task until some time expires. This call allows
 *           you to specify the delay time in HOURS, MINUTES, SECONDS, and MILLISECONDS instead of ticks.
 *
 * @param    hours       Specifies the number of hours that the task will be delayed (max. is 999 if the
 *                       tick rate is 1000 Hz or less otherwise, a higher value would overflow a 32-bit
 *                       unsigned counter). (max. 99 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
 *
 * @param    minutes     Specifies the number of minutes. (max. 59 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
 *
 * @param    seconds     Specifies the number of seconds. (max. 59 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
 *
 * @param    milli       Specifies the number of milliseconds. (max. 999 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
 *
 * @param    opt         Specifies time delay bit-field options logically OR'd; default options marked
 *                       with *** :
 *                           - OS_OPT_TIME_DLY         *** Specifies a relative time from the current
 *                                                   value of OSTickCtr.
 *                           - OS_OPT_TIME_TIMEOUT     Same as OS_OPT_TIME_DLY.
 *                           - OS_OPT_TIME_PERIODIC    Indicates that the delay specifies the
 *                                                     periodic value that OSTickCtr must reach
 *                                                     before the task will be resumed.
 *                           - OS_OPT_TIME_HMSM_STRICT    *** Strictly allows only hours        (0...99)
 *                                                                                 minutes      (0...59)
 *                                                                                 seconds      (0...59)
 *                                                                                 milliseconds (0...999)
 *                           - OS_OPT_TIME_HMSM_NON_STRICT    Allows any value of  hours        (0...999)
 *                                                                                 minutes      (0...9999)
 *                                                                                 seconds      (0...65535)
 *                                                                                 milliseconds (0...4294967295)
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_ARG
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *
 * @note     (1) The resolution of milliseconds depends on the tick rate. For example, you cannot
 *                   do a 10 mS delay if the ticker interrupts every 100 mS. In this case, the delay would
 *                   be set to 0. The actual delay is rounded to the nearest tick.
 *
 * @note     (2) Although this function allows you to delay a task for many hours, it is not recommended
 *                   to put a task to sleep for that long.
 *******************************************************************************************************/
void OSTimeDlyHMSM(CPU_INT16U hours,
                   CPU_INT16U minutes,
                   CPU_INT16U seconds,
                   CPU_INT32U milli,
                   OS_OPT     opt,
                   RTOS_ERR   *p_err)
{
  OS_OPT opt_time;
  OS_TICK ticks;
  OS_TICK hours_tick;
  OS_TICK minutes_tick;
  OS_TICK overflow_check;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  opt_time = opt & OS_OPT_TIME_MASK;                            // Retrieve time options only.

  OS_ASSERT_DBG_ERR_SET((DEF_BIT_IS_SET_ANY(opt, ~OS_OPT_TIME_OPTS_MASK) != DEF_YES), *p_err, RTOS_ERR_INVALID_ARG,; );

  if (DEF_BIT_IS_SET(opt, OS_OPT_TIME_HMSM_NON_STRICT) != DEF_YES) {
    OS_ASSERT_DBG_ERR_SET((milli <= 999u), *p_err, RTOS_ERR_INVALID_ARG,; );
    OS_ASSERT_DBG_ERR_SET((seconds <= 59u), *p_err, RTOS_ERR_INVALID_ARG,; );
    OS_ASSERT_DBG_ERR_SET((minutes <= 59u), *p_err, RTOS_ERR_INVALID_ARG,; );
    OS_ASSERT_DBG_ERR_SET((hours <= 99u), *p_err, RTOS_ERR_INVALID_ARG,; );
  } else {
    OS_ASSERT_DBG_ERR_SET((minutes <= 9999u), *p_err, RTOS_ERR_INVALID_ARG,; );
    OS_ASSERT_DBG_ERR_SET((hours <= 999u), *p_err, RTOS_ERR_INVALID_ARG,; );
  }

  hours_tick = (OS_TICK)hours * (OS_TICK)3600u;
  minutes_tick = (OS_TICK)minutes * (OS_TICK)60u;

  //                                                               Convert everything to milliseconds
  overflow_check = ((hours_tick + minutes_tick + (OS_TICK)seconds) * (OS_TICK)1000u) + milli;

  //                                                               Check for potential overflow
  if (OSDelayMaxMilli != 0 && overflow_check >= OSDelayMaxMilli) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    return;
  }

  //                                                               Compute the total number of clock ticks required..   */
  //                                                               .. (rounded to the nearest tick)                     */

  ticks = (hours_tick + minutes_tick + (OS_TICK)seconds) * OSCfg_TickRate_Hz
          + (OSCfg_TickRate_Hz * ((OS_TICK)milli + (OS_TICK)500u / OSCfg_TickRate_Hz)) / (OS_TICK)1000u;

  OSTimeDly(ticks, opt_time, p_err);
}

/****************************************************************************************************//**
 *                                               OSTimeDlyResume()
 *
 * @brief    Resumes a task that has been delayed through a call to either OSTimeDly() or
 *           OSTimeDlyHMSM(). Note that you cannot call this function to resume a task that is waiting
 *           for an event with timeout.
 *
 * @param    p_tcb   Pointer to the TCB of the task to resume.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_TASK_SUSPENDED
 *                       - RTOS_ERR_INVALID_STATE
 *                       - RTOS_ERR_NOT_READY
 *******************************************************************************************************/
void OSTimeDlyResume(OS_TCB   *p_tcb,
                     RTOS_ERR *p_err)
{
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               User must supply a valid OS_TCB
  OS_ASSERT_DBG_ERR_SET((p_tcb != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return;
  }

  CORE_ENTER_ATOMIC();
  switch (p_tcb->TaskState) {
    case OS_TASK_STATE_RDY:                                     // Cannot Abort delay if task is ready
    case OS_TASK_STATE_PEND:
    case OS_TASK_STATE_PEND_TIMEOUT:
    case OS_TASK_STATE_SUSPENDED:
    case OS_TASK_STATE_PEND_SUSPENDED:
    case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;

    case OS_TASK_STATE_DLY:
#if (OS_CFG_TICK_EN == DEF_ENABLED)
      p_tcb->TaskState = OS_TASK_STATE_RDY;
      OS_RdyListInsert(p_tcb);                                  // Add to ready list                                    */
      status = sl_sleeptimer_stop_timer(&p_tcb->TimerHandle);
      RTOS_ASSERT_CRITICAL((status == SL_STATUS_OK), RTOS_ERR_FAIL,; );
#endif
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_DLY_SUSPENDED:
#if (OS_CFG_TICK_EN == DEF_ENABLED)
      p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;
      status = sl_sleeptimer_stop_timer(&p_tcb->TimerHandle);
      RTOS_ASSERT_CRITICAL((status == SL_STATUS_OK), RTOS_ERR_FAIL,; );
#endif
      CORE_EXIT_ATOMIC();
      break;

    case OS_TASK_STATE_DEL:
#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
      CORE_EXIT_ATOMIC();
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;
#endif

    default:
      CORE_EXIT_ATOMIC();
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );
  }

  OSSched();
}

/****************************************************************************************************//**
 *                                               OSTimeGet()
 *
 * @brief    Used by your application to obtain the current value of the counter to keep track of
 *           the number of clock ticks.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_INIT
 *
 * @return   The current tick count.
 *
 * @note     (1) The conversion is based on a 64 bits large hardware tick counter. The overflow of the
 *               64 bits counter during the calculation is not handle in this function.
 *               It is assumed that with a 64 bits counter running on a LF clock the calculation would
 *               overflow in millions of years making it acceptable.
 *******************************************************************************************************/
OS_TICK OSTimeGet(RTOS_ERR *p_err)
{
  CPU_INT64U timer_ticks;
  OS_TICK ticks;

  if (OS_SleeptimerFrequency_Hz == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    return (0u);
  }

  timer_ticks = sl_sleeptimer_get_tick_count64();
  ticks = (CPU_INT64U)(timer_ticks * (CPU_INT64U)OSCfg_TickRate_Hz) / OS_SleeptimerFrequency_Hz;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (ticks);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (OS_CFG_TICK_EN == DEF_ENABLED)
#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
