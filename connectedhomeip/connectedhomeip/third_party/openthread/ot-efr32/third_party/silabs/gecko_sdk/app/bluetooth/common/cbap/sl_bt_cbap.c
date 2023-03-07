/***************************************************************************//**
 * @file
 * @brief Certificate Based Authentication and Pairing implementation
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

#include <stdbool.h>
#include "em_common.h"
#include "em_system.h"

#include "app_assert.h"
#include "gatt_db.h"
#include "sl_bluetooth.h"
#include "sl_simple_timer.h"
#include "sl_bt_cbap_config.h"
#include "sl_bt_cbap_root_cert.h"
#include "sl_bt_cbap.h"
#include "sl_bt_cbap_lib.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

// -----------------------------------------------------------------------------
// Defines

#if defined(SL_CATALOG_APP_LOG_PRESENT) && SL_BT_CBAP_LOG
#define sl_bt_cbap_log_debug(...)           app_log_debug(__VA_ARGS__)
#define sl_bt_cbap_log_info(...)            app_log_info(__VA_ARGS__)
#define sl_bt_cbap_log_error(...)           app_log_error(__VA_ARGS__)
#define sl_bt_cbap_log_hexdump(p_data, len) app_log_hexdump_debug(p_data, len)
#else
#define sl_bt_cbap_log_debug(...)
#define sl_bt_cbap_log_info(...)
#define sl_bt_cbap_log_error(...)
#define sl_bt_cbap_log_hexdump(p_data, len)
#endif

#define IS_PERIPHERAL_IN_PROGRESS (cbap_peripheral_state > 0 && cbap_peripheral_state < SL_BT_CBAP_PERIPHERAL_STATE_NUM - 1)
#define IS_CENTRAL_IN_PROGRESS    (cbap_central_state > 0 && cbap_central_state < SL_BT_CBAP_CENTRAL_STATE_NUM - 1)

#define UUID_16_LEN                   2
#define UUID_128_LEN                  16
#define HANDLE_NOT_INITIALIZED        0

#define GAP_INCOMPLETE_16B_UUID       0x02 // Incomplete List of 16-bit Service Class UUIDs
#define GAP_COMPLETE_16B_UUID         0x03 // Complete List of 16-bit Service Class UUIDs
#define GAP_INCOMPLETE_128B_UUID      0x06 // Incomplete List of 128-bit Service Class UUIDs
#define GAP_COMPLETE_128B_UUID        0x07 // Complete List of 128-bit Service Class UUIDs

#define CHAIN_LINK_DATA_LEN           192  // Length of an NVM3 chunk
#define CHAIN_LINK_DATA_NUM           4    // Number of how many chunks needed for a certificate

#define OOB_SIGNATURE_LEN             64
#define CERT_IND_CHUNK_LEN            100
#define EC_PUB_KEY_LEN                65
#define PUB_KEY_OFFSET                26

#define OOB_RANDOM_LEN                (sizeof(aes_key_128))
#define OOB_DATA_LEN                  (2 * OOB_RANDOM_LEN)
#define SIGNED_OOB_DATA_LEN           (OOB_DATA_LEN + OOB_SIGNATURE_LEN)

#define TIMEOUT                       5000 // ms
#define NO_CALLBACK_DATA              (void *)NULL // Callback has no parameters

// -----------------------------------------------------------------------------
// Type definitions.

typedef struct characteristic_128_ref_s {
  uint16_t handle;
  uint8_t uuid[UUID_128_LEN];
} characteristic_128_ref_t;

typedef enum {
  CHAR_CENTRAL_CERT,
  CHAR_PERIPHERAL_CERT,
  CHAR_CENTRAL_OOB,
  CHAR_PERIPHERAL_OOB,
  CHAR_NUM
} characteristics_t;

// -----------------------------------------------------------------------------
// Module variables.

// Device role
static sl_bt_cbap_role_t role;

// Handle of the active connection.
static uint8_t connection = SL_BT_INVALID_CONNECTION_HANDLE;

// Root certificate in PEM format.
const char *root_certificate_pem = SL_BT_CBAP_ROOT_CERT;

// Reference to the CBAP service.
static uint32_t cbap_service_handle = HANDLE_NOT_INITIALIZED;
static const uint8_t cbap_service_uuid[] = {
  0x10, 0x56, 0x28, 0xd0, 0x40, 0xdd, 0x8e, 0x91,
  0x4d, 0x41, 0x47, 0x81, 0xc6, 0x8c, 0x81, 0xd8
};

// Reference to the CBAP characteristics.
static characteristic_128_ref_t cbap_characteristics[] = {
  {   // CHAR_CENTRAL_CERT
    .handle = HANDLE_NOT_INITIALIZED,
    .uuid = {
      0xca, 0x25, 0xf3, 0xa2, 0xf6, 0xda, 0xbd, 0xb6,
      0x69, 0x4a, 0xaa, 0x08, 0xf9, 0xd0, 0x5f, 0x31
    }
  },
  {   // CHAR_PERIPHERAL_CERT
    .handle = HANDLE_NOT_INITIALIZED,
    .uuid = {
      0x66, 0x50, 0xfd, 0x84, 0x4d, 0xad, 0xa2, 0x99,
      0xc9, 0x4f, 0xf5, 0x16, 0x9e, 0xda, 0xf6, 0x0c
    }
  },
  {   // CHAR_CENTRAL_OOB
    .handle = HANDLE_NOT_INITIALIZED,
    .uuid = {
      0x2c, 0x19, 0xf1, 0xeb, 0x85, 0xcd, 0xb6, 0x8a,
      0x2c, 0x4e, 0x7d, 0x89, 0x51, 0x57, 0xd3, 0xe8
    }
  },
  {   // CHAR_PERIPHERAL_OOB
    .handle = HANDLE_NOT_INITIALIZED,
    .uuid = {
      0xab, 0x69, 0xb2, 0x5e, 0xb8, 0x41, 0xb3, 0x8b,
      0x82, 0x41, 0x51, 0xa2, 0x41, 0xcb, 0x91, 0x69
    }
  }
};

// State of the central device
static sl_bt_cbap_central_state_t cbap_central_state = SL_BT_CBAP_CENTRAL_SCANNING;
// Pointing to the characteristic that shall be discovered next
static characteristics_t char_state = (characteristics_t)0;
// State of the peripheral device
static sl_bt_cbap_peripheral_state_t cbap_peripheral_state = SL_BT_CBAP_PERIPHERAL_IDLE;

// Timer handle
static sl_simple_timer_t state_timer;

// Remote certificate which was sent over GATT in DER format
static uint8_t remote_certificate_der[CHAIN_LINK_DATA_LEN * CHAIN_LINK_DATA_NUM] = { 0 };
static uint32_t remote_certificate_der_len = 0;
static bool remote_cert_arrived = false;

// Device certificate in DER format
static uint8_t device_certificate_der[CHAIN_LINK_DATA_LEN * CHAIN_LINK_DATA_NUM] = { 0 };
static uint32_t device_certificate_der_len = 0;
static uint32_t dev_cert_sending_progression = 0;
static bool device_cert_sent = false;

static uint8_t signed_device_oob_data[SIGNED_OOB_DATA_LEN];
static size_t signed_device_oob_len = 0;

// -----------------------------------------------------------------------------
// Private function declarations

// Central device bluetooth event handler.
static void on_event_peripheral(sl_bt_msg_t *evt);
// Peripheral device bluetooth event handler.
static void on_event_central(sl_bt_msg_t *evt);

// Reset CBAP process states, flags and timers.
static void cbap_reset(void);

// Search for a Service UUID in scan report.
static bool find_service_in_advertisement(const uint8_t *scan_data,
                                          uint8_t scan_data_len,
                                          const uint8_t *uuid,
                                          uint8_t uuid_len);

// Start or stop timer for timeout check.
static void set_timeout(bool activate);
// Timer callback.
static void state_timer_cb(sl_simple_timer_t *handle, void *data);

// -----------------------------------------------------------------------------
// Public function definitions

// Initialize the component. Import and validate the device and root certificate.
void sl_bt_cbap_init(void)
{
  sl_status_t sc;
  sc = sl_bt_cbap_lib_init(root_certificate_pem,
                           device_certificate_der,
                           &device_certificate_der_len);
  app_assert_status(sc);
  sl_bt_cbap_log_info("Device certificate verified." APP_LOG_NL);

  cbap_reset();
}

// Start CBAP procedure.
sl_status_t sl_bt_cbap_start(sl_bt_cbap_role_t cbap_role,
                             uint8_t connection_handle)
{
  sl_status_t sc;
  if (IS_PERIPHERAL_IN_PROGRESS || IS_CENTRAL_IN_PROGRESS) {
    return SL_STATUS_IN_PROGRESS;
  }

  role = cbap_role;
  connection = connection_handle;

  if (role == SL_BT_CBAP_ROLE_CENTRAL) {
    // Discover CBAP service on the peripheral device
    sc = sl_bt_gatt_discover_primary_services_by_uuid(connection,
                                                      sizeof(cbap_service_uuid),
                                                      (const uint8_t *)cbap_service_uuid);
    app_assert_status(sc);
    cbap_central_state = SL_BT_CBAP_CENTRAL_DISCOVER_SERVICES;
    sl_bt_cbap_central_on_event(cbap_central_state);
  }

  set_timeout(true);
  return SL_STATUS_OK;
}

// Bluetooth stack event handler.
void sli_bt_cbap_on_event(sl_bt_msg_t *evt)
{
  switch (role) {
    case SL_BT_CBAP_ROLE_PERIPHERAL:
      on_event_peripheral(evt);
      break;
    case SL_BT_CBAP_ROLE_CENTRAL:
      on_event_central(evt);
      break;
  }
}

// Search for a the CBAP Service UUID in scan report.
bool sl_bt_cbap_find_service_in_advertisement(const uint8_t *scan_data,
                                              uint8_t scan_data_len)
{
  return find_service_in_advertisement(scan_data,
                                       scan_data_len,
                                       cbap_service_uuid,
                                       sizeof(cbap_service_uuid));
}

// -----------------------------------------------------------------------------
// Private function definitions

/*******************************************************************************
 * Peripheral device bluetooth event handler.
 ******************************************************************************/
