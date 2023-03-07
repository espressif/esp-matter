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

bool emAfProcessEzspCommandMessaging(uint16_t commandId)
{
  switch (commandId) {
//------------------------------------------------------------------------------

    case EZSP_MAXIMUM_PAYLOAD_LENGTH: {
      uint8_t apsLength;
      apsLength = emberMaximumPayloadLength();
      appendInt8u(apsLength);
      break;
    }

    case EZSP_SEND_UNICAST: {
      EmberStatus status;
      EmberOutgoingMessageType type;
      EmberNodeId indexOrDestination;
      EmberApsFrame apsFrame;
      uint8_t messageTag;
      uint8_t messageLength;
      uint8_t *messageContents;
      uint8_t sequence;
      type = fetchInt8u();
      indexOrDestination = fetchInt16u();
      fetchEmberApsFrame(&apsFrame);
      messageTag = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      status = emberAfEzspSendUnicastCommandCallback(type, indexOrDestination, &apsFrame, messageTag, messageLength, messageContents, &sequence);
      appendInt8u(status);
      appendInt8u(sequence);
      break;
    }

    case EZSP_SEND_BROADCAST: {
      EmberStatus status;
      EmberNodeId destination;
      EmberApsFrame apsFrame;
      uint8_t radius;
      uint8_t messageTag;
      uint8_t messageLength;
      uint8_t *messageContents;
      uint8_t sequence;
      destination = fetchInt16u();
      fetchEmberApsFrame(&apsFrame);
      radius = fetchInt8u();
      messageTag = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      status = emberAfEzspSendBroadcastCommandCallback(destination, &apsFrame, radius, messageTag, messageLength, messageContents, &sequence);
      appendInt8u(status);
      appendInt8u(sequence);
      break;
    }

    case EZSP_PROXY_BROADCAST: {
      EmberStatus status;
      EmberNodeId source;
      EmberNodeId destination;
      uint8_t nwkSequence;
      EmberApsFrame apsFrame;
      uint8_t radius;
      uint8_t messageTag;
      uint8_t messageLength;
      uint8_t *messageContents;
      uint8_t apsSequence;
      source = fetchInt16u();
      destination = fetchInt16u();
      nwkSequence = fetchInt8u();
      fetchEmberApsFrame(&apsFrame);
      radius = fetchInt8u();
      messageTag = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      status = emberAfEzspProxyBroadcastCommandCallback(source, destination, nwkSequence, &apsFrame, radius, messageTag, messageLength, messageContents, &apsSequence);
      appendInt8u(status);
      appendInt8u(apsSequence);
      break;
    }

    case EZSP_SEND_MULTICAST: {
      EmberStatus status;
      EmberApsFrame apsFrame;
      uint8_t hops;
      uint8_t nonmemberRadius;
      uint8_t messageTag;
      uint8_t messageLength;
      uint8_t *messageContents;
      uint8_t sequence;
      fetchEmberApsFrame(&apsFrame);
      hops = fetchInt8u();
      nonmemberRadius = fetchInt8u();
      messageTag = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      status = emberAfEzspSendMulticastCommandCallback(&apsFrame, hops, nonmemberRadius, messageTag, messageLength, messageContents, &sequence);
      appendInt8u(status);
      appendInt8u(sequence);
      break;
    }

    case EZSP_SEND_MULTICAST_WITH_ALIAS: {
      EmberStatus status;
      EmberApsFrame apsFrame;
      uint8_t hops;
      uint8_t nonmemberRadius;
      uint16_t alias;
      uint8_t nwkSequence;
      uint8_t messageTag;
      uint8_t messageLength;
      uint8_t *messageContents;
      uint8_t sequence;
      fetchEmberApsFrame(&apsFrame);
      hops = fetchInt8u();
      nonmemberRadius = fetchInt8u();
      alias = fetchInt16u();
      nwkSequence = fetchInt8u();
      messageTag = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      status = emberAfEzspSendMulticastWithAliasCommandCallback(&apsFrame, hops, nonmemberRadius, alias, nwkSequence, messageTag, messageLength, messageContents, &sequence);
      appendInt8u(status);
      appendInt8u(sequence);
      break;
    }

    case EZSP_SEND_REPLY: {
      EmberStatus status;
      EmberNodeId sender;
      EmberApsFrame apsFrame;
      uint8_t messageLength;
      uint8_t *messageContents;
      sender = fetchInt16u();
      fetchEmberApsFrame(&apsFrame);
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      status = emberAfEzspSendReplyCommandCallback(sender, &apsFrame, messageLength, messageContents);
      appendInt8u(status);
      break;
    }

    case EZSP_SEND_MANY_TO_ONE_ROUTE_REQUEST: {
      EmberStatus status;
      uint16_t concentratorType;
      uint8_t radius;
      concentratorType = fetchInt16u();
      radius = fetchInt8u();
      status = emberSendManyToOneRouteRequest(concentratorType, radius);
      appendInt8u(status);
      break;
    }

    case EZSP_POLL_FOR_DATA: {
      EmberStatus status;
      uint16_t interval;
      EmberEventUnits units;
      uint8_t failureLimit;
      interval = fetchInt16u();
      units = fetchInt8u();
      failureLimit = fetchInt8u();
      status = emberAfEzspPollForDataCommandCallback(interval, units, failureLimit);
      appendInt8u(status);
      break;
    }

    case EZSP_SET_SOURCE_ROUTE_DISCOVERY_MODE: {
      uint32_t remainingTime;
      uint8_t mode;
      mode = fetchInt8u();
      remainingTime = emberAfEzspSetSourceRouteDiscoveryModeCommandCallback(mode);
      appendInt32u(remainingTime);
      break;
    }

    case EZSP_SET_SOURCE_ROUTE: {
      EmberStatus status;
      EmberNodeId destination;
      uint8_t relayCount;
      uint16_t *relayList;
      destination = fetchInt16u();
      relayCount = fetchInt8u();
      relayList = (uint16_t *)fetchInt16uPointer(relayCount);
      status = emberAfEzspSetSourceRouteCommandCallback(destination, relayCount, relayList);
      appendInt8u(status);
      break;
    }

    case EZSP_UNICAST_CURRENT_NETWORK_KEY: {
      EmberStatus status;
      EmberNodeId targetShort;
      uint8_t targetLong[8];
      EmberNodeId parentShortId;
      targetShort = fetchInt16u();
      fetchInt8uArray(8, targetLong);
      parentShortId = fetchInt16u();
      status = emberAfEzspUnicastCurrentNetworkKeyCommandCallback(targetShort, targetLong, parentShortId);
      appendInt8u(status);
      break;
    }

    case EZSP_ADDRESS_TABLE_ENTRY_IS_ACTIVE: {
      bool active;
      uint8_t addressTableIndex;
      addressTableIndex = fetchInt8u();
      active = emberAddressTableEntryIsActive(addressTableIndex);
      appendInt8u(active);
      break;
    }

    case EZSP_SET_ADDRESS_TABLE_REMOTE_EUI64: {
      EmberStatus status;
      uint8_t addressTableIndex;
      uint8_t eui64[8];
      addressTableIndex = fetchInt8u();
      fetchInt8uArray(8, eui64);
      status = emberSetAddressTableRemoteEui64(addressTableIndex, eui64);
      appendInt8u(status);
      break;
    }

    case EZSP_SET_ADDRESS_TABLE_REMOTE_NODE_ID: {
      uint8_t addressTableIndex;
      EmberNodeId id;
      addressTableIndex = fetchInt8u();
      id = fetchInt16u();
      emberSetAddressTableRemoteNodeId(addressTableIndex, id);
      break;
    }

    case EZSP_GET_ADDRESS_TABLE_REMOTE_EUI64: {
      uint8_t addressTableIndex;
      uint8_t eui64[8];
      addressTableIndex = fetchInt8u();
      emberGetAddressTableRemoteEui64(addressTableIndex, eui64);
      appendInt8uArray(8, eui64);
      break;
    }

    case EZSP_GET_ADDRESS_TABLE_REMOTE_NODE_ID: {
      EmberNodeId nodeId;
      uint8_t addressTableIndex;
      addressTableIndex = fetchInt8u();
      nodeId = emberGetAddressTableRemoteNodeId(addressTableIndex);
      appendInt16u(nodeId);
      break;
    }

    case EZSP_SET_EXTENDED_TIMEOUT: {
      uint8_t remoteEui64[8];
      bool extendedTimeout;
      fetchInt8uArray(8, remoteEui64);
      extendedTimeout = fetchInt8u();
      emberSetExtendedTimeout(remoteEui64, extendedTimeout);
      break;
    }

    case EZSP_GET_EXTENDED_TIMEOUT: {
      bool extendedTimeout;
      uint8_t remoteEui64[8];
      fetchInt8uArray(8, remoteEui64);
      extendedTimeout = emberGetExtendedTimeout(remoteEui64);
      appendInt8u(extendedTimeout);
      break;
    }

    case EZSP_REPLACE_ADDRESS_TABLE_ENTRY: {
      EmberStatus status;
      uint8_t addressTableIndex;
      uint8_t newEui64[8];
      EmberNodeId newId;
      bool newExtendedTimeout;
      uint8_t oldEui64[8];
      EmberNodeId oldId;
      bool oldExtendedTimeout;
      addressTableIndex = fetchInt8u();
      fetchInt8uArray(8, newEui64);
      newId = fetchInt16u();
      newExtendedTimeout = fetchInt8u();
      status = emberAfEzspReplaceAddressTableEntryCommandCallback(addressTableIndex, newEui64, newId, newExtendedTimeout, oldEui64, &oldId, &oldExtendedTimeout);
      appendInt8u(status);
      appendInt8uArray(8, oldEui64);
      appendInt16u(oldId);
      appendInt8u(oldExtendedTimeout);
      break;
    }

    case EZSP_LOOKUP_NODE_ID_BY_EUI64: {
      EmberNodeId nodeId;
      uint8_t eui64[8];
      fetchInt8uArray(8, eui64);
      nodeId = emberLookupNodeIdByEui64(eui64);
      appendInt16u(nodeId);
      break;
    }

    case EZSP_LOOKUP_EUI64_BY_NODE_ID: {
      EmberStatus status;
      EmberNodeId nodeId;
      uint8_t eui64[8];
      nodeId = fetchInt16u();
      status = emberLookupEui64ByNodeId(nodeId, eui64);
      appendInt8u(status);
      appendInt8uArray(8, eui64);
      break;
    }

    case EZSP_GET_MULTICAST_TABLE_ENTRY: {
      EmberStatus status;
      uint8_t index;
      EmberMulticastTableEntry value;
      index = fetchInt8u();
      status = emberAfEzspGetMulticastTableEntryCommandCallback(index, &value);
      appendInt8u(status);
      appendEmberMulticastTableEntry(&value);
      break;
    }

    case EZSP_SET_MULTICAST_TABLE_ENTRY: {
      EmberStatus status;
      uint8_t index;
      EmberMulticastTableEntry value;
      index = fetchInt8u();
      fetchEmberMulticastTableEntry(&value);
      status = emberAfEzspSetMulticastTableEntryCommandCallback(index, &value);
      appendInt8u(status);
      break;
    }

    case EZSP_WRITE_NODE_DATA: {
      EmberStatus status;
      bool erase;
      erase = fetchInt8u();
      status = emberWriteNodeData(erase);
      appendInt8u(status);
      break;
    }

    case EZSP_SEND_RAW_MESSAGE: {
      EmberStatus status;
      uint8_t messageLength;
      uint8_t *messageContents;
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      status = emberAfEzspSendRawMessageCommandCallback(messageLength, messageContents);
      appendInt8u(status);
      break;
    }

    case EZSP_SEND_RAW_MESSAGE_EXTENDED: {
      EmberStatus status;
      uint8_t messageLength;
      uint8_t *messageContents;
      uint8_t priority;
      bool useCca;
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      priority = fetchInt8u();
      useCca = fetchInt8u();
      status = emberAfEzspSendRawMessageExtendedCommandCallback(messageLength, messageContents, priority, useCca);
      appendInt8u(status);
      break;
    }

    case EZSP_SET_MAC_POLL_FAILURE_WAIT_TIME: {
      uint32_t waitBeforeRetryIntervalMs;
      waitBeforeRetryIntervalMs = fetchInt32u();
      emberSetMacPollFailureWaitTime(waitBeforeRetryIntervalMs);
      break;
    }

    case EZSP_SET_BEACON_CLASSIFICATION_PARAMS: {
      EmberStatus status;
      EmberBeaconClassificationParams param;
      status = emberSetBeaconClassificationParams(&param);
      appendInt8u(status);
      appendEmberBeaconClassificationParams(&param);
      break;
    }

    case EZSP_GET_BEACON_CLASSIFICATION_PARAMS: {
      EmberStatus status;
      EmberBeaconClassificationParams param;
      status = emberGetBeaconClassificationParams(&param);
      appendInt8u(status);
      appendEmberBeaconClassificationParams(&param);
      break;
    }

//------------------------------------------------------------------------------

    default: {
      return false;
    }
  }

  return true;
}
