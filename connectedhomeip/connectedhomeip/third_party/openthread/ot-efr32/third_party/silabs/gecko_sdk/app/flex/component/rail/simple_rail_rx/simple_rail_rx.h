/***************************************************************************//**
 * @file
 * @brief simple_rail_rx.h
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
#ifndef SIMPLE_RAIL_RX_H
#define SIMPLE_RAIL_RX_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"
#include "em_device.h"
#include "sl_status.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Size of RAIL RX FIFO
#define RAIL_RX_FIFO_SIZE (256U)
/// RAIL receive channel
#define RX_CHANNEL (0U)

/// State machine of simple_trx
typedef enum {
  S_RX_PACKET_RECEIVED,
  S_RX_PACKET_ERROR,
  S_RX_NOT_VALID_PACKET_RECEIVED,
  S_RX_IDLE,
  S_RX_ENUM  // Used for parameter check
} simple_rail_rx_state_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * RAIL callback for receive event handling, used by simple_rail_rx
 *
 * @param rail_handle RAIL handle
 * @param events RAIL events
 * @returns None
 *****************************************************************************/
void sl_simple_rail_rx_on_event(RAIL_Handle_t rail_handle,
                                RAIL_Events_t events);

/**************************************************************************//**
 * Init of rail_rx - Starts RAIL reception
 *
 * @param None
 * @returns None
 *****************************************************************************/
void sl_simple_rail_rx_init(void);

/**************************************************************************//**
 * RAIL Rx state machine, used by simple_rail_rx
 *
 * @param None
 * @returns None
 *****************************************************************************/
void sl_simple_rail_rx_tick(void);

/**************************************************************************//**
 * Gets the current state of the simple_rail_rx state machine
 *
 * @param None
 * @returns Current state of the simple_rail_rx state machine
 *****************************************************************************/
simple_rail_rx_state_t sl_simple_rail_rx_get_state(void);

/**************************************************************************//**
 * Returns with the rx_fifo
 *
 * @param None
 * @returns rx_fifo
 *****************************************************************************/
uint8_t* sl_simple_rail_rx_get_rx_fifo(void);

#endif  // RAIL_RX_H
