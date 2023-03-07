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

bool emAfProcessEzspCommandZll(uint16_t commandId)
{
  switch (commandId) {
//------------------------------------------------------------------------------

    case EZSP_ZLL_NETWORK_OPS: {
      EmberStatus status;
      EmberZllNetwork networkInfo;
      EzspZllNetworkOperation op;
      int8_t radioTxPower;
      fetchEmberZllNetwork(&networkInfo);
      op = fetchInt8u();
      radioTxPower = fetchInt8u();
      status = emberAfEzspZllNetworkOpsCommandCallback(&networkInfo, op, radioTxPower);
      appendInt8u(status);
      break;
    }

    case EZSP_ZLL_SET_INITIAL_SECURITY_STATE: {
      EmberStatus status;
      EmberKeyData networkKey;
      EmberZllInitialSecurityState securityState;
      fetchEmberKeyData(&networkKey);
      fetchEmberZllInitialSecurityState(&securityState);
      status = emberZllSetInitialSecurityState(&networkKey, &securityState);
      appendInt8u(status);
      break;
    }

    case EZSP_ZLL_SET_SECURITY_STATE_WITHOUT_KEY: {
      EmberStatus status;
      EmberZllInitialSecurityState securityState;
      fetchEmberZllInitialSecurityState(&securityState);
      status = emberZllSetSecurityStateWithoutKey(&securityState);
      appendInt8u(status);
      break;
    }

    case EZSP_ZLL_START_SCAN: {
      EmberStatus status;
      uint32_t channelMask;
      int8_t radioPowerForScan;
      EmberNodeType nodeType;
      channelMask = fetchInt32u();
      radioPowerForScan = fetchInt8u();
      nodeType = fetchInt8u();
      status = emberZllStartScan(channelMask, radioPowerForScan, nodeType);
      appendInt8u(status);
      break;
    }

    case EZSP_ZLL_SET_RX_ON_WHEN_IDLE: {
      EmberStatus status;
      uint32_t durationMs;
      durationMs = fetchInt32u();
      status = emberZllSetRxOnWhenIdle(durationMs);
      appendInt8u(status);
      break;
    }

    case EZSP_ZLL_GET_TOKENS: {
      EmberTokTypeStackZllData data;
      EmberTokTypeStackZllSecurity security;
      emberZllGetTokens(&data, &security);
      appendEmberTokTypeStackZllData(&data);
      appendEmberTokTypeStackZllSecurity(&security);
      break;
    }

    case EZSP_ZLL_SET_DATA_TOKEN: {
      EmberTokTypeStackZllData data;
      fetchEmberTokTypeStackZllData(&data);
      emberZllSetDataToken(&data);
      break;
    }

    case EZSP_ZLL_SET_NON_ZLL_NETWORK: {
      emberZllSetNonZllNetwork();
      break;
    }

    case EZSP_IS_ZLL_NETWORK: {
      bool isZllNetwork;
      isZllNetwork = emberIsZllNetwork();
      appendInt8u(isZllNetwork);
      break;
    }

    case EZSP_ZLL_SET_RADIO_IDLE_MODE: {
      EmberRadioPowerMode mode;
      mode = fetchInt8u();
      emberZllSetRadioIdleMode(mode);
      break;
    }

    case EZSP_ZLL_GET_RADIO_IDLE_MODE: {
      uint8_t radioIdleMode;
      radioIdleMode = emberZllGetRadioIdleMode();
      appendInt8u(radioIdleMode);
      break;
    }

    case EZSP_SET_ZLL_NODE_TYPE: {
      EmberNodeType nodeType;
      nodeType = fetchInt8u();
      emberSetZllNodeType(nodeType);
      break;
    }

    case EZSP_SET_ZLL_ADDITIONAL_STATE: {
      uint16_t state;
      state = fetchInt16u();
      emberSetZllAdditionalState(state);
      break;
    }

    case EZSP_ZLL_OPERATION_IN_PROGRESS: {
      bool zllOperationInProgress;
      zllOperationInProgress = emberZllOperationInProgress();
      appendInt8u(zllOperationInProgress);
      break;
    }

    case EZSP_ZLL_RX_ON_WHEN_IDLE_GET_ACTIVE: {
      bool zllRxOnWhenIdleGetActive;
      zllRxOnWhenIdleGetActive = emberZllRxOnWhenIdleGetActive();
      appendInt8u(zllRxOnWhenIdleGetActive);
      break;
    }

    case EZSP_ZLL_SCANNING_COMPLETE: {
      emberZllScanningComplete();
      break;
    }

    case EZSP_GET_ZLL_PRIMARY_CHANNEL_MASK: {
      uint32_t zllPrimaryChannelMask;
      zllPrimaryChannelMask = emberGetZllPrimaryChannelMask();
      appendInt32u(zllPrimaryChannelMask);
      break;
    }

    case EZSP_GET_ZLL_SECONDARY_CHANNEL_MASK: {
      uint32_t zllSecondaryChannelMask;
      zllSecondaryChannelMask = emberGetZllSecondaryChannelMask();
      appendInt32u(zllSecondaryChannelMask);
      break;
    }

    case EZSP_SET_ZLL_PRIMARY_CHANNEL_MASK: {
      uint32_t zllPrimaryChannelMask;
      zllPrimaryChannelMask = fetchInt32u();
      emberSetZllPrimaryChannelMask(zllPrimaryChannelMask);
      break;
    }

    case EZSP_SET_ZLL_SECONDARY_CHANNEL_MASK: {
      uint32_t zllSecondaryChannelMask;
      zllSecondaryChannelMask = fetchInt32u();
      emberSetZllSecondaryChannelMask(zllSecondaryChannelMask);
      break;
    }

    case EZSP_ZLL_CLEAR_TOKENS: {
      emberZllClearTokens();
      break;
    }

//------------------------------------------------------------------------------

    default: {
      return false;
    }
  }

  return true;
}
