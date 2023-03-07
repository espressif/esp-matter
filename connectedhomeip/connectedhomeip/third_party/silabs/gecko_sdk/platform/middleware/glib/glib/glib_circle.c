/***************************************************************************//**
 * @file
 * @brief Silicon Labs Graphics Library: Circle Drawing Routines
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

/* Local function prototypes */
static EMSTATUS GLIB_drawPartialCirclePoints(GLIB_Context_t *pContext,
                                             int32_t xCenter, int32_t yCenter,
                                             int32_t x, int32_t y, uint8_t bitMask);

/**************************************************************************//**
*  @brief
*  Draws a circle with center at x, y, and a radius
*
*  Draws a circle using the Midpoint Circle Algorithm. See Wikipedia for algorithm.
*  Algorithm is optimized to use only integer arithmetic, so no floating
*  point arithmetic is needed.
*
*  @param pContext
*  Pointer to a GLIB_Context_t in which the circle is drawn. The circle is drawn using
*  the foreground color.
*  @param xCenter
*  Center x-coordinate
*  @param yCenter
*  Center y-coordinate
*  @param radius
*  Radius of the circle
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawCircle(GLIB_Context_t *pContext, int32_t xCenter,
                         int32_t yCenter, uint32_t radius)
{
  return GLIB_drawPartialCircle(pContext, xCenter, yCenter, radius, 0xFF);
}

/**************************************************************************//**
*  @brief
*  Draws a partial circle with center at x, y, and a radius
*
*  Draws a partial circle using the Midpoint Circle Algorithm. Algorithm is
*  optimized to use only integer arithmetic, so no floating point arithmetic is
*  needed. The bitMask passed in decides which octant that should be drawn.
*  The octants is numbered 1 to 8 in counterclockwise order.
*
*  Example: bitMask == 4 draws only pixels in 3. octant (00000100).
*  bitMask == 5 draws only pixels in 3. and 1. octant (00000101).
*
*  @param pContext
*  Pointer to a GLIB_Context_t in which the circle is drawn. The circle is drawn using the foreground color.
*  @param xCenter
*  Center x-coordinate
*  @param yCenter
*  Center y-coordinate
*  @param radius
*  Radius of the circle
*  @param bitMask
*  Bitmask which decides which octants pixels should be drawn.
*  The LSB is 1. octant, and the MSB is 8. octant.
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawPartialCircle(GLIB_Context_t *pContext, int32_t xCenter,
                                int32_t yCenter, uint32_t radius, uint8_t bitMask)
{
  EMSTATUS status;
  int32_t x = 0;
  int32_t y = radius;
  int32_t d = 1 - radius;
  uint32_t drawnElements = 0;

  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Draw initial circle points */
  status = GLIB_drawPartialCirclePoints(pContext, xCenter, yCenter, x, y, bitMask);
  if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
    return status;
  }
  if (status == GLIB_OK) {
    drawnElements++;
  }

  /* Loops through all points from 0 to 45 degrees (1 octant) of the circle
   * (0 is defined straight upward) */
  while (x < y) {
    if (d < 0) {
      d += 2 * x + 1;
    } else {
      y--;
      d += 2 * (x - y) + 1;
    }

    /* Draws the circle points using 8-way symmetry */
    status = GLIB_drawPartialCirclePoints(pContext, xCenter, yCenter, x, y, bitMask);
    x++;
    if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
      return status;
    }
    if (status == GLIB_OK) {
      drawnElements++;
    }
  }
  return ((drawnElements == 0) ? GLIB_ERROR_NOTHING_TO_DRAW : GLIB_OK);
}

