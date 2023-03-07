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

#ifndef EMBER_MULTI_NETWORK_H
#define EMBER_MULTI_NETWORK_H

#include "stack/include/ember.h"  // to get correct EMBER_MULTI_NETWORK_STRIPPED symbol
#include "stack/mac/multi-mac.h"
#include "upper-mac.h"

// We can only support up to 4 networks mainly because we only had 2 bits
// available in the MacInfo struct and only 2 bits available in the EZSP frame
// control. In order to support more than 4 networks, we need to redesign these
// two systems.

// Network index stack size.
#define EMBER_MULTI_NETWORK_MAX_DEPTH 10
// This value gets pushed on the network index stack when the application calls
#define EMBER_NETWORK_INDEX_STACK_APP_MARKER 0xFE

// If a node is coordinator/router on one network, it delays polls by 100ms
// in order to allow the node to switch back to the "always on" network.
#define EMBER_MULTI_NETWORK_POLLING_DELAY 100 // msec

// The way we set the current logical network is the following:
// - We set the current network with the call emSetCurrentNetwork()
// - The code after this call will be referring to the logical network we just
//   set
// - We call emRestoreCurrentNetwork() to restore the previous logical network.
//   Note that the restore step is MANDATORY and the stack will assert in case
//   we forget to perform the restore.
//
// Follows an example:
// ...
// emSetCurrentNetwork(nwkIndex);
// ...
// code that is network-dependent
// ...
// emRestoreCurrentNetwork();

// Declared in hal/config/ember-configuration.c
extern uint8_t emDefaultStackProfile;

//------------------------------------------------------------------------------
// Common variables and functions. These are declared in both the regular and
// the stub libraries (in different flavors).
//------------------------------------------------------------------------------
extern uint8_t emSupportedNetworks;
extern uint8_t emZcAndZrCount;

extern const EmberLibraryStatus emMultiNetworkLibraryStatus;

void emNetworkDescriptorInit(void);
EmberStatus emMultinetworkRadioInit(void);
uint8_t emGetActiveAlwaysOnNetworkIndex(void);
bool emIsNetworkJoined(uint8_t nwkIndex);
bool emIsNetworkAlwaysOn(uint8_t nwkIndex);

//------------------------------------------------------------------------------
// For saving flash on certain platforms, e.g. EFR32xG1, EFR32xG14, multi-network
// code is stripped by default. We do not maintain the network info in the
// NetworkInfo struct (which would result in accessing every field by using the
// '->' operator). Instead, we declare variables for storing the info related to
// the (only) network.
//------------------------------------------------------------------------------
#if defined(EMBER_MULTI_NETWORK_STRIPPED)
#define emStackProfile (emDefaultStackProfile)
#define emParentId (sl_mac_upper_mac_state[0].nwk_radio_parameters[0].parent_node_id)
#define sli_parent_mac_index (sl_mac_upper_mac_state[0].nwk_radio_parameters[0].mac_index)
#define emParentEui64 (sl_mac_upper_mac_state[0].nwk_radio_parameters[0].parent_eui)
extern uint8_t emNodeType;
extern uint8_t emZigbeeState;
// Allow the capability byte to be updated at runtime
extern uint8_t emDynamicCapabilities;
extern uint32_t emSecurityStateBitmask;
extern uint8_t emZigbeeSequenceNumber;
extern uint8_t emApsSequenceNumber;
extern uint8_t emZigbeeNetworkSecurityLevel;
extern uint16_t emUnicastTxAttempts;
extern uint16_t emUnicastTxFailures;
extern uint8_t emSecurityKeySequenceNumber;
extern uint32_t emIncomingTcLinkKeyFrameCounter;
extern uint32_t emLastChildAgeTimeMs;
extern uint32_t emMsSinceLastPoll;
#define emStackRadioChannel (sl_mac_upper_mac_state[0].nwk_radio_parameters[0].channel)
#define emStackRadioChannelPage (sl_mac_upper_mac_state[0].nwk_radio_parameters[0].channel_page)
#define emStackRadioPower (sl_mac_upper_mac_state[0].nwk_radio_parameters[0].tx_power)
#define emLocalNodeId (sl_mac_upper_mac_state[0].nwk_radio_parameters[0].local_node_id)
#define emLocalPanId (sl_mac_upper_mac_state[0].nwk_radio_parameters[0].pan_id)
extern uint32_t emNextNwkFrameCounter;
#define emNeighborTable ((NeighborTableEntry*)emNeighborData)
#define emNeighborTableSize (emRouterNeighborTableSize)
extern uint8_t emNeighborCount;
#define emFrameCounters (emFrameCountersTable)
extern uint16_t parentNwkInformation;
#define emChildTable (emChildTableData)
#define emChildStatus (emChildStatusData)
#define emChildTimers (emChildTimersData)
#define emChildLqi (emChildLqiData)
#define emEndDeviceTimeout (emEndDeviceTimeoutData)
extern uint8_t emEndDeviceChildCount;
#define emRouteRecordTable (emRouteRecordTableData)
#define emBroadcastTable (emBroadcastTableData)
extern uint32_t emBroadcastAgeCutoffIndexes;
// Externed for zigbee test app.
extern uint8_t emBroadcastHead;
// This is zero if the time since bootup is longer than the broadcast
// table timeout.
extern uint8_t emInInitialBroadcastTimeout;
// Used for sending link status more quickly at startup and only used by routers.
extern uint8_t emFastLinkStatusCount;
// Association info
extern bool emPermitJoining;
extern bool emMacPermitAssociation;
extern bool emAllowRejoinsWithWellKnownKey;
extern uint8_t emParentAnnounceIndex;
extern uint8_t emTotalInitialChildren;
extern EmberPanId emNewPanId;
#else
//------------------------------------------------------------------------------
// On all the other platforms we declare the networkInfo array (which is
// actually allocated in ember-configuration.c
// In this case we can either include the regular library or the stub library.
//------------------------------------------------------------------------------
extern EmberNetworkInfo emNetworkDescriptor[];
extern EmPanInfo emPanInfoData[];
//extern EmberNetworkInfo* emCurrentNetwork;
extern uint8_t emCurrentNetworkIndex;
#define emCurrentNetwork (emNetworkDescriptor[emCurrentNetworkIndex])

