/***************************************************************************//**
 * @file
 * @brief Graphical User Interface
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

#include <assert.h>
#include "sl_gui.h"
#include "sl_component_catalog.h"
#include "sl_simple_button_instances.h"
#include "sl_display.h"
#include "sl_wisun_trace_util.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// GUI width
#define GUI_MAX_X_SIZE                     (128U)

/// GUI height
#define GUI_MAX_Y_SIZE                     (128U)

/// GUI title x size
#define GUI_TITLE_X_SIZE                   (GUI_MAX_X_SIZE - (2 * SL_WIDGET_FRAME_WIDTH) - 1)

/// GUI title y size
#define GUI_TITLE_Y_SIZE                   (10U)

/// GUI title max string length
#define GUI_TITLE_MAX_STR_LEN              (20U)

/// GUI title x position
#define GUI_TITLE_X_POS                    (SL_WIDGET_FRAME_WIDTH)

/// GUI title y position
#define GUI_TITLE_Y_POS                    (SL_WIDGET_FRAME_WIDTH)

/// GUI button x size
#define GUI_BUTTON_X_SIZE                  (59U)

/// GUI button y size
#define GUI_BUTTON_Y_SIZE                  (10U)

/// GUI button1 x position
#define GUI_BUTTON1_X_POS                  (SL_WIDGET_FRAME_WIDTH)

/// GUI button1 y position
#define GUI_BUTTON1_Y_POS                  (GUI_MAX_Y_SIZE - SL_WIDGET_FRAME_WIDTH - GUI_BUTTON_Y_SIZE - 1)

/// GUI button0 x position
#define GUI_BUTTON0_X_POS                  (GUI_MAX_X_SIZE - SL_WIDGET_FRAME_WIDTH - GUI_BUTTON_X_SIZE - 1)

/// GUI button0 y position
#define GUI_BUTTON0_Y_POS                  (GUI_BUTTON1_Y_POS)

/// GUI button max string length
#define GUI_BUTTON_LABEL_MAX_STR_LEN       (10U)

/// GUI main frame x size
#define GUI_MAIN_FRAME_X_SIZE              ((GUI_MAX_X_SIZE - (2 * SL_WIDGET_FRAME_WIDTH) - 1))

/// GUI main frame y size
#define GUI_MAIN_FRAME_Y_SIZE              (GUI_MAX_Y_SIZE - GUI_BUTTON_Y_SIZE \
                                            - (2 * SL_WIDGET_FRAME_WIDTH)      \
                                            - GUI_TITLE_Y_SIZE - (2 * SL_WIDGET_FRAME_WIDTH) - SL_WIDGET_FRAME_WIDTH)
/// GUI main frame x position
#define GUI_MAIN_FRAME_X_POS               (2U)

/// GUI main frame y position
#define GUI_MAIN_FRAME_Y_POS               (GUI_TITLE_Y_POS + GUI_TITLE_Y_SIZE +  SL_WIDGET_FRAME_WIDTH + 1)

/// GUI optionlist x size
#define GUI_OPTION_X_SIZE                  GUI_MAIN_FRAME_X_SIZE

/// GUI optionlist y size
#define GUI_OPTION_Y_SIZE                  GUI_MAIN_FRAME_Y_SIZE

/// GUI optionlist x position
#define GUI_OPTION_X_POS                   GUI_MAIN_FRAME_X_POS

/// GUI optionlist y position
#define GUI_OPTION_Y_POS                   GUI_MAIN_FRAME_Y_POS

/// GUI optionlist label max string length
#define GUI_OPTION_LABEL_MAX_STR_LEN       (18U)

/// GUI progressbar x size
#define GUI_PROGRESSBAR_X_SIZE             GUI_MAIN_FRAME_X_SIZE

/// GUI progressbar y size
#define GUI_PROGRESSBAR_Y_SIZE             GUI_MAIN_FRAME_Y_SIZE

/// GUI progressbar x position
#define GUI_PROGRESSBAR_X_POS              GUI_MAIN_FRAME_X_POS

/// GUI progressbar y position
#define GUI_PROGRESSBAR_Y_POS              GUI_MAIN_FRAME_Y_POS

/// GUI progressbar label max string length
#define GUI_PROGRESSBAR_LABEL_MAX_STR_LEN  GUI_TITLE_MAX_STR_LEN

/// GUI textbox x size
#define GUI_TEXTBOX_X_SIZE                 GUI_MAIN_FRAME_X_SIZE

/// GUI textbox y size
#define GUI_TEXTBOX_Y_SIZE                 GUI_MAIN_FRAME_Y_SIZE

/// GUI textbox x position
#define GUI_TEXTBOX_X_POS                  GUI_MAIN_FRAME_X_POS

/// GUI textbox y position
#define GUI_TEXTBOX_Y_POS                  GUI_MAIN_FRAME_Y_POS

/// GUI textbox max line string length
#define GUI_TEXTBOX_MAX_LINE_STR_LEN       (20U)

/// Widget event queue max size
#define WIDGET_EVENT_QUEUE_MAX_SIZE        (32U)

/// GUI event task stack size
#define GUI_EVT_TASK_STACK_SIZE            (128U)

#if defined(SL_CATALOG_KERNEL_PRESENT)
/// Call widget API macro function
#define __call_widget_api(__widget_api, __widget_ptr, ...) \
  do {                                                     \
    _gui_mutex_acquire();                                  \
    __widget_api(__widget_ptr, ##__VA_ARGS__);             \
    _gui_mutex_release();                                  \
  } while (0)

/// Call widget API and return status macro function
#define __call_widget_api_ret_stat(__widget_api, __widget_ptr, ...) \
  do {                                                              \
    sl_status_t res = SL_STATUS_FAIL;                               \
    _gui_mutex_acquire();                                           \
    res = __widget_api(__widget_ptr, ##__VA_ARGS__);                \
    _gui_mutex_release();                                           \
    return res;                                                     \
  } while (0)
#else
/// Call widget API macro function
#define __call_widget_api(__widget_api, __widget_ptr, ...) \
  do {                                                     \
    __widget_api(__widget_ptr, ##__VA_ARGS__);             \
  } while (0)

/// Call widget API and return status macro function
#define __call_widget_api_ret_stat(__widget_api, __widget_ptr, ...) \
  do {                                                              \
    sl_status_t res = SL_STATUS_FAIL;                               \
    res = __widget_api(__widget_ptr, ##__VA_ARGS__);                \
    return res;                                                     \
  } while (0)
#endif

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

#if defined(SL_CATALOG_KERNEL_PRESENT)
/**************************************************************************//**
 * @brief Mutex acquire
 * @details Helper function
 *****************************************************************************/
