/***************************************************************************//**
 * @file
 * @brief Demo example for SLSTK3701A
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

#include <stdint.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_cryotimer.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_prs.h"

#include "dmd.h"
#include "glib.h"

#include "app_bumode.h"
#include "app_csen.h"
#include "app_rgbled.h"
#include "app_rtcc.h"
#include "app_screens.h"
#include "app_sensor.h"

#include "sl_system_init.h"

// The GLIB context
static GLIB_Context_t glibContext;

// Global state variables
static uint8_t currentApp = 0;
static int32_t touchHandle = 0;
static bool forceTriggered = false, swipeLock = false, cryoPeriod = false, lcTriggered = false;

#define BU_BUFFER_SIZE 90

// App-specific variables
static int16_t appLedStartValue = 0;
RGBLED_Settings_t appLedSettings = RGBLED_SETTINGS_DEFAULT;
int16_t appHallField = 0;
bool appRhtCelsius = true;
int32_t appRhtTempData = 0;
uint32_t appRhtRhData = 0;
CSEN_Event_t appCsenData = CSEN_EVENT_DEFAULT;
uint16_t appBuVddMeasBuffer[BU_BUFFER_SIZE];
uint8_t appBuBufferIndex = 0;
bool appBuChargeEn = false, appBuDisCharge = false;
uint16_t appBuAvddVoltage = 315;
uint32_t appBuRtccResetVal = 0, appBuRtccResetTs = 0, appBuBodCnt = 0, appBuBuTime = 0, appBuBodTime = 0;
uint16_t appLcCounter = 0;
uint32_t appLcRng[4];

#define APP_COUNT 7

#define RIGHT_BUTTON_LIMIT 135
#define LEFT_BUTTON_LIMIT 70

/**************************************************************************//**
 * @brief Setup CRYOTIMER to generate an interrupt every second.
 *****************************************************************************/
static void cryotimerSetup(void)
{
  static CRYOTIMER_Init_TypeDef cryotimerInit = CRYOTIMER_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_CRYOTIMER, true);

  // Use LFXO as cryotimer clock source
  cryotimerInit.osc = cryotimerOscLFXO;

  // Setup CRYOTIMER period to 1 second
  cryotimerInit.presc = cryotimerPresc_128;
  cryotimerInit.period = cryotimerPeriod_256;

  // Enable CRYOTIMER period interrupt
  CRYOTIMER_IntClear(0xFFFFFFFFUL);
  CRYOTIMER_IntEnable(CRYOTIMER_IEN_PERIOD);
  NVIC_EnableIRQ(CRYOTIMER_IRQn);

  // Initialize and enable CRYOTIMER
  CRYOTIMER_Init(&cryotimerInit);

  /* Disable PRS control of display EXTCOM pin since we want to use the RTCC for timekeeping
     and let the cryotimer handle the EXTCOM toggling */
  PRS_SourceAsyncSignalSet(11, 0, 0);
  PRS->ROUTEPEN &= ~PRS_ROUTEPEN_CH11PEN;
}

void drawScreen(uint8_t app, int8_t xOffset)
{
  switch (app) {
    case 1:
      drawLedScreen(&glibContext, xOffset, swipeLock, forceTriggered, appLedSettings);
      break;

    case 2:
      drawHallSensorScreen(&glibContext, xOffset, appHallField);
      break;

    case 3:
      drawHumTempScreen(&glibContext, xOffset, appRhtCelsius, appRhtTempData, appRhtRhData, forceTriggered);
      break;

    case 4:
      drawForceTouchScreen(&glibContext, xOffset, swipeLock, appCsenData);
      break;

    case 5:
      drawLCSenseScreen(&glibContext, xOffset, forceTriggered, appLcCounter, appLcRng);
      break;

    case 6:
      drawBuModeScreen(&glibContext, xOffset, appBuChargeEn, appBuDisCharge, appBuVddMeasBuffer, appBuBufferIndex, appBuAvddVoltage, appBuBodCnt, appBuBodTime, appBuBuTime);
      break;

    default:
      drawInitScreen(&glibContext);
      break;
  }
}

