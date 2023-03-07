/***************************************************************************//**
 * @file
 * @brief Driver for the Silicon Labs Si7210 Hall Effect Sensor
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

#ifndef SL_SI7210_H
#define SL_SI7210_H

#include <stdint.h>
#include <stdbool.h>

#include "sl_status.h"
#include "sl_i2cspm.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup si7210 Si7210 - Magnetic Hall Effect sensor
 *
 * @brief Driver for the Silicon Labs Si7210 Hall effect sensor

 @n @section si7210_example Si7210 example code

   Basic example for measurement of magnetic field strength: @n @n
   @verbatim

 #include "sl_i2cspm_instances.h"
 #include "sl_si7210.h"

 int main( void )
 {

   ...

   float mTdata:;

   sl_si7210_init(sl_i2cspm_sensor);
   sl_si7210_measure(sl_i2cspm_sensor, 10000, &mTdata);

   ...

 } @endverbatim
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Structure to configure the Si7210 sensor
 ******************************************************************************/
typedef struct sl_si7210_configure {
  float              threshold;     /**< Decision point for magnetic field high or low in mT  */
  float              hysteresis;    /**< Hysteresis in mT                                     */
  uint8_t            polarity;      /**< Magnetic field polarity setting                      */
  bool               output_invert;  /**< The polarity of the output pin                      */
} sl_si7210_configure_t;

/**************************************************************************//**
 * @brief
 *    Does device-specific initialization for the Si7210 chip.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 * @retval SL_STATUS_INITIALIZATION  Initialization failed due to device ID
 *            mismatch
 *****************************************************************************/
sl_status_t sl_si7210_init(sl_i2cspm_t *i2cspm);

/**************************************************************************//**
 * @brief
 *    Configure the Si7210 chip.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[in] config
 *    The structure, which contains the configuration parameters.
 *    If all fields are set to zero, the chip will revert to use default values.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
sl_status_t sl_si7210_configure(sl_i2cspm_t *i2cspm, sl_si7210_configure_t *config);

/**************************************************************************//**
 * @brief
 *    Perform a measurement.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[in] scale
 *    Desired scale in uT
 *
 * @param[out] result
 *    The measured field strength value in mT
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
sl_status_t sl_si7210_measure(sl_i2cspm_t *i2cspm, uint32_t scale, float *result);

/**************************************************************************//**
 * @brief
 *    Return the tamper level configured in the chip.
 *
 * @return
 *    The tamper level in mT
 *****************************************************************************/
float   sl_si7210_get_tamper_threshold(void);

/**************************************************************************//**
 * @brief
 *    Put Si7210 into Sleep (No-measurement) Mode.
 *    Wake-up command needs to be issued to become responsive.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
sl_status_t sl_si7210_sleep(sl_i2cspm_t *i2cspm);

/**************************************************************************//**
 * @brief
 *    Put the Si7210 into Sleep w/ Measurement Mode: OUTPUT is updated 200 ms.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
sl_status_t sl_si7210_sleep_sltimeena(sl_i2cspm_t *i2cspm);

/**************************************************************************//**
 * @brief
 *    Wake up the Hall sensor chip.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
sl_status_t sl_si7210_wake_up(sl_i2cspm_t *i2cspm);

/**************************************************************************//**
 * @brief
 *    Read out Si7210 Conversion Data - 15bits.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[out] data
 *    The raw magnetic field conversion data (15 bits)
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 * @retval SL_STATUS_OBJECT_READ  No measurement data available
 *****************************************************************************/
sl_status_t sl_si7210_read_data(sl_i2cspm_t *i2cspm, int16_t *data);

/**************************************************************************//**
 * @brief
 *    Read out Si7210 Revision and ID.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[out] id
 *    Si7210 part ID
 *
 * @param[out] rev
 *    Si7210 part Revision
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
sl_status_t sl_si7210_identify(sl_i2cspm_t *i2cspm, uint8_t *id, uint8_t *rev);

/**************************************************************************//**
 * @brief
 *    Change Mag-Field scale to 200mT.
 *    If desired, must be performed after power-up or wake-up from sleep.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
sl_status_t sl_si7210_set_mt_range_200(sl_i2cspm_t *i2cspm);

/**************************************************************************//**
 * @brief
 *   Perform burst-conversion(4 samples), read mT-data, and
 *   put part into sltimeena-sleep mode where OUT is updated every 200 ms.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[in] range200mT
 *   range200mT=false : full-scale equals 20mT
 *   range200mT=true  : full-scale equals 200mT
 *
 * @param[out] mTdata
 *   Mag-field conversion reading, signed 32-bit integer
 *   mTdata must be divided by 1000 to get decimal value in mT units
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 * @retval SL_STATUS_OBJECT_READ  No measurement data available
 *****************************************************************************/
sl_status_t sl_si7210_read_magfield_data_and_sltimeena(sl_i2cspm_t *i2cspm, bool range200mT, int32_t *mTdata);

