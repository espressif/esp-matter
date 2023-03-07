/***************************************************************************//**
 * @file
 * @brief Voice NCP-host Example.
 *
 * This is the client of Voice over Bluetooth Low Energy (VoBLE) application
 * that demonstrates Bluetooth connectivity using BGAPI and handling audio
 * streaming. The example enables Bluetooth advertisements and configures the
 * following VoBLE parameters:
 * 1. Sample Rate,
 * 2. Enable notification for Audio Data Characteristic
 * 3. Enable/Disable filtering
 * 4. Enable/Disable encoding
 * 5. Enable/Disable audio data streaming status
 * After initialization and establishing connection application waits for audio
 * data transmission and finally stores received data into the file or sends it
 * to stdout.
 * Most of the functionality in BGAPI uses a request-response-event pattern
 * where the module responds to a command with a command response indicating
 * it has processed the request and then later sending an event indicating
 * the requested operation has been completed.
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
#include <stdbool.h>
#include <errno.h>
#include "system.h"
#include "sl_bt_api.h"
#include "sl_bt_ncp_host.h"
#include "app_log.h"
#include "app_assert.h"
#include "uart.h"
#include "app.h"

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "config.h"
#include "scan.h"
#include "ble-callbacks.h"

#include "parse.h"

#ifndef POSIX
#include <windows.h>
#endif

// connection parameters
#define CONNECTION_HANDLE_INVALID     ((uint8_t)0xFFu)
#define SERVICE_HANDLE_INVALID        ((uint32_t)0xFFFFFFFFu)
#define CHARACTERISTIC_HANDLE_INVALID ((uint16_t)0xFFFFu)

//  Setup state machine
typedef enum {
  initializing,
  scanning,
  opening,
  discover_services,
  discover_characteristics,
  enable_indication,
  set_adc_sample_rate,
  set_filtering,
  set_encoding,
  transfer_status,
  running
} conn_state_t;

// Characteristic UUID iterator
typedef enum {
  char_audio_data = 0,
  char_sample_rate,
  char_filter,
  char_encoding,
  char_transfer,
  char_nonexistent
} char_db_index_t;

#define CHAR_UUID_LEN (16)
#define CHAR_N (16)

// Characteristic handles
static uint16_t char_handle[CHAR_N];

// Local function declarations
static int serial_port_init(int argc, char* argv[], int32_t timeout);
static void uart_tx_wrapper(uint32_t len, uint8_t* data);
static void initalise_characteristic_handles(void);
static float timediff_sec_msec(struct timeval t0, struct timeval t1);
static void init(int argc, char* argv[]);
static void connect_remote(bd_addr remote_address);
static int32_t uart_rx_wrapper(uint32_t len, uint8_t* data);
static int32_t uart_peek_wrapper(void);

// Bluetooth connection
static uint8_t ble_connection;
static uint16_t audio_packetCnt = 0;
static bool transfer_started = false;
struct timeval current_time, start_time;

#ifdef POSIX
int32_t handle = -1;
void *handle_ptr = &handle;
#else
HANDLE serial_handle;
void *handle_ptr = &serial_handle;
#endif

/***********************************************************************************************//**
 *  \brief  Save audio data to file
 *  \param[in] evt Event pointer.
 *  \return None.
 **************************************************************************************************/
void save_audio_data_to_file(sl_bt_msg_t *evt)
{
  if (!transfer_started) {
    transfer_started = true;
    gettimeofday(&start_time, NULL);
  } else {
    gettimeofday(&current_time, NULL);
  }

  DEBUG_INFO("%.4f[s] - %d packet. Data length: %d [bytes]\n", timediff_sec_msec(start_time, current_time), audio_packetCnt++, evt->data.evt_gatt_characteristic_value.value.len);

  FILE *fd;
  if ( (fd = fopen(CONF_get()->out_file_name, "ab+")) != NULL ) {
    fwrite(evt->data.evt_gatt_characteristic_value.value.data, sizeof(uint8_t), evt->data.evt_gatt_characteristic_value.value.len, fd);
    fclose(fd);
  } else {
    DEBUG_ERROR("File %s can not be open.\n", CONF_get()->out_file_name);
    sl_status_t sc = sl_bt_connection_close(ble_connection);
    app_assert(sc == SL_STATUS_OK,
               "[E: 0x%04x] Failed to close connection\n",
               (int)sc);
  }
}

