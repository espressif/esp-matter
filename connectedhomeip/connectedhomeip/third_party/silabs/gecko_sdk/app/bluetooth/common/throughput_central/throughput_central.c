/***************************************************************************//**
 * @file
 * @brief Throughput test application - platform interface
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

#include <stdlib.h>
#include <stdio.h>
#include "sl_bt_api.h"
#include "sl_common.h"
#include "app_log.h"
#include "app_assert.h"
#include "throughput_central.h"
#include "throughput_central_interface.h"
#include "throughput_ui_types.h"
#include "throughput_common.h"

// Platform specific includes
#include "throughput_central_system.h"

#define CONFIG_KEY_SET_AFH                               12

// Timeout for result indication in seconds
#define THROUGHPUT_CENTRAL_RESULT_TIMEOUT                1.0f

// Maximum data size
#define THROUGHPUT_CENTRAL_DATA_SIZE_MAX                 255

// Number of remote characteristics
#define THROUGHPUT_CENTRAL_CHARACTERISTICS_COUNT         4

// connection parameters
#define CONN_MIN_CE_LENGTH                          0
#define CONN_MAX_CE_LENGTH                          0x7FFF

// Hardware clock ticks that equal one second
#define HW_TICKS_PER_SECOND                         32768

#define TRANSMISSION_OFF                            0

#define UUID_LEN                                    16

#define CONFIG_TX_POWER_MIN                         -100

/// Enabled state
static bool enabled = false;

/// Finish test indicator
static bool finish_test = false;

/// Data for notification
static uint8_t notification_data[THROUGHPUT_CENTRAL_DATA_SIZE_MAX] = { 0 };

/// Data for indication
static uint8_t indication_data[THROUGHPUT_CENTRAL_DATA_SIZE_MAX] = { 0 };

/// Internal state
static throughput_t central_state = { .allowlist.next = NULL };

/// Bit counter variable
static throughput_count_t bytes_received = 0;

/// Operation (indication, notification) counter variable
static throughput_count_t operation_count = 0;

/// Power control status
static sl_bt_connection_power_reporting_mode_t power_control_enabled
  = sl_bt_connection_power_reporting_disable;

/// Deep sleep enabled
static bool deep_sleep_enabled = THROUGHPUT_CENTRAL_SLEEP_ENABLE;

/// Data size limit for fixed data mode
static uint32_t fixed_data_size = THROUGHPUT_CENTRAL_FIXED_DATA_SIZE;

/// Time limit for fixed time mode
static uint32_t fixed_time = THROUGHPUT_CENTRAL_FIXED_TIME;

/// Finished time
static float finish_time = 0;

/// Throughput calculated after stop
static bool throughput_calculated = false;

/// Counter for checking data
static uint8_t received_counter = 0;

/// Flag for checking counter or accepting remote one
static bool first_packet = true;

// BLE connection handle
static uint8_t connection_handle = 0xFF;
static uint32_t  service_handle = 0xFFFFFFFF;
static uint16_t  notifications_handle = 0xFFFF;
static uint16_t  indications_handle = 0xFFFF;
static uint16_t  transmission_handle = 0xFFFF;
static throughput_central_characteristic_found_t characteristic_found;
static action_t action = act_none;
static uint16_t result_handle = 0xFFFF;

const char *device_name = "Throughput Test"; // Device name to match against scan results.

// bbb99e70-fff7-46cf-abc7-2d32c71820f2
const uint8_t service_uuid[] = { 0xf2, 0x20, 0x18, 0xc7, 0x32, 0x2d, 0xc7, 0xab, 0xcf,
                                 0x46, 0xf7, 0xff, 0x70, 0x9e, 0xb9, 0xbb };
// 6109b631-a643-4a51-83d2-2059700ad49f
const uint8_t indications_characteristic_uuid[] = { 0x9f, 0xd4, 0x0a, 0x70, 0x59, 0x20, 0xd2,
                                                    0x83, 0x51, 0x4a, 0x43, 0xa6, 0x31, 0xb6, 0x09, 0x61 };
// 47b73dd6-dee3-4da1-9be0-f5c539a9a4be
const uint8_t notifications_characteristic_uuid[] = { 0xbe, 0xa4, 0xa9, 0x39, 0xc5, 0xf5, 0xe0, 0x9b,
                                                      0xa1, 0x4d, 0xe3, 0xde, 0xd6, 0x3d, 0xb7, 0x47 };
// be6b6be1-cd8a-4106-9181-5ffe2bc67718
const uint8_t transmission_characteristic_uuid[] = { 0x18, 0x77, 0xc6, 0x2b, 0xfe, 0x5f, 0x81, 0x91,
                                                     0x06, 0x41, 0x8a, 0xcd, 0xe1, 0x6b, 0x6b, 0xbe };
//adf32227-b00f-400c-9eeb-b903a6cc291b
const uint8_t result_characteristic_uuid[] = { 0x1b, 0x29, 0xcc, 0xa6, 0x03, 0xb9, 0xeb, 0x9e,
                                               0x0c, 0x40, 0x0f, 0xb0, 0x27, 0x22, 0xf3, 0xad };

// Function declarations
static void handle_scan_event(bd_addr *address,
                              uint8_t address_type,
                              uint8_t * data,
                              uint16_t len);
static bool process_scan_response(uint8_t *data, uint16_t data_len);
static void process_procedure_complete_event(sl_bt_msg_t *evt);
static void check_characteristic_uuid(sl_bt_msg_t *evt);
static void reset_variables(void);
static void check_received_data(uint8_t * data, uint8_t len);
static void handle_throughput_central_stop(bool send_transmission_on);
static void handle_throughput_central_start(bool send_transmission_on);
static void throughput_central_scanning_restart(void);
static void throughput_central_scanning_start(void);
static void throughput_central_scanning_stop(void);
static sl_status_t throughput_central_scanning_apply_phy(throughput_phy_t phy);
static bool throughput_central_allowlist_apply();
static bool throughput_address_compare(uint8_t *address1, uint8_t *address2);

/**************************************************************************//**
 * Event handler for timer
 *****************************************************************************/
