/***************************************************************************//**
 * @file
 * @brief Air Quality GATT Service
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
#include "sl_gatt_service_gas.h"
#include "sl_gatt_service_gas_config.h"

// -----------------------------------------------------------------------------
// Private variables

static uint16_t gas_eco2 = 0;
static uint16_t gas_tvoc = 0;

// -----------------------------------------------------------------------------
// Private function declarations

static void gas_update(void);
static void gas_eco2_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void gas_tvoc_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void gas_update(void)
{
  sl_status_t sc;
  uint16_t eco2;
  uint16_t tvoc;

  sc = sl_gatt_service_gas_get(&eco2, &tvoc);
  // keep previous data if measurement fails
  if (SL_STATUS_OK == sc) {
    gas_eco2 = eco2;
    gas_tvoc = tvoc;
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    gas_eco2 = SL_GATT_SERVICE_GAS_ECO2_INVALID;
    gas_tvoc = SL_GATT_SERVICE_GAS_TVOC_INVALID;
  }
}

static void gas_eco2_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  gas_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(gas_eco2),
    (uint8_t*)&gas_eco2,
    NULL);
  app_assert_status(sc);
}

static void gas_tvoc_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  gas_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(gas_tvoc),
    (uint8_t*)&gas_tvoc,
    NULL);
  app_assert_status(sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_gas_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  if (sl_bt_evt_gatt_server_user_read_request_id == SL_BT_MSG_ID(evt->header)) {
    if (gattdb_iaq_eco2 == evt->data.evt_gatt_server_user_read_request.characteristic) {
      gas_eco2_read_cb(&evt->data.evt_gatt_server_user_read_request);
    } else if (gattdb_iaq_tvoc == evt->data.evt_gatt_server_user_read_request.characteristic) {
      gas_tvoc_read_cb(&evt->data.evt_gatt_server_user_read_request);
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_gas_get(uint16_t *eco2, uint16_t *tvoc)
{
  (void)eco2;
  (void)tvoc;
  // keep default values
  return SL_STATUS_FAIL;
}
