/***************************************************************************//**
 * @file
 * @brief Bluetotoh over-the-air (OTA) device firmware update (DFU) example
 *
 * Bluetotoh over-the-air (OTA) device firmware update (DFU) example for
 * x86 host using Network Co-Processor (NCP)
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

/**
 * This an example application that demonstrates how to make a Bluetooth Over-the-Air (OTA)
 * firmware update.
 *
 * To use this application you must have a WSTK configured into NCP mode connected to your
 * PC and it is used as a Bluetooth radio to push the OTA update to a remote device.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "sl_bt_api.h"
#include "sl_bt_ncp_host.h"
#include "uart.h"
#include "app_log.h"
#include "app_assert.h"

#ifndef POSIX
#include <windows.h>
#endif

/**
 * Configurable parameters that can be modified to match the test setup.
 */

/** The serial port to use for BGAPI communication. */
static char* uart_port = NULL;

/** The baud rate to use. */
static uint32_t baud_rate = 0;

/** Usage string */
#define USAGE "Usage: %s [serial port] [baud rate] [ota file] [remote address] [(optional) force write without response: 0 / 1] [optional max mtu]\n\n"
/**/
/** dfu file to upload*/
FILE *dfu_file = NULL;
/** remote device address*/
static uint8_t remote_address_type;
static bd_addr remote_address;
static bd_addr remote_public_address;
static uint8_t addr_found = 0;
/** Force using write without response commands */
static uint32_t force_write_without_rsp = 0;
/*Bluetooth connection*/
uint8_t ble_connection;
/*found OTA descriptors*/
uint32_t ota_gatt_service_handle;
uint16_t ota_control_characteristic;
uint16_t ota_data_characteristic;
uint16_t ota_version_characteristic;
uint16_t apploader_version_characteristic;
uint16_t bootloader_version_characteristic;
uint16_t application_version_characteristic;
/*OTA UUIDS*/
uint8_t uuid_ota_service[] = { 0xf0, 0x19, 0x21, 0xb4, 0x47, 0x8f, 0xa4, 0xbf, 0xa1, 0x4f, 0x63, 0xfd, 0xee, 0xd6, 0x14, 0x1d };//1d14d6ee-fd63-4fa1-bfa4-8f47b42119f0
uint8_t uuid_ota_control[] = { 0x63, 0x60, 0x32, 0xe0, 0x37, 0x5e, 0xa4, 0x88, 0x53, 0x4e, 0x6d, 0xfb, 0x64, 0x35, 0xbf, 0xf7 };//f7bf3564-fb6d-4e53-88a4-5e37e0326063
uint8_t uuid_ota_data[] = { 0x53, 0xa1, 0x81, 0x1f, 0x58, 0x2c, 0xd0, 0xa5, 0x45, 0x40, 0xfc, 0x34, 0xf3, 0x27, 0x42, 0x98 };//984227f3-34fc-4045-a5d0-2c581f81a153
uint8_t uuid_bootloader_version[] = { 0xfe, 0x5a, 0x24, 0xe1, 0x2b, 0xaa, 0xa5, 0xb2, 0xe9, 0x46, 0x17, 0xe9, 0x0a, 0x5c, 0xf0, 0x25 };//25f05c0a-e917-46e9-b2a5-aa2be1245afe
uint8_t uuid_apploader_version[] = { 0x9f, 0x3e, 0xe2, 0x2e, 0x0e, 0xcf, 0xff, 0xbf, 0x1e, 0x45, 0xca, 0x8c, 0x68, 0x23, 0x4a, 0x4f };//4f4a2368-8cca-451e-bfff-cf0e2ee23e9f
uint8_t uuid_ota_version[] = { 0x16, 0x53, 0x1e, 0xc4, 0x4c, 0xba, 0xad, 0x9d, 0x32, 0x4b, 0x68, 0x08, 0xcf, 0x7b, 0xc0, 0x4c };//4cc07bcf-0868-4b32-9dad-ba4cc41e5316
uint8_t uuid_application_version[] = { 0xf8, 0x92, 0x7a, 0xac, 0x96, 0xcd, 0xa9, 0xbf, 0xf2, 0x49, 0xc1, 0x4a, 0x11, 0xcc, 0x77, 0x0d };//0d77cc11-4ac1-49f2-bfa9-cd96ac7a92f8