void timer_on_refresh_rssi(void)
{
  sl_status_t sc;
  if (connection_handle != 0xFF  && central_state.state != THROUGHPUT_STATE_TEST) {
    sc = sl_bt_connection_get_rssi(connection_handle);
    app_assert_status(sc);
  }
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void bt_on_event_central(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  static throughput_t results;

  // If the component is not enabled do not handle events
  if (!enabled) {
    return;
  }

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_scanner_legacy_advertisement_report_id:
      // If the device is connectable and scannable
      if (evt->data.evt_scanner_legacy_advertisement_report.event_flags
          & (SL_BT_SCANNER_EVENT_FLAG_CONNECTABLE | SL_BT_SCANNER_EVENT_FLAG_SCANNABLE)) {
        handle_scan_event(&evt->data.evt_scanner_legacy_advertisement_report.address,
                          evt->data.evt_scanner_legacy_advertisement_report.address_type,
                          evt->data.evt_scanner_legacy_advertisement_report.data.data,
                          evt->data.evt_scanner_legacy_advertisement_report.data.len);
      }
      break;
    case sl_bt_evt_scanner_extended_advertisement_report_id:
      // If the device is connectable, scannable and the data is complete
      if ((evt->data.evt_scanner_extended_advertisement_report.event_flags
           & (SL_BT_SCANNER_EVENT_FLAG_CONNECTABLE | SL_BT_SCANNER_EVENT_FLAG_SCANNABLE))
          && (evt->data.evt_scanner_extended_advertisement_report.data_completeness
              == sl_bt_scanner_data_status_complete)) {
        handle_scan_event(&evt->data.evt_scanner_extended_advertisement_report.address,
                          evt->data.evt_scanner_extended_advertisement_report.address_type,
                          evt->data.evt_scanner_extended_advertisement_report.data.data,
                          evt->data.evt_scanner_extended_advertisement_report.data.len);
      }
      break;
    case sl_bt_evt_connection_opened_id:
      connection_handle = evt->data.evt_connection_opened.connection;
      // Set remote connection power reporting - needed for Power Control
      sc = sl_bt_connection_set_remote_power_reporting(connection_handle,
                                                       power_control_enabled);
      app_assert_status(sc);

      central_state.state = THROUGHPUT_STATE_CONNECTED;
      throughput_central_on_state_change(central_state.state);

      central_state.discovery_state = THROUGHPUT_DISCOVERY_STATE_SERVICE;
      throughput_central_on_discovery_state_change(central_state.discovery_state);

      sc = sl_bt_gatt_discover_primary_services_by_uuid(connection_handle,
                                                        UUID_LEN,
                                                        service_uuid);
      app_assert_status(sc);
      break;

    case sl_bt_evt_connection_parameters_id:
      central_state.interval = evt->data.evt_connection_parameters.interval;
      central_state.connection_responder_latency = evt->data.evt_connection_parameters.latency;
      central_state.connection_timeout = evt->data.evt_connection_parameters.timeout;
      central_state.pdu_size = evt->data.evt_connection_parameters.txsize;

      throughput_central_on_connection_timings_change(central_state.interval,
                                                      central_state.connection_responder_latency,
                                                      central_state.connection_timeout);

      throughput_central_on_connection_settings_change(central_state.pdu_size,
                                                       central_state.mtu_size);
      break;

    case sl_bt_evt_gatt_procedure_completed_id:
      process_procedure_complete_event(evt);
      break;

    case sl_bt_evt_gatt_characteristic_id:
      check_characteristic_uuid(evt);
      break;

    case sl_bt_evt_gatt_service_id:
      if (evt->data.evt_gatt_service.uuid.len == UUID_LEN) {
        if (memcmp(service_uuid, evt->data.evt_gatt_service.uuid.data, UUID_LEN) == 0) {
          service_handle = evt->data.evt_gatt_service.service;
          action = act_discover_service;
        }
      }
      break;

    case sl_bt_evt_gatt_characteristic_value_id:
      if (evt->data.evt_gatt_characteristic_value.characteristic == transmission_handle) {
        if (evt->data.evt_gatt_characteristic_value.value.data[0]) {
          handle_throughput_central_start(false);
        } else {
          finish_test = true;
        }
      } else if (evt->data.evt_gatt_characteristic_value.characteristic == result_handle) {
        if (evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_indication) {
          sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);
          // Responder sends indication about result after each test. Data is uint8array LSB first.
          memcpy(&results.throughput_peripheral_side, evt->data.evt_gatt_characteristic_value.value.data, 4);
          handle_throughput_central_stop(false);
        }
        break;
      }

      if (evt->data.evt_gatt_characteristic_value.characteristic == indications_handle
          || evt->data.evt_gatt_characteristic_value.characteristic == notifications_handle) {
        // Send confirmation if needed
        if (evt->data.evt_gatt_characteristic_value.characteristic == indications_handle) {
          if (evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_indication) {
            sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);
          }
        }
        // Check data for loss or error
        check_received_data(evt->data.evt_gatt_characteristic_value.value.data,
                            evt->data.evt_gatt_characteristic_value.value.len);
        bytes_received += (evt->data.evt_gatt_characteristic_value.value.len);
        if (central_state.data_size != evt->data.evt_gatt_characteristic_value.value.len) {
          central_state.data_size = evt->data.evt_gatt_characteristic_value.value.len;
          throughput_central_on_data_size_change(central_state.data_size);
        }
        operation_count++;
        // Fixed data mode
        if (central_state.mode == THROUGHPUT_MODE_FIXED_LENGTH && bytes_received >= (fixed_data_size)) {
          finish_test = true;
        }
      }
      break;

    case sl_bt_evt_gatt_mtu_exchanged_id:
      central_state.mtu_size = evt->data.evt_gatt_mtu_exchanged.mtu;
      throughput_central_on_connection_settings_change(central_state.pdu_size,
                                                       central_state.mtu_size);
      break;

    case sl_bt_evt_connection_phy_status_id:
      central_state.phy = (throughput_phy_t)evt->data.evt_connection_phy_status.phy;
      throughput_central_on_phy_change(central_state.phy);
      break;

    case sl_bt_evt_connection_closed_id:
      // Stop RSSI refresh timer
      timer_refresh_rssi_stop();
      // Notify state change
      central_state.state = THROUGHPUT_STATE_DISCONNECTED;
      throughput_central_on_state_change(central_state.state);
      central_state.discovery_state = THROUGHPUT_DISCOVERY_STATE_IDLE;
      throughput_central_on_discovery_state_change(central_state.discovery_state);
      // Reset variables
      reset_variables();
      // Start scanning
      throughput_central_scanning_start();
      break;

    case sl_bt_evt_connection_rssi_id:
      central_state.rssi = evt->data.evt_connection_rssi.rssi;
      throughput_central_on_rssi_change(central_state.rssi);
      break;

    default:
      break;
  }
}
/***************************************************************************//**
 * Check received data for lost or error packages.
 * @param[in] data received data
 * @param[in] len length of the data
 ******************************************************************************/
static void check_received_data(uint8_t * data, uint8_t len)
{
  uint8_t counter;

  if (len == 0) {
    central_state.packet_error++;
    return;
  }

  counter = data[0];

  if (first_packet) {
    // Accept remote counter
    received_counter = counter;
    first_packet = false;
  } else {
    // Increment local counter
    received_counter = (received_counter + 1) % 100;
  }
  if (counter != received_counter) {
    // Lost packet is found
    uint8_t lost_packet_count = 0;
    // Calculate lost packet count
    if (counter > received_counter) {
      lost_packet_count = counter - received_counter;
    } else if (counter < received_counter) {
      lost_packet_count = 100 + counter - received_counter;
    }
    central_state.packet_lost += lost_packet_count;
    // Sync to remote counter
    received_counter = counter;
  }

  // Check data for bit errors
  for (int i = 1; i < len; i++) {
    if ( data[i] != (uint8_t) 'a' + (uint8_t) ((i - 1) % 26) ) {
      central_state.packet_error++;
      break;
    }
  }
}

static void handle_scan_event(bd_addr *address,
                              uint8_t address_type,
                              uint8_t * data,
                              uint16_t len)
{
  sl_status_t sc;

  if ((central_state.discovery_state == THROUGHPUT_DISCOVERY_STATE_SCAN)
      & process_scan_response(data, len)) {
    // Apply allowlist filtering
    if (false == throughput_central_allowlist_apply(address->addr)) {
      return;
    }

    // Stop scanning
    app_log_info("Scanning stop." APP_LOG_NL);
    sc = sl_bt_scanner_stop();
    app_assert_status(sc);

    // Open the connection
    central_state.discovery_state = THROUGHPUT_DISCOVERY_STATE_CONN;
    throughput_central_on_discovery_state_change(central_state.discovery_state);

    sc = sl_bt_connection_open(*address,
                               address_type,
                               central_state.phy,
                               &connection_handle);

    // Handle if the default PHY is not supported
    if (sc == SL_STATUS_INVALID_PARAMETER) {
      app_log_status_warning_f(sc, "Connection PHY is not supported and set to 1M PHY" APP_LOG_NEW_LINE);

      central_state.phy = sl_bt_gap_phy_coding_1m_uncoded;
      sc = sl_bt_connection_open(*address,
                                 address_type,
                                 central_state.phy,
                                 &connection_handle);
    }
    // Assertion to first or second attempt to connect
    app_assert_status(sc);
  } else {
    waiting_indication();
  }
}

// Cycle through advertisement contents and look for matching device name.
static bool process_scan_response(uint8_t *data, uint16_t data_len)
{
  int i = 0;
  bool device_name_match = false;
  uint8_t advertisement_length;
  uint8_t advertisement_type;

  while (i < (data_len - 1)) {
    advertisement_length = data[i];
    advertisement_type = data[i + 1];

    /* Type 0x09 = Complete Local Name, 0x08 Shortened Name */
    if (advertisement_type == 0x09) {
      /* Check if device name is Throughput Tester */
      if (memcmp(data + i + 2, device_name, strlen(device_name)) == 0) {
        device_name_match = true;
        break;
      }
    }
    /* Jump to next AD record */
    i = i + advertisement_length + 1;
  }

  return (device_name_match);
}