static inline void _gui_mutex_acquire(void);

/**************************************************************************//**
 * @brief Mutex release
 * @details Helper function
 *****************************************************************************/
static inline void _gui_mutex_release(void);

/**************************************************************************//**
 * @brief GUI event task function
 * @details Task function
 * @param args Arguments
 *****************************************************************************/
static void _gui_event_task(void *args);
#endif

/**************************************************************************//**
 * @brief Get button instance by ID
 * @details Helper function
 * @param[in] id Button ID
 * @return sl_widget_button_t* Button instance
 *****************************************************************************/
static inline sl_widget_button_t * _get_button_by_id(const sl_gui_button_id_t id);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
#if defined(SL_CATALOG_KERNEL_PRESENT)
/// GUI mutex
static osMutexId_t _gui_mtx = NULL;

/// GUI event message queue control block
static uint64_t _gui_evt_msg_queue_cb[osMessageQueueCbSize] = { 0 };

/// GUI event task control block
static uint64_t _gui_evt_task_cb[osThreadCbSize] = { 0 };

/// GUI event task stack allocation
static uint64_t _gui_evt_stack[GUI_EVT_TASK_STACK_SIZE] = { 0 };

/// GUI event message queue buffer
static sl_widget_event_hnd_t _event_buff[WIDGET_EVENT_QUEUE_MAX_SIZE] = { 0 };

