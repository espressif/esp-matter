/***************************************************************************//**
 * @file
 * @brief Driver for the Si72xx Hall Effect Sensor
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

#include "i2cspm.h"
#include "si72xx.h"
#include <stddef.h>
#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Si72xx
 * @brief Silicon Labs Si72xx Magnetic Hall Effect Sensor I2C driver.
 * @details
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define SI72XX_I2C_ADDR               0x30

#define SI72XX_SWOP_LATCHMODE         0x7F
#define SI72XX_FIELDPOLSEL_LATCHMODE  2
#define SI72XX_FIELDPOLSEL_SHIFT      6
#define SI72XX_SWHYST_LATCHMODE       0x12

#define SI72XX_OTP_BUSY_MASK          1
#define SI72XX_OTP_READ_EN_MASK       2

#define SI72XX_DSPSIGSEL_MASK         0x07
#define SI72XX_DSPSIGSEL_TEMP         0x01

#define SI72XX_ZERO_FIELD             16384
#define SI72XX_FRESH_BIT_MASK         0x80
#define SI72XX_FRESH_BIT_SHIFT        7
#define SI72XX_DSPSIGM_MASK           0x7F
#define SI72XX_DSPSIGM_SHIFT          8

#define SI72XX_SLEEP_MASK             1
#define SI72XX_STOP_MASK              2
#define SI72XX_ONEBURST_MASK          4
#define SI72XX_USESTORE_MASK          8
#define SI72XX_POWERCTRL_MASK         0x0F
#define SI72XX_MEASRO_MASK            0x80
#define SI72XX_MEASRO_SHIFT           7

#define SI72XX_SW_LOW4FIELD_MASK      0x80
#define SI72XX_SW_OP_MASK             0x7F

#define SI72XX_SWFIELDPOLSEL_MASK     0xC0
#define SI72XX_SWHYST_MASK            0x3F

#define SI72XX_SLTIMEENA_MASK         1
#define SI72XX_SL_FAST_MASK           2
#define SI72XX_SW_TAMPER_MASK         0xFC

#define SI72XX_DFBW                   0x0E
#define SI72XX_DFBW_MASK              0x1E
#define SI72XX_DFIIR_MASK             1

#define SI72XX_REV_MASK               0x0F
#define SI72XX_ID_MASK                0xF0
#define SI72XX_ID_SHIFT               4

#define SI72XX_BASE_PART_NUMBER       0x14
#define SI72XX_PART_VARIANT           0x15
/** @endcond */

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***********************************************************************//**
 * @brief
 *  Reads register from the Si72xx sensor.
 *  Command can only be issued if Si72xx is idle mode.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] data
 *   The data read from the sensor.
 * @param[in] reg
 *   The register address to read from in the sensor.
 * @return
 *   Returns number of bytes read on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t Si72xx_Read_Register(I2C_TypeDef *i2c,
                              uint8_t addr,
                              uint8_t reg,
                              uint8_t *data)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to start reading from */
  i2c_write_data[0] = reg;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  /* Select length of data to be read */
  seq.buf[1].data = data;
  seq.buf[1].len  = 1;

  ret = I2CSPM_Transfer(i2c, &seq);
  if (ret != i2cTransferDone) {
    *data = 0xff;
    return (uint32_t)ret;
  }
  return (uint32_t)0;
}

/***********************************************************************//**
 * @brief
 *   Writes register in the Si72xx sensor.
 *   Command can only be issued if Si72xx is idle mode.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[in] data
 *   The data to write to the sensor.
 * @param[in] reg
 *   The register address to write to in the sensor.
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CCSPM
 **************************************************************************/
uint32_t Si72xx_Write_Register(I2C_TypeDef *i2c,
                               uint8_t addr,
                               uint8_t reg,
                               uint8_t data)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[2];
  uint8_t i2c_read_data[1];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE;
  /* Select register and data to write */
  i2c_write_data[0] = reg;
  i2c_write_data[1] = data;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 2;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2c, &seq);
  if (ret != i2cTransferDone) {
    return (uint32_t)ret;
  }
  return (uint32_t)0;
}

