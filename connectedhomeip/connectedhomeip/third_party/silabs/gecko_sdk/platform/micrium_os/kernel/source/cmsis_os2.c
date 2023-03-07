/***************************************************************************//**
 * @file cmsis_os2.c
 * @brief CMSIS-RTOS2 -> MicriumOS Emulation Layer
 ******************************************************************************/

/***************************************************************************//**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/

/***************************************************************************//**
 * Copyright (c) , Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                                         I N C L U D E S
 ****************************************************************************************************
 ****************************************************************************************************
 */

#include <string.h>
#include <stdlib.h>

#include "kernel/include/os.h"
#include "em_core.h"
#include "rtos_err.h"
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "common/source/rtos/rtos_utils_priv.h"
#include "sl_sleeptimer.h"

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                                      F I L E   G L O B A L S
 ****************************************************************************************************
 ****************************************************************************************************
 */

#define CMSIS_DEFAULT_STACK_SIZE 1024u
#define RTOS_MODULE_CUR RTOS_CFG_MODULE_KERNEL

#if defined CMSIS_RTOS2_TIMER_TASK_EN && (CMSIS_RTOS2_TIMER_TASK_EN == DEF_ENABLED)

#ifndef CMSIS_RTOS2_TIMER_TASK_STACK_SIZE
#error CMSIS Timer task stack size not configured
#endif

#ifndef CMSIS_RTOS2_TIMER_TASK_PRIO
#error CMSIS Timer task priority not configured
#endif

#ifndef CMSIS_RTOS2_TIMER_TASK_QUEUE_SIZE
#error CMSIS Timer task queue size not configured
#endif

static OS_TCB   timer_task_tcb;
static CPU_STK  timer_task_stack[CMSIS_RTOS2_TIMER_TASK_STACK_SIZE];
static OS_Q     timer_msg_queue;

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                 L O C A L   F U N C T I O N S
 ********************************************************************************************************
 *******************************************************************************************************/
#if defined CMSIS_RTOS2_TIMER_TASK_EN && (CMSIS_RTOS2_TIMER_TASK_EN == DEF_ENABLED)

static void sleeptimer_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  RTOS_ERR err;
  (void)handle;

  OSQPost(&timer_msg_queue,
          (void *)data,
          sizeof(void *),
          OS_OPT_POST_FIFO,
          &err
          );
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(err),; );
}

static void timer_task(void *arg)
{
  RTOS_ERR err;
  OS_MSG_SIZE size;
  osTimer_t *p_tmr;
  (void)arg;

  while (1) {
    p_tmr = (osTimer_t *)OSQPend(&timer_msg_queue,
                                 0,
                                 OS_OPT_PEND_BLOCKING,
                                 &size,
                                 NULL,
                                 &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      continue;
    }

    if (p_tmr != NULL) {
      p_tmr->callback(p_tmr->callback_data);
    }
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                 G L O B A L   F U N C T I O N S
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                            I N F O R M A T I O N   A N D   C O N T R O L
 ****************************************************************************************************
 ****************************************************************************************************
 */

/*
 ****************************************************************************************************
 *                                       osKernelInitialize()
 *
 * Description: The function 'osKernelInitialize()' initializes the RTOS Kernel.  Before it is successfully
 *              executed, only the functions 'osKernelGetInfo()' and 'osKernelGetState()' may be called.
 *
 * Arguments  : None
 *
 * Returns    : osOK             in case of success.
 *              osError          if an unspecific error occurred.
 *              osErrorISR       if called from an Interrupt Service Routine.
 *              osErrorNoMemory  if no memory could be reserved for the operation.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osKernelInitialize(void)
{
  RTOS_ERR   err;

  if (OSRunning == OS_STATE_OS_RUNNING) {
    if (CORE_InIrqContext() == true) {
      return osErrorISR;
    } else {
      return osError;
    }
  }

  OSInit(&err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return osError;
  }

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
  OSSchedRoundRobinCfg(DEF_TRUE, 0, &err);
#endif

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return osError;
  }

#if defined CMSIS_RTOS2_TIMER_TASK_EN && (CMSIS_RTOS2_TIMER_TASK_EN == DEF_ENABLED)
  OSTaskCreate(&timer_task_tcb,
               "CMSIS RTOS2 Timer Task",
               timer_task,
               (void *)0,
               CMSIS_RTOS2_TIMER_TASK_PRIO,
               &timer_task_stack[0],
               ((CMSIS_RTOS2_TIMER_TASK_STACK_SIZE * 10u) / 100u),
               CMSIS_RTOS2_TIMER_TASK_STACK_SIZE,
               0,
               0,
               (void *)0,
               OS_OPT_TASK_STK_CHK + OS_OPT_TASK_STK_CLR,
               &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return osError;
  }

  OSQCreate(&timer_msg_queue,
            "CMSIS RTOS2 Timer Queue",
            CMSIS_RTOS2_TIMER_TASK_QUEUE_SIZE,
            &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return osError;
  }

#endif
  return osOK;
}

/*
 ****************************************************************************************************
 *                                       osKernelGetInfo()
 *
 * Description: The function 'osKernelGetInfo()' retrieves the API and kernel version of the underlying
 *              RTOS kernel and a human readable identifier string for the kernel.  It can be safely
 *              called before the RTOS is initialized or started (call to 'osKernelInitialize()' or
 *              'osKernelStart()').
 *
 * Arguments  : version          pointer to a buffer for retrieving the version information.
 *                               Format (decimal):
 *                                 MMmmmrrrr
 *                                    MM  : Major
 *                                    mmm : Minor
 *                                    rrrr: Revision
 *
 *              id_buf           pointer to the buffer for retrieving kernel identification string.
 *
 *              id_size          size    of the buffer for the kernel identification string
 *
 * Returns    : osOK             in case of success.
 *              osError          if an unspecific error occurred.
 *
 * Note(s)    : None
 ****************************************************************************************************
 */
osStatus_t  osKernelGetInfo(osVersion_t   *version,
                            char          *id_buf,
                            uint32_t      id_size)
{
  CPU_INT16U  os_version;
  RTOS_ERR    err;
  uint32_t    os_major;
  uint32_t    os_minor;
  uint32_t    os_revision;

  if (id_buf == (char *)0) {
    return osError;
  }

  if (id_size < 20u) {
    return osError;
  }

  os_version = OSVersion(&err);

  os_major = os_version / 10000u;
  os_minor = (os_version - os_major * 10000u) / 100u;
  os_revision = os_version % 100u;
  version->api = (os_major * 10000000u)
                 + (os_minor *    10000u)
                 +  os_revision;
  version->kernel = version->api;

  strcpy(id_buf, "MicriumOS");

  return osOK;
}

/*
 ****************************************************************************************************
 *                                       osKernelGetState()
 *
 * Description: The function 'osKernelGetState()' returns the current state of the kernel and can be
 *              safely called before the RTOS is initialized or started (call to 'osKernelInitialize()'
 *              or 'osKernelStart()').
 *
 *              In case it fails it will return 'osKernelError', otherwise it returns the kernel state
 *              (refer to 'osKernelState_t' for the list of kernel states).
 *
 * Arguments  : None
 *
 * Returns    : osKernelInactive        The kernel is not ready yet
 *              osKernelReady           The kernel is not running yet
 *              osKernelRunning         The kernel is initialized and running
 *              osKernelLocked          The kernel was locked by 'OSKernelLock()'
 *              osKernelSuspended       The kernel was suspended by 'OSKernelSuspend()'    (NOT SUPPORTED)
 *              osKernelError           An error occurred
 *              osKernelReserved        Prevents enum down-size compiler optimization
 *
 * Note(s)    : None
 ****************************************************************************************************
 */
osKernelState_t  osKernelGetState(void)
{
  if (OSInitialized == DEF_TRUE) {
    if (OSRunning == OS_STATE_OS_RUNNING) {
      if (OSSchedLockNestingCtr > 0u) {
        return osKernelLocked;
      } else {
        return osKernelRunning;
      }
    } else {
      return osKernelReady;
    }
  }

  return osKernelInactive;
}

/*
 ****************************************************************************************************
 *                                         osKernelStart()
 *
 * Description: The function 'osKernelStart()' starts the RTOS kernel and begins thread switching.
 *              It will not return to its calling function in case of success.  Before it is successfully
 *              executed, only the functions 'osKernelGetInfo()', 'osKernelGetState()', and object creation
 *              functions ('osXxxNew()') may be called.
 *
 *              At least one initial thread should be created prior 'osKernelStart()', see 'osThreadNew()'.
 *
 * Arguments  : None
 *
 * Returns    : osError                 If an unspecified error occurs
 *              osErrorISR              If called from an ISR
 *
 * Note(s)    : 1) This function cannot be called from Interrupt Service Routines
 *
 *              2) This function should not be called because MicriumOS should already be initialized.
 *
 *              3) CMSIS-RTOS2 requires atleast 56 priority levels. OS_CFG_PRIO_MAX must be >= 56.
 ****************************************************************************************************
 */
osStatus_t  osKernelStart(void)
{
  RTOS_ERR  err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  if (OSRunning == OS_STATE_OS_RUNNING) {
    return osError;
  }

#if OS_CFG_PRIO_MAX >= osPriorityISR
  OSStart(&err);

  (void)err;   // There is no point in checking the error here
               // because the function will not return in case of success

  return osError;
#else
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);   // CMSIS-RTOS2 requires atleast 56 priority levels
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                         osKernelLock()
 *
 * Description: The function 'osKernelLock()' allows to lock all task switches.  It returns the previous
 *              value of the lock state (1 if it was locked, 0 if it was unlocked), or a negative number
 *              representing an error code otherwise (refer to 'osStatus_t').
 *
 * Arguments  : None
 *
 * Returns    : osError                 If an unspecified error occurs
 *              osErrorISR              If called from an ISR
 *
 * Note(s)    : 1) This function cannot be called from Interrupt Service Routines
 *
 *              2) The MicriumOS equivalent function can be called from an ISR.
 ****************************************************************************************************
 */
int32_t  osKernelLock(void)
{
  RTOS_ERR  err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  if (OSSchedLockNestingCtr > 0u) {
    return 1u;
  }

  OSSchedLock(&err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return osError;
  }

  return 0u;
}

/*
 ****************************************************************************************************
 *                                         osKernelUnlock()
 *
 * Description: The function 'osKernelUnlock()' resumes from 'osKernelLock()'.  It returns the previous
 *              value of the lock state (1 if it was locked, 0 if it was unlocked), or a negative number
 *              representing an error code otherwise (refer to 'osStatus_t').
 *
 * Arguments  : None
 *
 * Returns    : osError                 If an unspecified error occurs
 *              osErrorISR              If called from an ISR
 *
 * Note(s)    : 1) This function cannot be called from Interrupt Service Routines
 *
 *              2) The MicriumOS equivalent function can be called from an ISR.
 *
 *              3) MicriumOS supports multiple lock levels but CMSIS-RTOS doesn't
 ****************************************************************************************************
 */
int32_t  osKernelUnlock(void)
{
  OS_NESTING_CTR  ctr;
  RTOS_ERR        err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  ctr = OSSchedLockNestingCtr;                    // Get the current value of the lock counter

  if (ctr > 1u) {                                 // CMSIS-RTOS only supports 1 lock level
    ctr = 1u;
  }

  OSSchedUnlock(&err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return osError;
  }

  return ctr;
}

/*
 ****************************************************************************************************
 *                                         osKernelRestoreLock()
 *
 * Description: The function 'osKernelRestoreLock()' restores the previous lock state after 'osKernelLock()'
 *              or 'osKernelUnlock()'.
 *
 *              The argument lock specifies the lock state as obtained by 'osKernelLock()' or 'osKernelUnlock()'.
 *
 *              The function returns the new value of the lock state (1 if it was locked, 0 if it was unlocked),
 *              or a negative number representing an error code otherwise (refer to 'osStatus_t').
 *
 * Arguments  : lock                    new lock state: 1 == locked, 0 == not locked
 *
 * Returns    : osError                 If an unspecified error occurs
 *              osErrorISR              If called from an ISR
 *
 * Note(s)    : 1) This function cannot be called from Interrupt Service Routines
 *
 *              2) There are no MicriumOS equivalent function to emulate this behavior.
 ****************************************************************************************************
 */
int32_t  osKernelRestoreLock(int32_t  lock)
{
  int32_t  new_state;
  RTOS_ERR   err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  if (lock > 0) {
    OSSchedLock(&err);
    new_state = 1u;
  } else {
    OSSchedUnlock(&err);
    new_state = 0u;
  }

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return osError;
  }

  return new_state;
}

/*
 ****************************************************************************************************
 *                                          osKernelSuspend()
 *
 * Description: CMSIS-RTOS provides extension for tick-less operation which is useful for applications
 *              that use extensively low-power modes where the SysTick timer is also disabled.  To provide
 *              a time-tick in such power-saving modes a wake-up timer is used to derive timer intervals.
 *              The function 'osKernelSuspend()' suspends the RTX kernel scheduler and thus enables sleep
 *              modes.
 *
 *              The return value can be used to determine the amount of system ticks until the next
 *              tick-based kernel event will occure, i.e. a delayed thread becomed ready again. It is
 *              recommended to set up the low power timer to generate a wake-up interrupt based on this
 *              return value.
 *
 * Arguments  : None
 *
 * Returns    : time in 'ticks', for how long the system can sleep or power-down
 *
 * Note(s)    : 1) This function cannot be called from Interrupt Service Routines
 *
 *              2) There are no MicriumOS equivalent function to emulate this behavior.
 ****************************************************************************************************
 */
uint32_t  osKernelSuspend(void)
{
  if (CORE_InIrqContext() == true) {
    return 0u;
  }

  return 0u;
}

/*
 ****************************************************************************************************
 *                                          osKernelResume()
 *
 * Description: CMSIS-RTOS provides extension for tick-less operation which is useful for applications
 *              that use extensively low-power modes where the SysTick timer is also disabled.  To provide
 *              a time-tick in such power-saving modes a wake-up timer is used to derive timer intervals.
 *              The function 'osKernelSuspend()' suspends the RTX kernel scheduler and thus enables sleep
 *              modes.
 *
 * Arguments  : time in 'ticks', for how long the system can sleep or power-down
 *
 * Returns    : None
 *
 * Note(s)    : 1) This function cannot be called from Interrupt Service Routines
 *
 *              2) There are no MicriumOS equivalent function to emulate this behavior.
 ****************************************************************************************************
 */
void  osKernelResume(uint32_t  sleep_ticks)
{
  (void)sleep_ticks;

  if (CORE_InIrqContext() == true) {
    return;
  }
}

/*
 ****************************************************************************************************
 *                                     osKernelGetTickCount()
 *
 * Description: The function 'osKernelGetTickCount()' returns the current RTOS kernel tick count.
 *
 * Arguments  : None
 *
 * Returns    : the kernel current tick count
 *
 * Note(s)    : None
 ****************************************************************************************************
 */
uint32_t  osKernelGetTickCount(void)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  RTOS_ERR err;
  OS_TICK ticks;

  ticks = OSTimeGet(&err);
  (void)err;   // CMSIS RTOS API does not give us a way to propagate errors here

  return ticks;
#else
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, 0u);
  return 0u;
#endif //#if (OS_CFG_TICK_EN == DEF_ENABLED)
}

/*
 ****************************************************************************************************
 *                                     osKernelGetTickFreq()
 *
 * Description: The function 'osKernelGetTickFreq()' returns the frequency of the current RTOS kernel tick.
 *
 * Arguments  : None
 *
 * Returns    : the kernel tick frequency in Hz
 *
 * Note(s)    : None
 ****************************************************************************************************
 */
uint32_t  osKernelGetTickFreq(void)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  uint32_t  tick_freq;
  RTOS_ERR  err;

  tick_freq = OSTimeTickRateHzGet(&err);

  return tick_freq;
#else
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, 0u);
  return 0u;
#endif
}

/*
 ****************************************************************************************************
 *                                    osKernelGetSysTimerCount()
 *
 * Description: The function 'osKernelGetSysTimerCount() returns the current RTOS kernel system timer
 *              as a 32-bit value.  The value is a rolling 32-bit counter that is composed of the kernel
 *              system interrupt timer value and the counter that counts these interrupts (RTOS kernel ticks).
 *
 *              This function allows the implementation of very short timeout checks below the RTOS tick
 *              granularity.  Such checks might be required when checking for a busy status in a device
 *              or peripheral initialization routine.
 *
 * Arguments  : None
 *
 * Returns    : the kernel timer tick count
 ****************************************************************************************************
 */
uint32_t  osKernelGetSysTimerCount(void)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  uint32_t ticks;

  ticks = sl_sleeptimer_get_tick_count();

  return ticks;
