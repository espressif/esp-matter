/***************************************************************************//**
 * @file
 * @brief Relative Humidity and Temperature GATT Service
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
#include "sl_gatt_service_rht.h"
#include "sl_gatt_service_rht_config.h"

// -----------------------------------------------------------------------------
// Private variables

// default relative humidity: 50 %
static uint16_t rht_humidity = 5000;
// default temperature: 25 C
static int16_t rht_temperature = 2500;

// -----------------------------------------------------------------------------
// Private function declarations

static void rht_update(void);
static void rht_humidity_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void rht_temperature_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void rht_update(void)
{
  sl_status_t sc;
  uint32_t humidity;
  int32_t temperature;

  sc = sl_gatt_service_rht_get(&humidity, &temperature);
  // keep previous data if measurement fails
  if (SL_STATUS_OK == sc) {
    rht_humidity = humidity / 10;       // 0.01 %
    rht_temperature = temperature / 10; // 0.01 C
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    rht_humidity = SL_GATT_SERVICE_RHT_RH_INVALID;
    rht_temperature = SL_GATT_SERVICE_RHT_T_INVALID;
  }
}

static void rht_humidity_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  rht_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(rht_humidity),
    (uint8_t*)&rht_humidity,
    NULL);
  app_assert_status(sc);
}

static void rht_temperature_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  rht_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(rht_temperature),
    (uint8_t*)&rht_temperature,
    NULL);
  app_assert_status(sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_rht_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  if (sl_bt_evt_gatt_server_user_read_request_id == SL_BT_MSG_ID(evt->header)) {
    if (gattdb_es_temperature == evt->data.evt_gatt_server_user_read_request.characteristic) {
      rht_temperature_read_cb(&evt->data.evt_gatt_server_user_read_request);
    } else if (gattdb_es_humidity == evt->data.evt_gatt_server_user_read_request.characteristic) {
      rht_humidity_read_cb(&evt->data.evt_gatt_server_user_read_request);
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_rht_get(uint32_t *rh, int32_t *t)
{
  (void)rh;
  (void)t;
  // keep default values
  return SL_STATUS_FAIL;
}
