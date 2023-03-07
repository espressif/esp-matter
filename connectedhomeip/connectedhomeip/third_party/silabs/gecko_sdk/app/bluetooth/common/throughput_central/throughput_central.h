/***************************************************************************//**
 * @file
 * @brief Throughput test application.
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
#ifndef THROUGHPUT_CENTRAL_H
#define THROUGHPUT_CENTRAL_H

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
#include "sl_power_manager.h"
#endif // SL_CATALOG_POWER_MANAGER_PRESENT
#include "throughput_central_config.h"
#include "throughput_central_system.h"
#include "throughput_types.h"

/*******************************************************************************
 ****************************  PUBLIC DEFINITIONS  *****************************
 ******************************************************************************/
#define THROUGHPUT_CENTRAL_CHARACTERISTICS_ALL         0x0F
#define ADR_LEN 6

/*******************************************************************************
 ****************************   PUBLIC STRUCTURES  *****************************
 ******************************************************************************/

/// Bitfield for holding characteristic discovery status and result
typedef struct {
  uint8_t indication : 1;
  uint8_t notification : 1;
  uint8_t transmission_on : 1;
  uint8_t result : 1;
} throughput_central_characteristic_t;

/// Union for checking characteristic discovery status and result
typedef union {
  throughput_central_characteristic_t characteristic;
  uint8_t all;
} throughput_central_characteristic_found_t;

/*******************************************************************************
 ****************************   PUBLIC FUNCTIONS   *****************************
 ******************************************************************************/

/**************************************************************************//**
 * Add an address to the allowlist.
 * @param[in] address BT address in 6 byte format
 * @return true if adding the address was successful
 *****************************************************************************/
bool throughput_central_allowlist_add(uint8_t *address);

/**************************************************************************//**
 * Clear the allowlist
 * @return true if adding the clear was succesful
 *****************************************************************************/
bool throughput_central_allowlist_clear(void);

/**************************************************************************//**
 * Enable receiver.
 *****************************************************************************/
void throughput_central_enable(void);

/**************************************************************************//**
 * Set receiver mode.
 * @param[in] mode the transmission mode is either of:
 *          - THROUGHPUT_MODE_CONTINUOUS: transfer until stop
 *          - THROUGHPUT_MODE_FIXED_TIME: transfer data for a fixed time
 *          - THROUGHPUT_MODE_FIXED_LENGTH: a fixed amount of data to transfer
 * @param[in] amount the time in ms or data in bytes to transfer
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_central_set_mode(throughput_mode_t mode,
                                        uint32_t amount);

/**************************************************************************//**
 * Set data sizes for reception.
 * @param[in] mtu MTU size in bytes
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_central_set_mtu_size(uint8_t mtu);

/**************************************************************************//**
 * Set transmission power.
 * @param[in] tx_power requested tx power in dBm
 * @param[in] power_control enable adaptive power control
 * @param[in] deep_sleep enable deep sleep during test
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_central_set_tx_power(throughput_tx_power_t tx_power,
                                            bool power_control,
                                            bool deep_sleep);

/**************************************************************************//**
 * Set connection parameters.
 * @param[in] min_interval Minimum connection interval (in 1.25 ms steps)
 * @param[in] max_interval Maximum connection interval (in 1.25 ms steps)
 * @param[in] latency Responder latency (in connection intervals)
 * @param[in] timeout Supervision timeout (in 10 ms steps)
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_central_set_connection_parameters(throughput_time_t min_interval,
                                                         throughput_time_t max_interval,
                                                         throughput_time_t latency,
                                                         throughput_time_t timeout);

/**************************************************************************//**
 * Set type of transmission.
 * @param[in] type type of the test (notification or indication)
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_central_set_type(throughput_notification_t type);

/**************************************************************************//**
 * Start transmission on remote side.
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_central_start(void);

/**************************************************************************//**
 * Stop transmission on remote side.
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_central_stop(void);

/**************************************************************************//**
 * Set PHY used for scanning.
 * @param[in] phy PHY used for scanning
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_central_set_scan_phy(throughput_phy_t phy);

/**************************************************************************//**
 * Set PHY used for connection.
 * @param[in] phy PHY used for the connection
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_central_set_connection_phy(throughput_phy_t phy);

/**************************************************************************//**
 * Change PHY to next one.
 *   - In case of scanning, it is used for changing between CODED and 1M PHY
 *   - In a connection, it is used to change between 1M, 2M and CODED PHYs
 * @return status of the operation
 *****************************************************************************/
sl_status_t throughput_central_change_phy(void);

/**************************************************************************//**
 * Process step for throughput central.
 *****************************************************************************/
