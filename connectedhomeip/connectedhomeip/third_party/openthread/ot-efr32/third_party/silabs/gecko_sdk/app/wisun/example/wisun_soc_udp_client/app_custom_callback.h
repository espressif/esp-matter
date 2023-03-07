/***************************************************************************//**
 * @file
 * @brief Application custom callback implementations
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

#ifndef APP_CUSTOM_CALLBACK_H
#define APP_CUSTOM_CALLBACK_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_wisun_api.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * @brief Application custom connected callback
 * @details This function registered as connected state custom callback
 * @param[in] evt event argument
 ******************************************************************************/
void app_custom_connected_callback (sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Application custom data sent on socket callback
 * @details This function registered as data sent on socket custom callback
 * @param[in] evt event argument
 *****************************************************************************/
void app_custom_socket_data_sent_callback(sl_wisun_evt_t *evt);

/**************************************************************************//**
 * @brief Application custom socket data callback
 * @details This function registered as socket data custom callback
 * @param[in] evt event argument
 *****************************************************************************/
void app_custom_socket_data_callback(sl_wisun_evt_t *evt);

#endif // APP_CUSTOM_CALLBACK_H
