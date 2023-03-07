/**
 * @file
 * Platform abstraction for LED Bulb application
 * 
 * @copyright 2021 Silicon Laboratories Inc.
 */
#include <LEDBulb_hw.h>
#include <ZAF_Actuator.h>
#include <sl_simple_rgb_pwm_led.h>
#include <sl_simple_rgb_pwm_led_instances.h>
#include <ev_man.h>
#include <events.h>
#include <board.h>
#include <zaf_event_helper.h>
//#define DEBUGPRINT
#include "DebugPrint.h"

static uint8_t multilevel_switch_max;
static uint8_t multilevel_switch_value;
static uint8_t color_switch_max;
static uint8_t color_switch_red_value;
static uint8_t color_switch_green_value;
static uint8_t color_switch_blue_value;

static void update_rgbw_led(void)
{
  DPRINTF("%s Setting RGB=(%u,%u,%u)\n", __func__,
          (color_switch_red_value * multilevel_switch_value) / multilevel_switch_max,
          (color_switch_green_value * multilevel_switch_value) / multilevel_switch_max,
          (color_switch_blue_value * multilevel_switch_value) / multilevel_switch_max);
  sl_led_set_rgb_color(&sl_simple_rgb_pwm_led_led,
                        (uint16_t)((color_switch_red_value * multilevel_switch_value) / multilevel_switch_max),
                        (uint16_t)((color_switch_green_value * multilevel_switch_value) / multilevel_switch_max),
                        (uint16_t)((color_switch_blue_value * multilevel_switch_value) / multilevel_switch_max));
}

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

void LEDBulb_hw_init(uint8_t multilevel_switch_max_, uint8_t color_switch_max_)
{
  DPRINT("--------------------------------\n");
  DPRINTF("%s: Toggle learn mode\n", Board_GetButtonLabel(APP_BUTTON_LEARN_RESET));
  DPRINT("      Hold 5 sec: Reset\n");
  DPRINTF("%s: Learn mode + identify\n", Board_GetLedLabel(APP_LED_INDICATOR));
  DPRINT("--------------------------------\n\n");

  Board_SetButtonCallback(button_handler);
  Board_EnableButton(APP_BUTTON_LEARN_RESET);

  multilevel_switch_max = multilevel_switch_max_;
  color_switch_max = color_switch_max_;
}

void LEDBulb_hw_callback_RED(s_colorComponent * colorComponent)
{
  color_switch_red_value = ZAF_Actuator_GetCurrentValue(&colorComponent->obj);
  update_rgbw_led();
}

void LEDBulb_hw_callback_GREEN(s_colorComponent * colorComponent)
{
  color_switch_green_value = ZAF_Actuator_GetCurrentValue(&colorComponent->obj);
  update_rgbw_led();
}
void LEDBulb_hw_callback_BLUE(s_colorComponent * colorComponent)
{
  color_switch_blue_value = ZAF_Actuator_GetCurrentValue(&colorComponent->obj);
  update_rgbw_led();
}

void LEDBulb_hw_multilevel_switch_handler(cc_multilevel_switch_t * p_switch)
{
  multilevel_switch_value = cc_multilevel_switch_get_current_value(p_switch);
  update_rgbw_led();
}
