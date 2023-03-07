/***************************************************************************//**
 * @file
 * @brief Driver for the Invensense ICM20648 6-axis motion sensor
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

#include <stdint.h>
#include <stdio.h>
#include "em_usart.h"
#include "em_gpio.h"
#include "em_cmu.h"

#include "thunderboard/util.h"
#include "thunderboard/board.h"
#include "thunderboard/icm20648.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/**************************************************************************//**
* @defgroup ICM20648 ICM20648 - 6-axis Motion Sensor Driver
* @{
* @brief Driver for the Invensense ICM20648 6-axis motion sensor
******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static void        ICM20648_chipSelectSet(bool select);

/** @endcond */

/***************************************************************************//**
 * @brief
 *    Initializes the ICM20648 sensor. Enables the power supply and SPI lines,
 *    sets up the host SPI controller, configures the chip control interface,
 *    clock generator and interrupt line.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_init(void)
{
  uint8_t data;

  /* Disable the chip to avoid misconfiguration due transients during the SPI configuration */
  BOARD_imuEnable(false);

  /* Enable and setup the SPI bus */
  ICM20648_spiInit();

  /* Enable the chip */
  BOARD_imuEnable(true);

  /* Issue reset */
  ICM20648_reset();

  /* Disable I2C interface, use SPI */
  ICM20648_registerWrite(ICM20648_REG_USER_CTRL, ICM20648_BIT_I2C_IF_DIS);

  /* Read Who am I register, should get 0x71 */
  ICM20648_registerRead(ICM20648_REG_WHO_AM_I, 1, &data);

  /* If not - return */
  if ( (data != ICM20648_DEVICE_ID) && (data != ICM20948_DEVICE_ID) ) {
    return ICM20648_ERROR_INVALID_DEVICE_ID;
  }

  /* Auto selects the best available clock source  PLL if ready, else use the Internal oscillator */
  ICM20648_registerWrite(ICM20648_REG_PWR_MGMT_1, ICM20648_BIT_CLK_PLL);

  /* PLL startup time - maybe it is too long but better be on the safe side, no spec in the datasheet */
  UTIL_delay(30);

  /* INT pin: active low, open drain, IT status read clears. It seems that latched mode does not work, the INT pin cannot be cleared if set */
  ICM20648_registerWrite(ICM20648_REG_INT_PIN_CFG, ICM20648_BIT_INT_ACTL | ICM20648_BIT_INT_OPEN);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    De-initializes the ICM20648 sensor by disconnecting the supply and SPI
 *    lines.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_deInit(void)
{
  /* Disable the chip */
  BOARD_imuEnable(false);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Performs soft reset on the ICM20648 chip
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_reset(void)
{
  /* Set H_RESET bit to initiate soft reset */
  ICM20648_registerWrite(ICM20648_REG_PWR_MGMT_1, ICM20648_BIT_H_RESET);

  /* Wait 100ms to complete the reset sequence */
  UTIL_delay(100);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Reads the raw acceleration value and converts to g value based on
 *    the actual resolution
 *
 * @param[out] accel
 *    A 3-element array of float numbers containing the acceleration values
 *    for the x, y and z axes in g units.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_accelDataRead(float *accel)
{
  uint8_t rawData[6];
  float accelRes;
  int16_t temp;

  /* Retrieve the current resolution */
  ICM20648_accelResolutionGet(&accelRes);

  /* Read the six raw data registers into data array */
  ICM20648_registerRead(ICM20648_REG_ACCEL_XOUT_H_SH, 6, &rawData[0]);

  /* Convert the MSB and LSB into a signed 16-bit value and multiply by the resolution to get the G value */
  temp = ( (int16_t) rawData[0] << 8) | rawData[1];
  accel[0] = (float) temp * accelRes;
  temp = ( (int16_t) rawData[2] << 8) | rawData[3];
  accel[1] = (float) temp * accelRes;
  temp = ( (int16_t) rawData[4] << 8) | rawData[5];
  accel[2] = (float) temp * accelRes;

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Reads the raw gyroscope value and converts to deg/sec value based on
 *    the actual resolution
 *
 * @param[out] gyro
 *    A 3-element array of float numbers containing the gyroscope values
 *    for the x, y and z axes in deg/sec units.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_gyroDataRead(float *gyro)
{
  uint8_t rawData[6];
  float gyroRes;
  int16_t temp;

  /* Retrieve the current resolution */
  ICM20648_gyroResolutionGet(&gyroRes);

  /* Read the six raw data registers into data array */
  ICM20648_registerRead(ICM20648_REG_GYRO_XOUT_H_SH, 6, &rawData[0]);

  /* Convert the MSB and LSB into a signed 16-bit value and multiply by the resolution to get the dps value */
  temp = ( (int16_t) rawData[0] << 8) | rawData[1];
  gyro[0] = (float) temp * gyroRes;
  temp = ( (int16_t) rawData[2] << 8) | rawData[3];
  gyro[1] = (float) temp * gyroRes;
  temp = ( (int16_t) rawData[4] << 8) | rawData[5];
  gyro[2] = (float) temp * gyroRes;

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Gets the actual resolution of the accelerometer
 *
 * @param[out] accelRes
 *    The resolution in g/bit units
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_accelResolutionGet(float *accelRes)
{
  uint8_t reg;

  /* Read the actual acceleration full scale setting */
  ICM20648_registerRead(ICM20648_REG_ACCEL_CONFIG, 1, &reg);
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

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Gets the actual resolution of the gyroscope
 *
 * @param[out] gyroRes
 *    The actual resolution in (deg/sec)/bit units
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_gyroResolutionGet(float *gyroRes)
{
  uint8_t reg;

  /* Read the actual gyroscope full scale setting */
  ICM20648_registerRead(ICM20648_REG_GYRO_CONFIG_1, 1, &reg);
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

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Sets the full scale value of the accelerometer
 *
 * @param[in] accelFs
 *    The desired full scale value. Use the ICM20648_ACCEL_FULLSCALE_xG
 *    macros, which are defined in the icm20648.h file. The value of x can be
 *    2, 4, 8 or 16.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_accelFullscaleSet(uint8_t accelFs)
{
  uint8_t reg;

  accelFs &= ICM20648_MASK_ACCEL_FULLSCALE;
  ICM20648_registerRead(ICM20648_REG_ACCEL_CONFIG, 1, &reg);
  reg &= ~(ICM20648_MASK_ACCEL_FULLSCALE);
  reg |= accelFs;
  ICM20648_registerWrite(ICM20648_REG_ACCEL_CONFIG, reg);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Sets the full scale value of the gyroscope
 *
 * @param[in] gyroFs
 *    The desired full scale value. Use the ICM20648_GYRO_FULLSCALE_yDPS
 *    macros, which are defined in the icm20648.h file. The value of y can be
 *    250, 500, 1000 or 2000.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_gyroFullscaleSet(uint8_t gyroFs)
{
  uint8_t reg;

  gyroFs &= ICM20648_MASK_GYRO_FULLSCALE;
  ICM20648_registerRead(ICM20648_REG_GYRO_CONFIG_1, 1, &reg);
  reg &= ~(ICM20648_MASK_GYRO_FULLSCALE);
  reg |= gyroFs;
  ICM20648_registerWrite(ICM20648_REG_GYRO_CONFIG_1, reg);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Sets the sample rate both of the accelerometer and the gyroscope.
 *
 * @param[in] sampleRate
 *    The desired sample rate in Hz. Since the resolution of the sample rate
 *    divider is different in the accel and gyro stages it is possible that
 *    the two sensor will have different sample rate set.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_sampleRateSet(float sampleRate)
{
  ICM20648_gyroSampleRateSet(sampleRate);
  ICM20648_accelSampleRateSet(sampleRate);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Sets the sample rate of the accelerometer
 *
 * @param[in] sampleRate
 *    The desired sample rate in Hz
 *
 * @return
 *    The actual sample rate. May be different from the desired value because
 *    of the finite and discrete number of divider settings
 ******************************************************************************/
float ICM20648_gyroSampleRateSet(float sampleRate)
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
  ICM20648_registerWrite(ICM20648_REG_GYRO_SMPLRT_DIV, gyroDiv);

  /* Calculate the actual sample rate from the divider value */
  gyroSampleRate = 1125.0 / (gyroDiv + 1);

  return gyroSampleRate;
}

/***************************************************************************//**
 * @brief
 *    Sets the sample rate of the gyroscope
 *
 * @param[in] sampleRate
 *    The desired sample rate in Hz
 *
 * @return
 *    The actual sample rate. May be different from the desired value because
 *    of the finite and discrete number of divider settings
 ******************************************************************************/
float ICM20648_accelSampleRateSet(float sampleRate)
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
  ICM20648_registerWrite(ICM20648_REG_ACCEL_SMPLRT_DIV_1, (uint8_t) (accelDiv >> 8) );
  ICM20648_registerWrite(ICM20648_REG_ACCEL_SMPLRT_DIV_2, (uint8_t) (accelDiv & 0xFF) );

  /* Calculate the actual sample rate from the divider value */
  accelSampleRate = 1125.0 / (accelDiv + 1);

  return accelSampleRate;
}

/***************************************************************************//**
 * @brief
 *    Sets the bandwidth of the gyroscope
 *
 * @param[in] gyroBw
 *    The desired bandwidth value. Use the ICM20648_GYRO_BW_xHZ macros, which
 *    are defined in the icm20648.h file. The value of x can be
 *    6, 12, 24, 51, 120, 150, 200, 360 or 12100.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_gyroBandwidthSet(uint8_t gyroBw)
{
  uint8_t reg;

  /* Read the GYRO_CONFIG_1 register */
  ICM20648_registerRead(ICM20648_REG_GYRO_CONFIG_1, 1, &reg);
  reg &= ~(ICM20648_MASK_GYRO_BW);

  /* Write the new bandwidth value to the gyro config register */
  reg |= (gyroBw & ICM20648_MASK_GYRO_BW);
  ICM20648_registerWrite(ICM20648_REG_GYRO_CONFIG_1, reg);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Sets the bandwidth of the accelerometer
 *
 * @param[in] accelBw
 *    The desired bandwidth value. Use the ICM20648_ACCEL_BW_yHZ macros, which
 *    are defined in the icm20648.h file. The value of y can be
 *    6, 12, 24, 50, 111, 246, 470 or 1210.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_accelBandwidthSet(uint8_t accelBw)
{
  uint8_t reg;

  /* Read the GYRO_CONFIG_1 register */
  ICM20648_registerRead(ICM20648_REG_ACCEL_CONFIG, 1, &reg);
  reg &= ~(ICM20648_MASK_ACCEL_BW);

  /* Write the new bandwidth value to the gyro config register */
  reg |= (accelBw & ICM20648_MASK_ACCEL_BW);
  ICM20648_registerWrite(ICM20648_REG_ACCEL_CONFIG, reg);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the sleep mode of the device
 *
 * @param[in] enable
 *    If true, sleep mode is enabled. Set to false to disable sleep mode.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_sleepModeEnable(bool enable)
{
  uint8_t reg;

  ICM20648_registerRead(ICM20648_REG_PWR_MGMT_1, 1, &reg);

  if ( enable ) {
    /* Sleep: set the SLEEP bit */
    reg |= ICM20648_BIT_SLEEP;
  } else {
    /* Wake up: clear the SLEEP bit */
    reg &= ~(ICM20648_BIT_SLEEP);
  }

  ICM20648_registerWrite(ICM20648_REG_PWR_MGMT_1, reg);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the cycle mode operation of the accel and gyro
 *
 * @param[in] enable
 *    If true both the accel and gyro sensors will operate in cycle mode. If
 *    false the senors working in continuous mode.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_cycleModeEnable(bool enable)
{
  uint8_t reg;

  reg = 0x00;

  if ( enable ) {
    reg = ICM20648_BIT_ACCEL_CYCLE | ICM20648_BIT_GYRO_CYCLE;
  }

  ICM20648_registerWrite(ICM20648_REG_LP_CONFIG, reg);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the sensors in the ICM20648 chip
 *
 * @param[in] accel
 *    If true enables the acceleration sensor
 *
 * @param[in] gyro
 *    If true enables the gyroscope sensor
 *
 * @param[in] temp
 *    If true enables the temperature sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_sensorEnable(bool accel, bool gyro, bool temp)
{
  uint8_t pwrManagement1;
  uint8_t pwrManagement2;

  ICM20648_registerRead(ICM20648_REG_PWR_MGMT_1, 1, &pwrManagement1);
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
  ICM20648_registerWrite(ICM20648_REG_PWR_MGMT_1, pwrManagement1);
  ICM20648_registerWrite(ICM20648_REG_PWR_MGMT_2, pwrManagement2);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the sensors in low power mode in the ICM20648 chip
 *
 * @param[in] enAccel
 *    If true enables the acceleration sensor in low power mode
 *
 * @param[in] enGyro
 *    If true enables the gyroscope sensor in low power mode
 *
 * @param[in] enTemp
 *    If true enables the temperature sensor in low power mode
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_lowPowerModeEnter(bool enAccel, bool enGyro, bool enTemp)
{
  uint8_t data;

  ICM20648_registerRead(ICM20648_REG_PWR_MGMT_1, 1, &data);

  if ( enAccel || enGyro || enTemp ) {
    /* Make sure that the chip is not in sleep */
    ICM20648_sleepModeEnable(false);

    /* And in continuous mode */
    ICM20648_cycleModeEnable(false);

    /* Enable the accelerometer and the gyroscope*/
    ICM20648_sensorEnable(enAccel, enGyro, enTemp);
    UTIL_delay(50);

    /* Enable cycle mode */
    ICM20648_cycleModeEnable(true);

    /* Set the LP_EN bit to enable low power mode */
    data |= ICM20648_BIT_LP_EN;
  } else {
    /* Enable continuous mode */
    ICM20648_cycleModeEnable(false);

    /* Clear the LP_EN bit to disable low power mode */
    data &= ~ICM20648_BIT_LP_EN;
  }

  /* Write the updated value to the PWR_MGNT_1 register */
  ICM20648_registerWrite(ICM20648_REG_PWR_MGMT_1, data);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the interrupts in the ICM20648 chip
 *
 * @param[in] dataReadyEnable
 *    If true enables the Raw Data Ready interrupt, otherwise disables.
 *
 * @param[in] womEnable
 *    If true enables the Wake-up On Motion interrupt, otherwise disables.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_interruptEnable(bool dataReadyEnable, bool womEnable)
{
  uint8_t intEnable;

  /* All interrupts disabled by default */
  intEnable = 0;

  /* Enable one or both of the interrupt sources if required */
  if ( womEnable ) {
    intEnable = ICM20648_BIT_WOM_INT_EN;
  }
  /* Write value to register */
  ICM20648_registerWrite(ICM20648_REG_INT_ENABLE, intEnable);

  /* All interrupts disabled by default */
  intEnable = 0;

  if ( dataReadyEnable ) {
    intEnable = ICM20648_BIT_RAW_DATA_0_RDY_EN;
  }

  /* Write value to register */
  ICM20648_registerWrite(ICM20648_REG_INT_ENABLE_1, intEnable);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Reads the interrupt status registers of the ICM20648 chip
 *
 * @param[out] intStatus
 *    The content the four interrupt registers. LSByte is INT_STATUS, MSByte is
 *    INT_STATUS_3
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_interruptStatusRead(uint32_t *intStatus)
{
  uint8_t reg[4];

  ICM20648_registerRead(ICM20648_REG_INT_STATUS, 4, reg);
  *intStatus = (uint32_t) reg[0];
  *intStatus |= ( ( (uint32_t) reg[1]) << 8);
  *intStatus |= ( ( (uint32_t) reg[2]) << 16);
  *intStatus |= ( ( (uint32_t) reg[3]) << 24);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Checks if new data is available for read
 *
 * @return
 *    Returns true if the Raw Data Ready interrupt bit set, false otherwise
 ******************************************************************************/
bool ICM20648_isDataReady(void)
{
  uint8_t status;
  bool ret;

  ret = false;
  ICM20648_registerRead(ICM20648_REG_INT_STATUS_1, 1, &status);

  if ( status & ICM20648_BIT_RAW_DATA_0_RDY_INT ) {
    ret = true;
  }

  return ret;
}

/***************************************************************************//**
 * @brief
 *    Sets up and enables the Wake-up On Motion feature
 *
 * @param[in] enable
 *    If true enables the WOM feature, disables otherwise
 *
 * @param[in] womThreshold
 *    Threshold value for the Wake on Motion Interrupt for ACCEL x/y/z axes.
 *    LSB = 4mg. Range is 0mg to 1020mg
 *
 * @param[in] sampleRate
 *    The desired sample rate of the accel sensor in Hz
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_wakeOnMotionITEnable(bool enable, uint8_t womThreshold, float sampleRate)
{
  if ( enable ) {
    /* Make sure that the chip is not in sleep */
    ICM20648_sleepModeEnable(false);

    /* And in continuous mode */
    ICM20648_cycleModeEnable(false);

    /* Enable only the accelerometer */
    ICM20648_sensorEnable(true, false, false);

    /* Set sample rate */
    ICM20648_sampleRateSet(sampleRate);

    /* Set the bandwidth to 1210Hz */
    ICM20648_accelBandwidthSet(ICM20648_ACCEL_BW_1210HZ);

    /* Accel: 2G full scale */
    ICM20648_accelFullscaleSet(ICM20648_ACCEL_FULLSCALE_2G);

    /* Enable the Wake On Motion interrupt */
    ICM20648_interruptEnable(false, true);
    UTIL_delay(50);

    /* Enable Wake On Motion feature */
    ICM20648_registerWrite(ICM20648_REG_ACCEL_INTEL_CTRL, ICM20648_BIT_ACCEL_INTEL_EN | ICM20648_BIT_ACCEL_INTEL_MODE);

    /* Set the wake on motion threshold value */
    ICM20648_registerWrite(ICM20648_REG_ACCEL_WOM_THR, womThreshold);

    /* Enable low power mode */
    ICM20648_lowPowerModeEnter(true, false, false);
  } else {
    /* Disable Wake On Motion feature */
    ICM20648_registerWrite(ICM20648_REG_ACCEL_INTEL_CTRL, 0x00);

    /* Disable the Wake On Motion interrupt */
    ICM20648_interruptEnable(false, false);

    /* Disable cycle mode */
    ICM20648_cycleModeEnable(false);
  }

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Accelerometer and gyroscope calibration function. Reads the gyroscope
 *    and accelerometer values while the device is at rest and in level. The
 *    resulting values are loaded to the accel and gyro bias registers to cancel
 *    the static offset error.
 *
 * @param[out] accelBiasScaled
 *    The mesured acceleration sensor bias in mg
 *
 * @param[out] gyroBiasScaled
 *    The mesured gyro sensor bias in deg/sec
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_accelGyroCalibrate(float *accelBiasScaled, float *gyroBiasScaled)
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
  ICM20648_sensorEnable(true, true, false);

  /* Set 1kHz sample rate */
  ICM20648_sampleRateSet(1100.0);

  /* 246Hz BW for the accelerometer and 200Hz for the gyroscope */
  ICM20648_accelBandwidthSet(ICM20648_ACCEL_BW_246HZ);
  ICM20648_gyroBandwidthSet(ICM20648_GYRO_BW_12HZ);

  /* Set the most sensitive range: 2G full scale and 250dps full scale */
  ICM20648_accelFullscaleSet(ICM20648_ACCEL_FULLSCALE_2G);
  ICM20648_gyroFullscaleSet(ICM20648_GYRO_FULLSCALE_250DPS);

  /* Retrieve the resolution per bit */
  ICM20648_accelResolutionGet(&accelRes);
  ICM20648_gyroResolutionGet(&gyroRes);

  /* The accel sensor needs max 30ms, the gyro max 35ms to fully start */
  /* Experiments show that the gyro needs more time to get reliable results */
  UTIL_delay(50);

  /* Disable the FIFO */
  ICM20648_registerWrite(ICM20648_REG_USER_CTRL, ICM20648_BIT_FIFO_EN);
  ICM20648_registerWrite(ICM20648_REG_FIFO_MODE, 0x0F);

  /* Enable accelerometer and gyro to store the data in FIFO */
  ICM20648_registerWrite(ICM20648_REG_FIFO_EN_2, ICM20648_BIT_ACCEL_FIFO_EN | ICM20648_BITS_GYRO_FIFO_EN);

  /* Reset the FIFO */
  ICM20648_registerWrite(ICM20648_REG_FIFO_RST, 0x0F);
  ICM20648_registerWrite(ICM20648_REG_FIFO_RST, 0x00);

  /* Enable the FIFO */
  ICM20648_registerWrite(ICM20648_REG_USER_CTRL, ICM20648_BIT_FIFO_EN);

  /* The max FIFO size is 4096 bytes, one set of measurements takes 12 bytes */
  /* (3 axes, 2 sensors, 2 bytes each value ) 340 samples use 4080 bytes of FIFO */
  /* Loop until at least 4080 samples gathered */
  fifoCount = 0;
  while ( fifoCount < 4080 ) {
    UTIL_delay(5);
    /* Read FIFO sample count */
    ICM20648_registerRead(ICM20648_REG_FIFO_COUNT_H, 2, &data[0]);
    /* Convert to a 16 bit value */
    fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);
  }

  /* Disable accelerometer and gyro to store the data in FIFO */
  ICM20648_registerWrite(ICM20648_REG_FIFO_EN_2, 0x00);

  /* Read FIFO sample count */
  ICM20648_registerRead(ICM20648_REG_FIFO_COUNT_H, 2, &data[0]);

  /* Convert to a 16 bit value */
  fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);

  /* Calculate the number of data sets (3 axis of accel an gyro, two bytes each = 12 bytes) */
  packetCount = fifoCount / 12;

  /* Retrieve the data from the FIFO */
  for ( i = 0; i < packetCount; i++ ) {
    ICM20648_registerRead(ICM20648_REG_FIFO_R_W, 12, &data[0]);
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
  ICM20648_registerRead(ICM20648_REG_XG_OFFS_USRH, 2, &data[0]);
  gyroBiasStored[0] = ( (int16_t) (data[0] << 8) | data[1]);
  ICM20648_registerRead(ICM20648_REG_YG_OFFS_USRH, 2, &data[0]);
  gyroBiasStored[1] = ( (int16_t) (data[0] << 8) | data[1]);
  ICM20648_registerRead(ICM20648_REG_ZG_OFFS_USRH, 2, &data[0]);
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
  ICM20648_registerWrite(ICM20648_REG_XG_OFFS_USRH, data[0]);
  ICM20648_registerWrite(ICM20648_REG_XG_OFFS_USRL, data[1]);
  ICM20648_registerWrite(ICM20648_REG_YG_OFFS_USRH, data[2]);
  ICM20648_registerWrite(ICM20648_REG_YG_OFFS_USRL, data[3]);
  ICM20648_registerWrite(ICM20648_REG_ZG_OFFS_USRH, data[4]);
  ICM20648_registerWrite(ICM20648_REG_ZG_OFFS_USRL, data[5]);

  /* Calculate the accelerometer bias values to store in the hardware accelerometer bias registers. These registers contain */
  /* factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold */
  /* non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature */
  /* compensation calculations(? the datasheet is not clear). Accelerometer bias registers expect bias input */
  /* as 2048 LSB per g, so that the accelerometer biases calculated above must be divided by 8. */

  /* Read factory accelerometer trim values */
  ICM20648_registerRead(ICM20648_REG_XA_OFFSET_H, 2, &data[0]);
  accelBiasFactory[0] = ( (int16_t) (data[0] << 8) | data[1]);
  ICM20648_registerRead(ICM20648_REG_YA_OFFSET_H, 2, &data[0]);
  accelBiasFactory[1] = ( (int16_t) (data[0] << 8) | data[1]);
  ICM20648_registerRead(ICM20648_REG_ZA_OFFSET_H, 2, &data[0]);
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
  ICM20648_registerWrite(ICM20648_REG_XA_OFFSET_H, data[0]);
  ICM20648_registerWrite(ICM20648_REG_XA_OFFSET_L, data[1]);
  ICM20648_registerWrite(ICM20648_REG_YA_OFFSET_H, data[2]);
  ICM20648_registerWrite(ICM20648_REG_YA_OFFSET_L, data[3]);
  ICM20648_registerWrite(ICM20648_REG_ZA_OFFSET_H, data[4]);
  ICM20648_registerWrite(ICM20648_REG_ZA_OFFSET_L, data[5]);

  /* Convert the values to G for displaying */
  accelBiasScaled[0] = (float) accelBias[0] * accelRes;
  accelBiasScaled[1] = (float) accelBias[1] * accelRes;
  accelBiasScaled[2] = (float) accelBias[2] * accelRes;

  /* Turn off FIFO */
  ICM20648_registerWrite(ICM20648_REG_USER_CTRL, 0x00);

  /* Disable all sensors */
  ICM20648_sensorEnable(false, false, false);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Gyroscope calibration function. Reads the gyroscope
 *    values while the device is at rest and in level. The
 *    resulting values are loaded to the gyro bias registers to cancel
 *    the static offset error.
 *
 * @param[out] gyroBiasScaled
 *    The mesured gyro sensor bias in deg/sec
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_gyroCalibrate(float *gyroBiasScaled)
{
  uint8_t data[12];
  uint16_t i, packetCount, fifoCount;
  int32_t gyroBias[3] = { 0, 0, 0 };
  int32_t gyroTemp[3];
  int32_t gyroBiasStored[3];
  float gyroRes;

  /* Enable the accelerometer and the gyro */
  ICM20648_sensorEnable(true, true, false);

  /* Set 1kHz sample rate */
  ICM20648_sampleRateSet(1100.0);

  /* Configure bandwidth for gyroscope to 12Hz */
  ICM20648_gyroBandwidthSet(ICM20648_GYRO_BW_12HZ);

  /* Configure sensitivity to 250dps full scale */
  ICM20648_gyroFullscaleSet(ICM20648_GYRO_FULLSCALE_250DPS);

  /* Retrieve the resolution per bit */
  ICM20648_gyroResolutionGet(&gyroRes);

  /* The accel sensor needs max 30ms, the gyro max 35ms to fully start */
  /* Experiments show that the gyro needs more time to get reliable results */
  UTIL_delay(50);

  /* Disable the FIFO */
  ICM20648_registerWrite(ICM20648_REG_USER_CTRL, ICM20648_BIT_FIFO_EN);
  ICM20648_registerWrite(ICM20648_REG_FIFO_MODE, 0x0F);

  /* Enable accelerometer and gyro to store the data in FIFO */
  ICM20648_registerWrite(ICM20648_REG_FIFO_EN_2, ICM20648_BITS_GYRO_FIFO_EN);

  /* Reset the FIFO */
  ICM20648_registerWrite(ICM20648_REG_FIFO_RST, 0x0F);
  ICM20648_registerWrite(ICM20648_REG_FIFO_RST, 0x00);

  /* Enable the FIFO */
  ICM20648_registerWrite(ICM20648_REG_USER_CTRL, ICM20648_BIT_FIFO_EN);

  /* The max FIFO size is 4096 bytes, one set of measurements takes 12 bytes */
  /* (3 axes, 2 sensors, 2 bytes each value ) 340 samples use 4080 bytes of FIFO */
  /* Loop until at least 4080 samples gathered */
  fifoCount = 0;
  while ( fifoCount < 4080 ) {
    UTIL_delay(5);

    /* Read FIFO sample count */
    ICM20648_registerRead(ICM20648_REG_FIFO_COUNT_H, 2, &data[0]);

    /* Convert to a 16 bit value */
    fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);
  }

  /* Disable accelerometer and gyro to store the data in FIFO */
  ICM20648_registerWrite(ICM20648_REG_FIFO_EN_2, 0x00);

  /* Read FIFO sample count */
  ICM20648_registerRead(ICM20648_REG_FIFO_COUNT_H, 2, &data[0]);

  /* Convert to a 16 bit value */
  fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);

  /* Calculate the number of data sets (3 axis of accel an gyro, two bytes each = 12 bytes) */
  packetCount = fifoCount / 12;

  /* Retrieve the data from the FIFO */
  for ( i = 0; i < packetCount; i++ ) {
    ICM20648_registerRead(ICM20648_REG_FIFO_R_W, 12, &data[0]);
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
  ICM20648_registerRead(ICM20648_REG_XG_OFFS_USRH, 2, &data[0]);
  gyroBiasStored[0] = ( (int16_t) (data[0] << 8) | data[1]);

  ICM20648_registerRead(ICM20648_REG_YG_OFFS_USRH, 2, &data[0]);
  gyroBiasStored[1] = ( (int16_t) (data[0] << 8) | data[1]);

  ICM20648_registerRead(ICM20648_REG_ZG_OFFS_USRH, 2, &data[0]);
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
  ICM20648_registerWrite(ICM20648_REG_XG_OFFS_USRH, data[0]);
  ICM20648_registerWrite(ICM20648_REG_XG_OFFS_USRL, data[1]);
  ICM20648_registerWrite(ICM20648_REG_YG_OFFS_USRH, data[2]);
  ICM20648_registerWrite(ICM20648_REG_YG_OFFS_USRL, data[3]);
  ICM20648_registerWrite(ICM20648_REG_ZG_OFFS_USRH, data[4]);
  ICM20648_registerWrite(ICM20648_REG_ZG_OFFS_USRL, data[5]);

  /* Turn off FIFO */
  ICM20648_registerWrite(ICM20648_REG_USER_CTRL, 0x00);

  /* Disable all sensors */
  ICM20648_sensorEnable(false, false, false);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Reads the temperature sensor raw value and converts to Celsius.
 *
 * @param[out] temperature
 *    The mesured temperature in Celsius
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_temperatureRead(float *temperature)
{
  uint8_t data[2];
  int16_t raw_temp;

  /* Read temperature registers */
  ICM20648_registerRead(ICM20648_REG_TEMPERATURE_H, 2, data);

  /* Convert to int16 */
  raw_temp = (int16_t) ( (data[0] << 8) + data[1]);

  /* Calculate the Celsius value from the raw reading */
  *temperature = ( (float) raw_temp / 333.87) + 21.0;

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Reads the device ID of the ICM20648
 *
 * @param[out] devID
 *    The ID of the device read from teh WHO_AM_I register. Expected value? 0xE0
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_getDeviceID(uint8_t *devID)
{
  ICM20648_registerRead(ICM20648_REG_WHO_AM_I, 1, devID);

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Initializes the SPI bus in order to communicate with the ICM20648
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t ICM20648_spiInit(void)
{
  USART_TypeDef *usart = ICM20648_SPI_USART;
  USART_InitSync_TypeDef init = ICM20648_USART_INITSYNC;

  /* Enabling clock to USART */
#if defined(_SILICON_LABS_32B_SERIES_2)
  CMU_ClockEnable(cmuClock_PCLK, true);
#else
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif
  CMU_ClockEnable(ICM20648_SPI_CLK, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* IO configuration */
  GPIO_PinModeSet(ICM20648_PORT_SPI_MOSI, ICM20648_PIN_SPI_MOSI, gpioModePushPull, 0);    /* TX - MOSI */
  GPIO_PinModeSet(ICM20648_PORT_SPI_MISO, ICM20648_PIN_SPI_MISO, gpioModeInput, 0);    /* RX - MISO */
  GPIO_PinModeSet(ICM20648_PORT_SPI_SCLK, ICM20648_PIN_SPI_SCLK, gpioModePushPull, 0);    /* Clock */
  GPIO_PinModeSet(ICM20648_PORT_SPI_CS, ICM20648_PIN_SPI_CS, gpioModePushPull, 1);     /* CS */

  USART_Reset(ICM20648_SPI_USART);

  /* Initialize USART, in SPI master mode. */
  USART_InitSync(usart, &init);

  /* Enable pins at correct UART/USART location. */
#ifdef _GPIO_USART_ROUTEEN_MASK
  GPIO->USARTROUTE[ICM20648_SPI_USART_INDEX].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_CLKPEN;
  GPIO->USARTROUTE[ICM20648_SPI_USART_INDEX].TXROUTE |= (ICM20648_PORT_SPI_MOSI << _GPIO_USART_TXROUTE_PORT_SHIFT) | (ICM20648_PIN_SPI_MOSI << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[ICM20648_SPI_USART_INDEX].RXROUTE |= (ICM20648_PORT_SPI_MISO << _GPIO_USART_RXROUTE_PORT_SHIFT) | (ICM20648_PIN_SPI_MISO << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[ICM20648_SPI_USART_INDEX].CLKROUTE |= (ICM20648_PORT_SPI_SCLK << _GPIO_USART_CLKROUTE_PORT_SHIFT) | (ICM20648_PIN_SPI_SCLK << _GPIO_USART_CLKROUTE_PIN_SHIFT);
#else
  usart->ROUTEPEN   = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_CLKPEN;
  usart->ROUTELOC0  = (usart->ROUTELOC0 & ~(_USART_ROUTELOC0_TXLOC_MASK | _USART_ROUTELOC0_RXLOC_MASK | _USART_ROUTELOC0_CLKLOC_MASK) );
  usart->ROUTELOC0 |= (ICM20648_LOCATION_SPI_MOSI << _USART_ROUTELOC0_TXLOC_SHIFT);
  usart->ROUTELOC0 |= (ICM20648_LOCATION_SPI_MISO << _USART_ROUTELOC0_RXLOC_SHIFT);
  usart->ROUTELOC0 |= (ICM20648_LOCATION_SPI_SCLK << _USART_ROUTELOC0_CLKLOC_SHIFT);
#endif

  return ICM20648_OK;
}

/***************************************************************************//**
 * @brief
 *    Reads register from the ICM20648 device
 *
 * @param[in] addr
 *    The register address to read from in the sensor
 *    Bit[8:7] - bank address
 *    Bit[6:0] - register address
 *
 * @param[in] numBytes
 *    The number of bytes to read
 *
 * @param[out] data
 *    The data read from the register
 *
 * @return
 *    None
 ******************************************************************************/
void ICM20648_registerRead(uint16_t addr, int numBytes, uint8_t *data)
{
  uint8_t regAddr;
  uint8_t bank;

  regAddr = (uint8_t) (addr & 0x7F);
  bank = (uint8_t) (addr >> 7);

  ICM20648_bankSelect(bank);

  /* Enable chip select */
  ICM20648_chipSelectSet(true);

  /* Set R/W bit to 1 - read */
  USART_Tx(ICM20648_SPI_USART, (regAddr | 0x80) );
  USART_Rx(ICM20648_SPI_USART);
  /* Transmit 0's to provide clock and read the data */
  while ( numBytes-- ) {
    USART_Tx(ICM20648_SPI_USART, 0x00);
    *data++ = USART_Rx(ICM20648_SPI_USART);
  }

  /* Disable chip select */
  ICM20648_chipSelectSet(false);

  return;
}

/***************************************************************************//**
 * @brief
 *    Writes a register in the ICM20648 device
 *
 * @param[in] addr
 *    The register address to write
 *    Bit[8:7] - bank address
 *    Bit[6:0] - register address
 *
 * @param[in] data
 *    The data to write to the register
 *
 * @return
 *    None
 ******************************************************************************/
void ICM20648_registerWrite(uint16_t addr, uint8_t data)
{
  uint8_t regAddr;
  uint8_t bank;

  regAddr = (uint8_t) (addr & 0x7F);
  bank = (uint8_t) (addr >> 7);

  ICM20648_bankSelect(bank);

  /* Enable chip select */
  ICM20648_chipSelectSet(true);

  /* clear R/W bit - write, send the address */
  USART_Tx(ICM20648_SPI_USART, (regAddr & 0x7F) );
  USART_Rx(ICM20648_SPI_USART);

  /* Send the data */
  USART_Tx(ICM20648_SPI_USART, data);
  USART_Rx(ICM20648_SPI_USART);

  /* Disable chip select */
  ICM20648_chipSelectSet(false);

  return;
}

/***************************************************************************//**
 * @brief
 *    Select the desired register bank
 *
 * @param[in] bank
 *    The address of the register bank (0..3)
 *
 * @return
 *    None
 ******************************************************************************/
void ICM20648_bankSelect(uint8_t bank)
{
  /* Enable chip select */
  ICM20648_chipSelectSet(true);

  /* Select the Bank Select register */
  USART_Tx(ICM20648_SPI_USART, ICM20648_REG_BANK_SEL);
  USART_Rx(ICM20648_SPI_USART);

  /* Write the desired bank address 0..3 */
  USART_Tx(ICM20648_SPI_USART, (bank << 4) );
  USART_Rx(ICM20648_SPI_USART);

  /* Disable chip select */
  ICM20648_chipSelectSet(false);

  return;
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
static void ICM20648_chipSelectSet(bool select)
{
  if ( select ) {
    GPIO_PinOutClear(ICM20648_PORT_SPI_CS, ICM20648_PIN_SPI_CS);
  } else {
    GPIO_PinOutSet(ICM20648_PORT_SPI_CS, ICM20648_PIN_SPI_CS);
  }

  return;
}

/** @endcond */

/** @} (end defgroup ICM20648) */
/** @} {end addtogroup TBSense_BSP} */
