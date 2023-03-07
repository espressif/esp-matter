/***************************************************************************//**
 * @file
 * @brief Power Manager API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sl_power_manager.h"
#include "sl_power_manager_config.h"
#include "sli_power_manager_private.h"
#include "sli_power_manager.h"
#include "sli_sleeptimer.h"
#include "sl_assert.h"
#include "sl_atomic.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

#if ((SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1) \
  && (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 2) \
  && (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 3))
#error "Lowest Energy mode allowed is invalid."
#endif

// Default overhead value for the wake-up time used for the schedule wake-up
// functionality.
#define SCHEDULE_WAKEUP_DEFAULT_RESTORE_TIME_OVERHEAD_TICK  0

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

// Initialization flag.
static bool is_initialized = false;

// Current active energy mode.
static sl_power_manager_em_t current_em = SL_POWER_MANAGER_EM0;

// Events subscribers lists
static sl_slist_node_t *power_manager_em_transition_event_list = NULL;

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
// Store the sleeptimer module clock frequency for conversion calculation
static uint32_t sleeptimer_frequency;

// Table of energy modes counters. Each counter indicates the presence (not zero)
// or absence (zero) of requirements on a given energy mode. The table doesn't
// contain requirement on EM0.
static uint8_t requirement_em_table[SLI_POWER_MANAGER_EM_TABLE_SIZE] = {
  0,  // EM1 requirement counter
  0,  // EM2 requirement counter
};

// Counter variable to save the number of High Accuracy HF clock requirements requested.
uint8_t requirement_high_accuracy_hf_clock_counter = 0;

#ifdef SLI_DEVICE_SUPPORTS_EM1P
// Variable to indicate if the High Accuracy HF clock requirements count is back to zero.
bool requirement_high_accuracy_hf_clock_back_to_zero = false;
#endif

// Saved energy mode we are coming from when waiting for HFXO ready.
static sl_power_manager_em_t waiting_clock_restore_from_em = SL_POWER_MANAGER_EM0;

// Flag indicating if we are sleeping, waiting for the HF clock restore
static bool is_sleeping_waiting_for_clock_restore = false;

// Flag indicating if the system states (clocks) are saved and should be restored
static bool is_states_saved = false;

// Timer that it is used for enabling the clock for the scheduled wakeup
static sl_sleeptimer_timer_handle_t clock_wakeup_timer_handle = { 0 };

// Store if requirement on EM1 has been added before sleeping;
// i.e. only possible if sleeping for less than minimum off time
static bool requirement_on_em1_added = false;

// Threshold delay in sleeptimer ticks indicating the minimum time required
// to make the shut down of external high frequency oscillator worthwhile before
// the next synchronous high frequency oscillator requirement. Shorter than this
// delay, the power gain of shutting down is invalidated.
uint32_t high_frequency_min_offtime_tick = 0;

// Store the configuration overhead value in sleeptimer tick to add/remove to the wake-up time.
int32_t wakeup_time_config_overhead_tick = 0;

static bool is_hf_x_oscillator_not_preserved;

// Store if we are currently waiting for HF clock restoration to finish
static bool is_actively_waiting_for_clock_restore = false;

// Indicates if the clock restore was completed from the HFXO ISR
static bool is_restored_from_hfxo_isr = false;
static bool is_restored_from_hfxo_isr_internal = false;
#endif

/*
 *********************************************************************************************************
 *                                           HOOK REFERENCES
 *********************************************************************************************************
 */

bool sl_power_manager_sleep_on_isr_exit(void);

// Callback to application after wakeup but before restoring interrupts.
// For internal Silicon Labs use only
__WEAK void sli_power_manager_on_wakeup(void);

// Hook that can be used by the log outputer to suspend transmission of logs
// in case it would require energy mode changes while in the sleep loop.
__WEAK void sli_power_manager_suspend_log_transmission(void);

// Hook that can be used by the log outputer to resume transmission of logs.
__WEAK void sli_power_manager_resume_log_transmission(void);

// Callback to notify possible transition from EM1P to EM2.
// For internal Silicon Labs use only
#ifdef SLI_DEVICE_SUPPORTS_EM1P
__WEAK void sli_power_manager_em1p_to_em2_notification(void);
#endif

/***************************************************************************//**
 * Mandatory callback that allows to cancel sleeping action.
 ******************************************************************************/