/*Error macro*/
#define ERROR_EXIT(...) do { app_log(__VA_ARGS__); exit(EXIT_FAILURE); } while (0)

#define GAP_ADDR_TYPE 0x1b

/*
   Enumeration of possible OTA states
 */
enum ota_states {
  OTA_INIT,   // default state
  OTA_CONNECT,  // connect to remote device
  OTA_FIND_SERVICES,  // find OTA control&data attributes
  OTA_FIND_CHARACTERISTICS,  // find OTA control&data attributes
  OTA_READ_OTA_DATA_PROPERTIES,  // Read OTA Data characteristic properties
  OTA_BEGIN,  // Begin OTA process
  OTA_UPLOAD_WITHOUT_RSP,  // Upload data to remote device using write without response
  OTA_UPLOAD_WITH_RSP,  // Upload data to remote device using write with response
  OTA_END,  // End current OTA process
  OTA_RESTART,  // End current OTA process
  OTA_RESET_TO_DFU,  // Reset to DFU mode
  OTA_SCAN, // Scan for device
  OTA_READ_BOOTLOADER_VERSION, //Read bootloader version
  OTA_READ_APPLOADER_VERSION, //Read apploader version
  OTA_READ_OTA_VERSION, //Read OTA version
  OTA_READ_APPLICATION_VERSION, //Read application version
} ota_state = OTA_INIT;
void ota_change_state(enum ota_states new_state);
static int32_t uart_rx_wrapper(uint32_t len, uint8_t* data);
static int32_t uart_peek_wrapper(void);

///DFU
#define MAX_DFU_PACKET 256
uint8_t dfu_data[MAX_DFU_PACKET];
bool dfu_resync = false;
size_t dfu_toload = 0;
size_t dfu_total = 0;
size_t dfu_current_pos = 0;
time_t dfu_start_time;

#define MAX_MTU 247
#define MIN_MTU 23
uint32_t mtu = MIN_MTU;
uint32_t max_mtu = MAX_MTU;
uint8_t ota_data_properties = 0;
uint32_t bootloader_version = -1;
uint32_t application_version = -1;
uint16_t apploader_version[4] = { 0 };
uint8_t ota_version = 0;
//
#ifdef POSIX
int32_t handle = -1;
void *handle_ptr = &handle;
#else
HANDLE serial_handle;
void *handle_ptr = &serial_handle;
#endif

int dfu_read_size()
{
  if (fseek(dfu_file, 0L, SEEK_END)) {
    return -1;
  }
  dfu_total = dfu_toload = ftell(dfu_file);
  if (fseek(dfu_file, 0L, SEEK_SET)) {
    return -1;
  }
  app_log("Bytes to send:%d\n", (int)dfu_toload);
  return 0;
}

void send_dfu_block()
{
  time_t ti;
  size_t dfu_size;
  sl_bt_msg_t evt;

  while (dfu_toload > 0) {
    sl_status_t sc;
    uint16_t sent_len;

    dfu_size = dfu_toload > (mtu - 3) ? (mtu - 3) : dfu_toload;
    if (fread(dfu_data, 1, dfu_size, dfu_file) != dfu_size) {
      app_log("File read failure\n");
      exit(-1);
    }

    do {
      sc = sl_bt_pop_event(&evt);
      sc = sl_bt_gatt_write_characteristic_value_without_response(ble_connection, ota_data_characteristic, dfu_size, dfu_data, &sent_len);
    } while (sc != SL_STATUS_OK);

    if (sc) {
      ERROR_EXIT("Error,%s, characteristic write failed:0x%x", __FUNCTION__, sc);
    }

    dfu_current_pos += dfu_size;
    dfu_toload -= dfu_size;

    ti = time(NULL);
    if (ti != dfu_start_time && dfu_total > 0) {
      app_log("\r%d%% %.2fkbit/s                   ",
              (int)(100 * dfu_current_pos / dfu_total),
              dfu_current_pos * 8.0 / 1000.0 / difftime(ti, dfu_start_time));
    }
  }

  app_log("\n");
  app_log("time: %.2fs", difftime(ti, dfu_start_time));

  app_log("\n");
  ota_change_state(OTA_END);
}

