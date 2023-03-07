/***************************************************************************//**
 * @file
 * @brief Driver for the Silicon Labs Si72xx Hall Effect Sensor
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

#include "hall_si7210.h"

#include <stddef.h>

#include "em_cmu.h"

#include "i2cspm.h"

#include "board.h"

#include "hall.h"
#include "hall_device.h"
#include "hall_config.h"

#include "hall_si7210_regs.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/**************************************************************************//**
* @addtogroup HALL
* @{
******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static void usDelay(uint32_t delay);
static uint32_t SI7210_loadCoeffsFromOtp(uint8_t otpAddr);

/** @endcond */

/**************************************************************************//**
 * @brief
 *    Does device-specific initializaiton for the Si7210 chip.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t HALL_initDevice(void)
{
  uint32_t status;
  uint8_t  chipId;
  uint8_t  revId;

  /* Try to contact the sensor and check it's device ID */
  status = SI7210_wakeUp();
  if ( status != HALL_OK ) {
    return status;
  }

  status = SI7210_identify(&chipId, &revId);
  if ( status != HALL_OK ) {
    return status;
  }

  /* printf("HALL CHIPID: %d, REVID: %d\r\n", chipId, revId); */

  if ( (chipId != SI7210_CHIP_ID) || (revId < SI7210_REV_ID_MIN) || (revId > SI7210_REV_ID_MAX) ) {
    status = HALL_ERROR_DEVICE_ID_MISMATCH;
  }

  return status;
}

/**************************************************************************//**
 * @brief
 *    Does device-specific de-initializaiton for the Si7210 chip.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t HALL_deInitDevice(void)
{
  return 0;
}

/**************************************************************************//**
 * @brief
 *    Configures the Si7210 chip.
 *
 * @param[in] config
 *    The structure, which contains the configuration parameters.
 *    If all fields are set to zero, the chip will revert to use default values.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t HALL_configure(HALL_Config *config)
{
  uint32_t status;
  uint8_t value;
  uint8_t sw_op, sw_hyst;

  /* Wake up device and/or stop measurements */
  status = SI7210_wakeUp();
  if ( status != HALL_OK ) {
    return status;
  }

  /* Stop the measurement loop */
  status = SI7210_regSetBits(SI7210_REG_ADDR_POWER_CTRL,
                             SI7210_REG_POWER_CTRL_STOP_MASK);

  if ( status != HALL_OK ) {
    return status;
  }

  if ( (config->threshold == 0.0)
       && (config->hysteresis == 0.0)
       && (config->polarity == 0)
       && (config->outputInvert == 0) ) {
    /* Use default values in the device for all parameters */
    status = SI7210_regClearBits(SI7210_REG_ADDR_POWER_CTRL,
                                 SI7210_REG_POWER_CTRL_USESTORE_MASK);

    if ( status != HALL_OK ) {
      return status;
    }
  } else {
    /* Program sw_low4field and sw_op bit fields */
    sw_op = SI7210_calculateSWOP(config->threshold);

    value = 0;

    if ( config->outputInvert ) {
      value = (1 << SI7210_REG_CTRL1_SW_LOW4FIELD_SHIFT) & SI7210_REG_CTRL1_SW_LOW4FIELD_MASK;
    }

    value |= (sw_op << SI7210_REG_CTRL1_SW_OP_SHIFT) & SI7210_REG_CTRL1_SW_OP_MASK;

    status = SI7210_registerWrite(SI7210_REG_ADDR_CTRL1, value);
    if ( status != HALL_OK ) {
      return status;
    }

    /* Program sw_fieldpolsel and sw_hyst bit fields */
    sw_hyst = SI7210_calculateSWHYST(config->hysteresis, false);

    value = (config->polarity << SI7210_REG_CTRL2_SW_FIELDPOLSEL_SHIFT) & SI7210_REG_CTRL2_SW_FIELDPOLSEL_MASK;

    value |= (sw_hyst << SI7210_REG_CTRL2_SW_HYST_SHIFT) & SI7210_REG_CTRL2_SW_HYST_MASK;

    status = SI7210_registerWrite(SI7210_REG_ADDR_CTRL2, value);
    if ( status != HALL_OK ) {
      return status;
    }

    /* Enable storing of these parameters in sleep mode */
    status = SI7210_regSetBits(SI7210_REG_ADDR_POWER_CTRL,
                               SI7210_REG_POWER_CTRL_USESTORE_MASK);

    if ( status != HALL_OK ) {
      return status;
    }
  }

  /* Enable sleep timer and clear stop bit to start operation */
  status = SI7210_regSetBits(SI7210_REG_ADDR_CTRL3,
                             SI7210_REG_CTRL3_SLTIMEENA_MASK);

  if ( status != HALL_OK ) {
    return status;
  }

  /* Resume operation  */
  status = SI7210_regClearBits(SI7210_REG_ADDR_POWER_CTRL,
                               SI7210_REG_POWER_CTRL_STOP_MASK);

  return status;
}

