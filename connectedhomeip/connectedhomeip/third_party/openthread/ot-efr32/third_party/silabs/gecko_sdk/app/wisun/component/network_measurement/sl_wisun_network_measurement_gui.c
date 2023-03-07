/***************************************************************************//**
 * @file
 * @brief
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
#include <assert.h>
#include "sl_component_catalog.h"
#include "sl_wisun_util.h"
#include "sl_wisun_app_core.h"
#include "sl_wisun_app_setting.h"
#include "sl_wisun_network_measurement_gui.h"
#include "sl_wisun_network_measurement.h"
#include "sl_wisun_network_measurement_config.h"
#include "sl_display.h"
#include "sl_gui.h"
#include "socket.h"
#include "sl_wisun_ping.h"
#include "silabs_wisun_logo.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Internal string buffer size
#define STR_BUFF_SIZE             (512U)

/// Max Packet count option size
#define MAX_PACKET_COUNT_OPTION   (7U)

/// Max Packet length option size
#define MAX_PACKET_LENGTH_OPTION  (8U)

/// Max Measurement type option size
#define MAX_MEAS_TYPE_OPTION      (3U)

/// Max regulatory domain count
#define MAX_REG_DOMAIN_COUNT      (16U)

/// Max networ size count
#define MAX_NETWORK_SIZE_COUNT    (6U)

/// Print Statistic format string
#define PRINT_FULL_STAT_FORMAT_STR \
  "[%s]\n\
Packets:       %u\n\
Packet length: %u\n\
Lost packets:  %u\n\
Packet loss:   %u%%\n\
Min. Time[ms]: %lu\n\
Max. Time[ms]: %lu\n\
Avg. Time[ms]: %lu\n\
lifetime:      %lu\n\
mac_tx_count:  %lu\n\
mac_tx_fail:   %lu\n\
mac_tx_ms_cnt: %lu\n\
mac_tx_ms_fail:%lu\n\
rpl_rank:      %u\n\
etx:           %u\n\
rsl_out:       %u\n\
rsl_in:        %u\n"

/// Print neighbor statistic format string
#define PRINT_NEIGHBOR_STAT_FORMAT_STR \
  "[%s]\n\
lifetime:      %lu\n\
mac_tx_count:  %lu\n\
mac_tx_fail:   %lu\n\
mac_tx_ms_cnt: %lu\n\
mac_tx_ms_fail:%lu\n\
rpl_rank:      %u\n\
etx:           %u\n\
rsl_out:       %u\n\
rsl_in:        %u\n"

/// Print ping statistic format string
#define PRINT_PING_STAT_FORMAT_STR \
  "[%s]\n\
Packets:       %u\n\
Packet length: %u\n\
Lost packets:  %u\n\
Packet loss:   %u%%\n\
Min. Time[ms]: %lu\n\
Max. Time[ms]: %lu\n\
Avg. Time[ms]: %lu\n"

/// Print Node Info format string
#define PRINT_NODE_INFO_FORMAT_STR \
  "Network Name:\n%s\n\
Nw. size: %s(%d)\n\
TX Power: %d\n\
Reg. domain: %s(%d)\n\
Op. class: %d\n\
Op. mode: 0x%x\n\
Global:\n[%s]\n\
Border Router:\n[%s]\n\
Primary Parent:\n[%s]\n\
Secondary Parent:\n[%s]\n\
TX budget: %lums\n%s\n"

/// Network Measurement settings structure
typedef struct sl_wisun_nwm_setting {
  /// Packet count
  uint16_t packet_count;
  /// Packet length
  uint16_t packet_length;
  /// Measurement type
  sl_wisun_nwm_target_type_t meas_type;
} sl_wisun_nwm_setting_t;

/// Node info structure type definition
typedef struct node_info {
  /// Addresses
  current_addr_t addresses;
  /// Settings
  app_setting_wisun_t settings;
} node_info_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Scroll optionlist and set selected
 * @details Callback function
 * @param[in] args Arguments
 *****************************************************************************/
static void _scroll_optionlist_and_set_select(void *args);

/**************************************************************************//**
 * @brief Scroll Text box
 * @details Callback function
 * @param[in] args Arguments
 *****************************************************************************/
static void _scroll_txt_box(void *args);

