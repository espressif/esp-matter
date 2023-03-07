/***************************************************************************//**
 * @file
 * @brief Silicon Labs Graphics Library: Bitmap Drawing Routines
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
*  Draws a bitmap
*
*  Sets up a bitmap that starts at x0,y0 and draws bitmap.
*
*  For monochrome displays, each 8-bit element contains 8 pixel values. A bit
*  value of 1 indicates a white pixel, while 0 indicates a black pixel. Note
*  that this format may vary with the DMD implementation.
*
*  For 3-bit RGB displays, each bit in the array are one color component (red,
*  green and blue) of the pixel, so that 3 bits represent one pixel (0xBGR).
*  Pixel 0: Bits 2:0 (0bBGR) of byte 0
*  Pixel 1: Bits 5:3 (0bBGR) of byte 0
*  Pixel 2: Bits 7:6 (0bGR) of byte 0 and bit 0 (0bB) of byte 1
*  Pixel 3: Bits 3:1 (0bBGR) of byte 1
*  ...
*
*  For RGB displays with 8-bits per color, each pixel is represented by 24-bits,
*  with one byte for each of the red, green and blue components. The data has to
*  be organized like this: picData = { R, G, B, R, G, B, R, G, B ... }
*
*  The pixels are ordered by increasing x coordinate, after the last pixel of a
*  row, the next pixel will be the first pixel on the next row.
*
*  @param pContext
*  Pointer to a GLIB_Context_t in which the bitmap is drawn.
*  @param x
*  Start x-coordinate for bitmap
*  @param y
*  Start y-coordinate for bitmap
*  @param width
*  Width of picture
*  @param height
*  Height of picture
*  @param picData
*  Bitmap data
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawBitmap(GLIB_Context_t *pContext, int32_t x, int32_t y,
                         uint32_t width, uint32_t height, const uint8_t *picData)
{
  EMSTATUS status;

  /* Set display clipping area for bitmap */
  status = DMD_setClippingArea(x, y, width, height);
  if (status != DMD_OK) {
    return status;
  }

  /* Write bitmap to display */
  status = DMD_writeData(0, 0, picData, width * height);
  if (status != DMD_OK) {
    return status;
  }

  /* Reset driver clipping area to GLIB clipping region */
  return GLIB_applyClippingRegion(pContext);
}

/**************************************************************************//**
*  @brief
*  Inverts each bit of the bitmap.
*
*  For monochrome displays, the result is an inversion of the image.
*
*  For RGB displays, the result is the negative of the input image.
*
*  @note
*  The function inverts entire bytes, meaning that any bits contained within
*  the array that may not be part of the image will also be inverted.
*
*  @param pContext
*  Pointer to a GLIB_Context_t in which the bitmap is drawn.
*  @param bitmapSize
*  Size of the bitmap array in terms of bytes.
*  @param picData
*  Bitmap data.
******************************************************************************/
void GLIB_invertBitmap(GLIB_Context_t *pContext, uint32_t bitmapSize,
                       uint8_t *picData)
{
  (void) pContext;  /* Suppress compiler warning: unused parameter. */

  /* Invert the entire array, byte by byte. */
  uint32_t i;
  for (i = 0; i < bitmapSize; ++i) {
    picData[i] = ~(picData[i]);
  }
}
