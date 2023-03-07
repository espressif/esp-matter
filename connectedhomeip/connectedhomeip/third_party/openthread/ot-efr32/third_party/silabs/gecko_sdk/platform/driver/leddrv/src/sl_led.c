/***************************************************************************//**
 * @file
 * @brief LED Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_led.h"

sl_status_t sl_led_init(const sl_led_t *led_handle)
{
  return led_handle->init(led_handle->context);
}

void sl_led_turn_on(const sl_led_t *led_handle)
{
  led_handle->turn_on(led_handle->context);
}

void sl_led_turn_off(const sl_led_t *led_handle)
{
  led_handle->turn_off(led_handle->context);
}

void sl_led_toggle(const sl_led_t *led_handle)
{
  led_handle->toggle(led_handle->context);
}

sl_led_state_t sl_led_get_state(const sl_led_t *led_handle)
{
  return led_handle->get_state(led_handle->context);
}
