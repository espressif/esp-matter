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

#ifdef SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
/// BLE channel number
  #define BLE_CHANNEL ((uint8_t) 0)
#endif
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// State machine of Switch
typedef enum {
  S_ADVERTISE_STATE,
  S_READY_STATE,
} state_t;

/// Shows the light bulb's state
typedef enum {
  LIGHT_STATE_OFF,
  LIGHT_STATE_ON,
} light_state_t;

/// The state of the Switch's state machine
typedef enum {
  SWITCH_MODE_SCAN,
  SWITCH_MODE_LINKED,
} switch_states_t;

///Indicates the control role of the device
typedef enum {
  DEMO_CONTROL_ROLE_LIGHT,
  DEMO_CONTROL_ROLE_SWITCH,
} demo_control_role_t;

/// The state of the Light's state machine
typedef enum {
  LIGHT_MODE_ADVERTISE,
  LIGHT_MODE_READY,
} light_mode_t;

/// Indicates the command
typedef enum {
  LIGHT_ADVERTISE = 0,
  LIGHT_TOGGLE = 1,
  LIGHT_STATE_REPORT = 2,
  LIGHT_STATE_GET = 3,
} demo_control_command_type_t;
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
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
 * @param None
 * @returns None
 *
 * The function is used for Application logic.
 * It is called infinitely.
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * Initialize the LCD Display at the beginning of the application
 *
 * @param None
 * @returns None
 *****************************************************************************/
void init_display(void);

#endif // APP_PROCESS_H
