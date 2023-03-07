/***************************************************************************//**
 * @file
 * @brief Code for PS board Si7212-EB demo
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

#include "ps_si7212.h"

extern volatile bool demoInitiated;
static bool overFlow = false;
static uint32_t pwmCaptureData, periodCaptureData;

/**************************************************************************//**
 * @brief Setup TIMER0 and Capture Channels for Si7212-PWM Demo
 *****************************************************************************/
static void pwmTimerInit(void)
{
  /* Clock frequency = 14MHz */
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);
  PRS_SourceSignalSet(1,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN4,
                      prsEdgeOff);
  /* CC channel 0 Measures Period on Rising Edge */
  TIMER_InitCC_TypeDef timerCC0 =
  {
    .eventCtrl  = timerEventEveryEdge,
    .edge       = timerEdgeRising,
    .prsSel     = timerPRSSELCh1,
    .cufoa      = timerOutputActionNone,
    .cofoa      = timerOutputActionNone,
    .cmoa       = timerOutputActionNone,
    .mode       = timerCCModeCapture,
    .filter     = false,
    .prsInput   = true,
    .coist      = false,
    .outInvert  = false,
  };
  /* CC channel 1 Measures PWM(high) on Falling Edge */
  TIMER_InitCC_TypeDef timerCC1 =
  {
    .eventCtrl  = timerEventEveryEdge,
    .edge       = timerEdgeFalling,
    .prsSel     = timerPRSSELCh1,
    .cufoa      = timerOutputActionNone,
    .cofoa      = timerOutputActionNone,
    .cmoa       = timerOutputActionNone,
    .mode       = timerCCModeCapture,
    .filter     = false,
    .prsInput   = true,
    .coist      = false,
    .outInvert  = false,
  };
  TIMER_InitCC(TIMER0, 0, &timerCC0);
  TIMER_InitCC(TIMER0, 1, &timerCC1);
  TIMER_Init_TypeDef timerInit =
  {
    .enable     = true,
    .debugRun   = true,
    .prescale   = timerPrescale1,
    .clkSel     = timerClkSelHFPerClk,
    .fallAction = timerInputActionNone,
    .riseAction = timerInputActionReloadStart,
    .mode       = timerModeUp,
    .dmaClrAct  = false,
    .quadModeX4 = false,
    .oneShot    = false,
    .sync       = false,
  };
  NVIC_ClearPendingIRQ(TIMER0_IRQn);
  NVIC_EnableIRQ(TIMER0_IRQn);
  TIMER_Init(TIMER0, &timerInit);
}

static void pwmSetup(void)
{
  EXP_SI72XX_placeSensorsInSleepMode();
  gpioDisablePushButton1();
  GRAPHICS_PS_PWM(0, 0);
  pwmTimerInit();
}

static void waitForPwmSignal(void)
{
  EMU_EnterEM1();
}

/**************************************************************************//**
 * @brief Enable TIMER0 Capture Channel interrupt
 *****************************************************************************/
static void startMeasuringPwm(void)
{
  uint8_t samples;
  TIMER_IntClear(TIMER0, TIMER_IF_OF | TIMER_IF_CC0);
  TIMER_IntEnable(TIMER0, TIMER_IF_OF | TIMER_IF_CC0);
  for (samples = 0; samples < 3; samples++) {
    waitForPwmSignal();
  }
}

/**************************************************************************//**
 * @brief Disable TIMER0 Capture Channel interrupt
 *****************************************************************************/
static void stopMeasuringPwm(void)
{
  TIMER_IntDisable(TIMER0, TIMER_IF_OF | TIMER_IF_CC0);
  TIMER_IntClear(TIMER0, TIMER_IF_OF | TIMER_IF_CC0);
}

/**************************************************************************//**
 * @brief Convert PWM(high) and period measurements into magnetic-field (uT)
 * @return microTeslas
 *****************************************************************************/
static int32_t convertPwmToMagneticField(uint32_t tHigh, uint32_t tPeriod)
{
  int32_t microTeslas = 0;
  /* Full-scale magnetic field = 20470uT */
  uint16_t uT_FullScale = 20470;

  if (tHigh <= tPeriod) {
    microTeslas = ((uT_FullScale * 2 * tHigh) / tPeriod) - uT_FullScale;
  } else {
    microTeslas = uT_FullScale;
  }
  return microTeslas;
}

static uint32_t calculateDutyCycle(uint32_t pwm, uint32_t period)
{
  return (pwm * 1000 / period);
}

static bool getOutputPinStatus(void)
{
  return GPIO_PinInGet(SI72XXPS_OUT_PORT, SI72XXPS_OUT_PIN);
}

/**************************************************************************
* @brief Run PS_Board Si7212 PWM demo.
**************************************************************************/
void PS_SI7212_runPwmDemo(void)
{
  uint8_t samples;
  uint32_t pwmAvg = 0;
  uint32_t periodAvg = 0;
  uint32_t dutyCycle;
  int32_t magField;
  bool outputPinStatus = false;

  if (!demoInitiated) {
    pwmSetup();
    demoInitiated = true;
  }

  startMeasuringPwm();
  for (samples = 0; samples < SAMPLES_TO_AVG; samples++) {
    waitForPwmSignal();
    pwmAvg = pwmAvg + pwmCaptureData;
    periodAvg = periodAvg + periodCaptureData;
    if (overFlow) {
      break;
    }
  }
  stopMeasuringPwm();

  if (overFlow) {
    outputPinStatus = getOutputPinStatus();
    if (outputPinStatus) {
      dutyCycle = 1000;
      magField = 20470;
    } else {
      dutyCycle = 0;
      magField = -20470;
    }
  } else {
    dutyCycle = calculateDutyCycle(pwmAvg, periodAvg);
    magField = convertPwmToMagneticField((pwmAvg / SAMPLES_TO_AVG),
                                         (periodAvg / SAMPLES_TO_AVG));
  }

  GRAPHICS_PS_PWM(magField, dutyCycle);
}

/**************************************************************************//**
 * @brief TIMER0_IRQHandler
 * Interrupt Service Routine TIMER0 Interrupt Line
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
  uint16_t intFlags = TIMER_IntGet(TIMER0);
  if (intFlags & TIMER_IF_OF) {
    overFlow = true;
  } else if (intFlags & TIMER_IF_CC0) {
    overFlow = false;
    periodCaptureData = TIMER0->CC[0].CCV;
    pwmCaptureData = TIMER0->CC[1].CCV;
  }
  TIMER_IntClear(TIMER0, TIMER_IF_OF | TIMER_IF_CC0);
}

/**************************************************************************//**
 * @brief Disable interrupts from PWM demo
 *****************************************************************************/
void PS_SI7212_disablePwmDemo(void)
{
  TIMER_Enable(TIMER0, false);
  CMU_ClockEnable(cmuClock_TIMER0, false);
  TIMER_IntDisable(TIMER0, TIMER_IF_CC0 | TIMER_IF_CC1 | TIMER_IF_OF);
  NVIC_DisableIRQ(TIMER0_IRQn);
  CMU_ClockEnable(cmuClock_PRS, false);
}
