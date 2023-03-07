/***************************************************************************//**
 * @file
 * @brief FreeRTOS Tick and Sleep port.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/* Compiler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "sl_sleeptimer.h"
#include "sl_atomic.h"
#include "sl_power_manager.h"

#if configUSE_TICKLESS_IDLE == 0
#error "This port requires configUSE_TICKLESS_IDLE to be enabled."
#endif

/* Local variables */
/* Number of lf ticks per OS ticks. */
static uint32_t lfticks_per_os_ticks = 0;

/* Tick count (in lf ticks) last time the tick count was updated in FreeRTOS. */
static uint32_t last_update_lftick = 0;

/* Indicates if system is sleeping. */
static bool is_sleeping = false;

/* Maximum number of os ticks the system can sleep per call. */
static TickType_t max_sleep_os_ticks = 0;

/* Expected sleep ticks */
static TickType_t expected_sleep_ticks = 0;

/* Total ticks slept */
static TickType_t total_slept_os_ticks = 0;

/* Handle to schedule wakeup timer. */
static sl_sleeptimer_timer_handle_t schedule_wakeup_timer_handle;

/* Local functions */
static void sli_schedule_wakeup_timer_expire_handler(sl_sleeptimer_timer_handle_t *handle,
                                                     void *data);

static void sli_os_schedule_wakeup(TickType_t os_ticks);

/***************************************************************************//**
 * Sets up sleeptimer timer for constant ticking.
 ******************************************************************************/
void vPortSetupTimerInterrupt( void )
{
  uint32_t sleeptimer_freq;

  /* FreeRTOS requires a high SVC priority when starting the scheduler */
  NVIC_SetPriority(SVCall_IRQn, 0);

  sleeptimer_freq = sl_sleeptimer_get_timer_frequency();
  configASSERT( configTICK_RATE_HZ <= sleeptimer_freq );

  lfticks_per_os_ticks = (sleeptimer_freq + (configTICK_RATE_HZ - 1)) / configTICK_RATE_HZ;
  max_sleep_os_ticks   = (0xFFFFFFFF / lfticks_per_os_ticks) - 10;

  last_update_lftick = sl_sleeptimer_get_tick_count();

  /* Schedule a wakeup in one tick. */
  sli_os_schedule_wakeup(1);
}

/***************************************************************************//**
 * Stop constant ticking and wake the system up after specified idle time.
 *
 * @param xExpectedIdleTime Time in os ticks that the system is expected to
 *                          sleep.
 ******************************************************************************/
SL_WEAK void sli_iot_power_set_expected_idle(TickType_t expected_idle)
{
  (void)expected_idle;
  return;
}

void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
  sl_atomic_store(is_sleeping, true);

  /* Schedule a wakeup for expected idle end time. */
  sl_sleeptimer_stop_timer(&schedule_wakeup_timer_handle);
  sli_os_schedule_wakeup(xExpectedIdleTime);

  configPRE_SLEEP_PROCESSING( xExpectedIdleTime );

  sli_iot_power_set_expected_idle(xExpectedIdleTime);

  expected_sleep_ticks = xExpectedIdleTime;
  total_slept_os_ticks = 0;

  sl_power_manager_sleep();

  configPOST_SLEEP_PROCESSING( total_slept_os_ticks );

  sl_atomic_store(is_sleeping, false);

  /* Schedule a wakeup in one tick. */
  sl_sleeptimer_stop_timer(&schedule_wakeup_timer_handle);
  sli_os_schedule_wakeup(1);
}

/***************************************************************************//**
 * Function called when schedule wakeup timer expires.
 ******************************************************************************/
