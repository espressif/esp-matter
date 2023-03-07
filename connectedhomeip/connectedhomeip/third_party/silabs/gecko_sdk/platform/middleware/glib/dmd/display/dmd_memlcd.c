/***************************************************************************//**
 * @file
 * @brief Dot matrix display support for memory lcd devices.
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

#include "dmd.h"
#include "sl_memlcd.h"
#include "sl_memlcd_display.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* Definitions for DIRTY word manipulations. */
#define DIRTY_WORD_BITS_LOG2       (5)
#define DIRTY_WORD_BITS_LOG2_MASK  ((1 << DIRTY_WORD_BITS_LOG2) - 1)

/* Definitions for RGB_3BIT mode */
#define RGB_3BIT_BITS_PER_PIXEL  3

/* Pointer to memory lcd to use. */
static const sl_memlcd_t *memlcd = NULL;

/* Dimensions of the display */
static DMD_DisplayGeometry dimensions;

/* The memory lcd display is row based, so we store one "dirty" bit for each
 * row. When a row is touched we set the "dirty" bit for that row, marking it
 * for rendering. */
static uint32_t dirtyRows[(SL_MEMLCD_DISPLAY_HEIGHT  + (sizeof(uint32_t) * 8 - 1)) / sizeof(uint32_t) / 8];

/* This framebuffer is large enough to store one full frame. */
static uint8_t framebuffer[(SL_MEMLCD_DISPLAY_WIDTH * SL_MEMLCD_DISPLAY_HEIGHT * SL_MEMLCD_DISPLAY_BPP) / 8];

static void setLineDirty(int line);

EMSTATUS DMD_init(DMD_InitConfig *initConfig)
{
//  EMSTATUS status;
  sl_status_t status;
  (void) initConfig;  /* Suppress compiler warning. */

  if (memlcd != NULL) {
    return DMD_OK;
  }

  /* Initialize the memory lcd. */
  status = sl_memlcd_init();
  if (status != SL_STATUS_OK) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  /* Retrieve the memory lcd. */
  memlcd = sl_memlcd_get();
  if (memlcd == NULL) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  /* Set up dimensions of the display */
  dimensions.xSize = memlcd->width;
  dimensions.ySize = memlcd->height;

  /* At initialization, the clip is the entire display */
  dimensions.xClipStart = 0;
  dimensions.yClipStart = 0;
  dimensions.clipWidth  = dimensions.xSize;
  dimensions.clipHeight = dimensions.ySize;

  /* Fill the entire display with black color */
  DMD_writeColor(0, 0, 0x00, 0x00, 0x00, dimensions.xSize * dimensions.ySize);

  return DMD_OK;
}

EMSTATUS DMD_getDisplayGeometry(DMD_DisplayGeometry **geometry)
{
  if (memlcd == NULL) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }
  *geometry = &dimensions;

  return DMD_OK;
}

