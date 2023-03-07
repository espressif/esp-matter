/***************************************************************************//**
 * @file
 * @brief Code for Si72xx-EXP calibration
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

#include "em_device.h"
#include "em_msc.h"
#include "em_emu.h"

#include "sl_si72xx.h"

#include "wheel.h"

#include "exp_si72xx_cal.h"

#include <stdio.h>

extern volatile bool pb1Pressed;
void GRAPHICS_ShowCalString (char *strCal);

/* Default Look-up table values */
const int16_t defaultCal[] = {
  -4597, -6334, -8353, -9695, -10759, -10914, -10088, -8665, -7050, -4994, -3605, -2441, -1290, -478, 316, 1181, 2051, 3178, 4591, 6201, 7924, 9291, 10267, 10334, 9560, 8095, 6428, 4832, 3489, 2300, 1377, 544, -142, -900, -1874, -2995,
  4554, 3165, 2006, 1075, 232, -502, -1409, -2423, -3585, -5376, -7065, -8708, -10303, -10970, -10756, -9705, -8388, -6673, -4987, -3463, -2250, -1261, -374, 415, 1334, 2405, 3685, 5171, 6744, 8446, 9674, 10331, 10128, 9143, 7626, 6010
};

static void waitForPb1()
{
  while (pb1Pressed == false) {
    EMU_EnterEM2(true);
  }
  pb1Pressed = false;
}

/**************************************************************************//**
 * @brief  Erase calibration from flash
 *****************************************************************************/
static void Cal_EraseFlash()
{
  uint32_t *addr = (uint32_t *) USERDATA_BASE;
  MSC_Init();
  MSC_ErasePage(addr);
  MSC_Deinit();
}

/**************************************************************************//**
 * @brief  Save single calibration value to flash
 *****************************************************************************/
static void Cal_WriteToFlash(uint32_t value, int device, int index)
{
  uint32_t data[1];
  uint32_t *addr = (uint32_t *) USERDATA_BASE;
  addr = &(addr[index + (36 * device)]);
  data[0] = value;
  MSC_Init();
  MSC_WriteWord(addr, data, sizeof(data));
  MSC_Deinit();
}

/**************************************************************************//**
 * @brief  Runs Calibration Sequence
 * Calibration requires a lookup value for each 10deg increment from
 * 0deg to 350deg
 *****************************************************************************/
void CAL_runAngleCalibration(void)
{
  int i;
  uint32_t i2cError = 0;
  int16_t u1, u2;
  char strCal[20];
  strCal[0] = 0;

  gpioEnablePushButton1();
  pb1Pressed = false;

  for (i = 0; i < 36; i++) {
    sprintf(strCal, " %d Deg ", i * 10);
    GRAPHICS_ShowCalString(strCal);
    waitForPb1();
    if (i == 0) {
      Cal_EraseFlash();
    }
    i2cError |= sl_si72xx_read_magfield_data_and_sleep(I2C0,
                                                       SI7200_ADDR_1,
                                                       SI7210_20MT,
                                                       SI72XX_SLEEP_MODE,
                                                       &u1);
    i2cError |= sl_si72xx_read_magfield_data_and_sleep(I2C0,
                                                       SI7200_ADDR_2,
                                                       SI7210_20MT,
                                                       SI72XX_SLEEP_MODE,
                                                       &u2);
    Cal_WriteToFlash(u1, 0, i);
    Cal_WriteToFlash(u2, 1, i);
  }
  /* Writes specific value at end of table for confirmation */
  if (!i2cError) {
    Cal_WriteToFlash(1996, 1, 36);
  }
  gpioDisablePushButton1();
}

/**************************************************************************//**
 * @brief  Return default or calibration value for Wheel Position
 *****************************************************************************/
int16_t CAL_getValue(int device, int index)
{
  uint32_t *addr = (uint32_t *) USERDATA_BASE;
  if (addr[72] == 1996) {
    return addr[index + (36 * device)];
  }
  return defaultCal[index + (36 * device)];
}

/**************************************************************************//**
 * @brief  Return true if the wheel EXP has been calibrated by user
 *****************************************************************************/
bool CAL_isExpCalibrated(void)
{
  uint32_t *addr = (uint32_t *) USERDATA_BASE;
  if (addr[72] == 1996) {
    return true;
  } else {
    return false;
  }
}
