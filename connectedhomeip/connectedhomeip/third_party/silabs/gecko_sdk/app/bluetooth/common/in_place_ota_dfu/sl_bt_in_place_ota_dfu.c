/***************************************************************************//**
 * @file
 * @brief In-Place Over-the-Air Device Firmware Update
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

#include "gatt_db.h"
#include "app_assert.h"
#include "sl_apploader_util.h"
#include "sl_bt_in_place_ota_dfu.h"

// Flag for indicating DFU reset must be performed.
static bool boot_to_dfu = false;

/**************************************************************************//**
 * Bluetooth stack event handler.
 *****************************************************************************/
void sl_bt_in_place_ota_dfu_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;

  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates that a remote GATT client is attempting to write
    // a value of a user type attribute in to the local GATT database.
    case sl_bt_evt_gatt_server_user_write_request_id:
      // If user-type OTA Control Characteristic was written, boot the device
      // into Device Firmware Upgrade (DFU) mode. Written value is ignored.
      if (evt->data.evt_gatt_server_user_write_request.characteristic
          == gattdb_ota_control) {
        // Set flag to enter OTA mode.
        boot_to_dfu = true;
        // Send response to user write request.
        sc = sl_bt_gatt_server_send_user_write_response(
          evt->data.evt_gatt_server_user_write_request.connection,
          gattdb_ota_control,
          SL_STATUS_OK);
        app_assert_status(sc);
        // Close connection to enter to DFU OTA mode
        sc = sl_bt_connection_close(
          evt->data.evt_gatt_server_user_write_request.connection);
        app_assert_status(sc);
      }
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      // Check if need to boot to OTA DFU mode.
      if (boot_to_dfu) {
        sl_apploader_util_reset_to_ota_dfu();
      }
      break;

    default:
      break;
  }
}
