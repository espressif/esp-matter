/***************************************************************************//**
 * @file
 * @brief Board HW abstraction header for BRD4166A
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

#ifndef BOARD_H
#define BOARD_H

//--------------------------------
#include "sl_simple_led_instances.h"

#define ADV_LED                    SL_SIMPLE_LED_INSTANCE(0)
#define adv_led_turn_on()          sl_led_turn_on(ADV_LED)
#define adv_led_turn_off()         sl_led_turn_off(ADV_LED)
#define adv_led_toggle()           sl_led_toggle(ADV_LED)

//--------------------------------
#define BOARD_EM4WUEN_BTN  sl_button_btn0 // The button to use for EM4 wake-up
#define BOARD_EM4WUEN_NUM  4 // The EM4WU interrupt number of the chosen button. (See the Reference Manual.)
#define BOARD_EM4WUEN_MASK (1 << BOARD_EM4WUEN_NUM) // GPIO pinmask for EM4 wake-up

//--------------------------------
#define BOARD_RGBLED_COUNT         4         // Number of RGB LEDs on the board
#define BOARD_RGBLED_MASK          0x0F      // Bitmask corresponding to the 4 RGB LEDs
#define BOARD_RGBLED_PWR_EN_PORT   gpioPortJ // RGB LED Power Enable port
#define BOARD_RGBLED_PWR_EN_PIN    14        // RGB LED Power Enable pin
#define BOARD_RGBLED_COM_PORT      gpioPortI // RGB LED COM port
#define BOARD_RGBLED_COM0_PORT     gpioPortI // RGB LED COM0 port
#define BOARD_RGBLED_COM0_PIN      0         // RGB LED COM0 pin
#define BOARD_RGBLED_COM1_PORT     gpioPortI // RGB LED COM1 port
#define BOARD_RGBLED_COM1_PIN      1         // RGB LED COM1 pin
#define BOARD_RGBLED_COM2_PORT     gpioPortI // RGB LED COM2 port
#define BOARD_RGBLED_COM2_PIN      2         // RGB LED COM2 pin
#define BOARD_RGBLED_COM3_PORT     gpioPortI // RGB LED COM3 port
#define BOARD_RGBLED_COM3_PIN      3         // RGB LED COM3 pin

void rgb_led_init(void);
void rgb_led_deinit(void);
void rgb_led_set(uint8_t m, uint8_t r, uint8_t g, uint8_t b);

#endif // BOARD_H
