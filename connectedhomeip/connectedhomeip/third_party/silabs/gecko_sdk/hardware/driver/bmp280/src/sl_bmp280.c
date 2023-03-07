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
#include <stdlib.h>

#include "bmp280.h"
#include "sl_bmp280.h"
#include "sl_sleeptimer.h"

/***************************************************************************//**
 * @defgroup BAP BAP - Barometric Pressure Sensor
 * @{
 * @brief Driver for the Bosch Sensortec BMP280 pressure sensor
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define BMP280_REG_ADDR_ID                    0xD0   /**< Chip ID register                      */
#define SL_BMP280_DEVICE_ID                   0x58   /**< Device ID of the BMP280 chip          */
#define SL_BMP280_I2C_BUS_ADDRESS             0x77   /**< I2C address of the BMP280 chip        */

/***************************************************************************//**
 * Local Function Prototypes
 ******************************************************************************/
static int8_t i2c_bus_read(uint8_t devAddr, uint8_t regAddr, uint8_t *regData, uint8_t count);
static int8_t i2c_bus_write(uint8_t devAddr, uint8_t regAddr, uint8_t *regData, uint8_t count);
static sl_status_t read_register(uint8_t addr, uint8_t *regData);
static void bmp280_delay_ms(uint32_t ms);

/***************************************************************************//**
 * Local Variables
 ******************************************************************************/
static uint8_t bmp280_device_id;       /* The device ID of the connected chip  */
static uint8_t bmp280_power_mode;   /* The actual power mode of the BMP280  */
static struct  bmp280_t bmp280;   /* Structure to hold BMP280 driver data */
static sl_i2cspm_t *bmp280_i2cspm_instance;
/** @endcond */

/***************************************************************************//**
 *    Initializes the barometric pressure sensor chip
 ******************************************************************************/
