/***************************************************************************//**
 * @file
 * @brief Application scheduler configuration
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

#ifndef APP_SCHEDULER_CONFIG_H
#define APP_SCHEDULER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> General

// <q APP_SCHEDULER_ENABLE> Enable scheduling on startup
// <i> Enable scheduling on startup.
#define APP_SCHEDULER_ENABLE                    1

// <q APP_SCHEDULER_EAGER_MODE> Eager mode
// <i> Enable eager mode to run all available active tasks in one main loop iteration.
#define APP_SCHEDULER_EAGER_MODE                0

// <o APP_SCHEDULER_MAX_QUEUE_SIZE> Queue length for static allocation
// <i> Maximum queue length for static allocation.
// <1-65535:1>
#define APP_SCHEDULER_MAX_QUEUE_SIZE            5

// <o APP_SCHEDULER_MAX_DATA_SIZE> Data size
// <i> Maximum data size in bytes.
// <0-65535:1>
#define APP_SCHEDULER_MAX_DATA_SIZE             4

// </h>

// <<< end of configuration section >>>

#endif // APP_SCHEDULER_CONFIG_H
