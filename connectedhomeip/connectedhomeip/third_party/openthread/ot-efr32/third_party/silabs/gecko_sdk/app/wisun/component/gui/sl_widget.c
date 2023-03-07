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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "sl_string.h"
#include "sl_widget.h"
#include "sl_widget_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Reset display region
 * @details Helper function
 * @param[in] region Region
 *****************************************************************************/
static inline void _reset_region(sl_display_rectangle_t * region);

/**************************************************************************//**
 * @brief Get max line count on widget
 * @details Helper function
 * @param[in] frame Frame
 * @return uint8_t Line count
 *****************************************************************************/
static inline uint8_t _get_max_lines_on_widget(const sl_display_rectangle_t
                                               *const frame);

/**************************************************************************//**
 * @brief Summarize sl_status
 * @details Helper function
 * @param[in,out] sum Sum destination
 * @param[in] stat Status to summarize
 *****************************************************************************/
static inline void _sum_status(sl_status_t * const sum, const sl_status_t stat);

/**************************************************************************//**
 * @brief Scroll optionlist
 * @details Helper function
 * @param[in,out] optionlist Optionlist
 *****************************************************************************/
static void _optionlist_scroll(sl_widget_option_list_t * const optionlist);

/**************************************************************************//**
 * @brief Get widget x size (width)
 * @details Helper function
 * @param[in] frame Frame
 * @return uint8_t X size (width)
 *****************************************************************************/
static inline uint8_t _get_widget_x_size(const sl_display_rectangle_t
                                         *const frame);

/**************************************************************************//**
 * @brief Get widget y size (width)
 * @details Helper function
 * @param[in] frame Frame
 * @return uint8_t Y size (width)
 *****************************************************************************/
static inline uint8_t _get_widget_y_size(const sl_display_rectangle_t
                                         *const frame);

/**************************************************************************//**
 * @brief Get string offset to align center
 * @details Helper function
 * @param[in] frame Frame
 * @param[in] str
 * @return uint8_t Offset value
 *****************************************************************************/
static inline uint8_t _get_str_offset_to_align_center(const sl_display_rectangle_t
                                                      *const frame, const char *str);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

sl_status_t sl_widget_title_init(sl_widget_title_t * const title)
{
  if (title == NULL) {
    return SL_STATUS_FAIL;
  }
  title->label = SL_WIDGET_EMPTY_STR;
  return SL_STATUS_OK;
}

sl_status_t sl_widget_title_update(const sl_widget_title_t * const title)
{
  sl_status_t res = SL_STATUS_OK;
  uint8_t offset = 0;

  if (title == NULL) {
    return SL_STATUS_FAIL;
  }

  offset = _get_str_offset_to_align_center(&title->frame, title->label);

#if SL_WIDGET_TITLE_DARK_THEME
  sl_display_invert_content();
#endif
  _reset_region((sl_display_rectangle_t *)&title->frame);
  _sum_status(&res, sl_display_draw_rect(&title->frame));
  _sum_status(&res, sl_display_draw_string(title->label,
                                           SL_WIDGET_MAX_LABEL_STR_LEN,
                                           title->frame.xMin + SL_WIDGET_FRAME_WIDTH + offset,
                                           title->frame.yMin + SL_WIDGET_FRAME_WIDTH,
                                           false));
#if SL_WIDGET_TITLE_DARK_THEME
  sl_display_invert_content();
#endif
  return res;
}

sl_status_t sl_widget_button_init(sl_widget_button_t * const button)
{
  if (button == NULL) {
    return SL_STATUS_FAIL;
  }
  button->label = SL_WIDGET_EMPTY_STR;
  button->event_hnd.callback = NULL;
  button->event_hnd.args = NULL;
  _reset_region((sl_display_rectangle_t *)&button->frame);
  return SL_STATUS_OK;
}

