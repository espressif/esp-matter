/***************************************************************************//**
 * @file
 * @brief app_bluetooth.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdio.h>
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#include "em_common.h"
#include "app_assert.h"
#include "app_log.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app_bluetooth.h"
#include "app_menu.h"
#include "app_measurement.h"
#include "app_task_init.h"
#include "app_process.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// enum to know why the connection was droped
typedef enum {
  unknown     = 0,       //!< unknown
  boot_to_dfu_reason = 1,//!< will reboot to update
  deactivated = 2        //!< deactivated by program
} connenction_closed_reasons_t;

/// queue for the outgoing indications
typedef struct range_test_indicaton_t {
  uint16_t characteristic[16];
  uint8_t count_of_indications;
}range_test_indicaton_t;

/// characteristic indication enable struct
typedef struct range_test_indication_enable_t {
  uint16_t characteristic;
  bool enabled;
}range_test_indication_enable_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * This function starts the advertising with the specific configurations
 ******************************************************************************/
static void start_advertising(void);

/*******************************************************************************
 * This function stops the advertising
 ******************************************************************************/
static void stop_advertising(void);

/*******************************************************************************
 * Helper function to easily answer a read request
 ******************************************************************************/
static bool answer_read_request(uint8_t* answer_value, uint8_t value_length, sl_bt_msg_t *evt);

/*******************************************************************************
 * Helper function to check and apply the received value for uint8_t
 ******************************************************************************/
static bool check_and_write_uint8_value(uint8_t *update_value_byte, uint16_t range_attribute, size_t range_max_size, sl_bt_msg_t *evt);

/*******************************************************************************
 * Helper function to check and apply the received value for uint16_t
 ******************************************************************************/
static bool check_and_write_uint16_value(uint16_t *update_value_byte, uint16_t range_attribute, size_t range_max_size, sl_bt_msg_t *evt);

/*******************************************************************************
 * Helper function to check and apply the received value for int16_t
 ******************************************************************************/
static bool check_and_write_int16_value(int16_t *update_value_byte, uint16_t range_attribute, size_t range_max_size, sl_bt_msg_t *evt);

/*******************************************************************************
 * Remove the oldest indication request, but check if this was really answered
 ******************************************************************************/
static void remove_last_indication(uint16_t characteristic);

/*******************************************************************************
 * Clear all indication settings
 ******************************************************************************/
static void clear_indication_enable_list(void);

/*******************************************************************************
 * Checks that this indication is enabled for the gatt characteristic
 ******************************************************************************/
static bool is_indication_enabled(uint16_t characteristic);

/*******************************************************************************
 * Update the setting of the indication of the selected characteristic
 ******************************************************************************/
static void update_indication_enabled(uint16_t characteristic, bool enabled);

/*******************************************************************************
 * As different phys have different channel ranges it will update the range in gatt table
 ******************************************************************************/
static void update_phy_channel_range(void);

/*******************************************************************************
 * As different phys have different payload ranges it will update the range in gatt table
 ******************************************************************************/
static void update_phy_payload_range(void);

/*******************************************************************************
 * As different phys have different power ranges it will update the range in gatt table
 ******************************************************************************/
static void update_phy_power_range(void);

/*******************************************************************************
 * Schedules the restart after the RX measurement
 ******************************************************************************/
static void receive_ended_callback(sl_sleeptimer_timer_handle_t *handle, void *data);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Save the device address
bd_addr bluetooth_address;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Save the active connection
static uint8_t connection_handler = 0xFF;

/// Save the device address type
static uint8_t bluetooth_address_type;

/// Flag for indicating DFU Reset must be performed.
static bool boot_to_dfu = false;

/// The advertising set handle allocated from Bluetooth stack
static uint8_t advertising_set_handle = 0xff;

/// Connection close reason
static connenction_closed_reasons_t connection_closed_reason = unknown;

/// Flag to schedule the indication sending
static bool indication_is_under_way = false;

/// Indication queue
static range_test_indicaton_t range_test_indicatons = { 0 };

/// Indication settings
static range_test_indication_enable_t range_test_indicaton_enable_list[16] = { 0 };

/// RX measurement ending helper variables
static sl_sleeptimer_timer_handle_t recevie_end_timer;
static uint8_t receive_ended_state = 0;

