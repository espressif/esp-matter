/***************************************************************************//**
 * @file
 * @brief Silicon Labs Graphics Library: String Drawing Routines
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/* Standard C header files */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
/* EM types */
#include "em_types.h"

/* GLIB header files */
#include "glib.h"
#include "glib_color.h"

/**************************************************************************//**
*  @brief
*  Draws a char using the font supplied with the library.
*
*  @param pContext
*  Pointer to the GLIB_Context_t
*
*  @param myChar
*  Char to be drawn
*
*  @param x
*  Start x-coordinate for the char (Upper left corner)
*
*  @param y
*  Start y-coordinate for the char (Upper left corner)
*
*  @param opaque
*  Determines whether to show the background or color it with the background
*  color specified by the GLIB_Context_t. If opaque == true, the background color is used.
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawChar(GLIB_Context_t *pContext, char myChar, int32_t x, int32_t y,
                       bool opaque)
{
  EMSTATUS status;
  uint16_t fontIdx;
  uint8_t *pPixMap8;
  uint16_t *pPixMap16;
  uint32_t *pPixMap32;
  uint16_t row;
  uint32_t currentRow;
  uint16_t xOffset;
  uint32_t drawnElements = 0;

  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Check input char */
  if ((myChar < ' ') || (myChar > '~')) {
    return GLIB_ERROR_INVALID_CHAR;
  }

  /* Sets the index in the font array */
  if (pContext->font.fontClass == NumbersOnlyFont) {
    fontIdx = (myChar - '0');
    if (myChar == ':') {
      fontIdx = 10;
    }
    if (myChar == ' ') {
      fontIdx = 11;
    }
  } else { /* FullFont class */
    fontIdx = myChar - ' ';
  }

  if (fontIdx > (pContext->font.cntOfMapElements - 1)) {
    return GLIB_ERROR_INVALID_CHAR;
  }

  /* Loop through the rows and draw the font */
  pPixMap8 = (uint8_t *)pContext->font.pFontPixMap;
  pPixMap16 = (uint16_t *)pContext->font.pFontPixMap;
  pPixMap32 = (uint32_t *)pContext->font.pFontPixMap;

  for (row = 0; row < pContext->font.fontHeight; row++) {
    switch (pContext->font.sizeOfMapElement) {
      case 1:
        currentRow = pPixMap8[fontIdx];
        break;

      case 2:
        currentRow = pPixMap16[fontIdx];
        break;

      default:
        currentRow = pPixMap32[fontIdx];
    }

    for (xOffset = 0; xOffset < pContext->font.fontWidth; ++xOffset) {
      /* Bit 1 means draw, Bit 0 means do not draw */
      if (currentRow & 0x1) {
        status = GLIB_drawPixel(pContext, x + xOffset, y + row);
        if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
          return status;
        }
        if (status == GLIB_OK) {
          drawnElements++;
        }
      } else if (opaque) {
        /* Draw background pixel */
        status = GLIB_drawPixelColor(pContext, x + xOffset, y + row, pContext->backgroundColor);
        if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
          return status;
        }
        if (status == GLIB_OK) {
          drawnElements++;
        }
      }
      currentRow >>= 1;
    }

    /* Handle character spacing */
    for (; xOffset < pContext->font.fontWidth + pContext->font.charSpacing; ++xOffset) {
      if (opaque) {
        /* Draw background pixel */
        status = GLIB_drawPixelColor(pContext, x + xOffset, y + row, pContext->backgroundColor);
        if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
          return status;
        }
        if (status == GLIB_OK) {
          drawnElements++;
        }
      }
    }

    /* fontIdx offset for a new row */
    fontIdx += pContext->font.fontRowOffset;
  }
  return ((drawnElements == 0) ? GLIB_ERROR_NOTHING_TO_DRAW : GLIB_OK);
}

