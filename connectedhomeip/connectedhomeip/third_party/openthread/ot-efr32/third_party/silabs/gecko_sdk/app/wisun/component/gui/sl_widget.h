/***************************************************************************//**
 * @file
 * @brief Widget
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
#ifndef __SL_WIDGET_H__
#define __SL_WIDGET_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sl_display.h"
#include "sl_status.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// EMPTY string
#define SL_WIDGET_EMPTY_STR                      ""

/// Optionlist max item
#define SL_WIDGET_OPTIONLIST_MAX_ITEM            (32U)

/// Textbox max lines
#define SL_WIDGET_TEXTBOX_MAX_LINES              (32U)

/// General Widget frame width
#define SL_WIDGET_FRAME_WIDTH                    (2U)

/// Optionlist Select Char
#define SL_WIDGET_OPTIONLIST_SELECT_CHAR         ">"

/// Max label string length
#define SL_WIDGET_MAX_LABEL_STR_LEN              (20U)

/// Progressbar max char length
#define SL_WIDGET_PROGRESSBAR_MAX_CHAR_LEN       (20U)

/// Progressbar char template
#define SL_WIDGET_PROGRESSBAR_STR_TEMPLATE       "####################"

/// Widget default font
#define sl_widget_default_font                   GLIB_FontNarrow6x8

/// Widget event argument type definition
typedef void * sl_widget_event_args_t;

/// Widget event callback type definition
typedef void (* sl_widget_event_callback_t)(sl_widget_event_args_t);

/// Widget event handler
typedef struct sl_widget_event_hnd {
  /// Callback
  sl_widget_event_callback_t callback;
  /// Arguments
  sl_widget_event_args_t args;
} sl_widget_event_hnd_t;

/// Button Widget type definition
typedef struct sl_widget_button {
  /// Identifier
  uint16_t id;
  /// Label
  const char *label;
  /// Frame
  const sl_display_rectangle_t frame;
  /// Event handler
  sl_widget_event_hnd_t event_hnd;
} sl_widget_button_t;

/// Optionitem type definition
typedef struct sl_widget_option_item {
  /// Index
  uint16_t idx;
  /// Label
  const char *label;
  /// Event handler
  sl_widget_event_hnd_t event_hnd;
} sl_widget_option_item_t;

/// Optionlist Widget type definition
typedef struct  sl_widget_option_list {
  /// Optionlist item buffer
  sl_widget_option_item_t items[SL_WIDGET_OPTIONLIST_MAX_ITEM];
  /// Currently selected item
  sl_widget_option_item_t *current_item;
  /// Top item on the screen
  sl_widget_option_item_t *top_item;
  /// Frame
  sl_display_rectangle_t frame;
  /// Count
  uint16_t count;
} sl_widget_option_list_t;

/// Title widget type definition
typedef struct sl_widget_title {
  /// Label
  const char * label;
  /// Frame
  sl_display_rectangle_t frame;
} sl_widget_title_t;

/// Progressbar Widget type definition
typedef struct sl_widget_progressbar {
  /// Top label
  const char *top_label;
  /// Bottom label
  const char *bottom_label;
  /// Percentage value
  uint8_t percentage;
  //// Frame
  sl_display_rectangle_t frame;
} sl_widget_progressbar_t;

/// Textbox line type definition
typedef struct sl_widget_textbox_line {
  /// String pointer
  const char *str;
  /// Length
  size_t len;
}sl_widget_textbox_line_t;

/// Textbox Widget type definition
typedef struct sl_widget_textbox {
  /// Lines
  sl_widget_textbox_line_t lines[SL_WIDGET_TEXTBOX_MAX_LINES];
  /// Top line on the screen
  sl_widget_textbox_line_t *top_line;
  /// Frame
  sl_display_rectangle_t frame;
  /// Line count
  uint8_t line_count;
} sl_widget_textbox_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Init title.
 * @details Init title widget structure
 * @param[in,out] title Title
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_title_init(sl_widget_title_t * const title);

/**************************************************************************//**
 * @brief Reset title.
 * @details Call init widget function
 * @param[in,out] title Title
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
static inline sl_status_t sl_widget_title_reset(sl_widget_title_t
                                                *const title)
{
  return sl_widget_title_init(title);
}

/**************************************************************************//**
 * @brief Update title.
 * @details Update the content of the widget
 * @param[in] title Title
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_title_update(const sl_widget_title_t * const title);

/**************************************************************************//**
 * @brief Init button.
 * @details Init button widget structure
 * @param[in,out] button Button
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_button_init(sl_widget_button_t * const button);

/**************************************************************************//**
 * @brief Reset button.
 * @details Call init widget function
 * @param[in,out] button Button
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
static inline sl_status_t sl_widget_button_reset(sl_widget_button_t
                                                 *const button)
{
  return sl_widget_button_init(button);
}

/**************************************************************************//**
 * @brief Update button.
 * @details Update the content of the widget
 * @param[in] button Button
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_button_update(const sl_widget_button_t
                                    *const button);

/**************************************************************************//**
 * @brief Call button event handler.
 * @details Calling the event handler function with arguments
 * @param[in] button Button
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_button_call(const sl_widget_button_t * const button);

/**************************************************************************//**
 * @brief Init optionlist.
 * @details Init optionlist widget structure
 * @param[in,out] optionlist Optionlist
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_optionlist_init(sl_widget_option_list_t * const optionlist);

/**************************************************************************//**
 * @brief Reset optionlist.
 * @details Call widget init function
 * @param[in,out] optionlist Optionlist
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
static inline sl_status_t sl_widget_optionlist_reset(sl_widget_option_list_t *
                                                     const optionlist)
{
  return sl_widget_optionlist_init(optionlist);
}

/**************************************************************************//**
 * @brief Update optionlist.
 * @details Update optionlist widget content
 * @param[in] optionlist Optionlist
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_optionlist_update(const sl_widget_option_list_t
                                        *const optionlist);

/**************************************************************************//**
 * @brief Add item to the optionlist.
 * @details Item contains label, callback and arguments
 * @param[in,out] optionlist Optionlist
 * @param[in] label Label
 * @param[in] callback Callback
 * @param[in] args Arguments
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_optionlist_add_item(sl_widget_option_list_t
                                          *const optionlist,
                                          const char * label,
                                          sl_widget_event_callback_t callback,
                                          sl_widget_event_args_t args);

/**************************************************************************//**
 * @brief Scroll down optionlist.
 * @details Scrolling the optionlist down and update top item too.
 * @param[in,out] optionlist Optinolist
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_optionlist_scroll_down(sl_widget_option_list_t
                                             *const optionlist);

/**************************************************************************//**
 * @brief Scroll up optionlist.
 * @details Scrolling the optionlist up and update top item too.
 * @param[in,out] optionlist Optinolist
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_optionlist_scroll_up(sl_widget_option_list_t
                                           *const optionlist);

/**************************************************************************//**
 * @brief Call optionlist selected item event handler.
 * @details Calling the selected item callback with args
 * @param[in] optionlist Optionlist
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_optionlist_call(const sl_widget_option_list_t
                                      *const optionlist);

/**************************************************************************//**
 * @brief Init progressbar.
 * @details Init textbox widget structure
 * @param[in,out] progressbar Progressbar
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_progressbar_init(sl_widget_progressbar_t
                                       *const progressbar);

/**************************************************************************//**
 * @brief Reset progressbar.
 * @details Call widget init function
 * @param[in,out] progressbar Progressbar
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
static inline sl_status_t sl_widget_progressbar_reset(sl_widget_progressbar_t
                                                      *const progressbar)
{
  return sl_widget_progressbar_init(progressbar);
}

/**************************************************************************//**
 * @brief Update progressbar.
 * @details Update progressbar widget content
 * @param[in] progressbar Progressbar
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_progressbar_update(const sl_widget_progressbar_t
                                         *const progressbar);

/**************************************************************************//**
 * @brief Set progressbar top label.
 * @details Set the top label of the progressbar
 * @param[in,out] progressbar Progressbar
 * @param[in] label Labal
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_progressbar_set_top_label(sl_widget_progressbar_t
                                                *const progressbar,
                                                const char *label);

/**************************************************************************//**
 * @brief Set progressbar bottom label.
 * @details Set the top label of the progressbar
 * @param[in,out] progressbar Progressbar
 * @param[in] label Labal
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_progressbar_set_bottom_label(sl_widget_progressbar_t
                                                   *const progressbar,
                                                   const char *label);

/**************************************************************************//**
 * @brief Set progressbar percentage value.
 * @details Set progressbar percentage value with limit check,
 *          the value should be in range 0-100
 * @param[in,out] progressbar
 * @param[in] percentage
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_progressbar_set_value(sl_widget_progressbar_t
                                            *const progressbar,
                                            const uint8_t percentage);

/**************************************************************************//**
 * @brief Init textbox.
 * @details Init textbox widget structure
 * @param[in,out] textbox Textbox
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_textbox_init(sl_widget_textbox_t * const textbox);

/**************************************************************************//**
 * @brief Reset textbox.
 * @details Call init widget function
 * @param[in,out] textbox Textbox
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
static inline sl_status_t sl_widget_textbox_reset(sl_widget_textbox_t
                                                  *const textbox)
{
  return sl_widget_textbox_init(textbox);
}

/**************************************************************************//**
 * @brief Update textbox.
 * @details Update textbox widget content
 * @param[in] textbox Textbox
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_textbox_update(const sl_widget_textbox_t * const textbox);

/**************************************************************************//**
 * @brief Set textbox.
 * @details Set the content string of the textbox
 * @param[in,out] textbox Textbox
 * @param[in] content Content string
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_textbox_set(sl_widget_textbox_t * const textbox,
                                  const char * const content);

/**************************************************************************//**
 * @brief Scroll textbox down.
 * @details Scrolling the text box content down and set top line too
 * @param[in,out] textbox Textbox
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_textbox_scroll_down(sl_widget_textbox_t * const textbox);

/**************************************************************************//**
 * @brief Scroll textbox up.
 * @details Scrolling the text box content up and set top line too
 * @param[in,out] textbox Textbox
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_widget_textbox_scroll_up(sl_widget_textbox_t * const textbox);

#ifdef __cplusplus
}
#endif
#endif
