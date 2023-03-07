/***************************************************************************//**
 * @file
 * @brief Driver for the Si7013/Si7020/Si7021 Relative Humidity and Temperature
 * sensor
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stddef.h>
#include "sl_si70xx.h"
#include "sl_i2cspm.h"
#include "sl_sleeptimer.h"
#include "stddef.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Si70xx Read Temperature Command */
#define SI70XX_READ_TEMP       0xE0  /* Read previous T data from RH measurement
                                      * command*/
/** Si70xx Read RH Command */
#define SI70XX_MEASURE_RH      0xE5  /* Perform RH (and T) measurement. */
/** Si70xx Read RH (no hold) Command */
#define SI70XX_MEASURE_RH_NH   0xF5  /* Perform RH (and T) measurement in no hold mode. */
/** Si7013 Read Thermistor Command */
#define SI7013_READ_VIN        0xEE  /* Perform thermistor measurement. Only available for Si7013 */
/** Si70xx Read ID */
#define SI70XX_READ_ID1_1      0xFA
#define SI70XX_READ_ID1_2      0x0F
#define SI70XX_READ_ID2_1      0xFc
#define SI70XX_READ_ID2_2      0xc9
/** Si70xx Read Firmware Revision */
#define SI70XX_READ_FWREV_1    0x84
#define SI70XX_READ_FWREV_2    0xB8
/** @endcond */

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
// Local prototypes
static sl_status_t sl_si70xx_send_command(sl_i2cspm_t *i2cspm, uint8_t addr, uint32_t *data, uint8_t command);
static sl_status_t sl_si70xx_start_no_hold_measure(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t command);
static sl_status_t sl_si70xx_read_data(sl_i2cspm_t *i2cspm, uint8_t addr, uint32_t *data);
static sl_status_t sl_si70xx_write_user_register_2(sl_i2cspm_t *i2cspm, uint8_t addr, int8_t data);
static int32_t sl_si70xx_get_celcius_temperature(int32_t temp_data);
static uint32_t sl_si70xx_get_percent_relative_humidity(uint32_t rh_data);
/** @endcond */

/***************************************************************************//**
 *    Initializes the Si7006/13/20/21 sensor
 ******************************************************************************/
sl_status_t sl_si70xx_init(sl_i2cspm_t *i2cspm, uint8_t addr)
{
  sl_status_t status;

  status = SL_STATUS_OK;

  if (!sl_si70xx_present(i2cspm, addr, NULL)) {
    /* Wait for sensor to become ready */
    sl_sleeptimer_delay_millisecond(80);

    if (!sl_si70xx_present(i2cspm, addr, NULL)) {
      status = SL_STATUS_INITIALIZATION;
    }
  }

  return status;
}

/**************************************************************************//**
 *  Reads Firmware Revision from a Si7006/13/20/21 sensor.
 *****************************************************************************/
sl_status_t sl_si70xx_get_firmware_revision(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t *fwRev)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_write_data[2];
  uint8_t                    i2c_read_data[1];

  seq.addr  = addr << 1;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select command to issue */
  i2c_write_data[0] = SI70XX_READ_FWREV_1;
  i2c_write_data[1] = SI70XX_READ_FWREV_2;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 2;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 1;

  ret = I2CSPM_Transfer(i2cspm, &seq);

  if (ret != i2cTransferDone) {
    *fwRev = 0;
    return SL_STATUS_TRANSMIT;
  }
  *fwRev = i2c_read_data[0];

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Starts no hold measurement of relative humidity and temperature from a Si7006/13/20/21 sensor.
 *****************************************************************************/
sl_status_t sl_si70xx_start_no_hold_measure_rh_and_temp(sl_i2cspm_t *i2cspm, uint8_t addr)
{
  sl_status_t retval;
  retval = sl_si70xx_start_no_hold_measure(i2cspm, addr, SI70XX_MEASURE_RH_NH);

  return retval;
}

