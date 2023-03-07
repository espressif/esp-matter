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

bool emAfProcessEzspCommandTrustCenter(uint16_t commandId)
{
  switch (commandId) {
//------------------------------------------------------------------------------

    case EZSP_BROADCAST_NEXT_NETWORK_KEY: {
      EmberStatus status;
      EmberKeyData key;
      fetchEmberKeyData(&key);
      status = emberBroadcastNextNetworkKey(&key);
      appendInt8u(status);
      break;
    }

    case EZSP_BROADCAST_NETWORK_KEY_SWITCH: {
      EmberStatus status;
      status = emberBroadcastNetworkKeySwitch();
      appendInt8u(status);
      break;
    }

    case EZSP_BECOME_TRUST_CENTER: {
      EmberStatus status;
      EmberKeyData newNetworkKey;
      fetchEmberKeyData(&newNetworkKey);
      status = emberBecomeTrustCenter(&newNetworkKey);
      appendInt8u(status);
      break;
    }

    case EZSP_AES_MMO_HASH: {
      EmberStatus status;
      EmberAesMmoHashContext context;
      bool finalize;
      uint8_t length;
      uint8_t *data;
      EmberAesMmoHashContext returnContext;
      fetchEmberAesMmoHashContext(&context);
      finalize = fetchInt8u();
      length = fetchInt8u();
      data = (uint8_t *)fetchInt8uPointer(length);
      status = emberAfEzspAesMmoHashCommandCallback(&context, finalize, length, data, &returnContext);
      appendInt8u(status);
      appendEmberAesMmoHashContext(&returnContext);
      break;
    }

    case EZSP_REMOVE_DEVICE: {
      EmberStatus status;
      EmberNodeId destShort;
      uint8_t destLong[8];
      uint8_t targetLong[8];
      destShort = fetchInt16u();
      fetchInt8uArray(8, destLong);
      fetchInt8uArray(8, targetLong);
      status = emberRemoveDevice(destShort, destLong, targetLong);
      appendInt8u(status);
      break;
    }

    case EZSP_UNICAST_NWK_KEY_UPDATE: {
      EmberStatus status;
      EmberNodeId destShort;
      uint8_t destLong[8];
      EmberKeyData key;
      destShort = fetchInt16u();
      fetchInt8uArray(8, destLong);
      fetchEmberKeyData(&key);
      status = emberUnicastNwkKeyUpdate(destShort, destLong, &key);
      appendInt8u(status);
      break;
    }

//------------------------------------------------------------------------------

    default: {
      return false;
    }
  }

  return true;
}
