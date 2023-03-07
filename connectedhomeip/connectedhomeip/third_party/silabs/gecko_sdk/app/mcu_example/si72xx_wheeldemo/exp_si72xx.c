/***************************************************************************//**
 * @file
 * @brief Code for Si72xx-EXP demos
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

#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_pcnt.h"
#include "em_prs.h"
#include "em_timer.h"

#include "sl_si72xx.h"

#include "wheel.h"
#include "graphics.h"
#include "ps_si7210.h"
#include "exp_si72xx_cal.h"

#include "exp_si72xx.h"

#include <stdlib.h>

#define PCNT_TOP                30
#define PCNT_HYSTERESIS         (PCNT_TOP / 2)
#define MENU_DEFAULT            5

extern volatile DemoNo_t demoSelection;
extern volatile bool pb0Pressed;
extern volatile bool pb1Pressed;
extern volatile bool expCalibrationFlag;
extern volatile bool expOutToggled;
extern volatile bool demoInitiated;
static bool detectQuadrant = false;

static int32_t findAngle (int16_t u1, int16_t u2);

/**************************************************************************//**
 * @brief Place Si7210 sensors on EXP board into latch-mode.
 *****************************************************************************/
uint32_t EXP_SI72XX_placeSensorsInLatchMode(void)
{
  uint32_t i2cError;

  i2cError = sl_si72xx_enter_latch_mode(I2C0, SI7200_ADDR_1);
  i2cError |= sl_si72xx_enter_latch_mode(I2C0, SI7200_ADDR_2);

  return i2cError;
}

/**************************************************************************//**
 * @brief Place Si7210 sensors on EXP board into sleep-mode.
 *****************************************************************************/
uint32_t EXP_SI72XX_placeSensorsInSleepMode(void)
{
  uint32_t i2cError;

  i2cError = sl_si72xx_enter_sleep_mode(I2C0, SI7200_ADDR_1, SI72XX_SLEEP_MODE);
  i2cError |= sl_si72xx_enter_sleep_mode(I2C0, SI7200_ADDR_2, SI72XX_SLEEP_MODE);

  return i2cError;
}

/**************************************************************************//**
 * @brief Run angular position demo for EXP board.
 *****************************************************************************/
void EXP_SI72XX_runAngleDemo(void)
{
  uint32_t i2cError;
  int16_t dataU1, dataU2;
  int32_t magFieldU1 = 0, magFieldU2 = 0;
  int32_t wheelAngle = 0;

  if (!demoInitiated) {
    demoInitiated = true;
  }
  if (expCalibrationFlag) {
    CAL_runAngleCalibration();
    expCalibrationFlag = false;
  }

  i2cError = sl_si72xx_read_magfield_data_and_sleep(I2C0,
                                                    SI7200_ADDR_1,
                                                    SI7210_20MT,
                                                    SI72XX_SLEEP_MODE,
                                                    &dataU1);
  i2cError |= sl_si72xx_read_magfield_data_and_sleep(I2C0,
                                                     SI7200_ADDR_2,
                                                     SI7210_20MT,
                                                     SI72XX_SLEEP_MODE,
                                                     &dataU2);
  if (!i2cError) {
    magFieldU1 = sl_si72xx_convert_data_codes_to_magnetic_field(SI7210_20MT, dataU1);
    magFieldU2 = sl_si72xx_convert_data_codes_to_magnetic_field(SI7210_20MT, dataU2);
    wheelAngle = findAngle(dataU1, dataU2);
  }
  GRAPHICS_WheelAngle(i2cError, magFieldU1, magFieldU2, wheelAngle);
}

/**************************************************************************//**
 * @brief Determines angular position of the wheel using
 * linear interpolation with a lookup table.
 * This calculation is done assuming U1 leads U2 by 90deg.
 *****************************************************************************/
