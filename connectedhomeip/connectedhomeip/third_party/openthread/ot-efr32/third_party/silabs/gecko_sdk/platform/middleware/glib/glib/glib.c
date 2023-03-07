/***************************************************************************//**
 * @file
 * @brief Silicon Labs Graphics Library: General Routines
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

/* EM types and devices */
#include "em_types.h"
#include "em_device.h"

/* GLIB files */
#include "glib.h"

/** Define the default font. An application can override the default font
 *  by defining GLIB_NO_DEFAULT_FONT and by providing a custom
 *  GLIB_DEFAULT_FONT macro that points to a @ref GLIB_Font_t structure
 *  that should be used as a default font. */
#ifndef GLIB_NO_DEFAULT_FONT
#define GLIB_DEFAULT_FONT       ((GLIB_Font_t *)&GLIB_FontNormal8x8)
#endif

/**
 * @cond DO_NOT_INCLUDE_WITH_DOXYGEN
 * @brief Inline version of the color transformation function.
 */
static __INLINE void GLIB_colorTranslate24bppInl(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue)
{
  *red   = (color >> RedShift) & 0xFF;
  *green = (color >> GreenShift) & 0xFF;
  *blue  = (color >> BlueShift) & 0xFF;
}
/** @endcond */

/**************************************************************************//**
*  @brief
*  Initialize the GLIB_Context_t
*
*  The context is set to default values and gets information about the display
*  from the display driver.
*
*  @param pContext
*  Pointer to a GLIB_Context_t
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_contextInit(GLIB_Context_t *pContext)
{
  EMSTATUS status;
  DMD_DisplayGeometry *pTmpDisplayGeometry;

  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Sets the default background and foreground color */
  pContext->backgroundColor = Black;
  pContext->foregroundColor = White;

  /* Sets a pointer to the display geometry struct */
  status = DMD_getDisplayGeometry(&pTmpDisplayGeometry);
  if (status != DMD_OK) {
    return status;
  }

  pContext->pDisplayGeometry = pTmpDisplayGeometry;

  /* Sets the clipping region to the whole display */
  GLIB_Rectangle_t tmpRect = { 0, 0, pTmpDisplayGeometry->xSize - 1, pTmpDisplayGeometry->ySize - 1 };
  status = GLIB_setClippingRegion(pContext, &tmpRect);
  if (status != GLIB_OK) {
    return status;
  }

  /* Configure font. Default to NORMAL 8x8 if included in project. */
#ifndef GLIB_NO_DEFAULT_FONT
  GLIB_setFont(pContext, GLIB_DEFAULT_FONT);
#endif

  return status;
}

/**************************************************************************//**
*  @brief
*  Returns the display from sleep mode
*
*  @return
*  Returns DMD_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_displayWakeUp()
{
  /* Use display driver's wake up function */
  return DMD_wakeUp();
}

/**************************************************************************//**
*  @brief
*  Sets the display in sleep mode
*
*  @return
*  Returns DMD_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_displaySleep()
{
  /* Use Display Driver sleep function */
  return DMD_sleep();
}

/**************************************************************************//**
*  @brief
*  Sets the clippingRegion of the passed in GLIB_Context_t
*
*  @param pContext
*  Pointer to a GLIB_Context_t
*  @param pRect
*  Pointer to a GLIB_Rectangle_t which is the clipping region to be set.
*
*  @return
*  - Returns GLIB_OK on success
*  - Returns GLIB_ERROR_INVALID_CLIPPINGREGION if invalid coordinates
*  - Returns GLIB_OUT_OF_BOUNDS if clipping region is bigger than display clipping
*  area
******************************************************************************/
EMSTATUS GLIB_setClippingRegion(GLIB_Context_t *pContext, const GLIB_Rectangle_t *pRect)
{
  /* Check arguments */
  if ((pContext == NULL) || (pRect == NULL)) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Check coordinates against the display region */
  if ((pRect->xMin >= pRect->xMax)
      || (pRect->yMin >= pRect->yMax)) {
    return GLIB_ERROR_INVALID_CLIPPINGREGION;
  }

  if ((pRect->xMin < 0)
      || (pRect->yMin < 0)
      || (pRect->xMax > pContext->pDisplayGeometry->xSize - 1)
      || (pRect->yMax > pContext->pDisplayGeometry->ySize - 1)) {
    return GLIB_OUT_OF_BOUNDS;
  }

  GLIB_Rectangle_t tmpRect = { pRect->xMin, pRect->yMin, pRect->xMax, pRect->yMax };
  pContext->clippingRegion = tmpRect;
  return GLIB_applyClippingRegion(pContext);
}

