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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "sl_component_catalog.h"
#include "em_types.h"
#include "dmd/dmd.h"
#include "sl_display.h"
#include "sl_board_control.h"
#include "sl_wisun_trace_util.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#if defined(SL_CATALOG_KERNEL_PRESENT)
/// Display message queue size
#define DISPLAY_MESSAGE_QUEUE_SIZE      (16U)

/// Display stack size
#define DISPLAY_STACK_SIZE              (160U)

/// Port API macro function
#define __port_api(__api_call, ...)                 \
  do {                                              \
    _display_mutex_acquire();                       \
    __api_call(&_disp.glib_context, ##__VA_ARGS__); \
    _display_mutex_release();                       \
  } while (0)                                       \


/// Port API and return state macro function
#define __port_api_ret(__api_call, ...)                               \
  do {                                                                \
    sl_status_t stat = SL_STATUS_FAIL;                                \
    _display_mutex_acquire();                                         \
    stat = (__api_call(&_disp.glib_context, ##__VA_ARGS__) == DMD_OK) \
           ? SL_STATUS_OK : SL_STATUS_FAIL;                           \
    _display_mutex_release();                                         \
    return stat;                                                      \
  } while (0)
#else

/// Port API macro function
#define __port_api(__api_call, ...)                 \
  do {                                              \
    __api_call(&_disp.glib_context, ##__VA_ARGS__); \
  } while (0)                                       \


/// Port API and return state macro function
#define __port_api_ret(__api_call, ...)                               \
  do {                                                                \
    sl_status_t stat = SL_STATUS_FAIL;                                \
    stat = (__api_call(&_disp.glib_context, ##__VA_ARGS__) == DMD_OK) \
           ? SL_STATUS_OK : SL_STATUS_FAIL;                           \
    return stat;                                                      \
  } while (0)
#endif

/// Update display callback type
typedef EMSTATUS (*update_display_t)(void);

/// Display handler type definition
typedef struct _display_hnd {
  /// State
  uint32_t state;
  /// GLIB context
  GLIB_Context_t glib_context;
  /// Update callback
  update_display_t update;
} _display_hnd_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

#if defined(SL_CATALOG_KERNEL_PRESENT)
/**************************************************************************//**
 * @brief Mutex acquire
 * @details Helper function
 *****************************************************************************/
static inline void _display_mutex_acquire(void);

/**************************************************************************//**
 * @brief Mutex release
 * @details Helper function
 *****************************************************************************/
static inline void _display_mutex_release(void);

/**************************************************************************//**
 * @brief Display renderer task
 * @details Thread function
 * @param args Arguments
 *****************************************************************************/
static void _display_renderer_task(void *args);
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Display message queue control block
static uint64_t _display_msg_queue_cb[osMessageQueueCbSize] = { 0 };

/// Display task control block
static uint64_t _display_task_cb[osThreadCbSize] = { 0 };

/// Display stack allocation
static uint64_t _display_stack[DISPLAY_STACK_SIZE] = { 0 };

/// Display thread id
static osThreadId_t _display_thr_id = NULL;

/// Display renderer message queue
static osMessageQueueId_t _display_msg_queue = NULL;

/// Display renderer message queue buffer allocation
static sl_display_renderer_queue_t _buff[DISPLAY_MESSAGE_QUEUE_SIZE] = { 0 };

/// Display mutex control block
static uint64_t _display_mtx_cb[osMutexCbSize] = { 0 };

/// Display mutex
static osMutexId_t _display_mtx = NULL;

/// Display mutex attribute
static const osMutexAttr_t _display_mtx_attr = {
  .name      = "DisplayMtx",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _display_mtx_cb,
  .cb_size   = sizeof(_display_mtx_cb)
};

/// Display task attribute
static const osThreadAttr_t _display_task_attr = {
  .name        = "DisplayTask",
  .attr_bits   = osThreadDetached,
  .cb_mem      = _display_task_cb,
  .cb_size     = sizeof(_display_task_cb),
  .stack_mem   = _display_stack,
  .stack_size  = sizeof(_display_stack),
  .priority    = osPriorityLow,
  .tz_module   = 0
};

/// Display message queue attributes
static const osMessageQueueAttr_t _display_msg_queue_attr = {
  .name = "DisplayMsgQueue",
  .attr_bits = 0,
  .cb_mem = _display_msg_queue_cb,
  .cb_size = sizeof(_display_msg_queue_cb),
  .mq_mem = _buff,
  .mq_size = sizeof(_buff)
};
#endif

/// Internal display handler
static _display_hnd_t _disp;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/*Display init*/
void sl_display_init(void)
{
  EMSTATUS status;

#if defined(SL_CATALOG_KERNEL_PRESENT)
  osKernelState_t kernel_state = osKernelLocked;

  _display_mtx = osMutexNew(&_display_mtx_attr);
  assert(_display_mtx != NULL);

  kernel_state = osKernelGetState();
  if (kernel_state == osKernelRunning) {
    _display_mutex_acquire();
  }
#endif

  // set display updater callback
  _disp.update = DMD_updateDisplay;

  sl_board_enable_display();

  status = DMD_init(0);
  assert(status == DMD_OK);

  status = GLIB_contextInit(&_disp.glib_context);

  _disp.glib_context.backgroundColor = White;
  _disp.glib_context.foregroundColor = Black;

  /* Use Narrow font */
  GLIB_setFont(&_disp.glib_context, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  GLIB_clear(&_disp.glib_context);

  _disp.update();

#if defined(SL_CATALOG_KERNEL_PRESENT)
  // start renderer thread
  _display_msg_queue = osMessageQueueNew(DISPLAY_MESSAGE_QUEUE_SIZE,
                                         sizeof(sl_display_renderer_queue_t),
                                         &_display_msg_queue_attr);
  assert(_display_msg_queue != NULL);

  _display_thr_id = osThreadNew(_display_renderer_task, NULL, &_display_task_attr);
  assert(_display_thr_id != NULL);

  if (kernel_state == osKernelRunning) {
    _display_mutex_release();
  }
#endif
}

void sl_display_invert_content(void)
{
  uint32_t tmp = 0;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _display_mutex_acquire();
#endif
  tmp = _disp.glib_context.foregroundColor;
  _disp.glib_context.foregroundColor = _disp.glib_context.backgroundColor;
  _disp.glib_context.backgroundColor = tmp;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _display_mutex_release();
#endif
}

#if defined(SL_CATALOG_KERNEL_PRESENT)
void sl_display_set(const sl_display_renderer_queue_t *display_renderer)
{
  osMessageQueuePut(_display_msg_queue, display_renderer, 0U, 0U);
}

sl_status_t sl_display_renderer_terminate(void)
{
  osStatus_t status;
  GLIB_clear(&_disp.glib_context);
  status = osThreadTerminate(_display_thr_id);
  if (status == osOK) {
    return SL_STATUS_OK;
  }
  return SL_STATUS_FAIL;
}

#endif
// PORT GLIB functions
sl_status_t sl_display_update(void)
{
  sl_status_t stat = SL_STATUS_FAIL;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _display_mutex_acquire();
#endif
  stat = _disp.update();
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _display_mutex_release();
#endif
  return stat;
}

void sl_display_wake_up(void)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _display_mutex_acquire();
#endif
  GLIB_displayWakeUp();
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _display_mutex_release();
#endif
}

void sl_display_sleep(void)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _display_mutex_acquire();
#endif
  GLIB_displaySleep();
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _display_mutex_release();
#endif
}

sl_status_t sl_display_clear(void)
{
  __port_api_ret(GLIB_clear);
}

sl_status_t sl_display_clear_region(void)
{
  __port_api_ret(GLIB_clearRegion);
}

sl_status_t sl_display_reset_clipping_area(void)
{
  __port_api_ret(GLIB_resetDisplayClippingArea);
}

sl_status_t sl_display_reset_clipping_region(void)
{
  __port_api_ret(GLIB_resetClippingRegion);
}

sl_status_t sl_display_apply_clipping_region(void)
{
  __port_api_ret(GLIB_applyClippingRegion);
}

void sl_display_color_translate24bpp(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue)
{
  GLIB_colorTranslate24bpp(color, red, green, blue);
}

uint32_t sl_display_rgb_color(uint8_t red, uint8_t green, uint8_t blue)
{
  return GLIB_rgbColor(red, green, blue);
}

bool sl_display_rect_contains_point(sl_display_rectangle_t *pRect, int32_t xCenter, int32_t yCenter)
{
  return GLIB_rectContainsPoint(pRect, xCenter, yCenter);
}

void sl_display_normalize_rect(sl_display_rectangle_t *pRect)
{
  GLIB_normalizeRect(pRect);
}

sl_status_t sl_display_set_clipping_region(sl_display_rectangle_t *pRect)
{
  __port_api_ret(GLIB_setClippingRegion, pRect);
}

sl_status_t sl_display_draw_circle(int32_t x, int32_t y, uint32_t radius)
{
  __port_api_ret(GLIB_drawCircle, x, y, radius);
}

sl_status_t sl_display_draw_circle_filled(int32_t x, int32_t y, uint32_t radius)
{
  __port_api_ret(GLIB_drawCircleFilled, x, y, radius);
}

sl_status_t sl_display_draw_partial_circle(int32_t xCenter, int32_t yCenter,
                                           uint32_t radius, uint8_t bitMask)
{
  __port_api_ret(GLIB_drawPartialCircle, xCenter, yCenter, radius, bitMask);
}

sl_status_t sl_display_set_font(sl_display_font_t *pFont)
{
  __port_api_ret(GLIB_setFont, pFont);
}

sl_status_t sl_display_draw_string(const char* pString, uint32_t sLength,
                                   int32_t x0, int32_t y0, bool opaque)
{
  __port_api_ret(GLIB_drawString, pString, sLength, x0, y0, opaque);
}

sl_status_t sl_display_draw_string_on_line(const char *pString, uint8_t line,
                                           sl_display_align_t align, int32_t xOffset,
                                           int32_t yOffset, bool opaque)
{
  __port_api_ret(GLIB_drawStringOnLine, pString, line, align, xOffset, yOffset, opaque);
}

sl_status_t sl_display_draw_char(char myChar, int32_t x, int32_t y, bool opaque)
{
  __port_api_ret(GLIB_drawChar, myChar, x, y, opaque);
}

sl_status_t sl_display_draw_bitmap(int32_t x, int32_t y, uint32_t width, uint32_t height, const uint8_t *picData)
{
  __port_api_ret(GLIB_drawBitmap, x, y, width, height, picData);
}

void sl_display_invert_bitmap(uint32_t bitmapSize, uint8_t *picData)
{
  __port_api(GLIB_invertBitmap, bitmapSize, picData);
}

sl_status_t sl_display_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
  __port_api_ret(GLIB_drawLine, x1, y1, x2, y2);
}

sl_status_t sl_display_draw_line_h(int32_t x1, int32_t y1, int32_t x2)
{
  __port_api_ret(GLIB_drawLineH, x1, y1, x2);
}

sl_status_t sl_display_draw_line_v(int32_t x1, int32_t y1, int32_t y2)
{
  __port_api_ret(GLIB_drawLineV, x1, y1, y2);
}

sl_status_t sl_display_draw_rect(const sl_display_rectangle_t *pRect)
{
  __port_api_ret(GLIB_drawRect, pRect);
}

sl_status_t sl_display_draw_rect_filled(const sl_display_rectangle_t *pRect)
{
  __port_api_ret(GLIB_drawRectFilled, pRect);
}

sl_status_t sl_display_draw_polygon(uint32_t numPoints, const int32_t *polyPoints)
{
  __port_api_ret(GLIB_drawPolygon, numPoints, polyPoints);
}

sl_status_t sl_display_draw_polygon_filled(uint32_t numPoints, const int32_t *polyPoints)
{
  __port_api_ret(GLIB_drawPolygonFilled, numPoints, polyPoints);
}

sl_status_t sl_display_draw_pixel_rgb(int32_t x, int32_t y, uint8_t red, uint8_t green, uint8_t blue)
{
  __port_api_ret(GLIB_drawPixelRGB, x, y, red, green, blue);
}

sl_status_t sl_display_draw_pixel(int32_t x, int32_t y)
{
  __port_api_ret(GLIB_drawPixel, x, y);
}

sl_status_t sl_display_draw_pixel_color(int32_t x, int32_t y, uint32_t color)
{
  __port_api_ret(GLIB_drawPixelColor, x, y, color);
}

#if defined(SL_CATALOG_KERNEL_PRESENT)
void sl_display_renderer(sl_renderer_callback_t callback, sl_renderer_callback_arg_t args, uint32_t delay_after_ms)
{
  sl_display_renderer_queue_t renderer;
  renderer.callback = callback;
  renderer.args = args;
  renderer.delay_ms_after = delay_after_ms;
  sl_display_set(&renderer);
}
#endif
// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

#if defined(SL_CATALOG_KERNEL_PRESENT)
/* Mutex acquire */
static inline void _display_mutex_acquire(void)
{
  assert(osMutexAcquire(_display_mtx, osWaitForever) == osOK);
}

/* Mutex release */
static inline void _display_mutex_release(void)
{
  assert(osMutexRelease(_display_mtx) == osOK);
}

static void _display_renderer_task(void *args)
{
  osStatus_t stat;
  sl_display_renderer_queue_t disp_ctrl;
  uint8_t msg_prio = 0;
  (void) args;
  SL_WISUN_THREAD_LOOP {
    stat = osMessageQueueGet(_display_msg_queue, &disp_ctrl, &msg_prio, 0U);
    (void) msg_prio;
    if (stat == osOK) {
      _display_mutex_acquire();
      disp_ctrl.callback(disp_ctrl.args);
      _disp.update();
      osDelay(disp_ctrl.delay_ms_after);
      _display_mutex_release();
    }
    osDelay(1);
  }
}
#endif
