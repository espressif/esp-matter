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

#include "response_print.h"

#include "rail.h"
#include "rail_types.h"
#include "rail_features.h"

#include "rail_ble.h"
#include "rail_ieee802154.h"
#include "rail_mfm.h"
#include "rail_zwave.h"

#include "app_common.h"
#include "sl_rail_util_pa_config.h"
#include "sl_rail_util_init.h"
#include "sli_rail_util_callbacks.h"

#if SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
  #include "rail_config.h"
#endif // SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE

bool verifyConfigEnabled = false;

// Channel Config Selection Variable
uint8_t configIndex = 0;

const char* paStrings[] = RAIL_TX_POWER_MODE_NAMES;

/******************************************************************************
 * Command Line Interface functions
 *****************************************************************************/
void getConfigIndex(sl_cli_command_arg_t *args)
{
#if SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
  responsePrint(sl_cli_get_command_string(args, 0), "configIndex:%d", configIndex);
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0x22, "External radio config support not enabled");
#endif // SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
}

void setConfigIndex(sl_cli_command_arg_t *args)
{
#if SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
  uint8_t proposedIndex = sl_cli_get_argument_uint8(args, 0);

  // Be sure that the proposed index is valid. Scan through all possible
  // indexes and check for the last NULL parameter since you can't
  // use sizeof on an extern-ed array without an explicit index.
  for (uint8_t i = 0; i <= proposedIndex; i++) {
    if (channelConfigs[i] == NULL) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x11,
                         "Invalid radio config index '%d'",
                         proposedIndex);
      return;
    }
  }

  configIndex = proposedIndex;
  RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
  // Load the channel configuration for the specified index.
  channel = RAIL_ConfigChannels(railHandle,
                                channelConfigs[configIndex],
                                &sli_rail_util_on_channel_config_change);
  responsePrint(sl_cli_get_command_string(args, 0), "configIndex:%d,firstAvailableChannel:%d",
                configIndex,
                channel);
  if (receiveModeEnabled) {
    (void) RAIL_StartRx(railHandle, channel, NULL);
  }
#else // !SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
  responsePrintError(sl_cli_get_command_string(args, 0), 0x22, "External radio config support not enabled");
#endif // SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
}

void setTransitionTime(sl_cli_command_arg_t *args)
{
  RAIL_SetTransitionTime((RAIL_Time_t)sl_cli_get_argument_uint32(args, 0));
  responsePrint(sl_cli_get_command_string(args, 0),
                "DMP transition time:%d,"
                "Status:Success",
                RAIL_GetTransitionTime());
}

void getTransitionTime(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0),
                "DMP transition time:%d",
                RAIL_GetTransitionTime());
}

void getChannelConfigEntry(sl_cli_command_arg_t *args)
{
#if SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
  uint8_t channelConfigIndex = sl_cli_get_argument_uint8(args, 0);
  uint8_t channelConfigEntryIndex = sl_cli_get_argument_uint8(args, 1);

  // Check if channelConfigs is empty
  if (channelConfigs[0] == NULL) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x11,
                       "Empty channelConfigs");
    return;
  }

  // Get number of channelConfig
  uint8_t maxChannelConfigIndex;
  for (maxChannelConfigIndex = 1; maxChannelConfigIndex < 0xFFU; maxChannelConfigIndex++) {
    if (channelConfigs[maxChannelConfigIndex] == NULL) {
      maxChannelConfigIndex--;
      break;
    }
  }

  // Get number of channelConfigEntry if valid channelConfigIndex
  if (channelConfigIndex > maxChannelConfigIndex) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x11,
                       "Invalid channelConfigIndex '%u', maxChannelConfigIndex:%u",
                       channelConfigIndex, maxChannelConfigIndex);
    return;
  }
  uint8_t maxChannelConfigEntryIndex = channelConfigs[channelConfigIndex]->length - 1;

  // Get parameters if valid channelConfigIndex & channelConfigEntryIndex
  if (channelConfigEntryIndex > maxChannelConfigEntryIndex) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x11,
                       "Invalid channelConfigEntryIndex '%u', maxChannelConfigEntryIndex:%u",
                       channelConfigEntryIndex, maxChannelConfigEntryIndex);
    return;
  }
  const RAIL_ChannelConfigEntry_t* entry = &(channelConfigs[channelConfigIndex]->configs[channelConfigEntryIndex]);
