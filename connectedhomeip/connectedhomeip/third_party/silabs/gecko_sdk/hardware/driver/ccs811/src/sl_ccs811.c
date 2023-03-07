/***************************************************************************//**
 * @file
 * @brief Driver for the Cambridge CMOS Sensors CCS811 gas and indoor air
 * quality sensor
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "sl_i2cspm.h"
#include "sl_ccs811_config.h"
#include "sl_ccs811.h"
#include "sl_sleeptimer.h"

#define CCS811_I2C_ADDRESS     (0xB4)
#define CCS811_HW_ID           (0x81)

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
// Local prototypes
static uint32_t ccs811_erase_application(sl_i2cspm_t *i2cspm);
static uint32_t ccs811_verify_application(sl_i2cspm_t *i2cspm, bool *appValid);
static uint32_t ccs811_program_firmware(sl_i2cspm_t *i2cspm, uint8_t buffer[]);
static uint32_t ccs811_set_app_start(sl_i2cspm_t *i2cspm);
/** @endcond */

/**************************************************************************//**
* @defgroup CCS811 CCS811 - Indoor Air Quality Sensor
* @{
* @brief Driver for the Cambridge CMOS Sensors CCS811 gas and indoor air
* quality sensor
******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Wakes up the Air Quality / Gas Sensor
 *
 * @param[in] wake
 *    Set true to wake up, false otherwise
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
static uint32_t ccs811_wake(bool wake)
{
  if ( wake ) {
    GPIO_PinOutClear(SL_CCS811_WAKE_PORT, SL_CCS811_WAKE_PIN);
    sl_sleeptimer_delay_millisecond(1);
  } else {
    GPIO_PinOutSet(SL_CCS811_WAKE_PORT, SL_CCS811_WAKE_PIN);
    sl_sleeptimer_delay_millisecond(1);
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Initializes the chip
 ******************************************************************************/
sl_status_t sl_ccs811_init(sl_i2cspm_t *i2cspm)
{
  uint8_t id;
  sl_status_t status;

  GPIO_PinModeSet(SL_CCS811_WAKE_PORT, SL_CCS811_WAKE_PIN, gpioModePushPull, 1);

  /* Set the wake pin low */
  ccs811_wake(true);

  /* About 80 ms required to reliably start the device, wait a bit more */
  sl_sleeptimer_delay_millisecond(100);

  /* Check if the chip present and working by reading the hardware ID */
  status = sl_ccs811_get_hardware_id(i2cspm, &id);
  if ((status != SL_STATUS_OK) || (id != CCS811_HW_ID) ) {
    return SL_STATUS_INITIALIZATION;
  }

  /* Switch from boot mode to application mode */
  status = sl_ccs811_start_application(i2cspm);
  if (status != SL_STATUS_OK){
    return status;
  }
  /* Go back to sleep */
  ccs811_wake(false);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    De-initializes the chip
 ******************************************************************************/
sl_status_t sl_ccs811_deinit(sl_i2cspm_t *i2cspm)
{
  /* Disable the sensor  */
  (void)i2cspm;
  ccs811_wake(false);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads Hardware ID from the CCS811 sensor
 ******************************************************************************/
sl_status_t sl_ccs811_get_hardware_id(sl_i2cspm_t *i2cspm, uint8_t *hardwareID)
{
  sl_status_t result;

  result = sl_ccs811_read_mailbox(i2cspm, CCS811_ADDR_HW_ID, 1, hardwareID);

  return result;
}

/**************************************************************************//**
*    Reads the status of the CCS811 sensor
******************************************************************************/
sl_status_t sl_ccs811_get_status(sl_i2cspm_t *i2cspm, uint8_t *status)
{
  sl_status_t result;

  result = sl_ccs811_read_mailbox(i2cspm, CCS811_ADDR_STATUS, 1, status);

  return result;
}

/***************************************************************************//**
 *    Checks if new measurement data available
 ******************************************************************************/
bool sl_ccs811_is_data_available(sl_i2cspm_t *i2cspm)
{
  bool state;
  sl_status_t status;
  uint8_t reg;

  state = false;
  /* Read the status register */
  status = sl_ccs811_get_status(i2cspm, &reg);

  /* Check if the DATA_READY bit is set */
  if ( (status == SL_STATUS_OK) && ( (reg & 0x08) == 0x08) ) {
    state = true;
  }

  return state;
}

/***************************************************************************//**
 *    Switches the CCS811 chip from boot to application mode
 ******************************************************************************/
sl_status_t sl_ccs811_start_application(sl_i2cspm_t *i2cspm)
{
  sl_status_t result;
  uint8_t status;

  /* Read status */
  result = sl_ccs811_read_mailbox(i2cspm, CCS811_ADDR_STATUS, 1, &status);

  /* If no application firmware present in the CCS811 then return an error message */
  if ( (status & 0x10) != 0x10 ) {
    return SL_STATUS_NOT_AVAILABLE;
  }

  /* Issue APP_START */
  result = ccs811_set_app_start(i2cspm);
  if (result != SL_STATUS_OK){
    return result;
  }

  /* Wait 1 ms after app start to send new i2c commands */
  sl_sleeptimer_delay_millisecond(1);

  /* Check status again */
  result = sl_ccs811_read_mailbox(i2cspm, CCS811_ADDR_STATUS, 1, &status);

  /* If the chip firmware did not switch to application mode then return with error */
  if ( (status & 0x90) != 0x90 ) {
    return SL_STATUS_INVALID_STATE;
  }

  return result;
}

/***************************************************************************//**
 *    Reads data from a specific Mailbox address
 ******************************************************************************/
sl_status_t sl_ccs811_read_mailbox(sl_i2cspm_t *i2cspm, uint8_t id, uint8_t length, uint8_t *data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];
  sl_status_t retval;

  retval = SL_STATUS_OK;

  ccs811_wake(true);

  /* Write data */
  i2c_write_data[0] = id;

  /* Configure I2C bus transaction */
  seq.addr        = CCS811_I2C_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE_READ;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;

  /* Select length of data to be read */
  seq.buf[1].data = data;
  seq.buf[1].len = length;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  }

  ccs811_wake(false);

  return retval;
}