/**************************************************************************//**
 * @brief Node info form
 * @details Form renderer
 * @param[in] args Arguments
 *****************************************************************************/
static void _node_info_form(void *args);

/**************************************************************************//**
 * @brief Neighbor info form
 * @details Form renderer
 * @param[in] args Arguments
 *****************************************************************************/
static void _neighbors_info_form(void *args);

/**************************************************************************//**
 * @brief Settings form
 * @details Form renderer
 * @param[in] args Arguments
 *****************************************************************************/
static void _settings_form(void *args);

/**************************************************************************//**
 * @brief Packet count form
 * @details Form renderer
 * @param[in] args Arguments
 *****************************************************************************/
static void _packet_count_form(void *args);

/**************************************************************************//**
 * @brief Packet length form
 * @details Form renderer
 * @param[in] args Arguments
 *****************************************************************************/
static void _packet_length_form(void *args);

/**************************************************************************//**
 * @brief Measurement type form
 * @details Form renderer
 * @param[in] args Arguments
 *****************************************************************************/
static void _target_type_form(void *args);

/**************************************************************************//**
 * @brief Set packet count
 * @details Callback function
 * @param[in] args Arguments
 *****************************************************************************/
static void _set_packet_count(void *args);

/**************************************************************************//**
 * @brief Set packet length
 * @details Callback function
 * @param[in] args Arguments
 *****************************************************************************/
static void _set_packet_length(void *args);

/**************************************************************************//**
 * @brief Set measurement type
 * @details Callback function
 * @param[in] args Arguments
 *****************************************************************************/
static void _set_meas_type(void *args);

/**************************************************************************//**
 * @brief Measurement type to string converter
 * @details Helper function
 * @param meas_type
 * @return const char* String format of measurement type
 *****************************************************************************/
static const char * _meas_type_to_str(sl_wisun_nwm_target_type_t meas_type);

/**************************************************************************//**
 * @brief Add packet length option
 * @details Helper function
 * @param[in] opt Option item
 *****************************************************************************/
static inline void _add_packet_length_opt(const app_enum_t *opt);

/**************************************************************************//**
 * @brief Add packet count option
 * @details Helper function
 * @param[in] opt Option item
 *****************************************************************************/
static inline void _add_packet_count_opt(const app_enum_t *opt);

/**************************************************************************//**
 * @brief Add measurement type option
 * @details Helper function
 * @param[in] opt Option item
 *****************************************************************************/
static inline void _add_meas_type_opt(const app_enum_t *opt);

/**************************************************************************//**
 * @brief Start test form
 * @details Form renderer
 * @param[in] args Arguments
 *****************************************************************************/
static void _start_test_form(void *args);

/**************************************************************************//**
 * @brief Renderer test forrm
 * @details Helper function to execute in the display renderer thread
 *          instead of GUI event thread
 * @param[in] args Arguments
 *****************************************************************************/
static void _renderer_test_form(void *args);

/**************************************************************************//**
 * @brief Calculate packet loss percentage value
 * @details Helper function
 * @param[in] stat
 * @return uint8_t packet loss in % value
 *****************************************************************************/
static inline uint8_t _calc_packet_loss_percent(const sl_wisun_nwm_node_stat_t *stat);

/**************************************************************************//**
 * @brief Set reult textbox
 * @details Form renderer
 * @param[in] args Arguments
 *****************************************************************************/
static void _set_test_result_txtbox(void *args);

/**************************************************************************//**
 * @brief Set neighbor info textbox
 * @details Form renderer
 * @param[in] args Arguments
 *****************************************************************************/
static void _set_nbinfo_txtbox(void *args);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// String internal buffer allocation
static char _str_buff[STR_BUFF_SIZE] = { 0 };

/// Border Router temp statistic storage
static sl_wisun_nwm_node_stat_t _border_router_stat;

/// Primary Parent temp statistic storage
static sl_wisun_nwm_node_stat_t _primary_parent_stat;

/// Secondary Parent temp statistic storage
static sl_wisun_nwm_node_stat_t _secondary_parent_stat;

/// Children temp statistic storage
static sl_wisun_nwm_node_stat_t _children_stat[SL_WISUN_MAX_CHILDREN_COUNT];