#define emStackProfile (emCurrentNetwork.stackProfile)
#define emParentId (sl_mac_upper_mac_state[0].nwk_radio_parameters[emCurrentNetworkIndex].parent_node_id)
#define sli_parent_mac_index (sl_mac_upper_mac_state[0].nwk_radio_parameters[emCurrentNetworkIndex].mac_index)
#define emParentEui64 (sl_mac_upper_mac_state[0].nwk_radio_parameters[emCurrentNetworkIndex].parent_eui)
#define emNodeType (emCurrentNetwork.nodeType)
#define emZigbeeState (emCurrentNetwork.zigbeeState)
#define emDynamicCapabilities (emCurrentNetwork.dynamicCapabilities)
#define emSecurityStateBitmask (emCurrentNetwork.securityStateBitmask)
#define emZigbeeSequenceNumber (emCurrentNetwork.zigbeeSequenceNumber)
#define emApsSequenceNumber (emCurrentNetwork.apsSequenceNumber)
#define emZigbeeNetworkSecurityLevel (emCurrentNetwork.zigbeeNetworkSecurityLevel)
#define emUnicastTxAttempts (emCurrentNetwork.unicastTxAttempts)
#define emUnicastTxFailures (emCurrentNetwork.unicastTxFailures)
#define emSecurityKeySequenceNumber (emCurrentNetwork.securityKeySequenceNumber)
#define emIncomingTcLinkKeyFrameCounter (emCurrentNetwork.incomingTcLinkKeyFrameCounter)
#define emLastChildAgeTimeMs (emCurrentNetwork.lastChildAgeTimeMs)
#define emMsSinceLastPoll (emCurrentNetwork.msSinceLastPoll)
#define emStackRadioChannel (sl_mac_upper_mac_state[0].nwk_radio_parameters[emCurrentNetworkIndex].channel)
#define emStackRadioChannelPage (sl_mac_upper_mac_state[0].nwk_radio_parameters[emCurrentNetworkIndex].channel_page)
#define emStackRadioPower (sl_mac_upper_mac_state[0].nwk_radio_parameters[emCurrentNetworkIndex].tx_power)
#define emLocalNodeId (sl_mac_upper_mac_state[0].nwk_radio_parameters[emCurrentNetworkIndex].local_node_id)
#define emLocalPanId (sl_mac_upper_mac_state[0].nwk_radio_parameters[emCurrentNetworkIndex].pan_id)
#define emNextNwkFrameCounter (emCurrentNetwork.nextNwkFrameCounter)
#define emNeighborTable ((NeighborTableEntry*)emCurrentNetwork.neighborTable)
#define emNeighborTableSize (emCurrentNetwork.neighborTableSize)
#define emNeighborCount (emCurrentNetwork.neighborCount)
#define emFrameCounters (emCurrentNetwork.frameCounters)
#define parentNwkInformation (emCurrentNetwork.parentNwkInformation)
#define emChildTable (emCurrentNetwork.panInfoData->childTable)
#define emChildStatus (emCurrentNetwork.panInfoData->childStatus)
#define emChildTimers (emCurrentNetwork.panInfoData->childTimers)
#define emChildLqi (emCurrentNetwork.panInfoData->childLqi)
#define emEndDeviceTimeout (emCurrentNetwork.panInfoData->endDeviceTimeout)
#define emEndDeviceChildCount (emCurrentNetwork.panInfoData->endDeviceChildCount)
#define emRouteRecordTable (emCurrentNetwork.panInfoData->routeRecordTable)
#define emBroadcastTable (emCurrentNetwork.panInfoData->broadcastTable)
#define emBroadcastAgeCutoffIndexes (emCurrentNetwork.panInfoData->broadcastAgeCutoffIndexes)
#define emBroadcastHead (emCurrentNetwork.panInfoData->broadcastHead)
#define emInInitialBroadcastTimeout (emCurrentNetwork.panInfoData->inInitialBroadcastTimeout)
#define emFastLinkStatusCount (emCurrentNetwork.panInfoData->fastLinkStatusCount)
#define emPermitJoining (emCurrentNetwork.panInfoData->permitJoining)
#define emMacPermitAssociation  (emCurrentNetwork.panInfoData->macPermitAssociation)
#define emAllowRejoinsWithWellKnownKey  (emCurrentNetwork.panInfoData->allowRejoinsWithWellKnownKey)
#define emParentAnnounceIndex (emCurrentNetwork.panInfoData->parentAnnounceIndex)
#define emTotalInitialChildren  (emCurrentNetwork.panInfoData->totalInitialChildren)
#define emNewPanId (emCurrentNetwork.panInfoData->newPanId)
#endif // defined(EMBER_MULTI_NETWORK_STRIPPED)

