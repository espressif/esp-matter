/***************************************************************************//**
 * @file
 * @brief The file names certain functions that run on host to have
 * the ezsp- prefix for functions instead of the ember- prefix.
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

//Utility Frames
#define emberGetLibraryStatus               ezspGetLibraryStatus
#define emberGetPhyInterfaceCount           ezspGetPhyInterfaceCount
#define emberGetTrueRandomEntropySource     ezspGetTrueRandomEntropySource

//Networking Frames
#define emberSetManufacturerCode            ezspSetManufacturerCode
#define emberSetPowerDescriptor             ezspSetPowerDescriptor
#define emberNetworkInit                    ezspNetworkInit
#define emberNetworkState                   ezspNetworkState
#define emberFormNetwork                    ezspFormNetwork
#define emberJoinNetwork                    ezspJoinNetwork
#define emberNetworkState                   ezspNetworkState
#define emberStartScan                      ezspStartScan
#define emberScanForUnusedPanId             ezspScanForUnusedPanId
#define emberStopScan                       ezspStopScan
#define emberFormNetwork                    ezspFormNetwork
#define emberJoinNetwork                    ezspJoinNetwork
#define emberLeaveNetwork                   ezspLeaveNetwork
#define emberPermitJoining                  ezspPermitJoining
#define emberEnergyScanRequest              ezspEnergyScanRequest
#define emberGetNodeId                      ezspGetNodeId
#define emberGetNeighbor                    ezspGetNeighbor
#define emberNeighborCount                  ezspNeighborCount
#define emberGetRouteTableEntry             ezspGetRouteTableEntry
#define emberSetRadioPower                  ezspSetRadioPower
#define emberSetRadioChannel                ezspSetRadioChannel
#define emberGetRadioChannel                ezspGetRadioChannel
#define emberSetRadioIeee802154CcaMode      ezspSetRadioIeee802154CcaMode
#define emberSetBrokenRouteErrorCode        ezspSetBrokenRouteErrorCode
#define emberGetNeighborFrameCounter        ezspGetNeighborFrameCounter
#define emberSetNeighborFrameCounter        ezspSetNeighborFrameCounter
#define emberGetChildData                   ezspGetChildData
#define emberSetChildData                   ezspSetChildData

#define emberSetConcentrator                ezspSetConcentrator
#define emberSetSourceRouteDiscoveryMode    ezspSetSourceRouteDiscoveryMode
// pro-compliance
#define emberConcentratorNoteRouteError     ezspConcentratorNoteRouteError
#define emberIncomingNetworkStatusHandler   ezspIncomingNetworkStatusHandler

#define emberConcentratorNoteDeliveryFailure  ezspConcentratorNoteDeliveryFailure

#define emberMultiPhyStart                  ezspMultiPhyStart
#define emberMultiPhyStop                   ezspMultiPhyStop
#define emberMultiPhySetRadioPower          ezspMultiPhySetRadioPower
#define emberMultiPhySetRadioChannel        ezspMultiPhySetRadioChannel
#define emberGetDutyCycleState              ezspGetDutyCycleState
#define emberGetDutyCycleLimits             ezspGetDutyCycleLimits
#define emberGetRadioParameters             ezspGetRadioParameters
#define emberSendLinkPowerDeltaRequest      ezspSendLinkPowerDeltaRequest

#define emberWriteNodeData                  ezspWriteNodeData
#define emberGetRoutingShortcutThreshold    ezspGetRoutingShortcutThreshold
#define emberSetRoutingShortcutThreshold    ezspSetRoutingShortcutThreshold

#define emberClearStoredBeacons             ezspClearStoredBeacons
#define emberGetFirstBeacon                 ezspGetFirstBeacon
#define emberGetNextBeacon                  ezspGetNextBeacon
#define emberGetNumStoredBeacons            ezspGetNumStoredBeacons
#define emberJoinNetworkDirectly            ezspJoinNetworkDirectly

//Binding Frames
#define emberClearBindingTable              ezspClearBindingTable
#define emberSetBinding                     ezspSetBinding
#define emberGetBinding                     ezspGetBinding
#define emberDeleteBinding                  ezspDeleteBinding
#define emberBindingIsActive                ezspBindingIsActive
#define emberGetBindingRemoteNodeId         ezspGetBindingRemoteNodeId
#define emberSetBindingRemoteNodeId         ezspSetBindingRemoteNodeId

//Messaging Frames
#define emberSendManyToOneRouteRequest      ezspSendManyToOneRouteRequest
#define emberAddressTableEntryIsActive      ezspAddressTableEntryIsActive
#define emberSetAddressTableRemoteEui64     ezspSetAddressTableRemoteEui64
#define emberSetAddressTableRemoteNodeId    ezspSetAddressTableRemoteNodeId
#define emberGetAddressTableRemoteEui64     ezspGetAddressTableRemoteEui64
#define emberGetAddressTableRemoteNodeId    ezspGetAddressTableRemoteNodeId
#define emberSetExtendedTimeout             ezspSetExtendedTimeout
#define emberGetExtendedTimeout             ezspGetExtendedTimeout
#define emberLookupNodeIdByEui64            ezspLookupNodeIdByEui64
#define emberLookupEui64ByNodeId            ezspLookupEui64ByNodeId
#define emberSetMacPollFailureWaitTime      ezspSetMacPollFailureWaitTime
#define emberGetBeaconClassificationParams  ezspGetBeaconClassificationParams
#define emberSetBeaconClassificationParams  ezspSetBeaconClassificationParams
#define emberSendPanIdUpdate                ezspSendPanIdUpdate

//Token Interface Frames
#define emberGetTokenCount                  ezspGetTokenCount
#define emberGetTokenInfo                   ezspGetTokenInfo
#define emberGetTokenData                   ezspGetTokenData
#define emberSetTokenData                   ezspSetTokenData
#define emberResetNode                      ezspResetNode

//Security Frames
#define emberSetInitialSecurityState        ezspSetInitialSecurityState
#define emberGetCurrentSecurityState        ezspGetCurrentSecurityState
#define emberGetKey                         ezspGetKey
#define emberGetKeyTableEntry               ezspGetKeyTableEntry
#define emberSetKeyTableEntry               ezspSetKeyTableEntry
#define emberFindKeyTableEntry              ezspFindKeyTableEntry
#define emberAddOrUpdateKeyTableEntry       ezspAddOrUpdateKeyTableEntry
#define emberEraseKeyTableEntry             ezspEraseKeyTableEntry
#define emberClearKeyTable                  ezspClearKeyTable
#define emberRequestLinkKey                 ezspRequestLinkKey
#define emberAddTransientLinkKey            ezspAddTransientLinkKey
#define emberClearTransientLinkKeys         ezspClearTransientLinkKeys
#define emberUpdateTcLinkKey                ezspUpdateTcLinkKey
#define emberSendTrustCenterLinkKey         ezspSendTrustCenterLinkKey

//ezspTrustCenter Frames
#define emberBroadcastNextNetworkKey        ezspBroadcastNextNetworkKey
#define emberBroadcastNetworkKeySwitch      ezspBroadcastNetworkKeySwitch
#define emberBecomeTrustCenter              ezspBecomeTrustCenter

#define emberUnicastCurrentNetworkKey       ezspUnicastCurrentNetworkKey

// Certificate Based Key Exchange (CBKE)
#define emberGenerateCbkeKeys               ezspGenerateCbkeKeys
#define emberGenerateCbkeKeys283k1          ezspGenerateCbkeKeys283k1
#define emberCalculateSmacs                 ezspCalculateSmacs
#define emberCalculateSmacs283k1            ezspCalculateSmacs283k1
#define emberGetCertificate                 ezspGetCertificate
#define emberGetCertificate283k1            ezspGetCertificate283k1
#define emberDsaVerify                      ezspDsaVerify
#define emberSetPreinstalledCbkeData        ezspSetPreinstalledCbkeData
#define emberSetPreinstalledCbkeData283k1   ezspSetPreinstalledCbkeData283k1
#define emberClearTemporaryDataMaybeStoreLinkKey      ezspClearTemporaryDataMaybeStoreLinkKey
#define emberClearTemporaryDataMaybeStoreLinkKey283k1 ezspClearTemporaryDataMaybeStoreLinkKey283k1

#define emberDGpSend ezspDGpSend
#define emberGpProxyTableProcessGpPairing ezspGpProxyTableProcessGpPairing
#define emberGpProxyTableGetEntry ezspGpProxyTableGetEntry
#define emberGpProxyTableLookup ezspGpProxyTableLookup

#define emberGpSinkTableGetEntry ezspGpSinkTableGetEntry
#define emberGpSinkTableLookup   ezspGpSinkTableLookup
#define emberGpSinkTableSetEntry ezspGpSinkTableSetEntry
#define emberGpSinkTableRemoveEntry ezspGpSinkTableRemoveEntry
#define emberGpSinkTableFindOrAllocateEntry ezspGpSinkTableFindOrAllocateEntry
#define emberGpSinkTableClearAll ezspGpSinkTableClearAll
#define emberGpSinkTableInit        ezspGpSinkTableInit
#define emberGpSinkTableSetSecurityFrameCounter ezspGpSinkTableSetSecurityFrameCounter

// WWAH frames
#define emberSetParentClassificationEnabled   ezspSetParentClassificationEnabled
#define emberSetLongUpTime                    ezspSetLongUpTime
#define emberSetHubConnectivity               ezspSetHubConnectivity
#define emberIsUpTimeLong                     ezspIsUpTimeLong
#define emberIsHubConnected                   ezspIsHubConnected

// ZLL frames
#define emberZllClearTokens ezspZllClearTokens
#define emberZllSetTokenStackZllData ezspZllSetDataToken
#define emberZllSetNonZllNetwork ezspZllSetNonZllNetwork
#define emberIsZllNetwork ezspIsZllNetwork
#define emberZllSetRadioIdleMode ezspZllSetRadioIdleMode
#define emberZllGetRadioIdleMode ezspZllGetRadioIdleMode
#define emberSetZllNodeType ezspSetZllNodeType
#define emberSetZllAdditionalState ezspSetZllAdditionalState
#define emberZllOperationInProgress ezspZllOperationInProgress
#define emberZllRxOnWhenIdleGetActive ezspZllRxOnWhenIdleGetActive
#define emberGetZllPrimaryChannelMask ezspGetZllPrimaryChannelMask
#define emberGetZllSecondaryChannelMask ezspGetZllSecondaryChannelMask
#define emberSetZllPrimaryChannelMask ezspSetZllPrimaryChannelMask
#define emberSetZllSecondaryChannelMask ezspSetZllSecondaryChannelMask
#define emberSetLogicalAndRadioChannel ezspSetLogicalAndRadioChannel
#define emberZllSetSecurityStateWithoutKey ezspZllSetSecurityStateWithoutKey
#define emberZllSetInitialSecurityState ezspZllSetInitialSecurityState
#define emberZllStartScan ezspZllStartScan
#define emberZllSetRxOnWhenIdle ezspZllSetRxOnWhenIdle
#define zllGetTokens ezspZllGetTokens
#define emberZllSetRxOnWhenIdle ezspZllSetRxOnWhenIdle
#define emberZllScanningComplete ezspZllScanningComplete

// Mirror SoC side macro in zll-api.h
#define emberZllCancelRxOnWhenIdle() ezspZllSetRxOnWhenIdle(0)