void send_dfu_packet_with_confirmation()
{
  time_t ti;
  size_t dfu_size;

  if (dfu_toload > 0) {
    sl_status_t sc;

    dfu_size = dfu_toload > (mtu - 3) ? (mtu - 3) : dfu_toload;
    if (fread(dfu_data, 1, dfu_size, dfu_file) != dfu_size) {
      app_log("File read failure\n");
      exit(-1);
    }

    sc = sl_bt_gatt_write_characteristic_value(ble_connection, ota_data_characteristic, dfu_size, dfu_data);

    if (sc) {
      ERROR_EXIT("Error,%s, characteristic write failed:0x%x", __FUNCTION__, sc);
    }

    dfu_current_pos += dfu_size;
    dfu_toload -= dfu_size;

    ti = time(NULL);
    if (ti != dfu_start_time && dfu_total > 0) {
      app_log("\r%d%% %.2fkbit/s                   ",
              (int)(100 * dfu_current_pos / dfu_total),
              dfu_current_pos * 8.0 / 1000.0 / difftime(ti, dfu_start_time));
    }
  } else {
    ti = time(NULL);
    app_log("\n");
    app_log("time: %.2fs", difftime(ti, dfu_start_time));

    app_log("\n");
    ota_change_state(OTA_END);
  }
}

/*
 */
void sync_boot()
{
  sl_status_t sc;
  sl_bt_msg_t evt;

  // Reset NCP to ensure it gets into a defined state.
  // Once the chip successfully boots, boot event should be received.
  sl_bt_system_reset(0);
  do {
    sc = sl_bt_pop_event(&evt);
    if (!sc) {
      switch (SL_BT_MSG_ID(evt.header)) {
        case sl_bt_evt_system_boot_id:
          app_log("System rebooted\n");
          sl_bt_evt_system_boot_t *p = \
            &evt.data.evt_system_boot;
          app_log("NCP version: v%d.%d.%d-b%d\n",
                  p->major,
                  p->minor,
                  p->patch,
                  p->build);
          return;
      }
    }
  } while (1);
}
/*

 */