#else
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, 0u);
  return 0u;
#endif
}

/*
 ****************************************************************************************************
 *                                    osKernelGetSysTimerFreq()
 *
 * Description: The function 'osKernelGetSysTimerFreq()' returns the frequency of the current RTOS
 *              kernel system timer.
 *
 * Arguments  : None
 *
 * Returns    : the kernel system timer frequency in Hz
 ****************************************************************************************************
 */
uint32_t  osKernelGetSysTimerFreq(void)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  uint32_t  tick_freq;

  tick_freq = sl_sleeptimer_get_timer_frequency();

  return tick_freq;
#else
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, 0u);
  return 0u;
#endif
}

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                                     G E N E R I C   W A I T
 ****************************************************************************************************
 ****************************************************************************************************
 */

/*
 ****************************************************************************************************
 *                                             osDelay()
 *
 * Description: The function 'osDelay()' waits for a time period specified in kernel ticks.  For a value
 *              of 1 the system waits until the next timer tick occurs.  The actual time delay may be up
 *              to one timer tick less than specified, i.e. calling 'osDelay(1)' right before the next
 *              system tick occurs the thread is rescheduled immediately.
 *
 *              The delayed thread is put into the BLOCKED state and a context switch occurs immediately.
 *              The thread is automatically put back to the READY state after the given amount of ticks
 *              has elapsed. If the thread will have the highest priority in READY state it will being
 *              scheduled immediately.
 *
 * Arguments  : ticks       is the amount of time (in ticks) that the task will be blocked
 *
 * Returns    : osOK        if the call is successful
 *              osErrorISR  if called from an ISR
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osDelay(uint32_t  ticks)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  RTOS_ERR  err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  OSTimeDly((OS_TICK)ticks, OS_OPT_TIME_DLY, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return osError;
  }

  return osOK;
#else
  (void)ticks;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif // #if (OS_CFG_TICK_EN == DEF_ENABLED)
}

/*
 ****************************************************************************************************
 *                                           osDelayUntil()
 *
 * Description: The function 'osDelayUntil()' waits until an absolute time (specified in kernel ticks)
 *              is reached.
 *
 *              The corner case when the kernel tick counter overflows is handled by 'osDelayUntil()'.
 *              Thus it is absolutely legal to provide a value which is lower than the current tick
 *              value, i.e. returned by 'osKernelGetTickCount()'.  Typically as a user you do not have
 *              to take care about the overflow.  The only limitation you have to have in mind is that
 *              the maximum delay is limited to 0x7FFFFFFF ticks.
 *
 *              The delayed thread is put into the BLOCKED state and a context switch occurs immediately.
 *              The thread is automatically put back to the READY state when the given time is reached.
 *              If the thread will have the highest priority in READY state it will being scheduled immediately.
 *
 * Arguments  : ticks       is the absolute time (in ticks) at which the task will wake up.
 *
 * Returns    : osOK        if the call is successful
 *              osParameter if time cannot be handled (out of bounds)
 *              osErrorISR  if called from an ISR
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osDelayUntil(uint32_t  ticks)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  RTOS_ERR err;
  OS_TICK  until;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  until = ticks - OSTimeGet(&err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return osError;
  }

  if (until > 0x7FFFFFF0u) {                      // Allow for 16 ticks of latency
    until = 0u;
  }

  OSTimeDly((OS_TICK)until, OS_OPT_TIME_DLY, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return osErrorParameter;
  }

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      return osOK;

    case RTOS_ERR_INVALID_ARG:
    case RTOS_ERR_OS_SCHED_LOCKED:
    default:
      return osErrorParameter;
  }
#else
  (void)ticks;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif // #if (OS_CFG_TICK_EN == DEF_ENABLED)
}

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                                      E V E N T   F L A G S
 ****************************************************************************************************
 ****************************************************************************************************
 */

/*
 ****************************************************************************************************
 *                                         osEventFlagsNew()
 *
 * Description: The function 'osEventFlagsNew()' creates a new event flags object that is used to send
 *              events across threads and returns the pointer to the event flags object identifier or
 *              NULL in case of an error.  It can be safely called before the RTOS is started
 *              (call to 'osKernelStart()'), but not before it is initialized (call to 'osKernelInitialize()').
 *
 *              The parameter 'attr' sets the event flags attributes (refer to 'osEventFlagsAttr_t').
 *              Default attributes will be used if set to NULL, i.e. kernel memory allocation is used for
 *              the event control block.
 *
 * Arguments  : attr     sets the event flags attributes or default values if NULL
 *
 *                       .name        is an ASCII string used to name the mutex
 *
 *                       .attr_bits   Not currently used, MUST be set to 0
 *
 *                       .cb_mem      Pointer to storage area for the event flags control block
 *
 *                       .cb_size     Size of the memory storage for the event flags control block
 *
 * Returns    : The event flags ID
 *
 * Note(s)    : 1) The event flags control block is assumed to be of type 'osEentFlags_t' which for the
 *                 CMSIS-RTOS adaptation layer is defined as an OS_FLAG_GRP in MicriumOS.
 ****************************************************************************************************
 */
osEventFlagsId_t  osEventFlagsNew(const  osEventFlagsAttr_t *attr)
{
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  osEventFlags_t *p_flags;
  CPU_CHAR       *p_name;
  RTOS_ERR        err;

  if (CORE_InIrqContext() == true) {
    return (osEventFlagsId_t)0;
  }

  p_name = (CPU_CHAR *)"EventFlags";
  if (attr == 0) {
    p_flags = (osEventFlags_t *)malloc(sizeof(osEventFlags_t));
    if (p_flags == (osEventFlags_t *)0) {
      return (osEventFlagsId_t)0;
    }
    p_flags->dyn_alloc = DEF_TRUE;
  } else {
    if (attr->cb_mem == (void *)0) {
      p_flags = (osEventFlags_t *)malloc(sizeof(osEventFlags_t));
      if (p_flags == (osEventFlags_t *)0) {
        return (osEventFlagsId_t)0;
      }
      p_flags->dyn_alloc = DEF_TRUE;
    } else {
      if (attr->cb_size < sizeof(osEventFlags_t) || (uint32_t)attr->cb_mem % sizeof(CPU_ALIGN)) {
        return (osEventFlagsId_t)0;
      }
      p_flags = (osEventFlags_t*)attr->cb_mem;
      p_flags->dyn_alloc = DEF_FALSE;
    }
    if (attr->name != (const char *)0) {
      p_name = (CPU_CHAR *)attr->name;
    }
  }

  p_flags->flags = 0;

  OSFlagCreate(&p_flags->flag_grp, p_name, p_flags->flags, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    if (p_flags->dyn_alloc == DEF_TRUE) {
      free(p_flags);
    }
    return (osEventFlagsId_t)0;
  }

  return (osEventFlagsId_t)p_flags;
#else
  (void)attr;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, (osEventFlagsId_t)0);
  return (osEventFlagsId_t)0;
#endif
}

/*
 ****************************************************************************************************
 *                                         osEventFlagsSet()
 *
 * Description: The function 'osEventFlagsSet()' sets the event flags specified by the parameter 'flags'
 *              in an event flags object specified by parameter 'ef_id'.  All threads waiting for the
 *              flag set will be notified to resume from BLOCKED state.
 *
 *              The function returns the event flags stored in the event control block or an error code
 *              (highest bit is set, refer to Flags Functions Error Codes).
 *
 * Arguments  : ef_id    Is the events flags ID
 *
 *              flags    The flags to set
 *
 * Returns    : The new state of the event flags
 *              osFlagsErrorUnknown     unspecified error.
 *              osFlagsErrorParameter   parameter ef_id does not identify a valid event flags object or flags has highest bit set.
 *              osFlagsErrorResource    the event flags object is in an invalid state.
 *
 * Note(s)    : None
 ****************************************************************************************************
 */
uint32_t  osEventFlagsSet(osEventFlagsId_t  ef_id,
                          uint32_t          flags)
{
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  osEventFlags_t *p_flags;
  OS_FLAG_GRP    *p_grp;
  OS_TCB         *p_tcb;
  RTOS_ERR        err;
  uint32_t        rdy_flags;
  uint32_t        new_flags;
  uint32_t        ret_flags;
  CORE_DECLARE_IRQ_STATE;

  p_flags = (osEventFlags_t *)ef_id;
  if (p_flags == (osEventFlags_t *)0) {
    return osFlagsErrorParameter;
  }
  p_grp = &p_flags->flag_grp;

  if ((flags & 0x80000000u) == 0x80000000u) {                    // MSB cannot be set
    return osFlagsErrorParameter;
  }

  CORE_ENTER_ATOMIC();

  p_flags->flags |= flags;
  new_flags = p_flags->flags;                                   // New flags after set

  p_tcb = p_grp->PendList.HeadPtr;                              // Loop over pending tasks
  while (p_tcb != DEF_NULL) {
    if (p_tcb->FlagsOpt & OS_OPT_PEND_FLAG_CONSUME) {           // Pender might consume the flags?
      switch (p_tcb->FlagsOpt & OS_OPT_PEND_FLAG_MASK) {
        case OS_OPT_PEND_FLAG_SET_ALL:                          // -> Pender waiting for all flags to be set
          rdy_flags = (new_flags & p_tcb->FlagsPend);
          if (rdy_flags == p_tcb->FlagsPend) {                  //    -> Pender will really consume?
            new_flags &= ~rdy_flags;                            //    -> Simulate the consumption
          }
          break;

        case OS_OPT_PEND_FLAG_SET_ANY:                          // -> Pender waiting for any flags to be set
          rdy_flags = (new_flags & p_tcb->FlagsPend);
          if (rdy_flags != 0u) {                                //    -> Pender will really consume?
            new_flags &= ~rdy_flags;                            //    -> Simulate the consumption
          }
          break;

        default:
          break;
      }
    }
    p_tcb = p_tcb->PendNextPtr;                                 // Point to next task waiting for event flag(s)
  }

  CORE_EXIT_ATOMIC();

  OSFlagPost(p_grp, flags, OS_OPT_POST_FLAG_SET, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      ret_flags = new_flags;
      break;

    default:
      ret_flags = osFlagsErrorResource;
      break;
  }
  return ret_flags;
#else
  (void)ef_id;
  (void)flags;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osFlagsErrorUnknown);
  return osFlagsErrorUnknown;
#endif
}

/*
 ****************************************************************************************************
 *                                        osEventFlagsClear()
 *
 * Description: The function 'osEventFlagsClear()' clears the event flags specified by the parameter 'flags'
 *              in an event flags object specified by parameter 'ef_id'.
 *
 *              The function returns the event flags stored in the event control block prior to clearing
 *              the flags or an error code (highest bit is set, refer to Flags Functions Error Codes).
 *
 * Arguments  : ef_id    Is the events flags ID
 *
 *              flags    The flags to clear
 *
 * Returns    : osFlagsErrorUnknown     unspecified error.
 *              osFlagsErrorParameter   parameter ef_id does not identify a valid event flags
 *                                      object or flags has highest bit set.
 *              osFlagsErrorResource    the event flags object is in an invalid state.
 *
 * Note(s)    : None
 ****************************************************************************************************
 */
uint32_t  osEventFlagsClear(osEventFlagsId_t  ef_id,
                            uint32_t          flags)
{
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  osEventFlags_t *p_flags;
  RTOS_ERR        err;
  uint32_t        old_flags;
  CORE_DECLARE_IRQ_STATE;

  p_flags = (osEventFlags_t *)ef_id;
  if (p_flags == (osEventFlags_t *)0) {
    return osFlagsErrorParameter;
  }

  CORE_ENTER_ATOMIC();
  old_flags = p_flags->flags;                                   // Get previous value of flags
  p_flags->flags &= ~flags;
  CORE_EXIT_ATOMIC();

  OSFlagPost(&p_flags->flag_grp, flags, OS_OPT_POST_FLAG_CLR, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    old_flags = osFlagsErrorUnknown;
  }

  return old_flags;
#else
  (void)ef_id;
  (void)flags;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osFlagsErrorUnknown);
  return osFlagsErrorUnknown;
#endif
}

/*
 ****************************************************************************************************
 *                                         osEventFlagsGet()
 *
 * Description: The function 'osEventFlagsGet()' gets the current state of the event flags in an event
 *              flags object specified by parameter 'ef_id'.
 *
 * Arguments  : ef_id    Is the events flags ID
 *
 *              flags    The flags to clear
 *
 * Returns    : osFlagsErrorUnknown     unspecified error.
 *              osFlagsErrorParameter   parameter ef_id does not identify a valid event flags object or flags has highest bit set.
 *              osFlagsErrorResource    the event flags object is in an invalid state.
 *
 * Note(s)    : None
 ****************************************************************************************************
 */
uint32_t  osEventFlagsGet(osEventFlagsId_t  ef_id)
{
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  osEventFlags_t *p_flags;
  uint32_t        flags;

  p_flags = (osEventFlags_t *)ef_id;
  if (p_flags == (osEventFlags_t *)0) {
    return 0u;
  }

  flags = (uint32_t)p_flags->flags;                             // Get current value of flags
  return flags;
#else
  (void)ef_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osFlagsErrorUnknown);
  return osFlagsErrorUnknown;
#endif
}

/*
 ****************************************************************************************************
 *                                         osEventFlagsWait()
 *
 * Description: The function 'osEventFlagsWait()' suspends the execution of the currently RUNNING thread
 *              until any or all event flags specified by the parameter 'flags' in the event object
 *              specified by parameter 'ef_id' are set.  When these event flags are already set, the
 *              function returns instantly.  Otherwise, the thread is put into the state BLOCKED.
 *
 * Arguments  : ef_id    Is the events flags ID
 *
 *              flags    The desired flag(s) to wait for
 *
 *              options
 *                       osFlagsWaitAny                   Wait for any flag (default).
 *                       osFlagsWaitAny | osFlagsNoClear  Wait for any flag  specified but do not consume
 *                       osFlagsWaitAll                   Wait for all flags.
 *                       osFlagsWaitAll | osFlagsNoClear  Wait for all flags specified but do not consume
 *
 *              timeout
 *                       The parameter timeout specifies how long the system waits for event flags.
 *                       While the system waits, the thread that is calling this function is put into the
 *                       BLOCKED state.  The parameter timeout can have the following values:
 *
 *                       when timeout is 0,                       the function returns instantly (i.e. try semantics).
 *                       when timeout is set to 'osWaitForever'   the function will wait for an infinite time until
 *                                                                the event flags become available (i.e. wait semantics).
 *                       all other values                         specify a time in kernel ticks for a timeout
 *                                                                (i.e. timed-wait semantics).
 *
 * Returns    : The
 *              osFlagsErrorUnknown          unspecified error.
 *              osFlagsErrorTimeout          awaited flags have not been set in the given time.
 *              osFlagsErrorResource         awaited flags have not been set when no timeout was specified.
 *              osFlagsErrorParameter        parameter 'ef_id' does not identify a valid event flags object or flags has highest bit set.
 *
 * Note(s)    : 1) May be called from Interrupt Service Routines if the parameter timeout is set to 0.
 *
 *              2) MicriumOS does NOT return the flags prior to clearing but instead, the flags that
 *                 made the task ready-to-run.
 ****************************************************************************************************
 */
uint32_t  osEventFlagsWait(osEventFlagsId_t  ef_id,
                           uint32_t          flags,
                           uint32_t          options,
                           uint32_t          timeout)
{
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  osEventFlags_t *p_flags;
  OS_TICK         os_timeout;
  OS_OPT          opt;
  RTOS_ERR        err;
  CPU_TS          ts;
  uint32_t        rtn_flags;
  CORE_DECLARE_IRQ_STATE;

  p_flags = (osEventFlags_t *)ef_id;
  if (p_flags == (osEventFlags_t *)0) {
    return osFlagsErrorParameter;
  }

  if ((flags & 0x80000000u) == 0x80000000u) {                    // MSB cannot be set
    return osFlagsErrorParameter;
  }
  opt = OS_OPT_PEND_BLOCKING;
  if (timeout == osWaitForever) {                                // Convert CMSIS-RTOS timeout to MicriumOS
    if (CORE_InIrqContext() == true) {
      return osFlagsErrorParameter;
    }
    os_timeout = 0u;                                             // 0 for MicriumOS means forever
  } else if (timeout == 0u) {
    opt = OS_OPT_PEND_NON_BLOCKING;                              // No timeout, non-blocking
    os_timeout = 0u;                                             // timeout for MicriumOS in non-blocking mode is ignored
  } else {
    if (CORE_InIrqContext() == true) {
      return osFlagsErrorParameter;
    }
    os_timeout = (OS_TICK)timeout;
  }

  if (options & osFlagsWaitAll) {
    opt |= OS_OPT_PEND_FLAG_SET_ALL;
  } else {
    opt |= OS_OPT_PEND_FLAG_SET_ANY;
  }

  if (!(options & osFlagsNoClear)) {
    opt |= OS_OPT_PEND_FLAG_CONSUME;
  }

  OSFlagPend(&p_flags->flag_grp, (OS_FLAGS)flags, os_timeout, opt, &ts, &err);

  CORE_ENTER_ATOMIC();
  rtn_flags = p_flags->flags;
  if (!(options & osFlagsNoClear)) {
    p_flags->flags &= ~flags;
  }
  CORE_EXIT_ATOMIC();

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      return rtn_flags;

    case RTOS_ERR_TIMEOUT:
      return osFlagsErrorTimeout;

    case RTOS_ERR_WOULD_BLOCK:
      if (rtn_flags != (uint32_t)0u) {
        return rtn_flags;
      } else {
        return osFlagsErrorResource;
      }

    case RTOS_ERR_OS_OBJ_DEL:
    case RTOS_ERR_OS_SCHED_LOCKED:
    case RTOS_ERR_ABORT:
      return osFlagsErrorResource;

    default:
      return osFlagsErrorUnknown;
  }