/***********************************************************************//**
 * @brief
 *   Read out Si72xx Magnetic Field Conversion Data
 *   Command can only be issued if Si72xx is idle mode.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor
 * @param[out] magData
 *   Mag-field conversion reading, signed 16-bit integer
 **************************************************************************/
uint32_t Si72xx_Read_MagField_Data(I2C_TypeDef *i2c,
                                   uint8_t addr,
                                   int16_t *magData)
{
  uint8_t read = 0;
  uint32_t result = 0;
  uint8_t freshBit = 0;

  result = Si72xx_Read_Register(i2c, addr, SI72XX_DSPSIGM, &read);

  freshBit = ((read & SI72XX_FRESH_BIT_MASK) >> SI72XX_FRESH_BIT_SHIFT);

  *magData = ((((uint16_t)read) & SI72XX_DSPSIGM_MASK) << SI72XX_DSPSIGM_SHIFT);

  result |= Si72xx_Read_Register(i2c, addr, SI72XX_DSPSIGL, &read);
  /* Data code output is 15-bit unsigned value where 0mT=16384 */
  *magData |= read;
  /* Converts data code output to a signed integer */
  *magData = *magData - SI72XX_ZERO_FIELD;
  if (freshBit == 0) {
    result = SI72XX_ERROR_NODATA;
  }
  return result;
}

/***********************************************************************//**
 * @brief
 *   Puts Si72xx into Sleep mode (lowest power)..
 *   Command can only be issued if Si72xx is idle mode.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 **************************************************************************/
uint32_t Si72xx_FromIdle_GoToSleep(I2C_TypeDef *i2c, uint8_t addr)
{
  uint32_t result = 0;
  uint8_t read;

  result = Si72xx_Read_Register(i2c, addr, SI72XX_CTRL3, &read);
  read = (read & (~SI72XX_SLTIMEENA_MASK));
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_CTRL3, read);
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  read = (read | SI72XX_SLEEP_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_POWER_CTRL, read);
  return result;
}

/***********************************************************************//**
 * @brief
 *   Puts Si72xx into Sleep-Timer-Enable mode.
 *   Si72xx periodically wakes-up, samples the magnetic field, updates the
 *   output, and goes back to sleep-timer-enabled mode.
 *   Command can only be issued if Si72xx is idle mode.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 **************************************************************************/
uint32_t Si72xx_FromIdle_GoToSltimeena(I2C_TypeDef *i2c, uint8_t addr)
{
  uint8_t read;
  uint32_t result = 0;

  result = Si72xx_Read_Register(i2c, addr, SI72XX_CTRL3, &read);
  read = ((read & ~SI72XX_SL_FAST_MASK) | SI72XX_SLTIMEENA_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_CTRL3, read);
  result = Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  read = (read & ~(SI72XX_ONEBURST_MASK
                   | SI72XX_STOP_MASK
                   | SI72XX_SLEEP_MASK));
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_POWER_CTRL, read);
  return result;
}

/***********************************************************************//**
 * @brief
 *   Wake-up Si72xx and places sensor in idle-mode.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CCSPM
 **************************************************************************/
uint32_t Si72xx_WakeUpAndIdle(I2C_TypeDef *i2c, uint8_t addr)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];
  uint8_t i2c_read_data[1];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE;
  /* Select register and data to write */
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 0;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2c, &seq);
  if (ret != i2cTransferDone) {
    return (uint32_t)ret;
  }
  return (uint32_t)0;
}

/***********************************************************************//**
 * @brief
 *   Read Si72xx OTP Data
 *   Command can only be issued if Si72xx is idle mode.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor
 * @param[in] otpAddr
 *       The OTB Byte address of the coefficients
 * @param[out] data
 *       OTP data read out
 **************************************************************************/
static uint32_t Si72xx_Read_OTP(I2C_TypeDef *i2c,
                                uint8_t addr,
                                uint8_t otpAddr,
                                uint8_t *otpData)
{
  uint8_t read;
  uint32_t result;
  result = Si72xx_Read_Register(i2c, addr, SI72XX_OTP_CTRL, &read);
  if (read & SI72XX_OTP_BUSY_MASK) {
    return SI72XX_ERROR_BUSY;
  }
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_OTP_ADDR, otpAddr);
  result |= Si72xx_Write_Register(i2c,
                                  addr,
                                  SI72XX_OTP_CTRL,
                                  SI72XX_OTP_READ_EN_MASK);
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_OTP_DATA, &read);
  *otpData =  read;
  return result;
}