void ota_change_state(enum ota_states new_state)
{
  ota_state = new_state;
  switch (ota_state) {
    case OTA_END:
    {
      app_log("Finishing DFU block...");
      {
        sl_status_t sc;
        sc = sl_bt_gatt_write_characteristic_value(ble_connection, ota_control_characteristic, 1, (uint8_t*)"\x03");
        if (sc) {
          ERROR_EXIT("Error,%s, characteristic write failed,0x%x", __FUNCTION__, sc);
        }
      }
    }
    break;

    case OTA_UPLOAD_WITHOUT_RSP:
      send_dfu_block();
      break;

    case OTA_UPLOAD_WITH_RSP:
      send_dfu_packet_with_confirmation();
      break;

    case OTA_BEGIN:
    {
      sl_status_t sc;
      sc = sl_bt_gatt_write_characteristic_value(ble_connection, ota_control_characteristic, 1, (uint8_t*)"\0");
      if (sc) {
        ERROR_EXIT("Error,%s, characteristic write failed,0x%x", __FUNCTION__, sc);
      }
      app_log("DFU mode...");
      dfu_start_time = time(NULL);
    }
    break;

    case OTA_FIND_CHARACTERISTICS:
    {
      sl_status_t sc;
      sc = sl_bt_gatt_discover_characteristics(ble_connection, ota_gatt_service_handle);
      if (sc) {
        ERROR_EXIT("Error, characteristics discover failed,0x%x", sc);
      }
      app_log("Discovering characteristics...");
      ota_control_characteristic = 0xFFFF;
      ota_data_characteristic = 0xFFFF;
      bootloader_version_characteristic = 0xFFFF;
      apploader_version_characteristic = 0xFFFF;
      ota_version_characteristic = 0xFFFF;
    }
    break;

    case OTA_READ_OTA_DATA_PROPERTIES:
    {
      sl_status_t sc;
      sc = sl_bt_gatt_read_characteristic_value_from_offset(ble_connection, ota_data_characteristic - 1, 0, 1);
      if (sc) {
        ERROR_EXIT("Error,%s, characteristic read failed,0x%x", __FUNCTION__, sc);
      }
    }
    break;

    case OTA_FIND_SERVICES:
    {
      sl_status_t sc;
      sc = sl_bt_gatt_discover_primary_services_by_uuid(ble_connection, sizeof(uuid_ota_service), uuid_ota_service);
      if (sc) {
        ERROR_EXIT("Error, service discover failed,0x%x", sc);
      }
      app_log("Discovering services...");
      ota_gatt_service_handle = 0xFFFFFFFF;
    }
    break;

    case OTA_SCAN:
    {
      addr_found = 0;
      sl_bt_scanner_start(sl_bt_gap_phy_1m, sl_bt_scanner_discover_generic);
      app_log("Scanning...");
    }
    break;

    case OTA_CONNECT:
    {
      sl_status_t sc;
      uint16_t set_mtu;
      uint8_t connection;
      sc = sl_bt_gatt_set_max_mtu(max_mtu, &set_mtu);
      if (sc) {
        ERROR_EXIT("Error, set max MTU failed,0x%x", sc);
      }
      //set default connection parameters
      sc = sl_bt_connection_set_default_parameters(6, 6, 0, 300, 0, 0xffff);
      if (sc) {
        ERROR_EXIT("Error, set default connection parameters failed,0x%x", sc);
      }

      //move to connect state, connect to device address
      sc = sl_bt_connection_open(remote_address, remote_address_type, sl_bt_gap_phy_1m, &connection);
      if (sc) {
        ERROR_EXIT("Error, open failed,0x%x", sc);
      }
      ble_connection = connection;
      app_log("Connecting...");
    }
    break;

    case OTA_INIT:
      sync_boot();
      {
        sl_status_t sc;
        bd_addr address;
        uint8_t address_type;
        sc = sl_bt_system_get_identity_address(&address, &address_type);
        if (sc) {
          ERROR_EXIT("Error, failed to get Bluetooth address,0x%x", sc);
        }
        app_log("Local %s address: %02x:%02x:%02x:%02x:%02x:%02x\n",
                address_type ? "static random" : "public device",
                address.addr[5],
                address.addr[4],
                address.addr[3],
                address.addr[2],
                address.addr[1],
                address.addr[0]);
      }
      if (dfu_read_size()) {
        ERROR_EXIT("Error, DFU file read failed\n");
      }
      ota_change_state(OTA_SCAN);
      break;

    case OTA_RESET_TO_DFU:
    {
      sl_status_t sc;
      sc = sl_bt_gatt_write_characteristic_value(ble_connection, ota_control_characteristic, 1, (uint8_t*)"\0");
      if (sc) {
        ERROR_EXIT("Error,%s, characteristic write failed,0x%x", __FUNCTION__, sc);
      }
    }
    break;

    case OTA_READ_OTA_VERSION:
    {
      sl_status_t sc;
      sc = sl_bt_gatt_read_characteristic_value(ble_connection, ota_version_characteristic);
      if (sc) {
        ERROR_EXIT("Error,%s, characteristic write failed,0x%x", __FUNCTION__, sc);
      }
    }
    break;

    case OTA_READ_APPLOADER_VERSION:
    {
      sl_status_t sc;
      sc = sl_bt_gatt_read_characteristic_value(ble_connection, apploader_version_characteristic);
      if (sc) {
        ERROR_EXIT("Error,%s, characteristic write failed,0x%x", __FUNCTION__, sc);
      }
    }
    break;

    case OTA_READ_BOOTLOADER_VERSION:
    {
      sl_status_t sc;
      sc = sl_bt_gatt_read_characteristic_value(ble_connection, bootloader_version_characteristic);
      if (sc) {
        ERROR_EXIT("Error,%s, characteristic write failed,0x%x", __FUNCTION__, sc);
      }
    }
    break;

    case OTA_READ_APPLICATION_VERSION:
    {
      sl_status_t sc;
      sc = sl_bt_gatt_read_characteristic_value(ble_connection, application_version_characteristic);
      if (sc) {
        ERROR_EXIT("Error,%s, characteristic write failed,0x%x", __FUNCTION__, sc);
      }
    }
    break;

    default:
      break;
  }
}

/**
 * Function called when a message needs to be written to the serial port.
 * @param msg_len Length of the message.
 * @param msg_data Message data, including the header.
 * @param data_len Optional variable data length.
 * @param data Optional variable data.
 */
static void on_message_send(uint32_t msg_len, uint8_t* msg_data)
{
  /** Variable for storing function return values. */
  int ret;

#if DEBUG
  app_log("on_message_send()\n");
#endif /* DEBUG */

  ret = uartTx(handle_ptr, msg_len, msg_data);
  if (ret < 0) {
    app_log("on_message_send() - failed to write to serial port %s, ret: %d, errno: %d\n", uart_port, ret, errno);
    exit(EXIT_FAILURE);
  }
}

