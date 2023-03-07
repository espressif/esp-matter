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

bool emAfProcessEzspCommandBinding(uint16_t commandId)
{
  switch (commandId) {
//------------------------------------------------------------------------------

    case EZSP_CLEAR_BINDING_TABLE: {
      EmberStatus status;
      status = emberClearBindingTable();
      appendInt8u(status);
      break;
    }

    case EZSP_SET_BINDING: {
      EmberStatus status;
      uint8_t index;
      EmberBindingTableEntry value;
      index = fetchInt8u();
      fetchEmberBindingTableEntry(&value);
      status = emberAfEzspSetBindingCommandCallback(index, &value);
      appendInt8u(status);
      break;
    }

    case EZSP_GET_BINDING: {
      EmberStatus status;
      uint8_t index;
      EmberBindingTableEntry value;
      index = fetchInt8u();
      status = emberGetBinding(index, &value);
      appendInt8u(status);
      appendEmberBindingTableEntry(&value);
      break;
    }

    case EZSP_DELETE_BINDING: {
      EmberStatus status;
      uint8_t index;
      index = fetchInt8u();
      status = emberAfEzspDeleteBindingCommandCallback(index);
      appendInt8u(status);
      break;
    }

    case EZSP_BINDING_IS_ACTIVE: {
      bool active;
      uint8_t index;
      index = fetchInt8u();
      active = emberBindingIsActive(index);
      appendInt8u(active);
      break;
    }

    case EZSP_GET_BINDING_REMOTE_NODE_ID: {
      EmberNodeId nodeId;
      uint8_t index;
      index = fetchInt8u();
      nodeId = emberGetBindingRemoteNodeId(index);
      appendInt16u(nodeId);
      break;
    }

    case EZSP_SET_BINDING_REMOTE_NODE_ID: {
      uint8_t index;
      EmberNodeId nodeId;
      index = fetchInt8u();
      nodeId = fetchInt16u();
      emberSetBindingRemoteNodeId(index, nodeId);
      break;
    }

//------------------------------------------------------------------------------

    default: {
      return false;
    }
  }

  return true;
}
