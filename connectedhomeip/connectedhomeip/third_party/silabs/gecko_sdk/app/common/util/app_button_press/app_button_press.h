/***************************************************************************//**
 * @file
 * @brief Button press header file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef APP_BUTTON_PRESS_H_
#define APP_BUTTON_PRESS_H_

#include "sl_power_manager.h"

/*******************************************************************************
 * Public definitions
 ******************************************************************************/
#define APP_BUTTON_PRESS_DURATION_SHORT        0
#define APP_BUTTON_PRESS_DURATION_MEDIUM       1
#define APP_BUTTON_PRESS_DURATION_LONG         2
#define APP_BUTTON_PRESS_DURATION_VERYLONG     3
#define APP_BUTTON_PRESS_NONE                  4

/***************************************************************************//**
 * Init function for button press
 ******************************************************************************/
void app_button_press_init(void);

/***************************************************************************//**
 * Enable button press callbacks.
 ******************************************************************************/
void app_button_press_enable(void);

/***************************************************************************//**
 * Disable button press callbacks.
 ******************************************************************************/
void app_button_press_disable(void);

/***************************************************************************//**
 * Step function for button press
 ******************************************************************************/
void app_button_press_step(void);

/***************************************************************************//**
 * Callback for button press
 * @param button button ID. From 0 up to SL_SIMPLE_BUTTON_COUNT-1
 * @param duration duration of button press. Either of
 *                   - APP_BUTTON_PRESS_NONE
 *                   - APP_BUTTON_PRESS_DURATION_SHORT
 *                   - APP_BUTTON_PRESS_DURATION_MEDIUM
 *                   - APP_BUTTON_PRESS_DURATION_LONG
 *                   - APP_BUTTON_PRESS_DURATION_VERYLONG
 ******************************************************************************/
void app_button_press_cb(uint8_t button, uint8_t duration);

/***************************************************************************//**
 * Routine for power manager handler
 *
 * @return SL_POWER_MANAGER_WAKEUP if there is an unhandled button press
 ******************************************************************************/
sl_power_manager_on_isr_exit_t app_button_press_sleep_on_isr_exit(void);

/***************************************************************************//**
 * Checks if it is ok to sleep now
 *
 * @return false if there is an unhandled button press
 ******************************************************************************/
bool app_button_press_is_ok_to_sleep(void);

#endif // APP_BUTTON_PRESS_H_
