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
#include "sl_btmesh_hsl_server.h"
#include <math.h>

#ifdef SL_BTMESH_HSL_RGB_BRD4166
#include "rgbled_brd4166.h"
#elif SL_BTMESH_HSL_RGB_BRD2601
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
static uint16_t hue_level = 0;
static uint16_t saturation_level = 0;

// -----------------------------------------------------------------------------
// Private function declaration

/*******************************************************************************
 * Set LED color based on lightness and temperature.
 *
 * @param[in] m            LED instance mask
 * @param[in] level        Lightness level.
 * @param[in] hue          Hue level.
 * @param[in] saturation   Saturation level.
 ******************************************************************************/
static void rgb_led_set(uint8_t m, uint16_t level, uint16_t hue, uint16_t saturation);

/***************************************************************************//**
 * Helper function for calculating RGB values from hue using helper variables.
 * If hue value passed to the function is increased by 1/3 it returns red value.
 * If hue value passed to the function is unchanged it returns green value.
 * If hue value passed to the function is decreased by 1/3 it returns blue value.
 *
 * @param[in] v1  First helper variable (range 0 to 1).
 * @param[in] v2  Second helper variable (range 0 to 1).
 * @param[in] vH  Hue value.
 *
 * @return Value of red or green or blue.
 ******************************************************************************/
static double Hue_to_RGB(double v1, double v2, double vH);

/***************************************************************************//**
 * Convert color from HSL space to RGB space
 *
 * @param[in] hue        Hue value.
 * @param[in] saturation Saturation value.
 * @param[in] lightness  Lightness value.
 *
 * @return RGB color converted from HSL space.
 ******************************************************************************/
static  RGB_t HSL_to_RGB(uint16_t hue, uint16_t saturation, uint16_t lightness);

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
  rgb_led_set(RGB_LED_MASK, light_level, hue_level, saturation_level);
}

/*******************************************************************************
 * Sets the hue of the LED if present.
 *
 * @param[in] hue  Hue level.
 *
 ******************************************************************************/
void app_led_set_hue(uint16_t hue)
{
  hue_level = hue;
  rgb_led_set(RGB_LED_MASK, light_level, hue_level, saturation_level);
}

/*******************************************************************************
 * Sets the hue of the LED if present.
 *
 * @param[in] hue  Hue level.
 *
 ******************************************************************************/
void app_led_set_saturation(uint16_t saturation)
{
  saturation_level = saturation;
  rgb_led_set(RGB_LED_MASK, light_level, hue_level, saturation_level);
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

static void rgb_led_set(uint8_t m, uint16_t level, uint16_t hue, uint16_t saturation)
{
  RGB_t color = HSL_to_RGB(hue, saturation, level);
  rgb_led_set_rgb(m,
                  color.R,
                  color.G,
                  color.B);
}

static double Hue_to_RGB(double v1, double v2, double vH)
{
  if ( vH < 0) {
    vH += 1;
  }
  if ( vH > 1) {
    vH -= 1;
  }
  if ( (6 * vH) < 1) {
    return (v1 + (v2 - v1) * 6 * vH);
  }
  if ((2 * vH) < 1) {
    return (v2);
  }
  if ((3 * vH) < 2) {
    return (v1 + (v2 - v1) * (2.0 / 3 - vH) * 6);
  }
  return (v1);
}

static RGB_t HSL_to_RGB(uint16_t hue, uint16_t saturation, uint16_t lightness)
{
  RGB_t color;
  double R, G, B;
  double H, S, L;
  double var_1, var_2;

  /* Change the range to (0-1) */
  H = (double)hue / 65535;
  S = (double)saturation / 65535;
  L = (double)lightness / 65535;

  if (S == 0) {
    R = L;
    G = L;
    B = L;
  } else {
    if (L < 0.5) {
      var_2 = L * (1 + S);
    } else {
      var_2 = (L + S) - (S * L);
    }

    var_1 = 2 * L - var_2;

    R = Hue_to_RGB(var_1, var_2, H + 1.0 / 3);
    G = Hue_to_RGB(var_1, var_2, H);
    B = Hue_to_RGB(var_1, var_2, H - 1.0 / 3);
  }

  color.R = (uint8_t)(255.0 * R + 0.5);
  color.G = (uint8_t)(255.0 * G + 0.5);
  color.B = (uint8_t)(255.0 * B + 0.5);
  return color;
}