/***********************************************************************//**
 * @brief
 *   Set magnetic-field output range, 20mT or 200mT full-scale
 *   Command can only be issued if Si72xx is idle mode.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor
 * @param[in] mTScale
 *   20mT or 200mT
 **************************************************************************/
static uint32_t Si72xx_Set_mT_Range(I2C_TypeDef *i2c,
                                    uint8_t addr,
                                    Si72xxFieldScale_t mTScale)
{
  uint8_t data;
  uint32_t result;
  uint8_t srcAddr = 0;

  if (mTScale == SI7210_20MT) {
    srcAddr = SI72XX_OTP_20MT_ADDR;
  } else if (mTScale == SI7210_200MT) {
    srcAddr = SI72XX_OTP_200MT_ADDR;
  }
  result = Si72xx_Read_OTP(i2c, addr, srcAddr++, &data);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_A0, data);
  result |= Si72xx_Read_OTP(i2c, addr, srcAddr++, &data);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_A1, data);
  result |= Si72xx_Read_OTP(i2c, addr, srcAddr++, &data);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_A2, data);
  result |= Si72xx_Read_OTP(i2c, addr, srcAddr++, &data);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_A3, data);
  result |= Si72xx_Read_OTP(i2c, addr, srcAddr++, &data);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_A4, data);
  result |= Si72xx_Read_OTP(i2c, addr, srcAddr++, &data);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_A5, data);
  return result;
}

/***********************************************************************//**
 * @brief
 *   Wake-up SI72xx, performs a magnetic-field conversion with FIR,
 *   and places Si72xx back to sleep-mode.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor
 * @param[in] mTScale
 *   mTScale= Si7210_20MT: 20mT full-scale magnetic-field range
 *   mTScale= Si7210_200MT: 200mT full-scale magnetic-field range
 * @param[in] sleepMode
 *   SI72XX_SLEEP: Sleep mode. Lowest power & doesn't update output
 *   SI72XX_SLTIMEENA: Sleep-Timer-Enabled mode. Updates output periodically
 * @param[out] magFieldData
 *   Magnetic-field conversion reading, signed 16-bit integer
 **************************************************************************/
uint32_t Si72xx_ReadMagFieldDataAndSleep(I2C_TypeDef *i2c,
                                         uint8_t addr,
                                         Si72xxFieldScale_t mTScale,
                                         Si72xxSleepMode_t sleepMode,
                                         int16_t *magFieldData)
{
  uint8_t read = 0;
  uint32_t result = 0;

  result = Si72xx_WakeUpAndIdle(i2c, addr);

  /* set the stop-bit */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  read = ((read & ~SI72XX_POWERCTRL_MASK) | SI72XX_STOP_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_POWER_CTRL, read);

  result |= Si72xx_Set_mT_Range(i2c, addr, mTScale);

  /* Set the burst-size for averaging */
  result |= Si72xx_Write_Register(i2c,
                                  addr,
                                  SI72XX_CTRL4,
                                  SI72XX_DFBW);

  /* Perform a magnetic field conversion */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  read = ((read & ~SI72XX_POWERCTRL_MASK) | SI72XX_ONEBURST_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_POWER_CTRL, read);

  /* Wait for measurement to complete */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  while (((read & SI72XX_MEASRO_MASK) >> SI72XX_MEASRO_SHIFT) && (!result)) {
    result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  }

  result |= Si72xx_Read_MagField_Data(i2c, addr, &*magFieldData);

  if (sleepMode == SI72XX_SLEEP_MODE) {
    result |= Si72xx_FromIdle_GoToSleep(i2c, addr);
  } else if (sleepMode == SI72XX_SLTIMEENA_MODE) {
    result |= Si72xx_FromIdle_GoToSltimeena(i2c, addr);
  }

  return result;
}

/**************************************************************************//**
 * @brief  Convert Si7210 I2C Data Readings to Magnetic Field in microTeslas
 * @param[in] fieldScale
 *   20mT or 200mT full-scale magnetic field range
 * @param[in] dataCodes
 *   signed 15bit value read from hall sensor after magnetic field conversion
 * @return microTeslas
 *****************************************************************************/
