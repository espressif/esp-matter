/***************************************************************************//**
 * @file
 * @brief Silicon Labs Graphics Library: Line Drawing Routines
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
#include <stdbool.h>

/* EM types */
#include "em_types.h"

/* GLIB Header files */
#include "glib.h"

/* Local function prototypes */
static uint8_t GLIB_getClipCode(GLIB_Context_t *pContext, int32_t x, int32_t y);
static bool GLIB_clipLine(GLIB_Context_t *pContext, int32_t *pX1, int32_t *pY1,
                          int32_t *pX2, int32_t *pY2);

/**************************************************************************//**
*  @brief
*  Draws a horizontal line from x1, y1 to x2, y2
*
*  @param pContext
*  Pointer to a GLIB_Context_t in which the line is drawn. The line is drawn using the
*  foreground color.
*
*  @param x1
*  Start x-coordinate
*  @param y1
*  Start y-coordinate
*  @param x2
*  End x-coordinate
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawLineH(GLIB_Context_t *pContext, int32_t x1, int32_t y1,
                        int32_t x2)
{
  EMSTATUS status;
  int32_t swap;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint32_t length;

  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Check if line is outside of clipping region */
  if ((y1 < pContext->clippingRegion.yMin) || (y1 > pContext->clippingRegion.yMax)) {
    return GLIB_ERROR_NOTHING_TO_DRAW;
  }

  /* Swap the coordinates if x1 is larger than x2 */
  if (x1 > x2) {
    swap = x1;
    x1   = x2;
    x2   = swap;
  }

  /* Check if entire line is outside clipping region */
  if ((x1 > pContext->clippingRegion.xMax) || (x2 < pContext->clippingRegion.xMin)) {
    return GLIB_ERROR_NOTHING_TO_DRAW;
  }

  /* Clip the line if necessary */
  if (x1 < pContext->clippingRegion.xMin) {
    x1 = pContext->clippingRegion.xMin;
  }
  if (x2 > pContext->clippingRegion.xMax) {
    x2 = pContext->clippingRegion.xMax;
  }

  /* Translate color and draw line using display driver */
  length = x2 - x1 + 1;
  status = DMD_setClippingArea(x1, y1, length, 1);
  if (status != DMD_OK) {
    return status;
  }

  GLIB_colorTranslate24bpp(pContext->foregroundColor, &red, &green, &blue);
  status = DMD_writeColor(0, 0, red, green, blue, length);
  if (status != DMD_OK) {
    return status;
  }

  /* Reset driver clipping area to GLIB clipping region */
  return GLIB_applyClippingRegion(pContext);
}

/**************************************************************************//**
*  @brief
*  Draws a vertical line from x1, y1 to x1, y2
*
*  @param pContext
*  Pointer to a GLIB_Context_t which the line is drawn. The line is drawn using the
*  foreground color.
*  @param x1
*  Start x-coordinate
*  @param y1
*  Start y-coordinate
*  @param y2
*  End y-coordinate
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawLineV(GLIB_Context_t *pContext, int32_t x1, int32_t y1,
                        int32_t y2)
{
  EMSTATUS status;
  int32_t swap;
  int32_t length;
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Check if line is outside of clipping region */
  if ((x1 < pContext->clippingRegion.xMin) || (x1 > pContext->clippingRegion.xMax)) {
    return GLIB_ERROR_NOTHING_TO_DRAW;
  }

  /* Swap the coordinates if y1 is larger than y2 */
  if (y1 > y2) {
    swap = y1;
    y1   = y2;
    y2   = swap;
  }

  /* Check if entire line is outside clipping region */
  if ((y1 > pContext->clippingRegion.yMax) || (y2 < pContext->clippingRegion.yMin)) {
    return GLIB_ERROR_NOTHING_TO_DRAW;
  }

  /* Clip the line if necessary */
  if (y1 < pContext->clippingRegion.yMin) {
    y1 = pContext->clippingRegion.yMin;
  }
  if (y2 > pContext->clippingRegion.yMax) {
    y2 = pContext->clippingRegion.yMax;
  }

  /* Translate color and draw line using display driver clipping (width = 1 => height <=> length) */
  length = y2 - y1 + 1;
  status = DMD_setClippingArea(x1, y1, 1, length);
  if (status != DMD_OK) {
    return status;
  }

  GLIB_colorTranslate24bpp(pContext->foregroundColor, &red, &green, &blue);
  status = DMD_writeColor(0, 0, red, green, blue, length);
  if (status != DMD_OK) {
    return status;
  }

  /* Reset driver clipping area to GLIB clipping region */
  return GLIB_applyClippingRegion(pContext);
}