/**************************************************************************//**
*  @brief
*  Draws a filled circle with center at x, y, and a radius.
*
*  Draws a circle using the Midpoint Circle Algorithm and using horizontal lines.
*  See Wikipedia for algorithm.
*
*  @param pContext
*  Pointer to a GLIB_Context_t in which the circle is drawn. The circle is drawn using the foreground color.
*  @param xCenter
*  Center x-coordinate
*  @param yCenter
*  Center y-coordinate
*  @param radius
*  Radius of the circle
*
*  @return
*  Returns GLIB_OK on succes.
*  Returns GLIB_ERROR_NOTHING_TO_DRAW if none of the pixel coordinates were inside the clipping region.
*  Returns error code otherwise.
******************************************************************************/
EMSTATUS GLIB_drawCircleFilled(GLIB_Context_t *pContext, int32_t xCenter,
                               int32_t yCenter, uint32_t radius)
{
  EMSTATUS status;
  int32_t x = 0;
  int32_t y = radius;
  int32_t d = 1 - radius;
  uint32_t drawnElements = 0;

  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Draws the initial circle fill line */
  status = GLIB_drawLineH(pContext, xCenter - y, yCenter, xCenter + y);
  if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
    return status;
  }
  if (status == GLIB_OK) {
    drawnElements++;
  }

  /* Loops through all points from 0 to 45 degrees of the circle
   * (0 is defined straight upward) */
  while (x < y) {
    if (d < 0) {
      d += 2 * x + 1;
    } else {
      y--;
      d += 2 * (x - y) + 1;
    }

    /* Draws horizontal fill lines using 4 way symmetry */
    status = GLIB_drawLineH(pContext, xCenter - x, yCenter + y, xCenter + x);
    if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
      return status;
    }
    if (status == GLIB_OK) {
      drawnElements++;
    }

    status = GLIB_drawLineH(pContext, xCenter - y, yCenter + x, xCenter + y);
    if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
      return status;
    }
    if (status == GLIB_OK) {
      drawnElements++;
    }

    status = GLIB_drawLineH(pContext, xCenter - x, yCenter - y, xCenter + x);
    if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
      return status;
    }
    if (status == GLIB_OK) {
      drawnElements++;
    }

    status = GLIB_drawLineH(pContext, xCenter - y, yCenter - x, xCenter + y);
    if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
      return status;
    }
    if (status == GLIB_OK) {
      drawnElements++;
    }

    x++;
  }
  return ((drawnElements == 0) ? GLIB_ERROR_NOTHING_TO_DRAW : GLIB_OK);
}

/**************************************************************************//**
*  @brief
*  Draws circle points using 8-way symmetry.
*
*  Example: bitMask = 4 draws only pixels in 3. octant (00000100).
*  bitMask = 5 draws only pixels in 3. and 1. octant (00000101).
*
*  @param pContext
*  Pointer to a GLIB_Context_t in which the circle is drawn. The circle is drawn using the foreground color.
*  @param xCenter
*  Center x-coordinate
*  @param yCenter
*  Center y-coordinate
*  @param x
*  x-coordinate of circle point
*  @param y
*  y-coordinate of circle point
*  @param bitMask
*  Bitmask which decides which octants pixels should be drawn
*  The LSB is 1. octant, and the MSB is 8. octant. The octants are ordered from 1 to 8
*  in counterclockwise order.
*
*  @return
*  - Returns GLIB_OK on succes.
*  - Returns GLIB_ERROR_NOTHING_TO_DRAW if none of the points were drawn
*  - Returns error code otherwise
******************************************************************************/
static EMSTATUS GLIB_drawPartialCirclePoints(GLIB_Context_t *pContext,
                                             int32_t xCenter, int32_t yCenter,
                                             int32_t x, int32_t y, uint8_t bitMask)
{
  EMSTATUS status;
  uint32_t drawnElements = 0;
  uint32_t i;

  /* Draw the circle points using 8-way symmetry */
  int32_t  xOffsets[] = { y, x, -x, -y, -y, -x, x, y };
  int32_t  yOffsets[] = { -x, -y, -y, -x, x, y, y, x };

  i = 0;
  while (bitMask) {
    /* Pixel in i+1. octant */
    if (bitMask & 0x1) {
      status = GLIB_drawPixel(pContext, xCenter + xOffsets[i], yCenter + yOffsets[i]);
      if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
        return status;
      }
      if (status == GLIB_OK) {
        drawnElements++;
      }
    }
    bitMask >>= 1;
    i++;
  }
  return ((drawnElements == 0) ? GLIB_ERROR_NOTHING_TO_DRAW : GLIB_OK);
}