/// Flag to prevent restart loop in RX ending
static volatile bool ble_needs_restart = true;
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 ******************************************************************************/
void sl_bt_on_event(sl_bt_msg_t* evt)
{
  char buf[8] = { 0 };
  sl_status_t bt_status = 0;

  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    ///////////////////////////////////////////////////////////////////////////
    // This event indicates the device has started and the radio is ready.   //
    // Do not call any stack command before receiving this boot event!       //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_system_boot_id:
      connection_handler = 0xFF;
      app_log_info("[info] [B] Booted: v%d.%d.%d-b%d\n",
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);

      bt_status = sl_bt_system_get_identity_address(&bluetooth_address, &bluetooth_address_type);
      app_assert_status_f(bt_status, "sl_bt_system_get_identity_address failed with %#X\n", bt_status);

      app_log_info("[info] [B] Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                   bluetooth_address_type ? "static random" : "public device",
                   bluetooth_address.addr[5],
                   bluetooth_address.addr[4],
                   bluetooth_address.addr[3],
                   bluetooth_address.addr[2],
                   bluetooth_address.addr[1],
                   bluetooth_address.addr[0]);

      snprintf(buf, 5, "%04x", *(uint16_t*)(bluetooth_address.addr));

      bt_status = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status_f(bt_status, "sl_bt_advertiser_create_set failed with %#X\n", bt_status);

      bt_status = sl_bt_gatt_server_write_attribute_value(
        gattdb_serial_number_string,
        0,
        4,
        (uint8_t *)buf);

      if (bt_status != SL_STATUS_OK) {
        app_log_error("sl_bt_gatt_server_write_attribute_value failed with %#X\n", bt_status);
      }

      update_phy_channel_range();
      update_phy_payload_range();
      update_phy_power_range();

      start_advertising();
      break;

    ///////////////////////////////////////////////////////////////////////////
    // This event indicates that a new connection was opened.                //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_connection_opened_id:
      connection_handler = evt->data.evt_connection_opened.connection;
      clear_indication_enable_list();

      connection_closed_reason = unknown;
      app_log_info("[info] [B] Connection opened\n");

      break;

    ///////////////////////////////////////////////////////////////////////////
    // This event indicates that a connection was closed.                    //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_connection_closed_id:
      connection_handler = 0xFF;
      app_log_info("[info] [B] Connection closed\n");
      // Check if need to boot to OTA DFU mode.
      if (boot_to_dfu) {
        // Enter to OTA DFU mode.
        sl_bt_system_reset(2);
      } else {
        if (deactivated == connection_closed_reason) {
        } else if (boot_to_dfu_reason != connection_closed_reason) {
          start_advertising();
        }
      }
      break;

    ///////////////////////////////////////////////////////////////////////////
    // This event indicates that a remote GATT client is attempting to write //
    // a value of a user type attribute in to the local GATT database.       //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_gatt_server_user_write_request_id:
    {
      bool update_needed = false;
      if (gattdb_pktsReq == evt->data.evt_gatt_server_user_write_request.characteristic) {
        uint16_t tmp_value = range_test_settings.packets_repeat_number;
        update_needed = check_and_write_uint16_value(&(tmp_value), gattdb_pktsReq_valid_range, 4, evt);
        range_test_settings.packets_repeat_number  = tmp_value;
      } else if (gattdb_channel == evt->data.evt_gatt_server_user_write_request.characteristic) {
        uint16_t tmp_value = range_test_settings.channel;
        update_needed = check_and_write_uint16_value(&(tmp_value), gattdb_channel_valid_range, 4, evt);
        range_test_settings.channel  = tmp_value;
      } else if (gattdb_phy == evt->data.evt_gatt_server_user_write_request.characteristic) {
        uint8_t value = *(uint8_t*)evt->data.evt_gatt_server_user_write_request.value.data;
        if ( value < number_of_phys ) {
          range_test_settings.current_phy = value;
          bt_status = sl_bt_gatt_server_send_user_write_response(
            evt->data.evt_gatt_server_user_write_request.connection,
            gattdb_phy,
            0);
          if (bt_status != SL_STATUS_OK) {
            app_log_error("sl_bt_gatt_server_send_user_write_response failed with %#X\n", bt_status);
          }
          update_needed = true;
          apply_new_phy(false);
          update_phy_channel_range();
          update_phy_payload_range();
          update_phy_power_range();
        } else {
          bt_status = sl_bt_gatt_server_send_user_write_response(
            evt->data.evt_gatt_server_user_read_request.connection,
            gattdb_phy,
            0xFF);
          if (bt_status != SL_STATUS_OK) {
            app_log_error("sl_bt_gatt_server_send_user_write_response failed with %#X\n", bt_status);
          }
        }
      } else if (gattdb_radioMode == evt->data.evt_gatt_server_user_write_request.characteristic) {
        uint8_t tmp_value = range_test_settings.radio_mode;
        uint8_t update_needed = check_and_write_uint8_value(&(tmp_value), gattdb_radioMode_valid_range, 2, evt);
        range_test_settings.radio_mode = tmp_value;
        if (update_needed) {
          // backwards compatibiliy
          range_test_settings.radio_mode--;
        }
      } else if (gattdb_txPower == evt->data.evt_gatt_server_user_write_request.characteristic) {
        int16_t tmp_value = range_test_settings.tx_power;
        update_needed = check_and_write_int16_value(&(tmp_value), gattdb_txPower_valid_range, 4, evt);
        range_test_settings.tx_power = tmp_value;
        if (update_needed) {
          update_tx_power();
        }
      } else if (gattdb_destID == evt->data.evt_gatt_server_user_write_request.characteristic) {
        uint8_t tmp_value = range_test_settings.destination_id;
        update_needed = check_and_write_uint8_value(&(tmp_value), gattdb_destID_valid_range, 2, evt);
        range_test_settings.destination_id = tmp_value;
      } else if (gattdb_srcID == evt->data.evt_gatt_server_user_write_request.characteristic) {
        uint8_t tmp_value = range_test_settings.source_id;
        update_needed = check_and_write_uint8_value(&(tmp_value), gattdb_srcID_valid_range, 2, evt);
        range_test_settings.source_id = tmp_value;
      } else if (gattdb_payload == evt->data.evt_gatt_server_user_write_request.characteristic) {
        uint8_t tmp_value = range_test_settings.payload_length;
        update_needed = check_and_write_uint8_value(&(tmp_value), gattdb_payload_valid_range, 2, evt);
        range_test_settings.payload_length = tmp_value;
      } else if (gattdb_maSize == evt->data.evt_gatt_server_user_write_request.characteristic) {
        uint8_t tmp_value = range_test_settings.moving_average_window_size;
        update_needed = check_and_write_uint8_value(&(tmp_value), gattdb_maSize_valid_range, 2, evt);
        range_test_settings.moving_average_window_size = tmp_value;
      } else if (gattdb_log == evt->data.evt_gatt_server_user_write_request.characteristic) {
        uint8_t tmp_value = range_test_settings.usart_log_enable;
        update_needed = check_and_write_uint8_value(&(tmp_value), gattdb_log_valid_range, 2, evt);
        range_test_settings.usart_log_enable = tmp_value;
      } else if (gattdb_isRunning == evt->data.evt_gatt_server_user_write_request.characteristic) {
        uint8_t tmp_value = range_test_measurement.tx_is_running;
        update_needed = check_and_write_uint8_value(&(tmp_value), gattdb_isRunning_valid_range, 2, evt);
        range_test_measurement.tx_is_running = tmp_value;
        if (update_needed) {
          set_next_state(START_MEASURMENT);
        }
      } else {
        app_log_info("[warning] [B] Unhandled characteristic write!\n");
      }

      app_log_info("[info] [B] GATT write: %u\n", evt->data.evt_gatt_server_user_read_request.characteristic);

      if (update_needed) {
        request_refresh_screen();
        app_task_notify();
      }
    }
    break;

    case sl_bt_evt_gatt_server_user_read_request_id:
      if (gattdb_PER == evt->data.evt_gatt_server_user_read_request.characteristic) {
        uint16_t per = (uint16_t)(range_test_measurement.PER * 10);
        answer_read_request((uint8_t*)&per, 2, evt);
      } else if (gattdb_MA == evt->data.evt_gatt_server_user_read_request.characteristic) {
        uint16_t moving_average = (uint16_t)(range_test_measurement.moving_average * 10);
        answer_read_request((uint8_t*)&moving_average, 2, evt);
      } else if (gattdb_pktsSent == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_measurement.packets_sent, 2, evt);
      } else if (gattdb_pktsCnt == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_measurement.packets_received_counter, 2, evt);
      } else if (gattdb_pktsRcvd == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_measurement.packets_received_correctly, 2, evt);
      } else if (gattdb_pktsReq == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_settings.packets_repeat_number, 2, evt);
      } else if (gattdb_channel == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_settings.channel, 2, evt);
      } else if (gattdb_phy == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_settings.current_phy, 1, evt);
      } else if (gattdb_phyList == evt->data.evt_gatt_server_user_read_request.characteristic) {
        static char phy_list[255] = { 0 };
        static uint8_t phy_list_length = 0;
        phy_list_generation((uint8_t*)&phy_list, &phy_list_length);
        answer_read_request((uint8_t*)&phy_list, phy_list_length, evt);
      } else if (gattdb_radioMode == evt->data.evt_gatt_server_user_read_request.characteristic) {
        uint8_t radio_mode = range_test_settings.radio_mode + 1;
        answer_read_request(&radio_mode, 1, evt);
      } else if (gattdb_frequency == evt->data.evt_gatt_server_user_read_request.characteristic) {
        uint32_t base_frequency = 0;
        uint32_t channel_spacing = 0;
        int16_t power = 0;

        get_rail_config_data(&base_frequency, &channel_spacing, &power);
        base_frequency = (base_frequency / 1000000);
        answer_read_request((uint8_t*)&base_frequency, 2, evt);
      } else if (gattdb_txPower == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_settings.tx_power, 2, evt);
      } else if (gattdb_destID == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_settings.destination_id, 1, evt);
      } else if (gattdb_srcID == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_settings.source_id, 1, evt);
      } else if (gattdb_payload == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_settings.payload_length, 1, evt);
      } else if (gattdb_maSize == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_settings.moving_average_window_size, 1, evt);
      } else if (gattdb_log == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_settings.usart_log_enable, 1, evt);
      } else if (gattdb_isRunning == evt->data.evt_gatt_server_user_read_request.characteristic) {
        answer_read_request((uint8_t*)&range_test_measurement.tx_is_running, 1, evt);
      } else {
        app_log_info("[warning] [B] Unhandled characteristic read!\n");
      }
      app_log_info("[info] [B] GATT read: %u\n", evt->data.evt_gatt_server_user_read_request.characteristic);
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      if (gatt_server_confirmation == evt->data.evt_gatt_server_characteristic_status.status_flags) {
        remove_last_indication(evt->data.evt_gatt_server_characteristic_status.characteristic);
        indication_is_under_way = false;
      } else if (gatt_server_client_config == evt->data.evt_gatt_server_characteristic_status.status_flags) {
        update_indication_enabled(
          evt->data.evt_gatt_server_characteristic_status.characteristic,
          (evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_indication));
      }
      break;
    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // Default event handler.                                                //
    ///////////////////////////////////////////////////////////////////////////
    default:
      break;
  }
}

