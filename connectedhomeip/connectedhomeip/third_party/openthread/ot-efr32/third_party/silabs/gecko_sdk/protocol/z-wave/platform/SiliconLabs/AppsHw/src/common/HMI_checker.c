/**
 * Z-Wave Certified Human Machine Interface checker.
 * Used to test LEDs and Buttons on all boards.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <Assert.h>
#include <misc.h>
#include <em_gpio.h>

#include "board.h"
#include "board_indicator.h"
#include "target_boards.h"

#include <sl_simple_rgbw_pwm_led.h>
#include "sl_led.h"

//#define DEBUGPRINT
#include "DebugPrint.h"

/****************************************************************************/
/*                            STATIC FUNCTIONS                              */
/****************************************************************************/

static bool m_checking_enabled = false;

/**
 * Uncomment the section below, once sl_simple_rgbw_pwm_led.c can be build
 * for series2.
 * Do it as part of SWPROT-5569.
 */

/* Setup data needed for led driver.
 *  Note that R,G,B colors are supported, but W is not, so for white color pin is set to the value that is not in use*/
//static sl_simple_rgbw_pwm_led_context_t simple_rgbw_pwm_led_context = {
//  .port[SL_SIMPLE_RGBW_PWM_LED_COLOR_R] = RGB1_R_GPIO_PORT,
//  .pin[SL_SIMPLE_RGBW_PWM_LED_COLOR_R] = RGB1_R_GPIO_PIN,
//  .polarity[SL_SIMPLE_RGBW_PWM_LED_COLOR_R] = SL_SIMPLE_RGBW_PWM_LED_POLARITY_ACTIVE_LOW,
//  .channel[SL_SIMPLE_RGBW_PWM_LED_COLOR_R] = 0,
//  .location[SL_SIMPLE_RGBW_PWM_LED_COLOR_R] = 18,
//
//  .port[SL_SIMPLE_RGBW_PWM_LED_COLOR_G] = RGB1_G_GPIO_PORT,
//  .pin[SL_SIMPLE_RGBW_PWM_LED_COLOR_G] = RGB1_G_GPIO_PIN,
//  .polarity[SL_SIMPLE_RGBW_PWM_LED_COLOR_G] = SL_SIMPLE_RGBW_PWM_LED_POLARITY_ACTIVE_LOW,
//  .channel[SL_SIMPLE_RGBW_PWM_LED_COLOR_G] = 1,
//  .location[SL_SIMPLE_RGBW_PWM_LED_COLOR_G] = 18,
//
//  .port[SL_SIMPLE_RGBW_PWM_LED_COLOR_B] = RGB1_B_GPIO_PORT,
//  .pin[SL_SIMPLE_RGBW_PWM_LED_COLOR_B] = RGB1_B_GPIO_PIN,
//  .polarity[SL_SIMPLE_RGBW_PWM_LED_COLOR_B] = SL_SIMPLE_RGBW_PWM_LED_POLARITY_ACTIVE_LOW,
//  .channel[SL_SIMPLE_RGBW_PWM_LED_COLOR_B] = 2,
//  .location[SL_SIMPLE_RGBW_PWM_LED_COLOR_B] = 18,
//
//  .port[SL_SIMPLE_RGBW_PWM_LED_COLOR_W] = gpioPortD,  // We need a define for this...
//  .pin[SL_SIMPLE_RGBW_PWM_LED_COLOR_W] = 9,
//  .polarity[SL_SIMPLE_RGBW_PWM_LED_COLOR_W] = SL_SIMPLE_RGBW_PWM_LED_POLARITY_ACTIVE_HIGH,
//  .channel[SL_SIMPLE_RGBW_PWM_LED_COLOR_W] = 3,
//  .location[SL_SIMPLE_RGBW_PWM_LED_COLOR_W] = 14,
//
//  .timer = TIMER1,
//  .frequency = 10000,
//  .resolution = 100
//};
//static const sl_led_rgbw_pwm_t m_sl_led = {
//  .led_common.context = &simple_rgbw_pwm_led_context,
//  .led_common.init = sl_simple_rgbw_pwm_led_init,
//  .led_common.turn_on = sl_simple_rgbw_pwm_led_turn_on,
//  .led_common.turn_off = sl_simple_rgbw_pwm_led_turn_off,
//  .led_common.toggle = sl_simple_rgbw_pwm_led_toggle,
//  .led_common.get_state = sl_simple_rgbw_pwm_led_get_state,
//  .set_rgbw_color = sl_simple_rgbw_pwm_led_set_color,
//  .get_rgbw_color = sl_simple_rgbw_pwm_led_get_color,
//};


/**
 * Turns on all available LEDs on all boards to check them and their connection across boards.
 */
static void checkLEDsOnEXPBoard()
{
  Board_SetLed(BOARD_LED1, LED_ON);
  Board_SetLed(BOARD_LED2, LED_ON);
  Board_SetLed(BOARD_LED3, LED_ON);
  Board_SetLed(BOARD_LED4, LED_ON);

//  sl_led_set_rgbw_color(&m_sl_led, 100, 100, 100, 0);
}

/****************************************************************************/
/*                               API FUNCTIONS                              */
/****************************************************************************/

