/***************************************************************************//**
 * @file
 * @brief Silicon Labs Graphics Library: Rectangle Routines
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

/* EM types */
#include "em_types.h"

/* GLIB header files */
#include "glib.h"

/**************************************************************************//**
*  @brief
*  Checks if the point passed in is in the interior of the rectangle passed in.
*
*  If the point is on the edge of the rectangle the function returns 1.
*
*  @param pRect
*  Pointer to a rectangle structure
*  @param x
*  X-coordinate of point
*  @param y
*  Y-coordinate of point
*  @return
*  - Returns false if coordinate is outside the rectangle
*  - Returns true otherwise
******************************************************************************/
bool GLIB_rectContainsPoint(const GLIB_Rectangle_t *pRect, int32_t x, int32_t y)
{
  if ((pRect == NULL)
      || (x < pRect->xMin)
      || (x > pRect->xMax)
      || (y < pRect->yMin)
      || (y > pRect->yMax)) {
    return false;
  }
  return true;
}

/**************************************************************************//**
*  @brief
*  Normalize the rectangle that is passed in.
*  Sets yMin to the minimum value of yMin and yMax.
*  Sets yMax to the maximum value of yMin and yMax.
*  And the same for xMin and xMax
*
*  @param pRect
*  Pointer to a rectangle structure
******************************************************************************/
void GLIB_normalizeRect(GLIB_Rectangle_t *pRect)
{
  int32_t swap;
  if (pRect->xMin > pRect->xMax) {
    swap        = pRect->xMin;
    pRect->xMin = pRect->xMax;
    pRect->xMax = swap;
  }

  if (pRect->yMin > pRect->yMax) {
    swap        = pRect->yMin;
    pRect->yMin = pRect->yMax;
    pRect->yMax = swap;
  }
}

/**************************************************************************//**
*  @brief
*  Draws a rectangle outline defined by the passed in rectangle
*
*  @param pContext
*  Pointer to a GLIB_Context_t in which the rectangle is drawn. The rectangle is drawn
*  using the foreground color.
*
*  @param pRect
*  Pointer to a rectangle structure
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawRect(GLIB_Context_t *pContext, const GLIB_Rectangle_t *pRect)
{
  EMSTATUS status;
  GLIB_Rectangle_t tmpRectangle = *pRect;

  GLIB_normalizeRect(&tmpRectangle);

  /* Clip rectangle if necessary */
  if (tmpRectangle.xMin < pContext->clippingRegion.xMin) {
    tmpRectangle.xMin = pContext->clippingRegion.xMin;
  }
  if (tmpRectangle.xMax > pContext->clippingRegion.xMax) {
    tmpRectangle.xMax = pContext->clippingRegion.xMax;
  }
  if (tmpRectangle.yMin < pContext->clippingRegion.yMin) {
    tmpRectangle.yMin = pContext->clippingRegion.yMin;
  }
  if (tmpRectangle.yMax > pContext->clippingRegion.yMax) {
    tmpRectangle.yMax = pContext->clippingRegion.yMax;
  }

  /* Draw a line across the top of the rectangle */
  status = GLIB_drawLineH(pContext, tmpRectangle.xMin, tmpRectangle.yMin, tmpRectangle.xMax);
  if (status != GLIB_OK) {
    return status;
  }

  /* Check if the rectangle is one pixel tall */
  if (tmpRectangle.yMin == tmpRectangle.yMax) {
    return GLIB_OK;
  }

  /* Draw the right side of the rectangle */
  status = GLIB_drawLineV(pContext, tmpRectangle.xMax, tmpRectangle.yMin + 1, tmpRectangle.yMax);
  if (status != GLIB_OK) {
    return status;
  }

  /* Check if the rectangle is one pixel wide */
  if (tmpRectangle.xMin == tmpRectangle.xMax) {
    return GLIB_OK;
  }

  /* Draw a line across the bottom of the rectangle */
  status = GLIB_drawLineH(pContext, tmpRectangle.xMin, tmpRectangle.yMax, tmpRectangle.xMax - 1);
  if (status != GLIB_OK) {
    return status;
  }

  /* Return if the rectangle is two pixels tall */
  if ((tmpRectangle.yMin + 1) == tmpRectangle.yMax) {
    return GLIB_OK;
  }

  /* Draw the left side of the rectangle */
  return GLIB_drawLineV(pContext, tmpRectangle.xMin, tmpRectangle.yMin + 1, tmpRectangle.yMax - 1);
}

/**************************************************************************//**
*  @brief
*  Draws a filled rectangle defined by the passed in rectangle. The filled rectangle
*  is drawn from (xMin, yMin) to (xMax, yMax), inclusive, of the rectangle passed in.
*
*  @param pContext
*  Pointer to a GLIB_Context_t in which the rectangle is drawn. The rectangle is filled
*  with the foreground color.
*  @param pRect
*  Pointer to a rectangle structure
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawRectFilled(GLIB_Context_t *pContext, const GLIB_Rectangle_t *pRect)
{
  EMSTATUS status;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  int32_t width;
  int32_t height;
  GLIB_Rectangle_t tmpRectangle = *pRect;

  GLIB_normalizeRect(&tmpRectangle);

  /* Clip rectangle if necessary */
  if (tmpRectangle.xMin < pContext->clippingRegion.xMin) {
    tmpRectangle.xMin = pContext->clippingRegion.xMin;
  }
  if (tmpRectangle.xMax > pContext->clippingRegion.xMax) {
    tmpRectangle.xMax = pContext->clippingRegion.xMax;
  }
  if (tmpRectangle.yMin < pContext->clippingRegion.yMin) {
    tmpRectangle.yMin = pContext->clippingRegion.yMin;
  }
  if (tmpRectangle.yMax > pContext->clippingRegion.yMax) {
    tmpRectangle.yMax = pContext->clippingRegion.yMax;
  }

  /* Draw filled rectangle */
  GLIB_colorTranslate24bpp(pContext->foregroundColor, &red, &green, &blue);

  width  = tmpRectangle.xMax - tmpRectangle.xMin + 1;
  height = tmpRectangle.yMax - tmpRectangle.yMin + 1;

  status = DMD_setClippingArea(tmpRectangle.xMin, tmpRectangle.yMin, width, height);
  if (status != DMD_OK) {
    return status;
  }

  status = DMD_writeColor(0, 0, red, green, blue, width * height);
  if (status != DMD_OK) {
    return status;
  }

  /* Reset driver clipping area to GLIB clipping region */
  return GLIB_applyClippingRegion(pContext);
}
