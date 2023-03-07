/***************************************************************************//**
 * @file
 * @brief Silicon Labs Graphics Library
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

#ifndef GLIB_H
#define GLIB_H

/***************************************************************************//**
 * @addtogroup glib GLIB - Graphics Library
 * @brief Silicon Labs Graphics Library
 * @{
 *
 * @brief Graphics Library
 *
 *   Silicon Labs Graphics Library. Feature rich graphics library for memory
 *   constrained applications.
 *
 *  @li @ref glib_intro
 *  @li @ref glib_init
 *  @li @ref glib_color_sect
 *  @li @ref glib_draw_shapes
 *  @li @ref glib_draw_pixel
 *  @li @ref glib_font
 *  @li @ref glib_bitmap
 *  @li @ref glib_example
 *
 * @n @section glib_intro Introduction
 *
 *   GLIB is a graphics library that can be used to draw pixels, common shapes,
 *   text or bitmaps to a display connected to an MCU. The main goal of this library
 *   is easy of use and small code and memory footprint. In this way GLIB can
 *   be used on even the smallest Cortex-M0 devices.
 *
 *   GLIB treats the display as a matrix of pixels which is a model of the
 *   physical display. This matrix is exposed to GLIB via an API called
 *   DMD (Dot Matrix Display). So in order to draw something on a physical
 *   display the user application needs to provide GLIB with a single
 *   implementation of the DMD interface. Sample DMD implementations are
 *   provided for the displays that are connected to the Silion Labs Starter
 *   Kits and the Silicon Labs Development Kits.
 *
 * @n @section glib_init Initialization
 *
 *   There are two things that must be initialized before GLIB can be used to
 *   draw pixels on the display. First the user needs to initialize the DMD
 *   implementation for the actual hardware display which is used. This
 *   initialization is display hardware specific.
 *
 *   After the DMD is initialized the user needs to initialize a @ref GLIB_Context_t
 *   that can be used for all the drawing operations. A @ref GLIB_Context_t
 *   structure is initialized using the @ref GLIB_contextInit() function.
 *
 *   Every GLIB drawing function will require a @ref GLIB_Context_t as one of
 *   the functions arguments. The GBLI context contains important configuration
 *   values that all the drawing functions use. It contains the foreground and
 *   background color, and it also contains information on what font to use
 *   when rendering text on the display.
 *
 * @n @section glib_color_sect Color
 *
 *   GLIB represent color in a pixel as three 8 bit color values representing
 *   red, green and blue, giving the user support for a 24 bit color depth.
 *   The @ref GLIB_rgbColor() function can be used to
 *   convert from distinct red, green and blue values to an 32-bit integer
 *   representing the color of single pixel. Similarly the
 *   @ref GLIB_colorTranslate24bpp() function can be used to convert from
 *   the 32-bit integer representation into the 3 separate red, green and
 *   blue components. The 32-bit integer representation of a pixel color is
 *   used when configuring the foreground and background color.
 *
 *   GLIB also contains some common predefined colors for use in places where
 *   a 32-bit integer is used to represent the color. These predefined colors
 *   include @ref White, @ref Black, @ref Brown, @ref Orange, and many more.
 *   See the @ref glib_color for the full list of colors.
 *
 *   Some displays are monochrome, meaning that they only support 2 colors.
 *   When using GLIB to draw shapes on these displays the applications should
 *   use only the 2 colors @ref White and @ref Black.
 *
 * @n @section glib_draw_shapes Draw Shapes
 *
 *   GLIB contains functions for drawing common shapes to a display. Here is a
 *   list containing all the drawing functions.
 *
 * @li @ref GLIB_drawCircle(). Draw a circle outline with the foreground color.
 * @li @ref GLIB_drawCircleFilled(). Draw a circle filled with the foreground
 * color.
 * @li @ref GLIB_drawPartialCircle(). Draw specific octants of a circle with
 * the foreground color.
 * @li @ref GLIB_drawLine(). Draw line with the foreground color.
 * @li @ref GLIB_drawLineH(). Draw a horizontal line.
 * @li @ref GLIB_drawLineV(). Draw a vertical line.
 * @li @ref GLIB_drawRect(). Draw a rectangle outline with the foreground
 * color.
 * @li @ref GLIB_drawRectFilled(). Draw a filled rectangle using the foreground
 * color.
 * @li @ref GLIB_drawPolygon(). Draw lines between all the points in the given
 * set using the foreground color.
 * @li @ref GLIB_drawPolygonFilled(). Draw filled polygon between points.
 *
 * @n @section glib_draw_pixel Draw Pixels
 *
 * These are functions that GLIB provides for drawing one pixel.
 *
 * @li @ref GLIB_drawPixel(). Draw a pixel using the foreground color.
 * @li @ref GLIB_drawPixelRGB(). Draw a single pixel using a specific color.
 * @li @ref GLIB_drawPixelColor(). Draw a single pixel using a specific color.
 *
 * @n @section glib_font Font rendering
 *
 *   GLIB Supports fixed width font rendering. The functions @ref GLIB_drawString()
 *   and @ref GLIB_drawChar() are used to render text on the display.
 *
 *   GLIB provides these fonts for the user
 *
 *   @li @ref GLIB_FontNormal8x8 A normal 8x8 pixel font which is the defaul font
 *     if nothing else is configured.
 *   @li @ref GLIB_FontNarrow6x8 A narrow 6x8 pixel font which can be used if
 *     the default font is to wide.
 *   @li @ref GLIB_FontNumber16x20 A large font for use with only numbers. This
 *     font is 16x20 pixels.
 *
 *   To change the font the user calls @ref GLIB_setFont() with a pointer
 *   to a @ref GLIB_Font_t structure which describes the whole font. The
 *   @ref GLIB_Font_t structure contains a pointer to the bitmap for each
 *   character and it describes attributes of the font like the width and
 *   height of each character and it also contains the bitmap of each
 *   character.
 *
 * @n @section glib_bitmap Draw Bitmap
 *
 *   To draw an image or custom bitmaps on the display the @ref GLIB_drawBitmap()
 *   function can be used. The color displays typically use a bitmap format
 *   where each pixel is represented by 3 consecutive bytes representing the
 *   red, green and blue color components. While monochrome displays
 *   typically use a bitmap format where 1 byte represents 8 pixels where a 1 bit
 *   is white and a 0 bit is black. Note that bitmaps are insensitive to the
 *   foreground and background color settings, and that the format of the bitmap
 *   depends on the DMD implementation of the display.
 *
 * @n @section glib_example Example
 *
 *   This examples shows how to initialize a GLIB context and draw something
 *   on a display.
 *
 *   @code
 *   static GLIB_Context_t context;
 *   static const char * msg = "Hello GLIB!";
 *
 *   void glibExample(void)
 *   {
 *     // Setup the dot matrix and initialize the GLIB context - these should be called once
 *     DMD_init(0);
 *     GLIB_contextInit(&context);
 *
 *     // Write our message into the GLIB instance.
 *     GLIB_drawString(&context, msg, strlen(msg), 10, 10, true);
 *
 *     // Needs to be called each time there is a change to GLIB to mirror
 *     // GLIB context data model changes to the physical LCD.
 *     DMD_updateDisplay();
 *   }
 *   @endcode
 *
 ******************************************************************************/

