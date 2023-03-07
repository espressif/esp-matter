/***************************************************************************//**
 * @file
 * @brief BMP Module
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

/* BMP Header files */
#include "bmp.h"

/* C Standard header files */
#include <stdint.h>

/* EM types */
#include "em_types.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define MODE_RLE     (0)
#define MODE_8BIT    (1)

BMP_Header     bmpHeader;
BMP_Palette    bmpPalette;

static uint8_t localCache[BMP_LOCAL_CACHE_SIZE];

/* Local variables */
static uint32_t moduleInit   = 0;
static uint32_t fileReset    = 0;
static uint32_t paletteRead  = 0;
static uint32_t bytesInImage = 0;
static uint32_t dataIdx      = 0;

/** RLE Info structure. */
typedef struct __BMP_RleInfo{
  /** Rle Mode (Can be either MODE_RLE or MODE_8BIT ) */
  uint32_t mode;
  /** Holds whether padding occurs at the end of the unencoded run */
  uint32_t isPadding;
  /** Holds how many pixels remaining */
  uint8_t  pixelsRemaining;
  /** If rleMode == MODE_RLE then this is used if pixelsRemaining > 0 */
  uint8_t  pixelIdx;
} RleInfo;

RleInfo rleInfo;

/* Local function pointer */
EMSTATUS (*fpReadData)(uint8_t buffer[], uint32_t bufLength, uint32_t bytesToRead);

/** @endcond */

/* Local function declarations */
static EMSTATUS BMP_readRawData8bit(BMP_DataType *dataType, uint8_t buffer[], uint32_t bufLength);
static EMSTATUS BMP_readRawData24bit(BMP_DataType *dataType, uint8_t buffer[], uint32_t bufLength);
static EMSTATUS BMP_readRawDataRLE8(BMP_DataType *dataType, uint8_t buffer[], uint32_t bufLength);
static EMSTATUS BMP_readPaddingBytes(uint8_t paddingBytes);
static EMSTATUS BMP_readRleData(BMP_DataType *dataType, uint8_t buffer[], uint32_t bufLength);
static EMSTATUS BMP_readRgbDataRLE8(uint8_t buffer[], uint32_t bufLength, uint32_t *pixelsRead);

/**************************************************************************//**
*  @brief
*  Initializes BMP Module.
*
*  Support:
*   - 24-bit Uncompressed.
*   - 8-bit Uncompressed.
*   - 8-bit RLE compressed.
*
*  @param palette
*  Data buffer to hold palette. Required for 8bpp BMPs.
*
*  @param paletteSize
*  Size of palette in bytes. If BMP is 8-bit, this potentially has to be 256 * 4 bytes.
*  If the user knows that he only loads BMPs with N bytes, he can pass in paletteSize = N.
*  Otherwise this value should be 256 * 4 = 1024 bytes to ensure that the palette is
*  big enough for all 8-bits BMPs.
*
*  @param fp
*  Function pointer that is used to read in bytes from BMP file. The function has to
*  return an EMSTATUS and have the following parameter list (uint32_t buffer[],
*  uint32_t bufLength, uint32_t bytesToRead). The function should fill (buffer)
*  with (bytesToRead) bytes from the beginning. If it succeds it has to return BMP_OK, otherwise
*  it should return BMP_ERROR_IO. When the function returns it should be ready to read
*  from where it left off.
*
*  @return
*  Returns BMP_OK on success, or else error code.
******************************************************************************/
EMSTATUS BMP_init(uint8_t *palette, uint32_t paletteSize, EMSTATUS (*fp)(uint8_t buffer[], uint32_t bufLength, uint32_t bytesToRead))
{
  /* Check if compiler has aligned structure members on equal boundarys */
  if (sizeof(bmpHeader) != BMP_HEADER_SIZE) {
    return BMP_ERROR_HEADER_SIZE_MISMATCH;
  }

  /* Check buffer size */
  if (BMP_LOCAL_CACHE_SIZE < BMP_LOCAL_CACHE_LIMIT) {
    return BMP_ERROR_BUFFER_TOO_SMALL;
  }

  bmpPalette.data = palette;
  bmpPalette.size = paletteSize;

  /* Set function pointer */
  fpReadData = fp;

  fileReset   = 0;
  paletteRead = 0;
  dataIdx     = 0;

  moduleInit = 1;

  return BMP_OK;
}

