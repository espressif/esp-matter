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

#include <stddef.h>

#include "sl_i2cspm.h"
#include "sl_udelay.h"
#include "sl_si7210.h"
#include "sl_si7210_regs.h"
#include "sl_sleeptimer.h"

// Local prototypes
static sl_status_t sl_si7210_load_coefficients_from_otp(sl_i2cspm_t *i2cspm, uint8_t otpAddr);

// Si7210 i2s address
#ifndef SI7210_I2C_BUS_ADDRESS
#define SI7210_I2C_BUS_ADDRESS  0x30
#endif

// Si7210 chip ID
#define SI7210_CHIP_ID          0x01

/**************************************************************************//**
 *    Does device-specific initializaiton for the Si7210 chip.
 *****************************************************************************/
sl_status_t sl_si7210_init(sl_i2cspm_t *i2cspm)
{
  sl_status_t status;
  uint8_t  chipId;
  uint8_t  revId;

  /* Wait 1 ms wake-up time */
  sl_sleeptimer_delay_millisecond(1);

  /* Try to contact the sensor and check it's device ID */
  status = sl_si7210_wake_up(i2cspm);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  status = sl_si7210_identify(i2cspm, &chipId, &revId);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  if ( (chipId != SI7210_CHIP_ID) ) {
    status = SL_STATUS_INITIALIZATION;
  }

  return status;
}

/**************************************************************************//**
 *    Configures the Si7210 chip.
 *****************************************************************************/
sl_status_t sl_si7210_configure(sl_i2cspm_t *i2cspm, sl_si7210_configure_t *config)
{
  sl_status_t status;
  uint8_t value;
  uint8_t sw_op, sw_hyst;

  /* Wake up device and/or stop measurements */
  status = sl_si7210_wake_up(i2cspm);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  /* Stop the measurement loop */
  status = sl_si7210_set_register_bits(i2cspm, SI7210_REG_ADDR_POWER_CTRL, SI7210_REG_POWER_CTRL_STOP_MASK);

  if ( status != SL_STATUS_OK ) {
    return status;
  }

  if ( (config->threshold == 0.0)
       && (config->hysteresis == 0.0)
       && (config->polarity == 0)
       && (config->output_invert == 0) ) {
    /* Use default values in the device for all parameters */
    status = sl_si7210_clear_register_bits(i2cspm, SI7210_REG_ADDR_POWER_CTRL,
                                           SI7210_REG_POWER_CTRL_USESTORE_MASK);

    if ( status != SL_STATUS_OK ) {
      return status;
    }
  } else {
    /* Program sw_low4field and sw_op bit fields */
    sw_op = sl_si7210_calculate_sw_op(config->threshold);

    value = 0;

    if ( config->output_invert ) {
      value = (1 << SI7210_REG_CTRL1_SW_LOW4FIELD_SHIFT) & SI7210_REG_CTRL1_SW_LOW4FIELD_MASK;
    }

    value |= (sw_op << SI7210_REG_CTRL1_SW_OP_SHIFT) & SI7210_REG_CTRL1_SW_OP_MASK;

    status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_CTRL1, value);
    if ( status != SL_STATUS_OK ) {
      return status;
    }

    /* Program sw_fieldpolsel and sw_hyst bit fields */
    sw_hyst = sl_si7210_calculate_sw_hyst(config->hysteresis, false);

    value = (config->polarity << SI7210_REG_CTRL2_SW_FIELDPOLSEL_SHIFT) & SI7210_REG_CTRL2_SW_FIELDPOLSEL_MASK;

    value |= (sw_hyst << SI7210_REG_CTRL2_SW_HYST_SHIFT) & SI7210_REG_CTRL2_SW_HYST_MASK;

    status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_CTRL2, value);
    if ( status != SL_STATUS_OK ) {
      return status;
    }

    /* Enable storing of these parameters in sleep mode */
    status = sl_si7210_set_register_bits(i2cspm, SI7210_REG_ADDR_POWER_CTRL,
                                         SI7210_REG_POWER_CTRL_USESTORE_MASK);

    if ( status != SL_STATUS_OK ) {
      return status;
    }
  }

  /* Enable sleep timer and clear stop bit to start operation */
  status = sl_si7210_set_register_bits(i2cspm, SI7210_REG_ADDR_CTRL3,
                                       SI7210_REG_CTRL3_SLTIMEENA_MASK);

  if ( status != SL_STATUS_OK ) {
    return status;
  }

  /* Resume operation  */
  status = sl_si7210_clear_register_bits(i2cspm, SI7210_REG_ADDR_POWER_CTRL,
                                         SI7210_REG_POWER_CTRL_STOP_MASK);

  return status;
}

