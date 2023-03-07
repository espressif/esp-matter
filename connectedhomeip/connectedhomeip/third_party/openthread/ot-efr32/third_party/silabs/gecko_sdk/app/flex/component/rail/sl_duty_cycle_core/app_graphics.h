/***************************************************************************//**
 * @file
 * @brief app_graphics.h
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
#ifndef APP_GRAPHICS_H
#define APP_GRAPHICS_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
// Error codes
typedef enum {
  INVALID_APP_STATE,
  FAILED_TO_SET_RADIO_CONFIG,
  ERROR_CODES_COUNT
} error_code_t;
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * @brief Initializes the graphics stack.
 * @note This function will /hang/ if errors occur (usually
 *       caused by faulty displays.
 ******************************************************************************/
void graphics_init(void);

/*******************************************************************************
 * @brief   A function for managing what is shown on the LCD display in an
 *          error state.
 *
 * @param error_code: the code for the error to be diplayed
 *
 * @return None.
 ******************************************************************************/
void display_error_on_lcd(error_code_t error_code);

/*******************************************************************************
 * @brief   A function for managing what is shown on the LCD display under
 *          normal conditions.
 *
 * @param app_type: string that will be printed on the display after title
 *
 * @param packet_transmitted: The number of packet transmitted to be displayed
 *
 * @param packet_received: The number of packet received to be displayed
 *
 * @return None.
 ******************************************************************************/
void update_display(const char *app_type, uint16_t packet_transmitted, uint16_t packet_received);

#endif  // APP_GRAPHICS_H
