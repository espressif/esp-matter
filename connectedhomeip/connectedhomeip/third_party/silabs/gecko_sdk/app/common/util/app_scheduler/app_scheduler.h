/***************************************************************************//**
 * @file
 * @brief Application Scheduler interface
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

#ifndef APP_SCHEDULER_H
#define APP_SCHEDULER_H

#include <stddef.h>
#include <stdbool.h>
#include "sl_status.h"
#include "sl_power_manager.h"
#include "app_scheduler_config.h"

// -----------------------------------------------------------------------------
// Definitions

/***************************************************************************//**
 * Expected prototype of the user's task function to be scheduled.
 *
 * @param[in] data Data that is passed to the task.
 * @param[in] size Size of the data.
 ******************************************************************************/
typedef void (*app_scheduler_task_t)(void *data, uint16_t size);

/// Task handle type
typedef struct app_scheduler_entry *app_scheduler_task_handle_t;

/***************************************************************************//**
 * Prototype of a task manipulation function.
 *
 * @param[in] handle        Handle of the task.
 * @param[in] task          Task function
 * @param[in] data          Data that is passed to the task.
 * @param[in] size          Size of the data.
 * @param[in] is_triggered  True if the task is triggered.
 * @param[in] is_periodic   True if the task is periodic.
 *
 * @return Sum of the return values of the operation
 ******************************************************************************/
typedef uint8_t (*app_scheduler_operation_t)(app_scheduler_task_handle_t handle, \
                                             app_scheduler_task_t task_function, \
                                             void *data,                         \
                                             uint16_t size,                      \
                                             bool is_triggered,                  \
                                             bool is_periodic);

/// Type for counting tasks
typedef enum {
  APP_SCHEDULER_TASK_ALL,
  APP_SCHEDULER_TASK_TRIGGERED,
  APP_SCHEDULER_TASK_UNTRIGGERED
} app_scheduler_task_type_t;

// -----------------------------------------------------------------------------
// Public functions

/***************************************************************************//**
 * Remove a previously scheduled task
 *
 * @param[in] handle Handle of the task to be removed
 *
 * @retval SL_STATUS_INVALID_PARAMETER Data is too long
 * @retval SL_STATUS_OK Operation was successful
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t app_scheduler_remove(app_scheduler_task_handle_t handle);

/***************************************************************************//**
 * Add a task to be scheduled with optional data parameter
 *
 * @param[in] task Task function to be scheduled
 * @param[in] data The data that is passed to the task.
 * @param[in] size The size of the data.
 * @param[out] handle Handle of the task
 *
 * @retval SL_STATUS_NO_MORE_RESOURCE No more space in queue
 *                                    or no memory is available
 * @retval SL_STATUS_NULL_POINTER Null pointer specified for data
 * @retval SL_STATUS_INVALID_PARAMETER Data is too long
 * @retval SL_STATUS_OK Operation was successful
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t app_scheduler_add(app_scheduler_task_t task,
                              void *data,
                              size_t size,
                              app_scheduler_task_handle_t *handle);

/***************************************************************************//**
 * Add a task to be scheduled with optional data parameter
 *
 * @param[in] task Task function to be scheduled
 * @param[in] delay_ms Initial delay in ms.
 * @param[in] data The data that is passed to the task.
 * @param[in] size The size of the data.
 * @param[out] handle Handle of the task. NULL can be specified if
 *
 * @retval SL_STATUS_NO_MORE_RESOURCE No more space in queue
 *                                    or no memory is available
 * @retval SL_STATUS_NULL_POINTER Null pointer specified for data
 * @retval SL_STATUS_INVALID_PARAMETER Data is too long
 * @retval SL_STATUS_OK Operation was successful
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t app_scheduler_add_delayed(app_scheduler_task_t task,
                                      uint32_t delay_ms,
                                      void *data,
                                      size_t size,
                                      app_scheduler_task_handle_t *handle);

/***************************************************************************//**
 * Add a periodic task to be scheduled with optional data parameter
 *
 * @param[in] task Task function to be scheduled
 * @param[in] period_ms Period in ms.
 * @param[in] data The data that is passed to the task.
 * @param[in] size The size of the data.
 * @param[out] handle Handle of the task
 *
 * @retval SL_STATUS_NO_MORE_RESOURCE No more space in queue
 *                                    or no memory is available
 * @retval SL_STATUS_NULL_POINTER Null pointer specified for data
 * @retval SL_STATUS_INVALID_PARAMETER Data is too long
 * @retval SL_STATUS_OK Operation was successful
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t app_scheduler_add_periodic(app_scheduler_task_t task,
                                       uint32_t period_ms,
                                       void *data,
                                       size_t size,
                                       app_scheduler_task_handle_t *handle);

/***************************************************************************//**
 * Initialize the scheduler
 ******************************************************************************/
void app_scheduler_init(void);

/***************************************************************************//**
 * Pause the scheduler
 ******************************************************************************/
void app_scheduler_pause(void);

/***************************************************************************//**
 * Resume the scheduler
 ******************************************************************************/
void app_scheduler_resume(void);

/***************************************************************************//**
 * Get the number of tasks with a given type
 *
 * @param[in] type The type of the task.
 *
 * @return Number of the tasks with the requested type
 ******************************************************************************/
uint16_t app_scheduler_task_count_get(app_scheduler_task_type_t type);

/***************************************************************************//**
 * Execute an operation on each task
 *
 * @param[in] operation Function to execute on each entry
 *
 * @return Sum of the return values of the operation
 ******************************************************************************/
uint32_t app_scheduler_foreach_task(app_scheduler_operation_t operation);

// -----------------------------------------------------------------------------
// External callbacks

/***************************************************************************//**
 * Execute timer callback functions.
 *
 ******************************************************************************/
void app_scheduler_step(void);

/***************************************************************************//**
 * Routine for power manager handler
 *
 * @return SL_POWER_MANAGER_WAKEUP if there is an unhandled timer event
 ******************************************************************************/
sl_power_manager_on_isr_exit_t app_scheduler_sleep_on_isr_exit(void);

/***************************************************************************//**
 * Checks if it is ok to sleep now
 *
 * @return false if there is an unhandled timer event
 ******************************************************************************/
bool app_scheduler_is_ok_to_sleep(void);

#endif // APP_SCHEDULER_H
