/***************************************************************************//**
 * @file
 * @brief BT Mesh Graphics: Draws the graphics on the display
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

// standard headers
#include <string.h>
#include <stdio.h>

#include "app_assert.h"
#include "em_types.h"
#include "glib.h"
#include "dmd.h"

// Own header
#include "sl_btmesh_wstk_graphics.h"

// -----------------------------------------------------------------------------
// Local Variables

// Global glib context
static GLIB_Context_t glibContext;
// Current line number stored for printing text
static uint8_t graphLineNum = 0;
// Device name string
static char *deviceHeader = NULL;

// -----------------------------------------------------------------------------
// Static Function Declarations

static sl_status_t graphPrintCenter(GLIB_Context_t *pContext, char *pString);

// -----------------------------------------------------------------------------
//   Function Definitions

void graphInit(char *header)
{
  EMSTATUS status;

  // Initialize the DMD module for the DISPLAY device driver.
  status = DMD_init(0);
  app_assert_status_f(status, "DMD init error\n");

  status = GLIB_contextInit(&glibContext);
  app_assert_status_f(status, "GLIB init error\n");

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  // Use Narrow font
  status = GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  app_assert_status_f(status, "Font setting error\n");
  deviceHeader = header;
}

sl_status_t graphWriteString(char *string)
{
  sl_status_t ret = SL_STATUS_OK;
  EMSTATUS status;
  status = GLIB_clear(&glibContext);
  if (GLIB_OK != status) {
    return SL_STATUS_FAIL;
  }

  // Reset line number, print header and device name
  graphLineNum = 0;
  ret = graphPrintCenter(&glibContext, deviceHeader);
  if (SL_STATUS_OK != ret) {
    return ret;
  }

  // Print the string below the header center aligned
  ret = graphPrintCenter(&glibContext, string);
  if (SL_STATUS_OK != ret) {
    return ret;
  }

  status = DMD_updateDisplay();
  if (DMD_OK != status) {
    return SL_STATUS_FAIL;
  }
  return ret;
}

// -----------------------------------------------------------------------------
// Static Function Definitions

/***************************************************************************//**
 *  @brief  Print the given string center aligned
 *  @note   The string may contain several lines separated by new line
 *          characters ('\n'). Each line will be printed center aligned.
 *  @param[in]  pContext  Context
 *  @param[in]  pString  String to be displayed
 *  @returns Status of printing to LCD. SL_STATUS_OK if successful,
 *           SL_STATUS_FAIL otherwise
 ******************************************************************************/
static sl_status_t graphPrintCenter(GLIB_Context_t *pContext, char *pString)
{
  do {
    char* nextToken;
    uint8_t len;

    // Search for the next important token (new line or terminating NULL)
    for (nextToken = pString;
         ((*nextToken != '\n') && (*nextToken != '\0'));
         nextToken++) {
      ;
    }

    len = nextToken - pString;
    // Print the line if it is not null length
    if (len) {
      uint8_t strWidth = len * pContext->font.fontWidth;
      uint8_t posX = (pContext->pDisplayGeometry->xSize - strWidth) >> 1;
      uint8_t posY = ((pContext->font.lineSpacing + pContext->font.fontHeight)
                      * graphLineNum)
                     + pContext->font.lineSpacing;
      EMSTATUS status = GLIB_drawString(pContext, pString, len, posX, posY, 0);
      if (GLIB_OK != status && GLIB_ERROR_NOTHING_TO_DRAW != status) {
        return SL_STATUS_FAIL;
      }
    }
    pString = nextToken;
    // If the token at the end of the line is new line character,
    // then increase line number
    if (*nextToken == '\n') {
      graphLineNum++;
      pString++;
    }
  } while (*pString); // while terminating NULL is not reached
  return SL_STATUS_OK;
}
