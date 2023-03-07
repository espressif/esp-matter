/***************************************************************************//**
 * @file
 * @brief Ambient Light and UV Index GATT Service
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
#include "sl_gatt_service_light.h"
#include "sl_gatt_service_light_config.h"

// -----------------------------------------------------------------------------
// Private variables

// default ambient light: 0 lux
static uint32_t light_lux = 0;
// default UV index: 0
static uint8_t light_uvi = 0;

// -----------------------------------------------------------------------------
// Private function declarations

static void light_update(void);
static void light_ambient_light_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void light_uvindex_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void light_update(void)
{
  sl_status_t sc;
  float lux;
  float uvi;

  sc = sl_gatt_service_light_get(&lux, &uvi);
  // keep previous data if measurement fails
  if (SL_STATUS_OK == sc) {
    light_lux = (uint32_t)(lux * 100);
    light_uvi = (uint8_t)uvi;
  } else if (sc == SL_STATUS_NOT_INITIALIZED) {
    light_lux = SL_GATT_SERVICE_LIGHT_LUX_INVALID;
    light_uvi = SL_GATT_SERVICE_LIGHT_UVI_INVALID;
  }
}

static void light_ambient_light_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  light_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(light_lux),
    (uint8_t*)&light_lux,
    NULL);
  app_assert_status(sc);
}

static void light_uvindex_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  light_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(light_uvi),
    (uint8_t*)&light_uvi,
    NULL);
  app_assert_status(sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_light_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  if (sl_bt_evt_gatt_server_user_read_request_id == SL_BT_MSG_ID(evt->header)) {
    if (gattdb_es_ambient_light == evt->data.evt_gatt_server_user_read_request.characteristic) {
      light_ambient_light_read_cb(&evt->data.evt_gatt_server_user_read_request);
    } else if (gattdb_es_uvindex == evt->data.evt_gatt_server_user_read_request.characteristic) {
      light_uvindex_read_cb(&evt->data.evt_gatt_server_user_read_request);
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_light_get(float* lux, float* uvi)
{
  (void)lux;
  (void)uvi;
  // keep default values
  return SL_STATUS_FAIL;
}
