/***************************************************************************//**
 * @file
 * @brief app_process.h
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
#ifndef APP_PROCESS_H
#define APP_PROCESS_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include "rail.h"
#include "sl_component_catalog.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// State machine states
typedef enum {
  S_INIT,             //!< Initialization before communication
  S_IDLE,             //!< Idle state waiting for TX or RX
  S_TRANSMIT,         //!< Request to TX a packet
  S_RECEIVE,          //!< Receiving a packet
  S_ERROR             //!< An error occurred
} state_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Is IEEE 802.15.4 application
 *
 * @param None
 * @returns bool
 *****************************************************************************/
extern bool is_ieee802154_app();

/**************************************************************************//**
 * Is BLE application
 *
 * @param None
 * @returns bool
 *****************************************************************************/
extern bool is_ble_app();

/**************************************************************************//**
 * The function sets the RAIL TX FIFO.
 *
 * @param rail_handle     Handle to the RAIL context
 * @returns None
 *****************************************************************************/
int16_t app_set_rail_tx_fifo(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * The function is used for Application logic.
 *
 * @param rail_handle     Handle to the RAIL context
 * @returns None
 *
 * The function is used for Application logic.
 * It is called infinitely.
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * The function initializes the all states, status for application process.
 *
 *****************************************************************************/
void app_process_init(RAIL_Handle_t rail_handle);

#endif  // APP_PROCESS_H