/// GUI event task attributes
static const osThreadAttr_t _gui_evt_task_attr = {
  .name        = "GuiEvent",
  .attr_bits   = osThreadDetached,
  .cb_mem      = _gui_evt_task_cb,
  .cb_size     = sizeof(_gui_evt_task_cb),
  .stack_mem   = _gui_evt_stack,
  .stack_size  = sizeof(_gui_evt_stack),
  .priority    = osPriorityLow,
  .tz_module   = 0
};

/// GUI event messagequeue attributes
static const osMessageQueueAttr_t _gui_evt_msg_queue_attr = {
  .name = "GuiEventMsgQueue",
  .attr_bits = 0,
  .cb_mem = _gui_evt_msg_queue_cb,
  .cb_size = sizeof(_gui_evt_msg_queue_cb),
  .mq_mem = _event_buff,
  .mq_size = sizeof(_event_buff)
};

/// GUI event thread ID
static osThreadId_t _gui_evt_thr_id = NULL;

/// GUI event message queue
static osMessageQueueId_t _gui_evt_msg_queue = NULL;

/// GUI mutex control block
__ALIGNED(8) static uint8_t _gui_mtx_cb[osMutexCbSize] = { 0 };

/// GUI mutex attributes
static const osMutexAttr_t _gui_mtx_attr = {
  .name      = "GuiMutex",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _gui_mtx_cb,
  .cb_size   = sizeof(_gui_mtx_cb)
};
#endif

/// GUI title instance
static sl_widget_title_t _gui_title = {
  .label = NULL,
  .frame = {
    .xMin = GUI_TITLE_X_POS,
    .xMax = GUI_TITLE_X_POS + GUI_TITLE_X_SIZE,
    .yMin = GUI_TITLE_Y_POS,
    .yMax = GUI_TITLE_Y_POS + GUI_TITLE_Y_SIZE
  }
};

/// GUI button0 instance
static sl_widget_button_t _gui_btn0  = {
  .id = SL_GUI_BUTTON0,
  .label = NULL,
  .frame = {
    .xMin = GUI_BUTTON0_X_POS,
    .xMax = GUI_BUTTON0_X_POS + GUI_BUTTON_X_SIZE,
    .yMin = GUI_BUTTON0_Y_POS,
    .yMax = GUI_BUTTON0_Y_POS + GUI_BUTTON_Y_SIZE,
  },
  .event_hnd = {
    .callback = NULL,
    .args = NULL,
  }
};

/// GUI button1 instance
static sl_widget_button_t _gui_btn1  = {
  .id = SL_GUI_BUTTON1,
  .label = NULL,
  .frame = {
    .xMin = GUI_BUTTON1_X_POS,
    .xMax = GUI_BUTTON1_X_POS + GUI_BUTTON_X_SIZE,
    .yMin = GUI_BUTTON1_Y_POS,
    .yMax = GUI_BUTTON1_Y_POS + GUI_BUTTON_Y_SIZE,
  },
  .event_hnd = {
    .callback = NULL,
    .args = NULL,
  }
};

/// GUI optionlist instance
static sl_widget_option_list_t _gui_optionlist = {
  .frame = {
    .xMin = GUI_OPTION_X_POS,
    .xMax = GUI_OPTION_X_POS + GUI_OPTION_X_SIZE,
    .yMin = GUI_OPTION_Y_POS,
    .yMax = GUI_OPTION_Y_POS + GUI_OPTION_Y_SIZE
  }
};

/// GUI progressbar instance
static sl_widget_progressbar_t _gui_progressbar = {
  .frame = {
    .xMin = GUI_PROGRESSBAR_X_POS,
    .xMax = GUI_PROGRESSBAR_X_POS + GUI_PROGRESSBAR_X_SIZE,
    .yMin = GUI_PROGRESSBAR_Y_POS,
    .yMax = GUI_PROGRESSBAR_Y_POS + GUI_PROGRESSBAR_Y_SIZE
  }
};

