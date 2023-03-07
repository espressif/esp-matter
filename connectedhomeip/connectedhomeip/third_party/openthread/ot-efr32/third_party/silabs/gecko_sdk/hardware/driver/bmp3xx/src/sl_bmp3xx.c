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

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "bmp3.h"
#include "sl_bmp3xx.h"
#include "ustimer.h"

/***************************************************************************//**
 * @defgroup BAP BAP - Barometric Pressure Sensor
 * @{
 * @brief Driver for the Bosch Sensortec BMP3xx pressure sensor
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define BMP3XX_REG_ADDR_ID                      0x00   /**< Chip ID register                      */
#define SL_BMP3XX_DEVICE_ID                     0x50   /**< Device ID of the BMP3xx chip          */
#define SL_BMP3XX_I2C_BUS_ADDRESS               0x77   /**< I2C address of the BMP3xx chip        */

/***************************************************************************//**
 * Local Function Prototypes
 ******************************************************************************/
static int8_t i2c_bus_read(uint8_t regAddr, uint8_t *regData, uint32_t count, void *intf_ptr);
static int8_t i2c_bus_write(uint8_t regAddr, const uint8_t *regData, uint32_t count, void *intf_ptr);
static sl_status_t read_register(uint8_t addr, uint8_t *regData);
static void bmp3xx_delay_us(uint32_t us, void *intf_ptr);

/***************************************************************************//**
 * Local Variables
 ******************************************************************************/
static uint8_t bmp3xx_device_id;       /* The device ID of the connected chip  */
static uint8_t bmp3xx_device_addr;     /* The device address of the connected chip  */
static uint8_t bmp3xx_power_mode;      /* The actual power mode of the BMP3xx  */
static struct  bmp3_dev bmp3xx;        /* Structure to hold BMP3xx driver data */
static sl_i2cspm_t *bmp3xx_i2cspm_instance;
static struct  bmp3_settings settings;    /* Structure to hold BMP3xx settings */
static uint16_t desired_settings;         /* Variable used to select the settings
                                             to be set in the sensor */
/** @endcond */

/***************************************************************************//**
 *    Initializes the barometric pressure sensor chip
 ******************************************************************************/