/**************************************************************************//**
 *    Performs a measurement
 *****************************************************************************/
sl_status_t sl_si7210_measure(sl_i2cspm_t *i2cspm, uint32_t scale, float *result)
{
  int32_t mT;
  uint32_t status;
  bool range200mT;

  if ( scale > 20500 ) {
    range200mT = true;
  } else {
    range200mT = false;
  }

  status = sl_si7210_read_magfield_data_and_sltimeena(i2cspm, range200mT, &mT);

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
float sl_si7210_get_tamper_threshold(void)
{
  return 19.87f;
}

/**************************************************************************//**
 *    Read out Si7210 Conversion Data - 15bits
 *****************************************************************************/
sl_status_t sl_si7210_read_data(sl_i2cspm_t *i2cspm, int16_t *data)
{
  uint8_t read;
  uint8_t flag;
  sl_status_t status;

  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_DSPSIGM, &read);
  if (status != SL_STATUS_OK) {
    return status;
  }

  flag = read >> SI7210_REG_DSPSIGM_FRESH_SHIFT;
  *data = ((uint16_t)(read  & SI7210_REG_DSPSIGM_DSPSIGM_MASK)) << 8;
  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_DSPSIGL, &read);
  if (status != SL_STATUS_OK) {
    return status;
  }

  *data |= read;
  *data = *data - 16384;

  if ( flag == 0 ) {
    status = SL_STATUS_OBJECT_READ;
  }

  return status;
}

/**************************************************************************//**
 *    Puts Si7210 into Sleep (No-measurement) Mode
 *    Wake-up command needs to be issued to become responsive
 *****************************************************************************/
sl_status_t sl_si7210_sleep(sl_i2cspm_t *i2cspm)
{
  sl_status_t status;
  uint8_t read;

  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_CTRL3, &read);
  if (status != SL_STATUS_OK) {
    return status;
  }
  // Clear the sleep timer enable bit
  read = (read & ~SI7210_REG_CTRL3_SLTIMEENA_MASK);
  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_CTRL3, read);
  if (status != SL_STATUS_OK) {
    return status;
  }

  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, &read);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Clear the oneburst and stop bits, set the sleep bit
  read = ((read & ~(SI7210_REG_POWER_CTRL_ONEBURST_MASK | SI7210_REG_POWER_CTRL_STOP_MASK)) | SI7210_REG_POWER_CTRL_SLEEP_MASK);
  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, read);

  return status;
}
/**************************************************************************//**
 *    Puts Si7210 into Sleep w/ Measurement Mode: OUTPUT is updated 200msec
 *****************************************************************************/
sl_status_t sl_si7210_sleep_sltimeena(sl_i2cspm_t *i2cspm)
{
  uint8_t read;
  sl_status_t status;

  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_CTRL3, &read);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Set the sleep timer enable bit
  read = ((read & SI7210_REG_CTRL3_SW_TAMPER_MASK) | SI7210_REG_CTRL3_SLTIMEENA_MASK);
  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_CTRL3, read);
  if (status != SL_STATUS_OK) {
    return status;
  }

  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, &read);
  if (status == SL_STATUS_OK) {
    return status;
  }

  // Clear the oneburst, stop and sleep bits
  read = (read & ~(SI7210_REG_POWER_CTRL_ONEBURST_MASK | SI7210_REG_POWER_CTRL_STOP_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK));
  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, read);

  return status;
}