/// GUI textbox instance
static sl_widget_textbox_t _gui_textbox = {
  .line_count = 0,
  .top_line = NULL,
  .frame = {
    .xMin = GUI_TEXTBOX_X_POS,
    .xMax = GUI_TEXTBOX_X_POS + GUI_TEXTBOX_X_SIZE,
    .yMin = GUI_TEXTBOX_Y_POS,
    .yMax = GUI_TEXTBOX_Y_POS + GUI_TEXTBOX_Y_SIZE
  }
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void sl_gui_init(void)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _gui_mtx = osMutexNew(&_gui_mtx_attr);
  assert(_gui_mtx != NULL);

  _gui_evt_msg_queue = osMessageQueueNew(WIDGET_EVENT_QUEUE_MAX_SIZE,
                                         sizeof(sl_widget_event_hnd_t),
                                         &_gui_evt_msg_queue_attr);
  assert(_gui_evt_msg_queue != NULL);

  _gui_evt_thr_id = osThreadNew(_gui_event_task, NULL, &_gui_evt_task_attr);
  assert(_gui_evt_thr_id != NULL);
#else
  (void) 0;
#endif
}

#if defined(SL_CATALOG_KERNEL_PRESENT)
void _gui_event_task(void *args)
{
  osStatus_t stat;
  sl_widget_event_hnd_t evt_hnd;
  uint8_t msg_prio = 0;
  (void) args;
  // Set font
  sl_display_set_font((sl_display_font_t *)&sl_widget_default_font);
  SL_WISUN_THREAD_LOOP {
    stat = osMessageQueueGet(_gui_evt_msg_queue, &evt_hnd, &msg_prio, 0U);
    (void) msg_prio;
    if (stat == osOK) {
      _gui_mutex_acquire();
      if (evt_hnd.callback != NULL) {
        evt_hnd.callback(evt_hnd.args);
      }
      _gui_mutex_release();
    }
    osDelay(1);
  }
}
#endif

sl_status_t sl_gui_button_init(const sl_gui_button_id_t id)
{
  __call_widget_api_ret_stat(sl_widget_button_init,
                             _get_button_by_id(id));
}

sl_status_t sl_gui_button_update(const sl_gui_button_id_t id)
{
  __call_widget_api_ret_stat(sl_widget_button_update,
                             _get_button_by_id(id));
}

void sl_gui_button_set_label(const sl_gui_button_id_t id, const char *label)
{
  sl_widget_button_t *btn = NULL;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _gui_mutex_acquire();
#endif
  btn = _get_button_by_id(id);
  btn->label = label;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _gui_mutex_release();
#endif
}

void sl_gui_button_set_callback(const sl_gui_button_id_t id, sl_widget_event_callback_t callback, sl_widget_event_args_t args)
{
  sl_widget_button_t *btn = NULL;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _gui_mutex_acquire();
#endif
  btn = _get_button_by_id(id);
  btn->event_hnd.callback = callback;
  btn->event_hnd.args = args;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _gui_mutex_release();
#endif
}

void sl_gui_button_call(const sl_gui_button_id_t id)
{
  __call_widget_api(sl_widget_button_call,
                    _get_button_by_id(id));
}

sl_status_t sl_gui_title_init(void)
{
  __call_widget_api_ret_stat(sl_widget_title_init, &_gui_title);
}

sl_status_t sl_gui_title_update(void)
{
  __call_widget_api_ret_stat(sl_widget_title_update, &_gui_title);
}

void sl_gui_title_set_label(const char * const label)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _gui_mutex_acquire();
#endif
  _gui_title.label = label;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _gui_mutex_release();
#endif
}

sl_status_t sl_gui_optionlist_init(void)
{
  __call_widget_api_ret_stat(sl_widget_optionlist_init, &_gui_optionlist);
}

sl_status_t sl_gui_optionlist_update(void)
{
  __call_widget_api_ret_stat(sl_widget_optionlist_update, &_gui_optionlist);
}

sl_status_t sl_gui_optionlist_add_item(const char * label, sl_widget_event_callback_t callback, sl_widget_event_args_t args)
{
  __call_widget_api_ret_stat(sl_widget_optionlist_add_item, &_gui_optionlist, label, callback, args);
}