sl_status_t sl_widget_button_update(const sl_widget_button_t * const button)
{
  sl_status_t res = SL_STATUS_OK;
  uint8_t offset = 0;
  if (button == NULL) {
    return SL_STATUS_FAIL;
  }

  offset = _get_str_offset_to_align_center(&button->frame, button->label);
#if SL_WIDGET_BUTTON_DARK_THEME
  sl_display_invert_content();
#endif
  _reset_region((sl_display_rectangle_t *)&button->frame);
  _sum_status(&res, sl_display_draw_rect(&button->frame));
  _sum_status(&res, sl_display_draw_string(button->label,
                                           SL_WIDGET_MAX_LABEL_STR_LEN,
                                           button->frame.xMin + SL_WIDGET_FRAME_WIDTH + offset,
                                           button->frame.yMin + SL_WIDGET_FRAME_WIDTH,
                                           false));

#if SL_WIDGET_BUTTON_DARK_THEME
  sl_display_invert_content();
#endif
  return res;
}

sl_status_t sl_widget_button_call(const sl_widget_button_t * const button)
{
  if (button->event_hnd.callback != NULL) {
    button->event_hnd.callback(button->event_hnd.args);
    return SL_STATUS_OK;
  }
  return SL_STATUS_FAIL;
}

sl_status_t sl_widget_optionlist_init(sl_widget_option_list_t * const optionlist)
{
  if (optionlist == NULL) {
    return SL_STATUS_FAIL;
  }
  for (uint16_t i = 0; i < SL_WIDGET_OPTIONLIST_MAX_ITEM; ++i) {
    optionlist->items[i].idx = i;
    optionlist->items[i].label = NULL;
    optionlist->items[i].event_hnd.callback = NULL;
    optionlist->items[i].event_hnd.args = NULL;
  }
  optionlist->current_item = &optionlist->items[0];
  optionlist->top_item = &optionlist->items[0];
  optionlist->count = 0;
  return SL_STATUS_OK;
}

