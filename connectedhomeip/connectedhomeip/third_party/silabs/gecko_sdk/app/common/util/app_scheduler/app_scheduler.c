/***************************************************************************//**
 * @file
 * @brief Application Scheduler source
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
#include <string.h>
#include "app_scheduler.h"
#include "app_scheduler_config.h"
#include "app_scheduler_memory.h"
#include "sl_simple_timer.h"
#include "sl_slist.h"
#include "em_core.h"
#include "app_scheduler_internal.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#error "The app_scheduler component can not be used with a kernel."
#endif

// -----------------------------------------------------------------------------
// Definitions

#if APP_SCHEDULER_EAGER_MODE
#define APP_SCHEDULER_RUN()  while (handle != NULL)
#define APP_SCHEDULER_NEXT() handle = next_active_task()
#else // APP_SCHEDULER_EAGER_MODE
#define APP_SCHEDULER_RUN()  if (handle != NULL)
#define APP_SCHEDULER_NEXT()
#endif // APP_SCHEDULER_EAGER_MODE

// -----------------------------------------------------------------------------
// Local variables

// State of the scheduler
static bool run;

// Start of the linked list which contains the queue
static sl_slist_node_t *queue = NULL;

// -----------------------------------------------------------------------------
// Function declarations

static app_scheduler_entry_t *next_active_task(void);
static void timer_callback(sl_simple_timer_t *handle,
                           void *data);

// -----------------------------------------------------------------------------
// Private functions

/***************************************************************************//**
 * Check if the queue is empty
 ******************************************************************************/
static app_scheduler_entry_t *next_active_task(void)
{
  app_scheduler_entry_t *task = NULL;
  app_scheduler_entry_t *handle;

  // Linear search in the queue for the next triggered task
  SL_SLIST_FOR_EACH_ENTRY(queue, handle, app_scheduler_entry_t, node) {
    if (handle->triggered) {
      // Save the handle
      task = handle;
      // Quit the loop
      break;
    }
  }

  return task;
}

/***************************************************************************//**
 * Timer callback for the scheduler
 ******************************************************************************/
static void timer_callback(sl_simple_timer_t *handle,
                           void *data)
{
  (void)handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  if (((app_scheduler_entry_t*)data)->triggered == false) {
    ((app_scheduler_entry_t*)data)->triggered = true;
  }
  CORE_EXIT_CRITICAL();
}

// -----------------------------------------------------------------------------
// Public functions

/***************************************************************************//**
 * Remove a previously scheduled task
 ******************************************************************************/
sl_status_t app_scheduler_remove(app_scheduler_task_handle_t handle)
{
  app_scheduler_entry_t *task = NULL;
  sl_status_t sc = SL_STATUS_INVALID_PARAMETER;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  // Linear search in the queue for the task
  SL_SLIST_FOR_EACH_ENTRY(queue, task, app_scheduler_entry_t, node) {
    if (((app_scheduler_entry_t *)handle) == task) {
      // Stop timer
      sl_simple_timer_stop(&task->timer_handle);

      // Remove from list
      sl_slist_remove(&queue, &task->node);

      // Free up memory
      app_scheduler_memory_free((app_scheduler_task_handle_t)task);

      sc = SL_STATUS_OK;
      // Quit the loop
      break;
    }
  }
  CORE_EXIT_CRITICAL();

  return sc;
}

/***************************************************************************//**
 * Add a periodic task to be scheduled with optional data parameter
 ******************************************************************************/
sl_status_t app_scheduler_add_periodic(app_scheduler_task_t task,
                                       uint32_t period_ms,
                                       void *data,
                                       size_t size,
                                       app_scheduler_task_handle_t *handle)
{
  app_scheduler_entry_t *entry = NULL;
  sl_status_t sc = SL_STATUS_OK;

  CORE_DECLARE_IRQ_STATE;

  // Check queue
  if (app_scheduler_task_count_get(APP_SCHEDULER_TASK_ALL)
      >= APP_SCHEDULER_MAX_QUEUE_SIZE) {
    sc = SL_STATUS_NO_MORE_RESOURCE;
  }

  // Check parameters - size and period_ms
  if ( (size > APP_SCHEDULER_MAX_DATA_SIZE) || (period_ms == 0) ) {
    sc = SL_STATUS_INVALID_PARAMETER;
  }
  if (task == NULL) {
    sc = SL_STATUS_NULL_POINTER;
  }

  if (sc == SL_STATUS_OK) {
    CORE_ENTER_CRITICAL();
    sc = app_scheduler_memory_alloc((app_scheduler_task_handle_t *)&entry);
    if (sc == SL_STATUS_OK) {
      entry->task = task;
      entry->periodic = true;
      // Copy data
      memcpy(entry->data, data, size);
      entry->data_size = size;

      sl_slist_push_back(&queue, &entry->node);

      // Make untriggered
      entry->triggered = false;
      // Start periodic timer
      sc = sl_simple_timer_start(&entry->timer_handle,
                                 period_ms,
                                 timer_callback,
                                 (void*)entry,
                                 true);
      if (handle != NULL) {
        *handle = (app_scheduler_task_handle_t)entry;
      }
    }
    CORE_EXIT_CRITICAL();
  }

  return sc;
}

/***************************************************************************//**
 * Add a task to be scheduled with optional data parameter and a delay
 ******************************************************************************/
