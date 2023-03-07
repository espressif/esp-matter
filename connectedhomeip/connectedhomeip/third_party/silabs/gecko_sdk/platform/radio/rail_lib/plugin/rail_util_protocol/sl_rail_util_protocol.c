/***************************************************************************//**
 * @file
 * @brief This file implements the radio config commands for RAILtest
 *   applications.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <string.h>
#include <stdio.h>

#include "rail.h"
#include "rail_ble.h"
#include "rail_ieee802154.h"
#include "rail_zwave.h"

#include "sl_rail_util_protocol.h"

static RAIL_Status_t sl_rail_util_protocol_config_proprietary(RAIL_Handle_t handle)
{
  (void) RAIL_SetPtiProtocol(handle, RAIL_PTI_PROTOCOL_CUSTOM);
  return RAIL_STATUS_NO_ERROR;
}

#if RAIL_FEAT_2G4_RADIO
static RAIL_Status_t sl_rail_util_protocol_config_ble(RAIL_Handle_t handle,
                                                      sl_rail_util_protocol_type_t protocol)
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
#endif

#if RAIL_FEAT_2G4_RADIO
static RAIL_Status_t sl_rail_util_protocol_config_ieee802154_2p4ghz(RAIL_Handle_t handle,
                                                                    sl_rail_util_protocol_type_t protocol)
{
  RAIL_Status_t status;
  RAIL_IEEE802154_Config_t config = {
    .addresses = NULL,
    .ackConfig = {
      .enable = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_AUTO_ACK_ENABLE,
      .ackTimeout = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_AUTO_ACK_TIMEOUT_US,
      .rxTransitions = {
        .success = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_AUTO_ACK_RX_TRANSITION_STATE,
        .error = RAIL_RF_STATE_IDLE // this parameter ignored
      },
      .txTransitions = {
        .success = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_AUTO_ACK_TX_TRANSITION_STATE,
        .error = RAIL_RF_STATE_IDLE // this parameter ignored
      }
    },
    .timings = {
      .idleToTx = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_TIMING_IDLE_TO_TX_US,
      .idleToRx = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_TIMING_IDLE_TO_RX_US,
      .rxToTx = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_TIMING_RX_TO_TX_US,
      // Make txToRx slightly lower than desired to make sure we get to
      // RX in time.
      .txToRx = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_TIMING_TX_TO_RX_US,
      .rxSearchTimeout = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_TIMING_RX_SEARCH_TIMEOUT_AFTER_IDLE_ENABLE
                         ? SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_TIMING_RX_SEARCH_TIMEOUT_AFTER_IDLE_US
                         : 0,
      .txToRxSearchTimeout = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_TIMING_RX_SEARCH_TIMEOUT_AFTER_TX_ENABLE
                             ? SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_TIMING_RX_SEARCH_TIMEOUT_AFTER_TX_US
                             : 0
    },
    .framesMask = 0U // enable appropriate mask bits
                  | (SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ACCEPT_BEACON_FRAME_ENABLE
                     ? RAIL_IEEE802154_ACCEPT_BEACON_FRAMES : 0U)
                  | (SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ACCEPT_DATA_FRAME_ENABLE
                     ? RAIL_IEEE802154_ACCEPT_DATA_FRAMES : 0U)
                  | (SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ACCEPT_ACK_FRAME_ENABLE
                     ? RAIL_IEEE802154_ACCEPT_ACK_FRAMES : 0U)
                  | (SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ACCEPT_COMMAND_FRAME_ENABLE
                     ? RAIL_IEEE802154_ACCEPT_COMMAND_FRAMES : 0U),
    // Enable promiscous mode since no PANID or destination address is
    // specified.
    .promiscuousMode = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_PROMISCUOUS_MODE_ENABLE,
    .isPanCoordinator = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_PAN_COORDINATOR_ENABLE,
    .defaultFramePendingInOutgoingAcks = SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_DEFAULT_FRAME_PENDING_STATE,
  };
  status = RAIL_IEEE802154_Init(handle, &config);
  if (RAIL_STATUS_NO_ERROR == status) {
    switch (protocol) {
      case SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ:
        status = RAIL_IEEE802154_Config2p4GHzRadio(handle);
        break;
      case SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV:
        status = RAIL_IEEE802154_Config2p4GHzRadioAntDiv(handle);
        break;
      case SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_COEX:
        status = RAIL_IEEE802154_Config2p4GHzRadioCoex(handle);
        break;
      case SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV_COEX:
        status = RAIL_IEEE802154_Config2p4GHzRadioAntDivCoex(handle);
        break;
      default:
        status = RAIL_STATUS_INVALID_PARAMETER;
        break;
    }
  }
  if (RAIL_STATUS_NO_ERROR != status) {
    (void) RAIL_IEEE802154_Deinit(handle);
  } else {
    (void) RAIL_SetPtiProtocol(handle, RAIL_PTI_PROTOCOL_802154);
  }
  return status;
}
#endif // RAIL_FEAT_2G4_RADIO

#if RAIL_FEAT_SUBGIG_RADIO
static RAIL_Status_t sl_rail_util_protocol_config_ieee802154_gb868(RAIL_Handle_t handle,
                                                                   sl_rail_util_protocol_type_t protocol)
{
  RAIL_Status_t status;
  RAIL_IEEE802154_Config_t config = {
    .addresses = NULL,
    .ackConfig = {
      .enable = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_AUTO_ACK_ENABLE,
      .ackTimeout = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_AUTO_ACK_TIMEOUT_US,
      .rxTransitions = {
        .success = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_AUTO_ACK_RX_TRANSITION_STATE,
        .error = RAIL_RF_STATE_IDLE // this parameter ignored
      },
      .txTransitions = {
        .success = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_AUTO_ACK_TX_TRANSITION_STATE,
        .error = RAIL_RF_STATE_IDLE // this parameter ignored
      }
    },
    .timings = {
      .idleToTx = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_TIMING_IDLE_TO_TX_US,
      .idleToRx = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_TIMING_IDLE_TO_RX_US,
      .rxToTx = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_TIMING_RX_TO_TX_US,
      // Make txToRx slightly lower than desired to make sure we get to
      // RX in time.
      .txToRx = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_TIMING_TX_TO_RX_US,
      .rxSearchTimeout = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_TIMING_RX_SEARCH_TIMEOUT_AFTER_IDLE_ENABLE
                         ? SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_TIMING_RX_SEARCH_TIMEOUT_AFTER_IDLE_US
                         : 0,
      .txToRxSearchTimeout = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_TIMING_RX_SEARCH_TIMEOUT_AFTER_TX_ENABLE
                             ? SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_TIMING_RX_SEARCH_TIMEOUT_AFTER_TX_US
                             : 0
    },
    .framesMask = 0U // enable appropriate mask bits
                  | (SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_ACCEPT_BEACON_FRAME_ENABLE
                     ? RAIL_IEEE802154_ACCEPT_BEACON_FRAMES : 0U)
                  | (SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_ACCEPT_DATA_FRAME_ENABLE
                     ? RAIL_IEEE802154_ACCEPT_DATA_FRAMES : 0U)
                  | (SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_ACCEPT_ACK_FRAME_ENABLE
                     ? RAIL_IEEE802154_ACCEPT_ACK_FRAMES : 0U)
                  | (SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_ACCEPT_COMMAND_FRAME_ENABLE
                     ? RAIL_IEEE802154_ACCEPT_COMMAND_FRAMES : 0U),
    // Enable promiscous mode since no PANID or destination address is
    // specified.
    .promiscuousMode = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_PROMISCUOUS_MODE_ENABLE,
    .isPanCoordinator = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_PAN_COORDINATOR_ENABLE,
    .defaultFramePendingInOutgoingAcks = SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_DEFAULT_FRAME_PENDING_STATE,
  };
  status = RAIL_IEEE802154_Init(handle, &config);
  if (RAIL_STATUS_NO_ERROR == status) {
    switch (protocol) {
      case SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ:
        status = RAIL_IEEE802154_ConfigGB915MHzRadio(handle);
        break;
      case SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ:
        status = RAIL_IEEE802154_ConfigGB863MHzRadio(handle);
        break;
      default:
        status = RAIL_STATUS_INVALID_PARAMETER;
        break;
    }
  }
  if (RAIL_STATUS_NO_ERROR != status) {
    (void) RAIL_IEEE802154_Deinit(handle);
  } else {
    (void) RAIL_SetPtiProtocol(handle, RAIL_PTI_PROTOCOL_802154);
  }
  return status;
}
#endif // RAIL_FEAT_SUBGIG_RADIO

#if RAIL_FEAT_SUBGIG_RADIO
static RAIL_Status_t sl_rail_util_protocol_config_zwave(RAIL_Handle_t handle,
                                                        sl_rail_util_protocol_type_t protocol)
{
  RAIL_Status_t status;
  RAIL_ZWAVE_Config_t config = {
    .options = 0U // enable appropriate mask bits
               | (SL_RAIL_UTIL_PROTOCOL_ZWAVE_PROMISCUOUS_MODE_ENABLE
                  ? RAIL_ZWAVE_OPTION_PROMISCUOUS_MODE : 0U)
               | (SL_RAIL_UTIL_PROTOCOL_ZWAVE_DETECT_BEAM_FRAME_ENABLE
                  ? RAIL_ZWAVE_OPTION_DETECT_BEAM_FRAMES : 0U)
               | (SL_RAIL_UTIL_PROTOCOL_ZWAVE_NODE_ID_FILTERING_ENABLE
                  ? RAIL_ZWAVE_OPTION_NODE_ID_FILTERING : 0U),
    .ackConfig = {
      .enable = SL_RAIL_UTIL_PROTOCOL_ZWAVE_AUTO_ACK_ENABLE,
      .ackTimeout = SL_RAIL_UTIL_PROTOCOL_ZWAVE_AUTO_ACK_TIMEOUT_US,
      .rxTransitions = {
        .success = SL_RAIL_UTIL_PROTOCOL_ZWAVE_AUTO_ACK_RX_TRANSITION_STATE,
        .error = RAIL_RF_STATE_IDLE // this parameter ignored
      },
      .txTransitions = {
        .success = SL_RAIL_UTIL_PROTOCOL_ZWAVE_AUTO_ACK_TX_TRANSITION_STATE,
        .error = RAIL_RF_STATE_IDLE // this parameter ignored
      }
    },
    .timings = {
      .idleToTx = SL_RAIL_UTIL_PROTOCOL_ZWAVE_TIMING_IDLE_TO_TX_US,
      .idleToRx = SL_RAIL_UTIL_PROTOCOL_ZWAVE_TIMING_IDLE_TO_RX_US,
      .rxToTx = SL_RAIL_UTIL_PROTOCOL_ZWAVE_TIMING_RX_TO_TX_US,
      // Make txToRx slightly lower than desired to make sure we get to
      // RX in time.
      .txToRx = SL_RAIL_UTIL_PROTOCOL_ZWAVE_TIMING_TX_TO_RX_US,
      .rxSearchTimeout = SL_RAIL_UTIL_PROTOCOL_ZWAVE_TIMING_RX_SEARCH_TIMEOUT_AFTER_IDLE_ENABLE
                         ? SL_RAIL_UTIL_PROTOCOL_ZWAVE_TIMING_RX_SEARCH_TIMEOUT_AFTER_IDLE_US
                         : 0,
      .txToRxSearchTimeout = SL_RAIL_UTIL_PROTOCOL_ZWAVE_TIMING_RX_SEARCH_TIMEOUT_AFTER_TX_ENABLE
                             ? SL_RAIL_UTIL_PROTOCOL_ZWAVE_TIMING_RX_SEARCH_TIMEOUT_AFTER_TX_US
                             : 0
    }
  };
  status = RAIL_ZWAVE_Init(handle, &config);
  if (RAIL_STATUS_NO_ERROR == status) {
    switch (protocol) {
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_ANZ: // Australia
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_ANZ);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_CN: // China
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_CN);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_EU: // European Union
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_EU);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_HK: // Hong Kong
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_HK);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_IN: // India
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_IN);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_IL: // Israel
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_IL);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_JP: // Japan
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_JP);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_KR: // Korea
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_KR);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_MY: // Malaysia
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_MY);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_RU: // Russia
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_RU);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_US: // United States
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_US);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR1: // US, Long Range 1
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_US_LR1);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR2: // US, Long Range 2
        status = RAIL_ZWAVE_ConfigRegion(handle, &RAIL_ZWAVE_REGION_US_LR2);
        break;
      case SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR_END_DEVICE: // US, LR End Device
        status = RAIL_ZWAVE_ConfigRegion(handle,
                                         &RAIL_ZWAVE_REGION_US_LR_END_DEVICE);
        break;
      default:
        status = RAIL_STATUS_INVALID_PARAMETER;
        break;
    }
  }
  if (RAIL_STATUS_NO_ERROR == status) {
    status = RAIL_ZWAVE_SetNodeId(handle, RAIL_ZWAVE_NODE_ID_DEFAULT);
  }
  if (RAIL_STATUS_NO_ERROR != status) {
    (void) RAIL_ZWAVE_Deinit(handle);
  }
  return status;
}
#endif // RAIL_FEAT_SUBGIG_RADIO

RAIL_Status_t sl_rail_util_protocol_config(RAIL_Handle_t handle,
                                           sl_rail_util_protocol_type_t protocol)
{
  switch (protocol) {
    case SL_RAIL_UTIL_PROTOCOL_PROPRIETARY:
      return sl_rail_util_protocol_config_proprietary(handle);
#if RAIL_FEAT_2G4_RADIO
    case SL_RAIL_UTIL_PROTOCOL_BLE_1MBPS:
    case SL_RAIL_UTIL_PROTOCOL_BLE_2MBPS:
    case SL_RAIL_UTIL_PROTOCOL_BLE_CODED_125KBPS:
    case SL_RAIL_UTIL_PROTOCOL_BLE_CODED_500KBPS:
    case SL_RAIL_UTIL_PROTOCOL_BLE_QUUPPA_1MBPS:
      return sl_rail_util_protocol_config_ble(handle, protocol);
    case SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ:
    case SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV:
    case SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_COEX:
    case SL_RAIL_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV_COEX:
      return sl_rail_util_protocol_config_ieee802154_2p4ghz(handle, protocol);
#endif
#if RAIL_FEAT_SUBGIG_RADIO
    case SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ:
    case SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ:
      return sl_rail_util_protocol_config_ieee802154_gb868(handle, protocol);
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_ANZ: // Australia
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_CN: // China
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_EU: // European Union
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_HK: // Hong Kong
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_IN: // India
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_IL: // Israel
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_JP: // Japan
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_KR: // Korea
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_MY: // Malaysia
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_RU: // Russia
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_US: // United States
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR1: // United States, Long Range 1
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR2: // United States, Long Range 2
    case SL_RAIL_UTIL_PROTOCOL_ZWAVE_US_LR_END_DEVICE: // US, LR End Device
      return sl_rail_util_protocol_config_zwave(handle, protocol);
#endif
    default:
      return RAIL_STATUS_INVALID_PARAMETER;
  }
}