bool sl_power_manager_is_ok_to_sleep(void);

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
static sl_power_manager_em_t get_lowest_em(void);

static void evaluate_wakeup(sl_power_manager_em_t to);

static void update_em1_requirement(bool add);

static void on_clock_wakeup_timeout(sl_sleeptimer_timer_handle_t *handle,
                                    void *data);

static void clock_restore_and_wait(void);

static void clock_restore(void);
#endif

static void power_manager_notify_em_transition(sl_power_manager_em_t from,
                                               sl_power_manager_em_t to);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize Power Manager module.
 ******************************************************************************/
sl_status_t sl_power_manager_init(void)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  if (!is_initialized) {
    sl_status_t status = SL_STATUS_OK;

    // Initialize Sleeptimer module in case not already done.
    status = sl_sleeptimer_init();
    if (status != SL_STATUS_OK) {
      CORE_EXIT_CRITICAL();
      return status;
    }

  #if (SL_POWER_MANAGER_DEBUG == 1)
    sli_power_manager_debug_init();
  #endif
    sl_slist_init(&power_manager_em_transition_event_list);

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
    // Set the default wake-up overhead value
    wakeup_time_config_overhead_tick = SCHEDULE_WAKEUP_DEFAULT_RESTORE_TIME_OVERHEAD_TICK;

    // Get the sleeptimer frequency
    sleeptimer_frequency = sl_sleeptimer_get_timer_frequency();
#endif
  }

  // Do all necessary hardware initialization.
  sli_power_manager_init_hardware();

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  // Set the HF minimum offtime in sleeptimer ticks
  high_frequency_min_offtime_tick = sli_power_manager_get_default_high_frequency_minimum_offtime();
#endif

  is_initialized = true;
  CORE_EXIT_CRITICAL();

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Sleep at the lowest allowed energy mode.
 ******************************************************************************/
void sl_power_manager_sleep(void)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();

  sli_power_manager_suspend_log_transmission();

  if (sl_power_manager_is_ok_to_sleep() != true) {
    sli_power_manager_resume_log_transmission();
    CORE_EXIT_CRITICAL();
    return;
  }

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  sl_power_manager_em_t lowest_em;

  // Go to another energy mode (same, higher to lower or lower to higher)
  do {
    // Remove any previous EM1 requirement added internally by the power manager itself
    if (requirement_on_em1_added) {
      update_em1_requirement(false);
      requirement_on_em1_added = false;
    }

    lowest_em = get_lowest_em();
    evaluate_wakeup(lowest_em);
    lowest_em = get_lowest_em();  // Reevaluate as a requirement can be added from evaluate_wakeup()

    if ((lowest_em >= SL_POWER_MANAGER_EM2)
        && (is_states_saved == false)) {
      sli_power_manager_save_states();
    }

    // Notify listeners if transition to another energy mode
    if (lowest_em != current_em) {
#ifdef SLI_DEVICE_SUPPORTS_EM1P
      requirement_high_accuracy_hf_clock_back_to_zero = false;
#endif
      if (is_sleeping_waiting_for_clock_restore == false) {
        // But only notify if we are not in the process of waiting for the HF oscillators restore.
        power_manager_notify_em_transition(current_em, lowest_em);
      }
      current_em = lowest_em;           // Keep new active energy mode
    }

#ifdef SLI_DEVICE_SUPPORTS_EM1P
    // Notification for possible transition from EM1P to EM2
    // For internal Silicon Labs use only
    if (requirement_high_accuracy_hf_clock_back_to_zero
        && current_em == SL_POWER_MANAGER_EM2) {
      requirement_high_accuracy_hf_clock_back_to_zero = false;
      sli_power_manager_em1p_to_em2_notification();
    }
#endif

    // Pre-sleep operations if any are necessary
    if ((lowest_em >= SL_POWER_MANAGER_EM2)
        && (is_states_saved == false)) {
      // Only do pre-sleep operations if there is no requirement on High Accuracy Clock.
      // Else we must not touch the clock tree.
      if (requirement_high_accuracy_hf_clock_counter == 0) {
        sli_power_manager_handle_pre_deepsleep_operations();
        is_hf_x_oscillator_not_preserved = true;
      }
      is_states_saved = true;
    }

    // Apply lowest reachable energy mode
    sli_power_manager_apply_em(current_em);

    // In case we are waiting for the restore from an early wake-up,
    // we put back the current EM to the one before the early wake-up to do the next notification correctly.
    if (is_sleeping_waiting_for_clock_restore == true) {
      current_em = waiting_clock_restore_from_em;
    }

    // Notify consumer of wakeup while interrupts are still off
    // For internal Silicon Labs use only
    sli_power_manager_on_wakeup();

    CORE_EXIT_CRITICAL();
    CORE_ENTER_CRITICAL();

    // In case the HF restore was completed from the HFXO ISR,
    // and notification not done elsewhere, do it here
    if (is_restored_from_hfxo_isr_internal == true) {
      is_restored_from_hfxo_isr_internal = false;
      if (current_em == waiting_clock_restore_from_em) {
        current_em = SL_POWER_MANAGER_EM1;
        power_manager_notify_em_transition(waiting_clock_restore_from_em, SL_POWER_MANAGER_EM1);
      }
    }

    // Stop the internal power manager sleeptimer.
    sl_sleeptimer_stop_timer(&clock_wakeup_timer_handle);
  } while (sl_power_manager_sleep_on_isr_exit() == true);

