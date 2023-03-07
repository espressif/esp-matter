/***************************************************************************//**
 * @file
 * @brief RGB LED GATT Service
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

#include <string.h>
#include "em_core.h"
#include "sl_status.h"
#include "gatt_db.h"
#include "app_assert.h"
#include "sl_gatt_service_rgb.h"

// -----------------------------------------------------------------------------
// Private variables

static struct {
  uint8_t m; // mask
  uint8_t r; // red
  uint8_t g; // green
  uint8_t b; // blue
} rgb_led_state = { .m = 0, .r = 0, .g = 0x50, .b = 0x70 };

// -----------------------------------------------------------------------------
// Private function declarations

static void rgb_system_boot_cb(sl_bt_evt_system_boot_t *data);
static void rgb_connection_opened_cb(sl_bt_evt_connection_opened_t *data);
static void rgb_connection_closed_cb(sl_bt_evt_connection_closed_t *data);
static void rgb_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void rgb_write_cb(sl_bt_evt_gatt_server_user_write_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void rgb_system_boot_cb(sl_bt_evt_system_boot_t *data)
{
  (void)data;
  sl_status_t sc;
  uint8_t mask;

  mask = sl_gatt_service_rgb_get_led_mask();
  sc = sl_bt_gatt_server_write_attribute_value(gattdb_ui_rgbled_mask,
                                               0,
                                               sizeof(mask),
                                               &mask);
  app_log_status_error(sc);
}

static void rgb_connection_opened_cb(sl_bt_evt_connection_opened_t *data)
{
  (void)data;
}

static void rgb_connection_closed_cb(sl_bt_evt_connection_closed_t *data)
{
  (void)data;
  rgb_led_state.m = 0;
}

static void rgb_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(rgb_led_state),
    (uint8_t*)&rgb_led_state,
    NULL);
  app_assert_status(sc);
}

static void rgb_write_cb(sl_bt_evt_gatt_server_user_write_request_t *data)
{
  sl_status_t sc;
  uint8_t att_errorcode = 0;

  if (data->value.len == sizeof(rgb_led_state)) {
    (void)memcpy(&rgb_led_state, data->value.data, sizeof(rgb_led_state));
    sl_gatt_service_rgb_set_led(rgb_led_state.m, rgb_led_state.r, rgb_led_state.g, rgb_led_state.b);
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

void sl_gatt_service_rgb_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      rgb_system_boot_cb(&evt->data.evt_system_boot);
      break;

    case sl_bt_evt_connection_opened_id:
      rgb_connection_opened_cb(&evt->data.evt_connection_opened);
      break;

    case sl_bt_evt_connection_closed_id:
      rgb_connection_closed_cb(&evt->data.evt_connection_closed);
      break;

    case sl_bt_evt_gatt_server_user_read_request_id:
      if (gattdb_ui_rgbleds == evt->data.evt_gatt_server_user_read_request.characteristic) {
        rgb_read_cb(&evt->data.evt_gatt_server_user_read_request);
      }
      break;

    case sl_bt_evt_gatt_server_user_write_request_id:
      if (gattdb_ui_rgbleds == evt->data.evt_gatt_server_user_write_request.characteristic) {
        rgb_write_cb(&evt->data.evt_gatt_server_user_write_request);
      }
      break;
  }
}

SL_WEAK void sl_gatt_service_rgb_set_led(uint8_t m, uint8_t r, uint8_t g, uint8_t b)
{
  app_log_debug("RGB set: %d %d %d %d\n", m, r, g, b);
}

SL_WEAK uint8_t sl_gatt_service_rgb_get_led_mask(void)
{
  return 0;
}