#ifdef RADIO_CONFIG_ENABLE_STACK_INFO
  const uint8_t *stackInfo = entry->stackInfo;
  if (stackInfo == NULL) {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "maxChannelConfigIndex:%d,maxChannelConfigEntryIndex:%d,baseFrequency:%d,channelSpacing:%d,"
                  "physicalChannelOffset:%d,channelNumberStart:%d,channelNumberEnd:%d,maxPower:%d,protocolId:%s,phyModeId:%s",
                  maxChannelConfigIndex, maxChannelConfigEntryIndex, entry->baseFrequency, entry->channelSpacing,
                  entry->physicalChannelOffset, entry->channelNumberStart, entry->channelNumberEnd, entry->maxPower, "N/A", "N/A");
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "maxChannelConfigIndex:%d,maxChannelConfigEntryIndex:%d,baseFrequency:%d,channelSpacing:%d,"
                  "physicalChannelOffset:%d,channelNumberStart:%d,channelNumberEnd:%d,maxPower:%d,protocolId:%d,phyModeId:%d",
                  maxChannelConfigIndex, maxChannelConfigEntryIndex, entry->baseFrequency, entry->channelSpacing,
                  entry->physicalChannelOffset, entry->channelNumberStart, entry->channelNumberEnd, entry->maxPower, stackInfo[0], stackInfo[1]);
  }
#else // RADIO_CONFIG_ENABLE_STACK_INFO
  responsePrint(sl_cli_get_command_string(args, 0),
                "maxChannelConfigIndex:%d,maxChannelConfigEntryIndex:%d,baseFrequency:%d,channelSpacing:%d,"
                "physicalChannelOffset:%d,channelNumberStart:%d,channelNumberEnd:%d,maxPower:%d",
                maxChannelConfigIndex, maxChannelConfigEntryIndex, entry->baseFrequency, entry->channelSpacing,
                entry->physicalChannelOffset, entry->channelNumberStart, entry->channelNumberEnd, entry->maxPower);
#endif // RADIO_CONFIG_ENABLE_STACK_INFO
#else // SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
  responsePrintError(sl_cli_get_command_string(args, 0), 0x22, "External radio config support not enabled");
#endif // SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE
}

/******************************************************************************
 * Callbacks
 *****************************************************************************/
// Override weak function called by callback sli_rail_util_on_channel_config_change.
void sl_rail_util_on_channel_config_change(RAIL_Handle_t railHandle,
                                           const RAIL_ChannelConfigEntry_t *entry)
{
  (void)entry;
  // Now that the radio config changed, verify the new data contents.
  RAIL_ConfigVerification(railHandle, &configVerify, NULL, NULL);
  if (verifyConfigEnabled && RAIL_Verify(&configVerify, RAIL_VERIFY_DURATION_MAX, true)
      == RAIL_STATUS_INVALID_STATE) {
    responsePrint("verifyRadio", "Radio data corruption detected!");
    while (1) ;
  }

  counters.radioConfigChanged++;
}

void disableIncompatibleProtocols(RAIL_PtiProtocol_t newProtocol)
{
  if ((newProtocol != RAIL_PTI_PROTOCOL_BLE)
      && RAIL_BLE_IsEnabled(railHandle)) {
    (void) RAIL_BLE_Deinit(railHandle);
  }
  if ((newProtocol != RAIL_PTI_PROTOCOL_ZIGBEE)
      && RAIL_IEEE802154_IsEnabled(railHandle)) {
    (void) RAIL_IEEE802154_Deinit(railHandle);
  }
  if ((newProtocol != RAIL_PTI_PROTOCOL_ZWAVE)
      && RAIL_ZWAVE_IsEnabled(railHandle)) {
    (void) RAIL_ZWAVE_Deinit(railHandle);
  }
  (void) RAIL_SetPtiProtocol(railHandle, newProtocol);
}

void configRfSenseSelectiveOokWakeupPhy(sl_cli_command_arg_t *args)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  RAIL_Status_t status = RAIL_ConfigRfSenseSelectiveOokWakeupPhy(railHandle);
  responsePrint(sl_cli_get_command_string(args, 0), "RFSense Wakeup PHY:%s", status ? "Disabled" : "Enabled");
}
