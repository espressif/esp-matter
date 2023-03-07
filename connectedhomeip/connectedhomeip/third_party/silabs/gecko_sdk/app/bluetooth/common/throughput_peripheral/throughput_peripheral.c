/***************************************************************************//**
 * @file
 * @brief Core logic for throughput test peripheral role API.
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

#include <stdbool.h>
#include "em_common.h"
#include "sl_status.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app_assert.h"
#include "app_log.h"
#include "sl_sleeptimer.h"
#include "throughput_peripheral_config.h"
#include "throughput_peripheral.h"
#include "sl_simple_timer.h"
#include "throughput_ui.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#endif // SL_CATALOG_CLI_PRESENT
#include "throughput_ui_types.h"
#include "throughput_common.h"

/*******************************************************************************
 *******************************  DEFINITIONS   ********************************
 ******************************************************************************/
// Size of the arrays for sending and receiving
#define THROUGHPUT_TX_DATA_SIZE                  255
// Refresh RSSI timer period
#define THROUGHPUT_TX_REFRESH_TIMER_PERIOD       1000
// Hardware clock ticks that equal one second
#define HW_TICKS_PER_SECOND                         (uint16_t)(32768)
// GATT operation header byte count
#define INDICATION_GATT_HEADER                      3
// GATT operation header byte count
#define NOTIFICATION_GATT_HEADER                    3
// Header byte count
#define L2CAP_HEADER                                4
// Indication timeout period
#define THROUGHPUT_TX_INDICATION_TIMEOUT         500
// Minimum TX power
#define CONFIG_TX_POWER_MIN                        -100

/*******************************************************************************
 ********************************  CONSTANTS   *********************************
 ******************************************************************************/
/// Transmission on
const uint8_t TRANSMISSION_ON = 1;

/// Transmission off
const uint8_t TRANSMISSION_OFF = 0;

// bbb99e70-fff7-46cf-abc7-2d32c71820f2
const uint8_t peripheral_service_uuid[] = { 0xf2, 0x20, 0x18, 0xc7, 0x32, 0x2d, 0xc7, 0xab, 0xcf,
                                            0x46, 0xf7, 0xff, 0x70, 0x9e, 0xb9, 0xbb };
// 6109b631-a643-4a51-83d2-2059700ad49f
const uint8_t peripheral_indications_characteristic_uuid[] = { 0x9f, 0xd4, 0x0a, 0x70, 0x59, 0x20, 0xd2,
                                                               0x83, 0x51, 0x4a, 0x43, 0xa6, 0x31, 0xb6, 0x09, 0x61 };
// 47b73dd6-dee3-4da1-9be0-f5c539a9a4be
const uint8_t peripheral_notifications_characteristic_uuid[] = { 0xbe, 0xa4, 0xa9, 0x39, 0xc5, 0xf5, 0xe0, 0x9b,
                                                                 0xa1, 0x4d, 0xe3, 0xde, 0xd6, 0x3d, 0xb7, 0x47 };
// be6b6be1-cd8a-4106-9181-5ffe2bc67718
const uint8_t peripheral_transmission_characteristic_uuid[] = { 0x18, 0x77, 0xc6, 0x2b, 0xfe, 0x5f, 0x81, 0x91,
                                                                0x06, 0x41, 0x8a, 0xcd, 0xe1, 0x6b, 0x6b, 0xbe };

#define UUID_LEN                                    16

/*******************************************************************************
 *****************************  LOCAL VARIABLES   ******************************
 ******************************************************************************/

/// Internal state
static throughput_t peripheral_state;

/// Connection handle
static uint8_t connection = 0;

/// Indication state for result
static throughput_notification_t result_indicated = sl_bt_gatt_disable;

/// Indication state for transmission state
static throughput_notification_t transmission_indicated = sl_bt_gatt_disable;

/// Advertising set handle
static uint8_t advertising_set_handle  = 0xff;

#ifdef SL_CATALOG_BLUETOOTH_FEATURE_EXTENDED_ADVERTISER_PRESENT
/// Advertising set handle on coded PHY
static uint8_t coded_advertising_set_handle  = 0xff;
#endif // SL_CATALOG_BLUETOOTH_FEATURE_EXTENDED_ADVERTISER_PRESENT

/// Enabled state
static bool enabled = false;

/// Finish test indicator
static bool finish_test = false;

/// Send transmission state
static bool send_transmission_state = false;

/// Data size for indication
static uint16_t indication_data_size = 0;

/// Data size for notification
static uint16_t notification_data_size = 0;

/// Data for notification
static uint8_t notification_data[THROUGHPUT_TX_DATA_SIZE] = { 0 };

/// Data for indication
static uint8_t indication_data[THROUGHPUT_TX_DATA_SIZE] = { 0 };

/// RSSI refresh timer
static sl_simple_timer_t refresh_timer;

/// Send timer
static sl_simple_timer_t send_timer;

/// Indication timer
static sl_simple_timer_t indication_timer;

/// Time storage variable
static throughput_count_t time_start = 0;

/// Time elapsed since start
static throughput_count_t time_elapsed = 0;

/// Bit counter variable
static throughput_count_t bytes_sent = 0;

/// Operation (indication, notification) counter variable
static throughput_count_t operation_count = 0;

/// Data size limit for fixed data mode
static uint32_t fixed_data_size = THROUGHPUT_PERIPHERAL_FIXED_DATA_SIZE;

/// Time limit for fixed time mode
static uint32_t fixed_time = THROUGHPUT_PERIPHERAL_FIXED_TIME;

/// Deep sleep enabled
static bool deep_sleep_enabled = THROUGHPUT_PERIPHERAL_TX_SLEEP_ENABLE;

/// Flag for send timer
static bool send_timer_rised = false;

/// Flag for indication timer
static bool indication_timer_rised = false;

/// Flag for indication
static bool indication_sent = false;

/// Flag for finish notification
static bool notification_sent = false;

/// Flag for indication confirmation
static bool indication_confirmed = false;

/// Power control status
static sl_bt_connection_power_reporting_mode_t power_control_enabled
  = sl_bt_connection_power_reporting_disable;

/// Requested notification data size
static uint8_t requested_notification_size =
  THROUGHPUT_PERIPHERAL_DATA_TRANSFER_SIZE_NOTIFICATIONS;

/// Requested indication data size
static uint8_t requested_indication_size =
  THROUGHPUT_PERIPHERAL_DATA_TRANSFER_SIZE_INDICATIONS;

/// Service handle
static uint32_t  service_handle = 0xFFFFFFFF;
/// Characteristic handles
static uint16_t  notifications_handle = 0xFFFF;
static uint16_t  indications_handle = 0xFFFF;
static uint16_t  transmission_handle = 0xFFFF;

/// Counter for checking data
static uint8_t received_counter = 0;

/// Flag for checking counter or accepting remote one
static bool first_packet = true;

/// Indicates that the test is from central to peripheral
static bool central_test = false;

/// Stores the found characteristics
static throughput_peripheral_characteristic_found_t characteristic_found;

/// Actions for the state machine
static action_t action = act_none;

/*******************************************************************************
 *******************  FORWARD DECLARATION OF FUNCTIONS   ***********************
 ******************************************************************************/
