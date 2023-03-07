/***************************************************************************//**
 * @file
 * @brief Dot matrix display driver for DISPLAY device driver interface.
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

#include <stdint.h>
#include <stdbool.h>

#include "dmd.h"
#include "bsp_tdk.h"
#include "bspconfig.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "glib.h"

#include "dmd_etm043010edh6.h"

/******************************************************************************/
/*                                                                            */
/* Local function declarations                                                */
/*                                                                            */
/******************************************************************************/

static inline uint16_t colorTransform24To16bpp(uint8_t red,
                                               uint8_t green,
                                               uint8_t blue);

/******************************************************************************/
/*                                                                            */
/* Local variable declarations                                                */
/*                                                                            */
/******************************************************************************/

static bool                DMD_initialized;
static uint16_t           *DMD_frameBuffer;
static uint8_t             DMD_currentBuffer;
static DMD_DisplayGeometry DMD_dimensions;

/******************************************************************************/
/*                                                                            */
/* Global function definitions                                                */
/*                                                                            */
/******************************************************************************/

/***************************************************************************//**
 * @brief
 *    This function initializes the SPI driver as well as the display. It also
 *    ensures the whole display is black before it is turned on.
 *
 * @param initConfig
 *    This is not used, only included for compatibliity with other DMD
 *    implementation.
 *
 * @return
 *    DMD_OK if everything went ok
 ******************************************************************************/
EMSTATUS DMD_init(DMD_InitConfig *initConfig)
{
  (void) initConfig;
  EMSTATUS stat;

  stat = DMD_OK;

  if (DMD_initialized) {
    return DMD_OK;
  }

  DMD_dimensions.xSize      = DMD_HORIZONTAL_SIZE;
  DMD_dimensions.ySize      = DMD_VERTICAL_SIZE;
  DMD_dimensions.xClipStart = 0;
  DMD_dimensions.yClipStart = 0;
  DMD_dimensions.clipWidth  = DMD_HORIZONTAL_SIZE;
  DMD_dimensions.clipHeight = DMD_VERTICAL_SIZE;

  /* Enable EBI */
  DMD_frameBuffer   = (uint16_t *)BSP_getTftSRAMDirectDriveBaseAddress();
  DMD_currentBuffer = 0;

  /* Enable backlight */
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(DMD_DISPLAY_BACKLIGHT_PORT,
                  DMD_DISPLAY_BACKLIGHT_PIN,
                  gpioModePushPull, 1);
  GPIO_PinModeSet(DMD_DISPLAY_ENABLE_PORT,
                  DMD_DISPLAY_ENABLE_PIN,
                  gpioModePushPull, 1);

  DMD_initialized = 1;

  /* Fill the entire display with black color */
  DMD_writeColor(0, 0, 0x00, 0x00, 0x00,
                 DMD_dimensions.xSize * DMD_dimensions.ySize);

  return stat;
}

/***************************************************************************//**
 * @brief
 *    This function returns the display geometry
 *
 * @param **geometry
 *    Pointer to store address to geometry struct
 *
 * @return
 *    DMD_OK
 ******************************************************************************/
EMSTATUS DMD_getDisplayGeometry(DMD_DisplayGeometry **geometry)
{
  *geometry = &DMD_dimensions;

  return DMD_OK;
}

/***************************************************************************//**
 * @brief
 *    This functions allows for setting a clipping area when writing
 *    to the display. 0,0 is upper left corner, with positive direction
 *    down and to the right.
 *
 * @param xStart
 *    x-coordinate of upper left corner of crop. Must be a value between 0 and
 *    horizontal_size - 1
 *
 * @param yStart
 *    y-coordinate of upper left corner of crop. Must be a value between 0 and
 *    vertical_size - 1
 *
 * @param width
 *    Width in pixels of the cropped area
 *
 * @param height
 *    Height in pixels of the cropped area
 *
 * @return
 *    DMD_OK
 ******************************************************************************/
EMSTATUS DMD_setClippingArea(uint16_t xStart, uint16_t yStart,
                             uint16_t width, uint16_t height)
{
  if (  (xStart           > DMD_HORIZONTAL_SIZE)
        || ( (xStart + width)  > DMD_HORIZONTAL_SIZE)
        || (yStart           > DMD_VERTICAL_SIZE)
        || ( (yStart + height) > DMD_VERTICAL_SIZE) ) {
    return DMD_ERROR_PIXEL_OUT_OF_BOUNDS;
  }

  DMD_dimensions.xClipStart = xStart;
  DMD_dimensions.yClipStart = yStart;
  DMD_dimensions.clipWidth  = width;
  DMD_dimensions.clipHeight = height;

  return DMD_OK;
}