sl_status_t sl_widget_optionlist_add_item(sl_widget_option_list_t * const optionlist,
                                          const char * label,
                                          sl_widget_event_callback_t callback,
                                          sl_widget_event_args_t args)
{
  for (uint16_t i = 0; i < SL_WIDGET_OPTIONLIST_MAX_ITEM; ++i) {
    if (optionlist->items[i].label == NULL) {
      optionlist->items[i].label = label == NULL ? SL_WIDGET_EMPTY_STR : label;
      optionlist->items[i].event_hnd.callback = callback;
      optionlist->items[i].event_hnd.args = args;
      ++optionlist->count;
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_FAIL;
}

sl_status_t sl_widget_optionlist_update(const sl_widget_option_list_t * const optionlist)
{
  sl_status_t res = SL_STATUS_OK;
  uint16_t pos = 0;
  uint16_t max_items_on_screen = _get_max_lines_on_widget(&optionlist->frame);
  uint16_t offset = sl_widget_default_font.fontWidth + (2 * SL_WIDGET_FRAME_WIDTH);
#if SL_WIDGET_OPTIONLIST_DARK_THEME
  sl_display_invert_content();
#endif
  _reset_region((sl_display_rectangle_t *)&optionlist->frame);
  sl_display_draw_rect(&optionlist->frame);

  for (uint16_t i = optionlist->top_item->idx; i < (optionlist->top_item->idx + max_items_on_screen); ++i, ++pos) {
    if (optionlist->items[i].label == NULL) {
      break;
    }
    // print
    _sum_status(&res, sl_display_draw_string(optionlist->items[i].label,
                                             SL_WIDGET_MAX_LABEL_STR_LEN,
                                             optionlist->frame.xMin + offset,
                                             optionlist->frame.yMin + SL_WIDGET_FRAME_WIDTH
                                             + pos * (sl_widget_default_font.fontHeight + SL_WIDGET_FRAME_WIDTH),
                                             false));
  }
  // print select char
  _sum_status(&res, sl_display_draw_string(SL_WIDGET_OPTIONLIST_SELECT_CHAR,
                                           1U,
                                           optionlist->frame.xMin + SL_WIDGET_FRAME_WIDTH,
                                           optionlist->frame.yMin + SL_WIDGET_FRAME_WIDTH
                                           + (optionlist->current_item->idx - optionlist->top_item->idx)
                                           * (sl_widget_default_font.fontHeight + SL_WIDGET_FRAME_WIDTH),
                                           false));
#if SL_WIDGET_OPTIONLIST_DARK_THEME
  sl_display_invert_content();
#endif
  return res;
}

sl_status_t sl_widget_optionlist_scroll_down(sl_widget_option_list_t * const optionlist)
{
  if (optionlist == NULL) {
    return SL_STATUS_FAIL;
  }
  if (optionlist->current_item->idx < (optionlist->count - 1)) {
    ++optionlist->current_item;
  } else {
    optionlist->current_item = &optionlist->items[0];
  }
  _optionlist_scroll(optionlist);
  return SL_STATUS_OK;
}

sl_status_t sl_widget_optionlist_scroll_up(sl_widget_option_list_t * const optionlist)
{
  if (optionlist == NULL) {
    return SL_STATUS_FAIL;
  }
  if (optionlist->current_item->idx == 0) {
    optionlist->current_item =  &optionlist->items[optionlist->count - 1];
  } else {
    optionlist->current_item = &optionlist->items[optionlist->current_item->idx - 1];
  }
  _optionlist_scroll(optionlist);
  return SL_STATUS_OK;
}

sl_status_t sl_widget_optionlist_call(const sl_widget_option_list_t * const optionlist)
{
  if (optionlist != NULL) {
    optionlist->current_item->event_hnd.callback(optionlist->current_item->event_hnd.args);
    return SL_STATUS_OK;
  }
  return SL_STATUS_FAIL;
}

sl_status_t sl_widget_progressbar_init(sl_widget_progressbar_t * const progressbar)
{
  if (progressbar == NULL) {
    return SL_STATUS_FAIL;
  }
  progressbar->top_label = SL_WIDGET_EMPTY_STR;
  progressbar->bottom_label = SL_WIDGET_EMPTY_STR;
  progressbar->percentage = 0;
  _reset_region(&progressbar->frame);
  return SL_STATUS_OK;
}

sl_status_t sl_widget_progressbar_update(const sl_widget_progressbar_t * const progressbar)
{
  sl_status_t res = SL_STATUS_OK;
  uint16_t str_x_offset = 0;
  uint16_t prg_x_offset = 0;
  uint16_t prg_y_offset = 0;
  sl_display_rectangle_t prog_rect = { 0 };

  if (progressbar == NULL) {
    return SL_STATUS_FAIL;
  }

  prg_x_offset = (_get_widget_x_size(&progressbar->frame)
                  - (SL_WIDGET_PROGRESSBAR_MAX_CHAR_LEN * sl_widget_default_font.fontWidth)) / 2;

  prg_y_offset = (_get_widget_y_size(&progressbar->frame) - 2
                  * sl_widget_default_font.fontHeight - 2 * SL_WIDGET_FRAME_WIDTH) / 2
                 - sl_widget_default_font.fontHeight;
#if SL_WIDGET_PROGRESSBAR_DARK_THEME
  sl_display_invert_content();
#endif
  _reset_region((sl_display_rectangle_t * )&progressbar->frame);
  _sum_status(&res, sl_display_draw_rect(&progressbar->frame));

  prog_rect.xMin = progressbar->frame.xMin + prg_x_offset;
  prog_rect.xMax = progressbar->frame.xMin + _get_widget_x_size(&progressbar->frame) - prg_x_offset;
  prog_rect.yMin = progressbar->frame.yMin + sl_widget_default_font.fontHeight + SL_WIDGET_FRAME_WIDTH + prg_y_offset;
  prog_rect.yMax = prog_rect.yMin + sl_widget_default_font.fontHeight + 2;
  _sum_status(&res, sl_display_draw_rect(&prog_rect));
  _sum_status(&res, sl_display_draw_string(SL_WIDGET_PROGRESSBAR_STR_TEMPLATE,
                                           progressbar->percentage / 5,
                                           prog_rect.xMin + SL_WIDGET_FRAME_WIDTH,
                                           prog_rect.yMin + SL_WIDGET_FRAME_WIDTH,
                                           false));

  str_x_offset = _get_str_offset_to_align_center(&progressbar->frame, progressbar->top_label);
  _sum_status(&res, sl_display_draw_string(SL_WIDGET_OPTIONLIST_SELECT_CHAR,
                                           1,
                                           prog_rect.xMin + SL_WIDGET_FRAME_WIDTH,
                                           prog_rect.yMax + SL_WIDGET_FRAME_WIDTH,
                                           false));
  _sum_status(&res, sl_display_draw_string(progressbar->top_label,
                                           SL_WIDGET_MAX_LABEL_STR_LEN - 1,
                                           prog_rect.xMin + str_x_offset,
                                           prog_rect.yMax + SL_WIDGET_FRAME_WIDTH,
                                           false));

  str_x_offset = _get_str_offset_to_align_center(&progressbar->frame, progressbar->bottom_label);
  _sum_status(&res, sl_display_draw_string(progressbar->bottom_label,
                                           SL_WIDGET_MAX_LABEL_STR_LEN,
                                           progressbar->frame.xMin + str_x_offset,
                                           prog_rect.yMax + sl_widget_default_font.fontHeight + 2 * SL_WIDGET_FRAME_WIDTH,
                                           false));
#if SL_WIDGET_PROGRESSBAR_DARK_THEME
  sl_display_invert_content();
#endif
  return res;
}

sl_status_t sl_widget_progressbar_set_top_label(sl_widget_progressbar_t * const progressbar, const char * label)
{
  if (progressbar == NULL) {
    return SL_STATUS_FAIL;
  }
  progressbar->top_label = label == NULL ? SL_WIDGET_EMPTY_STR : label;
  return SL_STATUS_OK;
}

sl_status_t sl_widget_progressbar_set_bottom_label(sl_widget_progressbar_t * const progressbar, const char * label)
{
  if (progressbar == NULL) {
    return SL_STATUS_FAIL;
  }
  progressbar->bottom_label = label == NULL ? SL_WIDGET_EMPTY_STR : label;
  return SL_STATUS_OK;
}

sl_status_t sl_widget_progressbar_set_value(sl_widget_progressbar_t * const progressbar, const uint8_t percentage)
{
  if (progressbar == NULL) {
    return SL_STATUS_FAIL;
  }
  progressbar->percentage = percentage > 100 ? 100 : percentage;
  return SL_STATUS_OK;
}

sl_status_t sl_widget_textbox_init(sl_widget_textbox_t * const textbox)
{
  if (textbox == NULL) {
    return SL_STATUS_FAIL;
  }
  textbox->top_line = &textbox->lines[0];
  for (uint8_t i = 0; i < SL_WIDGET_TEXTBOX_MAX_LINES; ++i) {
    textbox->lines[i].str = NULL;
    textbox->lines[i].len = 0;
  }
  return SL_STATUS_OK;
}

sl_status_t sl_widget_textbox_update(const sl_widget_textbox_t * const textbox)
{
  sl_status_t res = SL_STATUS_OK;
  uint32_t top_idx = 0;
  uint16_t max_lines_on_screen = 0;

  if (textbox == NULL) {
    return SL_STATUS_FAIL;
  }

  max_lines_on_screen = _get_max_lines_on_widget(&textbox->frame);
  top_idx = (uint32_t)(textbox->top_line - &textbox->lines[0]);
#if SL_WIDGET_TEXTBOX_DARK_THEME
  sl_display_invert_content();
#endif
  _reset_region((sl_display_rectangle_t *)&textbox->frame);
  _sum_status(&res, sl_display_draw_rect(&textbox->frame));

  for (uint8_t i = 0; i < max_lines_on_screen; ++i) {
    if (textbox->lines[i + top_idx].str == NULL) {
      break;
    }
    _sum_status(&res, sl_display_draw_string(
                  textbox->lines[i + top_idx].str,
                  textbox->lines[i + top_idx].len,
                  textbox->frame.xMin + SL_WIDGET_FRAME_WIDTH,
                  textbox->frame.yMin + SL_WIDGET_FRAME_WIDTH
                  + i * (sl_widget_default_font.fontHeight + SL_WIDGET_FRAME_WIDTH),
                  false));
  }
#if SL_WIDGET_TEXTBOX_DARK_THEME
  sl_display_invert_content();
#endif
  return res;
}

sl_status_t sl_widget_textbox_set(sl_widget_textbox_t * const textbox, const char * const content)
{
  const char *p_start = content;
  const char *p_end = content;

  if (textbox == NULL || content == NULL) {
    return SL_STATUS_FAIL;
  }

  textbox->line_count = 0;
  for (uint8_t i = 0; i < SL_WIDGET_TEXTBOX_MAX_LINES && *p_start; ++i, ++textbox->line_count) {
    textbox->lines[i].str = p_start;
    textbox->lines[i].len = 0;
    p_end = p_start;
    while (*p_end && *p_end != '\n' && textbox->lines[i].len < SL_WIDGET_MAX_LABEL_STR_LEN) {
      ++p_end;
      ++textbox->lines[i].len;
    }
    if (*p_end == '\n') {
      ++p_end;
    }
    p_start = p_end;
  }
  return SL_STATUS_OK;
}

sl_status_t sl_widget_textbox_scroll_down(sl_widget_textbox_t * const textbox)
{
  uint32_t idx = 0;
  uint16_t max_lines_on_screen = 0;

  if (textbox == NULL) {
    return SL_STATUS_FAIL;
  }

  idx = textbox->top_line - &textbox->lines[0];
  max_lines_on_screen = _get_max_lines_on_widget(&textbox->frame);

  if (idx < (SL_WIDGET_TEXTBOX_MAX_LINES - 1) && (textbox->line_count - idx) > max_lines_on_screen) {
    ++idx;
    textbox->top_line = &textbox->lines[idx];
  } else {
    textbox->top_line = &textbox->lines[0];
  }
  return SL_STATUS_OK;
}

sl_status_t sl_widget_textbox_scroll_up(sl_widget_textbox_t * const textbox)
{
  uint32_t idx = 0;
  if (textbox == NULL) {
    return SL_STATUS_FAIL;
  }
  idx = textbox->top_line - &textbox->lines[0];
  if (idx > 0) {
    --idx;
    textbox->top_line = &textbox->lines[idx];
  }
  return SL_STATUS_OK;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static inline void _reset_region(sl_display_rectangle_t * region)
{
  sl_display_set_clipping_region(region);
  sl_display_clear_region();
  sl_display_reset_clipping_region();
}

static inline void _sum_status(sl_status_t * const sum, const sl_status_t stat)
{
  if (*sum != SL_STATUS_FAIL) {
    *sum = stat;
  }
}

static inline uint8_t _get_max_lines_on_widget(const sl_display_rectangle_t * const frame)
{
  return (frame->yMax - frame->yMin) / (sl_widget_default_font.fontHeight + SL_WIDGET_FRAME_WIDTH);
}

static void _optionlist_scroll(sl_widget_option_list_t * const optionlist)
{
  uint32_t gap = optionlist->current_item - optionlist->top_item;
  uint16_t max_item_on_screen = _get_max_lines_on_widget(&optionlist->frame);
  if (optionlist->current_item->idx == 0) {
    optionlist->top_item = optionlist->current_item;
  } else if (gap >= max_item_on_screen) {
    optionlist->top_item = (optionlist->current_item - max_item_on_screen) +  1;
  }
}

static inline uint8_t _get_widget_x_size(const sl_display_rectangle_t *const frame)
{
  return frame->xMax - frame->xMin;
}

static inline uint8_t _get_widget_y_size(const sl_display_rectangle_t *const frame)
{
  return frame->yMax - frame->yMin;
}

static inline uint8_t _get_str_offset_to_align_center(const sl_display_rectangle_t *const frame,
                                                      const char *str)
{
  return ((_get_widget_x_size(frame) - (SL_WIDGET_FRAME_WIDTH * 2))
          - (sl_strnlen((char *)str, SL_WIDGET_MAX_LABEL_STR_LEN) * sl_widget_default_font.fontWidth)) / 2 + SL_WIDGET_FRAME_WIDTH;
}