static void throughput_peripheral_calculate_notification_size(void);
static void throughput_peripheral_calculate_indication_size(void);
static void throughput_peripheral_generate_indications_data(void);
static void throughput_peripheral_generate_notifications_data(void);
static void throughput_peripheral_calculate_data_size(void);
static void throughput_peripheral_advertising_start(void);
static void throughput_peripheral_refresh_connected_state(void);
static void throughput_peripheral_on_refresh_timer_rise(sl_simple_timer_t *timer,
                                                        void *data);
static void throughput_peripheral_on_send_timer_rise(sl_simple_timer_t *timer,
                                                     void *data);
static void throughput_peripheral_on_indication_timer_rise(sl_simple_timer_t *timer,
                                                           void *data);
static void handle_throughput_peripheral_stop(bool send_transmission_on);
static void handle_throughput_peripheral_start(bool send_transmission_on);
static void throughput_peripheral_send_notification(void);
static void throughput_peripheral_indication_confirm(void);
static void throughput_peripheral_send_indication(void);
static void process_procedure_complete_event(sl_bt_msg_t *evt);
static void check_characteristic_uuid(sl_bt_msg_t *evt);
static void check_received_data(uint8_t * data, uint8_t len);

/// Send counter for package identification
static uint8_t send_counter = 0;

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/**************************************************************************//**
 * Calculates and sets the indication and notification data size
 *****************************************************************************/
static void throughput_peripheral_calculate_data_size(void)
{
  throughput_peripheral_calculate_indication_size();
  throughput_peripheral_calculate_notification_size();
  if (peripheral_state.test_type & sl_bt_gatt_indication) {
    peripheral_state.data_size = indication_data_size;
  } else {
    peripheral_state.data_size = notification_data_size;
  }
}

/**************************************************************************//**
 * Starts advertising on both channels if possible
 *****************************************************************************/
static void throughput_peripheral_advertising_start(void)
{
  sl_status_t sc;
  int16_t tx_power_min, tx_power_max;

  // Stop running advertising.
  sl_bt_advertiser_stop(advertising_set_handle);

  #ifdef SL_CATALOG_BLUETOOTH_FEATURE_EXTENDED_ADVERTISER_PRESENT
  sl_bt_advertiser_stop(coded_advertising_set_handle);
  #endif // SL_CATALOG_BLUETOOTH_FEATURE_EXTENDED_ADVERTISER_PRESENT

  // Convert power to mdBm
  int16_t power = ( ((int16_t)peripheral_state.tx_power_requested) * 10);
  sc = sl_bt_system_set_tx_power(CONFIG_TX_POWER_MIN,
                                 power,
                                 &tx_power_min,
                                 &tx_power_max);
  app_assert_status(sc);
  peripheral_state.tx_power = tx_power_max / 10;

  throughput_peripheral_on_power_change(peripheral_state.tx_power);

  // Delete sets.
  sl_bt_advertiser_delete_set(advertising_set_handle);

  #ifdef SL_CATALOG_BLUETOOTH_FEATURE_EXTENDED_ADVERTISER_PRESENT
  sl_bt_advertiser_delete_set(coded_advertising_set_handle);
  #endif // SL_CATALOG_BLUETOOTH_FEATURE_EXTENDED_ADVERTISER_PRESENT

  // Create an advertising set for 1M PHY.
  sc = sl_bt_advertiser_create_set(&advertising_set_handle);
  app_assert_status(sc);

  // Generate data for legacy advertising
  sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                             sl_bt_advertiser_general_discoverable);
  app_assert_status(sc);

  sc = sl_bt_advertiser_set_timing(advertising_set_handle,
                                   200, // min. adv. interval
                                   200, // max. adv. interval
                                   0,   // adv. duration
                                   0);  // max. num. adv. events
  app_assert_status(sc);

  sc = sl_bt_advertiser_set_channel_map(advertising_set_handle, 7);
  app_assert_status(sc);

  sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                     sl_bt_advertiser_connectable_scannable);
  app_assert_status(sc);

  #ifdef SL_CATALOG_BLUETOOTH_FEATURE_EXTENDED_ADVERTISER_PRESENT

  // Create an advertising set for CODED PHY
  sc = sl_bt_advertiser_create_set(&coded_advertising_set_handle);
  app_assert_status(sc);

  sc = sl_bt_advertiser_set_report_scan_request(coded_advertising_set_handle,
                                                false);
  app_assert_status(sc);

  // Generate data for extended advertising for CODED PHY
  sc = sl_bt_extended_advertiser_generate_data(coded_advertising_set_handle,
                                               sl_bt_advertiser_general_discoverable);
  app_assert_status(sc);

  sc = sl_bt_advertiser_set_timing(coded_advertising_set_handle,
                                   160,  // min. adv. interval
                                   160,  // max. adv. interval
                                   0,    // adv. duration
                                   0);   // max. num. adv. events
  app_assert_status(sc);

  sc = sl_bt_advertiser_set_channel_map(coded_advertising_set_handle,
                                        7);
  app_assert_status(sc);

  // Set PHY for extended advertiser
  sc = sl_bt_extended_advertiser_set_phy(coded_advertising_set_handle,
                                         sl_bt_gap_phy_coded,
                                         sl_bt_gap_phy_coded);

  app_assert( (sc == SL_STATUS_OK) || (sc == SL_STATUS_INVALID_PARAMETER),
              "[E: 0x%04x] Failed to set CODED PHY for the advertistment\n",
              (int)sc);

  if (sc == SL_STATUS_OK) {
    sc = sl_bt_extended_advertiser_start(coded_advertising_set_handle,
                                         sl_bt_advertiser_connectable_non_scannable,
                                         SL_BT_EXTENDED_ADVERTISER_INCLUDE_TX_POWER);
    app_assert_status(sc);
  }
  #endif // SL_CATALOG_BLUETOOTH_FEATURE_EXTENDED_ADVERTISER_PRESENT
}

/**************************************************************************//**
 * Calculate optimal notification size given current PDU and MTU sizes.
 *****************************************************************************/
static void throughput_peripheral_calculate_notification_size(void)
{
  if (requested_notification_size == 0
      || requested_notification_size
      > (peripheral_state.mtu_size - NOTIFICATION_GATT_HEADER)) {
    if ((peripheral_state.pdu_size != 0) && (peripheral_state.mtu_size != 0)) {
      // Optimally split over multiple over-the-air packets.
      if (peripheral_state.pdu_size <= peripheral_state.mtu_size) {
        notification_data_size = (peripheral_state.pdu_size
                                  - (L2CAP_HEADER + NOTIFICATION_GATT_HEADER))
                                 + ((peripheral_state.mtu_size - NOTIFICATION_GATT_HEADER
                                     - peripheral_state.pdu_size + (L2CAP_HEADER
                                                                    + NOTIFICATION_GATT_HEADER))
                                    / peripheral_state.pdu_size
                                    * peripheral_state.pdu_size);
      } else {
        // Single over-the-air packet, but accommodate room for headers.
        if ((peripheral_state.pdu_size - peripheral_state.mtu_size) <= L2CAP_HEADER) {
          // LL PDU size - (L2CAP+GATT Headers)
          notification_data_size = peripheral_state.pdu_size
                                   - (L2CAP_HEADER + NOTIFICATION_GATT_HEADER);
        } else {
          // Room for the whole MTU, so data payload is MTU - Header of operation.
          notification_data_size = peripheral_state.mtu_size - NOTIFICATION_GATT_HEADER;
        }
      }
    }
  } else {
    notification_data_size = requested_notification_size;
  }
}

