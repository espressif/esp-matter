/***************************************************************************//**
 * @file
 * @brief Core application logic.
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
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app_assert.h"
#include "app_log.h"
#include "app.h"
#include "app_iop.h"
#include "app_memlcd.h"
#include "sl_apploader_util.h"

//--------------------------------
// Connection parameters
#define ADVERTISING_INTERVAL      32 // milliseconds * 1.6
#define RESPONDER_LATENCY_DEFAULT 0 // 0 interval
#define SUPV_TIMEOUT_DEFAULT      200 // 2 seconds
#define CE_LENGTH_DEFAULT         0xFFFF // Unlimited connection event length
#define BONDING_DISABLED          0x00
#define BONDING_ENABLED           0x01
#define BONDING_WITHOUT_MITM      0x00
#define BONDING_WITH_MITM         0x01

static uint8_t connection_id = 0xff;

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

/***************************************************************************//**
 * Application Init.
 ******************************************************************************/
SL_WEAK void app_init(void)
{
  app_test_data_init();
  app_memlcd_init();
}

/***************************************************************************//**
 * Application Process Action.
 ******************************************************************************/
SL_WEAK void app_process_action(void)
{
}

/***************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 ******************************************************************************/
void sl_bt_on_event(sl_bt_msg_t* evt)
{
  sl_status_t sc;
  switch (SL_BT_MSG_ID(evt->header)) {
    // This boot event is generated when the system boots up after reset.
    // Do not call any stack commands before receiving the boot event.
    case sl_bt_evt_system_boot_id: {
      // Read BLE stack version. The first 8 bytes of sl_bt_evt_system_boot_s
      // structure corresponds to major, minor, patch and build versions.
      sc = sl_bt_gatt_server_write_attribute_value(gattdb_iop_test_stack_version,
                                                   0,
                                                   8,
                                                   (uint8_t *)&evt->data.evt_system_boot);
      app_log_status_error(sc);
      app_log_info("Stack version: %d.%d.%d-b%d." APP_LOG_NL,
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);

      #ifdef SL_BOARD_NAME
      // Write hardware information.
      char board_name[] = SL_BOARD_NAME;
      sc =  sl_bt_gatt_server_write_attribute_value(gattdb_model_number_string,
                                                    0,
                                                    strlen(board_name),
                                                    (uint8_t *)board_name);
      app_log_status_error(sc);
      #endif

      // Read address.
      sc = sl_bt_system_get_identity_address(&public_address, 0);
      app_assert_status(sc);

      app_log_info("Public device address: ");
      for (int i = 0; i < 5; i++) {
        app_log_append("%2.2x:", public_address.addr[5 - i]);
      }
      app_log_append("%2.2x." APP_LOG_NL, public_address.addr[0]);

      set_display(IDLE);

      // Delete bondings.
      sc = sl_bt_sm_delete_bondings();
      app_log_status_error(sc);

      if (sc == SL_STATUS_OK) {
        app_log_info("All bondings deleted." APP_LOG_NL);
      }

      // Set Security Manager to debug mode, so the encrypted BLE packets can be
      // read in Network Analyzer.
      sc = sl_bt_sm_set_debug_mode();
      app_log_status_error(sc);

      // Advertising
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_log_status_error(sc);

      // Default advertisement parameters: 32 (20 ms) interval. Other optional
      // parameters for phones: 244 (152.5 ms), 338 (211.25 ms) 160 (100 ms).
      sc = sl_bt_advertiser_set_timing(advertising_set_handle,
                                       ADVERTISING_INTERVAL,
                                       ADVERTISING_INTERVAL,
                                       0,   // adv. duration
                                       0);  // max. num. adv. events
      app_log_status_error(sc);

      // Start advertising and enable connections.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);

      if (sc == SL_STATUS_OK) {
        app_log_info("Advertising started." APP_LOG_NL);
      }

      // Test 1 (Scan device) takes place now.
      // Test 2 (Connect to device) takes place now.
      break;
    }

    //--------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id: {
      connection_id = evt->data.evt_connection_opened.connection;
      app_log_info("Connection opened. connection_id=%d" APP_LOG_NL,
                   connection_id);

      set_display(CONNECTED);

      // Test 3 (MA discovers the GATT) takes place now.

      // Increase the security of the connection for tests 7.2, 7.3, 7.4.
      if (connection_security != sl_bt_connection_mode1_level1) {
        sl_bt_sm_increase_security(connection_id);
      }
      if (evt->data.evt_connection_opened.bonding
          != SL_BT_INVALID_BONDING_HANDLE) {
        app_log_info("Mobile is already bonded." APP_LOG_NL);
      }

      // Request connection parameter update.
      sc = sl_bt_connection_set_parameters(evt->data.evt_connection_opened.connection,
                                           connection_interval,
                                           connection_interval,
                                           RESPONDER_LATENCY_DEFAULT,
                                           SUPV_TIMEOUT_DEFAULT,
                                           CE_LENGTH_DEFAULT,
                                           CE_LENGTH_DEFAULT);
      app_log_status_error(sc);
      app_log_info("Request to change connection parameters to defaults."
                   APP_LOG_NL);
      break;
    }

    //--------------------------------
    // This event indicates there is a request to display the passkey to the user.
    case sl_bt_evt_sm_passkey_display_id: {
      passkey = evt->data.evt_sm_passkey_display.passkey; // Store passkey.
      app_log_info("Passkey: %4lu." APP_LOG_NL, passkey); // Make it appear on Console.
      // Note: It also appears in the EFR Connect app.
      set_display(DISPLAY_PASSKEY); // Make it appear on LCD.
      break;
    }

    // Indicates a user request to display that the new bonding request is
    // received and for the user to confirm the request.
    case sl_bt_evt_sm_confirm_bonding_id: {
      app_log_info("Bonding confirm." APP_LOG_NL);
      // Accept bonding request.
      sc = sl_bt_sm_bonding_confirm(evt->data.evt_sm_confirm_bonding.connection,
                                    1);
      app_log_status_error(sc);
      break;
    }

    // Triggered after the pairing or bonding procedure is successfully completed.
    case sl_bt_evt_sm_bonded_id: {
      app_log_info("Bonding success." APP_LOG_NL);
      set_display(BOND_SUCCESS);
      break;
    }

    // This event is triggered if the pairing or bonding procedure fails.
    case sl_bt_evt_sm_bonding_failed_id: {
      app_log_error("Bonding failed, reason 0x%2X." APP_LOG_NL,
                    evt->data.evt_sm_bonding_failed.reason);
      // Closing and reopening connection to retry.
      sc = sl_bt_connection_close(evt->data.evt_sm_bonding_failed.connection);
      app_log_status_error(sc);

      set_display(BOND_FAILURE);
      break;
    }

    //--------------------------------
    // Triggered whenever the connection parameters are changed
    case sl_bt_evt_connection_parameters_id: {
      pdu_size = evt->data.evt_connection_parameters.txsize;
      connection_interval = evt->data.evt_connection_parameters.interval;
      responder_latency = evt->data.evt_connection_parameters.latency;
      supv_timeout = (uint16_t)((evt->data.evt_connection_parameters.timeout) * 10);
      app_log_info("Connection parameters are changed: "
                   "pdu_size=%d, connection_interval=%d, "
                   "responder_latency=%d, supv_timeout=%d" APP_LOG_NL,
                   (int)pdu_size,
                   (int)connection_interval,
                   (int)responder_latency,
                   (int)supv_timeout);

      switch (evt->data.evt_connection_parameters.security_mode) {
        case sl_bt_connection_mode1_level1: {
          app_log_info("Connection security: No Security." APP_LOG_NL);
          break;
        }

        case sl_bt_connection_mode1_level2: {
          app_log_info("Connection security: Unauthenticated pairing." APP_LOG_NL);
          break;
        }

        case sl_bt_connection_mode1_level3: {
          app_log_info("Connection security: Authenticated pairing." APP_LOG_NL);
          break;
        }

        case sl_bt_connection_mode1_level4: {
          app_log_info("Connection security: Bonded." APP_LOG_NL);
          break;
        }

        default: {
          break;
        }
      }
      break;
    }

    // Indicates that an ATT_MTU exchange procedure is completed.
    case sl_bt_evt_gatt_mtu_exchanged_id: {
      mtu_size = evt->data.evt_gatt_mtu_exchanged.mtu;
      app_log_info("MTU value: %d." APP_LOG_NL, mtu_size);
      break;
    }

    //--------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id: {
      set_display(IDLE);

      if (ota_dfu_request) {
        sl_apploader_util_reset_to_ota_dfu();
        // (No need to reset ota_dfu_request since the device restarts.)
        // Test 6.1 (IOP Test OTA update with ACK) takes place now.
        // Test 6.2 (IOP Test OTA update without ACK) takes place now.
      } else {
        // Configure security manager for the next connection.
        switch (connection_security) {
          // Unauthenticated pairing with encryption
          case sl_bt_connection_mode1_level2: {
            // Preparing for test 7.2 (Security/Pairing).
            sc = sl_bt_sm_delete_bondings();
            app_log_status_error(sc);

            if (sc == SL_STATUS_OK) {
              app_log_info("All bondings deleted. "
                           "Connection security: Unauthenticated pairing." APP_LOG_NL);
            }

            sc = sl_bt_sm_configure(BONDING_WITHOUT_MITM,
                                    sl_bt_sm_io_capability_noinputnooutput);
            app_log_status_error_f(sc, "SM configure failure." APP_LOG_NL);

            sc = sl_bt_sm_set_bondable_mode(BONDING_DISABLED);
            app_log_status_error_f(sc, "SM set bondable mode failure." APP_LOG_NL);
            break;
          }

          // Authenticated pairing with encryption
          case sl_bt_connection_mode1_level3: {
            // Preparing for test 7.3 (Security/Authentication).
            sc = sl_bt_sm_delete_bondings();
            app_log_status_error(sc);

            if (sc == SL_STATUS_OK) {
              app_log_info("All bondings deleted. "
                           "Connection security: Authenticated pairing." APP_LOG_NL);
            }

            sc = sl_bt_sm_configure(BONDING_WITH_MITM,
                                    sl_bt_sm_io_capability_displayonly);
            app_log_status_error_f(sc, "SM configure failure." APP_LOG_NL);

            sc = sl_bt_sm_set_passkey(passkey);
            app_log_status_error_f(sc, "SM set passkey failure." APP_LOG_NL);

            sc = sl_bt_sm_set_bondable_mode(BONDING_DISABLED);
            app_log_status_error_f(sc, "SM set bondable mode failure." APP_LOG_NL);
            break;
          }

          // Authenticated Secure Connections pairing with encryption using a
          // 128-bit strength encryption key
          case sl_bt_connection_mode1_level4: {
            // Preparing for test 7.4 (Security/Bonding).
            sc = sl_bt_sm_delete_bondings();
            app_log_status_error(sc);

            if (sc == SL_STATUS_OK) {
              app_log_info("All bondings deleted. "
                           "Connection security: Bonded." APP_LOG_NL);
            }

            sc = sl_bt_sm_configure(BONDING_WITH_MITM,
                                    sl_bt_sm_io_capability_displayonly);
            app_log_status_error_f(sc, "SM configure failure." APP_LOG_NL);

            sc = sl_bt_sm_set_passkey(passkey);
            app_log_status_error_f(sc, "SM set passkey failure." APP_LOG_NL);

            sc = sl_bt_sm_set_bondable_mode(BONDING_ENABLED);
            app_log_status_error_f(sc, "SM set bondable mode failure." APP_LOG_NL);
            break;
          }

          default: {
            break;
          }
        }

        // Generate data for advertising
        sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                   sl_bt_advertiser_general_discoverable);
        app_log_status_error(sc);

        // Restart advertising.
        sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                           sl_bt_advertiser_connectable_scannable);
        app_log_status_error(sc);

        if (sc == SL_STATUS_OK) {
          app_log_info("Advertising started." APP_LOG_NL);
        }
      }
      break;
    }

    //--------------------------------
    // This event indicates that a remote GATT is attempting to write a
    // value of an attribute into the local GATT database.
    // Test 4 and 5 (MA performs all GATT) partly takes place here.
    case sl_bt_evt_gatt_server_user_write_request_id: {
      // Test 4.3 (iop_test_writeonly_len_1) takes place now.
      // Test 4.4 (iop_test_writeonly_len_255) takes place now.
      // Test 4.5 (iop_test_writewoutresp_len_1) takes place now.
      // Test 4.6 (iop_test_writewoutresp_len_255) takes place now.
      // Test 5.1.1 (iop_test_len_1) takes place now.
      // Test 5.2.1 (iop_test_len_255) takes place now.
      // Test 5.3.1.1 (iop_test_len_variable_4) takes place now.
      // Test 5.3.2.1 (iop_test_len_variable_4) takes place now.
      // Test 5.4.2 (iop_test_const_len_1) takes place now.
      // Test 5.5.2 (iop_test_const_len_255) takes place now.

      sc = handle_user_write(&(evt->data.evt_gatt_server_user_write_request));
      app_log_status_error(sc);
      break;
    }

    //--------------------------------
    // This event indicates that a remote GATT is attempting to read a
    // value of an attribute from the local GATT database.
    // Test 4 and 5 (MA performs all GATT) partly takes place here.
    case sl_bt_evt_gatt_server_user_read_request_id: {
      // Test 4.1 (iop_test_readonly_len_1) takes place now.
      // Test 4.2 (iop_test_readonly_len_255) takes place now.
      // Test 5.1.2 (iop_test_len_1) takes place now.
      // Test 5.2.2 (iop_test_len_255) takes place now.
      // Test 5.3.1.2 (iop_test_len_variable_4) takes place now.
      // Test 5.3.2.2 (iop_test_len_variable_4) takes place now.
      // Test 5.4.1 (iop_test_const_len_1) takes place now.
      // Test 5.5.1 (iop_test_const_len_255) takes place now.
      // Test 7.2 (Security/Pairing) takes place now.
      // Test 7.3 (Security/Authentication) takes place now.
      // Test 7.4 (Security/Bonding) takes place now.

      sc = handle_user_read(&(evt->data.evt_gatt_server_user_read_request));
      app_log_status_error(sc);
      break;
    }

    // Execute write completed event indicates that the execute write command
    // from a remote GATT has completed with the given result.
    case sl_bt_evt_gatt_server_execute_write_completed_id: {
      // End of Test 5.7.1.
      app_log_info("GATT server execute write completed." APP_LOG_NL);
      break;
    }

    //--------------------------------
    // This event occurs when the remote GATT enabled or disabled a
    // notification or indication.
    case sl_bt_evt_gatt_server_characteristic_status_id: {
      if (evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config) {
        sc = handle_timer_start(&(evt->data.evt_gatt_server_characteristic_status));
        app_log_status_error(sc);
      }
      break;
    }

    default: {
      break;
    }
  }
}
