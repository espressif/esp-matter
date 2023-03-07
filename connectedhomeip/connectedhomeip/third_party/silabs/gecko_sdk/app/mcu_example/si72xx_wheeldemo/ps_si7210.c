/***************************************************************************//**
 * @file
 * @brief Code for PS board Si7210-EB demos
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_gpio.h"

#include "sl_si72xx.h"

#include "wheel.h"
#include "graphics.h"
#include "exp_si72xx.h"

#include "ps_si7210.h"

#include <stdlib.h>

/* Tamper threshold for Switch Demo (75% of default threshold 15872) */
#define SI72XX_TAMPER_THRESHOLD                11904

/** variants of Si72xx PS_Boards */
typedef enum {
  PS_SI7210,
  PS_SI7201_02
} PsBoardVariant_t;

extern volatile bool pb1Pressed;
extern volatile bool demoInitiated;
static Si72xxFieldScale_t mTScale;
static int16_t tempOffsetValue, tempGainValue;

static void changeMagFieldScale(void)
{
  if (mTScale == SI7210_20MT) {
    mTScale = SI7210_200MT;
  } else if (mTScale == SI7210_200MT) {
    mTScale = SI7210_20MT;
  }
}

/**************************************************************************//**
 * @brief  Run PS_Board Si7210 Demo for Magnetic Field Readings
 *****************************************************************************/
void PS_SI7210_runMagneticFieldDemo(void)
{
  uint32_t i2cError;
  int16_t magData;
  int32_t magField;

  if (!demoInitiated) {
    EXP_SI72XX_placeSensorsInSleepMode();
    mTScale = SI7210_20MT;
    demoInitiated = true;
  }

  if (pb1Pressed) {
    changeMagFieldScale();
    pb1Pressed = false;
  }

  i2cError = sl_si72xx_read_magfield_data_and_sleep(I2C0,
                                                    SI7200_ADDR_0,
                                                    mTScale,
                                                    SI72XX_SLEEP_MODE,
                                                    &magData);

  magField = sl_si72xx_convert_data_codes_to_magnetic_field(mTScale, magData);

  GRAPHICS_PS_I2C(i2cError, mTScale, magField);
}

/**************************************************************************//**
 * @brief  Run PS_Board Si7210 Demo for Temperature Readings
 *****************************************************************************/
void PS_SI7210_runTemperatureDemo(void)
{
  uint8_t j;
  uint32_t i2cError;
  int32_t mCTemp;
  int32_t tempData;

  if (!demoInitiated) {
    EXP_SI72XX_placeSensorsInSleepMode();
    gpioDisablePushButton1();
    sl_si72xx_read_temp_correction_data_and_sleep(I2C0,
                                                  SI7200_ADDR_0,
                                                  &tempOffsetValue,
                                                  &tempGainValue);
    demoInitiated = true;
  }

  i2cError = sl_si72xx_read_corrected_temp_and_sleep(I2C0,
                                                     SI7200_ADDR_0,
                                                     tempOffsetValue,
                                                     tempGainValue,
                                                     &mCTemp);

  if (!i2cError) {
    tempData = mCTemp;
    for (j = 1; j < SAMPLES_TO_AVG; j++) {
      i2cError = sl_si72xx_read_temperature_and_sleep(I2C0, SI7200_ADDR_0, &mCTemp);
      tempData = tempData + mCTemp;
    }
    mCTemp = tempData / SAMPLES_TO_AVG;
  }

  GRAPHICS_PS_Temp(i2cError, mCTemp);
}

static void placeSi7210InSwitchMode(void)
{
  sl_si72xx_enter_sleep_mode(I2C0, SI7200_ADDR_0, SI72XX_SLTIMEENA_MODE);
}

static void enableOutputInterrupt(void)
{
  GPIO_IntEnable(1 << SI72XXPS_OUT_PIN);
}

static void disableOutputInterrupt(void)
{
  GPIO_IntDisable(1 << SI72XXPS_OUT_PIN);
}

static void setupSwitchDemo(void)
{
  placeSi7210InSwitchMode();
  enableOutputInterrupt();
}

static bool getOutputPinStatus(void)
{
  return GPIO_PinInGet(SI72XXPS_OUT_PORT, SI72XXPS_OUT_PIN);
}

static bool checkTamperStatus(int16_t magFieldData)
{
  bool tamperStatus;
  if (abs(magFieldData) > SI72XX_TAMPER_THRESHOLD) {
    tamperStatus = true;
  } else {
    tamperStatus = false;
  }
  return tamperStatus;
}

static PsBoardVariant_t identifyPsBoardAndMeasureMagneticField(int16_t *magFieldData)
{
  PsBoardVariant_t psBoard;
  uint32_t i2cError;
  i2cError = sl_si72xx_read_magfield_data_and_sleep(I2C0,
                                                    SI7200_ADDR_0,
                                                    SI7210_20MT,
                                                    SI72XX_SLTIMEENA_MODE,
                                                    &*magFieldData);
  if (i2cError) {
    psBoard = PS_SI7201_02;
  } else {
    psBoard = PS_SI7210;
  }
  return psBoard;
}

/**************************************************************************//**
 * @brief  Run PS_Board Si7210 Demo for Switch/Latch operation.
 *****************************************************************************/
void PS_SI7210_runSwitchDemo(void)
{
  bool pinState, tamperState = false;
  PsBoardVariant_t psBoard;
  int16_t psDataOutput;

  if (!demoInitiated) {
    EXP_SI72XX_placeSensorsInSleepMode();
    gpioDisablePushButton1();
    setupSwitchDemo();
    demoInitiated = true;
  }

  pinState = getOutputPinStatus();
  psBoard = identifyPsBoardAndMeasureMagneticField(&psDataOutput);
  if (!pinState) {
    tamperState = checkTamperStatus(psDataOutput);
  }

  GRAPHICS_PS_Switch(psBoard, pinState, tamperState);
}

static void putSi7210ToSleep(void)
{
  sl_si72xx_enter_sleep_mode(I2C0, SI7200_ADDR_0, SI72XX_SLEEP_MODE);
}

void PS_SI7210_disableSwitchDemo(void)
{
  disableOutputInterrupt();
  putSi7210ToSleep();
}
