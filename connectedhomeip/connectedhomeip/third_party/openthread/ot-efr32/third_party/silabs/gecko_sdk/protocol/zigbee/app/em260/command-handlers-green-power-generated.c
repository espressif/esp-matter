/*****************************************************************************/
/**
 * Copyright 2021 Silicon Laboratories, Inc.
 *
 *****************************************************************************/
//
// *** Generated file. Do not edit! ***
//
// Description: Handlers for the EZSP frames that directly correspond to Ember
// API calls.

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "ezsp-enum.h"
#include "secure-ezsp-types.h"
#include "app/em260/command-context.h"
#include "stack/include/cbke-crypto-engine.h"
#include "stack/include/mfglib.h"
#include "stack/include/binding-table.h"
#include "stack/include/message.h"
#include "app/util/ezsp/ezsp-frame-utilities.h"
#include "app/em260/command-handlers-cbke.h"
#include "app/em260/command-handlers-binding.h"
#include "app/em260/command-handlers-mfglib.h"
#include "app/em260/command-handlers-security.h"
#include "app/em260/command-handlers-zll.h"
#include "app/em260/command-handlers-zigbee-pro.h"
#include "child.h"
#include "message.h"
#include "zll-api.h"
#include "security.h"
#include "stack-info.h"
#include "network-formation.h"
#include "zigbee-device-stack.h"
#include "ember-duty-cycle.h"
#include "multi-phy.h"

bool emAfProcessEzspCommandGreenPower(uint16_t commandId)
{
  switch (commandId) {
//------------------------------------------------------------------------------

    case EZSP_GP_PROXY_TABLE_PROCESS_GP_PAIRING: {
      bool gpPairingAdded;
      uint32_t options;
      EmberGpAddress addr;
      uint8_t commMode;
      uint16_t sinkNetworkAddress;
      uint16_t sinkGroupId;
      uint16_t assignedAlias;
      uint8_t sinkIeeeAddress[8];
      EmberKeyData gpdKey;
      uint32_t gpdSecurityFrameCounter;
      uint8_t forwardingRadius;
      options = fetchInt32u();
      fetchEmberGpAddress(&addr);
      commMode = fetchInt8u();
      sinkNetworkAddress = fetchInt16u();
      sinkGroupId = fetchInt16u();
      assignedAlias = fetchInt16u();
      fetchInt8uArray(8, sinkIeeeAddress);
      fetchEmberKeyData(&gpdKey);
      gpdSecurityFrameCounter = fetchInt32u();
      forwardingRadius = fetchInt8u();
      gpPairingAdded = emberGpProxyTableProcessGpPairing(options, &addr, commMode, sinkNetworkAddress, sinkGroupId, assignedAlias, sinkIeeeAddress, &gpdKey, gpdSecurityFrameCounter, forwardingRadius);
      appendInt8u(gpPairingAdded);
      break;
    }

    case EZSP_D_GP_SEND: {
      EmberStatus status;
      bool action;
      bool useCca;
      EmberGpAddress addr;
      uint8_t gpdCommandId;
      uint8_t gpdAsduLength;
      uint8_t *gpdAsdu;
      uint8_t gpepHandle;
      uint16_t gpTxQueueEntryLifetimeMs;
      action = fetchInt8u();
      useCca = fetchInt8u();
      fetchEmberGpAddress(&addr);
      gpdCommandId = fetchInt8u();
      gpdAsduLength = fetchInt8u();
      gpdAsdu = (uint8_t *)fetchInt8uPointer(gpdAsduLength);
      gpepHandle = fetchInt8u();
      gpTxQueueEntryLifetimeMs = fetchInt16u();
      status = emberDGpSend(action, useCca, &addr, gpdCommandId, gpdAsduLength, gpdAsdu, gpepHandle, gpTxQueueEntryLifetimeMs);
      appendInt8u(status);
      break;
    }

    case EZSP_GP_PROXY_TABLE_GET_ENTRY: {
      EmberStatus status;
      uint8_t proxyIndex;
      EmberGpProxyTableEntry entry;
      proxyIndex = fetchInt8u();
      status = emberGpProxyTableGetEntry(proxyIndex, &entry);
      appendInt8u(status);
      appendEmberGpProxyTableEntry(&entry);
      break;
    }

    case EZSP_GP_PROXY_TABLE_LOOKUP: {
      uint8_t index;
      EmberGpAddress addr;
      fetchEmberGpAddress(&addr);
      index = emberGpProxyTableLookup(&addr);
      appendInt8u(index);
      break;
    }

    case EZSP_GP_SINK_TABLE_GET_ENTRY: {
      EmberStatus status;
      uint8_t sinkIndex;
      EmberGpSinkTableEntry entry;
      sinkIndex = fetchInt8u();
      status = emberGpSinkTableGetEntry(sinkIndex, &entry);
      appendInt8u(status);
      appendEmberGpSinkTableEntry(&entry);
      break;
    }

    case EZSP_GP_SINK_TABLE_LOOKUP: {
      uint8_t index;
      EmberGpAddress addr;
      fetchEmberGpAddress(&addr);
      index = emberGpSinkTableLookup(&addr);
      appendInt8u(index);
      break;
    }

    case EZSP_GP_SINK_TABLE_SET_ENTRY: {
      EmberStatus status;
      uint8_t sinkIndex;
      EmberGpSinkTableEntry entry;
      sinkIndex = fetchInt8u();
      fetchEmberGpSinkTableEntry(&entry);
      status = emberGpSinkTableSetEntry(sinkIndex, &entry);
      appendInt8u(status);
      break;
    }

    case EZSP_GP_SINK_TABLE_REMOVE_ENTRY: {
      uint8_t sinkIndex;
      sinkIndex = fetchInt8u();
      emberGpSinkTableRemoveEntry(sinkIndex);
      break;
    }

    case EZSP_GP_SINK_TABLE_FIND_OR_ALLOCATE_ENTRY: {
      uint8_t index;
      EmberGpAddress addr;
      fetchEmberGpAddress(&addr);
      index = emberGpSinkTableFindOrAllocateEntry(&addr);
      appendInt8u(index);
      break;
    }

    case EZSP_GP_SINK_TABLE_CLEAR_ALL: {
      emberGpSinkTableClearAll();
      break;
    }

    case EZSP_GP_SINK_TABLE_INIT: {
      emberGpSinkTableInit();
      break;
    }

    case EZSP_GP_SINK_TABLE_SET_SECURITY_FRAME_COUNTER: {
      uint8_t index;
      uint32_t sfc;
      index = fetchInt8u();
      sfc = fetchInt32u();
      emberGpSinkTableSetSecurityFrameCounter(index, sfc);
      break;
    }

//------------------------------------------------------------------------------

    default: {
      return false;
    }
  }

  return true;
}
