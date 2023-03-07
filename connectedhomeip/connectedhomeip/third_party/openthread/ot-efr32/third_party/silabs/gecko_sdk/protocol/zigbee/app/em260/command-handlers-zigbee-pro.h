/***************************************************************************//**
 * @file
 * @brief Prototypes for ZigBee PRO command handler functions.
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

//------------------------------------------------------------------------------
// Ezsp Command Handlers

EmberStatus emberAfEzspFindAndRejoinNetworkCommandCallback(bool haveCurrentNetworkKey,
                                                           uint32_t channelMask);

EmberStatus emberAfEzspUnicastCurrentNetworkKeyCommandCallback(EmberNodeId taretShort,
                                                               EmberEUI64 targetLong,
                                                               EmberNodeId parentShortId);

uint32_t emberAfEzspSetSourceRouteDiscoveryModeCommandCallback(uint8_t mode);

void emberAfEzspGetEui64CommandCallback(EmberEUI64 eui64);

EmberStatus emberAfEzspGetNetworkParametersCommandCallback(EmberNodeType* nodeType,
                                                           EmberNetworkParameters* parameters);

uint8_t emberAfEzspGetParentChildParametersCommandCallback(EmberEUI64 parentEui64,
                                                           EmberNodeId* parentNodeId);

EmberStatus emberAfEzspSendUnicastCommandCallback(EmberOutgoingMessageType type,
                                                  EmberNodeId indexOrDestination,
                                                  EmberApsFrame* apsFrame,
                                                  uint8_t messageTag,
                                                  uint8_t messageLength,
                                                  uint8_t messageContents[],
                                                  uint8_t* sequence);

EmberStatus emberAfEzspSendBroadcastCommandCallback(EmberNodeId destination,
                                                    EmberApsFrame* apsFrame,
                                                    uint8_t radius,
                                                    uint8_t messageTag,
                                                    uint8_t messageLength,
                                                    uint8_t messageContents[],
                                                    uint8_t* sequence);

EmberStatus emberAfEzspProxyBroadcastCommandCallback(EmberNodeId source,
                                                     EmberNodeId destination,
                                                     uint8_t nwkSequence,
                                                     EmberApsFrame* apsFrame,
                                                     uint8_t radius,
                                                     uint8_t messageTag,
                                                     uint8_t messageLength,
                                                     uint8_t messageContents[],
                                                     uint8_t* apsSequence);

EmberStatus emberAfEzspSendMulticastCommandCallback(EmberApsFrame* apsFrame,
                                                    uint8_t hops,
                                                    uint8_t nonmemberRadius,
                                                    uint8_t messageTag,
                                                    uint8_t messageLength,
                                                    uint8_t messageContents[],
                                                    uint8_t* sequence);

EmberStatus emberAfEzspSendReplyCommandCallback(EmberNodeId sender,
                                                EmberApsFrame* apsFrame,
                                                uint8_t messageLength,
                                                uint8_t messageContents[]);

EmberStatus emberAfEzspPollForDataCommandCallback(uint16_t interval,
                                                  EmberEventUnits units,
                                                  uint8_t failureLimit);

EmberStatus emberAfEzspSetRoutingShortcutThresholdCommandCallback(uint8_t costThresh);
uint8_t emberAfEzspGetRoutingShortcutThresholdCommandCallback(void);

EmberStatus emberAfEzspGetSourceRouteTableEntryCommandCallback(uint8_t index,
                                                               EmberNodeId *destination,
                                                               uint8_t *closerIndex);

uint8_t emberAfEzspGetSourceRouteTableTotalSizeCommandCallback(void);
uint8_t emberAfEzspGetSourceRouteTableFilledSizeCommandCallback(void);

EmberStatus emberAfEzspReplaceAddressTableEntryCommandCallback(uint8_t addressTableIndex,
                                                               EmberEUI64 newEui64,
                                                               EmberNodeId newId,
                                                               bool newExtendedTimeout,
                                                               EmberEUI64 oldEui64,
                                                               EmberNodeId* oldId,
                                                               bool* oldExtendedTimeout);

EmberStatus emberAfEzspGetMulticastTableEntryCommandCallback(uint8_t index,
                                                             EmberMulticastTableEntry* value);

EmberStatus emberAfEzspSetMulticastTableEntryCommandCallback(uint8_t index,
                                                             EmberMulticastTableEntry* value);

EmberStatus emberAfEzspSendRawMessageCommandCallback(uint8_t messageLength,
                                                     uint8_t messageContents[]);

EmberStatus emberAfEzspSendRawMessageExtendedCommandCallback(uint8_t messageLength,
                                                             uint8_t messageContents[],
                                                             uint8_t priority,
                                                             bool useCca);

EmberStatus emberAfEzspSendMulticastWithAliasCommandCallback(EmberApsFrame* apsFrame,
                                                             uint8_t hops,
                                                             uint8_t nonmemberRadius,
                                                             uint16_t alias,
                                                             uint8_t nwkSequence,
                                                             uint8_t messageTag,
                                                             uint8_t messageLength,
                                                             uint8_t messageContents[],
                                                             uint8_t* sequence);

EmberStatus emberAfEzspSetConcentratorCommandCallback(bool on,
                                                      uint16_t concentratorType,
                                                      uint16_t minTime,
                                                      uint16_t maxTime,
                                                      uint8_t routeErrorThreshold,
                                                      uint8_t deliveryFailureThreshold,
                                                      uint8_t maxHops);

EmberStatus emberAfEzspGetCurrentDutyCycleCommandCallback(uint8_t maxDevices, uint8_t* perDeviceDutyCycles);

#ifndef UC_BUILD
void emberAfPluginEzspZigbeeProGetValueCommandCallback(EmberAfPluginEzspValueCommandContext* context);
void emberAfPluginEzspZigbeeProGetExtendedValueCommandCallback(EmberAfPluginEzspValueCommandContext* context);
void emberAfPluginEzspZigbeeProSetValueCommandCallback(EmberAfPluginEzspValueCommandContext* context);
void emberAfPluginEzspZigbeeProPolicyCommandCallback(EmberAfPluginEzspPolicyCommandContext* context);
void emberAfPluginEzspZigbeeProGetConfigurationValueCommandCallback(EmberAfPluginEzspConfigurationValueCommandContext* context);
void emberAfPluginEzspZigbeeProSetConfigurationValueCommandCallback(EmberAfPluginEzspConfigurationValueCommandContext* context);
#endif  // UC_BUILD
EmberStatus emberAfEzspSetBrokenRouteErrorCodeCommandCallback(uint8_t errorCode);

//ezsp command processing picks the wrong name
uint8_t emberMaximumApsPayloadLength(void);
#define emberMaximumPayloadLength emberMaximumApsPayloadLength
#define emberUnicastNwkKeyUpdate emberSendUnicastNetworkKeyUpdate
#define emberRemoveDevice emberSendRemoveDevice
//--------------------------------------------------------------------------
// Utilities

void writeLqiAndRssi(uint8_t *loc);
void emberAfPluginEzspGreenPowerConfigurationValueCommandCallback(EmberAfPluginEzspConfigurationValueCommandContext* context);

EmberStatus emberAfEzspSetSourceRouteCommandCallback(EmberNodeId destination,
                                                     uint8_t relayCount,
                                                     uint16_t* relayList);