static int32_t findAngle(int16_t u1, int16_t u2)
{
  int8_t start, end, i;
  int32_t angle = -1;
  int32_t ratio;
  int32_t aRatio;
  int32_t bRatio;

  int32_t aRatioData1, aRatioData2;
  int32_t bRatioData1, bRatioData2;

  if (abs(u1) < abs(u2)) {
    ratio = (((int32_t)u1) * 1000) / u2;
    if (u2 > 0) {
      start = 26;
      end = 36;
    } else {
      start = 8;
      end = 18;
    }
  } else {
    ratio = (((int32_t)u2) * 1000) / u1;
    if (u1 > 0) {
      start = 17;
      end = 27;
    } else {
      start = -1;
      end = 9;
    }
  }
  for (i = start; i < end; i++) {
    /* Determine lower lookup ratio */
    if (i == -1) {
      aRatioData1 = (int32_t)CAL_getValue(0, 35);
      aRatioData2 = (int32_t)CAL_getValue(1, 35);
    } else {
      aRatioData1 = (int32_t)CAL_getValue(0, i);
      aRatioData2 = (int32_t)CAL_getValue(1, i);
    }
    /* Determine upper lookup ratio */
    if (i == 35) {
      bRatioData1 = (int32_t)CAL_getValue(0, 0);
      bRatioData2 = (int32_t)CAL_getValue(1, 0);
    } else {
      bRatioData1 = (int32_t)CAL_getValue(0, i + 1);
      bRatioData2 = (int32_t)CAL_getValue(1, i + 1);
    }
    /* Calculate lower and upper ratios */
    if ((start == 26) || (start == 8)) {
      aRatio = (aRatioData1 * 1000) / aRatioData2;
      bRatio = (bRatioData1 * 1000) / bRatioData2;
    } else {
      aRatio = (aRatioData2 * 1000) / aRatioData1;
      bRatio = (bRatioData2 * 1000) / bRatioData1;
    }
    /* Check if the sensor ratio is between lower and upper ratios */
    if (((ratio >= aRatio) && (ratio <= bRatio))
        || ((ratio <= aRatio) && (ratio >= bRatio))) {
      /* Calculate angle using linear interpolation */
      angle = i * 10 + ((ratio - aRatio) * ((i + 1) * 10 - i * 10)) / (bRatio - aRatio);
    }
  }
  if (angle >= 0) {
    return angle;
  }

  if (abs(u1) >= abs(u2)) {
    ratio = (((int32_t)u1) * 1000) / u2;
    if (u2 > 0) {
      start = 26;
      end = 36;
    } else {
      start = 8;
      end = 18;
    }
  } else {
    ratio = (((int32_t)u2) * 1000) / u1;
    if (u1 > 0) {
      start = 17;
      end = 27;
    } else {
      start = -1;
      end = 9;
    }
  }
  for (i = start; i < end; i++) {
    /* Determine lower lookup ratio */
    if (i == -1) {
      aRatioData1 = (int32_t)CAL_getValue(0, 35);
      aRatioData2 = (int32_t)CAL_getValue(1, 35);
    } else {
      aRatioData1 = (int32_t)CAL_getValue(0, i);
      aRatioData2 = (int32_t)CAL_getValue(1, i);
    }
    /* Determine upper lookup ratio */
    if (i == 35) {
      bRatioData1 = (int32_t)CAL_getValue(0, 0);
      bRatioData2 = (int32_t)CAL_getValue(1, 0);
    } else {
      bRatioData1 = (int32_t)CAL_getValue(0, i + 1);
      bRatioData2 = (int32_t)CAL_getValue(1, i + 1);
    }
    /* Calculate lower and upper ratios */
    if ((start == 26) || (start == 8)) {
      aRatio = (aRatioData1 * 1000) / aRatioData2;
      bRatio = (bRatioData1 * 1000) / bRatioData2;
    } else {
      aRatio = (aRatioData2 * 1000) / aRatioData1;
      bRatio = (bRatioData2 * 1000) / bRatioData1;
    }
    /* Check if the sensor ratio is between lower and upper ratios */
    if (((ratio <= aRatio) && (ratio >= bRatio))
        || ((ratio >= aRatio) && (ratio <= bRatio))) {
      /* Calculate angle using linear interpolation */
      angle = i * 10 + ((ratio - aRatio) * ((i + 1) * 10 - i * 10)) / (bRatio - aRatio);
    }
  }
  if (angle >= 0) {
    return angle;
  } else {
    return -10;
  }
}

