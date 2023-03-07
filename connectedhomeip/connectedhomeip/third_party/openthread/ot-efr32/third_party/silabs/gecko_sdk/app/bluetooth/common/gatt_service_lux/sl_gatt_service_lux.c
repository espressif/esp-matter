/***************************************************************************//**
 * @file
 * @brief Ambient Light GATT Service
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

#include "em_common.h"
#include "sl_status.h"
#include "gatt_db.h"
#include "app_assert.h"
#include "sl_gatt_service_lux.h"
#include "sl_gatt_service_lux_config.h"

// -----------------------------------------------------------------------------
// Private variables

// default ambient light: 0 lux
static uint32_t lux_char_value = 0;

// -----------------------------------------------------------------------------
// Private function declarations

static void lux_ambient_light_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void lux_ambient_light_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  float lux;

  sc = sl_gatt_service_lux_get(&lux);
  // update measurement data, keep previous data if measurement fails
  if (SL_STATUS_OK == sc) {
    lux_char_value = (uint32_t)(lux * 100);
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    lux_char_value = SL_GATT_SERVICE_LUX_LUX_INVALID;
  } else {
    return;
  }

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(lux_char_value),
    (uint8_t*)&lux_char_value,
    NULL);
  app_assert_status(sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_lux_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  if (sl_bt_evt_gatt_server_user_read_request_id == SL_BT_MSG_ID(evt->header)) {
    if (gattdb_es_ambient_light == evt->data.evt_gatt_server_user_read_request.characteristic) {
      lux_ambient_light_read_cb(&evt->data.evt_gatt_server_user_read_request);
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_lux_get(float* lux)
{
  (void)lux;
  // keep default values
  return SL_STATUS_FAIL;
}
