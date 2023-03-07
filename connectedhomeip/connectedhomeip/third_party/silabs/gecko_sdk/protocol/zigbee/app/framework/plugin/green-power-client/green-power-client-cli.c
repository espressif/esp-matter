/***************************************************************************//**
 * @file
 * @brief CLI for the Green Power Client plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "app/framework/include/af.h"
//#include "green-power-proxy-table.h"
#include "stack/gp/gp-proxy-table.h"
#include "green-power-client.h"

#include "app/util/serial/sl_zigbee_command_interpreter.h"

#ifndef EMBER_AF_GENERATE_CLI
#error The Green Power Client plugin is not compatible with the legacy CLI.
#endif

void emberAfPluginGreenPowerClientSetProxyEntry(SL_CLI_COMMAND_ARG)
{
#ifndef EZSP_HOST
  uint8_t index = emberUnsignedCommandArgument(0);
  uint32_t srcID = emberUnsignedCommandArgument(1);
  uint16_t sinkNodeID = emberUnsignedCommandArgument(2);
  EmberGpProxyTableEntry entry;

  entry.gpd.applicationId = 0;
  entry.gpd.id.sourceId = srcID;
  entry.options = emberUnsignedCommandArgument(3);
  entry.status = EMBER_GP_PROXY_TABLE_ENTRY_STATUS_ACTIVE;
  entry.securityOptions = 0;
  entry.sinkList[0].type = EMBER_GP_SINK_TYPE_LW_UNICAST;
  entry.sinkList[1].type = EMBER_GP_SINK_TYPE_UNUSED;
  entry.sinkList[0].target.unicast.sinkNodeId = sinkNodeID;
  entry.sinkList[0].target.unicast.sinkEUI[0] = 0x84;
  entry.sinkList[0].target.unicast.sinkEUI[1] = 0x40;
  entry.sinkList[0].target.unicast.sinkEUI[2] = 0x18;
  entry.sinkList[0].target.unicast.sinkEUI[3] = 0x00;
  entry.sinkList[0].target.unicast.sinkEUI[4] = 0x00;
  entry.sinkList[0].target.unicast.sinkEUI[5] = 0x00;
  entry.sinkList[0].target.unicast.sinkEUI[6] = 0x00;
  entry.sinkList[0].target.unicast.sinkEUI[7] = 0x00;
  entry.gpdSecurityFrameCounter = 0x00;

  emGpProxyTableSetEntry(index, &entry);
#endif
}

void emberAfPluginGreenPowerClientAddSink(SL_CLI_COMMAND_ARG)
{
#ifndef EZSP_HOST
  EmberGpAddress addr;
  uint32_t srcID = emberUnsignedCommandArgument(0);
  EmberEUI64 eui64;
  emberCopyBigEndianEui64Argument(1, eui64);
  addr.applicationId = 0;
  addr.id.sourceId = srcID;

  uint8_t index = emGpProxyTableFindOrAllocateEntry(&addr);
  if (index != 0xFF) {
    emGpProxyTableAddSink(index,
                          EMBER_GP_SINK_TYPE_LW_UNICAST,
                          eui64,
                          EMBER_UNKNOWN_NODE_ID,
                          0,
                          0xFFFF);
  }
#endif
}
void emberAfPluginGreenPowerClientAddGroupcastSink(SL_CLI_COMMAND_ARG)
{
#ifndef EZSP_HOST
  EmberGpAddress addr;
  uint32_t srcID = emberUnsignedCommandArgument(0);
  uint16_t groupID = (uint16_t) emberUnsignedCommandArgument(1);
  addr.applicationId = 0;
  addr.id.sourceId = srcID;

  uint8_t index = emGpProxyTableFindOrAllocateEntry(&addr);
  if (index != 0xFF) {
    emGpProxyTableAddSink(index,
                          EMBER_GP_SINK_TYPE_GROUPCAST,
                          0,
                          EMBER_UNKNOWN_NODE_ID,
                          groupID,
                          0xFFFF);
  }
#endif
}

void emberAfPluginGreenPowerClientRemoveProxyTableEntry(SL_CLI_COMMAND_ARG)
{
#ifndef EZSP_HOST
  EmberGpAddress addr;
  uint32_t srcID = emberUnsignedCommandArgument(0);
  addr.applicationId = 0;
  addr.id.sourceId = srcID;
  emGpProxyTableRemoveEntry(emberGpProxyTableLookup(&addr));
#endif
}

void emberAfPluginGreenPowerClientPrintProxyTable(SL_CLI_COMMAND_ARG)
{
#ifndef EZSP_HOST
  uint8_t i, j;
  uint8_t cleared = 0;
  emberAfGreenPowerClusterPrint("Proxy Table:\n");
  for (i = 0; i < emGpProxyTableSize; i++) {
    if (emGpProxyTableEntryInUse(i)) {
      emberAfCorePrint("%d opt:%4x seco:%x srcID:%4x ", i, emGpProxyTable[i].options, emGpProxyTable[i].securityOptions, emGpProxyTable[i].gpd.id.sourceId);
      for (j = 0; j < 2; j++) {
        if (emGpProxyTable[i].sinkList[j].type == EMBER_GP_SINK_TYPE_UNUSED) {
          emberAfCorePrint("unused");
        } else if (emGpProxyTable[i].sinkList[j].type == EMBER_GP_SINK_TYPE_GROUPCAST) {
          emberAfCorePrint("GC %2x", emGpProxyTable[i].sinkList[j].target.groupcast.groupID);
        } else if (emGpProxyTable[i].sinkList[j].type == EMBER_GP_SINK_TYPE_LW_UNICAST) {
          emberAfCorePrint("LU:");
          emberAfPrintBigEndianEui64(emGpProxyTable[i].sinkList[j].target.unicast.sinkEUI);
        }
        emberAfCorePrint(" ");
      }
      emberAfCorePrint(" ");
      for (j = 0; j < EMBER_ENCRYPTION_KEY_SIZE; j++) {
        emberAfCorePrint("%x", emGpProxyTable[i].gpdKey.contents[j]);
      }
      emberAfCorePrint(" ");
      emberAfCorePrint("%x", emGpProxyTable[i].gpdSecurityFrameCounter);
      emberAfCorePrint("\n");
    } else {
      cleared++;
    }
  }
  if (cleared == emGpProxyTableSize) {
    emberAfCorePrintln("No Proxy Entries");
  }
#else

  EmberGpProxyTableEntry entry;
  uint8_t i, j;
  emberAfGreenPowerClusterPrint("Proxy Table:\n");
  for (i = 0; i < 5; i++) { //TODO: set the size properly
    emberGpProxyTableGetEntry(i, &entry);
    if (entry.status == EMBER_GP_PROXY_TABLE_ENTRY_STATUS_ACTIVE) {
      emberAfCorePrint("%d opt:%4x seco:%x srcID:%4x ", i, entry.options, entry.securityOptions, entry.gpd.id.sourceId);
      for (j = 0; j < 2; j++) {
        if (entry.sinkList[j].type == EMBER_GP_SINK_TYPE_UNUSED) {
          emberAfCorePrint("unused");
        } else if (entry.sinkList[j].type == EMBER_GP_SINK_TYPE_GROUPCAST) {
          emberAfCorePrint("GC %2x", entry.sinkList[j].target.groupcast.groupID);
        } else if (entry.sinkList[j].type == EMBER_GP_SINK_TYPE_LW_UNICAST) {
          emberAfCorePrint("LU:");
          emberAfPrintBigEndianEui64(entry.sinkList[j].target.unicast.sinkEUI);
        }
        emberAfCorePrint(" ");
      }
      emberAfCorePrint(" ");
      for (j = 0; j < EMBER_ENCRYPTION_KEY_SIZE; j++) {
        emberAfCorePrint("%x", entry.gpdKey.contents[j]);
      }
      emberAfCorePrint(" ");
      emberAfCorePrint("%x", entry.gpdSecurityFrameCounter);
      emberAfCorePrint("\n");
    }
  }

#endif
}

void emberAfPluginGreenPowerClientDuplicateFilteringTest(SL_CLI_COMMAND_ARG)
{
#ifndef EZSP_HOST
  EmberGpAddress sender;
  sender.endpoint = (uint8_t) emberUnsignedCommandArgument(0);
  sender.applicationId = EMBER_GP_APPLICATION_SOURCE_ID;
  sender.id.sourceId = emberUnsignedCommandArgument(1);
  emGpMessageChecking(&sender, (uint8_t) emberUnsignedCommandArgument(2));
  //emAfGreenPowerFindDuplicateMacSeqNum(&sender, (uint8_t)emberUnsignedCommandArgument(2));
#endif
}

void emberAfPluginGreenPowerClientSetKey(SL_CLI_COMMAND_ARG)
{
#ifndef EZSP_HOST
  uint8_t index;
  EmberKeyData keyData;
  index = emberUnsignedCommandArgument(0);
  emberCopyKeyArgument(1, &keyData);
  emGpProxyTableSetKey(index, (keyData.contents), 0 /*key type TODO*/);
#endif
}
