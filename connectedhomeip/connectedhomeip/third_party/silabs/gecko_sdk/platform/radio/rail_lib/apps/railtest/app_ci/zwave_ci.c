/***************************************************************************//**
 * @file
 * @brief This file implements commands for configuring ZWAVE RAIL options
 *   relevant to receiving packets
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

#include <stdio.h>
#include <string.h>
#include "em_core.h"
#include "response_print.h"
#include "buffer_pool_allocator.h"
#include "circular_queue.h"
#include "rail.h"
#include "rail_features.h"
#include "rail_zwave.h"
#include "app_common.h"

// Macro to determine array size.
#define COMMON_UTILS_COUNTOF(a) (sizeof(a) / sizeof((a)[0]))

#if RAIL_FEAT_ZWAVE_SUPPORTED
RAIL_ZWAVE_Config_t config = {
  .options = RAIL_ZWAVE_OPTIONS_NONE,
  .ackConfig = {
    .enable = false,
    .ackTimeout = RAIL_ZWAVE_MAX_ACK_TIMEOUT_US,
    .rxTransitions = {
      .success = RAIL_RF_STATE_RX,
      .error = RAIL_RF_STATE_RX   // ignored
    },
    .txTransitions = {
      .success = RAIL_RF_STATE_RX,
      .error = RAIL_RF_STATE_RX   // ignored
    }
  },
  .timings = {
    .idleToTx = RAIL_ZWAVE_TIME_IDLE_TO_TX_US,
    .idleToRx = RAIL_ZWAVE_TIME_IDLE_TO_RX_US,
    .rxToTx = RAIL_ZWAVE_TIME_RX_TO_TX_US,
    .txToRx = RAIL_ZWAVE_TIME_TX_TO_RX_US,
    .rxSearchTimeout = 0,
    .txToRxSearchTimeout = 0
  }
};

typedef struct ZWAVE_Region {
  const char *name;
  RAIL_ZWAVE_RegionConfig_t const *config;
} ZWAVE_Region_t;

/******************* Local functions *******************/
static const ZWAVE_Region_t zwaveRegionTable[] = {
  { "EU-European Union", &RAIL_ZWAVE_REGION_EU },
  { "US-United States", &RAIL_ZWAVE_REGION_US },
  { "ANZ-Australia/New Zealand", &RAIL_ZWAVE_REGION_ANZ },
  { "HK-Hong Kong", &RAIL_ZWAVE_REGION_HK },
  { "MY-Malaysia", &RAIL_ZWAVE_REGION_MY },
  { "IN-India", &RAIL_ZWAVE_REGION_IN },
  { "JP-Japan", &RAIL_ZWAVE_REGION_JP },
  { "RU-Russia", &RAIL_ZWAVE_REGION_RU },
  { "IL-Israel", &RAIL_ZWAVE_REGION_IL },
  { "KR-Korea", &RAIL_ZWAVE_REGION_KR },
  { "CN-China", &RAIL_ZWAVE_REGION_CN },
  { "USLR1-United States, Long Range 1", &RAIL_ZWAVE_REGION_US_LR1 },
  { "USLR2-United States, Long Range 2", &RAIL_ZWAVE_REGION_US_LR2 },
  { "USLRED-United States, Long Range End Device", &RAIL_ZWAVE_REGION_US_LR_END_DEVICE },
};

#define ZWAVE_REGION_UNDEFINED (COMMON_UTILS_COUNTOF(zwaveRegionTable))
static uint8_t configuredRegion = ZWAVE_REGION_UNDEFINED;

static const char* baudrateNames[] = {
  "9600bps",
  "40Kbps",
  "100Kbps",
  "Long Range",
  "Energy Detect"
};

void zwaveListRegions(sl_cli_command_arg_t *args)
{
  uint8_t i;
  responsePrintStart(sl_cli_get_command_string(args, 0));
  for (i = 0;
       i < (uint8_t)ZWAVE_REGION_UNDEFINED - 1;
       ++i) {
    responsePrintContinue("%i:%s", i, zwaveRegionTable[i].name);
  }
  responsePrintEnd("%i:%s", i, zwaveRegionTable[(uint8_t)ZWAVE_REGION_UNDEFINED - 1].name);
}

void zwaveStatus(sl_cli_command_arg_t *args)
{
  bool enabled = RAIL_ZWAVE_IsEnabled(railHandle);

  // Report the current enabled status for ZWAVE
  responsePrint(sl_cli_get_command_string(args, 0),
                "ZWAVE:%s,"
                "Promiscuous:%s,"
                "BeamDetect:%s",
                enabled ? "Enabled" : "Disabled",
                ((config.options & RAIL_ZWAVE_OPTION_PROMISCUOUS_MODE) != 0U)
                ? "Enabled" : "Disabled",
                ((config.options & RAIL_ZWAVE_OPTION_DETECT_BEAM_FRAMES) != 0U)
                ? "Enabled" : "Disabled");
}

