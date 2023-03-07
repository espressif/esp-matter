/***************************************************************************//**
 * @file
 * @brief Constant Tone Extension GATT Service (Connection)
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

#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sli_gatt_service_cte.h"
#include "sl_gatt_service_cte.h"

// Bit mask for the CTE Enable characteristic.
#define ENABLE_CONNECTION_CTE_BIT_MASK          0x01

/**************************************************************************//**
 * Bluetooth stack event handler.
 *****************************************************************************/
void sl_gatt_service_cte_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  uint8_t connection;
  uint16_t characteristic;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_gatt_server_user_write_request_id:
      // Set default response parameters.
      sc = SL_STATUS_BT_ATT_WRITE_REQUEST_REJECTED;
      connection = evt->data.evt_gatt_server_user_write_request.connection;
      characteristic = evt->data.evt_gatt_server_user_write_request.characteristic;

      // Evaluate characteristics.
      if (characteristic == gattdb_cte_enable) {
        if (evt->data.evt_gatt_server_user_write_request.value.len == 1) {
          if (evt->data.evt_gatt_server_user_write_request.value.data[0] & ENABLE_CONNECTION_CTE_BIT_MASK) {
            // If CTE enable bit was set in CTE enable characteristic, then enable CTE.
            sc = sl_bt_cte_transmitter_enable_connection_cte(
              connection,
              (1 << SLI_CTE_TYPE_AOA),
              SLI_CTE_SWITCHING_PATTERN_LENGTH,
              SLI_CTE_SWITCHING_PATTERN);
          } else {
            // If CTE enable bit was cleared in CTE enable characteristic, then disable CTE.
            sc = sl_bt_cte_transmitter_disable_connection_cte(connection);
          }
          // Map status code to a valid attribute error.
          if (SL_STATUS_OK != sc) {
            sc = SL_STATUS_BT_ATT_WRITE_REQUEST_REJECTED;
          }
        } else {
          sc = SL_STATUS_BT_ATT_INVALID_ATT_LENGTH;
        }
        sl_bt_gatt_server_send_user_write_response(connection, characteristic, (uint8_t)sc);
      }
      break;
  }
}
