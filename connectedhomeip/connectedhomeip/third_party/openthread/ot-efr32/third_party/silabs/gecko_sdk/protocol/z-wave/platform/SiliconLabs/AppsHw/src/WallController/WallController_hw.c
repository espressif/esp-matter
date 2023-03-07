/**
 * @file
 * Platform abstraction for Wall Controller application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#include <WallController_hw.h>
#include <Assert.h>
#include <ev_man.h>
#include <events.h>
#include <board.h>
#include <zaf_event_helper.h>
//#define DEBUGPRINT
#include "DebugPrint.h"

#define KEY01_BTN   APP_BUTTON_A
#define KEY02_BTN   APP_BUTTON_B
#define KEY03_BTN   APP_BUTTON_C

STATIC_ASSERT((APP_BUTTON_LEARN_RESET != KEY01_BTN) &&
              (APP_BUTTON_LEARN_RESET != KEY02_BTN) &&
              (APP_BUTTON_LEARN_RESET != KEY03_BTN) &&
              (KEY01_BTN != KEY02_BTN) &&
              (KEY01_BTN != KEY03_BTN) &&
              (KEY02_BTN != KEY03_BTN),
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
  else if (BTN_EVENT_SHORT_PRESS(KEY01_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_KEY01_SHORT_PRESS;
  }
  else if (BTN_EVENT_HOLD(KEY01_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_KEY01_HOLD;
  }
  else if ((BTN_EVENT_UP(KEY01_BTN) == event) || (BTN_EVENT_LONG_PRESS(KEY01_BTN) == event))
  {
    app_event = EVENT_APP_BUTTON_KEY01_RELEASE;
  }
  else if (BTN_EVENT_SHORT_PRESS(KEY02_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_KEY02_SHORT_PRESS;
  }
  else if (BTN_EVENT_HOLD(KEY02_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_KEY02_HOLD;
  }
  else if ((BTN_EVENT_UP(KEY02_BTN) == event) || (BTN_EVENT_LONG_PRESS(KEY02_BTN) == event))
  {
    app_event = EVENT_APP_BUTTON_KEY02_RELEASE;
  }
  else if (BTN_EVENT_SHORT_PRESS(KEY03_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_KEY03_SHORT_PRESS;
  }
  else if (BTN_EVENT_HOLD(KEY03_BTN) == event)
  {
    app_event = EVENT_APP_BUTTON_KEY03_HOLD;
  }
  else if ((BTN_EVENT_UP(KEY03_BTN) == event) || (BTN_EVENT_LONG_PRESS(KEY03_BTN) == event))
  {
    app_event = EVENT_APP_BUTTON_KEY03_RELEASE;
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

void WallController_hw_init(void)
{
  DPRINT("----------------------------------\n");

  DPRINTF("%s: BTN0 press/hold/release\n", Board_GetButtonLabel(KEY01_BTN));
  DPRINTF("%s: BTN2 press/hold/release\n", Board_GetButtonLabel(KEY02_BTN));
  DPRINTF("%s: BTN3 press/hold/release\n", Board_GetButtonLabel(KEY03_BTN));
  DPRINTF("%s: Toggle learn mode\n", Board_GetButtonLabel(APP_BUTTON_LEARN_RESET));
  DPRINT("      Hold 5 sec: Reset\n");
  DPRINTF("%s: LED1 Learn mode + identify\n", Board_GetLedLabel(APP_LED_INDICATOR));
  DPRINT("----------------------------------\n\n");

  Board_SetButtonCallback(button_handler);
  Board_EnableButton(APP_BUTTON_LEARN_RESET);
  Board_EnableButton(KEY01_BTN);
  Board_EnableButton(KEY02_BTN);
  Board_EnableButton(KEY03_BTN);

}
