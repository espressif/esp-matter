/***************************************************************************//**
 * @file
 * @brief app_graphics.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <string.h>
#include "dmd.h"
#include "glib.h"
#include "printf.h"
#include "app_graphics.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Context used all over the graphics
static GLIB_Context_t glib_context;

static char *error_code_strings[] =
{
  "Invalid App State",
  "Failed to set radio config"
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * @brief Initializes the graphics stack.
 * @note This function will /hang/ if errors occur (usually
 *       caused by faulty displays.
 ******************************************************************************/
void graphics_init(void)
{
  EMSTATUS status;

  /* Initialize the DMD module for the DISPLAY device driver. */
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (1) ;
  }

  status = GLIB_contextInit(&glib_context);
  if (GLIB_OK != status) {
    while (1) ;
  }

  glib_context.backgroundColor = White;
  glib_context.foregroundColor = Black;

  /* Use Normal font */
  GLIB_setFont(&glib_context, (GLIB_Font_t *)&GLIB_FontNormal8x8);
}

/*******************************************************************************
 * @brief   A function for managing what is shown on the LCD display in an
 *          error state.
 *
 * @param error_code: the code for the error to be diplayed
 *
 * @return None.
 ******************************************************************************/
void display_error_on_lcd(error_code_t error_code)
{
  char textBuf[32];
  // Clear what's currently on screen
  GLIB_clear(&glib_context);
  // Add the demo output strings
  GLIB_setFont(&glib_context, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  GLIB_drawStringOnLine(&glib_context, "RAIL Duty Cycle Demo", 1, GLIB_ALIGN_CENTER, 0, 0, 0);
  GLIB_setFont(&glib_context, (GLIB_Font_t *)&GLIB_FontNormal8x8);

  snprintf(textBuf, sizeof(textBuf), "ERROR: %d", error_code);
  GLIB_drawStringOnLine(&glib_context, textBuf, 2, GLIB_ALIGN_CENTER, 0, 0, 0);

  if (error_code < ERROR_CODES_COUNT) {
    GLIB_drawStringOnLine(&glib_context, error_code_strings[error_code], 4, GLIB_ALIGN_CENTER, 0, 0, 0);
  }
  // Force a redraw
  DMD_updateDisplay();
}

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
void update_display(const char *app_type, uint16_t packet_transmitted, uint16_t packet_received)
{
  char textBuf[32];

  // Clear what's currently on screen
  GLIB_clear(&glib_context);

  // Add the demo output strings
  GLIB_drawStringOnLine(&glib_context, "-- Duty Cycle --", 1, GLIB_ALIGN_CENTER, 0, 0, 0);
  GLIB_drawStringOnLine(&glib_context, "With", 2, GLIB_ALIGN_CENTER, 0, 0, 0);
  GLIB_drawStringOnLine(&glib_context, app_type, 3, GLIB_ALIGN_CENTER, 0, 0, 0);

  GLIB_drawStringOnLine(&glib_context, "----- RX -----", 5, GLIB_ALIGN_CENTER, 0, 0, 0);
  snprintf(textBuf, sizeof(textBuf), "Packet Rx: %d", packet_received);
  GLIB_drawStringOnLine(&glib_context, textBuf, 6, GLIB_ALIGN_CENTER, 0, 0, 0);

  GLIB_drawStringOnLine(&glib_context, "----- TX -----", 8, GLIB_ALIGN_CENTER, 0, 0, 0);
  snprintf(textBuf, sizeof(textBuf), "Packet Tx: %d", packet_transmitted);
  GLIB_drawStringOnLine(&glib_context, textBuf, 9, GLIB_ALIGN_CENTER, 0, 0, 0);

  // Force a redraw
  DMD_updateDisplay();
}
// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