/**************************************************************************//**
*  @brief
*  Makes the module ready for new bmp file. Reads in header from file, and checks
*  if the provided bmp file is valid and supported. It reads in palette if
*  BMP file is 8bpp. Uses function pointer set in BMP_init().
*
*  @return
*  Returns BMP_OK on success, or else error code
******************************************************************************/
EMSTATUS BMP_reset()
{
  /* Check if module is initialized */
  if (moduleInit == 0) {
    return BMP_ERROR_MODULE_NOT_INITIALIZED;
  }

  EMSTATUS status;

  /* Read in header */
  status = fpReadData((uint8_t *) &bmpHeader, BMP_HEADER_SIZE, BMP_HEADER_SIZE);
  if (status != BMP_OK) {
    return status;
  }

  /* Do all the neccesary checks */
  /* Check for little-endian */
  if (bmpHeader.magic != 0x4D42) {
    if (bmpHeader.magic == 0x424D) {
      return BMP_ERROR_ENDIAN_MISMATCH;
    } else {
      return BMP_ERROR_FILE_INVALID;
    }
  }

  /* Check if header size is correct. The header size is used to indicate the version of BMP */
  if (bmpHeader.headerSize != 40) {
    return BMP_ERROR_FILE_NOT_SUPPORTED;
  }

  /* Check if file is supported */
  if (bmpHeader.bitsPerPixel != 24 && bmpHeader.bitsPerPixel != 8) {
    return BMP_ERROR_FILE_NOT_SUPPORTED;
  }

  /* Check if compression is supported */
  if (bmpHeader.compressionType > 1) {
    return BMP_ERROR_FILE_NOT_SUPPORTED;
  }

  /* Do a fix if imageDataSize is broken. It happens to some BMP pictures in some editors */
  if (bmpHeader.imageDataSize == 0) {
    bmpHeader.imageDataSize = bmpHeader.fileSize - bmpHeader.dataOffset;
  }

  bytesInImage = bmpHeader.imageDataSize;
  /* Set a byte limit to the number of bytes in images. This is required because some bmp editors store bmps differently */
  if (bmpHeader.compressionType == 0) {
    bytesInImage = (bmpHeader.imageDataSize / bmpHeader.height) * bmpHeader.height;
  }

  /* Check if palette is necessary */
  if (bmpHeader.bitsPerPixel == 8) {
    /* Check if BMP_Palette is big enough */
    uint32_t pSize = bmpHeader.dataOffset - BMP_HEADER_SIZE;
    if (pSize > bmpPalette.size) {
      return BMP_ERROR_INVALID_PALETTE_SIZE;
    }

    if (bmpPalette.data == NULL) {
      return BMP_ERROR_PALETTE_NOT_READ;
    }

    /* Read in palette */
    status = fpReadData(bmpPalette.data, bmpPalette.size, pSize);
    if (status != BMP_OK) {
      return status;
    }

    uint8_t  swap;
    uint32_t i;
    /* Convert BGR values to RGB values */
    for (i = 0; i < pSize; i += 4) {
      swap = bmpPalette.data[i];

      bmpPalette.data[i] = bmpPalette.data[i + 2];

      bmpPalette.data[i + 2] = swap;
    }

    paletteRead = 1;
  }

  /* Reset static variables */
  fileReset               = 1;
  dataIdx                 = 0;
  rleInfo.mode            = MODE_RLE;
  rleInfo.isPadding       = 0;
  rleInfo.pixelsRemaining = 0;
  rleInfo.pixelIdx        = 0;

  return BMP_OK;
}

