/***************************************************************************//**
 * @file
 * @brief Code for PS board Si7213-EB demo
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
#include "em_timer.h"
#include "em_prs.h"

#include "wheel.h"
#include "graphics.h"
#include "exp_si72xx.h"

#include "ps_si7213.h"

#define LOW_CAL_THRESHOLD                             3720
#define HIGH_CAL_THRESHOLD                            4120
#define TOTAL_NIBBLES                                 8
#define CALIBRATION_NIBBLE                            0
#define DATA_NIBBLE_1                                 2
#define DATA_NIBBLE_2                                 3
#define DATA_NIBBLE_3                                 4

extern volatile bool demoInitiated;
static uint16_t captureTicks;

static void enablePullupOnOutput(void)
{
  GPIO_PinModeSet(SI72XXPS_OUT_PORT, SI72XXPS_OUT_PIN, gpioModeInputPull, 1);
}

static void disablePullupOnOutput(void)
{
  GPIO_PinModeSet(SI72XXPS_OUT_PORT, SI72XXPS_OUT_PIN, gpioModeInput, 1);
}

/**************************************************************************//**
 * @brief TIMER1_IRQHandler
 * Interrupt Service Routine TIMER1 Interrupt Line
 *****************************************************************************/
void TIMER1_IRQHandler(void)
{
  uint16_t intFlags = TIMER_IntGet(TIMER1);

  if (intFlags & TIMER_IF_CC0) {
    TIMER_CaptureGet(TIMER1, 0);
    TIMER_CaptureGet(TIMER1, 0);
    captureTicks = TIMER_CaptureGet(TIMER1, 0);
  }
  TIMER_IntClear(TIMER1, TIMER_IF_CC0);
}

/**************************************************************************//**
 * @brief Setup TIMER1 and Capture Channels for Si7213-SENT Demo
 *****************************************************************************/
static void sentTimerInit(void)
{
  /* Clock frequency = 14MHz */
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_TIMER1, true);
  PRS_SourceSignalSet(1,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN4,
                      prsEdgeOff);

  /* CC channel 0 Measures Period on Falling Edge */
  TIMER_InitCC_TypeDef timerCC0 =
  {
    .eventCtrl  = timerEventEveryEdge,
    .edge       = timerEdgeFalling,
    .prsSel     = timerPRSSELCh1,
    .cufoa      = timerOutputActionNone,
    .cofoa      = timerOutputActionNone,
    .cmoa       = timerOutputActionNone,
    .mode       = timerCCModeCapture,
    .filter     = true,
    .prsInput   = true,
    .coist      = false,
    .outInvert  = false,
  };
  TIMER_InitCC(TIMER1, 0, &timerCC0);
  TIMER_Init_TypeDef timerInit =
  {
    .enable     = true,
    .debugRun   = true,
    .prescale   = timerPrescale1,
    .clkSel     = timerClkSelHFPerClk,
    .fallAction = timerInputActionReloadStart,
    .riseAction = timerInputActionNone,
    .mode       = timerModeUp,
    .dmaClrAct  = false,
    .quadModeX4 = false,
    .oneShot    = false,
    .sync       = false,
  };
  NVIC_ClearPendingIRQ(TIMER1_IRQn);
  NVIC_EnableIRQ(TIMER1_IRQn);
  TIMER_Init(TIMER1, &timerInit);
}

static void sentSetup(void)
{
  EXP_SI72XX_placeSensorsInSleepMode();
  gpioDisablePushButton1();
  enablePullupOnOutput();
  sentTimerInit();
}

/**************************************************************************//**
 * @brief Enable TIMER1 Capture Channel interrupt
 *****************************************************************************/
static void startMeasuringSent(void)
{
  TIMER_IntClear(TIMER1, TIMER_IF_CC0);
  TIMER_IntEnable(TIMER1, TIMER_IF_CC0);
}

/**************************************************************************//**
 * @brief Disable TIMER1 Capture Channel interrupt
 *****************************************************************************/