/**************************************************************************//**
 *    Read out Si7210 Revision and ID
 *****************************************************************************/
sl_status_t sl_si7210_identify(sl_i2cspm_t *i2cspm, uint8_t *id, uint8_t *rev)
{
  uint8_t read;
  sl_status_t status;

  status = sl_si7210_wake_up(i2cspm);
  if (status != SL_STATUS_OK) {
    return status;
  }

  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_HREVID, &read);
  if (status != SL_STATUS_OK) {
    return status;
  }

  *rev = read & SI7210_REG_HREVID_REVID_MASK;
  *id = read >> SI7210_REG_HREVID_CHIPID_SHIFT;
  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, &read);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Clear the oneburst and sleep bits, set the stop bit
  read = ((read & ~(SI7210_REG_POWER_CTRL_ONEBURST_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK)) | SI7210_REG_POWER_CTRL_STOP_MASK);
  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, read);

  return status;
}

/**************************************************************************//**
 *    Reads register from the OTP area of the Si7021 device
 *****************************************************************************/
sl_status_t sl_si7210_read_otp_register(sl_i2cspm_t *i2cspm, uint8_t otpAddr, uint8_t *otpData)
{
  sl_status_t status;
  uint8_t reg;

  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_OTP_CTRL, &reg);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  if ( reg & SI7210_REG_OTP_CTRL_BUSY_MASK ) {
    return SL_STATUS_BUSY;
  }

  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_OTP_ADDR, otpAddr);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_OTP_CTRL, SI7210_REG_OTP_CTRL_READ_EN_MASK);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_OTP_DATA, otpData);

  return status;
}

/**************************************************************************//**
 *    Change Mag-Field scale to 200mT.
 *    If desired, must be performed after power-up or wake-up from sleep.
 *****************************************************************************/
sl_status_t sl_si7210_set_mt_range_200(sl_i2cspm_t *i2cspm)
{
  sl_status_t status;
  status = sl_si7210_load_coefficients_from_otp(i2cspm, SI7210_OTP_ADDR_COEFFS_200MT);

  return status;
}

/**************************************************************************//**
 *   Perform burst-conversion(4samples), read mT-data, and then
 *   put part into sltimeena-sleep mode where OUT is updated every 200msec.
 *****************************************************************************/
sl_status_t sl_si7210_read_magfield_data_and_sltimeena(sl_i2cspm_t *i2cspm, bool range200mT, int32_t *mTdata)
{
  uint8_t read;
  int16_t data;
  sl_status_t status;

  status = sl_si7210_wake_up(i2cspm);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, &read);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  // Clear oneburst and sleep bits, set Usestore and stop to stop measurements
  read = ((read & ~(SI7210_REG_POWER_CTRL_ONEBURST_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK)) | (SI7210_REG_POWER_CTRL_USESTORE_MASK | SI7210_REG_POWER_CTRL_STOP_MASK));
  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, read);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  // Burst sample size = 4 (2^2), number of samples to average = 4 (2^2)
  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_CTRL4, ((2 << SI7210_REG_CTRL4_DF_BURSTSIZE_SHIFT) | (2 << SI7210_REG_CTRL4_DF_BW_SHIFT)));
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  if ( range200mT ) {
    status = sl_si7210_set_mt_range_200(i2cspm);
    if ( status != SL_STATUS_OK ) {
      return status;
    }
  }
  // Clear stop and sleep bits, set Usestore and oneburst to start a burst of measurements
  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, &read);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  read = ((read & ~(SI7210_REG_POWER_CTRL_STOP_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK)) | (SI7210_REG_POWER_CTRL_USESTORE_MASK | SI7210_REG_POWER_CTRL_ONEBURST_MASK));
  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, read);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  // Wait until the measurement is done
  do {
    status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, &read);
  } while ( (read >> SI7210_REG_POWER_CTRL_MEAS_SHIFT) && (status == 0) );
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  status = sl_si7210_read_data(i2cspm, &data);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  // To convert mTdata to decimal value, divide by 1000
  if ( range200mT ) {
    *mTdata = (data * 125 / 10);
  } else {
    *mTdata = (data * 125 / 100);
  }

  // Go to sleep with sleep timer enabled
  status = sl_si7210_sleep_sltimeena(i2cspm);

  return status;
}