/***************************************************************************//**
 *    Reads measurement data (eCO2 and TVOC) from the CCS811 sensor
 ******************************************************************************/
sl_status_t sl_ccs811_get_measurement(sl_i2cspm_t *i2cspm, uint16_t *eco2, uint16_t *tvoc)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[4];
  uint8_t i2c_write_data[1];
  sl_status_t retval;

  retval = SL_STATUS_OK;

  *eco2 = 0;
  *tvoc = 0;

  ccs811_wake(true);

  /* Read four bytes from the ALG_RESULT_DATA mailbox register */
  i2c_write_data[0] = CCS811_ADDR_ALG_RESULT_DATA;

  seq.addr        = CCS811_I2C_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE_READ;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 4;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  } else {
    /* Convert the read bytes to 16 bit values */
    *eco2 = ( (uint16_t) i2c_read_data[0] << 8) + (uint16_t) i2c_read_data[1];
    *tvoc = ( (uint16_t) i2c_read_data[2] << 8) + (uint16_t) i2c_read_data[3];
  }

  ccs811_wake(false);

  return retval;
}

/***************************************************************************//**
 *    Gets the latest readings from the sense resistor of the CCS811 sensor
 *****************************************************************************/
sl_status_t sl_ccs811_get_raw_data(sl_i2cspm_t *i2cspm, uint16_t *current, uint16_t *rawData)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[2];
  uint8_t i2c_write_data[1];
  sl_status_t retval;

  retval = SL_STATUS_OK;

  *current = 0;
  *rawData = 0;

  ccs811_wake(true);

  /* Read four bytes from the CCS811_ADDR_RAW_DATA mailbox register */
  i2c_write_data[0] = CCS811_ADDR_RAW_DATA;

  seq.addr        = CCS811_I2C_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE_READ;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 2;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  } else {
    /* current: the upper six bits of Byte0 */
    *current = (uint16_t) ( (i2c_read_data[0] >> 2) & 0x3F);
    /* raw ADC reading: the lower two bits of Byte0 is the MSB, Byte1 is the LSB */
    *rawData = (uint16_t) ( (i2c_read_data[0] & 0x03) << 8) + (uint16_t) i2c_read_data[1];
  }

  ccs811_wake(false);

  return retval;
}