/**************************************************************************//**
*  @brief
*  Reads in data from BMP file and fills buffer with RGB values.
*  This function terminates either when the buffer is full, end of row is reached
*  or end of file is reached.
*
*  @param buffer
*  Buffer to hold RGB values.
*  @param bufLength
*  Buffer length in bytes.
*  @param pixelsRead
*  Pointer to a uint32_t which holds how many bytes that are read.
*
*  @return
*  - Returns BMP_OK on success
*  - Returns BMP_ERROR_END_OF_FILE if end of file is reached
*  - Returns error code otherwise.
******************************************************************************/
EMSTATUS BMP_readRgbData(uint8_t buffer[], uint32_t bufLength, uint32_t *pixelsRead)
{
  *pixelsRead = 0;

  /* Check if module is initialized */
  if (moduleInit == 0) {
    return BMP_ERROR_MODULE_NOT_INITIALIZED;
  }

  /* Check file is reset */
  if (fileReset == 0) {
    return BMP_ERROR_FILE_NOT_RESET;
  }

  /* Check if end of file is reached */
  if (dataIdx >= bytesInImage) {
    return BMP_ERROR_END_OF_FILE;
  }

  /* Check if buffer is big enough to hold at least one pixel (3 bytes) */
  if (bufLength < 3) {
    return BMP_ERROR_BUFFER_TOO_SMALL;
  }

  EMSTATUS status = BMP_OK;

  /* Calculate how many bytes to read */
  uint32_t     bytesLeftInBuffer = bufLength - (bufLength % 3);
  uint32_t     bytesToRead;
  uint32_t     bytesPerRow = bmpHeader.imageDataSize / bmpHeader.height;
  uint32_t     bufferIdx   = 0;
  uint32_t     i           = 0;

  BMP_DataType dataType;

  /* Check color depth of BMP */
  if (bmpHeader.bitsPerPixel == 8) {
    /* Check if palette is read */
    if (paletteRead == 0) {
      return BMP_ERROR_PALETTE_NOT_READ;
    }

    /* Check for compression */
    if (bmpHeader.compressionType == RLE8_COMPRESSION) {
      /* Read 8-bit RLE */
      status = BMP_readRgbDataRLE8(buffer, bufLength, pixelsRead);
      if (status != BMP_OK) {
        return status;
      }
    } else if (bmpHeader.compressionType == NO_COMPRESSION) {
      /* Reads 8-bit data */
      dataType.endOfRow = 0;

      while (bytesLeftInBuffer > 0 && dataType.endOfRow == 0) {
        /* Calculate how many bytes to read */
        bytesToRead = BMP_LOCAL_CACHE_SIZE;

        /* If bytesLeftInBuffer is not large enough, reduce bytesToRead */
        if (bytesLeftInBuffer < bytesToRead * 3) {
          bytesToRead = bytesLeftInBuffer / 3;
        }

        /* Read in palette indicies */
        status = BMP_readRawData8bit(&dataType, localCache, bytesToRead);
        if (status != BMP_OK) {
          return status;
        }

        /* Decode the indicies to RGB values */
        for (i = 0; i < dataType.size; ++i) {
          /* Set red */
          buffer[bufferIdx] = bmpPalette.data[4 * localCache[i]];
          /* Set green */
          buffer[bufferIdx + 1] = bmpPalette.data[4 * localCache[i] + 1];
          /* Set blue */
          buffer[bufferIdx + 2] = bmpPalette.data[4 * localCache[i] + 2];

          bufferIdx += 3;
        }

        *pixelsRead       += dataType.size;
        bytesLeftInBuffer -= dataType.size * 3;
      }

      /* Check if padding bytes needs to be read */
      if (dataType.endOfRow == 1) {
        uint8_t paddingBytes = bytesPerRow - bmpHeader.width;

        status = BMP_readPaddingBytes(paddingBytes);
        if (status != BMP_OK) {
          return BMP_OK;
        }
      }
    }
  } else if (bmpHeader.bitsPerPixel == 24) {
    /* Reads 24-bit data */
    status      = BMP_readRawData(&dataType, buffer, bufLength);
    *pixelsRead = dataType.size / 3;
  }

  return status;
}

