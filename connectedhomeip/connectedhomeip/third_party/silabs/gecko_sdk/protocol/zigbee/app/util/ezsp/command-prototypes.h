/*****************************************************************************/
/**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/
//
// *** Generated file. Do not edit! ***
//
// Description: Function prototypes for sending every EM260 frame and returning
// the result to the Host.

#include "stack/include/library.h"

//------------------------------------------------------------------------------
// Configuration Frames
//------------------------------------------------------------------------------

// The command allows the Host to specify the desired EZSP version and must be
// sent before any other command. This document describes EZSP version 9 and
// stack type 2 (mesh). The response provides information about the firmware
// running on the NCP.
// Return: The EZSP version the NCP is using (9).
uint8_t ezspVersion(
  // The EZSP version the Host wishes to use. To successfully set the
  // version and allow other commands, this must be 9.
  uint8_t desiredProtocolVersion,
  // Return: The type of stack running on the NCP (2).
  uint8_t *stackType,
  // Return: The version number of the stack.
  uint16_t *stackVersion);

// Reads a configuration value from the NCP.
// Return: EZSP_SUCCESS if the value was read successfully,
// EZSP_ERROR_INVALID_ID if the NCP does not recognize configId.
EzspStatus ezspGetConfigurationValue(
  // Identifies which configuration value to read.
  EzspConfigId configId,
  // Return: The configuration value.
  uint16_t *value);

// Writes a configuration value to the NCP. Configuration values can be modified
// by the Host after the NCP has reset. Once the status of the stack changes to
// EMBER_NETWORK_UP, configuration values can no longer be modified and this
// command will respond with EZSP_ERROR_INVALID_CALL.
// Return: EZSP_SUCCESS if the configuration value was changed,
// EZSP_ERROR_OUT_OF_MEMORY if the new value exceeded the available memory,
// EZSP_ERROR_INVALID_VALUE if the new value was out of bounds,
// EZSP_ERROR_INVALID_ID if the NCP does not recognize configId,
// EZSP_ERROR_INVALID_CALL if configuration values can no longer be modified.
EzspStatus ezspSetConfigurationValue(
  // Identifies which configuration value to change.
  EzspConfigId configId,
  // The new configuration value.
  uint16_t value);

// Configures endpoint information on the NCP. The NCP does not remember these
// settings after a reset. Endpoints can be added by the Host after the NCP has
// reset. Once the status of the stack changes to EMBER_NETWORK_UP, endpoints
// can no longer be added and this command will respond with
// EZSP_ERROR_INVALID_CALL.
// Return: EZSP_SUCCESS if the endpoint was added, EZSP_ERROR_OUT_OF_MEMORY if
// there is not enough memory available to add the endpoint,
// EZSP_ERROR_INVALID_VALUE if the endpoint already exists,
// EZSP_ERROR_INVALID_CALL if endpoints can no longer be added.
EzspStatus ezspAddEndpoint(
  // The application endpoint to be added.
  uint8_t endpoint,
  // The endpoint's application profile.
  uint16_t profileId,
  // The endpoint's device ID within the application profile.
  uint16_t deviceId,
  // The endpoint's device version.
  uint8_t deviceVersion,
  // The number of cluster IDs in inputClusterList.
  uint8_t inputClusterCount,
  // The number of cluster IDs in outputClusterList.
  uint8_t outputClusterCount,
  // Input cluster IDs the endpoint will accept.
  uint16_t *inputClusterList,
  // Output cluster IDs the endpoint may send.
  uint16_t *outputClusterList);

// Allows the Host to change the policies used by the NCP to make fast
// decisions.
// Return: EZSP_SUCCESS if the policy was changed, EZSP_ERROR_INVALID_ID if the
// NCP does not recognize policyId.
EzspStatus ezspSetPolicy(
  // Identifies which policy to modify.
  EzspPolicyId policyId,
  // The new decision for the specified policy.
  EzspDecisionId decisionId);

// Allows the Host to read the policies used by the NCP to make fast decisions.
// Return: EZSP_SUCCESS if the policy was read successfully,
// EZSP_ERROR_INVALID_ID if the NCP does not recognize policyId.
EzspStatus ezspGetPolicy(
  // Identifies which policy to read.
  EzspPolicyId policyId,
  // Return: The current decision for the specified policy.
  EzspDecisionId *decisionId);

// Triggers a pan id update message.
// Return: true if the request was successfully handed to the stack, false
// otherwise
bool ezspSendPanIdUpdate(
  // The new Pan Id
  EmberPanId newPan);

// Reads a value from the NCP.
// Return: EZSP_SUCCESS if the value was read successfully,
// EZSP_ERROR_INVALID_ID if the NCP does not recognize valueId,
// EZSP_ERROR_INVALID_VALUE if the length of the returned value exceeds the size
// of local storage allocated to receive it.
EzspStatus ezspGetValue(
  // Identifies which value to read.
  EzspValueId valueId,
  // Return: Both a command and response parameter. On command, the maximum
  // size in bytes of local storage allocated to receive the returned value.
  // On response, the actual length in bytes of the returned value.
  uint8_t *valueLength,
  // Return: The value.
  uint8_t *value);

// Reads a value from the NCP but passes an extra argument specific to the value
// being retrieved.
// Return: EZSP_SUCCESS if the value was read successfully,
// EZSP_ERROR_INVALID_ID if the NCP does not recognize valueId,
// EZSP_ERROR_INVALID_VALUE if the length of the returned value exceeds the size
// of local storage allocated to receive it.
EzspStatus ezspGetExtendedValue(
  // Identifies which extended value ID to read.
  EzspExtendedValueId valueId,
  // Identifies which characteristics of the extended value ID to read.
  // These are specific to the value being read.
  uint32_t characteristics,
  // Return: Both a command and response parameter. On command, the maximum
  // size in bytes of local storage allocated to receive the returned value.
  // On response, the actual length in bytes of the returned value.
  uint8_t *valueLength,
  // Return: The value.
  uint8_t *value);

// Writes a value to the NCP.
// Return: EZSP_SUCCESS if the value was changed, EZSP_ERROR_INVALID_VALUE if
// the new value was out of bounds, EZSP_ERROR_INVALID_ID if the NCP does not
// recognize valueId, EZSP_ERROR_INVALID_CALL if the value could not be
// modified.
EzspStatus ezspSetValue(
  // Identifies which value to change.
  EzspValueId valueId,
  // The length of the value parameter in bytes.
  uint8_t valueLength,
  // The new value.
  uint8_t *value);

//------------------------------------------------------------------------------
// Utilities Frames
//------------------------------------------------------------------------------

// A command which does nothing. The Host can use this to set the sleep mode or
// to check the status of the NCP.
void ezspNop(void);

// Variable length data from the Host is echoed back by the NCP. This command
// has no other effects and is designed for testing the link between the Host
// and NCP.
// Return: The length of the echo parameter in bytes.
uint8_t ezspEcho(
  // The length of the data parameter in bytes.
  uint8_t dataLength,
  // The data to be echoed back.
  uint8_t *data,
  // Return: The echo of the data.
  uint8_t *echo);

// Allows the NCP to respond with a pending callback.
void ezspCallback(void);

// Callback
// Indicates that there are currently no pending callbacks.
void ezspNoCallbacks(void);

// Sets a token (8 bytes of non-volatile storage) in the Simulated EEPROM of the
// NCP.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSetToken(
  // Which token to set
  uint8_t tokenId,
  // The data to write to the token.
  uint8_t *tokenData);

// Retrieves a token (8 bytes of non-volatile storage) from the Simulated EEPROM
// of the NCP.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspGetToken(
  // Which token to read
  uint8_t tokenId,
  // Return: The contents of the token.
  uint8_t *tokenData);

// Retrieves a manufacturing token from the Flash Information Area of the NCP
// (except for EZSP_STACK_CAL_DATA which is managed by the stack).
// Return: The length of the tokenData parameter in bytes.
uint8_t ezspGetMfgToken(
  // Which manufacturing token to read.
  EzspMfgTokenId tokenId,
  // Return: The manufacturing token data.
  uint8_t *tokenData);

// Sets a manufacturing token in the Customer Information Block (CIB) area of
// the NCP if that token currently unset (fully erased). Cannot be used with
// EZSP_STACK_CAL_DATA, EZSP_STACK_CAL_FILTER, EZSP_MFG_ASH_CONFIG, or
// EZSP_MFG_CBKE_DATA token.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSetMfgToken(
  // Which manufacturing token to set.
  EzspMfgTokenId tokenId,
  // The length of the tokenData parameter in bytes.
  uint8_t tokenDataLength,
  // The manufacturing token data.
  uint8_t *tokenData);

// Callback
// A callback invoked to inform the application that a stack token has changed.
void ezspStackTokenChangedHandler(
  // The address of the stack token that has changed.
  uint16_t tokenAddress);

// Returns a pseudorandom number.
// Return: Always returns EMBER_SUCCESS.
EmberStatus ezspGetRandomNumber(
  // Return: A pseudorandom number.
  uint16_t *value);

// Sets a timer on the NCP. There are 2 independent timers available for use by
// the Host. A timer can be cancelled by setting time to 0 or units to
// EMBER_EVENT_INACTIVE.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSetTimer(
  // Which timer to set (0 or 1).
  uint8_t timerId,
  // The delay before the timerHandler callback will be generated. Note that
  // the timer clock is free running and is not synchronized with this
  // command. This means that the actual delay will be between time and
  // (time - 1). The maximum delay is 32767.
  uint16_t time,
  // The units for time.
  EmberEventUnits units,
  // If true, a timerHandler callback will be generated repeatedly. If
  // false, only a single timerHandler callback will be generated.
  bool repeat);

// Gets information about a timer. The Host can use this command to find out how
// much longer it will be before a previously set timer will generate a
// callback.
// Return: The delay before the timerHandler callback will be generated.
uint16_t ezspGetTimer(
  // Which timer to get information about (0 or 1).
  uint8_t timerId,
  // Return: The units for time.
  EmberEventUnits *units,
  // Return: True if a timerHandler callback will be generated repeatedly.
  // False if only a single timerHandler callback will be generated.
  bool *repeat);

// Callback
// A callback from the timer.
void ezspTimerHandler(
  // Which timer generated the callback (0 or 1).
  uint8_t timerId);

// Sends a debug message from the Host to the Network Analyzer utility via the
// NCP.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspDebugWrite(
  // true if the message should be interpreted as binary data, false if the
  // message should be interpreted as ASCII text.
  bool binaryMessage,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The binary message.
  uint8_t *messageContents);

// Retrieves and clears Ember counters. See the EmberCounterType enumeration for
// the counter types.
void ezspReadAndClearCounters(
  // Return: A list of all counter values ordered according to the
  // EmberCounterType enumeration.
  uint16_t *values);

// Retrieves Ember counters. See the EmberCounterType enumeration for the
// counter types.
void ezspReadCounters(
  // Return: A list of all counter values ordered according to the
  // EmberCounterType enumeration.
  uint16_t *values);

// Callback
// This call is fired when a counter exceeds its threshold
void ezspCounterRolloverHandler(
  // Type of Counter
  EmberCounterType type);

// Used to test that UART flow control is working correctly.
void ezspDelayTest(
  // Data will not be read from the host for this many milliseconds.
  uint16_t delay);

// This retrieves the status of the passed library ID to determine if it is
// compiled into the stack.
// Return: The status of the library being queried.
EmberLibraryStatus ezspGetLibraryStatus(
  // The ID of the library being queried.
  EmberLibraryId libraryId);

// Allows the HOST to know whether the NCP is running the XNCP library. If so,
// the response contains also the manufacturer ID and the version number of the
// XNCP application that is running on the NCP.
// Return: EMBER_SUCCESS if the NCP is running the XNCP library.
// EMBER_INVALID_CALL otherwise.
EmberStatus ezspGetXncpInfo(
  // Return: The manufactured ID the user has defined in the XNCP
  // application.
  uint16_t *manufacturerId,
  // Return: The version number of the XNCP application.
  uint16_t *versionNumber);

// Provides the customer a custom EZSP frame. On the NCP, these frames are only
// handled if the XNCP library is included. On the NCP side these frames are
// handled in the emberXNcpIncomingCustomEzspMessageCallback() callback
// function.
// Return: The status returned by the custom command.
EmberStatus ezspCustomFrame(
  // The length of the custom frame payload (maximum 119 bytes).
  uint8_t payloadLength,
  // The payload of the custom frame.
  uint8_t *payload,
  // Return: The length of the response.
  uint8_t *replyLength,
  // Return: The response.
  uint8_t *reply);

// Callback
// A callback indicating a custom EZSP message has been received.
void ezspCustomFrameHandler(
  // The length of the custom frame payload.
  uint8_t payloadLength,
  // The payload of the custom frame.
  uint8_t *payload);

// Returns the EUI64 ID of the local node.
void ezspGetEui64(
  // Return: The 64-bit ID.
  EmberEUI64 eui64);

// Returns the 16-bit node ID of the local node.
// Return: The 16-bit ID.
EmberNodeId ezspGetNodeId(void);

// Returns number of phy interfaces present.
// Return: Value indicate how many phy interfaces present.
uint8_t ezspGetPhyInterfaceCount(void);

// Returns the entropy source used for true random number generation.
// Return: Value indicates the used entropy source.
EmberEntropySource ezspGetTrueRandomEntropySource(void);

//------------------------------------------------------------------------------
// Networking Frames
//------------------------------------------------------------------------------

// Sets the manufacturer code to the specified value. The manufacturer code is
// one of the fields of the node descriptor.
void ezspSetManufacturerCode(
  // The manufacturer code for the local node.
  uint16_t code);

// Sets the power descriptor to the specified value. The power descriptor is a
// dynamic value. Therefore, you should call this function whenever the value
// changes.
void ezspSetPowerDescriptor(
  // The new power descriptor for the local node.
  uint16_t descriptor);

// Resume network operation after a reboot. The node retains its original type.
// This should be called on startup whether or not the node was previously part
// of a network. EMBER_NOT_JOINED is returned if the node is not part of a
// network. This command accepts options to control the network initialization.
// Return: An EmberStatus value that indicates one of the following: successful
// initialization, EMBER_NOT_JOINED if the node is not part of a network, or the
// reason for failure.
EmberStatus ezspNetworkInit(
  // An EmberNetworkInitStruct containing the options for initialization.
  EmberNetworkInitStruct *networkInitStruct);

// Returns a value indicating whether the node is joining, joined to, or leaving
// a network.
// Return: An EmberNetworkStatus value indicating the current join status.
EmberNetworkStatus ezspNetworkState(void);

// Callback
// A callback invoked when the status of the stack changes. If the status
// parameter equals EMBER_NETWORK_UP, then the getNetworkParameters command can
// be called to obtain the new network parameters. If any of the parameters are
// being stored in nonvolatile memory by the Host, the stored values should be
// updated.
void ezspStackStatusHandler(
  // Stack status
  EmberStatus status);

// This function will start a scan.
// Return: SL_STATUS_OK signals that the scan successfully started. Possible
// error responses and their meanings: SL_STATUS_MAC_SCANNING, we are already
// scanning; SL_STATUS_BAD_SCAN_DURATION, we have set a duration value that is
// not 0..14 inclusive; SL_STATUS_MAC_INCORRECT_SCAN_TYPE, we have requested an
// undefined scanning type; SL_STATUS_INVALID_CHANNEL_MASK, our channel mask did
// not specify any valid channels.
sl_status_t ezspStartScan(
  // Indicates the type of scan to be performed. Possible values are:
  // EZSP_ENERGY_SCAN and EZSP_ACTIVE_SCAN. For each type, the respective
  // callback for reporting results is: energyScanResultHandler and
  // networkFoundHandler. The energy scan and active scan report errors and
  // completion via the scanCompleteHandler.
  EzspNetworkScanType scanType,
  // Bits set as 1 indicate that this particular channel should be scanned.
  // Bits set to 0 indicate that this particular channel should not be
  // scanned. For example, a channelMask value of 0x00000001 would indicate
  // that only channel 0 should be scanned. Valid channels range from 11 to
  // 26 inclusive. This translates to a channel mask value of 0x07FFF800. As
  // a convenience, a value of 0 is reinterpreted as the mask for the
  // current channel.
  uint32_t channelMask,
  // Sets the exponent of the number of scan periods, where a scan period is
  // 960 symbols. The scan will occur for ((2^duration) + 1) scan periods.
  uint8_t duration);

// Callback
// Reports the result of an energy scan for a single channel. The scan is not
// complete until the scanCompleteHandler callback is called.
void ezspEnergyScanResultHandler(
  // The 802.15.4 channel number that was scanned.
  uint8_t channel,
  // The maximum RSSI value found on the channel.
  int8_t maxRssiValue);

// Callback
// Reports that a network was found as a result of a prior call to startScan.
// Gives the network parameters useful for deciding which network to join.
void ezspNetworkFoundHandler(
  // The parameters associated with the network found.
  EmberZigbeeNetwork *networkFound,
  // The link quality from the node that generated this beacon.
  uint8_t lastHopLqi,
  // The energy level (in units of dBm) observed during the reception.
  int8_t lastHopRssi);

// Callback
// Returns the status of the current scan of type EZSP_ENERGY_SCAN or
// EZSP_ACTIVE_SCAN. EMBER_SUCCESS signals that the scan has completed. Other
// error conditions signify a failure to scan on the channel specified.
void ezspScanCompleteHandler(
  // The channel on which the current error occurred. Undefined for the case
  // of EMBER_SUCCESS.
  uint8_t channel,
  // The error condition that occurred on the current channel. Value will be
  // EMBER_SUCCESS when the scan has completed.
  EmberStatus status);

// Callback
// This function returns an unused panID and channel pair found via the find
// unused panId scan procedure.
void ezspUnusedPanIdFoundHandler(
  // The unused panID which has been found.
  EmberPanId panId,
  // The channel that the unused panID was found on.
  uint8_t channel);

// This function starts a series of scans which will return an available panId.
// Return: The error condition that occurred during the scan. Value will be
// EMBER_SUCCESS if there are no errors.
EmberStatus ezspFindUnusedPanId(
  // The channels that will be scanned for available panIds.
  uint32_t channelMask,
  // The duration of the procedure.
  uint8_t duration);

// Terminates a scan in progress.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspStopScan(void);

// Forms a new network by becoming the coordinator.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspFormNetwork(
  // Specification of the new network.
  EmberNetworkParameters *parameters);

// Causes the stack to associate with the network using the specified network
// parameters. It can take several seconds for the stack to associate with the
// local network. Do not send messages until the stackStatusHandler callback
// informs you that the stack is up.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspJoinNetwork(
  // Specification of the role that this node will have in the network. This
  // role must not be EMBER_COORDINATOR. To be a coordinator, use the
  // formNetwork command.
  EmberNodeType nodeType,
  // Specification of the network with which the node should associate.
  EmberNetworkParameters *parameters);

// Causes the stack to associate with the network using the specified network
// parameters in the beacon parameter. It can take several seconds for the stack
// to associate with the local network. Do not send messages until the
// stackStatusHandler callback informs you that the stack is up. Unlike
// ::emberJoinNetwork(), this function does not issue an active scan before
// joining. Instead, it will cause the local node to issue a MAC Association
// Request directly to the specified target node. It is assumed that the beacon
// parameter is an artifact after issuing an active scan. (For more information,
// see emberGetBestBeacon and emberGetNextBeacon.)
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspJoinNetworkDirectly(
  // Specifies the role that this node will have in the network. This role
  // must not be EMBER_COORDINATOR. To be a coordinator, use the formNetwork
  // command.
  EmberNodeType localNodeType,
  // Specifies the network with which the node should associate.
  EmberBeaconData *beacon,
  // The radio transmit power to use, specified in dBm.
  int8_t radioTxPower,
  // If true, clear beacons in cache upon join success. If join fail, do
  // nothing.
  bool clearBeaconsAfterNetworkUp);

// Causes the stack to leave the current network. This generates a
// stackStatusHandler callback to indicate that the network is down. The radio
// will not be used until after sending a formNetwork or joinNetwork command.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspLeaveNetwork(void);

// The application may call this function when contact with the network has been
// lost. The most common usage case is when an end device can no longer
// communicate with its parent and wishes to find a new one. Another case is
// when a device has missed a Network Key update and no longer has the current
// Network Key.  The stack will call ezspStackStatusHandler to indicate that the
// network is down, then try to re-establish contact with the network by
// performing an active scan, choosing a network with matching extended pan id,
// and sending a ZigBee network rejoin request. A second call to the
// ezspStackStatusHandler callback indicates either the success or the failure
// of the attempt. The process takes approximately 150 milliseconds per channel
// to complete.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspFindAndRejoinNetwork(
  // This parameter tells the stack whether to try to use the current
  // network key. If it has the current network key it will perform a secure
  // rejoin (encrypted). If this fails the device should try an unsecure
  // rejoin. If the Trust Center allows the rejoin then the current Network
  // Key will be sent encrypted using the device's Link Key.
  bool haveCurrentNetworkKey,
  // A mask indicating the channels to be scanned. See emberStartScan for
  // format details. A value of 0 is reinterpreted as the mask for the
  // current channel.
  uint32_t channelMask);

// Tells the stack to allow other nodes to join the network with this node as
// their parent. Joining is initially disabled by default.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspPermitJoining(
  // A value of 0x00 disables joining. A value of 0xFF enables joining. Any
  // other value enables joining for that number of seconds.
  uint8_t duration);

// Callback
// Indicates that a child has joined or left.
void ezspChildJoinHandler(
  // The index of the child of interest.
  uint8_t index,
  // True if the child is joining. False the child is leaving.
  bool joining,
  // The node ID of the child.
  EmberNodeId childId,
  // The EUI64 of the child.
  EmberEUI64 childEui64,
  // The node type of the child.
  EmberNodeType childType);

// Sends a ZDO energy scan request. This request may only be sent by the current
// network manager and must be unicast, not broadcast. See ezsp-utils.h for
// related macros emberSetNetworkManagerRequest() and
// emberChangeChannelRequest().
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspEnergyScanRequest(
  // The network address of the node to perform the scan.
  EmberNodeId target,
  // A mask of the channels to be scanned
  uint32_t scanChannels,
  // How long to scan on each channel. Allowed values are 0..5, with the
  // scan times as specified by 802.15.4 (0 = 31ms, 1 = 46ms, 2 = 77ms, 3 =
  // 138ms, 4 = 261ms, 5 = 507ms).
  uint8_t scanDuration,
  // The number of scans to be performed on each channel (1..8).
  uint16_t scanCount);

// Returns the current network parameters.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspGetNetworkParameters(
  // Return: An EmberNodeType value indicating the current node type.
  EmberNodeType *nodeType,
  // Return: The current network parameters.
  EmberNetworkParameters *parameters);

// Returns the current radio parameters based on phy index.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspGetRadioParameters(
  // Desired index of phy interface for radio parameters.
  uint8_t phyIndex,
  // Return: The current radio parameters based on provided phy index.
  EmberMultiPhyRadioParameters *parameters);

// Returns information about the children of the local node and the parent of
// the local node.
// Return: The number of children the node currently has.
uint8_t ezspGetParentChildParameters(
  // Return: The parent's EUI64. The value is undefined for nodes without
  // parents (coordinators and nodes that are not joined to a network).
  EmberEUI64 parentEui64,
  // Return: The parent's node ID. The value is undefined for nodes without
  // parents (coordinators and nodes that are not joined to a network).
  EmberNodeId *parentNodeId);

// Returns information about a child of the local node.
// Return: EMBER_SUCCESS if there is a child at index. EMBER_NOT_JOINED if there
// is no child at index.
EmberStatus ezspGetChildData(
  // The index of the child of interest in the child table. Possible indexes
  // range from zero to EMBER_CHILD_TABLE_SIZE.
  uint8_t index,
  // Return: The data of the child.
  EmberChildData *childData);

// Sets child data to the child table token.
// Return: EMBER_SUCCESS if the child data is set successfully at index.
// EMBER_INDEX_OUT_OF_RANGE if provided index is out of range.
EmberStatus ezspSetChildData(
  // The index of the child of interest in the child table. Possible indexes
  // range from zero to (EMBER_CHILD_TABLE_SIZE - 1).
  uint8_t index,
  // The data of the child.
  EmberChildData *childData);

// Returns the source route table total size.
// Return: Total size of source route table.
uint8_t ezspGetSourceRouteTableTotalSize(void);

// Returns the number of filled entries in source route table.
// Return: The number of filled entries in source route table.
uint8_t ezspGetSourceRouteTableFilledSize(void);

// Returns information about a source route table entry
// Return: EMBER_SUCCESS if there is source route entry at
// index. EMBER_NOT_FOUND if there is no
// source route at index.
EmberStatus ezspGetSourceRouteTableEntry(
  // The index of the entry of interest in the
  // source route table. Possible indexes range from zero to
  // SOURCE_ROUTE_TABLE_FILLED_SIZE.
  uint8_t index,
  // Return: The node ID of the destination in that entry.
  EmberNodeId *destination,
  // Return: The closer node index for this source route table entry
  uint8_t *closerIndex);

// Returns the neighbor table entry at the given index. The number of active
// neighbors can be obtained using the neighborCount command.
// Return: EMBER_ERR_FATAL if the index is greater or equal to the number of
// active neighbors, or if the device is an end device. Returns EMBER_SUCCESS
// otherwise.
EmberStatus ezspGetNeighbor(
  // The index of the neighbor of interest. Neighbors are stored in
  // ascending order by node id, with all unused entries at the end of the
  // table.
  uint8_t index,
  // Return: The contents of the neighbor table entry.
  EmberNeighborTableEntry *value);

// Return EmberStatus depending on whether the frame counter of the node is
// found in the neighbor or child table. This function gets the last received
// frame counter as found in the Network Auxiliary header for the specified
// neighbor or child
// Return: Return EMBER_NOT_FOUND if the node is not found in the neighbor or
// child table. Returns EMBER_SUCCESS otherwise
EmberStatus ezspGetNeighborFrameCounter(
  // eui64 of the node
  EmberEUI64 eui64,
  // Return: Return the frame counter of the node from the neighbor or child
  // table
  uint32_t *returnFrameCounter);

// Sets the frame counter for the neighbour or child.
// Return: Return EMBER_NOT_FOUND if the node is not found in the neighbor or
// child table. Returns EMBER_SUCCESS otherwise
EmberStatus ezspSetNeighborFrameCounter(
  // eui64 of the node
  EmberEUI64 eui64,
  // Return the frame counter of the node from the neighbor or child table
  uint32_t frameCounter);

// Sets the routing shortcut threshold to directly use a neighbor instead of
// performing routing.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSetRoutingShortcutThreshold(
  // The routing shortcut threshold to configure.
  uint8_t costThresh);

// Gets the routing shortcut threshold used to differentiate between directly
// using a neighbor vs. performing routing.
// Return: The routing shortcut threshold
uint8_t ezspGetRoutingShortcutThreshold(void);

// Returns the number of active entries in the neighbor table.
// Return: The number of active entries in the neighbor table.
uint8_t ezspNeighborCount(void);

// Returns the route table entry at the given index. The route table size can be
// obtained using the getConfigurationValue command.
// Return: EMBER_ERR_FATAL if the index is out of range or the device is an end
// device, and EMBER_SUCCESS otherwise.
EmberStatus ezspGetRouteTableEntry(
  // The index of the route table entry of interest.
  uint8_t index,
  // Return: The contents of the route table entry.
  EmberRouteTableEntry *value);

// Sets the radio output power at which a node is operating. Ember radios have
// discrete power settings. For a list of available power settings, see the
// technical specification for the RF communication module in your Developer
// Kit. Note: Care should be taken when using this API on a running network, as
// it will directly impact the established link qualities neighboring nodes have
// with the node on which it is called. This can lead to disruption of existing
// routes and erratic network behavior.
// Return: An EmberStatus value indicating the success or failure of the
// command.
EmberStatus ezspSetRadioPower(
  // Desired radio output power, in dBm.
  int8_t power);

// Sets the channel to use for sending and receiving messages. For a list of
// available radio channels, see the technical specification for the RF
// communication module in your Developer Kit. Note: Care should be taken when
// using this API, as all devices on a network must use the same channel.
// Return: An EmberStatus value indicating the success or failure of the
// command.
EmberStatus ezspSetRadioChannel(
  // Desired radio channel.
  uint8_t channel);

// Gets the channel in use for sending and receiving messages.
// Return: Current radio channel.
uint8_t ezspGetRadioChannel(void);

// Set the configured 802.15.4 CCA mode in the radio.
// Return: An EmberStatus value indicating the success or failure of the
// command.
EmberStatus ezspSetRadioIeee802154CcaMode(
  // A RAIL_IEEE802154_CcaMode_t value.
  uint8_t ccaMode);

// Enable/disable concentrator support.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSetConcentrator(
  // If this bool is true the concentrator support is enabled. Otherwise is
  // disabled. If this bool is false all the other arguments are ignored.
  bool on,
  // Must be either EMBER_HIGH_RAM_CONCENTRATOR or
  // EMBER_LOW_RAM_CONCENTRATOR. The former is used when the caller has
  // enough memory to store source routes for the whole network. In that
  // case, remote nodes stop sending route records once the concentrator has
  // successfully received one. The latter is used when the concentrator has
  // insufficient RAM to store all outbound source routes. In that case,
  // route records are sent to the concentrator prior to every inbound APS
  // unicast.
  uint16_t concentratorType,
  // The minimum amount of time that must pass between MTORR broadcasts.
  uint16_t minTime,
  // The maximum amount of time that can pass between MTORR broadcasts.
  uint16_t maxTime,
  // The number of route errors that will trigger a re-broadcast of the
  // MTORR.
  uint8_t routeErrorThreshold,
  // The number of APS delivery failures that will trigger a re-broadcast of
  // the MTORR.
  uint8_t deliveryFailureThreshold,
  // The maximum number of hops that the MTORR broadcast will be allowed to
  // have. A value of 0 will be converted to the EMBER_MAX_HOPS value set by
  // the stack.
  uint8_t maxHops);

// Sets the error code that is sent back from a router with a broken route.
// Return: An EmberStatus value indicating the success or failure of the
// command.
EmberStatus ezspSetBrokenRouteErrorCode(
  // Desired error code.
  uint8_t errorCode);

// This causes to initialize the desired radio interface other than native and
// form a new network by becoming the coordinator with same panId as native
// radio network.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspMultiPhyStart(
  // Index of phy interface. The native phy index would be always zero hence
  // valid phy index starts from one.
  uint8_t phyIndex,
  // Desired radio channel page.
  uint8_t page,
  // Desired radio channel.
  uint8_t channel,
  // Desired radio output power, in dBm.
  int8_t power,
  // Network configuration bitmask.
  EmberMultiPhyNwkConfig bitmask);

// This causes to bring down the radio interface other than native.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspMultiPhyStop(
  // Index of phy interface. The native phy index would be always zero hence
  // valid phy index starts from one.
  uint8_t phyIndex);

// Sets the radio output power for desired phy interface at which a node is
// operating. Ember radios have discrete power settings. For a list of available
// power settings, see the technical specification for the RF communication
// module in your Developer Kit. Note: Care should be taken when using this api
// on a running network, as it will directly impact the established link
// qualities neighboring nodes have with the node on which it is called. This
// can lead to disruption of existing routes and erratic network behavior.
// Return: An EmberStatus value indicating the success or failure of the
// command.
EmberStatus ezspMultiPhySetRadioPower(
  // Index of phy interface. The native phy index would be always zero hence
  // valid phy index starts from one.
  uint8_t phyIndex,
  // Desired radio output power, in dBm.
  int8_t power);

// Send Link Power Delta Request from a child to its parent
// Return: An EmberStatus value indicating the success or failure of sending the
// request.
EmberStatus ezspSendLinkPowerDeltaRequest(void);

// Sets the channel for desired phy interface to use for sending and receiving
// messages. For a list of available radio pages and channels, see the technical
// specification for the RF communication module in your Developer Kit. Note:
// Care should be taken when using this API, as all devices on a network must
// use the same page and channel.
// Return: An EmberStatus value indicating the success or failure of the
// command.
EmberStatus ezspMultiPhySetRadioChannel(
  // Index of phy interface. The native phy index would be always zero hence
  // valid phy index starts from one.
  uint8_t phyIndex,
  // Desired radio channel page.
  uint8_t page,
  // Desired radio channel.
  uint8_t channel);

// Obtains the current duty cycle state.
// Return: An EmberStatus value indicating the success or failure of the
// command.
EmberStatus ezspGetDutyCycleState(
  // Return: The current duty cycle state in effect.
  EmberDutyCycleState *returnedState);

// Set the current duty cycle limits configuration. The Default limits set by
// stack if this call is not made.
// Return: EMBER_SUCCESS  if the duty cycle limit configurations set
// successfully, EMBER_BAD_ARGUMENT if set illegal value such as setting only
// one of the limits to default or violates constraints Susp > Crit > Limi,
// EMBER_INVALID_CALL if device is operating on 2.4Ghz
EmberStatus ezspSetDutyCycleLimitsInStack(
  // The duty cycle limits configuration to utilize.
  EmberDutyCycleLimits *limits);

// Obtains the current duty cycle limits that were previously set by a call to
// emberSetDutyCycleLimitsInStack(), or the defaults set by the stack if no set
// call was made.
// Return: An EmberStatus value indicating the success or failure of the
// command.
EmberStatus ezspGetDutyCycleLimits(
  // Return: Return current duty cycle limits if returnedLimits is not NULL
  EmberDutyCycleLimits *returnedLimits);

// Returns the duty cycle of the stack's connected children that are being
// monitored, up to maxDevices. It indicates the amount of overall duty cycle
// they have consumed (up to the suspend limit). The first entry is always the
// local stack's nodeId, and thus the total aggregate duty cycle for the device.
// The passed pointer arrayOfDeviceDutyCycles MUST have space for maxDevices.
// Return: EMBER_SUCCESS  if the duty cycles were read successfully,
// EMBER_BAD_ARGUMENT maxDevices is greater than EMBER_MAX_END_DEVICE_CHILDREN +
// 1.
EmberStatus ezspGetCurrentDutyCycle(
  // Number of devices to retrieve consumed duty cycle.
  uint8_t maxDevices,
  // Return: Consumed duty cycles up to maxDevices. When the number of
  // children that are being monitored is less than maxDevices, the
  // EmberNodeId element in the EmberPerDeviceDutyCycle will be 0xFFFF.
  uint8_t *arrayOfDeviceDutyCycles);

// Callback
// Callback fires when the duty cycle state has changed
void ezspDutyCycleHandler(
  // The channel page whose duty cycle state has changed.
  uint8_t channelPage,
  // The channel number whose duty cycle state has changed.
  uint8_t channel,
  // The current duty cycle state.
  EmberDutyCycleState state,
  // The total number of connected end devices that are being monitored for
  // duty cycle.
  uint8_t totalDevices,
  // Consumed duty cycles of end devices that are being monitored. The first
  // entry always be the local stack's nodeId, and thus the total aggregate
  // duty cycle for the device.
  EmberPerDeviceDutyCycle *arrayOfDeviceDutyCycles);

// Returns the first beacon in the cache. Beacons are stored in cache after
// issuing an active scan.
// Return: EMBER_SUCCESS if first beacon found, EMBER_BAD_ARGUMENT if input
// parameters are invalid, EMBER_INVALID_CALL if no beacons stored,
// EMBER_ERR_FATAL if no first beacon found.
EmberStatus ezspGetFirstBeacon(
  // Return: The iterator to use when returning the first beacon. This
  // argument must not be NULL.
  EmberBeaconIterator *beaconIterator);

// Returns the next beacon in the cache. Beacons are stored in cache after
// issuing an active scan.
// Return: EMBER_SUCCESS if next beacon found, EMBER_BAD_ARGUMENT if input
// parameters are invalid, EMBER_ERR_FATAL if no next beacon found.
EmberStatus ezspGetNextBeacon(
  // Return: The next beacon retrieved. It is assumed that
  // emberGetFirstBeacon has been called first. This argument must not be
  // NULL.
  EmberBeaconData *beacon);

// Returns the number of cached beacons that have been collected from a scan.
// Return: The number of cached beacons that have been collected from a scan.
uint8_t ezspGetNumStoredBeacons(void);

// Clears all cached beacons that have been collected from a scan.
void ezspClearStoredBeacons(void);

// This call sets the radio channel in the stack and propagates the information
// to the hardware.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSetLogicalAndRadioChannel(
  // The radio channel to be set.
  uint8_t radioChannel);

//------------------------------------------------------------------------------
// Binding Frames
//------------------------------------------------------------------------------

// Deletes all binding table entries.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspClearBindingTable(void);

// Sets an entry in the binding table.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSetBinding(
  // The index of a binding table entry.
  uint8_t index,
  // The contents of the binding entry.
  EmberBindingTableEntry *value);

// Gets an entry from the binding table.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspGetBinding(
  // The index of a binding table entry.
  uint8_t index,
  // Return: The contents of the binding entry.
  EmberBindingTableEntry *value);

// Deletes a binding table entry.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspDeleteBinding(
  // The index of a binding table entry.
  uint8_t index);

// Indicates whether any messages are currently being sent using this binding
// table entry. Note that this command does not indicate whether a binding is
// clear. To determine whether a binding is clear, check whether the type field
// of the EmberBindingTableEntry has the value EMBER_UNUSED_BINDING.
// Return: True if the binding table entry is active, false otherwise.
bool ezspBindingIsActive(
  // The index of a binding table entry.
  uint8_t index);

// Returns the node ID for the binding's destination, if the ID is known. If a
// message is sent using the binding and the destination's ID is not known, the
// stack will discover the ID by broadcasting a ZDO address request. The
// application can avoid the need for this discovery by using
// setBindingRemoteNodeId when it knows the correct ID via some other means. The
// destination's node ID is forgotten when the binding is changed, when the
// local node reboots or, much more rarely, when the destination node changes
// its ID in response to an ID conflict.
// Return: The short ID of the destination node or EMBER_NULL_NODE_ID if no
// destination is known.
EmberNodeId ezspGetBindingRemoteNodeId(
  // The index of a binding table entry.
  uint8_t index);

// Set the node ID for the binding's destination. See getBindingRemoteNodeId for
// a description.
void ezspSetBindingRemoteNodeId(
  // The index of a binding table entry.
  uint8_t index,
  // The short ID of the destination node.
  EmberNodeId nodeId);

// Callback
// The NCP used the external binding modification policy to decide how to handle
// a remote set binding request. The Host cannot change the current decision,
// but it can change the policy for future decisions using the setPolicy
// command.
void ezspRemoteSetBindingHandler(
  // The requested binding.
  EmberBindingTableEntry *entry,
  // The index at which the binding was added.
  uint8_t index,
  // EMBER_SUCCESS if the binding was added to the table and any other
  // status if not.
  EmberStatus policyDecision);

// Callback
// The NCP used the external binding modification policy to decide how to handle
// a remote delete binding request. The Host cannot change the current decision,
// but it can change the policy for future decisions using the setPolicy
// command.
void ezspRemoteDeleteBindingHandler(
  // The index of the binding whose deletion was requested.
  uint8_t index,
  // EMBER_SUCCESS if the binding was removed from the table and any other
  // status if not.
  EmberStatus policyDecision);

//------------------------------------------------------------------------------
// Messaging Frames
//------------------------------------------------------------------------------

// Returns the maximum size of the payload. The size depends on the security
// level in use.
// Return: The maximum APS payload length.
uint8_t ezspMaximumPayloadLength(void);

// Sends a unicast message as per the ZigBee specification. The message will
// arrive at its destination only if there is a known route to the destination
// node. Setting the ENABLE_ROUTE_DISCOVERY option will cause a route to be
// discovered if none is known. Setting the FORCE_ROUTE_DISCOVERY option will
// force route discovery. Routes to end-device children of the local node are
// always known. Setting the APS_RETRY option will cause the message to be
// retransmitted until either a matching acknowledgement is received or three
// transmissions have been made. Note: Using the FORCE_ROUTE_DISCOVERY option
// will cause the first transmission to be consumed by a route request as part
// of discovery, so the application payload of this packet will not reach its
// destination on the first attempt. If you want the packet to reach its
// destination, the APS_RETRY option must be set so that another attempt is made
// to transmit the message with its application payload after the route has been
// constructed. Note: When sending fragmented messages, the stack will only
// assign a new APS sequence number for the first fragment of the message (i.e.,
// EMBER_APS_OPTION_FRAGMENT is set and the low-order byte of the groupId field
// in the APS frame is zero). For all subsequent fragments of the same message,
// the application must set the sequence number field in the APS frame to the
// sequence number assigned by the stack to the first fragment.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSendUnicast(
  // Specifies the outgoing message type. Must be one of
  // EMBER_OUTGOING_DIRECT, EMBER_OUTGOING_VIA_ADDRESS_TABLE, or
  // EMBER_OUTGOING_VIA_BINDING.
  EmberOutgoingMessageType type,
  // Depending on the type of addressing used, this is either the
  // EmberNodeId of the destination, an index into the address table, or an
  // index into the binding table.
  EmberNodeId indexOrDestination,
  // The APS frame which is to be added to the message.
  EmberApsFrame *apsFrame,
  // A value chosen by the Host. This value is used in the
  // ezspMessageSentHandler response to refer to this message.
  uint8_t messageTag,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // Content of the message.
  uint8_t *messageContents,
  // Return: The sequence number that will be used when this message is
  // transmitted.
  uint8_t *sequence);

// Sends a broadcast message as per the ZigBee specification.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSendBroadcast(
  // The destination to which to send the broadcast. This must be one of the
  // three ZigBee broadcast addresses.
  EmberNodeId destination,
  // The APS frame for the message.
  EmberApsFrame *apsFrame,
  // The message will be delivered to all nodes within radius hops of the
  // sender. A radius of zero is converted to EMBER_MAX_HOPS.
  uint8_t radius,
  // A value chosen by the Host. This value is used in the
  // ezspMessageSentHandler response to refer to this message.
  uint8_t messageTag,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The broadcast message.
  uint8_t *messageContents,
  // Return: The sequence number that will be used when this message is
  // transmitted.
  uint8_t *sequence);

// Sends a proxied broadcast message as per the ZigBee specification.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspProxyBroadcast(
  // The source from which to send the broadcast.
  EmberNodeId source,
  // The destination to which to send the broadcast. This must be one of the
  // three ZigBee broadcast addresses.
  EmberNodeId destination,
  // The network sequence number for the broadcast.
  uint8_t nwkSequence,
  // The APS frame for the message.
  EmberApsFrame *apsFrame,
  // The message will be delivered to all nodes within radius hops of the
  // sender. A radius of zero is converted to EMBER_MAX_HOPS.
  uint8_t radius,
  // A value chosen by the Host. This value is used in the
  // ezspMessageSentHandler response to refer to this message.
  uint8_t messageTag,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The broadcast message.
  uint8_t *messageContents,
  // Return: The APS sequence number that will be used when this message is
  // transmitted.
  uint8_t *apsSequence);

// Sends a multicast message to all endpoints that share a specific multicast ID
// and are within a specified number of hops of the sender.
// Return: An EmberStatus value. For any result other than EMBER_SUCCESS, the
// message will not be sent. EMBER_SUCCESS - The message has been submitted for
// transmission. EMBER_INVALID_BINDING_INDEX - The bindingTableIndex refers to a
// non-multicast binding. EMBER_NETWORK_DOWN - The node is not part of a
// network. EMBER_MESSAGE_TOO_LONG - The message is too large to fit in a MAC
// layer frame. EMBER_NO_BUFFERS - The free packet buffer pool is empty.
// EMBER_NETWORK_BUSY - Insufficient resources available in Network or MAC
// layers to send message.
EmberStatus ezspSendMulticast(
  // The APS frame for the message. The multicast will be sent to the
  // groupId in this frame.
  EmberApsFrame *apsFrame,
  // The message will be delivered to all nodes within this number of hops
  // of the sender. A value of zero is converted to EMBER_MAX_HOPS.
  uint8_t hops,
  // The number of hops that the message will be forwarded by devices that
  // are not members of the group. A value of 7 or greater is treated as
  // infinite.
  uint8_t nonmemberRadius,
  // A value chosen by the Host. This value is used in the
  // ezspMessageSentHandler response to refer to this message.
  uint8_t messageTag,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The multicast message.
  uint8_t *messageContents,
  // Return: The sequence number that will be used when this message is
  // transmitted.
  uint8_t *sequence);

// Sends a multicast message to all endpoints that share a specific multicast ID
// and are within a specified number of hops of the sender.
// Return: An EmberStatus value. For any result other than EMBER_SUCCESS, the
// message will not be sent. EMBER_SUCCESS - The message has been submitted for
// transmission. EMBER_INVALID_BINDING_INDEX - The bindingTableIndex refers to a
// non-multicast binding. EMBER_NETWORK_DOWN - The node is not part of a
// network. EMBER_MESSAGE_TOO_LONG - The message is too large to fit in a MAC
// layer frame. EMBER_NO_BUFFERS - The free packet buffer pool is empty.
// EMBER_NETWORK_BUSY - Insufficient resources available in Network or MAC
// layers to send message.
EmberStatus ezspSendMulticastWithAlias(
  // The APS frame for the message. The multicast will be sent to the
  // groupId in this frame.
  EmberApsFrame *apsFrame,
  // The message will be delivered to all nodes within this number of hops
  // of the sender. A value of zero is converted to EMBER_MAX_HOPS.
  uint8_t hops,
  // The number of hops that the message will be forwarded by devices that
  // are not members of the group. A value of 7 or greater is treated as
  // infinite.
  uint8_t nonmemberRadius,
  // The alias source address
  uint16_t alias,
  // the alias sequence number
  uint8_t nwkSequence,
  // A value chosen by the Host. This value is used in the
  // ezspMessageSentHandler response to refer to this message.
  uint8_t messageTag,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The multicast message.
  uint8_t *messageContents,
  // Return: The sequence number that will be used when this message is
  // transmitted.
  uint8_t *sequence);

// Sends a reply to a received unicast message. The incomingMessageHandler
// callback for the unicast being replied to supplies the values for all the
// parameters except the reply itself.
// Return: An EmberStatus value. EMBER_INVALID_CALL - The
// EZSP_UNICAST_REPLIES_POLICY is set to EZSP_HOST_WILL_NOT_SUPPLY_REPLY. This
// means the NCP will automatically send an empty reply. The Host must change
// the policy to EZSP_HOST_WILL_SUPPLY_REPLY before it can supply the reply.
// There is one exception to this rule: In the case of responses to message
// fragments, the host must call sendReply when a message fragment is received.
// In this case, the policy set on the NCP does not matter. The NCP expects a
// sendReply call from the Host for message fragments regardless of the current
// policy settings. EMBER_NO_BUFFERS - Not enough memory was available to send
// the reply. EMBER_NETWORK_BUSY - Either no route or insufficient resources
// available. EMBER_SUCCESS - The reply was successfully queued for
// transmission.
EmberStatus ezspSendReply(
  // Value supplied by incoming unicast.
  EmberNodeId sender,
  // Value supplied by incoming unicast.
  EmberApsFrame *apsFrame,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The reply message.
  uint8_t *messageContents);

// Callback
// A callback indicating the stack has completed sending a message.
void ezspMessageSentHandler(
  // The type of message sent.
  EmberOutgoingMessageType type,
  // The destination to which the message was sent, for direct unicasts, or
  // the address table or binding index for other unicasts. The value is
  // unspecified for multicasts and broadcasts.
  uint16_t indexOrDestination,
  // The APS frame for the message.
  EmberApsFrame *apsFrame,
  // The value supplied by the Host in the ezspSendUnicast,
  // ezspSendBroadcast or ezspSendMulticast command.
  uint8_t messageTag,
  // An EmberStatus value of EMBER_SUCCESS if an ACK was received from the
  // destination or EMBER_DELIVERY_FAILED if no ACK was received.
  EmberStatus status,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The unicast message supplied by the Host. The message contents are only
  // included here if the decision for the messageContentsInCallback policy
  // is messageTagAndContentsInCallback.
  uint8_t *messageContents);

// Sends a route request packet that creates routes from every node in the
// network back to this node. This function should be called by an application
// that wishes to communicate with many nodes, for example, a gateway, central
// monitor, or controller. A device using this function was referred to as an
// 'aggregator' in EmberZNet 2.x and earlier, and is referred to as a
// 'concentrator' in the ZigBee specification and EmberZNet 3.  This function
// enables large scale networks, because the other devices do not have to
// individually perform bandwidth-intensive route discoveries. Instead, when a
// remote node sends an APS unicast to a concentrator, its network layer
// automatically delivers a special route record packet first, which lists the
// network ids of all the intermediate relays. The concentrator can then use
// source routing to send outbound APS unicasts. (A source routed message is one
// in which the entire route is listed in the network layer header.) This allows
// the concentrator to communicate with thousands of devices without requiring
// large route tables on neighboring nodes.  This function is only available in
// ZigBee Pro (stack profile 2), and cannot be called on end devices. Any router
// can be a concentrator (not just the coordinator), and there can be multiple
// concentrators on a network.  Note that a concentrator does not automatically
// obtain routes to all network nodes after calling this function. Remote
// applications must first initiate an inbound APS unicast.  Many-to-one routes
// are not repaired automatically. Instead, the concentrator application must
// call this function to rediscover the routes as necessary, for example, upon
// failure of a retried APS message. The reason for this is that there is no
// scalable one-size-fits-all route repair strategy. A common and recommended
// strategy is for the concentrator application to refresh the routes by calling
// this function periodically.
// Return: EMBER_SUCCESS if the route request was successfully submitted to the
// transmit queue, and EMBER_ERR_FATAL otherwise.
EmberStatus ezspSendManyToOneRouteRequest(
  // Must be either EMBER_HIGH_RAM_CONCENTRATOR or
  // EMBER_LOW_RAM_CONCENTRATOR. The former is used when the caller has
  // enough memory to store source routes for the whole network. In that
  // case, remote nodes stop sending route records once the concentrator has
  // successfully received one. The latter is used when the concentrator has
  // insufficient RAM to store all outbound source routes. In that case,
  // route records are sent to the concentrator prior to every inbound APS
  // unicast.
  uint16_t concentratorType,
  // The maximum number of hops the route request will be relayed. A radius
  // of zero is converted to EMBER_MAX_HOPS
  uint8_t radius);

// Periodically request any pending data from our parent. Setting interval to 0
// or units to EMBER_EVENT_INACTIVE will generate a single poll.
// Return: The result of sending the first poll.
EmberStatus ezspPollForData(
  // The time between polls. Note that the timer clock is free running and
  // is not synchronized with this command. This means that the time will be
  // between interval and (interval - 1). The maximum interval is 32767.
  uint16_t interval,
  // The units for interval.
  EmberEventUnits units,
  // The number of poll failures that will be tolerated before a
  // pollCompleteHandler callback is generated. A value of zero will result
  // in a callback for every poll. Any status value apart from EMBER_SUCCESS
  // and EMBER_MAC_NO_DATA is counted as a failure.
  uint8_t failureLimit);

// Callback
// Indicates the result of a data poll to the parent of the local node.
void ezspPollCompleteHandler(
  // An EmberStatus value: EMBER_SUCCESS - Data was received in response to
  // the poll. EMBER_MAC_NO_DATA - No data was pending.
  // EMBER_DELIVERY_FAILED - The poll message could not be sent.
  // EMBER_MAC_NO_ACK_RECEIVED - The poll message was sent but not
  // acknowledged by the parent.
  EmberStatus status);

// Callback
// Indicates that the local node received a data poll from a child.
void ezspPollHandler(
  // The node ID of the child that is requesting data.
  EmberNodeId childId);

// Callback
// A callback indicating a message has been received containing the EUI64 of the
// sender. This callback is called immediately before the incomingMessageHandler
// callback. It is not called if the incoming message did not contain the EUI64
// of the sender.
void ezspIncomingSenderEui64Handler(
  // The EUI64 of the sender
  EmberEUI64 senderEui64);

// Callback
// A callback indicating a message has been received.
void ezspIncomingMessageHandler(
  // The type of the incoming message. One of the following:
  // EMBER_INCOMING_UNICAST, EMBER_INCOMING_UNICAST_REPLY,
  // EMBER_INCOMING_MULTICAST, EMBER_INCOMING_MULTICAST_LOOPBACK,
  // EMBER_INCOMING_BROADCAST, EMBER_INCOMING_BROADCAST_LOOPBACK
  EmberIncomingMessageType type,
  // The APS frame from the incoming message.
  EmberApsFrame *apsFrame,
  // The link quality from the node that last relayed the message.
  uint8_t lastHopLqi,
  // The energy level (in units of dBm) observed during the reception.
  int8_t lastHopRssi,
  // The sender of the message.
  EmberNodeId sender,
  // The index of a binding that matches the message or 0xFF if there is no
  // matching binding.
  uint8_t bindingIndex,
  // The index of the entry in the address table that matches the sender of
  // the message or 0xFF if there is no matching entry.
  uint8_t addressIndex,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The incoming message.
  uint8_t *messageContents);

// Sets source route discovery(MTORR) mode to on, off, reschedule
// Return: Remaining time(ms) until next MTORR broadcast if the mode is on,
// MAX_INT32U_VALUE if the mode is off
uint32_t ezspSetSourceRouteDiscoveryMode(
  // Source route discovery mode: off:0, on:1, reschedule:2
  uint8_t mode);

// Callback
// A callback indicating that a many-to-one route to the concentrator with the
// given short and long id is available for use.
void ezspIncomingManyToOneRouteRequestHandler(
  // The short id of the concentrator.
  EmberNodeId source,
  // The EUI64 of the concentrator.
  EmberEUI64 longId,
  // The path cost to the concentrator. The cost may decrease as additional
  // route request packets for this discovery arrive, but the callback is
  // made only once.
  uint8_t cost);

// Callback
// A callback invoked when a route error message is received. The error
// indicates that a problem routing to or from the target node was encountered.
void ezspIncomingRouteErrorHandler(
  // EMBER_SOURCE_ROUTE_FAILURE or EMBER_MANY_TO_ONE_ROUTE_FAILURE.
  EmberStatus status,
  // The short id of the remote node.
  EmberNodeId target);

// Callback
// A callback invoked when a network status/route error message is received. The
// error indicates that there was a problem sending/receiving messages from the
// target node
void ezspIncomingNetworkStatusHandler(
  // One byte over-the-air error code from network status message
  uint8_t errorCode,
  // The short ID of the remote node
  EmberNodeId target);

// Callback
// Reports the arrival of a route record command frame.
void ezspIncomingRouteRecordHandler(
  // The source of the route record.
  EmberNodeId source,
  // The EUI64 of the source.
  EmberEUI64 sourceEui,
  // The link quality from the node that last relayed the route record.
  uint8_t lastHopLqi,
  // The energy level (in units of dBm) observed during the reception.
  int8_t lastHopRssi,
  // The number of relays in relayList.
  uint8_t relayCount,
  // The route record. Each relay in the list is an uint16_t node ID. The
  // list is passed as uint8_t * to avoid alignment problems.
  uint8_t *relayList);

// Supply a source route for the next outgoing message.
// Return: EMBER_SUCCESS if the source route was successfully stored, and
// EMBER_NO_BUFFERS otherwise.
EmberStatus ezspSetSourceRoute(
  // The destination of the source route.
  EmberNodeId destination,
  // The number of relays in relayList.
  uint8_t relayCount,
  // The source route.
  uint16_t *relayList);

// Send the network key to a destination.
// Return: EMBER_SUCCESS if send was successful
EmberStatus ezspUnicastCurrentNetworkKey(
  // The destination node of the key.
  EmberNodeId targetShort,
  // The long address of the destination node.
  EmberEUI64 targetLong,
  // The parent node of the destination node.
  EmberNodeId parentShortId);

// Indicates whether any messages are currently being sent using this address
// table entry. Note that this function does not indicate whether the address
// table entry is unused. To determine whether an address table entry is unused,
// check the remote node ID. The remote node ID will have the value
// EMBER_TABLE_ENTRY_UNUSED_NODE_ID when the address table entry is not in use.
// Return: True if the address table entry is active, false otherwise.
bool ezspAddressTableEntryIsActive(
  // The index of an address table entry.
  uint8_t addressTableIndex);

// Sets the EUI64 of an address table entry. This function will also check other
// address table entries, the child table and the neighbor table to see if the
// node ID for the given EUI64 is already known. If known then this function
// will also set node ID. If not known it will set the node ID to
// EMBER_UNKNOWN_NODE_ID.
// Return: EMBER_SUCCESS if the EUI64 was successfully set, and
// EMBER_ADDRESS_TABLE_ENTRY_IS_ACTIVE otherwise.
EmberStatus ezspSetAddressTableRemoteEui64(
  // The index of an address table entry.
  uint8_t addressTableIndex,
  // The EUI64 to use for the address table entry.
  EmberEUI64 eui64);

// Sets the short ID of an address table entry. Usually the application will not
// need to set the short ID in the address table. Once the remote EUI64 is set
// the stack is capable of figuring out the short ID on its own. However, in
// cases where the application does set the short ID, the application must set
// the remote EUI64 prior to setting the short ID.
void ezspSetAddressTableRemoteNodeId(
  // The index of an address table entry.
  uint8_t addressTableIndex,
  // The short ID corresponding to the remote node whose EUI64 is stored in
  // the address table at the given index or
  // EMBER_TABLE_ENTRY_UNUSED_NODE_ID which indicates that the entry stored
  // in the address table at the given index is not in use.
  EmberNodeId id);

// Gets the EUI64 of an address table entry.
void ezspGetAddressTableRemoteEui64(
  // The index of an address table entry.
  uint8_t addressTableIndex,
  // Return: The EUI64 of the address table entry is copied to this
  // location.
  EmberEUI64 eui64);

// Gets the short ID of an address table entry.
// Return: One of the following: The short ID corresponding to the remote node
// whose EUI64 is stored in the address table at the given index.
// EMBER_UNKNOWN_NODE_ID - Indicates that the EUI64 stored in the address table
// at the given index is valid but the short ID is currently unknown.
// EMBER_DISCOVERY_ACTIVE_NODE_ID - Indicates that the EUI64 stored in the
// address table at the given location is valid and network address discovery is
// underway. EMBER_TABLE_ENTRY_UNUSED_NODE_ID - Indicates that the entry stored
// in the address table at the given index is not in use.
EmberNodeId ezspGetAddressTableRemoteNodeId(
  // The index of an address table entry.
  uint8_t addressTableIndex);

// Tells the stack whether or not the normal interval between retransmissions of
// a retried unicast message should be increased by
// EMBER_INDIRECT_TRANSMISSION_TIMEOUT. The interval needs to be increased when
// sending to a sleepy node so that the message is not retransmitted until the
// destination has had time to wake up and poll its parent. The stack will
// automatically extend the timeout: - For our own sleepy children. - When an
// address response is received from a parent on behalf of its child. - When an
// indirect transaction expiry route error is received. - When an end device
// announcement is received from a sleepy node.
void ezspSetExtendedTimeout(
  // The address of the node for which the timeout is to be set.
  EmberEUI64 remoteEui64,
  // true if the retry interval should be increased by
  // EMBER_INDIRECT_TRANSMISSION_TIMEOUT. false if the normal retry interval
  // should be used.
  bool extendedTimeout);

// Indicates whether or not the stack will extend the normal interval between
// retransmissions of a retried unicast message by
// EMBER_INDIRECT_TRANSMISSION_TIMEOUT.
// Return: true if the retry interval will be increased by
// EMBER_INDIRECT_TRANSMISSION_TIMEOUT and false if the normal retry interval
// will be used.
bool ezspGetExtendedTimeout(
  // The address of the node for which the timeout is to be returned.
  EmberEUI64 remoteEui64);

// Replaces the EUI64, short ID and extended timeout setting of an address table
// entry. The previous EUI64, short ID and extended timeout setting are
// returned.
// Return: EMBER_SUCCESS if the EUI64, short ID and extended timeout setting
// were successfully modified, and EMBER_ADDRESS_TABLE_ENTRY_IS_ACTIVE
// otherwise.
EmberStatus ezspReplaceAddressTableEntry(
  // The index of the address table entry that will be modified.
  uint8_t addressTableIndex,
  // The EUI64 to be written to the address table entry.
  EmberEUI64 newEui64,
  // One of the following: The short ID corresponding to the new EUI64.
  // EMBER_UNKNOWN_NODE_ID if the new EUI64 is valid but the short ID is
  // unknown and should be discovered by the stack.
  // EMBER_TABLE_ENTRY_UNUSED_NODE_ID if the address table entry is now
  // unused.
  EmberNodeId newId,
  // true if the retry interval should be increased by
  // EMBER_INDIRECT_TRANSMISSION_TIMEOUT. false if the normal retry interval
  // should be used.
  bool newExtendedTimeout,
  // Return: The EUI64 of the address table entry before it was modified.
  EmberEUI64 oldEui64,
  // Return: One of the following: The short ID corresponding to the EUI64
  // before it was modified. EMBER_UNKNOWN_NODE_ID if the short ID was
  // unknown. EMBER_DISCOVERY_ACTIVE_NODE_ID if discovery of the short ID
  // was underway. EMBER_TABLE_ENTRY_UNUSED_NODE_ID if the address table
  // entry was unused.
  EmberNodeId *oldId,
  // Return: true if the retry interval was being increased by
  // EMBER_INDIRECT_TRANSMISSION_TIMEOUT. false if the normal retry interval
  // was being used.
  bool *oldExtendedTimeout);

// Returns the node ID that corresponds to the specified EUI64. The node ID is
// found by searching through all stack tables for the specified EUI64.
// Return: The short ID of the node or EMBER_NULL_NODE_ID if the short ID is not
// known.
EmberNodeId ezspLookupNodeIdByEui64(
  // The EUI64 of the node to look up.
  EmberEUI64 eui64);

// Returns the EUI64 that corresponds to the specified node ID. The EUI64 is
// found by searching through all stack tables for the specified node ID.
// Return: EMBER_SUCCESS if the EUI64 was found, EMBER_ERR_FATAL if the EUI64 is
// not known.
EmberStatus ezspLookupEui64ByNodeId(
  // The short ID of the node to look up.
  EmberNodeId nodeId,
  // Return: The EUI64 of the node.
  EmberEUI64 eui64);

// Gets an entry from the multicast table.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspGetMulticastTableEntry(
  // The index of a multicast table entry.
  uint8_t index,
  // Return: The contents of the multicast entry.
  EmberMulticastTableEntry *value);

// Sets an entry in the multicast table.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSetMulticastTableEntry(
  // The index of a multicast table entry
  uint8_t index,
  // The contents of the multicast entry.
  EmberMulticastTableEntry *value);

// Callback
// A callback invoked by the EmberZNet stack when an id conflict is discovered,
// that is, two different nodes in the network were found to be using the same
// short id. The stack automatically removes the conflicting short id from its
// internal tables (address, binding, route, neighbor, and child tables). The
// application should discontinue any other use of the id.
void ezspIdConflictHandler(
  // The short id for which a conflict was detected
  EmberNodeId id);

// Write the current node Id, PAN ID, or Node type to the tokens
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspWriteNodeData(
  // Erase the node type or not
  bool erase);

// Transmits the given message without modification. The MAC header is assumed
// to be configured in the message at the time this function is called.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSendRawMessage(
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The raw message.
  uint8_t *messageContents);

// Transmits the given message without modification. The MAC header is assumed
// to be configured in the message at the time this function is called.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSendRawMessageExtended(
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The raw message.
  uint8_t *messageContents,
  // transmit priority.
  uint8_t priority,
  // Should we enable CCA or not.
  bool useCca);

// Callback
// A callback invoked by the EmberZNet stack when a MAC passthrough message is
// received.
void ezspMacPassthroughMessageHandler(
  // The type of MAC passthrough message received.
  EmberMacPassthroughType messageType,
  // The link quality from the node that last relayed the message.
  uint8_t lastHopLqi,
  // The energy level (in units of dBm) observed during reception.
  int8_t lastHopRssi,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The raw message that was received.
  uint8_t *messageContents);

// Callback
// A callback invoked by the EmberZNet stack when a raw MAC message that has
// matched one of the application's configured MAC filters.
void ezspMacFilterMatchMessageHandler(
  // The index of the filter that was matched.
  uint8_t filterIndexMatch,
  // The type of MAC passthrough message received.
  EmberMacPassthroughType legacyPassthroughType,
  // The link quality from the node that last relayed the message.
  uint8_t lastHopLqi,
  // The energy level (in units of dBm) observed during reception.
  int8_t lastHopRssi,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The raw message that was received.
  uint8_t *messageContents);

// Callback
// A callback invoked by the EmberZNet stack when the MAC has finished
// transmitting a raw message.
void ezspRawTransmitCompleteHandler(
  // EMBER_SUCCESS if the transmission was successful, or
  // EMBER_DELIVERY_FAILED if not
  EmberStatus status);

// This function is useful to sleepy end devices. This function will set the
// retry interval (in milliseconds) for mac data poll. This interval is the time
// in milliseconds the device waits before retrying a data poll when a MAC level
// data poll fails for any reason.
void ezspSetMacPollFailureWaitTime(
  // Time in milliseconds the device waits before retrying a data poll when
  // a MAC level data poll fails for any reason.
  uint32_t waitBeforeRetryIntervalMs);

// Sets the priority masks and related variables for choosing the best beacon.
// Return: The attempt to set the pramaters returns EMBER_SUCCESS
EmberStatus ezspSetBeaconClassificationParams(
  // Return: Gets the beacon prioritization related variable
  EmberBeaconClassificationParams *param);

// Gets the priority masks and related variables for choosing the best beacon.
// Return: The attempt to get the pramaters returns EMBER_SUCCESS
EmberStatus ezspGetBeaconClassificationParams(
  // Return: Gets the beacon prioritization related variable
  EmberBeaconClassificationParams *param);

//------------------------------------------------------------------------------
// Security Frames
//------------------------------------------------------------------------------

// Sets the security state that will be used by the device when it forms or
// joins the network. This call should not be used when restoring saved network
// state via networkInit as this will result in a loss of security data and will
// cause communication problems when the device re-enters the network.
// Return: The success or failure code of the operation.
EmberStatus ezspSetInitialSecurityState(
  // The security configuration to be set.
  EmberInitialSecurityState *state);

// Gets the current security state that is being used by a device that is joined
// in the network.
// Return: The success or failure code of the operation.
EmberStatus ezspGetCurrentSecurityState(
  // Return: The security configuration in use by the stack.
  EmberCurrentSecurityState *state);

// Gets a Security Key based on the passed key type.
// Return: The success or failure code of the operation.
EmberStatus ezspGetKey(
  EmberKeyType keyType,
  // Return: The structure containing the key and its associated data.
  EmberKeyStruct *keyStruct);

// Callback
// A callback to inform the application that the Network Key has been updated
// and the node has been switched over to use the new key. The actual key being
// used is not passed up, but the sequence number is.
void ezspSwitchNetworkKeyHandler(
  // The sequence number of the new network key.
  uint8_t sequenceNumber);

// Retrieves the key table entry at the specified index.
// Return: EMBER_TABLE_ENTRY_ERASED if the index is an erased key entry.
// EMBER_INDEX_OUT_OF_RANGE if the passed index is not valid. EMBER_SUCCESS on
// success.
EmberStatus ezspGetKeyTableEntry(
  // The index of the entry in the table to retrieve.
  uint8_t index,
  // Return: The results retrieved by the stack.
  EmberKeyStruct *keyStruct);

// Sets the key table entry at the specified index.
// Return: EMBER_KEY_INVALID if the passed key data is using one of the reserved
// key values. EMBER_INDEX_OUT_OF_RANGE if passed index is not valid.
// EMBER_SUCCESS on success.
EmberStatus ezspSetKeyTableEntry(
  // The index of the entry in the table to set.
  uint8_t index,
  // The address of the partner device that shares the key
  EmberEUI64 address,
  // This bool indicates whether the key is a Link or a Master Key
  bool linkKey,
  // The actual key data associated with the table entry.
  EmberKeyData *keyData);

// This function searches through the Key Table and tries to find the entry that
// matches the passed search criteria.
// Return: This indicates the index of the entry that matches the search
// criteria. A value of 0xFF is returned if not matching entry is found.
uint8_t ezspFindKeyTableEntry(
  // The address to search for. Alternatively, all zeros may be passed in to
  // search for the first empty entry.
  EmberEUI64 address,
  // This indicates whether to search for an entry that contains a link key
  // or a master key. true means to search for an entry with a Link Key.
  bool linkKey);

// This function updates an existing entry in the key table or adds a new one.
// It first searches the table for an existing entry that matches the passed
// EUI64 address. If no entry is found, it searches for the first free entry. If
// successful, it updates the key data and resets the associated incoming frame
// counter. If it fails to find an existing entry and no free one exists, it
// returns a failure.
// Return: The success or failure error code of the operation.
EmberStatus ezspAddOrUpdateKeyTableEntry(
  // The address of the partner device associated with the Key.
  EmberEUI64 address,
  // An indication of whether this is a Link Key (true) or Master Key
  // (false)
  bool linkKey,
  // The actual key data associated with the entry.
  EmberKeyData *keyData);

// This function sends an APS TransportKey command containing the current trust
// center link key. The node to which the command is sent is specified via the
// short and long address arguments.
// Return: An EmberStatus value indicating success of failure of the operation
EmberStatus ezspSendTrustCenterLinkKey(
  // The short address of the node to which this command will be sent
  EmberNodeId destinationNodeId,
  // The long address of the node to which this command will be sent
  EmberEUI64 destinationEui64);

// This function erases the data in the key table entry at the specified index.
// If the index is invalid, false is returned.
// Return: The success or failure of the operation.
EmberStatus ezspEraseKeyTableEntry(
  // This indicates the index of entry to erase.
  uint8_t index);

// This function clears the key table of the current network.
// Return: The success or failure of the operation.
EmberStatus ezspClearKeyTable(void);

// A function to request a Link Key from the Trust Center with another device on
// the Network (which could be the Trust Center). A Link Key with the Trust
// Center is possible but the requesting device cannot be the Trust Center. Link
// Keys are optional in ZigBee Standard Security and thus the stack cannot know
// whether the other device supports them. If EMBER_REQUEST_KEY_TIMEOUT is
// non-zero on the Trust Center and the partner device is not the Trust Center,
// both devices must request keys with their partner device within the time
// period. The Trust Center only supports one outstanding key request at a time
// and therefore will ignore other requests. If the timeout is zero then the
// Trust Center will immediately respond and not wait for the second request.
// The Trust Center will always immediately respond to requests for a Link Key
// with it. Sleepy devices should poll at a higher rate until a response is
// received or the request times out. The success or failure of the request is
// returned via ezspZigbeeKeyEstablishmentHandler(...)
// Return: The success or failure of sending the request. This is not the final
// result of the attempt. ezspZigbeeKeyEstablishmentHandler(...) will return
// that.
EmberStatus ezspRequestLinkKey(
  // This is the IEEE address of the partner device that will share the link
  // key.
  EmberEUI64 partner);

// Requests a new link key from the Trust Center. This function starts by
// sending a Node Descriptor request to the Trust Center to verify its R21+
// stack version compliance. A Request Key message will then be sent, followed
// by a Verify Key Confirm message.
// Return: The success or failure of sending the request. If the Node Descriptor
// is successfully transmitted, ezspZigbeeKeyEstablishmentHandler(...) will be
// called at a later time with a final status result.
EmberStatus ezspUpdateTcLinkKey(
  // The maximum number of attempts a node should make when sending the Node
  // Descriptor, Request Key, and Verify Key Confirm messages. The number of
  // attempts resets for each message type sent (e.g., if maxAttempts is 3,
  // up to 3 Node Descriptors are sent, up to 3 Request Keys, and up to 3
  // Verify Key Confirm messages are sent).
  uint8_t maxAttempts);

// Callback
// This is a callback that indicates the success or failure of an attempt to
// establish a key with a partner device.
void ezspZigbeeKeyEstablishmentHandler(
  // This is the IEEE address of the partner that the device successfully
  // established a key with. This value is all zeros on a failure.
  EmberEUI64 partner,
  // This is the status indicating what was established or why the key
  // establishment failed.
  EmberKeyStatus status);

// This is a function to add a temporary link key for a joining device. The key
// will get timed out after a defined timeout period if the device does not
// update its link key with the Trust Center.
// Return: The success or failure of adding a transient key.
EmberStatus ezspAddTransientLinkKey(
  // This is the IEEE address of the partner that the device successfully
  // established a key with. This value is all zeros on a failure.
  EmberEUI64 partner,
  // The transient key data for the joining device.
  EmberKeyData *transientKey);

// Clear all of the transient link keys from RAM.
void ezspClearTransientLinkKeys(void);

// This is a function to get the transient link key structure in the transient
// key table. The EUI of the passed in key structure is searched and, if a match
// is found, the rest of the key structure is filled in.
// Return: The success or failure of getting the transient key.
EmberStatus ezspGetTransientLinkKey(
  // The IEEE address to look up the transient key for.
  EmberEUI64 eui,
  // Return: The transient key structure that is filled in upon success.
  EmberTransientKeyData *transientKeyData);

// Gets the transient link key at the index specified in the transient key
// table.
// Return: The success or failure of getting the transient key.
EmberStatus ezspGetTransientKeyTableEntry(
  // The index in the transient key table to fetch data from.
  uint8_t index,
  // Return: The transient key structure that is filled in upon success.
  EmberTransientKeyData *transientKeyData);

//------------------------------------------------------------------------------
// Trust Center Frames
//------------------------------------------------------------------------------

// Callback
// The NCP used the trust center behavior policy to decide whether to allow a
// new node to join the network. The Host cannot change the current decision,
// but it can change the policy for future decisions using the setPolicy
// command.
void ezspTrustCenterJoinHandler(
  // The Node Id of the node whose status changed
  EmberNodeId newNodeId,
  // The EUI64 of the node whose status changed.
  EmberEUI64 newNodeEui64,
  // The status of the node: Secure Join/Rejoin, Unsecure Join/Rejoin,
  // Device left.
  EmberDeviceUpdate status,
  // An EmberJoinDecision reflecting the decision made.
  EmberJoinDecision policyDecision,
  // The parent of the node whose status has changed.
  EmberNodeId parentOfNewNodeId);

// This function broadcasts a new encryption key, but does not tell the nodes in
// the network to start using it. To tell nodes to switch to the new key, use
// emberSendNetworkKeySwitch(). This is only valid for the Trust
// Center/Coordinator. It is up to the application to determine how quickly to
// send the Switch Key after sending the alternate encryption key.
// Return: EmberStatus value that indicates the success or failure of the
// command.
EmberStatus ezspBroadcastNextNetworkKey(
  // An optional pointer to a 16-byte encryption key
  // (EMBER_ENCRYPTION_KEY_SIZE). An all zero key may be passed in, which
  // will cause the stack to randomly generate a new key.
  EmberKeyData *key);

// This function broadcasts a switch key message to tell all nodes to change to
// the sequence number of the previously sent Alternate Encryption Key.
// Return: EmberStatus value that indicates the success or failure of the
// command.
EmberStatus ezspBroadcastNetworkKeySwitch(void);

// This function causes a coordinator to become the Trust Center when it is
// operating in a network that is not using one. It will send out an updated
// Network Key to all devices that will indicate a transition of the network to
// now use a Trust Center. The Trust Center should also switch all devices to
// using this new network key with the appropriate API.
EmberStatus ezspBecomeTrustCenter(
  // The key data for the Updated Network Key.
  EmberKeyData *newNetworkKey);

// This routine processes the passed chunk of data and updates the hash context
// based on it. If the 'finalize' parameter is not set, then the length of the
// data passed in must be a multiple of 16. If the 'finalize' parameter is set
// then the length can be any value up 1-16, and the final hash value will be
// calculated.
// Return: The result of the operation
EmberStatus ezspAesMmoHash(
  // The hash context to update.
  EmberAesMmoHashContext *context,
  // This indicates whether the final hash value should be calculated
  bool finalize,
  // The length of the data to hash.
  uint8_t length,
  // The data to hash.
  uint8_t *data,
  // Return: The updated hash context.
  EmberAesMmoHashContext *returnContext);

// This command sends an APS remove device using APS encryption to the
// destination indicating either to remove itself from the network, or one of
// its children.
// Return: An EmberStatus value indicating success, or the reason for failure
EmberStatus ezspRemoveDevice(
  // The node ID of the device that will receive the message
  EmberNodeId destShort,
  // The long address (EUI64) of the device that will receive the message.
  EmberEUI64 destLong,
  // The long address (EUI64) of the device to be removed.
  EmberEUI64 targetLong);

// This command will send a unicast transport key message with a new NWK key to
// the specified device. APS encryption using the device's existing link key
// will be used.
// Return: An EmberStatus value indicating success, or the reason for failure
EmberStatus ezspUnicastNwkKeyUpdate(
  // The node ID of the device that will receive the message
  EmberNodeId destShort,
  // The long address (EUI64) of the device that will receive the message.
  EmberEUI64 destLong,
  // The NWK key to send to the new device.
  EmberKeyData *key);

//------------------------------------------------------------------------------
// Certificate Based Key Exchange (CBKE) Frames
//------------------------------------------------------------------------------

// This call starts the generation of the ECC Ephemeral Public/Private key pair.
// When complete it stores the private key. The results are returned via
// ezspGenerateCbkeKeysHandler().
EmberStatus ezspGenerateCbkeKeys(void);

// Callback
// A callback by the Crypto Engine indicating that a new ephemeral
// public/private key pair has been generated. The public/private key pair is
// stored on the NCP, but only the associated public key is returned to the
// host. The node's associated certificate is also returned.
void ezspGenerateCbkeKeysHandler(
  // The result of the CBKE operation.
  EmberStatus status,
  // The generated ephemeral public key.
  EmberPublicKeyData *ephemeralPublicKey);

// Calculates the SMAC verification keys for both the initiator and responder
// roles of CBKE using the passed parameters and the stored public/private key
// pair previously generated with ezspGenerateKeysRetrieveCert(). It also stores
// the unverified link key data in temporary storage on the NCP until the key
// establishment is complete.
EmberStatus ezspCalculateSmacs(
  // The role of this device in the Key Establishment protocol.
  bool amInitiator,
  // The key establishment partner's implicit certificate.
  EmberCertificateData *partnerCertificate,
  // The key establishment partner's ephemeral public key
  EmberPublicKeyData *partnerEphemeralPublicKey);

// Callback
// A callback to indicate that the NCP has finished calculating the Secure
// Message Authentication Codes (SMAC) for both the initiator and responder. The
// associated link key is kept in temporary storage until the host tells the NCP
// to store or discard the key via emberClearTemporaryDataMaybeStoreLinkKey().
void ezspCalculateSmacsHandler(
  // The Result of the CBKE operation.
  EmberStatus status,
  // The calculated value of the initiator's SMAC
  EmberSmacData *initiatorSmac,
  // The calculated value of the responder's SMAC
  EmberSmacData *responderSmac);

// This call starts the generation of the ECC 283k1 curve Ephemeral
// Public/Private key pair. When complete it stores the private key. The results
// are returned via ezspGenerateCbkeKeysHandler283k1().
EmberStatus ezspGenerateCbkeKeys283k1(void);

// Callback
// A callback by the Crypto Engine indicating that a new 283k1 ephemeral
// public/private key pair has been generated. The public/private key pair is
// stored on the NCP, but only the associated public key is returned to the
// host. The node's associated certificate is also returned.
void ezspGenerateCbkeKeysHandler283k1(
  // The result of the CBKE operation.
  EmberStatus status,
  // The generated ephemeral public key.
  EmberPublicKey283k1Data *ephemeralPublicKey);

// Calculates the SMAC verification keys for both the initiator and responder
// roles of CBKE for the 283k1 ECC curve using the passed parameters and the
// stored public/private key pair previously generated with
// ezspGenerateKeysRetrieveCert283k1(). It also stores the unverified link key
// data in temporary storage on the NCP until the key establishment is complete.
EmberStatus ezspCalculateSmacs283k1(
  // The role of this device in the Key Establishment protocol.
  bool amInitiator,
  // The key establishment partner's implicit certificate.
  EmberCertificate283k1Data *partnerCertificate,
  // The key establishment partner's ephemeral public key
  EmberPublicKey283k1Data *partnerEphemeralPublicKey);

// Callback
// A callback to indicate that the NCP has finished calculating the Secure
// Message Authentication Codes (SMAC) for both the initiator and responder for
// the CBKE 283k1 Library. The associated link key is kept in temporary storage
// until the host tells the NCP to store or discard the key via
// emberClearTemporaryDataMaybeStoreLinkKey().
void ezspCalculateSmacsHandler283k1(
  // The Result of the CBKE operation.
  EmberStatus status,
  // The calculated value of the initiator's SMAC
  EmberSmacData *initiatorSmac,
  // The calculated value of the responder's SMAC
  EmberSmacData *responderSmac);

// Clears the temporary data associated with CBKE and the key establishment,
// most notably the ephemeral public/private key pair. If storeLinKey is true it
// moves the unverified link key stored in temporary storage into the link key
// table. Otherwise it discards the key.
EmberStatus ezspClearTemporaryDataMaybeStoreLinkKey(
  // A bool indicating whether to store (true) or discard (false) the
  // unverified link key derived when ezspCalculateSmacs() was previously
  // called.
  bool storeLinkKey);

// Clears the temporary data associated with CBKE and the key establishment,
// most notably the ephemeral public/private key pair. If storeLinKey is true it
// moves the unverified link key stored in temporary storage into the link key
// table. Otherwise it discards the key.
EmberStatus ezspClearTemporaryDataMaybeStoreLinkKey283k1(
  // A bool indicating whether to store (true) or discard (false) the
  // unverified link key derived when ezspCalculateSmacs() was previously
  // called.
  bool storeLinkKey);

// Retrieves the certificate installed on the NCP.
EmberStatus ezspGetCertificate(
  // Return: The locally installed certificate.
  EmberCertificateData *localCert);

// Retrieves the 283k certificate installed on the NCP.
EmberStatus ezspGetCertificate283k1(
  // Return: The locally installed certificate.
  EmberCertificate283k1Data *localCert);

// LEGACY FUNCTION: This functionality has been replaced by a single bit in the
// EmberApsFrame, EMBER_APS_OPTION_DSA_SIGN. Devices wishing to send signed
// messages should use that as it requires fewer function calls and message
// buffering. The dsaSignHandler response is still called when
// EMBER_APS_OPTION_DSA_SIGN is used. However, this function is still supported.
// This function begins the process of signing the passed message contained
// within the messageContents array. If no other ECC operation is going on, it
// will immediately return with EMBER_OPERATION_IN_PROGRESS to indicate the
// start of ECC operation. It will delay a period of time to let APS retries
// take place, but then it will shut down the radio and consume the CPU
// processing until the signing is complete. This may take up to 1 second. The
// signed message will be returned in the dsaSignHandler response. Note that the
// last byte of the messageContents passed to this function has special
// significance. As the typical use case for DSA signing is to sign the ZCL
// payload of a DRLC Report Event Status message in SE 1.0, there is often both
// a signed portion (ZCL payload) and an unsigned portion (ZCL header). The last
// byte in the content of messageToSign is therefore used as a special indicator
// to signify how many bytes of leading data in the array should be excluded
// from consideration during the signing process. If the signature needs to
// cover the entire array (all bytes except last one), the caller should ensure
// that the last byte of messageContents is 0x00. When the signature operation
// is complete, this final byte will be replaced by the signature type indicator
// (0x01 for ECDSA signatures), and the actual signature will be appended to the
// original contents after this byte.
// Return: EMBER_OPERATION_IN_PROGRESS if the stack has queued up the operation
// for execution. EMBER_INVALID_CALL if the operation can't be performed in this
// context, possibly because another ECC operation is pending.
EmberStatus ezspDsaSign(
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The message contents for which to create a signature. Per above notes,
  // this may include a leading portion of data not included in the
  // signature, in which case the last byte of this array should be set to
  // the index of the first byte to be considered for signing. Otherwise,
  // the last byte of messageContents should be 0x00 to indicate that a
  // signature should occur across the entire contents.
  uint8_t *messageContents);

// Callback
// The handler that returns the results of the signing operation. On success,
// the signature will be appended to the original message (including the
// signature type indicator that replaced the startIndex field for the signing)
// and both are returned via this callback.
void ezspDsaSignHandler(
  // The result of the DSA signing operation.
  EmberStatus status,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The message and attached which includes the original message and the
  // appended signature.
  uint8_t *messageContents);

// Verify that signature of the associated message digest was signed by the
// private key of the associated certificate.
EmberStatus ezspDsaVerify(
  // The AES-MMO message digest of the signed data. If dsaSign command was
  // used to generate the signature for this data, the final byte (replaced
  // by signature type of 0x01) in the messageContents array passed to
  // dsaSign is included in the hash context used for the digest
  // calculation.
  EmberMessageDigest *digest,
  // The certificate of the signer. Note that the signer's certificate and
  // the verifier's certificate must both be issued by the same Certificate
  // Authority, so they should share the same CA Public Key.
  EmberCertificateData *signerCertificate,
  // The signature of the signed data.
  EmberSignatureData *receivedSig);

// Callback
// This callback is executed by the stack when the DSA verification has
// completed and has a result. If the result is EMBER_SUCCESS, the signature is
// valid. If the result is EMBER_SIGNATURE_VERIFY_FAILURE then the signature is
// invalid. If the result is anything else then the signature verify operation
// failed and the validity is unknown.
void ezspDsaVerifyHandler(
  // The result of the DSA verification operation.
  EmberStatus status);

// Verify that signature of the associated message digest was signed by the
// private key of the associated certificate.
EmberStatus ezspDsaVerify283k1(
  // The AES-MMO message digest of the signed data. If dsaSign command was
  // used to generate the signature for this data, the final byte (replaced
  // by signature type of 0x01) in the messageContents array passed to
  // dsaSign is included in the hash context used for the digest
  // calculation.
  EmberMessageDigest *digest,
  // The certificate of the signer. Note that the signer's certificate and
  // the verifier's certificate must both be issued by the same Certificate
  // Authority, so they should share the same CA Public Key.
  EmberCertificate283k1Data *signerCertificate,
  // The signature of the signed data.
  EmberSignature283k1Data *receivedSig);

// Sets the device's CA public key, local certificate, and static private key on
// the NCP associated with this node.
EmberStatus ezspSetPreinstalledCbkeData(
  // The Certificate Authority's public key.
  EmberPublicKeyData *caPublic,
  // The node's new certificate signed by the CA.
  EmberCertificateData *myCert,
  // The node's new static private key.
  EmberPrivateKeyData *myKey);

// Sets the device's 283k1 curve CA public key, local certificate, and static
// private key on the NCP associated with this node.
EmberStatus ezspSavePreinstalledCbkeData283k1(void);

//------------------------------------------------------------------------------
// Mfglib Frames
//------------------------------------------------------------------------------

// Activate use of mfglib test routines and enables the radio receiver to report
// packets it receives to the mfgLibRxHandler() callback. These packets will not
// be passed up with a CRC failure. All other mfglib functions will return an
// error until the mfglibStart() has been called
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspMfglibStart(
  // true to generate a mfglibRxHandler callback when a packet is received.
  bool rxCallback);

// Deactivate use of mfglib test routines; restores the hardware to the state it
// was in prior to mfglibStart() and stops receiving packets started by
// mfglibStart() at the same time.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus mfglibEnd(void);

// Starts transmitting an unmodulated tone on the currently set channel and
// power level. Upon successful return, the tone will be transmitting. To stop
// transmitting tone, application must call mfglibStopTone(), allowing it the
// flexibility to determine its own criteria for tone duration (time, event,
// etc.)
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus mfglibStartTone(void);

// Stops transmitting tone started by mfglibStartTone().
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus mfglibStopTone(void);

// Starts transmitting a random stream of characters. This is so that the radio
// modulation can be measured.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus mfglibStartStream(void);

// Stops transmitting a random stream of characters started by
// mfglibStartStream().
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus mfglibStopStream(void);

// Sends a single packet consisting of the following bytes: packetLength,
// packetContents[0], ... , packetContents[packetLength - 3], CRC[0], CRC[1].
// The total number of bytes sent is packetLength + 1. The radio replaces the
// last two bytes of packetContents[] with the 16-bit CRC for the packet.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus mfglibSendPacket(
  // The length of the packetContents parameter in bytes. Must be greater
  // than 3 and less than 123.
  uint8_t packetLength,
  // The packet to send. The last two bytes will be replaced with the 16-bit
  // CRC.
  uint8_t *packetContents);

// Sets the radio channel. Calibration occurs if this is the first time the
// channel has been used.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus mfglibSetChannel(
  // The channel to switch to. Valid values are 11 to 26.
  uint8_t channel);

// Returns the current radio channel, as previously set via mfglibSetChannel().
// Return: The current channel.
uint8_t mfglibGetChannel(void);

// First select the transmit power mode, and then include a method for selecting
// the radio transmit power. The valid power settings depend upon the specific
// radio in use. Ember radios have discrete power settings, and then requested
// power is rounded to a valid power setting; the actual power output is
// available to the caller via mfglibGetPower().
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus mfglibSetPower(
  // Power mode. Refer to txPowerModes in stack/include/ember-types.h for
  // possible values.
  uint16_t txPowerMode,
  // Power in units of dBm. Refer to radio data sheet for valid range.
  int8_t power);

// Returns the current radio power setting, as previously set via
// mfglibSetPower().
// Return: Power in units of dBm. Refer to radio data sheet for valid range.
int8_t mfglibGetPower(void);

// Callback
// A callback indicating a packet with a valid CRC has been received.
void ezspMfglibRxHandler(
  // The link quality observed during the reception
  uint8_t linkQuality,
  // The energy level (in units of dBm) observed during the reception.
  int8_t rssi,
  // The length of the packetContents parameter in bytes. Will be greater
  // than 3 and less than 123.
  uint8_t packetLength,
  // The received packet (last 2 bytes are not FCS / CRC and may be
  // discarded)
  uint8_t *packetContents);

//------------------------------------------------------------------------------
// Bootloader Frames
//------------------------------------------------------------------------------

// Quits the current application and launches the standalone bootloader (if
// installed) The function returns an error if the standalone bootloader is not
// present
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspLaunchStandaloneBootloader(
  // Controls the mode in which the standalone bootloader will run. See the
  // app. note for full details. Options are:
  // STANDALONE_BOOTLOADER_NORMAL_MODE: Will listen for an over-the-air
  // image transfer on the current channel with current power settings.
  // STANDALONE_BOOTLOADER_RECOVERY_MODE: Will listen for an over-the-air
  // image transfer on the default channel with default power settings. Both
  // modes also allow an image transfer to begin with XMODEM over the serial
  // protocol's Bootloader Frame.
  uint8_t mode);

// Transmits the given bootload message to a neighboring node using a specific
// 802.15.4 header that allows the EmberZNet stack as well as the bootloader to
// recognize the message, but will not interfere with other ZigBee stacks.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSendBootloadMessage(
  // If true, the destination address and pan id are both set to the
  // broadcast address.
  bool broadcast,
  // The EUI64 of the target node. Ignored if the broadcast field is set to
  // true.
  EmberEUI64 destEui64,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The multicast message.
  uint8_t *messageContents);

// Detects if the standalone bootloader is installed, and if so returns the
// installed version. If not return 0xffff. A returned version of 0x1234 would
// indicate version 1.2 build 34. Also return the node's version of PLAT, MICRO
// and PHY.
// Return: BOOTLOADER_INVALID_VERSION if the standalone bootloader is not
// present, or the version of the installed standalone bootloader.
uint16_t ezspGetStandaloneBootloaderVersionPlatMicroPhy(
  // Return: The value of PLAT on the node
  uint8_t *nodePlat,
  // Return: The value of MICRO on the node
  uint8_t *nodeMicro,
  // Return: The value of PHY on the node
  uint8_t *nodePhy);

// Callback
// A callback invoked by the EmberZNet stack when a bootload message is
// received.
void ezspIncomingBootloadMessageHandler(
  // The EUI64 of the sending node.
  EmberEUI64 longId,
  // The link quality from the node that last relayed the message.
  uint8_t lastHopLqi,
  // The energy level (in units of dBm) observed during the reception.
  int8_t lastHopRssi,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The bootload message that was sent.
  uint8_t *messageContents);

// Callback
// A callback invoked by the EmberZNet stack when the MAC has finished
// transmitting a bootload message.
void ezspBootloadTransmitCompleteHandler(
  // An EmberStatus value of EMBER_SUCCESS if an ACK was received from the
  // destination or EMBER_DELIVERY_FAILED if no ACK was received.
  EmberStatus status,
  // The length of the messageContents parameter in bytes.
  uint8_t messageLength,
  // The message that was sent.
  uint8_t *messageContents);

// Perform AES encryption on plaintext using key.
void ezspAesEncrypt(
  // 16 bytes of plaintext.
  uint8_t *plaintext,
  // The 16-byte encryption key to use.
  uint8_t *key,
  // Return: 16 bytes of ciphertext.
  uint8_t *ciphertext);

//------------------------------------------------------------------------------
// ZLL Frames
//------------------------------------------------------------------------------

// A consolidation of ZLL network operations with similar signatures;
// specifically, forming and joining networks or touch-linking.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspZllNetworkOps(
  // Information about the network.
  EmberZllNetwork *networkInfo,
  // Operation indicator.
  EzspZllNetworkOperation op,
  // Radio transmission power.
  int8_t radioTxPower);

// This call will cause the device to setup the security information used in its
// network. It must be called prior to forming, starting, or joining a network.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspZllSetInitialSecurityState(
  // ZLL Network key.
  EmberKeyData *networkKey,
  // Initial security state of the network.
  EmberZllInitialSecurityState *securityState);

// This call will update ZLL security token information. Unlike
// emberZllSetInitialSecurityState, this can be called while a network is
// already established.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspZllSetSecurityStateWithoutKey(
  // Security state of the network.
  EmberZllInitialSecurityState *securityState);

// This call will initiate a ZLL network scan on all the specified channels.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspZllStartScan(
  // The range of channels to scan.
  uint32_t channelMask,
  // The radio output power used for the scan requests.
  int8_t radioPowerForScan,
  // The node type of the local device.
  EmberNodeType nodeType);

// This call will change the mode of the radio so that the receiver is on for a
// specified amount of time when the device is idle.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspZllSetRxOnWhenIdle(
  // The duration in milliseconds to leave the radio on.
  uint32_t durationMs);

// Callback
// This call is fired when a ZLL network scan finds a ZLL network.
void ezspZllNetworkFoundHandler(
  // Information about the network.
  EmberZllNetwork *networkInfo,
  // Used to interpret deviceInfo field.
  bool isDeviceInfoNull,
  // Device specific information.
  EmberZllDeviceInfoRecord *deviceInfo,
  // The link quality from the node that last relayed the message.
  uint8_t lastHopLqi,
  // The energy level (in units of dBm) observed during reception.
  int8_t lastHopRssi);

// Callback
// This call is fired when a ZLL network scan is complete.
void ezspZllScanCompleteHandler(
  // Status of the operation.
  EmberStatus status);

// Callback
// This call is fired when network and group addresses are assigned to a remote
// mode in a network start or network join request.
void ezspZllAddressAssignmentHandler(
  // Address assignment information.
  EmberZllAddressAssignment *addressInfo,
  // The link quality from the node that last relayed the message.
  uint8_t lastHopLqi,
  // The energy level (in units of dBm) observed during reception.
  int8_t lastHopRssi);

// Callback
// This call is fired when the device is a target of a touch link.
void ezspZllTouchLinkTargetHandler(
  // Information about the network.
  EmberZllNetwork *networkInfo);

// Get the ZLL tokens.
void ezspZllGetTokens(
  // Return: Data token return value.
  EmberTokTypeStackZllData *data,
  // Return: Security token return value.
  EmberTokTypeStackZllSecurity *security);

// Set the ZLL data token.
void ezspZllSetDataToken(
  // Data token to be set.
  EmberTokTypeStackZllData *data);

// Set the ZLL data token bitmask to reflect the ZLL network state.
void ezspZllSetNonZllNetwork(void);

// Is this a ZLL network?
// Return: ZLL network?
bool ezspIsZllNetwork(void);

// This call sets the radio's default idle power mode.
void ezspZllSetRadioIdleMode(
  // The power mode to be set.
  EmberRadioPowerMode mode);

// This call gets the radio's default idle power mode.
// Return: The current power mode.
uint8_t ezspZllGetRadioIdleMode(void);

// This call sets the default node type for a factory new ZLL device.
void ezspSetZllNodeType(
  // The node type to be set.
  EmberNodeType nodeType);

// This call sets additional capability bits in the ZLL state.
void ezspSetZllAdditionalState(
  // A mask with the bits to be set or cleared.
  uint16_t state);

// Is there a ZLL (Touchlink) operation in progress?
// Return: ZLL operation in progress?
bool ezspZllOperationInProgress(void);

// Is the ZLL radio on when idle mode is active?
// Return: ZLL radio on when idle mode is active?
bool ezspZllRxOnWhenIdleGetActive(void);

// Informs the ZLL API that application scanning is complete
void ezspZllScanningComplete(void);

// Get the primary ZLL (touchlink) channel mask.
// Return: The primary ZLL channel mask
uint32_t ezspGetZllPrimaryChannelMask(void);

// Get the secondary ZLL (touchlink) channel mask.
// Return: The secondary ZLL channel mask
uint32_t ezspGetZllSecondaryChannelMask(void);

// Set the primary ZLL (touchlink) channel mask
void ezspSetZllPrimaryChannelMask(
  // The primary ZLL channel mask
  uint32_t zllPrimaryChannelMask);

// Set the secondary ZLL (touchlink) channel mask.
void ezspSetZllSecondaryChannelMask(
  // The secondary ZLL channel mask
  uint32_t zllSecondaryChannelMask);

// Clear ZLL stack tokens.
void ezspZllClearTokens(void);

//------------------------------------------------------------------------------
// WWAH Frames
//------------------------------------------------------------------------------

// Sets whether to use parent classification when processing beacons during a
// join or rejoin. Parent classification considers whether a received beacon
// indicates trust center connectivity and long uptime on the network
void ezspSetParentClassificationEnabled(
  // Enable or disable parent classification
  bool enabled);

// Gets whether to use parent classification when processing beacons during a
// join or rejoin. Parent classification considers whether a received beacon
// indicates trust center connectivity and long uptime on the network
// Return: Enable or disable parent classification
bool ezspGetParentClassificationEnabled(void);

// sets the device uptime to be long or short
void ezspSetLongUpTime(
  // if the uptime is long or not
  bool hasLongUpTime);

// sets the hub connectivity to be true or false
void ezspSetHubConnectivity(
  // if the hub is connected or not
  bool connected);

// checks if the device uptime is long or short
// Return: if the uptime is long or not
bool ezspIsUpTimeLong(void);

// checks if the hub is connected or not
// Return: if the hub is connected or not
bool ezspIsHubConnected(void);

//------------------------------------------------------------------------------
// Green Power Frames
//------------------------------------------------------------------------------

// Update the GP Proxy table based on a GP pairing.
// Return: Whether a GP Pairing has been created or not.
bool ezspGpProxyTableProcessGpPairing(
  // The options field of the GP Pairing command.
  uint32_t options,
  // The target GPD.
  EmberGpAddress *addr,
  // The communication mode of the GP Sink.
  uint8_t commMode,
  // The network address of the GP Sink.
  uint16_t sinkNetworkAddress,
  // The group ID of the GP Sink.
  uint16_t sinkGroupId,
  // The alias assigned to the GPD.
  uint16_t assignedAlias,
  // The IEEE address of the GP Sink.
  uint8_t *sinkIeeeAddress,
  // The key to use for the target GPD.
  EmberKeyData *gpdKey,
  // The GPD security frame counter.
  uint32_t gpdSecurityFrameCounter,
  // The forwarding radius.
  uint8_t forwardingRadius);

// Adds/removes an entry from the GP Tx Queue.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspDGpSend(
  // The action to perform on the GP TX queue (true to add, false to
  // remove).
  bool action,
  // Whether to use ClearChannelAssessment when transmitting the GPDF.
  bool useCca,
  // The Address of the destination GPD.
  EmberGpAddress *addr,
  // The GPD command ID to send.
  uint8_t gpdCommandId,
  // The length of the GP command payload.
  uint8_t gpdAsduLength,
  // The GP command payload.
  uint8_t *gpdAsdu,
  // The handle to refer to the GPDF.
  uint8_t gpepHandle,
  // How long to keep the GPDF in the TX Queue.
  uint16_t gpTxQueueEntryLifetimeMs);

// Callback
// A callback to the GP endpoint to indicate the result of the GPDF
// transmission.
void ezspDGpSentHandler(
  // An EmberStatus value indicating success or the reason for failure.
  EmberStatus status,
  // The handle of the GPDF.
  uint8_t gpepHandle);

// Callback
// A callback invoked by the ZigBee GP stack when a GPDF is received.
void ezspGpepIncomingMessageHandler(
  // The status of the GPDF receive.
  EmberStatus status,
  // The gpdLink value of the received GPDF.
  uint8_t gpdLink,
  // The GPDF sequence number.
  uint8_t sequenceNumber,
  // The address of the source GPD.
  EmberGpAddress *addr,
  // The security level of the received GPDF.
  EmberGpSecurityLevel gpdfSecurityLevel,
  // The securityKeyType used to decrypt/authenticate the incoming GPDF.
  EmberGpKeyType gpdfSecurityKeyType,
  // Whether the incoming GPDF had the auto-commissioning bit set.
  bool autoCommissioning,
  // Bidirectional information represented in bitfields, where bit0 holds
  // the rxAfterTx of incoming gpdf and bit1 holds if tx queue is available
  // for outgoing gpdf.
  uint8_t bidirectionalInfo,
  // The security frame counter of the incoming GDPF.
  uint32_t gpdSecurityFrameCounter,
  // The gpdCommandId of the incoming GPDF.
  uint8_t gpdCommandId,
  // The received MIC of the GPDF.
  uint32_t mic,
  // The proxy table index of the corresponding proxy table entry to the
  // incoming GPDF.
  uint8_t proxyTableIndex,
  // The length of the GPD command payload.
  uint8_t gpdCommandPayloadLength,
  // The GPD command payload.
  uint8_t *gpdCommandPayload);

// Retrieves the proxy table entry stored at the passed index.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspGpProxyTableGetEntry(
  // The index of the requested proxy table entry.
  uint8_t proxyIndex,
  // Return: An EmberGpProxyTableEntry struct containing a copy of the
  // requested proxy entry.
  EmberGpProxyTableEntry *entry);

// Finds the index of the passed address in the gp table.
// Return: The index, or 0xFF for not found
uint8_t ezspGpProxyTableLookup(
  // The address to search for
  EmberGpAddress *addr);

// Retrieves the sink table entry stored at the passed index.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspGpSinkTableGetEntry(
  // The index of the requested sink table entry.
  uint8_t sinkIndex,
  // Return: An EmberGpSinkTableEntry struct containing a copy of the
  // requested sink entry.
  EmberGpSinkTableEntry *entry);

// Finds the index of the passed address in the gp table.
// Return: The index, or 0xFF for not found
uint8_t ezspGpSinkTableLookup(
  // The address to search for.
  EmberGpAddress *addr);

// Retrieves the sink table entry stored at the passed index.
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspGpSinkTableSetEntry(
  // The index of the requested sink table entry.
  uint8_t sinkIndex,
  // An EmberGpSinkTableEntry struct containing a copy of the sink entry to
  // be updated.
  EmberGpSinkTableEntry *entry);

// Removes the sink table entry stored at the passed index.
void ezspGpSinkTableRemoveEntry(
  // The index of the requested sink table entry.
  uint8_t sinkIndex);

// Finds or allocates a sink entry
// Return: An index of found or allocated sink or 0xFF if failed.
uint8_t ezspGpSinkTableFindOrAllocateEntry(
  // An EmberGpAddress struct containing a copy of the gpd address to be
  // found.
  EmberGpAddress *addr);

// Clear the entire sink table
void ezspGpSinkTableClearAll(void);

// Iniitializes Sink Table
void ezspGpSinkTableInit(void);

// Sets security framecounter in the sink table
void ezspGpSinkTableSetSecurityFrameCounter(
  // Index to the Sink table
  uint8_t index,
  // Security Frame Counter
  uint32_t sfc);

//------------------------------------------------------------------------------
// Secure EZSP Frames
//------------------------------------------------------------------------------

// Set the Security Key of the Secure EZSP Protocol.
// Return: An EzspStatus value indicating success or the reason for failure.
EzspStatus ezspSetSecurityKey(
  // The key to use for the Secure EZSP Protocol.
  EmberKeyData *key,
  // The security type to be used for the Secure EZSP Protocol.
  SecureEzspSecurityType securityType);

// Set the Host-side Security Parameters of the Secure EZSP Protocol.
// Return: An EzspStatus value indicating success or the reason for failure.
EzspStatus ezspSetSecurityParameters(
  // The security level to be used for the Secure EZSP communication.
  SecureEzspSecurityLevel securityLevel,
  // The Host-side random number to be used for Session ID generation.
  SecureEzspRandomNumber *hostRandomNumber,
  // Return: The NCP-side random number to be used for Session ID
  // generation.
  SecureEzspRandomNumber *returnNcpRandomNumber);

// Resets security key and security parameters of the Secure EZSP protocol. Node
// leaves the network before doing so for security reasons.
// Return: An EzspStatus value indicating success or the reason for failure.
EzspStatus ezspResetToFactoryDefaults(void);

// Get the security key status on the NCP: whether the security key is set or
// not and what the security type is.
// Return: An EzspStatus value indicating whether the security key is set or
// not.
EzspStatus ezspGetSecurityKeyStatus(
  // Return: The security type set at NCP for the Secure EZSP Protocol.
  SecureEzspSecurityType *returnSecurityType);

//------------------------------------------------------------------------------
// Token Interface Frames
//------------------------------------------------------------------------------

// Gets the total number of tokens.
// Return: Total number of tokens.
uint8_t ezspGetTokenCount(void);

// Gets the token information for a single token at provided index
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspGetTokenInfo(
  // Index of the token in the token table for which information is needed.
  uint8_t index,
  // Return: Token information.
  EmberTokenInfo *tokenInfo);

// Gets the token data for a single token with provided key
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspGetTokenData(
  // Key of the token in the token table for which data is needed.
  uint32_t token,
  // Index in case of the indexed token.
  uint32_t index,
  // Return: Token Data
  EmberTokenData *tokenData);

// Sets the token data for a single token with provided key
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus ezspSetTokenData(
  // Key of the token in the token table for which data is to be set.
  uint32_t token,
  // Index in case of the indexed token.
  uint32_t index,
  // Token Data
  EmberTokenData *tokenData);

// Reset the node by calling halReboot.
void ezspResetNode(void);