/// Measurement setting allocation
static sl_wisun_nwm_setting_t _meas_settings = {
  .packet_count = SL_WISUN_NETWORK_MEASUREMENT_DEFAULT_PACKET_COUNT,
  .packet_length = SL_WISUN_NETWORK_MEASUREMENT_DEFAULT_PACKET_LENGTH,
  .meas_type = SL_WISUN_NWM_TARGET_TYPE_ALL
};

/// Packet count options
static const app_enum_t _packet_count_options[MAX_PACKET_COUNT_OPTION] = {
  { .value_str = "1", .value = 1U },
  { .value_str = "10", .value = 10U },
  { .value_str = "50", .value = 50U },
  { .value_str = "100", .value = 100U },
  { .value_str = "500", .value = 500U },
  { .value_str = "1000", .value = 1000U },
  { .value_str = "5000", .value = 5000U }
};

/// Packet length options
static const app_enum_t _packet_length_options[MAX_PACKET_LENGTH_OPTION] = {
  { .value_str = "9", .value = 9U }, // Minimum packet length by ping component
  { .value_str = "16", .value = 16U },
  { .value_str = "40", .value = 40U },
  { .value_str = "64", .value = 64U },
  { .value_str = "128", .value = 128U },
  { .value_str = "256", .value = 256U },
  { .value_str = "512", .value = 512U },
  { .value_str = "1024", .value = 1024U }
};

/// Measurement type options
static const app_enum_t _meas_type_options[MAX_MEAS_TYPE_OPTION] = {
  { .value_str = "BR/Parent/Children", .value = SL_WISUN_NWM_TARGET_TYPE_ALL },
  { .value_str = "Border Router", .value = SL_WISUN_NWM_TARGET_TYPE_BORDER_ROUTER },
  { .value_str = "Parent", .value = SL_WISUN_NWM_TARGET_TYPE_PARENT }
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void sl_wisun_nwm_test_result_form(void *args)
{
  bool parent_available = false;
  uint8_t children_count = 0;
  (void) args;
  sl_gui_init_all_widget();

  sl_gui_title_set_label("Test result");
  sl_gui_title_update();

  sl_wisun_nwm_get_border_router_stat(&_border_router_stat);
  parent_available = sl_wisun_nwm_get_primary_parent_stat(&_primary_parent_stat);
  sl_wisun_nwm_get_children_stat(_children_stat,
                                 sizeof(_children_stat) / sizeof(sl_wisun_nwm_node_stat_t),
                                 &children_count);

  sl_gui_optionlist_add_item("Back to Main Menu", sl_wisun_nwm_main_form, NULL);
  // check whether BR meas is available
  if (_border_router_stat.name != NULL
      && strncmp(_border_router_stat.name, SL_WIDGET_EMPTY_STR, SL_WIDGET_MAX_LABEL_STR_LEN)) {
    sl_gui_optionlist_add_item(_border_router_stat.name,
                               _set_test_result_txtbox,
                               (void *)&_border_router_stat);
  }

  if (parent_available) {
    sl_gui_optionlist_add_item(_primary_parent_stat.name,
                               _set_test_result_txtbox,
                               (void *)&_primary_parent_stat);
  }

  if (children_count) {
    for (uint8_t i = 0; i < children_count; ++i) {
      sl_gui_optionlist_add_item(_children_stat[i].name,
                                 _set_test_result_txtbox,
                                 (void *)&_children_stat[i]);
    }
  }

  sl_gui_optionlist_assign_event_hnd_to_btn(SL_GUI_BUTTON0);
  sl_gui_optionlist_update();
  sl_gui_button_set_label(SL_GUI_BUTTON1, "Down");
  sl_gui_button_set_label(SL_GUI_BUTTON0, "Select");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, _scroll_optionlist_and_set_select, NULL);
  sl_gui_button_update(SL_GUI_BUTTON0);
  sl_gui_button_update(SL_GUI_BUTTON1);

  sl_display_update();
}