/*******************************************************************************
 * This function activates the BLE advertising to be connectable for mobiles
 *
 * @param None
 * @return None
 ******************************************************************************/
void activate_bluetooth(void)
{
  // no connection: start advertising again
  if (connection_handler == 0xFF) {
    stop_advertising();
    start_advertising();
    ble_needs_restart = false;
  }
}

/*******************************************************************************
 * This function deactivates the BLE advertising
 *
 * @param None
 * @return None
 ******************************************************************************/
void deactivate_bluetooth(void)
{
  sl_status_t bt_status = 0;
  // close connection
  if (connection_handler != 0xFF) {
    bt_status = sl_bt_connection_close(connection_handler);
    app_assert_status_f(bt_status, "sl_bt_connection_close failed with %#X\n", bt_status);
    connection_closed_reason = deactivated;
  }
  // stop advertisement
  else {
    stop_advertising();
  }
}

/*******************************************************************************
 * API advertises the received packets - note that we are not connected here
 *
 * @param rssi: current rssi value of the received packet
 * @param packet_count: the packet count
 * @param received_packets: corretly received packets
 * @return None
 ******************************************************************************/
void advertise_received_data(int8_t rssi, uint16_t packet_count, uint16_t received_packets)
{
  sl_status_t bt_status = 0;
  // Company Identifier of Silicon Labs
  uint16_t company_id = 0x02FF;
  char series_local_name[] = "DMP0000";
  uint8_t i = 0;
  uint8_t buf[31] = { 0 };
  // Construct advertisement structure
  // AD Structure: Flags
  buf[i++] = 2;               // Length of field: Type + Flags
  buf[i++] = 0x01;            // Type of field: Flags
  buf[i++] = 0x04 | 0x02;     // Flags: BR/EDR is disabled, LE General Discoverable Mode
  // AD Structure: Shortened Local Name, e.g.: DMP1234
  snprintf(&series_local_name[3], 5, "%04X", *(uint16_t*)(bluetooth_address.addr));
  buf[i++] = 1 + sizeof(series_local_name) - 1;  // Length of field: Type + Shortened Local Name
  buf[i++] = 0x08;            // Shortened Local Name
  memcpy(&buf[i], (uint8_t*)series_local_name, sizeof(series_local_name) - 1); i += (sizeof(series_local_name) - 1);
  // AD Structure: Manufacturer specific
  buf[i++] = 9;               // Length of structure
  buf[i++] = 0xFF;            // Manufacturer Specific Data
  buf[i++] = (uint8_t)(company_id & 0x00FF);        // Company ID
  buf[i++] = (uint8_t)((company_id >> 8) & 0x00FF); // Company ID
  buf[i++] = 0x00;            // Structure type; used for backward compatibility
  buf[i++] = rssi;            // RSSI
  buf[i++] = (uint8_t)(packet_count & 0x00FF);           // Packet counter
  buf[i++] = (uint8_t)((packet_count >> 8) & 0x00FF);    // Packet counter
  buf[i++] = (uint8_t)(received_packets & 0x00FF);          // Number of received packets
  buf[i++] = (uint8_t)((received_packets >> 8) & 0x00FF);   // Number of received packets

  bt_status = sl_bt_legacy_advertiser_set_data(advertising_set_handle, sl_bt_advertiser_advertising_data_packet, i, buf);
  app_assert_status_f(bt_status, "sl_bt_legacy_advertiser_set_data failed with %#X\n", bt_status);

  // Configure advertising to send out only 1 packet.
  bt_status = sl_bt_advertiser_set_timing(
    advertising_set_handle,           // advertising set handle
    32,           // min. adv. interval (milliseconds * 1.6)
    32,           // max. adv. interval (milliseconds * 1.6)
    0,             // adv. duration
    1);            // max. num. adv. events
  app_assert_status_f(bt_status, "sl_bt_advertiser_set_timing failed with %#X\n", bt_status);

  // Start advertising.
  bt_status = sl_bt_legacy_advertiser_start(
    advertising_set_handle,                    // advertising set handle
    sl_bt_legacy_advertiser_non_connectable);     // connectable mode
  app_assert_status_f(bt_status, "sl_bt_legacy_advertiser_start failed with %#X\n", bt_status);
//  APP_LOG("[info] [B] Advertise RSSI\n");
}