/**************************************************************************//**
*  @brief
*  Help function used by BMP_readRgbData to read in RLE8 data.
*  This function terminates either when the buffer is full or end of row is reached.
*
*  @param buffer
*  Buffer to hold RGB values
*  @param bufLength
*  Buffer length in bytes
*  @param pixelsRead
*  Pointer to a uint32_t which holds how many bytes that are read
*
*  @return
*  Returns BMP_OK on success, or else error code
******************************************************************************/
static EMSTATUS BMP_readRgbDataRLE8(uint8_t buffer[], uint32_t bufLength, uint32_t *pixelsRead)
{
  EMSTATUS     status;
  BMP_DataType dataType;
  dataType.endOfRow = 0;
  uint32_t     bufferIdx         = 0;
  uint32_t     bytesLeftInBuffer = bufLength - (bufLength % 3);
  uint32_t     bytesToRead;
  uint32_t     i;

  while (bytesLeftInBuffer > 0 && dataType.endOfRow == 0) {
    if (rleInfo.mode == MODE_RLE) {
      /* RLE mode */

      /* Check if any RLE pixels are left to read and convert remaining pixels */
      while (rleInfo.pixelsRemaining > 0 && bytesLeftInBuffer > 0) {
        /* Set red */
        buffer[bufferIdx] = bmpPalette.data[4 * rleInfo.pixelIdx];
        /* Set green */
        buffer[bufferIdx + 1] = bmpPalette.data[4 * rleInfo.pixelIdx + 1];
        /* Set blue */
        buffer[bufferIdx + 2] = bmpPalette.data[4 * rleInfo.pixelIdx + 2];

        rleInfo.pixelsRemaining -= 1;
        bytesLeftInBuffer       -= 3;
        bufferIdx               += 3;
        *pixelsRead             += 1;
      }

      /* Check if all RLE pixels has been decoded */
      if (rleInfo.pixelsRemaining == 0) {
        /* Read in 2 RLE bytes */
        status = BMP_readRleData(&dataType, localCache, BMP_LOCAL_CACHE_SIZE);
        if (status != BMP_OK) {
          return status;
        }

        /* Check if localCache contains RLE info */
        if (localCache[0] > 0) {
          /* Store the RLE info in buffer */
          rleInfo.pixelsRemaining = localCache[0];
          rleInfo.pixelIdx        = localCache[1];
        }
      }
    }

    if (rleInfo.mode == MODE_8BIT) {
      /* 8Bit mode */

      /* Calculate how many bytes to read */
      bytesToRead = rleInfo.pixelsRemaining;

      if (bytesToRead * 3 > bytesLeftInBuffer) {
        bytesToRead = bytesLeftInBuffer / 3;
      }

      /* Check if bytesToRead fit in localCache */
      if (bytesToRead > BMP_LOCAL_CACHE_SIZE) {
        bytesToRead = BMP_LOCAL_CACHE_SIZE;
      }

      /* Read in bytesToRead */
      status = fpReadData(localCache, BMP_LOCAL_CACHE_SIZE, bytesToRead);
      if (status != BMP_OK) {
        return status;
      }

      dataIdx += bytesToRead;

      /* Convert 8-bit bytes to RGB values */
      for (i = 0; i < bytesToRead; ++i) {
        /* Set red */
        buffer[bufferIdx] = bmpPalette.data[4 * localCache[i]];
        /* Set green */
        buffer[bufferIdx + 1] = bmpPalette.data[4 * localCache[i] + 1];
        /* Set blue */
        buffer[bufferIdx + 2] = bmpPalette.data[4 * localCache[i] + 2];

        bufferIdx         += 3;
        bytesLeftInBuffer -= 3;
      }

      *pixelsRead             += bytesToRead;
      rleInfo.pixelsRemaining -= bytesToRead;

      if (rleInfo.pixelsRemaining == 0) {
        /* Switch to RLE mode */
        rleInfo.mode = MODE_RLE;

        /* Read in padding if necessary */
        if (rleInfo.isPadding == 1) {
          status = fpReadData(localCache, BMP_LOCAL_CACHE_SIZE, 1);
          if (status != BMP_OK) {
            return status;
          }

          dataIdx += 1;
        }

        /* Read in the next 2 bytes to see if end of row is reached or end of file */
        status = BMP_readRleData(&dataType, localCache, BMP_LOCAL_CACHE_SIZE);
        if (status != BMP_OK) {
          return status;
        }

        /* Check if the next 2 bytes is RLE info */
        if (localCache[0] > 0) {
          /* Store rle bytes */
          rleInfo.pixelsRemaining = localCache[0];
          rleInfo.pixelIdx        = localCache[1];
        }
      }
    }
  }

  return BMP_OK;
}