/**************************************************************************//**
 * Calculate indication size given current MTU size.
 *****************************************************************************/
static void throughput_peripheral_calculate_indication_size(void)
{
  // MTU - 3B for indication GATT operation header.
  if (requested_indication_size == 0
      || requested_indication_size
      > (peripheral_state.mtu_size - INDICATION_GATT_HEADER)) {
    // If larger than max, use max for operation.
    indication_data_size = peripheral_state.mtu_size - INDICATION_GATT_HEADER;
  } else {
    // If smaller, use given.
    indication_data_size = requested_indication_size;
  }
}

/***************************************************************************//**
 * Checks received data for lost or error packages
 * @param[in] data received data
 * @param[in] len length of the data
 ******************************************************************************/
static void check_received_data(uint8_t * data, uint8_t len)
{
  uint8_t counter;

  if (len == 0) {
    peripheral_state.packet_error++;
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
    peripheral_state.packet_lost += lost_packet_count;
    // Sync to remote counter
    received_counter = counter;
  }

  // Check data for bit errors
  for (int i = 1; i < len; i++) {
    if ( data[i] != (uint8_t) 'a' + (uint8_t) ((i - 1) % 26) ) {
      peripheral_state.packet_error++;
      break;
    }
  }
}

/**************************************************************************//**
 * Function to generate payload
 *****************************************************************************/
static void throughput_peripheral_generate_notifications_data(void)
{
  notification_data[0] = send_counter;
  for (int i = 1; i < notification_data_size; i++) {
    notification_data[i] = (uint8_t) 'a' + (uint8_t) ((i - 1) % 26);
  }
  send_counter = (send_counter + 1) % 100;
}

/**************************************************************************//**
 * Function to generate payload
 *****************************************************************************/
static void throughput_peripheral_generate_indications_data(void)
{
  indication_data[0] = send_counter;
  for (int i = 1; i < indication_data_size; i++) {
    indication_data[i] = (uint8_t) 'a' + (uint8_t) ((i - 1) % 26);
  }
  send_counter = (send_counter + 1) % 100;
}

/**************************************************************************//**
 * Refresh throughput state
 *****************************************************************************/
static void throughput_peripheral_refresh_connected_state(void)
{
  if ( ( (peripheral_state.notifications == sl_bt_gatt_notification)
         || (peripheral_state.indications == sl_bt_gatt_indication) )
       && (result_indicated != sl_bt_gatt_disable)
       && (transmission_indicated != sl_bt_gatt_disable)) {
    peripheral_state.state = THROUGHPUT_STATE_SUBSCRIBED;
  } else {
    peripheral_state.state = THROUGHPUT_STATE_CONNECTED;
  }
  throughput_peripheral_on_state_change(peripheral_state.state);
}

/**************************************************************************//**
 * Refresh RSSI timer callback.
 *****************************************************************************/
static void throughput_peripheral_on_refresh_timer_rise(sl_simple_timer_t *timer,
                                                        void *data)
{
  (void) data;
  (void) timer;
  sl_status_t sc;
  if (connection && peripheral_state.state != THROUGHPUT_STATE_TEST) {
    sc = sl_bt_connection_get_rssi(connection);
    app_assert_status(sc);
  }
}

/**************************************************************************//**
 * Send timer callback.
 *****************************************************************************/
static void throughput_peripheral_on_send_timer_rise(sl_simple_timer_t *timer,
                                                     void *data)
{
  (void) data;
  (void) timer;
  send_timer_rised = true;
}

/**************************************************************************//**
 * Indication timer callback.
 *****************************************************************************/
static void throughput_peripheral_on_indication_timer_rise(sl_simple_timer_t *timer,
                                                           void *data)
{
  (void) data;
  (void) timer;
  indication_timer_rised = true;
}

/**************************************************************************//**
 * Finishes throughput test.
 *****************************************************************************/
static void handle_throughput_peripheral_stop(bool send_transmission_on)
{
  sl_status_t sc;

  // If first called finish
  if (peripheral_state.state != THROUGHPUT_STATE_TEST_FINISH) {
    // Set state to finish
    peripheral_state.state = THROUGHPUT_STATE_TEST_FINISH;
    // Test type off state
    peripheral_state.test_type = sl_bt_gatt_disable;

    // stop timer
    sl_simple_timer_stop(&indication_timer);

    send_transmission_state = send_transmission_on;

    notification_sent = false;
    indication_sent = false;
    indication_confirmed = false;
    send_timer_rised = false;
    indication_timer_rised = false;

    finish_test = false;
  }
  if (send_transmission_on && !notification_sent) {
    // Send out notification
    sc = sl_bt_gatt_server_send_notification(connection,
                                             gattdb_transmission_on,
                                             1,
                                             &TRANSMISSION_OFF);
    if (sc == SL_STATUS_OK) {
      notification_sent = true;
      indication_sent = false;
    }
  } else if (!send_transmission_on || notification_sent) {
    if (!indication_sent) {
      // Get elapsed time
      time_elapsed = sl_sleeptimer_get_tick_count64() - time_start;
      peripheral_state.count = operation_count;

      peripheral_state.time  = (throughput_time_t)((float)time_elapsed
                                                   / sl_sleeptimer_get_timer_frequency());

      // Calculate throughput
      peripheral_state.throughput = (throughput_value_t)((float)bytes_sent
                                                         * 8
                                                         / ((float)time_elapsed
                                                            / sl_sleeptimer_get_timer_frequency()));

      indication_confirmed = false;
      indication_timer_rised = false;

      sc = sl_bt_gatt_server_send_indication(connection,
                                             gattdb_throughput_result,
                                             sizeof(peripheral_state.throughput),
                                             (uint8_t *) &(peripheral_state.throughput));
      if (sc == SL_STATUS_OK) {
        indication_sent = true;
        sc = sl_simple_timer_start(&indication_timer,
                                   THROUGHPUT_TX_INDICATION_TIMEOUT,
                                   throughput_peripheral_on_indication_timer_rise,
                                   NULL,
                                   false);
        app_assert_status(sc);
      }
    } else {
      if (indication_confirmed || indication_timer_rised) {
        if (!deep_sleep_enabled) {
          // Enable sleep
          sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
        }
        // Set mode
        throughput_peripheral_refresh_connected_state();

        notification_sent = false;
        indication_sent = false;
        indication_confirmed = false;
        send_timer_rised = false;
        indication_timer_rised = false;

        // Start refresh timer
        sl_simple_timer_start(&refresh_timer,
                              THROUGHPUT_TX_REFRESH_TIMER_PERIOD,
                              throughput_peripheral_on_refresh_timer_rise,
                              NULL,
                              true);

        // Indicate the state change
        if (central_test) {
          throughput_peripheral_on_finish(peripheral_state.throughput,
                                          peripheral_state.count);
        } else {
          throughput_peripheral_on_finish_reception(peripheral_state.throughput,
                                                    peripheral_state.count,
                                                    peripheral_state.packet_lost,
                                                    peripheral_state.packet_error,
                                                    peripheral_state.time);
        }
      }
    }
  }
}

