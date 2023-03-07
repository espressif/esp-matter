/***************************************************************************//**
 * @file
 * @brief Application interface for LEDs
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

#ifndef APP_LED_H
#define APP_LED_H

#include <stdint.h>

/***************************************************************************//**
 * Sets the lightness level of the LED.
 *
 * @param[in] level  lightness level (0-65535)
 *
 ******************************************************************************/
void app_led_set_level(uint16_t level);

/***************************************************************************//**
 * Sets the hue of the LED if present.
 *
 * @param[in] hue  hue level (0-65535)
 *
 ******************************************************************************/
void app_led_set_hue(uint16_t hue);

/***************************************************************************//**
 * Sets the saturation of the LED if present.
 *
 * @param[in] saturation  saturation level (0-65535)
 *
 ******************************************************************************/
void app_led_set_saturation(uint16_t saturation);

/***************************************************************************//**
 * Gets the maximum lightness value
 *
 * @return maximum lightness value.
 ******************************************************************************/
uint16_t app_led_get_max(void);

/***************************************************************************//**
 * Init LED drivers
 *
 ******************************************************************************/
void app_led_init(void);

/***************************************************************************//**
 * Change buttons to leds in case of shared pin
 *
 ******************************************************************************/
void app_led_change_buttons_to_leds(void);

#endif // APP_LED_H