/*******************************************************************************
 * Add a characteristic indication to the queue
 *
 * @param characteristic
 * @return None
 ******************************************************************************/
void add_bluetooth_indication(uint16_t characteristic)
{
  if (connection_handler != 0xFF) {
    if (is_indication_enabled(characteristic)) {
      if (range_test_indicatons.count_of_indications < 16) {
        for (int i = 0; i < range_test_indicatons.count_of_indications; i++) {
          if (range_test_indicatons.characteristic[i] == characteristic) {
            return;
          }
        }
        range_test_indicatons.count_of_indications++;
        range_test_indicatons.characteristic[(range_test_indicatons.count_of_indications - 1)] = characteristic;
      } else {
        app_log_info("Indication queue is full\n");
      }
    }
  }
}

/*******************************************************************************
 * Send the first indication in the queue
 *
 * @param None
 * @return None
 ******************************************************************************/
void send_bluetooth_indications(void)
{
  sl_status_t bt_status = SL_STATUS_OK;
  uint16_t characteristic = 0;

  if (connection_handler != 0xFF) {
    if (!indication_is_under_way) {
      if (range_test_indicatons.count_of_indications > 0) {
        characteristic = range_test_indicatons.characteristic[0];
        switch (characteristic) {
          case gattdb_PER:
          {
            uint16_t per = (uint16_t)(range_test_measurement.PER * 10);
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 2, (uint8_t*)&per);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
          }
          break;
          case gattdb_MA:
          {
            uint16_t moving_average = (uint16_t)(range_test_measurement.moving_average * 10);
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 2, (uint8_t*)&moving_average);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
          }
          break;
          case gattdb_pktsSent:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 2, (uint8_t*)&range_test_measurement.packets_sent);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_pktsCnt:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 2, (uint8_t*)&range_test_measurement.packets_received_counter);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_pktsRcvd:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 2, (uint8_t*)&range_test_measurement.packets_received_correctly);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_pktsReq:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 2, (uint8_t*)&range_test_settings.packets_repeat_number);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_channel:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 2, (uint8_t*)&range_test_settings.channel);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_phy:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 1, (uint8_t*)&range_test_settings.current_phy);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            update_phy_channel_range();
            update_phy_payload_range();
            update_phy_power_range();
            break;
          case gattdb_radioMode:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 1, (uint8_t*)&range_test_settings.radio_mode);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_txPower:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 2, (uint8_t*)&range_test_settings.tx_power);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_destID:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 1, (uint8_t*)&range_test_settings.destination_id);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_srcID:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 1, (uint8_t*)&range_test_settings.source_id);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_payload:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 1, (uint8_t*)&range_test_settings.payload_length);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_maSize:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 1, (uint8_t*)&range_test_settings.moving_average_window_size);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_log:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 1, (uint8_t*)&range_test_settings.usart_log_enable);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          case gattdb_isRunning:
            bt_status = sl_bt_gatt_server_send_indication(connection_handler, characteristic, 1, (uint8_t*)&range_test_measurement.tx_is_running);
            if (bt_status != SL_STATUS_OK) {
              app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n", bt_status);
            }
            indication_is_under_way = true;
            break;
          default:
            break;
        }
      }
    }
  }
}

