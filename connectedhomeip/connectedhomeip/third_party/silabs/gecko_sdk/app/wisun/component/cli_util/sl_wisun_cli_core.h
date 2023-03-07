/***************************************************************************//**
 * @file
 * @brief Application commandline core
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

#ifndef __SL_WISUN_CLI_CORE_H__
#define __SL_WISUN_CLI_CORE_H__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sl_status.h"
#include "sl_wisun_types.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Unlock th CLI mutex and return.
 *****************************************************************************/
#define app_wisun_release_cli_mutex_and_return() \
  do {                                           \
    app_wisun_cli_mutex_unlock();                \
    return;                                      \
  } while (0)

/**************************************************************************//**
 * @brief Unlock the CLI mutex and return with a value.
 *****************************************************************************/
#define app_wisun_release_cli_mutex_and_return_val(__val) \
  do {                                                    \
    app_wisun_cli_mutex_unlock();                         \
    return (__val);                                       \
  } while (0)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Initialize the CLI components mutex.
 * @details This function initializes the CLI mutex.
 *****************************************************************************/
void app_wisun_cli_init(void);

/**************************************************************************//**
 * @brief Lock by mutex.
 *****************************************************************************/
void app_wisun_cli_mutex_lock(void);

/**************************************************************************//**
 * @brief Unlock by mutex.
 *****************************************************************************/
void app_wisun_cli_mutex_unlock(void);

#endif