void zwaveEnable(sl_cli_command_arg_t *args)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  if (sl_cli_get_argument_count(args) >= 1) {
    bool enable = !!sl_cli_get_argument_uint8(args, 0);
    if (sl_cli_get_argument_count(args) >= 2) {
      config.options = sl_cli_get_argument_uint32(args, 1);
    }

    // Turn ZWAVE mode on or off as requested
    if (enable) {
      disableIncompatibleProtocols(RAIL_PTI_PROTOCOL_ZWAVE);
      RAIL_ZWAVE_Init(railHandle, &config);
    } else {
      RAIL_ZWAVE_Deinit(railHandle);
    }
  }
  // Report the current status of ZWAVE mode
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveStatus(args);
}

void zwaveConfigureOptions(sl_cli_command_arg_t *args)
{
  if (!RAIL_ZWAVE_IsEnabled(railHandle)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x26,
                       "Need to enable Z-Wave for this command.");
    return;
  }

  if (sl_cli_get_argument_count(args) >= 1) {
    RAIL_ZWAVE_Options_t options = (RAIL_ZWAVE_Options_t)sl_cli_get_argument_uint32(args, 0);
    RAIL_ZWAVE_ConfigOptions(railHandle, RAIL_ZWAVE_OPTIONS_ALL, options);
    config.options = options;
  }

  config.ackConfig.enable = RAIL_IsAutoAckEnabled(railHandle);
  // Report the status for ZWAVE options.
  responsePrint(sl_cli_get_command_string(args, 0),
                "Promiscuous:%s,"
                "BeamDetect:%s,"
                "NodeIDFiltering:%s,"
                "AutoAck:%s",
                ((config.options & RAIL_ZWAVE_OPTION_PROMISCUOUS_MODE) != 0U)
                ? "Enabled" : "Disabled",
                ((config.options & RAIL_ZWAVE_OPTION_DETECT_BEAM_FRAMES) != 0U)
                ? "Enabled" : "Disabled",
                ((config.options & RAIL_ZWAVE_OPTION_NODE_ID_FILTERING) != 0U)
                ? "Enabled" : "Disabled",
                (config.ackConfig.enable
                 && (config.options & RAIL_ZWAVE_OPTION_NODE_ID_FILTERING) != 0U)
                ? "Enabled" : "Disabled");
}

void zwaveGetRegion(sl_cli_command_arg_t *args)
{
  if (configuredRegion < ZWAVE_REGION_UNDEFINED) {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "ZWaveRegion:%s,ZWaveRegionIndex:%i",
                  zwaveRegionTable[configuredRegion].name,
                  configuredRegion);
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "ZWaveRegion:Undefined,ZWaveRegionIndex:%i",
                  configuredRegion);
  }
}

void zwaveSetRegion(sl_cli_command_arg_t *args)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  uint8_t region = sl_cli_get_argument_uint8(args, 0);
  if (region >= ZWAVE_REGION_UNDEFINED) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x25,
                       "Unsupported Z-Wave Region.");
    args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
    zwaveListRegions(args);
    return;
  }
  configuredRegion = region;
  if (RAIL_ZWAVE_ConfigRegion(railHandle, zwaveRegionTable[configuredRegion].config) != RAIL_STATUS_NO_ERROR) {
    configuredRegion = ZWAVE_REGION_UNDEFINED;
  }
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveGetRegion(args);
}

void zwaveSetNodeId(sl_cli_command_arg_t *args)
{
  RAIL_ZWAVE_NodeId_t nodeId = sl_cli_get_argument_uint16(args, 0);
  RAIL_Status_t status = RAIL_ZWAVE_SetNodeId(railHandle, nodeId);
  responsePrint(sl_cli_get_command_string(args, 0), "Status:%s", status ? "Error" : "Set");
}

void zwaveSetHomeId(sl_cli_command_arg_t *args)
{
  RAIL_ZWAVE_HomeId_t homeId = sl_cli_get_argument_uint32(args, 0);
  RAIL_ZWAVE_HomeId_t homeIdHash = sl_cli_get_argument_uint32(args, 1);
  RAIL_Status_t status = RAIL_ZWAVE_SetHomeId(railHandle, homeId, homeIdHash);
  responsePrint(sl_cli_get_command_string(args, 0), "Status:%s", status ? "Error" : "Set");
}

