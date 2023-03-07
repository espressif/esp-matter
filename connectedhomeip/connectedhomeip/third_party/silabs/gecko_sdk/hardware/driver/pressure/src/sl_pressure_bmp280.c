/***************************************************************************//**
 * @file
 * @brief Driver for the Bosch Sensortec BMP280 barometric pressure sensor
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

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "bmp280.h"
#include "sl_bmp280.h"
#include "sl_pressure.h"

static uint8_t pressure_sampling_period_to_register_value(float sampling_period,
                                                       sl_pressure_oversampling_t osr);

/***************************************************************************//**
 * @brief
 *    Initialize the barometric pressure sensor.
 ******************************************************************************/
sl_status_t sl_pressure_init(sl_i2cspm_t *i2cspm)
{
  return sl_bmp280_init(i2cspm);
}

/***************************************************************************//**
 * @brief
 *    De-initialize the barometric pressure sensor.
 ******************************************************************************/
void sl_pressure_deinit(sl_i2cspm_t *i2cspm)
{
  sl_bmp280_deinit(i2cspm);
}

/***************************************************************************//**
 * @brief
 *    Configure the barometric pressure sensor.
 ******************************************************************************/
sl_status_t sl_pressure_configure(sl_i2cspm_t *i2cspm, sl_pressure_config_t *cfg)
{
  sl_bmp280_config_t bmp280_config;

  switch (cfg->power_mode) {
    case SLEEP_MODE:
      bmp280_config.power_mode = BMP280_SLEEP_MODE;
      break;
    case FORCED_MODE:
      bmp280_config.power_mode = BMP280_FORCED_MODE;
      break;
    case NORMAL_MODE:
      bmp280_config.power_mode = BMP280_NORMAL_MODE;
      break;
    default:
      EFM_ASSERT(false);
      return SL_STATUS_FAIL;
  }

  switch (cfg->oversampling) {
    case LOW_POWER:
      bmp280_config.oversampling = BMP280_OVERSAMP_1X;
      break;
    case STANDARD_RESOLUTION:
      bmp280_config.oversampling = BMP280_OVERSAMP_4X;
      break;
    case HIGH_RESOLUTION:
      bmp280_config.oversampling = BMP280_OVERSAMP_8X;
      break;
    default:
      EFM_ASSERT(false);
      return SL_STATUS_FAIL;
  }

  bmp280_config.standby_time = pressure_sampling_period_to_register_value((float)cfg->sampling_period,
                                                                       cfg->oversampling);

  return sl_bmp280_configure(i2cspm, &bmp280_config);
}

/***************************************************************************//**
 * @brief
 *    Initiate temperature measurement on the barometric pressure sensor and
 *    read temperature from it.
 ******************************************************************************/
sl_status_t sl_pressure_measure_temperature(sl_i2cspm_t *i2cspm, float *temperature)
{
  return sl_bmp280_measure_temperature(i2cspm, temperature);
}

/***************************************************************************//**
 * @brief
 *    Initiate pressure measurement on the barometric pressure sensor and read
 *    pressure from it.
 ******************************************************************************/
sl_status_t sl_pressure_measure_pressure(sl_i2cspm_t *i2cspm, float *pressure)
{
  sl_status_t status;
  status = sl_bmp280_measure_pressure(i2cspm, pressure);

  // Convert pressure value to Pascal.
  *pressure = *pressure * 100;
  return status;
}

// Convert sampling period from ms to register value to be stored in standby
// time register. If sampling period is out of range, it set the nearest value.
static uint8_t pressure_sampling_period_to_register_value(float sampling_period,
                                                       sl_pressure_oversampling_t osr)
{
  // Subtract measurement time
  if (osr == LOW_POWER) {
    sampling_period -= 7.5f;
  } else if (osr == STANDARD_RESOLUTION) {
    sampling_period -= 11.5f;
  } else if (osr == HIGH_RESOLUTION) {
    sampling_period -= 19.5f;
  }

  // Return the nearest possible integer value. Check BMP280 datasheet for possible
  // sampling periods.
  if (sampling_period <= 31.5f) {
    return 0UL;
  } else if (sampling_period < 62.5f) {
    return 1UL;
  } else if (sampling_period > 4000.0f) {
    return 7UL;
  } else {
    // Use this formula to get register value that gives the sampling period.
    return (uint8_t)round(log(sampling_period / 62.5f) / log(2)) + 1;
  }
}