void sl_wisun_nwm_logo_form(void *args)
{
  sl_wisun_nwm_logo_form_args_t *logo_form_args = (sl_wisun_nwm_logo_form_args_t *)args;
  static char buff[4U] = { 0 };

  sl_display_draw_bitmap(0, 0,
                         SILABS_WISUN_LOGO_WIDTH,
                         SILABS_WISUN_LOGO_HEIGHT,
                         silabs_wisun_logo_bits);
  sl_display_draw_string_on_line(logo_form_args->network_name, 2, GLIB_ALIGN_CENTER, 0, 0, false);
  sl_display_draw_string_on_line(logo_form_args->join_state_str, 10, GLIB_ALIGN_CENTER, 0, 0, false);
  snprintf(buff, 4U, "(%ld)", logo_form_args->join_state);
  sl_display_draw_string_on_line(buff, 11, GLIB_ALIGN_CENTER, 0, 0, false);
}

void sl_wisun_nwm_main_form(void *args)
{
  (void) args;
  sl_gui_init_all_widget();

  sl_gui_title_set_label("Network Measurement");
  sl_gui_title_update();
  sl_gui_optionlist_add_item("Node info", _node_info_form, NULL);
  sl_gui_optionlist_add_item("Start test", _renderer_test_form, NULL);
  sl_gui_optionlist_add_item("Test results", sl_wisun_nwm_test_result_form, NULL);
  sl_gui_optionlist_add_item("Neighbors info", _neighbors_info_form, NULL);
  sl_gui_optionlist_add_item("Settings", _settings_form, NULL);
  sl_gui_optionlist_assign_event_hnd_to_btn(SL_GUI_BUTTON0);
  sl_gui_optionlist_update();
  sl_gui_button_set_label(SL_GUI_BUTTON1, "Down");
  sl_gui_button_set_label(SL_GUI_BUTTON0, "Select");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, _scroll_optionlist_and_set_select, NULL);
  sl_gui_button_update(SL_GUI_BUTTON0);
  sl_gui_button_update(SL_GUI_BUTTON1);
  sl_display_update();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void _scroll_optionlist_and_set_select(void *args)
{
  (void) args;
  sl_gui_optionlist_scroll_down();
  sl_gui_optionlist_update();
  sl_gui_optionlist_assign_event_hnd_to_btn(SL_GUI_BUTTON0);
  sl_display_update();
}

static void _scroll_txt_box(void *args)
{
  (void) args;
  sl_gui_textbox_scroll_down();
  sl_gui_textbox_update();
  sl_display_update();
}

static void _node_info_form(void *args)
{
  node_info_t node_info = { 0 };
  (void) args;

  // Hold the remaning TX budget if applicable (regulation is active or value is valid)
  uint32_t tx_remaining_budget = 0UL;
  // Indicates if the remaining budget returned is a usable value.
  bool valid = false;

  sl_gui_title_set_label("Node Info");

  sl_gui_title_update();
  sl_gui_button_set_label(SL_GUI_BUTTON0, "Back");
  sl_gui_button_set_callback(SL_GUI_BUTTON0, sl_wisun_nwm_main_form, NULL);
  sl_gui_button_set_label(SL_GUI_BUTTON1, "Down");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, _scroll_txt_box, NULL);

  sl_gui_button_update(SL_GUI_BUTTON1);
  sl_gui_button_update(SL_GUI_BUTTON0);

  // getting IP addresses
  app_wisun_get_current_addresses(&node_info.addresses);

  // getting settings
  app_wisun_setting_get(&node_info.settings);

  // get remanining transmission budget if applicable
  if ( app_wisun_get_regulation_active() == true ) {
    valid = app_wisun_get_remaining_tx_budget(&tx_remaining_budget);
  }

  snprintf(_str_buff, STR_BUFF_SIZE,
           PRINT_NODE_INFO_FORMAT_STR,
           node_info.settings.network_name,
           app_wisun_trace_util_nw_size_to_str(node_info.settings.network_size),
           node_info.settings.network_size,
           node_info.settings.tx_power,
           app_wisun_trace_util_reg_domain_to_str(node_info.settings.phy.regulatory_domain), node_info.settings.phy.regulatory_domain,
           node_info.settings.phy.operating_class,
           node_info.settings.phy.operating_mode,
           app_wisun_trace_util_get_ip_address_str(&node_info.addresses.global),
           app_wisun_trace_util_get_ip_address_str(&node_info.addresses.border_router),
           app_wisun_trace_util_get_ip_address_str(&node_info.addresses.primary_parent),
           app_wisun_trace_util_get_ip_address_str(&node_info.addresses.secondary_parent),
           (valid ? tx_remaining_budget : 0UL),
           (valid ? ((tx_remaining_budget == 0UL) ? "[Exceeded]" : "[Remaining]") : "[Not available]"));
  sl_gui_textbox_set(_str_buff);
  sl_gui_textbox_update();
  sl_display_update();
}

