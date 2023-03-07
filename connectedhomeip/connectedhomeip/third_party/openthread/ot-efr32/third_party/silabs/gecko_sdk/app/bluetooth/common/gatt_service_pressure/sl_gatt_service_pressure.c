/***************************************************************************//**
 * @file
 * @brief Air Pressure GATT Service
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
#include "sl_gatt_service_pressure.h"
#include "sl_gatt_service_pressure_config.h"

// -----------------------------------------------------------------------------
// Private variables

static uint32_t pressure = 10;

// -----------------------------------------------------------------------------
// Private function declarations

static void pressure_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void pressure_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  float p;

  sc = sl_gatt_service_pressure_get(&p);
  // keep previous data if measurement fails
  if (SL_STATUS_OK == sc) {
    /* 1 mbar = 100 Pa. We want 0.1 Pa, so multiply by 1000 */
    pressure = (uint32_t)(p * 1000.0f);
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    pressure = SL_GATT_SERVICE_PRESSURE_INVALID;
  } else {
    return;
  }

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(pressure),
    (uint8_t*)&pressure,
    NULL);
  app_assert_status(sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_pressure_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  if (sl_bt_evt_gatt_server_user_read_request_id == SL_BT_MSG_ID(evt->header)) {
    if (gattdb_es_pressure == evt->data.evt_gatt_server_user_read_request.characteristic) {
      pressure_read_cb(&evt->data.evt_gatt_server_user_read_request);
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_pressure_get(float *pressure)
{
  (void)pressure;
  // keep default value
  return SL_STATUS_FAIL;
}