int32_t Si72xx_ConvertDataCodesToMagneticField(Si72xxFieldScale_t fieldScale,
                                               int16_t dataCode)
{
  int32_t microTeslas = 0;

  if (fieldScale == SI7210_20MT) {
    /* 20mT: 1(LSB) = 1.25uT */
    microTeslas = (dataCode * 125) / 100;
  } else if (fieldScale == SI7210_200MT) {
    /* 200mT: 1(LSB) = 12.5uT */
    microTeslas = (dataCode * 125) / 10;
  }
  return microTeslas;
}

/***********************************************************************//**
 * @brief
 *   Wake-up Si72xx, and set sleep-mode option.
 *   If Si72xx is in a sleep-mode, it requires a wake-up command first.
 *   Useful for placing Si72xx in SLTIMEENA mode from SLEEP mode,
 *   or vice-versa.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor
 * @param[in] sleepMode
 *   SI72XX_SLEEP: Puts Si72xx into sleep mode. Lowest power & doesn't update
 *   SI72XX_SLTIMEENA: Si72xx into sltimeena mode. Updates output periodically
 **************************************************************************/
uint32_t Si72xx_EnterSleepMode(I2C_TypeDef *i2c,
                               uint8_t addr,
                               Si72xxSleepMode_t sleepMode)
{
  uint32_t result = 0;
  result = Si72xx_WakeUpAndIdle(i2c, addr);
  if (sleepMode == SI72XX_SLEEP_MODE) {
    result |= Si72xx_FromIdle_GoToSleep(i2c, addr);
  } else if (sleepMode == SI72XX_SLTIMEENA_MODE) {
    result |= Si72xx_FromIdle_GoToSltimeena(i2c, addr);
  }
  return result;
}

/***********************************************************************//**
 * @brief
 *   Wake-up Si72xx, and configures output for Latch mode.
 *   Switch point = 0mT w/ 0.2mT hysteresis
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor
 **************************************************************************/
uint32_t Si72xx_EnterLatchMode(I2C_TypeDef *i2c, uint8_t addr)
{
  uint8_t read;
  uint32_t result = 0;

  result = Si72xx_WakeUpAndIdle(i2c, addr);

  /* Set Stop-bit */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  read = (read | SI72XX_USESTORE_MASK | SI72XX_STOP_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_POWER_CTRL, read);

  /* Set output high for low magnetic field */
  /* Set sw_op to zero for latch mode */
  read = (SI72XX_SWOP_LATCHMODE | SI72XX_SW_LOW4FIELD_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_CTRL1, read);

  /* Set output to unipolar positive with hysteresis = 0.2mT */
  read = ((SI72XX_FIELDPOLSEL_LATCHMODE << SI72XX_FIELDPOLSEL_SHIFT)
          | SI72XX_SWHYST_LATCHMODE);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_CTRL2, read);

  /* Enable the sleep-timer for periodic measurements */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_CTRL3, &read);
  read = (read | SI72XX_SLTIMEENA_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_CTRL3, read);

  /* Clear stop-bit */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  read = (read & ~SI72XX_STOP_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_POWER_CTRL, read);

  return result;
}

/***********************************************************************//**
 * @brief
 *   Wakes up SI72xx, performs temperature conversion and places Si72xx
 *   into SI72XX_SLEEP sleep-mode.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor
 * @param[out] temp
 *   Temperature measurement in millidegree Celsius
 **************************************************************************/
