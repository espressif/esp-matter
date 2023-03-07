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
#include <stdio.h>
#include "glib.h"
#include "sl_memlcd_display.h"
#include "app_memlcd.h"
#include "app_assert.h"
#include "app_log.h"

static int current_line = 0;

// For displaying narrow texts.
static GLIB_Context_t glib_context_narrow;
static uint8_t max_narrow_chars_per_line;

// For displaying normal texts.
static GLIB_Context_t glib_context_normal;
static uint8_t max_normal_chars_per_line;

// Initialize memory LCD.
void app_memlcd_init(void)
{
  EMSTATUS status;
  max_narrow_chars_per_line = SL_MEMLCD_DISPLAY_WIDTH / GLIB_FontNarrow6x8.fontWidth;
  max_normal_chars_per_line = SL_MEMLCD_DISPLAY_WIDTH / GLIB_FontNormal8x8.fontWidth;

  // Initialize the DMD support for memory LCD display.
  status = DMD_init(0);
  app_assert(DMD_OK == status, "Failed to init display driver.");

  // Initialize the glib context for narrow texts.
  status = GLIB_contextInit(&glib_context_narrow);
  app_assert(GLIB_OK == status, "Failed to init display context.");
  glib_context_narrow.backgroundColor = White;
  glib_context_narrow.foregroundColor = Black;

  // Use Narrow font.
  status = GLIB_setFont(&glib_context_narrow,
                        (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  app_assert(GLIB_OK == status, "Failed to init display font.");

  // Initialize the glib context for normal texts.
  status = GLIB_contextInit(&glib_context_normal);
  app_assert(GLIB_OK == status, "Failed to init display context.");
  glib_context_normal.backgroundColor = White;
  glib_context_normal.foregroundColor = Black;

  // Use Normal font.
  status = GLIB_setFont(&glib_context_normal,
                        (GLIB_Font_t *)&GLIB_FontNormal8x8);
  app_assert(GLIB_OK == status, "Failed to init display font.");

  // Fill LCD with background color.
  app_memlcd_clear();

  app_log_info("LCD initialization success." APP_LOG_NL);
}

// Clear memory LCD.
void app_memlcd_clear(void)
{
  EMSTATUS status;
  current_line = 0; // Reset line number.
  status = GLIB_clear(&glib_context_normal); // Fill LCD with background color.
  if (status != GLIB_OK) {
    app_log_error("Failed to clear display. E[%i]" APP_LOG_NL, status);
  }
}

// Update memory LCD.
void app_memlcd_update(void)
{
  EMSTATUS status;
  status = DMD_updateDisplay();
  if (status != DMD_OK) {
    app_log_error("Failed to update display. E[%i]" APP_LOG_NL, status);
  }
}

// Draw string on memory LCD.
void app_memlcd_draw_string(display_font_t font, char *format, ...)
{
  EMSTATUS status;

  // Compose char array from variable argument list.
  char line[MAX_CHARS_PER_LINE + 1];
  va_list va;
  va_start(va, format);

  switch (font) {
    case FONT_NARROW: {
      vsnprintf(line, max_narrow_chars_per_line + 1, format, va);
      status =  GLIB_drawStringOnLine(&glib_context_narrow,
                                      line,
                                      current_line++,
                                      GLIB_ALIGN_LEFT,
                                      5,
                                      5,
                                      true);
      break;
    }

    case FONT_NORMAL: {
      vsnprintf(line, max_normal_chars_per_line + 1, format, va);
      status =  GLIB_drawStringOnLine(&glib_context_normal,
                                      line,
                                      current_line++,
                                      GLIB_ALIGN_LEFT,
                                      5,
                                      5,
                                      true);
      break;
    }

    default: {
      status = GLIB_ERROR_INVALID_ARGUMENT;
      break;
    }
  }

  va_end(va);

  if (status != GLIB_OK) {
    app_log_error("Failed to draw string on display. E[%i]" APP_LOG_NL, status);
  }
}

// Append newlines on memory LCD.
void app_memlcd_newline(uint8_t num)
{
  current_line += num;
}