/**************************************************************************//**
 *   Wake-up from Sleep, perform burst-conversion(4samples), read mT-data,
 *   and then put part into sleep mode (no-measurement). Requires Wake-Up.
 *****************************************************************************/
sl_status_t sl_si7210_read_magfield_data_and_sleep(sl_i2cspm_t *i2cspm, bool range200mT, int32_t *mTdata)
{
  uint8_t read;
  int16_t data;
  sl_status_t status;

  status = sl_si7210_wake_up(i2cspm);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, &read);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  // Clear oneburst and sleep bits, set Usestore and stop to stop measurements
  read = ((read & ~(SI7210_REG_POWER_CTRL_ONEBURST_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK)) | (SI7210_REG_POWER_CTRL_USESTORE_MASK | SI7210_REG_POWER_CTRL_STOP_MASK));
  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, read);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  // Burst sample size = 4 (2^2), number of samples to average = 4 (2^2)
  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_CTRL4, ((2 << SI7210_REG_CTRL4_DF_BURSTSIZE_SHIFT) | (2 << SI7210_REG_CTRL4_DF_BW_SHIFT)));
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  if ( range200mT ) {
    status = sl_si7210_set_mt_range_200(i2cspm);
    if ( status != SL_STATUS_OK ) {
      return status;
    }
  }
  // Clear stop and sleep bits, set Usestore and oneburst to start a burst of measurements
  status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, &read);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  read = ((read & ~(SI7210_REG_POWER_CTRL_STOP_MASK | SI7210_REG_POWER_CTRL_SLEEP_MASK)) | (SI7210_REG_POWER_CTRL_USESTORE_MASK | SI7210_REG_POWER_CTRL_ONEBURST_MASK));
  status = sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, read);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  // Wait until the measurement is done
  do {
    status = sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, &read);
  } while ( (read >> SI7210_REG_POWER_CTRL_MEAS_SHIFT) && (status == 0) );
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  status = sl_si7210_read_data(i2cspm, &data);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  // To convert mTdata to decimal value, divide by 1000
  if ( range200mT ) {
    *mTdata = (data * 125 / 10);
  } else {
    *mTdata = (data * 125 / 100);
  }

  // Go to sleep
  status = sl_si7210_sleep(i2cspm);

  return status;
}

/**************************************************************************//**
 *   Puts sensor into continuous mode, conversions performed every 7usec
 *****************************************************************************/
sl_status_t sl_si7210_enter_continuous_mode(sl_i2cspm_t *i2cspm)
{
  uint8_t read;
  sl_status_t status;

  status = sl_si7210_wake_up(i2cspm);
  status |= sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, &read);
  read = ((read & 0xF0) | 0x0A);
  status |= sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, read);
  status |= sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_DSPSIGSEL, 4);
  status |= sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_CTRL1, 0x7F);
  status |= sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_CTRL2, 0x92);
  status |= sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_SLTIME, 0x00);
  status |= sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_CTRL3, 0xFE);
  status |= sl_si7210_read_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, &read);
  read = (read & 0xF8);
  status |= sl_si7210_write_register(i2cspm, SI7210_REG_ADDR_POWER_CTRL, read);

  return status;
}

/**************************************************************************//**
 *    Calculates the sw_op value from the threshold by finding the inverse of
 *    the formula:
 *    threshold = (16 + sw_op[3:0]) * 2^sw_op[6:4]
 *****************************************************************************/
uint8_t sl_si7210_calculate_sw_op(float threshold)
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
 *    Calculates the sw_hyst value from the hysteresis by finding the inverse of
 *    the formula:
 *    hysteresis = (8 + sw_hyst[2:0]) * 2^sw_hyst[5:3]
 *****************************************************************************/