/***************************************************************************//**
 *    Performs software reset on the CCS811
 ******************************************************************************/
sl_status_t sl_ccs811_software_reset(sl_i2cspm_t *i2cspm)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[4];
  uint8_t i2c_write_data[5];
  sl_status_t retval;

  retval = SL_STATUS_OK;

  ccs811_wake(true);

  /* Write the 0x11 0xE5 0x72 0x8A key sequence to software reset register */
  /* The key sequence is used to prevent accidental reset                  */
  i2c_write_data[0] = CCS811_ADDR_SW_RESET;
  i2c_write_data[1] = 0x11;
  i2c_write_data[2] = 0xE5;
  i2c_write_data[3] = 0x72;
  i2c_write_data[4] = 0x8A;

  seq.addr        = CCS811_I2C_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 5;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  }

  ccs811_wake(false);

  return retval;
}

/***************************************************************************//**
 *    Sets the measurement mode of the CCS811 sensor
 ******************************************************************************/
sl_status_t sl_ccs811_set_measure_mode(sl_i2cspm_t *i2cspm, uint8_t measMode)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[1];
  uint8_t i2c_write_data[2];
  sl_status_t retval;

  retval = SL_STATUS_OK;

  ccs811_wake(true);

  /* Bits 7,6,2,1 and 0 are reserved, clear them */
  measMode = (measMode & 0x38);

  /* Write to the measurement mode register      */
  i2c_write_data[0] = CCS811_ADDR_MEASURE_MODE;
  i2c_write_data[1] = measMode;

  seq.addr        = CCS811_I2C_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 2;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  }

  ccs811_wake(false);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Changes the mode of the CCS811 from Boot mode to running the application
 *
 * @return
 *    @retval SL_STATUS_OK Success
 *    @retval SL_STATUS_TRANSMIT I2C transmission error
 ******************************************************************************/
static uint32_t ccs811_set_app_start(sl_i2cspm_t *i2cspm)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[2];
  uint8_t i2c_write_data[1];
  sl_status_t retval;

  retval = SL_STATUS_OK;

  ccs811_wake(true);

  /* Perform a write with no data to the APP_START register to change the */
  /* state from boot mode to application mode                             */
  i2c_write_data[0] = CCS811_ADDR_APP_START;

  seq.addr        = CCS811_I2C_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  }

  ccs811_wake(false);

  return retval;
}

/***************************************************************************//**
 *    Writes temperature and humidity values to the environmental data regs
 ******************************************************************************/