#ifdef SLI_DEVICE_SUPPORTS_EM1P
  requirement_high_accuracy_hf_clock_back_to_zero = false;
#endif

  if (is_states_saved == true) {
    is_sleeping_waiting_for_clock_restore = false;
    // Restore clocks
    if (is_hf_x_oscillator_not_preserved) {
      sli_power_manager_restore_high_freq_accuracy_clk();
      is_hf_x_oscillator_not_preserved = false;
    }
    // If possible, go back to sleep in EM1 while waiting for HF accuracy restore
    while (!sli_power_manager_is_high_freq_accuracy_clk_ready(false)) {
      sli_power_manager_apply_em(SL_POWER_MANAGER_EM1);
      CORE_EXIT_CRITICAL();
      CORE_ENTER_CRITICAL();
    }
    sli_power_manager_restore_states();
    is_states_saved = false;
  }

  evaluate_wakeup(SL_POWER_MANAGER_EM0);
#else
  current_em = SL_POWER_MANAGER_EM1;

  // Notify listeners of transition to EM1
  power_manager_notify_em_transition(SL_POWER_MANAGER_EM0, SL_POWER_MANAGER_EM1);
  do {
    // Apply EM1 energy mode
    sli_power_manager_apply_em(SL_POWER_MANAGER_EM1);

    CORE_EXIT_CRITICAL();
    CORE_ENTER_CRITICAL();
  } while (sl_power_manager_sleep_on_isr_exit() == true);
#endif

  // Indicate back to EM0
  power_manager_notify_em_transition(current_em, SL_POWER_MANAGER_EM0);
  current_em = SL_POWER_MANAGER_EM0;

  sli_power_manager_resume_log_transmission();

  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * Updates requirement on the given energy mode.
 *
 * @param   em    Energy mode. Possible values are:
 *                SL_POWER_MANAGER_EM1
 *                SL_POWER_MANAGER_EM2
 *
 * @param   add   Flag indicating if requirement is added (true) or removed
 *                (false).
 *
 * @note Need to be call inside a critical section.
 *
 * @note This function will do nothing when SL_POWER_MANAGER_LOWEST_EM_ALLOWED
 *       config is set to EM1.
 ******************************************************************************/
void sli_power_manager_update_em_requirement(sl_power_manager_em_t em,
                                             bool add)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  // EM0 is not allowed
  EFM_ASSERT((em > SL_POWER_MANAGER_EM0) && (em < SL_POWER_MANAGER_EM3));

  // Cannot increment above 255 (wraparound not allowed)
  EFM_ASSERT(!((requirement_em_table[em - 1] == UINT8_MAX) && (add == true)));
  // Cannot decrement below 0 (wraparound not allowed)
  EFM_ASSERT(!((requirement_em_table[em - 1] == 0) && (add == false)));

  // Increment (add) or decrement (remove) energy mode counter.
  requirement_em_table[em - 1] += (add) ? 1 : -1;

  if (add == true
      && current_em >= SL_POWER_MANAGER_EM2) {  // if currently sleeping at a level that can require a clock restore; i.e. called from ISR
    sl_power_manager_em_t lowest_em;
    // If requirement added when sleeping, restore the clock before continuing the processing.
    // Retrieve lowest reachable energy mode
    lowest_em = get_lowest_em();

    if (lowest_em <= SL_POWER_MANAGER_EM1) {
      // If new lowest requirement is greater than the current
      // Restore clock; Everything is restored (HF and LF Clocks), the sleep loop will
      // shutdown the clocks when returning sleeping
      clock_restore_and_wait();
    } else if (current_em == SL_POWER_MANAGER_EM3
               && lowest_em == SL_POWER_MANAGER_EM2) {
      // Restore LF clocks if we are transitioning from EM3 to EM2
      sli_power_manager_low_frequency_restore();
    }

    if (current_em != lowest_em) {
      power_manager_notify_em_transition(current_em, lowest_em);
      current_em = lowest_em;           // Keep new active energy mode
    }
  }