/* C-header files */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* GLIB header files */
#include "glib_color.h"

/* Display Driver header files */
#include "dmd/dmd.h"

#include "em_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** GLIB Base error code */
#define ECODE_GLIB_BASE                         0x00000000

/* Error codes */
/** Successful call */
#define GLIB_OK                                 0x00000000
/** Function did not draw */
#define GLIB_ERROR_NOTHING_TO_DRAW              (ECODE_GLIB_BASE | 0x0001)
/** Invalid char */
#define GLIB_ERROR_INVALID_CHAR                 (ECODE_GLIB_BASE | 0x0002)
/** Coordinates out of bounds */
#define GLIB_OUT_OF_BOUNDS                      (ECODE_GLIB_BASE | 0x0003)
/** Invalid coordinates (ex. xMin > xMax) */
#define GLIB_ERROR_INVALID_CLIPPINGREGION       (ECODE_GLIB_BASE | 0x0004)
/** Invalid argument */
#define GLIB_ERROR_INVALID_ARGUMENT             (ECODE_GLIB_BASE | 0x0005)
/** Out of memory */
#define GLIB_ERROR_OUT_OF_MEMORY                (ECODE_GLIB_BASE | 0x0006)
/** File not supported */
#define GLIB_ERROR_FILE_NOT_SUPPORTED           (ECODE_GLIB_BASE | 0x0007)
/** General IO Error */
#define GLIB_ERROR_IO                           (ECODE_GLIB_BASE | 0x0008)
/** Invalid file */
#define GLIB_ERROR_INVALID_FILE                 (ECODE_GLIB_BASE | 0x0009)