/**************************************************************************//**
 *  Reads relative humidity and temperature from a Si7006/13/20/21 sensor.
 *****************************************************************************/
sl_status_t sl_si70xx_read_rh_and_temp(sl_i2cspm_t *i2cspm, uint8_t addr, uint32_t *rhData,
                                       int32_t *tData)
{
  sl_status_t status;
  status = sl_si70xx_read_data(i2cspm, addr, rhData);

  if (status != SL_STATUS_OK) {
    return status;
  }

  *rhData = sl_si70xx_get_percent_relative_humidity(*rhData);

  status = sl_si70xx_send_command(i2cspm, addr, (uint32_t *) tData, SI70XX_READ_TEMP);

  if (status != SL_STATUS_OK) {
    return status;
  }

  *tData = sl_si70xx_get_celcius_temperature(*tData);

  return status;
}

/**************************************************************************//**
 *  Measures relative humidity and temperature from a Si7006/13/20/21 sensor.
 *****************************************************************************/
sl_status_t sl_si70xx_measure_rh_and_temp(sl_i2cspm_t *i2cspm, uint8_t addr, uint32_t *rhData,
                                          int32_t *tData)
{
  sl_status_t retval;
  retval = sl_si70xx_send_command(i2cspm, addr, rhData, SI70XX_MEASURE_RH);

  if (retval != SL_STATUS_OK) {
    return retval;
  }

  *rhData = sl_si70xx_get_percent_relative_humidity(*rhData);

  retval = sl_si70xx_send_command(i2cspm, addr, (uint32_t *) tData, SI70XX_READ_TEMP);

  if (retval != SL_STATUS_OK) {
    return retval;
  }

  *tData = sl_si70xx_get_celcius_temperature(*tData);

  return retval;
}

/**************************************************************************//**
 *  Measures the analog voltage or thermistor temperature from the Si7013 sensor
 *****************************************************************************/
sl_status_t sl_si7013_measure_analog_voltage(sl_i2cspm_t *i2cspm, uint8_t addr, int32_t *vData)
{
  sl_status_t retval;
  uint8_t device_id;
  sl_si70xx_present(i2cspm, addr, &device_id);

  // Check if SI7013 device
  if (device_id != SI7013_DEVICE_ID) {
    return SL_STATUS_FAIL;
  }

  sl_si70xx_write_user_register_2(i2cspm, addr, 0x0e);
  retval = sl_si70xx_send_command(i2cspm, addr, (uint32_t *) vData, SI7013_READ_VIN);
  if (retval != SL_STATUS_OK) {
    return retval;
  }
  sl_si70xx_write_user_register_2(i2cspm, addr, 0x09);

  return retval;
}

/**************************************************************************//**
 *   Checks if a Si7006/13/20/21 is present on the I2C bus or not.
 *****************************************************************************/
bool sl_si70xx_present(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t *device_id)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[8];
  uint8_t                    i2c_write_data[2];

  seq.addr  = addr << 1;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select command to issue */
  i2c_write_data[0] = SI70XX_READ_ID2_1;
  i2c_write_data[1] = SI70XX_READ_ID2_2;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 2;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 8;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if (ret != i2cTransferDone) {
    return false;
  }
  if (NULL != device_id) {
    *device_id = i2c_read_data[0];
  }
  return true;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/**************************************************************************//**
 * @brief
 *  Sends a command and reads the result over the I2C bus
 * @param[in] i2cspm
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] data
 *   The data read from the sensor.
 * @param[in] command
 *   The command to send to device. See the \#define's for details.
 * @return
 *    @retval SL_STATUS_OK Success
 *    @retval SL_STATUS_TRANSMIT I2C transmission error
 *****************************************************************************/
