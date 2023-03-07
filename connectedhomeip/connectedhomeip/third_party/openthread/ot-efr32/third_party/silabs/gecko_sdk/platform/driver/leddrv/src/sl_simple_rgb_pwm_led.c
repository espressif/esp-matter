/***************************************************************************//**
 * @file
 * @brief Simple RGB PWM LED Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stddef.h>

#include "sl_pwm_led.h"
#include "sl_simple_rgb_pwm_led.h"

/*
 *  Public functions
 */
sl_status_t sl_simple_rgb_pwm_led_init(void *rgb)
{
  sl_status_t status;
  sl_simple_rgb_pwm_led_context_t *context = (sl_simple_rgb_pwm_led_context_t *)rgb;

  status = sl_pwm_led_init(context->red);
  if (status == SL_STATUS_FAIL) {
    return SL_STATUS_FAIL;
  }
  status = sl_pwm_led_init(context->green);
  if (status == SL_STATUS_FAIL) {
    return SL_STATUS_FAIL;
  }
  status = sl_pwm_led_init(context->blue);
  if (status == SL_STATUS_FAIL) {
    return SL_STATUS_FAIL;
  }

  context->state = SL_LED_CURRENT_STATE_OFF;

  return status;
}

void sl_simple_rgb_pwm_led_turn_on(void *rgb)
{
  sl_simple_rgb_pwm_led_context_t *context = (sl_simple_rgb_pwm_led_context_t *)rgb;

  sl_pwm_led_start(context->red);
  sl_pwm_led_start(context->green);
  sl_pwm_led_start(context->blue);

  context->state = SL_LED_CURRENT_STATE_ON;
}

void sl_simple_rgb_pwm_led_turn_off(void *rgb)
{
  sl_simple_rgb_pwm_led_context_t *context = (sl_simple_rgb_pwm_led_context_t *)rgb;

  sl_pwm_led_stop(context->red);
  sl_pwm_led_stop(context->green);
  sl_pwm_led_stop(context->blue);

  context->state = SL_LED_CURRENT_STATE_OFF;
}

void sl_simple_rgb_pwm_led_toggle(void *rgb)
{
  sl_simple_rgb_pwm_led_context_t *context = (sl_simple_rgb_pwm_led_context_t *)rgb;
  if (sl_simple_rgb_pwm_led_get_state(context) == SL_LED_CURRENT_STATE_OFF) {
    sl_simple_rgb_pwm_led_turn_on(context);
  } else {
    sl_simple_rgb_pwm_led_turn_off(context);
  }
}

sl_led_state_t sl_simple_rgb_pwm_led_get_state(void *rgb)
{
  sl_simple_rgb_pwm_led_context_t *context = (sl_simple_rgb_pwm_led_context_t *)rgb;

  return context->state;
}

void sl_simple_rgb_pwm_led_set_color(void *rgb, uint16_t red, uint16_t green, uint16_t blue)
{
  sl_simple_rgb_pwm_led_context_t *context = (sl_simple_rgb_pwm_led_context_t *)rgb;

  context->red->set_color(context->red, red);
  context->red->set_color(context->green, green);
  context->red->set_color(context->blue, blue);
}

void sl_simple_rgb_pwm_led_get_color(void *rgb, uint16_t *red, uint16_t *green, uint16_t *blue)
{
  sl_simple_rgb_pwm_led_context_t *context = (sl_simple_rgb_pwm_led_context_t *)rgb;

  context->red->get_color(context->red, red);
  context->red->get_color(context->green, green);
  context->red->get_color(context->blue, blue);
}

/*
 *  API functions (extentions to the Common LED Driver API)
 */
void sl_led_set_rgb_color(const sl_led_rgb_pwm_t *led_handle, uint16_t red, uint16_t green, uint16_t blue)
{
  led_handle->set_rgb_color(led_handle->led_common.context, red, green, blue);
}

void sl_led_get_rgb_color(const sl_led_rgb_pwm_t *led_handle, uint16_t *red, uint16_t *green, uint16_t *blue)
{
  led_handle->get_rgb_color(led_handle->led_common.context, red, green, blue);
}
