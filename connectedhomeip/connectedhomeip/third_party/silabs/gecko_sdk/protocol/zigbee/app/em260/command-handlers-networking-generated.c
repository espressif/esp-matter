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

bool emAfProcessEzspCommandNetworking(uint16_t commandId)
{
  switch (commandId) {
//------------------------------------------------------------------------------

    case EZSP_SET_MANUFACTURER_CODE: {
      uint16_t code;
      code = fetchInt16u();
      emberSetManufacturerCode(code);
      break;
    }

    case EZSP_SET_POWER_DESCRIPTOR: {
      uint16_t descriptor;
      descriptor = fetchInt16u();
      emberSetPowerDescriptor(descriptor);
      break;
    }

    case EZSP_NETWORK_INIT: {
      EmberStatus status;
      EmberNetworkInitStruct networkInitStruct;
      fetchEmberNetworkInitStruct(&networkInitStruct);
      status = emberNetworkInit(&networkInitStruct);
      appendInt8u(status);
      break;
    }

    case EZSP_NETWORK_STATE: {
      EmberNetworkStatus status;
      status = emberNetworkState();
      appendInt8u(status);
      break;
    }

    case EZSP_START_SCAN: {
      sl_status_t status;
      EzspNetworkScanType scanType;
      uint32_t channelMask;
      uint8_t duration;
      scanType = fetchInt8u();
      channelMask = fetchInt32u();
      duration = fetchInt8u();
      status = emberStartScan(scanType, channelMask, duration);
      appendInt32u(status);
      break;
    }

    case EZSP_FIND_UNUSED_PAN_ID: {
      EmberStatus status;
      uint32_t channelMask;
      uint8_t duration;
      channelMask = fetchInt32u();
      duration = fetchInt8u();
      status = emberFindUnusedPanId(channelMask, duration);
      appendInt8u(status);
      break;
    }

    case EZSP_STOP_SCAN: {
      EmberStatus status;
      status = emberStopScan();
      appendInt8u(status);
      break;
    }

    case EZSP_FORM_NETWORK: {
      EmberStatus status;
      EmberNetworkParameters parameters;
      fetchEmberNetworkParameters(&parameters);
      status = emberFormNetwork(&parameters);
      appendInt8u(status);
      break;
    }

    case EZSP_JOIN_NETWORK: {
      EmberStatus status;
      EmberNodeType nodeType;
      EmberNetworkParameters parameters;
      nodeType = fetchInt8u();
      fetchEmberNetworkParameters(&parameters);
      status = emberJoinNetwork(nodeType, &parameters);
      appendInt8u(status);
      break;
    }

    case EZSP_JOIN_NETWORK_DIRECTLY: {
      EmberStatus status;
      EmberNodeType localNodeType;
      EmberBeaconData beacon;
      int8_t radioTxPower;
      bool clearBeaconsAfterNetworkUp;
      localNodeType = fetchInt8u();
      fetchEmberBeaconData(&beacon);
      radioTxPower = fetchInt8u();
      clearBeaconsAfterNetworkUp = fetchInt8u();
      status = emberJoinNetworkDirectly(localNodeType, &beacon, radioTxPower, clearBeaconsAfterNetworkUp);
      appendInt8u(status);
      break;
    }

    case EZSP_LEAVE_NETWORK: {
      EmberStatus status;
      status = emberLeaveNetwork();
      appendInt8u(status);
      break;
    }

    case EZSP_FIND_AND_REJOIN_NETWORK: {
      EmberStatus status;
      bool haveCurrentNetworkKey;
      uint32_t channelMask;
      haveCurrentNetworkKey = fetchInt8u();
      channelMask = fetchInt32u();
      status = emberAfEzspFindAndRejoinNetworkCommandCallback(haveCurrentNetworkKey, channelMask);
      appendInt8u(status);
      break;
    }

    case EZSP_PERMIT_JOINING: {
      EmberStatus status;
      uint8_t duration;
      duration = fetchInt8u();
      status = emberPermitJoining(duration);
      appendInt8u(status);
      break;
    }

    case EZSP_ENERGY_SCAN_REQUEST: {
      EmberStatus status;
      EmberNodeId target;
      uint32_t scanChannels;
      uint8_t scanDuration;
      uint16_t scanCount;
      target = fetchInt16u();
      scanChannels = fetchInt32u();
      scanDuration = fetchInt8u();
      scanCount = fetchInt16u();
      status = emberEnergyScanRequest(target, scanChannels, scanDuration, scanCount);
      appendInt8u(status);
      break;
    }

    case EZSP_GET_NETWORK_PARAMETERS: {
      EmberStatus status;
      EmberNodeType nodeType;
      EmberNetworkParameters parameters;
      status = emberAfEzspGetNetworkParametersCommandCallback(&nodeType, &parameters);
      appendInt8u(status);
      appendInt8u(nodeType);
      appendEmberNetworkParameters(&parameters);
      break;
    }

    case EZSP_GET_RADIO_PARAMETERS: {
      EmberStatus status;
      uint8_t phyIndex;
      EmberMultiPhyRadioParameters parameters;
      phyIndex = fetchInt8u();
      status = emberGetRadioParameters(phyIndex, &parameters);
      appendInt8u(status);
      appendEmberMultiPhyRadioParameters(&parameters);
      break;
    }

    case EZSP_GET_PARENT_CHILD_PARAMETERS: {
      uint8_t childCount;
      uint8_t parentEui64[8];
      EmberNodeId parentNodeId;
      childCount = emberAfEzspGetParentChildParametersCommandCallback(parentEui64, &parentNodeId);
      appendInt8u(childCount);
      appendInt8uArray(8, parentEui64);
      appendInt16u(parentNodeId);
      break;
    }

    case EZSP_GET_CHILD_DATA: {
      EmberStatus status;
      uint8_t index;
      EmberChildData childData;
      index = fetchInt8u();
      status = emberGetChildData(index, &childData);
      appendInt8u(status);
      appendEmberChildData(&childData);
      break;
    }

    case EZSP_SET_CHILD_DATA: {
      EmberStatus status;
      uint8_t index;
      EmberChildData childData;
      index = fetchInt8u();
      fetchEmberChildData(&childData);
      status = emberSetChildData(index, &childData);
      appendInt8u(status);
      break;
    }

    case EZSP_GET_SOURCE_ROUTE_TABLE_TOTAL_SIZE: {
      uint8_t sourceRouteTableTotalSize;
      sourceRouteTableTotalSize = emberAfEzspGetSourceRouteTableTotalSizeCommandCallback();
      appendInt8u(sourceRouteTableTotalSize);
      break;
    }

    case EZSP_GET_SOURCE_ROUTE_TABLE_FILLED_SIZE: {
      uint8_t sourceRouteTableFilledSize;
      sourceRouteTableFilledSize = emberAfEzspGetSourceRouteTableFilledSizeCommandCallback();
      appendInt8u(sourceRouteTableFilledSize);
      break;
    }

    case EZSP_GET_SOURCE_ROUTE_TABLE_ENTRY: {
      EmberStatus status;
      uint8_t index;
      EmberNodeId destination;
      uint8_t closerIndex;
      index = fetchInt8u();
      status = emberAfEzspGetSourceRouteTableEntryCommandCallback(index, &destination, &closerIndex);
      appendInt8u(status);
      appendInt16u(destination);
      appendInt8u(closerIndex);
      break;
    }

    case EZSP_GET_NEIGHBOR: {
      EmberStatus status;
      uint8_t index;
      EmberNeighborTableEntry value;
      index = fetchInt8u();
      status = emberGetNeighbor(index, &value);
      appendInt8u(status);
      appendEmberNeighborTableEntry(&value);
      break;
    }

    case EZSP_GET_NEIGHBOR_FRAME_COUNTER: {
      EmberStatus status;
      uint8_t eui64[8];
      uint32_t returnFrameCounter;
      fetchInt8uArray(8, eui64);
      status = emberGetNeighborFrameCounter(eui64, &returnFrameCounter);
      appendInt8u(status);
      appendInt32u(returnFrameCounter);
      break;
    }

    case EZSP_SET_NEIGHBOR_FRAME_COUNTER: {
      EmberStatus status;
      uint8_t eui64[8];
      uint32_t frameCounter;
      fetchInt8uArray(8, eui64);
      frameCounter = fetchInt32u();
      status = emberSetNeighborFrameCounter(eui64, frameCounter);
      appendInt8u(status);
      break;
    }

    case EZSP_SET_ROUTING_SHORTCUT_THRESHOLD: {
      EmberStatus status;
      uint8_t costThresh;
      costThresh = fetchInt8u();
      status = emberAfEzspSetRoutingShortcutThresholdCommandCallback(costThresh);
      appendInt8u(status);
      break;
    }

    case EZSP_GET_ROUTING_SHORTCUT_THRESHOLD: {
      uint8_t routingShortcutThresh;
      routingShortcutThresh = emberAfEzspGetRoutingShortcutThresholdCommandCallback();
      appendInt8u(routingShortcutThresh);
      break;
    }

    case EZSP_NEIGHBOR_COUNT: {
      uint8_t value;
      value = emberNeighborCount();
      appendInt8u(value);
      break;
    }

    case EZSP_GET_ROUTE_TABLE_ENTRY: {
      EmberStatus status;
      uint8_t index;
      EmberRouteTableEntry value;
      index = fetchInt8u();
      status = emberGetRouteTableEntry(index, &value);
      appendInt8u(status);
      appendEmberRouteTableEntry(&value);
      break;
    }

    case EZSP_SET_RADIO_POWER: {
      EmberStatus status;
      int8_t power;
      power = fetchInt8u();
      status = emberSetRadioPower(power);
      appendInt8u(status);
      break;
    }

    case EZSP_SET_RADIO_CHANNEL: {
      EmberStatus status;
      uint8_t channel;
      channel = fetchInt8u();
      status = emberSetRadioChannel(channel);
      appendInt8u(status);
      break;
    }

    case EZSP_GET_RADIO_CHANNEL: {
      uint8_t channel;
      channel = emberGetRadioChannel();
      appendInt8u(channel);
      break;
    }

    case EZSP_SET_RADIO_IEEE802154_CCA_MODE: {
      EmberStatus status;
      uint8_t ccaMode;
      ccaMode = fetchInt8u();
      status = emberSetRadioIeee802154CcaMode(ccaMode);
      appendInt8u(status);
      break;
    }

    case EZSP_SET_CONCENTRATOR: {
      EmberStatus status;
      bool on;
      uint16_t concentratorType;
      uint16_t minTime;
      uint16_t maxTime;
      uint8_t routeErrorThreshold;
      uint8_t deliveryFailureThreshold;
      uint8_t maxHops;
      on = fetchInt8u();
      concentratorType = fetchInt16u();
      minTime = fetchInt16u();
      maxTime = fetchInt16u();
      routeErrorThreshold = fetchInt8u();
      deliveryFailureThreshold = fetchInt8u();
      maxHops = fetchInt8u();
      status = emberAfEzspSetConcentratorCommandCallback(on, concentratorType, minTime, maxTime, routeErrorThreshold, deliveryFailureThreshold, maxHops);
      appendInt8u(status);
      break;
    }

    case EZSP_SET_BROKEN_ROUTE_ERROR_CODE: {
      EmberStatus status;
      uint8_t errorCode;
      errorCode = fetchInt8u();
      status = emberAfEzspSetBrokenRouteErrorCodeCommandCallback(errorCode);
      appendInt8u(status);
      break;
    }

    case EZSP_MULTI_PHY_START: {
      EmberStatus status;
      uint8_t phyIndex;
      uint8_t page;
      uint8_t channel;
      int8_t power;
      EmberMultiPhyNwkConfig bitmask;
      phyIndex = fetchInt8u();
      page = fetchInt8u();
      channel = fetchInt8u();
      power = fetchInt8u();
      bitmask = fetchInt8u();
      status = emberMultiPhyStart(phyIndex, page, channel, power, bitmask);
      appendInt8u(status);
      break;
    }

    case EZSP_MULTI_PHY_STOP: {
      EmberStatus status;
      uint8_t phyIndex;
      phyIndex = fetchInt8u();
      status = emberMultiPhyStop(phyIndex);
      appendInt8u(status);
      break;
    }

    case EZSP_MULTI_PHY_SET_RADIO_POWER: {
      EmberStatus status;
      uint8_t phyIndex;
      int8_t power;
      phyIndex = fetchInt8u();
      power = fetchInt8u();
      status = emberMultiPhySetRadioPower(phyIndex, power);
      appendInt8u(status);
      break;
    }

    case EZSP_SEND_LINK_POWER_DELTA_REQUEST: {
      EmberStatus status;
      status = emberSendLinkPowerDeltaRequest();
      appendInt8u(status);
      break;
    }

    case EZSP_MULTI_PHY_SET_RADIO_CHANNEL: {
      EmberStatus status;
      uint8_t phyIndex;
      uint8_t page;
      uint8_t channel;
      phyIndex = fetchInt8u();
      page = fetchInt8u();
      channel = fetchInt8u();
      status = emberMultiPhySetRadioChannel(phyIndex, page, channel);
      appendInt8u(status);
      break;
    }

    case EZSP_GET_DUTY_CYCLE_STATE: {
      EmberStatus status;
      EmberDutyCycleState returnedState;
      status = emberGetDutyCycleState(&returnedState);
      appendInt8u(status);
      appendInt8u(returnedState);
      break;
    }

    case EZSP_SET_DUTY_CYCLE_LIMITS_IN_STACK: {
      EmberStatus status;
      EmberDutyCycleLimits limits;
      fetchEmberDutyCycleLimits(&limits);
      status = emberSetDutyCycleLimitsInStack(&limits);
      appendInt8u(status);
      break;
    }

    case EZSP_GET_DUTY_CYCLE_LIMITS: {
      EmberStatus status;
      EmberDutyCycleLimits returnedLimits;
      status = emberGetDutyCycleLimits(&returnedLimits);
      appendInt8u(status);
      appendEmberDutyCycleLimits(&returnedLimits);
      break;
    }

    case EZSP_GET_CURRENT_DUTY_CYCLE: {
      EmberStatus status;
      uint8_t maxDevices;
      uint8_t arrayOfDeviceDutyCycles[134];
      maxDevices = fetchInt8u();
      status = emberAfEzspGetCurrentDutyCycleCommandCallback(maxDevices, arrayOfDeviceDutyCycles);
      appendInt8u(status);
      appendInt8uArray(134, arrayOfDeviceDutyCycles);
      break;
    }

    case EZSP_GET_FIRST_BEACON: {
      EmberStatus status;
      EmberBeaconIterator beaconIterator;
      status = emberGetFirstBeacon(&beaconIterator);
      appendInt8u(status);
      appendEmberBeaconIterator(&beaconIterator);
      break;
    }

    case EZSP_GET_NEXT_BEACON: {
      EmberStatus status;
      EmberBeaconData beacon;
      status = emberGetNextBeacon(&beacon);
      appendInt8u(status);
      appendEmberBeaconData(&beacon);
      break;
    }

    case EZSP_GET_NUM_STORED_BEACONS: {
      uint8_t numBeacons;
      numBeacons = emberGetNumStoredBeacons();
      appendInt8u(numBeacons);
      break;
    }

    case EZSP_CLEAR_STORED_BEACONS: {
      emberClearStoredBeacons();
      break;
    }

    case EZSP_SET_LOGICAL_AND_RADIO_CHANNEL: {
      EmberStatus status;
      uint8_t radioChannel;
      radioChannel = fetchInt8u();
      status = emberSetLogicalAndRadioChannel(radioChannel);
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
