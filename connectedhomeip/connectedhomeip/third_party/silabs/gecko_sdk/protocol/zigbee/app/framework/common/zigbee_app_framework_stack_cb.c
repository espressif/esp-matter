/*****************************************************************************/
/**
 * Copyright 2020 Silicon Laboratories, Inc.
 *
 *****************************************************************************/
//
// *** Generated file. Do not edit! ***
//

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "zigbee_stack_callback_dispatcher.h"
#include "zigbee_app_framework_callback.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif

#ifdef SL_CATALOG_ZIGBEE_ZCL_FRAMEWORK_CORE_PRESENT
#include "af.h"
#else
#define emberAfPushCallbackNetworkIndex()
#define emberAfPopNetworkIndex()
#endif

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfIncomingMessageCallback
WEAK(void emberAfIncomingMessageCallback(
       // Incoming message type
       EmberIncomingMessageType type,
       // The aps Frame
       EmberApsFrame *apsFrame,
       // message
       EmberMessageBuffer message))
{
  (void)type;
  (void)apsFrame;
  (void)message;
}

// A callback invoked when receiving a message.
void emberIncomingMessageHandler(
  // Incoming message type
  EmberIncomingMessageType type,
  // The aps Frame
  EmberApsFrame *apsFrame,
  // message
  EmberMessageBuffer message)
{
  emberAfPushCallbackNetworkIndex();
  emAfIncomingMessage(type, apsFrame, message);
  emberAfIncomingMessageCallback(type, apsFrame, message);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// A callback invoked when a messageSend has been tried by the stack.
void emberMessageSentHandler(
  // Outgoing message type
  EmberOutgoingMessageType type,
  // Index or destination of the message
  uint16_t indexOrDestination,
  // The aps Frame
  EmberApsFrame *apsFrame,
  // message
  EmberMessageBuffer message,
  // Message delivery status
  EmberStatus status)
{
  emberAfPushCallbackNetworkIndex();
  emAfMessageSent(type, indexOrDestination, apsFrame, message, status);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of emAfTrustCenterJoinCallback
WEAK(EmberJoinDecision emAfTrustCenterJoinCallback(
       // Joining node's id
       EmberNodeId newNodeId,
       // Joining node's Eui64
       EmberEUI64 newNodeEui64,
       // Status
       EmberDeviceUpdate status,
       // Parent Id of the joining node
       EmberNodeId parentOfNewNode))
{
  (void)newNodeId;
  (void)newNodeEui64;
  (void)status;
  (void)parentOfNewNode;
  return EMBER_NO_ACTION;
}

// A callback invoked when a node attempts to join a TC
EmberJoinDecision emberTrustCenterJoinHandler(
  // Joining node's id
  EmberNodeId newNodeId,
  // Joining node's Eui64
  EmberEUI64 newNodeEui64,
  // Status
  EmberDeviceUpdate status,
  // Parent Id of the joining node
  EmberNodeId parentOfNewNode)
{
  EmberJoinDecision ret;
  emberAfPushCallbackNetworkIndex();
  ret = emAfTrustCenterJoinCallback(newNodeId, newNodeEui64, status, parentOfNewNode);
  emAfTrustCenterJoin(newNodeId, newNodeEui64, status, parentOfNewNode);
  emberAfPopNetworkIndex();
  return ret;
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfMarkBuffersCallback
WEAK(void emberAfMarkBuffersCallback(void))
{
}

// A callback that allows marking application buffers
void emberMarkBuffersHandler(void)
{
  emberAfPushCallbackNetworkIndex();
  emAfMarkBuffers();
  emberAfMarkBuffersCallback();
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of emAfPacketHandoffIncomingCallback
WEAK(EmberPacketAction emAfPacketHandoffIncomingCallback(
       EmberZigbeePacketType packetType,
       EmberMessageBuffer packetBuffer,
       uint8_t index,
       // Return:
       void *data))
{
  (void)packetType;
  (void)packetBuffer;
  (void)index;
  (void)data;
  return EMBER_ACCEPT_PACKET;
}

// A callback that allows the app to instrument the stack on what to do with an
// incoming packet
EmberPacketAction emberPacketHandoffIncomingHandler(
  EmberZigbeePacketType packetType,
  EmberMessageBuffer packetBuffer,
  uint8_t index,
  // Return:
  void *data)
{
  EmberPacketAction ret;
  emberAfPushCallbackNetworkIndex();
  ret = emAfPacketHandoffIncomingCallback(packetType, packetBuffer, index, data);
  emAfPacketHandoffIncoming(packetType, packetBuffer, index, data);
  emberAfPopNetworkIndex();
  return ret;
}

// -----------------------------------------------------------------------------
// Weak implementation of emAfPacketHandoffOutgoingCallback
WEAK(EmberPacketAction emAfPacketHandoffOutgoingCallback(
       EmberZigbeePacketType packetType,
       EmberMessageBuffer packetBuffer,
       uint8_t index,
       // Return:
       void *data))
{
  (void)packetType;
  (void)packetBuffer;
  (void)index;
  (void)data;
  return EMBER_ACCEPT_PACKET;
}

// A callback that allows the app to instrument the stack on what to do with an
// outgoing packet
EmberPacketAction emberPacketHandoffOutgoingHandler(
  EmberZigbeePacketType packetType,
  EmberMessageBuffer packetBuffer,
  uint8_t index,
  // Return:
  void *data)
{
  EmberPacketAction ret;
  emberAfPushCallbackNetworkIndex();
  ret = emAfPacketHandoffOutgoingCallback(packetType, packetBuffer, index, data);
  emAfPacketHandoffOutgoing(packetType, packetBuffer, index, data);
  emberAfPopNetworkIndex();
  return ret;
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfIncomingMfgTestMessageCallback
WEAK(void emberAfIncomingMfgTestMessageCallback(
       uint8_t messageType,
       uint8_t dataLength,
       // Return:
       uint8_t *data))
{
  (void)messageType;
  (void)dataLength;
  (void)data;
  return;
}

// incomingMfgTestMessageHandler
void emberIncomingMfgTestMessageHandler(
  uint8_t messageType,
  uint8_t dataLength,
  // Return:
  uint8_t *data)
{
  emberAfPushCallbackNetworkIndex();
  emAfIncomingMfgTestMessage(messageType, dataLength, data);
  emberAfIncomingMfgTestMessageCallback(messageType, dataLength, data);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Allows the application to decide if it wants to handle a route record itself
void emberOverrideIncomingRouteRecordHandler(
  EmberNodeId source,
  EmberEUI64 sourceEui,
  uint8_t relayCount,
  EmberMessageBuffer header,
  uint8_t relayListIndex,
  // Return:
  bool *consumed)
{
  emberAfPushCallbackNetworkIndex();
  emAfOverrideIncomingRouteRecord(source, sourceEui, relayCount, header, relayListIndex, consumed);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of emAfOverrideAppendSourceRouteCallback
WEAK(uint8_t emAfOverrideAppendSourceRouteCallback(
       EmberNodeId destination,
       // Return:
       EmberMessageBuffer *header,
       // Return:
       bool *consumed))
{
  (void)destination;
  (void)header;
  (void)consumed;
  return 0;
}

// Allows the application to decide if it wants to append a source route to
// packet header itself
// Return: Added bytes
uint8_t emberOverrideAppendSourceRouteHandler(
  EmberNodeId destination,
  // Return:
  EmberMessageBuffer *header,
  // Return:
  bool *consumed)
{
  uint8_t ret;
  emberAfPushCallbackNetworkIndex();
  ret = emAfOverrideAppendSourceRouteCallback(destination, header, consumed);
  emAfOverrideAppendSourceRoute(destination, header, consumed);
  emberAfPopNetworkIndex();
  return ret;
}

// -----------------------------------------------------------------------------
// Allows the application to override the further index of an entry in source
// route table
void emberOverrideFurthurIndexForSourceRouteAddEntryHandler(
  EmberNodeId id,
  // Return:
  uint8_t *furtherIndex)
{
  emberAfPushCallbackNetworkIndex();
  emAfOverrideFurthurIndexForSourceRouteAddEntry(id, furtherIndex);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfStackStatusCallback
WEAK(void emberAfStackStatusCallback(
       // Stack status
       EmberStatus status))
{
  (void)status;
}

// A callback invoked when the status of the stack changes. If the status
// parameter equals EMBER_NETWORK_UP, then the getNetworkParameters command can
// be called to obtain the new network parameters. If any of the parameters are
// being stored in nonvolatile memory by the Host, the stored values should be
// updated.
void emberStackStatusHandler(
  // Stack status
  EmberStatus status)
{
  emberAfPushCallbackNetworkIndex();
  emAfStackStatus(status);
  emberAfStackStatusCallback(status);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfEnergyScanResultCallback
WEAK(void emberAfEnergyScanResultCallback(
       // The 802.15.4 channel number that was scanned.
       uint8_t channel,
       // The maximum RSSI value found on the channel.
       int8_t maxRssiValue))
{
  (void)channel;
  (void)maxRssiValue;
}

// Reports the result of an energy scan for a single channel. The scan is not
// complete until the scanCompleteHandler callback is called.
void emberEnergyScanResultHandler(
  // The 802.15.4 channel number that was scanned.
  uint8_t channel,
  // The maximum RSSI value found on the channel.
  int8_t maxRssiValue)
{
  emberAfPushCallbackNetworkIndex();
  emAfEnergyScanResult(channel, maxRssiValue);
  emberAfEnergyScanResultCallback(channel, maxRssiValue);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfNetworkFoundCallback
WEAK(void emberAfNetworkFoundCallback(
       // The parameters associated with the network found.
       EmberZigbeeNetwork *networkFound,
       // The link quality indication of the network found.
       uint8_t lqi,
       // The received signal strength indication of the network found.
       int8_t rssi))
{
  (void)networkFound;
  (void)lqi;
  (void)rssi;
}

// Reports that a network was found as a result of a prior call to startScan.
// Gives the network parameters useful for deciding which network to join.
void emberNetworkFoundHandler(
  // The parameters associated with the network found.
  EmberZigbeeNetwork *networkFound,
  // The link quality indication of the network found.
  uint8_t lqi,
  // The received signal strength indication of the network found.
  int8_t rssi)
{
  emberAfPushCallbackNetworkIndex();
  emAfNetworkFound(networkFound, lqi, rssi);
  emberAfNetworkFoundCallback(networkFound, lqi, rssi);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfScanCompleteCallback
WEAK(void emberAfScanCompleteCallback(
       // The channel on which the current error occurred. Undefined for the case
       // of EMBER_SUCCESS.
       uint8_t channel,
       // The error condition that occurred on the current channel. Value will be
       // EMBER_SUCCESS when the scan has completed.
       EmberStatus status))
{
  (void)channel;
  (void)status;
}

// Returns the status of the current scan of type EMBER_ENERGY_SCAN or
// EMBER_ACTIVE_SCAN. EMBER_SUCCESS signals that the scan has completed. Other
// error conditions signify a failure to scan on the channel specified.
void emberScanCompleteHandler(
  // The channel on which the current error occurred. Undefined for the case
  // of EMBER_SUCCESS.
  uint8_t channel,
  // The error condition that occurred on the current channel. Value will be
  // EMBER_SUCCESS when the scan has completed.
  EmberStatus status)
{
  emberAfPushCallbackNetworkIndex();
  emAfScanComplete(channel, status);
  emberAfScanCompleteCallback(channel, status);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfUnusedPanIdFoundCallback
WEAK(void emberAfUnusedPanIdFoundCallback(
       // The unused panID which has been found.
       EmberPanId panId,
       // The channel that the unused panID was found on.
       uint8_t channel))
{
  (void)panId;
  (void)channel;
}

// This function returns an unused panID and channel pair found via the find
// unused panId scan procedure.
void emberUnusedPanIdFoundHandler(
  // The unused panID which has been found.
  EmberPanId panId,
  // The channel that the unused panID was found on.
  uint8_t channel)
{
  emberAfPushCallbackNetworkIndex();
  emAfUnusedPanIdFound(panId, channel);
  emberAfUnusedPanIdFoundCallback(panId, channel);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfChildJoinCallback
WEAK(void emberAfChildJoinCallback(
       // The index of the child of interest.
       uint8_t index,
       // True if the child is joining. False the child is leaving.
       bool joining))
{
  (void)index;
  (void)joining;
}

// Indicates that a child has joined or left.
void emberChildJoinHandler(
  // The index of the child of interest.
  uint8_t index,
  // True if the child is joining. False the child is leaving.
  bool joining)
{
  emberAfPushCallbackNetworkIndex();
  emAfChildJoin(index, joining);
  emberAfChildJoinCallback(index, joining);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfDutyCycleCallback
WEAK(void emberAfDutyCycleCallback(
       // The channel page whose duty cycle state has changed.
       uint8_t channelPage,
       // The channel number whose duty cycle state has changed.
       uint8_t channel,
       // The current duty cycle state.
       EmberDutyCycleState state))
{
  (void)channelPage;
  (void)channel;
  (void)state;
}

// Callback fires when the duty cycle state has changed
void emberDutyCycleHandler(
  // The channel page whose duty cycle state has changed.
  uint8_t channelPage,
  // The channel number whose duty cycle state has changed.
  uint8_t channel,
  // The current duty cycle state.
  EmberDutyCycleState state)
{
  emberAfPushCallbackNetworkIndex();
  emAfDutyCycle(channelPage, channel, state);
  emberAfDutyCycleCallback(channelPage, channel, state);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfRemoteSetBindingCallback
WEAK(EmberZdoStatus emberAfRemoteSetBindingCallback(
       // Return: The contents of the binding entry.
       EmberBindingTableEntry *entry))
{
  (void)entry;
  return EMBER_ZDP_TABLE_FULL;
}

// Weak implementation of emAfRemoteSetBindingCallback
WEAK(EmberZdoStatus emAfRemoteSetBindingCallback(
       // Return: The contents of the binding entry.
       EmberBindingTableEntry *entry))
{
  (void)entry;
  return EMBER_ZDP_TABLE_FULL;
}

// The NCP used the external binding modification policy to decide how to handle
// a remote set binding request. The Host cannot change the current decision,
// but it can change the policy for future decisions using the setPolicy
// command.
// Return: ZDO response status.
EmberZdoStatus emberRemoteSetBindingHandler(
  // Return: The contents of the binding entry.
  EmberBindingTableEntry *entry)
{
  EmberZdoStatus ret;
  emberAfPushCallbackNetworkIndex();
  ret = emAfRemoteSetBindingCallback(entry);
  emAfRemoteSetBinding(entry);
  emberAfRemoteSetBindingCallback(entry);
  emberAfPopNetworkIndex();
  return ret;
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfRemoteDeleteBindingCallback
WEAK(EmberZdoStatus emberAfRemoteDeleteBindingCallback(
       // The index of the binding whose deletion was requested.
       uint8_t index))
{
  (void)index;
  return EMBER_ZDP_TABLE_FULL;
}

// Weak implementation of emAfRemoteDeleteBindingCallback
WEAK(EmberZdoStatus emAfRemoteDeleteBindingCallback(
       // The index of the binding whose deletion was requested.
       uint8_t index))
{
  (void)index;
  return EMBER_ZDP_TABLE_FULL;
}

// The NCP used the external binding modification policy to decide how to handle
// a remote delete binding request. The Host cannot change the current decision,
// but it can change the policy for future decisions using the setPolicy
// command.
// Return: ZDO response status
EmberZdoStatus emberRemoteDeleteBindingHandler(
  // The index of the binding whose deletion was requested.
  uint8_t index)
{
  EmberZdoStatus ret;
  emberAfPushCallbackNetworkIndex();
  ret = emAfRemoteDeleteBindingCallback(index);
  emAfRemoteDeleteBinding(index);
  emberAfRemoteDeleteBindingCallback(index);
  emberAfPopNetworkIndex();
  return ret;
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfPollCompleteCallback
WEAK(void emberAfPollCompleteCallback(
       // An EmberStatus value: EMBER_SUCCESS - Data was received in response to
       // the poll. EMBER_MAC_NO_DATA - No data was pending.
       // EMBER_DELIVERY_FAILED - The poll message could not be sent.
       // EMBER_MAC_NO_ACK_RECEIVED - The poll message was sent but not
       // acknowledged by the parent.
       EmberStatus status))
{
  (void)status;
}

// Indicates the result of a data poll to the parent of the local node.
void emberPollCompleteHandler(
  // An EmberStatus value: EMBER_SUCCESS - Data was received in response to
  // the poll. EMBER_MAC_NO_DATA - No data was pending.
  // EMBER_DELIVERY_FAILED - The poll message could not be sent.
  // EMBER_MAC_NO_ACK_RECEIVED - The poll message was sent but not
  // acknowledged by the parent.
  EmberStatus status)
{
  emberAfPushCallbackNetworkIndex();
  emAfPollComplete(status);
  emberAfPollCompleteCallback(status);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfPollCallback
WEAK(void emberAfPollCallback(
       // The node ID of the child that is requesting data.
       EmberNodeId childId,
       // True if transmit expected, false otherwise.
       bool transmitExpected))
{
  (void)childId;
  (void)transmitExpected;
}

// Indicates that the local node received a data poll from a child.
void emberPollHandler(
  // The node ID of the child that is requesting data.
  EmberNodeId childId,
  // True if transmit expected, false otherwise.
  bool transmitExpected)
{
  emberAfPushCallbackNetworkIndex();
  emAfPoll(childId, transmitExpected);
  emberAfPollCallback(childId, transmitExpected);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfDebugCallback
WEAK(void emberAfDebugCallback(
       // debug message
       EmberMessageBuffer message))
{
  (void)message;
}

// debugHandler
void emberDebugHandler(
  // debug message
  EmberMessageBuffer message)
{
  emberAfPushCallbackNetworkIndex();
  emAfDebug(message);
  emberAfDebugCallback(message);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback
//emberAfIncomingManyToOneRouteRequestCallback
WEAK(void emberAfIncomingManyToOneRouteRequestCallback(
       // The short id of the concentrator.
       EmberNodeId source,
       // The EUI64 of the concentrator.
       EmberEUI64 longId,
       // The path cost to the concentrator. The cost may decrease as additional
       // route request packets for this discovery arrive, but the callback is
       // made only once.
       uint8_t cost))
{
  (void)source;
  (void)longId;
  (void)cost;
}

// A callback indicating that a many-to-one route to the concentrator with the
// given short and long id is available for use.
void emberIncomingManyToOneRouteRequestHandler(
  // The short id of the concentrator.
  EmberNodeId source,
  // The EUI64 of the concentrator.
  EmberEUI64 longId,
  // The path cost to the concentrator. The cost may decrease as additional
  // route request packets for this discovery arrive, but the callback is
  // made only once.
  uint8_t cost)
{
  emberAfPushCallbackNetworkIndex();
  emAfIncomingManyToOneRouteRequest(source, longId, cost);
  emberAfIncomingManyToOneRouteRequestCallback(source, longId, cost);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfIncomingRouteErrorCallback
WEAK(void emberAfIncomingRouteErrorCallback(
       // EMBER_SOURCE_ROUTE_FAILURE or EMBER_MANY_TO_ONE_ROUTE_FAILURE.
       EmberStatus status,
       // The short id of the remote node.
       EmberNodeId target))
{
  (void)status;
  (void)target;
}

// A callback invoked when a route error message is received. The error
// indicates that a problem routing to or from the target node was encountered.
void emberIncomingRouteErrorHandler(
  // EMBER_SOURCE_ROUTE_FAILURE or EMBER_MANY_TO_ONE_ROUTE_FAILURE.
  EmberStatus status,
  // The short id of the remote node.
  EmberNodeId target)
{
  emberAfPushCallbackNetworkIndex();
  emAfIncomingRouteError(status, target);
  emberAfIncomingRouteErrorCallback(status, target);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfIncomingNetworkStatusCallback
WEAK(void emberAfIncomingNetworkStatusCallback(
       // One byte over-the-air error code from network status message
       uint8_t errorCode,
       // The short ID of the remote node
       EmberNodeId target))
{
  (void)errorCode;
  (void)target;
}

// A callback invoked when a network status/route error message is received. The
// error indicates that there was a problem sending/receiving messages from the
// target node
void emberIncomingNetworkStatusHandler(
  // One byte over-the-air error code from network status message
  uint8_t errorCode,
  // The short ID of the remote node
  EmberNodeId target)
{
  emberAfPushCallbackNetworkIndex();
  emAfIncomingNetworkStatus(errorCode, target);
  emberAfIncomingNetworkStatusCallback(errorCode, target);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfIncomingRouteRecordCallback
WEAK(void emberAfIncomingRouteRecordCallback(
       // The source of the route record.
       EmberNodeId source,
       // The EUI64 of the source.
       EmberEUI64 sourceEui,
       // The number of relays in relayList.
       uint8_t relayCount,
       // Header.
       EmberMessageBuffer header,
       // The relay List Index.
       uint8_t relayListIndex))
{
  (void)source;
  (void)sourceEui;
  (void)relayCount;
  (void)header;
  (void)relayListIndex;
}

// Reports the arrival of a route record command frame.
void emberIncomingRouteRecordHandler(
  // The source of the route record.
  EmberNodeId source,
  // The EUI64 of the source.
  EmberEUI64 sourceEui,
  // The number of relays in relayList.
  uint8_t relayCount,
  // Header.
  EmberMessageBuffer header,
  // The relay List Index.
  uint8_t relayListIndex)
{
  emberAfPushCallbackNetworkIndex();
  emAfIncomingRouteRecord(source, sourceEui, relayCount, header, relayListIndex);
  emberAfIncomingRouteRecordCallback(source, sourceEui, relayCount, header, relayListIndex);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfIdConflictCallback
WEAK(void emberAfIdConflictCallback(
       // The short id for which a conflict was detected
       EmberNodeId conflictingId))
{
  (void)conflictingId;
}

// A callback invoked by the EmberZNet stack when an id conflict is discovered,
// that is, two different nodes in the network were found to be using the same
// short id. The stack automatically removes the conflicting short id from its
// internal tables (address, binding, route, neighbor, and child tables). The
// application should discontinue any other use of the id.
void emberIdConflictHandler(
  // The short id for which a conflict was detected
  EmberNodeId conflictingId)
{
  emberAfPushCallbackNetworkIndex();
  emAfIdConflict(conflictingId);
  emberAfIdConflictCallback(conflictingId);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfMacPassthroughMessageCallback
WEAK(void emberAfMacPassthroughMessageCallback(
       // The type of MAC passthrough message received.
       EmberMacPassthroughType messageType,
       // The raw message that was received.
       EmberMessageBuffer message))
{
  (void)messageType;
  (void)message;
}

// A callback invoked by the EmberZNet stack when a MAC passthrough message is
// received.
void emberMacPassthroughMessageHandler(
  // The type of MAC passthrough message received.
  EmberMacPassthroughType messageType,
  // The raw message that was received.
  EmberMessageBuffer message)
{
  emberAfPushCallbackNetworkIndex();
  emAfMacPassthroughMessage(messageType, message);
  emberAfMacPassthroughMessageCallback(messageType, message);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfStackTokenChangedCallback
WEAK(void emberAfStackTokenChangedCallback(
       // The address of the stack token that has changed.
       uint16_t tokenAddress))
{
  (void)tokenAddress;
}

// A callback invoked to inform the application that a stack token has changed.
void emberStackTokenChangedHandler(
  // The address of the stack token that has changed.
  uint16_t tokenAddress)
{
  emberAfPushCallbackNetworkIndex();
  emAfStackTokenChanged(tokenAddress);
  emberAfStackTokenChangedCallback(tokenAddress);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfTimerCallback
WEAK(void emberAfTimerCallback(
       // Which timer generated the callback (0 or 1).
       uint8_t timerId))
{
  (void)timerId;
}

// A callback from the timer.
void emberTimerHandler(
  // Which timer generated the callback (0 or 1).
  uint8_t timerId)
{
  emberAfPushCallbackNetworkIndex();
  emAfTimer(timerId);
  emberAfTimerCallback(timerId);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfCounterRolloverCallback
WEAK(void emberAfCounterRolloverCallback(
       // Type of Counter
       EmberCounterType type))
{
  (void)type;
}

// This call is fired when a counter exceeds its threshold
void emberCounterRolloverHandler(
  // Type of Counter
  EmberCounterType type)
{
  emberAfPushCallbackNetworkIndex();
  emAfCounterRollover(type);
  emberAfCounterRolloverCallback(type);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfRawTransmitCompleteCallback
WEAK(void emberAfRawTransmitCompleteCallback(
       // message
       EmberMessageBuffer message,
       // EMBER_SUCCESS if the transmission was successful, or
       // EMBER_DELIVERY_FAILED if not
       EmberStatus status))
{
  (void)message;
  (void)status;
}

// A callback invoked by the EmberZNet stack when the MAC has finished
// transmitting a raw message.
void emberRawTransmitCompleteHandler(
  // message
  EmberMessageBuffer message,
  // EMBER_SUCCESS if the transmission was successful, or
  // EMBER_DELIVERY_FAILED if not
  EmberStatus status)
{
  emberAfPushCallbackNetworkIndex();
  emAfRawTransmitComplete(message, status);
  emberAfRawTransmitCompleteCallback(message, status);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfSwitchNetworkKeyCallback
WEAK(void emberAfSwitchNetworkKeyCallback(
       // The sequence number of the new network key.
       uint8_t sequenceNumber))
{
  (void)sequenceNumber;
}

// A callback to inform the application that the Network Key has been updated
// and the node has been switched over to use the new key. The actual key being
// used is not passed up, but the sequence number is.
void emberSwitchNetworkKeyHandler(
  // The sequence number of the new network key.
  uint8_t sequenceNumber)
{
  emberAfPushCallbackNetworkIndex();
  emAfSwitchNetworkKey(sequenceNumber);
  emberAfSwitchNetworkKeyCallback(sequenceNumber);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfZigbeeKeyEstablishmentCallback
WEAK(void emberAfZigbeeKeyEstablishmentCallback(
       // This is the IEEE address of the partner that the device successfully
       // established a key with. This value is all zeros on a failure.
       EmberEUI64 partner,
       // This is the status indicating what was established or why the key
       // establishment failed.
       EmberKeyStatus status))
{
  (void)partner;
  (void)status;
}

// This is a callback that indicates the success or failure of an attempt to
// establish a key with a partner device.
void emberZigbeeKeyEstablishmentHandler(
  // This is the IEEE address of the partner that the device successfully
  // established a key with. This value is all zeros on a failure.
  EmberEUI64 partner,
  // This is the status indicating what was established or why the key
  // establishment failed.
  EmberKeyStatus status)
{
  emberAfPushCallbackNetworkIndex();
  emAfZigbeeKeyEstablishment(partner, status);
  emberAfZigbeeKeyEstablishmentCallback(partner, status);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfGenerateCbkeKeysCallback
WEAK(void emberAfGenerateCbkeKeysCallback(
       // The result of the CBKE operation.
       EmberStatus status,
       // Return: The generated ephemeral public key.
       EmberPublicKeyData *ephemeralPublicKey))
{
  (void)status;
  (void)ephemeralPublicKey;
}

// A callback by the Crypto Engine indicating that a new ephemeral
// public/private key pair has been generated. The public/private key pair is
// stored on the NCP, but only the associated public key is returned to the
// host. The node's associated certificate is also returned.
void emberGenerateCbkeKeysHandler(
  // The result of the CBKE operation.
  EmberStatus status,
  // Return: The generated ephemeral public key.
  EmberPublicKeyData *ephemeralPublicKey)
{
  emberAfPushCallbackNetworkIndex();
  emAfGenerateCbkeKeys(status, ephemeralPublicKey);
  emberAfGenerateCbkeKeysCallback(status, ephemeralPublicKey);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfCalculateSmacsCallback
WEAK(void emberAfCalculateSmacsCallback(
       // The Result of the CBKE operation.
       EmberStatus status,
       // Return: The calculated value of the initiator's SMAC
       EmberSmacData *initiatorSmac,
       // Return: The calculated value of the responder's SMAC
       EmberSmacData *responderSmac))
{
  (void)status;
  (void)initiatorSmac;
  (void)responderSmac;
}

// A callback to indicate that the NCP has finished calculating the Secure
// Message Authentication Codes (SMAC) for both the initiator and responder. The
// associated link key is kept in temporary storage until the host tells the NCP
// to store or discard the key via emberClearTemporaryDataMaybeStoreLinkKey().
void emberCalculateSmacsHandler(
  // The Result of the CBKE operation.
  EmberStatus status,
  // Return: The calculated value of the initiator's SMAC
  EmberSmacData *initiatorSmac,
  // Return: The calculated value of the responder's SMAC
  EmberSmacData *responderSmac)
{
  emberAfPushCallbackNetworkIndex();
  emAfCalculateSmacs(status, initiatorSmac, responderSmac);
  emberAfCalculateSmacsCallback(status, initiatorSmac, responderSmac);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfDsaSignCallback
WEAK(void emberAfDsaSignCallback(
       // The result of the DSA signing operation.
       EmberStatus status,
       // The message and attached which includes the original message and the
       // appended signature.
       EmberMessageBuffer signedMessage))
{
  (void)status;
  (void)signedMessage;
}

// The handler that returns the results of the signing operation. On success,
// the signature will be appended to the original message (including the
// signature type indicator that replaced the startIndex field for the signing)
// and both are returned via this callback.
void emberDsaSignHandler(
  // The result of the DSA signing operation.
  EmberStatus status,
  // The message and attached which includes the original message and the
  // appended signature.
  EmberMessageBuffer signedMessage)
{
  emberAfPushCallbackNetworkIndex();
  emAfDsaSign(status, signedMessage);
  emberAfDsaSignCallback(status, signedMessage);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfDsaVerifyCallback
WEAK(void emberAfDsaVerifyCallback(
       // The result of the DSA verification operation.
       EmberStatus status))
{
  (void)status;
}

// This callback is executed by the stack when the DSA verification has
// completed and has a result. If the result is EMBER_SUCCESS, the signature is
// valid. If the result is EMBER_SIGNATURE_VERIFY_FAILURE then the signature is
// invalid. If the result is anything else then the signature verify operation
// failed and the validity is unknown.
void emberDsaVerifyHandler(
  // The result of the DSA verification operation.
  EmberStatus status)
{
  emberAfPushCallbackNetworkIndex();
  emAfDsaVerify(status);
  emberAfDsaVerifyCallback(status);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfIncomingBootloadMessageCallback
WEAK(void emberAfIncomingBootloadMessageCallback(
       // The EUI64 of the sending node.
       EmberEUI64 longId,
       // The bootload message that was sent.
       EmberMessageBuffer message))
{
  (void)longId;
  (void)message;
}

// A callback invoked by the EmberZNet stack when a bootload message is
// received.
void emberIncomingBootloadMessageHandler(
  // The EUI64 of the sending node.
  EmberEUI64 longId,
  // The bootload message that was sent.
  EmberMessageBuffer message)
{
  emberAfPushCallbackNetworkIndex();
  emAfIncomingBootloadMessage(longId, message);
  emberAfIncomingBootloadMessageCallback(longId, message);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback
//emberAfBootloadTransmitCompleteCallback
WEAK(void emberAfBootloadTransmitCompleteCallback(
       // The bootload message that was sent.
       EmberMessageBuffer message,
       // An EmberStatus value of EMBER_SUCCESS if an ACK was received from the
       // destination or EMBER_DELIVERY_FAILED if no ACK was received.
       EmberStatus status))
{
  (void)message;
  (void)status;
}

// A callback invoked by the EmberZNet stack when the MAC has finished
// transmitting a bootload message.
void emberBootloadTransmitCompleteHandler(
  // The bootload message that was sent.
  EmberMessageBuffer message,
  // An EmberStatus value of EMBER_SUCCESS if an ACK was received from the
  // destination or EMBER_DELIVERY_FAILED if no ACK was received.
  EmberStatus status)
{
  emberAfPushCallbackNetworkIndex();
  emAfBootloadTransmitComplete(message, status);
  emberAfBootloadTransmitCompleteCallback(message, status);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfZllNetworkFoundCallback
WEAK(void emberAfZllNetworkFoundCallback(
       // Return: Information about the network.
       const EmberZllNetwork *networkInfo,
       // Return: Device specific information.
       const EmberZllDeviceInfoRecord *deviceInfo))
{
  (void)networkInfo;
  (void)deviceInfo;
}

// This call is fired when a ZLL network scan finds a ZLL network.
void emberZllNetworkFoundHandler(
  // Return: Information about the network.
  const EmberZllNetwork *networkInfo,
  // Return: Device specific information.
  const EmberZllDeviceInfoRecord *deviceInfo)
{
  emberAfPushCallbackNetworkIndex();
  emAfZllNetworkFound(networkInfo, deviceInfo);
  emberAfZllNetworkFoundCallback(networkInfo, deviceInfo);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfZllScanCompleteCallback
WEAK(void emberAfZllScanCompleteCallback(
       // Status of the operation.
       EmberStatus status))
{
  (void)status;
}

// This call is fired when a ZLL network scan is complete.
void emberZllScanCompleteHandler(
  // Status of the operation.
  EmberStatus status)
{
  emberAfPushCallbackNetworkIndex();
  emAfZllScanComplete(status);
  emberAfZllScanCompleteCallback(status);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfZllAddressAssignmentCallback
WEAK(void emberAfZllAddressAssignmentCallback(
       // Return: Address assignment information.
       const EmberZllAddressAssignment *addressInfo))
{
  (void)addressInfo;
}

// This call is fired when network and group addresses are assigned to a remote
// mode in a network start or network join request.
void emberZllAddressAssignmentHandler(
  // Return: Address assignment information.
  const EmberZllAddressAssignment *addressInfo)
{
  emberAfPushCallbackNetworkIndex();
  emAfZllAddressAssignment(addressInfo);
  emberAfZllAddressAssignmentCallback(addressInfo);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfZllTouchLinkTargetCallback
WEAK(void emberAfZllTouchLinkTargetCallback(
       // Return: Information about the network.
       const EmberZllNetwork *networkInfo))
{
  (void)networkInfo;
}

// This call is fired when the device is a target of a touch link.
void emberZllTouchLinkTargetHandler(
  // Return: Information about the network.
  const EmberZllNetwork *networkInfo)
{
  emberAfPushCallbackNetworkIndex();
  emAfZllTouchLinkTarget(networkInfo);
  emberAfZllTouchLinkTargetCallback(networkInfo);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfMacFilterMatchMessageCallback
WEAK(void emberAfMacFilterMatchMessageCallback(
       // Return: macFilterMatchStruct.
       const EmberMacFilterMatchStruct *macFilterMatchStruct))
{
  (void)macFilterMatchStruct;
}

// A callback invoked by the EmberZNet stack when a raw MAC message that has
// matched one of the application's configured MAC filters.
void emberMacFilterMatchMessageHandler(
  // Return: macFilterMatchStruct.
  const EmberMacFilterMatchStruct *macFilterMatchStruct)
{
  emberAfPushCallbackNetworkIndex();
  emAfMacFilterMatchMessage(macFilterMatchStruct);
  emberAfMacFilterMatchMessageCallback(macFilterMatchStruct);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfDGpSentCallback
WEAK(void emberAfDGpSentCallback(
       // An EmberStatus value indicating success or the reason for failure.
       EmberStatus status,
       // The handle of the GPDF.
       uint8_t gpepHandle))
{
  (void)status;
  (void)gpepHandle;
}

// A callback to the GP endpoint to indicate the result of the GPDF
// transmission.
void emberDGpSentHandler(
  // An EmberStatus value indicating success or the reason for failure.
  EmberStatus status,
  // The handle of the GPDF.
  uint8_t gpepHandle)
{
  emberAfPushCallbackNetworkIndex();
  emAfDGpSent(status, gpepHandle);
  emberAfDGpSentCallback(status, gpepHandle);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfPanIdConflictCallback
WEAK(EmberStatus emberAfPanIdConflictCallback(
       // Number of conflict reports
       int8_t conflictCount))
{
  (void)conflictCount;
  return EMBER_LIBRARY_NOT_PRESENT;
}

// Weak implementation of emAfPanIdConflictCallback
WEAK(EmberStatus emAfPanIdConflictCallback(
       // Number of conflict reports
       int8_t conflictCount))
{
  (void)conflictCount;
  return EMBER_LIBRARY_NOT_PRESENT;
}

// This handler is called by the stack to report the number of conflict reports
// exceeds EMBER_PAN_ID_CONFLICT_REPORT_THRESHOLD within a period of 1 minute )
// Return: An EmberStatus value indicating success or the reason for failure.
EmberStatus emberPanIdConflictHandler(
  // Number of conflict reports
  int8_t conflictCount)
{
  EmberStatus ret;
  emberAfPushCallbackNetworkIndex();
  ret = emAfPanIdConflictCallback(conflictCount);
  emAfPanIdConflict(conflictCount);
  emberAfPanIdConflictCallback(conflictCount);
  emberAfPopNetworkIndex();
  return ret;
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfOrphanNotificationCallback
WEAK(void emberAfOrphanNotificationCallback(
       // The 8 byte EUI64 of the sender.
       EmberEUI64 longId))
{
  (void)longId;
}

// Inform the application that an orphan notification has been received. This is
// generally not useful for applications. It could be useful in testing and is
// included for this purpose.
void emberOrphanNotificationHandler(
  // The 8 byte EUI64 of the sender.
  EmberEUI64 longId)
{
  emberAfPushCallbackNetworkIndex();
  emAfOrphanNotification(longId);
  emberAfOrphanNotificationCallback(longId);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfCounterCallback
WEAK(void emberAfCounterCallback(
       // Type of Counter
       EmberCounterType type,
       // Counter Info and value
       EmberCounterInfo Info))
{
  (void)type;
  (void)Info;
}

// This handler is called when the value of a counter has changed
void emberCounterHandler(
  // Type of Counter
  EmberCounterType type,
  // Counter Info and value
  EmberCounterInfo Info)
{
  emberAfPushCallbackNetworkIndex();
  emAfCounter(type, Info);
  emberAfCounterCallback(type, Info);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfMacPassthroughFilterCallback
WEAK(bool emberAfMacPassthroughFilterCallback(
       // Return: Mac Header of the matched messgae
       uint8_t *macHeader))
{
  (void)macHeader;
  return false;
}

// Weak implementation of emAfMacPassthroughFilterCallback
WEAK(bool emAfMacPassthroughFilterCallback(
       // Return: Mac Header of the matched messgae
       uint8_t *macHeader))
{
  (void)macHeader;
  return false;
}

// This handler is called for messages that need to be matched against mac
// passthrough filter
// Return: True if mac passthrough filter was matched. False otherwise.
bool emberMacPassthroughFilterHandler(
  // Return: Mac Header of the matched messgae
  uint8_t *macHeader)
{
  bool ret;
  emberAfPushCallbackNetworkIndex();
  ret = emAfMacPassthroughFilterCallback(macHeader);
  emAfMacPassthroughFilter(macHeader);
  emberAfMacPassthroughFilterCallback(macHeader);
  emberAfPopNetworkIndex();
  return ret;
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback
//emberAfGenerateCbkeKeysHandler283k1Callback
WEAK(void emberAfGenerateCbkeKeysHandler283k1Callback(
       // The result of the CBKE operation.
       EmberStatus status,
       // Return: The generated ephemeral public key.
       EmberPublicKey283k1Data *ephemeralPublicKey))
{
  (void)status;
  (void)ephemeralPublicKey;
}

// A callback by the Crypto Engine indicating that a new ephemeral
// public/private key pair has been generated. The public/private key pair is
// stored on the NCP, but only the associated public key is returned to the
// host. The node's associated certificate is also returned.
void emberGenerateCbkeKeysHandler283k1(
  // The result of the CBKE operation.
  EmberStatus status,
  // Return: The generated ephemeral public key.
  EmberPublicKey283k1Data *ephemeralPublicKey)
{
  emberAfPushCallbackNetworkIndex();
  emAfGenerateCbkeKeysHandler283k1(status, ephemeralPublicKey);
  emberAfGenerateCbkeKeysHandler283k1Callback(status, ephemeralPublicKey);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback
//emberAfCalculateSmacsHandler283k1Callback
WEAK(void emberAfCalculateSmacsHandler283k1Callback(
       // The Result of the CBKE operation.
       EmberStatus status,
       // Return: The calculated value of the initiator's SMAC
       EmberSmacData *initiatorSmac,
       // Return: The calculated value of the responder's SMAC
       EmberSmacData *responderSmac))
{
  (void)status;
  (void)initiatorSmac;
  (void)responderSmac;
}

// A callback to indicate that the NCP has finished calculating the Secure
// Message Authentication Codes (SMAC) for both the initiator and responder. The
// associated link key is kept in temporary storage until the host tells the NCP
// to store or discard the key via emberClearTemporaryDataMaybeStoreLinkKey().
void emberCalculateSmacsHandler283k1(
  // The Result of the CBKE operation.
  EmberStatus status,
  // Return: The calculated value of the initiator's SMAC
  EmberSmacData *initiatorSmac,
  // Return: The calculated value of the responder's SMAC
  EmberSmacData *responderSmac)
{
  emberAfPushCallbackNetworkIndex();
  emAfCalculateSmacsHandler283k1(status, initiatorSmac, responderSmac);
  emberAfCalculateSmacsHandler283k1Callback(status, initiatorSmac, responderSmac);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfGpepIncomingMessageCallback
WEAK(void emberAfGpepIncomingMessageCallback(
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
       uint8_t *gpdCommandPayload))
{
  (void)status;
  (void)gpdLink;
  (void)sequenceNumber;
  (void)addr;
  (void)gpdfSecurityLevel;
  (void)gpdfSecurityKeyType;
  (void)autoCommissioning;
  (void)bidirectionalInfo;
  (void)gpdSecurityFrameCounter;
  (void)gpdCommandId;
  (void)mic;
  (void)proxyTableIndex;
  (void)gpdCommandPayloadLength;
  (void)gpdCommandPayload;
}

// A callback invoked by the ZigBee GP stack when a GPDF is received.
void emberGpepIncomingMessageHandler(
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
  uint8_t *gpdCommandPayload)
{
  emberAfPushCallbackNetworkIndex();
  emAfGpepIncomingMessage(status, gpdLink, sequenceNumber, addr, gpdfSecurityLevel, gpdfSecurityKeyType, autoCommissioning, bidirectionalInfo, gpdSecurityFrameCounter, gpdCommandId, mic, proxyTableIndex, gpdCommandPayloadLength, gpdCommandPayload);
  emberAfGpepIncomingMessageCallback(status, gpdLink, sequenceNumber, addr, gpdfSecurityLevel, gpdfSecurityKeyType, autoCommissioning, bidirectionalInfo, gpdSecurityFrameCounter, gpdCommandId, mic, proxyTableIndex, gpdCommandPayloadLength, gpdCommandPayload);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of emAfRtosIdleCallback
WEAK(bool emAfRtosIdleCallback(
       // Return: Idle time duration
       uint32_t *idleTimeMs))
{
  (void)idleTimeMs;
  return false;
}

// rtos Idle Handler
// Return: True or False.
bool emberRtosIdleHandler(
  // Return: Idle time duration
  uint32_t *idleTimeMs)
{
  bool ret;
  emberAfPushCallbackNetworkIndex();
  ret = emAfRtosIdleCallback(idleTimeMs);
  emAfRtosIdle(idleTimeMs);
  emberAfPopNetworkIndex();
  return ret;
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfRtosStackWakeupIsrCallback
WEAK(void emberAfRtosStackWakeupIsrCallback(void))
{
}

// Rtos Stack Wakeup Isr Handler
void emberRtosStackWakeupIsrHandler(void)
{
  emberAfPushCallbackNetworkIndex();
  emAfRtosStackWakeupIsr();
  emberAfRtosStackWakeupIsrCallback();
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfRadioNeedsCalibratingCallback
WEAK(void emberAfRadioNeedsCalibratingCallback(void))
{
}

// RadioNeedsCalibratingHandler
void emberRadioNeedsCalibratingHandler(void)
{
  emberAfPushCallbackNetworkIndex();
  emAfRadioNeedsCalibrating();
  emberAfRadioNeedsCalibratingCallback();
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// Weak implementation of public Callback emberAfScanErrorCallback
WEAK(void emberAfScanErrorCallback(
       // The error status of a scan
       EmberStatus status))
{
  (void)status;
}

// scanErrorHandler
void emberScanErrorHandler(
  // The error status of a scan
  EmberStatus status)
{
  emberAfPushCallbackNetworkIndex();
  emAfScanError(status);
  emberAfScanErrorCallback(status);
  emberAfPopNetworkIndex();
}