#else
  (void)ef_id;
  (void)flags;
  (void)options;
  (void)timeout;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osFlagsErrorUnknown);
  return osFlagsErrorUnknown;
#endif
}

/*
 ****************************************************************************************************
 *                                        osEventFlagsDelete()
 *
 * Description: The function 'osEventFlagsDelete()' deletes the event flags object specified by parameter
 *              'ef_id' and releases the internal memory obtained for the event flags handling.
 *
 *              After this call, the 'ef_id' is no longer valid and cannot be used.  This can cause starvation
 *              of threads that are waiting for flags of this event object.  The 'ef_id' may be created again
 *              using the function 'osEventFlagsNew()'.
 *
 * Arguments  : ef_id                   Is the ID of the events flags to delete
 *
 * Returns    : osOK                    the specified event flags object was deleted
 *              osErrorISR              the function was called from an ISR
 *              osErrorParameter        parameter 'ef_id' does not identify a valid event flags object
 *              osErrorResource         the event flags object is in an invalid state.
 *
 * Note(s)    : 1) This function cannot be called from an ISR.
 ****************************************************************************************************
 */
osStatus_t  osEventFlagsDelete(osEventFlagsId_t  ef_id)
{
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  osEventFlags_t *p_flags;
  RTOS_ERR        err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_flags = (osEventFlags_t *)ef_id;
  if (p_flags == (osEventFlags_t *)0) {
    return osErrorParameter;
  }

  OSFlagDel(&p_flags->flag_grp, OS_OPT_DEL_ALWAYS, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      if (p_flags->dyn_alloc == DEF_TRUE) {
        free(p_flags);
      }
      return osOK;

    case RTOS_ERR_OS_ILLEGAL_RUN_TIME:
    case RTOS_ERR_OS_TASK_WAITING:
    default:
      return osErrorResource;
  }
#else
  (void)ef_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osFlagsErrorUnknown);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                        osEventFlagsGetName()
 *
 * Description: The function 'osEventFlagsGetName()' gets the name of the event flags object specified
 *              by parameter 'ef_id'.
 *
 * Arguments  : ef_id    Is the events flags ID
 *
 * Returns    : A pointer to the name   upon success
 *              NULL                    upon failure
 *
 * Note(s)    : 1) This function cannot be called from an ISR
 ****************************************************************************************************
 */
const  char  *osEventFlagsGetName(osEventFlagsId_t  ef_id)
{
#if (OS_CFG_FLAG_EN == DEF_ENABLED) && (OS_CFG_DBG_EN == DEF_ENABLED)
  osEventFlags_t *p_flags;
  CPU_CHAR       *p_name;

  p_flags = (osEventFlags_t *)ef_id;
  if (p_flags == (osEventFlags_t *)0) {
    return 0u;
  }

  p_name = p_flags->flag_grp.NamePtr;   // Get name of the event flags
  return p_name;
#else
  (void)ef_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, 0u);
  return 0u;
#endif
}

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                         M U T U A L   E X C L U S I O N   S E M A P H O R E S
 ****************************************************************************************************
 ****************************************************************************************************
 */

/*
 ****************************************************************************************************
 *                                            osMutexNew()
 *
 * Description: The function 'osMutexNew()' creates and initializes a new mutex object and returns the
 *              pointer to the mutex object identifier or NULL in case of an error.  It can be safely
 *              called before the RTOS is started (call to 'osKernelStart()'), but not before it is
 *              initialized (call to 'osKernelInitialize()').
 *
 *              The parameter 'attr' sets the mutex object attributes (refer to 'osMutexAttr_t').
 *              Default attributes will be used if set to NULL.
 *
 * Arguments  : attr     sets the mutex attributes or default values if NULL
 *
 *                       .name        is an ASCII string used to name the mutex
 *
 *                       .attr_bits   can have the following attributes:
 *
 *                                    osMutexRecursive
 *                                    osMutexPrioInherit
 *                                    osMutexRobust
 *
 *                       .cb_mem      Pointer to storage area for the mutex control block
 *
 *                       .cb_size     Size of the memory storage for the mutex control block
 *
 * Returns    : The mutex ID
 *
 * Note(s)    : 1) MicriumOS does NOT support the osMutexRobust attribute.  The user must explicitely
 *                 release ownership of the mutex before deleting a task.
 *
 *              2) MicriumOS mutexes are always recursive and support priority inheritance.  Those
 *                 attributes cannot be disabled.
 *
 *              3) The memory control block is assumed to be of type 'osMutex_t' which for the CMSIS-RTOS
 *                 adaptation layer is defined as an OS_MUTEX in MicriumOS.
 ****************************************************************************************************
 */
osMutexId_t  osMutexNew(const  osMutexAttr_t  *attr)
{
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  osMutex_t    *p_mutex;
  CPU_CHAR     *p_name;
  RTOS_ERR      err;

  if (CORE_InIrqContext() == true) {
    return (osMutexId_t)0;
  }

  p_name = (CPU_CHAR *)"Mutex";
  if (attr == 0) {
    p_mutex = (osMutex_t *)malloc(sizeof(osMutex_t));
    if (p_mutex == (osMutex_t *)0) {
      return (osMutexId_t)0;
    }
    p_mutex->dyn_alloc = DEF_TRUE;
  } else {
    if (attr->cb_mem == (void *)0) {
      p_mutex = (osMutex_t*)malloc(sizeof(osMutex_t));
      if (p_mutex == (osMutex_t*)0) {
        return (osMutexId_t)0;
      }
      p_mutex->dyn_alloc = DEF_TRUE;
    } else {
      if (attr->cb_size < sizeof(osMutex_t) || (uint32_t)attr->cb_mem % sizeof(CPU_ALIGN)) {
        return (osMutexId_t)0;
      }
      p_mutex = (osMutex_t *)attr->cb_mem;
      p_mutex->dyn_alloc = DEF_FALSE;
    }
    if (attr->name != (const char *)0) {
      p_name = (CPU_CHAR *)attr->name;
    }
  }

  OSMutexCreate(&p_mutex->mutex, p_name, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    if (p_mutex->dyn_alloc == DEF_TRUE) {
      free(p_mutex);
    }
    return (osMutexId_t)0;
  }

  if (attr != NULL && attr->attr_bits & osMutexRecursive) {
    p_mutex->recursive = DEF_TRUE;
  } else {
    p_mutex->recursive = DEF_FALSE;
  }

  return (osMutexId_t)p_mutex;
#else
  (void)attr;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, (osMutexId_t)0);
  return (osMutexId_t)0;
#endif
}

/*
 ****************************************************************************************************
 *                                           osMutexGetName()
 *
 * Description: The function 'osMutexGetName()' returns the pointer to the name string of the mutex
 *              identified by parameter 'mutex_id' or NULL in case of an error.
 *
 * Arguments  : mutex_id     is the mutex ID returned by 'osMutexNew()'
 *
 * Returns    : A pointer to the ASCII string containing the name of the mutex.
 *              NULL             upon error
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
const  char  *osMutexGetName(osMutexId_t  mutex_id)
{
#if (OS_CFG_MUTEX_EN == DEF_ENABLED) && (OS_CFG_DBG_EN == DEF_ENABLED)
  osMutex_t    *p_mutex;

  if (CORE_InIrqContext() == true) {
    return NULL;
  }

  p_mutex = (osMutex_t *)mutex_id;
  if (p_mutex == (osMutex_t *)0) {
    return NULL;
  }

  return p_mutex->mutex.NamePtr;
#else
  (void)mutex_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, NULL);
  return NULL;
#endif
}

/*
 ****************************************************************************************************
 *                                            osMutexAcquire()
 *
 * Description: The blocking function 'osMutexAcquire()' waits until a mutex object specified by parameter
 *              mutex_id becomes available.  If no other thread has obtained the mutex, the function
 *              instantly returns and blocks the mutex object.
 *
 *              The parameter timeout specifies how long the system waits to acquire the mutex.  While
 *              the system waits, the thread that is calling this function is put into the BLOCKED state.
 *              The parameter timeout can have the following values:
 *
 * Arguments  : mutex_id         is the mutex ID returned by 'osMutexNew()'
 *
 *              timeout          when timeout is 0, the function returns instantly (i.e. try semantics).
 *                               when timeout is set to osWaitForever the function will wait for an infinite
 *                                   time until the mutex becomes available (i.e. wait semantics).
 *                               all other values specify a time in kernel ticks for a timeout
 *                                   (i.e. timed-wait semantics).Timeout Value or 0 in case of no time-out.
 *
 * Returns    : osOK             the token has been obtained and the token count decremented.
 *              osErrorTimeout   the token could not be obtained in the given time.
 *              osErrorResource  the token could not be obtained when no timeout was specified.
 *              osErrorParameter the parameter semaphore_id is NULL or invalid.
 *              osErrorISR       cannot be called from an ISR
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osMutexAcquire(osMutexId_t  mutex_id,
                           uint32_t     timeout)
{
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  OS_OPT      opt;
  osMutex_t  *p_mutex;
  RTOS_ERR    err;
  CPU_TS      ts;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_mutex = (osMutex_t *)mutex_id;
  if (p_mutex == (osMutex_t *)0) {
    return osErrorParameter;
  }
  if (timeout == 0u) {
    opt = OS_OPT_PEND_NON_BLOCKING;
  } else {
    opt = OS_OPT_PEND_BLOCKING;
  }

  if (timeout == osWaitForever) {
    timeout = 0u;
  }

  OSMutexPend(&p_mutex->mutex, (OS_TICK)timeout, opt, &ts, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_IS_OWNER:
      if (p_mutex->recursive == DEF_FALSE) {
        return osErrorResource;
      }
      return osOK;

    case RTOS_ERR_NONE:
      return osOK;

    case RTOS_ERR_TIMEOUT:
      return osErrorTimeout;

    case RTOS_ERR_OS_OBJ_DEL:
    case RTOS_ERR_WOULD_OVF:
    case RTOS_ERR_OS_SCHED_LOCKED:
    case RTOS_ERR_ABORT:
    case RTOS_ERR_WOULD_BLOCK:
    default:
      return osErrorResource;
  }
#else
  (void)mutex_id;
  (void)timeout;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                            osMutexRelease()
 *
 * Description: The function 'osMutexRelease()' releases a mutex specified by parameter 'mutex_id'.
 *              Other threads that currently wait for this mutex will be put into the READY state.
 *
 * Arguments  : mutex_id         is the mutex ID returned by 'osMutexNew()'
 *
 * Returns    : osOK             the mutex has been released and the count incremented.
 *              osErrorResource  the mutex could not be released
 *              osErrorParameter the parameter mutex_id is NULL or invalid.
 *              osErrorISR       if called from an ISR
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 *
 *              2) MicriumOS always allows mutex nesting and thus that feature cannot be disabled.
 ****************************************************************************************************
 */
osStatus_t  osMutexRelease(osMutexId_t  mutex_id)
{
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  osMutex_t    *p_mutex;
  RTOS_ERR      err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_mutex = (osMutex_t *)mutex_id;

  if (p_mutex == (osMutex_t *)0) {
    return osErrorParameter;
  }

  OSMutexPost(&p_mutex->mutex, OS_OPT_POST_NONE, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
    case RTOS_ERR_IS_OWNER:
      return osOK;

    case RTOS_ERR_OWNERSHIP:
    default:
      return osErrorResource;
  }
#else
  (void)mutex_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                        osMutexGetOwner()
 *
 * Description: The function 'osMutexGetOwner()' returns the thread ID of the thread that acquired a
 *              mutex specified by parameter 'mutex_id'.  In case of an error or if the mutex is not
 *              blocked by any thread, it returns NULL.
 *
 * Arguments  : mutex_id     is the mutex ID returned by 'osMutexNew()'
 *
 * Returns    : the theard ID of the mutex owner
 *
 * Note(s)    : None
 ****************************************************************************************************
 */
osThreadId_t  osMutexGetOwner(osMutexId_t  mutex_id)
{
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  osMutex_t  *p_mutex;
  osThread_t *p_thread;

  p_mutex = (osMutex_t *)mutex_id;

  if (p_mutex == (osMutex_t *)0) {
    return 0u;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  p_thread = (osThread_t *)p_mutex->mutex.OwnerTCBPtr;
  CORE_EXIT_CRITICAL();
  return (osThreadId_t)p_thread;
#else
  (void)mutex_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, 0u);
  return 0u;
#endif
}

/*
 ****************************************************************************************************
 *                                            osMutexDelete()
 *
 * Description: The function 'osMutexDelete()' deletes a mutex object specified by parameter 'mutex_id'.
 *              It releases internal memory obtained for mutex handling.  After this call, the 'mutex_id'
 *              is no longer valid and cannot be used.
 *
 *              The mutex may be created again using the function 'osMutexNew()'.
 *
 * Arguments  : mutex_id         is the mutex ID returned by 'osMutexNew()'
 *
 * Returns    : osOK             the mutex object has been deleted.
 *              osErrorParameter the parameter mutex_id is NULL or invalid.
 *              osErrorResource  the mutex is in an invalid state.
 *              osErrorISR       osMutexDelete() cannot be called from interrupt service routines.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osMutexDelete(osMutexId_t  mutex_id)
{
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  osMutex_t  *p_mutex;
  RTOS_ERR    err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_mutex = (osMutex_t *)mutex_id;
  if (p_mutex == (osMutex_t *)0) {
    return osErrorParameter;
  }

  OSMutexDel(&p_mutex->mutex, OS_OPT_DEL_ALWAYS, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      if (p_mutex->dyn_alloc == DEF_TRUE) {
        free(p_mutex);
      }
      return osOK;

    case RTOS_ERR_OS_ILLEGAL_RUN_TIME:
    case RTOS_ERR_OS_TASK_WAITING:
    default:
      return osErrorResource;
  }
#else
  (void)mutex_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                               S E M A P H O R E   M A N A G E M E N T
 ****************************************************************************************************
 ****************************************************************************************************
 */

/*
 ****************************************************************************************************
 *                                        osSemaphoreNew()
 *
 * Description: The function 'osSemaphoreNew()' creates and initializes a semaphore object that is used
 *              to manage access to shared resources and returns the pointer to the semaphore object
 *              identifier or NULL in case of an error.
 *
 *              It can be safely called before the RTOS is started (call to 'osKernelStart()'), but not
 *              before it is initialized (call to 'osKernelInitialize()').
 *
 *              The parameter 'max_count' specifies the maximum number of available tokens.  A 'max_count'
 *              value of 1 creates a binary semaphore.
 *
 *              The parameter 'initial_count' sets the initial number of available tokens.
 *
 *              The parameter 'attr' specifies additional semaphore attributes.  Default attributes will
 *              be used if set to NULL.
 *
 * Arguments  : max_count       maximum number of available tokens.
 *
 *              initial_count   initial number of available tokens.
 *
 *              attr            semaphore attributes; NULL: default values.
 *
 * Returns    : semaphore ID for reference by other functions or NULL in case of error.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 *
 *              2) MicriumOS does not support a maximum count so the first argument is ignored.
 ****************************************************************************************************
 */
osSemaphoreId_t  osSemaphoreNew(uint32_t                   max_count,
                                uint32_t                   initial_count,
                                const  osSemaphoreAttr_t   *attr)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osSemaphore_t *p_sem;
  CPU_CHAR      *p_name;
  RTOS_ERR       err;

  if (CORE_InIrqContext() == true) {
    return (osSemaphoreId_t)0;
  }

  if (max_count == 0u) {
    return (osSemaphoreId_t)0;   // Cannot specify a 0 value for max count
  }

  p_name = (CPU_CHAR *)"Semaphore";
  if (attr == 0) {
    p_sem = (osSemaphore_t *)malloc(sizeof(osSemaphore_t));
    if (p_sem == (osSemaphore_t *)0) {
      return (osSemaphoreId_t)0;
    }
    p_sem->dyn_alloc = DEF_TRUE;
  } else {
    if (attr->cb_mem == (void *)0) {
      p_sem = (osSemaphore_t *)malloc(sizeof(osSemaphore_t));
      if (p_sem == (osSemaphore_t *)0) {
        return (osSemaphoreId_t)0;
      }
      p_sem->dyn_alloc = DEF_TRUE;
    } else {
      if (attr->cb_size < sizeof(osSemaphore_t) || (uint32_t)attr->cb_mem % sizeof(CPU_ALIGN)) {
        return (osSemaphoreId_t)0;
      }
      p_sem = (osSemaphore_t *)attr->cb_mem;
      p_sem->dyn_alloc = DEF_FALSE;
    }
    if (attr->name != (const char *)0) {
      p_name = (CPU_CHAR *)attr->name;
    }
  }

  OSSemCreate(&p_sem->sem, p_name, initial_count, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    if (p_sem->dyn_alloc == DEF_TRUE) {
      free(p_sem);
    }
    return (osSemaphoreId_t)0;
  }

  return (osSemaphoreId_t)p_sem;
#else
  (void)max_count;
  (void)initial_count;
  (void)attr;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, (osSemaphoreId_t)0);
  return (osSemaphoreId_t)0;
