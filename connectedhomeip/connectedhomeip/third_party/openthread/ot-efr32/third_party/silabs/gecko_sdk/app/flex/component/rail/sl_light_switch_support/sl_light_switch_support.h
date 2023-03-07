/***************************************************************************//**
 * @file
 * @brief sl_light_switch_support.h
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
#ifndef SL_LIGHT_SWITCH_SUPPORT_H_
#define SL_LIGHT_SWITCH_SUPPORT_H_

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_process.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Size of RAIL RX/TX FIFO
#define RAIL_FIFO_SIZE (256U)
/// Transmit data length
#define TX_PAYLOAD_LENGTH (16U)
/// Used for checking, if the incoming message comes from a light device
#define DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT       (0x80U)
/// This is the actual payload of the message
#define DEMO_CONTROL_PAYLOAD_BYTE               (15U)
/// Used for checking if an advertise message has come from the Light device
#define DEMO_CONTROL_PAYLOAD_CMD_MASK           (0x70U)
#define DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT     (4U)
/// Used for checking the light state in the payload
#define DEMO_CONTROL_PAYLOAD_CMD_DATA           (0x0FU)
/// The incoming message's 2. bit contains the length of the package
#define PACKET_HEADER_LEN                       (2U)
#define DEVICE_STATUS_PAYLOAD_BYTE              (14U)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 *  Set the unique device ID in the transmit data
 *
 * @param payload: this is send out during the communication
 * @returns None
 *****************************************************************************/
void set_EUI(uint8_t * payload);

/**************************************************************************//**
 *  Set the specific bits that indicates the Switch Device for the Light devices
 *
 * @param payload: this is send out during the communication
 * @param role:  DEMO_CONTROL_ROLE_LIGHT,
 *               DEMO_CONTROL_ROLE_SWITCH,
 * @returns None
 *****************************************************************************/
void set_role(uint8_t * payload, demo_control_role_t role);

/**************************************************************************//**
 * Set the command type during the communication
 *
 * @param payload is transmitted during the communication
 * @param type
 * @returns None
 *****************************************************************************/
void set_command_type(uint8_t * payload, demo_control_command_type_t type);

#endif /* SL_LIGHT_SWITCH_SUPPORT_H_ */
