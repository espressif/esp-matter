/***************************************************************************//**
 * @file
 * @brief sl_flex_util_802154_protocol.c
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
#include "rail_ieee802154.h"
#include "sl_flex_util_802154_protocol.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
#if RAIL_FEAT_2G4_RADIO
/**************************************************************************//**
 * This function configures the RAIL for IEEE 802.15.4 on 2.4GHz protocol
 *
 * @param[in] handle      RAIL handle
 * @param[in] protocol    IEEE 802.15.4 protocol
 *
 * @return RAIL status, RAIL_STATUS_NO_ERROR if the configuration has been successful.
 *****************************************************************************/
static RAIL_Status_t sl_flex_util_protocol_config_ieee802154_2p4ghz(RAIL_Handle_t handle,
                                                                    sl_flex_util_802154_protocol_type_t protocol);
#endif

#if RAIL_FEAT_SUBGIG_RADIO && defined(SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_AUTO_ACK_ENABLE)
/**************************************************************************//**
 * This function configures the RAIL for IEEE 802.15.4 on 863MHz protocol
 *
 * @param[in] handle      RAIL handle
 * @param[in] protocol    IEEE 802.15.4 protocol
 *
 * @return RAIL status, RAIL_STATUS_NO_ERROR if the configuration has been successful.
 *****************************************************************************/
static RAIL_Status_t sl_flex_util_protocol_config_ieee802154_gb868_863mhz(RAIL_Handle_t handle,
                                                                          sl_flex_util_802154_protocol_type_t protocol);

/**************************************************************************//**
 * This function configures the RAIL for IEEE 802.15.4 on 915MHz protocol
 *
 * @param[in] handle      RAIL handle
 * @param[in] protocol    IEEE 802.15.4 protocol
 *
 * @return RAIL status, RAIL_STATUS_NO_ERROR if the configuration has been successful.
 *****************************************************************************/
static RAIL_Status_t sl_flex_util_protocol_config_ieee802154_gb868_915mhz(RAIL_Handle_t handle,
                                                                          sl_flex_util_802154_protocol_type_t protocol);
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
RAIL_Status_t sl_flex_util_802154_protocol_config(RAIL_Handle_t handle,
                                                  sl_flex_util_802154_protocol_type_t protocol)
{
  switch (protocol) {
#if RAIL_FEAT_2G4_RADIO
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ:
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV:
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_COEX:
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV_COEX:
      return sl_flex_util_protocol_config_ieee802154_2p4ghz(handle, protocol);
#endif
#if RAIL_FEAT_SUBGIG_RADIO && defined(SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_AUTO_ACK_ENABLE)
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ:
      return sl_flex_util_protocol_config_ieee802154_gb868_915mhz(handle, protocol);
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ:
      return sl_flex_util_protocol_config_ieee802154_gb868_863mhz(handle, protocol);
#endif
    default:
      return RAIL_STATUS_INVALID_PARAMETER;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
#if RAIL_FEAT_2G4_RADIO
static RAIL_Status_t sl_flex_util_protocol_config_ieee802154_2p4ghz(RAIL_Handle_t handle,
                                                                    sl_flex_util_802154_protocol_type_t protocol)
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
      case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ:
        status = RAIL_IEEE802154_Config2p4GHzRadio(handle);
        break;
      case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV:
        status = RAIL_IEEE802154_Config2p4GHzRadioAntDiv(handle);
        break;
      case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_COEX:
        status = RAIL_IEEE802154_Config2p4GHzRadioCoex(handle);
        break;
      case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV_COEX:
        status = RAIL_IEEE802154_Config2p4GHzRadioAntDivCoex(handle);
        break;
      default:
        status = RAIL_STATUS_INVALID_PARAMETER;
        break;
    }
  }
  if (RAIL_STATUS_NO_ERROR != status) {
    (void) RAIL_IEEE802154_Deinit(handle);
  }
  return status;
}
#endif // RAIL_FEAT_2G4_RADIO

#if RAIL_FEAT_SUBGIG_RADIO && defined(SL_RAIL_UTIL_PROTOCOL_IEEE802154_GB868_AUTO_ACK_ENABLE)
static RAIL_Status_t sl_flex_util_protocol_config_ieee802154_gb868_863mhz(RAIL_Handle_t handle,
                                                                          sl_flex_util_802154_protocol_type_t protocol)
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
    if (RAIL_STATUS_NO_ERROR == status) {
      if (protocol == SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ) {
        status = RAIL_IEEE802154_ConfigGB863MHzRadio(handle);
      } else {
        status = RAIL_STATUS_INVALID_PARAMETER;
      }
    }
  }
  if (RAIL_STATUS_NO_ERROR != status) {
    (void) RAIL_IEEE802154_Deinit(handle);
    return status;
  }

  // G option set
  status = RAIL_IEEE802154_ConfigGOptions(handle,
                                          RAIL_IEEE802154_G_OPTION_GB868,
                                          RAIL_IEEE802154_G_OPTION_GB868);

  return status;
}

static RAIL_Status_t sl_flex_util_protocol_config_ieee802154_gb868_915mhz(RAIL_Handle_t handle,
                                                                          sl_flex_util_802154_protocol_type_t protocol)
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
    if (protocol == SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ) {
      status = RAIL_IEEE802154_ConfigGB915MHzRadio(handle);
    } else {
      status = RAIL_STATUS_INVALID_PARAMETER;
    }
  }
  if (RAIL_STATUS_NO_ERROR != status) {
    (void) RAIL_IEEE802154_Deinit(handle);
    return status;
  }

  // G option set
  status = RAIL_IEEE802154_ConfigGOptions(handle,
                                          RAIL_IEEE802154_G_OPTION_GB868,
                                          RAIL_IEEE802154_G_OPTION_GB868);
  return status;
}
#endif // RAIL_FEAT_SUBGIG_RADIO
