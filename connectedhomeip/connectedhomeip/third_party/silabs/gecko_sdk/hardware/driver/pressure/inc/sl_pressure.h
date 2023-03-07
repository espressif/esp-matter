/***************************************************************************//**
 * @file
 * @brief Driver for pressure sensor
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

#ifndef SL_PRESSURE_H
#define SL_PRESSURE_H

#include "sl_i2cspm.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @brief
 *    Enum to set oversampling value.
 *
 ******************************************************************************/
typedef enum {
  LOW_POWER,
  STANDARD_RESOLUTION,
  HIGH_RESOLUTION,
} sl_pressure_oversampling_t;

/***************************************************************************//**
 * @brief
 *    Enum to set power mode value.
 *
 ******************************************************************************/
typedef enum {
  SLEEP_MODE,
  FORCED_MODE,
  NORMAL_MODE,
} sl_pressure_power_mode_t;

/***************************************************************************//**
 * @brief
 *    Structure to configure the pressure sensor.
 *
 * @note
 *    It is important to note that not all sampling period are reachable. Sensor
 *    register allow a small subset of possible values. For instance, 1100 ms
 *    will be rounded to 1000 ms in bmp280, and 1280 ms in bmp3xx. Refer to sensor
 *    datasheet for possible values. Also, if sampling period is out of range of
 *    possible values, the sensor will use the nearest value.
 ******************************************************************************/
typedef struct sl_pressure_config {
  sl_pressure_oversampling_t oversampling;  /**< Oversampling value.    */
  sl_pressure_power_mode_t power_mode;      /**< Power mode setting.    */
  int sampling_period;                      /**< Sampling period in ms. The closest
                                                 sampling period available on the
                                                 sensor will be selected */
} sl_pressure_config_t;

/***************************************************************************//**
 * @brief
 *    Initialize the barometric pressure sensor.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_FAIL Initialization failed
 ******************************************************************************/
sl_status_t sl_pressure_init(sl_i2cspm_t *i2cspm);

/***************************************************************************//**
 * @brief
 *    De-initialize the barometric pressure sensor.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 ******************************************************************************/
void sl_pressure_deinit(sl_i2cspm_t *i2cspm);

/***************************************************************************//**
 * @brief
 *    Configure the barometric pressure sensor.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[in] cfg
 *    Structure, which holds the configuration parameters
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_FAIL Sensor configuration failed
 ******************************************************************************/
sl_status_t sl_pressure_configure(sl_i2cspm_t *i2cspm, sl_pressure_config_t *cfg);

/***************************************************************************//**
 * @brief
 *    Initiate temperature measurement on the barometric pressure sensor and
 *    read temperature from it.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[out] temperature
 *    The measured temperature in degrees Celsius
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_FAIL Failure during temperature measurement/read
 ******************************************************************************/
sl_status_t sl_pressure_measure_temperature(sl_i2cspm_t *i2cspm, float *temperature);

/***************************************************************************//**
 * @brief
 *    Initiate pressure measurement on the barometric pressure sensor and read
 *    pressure from it.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[out] pressure
 *    The measured pressure in Pascal.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_FAIL Failure during pressure measurement/read
 ******************************************************************************/
sl_status_t sl_pressure_measure_pressure(sl_i2cspm_t *i2cspm, float *pressure);

#ifdef __cplusplus
}
#endif

#endif // SL_PRESSURE_H