#endif
}

/*
 ****************************************************************************************************
 *                                        osSemaphoreGetName()
 *
 * Description: The function 'osSemaphoreGetName()' returns the pointer to the name string of the semaphore
 *              identified by parameter 'semaphore_id' or NULL in case of an error.
 *
 * Arguments  : semaphore_id     is the semaphore ID returned by 'osSemaphoreNew()'
 *
 * Returns    : A pointer to the ASCII string containing the name of the semaphore.
 *              NULL             upon error
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
const  char  *osSemaphoreGetName(osSemaphoreId_t  semaphore_id)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED) && (OS_CFG_DBG_EN == DEF_ENABLED)
  osSemaphore_t *p_sem;

  if (CORE_InIrqContext() == true) {
    return NULL;
  }
  p_sem = (osSemaphore_t *)semaphore_id;
  if (p_sem == (osSemaphore_t *)0) {
    return NULL;
  }

  return p_sem->sem.NamePtr;
#else
  (void)semaphore_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, NULL);
  return NULL;
#endif
}

/*
 ****************************************************************************************************
 *                                        osSemaphoreAcquire()
 *
 * Description: The blocking function 'osSemaphoreAcquire() waits until a token of the semaphore object
 *              specified by parameter 'semaphore_id' becomes available.  If a token is available, the
 *              function instantly returns and decrements the token count.
 *
 *              The parameter timeout specifies how long the system waits to acquire the token. While the
 *              system waits, the thread that is calling this function is put into the BLOCKED state.
 *              The parameter timeout can have the following values:
 *
 *              when timeout is 0, the function returns instantly (i.e. try semantics).
 *
 *              when timeout is set to 'osWaitForever' the function will wait for an infinite time until
 *              the semaphore becomes available (i.e. wait semantics).
 *
 *              all other values specify a time in kernel ticks for a timeout (i.e. timed-wait semantics).
 *
 * Arguments  : semaphore_id     is the semaphore ID returned by 'osSemaphoreNew()'
 *
 *              timeout          Timeout Value or 0 in case of no time-out.
 *
 * Returns    : osOK             the token has been obtained and the token count decremented.
 *              osErrorTimeout   the token could not be obtained in the given time.
 *              osErrorResource  the token could not be obtained when no timeout was specified.
 *              osErrorParameter the parameter semaphore_id is NULL or invalid.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR if you specify an NON-Zero timeout
 ****************************************************************************************************
 */
osStatus_t  osSemaphoreAcquire(osSemaphoreId_t  semaphore_id,
                               uint32_t         timeout)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osSemaphore_t *p_sem;
  RTOS_ERR       err;
  CPU_TS         ts;

  p_sem = (osSemaphore_t *)semaphore_id;
  if (p_sem == (osSemaphore_t *)0) {
    return osErrorParameter;
  }
  if (timeout == 0u) {
    OSSemPend(&p_sem->sem, (OS_TICK)0u, OS_OPT_PEND_NON_BLOCKING, &ts, &err);
  } else {
    if (CORE_InIrqContext() == true) {
      return osErrorParameter;
    } else {
      if (timeout == osWaitForever) {
        timeout = 0u;
      }
      OSSemPend(&p_sem->sem, (OS_TICK)timeout, OS_OPT_PEND_BLOCKING, &ts, &err);
    }
  }

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      return osOK;

    case RTOS_ERR_TIMEOUT:
      return osErrorTimeout;

    case RTOS_ERR_ABORT:
    case RTOS_ERR_OS_SCHED_LOCKED:
    case RTOS_ERR_WOULD_BLOCK:
    case RTOS_ERR_OS_OBJ_DEL:
    default:
      return osErrorResource;
  }
#else
  (void)semaphore_id;
  (void)timeout;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                        osSemaphoreRelease()
 *
 * Description: The function 'osSemaphoreRelease()' releases a token of the semaphore object specified
 *              by parameter 'semaphore_id'.  Tokens can only be released up to the maximum count
 *              specified at creation time, see 'osSemaphoreNew()'.  Other threads that currently wait
 *              for a token of this semaphore object will be put into the READY state.
 *
 * Arguments  : semaphore_id     is the semaphore ID returned by 'osSemaphoreNew()'
 *
 * Returns    : osOK             the token has been released and the count incremented.
 *              osErrorResource  the token could not be released (maximum token count has been reached).
 *              osErrorParameter the parameter semaphore_id is NULL or invalid.
 *
 * Note(s)    : None
 ****************************************************************************************************
 */
osStatus_t  osSemaphoreRelease(osSemaphoreId_t  semaphore_id)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osSemaphore_t *p_sem;
  RTOS_ERR       err;

  p_sem = (osSemaphore_t *)semaphore_id;
  if (p_sem == (osSemaphore_t *)0) {
    return osErrorParameter;
  }
  (void)OSSemPost(&p_sem->sem, OS_OPT_POST_1, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      return osOK;

    case RTOS_ERR_WOULD_OVF:
    default:
      return osErrorResource;
  }
#else
  (void)semaphore_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                        osSemaphoreGetCount()
 *
 * Description: The function 'osSemaphoreGetCount()' returns the number of available tokens of the
 *              semaphore object specified by parameter 'semaphore_id'.
 *
 *              In case of an error it returns 0.
 *
 * Arguments  : semaphore_id     is the semaphore ID returned by 'osSemaphoreNew()'
 *
 * Returns    : the number of tokens available
 *
 * Note(s)    : None
 ****************************************************************************************************
 */
uint32_t  osSemaphoreGetCount(osSemaphoreId_t  semaphore_id)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osSemaphore_t *p_sem;
  uint32_t       count;

  p_sem = (osSemaphore_t*)semaphore_id;

  if (p_sem == (osSemaphore_t *)0) {
    return 0u;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  count = p_sem->sem.Ctr;
  CORE_EXIT_CRITICAL();
  return count;
#else
  (void)semaphore_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, 0u);
  return 0u;
#endif
}

/*
 ****************************************************************************************************
 *                                        osSemaphoreDelete()
 *
 * Description: The function 'osSemaphoreDelete()' deletes a semaphore object specified by parameter
 *              'semaphore_id'.  It releases internal memory obtained for semaphore handling.  After
 *              this call, the 'semaphore_id' is no longer valid and cannot be used.
 *
 *              The semaphore may be created again using the function 'osSemaphoreNew()'.
 *
 * Arguments  : semaphore_id     is the semaphore ID returned by 'osSemaphoreNew()'
 *
 * Returns    : osOK             the semaphore object has been deleted.
 *              osErrorParameter the parameter semaphore_id is NULL or invalid.
 *              osErrorResource  the semaphore is in an invalid state.
 *              osErrorISR       osSemaphoreDelete() cannot be called from interrupt service routines.
 *
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osSemaphoreDelete(osSemaphoreId_t  semaphore_id)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osSemaphore_t *p_sem;
  RTOS_ERR       err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_sem = (osSemaphore_t *)semaphore_id;

  if (p_sem == (osSemaphore_t *)0) {
    return osErrorParameter;
  }

  OSSemDel(&p_sem->sem, OS_OPT_DEL_ALWAYS, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      if (p_sem->dyn_alloc == DEF_TRUE) {
        free(p_sem);
      }
      return osOK;

    case RTOS_ERR_OS_ILLEGAL_RUN_TIME:
    case RTOS_ERR_OS_TASK_WAITING:
    default:
      return osErrorResource;
  }
#else
  (void)semaphore_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                                 T A S K   M A N A G E M E N T
 ****************************************************************************************************
 ****************************************************************************************************
 */

/*
 ****************************************************************************************************
 *                                          osThreadNew()
 *
 * Description: The function 'osThreadNew()' starts a thread function by adding it to the list of active
 *              threads and sets it to state READY. Arguments for the thread function are passed using
 *              the parameter pointer *argument. When the priority of the created thread function is
 *              higher than the current RUNNING thread, the created thread function starts instantly
 *              and becomes the new RUNNING thread.
 *
 *              Thread attributes are defined with the parameter pointer 'attr'.  Attributes include
 *              settings for thread priority, stack size, or memory allocation.
 *
 * Arguments  : func             is a pointer to the task code
 *
 *              argument         is a pointer to an argument that is passed to the task when the task
 *                               starts execution.
 *
 *              attr             attribute structure passed to the task creation code.
 *                               specifying NULL assumes defaults.
 *
 *                               Thread attributes are:
 *
 *                               .name         name of the thread
 *                               .attr_bits    values:
 *                                             osThreadDetached     (default)    0x00000000
 *                                             osThreadJoinable                  0x00000001
 *                               .cb_mem       pointer to TCB
 *                                                (allocated dynamically if NULL or not specified)
 *                               .cb_size      size of the TCB (in bytes)
 *                               .stack_mem    pointer to the base address of the stack
 *                                                (allocated dynamically if NULL or not specified)
 *                               .stack_size   size of stack (in bytes)
 *                                                (defaults to CMSIS_DEFAULT_STACK_SIZE bytes
 *                                                 if 0 specified or not specified)
 *                               .priority     Thread priority (0 = lowest, 55 = highest)
 *                               .tz_module    TrustZone Module Identified
 *                               .reserved     Must be 0
 *
 * Returns    : The thread ID                           upon success
 *              NULL                                    upon error
 *
 * Note(s)    : 1) '.tz_module'  is ignored in this implementation
 *
 *              2) if '.cb_mem' is specified it must point to MicriumOS OS_TCB.
 *                 also, '.cb_size' MUST be declared as 'sizeof(OS_TCB)'
 *
 *              3) 'attr_bits' are not supported and tasks are always assumed to be 'detached'
 ****************************************************************************************************
 */
osThreadId_t  osThreadNew(osThreadFunc_t          func,
                          void                    *argument,
                          const  osThreadAttr_t   *attr)
{
  osThread_t   *p_thread;
  CPU_STK      *p_stk_base;
  uint32_t      stk_size_in_bytes;
  CPU_CHAR     *p_name;
  OS_PRIO       prio;
  RTOS_ERR      err;

  if (CORE_InIrqContext() == true) {
    return (osThreadId_t)0;          // Can't create a thread from an ISR
  }
  if (func == (osThreadFunc_t)0) {                                             // Make sure a thread is specified
    return (osThreadId_t)0;
  }
  p_name = (CPU_CHAR *)"TaskName?";
  if (attr == 0) {
    p_thread = (osThread_t *)malloc(sizeof(osThread_t));
    if (p_thread == (osThread_t *)0) {
      return (osThreadId_t)0;
    } else {
      p_thread->obj_dyn_alloc = DEF_TRUE;
      p_stk_base = (CPU_STK *)malloc(CMSIS_DEFAULT_STACK_SIZE);
      if (p_stk_base == (CPU_STK *)0) {
        return (osThreadId_t)0;
      }
      p_thread->stack_dyn_alloc = DEF_TRUE;
      stk_size_in_bytes = CMSIS_DEFAULT_STACK_SIZE;
      prio = (OS_PRIO)(osPriorityRealtime7 - osPriorityNormal);
    }
  } else {
    if (attr->cb_mem == (void *)0) {
      p_thread = (osThread_t *)malloc(sizeof(osThread_t));
      if (p_thread == (osThread_t *)0) {
        return (osThreadId_t)0;
      }
      p_thread->obj_dyn_alloc = DEF_TRUE;
    } else {
      if (attr->cb_size < sizeof(osThread_t) || (uint32_t)attr->cb_mem % sizeof(CPU_ALIGN)) {
        return (osThreadId_t)0;
      }
      p_thread = attr->cb_mem;
      p_thread->obj_dyn_alloc = DEF_FALSE;
    }
    if (attr->stack_size == 0u) {
      p_stk_base = (CPU_STK *)malloc(CMSIS_DEFAULT_STACK_SIZE);
      if (p_stk_base == (CPU_STK *)0) {
        if (p_thread->obj_dyn_alloc == DEF_TRUE) {
          free(p_thread);
        }
        return (osThreadId_t)0;
      }
      stk_size_in_bytes = CMSIS_DEFAULT_STACK_SIZE;
      p_thread->stack_dyn_alloc = DEF_TRUE;
    } else if ((attr->stack_size != 0u)
               && (attr->stack_mem == NULL)) {
      stk_size_in_bytes = attr->stack_size;
      p_stk_base = (CPU_STK *)malloc(stk_size_in_bytes);
      if (p_stk_base == (CPU_STK *)0) {
        if (p_thread->obj_dyn_alloc == DEF_TRUE) {
          free(p_thread);
        }
        return (osThreadId_t)0;
      }
      p_thread->stack_dyn_alloc = DEF_TRUE;
    } else {
      if (attr->stack_mem == NULL || ((uint32_t)attr->stack_mem % CPU_CFG_STK_ALIGN_BYTES)) {
        if (p_thread->obj_dyn_alloc == DEF_TRUE) {
          free(p_thread);
        }
        return (osThreadId_t)0;
      }
      p_stk_base = (CPU_STK *)attr->stack_mem;
      stk_size_in_bytes = attr->stack_size;
      p_thread->stack_dyn_alloc = DEF_FALSE;
    }
    if (attr->name != (const char *)0) {
      p_name = (CPU_CHAR *)attr->name;
    }
    if (attr->priority == osPriorityNone) {
      prio = (OS_PRIO)(osPriorityRealtime7 - osPriorityNormal);
    } else {
      if (attr->priority == osPriorityError) {
        prio = (OS_PRIO)(OS_CFG_PRIO_MAX);                          // Set to an invalid priority level for MicriumOS
      } else {
        if (attr->priority > osPriorityRealtime7) {
          if (p_thread->stack_dyn_alloc == DEF_TRUE) {
            free(p_stk_base);
          }
          if (p_thread->obj_dyn_alloc == DEF_TRUE) {
            free(p_thread);
          }
          return (osThreadId_t)0;
        }
        prio = (OS_PRIO)(osPriorityRealtime7 - attr->priority);
      }
    }
  }

#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  // Create thread flags
  OSFlagCreate(&p_thread->flag_grp, "ThreadFlags", 0, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    if (p_thread->stack_dyn_alloc == DEF_TRUE) {
      free(p_stk_base);
    }
    if (p_thread->obj_dyn_alloc == DEF_TRUE) {
      free(p_thread);
    }
    return (osThreadId_t)0;
  }
#endif

  // Round down the stack size to a multiple of the cpu stack size
  stk_size_in_bytes -= (stk_size_in_bytes % CPU_CFG_STK_ALIGN_BYTES);

  OSTaskCreate(&p_thread->tcb, p_name, func, argument, prio, p_stk_base, 0,
               stk_size_in_bytes / sizeof(CPU_STK), 0u, 0u, 0u,
               OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,
               &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    if (p_thread->stack_dyn_alloc == DEF_TRUE) {
      free(p_stk_base);
    }
    if (p_thread->obj_dyn_alloc == DEF_TRUE) {
      free(p_thread);
    }
    return (osThreadId_t)0;
  }

  return (osThreadId_t)p_thread;
}