/***********************************************************************************************//**
 *  \brief  Save audio data to standard output
 *  \param[in] evt Event pointer.
 *  \return None.
 **************************************************************************************************/
void save_audio_data_to_stdout(sl_bt_msg_t *evt)
{
  fwrite(evt->data.evt_gatt_characteristic_value.value.data, sizeof(uint8_t), evt->data.evt_gatt_characteristic_value.value.len, stdout);
}

/**************************************************************************//**
 * @brief
 *   Function to Read and Cache Bluetooth Address.
 * @param address_type_out [out]
 *   A pointer to the outgoing address_type. This pointer can be NULL.
 * @return
 *   Pointer to the cached Bluetooth Address
 *****************************************************************************/
static bd_addr *read_and_cache_bluetooth_address(uint8_t *address_type_out)
{
  static bd_addr address;
  static uint8_t address_type;
  static bool cached = false;

  if (!cached) {
    sl_status_t sc = sl_bt_system_get_identity_address(&address, &address_type);
    app_assert(sc == SL_STATUS_OK,
               "[E: 0x%04x] Failed to get Bluetooth address\n",
               (int)sc);
    cached = true;
  }

  if (address_type_out) {
    *address_type_out = address_type;
  }

  return &address;
}

/**************************************************************************//**
 * @brief
 *   Function to Print Bluetooth Address.
 * @return
 *   None
 *****************************************************************************/
static void print_bluetooth_address(void)
{
  uint8_t address_type;
  bd_addr *address = read_and_cache_bluetooth_address(&address_type);

  app_log("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
          address_type ? "static random" : "public device",
          address->addr[5],
          address->addr[4],
          address->addr[3],
          address->addr[2],
          address->addr[1],
          address->addr[0]);
}

/**************************************************************************//**
 * @brief
 *    Application Init.
 * @param argc [in]
 *    Argument count
 * @param argv [in]
 *    Argument vector
 * @return
 *   None
 *****************************************************************************/
