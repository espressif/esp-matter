/***************************************************************************//**
 * @file
 * @brief Application Output LCD code
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
#include <stdio.h>
#include "em_common.h"
#include "sl_status.h"

#include "app.h"
#include "app_log.h"

#include "sl_btmesh_api.h"
#include "sl_btmesh_factory_reset.h"
#include "sl_btmesh_sensor_client.h"
#include "sl_btmesh_wstk_lcd.h"

/// Integer part of illuminance
#define INT_ILLUM(x) (x / 100)
/// Fractional part of illuminance
#define FRAC_ILLUM(x) (x % 100)
/// Integer part of temperature
#define INT_TEMP(x) (x / 2)
/// Fractional part of temperature
#define FRAC_TEMP(x) ((x * 5) % 10)

// -----------------------------------------------------------------------------
// BT mesh Sensor Client Callbacks

/*******************************************************************************
 * Called when Sensor Server discovery is started
 *
 ******************************************************************************/
void sl_btmesh_sensor_client_on_discovery_started(uint16_t property_id)
{
  app_log("BT mesh Sensor Device discovery is started. (property_id: 0x%04x)\r\n",
          property_id);
  // Clear the previous sensor measurement values
  for (uint8_t row = SL_BTMESH_WSTK_LCD_ROW_SENSOR_DATA_CFG_VAL; row <= LCD_ROW_MAX; row++) {
    sl_status_t lcd_status = sl_btmesh_LCD_write("", row);
    app_log_status_level_f(APP_LOG_LEVEL_ERROR, lcd_status, "LCD write failed");
  }
  (void)property_id;
}

/*******************************************************************************
 * Called when a Device with the current Device Property ID was found
 *
 ******************************************************************************/
void sl_btmesh_sensor_client_on_new_device_found(uint16_t property_id,
                                                 uint16_t address)
{
  app_log("BT mesh Sensor Device (address: 0x%04x, property_id: 0x%04x) "
          "is found.\r\n",
          address,
          property_id);
  (void)address;
  (void)property_id;
}

/*******************************************************************************
 * Called when temperature sensor data is received from one of the
 * registered devices
 *
 ******************************************************************************/
void sl_btmesh_sensor_client_on_new_temperature_data(uint8_t sensor_idx,
                                                     uint16_t address,
                                                     sl_btmesh_sensor_client_data_status_t status,
                                                     temperature_8_t temperature)
{
  // Temporary buffer to format the LCD output text
  char tmp_str[LCD_ROW_LEN];

  if (PRESENT_AMBIENT_TEMPERATURE != app_get_current_property()) {
    return;
  }

  if (SL_BTMESH_SENSOR_CLIENT_DATA_VALID == status) {
    app_log("BT mesh Sensor Temperature (from 0x%04x): %3d.%1d °C\r\n",
            address,
            INT_TEMP(temperature),
            FRAC_TEMP(temperature));

    snprintf(tmp_str,
             LCD_ROW_LEN,
             "Adr %4x Temp%3d.%1d C",
             address,
             INT_TEMP(temperature),
             FRAC_TEMP(temperature));
  } else if (SL_BTMESH_SENSOR_CLIENT_DATA_UNKNOWN == status) {
    app_log("BT mesh Sensor Temperature (from 0x%04x): UNKNOWN\r\n",
            address);
    snprintf(tmp_str, LCD_ROW_LEN, "Adr %4x Temp    N/K", address);
  } else {
    app_log("BT mesh Sensor Temperature (from 0x%04x): NOT AVAILABLE\r\n",
            address);
    snprintf(tmp_str, LCD_ROW_LEN, "Adr %4x Temp    N/A", address);
  }

  sl_status_t lcd_status = sl_btmesh_LCD_write(tmp_str,
                                               SL_BTMESH_WSTK_LCD_ROW_SENSOR_DATA_CFG_VAL + sensor_idx);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, lcd_status, "LCD write failed");
}

/*******************************************************************************
 * Called when people count sensor data is received from one of the
 * registered devices.
 *
 ******************************************************************************/
void sl_btmesh_sensor_client_on_new_people_count_data(uint8_t sensor_idx,
                                                      uint16_t address,
                                                      sl_btmesh_sensor_client_data_status_t status,
                                                      count16_t people_count)
{
  // Temporary buffer to format the LCD output text
  char tmp_str[LCD_ROW_LEN];

  if (PEOPLE_COUNT != app_get_current_property()) {
    return;
  }

  if (SL_BTMESH_SENSOR_CLIENT_DATA_VALID == status) {
    app_log("BT mesh Sensor People Count (from 0x%04x): %5u\r\n",
            address,
            people_count);
    snprintf(tmp_str, LCD_ROW_LEN, "Adr %4x Count  %5u", address, people_count);
  } else if (SL_BTMESH_SENSOR_CLIENT_DATA_UNKNOWN == status) {
    app_log("BT mesh Sensor People Count (from 0x%04x): UNKNOWN\r\n",
            address);
    snprintf(tmp_str, LCD_ROW_LEN, "Adr %4x Count    N/K", address);
  } else {
    app_log("BT mesh Sensor People Count (from 0x%04x): NOT AVAILABLE\r\n",
            address);
    snprintf(tmp_str, LCD_ROW_LEN, "Adr %4x Count    N/A", address);
  }

  sl_status_t lcd_status = sl_btmesh_LCD_write(tmp_str,
                                               SL_BTMESH_WSTK_LCD_ROW_SENSOR_DATA_CFG_VAL + sensor_idx);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, lcd_status, "LCD write failed");
}

