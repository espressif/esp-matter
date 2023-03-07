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

#ifndef __BMP_H_
#define __BMP_H_

/* C Standard header files */
#include <stdlib.h>
#include <stdint.h>

/* EM types */
#include "em_types.h"

#include "bmp_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup glib
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup glib_bmp GLIB BMP
 * @brief Bitmap Support
 * @{
 ******************************************************************************/

/** BMP base error code */
#define ECODE_BMP_BASE    0x00000000

/* Error codes */
/** Successful call */
#define BMP_OK    0x00000000
/** End of file has been reached */
#define BMP_END_OF_FILE
/** General IO error */
#define BMP_ERROR_IO                        (ECODE_BMP_BASE | 0x0001)
/** BMP_Header size in bytes is different from BMP_HEADER_SIZE */
#define BMP_ERROR_HEADER_SIZE_MISMATCH      (ECODE_BMP_BASE | 0x0002)
/** Endian mismatch */
#define BMP_ERROR_ENDIAN_MISMATCH           (ECODE_BMP_BASE | 0x0003)
/** BMP file is not supported */
#define BMP_ERROR_FILE_NOT_SUPPORTED        (ECODE_BMP_BASE | 0x0004)
/** BMP "file" is not a BMP file */
#define BMP_ERROR_FILE_INVALID              (ECODE_BMP_BASE | 0x0005)
/** Argument passed to function is invalid */
#define BMP_ERROR_INVALID_ARGUMENT          (ECODE_BMP_BASE | 0x0006)
/** BMP module is not initialized. Call BMP_init() */
#define BMP_ERROR_MODULE_NOT_INITIALIZED    (ECODE_BMP_BASE | 0x0007)
/** Invalid palette size */
#define BMP_ERROR_INVALID_PALETTE_SIZE      (ECODE_BMP_BASE | 0x0008)
/** File not reset. Call BMP_reset() */
#define BMP_ERROR_FILE_NOT_RESET            (ECODE_BMP_BASE | 0x0009)
/** End of bmp file is reached */
#define BMP_ERROR_END_OF_FILE               (ECODE_BMP_BASE | 0x0010)
/** Buffer provided is too small */
#define BMP_ERROR_BUFFER_TOO_SMALL          (ECODE_BMP_BASE | 0x0020)
/** Bmp palette is not read */
#define BMP_ERROR_PALETTE_NOT_READ          (ECODE_BMP_BASE | 0x0030)

/** Palette size in bytes */
#define BMP_PALETTE_8BIT_SIZE               (256 * 4)
/** BMP Header Size in bytes */
#define BMP_HEADER_SIZE                     (54)
/** BMP Local cache limit */
#define BMP_LOCAL_CACHE_LIMIT               (3)

/** Use RLE8 compression */
#define RLE8_COMPRESSION                    (1)
/** Use no compression */
#define NO_COMPRESSION                      (0)

/** BMP Local cache size */
#define BMP_LOCAL_CACHE_SIZE                (BMP_CONFIG_LOCAL_CACHE_SIZE)

/** @brief BMP Module header structure. Must be packed to exact 54 bytes.
 */
#if defined (__GNUC__)
struct __BMP_Header
#else
__packed struct __BMP_Header
#endif
{
  /** Magic identifier: "BM" (for bmp files)*/
  uint16_t magic;
  /** Size of the bmp file in bytes */
  uint32_t fileSize;
  /** Reserved 1 */
  uint16_t reserved1;
  /** Reserved 2 */
  uint16_t reserved2;
  /** Data offset relative to the start of the bmp data */
  uint32_t dataOffset;
  /** Header size in bytes */
  uint32_t headerSize;
  /** Width of bmp */
  uint32_t width;
  /** Height of bmp */
  uint32_t height;
  /** Color planes */
  uint16_t colorPlanes;
  /** Color depth */
  uint16_t bitsPerPixel;
  /** Compression type */
  uint32_t compressionType;
  /** Size of raw BMP data */
  uint32_t imageDataSize;
  /** Horizontal resolution (pixels per meter) */
  uint32_t hPixelsPerMeter;
  /** Vertical resolution (pixels per meter) */
  uint32_t vPixelsPerMeter;
  /** Number of color indices in the color table that are actually used by the bitmap */
  uint32_t colorsUsed;
  /** Number of color indices that are required for displaying the bitmap */
  uint32_t colorsRequired;
#if defined (__GNUC__)
} __attribute__ ((__packed__));
#else
};
#endif

/** @brief BMP Module header structure. */
typedef struct __BMP_Header   BMP_Header;

/** @brief BMP palette structure to hold palette pointer and size
 */
typedef struct __BMP_Palette{
  /** Palette data pointer */
  uint8_t  *data;
  /** Size of palette data */
  uint32_t size;
} BMP_Palette;

/** @brief BMP Data type structure to hold information about the bmp data returned
 */
typedef struct __BMP_DataType{
  /** Color depth of the data returned from function. */
  uint16_t bitsPerPixel;
  /** Compression type */
  uint32_t compressionType;
  /** Byte length of returned data */
  uint32_t size;
  /** Marks whether this data is at the end of the current row. endOfRow == 1, if end of row is reached. endOfRow == 0, if there is still unread data left in the row */
  uint32_t endOfRow;
} BMP_DataType;

/* Module prototypes */
EMSTATUS BMP_init(uint8_t *palette, uint32_t paletteSize, EMSTATUS (*fp)(uint8_t buffer[], uint32_t bufLength, uint32_t bytesToRead));
EMSTATUS BMP_reset(void);
EMSTATUS BMP_readRgbData(uint8_t buffer[], uint32_t bufLength, uint32_t *pixelsRead);
EMSTATUS BMP_readRawData(BMP_DataType *dataType, uint8_t buffer[], uint32_t bufLength);

/* Accessor functions */
int32_t BMP_getWidth(void);
int32_t BMP_getHeight(void);
int16_t BMP_getBitsPerPixel(void);
int32_t BMP_getCompressionType(void);
int32_t BMP_getImageDataSize(void);
int32_t BMP_getDataOffset(void);
int32_t BMP_getFileSize(void);

/** @} (end addtogroup bmp) */
/** @} (end addtogroup glib) */

#ifdef __cplusplus
}
#endif

#endif /* __BMP_H_ */