/**************************************************************************//**
 * Starts throughput test.
 *****************************************************************************/
static void handle_throughput_peripheral_start(bool send_transmission_on)
{
  sl_status_t sc;

  // Clear transmission variables
  bytes_sent = 0;
  send_counter = 0;
  peripheral_state.throughput = 0;
  peripheral_state.count = 0;
  operation_count = 0;

  // Clear reception variables
  received_counter = 0;
  first_packet = true;
  peripheral_state.packet_error = 0;
  peripheral_state.packet_lost = 0;

  // Clear flags
  indication_timer_rised = false;
  indication_sent = false;
  indication_confirmed = false;

  send_timer_rised = false;

  // Stop timers
  sl_simple_timer_stop(&indication_timer);
  sl_simple_timer_stop(&send_timer);
  sl_simple_timer_stop(&refresh_timer);

  // Generate data to send
  if (peripheral_state.test_type & sl_bt_gatt_notification) {
    throughput_peripheral_generate_notifications_data();
  }
  if (peripheral_state.test_type & sl_bt_gatt_indication) {
    throughput_peripheral_generate_indications_data();
  }
  if (send_transmission_on) {
    sc = sl_bt_gatt_server_send_notification(connection,
                                             gattdb_transmission_on,
                                             1,
                                             &TRANSMISSION_ON);
    app_assert_status(sc);
  }

  if (peripheral_state.mode == THROUGHPUT_MODE_FIXED_TIME) {
    sc = sl_simple_timer_start(&send_timer,
                               fixed_time,
                               throughput_peripheral_on_send_timer_rise,
                               NULL,
                               false);
    app_assert_status(sc);
  }
  if (!deep_sleep_enabled) {
    // Disable sleep
    sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
  }

  peripheral_state.state = THROUGHPUT_STATE_TEST;
  throughput_peripheral_on_state_change(peripheral_state.state);
  throughput_peripheral_on_start();

  // Start timer
  time_start = sl_sleeptimer_get_tick_count64();
}

/**************************************************************************//**
 * Sends out single notification for the test.
 *****************************************************************************/
static void throughput_peripheral_send_notification(void)
{
  sl_status_t sc;
  if (finish_test) {
    handle_throughput_peripheral_stop(true);
  } else {
    if (send_timer_rised) {
      send_timer_rised = false;
      handle_throughput_peripheral_stop(true);
    } else {
      sc = sl_bt_gatt_server_send_notification(connection,
                                               gattdb_throughput_notifications,
                                               notification_data_size,
                                               notification_data);
      if (sc == SL_STATUS_OK) {
        bytes_sent += (notification_data_size);
        operation_count++;
        throughput_peripheral_generate_notifications_data();
        if ( (peripheral_state.mode == THROUGHPUT_MODE_FIXED_LENGTH)
             && (bytes_sent >= (fixed_data_size))) {
          handle_throughput_peripheral_stop(true);
        }
      }
    }
  }
}

/**************************************************************************//**
 * Indication confirmed callback.
 *****************************************************************************/
static void throughput_peripheral_indication_confirm(void)
{
  indication_confirmed = true;
}

/**************************************************************************//**
 * Sends out single indication for the test.
 *****************************************************************************/