//------------------------------------------------------------------------------
// Certain platforms are multi-network-stripped by default. In order
// to save flash on these platforms we #define to () the following functions.
//------------------------------------------------------------------------------
#if defined(EMBER_MULTI_NETWORK_STRIPPED)
#define emSetCurrentNetwork(n)
#define emRestoreCurrentNetwork()
#define emGetCurrentNetworkIndex() (0)
#define emEnableApplicationCurrentNetwork()
#define emIsCurrentNetworkStackEmpty() (true)
#define emAssociationInProgress() (false)
#define emActiveAlwaysOnNetworkFound() (false)
#define emSetZigbeeEventNetworkIndex(offset)
#define emGetZigbeeEventNetworkIndex(offset) (0)
#define emGetRadioNodeId() (emLocalNodeId)
#define emGetRadioPanId() (emLocalPanId)
#define emNoteNodeTypeChange(nodeType)
#define emScheduleNextPoll() sl_mac_request_poll(0, emGetCurrentNetworkIndex())
#else
#define emSetCurrentNetwork(n) (emSetCurrentNetworkInternal(n))
#define emRestoreCurrentNetwork() (emRestoreCurrentNetworkInternal())
uint8_t emGetCurrentNetworkIndex(void);
void emEnableApplicationCurrentNetwork(void);
bool emIsCurrentNetworkStackEmpty(void);
bool emAssociationInProgress(void);
bool emActiveAlwaysOnNetworkFound(void);
void emSetCurrentNetworkInternal(uint8_t nwkIndex);
void emRestoreCurrentNetworkInternal(void);
void emSetZigbeeEventNetworkIndex(uint8_t offset);
uint8_t emGetZigbeeEventNetworkIndex(uint8_t offset);
EmberNodeId emGetRadioNodeId(void);
EmberPanId emGetRadioPanId(void);
uint8_t emGetFirstActiveNetworkIndex(void);
bool emIsActiveCoordinatorOrRouterNetworkIndex(uint8_t index);
uint8_t emGetActiveCoordinatorOrRouterNetworkIndex(void);
void emNoteNodeTypeChange(EmberNodeType nodeType);
void emScheduleNextPoll(void);
#endif // defined(EMBER_MULTI_NETWORK_STRIPPED)

//------------------------------------------------------------------------------
// Many zigbee events require storing the current network index when we schedule
// the event and restore it later once the event expired. We also save the the
// nerwork index for other asynchronous processes, for instance the MAC shutdown
// mechanism.
//------------------------------------------------------------------------------