static void _renderer_test_form(void *args)
{
  (void) args;
  sl_display_renderer(_start_test_form, NULL, 0);
}

static void _start_test_form(void *args)
{
  (void) args;
  sl_wisun_nwm_quick_measure(_meas_settings.meas_type, _meas_settings.packet_count, _meas_settings.packet_length);
}

static void _settings_form(void *args)
{
  (void) args;
  sl_gui_init_all_widget();

  sl_gui_title_set_label("Settings");
  sl_gui_title_update();

  sl_gui_optionlist_add_item("Back to Main Menu", sl_wisun_nwm_main_form, NULL);
  sl_gui_optionlist_add_item("Packet count", _packet_count_form, NULL);
  sl_gui_optionlist_add_item("Packet length", _packet_length_form, NULL);
  sl_gui_optionlist_add_item("Target devices", _target_type_form, NULL);
  sl_gui_optionlist_assign_event_hnd_to_btn(SL_GUI_BUTTON0);
  sl_gui_optionlist_update();
  sl_gui_button_set_label(SL_GUI_BUTTON1, "Down");
  sl_gui_button_set_label(SL_GUI_BUTTON0, "Select");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, _scroll_optionlist_and_set_select, NULL);
  sl_gui_button_update(SL_GUI_BUTTON0);
  sl_gui_button_update(SL_GUI_BUTTON1);

  sl_display_update();
}

static void _set_packet_count(void *args)
{
  app_enum_t *val = (void *) args;
  _meas_settings.packet_count = val->value;
  sl_display_renderer(_settings_form, NULL, 0);
}

static void _set_meas_type(void *args)
{
  app_enum_t *val = (void *) args;
  _meas_settings.meas_type = (sl_wisun_nwm_target_type_t) val->value;
  sl_display_renderer(_settings_form, NULL, 0);
}

static void _set_packet_length(void *args)
{
  app_enum_t *val = (void *) args;
  _meas_settings.packet_length = val->value;
  sl_display_renderer(_settings_form, NULL, 0);
}

static void _packet_count_form(void *args)
{
  (void) args;
  sl_gui_init_all_widget();

  snprintf(_str_buff, STR_BUFF_SIZE, "Packet cnt: %d", _meas_settings.packet_count);
  sl_gui_title_set_label(_str_buff);
  sl_gui_title_update();

  sl_gui_optionlist_add_item("Back to Settings", _settings_form, NULL);

  for (uint32_t i = 0; i < MAX_PACKET_COUNT_OPTION; ++i) {
    _add_packet_count_opt(&_packet_count_options[i]);
  }
  sl_gui_optionlist_assign_event_hnd_to_btn(SL_GUI_BUTTON0);
  sl_gui_optionlist_update();
  sl_gui_button_set_label(SL_GUI_BUTTON1, "Down");
  sl_gui_button_set_label(SL_GUI_BUTTON0, "Select");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, _scroll_optionlist_and_set_select, NULL);
  sl_gui_button_update(SL_GUI_BUTTON0);
  sl_gui_button_update(SL_GUI_BUTTON1);

  sl_display_update();
}

static inline void _add_packet_count_opt(const app_enum_t *opt)
{
  sl_gui_optionlist_add_item(opt->value_str, _set_packet_count, (void *)opt);
}

static inline void _add_meas_type_opt(const app_enum_t *opt)
{
  sl_gui_optionlist_add_item(opt->value_str, _set_meas_type, (void *)opt);
}

static inline void _add_packet_length_opt(const app_enum_t *opt)
{
  if (opt->value < SL_WISUN_PING_MAX_PACKET_LENGTH) {
    sl_gui_optionlist_add_item(opt->value_str, _set_packet_length, (void *)opt);
  }
}

