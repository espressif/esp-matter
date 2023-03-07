/**
 * @file
 * Platform abstraction for Door Lock Key Pad application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#include <DoorLockKeyPad_hw.h>
#include <board.h>
#include <ADC.h>
#include <events.h>
#include <Assert.h>
#include <zaf_event_helper.h>
//#define DEBUGPRINT
#include <DebugPrint.h>
#include <CC_Battery.h>


#define DOORHANDLE_BTN       APP_BUTTON_A
#define BATTERY_REPORT_BTN   APP_BUTTON_B
#define ENTER_USER_CODE      APP_BUTTON_C
#define LATCH_STATUS_LED     APP_LED_A
#define BOLT_STATUS_LED      APP_LED_B

/* Ensure we did not allocate the same physical button or led to more than one function */
STATIC_ASSERT((APP_BUTTON_LEARN_RESET != DOORHANDLE_BTN) &&
              (APP_BUTTON_LEARN_RESET != BATTERY_REPORT_BTN) &&
              (APP_BUTTON_LEARN_RESET != ENTER_USER_CODE) &&
              (DOORHANDLE_BTN != BATTERY_REPORT_BTN) &&
              (DOORHANDLE_BTN != ENTER_USER_CODE) &&
              (BATTERY_REPORT_BTN != ENTER_USER_CODE),
              STATIC_ASSERT_FAILED_button_overlap);
STATIC_ASSERT((APP_LED_INDICATOR != LATCH_STATUS_LED) &&
              (APP_LED_INDICATOR != BOLT_STATUS_LED) &&
              (LATCH_STATUS_LED != BOLT_STATUS_LED),
              STATIC_ASSERT_FAILED_led_overlap);


#define MY_BATTERY_SPEC_LEVEL_FULL         3000  // My battery's 100% level (millivolts)
#define MY_BATTERY_SPEC_LEVEL_EMPTY        2400  // My battery's 0% level (millivolts)

static void button_handler(BUTTON_EVENT event, bool is_called_from_isr)
{
  EVENT_APP app_event = EVENT_EMPTY;
  (void)event;
      /* Outside door handle #1 deactivated?
       * NB: If DOORHANDLE_BTN is held for more than 5 seconds then BTN_EVENT_LONG_PRESS
       *     will be received on DOORHANDLE_BTN release instead of BTN_EVENT_UP
       */
   
  if (BTN_EVENT_LONG_PRESS(APP_BUTTON_LEARN_RESET) == event)
  {
    app_event = EVENT_APP_BUTTON_LEARN_RESET_LONG_PRESS;
  }
  else if (BTN_EVENT_SHORT_PRESS(APP_BUTTON_LEARN_RESET) == event)
  {
    app_event = EVENT_APP_BUTTON_LEARN_RESET_SHORT_PRESS;
  }
  else if (BTN_EVENT_SHORT_PRESS(BATTERY_REPORT_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_BATTERY_REPORT;
  }
  else if (BTN_EVENT_SHORT_PRESS(ENTER_USER_CODE) == event)
  {
    app_event = EVENT_APP_BUTTON_ENTER_USER_CODE;
  }
  else if (BTN_EVENT_HOLD(DOORHANDLE_BTN) == event)
  {
    app_event = EVENT_APP_DOORHANDLE_ACTIVATED;
  }
  else if ((BTN_EVENT_UP(DOORHANDLE_BTN) == event) ||
           (BTN_EVENT_LONG_PRESS(DOORHANDLE_BTN) == event))
  {
    app_event = EVENT_APP_DOORHANDLE_DEACTIVATED;
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

void DoorLockKeyPad_hw_init(void)
{
  DPRINT("-----------------------------------\n");
  DPRINTF("%s: Hold/release: Activate/deactivate outside door handle #1\n", Board_GetButtonLabel(DOORHANDLE_BTN));
  DPRINTF("%s: Send battery report\n", Board_GetButtonLabel(BATTERY_REPORT_BTN));
  DPRINTF("%s: Toggle learn mode\n", Board_GetButtonLabel(APP_BUTTON_LEARN_RESET));
  DPRINT("      Hold 5 sec: Reset\n");
  DPRINTF("%s: Enter user code\n", Board_GetButtonLabel(ENTER_USER_CODE));
  DPRINTF("%s: Learn mode + identify\n", Board_GetLedLabel(APP_LED_INDICATOR));
  DPRINTF("%s: Latch closed(off)/open(on)\n", Board_GetLedLabel(LATCH_STATUS_LED));
  DPRINTF("%s: Bolt locked(on)/unlocked(off)\n", Board_GetLedLabel(BOLT_STATUS_LED));
  DPRINT("-----------------------------------\n\n");

  Board_SetButtonCallback(button_handler);
  Board_EnableButton(APP_BUTTON_LEARN_RESET);
  Board_EnableButton(DOORHANDLE_BTN);
  Board_EnableButton(BATTERY_REPORT_BTN);
  Board_EnableButton(ENTER_USER_CODE);
}

void DoorLockKeyPad_hw_latch_status_handler(bool opened)
{
  Board_SetLed(LATCH_STATUS_LED, opened ? LED_ON : LED_OFF);
}

void DoorLockKeyPad_hw_bolt_status_handler(bool locked)
{
  Board_SetLed(BOLT_STATUS_LED, locked ? LED_ON : LED_OFF);
}

uint8_t DoorLockKeyPad_hw_get_battery_level(void)
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
