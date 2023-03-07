/***************************************************************************//**
 * @file
 * @brief Driver for the VEML6035 ambient light sensor
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

#include "sl_veml6035.h"
#include "sl_sleeptimer.h"
#include "sl_i2cspm.h"
/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * VEML6035 i2c address
 ******************************************************************************/
#define SL_VEML6035_I2C_ADDRESS  0X29

/***************************************************************************//**
 * VEML6035 register addresses
 ******************************************************************************/
#define SL_VEML6035_ALS_CONF     0x00
#define SL_VEML6035_WH           0x01
#define SL_VEML6035_WL           0x02
#define SL_VEML6035_PSM          0x03
#define SL_VEML6035_ALS          0x04
#define SL_VEML6035_WHITE        0x05
#define SL_VEML6035_IF           0x06

/***************************************************************************//**
 * Local prototypes
 ******************************************************************************/

static sl_status_t veml6035_get_resolution(sl_i2cspm_t *i2cspm, float *resolution);

static sl_status_t veml6035_read_als_raw(sl_i2cspm_t *i2cspm, bool white_ch, uint16_t *als);

static sl_status_t veml6035_read_register(sl_i2cspm_t *i2cspm, uint8_t reg, uint16_t *data);

static sl_status_t veml6035_write_register(sl_i2cspm_t *i2cspm, uint8_t reg, uint16_t data);

static sl_status_t veml6035_write_register_field(sl_i2cspm_t *i2cspm, uint8_t reg, uint16_t data, uint16_t mask);
/** @endcond */

/**************************************************************************//**
 *    Initialize and enable the VEML6035 sensor to operate in normal mode.
 *****************************************************************************/