void throughput_central_step(void);

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void bt_on_event_central(sl_bt_msg_t *evt);

#ifdef SL_CATALOG_POWER_MANAGER_PRESENT

/***************************************************************************//**
 * Routine for power manager handler
 * @return SL_POWER_MANAGER_WAKEUP if the test has been started
 ******************************************************************************/
sl_power_manager_on_isr_exit_t throughput_central_sleep_on_isr_exit(void);

/***************************************************************************//**
 * Checks if it is ok to sleep now
 * @return false if the test has been started
 ******************************************************************************/
bool throughput_central_is_ok_to_sleep(void);

#endif // SL_CATALOG_POWER_MANAGER_PRESENT

/*******************************************************************************
 ******************************   CALLBACKS    *********************************
 ******************************************************************************/

/**************************************************************************//**
 * Callback to handle role settings.
 * @param[in] role the role can be only THROUGHPUT_ROLE_PERIPHERAL
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_role_set(throughput_role_t role);

/**************************************************************************//**
 * Callback to handle state change.
 * @param[in] state current state
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_state_change(throughput_state_t state);

/**************************************************************************//**
 * Callback to handle mode change.
 * @param[in] mode current mode
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_mode_change(throughput_mode_t mode);

/**************************************************************************//**
 * Callback to handle transmission start event.
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_start(void);

/**************************************************************************//**
 * Callback to handle transmission finished event.
 * @param[in] throughput throughput value in bits/second (bps)
 * @param[in] count data volume transmitted, in bytes
 * @param[in] lost number of packets lost
 * @param[in] error number of wrong packets
 * @param[in] time total measurement time
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_finish(throughput_value_t throughput,
                                  throughput_count_t count,
                                  throughput_count_t lost,
                                  throughput_count_t error,
                                  throughput_time_t time);

/**************************************************************************//**
 * Callback to handle tx power changed event.
 * @param[in] power tx power in dBm
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_transmit_power_change(throughput_tx_power_t power);

/**************************************************************************//**
 * Callback to handle RSSI changed event.
 * @param[in] rssi RSSI value
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_rssi_change(throughput_rssi_t rssi);

/**************************************************************************//**
 * Callback to handle phy changed event.
 * @param[in] phy PHY that is in use
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_phy_change(throughput_phy_t phy);

/**************************************************************************//**
 * Callback to handle notification changed event.
 * @param[in] notification notification status
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_notification_change(throughput_notification_t notification);

/**************************************************************************//**
 * Callback to handle indication changed event.
 * @param[in] indication indication status
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_indication_change(throughput_notification_t indication);

/**************************************************************************//**
 * Callback to handle result indication changed event.
 * @param[in] result result indication status
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_result_indication_change(throughput_notification_t result);

/**************************************************************************//**
 * Callback to handle data size change when receiving data.
 * @param[in] data_size Data size in bytes
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_data_size_change(throughput_pdu_size_t data_size);

/**************************************************************************//**
 * Callback to handle connection parameter changes.
 * @param[in] pdu PDU size in bytes
 * @param[in] mtu MTU size in bytes
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_connection_settings_change(throughput_pdu_size_t pdu,
                                                      throughput_mtu_size_t mtu);

/**************************************************************************//**
 * Callback to handle connection timing changes.
 * @param[in] min_interval Connection interval (in 1.25 ms steps)
 * @param[in] latency Responder latency (in connection intervals)
 * @param[in] timeout Supervision timeout (in 10 ms steps)
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_connection_timings_change(throughput_time_t interval,
                                                     throughput_time_t latency,
                                                     throughput_time_t timeout);

/**************************************************************************//**
 * Callback to handle characteristic found event.
 * @param[in] characteristics Found characteristics
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_characteristics_found(
  throughput_central_characteristic_found_t characteristics);

/**************************************************************************//**
 * Callback to handle discovery state change.
 * @param[in] state State of the discovery
 * @note To be implemented in user code.
 *****************************************************************************/
void throughput_central_on_discovery_state_change(
  throughput_discovery_state_t state);

/**************************************************************************//**
 * Calculate throughput.
 * @param[out] throughput calculated throughput value
 * @return the elapsed time in seconds since measurement started
 *****************************************************************************/
float throughput_central_calculate(throughput_value_t *throughput);

/**************************************************************************//**
 * Convert address string to address data bytes.
 * @param[in] addess_str Address string
 * @param[out] address address byte array
 * @return true if operation was successful
 *****************************************************************************/
bool throughput_central_decode_address(char * addess_str, uint8_t *address);

#endif // THROUGHPUT_CENTRAL_H
