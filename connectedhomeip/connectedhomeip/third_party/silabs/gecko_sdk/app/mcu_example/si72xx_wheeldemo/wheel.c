/***************************************************************************//**
 * @file
 * @brief Hall Effect Wheel Demo for SLSTK3400A_EFM32HG
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
#include "em_chip.h"
#include "em_system.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_i2c.h"
#include "sl_i2cspm.h"
#include "sl_sleeptimer.h"
#include "sl_i2cspm_sensor_config.h"
#include "graphics.h"
#include "wheel.h"
#include "exp_si72xx.h"
#include "exp_si72xx_cal.h"
#include "ps_si7210.h"
#include "ps_si7211.h"
#include "ps_si7212.h"
#include "ps_si7213.h"
#include "sl_system_init.h"
#include "sl_simple_button_instances.h"
#include "gpiointerrupt.h"

#define REFRESH_RATE              1000
sl_sleeptimer_timer_handle_t periodicUpdateTimerId;

volatile DemoNo_t demoSelection = menuScreen;
volatile bool pb0Pressed = false;
volatile bool pb1Pressed = false;
volatile bool expCalibrationFlag = false;
volatile bool expOutToggled = false;
volatile bool demoInitiated = false;

/**************************************************************************//**
 * Local prototype declarations
 *****************************************************************************/
static void setupAndStartSleeptimer(void);
//static void i2cSetup(void);
static void gpioSetup (void);
//void GPIO_Unified_IRQ(void);
static void disableDemos(void);
static void exp_pin_callback(uint8_t interrupt_no);
/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  sl_system_init();
  gpioSetup();
  setupAndStartSleeptimer();
  GRAPHICS_SetupGraphics();

  for (;; ) {
    switch (demoSelection) {
      case menuScreen:
        EXP_SI72XX_runMenuScreen();
        break;
      case expAnglePosition:
        EXP_SI72XX_runAngleDemo();
        break;
      case expRevolutionCounter:
        EXP_SI72XX_runRevolutionCounterDemo();
        break;
      case psI2cFieldData:
        PS_SI7210_runMagneticFieldDemo();
        break;
      case psI2cTempData:
        PS_SI7210_runTemperatureDemo();
        break;
      case psSwitchLatch:
        PS_SI7210_runSwitchDemo();
        break;
      case psAnalogOut:
        PS_SI7211_runAnalogDemo();
        break;
      case psPwmOut:
        PS_SI7212_runPwmDemo();
        break;
      case psSentOut:
        PS_SI7213_runSentDemo();
        break;

      default:
        demoSelection = menuScreen;
        break;
    }
    if (!pb1Pressed) {
      EMU_EnterEM2(true);
    }
  }
}

/**************************************************************************//**
 * @brief Configure RTC Sleeptimer for 1 second interrupts
 *****************************************************************************/
static void setupAndStartSleeptimer(void)
{
  /* Use LFXO for rtc used by the sleeptimer */
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

  /* Initialize sleeptimer driver. */
  sl_sleeptimer_init();

  sl_sleeptimer_start_periodic_timer_ms(&periodicUpdateTimerId, REFRESH_RATE, NULL, NULL, 0, 0);
}

/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 *****************************************************************************/
static void gpioSetup(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  /* Enable PRS sense on GPIO */
  GPIO_InputSenseSet((GPIO_INSENSE_PRS | GPIO_INSENSE_INT),
                     _GPIO_INSENSE_RESETVALUE);

  /* Rev2 Wheel-EXP OUTs C_0:(U2:0x31) & C_1:(U1:0x32) */
  GPIO_PinModeSet(SI72XXEXP_PORT, SI72XXEXP_U2_PIN, gpioModeInput, 1);
  GPIO_PinModeSet(SI72XXEXP_PORT, SI72XXEXP_U1_PIN, gpioModeInput, 1);
  GPIO_ExtIntConfig(SI72XXEXP_PORT, SI72XXEXP_U2_PIN, SI72XXEXP_U2_PIN, true, true, false);
  GPIO_ExtIntConfig(SI72XXEXP_PORT, SI72XXEXP_U1_PIN, SI72XXEXP_U1_PIN, true, true, false);

  GPIOINT_CallbackRegister(SI72XXEXP_U1_PIN,
                           (GPIOINT_IrqCallbackPtr_t)exp_pin_callback);
  GPIOINT_CallbackRegister(SI72XXEXP_U2_PIN,
                           (GPIOINT_IrqCallbackPtr_t)exp_pin_callback);

  /* Postage Stamp OUT D_4:(U0:0x30) */
  GPIO_PinModeSet(SI72XXPS_OUT_PORT, SI72XXPS_OUT_PIN, gpioModeInput, 0);
  GPIO_ExtIntConfig(SI72XXPS_OUT_PORT, SI72XXPS_OUT_PIN, SI72XXPS_OUT_PIN, true, true, false);
}

/**************************************************************************//**
 * @brief Push button handler
 *        PB0 Starts selected test
 *        PB1 Cycles through the available tests
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if ((&sl_button_btn0 == handle) && (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)) {
    if (demoSelection == menuScreen) {
      pb0Pressed = true;
    } else {
      if ((demoSelection == expAnglePosition) && (sl_button_get_state(&sl_button_btn1) == SL_SIMPLE_BUTTON_PRESSED)) {
        expCalibrationFlag = true;
      } else {
        demoSelection = menuScreen;
        disableDemos();
      }
    }
  }

  if ((&sl_button_btn1 == handle) && (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)) {
    pb1Pressed = true;
  }
}

static void exp_pin_callback(uint8_t interrupt_no)
{
  if (interrupt_no == SI72XXEXP_U1_PIN || interrupt_no == SI72XXEXP_U2_PIN) {
    if (demoSelection == menuScreen) {
      expOutToggled = true;
    }
  }
}

void gpioEnablePushButton1(void)
{
  sl_button_enable(&sl_button_btn1);
}

void gpioDisablePushButton1(void)
{
  sl_button_disable(&sl_button_btn1);
}

static void disableDemos(void)
{
  EXP_SI72XX_disableRevolutionCounterDemo();
  PS_SI7210_disableSwitchDemo();
  PS_SI7212_disablePwmDemo();
  PS_SI7213_disableSentDemo();
}
