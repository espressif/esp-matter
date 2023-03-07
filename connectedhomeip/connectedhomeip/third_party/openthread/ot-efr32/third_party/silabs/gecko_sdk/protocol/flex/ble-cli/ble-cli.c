/***************************************************************************//**
 * @brief
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

#include "debug_print.h"
#include "sl_cli.h"
#include "sl_bluetooth.h"

void connect_ble_cli_hello_command(sl_cli_command_arg_t *arguments)
{
  sl_status_t status = sl_bt_system_hello();

  connect_core_debug_print("BLE hello: %s\n",
                           (status == SL_STATUS_OK) ? "success" : "error");
}

void connect_ble_cli_get_address_command(sl_cli_command_arg_t *arguments)
{
  bd_addr ble_address;

  sl_status_t status = sl_bt_system_get_identity_address(&ble_address, 0);

  connect_core_debug_print("BLE address: [%02X %02X %02X %02X %02X %02X]\n",
                           ble_address.addr[5], ble_address.addr[4],
                           ble_address.addr[3], ble_address.addr[2],
                           ble_address.addr[1], ble_address.addr[0]);
}

void connect_ble_cli_set_adv_params_command(sl_cli_command_arg_t *arguments)
{
  uint16_t min_interval = sl_cli_get_argument_uint16(arguments, 0);
  uint16_t max_interval = sl_cli_get_argument_uint16(arguments, 1);

  sl_status_t status = sl_bt_advertiser_set_timing(0,  // handle
                                                   min_interval,
                                                   max_interval,
                                                   0,              // continue advertisement until stopped
                                                   0);             // continue advertisement until stopped

  if (status == SL_STATUS_OK) {
    connect_core_debug_print("success\n");
  } else {
    connect_core_debug_print("error: 0x%04X\n", status);
  }
}

void connect_ble_cli_start_adv_command(sl_cli_command_arg_t *arguments)
{
  static uint8_t adv_handle = 0xFF;
  uint8_t discoverable_mode = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t connectable_mode = sl_cli_get_argument_uint8(arguments, 1);

  if (adv_handle == 0xFF) {
    sl_bt_advertiser_create_set(&adv_handle);
  }

  // Generate the advertising data from the GATT configurator
  sl_status_t status = sl_bt_legacy_advertiser_generate_data(adv_handle,
                                                             discoverable_mode);
  if (status == SL_STATUS_OK) {
    connect_core_debug_print("success\n");
  } else {
    connect_core_debug_print("error: 0x%04X\n", status);
  }

  status = sl_bt_legacy_advertiser_start(adv_handle,
                                         connectable_mode);

  if (status == SL_STATUS_OK) {
    connect_core_debug_print("success\n");
  } else {
    connect_core_debug_print("error: 0x%04X\n", status);
  }
}

void connect_ble_cli_open_connection_command(sl_cli_command_arg_t *arguments)
{
  size_t arg_length;
  uint8_t* contents = sl_cli_get_argument_hex(arguments, 0, &arg_length);
  uint8_t address_type = sl_cli_get_argument_uint8(arguments, 1);
  bd_addr address;
  uint8_t connection_handle;
  sl_status_t status;
  uint8_t i;

  if (arg_length != 6) {
    connect_core_debug_print("wrong address length\n");
    return;
  }

  // We do a reverse memcpy here so that we can cut&paste the address from the
  // node CLI output.
  for (i = 0; i < 6; i++) {
    address.addr[i] = contents[6 - 1 - i];
  }

  status = sl_bt_connection_open(address, address_type, 1, &connection_handle);

  if (status == SL_STATUS_OK) {
    connect_core_debug_print("success, handle=0x%02X\n", connection_handle);
  } else {
    connect_core_debug_print("error: 0x%04X\n", status);
  }
}

void connect_ble_cli_close_connection_command(sl_cli_command_arg_t *arguments)
{
  uint8_t connection_handle = sl_cli_get_argument_uint8(arguments, 0);

  sl_status_t status = sl_bt_connection_close(connection_handle);

  if (status == SL_STATUS_OK) {
    connect_core_debug_print("success\n");
  } else {
    connect_core_debug_print("error: 0x%04X\n", status);
  }
}

void connect_ble_cli_set_connection_params_command(sl_cli_command_arg_t *arguments)
{
  uint16_t min_interval = sl_cli_get_argument_uint16(arguments, 0);
  uint16_t max_interval = sl_cli_get_argument_uint16(arguments, 1);
  uint16_t slave_latency = sl_cli_get_argument_uint16(arguments, 2);
  uint16_t supervision_timeout = sl_cli_get_argument_uint16(arguments, 3);

  sl_status_t status = sl_bt_connection_set_default_parameters(min_interval,
                                                               max_interval,
                                                               slave_latency,
                                                               supervision_timeout,
                                                               0, // min_ce_length
                                                               0xFFFF); // max_ce_length
  if (status == SL_STATUS_OK) {
    connect_core_debug_print("success\n");
  } else {
    connect_core_debug_print("error: 0x%04X\n", status);
  }
}
