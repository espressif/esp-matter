/***************************************************************************//**
 * @file
 * @brief Application custom callback implementations
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
#include "sl_status.h"
#include "sl_wisun_types.h"
#include "app_custom_callback.h"
#include "sl_wisun_app_core.h"
#include "sl_wisun_app_setting.h"
#include "sl_wisun_network_measurement_gui.h"
#include "sl_display.h"
#include "sl_gui.h"
#include "sl_wisun_cli_util.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

static const app_enum_t app_regulation_tx_level_enum[] =
{
  { "low", SL_WISUN_REGULATION_TX_LEVEL_LOW },
  { "warning", SL_WISUN_REGULATION_TX_LEVEL_WARNING },
  { "alert", SL_WISUN_REGULATION_TX_LEVEL_ALERT },
  { NULL, 0 }
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* Wi-SUN connected custom callback */
void app_custom_connected_callback(sl_wisun_evt_t *evt)
{
  (void) evt;
}

/* Socket data custom callback */
void app_custom_socket_data_callback(sl_wisun_evt_t *evt)
{
  (void) evt;
}

/* Socket data sent custom callback */
void app_custom_socket_data_sent_callback(sl_wisun_evt_t *evt)
{
  (void) evt;
}

void app_custom_join_state_callback(sl_wisun_evt_t *evt)
{
  static char nw_name[MAX_SIZE_OF_NETWORK_NAME] = { 0 };
  static char join_state_str[20U] = { 0 };
  static sl_wisun_nwm_logo_form_args_t args = { 0 };
  sl_status_t stat = SL_STATUS_FAIL;

  args.join_state = evt->evt.join_state.join_state;
  args.join_state_str = join_state_str;
  args.network_name = nw_name;

  if (evt->evt.join_state.join_state == SL_WISUN_JOIN_STATE_SELECT_PAN) {
    stat = app_wisun_setting_get_network_name(nw_name, MAX_SIZE_OF_NETWORK_NAME);
    if (stat != SL_STATUS_OK) {
      snprintf(nw_name, MAX_SIZE_OF_NETWORK_NAME, "Unknow Network");
    }
  }

  // trimming join state long strings
  snprintf(join_state_str, 20U, "%s", app_wisun_trace_util_conn_state_to_str(evt->evt.join_state.join_state));

  sl_display_renderer(sl_wisun_nwm_logo_form, (void *)&args, 0);
}

void app_custom_regulation_tx_level_callback(sl_wisun_evt_t *evt)
{
  const app_enum_t *ptr;

  ptr = app_util_get_enum_by_integer(app_regulation_tx_level_enum,
                                     evt->evt.regulation_tx_level.tx_level);
  if (ptr) {
    printf("[Regulation TX level: %s (%lu) (%lu ms)]\r\n", ptr->value_str, ptr->value, evt->evt.regulation_tx_level.tx_duration_ms);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