void app_init(int argc, char* argv[])
{
  // Initialize SL_BT_API with our output function for sending messages.
  sl_status_t sc = sl_bt_api_initialize_nonblock(uart_tx_wrapper, uart_rx_wrapper, uart_peek_wrapper);
  app_assert(sc == SL_STATUS_OK,
             "[E: 0x%04x] Failed to init Bluetooth NCP\n",
             (int)sc);

  // Calling application specific init
  init(argc, argv);

  // Initialise serial communication as non-blocking.
  if (serial_port_init(argc, argv, 100) < 0) {
    app_log("Non-blocking serial port init failure\n");
    exit(EXIT_FAILURE);
  }

  app_log("Voice NCP-host initialised\n");
  app_log("Resetting NCP target...\n");

  // Reset NCP to ensure it gets into a defined state.
  // Once the chip successfully boots, boot event should be received.
  sl_bt_system_reset(sl_bt_system_boot_mode_normal);

  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////

  initalise_characteristic_handles();
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void)
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
void sl_bt_on_event(sl_bt_msg_t* evt)
{
  sl_status_t sc;
  static volatile uint32_t service_handle = SERVICE_HANDLE_INVALID;

  const uint8_t service_uuid[] = SERVICE_VOICE_OVER_BLE_UUID;
  uint8_t service_uuid_len = sizeof(service_uuid) / sizeof(uint8_t);
  uint8_t char_iterator, uuid_iterator;
  uint8_t uuid_len;
  uint16_t result;

  // UUID database of characteristic
  static const uint8_t char_uuid_db[CHAR_N][CHAR_UUID_LEN] = { CHAR_AUDIO_DATA_UUID,
                                                               CHAR_SAMPLE_RATE_UUID,
                                                               CHAR_FILTER_ENABLE_UUID,
                                                               CHAR_ENCODING_ENABLE_UUID,
                                                               CHAR_TRANSFER_STATUS_UUID };

  // State of the connection under establishment
  static conn_state_t conn_state = initializing;

  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Print boot message.
      app_log("Bluetooth stack booted: v%d.%d.%d-b%d\n",
              evt->data.evt_system_boot.major,
              evt->data.evt_system_boot.minor,
              evt->data.evt_system_boot.patch,
              evt->data.evt_system_boot.build);
      // Print bluetooth address.
      print_bluetooth_address();

      uint16_t max_mtu_out = 0;
      sc = sl_bt_gatt_set_max_mtu(250, &max_mtu_out);
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to set max mtu\n",
                 (int)sc);
      DEBUG_SUCCESS("System booted.");

      if (CONF_get()->remote_address_set) {
        connect_remote(CONF_get()->remote_address);
      } else {
        DEBUG_INFO("Scanning for VoBLE devices...");
        sc = sl_bt_scanner_start(sl_bt_gap_phy_1m, sl_bt_scanner_discover_generic);
        app_assert(sc == SL_STATUS_OK,
                   "[E: 0x%04x] Failed to start discovery #1\n",
                   (int)sc);
      }
      conn_state = scanning;
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(0,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to generate advertising data\n",
                 (int)sc);

      /* Restart general advertising and re-enable connections after disconnection. */
      sc = sl_bt_legacy_advertiser_start(0,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to start advertising\n",
                 (int)sc);

      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      DEBUG_SUCCESS("Connected.");
      //  Discover BT service. This will trigger a gatt service event
      sc = sl_bt_gatt_discover_primary_services_by_uuid(evt->data.evt_connection_opened.connection,
                                                        service_uuid_len,
                                                        service_uuid);
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to discover primary services\n",
                 (int)sc);
      conn_state = discover_services;
      break;

    // -------------------------------
    // This event indicates that a GATT procedure completed.
    case sl_bt_evt_gatt_procedure_completed_id:
      result =  evt->data.evt_gatt_procedure_completed.result;

      if (!result) {
        DEBUG_SUCCESS("Success.");
      } else {
        DEBUG_ERROR("Error: %d\n", result);
      }

      // Discover VoBLE characteristic
      if (conn_state == discover_services && service_handle != SERVICE_HANDLE_INVALID) {
        app_log("Discovering characteristic...\n");
        sc = sl_bt_gatt_discover_characteristics(evt->data.evt_gatt_procedure_completed.connection, service_handle);

        app_assert(sc == SL_STATUS_OK,
                   "[E: 0x%04x] Failed to discover characteristics\n",
                   (int)sc);
        conn_state = discover_characteristics;
        break;
      }

      // If characteristic discovery finished. Enable audio data notifications
      if (conn_state == discover_characteristics && char_handle[char_audio_data] != CHARACTERISTIC_HANDLE_INVALID) {
        app_log("Enabling voice data flow.\n");
        // stop discovering
        sl_bt_scanner_stop();
        sc = sl_bt_gatt_set_characteristic_notification(evt->data.evt_gatt_procedure_completed.connection,
                                                        char_handle[char_audio_data],
                                                        sl_bt_gatt_notification);
        app_assert(sc == SL_STATUS_OK,
                   "[E: 0x%04x] Failed to set notification for characteristic\n",
                   (int)sc);
        conn_state = enable_indication;
        break;
      }

      //  Set ADC sample rate
      if (conn_state == enable_indication && char_handle[char_sample_rate] != CHARACTERISTIC_HANDLE_INVALID) {
        app_log("Setting ADC sample rate.\n");
        uint8_t sr = CONF_get()->adc_sample_rate;
        sc = sl_bt_gatt_write_characteristic_value(evt->data.evt_gatt_procedure_completed.connection,
                                                   char_handle[char_sample_rate], 1,
                                                   &sr);
        app_assert(sc == SL_STATUS_OK,
                   "[E: 0x%04x] Failed to set sample rate\n",
                   (int)sc);
        conn_state = set_adc_sample_rate;
        break;
      }

      //  Set filtering
      if (conn_state == set_adc_sample_rate && char_handle[char_filter] != CHARACTERISTIC_HANDLE_INVALID) {
        app_log("Setting filtering.\n");
        uint8_t filt = CONF_get()->filter_enabled;
        sc = sl_bt_gatt_write_characteristic_value(evt->data.evt_gatt_procedure_completed.connection,
                                                   char_handle[char_filter], 1, /*This going to be changed*/
                                                   &filt);
        app_assert(sc == SL_STATUS_OK,
                   "[E: 0x%04x] Failed to set filtering\n",
                   (int)sc);
        conn_state = set_filtering;
        break;
      }

      //  Enable encoding
      if (conn_state == set_filtering && char_handle[char_encoding] != CHARACTERISTIC_HANDLE_INVALID) {
        app_log("Setting encoding.\n");
        uint8_t filt = CONF_get()->encoding_enabled;
        sc = sl_bt_gatt_write_characteristic_value(evt->data.evt_gatt_procedure_completed.connection,
                                                   char_handle[char_encoding], 1, /*This going to be changed*/
                                                   &filt);
        app_assert(sc == SL_STATUS_OK,
                   "[E: 0x%04x] Failed to set encoding\n",
                   (int)sc);
        conn_state = set_encoding;
        break;
      }

      //  Transfer status
      if (conn_state == set_encoding && char_handle[char_transfer] != CHARACTERISTIC_HANDLE_INVALID) {
        app_log("Setting transfer status.\n");
        uint8_t txfer_flags = sl_bt_gatt_disable;
        if (CONF_get()->transfer_status) {
          txfer_flags = sl_bt_gatt_notification;
        }
        sc = sl_bt_gatt_set_characteristic_notification(ble_connection, char_handle[char_transfer], txfer_flags);
        app_assert(sc == SL_STATUS_OK,
                   "[E: 0x%04x] Failed to set transfer status\n",
                   (int)sc);
        conn_state = transfer_status;
        break;
      }

      break;

    // -------------------------------
    // This event is triggered when a new service is discovered
    case sl_bt_evt_gatt_service_id:
      DEBUG_SUCCESS("New Service discovered.");
      //  Save the service handle
      service_handle = evt->data.evt_gatt_service.service;
      break;

    // -------------------------------
    // This event is triggered when a new characteristic is discovered
    case sl_bt_evt_gatt_characteristic_id:
      // Compare the UUID with the local data

      //  Select the smallest UUID length
      if (evt->data.evt_gatt_characteristic.uuid.len > CHAR_UUID_LEN) {
        uuid_len = CHAR_UUID_LEN;
      } else {
        uuid_len = evt->data.evt_gatt_characteristic.uuid.len;
      }

      //  Look up the UUID
      for (char_iterator = 0; char_iterator < char_nonexistent; char_iterator++) {
        for (uuid_iterator = 0; uuid_iterator < uuid_len; uuid_iterator++ ) {
          if (evt->data.evt_gatt_characteristic.uuid.data[uuid_iterator] != char_uuid_db[char_iterator][uuid_iterator]) {
            break;
          }
        }
        if (uuid_iterator == uuid_len ) {
          /*Found it!*/
          char_handle[char_iterator] = evt->data.evt_gatt_characteristic.characteristic;
          break;
        }
      }

      break;

    // -------------------------------
    // This event is triggered when a new characteristic value received
    case sl_bt_evt_gatt_characteristic_value_id:
      if (evt->data.evt_gatt_characteristic_value.characteristic == char_handle[char_audio_data]) {
        if ( CONF_get()->output_to_stdout == true ) {
          save_audio_data_to_stdout(evt);
        } else {
          save_audio_data_to_file(evt);
        }
      } else if (evt->data.evt_gatt_characteristic_value.characteristic == char_handle[char_transfer]) {
        FILE *fd;
        if ( (fd = fopen(CONF_get()->transfer_status_file_name, "w")) != NULL ) {
          fwrite(evt->data.evt_gatt_characteristic_value.value.data, sizeof(uint8_t), evt->data.evt_gatt_characteristic_value.value.len, fd);
          fclose(fd);
        } else {
          DEBUG_ERROR("File %s can not be opened.\n", CONF_get()->transfer_status_file_name);
          sc = sl_bt_connection_close(ble_connection);
          app_assert(sc == SL_STATUS_OK,
                     "[E: 0x%04x] Failed to close connection\n",
                     (int)sc);
        }
      }
      break;

    // -------------------------------
    // This event is triggered when a scan report received
    case sl_bt_evt_scanner_legacy_advertisement_report_id:
      SCAN_Process_scan_response(evt);

      if (SCAN_Is_Device_Found()) {
        connect_remote(CONF_get()->remote_address);
      }

      conn_state = opening;
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

