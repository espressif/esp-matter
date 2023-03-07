/***************************************************************************//**
 * @file
 * @brief User Interface for throughput test peripheral role.
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
#ifndef THROUGHPUT_PERIPHERAL_H
#define THROUGHPUT_PERIPHERAL_H

#include "throughput_types.h"
#include "sl_power_manager.h"

/*******************************************************************************
 ****************************  PUBLIC DEFINITIONS  *****************************
 ******************************************************************************/
#define THROUGHPUT_PERIPHERAL_CHARACTERISTICS_ALL         0x07

/*******************************************************************************
 ****************************   PUBLIC STRUCTURES  *****************************
 ******************************************************************************/

/// Bitfield for holding characteristic discovery status and result
typedef struct {
  uint8_t indication : 1;
  uint8_t notification : 1;
  uint8_t transmission_on : 1;
} throughput_peripheral_characteristic_t;

/// Union for checking characteristic discovery status and result
typedef union {
  throughput_peripheral_characteristic_t characteristic;
  uint8_t all;
} throughput_peripheral_characteristic_found_t;

/*******************************************************************************
 ****************************   PUBLIC FUNCTIONS   *****************************
 ******************************************************************************/

/**************************************************************************//**
 * Enables the the transmission.
 *****************************************************************************/
void throughput_peripheral_enable(void);

/**************************************************************************//**
 * Sets the the transmission mode.
 * @param[in] mode the transmission mode is either of:
 *          - THROUGHPUT_MODE_CONTINOUS: transfer until stop
 *          - THROUGHPUT_MODE_FIXED_TIME: transfer data for a fixed time
 *          - THROUGHPUT_MODE_FIXED_DATA: a fixed amount of data to transfer
 * @param[in] amount the time in ms or data in bytes to transfer
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_peripheral_set_mode(throughput_mode_t mode,
                                           uint32_t amount);

/**************************************************************************//**
 * Sets the the transmission sizes.
 * @param[in] mtu MTU size in bytes
 * @param[in] ind_data indication data size (1-MTU-3)
 * @param[in] not_data notification data size (1-MTU-3)
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_peripheral_set_data_size(uint8_t mtu,
                                                uint8_t ind_data,
                                                uint8_t not_data);

/**************************************************************************//**
 * Sets the transmission power.
 * @param[in] tx_power requested TX power in dBm
 * @param[in] power_control enable adaptive power control
 * @param[in] deep_sleep enable deep sleep during test
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_peripheral_set_tx_power(throughput_tx_power_t tx_power,
                                               bool power_control,
                                               bool deep_sleep);

/**************************************************************************//**
 * Starts the the transmission.
 * @param[in] type type of the test (notification or indication)
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_peripheral_start(throughput_notification_t type);

/**************************************************************************//**
 * Stops the transmission.
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_peripheral_stop(void);

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void throughput_peripheral_on_bt_event(sl_bt_msg_t *evt);

/**************************************************************************//**
 * Process step for throughput peripheral.
 *****************************************************************************/
void throughput_peripheral_step(void);

/***************************************************************************//**
 * Routine for power manager handler
 * @return SL_POWER_MANAGER_WAKEUP if the test has been started
 ******************************************************************************/
sl_power_manager_on_isr_exit_t throughput_peripheral_sleep_on_isr_exit(void);

/***************************************************************************//**
 * Checks if it is ok to sleep now
 * @return false if the test has been started
 ******************************************************************************/
bool throughput_peripheral_is_ok_to_sleep(void);

/*******************************************************************************
 ******************************   CALLBACKS    *********************************
 ******************************************************************************/

/**************************************************************************//**
 * Callback to handle role settings.
 * @param[in] role the role can be only THROUGHPUT_ROLE_PERIPHERAL
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_role_set(throughput_role_t role);

/**************************************************************************//**
 * Callback to handle state change.
 * @param[in] state current state
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_state_change(throughput_state_t state);

/**************************************************************************//**
 * Callback to handle mode change.
 * @param[in] mode current mode
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_mode_change(throughput_mode_t mode);

/**************************************************************************//**
 * Callback to handle transmission start event.
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_start(void);

/**************************************************************************//**
 * Callback to handle transmission finished event.
 * @param[in] throughput throughput value in bits/second (bps)
 * @param[in] count data volume transmitted, in bytes
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_finish(throughput_value_t throughput,
                                     throughput_count_t count);

/**************************************************************************//**
 * Callback to handle reception finished event.
 * @param[in] throughput throughput value in bits/second (bps)
 * @param[in] count data volume received, in bytes
 * @param[in] lost number of packets lost
 * @param[in] error number of wrong packets
 * @param[in] time total measurement time
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_finish_reception(throughput_value_t throughput,
                                               throughput_count_t count,
                                               throughput_count_t lost,
                                               throughput_count_t error,
                                               throughput_time_t time);

/**************************************************************************//**
 * Callback to handle TX power changed event.
 * @param[in] power TX power in dBm
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_power_change(throughput_tx_power_t power);

/**************************************************************************//**
 * Callback to handle RSSI changed event.
 * @param[in] rssi RSSI value
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_rssi_change(throughput_rssi_t rssi);

/**************************************************************************//**
 * Callback to handle phy changed event.
 * @param[in] phy PHY that is in use
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_phy_change(throughput_phy_t phy);

/**************************************************************************//**
 * Callback to handle connection settings changes.
 * @param[in] interval connection interval
 * @param[in] pdu PDU size in bytes
 * @param[in] mtu MTU size in bytes
 * @param[in] data data size in bytes
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_connection_settings_change(throughput_time_t interval,
                                                         throughput_pdu_size_t pdu,
                                                         throughput_mtu_size_t mtu,
                                                         throughput_data_size_t data);

/**************************************************************************//**
 * Callback to handle notification changed event.
 * @param[in] notification notification status
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_notification_change(throughput_notification_t notification);

/**************************************************************************//**
 * Callback to handle indication changed event.
 * @param[in] indication indication status
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_peripheral_on_indication_change(throughput_notification_t indication);

#endif // THROUGHPUT_PERIPHERAL_H