/***************************************************************************//**
 * @brief
 *    This function writes an array to the display.
 *    Each pixel consists of 2 bytes with the following structure:
 *       byte0[3:8] - Red (5 bits)
 *       byte0[0:3] - Green (3 high bits)
 *       byte1[5:8] - Green (3 low bits)
 *       byte1[0:5] - Blue (5 bits)
 *
 * @param x
 *    Starting x coordinate relative to clipping area
 *
 * @param y
 *    Start y coordinate relative to clipping area
 *
 * @param data
 *    Data to write to display
 *
 * @param numPixels
 *    Total amount of pixels to write to display. Size of array / 2
 *
 * @return
 *    DMD_OK
 ******************************************************************************/
EMSTATUS DMD_writeData(uint16_t x, uint16_t y,
                       const uint8_t data[], uint32_t numPixels)
{
  EMSTATUS stat;
  uint16_t xMax, xMin, yMax, yMin;
  uint16_t yInc;
  uint32_t startIdx;
  uint16_t *frameData;
  uint32_t i;

  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint16_t color;

  i = 0;
  stat = DMD_OK;

  xMin = DMD_dimensions.xClipStart + x;
  yMin = DMD_dimensions.yClipStart + y;
  xMax = DMD_dimensions.xClipStart + DMD_dimensions.clipWidth;
  yMax = DMD_dimensions.yClipStart + DMD_dimensions.clipHeight;
  yInc = DMD_dimensions.xSize - DMD_dimensions.clipWidth + x;

  /* Calculate pointer to first pixel */
  startIdx = (yMin * DMD_dimensions.xSize) + xMin;
  frameData = &DMD_frameBuffer[startIdx];

  for ( y = yMin; y < yMax; ++y ) {
    for ( x = xMin; x < xMax; ++x ) {
      red = data[i + 0];
      green = data[i + 1];
      blue = data[i + 2];

      color = colorTransform24To16bpp(red, green, blue);
      // Write data to framebuffer
      *frameData = color;
      frameData++;

      // Increment number of pixels transferred
      i = i + 3;;
      if ( i > numPixels * 3 ) {
        goto returnStatus;
      }
    }
    // Increment till start of next line
    frameData += yInc;
  }

  returnStatus:
  return stat;
}

/***************************************************************************//**
 * @brief
 *    This function writes numPixels pixels of the specified color to the
 *    display.
 *
 * @param x
 *    Starting x coordinate relative to clipping area
 *
 * @param y
 *    Start y coordinate relative to clipping area
 *
 * @param red, green, blue
 *    Color to write to display.
 *
 * @param numPixels
 *    Total amount of pixels to write to display. Size of array / 2
 *
 * @return
 *    DMD_OK
 ******************************************************************************/
EMSTATUS DMD_writeColor(uint16_t x, uint16_t y, uint8_t red,
                        uint8_t green, uint8_t blue, uint32_t numPixels)
{
  EMSTATUS stat;
  uint16_t xMax, xMin, yMax, yMin;
  uint16_t yInc;
  uint32_t startIdx;
  uint16_t *frameData;
  uint16_t color;

  stat = DMD_OK;

  xMin = DMD_dimensions.xClipStart + x;
  yMin = DMD_dimensions.yClipStart + y;
  xMax = DMD_dimensions.xClipStart + DMD_dimensions.clipWidth;
  yMax = DMD_dimensions.yClipStart + DMD_dimensions.clipHeight;
  yInc = DMD_dimensions.xSize - DMD_dimensions.clipWidth + x;

  /* Calculate pointer to first pixel */
  startIdx = (yMin * DMD_dimensions.xSize) + xMin;
  frameData = &DMD_frameBuffer[startIdx];

  color = colorTransform24To16bpp(red, green, blue);

  for ( y = yMin; y < yMax; ++y ) {
    for ( x = xMin; x < xMax; ++x ) {
      /* Write data to framebuffer */
      *frameData = color;
      frameData++;

      /* Count number of pixels to draw */
      numPixels--;
      if ( numPixels == 0 ) {
        goto returnStatus;
      }
    }
    /* Increment till start of next line */
    frameData += yInc;
  }

  returnStatus:

  return stat;
}

