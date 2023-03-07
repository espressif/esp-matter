/***************************************************************************//**
 * @file
 * @brief RGB LED driver for BRD4166A
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

#include "em_gpio.h"
#include "sl_simple_rgb_pwm_led_instances.h"
#include "sl_simple_rgb_pwm_led.h"
#include "app_log.h"
#include "board.h"

// -----------------------------------------------------------------------------
// Private variables

//  Array to linearize the light level of the RGB LEDs
static const uint8_t light_levels[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05,
  0x05, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x09, 0x09, 0x0A, 0x0A, 0x0B, 0x0B,
  0x0C, 0x0C, 0x0D, 0x0D, 0x0E, 0x0F, 0x0F, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x26, 0x27, 0x29, 0x2B, 0x2C,
  0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E, 0x40, 0x43, 0x45, 0x47, 0x4A, 0x4C, 0x4F,
  0x51, 0x54, 0x57, 0x59, 0x5C, 0x5F, 0x62, 0x64, 0x67, 0x6A, 0x6D, 0x70, 0x73, 0x76, 0x79, 0x7C,
  0x7F, 0x82, 0x85, 0x88, 0x8B, 0x8E, 0x91, 0x94, 0x97, 0x9A, 0x9C, 0x9F, 0xA2, 0xA5, 0xA7, 0xAA,
  0xAD, 0xAF, 0xB2, 0xB4, 0xB7, 0xB9, 0xBB, 0xBE, 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
  0xD0, 0xD2, 0xD3, 0xD5, 0xD7, 0xD8, 0xDA, 0xDB, 0xDD, 0xDE, 0xDF, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5,
  0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xED, 0xEE, 0xEF, 0xEF, 0xF0, 0xF1, 0xF1, 0xF2,
  0xF2, 0xF3, 0xF3, 0xF4, 0xF4, 0xF5, 0xF5, 0xF6, 0xF6, 0xF6, 0xF7, 0xF7, 0xF7, 0xF8, 0xF8, 0xF8,
  0xF9, 0xF9, 0xF9, 0xF9, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFC,
  0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD,
  0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF
};

// -----------------------------------------------------------------------------
// Private function definitions

static void rgb_led_enable(bool enable, uint8_t mask)
{
  // Make sure the mask corresponds to the RGB LEDs of the board.
  if (mask != (mask & BOARD_RGBLED_MASK)) {
    mask = mask & BOARD_RGBLED_MASK;
    app_log_error("Undefined RGB LED is trying to be set." APP_LOG_NL);
  }

  if ((mask != 0) && enable) {
    // Enable RGB LED power if any of the LEDs are enabled.
    GPIO_PinOutSet(BOARD_RGBLED_PWR_EN_PORT, BOARD_RGBLED_PWR_EN_PIN);
  } else if ((mask == BOARD_RGBLED_MASK) && !enable) {
    // Disable RGB LED power if all of the LEDs are disabled.
    GPIO_PinOutClear(BOARD_RGBLED_PWR_EN_PORT, BOARD_RGBLED_PWR_EN_PIN);
  }

  uint8_t pins[BOARD_RGBLED_COUNT] = { BOARD_RGBLED_COM0_PIN,
                                       BOARD_RGBLED_COM1_PIN,
                                       BOARD_RGBLED_COM2_PIN,
                                       BOARD_RGBLED_COM3_PIN };

  for (int i = 0; i < BOARD_RGBLED_COUNT; i++) {
    if (((1 << i) & mask) != 0) {
      if (enable) {
        GPIO_PinOutSet(BOARD_RGBLED_COM_PORT, pins[i]);
      } else {
        GPIO_PinOutClear(BOARD_RGBLED_COM_PORT, pins[i]);
      }
    }
  }

  return;
}

// -----------------------------------------------------------------------------
// Public function definitions

void rgb_led_init(void)
{
  GPIO_PinModeSet(BOARD_RGBLED_PWR_EN_PORT,
                  BOARD_RGBLED_PWR_EN_PIN,
                  gpioModePushPull,
                  0);
  GPIO_PinModeSet(BOARD_RGBLED_COM0_PORT,
                  BOARD_RGBLED_COM0_PIN,
                  gpioModePushPull,
                  0);
  GPIO_PinModeSet(BOARD_RGBLED_COM1_PORT,
                  BOARD_RGBLED_COM1_PIN,
                  gpioModePushPull,
                  0);
  GPIO_PinModeSet(BOARD_RGBLED_COM2_PORT,
                  BOARD_RGBLED_COM2_PIN,
                  gpioModePushPull,
                  0);
  GPIO_PinModeSet(BOARD_RGBLED_COM3_PORT,
                  BOARD_RGBLED_COM3_PIN,
                  gpioModePushPull,
                  0);
}

void rgb_led_deinit(void)
{
  rgb_led_enable(false, BOARD_RGBLED_MASK);
}

void rgb_led_set(uint8_t m, uint8_t r, uint8_t g, uint8_t b)
{
  rgb_led_enable(false, (~m & BOARD_RGBLED_MASK));
  rgb_led_enable(true, m);
  sl_led_set_rgb_color(&sl_simple_rgb_pwm_led_rgb_led0,
                       light_levels[r],
                       light_levels[g],
                       light_levels[b]);
}