#else
  (void)em;
  (void)add;
#endif
}

#if defined(SLI_DEVICE_SUPPORTS_EM1P)
/***************************************************************************//**
 * Updates requirement on preservation of High Frequency Clocks settings.
 *
 * @param   add   Flag indicating if requirement is added (true) or removed
 *                (false).
 ******************************************************************************/
void sli_power_manager_update_hf_clock_settings_preservation_requirement(bool add)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  // Cannot increment above 255 (wraparound not allowed)
  EFM_ASSERT(!((requirement_high_accuracy_hf_clock_counter == UINT8_MAX) && (add == true)));
  // Cannot decrement below 0 (wraparound not allowed)
  EFM_ASSERT(!((requirement_high_accuracy_hf_clock_counter == 0) && (add == false)));

  // Cannot add requirement if the "normal" clock settings are not currently applied
  EFM_ASSERT(!((current_em > SL_POWER_MANAGER_EM2) && (add == true)));

  // Increment (add) or decrement (remove) energy mode counter.
  requirement_high_accuracy_hf_clock_counter += (add) ? 1 : -1;

  // Save if the requirement is back to zero.
  requirement_high_accuracy_hf_clock_back_to_zero = (requirement_high_accuracy_hf_clock_counter == 0) ? true : false;

  CORE_EXIT_CRITICAL();
#else
  (void)add;
#endif
}
#endif

/***************************************************************************//**
 * Gets the wake-up restore process time.
 * If we are not in the context of a deepsleep and therefore don't need to
 * do a restore, the return value is 0.
 *
 * @return   Wake-up restore process time.
 ******************************************************************************/
uint32_t sli_power_manager_get_restore_delay(void)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  uint32_t wakeup_delay = 0;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();

  // If we are not currently in deepsleep, not need for any clock restore
  if (current_em <= SL_POWER_MANAGER_EM1) {
    CORE_EXIT_CRITICAL();
    return wakeup_delay;
  }

  // Get the clock restore delay
  wakeup_delay = sl_power_manager_schedule_wakeup_get_restore_overhead_tick();
  wakeup_delay += sli_power_manager_get_wakeup_process_time_overhead();

  CORE_EXIT_CRITICAL();

  return wakeup_delay;
#else
  return 0;
#endif
}

/***************************************************************************//**
 * Initiates the wake-up restore process.
 ******************************************************************************/
void sli_power_manager_initiate_restore(void)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();

  // Start restore process
  clock_restore();

  CORE_EXIT_CRITICAL();
#endif
}

/***************************************************************************//**
 * Registers a callback to be called on given Energy Mode transition(s).
 *
 * @note Adding/Removing requirement(s) from the callback is not supported.
 ******************************************************************************/
void sl_power_manager_subscribe_em_transition_event(sl_power_manager_em_transition_event_handle_t *event_handle,
                                                    const sl_power_manager_em_transition_event_info_t *event_info)
{
  CORE_DECLARE_IRQ_STATE;

  event_handle->info = (sl_power_manager_em_transition_event_info_t *)event_info;
  CORE_ENTER_CRITICAL();
  sl_slist_push(&power_manager_em_transition_event_list, &event_handle->node);
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * Unregisters an event callback handle on Energy mode transition.
 ******************************************************************************/
void sl_power_manager_unsubscribe_em_transition_event(sl_power_manager_em_transition_event_handle_t *event_handle)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  sl_slist_remove(&power_manager_em_transition_event_list, &event_handle->node);
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * Get configurable overhead value for early restore time in Sleeptimer ticks
 * when a schedule wake-up is set.
 *
 * @return  Current overhead value for early wake-up time.
 *
 * @note This function will return 0 in case SL_POWER_MANAGER_LOWEST_EM_ALLOWED
 *       config is set to EM1.
 ******************************************************************************/
int32_t sl_power_manager_schedule_wakeup_get_restore_overhead_tick(void)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  int32_t overhead_tick;

  sl_atomic_load(overhead_tick, wakeup_time_config_overhead_tick);
  return overhead_tick;
#else
  return 0;
#endif
}

