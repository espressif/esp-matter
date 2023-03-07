/***************************************************************************//**
 * @file
 * @brief Code for multi-network support.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifdef PLATFORM_HEADER
  #include PLATFORM_HEADER
#endif // PLATFORM_HEADER
#include "core/ember-multi-network.h"
#include "hal/hal.h"
#include "upper-mac.h"
#include "stack/include/ember-types-internal.h" // for PAN_ID_OKAY and NETWORK_INITIAL

extern sl_mac_tx_options_bitmask_t emMacPrepareTxHandler(PacketHeader packet,
                                                         uint8_t *flat_packet_buffer,
                                                         uint8_t mac_payload_offset,
                                                         uint8_t mac_index,
                                                         uint8_t nwk_index,
                                                         int8_t *tx_power);
extern bool emMacPassthroughHandler(uint8_t* macHeader, uint8_t macPayloadLength);
extern void emMacPacketSendCompleteCallback(uint8_t mac_index, sl_status_t status, PacketHeader packet, uint8_t tag);
extern bool emMakeStackJitMessage(void);
extern bool emPacketHandoffIncomingCallback(Buffer rawPacket, uint8_t index, void *data);
extern bool emProcessNetworkHeader(PacketHeader macHeaderOnly,
                                   const uint8_t* networkHeader);
extern void emNoteSuccessfulPollReceived(uint8_t childIndex, uint8_t nwk_index);
extern uint8_t emGetPhyInterfaceByNodeId(EmberNodeId nodeId);

const EmberLibraryStatus emMultiNetworkLibraryStatus = EMBER_LIBRARY_IS_STUB;

// A few internal targets without stripped need these dependencies brought in
#ifndef EMBER_MULTI_NETWORK_STRIPPED
extern EmNeighborTableEntry emNeighborData[];
extern uint8_t emRouterNeighborTableSize;
extern uint32_t emFrameCountersTable[];
#include "stack/core/multi-pan.h"
#endif // EMBER_MULTI_NETWORK_STRIPPED

//------------------------------------------------------------------------------
// EMBER_MULTI_NETWORK_STRIPPED is an optional #define
// that is used to conditionally compile out multi-network related source code.
// For flash-space constrained chips, we provide more flash savings by declaring
// a variable for each field in the NetworkInfo struct so that we avoid using
// the (->) operator everywhere in the code.
//------------------------------------------------------------------------------
#if defined(EMBER_MULTI_NETWORK_STRIPPED)
uint8_t emNodeType = EMBER_UNKNOWN_DEVICE;
uint8_t emZigbeeState = NETWORK_INITIAL;
uint8_t emDynamicCapabilities = 0;
uint32_t emSecurityStateBitmask = 0;
uint8_t emZigbeeSequenceNumber = 0;
uint8_t emApsSequenceNumber = 0;
uint8_t emZigbeeNetworkSecurityLevel = 0;
uint16_t emUnicastTxAttempts = 0;
uint16_t emUnicastTxFailures = 0;
uint8_t emSecurityKeySequenceNumber = 0;
uint32_t emIncomingTcLinkKeyFrameCounter = 0;
uint32_t emLastChildAgeTimeMs = 0;
uint32_t emMsSinceLastPoll = 0;
uint32_t emNextNwkFrameCounter = 0;
uint8_t emNeighborCount = 0;
uint8_t emEndDeviceChildCount = 0;
uint16_t parentNwkInformation = 0;
uint32_t emBroadcastAgeCutoffIndexes;
uint8_t emBroadcastHead = 0;
// This is zero if the time since bootup is longer than the broadcast
// table timeout.
uint8_t emInInitialBroadcastTimeout;
uint8_t emFastLinkStatusCount = 0;
bool emPermitJoining = false;
bool emMacPermitAssociation = false;
// A bool of whether we allow trust center (insecure) rejoins for devices
// using the well-known link key. Sending the network key encrypted with the
// well-known key is a security hole, so we ideally want to reject the TC
// rejoin. Allowing it provides backwards compatibility with R20 (HA) devices
// Setting this value to true is done through a setter, which arms a timer that,
// when fired, sets the variable back to false
// This variable corresponds to the allowRejoins attribute in the Zigbee spec
bool emAllowRejoinsWithWellKnownKey = false;
uint8_t emParentAnnounceIndex = 0;
// The number of children for the parent announce messages can change if children
// get deleted between consecutive parent Announce messages.
uint8_t emTotalInitialChildren = 0;
EmberPanId emNewPanId = PAN_ID_OKAY;
extern sl_mac_child_entry_t emChildTableData[];
extern uint16_t emChildStatusData[];
#else
uint8_t emCurrentNetworkIndex = 0;
#endif // defined(EMBER_MULTI_NETWORK_STRIPPED)

//------------------------------------------------------------------------------
// Network descriptor init function.
//------------------------------------------------------------------------------
#if defined(EMBER_MULTI_NETWORK_STRIPPED)
void emNetworkDescriptorInit(void)
{
  MEMSET(&emParentEui64, 0, sizeof(EmberEUI64));
  emZigbeeSequenceNumber = emberGetPseudoRandomNumber() & 0x00FF;
  emApsSequenceNumber = emberGetPseudoRandomNumber() & 0x00FF;
  emZigbeeNetworkSecurityLevel = emDefaultSecurityLevel;
}
#else
void emNetworkDescriptorInit(void)
{
  // Initialize the current network pointers to point to the 0-index network.
  emCurrentNetworkIndex = 0;

  MEMSET(&emCurrentNetwork, 0, sizeof(EmberNetworkInfo));
  emCurrentNetwork.stackProfile = emDefaultStackProfile;
  //emCurrentNetwork.parentId = EMBER_NULL_NODE_ID;
  emCurrentNetwork.nodeType = EMBER_UNKNOWN_DEVICE;
  emCurrentNetwork.zigbeeState = NETWORK_INITIAL;
  //emCurrentNetwork.radioChannel = EMBER_MIN_802_15_4_CHANNEL_NUMBER; // default to 802.15.4 ch 11
  //emCurrentNetwork.radioPower = MAX_RADIO_POWER;
  //emCurrentNetwork.localNodeId = EM_USE_LONG_ADDRESS;
  //emCurrentNetwork.localPanId = EM_BROADCAST_PAN_ID;
  //emCurrentNetwork.macDataSequenceNumber = emberGetPseudoRandomNumber() & 0x00FF;
  emCurrentNetwork.zigbeeSequenceNumber = emberGetPseudoRandomNumber() & 0x00FF;
  emCurrentNetwork.apsSequenceNumber = emberGetPseudoRandomNumber() & 0x00FF;
  emCurrentNetwork.zigbeeNetworkSecurityLevel = emDefaultSecurityLevel;
  emCurrentNetwork.neighborTable = emNeighborData;
  emCurrentNetwork.neighborTableSize = emRouterNeighborTableSize;
  emCurrentNetwork.frameCounters = emFrameCountersTable;
  emCurrentNetwork.panInfoData = &(emPanInfoData[emCurrentNetworkIndex]);

  emNetworkPanInfoDataInit(emCurrentNetworkIndex, &emCurrentNetwork);
  sli_mac_init_child_table_pointers(emCurrentNetworkIndex, emChildTable, emChildStatus);
}
#endif // defined(EMBER_MULTI_NETWORK_STRIPPED)

EmberStatus emMultinetworkRadioInit(void)
{
  sl_status_t status = EMBER_SUCCESS;
  sl_mac_radio_parameters_t radio_params;

#ifdef MAC_DUAL_PRESENT
  status = sl_mac_get_nwk_radio_parameters(PHY_INDEX_NATIVE, 0, &radio_params);
  if (status != SL_STATUS_OK) {
    return EMBER_ERR_FATAL;
  }

  // This function only sets the following fields. Other fields like channel
  // and tx_power are set by other functions.
  radio_params.prepare_tx_callback = emMacPrepareTxHandler;
  radio_params.passthrough_filter_callback = emMacPassthroughHandler;
  radio_params.poll_handler_callback = emberPollHandler;
  radio_params.indirect_tx_complete_callback = emMacPacketSendCompleteCallback;
  radio_params.make_jit_message_callback = emMakeStackJitMessage;
  radio_params.packet_handoff_incoming_callback = emPacketHandoffIncomingCallback;
  radio_params.process_network_header_callback = emProcessNetworkHeader;
  radio_params.poll_tx_complete_callback = emMacPacketSendCompleteCallback;
  radio_params.poll_rx_callback = emNoteSuccessfulPollReceived;

  status = sl_mac_set_nwk_radio_parameters(PHY_INDEX_NATIVE, 0, &radio_params);
  if (status != SL_STATUS_OK) {
#ifdef EMBER_TEST
    assert(0);
#endif // EMBER_TEST
    return EMBER_ERR_FATAL;
  }

  status = sl_mac_get_nwk_radio_parameters(PHY_INDEX_PRO2PLUS, 0, &radio_params);
  if (status != SL_STATUS_OK) {
    return EMBER_ERR_FATAL;
  }

  radio_params.prepare_tx_callback = emMacPrepareTxHandler;
  radio_params.passthrough_filter_callback = emMacPassthroughHandler;
  radio_params.poll_handler_callback = emberPollHandler;
  radio_params.indirect_tx_complete_callback = emMacPacketSendCompleteCallback;
  radio_params.make_jit_message_callback = emMakeStackJitMessage;
  radio_params.packet_handoff_incoming_callback = emPacketHandoffIncomingCallback;
  radio_params.process_network_header_callback = emProcessNetworkHeader;
  radio_params.poll_tx_complete_callback = emMacPacketSendCompleteCallback;
  radio_params.poll_rx_callback = emNoteSuccessfulPollReceived;

  status = sl_mac_set_nwk_radio_parameters(PHY_INDEX_PRO2PLUS, 0, &radio_params);
  if (status != SL_STATUS_OK) {
#ifdef EMBER_TEST
    assert(0);
#endif // EMBER_TEST
    return EMBER_ERR_FATAL;
  }
#else // MAC_DUAL_PRESENT
  status = sl_mac_get_nwk_radio_parameters(0, 0, &radio_params);
  if (status != SL_STATUS_OK) {
    return EMBER_ERR_FATAL;
  }

  // This function only sets the following fields. Other fields like channel
  // and tx_power are set by other functions.
  radio_params.prepare_tx_callback = emMacPrepareTxHandler;
  radio_params.passthrough_filter_callback = emMacPassthroughHandler;
  radio_params.poll_handler_callback = emberPollHandler;
  radio_params.indirect_tx_complete_callback = emMacPacketSendCompleteCallback;
  radio_params.make_jit_message_callback = emMakeStackJitMessage;
  radio_params.packet_handoff_incoming_callback = emPacketHandoffIncomingCallback;
  radio_params.process_network_header_callback = emProcessNetworkHeader;
  radio_params.poll_tx_complete_callback = emMacPacketSendCompleteCallback;
  radio_params.poll_rx_callback = emNoteSuccessfulPollReceived;

  status = sl_mac_set_nwk_radio_parameters(0, 0, &radio_params);
  if (status != SL_STATUS_OK) {
#ifdef EMBER_TEST
    assert(0);
#endif // EMBER_TEST
    return EMBER_ERR_FATAL;
  }
#endif // MAC_DUAL_PRESENT
  return status;
}

uint8_t emGetActiveAlwaysOnNetworkIndex(void)
{
  if (emIsNetworkAlwaysOn(0) && emIsNetworkJoined(0)) {
    return 0;
  }

  return EMBER_NULL_NETWORK_INDEX;
}

bool emIsNetworkJoined(uint8_t nwkIndex)
{
  return (emZigbeeState == NETWORK_JOINED
          || emZigbeeState == NETWORK_JOINED_UNAUTHENTICATED
          || emZigbeeState == NETWORK_REJOINED_UNAUTHENTICATED
          || emZigbeeState == NETWORK_JOINED_S2S_INITIATOR
          || emZigbeeState == NETWORK_JOINED_S2S_TARGET);
}

bool emIsNetworkAlwaysOn(uint8_t nwkIndex)
{
  return (emNodeType == EMBER_COORDINATOR
          || emNodeType == EMBER_ROUTER
          || emNodeType == EMBER_END_DEVICE);
}

// EMBER_MULTI_NETWORK_STRIPPED is an optional #define
// that is used on flash-space constrained chips
// to conditionally compile out multi-network related source code.
// These functions are #defined in ember-multi-network.h or not #defined at all.
//------------------------------------------------------------------------------
#if !defined(EMBER_MULTI_NETWORK_STRIPPED)
// Public multi-network APIs.
//------------------------------------------------------------------------------
uint8_t emberGetCurrentNetwork(void)
{
  return 0;
}
EmberStatus emberSetCurrentNetwork(uint8_t index)
{
  return EMBER_INVALID_CALL;
}
uint8_t emberGetCallbackNetwork(void)
{
  return 0;
}
void emSetZigbeeEventNetworkIndex(uint8_t offset)
{
}
uint8_t emGetZigbeeEventNetworkIndex(uint8_t offset)
{
  return 0;
}
bool emIsCurrentNetworkStackEmpty(void)
{
  return true;
}
void emEnableApplicationCurrentNetwork(void)
{
}
uint8_t emGetCurrentNetworkIndex(void)
{
  return 0;
}
void emSetCurrentNetworkInternal(uint8_t nwkIndex)
{
}
void emRestoreCurrentNetworkInternal(void)
{
}
bool emAssociationInProgress(void)
{
  return false;
}

bool emActiveAlwaysOnNetworkFound(void)
{
  return (emIsNetworkJoined(0) && emIsNetworkAlwaysOn(0));
}

uint8_t emGetFirstActiveNetworkIndex(void)
{
  if (emIsNetworkJoined(0)) {
    return 0;
  } else {
    return 0xFF;
  }
}

bool emIsActiveCoordinatorOrRouterNetworkIndex(uint8_t index)
{
  if (emIsNetworkJoined(0)
      && (emNodeType == EMBER_COORDINATOR
          || emNodeType == EMBER_ROUTER)
      && index == 0) {
    return true;
  }
  return false;
}

uint8_t emGetActiveCoordinatorOrRouterNetworkIndex(void)
{
  if (emIsNetworkJoined(0) && emNodeType <= EMBER_ROUTER) {
    return 0;
  } else {
    return EMBER_NULL_NETWORK_INDEX;
  }
}

// Polling stuff
//------------------------------------------------------------------------------

void emScheduleNextPoll(void)
{
  sl_mac_request_poll(0, 0);
}
// Tables allocation stuff
//------------------------------------------------------------------------------

extern void emResetNwkIncomingFrameCounters(void);

void emNoteNodeTypeChange(EmberNodeType nodeType)
{
  if (nodeType != EMBER_UNKNOWN_DEVICE) {
    emResetNwkIncomingFrameCounters();
  }
}

// Handlers stubs
//------------------------------------------------------------------------------
void emCallZigbeeKeyEstablishmentHandler(EmberEUI64 partner,
                                         EmberKeyStatus status)
{
  emberZigbeeKeyEstablishmentHandler(partner, status);
}

void emCallStackStatusHandler(EmberStatus status)
{
  emberStackStatusHandler(status);
}

EmberPacketAction emCallPacketHandoffIncomingHandler(
  EmberZigbeePacketType packetType,
  EmberMessageBuffer packetBuffer,
  uint8_t index,
  // Return:
  void *data)
{
  return emberPacketHandoffIncomingHandler(packetType,
                                           packetBuffer,
                                           index,
                                           data);
}

EmberPacketAction emCallPacketHandoffOutgoingHandler(
  EmberZigbeePacketType packetType,
  EmberMessageBuffer packetBuffer,
  uint8_t index,
  // Return:
  void *data)
{
  return emberPacketHandoffOutgoingHandler(packetType,
                                           packetBuffer,
                                           index,
                                           data);
}
#endif // !defined(EMBER_MULTI_NETWORK_STRIPPED)

// This is called also from the HAL so we need to provide an implementation
// for both stripped and non-stripped versions of the stack.
/*
 * This function is here to support the direct calls from platform/base
 * (for a few counters that only need the data field).
 * The following function should never be called from the stack, within the
 * stack
 * we only use emBuildAndSendCounterInfo
 */