/*
 ****************************************************************************************************
 *                                          osThreadGetName()
 *
 * Description: The function osThreadGetName returns the pointer to the name string of the thread
 *              identified by parameter 'thread_id' or NULL in case of an error.
 *
 * Arguments  : thread_id        is the thread ID returned by osThreadNew() when the thread is created
 *
 * Returns    : A pointer to the ASCII string containing the name of the thread.
 *              NULL             upon error
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
const  char  *osThreadGetName(osThreadId_t  thread_id)
{
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  osThread_t *p_thread;

  p_thread = (osThread_t *)thread_id;
  if (p_thread == (osThread_t *)0) {
    return NULL;
  } else {
    return p_thread->tcb.NamePtr;
  }
#else
  (void)thread_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, NULL);
  return NULL;
#endif
}

/*
 ****************************************************************************************************
 *                                          osThreadGetId()
 *
 * Description: The function 'osThreadGetId()' returns the thread object ID of the currently running
 *              thread or NULL in case of an error.
 *
 * Arguments  : None
 *
 * Returns    : The thread ID
 *              NULL             upon error
 ****************************************************************************************************
 */
osThreadId_t  osThreadGetId(void)
{
  if (OSRunning == OS_STATE_OS_RUNNING) {
    return (osThreadId_t)OSTCBCurPtr;
  }

  return (osThreadId_t)0;
}

/*
 ****************************************************************************************************
 *                                         osThreadGetState()
 *
 * Description: The function osThreadGetState() returns the state of the thread identified by parameter
 *              'thread_id'.  In case it fails or if it is called from an ISR, it will return
 *              osThreadError, otherwise it returns the thread state (refer to 'osThreadState_t' for
 *              the list of thread states).
 *
 * Arguments  : thread_id   is the ID of the desired thread
 *
 * Returns    : The thread state (see below)
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osThreadState_t  osThreadGetState(osThreadId_t  thread_id)
{
  osThread_t      *p_thread;
  osThreadState_t  state;

  if (CORE_InIrqContext() == true) {
    return osThreadError;
  }
  p_thread = (osThread_t *)thread_id;
  if (p_thread == (osThread_t *)0) {
    return osThreadError;
  }

  if (OSRunning == OS_STATE_OS_RUNNING) {
    switch (p_thread->tcb.TaskState) {
      case OS_TASK_STATE_RDY:
        state = osThreadReady;
        break;

      case OS_TASK_STATE_DEL:
        state = osThreadInactive;
        break;

      case OS_TASK_STATE_DLY:
      case OS_TASK_STATE_PEND:
      case OS_TASK_STATE_PEND_TIMEOUT:
      case OS_TASK_STATE_SUSPENDED:
      case OS_TASK_STATE_DLY_SUSPENDED:
      case OS_TASK_STATE_PEND_SUSPENDED:
      case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
        state = osThreadBlocked;
        break;

      default:
        state = osThreadError;
        break;
    }
  } else {
    state = osThreadError;
  }
  return state;
}

/*
 ****************************************************************************************************
 *                                        osThreadSetPriority()
 *
 * Description: The function 'osThreadSetPriority()' changes the priority of an active thread specified
 *              by the parameter 'thread_id' to the priority specified by the parameter 'priority'.
 *
 * Arguments  : thread_id        is the thread ID
 *
 *              priority         is the desired new priority for the specified thread
 *
 * Returns    : osOK               the priority of the specified thread has been changed
 *              osErrorParameter   an invalid parameter was specified
 *              osErrorResource    the thread is in an invalid state
 *              osErrorISR         if called from an ISR
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osThreadSetPriority(osThreadId_t  thread_id,
                                osPriority_t  priority)
{
  OS_PRIO     prio;
  osThread_t *p_thread;
  RTOS_ERR    err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }
  if (thread_id == (osThreadId_t)0) {
    return osErrorParameter;
  }
  if (priority == osPriorityError) {
    return osErrorParameter;
  }
  if (priority > osPriorityRealtime7) {
    return osErrorParameter;
  }
  prio = (OS_PRIO)(osPriorityRealtime7 - priority);
  p_thread = (osThread_t *)thread_id;

  OSTaskChangePrio(&p_thread->tcb, prio, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      return osOK;

    case RTOS_ERR_INVALID_ARG:
      return osErrorParameter;

    default:
      return osErrorResource;
  }
}

/*
 ****************************************************************************************************
 *                                        osThreadGetPriority()
 *
 * Description: The function 'osThreadGetPriority()' returns the priority of an active thread specified
 *              by the parameter 'thread_id'.
 *
 * Arguments  : None
 *
 * Returns    : The thread ID        upon success
 *              osPriorityError      upon error
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osPriority_t  osThreadGetPriority(osThreadId_t  thread_id)
{
  osPriority_t   priority;
  osThread_t    *p_thread;

  if (OSRunning != OS_STATE_OS_RUNNING) {
    return osPriorityError;
  }
  if (CORE_InIrqContext() == true) {
    return osPriorityError;
  }
  p_thread = (osThread_t *)thread_id;
  if (p_thread == (osThread_t *)0) {
    return osPriorityError;
  }
  priority = (osPriority_t)((OS_PRIO)osPriorityRealtime7 - p_thread->tcb.Prio);
  return priority;
}

/*
 ****************************************************************************************************
 *                                        osThreadYield()
 *
 * Description: The function 'osThreadYield()' passes control to the next thread with the same priority
 *              that is in the READY state. I f there is no other thread with the same priority in state
 *              READY, then the current thread continues execution and no thread switch occurs.
 *
 *              'osThreadYield()' does not set the thread to state BLOCKED.  Thus no thread with a lower
 *              priority will be scheduled even if threads in state READY are available.
 *
 * Arguments  : None
 *
 * Returns    : osOK        control has passed to the next thread
 *              osError     an unspecified error occurred
 *              osErrorISR  if called from an ISR
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osThreadYield(void)
{
#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
  RTOS_ERR  err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }
  OSSchedRoundRobinYield(&err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      return osOK;

    case RTOS_ERR_NOT_AVAIL:
    case RTOS_ERR_NONE_WAITING:
    case RTOS_ERR_OS_SCHED_LOCKED:
    default:
      return osError;
  }
#else
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                        osThreadSuspend()
 *
 * Description: The function 'osThreadSuspend()' suspends the execution of the thread identified by
 *              parameter 'thread_id'.  The thread is put into the BLOCKED state (osThreadBlocked).
 *              Suspending the running thread will cause a context switch to another thread in READY
 *              state immediately.  The suspended thread is not executed until explicitly resumed
 *              by the function 'osThreadResume()'.
 *
 *              Threads that are already BLOCKED are removed from any wait list and become ready when
 *              they are resumed.  Thus it is not recommended to suspend an already blocked thread.
 *
 * Arguments  : thread_id    is the ID of the thread
 *
 * Returns    : osOK                the thread has been suspended successfully.
 *              osErrorParameter    thread_id is NULL or invalid.
 *              osErrorResource     the thread is in an invalid state.
 *              osErrorISR          the function osThreadSuspend cannot be called from interrupt service routines.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osThreadSuspend(osThreadId_t  thread_id)
{
  RTOS_ERR     err;
  osThread_t  *p_thread;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_thread = (osThread_t *)thread_id;
  if (p_thread == (osThread_t *)0) {
    return osErrorParameter;
  }

  OSTaskSuspend(&p_thread->tcb, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      return osOK;

    case RTOS_ERR_OS:
    case RTOS_ERR_NOT_READY:
    case RTOS_ERR_WOULD_OVF:
    case RTOS_ERR_INVALID_STATE:
    case RTOS_ERR_OS_SCHED_LOCKED:
    default:
      return osErrorResource;
  }
}

/*
 ****************************************************************************************************
 *                                        osThreadResume()
 *
 * Description: The function 'osThreadResume()' puts the thread identified by parameter 'thread_id'
 *              (which has to be in BLOCKED state) back to the READY state.  If the resumed thread
 *              has a higher priority than the running thread a context switch occurs immediately.
 *
 *              The thread becomes ready regardless of the reason why the thread was blocked.  Thus
 *              it is not recommended to resume a thread not suspended by 'osThreadSuspend()'.
 *
 *              Functions that will put a thread into BLOCKED state are:
 *
 *                 'osEventFlagsWait()' and 'osThreadFlagsWait()',
 *                 'osDelay()'          and 'osDelayUntil()',
 *                 'osMutexAcquire()'   and 'osSemaphoreAcquire()',
 *                 'osMessageQueueGet()'
 *                 'osMemoryPoolAlloc(),
 *                 'osThreadJoin()'         Function NOT supported
 *                 'osThreadSuspend()'
 *
 * Arguments  : thread_id    is the ID of the thread
 *
 * Returns    : osOK                the thread has been resumed successfully.
 *              osErrorParameter    thread_id is NULL or invalid.
 *              osErrorResource     the thread is in an invalid state.
 *              osErrorISR          the function osThreadSuspend cannot be called from interrupt service routines.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 *
 *              2) MicriumOS cannot emulate this function exactly as described above.  In fact,
 *                 MicriumOS doesn't cancel pending on an object.  The suspension is simply removed if
 *                 the task was suspended by 'osThreadSuspend()'.
 ****************************************************************************************************
 */
osStatus_t  osThreadResume(osThreadId_t  thread_id)
{
  RTOS_ERR    err;
  osThread_t *p_thread;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_thread = (osThread_t *)thread_id;

  if (p_thread == (osThread_t *)0) {
    return osErrorParameter;
  }

  OSTaskResume(&p_thread->tcb, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      return osOK;

    case RTOS_ERR_INVALID_STATE:
    default:
      return osErrorResource;
  }
}
/*
 ****************************************************************************************************
 *                                        osThreadDetach()
 *
 * Description: The function 'osThreadDetach()' changes the attribute of a thread (specified by 'thread_id')
 *              to 'osThreadDetached'.  Detached threads are not joinable with 'osThreadJoin()'.
 *
 *              When a detached thread is terminated, all resources are returned to the system.
 *
 *              The behavior of 'osThreadDetach()' on an already detached thread is undefined.
 *
 * Arguments  : thread_id    is the ID of the thread
 *
 * Returns    : osOK                the thread has been detached successfully.
 *              osErrorParameter    thread_id is NULL or invalid.
 *              osErrorResource     the thread is in an invalid state.
 *              osErrorISR          the function osThreadSuspend cannot be called from interrupt service routines.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 *
 *              2) MicriumOS DOES NOT support this functionality
 ****************************************************************************************************
 */
osStatus_t  osThreadDetach(osThreadId_t  thread_id)
{
  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  if (thread_id == (osThreadId_t)0) {
    return osErrorParameter;
  }

  (void)thread_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, osError);
  return osError;
}

/*
 ****************************************************************************************************
 *                                        osThreadJoin()
 *
 * Description: The function 'osThreadJoin()' waits for the thread specified by 'thread_id' to terminate.
 *
 *              If that thread has already terminated, then 'osThreadJoin()' returns immediately.
 *
 *              The thread must be joinable.  By default threads are created with the attribute 'osThreadDetached'.
 *
 * Arguments  : thread_id    is the ID of the thread
 *
 * Returns    : osOK                the thread has been joined successfully.
 *              osErrorParameter    thread_id is NULL or invalid.
 *              osErrorResource     the thread is in an invalid state.
 *              osErrorISR          the function osThreadSuspend cannot be called from interrupt service routines.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 *
 *              2) MicriumOS DOES NOT support this functionality
 ****************************************************************************************************
 */
osStatus_t  osThreadJoin(osThreadId_t  thread_id)
{
  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  if (thread_id == (osThreadId_t)0) {
    return osErrorParameter;
  }

  (void)thread_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, osError);
  return osError;
}

/*
 ****************************************************************************************************
 *                                        osThreadExit()
 *
 * Description: The function 'osThreadExit()' terminates the calling thread.
 *
 *              This allows the thread to be synchronized with 'osThreadJoin()'.
 *
 * Arguments  : None
 *
 * Returns    : None
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
void  osThreadExit(void)
{
  RTOS_ERR  err;

  if (CORE_InIrqContext() == false) {
    OSTaskDel((OS_TCB *)0, &err);
  }
  for (;; ) {
    ;       // This function cannot return
  }
}

/*
 ****************************************************************************************************
 *                                        osThreadTerminate()
 *
 * Description: The function 'osThreadTerminate()' removes the thread specified by parameter 'thread_id'
 *              from the list of active threads.  If the thread is currently RUNNING, the thread
 *              terminates and the execution continues with the next READY thread.
 *
 *              If no such thread exists, the function will not terminate the running thread, but return
 *              'osErrorResource'.
 *
 * Arguments  : thread_id    is the ID of the thread
 *
 * Returns    : osOK                the thread has been joined successfully.
 *              osErrorParameter    thread_id is NULL or invalid.
 *              osErrorResource     the thread is in an invalid state.
 *              osErrorISR          the function osThreadSuspend cannot be called from interrupt service routines.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osThreadTerminate(osThreadId_t  thread_id)
{
  osThread_t   *p_thread;
  CPU_STK      *p_stk_base;
  RTOS_ERR      err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_thread = (osThread_t *)thread_id;

  if (p_thread == (osThread_t *)0) {
    return osErrorParameter;
  }

  p_stk_base = p_thread->tcb.StkBasePtr;

  OSTaskDel(&p_thread->tcb, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      if (p_thread->stack_dyn_alloc == DEF_TRUE) {
        free(p_stk_base);
      }
      if (p_thread->obj_dyn_alloc == DEF_TRUE) {
        free(p_thread);
      }
      return osOK;

    case RTOS_ERR_INVALID_ARG:
      return osErrorParameter;

    case RTOS_ERR_INVALID_STATE:
    case RTOS_ERR_OS_ILLEGAL_RUN_TIME:
    default:
      return osErrorResource;
  }
}

/*
 ****************************************************************************************************
 *                                        osThreadGetStackSize()
 *
 * Description: The function 'osThreadGetStackSize()' returns the  stack size of the thread specified
 *              by parameter 'thread_id'. In case of an error, it returns 0.
 *
 * Arguments  : thread_id    is the ID of the thread
 *
 * Returns    : The stack size (in bytes)
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
uint32_t  osThreadGetStackSize(osThreadId_t  thread_id)
{
  osThread_t *p_thread;
  uint32_t    stk_size;

  if (CORE_InIrqContext() == true) {
    return 0u;
  }

  p_thread = (osThread_t *)thread_id;

  if (p_thread == (osThread_t *)0) {
    return 0u;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  stk_size = (uint32_t)p_thread->tcb.StkSize * sizeof(CPU_STK);
  CORE_EXIT_CRITICAL();
  return stk_size;
}

/*
 ****************************************************************************************************
 *                                        osThreadGetStackSpace()
 *
 * Description: The function 'osThreadGetStackSize()' returns the unused stack space still available
 *              for the thread specified by parameter 'thread_id'. In case of an error, it returns 0.
 *
 * Arguments  : thread_id    is the ID of the thread
 *
 * Returns    : The stack size (in bytes)
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 *              2) This function will likely return an incorrect value if called for the currently running task
 ****************************************************************************************************
 */
uint32_t  osThreadGetStackSpace(osThreadId_t  thread_id)
{
#if (OS_CFG_STAT_TASK_STK_CHK_EN == DEF_ENABLED)
  osThread_t *p_thread;
  uint32_t    free_space;

  if (CORE_InIrqContext() == true) {
    return 0u;
  }

  p_thread = (osThread_t *)thread_id;

  if (p_thread == (osThread_t *)0) {
    return 0u;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  free_space = (uint32_t)(p_thread->tcb.StkSize - p_thread->tcb.StkUsed) * sizeof(CPU_STK);
  CORE_EXIT_CRITICAL();
  return free_space;
#else
  (void)thread_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, 0u);
  return 0u;
#endif
}

/*
 ****************************************************************************************************
 *                                        osThreadGetCount()
 *
 * Description: The function 'osThreadGetCount()' returns the number of active threads or 0 in case of an error.
 *
 * Arguments  : None
 *
 * Returns    : The number of active tasks
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
uint32_t  osThreadGetCount(void)
{
  if (CORE_InIrqContext() == true) {
    return 0u;
  }
  return (uint32_t)OSTaskQty;
}

/*
 ****************************************************************************************************
 *                                        osThreadEnumerate()
 *
 * Description: The function 'osThreadEnumerate()' returns the number of enumerated threads or 0 in case
 *              of an error.
 *
 * Arguments  : thread_array      pointer to array for retrieving thread IDs.
 *              array_items       maximum number of items in array for retrieving thread IDs.
 *
 * Returns    : The number of active tasks
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 *              2) This feature require OS_CFG_DBG_EN
 ****************************************************************************************************
 */