/**************************************************************************//**
*  @brief
*  Clears the display with the background color of the GLIB_Context_t
*
*  @param pContext
*  Pointer to a GLIB_Context_t which holds the background color.
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_clear(GLIB_Context_t *pContext)
{
  EMSTATUS status;
  uint8_t  red;
  uint8_t  green;
  uint8_t  blue;
  uint32_t width;
  uint32_t height;

  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Divide the 24-color into it's components */
  GLIB_colorTranslate24bpp(pContext->backgroundColor, &red, &green, &blue);

  /* Reset display driver clipping area */
  status = GLIB_resetDisplayClippingArea(pContext);
  if (status != GLIB_OK) {
    return status;
  }

  /* Fill the display with the background color of the GLIB_Context_t  */
  width = pContext->pDisplayGeometry->clipWidth;
  height = pContext->pDisplayGeometry->clipHeight;
  return DMD_writeColor(0, 0, red, green, blue, width * height);
}

/**************************************************************************//**
*  @brief
*  Clears the clipping region by filling it with the background color of
*  the GLIB_Context_t
*
*  @param pContext
*  Pointer to a GLIB_Context_t which holds the background color.
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_clearRegion(const GLIB_Context_t *pContext)
{
  EMSTATUS status;
  uint8_t  red;
  uint8_t  green;
  uint8_t  blue;
  uint32_t width;
  uint32_t height;

  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Divide the 24-color into it's components */
  GLIB_colorTranslate24bpp(pContext->backgroundColor, &red, &green, &blue);

  status = GLIB_applyClippingRegion(pContext);
  if (status != DMD_OK) {
    return status;
  }

  /* Fill the region with the background color of the GLIB_Context_t */
  width = pContext->clippingRegion.xMax - pContext->clippingRegion.xMin + 1;
  height = pContext->clippingRegion.yMax - pContext->clippingRegion.yMin + 1;
  status = DMD_writeColor(0, 0, red, green, blue, width * height);
  if (status != DMD_OK) {
    return status;
  }

  return status;
}

/**************************************************************************//**
*  @brief
*  Reset the display driver clipping area to the whole display
*
*  @param pContext
*  Pointer to a GLIB_Context_t
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_resetDisplayClippingArea(GLIB_Context_t *pContext)
{
  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  return DMD_setClippingArea(0, 0, pContext->pDisplayGeometry->xSize,
                             pContext->pDisplayGeometry->ySize);
}

/**************************************************************************//**
*  @brief
*  Reset the GLIB_Context_t clipping region to the whole display
*
*  @param pContext
*  Pointer to a GLIB_Context_t
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_resetClippingRegion(GLIB_Context_t *pContext)
{
  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  pContext->clippingRegion.xMin = 0;
  pContext->clippingRegion.yMin = 0;
  pContext->clippingRegion.xMax = pContext->pDisplayGeometry->xSize - 1;
  pContext->clippingRegion.yMax = pContext->pDisplayGeometry->ySize - 1;

  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Apply the clipping region from the GLIB_Context_t in the DMD driver.
*
*  @param pContext
*  Pointer to a GLIB_Context_t
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_applyClippingRegion(const GLIB_Context_t *pContext)
{
  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Reset driver clipping area to GLIB clipping region */
  return DMD_setClippingArea(pContext->clippingRegion.xMin,
                             pContext->clippingRegion.yMin,
                             pContext->clippingRegion.xMax - pContext->clippingRegion.xMin + 1,
                             pContext->clippingRegion.yMax - pContext->clippingRegion.yMin + 1);
}