void zwaveGetBaudRate(sl_cli_command_arg_t *args)
{
  uint16_t channel = -1;
  RAIL_GetChannel(railHandle, &channel);
  if (channel < RAIL_NUM_ZWAVE_CHANNELS
      && configuredRegion < ZWAVE_REGION_UNDEFINED) {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "baudrate:%s",
                  baudrateNames[zwaveRegionTable[configuredRegion].config->baudRate[channel]]);
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "baudrate:Undefined");
  }
}

void zwaveSetLowPowerLevel(sl_cli_command_arg_t *args)
{
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;
  if (!RAIL_ZWAVE_IsEnabled(railHandle)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x26,
                       "Need to enable Z-Wave for this command.");
    return;
  }
  if (sl_cli_get_argument_count(args) >= 2 && strcmp(sl_cli_get_argument_string(args, 1), "raw") == 0) {
    RAIL_TxPowerLevel_t powerLevelRaw = (RAIL_TxPowerLevel_t)sl_cli_get_argument_uint32(args, 0);
    status = RAIL_ZWAVE_SetTxLowPower(railHandle, powerLevelRaw);
  } else {
    RAIL_TxPower_t powerLevelDbm = (RAIL_TxPower_t)sl_cli_get_argument_uint32(args, 0);
    status = RAIL_ZWAVE_SetTxLowPowerDbm(railHandle, powerLevelDbm);
  }
  responsePrint(sl_cli_get_command_string(args, 0), "LowPowerLevel:%s", status ? "Error" : "Set");
}

void zwaveGetLowPowerLevel(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0),
                "powerLevelRaw:%d,powerLeveldBm:%d",
                RAIL_ZWAVE_GetTxLowPower(railHandle),
                RAIL_ZWAVE_GetTxLowPowerDbm(railHandle));
}

void zwaveReceiveBeam(sl_cli_command_arg_t *args)
{
  uint8_t beamDetectIndex;
  RAIL_Status_t status = RAIL_ZWAVE_ReceiveBeam(railHandle, &beamDetectIndex, NULL);
  responsePrint(sl_cli_get_command_string(args, 0),
                "status:%s",
                (status == RAIL_STATUS_NO_ERROR) ? "Success" : "Error");
}

void RAILCb_ZWAVE_BeamFrame(RAIL_Handle_t railHandle)
{
  void *beamPacketHandle = memoryAllocate(sizeof(RailAppEvent_t));
  RailAppEvent_t *beamPacket = (RailAppEvent_t *)memoryPtrFromHandle(beamPacketHandle);
  if (beamPacket == NULL) {
    eventsMissed++;
    return;
  }

  beamPacket->type = BEAM_PACKET;
  if ((RAIL_ZWAVE_GetBeamNodeId(railHandle, &beamPacket->beamPacket.nodeId)
       != RAIL_STATUS_NO_ERROR)) {
    return;
  }

  (void)RAIL_ZWAVE_GetLrBeamTxPower(railHandle, &beamPacket->beamPacket.lrBeamTxPower);
  (void)RAIL_ZWAVE_GetBeamChannelIndex(railHandle, &beamPacket->beamPacket.channelIndex);
  (void)RAIL_ZWAVE_GetBeamRssi(railHandle, &beamPacket->beamPacket.beamRssi);

  queueAdd(&railAppEventQueue, beamPacketHandle);
}

void RAILCb_ZWAVE_LrAckData(RAIL_Handle_t railHandle)
{
  RAIL_ZWAVE_LrAckData_t lrAckData = {
    .noiseFloorDbm = (int8_t)(RAIL_GetRssi(railHandle, false) / 4),
    .txPowerDbm = (int8_t)(RAIL_GetTxPowerDbm(railHandle) / 10),
    .receiveRssiDbm = (int8_t)(RAIL_GetRssi(railHandle, false) / 4) // Ideally should be AGC_FRAMERSSI
  };
  RAIL_ZWAVE_SetLrAckData(railHandle, &lrAckData);
}
#else //!RAIL_FEAT_ZWAVE_SUPPORTED

void zwaveNotSupported(sl_cli_command_arg_t *args)
{
  (void)args;
  responsePrintError(sl_cli_get_command_string(args, 0), 0x56, "Z-Wave not suppported on this chip");
}

void zwaveListRegions(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}

void zwaveStatus(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}

void zwaveEnable(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}

void zwaveConfigureOptions(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}

void zwaveGetRegion(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}

void zwaveSetRegion(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}

void zwaveSetNodeId(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}

void zwaveSetHomeId(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}

void zwaveGetBaudRate(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}

void zwaveSetLowPowerLevel(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}

void zwaveGetLowPowerLevel(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}

void zwaveReceiveBeam(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  zwaveNotSupported(args);
}
#endif //RAIL_FEAT_ZWAVE_SUPPORTED
