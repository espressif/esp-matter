/***************************************************************************//**
 * @file
 * @brief Scan for VoBLE devices source file
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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "scan.h"

#include "app_log.h"
#include "app_assert.h"

#include "sl_bt_api.h"

/** Flag indicated if scanning process found device with VoBLE service */
static bool voble_device_found = false;

/***********************************************************************************************//**
 *  \brief  Check if VoBLE service UUID is included at given data buffer  .
 *  \param[in]  data buffer
 *  \param[in] data buffer size
 *  \return  true if VoBLE service UUID exists, otherwise false
 **************************************************************************************************/
static bool is_voble_service(const uint8_t *data, size_t data_len)
{
  uint8_t service_uuid[] = SERVICE_VOICE_OVER_BLE_UUID;
  uint8_t service_uuid_len = sizeof(service_uuid) / sizeof(uint8_t);
  uint8_t offset = 0;

  for ( uint8_t i = 0; i < data_len; i++) {
    for (uint8_t j = 0; j < service_uuid_len; j++) {
      if ( data[i + offset] != service_uuid[j] ) {
        offset = 0;
        break;
      } else {
        offset++;
        if (j == (service_uuid_len - 1)) {
          return true;
        }
      }
    }
  }
  return false;
}

/***********************************************************************************************//**
 *  \brief  Check if device with VoBLE service found.
 *  \return  true if device found, otherwise false
 **************************************************************************************************/
bool SCAN_Is_Device_Found(void)
{
  return voble_device_found;
}

static void print_address(bd_addr address)
{
  for (int i = 5; i >= 0; i--) {
    app_log("%02x", address.addr[i]);
    if (i > 0) {
      app_log(":");
    }
  }
}

/***********************************************************************************************//**
 *  \brief  Event handler for gecko_evt_le_gap_scan_response_id event.
 *  \param[in] evt Event pointer.
 **************************************************************************************************/
void SCAN_Process_scan_response(sl_bt_msg_t *evt)
{
  if ( is_voble_service(evt->data.evt_scanner_legacy_advertisement_report.data.data,
                        evt->data.evt_scanner_legacy_advertisement_report.data.len) ) {
    memcpy(&CONF_get()->remote_address, &evt->data.evt_scanner_legacy_advertisement_report.address, sizeof(bd_addr));
    app_log("\r"); DEBUG_INFO("VoBLE device found: "); print_address(CONF_get()->remote_address); app_log("\n");
    sl_status_t sc = sl_bt_scanner_stop();
    app_assert(sc == SL_STATUS_OK,
               "[E: 0x%04x] Failed to stop discovery\n",
               (int)sc);

    voble_device_found = true;
  }
}
