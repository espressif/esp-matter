/***************************************************************************//**
 * @file
 * @brief Driver for the Bosch Sensortec BMP3xx barometric pressure sensor
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_BMP3XX_H
#define SL_BMP3XX_H

#include "sl_i2cspm.h"
#include "sl_status.h"
#include "bmp3.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup bmp3xx BMP3xx - Barometric Pressure Sensor
 * @brief Driver for the Bosch Sensortec BMP3xx barometric pressure sensor


   @n @section bmp3xx_example BMP3xx example

     Basic example to perform pressure measurement: @n @n
     @verbatim

 #include "sl_i2cspm_instances.h"
 #include "sl_bmp3xx.h"

   int main( void )
   {

     ...

     float pressure;
     int8_t result;

     result = sl_bmp3xx_init(sl_i2cspm_sensor_env);

     if (result != SL_STATUS_OK) {
       return SL_STATUS_FAIL;
     }

     result = sl_bmp3xx_measure_pressure(sl_i2cspm_sensor_env, &pressure);

     if (result != SL_STATUS_OK) {
       return SL_STATUS_FAIL;
     }

     ...

   } @endverbatim

 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    BMP3xx device configuration structure
 *
 * @note
 *    Refer to Bosch driver for more details on how to set settings and
 *    desired settings.
 *
 *    Here is the list of available macros to be used to configure desired
 *    settings. User can do OR operation of these macros for configuring
 *    multiple settings.
 *
 * @verbatim
 *    Macros               |   Functionality
 *    ---------------------|----------------------------------------------
 *    BMP3_SEL_PRESS_EN    |   Enable/Disable pressure.
 *    BMP3_SEL_TEMP_EN     |   Enable/Disable temperature.
 *    BMP3_SEL_PRESS_OS    |   Set pressure oversampling.
 *    BMP3_SEL_TEMP_OS     |   Set temperature oversampling.
 *    BMP3_SEL_IIR_FILTER  |   Set IIR filter.
 *    BMP3_SEL_ODR         |   Set ODR.
 *    BMP3_SEL_OUTPUT_MODE |   Set either open drain or push pull
 *    BMP3_SEL_LEVEL       |   Set interrupt pad to be active high or low
 *    BMP3_SEL_LATCH       |   Set interrupt pad to be latched or nonlatched.
 *    BMP3_SEL_DRDY_EN     |   Map/Unmap the drdy interrupt to interrupt pad.
 *    BMP3_SEL_I2C_WDT_EN  |   Enable/Disable I2C internal watch dog.
 *    BMP3_SEL_I2C_WDT     |   Set I2C watch dog timeout delay.
 * @endverbatim
 *
 ******************************************************************************/
typedef struct sl_bmp3xx_config {
  struct  bmp3_settings settings;   /**< Settings structure used by the Bosch
                                        BMP3xx driver */
  uint32_t desired_settings;        /**< Desired settings to be configured */
} sl_bmp3xx_config_t;

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
sl_status_t sl_bmp3xx_init(sl_i2cspm_t *i2cspm);

/***************************************************************************//**
 * @brief
 *    De-initialize the barometric pressure module.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 ******************************************************************************/
void sl_bmp3xx_deinit(sl_i2cspm_t *i2cspm);

/***************************************************************************//**
 * @brief
 *    Set the barometric pressure sensor configuration.
 *
 * @param[in] i2cspm
 *    The I2CSPM instance to use.
 *
 * @param[in] cfg
 *    Structure, which holds the configuration parameters
 *
 * @note
 *   Make sure to pass by Sleep Mode before switching between forced mode
 *   and normal mode. Refer to datasheet for more details.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_FAIL Sensor configuration failed
 ******************************************************************************/
sl_status_t sl_bmp3xx_set_config(sl_i2cspm_t *i2cspm, sl_bmp3xx_config_t *cfg);

/***************************************************************************//**
 * @brief
 *    Get the barometric pressure sensor configuration.
 *
 * @param[in] i2cspm
 *    The I2CSPM instance to use.
 *
 * @param[out] cfg
 *    Structure to get the sensor configuration.
 ******************************************************************************/
void sl_bmp3xx_get_config(sl_i2cspm_t *i2cspm, sl_bmp3xx_config_t *cfg);

/***************************************************************************//**
 * @brief
 *    Initiate temperature measurement on the barometric pressure sensor and
 *    read temperature from it. It is a blocking function.
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
sl_status_t sl_bmp3xx_measure_temperature(sl_i2cspm_t *i2cspm, float *temperature);

/***************************************************************************//**
 * @brief
 *    Initiate pressure measurement on the barometric pressure sensor and read
 *    pressure from it. It is a blocking function.
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
sl_status_t sl_bmp3xx_measure_pressure(sl_i2cspm_t *i2cspm, float *pressure);

/** @} */

#ifdef __cplusplus
}
#endif
#endif // SL_BMP3XX_H
