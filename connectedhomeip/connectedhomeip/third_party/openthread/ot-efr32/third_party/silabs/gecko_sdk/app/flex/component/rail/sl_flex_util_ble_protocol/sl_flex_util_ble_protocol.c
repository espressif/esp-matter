/***************************************************************************//**
 * @file
 * @brief sl_flex_util_ble_protocol.c
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
#include <string.h>
#include "rail.h"
#include "rail_ble.h"
#include "sl_flex_util_ble_protocol.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
#if RAIL_FEAT_2G4_RADIO
/**************************************************************************//**
 * This function configures the BLE based on the BLE protocols.
 *
 * @param[in] handle The RAIL handle to apply the radio configuration to.
 * @param[in] protocol The radio configuration type to initialize and configure.(BLE)
 * @return A status code indicating success of the function call.
 *****************************************************************************/
static RAIL_Status_t sl_rail_util_protocol_config_ble(RAIL_Handle_t handle,
                                                      sl_flex_util_ble_protocol_type_t protocol);
#endif

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
RAIL_Status_t sl_flex_util_ble_protocol_config(RAIL_Handle_t handle,
                                               sl_flex_util_ble_protocol_type_t protocol)
{
  switch (protocol) {
#if RAIL_FEAT_2G4_RADIO
    case SL_RAIL_UTIL_PROTOCOL_BLE_1MBPS:
    case SL_RAIL_UTIL_PROTOCOL_BLE_2MBPS:
    case SL_RAIL_UTIL_PROTOCOL_BLE_CODED_125KBPS:
    case SL_RAIL_UTIL_PROTOCOL_BLE_CODED_500KBPS:
    case SL_RAIL_UTIL_PROTOCOL_BLE_QUUPPA_1MBPS:
      return sl_rail_util_protocol_config_ble(handle, protocol);
#endif
    default:
      return RAIL_STATUS_INVALID_PARAMETER;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
#if RAIL_FEAT_2G4_RADIO
static RAIL_Status_t sl_rail_util_protocol_config_ble(RAIL_Handle_t handle,
                                                      sl_flex_util_ble_protocol_type_t protocol)
{
  RAIL_Status_t status;
  // Override BLE's default timings to get rid of the default rx search timeout
  RAIL_StateTiming_t timings = {
    .idleToRx = SL_RAIL_UTIL_PROTOCOL_BLE_TIMING_IDLE_TO_RX_US,
    .txToRx = SL_RAIL_UTIL_PROTOCOL_BLE_TIMING_TX_TO_RX_US,
    .idleToTx = SL_RAIL_UTIL_PROTOCOL_BLE_TIMING_IDLE_TO_TX_US,
    .rxToTx = SL_RAIL_UTIL_PROTOCOL_BLE_TIMING_RX_TO_TX_US,
    .rxSearchTimeout = SL_RAIL_UTIL_PROTOCOL_BLE_TIMING_RX_SEARCH_TIMEOUT_AFTER_IDLE_ENABLE
                       ? SL_RAIL_UTIL_PROTOCOL_BLE_TIMING_RX_SEARCH_TIMEOUT_AFTER_IDLE_US
                       : 0U,
    .txToRxSearchTimeout = SL_RAIL_UTIL_PROTOCOL_BLE_TIMING_RX_SEARCH_TIMEOUT_AFTER_TX_ENABLE
                           ? SL_RAIL_UTIL_PROTOCOL_BLE_TIMING_RX_SEARCH_TIMEOUT_AFTER_TX_US
                           : 0U,
  };

  RAIL_BLE_Init(handle);
  switch (protocol) {
    case SL_RAIL_UTIL_PROTOCOL_BLE_1MBPS:
      status = RAIL_BLE_ConfigPhy1MbpsViterbi(handle);
      break;
    case SL_RAIL_UTIL_PROTOCOL_BLE_2MBPS:
      status = RAIL_BLE_ConfigPhy2MbpsViterbi(handle);
      break;
    case SL_RAIL_UTIL_PROTOCOL_BLE_CODED_125KBPS:
      status = RAIL_BLE_ConfigPhyCoded(handle, RAIL_BLE_Coding_125kbps);
      break;
    case SL_RAIL_UTIL_PROTOCOL_BLE_CODED_500KBPS:
      status = RAIL_BLE_ConfigPhyCoded(handle, RAIL_BLE_Coding_500kbps);
      break;
    case SL_RAIL_UTIL_PROTOCOL_BLE_QUUPPA_1MBPS:
      status = RAIL_BLE_ConfigPhyQuuppa(handle);
      break;
    default:
      status = RAIL_STATUS_INVALID_PARAMETER;
      break;
  }
  if (RAIL_STATUS_NO_ERROR == status) {
    status = RAIL_SetStateTiming(handle, &timings);
  }
  if (RAIL_STATUS_NO_ERROR != status) {
    RAIL_BLE_Deinit(handle);
  }
  return status;
}
#endif // RAIL_FEAT_2G4_RADIO
