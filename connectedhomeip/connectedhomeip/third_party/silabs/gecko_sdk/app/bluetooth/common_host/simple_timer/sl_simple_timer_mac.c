/***************************************************************************//**
 * @file
 * @brief Simple timer Mac specific code
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

#include <dispatch/dispatch.h>
#include <time.h>
#include "sl_simple_timer.h"
#include "app_log.h"
#include "stdint.h"

/***************************************************************************//**
 * @addtogroup timer
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Private varables

/// Start of the linked list which contains the active timers
static sl_simple_timer_t *simple_timer_head = NULL;

/// OS queue of timers
static dispatch_queue_t queue;

// -----------------------------------------------------------------------------
// Private function declarations

/***************************************************************************//**
 * Timer cancel event handler
 *
 * @param[in] timer Pointer to the timer
 ******************************************************************************/
static void simple_timer_cancel_handler(void *timer);

/***************************************************************************//**
 * Common handler of the timers
 *
 * @param[in] timer Pointer to the timer structure that has expired
 ******************************************************************************/
static void simple_timer_common_handler(void *timer);

/***************************************************************************//**
 * Delete timer
 *
 * @param[in] timer Pointer to the timer structure.
 *
 * @returns Status of the operation
 *
 * Find and remove timer from linked list based on the ID.
 ******************************************************************************/
static sl_status_t delete_timer(sl_simple_timer_t *timer);

/***************************************************************************//**
 * Append a timer to the end of the linked list.
 *
 * @param[in] timer Pointer to the timer structure.
 *
 * @pre Assumes that the timer is not present in the list.
 ******************************************************************************/
static void append_simple_timer(sl_simple_timer_t *timer);

/***************************************************************************//**
 * Remove a timer from the linked list.
 *
 * @param[in] timer Pointer to the timer structure.
 *
 * @return Presence of the timer in the linked list.
 * @retval true  Timer was in the list.
 * @retval false Timer was not found in the list.
 ******************************************************************************/
static bool remove_simple_timer(sl_simple_timer_t *timer);

// -----------------------------------------------------------------------------
// Public function definitions

/*******************************************************************************
 * Timer init
 ******************************************************************************/
void sl_simple_timer_init(void)
{
  // Create a serial dispatch queue
  queue = dispatch_queue_create("timer_queue", DISPATCH_QUEUE_SERIAL);
  app_log_debug("Timer function intialized" APP_LOG_NEW_LINE);
}

/*******************************************************************************
 * Step
 ******************************************************************************/
void sli_simple_timer_step(void)
{
  sl_status_t status = SL_STATUS_FAIL;
  sl_simple_timer_t *tmp_timer_ptr = simple_timer_head;

  while (NULL != tmp_timer_ptr) {
    if (true == tmp_timer_ptr->simple_timer_handle.triggered) {
      // Delete non-periodic timers
      if (false == tmp_timer_ptr->periodic) {
        status = delete_timer(tmp_timer_ptr);
        if (SL_STATUS_OK != status) {
          app_log_error("Failed to delete timer after stopping it"
                        APP_LOG_NEW_LINE);
        }
      }
      if (NULL != tmp_timer_ptr->callback) {
        tmp_timer_ptr->callback(tmp_timer_ptr, tmp_timer_ptr->callback_data);
      }
      tmp_timer_ptr->simple_timer_handle.triggered = false;
    }
    tmp_timer_ptr = tmp_timer_ptr->next;
  }
}

/*******************************************************************************
 * Start timer
 ******************************************************************************/
