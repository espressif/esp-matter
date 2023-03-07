/***************************************************************************//**
 * @file
 * @brief Simple timer Posix specific code
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

#include <unistd.h>
#include <errno.h>
#include "sl_simple_timer.h"
#include "app_log.h"
#include "stdint.h"

/**************************************************************************//**
 * @addtogroup timer
 * @{
 *****************************************************************************/

// -----------------------------------------------------------------------------
// Private varables

/// Start of the linked list which contains the active timers
static sl_simple_timer_t *simple_timer_head = NULL;

/// Signal handle boundary conditions
static struct sigaction sig_act;

// -----------------------------------------------------------------------------
// Private function declarations

/***************************************************************************//**
 * Delete timer
 *
 * @param[in] timer_id Pointer to the ID of the timer
 *
 * @returns Status of the operation
 *
 * Find and remove timer from linked list based on the ID.
 ******************************************************************************/
static sl_status_t delete_timer(timer_t *timer_id);

/***************************************************************************//**
 * Find timer
 *
 * @param[in] Pointer to the ID of the timer
 *
 * @returns Timer present in linked list
 * @retval Return timer pointer. If NULL, timer was not found with the ID
 *
 * Find timer based on ID
 ******************************************************************************/
static sl_simple_timer_t* find_timer(timer_t *timer_id);

/***************************************************************************//**
 * Common timer callback
 *
 * @param[in] sig Required by function prototype, unused parameter
 * @param[in] si Contains expired timer ID
 * @param[in] uc Required by function prototype, unused parameter
 *
 * Common handler called when a timer expired. Call timer specific callback.
 ******************************************************************************/
static void handler(int sig, siginfo_t *si, void *uc);

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

/***************************************************************************//**
 * Timer init
 ******************************************************************************/
void sl_simple_timer_init(void)
{
  int status = -1;
  int errsv;

  sig_act.sa_flags = SA_SIGINFO;
  sig_act.sa_sigaction = handler;

  status = sigemptyset(&sig_act.sa_mask);
  errsv = errno;
  if (-1 == status) {
    app_log_error("Failed to initialize signal set with error %d."
                  APP_LOG_NL, errsv);
    exit(EXIT_FAILURE);
  }

  status = sigaction(SIGRTMIN, &sig_act, NULL);
  errsv = errno;
  if (-1 == status) {
    app_log_error("Sigaction failed with error %d." APP_LOG_NL, errsv);
    exit(EXIT_FAILURE);
  } else {
    app_log_debug("Timer function intialized" APP_LOG_NL);
  }
}

/***************************************************************************//**
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
        status = delete_timer(&(tmp_timer_ptr->simple_timer_handle.timer_id));
        if (SL_STATUS_OK != status) {
          app_log_error("Failed to delete timer after stopping it"
                        APP_LOG_NL);
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

/***************************************************************************//**
 * Start timer
 ******************************************************************************/
sl_status_t sl_simple_timer_start(sl_simple_timer_t *timer,
                                  uint32_t timeout_ms,
                                  sl_simple_timer_callback_t callback,
                                  void *callback_data,
                                  bool is_periodic)
{
  int status = -1;
  int errsv;
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
    timer->callback                                             = callback;
    timer->callback_data                                        = callback_data;
    timer->periodic                                             = is_periodic;
    timer->simple_timer_handle.sig_evt.sigev_notify             = SIGEV_SIGNAL;
    timer->simple_timer_handle.sig_evt.sigev_signo              = SIGRTMIN;
    timer->simple_timer_handle.sig_evt.sigev_value.sival_ptr    = &(timer->simple_timer_handle.timer_id);

    status = timer_create(CLOCK_REALTIME, &(timer->simple_timer_handle.sig_evt),
                          &(timer->simple_timer_handle.timer_id));
    errsv = errno;
    if (0 != status) {
      app_log_error("Error creating timer. with error %d"
                    APP_LOG_NL, errsv);
      return SL_STATUS_FAIL;
    }
    append_simple_timer(timer);
  }

  timer->simple_timer_handle.handle_timeout.it_value.tv_sec  =
    timeout_ms / 1000;
  timer->simple_timer_handle.handle_timeout.it_value.tv_nsec =
    (timeout_ms % 1000) * 1000000;

  if (true == timer->periodic) {
    timer->simple_timer_handle.handle_timeout.it_interval.tv_sec =
      timeout_ms / 1000;
    timer->simple_timer_handle.handle_timeout.it_interval.tv_nsec =
      (timeout_ms % 1000) * 1000000;
  } else {
    timer->simple_timer_handle.handle_timeout.it_interval.tv_sec = 0;
    timer->simple_timer_handle.handle_timeout.it_interval.tv_nsec = 0;
  }

  status = timer_settime(timer->simple_timer_handle.timer_id,
                         0,
                         &(timer->simple_timer_handle.handle_timeout),
                         NULL);
  errsv = errno;
  if (0 != status) {
    app_log_error("Failed to set up timer with error %d." APP_LOG_NL,
                  errsv);
    timer_delete(timer->simple_timer_handle.timer_id);
    remove_simple_timer(timer);
    return SL_STATUS_FAIL;
  }

  return 0;
}