uint8_t sl_si7210_calculate_sw_hyst(float hysteresis, bool scale200mT)
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
 *    Calculates the sw_tamper value from the tamper threshold by finding the
 *    inverse of the formula:
 *    tamper = (16 + sw_tamper[3:0]) * 2^(sw_tamper[5:4] + 5)
 *****************************************************************************/
uint8_t sl_si7210_calculate_sw_tamper(float tamper, bool scale200mT)
{
  int tamp;
  int exp;
  uint8_t a;
  uint8_t swtamp;

  tamp = (int) (tamper / 0.005 + 0.5);

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
 *    Calculates the slTime value from the sleep time by finding the inverse of
 *    the formula:
 *    tsleep = (32 + slTime[4:0]) * 2^(8 + slTime[7:5]) / 12 MHz
 *****************************************************************************/
uint8_t sl_si7210_calculate_sltime(uint32_t samplePeriod, uint8_t *slFast)
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
 *    Reads register from the Hall sensor device
 *****************************************************************************/
sl_status_t sl_si7210_read_register(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t *data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = SI7210_I2C_BUS_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE_READ;

  seq.buf[0].len = 1;
  seq.buf[1].len = 1;
  seq.buf[0].data = &addr;
  seq.buf[1].data = data;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    return SL_STATUS_TRANSMIT;
  }

  return SL_STATUS_OK;
}

/**************************************************************************//**
 *    Writes a register in the Hall sensor device
 *****************************************************************************/
sl_status_t sl_si7210_write_register(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = SI7210_I2C_BUS_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE_WRITE;

  seq.buf[0].len = 1;
  seq.buf[1].len = 1;
  seq.buf[0].data = &addr;
  seq.buf[1].data = &data;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    return SL_STATUS_TRANSMIT;
  }

  return SL_STATUS_OK;
}

/**************************************************************************//**
 *    Sets the given bit(s) in a register in the Hall sensor device
 *****************************************************************************/
sl_status_t sl_si7210_set_register_bits(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t mask)
{
  uint8_t value;
  sl_status_t status;

  status = sl_si7210_read_register(i2cspm, addr, &value);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  value |= mask;

  status = sl_si7210_write_register(i2cspm, addr, value);

  return status;
}

/**************************************************************************//**
 *    Clears the given bit(s) in a register in the Hall sensor device
 *****************************************************************************/
sl_status_t sl_si7210_clear_register_bits(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t mask)
{
  uint8_t value;
  sl_status_t status;

  status = sl_si7210_read_register(i2cspm, addr, &value);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  value &= ~mask;

  status = sl_si7210_write_register(i2cspm, addr, value);

  return status;
}

/**************************************************************************//**
 *    Wakes up the Hall sensor chip
 *****************************************************************************/
sl_status_t sl_si7210_wake_up(sl_i2cspm_t *i2cspm)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = SI7210_I2C_BUS_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE;

  seq.buf[0].len = 0;
  seq.buf[0].data = NULL;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    return SL_STATUS_TRANSMIT;
  }

  sl_udelay_wait(10);

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * @brief
 *    Loads the coefficients A0..A6 from the OTP memory
 *
 * @param[in] i2cspm
 *   The I2CSPM instance to use.
 *
 * @param[in] otpAddr
 *    The register address to start reading from the OTP memory
 *
 * @return
 *    @status SL_STATUS_OK Success
 *    @status SL_STATUS_TRANSMIT  I2C transmission error
 *****************************************************************************/
static sl_status_t sl_si7210_load_coefficients_from_otp(sl_i2cspm_t *i2cspm, uint8_t otpAddr)
{
  unsigned int i;
  sl_status_t status;
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
    status = sl_si7210_read_otp_register(i2cspm, otpAddr++, &value);

    if ( status != SL_STATUS_OK ) {
      return status;
    }

    status = sl_si7210_write_register(i2cspm, writeAddr[i], value);

    if ( status != SL_STATUS_OK ) {
      return status;
    }
  }

  return status;
}