static void _packet_length_form(void *args)
{
  (void) args;
  sl_gui_init_all_widget();

  snprintf(_str_buff, STR_BUFF_SIZE, "Packet len: %d", _meas_settings.packet_length);
  sl_gui_title_set_label(_str_buff);
  sl_gui_title_update();

  sl_gui_optionlist_add_item("Back to Settings", _settings_form, NULL);

  for (uint32_t i = 0; i < MAX_PACKET_LENGTH_OPTION; ++i) {
    _add_packet_length_opt(&_packet_length_options[i]);
  }

  sl_gui_optionlist_assign_event_hnd_to_btn(SL_GUI_BUTTON0);
  sl_gui_optionlist_update();
  sl_gui_button_set_label(SL_GUI_BUTTON1, "Down");
  sl_gui_button_set_label(SL_GUI_BUTTON0, "Select");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, _scroll_optionlist_and_set_select, NULL);
  sl_gui_button_update(SL_GUI_BUTTON0);
  sl_gui_button_update(SL_GUI_BUTTON1);

  sl_display_update();
}

static const char * _meas_type_to_str(sl_wisun_nwm_target_type_t meas_type)
{
  for (uint8_t i = 0; i < MAX_MEAS_TYPE_OPTION; ++i) {
    if (_meas_type_options[i].value == meas_type) {
      return _meas_type_options[i].value_str;
    }
  }
  return "Undefined";
}

static void _target_type_form(void *args)
{
  (void) args;
  sl_gui_init_all_widget();

  snprintf(_str_buff, STR_BUFF_SIZE, "%s",
           _meas_type_to_str(_meas_settings.meas_type));
  sl_gui_title_set_label(_str_buff);
  sl_gui_title_update();

  sl_gui_optionlist_add_item("Back to Settings", _settings_form, NULL);

  for (uint32_t i = 0; i < MAX_MEAS_TYPE_OPTION; ++i) {
    _add_meas_type_opt(&_meas_type_options[i]);
  }

  sl_gui_optionlist_assign_event_hnd_to_btn(SL_GUI_BUTTON0);
  sl_gui_optionlist_update();
  sl_gui_button_set_label(SL_GUI_BUTTON1, "Down");
  sl_gui_button_set_label(SL_GUI_BUTTON0, "Select");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, _scroll_optionlist_and_set_select, NULL);
  sl_gui_button_update(SL_GUI_BUTTON0);
  sl_gui_button_update(SL_GUI_BUTTON1);

  sl_display_update();
}
static inline uint8_t _calc_packet_loss_percent(const sl_wisun_nwm_node_stat_t *stat)
{
  return stat->ping_stat.packet_count ? (stat->ping_stat.lost * 100) / stat->ping_stat.packet_count : 0xFF;
}

static void _set_test_result_txtbox(void *args)
{
  sl_wisun_nwm_node_stat_t *stat = (sl_wisun_nwm_node_stat_t *) args;
  sl_gui_title_set_label(stat->name);
  sl_gui_title_update();

  sl_gui_textbox_init();
  if (stat->type == SL_WISUN_NWM_NODE_TYPE_BORDER_ROUTER) {
    snprintf(_str_buff, STR_BUFF_SIZE, PRINT_PING_STAT_FORMAT_STR,
             app_wisun_trace_util_get_ip_address_str(&stat->addr.sin6_addr),
             stat->ping_stat.packet_count,
             stat->ping_stat.packet_length,
             stat->ping_stat.lost,
             _calc_packet_loss_percent(stat),
             stat->ping_stat.min_time_ms,
             stat->ping_stat.max_time_ms,
             stat->ping_stat.avg_time_ms);
  } else {
    snprintf(_str_buff, STR_BUFF_SIZE, PRINT_FULL_STAT_FORMAT_STR,
             app_wisun_trace_util_get_ip_address_str(&stat->addr.sin6_addr),
             stat->ping_stat.packet_count,
             stat->ping_stat.packet_length,
             stat->ping_stat.lost,
             _calc_packet_loss_percent(stat),
             stat->ping_stat.min_time_ms,
             stat->ping_stat.max_time_ms,
             stat->ping_stat.avg_time_ms,
             stat->stat.lifetime,
             stat->stat.mac_tx_count,
             stat->stat.mac_tx_failed_count,
             stat->stat.mac_tx_ms_count,
             stat->stat.mac_tx_ms_failed_count,
             stat->stat.rpl_rank,
             stat->stat.etx,
             stat->stat.rsl_out,
             stat->stat.rsl_in);
  }

  sl_gui_textbox_set(_str_buff);
  sl_gui_textbox_update();

  sl_gui_button_set_label(SL_GUI_BUTTON0, "Back");
  sl_gui_button_set_callback(SL_GUI_BUTTON0, sl_wisun_nwm_test_result_form, NULL);
  sl_gui_button_set_label(SL_GUI_BUTTON1, "Down");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, _scroll_txt_box, NULL);

  sl_gui_button_update(SL_GUI_BUTTON1);
  sl_gui_button_update(SL_GUI_BUTTON0);

  sl_display_update();
}

