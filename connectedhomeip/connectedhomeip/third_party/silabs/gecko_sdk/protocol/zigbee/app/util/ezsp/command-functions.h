/*****************************************************************************/
/**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/
//
// *** Generated file. Do not edit! ***
//
// Description: Functions for sending every EM260 frame and returning the result
// to the Host.

//------------------------------------------------------------------------------
// Configuration Frames
//------------------------------------------------------------------------------

uint8_t ezspVersion(
  uint8_t desiredProtocolVersion,
  uint8_t *stackType,
  uint16_t *stackVersion)
{
  uint8_t protocolVersion;
  startCommand(EZSP_VERSION);
  appendInt8u(desiredProtocolVersion);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    protocolVersion = fetchInt8u();
    *stackType = fetchInt8u();
    *stackVersion = fetchInt16u();
    return protocolVersion;
  }
  return 255;
}

EzspStatus ezspGetConfigurationValue(
  EzspConfigId configId,
  uint16_t *value)
{
  EzspStatus status;
  startCommand(EZSP_GET_CONFIGURATION_VALUE);
  appendInt8u(configId);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *value = fetchInt16u();
    return status;
  }
  return sendStatus;
}

EzspStatus ezspSetConfigurationValue(
  EzspConfigId configId,
  uint16_t value)
{
  EzspStatus status;
  startCommand(EZSP_SET_CONFIGURATION_VALUE);
  appendInt8u(configId);
  appendInt16u(value);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EzspStatus ezspAddEndpoint(
  uint8_t endpoint,
  uint16_t profileId,
  uint16_t deviceId,
  uint8_t deviceVersion,
  uint8_t inputClusterCount,
  uint8_t outputClusterCount,
  uint16_t *inputClusterList,
  uint16_t *outputClusterList)
{
  EzspStatus status;
  startCommand(EZSP_ADD_ENDPOINT);
  appendInt8u(endpoint);
  appendInt16u(profileId);
  appendInt16u(deviceId);
  appendInt8u(deviceVersion);
  appendInt8u(inputClusterCount);
  appendInt8u(outputClusterCount);
  appendInt16uArray(inputClusterCount, inputClusterList);
  appendInt16uArray(outputClusterCount, outputClusterList);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EzspStatus ezspSetPolicy(
  EzspPolicyId policyId,
  EzspDecisionId decisionId)
{
  EzspStatus status;
  startCommand(EZSP_SET_POLICY);
  appendInt8u(policyId);
  appendInt8u(decisionId);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EzspStatus ezspGetPolicy(
  EzspPolicyId policyId,
  EzspDecisionId *decisionId)
{
  EzspStatus status;
  startCommand(EZSP_GET_POLICY);
  appendInt8u(policyId);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *decisionId = fetchInt8u();
    return status;
  }
  return sendStatus;
}

bool ezspSendPanIdUpdate(
  EmberPanId newPan)
{
  bool status;
  startCommand(EZSP_SEND_PAN_ID_UPDATE);
  appendInt16u(newPan);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EzspStatus ezspGetValue(
  EzspValueId valueId,
  uint8_t *valueLength,
  uint8_t *value)
{
  EzspStatus status;
  uint8_t maxValueLength = *valueLength;
  startCommand(EZSP_GET_VALUE);
  appendInt8u(valueId);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *valueLength = fetchInt8u();
    if (*valueLength > maxValueLength) {
      return EZSP_ERROR_INVALID_VALUE;
    }
    fetchInt8uArray(*valueLength, value);
    return status;
  }
  return sendStatus;
}

EzspStatus ezspGetExtendedValue(
  EzspExtendedValueId valueId,
  uint32_t characteristics,
  uint8_t *valueLength,
  uint8_t *value)
{
  EzspStatus status;
  uint8_t maxValueLength = *valueLength;
  startCommand(EZSP_GET_EXTENDED_VALUE);
  appendInt8u(valueId);
  appendInt32u(characteristics);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *valueLength = fetchInt8u();
    if (*valueLength > maxValueLength) {
      return EZSP_ERROR_INVALID_VALUE;
    }
    fetchInt8uArray(*valueLength, value);
    return status;
  }
  return sendStatus;
}

EzspStatus ezspSetValue(
  EzspValueId valueId,
  uint8_t valueLength,
  uint8_t *value)
{
  EzspStatus status;
  startCommand(EZSP_SET_VALUE);
  appendInt8u(valueId);
  appendInt8u(valueLength);
  appendInt8uArray(valueLength, value);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

//------------------------------------------------------------------------------
// Utilities Frames
//------------------------------------------------------------------------------

void ezspNop(void)
{
  startCommand(EZSP_NOP);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

uint8_t ezspEcho(
  uint8_t dataLength,
  uint8_t *data,
  uint8_t *echo)
{
  uint8_t echoLength;
  startCommand(EZSP_ECHO);
  appendInt8u(dataLength);
  appendInt8uArray(dataLength, data);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    echoLength = fetchInt8u();
    if (echoLength > dataLength) {
      return 0;
    }
    fetchInt8uArray(echoLength, echo);
    return echoLength;
  }
  return 0;
}

void ezspCallback(void)
{
  startCommand(EZSP_CALLBACK);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    callbackDispatch();
  }
}

EmberStatus ezspSetToken(
  uint8_t tokenId,
  uint8_t *tokenData)
{
  EmberStatus status;
  startCommand(EZSP_SET_TOKEN);
  appendInt8u(tokenId);
  appendInt8uArray(8, tokenData);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetToken(
  uint8_t tokenId,
  uint8_t *tokenData)
{
  EmberStatus status;
  startCommand(EZSP_GET_TOKEN);
  appendInt8u(tokenId);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchInt8uArray(8, tokenData);
    return status;
  }
  return sendStatus;
}

uint8_t ezspGetMfgToken(
  EzspMfgTokenId tokenId,
  uint8_t *tokenData)
{
  uint8_t tokenDataLength;
  startCommand(EZSP_GET_MFG_TOKEN);
  appendInt8u(tokenId);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    tokenDataLength = fetchInt8u();
    uint8_t expectedTokenDataLength = 0;
    // the size of corresponding the EZSP Mfg token,
    // please refer to app/util/ezsp/ezsp-enum.h
    switch (tokenId) {
      // 2 bytes
      case EZSP_MFG_CUSTOM_VERSION:
      case EZSP_MFG_MANUF_ID:
      case EZSP_MFG_PHY_CONFIG:
      case EZSP_MFG_CTUNE:
        expectedTokenDataLength = 2;
        break;
      // 8 bytes
      case EZSP_MFG_EZSP_STORAGE:
      case EZSP_MFG_CUSTOM_EUI_64:
        expectedTokenDataLength = 8;
        break;
      // 16 bytes
      case EZSP_MFG_STRING:
      case EZSP_MFG_BOARD_NAME:
      case EZSP_MFG_BOOTLOAD_AES_KEY:
        expectedTokenDataLength = 16;
        break;
      // 20 bytes
      case EZSP_MFG_INSTALLATION_CODE:
        expectedTokenDataLength = 20;
        break;
      // 40 bytes
      case EZSP_MFG_ASH_CONFIG:
        expectedTokenDataLength = 40;
        break;
      // 92 bytes
      case EZSP_MFG_CBKE_DATA:
        expectedTokenDataLength = 92;
        break;
      default:
        break;
    }
    if (tokenDataLength != expectedTokenDataLength) {
      return 255;
    }
    fetchInt8uArray(tokenDataLength, tokenData);
    return tokenDataLength;
  }
  return 255;
}

EmberStatus ezspSetMfgToken(
  EzspMfgTokenId tokenId,
  uint8_t tokenDataLength,
  uint8_t *tokenData)
{
  EmberStatus status;
  startCommand(EZSP_SET_MFG_TOKEN);
  appendInt8u(tokenId);
  appendInt8u(tokenDataLength);
  appendInt8uArray(tokenDataLength, tokenData);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetRandomNumber(
  uint16_t *value)
{
  EmberStatus status;
  startCommand(EZSP_GET_RANDOM_NUMBER);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *value = fetchInt16u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetTimer(
  uint8_t timerId,
  uint16_t time,
  EmberEventUnits units,
  bool repeat)
{
  EmberStatus status;
  startCommand(EZSP_SET_TIMER);
  appendInt8u(timerId);
  appendInt16u(time);
  appendInt8u(units);
  appendInt8u(repeat);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

uint16_t ezspGetTimer(
  uint8_t timerId,
  EmberEventUnits *units,
  bool *repeat)
{
  uint16_t time;
  startCommand(EZSP_GET_TIMER);
  appendInt8u(timerId);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    time = fetchInt16u();
    *units = fetchInt8u();
    *repeat = fetchInt8u();
    return time;
  }
  return 255;
}

EmberStatus ezspDebugWrite(
  bool binaryMessage,
  uint8_t messageLength,
  uint8_t *messageContents)
{
  EmberStatus status;
  startCommand(EZSP_DEBUG_WRITE);
  appendInt8u(binaryMessage);
  appendInt8u(messageLength);
  appendInt8uArray(messageLength, messageContents);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

void ezspReadAndClearCounters(
  uint16_t *values)
{
  startCommand(EZSP_READ_AND_CLEAR_COUNTERS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    fetchInt16uArray(EMBER_COUNTER_TYPE_COUNT, values);
  }
}

void ezspReadCounters(
  uint16_t *values)
{
  startCommand(EZSP_READ_COUNTERS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    fetchInt16uArray(EMBER_COUNTER_TYPE_COUNT, values);
  }
}

void ezspDelayTest(
  uint16_t delay)
{
  startCommand(EZSP_DELAY_TEST);
  appendInt16u(delay);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

EmberLibraryStatus ezspGetLibraryStatus(
  EmberLibraryId libraryId)
{
  EmberLibraryStatus status;
  startCommand(EZSP_GET_LIBRARY_STATUS);
  appendInt8u(libraryId);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetXncpInfo(
  uint16_t *manufacturerId,
  uint16_t *versionNumber)
{
  EmberStatus status;
  startCommand(EZSP_GET_XNCP_INFO);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *manufacturerId = fetchInt16u();
    *versionNumber = fetchInt16u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspCustomFrame(
  uint8_t payloadLength,
  uint8_t *payload,
  uint8_t *replyLength,
  uint8_t *reply)
{
  EmberStatus status;
  uint8_t maxReplyLength = *replyLength;
  startCommand(EZSP_CUSTOM_FRAME);
  appendInt8u(payloadLength);
  appendInt8uArray(payloadLength, payload);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *replyLength = fetchInt8u();
    if (*replyLength > maxReplyLength) {
      return EZSP_ERROR_INVALID_VALUE;
    }
    fetchInt8uArray(*replyLength, reply);
    return status;
  }
  return sendStatus;
}

void ezspGetEui64(
  EmberEUI64 eui64)
{
  startCommand(EZSP_GET_EUI64);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    fetchInt8uArray(8, eui64);
  }
}

EmberNodeId ezspGetNodeId(void)
{
  EmberNodeId nodeId;
  startCommand(EZSP_GET_NODE_ID);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    nodeId = fetchInt16u();
    return nodeId;
  }
  return 0xFFFE;
}

uint8_t ezspGetPhyInterfaceCount(void)
{
  uint8_t interfaceCount;
  startCommand(EZSP_GET_PHY_INTERFACE_COUNT);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    interfaceCount = fetchInt8u();
    return interfaceCount;
  }
  return 255;
}

EmberEntropySource ezspGetTrueRandomEntropySource(void)
{
  EmberEntropySource entropySource;
  startCommand(EZSP_GET_TRUE_RANDOM_ENTROPY_SOURCE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    entropySource = fetchInt8u();
    return entropySource;
  }
  return 255;
}

//------------------------------------------------------------------------------
// Networking Frames
//------------------------------------------------------------------------------

void ezspSetManufacturerCode(
  uint16_t code)
{
  startCommand(EZSP_SET_MANUFACTURER_CODE);
  appendInt16u(code);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

void ezspSetPowerDescriptor(
  uint16_t descriptor)
{
  startCommand(EZSP_SET_POWER_DESCRIPTOR);
  appendInt16u(descriptor);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

EmberStatus ezspNetworkInit(
  EmberNetworkInitStruct *networkInitStruct)
{
  EmberStatus status;
  startCommand(EZSP_NETWORK_INIT);
  appendEmberNetworkInitStruct(networkInitStruct);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberNetworkStatus ezspNetworkState(void)
{
  EmberNetworkStatus status;
  startCommand(EZSP_NETWORK_STATE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

sl_status_t ezspStartScan(
  EzspNetworkScanType scanType,
  uint32_t channelMask,
  uint8_t duration)
{
  sl_status_t status;
  startCommand(EZSP_START_SCAN);
  appendInt8u(scanType);
  appendInt32u(channelMask);
  appendInt8u(duration);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt32u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspFindUnusedPanId(
  uint32_t channelMask,
  uint8_t duration)
{
  EmberStatus status;
  startCommand(EZSP_FIND_UNUSED_PAN_ID);
  appendInt32u(channelMask);
  appendInt8u(duration);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspStopScan(void)
{
  EmberStatus status;
  startCommand(EZSP_STOP_SCAN);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspFormNetwork(
  EmberNetworkParameters *parameters)
{
  EmberStatus status;
  startCommand(EZSP_FORM_NETWORK);
  appendEmberNetworkParameters(parameters);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspJoinNetwork(
  EmberNodeType nodeType,
  EmberNetworkParameters *parameters)
{
  EmberStatus status;
  startCommand(EZSP_JOIN_NETWORK);
  appendInt8u(nodeType);
  appendEmberNetworkParameters(parameters);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspJoinNetworkDirectly(
  EmberNodeType localNodeType,
  EmberBeaconData *beacon,
  int8_t radioTxPower,
  bool clearBeaconsAfterNetworkUp)
{
  EmberStatus status;
  startCommand(EZSP_JOIN_NETWORK_DIRECTLY);
  appendInt8u(localNodeType);
  appendEmberBeaconData(beacon);
  appendInt8u(radioTxPower);
  appendInt8u(clearBeaconsAfterNetworkUp);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspLeaveNetwork(void)
{
  EmberStatus status;
  startCommand(EZSP_LEAVE_NETWORK);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspFindAndRejoinNetwork(
  bool haveCurrentNetworkKey,
  uint32_t channelMask)
{
  EmberStatus status;
  startCommand(EZSP_FIND_AND_REJOIN_NETWORK);
  appendInt8u(haveCurrentNetworkKey);
  appendInt32u(channelMask);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspPermitJoining(
  uint8_t duration)
{
  EmberStatus status;
  startCommand(EZSP_PERMIT_JOINING);
  appendInt8u(duration);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspEnergyScanRequest(
  EmberNodeId target,
  uint32_t scanChannels,
  uint8_t scanDuration,
  uint16_t scanCount)
{
  EmberStatus status;
  startCommand(EZSP_ENERGY_SCAN_REQUEST);
  appendInt16u(target);
  appendInt32u(scanChannels);
  appendInt8u(scanDuration);
  appendInt16u(scanCount);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetNetworkParameters(
  EmberNodeType *nodeType,
  EmberNetworkParameters *parameters)
{
  EmberStatus status;
  startCommand(EZSP_GET_NETWORK_PARAMETERS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *nodeType = fetchInt8u();
    fetchEmberNetworkParameters(parameters);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetRadioParameters(
  uint8_t phyIndex,
  EmberMultiPhyRadioParameters *parameters)
{
  EmberStatus status;
  startCommand(EZSP_GET_RADIO_PARAMETERS);
  appendInt8u(phyIndex);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberMultiPhyRadioParameters(parameters);
    return status;
  }
  return sendStatus;
}

uint8_t ezspGetParentChildParameters(
  EmberEUI64 parentEui64,
  EmberNodeId *parentNodeId)
{
  uint8_t childCount;
  startCommand(EZSP_GET_PARENT_CHILD_PARAMETERS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    childCount = fetchInt8u();
    fetchInt8uArray(8, parentEui64);
    *parentNodeId = fetchInt16u();
    return childCount;
  }
  return 255;
}

EmberStatus ezspGetChildData(
  uint8_t index,
  EmberChildData *childData)
{
  EmberStatus status;
  startCommand(EZSP_GET_CHILD_DATA);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberChildData(childData);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetChildData(
  uint8_t index,
  EmberChildData *childData)
{
  EmberStatus status;
  startCommand(EZSP_SET_CHILD_DATA);
  appendInt8u(index);
  appendEmberChildData(childData);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

uint8_t ezspGetSourceRouteTableTotalSize(void)
{
  uint8_t sourceRouteTableTotalSize;
  startCommand(EZSP_GET_SOURCE_ROUTE_TABLE_TOTAL_SIZE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    sourceRouteTableTotalSize = fetchInt8u();
    return sourceRouteTableTotalSize;
  }
  return 255;
}

uint8_t ezspGetSourceRouteTableFilledSize(void)
{
  uint8_t sourceRouteTableFilledSize;
  startCommand(EZSP_GET_SOURCE_ROUTE_TABLE_FILLED_SIZE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    sourceRouteTableFilledSize = fetchInt8u();
    return sourceRouteTableFilledSize;
  }
  return 255;
}

EmberStatus ezspGetSourceRouteTableEntry(
  uint8_t index,
  EmberNodeId *destination,
  uint8_t *closerIndex)
{
  EmberStatus status;
  startCommand(EZSP_GET_SOURCE_ROUTE_TABLE_ENTRY);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *destination = fetchInt16u();
    *closerIndex = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetNeighbor(
  uint8_t index,
  EmberNeighborTableEntry *value)
{
  EmberStatus status;
  startCommand(EZSP_GET_NEIGHBOR);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberNeighborTableEntry(value);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetNeighborFrameCounter(
  EmberEUI64 eui64,
  uint32_t *returnFrameCounter)
{
  EmberStatus status;
  startCommand(EZSP_GET_NEIGHBOR_FRAME_COUNTER);
  appendInt8uArray(8, eui64);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *returnFrameCounter = fetchInt32u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetNeighborFrameCounter(
  EmberEUI64 eui64,
  uint32_t frameCounter)
{
  EmberStatus status;
  startCommand(EZSP_SET_NEIGHBOR_FRAME_COUNTER);
  appendInt8uArray(8, eui64);
  appendInt32u(frameCounter);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetRoutingShortcutThreshold(
  uint8_t costThresh)
{
  EmberStatus status;
  startCommand(EZSP_SET_ROUTING_SHORTCUT_THRESHOLD);
  appendInt8u(costThresh);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

uint8_t ezspGetRoutingShortcutThreshold(void)
{
  uint8_t routingShortcutThresh;
  startCommand(EZSP_GET_ROUTING_SHORTCUT_THRESHOLD);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    routingShortcutThresh = fetchInt8u();
    return routingShortcutThresh;
  }
  return 255;
}

uint8_t ezspNeighborCount(void)
{
  uint8_t value;
  startCommand(EZSP_NEIGHBOR_COUNT);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    value = fetchInt8u();
    return value;
  }
  return 255;
}

EmberStatus ezspGetRouteTableEntry(
  uint8_t index,
  EmberRouteTableEntry *value)
{
  EmberStatus status;
  startCommand(EZSP_GET_ROUTE_TABLE_ENTRY);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberRouteTableEntry(value);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetRadioPower(
  int8_t power)
{
  EmberStatus status;
  startCommand(EZSP_SET_RADIO_POWER);
  appendInt8u(power);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetRadioChannel(
  uint8_t channel)
{
  EmberStatus status;
  startCommand(EZSP_SET_RADIO_CHANNEL);
  appendInt8u(channel);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

uint8_t ezspGetRadioChannel(void)
{
  uint8_t channel;
  startCommand(EZSP_GET_RADIO_CHANNEL);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    channel = fetchInt8u();
    return channel;
  }
  return 255;
}

EmberStatus ezspSetRadioIeee802154CcaMode(
  uint8_t ccaMode)
{
  EmberStatus status;
  startCommand(EZSP_SET_RADIO_IEEE802154_CCA_MODE);
  appendInt8u(ccaMode);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetConcentrator(
  bool on,
  uint16_t concentratorType,
  uint16_t minTime,
  uint16_t maxTime,
  uint8_t routeErrorThreshold,
  uint8_t deliveryFailureThreshold,
  uint8_t maxHops)
{
  EmberStatus status;
  startCommand(EZSP_SET_CONCENTRATOR);
  appendInt8u(on);
  appendInt16u(concentratorType);
  appendInt16u(minTime);
  appendInt16u(maxTime);
  appendInt8u(routeErrorThreshold);
  appendInt8u(deliveryFailureThreshold);
  appendInt8u(maxHops);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetBrokenRouteErrorCode(
  uint8_t errorCode)
{
  EmberStatus status;
  startCommand(EZSP_SET_BROKEN_ROUTE_ERROR_CODE);
  appendInt8u(errorCode);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspMultiPhyStart(
  uint8_t phyIndex,
  uint8_t page,
  uint8_t channel,
  int8_t power,
  EmberMultiPhyNwkConfig bitmask)
{
  EmberStatus status;
  startCommand(EZSP_MULTI_PHY_START);
  appendInt8u(phyIndex);
  appendInt8u(page);
  appendInt8u(channel);
  appendInt8u(power);
  appendInt8u(bitmask);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspMultiPhyStop(
  uint8_t phyIndex)
{
  EmberStatus status;
  startCommand(EZSP_MULTI_PHY_STOP);
  appendInt8u(phyIndex);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspMultiPhySetRadioPower(
  uint8_t phyIndex,
  int8_t power)
{
  EmberStatus status;
  startCommand(EZSP_MULTI_PHY_SET_RADIO_POWER);
  appendInt8u(phyIndex);
  appendInt8u(power);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSendLinkPowerDeltaRequest(void)
{
  EmberStatus status;
  startCommand(EZSP_SEND_LINK_POWER_DELTA_REQUEST);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspMultiPhySetRadioChannel(
  uint8_t phyIndex,
  uint8_t page,
  uint8_t channel)
{
  EmberStatus status;
  startCommand(EZSP_MULTI_PHY_SET_RADIO_CHANNEL);
  appendInt8u(phyIndex);
  appendInt8u(page);
  appendInt8u(channel);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetDutyCycleState(
  EmberDutyCycleState *returnedState)
{
  EmberStatus status;
  startCommand(EZSP_GET_DUTY_CYCLE_STATE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *returnedState = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetDutyCycleLimitsInStack(
  EmberDutyCycleLimits *limits)
{
  EmberStatus status;
  startCommand(EZSP_SET_DUTY_CYCLE_LIMITS_IN_STACK);
  appendEmberDutyCycleLimits(limits);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetDutyCycleLimits(
  EmberDutyCycleLimits *returnedLimits)
{
  EmberStatus status;
  startCommand(EZSP_GET_DUTY_CYCLE_LIMITS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberDutyCycleLimits(returnedLimits);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetCurrentDutyCycle(
  uint8_t maxDevices,
  uint8_t *arrayOfDeviceDutyCycles)
{
  EmberStatus status;
  startCommand(EZSP_GET_CURRENT_DUTY_CYCLE);
  appendInt8u(maxDevices);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchInt8uArray(134, arrayOfDeviceDutyCycles);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetFirstBeacon(
  EmberBeaconIterator *beaconIterator)
{
  EmberStatus status;
  startCommand(EZSP_GET_FIRST_BEACON);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberBeaconIterator(beaconIterator);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetNextBeacon(
  EmberBeaconData *beacon)
{
  EmberStatus status;
  startCommand(EZSP_GET_NEXT_BEACON);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberBeaconData(beacon);
    return status;
  }
  return sendStatus;
}

uint8_t ezspGetNumStoredBeacons(void)
{
  uint8_t numBeacons;
  startCommand(EZSP_GET_NUM_STORED_BEACONS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    numBeacons = fetchInt8u();
    return numBeacons;
  }
  return 255;
}

void ezspClearStoredBeacons(void)
{
  startCommand(EZSP_CLEAR_STORED_BEACONS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

EmberStatus ezspSetLogicalAndRadioChannel(
  uint8_t radioChannel)
{
  EmberStatus status;
  startCommand(EZSP_SET_LOGICAL_AND_RADIO_CHANNEL);
  appendInt8u(radioChannel);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

//------------------------------------------------------------------------------
// Binding Frames
//------------------------------------------------------------------------------

EmberStatus ezspClearBindingTable(void)
{
  EmberStatus status;
  startCommand(EZSP_CLEAR_BINDING_TABLE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetBinding(
  uint8_t index,
  EmberBindingTableEntry *value)
{
  EmberStatus status;
  startCommand(EZSP_SET_BINDING);
  appendInt8u(index);
  appendEmberBindingTableEntry(value);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetBinding(
  uint8_t index,
  EmberBindingTableEntry *value)
{
  EmberStatus status;
  startCommand(EZSP_GET_BINDING);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberBindingTableEntry(value);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspDeleteBinding(
  uint8_t index)
{
  EmberStatus status;
  startCommand(EZSP_DELETE_BINDING);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

bool ezspBindingIsActive(
  uint8_t index)
{
  bool active;
  startCommand(EZSP_BINDING_IS_ACTIVE);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    active = fetchInt8u();
    return active;
  }
  return false;
}

EmberNodeId ezspGetBindingRemoteNodeId(
  uint8_t index)
{
  EmberNodeId nodeId;
  startCommand(EZSP_GET_BINDING_REMOTE_NODE_ID);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    nodeId = fetchInt16u();
    return nodeId;
  }
  return 0xFFFE;
}

void ezspSetBindingRemoteNodeId(
  uint8_t index,
  EmberNodeId nodeId)
{
  startCommand(EZSP_SET_BINDING_REMOTE_NODE_ID);
  appendInt8u(index);
  appendInt16u(nodeId);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

//------------------------------------------------------------------------------
// Messaging Frames
//------------------------------------------------------------------------------

uint8_t ezspMaximumPayloadLength(void)
{
  uint8_t apsLength;
  startCommand(EZSP_MAXIMUM_PAYLOAD_LENGTH);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    apsLength = fetchInt8u();
    return apsLength;
  }
  return 255;
}

EmberStatus ezspSendUnicast(
  EmberOutgoingMessageType type,
  EmberNodeId indexOrDestination,
  EmberApsFrame *apsFrame,
  uint8_t messageTag,
  uint8_t messageLength,
  uint8_t *messageContents,
  uint8_t *sequence)
{
  EmberStatus status;
  startCommand(EZSP_SEND_UNICAST);
  appendInt8u(type);
  appendInt16u(indexOrDestination);
  appendEmberApsFrame(apsFrame);
  appendInt8u(messageTag);
  appendInt8u(messageLength);
  appendInt8uArray(messageLength, messageContents);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *sequence = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSendBroadcast(
  EmberNodeId destination,
  EmberApsFrame *apsFrame,
  uint8_t radius,
  uint8_t messageTag,
  uint8_t messageLength,
  uint8_t *messageContents,
  uint8_t *sequence)
{
  EmberStatus status;
  startCommand(EZSP_SEND_BROADCAST);
  appendInt16u(destination);
  appendEmberApsFrame(apsFrame);
  appendInt8u(radius);
  appendInt8u(messageTag);
  appendInt8u(messageLength);
  appendInt8uArray(messageLength, messageContents);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *sequence = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspProxyBroadcast(
  EmberNodeId source,
  EmberNodeId destination,
  uint8_t nwkSequence,
  EmberApsFrame *apsFrame,
  uint8_t radius,
  uint8_t messageTag,
  uint8_t messageLength,
  uint8_t *messageContents,
  uint8_t *apsSequence)
{
  EmberStatus status;
  startCommand(EZSP_PROXY_BROADCAST);
  appendInt16u(source);
  appendInt16u(destination);
  appendInt8u(nwkSequence);
  appendEmberApsFrame(apsFrame);
  appendInt8u(radius);
  appendInt8u(messageTag);
  appendInt8u(messageLength);
  appendInt8uArray(messageLength, messageContents);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *apsSequence = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSendMulticast(
  EmberApsFrame *apsFrame,
  uint8_t hops,
  uint8_t nonmemberRadius,
  uint8_t messageTag,
  uint8_t messageLength,
  uint8_t *messageContents,
  uint8_t *sequence)
{
  EmberStatus status;
  startCommand(EZSP_SEND_MULTICAST);
  appendEmberApsFrame(apsFrame);
  appendInt8u(hops);
  appendInt8u(nonmemberRadius);
  appendInt8u(messageTag);
  appendInt8u(messageLength);
  appendInt8uArray(messageLength, messageContents);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *sequence = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSendMulticastWithAlias(
  EmberApsFrame *apsFrame,
  uint8_t hops,
  uint8_t nonmemberRadius,
  uint16_t alias,
  uint8_t nwkSequence,
  uint8_t messageTag,
  uint8_t messageLength,
  uint8_t *messageContents,
  uint8_t *sequence)
{
  EmberStatus status;
  startCommand(EZSP_SEND_MULTICAST_WITH_ALIAS);
  appendEmberApsFrame(apsFrame);
  appendInt8u(hops);
  appendInt8u(nonmemberRadius);
  appendInt16u(alias);
  appendInt8u(nwkSequence);
  appendInt8u(messageTag);
  appendInt8u(messageLength);
  appendInt8uArray(messageLength, messageContents);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *sequence = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSendReply(
  EmberNodeId sender,
  EmberApsFrame *apsFrame,
  uint8_t messageLength,
  uint8_t *messageContents)
{
  EmberStatus status;
  startCommand(EZSP_SEND_REPLY);
  appendInt16u(sender);
  appendEmberApsFrame(apsFrame);
  appendInt8u(messageLength);
  appendInt8uArray(messageLength, messageContents);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSendManyToOneRouteRequest(
  uint16_t concentratorType,
  uint8_t radius)
{
  EmberStatus status;
  startCommand(EZSP_SEND_MANY_TO_ONE_ROUTE_REQUEST);
  appendInt16u(concentratorType);
  appendInt8u(radius);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspPollForData(
  uint16_t interval,
  EmberEventUnits units,
  uint8_t failureLimit)
{
  EmberStatus status;
  startCommand(EZSP_POLL_FOR_DATA);
  appendInt16u(interval);
  appendInt8u(units);
  appendInt8u(failureLimit);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

uint32_t ezspSetSourceRouteDiscoveryMode(
  uint8_t mode)
{
  uint32_t remainingTime;
  startCommand(EZSP_SET_SOURCE_ROUTE_DISCOVERY_MODE);
  appendInt8u(mode);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    remainingTime = fetchInt32u();
    return remainingTime;
  }
  return 255;
}

EmberStatus ezspSetSourceRoute(
  EmberNodeId destination,
  uint8_t relayCount,
  uint16_t *relayList)
{
  EmberStatus status;
  startCommand(EZSP_SET_SOURCE_ROUTE);
  appendInt16u(destination);
  appendInt8u(relayCount);
  appendInt16uArray(relayCount, relayList);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspUnicastCurrentNetworkKey(
  EmberNodeId targetShort,
  EmberEUI64 targetLong,
  EmberNodeId parentShortId)
{
  EmberStatus status;
  startCommand(EZSP_UNICAST_CURRENT_NETWORK_KEY);
  appendInt16u(targetShort);
  appendInt8uArray(8, targetLong);
  appendInt16u(parentShortId);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

bool ezspAddressTableEntryIsActive(
  uint8_t addressTableIndex)
{
  bool active;
  startCommand(EZSP_ADDRESS_TABLE_ENTRY_IS_ACTIVE);
  appendInt8u(addressTableIndex);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    active = fetchInt8u();
    return active;
  }
  return false;
}

EmberStatus ezspSetAddressTableRemoteEui64(
  uint8_t addressTableIndex,
  EmberEUI64 eui64)
{
  EmberStatus status;
  startCommand(EZSP_SET_ADDRESS_TABLE_REMOTE_EUI64);
  appendInt8u(addressTableIndex);
  appendInt8uArray(8, eui64);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

void ezspSetAddressTableRemoteNodeId(
  uint8_t addressTableIndex,
  EmberNodeId id)
{
  startCommand(EZSP_SET_ADDRESS_TABLE_REMOTE_NODE_ID);
  appendInt8u(addressTableIndex);
  appendInt16u(id);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

void ezspGetAddressTableRemoteEui64(
  uint8_t addressTableIndex,
  EmberEUI64 eui64)
{
  startCommand(EZSP_GET_ADDRESS_TABLE_REMOTE_EUI64);
  appendInt8u(addressTableIndex);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    fetchInt8uArray(8, eui64);
  }
}

EmberNodeId ezspGetAddressTableRemoteNodeId(
  uint8_t addressTableIndex)
{
  EmberNodeId nodeId;
  startCommand(EZSP_GET_ADDRESS_TABLE_REMOTE_NODE_ID);
  appendInt8u(addressTableIndex);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    nodeId = fetchInt16u();
    return nodeId;
  }
  return 0xFFFE;
}

void ezspSetExtendedTimeout(
  EmberEUI64 remoteEui64,
  bool extendedTimeout)
{
  startCommand(EZSP_SET_EXTENDED_TIMEOUT);
  appendInt8uArray(8, remoteEui64);
  appendInt8u(extendedTimeout);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

bool ezspGetExtendedTimeout(
  EmberEUI64 remoteEui64)
{
  bool extendedTimeout;
  startCommand(EZSP_GET_EXTENDED_TIMEOUT);
  appendInt8uArray(8, remoteEui64);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    extendedTimeout = fetchInt8u();
    return extendedTimeout;
  }
  return false;
}

EmberStatus ezspReplaceAddressTableEntry(
  uint8_t addressTableIndex,
  EmberEUI64 newEui64,
  EmberNodeId newId,
  bool newExtendedTimeout,
  EmberEUI64 oldEui64,
  EmberNodeId *oldId,
  bool *oldExtendedTimeout)
{
  EmberStatus status;
  startCommand(EZSP_REPLACE_ADDRESS_TABLE_ENTRY);
  appendInt8u(addressTableIndex);
  appendInt8uArray(8, newEui64);
  appendInt16u(newId);
  appendInt8u(newExtendedTimeout);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchInt8uArray(8, oldEui64);
    *oldId = fetchInt16u();
    *oldExtendedTimeout = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberNodeId ezspLookupNodeIdByEui64(
  EmberEUI64 eui64)
{
  EmberNodeId nodeId;
  startCommand(EZSP_LOOKUP_NODE_ID_BY_EUI64);
  appendInt8uArray(8, eui64);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    nodeId = fetchInt16u();
    return nodeId;
  }
  return 0xFFFE;
}

EmberStatus ezspLookupEui64ByNodeId(
  EmberNodeId nodeId,
  EmberEUI64 eui64)
{
  EmberStatus status;
  startCommand(EZSP_LOOKUP_EUI64_BY_NODE_ID);
  appendInt16u(nodeId);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchInt8uArray(8, eui64);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetMulticastTableEntry(
  uint8_t index,
  EmberMulticastTableEntry *value)
{
  EmberStatus status;
  startCommand(EZSP_GET_MULTICAST_TABLE_ENTRY);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberMulticastTableEntry(value);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetMulticastTableEntry(
  uint8_t index,
  EmberMulticastTableEntry *value)
{
  EmberStatus status;
  startCommand(EZSP_SET_MULTICAST_TABLE_ENTRY);
  appendInt8u(index);
  appendEmberMulticastTableEntry(value);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspWriteNodeData(
  bool erase)
{
  EmberStatus status;
  startCommand(EZSP_WRITE_NODE_DATA);
  appendInt8u(erase);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSendRawMessage(
  uint8_t messageLength,
  uint8_t *messageContents)
{
  EmberStatus status;
  startCommand(EZSP_SEND_RAW_MESSAGE);
  appendInt8u(messageLength);
  appendInt8uArray(messageLength, messageContents);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSendRawMessageExtended(
  uint8_t messageLength,
  uint8_t *messageContents,
  uint8_t priority,
  bool useCca)
{
  EmberStatus status;
  startCommand(EZSP_SEND_RAW_MESSAGE_EXTENDED);
  appendInt8u(messageLength);
  appendInt8uArray(messageLength, messageContents);
  appendInt8u(priority);
  appendInt8u(useCca);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

void ezspSetMacPollFailureWaitTime(
  uint32_t waitBeforeRetryIntervalMs)
{
  startCommand(EZSP_SET_MAC_POLL_FAILURE_WAIT_TIME);
  appendInt32u(waitBeforeRetryIntervalMs);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

EmberStatus ezspSetBeaconClassificationParams(
  EmberBeaconClassificationParams *param)
{
  EmberStatus status;
  startCommand(EZSP_SET_BEACON_CLASSIFICATION_PARAMS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberBeaconClassificationParams(param);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetBeaconClassificationParams(
  EmberBeaconClassificationParams *param)
{
  EmberStatus status;
  startCommand(EZSP_GET_BEACON_CLASSIFICATION_PARAMS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberBeaconClassificationParams(param);
    return status;
  }
  return sendStatus;
}

//------------------------------------------------------------------------------
// Security Frames
//------------------------------------------------------------------------------

EmberStatus ezspSetInitialSecurityState(
  EmberInitialSecurityState *state)
{
  EmberStatus success;
  startCommand(EZSP_SET_INITIAL_SECURITY_STATE);
  appendEmberInitialSecurityState(state);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    success = fetchInt8u();
    return success;
  }
  return sendStatus;
}

EmberStatus ezspGetCurrentSecurityState(
  EmberCurrentSecurityState *state)
{
  EmberStatus status;
  startCommand(EZSP_GET_CURRENT_SECURITY_STATE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberCurrentSecurityState(state);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetKey(
  EmberKeyType keyType,
  EmberKeyStruct *keyStruct)
{
  EmberStatus status;
  startCommand(EZSP_GET_KEY);
  appendInt8u(keyType);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberKeyStruct(keyStruct);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetKeyTableEntry(
  uint8_t index,
  EmberKeyStruct *keyStruct)
{
  EmberStatus status;
  startCommand(EZSP_GET_KEY_TABLE_ENTRY);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberKeyStruct(keyStruct);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetKeyTableEntry(
  uint8_t index,
  EmberEUI64 address,
  bool linkKey,
  EmberKeyData *keyData)
{
  EmberStatus status;
  startCommand(EZSP_SET_KEY_TABLE_ENTRY);
  appendInt8u(index);
  appendInt8uArray(8, address);
  appendInt8u(linkKey);
  appendEmberKeyData(keyData);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

uint8_t ezspFindKeyTableEntry(
  EmberEUI64 address,
  bool linkKey)
{
  uint8_t index;
  startCommand(EZSP_FIND_KEY_TABLE_ENTRY);
  appendInt8uArray(8, address);
  appendInt8u(linkKey);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    index = fetchInt8u();
    return index;
  }
  return 255;
}

EmberStatus ezspAddOrUpdateKeyTableEntry(
  EmberEUI64 address,
  bool linkKey,
  EmberKeyData *keyData)
{
  EmberStatus status;
  startCommand(EZSP_ADD_OR_UPDATE_KEY_TABLE_ENTRY);
  appendInt8uArray(8, address);
  appendInt8u(linkKey);
  appendEmberKeyData(keyData);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSendTrustCenterLinkKey(
  EmberNodeId destinationNodeId,
  EmberEUI64 destinationEui64)
{
  EmberStatus status;
  startCommand(EZSP_SEND_TRUST_CENTER_LINK_KEY);
  appendInt16u(destinationNodeId);
  appendInt8uArray(8, destinationEui64);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspEraseKeyTableEntry(
  uint8_t index)
{
  EmberStatus status;
  startCommand(EZSP_ERASE_KEY_TABLE_ENTRY);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspClearKeyTable(void)
{
  EmberStatus status;
  startCommand(EZSP_CLEAR_KEY_TABLE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspRequestLinkKey(
  EmberEUI64 partner)
{
  EmberStatus status;
  startCommand(EZSP_REQUEST_LINK_KEY);
  appendInt8uArray(8, partner);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspUpdateTcLinkKey(
  uint8_t maxAttempts)
{
  EmberStatus status;
  startCommand(EZSP_UPDATE_TC_LINK_KEY);
  appendInt8u(maxAttempts);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspAddTransientLinkKey(
  EmberEUI64 partner,
  EmberKeyData *transientKey)
{
  EmberStatus status;
  startCommand(EZSP_ADD_TRANSIENT_LINK_KEY);
  appendInt8uArray(8, partner);
  appendEmberKeyData(transientKey);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

void ezspClearTransientLinkKeys(void)
{
  startCommand(EZSP_CLEAR_TRANSIENT_LINK_KEYS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

EmberStatus ezspGetTransientLinkKey(
  EmberEUI64 eui,
  EmberTransientKeyData *transientKeyData)
{
  EmberStatus status;
  startCommand(EZSP_GET_TRANSIENT_LINK_KEY);
  appendInt8uArray(8, eui);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberTransientKeyData(transientKeyData);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetTransientKeyTableEntry(
  uint8_t index,
  EmberTransientKeyData *transientKeyData)
{
  EmberStatus status;
  startCommand(EZSP_GET_TRANSIENT_KEY_TABLE_ENTRY);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberTransientKeyData(transientKeyData);
    return status;
  }
  return sendStatus;
}

//------------------------------------------------------------------------------
// Trust Center Frames
//------------------------------------------------------------------------------

EmberStatus ezspBroadcastNextNetworkKey(
  EmberKeyData *key)
{
  EmberStatus status;
  startCommand(EZSP_BROADCAST_NEXT_NETWORK_KEY);
  appendEmberKeyData(key);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspBroadcastNetworkKeySwitch(void)
{
  EmberStatus status;
  startCommand(EZSP_BROADCAST_NETWORK_KEY_SWITCH);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspBecomeTrustCenter(
  EmberKeyData *newNetworkKey)
{
  EmberStatus status;
  startCommand(EZSP_BECOME_TRUST_CENTER);
  appendEmberKeyData(newNetworkKey);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspAesMmoHash(
  EmberAesMmoHashContext *context,
  bool finalize,
  uint8_t length,
  uint8_t *data,
  EmberAesMmoHashContext *returnContext)
{
  EmberStatus status;
  startCommand(EZSP_AES_MMO_HASH);
  appendEmberAesMmoHashContext(context);
  appendInt8u(finalize);
  appendInt8u(length);
  appendInt8uArray(length, data);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberAesMmoHashContext(returnContext);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspRemoveDevice(
  EmberNodeId destShort,
  EmberEUI64 destLong,
  EmberEUI64 targetLong)
{
  EmberStatus status;
  startCommand(EZSP_REMOVE_DEVICE);
  appendInt16u(destShort);
  appendInt8uArray(8, destLong);
  appendInt8uArray(8, targetLong);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspUnicastNwkKeyUpdate(
  EmberNodeId destShort,
  EmberEUI64 destLong,
  EmberKeyData *key)
{
  EmberStatus status;
  startCommand(EZSP_UNICAST_NWK_KEY_UPDATE);
  appendInt16u(destShort);
  appendInt8uArray(8, destLong);
  appendEmberKeyData(key);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

//------------------------------------------------------------------------------
// Certificate Based Key Exchange (CBKE) Frames
//------------------------------------------------------------------------------

EmberStatus ezspGenerateCbkeKeys(void)
{
  EmberStatus status;
  startCommand(EZSP_GENERATE_CBKE_KEYS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspCalculateSmacs(
  bool amInitiator,
  EmberCertificateData *partnerCertificate,
  EmberPublicKeyData *partnerEphemeralPublicKey)
{
  EmberStatus status;
  startCommand(EZSP_CALCULATE_SMACS);
  appendInt8u(amInitiator);
  appendEmberCertificateData(partnerCertificate);
  appendEmberPublicKeyData(partnerEphemeralPublicKey);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGenerateCbkeKeys283k1(void)
{
  EmberStatus status;
  startCommand(EZSP_GENERATE_CBKE_KEYS283K1);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspCalculateSmacs283k1(
  bool amInitiator,
  EmberCertificate283k1Data *partnerCertificate,
  EmberPublicKey283k1Data *partnerEphemeralPublicKey)
{
  EmberStatus status;
  startCommand(EZSP_CALCULATE_SMACS283K1);
  appendInt8u(amInitiator);
  appendEmberCertificate283k1Data(partnerCertificate);
  appendEmberPublicKey283k1Data(partnerEphemeralPublicKey);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspClearTemporaryDataMaybeStoreLinkKey(
  bool storeLinkKey)
{
  EmberStatus status;
  startCommand(EZSP_CLEAR_TEMPORARY_DATA_MAYBE_STORE_LINK_KEY);
  appendInt8u(storeLinkKey);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspClearTemporaryDataMaybeStoreLinkKey283k1(
  bool storeLinkKey)
{
  EmberStatus status;
  startCommand(EZSP_CLEAR_TEMPORARY_DATA_MAYBE_STORE_LINK_KEY283K1);
  appendInt8u(storeLinkKey);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetCertificate(
  EmberCertificateData *localCert)
{
  EmberStatus status;
  startCommand(EZSP_GET_CERTIFICATE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberCertificateData(localCert);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetCertificate283k1(
  EmberCertificate283k1Data *localCert)
{
  EmberStatus status;
  startCommand(EZSP_GET_CERTIFICATE283K1);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberCertificate283k1Data(localCert);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspDsaSign(
  uint8_t messageLength,
  uint8_t *messageContents)
{
  EmberStatus status;
  startCommand(EZSP_DSA_SIGN);
  appendInt8u(messageLength);
  appendInt8uArray(messageLength, messageContents);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspDsaVerify(
  EmberMessageDigest *digest,
  EmberCertificateData *signerCertificate,
  EmberSignatureData *receivedSig)
{
  EmberStatus status;
  startCommand(EZSP_DSA_VERIFY);
  appendEmberMessageDigest(digest);
  appendEmberCertificateData(signerCertificate);
  appendEmberSignatureData(receivedSig);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspDsaVerify283k1(
  EmberMessageDigest *digest,
  EmberCertificate283k1Data *signerCertificate,
  EmberSignature283k1Data *receivedSig)
{
  EmberStatus status;
  startCommand(EZSP_DSA_VERIFY283K1);
  appendEmberMessageDigest(digest);
  appendEmberCertificate283k1Data(signerCertificate);
  appendEmberSignature283k1Data(receivedSig);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetPreinstalledCbkeData(
  EmberPublicKeyData *caPublic,
  EmberCertificateData *myCert,
  EmberPrivateKeyData *myKey)
{
  EmberStatus status;
  startCommand(EZSP_SET_PREINSTALLED_CBKE_DATA);
  appendEmberPublicKeyData(caPublic);
  appendEmberCertificateData(myCert);
  appendEmberPrivateKeyData(myKey);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSavePreinstalledCbkeData283k1(void)
{
  EmberStatus status;
  startCommand(EZSP_SAVE_PREINSTALLED_CBKE_DATA283K1);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

//------------------------------------------------------------------------------
// Mfglib Frames
//------------------------------------------------------------------------------

EmberStatus ezspMfglibStart(
  bool rxCallback)
{
  EmberStatus status;
  startCommand(EZSP_MFGLIB_START);
  appendInt8u(rxCallback);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus mfglibEnd(void)
{
  EmberStatus status;
  startCommand(EZSP_MFGLIB_END);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus mfglibStartTone(void)
{
  EmberStatus status;
  startCommand(EZSP_MFGLIB_START_TONE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus mfglibStopTone(void)
{
  EmberStatus status;
  startCommand(EZSP_MFGLIB_STOP_TONE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus mfglibStartStream(void)
{
  EmberStatus status;
  startCommand(EZSP_MFGLIB_START_STREAM);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus mfglibStopStream(void)
{
  EmberStatus status;
  startCommand(EZSP_MFGLIB_STOP_STREAM);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus mfglibSendPacket(
  uint8_t packetLength,
  uint8_t *packetContents)
{
  EmberStatus status;
  startCommand(EZSP_MFGLIB_SEND_PACKET);
  appendInt8u(packetLength);
  appendInt8uArray(packetLength, packetContents);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus mfglibSetChannel(
  uint8_t channel)
{
  EmberStatus status;
  startCommand(EZSP_MFGLIB_SET_CHANNEL);
  appendInt8u(channel);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

uint8_t mfglibGetChannel(void)
{
  uint8_t channel;
  startCommand(EZSP_MFGLIB_GET_CHANNEL);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    channel = fetchInt8u();
    return channel;
  }
  return 255;
}

EmberStatus mfglibSetPower(
  uint16_t txPowerMode,
  int8_t power)
{
  EmberStatus status;
  startCommand(EZSP_MFGLIB_SET_POWER);
  appendInt16u(txPowerMode);
  appendInt8u(power);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

int8_t mfglibGetPower(void)
{
  int8_t power;
  startCommand(EZSP_MFGLIB_GET_POWER);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    power = fetchInt8u();
    return power;
  }
  return -128;
}

//------------------------------------------------------------------------------
// Bootloader Frames
//------------------------------------------------------------------------------

EmberStatus ezspLaunchStandaloneBootloader(
  uint8_t mode)
{
  EmberStatus status;
  startCommand(EZSP_LAUNCH_STANDALONE_BOOTLOADER);
  appendInt8u(mode);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSendBootloadMessage(
  bool broadcast,
  EmberEUI64 destEui64,
  uint8_t messageLength,
  uint8_t *messageContents)
{
  EmberStatus status;
  startCommand(EZSP_SEND_BOOTLOAD_MESSAGE);
  appendInt8u(broadcast);
  appendInt8uArray(8, destEui64);
  appendInt8u(messageLength);
  appendInt8uArray(messageLength, messageContents);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

uint16_t ezspGetStandaloneBootloaderVersionPlatMicroPhy(
  uint8_t *nodePlat,
  uint8_t *nodeMicro,
  uint8_t *nodePhy)
{
  uint16_t bootloader_version;
  startCommand(EZSP_GET_STANDALONE_BOOTLOADER_VERSION_PLAT_MICRO_PHY);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    bootloader_version = fetchInt16u();
    *nodePlat = fetchInt8u();
    *nodeMicro = fetchInt8u();
    *nodePhy = fetchInt8u();
    return bootloader_version;
  }
  return 255;
}

void ezspAesEncrypt(
  uint8_t *plaintext,
  uint8_t *key,
  uint8_t *ciphertext)
{
  startCommand(EZSP_AES_ENCRYPT);
  appendInt8uArray(16, plaintext);
  appendInt8uArray(16, key);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    fetchInt8uArray(16, ciphertext);
  }
}

//------------------------------------------------------------------------------
// ZLL Frames
//------------------------------------------------------------------------------

EmberStatus ezspZllNetworkOps(
  EmberZllNetwork *networkInfo,
  EzspZllNetworkOperation op,
  int8_t radioTxPower)
{
  EmberStatus status;
  startCommand(EZSP_ZLL_NETWORK_OPS);
  appendEmberZllNetwork(networkInfo);
  appendInt8u(op);
  appendInt8u(radioTxPower);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspZllSetInitialSecurityState(
  EmberKeyData *networkKey,
  EmberZllInitialSecurityState *securityState)
{
  EmberStatus status;
  startCommand(EZSP_ZLL_SET_INITIAL_SECURITY_STATE);
  appendEmberKeyData(networkKey);
  appendEmberZllInitialSecurityState(securityState);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspZllSetSecurityStateWithoutKey(
  EmberZllInitialSecurityState *securityState)
{
  EmberStatus status;
  startCommand(EZSP_ZLL_SET_SECURITY_STATE_WITHOUT_KEY);
  appendEmberZllInitialSecurityState(securityState);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspZllStartScan(
  uint32_t channelMask,
  int8_t radioPowerForScan,
  EmberNodeType nodeType)
{
  EmberStatus status;
  startCommand(EZSP_ZLL_START_SCAN);
  appendInt32u(channelMask);
  appendInt8u(radioPowerForScan);
  appendInt8u(nodeType);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspZllSetRxOnWhenIdle(
  uint32_t durationMs)
{
  EmberStatus status;
  startCommand(EZSP_ZLL_SET_RX_ON_WHEN_IDLE);
  appendInt32u(durationMs);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

void ezspZllGetTokens(
  EmberTokTypeStackZllData *data,
  EmberTokTypeStackZllSecurity *security)
{
  startCommand(EZSP_ZLL_GET_TOKENS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    fetchEmberTokTypeStackZllData(data);
    fetchEmberTokTypeStackZllSecurity(security);
  }
}

void ezspZllSetDataToken(
  EmberTokTypeStackZllData *data)
{
  startCommand(EZSP_ZLL_SET_DATA_TOKEN);
  appendEmberTokTypeStackZllData(data);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

void ezspZllSetNonZllNetwork(void)
{
  startCommand(EZSP_ZLL_SET_NON_ZLL_NETWORK);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

bool ezspIsZllNetwork(void)
{
  bool isZllNetwork;
  startCommand(EZSP_IS_ZLL_NETWORK);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    isZllNetwork = fetchInt8u();
    return isZllNetwork;
  }
  return false;
}

void ezspZllSetRadioIdleMode(
  EmberRadioPowerMode mode)
{
  startCommand(EZSP_ZLL_SET_RADIO_IDLE_MODE);
  appendInt8u(mode);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

uint8_t ezspZllGetRadioIdleMode(void)
{
  uint8_t radioIdleMode;
  startCommand(EZSP_ZLL_GET_RADIO_IDLE_MODE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    radioIdleMode = fetchInt8u();
    return radioIdleMode;
  }
  return 255;
}

void ezspSetZllNodeType(
  EmberNodeType nodeType)
{
  startCommand(EZSP_SET_ZLL_NODE_TYPE);
  appendInt8u(nodeType);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

void ezspSetZllAdditionalState(
  uint16_t state)
{
  startCommand(EZSP_SET_ZLL_ADDITIONAL_STATE);
  appendInt16u(state);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

bool ezspZllOperationInProgress(void)
{
  bool zllOperationInProgress;
  startCommand(EZSP_ZLL_OPERATION_IN_PROGRESS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    zllOperationInProgress = fetchInt8u();
    return zllOperationInProgress;
  }
  return false;
}

bool ezspZllRxOnWhenIdleGetActive(void)
{
  bool zllRxOnWhenIdleGetActive;
  startCommand(EZSP_ZLL_RX_ON_WHEN_IDLE_GET_ACTIVE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    zllRxOnWhenIdleGetActive = fetchInt8u();
    return zllRxOnWhenIdleGetActive;
  }
  return false;
}

void ezspZllScanningComplete(void)
{
  startCommand(EZSP_ZLL_SCANNING_COMPLETE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

uint32_t ezspGetZllPrimaryChannelMask(void)
{
  uint32_t zllPrimaryChannelMask;
  startCommand(EZSP_GET_ZLL_PRIMARY_CHANNEL_MASK);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    zllPrimaryChannelMask = fetchInt32u();
    return zllPrimaryChannelMask;
  }
  return 255;
}

uint32_t ezspGetZllSecondaryChannelMask(void)
{
  uint32_t zllSecondaryChannelMask;
  startCommand(EZSP_GET_ZLL_SECONDARY_CHANNEL_MASK);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    zllSecondaryChannelMask = fetchInt32u();
    return zllSecondaryChannelMask;
  }
  return 255;
}

void ezspSetZllPrimaryChannelMask(
  uint32_t zllPrimaryChannelMask)
{
  startCommand(EZSP_SET_ZLL_PRIMARY_CHANNEL_MASK);
  appendInt32u(zllPrimaryChannelMask);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

void ezspSetZllSecondaryChannelMask(
  uint32_t zllSecondaryChannelMask)
{
  startCommand(EZSP_SET_ZLL_SECONDARY_CHANNEL_MASK);
  appendInt32u(zllSecondaryChannelMask);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

void ezspZllClearTokens(void)
{
  startCommand(EZSP_ZLL_CLEAR_TOKENS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

//------------------------------------------------------------------------------
// WWAH Frames
//------------------------------------------------------------------------------

void ezspSetParentClassificationEnabled(
  bool enabled)
{
  startCommand(EZSP_SET_PARENT_CLASSIFICATION_ENABLED);
  appendInt8u(enabled);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

bool ezspGetParentClassificationEnabled(void)
{
  bool enabled;
  startCommand(EZSP_GET_PARENT_CLASSIFICATION_ENABLED);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    enabled = fetchInt8u();
    return enabled;
  }
  return false;
}

void ezspSetLongUpTime(
  bool hasLongUpTime)
{
  startCommand(EZSP_SET_LONG_UP_TIME);
  appendInt8u(hasLongUpTime);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

void ezspSetHubConnectivity(
  bool connected)
{
  startCommand(EZSP_SET_HUB_CONNECTIVITY);
  appendInt8u(connected);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

bool ezspIsUpTimeLong(void)
{
  bool hasLongUpTime;
  startCommand(EZSP_IS_UP_TIME_LONG);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    hasLongUpTime = fetchInt8u();
    return hasLongUpTime;
  }
  return false;
}

bool ezspIsHubConnected(void)
{
  bool isHubConnected;
  startCommand(EZSP_IS_HUB_CONNECTED);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    isHubConnected = fetchInt8u();
    return isHubConnected;
  }
  return false;
}

//------------------------------------------------------------------------------
// Green Power Frames
//------------------------------------------------------------------------------

bool ezspGpProxyTableProcessGpPairing(
  uint32_t options,
  EmberGpAddress *addr,
  uint8_t commMode,
  uint16_t sinkNetworkAddress,
  uint16_t sinkGroupId,
  uint16_t assignedAlias,
  uint8_t *sinkIeeeAddress,
  EmberKeyData *gpdKey,
  uint32_t gpdSecurityFrameCounter,
  uint8_t forwardingRadius)
{
  bool gpPairingAdded;
  startCommand(EZSP_GP_PROXY_TABLE_PROCESS_GP_PAIRING);
  appendInt32u(options);
  appendEmberGpAddress(addr);
  appendInt8u(commMode);
  appendInt16u(sinkNetworkAddress);
  appendInt16u(sinkGroupId);
  appendInt16u(assignedAlias);
  appendInt8uArray(8, sinkIeeeAddress);
  appendEmberKeyData(gpdKey);
  appendInt32u(gpdSecurityFrameCounter);
  appendInt8u(forwardingRadius);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    gpPairingAdded = fetchInt8u();
    return gpPairingAdded;
  }
  return false;
}

EmberStatus ezspDGpSend(
  bool action,
  bool useCca,
  EmberGpAddress *addr,
  uint8_t gpdCommandId,
  uint8_t gpdAsduLength,
  uint8_t *gpdAsdu,
  uint8_t gpepHandle,
  uint16_t gpTxQueueEntryLifetimeMs)
{
  EmberStatus status;
  startCommand(EZSP_D_GP_SEND);
  appendInt8u(action);
  appendInt8u(useCca);
  appendEmberGpAddress(addr);
  appendInt8u(gpdCommandId);
  appendInt8u(gpdAsduLength);
  appendInt8uArray(gpdAsduLength, gpdAsdu);
  appendInt8u(gpepHandle);
  appendInt16u(gpTxQueueEntryLifetimeMs);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGpProxyTableGetEntry(
  uint8_t proxyIndex,
  EmberGpProxyTableEntry *entry)
{
  EmberStatus status;
  startCommand(EZSP_GP_PROXY_TABLE_GET_ENTRY);
  appendInt8u(proxyIndex);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberGpProxyTableEntry(entry);
    return status;
  }
  return sendStatus;
}

uint8_t ezspGpProxyTableLookup(
  EmberGpAddress *addr)
{
  uint8_t index;
  startCommand(EZSP_GP_PROXY_TABLE_LOOKUP);
  appendEmberGpAddress(addr);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    index = fetchInt8u();
    return index;
  }
  return 255;
}

EmberStatus ezspGpSinkTableGetEntry(
  uint8_t sinkIndex,
  EmberGpSinkTableEntry *entry)
{
  EmberStatus status;
  startCommand(EZSP_GP_SINK_TABLE_GET_ENTRY);
  appendInt8u(sinkIndex);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberGpSinkTableEntry(entry);
    return status;
  }
  return sendStatus;
}

uint8_t ezspGpSinkTableLookup(
  EmberGpAddress *addr)
{
  uint8_t index;
  startCommand(EZSP_GP_SINK_TABLE_LOOKUP);
  appendEmberGpAddress(addr);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    index = fetchInt8u();
    return index;
  }
  return 255;
}

EmberStatus ezspGpSinkTableSetEntry(
  uint8_t sinkIndex,
  EmberGpSinkTableEntry *entry)
{
  EmberStatus status;
  startCommand(EZSP_GP_SINK_TABLE_SET_ENTRY);
  appendInt8u(sinkIndex);
  appendEmberGpSinkTableEntry(entry);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

void ezspGpSinkTableRemoveEntry(
  uint8_t sinkIndex)
{
  startCommand(EZSP_GP_SINK_TABLE_REMOVE_ENTRY);
  appendInt8u(sinkIndex);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

uint8_t ezspGpSinkTableFindOrAllocateEntry(
  EmberGpAddress *addr)
{
  uint8_t index;
  startCommand(EZSP_GP_SINK_TABLE_FIND_OR_ALLOCATE_ENTRY);
  appendEmberGpAddress(addr);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    index = fetchInt8u();
    return index;
  }
  return 255;
}

void ezspGpSinkTableClearAll(void)
{
  startCommand(EZSP_GP_SINK_TABLE_CLEAR_ALL);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

void ezspGpSinkTableInit(void)
{
  startCommand(EZSP_GP_SINK_TABLE_INIT);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

void ezspGpSinkTableSetSecurityFrameCounter(
  uint8_t index,
  uint32_t sfc)
{
  startCommand(EZSP_GP_SINK_TABLE_SET_SECURITY_FRAME_COUNTER);
  appendInt8u(index);
  appendInt32u(sfc);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

//------------------------------------------------------------------------------
// Secure EZSP Frames
//------------------------------------------------------------------------------

EzspStatus ezspSetSecurityKey(
  EmberKeyData *key,
  SecureEzspSecurityType securityType)
{
  EzspStatus status;
  startCommand(EZSP_SET_SECURITY_KEY);
  appendEmberKeyData(key);
  appendInt32u(securityType);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EzspStatus ezspSetSecurityParameters(
  SecureEzspSecurityLevel securityLevel,
  SecureEzspRandomNumber *hostRandomNumber,
  SecureEzspRandomNumber *returnNcpRandomNumber)
{
  EzspStatus status;
  startCommand(EZSP_SET_SECURITY_PARAMETERS);
  appendInt8u(securityLevel);
  appendSecureEzspRandomNumber(hostRandomNumber);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchSecureEzspRandomNumber(returnNcpRandomNumber);
    return status;
  }
  return sendStatus;
}

EzspStatus ezspResetToFactoryDefaults(void)
{
  EzspStatus status;
  startCommand(EZSP_RESET_TO_FACTORY_DEFAULTS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

EzspStatus ezspGetSecurityKeyStatus(
  SecureEzspSecurityType *returnSecurityType)
{
  EzspStatus status;
  startCommand(EZSP_GET_SECURITY_KEY_STATUS);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    *returnSecurityType = fetchInt32u();
    return status;
  }
  return sendStatus;
}

//------------------------------------------------------------------------------
// Token Interface Frames
//------------------------------------------------------------------------------

uint8_t ezspGetTokenCount(void)
{
  uint8_t count;
  startCommand(EZSP_GET_TOKEN_COUNT);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    count = fetchInt8u();
    return count;
  }
  return 255;
}

EmberStatus ezspGetTokenInfo(
  uint8_t index,
  EmberTokenInfo *tokenInfo)
{
  EmberStatus status;
  startCommand(EZSP_GET_TOKEN_INFO);
  appendInt8u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberTokenInfo(tokenInfo);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspGetTokenData(
  uint32_t token,
  uint32_t index,
  EmberTokenData *tokenData)
{
  EmberStatus status;
  startCommand(EZSP_GET_TOKEN_DATA);
  appendInt32u(token);
  appendInt32u(index);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    fetchEmberTokenData(tokenData);
    return status;
  }
  return sendStatus;
}

EmberStatus ezspSetTokenData(
  uint32_t token,
  uint32_t index,
  EmberTokenData *tokenData)
{
  EmberStatus status;
  startCommand(EZSP_SET_TOKEN_DATA);
  appendInt32u(token);
  appendInt32u(index);
  appendEmberTokenData(tokenData);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    status = fetchInt8u();
    return status;
  }
  return sendStatus;
}

void ezspResetNode(void)
{
  startCommand(EZSP_RESET_NODE);
  EzspStatus sendStatus = sendCommand();
  if (sendStatus == EZSP_SUCCESS) {
    EZSP_ASH_TRACE("%s(): sendCommand() error: 0x%x", __func__, sendStatus);
  }
}

static void callbackDispatch(void)
{
  callbackPointerInit();

  switch (emEzspGetFrameId()) {
    case EZSP_NO_CALLBACKS: {
      ezspNoCallbacks();
      break;
    }

    case EZSP_STACK_TOKEN_CHANGED_HANDLER: {
      uint16_t tokenAddress;
      tokenAddress = fetchInt16u();
      ezspStackTokenChangedHandler(tokenAddress);
      break;
    }

    case EZSP_TIMER_HANDLER: {
      uint8_t timerId;
      timerId = fetchInt8u();
      ezspTimerHandler(timerId);
      break;
    }

    case EZSP_COUNTER_ROLLOVER_HANDLER: {
      EmberCounterType type;
      type = fetchInt8u();
      ezspCounterRolloverHandler(type);
      break;
    }

    case EZSP_CUSTOM_FRAME_HANDLER: {
      uint8_t payloadLength;
      uint8_t *payload;
      payloadLength = fetchInt8u();
      payload = (uint8_t *)fetchInt8uPointer(payloadLength);
      ezspCustomFrameHandler(payloadLength, payload);
      break;
    }

    case EZSP_STACK_STATUS_HANDLER: {
      EmberStatus status;
      status = fetchInt8u();
      ezspStackStatusHandler(status);
      break;
    }

    case EZSP_ENERGY_SCAN_RESULT_HANDLER: {
      uint8_t channel;
      int8_t maxRssiValue;
      channel = fetchInt8u();
      maxRssiValue = fetchInt8u();
      ezspEnergyScanResultHandler(channel, maxRssiValue);
      break;
    }

    case EZSP_NETWORK_FOUND_HANDLER: {
      EmberZigbeeNetwork networkFound;
      uint8_t lastHopLqi;
      int8_t lastHopRssi;
      fetchEmberZigbeeNetwork(&networkFound);
      lastHopLqi = fetchInt8u();
      lastHopRssi = fetchInt8u();
      ezspNetworkFoundHandler(&networkFound, lastHopLqi, lastHopRssi);
      break;
    }

    case EZSP_SCAN_COMPLETE_HANDLER: {
      uint8_t channel;
      EmberStatus status;
      channel = fetchInt8u();
      status = fetchInt8u();
      ezspScanCompleteHandler(channel, status);
      break;
    }

    case EZSP_UNUSED_PAN_ID_FOUND_HANDLER: {
      EmberPanId panId;
      uint8_t channel;
      panId = fetchInt16u();
      channel = fetchInt8u();
      ezspUnusedPanIdFoundHandler(panId, channel);
      break;
    }

    case EZSP_CHILD_JOIN_HANDLER: {
      uint8_t index;
      bool joining;
      EmberNodeId childId;
      uint8_t childEui64[8];
      EmberNodeType childType;
      index = fetchInt8u();
      joining = fetchInt8u();
      childId = fetchInt16u();
      fetchInt8uArray(8, childEui64);
      childType = fetchInt8u();
      ezspChildJoinHandler(index, joining, childId, childEui64, childType);
      break;
    }

    case EZSP_DUTY_CYCLE_HANDLER: {
      uint8_t channelPage;
      uint8_t channel;
      EmberDutyCycleState state;
      uint8_t totalDevices;
      EmberPerDeviceDutyCycle arrayOfDeviceDutyCycles;
      channelPage = fetchInt8u();
      channel = fetchInt8u();
      state = fetchInt8u();
      totalDevices = fetchInt8u();
      fetchEmberPerDeviceDutyCycle(&arrayOfDeviceDutyCycles);
      ezspDutyCycleHandler(channelPage, channel, state, totalDevices, &arrayOfDeviceDutyCycles);
      break;
    }

    case EZSP_REMOTE_SET_BINDING_HANDLER: {
      EmberBindingTableEntry entry;
      uint8_t index;
      EmberStatus policyDecision;
      fetchEmberBindingTableEntry(&entry);
      index = fetchInt8u();
      policyDecision = fetchInt8u();
      ezspRemoteSetBindingHandler(&entry, index, policyDecision);
      break;
    }

    case EZSP_REMOTE_DELETE_BINDING_HANDLER: {
      uint8_t index;
      EmberStatus policyDecision;
      index = fetchInt8u();
      policyDecision = fetchInt8u();
      ezspRemoteDeleteBindingHandler(index, policyDecision);
      break;
    }

    case EZSP_MESSAGE_SENT_HANDLER: {
      EmberOutgoingMessageType type;
      uint16_t indexOrDestination;
      EmberApsFrame apsFrame;
      uint8_t messageTag;
      EmberStatus status;
      uint8_t messageLength;
      uint8_t *messageContents;
      type = fetchInt8u();
      indexOrDestination = fetchInt16u();
      fetchEmberApsFrame(&apsFrame);
      messageTag = fetchInt8u();
      status = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      ezspMessageSentHandler(type, indexOrDestination, &apsFrame, messageTag, status, messageLength, messageContents);
      break;
    }

    case EZSP_POLL_COMPLETE_HANDLER: {
      EmberStatus status;
      status = fetchInt8u();
      ezspPollCompleteHandler(status);
      break;
    }

    case EZSP_POLL_HANDLER: {
      EmberNodeId childId;
      childId = fetchInt16u();
      ezspPollHandler(childId);
      break;
    }

    case EZSP_INCOMING_SENDER_EUI64_HANDLER: {
      uint8_t senderEui64[8];
      fetchInt8uArray(8, senderEui64);
      ezspIncomingSenderEui64Handler(senderEui64);
      break;
    }

    case EZSP_INCOMING_MESSAGE_HANDLER: {
      EmberIncomingMessageType type;
      EmberApsFrame apsFrame;
      uint8_t lastHopLqi;
      int8_t lastHopRssi;
      EmberNodeId sender;
      uint8_t bindingIndex;
      uint8_t addressIndex;
      uint8_t messageLength;
      uint8_t *messageContents;
      type = fetchInt8u();
      fetchEmberApsFrame(&apsFrame);
      lastHopLqi = fetchInt8u();
      lastHopRssi = fetchInt8u();
      sender = fetchInt16u();
      bindingIndex = fetchInt8u();
      addressIndex = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      ezspIncomingMessageHandler(type, &apsFrame, lastHopLqi, lastHopRssi, sender, bindingIndex, addressIndex, messageLength, messageContents);
      break;
    }

    case EZSP_INCOMING_MANY_TO_ONE_ROUTE_REQUEST_HANDLER: {
      EmberNodeId source;
      uint8_t longId[8];
      uint8_t cost;
      source = fetchInt16u();
      fetchInt8uArray(8, longId);
      cost = fetchInt8u();
      ezspIncomingManyToOneRouteRequestHandler(source, longId, cost);
      break;
    }

    case EZSP_INCOMING_ROUTE_ERROR_HANDLER: {
      EmberStatus status;
      EmberNodeId target;
      status = fetchInt8u();
      target = fetchInt16u();
      ezspIncomingRouteErrorHandler(status, target);
      break;
    }

    case EZSP_INCOMING_NETWORK_STATUS_HANDLER: {
      uint8_t errorCode;
      EmberNodeId target;
      errorCode = fetchInt8u();
      target = fetchInt16u();
      ezspIncomingNetworkStatusHandler(errorCode, target);
      break;
    }

    case EZSP_INCOMING_ROUTE_RECORD_HANDLER: {
      EmberNodeId source;
      uint8_t sourceEui[8];
      uint8_t lastHopLqi;
      int8_t lastHopRssi;
      uint8_t relayCount;
      uint8_t *relayList;
      source = fetchInt16u();
      fetchInt8uArray(8, sourceEui);
      lastHopLqi = fetchInt8u();
      lastHopRssi = fetchInt8u();
      relayCount = fetchInt8u();
      relayList = (uint8_t *)fetchInt8uPointer(relayCount * 2);
      ezspIncomingRouteRecordHandler(source, sourceEui, lastHopLqi, lastHopRssi, relayCount, relayList);
      break;
    }

    case EZSP_ID_CONFLICT_HANDLER: {
      EmberNodeId id;
      id = fetchInt16u();
      ezspIdConflictHandler(id);
      break;
    }

    case EZSP_MAC_PASSTHROUGH_MESSAGE_HANDLER: {
      EmberMacPassthroughType messageType;
      uint8_t lastHopLqi;
      int8_t lastHopRssi;
      uint8_t messageLength;
      uint8_t *messageContents;
      messageType = fetchInt8u();
      lastHopLqi = fetchInt8u();
      lastHopRssi = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      ezspMacPassthroughMessageHandler(messageType, lastHopLqi, lastHopRssi, messageLength, messageContents);
      break;
    }

    case EZSP_MAC_FILTER_MATCH_MESSAGE_HANDLER: {
      uint8_t filterIndexMatch;
      EmberMacPassthroughType legacyPassthroughType;
      uint8_t lastHopLqi;
      int8_t lastHopRssi;
      uint8_t messageLength;
      uint8_t *messageContents;
      filterIndexMatch = fetchInt8u();
      legacyPassthroughType = fetchInt8u();
      lastHopLqi = fetchInt8u();
      lastHopRssi = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      ezspMacFilterMatchMessageHandler(filterIndexMatch, legacyPassthroughType, lastHopLqi, lastHopRssi, messageLength, messageContents);
      break;
    }

    case EZSP_RAW_TRANSMIT_COMPLETE_HANDLER: {
      EmberStatus status;
      status = fetchInt8u();
      ezspRawTransmitCompleteHandler(status);
      break;
    }

    case EZSP_SWITCH_NETWORK_KEY_HANDLER: {
      uint8_t sequenceNumber;
      sequenceNumber = fetchInt8u();
      ezspSwitchNetworkKeyHandler(sequenceNumber);
      break;
    }

    case EZSP_ZIGBEE_KEY_ESTABLISHMENT_HANDLER: {
      uint8_t partner[8];
      EmberKeyStatus status;
      fetchInt8uArray(8, partner);
      status = fetchInt8u();
      ezspZigbeeKeyEstablishmentHandler(partner, status);
      break;
    }

    case EZSP_TRUST_CENTER_JOIN_HANDLER: {
      EmberNodeId newNodeId;
      uint8_t newNodeEui64[8];
      EmberDeviceUpdate status;
      EmberJoinDecision policyDecision;
      EmberNodeId parentOfNewNodeId;
      newNodeId = fetchInt16u();
      fetchInt8uArray(8, newNodeEui64);
      status = fetchInt8u();
      policyDecision = fetchInt8u();
      parentOfNewNodeId = fetchInt16u();
      ezspTrustCenterJoinHandler(newNodeId, newNodeEui64, status, policyDecision, parentOfNewNodeId);
      break;
    }

    case EZSP_GENERATE_CBKE_KEYS_HANDLER: {
      EmberStatus status;
      EmberPublicKeyData ephemeralPublicKey;
      status = fetchInt8u();
      fetchEmberPublicKeyData(&ephemeralPublicKey);
      ezspGenerateCbkeKeysHandler(status, &ephemeralPublicKey);
      break;
    }

    case EZSP_CALCULATE_SMACS_HANDLER: {
      EmberStatus status;
      EmberSmacData initiatorSmac;
      EmberSmacData responderSmac;
      status = fetchInt8u();
      fetchEmberSmacData(&initiatorSmac);
      fetchEmberSmacData(&responderSmac);
      ezspCalculateSmacsHandler(status, &initiatorSmac, &responderSmac);
      break;
    }

    case EZSP_GENERATE_CBKE_KEYS_HANDLER283K1: {
      EmberStatus status;
      EmberPublicKey283k1Data ephemeralPublicKey;
      status = fetchInt8u();
      fetchEmberPublicKey283k1Data(&ephemeralPublicKey);
      ezspGenerateCbkeKeysHandler283k1(status, &ephemeralPublicKey);
      break;
    }

    case EZSP_CALCULATE_SMACS_HANDLER283K1: {
      EmberStatus status;
      EmberSmacData initiatorSmac;
      EmberSmacData responderSmac;
      status = fetchInt8u();
      fetchEmberSmacData(&initiatorSmac);
      fetchEmberSmacData(&responderSmac);
      ezspCalculateSmacsHandler283k1(status, &initiatorSmac, &responderSmac);
      break;
    }

    case EZSP_DSA_SIGN_HANDLER: {
      EmberStatus status;
      uint8_t messageLength;
      uint8_t *messageContents;
      status = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      ezspDsaSignHandler(status, messageLength, messageContents);
      break;
    }

    case EZSP_DSA_VERIFY_HANDLER: {
      EmberStatus status;
      status = fetchInt8u();
      ezspDsaVerifyHandler(status);
      break;
    }

    case EZSP_MFGLIB_RX_HANDLER: {
      uint8_t linkQuality;
      int8_t rssi;
      uint8_t packetLength;
      uint8_t *packetContents;
      linkQuality = fetchInt8u();
      rssi = fetchInt8u();
      packetLength = fetchInt8u();
      packetContents = (uint8_t *)fetchInt8uPointer(packetLength);
      ezspMfglibRxHandler(linkQuality, rssi, packetLength, packetContents);
      break;
    }

    case EZSP_INCOMING_BOOTLOAD_MESSAGE_HANDLER: {
      uint8_t longId[8];
      uint8_t lastHopLqi;
      int8_t lastHopRssi;
      uint8_t messageLength;
      uint8_t *messageContents;
      fetchInt8uArray(8, longId);
      lastHopLqi = fetchInt8u();
      lastHopRssi = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      ezspIncomingBootloadMessageHandler(longId, lastHopLqi, lastHopRssi, messageLength, messageContents);
      break;
    }

    case EZSP_BOOTLOAD_TRANSMIT_COMPLETE_HANDLER: {
      EmberStatus status;
      uint8_t messageLength;
      uint8_t *messageContents;
      status = fetchInt8u();
      messageLength = fetchInt8u();
      messageContents = (uint8_t *)fetchInt8uPointer(messageLength);
      ezspBootloadTransmitCompleteHandler(status, messageLength, messageContents);
      break;
    }

    case EZSP_ZLL_NETWORK_FOUND_HANDLER: {
      EmberZllNetwork networkInfo;
      bool isDeviceInfoNull;
      EmberZllDeviceInfoRecord deviceInfo;
      uint8_t lastHopLqi;
      int8_t lastHopRssi;
      fetchEmberZllNetwork(&networkInfo);
      isDeviceInfoNull = fetchInt8u();
      fetchEmberZllDeviceInfoRecord(&deviceInfo);
      lastHopLqi = fetchInt8u();
      lastHopRssi = fetchInt8u();
      ezspZllNetworkFoundHandler(&networkInfo, isDeviceInfoNull, &deviceInfo, lastHopLqi, lastHopRssi);
      break;
    }

    case EZSP_ZLL_SCAN_COMPLETE_HANDLER: {
      EmberStatus status;
      status = fetchInt8u();
      ezspZllScanCompleteHandler(status);
      break;
    }

    case EZSP_ZLL_ADDRESS_ASSIGNMENT_HANDLER: {
      EmberZllAddressAssignment addressInfo;
      uint8_t lastHopLqi;
      int8_t lastHopRssi;
      fetchEmberZllAddressAssignment(&addressInfo);
      lastHopLqi = fetchInt8u();
      lastHopRssi = fetchInt8u();
      ezspZllAddressAssignmentHandler(&addressInfo, lastHopLqi, lastHopRssi);
      break;
    }

    case EZSP_ZLL_TOUCH_LINK_TARGET_HANDLER: {
      EmberZllNetwork networkInfo;
      fetchEmberZllNetwork(&networkInfo);
      ezspZllTouchLinkTargetHandler(&networkInfo);
      break;
    }

    case EZSP_D_GP_SENT_HANDLER: {
      EmberStatus status;
      uint8_t gpepHandle;
      status = fetchInt8u();
      gpepHandle = fetchInt8u();
      ezspDGpSentHandler(status, gpepHandle);
      break;
    }

    case EZSP_GPEP_INCOMING_MESSAGE_HANDLER: {
      EmberStatus status;
      uint8_t gpdLink;
      uint8_t sequenceNumber;
      EmberGpAddress addr;
      EmberGpSecurityLevel gpdfSecurityLevel;
      EmberGpKeyType gpdfSecurityKeyType;
      bool autoCommissioning;
      uint8_t bidirectionalInfo;
      uint32_t gpdSecurityFrameCounter;
      uint8_t gpdCommandId;
      uint32_t mic;
      uint8_t proxyTableIndex;
      uint8_t gpdCommandPayloadLength;
      uint8_t *gpdCommandPayload;
      status = fetchInt8u();
      gpdLink = fetchInt8u();
      sequenceNumber = fetchInt8u();
      fetchEmberGpAddress(&addr);
      gpdfSecurityLevel = fetchInt8u();
      gpdfSecurityKeyType = fetchInt8u();
      autoCommissioning = fetchInt8u();
      bidirectionalInfo = fetchInt8u();
      gpdSecurityFrameCounter = fetchInt32u();
      gpdCommandId = fetchInt8u();
      mic = fetchInt32u();
      proxyTableIndex = fetchInt8u();
      gpdCommandPayloadLength = fetchInt8u();
      gpdCommandPayload = (uint8_t *)fetchInt8uPointer(gpdCommandPayloadLength);
      ezspGpepIncomingMessageHandler(status, gpdLink, sequenceNumber, &addr, gpdfSecurityLevel, gpdfSecurityKeyType, autoCommissioning, bidirectionalInfo, gpdSecurityFrameCounter, gpdCommandId, mic, proxyTableIndex, gpdCommandPayloadLength, gpdCommandPayload);
      break;
    }

    default:
      ezspErrorHandler(EZSP_ERROR_INVALID_FRAME_ID);
  }
}