sl_status_t sl_veml6035_init(sl_i2cspm_t *i2cspm, bool white_enable)
{
  sl_status_t status;

  // Do not access sensor too early following power-up
  sl_sleeptimer_delay_millisecond(10);

  status = sl_veml6035_reset(i2cspm);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Configure to lowest sensitivity (highest range)
  status = sl_veml6035_configure_sensitivity(i2cspm, true, 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if (white_enable) {
    status = veml6035_write_register_field(i2cspm, SL_VEML6035_ALS_CONF, SL_VEML6035_CHANNEL_EN_ALS_WHITE, SL_VEML6035_CHANNEL_EN_MASK);
    if (status != SL_STATUS_OK) {
      return status;
    }
  }

  status = sl_veml6035_enable_sensor(i2cspm, true);

  return status;
}

/***************************************************************************//**
 *    Reset all writeable registers of the VEML6035.
 ******************************************************************************/
sl_status_t sl_veml6035_reset(sl_i2cspm_t *i2cspm)
{
  sl_status_t status;
  uint16_t regdata;

  // Reset ALS_CONF register
  status = veml6035_read_register(i2cspm, SL_VEML6035_ALS_CONF, &regdata);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Bit 15:13 are reserved
  regdata &= 0xE000;
  regdata |= SL_VEML6035_SD_OFF;

  status = veml6035_write_register(i2cspm, SL_VEML6035_ALS_CONF, regdata);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Reset threshold window setting registers
  status |= veml6035_write_register(i2cspm, SL_VEML6035_WH, 0x0000);
  status |= veml6035_write_register(i2cspm, SL_VEML6035_WL, 0x0000);

  // Reset PSM settings register
  status |= veml6035_write_register_field(i2cspm, SL_VEML6035_PSM, 0x0000,
                                          SL_VEML6035_PSM_WAIT_MASK | SL_VEML6035_PSM_EN_MASK);

  return status;
}

/***************************************************************************//**
 *    Enable or disable sensor measurements.
 ******************************************************************************/
sl_status_t sl_veml6035_enable_sensor(sl_i2cspm_t *i2cspm, bool enable)
{
  sl_status_t status;

  uint16_t data = SL_VEML6035_SD_OFF;
  if (enable) {
    data = SL_VEML6035_SD_ON;
  }

  status = veml6035_write_register_field(i2cspm, SL_VEML6035_ALS_CONF, data, SL_VEML6035_SD_MASK);

  return status;
}

/**************************************************************************//**
 *    Configure sensor sensitivity settings.
 *****************************************************************************/
sl_status_t sl_veml6035_configure_sensitivity(sl_i2cspm_t *i2cspm, bool low_sensitivity, uint8_t gain)
{
  sl_status_t status;
  uint16_t data_mask = (SL_VEML6035_SENS_MASK | SL_VEML6035_GAIN_MASK | SL_VEML6035_DG_MASK);
  uint16_t data = 0x0000;

  if (low_sensitivity) {
    data |= SL_VEML6035_SENS_LOW;
  }

  if (gain == 2) {
    data |= SL_VEML6035_GAIN_DOUBLE;
  } else if (gain == 4) {
    data |= SL_VEML6035_GAIN_DOUBLE;
    data |= SL_VEML6035_DG_DOUBLE;
  } else if (gain != 1) {
    // Invalid gain val
    return SL_STATUS_INVALID_PARAMETER;
  }

  status = veml6035_write_register_field(i2cspm, SL_VEML6035_ALS_CONF, data, data_mask);

  return status;
}

/**************************************************************************//**
 *    Set integration time.
 *****************************************************************************/
sl_status_t sl_veml6035_configure_integration_time(sl_i2cspm_t *i2cspm,
                                                   sl_veml6035_integration_time_t integration_time)
{
  sl_status_t status;
  status = veml6035_write_register_field(i2cspm, SL_VEML6035_ALS_CONF, integration_time, SL_VEML6035_ALS_IT_MASK);

  return status;
}

/***********************************************************************//**
 * Get the lux value from the ALS channel.
 **************************************************************************/
sl_status_t sl_veml6035_get_als_lux(sl_i2cspm_t *i2cspm, float *lux)
{
  sl_status_t status;
  uint16_t als_counts;
  float resolution;

  status = veml6035_read_als_raw(i2cspm, false, &als_counts);
  if (status != SL_STATUS_OK) {
    return status;
  }

  status = veml6035_get_resolution(i2cspm, &resolution);
  if (status != SL_STATUS_OK) {
    return status;
  }

  *lux = resolution * (float)als_counts;

  return status;
}

/***********************************************************************//**
 * Get the lux value from the WHITE channel if enabled.
 **************************************************************************/
sl_status_t sl_veml6035_get_white_lux(sl_i2cspm_t *i2cspm, float *lux)
{
  sl_status_t status;

  uint16_t als_counts;
  uint16_t regdata;
  float resolution;

  status = veml6035_read_register(i2cspm, SL_VEML6035_ALS_CONF, &regdata);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if ((regdata & SL_VEML6035_CHANNEL_EN_MASK) != SL_VEML6035_CHANNEL_EN_ALS_WHITE) {
    // White channel not enabled
    return SL_STATUS_INVALID_CONFIGURATION;
  }

  status = veml6035_read_als_raw(i2cspm, true, &als_counts);
  if (status != SL_STATUS_OK) {
    return status;
  }

  status = veml6035_get_resolution(i2cspm, &resolution);
  if (status != SL_STATUS_OK) {
    return status;
  }

  *lux = resolution * (float)als_counts;

  return status;
}

/**************************************************************************//**
 *      Configure interrupt mode.
 *****************************************************************************/
sl_status_t sl_veml6035_configure_interrupt_mode(sl_i2cspm_t *i2cspm,
                                                 uint16_t high_threshold,
                                                 uint16_t low_threshold,
                                                 sl_veml6035_als_pers_t persistence,
                                                 bool white_enable,
                                                 bool enable)
{
  sl_status_t status;
  uint16_t data;

  // Set thresholds
  status = veml6035_write_register(i2cspm, SL_VEML6035_WH, high_threshold);
  if (status != SL_STATUS_OK) {
    return status;
  }

  status = veml6035_write_register(i2cspm, SL_VEML6035_WL, low_threshold);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Set interrupt channel
  data = SL_VEML6035_INT_CHANNEL_ALS;
  if (white_enable) {
    data = SL_VEML6035_INT_CHANNEL_WHITE;
  }

  status = veml6035_write_register_field(i2cspm, SL_VEML6035_ALS_CONF, data, SL_VEML6035_INT_CHANNEL_MASK);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Set persistence
  status = veml6035_write_register_field(i2cspm, SL_VEML6035_ALS_CONF, persistence, SL_VEML6035_ALS_PERS_MASK);
  if (status != SL_STATUS_OK) {
    return status;
  }

  status = sl_veml6035_enable_interrupt_mode(i2cspm, enable);

  return status;
}

/***************************************************************************//**
 *      Enable or disable threshold interrupts.
 *****************************************************************************/
sl_status_t sl_veml6035_enable_interrupt_mode(sl_i2cspm_t *i2cspm, bool enable)
{
  sl_status_t status;
  uint16_t data;

  data = SL_VEML6035_INT_EN_DIS;
  if (enable) {
    data = SL_VEML6035_INT_EN_EN;
  }
  status = veml6035_write_register_field(i2cspm, SL_VEML6035_ALS_CONF, data, SL_VEML6035_INT_EN_MASK);
  return status;
}

/**************************************************************************//**
 *    Read threshold interrupt status register and return interrupt status
 *    for high and low threshold.
 *****************************************************************************/
sl_status_t sl_veml6035_read_interrupt_status(sl_i2cspm_t *i2cspm, bool *threshold_low, bool *threshold_high)
{
  sl_status_t status;
  uint16_t regdata;

  *threshold_low = false;
  *threshold_high = false;

  status = veml6035_read_register(i2cspm, SL_VEML6035_IF, &regdata);
  if (status != SL_STATUS_OK) {
    return status;
  }

  if ((regdata >> 15) & 0x01) {
    *threshold_low = true;
  }

  if ((regdata >> 14) & 0x01) {
    *threshold_high = true;
  }

  return status;
}

/**************************************************************************//**
 *    Enable power save mode, with a defined wait time between measurements.
 *****************************************************************************/
sl_status_t sl_veml6035_configure_psm(sl_i2cspm_t *i2cspm, sl_veml6035_psm_wait_t psm_wait, bool enable)
{
  sl_status_t status;
  uint16_t data;

  status = veml6035_write_register_field(i2cspm, SL_VEML6035_PSM, psm_wait, SL_VEML6035_PSM_WAIT_MASK);
  if (status != SL_STATUS_OK) {
    return status;
  }

  data = SL_VEML6035_PSM_EN_DIS;
  if (enable) {
    data = SL_VEML6035_PSM_EN_EN;
  }
  status = veml6035_write_register_field(i2cspm, SL_VEML6035_PSM, data, SL_VEML6035_PSM_EN_MASK);

  return status;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/**************************************************************************//**
 * @brief Get resolution value from VEML6035 gain configurations.
 *
 * @param[in] i2cspm
 *  The I2C peripheral to use
 * @param[out] resolution
 *  The calculated resolution
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmission failure
 * @retval SL_STATUS_INVALID_CONFIGURATION Invalid sensor configuration
 *****************************************************************************/
static sl_status_t veml6035_get_resolution(sl_i2cspm_t *i2cspm, float *resolution)
{
  sl_status_t status;
  uint16_t conf_register;
  uint16_t sensitivity, dg, gain;
  uint16_t als_it;

  // Maximum possible resolution
  float res = 0.0004;

  status = veml6035_read_register(i2cspm, SL_VEML6035_ALS_CONF, &conf_register);
  if (status != SL_STATUS_OK) {
    return status;
  }

  sensitivity = conf_register & SL_VEML6035_SENS_MASK;
  dg          = conf_register & SL_VEML6035_DG_MASK;
  gain        = conf_register & SL_VEML6035_GAIN_MASK;
  als_it      = conf_register & SL_VEML6035_ALS_IT_MASK;

  if (sensitivity == SL_VEML6035_SENS_LOW) {
    res = res * 8;
  }

  if (dg == SL_VEML6035_DG_NORMAL) {
    res = res * 2;
  }

  if (gain == SL_VEML6035_GAIN_NORMAL) {
    res = res * 2;
  }

  switch (als_it) {
    case SL_VEML6035_ALS_IT_800MS:
      // Do nothing
      break;

    case SL_VEML6035_ALS_IT_400MS:
      res = res * 2;
      break;

    case SL_VEML6035_ALS_IT_200MS:
      res = res * 4;
      break;

    case SL_VEML6035_ALS_IT_100MS:
      res = res * 8;
      break;

    case SL_VEML6035_ALS_IT_50MS:
      res = res * 16;
      break;

    case SL_VEML6035_ALS_IT_25MS:
      res = res * 32;
      break;

    default:
      // Invalid integration time configuration
      return SL_STATUS_INVALID_CONFIGURATION;
  }

  *resolution = res;

  return status;
}

/***********************************************************************//**
 * @brief Read out ALS raw data.
 *
 * @param[in] i2cspm
 *  The I2C peripheral to use
 * @param[in] white_ch
 *  Set to true to read raw data from WHITE channel
 * @param[out] als
 *  The raw ALS data for ALS or WHITE channel
 *
 * @retval SL_STATUS_OK
 * @retval SL_STATUS_TRANSMIT I2C transmission failure
 **************************************************************************/
static sl_status_t veml6035_read_als_raw(sl_i2cspm_t *i2cspm, bool white_ch, uint16_t *als)
{
  sl_status_t status = SL_STATUS_OK;
  uint16_t als_reg;

  /* Read out ALS or WHITE register */
  if (white_ch) {
    status = veml6035_read_register(i2cspm, SL_VEML6035_WHITE, &als_reg);
  } else {
    status = veml6035_read_register(i2cspm, SL_VEML6035_ALS, &als_reg);
  }

  *als = als_reg;

  return status;
}

/***********************************************************************//**
 * @brief Read register from the VEML6035 sensor.
 *
 * @param[in] i2cspm
 *  The I2C peripheral to use
 * @param[in] reg
 *  Register address
 * @param[out] data
 *  Data read from the register
 *
 * @retval SL_STATUS_OK
 * @retval SL_STATUS_TRANSMIT I2C transmission failure
 **************************************************************************/
static sl_status_t veml6035_read_register(sl_i2cspm_t *i2cspm, uint8_t reg, uint16_t *data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];
  uint8_t i2c_read_data[2];
  sl_status_t status;

  status = SL_STATUS_OK;

  seq.addr = SL_VEML6035_I2C_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to start reading from */
  i2c_write_data[0] = reg;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len = 1;
  /* Select length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len = 2;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    return SL_STATUS_TRANSMIT;
  } else {
    *data = (uint16_t) (i2c_read_data[1] << 8) + (uint16_t)i2c_read_data[0];
  }

  return status;
}

/***********************************************************************//**
 * @brief Write VEML6035 register field.
 *
 * @param[in] i2cspm
 *  The I2C peripheral to use
 * @param[in] reg
 *  Register address
 * @param[in] data
 *  Data to write
 * @param[in] mask
 *  Bit mask corresponding to target bit field
 *
 * @retval SL_STATUS_OK
 * @retval SL_STATUS_TRANSMIT I2C transmission failure
 **************************************************************************/
static sl_status_t veml6035_write_register_field(sl_i2cspm_t *i2cspm, uint8_t reg, uint16_t data, uint16_t mask)
{
  sl_status_t status;
  uint16_t temp;

  status = veml6035_read_register(i2cspm, reg, &temp);
  if (status != SL_STATUS_OK) {
    return status;
  }

  temp = (temp & ~mask) | (data & mask);
  status = veml6035_write_register(i2cspm, reg, temp);

  return status;
}

/***********************************************************************//**
 * @brief Write VEML6035 register.
 * @param[in] i2cspm
 *  The I2C peripheral to use
 * @param[in] reg
 *  Register address
 * @param[in] data
 *  Data to write
 *
 * @retval SL_STATUS_OK
 * @retval SL_STATUS_TRANSMIT I2C transmission failure
 **************************************************************************/
static sl_status_t veml6035_write_register(sl_i2cspm_t *i2cspm, uint8_t reg, uint16_t data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[3];
  uint8_t i2c_read_data[1];

  sl_status_t status;

  status = SL_STATUS_OK;

  seq.addr = SL_VEML6035_I2C_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE;
  /* Select register to start writing to*/
  i2c_write_data[0] = reg;
  i2c_write_data[1] = (uint8_t) (data & 0x00FF);
  i2c_write_data[2] = (uint8_t) ((data >> 8) & 0x00FF);
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len = 3;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len = 0;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    status = SL_STATUS_TRANSMIT;
  }

  return status;
}
/** @endcond */
