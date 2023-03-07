/***************************************************************************//**
 * @file
 * @brief Display
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __SL_DISPLAY_H__
#define __SL_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "glib.h"
#include "sl_status.h"
#include "sl_component_catalog.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Display font class definition
typedef GLIB_Font_Class sl_display_font_class_t;

/// Display align type definition
typedef GLIB_Align_t sl_display_align_t;

/// Display font type definition
typedef GLIB_Font_t sl_display_font_t;

/// Display rectangle type definition
typedef GLIB_Rectangle_t sl_display_rectangle_t;

/// Display renderer callback definition
typedef void(*sl_renderer_callback_t)(void *);

/// Display renderer callback argument type definition
typedef void * sl_renderer_callback_arg_t;

/// Display renderer queue type definition
typedef struct sl_display_renderer_queue {
  /// Callback
  sl_renderer_callback_t callback;
  /// Arguments
  sl_renderer_callback_arg_t args;
  /// Delay after ms
  uint32_t delay_ms_after;
} sl_display_renderer_queue_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Initialize display.
 * @details Init component resources
 *****************************************************************************/
void sl_display_init(void);

#if defined(SL_CATALOG_KERNEL_PRESENT)
/**************************************************************************//**
 * @brief Set display.
 * @details Add item into renderer queue
 * @param[in] display_renderer Display renderer
 *****************************************************************************/
void sl_display_set(const sl_display_renderer_queue_t *display_renderer);

/**************************************************************************//**
 * @brief Renderer Display.
 * @details Create a renderer queue object and add it to the renderer queue
 * @param[in] callback Callback
 * @param[in] args Arguments
 * @param[in] delay_after_ms  Delay after renderer in ms
 *****************************************************************************/
void sl_display_renderer(sl_renderer_callback_t callback, sl_renderer_callback_arg_t args, uint32_t delay_after_ms);
#endif

/**************************************************************************//**
 * @brief Terminate renderer.
 * @details Terminate renderer thread
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_renderer_terminate(void);

/**************************************************************************//**
 * @brief Invert display content.
 * @details Invert colors after function call
 *****************************************************************************/
void sl_display_invert_content(void);

/**************************************************************************//**
 * @brief Display update.
 * @details Update display content
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_update(void);

/**************************************************************************//**
 * @brief Display wake up.
 * @details Wrapper function
 *****************************************************************************/
void sl_display_wake_up(void);

/**************************************************************************//**
 * @brief Display sleep.
 * @details Wrapper function
 *****************************************************************************/
void sl_display_sleep(void);

/**************************************************************************//**
 * @brief Display clear.
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_clear(void);

/**************************************************************************//**
 * @brief Clear region.
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_clear_region(void);

/**************************************************************************//**
 * @brief Reset clipping area.
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_reset_clipping_area(void);

/**************************************************************************//**
 * @brief Reset clipping region.
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_reset_clipping_region(void);

/**************************************************************************//**
 * @brief Apply clipping region.
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_apply_clipping_region(void);

/**************************************************************************//**
 * @brief Translate 24bpp color.
 * @details Wrapper function
 * @param[in] color Color 24bpp
 * @param[out] red Red value
 * @param[out] green Green value
 * @param[out] blue Blue value
 *****************************************************************************/
void sl_display_color_translate24bpp(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue);

/**************************************************************************//**
 * @brief Get RGB color.
 * @details Wrapper function
 * @param[in] red Read value
 * @param[in] green Green value
 * @param[in] blue Blue value
 * @return uint32_t RGB color
 *****************************************************************************/
uint32_t sl_display_rgb_color(uint8_t red, uint8_t green, uint8_t blue);

/**************************************************************************//**
 * @brief Display a rectangle which contains a point.
 * @details Wrapper function
 * @param[in] pRect Rectangle
 * @param[in] xCenter x coordinate
 * @param[in] yCenter y coordinate
 * @return true Contain
 * @return false Not contain
 *****************************************************************************/
bool sl_display_rect_contains_point(sl_display_rectangle_t *pRect, int32_t xCenter, int32_t yCenter);

/**************************************************************************//**
 * @brief Normalize rectangle.
 * @details Wrapper function
 * @param[in] pRect Rectangle
 *****************************************************************************/
void sl_display_normalize_rect(sl_display_rectangle_t *pRect);

/**************************************************************************//**
 * @brief Set clipping region.
 * @details Wrapper function
 * @param[in] pRect Rectangle
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_set_clipping_region(sl_display_rectangle_t *pRect);

/**************************************************************************//**
 * @brief Draw circle.
 * @details
 * @param[in] x Center x coordinate
 * @param[in] y Center y coordinate
 * @param[in] radius Radius
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_circle(int32_t x, int32_t y, uint32_t radius);

/**************************************************************************//**
 * @brief Draw filled circle.
 * @details Wrapper function
 * @param[in] x Center x coordinate
 * @param[in] y Center y coordinate
 * @param[in] radius Radius
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_circle_filled(int32_t x, int32_t y, uint32_t radius);

/**************************************************************************//**
 * @brief Draw partial circle.
 * @details Wrapper function
 * @param[in] xCenter Center x coordinate
 * @param[in] yCenter Center y coordinate
 * @param[in] radius Radius
 * @param[in] bitMask Bitmask
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_partial_circle(int32_t xCenter, int32_t yCenter,
                                           uint32_t radius, uint8_t bitMask);

/**************************************************************************//**
 * @brief Set font.
 * @details Wrapper function
 * @param[in] pFont
 * @return sl_status_t
 *****************************************************************************/
