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
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "display.h"
#include "dmd.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* DISPLAY device number to use. */
#define DISPLAY_DEVICE_NO                           (0)

/* Definitions for DIRTY word manipulations. */
#define DIRTY_WORD_BITS_LOG2       (5)
#define DIRTY_WORD_BITS_LOG2_MASK  ((1 << DIRTY_WORD_BITS_LOG2) - 1)

/* Definitions for RGB_3BIT mode */
#define RGB_3BIT_BITS_PER_PIXEL  3

/* Local variables */
static bool  moduleInitialized = false;

/* Display device structure. */
static DISPLAY_Device_t      displayDevice;
static DISPLAY_PixelMatrix_t pixelMatrixBuffer = NULL;

/* Dimensions of the display */
static DMD_DisplayGeometry dimensions;

/* Dirty rows flags.
   The dirty table contains one bit per row/line on the display which
   indicates whether the corresponding row/line is dirty (written to without
   having been updated on the display. */
uint32_t dirtyRows[(DISPLAY0_WIDTH + (sizeof(uint32_t) * 8 - 1)) / sizeof(uint32_t) / 8];

/* To become API functions later. */
EMSTATUS DMD_allocateFramebuffer(void **framebuffer);
EMSTATUS DMD_freeFramebuffer(void *framebuffer);
EMSTATUS DMD_copyFramebuffer (void *dst, void *src);

/**************************************************************************//**
*  @brief
*  Initializes the DIDPLAY driver module
*
*  @param initConfig
*     Not used in this DMD module.
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_init(DMD_InitConfig *initConfig)
{
  EMSTATUS status;
  (void)   initConfig;  /* Suppress compiler warning. */

  if (moduleInitialized) {
    return DMD_OK;
  }

  /* Initialize the DISPLAY module. */
  status = DISPLAY_Init();
  if (DISPLAY_EMSTATUS_OK != status) {
    return status;
  }

  /* Retrieve the properties of the DISPLAY. */
  status = DISPLAY_DeviceGet(DISPLAY_DEVICE_NO, &displayDevice);
  if (DISPLAY_EMSTATUS_OK != status) {
    return status;
  }

  /* Allocate the default framebuffer. */
  status = DMD_allocateFramebuffer(&pixelMatrixBuffer);
  if (DMD_OK != status) {
    return status;
  }
  if (NULL == pixelMatrixBuffer) {
    return DMD_ERROR_NOT_ENOUGH_MEMORY;
  }

  /* Set up dimensions of the display */
  dimensions.xSize = displayDevice.geometry.width;
  dimensions.ySize = displayDevice.geometry.height;

  /* At initialization, the clip is the entire display */
  dimensions.xClipStart = 0;
  dimensions.yClipStart = 0;
  dimensions.clipWidth  = dimensions.xSize;
  dimensions.clipHeight = dimensions.ySize;

  moduleInitialized = true;

  /* Fill the entire display with black color */
  DMD_writeColor(0, 0, 0x00, 0x00, 0x00, dimensions.xSize * dimensions.ySize);

  return DMD_OK;
}