/***************************************************************************//**
 * Set configurable overhead value for early restore time in Sleeptimer ticks
 * used for schedule wake-up.
 * Must be called after initialization else the value will be overwritten.
 *
 * @param overhead_tick Overhead value to set for early restore time.
 *
 * @note The overhead value can also be negative to remove time from the restore
 *       process.
 *
 * @note This function will do nothing when SL_POWER_MANAGER_LOWEST_EM_ALLOWED
 *       config is set to EM1.
 ******************************************************************************/
void sl_power_manager_schedule_wakeup_set_restore_overhead_tick(int32_t overhead_tick)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  sl_atomic_store(wakeup_time_config_overhead_tick, overhead_tick);
#else
  (void)overhead_tick;
#endif
}

/***************************************************************************//**
 * Get configurable minimum off-time value for schedule wake-up in Sleeptimer
 * ticks.
 *
 * @return  Current minimum off-time value for schedule wake-up.
 *
 * @note  Turning on external high frequency oscillator, such as HFXO, requires
 *        more energy since we must supply higher current for the wake-up.
 *        Therefore, when an 'external high frequency oscillator enable' is
 *        scheduled in 'x' time, there is a threshold 'x' value where turning
 *        off the oscillator is not worthwhile since the energy consumed by
 *        taking into account the wake-up will be greater than if we just keep
 *        the oscillator on until the next scheduled oscillator enabled. This
 *        threshold value is what we refer as the minimum off-time.
 *
 * @note This function will return 0 in case SL_POWER_MANAGER_LOWEST_EM_ALLOWED
 *       config is set to EM1.
 ******************************************************************************/
uint32_t sl_power_manager_schedule_wakeup_get_minimum_offtime_tick(void)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  uint32_t offtime_tick;

  sl_atomic_load(offtime_tick, high_frequency_min_offtime_tick);
  return offtime_tick;
#else
  return 0;
#endif
}

/***************************************************************************//**
 * Set configurable minimum off-time value for schedule wake-up in Sleeptimer
 * ticks.
 *
 * @param minimum_offtime_tick  minimum off-time value to set for schedule
 *                              wake-up.
 *
 * @note  Turning on external high frequency oscillator, such as HFXO, requires
 *        more energy since we must supply higher current for the wake-up.
 *        Therefore, when an 'external high frequency oscillator enable' is
 *        scheduled in 'x' time, there is a threshold 'x' value where turning
 *        off the oscillator is not worthwhile since the energy consumed by
 *        taking into account the wake-up will be greater than if we just keep
 *        the oscillator on until the next scheduled oscillator enabled. This
 *        threshold value is what we refer as the minimum off-time.
 *
 * @note This function will do nothing when SL_POWER_MANAGER_LOWEST_EM_ALLOWED
 *       config is set to EM1.
 ******************************************************************************/
void sl_power_manager_schedule_wakeup_set_minimum_offtime_tick(uint32_t minimum_offtime_tick)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  sl_atomic_store(high_frequency_min_offtime_tick, minimum_offtime_tick);
#else
  (void)minimum_offtime_tick;
#endif
}

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
/*******************************************************************************
 * Converts microseconds time in sleeptimer ticks.
 ******************************************************************************/
uint32_t sli_power_manager_convert_delay_us_to_tick(uint32_t time_us)
{
  return (((time_us * sleeptimer_frequency) + (1000000 - 1)) / 1000000);
}
#endif

/***************************************************************************//**
 * Last-chance check before sleep.
 *
 * @return  True, if the system should actually sleep.
 *          False, if not.
 *
 * @note This is the fallback implementation of the callback, it can be
 *       overridden by the application or other components.
 ******************************************************************************/
__WEAK bool sl_power_manager_is_ok_to_sleep(void)
{
  return true;
}

/***************************************************************************//**
 * Check if the MCU can sleep after an interrupt.
 *
 * @return  True, if the system can sleep after the interrupt.
 *          False, otherwise.
 *
 * @note This is the fallback implementation of the callback, it can be
 *       overridden by the application or other components.
 ******************************************************************************/
