/***************************************************************************//**
 * @file
 * @brief sl_flex_util_802154_init.h
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
#ifndef SL_RAIL_UTIL_INIT_H
#define SL_RAIL_UTIL_INIT_H
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"
#include "sl_flex_util_802154_init_config.h"

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
 * Initializes the RAIL, configures data management, channels, protocol,
 * calibration and RAIL events.
 *****************************************************************************/
void sl_flex_util_init(void);

/**************************************************************************//**
 * Returns RAIL handle
 *****************************************************************************/
RAIL_Handle_t sl_flex_util_get_handle(void);

/**************************************************************************//**
 * RAIL callback: called when any assert happens
 *****************************************************************************/
void sl_rail_util_on_assert_failed(RAIL_Handle_t rail_handle,
                                   RAIL_AssertErrorCodes_t error_code);

/**************************************************************************//**
 * RAIL callback: called when the RAIL is ready for use
 *****************************************************************************/
void sl_rail_util_on_rf_ready(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * RAIL callback: called when channel changed
 *****************************************************************************/
void sl_rail_util_on_channel_config_change(RAIL_Handle_t rail_handle,
                                           const RAIL_ChannelConfigEntry_t *entry);

/**************************************************************************//**
 * RAIL callback: called when any event occurred inside of RAIL
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle,
                           RAIL_Events_t events);

#endif // SL_RAIL_UTIL_INIT_H