/**************************************************************************//**
 * @brief
 *    UART TX Wrapper.
 * @param len [in]
 *    Length of the data
 * @param data [in]
 *    pointer to data buffer to send
 * @return
 *    None
 *****************************************************************************/
static void uart_tx_wrapper(uint32_t len, uint8_t* data)
{
  /** Variable for storing function return values. */
  int32_t ret;

  ret = uartTx(handle_ptr, len, data);
  if (ret < 0) {
    app_log("Failed to write to serial port %s, ret: %d, errno: %d\n", CONF_get()->uart_port, ret, errno);
    exit(EXIT_FAILURE);
  }
}

/**************************************************************************//**
 * Initialise serial port.
 *****************************************************************************/
/**************************************************************************//**
 * @brief
 *    Initialise serial port.
 * @param argc [in]
 *    Argument count
 * @param argv [in]
 *    Argument vector
 * @param timout [in]
 *    Timeout
 * @return
 *    Status
 *****************************************************************************/
static int serial_port_init(int argc, char* argv[], int32_t timeout)
{
  (void)argc;
  int ret;

  if (!CONF_get()->uart_port || !CONF_get()->baud_rate) {
    help();
    exit(EXIT_FAILURE);
  }

  /* Initialise the serial port with RTS/CTS enabled. */
  ret = uartOpen(handle_ptr,
                 (int8_t*)CONF_get()->uart_port,
                 CONF_get()->baud_rate,
                 0,
                 timeout);
  if (ret >= 0) {
    uartFlush(handle_ptr);
  }
  return ret;
}

