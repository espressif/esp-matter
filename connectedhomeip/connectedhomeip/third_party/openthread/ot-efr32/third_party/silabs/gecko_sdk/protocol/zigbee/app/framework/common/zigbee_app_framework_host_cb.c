/*****************************************************************************/
/**
 * Copyright 2020 Silicon Laboratories, Inc.
 *
 *****************************************************************************/
//
// EMZIGBEE-7387: modify "gen_stack_callback_dispatchers.py" to generate this file
// as well as zigbee_stack_callback_dispatcher jinja templates to support UC host
// *** Generated file. Do not edit! ***
//

#include PLATFORM_HEADER
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
// A callback invoked when receiving a message.
void ezspIncomingMessageHandler(
  // Incoming message type
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
  uint8_t *messageContents)
{
  emberAfPushCallbackNetworkIndex();
  emAfIncomingMessage(type, apsFrame, lastHopLqi, lastHopRssi, sender, bindingIndex, addressIndex, messageLength, messageContents);
  emberAfPopNetworkIndex();
}

// -----------------------------------------------------------------------------
// A callback invoked when a messageSend has been tried by the stack.
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
  uint8_t *messageContents)
{
  emberAfPushCallbackNetworkIndex();
  emAfMessageSent(type, indexOrDestination, apsFrame, messageTag, status, messageLength, messageContents);
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
void ezspZigbeeKeyEstablishmentHandler(
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
  uint8_t *gpdCommandPayload)
{
  emberAfPushCallbackNetworkIndex();
  emAfGpepIncomingMessage(status, gpdLink, sequenceNumber, addr, gpdfSecurityLevel, gpdfSecurityKeyType, autoCommissioning, bidirectionalInfo, gpdSecurityFrameCounter, gpdCommandId, mic, proxyTableIndex, gpdCommandPayloadLength, gpdCommandPayload);
  emberAfGpepIncomingMessageCallback(status, gpdLink, sequenceNumber, addr, gpdfSecurityLevel, gpdfSecurityKeyType, autoCommissioning, bidirectionalInfo, gpdSecurityFrameCounter, gpdCommandId, mic, proxyTableIndex, gpdCommandPayloadLength, gpdCommandPayload);
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
void ezspDGpSentHandler(
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
// Weak implementation of public Callback emberAfStackStatusCallback
WEAK(void emberAfStackStatusCallback(
       // Stack status. One of the following: EMBER_NETWORK_UP,
       // EMBER_NETWORK_DOWN, EMBER_JOIN_FAILED, EMBER_MOVE_FAILED
       EmberStatus status))
{
  (void)status;
}

// A callback invoked when the status of the stack changes. If the status
// parameter equals EMBER_NETWORK_UP, then the getNetworkParameters command can
// be called to obtain the new network parameters. If any of the parameters are
// being stored in nonvolatile memory by the Host, the stored values should be
// updated.
void ezspStackStatusHandler(
  // Stack status. One of the following: EMBER_NETWORK_UP,
  // EMBER_NETWORK_DOWN, EMBER_JOIN_FAILED, EMBER_MOVE_FAILED
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
void ezspEnergyScanResultHandler(
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
void ezspNetworkFoundHandler(
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
void ezspScanCompleteHandler(
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
void ezspUnusedPanIdFoundHandler(
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
void ezspSwitchNetworkKeyHandler(
  // The sequence number of the new network key.
  uint8_t sequenceNumber)
{
  emberAfPushCallbackNetworkIndex();
  emAfSwitchNetworkKey(sequenceNumber);
  emberAfSwitchNetworkKeyCallback(sequenceNumber);
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
  EmberNodeType childType)
{
  emberAfPushCallbackNetworkIndex();
  emAfChildJoin(index, joining, childId, childEui64, childType);
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
  EmberPerDeviceDutyCycle *arrayOfDeviceDutyCycles)
{
  emberAfPushCallbackNetworkIndex();
  emAfDutyCycle(channelPage, channel, state, totalDevices, arrayOfDeviceDutyCycles);
  emberAfDutyCycleCallback(channelPage, channel, state);
  emberAfPopNetworkIndex();
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
void ezspPollCompleteHandler(
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