/**************************************************************************//**
*  @brief
*  Draws a string using the font supplied with the library.
*
*  @param pContext
*  Pointer to a GLIB_Context_t
*
*  @param pString
*  Pointer to the string that is drawn
*
*  @param sLength
*  number of characters in the string
*
*  @param x0
*  Start x-coordinate for the string (Upper left corner)
*
*  @param y0
*  Start y-coordinate for the string (Upper left corner)
*
*  @param opaque
*  Determines whether to show the background or color it with the background
*  color specified by the GLIB_Context_t. If opaque == 1, the background color is used.
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawString(GLIB_Context_t *pContext, const char* pString, uint32_t sLength,
                         int32_t x0, int32_t y0, bool opaque)
{
  EMSTATUS status;
  uint32_t drawnElements = 0;
  uint32_t stringIndex;
  int32_t x, y;

  /* Check arguments */
  if (pContext == NULL || pString == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  if (pContext->font.fontClass == InvalidFont) {
    return GLIB_ERROR_INVALID_CHAR;
  }

  x = x0;
  y = y0;

  /* Loops through the string and prints char for char */
  for (stringIndex = 0; stringIndex < sLength; stringIndex++) {
    /* Newline char */
    if (pString[stringIndex] == '\n') {
      x = x0;
      y = y + pContext->font.fontHeight + pContext->font.lineSpacing;
      continue;
    }

    /* Draw the current char */
    status = GLIB_drawChar(pContext, pString[stringIndex], x, y, opaque);
    if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
      return status;
    }
    if (status == GLIB_OK) {
      drawnElements++;
    }

    /* Adjust x and y coordinate */
    x += (pContext->font.fontWidth + pContext->font.charSpacing);
  }
  return ((drawnElements == 0) ? GLIB_ERROR_NOTHING_TO_DRAW : GLIB_OK);
}

/**************************************************************************//**
*  @brief
*  Set new font for the library. Note that GLIB defines a default font in glib.c.
*  Redefine GLIB_DEFAULT_FONT to change the default font.
*
*  @param pContext
*  Pointer to the GLIB_Context_t
*
*  @param pFont
*  Pointer to the new font
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_setFont(GLIB_Context_t *pContext, GLIB_Font_t *pFont)
{
  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  if (pFont == NULL) {
    memset(&pContext->font, 0, sizeof(GLIB_Font_t));
    return GLIB_ERROR_INVALID_ARGUMENT;
  } else {
    memcpy(&pContext->font, pFont, sizeof(GLIB_Font_t));
    return GLIB_OK;
  }
}

/**************************************************************************//**
*  @brief
*  Draws a string on the specified line on the display.
*
*  @param pContext
*  Pointer to a GLIB_Context_t
*
*  @param pString
*  Pointer to the string that is drawn
*
*  @param line
*  Specifies which line on the display to draw on.
*
*  @param align
*  Horizontal alignment of the string on the line. This can be left, right or
*  center. Note that left alignment is default.
*
*  @param xOffset
*  This parameter can be used to create a margin on the left or right side of
*  the string. Note that this argument accepts a negative value which can be
*  useful when aligning the string to the right.
*
*  @param yOffset
*  This parameter can be used to move the string further down or up a certain
*  number of pixels. A positive value would move the string down, while a
*  negative value would move the string up.
*
*  @param opaque
*  Determines whether to show the background or color it with the background
*  color specified by the GLIB_Context_t. If opaque == 1, the background color is used.
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawStringOnLine(GLIB_Context_t *pContext, const char *pString, uint8_t line,
                               GLIB_Align_t align, int32_t xOffset, int32_t yOffset, bool opaque)
{
  int32_t x, y;
  size_t length;
  size_t pixels;

  length = strlen(pString);
  pixels = length * pContext->font.fontWidth;
  y = line * (pContext->font.fontHeight + pContext->font.lineSpacing) + yOffset;

  switch (align) {
    case GLIB_ALIGN_LEFT:
      x = 0;
      break;
    case GLIB_ALIGN_CENTER:
      x = (pContext->pDisplayGeometry->xSize - pixels) / 2;
      break;
    case GLIB_ALIGN_RIGHT:
      x = pContext->pDisplayGeometry->xSize - pixels;
      break;
    default:
      x = 0;
      break;
  }
  x += xOffset;

  return GLIB_drawString(pContext, pString, length, x, y, opaque);
}