uint32_t Si72xx_ReadTemperatureAndSleep(I2C_TypeDef *i2c,
                                        uint8_t addr,
                                        int32_t *rawTemp)
{
  uint32_t result = 0;
  uint8_t read, dspSigM, dspSigL;
  uint8_t freshBit;
  int16_t dataValue;
  int32_t milliCelsius;

  uint16_t tempCtrl4Setting = 0x00;

  result = Si72xx_WakeUpAndIdle(i2c, addr);

  /* Set stop-bit */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  read = ((read & ~SI72XX_POWERCTRL_MASK) | SI72XX_STOP_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_POWER_CTRL, read);

  /* clear IIR & FIR filtering */
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_CTRL4, tempCtrl4Setting);

  /* Select temperature conversion */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_DSPSIGSEL, &read);
  read = ((read & ~SI72XX_DSPSIGSEL_MASK) | SI72XX_DSPSIGSEL_TEMP);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_DSPSIGSEL, read);

  /* Perform temperature conversion */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  read = (((read & (~SI72XX_STOP_MASK)) & ~(SI72XX_SLEEP_MASK))
          | SI72XX_ONEBURST_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_POWER_CTRL, read);

  /* Read conversion result */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_DSPSIGM, &dspSigM);
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_DSPSIGL, &dspSigL);

  result |= Si72xx_FromIdle_GoToSleep(i2c, addr);

  freshBit = dspSigM >> SI72XX_FRESH_BIT_SHIFT;
  if (freshBit == 0) {
    return SI72XX_ERROR_NODATA;
  }

  /* dataValue = (Dspigm[6:0]<<5) + (Dspigl[7:0]>>3) */
  dataValue = (((uint16_t)dspSigM) & SI72XX_DSPSIGM_MASK) << 8;
  dataValue = (dataValue | dspSigL) >> 3;

  /* rawTemp(mC) = ((dataValue^2)*(-3.83*10^-6))+(0.16094*dataValue)-279.8 */
  milliCelsius = ((int32_t)dataValue * (int32_t)dataValue * -383 / 100000)
                 + ((16094 * dataValue) / 100) - 279800;

  *rawTemp = milliCelsius;

  return result;
}

/***********************************************************************//**
 * @brief
 *   Wakes up SI72xx, performs temperature conversion and places Si72xx
 *   into SI72XX_SLEEP sleep-mode.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor
 * @param[out] offsetValue
 *   Temperature offset correction
 * @param[out] gainValue
 *   Temperature gain correction
 **************************************************************************/
uint32_t Si72xx_ReadTempCorrectionDataAndSleep(I2C_TypeDef *i2c,
                                               uint8_t addr,
                                               int16_t *offsetValue,
                                               int16_t *gainValue)
{
  uint32_t result = 0;
  uint8_t read;

  uint8_t offsetAddr = 0x1D;
  uint8_t gainAddr = 0x1E;

  result = Si72xx_WakeUpAndIdle(i2c, addr);

  /* Set Stop-bit */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  read = ((read & ~SI72XX_POWERCTRL_MASK) | SI72XX_STOP_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_POWER_CTRL, read);

  /* Read offset register value */
  result |= Si72xx_Read_OTP(i2c, addr, offsetAddr, &read);
  /* Calculate offset: Offset = value(0x1D)/16 */
  *offsetValue = (int8_t)read * 1000 / 16;

  /* Read gain register value */
  result |= Si72xx_Read_OTP(i2c, addr, gainAddr, &read);
  /* calculate gain: Gain = (value(0x1E)/2048) + 1 */
  *gainValue = ((int8_t)read * 1000 / 2048) + 1000;

  result |= Si72xx_FromIdle_GoToSleep(i2c, addr);
  return result;
}

/**************************************************************************//**
 * @brief
 *   Wakes up SI72xx, performs a temperature conversion, and places sensor
 *   back to sleep. Temperature calculation is performed using compensation
 *   data.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor
 * @param[out] temp
 *   Temperature measurement in millidegree Celsius
 * @param[in] offsetData
 *   Offset correction data
 * @param[in] gainData
 *   Gain correction data
 *****************************************************************************/