// Helper function to make the discovery and subscribing flow correct.
// Action enum values indicate which procedure was completed.
static void process_procedure_complete_event(sl_bt_msg_t *evt)
{
  uint16_t procedure_result =  evt->data.evt_gatt_procedure_completed.result;
  sl_status_t sc;

  switch (action) {
    case act_discover_service:
      action = act_none;
      app_assert_status(procedure_result);
      if (!procedure_result) {
        // Discover successful, start characteristic discovery.
        sc = sl_bt_gatt_discover_characteristics(connection_handle, service_handle);
        app_assert_status(sc);
        action = act_discover_characteristics;
        central_state.discovery_state = THROUGHPUT_DISCOVERY_STATE_CHARACTERISTICS;
        throughput_central_on_discovery_state_change(central_state.discovery_state);
      }
      break;
    case act_discover_characteristics:
      action = act_none;
      app_assert_status(procedure_result);
      if (!procedure_result) {
        if (characteristic_found.all == THROUGHPUT_CENTRAL_CHARACTERISTICS_ALL) {
          central_state.discovery_state = THROUGHPUT_DISCOVERY_STATE_FINISHED;
          throughput_central_on_discovery_state_change(central_state.discovery_state);
          sc = sl_bt_gatt_set_characteristic_notification(connection_handle, notifications_handle, sl_bt_gatt_notification);
          app_assert_status(sc);
          action = act_enable_notification;
        }
      }
      break;
    case act_enable_notification:
      action = act_none;
      app_assert_status(procedure_result);
      if (!procedure_result) {
        // Notifications turned on, turn on indication
        central_state.notifications = sl_bt_gatt_notification;
        throughput_central_on_notification_change(central_state.notifications);
        sl_bt_gatt_set_characteristic_notification(connection_handle, indications_handle, sl_bt_gatt_indication);
        action = act_enable_indication;
      }
      break;
    case act_enable_indication:
      action = act_enable_indication;
      app_assert_status(procedure_result);
      if (!procedure_result) {
        // Subscribe to peripheral result.
        central_state.indications = sl_bt_gatt_indication;
        throughput_central_on_indication_change(central_state.indications);
        sc = sl_bt_gatt_set_characteristic_notification(connection_handle, transmission_handle, sl_bt_gatt_notification);
        app_assert_status(sc);
        action = act_enable_transmission_notification;
      }
      break;
    case act_enable_transmission_notification:
      action = act_none;
      app_assert_status(procedure_result);
      if (!procedure_result) {
        // Subscribe to peripheral result.
        sc = sl_bt_gatt_set_characteristic_notification(connection_handle, result_handle, sl_bt_gatt_indication);
        app_assert_status(sc);
        action = act_subscribe_result;
      }
      break;
    case act_subscribe_result:
      action = act_none;
      app_assert_status(procedure_result);
      if (!procedure_result) {
        central_state.state = THROUGHPUT_STATE_SUBSCRIBED;
        throughput_central_on_state_change(central_state.state);
        // Start RSSI refresh timer
        timer_refresh_rssi_start();
      }
      break;
    case act_none:
      break;
    default:
      break;
  }
}

// Check if found characteristic matches the UUIDs that we are searching for.
static void check_characteristic_uuid(sl_bt_msg_t *evt)
{
  if (evt->data.evt_gatt_characteristic.uuid.len == UUID_LEN) {
    if (memcmp(notifications_characteristic_uuid, evt->data.evt_gatt_characteristic.uuid.data, UUID_LEN) == 0) {
      notifications_handle = evt->data.evt_gatt_characteristic.characteristic;
      characteristic_found.characteristic.notification = true;
      throughput_central_on_characteristics_found(characteristic_found);
    } else if (memcmp(indications_characteristic_uuid, evt->data.evt_gatt_characteristic.uuid.data, UUID_LEN) == 0) {
      indications_handle = evt->data.evt_gatt_characteristic.characteristic;
      characteristic_found.characteristic.indication = true;
      throughput_central_on_characteristics_found(characteristic_found);
    } else if (memcmp(transmission_characteristic_uuid, evt->data.evt_gatt_characteristic.uuid.data, UUID_LEN) == 0) {
      transmission_handle = evt->data.evt_gatt_characteristic.characteristic;
      characteristic_found.characteristic.transmission_on = true;
      throughput_central_on_characteristics_found(characteristic_found);
    } else if (memcmp(result_characteristic_uuid, evt->data.evt_gatt_characteristic.uuid.data, UUID_LEN) == 0) {
      result_handle = evt->data.evt_gatt_characteristic.characteristic;
      characteristic_found.characteristic.result = true;
      throughput_central_on_characteristics_found(characteristic_found);
    }
  }
}

static void reset_variables(void)
{
  connection_handle = 0xFF;
  service_handle = 0xFFFFFFFF;
  notifications_handle = 0xFFFF;
  indications_handle = 0xFFFF;
  transmission_handle = 0xFFFF;
  characteristic_found.all = 0;

  bytes_received = 0;
  operation_count = 0;

  received_counter = 0;
  first_packet = true;

  central_state.notifications = sl_bt_gatt_disable;
  central_state.indications = sl_bt_gatt_disable;

  central_state.throughput = 0;
  central_state.throughput_peripheral_side = 0;
  central_state.count = 0;
  central_state.packet_error = 0;
  central_state.packet_lost = 0;
}

bool throughput_central_decode_address(char * addess_str, uint8_t *address)
{
  (void) addess_str;
  (void) address;
  int retval;
  unsigned int address_cache[ADR_LEN];
  uint8_t i;

  retval = sscanf(addess_str, "%2X:%2X:%2X:%2X:%2X:%2X",
                  &address_cache[5],
                  &address_cache[4],
                  &address_cache[3],
                  &address_cache[2],
                  &address_cache[1],
                  &address_cache[0]);

  if (retval == ADR_LEN) {
    for (i = 0; i < ADR_LEN; i++) {
      address[i] = (uint8_t)(address_cache[i]);
    }
    return true;
  } else {
    return false;
  }
}

bool throughput_central_allowlist_clear(void)
{
  throughput_allowlist_t *current_entry;
  throughput_allowlist_t *entry_cache;

  current_entry = central_state.allowlist.next;

  while (current_entry != NULL) {
    entry_cache = current_entry->next;
    free(current_entry);
    current_entry = entry_cache;
  }

  central_state.allowlist.next = NULL;
  return true;
}

bool throughput_central_allowlist_add(uint8_t *address)
{
  throughput_allowlist_t *current_entry;
  throughput_allowlist_t *next_entry;
  bool ret_val = false;
  uint8_t addr_type = 0;

  current_entry = &central_state.allowlist;

  //Search for the last entry in the list
  while (current_entry != NULL) {
    // break if the address is already in the list
    if (throughput_address_compare(current_entry->address.addr, address)) {
      break;
    }
    if (current_entry->next == NULL) { //This is the last entry in the list
      //Allocate memory for the next one.
      next_entry = malloc(sizeof(throughput_allowlist_t));
      if (next_entry == NULL) {
        ret_val = false;
      } else {
        //Set the pointer to the next entry
        current_entry->next = next_entry;
        //Add the new address
        app_log_warning("Adding address to the allowlist %02X:%02X:%02X:%02X:%02X:%02X\n",
                        address[5],
                        address[4],
                        address[3],
                        address[2],
                        address[1],
                        address[0]);
        memcpy(&current_entry->address.addr, address, 6);
        current_entry->address_type = (sl_bt_gap_address_type_t)addr_type;
        next_entry->next = NULL;
        ret_val = true;
      }
      break;
    } else {
      current_entry = current_entry->next;
    }
  }
  return ret_val;
}

static bool throughput_central_allowlist_apply(uint8_t *address)
{
  bool ret_val = false;
  throughput_allowlist_t *current_entry = &central_state.allowlist;

  if (current_entry->next == NULL) {
    ret_val = true;
  }

  //Search for the last entry in the list
  while (current_entry->next != NULL) {
    if (true == throughput_address_compare(current_entry->address.addr, address)) {
      ret_val = true;
      break;
    }
    current_entry = current_entry->next;
  }

  return ret_val;
}

bool throughput_address_compare(uint8_t *address1, uint8_t *address2)
{
  bool ret_val = false;

  if (memcmp(address1, address2, ADR_LEN) == 0) {
    ret_val = true;
  }

  return ret_val;
}

