/***************************************************************************//**
 * @file
 * @brief Simple timer service
 *
 * Simple timer service for applications with less strict timing requirements.
 * This module is based on the sleeptimer, but runs the timer callbacks in
 * non-interrupt context. This behavior gives more flexibility for the callback
 * implementation but causes a less precise timing.
 *
 * @note If your application requires precise timing, please use the sleeptimer
 *       directly.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_simple_timer.h"
#include "em_core.h"

// -----------------------------------------------------------------------------
// Definitions

#define LONG_TIMER_CHECK(timer) (0 != timer->overflow_max)

// -----------------------------------------------------------------------------
// Private variables

/// Number of the triggered timers.
static uint32_t trigger_count = 0;

/// Start of the linked list which contains the active timers.
static sl_simple_timer_t *simple_timer_head = NULL;

// -----------------------------------------------------------------------------
// Private function declarations

/***************************************************************************//**
 * Common callback for the sleeptimers.
 *
 * @param[in] handle Pointer to the sleeptimer handle.
 * @param[in] data Pointer to the sleeptimer's parent simple timer.
 *
 * @note This function runs in interrupt context.
 ******************************************************************************/
static void simple_timer_callback(sl_sleeptimer_timer_handle_t *handle,
                                  void *data);

/***************************************************************************//**
 * Append a timer to the end of the linked list.
 *
 * @param[in] timer Pointer to the timer handle.
 *
 * @pre Assumes that the timer is not present in the list.
 ******************************************************************************/
static void append_simple_timer(sl_simple_timer_t *timer);

/***************************************************************************//**
 * Remove a timer from the linked list.
 *
 * @param[in] timer Pointer to the timer handle.
 *
 * @return Presence of the timer in the linked list.
 * @retval true  Timer was in the list.
 * @retval false Timer was not found in the list.
 ******************************************************************************/
static bool remove_simple_timer(sl_simple_timer_t *timer);

// -----------------------------------------------------------------------------
// Public function definitions

sl_status_t sl_simple_timer_start(sl_simple_timer_t *timer,
                                  uint32_t timeout_ms,
                                  sl_simple_timer_callback_t callback,
                                  void *callback_data,
                                  bool is_periodic)
{
  sl_status_t sc;
  uint32_t timeout_initial_tick;
  uint32_t timer_freq;
  uint64_t required_tick;

  // Check input parameters.
  if ((timeout_ms == 0) && is_periodic) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Make sure that timer is stopped, also check for NULL.
  sc = sl_simple_timer_stop(timer);
  if (SL_STATUS_OK != sc) {
    return sc;
  }

  timer->triggered = false;
  timer->overflow_counter = 0;
  timer->overflow_max = 0;

  // Check if the timer has to be a long one
  if (timeout_ms > sl_sleeptimer_get_max_ms32_conversion()) {
    // Calculate the required ticks
    timer_freq = sl_sleeptimer_get_timer_frequency();
    required_tick = ((uint64_t)timeout_ms * (uint64_t)timer_freq + 999)
                    / 1000;

    // Calculate the initial time in ticks for the first run and the number of
    // maximum-time runs
    timeout_initial_tick = (uint32_t)(required_tick % UINT32_MAX);
    timer->overflow_max = (uint16_t)(required_tick / UINT32_MAX);

    // Start the timer with the initial time
    sc = sl_sleeptimer_start_periodic_timer(&timer->sleeptimer_handle,
                                            timeout_initial_tick,
                                            simple_timer_callback,
                                            (void*)timer,
                                            0,
                                            0);
  } else {
    // Start sleeptimer with the given timeout/period.
    if (is_periodic) {
      sc = sl_sleeptimer_start_periodic_timer_ms(
        &timer->sleeptimer_handle,
        timeout_ms,
        simple_timer_callback,
        (void*)timer,
        0,
        0);
    } else {
      sc = sl_sleeptimer_start_timer_ms(
        &timer->sleeptimer_handle,
        timeout_ms,
        simple_timer_callback,
        (void*)timer,
        0,
        0);
    }
  }

  if (SL_STATUS_OK == sc) {
    timer->callback = callback;
    timer->callback_data = callback_data;
    timer->periodic = is_periodic;
    timer->timeout_ms = timeout_ms;
    append_simple_timer(timer);
  }
  return sc;
}

