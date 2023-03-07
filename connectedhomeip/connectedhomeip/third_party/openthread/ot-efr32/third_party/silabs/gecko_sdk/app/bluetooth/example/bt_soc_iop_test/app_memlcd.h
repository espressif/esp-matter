/***************************************************************************//**
 * @file
 * @brief High level functions for memory LCD control.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef APP_MEMLCD_H
#define APP_MEMLCD_H

#define MAX_CHARS_PER_LINE 30

typedef enum {
  FONT_NARROW,
  FONT_NORMAL
} display_font_t;

#ifdef BOARD_MEMLCD_PRESENT
/***************************************************************************//**
 * Initialize memory LCD.
 ******************************************************************************/
void app_memlcd_init(void);

/***************************************************************************//**
 * Clear memory LCD.
 ******************************************************************************/
void app_memlcd_clear(void);

/***************************************************************************//**
 * Update memory LCD.
 ******************************************************************************/
void app_memlcd_update(void);

/***************************************************************************//**
 * Draw string on memory LCD.
 *
 * @param[in] font Font Type. FONT_NARROW or FONT_NORMAL.
 * @param[in] format The string to draw on LCD.
 ******************************************************************************/
void app_memlcd_draw_string(display_font_t font, char *format, ...);

/***************************************************************************//**
 * Append newlines on memory LCD.
 *
 * @param[in] num Number of newlines.
 ******************************************************************************/
void app_memlcd_newline(uint8_t num);

#else // BOARD_MEMLCD_PRESENT
#define app_memlcd_init()
#define app_memlcd_clear()
#define app_memlcd_update()
#define app_memlcd_draw_string(...)
#define app_memlcd_newline(x)
#endif // BOARD_MEMLCD_PRESENT

#endif // APP_MEMLCD_H