static void on_event_peripheral(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Request OOB data from both device
      sc = sl_bt_sm_configure(SL_BT_SM_CONFIGURATION_OOB_FROM_BOTH_DEVICES_REQUIRED,
                              sl_bt_sm_io_capability_noinputnooutput);
      app_assert_status(sc);
      break;

    //--------------------------------
    // Triggered whenever the connection parameters are changed
    case sl_bt_evt_connection_parameters_id:
      if (evt->data.evt_connection_parameters.connection != connection) {
        break;
      }

      sl_bt_cbap_log_debug("Security mode: %i" APP_LOG_NL,
                           evt->data.evt_connection_parameters.security_mode);
      if (evt->data.evt_connection_parameters.security_mode > sl_bt_connection_mode1_level1
          && cbap_peripheral_state != SL_BT_CBAP_PERIPHERAL_CENTRAL_OOB_OK) {
        sl_bt_cbap_log_error("The central device increased the security level with " \
                             "no CBAP. Disconnecting." APP_LOG_NL);
        sl_bt_on_cbap_error();
        cbap_reset();
        break;
      }

      if (evt->data.evt_connection_parameters.security_mode == sl_bt_connection_mode1_level4) {
        cbap_peripheral_state = SL_BT_CBAP_PERIPHERAL_DONE;
        sl_bt_cbap_peripheral_on_event(cbap_peripheral_state);
        cbap_reset();
      }
      break;

    case sl_bt_evt_gatt_server_user_write_request_id:
      if (evt->data.evt_gatt_server_user_write_request.connection != connection) {
        break;
      }

      // Set default response parameters.
      sc = SL_STATUS_BT_ATT_WRITE_REQUEST_REJECTED;
      // Receiving Certificate from central device
      if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_central_cert) {
        if (remote_cert_arrived == false) {
          // First byte indicates that it is a last packet or not
          memcpy(&remote_certificate_der[remote_certificate_der_len],
                 &evt->data.evt_gatt_server_user_write_request.value.data[1],
                 evt->data.evt_gatt_server_user_write_request.value.len - 1);
          remote_certificate_der_len += evt->data.evt_gatt_server_user_write_request.value.len - 1;
          sc = SL_STATUS_OK;
          if (evt->data.evt_gatt_server_user_write_request.value.data[0] == 0) {
            // Last packet of the remote cert arrived
            sl_bt_cbap_log_info("Getting certificate from central." APP_LOG_NL);
            remote_cert_arrived = true;
            sc = sl_bt_cbap_lib_process_remote_cert(remote_certificate_der,
                                                    remote_certificate_der_len);
            if (sc == SL_STATUS_OK) {
              app_assert(cbap_peripheral_state == SL_BT_CBAP_PERIPHERAL_IDLE,
                         "Unexpected peripheral state.");
              cbap_peripheral_state = SL_BT_CBAP_PERIPHERAL_CENTRAL_CERT_OK;
              sl_bt_cbap_peripheral_on_event(cbap_peripheral_state);
              set_timeout(true);
            } else {
              sl_bt_cbap_log_error("Remote certificate verification failed. " \
                                   "Disconnecting." APP_LOG_NL);
              sl_bt_on_cbap_error();
              cbap_reset();
              break;
            }
          }
          // Map status code to a valid attribute error.
          if (SL_STATUS_OK != sc) {
            sc = SL_STATUS_BT_ATT_WRITE_REQUEST_REJECTED;
          }
        } else {
          sc = SL_STATUS_BT_ATT_PROCEDURE_ALREADY_IN_PROGRESS;
        }
        sl_bt_gatt_server_send_user_write_response(evt->data.evt_gatt_server_user_write_request.connection,
                                                   evt->data.evt_gatt_server_user_write_request.characteristic,
                                                   (uint8_t)sc);
      }
      // Receiving OOB data from central device
      else if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_central_oob ) {
        sl_bt_cbap_log_info("Getting OOB data from central." APP_LOG_NL);
        aes_key_128 remote_random;
        aes_key_128 remote_confirm;
        uint8_t remote_oob_signature[OOB_SIGNATURE_LEN];
        memcpy(&remote_random,
               &evt->data.evt_gatt_server_user_write_request.value.data[0],
               sizeof(remote_random));
        memcpy(&remote_confirm,
               &evt->data.evt_gatt_server_user_write_request.value.data[16],
               sizeof(remote_confirm));
        memcpy(&remote_oob_signature,
               &evt->data.evt_gatt_server_user_write_request.value.data[32],
               OOB_SIGNATURE_LEN);

        sc = sl_bt_gatt_server_send_user_write_response(evt->data.evt_gatt_server_user_write_request.connection,
                                                        evt->data.evt_gatt_server_user_write_request.characteristic,
                                                        SL_STATUS_OK);
        app_assert_status(sc);

        sl_bt_cbap_log_debug("Remote OOB data:" APP_LOG_NL);
        sl_bt_cbap_log_hexdump(&remote_random, sizeof(aes_key_128));
        sl_bt_cbap_log_debug(APP_LOG_NL);
        sl_bt_cbap_log_hexdump(&remote_confirm, sizeof(aes_key_128));
        sl_bt_cbap_log_debug(APP_LOG_NL);
        sl_bt_cbap_log_debug("Remote OOB signature:" APP_LOG_NL);
        sl_bt_cbap_log_hexdump(&remote_oob_signature, OOB_SIGNATURE_LEN);
        sl_bt_cbap_log_debug(APP_LOG_NL);

        sc = sl_bt_cbap_lib_verify_remote_oob_data(remote_random.data,
                                                   remote_confirm.data,
                                                   remote_oob_signature);
        app_assert_status(sc);
        sl_bt_cbap_log_info("Remote OOB data verified." APP_LOG_NL);
        sc = sl_bt_sm_set_remote_oob(1, remote_random, remote_confirm);
        app_assert_status(sc);
        sc = sl_bt_cbap_destroy_key();
        app_assert_status(sc);

        app_assert(cbap_peripheral_state == SL_BT_CBAP_PERIPHERAL_CENTRAL_CERT_OK,
                   "Unexpected peripheral state.");
        cbap_peripheral_state = SL_BT_CBAP_PERIPHERAL_CENTRAL_OOB_OK;
        sl_bt_cbap_peripheral_on_event(cbap_peripheral_state);
        set_timeout(true);
      }
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      if (evt->data.evt_gatt_server_characteristic_status.connection != connection) {
        break;
      }

      if (gattdb_peripheral_cert == evt->data.evt_gatt_server_characteristic_status.characteristic) {
        if (sl_bt_gatt_server_client_config == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          if (sl_bt_gatt_indication == (sl_bt_gatt_client_config_flag_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags
              && device_cert_sent == false) {
            uint8_t buff[CERT_IND_CHUNK_LEN + 1];
            buff[0] = 1;
            memcpy(&buff[1], device_certificate_der, CERT_IND_CHUNK_LEN);
            sc = sl_bt_gatt_server_send_indication(connection,
                                                   gattdb_peripheral_cert,
                                                   CERT_IND_CHUNK_LEN + 1,
                                                   buff);
            app_assert_status(sc);
            dev_cert_sending_progression += CERT_IND_CHUNK_LEN;
          }
        }
        // Sending Peripheral certificate to Central device
        else if (sl_bt_gatt_server_confirmation == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          if (device_cert_sent == false) {
            uint32_t remaining = device_certificate_der_len - dev_cert_sending_progression;
            uint8_t buff[CERT_IND_CHUNK_LEN + 1];
            uint8_t len = 0;
            if (remaining > CERT_IND_CHUNK_LEN) {
              buff[0] = 1;
              len = CERT_IND_CHUNK_LEN + 1;
            } else {
              // Send last chunk
              buff[0] = 0;
              len = remaining + 1;
              device_cert_sent = true;
            }
            memcpy(&buff[1], &device_certificate_der[dev_cert_sending_progression], len - 1);
            dev_cert_sending_progression += len - 1;
            sc = sl_bt_gatt_server_send_indication(connection,
                                                   gattdb_peripheral_cert,
                                                   len,
                                                   buff);
            app_assert_status(sc);
          }
        }
      }
      // Sending Peripheral OOB data to Central device
      else if (gattdb_peripheral_oob == evt->data.evt_gatt_server_characteristic_status.characteristic ) {
        if (sl_bt_gatt_server_client_config == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags
            && sl_bt_gatt_indication == (sl_bt_gatt_client_config_flag_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags) {
          aes_key_128 device_random;
          aes_key_128 device_confirm;
          // Generate device oob data and send over GATT
          sc = sl_bt_sm_set_oob(1, &device_random, &device_confirm);
          app_assert_status(sc);

          sl_bt_cbap_log_debug("Device OOB Data:" APP_LOG_NL);
          sl_bt_cbap_log_hexdump(&device_random, OOB_RANDOM_LEN);
          sl_bt_cbap_log_debug(APP_LOG_NL);
          sl_bt_cbap_log_hexdump(&device_confirm, OOB_RANDOM_LEN);
          sl_bt_cbap_log_debug(APP_LOG_NL);

          sc = sl_bt_cbap_lib_sign_device_oob_data(device_random.data,
                                                   device_confirm.data,
                                                   signed_device_oob_data,
                                                   &signed_device_oob_len);
          app_assert_status(sc);

          sl_bt_cbap_log_debug("Device OOB Signature:" APP_LOG_NL);
          sl_bt_cbap_log_hexdump(&signed_device_oob_data[OOB_DATA_LEN],
                                 OOB_SIGNATURE_LEN);

          sl_bt_cbap_log_debug(APP_LOG_NL);

          sc = sl_bt_gatt_server_send_indication(connection,
                                                 gattdb_peripheral_oob,
                                                 signed_device_oob_len,
                                                 signed_device_oob_data);
          app_assert_status(sc);
        }
      }
      break;
  }
}