#define ZIGBEE_NWK_INDEX_OFFSET_BEACON_EVENT                      0
#define ZIGBEE_NWK_INDEX_OFFSET_ASSOCIATION_EVENT                 2
#define ZIGBEE_NWK_INDEX_OFFSET_DISASSOCIATION_MAC_SHUTDOWN       4
#define ZIGBEE_NWK_INDEX_OFFSET_LEAVE_EVENT                       6
#define ZIGBEE_NWK_INDEX_OFFSET_NEW_CHANNEL_EVENT                 8
#define ZIGBEE_NWK_INDEX_OFFSET_SCAN_EVENT                        10
#define ZIGBEE_NWK_INDEX_OFFSET_MANAGEMENT_EVENT                  12
#define ZIGBEE_NWK_INDEX_OFFSET_ZDO_CHANNEL_CHANGE_EVENT          14
#define ZIGBEE_NWK_INDEX_OFFSET_ENTITY_AUTHENTICATION_EVENT       16
#define ZIGBEE_NWK_INDEX_OFFSET_ZIGBEE_DEVICE_EVENT               18
#define ZIGBEE_NWK_INDEX_OFFSET_POLL_DELAYING_EVENT               20
#define ZIGBEE_NWK_INDEX_OFFSET_SUPER_RETRY_FOR_POLL_EVENT        22
#define ZIGBEE_NWK_INDEX_OFFSET_NETWORK_TIMEOUT_REQUEST_EVENT   26
#define ZIGBEE_NWK_INDEX_OFFSET_SEND_ORPHAN_NOTIFICATION_EVENT    28
#define ZIGBEE_NWK_INDEX_OFFSET_SEND_PARENT_ANNOUNCE_EVENT      30

//------------------------------------------------------------------------------
// Wrappers for application callbacks that are called in many places in the code.
//------------------------------------------------------------------------------
#if defined(EMBER_MULTI_NETWORK_STRIPPED)
#define emCallZigbeeKeyEstablishmentHandler(partner, status) \
  emberZigbeeKeyEstablishmentHandler(partner, status)
#define emCallStackStatusHandler(status) \
  emberStackStatusHandler(status)
#define emCallPollHandler(childId, jit) \
  emberPollHandler(childId, jit)
#define emCallPacketHandoffOutgoingHandler(packetType, packetBuffer, index, data) \
  emberPacketHandoffOutgoingHandler(packetType, packetBuffer, index, data)
#define emCallPacketHandoffIncomingHandler(packetType, packetBuffer, index, data) \
  emberPacketHandoffIncomingHandler(packetType, packetBuffer, index, data)
#else
// Wrapper for the emberZigbeeKeyEstablishmentHandler() callback.
void emCallZigbeeKeyEstablishmentHandler(EmberEUI64 partner,
                                         EmberKeyStatus status);
// Wrapper for the emberStackStatusHandler() callback.
void emCallStackStatusHandler(EmberStatus status);
// Wrapper for the emberPollHandler() callback
void emCallPollHandler(EmberNodeId childId, bool jit);

// Wrapper for the emberPacketHandoffOutgoingHandler() callback
EmberPacketAction emCallPacketHandoffOutgoingHandler(
  EmberZigbeePacketType packetType,
  EmberMessageBuffer packetBuffer,
  uint8_t index,
  // Return:
  void *data);

// Wrapper for the emberPacketHandoffIncomingHandler() callback
EmberPacketAction emCallPacketHandoffIncomingHandler(
  EmberZigbeePacketType packetType,
  EmberMessageBuffer packetBuffer,
  uint8_t index,
  // Return:
  void *data);
#endif // defined(EMBER_MULTI_NETWORK_STRIPPED)

// This is called also from the HAL so we need to provide an implementation
// for both stripped and unstripped versions of the stack.
/*
 * This function is here to support the direct calls from platform/base
 * (for a few counters that only need the data field).
 * The following function should never be called from the stack, within the
 * stack we only use emBuildAndSendCounterInfo
 */
void emCallCounterHandler(EmberCounterType type, uint8_t data);
void emBuildAndSendCounterInfo(EmberCounterType counter, EmberNodeId dst, uint8_t data);

// emBuildAndSendCounterInfo() is used to log all counter types within stack,
// including phyIndex and/or destination Id required counters. For phyIndex required
// counters, we get the phyIndex by destination/node id look up.
// Following internal function is used for the counters where destination
// Id is not applicable/valid but counter requires the phyIndex.
// For all other counters, call emBuildAndSendCounterInfo().
void emBuildAndSendCounterInfoWithPhyIndex(EmberCounterType counter,
                                           uint8_t phyIndex,
                                           uint8_t data);

//------------------------------------------------------------------------------

// This is a stub placed everywhere we used to call the current radio network
// handler to switch the configuration.  Things seem to work replacing it with
// nothing, but I wanted an easy way to add them back if necessary.
#define SET_RADIO_PARAMETERS()  if (sl_mac_lower_mac_is_idle(0)) { sli_upper_mac_update_lower_mac_params(0); }

#endif // EMBER_MULTI_NETWORK_H