static void throughput_peripheral_send_indication(void)
{
  sl_status_t sc;

  if (indication_sent) {
    if (indication_confirmed) {
      // move on.
      bytes_sent += (indication_data_size);
      operation_count++;

      sl_simple_timer_stop(&indication_timer);

      indication_sent = false;
      indication_confirmed = false;

      if ( (peripheral_state.mode == THROUGHPUT_MODE_FIXED_LENGTH)
           && (bytes_sent >= (fixed_data_size))) {
        handle_throughput_peripheral_stop(true);
      } else if (send_timer_rised) {
        send_timer_rised = false;
        handle_throughput_peripheral_stop(true);
      }
    } else {
      if (indication_timer_rised) {
        handle_throughput_peripheral_stop(true);
      }
    }
  } else {
    // No indication sent, send it out
    if (finish_test) {
      handle_throughput_peripheral_stop(true);
    } else {
      throughput_peripheral_generate_indications_data();

      indication_confirmed = false;

      sl_simple_timer_stop(&indication_timer);

      sc = sl_bt_gatt_server_send_indication(connection,
                                             gattdb_throughput_indications,
                                             indication_data_size,
                                             indication_data);
      (void) sc;
      indication_sent = true;
      sc = sl_simple_timer_start(&indication_timer,
                                 THROUGHPUT_TX_INDICATION_TIMEOUT,
                                 throughput_peripheral_on_indication_timer_rise,
                                 NULL,
                                 false);
      app_assert_status(sc);
    }
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/**************************************************************************//**
 * Enables the transmission.
 *****************************************************************************/
void throughput_peripheral_enable(void)
{
  sl_status_t sc;
  int16_t tx_power_min, tx_power_max;

  // Enable UI
  throughput_ui_init();

  memset(notification_data, 0, THROUGHPUT_TX_DATA_SIZE);
  memset(indication_data, 0, THROUGHPUT_TX_DATA_SIZE);

  peripheral_state.role          = THROUGHPUT_ROLE_PERIPHERAL;
  peripheral_state.state         = THROUGHPUT_STATE_DISCONNECTED;
  peripheral_state.mode          = THROUGHPUT_PERIPHERAL_MODE_DEFAULT;
  peripheral_state.tx_power      = THROUGHPUT_PERIPHERAL_TX_POWER;
  peripheral_state.rssi          = 0;
  peripheral_state.phy           = sl_bt_gap_phy_coding_1m_uncoded;
  peripheral_state.interval      = 0;
  peripheral_state.pdu_size      = 0;
  peripheral_state.mtu_size      = THROUGHPUT_PERIPHERAL_MTU_SIZE;
  peripheral_state.data_size     = 0;
  peripheral_state.notifications = sl_bt_gatt_disable;
  peripheral_state.indications   = sl_bt_gatt_disable;
  peripheral_state.throughput    = 0;
  peripheral_state.count         = 0;
  peripheral_state.packet_error  = 0;
  peripheral_state.packet_lost   = 0;

  if (THROUGHPUT_PERIPHERAL_TX_POWER_CONTROL_ENABLE) {
    power_control_enabled = sl_bt_connection_power_reporting_enable;
  } else {
    power_control_enabled = sl_bt_connection_power_reporting_disable;
  }

  // Convert power to mdBm
  int16_t power = ( ((int16_t)peripheral_state.tx_power) * 10);
  sc = sl_bt_system_set_tx_power(CONFIG_TX_POWER_MIN,
                                 power,
                                 &tx_power_min,
                                 &tx_power_max);
  app_assert_status(sc);
  peripheral_state.tx_power = tx_power_max / 10;
  throughput_peripheral_on_power_change(peripheral_state.tx_power);

  sc = sl_bt_gatt_server_set_max_mtu(peripheral_state.mtu_size, &(peripheral_state.mtu_size));
  app_assert_status(sc);

  // Start advertising
  throughput_peripheral_advertising_start();

  enabled = true;

  throughput_ui_set_all(peripheral_state);
  central_test = false;
}

/**************************************************************************//**
 * Process step for throughput peripheral.
 *****************************************************************************/
void throughput_peripheral_step(void)
{
  // Return early, if the central started a test
  if (central_test) {
    return;
  }
  if (peripheral_state.state == THROUGHPUT_STATE_TEST) {
    if (peripheral_state.test_type & sl_bt_gatt_indication) {
      throughput_peripheral_send_indication();
    }
    if (peripheral_state.test_type & sl_bt_gatt_notification) {
      throughput_peripheral_send_notification();
    }
  } else if (peripheral_state.state == THROUGHPUT_STATE_TEST_FINISH) {
    handle_throughput_peripheral_stop(send_transmission_state);
  }
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 *****************************************************************************/
void throughput_peripheral_on_bt_event(sl_bt_msg_t *evt)
{
  bool response;
  sl_status_t sc;
  uint8_t data;

  if (!enabled) {
    return;
  }

  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_gatt_server_attribute_value_id:
      if (gattdb_transmission_on == evt->data.evt_gatt_server_attribute_value.attribute) {
        response = false;
        data = evt->data.evt_gatt_server_attribute_value.value.data[0];
        if (data > 0) {
          if (peripheral_state.state == THROUGHPUT_STATE_SUBSCRIBED) {
            peripheral_state.test_type = sl_bt_gatt_disable;
            if (peripheral_state.notifications && peripheral_state.indications ) {
              if ( (peripheral_state.notifications & sl_bt_gatt_notification)
                   && (data & sl_bt_gatt_notification) ) {
                peripheral_state.test_type = sl_bt_gatt_notification;
              } else if ( (peripheral_state.indications & sl_bt_gatt_indication)
                          && (data & sl_bt_gatt_indication) ) {
                peripheral_state.test_type = sl_bt_gatt_indication;
              }
            } else if (peripheral_state.indications & sl_bt_gatt_indication) {
              peripheral_state.test_type = sl_bt_gatt_indication;
            } else if (peripheral_state.notifications & sl_bt_gatt_notification) {
              peripheral_state.test_type = sl_bt_gatt_notification;
            }
            if (peripheral_state.test_type & sl_bt_gatt_indication) {
              throughput_peripheral_generate_indications_data();
              response = true;
            } else if (peripheral_state.test_type & sl_bt_gatt_notification) {
              throughput_peripheral_generate_notifications_data();
              response = true;
            }
            if (response) {
              handle_throughput_peripheral_start(false);
            }
          }
        } else {
          if (peripheral_state.state == THROUGHPUT_STATE_TEST) {
            handle_throughput_peripheral_stop(false);
            response = true;
          }
        }
        sl_bt_gatt_server_send_user_write_response(connection,
                                                   gattdb_transmission_on,
                                                   response);
      }
      break;

    case sl_bt_evt_connection_tx_power_id:
      if (peripheral_state.state != THROUGHPUT_STATE_TEST) {
        peripheral_state.tx_power = evt->data.evt_connection_tx_power.power_level;
        throughput_peripheral_on_power_change(peripheral_state.tx_power);
      }
      break;

    case sl_bt_evt_connection_rssi_id:
      peripheral_state.rssi = evt->data.evt_connection_rssi.rssi;
      throughput_peripheral_on_rssi_change(peripheral_state.rssi);
      break;

    case sl_bt_evt_gatt_mtu_exchanged_id:
      peripheral_state.mtu_size = evt->data.evt_gatt_mtu_exchanged.mtu;
      throughput_peripheral_calculate_data_size();

      sc = sl_bt_gatt_server_write_attribute_value(gattdb_mtu_size,
                                                   0,
                                                   1,
                                                   (uint8_t *)&peripheral_state.mtu_size);
      app_assert_status(sc);

      sc = sl_bt_gatt_server_notify_all(gattdb_mtu_size,
                                        1,
                                        (uint8_t *)&peripheral_state.mtu_size);
      app_assert_status(sc);

      throughput_peripheral_on_connection_settings_change(peripheral_state.interval,
                                                          peripheral_state.pdu_size,
                                                          peripheral_state.mtu_size,
                                                          peripheral_state.data_size);
      break;

    case sl_bt_evt_connection_parameters_id:
      peripheral_state.interval = evt->data.evt_connection_parameters.interval;
      peripheral_state.connection_responder_latency
        = evt->data.evt_connection_parameters.latency;
      peripheral_state.connection_timeout
        = evt->data.evt_connection_parameters.timeout;

      sc = sl_bt_gatt_server_get_mtu(evt->data.evt_connection_parameters.connection,
                                     &(peripheral_state.mtu_size));
      app_assert_status(sc);

      peripheral_state.pdu_size = evt->data.evt_connection_parameters.txsize;
      throughput_peripheral_calculate_data_size();

      sc = sl_bt_gatt_server_write_attribute_value(gattdb_pdu_size,
                                                   0,
                                                   1,
                                                   (uint8_t *)&peripheral_state.pdu_size);
      app_assert_status(sc);
      sc = sl_bt_gatt_server_write_attribute_value(gattdb_mtu_size,
                                                   0,
                                                   1,
                                                   (uint8_t *)&peripheral_state.mtu_size);
      app_assert_status(sc);
      sc = sl_bt_gatt_server_write_attribute_value(gattdb_connection_interval,
                                                   0,
                                                   4,
                                                   (uint8_t *)&peripheral_state.interval);
      app_assert_status(sc);
      sc = sl_bt_gatt_server_write_attribute_value(gattdb_responder_latency,
                                                   0,
                                                   4,
                                                   (uint8_t *)&peripheral_state.connection_responder_latency);
      app_assert_status(sc);
      sc = sl_bt_gatt_server_write_attribute_value(gattdb_supervision_timeout,
                                                   0,
                                                   4,
                                                   (uint8_t *)&peripheral_state.connection_timeout);
      app_assert_status(sc);

      sc = sl_bt_gatt_server_notify_all(gattdb_pdu_size,
                                        1,
                                        (uint8_t *)&peripheral_state.pdu_size);
      app_assert_status(sc);
      sc = sl_bt_gatt_server_notify_all(gattdb_mtu_size,
                                        1,
                                        (uint8_t *)&peripheral_state.mtu_size);
      app_assert_status(sc);
      sc = sl_bt_gatt_server_notify_all(gattdb_connection_interval,
                                        4,
                                        (uint8_t *)&peripheral_state.interval);
      app_assert_status(sc);
      sc = sl_bt_gatt_server_notify_all(gattdb_responder_latency,
                                        4,
                                        (uint8_t *)&peripheral_state.connection_responder_latency);
      app_assert_status(sc);
      sc = sl_bt_gatt_server_notify_all(gattdb_supervision_timeout,
                                        4,
                                        (uint8_t *)&peripheral_state.connection_timeout);
      app_assert_status(sc);

      throughput_peripheral_on_connection_settings_change(peripheral_state.interval,
                                                          peripheral_state.pdu_size,
                                                          peripheral_state.mtu_size,
                                                          peripheral_state.data_size);
      break;

    case sl_bt_evt_connection_phy_status_id:
      peripheral_state.phy = (throughput_phy_t)evt->data.evt_connection_phy_status.phy;

      sc = sl_bt_gatt_server_write_attribute_value(gattdb_connection_phy,
                                                   0,
                                                   1,
                                                   (uint8_t *)&peripheral_state.phy);
      app_assert_status(sc);

      sc = sl_bt_gatt_server_notify_all(gattdb_connection_phy,
                                        1,
                                        (uint8_t *)&peripheral_state.phy);
      app_assert_status(sc);

      throughput_peripheral_on_phy_change(peripheral_state.phy);
      break;

    case sl_bt_evt_connection_closed_id:
      connection = 0;
      // Delete the connection, reset variables and start advertising
      peripheral_state.state = THROUGHPUT_STATE_DISCONNECTED;
      sl_simple_timer_stop(&refresh_timer);
      sl_simple_timer_stop(&send_timer);
      peripheral_state.notifications = sl_bt_gatt_disable;
      peripheral_state.indications = sl_bt_gatt_disable;
      result_indicated = sl_bt_gatt_disable;
      transmission_indicated = sl_bt_gatt_disable;
      throughput_peripheral_on_state_change(peripheral_state.state);
      throughput_peripheral_advertising_start();
      break;

    case sl_bt_evt_connection_opened_id:
      if (peripheral_state.state == THROUGHPUT_STATE_DISCONNECTED) {
        // Store the connection and disable advertising
        connection = evt->data.evt_connection_opened.connection;
        sc = sl_bt_advertiser_stop(advertising_set_handle);
        app_assert_status(sc);

        #ifdef SL_CATALOG_BLUETOOTH_FEATURE_EXTENDED_ADVERTISER_PRESENT
        sc = sl_bt_advertiser_stop(coded_advertising_set_handle);
        app_assert_status(sc);
        #endif // SL_CATALOG_BLUETOOTH_FEATURE_EXTENDED_ADVERTISER_PRESENT

        peripheral_state.state = THROUGHPUT_STATE_CONNECTED;
        throughput_peripheral_refresh_connected_state();
        sl_simple_timer_start(&refresh_timer,
                              THROUGHPUT_TX_REFRESH_TIMER_PERIOD,
                              throughput_peripheral_on_refresh_timer_rise,
                              NULL,
                              true);

        // Set remote connection power reporting - needed for Power Control
        sc = sl_bt_connection_set_remote_power_reporting(connection,
                                                         power_control_enabled);
        app_assert_status(sc);

        // Subscribe to service provided by the mobile app
        sc = sl_bt_gatt_discover_primary_services_by_uuid(connection,
                                                          UUID_LEN,
                                                          peripheral_service_uuid);
        app_assert_status(sc);
      }
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      if ( (gattdb_throughput_result == evt->data.evt_gatt_server_characteristic_status.characteristic)
           && (sl_bt_gatt_server_confirmation == evt->data.evt_gatt_server_characteristic_status.status_flags) ) {
        // Result confirmed
        throughput_peripheral_indication_confirm();
      } else if ( (gattdb_throughput_indications == evt->data.evt_gatt_server_characteristic_status.characteristic)
                  && (sl_bt_gatt_server_confirmation == evt->data.evt_gatt_server_characteristic_status.status_flags) ) {
        throughput_peripheral_indication_confirm();
      } else {
        if (sl_bt_gatt_server_client_config == evt->data.evt_gatt_server_characteristic_status.status_flags ) {
          if (gattdb_throughput_result == evt->data.evt_gatt_server_characteristic_status.characteristic) {
            result_indicated = (throughput_notification_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags;
          }
          if (gattdb_transmission_on == evt->data.evt_gatt_server_characteristic_status.characteristic) {
            transmission_indicated = (throughput_notification_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags;
          }
          if (gattdb_throughput_indications == evt->data.evt_gatt_server_characteristic_status.characteristic) {
            peripheral_state.indications = (throughput_notification_t)(evt->data.evt_gatt_server_characteristic_status.client_config_flags
                                                                       & sl_bt_gatt_indication);
            throughput_peripheral_on_indication_change(peripheral_state.indications);
          }
          if (gattdb_throughput_notifications == evt->data.evt_gatt_server_characteristic_status.characteristic) {
            peripheral_state.notifications = (throughput_notification_t)(evt->data.evt_gatt_server_characteristic_status.client_config_flags
                                                                         & sl_bt_gatt_notification);
            throughput_peripheral_on_notification_change(peripheral_state.notifications);
          }
          throughput_peripheral_refresh_connected_state();
        }
      }
      break;
    case sl_bt_evt_gatt_procedure_completed_id:
      process_procedure_complete_event(evt);
      break;
    case sl_bt_evt_gatt_characteristic_id:
      check_characteristic_uuid(evt);
      break;
    case sl_bt_evt_gatt_service_id:
      if (evt->data.evt_gatt_service.uuid.len == UUID_LEN) {
        if (memcmp(peripheral_service_uuid, evt->data.evt_gatt_service.uuid.data, UUID_LEN) == 0) {
          service_handle = evt->data.evt_gatt_service.service;
          action = act_discover_service;
        }
      }
      break;
    case sl_bt_evt_gatt_characteristic_value_id:
      // Handle remote start/stop event
      if (evt->data.evt_gatt_characteristic_value.characteristic == transmission_handle) {
        if (evt->data.evt_gatt_characteristic_value.value.data[0]) {
          central_test = true;
          handle_throughput_peripheral_start(false);
        } else {
          handle_throughput_peripheral_stop(false);
          central_test = false;
        }
      } else if (evt->data.evt_gatt_characteristic_value.characteristic == indications_handle
                 || evt->data.evt_gatt_characteristic_value.characteristic == notifications_handle) {
        // Handle received data
        // Send confirmation if needed
        if (evt->data.evt_gatt_characteristic_value.characteristic == indications_handle) {
          if (evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_indication) {
            sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);
          }
        }
        // Check data for loss or error
        check_received_data(evt->data.evt_gatt_characteristic_value.value.data,
                            evt->data.evt_gatt_characteristic_value.value.len);
        // Count bytes and operation
        bytes_sent += (evt->data.evt_gatt_characteristic_value.value.len);
        operation_count++;
      }
      // We silently ignore other data.
      break;
    default:
      break;
  }
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
        sc = sl_bt_gatt_discover_characteristics(connection, service_handle);
        app_assert_status(sc);
        action = act_discover_characteristics;
      }
      break;
    case act_discover_characteristics:
      action = act_none;
      app_assert_status(procedure_result);
      if (!procedure_result) {
        if (characteristic_found.all == THROUGHPUT_PERIPHERAL_CHARACTERISTICS_ALL) {
          sc = sl_bt_gatt_set_characteristic_notification(connection, notifications_handle, sl_bt_gatt_notification);
          app_assert_status(sc);
          action = act_enable_notification;
        }
      }
      break;
    case act_enable_notification:
      action = act_none;
      app_assert_status(procedure_result);
      if (!procedure_result) {
        sl_bt_gatt_set_characteristic_notification(connection, indications_handle, sl_bt_gatt_indication);
        action = act_enable_indication;
      }
      break;
    case act_enable_indication:
      action = act_enable_indication;
      app_assert_status(procedure_result);
      if (!procedure_result) {
        sc = sl_bt_gatt_set_characteristic_notification(connection, transmission_handle, sl_bt_gatt_notification);
        app_assert_status(sc);
        // Clear the display
        throughput_ui_set_throughput(0);
        throughput_ui_set_count(0);
        throughput_ui_update();
        action = act_none;
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
    if (memcmp(peripheral_notifications_characteristic_uuid, evt->data.evt_gatt_characteristic.uuid.data, UUID_LEN) == 0) {
      notifications_handle = evt->data.evt_gatt_characteristic.characteristic;
      characteristic_found.characteristic.notification = true;
    } else if (memcmp(peripheral_indications_characteristic_uuid, evt->data.evt_gatt_characteristic.uuid.data, UUID_LEN) == 0) {
      indications_handle = evt->data.evt_gatt_characteristic.characteristic;
      characteristic_found.characteristic.indication = true;
    } else if (memcmp(peripheral_transmission_characteristic_uuid, evt->data.evt_gatt_characteristic.uuid.data, UUID_LEN) == 0) {
      transmission_handle = evt->data.evt_gatt_characteristic.characteristic;
      characteristic_found.characteristic.transmission_on = true;
    }
  }
}

/**************************************************************************//**
 * Sets the the transmission power.
 *****************************************************************************/
sl_status_t throughput_peripheral_set_tx_power(throughput_tx_power_t tx_power,
                                               bool power_control,
                                               bool deep_sleep)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && peripheral_state.state != THROUGHPUT_STATE_TEST) {
    peripheral_state.tx_power_requested = tx_power;
    deep_sleep_enabled = deep_sleep;

    if (power_control) {
      power_control_enabled = sl_bt_connection_power_reporting_enable;
    } else {
      power_control_enabled = sl_bt_connection_power_reporting_disable;
    }

    // Reconnect if required
    if (peripheral_state.state != THROUGHPUT_STATE_DISCONNECTED) {
      // Close connection and apply power
      res = sl_bt_connection_close(connection);
    } else {
      // Restart advertising and apply power
      throughput_peripheral_advertising_start();
    }
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Sets the the transmission sizes.
 *****************************************************************************/
sl_status_t throughput_peripheral_set_data_size(uint8_t mtu,
                                                uint8_t ind_data,
                                                uint8_t not_data)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && peripheral_state.state != THROUGHPUT_STATE_TEST) {
    peripheral_state.mtu_size = mtu;
    res = sl_bt_gatt_server_set_max_mtu(peripheral_state.mtu_size, &(peripheral_state.mtu_size));

    if (res == SL_STATUS_OK) {
      requested_indication_size = ind_data;
      requested_notification_size = not_data;
      throughput_peripheral_calculate_data_size();
      throughput_peripheral_on_connection_settings_change(peripheral_state.interval,
                                                          peripheral_state.pdu_size,
                                                          peripheral_state.mtu_size,
                                                          peripheral_state.data_size);
      // Reconnect if required
      if (peripheral_state.state != THROUGHPUT_STATE_DISCONNECTED) {
        sl_bt_connection_close(connection);
      }
    }
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Sets the the transmission mode.
 *****************************************************************************/
sl_status_t throughput_peripheral_set_mode(throughput_mode_t mode,
                                           uint32_t amount)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && peripheral_state.state != THROUGHPUT_STATE_TEST) {
    if (mode == THROUGHPUT_MODE_FIXED_LENGTH) {
      fixed_data_size = amount;
    } else if (mode == THROUGHPUT_MODE_FIXED_TIME) {
      fixed_time = amount;
    }
    peripheral_state.mode = mode;
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Starts the the transmission.
 *****************************************************************************/
sl_status_t throughput_peripheral_start(throughput_notification_t type)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && peripheral_state.state == THROUGHPUT_STATE_SUBSCRIBED) {
    if ((peripheral_state.indications & sl_bt_gatt_indication)
        && (peripheral_state.notifications & sl_bt_gatt_notification)
        && (type != sl_bt_gatt_disable) ) {
      peripheral_state.test_type = sl_bt_gatt_notification;
    }
    if (type == sl_bt_gatt_indication
        && (peripheral_state.indications & sl_bt_gatt_indication) ) {
      peripheral_state.test_type = sl_bt_gatt_indication;
    } else if (type == sl_bt_gatt_notification
               && (peripheral_state.notifications & sl_bt_gatt_notification) ) {
      peripheral_state.test_type = sl_bt_gatt_notification;
    }
    if (peripheral_state.test_type != sl_bt_gatt_disable) {
      if (peripheral_state.test_type & sl_bt_gatt_notification) {
        peripheral_state.data_size = notification_data_size;
      }
      if (peripheral_state.test_type & sl_bt_gatt_indication) {
        peripheral_state.data_size = indication_data_size;
      }
      throughput_peripheral_on_connection_settings_change(peripheral_state.interval,
                                                          peripheral_state.pdu_size,
                                                          peripheral_state.mtu_size,
                                                          peripheral_state.data_size);
      handle_throughput_peripheral_start(true);
    } else {
      res = SL_STATUS_INVALID_TYPE;
    }
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Stops the the transmission.
 *****************************************************************************/
sl_status_t throughput_peripheral_stop(void)
{
  sl_status_t res = SL_STATUS_OK;
  if (enabled && peripheral_state.state == THROUGHPUT_STATE_TEST) {
    finish_test = true;
  } else {
    res = SL_STATUS_INVALID_STATE;
  }
  return res;
}

/**************************************************************************//**
 * Checks if it is ok to sleep now
 *****************************************************************************/
bool throughput_peripheral_is_ok_to_sleep(void)
{
  bool ret = true;
  if (enabled && !deep_sleep_enabled && (peripheral_state.state == THROUGHPUT_STATE_TEST)) {
    ret = false;
  }
  return ret;
}
/**************************************************************************//**
 * Routine for power manager handler
 *****************************************************************************/
sl_power_manager_on_isr_exit_t throughput_peripheral_sleep_on_isr_exit(void)
{
  sl_power_manager_on_isr_exit_t ret = SL_POWER_MANAGER_IGNORE;
  if (enabled && !deep_sleep_enabled && (peripheral_state.state == THROUGHPUT_STATE_TEST)) {
    ret = SL_POWER_MANAGER_WAKEUP;
  }
  return ret;
}

/*******************************************************************************
 *********************** CALLBACK WEAK IMPLEMENTATIONS *************************
 ******************************************************************************/

/**************************************************************************//**
 * Weak implementation of callback to handle role settings.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_role_set(throughput_role_t role)
{
  throughput_ui_set_role(role);
  throughput_ui_update();
}

/**************************************************************************//**
 * Weak implementation of callback to handle state change.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_state_change(throughput_state_t state)
{
  throughput_ui_set_state(state);
  throughput_ui_update();
}

/**************************************************************************//**
 * Weak implementation of callback to handle mode change.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_mode_change(throughput_mode_t mode)
{
  (void) mode;
}

/**************************************************************************//**
 * Weak implementation of callback to handle transmission start event.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_start(void)
{
  // Do nothing
}

/**************************************************************************//**
 * Weak implementation of callback to handle transmission finished event.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_finish(throughput_value_t throughput,
                                             throughput_count_t count)
{
  throughput_ui_set_throughput(throughput);
  throughput_ui_set_count(count);
  throughput_ui_update();
}

/**************************************************************************//**
 * Weak implementation of callback to handle reception finished event.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_finish_reception(throughput_value_t throughput,
                                                       throughput_count_t count,
                                                       throughput_count_t lost,
                                                       throughput_count_t error,
                                                       throughput_time_t time)
{
  throughput_peripheral_on_finish(throughput, count);
  app_log_info(THROUGHPUT_UI_LOST_FORMAT APP_LOG_NEW_LINE, ((int)lost));
  app_log_info(THROUGHPUT_UI_ERROR_FORMAT APP_LOG_NEW_LINE, ((int)error));
  app_log_info(THROUGHPUT_UI_TIME_FORMAT APP_LOG_NEW_LINE, ((int)time));
}

/**************************************************************************//**
 * Weak implementation of callback to handle TX power changed event.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_power_change(throughput_tx_power_t power)
{
  throughput_ui_set_tx_power(power);
  throughput_ui_update();
}

/**************************************************************************//**
 * Weak implementation of callback to handle RSSI changed event.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_rssi_change(throughput_rssi_t rssi)
{
  throughput_ui_set_rssi(rssi);
  throughput_ui_update();
}

/**************************************************************************//**
 * Weak implementation of callback to handle phy changed event.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_phy_change(throughput_phy_t phy)
{
  throughput_ui_set_phy(phy);
  throughput_ui_update();
}

/**************************************************************************//**
 * Weak implementation of callback to handle connection settings changes.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_connection_settings_change(throughput_time_t interval,
                                                                 throughput_pdu_size_t pdu,
                                                                 throughput_mtu_size_t mtu,
                                                                 throughput_data_size_t data)
{
  throughput_ui_set_connection_interval(interval);
  throughput_ui_set_pdu_size(pdu);
  throughput_ui_set_mtu_size(mtu);
  throughput_ui_set_data_size(data);
  throughput_ui_update();
}

/**************************************************************************//**
 * Weak implementation of callback to handle notification changed event.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_notification_change(throughput_notification_t notification)
{
  throughput_ui_set_notifications(notification);
  throughput_ui_update();
}

/**************************************************************************//**
 * Weak implementation of callback to handle indication changed event.
 *****************************************************************************/
SL_WEAK void throughput_peripheral_on_indication_change(throughput_notification_t indication)
{
  throughput_ui_set_indications(indication);
  throughput_ui_update();
}

/*******************************************************************************
 **************************** CLI RELATED FUNCTIONS ****************************
 ******************************************************************************/

#ifdef SL_CATALOG_CLI_PRESENT
/***************************************************************************//**
 * CLI command for peripheral stop
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_peripheral_stop(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  sl_status_t sc;
  if (peripheral_state.state == THROUGHPUT_STATE_TEST) {
    sc = throughput_peripheral_stop();
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
 * CLI command for peripheral start
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_peripheral_start(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  sl_status_t sc;
  if (peripheral_state.state == THROUGHPUT_STATE_SUBSCRIBED) {
    uint8_t test = sl_cli_get_argument_uint8(arguments, 0);
    sc = throughput_peripheral_start((throughput_notification_t)test);
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
 * CLI command for peripheral status
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_peripheral_status(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  switch (peripheral_state.state) {
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

  if (peripheral_state.role == THROUGHPUT_ROLE_PERIPHERAL) {
    CLI_RESPONSE(THROUGHPUT_UI_ROLE_PERIPHERAL_TEXT);
  } else {
    CLI_RESPONSE(THROUGHPUT_UI_ROLE_CENTRAL_TEXT);
  }
  CLI_RESPONSE(APP_LOG_NEW_LINE);

  CLI_RESPONSE(CLI_OK);
}

/***************************************************************************//**
 * CLI command for setting transmission mode
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_peripheral_mode_set(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  sl_status_t sc;
  uint8_t mode;
  uint32_t amount;
  if (peripheral_state.state != THROUGHPUT_STATE_TEST) {
    mode = sl_cli_get_argument_uint8(arguments, 0);
    amount = sl_cli_get_argument_uint16(arguments, 1);
    sc = throughput_peripheral_set_mode((throughput_mode_t)mode, amount);
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
 * CLI command for reading transmission mode
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_peripheral_mode_get(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  CLI_RESPONSE("cli_throughput_peripheral_mode_get\n");
  CLI_RESPONSE("%d %lu %lu\n",
               (int)peripheral_state.mode,
               (uint32_t)fixed_data_size,
               (uint32_t)fixed_time);
}

/***************************************************************************//**
 * CLI command for setting TX power
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_tx_power_set(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  int16_t power;
  uint8_t control, sleep;
  sl_status_t sc;
  if (peripheral_state.state != THROUGHPUT_STATE_TEST) {
    power = sl_cli_get_argument_int16(arguments, 0);
    control = sl_cli_get_argument_uint8(arguments, 1);
    sleep = sl_cli_get_argument_uint8(arguments, 2);
    sc = throughput_peripheral_set_tx_power(power,
                                            control,
                                            sleep);
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
 * CLI command for reading TX power settings
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_tx_power_get(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  CLI_RESPONSE("cli_throughput_tx_power_get\n");
  CLI_RESPONSE("%d %d %d\n",
               (int)peripheral_state.tx_power,
               (int)power_control_enabled,
               (int)deep_sleep_enabled);
}

/***************************************************************************//**
 * CLI command for setting data sizes
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_peripheral_data_set(sl_cli_command_arg_t *arguments)
{
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  uint8_t mtu, ind_data, not_data;
  sl_status_t sc;
  if (peripheral_state.state != THROUGHPUT_STATE_TEST) {
    mtu = sl_cli_get_argument_uint8(arguments, 0);
    ind_data = sl_cli_get_argument_uint8(arguments, 1);
    not_data = sl_cli_get_argument_uint8(arguments, 2);
    sc = throughput_peripheral_set_data_size(mtu,
                                             ind_data,
                                             not_data);
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
 * CLI command for reading data sizes
 * @param[in] arguments command line argument list
 ******************************************************************************/
void cli_throughput_peripheral_data_get(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  if (!enabled) {
    CLI_RESPONSE(CLI_ERROR);
    return;
  }
  CLI_RESPONSE("cli_throughput_peripheral_data_get\n");
  CLI_RESPONSE("%d %d %d\n",
               (int)peripheral_state.mtu_size,
               (int)indication_data_size,
               (int)notification_data_size);
}
#endif // SL_CATALOG_CLI_PRESENT