/** @brief Font classes
 */
typedef enum __GLIB_Font_Class{
  InvalidFont = 0,  /**< Invalid font. */
  FullFont,         /**< Characters and numbers font. */
  NumbersOnlyFont,  /**< Numbers only font. */
} GLIB_Font_Class;

/** @brief Alignment types
 */
typedef enum __GLIB_Align{
  GLIB_ALIGN_LEFT,
  GLIB_ALIGN_CENTER,
  GLIB_ALIGN_RIGHT,
} GLIB_Align_t;

/** @brief Font definition structure
 */
typedef struct __GLIB_Font_t{
  /** Pointer to the pixel map for the font. */
  void *pFontPixMap;

  /** Number of elements in the font pixel map. */
  uint16_t cntOfMapElements;

  /** Size of each element in the font pixel map. */
  uint8_t sizeOfMapElement;

  /** Font row offset. */
  uint8_t fontRowOffset;

  /** Width in pixels of each character. */
  uint8_t fontWidth;

  /** Height in pixels of each character. */
  uint8_t fontHeight;

  /** Number of pixels between each line in this font. */
  uint8_t lineSpacing;

  /** Number of pixels between each character in this font. */
  uint8_t charSpacing;

  /** The font class is used to tell glib if the font contains numbers only
   *  or characters and numbers */
  GLIB_Font_Class fontClass;
} GLIB_Font_t;

/** @brief Rectangle structure
 */
typedef struct __GLIB_Rectangle_t{
  /** Minimum x-coordinate */
  int32_t xMin;
  /** Minimum y-coordinate */
  int32_t yMin;
  /** Maximum x-coordinate */
  int32_t xMax;
  /** Maximum y-coordinate */
  int32_t yMax;
} GLIB_Rectangle_t;

/** @brief GLIB Drawing Context
 *  (Multiple instances of GLIB_Context_t can exist)
 */
typedef struct __GLIB_Context_t{
  /** Pointer to the dimensions of the display */
  const DMD_DisplayGeometry *pDisplayGeometry;

  /** Background color */
  uint32_t backgroundColor;

  /** Foreground color */
  uint32_t foregroundColor;

  /** Clipping rectangle */
  GLIB_Rectangle_t clippingRegion;

  /** Font definition */
  GLIB_Font_t font;
} GLIB_Context_t;

/* Prototypes for graphics library functions */
EMSTATUS GLIB_contextInit(GLIB_Context_t *pContext);

EMSTATUS GLIB_displayWakeUp(void);

EMSTATUS GLIB_displaySleep(void);

EMSTATUS GLIB_clear(GLIB_Context_t *pContext);

EMSTATUS GLIB_clearRegion(const GLIB_Context_t *pContext);

EMSTATUS GLIB_resetDisplayClippingArea(GLIB_Context_t *pContext);