uint32_t  osThreadEnumerate(osThreadId_t   *thread_array,
                            uint32_t       array_items)
{
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_TCB   *p_tcb;
  uint32_t  items;

  if (CORE_InIrqContext() == true) {
    return 0u;
  }
  items = 0u;

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  p_tcb = OSTaskDbgListPtr;
  while ((p_tcb != (OS_TCB *)0) && (items < array_items)) {
    *thread_array = p_tcb;
    thread_array++;
    p_tcb = OSTaskDbgListPtr->DbgNextPtr;
    items++;
  }
  CORE_EXIT_CRITICAL();
  return items;
#else
  (void)thread_array;
  (void)array_items;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, 0u);
  return 0u;
#endif
}

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                                     T H R E A D   F L A G S
 ****************************************************************************************************
 ****************************************************************************************************
 */

/*
 ****************************************************************************************************
 *                                        osThreadFlagsSet()
 *
 * Description: Thread Flags are a more specialized version of the Event Flags.  See Event Flags.
 *              While Event Flags can be used to globally signal a number of threads, thread flags are
 *              only sent to a single specific thread.  Every thread instance can receive thread flags
 *              without any additional allocation of a thread flags object.
 *
 * Arguments  : thread_id              is the desired theard ID
 *
 *              flags                  are the flags to set
 *                                     the upper bit (i.e. bit 31, is not allowed to be set)
 *
 * Returns    : osFlagsErrorUnknown    unspecified error.
 *              osFlagsErrorParameter  parameter 'thread_id' is not a valid thread or flags has highest bit set.
 *              osFlagsErrorResource   the thread is in invalid state.
 *
 * Note(s)    : None
 ****************************************************************************************************
 */
uint32_t  osThreadFlagsSet(osThreadId_t  thread_id,
                           uint32_t      flags)
{
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  RTOS_ERR      err;
  osThread_t   *p_thread;
  OS_FLAG_GRP  *p_grp;
  OS_TCB       *p_tcb;
  uint32_t      rdy_flags;
  uint32_t      new_flags;
  uint32_t      ret_flags;
  CORE_DECLARE_IRQ_STATE;

  p_thread = (osThread_t*)thread_id;
  if (p_thread == (osThread_t *)0) {
    return osFlagsErrorParameter;
  }
  p_grp = &p_thread->flag_grp;

  if ((flags & 0x80000000u) == 0x80000000u) {          // Upper bit is not allowed to be set
    return osFlagsErrorParameter;
  }

  CORE_ENTER_ATOMIC();

  new_flags = p_grp->Flags |= flags;                            // New flags after set

  p_tcb = p_grp->PendList.HeadPtr;                              // Loop over pending tasks
  while (p_tcb != DEF_NULL) {
    if (p_tcb->FlagsOpt & OS_OPT_PEND_FLAG_CONSUME) {           // Pender might consume the flags?
      switch (p_tcb->FlagsOpt & OS_OPT_PEND_FLAG_MASK) {
        case OS_OPT_PEND_FLAG_SET_ALL:                          // -> Pender waiting for all flags to be set
          rdy_flags = (new_flags & p_tcb->FlagsPend);
          if (rdy_flags == p_tcb->FlagsPend) {                  //    -> Pender will really consume?
            new_flags &= ~rdy_flags;                            //    -> Simulate the consumption
          }
          break;

        case OS_OPT_PEND_FLAG_SET_ANY:                          // -> Pender waiting for any flags to be set
          rdy_flags = (new_flags & p_tcb->FlagsPend);
          if (rdy_flags != 0u) {                                //    -> Pender will really consume?
            new_flags &= ~rdy_flags;                            //    -> Simulate the consumption
          }
          break;

        default:
          break;
      }
    }
    p_tcb = p_tcb->PendNextPtr;                                 // Point to next task waiting for event flag(s)
  }

  CORE_EXIT_ATOMIC();

  OSFlagPost(p_grp, (OS_FLAGS)flags, OS_OPT_POST_FLAG_SET, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      ret_flags = new_flags;
      break;

    default:
      ret_flags = osFlagsErrorResource;
      break;
  }
  return ret_flags;
#else
  (void)thread_id;
  (void)flags;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osFlagsErrorUnknown);
  return osFlagsErrorUnknown;
#endif
}

/*
 ****************************************************************************************************
 *                                        osThreadFlagsClear()
 *
 * Description: The function 'osThreadFlagsClear()' clears the specified flags for the currently running
 *              thread.  It returns the flags before clearing, or an error code if highest bit is set
 *              (refer to Flags Functions Error Codes).
 *
 * Arguments  : flags                  are the flags to clear
 *
 * Returns    : osFlagsErrorUnknown    unspecified error, i.e. not called from a running threads context.
 *              osFlagsErrorParameter  parameter flags has highest bit set.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
uint32_t  osThreadFlagsClear(uint32_t  flags)
{
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  osThread_t   *p_thread;
  OS_FLAG_GRP  *p_grp;
  RTOS_ERR      err;
  uint32_t      old_flags;

  if (CORE_InIrqContext() == true) {
    return (uint32_t)osErrorISR;
  }

  if ((flags & 0x80000000u) == 0x80000000u) {          // Upper bit is not allowed to be set
    return osFlagsErrorParameter;
  }

  p_thread = (osThread_t*)OSTCBCurPtr;
  p_grp = &p_thread->flag_grp;
  old_flags = p_grp->Flags;
  (void)OSFlagPost(p_grp, (OS_FLAGS)flags, OS_OPT_POST_FLAG_CLR, &err);

  return old_flags;
#else
  (void)flags;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osFlagsErrorUnknown);
  return osFlagsErrorUnknown;
#endif
}

/*
 ****************************************************************************************************
 *                                        osThreadFlagsGet()
 *
 * Description: The function 'osThreadFlagsGet()' returns the flags currently set for the currently
 *              running thread.  If called without a active and currently running thread
 *              'osThreadFlagsGet()' return zero.
 *
 * Arguments  : None
 *
 * Returns    : the flags
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
uint32_t  osThreadFlagsGet(void)
{
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  osThread_t   *p_thread;
  OS_FLAG_GRP  *p_grp;
  uint32_t      flags;

  if (CORE_InIrqContext() == true) {
    return (uint32_t)osErrorISR;
  }

  p_thread = (osThread_t*)OSTCBCurPtr;
  p_grp = &p_thread->flag_grp;
  flags = p_grp->Flags;

  return flags;
#else
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osFlagsErrorUnknown);
  return osFlagsErrorUnknown;
#endif
}

/*
 ****************************************************************************************************
 *                                        osThreadFlagsWait()
 *
 * Description: The function 'osThreadFlagsWait()' suspends the execution of the currently RUNNING
 *              thread until any or all of the thread flags specified with the parameter flags are set.
 *
 *              When these thread flags are already set, the function returns instantly.  Otherwise the
 *              thread is put into the state BLOCKED.
 *
 * Arguments  : flags                  are the flags to wait for
 *
 *              options                wait for:
 *                                         osFlagsWaitAny
 *                                         osFlagsWaitAnyNoClear
 *                                         osFlagsWaitAll
 *                                         osFlagsWaitAllNoClear
 *
 *              timeout                the amount of time to wait for the desired flags
 *
 * Returns    : osFlagsErrorUnknown    unspecified error, i.e. not called from a running threads context.
 *              osFlagsErrorTimeout    awaited flags have not been set in the given time.
 *              osFlagsErrorResource   awaited flags have not been set when no timeout was specified.
 *              osFlagsErrorParameter  Parameter flags has highest bit set.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
uint32_t  osThreadFlagsWait(uint32_t   flags,
                            uint32_t   options,
                            uint32_t   timeout)
{
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  osThread_t   *p_thread;
  OS_FLAG_GRP  *p_grp;
  RTOS_ERR      err;
  OS_TICK       os_timeout;
  OS_OPT        opt;
  CPU_TS        ts;
  uint32_t      old_flags;
  uint32_t      rdy_flags;
  uint32_t      rtn_flags;

  if (CORE_InIrqContext() == true) {
    return (uint32_t)osErrorISR;
  }

  if ((flags & 0x80000000u) == 0x80000000u) {                           // Upper bit is not
                                                                        // allowed to be set
    return osFlagsErrorParameter;
  }

  if (timeout == osWaitForever) {
    opt = OS_OPT_PEND_BLOCKING;
    os_timeout = 0u;                                                    // Specifying 0 in MicriumOS
                                                                        // means wait forever w/ blocking
  } else if (timeout == 0u) {
    opt = OS_OPT_PEND_NON_BLOCKING;
    os_timeout = 0u;                                                    // The timeout parameter is
                                                                        // ignored by MicriumOS in
                                                                        // non-blocking mode
  } else {
    opt = OS_OPT_PEND_BLOCKING;
    os_timeout = timeout;
  }

  if (options & osFlagsWaitAll) {
    opt |= OS_OPT_PEND_FLAG_SET_ALL;
  } else {
    opt |= OS_OPT_PEND_FLAG_SET_ANY;
  }

  if (!(options & osFlagsNoClear)) {
    opt |= OS_OPT_PEND_FLAG_CONSUME;
  }

  p_thread = (osThread_t *)OSTCBCurPtr;
  p_grp = &p_thread->flag_grp;
  old_flags = p_grp->Flags;                                              // Get flags

  rdy_flags = (uint32_t)OSFlagPend(p_grp, (OS_FLAGS)flags, os_timeout, opt, &ts, &err);
  rtn_flags = rdy_flags | old_flags;

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_TIMEOUT:
      return osFlagsErrorTimeout;

    case RTOS_ERR_NONE:
      return rtn_flags;

    case RTOS_ERR_WOULD_BLOCK:
      if (rdy_flags != (uint32_t)0u) {
        return rtn_flags;
      } else {
        return osFlagsErrorResource;
      }

    case RTOS_ERR_OS_OBJ_DEL:
    case RTOS_ERR_OS_SCHED_LOCKED:
    case RTOS_ERR_ABORT:
    default:
      return osFlagsErrorResource;
  }
#else
  (void)flags;
  (void)options;
  (void)timeout;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osFlagsErrorUnknown);
  return osFlagsErrorUnknown;
#endif
}

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                                  T I M E R   M A N A G E M E N T
 ***************************************************************************************************/
/****************************************************************************************************
 *                                           osTimerNew()
 *
 * Description: The function 'osTimerNew()' creates an one-shot or periodic timer and associates it with
 *              a callback function with argument.  The timer is in stopped state until it is started
 *              with 'osTimerStart()'.  The function can be safely called before the RTOS is started
 *              (call to 'osKernelStart()'), but not before it is initialized (call to 'osKernelInitialize()').
 *
 *              The function 'osTimerNew()' returns the pointer to the timer object identifier or NULL
 *              in case of an error.
 *
 * Arguments  : func            pointer to function that will be called when the timer expires
 *
 *              type            the type of timer created:
 *                                  osTimerOnce      A one-shot timer
 *                                  osTimerPeriodic  A periodic timer
 *
 *              argument        a pointer to arguments passed to the callback function (func) when the
 *                                  timer expires.
 *
 *              attr            timer attributes or NULL to use default values
 *
 * Returns    : timer ID for reference by other functions or NULL in case of error.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osTimerId_t  osTimerNew(osTimerFunc_t          func,
                        osTimerType_t          type,
                        void                   *argument,
                        const  osTimerAttr_t   *attr)
{
#if defined CMSIS_RTOS2_TIMER_TASK_EN && (CMSIS_RTOS2_TIMER_TASK_EN == DEF_ENABLED)
  osTimer_t    *p_tmr;
  const char   *p_name;

  if (CORE_InIrqContext() == true) {
    return (osTimerId_t)0;
  }

  if (func == (osTimerFunc_t)0) {                                   // Caller must specify a callback
    return (osTimerId_t)0;
  }

  p_name = "Timer";
  if (attr == NULL) {
    p_tmr = (osTimer_t *)malloc(sizeof(osTimer_t));
    if (p_tmr == (osTimer_t *)0) {
      return (osTimerId_t)0;
    }
    p_tmr->dyn_alloc = DEF_TRUE;
  } else {
    if (attr->cb_mem == (void *)0) {
      p_tmr = (osTimer_t *)malloc(sizeof(osTimer_t));
      if (p_tmr == (osTimer_t *)0) {
        return (osTimerId_t)0;
      }
      p_tmr->dyn_alloc = DEF_TRUE;
    } else {
      if (attr->cb_size < sizeof(osTimer_t) || (uint32_t)attr->cb_mem % sizeof(CPU_ALIGN)) {
        return (osTimerId_t)0;
      }
      p_tmr = (osTimer_t *)attr->cb_mem;
      p_tmr->dyn_alloc = DEF_FALSE;
    }
    if (attr->name != (const char *)0) {
      p_name = attr->name;
    }
  }

  p_tmr->type = type;
  p_tmr->name = p_name;
  p_tmr->callback = func;
  p_tmr->callback_data = argument;

  return (osTimerId_t)p_tmr;
#else
  (void)func;
  (void)type;
  (void)argument;
  (void)attr;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, (osTimerId_t)0);
  return (osTimerId_t)0;
#endif
}

/*
 ****************************************************************************************************
 *                                          osTimerGetName()
 *
 * Description: The function 'osTimerGetName()' returns the pointer to the name string of the timer
 *              identified by parameter 'timer_id' or NULL in case of an error.
 *
 * Arguments  : timer_id      is the timer ID returned by 'osTimerNew()'
 *
 * Returns    : A pointer to the ASCII string containing the name of the timer.
 *              NULL             upon error
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
const  char  *osTimerGetName(osTimerId_t  timer_id)
{
#if defined CMSIS_RTOS2_TIMER_TASK_EN && (CMSIS_RTOS2_TIMER_TASK_EN == DEF_ENABLED) && (OS_CFG_DBG_EN == DEF_ENABLED)
  osTimer_t *p_tmr;

  if (CORE_InIrqContext() == true) {
    return NULL;
  }
  p_tmr = (osTimer_t *)timer_id;
  if (p_tmr == (osTimer_t *)0) {
    return NULL;
  }

  return p_tmr->name;
#else
  (void)timer_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, NULL);
  return NULL;
#endif
}

/*
 ****************************************************************************************************
 *                                          osTimerStart()
 *
 * Description: The function 'osTimerStart()' starts or restarts a timer specified by the parameter
 *              'timer_id'.  The parameter ticks specifies the value of the timer in time ticks.
 *
 * Arguments  : timer_id           is the timer ID returned by 'osTimerNew()'
 *
 *              ticks              is the one shot delay (osTimerOnce) or
 *                                 the period of timer repeat (osTimerPeriodic)
 *
 * Returns    : osOK               the specified timer has been started or restarted.
 *              osErrorISR         osTimerStart() cannot be called from interrupt service routines.
 *              osErrorParameter   parameter 'timer_id' is either NULL or invalid or ticks is incorrect.
 *              osErrorResource    the timer is in an invalid state.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osTimerStart(osTimerId_t  timer_id,
                         uint32_t     ticks)
{
#if defined CMSIS_RTOS2_TIMER_TASK_EN && (CMSIS_RTOS2_TIMER_TASK_EN == DEF_ENABLED)
  osTimer_t   *p_tmr;
  sl_status_t status;
  uint32_t    delay;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  if (ticks == 0u) {                                                // Cannot be 0 ticks
    return osErrorParameter;
  }

  p_tmr = (osTimer_t *)timer_id;

  if (p_tmr ==  (osTimer_t *)0) {
    return osErrorParameter;
  }

  // Convert from OS tick to sleeptimer ticks
  delay = (uint64_t)(((uint64_t)ticks * (uint64_t)sl_sleeptimer_get_timer_frequency()) + ((uint64_t)OSCfg_TickRate_Hz - 1u)) / OSCfg_TickRate_Hz;

  if (p_tmr->type == osTimerOnce) {
    status = sl_sleeptimer_start_timer(&p_tmr->handle, delay, sleeptimer_callback, (void *)p_tmr, 0, 0);
  } else {
    status = sl_sleeptimer_start_periodic_timer(&p_tmr->handle, delay, sleeptimer_callback, (void *)p_tmr, 0, 0);
  }
  if (status == SL_STATUS_OK) {
    return osOK;
  } else {
    return osErrorResource;
  }

#else
  (void)timer_id;
  (void)ticks;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                          osTimerStop()
 *
 * Description: The function 'osTimerStop()' stops the timer specified by the parameter 'timer_id'.
 *
 * Arguments  : timer_id           is the timer ID returned by 'osTimerNew()'
 *
 * Returns    : osOK               the specified timer has been stopped
 *              osErrorISR         osTimerStop() cannot be called from interrupt service routines.
 *              osErrorParameter   parameter 'timer_id' is either NULL or invalid or ticks is incorrect.
 *              osErrorResource    the timer is in an invalid state.
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osTimerStop(osTimerId_t  timer_id)
{
#if defined CMSIS_RTOS2_TIMER_TASK_EN && (CMSIS_RTOS2_TIMER_TASK_EN == DEF_ENABLED)
  osTimer_t *p_tmr;
  sl_status_t status;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_tmr = (osTimer_t *)timer_id;
  if (p_tmr == (osTimer_t *)0) {
    return osErrorParameter;
  }

  status = sl_sleeptimer_stop_timer(&p_tmr->handle);
  if (status == SL_STATUS_OK) {
    return osOK;
  } else {
    return osErrorResource;
  }

#else
  (void)timer_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                        osTimerIsRunning()
 *
 * Description: The function 'osTimerIsRunning()' checks whether a timer specified by parameter 'timer_id'
 *              is running.
 *
 * Arguments  : timer_id           is the timer ID returned by 'osTimerNew()'
 *
 * Returns    : 1                  if the timer is running
 *              0                  if stopped or an error occurs
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
uint32_t  osTimerIsRunning(osTimerId_t  timer_id)
{
#if defined CMSIS_RTOS2_TIMER_TASK_EN && (CMSIS_RTOS2_TIMER_TASK_EN == DEF_ENABLED)
  osTimer_t  *p_tmr;
  sl_status_t status;
  bool        running;

  if (CORE_InIrqContext() == true) {
    return 0u;
  }

  p_tmr = (osTimer_t*)timer_id;
  if (p_tmr == (osTimer_t*)0) {
    return 0u;
  }

  status = sl_sleeptimer_is_timer_running(&p_tmr->handle, &running);

  if (status != SL_STATUS_OK) {
    return 0u;
  }

  return (uint32_t)running;

#else
  (void)timer_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, 0u);
  return 0u;
#endif
}

/*
 ****************************************************************************************************
 *                                           osTimerDelete()
 *
 * Description: The function 'osTimerDelete()' deletes the timer object specified by parameter 'timer_id'.
 *
 * Arguments  : timer_id         is the timer ID returned by 'osTimerNew()'
 *
 * Returns    : osOK             the timer object has been deleted.
 *              osErrorParameter the parameter timer_id is NULL or invalid.
 *              osErrorResource  the timer is in an invalid state.
 *              osErrorISR       osTimerDelete() cannot be called from interrupt service routines.
 *
 *
 * Note(s)    : 1) This function CANNOT be called from an ISR
 ****************************************************************************************************
 */
