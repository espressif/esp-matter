/***************************************************************************//**
 * @file
 * @brief Source file for RAIL Ram Modem Reconfiguration functionality
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

#include <stdlib.h>
#include <stdint.h>

#include "rail.h"
#include "em_core.h"
#include "em_device.h"
#include "sli_rail_util_callbacks.h"

#include "railapp_rmr.h"
#include "railapp_malloc.h"
#include "app_common.h"
#include "response_print.h"

typedef struct RMR_State{
  uint32_t phyInfo[RMR_PHY_INFO_LEN];
  uint8_t irCalConfig[RMR_IRCAL_LEN];
  uint32_t modemConfigEntry[RMR_MODEM_CONFIG_LEN];
  RAIL_FrameType_t frameTypeConfig;
  uint16_t frameLenList[RMR_FRAME_LENGTH_LIST_LEN];
  uint32_t frameCodingTable[RMR_FRAME_CODING_TABLE_LEN];
  RAIL_ChannelConfigEntryAttr_t generatedEntryAttr;
  RAIL_ChannelConfigEntry_t generatedChannels[1];
  __ALIGNED(4) uint8_t convDecodeBuffer[RMR_CONV_DECODE_BUFFER_LEN];
  RAIL_ChannelConfig_t channelConfig;
  uint8_t dcdcRetimingConfig[RMR_DCDC_RETIMING_LEN];
  uint8_t hfxoRetimingConfig[RMR_HFXO_RETIMING_LEN];
  uint32_t rffpllConfig[RMR_RFFPLL_CONFIG_LEN];
  uint8_t txIrCalConfig[RMR_TXIRCAL_CONFIG_LEN];
} RMR_State_t;

static RMR_State_t *rmrState = NULL;

// Internal commands
RAIL_Status_t Rmr_writeRmrStructure(RAIL_RMR_StructureIndex_t structure, uint16_t offset, uint8_t count, uint8_t *dataPtr);
RAIL_Status_t Rmr_updateConfigurationPointer(uint8_t structToModify, uint16_t offset, uint8_t structToPointTo);
RAIL_Status_t Rmr_reconfigureModem(RAIL_Handle_t railHandle);

//----------------------------------------------------------------------------
// Ram Modem Reconfiguration Commands
//-----------------------------------------------------------------------------
RAIL_Status_t Rmr_updateConfigurationPointer(uint8_t structToModify, uint16_t offset, uint8_t structToPointTo)
{
  uint32_t structPointer = 0u; // NULL
  // First get the addres of the structure we are trying to reference
  switch (structToPointTo) {
    case (RMR_STRUCT_PHY_INFO): {
      structPointer = (uint32_t)&(rmrState->phyInfo);
      break;
    }
    case (RMR_STRUCT_IRCAL_CONFIG): {
      structPointer = (uint32_t)&(rmrState->irCalConfig);
      break;
    }
    case (RMR_STRUCT_FRAME_TYPE_CONFIG): {
      structPointer = (uint32_t)&(rmrState->frameTypeConfig);
      break;
    }
    case (RMR_STRUCT_FRAME_LENGTH_LIST): {
      structPointer = (uint32_t)&(rmrState->frameLenList);
      break;
    }
    case (RMR_STRUCT_FRAME_CODING_TABLE): {
      structPointer = (uint32_t)&(rmrState->frameCodingTable);
      break;
    }
    case (RMR_STRUCT_CONV_DECODE_BUFFER): {
      structPointer = (uint32_t)&(rmrState->convDecodeBuffer);
      break;
    }
    case (RMR_STRUCT_NULL): {
      structPointer = 0u; // NULL
      break;
    }
    case (RMR_STRUCT_DCDC_RETIMING_CONFIG): {
      structPointer = (uint32_t)&(rmrState->dcdcRetimingConfig);
      break;
    }
    case (RMR_STRUCT_HFXO_RETIMING_CONFIG): {
      structPointer = (uint32_t)&(rmrState->hfxoRetimingConfig);
      break;
    }
    case (RMR_STRUCT_RFFPLL_CONFIG): {
      structPointer = (uint32_t)&(rmrState->rffpllConfig);
      break;
    }
    case (RMR_STRUCT_TXIRCAL_CONFIG): {
      structPointer = (uint32_t)&(rmrState->txIrCalConfig);
      break;
    }
    default: {
      // Error, unrecognized structure
      return RAIL_STATUS_INVALID_PARAMETER;
    }
  }
  switch (structToModify) {
    case (RMR_STRUCT_MODEM_CONFIG): {
      if (offset >= RMR_MODEM_CONFIG_LEN) {
        return RAIL_STATUS_INVALID_PARAMETER;
      }
      rmrState->modemConfigEntry[offset] = structPointer;
      break;
    }
    case (RMR_STRUCT_PHY_INFO): {
      if (offset >= RMR_PHY_INFO_LEN) {
        return RAIL_STATUS_INVALID_PARAMETER;
      }
      rmrState->phyInfo[offset] = structPointer;
      break;
    }
    case (RMR_STRUCT_FRAME_TYPE_CONFIG): {
      if (offset != 0) {
        return RAIL_STATUS_INVALID_PARAMETER;
      }
      rmrState->frameTypeConfig.frameLen = (uint16_t *)structPointer;
      break;
    }
    default: {
      // Error unrecognized structure
      return RAIL_STATUS_INVALID_PARAMETER;
    }
  }
  return RAIL_STATUS_NO_ERROR;
}

RAIL_Status_t Rmr_reconfigureModem(RAIL_Handle_t railHandle)
{
  RAIL_RadioState_t currentState = RAIL_GetRadioState(railHandle);
  if (currentState != RAIL_RF_STATE_IDLE) {
    return RAIL_STATUS_INVALID_STATE;
  }
  disableIncompatibleProtocols(RAIL_PTI_PROTOCOL_CUSTOM);

  // Always include frame type length functionality when using RMR or config
  // channels won't work for frame type based lengths.
  RAIL_IncludeFrameTypeLength(railHandle);

  // Configure with the downloaded channel configuration.
  RAIL_ConfigChannels(railHandle, &rmrState->channelConfig, &sli_rail_util_on_channel_config_change);

  // Make sure that we stay in idle after the reconfiguration.
  RAIL_Idle(railHandle, RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS, false);

  return RAIL_STATUS_NO_ERROR;
}

//-----------------------------------------------------------------------------
// RMR CI Commands
//-----------------------------------------------------------------------------
RAIL_Status_t Rmr_writeRmrStructure(RAIL_RMR_StructureIndex_t structure, uint16_t offset, uint8_t count, uint8_t *dataPtr)
{
  uint8_t *targetStruct;
  uint32_t size;
  switch (structure) {
    case (RMR_STRUCT_PHY_INFO): {
      size = sizeof(rmrState->phyInfo);
      targetStruct = (uint8_t *) &(rmrState->phyInfo);
      break;
    }
    case (RMR_STRUCT_IRCAL_CONFIG): {
      size = sizeof(rmrState->irCalConfig);
      targetStruct = (uint8_t *) &(rmrState->irCalConfig);
      break;
    }
    case (RMR_STRUCT_MODEM_CONFIG): {
      size = sizeof(rmrState->modemConfigEntry);
      targetStruct = (uint8_t *) &(rmrState->modemConfigEntry);
      break;
    }
    case (RMR_STRUCT_FRAME_TYPE_CONFIG): {
      size = sizeof(rmrState->frameTypeConfig);
      targetStruct = (uint8_t *) &(rmrState->frameTypeConfig);
      break;
    }
    case (RMR_STRUCT_FRAME_LENGTH_LIST): {
      size = sizeof(rmrState->frameLenList);
      targetStruct = (uint8_t *) &(rmrState->frameLenList);
      break;
    }
    case (RMR_STRUCT_FRAME_CODING_TABLE): {
      size = RMR_FRAME_CODING_TABLE_LEN * sizeof(uint32_t);
      targetStruct = (uint8_t *) &(rmrState->frameCodingTable);
      break;
    }
    case (RMR_STRUCT_CHANNEL_CONFIG_ATTRIBUTES): {
      size = sizeof(rmrState->generatedEntryAttr);
      targetStruct = (uint8_t *) &(rmrState->generatedEntryAttr);
      break;
    }
    case (RMR_STRUCT_CHANNEL_CONFIG_ENTRY): {
      size = sizeof(rmrState->generatedChannels);
      targetStruct = (uint8_t *) &(rmrState->generatedChannels);
      break;
    }
    case (RMR_STRUCT_DCDC_RETIMING_CONFIG): {
      size = sizeof(rmrState->dcdcRetimingConfig);
      targetStruct = (uint8_t *) &(rmrState->dcdcRetimingConfig);
      break;
    }
    case (RMR_STRUCT_HFXO_RETIMING_CONFIG): {
      size = sizeof(rmrState->hfxoRetimingConfig);
      targetStruct = (uint8_t *) &(rmrState->hfxoRetimingConfig);
      break;
    }
    case (RMR_STRUCT_RFFPLL_CONFIG): {
      size = sizeof(rmrState->rffpllConfig);
      targetStruct = (uint8_t *) &(rmrState->rffpllConfig);
      break;
    }
    case (RMR_STRUCT_TXIRCAL_CONFIG): {
      size = sizeof(rmrState->txIrCalConfig);
      targetStruct = (uint8_t *) &(rmrState->txIrCalConfig);
      break;
    }
    default: {
      return RAIL_STATUS_INVALID_PARAMETER;
      break;
    }
  }
  // Check that we are not writing out of bounds
  if ((offset + count) > size) {
    return RAIL_STATUS_INVALID_PARAMETER;
  }
  targetStruct += offset;
  while (count-- != 0u) {
    *targetStruct = *dataPtr;
    targetStruct++;
    dataPtr++;
  }
  return RAIL_STATUS_NO_ERROR;
}

static bool rmrInit(sl_cli_command_arg_t *args)
{
  if (rmrState == NULL) {
    rmrState = RAILAPP_Malloc(sizeof(RMR_State_t));
    if (rmrState == NULL) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x86, "Error allocating RMR memory.");
      return false;
    }
    rmrState->channelConfig.phyConfigBase = &(rmrState->modemConfigEntry[0]);
    rmrState->channelConfig.phyConfigDeltaSubtract = NULL;
    rmrState->channelConfig.configs = rmrState->generatedChannels;
    rmrState->channelConfig.length = 1;
    rmrState->channelConfig.signature = 0U;
    rmrState->generatedChannels[0].phyConfigDeltaAdd = NULL;
    rmrState->generatedChannels[0].attr = &rmrState->generatedEntryAttr;
  }
  return true;
}

void CI_printRmrStructureLocations(sl_cli_command_arg_t *args)
{
  if (!rmrInit(args)) {
    return;
  }

  responsePrintHeader(sl_cli_get_command_string(args, 0), "Id:%u,Address:0x%x,Size:%u");
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_PHY_INFO,
                     (uint8_t *) &(rmrState->phyInfo),
                     sizeof(rmrState->phyInfo));
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_IRCAL_CONFIG,
                     (uint8_t *) &(rmrState->irCalConfig),
                     sizeof(rmrState->irCalConfig));
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_MODEM_CONFIG,
                     (uint8_t *) &(rmrState->modemConfigEntry),
                     sizeof(rmrState->modemConfigEntry));
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_FRAME_TYPE_CONFIG,
                     (uint8_t *) &(rmrState->frameTypeConfig),
                     sizeof(rmrState->frameTypeConfig));
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_FRAME_LENGTH_LIST,
                     (uint8_t *) &(rmrState->frameLenList),
                     sizeof(rmrState->frameLenList));
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_FRAME_CODING_TABLE,
                     (uint8_t *) &(rmrState->frameCodingTable),
                     sizeof(rmrState->frameCodingTable));
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_CHANNEL_CONFIG_ATTRIBUTES,
                     (uint8_t *) &(rmrState->generatedEntryAttr),
                     sizeof(rmrState->generatedEntryAttr));
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_CHANNEL_CONFIG_ENTRY,
                     (uint8_t *) &(rmrState->generatedChannels),
                     sizeof(rmrState->generatedChannels));
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_DCDC_RETIMING_CONFIG,
                     (uint8_t *) &(rmrState->dcdcRetimingConfig),
                     sizeof(rmrState->dcdcRetimingConfig));
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_HFXO_RETIMING_CONFIG,
                     (uint8_t *) &(rmrState->hfxoRetimingConfig),
                     sizeof(rmrState->hfxoRetimingConfig));
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_RFFPLL_CONFIG,
                     (uint8_t *) &(rmrState->rffpllConfig),
                     sizeof(rmrState->rffpllConfig));
  responsePrintMulti("Id:%u,Address:0x%x,Size:%u",
                     RMR_STRUCT_TXIRCAL_CONFIG,
                     (uint8_t *) &(rmrState->txIrCalConfig),
                     sizeof(rmrState->txIrCalConfig));
}

void CI_writeRmrStructure(sl_cli_command_arg_t *args)
{
  uint8_t count = sl_cli_get_argument_uint8(args, RMR_CI_COUNT);
  uint8_t bufferedData[RMR_ARGUMENT_BUFFER_SIZE];
  if (!rmrInit(args)) {
    return;
  }
  if (sl_cli_get_argument_count(args) != (count + RMR_CI_DATA_START)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x80, "Argument count does not match number of arguments.");
    return;
  }
  if (count > RMR_ARGUMENT_BUFFER_SIZE) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x81, "Number of arguments greater than local buffer.");
    return;
  }
  RAIL_RMR_StructureIndex_t structure = sl_cli_get_argument_uint8(args, RMR_CI_RMR_STRUCTURE);
  uint16_t offset = sl_cli_get_argument_uint16(args, RMR_CI_OFFSET);
  for (uint8_t i = 0; i < count; i++) {
    bufferedData[i] = sl_cli_get_argument_uint8(args, RMR_CI_DATA_START + i);
  }
  if (Rmr_writeRmrStructure(structure, offset, count, bufferedData) != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x82, "Error writing to structure.");
    return;
  }
  responsePrint(sl_cli_get_command_string(args, 0), "CommandStatus:Success");
  return;
}

void CI_updateConfigurationPointer(sl_cli_command_arg_t *args)
{
  if (!rmrInit(args)) {
    return;
  }
  if (sl_cli_get_argument_count(args) != 3) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x83, "Incorrect number of arguments");
    return;
  }
  uint8_t structure = sl_cli_get_argument_uint8(args, 0);
  uint16_t location = sl_cli_get_argument_uint16(args, 1);
  uint8_t pointer = sl_cli_get_argument_uint8(args, 2);
  if (Rmr_updateConfigurationPointer(structure, location, pointer) != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x84, "Error updating structure");
    return;
  }
  responsePrint(sl_cli_get_command_string(args, 0), "CommandStatus:Success");
}

void CI_reconfigureModem(sl_cli_command_arg_t *args)
{
  if (!rmrInit(args)) {
    return;
  }
  if (Rmr_reconfigureModem(railHandle) == RAIL_STATUS_NO_ERROR) {
    responsePrint(sl_cli_get_command_string(args, 0), "CommandStatus:Success");
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x85, "Need to be in Idle radio state for this command");
  }
}
