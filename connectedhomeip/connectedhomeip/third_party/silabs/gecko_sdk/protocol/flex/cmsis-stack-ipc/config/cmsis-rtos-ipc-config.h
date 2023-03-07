/***************************************************************************//**
 * @brief Connect CMSIS IPC component configuration header.
 *
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Connect CMSIS IPC configuration

// <q EMBER_AF_PLUGIN_CMSIS_RTOS_CPU_USAGE> CPU usage tracking
// <i> Default: 0
// <i> If this option is enabled, the OS will keep track of CPU usage required from uc/Probe.
#define EMBER_AF_PLUGIN_CMSIS_RTOS_CPU_USAGE                      (0)

// <o EMBER_AF_PLUGIN_CMSIS_RTOS_CONNECT_STACK_PRIO> Connect task priority <2-55>
// <i> Default: 39
// <i> The priority of the Connect task using the CMSIS order (55 is highest priority)
#define EMBER_AF_PLUGIN_CMSIS_RTOS_CONNECT_STACK_PRIO             (39)

// <o EMBER_AF_PLUGIN_CMSIS_RTOS_CONNECT_STACK_SIZE> Connect Task call stack size <250-5000>
// <i> Default: 1000
// <i> The size in 32-bit words of the Connect task call stack.
#define EMBER_AF_PLUGIN_CMSIS_RTOS_CONNECT_STACK_SIZE             (1000)

// <o EMBER_AF_PLUGIN_CMSIS_RTOS_APP_FRAMEWORK_PRIO> Application Framework task priority <2-55>
// <i> Default: 38
// <i> The priority of the Application Framework task using the CMSIS order (55 is highest priority)
#define EMBER_AF_PLUGIN_CMSIS_RTOS_APP_FRAMEWORK_PRIO             (38)

// <o EMBER_AF_PLUGIN_CMSIS_RTOS_APP_FRAMEWORK_STACK_SIZE> Application Framework Task call stack size <250-5000>
// <i> Default: 500
// <i> The size in 32-bit words of the Application Framework task call stack.
#define EMBER_AF_PLUGIN_CMSIS_RTOS_APP_FRAMEWORK_STACK_SIZE       (500)

// <o EMBER_AF_PLUGIN_CMSIS_RTOS_APP_FRAMEWORK_YIELD_TIMEOUT_MS> Application Framework Task yield timeout
// <i> Default: 1000000
// <i> The Application Framework Task yield timeout in milliseconds. This is the most the application task shall yield. Upon timeout, the task will check again if yielding is possible.
#define EMBER_AF_PLUGIN_CMSIS_RTOS_APP_FRAMEWORK_YIELD_TIMEOUT_MS (1000000)

// <o EMBER_AF_PLUGIN_CMSIS_RTOS_MAX_CALLBACK_QUEUE_SIZE> Max callback queue size <5-20>
// <i> Default: 10
// <i> The maximum number of simultaneous callback messages from the stack task to the application tasks.
#define EMBER_AF_PLUGIN_CMSIS_RTOS_MAX_CALLBACK_QUEUE_SIZE        (10)

// </h>

// <<< end of configuration section >>>