/**************************************************************************//**
*  @brief
*  Gets the clip code for the point that is used by Cohen-Sutherland algorithm
*
*  @param pContext
*  Pointer to the GLIB_Context_t which holds the clipping region
*  @param x
*  Start x-coordinate
*  @param y
*  Start y-coordinate
*
*  @return
*  Returns the 4-bit clip code
*
******************************************************************************/
static uint8_t GLIB_getClipCode(GLIB_Context_t *pContext, int32_t x, int32_t y)
{
  uint8_t code = 0;

  /* The point is to the left of the clipping region */
  if (x < pContext->clippingRegion.xMin) {
    code |= 1;
  }

  /* The point is to the right of the clipping region */
  if (x > pContext->clippingRegion.xMax) {
    code |= 2;
  }

  /* The point is below the clipping region */
  if (y > pContext->clippingRegion.yMax) {
    code |= 4;
  }

  /* The point is above clipping region */
  if (y < pContext->clippingRegion.yMin) {
    code |= 8;
  }

  return code;
}

/**************************************************************************//**
*  @brief
*  Clips the line if necessary, using the Cohen-Sutherland algorithm for clipping
*  lines. See Wikipedia for algorithm.
*
*  The points are moved using the linear equations:
*  y=y0+slope*(x-x0),x=x0+(1/slope)*(y-y0)
*
*  @param pContext
*  Pointer to the GLIB_Context_t which holds the clipping region
*  @param pX1
*  Pointer to the start x-coordinate
*  @param pY1
*  Pointer to the start y-coordinate
*  @param pX2
*  Pointer to the end-x coordinate
*  @param pY2
*  Pointer to the end-y coordinate
*
*  @return
*  Returns true if the line is inside, or crosses into, the clipping region,
*  otherwise return false
******************************************************************************/
static bool GLIB_clipLine(GLIB_Context_t *pContext, int32_t *pX1,
                          int32_t *pY1, int32_t *pX2, int32_t *pY2)
{
  uint8_t currentCode, code1, code2;
  int32_t x = 0, y = 0;

  /* Compute the clipping code for the two points */
  code1 = GLIB_getClipCode(pContext, *pX1, *pY1);
  code2 = GLIB_getClipCode(pContext, *pX2, *pY2);

  while (true) {
    /* Case 1: Check if the points is inside the clipping rectangle */
    if ((code1 | code2) == 0) {
      return true;
    }

    /* Case 2: Check if the points can be trivially rejected */
    if (code1 & code2) {
      return false;
    }

    /* Case 3: Move the points so they can be either trivially accepted or rejected */
    /* Choose one of the points that are outside of the clipping region */
    if (code1) {
      currentCode = code1;
    } else {
      currentCode = code2;
    }

    /* Check if currentCode is to the left of the clipping region */
    if (currentCode & 1) {
      /* Move the point to the left edge of the clipping region */
      y = *pY1 + ((*pY2 - *pY1) * (pContext->clippingRegion.xMin - *pX1)) / (*pX2 - *pX1);
      x = pContext->clippingRegion.xMin;
    }
    /* Check if currentCode is to the right of the clipping region */
    else if (currentCode & 2) {
      /* Move the point to the right edge of the clipping region */
      x = pContext->clippingRegion.xMax;
      y = *pY1 + ((*pY2 - *pY1) * (pContext->clippingRegion.xMax - *pX1)) / (*pX2 - *pX1);
    }
    /* Check if currentCode is below the clipping region */
    else if (currentCode & 4) {
      /* Move the point to the bottom of the clipping region */
      y = pContext->clippingRegion.yMax;
      x = *pX1 + ((*pX2 - *pX1) * (pContext->clippingRegion.yMax - *pY1)) / (*pY2 - *pY1);
    }
    /* Check if currentCode is above the clipping region */
    else if (currentCode & 8) {
      /* Move the point to the top of the clipping region */
      y = pContext->clippingRegion.yMin;
      x = *pX1 + ((*pX2 - *pX1) * (pContext->clippingRegion.yMin - *pY1)) / (*pY2 - *pY1);
    }

    /* Determine which point is moved and set the new coordinates */
    if (code1) {
      *pX1 = x;
      *pY1 = y;
      /* Compute new clipCode */
      code1 = GLIB_getClipCode(pContext, x, y);
    } else {
      *pX2 = x;
      *pY2 = y;
      /* Compute new clipCode */
      code2 = GLIB_getClipCode(pContext, x, y);
    }
  }
}

