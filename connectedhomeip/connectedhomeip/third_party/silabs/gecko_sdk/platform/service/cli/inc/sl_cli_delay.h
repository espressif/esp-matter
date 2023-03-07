/***************************************************************************//**
 * @file
 * @brief A non-blocking delay for the CLI.
 * @version x.y.z
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_CLI_DELAY_H
#define SL_CLI_DELAY_H

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "sl_cli.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup cli
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup cli_delay CLI Delay
 * @brief CLI Delay
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *  Delay the specified number of milliseconds.
 *  The function will block the CLI instance from executing any more commands
 *  until the specified time has elapsed. Other components will not be affected
 *  or blocked and will execute as expected during the delay.
 *
 * @note
 *  The function has a different implementation in a bare metal vs. kernel
 *  configurations.
 *  In a bare metal configuration, the components that require execution have a
 *  tick function that must be called regularly by the application. This is
 *  typically done in a loop in the main function. To prevent the loop in main
 *  from stalling, this delay function will return before the delay has completed.
 *  Subsequent calls to the CLI-tick function will not do any further CLI
 *  processing and will return until the delay has expired.
 *  In a kernel configuration, the delay function will block until the delay has
 *  expired. Other components running in separate tasks will execute as expected.
 *  Kernel configuration has no explicit tick functions or a main loop that has to
 *  be kept alive.
 *
 * @param[in] arguments
 *   A pointer to the command arguments.
 ******************************************************************************/
void sl_cli_delay_command(sl_cli_command_arg_t *arguments);

#if !defined(SL_CATALOG_KERNEL_PRESENT)
/***************************************************************************//**
 * @brief
 *  Complete execution of a delay started with the sl_cli_delay_command when
 *  running in a bare metal configuration.
 *
 * @param[in] handle
 *   A handle to the CLI instance where the delay is running.
 *
 * @return
 *   A boolean that is true if a delay is in progress, else false.
 ******************************************************************************/
bool sl_cli_delay_tick(sl_cli_handle_t handle);
#endif
/** @} (end addtogroup cli_delay) */
/** @} (end addtogroup cli) */

#ifdef __cplusplus
}
#endif

#endif // SL_CLI_DELAY_H
