/***************************************************************************//**
 * @file
 * @brief Driver for the Si7013/Si7020/Si7021 Relative Humidity and Temperature
 * sensor
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

#ifndef SL_SI70XX_H
#define SL_SI70XX_H

#include <stdbool.h>
#include "sl_status.h"
#include "sl_i2cspm.h"

/***************************************************************************//**
 * @addtogroup si70xx Si70xx - RHT Sensor
 * @brief Silicon Labs Si7006/13/20/21 Relative Humidity and Temperature Sensor I2C driver.

 @n @section si70xx_example Si70xx example code

   Basic example for relative humidity and temperature measurement using
   an Si7021 sensor: @n @n
   @verbatim

 #include "sl_i2cspm_instances.h"
 #include "sl_si70xx.h"

 int main( void )
 {

   ...

   int32_t temp_data;
   uint32_t rh_data;

   sl_si70xx_init(sl_i2cspm_sensor, SI7021_ADDR);
   sl_si70xx_measure_rh_and_temp(sl_i2cspm_sensor, SI7021_ADDR, &rh_data, &temp_data);

   ...

 } @endverbatim
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
* @name Sensor Defines
* @{
******************************************************************************/
/** I2C device address for Si7006 */
#define SI7006_ADDR      0X40
/** I2C device address for Si7013 */
#define SI7013_ADDR      0x41
/** I2C device address for Si7020 */
#define SI7020_ADDR      0X40
/** I2C device address for Si7021 */
#define SI7021_ADDR      0x40

/** Device ID value for Si7006 */
#define SI7006_DEVICE_ID 0x06
/** Device ID value for Si7013 */
#define SI7013_DEVICE_ID 0x0D
/** Device ID value for Si7020 */
#define SI7020_DEVICE_ID 0x14
/** Device ID value for Si7021 */
#define SI7021_DEVICE_ID 0x15

/** @}  */

/**************************************************************************//**
 * @brief
 *   Initialize the Si70xx sensor.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address to probe.
 * @retval SL_STATUS_OK An Si70xx device is present on the I2C bus
 * @retval SL_STATUS_INITIALIZATION No Si70xx device present
 *****************************************************************************/
sl_status_t sl_si70xx_init(sl_i2cspm_t *i2cspm, uint8_t addr);

/**************************************************************************//**
 * @brief
 *   Check whether an Si7006/13/20/21 is present on the I2C bus or not.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address to probe.
 * @param[out] device_id
 *   Write device ID from SNB_3 if device responds. Pass in NULL to discard.
 *   Should be 0x0D for Si7013, 0x14 for Si7020 or 0x15 for Si7021
 * @retval true An Si70xx device is present on the I2C bus
 * @retval false No Si70xx device present
 *****************************************************************************/
bool sl_si70xx_present(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t *device_id);

/**************************************************************************//**
 * @brief
 *  Measure relative humidity and temperature from an Si7006/13/20/21 sensor.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] rhData
 *   The relative humidity in percent (multiplied by 1000).
 * @param[out] tData
 *   The temperature in milliCelsius.
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmission error
 *****************************************************************************/
sl_status_t sl_si70xx_measure_rh_and_temp(sl_i2cspm_t *i2cspm, uint8_t addr,
                                          uint32_t *rhData, int32_t *tData);

/**************************************************************************//**
 * @brief
 *  Read Firmware Revision from an Si7006/13/20/21 sensor.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] fwRev
 *   The internal firmware revision. 0xFF === 1.0
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmission error
 *****************************************************************************/
sl_status_t sl_si70xx_get_firmware_revision(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t *fwRev);

/**************************************************************************//**
 * @brief
 *  Read relative humidity and temperature from an Si7006/13/20/21 sensor.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] rhData
 *   The relative humidity in percent (multiplied by 1000).
 * @param[out] tData
 *   The temperature in milliCelsius.
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmission error
 *****************************************************************************/
sl_status_t sl_si70xx_read_rh_and_temp(sl_i2cspm_t *i2cspm, uint8_t addr, uint32_t *rhData,
                                       int32_t *tData);

/**************************************************************************//**
 * @brief
 * Start a no hold measurement of relative humidity and temperature from an Si7006/13/20/21 sensor.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmission error
 *****************************************************************************/
sl_status_t sl_si70xx_start_no_hold_measure_rh_and_temp(sl_i2cspm_t *i2cspm, uint8_t addr);

/**************************************************************************//**
 * @brief
 *  Measure the analog voltage or thermistor temperature from the Si7013 sensor.
 * @note Analog voltage measurement only supported by Si7013
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] vData
 *   The data read from the sensor.
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmission error
 * @retval SL_STATUS_FAIL Failure due to incorrect device ID
 *****************************************************************************/
sl_status_t sl_si7013_measure_analog_voltage(sl_i2cspm_t *i2cspm, uint8_t addr, int32_t *vData);

#ifdef __cplusplus
}
#endif

/**@}* (si70xx) */

#endif /* SL_SI70XX_H */
