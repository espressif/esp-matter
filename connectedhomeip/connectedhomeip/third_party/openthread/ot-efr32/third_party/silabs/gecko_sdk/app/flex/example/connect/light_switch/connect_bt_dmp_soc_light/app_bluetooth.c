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
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sl_simple_led_instances.h"
#include "app_assert.h"
#include "app_log.h"
#include "sl_light_switch.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define MAX_INDICATION_DATA_LENGTH_BYTE         (8)
// gattdb_light_state_connect
// gattdb_trigger_source_connect
// gattdb_source_address_connect
#define SL_USED_CHARACTERISTIC_INDICATION_COUNT  (16)

typedef struct {
  // ble characteristic
  uint8_t characteristic;
  //notification sending can be enabled
  bool enabled;
} sl_indication_setting_t;

typedef struct {
  // ble characteristic
  uint8_t characteristic;
  //indication data
  uint8_t data[MAX_INDICATION_DATA_LENGTH_BYTE];
  size_t data_size;
} sl_indication_t;

/// queue for the outgoing indications
typedef struct {
  // indication array for the ble actions
  sl_indication_t indication[SL_USED_CHARACTERISTIC_INDICATION_COUNT];
  // number of indications
  uint8_t count_of_indications;
} sl_indicaton_queue_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Remove the oldest indication request, but check if this was really answered
 *
 * @param characteristic: the answered indication
 * @return None
 ******************************************************************************/
static void sl_remove_last_indication (uint8_t characteristic);

/*******************************************************************************
 * Checks that this indication is enabled for the gatt characteristic
 *
 * @param characteristic: which characteristic needed indication
 * @return bool: true if indications are allowed
 ******************************************************************************/
static bool sl_is_indication_enabled (uint8_t characteristic);

/*******************************************************************************
 * Update the setting of the indication of the selected characteristic
 *
 * @param characteristic: which characteristic needed indication
 * @param enabled: is it on or off
 * @return None
 ******************************************************************************/
static void sl_update_indication_enabled (uint8_t characteristic, bool enabled);

/*******************************************************************************
 * Add a characteristic indication to the queue
 *
 * @param characteristic
 * @return None
 ******************************************************************************/
static void sl_add_bluetooth_indication (uint8_t characteristic, void * pData, uint8_t data_length_byte);

extern void toggle_light_state();
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// The advertising set handle allocated from Bluetooth stack
static uint8_t advertising_set_handle = 0xFF;
/// Address of the mobile device
static uint8_t device_address_display[8] = { 0 };
/// Flag to schedule the indication sending
static bool indication_is_under_way = false;
/// Indication settings
static sl_indication_setting_t sl_indication_settings[SL_USED_CHARACTERISTIC_INDICATION_COUNT] = { 0 };
/// Indication queue
static sl_indicaton_queue_t indicatons_queue = { 0 };
/// handle of the connected device
static uint8_t connected_device_handle;
/// Information used by the mobile device
static sl_direction_t direction = SL_DIRECTION_PROPRIETARY;
/// address of the connected mobile device
static uint8_t ble_device_address[8] = { 0 };
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * Notify the connected mobile device about any change within the characteristics
 *****************************************************************************/
void notify_connected_ble_device(sl_direction_t in_direction, uint8_t* address)
{
  direction = in_direction;
  demo_light_t light_state = sl_get_light_state();
  sl_add_bluetooth_indication(gattdb_light_state_connect, &light_state, sizeof(uint8_t));
  sl_add_bluetooth_indication(gattdb_trigger_source_connect, &direction, sizeof(uint8_t));
  sl_add_bluetooth_indication(gattdb_source_address_connect, address, 8);
}

