/***************************************************************************//**
 * @file
 * @brief Board HW abstraction header for BRD4166A
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

#ifndef RGBLED_LIGHT_BRD4166_H
#define RGBLED_LIGHT_BRD4166_H

/// Mask to enable EM4 wake-up BTN0
#define BOARD_BUTTON0_EM4WUEN_MASK 0x10

/// RGB LED present on board
#define BOARD_RGBLED_PRESENT       1
/// Number of LEDs on board
#define BOARD_RGBLED_COUNT         4
/// RGB LED Power Enable port
#define BOARD_RGBLED_PWR_EN_PORT   gpioPortJ
/// RGB LED Power Enable pin
#define BOARD_RGBLED_PWR_EN_PIN    14
/// RGB LED COM port
#define BOARD_RGBLED_COM_PORT      gpioPortI
/// RGB LED COM0 port
#define BOARD_RGBLED_COM0_PORT     gpioPortI
/// RGB LED COM0 pin
#define BOARD_RGBLED_COM0_PIN      0
/// RGB LED COM1 port
#define BOARD_RGBLED_COM1_PORT     gpioPortI
/// RGB LED COM1 pin
#define BOARD_RGBLED_COM1_PIN      1
/// RGB LED COM2 port
#define BOARD_RGBLED_COM2_PORT     gpioPortI
/// RGB LED COM2 pin
#define BOARD_RGBLED_COM2_PIN      2
/// RGB LED COM3 port
#define BOARD_RGBLED_COM3_PORT     gpioPortI
/// RGB LED COM3 pin
#define BOARD_RGBLED_COM3_PIN      3

/***************************************************************************//**
 * Initialize all LEDs.
 ******************************************************************************/
void rgb_led_init(void);

/***************************************************************************//**
 * De-initialize all LEDs by disabling them.
 ******************************************************************************/
void rgb_led_deinit(void);

/***************************************************************************//**
 * Set LED color based on RGB components.
 *
 * @param[in] mask         LED instance mask
 * @param[in] r            Red component
 * @param[in] g            Green component
 * @param[in] b            Blue component
 ******************************************************************************/
void rgb_led_set_rgb(uint8_t m, uint8_t r, uint8_t g, uint8_t b);

#endif // RGBLED_LIGHT_BRD4166_H
