/***************************************************************************//**
 * @file
 * @brief Core application logic.
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
#include <stdio.h>
#include <stdbool.h>
#include "em_common.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app_assert.h"
#include "app_log.h"
#include "sl_simple_led_instances.h"
#include "sl_simple_timer.h"
#include "sl_bt_cbap.h"
#include "cbap_config.h"
#include "app.h"

#define LED_TIMEOUT                   500 // ms
#define NO_CALLBACK_DATA              (void *)NULL // Callback has no parameters

#if SL_BT_CONFIG_MAX_CONNECTIONS < 1
  #error At least 1 connection has to be enabled!
#endif

// Connection properties
typedef struct {
  uint8_t connection_handle;
  bd_addr address;
} conn_properties_t;

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = SL_BT_INVALID_ADVERTISING_SET_HANDLE;

// The connectiopn handle and the Bluetooth address of the remote device we
// have CBAP in progress with
static conn_properties_t candidate_device;

// Array for holding properties of the trusted connections
static conn_properties_t trusted_devices[SL_BT_CONFIG_MAX_CONNECTIONS];

// Device role
static sl_bt_cbap_role_t role = ROLE;
// Should we search for a specified peripheral device or not
static bool peripheral_target_defined = ADDR_ENABLE;
// Target device Bluetooth address
static bd_addr peripheral_target_addr;

// Timer handle
static sl_simple_timer_t led_timer;

// Clears candidate device.
static void clear_connection_info(void);
// Adds the candidate device to the trusted devices array.
static void save_connection_info(void);
// Logs the connection handle and the Bluetooth address of the trusted devices.
static void print_trusted_devices(void);

// Convert address string to address data bytes.
static bool decode_address(char *addess_str, bd_addr *address);

// Examine a scan report and decide if a connection should be established.
bool check_scan_report(sl_bt_evt_scanner_legacy_advertisement_report_t *scan_report);

// Timer Callback.
static void led_timer_cb(sl_simple_timer_t *handle, void *data);

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  // Initialize candidate device data
  clear_connection_info();

  // Initialize connection array
  uint8_t i, j;
  for (i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    trusted_devices[i].connection_handle = SL_BT_INVALID_CONNECTION_HANDLE;
    for (j = 0; j < sizeof(bd_addr); j++) {
      trusted_devices[i].address.addr[j] = 0xff;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;
  uint8_t system_id[8];

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      switch (role) {
        case SL_BT_CBAP_ROLE_PERIPHERAL:
          app_log_info("Peripheral role is selected." APP_LOG_NL);

          // Extract unique ID from BT Address.
          sc = sl_bt_system_get_identity_address(&address, &address_type);
          app_assert_status(sc);

          // Pad and reverse unique ID to get System ID.
          system_id[0] = address.addr[5];
          system_id[1] = address.addr[4];
          system_id[2] = address.addr[3];
          system_id[3] = 0xFF;
          system_id[4] = 0xFE;
          system_id[5] = address.addr[2];
          system_id[6] = address.addr[1];
          system_id[7] = address.addr[0];

          sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                                       0,
                                                       sizeof(system_id),
                                                       system_id);
          app_assert_status(sc);

          // Create an advertising set.
          sc = sl_bt_advertiser_create_set(&advertising_set_handle);
          app_assert_status(sc);

          // Generate data for advertising
          sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                     sl_bt_advertiser_general_discoverable);
          app_assert_status(sc);

          // Set advertising interval to 100ms.
          sc = sl_bt_advertiser_set_timing(
            advertising_set_handle,
            160, // min. adv. interval (milliseconds * 1.6)
            160, // max. adv. interval (milliseconds * 1.6)
            0,   // adv. duration
            0);  // max. num. adv. events
          app_assert_status(sc);

          // Start advertising and enable connections.
          sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                             sl_bt_advertiser_connectable_scannable);
          app_assert_status(sc);
          app_log_info("Advertising started." APP_LOG_NL);
          break;

        case SL_BT_CBAP_ROLE_CENTRAL:
          app_log_info("Central role selected." APP_LOG_NL);

          // If defined, get target address
          if (peripheral_target_defined) {
            if (decode_address(ADDR, &peripheral_target_addr)) {
              app_log_info("Searching for %02X:%02X:%02X:%02X:%02X:%02X. " APP_LOG_NL,
                           peripheral_target_addr.addr[5],
                           peripheral_target_addr.addr[4],
                           peripheral_target_addr.addr[3],
                           peripheral_target_addr.addr[2],
                           peripheral_target_addr.addr[1],
                           peripheral_target_addr.addr[0]);
            } else {
              peripheral_target_defined = false;
              app_log_error("Reading target address failed. Searching for any " \
                            "device advertising the CBAP Service." APP_LOG_NL);
            }
          } else {
            app_log_info("Searching for any device advertising the CBAP " \
                         "Service." APP_LOG_NL);
          }

          // Start scanning
          sc = sl_bt_scanner_start(sl_bt_scanner_scan_phy_1m,
                                   sl_bt_scanner_discover_generic);
          app_assert_status(sc);
          app_log_info("Scanning started." APP_LOG_NL);
          break;

        default:
          app_assert_status_f(SL_STATUS_INVALID_STATE, "Invalid role!");
          break;
      }
      break;

    // -------------------------------
    // This event is generated when an advertisement packet or a scan response
    // is received from a responder
    case sl_bt_evt_scanner_legacy_advertisement_report_id:
      if (role == SL_BT_CBAP_ROLE_CENTRAL) {
        // Filter for connectable scannable undirected advertisements
        if ((evt->data.evt_scanner_legacy_advertisement_report.event_flags
             == (SL_BT_SCANNER_EVENT_FLAG_CONNECTABLE | SL_BT_SCANNER_EVENT_FLAG_SCANNABLE))
            && check_scan_report(&evt->data.evt_scanner_legacy_advertisement_report)) {
          // Target device found. Stop scanning.
          sc = sl_bt_scanner_stop();
          app_assert_status(sc);

          // Connect to device
          sc = sl_bt_connection_open(evt->data.evt_scanner_legacy_advertisement_report.address,
                                     evt->data.evt_scanner_legacy_advertisement_report.address_type,
                                     sl_bt_gap_phy_1m,
                                     NULL);
          app_assert_status(sc);
        }
      }
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      app_log_info("Connection opened." APP_LOG_NL);
      // Store data of the candidate device
      candidate_device.connection_handle = evt->data.evt_connection_opened.connection;
      candidate_device.address = evt->data.evt_connection_opened.address;

      if (evt->data.evt_connection_opened.bonding
          != SL_BT_INVALID_BONDING_HANDLE) {
        app_log_warning("Devices are already bonded." APP_LOG_NL);
      }

      sc = sl_bt_cbap_start(role, candidate_device.connection_handle);
      app_log_status_error(sc);
      if (sc == SL_STATUS_OK) {
        app_log_info("CBAP procedure start." APP_LOG_NL);
      }
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      // Remove connection from the connection array if present
      for (int i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
        if (trusted_devices[i].connection_handle == evt->data.evt_connection_closed.connection) {
          trusted_devices[i].connection_handle = SL_BT_INVALID_CONNECTION_HANDLE;
          for (uint8_t j = 0; j < sizeof(bd_addr); j++) {
            trusted_devices[i].address.addr[j] = 0xff;
          }
          app_log_info("Trusted device [%d] removed." APP_LOG_NL,
                       evt->data.evt_connection_closed.connection);
        }
      }
      break;

    // -------------------------------
    // This event indicates that the value of an attribute in the local GATT
    // database was changed by a remote GATT client.
    case sl_bt_evt_gatt_server_attribute_value_id:
      if (role == SL_BT_CBAP_ROLE_PERIPHERAL
          && gattdb_aio_digital_out == evt->data.evt_gatt_server_characteristic_status.characteristic) {
        // The value of the gattdb_aio_digital_out characteristic was changed.
        uint8_t data_recv;
        size_t data_recv_len;

        // Read characteristic value.
        sc = sl_bt_gatt_server_read_attribute_value(gattdb_aio_digital_out,
                                                    0,
                                                    sizeof(data_recv),
                                                    &data_recv_len,
                                                    &data_recv);
        (void)data_recv_len;
        app_log_status_error(sc);

        if (sc != SL_STATUS_OK) {
          break;
        }

        if (data_recv == 0x00) {
          // Turn off LED.
          sl_led_turn_off(SL_SIMPLE_LED_INSTANCE(0));
          app_log_info("LED off." APP_LOG_NL);
        } else {
          // Blink LED.
          sl_led_turn_on(SL_SIMPLE_LED_INSTANCE(0));
          app_log_info("LED on." APP_LOG_NL);
          sc = sl_simple_timer_start(&led_timer,
                                     LED_TIMEOUT,
                                     led_timer_cb,
                                     NO_CALLBACK_DATA,
                                     false);
          app_assert_status(sc);
        }
      }
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

// CBAP Peripheral event handler.
void sl_bt_cbap_peripheral_on_event(sl_bt_cbap_peripheral_state_t status)
{
  sl_status_t sc;

  switch (status) {
    case SL_BT_CBAP_PERIPHERAL_IDLE:
      break;

    case SL_BT_CBAP_PERIPHERAL_CENTRAL_CERT_OK:
      app_log_info("Remote certificate verified." APP_LOG_NL);
      break;

    case SL_BT_CBAP_PERIPHERAL_CENTRAL_OOB_OK:
      app_log_info("Remote OOB data verified." APP_LOG_NL);
      break;

    case SL_BT_CBAP_PERIPHERAL_DONE:
      app_log_info("CBAP procedure complete." APP_LOG_NL);
      save_connection_info();
      clear_connection_info();
      print_trusted_devices();

      // Restart advertising and enable connections
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      app_log_info("Advertising started." APP_LOG_NL);
      break;

    default:
      break;
  }
}

// CBAP Central event handler.
void sl_bt_cbap_central_on_event(sl_bt_cbap_central_state_t status)
{
  sl_status_t sc;

  switch (status) {
    case SL_BT_CBAP_CENTRAL_SCANNING:
      break;

    case SL_BT_CBAP_CENTRAL_DISCOVER_SERVICES:
      app_log_info("Discovering services." APP_LOG_NL);
      break;

    case SL_BT_CBAP_CENTRAL_DISCOVER_CHARACTERISTICS:
      app_log_info("Discovering characteristics." APP_LOG_NL);
      break;

    case SL_BT_CBAP_CENTRAL_GET_PERIPHERAL_CERT:
      app_log_info("Getting certificate from peripheral." APP_LOG_NL);
      break;

    case SL_BT_CBAP_CENTRAL_SEND_CENTRAL_CERT:
      app_log_info("Sending certificate." APP_LOG_NL);
      break;

    case SL_BT_CBAP_CENTRAL_GET_PERIPHERAL_OOB:
      app_log_info("Getting OOB data from peripheral." APP_LOG_NL);
      break;

    case SL_BT_CBAP_CENTRAL_SEND_OOB:
      app_log_info("Sending OOB data." APP_LOG_NL);
      break;

    case SL_BT_CBAP_CENTRAL_INCREASE_SECURITY:
      app_log_info("Increasing security level." APP_LOG_NL);
      break;

    case SL_BT_CBAP_CENTRAL_DONE: {
      app_log_info("CBAP procedure complete." APP_LOG_NL);

      // Blink LED on peripheral
      uint8_t led = 0x01;
      sc = sl_bt_gatt_write_characteristic_value(candidate_device.connection_handle,
                                                 gattdb_aio_digital_out,
                                                 sizeof(led),
                                                 &led);
      app_assert_status(sc);

      save_connection_info();
      clear_connection_info();
      print_trusted_devices();

      // Start scanning
      sc = sl_bt_scanner_start(sl_bt_scanner_scan_phy_1m,
                               sl_bt_scanner_discover_generic);
      app_log_info("Scanning started." APP_LOG_NL);
      break;
    }

    default:
      break;
  }
}

// Callback to handle CBAP process errors.
void sl_bt_on_cbap_error(void)
{
  sl_status_t sc;
  app_log_info("CBAP procedure was aborted for connection %d." APP_LOG_NL,
               candidate_device.connection_handle);

  sc = sl_bt_connection_close(candidate_device.connection_handle);
  app_log_status_error(sc);
  clear_connection_info();

  switch (role) {
    case SL_BT_CBAP_ROLE_PERIPHERAL:
      // Restart advertising and enable connections
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      app_log_info("Advertising started." APP_LOG_NL);
      break;

    case SL_BT_CBAP_ROLE_CENTRAL:
      // Start scanning
      sc = sl_bt_scanner_start(sl_bt_scanner_scan_phy_1m,
                               sl_bt_scanner_discover_generic);
      app_assert_status(sc);
      app_log_info("Scanning started." APP_LOG_NL);
      break;

    default:
      app_assert_status_f(SL_STATUS_INVALID_STATE, "Invalid role!");
      break;
  }
}

/**************************************************************************//**
 * Clears candidate device.
 *****************************************************************************/
