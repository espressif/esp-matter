/***************************************************************************//**
 * @file
 * @brief LED Application code
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_btmesh_ctl_server.h"
#include <math.h>

#ifdef SL_BTMESH_LIGHT_RGB_BRD4166
#include "rgbled_brd4166.h"
#elif SL_BTMESH_LIGHT_RGB_BRD2601
#include "rgbled_brd2601.h"
#endif

// -----------------------------------------------------------------------------
// Definitions

#define RGB_LED_MASK        0xF           // Use all LEDs

// -----------------------------------------------------------------------------
// Type definitions

typedef struct {
  uint8_t R;  ///< Red value
  uint8_t G;  ///< Green value
  uint8_t B;  ///< Blue value
}RGB_t;

// -----------------------------------------------------------------------------
// Private variables

static uint16_t light_level = 0;
static uint16_t light_color = 0;

// -----------------------------------------------------------------------------
// Private function declaration

/***************************************************************************//**
 * Set LED color based on lightness and temperature.
 *
 * @param[in] m            LED instance mask
 * @param[in] level        Lightness level.
 * @param[in] temperature  Color temperature in Kelvins.
 ******************************************************************************/
static void rgb_led_set(uint8_t m, uint16_t level, uint16_t temperature);

/***************************************************************************//**
 * Change lightness of given color temperature.
 *
 * @param[in] color  RGB color representing color temperature.
 * @param[in] level  Lightness level of given color.
 *
 * @return RGB color representing given temperature and lightness level.
 ******************************************************************************/
static RGB_t RGB_to_LightnessRGB(RGB_t color, uint16_t level);

/***************************************************************************//**
 * Convert temperature to RGB color using approximation functions.
 *
 * @param[in] temperature  Color temperature in Kelvins.
 *
 * @return RGB color representing given temperature.
 ******************************************************************************/
static RGB_t Temperature_to_RGB(uint16_t temperature);

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
  light_level = level;
  rgb_led_set(RGB_LED_MASK, light_level, light_color);
}

/*******************************************************************************
 * Sets the color temperature of the LED if present.
 *
 * @param[in] color  Color temperature in Kelvins.
 *
 ******************************************************************************/
void app_led_set_color(uint16_t color)
{
  light_color = color;
  rgb_led_set(RGB_LED_MASK, light_level, light_color);
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
  rgb_led_init();
}

/*******************************************************************************
 * Change buttons to leds in case of shared pin
 *
 ******************************************************************************/
void app_led_change_buttons_to_leds(void)
{
}

// -----------------------------------------------------------------------------
// Private function definitions

/***************************************************************************//**
 * Convert temperature to RGB color using approximation functions.
 *
 * @param[in] temperature  Color temperature in Kelvins.
 *
 * @return RGB color representing given temperature.
 ******************************************************************************/
static RGB_t Temperature_to_RGB(uint16_t temperature)
{
  RGB_t color;
  double temp_R, temp_G, temp_B;

  //approximation of temperature using RGB
  if (temperature < 6563) {
    temp_R = 255;
    if (temperature < 1925) {
      temp_B = 0;
    } else {
      temp_B = temperature - 1918.74282;
      temp_B = 2.55822107 * pow(temp_B, 0.546877914);
    }
    if ( temperature < 909) {
      temp_G = 0;
    } else {
      temp_G = temperature - 636.62578769;
      temp_G = 73.13384712 * log(temp_G) - 383.76244858;
    }
  } else {
    temp_R = temperature - 5882.02392431;
    temp_R = -29.28670147 * log(temp_R) + 450.50427359;
    temp_R = temp_R + 0.5;
    temp_G = temperature - 5746.13180276;
    temp_G = -18.69512921 * log(temp_G) + 377.39334366;
    temp_B = 255;
  }

  // Norming
  double temp_max = SL_MAX(temp_R, SL_MAX(temp_G, temp_B));

  temp_R = temp_R * 255 / temp_max;
  temp_G = temp_G * 255 / temp_max;
  temp_B = temp_B * 255 / temp_max;

  color.R = temp_R > 255 ? 255 : (temp_R >= 0 ? (int)(temp_R + 0.5) : 0);
  color.G = temp_G > 255 ? 255 : (temp_G >= 0 ? (int)(temp_G + 0.5) : 0);
  color.B = temp_B > 255 ? 255 : (temp_B >= 0 ? (int)(temp_B + 0.5) : 0);

  return color;
}

static RGB_t RGB_to_LightnessRGB(RGB_t color, uint16_t level)
{
  uint32_t temp_level;
  RGB_t new_color;

  temp_level = color.R * (uint32_t) level / 65535;
  new_color.R = temp_level;
  temp_level = color.G * (uint32_t) level / 65535;
  new_color.G = temp_level;
  temp_level = color.B * (uint32_t) level / 65535;
  new_color.B = temp_level;

  return new_color;
}

static void rgb_led_set(uint8_t m, uint16_t level, uint16_t temperature)
{
  RGB_t color = Temperature_to_RGB(temperature);
  color = RGB_to_LightnessRGB(color, level);
  rgb_led_set_rgb(m,
                  color.R,
                  color.G,
                  color.B);
}