// Stop scanning
void throughput_central_scanning_stop(void)
{
  sl_status_t sc;
  if (central_state.discovery_state == THROUGHPUT_DISCOVERY_STATE_SCAN) {
    app_log_info("Scanning stop." APP_LOG_NL);
    sc = sl_bt_scanner_stop();
    app_assert_status(sc);
    central_state.discovery_state = THROUGHPUT_DISCOVERY_STATE_IDLE;
    throughput_central_on_discovery_state_change(central_state.discovery_state);
  }
}

// Apply phy for scanning
sl_status_t throughput_central_scanning_apply_phy(throughput_phy_t phy)
{
  throughput_central_scanning_stop();
  central_state.scan_phy = phy;
  throughput_central_scanning_start();
  return SL_STATUS_OK;
}

// Start scanning
void throughput_central_scanning_start(void)
{
  sl_status_t sc;
  int16_t tx_power_min, tx_power_max;

  // if the power is greater than 10 dBm AFH must be used
  uint32_t afh_bit = (central_state.tx_power_requested > 10);
  sc = sl_bt_system_linklayer_configure(CONFIG_KEY_SET_AFH,
                                        sizeof(afh_bit),
                                        (uint8_t *)&afh_bit);
  app_assert_status(sc);

  app_log_info("Scanning started..." APP_LOG_NEW_LINE);

  // Reset found characteristics
  characteristic_found.all = 0;

  // Reset discovery state
  central_state.discovery_state = THROUGHPUT_DISCOVERY_STATE_SCAN;
  throughput_central_on_discovery_state_change(central_state.discovery_state);

  // Convert power to mdBm
  int16_t power = ( ((int16_t)central_state.tx_power_requested) * 10);
  sc = sl_bt_system_set_tx_power(CONFIG_TX_POWER_MIN,
                                 power,
                                 &tx_power_min,
                                 &tx_power_max);
  app_assert_status(sc);
  central_state.tx_power = tx_power_max / 10;
  throughput_central_on_transmit_power_change(central_state.tx_power);

  sc = sl_bt_gatt_server_set_max_mtu(central_state.mtu_size, &(central_state.mtu_size));
  app_assert_status(sc);

  // Set the default connection parameters for subsequent connections
  sc = sl_bt_connection_set_default_parameters(central_state.connection_interval_min,
                                               central_state.connection_interval_max,
                                               central_state.connection_responder_latency,
                                               central_state.connection_timeout,
                                               CONN_MIN_CE_LENGTH,
                                               CONN_MAX_CE_LENGTH);
  app_assert_status(sc);

  // Start scanning - looking for peripheral devices
  sc = sl_bt_scanner_start(central_state.scan_phy, sl_bt_scanner_discover_generic);
  if (sc != SL_STATUS_OK) {
    central_state.scan_phy = sl_bt_gap_phy_coding_1m_uncoded;
    app_log_warning("Requested scanning PHY is not supported and set to 1M PHY" APP_LOG_NEW_LINE);
    // Start scanning with the modified PHY
    sc = sl_bt_scanner_start(central_state.scan_phy, sl_bt_scanner_discover_generic);
  }
  app_assert_status(sc);
}

float throughput_central_calculate(throughput_value_t *throughput)
{
  float time_elapsed;

  time_elapsed = timer_end();
  central_state.time = (throughput_time_t)time_elapsed;
  central_state.throughput = (throughput_value_t)((float)bytes_received
                                                  * 8
                                                  / time_elapsed);

  if (throughput != NULL) {
    *throughput = central_state.throughput;
  }

  return time_elapsed;
}

// Finish reception
void handle_throughput_central_stop(bool send_transmission_on)
{
  sl_status_t sc;
  uint8_t value = TRANSMISSION_OFF;
  uint16_t sent_len;
  float time_elapsed = 0.0f;

  // Calculate throughput
  if (!throughput_calculated) {
    time_elapsed = throughput_central_calculate(NULL);
    throughput_calculated = true;
  }

  if (send_transmission_on) {
    finish_time = time_elapsed;

    // Triggers the data transmission end on remote
    sc = sl_bt_gatt_write_characteristic_value_without_response(connection_handle,
                                                                transmission_handle,
                                                                1,
                                                                &value,
                                                                &sent_len);
    app_assert_status(sc);
  } else {
    finish_test = false;
    finish_time = 0;

    #ifdef SL_CATALOG_POWER_MANAGER_PRESENT
    if (!deep_sleep_enabled) {
      // Enable sleep
      sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
    }
    #endif

    central_state.count = operation_count;
    throughput_central_on_finish(central_state.throughput,
                                 central_state.count,
                                 central_state.packet_lost,
                                 central_state.packet_error,
                                 central_state.time);
    central_state.state = THROUGHPUT_STATE_SUBSCRIBED;
    throughput_central_on_state_change(central_state.state);

    // Start RSSI refresh timer
    timer_refresh_rssi_start();
  }
}

// Start reception
void handle_throughput_central_start(bool send_transmission_on)
{
  uint8_t value;
  uint16_t sent_len;
  sl_status_t sc;

  // Set test type
  value = central_state.test_type;

  // Clear results
  central_state.throughput = 0;
  central_state.count = 0;
  central_state.packet_error = 0;
  central_state.packet_lost = 0;

  // Clear counters
  bytes_received = 0;
  operation_count = 0;

  received_counter = 0;
  first_packet = true;

  throughput_calculated = false;

  // Stop RSSI refresh timer
  timer_refresh_rssi_stop();

  // Manage power
  #ifdef SL_CATALOG_POWER_MANAGER_PRESENT
  if (!deep_sleep_enabled) {
    // Disable sleep
    sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
  }
  #endif

  if (send_transmission_on) {
    // This triggers the data transmission on remote side
    sc = sl_bt_gatt_write_characteristic_value_without_response(connection_handle,
                                                                transmission_handle,
                                                                1,
                                                                &value,
                                                                &sent_len);
    app_assert_status(sc);
  }

  // Set state and call back
  central_state.state = THROUGHPUT_STATE_TEST;
  throughput_central_on_state_change(central_state.state);
  throughput_central_on_start();

  // Start timer
  timer_start();
}

// Restart scanning
void throughput_central_scanning_restart(void)
{
  sl_status_t sc;

  bool close_required =  (central_state.state != THROUGHPUT_STATE_DISCONNECTED);

  if (!enabled) {
    return;
  }
  if (close_required) {
    sc = sl_bt_connection_close(connection_handle);
    app_assert_status(sc);
  } else {
    throughput_central_scanning_stop();
  }

  memset(notification_data, 0, THROUGHPUT_CENTRAL_DATA_SIZE_MAX);
  memset(indication_data, 0, THROUGHPUT_CENTRAL_DATA_SIZE_MAX);

  central_state.state         = THROUGHPUT_STATE_DISCONNECTED;
  central_state.client_conf_flag = sl_bt_gatt_disable;

  // Clear results
  central_state.throughput    = 0;
  central_state.count         = 0;
  central_state.packet_error  = 0;
  central_state.packet_lost   = 0;

  reset_variables();

  if (!close_required) {
    throughput_central_scanning_start();
  }

  // Notify application
  throughput_central_on_state_change(central_state.state);
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_all(central_state);
  #endif // SL_CATALOG_THROUGHPUT_UI_PRESENT
}

/**************************************************************************//**
 * Process step for throughput central.
 *****************************************************************************/
void throughput_central_step(void)
{
  if (enabled && central_state.state == THROUGHPUT_STATE_TEST) {
    if (central_state.mode == THROUGHPUT_MODE_FIXED_TIME) {
      if ( timer_end() >=  fixed_time ) {
        finish_test = true;
      }
    }
    // Test should be finished
    if (finish_test) {
      if (finish_time == 0) {
        handle_throughput_central_stop(true);
      } else {
        // Check timeout for result
        if ( (timer_end() - finish_time) > THROUGHPUT_CENTRAL_RESULT_TIMEOUT ) {
          handle_throughput_central_stop(false);
        }
      }
    }
  }
}

/**************************************************************************//**
 * Set receiver mode.
 *****************************************************************************/
