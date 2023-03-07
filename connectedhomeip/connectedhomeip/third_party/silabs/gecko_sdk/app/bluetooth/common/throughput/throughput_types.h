/***************************************************************************//**
 * @file
 * @brief Throughput test type definitions
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
#ifndef THROUGHPUT_TYPES_H
#define THROUGHPUT_TYPES_H

#include "sl_bt_api.h"

/*******************************************************************************
 ********************************  TYPEDEFS  ***********************************
 ******************************************************************************/

/// Role enum type
typedef enum {
  THROUGHPUT_ROLE_PERIPHERAL,
  THROUGHPUT_ROLE_CENTRAL
} throughput_role_t;

/// State enum type
typedef enum {
  THROUGHPUT_STATE_DISCONNECTED,
  THROUGHPUT_STATE_CONNECTED,
  THROUGHPUT_STATE_SUBSCRIBED,
  THROUGHPUT_STATE_TEST,
  THROUGHPUT_STATE_TEST_FINISH
} throughput_state_t;

/// Mode enum type
typedef enum {
  THROUGHPUT_MODE_CONTINUOUS,
  THROUGHPUT_MODE_FIXED_TIME,
  THROUGHPUT_MODE_FIXED_LENGTH
} throughput_mode_t;

/// Discovery state
typedef enum {
  THROUGHPUT_DISCOVERY_STATE_IDLE,
  THROUGHPUT_DISCOVERY_STATE_CONN,
  THROUGHPUT_DISCOVERY_STATE_SCAN,
  THROUGHPUT_DISCOVERY_STATE_SERVICE,
  THROUGHPUT_DISCOVERY_STATE_CHARACTERISTICS,
  THROUGHPUT_DISCOVERY_STATE_FINISHED
} throughput_discovery_state_t;

/// TX power type
typedef int8_t throughput_tx_power_t;
/// RSSI type
typedef int8_t throughput_rssi_t;
/// PDU size type
typedef uint16_t throughput_pdu_size_t;
/// MTU size type
typedef uint16_t throughput_mtu_size_t;
/// Data size type
typedef uint16_t throughput_data_size_t;
/// PHY type
typedef sl_bt_gap_phy_coding_t throughput_phy_t;
/// Notification/indication type
typedef sl_bt_gatt_client_config_flag_t throughput_notification_t;
/// Throughput type
typedef uint32_t throughput_value_t;
/// Data counter type type
typedef uint32_t throughput_count_t;
/// Time type type
typedef uint32_t throughput_time_t;

/// Throughput test allowlist structure
typedef struct allowlist_s {
  bd_addr address;
  sl_bt_gap_address_type_t address_type;
  struct allowlist_s *next;
} throughput_allowlist_t;

/// Throughput test status structure
typedef struct {
  //General parameters
  throughput_role_t role;
  throughput_state_t state;
  throughput_mode_t mode;
  throughput_notification_t test_type;
  // Common parameters
  throughput_tx_power_t tx_power;
  throughput_tx_power_t tx_power_requested;
  throughput_phy_t phy;
  throughput_mtu_size_t mtu_size;
  // Peripheral parameters
  throughput_time_t interval;
  throughput_rssi_t rssi;
  throughput_pdu_size_t pdu_size;
  throughput_data_size_t data_size;
  throughput_notification_t notifications;
  throughput_notification_t indications;
  // Central parameters
  throughput_phy_t scan_phy;
  throughput_time_t connection_interval_min;
  throughput_time_t connection_interval_max;
  throughput_time_t connection_responder_latency;
  throughput_time_t connection_timeout;
  throughput_discovery_state_t discovery_state;
  throughput_notification_t client_conf_flag;
  throughput_allowlist_t allowlist;
  // Results
  throughput_value_t throughput;
  throughput_value_t throughput_peripheral_side;
  throughput_count_t count;
  throughput_count_t packet_error;
  throughput_count_t packet_lost;
  throughput_time_t time;
} throughput_t;

/*******************************************************************************
 ****************************** DEFINITIONS  ***********************************
 ******************************************************************************/
// CLI
#define CLI_RESPONSE(...)                           app_log(__VA_ARGS__)
#define CLI_OK                                      "OK" APP_LOG_NEW_LINE
#define CLI_ERROR                                   "ERROR" APP_LOG_NEW_LINE

#endif // THROUGHPUT_TYPES_H
