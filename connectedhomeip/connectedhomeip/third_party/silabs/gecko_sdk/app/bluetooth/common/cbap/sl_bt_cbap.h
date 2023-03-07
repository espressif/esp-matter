/***************************************************************************//**
 * @file
 * @brief Certificate Based Authentication and Pairing header
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_BT_CBAP_H
#define SL_BT_CBAP_H

#include <stdbool.h>

/// Role enum type
typedef enum {
  SL_BT_CBAP_ROLE_PERIPHERAL,
  SL_BT_CBAP_ROLE_CENTRAL
} sl_bt_cbap_role_t;

/// Central device states
typedef enum {
  SL_BT_CBAP_CENTRAL_SCANNING,
  SL_BT_CBAP_CENTRAL_DISCOVER_SERVICES,
  SL_BT_CBAP_CENTRAL_DISCOVER_CHARACTERISTICS,
  SL_BT_CBAP_CENTRAL_GET_PERIPHERAL_CERT,
  SL_BT_CBAP_CENTRAL_SEND_CENTRAL_CERT,
  SL_BT_CBAP_CENTRAL_GET_PERIPHERAL_OOB,
  SL_BT_CBAP_CENTRAL_SEND_OOB,
  SL_BT_CBAP_CENTRAL_INCREASE_SECURITY,
  SL_BT_CBAP_CENTRAL_DONE,
  SL_BT_CBAP_CENTRAL_STATE_NUM
} sl_bt_cbap_central_state_t;

/// Peripheral device states
typedef enum {
  SL_BT_CBAP_PERIPHERAL_IDLE,
  SL_BT_CBAP_PERIPHERAL_CENTRAL_CERT_OK,
  SL_BT_CBAP_PERIPHERAL_CENTRAL_OOB_OK,
  SL_BT_CBAP_PERIPHERAL_DONE,
  SL_BT_CBAP_PERIPHERAL_STATE_NUM
} sl_bt_cbap_peripheral_state_t;

/**************************************************************************//**
 * Initialize the component.
 * Import and validate the device and root certificate.
 *****************************************************************************/
void sl_bt_cbap_init(void);

/**************************************************************************//**
 * Start CBAP procedure.
 * @param[in] cbap_role Device role. Should be either SL_BT_CBAP_ROLE_PERIPHERAL or
 * SL_BT_CBAP_ROLE_CENTRAL.
 * @param[in] connection Handle of the active connection.
 *
 * @param SL_STATUS_OK if successful otherwise error code.
 *****************************************************************************/
sl_status_t sl_bt_cbap_start(sl_bt_cbap_role_t cbap_role,
                             uint8_t connection_handle);

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sli_bt_cbap_on_event(sl_bt_msg_t *evt);

/**************************************************************************//**
 * CBAP Peripheral event handler.
 * @param[in] status Peripheral state
 * @note To be implemented in user code.
 *****************************************************************************/
void sl_bt_cbap_peripheral_on_event(sl_bt_cbap_peripheral_state_t status);

/**************************************************************************//**
 * CBAP Central event handler
 * @param[in] status Central state
 * @note To be implemented in user code.
 *****************************************************************************/
void sl_bt_cbap_central_on_event(sl_bt_cbap_central_state_t status);

/**************************************************************************//**
 * Callback to handle CBAP process errors.
 * @note To be implemented in user code.
 *****************************************************************************/
void sl_bt_on_cbap_error(void);

/**************************************************************************//**
 * Search for a the CBAP Service UUID in scan report.
 *
 * @param[in] scan_data Data received in scanner advertisement report event
 * @param[in] scan_data_len Length of the scan data
 * @return true if the CBAP service is found
 *****************************************************************************/
bool sl_bt_cbap_find_service_in_advertisement(const uint8_t *scan_data,
                                              uint8_t scan_data_len);

#endif // SL_BT_CBAP_H
