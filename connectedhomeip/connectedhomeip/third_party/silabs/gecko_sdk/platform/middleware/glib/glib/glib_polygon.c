/***************************************************************************//**
 * @file
 * @brief Silicon Labs Graphics Library: Polygon Drawing Routines
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

/* C-header files */
#include <stdint.h>

/* EM types */
#include "em_types.h"

/* GLIB header files */
#include "glib.h"

/* Polygon fill algorithm allocation sizes */
enum {
  MAX_CROSSES = 64, /* Maximum intersection points (arbitrary limit) */
};

/**************************************************************************//**
 * @brief
 * Draws a polygon using Bresnham's Midpoint Line Algorithm.
 *
 * This function draws a line between all points outlining the polygon.
 * The first and last point doesn't have to be the same. The function
 * automatically draws a line from the start point to the end point.
 *
 * @param pContext
 *   Pointer to a GLIB_Context_t which the line is drawn.
 *   The lines are drawn using the foreground color.
 * @param numPoints
 *   Number of points in the polygon ( Has to be greater than 1 )
 * @param polyPoints
 *   Pointer to array of polygon points.
 *   The points are laid out like this: polyPoints = {x1,y1,x2,y2 ... }
 *   Polypoints has to contain at least (numPoints * 2) entries
 *
 * @return
 * Returns GLIB_OK on if at least one element was drawn, or else error code
 *****************************************************************************/
EMSTATUS GLIB_drawPolygon(GLIB_Context_t *pContext,
                          uint32_t numPoints, const int32_t *polyPoints)
{
  EMSTATUS status;
  uint32_t drawnElements = 0;
  uint32_t point;
  int32_t firstX;
  int32_t firstY;
  int32_t startX;
  int32_t startY;
  int32_t endX = 0;
  int32_t endY = 0;

  /* Check arguments */
  if (pContext == NULL || polyPoints == NULL || numPoints < 2) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  startX = *polyPoints++;
  startY = *polyPoints++;
  firstX = startX;
  firstY = startY;

  /* Loop through the points by moving the pointer */
  for (point = 1; point < numPoints; point++) {
    endX = *polyPoints++;
    endY = *polyPoints++;

    /* Draw a line between each pair of points */
    status = GLIB_drawLine(pContext, startX, startY, endX, endY);
    if (status > GLIB_ERROR_NOTHING_TO_DRAW) {
      return status;
    }
    if (status == GLIB_OK) {
      drawnElements++;
    }

    startX = endX;
    startY = endY;
  }

  /* Draw a line from last point to first point */
  if ((endX != firstX) || (endY != firstY)) {
    status = GLIB_drawLine(pContext, firstX, firstY, endX, endY);
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
 * @brief
 * Draws a filled polygon using a scan line algorithm.
 *
 * This function draws a line between all points outlining the polygon.
 * The first and last point doesn't have to be the same. The function
 * automatically draws a line from the start point to the end point.
 *
 * @param pContext
 *   Pointer to a GLIB_Context_t where the polygon is drawn.
 *   The polygon drawn using the foreground color.
 * @param numPoints
 *   Number of points in the polygon ( Has to be greater than 1 )
 * @param polyPoints
 *   Pointer to array of polygon points.
 *   The points are laid out like this: polyPoints = {x1,y1,x2,y2 ... }
 *   Polypoints has to contain at least (numPoints * 2) entries
 *
 * @return
 * Returns GLIB_OK on success, otherwise a GLIB error code.
 *****************************************************************************/
EMSTATUS GLIB_drawPolygonFilled(GLIB_Context_t *pContext,
                                uint32_t numPoints, const int32_t *polyPoints)
{
  size_t i, j, numcrosses;
  int32_t clip_y0 = pContext->clippingRegion.yMin;
  int32_t clip_y1 = pContext->clippingRegion.yMax;
  int32_t cur_y, min_y, max_y;
  int32_t cross_x[MAX_CROSSES];
  int32_t curpoint_y, curpoint_x, prvpoint_y, prvpoint_x;

  /* Check arguments */
  if (pContext == NULL || polyPoints == NULL || numPoints < 2
      || numPoints > MAX_CROSSES) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  /* Find bounding box (respecting clipping region) */
  min_y = max_y = polyPoints[1];
  for (i = 1; i < numPoints; i++) {
    cur_y = polyPoints[2 * i + 1];
    min_y = (cur_y < min_y) ? cur_y : min_y;
    max_y = (cur_y > max_y) ? cur_y : max_y;
  }
  min_y = (min_y < clip_y0) ? clip_y0 : min_y;
  max_y = (max_y > clip_y1) ? clip_y1 : max_y;

  /* Find intersection points */
  for (cur_y = min_y; cur_y < max_y; cur_y++) {
    /* Find where the current line intersects each 2-tuple */
    numcrosses = 0;
    j = numPoints - 1;
    for (i = 0; i < numPoints; i++) {
      curpoint_y = polyPoints[i * 2 + 1];
      prvpoint_y = polyPoints[j * 2 + 1];
      if ((curpoint_y < cur_y && prvpoint_y >= cur_y)
          || (prvpoint_y < cur_y && curpoint_y >= cur_y)) {
        curpoint_x = polyPoints[i * 2];
        prvpoint_x = polyPoints[j * 2];
        cross_x[numcrosses] = (int) (curpoint_x
                                     + (cur_y - curpoint_y)
                                     / (float)(prvpoint_y - curpoint_y)
                                     * (float) (prvpoint_x - curpoint_x));
        numcrosses++;
      }
      j = i;
    }

    /* Draw horizontal lines between intersection point 2-tuples */
    for (i = 0; i < numcrosses; i += 2) {
      GLIB_drawLineH(pContext, cross_x[i], cur_y, cross_x[i + 1]);
    }
  }

  return GLIB_OK;
}