sl_status_t sl_simple_timer_stop(sl_simple_timer_t *timer)
{
  bool timer_present;

  if (timer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // Stop sleeptimer, ignore error code if was not running.
  (void)sl_sleeptimer_stop_timer(&timer->sleeptimer_handle);

  timer_present = remove_simple_timer(timer);
  if (timer_present && timer->triggered) {
    // Timer has been triggered but not served yet.
    CORE_ATOMIC_SECTION(--trigger_count; )
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Execute timer callback functions.
 ******************************************************************************/
void sli_simple_timer_step(void)
{
  if (trigger_count > 0) {
    sl_simple_timer_t *timer = simple_timer_head;
    // Find triggered timers in list and call their callbacks.
    while (timer != NULL) {
      if (timer->triggered) {
        CORE_ATOMIC_SECTION(
          timer->triggered = false;
          --trigger_count;
          )
        if (!timer->periodic) {
          (void)remove_simple_timer(timer);
        }
        timer->callback(timer, timer->callback_data);
      }
      timer = timer->next;
    }
  }
}

/***************************************************************************//**
 * Routine for power manager handler
 ******************************************************************************/
sl_power_manager_on_isr_exit_t sli_simple_timer_sleep_on_isr_exit(void)
{
  sl_power_manager_on_isr_exit_t ret = SL_POWER_MANAGER_IGNORE;
  // if there is a triggered event, wake up to handle it
  if (trigger_count > 0) {
    ret = SL_POWER_MANAGER_WAKEUP;
  }
  return ret;
}

/***************************************************************************//**
 * Checks if it is ok to sleep now
 ******************************************************************************/
bool sli_simple_timer_is_ok_to_sleep(void)
{
  bool ret = true;
  // if there is a triggered event, do not go to sleep
  if (trigger_count > 0) {
    ret = false;
  }
  return ret;
}

// -----------------------------------------------------------------------------
// Private function definitions

static void simple_timer_callback(sl_sleeptimer_timer_handle_t *handle,
                                  void *data)
{
  (void)handle;
  sl_simple_timer_t *timer = (sl_simple_timer_t*)data;
  if (timer->triggered == false) {
    if (timer->overflow_counter < timer->overflow_max) {
      // Timer has to run
      if (timer->overflow_counter == 0) {
        // For the first round, restart periodic timer with maximum value
        sl_sleeptimer_restart_periodic_timer(&timer->sleeptimer_handle,
                                             UINT32_MAX,
                                             simple_timer_callback,
                                             (void*)timer,
                                             0,
                                             0);
      }
      timer->overflow_counter++;
    } else {
      if (LONG_TIMER_CHECK(timer)) {
        if (timer->periodic) {
          // Restart long timer
          sl_simple_timer_start(timer,
                                timer->timeout_ms,
                                timer->callback,
                                timer->callback_data,
                                true);
        } else {
          // Stop periodic timer
          sl_sleeptimer_stop_timer(&timer->sleeptimer_handle);
        }
      }
      timer->triggered = true;
      ++trigger_count;
    }
  }
}

static void append_simple_timer(sl_simple_timer_t *timer)
{
  if (simple_timer_head != NULL) {
    sl_simple_timer_t *current = simple_timer_head;
    // Find end of list.
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = timer;
  } else {
    simple_timer_head = timer;
  }
  timer->next = NULL;
}

static bool remove_simple_timer(sl_simple_timer_t *timer)
{
  sl_simple_timer_t *prev = NULL;
  sl_simple_timer_t *current = simple_timer_head;

  // Find timer in list.
  while (current != NULL && current != timer) {
    prev = current;
    current = current->next;
  }

  if (current != timer) {
    // Not found.
    return false;
  }

  if (prev != NULL) {
    prev->next = timer->next;
  } else {
    simple_timer_head = timer->next;
  }
  return true;
}
