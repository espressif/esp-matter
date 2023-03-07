/***************************************************************************//**
 * @file
 * @brief sl_flex_ble_protocol.h
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

#ifndef SL_RAIL_UTIL_BLE_PROTOCOL_H
#define SL_RAIL_UTIL_BLE_PROTOCOL_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"
#include "sl_flex_util_ble_protocol_types.h"
#include "sl_flex_util_ble_protocol_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * Initialize and configure the radio for use.
 *
 * @param[in] handle The RAIL handle to apply the radio configuration to.
 * @param[in] protocol The radio configuration type to initialize and configure.
 * @return A status code indicating success of the function call.
 *****************************************************************************/
RAIL_Status_t sl_flex_util_ble_protocol_config(RAIL_Handle_t handle,
                                               sl_flex_util_ble_protocol_type_t protocol);

#endif // SL_RAIL_UTIL_BLE_PROTOCOL_H
