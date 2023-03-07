/***************************************************************************//**
 * @file
 * @brief Application Scheduler internal types
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

#ifndef APP_SCHEDULER_INTERNAL_H
#define APP_SCHEDULER_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "sl_slist.h"
#include "sl_simple_timer.h"
#include "app_scheduler.h"
// -----------------------------------------------------------------------------
// Definitions

/// Scheduler entry structure
typedef struct {
  sl_slist_node_t node;                                     ///< List node.
  app_scheduler_task_t task;                                ///< Task function.
  sl_simple_timer_t timer_handle;                           ///< Simple Timer handle.
  bool triggered;                                           ///< Timer is triggered.
  bool periodic;                                            ///< Timer is periodic.
  size_t data_size;                                         ///< Size of the data
  uint8_t data[APP_SCHEDULER_MAX_DATA_SIZE];                ///< Data to pass
} app_scheduler_entry_t;

#endif // APP_SCHEDULER_INTERNAL_H