sl_status_t sl_ccs811_set_env_data(sl_i2cspm_t *i2cspm, int32_t tempData, uint32_t rhData)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[4];
  uint8_t i2c_write_data[5];
  uint8_t humidityRegValue;
  uint8_t temperatureRegValue;
  sl_status_t retval;

  ccs811_wake(true);

  /* The CCS811 currently supports only 0.5% resolution        */
  /* If the fraction greater than 0.7 then round up the value  */
  /* Shift to the left by one to meet the required data format */
  if ( ( (rhData % 1000) / 100) > 7 ) {
    humidityRegValue = (rhData / 1000 + 1) << 1;
  } else {
    humidityRegValue = (rhData / 1000) << 1;
  }

  /* If the fraction is greater than 0.2 or less than 0.8 set the            */
  /* LSB bit, which is the most significant bit of the fraction 2^(-1) = 0.5 */
  if ( ( (rhData % 1000) / 100) > 2 && ( ( (rhData % 1000) / 100) < 8) ) {
    humidityRegValue |= 0x01;
  }

  /* Add +25 C to the temperature value                        */
  /* The CCS811 currently supports only 0.5C resolution        */
  /* If the fraction greater than 0.7 then round up the value  */
  /* Shift to the left by one to meet the required data format */
  tempData += 25000;
  if ( ( (tempData % 1000) / 100) > 7 ) {
    temperatureRegValue = (tempData / 1000 + 1) << 1;
  } else {
    temperatureRegValue = (tempData / 1000) << 1;
  }

  /* If the fraction is greater than 0.2 or less than 0.8 set the            */
  /* LSB bit, which is the most significant bit of the fraction 2^(-1) = 0.5 */
  if ( ( (tempData % 1000) / 100) > 2 && ( ( (tempData % 1000) / 100) < 8) ) {
    temperatureRegValue |= 0x01;
  }

  /* Write the correctly formatted values to the environmental data register */
  /* The LSB bytes of the humidity and temperature data are 0x00 because     */
  /* the current CCS811 supports only 0.5% and 0.5C resolution               */
  i2c_write_data[0] = CCS811_ADDR_ENV_DATA;
  i2c_write_data[1] = humidityRegValue;
  i2c_write_data[2] = 0x00;
  i2c_write_data[3] = temperatureRegValue;
  i2c_write_data[4] = 0x00;

  seq.addr        = CCS811_I2C_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 5;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  retval = SL_STATUS_OK;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  }

  ccs811_wake(false);

  return retval;
}

/***************************************************************************//**
 *    Read out current firmware of the CCS811 sensor
 ******************************************************************************/
