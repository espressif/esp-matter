/***************************************************************************//**
 * @file
 * @brief Driver for the Bosch Sensortec BMP3XX barometric pressure sensor
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

#include "bmp3_defs.h"
#include "sl_bmp3xx.h"
#include "sl_pressure.h"

static uint8_t pressure_sampling_period_to_register_value(int sampling_period);

/***************************************************************************//**
 * @brief
 *    Initialize the barometric pressure sensor.
 ******************************************************************************/
sl_status_t sl_pressure_init(sl_i2cspm_t *i2cspm)
{
  return sl_bmp3xx_init(i2cspm);
}

/***************************************************************************//**
 * @brief
 *    De-initialize the barometric pressure sensor.
 ******************************************************************************/
void sl_pressure_deinit(sl_i2cspm_t *i2cspm)
{
  sl_bmp3xx_deinit(i2cspm);
}

/***************************************************************************//**
 * @brief
 *    Configure the barometric pressure sensor.
 ******************************************************************************/
sl_status_t sl_pressure_configure(sl_i2cspm_t *i2cspm, sl_pressure_config_t *cfg)
{
  sl_bmp3xx_config_t bmp3xx_config;

  sl_bmp3xx_get_config(i2cspm, &bmp3xx_config);

  switch (cfg->power_mode) {
    case SLEEP_MODE:
      bmp3xx_config.settings.op_mode = BMP3_MODE_SLEEP;
      break;
    case FORCED_MODE:
      bmp3xx_config.settings.op_mode = BMP3_MODE_FORCED;
      break;
    case NORMAL_MODE:
      bmp3xx_config.settings.op_mode = BMP3_MODE_NORMAL;
      break;
    default:
      EFM_ASSERT(false);
      return SL_STATUS_FAIL;
  }

  switch (cfg->oversampling) {
    case LOW_POWER:
      bmp3xx_config.settings.odr_filter.press_os = BMP3_NO_OVERSAMPLING;
      bmp3xx_config.settings.odr_filter.temp_os = BMP3_NO_OVERSAMPLING;
      break;
    case STANDARD_RESOLUTION:
      bmp3xx_config.settings.odr_filter.press_os = BMP3_OVERSAMPLING_8X;
      bmp3xx_config.settings.odr_filter.temp_os = BMP3_OVERSAMPLING_8X;
      break;
    case HIGH_RESOLUTION:
      bmp3xx_config.settings.odr_filter.press_os = BMP3_OVERSAMPLING_16X;
      bmp3xx_config.settings.odr_filter.temp_os = BMP3_OVERSAMPLING_16X;
      break;
    default:
      EFM_ASSERT(false);
      return SL_STATUS_FAIL;
  }

  bmp3xx_config.settings.odr_filter.odr = pressure_sampling_period_to_register_value(cfg->sampling_period);

  bmp3xx_config.settings.press_en = BMP3_ENABLE;
  bmp3xx_config.settings.temp_en = BMP3_ENABLE;

  bmp3xx_config.desired_settings = BMP3_SEL_PRESS_EN
                                   | BMP3_SEL_TEMP_EN
                                   | BMP3_SEL_PRESS_OS
                                   | BMP3_SEL_TEMP_OS
                                   | BMP3_SEL_ODR;

  return sl_bmp3xx_set_config(i2cspm, &bmp3xx_config);
}

/***************************************************************************//**
 * @brief
 *    Initiate temperature measurement on the barometric pressure sensor and
 *    read temperature from it.
 ******************************************************************************/
sl_status_t sl_pressure_measure_temperature(sl_i2cspm_t *i2cspm, float *temperature)
{
  return sl_bmp3xx_measure_temperature(i2cspm, temperature);
}

/***************************************************************************//**
 * @brief
 *    Initiate pressure measurement on the barometric pressure sensor and read
 *    pressure from it.
 ******************************************************************************/
sl_status_t sl_pressure_measure_pressure(sl_i2cspm_t *i2cspm, float *pressure)
{
  return sl_bmp3xx_measure_pressure(i2cspm, pressure);
}

// Convert sampling value from ms to register value to be stored in odr register.
// If sampling period is out of range, it set the nearest value.
static uint8_t pressure_sampling_period_to_register_value(int sampling_period)
{
  // Return the nearest integer. Check BMP3XX datasheet for possible
  // sampling periods.
  if (sampling_period < 5) {
    return 0x00;
  } else if (sampling_period > 655360) {
    return 0x11;
  } else {
    // Use this formula to get register value that gives the sampling period.
    // Refer to datasheet sampling periods table for more details.
    return (uint8_t)round(log((float)sampling_period / 5) / log(2));
  }
}
