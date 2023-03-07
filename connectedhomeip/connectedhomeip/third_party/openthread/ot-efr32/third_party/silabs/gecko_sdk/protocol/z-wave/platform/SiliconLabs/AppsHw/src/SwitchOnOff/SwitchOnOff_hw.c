/**
 * @file
 * Platform abstraction for Switch On/Off application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#include <SwitchOnOff_hw.h>
#include <board.h>
#include <Assert.h>
#include <events.h>
#include <board.h>
#include <zaf_event_helper.h>
//#define DEBUGPRINT
#include "DebugPrint.h"

/**
 * To run this feature in the application, please press and hold BTN0 on the EXP board.
 */
#ifndef HMI_CHECKER_ENABLE
#define HMI_CHECKER_ENABLE        0
#endif

#if (HMI_CHECKER_ENABLE)
#include "HMI_checker.h"
#endif

#define SWITCH_TOGGLE_BTN   APP_BUTTON_A
#define SWITCH_STATUS_LED   APP_LED_A

/* Ensure we did not allocate the same physical button or led to more than one function */
STATIC_ASSERT(APP_BUTTON_LEARN_RESET != SWITCH_TOGGLE_BTN,
              STATIC_ASSERT_FAILED_button_overlap);
STATIC_ASSERT(APP_LED_INDICATOR != SWITCH_STATUS_LED,
              STATIC_ASSERT_FAILED_led_overlap);


static void button_handler(BUTTON_EVENT event, bool is_called_from_isr)
{
#if (HMI_CHECKER_ENABLE)
  HMIChecker_buttonEventHandler(event);
#endif

  EVENT_APP app_event = EVENT_EMPTY;

  if (BTN_EVENT_LONG_PRESS(APP_BUTTON_LEARN_RESET) == event)
  {
    app_event = EVENT_APP_BUTTON_LEARN_RESET_LONG_PRESS;
  }
  else if (BTN_EVENT_SHORT_PRESS(APP_BUTTON_LEARN_RESET) == event)
  {
    app_event = EVENT_APP_BUTTON_LEARN_RESET_SHORT_PRESS;
  }
  else if (BTN_EVENT_SHORT_PRESS(SWITCH_TOGGLE_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_TOGGLE_SHORT_PRESS;
  }
  else if (BTN_EVENT_HOLD(SWITCH_TOGGLE_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_TOGGLE_HOLD;
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

void SwitchOnOff_hw_init(void)
{
  DPRINT("--------------------------------\n");
  DPRINTF("%s: Toggle switch on/off\n", Board_GetButtonLabel(SWITCH_TOGGLE_BTN));
  DPRINT("      Hold 5 sec: TX INIF\n");
  DPRINTF("%s: Toggle learn mode\n", Board_GetButtonLabel(APP_BUTTON_LEARN_RESET));
  DPRINT("      Hold 5 sec: Reset\n");
  DPRINTF("%s: Learn mode + identify\n", Board_GetLedLabel(APP_LED_INDICATOR));
  DPRINTF("%s: Switch status on/off\n", Board_GetLedLabel(SWITCH_STATUS_LED));
  DPRINT("--------------------------------\n\n");

  Board_SetButtonCallback(button_handler);
  Board_EnableButton(APP_BUTTON_LEARN_RESET);
  Board_EnableButton(SWITCH_TOGGLE_BTN);

#if (HMI_CHECKER_ENABLE)
  HMIChecker_Init();  // Used to check the LEDs on the EXP board.
#endif
}

void SwitchOnOff_hw_binary_switch_handler(bool on)
{
    Board_SetLed(SWITCH_STATUS_LED, on ? LED_ON : LED_OFF);
}