/**************************************************************************//**
*  @brief
*  Help function to read 8-bit BMP data
*
*  @param dataType
*  Data type struct which holds information about the data returned
*  @param buffer
*  Buffer to be filled with raw data
*  @param bufLength
*  Length of buffer
*
*  @return
*  Returns BMP_OK on success
******************************************************************************/
static EMSTATUS BMP_readRawData8bit(BMP_DataType *dataType, uint8_t buffer[], uint32_t bufLength)
{
  /* Check if end of file is reached */
  if (dataIdx >= bytesInImage) {
    return BMP_ERROR_END_OF_FILE;
  }

  EMSTATUS status;

  uint32_t bytesToRead;
  uint32_t bytesPerRow = bmpHeader.imageDataSize / bmpHeader.height;

  dataType->bitsPerPixel    = 8;
  dataType->compressionType = 0;
  dataType->endOfRow        = 0;

  /* Calculate how many bytes to read */
  /* Set bytes to read to end of row */
  bytesToRead        = bmpHeader.width - dataIdx % bytesPerRow;
  dataType->endOfRow = 1;

  /* If bytesLeftInBuffer is not large enough, reduce bytesToRead */
  if (bufLength < bytesToRead) {
    bytesToRead        = bufLength;
    dataType->endOfRow = 0;
  }

  /* Read in bytesToRead */
  status = fpReadData(buffer, bufLength, bytesToRead);
  if (status != BMP_OK) {
    return status;
  }

  dataIdx       += bytesToRead;
  dataType->size = bytesToRead;

  return BMP_OK;
}

/**************************************************************************//**
*  @brief
*  Help function to read 24-bit RGB BMP data
*
*  @param dataType
*  Data type struct which holds information about the data returned
*  @param buffer
*  Buffer to be filled with raw data
*  @param bufLength
*  Length of buffer
*
*  @return
*  Returns BMP_OK on success, or else error code
******************************************************************************/
static EMSTATUS BMP_readRawData24bit(BMP_DataType *dataType, uint8_t buffer[], uint32_t bufLength)
{
  /* Check if end of file is reached */
  if (dataIdx >= bytesInImage) {
    return BMP_ERROR_END_OF_FILE;
  }

  EMSTATUS status;

  uint32_t i;
  uint32_t bytesLeftInBuffer = bufLength - (bufLength % 3);
  uint32_t bytesToRead;
  uint32_t bytesPerRow = bmpHeader.imageDataSize / bmpHeader.height;
  uint8_t  swap;

  dataType->bitsPerPixel    = 24;
  dataType->compressionType = NO_COMPRESSION;

  /* Read 24-bits data */
  /* Set bytesToRead to end of row */
  bytesToRead        = bmpHeader.width * 3 - dataIdx % bytesPerRow;
  dataType->endOfRow = 1;

  if (bytesToRead > bytesLeftInBuffer) {
    bytesToRead        = bytesLeftInBuffer;
    dataType->endOfRow = 0;
  }

  /* Read in bytesToRead */
  status = fpReadData(buffer, bufLength, bytesToRead);
  if (status != BMP_OK) {
    return status;
  }

  dataIdx       += bytesToRead;
  dataType->size = bytesToRead;

  /* Copy BGR values to buffer. Flip BGR to RGB */
  for (i = 0; i < bytesToRead; i += 3) {
    swap = buffer[i];
    /* Set Red */
    buffer[i] = buffer[i + 2];
    /* Set Blue */
    buffer[i + 2] = swap;
  }

  bytesLeftInBuffer -= bytesToRead;

  return BMP_OK;
}