sl_status_t sl_bmp280_init(sl_i2cspm_t *i2cspm)
{
  int8_t result;

  /* The device needs 2 ms startup time */
  sl_sleeptimer_delay_millisecond(2);

  /* Update i2cspm instance */
  bmp280_i2cspm_instance = i2cspm;

  /* Read device ID to determine if we have a BMP280 connected */
  read_register(BMP280_REG_ADDR_ID, &bmp280_device_id);

  if (bmp280_device_id != SL_BMP280_DEVICE_ID) {
    return SL_STATUS_NOT_FOUND;
  }

  bmp280.bus_write  = i2c_bus_write;
  bmp280.bus_read   = i2c_bus_read;
  bmp280.dev_addr   = SL_BMP280_I2C_BUS_ADDRESS;
  bmp280.delay_msec = bmp280_delay_ms;

  result = bmp280_init(&bmp280);

  if (result != SUCCESS) {
    return SL_STATUS_FAIL;
  }

  result = bmp280_set_power_mode(BMP280_FORCED_MODE);

  if (result != SUCCESS) {
    return SL_STATUS_FAIL;
  }

  result = bmp280_set_work_mode(BMP280_ULTRA_HIGH_RESOLUTION_MODE);

  if (result != SUCCESS) {
    return SL_STATUS_FAIL;
  }

  bmp280_power_mode = BMP280_FORCED_MODE;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    De-initializes the barometric pressure module
 ******************************************************************************/
void sl_bmp280_deinit(sl_i2cspm_t *i2cspm)
{
  /* Update i2cspm instance */
  bmp280_i2cspm_instance = i2cspm;
  bmp280_set_power_mode(BMP280_SLEEP_MODE);
}

/***************************************************************************//**
 *    Configure the barometric pressure sensor
 ******************************************************************************/
sl_status_t sl_bmp280_configure(sl_i2cspm_t *i2cspm, sl_bmp280_config_t *cfg)
{
  int8_t result;

  /* Update i2cspm instance */
  bmp280_i2cspm_instance = i2cspm;

  result = 0;

  result |= bmp280_set_work_mode(cfg->oversampling);
  result |= bmp280_set_power_mode(cfg->power_mode);
  bmp280_power_mode = cfg->power_mode;
  result |= bmp280_set_standby_durn(cfg->standby_time);

  if (result != SUCCESS) {
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Initiates temperature measurement on the barometric pressure sensor and
 *    reads temperature from it.
 ******************************************************************************/
sl_status_t sl_bmp280_measure_temperature(sl_i2cspm_t *i2cspm, float *temperature)
{
  int8_t result;
  int32_t uncompTemp;
  int32_t uncompPressure;
  int32_t compTemp;

  /* Update i2cspm instance */
  bmp280_i2cspm_instance = i2cspm;

  if (bmp280_power_mode == BMP280_NORMAL_MODE) {
    result = bmp280_read_uncomp_temperature(&uncompTemp);
  } else {
    result = bmp280_get_forced_uncomp_pressure_temperature(&uncompPressure, &uncompTemp);
  }

  if (result != SUCCESS) {
    return SL_STATUS_FAIL;
  }

  compTemp = bmp280_compensate_temperature_int32(uncompTemp);
  *temperature = (float) compTemp;
  *temperature /= 100.0f;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Initiates pressure measurement on the barometric pressure sensor and reads
 *    pressure from it.
 ******************************************************************************/
sl_status_t sl_bmp280_measure_pressure(sl_i2cspm_t *i2cspm, float *pressure)
{
  int8_t result;
  int32_t uncompTemp;
  int32_t uncompPressure;
  uint32_t compPressure;

  /* Update i2cspm instance */
  bmp280_i2cspm_instance = i2cspm;

  if (bmp280_power_mode == BMP280_NORMAL_MODE) {
    result = bmp280_read_uncomp_pressure(&uncompPressure);
    if (result == SUCCESS) {
      result = bmp280_read_uncomp_temperature(&uncompTemp);
    }
  } else {
    result = bmp280_get_forced_uncomp_pressure_temperature(&uncompPressure, &uncompTemp);
  }

  if (result != SUCCESS) {
    return SL_STATUS_FAIL;
  }

  bmp280_compensate_temperature_int32(uncompTemp);
  compPressure = bmp280_compensate_pressure_int64(uncompPressure);

  *pressure = (float) compPressure;
  *pressure /= 256.0f;
  *pressure /= 100.0f;

  return SL_STATUS_OK;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Reads register from the barometric pressure sensor
 *
 * @param[in] addr
 *    The register address to read from in the sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise.
 ******************************************************************************/
static sl_status_t read_register(uint8_t addr, uint8_t *regData)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = SL_BMP280_I2C_BUS_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE_READ;

  seq.buf[0].len  = 1;
  seq.buf[0].data = &addr;
  seq.buf[1].len  = 1;
  seq.buf[1].data = regData;

  ret = I2CSPM_Transfer(bmp280_i2cspm_instance, &seq);
  if (ret != i2cTransferDone) {
    return SL_STATUS_TRANSMIT;;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *    Writes data to a device via the I2C bus
 *
 * @param[in] devAddr
 *    The I2C address of the device
 *
 * @param[in] regAddr
 *    The address of the register to write to
 *
 * @param[in] regData
 *    The data to write
 *
 * @param[in] count
 *    The number of data bytes to write
 *
 * @return
 *    *    Returns zero on OK, -1 on failure.
 ******************************************************************************/
static int8_t i2c_bus_write(uint8_t devAddr, uint8_t regAddr, uint8_t *regData, uint8_t count)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = devAddr << 1;
  seq.flags = I2C_FLAG_WRITE_WRITE;

  seq.buf[0].len  = 1;
  seq.buf[0].data = &regAddr;
  seq.buf[1].len  = count;
  seq.buf[1].data = regData;

  ret = I2CSPM_Transfer(bmp280_i2cspm_instance, &seq);
  if (ret != i2cTransferDone) {
    return -1;
  }

  return 0;
}

/***************************************************************************//**
 * @brief
 *    Reads data from a device via the I2C bus
 *
 * @param[in] devAddr
 *    The I2C address of the device
 *
 * @param[in] regAddr
 *    The address of the register to read from
 *
 * @param[out] regData
 *    The data read from the device
 *
 * @param[in] count
 *    The number of data bytes to read
 *
 * @return
 *    Returns zero on OK, -1 on failure.
 ******************************************************************************/
static int8_t i2c_bus_read(uint8_t devAddr, uint8_t regAddr, uint8_t *regData, uint8_t count)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = devAddr << 1;
  seq.flags = I2C_FLAG_WRITE_READ;

  seq.buf[0].len  = 1;
  seq.buf[0].data = &regAddr;
  seq.buf[1].len  = count;
  seq.buf[1].data = regData;

  ret = I2CSPM_Transfer(bmp280_i2cspm_instance, &seq);

  if (ret != i2cTransferDone) {
    return -1;
  }

  return 0;
}

/***************************************************************************//**
 * @brief
 *    Delay routine for the bmp280 sensor
 *
 * @param[in] ms
 *    Delay duration in ms
 ******************************************************************************/
static void bmp280_delay_ms(uint32_t ms)
{
  sl_sleeptimer_delay_millisecond((uint16_t)ms);
}
/** @endcond */

/** @} */
