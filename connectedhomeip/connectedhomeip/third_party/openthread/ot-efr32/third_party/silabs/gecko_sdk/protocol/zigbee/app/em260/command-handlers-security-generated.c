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

bool emAfProcessEzspCommandSecurity(uint16_t commandId)
{
  switch (commandId) {
//------------------------------------------------------------------------------

    case EZSP_SET_INITIAL_SECURITY_STATE: {
      EmberStatus success;
      EmberInitialSecurityState state;
      fetchEmberInitialSecurityState(&state);
      success = emberSetInitialSecurityState(&state);
      appendInt8u(success);
      break;
    }

    case EZSP_GET_CURRENT_SECURITY_STATE: {
      EmberStatus status;
      EmberCurrentSecurityState state;
      status = emberGetCurrentSecurityState(&state);
      appendInt8u(status);
      appendEmberCurrentSecurityState(&state);
      break;
    }

    case EZSP_GET_KEY: {
      EmberStatus status;
      EmberKeyType keyType;
      EmberKeyStruct keyStruct;
      keyType = fetchInt8u();
      status = emberGetKey(keyType, &keyStruct);
      appendInt8u(status);
      appendEmberKeyStruct(&keyStruct);
      break;
    }

    case EZSP_GET_KEY_TABLE_ENTRY: {
      EmberStatus status;
      uint8_t index;
      EmberKeyStruct keyStruct;
      index = fetchInt8u();
      status = emberGetKeyTableEntry(index, &keyStruct);
      appendInt8u(status);
      appendEmberKeyStruct(&keyStruct);
      break;
    }

    case EZSP_SET_KEY_TABLE_ENTRY: {
      EmberStatus status;
      uint8_t index;
      uint8_t address[8];
      bool linkKey;
      EmberKeyData keyData;
      index = fetchInt8u();
      fetchInt8uArray(8, address);
      linkKey = fetchInt8u();
      fetchEmberKeyData(&keyData);
      status = emberSetKeyTableEntry(index, address, linkKey, &keyData);
      appendInt8u(status);
      break;
    }

    case EZSP_FIND_KEY_TABLE_ENTRY: {
      uint8_t index;
      uint8_t address[8];
      bool linkKey;
      fetchInt8uArray(8, address);
      linkKey = fetchInt8u();
      index = emberFindKeyTableEntry(address, linkKey);
      appendInt8u(index);
      break;
    }

    case EZSP_ADD_OR_UPDATE_KEY_TABLE_ENTRY: {
      EmberStatus status;
      uint8_t address[8];
      bool linkKey;
      EmberKeyData keyData;
      fetchInt8uArray(8, address);
      linkKey = fetchInt8u();
      fetchEmberKeyData(&keyData);
      status = emberAddOrUpdateKeyTableEntry(address, linkKey, &keyData);
      appendInt8u(status);
      break;
    }

    case EZSP_SEND_TRUST_CENTER_LINK_KEY: {
      EmberStatus status;
      EmberNodeId destinationNodeId;
      uint8_t destinationEui64[8];
      destinationNodeId = fetchInt16u();
      fetchInt8uArray(8, destinationEui64);
      status = emberSendTrustCenterLinkKey(destinationNodeId, destinationEui64);
      appendInt8u(status);
      break;
    }

    case EZSP_ERASE_KEY_TABLE_ENTRY: {
      EmberStatus status;
      uint8_t index;
      index = fetchInt8u();
      status = emberEraseKeyTableEntry(index);
      appendInt8u(status);
      break;
    }

    case EZSP_CLEAR_KEY_TABLE: {
      EmberStatus status;
      status = emberClearKeyTable();
      appendInt8u(status);
      break;
    }

    case EZSP_REQUEST_LINK_KEY: {
      EmberStatus status;
      uint8_t partner[8];
      fetchInt8uArray(8, partner);
      status = emberRequestLinkKey(partner);
      appendInt8u(status);
      break;
    }

    case EZSP_UPDATE_TC_LINK_KEY: {
      EmberStatus status;
      uint8_t maxAttempts;
      maxAttempts = fetchInt8u();
      status = emberUpdateTcLinkKey(maxAttempts);
      appendInt8u(status);
      break;
    }

    case EZSP_ADD_TRANSIENT_LINK_KEY: {
      EmberStatus status;
      uint8_t partner[8];
      EmberKeyData transientKey;
      fetchInt8uArray(8, partner);
      fetchEmberKeyData(&transientKey);
      status = emberAfEzspAddTransientLinkKeyCommandCallback(partner, &transientKey);
      appendInt8u(status);
      break;
    }

    case EZSP_CLEAR_TRANSIENT_LINK_KEYS: {
      emberClearTransientLinkKeys();
      break;
    }

    case EZSP_GET_TRANSIENT_LINK_KEY: {
      EmberStatus status;
      uint8_t eui[8];
      EmberTransientKeyData transientKeyData;
      fetchInt8uArray(8, eui);
      status = emberAfEzspGetTransientLinkKeyCommandCallback(eui, &transientKeyData);
      appendInt8u(status);
      appendEmberTransientKeyData(&transientKeyData);
      break;
    }

    case EZSP_GET_TRANSIENT_KEY_TABLE_ENTRY: {
      EmberStatus status;
      uint8_t index;
      EmberTransientKeyData transientKeyData;
      index = fetchInt8u();
      status = emberGetTransientKeyTableEntry(index, &transientKeyData);
      appendInt8u(status);
      appendEmberTransientKeyData(&transientKeyData);
      break;
    }

//------------------------------------------------------------------------------

    default: {
      return false;
    }
  }

  return true;
}