/**************************************************************************//**
 * @brief Determines the Quadrature Positioning of wheel
 *****************************************************************************/
static uint8_t getQuadrantLocation(void)
{
  uint8_t quad, out1, out2  = 0;
  out2 = GPIO_PinInGet(SI72XXEXP_PORT, SI72XXEXP_U2_PIN);
  out1 = GPIO_PinInGet(SI72XXEXP_PORT, SI72XXEXP_U1_PIN);
  if ((out2 == 0) && (out1 == 1)) {
    quad = 1;
  }
  if ((out2 == 1) && (out1 == 1)) {
    quad = 2;
  }
  if ((out2 == 1) && (out1 == 0)) {
    quad = 3;
  }
  if ((out2 == 0) && (out1 == 0)) {
    quad = 4;
  }
  return quad;
}

/**************************************************************************//**
 * @brief Returns revolution count minus the hysteresis (top/2) so that
 * counter clockwise revolutions result in a negative count.
 *****************************************************************************/
static int8_t getRevolutionCount(void)
{
  return (PCNT_CounterGet(PCNT0) - PCNT_HYSTERESIS);
}

/* Pulse counter requires clock pulses due to asynchronous mode */
static void syncClockAndInitializeCounter(void)
{
  uint8_t prsChn2 = 2;
  uint8_t prsChn3 = 3;
  uint8_t i;

  for (i = 0; i < 4; i++) {
    PRS_PulseTrigger(1 << prsChn3);
  }
  /* Manually triggers PRS to set counter equal to hysteresis */
  while (getRevolutionCount() != 0) {
    PRS_LevelSet(0x0, (1 << prsChn2) | (1 << prsChn3));
    PRS_LevelSet((1 << prsChn3), (1 << prsChn2) | (1 << prsChn3));
    PRS_LevelSet((1 << prsChn3) | (1 << prsChn2), (1 << prsChn2) | (1 << prsChn3));
    PRS_LevelSet((1 << prsChn2), (1 << prsChn2) | (1 << prsChn3));
  }
}

/**************************************************************************//**
 * @brief Initialize pulse counter for Revolution Demo
 *****************************************************************************/
static void pcntSetup(void)
{
  CMU_ClockEnable(cmuClock_PCNT0, true);
  CMU_ClockEnable(cmuClock_PRS, true);
  PCNT_Init_TypeDef pcntInit =
  {
    .mode        = pcntModeExtQuad,
    .counter     = 0,
    .top         = PCNT_TOP,
    .hyst        = true,
    .negEdge     = true,
    .countDown   = true,
    .filter      = true,
    .s1CntDir    = false,
    .cntEvent    = pcntCntEventBoth,
    .auxCntEvent = pcntCntEventNone,
    .s0PRS       = pcntPRSCh2,
    .s1PRS       = pcntPRSCh3,
  };
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS1, true);
  PCNT_Init(PCNT0, &pcntInit);
  syncClockAndInitializeCounter();
  PRS_SourceAsyncSignalSet(2,
                           PRS_CH_CTRL_SOURCESEL_GPIOL,
                           PRS_CH_CTRL_SIGSEL_GPIOPIN0);
  PRS_SourceAsyncSignalSet(3,
                           PRS_CH_CTRL_SOURCESEL_GPIOL,
                           PRS_CH_CTRL_SIGSEL_GPIOPIN1);
}

static void disableExpOutputInterrupts(void)
{
  GPIO_ExtIntConfig(SI72XXEXP_PORT, SI72XXEXP_U2_PIN, SI72XXEXP_U2_PIN, true, true, false);
  GPIO_ExtIntConfig(SI72XXEXP_PORT, SI72XXEXP_U1_PIN, SI72XXEXP_U1_PIN, true, false, false);
}

/**************************************************************************//**
 * @brief Disable PCNT for EXP Revolution Counter Demo
 *****************************************************************************/
void EXP_SI72XX_disableRevolutionCounterDemo(void)
{
  CMU_ClockEnable(cmuClock_PCNT0, false);
  CMU_ClockEnable(cmuClock_PRS, false);
  disableExpOutputInterrupts();
}