/**************************************************************************//**
*  @brief
*  Help function to read 8-bit RLE BMP data
*
*  @param dataType
*  Data type struct which holds information about the data returned
*  @param buffer
*  Buffer to be filled with raw data
*  @param bufLength
*  Length of buffer
*
*  @return
*  Returns BMP_OK on success, or else error code
******************************************************************************/
static EMSTATUS BMP_readRawDataRLE8(BMP_DataType *dataType, uint8_t buffer[], uint32_t bufLength)
{
  EMSTATUS status;

  uint32_t bufferIdx = 0;

  dataType->bitsPerPixel = 8;

  if (rleInfo.mode == MODE_RLE) {
    /* RLE mode */

    dataType->compressionType = RLE8_COMPRESSION; /* RLE compressionType */

    /* Reduce bufLength to an even number if necessary */
    if (bufLength % 2 == 1) {
      bufLength--;
    }

    /* Check if any RLE pixels are left to read */
    if (rleInfo.pixelsRemaining > 0) {
      buffer[bufferIdx]     = rleInfo.pixelsRemaining;
      buffer[bufferIdx + 1] = rleInfo.pixelIdx;

      bufferIdx      += 2;
      dataType->size += 2;
    }

    while (rleInfo.mode == MODE_RLE && bufferIdx < bufLength && dataType->endOfRow == 0) {
      /* Read in 2 RLE bytes */
      status = BMP_readRleData(dataType, localCache, BMP_LOCAL_CACHE_SIZE);
      if (status != BMP_OK) {
        return status;
      }

      /* Check if localCache contains RLE info */
      if (localCache[0] > 0) {
        /* Store the RLE info in buffer */
        buffer[bufferIdx]     = localCache[0];
        buffer[bufferIdx + 1] = localCache[1];

        bufferIdx      += 2;
        dataType->size += 2;
      }
    }
  }

  if (rleInfo.mode == MODE_8BIT && bufferIdx == 0) {
    /* 8Bit mode */
    dataType->compressionType = NO_COMPRESSION;
    /* Calculate how many bytes to read */
    uint32_t bytesToRead = rleInfo.pixelsRemaining;

    if (bytesToRead > bufLength) {
      bytesToRead = bufLength;
    }

    /* Read in bytesToRead */
    status = fpReadData(buffer, bufLength, bytesToRead);
    if (status != BMP_OK) {
      return status;
    }

    /* Update variables */
    rleInfo.pixelsRemaining -= bytesToRead;
    dataIdx                 += bytesToRead;
    dataType->size           = bytesToRead;

    if (rleInfo.pixelsRemaining == 0) {
      /* Switch to RLE mode */
      rleInfo.mode = MODE_RLE;

      /* Read in padding if necessary */
      if (rleInfo.isPadding == 1) {
        status = fpReadData(localCache, BMP_LOCAL_CACHE_SIZE, 1);
        if (status != BMP_OK) {
          return status;
        }

        dataIdx += 1;
      }
    }
  }

  if (rleInfo.mode == MODE_RLE && rleInfo.pixelsRemaining == 0 && dataType->endOfRow == 0) {
    /* Read in the next 2 bytes to see if end of row is reached or end of file */
    status = BMP_readRleData(dataType, localCache, BMP_LOCAL_CACHE_SIZE);
    if (status != BMP_OK) {
      return status;
    }

    /* Check if the next 2 bytes is RLE info */
    if (localCache[0] > 0) {
      /* Store rle bytes */
      rleInfo.pixelsRemaining = localCache[0];
      rleInfo.pixelIdx        = localCache[1];
    }
  }

  return BMP_OK;
}

/**************************************************************************//**
*  @brief
*  Help function to read in padding bytes
*
*  @param paddingBytes
*  Number of paddingBytes at the end of the row
*
*  @return
*  Returns BMP_OK on success, or else error code
******************************************************************************/
static EMSTATUS BMP_readPaddingBytes(uint8_t paddingBytes)
{
  if (paddingBytes > 0) {
    /* Read in padding bytes from bmp file */
    EMSTATUS status = fpReadData(localCache, BMP_LOCAL_CACHE_SIZE, paddingBytes);
    if (status != BMP_OK) {
      return status;
    }

    dataIdx += paddingBytes;
  }

  return BMP_OK;
}

