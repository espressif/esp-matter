/***************************************************************************//**
 * @file
 * @brief Application Scheduler Memory Management Interface
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

#ifndef APP_SCHEDULER_MEMORY_H
#define APP_SCHEDULER_MEMORY_H

#include "sl_status.h"
#include "app_scheduler.h"

// -----------------------------------------------------------------------------
// Public functions

/***************************************************************************//**
 * Allocate memory for the scheduler
 *
 * @param[out] data pointer of the pointer to the allocated data.
 *
 * @retval SL_STATUS_NO_MORE_RESOURCE No more memory available
 * @retval SL_STATUS_NULL_POINTER Null pointer specified for data
 * @retval SL_STATUS_OK Operation was successful
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t app_scheduler_memory_alloc(app_scheduler_task_handle_t *data);

/***************************************************************************//**
 * Free memory that was allocated for the scheduler
 *
 * @param[out] data pointer to the previously allocated data.

 * @retval SL_STATUS_INVALID_PARAMETER The pointer is not found
 * @retval SL_STATUS_OK Operation was successful
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t app_scheduler_memory_free(app_scheduler_task_handle_t data);

#endif // APP_SCHEDULER_MEMORY_H