/***************************************************************************//**
 * Stop timer
 ******************************************************************************/
sl_status_t sl_simple_timer_stop(sl_simple_timer_t *timer)
{
  int status = -1;
  int errsv;
  struct itimerspec ts = { { 0, 0 }, { 0, 0 } };
  sl_simple_timer_t *tmp_timer_ptr = NULL;

  if (NULL == timer) {
    return SL_STATUS_NULL_POINTER;
  }
  tmp_timer_ptr = find_timer(&(timer->simple_timer_handle.timer_id));
  if (NULL != tmp_timer_ptr) {
    status = timer_settime(timer->simple_timer_handle.timer_id, 0, &ts, NULL);
    errsv = errno;
    if (0 != status) {
      app_log_error("Failed to stop timer with error %d." APP_LOG_NL,
                    errsv);
      return SL_STATUS_FAIL;
    }
  } else {
    return SL_STATUS_NOT_FOUND;
  }
  if (SL_STATUS_OK != delete_timer(&(timer->simple_timer_handle.timer_id))) {
    app_log_error("Failed to delete timer after stopping it" APP_LOG_NL);
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

// -----------------------------------------------------------------------------
//Private function definitions

/***************************************************************************//**
 * Delete timer
 ******************************************************************************/
static sl_status_t delete_timer(timer_t *timer_id)
{
  int status = -1;
  int errsv;
  sl_simple_timer_t *tmp_timer_ptr = NULL;

  if (NULL == timer_id) {
    return SL_STATUS_NULL_POINTER;
  }
  tmp_timer_ptr = find_timer(timer_id);

  // Clear previously stuck errors
  errno = 0;
  status = timer_delete(*timer_id);
  errsv = errno;
  if ((0 != status) && 0 != errsv) {
    if (EINTR != errsv) {
      app_log_error("Deleting timer failed with error %d"
                    APP_LOG_NL, errsv);
      return SL_STATUS_FAIL;
    } else {  // retry to see if the timer was removed successfully
      status = timer_delete(*timer_id);
      errsv = errno;
      // If ID is not valid the timer was deleted
      if ((0 != status) && (EINVAL != errsv)) {
        app_log_error("Deleting timer failed with error %d" APP_LOG_NL,
                      errsv);
        return SL_STATUS_FAIL;
      }
    }
  }
  if (false == remove_simple_timer(tmp_timer_ptr)) {
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Find timer
 ******************************************************************************/
static sl_simple_timer_t *find_timer(timer_t *timer_id)
{
  sl_simple_timer_t *local_timer = NULL;

  local_timer = simple_timer_head;
  while ((NULL != local_timer)
         && (*timer_id != local_timer->simple_timer_handle.timer_id)) {
    local_timer = local_timer->next;
  }
  return local_timer;
}

/***************************************************************************//**
 * Common handler
 ******************************************************************************/
static void handler(int sig, siginfo_t *si, void *uc)
{
  sl_simple_timer_t *tmp_timer_ptr = NULL;

  tmp_timer_ptr = find_timer((timer_t*)(si->si_value.sival_ptr));

  if (NULL != tmp_timer_ptr) {
    tmp_timer_ptr->simple_timer_handle.triggered = true;
  } else {
    app_log_error("Timer was not found with the ID that was expired."
                  APP_LOG_NL);
  }
}

/***************************************************************************//**
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

/***************************************************************************//**
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
