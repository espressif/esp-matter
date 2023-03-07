/***************************************************************************//**
 * @file
 * @brief Application Output LCD code
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

#include <stdbool.h>
#include <stdio.h>
#include "em_common.h"
#include "sl_status.h"

#include "app.h"
#include "app_log.h"

#include "sl_btmesh_api.h"
#include "sl_btmesh_wstk_lcd.h"

#include "sl_btmesh_factory_reset.h"
#include "sl_btmesh_sensor_server.h"
#include "sl_btmesh_sensor_people_count.h"
#include "sl_btmesh_sensor_people_count_config.h"

/// Integer part of illuminance
#define INT_ILLUM(x)  (x / 100)
/// Fractional part of illuminance
#define FRAC_ILLUM(x) (x % 100)
/// Integer part of temperature
#define INT_TEMP(x)   (x / 2)
/// Fractional part of temperature
#define FRAC_TEMP(x)  ((x * 5) % 10)

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

// -----------------------------------------------------------------------------
// Sensor Server Callbacks

/*******************************************************************************
 *  Called when a temperature measurement is done
 ******************************************************************************/
void sl_btmesh_sensor_server_on_temperature_measurement(temperature_8_t temperature)
{
  sl_status_t status = SL_STATUS_OK;
  if ((temperature_8_t)SL_BTMESH_SENSOR_TEMPERATURE_VALUE_UNKNOWN
      == temperature) {
    app_log("Temperature: UNKNOWN\r\n");
    status = sl_btmesh_LCD_write("Temperature: N/K ",
                                 SL_BTMESH_WSTK_LCD_ROW_TEMPERATURE_CFG_VAL);
  } else {
    char str[LCD_ROW_LEN];
    app_log("Temperature:    %3d.%1d  °C\r\n",
            INT_TEMP(temperature),
            FRAC_TEMP(temperature));
    snprintf(str,
             LCD_ROW_LEN,
             "Temperature: %3d.%1d C",
             INT_TEMP(temperature),
             FRAC_TEMP(temperature));
    status = sl_btmesh_LCD_write(str, SL_BTMESH_WSTK_LCD_ROW_TEMPERATURE_CFG_VAL);
  }
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
}
/*******************************************************************************
 *  Called when a light measurement is done
 ******************************************************************************/
void sl_btmesh_sensor_server_on_light_measurement(illuminance_t light)
{
  sl_status_t status = SL_STATUS_OK;
  if ((illuminance_t)SL_BTMESH_SENSOR_LIGHT_VALUE_UNKNOWN == light) {
    app_log("Illuminance: UNKNOWN\r\n");
    status = sl_btmesh_LCD_write("Illuminance: N/K",
                                 SL_BTMESH_WSTK_LCD_ROW_ILLUMINANCE_CFG_VAL);
  } else {
    app_log("Illuminance:   %4lu.%02lu lx\r\n",
            INT_ILLUM(light),
            FRAC_ILLUM(light));
    char str[LCD_ROW_LEN];
    snprintf(str,
             LCD_ROW_LEN,
             "Illum: %4u.%02u lx",
             (uint16_t)(INT_ILLUM(light)),
             (uint8_t)(FRAC_ILLUM(light)));
    app_log(str);
    status = sl_btmesh_LCD_write(str, SL_BTMESH_WSTK_LCD_ROW_ILLUMINANCE_CFG_VAL);
  }
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
}

/*******************************************************************************
 *  Called when a people count measurement is done
 ******************************************************************************/
void sl_btmesh_sensor_server_on_people_count_measurement(count16_t people)
{
  sl_status_t status = SL_STATUS_OK;
  if ((count16_t)SL_BTMESH_SENSOR_PEOPLE_COUNT_VALUE_IS_NOT_KNOWN
      == people) {
    app_log("People count: UNKNOWN\r\n");
    status = sl_btmesh_LCD_write("People count:     N/K",
                                 SL_BTMESH_WSTK_LCD_ROW_PEOPLE_COUNT_CFG_VAL);
  } else {
    app_log("People count: %5u\r\n", people);
    char str[LCD_ROW_LEN];
    snprintf(str,
             LCD_ROW_LEN,
             "People count: %5u",
             people);
    status = sl_btmesh_LCD_write(str, SL_BTMESH_WSTK_LCD_ROW_PEOPLE_COUNT_CFG_VAL);
  }
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
}

// -----------------------------------------------------------------------------
// Provisioning Decorator Callbacks

/*******************************************************************************
 *  Called at node initialization time to provide provisioning information
 ******************************************************************************/
void sl_btmesh_on_provision_init_status(bool provisioned,
                                        uint16_t address,
                                        uint32_t iv_index)
{
  if (provisioned) {
    app_show_btmesh_node_provisioned(address, iv_index);
  } else {
    app_log("BT mesh node is unprovisioned, started unprovisioned beaconing...\r\n");
    sl_status_t status = sl_btmesh_LCD_write("unprovisioned",
                                             SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
    app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
  }
}

/*******************************************************************************
 *  Called from sl_btmesh_on_node_provisioning_started callback in app.c
 ******************************************************************************/
void app_show_btmesh_node_provisioning_started(uint16_t result)
{
  app_log("BT mesh node provisioning is started (result: 0x%04x)\r\n", result);
  sl_status_t status = sl_btmesh_LCD_write("provisioning...",
                                           SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
  (void)result;
}

/*******************************************************************************
 *  Called from sl_btmesh_on_node_provisioned callback in app.c
 ******************************************************************************/
void app_show_btmesh_node_provisioned(uint16_t address,
                                      uint32_t iv_index)
{
  app_log("BT mesh node is provisioned (address: 0x%04x, iv_index: 0x%lx)\r\n",
          address,
          iv_index);
  sl_status_t status = sl_btmesh_LCD_write("provisioned",
                                           SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
  (void)address;
  (void)iv_index;
}

/*******************************************************************************
 *  Called when the Provisioning fails
 ******************************************************************************/
void sl_btmesh_on_node_provisioning_failed(uint16_t result)
{
  app_log("BT mesh node provisioning failed (result: 0x%04x)\r\n", result);
  sl_status_t status = sl_btmesh_LCD_write("prov failed...",
                                           SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
  (void)result;
}