/*******************************************************************************
 * Called when illuminance sensor data is received from one of the
 * registered devices.
 *
 ******************************************************************************/
void sl_btmesh_sensor_client_on_new_illuminance_data(uint8_t sensor_idx,
                                                     uint16_t address,
                                                     sl_btmesh_sensor_client_data_status_t status,
                                                     illuminance_t illuminance)
{
  // Temporary buffer to format the LCD output text
  char tmp_str[LCD_ROW_LEN];

  if (PRESENT_AMBIENT_LIGHT_LEVEL != app_get_current_property()) {
    return;
  }

  if (SL_BTMESH_SENSOR_CLIENT_DATA_VALID == status) {
    app_log("BT mesh Sensor Illuminance (from 0x%04x): %4lu.%02u lx\r\n",
            address,
            INT_ILLUM(illuminance),
            (uint8_t)(FRAC_ILLUM(illuminance)));
    snprintf(tmp_str,
             LCD_ROW_LEN,
             "Adr %4x Lux %4u.%02u",
             address,
             (uint16_t)(INT_ILLUM(illuminance)),
             (uint8_t)(FRAC_ILLUM(illuminance)));
  } else if (SL_BTMESH_SENSOR_CLIENT_DATA_UNKNOWN == status) {
    app_log("BT mesh Sensor Illuminance (from 0x%04x): UNKNOWN\r\n",
            address);
    snprintf(tmp_str, LCD_ROW_LEN, "Adr %4x Lux     N/K", address);
  } else {
    app_log("BT mesh Sensor Illuminance (from 0x%04x): NOT AVAILABLE\r\n",
            address);
    snprintf(tmp_str, LCD_ROW_LEN, "Adr %4x Lux     N/A", address);
  }

  sl_status_t lcd_status = sl_btmesh_LCD_write(tmp_str,
                                               SL_BTMESH_WSTK_LCD_ROW_SENSOR_DATA_CFG_VAL + sensor_idx);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, lcd_status, "LCD write failed");
}

// -----------------------------------------------------------------------------
// Provisioning Decorator Callbacks

/*******************************************************************************
 * Called at node initialization time to provide provisioning information
 *
 ******************************************************************************/
void sl_btmesh_on_provision_init_status(bool provisioned,
                                        uint16_t address,
                                        uint32_t iv_index)
{
  if (provisioned) {
    app_show_btmesh_node_provisioned(address, iv_index);
  } else {
    app_log("BT mesh node is unprovisioned, started unprovisioned beaconing...\r\n");
    sl_status_t lcd_status = sl_btmesh_LCD_write("unprovisioned",
                                                 SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
    app_log_status_level_f(APP_LOG_LEVEL_ERROR, lcd_status, "LCD write failed");
  }
}

/*******************************************************************************
 * Called when the Provisioning starts
 *
 ******************************************************************************/
void app_show_btmesh_node_provisioning_started(uint16_t result)
{
  app_log("BT mesh node provisioning is started (result: 0x%04x)\r\n", result);
  sl_status_t lcd_status = sl_btmesh_LCD_write("provisioning...",
                                               SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, lcd_status, "LCD write failed");
  (void)result;
}

/*******************************************************************************
 * Called when the Provisioning finishes successfully
 *
 ******************************************************************************/
void app_show_btmesh_node_provisioned(uint16_t address,
                                      uint32_t iv_index)
{
  app_log("BT mesh node is provisioned (address: 0x%04x, iv_index: 0x%lx)\r\n",
          address,
          iv_index);
  sl_status_t lcd_status = sl_btmesh_LCD_write("provisioned",
                                               SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, lcd_status, "LCD write failed");
  (void)address;
  (void)iv_index;
}

/*******************************************************************************
 * Called when the Provisioning fails
 *
 ******************************************************************************/
void sl_btmesh_on_node_provisioning_failed(uint16_t result)
{
  app_log("BT mesh node provisioning failed (result: 0x%04x)\r\n", result);
  sl_status_t lcd_status = sl_btmesh_LCD_write("prov failed...",
                                               SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, lcd_status, "LCD write failed");
  (void)result;
}

// -----------------------------------------------------------------------------
// Factory Reset Callbacks

/*******************************************************************************
 * Called when full reset is established, before system reset
 ******************************************************************************/
void sl_btmesh_factory_reset_on_full_reset(void)
{
  app_log("Factory reset\r\n");
  sl_status_t status = sl_btmesh_LCD_write("Factory reset",
                                           SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
}

/*******************************************************************************
 * Called when node reset is established, before system reset
 ******************************************************************************/
void sl_btmesh_factory_reset_on_node_reset(void)
{
  app_log("Node reset\r\n");
  sl_status_t status = sl_btmesh_LCD_write("Node reset",
                                           SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
}