sl_status_t sl_gui_progressbar_init(void)
{
  __call_widget_api_ret_stat(sl_widget_progressbar_init, &_gui_progressbar);
}

sl_status_t sl_gui_progressbar_update(void)
{
  __call_widget_api_ret_stat(sl_widget_progressbar_update, &_gui_progressbar);
}

void sl_gui_progressbar_set_value(const uint8_t percentage)
{
  __call_widget_api(sl_widget_progressbar_set_value, &_gui_progressbar, percentage);
}

void sl_gui_progressbar_set_top_label(const char * label)
{
  __call_widget_api(sl_widget_progressbar_set_top_label, &_gui_progressbar, label);
}

void sl_gui_progressbar_set_bottom_label(const char * label)
{
  __call_widget_api(sl_widget_progressbar_set_bottom_label, &_gui_progressbar, label);
}

sl_status_t sl_gui_textbox_init(void)
{
  __call_widget_api_ret_stat(sl_widget_textbox_init, &_gui_textbox);
}

sl_status_t sl_gui_textbox_update(void)
{
  __call_widget_api_ret_stat(sl_widget_textbox_update, &_gui_textbox);
}

sl_status_t sl_gui_textbox_set(const char * content)
{
  __call_widget_api_ret_stat(sl_widget_textbox_set, &_gui_textbox, content);
}

sl_status_t sl_gui_textbox_scroll_down(void)
{
  __call_widget_api_ret_stat(sl_widget_textbox_scroll_down, &_gui_textbox);
}

sl_status_t sl_gui_textbox_scroll_up(void)
{
  __call_widget_api_ret_stat(sl_widget_textbox_scroll_up, &_gui_textbox);
}

sl_status_t sl_gui_optionlist_scroll_down(void)
{
  __call_widget_api_ret_stat(sl_widget_optionlist_scroll_down, &_gui_optionlist);
}

sl_status_t sl_gui_optionlist_scroll_up(void)
{
  __call_widget_api_ret_stat(sl_widget_optionlist_scroll_up, &_gui_optionlist);
}

void sl_gui_optionlist_assign_event_hnd_to_btn(const sl_gui_button_id_t id)
{
  sl_widget_button_t *btn = NULL;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _gui_mutex_acquire();
#endif
  btn = _get_button_by_id(id);
  btn->event_hnd.callback = _gui_optionlist.current_item->event_hnd.callback;
  btn->event_hnd.args = _gui_optionlist.current_item->event_hnd.args;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _gui_mutex_release();
#endif
}

void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) != SL_SIMPLE_BUTTON_PRESSED) {
    return;
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (handle == &sl_button_btn0) {
    osMessageQueuePut(_gui_evt_msg_queue, &_gui_btn0.event_hnd, 0U, 0U);
  } else if (handle == &sl_button_btn1) {
    osMessageQueuePut(_gui_evt_msg_queue, &_gui_btn1.event_hnd, 0U, 0U);
  }
#else
  (void) 0;
#endif
}

void sl_gui_init_all_widget(void)
{
  sl_display_clear();
  sl_gui_title_init();
  sl_gui_button_init(SL_GUI_BUTTON0);
  sl_gui_button_init(SL_GUI_BUTTON1);
  sl_gui_optionlist_init();
  sl_gui_progressbar_init();
  sl_gui_textbox_init();
}

static inline sl_widget_button_t * _get_button_by_id(const sl_gui_button_id_t id)
{
  return id == SL_GUI_BUTTON0 ? &_gui_btn0 : &_gui_btn1;
}

#if defined(SL_CATALOG_KERNEL_PRESENT)
/* Mutex acquire */
static inline void _gui_mutex_acquire(void)
{
  assert(osMutexAcquire(_gui_mtx, osWaitForever) == osOK);
}

/* Mutex release */
static inline void _gui_mutex_release(void)
{
  assert(osMutexRelease(_gui_mtx) == osOK);
}
#endif
