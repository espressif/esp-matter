/***************************************************************************//**
 * @file
 * @brief Simple timer service based on FreeRTOS with static allocation
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdbool.h>
#include "sl_simple_timer.h"

// -----------------------------------------------------------------------------
// Private function declarations

static void simple_timer_callback(TimerHandle_t xTimer)
{
  sl_simple_timer_t *timer = (sl_simple_timer_t *)(pvTimerGetTimerID(xTimer));
  timer->callback(timer, timer->callback_data);
}

// -----------------------------------------------------------------------------
// Public function definitions

sl_status_t sl_simple_timer_start(sl_simple_timer_t *timer,
                                  uint32_t timeout_ms,
                                  sl_simple_timer_callback_t callback,
                                  void *callback_data,
                                  bool is_periodic)
{
  BaseType_t err;
  TickType_t tick_rate;
  TickType_t delay, period;
  UBaseType_t opt;
  TimerHandle_t handle;
  sl_status_t sc;

  // Check input parameters.
  if ( (timeout_ms == 0) && is_periodic ) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ( timer == NULL ) {
    return SL_STATUS_NULL_POINTER;
  }

  // Calculate timer period.
  tick_rate = configTICK_RATE_HZ;
  delay = (timeout_ms * tick_rate) / (1000);
  if ( delay == 0 ) {
    // The timer resolution is too small for the requested timeout.
    return SL_STATUS_INVALID_PARAMETER;
  }
  period = delay;
  opt = is_periodic ? pdTRUE : pdFALSE;

  // Make sure that timer is stopped, also check for NULL.
  sc = sl_simple_timer_stop(timer);

  if (sc == SL_STATUS_FAIL) {
    return SL_STATUS_FAIL;
  }

  // Create OS timer.
  handle = xTimerCreateStatic("Simple Timer",
                              period,
                              opt,
                              (void *)timer,
                              (TimerCallbackFunction_t)simple_timer_callback,
                              &timer->static_timer
                              );
  if ( handle == NULL ) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  timer->handle = handle;
  timer->callback = callback;
  timer->callback_data = callback_data;
  timer->periodic = is_periodic;

  // Start OS timer.
  err = xTimerStart(timer->handle, 0);
  if ( err != pdPASS ) {
    sl_simple_timer_stop(timer);
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

sl_status_t sl_simple_timer_stop(sl_simple_timer_t *timer)
{
  BaseType_t res;
  if (timer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  if (timer->handle != NULL) {
    res = xTimerDelete(timer->handle, 100);
    if (pdPASS != res) {
      return SL_STATUS_FAIL;
    }
    timer->handle = NULL;
  }
  return SL_STATUS_OK;
}
