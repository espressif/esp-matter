/***************************************************************************//**
 * @file
 * @brief Demo example for SLSTK3301A
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
#include <stdlib.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_cryotimer.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"

#include "segmentlcd.h"

#include "app_bumode.h"
#include "app_csen.h"
#include "app_rtcc.h"
#include "app_sensor.h"

#include "sl_system_init.h"
#include "sl_simple_led.h"
#include "sl_simple_led_instances.h"
#include "sl_simple_button_instances.h"

// Global state variables
static uint8_t currentApp = 0;
static bool cryoPeriod = false, lcTriggered = false, rngDisp = false;

// App-specific variables
int16_t appHallField = 0;
bool appRhtCelsius = true;
int32_t appRhtTempData = 0;
uint32_t appRhtRhData = 0;
CSEN_Event_t appCsenData = CSEN_EVENT_DEFAULT;
bool appBuChargeEn = false, appBuDisCharge = false;
uint8_t appBuSelect = 3;
uint16_t appBuVoltage = 0;
uint16_t appBuAvddVoltage = 315;
uint32_t appBuRtccResetVal = 0, appBuRtccResetTs = 0, appBuBodCnt = 0, appBuBuTime = 0, appBuBodTime = 0;
uint16_t appLcCounter = 0;
uint32_t appLcRng[4];
bool btn0Pressed = false;
bool btn1Pressed = false;

#define APP_COUNT       6
#define RUN_TIME        0
#define HALL_SENSOR     1
#define RT_SENSOR       2
#define TOUCH_SENSE     3
#define LC_SENSE        4
#define BU_DOMAIN       5

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
}

/**************************************************************************//**
 * @brief LCD display for different application.
 *****************************************************************************/