void forceTouchHandler(uint8_t sliderPos)
{
  switch (currentApp) {
    case 1:
      if (sliderPos > RIGHT_BUTTON_LIMIT) {
        if (!swipeLock) {
          swipeLock = true;
        } else {
          appLedSettings.curSetting = (appLedSettings.curSetting + 1) % 4;
        }
      }
      if (sliderPos < LEFT_BUTTON_LIMIT) {
        swipeLock = false;
      }
      break;

    case 3:
      if (sliderPos > RIGHT_BUTTON_LIMIT) {
        if (appRhtCelsius) {
          appRhtCelsius = false;
        } else {
          appRhtCelsius = true;
        }
      }
      break;

    case 4:
      if (sliderPos > RIGHT_BUTTON_LIMIT) {
        swipeLock = true;
      }
      if (sliderPos < LEFT_BUTTON_LIMIT) {
        swipeLock = false;
      }
      break;

    case 5:
      if (sliderPos > RIGHT_BUTTON_LIMIT) {
        appLcCounter = 0;
      }
      break;

    case 6:
      if (sliderPos > RIGHT_BUTTON_LIMIT) {
        if (appBuChargeEn == false) {
          appBuDisCharge = false;
          appBuChargeEn = true;
        } else {
          appBuChargeEn = false;
        }
      }
      if (sliderPos < LEFT_BUTTON_LIMIT) {
        if (appBuDisCharge == false) {
          appBuDisCharge = true;
          appBuChargeEn = false;
        } else {
          appBuDisCharge = false;
        }
      }
      buPwrSwitch(appBuChargeEn, appBuDisCharge);
      break;

    default:
      break;
  }
}

void swipeHandler(void)
{
  switch (currentApp) {
    case 1:
      if (appLedSettings.curSetting == 0) {
        if (appLedStartValue == 0) {
          appLedStartValue = appLedSettings.led0Color;
        }
        appLedSettings.led0Color = appLedStartValue + touchHandle;
        if (appLedSettings.led0Color > 359) {
          appLedSettings.led0Color = 359;
        }
        if (appLedSettings.led0Color < 0) {
          appLedSettings.led0Color = 0;
        }
      } else if (appLedSettings.curSetting == 1) {
        if (appLedStartValue == 0) {
          appLedStartValue = appLedSettings.led0Power;
        }
        if (abs(touchHandle) > 20) {
          appLedSettings.led0Power = appLedStartValue + touchHandle / 20;
          if (appLedSettings.led0Power > 12) {
            appLedSettings.led0Power = 12;
          }
          if (appLedSettings.led0Power < 0) {
            appLedSettings.led0Power = 0;
          }
        }
      } else if (appLedSettings.curSetting == 2) {
        if (appLedStartValue == 0) {
          appLedStartValue = appLedSettings.led1Color;
        }
        appLedSettings.led1Color = appLedStartValue + touchHandle;
        if (appLedSettings.led1Color > 359) {
          appLedSettings.led1Color = 359;
        }
        if (appLedSettings.led1Color < 0) {
          appLedSettings.led1Color = 0;
        }
      } else if (appLedSettings.curSetting == 3) {
        if (appLedStartValue == 0) {
          appLedStartValue = appLedSettings.led1Power;
        }
        if (abs(touchHandle) > 20) {
          appLedSettings.led1Power = appLedStartValue + touchHandle / 20;
          touchHandle %= 20;
          if (appLedSettings.led1Power > 12) {
            appLedSettings.led1Power = 12;
          }
          if (appLedSettings.led1Power < 0) {
            appLedSettings.led1Power = 0;
          }
        }
      }
      buSaveLedSettings(&appLedSettings);
      break;

    default:
      break;
  }
}

/**************************************************************************//**
 * @brief  Main function of EFM32GG11 demo example.
 *****************************************************************************/
