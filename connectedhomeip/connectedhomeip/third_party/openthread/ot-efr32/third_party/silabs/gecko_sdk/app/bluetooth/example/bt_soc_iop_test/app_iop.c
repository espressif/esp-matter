/***************************************************************************//**
 * @file
 * @brief Helper functions for BLE interoperability test.
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
#include <stdio.h>
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sl_simple_timer.h"
#include "app_log.h"
#include "app_iop.h"
#include "app_memlcd.h"

//--------------------------------
// Mobile OS types
#define ANDROID 0x01
#define IOS     0x02
static uint8_t mobile_os = ANDROID;
// Number of packages per connection interval
#define PACK_NUM_ANDROID  6
#define PACK_NUM_IOS      4

// Size of the arrays for sending and receiving data
#define DATA_SIZE_MAX     255
// Size of the arrays for indication or notification data
#define DATA_SIZE_NOTIFY  250

//--------------------------------
// Data payload to be sent over indications or notifications.
// Characteristic value for gattdb_iop_test_notify_len_1 and
// gattdb_iop_test_indicate_len_1 characteristics. Test cases: 4.7, 4.9.
static uint8_t iop_test_notification_1;
// Characteristic value for gattdb_iop_test_notify_len_mtu_3,
// gattdb_iop_test_indicate_len_mtu_3 and gattdb_iop_test_throughput
// characteristics. Test cases: 4.8, 4.10, 7.1.
static uint8_t iop_test_notification_250_arr[DATA_SIZE_NOTIFY];

//--------------------------------
// Variables for the remote device to write and read back.
// Storage for the value of the gattdb_iop_test_user_len_1 characteristic.
// Test case: 5.6.
static uint8_t iop_test_chr_user_1;
// Storage for the value of the gattdb_iop_test_user_len_255 characteristic.
// Test case: 5.7.
static uint8_t iop_test_chr_user_255_arr[DATA_SIZE_MAX];
// Storage for the value and the length of the gattdb_iop_test_user_len_var_4
// characteristic. Test case: 5.8.
static uint8_t iop_test_chr_user_var_4_arr[4];
static uint8_t iop_test_chr_user_var_4_len;

//--------------------------------
// Connection parameters
uint16_t mtu_size = 23;
uint16_t pdu_size = 0;
uint16_t connection_interval = 12; // 15 ms / 1.25
uint16_t responder_latency;
uint16_t supv_timeout;

// Storage for the connection parameters, defined in iop_test_connection.
static uint8_t iop_connection_arr[10];

//--------------------------------
// Security
sl_bt_connection_security_t connection_security = sl_bt_connection_mode1_level1;
// Encryption key for pairing and bonding.
uint32_t passkey = 123456;

// OTA-DFU
bool ota_dfu_request = false;

//--------------------------------
// Throughput
#define CONN_PARAM_MULTIPLIER (1.25f)
static bool throughput_enabled = true;

//--------------------------------
// Simple timer
#define TIMER_TRIGGER 100 // [ms] timeout
static sl_simple_timer_t timer;
static uint16_t timer_cb_data;

/***************************************************************************//**
 * Timer Callback
 * @param[in] handle pointer to handle instance
 * @param[in] data pointer to input data
 ******************************************************************************/
static void timer_cb(sl_simple_timer_t *handle, void *data);

//--------------------------------
// Display operations
bd_addr public_address;
#define STATE_STRING "State"
#define PASSKEY_STRING "Passkey"
#define ADDRESS_STRING "Address"
#define INDENT_STRING "  "

// Initialize test data values.
void app_test_data_init(void)
{
  iop_test_notification_1 = 0x55;

  for (int i = 0; i < DATA_SIZE_NOTIFY; i++) {
    iop_test_notification_250_arr[i] = i;
  }

  iop_test_chr_user_1 = 0x00;

  for (int i = 0; i < DATA_SIZE_MAX; i++) {
    iop_test_chr_user_255_arr[i] = 0x00;
  }

  for (int i = 0; i < 4; i++) {
    iop_test_chr_user_var_4_arr[i] = 0x00;
  }

  iop_test_chr_user_var_4_len = 1;
}

