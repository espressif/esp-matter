/***************************************************************************//**
 * @file
 * @brief sl_flex_ble_protocol_type.h
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

#ifndef SL_FLEX_UTIL_BLE_PROTOCOL_TYPES_H
#define SL_FLEX_UTIL_BLE_PROTOCOL_TYPES_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
typedef enum sl_flex_util_ble_protocol_type{
  SL_RAIL_UTIL_PROTOCOL_BLE_1MBPS,
  SL_RAIL_UTIL_PROTOCOL_BLE_2MBPS,
  SL_RAIL_UTIL_PROTOCOL_BLE_CODED_125KBPS,
  SL_RAIL_UTIL_PROTOCOL_BLE_CODED_500KBPS,
  SL_RAIL_UTIL_PROTOCOL_BLE_QUUPPA_1MBPS,
} sl_flex_util_ble_protocol_type_t;

#define SL_RAIL_UTIL_PROTOCOL_IS_BLE(x)              \
  ((x == SL_RAIL_UTIL_PROTOCOL_BLE_1MBPS)            \
   || (x == SL_RAIL_UTIL_PROTOCOL_BLE_2MBPS)         \
   || (x == SL_RAIL_UTIL_PROTOCOL_BLE_CODED_125KBPS) \
   || (x == SL_RAIL_UTIL_PROTOCOL_BLE_CODED_500KBPS))

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

#endif // SL_FLEX_UTIL_BLE_PROTOCOL_TYPES_H
