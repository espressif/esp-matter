/**
 * @file
 * Platform abstraction for KeyFob application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#include <KeyFob_hw.h>
#include <Assert.h>
#include <board.h>
#include <events.h>
#include <zaf_event_helper.h>

//#define DEBUGPRINT
#include <DebugPrint.h>
#include <CC_Battery.h>

#define MY_BATTERY_SPEC_LEVEL_FULL         3000  // My battery's 100% level (millivolts)
#define MY_BATTERY_SPEC_LEVEL_EMPTY        2400  // My battery's 0% level (millivolts)

#define BUTTON_NETWORK_ADD_REMOVE   APP_BUTTON_A           // Dual function for inclusion and exclusion (no interrupt)
#define BUTTON_BASIC_SET_ON         APP_BUTTON_LEARN_RESET // Basic Set ON  (interrupt)
#define BUTTON_BASIC_SET_OFF        APP_BUTTON_B           // Basic Set OFF (interrupt)
#define BUTTON_NETWORK_LEARNMODE    APP_BUTTON_C           // Function for including/excluding KeyFob into/from Network

/************************************************
 * LED definitions
 ***********************************************/

#define LED_BASIC_ADD_GROUP         APP_LED_A
#define LED_BASIC_REMOVE_GROUP      APP_LED_C
#define LED_BASIC_SET_ON            APP_LED_INDICATOR
#define LED_BASIC_SET_OFF           APP_LED_C
#define LED_NETWORK_ADD_REMOVE      APP_LED_INDICATOR
#define LED_NETWORK_LEARNMODE       APP_LED_B

STATIC_ASSERT((BUTTON_NETWORK_ADD_REMOVE != BUTTON_BASIC_SET_ON) &&
              (BUTTON_NETWORK_ADD_REMOVE != BUTTON_BASIC_SET_OFF) &&
              (BUTTON_BASIC_SET_ON != BUTTON_BASIC_SET_OFF),
              STATIC_ASSERT_FAILED_button_overlap);


