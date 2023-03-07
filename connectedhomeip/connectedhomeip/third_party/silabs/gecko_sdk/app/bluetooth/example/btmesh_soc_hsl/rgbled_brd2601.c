/***************************************************************************//**
 * @file
 * @brief RGB LED driver for BRD2601B
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

#include "sl_simple_rgb_pwm_led_instances.h"
#include "sl_simple_rgb_pwm_led.h"
#include "rgbled_brd2601.h"

// -----------------------------------------------------------------------------
// Public function definitions

/*******************************************************************************
 * Initialize all LEDs.
 ******************************************************************************/
void rgb_led_init(void)
{
}

/*******************************************************************************
 * De-initialize all LEDs by disabling them.
 ******************************************************************************/
void rgb_led_deinit(void)
{
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
  (void) m;
  sl_led_set_rgb_color(&sl_simple_rgb_pwm_led_rgb_led0,
                       (uint16_t)r,
                       (uint16_t)g,
                       (uint16_t)b);
}