__WEAK bool sl_power_manager_sleep_on_isr_exit(void)
{
  return false;
}

/**************************************************************************//**
 * Determines if the HFXO interrupt was part of the last wake-up and/or if
 * the HFXO early wakeup expired during the last ISR
 * and if it was the only timer to expire in that period.
 *
 * @return true if power manager sleep can return to sleep,
 *         false otherwise.
 *
 * @note This function will always return false in case
 *       SL_POWER_MANAGER_LOWEST_EM_ALLOWED config is set to EM1, since we will
 *       never sleep at a lower level than EM1.
 *****************************************************************************/
bool sl_power_manager_is_latest_wakeup_internal(void)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  CORE_DECLARE_IRQ_STATE;
  bool sleep;

  CORE_ENTER_CRITICAL();
  sleep = is_restored_from_hfxo_isr;
  is_restored_from_hfxo_isr = false;
  CORE_EXIT_CRITICAL();

  sleep |= sl_sleeptimer_is_power_manager_early_restore_timer_latest_to_expire();
  return sleep;
#else
  return false;
#endif
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
/***************************************************************************//**
 * Get lowest energy mode to apply given the requirements on the different
 * energy modes.
 *
 * @return  Lowest energy mode: EM1, EM2 or EM3.
 *
 * @note If no requirement for any energy mode (EM1 and EM2), lowest energy mode
 * is EM3.
 ******************************************************************************/
static sl_power_manager_em_t get_lowest_em(void)
{
  uint32_t em_ix;
  sl_power_manager_em_t em;

  // Retrieve lowest Energy mode allowed given the requirements
  for (em_ix = 1; (em_ix < SL_POWER_MANAGER_LOWEST_EM_ALLOWED) && (requirement_em_table[em_ix - 1] == 0); em_ix++) {
    ;
  }

  em = (sl_power_manager_em_t)em_ix;

  return em;
}
#endif

/***************************************************************************//**
 * Notify subscribers about energy mode transition.
 *
 * @param  from  Energy mode from which CPU comes from.
 *
 * @param  to    Energy mode to which CPU is going to.
 ******************************************************************************/
static void power_manager_notify_em_transition(sl_power_manager_em_t from,
                                               sl_power_manager_em_t to)
{
  sl_power_manager_em_transition_event_handle_t *handle;
  sl_power_manager_em_transition_event_t transition = 0;

  switch (to) {
    case SL_POWER_MANAGER_EM0:
      transition = SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM0;
      break;

    case SL_POWER_MANAGER_EM1:
      transition = SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM1;
      break;

    case SL_POWER_MANAGER_EM2:
      transition = SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2;
      break;

    case SL_POWER_MANAGER_EM3:
      transition = SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM3;
      break;

    default:
      EFM_ASSERT(0);
  }

  switch (from) {
    case SL_POWER_MANAGER_EM0:
      transition |= SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0;
      break;

    case SL_POWER_MANAGER_EM1:
      transition |= SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM1;
      break;

    case SL_POWER_MANAGER_EM2:
      transition |= SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2;
      break;

    case SL_POWER_MANAGER_EM3:
      transition |= SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM3;
      break;

    default:
      EFM_ASSERT(0);
  }

  SL_SLIST_FOR_EACH_ENTRY(power_manager_em_transition_event_list, handle, sl_power_manager_em_transition_event_handle_t, node) {
    if ((handle->info->event_mask & transition) > 0) {
      handle->info->on_event(from, to);
    }
  }
}

