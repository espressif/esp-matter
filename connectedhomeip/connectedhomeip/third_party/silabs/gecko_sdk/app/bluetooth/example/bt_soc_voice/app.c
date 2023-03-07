/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "app_log.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app.h"
#include "voice.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_SIMPLE_COM_PRESENT
#include "sl_simple_com.h"
#endif // SL_CATALOG_SIMPLE_COM_PRESENT
#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#endif // SL_CATALOG_CLI_PRESENT

#define INVALID_HANDLE       0xff

#define TRANSFER_STOP        ('0')
#define TRANSFER_START       ('1')

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = INVALID_HANDLE;

// Connection handle, available for other modules
uint8_t ble_connection = INVALID_HANDLE;

// Button events
static bool button_pressed = false;
static bool button_released = false;

/***************************************************************************//**
 * Notify client about the actual transfer status.
 ******************************************************************************/
static void app_notify_client(uint8_t transfer_status);

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void app_init(void)
{
  voice_init();
  app_log_info("soc_voice initialised\n");
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void)
{
  voice_process_action();
  if (button_pressed) {
    button_pressed = false;
    // Turn LED on
    sl_led_turn_on(&sl_led_led0);
    // Start recording
    voice_start();
    // Notify client about transfer status change
    app_notify_client(TRANSFER_START);
  }
  if (button_released) {
    button_released = false;
    // Turn LED off
    sl_led_turn_off(&sl_led_led0);
    // Stop recording
    voice_stop();
    // Notify client about transfer status change
    app_notify_client(TRANSFER_STOP);
  }
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
  uint16_t max_mtu_out;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Print boot message.
      app_log_info("Bluetooth stack booted: v%d.%d.%d-b%d\n",
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);

      // Set maximal MTU for GATT Server.
      sc = sl_bt_gatt_server_set_max_mtu(250, &max_mtu_out);
      app_assert_status(sc);

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
      // Start general advertising and enable connections.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);

      app_log_info("Start advertising\n");
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      app_log_info("Connection opened\n");
      ble_connection = evt->data.evt_connection_opened.connection;
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      app_log_info("Connection closed\n");
      ble_connection = INVALID_HANDLE;

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Restart advertising after client has disconnected.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      app_log_info("Start advertising\n");
      break;

    // -------------------------------
    // Handle Voice configuration characteristics.
    case sl_bt_evt_gatt_server_user_write_request_id:
      switch (evt->data.evt_gatt_server_user_write_request.characteristic) {
        case gattdb_sample_rate:
          voice_set_sample_rate((sample_rate_t)evt->data.evt_gatt_server_user_write_request.value.data[0]);
          break;

        case gattdb_filter_enable:
          voice_set_filter_enable((bool)evt->data.evt_gatt_server_user_write_request.value.data[0]);
          break;

        case gattdb_encoding_enable:
          voice_set_encoding_enable((bool)evt->data.evt_gatt_server_user_write_request.value.data[0]);
          break;

        case gattdb_audio_channels:
          voice_set_channels(evt->data.evt_gatt_server_user_write_request.value.data[0]);
          break;

        default:
          // Other characteristics are not handled here.
          return;
      }
      // Send write response.
      sc = sl_bt_gatt_server_send_user_write_response(
        evt->data.evt_gatt_server_user_write_request.connection,
        evt->data.evt_gatt_server_user_write_request.characteristic,
        0);    // no error
      app_assert_status(sc);
      break;

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

static void app_notify_client(uint8_t transfer_status)
{
  if (INVALID_HANDLE != ble_connection) {
    (void)sl_bt_gatt_server_send_notification(
      ble_connection,
      gattdb_transfer_status,
      sizeof(transfer_status),
      &transfer_status);
  }
}

void voice_transmit(uint8_t *buffer, uint32_t size)
{
  if (INVALID_HANDLE != ble_connection) {
    // Write data to characteristic
    (void)sl_bt_gatt_server_send_notification(
      ble_connection,
      gattdb_audio_data,
      size,
      buffer);
  }
#ifdef SL_CATALOG_SIMPLE_COM_PRESENT
  sl_simple_com_transmit(size, buffer);
#endif // SL_CATALOG_SIMPLE_COM_PRESENT
}

/***************************************************************************//**
 * Button event handler callback.
 * Overrides dummy weak implementation.
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  // button pressed
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&sl_button_btn0 == handle) {
      button_pressed = true;
    }
  }
  // button released
  else if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED) {
    if (&sl_button_btn0 == handle) {
      button_released = true;
    }
  }
}

#ifdef SL_CATALOG_CLI_PRESENT
/***************************************************************************//**
 * Command line interface handlers.
 ******************************************************************************/
void cli_sample_rate(sl_cli_command_arg_t *arguments)
{
  sample_rate_t sample_rate = (sample_rate_t)sl_cli_get_argument_uint8(arguments, 0);
  voice_set_sample_rate(sample_rate);
}

void cli_channels(sl_cli_command_arg_t *arguments)
{
  uint8_t channels = sl_cli_get_argument_uint8(arguments, 0);
  voice_set_channels(channels);
}

void cli_filter(sl_cli_command_arg_t *arguments)
{
  bool status = (bool)sl_cli_get_argument_uint8(arguments, 0);
  voice_set_filter_enable(status);
}

void cli_encoding(sl_cli_command_arg_t *arguments)
{
  bool status = (bool)sl_cli_get_argument_uint8(arguments, 0);
  voice_set_encoding_enable(status);
}
#endif // SL_CATALOG_CLI_PRESENT
