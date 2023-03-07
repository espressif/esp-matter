/***************************************************************************//**
 * @file
 * @brief Hall Effect GATT Service
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

#include <math.h>
#include "em_core.h"
#include "em_common.h"
#include "sl_status.h"
#include "sl_simple_timer.h"
#include "gatt_db.h"
#include "app_assert.h"
#include "sl_gatt_service_hall.h"
#include "sl_gatt_service_hall_config.h"

// -----------------------------------------------------------------------------
// Configuration

#define HALL_MEASUREMENT_INTERVAL_MS 250

// -----------------------------------------------------------------------------
// Private macros

// Possible values of State characteristic
#define HALL_STATE_CLOSED  0x00
#define HALL_STATE_OPEN    0x01
#define HALL_STATE_TAMPER  0x02

#define HALL_CONTROLPOINT_OPCODE_TAMPER_CLEAR       0x0001

// -----------------------------------------------------------------------------
// Private variables

static sl_simple_timer_t hall_timer;
static uint8_t hall_connection = 0;

// Field strength characteristic variables
static bool hall_field_strength_notification = false;
static int32_t hall_field_strength_value = 0;

// State characteristic variables
static bool hall_state_notification = false;
static uint8_t hall_state_value = HALL_STATE_OPEN;
static bool hall_tamper_latch = false;

// -----------------------------------------------------------------------------
// Private function declarations

static void hall_update(void);
static void hall_field_strength_notify(void);
static void hall_state_notify(void);
static void hall_timer_cb(sl_simple_timer_t *timer, void *data);
static void hall_connection_closed_cb(sl_bt_evt_connection_closed_t *data);
static void hall_char_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void hall_char_config_changed_cb(sl_bt_evt_gatt_server_characteristic_status_t *data);
static void hall_char_write_cb(sl_bt_evt_gatt_server_user_write_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void hall_update(void)
{
  sl_status_t sc;
  float field_strength = 0;
  bool alert = false;
  bool tamper = false;

  // get measurement values
  sc = sl_gatt_service_hall_get(&field_strength, &alert, &tamper);

  if (SL_STATUS_OK == sc) {
    // convert mT to uT, round to closest integer
    hall_field_strength_value = lroundf(field_strength * 1000);
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    hall_field_strength_value = SL_GATT_SERVICE_HALL_FIELD_STRENGTH_INVALID;
    alert = SL_GATT_SERVICE_HALL_ALERT_INVALID;
    tamper = SL_GATT_SERVICE_HALL_TAMPER_INVALID;
  } else {
    return;
  }

  // derive State characteristic value from measurement
  if (tamper) {
    hall_tamper_latch = true;
  }
  if (hall_tamper_latch) {
    hall_state_value = HALL_STATE_TAMPER;
  } else if (alert) {
    hall_state_value = HALL_STATE_CLOSED;
  } else {
    hall_state_value = HALL_STATE_OPEN;
  }
}

static void hall_field_strength_notify(void)
{
  sl_status_t sc;
  sc = sl_bt_gatt_server_send_notification(
    hall_connection,
    gattdb_hall_field_strength,
    sizeof(hall_field_strength_value),
    (uint8_t*)&hall_field_strength_value);
  app_assert_status(sc);
}

static void hall_state_notify(void)
{
  sl_status_t sc;
  sc = sl_bt_gatt_server_send_notification(
    hall_connection,
    gattdb_hall_state,
    sizeof(hall_state_value),
    (uint8_t*)&hall_state_value);
  app_assert_status(sc);
}

static void hall_timer_cb(sl_simple_timer_t * timer, void *data)
{
  (void)data;
  (void)timer;
  uint8_t hall_state_old = hall_state_value;

  hall_update();

  if (hall_field_strength_notification) {
    hall_field_strength_notify();
  }
  if (hall_state_notification && (hall_state_old != hall_state_value)) {
    hall_state_notify();
  }
}

static void hall_connection_closed_cb(sl_bt_evt_connection_closed_t * data)
{
  (void)data;
  sl_status_t sc;
  // stop periodic timer
  sc = sl_simple_timer_stop(&hall_timer);
  app_assert_status(sc);
  // reset notification flags
  hall_field_strength_notification = false;
  hall_state_notification = false;
}

static void hall_char_read_cb(sl_bt_evt_gatt_server_user_read_request_t * data)
{
  sl_status_t sc;
  uint8_t* value = NULL;
  size_t value_len = 0;

  hall_update();

  switch (data->characteristic) {
    case gattdb_hall_field_strength:
      value = (uint8_t*)&hall_field_strength_value;
      value_len = sizeof(hall_field_strength_value);
      break;
    case gattdb_hall_state:
      value = &hall_state_value;
      value_len = sizeof(hall_state_value);
      break;
    default:
      app_assert(false, "Unexpected characteristic\n");
      break;
  }
  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    value_len,
    value,
    NULL);
  app_assert_status(sc);
}

static void hall_char_config_changed_cb(sl_bt_evt_gatt_server_characteristic_status_t * data)
{
  sl_status_t sc;
  bool enable = sl_bt_gatt_disable != data->client_config_flags;
  void (*notify)(void) = NULL;
  hall_connection = data->connection;

  // update notification status
  switch (data->characteristic) {
    case gattdb_hall_field_strength:
      hall_field_strength_notification = enable;
      notify = &hall_field_strength_notify;
      break;
    case gattdb_hall_state:
      hall_state_notification = enable;
      notify = &hall_state_notify;
      break;
    default:
      app_assert(false, "Unexpected characteristic\n");
      break;
  }

  if (enable) {
    // update measurement data
    hall_update();
    // send the first notification
    (*notify)();
  }

  // start periodic timer if any of the notifications are enabled
  if (hall_field_strength_notification || hall_state_notification) {
    sc = sl_simple_timer_start(&hall_timer,
                               HALL_MEASUREMENT_INTERVAL_MS,
                               hall_timer_cb,
                               NULL,
                               true);
    app_assert_status(sc);
  } else {
    sc = sl_simple_timer_stop(&hall_timer);
    app_assert_status(sc);
  }
}

static void hall_char_write_cb(sl_bt_evt_gatt_server_user_write_request_t * data)
{
  sl_status_t sc;
  uint8_t att_errorcode = 0;
  uint16_t opcode = 0;

  if (data->value.len == 2) {
    opcode = *((uint16_t*)data->value.data);
    if (opcode != HALL_CONTROLPOINT_OPCODE_TAMPER_CLEAR) {
      att_errorcode = 0x13;   // Value Not Allowed
    }
  } else {
    att_errorcode = 0x0D;   // Invalid Attribute Value Length
  }

  sc = sl_bt_gatt_server_send_user_write_response(data->connection,
                                                  data->characteristic,
                                                  att_errorcode);
  app_assert_status(sc);

  if (0 == att_errorcode) {
    // reset tamper latch
    hall_tamper_latch = false;
    hall_update();
    if (hall_state_notification) {
      hall_state_notify();
    }
  }
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_hall_on_event(sl_bt_msg_t * evt)
{
  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_connection_closed_id:
      hall_connection_closed_cb(&evt->data.evt_connection_closed);
      break;

    case sl_bt_evt_gatt_server_user_read_request_id:
      if ((gattdb_hall_field_strength == evt->data.evt_gatt_server_user_read_request.characteristic)
          || (gattdb_hall_state == evt->data.evt_gatt_server_user_read_request.characteristic)) {
        hall_char_read_cb(&evt->data.evt_gatt_server_user_read_request);
      }
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      if ((sl_bt_gatt_server_client_config == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags)
          && ((gattdb_hall_field_strength == evt->data.evt_gatt_server_user_read_request.characteristic)
              || (gattdb_hall_state == evt->data.evt_gatt_server_user_read_request.characteristic))) {
        // client characteristic configuration changed by remote GATT client
        hall_char_config_changed_cb(&evt->data.evt_gatt_server_characteristic_status);
      }
      break;

    case sl_bt_evt_gatt_server_user_write_request_id:
      if (gattdb_hall_control_point == evt->data.evt_gatt_server_user_write_request.characteristic) {
        hall_char_write_cb(&evt->data.evt_gatt_server_user_write_request);
      }
      break;
  }
}

SL_WEAK sl_status_t sl_gatt_service_hall_get(float *field_strength, bool * alert, bool * tamper)
{
  static uint32_t cnt = 0;
  static float dummy_field_strength = 0;
  static bool dummy_alert = false;
  cnt++;
  dummy_field_strength += 1.0;
  if ((cnt % 10) == 0) {
    dummy_alert = dummy_alert ? false : true;
  }
  *field_strength = dummy_field_strength;
  *alert = dummy_alert;
  *tamper = false;
  return SL_STATUS_OK;
}