/**************************************************************************//**
*  @brief
*  Help function to read in RLE info. Use this only if you know that RLE info is coming.
*  This function takes the markers and decode them. If no marker is present, buffer is filled
*  with 2 bytes containing the RLE info. This function always reads in two bytes.
*
*  @param dataType
*  Data type structure which holds endOfRow.
*  @param buffer
*  Buffer to be filled with RLE info.
*  @param bufLength
*  Length of buffer. Has to be at least 2.
*
*  @return
*  Returns BMP_OK on success.
*  Returns BMP_END_OF_FILE if EOF is reached, or else error code
******************************************************************************/
static EMSTATUS BMP_readRleData(BMP_DataType *dataType, uint8_t buffer[], uint32_t bufLength)
{
  EMSTATUS status;
  (void)buffer;          /* Unused parameter */
  (void)bufLength;    /* Unused parameter */

  rleInfo.mode            = MODE_RLE;
  rleInfo.pixelsRemaining = 0;
  rleInfo.pixelIdx        = 0;
  rleInfo.isPadding       = 0;

  /* Read in 2 bytes */
  status = fpReadData(localCache, BMP_LOCAL_CACHE_SIZE, 2);
  if (status != BMP_OK) {
    return status;
  }

  dataIdx += 2;

  if (dataIdx >= bytesInImage) {
    dataType->endOfRow = 1;
    return BMP_ERROR_END_OF_FILE;
  }

  /* 1. Check for marker */
  if (localCache[0] == 0) {
    /* a. End of scan line marker */
    if (localCache[1] == 0) {
      dataType->endOfRow = 1;
    }
    /* b. End of file marker */
    else if (localCache[1] == 1) {
      dataType->endOfRow = 1;
      return BMP_ERROR_END_OF_FILE;
    }
    /* c. Run offset marker */
    else if (localCache[1] == 2) {
      /* Read in coordinates for offset marker */
      /* BMP Module doesnt support this marker, but needs to read it in anyway */
      status = fpReadData(localCache, BMP_LOCAL_CACHE_SIZE, 2);
      if (status != BMP_OK) {
        return status;
      }

      dataIdx += 2;
    }
    /* d. Unencoded run marker */
    else if (localCache[1] > 2) {
      /* Switch to 8bit mode and set pixelsRemaining */
      rleInfo.mode            = MODE_8BIT;
      rleInfo.pixelsRemaining = localCache[1];

      if (rleInfo.pixelsRemaining % 2 == 1) {
        rleInfo.isPadding = 1;
      }
    }
  }

  return BMP_OK;
}

/**************************************************************************//**
*  @brief
*  Fills buffer with raw data from BMP file.
*
*  - If data is 24bit: Buffer is filled with RGB values till its full or end of row is reached.
*  - If data is 8bit: Buffer is filled with palette indicies till its full or end of row is reached.
*  - If data is RLE8: Buffer is filled with number of pixels, followed by palette indicies like this
*  (N is number of pixels, P is palette index) buffer = { N P N P ... }.
*
*  - Data is 24bpp if dataType.bitsPerPixel == 24 and dataType.compressionType == NO_COMPRESSION.
*  - Data is 8bpp if dataType.bitsPerPixel == 8 and dataType.compressionType == NO_COMPRESSION.
*  - Data is RLE8 if dataType.bitsPerPixel == 8 and dataType.compressionType == RLE8_COMPRESSION.
*
*  @param dataType
*  Data type struct which holds information about the data returned
*  @param buffer
*  Buffer to be filled with raw data
*  @param bufLength
*  Length of buffer
*
*  @return
*  Returns BMP_OK on success, or else error code
******************************************************************************/
EMSTATUS BMP_readRawData(BMP_DataType *dataType, uint8_t buffer[], uint32_t bufLength)
{
  EMSTATUS status = BMP_OK;
  uint32_t bytesPerRow = bmpHeader.imageDataSize / bmpHeader.height;
  uint8_t  paddingBytes;

  /* Check if module is initialized */
  if (moduleInit == 0) {
    return BMP_ERROR_MODULE_NOT_INITIALIZED;
  }

  /* Check file is reset */
  if (fileReset == 0) {
    return BMP_ERROR_FILE_NOT_RESET;
  }

  if (dataIdx >= bytesInImage) {
    return BMP_ERROR_END_OF_FILE;
  }

  if (dataType == NULL || buffer == NULL) {
    return BMP_ERROR_INVALID_ARGUMENT;
  }

  dataType->size            = 0;
  dataType->bitsPerPixel    = 0;
  dataType->compressionType = 0;
  dataType->endOfRow        = 0;

  if (bmpHeader.bitsPerPixel == 24) {
    /* Read 24 bit data */
    status = BMP_readRawData24bit(dataType, buffer, bufLength);

    /* Check if end of row is reached */
    if (dataType->endOfRow == 1) {
      /* Read in padding bytes */
      paddingBytes = bytesPerRow - bmpHeader.width * 3;

      status = BMP_readPaddingBytes(paddingBytes);
      if (status != BMP_OK) {
        return status;
      }
    }
  } else if (bmpHeader.bitsPerPixel == 8) {
    /* Check if palette is read */
    if (paletteRead == 0) {
      return BMP_ERROR_PALETTE_NOT_READ;
    }

    /* Check for RLE compression */
    if (bmpHeader.compressionType == NO_COMPRESSION) {
      /* Read in 8 bit data */
      status = BMP_readRawData8bit(dataType, buffer, bufLength);

      /* Check if end of row is reached */
      if (dataType->endOfRow == 1) {
        /* Read in padding bytes */
        paddingBytes = bytesPerRow - bmpHeader.width;

        status = BMP_readPaddingBytes(paddingBytes);
        if (status != BMP_OK) {
          return status;
        }
      }
    } else if (bmpHeader.compressionType == RLE8_COMPRESSION) {
      /* Read in RLE8 data */
      status = BMP_readRawDataRLE8(dataType, buffer, bufLength);
    }
  }

  return status;
}