static sl_status_t sl_si70xx_send_command(sl_i2cspm_t *i2cspm, uint8_t addr, uint32_t *data,
                                          uint8_t command)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[2];
  uint8_t                    i2c_write_data[1];

  seq.addr  = addr << 1;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select command to issue */
  i2c_write_data[0] = command;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 1;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 2;

  ret = I2CSPM_Transfer(i2cspm, &seq);

  if (ret != i2cTransferDone) {
    *data = 0;
    return SL_STATUS_TRANSMIT;
  }
  *data = ((uint32_t) i2c_read_data[0] << 8) + (i2c_read_data[1] & 0xfc);
  return SL_STATUS_OK;
}

/**************************************************************************//**
 * @brief
 * Starts no hold measurement of relative humidity and temperature from a Si70xx sensor.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @return
 *    @retval SL_STATUS_OK Success
 *    @retval SL_STATUS_TRANSMIT I2C transmission error
 *****************************************************************************/
static sl_status_t sl_si70xx_start_no_hold_measure(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t command)

{
  sl_status_t retval = SL_STATUS_OK;

  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[2];
  uint8_t                    i2c_write_data[1];

  seq.addr  = addr << 1;
  seq.flags = I2C_FLAG_WRITE;
  /* Select command to issue */
  i2c_write_data[0] = command;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 1;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2cspm, &seq);

  if (ret != i2cTransferDone) {
    retval = SL_STATUS_TRANSMIT;
  }

  return retval;
}

/**************************************************************************//**
 * @brief
 *  Reads data from the Si70xx sensor.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] data
 *   The data read from the sensor.
 * @return
 *    @retval SL_STATUS_OK Success
 *    @retval SL_STATUS_TRANSMIT I2C transmission error
 *****************************************************************************/
static sl_status_t sl_si70xx_read_data(sl_i2cspm_t *i2cspm, uint8_t addr, uint32_t *data)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[2];

  seq.addr  = addr << 1;
  seq.flags = I2C_FLAG_READ;
  /* Select command to issue */
  seq.buf[0].data = i2c_read_data;
  seq.buf[0].len  = 2;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 2;

  ret = I2CSPM_Transfer(i2cspm, &seq);

  if (ret != i2cTransferDone) {
    *data = 0;
    return SL_STATUS_TRANSMIT;
  }

  *data = ((uint32_t) i2c_read_data[0] << 8) + (i2c_read_data[1] & 0xfc);

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * @brief
 *  Write to user register 2
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] data
 *   The data read from the sensor.
 * @return
 *    @retval SL_STATUS_OK Success
 *    @retval SL_STATUS_TRANSMIT I2C transmission error
 *****************************************************************************/
static sl_status_t sl_si70xx_write_user_register_2(sl_i2cspm_t *i2cspm, uint8_t addr, int8_t data)
{
  sl_status_t retval = SL_STATUS_OK;

  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[2];
  uint8_t                    i2c_write_data[2];

  seq.addr  = addr << 1;
  seq.flags = I2C_FLAG_WRITE;
  /* Select command to issue */
  i2c_write_data[0] = 0x50;
  i2c_write_data[1] = (uint8_t)data;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 2;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2cspm, &seq);

  if (ret != i2cTransferDone) {
    retval = SL_STATUS_TRANSMIT;
  }

  return retval;
}

/**************************************************************************//**
 * @brief
 *   Converts a relative humidity measurement to percent relative humidity
 *   (multiplied by 1000)
 * @param[in] rh_data
 *   Relative humidity measurement data to convert
 * @return
 *   The converted relative humidity data
 *****************************************************************************/
static uint32_t sl_si70xx_get_percent_relative_humidity(uint32_t rh_data)
{
  return (((rh_data) * 15625L) >> 13) - 6000;
}

/**************************************************************************//**
 * @brief
 *   Converts a temperature measurement to temperature in degrees Celcius
 *   (multiplied by 1000)
 * @param[in] temp_data
 *   Temperature measurement data to convert
 * @return
 *   The converted temperature measurement
 *****************************************************************************/
static int32_t sl_si70xx_get_celcius_temperature(int32_t temp_data)
{
  return (((temp_data) * 21965L) >> 13) - 46850;
}
/** @endcond */