void drawScreen(uint8_t app)
{
  uint32_t i;
  char str[13];

  switch (app) {
    case RUN_TIME:
      // Display elapsed time
      i = millis() / 1000;
      SegmentLCD_Number((i / 3600) * 10000 + ((i % 3600) / 60) * 100 + (i % 60));
      SegmentLCD_Symbol(LCD_SYMBOL_COL1, true);
      SegmentLCD_Symbol(LCD_SYMBOL_COL2, true);
      // Display title
      SegmentLCD_Write("TG11DEMO");
      SegmentLCD_Symbol(LCD_SYMBOL_EFM32, true);
      break;

    case HALL_SENSOR:
      // Display H for hall sensor
      SegmentLCD_Array(0, true);
      SegmentLCD_Array(4, true);
      SegmentLCD_Array(7, true);
      SegmentLCD_Array(11, true);
      for (i = 14; i < 19; i++) {
        SegmentLCD_Array(i, true);
      }
      SegmentLCD_Array(21, true);
      SegmentLCD_Array(25, true);
      SegmentLCD_Array(28, true);
      SegmentLCD_Array(32, true);
      // Display field strength
      SegmentLCD_Write("FIELD mT");
      SegmentLCD_Number(appHallField * 100 / 80);
      SegmentLCD_Symbol(LCD_SYMBOL_C18, 1);
      break;

    case RT_SENSOR:
      // Display R for relative humidity
      for (i = 0; i < 4; i++) {
        SegmentLCD_Array(i, true);
      }
      SegmentLCD_Array(7, true);
      SegmentLCD_Array(11, true);
      for (i = 14; i < 18; i++) {
        SegmentLCD_Array(i, true);
      }
      SegmentLCD_Array(21, true);
      SegmentLCD_Array(23, true);
      SegmentLCD_Array(28, true);
      SegmentLCD_Array(31, true);
      // Display temperature and relative humidity
      if (appRhtCelsius) {
        sprintf(str, "T %6ld", appRhtTempData);
        SegmentLCD_Symbol(LCD_SYMBOL_S13, true);
        SegmentLCD_Symbol(LCD_SYMBOL_S14, false);
      } else {
        sprintf(str, "T %6ld", (appRhtTempData * 9 + 160000) / 5);
        SegmentLCD_Symbol(LCD_SYMBOL_S13, false);
        SegmentLCD_Symbol(LCD_SYMBOL_S14, true);
      }
      str[8] = '\0';                            // Truncate to display size.
      SegmentLCD_Write(str);
      SegmentLCD_Number(appRhtRhData);
      SegmentLCD_Symbol(LCD_SYMBOL_S11, true);
      SegmentLCD_Symbol(LCD_SYMBOL_S12, true);
      SegmentLCD_Symbol(LCD_SYMBOL_C13, true);
      SegmentLCD_Symbol(LCD_SYMBOL_C17, true);
      SegmentLCD_Symbol(LCD_SYMBOL_GECKO, true);
      break;

    case TOUCH_SENSE:
      // Display F for force
      for (i = 0; i < 5; i++) {
        SegmentLCD_Array(i, true);
      }
      SegmentLCD_Array(7, true);
      for (i = 14; i < 18; i++) {
        SegmentLCD_Array(i, true);
      }
      SegmentLCD_Array(21, true);
      SegmentLCD_Array(28, true);
      // Display slider position and force
      if (appCsenData.eventActive) {
        sprintf(str, "POS %3ld", appCsenData.sliderPos);
        i = appCsenData.touchForce / 100;
      } else {
        sprintf(str, "POS %3d", 0);
        i = 0;
      }
      SegmentLCD_Write(str);
      SegmentLCD_Number(i);
      break;

    case LC_SENSE:
      if (rngDisp) {
        // Display random pattern based on TRNG
        appLcRng[0] = appLcRng[0] ^ appLcRng[1];
        for (i = 0; i < 17; i++) {
          SegmentLCD_Array(i, (appLcRng[0] >> i) & 0x01);
        }
        appLcRng[2] = appLcRng[2] ^ appLcRng[3];
        for (i = 17; i < 35; i++) {
          SegmentLCD_Array(i, (appLcRng[2] >> (i - 17)) & 0x01);
        }
        rngDisp = false;
      }
      // Display LCSENSE triggered count
      SegmentLCD_Write("TRIG CNT");
      SegmentLCD_Number(appLcCounter);
      SegmentLCD_Symbol(LCD_SYMBOL_GECKO, true);
      break;

    case BU_DOMAIN:
      // Clear array
      for (i = 0; i < 35; i++) {
        SegmentLCD_Array(i, false);
      }
      // Display C for charge
      if (appBuChargeEn) {
        for (i = 1; i < 4; i++) {
          SegmentLCD_Array(i, true);
        }
        SegmentLCD_Array(7, true);
        SegmentLCD_Array(11, true);
        SegmentLCD_Array(14, true);
        SegmentLCD_Array(21, true);
        SegmentLCD_Array(25, true);
        for (i = 29; i < 32; i++) {
          SegmentLCD_Array(i, true);
        }
      }
      // Display D for discharge
      if (appBuDisCharge) {
        for (i = 0; i < 4; i++) {
          SegmentLCD_Array(i, true);
        }
        SegmentLCD_Array(8, true);
        SegmentLCD_Array(11, true);
        SegmentLCD_Array(15, true);
        SegmentLCD_Array(18, true);
        SegmentLCD_Array(22, true);
        SegmentLCD_Array(25, true);
        for (i = 28; i < 32; i++) {
          SegmentLCD_Array(i, true);
        }
      }
      // Display backup voltage
      sprintf(str, "BUV %3dV", appBuVoltage);
      str[8] = '\0';                            // Truncate to display size.
      SegmentLCD_Write(str);
      if (appBuVoltage) {
        SegmentLCD_Symbol(LCD_SYMBOL_C13, true);
      } else {
        SegmentLCD_Symbol(LCD_SYMBOL_C13, false);
      }
      // Display AVDD
      SegmentLCD_Number(appBuAvddVoltage);
      SegmentLCD_Symbol(LCD_SYMBOL_C18, true);
      SegmentLCD_Symbol(LCD_SYMBOL_GECKO, true);
      break;

    default:
      break;
  }
}