sl_status_t app_scheduler_add_delayed(app_scheduler_task_t task,
                                      uint32_t delay_ms,
                                      void *data,
                                      size_t size,
                                      app_scheduler_task_handle_t *handle)
{
  app_scheduler_entry_t *entry = NULL;
  sl_status_t sc = SL_STATUS_OK;

  CORE_DECLARE_IRQ_STATE;

  // Check queue
  if (app_scheduler_task_count_get(APP_SCHEDULER_TASK_ALL)
      >= APP_SCHEDULER_MAX_QUEUE_SIZE) {
    sc = SL_STATUS_NO_MORE_RESOURCE;
  }

  // Check parameters
  if (size > APP_SCHEDULER_MAX_DATA_SIZE) {
    sc = SL_STATUS_INVALID_PARAMETER;
  }
  if (task == NULL) {
    sc = SL_STATUS_NULL_POINTER;
  }

  if (sc == SL_STATUS_OK) {
    CORE_ENTER_CRITICAL();
    sc = app_scheduler_memory_alloc((app_scheduler_task_handle_t *)&entry);
    if (sc == SL_STATUS_OK) {
      entry->task = task;
      entry->periodic = false;
      // Copy data
      memcpy(entry->data, data, size);
      entry->data_size = size;

      sl_slist_push_back(&queue, &entry->node);

      if (delay_ms == 0) {
        // Make triggered
        entry->triggered = true;
      } else {
        // Make untriggered
        entry->triggered = false;
        // Start timer
        sc = sl_simple_timer_start(&entry->timer_handle,
                                   delay_ms,
                                   timer_callback,
                                   (void*)entry,
                                   false);
        if (handle != NULL) {
          *handle = (app_scheduler_task_handle_t)entry;
        }
      }
    }
    CORE_EXIT_CRITICAL();
  }

  return sc;
}

/***************************************************************************//**
 * Add a task to be scheduled with optional data parameter
 ******************************************************************************/
sl_status_t app_scheduler_add(app_scheduler_task_t task,
                              void *data,
                              size_t size,
                              app_scheduler_task_handle_t *handle)
{
  sl_status_t sc;
  sc = app_scheduler_add_delayed(task,
                                 0,
                                 data,
                                 size,
                                 handle);
  return sc;
}

/***************************************************************************//**
 * Get the number of tasks with a given type
 ******************************************************************************/
uint16_t app_scheduler_task_count_get(app_scheduler_task_type_t type)
{
  app_scheduler_entry_t *handle;
  uint16_t count = 0;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  SL_SLIST_FOR_EACH_ENTRY(queue, handle, app_scheduler_entry_t, node) {
    switch (type) {
      case APP_SCHEDULER_TASK_UNTRIGGERED:
        if (handle->triggered == false) {
          count++;
        }
        break;
      case APP_SCHEDULER_TASK_TRIGGERED:
        if (handle->triggered == true) {
          count++;
        }
        break;
      default:
        count++;
        break;
    }
  }
  CORE_EXIT_CRITICAL();
  return count;
}

/***************************************************************************//**
 * Execute an operation on each task
 ******************************************************************************/
uint32_t app_scheduler_foreach_task(app_scheduler_operation_t operation)
{
  app_scheduler_entry_t *handle;
  uint32_t count = 0;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  SL_SLIST_FOR_EACH_ENTRY(queue, handle, app_scheduler_entry_t, node) {
    if (operation != NULL) {
      uint32_t ret = operation((app_scheduler_task_handle_t)handle,
                               handle->task,
                               handle->data,
                               handle->data_size,
                               handle->triggered,
                               handle->periodic);
      count = count + ret;
    }
  }
  CORE_EXIT_CRITICAL();
  return count;
}

/******************************************************************************
* Application Scheduler init
******************************************************************************/
void app_scheduler_init(void)
{
  sl_slist_init(&queue);
  run = APP_SCHEDULER_ENABLE;
}

/******************************************************************************
* Application Scheduler pause
******************************************************************************/
void app_scheduler_pause(void)
{
  CORE_CRITICAL_SECTION(
    run = false;
    );
}

/******************************************************************************
* Application Scheduler resume
******************************************************************************/
void app_scheduler_resume(void)
{
  CORE_CRITICAL_SECTION(
    run = true;
    );
}

/***************************************************************************//**
 * Execute timer callback functions.
 ******************************************************************************/
void app_scheduler_step(void)
{
  app_scheduler_entry_t * handle;
  CORE_DECLARE_IRQ_STATE;

  if (run) {
    // Get next active task
    handle = next_active_task();

    APP_SCHEDULER_RUN() {
      // Call task function
      handle->task(handle->data, handle->data_size);

      CORE_ENTER_CRITICAL();
      handle->triggered = false;
      // if not periodic, remove
      if (false == handle->periodic) {
        // Remove from list
        sl_slist_remove(&queue, &handle->node);
        // Free up memory
        app_scheduler_memory_free((app_scheduler_task_handle_t)handle);
      }
      CORE_EXIT_CRITICAL();

      APP_SCHEDULER_NEXT();
    }
  }
}

/***************************************************************************//**
 * Routine for power manager handler
 ******************************************************************************/
sl_power_manager_on_isr_exit_t app_scheduler_sleep_on_isr_exit(void)
{
  sl_power_manager_on_isr_exit_t ret = SL_POWER_MANAGER_IGNORE;
  // if there is a triggered event, wake up to handle it
  if (next_active_task() != NULL) {
    ret = SL_POWER_MANAGER_WAKEUP;
  }
  return ret;
}

/***************************************************************************//**
 * Checks if it is ok to sleep now
 ******************************************************************************/
bool app_scheduler_is_ok_to_sleep(void)
{
  bool ret = true;
  // if there is a triggered event, do not go to sleep
  if (next_active_task() != NULL) {
    ret = false;
  }
  return ret;
}