/*******************************************************************************
 * Central device bluetooth event handler.
 ******************************************************************************/
static void on_event_central(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    //--------------------------------
    // Triggered whenever the connection parameters are changed
    case sl_bt_evt_connection_parameters_id:
      if (evt->data.evt_connection_parameters.connection != connection) {
        break;
      }

      sl_bt_cbap_log_debug("Security mode: %i" APP_LOG_NL, evt->data.evt_connection_parameters.security_mode);
      if (evt->data.evt_connection_parameters.security_mode > sl_bt_connection_mode1_level1
          && cbap_central_state != SL_BT_CBAP_CENTRAL_INCREASE_SECURITY) {
        sl_bt_cbap_log_error("Security level has been increased with no CBAP. " \
                             "Disconnecting." APP_LOG_NL);
        sl_bt_on_cbap_error();
        cbap_reset();
        break;
      }

      if (evt->data.evt_connection_parameters.security_mode == sl_bt_connection_mode1_level4) {
        cbap_central_state = SL_BT_CBAP_CENTRAL_DONE;
        sl_bt_cbap_central_on_event(cbap_central_state);
        cbap_reset();
      }
      break;

    // -------------------------------
    // This event is generated when a new service is discovered
    case sl_bt_evt_gatt_service_id:
      if (evt->data.evt_gatt_service.connection != connection) {
        break;
      }

      if (cbap_service_handle == HANDLE_NOT_INITIALIZED) {
        // Save service handle for future reference
        cbap_service_handle = evt->data.evt_gatt_service.service;
        sl_bt_cbap_log_debug("Service handle found: %i" APP_LOG_NL, cbap_service_handle);
      }
      break;

    // -------------------------------
    // This event is generated when a new characteristic is discovered
    case sl_bt_evt_gatt_characteristic_id:
      if (evt->data.evt_gatt_characteristic.connection != connection) {
        break;
      }

      if (cbap_characteristics[char_state].handle == HANDLE_NOT_INITIALIZED) {
        // Save characteristic handle for future reference
        cbap_characteristics[char_state].handle = evt->data.evt_gatt_characteristic.characteristic;
        sl_bt_cbap_log_debug("Characteristic handle found: %i" APP_LOG_NL,
                             cbap_characteristics[char_state].handle);
      }
      break;

    // -------------------------------
    // This event is generated for various procedure completions, e.g. when a
    // write procedure is completed, or service discovery is completed
    case sl_bt_evt_gatt_procedure_completed_id:
      if (evt->data.evt_gatt_procedure_completed.connection != connection) {
        break;
      }

      // Check result
      if (evt->data.evt_gatt_procedure_completed.result != 0) {
        sl_bt_cbap_log_error("GATT procedure failed [E:%i]. Disconnecting." APP_LOG_NL,
                             evt->data.evt_gatt_procedure_completed.result);
        sl_bt_on_cbap_error();
        cbap_reset();
        break;
      }

      switch (cbap_central_state) {
        case SL_BT_CBAP_CENTRAL_DISCOVER_SERVICES: {
          // Continue by finding the characteristics under the CBAP service.
          char_state = (characteristics_t)0; // Start with the first characteristic
          sc = sl_bt_gatt_discover_characteristics_by_uuid(evt->data.evt_gatt_procedure_completed.connection,
                                                           cbap_service_handle,
                                                           sizeof(cbap_characteristics[char_state].uuid),
                                                           (const uint8_t *)cbap_characteristics[char_state].uuid);
          app_assert_status(sc);
          cbap_central_state = SL_BT_CBAP_CENTRAL_DISCOVER_CHARACTERISTICS;
          sl_bt_cbap_central_on_event(cbap_central_state);
          set_timeout(true);
          break;
        }

        case SL_BT_CBAP_CENTRAL_DISCOVER_CHARACTERISTICS: {
          char_state++;
          if (char_state < CHAR_NUM) {
            // Find the next characteristic
            sc = sl_bt_gatt_discover_characteristics_by_uuid(evt->data.evt_gatt_procedure_completed.connection,
                                                             cbap_service_handle,
                                                             sizeof(cbap_characteristics[char_state].uuid),
                                                             (const uint8_t *)cbap_characteristics[char_state].uuid);
            app_assert_status(sc);
          } else {
            // Get Peripheral certificates
            sc = sl_bt_gatt_set_characteristic_notification(evt->data.evt_gatt_procedure_completed.connection,
                                                            cbap_characteristics[CHAR_PERIPHERAL_CERT].handle,
                                                            sl_bt_gatt_indication);
            app_assert_status(sc);
            cbap_central_state = SL_BT_CBAP_CENTRAL_GET_PERIPHERAL_CERT;
            sl_bt_cbap_central_on_event(cbap_central_state);
            set_timeout(true);
          }
          break;
        }

        case SL_BT_CBAP_CENTRAL_SEND_CENTRAL_CERT: {
          if (!device_cert_sent) {
            uint32_t remaining = device_certificate_der_len
                                 - dev_cert_sending_progression;
            uint8_t buff[CERT_IND_CHUNK_LEN + 1];
            uint8_t len = 0;
            if (remaining > CERT_IND_CHUNK_LEN) {
              buff[0] = 1;
              memcpy(&buff[1],
                     &device_certificate_der[dev_cert_sending_progression],
                     CERT_IND_CHUNK_LEN);
              dev_cert_sending_progression += CERT_IND_CHUNK_LEN;
              len = CERT_IND_CHUNK_LEN + 1;
            } else {
              // Last chunk
              buff[0] = 0;
              memcpy(&buff[1],
                     &device_certificate_der[dev_cert_sending_progression],
                     remaining);
              len = remaining + 1;
              device_cert_sent = true;
            }
            sc = sl_bt_gatt_write_characteristic_value(connection,
                                                       cbap_characteristics[CHAR_CENTRAL_CERT].handle,
                                                       len,
                                                       buff);
            app_assert_status(sc);
          } else {
            // If certificate exchange completed get OOB data. Enable indication.
            sc = sl_bt_gatt_set_characteristic_notification(evt->data.evt_gatt_procedure_completed.connection,
                                                            cbap_characteristics[CHAR_PERIPHERAL_OOB].handle,
                                                            sl_bt_gatt_indication);
            app_assert_status(sc);
            aes_key_128 device_random;
            aes_key_128 device_confirm;
            // Generate device oob data and send over GATT
            sc = sl_bt_sm_set_oob(1, &device_random, &device_confirm);
            app_assert_status(sc);

            sl_bt_cbap_log_debug("Device OOB Data:" APP_LOG_NL);
            sl_bt_cbap_log_hexdump(&device_random, OOB_RANDOM_LEN);
            sl_bt_cbap_log_debug(APP_LOG_NL);
            sl_bt_cbap_log_hexdump(&device_confirm, OOB_RANDOM_LEN);
            sl_bt_cbap_log_debug(APP_LOG_NL);

            sc = sl_bt_cbap_lib_sign_device_oob_data(device_random.data,
                                                     device_confirm.data,
                                                     signed_device_oob_data,
                                                     &signed_device_oob_len);
            app_assert_status(sc);

            sl_bt_cbap_log_debug("Device OOB Signature:" APP_LOG_NL);
            sl_bt_cbap_log_hexdump(&signed_device_oob_data[OOB_DATA_LEN],
                                   OOB_SIGNATURE_LEN);
            sl_bt_cbap_log_debug(APP_LOG_NL);

            cbap_central_state = SL_BT_CBAP_CENTRAL_GET_PERIPHERAL_OOB;
            sl_bt_cbap_central_on_event(cbap_central_state);
            set_timeout(true);
          }
          break;
        }

        case SL_BT_CBAP_CENTRAL_SEND_OOB: {
          sc = sl_bt_gatt_write_characteristic_value(connection,
                                                     cbap_characteristics[CHAR_CENTRAL_OOB].handle,
                                                     signed_device_oob_len,
                                                     signed_device_oob_data);
          app_assert_status(sc);

          // Request OOB data from both device
          sc = sl_bt_sm_configure(SL_BT_SM_CONFIGURATION_OOB_FROM_BOTH_DEVICES_REQUIRED,
                                  sl_bt_sm_io_capability_noinputnooutput);
          app_assert_status(sc);
          sc = sl_bt_sm_increase_security(connection);
          app_assert_status(sc);

          cbap_central_state = SL_BT_CBAP_CENTRAL_INCREASE_SECURITY;
          sl_bt_cbap_central_on_event(cbap_central_state);
          set_timeout(true);
          break;
        }

        default: {
          break;
        }
      }
      break;

    // -------------------------------
    // This event is generated when a characteristic value was received e.g. an indication
    case sl_bt_evt_gatt_characteristic_value_id:
      if (evt->data.evt_gatt_characteristic_value.connection != connection) {
        break;
      }

      if (cbap_central_state == SL_BT_CBAP_CENTRAL_GET_PERIPHERAL_CERT) {
        memcpy(&remote_certificate_der[remote_certificate_der_len],
               &evt->data.evt_gatt_characteristic_value.value.data[1],
               evt->data.evt_gatt_characteristic_value.value.len - 1);
        remote_certificate_der_len += evt->data.evt_gatt_characteristic_value.value.len - 1;
        sc = sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);
        app_assert_status(sc);
        if (evt->data.evt_gatt_characteristic_value.value.data[0] == 0) {
          // Last chunk stop indication
          sc = sl_bt_gatt_set_characteristic_notification(connection,
                                                          cbap_characteristics[CHAR_PERIPHERAL_CERT].handle,
                                                          sl_bt_gatt_disable);
          app_assert_status(sc);

          remote_cert_arrived = true;
          cbap_central_state = SL_BT_CBAP_CENTRAL_SEND_CENTRAL_CERT;
          sl_bt_cbap_central_on_event(cbap_central_state);
          set_timeout(true);

          sc = sl_bt_cbap_lib_process_remote_cert(remote_certificate_der,
                                                  remote_certificate_der_len);
          if (sc == SL_STATUS_OK) {
            sl_bt_cbap_log_info("Remote certificate verified." APP_LOG_NL);
          } else {
            sl_bt_cbap_log_error("Remote certificate verification failed. " \
                                 "Disconnecting." APP_LOG_NL);
            sl_bt_on_cbap_error();
            cbap_reset();
            break;
          }
        }
      } else if (cbap_central_state == SL_BT_CBAP_CENTRAL_GET_PERIPHERAL_OOB) {
        aes_key_128 remote_random;
        aes_key_128 remote_confirm;
        uint8_t remote_oob_signature[OOB_SIGNATURE_LEN];
        memcpy(&remote_random,
               &evt->data.evt_gatt_characteristic_value.value.data[0],
               sizeof(aes_key_128));
        memcpy(&remote_confirm,
               &evt->data.evt_gatt_characteristic_value.value.data[16],
               sizeof(aes_key_128));
        memcpy(&remote_oob_signature,
               &evt->data.evt_gatt_server_user_write_request.value.data[32],
               OOB_SIGNATURE_LEN);
        sc = sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);
        app_assert_status(sc);
        cbap_central_state = SL_BT_CBAP_CENTRAL_SEND_OOB;
        sl_bt_cbap_central_on_event(cbap_central_state);
        set_timeout(true);
        sc = sl_bt_gatt_set_characteristic_notification(connection,
                                                        cbap_characteristics[CHAR_PERIPHERAL_OOB].handle,
                                                        sl_bt_gatt_disable);
        app_assert_status(sc);

        sl_bt_cbap_log_debug("Remote OOB data:" APP_LOG_NL);
        sl_bt_cbap_log_hexdump(&remote_random, sizeof(aes_key_128));
        sl_bt_cbap_log_debug(APP_LOG_NL);
        sl_bt_cbap_log_hexdump(&remote_confirm, sizeof(aes_key_128));
        sl_bt_cbap_log_debug(APP_LOG_NL);
        sl_bt_cbap_log_debug("Remote OOB signature:" APP_LOG_NL);
        sl_bt_cbap_log_hexdump(&remote_oob_signature, OOB_SIGNATURE_LEN);
        sl_bt_cbap_log_debug(APP_LOG_NL);

        sc = sl_bt_cbap_lib_verify_remote_oob_data(remote_random.data,
                                                   remote_confirm.data,
                                                   remote_oob_signature);
        app_assert_status(sc);
        sl_bt_cbap_log_info("Remote OOB data verified." APP_LOG_NL);
        sc = sl_bt_sm_set_remote_oob(1, remote_random, remote_confirm);
        app_assert_status(sc);
        sc = sl_bt_cbap_destroy_key();
        app_assert_status(sc);
      }
      break;
  }
}