/**************************************************************************//**
 * @brief
 *    Performs a measurement
 *
 * @param[in] scale
 *    Desired scale in uT
 *
 * @param[out] result
 *    The measured field strength value in mT
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t HALL_measure(uint32_t scale, float *result)
{
  int32_t mT;
  uint32_t status;
  bool range200mT;

  if ( scale > 20500 ) {
    range200mT = true;
  } else {
    range200mT = false;
  }

  status = SI7210_readmTDataSltimeena(range200mT, &mT);

  *result = ((float) mT) / 1000;

  return status;
}

/**************************************************************************//**
 * @brief
 *    Returns the tamper level configured in the chip
 *
 * @return
 *    The tamper level in mT
 *****************************************************************************/
float HALL_getTamperLevel(void)
{
  return 19.87f;
}

/**************************************************************************//**
 * @defgroup Si7210 Si7210 - Hall Effect Sensor
 * @{
 * @brief Driver for the Silicon Labs Si7210 Hall effect sensor
 *****************************************************************************/

/**************************************************************************//**
 * @brief
 *    Read out Si7210 Conversion Data - 15bits
 *
 * @param[out] data
 *    The raw magnetic field conversion data (15 bits)
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_dataRead(int16_t *data)
{
  uint8_t read;
  uint8_t flag;
  uint32_t result;

  result = SI7210_registerRead(SI7210_REG_ADDR_DSPSIGM, &read);
  flag = read >> SI7210_REG_DSPSIGM_FRESH_SHIFT;
  *data = ((uint16_t)(read  & SI7210_REG_DSPSIGM_DSPSIGM_MASK)) << 8;
  result |= SI7210_registerRead(SI7210_REG_ADDR_DSPSIGL, &read);
  *data |= read;
  *data = *data - 16384;

  if ( flag == 0 ) {
    result = SI7210_ERROR_NODATA;
  }

  return result;
}

/**************************************************************************//**
 * @brief
 *    Puts Si7210 into Sleep (No-measurement) Mode
 *    Wake-up command needs to be issued to become responsive
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_sleep(void)
{
  uint32_t result;
  uint8_t read;

  result = SI7210_registerRead(SI7210_REG_ADDR_CTRL3, &read);
  // Clear the sleep timer enable bit
  read = (read & ~SI7210_REG_CTRL3_SLTIMEENA_MASK);
  result |= SI7210_registerWrite(SI7210_REG_ADDR_CTRL3, read);
  result |= SI7210_registerRead(SI7210_REG_ADDR_POWER_CTRL, &read);
  // Clear the oneburst and stop bits, set the sleep bit
  read = ((read & ~(SI7210_REG_POWER_CTRL_ONEBURST_MASK | SI7210_REG_POWER_CTRL_STOP_MASK)) | SI7210_REG_POWER_CTRL_SLEEP_MASK);
  result |= SI7210_registerWrite(SI7210_REG_ADDR_POWER_CTRL, read);

  return result;
}
/**************************************************************************//**
 * @brief
 *    Puts Si7210 into Sleep w/ Measurement Mode: OUTPUT is updated 200msec
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_sleepSltimeena(void)
{
  uint8_t read;
  uint32_t result;

  result = SI7210_registerRead(SI7210_REG_ADDR_CTRL3, &read);
  // Set the sleep timer enable bit
  read = ((read & SI7210_REG_CTRL3_SW_TAMPER_MASK) | SI7210_REG_CTRL3_SLTIMEENA_MASK);
  result |= SI7210_registerWrite(SI7210_REG_ADDR_CTRL3, read);
  result = SI7210_registerRead(SI7210_REG_ADDR_POWER_CTRL, &read);
  // Clear the oneburst, stop and sleep bits
  read = (read & ~(SI7210_REG_POWER_CTRL_ONEBURST_MASK | SI7210_REG_POWER_CTRL_STOP_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK));
  result |= SI7210_registerWrite(SI7210_REG_ADDR_POWER_CTRL, read);

  return result;
}

/**************************************************************************//**
 * @brief
 *    Read out Si7210 Revision and ID
 *
 * @param[out] id
 *    Si7210 part ID
 *
 * @param[out] rev
 *    Si7210 part Revision
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_identify(uint8_t *id, uint8_t *rev)
{
  uint8_t read;
  uint32_t result;

  result = SI7210_wakeUp();
  result |= SI7210_registerRead(SI7210_REG_ADDR_HREVID, &read);
  *rev = read & SI7210_REG_HREVID_REVID_MASK;
  *id = read >> SI7210_REG_HREVID_CHIPID_SHIFT;
  result |= SI7210_registerRead(SI7210_REG_ADDR_POWER_CTRL, &read);
  // Clear the oneburst and sleep bits, set the stop bit
  read = ((read & ~(SI7210_REG_POWER_CTRL_ONEBURST_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK)) | SI7210_REG_POWER_CTRL_STOP_MASK);
  result |= SI7210_registerWrite(SI7210_REG_ADDR_POWER_CTRL, read);

  return result;
}

/**************************************************************************//**
 * @brief
 *    Reads register from the OTP area of the Si7021 device
 *
 * @param[in] otpAddr
 *    The register address to read from in the sensor
 *
 * @param[out] otpData
 *    The data read from the device
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_regReadOTP(uint8_t otpAddr, uint8_t *otpData)
{
  uint32_t status;
  uint8_t reg;

  status = SI7210_registerRead(SI7210_REG_ADDR_OTP_CTRL, &reg);
  if ( status != HALL_OK ) {
    return status;
  }

  if ( reg & SI7210_REG_OTP_CTRL_BUSY_MASK ) {
    return SI7210_ERROR_OTP_BUSY;
  }

  status = SI7210_registerWrite(SI7210_REG_ADDR_OTP_ADDR, otpAddr);
  if ( status != HALL_OK ) {
    return status;
  }

  status = SI7210_registerWrite(SI7210_REG_ADDR_OTP_CTRL, SI7210_REG_OTP_CTRL_READ_EN_MASK);
  if ( status != HALL_OK ) {
    return status;
  }

  status = SI7210_registerRead(SI7210_REG_ADDR_OTP_DATA, otpData);

  return status;
}

/**************************************************************************//**
 * @brief
 *    Loads the coefficients A0..A6 from the OTP memory
 *
 * @param[in] otpAddr
 *    The register address to start reading from the OTP memory
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
static uint32_t SI7210_loadCoeffsFromOtp(uint8_t otpAddr)
{
  int i;
  uint32_t status;
  uint8_t value;

  const uint8_t writeAddr[] = {
    SI7210_REG_ADDR_A0,
    SI7210_REG_ADDR_A1,
    SI7210_REG_ADDR_A2,
    SI7210_REG_ADDR_A3,
    SI7210_REG_ADDR_A4,
    SI7210_REG_ADDR_A5
  };

  for ( i = 0; i < sizeof(writeAddr); i++ ) {
    status = SI7210_regReadOTP(otpAddr++, &value);

    if ( status != SI7210_OK ) {
      return status;
    }

    status = SI7210_registerWrite(writeAddr[i], value);

    if ( status != HALL_OK ) {
      return status;
    }
  }

  return status;
}

/**************************************************************************//**
 * @brief
 *    Change Mag-Field scale to 200mT.
 *    If desired, must be performed after power-up or wake-up from sleep.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_set200mTRange(void)
{
  SI7210_loadCoeffsFromOtp(SI7210_OTP_ADDR_COEFFS_200MT);

  return SI7210_OK;
}

/**************************************************************************//**
 * @brief
 *   Perform burst-conversion(4samples), read mT-data, and then
 *   put part into sltimeena-sleep mode where OUT is updated every 200msec.
 *
 * @param[in] range200mT
 *   range200mT=false : full-scale equals 20mT
 *   range200mT=true  : full-scale equals 200mT
 *
 * @param[out] mTdata
 *   Mag-field conversion reading, signed 32-bit integer
 *   mTdata must be divided by 1000 to get decimal value in mT units
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_readmTDataSltimeena(bool range200mT, int32_t *mTdata)
{
  uint8_t read;
  int16_t data;
  uint32_t result;

  result = SI7210_wakeUp();
  result |= SI7210_registerRead(SI7210_REG_ADDR_POWER_CTRL, &read);
  // Clear oneburst and sleep bits, set Usestore and stop to stop measurements
  //read = ( ( read & 0xF0 ) | 0x0A );
  read = ((read & ~(SI7210_REG_POWER_CTRL_ONEBURST_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK)) | (SI7210_REG_POWER_CTRL_USESTORE_MASK | SI7210_REG_POWER_CTRL_STOP_MASK));
  result |= SI7210_registerWrite(SI7210_REG_ADDR_POWER_CTRL, read);
  // Burst sample size = 4 (2^2), number of samples to average = 4 (2^2)
  result |= SI7210_registerWrite(SI7210_REG_ADDR_CTRL4, ((2 << SI7210_REG_CTRL4_DF_BURSTSIZE_SHIFT) | (2 << SI7210_REG_CTRL4_DF_BW_SHIFT)));
  if ( range200mT ) {
    result |= SI7210_set200mTRange();
  }
  // Clear stop and sleep bits, set Usestore and oneburst to start a burst of measurements
  result |= SI7210_registerRead(SI7210_REG_ADDR_POWER_CTRL, &read);
  read = ((read & ~(SI7210_REG_POWER_CTRL_STOP_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK)) | (SI7210_REG_POWER_CTRL_USESTORE_MASK | SI7210_REG_POWER_CTRL_ONEBURST_MASK));
  result |= SI7210_registerWrite(SI7210_REG_ADDR_POWER_CTRL, read);

  // Wait until the measurement is done
  do {
    result |= SI7210_registerRead(SI7210_REG_ADDR_POWER_CTRL, &read);
  } while ( (read >> SI7210_REG_POWER_CTRL_MEAS_SHIFT) && (result == 0) );

  result |= SI7210_dataRead(&data);

  // To convert mTdata to decimal value, divide by 1000
  if ( range200mT ) {
    *mTdata = (data * 125 / 10);
  } else {
    *mTdata = (data * 125 / 100);
  }

  // Go to sleep with sleep timer enabled
  result |= SI7210_sleepSltimeena();

  return result;
}

/**************************************************************************//**
 * @brief
 *   Wake-up from Sleep, perform burst-conversion(4samples), read mT-data,
 *   and then put part into sleep mode (no-measurement). Requires Wake-Up.
 *
 * @param[in] range200mT
 *   range200mT=false : full-scale equals 20mT
 *   range200mT=true  : full-scale equals 200mT
 *
 * @param[out] mTdata
 *   Mag-field conversion reading, signed 32-bit integer
 *   mTdata must be divided by 1000 to get decimal value in mT units
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_readmTDataSleep(bool range200mT, int32_t *mTdata)
{
  uint8_t read;
  int16_t data;
  uint32_t result;

  result = SI7210_wakeUp();
  result = SI7210_registerRead(SI7210_REG_ADDR_POWER_CTRL, &read);
  // Clear oneburst and sleep bits, set Usestore and stop to stop measurements
  //read = ( ( read & 0xF0 ) | 0x0A );
  read = ((read & ~(SI7210_REG_POWER_CTRL_ONEBURST_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK)) | (SI7210_REG_POWER_CTRL_USESTORE_MASK | SI7210_REG_POWER_CTRL_STOP_MASK));
  result |= SI7210_registerWrite(SI7210_REG_ADDR_POWER_CTRL, read);
  // Burst sample size = 4 (2^2), number of samples to average = 4 (2^2)
  result |= SI7210_registerWrite(SI7210_REG_ADDR_CTRL4, ((2 << SI7210_REG_CTRL4_DF_BURSTSIZE_SHIFT) | (2 << SI7210_REG_CTRL4_DF_BW_SHIFT)));
  if ( range200mT ) {
    result |= SI7210_set200mTRange();
  }
  // Clear stop and sleep bits, set Usestore and oneburst to start a burst of measurements
  result |= SI7210_registerRead(SI7210_REG_ADDR_POWER_CTRL, &read);
  read = ((read & ~(SI7210_REG_POWER_CTRL_STOP_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK)) | (SI7210_REG_POWER_CTRL_USESTORE_MASK | SI7210_REG_POWER_CTRL_ONEBURST_MASK));
  result |= SI7210_registerWrite(SI7210_REG_ADDR_POWER_CTRL, read);

  // Wait until the measurement is done
  do {
    result |= SI7210_registerRead(SI7210_REG_ADDR_POWER_CTRL, &read);
  } while ( (read >> SI7210_REG_POWER_CTRL_MEAS_SHIFT) && (result == 0) );

  result |= SI7210_dataRead(&data);

  // To convert mTdata to decimal value, divide by 1000
  if ( range200mT ) {
    *mTdata = (data * 125 / 10);
  } else {
    *mTdata = (data * 125 / 100);
  }

  // Go to sleep
  result |= SI7210_sleep();

  return result;
}

/**************************************************************************//**
 * @brief
 *   Puts sensor into continuous mode, conversions performed every 7usec
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_enterContMode(void)
{
  uint8_t read;
  uint32_t result;

  result = SI7210_wakeUp();
  result |= SI7210_registerRead(SI7210_REG_ADDR_POWER_CTRL, &read);
  read = ((read & 0xF0) | 0x0A);
  result |= SI7210_registerWrite(SI7210_REG_ADDR_POWER_CTRL, read);
  result |= SI7210_registerWrite(SI7210_REG_ADDR_DSPSIGSEL, 4);
  result |= SI7210_registerWrite(SI7210_REG_ADDR_CTRL1, 0x7F);
  result |= SI7210_registerWrite(SI7210_REG_ADDR_CTRL2, 0x92);
  result |= SI7210_registerWrite(SI7210_REG_ADDR_SLTIME, 0x00);
  result |= SI7210_registerWrite(SI7210_REG_ADDR_CTRL3, 0xFE);
  result |= SI7210_registerRead(SI7210_REG_ADDR_POWER_CTRL, &read);
  read = (read & 0xF8);
  result |= SI7210_registerWrite(SI7210_REG_ADDR_POWER_CTRL, read);

  return result;
}

/**************************************************************************//**
 * @brief
 *    Calculates the sw_op value from the threshold by finding the inverse of
 *    the formula:
 *    threshold = (16 + sw_op[3:0]) * 2^sw_op[6:4]
 *
 * @param[in] threshold
 *    Threshold value
 *
 * @return
 *    The value of the sw_op bitfield
 *****************************************************************************/
