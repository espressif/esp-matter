/***************************************************************************//**
 * @file
 * @brief sli_rail_util_callbacks.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "rail.h"
#include "rail_ble.h" // for RAIL_BLE_State_t
#include "sli_rail_util_callbacks.h" // for internal-only callback signatures
#include "sl_flex_util_ble_init.h"
#include "sl_flex_util_ble_init_config.h"
#include "sl_flex_util_ble_protocol_config.h"
#include "sl_flex_util_ble_protocol.h"
#include "app_assert.h"
#include "app_log.h"

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

// RAIL handle and scheduler settings for BLE
static RAIL_Handle_t sl_rail_handle = RAIL_EFR32_HANDLE;
#if SL_RAIL_UTIL_INIT_SCHEDULER_SUPPORT_BLE_ENABLE
static RAILSched_Config_t sl_rail_sched_config;
#endif // SL_RAIL_UTIL_INIT_SCHEDULER_SUPPORT_BLE_ENABLE
static RAIL_Config_t sl_rail_config = {
  .eventsCallback = &sli_rail_util_on_event,
#if SL_RAIL_UTIL_INIT_SCHEDULER_SUPPORT_BLE_ENABLE
  .scheduler = &sl_rail_sched_config,
#endif // SL_RAIL_UTIL_INIT_SCHEDULER_SUPPORT_BLE_ENABLE
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void sl_flex_util_init(void)
{
#if SL_RAIL_UTIL_INIT_BLE_ENABLE
  RAIL_Status_t status;
  // RAIL initialization
  sl_rail_handle = RAIL_Init(&sl_rail_config,
#if SL_RAIL_UTIL_INIT_INIT_COMPLETE_CALLBACK_BLE_ENABLE
                             &sli_rail_util_on_rf_ready
#else
                             NULL
#endif // SL_RAIL_UTIL_INIT_INIT_COMPLETE_CALLBACK_BLE_ENABLE
                             );
  app_assert((NULL != sl_rail_handle),
             "RAIL_Init failed, return value: NULL");

#if SL_RAIL_UTIL_INIT_DATA_FORMATS_BLE_ENABLE
  // data configuration for BLE
  RAIL_DataConfig_t data_config = {
    .txSource = SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_TX_SOURCE,
    .rxSource = SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_RX_SOURCE,
    .txMethod = SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_TX_MODE,
    .rxMethod = SL_RAIL_UTIL_INIT_DATA_FORMAT_BLE_RX_MODE,
  };
  status = RAIL_ConfigData(sl_rail_handle, &data_config);
  app_assert((RAIL_STATUS_NO_ERROR == status),
             "RAIL_ConfigData failed, return value: %d",
             status);
#endif // SL_RAIL_UTIL_INIT_DATA_FORMATS_BLE_ENABLE

#if SL_RAIL_UTIL_INIT_PROTOCOLS_BLE_ENABLE
  // channels confguration
  const RAIL_ChannelConfig_t *channel_config = NULL;
  (void) RAIL_ConfigChannels(sl_rail_handle,
                             channel_config,
                             &sli_rail_util_on_channel_config_change);
  // BLE protocol configuration based on the chosen BLE protocol
  status = sl_flex_util_ble_protocol_config(sl_rail_handle,
                                            SL_FLEX_UTIL_INIT_PROTOCOL_INSTANCE_DEFAULT);
  app_assert((RAIL_STATUS_NO_ERROR == status),
             "sl_rail_util_protocol_config failed, return value: %d",
             status);
#endif // SL_RAIL_UTIL_INIT_PROTOCOLS_BLE_ENABLE

#if SL_RAIL_UTIL_INIT_CALIBRATIONS_BLE_ENABLE
  status = RAIL_ConfigCal(sl_rail_handle,
                          0U
                          | (SL_RAIL_UTIL_INIT_CALIBRATION_TEMPERATURE_NOTIFY_BLE_ENABLE
                             ? RAIL_CAL_TEMP : 0U)
                          | (SL_RAIL_UTIL_INIT_CALIBRATION_ONETIME_NOTIFY_BLE_ENABLE
                             ? RAIL_CAL_ONETIME : 0U));
  app_assert((RAIL_STATUS_NO_ERROR == status),
             "RAIL_ConfigCal failed, return value: %d",
             status);
#endif // SL_RAIL_UTIL_INIT_CALIBRATIONS_BLE_ENABLE

#if SL_RAIL_UTIL_INIT_EVENTS_BLE_ENABLE
  // Events configuration
  status = RAIL_ConfigEvents(sl_rail_handle,
                             RAIL_EVENTS_ALL,
                             SL_RAIL_UTIL_INIT_EVENT_BLE_MASK);
  app_assert((RAIL_STATUS_NO_ERROR == status),
             "RAIL_ConfigEvents failed, return value: %d",
             status);
#endif // SL_RAIL_UTIL_INIT_EVENTS_BLE_ENABLE

#if SL_RAIL_UTIL_INIT_TRANSITIONS_BLE_ENABLE
  // transitions settings
  RAIL_StateTransitions_t tx_transitions = {
    .success = SL_RAIL_UTIL_INIT_TRANSITION_BLE_TX_SUCCESS,
    .error = SL_RAIL_UTIL_INIT_TRANSITION_BLE_TX_ERROR
  };
  RAIL_StateTransitions_t rx_transitions = {
    .success = SL_RAIL_UTIL_INIT_TRANSITION_BLE_RX_SUCCESS,
    .error = SL_RAIL_UTIL_INIT_TRANSITION_BLE_RX_ERROR
  };
  status = RAIL_SetTxTransitions(sl_rail_handle,
                                 &tx_transitions);
  app_assert((RAIL_STATUS_NO_ERROR == status),
             "RAIL_SetTxTransitions failed, return value: %d",
             status);
  status = RAIL_SetRxTransitions(sl_rail_handle,
                                 &rx_transitions);
  app_assert((RAIL_STATUS_NO_ERROR == status),
             "RAIL_SetRxTransitions failed, return value: %d",
             status);
#endif // SL_RAIL_UTIL_INIT_TRANSITIONS_BLE_ENABLE
#else // !SL_RAIL_UTIL_INIT_BLE_ENABLE
  // Eliminate compiler warnings.
  (void) sl_rail_handle;
  (void) sl_rail_config;
#endif // SL_RAIL_UTIL_INIT_BLE_ENABLE
}

RAIL_Handle_t sl_flex_util_get_handle(void)
{
  return sl_rail_handle;
}

#if !SL_RAIL_LIB_MULTIPROTOCOL_SUPPORT && ((0 + 1) > 1)
  #error "sl_rail_util_init.c: If you are going to use more than one rail_util_init instance, you must use the Multiprotocol RAIL library."
#endif