int main(void)
{
  EMSTATUS status;
  int16_t screenXOffset = 0;
  uint8_t nextApp = 1;
  int32_t lastCsenSliderPos = -1;

  sl_system_init();

  /* Setup backup mode and restore values stored in retention registers if
     we are waking up from a backup event */
  buSetup(&appBuRtccResetVal, &appBuRtccResetTs, &appBuBodCnt, &appBuBuTime, &appBuBodTime, &currentApp);
  buLoadLedSettings(&appLedSettings);

  // Initialize the DMD module for the DISPLAY device driver
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (1) {
    }
  }

  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status) {
    while (1) {
    }
  }

  /* Setup RTCC for timekeeping in both active and backup modes.
     This also sets up and enables the LFXO */
  setupRTCC(appBuRtccResetVal);

  // Setup CRYOTIMER to give an interrupt once per second
  cryotimerSetup();

  // Setup humidity/temperature sensor, hall effect sensor and inductive proximity (LC) sensor
  sensorSetup();

  // Setup timers for PWM control of RGB leds
  setupRGBLed();

  // Inititate and calibrate touch slider
  // It is important that the slider is not touched during calibration
  setupCSEN();

  // Infinite loop
  while (1) {
    appCsenData = csenGetEvent();

    // Actions that should only be performed once per second (controlled by CRYOTIMER)
    if (cryoPeriod) {
      cryoPeriod = false;
      buMeasVdd(appBuVddMeasBuffer, (appBuBufferIndex + 1) % 90, &appBuAvddVoltage);
      appBuBufferIndex = (appBuBufferIndex + 1) % 90;
      GPIO_PinOutToggle(gpioPortA, 11);
    }

    // If the LC Sensor has been triggered, perform the required action
    if (lcTriggered) {
      appLcCounter++;
      randomColor(appLcRng, &appLedSettings);
      buSaveLedSettings(&appLedSettings);
      lcTriggered = false;
    }

    // Only read sensor data from I2C sensors when needed to save time and power
    if (currentApp == 3) {
      sensorReadHumTemp(&appRhtRhData, &appRhtTempData);
    } else if (currentApp == 2) {
      sensorReadHallEffect(&appHallField);
    }

    setLedSettings(&appLedSettings);

    // Touch event handling
    if (appCsenData.eventActive) {
      touchHandle = appCsenData.sliderTravel;

      // Active touch event - track movement
      if (lastCsenSliderPos != -1) {
        if (swipeLock) {
          swipeHandler();
        }
      }

      lastCsenSliderPos = appCsenData.sliderPos;
    } else {
      /* Touch event not active - check if one that is supposed to trigger an
         app change was recently conducted and not handled */
      if (touchHandle < -80 && !swipeLock) {
        currentApp++;
        if (currentApp == APP_COUNT) {
          currentApp = 0;
        }
      } else if (touchHandle > 80 && !swipeLock) {
        if (currentApp == 0) {
          currentApp = APP_COUNT - 1;
        } else {
          currentApp--;
        }
      }

      if (lastCsenSliderPos != -1) {
        /* Check if we have a tap event, which should trigger specific actions in certain apps */
        if (!forceTriggered && appCsenData.eventDuration < 300) {
          forceTouchHandler(lastCsenSliderPos);
        }
      }

      lastCsenSliderPos = -1;
      forceTriggered = false;
      buSetCurApp(currentApp);
      touchHandle = 0;
      appLedStartValue = 0;
    }

    GLIB_clear(&glibContext);

    if (touchHandle < 0 && !swipeLock) {
      if (currentApp == APP_COUNT - 1) {
        drawScreen(0, 0);
        screenXOffset = (touchHandle * 2) / 3;
        drawScreen(currentApp, screenXOffset);
      } else {
        screenXOffset = 128 + ((touchHandle * 2) / 3);
        nextApp = currentApp + 1;
        if (nextApp == APP_COUNT) {
          nextApp = 0;
        }
        drawScreen(currentApp, 0);
        drawScreen(nextApp, screenXOffset);
      }
    } else if (touchHandle > 0 && !swipeLock) {
      if (currentApp == 1) {
        drawScreen(0, 0);
        screenXOffset = (touchHandle * 2) / 3;
        drawScreen(currentApp, screenXOffset);
      } else {
        screenXOffset = ((touchHandle * 2) / 3) - 127;
        if (currentApp == 0) {
          nextApp = APP_COUNT - 1;
        } else {
          nextApp = currentApp - 1;
        }
        drawScreen(currentApp, 0);
        drawScreen(nextApp, screenXOffset);
      }
    } else {
      drawScreen(currentApp, 0);
    }

    DMD_updateDisplay();
  }
}

/**************************************************************************//**
 * @brief CRYOTIMER_IRQHandler
 * Interrupt Service Routine for CRYOTIMER Interrupt Line
 *****************************************************************************/
void CRYOTIMER_IRQHandler(void)
{
  CRYOTIMER_IntClear(0xFFFFFFFFUL);

  cryoPeriod = true;
}

/**************************************************************************//**
 * @brief LESENSE_IRQHandler
 * Interrupt Service Routine for LESENSE Interrupt Line
 *****************************************************************************/
void LESENSE_IRQHandler(void)
{
  /* Clear interrupt flag */
  LESENSE_IntClear(LESENSE_IF_CH3);

  lcTriggered = true;
}