/**************************************************************************//**
 * @brief  Main function of EFM32TG11 demo example.
 *****************************************************************************/
int main(void)
{
  sl_system_init();

  // Setup backup mode.
  // Restore values stored in retention registers if we are waking up from a backup event.
  buSetup(&appBuRtccResetVal, &appBuRtccResetTs, &appBuBodCnt, &appBuBuTime, &appBuBodTime, &currentApp);

  // Setup RTCC for timekeeping in both active and backup modes.
  setupRTCC(appBuRtccResetVal);

  // Enable LCD without voltage boost, use LFRCO as LCD clock source
  SegmentLCD_Init(false);

  // Setup CRYOTIMER to give an interrupt once per second
  cryotimerSetup();

  // Setup humidity/temperature sensor, hall effect sensor and inductive proximity (LC) sensor
  sensorSetup();

  // Inititate and calibrate touch slider
  // It is important that the slider is not touched during calibration
  setupCSEN();

  // Infinite loop
  while (1) {
    // Wait event at EM2
    EMU_EnterEM2(false);

    // Get the latest CSEN status
    appCsenData = csenGetEvent();

    // Actions that should only be performed once per second (controlled by CRYOTIMER)
    if (cryoPeriod) {
      cryoPeriod = false;
      // Read sensor data from I2C sensors
      sensorReadHumTemp(&appRhtRhData, &appRhtTempData);
      sensorReadHallEffect(&appHallField);
      buMeasVdd(&appBuVoltage, &appBuAvddVoltage);
    }

    // If the LC Sensor has been triggered, perform the required action
    if (lcTriggered) {
      appLcCounter++;
      sl_led_turn_on(&sl_led_led1);
      lcTriggered = false;
      if (currentApp == LC_SENSE) {
        setupTRNG();
        randomNumber(appLcRng);
        rngDisp = true;
      }
    }

    // Push buttons event handling
    if (btn0Pressed) {
      // Next application
      currentApp++;
      if (currentApp == APP_COUNT) {
        currentApp = 0;
      }
      // Reset flag, save current application and clear screen
      btn0Pressed = false;
      buSetCurApp(currentApp);
      SegmentLCD_AllOff();
    }

    if (btn1Pressed) {
      switch (currentApp) {
        case RT_SENSOR:
          // Toggle between C and F
          if (appRhtCelsius) {
            appRhtCelsius = false;
          } else {
            appRhtCelsius = true;
          }
          break;

        case LC_SENSE:
          // Clear LCSENSE triggered count
          appLcCounter = 0;
          sl_led_turn_on(&sl_led_led0);
          appLcRng[0] = 0;
          appLcRng[1] = 0;
          appLcRng[2] = 0;
          appLcRng[3] = 0;
          rngDisp = true;
          break;

        case BU_DOMAIN:
          // Select charge on/off and discharge on/off on backup capacitor
          switch (++appBuSelect & 0x03) {
            case 0:
              appBuChargeEn = true;
              break;

            case 1:
              appBuChargeEn = false;
              break;

            case 2:
              appBuDisCharge = true;
              break;

            case 3:
              appBuDisCharge = false;
              break;
          }
          buPwrSwitch(appBuChargeEn, appBuDisCharge);
          break;

        default:
          break;
      }
      // Reset flag
      btn1Pressed = false;
    }

    // Update LCD display
    drawScreen(currentApp);
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

/**************************************************************************//**
 * @brief Push button handler
 *        PB0 Starts selected test
 *        PB1 Cycles through the available tests
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (&sl_button_btn0 == handle) {
    if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
      btn0Pressed = true;
    }
  }

  if (&sl_button_btn1 == handle) {
    if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
      btn1Pressed = true;
    }
  }
}