uint8_t SI7210_calculateSWOP(float threshold)
{
  int th;
  uint8_t a;
  uint8_t swop;

  th = (int) (threshold / 0.005);

  if ( th == 0 ) {
    /* threshold = 0, when swop = 127 */
    return 127;
  } else if ( th < 16 ) {
    threshold = 16;
  } else if ( threshold > 3840 ) {
    threshold = 3840;
  }

  a = th / 16;
  swop = 0;
  while ( a != 0x01 ) {
    a = a >> 1;   /* Find the most significant 1 of th */
    swop += 0x10;   /* increment sw_op[6:4] */
  }

  /* Add remainder as sw_op[3:0] */
  swop |= th / (1 << (swop >> 4)) - 16;

  return swop;
}

/**************************************************************************//**
 * @brief
 *    Calculates the sw_hyst value from the hysteresis by finding the inverse of
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
uint8_t SI7210_calculateSWHYST(float hysteresis, bool scale200mT)
{
  int hyst;
  uint8_t a;
  uint8_t swhyst;

  hyst = (int) (hysteresis / 0.005 + 0.5);

  if ( scale200mT ) {
    hyst /= 10;
  }

  if ( hyst == 0 ) {
    /* When sw_op = 63 the hysteresis is set to zero */
    return 63;
  } else if ( hyst < 8 ) {
    hyst = 8;
  } else if ( hyst > 1792 ) {
    hyst = 1792;
  }

  a = hyst / 8;
  swhyst = 0;

  while ( a != 0x01 ) {
    a = a >> 1;
    swhyst += 0x08;
  }

  swhyst |= hyst / (1 << (swhyst >> 3)) - 8;

  return swhyst;
}

