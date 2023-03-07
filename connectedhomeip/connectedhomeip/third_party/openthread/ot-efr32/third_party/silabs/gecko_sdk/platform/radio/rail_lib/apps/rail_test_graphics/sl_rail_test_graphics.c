/***************************************************************************//**
 * @file
 * @brief Draws the graphics on the display
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

#include "app_common.h"
#include "sl_rail_test_graphics.h"
#include "sl_rail_test_graphics_config.h"
#include "em_types.h"
#include "glib.h"
#include "dmd.h"
#include "sl_board_control.h"

#include <string.h>
#include <stdio.h>

// Configuration defines
#define X_BORDER 5
#define Y_BORDER 2

// Globals
static uint32_t xOffset, yOffset;
static GLIB_Context_t glibContext;
static bool graphicsUpdateFlag = true;
static bool displayIsAwake;

// Macros
#define X(index) (2 * (index))
#define Y(index) (2 * (index) + 1)

#ifndef APP_DISPLAY_BUFFER_SIZE
#define APP_DISPLAY_BUFFER_SIZE 64
#endif

// Declarations
static bool pointInTriangle(int x, int y, int32_t *polyPoints);

/**************************************************************************//**
 * @brief Initializes the graphics stack.
 * @note This function will /hang/ if errors occur (usually
 *       caused by faulty displays.
 *****************************************************************************/
void sl_rail_test_graphics_init(void)
{
  /* Enable the memory lcd */
  if (SL_STATUS_OK != sl_board_enable_display()) {
    while (1) ;
  }

  /* Initialize the DMD module for the DISPLAY device driver. */
  if (DMD_OK != DMD_init(0)) {
    while (1) ;
  }

  if (GLIB_OK != GLIB_contextInit(&glibContext)) {
    while (1) ;
  }

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  /* Use Normal font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);

  displayIsAwake = true;
}

void sl_rail_test_graphics_sleep(void)
{
  if (displayIsAwake) {
    displayIsAwake = false;
    // Put the display to sleep
    EMSTATUS status = GLIB_displaySleep();
    while (status != 0)
      ;
  }
}

void sl_rail_test_graphics_wakeup(void)
{
  if (!displayIsAwake) {
    displayIsAwake = true;
    EMSTATUS status = GLIB_displayWakeUp();
    while (status != 0)
      ;
  }
}

/**************************************************************************//**
 * @brief This function draws the initial display screen
 *****************************************************************************/
void sl_rail_test_graphics_update(void)
{
  graphicsUpdateFlag = true;
}

void sl_rail_test_graphics_process_action(void)
{
  if (graphicsUpdateFlag) {
    graphicsUpdateFlag = false;

    char textBuf[APP_DISPLAY_BUFFER_SIZE];

    // Clear what's currently on screen
    sl_rail_test_graphics_clear();

    // Add the demo output strings
    sl_rail_test_graphics_append_string("\n"SL_RAIL_TEST_GRAPHICS_APP_NAME "\n");
    sl_rail_test_graphics_append_string("");
    snprintf(textBuf, APP_DISPLAY_BUFFER_SIZE, "Rx Count: %05lu",
             counters.receive % 100000);
    sl_rail_test_graphics_append_string(textBuf);
    snprintf(textBuf, APP_DISPLAY_BUFFER_SIZE, "Tx Count: %05lu",
             (counters.userTx + counters.ackTx) % 100000);
    sl_rail_test_graphics_append_string(textBuf);
    snprintf(textBuf, APP_DISPLAY_BUFFER_SIZE, "Channel: %d", channel);
    sl_rail_test_graphics_append_string(textBuf);
    sl_rail_test_graphics_append_string("");
    sl_rail_test_graphics_append_string("   Tx     Rx");

    // Draw Tx/Rx triangles if the timeout hasn't occurred
    sl_rail_test_graphics_insert_triangle(20, 80, 32, true,
                                          ((int8_t)((counters.userTx + counters.ackTx) % 10)) * -10);
    sl_rail_test_graphics_insert_triangle(76, 80, 32, false, (counters.receive % 10) * 10);

    DMD_updateDisplay();
  }
}

void sl_rail_test_graphics_clear(void)
{
  GLIB_clear(&glibContext);

  // Reset the offset values to their defaults
  xOffset = X_BORDER;
  yOffset = Y_BORDER;
}

void sl_rail_test_graphics_append_string(char *str)
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
void sl_rail_test_graphics_insert_triangle(uint32_t x,
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