/**************************************************************************//**
*  @brief
*  Extracts the color components from the 32-bit color passed and puts them in
*  the passed in 8-bits ints. The color is 24-bit RGB.
*
*  Example: color = 0x00FFFF00 -> red = 0xFF, green = 0xFF, blue = 0x00.
*
*  @param color
*  The color which is to be translated
*  @param red
*  Pointer to a uint8_t holding the red component
*  @param green
*  Pointer to a uint8_t holding the green component
*  @param blue
*  Pointer to a uint8_t holding the blue component
******************************************************************************/
void GLIB_colorTranslate24bpp(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue)
{
  GLIB_colorTranslate24bppInl(color, red, green, blue);
}

/**************************************************************************//**
*  @brief
*  Convert 3 uint8_t color components into a 24-bit color
*
*  Example: red = 0xFF, green = 0xFF, blue = 0x00 -> 0x00FFFF00 = Yellow
*
*  @param red
*  Red component
*  @param green
*  Green component
*  @param blue
*  Blue component
*  @return
*  Returns a 32-bit unsigned integer representing the color. The 8 LSB is blue,
*  the next 8 is green and the next 8 is red. 0x00RRGGBB
*
******************************************************************************/
uint32_t GLIB_rgbColor(uint8_t red, uint8_t green, uint8_t blue)
{
  return (red << RedShift) | (green << GreenShift) | (blue << BlueShift);
}

/**************************************************************************//**
*  @brief
*  Draws a pixel at x, y using foregroundColor defined in the GLIB_Context_t.
*
*  @param pContext
*  Pointer to a GLIB_Context_t which holds the foreground color and clipping region
*  @param x
*  X-coordinate
*  @param y
*  Y-coordinate
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawPixel(GLIB_Context_t *pContext, int32_t x, int32_t y)
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }
  if (!GLIB_rectContainsPoint(&pContext->clippingRegion, x, y)) {
    return GLIB_ERROR_NOTHING_TO_DRAW;
  }

  /* Translate color and draw pixel */
  GLIB_colorTranslate24bppInl(pContext->foregroundColor, &red, &green, &blue);
  return DMD_writeColor(x, y, red, green, blue, 1);
}

/**************************************************************************//**
*  @brief
*  Draws a pixel at x, y using the color parameter
*
*  @param pContext
*  Pointer to a GLIB_Context_t which holds the clipping region
*  @param x
*  X-coordinate
*  @param y
*  Y-coordinate
*  @param color
*  32-bit int defining the RGB color. The 24 LSB defines the RGB color like this:
*  RRRRRRRRGGGGGGGGBBBBBBBB. Example: Yellow = 0x00FFFF00
*  @return
*  Returns DMD_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawPixelColor(GLIB_Context_t *pContext, int32_t x, int32_t y,
                             uint32_t color)
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }
  if (!GLIB_rectContainsPoint(&pContext->clippingRegion, x, y)) {
    return GLIB_ERROR_NOTHING_TO_DRAW;
  }

  /* Translate color and draw pixel */
  GLIB_colorTranslate24bppInl(color, &red, &green, &blue);
  return DMD_writeColor(x, y, red, green, blue, 1);
}

/**************************************************************************//**
*  @brief
*  Draws a pixel at x, y with color defined by red, green and blue
*  1 byte per channel.
*
*  Example: To draw a yellow pixel at (10, 10).
*  x = 10, y = 10, red = 0xFF, green = 0xFF, blue = 0x00
*
*  @param pContext
*  Pointer to a GLIB_Context_t which holds the clipping region
*  @param x
*  X-coordinate
*  @param y
*  Y-coordinate
*  @param red
*  8-bit red code
*  @param green
*  8-bit green code
*  @param blue
*  8-bit blue code
*
*  @return
*  Returns DMD_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawPixelRGB(GLIB_Context_t *pContext, int32_t x, int32_t y,
                           uint8_t red, uint8_t green, uint8_t blue)
{
  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }
  if (!GLIB_rectContainsPoint(&pContext->clippingRegion, x, y)) {
    return GLIB_ERROR_NOTHING_TO_DRAW;
  }

  /* Call Display driver function */
  return DMD_writeColor(x, y, red, green, blue, 1);
}