sl_status_t throughput_central_set_mode(throughput_mode_t mode,
                                        uint32_t amount)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && central_state.state != THROUGHPUT_STATE_TEST) {
    if (mode == THROUGHPUT_MODE_FIXED_LENGTH) {
      fixed_data_size = amount;
    } else if (mode == THROUGHPUT_MODE_FIXED_TIME) {
      fixed_time = amount;
    }
    central_state.mode = mode;
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Set data sizes for reception.
 *****************************************************************************/
sl_status_t throughput_central_set_mtu_size(uint8_t mtu)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && central_state.state != THROUGHPUT_STATE_TEST) {
    central_state.mtu_size = mtu;
    res = sl_bt_gatt_server_set_max_mtu(central_state.mtu_size, &(central_state.mtu_size));
    throughput_central_on_connection_settings_change(central_state.pdu_size,
                                                     central_state.mtu_size);
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Set transmission power.
 *****************************************************************************/
sl_status_t throughput_central_set_tx_power(throughput_tx_power_t tx_power,
                                            bool power_control,
                                            bool deep_sleep)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && central_state.state != THROUGHPUT_STATE_TEST) {
    central_state.tx_power_requested = tx_power;
    power_control_enabled = (sl_bt_connection_power_reporting_mode_t)power_control;
    deep_sleep_enabled = deep_sleep;
    throughput_central_scanning_restart();
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Set connection parameters.
 *****************************************************************************/
sl_status_t throughput_central_set_connection_parameters(throughput_time_t min_interval,
                                                         throughput_time_t max_interval,
                                                         throughput_time_t latency,
                                                         throughput_time_t timeout)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled
      && (central_state.state == THROUGHPUT_STATE_SUBSCRIBED
          || central_state.state == THROUGHPUT_STATE_CONNECTED)) {
    central_state.connection_interval_min = min_interval;
    central_state.connection_interval_max = max_interval;
    central_state.connection_responder_latency = latency;
    central_state.connection_timeout = timeout;

    // Set connection parameters for this connection.
    res = sl_bt_connection_set_parameters(connection_handle,
                                          central_state.connection_interval_min,
                                          central_state.connection_interval_max,
                                          central_state.connection_responder_latency,
                                          central_state.connection_timeout,
                                          CONN_MIN_CE_LENGTH,
                                          CONN_MAX_CE_LENGTH);
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Set type of transmission.
 *****************************************************************************/
sl_status_t throughput_central_set_type(throughput_notification_t type)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && central_state.state != THROUGHPUT_STATE_TEST) {
    central_state.test_type = type;
    if ( (central_state.test_type != sl_bt_gatt_indication)
         && (central_state.test_type != sl_bt_gatt_notification) ) {
      res = SL_STATUS_INVALID_TYPE;
    }
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Start transmission on remote side.
 *****************************************************************************/
sl_status_t throughput_central_start(void)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && central_state.state == THROUGHPUT_STATE_SUBSCRIBED) {
    handle_throughput_central_start(true);
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Stop transmission on remote side.
 *****************************************************************************/
sl_status_t throughput_central_stop(void)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && central_state.state == THROUGHPUT_STATE_TEST) {
    finish_test = true;
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Set PHY used for scanning.
 *****************************************************************************/
sl_status_t throughput_central_set_scan_phy(throughput_phy_t phy)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && central_state.state == THROUGHPUT_STATE_DISCONNECTED) {
    res = throughput_central_scanning_apply_phy(phy);
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Set PHY used for the connection.
 *****************************************************************************/
sl_status_t throughput_central_set_connection_phy(throughput_phy_t phy)
{
  sl_status_t res = SL_STATUS_INVALID_STATE;
  uint8_t accepted_phy = (uint8_t)phy;
  if (enabled
      && (central_state.state == THROUGHPUT_STATE_CONNECTED
          || central_state.state == THROUGHPUT_STATE_SUBSCRIBED) ) {
    if (phy == sl_bt_gap_phy_coding_500k_coded) {
      accepted_phy = sl_bt_gap_phy_coded;
    }
    res = sl_bt_connection_set_preferred_phy(connection_handle,
                                             phy,
                                             accepted_phy);
  }
  return res;
}

/**************************************************************************//**
 * Change PHY to next one.
 *****************************************************************************/
sl_status_t throughput_central_change_phy(void)
{
  sl_status_t res = SL_STATUS_INVALID_STATE;
  throughput_phy_t current_phy;
  if (enabled) {
    if ((central_state.state == THROUGHPUT_STATE_CONNECTED
         || central_state.state == THROUGHPUT_STATE_SUBSCRIBED) ) {
      // If connected
      current_phy = central_state.phy;
      switch (current_phy) {
        case sl_bt_gap_phy_coding_1m_uncoded:
          res = throughput_central_set_connection_phy(sl_bt_gap_phy_coding_2m_uncoded);
          // if cannot switch to 2M, switch to 1M
          if (res != SL_STATUS_OK) {
            res = throughput_central_set_connection_phy(sl_bt_gap_phy_coding_1m_uncoded);
          }
          break;
        case sl_bt_gap_phy_coding_2m_uncoded:
          res = throughput_central_set_connection_phy(sl_bt_gap_phy_coding_125k_coded);
          // if cannot switch to coded, switch to 1M
          if (res != SL_STATUS_OK) {
            res = throughput_central_set_connection_phy(sl_bt_gap_phy_coding_1m_uncoded);
          }
          break;
        case sl_bt_gap_phy_coding_125k_coded:
          res = throughput_central_set_connection_phy(sl_bt_gap_phy_coding_500k_coded);
          // if cannot switch to coded, switch to 1M
          if (res != SL_STATUS_OK) {
            res = throughput_central_set_connection_phy(sl_bt_gap_phy_coding_1m_uncoded);
          }
          break;
        case sl_bt_gap_phy_coding_500k_coded:
          res = throughput_central_set_connection_phy(sl_bt_gap_phy_coding_1m_uncoded);
          break;
        default:
          res = throughput_central_set_connection_phy(sl_bt_gap_phy_coding_1m_uncoded);
          break;
      }
    } else if (central_state.state == THROUGHPUT_STATE_DISCONNECTED) {
      // if disconnected
      current_phy = central_state.scan_phy;
      if (current_phy == sl_bt_gap_phy_coding_1m_uncoded) {
        res = throughput_central_set_scan_phy(sl_bt_gap_phy_coding_125k_coded);
      } else {
        res = throughput_central_set_scan_phy(sl_bt_gap_phy_coding_1m_uncoded);
      }
    }
  }
  return res;
}

/**************************************************************************//**
 * Enable receiver.
 *****************************************************************************/
void throughput_central_enable(void)
{
  sl_status_t sc;
  int16_t tx_power_min, tx_power_max;
  uint8_t address[ADR_LEN];
  uint32_t afh_bit;

  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_init();
  #endif // SL_CATALOG_THROUGHPUT_UI_PRESENT

  memset(notification_data, 0, THROUGHPUT_CENTRAL_DATA_SIZE_MAX);
  memset(indication_data, 0, THROUGHPUT_CENTRAL_DATA_SIZE_MAX);

  central_state.role          = THROUGHPUT_ROLE_CENTRAL;
  central_state.state         = THROUGHPUT_STATE_DISCONNECTED;

  central_state.test_type     = THROUGHPUT_CENTRAL_TEST_TYPE;

  central_state.mode          = THROUGHPUT_CENTRAL_MODE_DEFAULT;
  central_state.client_conf_flag = sl_bt_gatt_disable;
  central_state.tx_power      = THROUGHPUT_CENTRAL_TX_POWER;
  central_state.phy           = THROUGHPUT_DEFAULT_PHY;
  central_state.mtu_size      = THROUGHPUT_CENTRAL_DATA_SIZE_MAX;

  central_state.connection_interval_min = THROUGHPUT_CENTRAL_CONNECTION_INTERVAL_MIN;
  central_state.connection_interval_max = THROUGHPUT_CENTRAL_CONNECTION_INTERVAL_MAX;
  central_state.connection_responder_latency = THROUGHPUT_CENTRAL_CONNECTION_RESPONDER_LATENCY;
  central_state.connection_timeout = THROUGHPUT_CENTRAL_CONNECTION_TIMEOUT;

  central_state.scan_phy = THROUGHPUT_DEFAULT_SCAN_PHY;

  // Clear results
  central_state.throughput    = 0;
  central_state.count         = 0;
  central_state.packet_error  = 0;
  central_state.packet_lost   = 0;

  if (THROUGHPUT_CENTRAL_POWER_CONTROL_ENABLE) {
    power_control_enabled = sl_bt_connection_power_reporting_enable;
  } else {
    power_control_enabled = sl_bt_connection_power_reporting_disable;
  }
  // if the power is greater than 10 dBm AFH must be used
  afh_bit = (central_state.tx_power_requested > 10);
  sc = sl_bt_system_linklayer_configure(CONFIG_KEY_SET_AFH,
                                        sizeof(afh_bit),
                                        (uint8_t *)&afh_bit);
  app_assert_status(sc);

  // Convert power to mdBm
  int16_t power = ( ((int16_t)central_state.tx_power) * 10);
  sc = sl_bt_system_set_tx_power(CONFIG_TX_POWER_MIN,
                                 power,
                                 &tx_power_min,
                                 &tx_power_max);
  app_assert_status(sc);
  central_state.tx_power = tx_power_max / 10;
  throughput_central_on_transmit_power_change(central_state.tx_power);

  sc = sl_bt_gatt_server_set_max_mtu(central_state.mtu_size, &(central_state.mtu_size));
  app_assert_status(sc);

  reset_variables();

  #if defined(THROUGHPUT_CENTRAL_ALLOWLIST_ENABLE) && THROUGHPUT_CENTRAL_ALLOWLIST_ENABLE == 1
  #if defined(THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_1_ENABLE) && THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_1_ENABLE == 1
  throughput_central_decode_address(THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_1, address);
  throughput_central_allowlist_add(address);
  #endif // THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_1_ENABLE
  #if defined(THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_2_ENABLE) && THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_2_ENABLE == 1
  throughput_central_decode_address(THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_2, address);
  throughput_central_allowlist_add(address);
  #endif // THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_2_ENABLE
  #if defined(THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_3_ENABLE) && THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_3_ENABLE == 1
  throughput_central_decode_address(THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_3, address);
  throughput_central_allowlist_add(address);
  #endif // THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_3_ENABLE
  #if defined(THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_4_ENABLE) && THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_4_ENABLE == 1
  throughput_central_decode_address(THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_4, address);
  throughput_central_allowlist_add(address);
  #endif // THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_4_ENABLE
  #endif // THROUGHPUT_CENTRAL_ALLOWLIST_ENABLE

  (void) address;

  // Notify application
  throughput_central_on_state_change(central_state.state);
  throughput_central_on_role_set(central_state.role);

  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_all(central_state);
  #endif // SL_CATALOG_THROUGHPUT_UI_PRESENT

  // Start scanning
  throughput_central_scanning_start();

  enabled = true;
}

#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
/**************************************************************************//**
 * Check if it is ok to sleep now.
 *****************************************************************************/
bool throughput_central_is_ok_to_sleep(void)
{
  bool ret = true;
  if (enabled && !deep_sleep_enabled && (central_state.state == THROUGHPUT_STATE_TEST)) {
    ret = false;
  }
  return ret;
}

/**************************************************************************//**
 * Routine for power manager handler.
 *****************************************************************************/
sl_power_manager_on_isr_exit_t throughput_central_sleep_on_isr_exit(void)
{
  sl_power_manager_on_isr_exit_t ret = SL_POWER_MANAGER_IGNORE;
  if (enabled && !deep_sleep_enabled && (central_state.state == THROUGHPUT_STATE_TEST)) {
    ret = SL_POWER_MANAGER_WAKEUP;
  }
  return ret;
}
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

/*******************************************************************************
 *********************** CALLBACK WEAK IMPLEMENTATIONS *************************
 ******************************************************************************/

/**************************************************************************//**
 * Weak implementation of callback to handle role settings.
 *****************************************************************************/
SL_WEAK void throughput_central_on_role_set(throughput_role_t role)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_role(role);
  throughput_ui_update();
  #else
  if (role == THROUGHPUT_ROLE_PERIPHERAL) {
    app_log_info(THROUGHPUT_UI_ROLE_PERIPHERAL_TEXT);
  } else {
    app_log_info(THROUGHPUT_UI_ROLE_CENTRAL_TEXT);
  }
  app_log_nl();
  #endif
}