void print_address(bd_addr address)
{
  for (int i = 5; i >= 0; i--) {
    app_log("%02x", address.addr[i]);
    if (i > 0) {
      app_log(":");
    }
  }
}

static int parse_scan_data(uint8_t *data, uint8_t len, bd_addr *addr)
{
  uint8_t i = 0;
  while (i < len) {
    if (data[i + 1] == GAP_ADDR_TYPE) {
      memcpy(addr, &data[i + 3], sizeof(bd_addr));
      return 0;
    } else {
      i += data[i] + 1;
    }
  }

  return -1;
}

int parse_address(const char *str, bd_addr *addr)
{
  int a[6];
  int i;
  i = sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
             &a[5],
             &a[4],
             &a[3],
             &a[2],
             &a[1],
             &a[0]
             );
  if (i != 6) {
    return -1;
  }

  for (i = 0; i < 6; i++) {
    addr->addr[i] = (uint8_t)a[i];
  }

  return 0;
}
int hw_init(int argc, char* argv[])
{
  int ret = 0;
  if (argc < 5) {
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }
  /**
   * Handle the command-line arguments.
   */

  //filename
  dfu_file = fopen(argv[3], "rb");
  if (dfu_file == NULL) {
    app_log("cannot open file %s\n", argv[3]);
    exit(-1);
  }
  //baudrate
  baud_rate = atoi(argv[2]);
  //uart port
  uart_port = argv[1];
  //remote address
  if (parse_address(argv[4], &remote_public_address)) {
    app_log("Unable to parse address %s", argv[4]);
    exit(EXIT_FAILURE);
  }

  if (argc >= 6) {
    force_write_without_rsp = atoi(argv[5]);
  }

  if (argc == 7) {
    max_mtu = atoi(argv[6]);
    if (max_mtu > MAX_MTU) {
      max_mtu = MAX_MTU;
    } else if (max_mtu < MIN_MTU) {
      max_mtu = MIN_MTU;
    }
    app_log("MTU set to %u\n", max_mtu);
  }
  /**
   * Initialise the serial port.
   */
  ret = uartOpen(handle_ptr, (int8_t*)uart_port, baud_rate, 1, 100);
  if (ret >= 0) {
    uartFlush(handle_ptr);
  }
  return ret;
}

static void handle_scan_event(uint8_t *address,
                              uint8_t address_type,
                              uint8_t *data,
                              uint8_t data_len)
{
  if (!addr_found) {
    bd_addr addr;
    if (!memcmp(address, &remote_public_address, sizeof(bd_addr))) {
      memcpy(&remote_address, address, sizeof(bd_addr));
      remote_address_type = address_type;
      addr_found = 1;
    } else if (parse_scan_data(data, data_len, &addr) == 0) {
      if (!memcmp(&addr, &remote_public_address, sizeof(bd_addr))) {
        memcpy(&remote_address, address, sizeof(bd_addr));
        remote_address_type = address_type;
        addr_found = 1;
      }
    }
    if (addr_found) {
      sl_bt_scanner_stop();
      app_log("OK\n");
      app_log("Device address found, connecting.\n");
      ota_change_state(OTA_CONNECT);
    }
  }
}

/**
 * The main program.
 */
