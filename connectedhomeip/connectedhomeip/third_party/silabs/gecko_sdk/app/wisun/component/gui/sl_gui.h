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

#ifndef __SL_GUI_H__
#define __SL_GUI_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sl_widget.h"
#include "sl_status.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// GUI Button init
typedef enum sl_gui_button_id {
  /// GUI Button0
  SL_GUI_BUTTON0,
  /// GUI Button1
  SL_GUI_BUTTON1
} sl_gui_button_id_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief GUI init
 * @details Initialize GUI thread-safe component
 *****************************************************************************/
void sl_gui_init(void);

/**************************************************************************//**
 * @brief Init all widget
 * @details Initialize all of widgets
 *****************************************************************************/
void sl_gui_init_all_widget(void);

/**************************************************************************//**
 * @brief Reset all of widgets
 * @details Call init all
 *****************************************************************************/
static inline void sl_gui_reset_all_widget(void)
{
  sl_gui_init_all_widget();
}

/**************************************************************************//**
 * @brief Init GUI title widget
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_title_init(void);

/**************************************************************************//**
 * @brief Update GUI title widget
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_title_update(void);

/**************************************************************************//**
 * @brief Set GUI title label
 * @details Wrapper function
 * @param[in] label Label
 *****************************************************************************/
void sl_gui_title_set_label(const char * const label);

/**************************************************************************//**
 * @brief Init GUI button widget
 * @details Wrapper function
 * @param[in] id Button ID
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_button_init(const sl_gui_button_id_t id);

/**************************************************************************//**
 * @brief Update GUI button widget
 * @details Wrapper function
 * @param[in] id Button ID
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_button_update(const sl_gui_button_id_t id);

/**************************************************************************//**
 * @brief Set GUI button label
 * @details Set the particular button label
 * @param[in] id Button ID
 * @param[in] label Label
 *****************************************************************************/
void sl_gui_button_set_label(const sl_gui_button_id_t id, const char *label);

/**************************************************************************//**
 * @brief Set callback for GUI button
 * @details Set evenet handler callback and arguments for the particular button
 * @param[in] id Button ID
 * @param[in] callback Event callback
 * @param[in] args Arguments
 *****************************************************************************/
void sl_gui_button_set_callback(const sl_gui_button_id_t id,
                                sl_widget_event_callback_t callback,
                                sl_widget_event_args_t args);

/**************************************************************************//**
 * @brief Call GUI button event callback
 * @details Wrapper function
 * @param[in] id Button ID
 *****************************************************************************/
void sl_gui_button_call(const sl_gui_button_id_t id);

/**************************************************************************//**
 * @brief Init GUI optionlist
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_optionlist_init(void);

/**************************************************************************//**
 * @brief Update GUI optionlist
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_optionlist_update(void);

/**************************************************************************//**
 * @brief Add item to GUI optionlist
 * @details Wrapper function
 * @param[in] label Label
 * @param[in] callback Callback
 * @param[in] args Arguments
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_optionlist_add_item(const char * label,
                                       sl_widget_event_callback_t callback,
                                       sl_widget_event_args_t args);

/**************************************************************************//**
 * @brief Scroll GUI optionlist down
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_optionlist_scroll_down(void);

/**************************************************************************//**
 * @brief Scroll GUI optionlist up
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_optionlist_scroll_up(void);

/**************************************************************************//**
 * @brief Assign event handler to button from selected optionlist item
 * @details Set event handler for a particular button from selected optionlist item
 * @param[in] id Button ID
 *****************************************************************************/
void sl_gui_optionlist_assign_event_hnd_to_btn(const sl_gui_button_id_t id);

/**************************************************************************//**
 * @brief Init GUI progressbar
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_progressbar_init(void);

/**************************************************************************//**
 * @brief Update GUI progressbar
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_progressbar_update(void);

/**************************************************************************//**
 * @brief Set GUI progressbar value
 * @details Wrapper function
 * @param[in] percentage Percentage value
 *****************************************************************************/
void sl_gui_progressbar_set_value(const uint8_t percentage);

/**************************************************************************//**
 * @brief Set GUI progressbar top label
 * @details Wrapper function
 * @param[in] label Label
 *****************************************************************************/
void sl_gui_progressbar_set_top_label(const char * label);

/**************************************************************************//**
 * @brief Set GUI progressbar bottom label
 * @details Wrapper function
 * @param label Label
 *****************************************************************************/
void sl_gui_progressbar_set_bottom_label(const char * label);

/**************************************************************************//**
 * @brief Init GUI textbox
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_textbox_init(void);

/**************************************************************************//**
 * @brief Update GUI textbox
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_textbox_update(void);

/**************************************************************************//**
 * @brief Set GUI textbox content
 * @details Wrapper function
 * @param[in] content Content string
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_textbox_set(const char * content);

/**************************************************************************//**
 * @brief Scroll GUI textbox down
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_textbox_scroll_down(void);

/**************************************************************************//**
 * @brief Scroll GUI textbox up
 * @details Wrapper function
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_gui_textbox_scroll_up(void);

/**************************************************************************//**
 * @brief Update all GUI widget
 * @details Update title, buttons, optionlist, progressbar and textbox in one step.
 *****************************************************************************/
static inline void sl_gui_update_all_widget(void)
{
  sl_gui_title_update();
  sl_gui_button_update(SL_GUI_BUTTON0);
  sl_gui_button_update(SL_GUI_BUTTON1);
  sl_gui_optionlist_update();
  sl_gui_progressbar_update();
  sl_gui_textbox_update();
}

#ifdef __cplusplus
}
#endif
#endif