static void clear_connection_info(void)
{
  candidate_device.connection_handle = SL_BT_INVALID_CONNECTION_HANDLE;
  for (uint8_t i = 0; i < sizeof(bd_addr); i++) {
    candidate_device.address.addr[i] = 0xff;
  }
}

/**************************************************************************//**
 * Adds the candidate device to the trusted devices array.
 *****************************************************************************/
static void save_connection_info(void)
{
  // Find next available slot
  int index = -1;
  for (int i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (trusted_devices[i].connection_handle == SL_BT_INVALID_CONNECTION_HANDLE) {
      index = i;
      break;
    }
  }
  if (index == -1) {
    app_log_error("Connection array is full." APP_LOG_NL);
    return;
  }

  // Save connection parameters
  trusted_devices[index].connection_handle = candidate_device.connection_handle;
  trusted_devices[index].address = candidate_device.address;

  app_log_info("Trusted device [%d] added." APP_LOG_NL,
               trusted_devices[index].connection_handle);
}

/**************************************************************************//**
 * Logs the connection handle and the Bluetooth address of the trusted devices.
 *****************************************************************************/
static void print_trusted_devices(void)
{
  bool found = false;
  app_log_info("List of trusted connections:" APP_LOG_NL);

  for (int i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (trusted_devices[i].connection_handle != SL_BT_INVALID_CONNECTION_HANDLE) {
      found = true;
      app_log_info("  Connection handle: %d  Address: %02X:%02X:%02X:%02X:%02X:%02X" APP_LOG_NL,
                   trusted_devices[i].connection_handle,
                   trusted_devices[i].address.addr[5],
                   trusted_devices[i].address.addr[4],
                   trusted_devices[i].address.addr[3],
                   trusted_devices[i].address.addr[2],
                   trusted_devices[i].address.addr[1],
                   trusted_devices[i].address.addr[0]);
    }
  }

  if (!found) {
    app_log_info("  None." APP_LOG_NL);
  }
}