/*******************************************************************************
 * After RX measurement we have to restart the advertising
 *
 * @param None
 * @return None
 ******************************************************************************/
void manage_bluetooth_restart(void)
{
  if (ble_needs_restart) {
    if ((range_test_measurement.packets_received_counter == range_test_settings.packets_repeat_number) && (receive_ended_state == 0)) {
      receive_ended_state++;
      sl_sleeptimer_start_timer_ms(&recevie_end_timer, 500, receive_ended_callback, NULL, 0, 0);
    }
    if (receive_ended_state == 2) {
      advertise_received_data(range_test_measurement.rssi_latch_value,
                              range_test_measurement.packets_received_counter,
                              range_test_measurement.packets_received_counter);
      sl_sleeptimer_start_timer_ms(&recevie_end_timer, 500, receive_ended_callback, NULL, 0, 0);
    }
    if (receive_ended_state == 3) {
      activate_bluetooth();
      receive_ended_state = 0;
    }
  }
}

/*******************************************************************************
 * Check for active bluetooth connection
 * @return bool: true if there is an active connection
 ******************************************************************************/
bool is_bluetooth_connected(void)
{
  return (connection_handler != 0xFF);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * This function starts the advertising with the specific configurations
 *
 * @param None
 * @return None
 ******************************************************************************/
static void start_advertising(void)
{
  sl_status_t bt_status = 0;
  // Range Test RAIL DMP UUID defined by marketing.
  const uint8_t uuid[16] = { 0x63, 0x7e, 0x17, 0x3b, 0x39, 0x9e, 0x20, 0x9f,
                             0x62, 0x4d, 0xe6, 0x17, 0x49, 0xa6, 0x0a, 0x53 };
  char series_local_name[] = "DMP0000";
  uint8_t i = 0;
  uint8_t buf[31] = { 0 };
  // Construct advertisement structure
  // AD Structure: Flags
  buf[i++] = 2;                 // Length of field: Type + Flags
  buf[i++] = 0x01;              // Type of field: Flags
  buf[i++] = 0x04 | 0x02;       // Flags: BR/EDR is disabled, LE General Discoverable Mode
  // AD Structure: Shortened Local Name, e.g.: DMP1234
  snprintf(&series_local_name[3], 5, "%04X", *(uint16_t*)(bluetooth_address.addr));
  buf[i++] = 1 + sizeof(series_local_name) - 1;    // Length of field: Type + Shortened Local Name
  buf[i++] = 0x08;              // Shortened Local Name
  memcpy(&buf[i], (uint8_t*)series_local_name, sizeof(series_local_name) - 1); i += (sizeof(series_local_name) - 1);
  // AD Structure: Range Test DMP UUID
  buf[i++] = 1 + sizeof(uuid);
  buf[i++] = 0x06;              // Incomplete List of 128-bit Service Class UUID
  memcpy(&buf[i], uuid, sizeof(uuid)); i += sizeof(uuid);

  bt_status = sl_bt_legacy_advertiser_set_data(advertising_set_handle, sl_bt_advertiser_advertising_data_packet, i, buf);
  app_assert_status_f(bt_status, "sl_bt_legacy_advertiser_set_data failed with %#X\n", bt_status);

  // Set advertising interval to 100ms.
  bt_status = sl_bt_advertiser_set_timing(
    advertising_set_handle,         // advertising set handle
    160,         // min. adv. interval (milliseconds * 1.6)
    160,         // max. adv. interval (milliseconds * 1.6)
    0,           // adv. duration
    0);          // max. num. adv. events
  app_assert_status_f(bt_status, "sl_bt_advertiser_set_timing failed with %#X\n", bt_status);

  // Start advertising and enable connections.
  bt_status = sl_bt_legacy_advertiser_start(
    advertising_set_handle,                    // advertising set handle
    sl_bt_legacy_advertiser_connectable);         // connectable mode
  app_assert_status_f(bt_status, "sl_bt_legacy_advertiser_start failed with %#X\n", bt_status);

  app_log_info("[info] [B] Start advertising\n");
}

/*******************************************************************************
 * This function stops the advertising
 *
 * @param None
 * @return None
 ******************************************************************************/
static void stop_advertising(void)
{
  sl_status_t bt_status = 0;
  bt_status = sl_bt_advertiser_stop(advertising_set_handle);
  app_assert_status_f(bt_status, "sl_bt_advertiser_stop failed with %#X\n", bt_status);
  app_log_info("[info] [B] Stop advertising\n");
  ble_needs_restart = true;
}

/*******************************************************************************
 * Helper function to easily answer a read request
 * @param answer_value: pointer to the value to be sent
 * @param value_length: the length of the value
 * @param evt: pointer to the ble message
 ******************************************************************************/
static bool answer_read_request(uint8_t* answer_value, uint8_t value_length, sl_bt_msg_t *evt)
{
  uint16_t sent_len = 0;
  sl_status_t bt_status = SL_STATUS_OK;
  bt_status = sl_bt_gatt_server_send_user_read_response(
    evt->data.evt_gatt_server_user_read_request.connection,
    evt->data.evt_gatt_server_user_read_request.characteristic,
    0,
    value_length,
    answer_value,
    &sent_len);
  if (bt_status != SL_STATUS_OK) {
    app_log_error("sl_bt_gatt_server_send_user_read_response failed with 0x%04X\n", bt_status);
  }
  if (bt_status == SL_STATUS_OK) {
    return true;
  } else {
    return false;
  }
}

/*******************************************************************************
 * Helper function to check and apply the received value for uint8_t
 * @param update_value_byte: pointer to the field that will be updated
 * @param range_attribute: range attribute for the gatt table
 * @param range_max_size: the size of the range attribute
 * @param evt: pointer to the ble message
 ******************************************************************************/
static bool check_and_write_uint8_value(uint8_t *update_value_byte, uint16_t range_attribute, size_t range_max_size, sl_bt_msg_t *evt)
{
  size_t read_length = 0;
  uint8_t range_buff[4];
  sl_status_t bt_status = SL_STATUS_OK;
  uint8_t value;
  uint8_t max_range;
  uint8_t min_range;
  bool success = false;

  bt_status = sl_bt_gatt_server_read_attribute_value(range_attribute, 0, range_max_size, &read_length, range_buff);
  if (bt_status != SL_STATUS_OK) {
    app_log_error("sl_bt_gatt_server_read_attribute_value failed with 0x%04X\n", bt_status);
  }

  min_range = range_buff[0];
  max_range = range_buff[1];
  value = evt->data.evt_gatt_server_user_write_request.value.data[0];

  if ((min_range <= value) && (value <= max_range)) {
    *update_value_byte = value;
    bt_status = sl_bt_gatt_server_send_user_write_response(
      evt->data.evt_gatt_server_user_write_request.connection,
      evt->data.evt_gatt_server_user_write_request.characteristic,
      0);
    if (bt_status != SL_STATUS_OK) {
      app_log_error("sl_bt_gatt_server_send_user_write_response failed with 0x%04X\n", bt_status);
    }
    success = true;
  } else {
    bt_status = sl_bt_gatt_server_send_user_write_response(
      evt->data.evt_gatt_server_user_write_request.connection,
      evt->data.evt_gatt_server_user_write_request.characteristic,
      0xFF);
    if (bt_status != SL_STATUS_OK) {
      app_log_warning("sl_bt_gatt_server_send_user_write_response failed with 0x%04X\n", bt_status);
    }
  }
  return success;
}

/*******************************************************************************
 * Helper function to check and apply the received value for uint16_t
 * @param update_value_byte: pointer to the field that will be updated
 * @param range_attribute: range attribute for the gatt table
 * @param range_max_size: the size of the range attribute
 * @param evt: pointer to the ble message
 ******************************************************************************/
static bool check_and_write_uint16_value(uint16_t *update_value_byte, uint16_t range_attribute, size_t range_max_size, sl_bt_msg_t *evt)
{
  size_t read_length = 0;
  uint8_t range_buff[4];
  sl_status_t bt_status = SL_STATUS_OK;
  uint16_t value;
  uint16_t max_range;
  uint16_t min_range;
  bool success = false;

  bt_status = sl_bt_gatt_server_read_attribute_value(range_attribute, 0, range_max_size, &read_length, range_buff);
  if (bt_status != SL_STATUS_OK) {
    app_log_error("sl_bt_gatt_server_read_attribute_value failed with 0x%04X\n", bt_status);
  }

  min_range = *(uint16_t*)&range_buff[0];
  max_range = *(uint16_t*)&range_buff[2];
  value = *(uint16_t*)evt->data.evt_gatt_server_user_write_request.value.data;

  if (min_range <= value && value <= max_range) {
    *update_value_byte = value;
    bt_status = sl_bt_gatt_server_send_user_write_response(
      evt->data.evt_gatt_server_user_write_request.connection,
      evt->data.evt_gatt_server_user_write_request.characteristic,
      0);
    if (bt_status != SL_STATUS_OK) {
      app_log_error("sl_bt_gatt_server_send_user_write_response failed with 0x%04X\n", bt_status);
    }
    success = true;
  } else {
    bt_status = sl_bt_gatt_server_send_user_write_response(
      evt->data.evt_gatt_server_user_write_request.connection,
      evt->data.evt_gatt_server_user_write_request.characteristic,
      0xFF);
    if (bt_status != SL_STATUS_OK) {
      app_log_error("sl_bt_gatt_server_send_user_write_response failed with 0x%04X\n", bt_status);
    }
  }
  return success;
}

/*******************************************************************************
 * Helper function to check and apply the received value for int16_t
 * @param update_value_byte: pointer to the field that will be updated
 * @param range_attribute: range attribute for the gatt table
 * @param range_max_size: the size of the range attribute
 * @param evt: pointer to the ble message
 * @return None
 ******************************************************************************/
static bool check_and_write_int16_value(int16_t *update_value_byte, uint16_t range_attribute, size_t range_max_size, sl_bt_msg_t *evt)
{
  size_t read_length = 0;
  uint8_t range_buff[4];
  sl_status_t bt_status = SL_STATUS_OK;
  int16_t value;
  int16_t max_range;
  int16_t min_range;
  bool success = false;

  bt_status = sl_bt_gatt_server_read_attribute_value(range_attribute, 0, range_max_size, &read_length, range_buff);
  if (bt_status != SL_STATUS_OK) {
    app_log_error("sl_bt_gatt_server_read_attribute_value failed with 0x%04X\n", bt_status);
  }

  min_range = *(int16_t*)&range_buff[0];
  max_range = *(int16_t*)&range_buff[2];
  value = *(int16_t*)evt->data.evt_gatt_server_user_write_request.value.data;

  if (min_range <= value && value <= max_range) {
    *update_value_byte = value;
    bt_status = sl_bt_gatt_server_send_user_write_response(
      evt->data.evt_gatt_server_user_write_request.connection,
      evt->data.evt_gatt_server_user_write_request.characteristic,
      0);
    if (bt_status != SL_STATUS_OK) {
      app_log_error("sl_bt_gatt_server_send_user_write_response failed with 0x%04X\n", bt_status);
    }
    success = true;
  } else {
    bt_status = sl_bt_gatt_server_send_user_write_response(
      evt->data.evt_gatt_server_user_write_request.connection,
      evt->data.evt_gatt_server_user_write_request.characteristic,
      0xFF);
    if (bt_status != SL_STATUS_OK) {
      app_log_error("sl_bt_gatt_server_send_user_write_response failed with 0x%04X\n", bt_status);
    }
  }
  return success;
}

/*******************************************************************************
 * Remove the oldest indication request, but check if this was really answered
 * @param characteristic: the answered indication
 * @return None
 ******************************************************************************/
static void remove_last_indication(uint16_t characteristic)
{
  if (range_test_indicatons.count_of_indications > 0) {
    if (characteristic == range_test_indicatons.characteristic[0]) {
      range_test_indicatons.count_of_indications--;
      for (uint8_t i = 0; i < 15; i++) {
        range_test_indicatons.characteristic[i] = range_test_indicatons.characteristic[i + 1];
        if (i >= range_test_indicatons.count_of_indications) {
          range_test_indicatons.characteristic[i] = 0;
        }
      }
    } else {
      app_log_info("Not correct indication\n");
    }
  }
  if (range_test_indicatons.count_of_indications > 0) {
    app_task_notify();
  }
}

/*******************************************************************************
 * Clear all indication settings
 * @param None
 * @return None
 ******************************************************************************/
static void clear_indication_enable_list(void)
{
  for (uint8_t indication = 0; indication < 16; indication++) {
    range_test_indicaton_enable_list[indication].characteristic = 0;
    range_test_indicaton_enable_list[indication].enabled = false;
  }
}

/*******************************************************************************
 * Checks that this indication is enabled for the gatt characteristic
 * @param characteristic: which characteristic needed indication
 * @return bool: true if indications are allowed
 ******************************************************************************/
static bool is_indication_enabled(uint16_t characteristic)
{
  bool enabled = false;
  for (uint8_t indication = 0; indication < 16; indication++) {
    if (characteristic == range_test_indicaton_enable_list[indication].characteristic) {
      enabled = range_test_indicaton_enable_list[indication].enabled;
      break;
    }
  }
  return enabled;
}

/*******************************************************************************
 * Update the setting of the indication of the selected characteristic
 * @param characteristic: which characteristic needed indication
 * @param enabled: is it on or off
 * @return None
 ******************************************************************************/
static void update_indication_enabled(uint16_t characteristic, bool enabled)
{
  uint8_t first_blank = 0;
  bool first_blank_found = false;
  bool updated = false;
  for (uint8_t indication = 0; indication < 16; indication++) {
    if (0 == range_test_indicaton_enable_list[indication].characteristic && !first_blank_found) {
      first_blank = indication;
      first_blank_found = true;
    }
    if (characteristic == range_test_indicaton_enable_list[indication].characteristic) {
      range_test_indicaton_enable_list[indication].enabled = enabled;
      updated = true;
      break;
    }
  }
  if (!updated) {
    range_test_indicaton_enable_list[first_blank].characteristic = characteristic;
    range_test_indicaton_enable_list[first_blank].enabled = enabled;
  }
}

/*******************************************************************************
 * As different phys have different channel ranges it will update the range in gatt table
 * @param None
 * @return None
 ******************************************************************************/
static void update_phy_channel_range(void)
{
  uint16_t channel_end_number = 0;
  uint16_t channel_start_number = 0;
  sl_status_t bt_status = SL_STATUS_OK;
  uint8_t buf[4] = { 0 };

  get_rail_channel_range(&channel_start_number, &channel_end_number);

  buf[0] = (uint8_t)(channel_start_number & 0x00FF);
  buf[1] = (uint8_t)((channel_start_number >> 8) & 0x00FF);
  buf[2] = (uint8_t)(channel_end_number & 0x00FF);
  buf[3] = (uint8_t)((channel_end_number >> 8) & 0x00FF);

  bt_status = sl_bt_gatt_server_write_attribute_value(gattdb_channel_valid_range, 0, 4, (uint8_t*)buf);

  if (bt_status != SL_STATUS_OK) {
    app_log_error("sl_bt_gatt_server_write_attribute_value failed with %#X\n", bt_status);
  }
}

/*******************************************************************************
 * As different phys have different payload ranges it will update the range in gatt table
 * @param None
 * @return None
 ******************************************************************************/
static void update_phy_payload_range(void)
{
  uint8_t payload_min = 0;
  uint8_t payload_max = 0;
  sl_status_t bt_status = SL_STATUS_OK;
  uint8_t buf[2] = { 0 };

  get_rail_payload_range(&payload_min, &payload_max);

  buf[0] = payload_min;
  buf[1] = payload_max;

  bt_status = sl_bt_gatt_server_write_attribute_value(gattdb_payload_valid_range, 0, 2, (uint8_t*)buf);

  if (bt_status != SL_STATUS_OK) {
    app_log_error("sl_bt_gatt_server_write_attribute_value failed with %#X\n", bt_status);
  }
}

/*******************************************************************************
 * As different phys have different power ranges it will update the range in gatt table
 * @param None
 * @return None
 ******************************************************************************/
static void update_phy_power_range(void)
{
  int16_t power_min = 0;
  int16_t power_max = 0;
  sl_status_t bt_status = SL_STATUS_OK;
  uint8_t buf[4] = { 0 };

  power_min = get_min_tx_power_deci_dbm();
  power_max = get_max_tx_power_deci_dbm();

  buf[0] = (uint8_t)(power_min & 0x00FF);
  buf[1] = (uint8_t)((power_min >> 8) & 0x00FF);
  buf[2] = (uint8_t)(power_max & 0x00FF);
  buf[3] = (uint8_t)((power_max >> 8) & 0x00FF);

  bt_status = sl_bt_gatt_server_write_attribute_value(gattdb_txPower_valid_range, 0, 4, (uint8_t*)buf);

  if (bt_status != SL_STATUS_OK) {
    app_log_error("sl_bt_gatt_server_write_attribute_value failed with %#X\n", bt_status);
  }
}

/*******************************************************************************
 * Schedules the restart after the RX measurement
 * @param handle: not used
 * @param data: not used
 * @return None
 ******************************************************************************/
static void receive_ended_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)handle;
  (void)data;
  receive_ended_state++;
  app_task_notify();
}
