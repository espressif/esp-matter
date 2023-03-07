/***************************************************************************//**
 * @file
 * @brief Constant Tone Extension GATT Service (Advertising)
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
#include <string.h>
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sli_gatt_service_cte.h"
#include "sli_gatt_service_cte_adv.h"
#include "sl_gatt_service_cte_adv.h"

// CTE advertising characteristic values
adv_cte_min_len_t adv_cte_min_len;
adv_cte_min_tx_count_t adv_cte_min_tx_count;
adv_cte_interval_t adv_cte_interval;
adv_cte_phy_t adv_cte_phy;

/**************************************************************************//**
 * Bluetooth stack event handler.
 *****************************************************************************/
void sl_gatt_service_cte_adv_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  uint8_t connection;
  uint16_t characteristic;
  uint8_t len;
  void *data;
  bool request_processed;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      // Initialize advertising.
      adv_cte_init();
      break;

    case sl_bt_evt_gatt_server_user_write_request_id:
      // Set default response parameters.
      sc = SL_STATUS_OK;
      connection = evt->data.evt_gatt_server_user_write_request.connection;
      characteristic = evt->data.evt_gatt_server_user_write_request.characteristic;
      len = evt->data.evt_gatt_server_user_write_request.value.len;
      data = evt->data.evt_gatt_server_user_write_request.value.data;
      request_processed = true;

      // Evaluate characteristics.
      switch (characteristic) {
        case gattdb_adv_cte_min_len:
          if (len != sizeof(adv_cte_min_len_t)) {
            sc = SL_STATUS_BT_ATT_INVALID_ATT_LENGTH;
          } else {
            if ((*((adv_cte_min_len_t *)data) < ADV_CTE_MIN_LEN_MIN)
                || (*((adv_cte_min_len_t *)data) > ADV_CTE_MIN_LEN_MAX)) {
              sc = SL_STATUS_BT_ATT_OUT_OF_RANGE;
            } else {
              adv_cte_min_len = *((adv_cte_min_len_t *)data);
            }
          }
          break;

        case gattdb_adv_cte_min_tx_count:
          if (len != sizeof(adv_cte_min_tx_count_t)) {
            sc = SL_STATUS_BT_ATT_INVALID_ATT_LENGTH;
          } else {
            if ((*((adv_cte_min_tx_count_t *)data) < ADV_CTE_MIN_TX_COUNT_MIN)
                || (*((adv_cte_min_tx_count_t *)data) > ADV_CTE_MIN_TX_COUNT_MAX)) {
              sc = SL_STATUS_BT_ATT_OUT_OF_RANGE;
            } else {
              adv_cte_min_tx_count = *((adv_cte_min_tx_count_t *)data);
            }
          }
          break;

        case gattdb_adv_cte_tx_duration:
          // To be implemented.
          sc = SL_STATUS_BT_ATT_WRITE_REQUEST_REJECTED;
          break;

        case gattdb_adv_cte_interval:
          // Supports single byte values
          if ((len > sizeof(adv_cte_interval_t)) || (len == 0)) {
            sc = SL_STATUS_BT_ATT_INVALID_ATT_LENGTH;
          } else {
            adv_cte_interval_t buffer = 0;
            memcpy(&buffer, data, len);
            if (buffer < ADV_CTE_INTERVAL_MIN) {
              sc = SL_STATUS_BT_ATT_OUT_OF_RANGE;
            } else {
              adv_cte_interval = buffer;
            }
          }
          break;

        case gattdb_adv_cte_phy:
          if (len != sizeof(adv_cte_phy_t)) {
            sc = SL_STATUS_BT_ATT_INVALID_ATT_LENGTH;
          } else {
            if (*((adv_cte_phy_t *)data) > ADV_CTE_PHY_2M) {
              sc = SL_STATUS_BT_ATT_OUT_OF_RANGE;
            } else {
              adv_cte_phy = *((adv_cte_phy_t *)data);
            }
          }
          break;

        default:
          // Characteristic is not handled by this component.
          request_processed = false;
          break;
      }

      // Process characteristic value.
      if (request_processed) {
        if (SL_STATUS_OK == sc) {
          sc = adv_cte_start();
          // Map status code to a valid attribute error.
          if (SL_STATUS_OK != sc) {
            sc = SL_STATUS_BT_ATT_WRITE_REQUEST_REJECTED;
          }
        }
        // Send response.
        sl_bt_gatt_server_send_user_write_response(
          connection,
          characteristic,
          (uint8_t)sc);
      }
      break;
  }
}
