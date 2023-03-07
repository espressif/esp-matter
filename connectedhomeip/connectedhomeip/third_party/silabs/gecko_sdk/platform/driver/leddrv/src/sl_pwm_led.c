/***************************************************************************//**
 * @file
 * @brief PWM LED driver implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include <stddef.h>

#include "sl_pwm_led.h"

/*
 *  Private functions
 */
static CMU_Clock_TypeDef pwm_led_get_timer_clock(TIMER_TypeDef *timer)
{
#if defined(_CMU_HFCLKSEL_MASK) || defined(_CMU_CMD_HFCLKSEL_MASK)
  CMU_Clock_TypeDef timer_clock = cmuClock_HF;
#elif defined(_CMU_SYSCLKCTRL_MASK)
  CMU_Clock_TypeDef timer_clock = cmuClock_SYSCLK;
#else
#error "Unknown root of clock tree"
#endif

  switch ((uint32_t)timer) {
#if defined(TIMER0_BASE)
    case TIMER0_BASE:
      timer_clock = cmuClock_TIMER0;
      break;
#endif
#if defined(TIMER1_BASE)
    case TIMER1_BASE:
      timer_clock = cmuClock_TIMER1;
      break;
#endif
#if defined(TIMER2_BASE)
    case TIMER2_BASE:
      timer_clock = cmuClock_TIMER2;
      break;
#endif
#if defined(TIMER3_BASE)
    case TIMER3_BASE:
      timer_clock = cmuClock_TIMER3;
      break;
#endif
#if defined(TIMER4_BASE)
    case TIMER4_BASE:
      timer_clock = cmuClock_TIMER4;
      break;
#endif
#if defined(TIMER5_BASE)
    case TIMER5_BASE:
      timer_clock = cmuClock_TIMER5;
      break;
#endif
#if defined(TIMER6_BASE)
    case TIMER6_BASE:
      timer_clock = cmuClock_TIMER6;
      break;
#endif
#if defined(WTIMER0_BASE)
    case WTIMER0_BASE:
      timer_clock = cmuClock_WTIMER0;
      break;
#endif
#if defined(WTIMER1_BASE)
    case WTIMER1_BASE:
      timer_clock = cmuClock_WTIMER1;
      break;
#endif
#if defined(WTIMER2_BASE)
    case WTIMER2_BASE:
      timer_clock = cmuClock_WTIMER2;
      break;
#endif
#if defined(WTIMER3_BASE)
    case WTIMER3_BASE:
      timer_clock = cmuClock_WTIMER3;
      break;
#endif
    default:
      EFM_ASSERT(0);
      break;
  }
  return timer_clock;
}

void sl_pwm_led_start(void *led_handler)
{
  sl_led_pwm_t *led = (sl_led_pwm_t *)led_handler;
  // Enable PWM output
#if defined(_TIMER_ROUTE_MASK)
  BUS_RegMaskedSet(&led->timer->ROUTE,
                   1 << (led->channel + _TIMER_ROUTE_CC0PEN_SHIFT));
#elif defined(_TIMER_ROUTELOC0_MASK)
  BUS_RegMaskedSet(&led->timer->ROUTEPEN,
                   1 << (led->channel + _TIMER_ROUTEPEN_CC0PEN_SHIFT));
#elif defined(_GPIO_TIMER_ROUTEEN_MASK)
  GPIO->TIMERROUTE_SET[TIMER_NUM(led->timer)].ROUTEEN = 1 << (led->channel + _GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT);
#else
#error "Unknown route setting"
#endif
}

void sl_pwm_led_stop(void *led_handler)
{
  sl_led_pwm_t *led = (sl_led_pwm_t *)led_handler;
  // Disable PWM output
#if defined(_TIMER_ROUTE_MASK)
  BUS_RegMaskedClear(&led->timer->ROUTE,
                     1 << (led->channel + _TIMER_ROUTE_CC0PEN_SHIFT));
#elif defined(_TIMER_ROUTELOC0_MASK)
  BUS_RegMaskedClear(&led->timer->ROUTEPEN,
                     1 << (led->channel + _TIMER_ROUTEPEN_CC0PEN_SHIFT));
#elif defined(_GPIO_TIMER_ROUTEEN_MASK)
  GPIO->TIMERROUTE_CLR[TIMER_NUM(led->timer)].ROUTEEN = 1 << (led->channel + _GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT);
#else
#error "Unknown route setting"
#endif
}

/*
 *  Public functions
 */