void HMIChecker_buttonEventHandler(BUTTON_EVENT btnEvent)
{
  if (btnEvent == EVENT_PB1_HOLD)
  {
    DPRINT("    HMI Checker enabled! \n");
    DPRINT("      Now press different buttons to verify functionality... \n");
    DPRINT("      Reset device to disable this feature. \n");
    m_checking_enabled = true;
  }

  // Continue only if enabled through press and holding BTN3
  if (!m_checking_enabled)
  {
    return;
  }

  switch (btnEvent)
  {
    case EVENT_PB1_DOWN:            DPRINT("    BTN0 DOWN \n"); break;
    case EVENT_PB2_DOWN:            DPRINT("    BTN1 DOWN \n"); break;
    case EVENT_PB3_DOWN:            DPRINT("    BTN2 DOWN \n"); break;
    case EVENT_PB4_DOWN:            DPRINT("    BTN3 DOWN \n"); break;
    case EVENT_PB5_DOWN:            DPRINT("    BTN4 DOWN \n"); break;
    case EVENT_PB6_DOWN:            DPRINT("    BTN5 DOWN \n"); break;
    case EVENT_SLIDER1_DOWN:        DPRINT("    SLIDER1 DOWN \n"); break;

    case EVENT_PB1_UP:              DPRINT("    BTN0 UP \n"); break;
    case EVENT_PB2_UP:              DPRINT("    BTN1 UP \n"); break;
    case EVENT_PB3_UP:              DPRINT("    BTN2 UP \n"); break;
    case EVENT_PB4_UP:              DPRINT("    BTN3 UP \n"); break;
    case EVENT_PB5_UP:              DPRINT("    BTN4 UP \n"); break;
    case EVENT_PB6_UP:              DPRINT("    BTN5 UP \n"); break;
    case EVENT_SLIDER1_UP:          DPRINT("    SLIDER1 UP \n"); break;

    case EVENT_PB1_SHORT_PRESS:     DPRINT("    BTN0 SHORT PRESS \n"); break;
    case EVENT_PB2_SHORT_PRESS:     DPRINT("    BTN1 SHORT PRESS \n"); break;
    case EVENT_PB3_SHORT_PRESS:     DPRINT("    BTN2 SHORT PRESS \n"); break;
    case EVENT_PB4_SHORT_PRESS:     DPRINT("    BTN3 SHORT PRESS \n"); break;
    case EVENT_PB5_SHORT_PRESS:     DPRINT("    BTN4 SHORT PRESS \n"); break;
    case EVENT_PB6_SHORT_PRESS:     DPRINT("    BTN5 SHORT PRESS \n"); break;
    case EVENT_SLIDER1_SHORT_PRESS: DPRINT("    SLIDER1 SHORT PRESS \n"); break;

    case EVENT_PB1_HOLD:            DPRINT("    BTN0 HOLD \n");
      checkLEDsOnEXPBoard();
      break;
    case EVENT_PB2_HOLD:            DPRINT("    BTN1 HOLD \n"); break;
    case EVENT_PB3_HOLD:            DPRINT("    BTN2 HOLD \n"); break;
    case EVENT_PB4_HOLD:            DPRINT("    BTN3 HOLD \n"); break;
    case EVENT_PB5_HOLD:            DPRINT("    BTN4 HOLD \n"); break;
    case EVENT_PB6_HOLD:            DPRINT("    BTN5 HOLD \n"); break;
    case EVENT_SLIDER1_HOLD:        DPRINT("    SLIDER1 HOLD \n"); break;

    case EVENT_PB1_LONG_PRESS:      DPRINT("    BTN0 LONG PRESS \n"); break;
    case EVENT_PB2_LONG_PRESS:      DPRINT("    BTN1 LONG PRESS \n"); break;
    case EVENT_PB3_LONG_PRESS:      DPRINT("    BTN2 LONG PRESS \n"); break;
    case EVENT_PB4_LONG_PRESS:      DPRINT("    BTN3 LONG PRESS \n"); break;
    case EVENT_PB5_LONG_PRESS:      DPRINT("    BTN4 LONG PRESS \n"); break;
    case EVENT_PB6_LONG_PRESS:      DPRINT("    BTN5 LONG PRESS \n"); break;
    case EVENT_SLIDER1_LONG_PRESS:  DPRINT("    SLIDER1 LONG PRESS \n"); break;

    default:
      // Check whether all button events are handled.
      ASSERT(!(btnEvent >= EVENT_PB1_DOWN && btnEvent < EVENT_BTN_MAX));
      break;
  }
}


/**
 * Initializes the module.
 */
void HMIChecker_Init()
{
//  sl_led_init((sl_led_t *)&m_sl_led);

  Board_IndicatorInitLed(BOARD_LED1);
  Board_IndicatorInitLed(BOARD_LED2);
  Board_IndicatorInitLed(BOARD_LED3);
  Board_IndicatorInitLed(BOARD_LED4);

  Board_EnableButton(BOARD_BUTTON_PB1);
  Board_EnableButton(BOARD_BUTTON_PB2);
  Board_EnableButton(BOARD_BUTTON_PB3);
  Board_EnableButton(BOARD_BUTTON_PB4);
  Board_EnableButton(BOARD_BUTTON_PB5);
  Board_EnableButton(BOARD_BUTTON_PB6);
  Board_EnableButton(BOARD_BUTTON_SLIDER1);
}
