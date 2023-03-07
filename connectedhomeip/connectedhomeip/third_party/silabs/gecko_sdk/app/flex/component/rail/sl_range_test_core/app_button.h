/***************************************************************************//**
 * @file
 * @brief app_button.h
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
#ifndef APP_BUTTON_H
#define APP_BUTTON_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Readability define for button 0
#define BUTTON_0   ((uint8_t) 0)
/// Readability define for button 1
#define BUTTON_1   ((uint8_t) 1)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Init function for internal_app_init, initialize the button timers' times.
 *
 * @param None
 * @returns None
 ******************************************************************************/
void init_button_delay(void);

/*******************************************************************************
 * Gets the state of the polled button and clears it after.
 *
 * @param button_number The selected button's number
 * @return The state of the selected button
 ******************************************************************************/
bool get_and_clear_button_state(uint8_t button_number);

#endif  // APP_BUTTON_H