static void button_handler(BUTTON_EVENT event, bool is_called_from_isr)
{
  EVENT_APP app_event = EVENT_EMPTY;

  if (BTN_EVENT_LONG_PRESS(BUTTON_NETWORK_ADD_REMOVE) == event)
  {
    app_event = EVENT_APP_BUTTON_RESET;
  }
  if (BTN_EVENT_HOLD(BUTTON_NETWORK_ADD_REMOVE) == event)
  {
    app_event = EVENT_APP_BUTTON_EXCLUDE_DEVICE;
  }
  else if (BTN_EVENT_SHORT_PRESS(BUTTON_NETWORK_ADD_REMOVE) == event)
  {
    app_event = EVENT_APP_BUTTON_INCLUDE_DEVICE;
  }
  if (BTN_EVENT_HOLD(BUTTON_BASIC_SET_ON) == event)
  {
    app_event = EVENT_APP_BUTTON_ASSOCIATION_GROUP_ADD;
  }
  if (BTN_EVENT_UP(BUTTON_BASIC_SET_ON) == event)
  {
    app_event = EVENT_APP_BUTTON_UP_ASSOCIATION_GROUP_ADD;
  }
  else if (BTN_EVENT_SHORT_PRESS(BUTTON_BASIC_SET_ON) == event)
  {
    app_event = EVENT_APP_BUTTON_BASIC_ON;
  }
  if (BTN_EVENT_HOLD(BUTTON_BASIC_SET_OFF) == event)
  {
    app_event = EVENT_APP_BUTTON_ASSOCIATION_GROUP_REMOVE;
  }
  if (BTN_EVENT_UP(BUTTON_BASIC_SET_OFF) == event)
  {
    app_event = EVENT_APP_BUTTON_UP_ASSOCIATION_GROUP_REMOVE;
  }
  else if (BTN_EVENT_SHORT_PRESS(BUTTON_BASIC_SET_OFF) == event)
  {
    app_event = EVENT_APP_BUTTON_BASIC_OFF;
  }
  if (BTN_EVENT_HOLD(BUTTON_NETWORK_LEARNMODE) == event)
  {
    app_event = EVENT_APP_BUTTON_NETWORK_LEARNMODE_NWE;
  }
  else if (BTN_EVENT_SHORT_PRESS(BUTTON_NETWORK_LEARNMODE) == event)
  {
    app_event = EVENT_APP_BUTTON_NETWORK_LEARNMODE_NWI;
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

void KeyFob_hw_init(EResetReason_t reset_reason)
{
  Board_SetButtonCallback(button_handler);
  Board_EnableButton(BUTTON_NETWORK_ADD_REMOVE);
  Board_EnableButton(BUTTON_BASIC_SET_ON);
  Board_EnableButton(BUTTON_BASIC_SET_OFF);
  Board_EnableButton(BUTTON_NETWORK_LEARNMODE);

  // Initialize LEDs and turn them off.
  Board_ConfigLed(LED_BASIC_ADD_GROUP, true);
  Board_ConfigLed(LED_BASIC_SET_ON, true);
  Board_ConfigLed(LED_BASIC_SET_OFF, true);
  Board_ConfigLed(LED_NETWORK_LEARNMODE, true);


  DPRINT("-----------------------------\n");

  DPRINTF("%s: Press for ADD, HOLD for REMOVE End device\n",
                                                       Board_GetButtonLabel(BUTTON_NETWORK_ADD_REMOVE));
  DPRINTF("%s: Hold 5+ sec and release for Reset to Default\n",
                                                       Board_GetButtonLabel(BUTTON_NETWORK_ADD_REMOVE));
  DPRINTF("%s: Press for NWI, HOLD for NWE of KeyFob\n",
                                                       Board_GetButtonLabel(BUTTON_NETWORK_LEARNMODE));
  DPRINT("Basic Controls:\n");
  DPRINTF("%s: Press and release for BasicSet ON to Group\n",
                                                       Board_GetButtonLabel(BUTTON_BASIC_SET_ON));
  DPRINTF("%s: HOLD and release for ADD End device to Group\n",
                                                       Board_GetButtonLabel(BUTTON_BASIC_SET_ON));
  DPRINTF("%s: Press and release for BasicSet OFF to Group\n",
                                                       Board_GetButtonLabel(BUTTON_BASIC_SET_OFF));
  DPRINTF("%s: HOLD and release for REMOVE End device from Group\n",
                                                       Board_GetButtonLabel(BUTTON_BASIC_SET_OFF));
  DPRINT("LED indications:\n");
  DPRINTF("%s: One short Blink at BasicSet ON Tx\n",  Board_GetLedLabel(LED_BASIC_SET_ON));
  DPRINTF("%s: ON when ADD End device to Group Active\n",
                                                       Board_GetLedLabel(LED_BASIC_ADD_GROUP));
  DPRINTF("%s: One short Blink at Basic Set OFF Tx\n", Board_GetLedLabel(LED_BASIC_SET_OFF));
  DPRINTF("%s: ON when REMOVE End device from Group Active\n",
                                                       Board_GetLedLabel(LED_BASIC_REMOVE_GROUP));
  DPRINTF("%s: Blink once a sec when ADD/REMOVE to/from Group Active\n", Board_GetLedLabel(LED_NETWORK_ADD_REMOVE));
  DPRINTF("%s: Short Blink once a sec ADD/REMOVE End device Active\n",
                                                       Board_GetLedLabel(LED_NETWORK_ADD_REMOVE));
  DPRINTF("%s: ON when LearnMode NWI/NWE Active\n",
                                                       Board_GetLedLabel(LED_NETWORK_LEARNMODE));
  DPRINT("-----------------------------\n\n");

  DPRINTF("Board_GetGpioEm4Flags()      = 0x%08x\n",   Board_GetGpioEm4Flags());

  if (reset_reason == ERESETREASON_DEEP_SLEEP_WUT || reset_reason == ERESETREASON_DEEP_SLEEP_EXT_INT)
  {
    #ifdef DEBUGPRINT
      Board_DebugPrintEm4WakeupFlags(Board_GetGpioEm4Flags());
    #endif
  }
}

uint8_t KeyFob_hw_get_battery_level(void)
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

void KeyFob_hw_deep_sleep_wakeup_handler(void)
{
  uint32_t em4_wakeup_flags = Board_GetGpioEm4Flags();

  if (0 != em4_wakeup_flags)
  {
    Board_ProcessEm4PinWakeupFlags(em4_wakeup_flags);
  }
}

void KeyFob_basic_on_Led_handler(bool ledOn)
{
  Board_SetLed(LED_BASIC_SET_ON, ledOn ? LED_ON : LED_OFF);
}

void KeyFob_basic_off_Led_handler(bool ledOn)
{
  Board_SetLed(LED_BASIC_SET_OFF, ledOn ? LED_ON : LED_OFF);
}

void KeyFob_group_add_Led_handler(bool ledOn)
{
  Board_SetLed(LED_BASIC_ADD_GROUP, ledOn ? LED_ON : LED_OFF);
}

void KeyFob_group_remove_Led_handler(bool ledOn)
{
  Board_SetLed(LED_BASIC_REMOVE_GROUP, ledOn ? LED_ON : LED_OFF);
}

void KeyFob_node_add_remove_Led_handler(bool ledOn)
{
  Board_SetLed(LED_NETWORK_ADD_REMOVE, ledOn ? LED_ON : LED_OFF);
}

void KeyFob_network_learnmode_led_handler(bool ledOn)
{
  Board_SetLed(LED_NETWORK_LEARNMODE, ledOn ? LED_ON : LED_OFF);
}
