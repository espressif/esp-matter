/***************************************************************************//**
 * @file
 * @brief sl_light_switch.h
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
#ifndef SL_LIGHT_SWITCH_H
#define SL_LIGHT_SWITCH_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include PLATFORM_HEADER

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// The endpoint dedicated for the Light and Switch apps
#define LIGHT_SWITCH_ENDPOINT             (1)
/// Maximal transmit power setting
#define LIGHT_SWITCH_TX_POWER             (0)
/// Indicates the toggle information's location in the buffer
#define LIGHT_SWITCH_MESSAGE_CONTROL_BYTE (0)
/// Default PAN ID (Personal Area Network ID) used as an address between the nodes
#define DEFAULT_LIGHT_SWITCH_PAN_ID       (0xBEEF)

/// State machine states
typedef enum  {
  S_INIT,
  S_STANDBY,
  S_NETWORK,
  S_OPERATE,
  S_ERROR,
} light_switch_state_machine_t;

/// state of the on-board LED
typedef enum {
  DEMO_LIGHT_OFF,
  DEMO_LIGHT_ON,
}demo_light_t;

/// trigger source
typedef enum {
  SL_DIRECTION_BLUETOOTH,
  SL_DIRECTION_PROPRIETARY
} sl_direction_t;

///This structure contains all the flags used in the light's state machine
typedef struct {
  /// This flag indicates if the initialization process was successful
  bool init_success;
  /// This flag indicates if network forming has requested
  bool form_network_request;
  /// This flag indicates if network is formed properly
  bool network_formed;
  /// This flag indicates if the node wants to leave the a network
  bool leave_request;
  /// This flag indicates if error occurred during any process
  bool error_detected;
} light_application_flags_t;

///This structure contains all the flags used in the switch's state machine
typedef struct {
  /// This flag indicates if the initialization process was successful
  bool init_success;
  /// This flag indicates if the node wants to connect to a network
  bool join_request;
  /// This flag indicates if the node is connected successfully
  bool joined_network;
  /// This flag indicates if the node wants to leave the a network
  bool leave_request;
  /// This flag indicates if error occurred during any process
  bool error_detected;
} switch_application_flags_t;
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Set the actual state of the light
 *
 * @param light state: DEMO_LIGHT_OFF / DEMO_LIGHT_ON
 * @returns None
 *****************************************************************************/
void sl_set_light_state(demo_light_t new_state);

/**************************************************************************//**
 * Get the actual state of the light
 *
 * @param None
 * @returns light state: DEMO_LIGHT_OFF / DEMO_LIGHT_ON
 *****************************************************************************/
demo_light_t sl_get_light_state(void);

/**************************************************************************//**
 * Set the device's PAN ID
 *
 * @param PAN ID
 * @returns None
 *****************************************************************************/
void sl_set_pan_id(uint16_t new_id);

/**************************************************************************//**
 * Get the device's PAN ID
 *
 * @param None
 * @returns PAN_ID
 *****************************************************************************/
uint16_t sl_get_pan_id(void);

/**************************************************************************//**
 * Set the communication channel
 *
 * @param communication channel
 * @returns None
 *****************************************************************************/
void sl_set_channel(uint16_t new_channel);

/**************************************************************************//**
 * Get the communication channel
 *
 * @param None
 * @returns communication channel
 *****************************************************************************/
uint16_t sl_get_channel(void);

#endif //SL_LIGHT_SWITCH_H
