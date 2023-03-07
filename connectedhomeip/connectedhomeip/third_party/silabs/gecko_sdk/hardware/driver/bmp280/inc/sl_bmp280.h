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

#ifndef SL_BMP280_H
#define SL_BMP280_H

#include "sl_i2cspm.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup bmp280 BMP280 - Barometric Pressure Sensor
 * @brief Driver for the Bosch Sensortec BMP280 barometric pressure sensor


   @n @section bmp280_example BMP280 example

     Basic example for performing pressure measurement: @n @n
     @verbatim

   #include "sl_i2cspm_instances.h"
   #include "sl_bmp280.h"

   int main( void )
   {

     ...

     float pressure;

     sl_bmp280_init(sl_i2cspm_sensor_env);
     sl_bmp280_measure_pressure(sl_i2cspm_sensor_env, &pressure);

     ...

   } @endverbatim

 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Structure to configure the BMP280 device
 ******************************************************************************/
typedef struct sl_bmp280_config {
  uint8_t oversampling;       /**< Oversampling value                         */
  uint8_t power_mode;         /**< SLEEP, FORCED or NORMAL power mode setting */
  uint8_t standby_time;       /**< Standby time setting                       */
} sl_bmp280_config_t;

/***************************************************************************//**
 * @brief
 *    Initialize the barometric pressure sensor chip.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_FAIL Initialization failed
 ******************************************************************************/
sl_status_t sl_bmp280_init(sl_i2cspm_t *i2cspm);

/***************************************************************************//**
 * @brief
 *    De-initialize the barometric pressure module.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 ******************************************************************************/
void sl_bmp280_deinit(sl_i2cspm_t *i2cspm);

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
 * @note
 *   Make sure to pass by Sleep Mode before switching between Forced mode
 *   and normal mode. Refer to datasheet for more details.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_FAIL Sensor configuration failed
 ******************************************************************************/
sl_status_t sl_bmp280_configure(sl_i2cspm_t *i2cspm, sl_bmp280_config_t *cfg);

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
sl_status_t sl_bmp280_measure_temperature(sl_i2cspm_t *i2cspm, float *temperature);

/***************************************************************************//**
 * @brief
 *    Initiate pressure measurement on the barometric pressure sensor and read
 *    pressure from it.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[out] pressure
 *    The measured pressure in millibars.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_FAIL Failure during pressure measurement/read
 ******************************************************************************/
sl_status_t sl_bmp280_measure_pressure(sl_i2cspm_t *i2cspm, float *pressure);

/** @} */

#ifdef __cplusplus
}
#endif
#endif // SL_BMP280_H
