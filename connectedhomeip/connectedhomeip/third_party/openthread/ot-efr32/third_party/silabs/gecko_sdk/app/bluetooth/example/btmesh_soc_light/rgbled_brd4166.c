/***************************************************************************//**
 * @file
 * @brief RGB LED driver for BRD4166A
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

#include "sl_simple_rgb_pwm_led_instances.h"
#include "sl_simple_rgb_pwm_led.h"
#include "em_gpio.h"
#include "rgbled_brd4166.h"

// -----------------------------------------------------------------------------
// Private function definitions

/***************************************************************************//**
 * Enable RGB LEDs with respect to the enabling bitmask.
 *
 * @param[in] enable  Enable or disable the selected LEDs.
 * @param[in] mask    LED instance mask.
 ******************************************************************************/
static void rgb_led_enable(bool enable, uint8_t mask)
{
  if ( ( (mask != 0) && (enable == true) )
       || ( ( (mask & 0xf) == 0xf) && (enable == false) ) ) {
    if ( enable ) {
      GPIO_PinOutSet(BOARD_RGBLED_PWR_EN_PORT, BOARD_RGBLED_PWR_EN_PIN);
    } else {
      GPIO_PinOutClear(BOARD_RGBLED_PWR_EN_PORT, BOARD_RGBLED_PWR_EN_PIN);
    }
  }

  int i;
  uint8_t pins[BOARD_RGBLED_COUNT] = { BOARD_RGBLED_COM0_PIN,
                                       BOARD_RGBLED_COM1_PIN,
                                       BOARD_RGBLED_COM2_PIN,
                                       BOARD_RGBLED_COM3_PIN };

  for ( i = 0; i < BOARD_RGBLED_COUNT; i++ ) {
    if ( ( (mask >> i) & 1) == 1 ) {
      if ( enable ) {
        GPIO_PinOutSet(BOARD_RGBLED_COM_PORT,
                       pins[(BOARD_RGBLED_COUNT - 1) - i]);
      } else {
        GPIO_PinOutClear(BOARD_RGBLED_COM_PORT,
                         pins[(BOARD_RGBLED_COUNT - 1) - i]);
      }
    }
  }

  return;
}

// -----------------------------------------------------------------------------
// Public function definitions

/*******************************************************************************
 * Initialize all LEDs.
 ******************************************************************************/
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

/*******************************************************************************
 * De-initialize all LEDs by disabling them.
 ******************************************************************************/
void rgb_led_deinit(void)
{
  rgb_led_enable(false, 0xf);
}

/*******************************************************************************
 * Set LED color based on RGB components.
 *
 * @param[in] mask         LED instance mask
 * @param[in] r            Red component
 * @param[in] g            Green component
 * @param[in] b            Blue component
 ******************************************************************************/
void rgb_led_set_rgb(uint8_t m, uint8_t r, uint8_t g, uint8_t b)
{
  rgb_led_enable(false, ~m);
  rgb_led_enable(true, m);
  sl_led_set_rgb_color(&sl_simple_rgb_pwm_led_rgb_led0,
                       (uint16_t)r,
                       (uint16_t)g,
                       (uint16_t)b);
}