int main(int argc, char* argv[])
{
  sl_bt_msg_t evt;
  sl_bt_msg_t *p = &evt;
  /**
   * Initialize BGLIB with our output function for sending messages.
   */
  sl_status_t sc = sl_bt_api_initialize_nonblock(on_message_send, uart_rx_wrapper, uart_peek_wrapper);
  app_assert(sc == SL_STATUS_OK,
             "[E: 0x%04x] Failed to init Bluetooth NCP\n",
             (int)sc);

  if (hw_init(argc, argv) < 0) {
    app_log("HW init failure\n");
    exit(EXIT_FAILURE);
  }
  ota_change_state(OTA_INIT);
  while (1) {
    sl_bt_wait_event(&evt);

    if (p && SL_BT_MSG_ID(p->header) == sl_bt_evt_gatt_mtu_exchanged_id) {
      mtu = p->data.evt_gatt_mtu_exchanged.mtu;
      app_log("ATT MTU exchanged: %d\n", mtu);
      continue;
    }

    switch (ota_state) {
      case OTA_END:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_connection_closed_id:
            app_log("OK\n");
            exit(EXIT_SUCCESS);
            break;

          case sl_bt_evt_gatt_procedure_completed_id:
            if (p->data.evt_gatt_procedure_completed.result) {
              ERROR_EXIT("Error, OTA DFU failed,0x%x", p->data.evt_gatt_procedure_completed.result);
            }
            app_log("OK\n");
            app_log("Closing connection...");
            {
              sl_status_t sc;
              sc = sl_bt_connection_close(ble_connection);
              if (sc) {
                ERROR_EXIT("Error,%s, close failed,0x%x", __FUNCTION__, sc);
              }
            }
            break;

          default:

            break;
        }
        break;

      case OTA_UPLOAD_WITHOUT_RSP:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_gatt_procedure_completed_id:
            if (p->data.evt_gatt_procedure_completed.result) {
              ERROR_EXIT("procedure failed:0x%x\r\n", p->data.evt_gatt_procedure_completed.result);
            }
            send_dfu_block();
            break;

          case sl_bt_evt_connection_closed_id:
            ERROR_EXIT("\nError, Connection closed, reason 0x%x", p->data.evt_connection_closed.reason);
            break;

          default:
            break;
        }
        break;

      case OTA_UPLOAD_WITH_RSP:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_gatt_procedure_completed_id:
            if (p->data.evt_gatt_procedure_completed.result) {
              ERROR_EXIT("procedure failed:0x%x\r\n", p->data.evt_gatt_procedure_completed.result);
            }
            send_dfu_packet_with_confirmation();
            break;

          case sl_bt_evt_connection_closed_id:
            ERROR_EXIT("\nError, Connection closed, reason 0x%x", p->data.evt_connection_closed.reason);
            break;

          default:
            break;
        }
        break;

      case OTA_BEGIN:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_gatt_procedure_completed_id:
            app_log("OK\n");
            if ((ota_data_properties & 0x0C) == 0) {
              ERROR_EXIT("Wrong supported OTA Data properties\r\n");
            } else {
              if (force_write_without_rsp == 1 && ota_data_properties & 0x04) {     //Write without response is supported and forced
                app_log("OTA DFU - write without response \n");
                ota_change_state(OTA_UPLOAD_WITHOUT_RSP);
              } else {
                app_log("OTA DFU - write with response \n");
                ota_change_state(OTA_UPLOAD_WITH_RSP);
              }
            }
            break;

          case sl_bt_evt_connection_closed_id:
            ERROR_EXIT("\nError, Connection closed, reason 0x%x", p->data.evt_connection_closed.reason);
            break;

          default:
            break;
        }
        break;

      case OTA_CONNECT:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_connection_opened_id:
            app_log("OK\n");
            ota_change_state(OTA_FIND_SERVICES);
            break;

          case sl_bt_evt_connection_closed_id:
            ERROR_EXIT("\nError, Connection closed, reason 0x%x", p->data.evt_connection_closed.reason);
            break;

          default:
            break;
        }
        break;

      case OTA_FIND_SERVICES:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_gatt_procedure_completed_id:
            if (ota_gatt_service_handle == 0xFFFFFFFF) {
              ERROR_EXIT("Error, no valid OTA service found");
            }

            app_log("OK\n");
            ota_change_state(OTA_FIND_CHARACTERISTICS);
            break;

          case sl_bt_evt_gatt_service_id:
            ota_gatt_service_handle = p->data.evt_gatt_service.service;
            break;

          case sl_bt_evt_connection_closed_id:
            ERROR_EXIT("\nError, Connection closed, reason 0x%x", p->data.evt_connection_closed.reason);
            break;

          default:
            break;
        }
        break;

      case OTA_FIND_CHARACTERISTICS:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_gatt_procedure_completed_id:
            if (ota_control_characteristic == 0xFFFF) {
              ERROR_EXIT("Error, no valid OTA characteristics found");
            } else if (ota_data_characteristic == 0xFFFF) {
              app_log("\nRestarting target.");
              ota_change_state(OTA_RESET_TO_DFU);
              break;
            }

            app_log("OK\n    Control handle:%d\n    Data handle:%d\n", ota_control_characteristic, ota_data_characteristic);
            if (ota_version_characteristic != 0xFFFF) {
              ota_change_state(OTA_READ_OTA_VERSION);
            } else {
              ota_change_state(OTA_READ_OTA_DATA_PROPERTIES);
            }
            break;

          case sl_bt_evt_gatt_characteristic_id:
            if (p->data.evt_gatt_characteristic.uuid.len == sizeof(uuid_ota_control)
                && !memcmp(p->data.evt_gatt_characteristic.uuid.data, uuid_ota_control, sizeof(uuid_ota_control))) {
              ota_control_characteristic = p->data.evt_gatt_characteristic.characteristic;
            } else if (p->data.evt_gatt_characteristic.uuid.len == sizeof(uuid_ota_data)
                       && !memcmp(p->data.evt_gatt_characteristic.uuid.data, uuid_ota_data, sizeof(uuid_ota_data))) {
              ota_data_characteristic = p->data.evt_gatt_characteristic.characteristic;
            } else if (p->data.evt_gatt_characteristic.uuid.len == sizeof(uuid_bootloader_version)
                       && !memcmp(p->data.evt_gatt_characteristic.uuid.data, uuid_bootloader_version, sizeof(uuid_bootloader_version))) {
              bootloader_version_characteristic = p->data.evt_gatt_characteristic.characteristic;
            } else if (p->data.evt_gatt_characteristic.uuid.len == sizeof(uuid_apploader_version)
                       && !memcmp(p->data.evt_gatt_characteristic.uuid.data, uuid_apploader_version, sizeof(uuid_apploader_version))) {
              apploader_version_characteristic = p->data.evt_gatt_characteristic.characteristic;
            } else if (p->data.evt_gatt_characteristic.uuid.len == sizeof(uuid_ota_version)
                       && !memcmp(p->data.evt_gatt_characteristic.uuid.data, uuid_ota_version, sizeof(uuid_ota_version))) {
              ota_version_characteristic = p->data.evt_gatt_characteristic.characteristic;
            } else if (p->data.evt_gatt_characteristic.uuid.len == sizeof(uuid_application_version)
                       && !memcmp(p->data.evt_gatt_characteristic.uuid.data, uuid_application_version, sizeof(uuid_bootloader_version))) {
              application_version_characteristic = p->data.evt_gatt_characteristic.characteristic;
            }
            break;

          case sl_bt_evt_connection_closed_id:
            ERROR_EXIT("\nError, Connection closed, reason 0x%x", p->data.evt_connection_closed.reason);
            break;

          default:
            break;
        }
        break;

      case OTA_READ_OTA_DATA_PROPERTIES:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_gatt_characteristic_value_id:
            if (p->data.evt_gatt_characteristic_value.value.len == 1) {
              ota_data_properties = p->data.evt_gatt_characteristic_value.value.data[0];
            }
            app_log("    OTA Data characteristic properties:0x%02x\n", ota_data_properties);
            break;

          case sl_bt_evt_gatt_procedure_completed_id:
            ota_change_state(OTA_BEGIN);
            break;

          case sl_bt_evt_connection_closed_id:
            ERROR_EXIT("\nError, Connection closed, reason 0x%x", p->data.evt_connection_closed.reason);
            break;

          default:
            break;
        }
        break;

      case OTA_RESET_TO_DFU:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_connection_closed_id:
            app_log("\nConnection closed, retrying. (Remote device booting in DFU mode)\n");
            ota_change_state(OTA_SCAN);
            break;
          default:
            break;
        }
        break;

      case OTA_SCAN:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_scanner_legacy_advertisement_report_id:
            if (p->data.evt_scanner_legacy_advertisement_report.event_flags
                & SL_BT_SCANNER_EVENT_FLAG_CONNECTABLE) {
              handle_scan_event(p->data.evt_scanner_legacy_advertisement_report.address.addr,
                                p->data.evt_scanner_legacy_advertisement_report.address_type,
                                p->data.evt_scanner_legacy_advertisement_report.data.data,
                                p->data.evt_scanner_legacy_advertisement_report.data.len);
            }
            break;
          case sl_bt_evt_scanner_extended_advertisement_report_id:
            if ((p->data.evt_scanner_extended_advertisement_report.event_flags
                 & SL_BT_SCANNER_EVENT_FLAG_CONNECTABLE)
                && (p->data.evt_scanner_extended_advertisement_report.data_completeness
                    == sl_bt_scanner_data_status_complete)) {
              handle_scan_event(p->data.evt_scanner_extended_advertisement_report.address.addr,
                                p->data.evt_scanner_extended_advertisement_report.address_type,
                                p->data.evt_scanner_extended_advertisement_report.data.data,
                                p->data.evt_scanner_extended_advertisement_report.data.len);
            }
            break;
          default:
            break;
        }
        break;

      case OTA_READ_OTA_VERSION:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_gatt_characteristic_value_id:
            if (p->data.evt_gatt_characteristic_value.value.len == 1) {
              memcpy(&ota_version, p->data.evt_gatt_characteristic_value.value.data, p->data.evt_gatt_characteristic_value.value.len);
              app_log("    OTA protocol version:0x%02x\n", ota_version);
            }
            break;
          case sl_bt_evt_gatt_procedure_completed_id:
            ota_change_state(OTA_READ_APPLOADER_VERSION);
            break;
          case sl_bt_evt_connection_closed_id:
            ERROR_EXIT("\nError, Connection closed, reason 0x%x", p->data.evt_connection_closed.reason);
            break;
          default:
            break;
        }
        break;

      case OTA_READ_APPLOADER_VERSION:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_gatt_characteristic_value_id:
            if (p->data.evt_gatt_characteristic_value.value.len == 8) {
              memcpy(&apploader_version, p->data.evt_gatt_characteristic_value.value.data, p->data.evt_gatt_characteristic_value.value.len);
              if (ota_version >= 3) {
                app_log("    Apploader version:%d.%d.%d.%d\n", apploader_version[0], apploader_version[1], apploader_version[2], apploader_version[3]);
              } else {
                app_log("    Bluetooth stack version:%d.%d.%d.%d\n", apploader_version[0], apploader_version[1], apploader_version[2], apploader_version[3]);
              }
            }
            break;
          case sl_bt_evt_gatt_procedure_completed_id:
            if (ota_version >= 3) {
              ota_change_state(OTA_READ_BOOTLOADER_VERSION);
            } else {
              ota_change_state(OTA_READ_OTA_DATA_PROPERTIES);
            }
            break;
          case sl_bt_evt_connection_closed_id:
            ERROR_EXIT("\nError, Connection closed, reason 0x%x", p->data.evt_connection_closed.reason);
            break;
          default:
            break;
        }
        break;

      case OTA_READ_BOOTLOADER_VERSION:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_gatt_characteristic_value_id:
            if (p->data.evt_gatt_characteristic_value.value.len == 4) {
              memcpy(&bootloader_version, p->data.evt_gatt_characteristic_value.value.data, p->data.evt_gatt_characteristic_value.value.len);
              app_log("    Bootloader version:0x%08x\n", bootloader_version);
            }
            break;
          case sl_bt_evt_gatt_procedure_completed_id:
            if (bootloader_version == 0) {
              ERROR_EXIT("\nError, no bootloader present");
            }
            ota_change_state(OTA_READ_APPLICATION_VERSION);
            break;
          case sl_bt_evt_connection_closed_id:
            ERROR_EXIT("\nError, Connection closed, reason 0x%x", p->data.evt_connection_closed.reason);
            break;
          default:
            break;
        }
        break;

      case OTA_READ_APPLICATION_VERSION:
        switch (SL_BT_MSG_ID(p->header)) {
          case sl_bt_evt_gatt_characteristic_value_id:
            if (p->data.evt_gatt_characteristic_value.value.len == 4) {
              memcpy(&application_version, p->data.evt_gatt_characteristic_value.value.data, p->data.evt_gatt_characteristic_value.value.len);
              app_log("    Application version:0x%08x\n", application_version);
            }
            break;
          case sl_bt_evt_gatt_procedure_completed_id:
            ota_change_state(OTA_READ_OTA_DATA_PROPERTIES);
            break;
          case sl_bt_evt_connection_closed_id:
            ERROR_EXIT("\nError, Connection closed, reason 0x%x", p->data.evt_connection_closed.reason);
            break;
          default:
            break;
        }
        break;

      default:
        break;
    }
  }

  fclose(dfu_file);
  return 0;
}

static inline int32_t uart_rx_wrapper(uint32_t len, uint8_t* data)
{
  return uartRx(handle_ptr, len, data);
}

static inline int32_t uart_peek_wrapper(void)
{
  return uartRxPeek(handle_ptr);
}