sl_status_t sl_ccs811_read_firmware_version(sl_i2cspm_t *i2cspm, uint16_t *fw_version)
{
  sl_status_t status;
  uint8_t buffer[2];
  status = sl_ccs811_read_mailbox(i2cspm, CCS811_ADDR_FW_APP_VERSION, 2, buffer);
  if (status != SL_STATUS_OK){
    return status;
  }

  *fw_version = (buffer[0] << 8) + buffer[1];

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Performs a firmware update of the CCS811 sensor
 ******************************************************************************/
sl_status_t sl_ccs811_update_firmware(sl_i2cspm_t *i2cspm, const uint8_t *firmware, uint32_t length)
{
  sl_status_t status;
  uint8_t reg;
  uint8_t buffer[8];
  uint32_t i;
  bool valid;

  /*************************************************************************/
  /** Put CCS811 in BOOT mode                                             **/
  /*************************************************************************/
  status = sl_ccs811_get_status(i2cspm, &reg);
  if (status != SL_STATUS_OK){
    return status;
  }

  if ( (reg & 0x80) == 0x80 ) {
    /* In APP mode - Go to BOOT mode */

    status = sl_ccs811_software_reset(i2cspm);
    if ( (status & 0x80) != 0x00 ) {
      return SL_STATUS_FAIL;
    }

    sl_sleeptimer_delay_millisecond(100);

    /* Check for BOOT mode */
    status = sl_ccs811_get_status(i2cspm, &reg);
    if ( (reg & 0x80) != 0x00 ) {
      return SL_STATUS_FAIL;
    }
  }

  /*************************************************************************/
  /** Erase                                                               **/
  /*************************************************************************/
  status = sl_ccs811_get_status(i2cspm, &reg);
  if ( (reg & 0x10) == 0x10 ) {
    /* Valid application - Erase this */
    status = ccs811_erase_application(i2cspm);
    sl_sleeptimer_delay_millisecond(500);
    if (status != SL_STATUS_OK) {
      return SL_STATUS_FAIL;
    }

    sl_sleeptimer_delay_millisecond(100);

    /* Check APP_VALID flag again */
    status = sl_ccs811_get_status(i2cspm, &reg);
    if ( (reg & 0x10) != 0x00 ) {
      return SL_STATUS_FAIL;
    }
  }

  /*************************************************************************/
  /** Program APP                                                         **/
  /*************************************************************************/
  for ( i = 0; i < length; i += 8 ) {
    /* Read the next 8 bytes from the firmware file */
    memcpy(&buffer, &firmware[i], 8);

    /* Write 8 bytes to the device's flash memory */
    status = ccs811_program_firmware(i2cspm, buffer);
    if ( status != SL_STATUS_OK ) {
      return SL_STATUS_FAIL;
    }
  }

  /*************************************************************************/
  /** Verify APP                                                          **/
  /*************************************************************************/
  status = ccs811_verify_application(i2cspm, &valid);
  if ( (status != SL_STATUS_OK) || (valid == false) ) {
    return false;
  }

  /* Wait at least 70ms before issuing i2c transmissions to the CCS811 sensor */
  sl_sleeptimer_delay_millisecond(70);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *    Writes 8 bytes of data to the firmware memory of the CCS811
 *
 * @param[in] buffer
 *    An 8-byte length buffer containing the data to write
 *
 * @return
 *    @retval SL_STATUS_OK Success
 *    @retval SL_STATUS_TRANSMIT I2C transmission error
 ******************************************************************************/
static uint32_t ccs811_program_firmware(sl_i2cspm_t *i2cspm, uint8_t buffer[])
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[2];
  uint8_t i2c_write_data[9];
  sl_status_t retval;

  retval = SL_STATUS_OK;

  ccs811_wake(true);

  /* Send the Write Data to FLASH command and 8 bytes of binary program code */
  i2c_write_data[0] = CCS811_ADDR_FW_PROGRAM;
  i2c_write_data[1] = buffer[0];
  i2c_write_data[2] = buffer[1];
  i2c_write_data[3] = buffer[2];
  i2c_write_data[4] = buffer[3];
  i2c_write_data[5] = buffer[4];
  i2c_write_data[6] = buffer[5];
  i2c_write_data[7] = buffer[6];
  i2c_write_data[8] = buffer[7];

  seq.addr        = CCS811_I2C_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 9;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  }

  ccs811_wake(false);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Verifies if the application firmware downloaded to the CCS811
 *    was received error free
 *
 * @return
 *    @retval SL_STATUS_OK Success
 *    @retval SL_STATUS_TRANSMIT I2C transmission error
 *****************************************************************************/
static uint32_t ccs811_verify_application(sl_i2cspm_t *i2cspm, bool *appValid)
{
  sl_status_t status;
  uint8_t reg;
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[2];
  uint8_t i2c_write_data[2];

  *appValid = false;

  ccs811_wake(true);

  /* Write (with no data) to the verify register to check if the firmware */
  /* programmed to the CCS811 was received error free                     */
  i2c_write_data[0] = CCS811_ADDR_FW_VERIFY;

  seq.addr        = CCS811_I2C_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  status = SL_STATUS_OK;
  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    status = SL_STATUS_TRANSMIT;
  } else {
    /* Wait until the Verify command finishes */
    sl_sleeptimer_delay_millisecond(100);
    /* Check the status register to see if there were no errors */
    status = sl_ccs811_get_status(i2cspm, &reg);
    if ( (status == SL_STATUS_OK) && ((reg & 0x10) == 0x10) ) {
      *appValid = true;
    }
  }

  ccs811_wake(false);

  return status;
}

/***************************************************************************//**
 * @brief
 *    Erases the application firmware on the CCS811
 *
 * @return
 *    @retval SL_STATUS_OK Success
 *    @retval SL_STATUS_TRANSMIT I2C transmission error
 ******************************************************************************/
static uint32_t ccs811_erase_application(sl_i2cspm_t *i2cspm)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[4];
  uint8_t i2c_write_data[5];
  sl_status_t retval;

  retval = SL_STATUS_OK;

  ccs811_wake(true);

  /* Write the 0xE7 0xA7 0xE6 0x09 key sequence to firmware erase register */
  /* The key sequence is used to prevent accidental erase                  */
  i2c_write_data[0] = CCS811_ADDR_FW_ERASE;
  i2c_write_data[1] = 0xE7;
  i2c_write_data[2] = 0xA7;
  i2c_write_data[3] = 0xE6;
  i2c_write_data[4] = 0x09;

  seq.addr        = CCS811_I2C_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 5;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  }

  ccs811_wake(false);

  return retval;
}

/** @} (end defgroup CCS811) */