/***************************************************************************//**
 * Reset CBAP process states, flags and timers.
 ******************************************************************************/
static void cbap_reset(void)
{
  set_timeout(false); // Make sure timer is stopped
  connection = SL_BT_INVALID_CONNECTION_HANDLE; // Clear connection handle
  // Reset states
  cbap_peripheral_state = (sl_bt_cbap_peripheral_state_t)0;
  sl_bt_cbap_peripheral_on_event(cbap_peripheral_state);
  cbap_central_state = (sl_bt_cbap_central_state_t)0;
  sl_bt_cbap_central_on_event(cbap_central_state);
  char_state = (characteristics_t)0;
  // Reset flags
  remote_cert_arrived = false;
  device_cert_sent = false;
  remote_certificate_der_len = 0;
  dev_cert_sending_progression = 0;
}

/*******************************************************************************
 * Search for a Service UUID in scan report.
 *
 * @param[in] scan_data Data received in scanner advertisement report event
 * @param[in] scan_data_len Length of the scan data
 * @param[in] uuid Service UUID to search for
 * @param[in] uuid_len Service UUID length
 * @return true if the service is found
 ******************************************************************************/
static bool find_service_in_advertisement(const uint8_t *scan_data,
                                          uint8_t scan_data_len,
                                          const uint8_t *uuid,
                                          uint8_t uuid_len)
{
  uint8_t ad_field_length;
  uint8_t ad_field_type;
  uint8_t i = 0;

  while (i < scan_data_len) {
    // Parse advertisement packet
    ad_field_length = scan_data[i];  // Not counting the length byte itself
    ad_field_type = scan_data[i + 1];
    if ((uuid_len == UUID_16_LEN && (ad_field_type == GAP_INCOMPLETE_16B_UUID
                                     || ad_field_type == GAP_COMPLETE_16B_UUID))
        || (uuid_len == UUID_128_LEN && (ad_field_type == GAP_INCOMPLETE_128B_UUID
                                         || ad_field_type == GAP_COMPLETE_128B_UUID))) {
      // Packet containing the list of complete/incomplete 16/128-bit services found.
      // Loop through the UUID list
      uint8_t j = 2;
      while (j < ad_field_length + 1) {
        // Compare payload.
        if (memcmp(&scan_data[i + j], uuid, uuid_len) == 0) {
          return true;
        }
        // Advance to the next UUID
        j += uuid_len;
      }
    }
    // Advance to the next packet
    i += ad_field_length + 1;
  }
  return false;
}