/**************************************************************************//**
*  @brief
*  Draws a line from x1,y1 to x2, y2
*
*  Draws a straight line using the Bresnham's Midpoint Line Algorithm.
*  Checks for vertical and horizontal line.
*
*  @param pContext
*  Pointer to the GLIB_Context_t which holds the clipping region
*  @param x1
*  Start x-coordinate
*  @param y1
*  Start y-coordinate
*  @param x2
*  End x-coordinate
*  @param y2
*  End y-coordinate
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawLine(GLIB_Context_t *pContext, int32_t x1, int32_t y1,
                       int32_t x2, int32_t y2)
{
  EMSTATUS status;
  int32_t error;
  int32_t deltaX;
  int32_t deltaY;
  int32_t yMotion;
  int32_t xMotion;
  bool steepLine = false;
  int32_t yStep = 1;

  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Use simple algorithm for vertical line */
  if (x1 == x2) {
    return GLIB_drawLineV(pContext, x1, y1, y2);
  }

  /* Use simple algorithm for horizontal line */
  if (y1 == y2) {
    return GLIB_drawLineH(pContext, x1, y1, x2);
  }

  /* Clip the line against the clipping region */
  if (!GLIB_clipLine(pContext, &x1, &y1, &x2, &y2)) {
    return GLIB_ERROR_NOTHING_TO_DRAW;
  }

  /* Determine if steep or not steep
   * (Steep means more motion in Y-direction than X-direction) */
  yMotion = (y2 > y1) ? (y2 - y1) : (y1 - y2);
  xMotion = (x2 > x1) ? (x2 - x1) : (x1 - x2);
  if (yMotion > xMotion) {
    /* If line is steep, swap x and y values */
    steepLine = true;

    error = x1;
    x1    = y1;
    y1    = error;

    error = x2;
    x2    = y2;
    y2    = error;
  }

  /* Place the leftmost point in x1, y1 */
  if (x2 < x1) {
    /* Swap x-values */
    error = x1;
    x1    = x2;
    x2    = error;

    /* Swap y-values */
    error = y1;
    y1    = y2;
    y2    = error;
  }

  /* Compute the differences between the points */
  deltaX = x2 - x1;
  deltaY = (y2 > y1) ? (y2 - y1) : (y1 - y2);

  /* Set error to negative half deltaX ? */
  error = -deltaX / 2;

  /* Determine which direction to step in */
  if (y2 < y1) {
    yStep = -1;
  }

  /* Loop through all points along the x-axis */
  for (; x1 <= x2; x1++) {
    if (steepLine) {
      /* If steep, swap x and y coordinates */
      status = GLIB_drawPixel(pContext, y1, x1);
    } else {
      status = GLIB_drawPixel(pContext, x1, y1);
    }

    if (status != GLIB_OK) {
      return status;
    }

    error += deltaY;

    if (error > 0) {
      y1    += yStep;
      error -= deltaX;
    }
  }

  return GLIB_OK;
}