sl_status_t sl_bmp3xx_init(sl_i2cspm_t *i2cspm)
{
  int8_t result;
  sl_bmp3xx_config_t config;

  // The device needs 2 ms startup time
  USTIMER_Init();
  USTIMER_Delay(2000);

  bmp3xx_device_addr = SL_BMP3XX_I2C_BUS_ADDRESS;

  // Update i2cspm instance
  bmp3xx_i2cspm_instance = i2cspm;

  // Read device ID to determine if we have a BMP3xx connected
  result = read_register(BMP3XX_REG_ADDR_ID, &bmp3xx_device_id);

  if (result != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  if (bmp3xx_device_id != SL_BMP3XX_DEVICE_ID) {
    return SL_STATUS_NOT_FOUND;
  }

  // Initialize BMP3xx
  bmp3xx.intf_ptr = &bmp3xx_device_addr;
  bmp3xx.intf     = BMP3_I2C_INTF;
  bmp3xx.write    = i2c_bus_write;
  bmp3xx.read     = i2c_bus_read;
  bmp3xx.delay_us = bmp3xx_delay_us;

  result = bmp3_init(&bmp3xx);

  if (result != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  // Configure BMP3xx
  config.settings.press_en = BMP3_ENABLE;
  config.settings.temp_en = BMP3_ENABLE;
  config.desired_settings = BMP3_SEL_PRESS_EN | BMP3_SEL_TEMP_EN;
  // Set forced mode
  config.settings.op_mode = BMP3_MODE_FORCED;

  result = sl_bmp3xx_set_config(bmp3xx_i2cspm_instance, &config);

  if (result != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  settings = config.settings;
  desired_settings = config.desired_settings;

  bmp3xx_power_mode = BMP3_MODE_FORCED;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    De-initializes the barometric pressure module
 ******************************************************************************/
void sl_bmp3xx_deinit(sl_i2cspm_t *i2cspm)
{
  // Update i2cspm instance
  bmp3xx_i2cspm_instance = i2cspm;

  settings.op_mode = BMP3_MODE_SLEEP;
  bmp3_set_op_mode(&settings, &bmp3xx);
}

/***************************************************************************//**
 *    Set the barometric pressure sensor configuration
 ******************************************************************************/
sl_status_t sl_bmp3xx_set_config(sl_i2cspm_t *i2cspm, sl_bmp3xx_config_t *cfg)
{
  int8_t result;

  // Update i2cspm instance
  bmp3xx_i2cspm_instance = i2cspm;

  desired_settings = cfg->desired_settings;
  settings = cfg->settings;

  result = bmp3_set_sensor_settings(desired_settings, &settings, &bmp3xx);
  result |= bmp3_set_op_mode(&settings, &bmp3xx);
  bmp3xx_power_mode = settings.op_mode;

  if (result != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Get the barometric pressure sensor configuration
 ******************************************************************************/
void sl_bmp3xx_get_config(sl_i2cspm_t *i2cspm, sl_bmp3xx_config_t *cfg)
{
  // Update i2cspm instance
  bmp3xx_i2cspm_instance = i2cspm;

  cfg->settings = settings;
  cfg->desired_settings = desired_settings;
}

/***************************************************************************//**
 *    Initiates temperature measurement on the barometric pressure sensor and
 *    reads temperature from it.
 ******************************************************************************/
sl_status_t sl_bmp3xx_measure_temperature(sl_i2cspm_t *i2cspm, float *temperature)
{
  int8_t result;
  struct bmp3_status status;
  struct bmp3_data data;

  // Update i2cspm instance
  bmp3xx_i2cspm_instance = i2cspm;

  // Flush any previous data before measuring
  result = bmp3_get_sensor_data(BMP3_TEMP, &data, &bmp3xx);

  if (result != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  if (bmp3xx_power_mode == BMP3_MODE_FORCED) {
    // In Forced Mode, a conversion starts after setting operation mode
    bmp3_set_op_mode(&settings, &bmp3xx);
  }

  // Check status register to make sure the conversion is finished
  result = bmp3_get_status(&status, &bmp3xx);
  if (result != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }
  // Wait for the conversion to finish
  while (status.intr.drdy != BMP3_ENABLE) {
    result = bmp3_get_status(&status, &bmp3xx);
    if (result != SL_STATUS_OK) {
      return SL_STATUS_FAIL;
    }
  }

  // Read temperature value
  result = bmp3_get_sensor_data(BMP3_TEMP, &data, &bmp3xx);

  if (result != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  *temperature = (float) data.temperature;
#ifndef BMP3_FLOAT_COMPENSATION
  *temperature /= 100.0f;
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Initiates pressure measurement on the barometric pressure sensor and reads
 *    pressure from it.
 ******************************************************************************/
sl_status_t sl_bmp3xx_measure_pressure(sl_i2cspm_t *i2cspm, float *pressure)
{
  int8_t result;
  struct bmp3_status status;
  struct bmp3_data data;

  // Update i2cspm instance
  bmp3xx_i2cspm_instance = i2cspm;

  // Flush any previous data before measuring
  result = bmp3_get_sensor_data(BMP3_TEMP, &data, &bmp3xx);

  if (result != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  if (bmp3xx_power_mode == BMP3_MODE_FORCED) {
    // In Forced Mode, a conversion starts after setting operation mode
    bmp3_set_op_mode(&settings, &bmp3xx);
  }

  // Check status register to make sure the conversion is finished
  result = bmp3_get_status(&status, &bmp3xx);
  if (result != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }
  // Wait for the conversion to finish
  while (status.intr.drdy != BMP3_ENABLE) {
    result = bmp3_get_status(&status, &bmp3xx);
    if (result != SL_STATUS_OK) {
      return SL_STATUS_FAIL;
    }
  }

  // Read pressure value
  result = bmp3_get_sensor_data(BMP3_PRESS, &data, &bmp3xx);

  if (result != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  *pressure = (float) data.pressure;
#ifndef BMP3_FLOAT_COMPENSATION
  *pressure = / 100.0f;
#endif

  return SL_STATUS_OK;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Reads register from the barometric pressure sensor
 *
 * @param[in] addr
 *    The address of the register to read from
 *
 * @param[out] regData
 *    Pointer to read register data
 *
 * @return
 *    Returns SL_STATUS_OK on OK, SL_STATUS_FAIL on failure.
 ******************************************************************************/
static sl_status_t read_register(uint8_t addr, uint8_t *regData)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = bmp3xx_device_addr << 1;
  seq.flags = I2C_FLAG_WRITE_READ;

  seq.buf[0].len  = 1;
  seq.buf[0].data = &addr;
  seq.buf[1].len  = 1;
  seq.buf[1].data = regData;

  ret = I2CSPM_Transfer(bmp3xx_i2cspm_instance, &seq);
  if (ret != i2cTransferDone) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *    Writes data to a device via the I2C bus
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
 * @param[in] intf_ptr
 *    Pointer to device I2C address
 *
 * @return
 *    Returns SL_STATUS_OK on OK, SL_STATUS_FAIL on failure.
 ******************************************************************************/
static int8_t i2c_bus_write(uint8_t regAddr, const uint8_t *regData, uint32_t count, void *intf_ptr)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  uint8_t dev_addr = *(uint8_t*)intf_ptr;

  seq.addr = dev_addr << 1;
  seq.flags = I2C_FLAG_WRITE_WRITE;

  seq.buf[0].len  = 1;
  seq.buf[0].data = &regAddr;
  seq.buf[1].len  = count;
  seq.buf[1].data = (uint8_t *)regData;

  ret = I2CSPM_Transfer(bmp3xx_i2cspm_instance, &seq);
  if (ret != i2cTransferDone) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *    Reads data from a device via the I2C bus
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
 * @param[in] intf_ptr
 *    Pointer to device I2C address
 *
 * @return
 *    Returns SL_STATUS_OK on OK, SL_STATUS_FAIL on failure.
 ******************************************************************************/
static int8_t i2c_bus_read(uint8_t regAddr, uint8_t *regData, uint32_t count, void *intf_ptr)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  uint8_t dev_addr = *(uint8_t *)intf_ptr;

  seq.addr = dev_addr << 1;
  seq.flags = I2C_FLAG_WRITE_READ;

  seq.buf[0].len  = 1;
  seq.buf[0].data = &regAddr;
  seq.buf[1].len  = count;
  seq.buf[1].data = regData;

  ret = I2CSPM_Transfer(bmp3xx_i2cspm_instance, &seq);
  if (ret != i2cTransferDone) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *    Delay routine for the bmp3 sensor
 *
 * @param[in] us
 *    Delay duration in us
 *
 * @param[in] intf_ptr
 *    Pointer to device I2C address
 ******************************************************************************/
static void bmp3xx_delay_us(uint32_t us, void *intf_ptr)
{
  (void)(intf_ptr);
  USTIMER_Delay(us);
}
/** @endcond */

/** @} */