EMSTATUS GLIB_resetClippingRegion(GLIB_Context_t *pContext);

EMSTATUS GLIB_applyClippingRegion(const GLIB_Context_t *pContext);

void GLIB_colorTranslate24bpp(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue);

uint32_t GLIB_rgbColor(uint8_t red, uint8_t green, uint8_t blue);

bool GLIB_rectContainsPoint(const GLIB_Rectangle_t *pRect, int32_t xCenter, int32_t yCenter);

void GLIB_normalizeRect(GLIB_Rectangle_t *pRect);

EMSTATUS GLIB_setClippingRegion(GLIB_Context_t *pContext, const GLIB_Rectangle_t *pRect);

EMSTATUS GLIB_drawCircle(GLIB_Context_t *pContext, int32_t x, int32_t y,
                         uint32_t radius);

EMSTATUS GLIB_drawCircleFilled(GLIB_Context_t *pContext, int32_t x, int32_t y,
                               uint32_t radius);

EMSTATUS GLIB_drawPartialCircle(GLIB_Context_t *pContext, int32_t xCenter,
                                int32_t yCenter, uint32_t radius, uint8_t bitMask);

EMSTATUS GLIB_setFont(GLIB_Context_t *pContext, GLIB_Font_t *pFont);

EMSTATUS GLIB_drawString(GLIB_Context_t *pContext, const char* pString, uint32_t sLength,
                         int32_t x0, int32_t y0, bool opaque);

EMSTATUS GLIB_drawStringOnLine(GLIB_Context_t *pContext, const char *pString, uint8_t line,
                               GLIB_Align_t align, int32_t xOffset, int32_t yOffset, bool opaque);

EMSTATUS GLIB_drawChar(GLIB_Context_t *pContext, char myChar, int32_t x,
                       int32_t y, bool opaque);

EMSTATUS GLIB_drawBitmap(GLIB_Context_t *pContext, int32_t x, int32_t y,
                         uint32_t width, uint32_t height, const uint8_t *picData);

void GLIB_invertBitmap(GLIB_Context_t *pContext, uint32_t bitmapSize,
                       uint8_t *picData);

EMSTATUS GLIB_drawLine(GLIB_Context_t *pContext, int32_t x1, int32_t y1,
                       int32_t x2, int32_t y2);

EMSTATUS GLIB_drawLineH(GLIB_Context_t *pContext, int32_t x1, int32_t y1,
                        int32_t x2);

EMSTATUS GLIB_drawLineV(GLIB_Context_t *pContext, int32_t x1, int32_t y1,
                        int32_t y2);

EMSTATUS GLIB_drawRect(GLIB_Context_t *pContext, const GLIB_Rectangle_t *pRect);

EMSTATUS GLIB_drawRectFilled(GLIB_Context_t *pContext,
                             const GLIB_Rectangle_t *pRect);

EMSTATUS GLIB_drawPolygon(GLIB_Context_t *pContext,
                          uint32_t numPoints, const int32_t *polyPoints);

EMSTATUS GLIB_drawPolygonFilled(GLIB_Context_t *pContext,
                                uint32_t numPoints, const int32_t *polyPoints);

EMSTATUS GLIB_drawPixelRGB(GLIB_Context_t *pContext, int32_t x, int32_t y,
                           uint8_t red, uint8_t green, uint8_t blue);

EMSTATUS GLIB_drawPixel(GLIB_Context_t *pContext, int32_t x, int32_t y);

EMSTATUS GLIB_drawPixelColor(GLIB_Context_t *pContext, int32_t x, int32_t y,
                             uint32_t color);

/* Fonts included in the library */
extern const GLIB_Font_t GLIB_FontNormal8x8; /* Default */
extern const GLIB_Font_t GLIB_FontNarrow6x8;
extern const GLIB_Font_t GLIB_FontNumber16x20;

/** @} (end addtogroup glib) */

#ifdef __cplusplus
}
#endif

#endif