/*********************************** ***************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *****************************************************************************/
void
sl_bt_on_event(sl_bt_msg_t *evt)
{
  /// Flag for indicating DFU Reset must be performed.
  bool boot_to_dfu = false;
  /// Bluetooth status indicator
  sl_status_t bt_status = 0;
  /// Length of the received message
  uint16_t sent_len = 0;

  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    ///////////////////////////////////////////////////////////////////////////
    // This event indicates the device has started and the radio is ready.   //
    // Do not call any stack command before receiving this boot event!       //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_system_boot_id:

      // Create Advertising Set
      bt_status = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status_f(bt_status,
                          "[E: 0x%04x] Failed to create advertising set\n",
                          (int )bt_status);
      // Generate the advertising data from the GATT configurator
      bt_status = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                        sl_bt_advertiser_general_discoverable);

      app_assert_status_f(bt_status,
                          "[E: 0x%04x] Failed to create advertising data\n",
                          (int )bt_status);

      // Set advertising parameters. 100ms advertisement interval.
      // The first parameter is advertising set handle
      // The next two parameters are minimum and maximum advertising
      // interval, both in units of (milliseconds * 1.6).
      // The last two parameters are duration and maxevents left as default..
      bt_status = sl_bt_advertiser_set_timing(0, 160, 160, 0, 0);

      // Start general advertising and enable connections.
      bt_status = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                                sl_bt_legacy_advertiser_connectable);
      break;

    ///////////////////////////////////////////////////////////////////////////
    // This event indicates that a connection was closed.                    //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_connection_closed_id:
      // Check if need to boot to OTA DFU mode
      if (boot_to_dfu) {
        // Enter to OTA DFU mode
        sl_bt_system_reset(2);
      } else {
        // Restart advertising after client has disconnected
        bt_status = sl_bt_legacy_advertiser_start(0,
                                                  sl_bt_legacy_advertiser_connectable);
      }
      break;

    ///////////////////////////////////////////////////////////////////////////
    // This event indicates that a connection is opened.                     //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_connection_opened_id:

      app_log_info("Mobile device connected\n");
      // Save the address of the android device
      memcpy(device_address_display,
             evt->data.evt_connection_opened.address.addr,
             sizeof(evt->data.evt_connection_opened.address.addr));

      memcpy(ble_device_address, evt->data.evt_connection_opened.address.addr, sizeof(evt->data.evt_connection_opened.address.addr));
      connected_device_handle = evt->data.evt_connection_opened.connection;
      // Send notification data about the light-state
      notify_connected_ble_device(SL_DIRECTION_BLUETOOTH, ble_device_address);
      break;

    ///////////////////////////////////////////////////////////////////////////
    // This event indicates that a remote GATT client is attempting to write //
    // a value of a user type attribute in to the local GATT database.       //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_gatt_server_user_write_request_id:

      if (gattdb_light_state_connect
          == evt->data.evt_gatt_server_user_write_request.characteristic) {
        toggle_light_state();
        //Send response to write request
        bt_status = sl_bt_gatt_server_send_user_write_response(
          evt->data.evt_gatt_server_user_write_request.connection,
          gattdb_light_state_connect,
          0);
        app_assert(
          (bt_status == SL_STATUS_OK),
          "[E: 0x%04x] Failed to send_user_write_response gattdb_light_state_connect\n",
          (int )bt_status);

        /* Send notification/indication data */
        notify_connected_ble_device(SL_DIRECTION_BLUETOOTH, ble_device_address);
        app_log_info("Toggle message from mobile device, light is %s\n",
                     (sl_get_light_state() == DEMO_LIGHT_ON) ? "on" : "off");
      }
      break;

    ///////////////////////////////////////////////////////////////////////////
    // This event indicates that a remote GATT client is attempting to read  //
    // one of the available characteristic                                   //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_gatt_server_user_read_request_id:

      // read request of the light state
      if (gattdb_light_state_connect
          == evt->data.evt_gatt_server_user_read_request.characteristic) {
        demo_light_t light_state = sl_get_light_state();
        // Send response to read request
        bt_status = sl_bt_gatt_server_send_user_read_response(
          evt->data.evt_gatt_server_user_read_request.connection,
          gattdb_light_state_connect,
          0, sizeof(light_state), &light_state, &sent_len);
        app_assert_status_f(
          bt_status,
          "[E: 0x%04x] Failed to send user read response LIGHT_STATE_GATTDB\n",
          (int )bt_status);

        // read request of the trigger source
      } else if (gattdb_trigger_source_connect
                 == evt->data.evt_gatt_server_user_read_request.characteristic) {
        bt_status = sl_bt_gatt_server_send_user_read_response(
          evt->data.evt_gatt_server_user_read_request.connection,
          gattdb_trigger_source_connect,
          0, sizeof(direction), (uint8_t*) &direction, &sent_len);

        app_assert_status_f(
          bt_status,
          "[E: 0x%04x] Failed to send user read response TRIGGER_SOURCE_GATTDB\n",
          (int )bt_status);

        // read request of the source address
      } else if (gattdb_source_address_connect
                 == evt->data.evt_gatt_server_user_read_request.characteristic) {
        bt_status = sl_bt_gatt_server_send_user_read_response(
          evt->data.evt_gatt_server_user_read_request.connection,
          gattdb_source_address_connect,
          0, sizeof(ble_device_address),
          (uint8_t*) ble_device_address, &sent_len);

        app_assert_status_f(
          bt_status,
          "[E: 0x%04x] Failed to send user read response SOURCE_ADDRESS_GATTDB\n",
          (int )bt_status);
      }
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      if (gatt_server_confirmation
          == evt->data.evt_gatt_server_characteristic_status.status_flags) {
        sl_remove_last_indication(
          evt->data.evt_gatt_server_characteristic_status.characteristic);
        indication_is_under_way = false;
      } else if (gatt_server_client_config
                 == evt->data.evt_gatt_server_characteristic_status.status_flags) {
        sl_update_indication_enabled(
          evt->data.evt_gatt_server_characteristic_status.characteristic,
          true);
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
 * Add a characteristic indication to the queue
 ******************************************************************************/
static void sl_add_bluetooth_indication(uint8_t characteristic, void * pData, uint8_t data_length_byte)
{
  if (advertising_set_handle != 0xFF) {
    if (sl_is_indication_enabled(characteristic)) {
      if (indicatons_queue.count_of_indications < SL_USED_CHARACTERISTIC_INDICATION_COUNT) {
        for (int i = 0; i < indicatons_queue.count_of_indications; i++) {
          if (indicatons_queue.indication[i].characteristic == characteristic) {
            return;
          }
        }
        indicatons_queue.count_of_indications++;
        indicatons_queue.indication[(indicatons_queue.count_of_indications - 1)].characteristic = characteristic;
        indicatons_queue.indication[(indicatons_queue.count_of_indications - 1)].data_size = 0;
        if ((pData != NULL) && (data_length_byte <= MAX_INDICATION_DATA_LENGTH_BYTE)) {
          memcpy(indicatons_queue.indication[(indicatons_queue.count_of_indications - 1)].data,
                 pData,
                 data_length_byte);
          indicatons_queue.indication[(indicatons_queue.count_of_indications - 1)].data_size = data_length_byte;
        }
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
void sl_send_bluetooth_indications(void)
{
  sl_status_t bt_status   = SL_STATUS_OK;
  sl_indication_t* indication = NULL;
  if (advertising_set_handle != 0xFF) {
    if (!indication_is_under_way) {
      if (indicatons_queue.count_of_indications > 0) {
        indication = &indicatons_queue.indication[0];
        if (indication != NULL) {
          switch (indication->characteristic) {
            case gattdb_source_address_connect: /*Intentional fall through*/
            case gattdb_trigger_source_connect:
            case gattdb_light_state_connect:
            {
              bt_status =
                sl_bt_gatt_server_send_indication(
                  connected_device_handle,
                  indication->characteristic,
                  indication->data_size,
                  indication->data);
              if (bt_status != SL_STATUS_OK) {
                app_log_error("sl_bt_gatt_server_send_indication failed with 0x%04X\n",
                              bt_status);
              }
              indication_is_under_way = true;
            }
            break;
            default:
              break;
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//                         Static Function Definitions
// -----------------------------------------------------------------------------

/*******************************************************************************
 * Remove the oldest indication request, but check if this was really answered
 ******************************************************************************/
static void sl_remove_last_indication(uint8_t characteristic)
{
  if (indicatons_queue.count_of_indications > 0) {
    if (characteristic == indicatons_queue.indication[0].characteristic) {
      indicatons_queue.count_of_indications--;
      for (uint8_t i = 0; i < SL_USED_CHARACTERISTIC_INDICATION_COUNT - 1; i++) {
        memcpy(&indicatons_queue.indication[i], &indicatons_queue.indication[i + 1], sizeof(sl_indication_t));

        if (i >= indicatons_queue.count_of_indications) {
          memset(&indicatons_queue.indication[i], 0, sizeof(sl_indication_t));
          break;
        }
      }
    } else {
      app_log_info("Not correct indication\n");
    }
  }
}

/*******************************************************************************
 * Checks that this indication is enabled for the gatt characteristic
 ******************************************************************************/
static bool sl_is_indication_enabled(uint8_t characteristic)
{
  bool enabled = false;
  for (uint8_t indication = 0; indication < SL_USED_CHARACTERISTIC_INDICATION_COUNT; indication++) {
    if (characteristic == sl_indication_settings[indication].characteristic) {
      enabled = sl_indication_settings[indication].enabled;
      break;
    }
  }
  return enabled;
}

/*******************************************************************************
 * Update the setting of the indication of the selected characteristic
 ******************************************************************************/
static void sl_update_indication_enabled(uint8_t characteristic, bool enabled)
{
  uint8_t first_blank    = 0;
  bool first_blank_found = false;
  bool updated           = false;

  for (uint8_t indication = 0; indication < SL_USED_CHARACTERISTIC_INDICATION_COUNT; indication++) {
    if ((0 == sl_indication_settings[indication].characteristic) && !first_blank_found) {
      first_blank = indication;
      first_blank_found = true;
    }
    if (characteristic == sl_indication_settings[indication].characteristic) {
      sl_indication_settings[indication].enabled = enabled;
      updated = true;
      break;
    }
  }

  if (!updated) {
    sl_indication_settings[first_blank].characteristic = characteristic;
    sl_indication_settings[first_blank].enabled = enabled;
  }
}
