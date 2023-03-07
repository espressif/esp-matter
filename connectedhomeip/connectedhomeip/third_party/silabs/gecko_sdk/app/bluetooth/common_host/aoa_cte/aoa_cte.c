/***************************************************************************//**
 * @file
 * @brief CTE Handle module for AoA locator
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

#include <stdio.h>
#include <stdlib.h>
#include "aoa_cte.h"
#include "sl_common.h"
#include "aoa_cte_config.h"

// -----------------------------------------------------------------------------
// Module variables.

// Current CTE mode.
static aoa_cte_type_t cte_mode = AOA_CTE_DEFAULT_MODE;

// Common switch pattern for all CTE modes.
uint8_t cte_switch_pattern[ANTENNA_ARRAY_MAX_PIN_PATTERN_SIZE];
uint8_t cte_switch_pattern_size;

aoa_cte_config_t aoa_cte_config = {
  AOA_CTE_SAMPLING_INTERVAL,
  AOA_CTE_MIN_LENGTH,
  AOA_CTE_COUNT,
  AOA_CTE_SLOT_DURATION,
  NULL
};

// -----------------------------------------------------------------------------
// Public function definitions.

/**************************************************************************//**
 * Handles the bluetooth event.
 *****************************************************************************/
sl_status_t aoa_cte_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc = SL_STATUS_OK;

  if (SL_BT_MSG_ID(evt->header) == sl_bt_evt_system_boot_id) {
    // Get actual switch pattern at boot.
    cte_switch_pattern_size = sizeof(cte_switch_pattern);
    sc = antenna_array_get_pin_pattern(aoa_cte_config.antenna_array,
                                       cte_switch_pattern,
                                       &cte_switch_pattern_size);
    if (sc != SL_STATUS_OK) {
      return sc;
    }
  }

  switch (cte_mode) {
    case AOA_CTE_TYPE_SILABS:
      sc = cte_bt_on_event_silabs(evt);
      break;
    case AOA_CTE_TYPE_CONN_LESS:
      sc = cte_bt_on_event_conn_less(evt);
      break;
    case AOA_CTE_TYPE_CONN:
      sc = cte_bt_on_event_conn(evt);
      break;
    default:
      break;
  }

  return sc;
}

/**************************************************************************//**
 * Sets the CTE Mode.
 *****************************************************************************/
sl_status_t aoa_cte_set_mode(aoa_cte_type_t mode)
{
  if (mode >= AOA_CTE_TYPE_COUNT) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  cte_mode = mode;
  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Returns the CTE Mode.
 *****************************************************************************/
aoa_cte_type_t aoa_cte_get_mode(void)
{
  return cte_mode;
}

/**************************************************************************//**
 * Callback to notify the application on new iq report.
 *****************************************************************************/
SL_WEAK void aoa_cte_on_iq_report(aoa_db_entry_t *tag,
                                  aoa_iq_report_t *iq_report)
{
  // Implement in the application.
}