/**************************************************************************//**
 * @brief IRQ Handler for pulse counter PCNT0
 *****************************************************************************/
void PCNT0_IRQHandler(void)
{
  uint32_t interruptMask = PCNT_IntGet(PCNT0);
  PCNT_IntClear(PCNT0, interruptMask);
}

static void enableQuadrantDetection(void)
{
  detectQuadrant = true;
  GPIO_ExtIntConfig(SI72XXEXP_PORT, SI72XXEXP_U2_PIN, SI72XXEXP_U2_PIN, true, true, true);
  GPIO_ExtIntConfig(SI72XXEXP_PORT, SI72XXEXP_U1_PIN, SI72XXEXP_U1_PIN, true, true, true);
}

static void enableRevolutionDetection(void)
{
  detectQuadrant = false;
  GPIO_ExtIntConfig(SI72XXEXP_PORT, SI72XXEXP_U2_PIN, SI72XXEXP_U2_PIN, true, true, false);
  GPIO_ExtIntConfig(SI72XXEXP_PORT, SI72XXEXP_U1_PIN, SI72XXEXP_U1_PIN, true, false, true);
}

static void changeQuadrantDetection(void)
{
  if (!detectQuadrant) {
    enableQuadrantDetection();
  } else if (detectQuadrant) {
    enableRevolutionDetection();
  }
}

static uint32_t setupRevolutionCounterDemo(void)
{
  uint32_t i2cError;
  i2cError = EXP_SI72XX_placeSensorsInLatchMode();
  enableRevolutionDetection();
  pcntSetup();
  return i2cError;
}

/**************************************************************************//**
 * @brief Run revolution counter demo for EXP board.
 *****************************************************************************/
void EXP_SI72XX_runRevolutionCounterDemo(void)
{
  uint32_t i2cError = 0;
  uint8_t quadrantNo = 0;
  int8_t revolutionCount;
  if (!demoInitiated) {
    i2cError = setupRevolutionCounterDemo();
    demoInitiated = true;
  }

  if (pb1Pressed) {
    changeQuadrantDetection();
    pb1Pressed = false;
  }

  if (detectQuadrant) {
    quadrantNo = getQuadrantLocation();
  }
  revolutionCount = getRevolutionCount();

  i2cError = EXP_SI72XX_placeSensorsInLatchMode();
  GRAPHICS_RevCounter(i2cError, revolutionCount, quadrantNo);
}

/**************************************************************************//**
 * @brief Setup timer for Menu selection screen
 *****************************************************************************/
static void menuTimerInit(void)
{
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
  CMU_HFRCOBandSet(cmuHFRCOBand_14MHz);
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);

  PRS_SourceSignalSet(2,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN0,
                      prsEdgeOff);
  PRS_SourceSignalSet(3,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN1,
                      prsEdgeOff);

  /* Select CC channel parameters */
  TIMER_InitCC_TypeDef timerCCInit =
  {
    .eventCtrl  = timerEventEveryEdge,
    .edge       = timerEdgeBoth,
    .prsSel     = timerPRSSELCh3,
    .cufoa      = timerOutputActionNone,
    .cofoa      = timerOutputActionNone,
    .cmoa       = timerOutputActionNone,
    .mode       = timerCCModeOff,
    .filter     = false,
    .prsInput   = true,
    .coist      = false,
    .outInvert  = false,
  };
  TIMER_InitCC(TIMER0, 0, &timerCCInit);
  /* Change PRS channel to 2 */
  timerCCInit.prsSel = timerPRSSELCh2;
  TIMER_InitCC(TIMER0, 1, &timerCCInit);

  /* Select TIMER0 parameters */
  TIMER_Init_TypeDef timerInit =
  {
    .enable     = true,
    .debugRun   = true,
    .prescale   = timerPrescale1,
    .clkSel     = timerClkSelHFPerClk,
    .fallAction = timerInputActionNone,
    .riseAction = timerInputActionNone,
    .mode       = timerModeQDec,
    .dmaClrAct  = false,
    .quadModeX4 = true,
    .oneShot    = false,
    .sync       = false,
  };
  /* Configure TIMER */
  TIMER_Init(TIMER0, &timerInit);
}

