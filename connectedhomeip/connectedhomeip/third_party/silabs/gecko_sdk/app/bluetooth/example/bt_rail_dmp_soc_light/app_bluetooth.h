/***************************************************************************//**
 * @file
 * @brief Application interface.
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
#ifndef APP_BLUETOOTH_H
#define APP_BLUETOOTH_H

#include "sl_bt_api.h"

// Event flags of demo application
typedef enum {
  DEMO_EVT_NONE                     = 0x00,
  DEMO_EVT_BOOTED                   = 0x01,
  DEMO_EVT_BLUETOOTH_CONNECTED      = 0x02,
  DEMO_EVT_BLUETOOTH_DISCONNECTED   = 0x03,
  DEMO_EVT_RAIL_READY               = 0x04,
  DEMO_EVT_RAIL_ADVERTISE           = 0x05,
  DEMO_EVT_LIGHT_CHANGED_BLUETOOTH  = 0x06,
  DEMO_EVT_LIGHT_CHANGED_RAIL       = 0x07,
  DEMO_EVT_INDICATION               = 0x08,
  DEMO_EVT_INDICATION_SUCCESSFUL    = 0x09,
  DEMO_EVT_INDICATION_FAILED        = 0x0A,
  DEMO_EVT_BUTTON0_PRESSED          = 0x0B,
  DEMO_EVT_BUTTON1_PRESSED          = 0x0C,
  DEMO_EVT_CLEAR_DIRECTION          = 0x0D
} demo_msg_t;

// Light states
typedef enum {
  demo_light_off = 0,
  demo_light_on  = 1
} demo_light_t;

// Demo task machine states
typedef enum {
  DEMO_STATE_INIT       = 0x00,
  DEMO_STATE_READY      = 0x01
} demo_state_t;

// Indicates the source of last light state change
typedef enum {
  demo_light_direction_bluetooth   = 0,
  demo_light_direction_proprietary = 1,
  demo_light_direction_button      = 2,
  demo_light_direction_invalid     = 3
} demo_light_direction_t;

// Storage for last Bluetooth source address
typedef struct {
  uint8_t addr[8];
} demo_light_src_addr_t;

// Overall status container type for demo application
typedef struct {
  demo_state_t state;
  demo_light_t light;
  sl_bt_gatt_client_config_flag_t light_ind;
  demo_light_direction_t direction;
  sl_bt_gatt_client_config_flag_t direction_ind;
  demo_light_src_addr_t src_addr;
  sl_bt_gatt_client_config_flag_t src_addr_ind;
  uint8_t conn_bluetooth_in_use;
  uint8_t conn_proprietary_in_use;
  bool indication_ongoing;
  bool indication_pending;
  demo_light_src_addr_t own_addr;
} demo_t;

// Overall status container instance for demo application
extern demo_t demo;

/**************************************************************************//**
 * Waits for light switch event
 *****************************************************************************/
extern void light_pend(void);

/**************************************************************************//**
 * Posts light switch event to task
 *****************************************************************************/
extern void light_post(void);

/**************************************************************************//**
 * Message queue post handler for demo application
 * @param[in] msg Message to put into the queue
 *****************************************************************************/
extern void demo_queue_post(demo_msg_t msg);

/**************************************************************************//**
 * Demo application Init.
 *****************************************************************************/
extern void init_demo_app(void);

#endif // APP_BLUETOOTH_H
