/***************************************************************************//**
 * @file
 * @brief Wi-SUN Network measurement component
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
#include <string.h>
#include "sl_wisun_network_measurement.h"
#include "sl_wisun_network_measurement_gui.h"
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "sl_wisun_app_core.h"
#include "sl_wisun_ping.h"
#include "sl_wisun_ping_config.h"
#include "sl_gui.h"
#include "sl_display.h"
#include "cmsis_os2.h"
#include "socket.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Measurement packet counter structure type definition
typedef struct meas_packet_cnt {
  /// Counter
  uint16_t cnt;
  /// Maximum count of measurement
  uint16_t max_count;
} meas_packet_cnt_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Progressbar updater
 * @details Ping packet request/response sent handler
 * @param[in] req Request ping info (not used)
 * @param[in] resp Response ping info (not used)
 *****************************************************************************/
static void _progbar_updater(sl_wisun_ping_info_t *req, sl_wisun_ping_info_t *resp);

/**************************************************************************//**
 * @brief Interrupt Ping process
 * @details Function is executed in the GUI event thread
 * @param[in] args Arguments
 *****************************************************************************/
static void _interrupt_ping(void *args);

/**************************************************************************//**
 * @brief Is the node measurable
 * @details
 * @param[in] meas_type Measurement type
 * @param[in] node_type Node type
 * @return true Must be measured
 * @return false Not requested
 *****************************************************************************/
static inline bool _is_measurable(const sl_wisun_nwm_target_type_t meas_type,
                                  const sl_wisun_nwm_node_type_t node_type);

/**************************************************************************//**
 * @brief Init measurable elements
 * @details Set requested flag if it's necessary
 * @param[in] meas_type Measurement type
 * @param[in] node_count Available node count
 * @return uint8_t Requested measurement count
 *****************************************************************************/
static uint8_t _init_meas_by_setting_type(const sl_wisun_nwm_target_type_t meas_type,
                                          const uint8_t node_count);

// -----------------------------------------------------------------------------
//                          Static Variable Declarations
// -----------------------------------------------------------------------------

/// Measurable router storage
static sl_wisun_nwm_measurable_node_t _meas_nodes[SL_WISUN_MAX_NODE_COUNT];

/// Measurement packet counter
static meas_packet_cnt_t _meas_packet_cnt = { 0 };

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* Network quick measurement */
void sl_wisun_nwm_quick_measure(const sl_wisun_nwm_target_type_t meas_type,
                                const uint16_t meas_count,
                                const uint16_t meas_packet_length)
{
  uint8_t node_count = 0;
  uint8_t measurable_count = 0;
  char str_buff[64U] = { 0 };

  node_count =  sl_wisun_nwm_get_nodes(_meas_nodes, SL_WISUN_MAX_NODE_COUNT);
  measurable_count = _init_meas_by_setting_type(meas_type, node_count);

  _meas_packet_cnt.max_count = measurable_count * meas_count;
  _meas_packet_cnt.cnt = 0;

  snprintf(str_buff, 64U, "Measure %u Nodes", measurable_count);

  sl_gui_init_all_widget();
  sl_gui_title_set_label(str_buff);
  sl_gui_title_update();

  sl_gui_progressbar_set_value(0);
  sl_gui_progressbar_update();
  sl_display_update();

  sl_gui_button_set_label(SL_GUI_BUTTON1, "Stop");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, _interrupt_ping, NULL);
  sl_gui_button_update(SL_GUI_BUTTON1);

  for (uint8_t i = 0; i < node_count; ++i) {
    if (!_meas_nodes[i].is_requested) {
      continue;
    }
    printf("%s[%s]\n",
           _meas_nodes[i].name,
           app_wisun_trace_util_get_ip_address_str(&_meas_nodes[i].addr.sin6_addr));
    sl_gui_progressbar_set_top_label(_meas_nodes[i].name);
    sl_gui_progressbar_set_bottom_label(meas_type == SL_WISUN_NWM_TARGET_TYPE_ALL && i < (node_count - 1)
                                        ? _meas_nodes[i + 1].name : NULL);
    sl_gui_progressbar_update();
    sl_display_update();
    // measure (secondary parent should not be measured)
    if (_meas_nodes[i].type != SL_WISUN_NWM_NODE_TYPE_SECONDARY_PARENT) {
      sl_wisun_nwm_measure(&_meas_nodes[i].addr, meas_count, meas_packet_length, true);
    }
  }

  sl_gui_progressbar_set_value(100);
  sl_gui_progressbar_update();
  sl_display_update();

  sl_gui_progressbar_set_top_label("Done");
  sl_gui_progressbar_set_bottom_label(NULL);
  sl_gui_progressbar_update();

  sl_gui_button_set_label(SL_GUI_BUTTON1, "Back");
  sl_gui_button_set_callback(SL_GUI_BUTTON1, sl_wisun_nwm_main_form, NULL);
  sl_gui_button_set_label(SL_GUI_BUTTON0, "Results");
  sl_gui_button_set_callback(SL_GUI_BUTTON0, sl_wisun_nwm_test_result_form, NULL);
  sl_gui_button_update(SL_GUI_BUTTON1);
  sl_gui_button_update(SL_GUI_BUTTON0);
  sl_display_update();
}

/* Network quick measurement */
void sl_wisun_nwm_measure(const wisun_addr_t * const remote_address,
                          const uint16_t meas_count,
                          const uint16_t meas_packet_length,
                          const bool update_gui)
{
  (void) sl_wisun_ping(remote_address, meas_count, meas_packet_length,
                       sl_wisun_nwm_stat_handler,
                       update_gui ? _progbar_updater : NULL);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void _progbar_updater(sl_wisun_ping_info_t *req, sl_wisun_ping_info_t *resp)
{
  (void) req;
  (void) resp;

  assert(_meas_packet_cnt.max_count);

  ++_meas_packet_cnt.cnt;

  sl_gui_progressbar_set_value((_meas_packet_cnt.cnt * 100) / _meas_packet_cnt.max_count);
  sl_gui_progressbar_update();
  sl_display_update();
}

static void _interrupt_ping(void *args)
{
  (void) args;
  sl_wisun_ping_stop();
  sl_display_renderer(sl_wisun_nwm_main_form, NULL, 0);
}

static inline bool _is_measurable(const sl_wisun_nwm_target_type_t meas_type,
                                  const sl_wisun_nwm_node_type_t node_type)
{
  if (meas_type == SL_WISUN_NWM_TARGET_TYPE_ALL ||

      (meas_type == SL_WISUN_NWM_TARGET_TYPE_BORDER_ROUTER
       && node_type == SL_WISUN_NWM_NODE_TYPE_BORDER_ROUTER) ||

      (meas_type == SL_WISUN_NWM_TARGET_TYPE_PARENT
       && node_type == SL_WISUN_NWM_NODE_TYPE_PRIMARY_PARENT)) {
    return true;
  } else {
    return false;
  }
}

static uint8_t _init_meas_by_setting_type(const sl_wisun_nwm_target_type_t meas_type,
                                          const uint8_t node_count)
{
  uint8_t retval = 0;
  assert(node_count <= SL_WISUN_MAX_NODE_COUNT);
  for (uint8_t i = 0; i < node_count; ++i) {
    if (_is_measurable(meas_type, _meas_nodes[i].type)) {
      _meas_nodes[i].is_requested = true;
      ++retval;
    } else {
      _meas_nodes[i].is_requested = false;
    }
  }
  return retval;
}