// Handle read operation for user-type characteristics.
sl_status_t handle_user_read(sl_bt_evt_gatt_server_user_read_request_t *user_read_req)
{
  sl_status_t sc;

  if (user_read_req == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  switch (user_read_req->characteristic) {
    case gattdb_iop_test_connection: {
      // Packing MTU Size.
      iop_connection_arr[0] = mtu_size & 0xff;
      iop_connection_arr[1] = mtu_size >> 8;
      // Packing PDU Size.
      iop_connection_arr[2] = pdu_size & 0xff;
      iop_connection_arr[3] = pdu_size >> 8;
      // Packing Connection Interval.
      iop_connection_arr[4] = connection_interval & 0xff;
      iop_connection_arr[5] = connection_interval >> 8;
      // Packing Responder Latency.
      iop_connection_arr[6] = responder_latency & 0xff;
      iop_connection_arr[7] = responder_latency >> 8;
      // Packing Supervision Timeout.
      iop_connection_arr[8] = supv_timeout & 0xff;
      iop_connection_arr[9] = supv_timeout >> 8;

      app_log_info("Getting MTU size: %d, PDU Size:%d, Connection Interval:%d, "
                   "Responder Latency:%d, Supervision Timeout:%d ms." APP_LOG_NL,
                   (int)mtu_size,
                   (int)pdu_size,
                   (int)connection_interval,
                   (int)responder_latency,
                   (int)supv_timeout);
      sc = sl_bt_gatt_server_send_user_read_response(user_read_req->connection,
                                                     user_read_req->characteristic,
                                                     (uint8_t)SL_STATUS_OK,
                                                     sizeof(iop_connection_arr),
                                                     (uint8_t *)&iop_connection_arr,
                                                     NULL);
      break;
    }

    //--------------------------------
    case gattdb_iop_test_user_len_1: {
      // Test 5.6.2 (iop_test_user_len_1) takes place now.
      app_log_info("Read user-type bytes for gattdb_iop_test_user_len_1." APP_LOG_NL);
      sc = sl_bt_gatt_server_send_user_read_response(user_read_req->connection,
                                                     user_read_req->characteristic,
                                                     (uint8_t)SL_STATUS_OK,
                                                     sizeof(iop_test_chr_user_1),
                                                     (uint8_t *)&iop_test_chr_user_1,
                                                     NULL);
      break;
    }

    case gattdb_iop_test_user_len_255: {
      // Test 5.7.2 (iop_test_user_len_255) takes place now.
      app_log_info("Read user-type bytes for gattdb_iop_test_user_len_255. "
                   "offset:%d." APP_LOG_NL,
                   user_read_req->offset);
      uint16_t bytes_to_send = sizeof(iop_test_chr_user_255_arr)
                               - user_read_req->offset;
      if (bytes_to_send > mtu_size - 1) {
        bytes_to_send = mtu_size - 1; // Clamp to MTU - 1.
      }

      if (bytes_to_send > 0) {
        uint16_t sent_len;
        sc = sl_bt_gatt_server_send_user_read_response(user_read_req->connection,
                                                       user_read_req->characteristic,
                                                       (uint8_t)SL_STATUS_OK,
                                                       bytes_to_send,
                                                       (uint8_t *)&iop_test_chr_user_255_arr[user_read_req->offset],
                                                       &sent_len);
        if (sc == SL_STATUS_OK) {
          app_log_info("Length of data:%d; mtu:%d; result:%x; req len:%d; "
                       "sent len:%d." APP_LOG_NL,
                       sizeof(iop_test_chr_user_255_arr),
                       mtu_size,
                       sc,
                       bytes_to_send,
                       sent_len);
        }
      } else {
        // Invalid value length because of the offset.
        app_log_error("Reading iop_test_user_len_255 with invalid length "
                      "because of offset: 0x%02x." APP_LOG_NL,
                      user_read_req->offset);
        sc = sl_bt_gatt_server_send_user_read_response(user_read_req->connection,
                                                       user_read_req->characteristic,
                                                       (uint8_t)SL_STATUS_BT_ATT_INVALID_ATT_LENGTH,
                                                       (size_t)0,
                                                       NULL,
                                                       NULL);
      }
      break;
    }

    case gattdb_iop_test_user_len_var_4: {
      // Test 5.8.1.2 (iop_test_user_len_var_4) takes place now.
      // Test 5.8.2.2 (iop_test_user_len_var_4) takes place now.
      app_log_info("Read user-type bytes for "
                   "gattdb_iop_test_user_len_var_4." APP_LOG_NL);
      sc = sl_bt_gatt_server_send_user_read_response(user_read_req->connection,
                                                     user_read_req->characteristic,
                                                     (uint8_t)SL_STATUS_OK,
                                                     iop_test_chr_user_var_4_len,
                                                     (uint8_t *)&iop_test_chr_user_var_4_arr,
                                                     NULL);
      break;
    }

    default: {
      // Invalid characteristic.
      app_log_error("Invalid characteristic, generating error response." APP_LOG_NL);
      sc = sl_bt_gatt_server_send_user_read_response(user_read_req->connection,
                                                     user_read_req->characteristic,
                                                     (uint8_t)SL_STATUS_BT_ATT_INVALID_HANDLE,
                                                     (size_t)0,
                                                     NULL,
                                                     NULL);
      break;
    }
  }

  return sc;
}

// Handle write operation for user-type characteristics.
sl_status_t handle_user_write(sl_bt_evt_gatt_server_user_write_request_t *user_write_req)
{
  sl_status_t sc = SL_STATUS_OK;
  uint8_t ec = (uint8_t)SL_STATUS_OK;

  if (user_write_req == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (user_write_req->att_opcode == sl_bt_gatt_execute_write_request) {
    // Upon sl_bt_gatt_execute_write_request the GATT server is processing the
    // execute write, the characteristic is set to 0 and should be ignored.
    // End of Test 5.7.1, every data piece arrived.
    sc = sl_bt_gatt_server_send_user_write_response(user_write_req->connection,
                                                    user_write_req->characteristic,
                                                    (uint8_t)SL_STATUS_OK);
    if (sc == SL_STATUS_OK) {
      app_log_info("Execute write user-type bytes for "
                   "gattdb_iop_test_user_len_255." APP_LOG_NL);
    }

    return sc;
  }

  switch (user_write_req->characteristic) {
    //--------------------------------
    case gattdb_iop_test_user_len_1: {
      // Test 5.6.1 (iop_test_user_len_1) takes place now.
      app_log_info("Write user-type bytes for gattdb_iop_test_user_len_1. "
                   "length: %d offset: %d." APP_LOG_NL,
                   user_write_req->value.len,
                   user_write_req->offset);
      if (user_write_req->value.len <= sizeof(iop_test_chr_user_1)) {
        memcpy(&iop_test_chr_user_1,
               user_write_req->value.data,
               user_write_req->value.len);
        sc = sl_bt_gatt_server_send_user_write_response(user_write_req->connection,
                                                        user_write_req->characteristic,
                                                        SL_STATUS_OK);
      } else {
        ec = (uint8_t)SL_STATUS_BT_ATT_INVALID_ATT_LENGTH;
      }
      break;
    }

    case gattdb_iop_test_user_len_255: {
      // Test 5.7.1 (iop_test_user_len_255) takes place now.
      if ((user_write_req->value.len + user_write_req->offset) <= sizeof(iop_test_chr_user_255_arr)) {
        if (user_write_req->att_opcode == sl_bt_gatt_prepare_write_request) {
          // Store the arriving data chunk
          memcpy(&iop_test_chr_user_255_arr[user_write_req->offset],
                 user_write_req->value.data,
                 user_write_req->value.len);
          // The sl_bt_gatt_prepare_write_request attribute was used to write.
          // The application needs to respond to this request by using the
          // sl_bt_gatt_server_send_prepare_user_write_response.
          sc = sl_bt_gatt_server_send_user_prepare_write_response(user_write_req->connection,
                                                                  user_write_req->characteristic,
                                                                  SL_STATUS_OK,
                                                                  user_write_req->offset,
                                                                  (size_t)user_write_req->value.len,
                                                                  user_write_req->value.data);

          if (sc == SL_STATUS_OK) {
            app_log_info("Prepare write user-type bytes for gattdb_iop_test_user_len_255. "
                         "length: %d offset: %d." APP_LOG_NL,
                         user_write_req->value.len,
                         user_write_req->offset);
          }
        } else {
          // The sl_bt_gatt_write_request attribute was used to write. The
          // application needs to respond to this request by using the
          // sl_bt_gatt_server_send_user_write_response.
          sc = sl_bt_gatt_server_send_user_write_response(user_write_req->connection,
                                                          user_write_req->characteristic,
                                                          SL_STATUS_OK);
          if (sc == SL_STATUS_OK) {
            app_log_info("Write / Execute write user-type bytes for gattdb_iop_test_user_len_255. "
                         "length: %d offset: %d." APP_LOG_NL,
                         user_write_req->value.len,
                         user_write_req->offset);
          }
        }
      } else {
        app_log_error("Fail to write user-type bytes for gattdb_iop_test_user_len_255. "
                      "length: %d offset: %d." APP_LOG_NL,
                      user_write_req->value.len,
                      user_write_req->offset);
        ec = (uint8_t)SL_STATUS_BT_ATT_INVALID_ATT_LENGTH;
      }
      break;
    }

    case gattdb_iop_test_user_len_var_4: {
      // Test 5.8.1.1 (iop_test_user_len_var_4) takes place now.
      // Test 5.8.2.1 (iop_test_user_len_var_4) takes place now.
      app_log_info("Write user-type bytes for "
                   "gattdb_iop_test_user_len_var_4. length: %d." APP_LOG_NL,
                   user_write_req->value.len);

      if (user_write_req->value.len <= sizeof(iop_test_chr_user_var_4_arr)) {
        // Save length for next reading
        iop_test_chr_user_var_4_len = user_write_req->value.len;
        memcpy(&iop_test_chr_user_var_4_arr,
               user_write_req->value.data,
               iop_test_chr_user_var_4_len);
        sc = sl_bt_gatt_server_send_user_write_response(user_write_req->connection,
                                                        user_write_req->characteristic,
                                                        SL_STATUS_OK);
      } else {
        ec = (uint8_t)SL_STATUS_BT_ATT_INVALID_ATT_LENGTH;
      }
      break;
    }

    //--------------------------------
    case gattdb_ota_control: {
      // The value of OTA Control characteristic was changed. This indicates
      // there is a request to perform OTA-DFU.
      // Test 6.1 (IOP Test OTA update with ACK) takes place now.
      // Test 6.2 (IOP Test OTA update without ACK) takes place now.
      app_log_info("Start OTA-DFU." APP_LOG_NL);
      ota_dfu_request = true;
      sc = sl_bt_gatt_server_send_user_write_response(user_write_req->connection,
                                                      user_write_req->characteristic,
                                                      SL_STATUS_OK);
      sl_bt_connection_close(user_write_req->connection);
      break;
    }

    //--------------------------------
    case gattdb_iop_test_phase3_control: {
      uint8_t gattdb_control_value[3];
      app_log_info("Write user-type bytes for gattdb_iop_test_phase3_control. "
                   "length: %d." APP_LOG_NL,
                   user_write_req->value.len);
      memcpy(&gattdb_control_value,
             user_write_req->value.data,
             user_write_req->value.len);
      sc = sl_bt_gatt_server_send_user_write_response(user_write_req->connection,
                                                      user_write_req->characteristic,
                                                      SL_STATUS_OK);
      app_log_status_error(sc);
      app_log_info("Mobile OS: 0x%02x, Security level: 0x%02x." APP_LOG_NL,
                   gattdb_control_value[0],
                   gattdb_control_value[1]);

      //--------------------------------
      // Mobile OS type
      // Being set for Test 7.1 (Throughput).
      if ((gattdb_control_value[0] & 0xFF) == ANDROID) {
        app_log_info("Android OS." APP_LOG_NL);
        mobile_os = ANDROID;
      } else if ((gattdb_control_value[0] & 0xFF) == IOS) {
        app_log_info("IOS\n." APP_LOG_NL);
        mobile_os = IOS;
      } else {
        app_log_info("[0x%02x] Mobile OS unchanged." APP_LOG_NL,
                     (int)(gattdb_control_value[0] & 0xFF));
      }

      //--------------------------------
      // Security level
      switch (gattdb_control_value[1] & 0xFF) {
        // Unauthenticated pairing with encryption
        case sl_bt_connection_mode1_level2: {
          // Test 7.2 (Security/Pairing) takes place now.
          connection_security = sl_bt_connection_mode1_level2;
          sc = sl_bt_connection_close(user_write_req->connection);
          break;
        }

        // Authenticated pairing with encryption
        case sl_bt_connection_mode1_level3: {
          // Test 7.3 (Security/Authentication) takes place now.
          connection_security = sl_bt_connection_mode1_level3;
          sc = sl_bt_connection_close(user_write_req->connection);
          break;
        }

        // Authenticated Secure Connections pairing with encryption using a
        // 128-bit strength encryption key
        case sl_bt_connection_mode1_level4: {
          // Test 7.4 (Security/Bonding) takes place now.
          connection_security = sl_bt_connection_mode1_level4;
          sc = sl_bt_connection_close(user_write_req->connection);
          break;
        }

        default: {
          app_log_info("[0x%02x] No request to change security level." APP_LOG_NL,
                       (int)(gattdb_control_value[1] & 0xFF));
          break;
        }
      }

      if (sc == SL_STATUS_IDLE) {
        app_log_info("Connection kept open." APP_LOG_NL);
      } else {
        app_log_info("Connection closed." APP_LOG_NL);
      }
      break;
    }

    default: {
      ec = (uint8_t)SL_STATUS_BT_ATT_INVALID_HANDLE;
      break;
    }
  }

  if (ec != (uint8_t)SL_STATUS_OK) {
    // It is either an invalid characteristic or invalid value length.
    app_log_error("[E: 0x%04x] sl_bt_evt_gatt_server_user_write_request_id: "
                  "invalid characteristic [%04d] or invalid value "
                  "length [%04d]." APP_LOG_NL,
                  (int)ec,
                  (int)user_write_req->characteristic,
                  (int)user_write_req->value.len);

    sc = sl_bt_gatt_server_send_user_write_response(user_write_req->connection,
                                                    user_write_req->characteristic,
                                                    ec);
  }

  return sc;
}

// Set and start timer for corresponding test case.
sl_status_t handle_timer_start(sl_bt_evt_gatt_server_characteristic_status_t *char_stat)
{
  sl_status_t sc = SL_STATUS_OK;
  timer_cb_data = char_stat->characteristic;

  switch (char_stat->characteristic) {
    case gattdb_iop_test_notify_len_1: {
      if (char_stat->client_config_flags == sl_bt_gatt_notification) {
        // Test 4.7 (iop_test_notify_len_1) takes place now.
        sc = sl_simple_timer_start(&timer,
                                   TIMER_TRIGGER,
                                   timer_cb,
                                   (void *)&timer_cb_data,
                                   false);
      }
      break;
    }

    case gattdb_iop_test_notify_len_mtu_3: {
      if (char_stat->client_config_flags == sl_bt_gatt_notification) {
        // Test 4.8 (iop_test_notify_len_mtu_3) takes place now.
        sc = sl_simple_timer_start(&timer,
                                   TIMER_TRIGGER,
                                   timer_cb,
                                   (void *)&timer_cb_data,
                                   false);
      }
      break;
    }

    case gattdb_iop_test_indicate_len_1: {
      if (char_stat->client_config_flags == sl_bt_gatt_indication) {
        // Test 4.9 (iop_test_indicate_len_1) takes place now.
        sc = sl_simple_timer_start(&timer,
                                   TIMER_TRIGGER,
                                   timer_cb,
                                   (void *)&timer_cb_data,
                                   false);
      }
      break;
    }

    case gattdb_iop_test_indicate_len_mtu_3: {
      if (char_stat->client_config_flags == sl_bt_gatt_indication) {
        // Test 4.10 (iop_test_indicate_len_mtu_3) takes place now.
        sc = sl_simple_timer_start(&timer,
                                   TIMER_TRIGGER,
                                   timer_cb,
                                   (void *)&timer_cb_data,
                                   false);
      }
      break;
    }

    //--------------------------------
    case gattdb_iop_test_throughput: {
      if (char_stat->client_config_flags == sl_bt_gatt_notification) {
        // Test 7.1 (Throughput) takes place now.
        uint32_t timeout_ms;

        if (mobile_os == IOS) {
          // IOS
          timeout_ms = (int)(CONN_PARAM_MULTIPLIER
                             * connection_interval
                             / PACK_NUM_IOS);
        } else {
          // Android
          timeout_ms = (int)(CONN_PARAM_MULTIPLIER
                             * connection_interval
                             / PACK_NUM_ANDROID);
        }

        sc = sl_simple_timer_start(&timer,
                                   timeout_ms,
                                   timer_cb,
                                   (void *)&timer_cb_data,
                                   true);
        if (sc == SL_STATUS_OK) {
          app_log_info("Simple timer for throughput started." APP_LOG_NL);
        }
      } else if (char_stat->client_config_flags == sl_bt_gatt_disable) {
        throughput_enabled = false;
      }
      break;
    }

    default: {
      break;
    }
  }
  return sc;
}

// Timer Callback
static void timer_cb(sl_simple_timer_t *handle, void *data)
{
  (void)handle;
  sl_status_t sc = SL_STATUS_OK;

  switch (*(uint16_t *)data) {
    case gattdb_iop_test_notify_len_1: {
      sc = sl_bt_gatt_server_notify_all(gattdb_iop_test_notify_len_1,
                                        sizeof(iop_test_notification_1),
                                        &iop_test_notification_1);
      break;
    }

    case gattdb_iop_test_notify_len_mtu_3: {
      sc = sl_bt_gatt_server_notify_all(gattdb_iop_test_notify_len_mtu_3,
                                        mtu_size - 3,
                                        iop_test_notification_250_arr);
      break;
    }

    case gattdb_iop_test_indicate_len_1: {
      sc = sl_bt_gatt_server_notify_all(gattdb_iop_test_indicate_len_1,
                                        sizeof(iop_test_notification_1),
                                        &iop_test_notification_1);
      break;
    }

    case gattdb_iop_test_indicate_len_mtu_3: {
      sc = sl_bt_gatt_server_notify_all(gattdb_iop_test_indicate_len_mtu_3,
                                        mtu_size - 3,
                                        iop_test_notification_250_arr);
      break;
    }

    case gattdb_iop_test_throughput: {
      if (throughput_enabled) {
        sc = sl_bt_gatt_server_notify_all(gattdb_iop_test_throughput,
                                          pdu_size - 7,
                                          iop_test_notification_250_arr);
      } else {
        sc = sl_simple_timer_stop(&timer); // Throughput finished, stop timer.
        if (sc == SL_STATUS_OK) {
          app_log_info("Simple timer for throughput stopped." APP_LOG_NL);
        }
      }
      break;
    }

    default: {
      break;
    }
  }
  app_log_status_error(sc);
}

// Handle display update.
void set_display(display_state_t state)
{
  sl_status_t sc;

  // Clear display
  app_memlcd_clear();

  // Read device name from the local GATT table
  size_t value_len;
  uint8_t value[MAX_CHARS_PER_LINE + 1];
  sc = sl_bt_gatt_server_read_attribute_value(gattdb_device_name,
                                              0,
                                              sizeof(value) - 1,
                                              &value_len,
                                              value);
  app_log_status_error(sc);

  if (sc == SL_STATUS_OK) {
    // Make sure that the attribute value is null terminated.
    value[value_len] = '\0';

    app_memlcd_draw_string(FONT_NORMAL, "%s", (char *)value);
    app_memlcd_newline(1);
  } else {
    app_memlcd_newline(2);
  }

  // Display bluetooth address
  app_memlcd_draw_string(FONT_NORMAL, ADDRESS_STRING);
  app_memlcd_draw_string(FONT_NARROW,
                         INDENT_STRING "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
                         public_address.addr[5],
                         public_address.addr[4],
                         public_address.addr[3],
                         public_address.addr[2],
                         public_address.addr[1],
                         public_address.addr[0]);
  app_memlcd_newline(1);

  // Display state
  switch (state) {
    case IDLE: {
      app_memlcd_draw_string(FONT_NORMAL, STATE_STRING);
      app_memlcd_draw_string(FONT_NARROW, INDENT_STRING "Advertising");
      break;
    }

    case CONNECTED: {
      app_memlcd_draw_string(FONT_NORMAL, STATE_STRING);
      app_memlcd_draw_string(FONT_NARROW, INDENT_STRING "Connected");
      break;
    }

    case DISPLAY_PASSKEY: {
      // Display key for inputting on other device
      app_memlcd_draw_string(FONT_NORMAL, PASSKEY_STRING);
      app_memlcd_draw_string(FONT_NARROW, INDENT_STRING "%lu", passkey);
      break;
    }

    case BOND_SUCCESS: {
      app_memlcd_draw_string(FONT_NORMAL, STATE_STRING);
      app_memlcd_draw_string(FONT_NARROW, INDENT_STRING "Bond success");
      break;
    }

    case BOND_FAILURE: {
      app_memlcd_draw_string(FONT_NORMAL, STATE_STRING);
      app_memlcd_draw_string(FONT_NARROW, INDENT_STRING "Bond failure");
      break;
    }

    default: {
      break;
    }
  }
  app_memlcd_newline(1);

  // Update screen
  app_memlcd_update();
}
