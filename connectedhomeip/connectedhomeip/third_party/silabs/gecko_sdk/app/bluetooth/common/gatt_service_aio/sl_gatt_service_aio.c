/***************************************************************************//**
 * @file
 * @brief Automation IO GATT service
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

#include "stdbool.h"
#include "sl_status.h"
#include "gatt_db.h"
#include "app_assert.h"
#include "sli_gatt_service_aio.h"
#include "sl_gatt_service_aio.h"

// -----------------------------------------------------------------------------
// Private variables

static bool aio_notification_enabled = false;
static bool aio_button_changed = false;
static uint8_t aio_connection = 0;

// -----------------------------------------------------------------------------
// Private function declarations

static void aio_digital_in_notify(void);
static void aio_system_boot_cb(void);
static void aio_connection_opened_cb(sl_bt_evt_connection_opened_t *data);
static void aio_connection_closed_cb(sl_bt_evt_connection_closed_t *data);
static void aio_digital_in_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void aio_digital_out_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void aio_digital_in_changed_cb(sl_bt_evt_gatt_server_characteristic_status_t *data);
static void aio_digital_out_write_cb(sl_bt_evt_gatt_server_user_write_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

SL_WEAK uint8_t aio_digital_in_get_num(void)
{
  return 0;
}

SL_WEAK uint8_t aio_digital_in_get_state(void)
{
  return 0;
}

SL_WEAK uint8_t aio_digital_out_get_num(void)
{
  return 0;
}

SL_WEAK uint8_t aio_digital_out_get_state(void)
{
  return 0;
}

SL_WEAK void aio_digital_out_set_state(uint8_t state)
{
  (void)state;
}

static void aio_digital_in_notify(void)
{
  sl_status_t sc;
  uint8_t value = aio_digital_in_get_state();
  sc = sl_bt_gatt_server_send_notification(
    aio_connection,
    gattdb_aio_digital_in,
    1,
    &value);
  app_assert_status(sc);
}

static void aio_system_boot_cb(void)
{
  sl_status_t sc;
  uint8_t in_cnt = aio_digital_in_get_num();
  uint8_t out_cnt = aio_digital_out_get_num();

  sc = sl_bt_gatt_server_write_attribute_value(gattdb_aio_num_of_digitals_in, 0, 1, &in_cnt);
  app_assert_status(sc);
  sc = sl_bt_gatt_server_write_attribute_value(gattdb_aio_num_of_digitals_out, 0, 1, &out_cnt);
  app_assert_status(sc);
}

static void aio_connection_opened_cb(sl_bt_evt_connection_opened_t *data)
{
  (void)data;
  // Reset LED state
  aio_digital_out_set_state(0);
}

static void aio_connection_closed_cb(sl_bt_evt_connection_closed_t *data)
{
  (void)data;
  // Reset LED state
  aio_digital_out_set_state(0);
  // Disable notifications
  aio_notification_enabled = false;
}

static void aio_digital_in_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  uint8_t value = aio_digital_in_get_state();
  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    1,
    &value,
    NULL);
  app_assert_status(sc);
}

static void aio_digital_out_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  uint8_t value = aio_digital_out_get_state();
  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    1,
    &value,
    NULL);
  app_assert_status(sc);
}

static void aio_digital_in_changed_cb(sl_bt_evt_gatt_server_characteristic_status_t *data)
{
  aio_connection = data->connection;
  // indication or notification enabled
  if (sl_bt_gatt_disable != data->client_config_flags) {
    // enable notifications
    aio_notification_enabled = true;
    // send the first notification
    aio_digital_in_notify();
  }
  // indication and notification disabled
  else {
    aio_notification_enabled = false;
  }
}

static void aio_digital_out_write_cb(sl_bt_evt_gatt_server_user_write_request_t *data)
{
  sl_status_t sc;
  uint8_t att_errorcode = 0;

  if (data->value.len == 1) {
    aio_digital_out_set_state(data->value.data[0]);
  } else {
    att_errorcode = 0x0D; // Invalid Attribute Value Length
  }

  sc = sl_bt_gatt_server_send_user_write_response(data->connection,
                                                  data->characteristic,
                                                  att_errorcode);
  app_assert_status(sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_aio_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      aio_system_boot_cb();
      break;

    case sl_bt_evt_connection_opened_id:
      aio_connection_opened_cb(&evt->data.evt_connection_opened);
      break;

    case sl_bt_evt_connection_closed_id:
      aio_connection_closed_cb(&evt->data.evt_connection_closed);
      break;

    case sl_bt_evt_gatt_server_user_read_request_id:
      switch (evt->data.evt_gatt_server_user_read_request.characteristic) {
        case gattdb_aio_digital_in:
          aio_digital_in_read_cb(&evt->data.evt_gatt_server_user_read_request);
          break;
        case gattdb_aio_digital_out:
          aio_digital_out_read_cb(&evt->data.evt_gatt_server_user_read_request);
          break;
      }
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      if ((gattdb_aio_digital_in == evt->data.evt_gatt_server_characteristic_status.characteristic)
          && (sl_bt_gatt_server_client_config == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags)) {
        // client characteristic configuration changed by remote GATT client
        aio_digital_in_changed_cb(&evt->data.evt_gatt_server_characteristic_status);
      }
      break;

    case sl_bt_evt_gatt_server_user_write_request_id:
      if (gattdb_aio_digital_out == evt->data.evt_gatt_server_user_write_request.characteristic) {
        aio_digital_out_write_cb(&evt->data.evt_gatt_server_user_write_request);
      }
      break;
  }
}

void sl_gatt_service_aio_step(void)
{
  if (aio_button_changed) {
    aio_button_changed = false;
    aio_digital_in_notify();
  }
}

void sl_gatt_service_aio_on_change(void)
{
  if (aio_notification_enabled) {
    aio_button_changed = true;
  }
}
