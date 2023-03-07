/***************************************************************************//**
 * @file
 * @brief Core application logic.
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
#include "em_emu.h"
#include "em_gpio.h"
#include "sl_status.h"
#include "sl_simple_button_instances.h"
#include "app_log.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "sl_simple_timer.h"
#include "gatt_db.h"
#include "advertise.h"
#include "sl_power_supply.h"
#include "board.h"
#include "app.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_GATT_SERVICE_AIO_PRESENT
#include "sl_gatt_service_aio.h"
#endif // SL_CATALOG_GATT_SERVICE_AIO_PRESENT
#ifdef SL_CATALOG_GATT_SERVICE_BATTERY_PRESENT
#include "sl_gatt_service_battery.h"
#endif // SL_CATALOG_GATT_SERVICE_BATTERY_PRESENT
#ifdef SL_CATALOG_GATT_SERVICE_HALL_PRESENT
#include "sl_gatt_service_hall.h"
#endif // SL_CATALOG_GATT_SERVICE_HALL_PRESENT
#ifdef SL_CATALOG_SENSOR_HALL_PRESENT
#include "sl_sensor_hall.h"
#endif // SL_CATALOG_SENSOR_HALL_PRESENT
#ifdef SL_CATALOG_GATT_SERVICE_LIGHT_PRESENT
#include "sl_gatt_service_light.h"
#endif // SL_CATALOG_GATT_SERVICE_LIGHT_PRESENT
#ifdef SL_CATALOG_SENSOR_LIGHT_PRESENT
#include "sl_sensor_light.h"
#endif // SL_CATALOG_SENSOR_LIGHT_PRESENT
#ifdef SL_CATALOG_GATT_SERVICE_LUX_PRESENT
#include "sl_gatt_service_lux.h"
#endif // SL_CATALOG_GATT_SERVICE_LUX_PRESENT
#ifdef SL_CATALOG_SENSOR_LUX_PRESENT
#include "sl_sensor_lux.h"
#endif // SL_CATALOG_SENSOR_LUX_PRESENT
#ifdef SL_CATALOG_GATT_SERVICE_RHT_PRESENT
#include "sl_gatt_service_rht.h"
#endif // SL_CATALOG_GATT_SERVICE_RHT_PRESENT
#ifdef SL_CATALOG_SENSOR_RHT_PRESENT
#include "sl_sensor_rht.h"
#endif // SL_CATALOG_SENSOR_RHT_PRESENT
#ifdef SL_CATALOG_GATT_SERVICE_IMU_PRESENT
#include "sl_gatt_service_imu.h"
#endif // SL_CATALOG_GATT_SERVICE_IMU_PRESENT
#ifdef SL_CATALOG_SENSOR_IMU_PRESENT
#include "sl_sensor_imu.h"
#endif // SL_CATALOG_SENSOR_IMU_PRESENT
#ifdef SL_CATALOG_GATT_SERVICE_RGB_PRESENT
#include "sl_gatt_service_rgb.h"
#endif // SL_CATALOG_GATT_SERVICE_RGB_PRESENT
#ifdef SL_CATALOG_GATT_SERVICE_PRESSURE_PRESENT
#include "sl_gatt_service_pressure.h"
#endif // SL_CATALOG_GATT_SERVICE_PRESSURE_PRESENT
#ifdef SL_CATALOG_SENSOR_PRESSURE_PRESENT
#include "sl_sensor_pressure.h"
#endif // SL_CATALOG_SENSOR_PRESSURE_PRESENT
#ifdef SL_CATALOG_GATT_SERVICE_GAS_PRESENT
#include "sl_gatt_service_gas.h"
#endif // SL_CATALOG_GATT_SERVICE_GAS_PRESENT
#ifdef SL_CATALOG_SENSOR_GAS_PRESENT
#include "sl_sensor_gas.h"
#endif // SL_CATALOG_SENSOR_GAS_PRESENT
#ifdef SL_CATALOG_GATT_SERVICE_SOUND_PRESENT
#include "sl_gatt_service_sound.h"
#endif // SL_CATALOG_GATT_SERVICE_SOUND_PRESENT
#ifdef SL_CATALOG_SENSOR_SOUND_PRESENT
#include "sl_sensor_sound.h"
#endif // SL_CATALOG_SENSOR_SOUND_PRESENT

// -----------------------------------------------------------------------------
// Macros

#define SHUTDOWN_TIMEOUT_MS             60000
#define FW_REV_STR_LEN                  6
// -----------------------------------------------------------------------------
// Private variables

// Timer
static sl_simple_timer_t shutdown_timer;

// -----------------------------------------------------------------------------
// Private function declarations

static void shutdown_start_timer(void);
static void shutdown_stop_timer(void);
static void shutdown(sl_simple_timer_t *timer, void *data);
static void sensor_init(void);
static void sensor_deinit(void);

// -----------------------------------------------------------------------------
// Public function definitions

void app_init(void)
{
  app_log_info("Thuderboard demo initialised" APP_LOG_NL);
  sl_power_supply_probe();
  shutdown_start_timer();
#if defined(BOARD_RGBLED_COUNT) && (BOARD_RGBLED_COUNT > 0)
  rgb_led_init();
#endif // BOARD_RGBLED_COUNT
}

SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

// -----------------------------------------------------------------------------
// Bluetooth event handler

void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;
  uint32_t unique_id;
  uint8_t system_id[8];

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    case sl_bt_evt_system_boot_id: {
      // Set Firmware Revision string. Use the BLE stack version.
      char fw_rev[FW_REV_STR_LEN];
      snprintf(fw_rev,
               FW_REV_STR_LEN,
               "%1d.%1d.%1d",
               evt->data.evt_system_boot.major,
               evt->data.evt_system_boot.minor,
               evt->data.evt_system_boot.patch);
      sc = sl_bt_gatt_server_write_attribute_value(gattdb_firmware_rev,
                                                   0,
                                                   FW_REV_STR_LEN - 1,
                                                   (uint8_t *)fw_rev);
      app_log_status_error(sc);
      // Print boot message.
      app_log_info("Bluetooth stack booted: v%d.%d.%d-b%d" APP_LOG_NL,
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      app_assert_status(sc);
      app_log_info("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X",
                   address_type ? "static random" : "public device" APP_LOG_NL,
                   address.addr[5],
                   address.addr[4],
                   address.addr[3],
                   address.addr[2],
                   address.addr[1],
                   address.addr[0]);
      unique_id = 0xFFFFFF & *((uint32_t*) address.addr);

      // Pad and reverse unique ID to get System ID
      system_id[0] = address.addr[5];
      system_id[1] = address.addr[4];
      system_id[2] = address.addr[3];
      system_id[3] = 0xFF;
      system_id[4] = 0xFE;
      system_id[5] = address.addr[2];
      system_id[6] = address.addr[1];
      system_id[7] = address.addr[0];

      sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                                   0,
                                                   sizeof(system_id),
                                                   system_id);
      app_assert_status(sc);
      advertise_init(unique_id);
      break;
    }

    // -------------------------------
    case sl_bt_evt_connection_opened_id:
      app_log_info("Connection opened" APP_LOG_NL);
      advertise_stop();
      shutdown_stop_timer();
      sensor_init();
      break;

    // -------------------------------
    case sl_bt_evt_connection_closed_id:
      app_log_info("Connection closed" APP_LOG_NL);
      shutdown_start_timer();
      sensor_deinit();
      advertise_start();
      break;

    // -------------------------------
    default:
      break;
  }
}

// -----------------------------------------------------------------------------
// Push button event handler

void sl_button_on_change(const sl_button_t *handle)
{
  (void)handle;
#ifdef SL_CATALOG_GATT_SERVICE_AIO_PRESENT
  sl_gatt_service_aio_on_change();
#endif // SL_CATALOG_GATT_SERVICE_AIO_PRESENT
}

// -----------------------------------------------------------------------------
// Shutdown feature function definitions

static void shutdown(sl_simple_timer_t *timer, void *data)
{
  (void)timer;
  (void)data;
  EMU_EM4Init_TypeDef em4_init = EMU_EM4INIT_DEFAULT;
  em4_init.pinRetentionMode = emuPinRetentionEm4Exit;
  EMU_EM4Init(&em4_init);

  // Set up EM4 wake-up for the chosen button. Need to enable glitch filter.
  sl_simple_button_context_t *button = BOARD_EM4WUEN_BTN.context;
  GPIO_PinModeSet(button->port, button->pin, gpioModeInputPullFilter, 1);
  GPIO_EM4EnablePinWakeup((BOARD_EM4WUEN_MASK << _GPIO_EM4WUEN_EM4WUEN_SHIFT), 0);

  advertise_stop();

  EMU_EnterEM4();
}

static void shutdown_start_timer(void)
{
  sl_status_t sc;
  if (sl_power_supply_is_low_power()) {
    sc = sl_simple_timer_start(&shutdown_timer, SHUTDOWN_TIMEOUT_MS, shutdown, NULL, false);
    app_assert_status(sc);
  }
}

static void shutdown_stop_timer(void)
{
  sl_status_t sc;
  sc = sl_simple_timer_stop(&shutdown_timer);
  app_assert_status(sc);
}

// -----------------------------------------------------------------------------
// Sensor batch init/deinit

static void sensor_init(void)
{
  sl_status_t sc;
#ifdef SL_CATALOG_SENSOR_HALL_PRESENT
  sc = sl_sensor_hall_init();
  if (sc != SL_STATUS_OK) {
    app_log_warning("Hall sensor initialization failed." APP_LOG_NL);
  }
#endif // SL_CATALOG_SENSOR_HALL_PRESENT
#ifdef SL_CATALOG_SENSOR_LIGHT_PRESENT
  sc = sl_sensor_light_init();
  if (sc != SL_STATUS_OK) {
    app_log_warning("Ambient light and UV index sensor initialization failed." APP_LOG_NL);
  }
#endif // SL_CATALOG_SENSOR_LIGHT_PRESENT
#ifdef SL_CATALOG_SENSOR_LUX_PRESENT
  sc = sl_sensor_lux_init();
  if (sc != SL_STATUS_OK) {
    app_log_warning("Ambient light sensor initialization failed." APP_LOG_NL);
  }
#endif // SL_CATALOG_SENSOR_LUX_PRESENT
#ifdef SL_CATALOG_SENSOR_RHT_PRESENT
  sc = sl_sensor_rht_init();
  if (sc != SL_STATUS_OK) {
    app_log_warning("Relative Humidity and Temperature sensor initialization failed." APP_LOG_NL);
  }
#endif // SL_CATALOG_SENSOR_RHT_PRESENT
#ifdef SL_CATALOG_SENSOR_IMU_PRESENT
  sl_sensor_imu_init();
#endif // SL_CATALOG_SENSOR_IMU_PRESENT
#ifdef SL_CATALOG_SENSOR_PRESSURE_PRESENT
  sc = sl_sensor_pressure_init();
  if (sc != SL_STATUS_OK) {
    app_log_warning("Air Pressure sensor initialization failed." APP_LOG_NL);
  }
#endif // SL_CATALOG_SENSOR_PRESSURE_PRESENT
#ifdef SL_CATALOG_SENSOR_GAS_PRESENT
  if (!sl_power_supply_is_low_power()) {
    sc = sl_sensor_gas_init();
    if (sc != SL_STATUS_OK) {
      app_log_warning("Air quality sensor initialization failed." APP_LOG_NL);
    }
  }
#endif // SL_CATALOG_SENSOR_GAS_PRESENT
#ifdef SL_CATALOG_SENSOR_SOUND_PRESENT
  sc = sl_sensor_sound_init();
  if (sc != SL_STATUS_OK) {
    app_log_warning("Sound level sensor initialization failed." APP_LOG_NL);
  }
#endif // SL_CATALOG_SENSOR_SOUND_PRESENT
}

static void sensor_deinit(void)
{
#ifdef SL_CATALOG_SENSOR_HALL_PRESENT
  sl_sensor_hall_deinit();
#endif // SL_CATALOG_SENSOR_HALL_PRESENT
#ifdef SL_CATALOG_SENSOR_LIGHT_PRESENT
  sl_sensor_light_deinit();
#endif // SL_CATALOG_SENSOR_LIGHT_PRESENT
#ifdef SL_CATALOG_SENSOR_LUX_PRESENT
  sl_sensor_lux_deinit();
#endif // SL_CATALOG_SENSOR_LUX_PRESENT
#ifdef SL_CATALOG_SENSOR_RHT_PRESENT
  sl_sensor_rht_deinit();
#endif // SL_CATALOG_SENSOR_RHT_PRESENT
#ifdef SL_CATALOG_SENSOR_IMU_PRESENT
  sl_sensor_imu_deinit();
#endif // SL_CATALOG_SENSOR_IMU_PRESENT
#if defined(BOARD_RGBLED_COUNT) && (BOARD_RGBLED_COUNT > 0)
  rgb_led_deinit();
#endif // BOARD_RGBLED_COUNT
#ifdef SL_CATALOG_SENSOR_PRESSURE_PRESENT
  sl_sensor_pressure_deinit();
#endif // SL_CATALOG_SENSOR_PRESSURE_PRESENT
#ifdef SL_CATALOG_SENSOR_GAS_PRESENT
  if (!sl_power_supply_is_low_power()) {
    sl_sensor_gas_deinit();
  }
#endif // SL_CATALOG_SENSOR_GAS_PRESENT
#ifdef SL_CATALOG_SENSOR_SOUND_PRESENT
  sl_sensor_sound_deinit();
#endif // SL_CATALOG_SENSOR_SOUND_PRESENT
}

// -----------------------------------------------------------------------------
// Connect GATT services with sensors by overriding weak functions

#ifdef SL_CATALOG_GATT_SERVICE_BATTERY_PRESENT
uint8_t sl_gatt_service_battery_get_level(void)
{
  uint8_t bat_level;
  bat_level = sl_power_supply_get_battery_level();
  app_log_info("Battery level = %d %%" APP_LOG_NL, bat_level);
  return bat_level;
}

uint8_t sl_gatt_service_battery_get_type(void)
{
  return sl_power_supply_get_type();
}
#endif // SL_CATALOG_GATT_SERVICE_BATTERY_PRESENT

#if defined(SL_CATALOG_GATT_SERVICE_HALL_PRESENT) && defined(SL_CATALOG_SENSOR_HALL_PRESENT)
sl_status_t sl_gatt_service_hall_get(float *field_strength, bool *alert, bool *tamper)
{
  sl_status_t sc;
  sc = sl_sensor_hall_get(field_strength, alert, tamper);
  if (SL_STATUS_OK == sc) {
    app_log_info("Magnetic Flux = %4.3f mT" APP_LOG_NL, *field_strength);
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    app_log_info("Hall sensor is not initialized." APP_LOG_NL);
  } else {
    app_log_status_error_f(sc, "Hall sensor measurement failed" APP_LOG_NL);
  }
  return sc;
}
#endif

#if defined(SL_CATALOG_GATT_SERVICE_LIGHT_PRESENT) && defined(SL_CATALOG_SENSOR_LIGHT_PRESENT)
sl_status_t sl_gatt_service_light_get(float *lux, float *uvi)
{
  sl_status_t sc;
  sc = sl_sensor_light_get(lux, uvi);
  if (SL_STATUS_OK == sc) {
    app_log_info("Amb light = %f Lux" APP_LOG_NL, *lux);
    app_log_info("UV Index = %d" APP_LOG_NL, *uvi);
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    app_log_info("Ambient light and UV index sensor is not initialized." APP_LOG_NL);
  } else {
    app_log_status_error_f(sc, "Light sensor measurement failed" APP_LOG_NL);
  }
  return sc;
}
#endif

#if defined(SL_CATALOG_GATT_SERVICE_LUX_PRESENT) && defined(SL_CATALOG_SENSOR_LUX_PRESENT)
sl_status_t sl_gatt_service_lux_get(float *lux)
{
  sl_status_t sc;
  sc = sl_sensor_lux_get(lux);
  if (SL_STATUS_OK == sc) {
    app_log_info("Amb light = %f Lux\r\n", *lux);
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    app_log_info("Ambient light sensor is not initialized." APP_LOG_NL);
  } else {
    app_log_status_error_f(sc, "Light sensor measurement failed\n");
  }
  return sc;
}
#endif

#if defined(SL_CATALOG_GATT_SERVICE_RHT_PRESENT) && defined(SL_CATALOG_SENSOR_RHT_PRESENT)
sl_status_t sl_gatt_service_rht_get(uint32_t *rh, int32_t *t)
{
  sl_status_t sc;
  sc = sl_sensor_rht_get(rh, t);
  if (SL_STATUS_OK == sc) {
    app_log_info("Humidity = %3.2f %%RH" APP_LOG_NL, (float)*rh / 1000.0f);
    app_log_info("Temp = %3.2f C" APP_LOG_NL, (float)*t / 1000.0f);
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    app_log_info("Relative Humidity and Temperature sensor is not initialized." APP_LOG_NL);
  } else {
    app_log_status_error_f(sc, "RHT sensor measurement failed" APP_LOG_NL);
  }
  return sc;
}
#endif

#if defined(SL_CATALOG_GATT_SERVICE_IMU_PRESENT) && defined(SL_CATALOG_SENSOR_IMU_PRESENT)
sl_status_t sl_gatt_service_imu_get(int16_t ovec[3], int16_t avec[3])
{
  sl_status_t sc;
  sc = sl_sensor_imu_get(ovec, avec);
  if (SL_STATUS_OK == sc) {
    app_log_info("IMU: ORI : %04d,%04d,%04d" APP_LOG_NL, ovec[0], ovec[1], ovec[2]);
    app_log_info("IMU: ACC : %04d,%04d,%04d" APP_LOG_NL, avec[0], avec[1], avec[2]);
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    app_log_info("Inertial Measurement Unit sensor is not initialized." APP_LOG_NL);
  }
  return sc;
}

sl_status_t sl_gatt_service_imu_calibrate(void)
{
  sl_status_t sc;
  sc = sl_sensor_imu_calibrate();
  if (SL_STATUS_NOT_INITIALIZED == sc) {
    app_log_info("Inertial Measurement Unit sensor is not initialized." APP_LOG_NL);
  } else {
    app_log_info("IMU calibration status: %d" APP_LOG_NL, sc);
  }
  return sc;
}

void sl_gatt_service_imu_enable(bool enable)
{
  sl_status_t sc;
  app_log_info("IMU %sable" APP_LOG_NL, enable ? "en" : "dis");
  sc = sl_sensor_imu_enable(enable);
  if (enable && SL_STATUS_OK != sc) {
    app_log_warning("Inertial Measurement Unit sensor sensor initialization failed." APP_LOG_NL);
  }
}
#endif

#if defined(SL_CATALOG_GATT_SERVICE_RGB_PRESENT) && defined(BOARD_RGBLED_COUNT) && (BOARD_RGBLED_COUNT > 0)
void sl_gatt_service_rgb_set_led(uint8_t m, uint8_t r, uint8_t g, uint8_t b)
{
  if (!sl_power_supply_is_low_power()) {
    rgb_led_set(m, r, g, b);
    app_log_info("RGBLED write: m:%02x r:%02x g:%02x b:%02x" APP_LOG_NL, m, r, g, b);
  }
}

uint8_t sl_gatt_service_rgb_get_led_mask(void)
{
  return BOARD_RGBLED_MASK;
}
#endif

#if defined(SL_CATALOG_GATT_SERVICE_PRESSURE_PRESENT) && defined(SL_CATALOG_SENSOR_PRESSURE_PRESENT)
sl_status_t sl_gatt_service_pressure_get(float *pressure)
{
  sl_status_t sc;
  sc = sl_sensor_pressure_get(pressure);
  if (SL_STATUS_OK == sc) {
    app_log_info("Pressure = %0.3f mbar" APP_LOG_NL, *pressure);
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    app_log_info("Air Pressure sensor is not initialized." APP_LOG_NL);
  } else {
    app_log_status_error_f(sc, "Pressure sensor measurement failed" APP_LOG_NL);
  }
  return sc;
}
#endif

#if defined(SL_CATALOG_GATT_SERVICE_GAS_PRESENT) && defined(SL_CATALOG_SENSOR_GAS_PRESENT)
sl_status_t sl_gatt_service_gas_get(uint16_t *eco2, uint16_t *tvoc)
{
  sl_status_t sc = SL_STATUS_NOT_AVAILABLE;
  if (!sl_power_supply_is_low_power()) {
    sc = sl_sensor_gas_get(eco2, tvoc);
    if (SL_STATUS_OK == sc) {
      app_log_info("eCO2 = %u ppm" APP_LOG_NL, (uint16_t)*eco2);
      app_log_info("TVOC = %u ppd" APP_LOG_NL, (uint16_t)*tvoc);
    } else if (SL_STATUS_NOT_INITIALIZED == sc) {
      app_log_info("Air quality sensor is not initialized." APP_LOG_NL);
    } else if (SL_STATUS_NOT_READY != sc) {
      app_log_status_error_f(sc, "Air quality sensor measurement failed" APP_LOG_NL);
    }
  }
  return sc;
}
#endif

#if defined(SL_CATALOG_GATT_SERVICE_SOUND_PRESENT) && defined(SL_CATALOG_SENSOR_SOUND_PRESENT)
sl_status_t sl_gatt_service_sound_get(float *sound_level)
{
  sl_status_t sc;
  sc = sl_sensor_sound_get(sound_level);
  if (SL_STATUS_OK == sc) {
    app_log_info("Sound Level = %3.2f dBA" APP_LOG_NL, *sound_level);
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    app_log_info("Sound level sensor is not initialized." APP_LOG_NL);
  } else {
    app_log_status_error_f(sc, "Sound level measurement failed" APP_LOG_NL);
  }
  return sc;
}
#endif