static void init(int argc, char* argv[])
{
  /* Additional UART port initialization required due to BGAPI communication.*/
  CONF_get()->uart_port = NULL;

  /* Parsing list of arguments */
  PAR_parse(argc, argv);
}

/***********************************************************************************************//**
 *  \brief  Establish connection to remote device
 *  \param[in]  remote_address Remote bluetooth device address
 **************************************************************************************************/
static void connect_remote(bd_addr remote_address)
{
  sl_status_t sc;

  //set default connection parameters
  sc = sl_bt_connection_set_default_parameters(6, 6, 0, 300, 0, 0xFFFF);
  if (sc != SL_STATUS_OK) {
    ERROR_EXIT("Error, set connection parameters failed,%x", sc);
  }

  //move to connect state, connect to device address
  sc = sl_bt_connection_open(remote_address, /* le_gap_address_type_public = */ 0, sl_bt_gap_phy_1m, &ble_connection);
  if (sc != SL_STATUS_OK) {
    ERROR_EXIT("Error, open failed,%x", sc);
  }
  DEBUG_INFO("Connecting...\n");
}

/***********************************************************************************************//**
 *  \brief  Calculate difference between two time stamps and return elapsed time.
 *  \param[in]  t0 - first time stamp
 *  \param[in]  t1 - second time stamp
 *  \return elapsed time or 0 if elapsed time is negative
 **************************************************************************************************/
static float timediff_sec_msec(struct timeval t0, struct timeval t1)
{
  float elapsed_time = ((t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f) / 1000;
  return (elapsed_time < 0) ? (float)0 : elapsed_time;
}

/**************************************************************************//**
 * Initialise characteristic handles.
 *****************************************************************************/
static void initalise_characteristic_handles(void)
{
  uint8_t characteristic_iterator;

  for (characteristic_iterator = 0; characteristic_iterator < CHAR_N; characteristic_iterator++ ) {
    char_handle[characteristic_iterator] = CHARACTERISTIC_HANDLE_INVALID;
  }
}

static inline int32_t uart_rx_wrapper(uint32_t len, uint8_t* data)
{
  return uartRx(handle_ptr, len, data);
}

static inline int32_t uart_peek_wrapper(void)
{
  return uartRxPeek(handle_ptr);
}