sl_status_t sl_display_set_font(sl_display_font_t *pFont);

/**************************************************************************//**
 * @brief Draw string.
 * @details Wrapper function
 * @param[in] pString String
 * @param[in] sLength Length of string
 * @param[in] x0 X position
 * @param[in] y0 Y position
 * @param[in] opaque Opaque
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_string(const char* pString, uint32_t sLength,
                                   int32_t x0, int32_t y0, bool opaque);

/**************************************************************************//**
 * @brief Draw string on a line.
 * @details Wrapper function
 * @param pString String
 * @param line Line number
 * @param align Alignment
 * @param xOffset X offset
 * @param yOffset Y offset
 * @param opaque Opaque
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_string_on_line(const char *pString, uint8_t line,
                                           sl_display_align_t align, int32_t xOffset,
                                           int32_t yOffset, bool opaque);

/**************************************************************************//**
 * @brief Draw a char.
 * @details Wrapper function
 * @param myChar Char
 * @param x X position
 * @param y Y position
 * @param opaque Opaque
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_char(char myChar, int32_t x, int32_t y, bool opaque);

/**************************************************************************//**
 * @brief Draw a bitmap.
 * @details Wrapper function
 * @param[in] x X position
 * @param[in] y Y position
 * @param[in] width Width
 * @param[in] height Height
 * @param[in] picData Picture byte buffer
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_bitmap(int32_t x, int32_t y, uint32_t width, uint32_t height, const uint8_t *picData);

/**************************************************************************//**
 * @brief Invert bitmap.
 * @details Wrapper function
 * @param[in] bitmapSize Bitmap size
 * @param[in,out] picData Picture byte buffer
 *****************************************************************************/
void sl_display_invert_bitmap(uint32_t bitmapSize, uint8_t *picData);

/**************************************************************************//**
 * @brief Draw line
 * @details Wrapper function
 * @param[in] x1 X1 coordinate
 * @param[in] y1 Y1 coordinate
 * @param[in] x2 X2 coordinate
 * @param[in] y2 Y2 coordinate
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/**************************************************************************//**
 * @brief Draw a horizontal line.
 * @details Wrapper function
 * @param[in] x1 X1 coordinate
 * @param[in] y1 Y1 coordinate
 * @param[in] x2 X2 coordinate
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_line_h(int32_t x1, int32_t y1, int32_t x2);

/**************************************************************************//**
 * @brief Draw a vertical line.
 * @details Wrapper function
 * @param[in] x1 X1 coordinate
 * @param[in] y1 Y1 coordinate
 * @param[in] y2 Y2 coordinate
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_line_v(int32_t x1, int32_t y1, int32_t y2);

/**************************************************************************//**
 * @brief Draw a rectangle.
 * @details Wrapper function
 * @param[in] pRect Rectangle
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_rect(const sl_display_rectangle_t *pRect);

/**************************************************************************//**
 * @brief Draw filled rectangle
 * @details Wrapper function
 * @param pRect Rectangle
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_rect_filled(const sl_display_rectangle_t *pRect);

/**************************************************************************//**
 * @brief Draw a polygon.
 * @details Wrapper function
 * @param numPoints Number of points
 * @param polyPoints Poly points
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_polygon(uint32_t numPoints, const int32_t *polyPoints);

/**************************************************************************//**
 * @brief Draw a filled polygon.
 * @details Wrapper function
 * @param numPoints Number of points
 * @param polyPoints Ploy points
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_polygon_filled(uint32_t numPoints, const int32_t *polyPoints);

/**************************************************************************//**
 * @brief Draw RGB pixel.
 * @details Wrapper function
 * @param x X coordinate
 * @param y Y coordinate
 * @param red Read value
 * @param green Green Value
 * @param blue Blue Vlaue
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_pixel_rgb(int32_t x, int32_t y, uint8_t red, uint8_t green, uint8_t blue);

/**************************************************************************//**
 * @brief Draw pixel.
 * @details Wrapper function
 * @param x X coordinate
 * @param y Y coordinate
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_display_draw_pixel(int32_t x, int32_t y);

/**************************************************************************//**
 * @brief Draw color pixel.
 * @details Wrapper function
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Color value
 * @return sl_status_t
 *****************************************************************************/
sl_status_t sl_display_draw_pixel_color(int32_t x, int32_t y, uint32_t color);

#ifdef __cplusplus
}
#endif
#endif