static void resetTimerCount(void)
{
  TIMER0->CNT = MENU_DEFAULT; /* Reset counter */
}

static uint32_t menuSetup(void)
{
  uint32_t i2cError = EXP_SI72XX_placeSensorsInLatchMode();
  gpioEnablePushButton1();
  menuTimerInit();
  resetTimerCount();
  enableQuadrantDetection(); /* Enable GPIO interrupts for EXP OUTs */
  return i2cError;
}

static bool determineIfExpWheelScrolledUp(void)
{
  /* Get the current counter value */
  uint32_t timerCurrentCount = TIMER_CounterGet(TIMER0);
  resetTimerCount();
  if (timerCurrentCount < MENU_DEFAULT) {
    return true;
  } else {
    return false;
  }
}

static DemoNo_t changeMenuSelection(DemoNo_t currentMenuOption)
{
  DemoNo_t nextMenuOption;
  bool scrollUp = determineIfExpWheelScrolledUp();

  switch (currentMenuOption) {
    case expAnglePosition:
      if (scrollUp) {
        nextMenuOption = psSentOut;
      } else {
        nextMenuOption = expRevolutionCounter;
      }
      break;
    case expRevolutionCounter:
      if (scrollUp) {
        nextMenuOption = expAnglePosition;
      } else {
        nextMenuOption = psI2cFieldData;
      }
      break;
    case psI2cFieldData:
      if (scrollUp) {
        nextMenuOption = expRevolutionCounter;
      } else {
        nextMenuOption = psI2cTempData;
      }
      break;
    case psI2cTempData:
      if (scrollUp) {
        nextMenuOption = psI2cFieldData;
      } else {
        nextMenuOption = psSwitchLatch;
      }
      break;
    case psSwitchLatch:
      if (scrollUp) {
        nextMenuOption = psI2cTempData;
      } else {
        nextMenuOption = psAnalogOut;
      }
      break;
    case psAnalogOut:
      if (scrollUp) {
        nextMenuOption = psSwitchLatch;
      } else {
        nextMenuOption = psPwmOut;
      }
      break;
    case psPwmOut:
      if (scrollUp) {
        nextMenuOption = psAnalogOut;
      } else {
        nextMenuOption = psSentOut;
      }
      break;
    case psSentOut:
      if (scrollUp) {
        nextMenuOption = psPwmOut;
      } else {
        nextMenuOption = expAnglePosition;
      }
      break;

    default:
      nextMenuOption = expAnglePosition;
      break;
  }

  return nextMenuOption;
}

static void waitForUser(void)
{
  EMU_EnterEM1();
}

static void disableMenuScreen(void)
{
  TIMER_Enable(TIMER0, false);
  CMU_ClockEnable(cmuClock_TIMER0, false);
  CMU_ClockEnable(cmuClock_PRS, false);
  disableExpOutputInterrupts();
  EXP_SI72XX_placeSensorsInSleepMode();
  demoInitiated = false;
}

/**************************************************************************//**
 * @brief Run menu screen for demo selection
 *****************************************************************************/
void EXP_SI72XX_runMenuScreen(void)
{
  uint32_t i2cError;
  DemoNo_t nextMenuSelection;
  DemoNo_t menuScreenSelection = expAnglePosition;

  menuSetup();

  while (demoSelection == menuScreen) {
    if (pb0Pressed) {
      demoSelection = menuScreenSelection;
      disableMenuScreen();
      pb0Pressed = false;
      break;
    }
    if ((pb1Pressed) || (expOutToggled)) {
      nextMenuSelection = changeMenuSelection(menuScreenSelection);
      menuScreenSelection = nextMenuSelection;
      pb1Pressed = false;
      expOutToggled = false;
    }

    i2cError = EXP_SI72XX_placeSensorsInLatchMode();
    GRAPHICS_Demo_Menu(i2cError, menuScreenSelection);
    if (!(pb1Pressed) && !(expOutToggled)) {
      waitForUser();
    }
  }
}
