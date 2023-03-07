/**
 * @file
 * Platform abstraction for Sensor PIR application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#include <SensorPIR_hw.h>
#include <Assert.h>
#include <board.h>
#include <events.h>
#include <ADC.h>
#include <zaf_event_helper.h>
//#define DEBUGPRINT
#include <DebugPrint.h>
#include <CC_Battery.h>
#include <zpal_init.h>

#define MY_BATTERY_SPEC_LEVEL_FULL         3000  // My battery's 100% level (millivolts)
#define MY_BATTERY_SPEC_LEVEL_EMPTY        2400  // My battery's 0% level (millivolts)

#if defined(RADIO_BOARD_EFR32ZG13P32) || defined(RADIO_BOARD_EFR32ZG13S)
  // The EFR32ZG13P32 device has reduced number of GPIO pins and hence less
  // button inputs available for the application to use. Therefore alternative
  // button mapping is required

  #define PIR_EVENT_BTN        APP_BUTTON_LEARN_RESET  // Overload the LEARN_RESET button to also function as the PIR event button
                                                       // (It's the only button that can wakeup from EM4 on EFR32ZG13P32)

  // Define the button events used to signify PIR sensor state transitions:
  //
  // PIR_EVENT_TRANSITION_TO_ACTIVE
  //   Triggered by the BTN_EVENT_HOLD event which means the button is pressed and held
  #define PIR_EVENT_TRANSITION_TO_ACTIVE(event)   (BTN_EVENT_HOLD(PIR_EVENT_BTN) == (BUTTON_EVENT)event)
  //
  // PIR_EVENT_TRANSITION_TO_DEACTIVE
  //   Triggered by the BTN_EVENT_UP event which means the button was released after a BTN_EVENT_HOLD period
  #define PIR_EVENT_TRANSITION_TO_DEACTIVE(event) (BTN_EVENT_UP(PIR_EVENT_BTN) == (BUTTON_EVENT)event)

#else

  #define PIR_EVENT_BTN        APP_WAKEUP_SLDR_BTN  // We prefer a wakeup enabled slider, but a button will do

  // Define the button events used to signify PIR sensor state transitions:
  //
  // PIR_EVENT_TRANSITION_TO_ACTIVE
  //   The PIR_EVENT_BTN could be allocated to either a slider or a button.
  //   - The slider will always send a DOWN event when moved to the ON position.
  //   - A button will send SHORT_PRESS or HOLD event when pressed. Only the
  //     HOLD event will be followed by an UP event when the button is
  //     released. Since we need the UP event later to cancel the power
  //     lock, we ignore the SHORT_PRESS event here.
  #define PIR_EVENT_TRANSITION_TO_ACTIVE(event)   ((BTN_EVENT_DOWN(PIR_EVENT_BTN) == (BUTTON_EVENT)event) || \
                                                   (BTN_EVENT_HOLD(PIR_EVENT_BTN) == (BUTTON_EVENT)event))
  // PIR_EVENT_TRANSITION_TO_DEACTIVE
  //   The PIR_EVENT_BTN could be allocated to either a slider or a button.
  //   The slider will always send an UP event when moved to the OFF position.
  //   A button will send either an UP, SHORT_PRESS or LONG_PRESS on release
  //   depending on how long it has been pressed.
  #define PIR_EVENT_TRANSITION_TO_DEACTIVE(event) ((BTN_EVENT_UP(PIR_EVENT_BTN) == (BUTTON_EVENT)event) || \
                                                   (BTN_EVENT_SHORT_PRESS(PIR_EVENT_BTN) == (BUTTON_EVENT)event) || \
                                                   (BTN_EVENT_LONG_PRESS(PIR_EVENT_BTN) == (BUTTON_EVENT)event))

#endif
#define BATTERY_REPORT_BTN   APP_BUTTON_A         // This button cannot wake up the device from EM4
                                                  // (i.e. it will generally not work with SensorPIR)

/* Ensure we did not allocate the same physical button to more than one function */
#if !defined(RADIO_BOARD_EFR32ZG13P32) && !defined(RADIO_BOARD_EFR32ZG13S) // Skipped for EFR32ZG13P32 where the shortage of GPIOs means we need to assign dual function to buttons
STATIC_ASSERT((APP_BUTTON_LEARN_RESET != PIR_EVENT_BTN) &&
              (APP_BUTTON_LEARN_RESET != BATTERY_REPORT_BTN) &&
              (PIR_EVENT_BTN != BATTERY_REPORT_BTN),
              STATIC_ASSERT_FAILED_button_overlap);
#endif


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
  else if (BTN_EVENT_SHORT_PRESS(BATTERY_REPORT_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_BATTERY_REPORT;
  }
  else if (PIR_EVENT_TRANSITION_TO_ACTIVE(event))
  {
    app_event = EVENT_APP_TRANSITION_TO_ACTIVE;
  }
  else if (PIR_EVENT_TRANSITION_TO_DEACTIVE(event))
  {
    app_event = EVENT_APP_TRANSITION_TO_DEACTIVE;
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

void SensorPIR_hw_init(void)
{
  Board_SetButtonCallback(button_handler);
  Board_EnableButton(APP_BUTTON_LEARN_RESET);
  Board_EnableButton(BATTERY_REPORT_BTN);
  Board_EnableButton(PIR_EVENT_BTN);

  DPRINT("-----------------------------\n");
  DPRINTF("%s: Send battery report\n", Board_GetButtonLabel(BATTERY_REPORT_BTN));
  DPRINTF("%s: Toggle learn mode\n", Board_GetButtonLabel(APP_BUTTON_LEARN_RESET));
  DPRINT("      Hold 5 sec: Reset\n");
  DPRINTF("%s: Activate PIR event\n", Board_GetButtonLabel(PIR_EVENT_BTN));
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

uint8_t SensorPIR_hw_get_battery_level(void)
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

void SensorPIR_hw_deep_sleep_wakeup_handler(void)
{
  uint32_t em4_wakeup_flags = Board_GetGpioEm4Flags();

  if (0 != em4_wakeup_flags)
  {
    Board_ProcessEm4PinWakeupFlags(em4_wakeup_flags);
  }
}