static void sli_schedule_wakeup_timer_expire_handler(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  uint32_t current_tick_count = sl_sleeptimer_get_tick_count();

  (void)handle;
  (void)data;

  uint32_t originalMask = portSET_INTERRUPT_MASK_FROM_ISR();
  /* If system was not sleeping, update lfticks counter. */
  if (!is_sleeping) {
    bool sched = false;

    /* Increment the RTOS tick. */
    while ((current_tick_count - last_update_lftick) > lfticks_per_os_ticks) {
      sched |= xTaskIncrementTick();
      last_update_lftick+= lfticks_per_os_ticks;
    }

    if( sched != pdFALSE ) {
      /* A context switch is required.  Context switching is performed in
      the PendSV interrupt.  Pend the PendSV interrupt. */
      portYIELD();
    }

    sli_os_schedule_wakeup(1);
  }
  portCLEAR_INTERRUPT_MASK_FROM_ISR(originalMask);
}

/***************************************************************************//**
 * (Re)-start schedule wakeup timer with delay specified.
 *
 * @param os_ticks Delay, in os ticks, before next wakeup/tick.
 ******************************************************************************/
static void sli_os_schedule_wakeup(TickType_t os_ticks)
{
  sl_status_t status;
  uint32_t lf_ticks_to_sleep;
  TickType_t os_ticks_to_sleep;
  uint32_t current_tick_count = sl_sleeptimer_get_tick_count();

  /* Compute number of lfticks to sleep. */
  os_ticks_to_sleep = (os_ticks <= max_sleep_os_ticks) ? os_ticks : max_sleep_os_ticks;

  /* This function implements a correction mechanism that corrects any drift that can */
  /* occur between the sleep timer time and the tick count in FreeRTOS. */
  lf_ticks_to_sleep = os_ticks_to_sleep * lfticks_per_os_ticks;
  if (lf_ticks_to_sleep <= (current_tick_count - last_update_lftick)) {
    lf_ticks_to_sleep = 1;
  } else {
    lf_ticks_to_sleep -= (current_tick_count - last_update_lftick);
  }

  status = sl_sleeptimer_start_timer(&schedule_wakeup_timer_handle,
                                     lf_ticks_to_sleep,
                                     sli_schedule_wakeup_timer_expire_handler,
                                     0,
                                     0,
                                     0);
  configASSERT( status == SL_STATUS_OK );

#if (configASSERT_DEFINED == 0)
  (void)status;
#endif
}

/***************************************************************************//**
 * Function called by power manager to ensure that system is ok to sleep.
 ******************************************************************************/
SL_WEAK bool sli_iot_power_ok_to_sleep(void)
{
  return true;
}

bool sl_power_manager_is_ok_to_sleep()
{
  return sli_iot_power_ok_to_sleep() && (eTaskConfirmSleepModeStatus() != eAbortSleep);
}

/***************************************************************************//**
 * Function called by power manager to determine if system can go back to sleep
 * after a wakeup.
 *
 * @note Function is called in critical section
 ******************************************************************************/
bool sl_power_manager_sleep_on_isr_exit()
{
  uint32_t slept_lf_ticks;
  uint32_t slept_os_ticks;

  /* Determine how long we slept. */
  slept_lf_ticks = sl_sleeptimer_get_tick_count() - last_update_lftick;
  slept_os_ticks = slept_lf_ticks / lfticks_per_os_ticks;
  last_update_lftick += slept_os_ticks * lfticks_per_os_ticks;

  /* Notify FreeRTOS of how long we slept. */
  if ((total_slept_os_ticks + slept_os_ticks) < expected_sleep_ticks) {
    vTaskStepTick(slept_os_ticks);
    total_slept_os_ticks += slept_os_ticks;
  } else {
    vTaskStepTick(expected_sleep_ticks - total_slept_os_ticks);
    total_slept_os_ticks = expected_sleep_ticks;
  }

  /* Have we slept enough ? */
  if (total_slept_os_ticks >= expected_sleep_ticks) {
    return false;
  }

  /* Check if we can sleep again */
  return (eTaskConfirmSleepModeStatus() != eAbortSleep);
}
