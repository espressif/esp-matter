/***************************************************************************//**
 * @file
 * @brief Application logging command line interface
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

#ifndef APP_LOG_CLI_H
#define APP_LOG_CLI_H

#include "sl_status.h"
#include "app_log_config.h"

// Usage info.
#define APP_LOG_USAGE " [-l <log_level_filter>] "

// Optstring argument for getopt.
#define APP_LOG_OPTSTRING  "l:"

// Options info with log level filter parameter (def_val).
#define _APP_LOG_OPTIONS(def_val)                                          \
  "    -l  Application log level filter.\n"                                \
  "        <level>          Integer representing the log level, default: " \
  #def_val                                                                 \
  ".\n"                                                                    \
  "        0 : Critical.\n"                                                \
  "        1 : Critical, error.\n"                                         \
  "        2 : Critical, error, warning.\n"                                \
  "        3 : Critical, error, warning, info.\n"                          \
  "        4 : Critical, error, warning, info, debug.\n"

// Helper macro to expand default value.
#define _APP_LOG_OPTIONS_X(def_val) _APP_LOG_OPTIONS(def_val)

// Options info.
#define APP_LOG_OPTIONS _APP_LOG_OPTIONS_X(APP_LOG_LEVEL_FILTER_THRESHOLD)

/**************************************************************************//**
 * Set application log options.
 *
 * @param[in] option Option to set.
 * @param[in] value Value of the option.
 *
 * @retval SL_STATUS_OK Option set successfully.
 * @retval SL_STATUS_NOT_FOUND Unknown option.
 * @retval SL_STATUS_INVALID_PARAMETER Invalid parameter value.
 *****************************************************************************/
sl_status_t app_log_set_option(char option, char *value);

#endif // APP_LOG_CLI_H