sl_status_t sl_pwm_led_init(void *led_handler)
{
  sl_led_pwm_t *led = (sl_led_pwm_t *)led_handler;

  // Validate that the selected channel exist in the selected timer.
  EFM_ASSERT(TIMER_REF_VALID(led->timer));
  EFM_ASSERT(TIMER_CH_VALID(led->channel));

  led->set_color = sl_pwm_led_set_color;
  led->get_color = sl_pwm_led_get_color;

#if !defined(_SILICON_LABS_32B_SERIES_2)
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif
  CMU_Clock_TypeDef timer_clock = pwm_led_get_timer_clock(led->timer);
  CMU_ClockEnable(timer_clock, true);

  TIMER_InitCC_TypeDef channel_init = TIMER_INITCC_DEFAULT;
  channel_init.mode = timerCCModePWM;
  channel_init.cmoa = timerOutputActionToggle;
  channel_init.edge = timerEdgeBoth;

#if defined(_SILICON_LABS_32B_SERIES_2)
  led->timer->EN_SET = TIMER_EN_EN;
#endif

  // The configured resolution configuration has to be at least 2. And not bigger than the timer max count + 1
  uint32_t max_count = TIMER_MaxCount(led->timer);
  if ((led->resolution < 2UL) || (led->resolution > (max_count))) {
    // The user configured max level value is invalid
#if !defined(_SILICON_LABS_32B_SERIES_2)
    CMU_ClockEnable(cmuClock_HFPER, false);
#endif
    CMU_ClockEnable(timer_clock, false);

    return SL_STATUS_FAIL;
  }

  // User configuration values are OK. Continue with the initialization...

  // Set CC channel parameters
  channel_init.outInvert = !!led->polarity;
  TIMER_InitCC(led->timer, led->channel, &channel_init);

  // Set PWM pin as output
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet((GPIO_Port_TypeDef)led->port,
                  led->pin,
                  gpioModePushPull,
                  led->polarity);

  // Configure CC channel pinout
#if defined(_TIMER_ROUTE_MASK)
  BUS_RegMaskedWrite(&led->timer->ROUTE,
                     _TIMER_ROUTE_LOCATION_MASK,
                     led->location << _TIMER_ROUTE_LOCATION_SHIFT);
}
#elif defined(_TIMER_ROUTELOC0_MASK)
  BUS_RegMaskedWrite(&led->timer->ROUTELOC0,
                     _TIMER_ROUTELOC0_CC0LOC_MASK << (led->channel * 8U),
                     led->location << (led->channel * 8U));
#elif defined(_GPIO_TIMER_ROUTEEN_MASK)
  volatile uint32_t * route_register = &GPIO->TIMERROUTE[TIMER_NUM(led->timer)].CC0ROUTE;
  *(route_register + led->channel) = (led->port << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                                     | (led->pin << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);
#else
#error "Unknown route setting"
#endif

// Configure the Timer TOP value on basis of the application PWM frequency and resolution config values
uint32_t top = 0;
// First find the TOP value that satisfies the PWM frequency config (if not set to 0 = "don't care")
if (led->frequency > 0U) {
  top = (CMU_ClockFreqGet(timer_clock) / (led->frequency)) - 1U;
}

// Then adjust it to make it an even multiplier of the PWM resolution config
top = (top / (led->resolution - 1)) * (led->resolution - 1);

if (top < (led->resolution - 1)) {
  // The PWM resolution config demands a higher TOP value than what the frequency config sets.
  // In this case the resolution config takes precedence over frequency.
  // So simply set the TOP value equal to the max input value (resolution value - 1).
  top = led->resolution - 1;
}
TIMER_TopSet(led->timer, top);

// Initialize TIMER
TIMER_Init_TypeDef timer_init = TIMER_INIT_DEFAULT;
TIMER_Init(led->timer, &timer_init);

// Initialize the level settings values
sl_pwm_led_set_color(led, led->level);

// Set LED state to Off.
sl_pwm_led_stop(led);

return SL_STATUS_OK;
}

void sl_pwm_led_set_color(void *led_handler, uint16_t color)
{
  sl_led_pwm_t *led = (sl_led_pwm_t *)led_handler;
  // Get the counter increments per intensity level step
  uint32_t level_increments = TIMER_TopGet(led->timer) / (led->resolution - 1);
  led->level = color;

  sl_pwm_led_stop(led);
  TIMER_CompareBufSet(led->timer, led->channel, led->level * level_increments);
  sl_pwm_led_start(led);
}

void sl_pwm_led_get_color(void *led_handler, uint16_t *color)
{
  sl_led_pwm_t *led = (sl_led_pwm_t *)led_handler;
  *color   = led->level;
}