EMSTATUS DMD_setClippingArea(uint16_t xStart, uint16_t yStart,
                             uint16_t width, uint16_t height)
{
  if (memlcd == NULL) {
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

EMSTATUS DMD_writeData(uint16_t x, uint16_t y, const uint8_t data[],
                       uint32_t numPixels)
{
  uint32_t clipRemaining;

  if (memlcd == NULL) {
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
  unsigned int rowPixels;
  uint8_t      pixelData = 0;
  int          pixelBit = 0;
  uint8_t      matrixByte;
  uint8_t     *pDst;
  int          bytesPerRow = (SL_MEMLCD_DISPLAY_WIDTH * SL_MEMLCD_DISPLAY_BPP) / 8;
#if (SL_MEMLCD_DISPLAY_RGB_3BIT)
  int          pixelSrcByte = 0;
  int          pixelSrcBit  = 0;
#endif
  uint16_t     currentY;
  uint16_t     maxY;

  /* Adjust y to account for clipping. */
  maxY = dimensions.yClipStart + dimensions.clipHeight;
  currentY = dimensions.yClipStart + y;

  /* Write pixel data to the framebuffer. */
  while (numPixels) {
    if (currentY >= maxY) {
      return DMD_ERROR_PIXEL_OUT_OF_BOUNDS;
    }

    /* Determine how many bits to write on the current row/line. */
    rowPixels =  numPixels > (unsigned int)(dimensions.clipWidth - x)
                ? (unsigned int)(dimensions.clipWidth - x) : numPixels;
    numPixels -= rowPixels;

    pDst = framebuffer + currentY * bytesPerRow;

    /* Adjust x to account for clipping. */
    x += dimensions.xClipStart;

#if (SL_MEMLCD_DISPLAY_RGB_3BIT) /* RGB Display */
    uint32_t *dataWord;
    int       pixelByte;

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

#else /* Monochrome display */

    /* If the start pixel (x) or the corresponding data bit
       (pixelBit) are not aligned on a 8-bit boundary or there are
       less than 8 bits to copy to the current row we copy pixel by
       pixel. */
    if ((0 != (x & 0x7))
        || (0 != (pixelBit & 0x7))
        || (rowPixels < 8)) {
      rowPixels += x;
      for (; x < rowPixels; x++, pixelBit++) {
        pixelData = (data[pixelBit >> 3] >> (pixelBit & 0x7)) & 0x1;
        /* Write pixel data to the pixelMatrix buffer. */
        if (pixelData) {
          pDst[x >> 3] |= 1 << (x & 0x7);
        } else {
          pDst[x >> 3] &= ~(1 << (x & 0x7));
        }
      }
    } else {
      /* The start pixel and it's corresponding data bit are aligned on
         an 8-bit boundary and there are more than 8 bits to copy. Use
         memcpy to copy pixel bits to the current row. Take special care
         of potential remaining bits in the last byte on the row. */
      pDst += x >> 3;

      int numBytesToCopy = rowPixels >> 3;

      if (numBytesToCopy) {
        /* We can copy data continuosly from start to end. */
        memcpy(pDst, &data[pixelBit >> 3], numBytesToCopy);
        pixelBit  += numBytesToCopy << 3;
        rowPixels -= numBytesToCopy << 3;
        pDst      += numBytesToCopy;
      }

      /* Copy any remaining bits to the framebuffer. */
      if (rowPixels) {
        uint8_t pixelMask = (1 << rowPixels) - 1;
        matrixByte = (*pDst & ~pixelMask)
                     | (data[pixelBit >> 3] & pixelMask);
        *pDst = matrixByte;
        pixelBit += rowPixels;
      }
    }
#endif

    /* Mark row/line as dirty */
    setLineDirty(currentY);

    /* Update variables for next row. */
    currentY++;
    x = 0;
  }

  return DMD_OK;
}

/***************************************************************************//**
 *  @brief
 *    This function is not supported for memory lcd displays
 *
 *  @return
 *    DMD_ERROR_NOT_SUPPORTED
 ******************************************************************************/
EMSTATUS DMD_readData(uint16_t x, uint16_t y, uint8_t data[], uint32_t numPixels)
{
  (void) x;          /* Suppress compiler warning: unused parameter. */
  (void) y;          /* Suppress compiler warning: unused parameter. */
  (void) data;       /* Suppress compiler warning: unused parameter. */
  (void) numPixels;  /* Suppress compiler warning: unused parameter. */

  return DMD_ERROR_NOT_SUPPORTED;
}

EMSTATUS DMD_writeColor(uint16_t x, uint16_t y, uint8_t red,
                        uint8_t green, uint8_t blue, uint32_t numPixels)
{
  (void) red;     /* Suppress compiler warning: unused parameter. */
  (void) green;   /* Suppress compiler warning: unused parameter. */
  (void) blue;    /* Suppress compiler warning: unused parameter. */

  if (memlcd == NULL) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  unsigned int rowPixels;
  uint8_t      matrixByte;
  uint8_t     *pDst;
  int          bytesPerRow = (SL_MEMLCD_DISPLAY_WIDTH * SL_MEMLCD_DISPLAY_BPP) / 8;
  uint8_t      pixelData;
  uint16_t     currentY;
  uint16_t     maxY;

  /* Adjust y to account for clipping. */
  maxY = dimensions.yClipStart + dimensions.clipHeight;
  currentY = dimensions.yClipStart + y;

  /* Write one row at a time until there are no more pixels to be written */
  while (numPixels) {
    if (currentY >= maxY) {
      return DMD_ERROR_PIXEL_OUT_OF_BOUNDS;
    }

    /* Determine how many pixels to write on the current row */
    rowPixels = numPixels > (unsigned int)(dimensions.clipWidth - x)
                ? (unsigned int)(dimensions.clipWidth - x) : numPixels;
    numPixels -= rowPixels;

    /* Adjust x to account for clipping. */
    x += dimensions.xClipStart;

    pDst = framebuffer + currentY * bytesPerRow;

#if (SL_MEMLCD_DISPLAY_RGB_3BIT) /* RGB display */

    pixelData = ((red & 0x80) >> 7)
                | ((green & 0x80) >> 6)
                | ((blue & 0x80) >> 5);

    /* Calculate what byte the first pixel is in */
    int pixelByte = (x * RGB_3BIT_BITS_PER_PIXEL) / 8;

    /* Calculate the which bit to start writing pixel data to */
    int pixelBit = (x * RGB_3BIT_BITS_PER_PIXEL) % 8;

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
#else /* Monochrome display */
    pixelData = green ? 0xFF : 0x00;

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
      int byteOffset = x & 0x7;
      uint8_t pixelMask;

      pDst += x >> 3;

      if (byteOffset) {
        /* Copy the pixels into first byte of the pixelMatrix buffer. */
        pixelMask = (1 << byteOffset) - 1;
        matrixByte = (*pDst & pixelMask)
                     | (pixelData & ~pixelMask);
        *pDst = matrixByte;
        pDst++;
        rowPixels -= 8 - byteOffset;
      }

      /* Now, remaining pixels start is 8-bit aligned. Copy the corresponding
         number of bytes, then if there are remaining bits, copy them correctly
         into the last byte. */
      int numBytesToCopy = rowPixels >> 3;

      if (numBytesToCopy) {
        /* We can copy data continuosly from start to end. */
        memset(pDst, pixelData, numBytesToCopy);
        rowPixels  -= numBytesToCopy << 3;
        pDst       += numBytesToCopy;
      }

      /* Copy any remaining bits to the framebuffer. */
      if (rowPixels) {
        pixelMask = (1 << rowPixels) - 1;
        matrixByte = (*pDst & ~pixelMask)
                     | (pixelData & pixelMask);
        *pDst = matrixByte;
      }
    }
#endif

    /* Mark row/line as dirty */
    setLineDirty(currentY);

    /* Update variable for next row/line. */
    x = 0;
    currentY++;
  }

  return DMD_OK;
}

EMSTATUS DMD_sleep(void)
{
  if (memlcd == NULL) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  return sl_memlcd_power_on(memlcd, false);
}

EMSTATUS DMD_wakeUp(void)
{
  if (memlcd == NULL) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  return sl_memlcd_power_on(memlcd, true);
}

EMSTATUS DMD_flipDisplay(int horizontal, int vertical)
{
  (void) horizontal;    /* Suppress compiler warning: unused parameter. */
  (void) vertical;      /* Suppress compiler warning: unused parameter. */

  return DMD_ERROR_NOT_SUPPORTED;
}

/***************************************************************************//**
 *  @brief
 *    Deallocate a framebuffer
 *
 *  @param framebuffer
 *    Pointer to the framebuffer to be deallocated.
 *
 *  @return
 *    Returns DMD_OK if successful, error otherwise.
 ******************************************************************************/
EMSTATUS DMD_freeFramebuffer(void *framebuffer)
{
  (void) framebuffer;
  /* Unsupported operation */
  return DMD_ERROR_NOT_SUPPORTED;
}

EMSTATUS DMD_selectFramebuffer(void *framebuffer)
{
  (void) framebuffer;
  return DMD_ERROR_NOT_SUPPORTED;
}

EMSTATUS DMD_updateDisplay(void)
{
  sl_status_t   status;
  unsigned int  startRow;
  unsigned int  consecutiveDirtyRows;
  uint8_t      *pStartRow;
  int           bytesPerRow  = (SL_MEMLCD_DISPLAY_WIDTH * SL_MEMLCD_DISPLAY_BPP) / 8;
  uint32_t      dirtyFlags   = dirtyRows[0];
  int           dirtyWordCnt = 1;

  startRow             = 0;
  consecutiveDirtyRows = 0;

  while (startRow + consecutiveDirtyRows < memlcd->height) {
    if (dirtyFlags & 0x1) {
      consecutiveDirtyRows++;
    } else {
      if (consecutiveDirtyRows) {
        /* We have reached the end of a series of consecutive dirty rows,
           update display now. */
        pStartRow = (uint8_t*) framebuffer + startRow * bytesPerRow;
        status = sl_memlcd_draw(memlcd, pStartRow, startRow, consecutiveDirtyRows);
        if (status != SL_STATUS_OK) {
          return DMD_ERROR_MEMORY_ERROR;
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
    pStartRow = (uint8_t*) framebuffer + startRow * bytesPerRow;
    status = sl_memlcd_draw(memlcd, pStartRow, startRow, consecutiveDirtyRows);
    if (status != SL_STATUS_OK) {
      return DMD_ERROR_MEMORY_ERROR;
    }
  }

  /* Clear dirty rows flags. */
  memset(dirtyRows, 0x0, sizeof(dirtyRows));

  return DMD_OK;
}

EMSTATUS DMD_getFrameBuffer(void **fb)
{
  *fb = framebuffer;

  return DMD_OK;
}

/***************************************************************************//**
 * @brief
 *   Mark the line as dirty.
 ******************************************************************************/
static void setLineDirty(int line)
{
  dirtyRows[line >> DIRTY_WORD_BITS_LOG2] |= 1 << (line & DIRTY_WORD_BITS_LOG2_MASK);
}

/** @endcond */