/**************************************************************************//**
*  @brief
*  Get width of BMP image in pixels
*  @return
*  Returns width of image, or -1 on error
******************************************************************************/
int32_t BMP_getWidth()
{
  /* Check if header is read correctly */
  if (moduleInit == 0 || fileReset == 0) {
    return -1;
  }

  return bmpHeader.width;
}

/**************************************************************************//**
*  @brief
*  Get height of BMP image in pixels
*  @return
*  Returns height, or -1 on error
******************************************************************************/
int32_t BMP_getHeight()
{
  /* Check if header is read correctly */
  if (moduleInit == 0 || fileReset == 0) {
    return -1;
  }

  return bmpHeader.height;
}

/**************************************************************************//**
*  @brief
*  Get color depth (bits per pixel)
*  @return
*  Returns bitsPerPixel, or -1 on error
******************************************************************************/
int16_t BMP_getBitsPerPixel()
{
  /* Check if header is read correctly */
  if (moduleInit == 0 || fileReset == 0) {
    return -1;
  }

  return bmpHeader.bitsPerPixel;
}

/**************************************************************************//**
*  @brief
*  Get compression type
*
*  0 - No compression
*  1 - RLE 8bpp
*  2 - RLE 4bpp
*  @return
*  Returns compressionType, or -1 on error
******************************************************************************/
int32_t BMP_getCompressionType()
{
  /* Check if header is read correctly */
  if (moduleInit == 0 || fileReset == 0) {
    return -1;
  }

  return bmpHeader.compressionType;
}

/**************************************************************************//**
*  @brief
*  Get imageDataSize in bytes
*  @return
*  Returns imageDataSize, or -1 on error
******************************************************************************/
int32_t BMP_getImageDataSize()
{
  /* Check if header is read correctly */
  if (moduleInit == 0 || fileReset == 0) {
    return -1;
  }

  return bmpHeader.imageDataSize;
}

/**************************************************************************//**
*  @brief
*  Get the offset, i.e. starting address, of the byte where the bitmap data can be found.
*  @return
*  Returns dataOffset, or -1 on error
******************************************************************************/
int32_t BMP_getDataOffset()
{
  /* Check if header is read correctly */
  if (moduleInit == 0 || fileReset == 0) {
    return -1;
  }

  return bmpHeader.dataOffset;
}

/**************************************************************************//**
*  @brief
*  Get the fileSize in bytes
*  @return
*  Returns fileSize, or -1 on error
******************************************************************************/
int32_t BMP_getFileSize()
{
  /* Check if header is read correctly */
  if (moduleInit == 0 || fileReset == 0) {
    return -1;
  }

  return bmpHeader.fileSize;
}