static void _set_nbinfo_txtbox(void *args)
{
  sl_wisun_nwm_node_stat_t *stat = (sl_wisun_nwm_node_stat_t *) args;
  sl_gui_title_set_label(stat->name);
  sl_gui_title_update();

  sl_gui_textbox_init();

  snprintf(_str_buff, STR_BUFF_SIZE, PRINT_NEIGHBOR_STAT_FORMAT_STR,
           app_wisun_trace_util_get_ip_address_str(&stat->addr.sin6_addr),
           stat->stat.lifetime,
           stat->stat.mac_tx_count,
           stat->stat.mac_tx_failed_count,
           stat->stat.mac_tx_ms_count,
           stat->stat.mac_tx_ms_failed_count,
           stat->stat.rpl_rank,
           stat->stat.etx,
           stat->stat.rsl_out,
           stat->stat.rsl_in);

  sl_gui_textbox_set(_str_buff);
  sl_gui_textbox_update();

  sl_gui_button_set_label(SL_GUI_BUTTON0, "Back");
  sl_gui_button_set_callback(SL_GUI_BUTTON0, _neighbors_info_form, NULL);
  sl_gui_button_set_label(SL_GUI_BUTTON1, "Down");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, _scroll_txt_box, NULL);

  sl_gui_button_update(SL_GUI_BUTTON1);
  sl_gui_button_update(SL_GUI_BUTTON0);

  sl_display_update();
}

static void _neighbors_info_form(void *args)
{
  bool parent_available = false;
  uint8_t children_count = 0;

  (void) args;
  sl_gui_init_all_widget();

  sl_gui_title_set_label("Neighbors Info");
  sl_gui_title_update();

  sl_wisun_nwm_get_nodes(NULL, 0);

  parent_available = sl_wisun_nwm_get_primary_parent_stat(&_primary_parent_stat);
  sl_wisun_nwm_get_children_stat(_children_stat,
                                 sizeof(_children_stat) / sizeof(sl_wisun_nwm_node_stat_t),
                                 &children_count);

  sl_gui_optionlist_add_item("Back to Main Menu", sl_wisun_nwm_main_form, NULL);

  if (parent_available) {
    sl_gui_optionlist_add_item("Primary Parent",
                               _set_nbinfo_txtbox,
                               (void *)&_primary_parent_stat);
  }

  parent_available = sl_wisun_nwm_get_secondary_parent_stat(&_secondary_parent_stat);

  if (parent_available) {
    sl_gui_optionlist_add_item("Secondary Parent",
                               _set_nbinfo_txtbox,
                               (void *)&_secondary_parent_stat);
  }

  if (children_count) {
    for (uint8_t i = 0; i < children_count; ++i) {
      sl_gui_optionlist_add_item(_children_stat[i].name,
                                 _set_nbinfo_txtbox,
                                 (void *)&_children_stat[i]);
    }
  }

  sl_gui_optionlist_assign_event_hnd_to_btn(SL_GUI_BUTTON0);
  sl_gui_optionlist_update();
  sl_gui_button_set_label(SL_GUI_BUTTON1, "Down");
  sl_gui_button_set_label(SL_GUI_BUTTON0, "Select");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, _scroll_optionlist_and_set_select, NULL);
  sl_gui_button_update(SL_GUI_BUTTON0);
  sl_gui_button_update(SL_GUI_BUTTON1);

  sl_display_update();
}