/***************************************************************************//**
 * Start or stop timer for timeout check.
 * @param[in] activate If true timer will start for timeout check
 ******************************************************************************/
static void set_timeout(bool activate)
{
  sl_status_t sc;
  if (activate) {
    // Start or restart timer to timeout check
    sc = sl_simple_timer_start(&state_timer,
                               TIMEOUT,
                               state_timer_cb,
                               NO_CALLBACK_DATA,
                               false);
    app_assert_status(sc);
  } else {
    // Stop timer
    sc = sl_simple_timer_stop(&state_timer);
    app_assert_status(sc);
  }
}

/***************************************************************************//**
 * Timer Callback.
 * @param[in] handle pointer to handle instance
 * @param[in] data pointer to input data
 ******************************************************************************/
static void state_timer_cb(sl_simple_timer_t *handle, void *data)
{
  (void)handle;
  (void)data;

  sl_bt_cbap_log_error("Timeout error. Disconnecting." APP_LOG_NL);
  sl_bt_on_cbap_error();
  cbap_reset();
}

// CBAP Peripheral event handler WEAK implementation.
SL_WEAK void sl_bt_cbap_peripheral_on_event(sl_bt_cbap_peripheral_state_t status)
{
  (void)status;
}

// CBAP Central event handler WEAK implementation.
SL_WEAK void sl_bt_cbap_central_on_event(sl_bt_cbap_central_state_t status)
{
  (void)status;
}

// Callback to handle CBAP process errors.
SL_WEAK void sl_bt_on_cbap_error(void)
{
  sl_status_t sc;
  sc = sl_bt_connection_close(connection);
  app_assert_status(sc);
}
