/***************************************************************************//**
 * @file
 * @brief simple_rail_tx.h
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
#ifndef SIMPLE_RAIL_TX_H
#define SIMPLE_RAIL_TX_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"
#include "sl_status.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Size of RAIL TX FIFO
#define RAIL_TX_FIFO_SIZE (256U)
/// RAIL transmit data length
#define TX_PAYLOAD_LENGTH (16U)
/// RAIL transmit channel
#define TX_CHANNEL (0U)
/// 802.15.4 / BLE / Z-Wave valid PDU (16bytes)
//  0x0F, - 1st Byte: IEEE 802.15.4 PHY Header, (PSDU Length:8 bits)
//  0x0E, - 2nd Byte: BLE PDU Header, (Length:8 bits)
//  0x11, 0x22, 0x33, 0x44, 0x55,
//  0x16, - 8th Byte: Z-Wave Data Length, (Length:8 bits)
//  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE
#define UNIVERSAL_PAYLOAD {       \
    0x0F,                         \
    0x0E,                         \
    0x11, 0x22, 0x33, 0x44, 0x55, \
    0x16,                         \
    0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE }

/// State machine of simple_rail_tx
typedef enum {
  S_TX_PACKET_SEND_REQUEST,
  S_TX_PACKET_SENDING,
  S_TX_PACKET_SENT,
  S_TX_PACKET_ERROR,
  S_TX_IDLE,
  S_TX_ENUM  // Used for parameter check
} simple_rail_tx_state_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * Init of rail_tx - Starts RAIL reception
 *
 * @param None
 * @returns None
 *****************************************************************************/
void sl_simple_rail_tx_init(void);

/**************************************************************************//**
 * RAIL callback for transmit event handling, used by simple_rail_tx
 *
 * @param rail_handle RAIL handle
 * @param events RAIL events
 * @returns None
 *****************************************************************************/
void sl_simple_rail_tx_on_event(RAIL_Handle_t rail_handle,
                                RAIL_Events_t events);

/**************************************************************************//**
 * RAIL Tx state machine, used by simple_rail_tx
 *
 * @param None
 * @returns None
 *****************************************************************************/
void sl_simple_rail_tx_tick(void);

/**************************************************************************//**
 * Updates sl_tx_fifo, i.e. updates RAIL transmit data with a memcpy() call
 * and starts RAIL transmission
 * Max 16 bytes can be written
 *
 * @param p_src buffer to be sent
 * @returns num_of_bytes number of bytes to be copied and sent
 *****************************************************************************/
sl_status_t sl_simple_rail_tx_transmit(const void * const p_src,
                                       const uint8_t num_of_bytes);

/**************************************************************************//**
 * Gets the current state of the simple_rail_tx state machine
 *
 * @param None
 * @returns Current state of the simple_rail_tx state machine
 *****************************************************************************/
simple_rail_tx_state_t sl_simple_rail_tx_get_state(void);

#endif  // RAIL_TX_H