/***************************************************************************//**
 * @brief
 * Draws a bitmap
 *
 * Sets up a bitmap that starts at x0,y0 and draws bitmap.
 *
 * For monochrome displays, each 8-bit element contains 8 pixels values.
 *
 * For 3-bit RGB displays, each bit in the array are one color component (red,
 * green and blue) of the pixel, so that 3 bits represent one pixel (0xBGR).
 * Pixel 0: Bits 2:0 (0bBGR) of byte 0
 * Pixel 1: Bits 5:3 (0bBGR) of byte 0
 * Pixel 2: Bits 7:6 (0bGR) of byte 0 and bit 0 (0bB) of byte 1
 * Pixel 3: Bits 3:1 (0bBGR) of byte 1
 * ...
 *
 * For RGB displays with 8-bits per color, each pixel is represented by 24-bits,
 * with one byte for each of the red, green and blue components. The data has to
 * be organized like this: picData = { R, G, B, R, G, B, R, G, B ... }
 *
 * The pixels are ordered by increasing x coordinate, after the last pixel of a
 * row, the next pixel will be the first pixel on the next row.
 *
 * @param pContext
 * Pointer to a GLIB_Context_t in which the bitmap is drawn.
 * @param x
 * Start x-coordinate for bitmap
 * @param y
 * Start y-coordinate for bitmap
 * @param width
 * Width of picture
 * @param height
 * Height of picture
 * @param picData
 * Bitmap data
 *
 * @return
 * Returns GLIB_OK on success, or else error code
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

/***************************************************************************//**
 * @brief
 *    This function switches between the two possible memory areas.
 *    Call this function before drawing a scene in order to avoid tearing
 *
 * @return
 *    DMD_OK
 *
 ******************************************************************************/
EMSTATUS DMD_startDrawing(void)
{
  /* Alternate back buffer between the available frame buffers */
  if (DMD_currentBuffer == 0) {
    DMD_currentBuffer = 1;
    DMD_frameBuffer  += DMD_HORIZONTAL_SIZE * DMD_VERTICAL_SIZE;
  } else {
    DMD_currentBuffer = 0;
    DMD_frameBuffer  -= DMD_HORIZONTAL_SIZE * DMD_VERTICAL_SIZE;
  }

  /* Wait for current VSYNC to be over */
  while (!GPIO_PinInGet(BSP_CONFIG_EBI_VSNC_PORT, BSP_CONFIG_EBI_VSNC_PIN)) {
  }

  /* Wait for next VSYNC to begin */
  while (GPIO_PinInGet(BSP_CONFIG_EBI_VSNC_PORT, BSP_CONFIG_EBI_VSNC_PIN)) {
  }

  /* Drawing to back buffer is now ready */
  return DMD_OK;
}

/***************************************************************************//**
 * @brief
 *    This function sets the EBI TFT peripheral to display the framebuffer
 *    which is currently used for drawing.
 *    Call this function after drawing a scene in order to avoid tearing.
 *
 * @return
 *    DMD_OK on success
 *
 ******************************************************************************/
EMSTATUS DMD_stopDrawing(void)
{
  BSP_setTftSRAMDirectDriveBaseAddress( (void *)DMD_frameBuffer);

  return DMD_OK;
}

/***************************************************************************//**
 * @brief
 *    Get current framebuffer used by DMD for drawing (backbuffer).
 *
 * @param framebuffer
 *    Pointer to a framebuffer array.
 *    Gets set to DMD's current buffer.
 *
 * @return
 *    DMD_OK on success
 *
 ******************************************************************************/
EMSTATUS DMD_getFrameBuffer(void **framebuffer)
{
  *framebuffer = DMD_frameBuffer;

  return DMD_OK;
}

/******************************************************************************/
/*                                                                            */
/* Local function definitions                                                 */
/*                                                                            */
/******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Transforms a 24bpp pixel data into an 16bpp pixel
 *
 * @param red
 *    8-bit red component of the pixel
 * @param green
 *    8-bit green component of the pixel
 * @param blue
 *    8-bit blue component of the pixel
 *
 * @return
 *    16bpp value of pixel
 ******************************************************************************/
static inline uint16_t colorTransform24To16bpp(uint8_t red,
                                               uint8_t green,
                                               uint8_t blue)
{
  /* Transform each color into 6 bits by dropping the 2 LSB */
  red   = (red >> 3) & 0x1F;
  green = (green >> 2) & 0x3F;
  blue  = (blue >> 3) & 0x1F;

  /* Put it together to one 16bpp color number */
  return (red << 11) | (green << 5) | blue;
}