static void stopMeasuringSent(void)
{
  TIMER_IntDisable(TIMER1, TIMER_IF_CC0);
  TIMER_IntClear(TIMER1, TIMER_IF_CC0);
}

/**************************************************************************//**
 * @brief Convert SENT Nibble Data to Magnetic-Field (uT)
 * @return microTeslas
 *****************************************************************************/
static int32_t convertNibbleToMagneticField(uint16_t nibbleData)
{
  int32_t microTeslas = 0;

  /* Nibble value for zero magnetic field */
  int32_t nibbleZeroField = 2048;

  microTeslas = (nibbleData - nibbleZeroField) * 10;
  return microTeslas;
}

static void waitForNextNibbleSignal(void)
{
  EMU_EnterEM1();
}

static void waitToDetectCalibrationNibble(void)
{
  bool calNibbleDetected = false;
  while (calNibbleDetected == false) {
    waitForNextNibbleSignal();
    if ((captureTicks < HIGH_CAL_THRESHOLD) && (captureTicks > LOW_CAL_THRESHOLD)) {
      calNibbleDetected = true;
    }
  }
}

/**************************************************************************//**
 * @brief Combine the 3 Nibble Data signals
 * @return Nibble signal for magnetic field conversion
 *****************************************************************************/
static uint16_t calculateNibbleSignal(uint16_t calTicks,
                                      uint16_t nibble1,
                                      uint16_t nibble2,
                                      uint16_t nibble3)
{
  uint16_t nibbleSignal;

  nibbleSignal = ((((56 * (nibble1 + (calTicks / 112)) / calTicks) - 12) << 8)
                  | (((56 * (nibble2 + (calTicks / 112)) / calTicks) - 12) << 4)
                  | ((56 * (nibble3 + (calTicks / 112)) / calTicks) - 12));
  return nibbleSignal;
}

/**************************************************************************//**
 * @brief Run PS_Board Si7213 SENT demo
 *****************************************************************************/
void PS_SI7213_runSentDemo(void)
{
  uint8_t samples;

  uint16_t sentNibble[TOTAL_NIBBLES];
  uint8_t nibbleNo;
  uint16_t nibbleSignal;
  uint16_t nibbleSignalAvg = 0;
  int32_t magField;

  if (!demoInitiated) {
    GRAPHICS_PS_SENT(0, 0);
    sentSetup();
    demoInitiated = true;
  }

  for (samples = 0; samples < SAMPLES_TO_AVG; samples++) {
    startMeasuringSent();
    waitToDetectCalibrationNibble();
    sentNibble[CALIBRATION_NIBBLE] = captureTicks;
    for (nibbleNo = 1; nibbleNo < TOTAL_NIBBLES; nibbleNo++) {
      waitForNextNibbleSignal();
      sentNibble[nibbleNo] = captureTicks;
    }
    stopMeasuringSent();

    nibbleSignal = calculateNibbleSignal(sentNibble[CALIBRATION_NIBBLE],
                                         sentNibble[DATA_NIBBLE_1],
                                         sentNibble[DATA_NIBBLE_2],
                                         sentNibble[DATA_NIBBLE_3]);
    nibbleSignalAvg = nibbleSignalAvg + nibbleSignal;
  }

  magField = convertNibbleToMagneticField(nibbleSignalAvg / SAMPLES_TO_AVG);
  GRAPHICS_PS_SENT(magField, (nibbleSignalAvg / SAMPLES_TO_AVG));
}

/**************************************************************************//**
 * @brief Disable TIMER1 and Capture Channels for Si7213-SENT Demo
 *****************************************************************************/
void PS_SI7213_disableSentDemo(void)
{
  disablePullupOnOutput();

  TIMER_Enable(TIMER1, false);
  TIMER_IntDisable(TIMER1, TIMER_IF_CC0 | TIMER_IF_CC1);
  NVIC_DisableIRQ(TIMER1_IRQn);
  CMU_ClockEnable(cmuClock_TIMER1, false);
  CMU_ClockEnable(cmuClock_PRS, false);
}