/***************************************************************************//**
 * Evaluates scheduled wakeup and restart timer based on the wakeup time.
 * If the remaining time is shorter than the wakeup time then add a requirement
 * on EM1 for avoiding the wakeup delay time.
 *
 * @note Must be called in a critical section.
 ******************************************************************************/
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
static void evaluate_wakeup(sl_power_manager_em_t to)
{
  sl_status_t status;
  uint32_t tick_remaining;

  switch (to) {
    case SL_POWER_MANAGER_EM0:
      // Coming back from Sleep.
      if (requirement_on_em1_added) {
        update_em1_requirement(false);
        requirement_on_em1_added = false;
      }
      break;

    case SL_POWER_MANAGER_EM1:
      // External high frequency clock, such as HFXO, already enabled; No wakeup delay
      break;

    case SL_POWER_MANAGER_EM2:
    case SL_POWER_MANAGER_EM3:
      // Get the time remaining until the next sleeptimer requiring early wake-up
      status = sl_sleeptimer_get_remaining_time_of_first_timer(0, &tick_remaining);
      if (status == SL_STATUS_OK) {
        if (tick_remaining <= high_frequency_min_offtime_tick) {
          // Add EM1 requirement if time remaining is to short to be energy efficient
          // if going back to deepsleep.
          update_em1_requirement(true);
          requirement_on_em1_added = true;
        } else {
          int32_t wakeup_delay = 0;
          int32_t cfg_overhead_tick = 0;

          // Calculate overall wake-up delay.
          sl_atomic_load(cfg_overhead_tick, wakeup_time_config_overhead_tick);
          wakeup_delay += cfg_overhead_tick;
          wakeup_delay += sli_power_manager_get_wakeup_process_time_overhead();
          EFM_ASSERT(wakeup_delay >= 0);
          if (tick_remaining <= (uint32_t)wakeup_delay) {
            // Add EM1 requirement if time remaining is smaller than wake-up delay.
            update_em1_requirement(true);
            requirement_on_em1_added = true;
          } else {
            // Start internal sleeptimer to do the early wake-up.
            sl_sleeptimer_restart_timer(&clock_wakeup_timer_handle,
                                        (tick_remaining - (uint32_t)wakeup_delay),
                                        on_clock_wakeup_timeout,
                                        NULL,
                                        0,
                                        SLI_SLEEPTIMER_POWER_MANAGER_EARLY_WAKEUP_TIMER_FLAG);
          }
        }
      }
      break;

    default:
      EFM_ASSERT(false);
  }
}
#endif

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
/***************************************************************************//**
 * Updates internal EM1 requirement.
 * We add an internal EM1 requirement when we would usually go into EM2/EM3
 * but there is not enough time before the next schedule event requiring a
 * clock restore. So we just go to sleep in EM1.
 * We remove this internal EM1 requirement next time we wake-up.
 *
 * @param   add  true, to add EM1 requirement,
 *               false, to remove EM1 requirement.
 *
 * @note For internal use only.
 *
 * @note Need to be call inside a critical section.
 ******************************************************************************/
static void update_em1_requirement(bool add)
{
  // Cannot increment above 255 (wraparound not allowed)
  EFM_ASSERT(!((requirement_em_table[SL_POWER_MANAGER_EM1 - 1] == UINT8_MAX) && (add == true)));
  // Cannot decrement below 0 (wraparound not allowed)
  EFM_ASSERT(!((requirement_em_table[SL_POWER_MANAGER_EM1 - 1] == 0) && (add == false)));

#if (SL_POWER_MANAGER_DEBUG == 1)
  sli_power_manager_debug_log_em_requirement(SL_POWER_MANAGER_EM1, add, "PM_INTERNAL_EM1_REQUIREMENT");
#endif

  // Increment (add) or decrement (remove) energy mode counter.
  requirement_em_table[SL_POWER_MANAGER_EM1 - 1] += (add) ? 1 : -1;

  // In rare occasions a clock restore must be started here:
  // - An asynchronous event wake-up the system from deepsleep very near the early wake-up event,
  //   When we re-enter the sleep loop, we delete the internal early wake-up timer, but during
  //   the evaluation before sleep, it is calculated that not enough time is remains to go to
  //   deepsleep. In that case, since we deleted the early wake-up timer we must start the
  //   restore process here.
  // - A synchronous event is added during an ISR, when we evaluate if the timeout is bigger
  //   than the clock restore time, it's barely bigger, so no clock restore process is started
  //   at that time. But when we do the evaluate before sleep, the remaining time is now smaller
  //   than the clock restore delay. So me must start the restore process here.
  if (add == true
      && current_em >= SL_POWER_MANAGER_EM2
      && is_sleeping_waiting_for_clock_restore == false) {
    clock_restore();
  }
}
#endif

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
/***************************************************************************//**
 * Do clock restore process and wait for it to be completed.
 ******************************************************************************/