/**************************************************************************//**
 * Weak implementation of callback to handle state change.
 *****************************************************************************/
SL_WEAK void throughput_central_on_state_change(throughput_state_t state)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_state(state);
  throughput_ui_update();
  #else
  switch (state) {
    case THROUGHPUT_STATE_CONNECTED:
      app_log_info(THROUGHPUT_UI_STATE_CONNECTED_TEXT);
      break;
    case THROUGHPUT_STATE_DISCONNECTED:
      app_log_info(THROUGHPUT_UI_STATE_DISCONNECTED_TEXT);
      break;
    case THROUGHPUT_STATE_SUBSCRIBED:
      app_log_info(THROUGHPUT_UI_STATE_SUBSCRIBED_TEXT);
      break;
    case THROUGHPUT_STATE_TEST:
      app_log_info(THROUGHPUT_UI_STATE_TEST_TEXT);
      break;
    default:
      app_log_info(THROUGHPUT_UI_STATE_UNKNOWN_TEXT);
      break;
  }
  app_log_nl();
  #endif
}

/**************************************************************************//**
 * Weak implementation of callback to handle mode change.
 *****************************************************************************/
SL_WEAK void throughput_central_on_mode_change(throughput_mode_t mode)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  (void) mode;
  #else
  (void) mode;
  #endif
}

/**************************************************************************//**
 * Weak implementation of callback to handle transmission start event.
 *****************************************************************************/
SL_WEAK void throughput_central_on_start(void)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  // Do nothing
  #else
  // Do something on start
  #endif
}

/**************************************************************************//**
 * Weak implementation of callback to handle transmission finished event.
 *****************************************************************************/
SL_WEAK void throughput_central_on_finish(throughput_value_t throughput,
                                          throughput_count_t count,
                                          throughput_count_t lost,
                                          throughput_count_t error,
                                          throughput_time_t time)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_throughput(throughput);
  throughput_ui_set_count(count);
  throughput_ui_update();
  #else
  app_log_info(THROUGHPUT_UI_TH_FORMAT APP_LOG_NEW_LINE, ((int)throughput));
  app_log_info(THROUGHPUT_UI_CNT_FORMAT APP_LOG_NEW_LINE, ((int)count));
  #endif
  app_log_info(THROUGHPUT_UI_LOST_FORMAT APP_LOG_NEW_LINE, ((int)lost));
  app_log_info(THROUGHPUT_UI_ERROR_FORMAT APP_LOG_NEW_LINE, ((int)error));
  app_log_info(THROUGHPUT_UI_TIME_FORMAT APP_LOG_NEW_LINE, ((int)time));
}

/**************************************************************************//**
 * Weak implementation of callback to handle tx power changed event.
 *****************************************************************************/
SL_WEAK void throughput_central_on_transmit_power_change(throughput_tx_power_t power)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_tx_power(power);
  throughput_ui_update();
  #else
  app_log_info(THROUGHPUT_UI_TX_POWER_FORMAT APP_LOG_NEW_LINE, ((int)power));
  #endif
}

/**************************************************************************//**
 * Weak implementation of callback to handle RSSI changed event.
 *****************************************************************************/
SL_WEAK void throughput_central_on_rssi_change(throughput_rssi_t rssi)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_rssi(rssi);
  throughput_ui_update();
  #else
  app_log_info(THROUGHPUT_UI_RSSI_FORMAT APP_LOG_NEW_LINE, ((int)rssi));
  #endif
}

/**************************************************************************//**
 * Weak implementation of callback to handle phy changed event.
 *****************************************************************************/
SL_WEAK void throughput_central_on_phy_change(throughput_phy_t phy)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_phy(phy);
  throughput_ui_update();
  #else
  switch (phy) {
    case sl_bt_gap_phy_coding_1m_uncoded:
      app_log_info(THROUGHPUT_UI_PHY_1M_TEXT);
      break;
    case sl_bt_gap_phy_coding_2m_uncoded:
      app_log_info(THROUGHPUT_UI_PHY_2M_TEXT);
      break;
    case sl_bt_gap_phy_coding_125k_coded:
      app_log_info(THROUGHPUT_UI_PHY_CODED_125K_TEXT);
      break;
    case sl_bt_gap_phy_coding_500k_coded:
      app_log_info(THROUGHPUT_UI_PHY_CODED_500K_TEXT);
      break;
    default:
      app_log_info(THROUGHPUT_UI_PHY_UNKNOWN_TEXT);
      break;
  }
  app_log_nl();
  #endif
}