osStatus_t  osTimerDelete(osTimerId_t  timer_id)
{
#if defined CMSIS_RTOS2_TIMER_TASK_EN && (CMSIS_RTOS2_TIMER_TASK_EN == DEF_ENABLED)
  osTimer_t   *p_tmr;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_tmr = (osTimer_t *)timer_id;
  if (p_tmr == (osTimer_t *)0) {
    return osErrorParameter;
  }

  sl_sleeptimer_stop_timer(&p_tmr->handle);

  if (p_tmr->dyn_alloc == DEF_TRUE) {
    free(p_tmr);
  }
  return osOK;

#else
  (void)timer_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                               M E M O R Y   P O O L   M A N A G E M E N T
 ****************************************************************************************************
 ****************************************************************************************************
 */

/*
 ****************************************************************************************************
 *                                           osMemoryPoolNew()
 *
 * Description: The function 'osMemoryPoolNew()' creates and initializes a memory pool object and returns
 *              the pointer to the memory pool object identifier or NULL in case of an error.
 *
 *              It can be safely called before the RTOS is started (call to 'osKernelStart()'), but not
 *              before it is initialized (call to 'osKernelInitialize()').
 *
 * Arguments  : block_count            maximum number of memory blocks in memory pool.
 *              block_size             memory block size in bytes.
 *              attr                   memory pool attributes.  If NULL, use default values.
 *
 * Returns    : Memory Pool ID for reference by other functions or NULL in case of error.
 *
 * Note(s)    : 1) MicriumOS DOES NOT support this functionality
 ****************************************************************************************************
 */
osMemoryPoolId_t osMemoryPoolNew(uint32_t block_count,
                                 uint32_t block_size,
                                 const osMemoryPoolAttr_t *attr)
{
  (void)block_count;
  (void)block_size;
  (void)attr;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, NULL);
  return NULL;
}

/*
 ****************************************************************************************************
 *                                       osMemoryPoolGetName()
 *
 * Description: The function 'osMemoryPoolName()' returns the pointer to the name string of the memory
 *              pool identified by parameter 'mp_id' or NULL in case of an error.
 *
 * Arguments  : mp_id      is the memory pool ID returned by 'osMemoryPoolNew()'
 *
 * Returns    : A pointer to the ASCII string containing the name of the memory pool.
 *              NULL             upon error
 *
 * Note(s)    : 1) MicriumOS DOES NOT support this functionality
 ****************************************************************************************************
 */
const char * osMemoryPoolGetName(osMemoryPoolId_t mp_id)
{
  (void)mp_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, NULL);
  return NULL;
}

/*
 ****************************************************************************************************
 *                                       osMemoryPoolAlloc()
 *
 * Description: The blocking function 'osMemoryPoolAlloc()' allocates the memory pool parameter 'mp_id'
 *              and returns a pointer to the address of the allocated memory or 0 in case of an error.
 *
 *              The parameter 'timeout' specifies how long the system waits to allocate the memory.
 *              While the system waits, the thread that is calling this function is put into the
 *              BLOCKED state.  The thread will become READY as soon as at least one block of memory
 *              gets available.
 *
 * Arguments  : mp_id      is the memory pool ID returned by 'osMemoryPoolNew()'
 *
 *              timeout    when timeout is 0, the function returns instantly (i.e. try semantics).
 *                         when timeout is set to 'osWaitForever' the function will wait for an infinite
 *                            time until the memory is allocated (i.e. wait semantics).
 *                         all other values specify a time in kernel ticks for a timeout
 *                            (i.e. timed-wait semantics).
 *
 * Returns    : The result is the pointer to the memory block allocated, or NULL if no memory is available.
 *
 * Note(s)    : 1) MicriumOS DOES NOT support this functionality
 ****************************************************************************************************
 */
void * osMemoryPoolAlloc(osMemoryPoolId_t mp_id,
                         uint32_t timeout)
{
  (void)mp_id;
  (void)timeout;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, NULL);
  return NULL;
}

/*
 ****************************************************************************************************
 *                                       osMemoryPoolFree()
 *
 * Description: The function 'osMemoryPoolFree()' frees the memory pool block specified by the parameter
 *              'block' in the memory pool object specified by the parameter 'mp_id'.  The memory block
 *              is put back to the list of available blocks.
 *
 *              If another thread is waiting for memory to become available the thread is put to READY state.
 *
 * Arguments  : mp_id      is the memory pool ID returned by 'osMemoryPoolNew()'
 *
 *              block      address of the allocated memory block to be returned to the memory pool.
 *
 * Returns    : osOK                the memory has been freed.
 *              osErrorParameter    parameter mp_id is NULL or invalid, block points to invalid memory.
 *              osErrorResource     the memory pool is in an invalid state.
 *
 * Note(s)    : 1) MicriumOS DOES NOT support this functionality
 ****************************************************************************************************
 */
osStatus_t osMemoryPoolFree(osMemoryPoolId_t mp_id,
                            void *block)
{
  (void)mp_id;
  (void)block;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, NULL);
  return osError;
}

/*
 ****************************************************************************************************
 *                                     osMemoryPoolGetCapacity()
 *
 * Description: The function 'osMemoryPoolGetCapacity()' returns the maximum number of memory blocks
 *              in the memory pool object specified by parameter 'mp_id' or 0 in case of an error.
 *
 * Arguments  : mp_id      is the memory pool ID returned by 'osMemoryPoolNew()'
 *
 * Returns    : Maximum number of memory blocks
 *
 * Note(s)    : 1) MicriumOS DOES NOT support this functionality
 ****************************************************************************************************
 */
uint32_t osMemoryPoolGetCapacity(osMemoryPoolId_t mp_id)
{
  (void)mp_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, 0);
  return 0;
}

/*
 ****************************************************************************************************
 *                                 osMemoryPoolGetBlockSize()
 *
 * Description: The function 'osMemoryPoolGetBlockSize()' returns the memory block size in bytes in
 *              the memory pool object specified by parameter 'mp_id' or 0 in case of an error.
 *
 * Arguments  : mp_id      is the memory pool ID returned by 'osMemoryPoolNew()'
 *
 * Returns    : Size (in bytes) of each block
 *              0  upon error
 *
 * Note(s)    : 1) MicriumOS DOES NOT support this functionality
 ****************************************************************************************************
 */
uint32_t osMemoryPoolGetBlockSize(osMemoryPoolId_t mp_id)
{
  (void)mp_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, 0);
  return 0;
}

/*
 ****************************************************************************************************
 *                                    osMemoryPoolGetCount()
 *
 * Description: The function 'osMemoryPoolGetCount()' returns the number of memory blocks used in the
 *              memory pool object specified by parameter 'mp_id' or 0 in case of an error.
 *
 * Arguments  : mp_id      is the memory pool ID returned by 'osMemoryPoolNew()'
 *
 * Returns    : number of memory blocks used.
 *              0  upon error
 *
 * Note(s)    : 1) MicriumOS DOES NOT support this functionality
 ****************************************************************************************************
 */
uint32_t osMemoryPoolGetCount(osMemoryPoolId_t mp_id)
{
  (void)mp_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, 0);
  return 0;
}

/*
 ****************************************************************************************************
 *                                    osMemoryPoolGetSpace()
 *
 * Description: The function 'osMemoryPoolGetSpace()' returns the number of memory blocks available
 *              in the memory pool object specified by parameter 'mp_id' or 0 in case of an error.
 *
 * Arguments  : mp_id      is the memory pool ID returned by 'osMemoryPoolNew()'
 *
 * Returns    : number of memory blocks available.
 *              0  upon error
 *
 * Note(s)    : 1) MicriumOS DOES NOT support this functionality
 ****************************************************************************************************
 */
uint32_t osMemoryPoolGetSpace(osMemoryPoolId_t mp_id)
{
  (void)mp_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, 0);
  return 0;
}

/*
 ****************************************************************************************************
 *                                       osMemoryPoolDelete()
 *
 * Description: The function 'osMemoryPoolDelete()' deletes a memory pool object specified by parameter
 *              'mp_id'.  It releases internal memory obtained for memory pool handling.
 *
 *              After this call, the 'mp_id' is no longer valid and cannot be used.  The memory pool
 *              may be created again using the function 'osMemoryPoolNew()'.
 *
 * Arguments  : mp_id              is the memory pool ID returned by 'osMemoryPoolNew()'
 *
 * Returns    : osOK               the memory pool object has been deleted.
 *              osErrorParameter   parameter mp_id is NULL or invalid.
 *              osErrorResource    the memory pool is in an invalid state.
 *              osErrorISR         'osMemoryPoolDelete()' cannot be called from an ISR
 *
 * Note(s)    : 1) MicriumOS DOES NOT support this functionality
 ****************************************************************************************************
 */
osStatus_t osMemoryPoolDelete(osMemoryPoolId_t mp_id)
{
  (void)mp_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, osError);
  return osError;
}

/*
 ****************************************************************************************************
 ****************************************************************************************************
 *                          M E S S A G E   Q U E U E   M A N A G E M E N T
 ****************************************************************************************************
 ****************************************************************************************************
 */

/*
 ****************************************************************************************************
 *                                          osMessageQueueNew()
 *
 * Description: The function 'osMessageQueueNew()' creates and initializes a message queue object.
 *              The function returns a message queue object identifier or NULL in case of an error.
 *
 *              The function can be called after kernel initialization with 'osKernelInitialize()'.
 *              It is possible to create message queue objects before the RTOS kernel is started with
 *              'osKernelStart()'.
 *
 *              The total amount of memory required for the message queue data is at least 'msg_count * msg_size'.
 *              The 'msg_size' is rounded up to a double even number to ensure 32-bit alignment of the memory blocks.
 *
 *              The memory blocks allocated from the message queue have a fixed size defined with the parameter 'msg_size'.
 *
 * Arguments  : msg_count        is the maximum number of messages in the queue
 *
 *              msg_size         maximum message size (in bytes)
 *
 *              attr             attribute structure passed to the message queue creation code.
 *                               specifying NULL assumes defaults.
 *
 *                               Message queue attributes are:
 *
 *                               .name         name of the message queue
 *                               .attr_bits    reserved (should be 0)
 *                               .cb_mem       pointer to message queue control block
 *                                                (allocated dynamically if NULL or not specified)
 *                               .cb_size      size of the message queue control block (in bytes)
 *                               .mq_mem       pointer to storage of messages
 *                                                (allocated dynamically if NULL or not specified)
 *                               .mq_size      size of each message (in bytes)
 *                                                (the minimum is 'msg_count' * 'msg_size')
 *
 * Returns    : The message queue ID           upon success
 *              NULL                           upon error
 ****************************************************************************************************
 */
osMessageQueueId_t osMessageQueueNew(uint32_t msg_count,
                                     uint32_t msg_size,
                                     const osMessageQueueAttr_t *attr)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osMessageQueue_t   *p_msgqueue;
  CPU_CHAR           *p_name;
  RTOS_ERR            err;

  if (CORE_InIrqContext() == true) {
    return (osMessageQueueId_t)0;                                              // Can't create a msgqueue from an ISR
  }
  if (msg_count == 0) {                                                        // Validate msg_count
    return (osMessageQueueId_t)0;
  }
  if (msg_size == 0) {                                                         // Validate msg_size
    return (osMessageQueueId_t)0;
  } else {
    msg_size = (msg_size + sizeof(CPU_ALIGN) - 1) & ~(sizeof(CPU_ALIGN) - 1);        // Align to 4 bytes
  }
  p_name = (CPU_CHAR *)"QueueName?";
  if (attr == 0) {
    p_msgqueue = (osMessageQueue_t *)malloc(sizeof(osMessageQueue_t));
    if (p_msgqueue == (osMessageQueue_t *)0) {
      return (osMessageQueueId_t)0;
    } else {
      p_msgqueue->obj_dyn_alloc = DEF_TRUE;
      p_msgqueue->buf = (uint8_t *) malloc(msg_size * msg_count);
      if (p_msgqueue->buf == (uint8_t *)0) {
        free(p_msgqueue);
        return (osMessageQueueId_t)0;
      }
      p_msgqueue->buf_dyn_alloc = DEF_TRUE;
    }
  } else {
    if (attr->cb_mem == (void *)0) {
      p_msgqueue = (osMessageQueue_t *)malloc(sizeof(osMessageQueue_t));
      if (p_msgqueue == (osMessageQueue_t *)0) {
        return (osMessageQueueId_t)0;
      }
      p_msgqueue->obj_dyn_alloc = DEF_TRUE;
    } else {
      if (attr->cb_size < sizeof(osMessageQueue_t) || (uint32_t)attr->cb_mem % sizeof(CPU_ALIGN)) {
        return (osMessageQueueId_t)0;
      }
      p_msgqueue = attr->cb_mem;
      p_msgqueue->obj_dyn_alloc = DEF_FALSE;
    }
    if (attr->mq_size == 0u) {
      p_msgqueue->buf = (uint8_t *)malloc(msg_size * msg_count);
      if (p_msgqueue->buf == (uint8_t *)0) {
        if (p_msgqueue->obj_dyn_alloc == DEF_TRUE) {
          free(p_msgqueue);
        }
        return (osMessageQueueId_t)0;
      }
      p_msgqueue->buf_dyn_alloc = DEF_TRUE;
    } else if ((attr->mq_size != 0u) && (attr->mq_mem == NULL)) {
      if (attr->mq_size < msg_size * msg_count) {
        if (p_msgqueue->obj_dyn_alloc == DEF_TRUE) {
          free(p_msgqueue);
        }
        return (osMessageQueueId_t)0;
      }
      p_msgqueue->buf = (uint8_t *)malloc(attr->mq_size);
      if (p_msgqueue->buf == (uint8_t *)0) {
        if (p_msgqueue->obj_dyn_alloc == DEF_TRUE) {
          free(p_msgqueue);
        }
        return (osMessageQueueId_t)0;
      }
      p_msgqueue->buf_dyn_alloc = DEF_TRUE;
    } else {
      if (attr->mq_mem == NULL
          || attr->mq_size < msg_size * msg_count
          || (uint32_t)attr->mq_mem % sizeof(CPU_ALIGN)) {
        if (p_msgqueue->obj_dyn_alloc == DEF_TRUE) {
          free(p_msgqueue);
        }
        return (osMessageQueueId_t)0;
      }
      p_msgqueue->buf = (uint8_t *)attr->mq_mem;
      p_msgqueue->buf_dyn_alloc = DEF_FALSE;
    }
    if (attr->name != (const char *)0) {
      p_name = (CPU_CHAR *)attr->name;
    }
  }
  p_msgqueue->msg_count  = msg_count;
  p_msgqueue->msg_size   = msg_size;
  p_msgqueue->msg_queued = 0;
  p_msgqueue->msg_head   = 0;
  p_msgqueue->msg_tail   = 0;

  OSSemCreate(&p_msgqueue->sem_put, p_name, msg_count, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    if (p_msgqueue->buf_dyn_alloc == DEF_TRUE) {
      free(p_msgqueue->buf);
    }
    if (p_msgqueue->obj_dyn_alloc == DEF_TRUE) {
      free(p_msgqueue);
    }
    return (osMessageQueueId_t)0;
  }

  OSSemCreate(&p_msgqueue->sem_get, p_name, 0, &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    if (p_msgqueue->buf_dyn_alloc == DEF_TRUE) {
      free(p_msgqueue->buf);
    }
    if (p_msgqueue->obj_dyn_alloc == DEF_TRUE) {
      free(p_msgqueue);
    }
    return (osMessageQueueId_t)0;
  }

  return (osMessageQueueId_t)p_msgqueue;
