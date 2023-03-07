/***************************************************************************//**
 * @file
 * @brief Health Thermometer GATT service
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
#include "em_common.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app_assert.h"
#include "sl_health_thermometer.h"

enum temperature_measurement_flag {
  TEMPERATURE_MEASUREMENT_FLAG_UNITS     = 0x1,
  TEMPERATURE_MEASUREMENT_FLAG_TIMESTAMP = 0x2,
  TEMPERATURE_MEASUREMENT_FLAG_TYPE      = 0x4,
};

/**************************************************************************//**
 * Initialize Health Thermometer module.
 *****************************************************************************/
static void health_thermometer_init(void);

/**************************************************************************//**
 * Convert temperature value to characteristic buffer.
 * @param[in] value Temperature value in millidegree.
 * @param[in] fahrenheit Value is given in Fahrenheit (true) or Celsius (false).
 * @param[out] buffer Buffer to hold GATT characteristics value.
 *****************************************************************************/
static void temperature_measurement_val_to_buf(int32_t value,
                                               bool fahrenheit,
                                               uint8_t *buffer);

/**************************************************************************//**
 * Callback to handle connection closed event.
 *****************************************************************************/
SL_WEAK void sl_bt_connection_closed_cb(uint16_t reason, uint8_t connection)
{
  (void)reason;
  (void)connection;
}

/**************************************************************************//**
 * Send Temperature Measurement characteristic indication to the client.
 *****************************************************************************/
sl_status_t sl_bt_ht_temperature_measurement_indicate(uint8_t connection,
                                                      int32_t value,
                                                      bool fahrenheit)
{
  sl_status_t sc;
  uint8_t buf[5] = { 0 };
  temperature_measurement_val_to_buf(value, fahrenheit, buf);
  sc = sl_bt_gatt_server_send_indication(
    connection,
    gattdb_temperature_measurement,
    sizeof(buf),
    buf);
  return sc;
}

/**************************************************************************//**
 * Temperature Measurement characteristic's CCCD has changed.
 *****************************************************************************/
SL_WEAK void sl_bt_ht_temperature_measurement_indication_changed_cb(uint8_t connection,
                                                                    sl_bt_gatt_client_config_flag_t client_config)
{
  (void)connection;
  (void)client_config;
}

/**************************************************************************//**
 * Temperature Measurement characteristic indication confirmed.
 *****************************************************************************/
SL_WEAK void sl_bt_ht_temperature_measurement_indication_confirmed_cb(uint8_t connection)
{
  (void)connection;
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 *****************************************************************************/
void sl_bt_ht_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      health_thermometer_init();
      break;

    case sl_bt_evt_connection_closed_id:
      sl_bt_connection_closed_cb(evt->data.evt_connection_closed.reason,
                                 evt->data.evt_connection_closed.connection);
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      if (gattdb_temperature_measurement == evt->data.evt_gatt_server_characteristic_status.characteristic) {
        // client characteristic configuration changed by remote GATT client
        if (sl_bt_gatt_server_client_config == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          sl_bt_ht_temperature_measurement_indication_changed_cb(
            evt->data.evt_gatt_server_characteristic_status.connection,
            (sl_bt_gatt_client_config_flag_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags);
        }
        // confirmation of indication received from remove GATT client
        else if (sl_bt_gatt_server_confirmation == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          sl_bt_ht_temperature_measurement_indication_confirmed_cb(
            evt->data.evt_gatt_server_characteristic_status.connection);
        } else {
          app_assert(false,
                     "[E: 0x%04x] Unexpected status flag in evt_gatt_server_characteristic_status\n",
                     (int)evt->data.evt_gatt_server_characteristic_status.status_flags);
        }
      }
      break;

    default:
      break;
  }
}

// -----------------------------------------------------------------------------
// Private function definitions

static void health_thermometer_init(void)
{
  sl_status_t sc;
  uint8_t temperature_type = SL_BT_HT_TEMPERATURE_TYPE;
  uint16_t measurement_interval = SL_BT_HT_MEASUREMENT_INTERVAL_SEC;

  sc = sl_bt_gatt_server_write_attribute_value(gattdb_temperature_type,
                                               0,
                                               sizeof(temperature_type),
                                               &temperature_type);
  app_assert_status(sc);

  sc = sl_bt_gatt_server_write_attribute_value(gattdb_measurement_interval,
                                               0,
                                               sizeof(measurement_interval),
                                               (uint8_t *)&measurement_interval);
  app_assert_status(sc);
}

static void temperature_measurement_val_to_buf(int32_t value,
                                               bool fahrenheit,
                                               uint8_t *buffer)
{
  uint32_t tmp_value = ((uint32_t)value & 0x00ffffffu) \
                       | ((uint32_t)(-3) << 24);
  buffer[0] = fahrenheit ? TEMPERATURE_MEASUREMENT_FLAG_UNITS : 0;
  buffer[1] = tmp_value & 0xff;
  buffer[2] = (tmp_value >> 8) & 0xff;
  buffer[3] = (tmp_value >> 16) & 0xff;
  buffer[4] = (tmp_value >> 24) & 0xff;
}