/**************************************************************************//**
 * Weak implementation of callback to handle notification changed event.
 *****************************************************************************/
SL_WEAK void throughput_central_on_notification_change(throughput_notification_t notification)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_notifications(notification);
  throughput_ui_update();
  #else

  switch (notification) {
    case sl_bt_gatt_notification:
      app_log_info(THROUGHPUT_UI_NOTIFY_YES_TEXT);
      break;
    default:
      app_log_info(THROUGHPUT_UI_NOTIFY_NO_TEXT);
      break;
  }
  app_log_nl();
  #endif
}

/**************************************************************************//**
 * Weak implementation of callback to handle indication changed event.
 *****************************************************************************/
SL_WEAK void throughput_central_on_indication_change(throughput_notification_t indication)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_indications(indication);
  throughput_ui_update();
  #else
  switch (indication) {
    case sl_bt_gatt_indication:
      app_log_info(THROUGHPUT_UI_INDICATE_YES_TEXT);
      break;
    default:
      app_log_info(THROUGHPUT_UI_INDICATE_NO_TEXT);
      break;
  }
  app_log_nl();
  #endif
}

/**************************************************************************//**
 * Weak implementation of callback to handle result indication changed event.
 *****************************************************************************/
SL_WEAK void throughput_central_on_result_indication_change(throughput_notification_t result)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  (void) result;
  #else
  switch (result) {
    case sl_bt_gatt_indication:
      app_log_info(THROUGHPUT_UI_RESULT_YES_TEXT);
      break;
    default:
      app_log_info(THROUGHPUT_UI_RESULT_NO_TEXT);
      break;
  }
  app_log_nl();
  #endif
}

/**************************************************************************//**
 * Weak implementation of callback to handle connection parameter changes.
 *****************************************************************************/
SL_WEAK void throughput_central_on_data_size_change(throughput_pdu_size_t data_size)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_data_size(data_size);
  #else
  app_log_info(THROUGHPUT_UI_DATA_SIZE_FORMAT APP_LOG_NEW_LINE, (int)data_size);
  #endif
}

/**************************************************************************//**
 * Weak implementation of callback to handle connection parameter changes.
 *****************************************************************************/
SL_WEAK void throughput_central_on_connection_settings_change(throughput_pdu_size_t pdu,
                                                              throughput_mtu_size_t mtu)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_pdu_size(pdu);
  throughput_ui_set_mtu_size(mtu);
  throughput_ui_update();
  #else
  app_log_info(THROUGHPUT_UI_PDU_SIZE_FORMAT APP_LOG_NEW_LINE, (int)pdu);
  app_log_info(THROUGHPUT_UI_MTU_SIZE_FORMAT APP_LOG_NEW_LINE, (int)mtu);
  #endif
}
/**************************************************************************//**
 * Weak implementation of callback to handle connection timing changes.
 *****************************************************************************/
SL_WEAK void throughput_central_on_connection_timings_change(throughput_time_t interval,
                                                             throughput_time_t latency,
                                                             throughput_time_t timeout)
{
  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_connection_interval(interval);
  throughput_ui_update();
  #else
  app_log_info(THROUGHPUT_UI_INTERVAL_FORMAT APP_LOG_NEW_LINE, (int)((float)interval * 1.25));
  #endif
  app_log_info(THROUGHPUT_UI_LATENCY_FORMAT APP_LOG_NEW_LINE, (int)latency);
  app_log_info(THROUGHPUT_UI_TIMEOUT_FORMAT APP_LOG_NEW_LINE, (int)timeout);
}

/**************************************************************************//**
 * Weak implementation of callback to handle characteristic found event.
 *****************************************************************************/
SL_WEAK void throughput_central_on_characteristics_found(
  throughput_central_characteristic_found_t characteristics)
{
  app_log_info(THROUGHPUT_UI_CHARACTERISTICS_FOUND_FORMAT APP_LOG_NEW_LINE,
               THROUGHPUT_UI_YESNO(characteristics.characteristic.notification),
               THROUGHPUT_UI_YESNO(characteristics.characteristic.indication),
               THROUGHPUT_UI_YESNO(characteristics.characteristic.transmission_on),
               THROUGHPUT_UI_YESNO(characteristics.characteristic.result)
               );
}

/**************************************************************************//**
 * Weak implementation of callback to handle discovery state change.
 *****************************************************************************/
SL_WEAK void throughput_central_on_discovery_state_change(
  throughput_discovery_state_t state)
{
  switch (state) {
    case THROUGHPUT_DISCOVERY_STATE_IDLE:
      app_log_info(THROUGHPUT_UI_DISCOVERY_STATE_IDLE_TEXT);
      break;
    case THROUGHPUT_DISCOVERY_STATE_CONN:
      app_log_info(THROUGHPUT_UI_DISCOVERY_STATE_CONN_TEXT);
      break;
    case THROUGHPUT_DISCOVERY_STATE_SCAN:
      app_log_info(THROUGHPUT_UI_DISCOVERY_STATE_SCAN_TEXT);
      break;
    case THROUGHPUT_DISCOVERY_STATE_SERVICE:
      app_log_info(THROUGHPUT_UI_DISCOVERY_STATE_SERVICE_TEXT);
      break;
    case THROUGHPUT_DISCOVERY_STATE_CHARACTERISTICS:
      app_log_info(THROUGHPUT_UI_DISCOVERY_STATE_CHARACTERISTICS_TEXT);
      break;
    case THROUGHPUT_DISCOVERY_STATE_FINISHED:
      app_log_info(THROUGHPUT_UI_DISCOVERY_STATE_FINISHED_TEXT);
      break;
    default:
      app_log_info(THROUGHPUT_UI_DISCOVERY_STATE_UNKNOWN_TEXT);
      break;
  }
  app_log_nl();
}

