/***************************************************************************//**
 * @file
 * @brief Helper functions for BLE interoperability test.
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
#ifndef APP_IOP_H
#define APP_IOP_H

//--------------------------------
// Connection parameters
extern uint16_t mtu_size;
extern uint16_t pdu_size;
extern uint16_t connection_interval;
extern uint16_t responder_latency;
extern uint16_t supv_timeout;

//--------------------------------
// Security
extern sl_bt_connection_security_t connection_security;
// Encryption key for pairing and bonding.
extern uint32_t passkey;

// OTA-DFU
extern bool ota_dfu_request;

//--------------------------------
// Public function prototypes
/***************************************************************************//**
 * Initialize test data values.
 ******************************************************************************/
void app_test_data_init(void);

/***************************************************************************//**
 * Handle read operation for user-type characteristics.
 *
 * @param[in] user_read_req Data structure of the user_read_request event.
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t handle_user_read(sl_bt_evt_gatt_server_user_read_request_t *user_read_req);

/***************************************************************************//**
 * Handle write operation for user-type characteristics.
 *
 * @param[in] user_write_req Data structure of the user_write_request event.
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t handle_user_write(sl_bt_evt_gatt_server_user_write_request_t *user_write_req);

/***************************************************************************//**
 * Set and start timer for corresponding test case.
 *
 * @param[in] char_stat Data structure of the characteristic_status event.
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t handle_timer_start(sl_bt_evt_gatt_server_characteristic_status_t *char_stat);

//--------------------------------
// Display operations
extern bd_addr public_address;

typedef enum {
  IDLE,
  CONNECTED,
  DISPLAY_PASSKEY,
  BOND_SUCCESS,
  BOND_FAILURE
} display_state_t;

/***************************************************************************//**
 * Handle display update.
 *
 * @param[in] state State to be displayed.
 ******************************************************************************/
void set_display(display_state_t state);

#endif // APP_IOP_H