/**************************************************************************//**
 * @brief
 *    Calculates the sw_tamper value from the tamper threshold by finding the
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
uint8_t SI7210_calculateSWTAMPER(float tamper, bool scale200mT)
{
  int tamp;
  int exp;
  uint8_t a;
  uint8_t swtamp;

  tamp = (int) (tamper / 0.005 + 0.5);

  //if ( scale == 204.7f ) {
  if ( scale200mT ) {
    tamp /= 10;
  }

  if ( tamper == 0 ) {
    /* When sw_tamper = 63 the tamper is set to zero */
    return 63;
  } else if ( tamp < 512 ) {
    tamp = 512;
  } else if ( tamp > 3968 ) {
    tamp = 3968;
  }

  a = tamp / 16;

  exp = 0;
  while ( a != 0x01 ) {
    a = a >> 1;
    exp++;
  }

  swtamp = (exp - 5) << 4;

  swtamp |= tamp / (1 << exp) - 16;

  return swtamp;
}

/**************************************************************************//**
 * @brief
 *    Calculates the slTime value from the sleep time by finding the inverse of
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
uint8_t SI7210_calculateSlTime(uint32_t samplePeriod, uint8_t *slFast)
{
  int a;
  uint8_t slTime;

  if ( samplePeriod == 0 ) {
    *slFast = 1;
    slTime = 0;
    return slTime;
  }

  /* Impose limits */
  if ( samplePeriod < 11 ) {
    samplePeriod = 11;
  } else if ( samplePeriod > 172000 ) {
    samplePeriod = 172000;
  }

  /* Decide on wether we need slFast or not */
  if ( samplePeriod < 750 ) {
    *slFast = 1;
    a = samplePeriod * 12 / 32 / 4;
  } else {
    *slFast = 0;
    a = samplePeriod * 12 / 32 / 256;
  }

  slTime = 0;
  while ( a != 0x01 ) {
    a = a >> 1;
    slTime += 0x20;
  }

  if ( *slFast ) {
    slTime |= samplePeriod * 12 / (4 << (slTime >> 5)) - 32;
  } else {
    slTime |= samplePeriod * 12 / (256 << (slTime >> 5)) - 32;
  }

  return slTime;
}