static void clock_restore_and_wait(void)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  if (is_states_saved == true) {
    if (is_actively_waiting_for_clock_restore == false) {
      is_actively_waiting_for_clock_restore = true;

      // Since we will actively wait for clock restore, we cancel any current non-active wait.
      is_sleeping_waiting_for_clock_restore = false;
    }

    if (is_hf_x_oscillator_not_preserved) {
      sli_power_manager_restore_high_freq_accuracy_clk();
      is_hf_x_oscillator_not_preserved = false;
    }

    CORE_EXIT_CRITICAL();
    // We remove the critical section in case HFXO fails to startup and the HFXO Interrupt needs to run to handle the error.
    sli_power_manager_is_high_freq_accuracy_clk_ready(true);
    CORE_ENTER_CRITICAL();
    if (is_actively_waiting_for_clock_restore) {
      sli_power_manager_restore_states();
      is_actively_waiting_for_clock_restore = false;
    }

    is_states_saved = false;
  }
  CORE_EXIT_CRITICAL();
}
#endif

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
/***************************************************************************//**
 * Start clock restore process.
 *
 * @note Need to be call inside a critical section.
 ******************************************************************************/
static void clock_restore(void)
{
  // Check if we need to start the clock restore process
  if (is_states_saved == true) {
    if (is_hf_x_oscillator_not_preserved) {
      sli_power_manager_restore_high_freq_accuracy_clk();
      is_hf_x_oscillator_not_preserved = false;
    }
    if (sli_power_manager_is_high_freq_accuracy_clk_ready(false)) {
      // Do the clock restore if the HF oscillator is already ready
      sli_power_manager_restore_states();
      is_states_saved = false;

      // We do the notification only when the restore is completed.
      power_manager_notify_em_transition(current_em, SL_POWER_MANAGER_EM1);
      current_em = SL_POWER_MANAGER_EM1; // Keep new active energy mode
    } else {
      // If the HF oscillator is not yet ready, we will go back to sleep while waiting
      is_sleeping_waiting_for_clock_restore = true;

      // Save current EM to do the right notification later
      waiting_clock_restore_from_em = current_em;
    }
  }
}
#endif

#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
/***************************************************************************//**
 * Callback for clock enable timer.
 *
 * @param   handle  Pointer to sleeptimer handle
 *
 * @param   data    Pointer to callback data
 *
 * @note We restore the HF clocks and go to EM1 here to be ready in time for the
 *       Application sleeptimer callback. But no EM1 requirement is added
 *       here. Since the time until the Application sleeptimer times out is <=
 *       than the wake-up delay, it protects us from going back to sleep lower
 *       than EM1. After that, it's up to the Application sleeptimer callback to
 *       put a EM1 requirement if still needed.
 ******************************************************************************/
static void on_clock_wakeup_timeout(sl_sleeptimer_timer_handle_t *handle,
                                    void *data)
{
  (void)handle;
  (void)data;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();

  if (is_actively_waiting_for_clock_restore) {
    // In case we are already actively waiting for HFXO ready in another ISR, just exit
    CORE_EXIT_CRITICAL();
    return;
  }

  // If needed start the clock restore process
  clock_restore();

  CORE_EXIT_CRITICAL();
}
#endif

/***************************************************************************//**
 * HFXO ready notification callback for internal use with power manager
 *
 * @note Will only be used on series 2 devices when HFXO Manager is present.
 ******************************************************************************/
void sli_hfxo_manager_notify_ready_for_power_manager(void)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  // Complete HF restore and change current Energy mode
  // The notification will be done once back in the sleep loop
  if (current_em != SL_POWER_MANAGER_EM0
      && is_sleeping_waiting_for_clock_restore == true) {
    sli_power_manager_restore_states();
    is_sleeping_waiting_for_clock_restore = false;
    is_states_saved = false;
    is_restored_from_hfxo_isr = true;
    is_restored_from_hfxo_isr_internal = true;
  }
#endif
}

#if defined(EMU_VSCALE_PRESENT)
/***************************************************************************//**
 * Enable or disable fast wake-up in EM2 and EM3
 *
 * @note Will also update the wake up time from EM2 to EM0.
 *
 * @note This function will do nothing when SL_POWER_MANAGER_LOWEST_EM_ALLOWED
 *       config is set to EM1.
 ******************************************************************************/
void sl_power_manager_em23_voltage_scaling_enable_fast_wakeup(bool enable)
{
#if (SL_POWER_MANAGER_LOWEST_EM_ALLOWED != 1)
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();

  sli_power_manager_em23_voltage_scaling_enable_fast_wakeup(enable);

  CORE_EXIT_CRITICAL();
#else
  (void)enable;
#endif
}
#endif
