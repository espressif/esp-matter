/**
 * @file
 * Platform abstraction for Multilevel Sensor application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#include <MultilevelSensor_hw.h>
#include <Assert.h>
#include <ev_man.h>
#include <events.h>
#include <board.h>
#include <zaf_event_helper.h>
//#define DEBUGPRINT
#include "DebugPrint.h"
#include <CC_Battery.h>
#include <zpal_init.h>

#include <em_emu.h>

#define MY_BATTERY_SPEC_LEVEL_FULL         3000  // My battery's 100% level (millivolts)
#define MY_BATTERY_SPEC_LEVEL_EMPTY        2400  // My battery's 0% level (millivolts)

// Only use BTN PB1 and PB2 on Multilevel Sensor
// PB3 and PB4 from the extension board use the same 
// Ports and Pins that the I2C uses
#define EVENT_BTN             APP_BUTTON_LEARN_RESET
#define REPORT_BTN            APP_BUTTON_A         // This button cannot wake up the device from EM4
#define BASIC_SET_BTN         APP_BUTTON_A

#define APP_LED_POWER_ON      APP_LED_A

// Define the button events used to signify  sensor state transitions:
//
// EVENT_TRANSITION_TO_ACTIVE
//   Triggered by the BTN_EVENT_HOLD event which means the button is pressed and held
#define EVENT_TRANSITION_TO_ACTIVE(event)   (BTN_EVENT_HOLD(EVENT_BTN) == (BUTTON_EVENT)event)
//
// EVENT_TRANSITION_TO_DEACTIVE
//   Triggered by the BTN_EVENT_UP event which means the button was released after a BTN_EVENT_HOLD period
#define EVENT_TRANSITION_TO_DEACTIVE(event) (BTN_EVENT_UP(EVENT_BTN) == (BUTTON_EVENT)event)

/* Ensure we did not allocate the same physical button to more than one function */
STATIC_ASSERT((APP_BUTTON_LEARN_RESET != REPORT_BTN),
              STATIC_ASSERT_FAILED_button_overlap);

static void button_handler(BUTTON_EVENT event, bool is_called_from_isr)
{
  EVENT_APP app_event = EVENT_EMPTY;

  if (BTN_EVENT_LONG_PRESS(APP_BUTTON_LEARN_RESET) == event)
  {
    app_event = EVENT_APP_BUTTON_LEARN_RESET_LONG_PRESS;
  }
  else if (BTN_EVENT_SHORT_PRESS(APP_BUTTON_LEARN_RESET) == event)
  {
    app_event = EVENT_APP_BUTTON_LEARN_RESET_SHORT_PRESS;
  }
  else if (BTN_EVENT_SHORT_PRESS(REPORT_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_BATTERY_AND_SENSOR_REPORT;
  }
  else if (EVENT_TRANSITION_TO_ACTIVE(event))
  {
    app_event = EVENT_APP_TRANSITION_TO_ACTIVE;
  }
  else if (EVENT_TRANSITION_TO_DEACTIVE(event))
  {
    app_event = EVENT_APP_TRANSITION_TO_DEACTIVE;
  }
  else if (BTN_EVENT_HOLD(BASIC_SET_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_BASIC_SET_REPORT;
  }

  if (app_event != EVENT_EMPTY)
  {
    if (is_called_from_isr)
    {
      ZAF_EventHelperEventEnqueueFromISR(app_event);
    }
    else
    {
      ZAF_EventHelperEventEnqueue(app_event);
    }
  }
}

void MultilevelSensor_hw_init(void)
{
  /* Init indicator LED */
  Board_SetLed(APP_LED_POWER_ON, LED_ON);

  /* hardware initialization */
  Board_SetButtonCallback(button_handler);
  Board_EnableButton(APP_BUTTON_LEARN_RESET);
  Board_EnableButton(REPORT_BTN); // BASIC_SET_BTN mapped to the same button
  Board_EnableButton(EVENT_BTN);

  DPRINT("-----------------------------\n");
  DPRINTF("%s: Send battery and temperature report\n", Board_GetButtonLabel(REPORT_BTN));
  DPRINTF("%s: Toggle learn mode\n", Board_GetButtonLabel(APP_BUTTON_LEARN_RESET));
  DPRINT("      Hold 5 sec: Reset\n");
  DPRINTF("%s: Activate  event\n", Board_GetButtonLabel(EVENT_BTN));
  DPRINT("      (leave deactivated to allow going to sleep)\n");
  DPRINTF("%s: Learn mode + identify\n", Board_GetLedLabel(APP_LED_INDICATOR));
  DPRINT("-----------------------------\n\n");
  DPRINTF("\r\nBoard_GetGpioEm4Flags()      = 0b%08x", Board_GetGpioEm4Flags());

  const zpal_reset_reason_t reset_reason = zpal_get_reset_reason();
  if (reset_reason == ZPAL_RESET_REASON_DEEP_SLEEP_WUT || reset_reason == ZPAL_RESET_REASON_DEEP_SLEEP_EXT_INT)
  {
    #ifdef DEBUGPRINT
      Board_DebugPrintEm4WakeupFlags(Board_GetGpioEm4Flags());
    #endif
  }

}

void MultilevelSensor_hw_deep_sleep_wakeup_handler(void)
{
  uint32_t em4_wakeup_flags = Board_GetGpioEm4Flags();

  if (0 != em4_wakeup_flags)
  {
    Board_ProcessEm4PinWakeupFlags(em4_wakeup_flags);
  }
}

uint8_t MultilevelSensor_hw_get_battery_level(void)
{
  uint32_t VBattery;
  uint8_t  accurateLevel;
  uint8_t  roundedLevel;

  /*
   * Simple example how to use the ADC to measure the battery voltage
   * and convert to a percentage battery level on a linear scale.
   */
  ADC_Enable();
  VBattery = ADC_Measure_VSupply();
  DPRINTF("\r\nBattery voltage: %dmV", VBattery);
  ADC_Disable();

  if (MY_BATTERY_SPEC_LEVEL_FULL <= VBattery)
  {
    // Level is full
    return (uint8_t)CMD_CLASS_BATTERY_LEVEL_FULL;
  }
  else if (MY_BATTERY_SPEC_LEVEL_EMPTY > VBattery)
  {
    // Level is empty (<0%)
    return (uint8_t)CMD_CLASS_BATTERY_LEVEL_WARNING;
  }
  else
  {
    // Calculate the percentage level from 0 to 100
    accurateLevel = (uint8_t)((100 * (VBattery - MY_BATTERY_SPEC_LEVEL_EMPTY)) / (MY_BATTERY_SPEC_LEVEL_FULL - MY_BATTERY_SPEC_LEVEL_EMPTY));

    // And round off to the nearest "BATTERY_LEVEL_REPORTING_DECREMENTS" level
    roundedLevel =  (accurateLevel / BATTERY_LEVEL_REPORTING_DECREMENTS) * BATTERY_LEVEL_REPORTING_DECREMENTS; // Rounded down
    if ((accurateLevel % BATTERY_LEVEL_REPORTING_DECREMENTS) >= (BATTERY_LEVEL_REPORTING_DECREMENTS / 2))
    {
      roundedLevel += BATTERY_LEVEL_REPORTING_DECREMENTS; // Round up
    }
  }
  return roundedLevel;
}

void EMU_EM4PresleepHook(void)
{
  Board_SetLed(APP_LED_POWER_ON, LED_OFF);
}
