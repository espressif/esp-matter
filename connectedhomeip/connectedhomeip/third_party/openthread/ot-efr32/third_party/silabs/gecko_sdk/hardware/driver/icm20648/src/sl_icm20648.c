/***************************************************************************//**
 * @file
 * @brief Driver for the Invensense ICM20648 6-axis motion sensor
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
#include <stdio.h>
#include "em_usart.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "sl_sleeptimer.h"
#include "sl_icm20648.h"
#include "sl_icm20648_config.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static void sl_icm20648_chip_select_set(bool select);

/* Concatenate preprocessor tokens A and B. */
#define SL_CONCAT(A, B) A ## B

/* Generate the cmu clock symbol based on instance. */
#define ICM20648_SPI_CLK(N) SL_CONCAT(cmuClock_USART, N)

/** @endcond */

/***************************************************************************//**
 *    Initializes the ICM20648 sensor. Enables the power supply and SPI lines,
 *    sets up the host SPI controller, configures the chip control interface,
 *    clock generator and interrupt line.
 ******************************************************************************/
sl_status_t sl_icm20648_init(void)
{
  uint8_t data;

  /* Enable and setup the SPI bus */
  sl_icm20648_spi_init();

  /* Issue reset */
  sl_icm20648_reset();

  /* Disable I2C interface, use SPI */
  sl_icm20648_write_register(ICM20648_REG_USER_CTRL, ICM20648_BIT_I2C_IF_DIS);

  /* Read Who am I register, should get 0x71 */
  sl_icm20648_read_register(ICM20648_REG_WHO_AM_I, 1, &data);

  /* If not - return */
  if ( (data != ICM20648_DEVICE_ID) && (data != ICM20948_DEVICE_ID) ) {
    return SL_STATUS_INITIALIZATION;
  }

  /* Auto selects the best available clock source - PLL if ready, else use the Internal oscillator */
  sl_icm20648_write_register(ICM20648_REG_PWR_MGMT_1, ICM20648_BIT_CLK_PLL);

  /* PLL startup time - maybe it is too long but better be on the safe side, no spec in the datasheet */
  sl_sleeptimer_delay_millisecond(30);

  /* INT pin: active low, open drain, IT status read clears. It seems that latched mode does not work, the INT pin cannot be cleared if set */
  sl_icm20648_write_register(ICM20648_REG_INT_PIN_CFG, ICM20648_BIT_INT_ACTL | ICM20648_BIT_INT_OPEN);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    De-initializes the ICM20648 sensor by disconnecting the supply and SPI
 *    lines.
 ******************************************************************************/
sl_status_t sl_icm20648_deinit(void)
{
  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Performs soft reset on the ICM20648 chip
 ******************************************************************************/
sl_status_t sl_icm20648_reset(void)
{
  /* Set H_RESET bit to initiate soft reset */
  sl_icm20648_write_register(ICM20648_REG_PWR_MGMT_1, ICM20648_BIT_H_RESET);

  /* Wait 100ms to complete the reset sequence */
  sl_sleeptimer_delay_millisecond(100);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads the raw acceleration value and converts to g value based on
 *    the actual resolution
 ******************************************************************************/
sl_status_t sl_icm20648_accel_read_data(float *accel)
{
  uint8_t rawData[6];
  float accelRes;
  int16_t temp;

  /* Retrieve the current resolution */
  sl_icm20648_accel_get_resolution(&accelRes);

  /* Read the six raw data registers into data array */
  sl_icm20648_read_register(ICM20648_REG_ACCEL_XOUT_H_SH, 6, &rawData[0]);

  /* Convert the MSB and LSB into a signed 16-bit value and multiply by the resolution to get the G value */
  temp = ( (int16_t) rawData[0] << 8) | rawData[1];
  accel[0] = (float) temp * accelRes;
  temp = ( (int16_t) rawData[2] << 8) | rawData[3];
  accel[1] = (float) temp * accelRes;
  temp = ( (int16_t) rawData[4] << 8) | rawData[5];
  accel[2] = (float) temp * accelRes;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads the raw gyroscope value and converts to deg/sec value based on
 *    the actual resolution
 ******************************************************************************/
sl_status_t sl_icm20648_gyro_read_data(float *gyro)
{
  uint8_t rawData[6];
  float gyroRes;
  int16_t temp;

  /* Retrieve the current resolution */
  sl_icm20648_gyro_get_resolution(&gyroRes);

  /* Read the six raw data registers into data array */
  sl_icm20648_read_register(ICM20648_REG_GYRO_XOUT_H_SH, 6, &rawData[0]);

  /* Convert the MSB and LSB into a signed 16-bit value and multiply by the resolution to get the dps value */
  temp = ( (int16_t) rawData[0] << 8) | rawData[1];
  gyro[0] = (float) temp * gyroRes;
  temp = ( (int16_t) rawData[2] << 8) | rawData[3];
  gyro[1] = (float) temp * gyroRes;
  temp = ( (int16_t) rawData[4] << 8) | rawData[5];
  gyro[2] = (float) temp * gyroRes;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Gets the actual resolution of the accelerometer
 ******************************************************************************/
sl_status_t sl_icm20648_accel_get_resolution(float *accelRes)
{
  uint8_t reg;

  /* Read the actual acceleration full scale setting */
  sl_icm20648_read_register(ICM20648_REG_ACCEL_CONFIG, 1, &reg);
  reg &= ICM20648_MASK_ACCEL_FULLSCALE;

  /* Calculate the resolution */
  switch ( reg ) {
    case ICM20648_ACCEL_FULLSCALE_2G:
      *accelRes = 2.0 / 32768.0;
      break;

    case ICM20648_ACCEL_FULLSCALE_4G:
      *accelRes = 4.0 / 32768.0;
      break;

    case ICM20648_ACCEL_FULLSCALE_8G:
      *accelRes = 8.0 / 32768.0;
      break;

    case ICM20648_ACCEL_FULLSCALE_16G:
      *accelRes = 16.0 / 32768.0;
      break;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Gets the actual resolution of the gyroscope
 ******************************************************************************/
sl_status_t sl_icm20648_gyro_get_resolution(float *gyroRes)
{
  uint8_t reg;

  /* Read the actual gyroscope full scale setting */
  sl_icm20648_read_register(ICM20648_REG_GYRO_CONFIG_1, 1, &reg);
  reg &= ICM20648_MASK_GYRO_FULLSCALE;

  /* Calculate the resolution */
  switch ( reg ) {
    case ICM20648_GYRO_FULLSCALE_250DPS:
      *gyroRes = 250.0 / 32768.0;
      break;

    case ICM20648_GYRO_FULLSCALE_500DPS:
      *gyroRes = 500.0 / 32768.0;
      break;

    case ICM20648_GYRO_FULLSCALE_1000DPS:
      *gyroRes = 1000.0 / 32768.0;
      break;

    case ICM20648_GYRO_FULLSCALE_2000DPS:
      *gyroRes = 2000.0 / 32768.0;
      break;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Sets the full scale value of the accelerometer
 ******************************************************************************/
sl_status_t sl_icm20648_accel_set_full_scale(uint8_t accelFs)
{
  uint8_t reg;

  accelFs &= ICM20648_MASK_ACCEL_FULLSCALE;
  sl_icm20648_read_register(ICM20648_REG_ACCEL_CONFIG, 1, &reg);
  reg &= ~(ICM20648_MASK_ACCEL_FULLSCALE);
  reg |= accelFs;
  sl_icm20648_write_register(ICM20648_REG_ACCEL_CONFIG, reg);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Sets the full scale value of the gyroscope
 ******************************************************************************/
sl_status_t sl_icm20648_gyro_set_full_scale(uint8_t gyroFs)
{
  uint8_t reg;

  gyroFs &= ICM20648_MASK_GYRO_FULLSCALE;
  sl_icm20648_read_register(ICM20648_REG_GYRO_CONFIG_1, 1, &reg);
  reg &= ~(ICM20648_MASK_GYRO_FULLSCALE);
  reg |= gyroFs;
  sl_icm20648_write_register(ICM20648_REG_GYRO_CONFIG_1, reg);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Sets the sample rate both of the accelerometer and the gyroscope.
 ******************************************************************************/
sl_status_t sl_icm20648_set_sample_rate(float sampleRate)
{
  sl_icm20648_gyro_set_sample_rate(sampleRate);
  sl_icm20648_accel_set_sample_rate(sampleRate);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Sets the sample rate of the accelerometer
 ******************************************************************************/
float sl_icm20648_gyro_set_sample_rate(float sampleRate)
{
  uint8_t gyroDiv;
  float gyroSampleRate;

  /* Calculate the sample rate divider */
  gyroSampleRate = (1125.0 / sampleRate) - 1.0;

  /* Check if it fits in the divider register */
  if ( gyroSampleRate > 255.0 ) {
    gyroSampleRate = 255.0;
  }

  if ( gyroSampleRate < 0 ) {
    gyroSampleRate = 0.0;
  }

  /* Write the value to the register */
  gyroDiv = (uint8_t) gyroSampleRate;
  sl_icm20648_write_register(ICM20648_REG_GYRO_SMPLRT_DIV, gyroDiv);

  /* Calculate the actual sample rate from the divider value */
  gyroSampleRate = 1125.0 / (gyroDiv + 1);

  return gyroSampleRate;
}

/***************************************************************************//**
 *    Sets the sample rate of the gyroscope
 ******************************************************************************/
float sl_icm20648_accel_set_sample_rate(float sampleRate)
{
  uint16_t accelDiv;
  float accelSampleRate;

  /* Calculate the sample rate divider */
  accelSampleRate = (1125.0 / sampleRate) - 1.0;

  /* Check if it fits in the divider registers */
  if ( accelSampleRate > 4095.0 ) {
    accelSampleRate = 4095.0;
  }

  if ( accelSampleRate < 0 ) {
    accelSampleRate = 0.0;
  }

  /* Write the value to the registers */
  accelDiv = (uint16_t) accelSampleRate;
  sl_icm20648_write_register(ICM20648_REG_ACCEL_SMPLRT_DIV_1, (uint8_t) (accelDiv >> 8) );
  sl_icm20648_write_register(ICM20648_REG_ACCEL_SMPLRT_DIV_2, (uint8_t) (accelDiv & 0xFF) );

  /* Calculate the actual sample rate from the divider value */
  accelSampleRate = 1125.0 / (accelDiv + 1);

  return accelSampleRate;
}

/***************************************************************************//**
 *    Sets the bandwidth of the gyroscope
 ******************************************************************************/
sl_status_t sl_icm20648_gyro_set_bandwidth(uint8_t gyroBw)
{
  uint8_t reg;

  /* Read the GYRO_CONFIG_1 register */
  sl_icm20648_read_register(ICM20648_REG_GYRO_CONFIG_1, 1, &reg);
  reg &= ~(ICM20648_MASK_GYRO_BW);

  /* Write the new bandwidth value to the gyro config register */
  reg |= (gyroBw & ICM20648_MASK_GYRO_BW);
  sl_icm20648_write_register(ICM20648_REG_GYRO_CONFIG_1, reg);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Sets the bandwidth of the accelerometer
 ******************************************************************************/
sl_status_t sl_icm20648_accel_set_bandwidth(uint8_t accelBw)
{
  uint8_t reg;

  /* Read the GYRO_CONFIG_1 register */
  sl_icm20648_read_register(ICM20648_REG_ACCEL_CONFIG, 1, &reg);
  reg &= ~(ICM20648_MASK_ACCEL_BW);

  /* Write the new bandwidth value to the gyro config register */
  reg |= (accelBw & ICM20648_MASK_ACCEL_BW);
  sl_icm20648_write_register(ICM20648_REG_ACCEL_CONFIG, reg);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Enables or disables the sleep mode of the device
 ******************************************************************************/
sl_status_t sl_icm20648_enable_sleep_mode(bool enable)
{
  uint8_t reg;

  sl_icm20648_read_register(ICM20648_REG_PWR_MGMT_1, 1, &reg);

  if ( enable ) {
    /* Sleep: set the SLEEP bit */
    reg |= ICM20648_BIT_SLEEP;
  } else {
    /* Wake up: clear the SLEEP bit */
    reg &= ~(ICM20648_BIT_SLEEP);
  }

  sl_icm20648_write_register(ICM20648_REG_PWR_MGMT_1, reg);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Enables or disables the cycle mode operation of the accel and gyro
 ******************************************************************************/
sl_status_t sl_icm20648_enable_cycle_mode(bool enable)
{
  uint8_t reg;

  reg = 0x00;

  if ( enable ) {
    reg = ICM20648_BIT_ACCEL_CYCLE | ICM20648_BIT_GYRO_CYCLE;
  }

  sl_icm20648_write_register(ICM20648_REG_LP_CONFIG, reg);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Enables or disables the sensors in the ICM20648 chip

 ******************************************************************************/
sl_status_t sl_icm20648_enable_sensor(bool accel, bool gyro, bool temp)
{
  uint8_t pwrManagement1;
  uint8_t pwrManagement2;

  sl_icm20648_read_register(ICM20648_REG_PWR_MGMT_1, 1, &pwrManagement1);
  pwrManagement2 = 0;

  /* To enable the accelerometer clear the DISABLE_ACCEL bits in PWR_MGMT_2 */
  if ( accel ) {
    pwrManagement2 &= ~(ICM20648_BIT_PWR_ACCEL_STBY);
  } else {
    pwrManagement2 |= ICM20648_BIT_PWR_ACCEL_STBY;
  }

  /* To enable gyro clear the DISABLE_GYRO bits in PWR_MGMT_2 */
  if ( gyro ) {
    pwrManagement2 &= ~(ICM20648_BIT_PWR_GYRO_STBY);
  } else {
    pwrManagement2 |= ICM20648_BIT_PWR_GYRO_STBY;
  }

  /* To enable the temperature sensor clear the TEMP_DIS bit in PWR_MGMT_1 */
  if ( temp ) {
    pwrManagement1 &= ~(ICM20648_BIT_TEMP_DIS);
  } else {
    pwrManagement1 |= ICM20648_BIT_TEMP_DIS;
  }

  /* Write back the modified values */
  sl_icm20648_write_register(ICM20648_REG_PWR_MGMT_1, pwrManagement1);
  sl_icm20648_write_register(ICM20648_REG_PWR_MGMT_2, pwrManagement2);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Enables or disables the sensors in low power mode in the ICM20648 chip
 ******************************************************************************/
sl_status_t sl_icm20648_enter_low_power_mode(bool enAccel, bool enGyro, bool enTemp)
{
  uint8_t data;

  sl_icm20648_read_register(ICM20648_REG_PWR_MGMT_1, 1, &data);

  if ( enAccel || enGyro || enTemp ) {
    /* Make sure that the chip is not in sleep */
    sl_icm20648_enable_sleep_mode(false);

    /* And in continuous mode */
    sl_icm20648_enable_cycle_mode(false);

    /* Enable the accelerometer and the gyroscope*/
    sl_icm20648_enable_sensor(enAccel, enGyro, enTemp);
    sl_sleeptimer_delay_millisecond(50);

    /* Enable cycle mode */
    sl_icm20648_enable_cycle_mode(true);

    /* Set the LP_EN bit to enable low power mode */
    data |= ICM20648_BIT_LP_EN;
  } else {
    /* Enable continuous mode */
    sl_icm20648_enable_cycle_mode(false);

    /* Clear the LP_EN bit to disable low power mode */
    data &= ~ICM20648_BIT_LP_EN;
  }

  /* Write the updated value to the PWR_MGNT_1 register */
  sl_icm20648_write_register(ICM20648_REG_PWR_MGMT_1, data);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Enables or disables the interrupts in the ICM20648 chip
 ******************************************************************************/
sl_status_t sl_icm20648_enable_interrupt(bool dataReadyEnable, bool womEnable)
{
  uint8_t intEnable;

  /* All interrupts disabled by default */
  intEnable = 0;

  /* Enable one or both of the interrupt sources if required */
  if ( womEnable ) {
    intEnable = ICM20648_BIT_WOM_INT_EN;
  }
  /* Write value to register */
  sl_icm20648_write_register(ICM20648_REG_INT_ENABLE, intEnable);

  /* All interrupts disabled by default */
  intEnable = 0;

  if ( dataReadyEnable ) {
    intEnable = ICM20648_BIT_RAW_DATA_0_RDY_EN;
  }

  /* Write value to register */
  sl_icm20648_write_register(ICM20648_REG_INT_ENABLE_1, intEnable);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads the interrupt status registers of the ICM20648 chip
 ******************************************************************************/
sl_status_t sl_icm20648_read_interrupt_status(uint32_t *intStatus)
{
  uint8_t reg[4];

  sl_icm20648_read_register(ICM20648_REG_INT_STATUS, 4, reg);
  *intStatus = (uint32_t) reg[0];
  *intStatus |= ( ( (uint32_t) reg[1]) << 8);
  *intStatus |= ( ( (uint32_t) reg[2]) << 16);
  *intStatus |= ( ( (uint32_t) reg[3]) << 24);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Checks if new data is available for read
 ******************************************************************************/
bool sl_icm20648_is_data_ready(void)
{
  uint8_t status;
  bool ret;

  ret = false;
  sl_icm20648_read_register(ICM20648_REG_INT_STATUS_1, 1, &status);

  if ( status & ICM20648_BIT_RAW_DATA_0_RDY_INT ) {
    ret = true;
  }

  return ret;
}

/***************************************************************************//**
 *    Sets up and enables the Wake-up On Motion feature
 ******************************************************************************/
sl_status_t sl_icm20648_enable_wake_on_motion_interrupt(bool enable, uint8_t womThreshold, float sampleRate)
{
  if ( enable ) {
    /* Make sure that the chip is not in sleep */
    sl_icm20648_enable_sleep_mode(false);

    /* And in continuous mode */
    sl_icm20648_enable_cycle_mode(false);

    /* Enable only the accelerometer */
    sl_icm20648_enable_sensor(true, false, false);

    /* Set sample rate */
    sl_icm20648_set_sample_rate(sampleRate);

    /* Set the bandwidth to 1210Hz */
    sl_icm20648_accel_set_bandwidth(ICM20648_ACCEL_BW_1210HZ);

    /* Accel: 2G full scale */
    sl_icm20648_accel_set_full_scale(ICM20648_ACCEL_FULLSCALE_2G);

    /* Enable the Wake On Motion interrupt */
    sl_icm20648_enable_interrupt(false, true);
    sl_sleeptimer_delay_millisecond(50);

    /* Enable Wake On Motion feature */
    sl_icm20648_write_register(ICM20648_REG_ACCEL_INTEL_CTRL, ICM20648_BIT_ACCEL_INTEL_EN | ICM20648_BIT_ACCEL_INTEL_MODE);

    /* Set the wake on motion threshold value */
    sl_icm20648_write_register(ICM20648_REG_ACCEL_WOM_THR, womThreshold);

    /* Enable low power mode */
    sl_icm20648_enter_low_power_mode(true, false, false);
  } else {
    /* Disable Wake On Motion feature */
    sl_icm20648_write_register(ICM20648_REG_ACCEL_INTEL_CTRL, 0x00);

    /* Disable the Wake On Motion interrupt */
    sl_icm20648_enable_interrupt(false, false);

    /* Disable cycle mode */
    sl_icm20648_enable_cycle_mode(false);
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Accelerometer and gyroscope calibration function. Reads the gyroscope
 *    and accelerometer values while the device is at rest and in level. The
 *    resulting values are loaded to the accel and gyro bias registers to cancel
 *    the static offset error.
 ******************************************************************************/
sl_status_t sl_icm20648_calibrate_accel_and_gyro(float *accelBiasScaled, float *gyroBiasScaled)
{
  uint8_t data[12];
  uint16_t i, packetCount, fifoCount;
  int32_t gyroBias[3] = { 0, 0, 0 };
  int32_t accelBias[3] = { 0, 0, 0 };
  int32_t accelTemp[3];
  int32_t gyroTemp[3];
  int32_t accelBiasFactory[3];
  int32_t gyroBiasStored[3];
  float gyroRes, accelRes;

  /* Enable the accelerometer and the gyro */
  sl_icm20648_enable_sensor(true, true, false);

  /* Set 1kHz sample rate */
  sl_icm20648_set_sample_rate(1100.0);

  /* 246Hz BW for the accelerometer and 200Hz for the gyroscope */
  sl_icm20648_accel_set_bandwidth(ICM20648_ACCEL_BW_246HZ);
  sl_icm20648_gyro_set_bandwidth(ICM20648_GYRO_BW_12HZ);

  /* Set the most sensitive range: 2G full scale and 250dps full scale */
  sl_icm20648_accel_set_full_scale(ICM20648_ACCEL_FULLSCALE_2G);
  sl_icm20648_gyro_set_full_scale(ICM20648_GYRO_FULLSCALE_250DPS);

  /* Retrieve the resolution per bit */
  sl_icm20648_accel_get_resolution(&accelRes);
  sl_icm20648_gyro_get_resolution(&gyroRes);

  /* The accel sensor needs max 30ms, the gyro max 35ms to fully start */
  /* Experiments show that the gyro needs more time to get reliable results */
  sl_sleeptimer_delay_millisecond(50);

  /* Disable the FIFO */
  sl_icm20648_write_register(ICM20648_REG_USER_CTRL, ICM20648_BIT_FIFO_EN);
  sl_icm20648_write_register(ICM20648_REG_FIFO_MODE, 0x0F);

  /* Enable accelerometer and gyro to store the data in FIFO */
  sl_icm20648_write_register(ICM20648_REG_FIFO_EN_2, ICM20648_BIT_ACCEL_FIFO_EN | ICM20648_BITS_GYRO_FIFO_EN);

  /* Reset the FIFO */
  sl_icm20648_write_register(ICM20648_REG_FIFO_RST, 0x0F);
  sl_icm20648_write_register(ICM20648_REG_FIFO_RST, 0x00);

  /* Enable the FIFO */
  sl_icm20648_write_register(ICM20648_REG_USER_CTRL, ICM20648_BIT_FIFO_EN);

  /* The max FIFO size is 4096 bytes, one set of measurements takes 12 bytes */
  /* (3 axes, 2 sensors, 2 bytes each value ) 340 samples use 4080 bytes of FIFO */
  /* Loop until at least 4080 samples gathered */
  fifoCount = 0;
  while ( fifoCount < 4080 ) {
    sl_sleeptimer_delay_millisecond(5);
    /* Read FIFO sample count */
    sl_icm20648_read_register(ICM20648_REG_FIFO_COUNT_H, 2, &data[0]);
    /* Convert to a 16 bit value */
    fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);
  }

  /* Disable accelerometer and gyro to store the data in FIFO */
  sl_icm20648_write_register(ICM20648_REG_FIFO_EN_2, 0x00);

  /* Read FIFO sample count */
  sl_icm20648_read_register(ICM20648_REG_FIFO_COUNT_H, 2, &data[0]);

  /* Convert to a 16 bit value */
  fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);

  /* Calculate the number of data sets (3 axis of accel an gyro, two bytes each = 12 bytes) */
  packetCount = fifoCount / 12;

  /* Retrieve the data from the FIFO */
  for ( i = 0; i < packetCount; i++ ) {
    sl_icm20648_read_register(ICM20648_REG_FIFO_R_W, 12, &data[0]);
    /* Convert to 16 bit signed accel and gyro x,y and z values */
    accelTemp[0] = ( (int16_t) (data[0] << 8) | data[1]);
    accelTemp[1] = ( (int16_t) (data[2] << 8) | data[3]);
    accelTemp[2] = ( (int16_t) (data[4] << 8) | data[5]);
    gyroTemp[0] = ( (int16_t) (data[6] << 8) | data[7]);
    gyroTemp[1] = ( (int16_t) (data[8] << 8) | data[9]);
    gyroTemp[2] = ( (int16_t) (data[10] << 8) | data[11]);

    /* Sum the values */
    accelBias[0] += accelTemp[0];
    accelBias[1] += accelTemp[1];
    accelBias[2] += accelTemp[2];
    gyroBias[0] += gyroTemp[0];
    gyroBias[1] += gyroTemp[1];
    gyroBias[2] += gyroTemp[2];
  }

  /* Divide by packet count to get the average */
  accelBias[0] /= packetCount;
  accelBias[1] /= packetCount;
  accelBias[2] /= packetCount;
  gyroBias[0] /= packetCount;
  gyroBias[1] /= packetCount;
  gyroBias[2] /= packetCount;

  /* Acceleormeter: add or remove (depending on the orientation of the chip) 1G (gravity) from the Z axis value */
  if ( accelBias[2] > 0L ) {
    accelBias[2] -= (int32_t) (1.0 / accelRes);
  } else {
    accelBias[2] += (int32_t) (1.0 / accelRes);
  }

  /* Convert the values to degrees per sec for displaying */
  gyroBiasScaled[0] = (float) gyroBias[0] * gyroRes;
  gyroBiasScaled[1] = (float) gyroBias[1] * gyroRes;
  gyroBiasScaled[2] = (float) gyroBias[2] * gyroRes;

  /* Read stored gyro trim values. After reset these values are all 0 */
  sl_icm20648_read_register(ICM20648_REG_XG_OFFS_USRH, 2, &data[0]);
  gyroBiasStored[0] = ( (int16_t) (data[0] << 8) | data[1]);
  sl_icm20648_read_register(ICM20648_REG_YG_OFFS_USRH, 2, &data[0]);
  gyroBiasStored[1] = ( (int16_t) (data[0] << 8) | data[1]);
  sl_icm20648_read_register(ICM20648_REG_ZG_OFFS_USRH, 2, &data[0]);
  gyroBiasStored[2] = ( (int16_t) (data[0] << 8) | data[1]);

  /* The gyro bias should be stored in 1000dps full scaled format. We measured in 250dps to get */
  /* the best sensitivity, so need to divide by 4 */
  /* Substract from the stored calibration value */
  gyroBiasStored[0] -= gyroBias[0] / 4;
  gyroBiasStored[1] -= gyroBias[1] / 4;
  gyroBiasStored[2] -= gyroBias[2] / 4;

  /* Split the values into two bytes */
  data[0] = (gyroBiasStored[0] >> 8) & 0xFF;
  data[1] = (gyroBiasStored[0]) & 0xFF;
  data[2] = (gyroBiasStored[1] >> 8) & 0xFF;
  data[3] = (gyroBiasStored[1]) & 0xFF;
  data[4] = (gyroBiasStored[2] >> 8) & 0xFF;
  data[5] = (gyroBiasStored[2]) & 0xFF;

  /* Write the  gyro bias values to the chip */
  sl_icm20648_write_register(ICM20648_REG_XG_OFFS_USRH, data[0]);
  sl_icm20648_write_register(ICM20648_REG_XG_OFFS_USRL, data[1]);
  sl_icm20648_write_register(ICM20648_REG_YG_OFFS_USRH, data[2]);
  sl_icm20648_write_register(ICM20648_REG_YG_OFFS_USRL, data[3]);
  sl_icm20648_write_register(ICM20648_REG_ZG_OFFS_USRH, data[4]);
  sl_icm20648_write_register(ICM20648_REG_ZG_OFFS_USRL, data[5]);

  /* Calculate the accelerometer bias values to store in the hardware accelerometer bias registers. These registers contain */
  /* factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold */
  /* non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature */
  /* compensation calculations(? the datasheet is not clear). Accelerometer bias registers expect bias input */
  /* as 2048 LSB per g, so that the accelerometer biases calculated above must be divided by 8. */

  /* Read factory accelerometer trim values */
  sl_icm20648_read_register(ICM20648_REG_XA_OFFSET_H, 2, &data[0]);
  accelBiasFactory[0] = ( (int16_t) (data[0] << 8) | data[1]);
  sl_icm20648_read_register(ICM20648_REG_YA_OFFSET_H, 2, &data[0]);
  accelBiasFactory[1] = ( (int16_t) (data[0] << 8) | data[1]);
  sl_icm20648_read_register(ICM20648_REG_ZA_OFFSET_H, 2, &data[0]);
  accelBiasFactory[2] = ( (int16_t) (data[0] << 8) | data[1]);

  /* Construct total accelerometer bias, including calculated average accelerometer bias from above */
  /* Scale the 2g full scale (most sensitive range) results to 16g full scale - divide by 8 */
  /* Clear the last bit (temperature compensation? - the datasheet is not clear) */
  /* Substract from the factory calibration value */

  accelBiasFactory[0] -= ( (accelBias[0] / 8) & ~1);
  accelBiasFactory[1] -= ( (accelBias[1] / 8) & ~1);
  accelBiasFactory[2] -= ( (accelBias[2] / 8) & ~1);

  /* Split the values into two bytes */
  data[0] = (accelBiasFactory[0] >> 8) & 0xFF;
  data[1] = (accelBiasFactory[0]) & 0xFF;
  data[2] = (accelBiasFactory[1] >> 8) & 0xFF;
  data[3] = (accelBiasFactory[1]) & 0xFF;
  data[4] = (accelBiasFactory[2] >> 8) & 0xFF;
  data[5] = (accelBiasFactory[2]) & 0xFF;

  /* Store them in the accelerometer offset registers */
  sl_icm20648_write_register(ICM20648_REG_XA_OFFSET_H, data[0]);
  sl_icm20648_write_register(ICM20648_REG_XA_OFFSET_L, data[1]);
  sl_icm20648_write_register(ICM20648_REG_YA_OFFSET_H, data[2]);
  sl_icm20648_write_register(ICM20648_REG_YA_OFFSET_L, data[3]);
  sl_icm20648_write_register(ICM20648_REG_ZA_OFFSET_H, data[4]);
  sl_icm20648_write_register(ICM20648_REG_ZA_OFFSET_L, data[5]);

  /* Convert the values to G for displaying */
  accelBiasScaled[0] = (float) accelBias[0] * accelRes;
  accelBiasScaled[1] = (float) accelBias[1] * accelRes;
  accelBiasScaled[2] = (float) accelBias[2] * accelRes;

  /* Turn off FIFO */
  sl_icm20648_write_register(ICM20648_REG_USER_CTRL, 0x00);

  /* Disable all sensors */
  sl_icm20648_enable_sensor(false, false, false);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Gyroscope calibration function. Reads the gyroscope
 *    values while the device is at rest and in level. The
 *    resulting values are loaded to the gyro bias registers to cancel
 *    the static offset error.
 ******************************************************************************/
sl_status_t sl_icm20648_calibrate_gyro(float *gyroBiasScaled)
{
  uint8_t data[12];
  uint16_t i, packetCount, fifoCount;
  int32_t gyroBias[3] = { 0, 0, 0 };
  int32_t gyroTemp[3];
  int32_t gyroBiasStored[3];
  float gyroRes;

  /* Enable the accelerometer and the gyro */
  sl_icm20648_enable_sensor(true, true, false);

  /* Set 1kHz sample rate */
  sl_icm20648_set_sample_rate(1100.0);

  /* Configure bandwidth for gyroscope to 12Hz */
  sl_icm20648_gyro_set_bandwidth(ICM20648_GYRO_BW_12HZ);

  /* Configure sensitivity to 250dps full scale */
  sl_icm20648_gyro_set_full_scale(ICM20648_GYRO_FULLSCALE_250DPS);

  /* Retrieve the resolution per bit */
  sl_icm20648_gyro_get_resolution(&gyroRes);

  /* The accel sensor needs max 30ms, the gyro max 35ms to fully start */
  /* Experiments show that the gyro needs more time to get reliable results */
  sl_sleeptimer_delay_millisecond(50);

  /* Disable the FIFO */
  sl_icm20648_write_register(ICM20648_REG_USER_CTRL, ICM20648_BIT_FIFO_EN);
  sl_icm20648_write_register(ICM20648_REG_FIFO_MODE, 0x0F);

  /* Enable accelerometer and gyro to store the data in FIFO */
  sl_icm20648_write_register(ICM20648_REG_FIFO_EN_2, ICM20648_BITS_GYRO_FIFO_EN);

  /* Reset the FIFO */
  sl_icm20648_write_register(ICM20648_REG_FIFO_RST, 0x0F);
  sl_icm20648_write_register(ICM20648_REG_FIFO_RST, 0x00);

  /* Enable the FIFO */
  sl_icm20648_write_register(ICM20648_REG_USER_CTRL, ICM20648_BIT_FIFO_EN);

  /* The max FIFO size is 4096 bytes, one set of measurements takes 12 bytes */
  /* (3 axes, 2 sensors, 2 bytes each value ) 340 samples use 4080 bytes of FIFO */
  /* Loop until at least 4080 samples gathered */
  fifoCount = 0;
  while ( fifoCount < 4080 ) {
    sl_sleeptimer_delay_millisecond(5);

    /* Read FIFO sample count */
    sl_icm20648_read_register(ICM20648_REG_FIFO_COUNT_H, 2, &data[0]);

    /* Convert to a 16 bit value */
    fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);
  }

  /* Disable accelerometer and gyro to store the data in FIFO */
  sl_icm20648_write_register(ICM20648_REG_FIFO_EN_2, 0x00);

  /* Read FIFO sample count */
  sl_icm20648_read_register(ICM20648_REG_FIFO_COUNT_H, 2, &data[0]);

  /* Convert to a 16 bit value */
  fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);

  /* Calculate the number of data sets (3 axis of accel an gyro, two bytes each = 12 bytes) */
  packetCount = fifoCount / 12;

  /* Retrieve the data from the FIFO */
  for ( i = 0; i < packetCount; i++ ) {
    sl_icm20648_read_register(ICM20648_REG_FIFO_R_W, 12, &data[0]);
    /* Convert to 16 bit signed accel and gyro x,y and z values */
    gyroTemp[0] = ( (int16_t) (data[6] << 8) | data[7]);
    gyroTemp[1] = ( (int16_t) (data[8] << 8) | data[9]);
    gyroTemp[2] = ( (int16_t) (data[10] << 8) | data[11]);

    /* Sum the values */
    gyroBias[0] += gyroTemp[0];
    gyroBias[1] += gyroTemp[1];
    gyroBias[2] += gyroTemp[2];
  }

  /* Divide by packet count to get the average */
  gyroBias[0] /= packetCount;
  gyroBias[1] /= packetCount;
  gyroBias[2] /= packetCount;

  /* Convert the values to degrees per sec for displaying */
  gyroBiasScaled[0] = (float) gyroBias[0] * gyroRes;
  gyroBiasScaled[1] = (float) gyroBias[1] * gyroRes;
  gyroBiasScaled[2] = (float) gyroBias[2] * gyroRes;

  /* Read stored gyro trim values. After reset these values are all 0 */
  sl_icm20648_read_register(ICM20648_REG_XG_OFFS_USRH, 2, &data[0]);
  gyroBiasStored[0] = ( (int16_t) (data[0] << 8) | data[1]);

  sl_icm20648_read_register(ICM20648_REG_YG_OFFS_USRH, 2, &data[0]);
  gyroBiasStored[1] = ( (int16_t) (data[0] << 8) | data[1]);

  sl_icm20648_read_register(ICM20648_REG_ZG_OFFS_USRH, 2, &data[0]);
  gyroBiasStored[2] = ( (int16_t) (data[0] << 8) | data[1]);

  /* The gyro bias should be stored in 1000dps full scaled format. We measured in 250dps to get */
  /* the best sensitivity, so need to divide by 4 */
  /* Substract from the stored calibration value */
  gyroBiasStored[0] -= gyroBias[0] / 4;
  gyroBiasStored[1] -= gyroBias[1] / 4;
  gyroBiasStored[2] -= gyroBias[2] / 4;

  /* Split the values into two bytes */
  data[0] = (gyroBiasStored[0] >> 8) & 0xFF;
  data[1] = (gyroBiasStored[0]) & 0xFF;
  data[2] = (gyroBiasStored[1] >> 8) & 0xFF;
  data[3] = (gyroBiasStored[1]) & 0xFF;
  data[4] = (gyroBiasStored[2] >> 8) & 0xFF;
  data[5] = (gyroBiasStored[2]) & 0xFF;

  /* Write the  gyro bias values to the chip */
  sl_icm20648_write_register(ICM20648_REG_XG_OFFS_USRH, data[0]);
  sl_icm20648_write_register(ICM20648_REG_XG_OFFS_USRL, data[1]);
  sl_icm20648_write_register(ICM20648_REG_YG_OFFS_USRH, data[2]);
  sl_icm20648_write_register(ICM20648_REG_YG_OFFS_USRL, data[3]);
  sl_icm20648_write_register(ICM20648_REG_ZG_OFFS_USRH, data[4]);
  sl_icm20648_write_register(ICM20648_REG_ZG_OFFS_USRL, data[5]);

  /* Turn off FIFO */
  sl_icm20648_write_register(ICM20648_REG_USER_CTRL, 0x00);

  /* Disable all sensors */
  sl_icm20648_enable_sensor(false, false, false);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads the temperature sensor raw value and converts to Celsius.
 ******************************************************************************/
sl_status_t sl_icm20648_read_temperature_data(float *temperature)
{
  uint8_t data[2];
  int16_t raw_temp;

  /* Read temperature registers */
  sl_icm20648_read_register(ICM20648_REG_TEMPERATURE_H, 2, data);

  /* Convert to int16 */
  raw_temp = (int16_t) ( (data[0] << 8) + data[1]);

  /* Calculate the Celsius value from the raw reading */
  *temperature = ( (float) raw_temp / 333.87) + 21.0;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads the device ID of the ICM20648
 ******************************************************************************/
sl_status_t sl_icm20648_get_device_id(uint8_t *devID)
{
  sl_icm20648_read_register(ICM20648_REG_WHO_AM_I, 1, devID);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Initializes the SPI bus in order to communicate with the ICM20648
 ******************************************************************************/
sl_status_t sl_icm20648_spi_init(void)
{
  USART_TypeDef *usart = SL_ICM20648_SPI_PERIPHERAL;

  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;
  init.msbf = true;           // Send most significant byte first
  init.baudrate = 3300000;    // SPI-frequency at 3.3 MHz

  /* Enabling clock to USART */
#if defined(_SILICON_LABS_32B_SERIES_2)
  CMU_ClockEnable(cmuClock_PCLK, true);
#else
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif
  CMU_ClockEnable(ICM20648_SPI_CLK(SL_ICM20648_SPI_PERIPHERAL_NO), true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* IO configuration */
  GPIO_PinModeSet(SL_ICM20648_SPI_TX_PORT, SL_ICM20648_SPI_TX_PIN, gpioModePushPull, 0);    /* TX - MOSI */
  GPIO_PinModeSet(SL_ICM20648_SPI_RX_PORT, SL_ICM20648_SPI_RX_PIN, gpioModeInput, 0);    /* RX - MISO */
  GPIO_PinModeSet(SL_ICM20648_SPI_CLK_PORT, SL_ICM20648_SPI_CLK_PIN, gpioModePushPull, 0);    /* Clock */
  GPIO_PinModeSet(SL_ICM20648_SPI_CS_PORT, SL_ICM20648_SPI_CS_PIN, gpioModePushPull, 1);     /* CS */

  USART_Reset(SL_ICM20648_SPI_PERIPHERAL);

  /* Initialize USART, in SPI master mode. */
  USART_InitSync(usart, &init);

  /* Enable pins at correct UART/USART location. */
#if defined(_SILICON_LABS_32B_SERIES_2)
  GPIO->USARTROUTE[SL_ICM20648_SPI_PERIPHERAL_NO].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_CLKPEN;
  GPIO->USARTROUTE[SL_ICM20648_SPI_PERIPHERAL_NO].TXROUTE |= ((SL_ICM20648_SPI_TX_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT) | (SL_ICM20648_SPI_TX_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT));
  GPIO->USARTROUTE[SL_ICM20648_SPI_PERIPHERAL_NO].RXROUTE |= ((SL_ICM20648_SPI_RX_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT) | (SL_ICM20648_SPI_RX_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT));
  GPIO->USARTROUTE[SL_ICM20648_SPI_PERIPHERAL_NO].CLKROUTE |= ((SL_ICM20648_SPI_CLK_PORT << _GPIO_USART_CLKROUTE_PORT_SHIFT) | (SL_ICM20648_SPI_CLK_PIN << _GPIO_USART_CLKROUTE_PIN_SHIFT));
#else
  usart->ROUTEPEN   = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_CLKPEN;
  usart->ROUTELOC0  = (usart->ROUTELOC0 & ~(_USART_ROUTELOC0_TXLOC_MASK | _USART_ROUTELOC0_RXLOC_MASK | _USART_ROUTELOC0_CLKLOC_MASK) );
  usart->ROUTELOC0 |= (SL_ICM20648_SPI_TX_LOC << _USART_ROUTELOC0_TXLOC_SHIFT);
  usart->ROUTELOC0 |= (SL_ICM20648_SPI_RX_LOC << _USART_ROUTELOC0_RXLOC_SHIFT);
  usart->ROUTELOC0 |= (SL_ICM20648_SPI_CLK_LOC << _USART_ROUTELOC0_CLKLOC_SHIFT);
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads register from the ICM20648 device
 ******************************************************************************/
void sl_icm20648_read_register(uint16_t addr, int numBytes, uint8_t *data)
{
  uint8_t regAddr;
  uint8_t bank;

  regAddr = (uint8_t) (addr & 0x7F);
  bank = (uint8_t) (addr >> 7);

  sl_icm20648_select_register_bank(bank);

  /* Enable chip select */
  sl_icm20648_chip_select_set(true);

  /* Set R/W bit to 1 - read */
  USART_Tx(SL_ICM20648_SPI_PERIPHERAL, (regAddr | 0x80) );
  USART_Rx(SL_ICM20648_SPI_PERIPHERAL);
  /* Transmit 0's to provide clock and read the data */
  while ( numBytes-- ) {
    USART_Tx(SL_ICM20648_SPI_PERIPHERAL, 0x00);
    *data++ = USART_Rx(SL_ICM20648_SPI_PERIPHERAL);
  }

  /* Disable chip select */
  sl_icm20648_chip_select_set(false);
}

/***************************************************************************//**
 *    Writes a register in the ICM20648 device
 ******************************************************************************/
void sl_icm20648_write_register(uint16_t addr, uint8_t data)
{
  uint8_t regAddr;
  uint8_t bank;

  regAddr = (uint8_t) (addr & 0x7F);
  bank = (uint8_t) (addr >> 7);

  sl_icm20648_select_register_bank(bank);

  /* Enable chip select */
  sl_icm20648_chip_select_set(true);

  /* clear R/W bit - write, send the address */
  USART_Tx(SL_ICM20648_SPI_PERIPHERAL, (regAddr & 0x7F) );
  USART_Rx(SL_ICM20648_SPI_PERIPHERAL);

  /* Send the data */
  USART_Tx(SL_ICM20648_SPI_PERIPHERAL, data);
  USART_Rx(SL_ICM20648_SPI_PERIPHERAL);

  /* Disable chip select */
  sl_icm20648_chip_select_set(false);
}

/***************************************************************************//**
 *    Select the desired register bank
 ******************************************************************************/
void sl_icm20648_select_register_bank(uint8_t bank)
{
  /* Enable chip select */
  sl_icm20648_chip_select_set(true);

  /* Select the Bank Select register */
  USART_Tx(SL_ICM20648_SPI_PERIPHERAL, ICM20648_REG_BANK_SEL);
  USART_Rx(SL_ICM20648_SPI_PERIPHERAL);

  /* Write the desired bank address 0..3 */
  USART_Tx(SL_ICM20648_SPI_PERIPHERAL, (bank << 4) );
  USART_Rx(SL_ICM20648_SPI_PERIPHERAL);

  /* Disable chip select */
  sl_icm20648_chip_select_set(false);
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Controls the state of the ICM20648 SPI chip select pin
 *
 * @param[in] select
 *    The desired state of the select pin. True: asserted (logic L)
 *
 * @return
 *    None
 ******************************************************************************/
static void sl_icm20648_chip_select_set(bool select)
{
  if ( select ) {
    GPIO_PinOutClear(SL_ICM20648_SPI_CS_PORT, SL_ICM20648_SPI_CS_PIN);
  } else {
    GPIO_PinOutSet(SL_ICM20648_SPI_CS_PORT, SL_ICM20648_SPI_CS_PIN);
  }
}

/** @endcond */