/**************************************************************************//**
 * @brief
 *   Wake-up from Sleep, perform burst-conversion(4 samples), read mT-data,
 *   and put part into sleep mode (no-measurement). Requires Wake-Up.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[in] range200mT
 *   range200mT=false : full-scale equals 20mT
 *   range200mT=true  : full-scale equals 200mT
 *
 * @param[out] mTdata
 *   Mag-field conversion reading, signed 32-bit integer
 *   mTdata must be divided by 1000 to get decimal value in mT units
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 * @retval SL_STATUS_OBJECT_READ  No measurement data available
 *****************************************************************************/
sl_status_t sl_si7210_read_magfield_data_and_sleep(sl_i2cspm_t *i2cspm, bool range200mT, int32_t *mTdata);

/***************************************************************************//**
 * @addtogroup si7210_details Si7210 Details
 * @brief Register interface and implementation details
 * @{
 ******************************************************************************/

 /**************************************************************************//**
  * @brief
  *    Read register from the OTP area of the Si7021 device.
  *
  * @param[in] i2cspm
  *   The I2CSPM instance to use.
  *
  * @param[in] otpAddr
  *    The register address to read from in the sensor
  *
  * @param[out] otpData
  *    The data read from the device
  *
  * @retval SL_STATUS_OK Success
  * @retval SL_STATUS_TRANSMIT  I2C transmission error
  *****************************************************************************/
 sl_status_t sl_si7210_read_otp_register(sl_i2cspm_t *i2cspm, uint8_t otpAddr, uint8_t *otpData);

/**************************************************************************//**
 * @brief
 *    Read register from the Hall sensor device.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[in] addr
 *    The register address to read from in the sensor
 *
 * @param[out] data
 *    The data read from the device
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
sl_status_t sl_si7210_read_register(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t *data);

/**************************************************************************//**
 * @brief
 *    Write a register in the Hall sensor device.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[in] addr
 *    The register address to write
 *
 * @param[in] data
 *    The data to write to the register
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
sl_status_t sl_si7210_write_register(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t data);

/**************************************************************************//**
 * @brief
 *    Set the given bit(s) in a register in the Hall sensor device.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[in] addr
 *    The address of the register
 *
 * @param[in] mask
 *    The mask specifies which bits should be set. If a given bit of the mask is
 *    1, that register bit will be set to 1. All the other register bits will be
 *    untouched.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
sl_status_t sl_si7210_set_register_bits(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t mask);

/**************************************************************************//**
 * @brief
 *    Clear the given bit(s) in a register in the Hall sensor device.
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[in] addr
 *    The address of the register
 *
 * @param[in] mask
 *    The mask specifies which bits should be clear. If a given bit of the mask
 *    is 1 that register bit will be cleared to 0. All the other register bits
 *    will be untouched.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
sl_status_t sl_si7210_clear_register_bits(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t mask);

/**************************************************************************//**
 * @brief
 *    Calculate the sw_op value from the threshold by finding the inverse of
 *    the formula:
 *    threshold = (16 + sw_op[3:0]) * 2^sw_op[6:4]
 *
 * @param[in] threshold
 *    Threshold value
 *
 * @return
 *    The value of the sw_op bitfield
 *****************************************************************************/
uint8_t sl_si7210_calculate_sw_op(float threshold);

/**************************************************************************//**
 * @brief
 *    Calculate the sw_hyst value from the hysteresis by finding the inverse of
 *    the formula:
 *    hysteresis = (8 + sw_hyst[2:0]) * 2^sw_hyst[5:3]
 *
 * @param[in] hysteresis
 *    Hysteresis value
 *
 * @param[in] scale200mT
 *    scale200mT=false : full-scale equals 20mT
 *    scale200mT=true  : full-scale equals 200mT
 *
 * @return
 *    The value of the sw_hyst bitfield
 *****************************************************************************/
uint8_t sl_si7210_calculate_sw_hyst(float hysteresis, bool scale200mT);

/**************************************************************************//**
 * @brief
 *    Calculate the sw_tamper value from the tamper threshold by finding the
 *    inverse of the formula:
 *    tamper = (16 + sw_tamper[3:0]) * 2^(sw_tamper[5:4] + 5)
 *
 * @param[in] tamper
 *    Tamper value
 *
 * * @param[in] scale200mT
 *    scale200mT=false : full-scale equals 20mT
 *    scale200mT=true  : full-scale equals 200mT
 *
 * @return
 *    The value of the sw_tamper bitfield
 *****************************************************************************/
uint8_t sl_si7210_calculate_sw_tamper(float tamper, bool scale200mT);

/**************************************************************************//**
 * @brief
 *    Calculate the slTime value from the sleep time by finding the inverse of
 *    the formula:
 *    tsleep = (32 + slTime[4:0]) * 2^(8 + slTime[7:5]) / 12 MHz
 *
 * @param[in] samplePeriod
 *    The sleep time
 *
 * @param[out] slFast
 *    The value of the slFast bit
 *
 * @return
 *    The value of the slTime bitfield
 *****************************************************************************/
uint8_t sl_si7210_calculate_sltime(uint32_t samplePeriod, uint8_t *slFast);
/** @} (end addtogroup si7210_details) */
/** @} (end addtogroup si7210) */

#ifdef __cplusplus
}
#endif
#endif // SL_SI7210_H