/**************************************************************************//**
 * Convert address string to address data bytes.
 * @param[in] addess_str Address string
 * @param[out] address Bluetooth address byte array
 * @return true if operation was successful
 *****************************************************************************/
static bool decode_address(char *addess_str, bd_addr *address)
{
  uint8_t retval;
  unsigned int address_cache[sizeof(bd_addr)];

  retval = sscanf(addess_str, "%02X:%02X:%02X:%02X:%02X:%02X",
                  &address_cache[5],
                  &address_cache[4],
                  &address_cache[3],
                  &address_cache[2],
                  &address_cache[1],
                  &address_cache[0]);

  if (retval != sizeof(bd_addr)) {
    app_log_error("Invalid Bluetooth address." APP_LOG_NL);
    return false;
  }

  for (uint8_t i = 0; i < sizeof(bd_addr); i++) {
    address->addr[i] = (uint8_t)(address_cache[i]);
  }
  return true;
}

/**************************************************************************//**
 * Examine a scan report and decide if a connection should be established.
 * @param[in] scan_report Scan report coming from the Bluetooth stack event.
 * return true if a connection should be established with the device.
 *****************************************************************************/
bool check_scan_report(sl_bt_evt_scanner_legacy_advertisement_report_t *scan_report)
{
  // Check if there is a connection with this device already
  for (int i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (memcmp(scan_report->address.addr, trusted_devices[i].address.addr, sizeof(bd_addr)) == 0) {
      return false;
    }
  }

  // If target defined, check the address
  if (peripheral_target_defined
      && memcmp(scan_report->address.addr, peripheral_target_addr.addr, sizeof(bd_addr)) != 0) {
    return false; // Target device is defined but with different address.
  }

  // Look for CBAP service in advertisement packets
  return sl_bt_cbap_find_service_in_advertisement(scan_report->data.data,
                                                  scan_report->data.len);
}

/***************************************************************************//**
 * Timer Callback.
 * @param[in] handle pointer to handle instance
 * @param[in] data pointer to input data
 ******************************************************************************/
static void led_timer_cb(sl_simple_timer_t *handle, void *data)
{
  (void)handle;
  (void)data;

  sl_led_turn_off(SL_SIMPLE_LED_INSTANCE(0));
  app_log_info("LED off." APP_LOG_NL);
}