/**************************************************************************//**
*  \brief
*  Get the dimensions of the display and of the current clipping area
*
*  \return
*  DMD_Dimensions structure containing the size of the display and the
*  clipping area
******************************************************************************/
EMSTATUS DMD_getDisplayGeometry(DMD_DisplayGeometry **geometry)
{
  if (!moduleInitialized) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }
  *geometry = &dimensions;

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Sets the clipping area. All coordinates given to writeData/writeColor/readData
*  are relative to this clipping area.
*
*  @param xStart
*  X coordinate of the upper left corner of the clipping area
*  @param yStart
*  Y coordinate of the upper left corner of the clipping area
*  @param width
*  Width of the clipping area
*  @param height
*  Height of the clipping area
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_setClippingArea(uint16_t xStart, uint16_t yStart,
                             uint16_t width, uint16_t height)
{
  if (!moduleInitialized) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  /* Check parameters */
  if (xStart + width > dimensions.xSize
      || yStart + height > dimensions.ySize) {
    return DMD_ERROR_PIXEL_OUT_OF_BOUNDS;
  }

  if (width == 0 || height == 0) {
    return DMD_ERROR_EMPTY_CLIPPING_AREA;
  }

  /* Update the dimensions structure */
  dimensions.xClipStart = xStart;
  dimensions.yClipStart = yStart;
  dimensions.clipWidth  = width;
  dimensions.clipHeight = height;

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Draws pixels to the display
*
*  @param x
*  X coordinate of the first pixel to be written, relative to the clipping area
*  @param y
*  Y coordinate of the first pixel to be written, relative to the clipping area
*  @param data
*  Array containing the pixel data.
*  For monochrome displays, each 8-bit element contains 8 pixels values. A
*  pixel value of 1 means white, while a value of 0 means black.
*  For RGB displays, each bit in the array are one color component of the pixel,
*  so that 3 bits represent one pixel. The pixels are ordered by increasing x
*  coordinate, after the last pixel of a row, the next pixel will be the first
*  pixel on the next row.
*  @param numPixels
*  Number of pixels to be written
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_writeData(uint16_t x, uint16_t y, const uint8_t data[],
                       uint32_t numPixels)
{
  uint32_t clipRemaining;

  if (!moduleInitialized) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  if (NULL == pixelMatrixBuffer) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  /* Number of pixels from the first pixel (given by x and y) to the end
   * of the clipping area */
  clipRemaining = (dimensions.clipHeight - y) * dimensions.clipWidth - x;

  /* Check that the length of data isn't longer than the number of pixels
   * in the rest of the clipping area */
  if (numPixels > clipRemaining) {
    return DMD_ERROR_TOO_MUCH_DATA;
  }

  /* Write data */
  switch (displayDevice.addressMode) {
    default:
    case DISPLAY_ADDRESSING_BY_ROWS_AND_COLUMNS:
      /* Not supported yet. */
      return DMD_ERROR_NOT_SUPPORTED;

    case DISPLAY_ADDRESSING_BY_ROWS_ONLY:
    {
      unsigned int rowPixels;
      uint8_t      pixelData = 0;
      int          numBytesToCopy;
      uint8_t      pixelMask;
      int          pixelBit     = 0;
      uint8_t      matrixByte;
      uint8_t     *pStartRow;
      uint8_t     *pDst;
      int          rows          = 0;
      int          bytesPerRow   = displayDevice.geometry.stride >> 3;
    #if defined(DISPLAY_COLOUR_MODE_IS_RGB_3BIT)
      int        pixelSrcByte = 0;
      int        pixelSrcBit  = 0;
    #endif

      /* Adjust y to account for clipping. */
      y += dimensions.yClipStart;

      pStartRow = (uint8_t*) pixelMatrixBuffer + y * bytesPerRow;

      /* Write pixel data to the pixelMatrix buffer. */
      while (numPixels) {
        /* Determine how many bits to write on the current row/line. */
        rowPixels =  numPixels > (unsigned int)(dimensions.clipWidth - x)
                    ? (unsigned int)(dimensions.clipWidth - x) : numPixels;

        numPixels -= rowPixels;

        pDst = pStartRow + rows * bytesPerRow;

        /* Adjust x to account for clipping. */
        x += dimensions.xClipStart;

        switch (displayDevice.colourMode) {
        #if defined(DISPLAY_COLOUR_MODE_IS_RGB_3BIT)
          uint32_t *dataWord;
          int       pixelByte;

          case DISPLAY_COLOUR_MODE_RGB_3BIT:

            /* Calculate which byte the first pixel is going to be written to */
            pixelByte = (x * RGB_3BIT_BITS_PER_PIXEL) / 8;

            /* Calculate which bit to start writing pixel data to */
            pixelBit = (x * RGB_3BIT_BITS_PER_PIXEL) % 8;

            /* Fill in part of first byte that is not modified */
            matrixByte = pDst[pixelByte] & (0xff >> (8 - pixelBit));

            /* Go through pixels to write on this row */
            while (rowPixels) {
              /* Fill current byte with pixel data */
              for (; pixelBit < 8; pixelBit += RGB_3BIT_BITS_PER_PIXEL) {
                if (rowPixels) {
                  /* Read out data for the pixel */
                  dataWord = (uint32_t *) &data[pixelSrcByte];
                  pixelData = (uint8_t) (*dataWord >> pixelSrcBit) & 0x7;
                  pixelSrcBit += RGB_3BIT_BITS_PER_PIXEL;

                  /* Write pixeldata to the byte to be written to the buffer */
                  matrixByte |= pixelData << pixelBit;

                  /* If we cross to the next byte in the source we need to
                     move our indexes */
                  if (pixelSrcBit > 8) {
                    pixelSrcBit -= 8;
                    pixelSrcByte++;
                  }
                  rowPixels--;
                } else { /* Copy unmodified bits when there are no more pixels */
                  matrixByte |= pDst[pixelByte] & (0xff << pixelBit);
                  break;
                }
              }

              /* Store byte */
              pDst[pixelByte] = matrixByte;

              pixelByte++;
              matrixByte = 0;

              /* If the last pixel written crosses byte boundary we need to
                 write the rest of the bits to the next byte */
              if (pixelBit > 8) {
                /* First we write the remaining pixel bits */
                matrixByte = pixelData >> (RGB_3BIT_BITS_PER_PIXEL + 8 - pixelBit);

                /* Then we write these new bits to the next byte while keeping
                   the rest of the bits intact */
                pDst[pixelByte] = matrixByte
                                  | (pDst[pixelByte] & (0xff << (pixelBit - 8)));
              }
              pixelBit = pixelBit % 8; /* Truncate pixel index for next byte */
            }
            break;
        #endif

          case DISPLAY_COLOUR_MODE_MONOCHROME:
          case DISPLAY_COLOUR_MODE_MONOCHROME_INVERSE:

            /* If the start pixel (x) or the corresponding data bit
               (pixelBit) are not aligned on a 8-bit boundary or there are
               less than 8 bits to copy to the current row we copy pixel by
               pixel. */
            if ( (0 != (x & 0x7))
                 || (0 != (pixelBit & 0x7))
                 || (rowPixels < 8) ) {
              rowPixels += x;
              for (; x < rowPixels; x++, pixelBit++) {
                pixelData = (data[pixelBit >> 3] >> (pixelBit & 0x7)) & 0x1;
                /* Write pixel data to the pixelMatrix buffer. */
                if ( ( (displayDevice.colourMode
                        == DISPLAY_COLOUR_MODE_MONOCHROME_INVERSE)
                       && pixelData)
                     || ( (displayDevice.colourMode
                           == DISPLAY_COLOUR_MODE_MONOCHROME)
                          && (0 == pixelData) ) ) {
                  pDst[x >> 3] |= 1 << (x & 0x7);
                } else {
                  pDst[x >> 3] &= ~(1 << (x & 0x7));
                }
              }
            } else {
              /* The start pixel and it's corresponding data bit are aligned on
                 an 8-bit boundary and there are more than 8 bits to copy.
                 Use memcpy to copy pixel bits to the current row if the display
                 is not MONOCHROME, and copy byte-by-byte if the display is
                 MONOCHROME_INVERSE. Take special care of potential remaining
                 bits in the last byte on the row. */

              pDst += x >> 3;

              numBytesToCopy = rowPixels >> 3;

              if (numBytesToCopy) {
                if (displayDevice.colourMode
                    == DISPLAY_COLOUR_MODE_MONOCHROME_INVERSE) {
                  /* We can copy data continuosly from start to end. */
                  memcpy(pDst, &data[pixelBit >> 3], numBytesToCopy);
                  pixelBit  += numBytesToCopy << 3;
                } else {
                  /* Inverse display. We can copy byte by byte, since each bit
                     must be flipped */
                  for (; x <= rowPixels - 8; x += 8, pixelBit += 8) {
                    pixelData = data[pixelBit >> 3];
                    pDst[x >> 3] = ~pixelData;
                  }
                }

                rowPixels -= numBytesToCopy << 3;
                pDst      += numBytesToCopy;
              }

              /* If there are remaining pixels on this row,
                 write them into the last byte. */
              if (rowPixels) {
                /* Copy the remaining pixels into last byte of pixel buffer. */
                matrixByte = *pDst;
                pixelMask = (1 << rowPixels) - 1;
                if (displayDevice.colourMode
                    == DISPLAY_COLOUR_MODE_MONOCHROME_INVERSE) {
                  matrixByte &= ~pixelMask;  /* Set selected bits to 0 */
                  matrixByte |= data[pixelBit >> 3] & pixelMask;
                } else {
                  matrixByte |= pixelMask;  /* Set selected bits to 1 */
                  matrixByte &= ~(data[pixelBit >> 3] & pixelMask);
                }
                *pDst = matrixByte;
                pixelBit += rowPixels;
              }
            }
            break;
          default:
            break;
        }

        /* Mark row/line as dirty */
        dirtyRows[(y + rows) >> DIRTY_WORD_BITS_LOG2] |=
          1 << ((y + rows) & DIRTY_WORD_BITS_LOG2_MASK);

        /* Update variables for next row. */
        rows++;
        x = 0;
      }

#ifdef UPDATE_PER_WRITE_CALL
      /* Update the display device now. */
      displayDevice.pPixelMatrixDraw(&displayDevice,
                                     pStartRow,
                                     0,
                                     displayDevice.geometry.width,
                                     y,
                                     rows);
#endif
    }
    break;
  }

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Reads data from display memory
*  DOESN'T WORK yet - TIMING ISSUE?
*
*  @param x
*  X coordinate of the first pixel to be read, relative to the clipping area
*  @param y
*  Y coordinate of the first pixel to be read, relative to the clipping area
*  @param data
*  Pointer to where the pixel data will be stored
*  @param numPixels
*  Number of pixels to be read
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_readData(uint16_t x, uint16_t y,
                      uint8_t data[], uint32_t numPixels)
{
  (void) x;          /* Suppress compiler warning: unused parameter. */
  (void) y;          /* Suppress compiler warning: unused parameter. */
  (void) data;       /* Suppress compiler warning: unused parameter. */
  (void) numPixels;  /* Suppress compiler warning: unused parameter. */

  return DMD_ERROR_NOT_SUPPORTED;
}

/**************************************************************************//**
*  \brief
*  Draws a number of pixels of the same color to the display
*
*  @param x
*  X coordinate of the first pixel to be written, relative to the clipping area
*  @param y
*  Y coordinate of the first pixel to be written, relative to the clipping area
*  @param red
*  Red component of the color
*  @param green
*  Green component of the color
*  @param blue
*  Blue component of the color
*  @param numPixels
*  Number of pixels to be written
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_writeColor(uint16_t x, uint16_t y, uint8_t red,
                        uint8_t green, uint8_t blue, uint32_t numPixels)
{
  (void) red;     /* Suppress compiler warning: unused parameter. */
  (void) green;   /* Suppress compiler warning: unused parameter. */
  (void) blue;    /* Suppress compiler warning: unused parameter. */

  if (!moduleInitialized) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  if (NULL == pixelMatrixBuffer) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  switch (displayDevice.addressMode) {
    default:
    case DISPLAY_ADDRESSING_BY_ROWS_AND_COLUMNS:
      /* Not supported yet. */
      return DMD_ERROR_NOT_SUPPORTED;

    case DISPLAY_ADDRESSING_BY_ROWS_ONLY:
    {
      unsigned int rowPixels;
      int          byteOffset;
      int          numBytesToCopy;
      uint8_t      pixelMask;
      uint8_t      matrixByte;
      uint8_t     *pStartRow;
      uint8_t     *pDst;
      int          rows = 0;
      int          bytesPerRow  = displayDevice.geometry.stride / 8;
      uint8_t      pixelData;

      /* Adjust y to account for clipping. */
      y += dimensions.yClipStart;

      pStartRow = (uint8_t*) pixelMatrixBuffer + y * bytesPerRow;

      /* Write one row at a time until there are no more pixels to be written */
      while (numPixels) {
        /* Determine how many pixels to write on the current row */
        rowPixels = numPixels > (unsigned int)(dimensions.clipWidth - x)
                    ? (unsigned int)(dimensions.clipWidth - x) : numPixels;

        numPixels -= rowPixels;

        /* Adjust x to account for clipping. */
        x += dimensions.xClipStart;

        pDst = pStartRow + rows * bytesPerRow;

        switch (displayDevice.colourMode) {
        #if defined(DISPLAY_COLOUR_MODE_IS_RGB_3BIT)
          int pixelByte;
          int pixelBit;

          case DISPLAY_COLOUR_MODE_RGB_3BIT:

            pixelData = ((red & 0x80) >> 7)
                        | ((green & 0x80) >> 6)
                        | ((blue & 0x80) >> 5);

            /* Calculate what byte the first pixel is in */
            pixelByte = (x * RGB_3BIT_BITS_PER_PIXEL) / 8;

            /* Calculate the which bit to start writing pixel data to */
            pixelBit = (x * RGB_3BIT_BITS_PER_PIXEL) % 8;

            /* Fill in part of first byte that is not modified */
            matrixByte = pDst[pixelByte] & (0xff >> (8 - pixelBit));

            /* Go through pixels to write on this row */
            while (rowPixels) {
              /* Fill current byte with pixel data */
              for (; pixelBit < 8; pixelBit += RGB_3BIT_BITS_PER_PIXEL) {
                if (rowPixels) {
                  matrixByte |= pixelData << pixelBit;
                  rowPixels--;
                }
                /* If there are still bits left in byte, but no more pixels to
                   write, we fill these bits with unmodified data */
                else {
                  matrixByte |= pDst[pixelByte] & (0xff << pixelBit);
                  break;
                }
              }

              /* Store byte */
              pDst[pixelByte] = matrixByte;

              pixelByte++;
              matrixByte = 0;

              /* If the last pixel written crosses byte boundary we need to
                 write the rest of the bits to the next byte */
              if (pixelBit > 8) {
                /* First we write the remaining pixel bits */
                matrixByte = pixelData >> (RGB_3BIT_BITS_PER_PIXEL + 8 - pixelBit);

                /* Then we write these new bits to the next byte while keeping
                   the rest of the bits intact */
                pDst[pixelByte] = matrixByte
                                  | (pDst[pixelByte] & (0xff << (pixelBit - 8)));
              }
              pixelBit = pixelBit % 8; /* Truncate pixel index for next byte */
            }
            break;
        #endif

          case DISPLAY_COLOUR_MODE_MONOCHROME:
          case DISPLAY_COLOUR_MODE_MONOCHROME_INVERSE:
            pixelData = green ? 0x00 : 0xff;
            if (displayDevice.colourMode == DISPLAY_COLOUR_MODE_MONOCHROME_INVERSE) {
              pixelData = ~pixelData;
            }
            /* Write pixel data to the pixelMatrix buffer. */
            if (rowPixels < 8) {
              rowPixels += x;
              if (pixelData) {
                for (; x < rowPixels; x++) {
                  pDst[x >> 3] |= 1 << (x & 0x7);
                }
              } else {
                for (; x < rowPixels; x++) {
                  pDst[x >> 3] &= ~(1 << (x & 0x7));
                }
              }
            } else {
              byteOffset = x & 0x7;

              pDst += x >> 3;

              if (byteOffset) {
                /* Copy the pixels into first byte of the pixelMatrix buffer. */
                matrixByte = *pDst;
                pixelMask = (1 << byteOffset) - 1;
                matrixByte &= pixelMask;
                matrixByte |= pixelData & ~pixelMask;
                *pDst = matrixByte;
                pDst++;
                rowPixels -= 8 - byteOffset;
              }

              /* Now, remaining pixels start is 8-bit aligned. Copy the corresponding
                 number of bytes, then if there are remaining bits, copy them correctly
                 into the last byte. */
              numBytesToCopy = rowPixels >> 3;

              if (numBytesToCopy) {
                /* We can copy data continuosly from start to end. */
                memset(pDst, pixelData, numBytesToCopy);
                rowPixels  -= numBytesToCopy << 3;
                pDst       += numBytesToCopy;
              }

              /* If there are remaining pixels on this row,
                 write them into the last byte. */
              if (rowPixels) {
                /* Copy the remaining pixels into last byte of pixelMatrix buffer. */
                matrixByte = *pDst;
                pixelMask = (1 << rowPixels) - 1;
                matrixByte &= ~pixelMask;
                matrixByte |= pixelData & pixelMask;
                *pDst = matrixByte;
              }
            }

            break;
          default:
            break;
        }

        /* Mark row/line as dirty */
        dirtyRows[(y + rows) >> DIRTY_WORD_BITS_LOG2] |=
          1 << ((y + rows) & DIRTY_WORD_BITS_LOG2_MASK);

        /* Update variable for next row/line. */
        x = 0;
        rows++;
      }

#ifdef UPDATE_PER_WRITE_CALL
      /* Update the display device now. */
      displayDevice.pPixelMatrixDraw(&displayDevice,
                                     pStartRow,
                                     0,
                                     displayDevice.geometry.width,
                                     y,
                                     rows);
#endif
    }
    break;
  }

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Turns off the display and puts it into sleep mode
*  Does not turn off backlight
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_sleep(void)
{
  return displayDevice.pDisplayPowerOn(&displayDevice, false);
}

/**************************************************************************//**
*  @brief
*  Wakes up the display from sleep mode
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_wakeUp(void)
{
  return displayDevice.pDisplayPowerOn(&displayDevice, true);
}

/**************************************************************************//**
*  @brief
*  Set horizontal and vertical flip mode of display controller
*
*  @param hor
*  Set to flip display horizontally
*
*  @param ver
*  Set to flip display vertically
*
*  @return
*  Returns DMD_OK if successful, error otherwise.
******************************************************************************/
EMSTATUS DMD_flipDisplay(int horizontal, int vertical)
{
  (void) horizontal;    /* Suppress compiler warning: unused parameter. */
  (void) vertical;      /* Suppress compiler warning: unused parameter. */

  return DMD_ERROR_NOT_SUPPORTED;
}

/**************************************************************************//**
*  @brief
*  Allocate a framebuffer and select it for drawing via the DMD interface.
*
*  @param framebuffer
*  Pointer to void* pointer where the framebuffer pointer will be returned.
*
*  @return
*  Returns DMD_OK if successful, error otherwise.
******************************************************************************/
EMSTATUS DMD_allocateFramebuffer(void **framebuffer)
{
  /* Allocate a framebuffer from the DISPLAY device driver. */
  if (NULL == displayDevice.pPixelMatrixAllocate) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }
  displayDevice.pPixelMatrixAllocate(&displayDevice,
                                     displayDevice.geometry.width,
                                     displayDevice.geometry.height,
                                     &pixelMatrixBuffer);

  if (NULL == pixelMatrixBuffer) {
    return DMD_ERROR_NOT_ENOUGH_MEMORY;
  }

  /* Select and use the newly allocated framebuffer for drawing from now. */
  *framebuffer = pixelMatrixBuffer;

  /* Mark all lines as dirty. */
  memset(dirtyRows, 0xff, sizeof(dirtyRows));

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Deallocate a framebuffer
*
*  @param framebuffer
*  Pointer to the framebuffer to be deallocated.
*
*  @return
*  Returns DMD_OK if successful, error otherwise.
******************************************************************************/
EMSTATUS DMD_freeFramebuffer(void *framebuffer)
{
  /* Free a framebuffer. */
  return displayDevice.pPixelMatrixFree(&displayDevice, framebuffer);
}

/**************************************************************************//**
*  @brief
*  Select the active framebuffer DMD functions will draw in.
*
*  @param framebuffer
*  Pointer to the framebuffer to be selected as active framebuffer.
*
*  @return
*  Returns DMD_OK if successful, error otherwise.
******************************************************************************/
EMSTATUS DMD_selectFramebuffer(void *framebuffer)
{
  if (framebuffer != pixelMatrixBuffer) {
    /* Set the active framebuffer (pixelMatrixBuffer). */
    pixelMatrixBuffer = framebuffer;

    /* Mark all lines as dirty. */
    memset(dirtyRows, 0xff, sizeof(dirtyRows));
  }
  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Copy contents of a framebuffer to another framebuffer.
*
*  @param dst
*  Pointer to the destination framebuffer.
*
*  @param src
*  Pointer to the source framebuffer.
*
*  @return
*  Returns DMD_OK if successful, error otherwise.
******************************************************************************/
EMSTATUS DMD_copyFramebuffer(void *dst, void *src)
{
  unsigned int size =
    displayDevice.geometry.stride * displayDevice.geometry.height;

  /* Copy contents of source framebuffer to destination framebuffer. */
  memcpy(dst, src, size);

  /* If the destination is the active buffer, mark all rows/lines as dirty. */
  if (dst == pixelMatrixBuffer) {
    memset(dirtyRows, 0xff, sizeof(dirtyRows));
  }
  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Update the display device with contents of active framebuffer.
*
*  @details
*  Only the dirty rows/lines are updated on the display device. Dirty rows/lines
*  are those that have been written to since the last display update. When a
*  new active framebuffer is selected, all lines/rows will be marked as dirty.
*
*  @return
*  Returns DMD_OK if successful, error otherwise.
******************************************************************************/
EMSTATUS DMD_updateDisplay(void)
{
  EMSTATUS      status;
  unsigned int  startRow;
  unsigned int  consecutiveDirtyRows;
  uint8_t      *pStartRow;
  int           bytesPerRow  = displayDevice.geometry.stride >> 3;
  uint32_t      dirtyFlags   = dirtyRows[0];
  int           dirtyWordCnt = 1;

  startRow             = 0;
  consecutiveDirtyRows = 0;

  while (startRow + consecutiveDirtyRows < displayDevice.geometry.height) {
    if (dirtyFlags & 0x1) {
      consecutiveDirtyRows++;
    } else {
      if (consecutiveDirtyRows) {
        /* We have reached the end of a series of consecutive dirty rows,
           update display now. */
        pStartRow = (uint8_t*) pixelMatrixBuffer + startRow * bytesPerRow;
        status = displayDevice.pPixelMatrixDraw(&displayDevice,
                                                pStartRow,
                                                0,
                                                displayDevice.geometry.width,
                                                startRow,
                                                consecutiveDirtyRows);
        if (DISPLAY_EMSTATUS_OK != status) {
          return status;
        }

        startRow += consecutiveDirtyRows + 1;
        consecutiveDirtyRows = 0;
      } else {
        startRow++;
      }
    }

    /* Shift down dirtyFlags until
       all dirtyFlags in the current dirty word have been checked,
       then set to next dirty word.  */
    if ( (startRow + consecutiveDirtyRows) & DIRTY_WORD_BITS_LOG2_MASK ) {
      dirtyFlags >>= 1;
    } else {
      dirtyFlags = dirtyRows[dirtyWordCnt++];
    }
  }

  /* Check if there dirty rows at end that have not been written yet. */
  if (consecutiveDirtyRows) {
    pStartRow = (uint8_t*) pixelMatrixBuffer + startRow * bytesPerRow;
    status = displayDevice.pPixelMatrixDraw(&displayDevice,
                                            pStartRow,
                                            0,
                                            displayDevice.geometry.width,
                                            startRow,
                                            consecutiveDirtyRows);
    if (DISPLAY_EMSTATUS_OK != status) {
      return status;
    }
  }

  /* Clear dirty rows flags. */
  memset(dirtyRows, 0x0, sizeof(dirtyRows));

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
 ******************************************************************************/
EMSTATUS DMD_getFrameBuffer(void **framebuffer)
{
  *framebuffer = pixelMatrixBuffer;

  return DMD_OK;
}

/** @endcond */