void emCallCounterHandler(EmberCounterType type, uint8_t data)
{
  EmberCounterInfo info;
  info.data = data;
  emberCounterHandler(type, info);
}
void emBuildAndSendCounterInfo(EmberCounterType counter, EmberNodeId dst, uint8_t data)
{
  EmberCounterInfo info;
  EmberExtraCounterInfo other;
  uint8_t tmpIndex;

  info.data = data;
  bool requirePhyIndex = emberCounterRequiresPhyIndex(counter);
  bool requireDestination = emberCounterRequiresDestinationNodeId(counter);
  if (requirePhyIndex
      && !requireDestination) {
    tmpIndex  = emGetPhyInterfaceByNodeId(dst);
    // default to phy index 0 if dst is not found in neighbor or child table,
    // dst should always present for phyIndex required counters though.
    tmpIndex = (tmpIndex == 0xFF) ? PHY_INDEX_NATIVE : tmpIndex;
    info.otherFields = &tmpIndex;
  } else if (!requirePhyIndex
             && requireDestination) {
    info.otherFields = &dst;
  } else if (requirePhyIndex
             && requireDestination) {
    tmpIndex  = emGetPhyInterfaceByNodeId(dst);
    // default to phy index 0 if dst is not found in neighbor or child table,
    // dst should always present for phyIndex required counters though.
    tmpIndex = (tmpIndex == 0xFF) ? PHY_INDEX_NATIVE : tmpIndex;
    other.phy_index = tmpIndex;
    other.destinationNodeId = dst;
    info.otherFields = &other;
  }

  emberCounterHandler(counter, info);
}

// emBuildAndSendCounterInfo() is used to log all counter types within stack,
// including phyIndex and/or destination Id required counters. For phyIndex required
// counters, we get the phyIndex by destination/node id look up.
// Following internal function is used for the counters where destination
// Id is not applicable/valid but counter requires the phyIndex.
// For all other counters, call emBuildAndSendCounterInfo().
void emBuildAndSendCounterInfoWithPhyIndex(EmberCounterType counter,
                                           uint8_t phyIndex,
                                           uint8_t data)
{
  EmberCounterInfo info;
  bool requirePhyIndex = emberCounterRequiresPhyIndex(counter);
  bool requireDestination = emberCounterRequiresDestinationNodeId(counter);

  if (requirePhyIndex
      && !requireDestination) {
    info.data = data;
    info.otherFields = &phyIndex;
  } else {
    assert(0);  // wrong api! call emBuildAndSendCounterInfo build with correct counter info.
  }

  emberCounterHandler(counter, info);
}
//------------------------------------------------------------------------------
