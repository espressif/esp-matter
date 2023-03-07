/***************************************************************************//**
 * @file
 * @brief LED Application code
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app_led.h"
#include "sl_btmesh.h"
#include "sl_btmesh_lighting_server.h"
#include "sl_btmesh_lighting_server_config.h"
#include "sl_pwm_init_led0_config.h"
#include "sl_pwm_init_led1_config.h"

#include "sl_pwm.h"
#include "sl_pwm_instances.h"
#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"

// -----------------------------------------------------------------------------
// Definitions

/// 100% PWM duty cycle
#define PWM_MAX_DUTY_CYCLE 100

// -----------------------------------------------------------------------------
// Public function definitions

/*******************************************************************************
 * Sets the lightness level of the LED.
 *
 * @param[in] level  lightness level (0-65535)
 *
 ******************************************************************************/
void app_led_set_level(uint16_t level)
{
  uint16_t pwm_duty_cycle = (uint16_t)((uint32_t)level * PWM_MAX_DUTY_CYCLE
                                       / SL_BTMESH_LIGHTING_SERVER_PWM_MAXIMUM_BRIGHTNESS_CFG_VAL);

  if (pwm_duty_cycle > PWM_MAX_DUTY_CYCLE) {
    pwm_duty_cycle = PWM_MAX_DUTY_CYCLE;
  }
  sl_pwm_set_duty_cycle(&sl_pwm_led0, pwm_duty_cycle);
  sl_pwm_set_duty_cycle(&sl_pwm_led1, pwm_duty_cycle);
}

/*******************************************************************************
 * Sets the hue of the LED if present.
 *
 * @param[in] hue  hue level (0-65535)
 *
 ******************************************************************************/
void app_led_set_hue(uint16_t hue)
{
  (void)hue;
}

/*******************************************************************************
 * Sets the saturation of the LED if present.
 *
 * @param[in] saturation  saturation level (0-65535)
 *
 ******************************************************************************/
void app_led_set_saturation(uint16_t saturation)
{
  (void)saturation;
}

/*******************************************************************************
 * Gets the maximum lightness value
 *
 * @return maximum lightness value.
 ******************************************************************************/
uint16_t app_led_get_max(void)
{
  return SL_BTMESH_LIGHTING_SERVER_PWM_MAXIMUM_BRIGHTNESS_CFG_VAL;
}

/*******************************************************************************
 * Init LED drivers
 *
 ******************************************************************************/
void app_led_init(void)
{
  // Enable PWM output
  sl_pwm_start(&sl_pwm_led0);
  // Enable PWM output
  sl_pwm_start(&sl_pwm_led1);
}

/*******************************************************************************
 * Change buttons to leds in case of shared pin
 *
 ******************************************************************************/
void app_led_change_buttons_to_leds(void)
{
  sl_simple_button_disable(&sl_button_btn0);
  sl_pwm_config_t pwm_led0_config = {
    .frequency = SL_PWM_LED0_FREQUENCY,
    .polarity = SL_PWM_LED0_POLARITY,
  };
  sl_pwm_init(&sl_pwm_led0, &pwm_led0_config);
  sl_pwm_start(&sl_pwm_led0);
  sl_simple_button_disable(&sl_button_btn1);

  sl_pwm_config_t pwm_led1_config = {
    .frequency = SL_PWM_LED1_FREQUENCY,
    .polarity = SL_PWM_LED1_POLARITY,
  };
  sl_pwm_init(&sl_pwm_led1, &pwm_led1_config);
  sl_pwm_start(&sl_pwm_led1);
}
