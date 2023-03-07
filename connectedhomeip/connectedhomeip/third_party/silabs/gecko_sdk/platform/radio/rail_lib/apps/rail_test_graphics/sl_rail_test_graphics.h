/***************************************************************************//**
 * @file
 * @brief Draws graphics on the display
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

#ifndef SL_RAIL_TEST_GRAPHICS_H
#define SL_RAIL_TEST_GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/
/**
 * Initialize the display.
 *
 * @note: This function should be called during application initialization.
 */
void sl_rail_test_graphics_init(void);

/**
 * Process display actions.
 *
 * @note: This function should be called from the application's super loop.
 */
void sl_rail_test_graphics_process_action(void);

/**
 * Signal for updates to occur during the call to process actions.
 */
void sl_rail_test_graphics_update(void);

/**
 * Put the display to sleep.
 */
void sl_rail_test_graphics_sleep(void);

/**
 * Wake up the display.
 */
void sl_rail_test_graphics_wakeup(void);

/**
 * Append a string to the display.
 *
 * @param[in] str String pointer to display
 */
void sl_rail_test_graphics_append_string(char *str);

/**
 * Clear the display.
 */
void sl_rail_test_graphics_clear(void);

/**
 * Insert a triangle on display.
 *
 * @param[in] x The x location of the box that would contain this triangle.
 * @param[in] y The y location of the box that would contain this triangle.
 * @param[in] size The size of the triangle to draw.
 * @param[in] up   Whether the triangle should point upwards or downwards.
 * @param[in] fillPercent  The percent to fill this triangle. If the value
 * is negative then fill from bottom to top.
 */
void sl_rail_test_graphics_insert_triangle(uint32_t x,
                                           uint32_t y,
                                           uint32_t size,
                                           bool up,
                                           int8_t fillPercent);

#ifdef __cplusplus
}
#endif

#endif // SL_RAIL_TEST_GRAPHICS_H
