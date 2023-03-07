/***************************************************************************//**
 * @file
 * @brief User Interface for throughput test UI.
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

#ifndef THROUGHPUT_UI_H
#define THROUGHPUT_UI_H

#include "throughput_types.h"

/*******************************************************************************
 ******************************   PROTOTYPES   *********************************
 ******************************************************************************/

/**************************************************************************//**
 * @brief
 *   Initilize the the UI.
 *****************************************************************************/
void throughput_ui_init(void);

/**************************************************************************//**
 * @brief
 *   Updates the the UI.
 *****************************************************************************/
void throughput_ui_update(void);

/**************************************************************************//**
 * @brief
 *   Sets the state value on UI.
 *
 * @param[in] state current state
 *****************************************************************************/
void throughput_ui_set_state(throughput_state_t state);

/**************************************************************************//**
 * @brief
 *   Sets the role value on UI.
 *
 * @param[in] role the role can be
 *  - THROUGHPUT_UI_ROLE_PERIPHERAL or
 *  - THROUGHPUT_UI_ROLE_CENTRAL
 *****************************************************************************/
void throughput_ui_set_role(throughput_role_t role);

/**************************************************************************//**
 * @brief
 *   Sets the TX Power value on UI.
 *
 * @param[in] power TX power in dBm
 *****************************************************************************/
void throughput_ui_set_tx_power(throughput_tx_power_t power);

/**************************************************************************//**
 * @brief
 *   Sets the RSSI value on UI.
 *
 * @param[in] power measured RSSI value
 *****************************************************************************/
void throughput_ui_set_rssi(throughput_rssi_t rssi);

/**************************************************************************//**
 * @brief
 *   Sets the connection interval value on UI.
 *
 * @param[in] interval connectio interval in ms
 *****************************************************************************/
void throughput_ui_set_connection_interval(throughput_time_t interval);

/**************************************************************************//**
 * @brief
 *   Sets the PDU size value on UI.
 *
 * @param[in] size PDU size in bytes
 *****************************************************************************/
void throughput_ui_set_pdu_size(throughput_pdu_size_t size);

/**************************************************************************//**
 * @brief
 *   Sets the MTU size value on UI.
 *
 * @param[in] size MTU size in bytes
 *****************************************************************************/
void throughput_ui_set_mtu_size(throughput_mtu_size_t size);

/**************************************************************************//**
 * @brief
 *   Sets the data size value on UI.
 *
 * @param[in] size data size in bytes
 *****************************************************************************/
void throughput_ui_set_data_size(throughput_data_size_t size);

/**************************************************************************//**
 * @brief
 *   Sets the PHY on UI.
 *
 * @param[in] phy can be either of
 *  - sl_bt_gap_phy_coding_1m_uncoded: 1M phy
 *  - sl_bt_gap_phy_coding_2m_uncoded: 2M phy
 *  - sl_bt_gap_phy_coding_500k_coded: Coded phy
 *****************************************************************************/
void throughput_ui_set_phy(throughput_phy_t phy);

/**************************************************************************//**
 * @brief
 *   Sets the notification state on UI.
 *
 * @param[in] notifications settings in CCCD, that can be either of
 *  - sl_bt_gatt_notification: notifications enabled
 *  - sl_bt_gatt_disable: notifications disabled
 *****************************************************************************/
void throughput_ui_set_notifications(throughput_notification_t notifications);

/**************************************************************************//**
 * @brief
 *   Sets the indication state on UI.
 *
 * @param[in] notifications settings in CCCD, that can be either of
 *  - sl_bt_gatt_indication: indications enabled
 *  - sl_bt_gatt_disable: indications disabled
 *****************************************************************************/
void throughput_ui_set_indications(throughput_notification_t indications);

/**************************************************************************//**
 * @brief
 *   Sets the throughput value on UI.
 *
 * @param[in] throughput measured throughput in bits/second (bps)
 *****************************************************************************/
void throughput_ui_set_throughput(throughput_value_t throughput);

/**************************************************************************//**
 * @brief
 *   Sets the counter value on UI.
 *
 * @param[in] count counted bytes
 *****************************************************************************/
void throughput_ui_set_count(throughput_count_t count);

/**************************************************************************//**
 * @brief
 *   Sets all values.
 *
 * @param[in] status throughput status
 *****************************************************************************/
void throughput_ui_set_all(throughput_t status);

#endif // THROUGHPUT_UI_H