#ifdef SL_CATALOG_CLI_PRESENT
/***************************************************************************//**
 * CLI command for central stop
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_stop(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  sl_status_t sc;
  if (central_state.state == THROUGHPUT_STATE_TEST) {
    sc = throughput_central_stop();
    if (sc == SL_STATUS_OK) {
      CLI_RESPONSE(CLI_OK);
    } else {
      CLI_RESPONSE(CLI_ERROR);
    }
  } else {
    CLI_RESPONSE(CLI_ERROR);
  }
}

/***************************************************************************//**
 * CLI command for central start
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_start(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  sl_status_t sc;
  if (central_state.state == THROUGHPUT_STATE_SUBSCRIBED) {
    uint8_t test = sl_cli_get_argument_uint8(arguments, 0);
    sc = throughput_central_set_type((throughput_notification_t)test);
    if (sc == SL_STATUS_OK) {
      sc = throughput_central_start();
      if (sc == SL_STATUS_OK) {
        CLI_RESPONSE(CLI_OK);
      } else {
        CLI_RESPONSE(CLI_ERROR);
      }
    } else {
      CLI_RESPONSE(CLI_ERROR);
    }
  } else {
    CLI_RESPONSE(CLI_ERROR);
  }
}

/***************************************************************************//**
 * CLI command for central status
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_status(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  switch (central_state.state) {
    case THROUGHPUT_STATE_CONNECTED:
      CLI_RESPONSE(THROUGHPUT_UI_STATE_CONNECTED_TEXT);
      break;
    case THROUGHPUT_STATE_DISCONNECTED:
      CLI_RESPONSE(THROUGHPUT_UI_STATE_DISCONNECTED_TEXT);
      break;
    case THROUGHPUT_STATE_SUBSCRIBED:
      CLI_RESPONSE(THROUGHPUT_UI_STATE_SUBSCRIBED_TEXT);
      break;
    case THROUGHPUT_STATE_TEST:
      CLI_RESPONSE(THROUGHPUT_UI_STATE_TEST_TEXT);
      break;
    default:
      CLI_RESPONSE(THROUGHPUT_UI_STATE_UNKNOWN_TEXT);
      break;
  }
  CLI_RESPONSE(APP_LOG_NEW_LINE);

  if (central_state.role == THROUGHPUT_ROLE_PERIPHERAL) {
    CLI_RESPONSE(THROUGHPUT_UI_ROLE_PERIPHERAL_TEXT);
  } else {
    CLI_RESPONSE(THROUGHPUT_UI_ROLE_CENTRAL_TEXT);
  }
  CLI_RESPONSE(APP_LOG_NEW_LINE);

  CLI_RESPONSE(CLI_OK);
}

/***************************************************************************//**
 * CLI command for setting reception mode
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_mode_set(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  sl_status_t sc;
  uint8_t mode;
  uint32_t amount;
  if (central_state.state != THROUGHPUT_STATE_TEST) {
    mode = sl_cli_get_argument_uint8(arguments, 0);
    amount = sl_cli_get_argument_uint16(arguments, 1);
    sc = throughput_central_set_mode((throughput_mode_t)mode, amount);
    if (sc == SL_STATUS_OK) {
      CLI_RESPONSE(CLI_OK);
    } else {
      CLI_RESPONSE(CLI_ERROR);
    }
  } else {
    CLI_RESPONSE(CLI_ERROR);
  }
}

/***************************************************************************//**
 * CLI command for reading reception mode
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_mode_get(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  CLI_RESPONSE("mode settings\n");
  CLI_RESPONSE("%d %lu %lu\n",
               (int)central_state.mode,
               (uint32_t)fixed_data_size,
               (uint32_t)fixed_time);
}

/***************************************************************************//**
 * CLI command for setting local transmission power
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_tx_power_set(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  int16_t power;
  uint8_t control, sleep;
  sl_status_t sc;
  if (central_state.state != THROUGHPUT_STATE_TEST) {
    power = sl_cli_get_argument_int16(arguments, 0);
    control = sl_cli_get_argument_uint8(arguments, 1);
    sleep = sl_cli_get_argument_uint8(arguments, 2);
    sc = throughput_central_set_tx_power(power,
                                         control,
                                         sleep);

    if (sc == SL_STATUS_OK) {
      CLI_RESPONSE(CLI_OK);
    } else {
      app_assert_status(sc);
      CLI_RESPONSE(CLI_ERROR);
    }
  } else {
    CLI_RESPONSE(CLI_ERROR);
  }
}

/***************************************************************************//**
 * CLI command for reading local transmission power settings
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_tx_power_get(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  CLI_RESPONSE("TX power settings\n");
  CLI_RESPONSE("%d %d %d\n",
               (int)central_state.tx_power,
               (int)power_control_enabled,
               (int)deep_sleep_enabled);
}

/***************************************************************************//**
 * CLI command for setting data sizes
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_data_set(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  uint8_t mtu;
  if (central_state.state != THROUGHPUT_STATE_TEST) {
    mtu = sl_cli_get_argument_uint8(arguments, 0);
    central_state.mtu_size = mtu;
    CLI_RESPONSE(CLI_OK);
    throughput_central_scanning_restart();
  } else {
    CLI_RESPONSE(CLI_ERROR);
  }
}

/***************************************************************************//**
 * CLI command for reading data sizes
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_data_get(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  CLI_RESPONSE("data\n");
  CLI_RESPONSE("%d\n",
               (int)central_state.mtu_size);
}

/***************************************************************************//**
 * CLI command for setting PHY for scanning
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_phy_scan_set(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  uint8_t phy_scan;
  if (central_state.state != THROUGHPUT_STATE_TEST) {
    phy_scan = sl_cli_get_argument_uint8(arguments, 0);
    central_state.scan_phy = (throughput_phy_t)phy_scan;
    throughput_central_scanning_restart();
    CLI_RESPONSE(CLI_OK);
  } else {
    CLI_RESPONSE(CLI_ERROR);
  }
}

/***************************************************************************//**
 * CLI command for setting PHY for connection
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_phy_conn_set(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  uint8_t phy_conn;
  sl_status_t sc;
  if (central_state.state != THROUGHPUT_STATE_TEST) {
    phy_conn = sl_cli_get_argument_uint8(arguments, 0);
    if (central_state.state == THROUGHPUT_STATE_DISCONNECTED) {
      central_state.phy = (throughput_phy_t)phy_conn;
      CLI_RESPONSE(CLI_OK);
    } else {
      sc = throughput_central_set_connection_phy((throughput_phy_t)phy_conn);
      if (sc == SL_STATUS_OK) {
        CLI_RESPONSE(CLI_OK);
      } else {
        CLI_RESPONSE(CLI_ERROR);
      }
    }
  } else {
    CLI_RESPONSE(CLI_ERROR);
  }
}

/***************************************************************************//**
 * CLI command for reading PHY settings
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_phy_get(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  CLI_RESPONSE("phy settings\n");
  CLI_RESPONSE("%d %d\n",
               (int)central_state.scan_phy,
               (int)central_state.phy);
}

/***************************************************************************//**
 * CLI command for setting connection parameters
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_connection_set(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  throughput_time_t connection_interval_min;
  throughput_time_t connection_interval_max;
  throughput_time_t connection_responder_latency;
  throughput_time_t connection_timeout;
  if (central_state.state != THROUGHPUT_STATE_TEST) {
    connection_interval_min = sl_cli_get_argument_uint32(arguments, 0);
    connection_interval_max = sl_cli_get_argument_uint32(arguments, 1);
    connection_responder_latency = sl_cli_get_argument_uint32(arguments, 2);
    connection_timeout = sl_cli_get_argument_uint32(arguments, 3);
    central_state.connection_interval_min = connection_interval_min;
    central_state.connection_interval_max = connection_interval_max;
    central_state.connection_responder_latency = connection_responder_latency;
    central_state.connection_timeout = connection_timeout;
    CLI_RESPONSE(CLI_OK);
    throughput_central_scanning_restart();
  } else {
    CLI_RESPONSE(CLI_ERROR);
  }
}

/***************************************************************************//**
 * CLI command for reading connection parameters
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_connection_get(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  CLI_RESPONSE("connection settings\n");
  CLI_RESPONSE("%u %u %u %u\n",
               (uint32_t)central_state.connection_interval_min,
               (uint32_t)central_state.connection_interval_max,
               (uint32_t)central_state.connection_responder_latency,
               (uint32_t)central_state.connection_timeout);
}

/***************************************************************************//**
 * CLI command for adding to allowlist
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_allowlist_add(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  bool ret;
  uint8_t address[ADR_LEN];
  char *address_str;
  if (central_state.state != THROUGHPUT_STATE_TEST) {
    address_str = sl_cli_get_argument_string(arguments, 0);
    if (throughput_central_decode_address(address_str, address)) {
      ret = throughput_central_allowlist_add(address);
      if (ret) {
        CLI_RESPONSE(CLI_OK);
        throughput_central_scanning_restart();
      } else {
        CLI_RESPONSE(CLI_ERROR);
      }
    } else {
      CLI_RESPONSE(CLI_ERROR);
    }
  } else {
    CLI_RESPONSE(CLI_ERROR);
  }
}

/***************************************************************************//**
 * CLI command for clearing allowlist
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_allowlist_clear(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  if (central_state.state != THROUGHPUT_STATE_TEST) {
    throughput_central_allowlist_clear();
    CLI_RESPONSE(CLI_OK);
    throughput_central_scanning_restart();
  } else {
    CLI_RESPONSE(CLI_ERROR);
  }
}

/***************************************************************************//**
 * CLI command for reading allowlist
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_central_allowlist_get(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  CLI_RESPONSE("allowlist\n");
  throughput_allowlist_t *allowlist = &central_state.allowlist;
  CLI_RESPONSE("---------------------" APP_LOG_NEW_LINE);
  CLI_RESPONSE("|      ADDRESS      |" APP_LOG_NEW_LINE);
  CLI_RESPONSE("---------------------" APP_LOG_NEW_LINE);
  while (allowlist->next != NULL) {
    CLI_RESPONSE("| %02X:%02X:%02X:%02X:%02X:%02X |" APP_LOG_NEW_LINE,
                 allowlist->address.addr[5],
                 allowlist->address.addr[4],
                 allowlist->address.addr[3],
                 allowlist->address.addr[2],
                 allowlist->address.addr[1],
                 allowlist->address.addr[0]);
    allowlist = allowlist->next;
  }
  CLI_RESPONSE("---------------------" APP_LOG_NEW_LINE);
}

#endif // SL_CATALOG_CLI_PRESENT
