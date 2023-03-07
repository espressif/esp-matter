/***************************************************************************//**
 * @file
 * @brief Wi-SUN Application Core utility functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_WISUN_APP_CORE_UTIL_H
#define SL_WISUN_APP_CORE_UTIL_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdint.h>
#include <stdlib.h>
#include "sl_wisun_types.h"
#include "sl_wisun_app_core.h"
#include "sl_status.h"
#include "cmsis_os2.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Exit from thread (wrapper)
#define exit(exitcode)                                    \
  do {                                                    \
    printf("[%s] Exit: %d\n",                             \
           osThreadGetName(osThreadGetId()), (exitcode)); \
    osThreadTerminate(osThreadGetId());                   \
  } while (0)

/// Milli sec sleep (wrapper)
#define msleep(milis) \
  do {                \
    osDelay((milis)); \
  } while (0)

/// Assert POSIX API result with thread exit
#define assert_res(val, message)                                                     \
  do {                                                                               \
    printf("%s [%ld] %s.\n", message, (int32_t)val, (val == -1) ? "error" : "done"); \
    if ((val) == -1) {                                                               \
      exit(-1);                                                                      \
    }                                                                                \
  } while (0)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @ingroup SL_WISUN_APP_CORE_API
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @brief Initialize Wi-SUN project information.
 * @details Init internal instance
 * @param[in] app_name Application name
 *****************************************************************************/
void app_wisun_project_info_init(const char * app_name);

/**************************************************************************//**
 * @brief Print Wi-SUN project information.
 * @details Print project info in pretty or json format.
 * @param[in] json_format Json format required indicator
 *****************************************************************************/
void app_wisun_project_info_print(const bool json_format);

/**************************************************************************//**
 * @brief Wait for the connection.
 * @details This function doesn't call the app_wisun_network_connect() function.
 *          The function provides a delay loop with optional heart beat printing
 *          till the connection state has not been changed.
 *****************************************************************************/
void app_wisun_wait_for_connection(void);

/**************************************************************************//**
 * @brief Connect and wait for connection.
 * @details The function calls app_wisun_network_connect() function and
 *          app_wisun_wait_for_connection() function.
 *          It can be useful at the beginning of application thread.
 *****************************************************************************/
void app_wisun_connect_and_wait(void);

/**************************************************************************//**
 * @brief The network is connected.
 * @details Wrapper function of join state getter
 * @return true Connected
 * @return false Not connected
 *****************************************************************************/
bool app_wisun_network_is_connected(void);

/** @}*/

#endif  // SL_WISUN_APP_UTIL_H