sl_status_t sl_simple_timer_start(sl_simple_timer_t *timer,
                                  uint32_t timeout_ms,
                                  sl_simple_timer_callback_t callback,
                                  void *callback_data,
                                  bool is_periodic)
{
  sl_simple_timer_t *tmp_timer_ptr = simple_timer_head;

  // Check NULL pointer parameter
  if (NULL == timer) {
    return SL_STATUS_NULL_POINTER;
  }

  // Check if timer was already used
  while ((timer != tmp_timer_ptr) && (NULL != tmp_timer_ptr)) {
    tmp_timer_ptr = tmp_timer_ptr->next;
  }
  if (timer != tmp_timer_ptr) {
    timer->callback                     = callback;
    timer->callback_data                = callback_data;
    timer->periodic                     = is_periodic;
    timer->simple_timer_handle.os_timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,
                                                                 0,
                                                                 0,
                                                                 queue);
    if (NULL == timer->simple_timer_handle.os_timer) {
      app_log_error("Error creating timer." APP_LOG_NEW_LINE);
      return SL_STATUS_FAIL;
    }
    append_simple_timer(timer);
  }

  //Set what to do on cancel
  dispatch_source_set_cancel_handler_f(timer->simple_timer_handle.os_timer,
                                       &simple_timer_cancel_handler);

  // Start timer from now, expire in timeout_ms
  dispatch_time_t start_time = dispatch_time(DISPATCH_TIME_NOW,
                                             (NSEC_PER_MSEC * timeout_ms));
  // Set handler for the new timer
  dispatch_source_set_event_handler_f(timer->simple_timer_handle.os_timer,
                                      &simple_timer_common_handler);
  // Set data of the handler
  dispatch_set_context(timer->simple_timer_handle.os_timer, timer);

  if (true == timer->periodic) {
    dispatch_source_set_timer(timer->simple_timer_handle.os_timer, // Timer handle
                              start_time,                          // Start now
                              (NSEC_PER_MSEC * timeout_ms),        // Period
                              0);                                  // Leeway
  } else {
    dispatch_source_set_timer(timer->simple_timer_handle.os_timer, // Timer handle
                              start_time,                          // Start now
                              0,                                   // Period
                              0);                                  // Leeway
  }

  // Dispatch the timer
  app_log_debug("Dispatching timer" APP_LOG_NEW_LINE);
  dispatch_resume(timer->simple_timer_handle.os_timer);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Stop timer
 ******************************************************************************/
sl_status_t sl_simple_timer_stop(sl_simple_timer_t *timer)
{
  if (NULL == timer) {
    return SL_STATUS_NULL_POINTER;
  }

  dispatch_release(timer->simple_timer_handle.os_timer);

  if (SL_STATUS_OK != delete_timer(timer)) {
    app_log_error("Failed to delete timer after stopping it" APP_LOG_NEW_LINE);
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

// -----------------------------------------------------------------------------
//Private function definitions

/***************************************************************************//**
 * Delete timer
 ******************************************************************************/
static sl_status_t delete_timer(sl_simple_timer_t *timer)
{
  if (NULL == timer) {
    return SL_STATUS_NULL_POINTER;
  }

  if (false == remove_simple_timer(timer)) {
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

// -----------------------------------------------------------------------------
// Private function definitions

/*******************************************************************************
 * Cancel handler
 ******************************************************************************/
static void simple_timer_cancel_handler(void *timer)
{
  sl_simple_timer_t *simple_timer = (sl_simple_timer_t *)timer;
  dispatch_source_t os_timer = simple_timer->simple_timer_handle.os_timer;
  dispatch_release(os_timer);
}

/*******************************************************************************
 * Common handler
 ******************************************************************************/
static void simple_timer_common_handler(void *timer)
{
  sl_simple_timer_t *simple_timer = (sl_simple_timer_t *)timer;
  app_log_debug("Timer finished" APP_LOG_NEW_LINE);
  // Stop timer if not periodic
  if (false == simple_timer->periodic) {
    dispatch_source_cancel(simple_timer->simple_timer_handle.os_timer);
  }
  // Set triggered
  simple_timer->simple_timer_handle.triggered = true;
}

/*******************************************************************************
 * Append timer
 ******************************************************************************/
static void append_simple_timer(sl_simple_timer_t *timer)
{
  if (NULL != simple_timer_head) {
    bool exist = false;
    sl_simple_timer_t *current = simple_timer_head;
    // Find end of list.
    while (NULL != current->next) {
      current = current->next;
      if (timer == current) {
        exist = true;
        break;
      }
    }
    if (false == exist) {
      current->next = timer;
    }
  } else {
    simple_timer_head = timer;
  }
  timer->next = NULL;
}

/*******************************************************************************
 * Remove timer
 ******************************************************************************/
static bool remove_simple_timer(sl_simple_timer_t *timer)
{
  sl_simple_timer_t *prev = NULL;
  sl_simple_timer_t *current = simple_timer_head;

  // Find timer in list.
  while ((NULL != current) && (timer != current)) {
    prev = current;
    current = current->next;
  }

  if (timer != current) {
    // Not found.
    return false;
  }

  if (NULL != prev) {
    prev->next = timer->next;
  } else {
    simple_timer_head = timer->next;
  }
  return true;
}
/** @} (end addtogroup timer) */