/**************************************************************************//**
 * @brief
 *    Reads register from the Hall sensor device
 *
 * @param[in] addr
 *    The register address to read from in the sensor
 *
 * @param[out] data
 *    The data read from the device
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_registerRead(uint8_t addr, uint8_t *data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = SI7210_I2C_BUS_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE_READ;

  seq.buf[0].len = 1;
  seq.buf[1].len = 1;
  seq.buf[0].data = &addr;
  seq.buf[1].data = data;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_HALL);

  ret = I2CSPM_Transfer(SI7210_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    return HALL_ERROR_I2C_TRANSACTION_FAILED;
  }

  return HALL_OK;
}

/**************************************************************************//**
 * @brief
 *    Writes a register in the Hall sensor device
 *
 * @param[in] addr
 *    The register address to write
 *
 * @param[in] data
 *    The data to write to the register
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_registerWrite(uint8_t addr, uint8_t data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = SI7210_I2C_BUS_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE_WRITE;

  seq.buf[0].len = 1;
  seq.buf[1].len = 1;
  seq.buf[0].data = &addr;
  seq.buf[1].data = &data;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_HALL);

  ret = I2CSPM_Transfer(SI7210_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    return HALL_ERROR_I2C_TRANSACTION_FAILED;
  }

  return HALL_OK;
}

/**************************************************************************//**
 * @brief
 *    Sets the given bit(s) in a register in the Hall sensor device
 *
 * @param[in] addr
 *    The address of the register
 *
 * @param[in] mask
 *    The mask specifies which bits should be set. If a given bit of the mask is
 *    1 that register bit will be set to 1. All the other register bits will be
 *    untouched.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_regSetBits(uint8_t addr, uint8_t mask)
{
  uint8_t value;
  uint32_t status;

  status = SI7210_registerRead(addr, &value);
  if ( status != HALL_OK ) {
    return status;
  }

  value |= mask;

  status = SI7210_registerWrite(addr, value);

  return status;
}

/**************************************************************************//**
 * @brief
 *    Clears the given bit(s) in a register in the Hall sensor device
 *
 * @param[in] addr
 *    The address of the register
 *
 * @param[in] mask
 *    The mask specifies which bits should be clear. If a given bit of the mask
 *    is 1 that register bit will be cleared to 0. All the other register bits
 *    will be untouched.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI7210_regClearBits(uint8_t addr, uint8_t mask)
{
  uint8_t value;
  uint32_t status;

  status = SI7210_registerRead(addr, &value);
  if ( status != HALL_OK ) {
    return status;
  }

  value &= ~mask;

  status = SI7210_registerWrite(addr, value);

  return status;
}

/**************************************************************************//**
 * @brief
 *    Wakes up the Hall sensor chip
 *
 * @return
 *    None
 *****************************************************************************/
uint32_t SI7210_wakeUp(void)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = SI7210_I2C_BUS_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE;

  seq.buf[0].len = 0;
  seq.buf[0].data = NULL;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_HALL);

  ret = I2CSPM_Transfer(SI7210_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    return HALL_ERROR_I2C_TRANSACTION_FAILED;
  }

  usDelay(10);

  return HALL_OK;
}
/** @} {end defgroup Si7210} */

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/**************************************************************************//**
 * @brief
 *    Microseconds delay routine
 *
 * @param[in] delay
 *    The desired delay time in microseconds
 *
 * @return
 *    None
 *****************************************************************************/
static void usDelay(uint32_t delay)
{
  volatile uint32_t cnt;
#ifdef _SILICON_LABS_32B_SERIES_2
  cnt = CMU_ClockFreqGet(cmuClock_HCLK) * delay / (uint32_t) 1e6;
#else
  cnt = CMU_ClockFreqGet(cmuClock_HF) * delay / (uint32_t) 1e6;
#endif
  cnt /= 7;  /* 7 cycles per loop count */

  while ( cnt-- )
    ;

  return;
}
/** @endcond */

/** @} {end addtogroup HALL} */
/** @} {end addtogroup TBSense_BSP} */
