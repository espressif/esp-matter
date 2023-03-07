/***************************************************************************//**
 * @file
 * @brief Draws the graphics on the display
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

#include "graphics.h"
#include "em_types.h"
#include "glib.h"
#include "dmd.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include <string.h>
#include <stdio.h>

// Configuration defines
#define X_BORDER 5
#define Y_BORDER 2

// Globals
static uint32_t xOffset, yOffset;
static GLIB_Context_t glibContext;

// Macros
#define X(index) (2 * (index))
#define Y(index) (2 * (index) + 1)

// Declarations
static bool pointInTriangle(int x, int y, int32_t *polyPoints);

/**************************************************************************//**
 * @brief Initializes the graphics stack.
 * @note This function will /hang/ if errors occur (usually
 *       caused by faulty displays.
 *****************************************************************************/
void GRAPHICS_Init(void)
{
  EMSTATUS status;

  /* Initialize the display module. */
  status = DISPLAY_Init();
  if (DISPLAY_EMSTATUS_OK != status) {
    while (1) ;
  }

  /* Initialize the DMD module for the DISPLAY device driver. */
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (1) ;
  }

  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status) {
    while (1) ;
  }

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  /* Use Normal font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
}

void GRAPHICS_Sleep(void)
{
  // Put the display to sleep
  GLIB_displaySleep();
}

void GRAPHICS_Wakeup(void)
{
  GLIB_displayWakeUp();
}

/**************************************************************************//**
 * @brief This function draws the initial display screen
 *****************************************************************************/
void GRAPHICS_Update(void)
{
  DMD_updateDisplay();
}

void GRAPHICS_Clear(void)
{
  GLIB_clear(&glibContext);

  // Reset the offset values to their defaults
  xOffset = X_BORDER;
  yOffset = Y_BORDER;
}

void GRAPHICS_AppendString(char *str)
{
  GLIB_drawString(&glibContext,
                  str,
                  strlen(str),
                  xOffset,
                  yOffset,
                  0);
  yOffset += glibContext.font.fontHeight + glibContext.font.lineSpacing;
}

/**************************************************************************//**
 * @brief   Function to draw a triangle anchored at the given (x,y) location
 *
 * @param[in] x    The x location of the box that would contain this triangle.
 * @param[in] y    The y location of the box that would contain this triangle.
 * @param[in] size The size of the triangle to draw.
 * @param[in] up   Whether the triangle should point upwards or downwards.
 * @param[in] fillPercent  The percent to fill this triangle. If the value
                           is negative then fill from bottom to top.
 *****************************************************************************/
void GRAPHICS_InsertTriangle(uint32_t x,
                             uint32_t y,
                             uint32_t size,
                             bool up,
                             int8_t fillPercent)
{
  int32_t polyPoints[2 * 3];

  if (up) {
    // Start with the 'point'
    polyPoints[X(0)] = x + size / 2;
    polyPoints[Y(0)] = y;

    // Go to the bottom right corner
    polyPoints[X(1)] = x + size;
    polyPoints[Y(1)] = y + size;

    // Then to the bottom left corner
    polyPoints[X(2)] = x;
    polyPoints[Y(2)] = y + size;
  } else {
    // Start at the given corner
    polyPoints[X(0)] = x;
    polyPoints[Y(0)] = y;

    // Size is equal to width so move over by that amount
    polyPoints[X(1)] = x + size;
    polyPoints[Y(1)] = y;

    // Now make the 'point'
    polyPoints[X(2)] = x + size / 2;
    polyPoints[Y(2)] = y + size;
  }

  // Draw the triangle outline with the draw polygon function
  GLIB_drawPolygon(&glibContext, 3, polyPoints);

  // If the user wants to fill the triangle then create an appropriate bounding
  // box and check the points within it
  if ((fillPercent != 0) && (fillPercent >= -100) && (fillPercent <= 100)) {
    // Compute the fill rectangle to search
    int fillStartX = x;
    int fillStopX  = fillStartX + size;
    int fillStartY, fillStopY;

    if (fillPercent < 0) {
      fillPercent = -fillPercent;
      fillStopY  = y + size;
      fillStartY = fillStopY - (size * fillPercent) / 100;
    } else {
      fillStartY = y;
      fillStopY  = y + (size * fillPercent) / 100;
    }

    for (int i = fillStartX; i < fillStopX; i++) {
      for (int j = fillStartY; j < fillStopY; j++) {
        // If this point is within the triangle draw a point
        if (pointInTriangle(i, j, polyPoints)) {
          GLIB_drawPixel(&glibContext, i, j);
        }
      }
    }
  }
}

// Helper function to take the cross product of the vectors P0P1 and P1P2
// where the points P0, P1, and P2 are stored in the points array as P0x, P0y,
// P1x, ...
static int crossProduct(int32_t *points)
{
  return ((points[X(1)] - points[X(0)]) * (points[Y(2)] - points[Y(0)])
          - (points[Y(1)] - points[Y(0)]) * (points[X(2)] - points[X(0)]));
}

// Helper function to tell you if a point (x,y) lies within the triangle with
// the verticies passed in in polyPoints. This assumes a clockwise definition
// of the verticies in the array.
static bool pointInTriangle(int x, int y, int32_t *polyPoints)
{
  int32_t points[3 * 2];

  // Make sure we're on the right side of line AB
  points[X(0)] = polyPoints[X(0)];
  points[Y(0)] = polyPoints[Y(0)];
  points[X(1)] = polyPoints[X(1)];
  points[Y(1)] = polyPoints[Y(1)];
  points[X(2)] = x;
  points[Y(2)] = y;
  if (crossProduct(points) < 0) {
    return false;
  }

  // Make sure we're on the right side of line BC
  points[X(0)] = polyPoints[X(1)];
  points[Y(0)] = polyPoints[Y(1)];
  points[X(1)] = polyPoints[X(2)];
  points[Y(1)] = polyPoints[Y(2)];
  if (crossProduct(points) < 0) {
    return false;
  }

  // Make sure we're on the right side of line CA
  points[X(0)] = polyPoints[X(2)];
  points[Y(0)] = polyPoints[Y(2)];
  points[X(1)] = polyPoints[X(0)];
  points[Y(1)] = polyPoints[Y(0)];
  if (crossProduct(points) < 0) {
    return false;
  }

  return true;
}

/**************************************************************************//**
 * @brief   Register a callback function at the given frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] argument   Argument to be given to the function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  0 for successful or
 *         -1 if the requested frequency does not match the RTC frequency.
 *****************************************************************************/
int rtcIntCallbackRegister(void (*pFunction)(void*),
                           void* argument,
                           unsigned int frequency)
{
  (void)argument;
  (void)frequency;
  (void)pFunction;
  return 0;
}