uint32_t Si72xx_ReadCorrectedTempAndSleep(I2C_TypeDef *i2c,
                                          uint8_t addr,
                                          int16_t offsetData,
                                          int16_t gainData,
                                          int32_t *correctedTemp)
{
  uint32_t result = 0;
  uint8_t read, dspSigM, dspSigL;
  uint8_t freshBit;
  int16_t dataValue;
  int32_t rawTemp;
  int32_t milliCelsius;

  uint16_t tempCtrl4Setting = 0x00;

  result = Si72xx_WakeUpAndIdle(i2c, addr);

  /* Set stop-bit */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  read = ((read & ~SI72XX_POWERCTRL_MASK) | SI72XX_STOP_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_POWER_CTRL, read);

  /* Clear IIR and FIR filtering */
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_CTRL4, tempCtrl4Setting);

  /* Select Temperature conversion */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_DSPSIGSEL, &read);
  read = ((read & ~SI72XX_DSPSIGSEL_MASK) | SI72XX_DSPSIGSEL_TEMP);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_DSPSIGSEL, read);

  /* Perform temperature conversion */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  read = (((read & (~SI72XX_STOP_MASK)) & ~(SI72XX_SLEEP_MASK))
          | SI72XX_ONEBURST_MASK);
  result |= Si72xx_Write_Register(i2c, addr, SI72XX_POWER_CTRL, read);

  /* Read temperature conversion result */
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_DSPSIGM, &dspSigM);
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_DSPSIGL, &dspSigL);

  result |= Si72xx_FromIdle_GoToSleep(i2c, addr);

  freshBit = dspSigM >> SI72XX_FRESH_BIT_SHIFT;
  if (freshBit == 0) {
    return SI72XX_ERROR_NODATA;
  }

  /* dataValue = (Dspigm[6:0]<<5) + (Dspigl[7:0]>>3) */
  dataValue = (((uint16_t)dspSigM) & SI72XX_DSPSIGM_MASK) << 8;
  dataValue = (dataValue | dspSigL) >> 3;

  /* rawTemp equation is from Si7210 datasheet */
  /* rawTemp(mC) = ((dataValue^2)*(-3.83*10^-6))+(0.16094*dataValue)-279.8 */
  rawTemp = ((int32_t)dataValue * (int32_t)dataValue * -383 / 100000)
            + ((16094 * dataValue) / 100) - 279800;

  milliCelsius = ((rawTemp * (int32_t)gainData) + offsetData) / 1000;

  *correctedTemp = milliCelsius;

  return result;
}

/**************************************************************************
* @brief
*   Wake-up Si72xx, read out part Revision and ID, and place Si72xx
*   back to SLEEP sleep-mode.
* @param[in] i2c
*   The I2C peripheral to use (not used).
* @param[in] addr
*   The I2C address of the sensor
* @param[out] partId
*        Si7210 part ID
* @param[out] partRev
*        Si72xx part Revision
**************************************************************************/
uint32_t Si72xx_IdentifyAndSleep(I2C_TypeDef *i2c,
                                 uint8_t addr,
                                 uint8_t *partId,
                                 uint8_t *partRev)
{
  uint8_t read;
  uint32_t result = 0;
  result = Si72xx_WakeUpAndIdle(i2c, addr);
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_POWER_CTRL, &read);
  result |= Si72xx_Write_Register(i2c,
                                  addr,
                                  SI72XX_POWER_CTRL,
                                  (read | SI72XX_STOP_MASK));
  result |= Si72xx_Read_Register(i2c, addr, SI72XX_HREVID, &read);
  result |= Si72xx_FromIdle_GoToSleep(i2c, addr);

  *partRev = read & SI72XX_REV_MASK;
  *partId = read >> SI72XX_ID_SHIFT;

  return result;
}

/**************************************************************************
* @brief
*   Wake-up Si72xx, read out Si72xx base part-number and variant, and
*   place sensor back to SLEEP sleep-mode.
* @param[in] i2c
*   The I2C peripheral to use (not used).
* @param[in] addr
*   The I2C address of the sensor
* @param[out] basePn
*        Si7210 part ID
* @param[out] partRev
*        Si72xx part Revision
**************************************************************************/
uint32_t Si72xx_ReadVariantAndSleep(I2C_TypeDef *i2c,
                                    uint8_t addr,
                                    uint8_t *basePn,
                                    uint8_t *pnVariant)
{
  uint8_t read;
  uint32_t result = 0;
  result = Si72xx_WakeUpAndIdle(i2c, addr);
  result |= Si72xx_Read_OTP(i2c, addr, SI72XX_BASE_PART_NUMBER, &read);
  *basePn = read;
  result |= Si72xx_Read_OTP(i2c, addr, SI72XX_PART_VARIANT, &read);
  *pnVariant = read;
  result |= Si72xx_FromIdle_GoToSleep(i2c, addr);
  return result;
}

/** @} (end group Si72xx) */
/** @} (end group kitdrv) */