#else
  (void) msg_count;
  (void) msg_size;
  (void) attr;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_AVAIL, (osMessageQueueId_t)0);
  return (osMessageQueueId_t)0;
#endif
}

/*
 ****************************************************************************************************
 *                                         osMessageQueueGetName()
 *
 * Description: The function 'osMessageQueueGetName()' returns the pointer to the name string of the
 *              message queue identified by parameter 'mq_id' or NULL in case of an error.
 *
 * Arguments  : mq_id       is the message queue ID returned by 'osMessageQueueNew()'
 *
 * Returns    : A pointer to the ASCII string containing the name of the mutex.
 *              NULL             upon error
 ****************************************************************************************************
 */
const char * osMessageQueueGetName(osMessageQueueId_t mq_id)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED) && (OS_CFG_DBG_EN == DEF_ENABLED)
  osMessageQueue_t   *p_msgqueue;

  if (CORE_InIrqContext() == true) {
    return NULL;
  }
  p_msgqueue = (osMessageQueue_t *)mq_id;
  if (p_msgqueue == (osMessageQueue_t *)0) {
    return NULL;
  }

  return p_msgqueue->sem_put.NamePtr;
#else
  (void)mq_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, NULL);
  return NULL;
#endif
}

/*
 ****************************************************************************************************
 *                                         osMessageQueuePut()
 *
 * Description: The blocking function 'osMessageQueuePut()' puts the message pointed to by 'msg_ptr'
 *              into the message queue specified by parameter 'mq_id'.
 *
 *              The parameter 'msg_prio' is used to sort message according their priority
 *              (higher numbers indicate a higher priority) on insertion.
 *
 *              The parameter 'timeout' specifies how long the system waits to put the message into
 *              the queue.  While the system waits, the thread that is calling this function is put
 *              into the BLOCKED state. The parameter timeout can have the following values:
 *
 *              when timeout is 0, the function returns instantly (i.e. try semantics).
 *              when timeout is set to osWaitForever the function will wait for an infinite time
 *                  until the message is delivered (i.e. wait semantics).
 *              all other values specify a time in kernel ticks for a timeout (i.e. timed-wait semantics).
 *
 * Arguments  : mq_id       is the message queue ID returned by 'osMessageQueueNew()'
 *
 *              msg_ptr     pointer to message to send
 *
 *              msg_prio    priority of message sent
 *
 *              timeout     when timeout is 0, the function returns instantly (i.e. try semantics).
 *                          when timeout is set to 'osWaitForever()' the function will wait for an
 *                              infinite time until the message is delivered (i.e. wait semantics).
 *                          all other values specify a time in kernel ticks for a timeout
 *                              (i.e. timed-wait semantics).
 *
 * Returns    : osOK             the message has been put in the queue
 *              osErrorTimeout   the message could not be put into the queue within the timeout specified
 *              osErrorParameter the parameter 'mq_id' is NULL or invalid.
 *              osErrorResource  not enough space in the queue
 ****************************************************************************************************
 */
osStatus_t osMessageQueuePut(osMessageQueueId_t mq_id,
                             const void *msg_ptr,
                             uint8_t msg_prio,
                             uint32_t timeout)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osMessageQueue_t   *p_msgqueue;
  CPU_TS              ts;
  RTOS_ERR            err;
  uint32_t            msg_id, msg_size;
  CORE_DECLARE_IRQ_STATE;

  (void) msg_prio;

  p_msgqueue = (osMessageQueue_t *)mq_id;
  if (p_msgqueue == (osMessageQueue_t *)0) {
    return osErrorParameter;
  }

  if (timeout == 0u) {
    OSSemPend(&p_msgqueue->sem_put, (OS_TICK)0u, OS_OPT_PEND_NON_BLOCKING, &ts, &err);
  } else {
    if (CORE_InIrqContext() == true) {
      return osErrorParameter;
    } else {
      OSSemPend(&p_msgqueue->sem_put, (OS_TICK)timeout, OS_OPT_PEND_BLOCKING, &ts, &err);
    }
  }

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_TIMEOUT:
      return osErrorTimeout;

    case RTOS_ERR_ABORT:
    case RTOS_ERR_OS_SCHED_LOCKED:
    case RTOS_ERR_WOULD_BLOCK:
    case RTOS_ERR_OS_OBJ_DEL:
    default:
      return osErrorResource;
  }

  CORE_ENTER_ATOMIC();
  msg_id   = p_msgqueue->msg_head;
  msg_size = p_msgqueue->msg_size;
  p_msgqueue->msg_head = (p_msgqueue->msg_head + 1) %  p_msgqueue->msg_count;
  memcpy(&p_msgqueue->buf[msg_id * msg_size], msg_ptr, msg_size);
  p_msgqueue->msg_queued++;
  CORE_EXIT_ATOMIC();

  OSSemPost(&p_msgqueue->sem_get, OS_OPT_POST_1, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      return osOK;

    case RTOS_ERR_WOULD_OVF:
    default:
      return osErrorResource;
  }
#else
  (void)mq_id;
  (void)msg_ptr;
  (void)msg_prio;
  (void)timeout;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                          osMessageQueueGet()
 *
 * Description: The function 'osMessageQueueGet()' retrieves a message from the message queue specified
 *              by the parameter 'mq_id' and saves it to the buffer pointed to by the parameter 'msg_ptr'.
 *
 *              The message priority is stored to parameter 'msg_prio' if not token{NULL}.
 *
 *              The parameter 'timeout' specifies how long the system waits to retrieve the message from
 *              the queue.  While the system waits, the thread that is calling this function is put into
 *              the BLOCKED state.  The parameter 'timeout' can have the following values:
 *
 *
 *
 *
 * Arguments  : mq_id            the message queue ID
 *
 *              msg_ptr          a pointer to where the message will be placed
 *
 *              msg_prio         a pointer to where the message priority will be deposited to.
 *
 *              timeout          when timeout is 0, the function returns instantly (i.e. try semantics).
 *
 *                               when timeout is set to 'osWaitForever' the function will wait for an infinite
 *                                   time until the message is retrieved (i.e. wait semantics).
 *
 *                               all other values specify a time in kernel ticks for a timeout
 *                                   (i.e. timed-wait semantics).
 *
 * Returns    : osOK             the message has been retrieved from the queue.
 *              osErrorTimeout   the message could not be retrieved from the queue in the given time
 *                                   (timed-wait semantics).
 *              osErrorResource  nothing to get from the queue (try semantics).
 *              osErrorParameter parameter 'mq_id' is NULL or invalid, non-zero timeout specified in an ISR.
 ****************************************************************************************************
 */
osStatus_t osMessageQueueGet(osMessageQueueId_t mq_id,
                             void *msg_ptr,
                             uint8_t *msg_prio,
                             uint32_t timeout)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osMessageQueue_t   *p_msgqueue;
  CPU_TS              ts;
  RTOS_ERR            err;
  uint32_t            msg_id, msg_size;
  CORE_DECLARE_IRQ_STATE;

  (void) msg_prio;

  p_msgqueue = (osMessageQueue_t *)mq_id;
  if (p_msgqueue == (osMessageQueue_t *)0) {
    return osErrorParameter;
  }

  if (timeout == 0u) {
    OSSemPend(&p_msgqueue->sem_get, (OS_TICK)0u, OS_OPT_PEND_NON_BLOCKING, &ts, &err);
  } else {
    if (CORE_InIrqContext() == true) {
      return osErrorParameter;
    } else {
      OSSemPend(&p_msgqueue->sem_get, (OS_TICK)timeout, OS_OPT_PEND_BLOCKING, &ts, &err);
    }
  }

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_TIMEOUT:
      return osErrorTimeout;

    case RTOS_ERR_ABORT:
    case RTOS_ERR_OS_SCHED_LOCKED:
    case RTOS_ERR_WOULD_BLOCK:
    case RTOS_ERR_OS_OBJ_DEL:
    default:
      return osErrorResource;
  }

  CORE_ENTER_ATOMIC();
  msg_id   = p_msgqueue->msg_tail;
  msg_size = p_msgqueue->msg_size;
  p_msgqueue->msg_tail = (p_msgqueue->msg_tail + 1) %  p_msgqueue->msg_count;
  memcpy(msg_ptr, &p_msgqueue->buf[msg_id * msg_size], msg_size);
  p_msgqueue->msg_queued--;
  if (msg_prio != NULL) {
    *msg_prio = 0;
  }
  CORE_EXIT_ATOMIC();

  OSSemPost(&p_msgqueue->sem_put, OS_OPT_POST_1, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      return osOK;

    case RTOS_ERR_WOULD_OVF:
    default:
      return osErrorResource;
  }
#else
  (void)mq_id;
  (void)msg_ptr;
  (void)msg_prio;
  (void)timeout;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                      osMessageQueueGetCapacity()
 *
 * Description: The function 'osMessageQueueGetCapacity()' returns the maximum number of messages in
 *              the message queue object specified by parameter 'mq_id' or 0 in case of an error.
 *
 * Arguments  : mq_id       is the message queue ID returned by 'osMessageQueueNew()'
 *
 * Returns    : The maximum number of messages
 *              0             upon error
 ****************************************************************************************************
 */
uint32_t osMessageQueueGetCapacity(osMessageQueueId_t mq_id)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osMessageQueue_t   *p_msgqueue;

  p_msgqueue = (osMessageQueue_t *)mq_id;
  if (p_msgqueue == (osMessageQueue_t *)0) {
    return 0;
  }

  return p_msgqueue->msg_count;
#else
  (void)mq_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, 0);
  return 0;
#endif
}

/*
 ****************************************************************************************************
 *                                     osMessageQueueGetMsgSize()
 *
 * Description: The function 'osMessageQueueGetMsgSize()' returns the maximum message size in bytes
 *              for the message queue object specified by parameter 'mq_id' or 0 in case of an error.
 *
 * Arguments  : mq_id       is the message queue ID returned by 'osMessageQueueNew()'
 *
 * Returns    : The maximum message size (in bytes)
 *              0             upon error
 ****************************************************************************************************
 */
uint32_t osMessageQueueGetMsgSize(osMessageQueueId_t mq_id)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osMessageQueue_t   *p_msgqueue;

  p_msgqueue = (osMessageQueue_t *)mq_id;
  if (p_msgqueue == (osMessageQueue_t *)0) {
    return 0;
  }

  return p_msgqueue->msg_size;
#else
  (void)mq_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, 0);
  return 0;
#endif
}

/*
 ****************************************************************************************************
 *                                     osMessageQueueGetCount()
 *
 * Description: The function 'osMessageQueueGetCount()' returns the number of queued messages in the
 *              message queue object specified by parameter 'mq_id' or 0 in case of an error.
 *
 * Arguments  : mq_id       is the message queue ID returned by 'osMessageQueueNew()'
 *
 * Returns    : The number of messages in the queue
 *              0             upon error
 ****************************************************************************************************
 */
uint32_t osMessageQueueGetCount(osMessageQueueId_t mq_id)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osMessageQueue_t   *p_msgqueue;

  p_msgqueue = (osMessageQueue_t *)mq_id;
  if (p_msgqueue == (osMessageQueue_t *)0) {
    return 0;
  }

  return p_msgqueue->msg_queued;
#else
  (void)mq_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, 0);
  return 0;
#endif
}

/*
 ****************************************************************************************************
 *                                     osMessageQueueGetSpace()
 *
 * Description: The function 'osMessageQueueGetSpace()' returns the number available slots for messages
 *              in the message queue object specified by parameter 'mq_id' or 0 in case of an error.
 *
 * Arguments  : mq_id       is the message queue ID returned by 'osMessageQueueNew()'
 *
 * Returns    : The number of available slots in the queue
 *              0             upon error
 ****************************************************************************************************
 */
uint32_t osMessageQueueGetSpace(osMessageQueueId_t mq_id)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osMessageQueue_t   *p_msgqueue;

  p_msgqueue = (osMessageQueue_t *)mq_id;
  if (p_msgqueue == (osMessageQueue_t *)0) {
    return 0;
  }

  return p_msgqueue->msg_count - p_msgqueue->msg_queued;
#else
  (void)mq_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, 0);
  return 0;
#endif
}

/*
 ****************************************************************************************************
 *                                       osMessageQueueReset()
 *
 * Description: The function 'osMessageQueueReset()' resets the message queue specified by the parameter
 *              'mq_id'.
 *
 * Arguments  : mq_id             is the message queue ID returned by 'osMessageQueueNew()'
 *
 * Returns    : osOK              the queue has been reset
 *              osErrorParameter  parameter 'mq_id' is NULL or invalid.
 *              osErrorResource   the message queue is in an invalid state.
 *              osErrorISR        'osMessageQueueReset()' cannot be called from interrupt service routines.
 ****************************************************************************************************
 */
osStatus_t osMessageQueueReset(osMessageQueueId_t mq_id)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osMessageQueue_t   *p_msgqueue;
  CPU_TS              ts;
  RTOS_ERR            err;
  CORE_DECLARE_IRQ_STATE;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_msgqueue = (osMessageQueue_t *)mq_id;
  if (p_msgqueue == (osMessageQueue_t *)0) {
    return osErrorParameter;
  }

  while (p_msgqueue->msg_queued != 0) {
    OSSemPend(&p_msgqueue->sem_get, (OS_TICK)0u, OS_OPT_PEND_BLOCKING, &ts, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      break;
    }
    CORE_ENTER_ATOMIC();
    p_msgqueue->msg_queued--;
    p_msgqueue->msg_tail = (p_msgqueue->msg_tail + 1) % p_msgqueue->msg_count;
    CORE_EXIT_ATOMIC();
    OSSemPost(&p_msgqueue->sem_put, OS_OPT_POST_1, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      break;
    }
  }

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      return osOK;

    default:
      return osErrorResource;
  }
#else
  (void)mq_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, osError);
  return osError;
#endif
}

/*
 ****************************************************************************************************
 *                                       osMessageQueueDelete()
 *
 * Description: The function 'osMessageQueueDelete() deletes a message queue object specified by
 *              parameter 'mq_id'.  It releases internal memory obtained for message queue handling.
 *
 *              After this call, the 'mq_id' is no longer valid and cannot be used.
 *
 *              The message queue may be created again using the function 'osMessageQueueNew()'.
 *
 * Arguments  : mq_id             is the message queue ID returned by 'osMessageQueueNew()'
 *
 * Returns    : osOK              the message queue object has been deleted.
 *              osErrorParameter  parameter 'mq_id' is NULL or invalid.
 *              osErrorResource   the message queue is in an invalid state.
 *              osErrorISR        'osMessageQueueDelete()' cannot be called from interrupt service routines.
 ****************************************************************************************************
 */
osStatus_t osMessageQueueDelete(osMessageQueueId_t mq_id)
{
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  osMessageQueue_t  *p_msgqueue;
  RTOS_ERR           err;

  if (CORE_InIrqContext() == true) {
    return osErrorISR;
  }

  p_msgqueue = (osMessageQueue_t *)mq_id;
  if (p_msgqueue == (osMessageQueue_t *)0) {
    return osErrorParameter;
  }

  OSSemDel(&p_msgqueue->sem_put, OS_OPT_DEL_ALWAYS, &err);
  OSSemDel(&p_msgqueue->sem_get, OS_OPT_DEL_ALWAYS, &err);

  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_NONE:
      if (p_msgqueue->buf_dyn_alloc == DEF_TRUE) {
        free(p_msgqueue->buf);
      }
      if (p_msgqueue->obj_dyn_alloc == DEF_TRUE) {
        free(p_msgqueue);
      }
      return osOK;

    default:
      return osErrorResource;
  }
#else
  (void)mq_id;
  RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_NOT_SUPPORTED, osError);
  return osError;
#endif
}
