/*
 * Copyright (c) 2020, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 */

#ifndef _GPHCI_STRUCTURES_H_
#define _GPHCI_STRUCTURES_H_

/*****************************************************************************
 *                    Includes Definition
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
*                    Type Definitions
*****************************************************************************/

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    gpHci_Result_t                           reason;
} gpHci_DisconnectCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_ReadRemoteVersionInfoCommand_t;

typedef struct {
    gpHci_EventMask_t                        eventMask;
} gpHci_SetEventMaskCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_ResetCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_ReadConnectionAcceptTimeoutCommand_t;

typedef struct {
    UInt16                                   connectionAcceptTimeoutUnit;
} gpHci_WriteConnectionAcceptTimeoutCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt8                                    type;
} gpHci_ReadTransmitPowerLevelCommand_t;

typedef struct {
    UInt8                                    DataPathDirection;
    UInt8                                    DataPathID;
    UInt8                                    VendorSpecificConfigLength;
    UInt8*                                   VendorSpecificConfig;
} gpHci_ConfigureDataPathCommand_t;

typedef struct {
    UInt16                                   hostAclDataLength;
    UInt8                                    hostSyncDataLength;
    UInt16                                   hostTotalNumAclPackets;
    UInt16                                   hostTotalNumSyncPackets;
} gpHci_HostBufferSizeCommand_t;

typedef struct {
    gpHci_EventMask_t                        eventMask;
} gpHci_SetEventMaskPage2Command_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_ReadAuthenticatedPayloadTOCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt16                                   authenticatedPayloadTO;
} gpHci_WriteAuthenticatedPayloadTOCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_ReadLocalVersionInformationCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_ReadLocalSupportedCommandsCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_ReadLocalSupportedFeaturesCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_ReadBufferSizeCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_ReadBdAddrCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_ReadLocalSupportedCodecsCommand_t;

typedef struct {
    gpHci_CodecId_t                          CodecId;
    UInt8                                    LogicalTransportType;
    UInt8                                    Direction;
} gpHci_ReadLocalSupportedCodecCapabilitiesCommand_t;

typedef struct {
    gpHci_CodecId_t                          CodecId;
    UInt8                                    LogicalTransportType;
    UInt8                                    Direction;
    UInt8                                    CodecConfigurationLength;
    UInt8*                                   CodecConfiguration;
} gpHci_ReadLocalSupportedControllerDelayCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_ReadRSSICommand_t;

typedef struct {
    gpHci_EventMask_t                        eventMask;
} gpHci_LeSetEventMaskCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadBufferSizeCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadLocalSupportedFeaturesCommand_t;

typedef struct {
    BtDeviceAddress_t                        address;
} gpHci_LeSetRandomAddressCommand_t;

typedef struct {
    UInt16                                   advertisingIntervalMin;
    UInt16                                   advertisingIntervalMax;
    gpHci_AdvertisingType_t                  advertisingType;
    gpHci_OwnAddressType_t                   ownAddressType;
    gpHci_AdvPeerAddressType_t               peerAddressType;
    BtDeviceAddress_t                        peerAddress;
    UInt8                                    channelMap;
    gpHci_AdvFilterPolicy_t                  filterPolicy;
} gpHci_LeSetAdvertisingParametersCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadAdvertisingChannelTxPowerCommand_t;

typedef struct {
    UInt8                                    length;
    UInt8*                                   data;
} gpHci_LeSetAdvertisingDataCommand_t;

typedef struct {
    UInt8                                    length;
    UInt8*                                   data;
} gpHci_LeSetScanResponseDataCommand_t;

typedef struct {
    Bool                                     enable;
} gpHci_LeSetAdvertiseEnableCommand_t;

typedef struct {
    gpHci_ScanType_t                         scanType;
    UInt16                                   scanInterval;
    UInt16                                   scanWindow;
    gpHci_OwnAddressType_t                   ownAddressType;
    gpHci_ScanFilterPolicy_t                 filterPolicy;
} gpHci_LeSetScanParametersCommand_t;

typedef struct {
    Bool                                     enable;
    Bool                                     filterDuplicates;
} gpHci_LeSetScanEnableCommand_t;

typedef struct {
    UInt16                                   scanInterval;
    UInt16                                   scanWindow;
    gpHci_InitiatorFilterPolicy_t            filterPolicy;
    gpHci_InitPeerAddressType_t              peerAddressType;
    BtDeviceAddress_t                        peerAddress;
    gpHci_OwnAddressType_t                   ownAddressType;
    UInt16                                   connIntervalMin;
    UInt16                                   connIntervalMax;
    UInt16                                   connLatency;
    UInt16                                   supervisionTimeout;
    UInt16                                   minCELength;
    UInt16                                   maxCELength;
} gpHci_LeCreateConnectionCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeCreateConnectionCancelCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadWhiteListSizeCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeClearWhiteListCommand_t;

typedef struct {
    gpHci_WhitelistAddressType_t             addressType;
    BtDeviceAddress_t                        address;
} gpHci_LeAddDeviceToWhiteListCommand_t;

typedef struct {
    gpHci_WhitelistAddressType_t             addressType;
    BtDeviceAddress_t                        address;
} gpHci_LeRemoveDeviceFromWhiteListCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt16                                   connIntervalMin;
    UInt16                                   connIntervalMax;
    UInt16                                   connLatency;
    UInt16                                   supervisionTimeout;
    UInt16                                   minCELength;
    UInt16                                   maxCELength;
} gpHci_LeConnectionUpdateCommand_t;

typedef struct {
    gpHci_ChannelMap_t                       channels;
} gpHci_LeSetHostChannelClassificationCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_LeReadChannelMapCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_LeReadRemoteFeaturesCommand_t;

typedef struct {
    UInt8*                                   key;
    UInt8*                                   data;
} gpHci_LeEncryptCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeRandCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt8*                                   randomNumber;
    UInt16                                   encryptedDiversifier;
    UInt8*                                   longTermKey;
} gpHci_LeEnableEncryptionCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt8*                                   longTermKey;
} gpHci_LeLongTermKeyRequestReplyCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_LeLongTermKeyRequestNegativeReplyCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadSupportedStatesCommand_t;

typedef struct {
    UInt8                                    rxchannel;
} gpHci_LeReceiverTestCommand_t;

typedef struct {
    UInt8                                    txchannel;
    UInt8                                    length;
    UInt8                                    payload;
} gpHci_LeTransmitterTestCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeTestEndCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt16                                   connIntervalMin;
    UInt16                                   connIntervalMax;
    UInt16                                   connLatency;
    UInt16                                   supervisionTimeout;
    UInt16                                   minCELength;
    UInt16                                   maxCELength;
} gpHci_LeRemoteConnectionParamRequestReplyCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt8                                    reason;
} gpHci_LeRemoteConnectionParamRequestNegReplyCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt16                                   txOctets;
    UInt16                                   txTime;
} gpHci_LeSetDataLengthCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadSuggestedDefDataLengthCommand_t;

typedef struct {
    UInt16                                   suggestedMaxTxOctets;
    UInt16                                   suggestedMaxTxTime;
} gpHci_LeWriteSuggestedDefDataLengthCommand_t;

typedef struct {
    gpHci_AdvPeerAddressType_t               peerIdentityAddressType;
    BtDeviceAddress_t                        peerIdentityAddress;
    gpHci_IRK_t                              peerIRK;
    gpHci_IRK_t                              localIRK;
} gpHci_LeAddDeviceToResolvingListCommand_t;

typedef struct {
    gpHci_AdvPeerAddressType_t               peerIdentityAddressType;
    BtDeviceAddress_t                        peerIdentityAddress;
} gpHci_LeRemoveDeviceFromResolvingListCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeClearResolvingListCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadResolvingListSizeCommand_t;

typedef struct {
    UInt8                                    enable;
} gpHci_LeSetAddressResolutionEnableCommand_t;

typedef struct {
    UInt16                                   rpa_timeout;
} gpHci_LeSetResolvablePrivateAddressTimeoutCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadMaxDataLengthCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_LeReadPhyCommand_t;

typedef struct {
    gpHci_PhyDirectionMask_t                 allPhys;
    gpHci_PhyMask_t                          txPhys;
    gpHci_PhyMask_t                          rxPhys;
} gpHci_LeSetDefaultPhyCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt8                                    allPhys;
    gpHci_PhyMask_t                          txPhys;
    gpHci_PhyMask_t                          rxPhys;
    gpHci_PhyOptions_t                       phyOptions;
} gpHci_LeSetPhyCommand_t;

typedef struct {
    UInt8                                    rxchannel;
    gpHci_Phy_t                              phy;
    gpHci_ModulationIndex_t                  modulationIndex;
} gpHci_LeEnhancedReceiverTestCommand_t;

typedef struct {
    UInt8                                    txchannel;
    UInt8                                    length;
    UInt8                                    payload;
    gpHci_PhyWithCoding_t                    phy;
} gpHci_LeEnhancedTransmitterTestCommand_t;

typedef struct {
    UInt8                                    advertisingHandle;
    BtDeviceAddress_t                        address;
} gpHci_LeSetAdvertisingSetRandomAddressCommand_t;

typedef struct {
    UInt8                                    advertisingHandle;
    UInt16                                   advertisingEventProperties;
    UInt24                                   primaryAdvertisingIntervalMin;
    UInt24                                   primaryAdvertisingIntervalMax;
    UInt8                                    primaryAdvertisingChannelMap;
    gpHci_OwnAddressType_t                   ownAddressType;
    gpHci_AdvPeerAddressType_t               peerAddressType;
    BtDeviceAddress_t                        peerAddress;
    gpHci_AdvFilterPolicy_t                  filterPolicy;
    Int8                                     txPower;
    gpHci_Phy_t                              primaryAdvertisingPhy;
    UInt8                                    secondaryAdvertisingMaxSkip;
    gpHci_Phy_t                              secondaryAdvertisingPhy;
    UInt8                                    advertisingSID;
    UInt8                                    scanRequestNotificationEnable;
} gpHci_LeSetExtendedAdvertisingParametersCommand_t;

typedef struct {
    gpHci_SetExtendedAdvScanRspData_t        advertisingData;
} gpHci_LeSetExtendedAdvertisingDataCommand_t;

typedef struct {
    gpHci_SetExtendedAdvScanRspData_t        scanResponseData;
} gpHci_LeSetExtendedScanResponseDataCommand_t;

typedef struct {
    UInt8                                    enable;
    UInt8                                    numberOfSets;
    gpHci_ExtendedAdvEnableData_t*           enableData;
} gpHci_LeSetExtendedAdvertisingEnableCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadMaximumAdvertisingDataLengthCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadNumberSupportedAdvertisingSetsCommand_t;

typedef struct {
    UInt8                                    advertisingHandle;
} gpHci_LeRemoveAdvertisingSetCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeClearAdvertisingSetsCommand_t;

typedef struct {
    UInt8                                    advertisingHandle;
    UInt16                                   periodicAdvertisingIntervalMin;
    UInt16                                   periodicAdvertisingIntervalMax;
    UInt16                                   periodicAdvertisingProperties;
} gpHci_LeSetPeriodicAdvertisingParametersCommand_t;

typedef struct {
    UInt8                                    advertisingHandle;
    UInt8                                    operation;
    UInt8                                    advDataLength;
    UInt8*                                   advData;
} gpHci_LeSetPeriodicAdvertisingDataCommand_t;

typedef struct {
    UInt8                                    enable;
    UInt8                                    advertisingHandle;
} gpHci_LeSetPeriodicAdvertisingEnableCommand_t;

typedef struct {
    gpHci_OwnAddressType_t                   ownAddressType;
    gpHci_ScanFilterPolicy_t                 filterPolicy;
    gpHci_PhyMask_t                          scanningPhys;
    gpHci_SetExtendedScanData_t*             data;
} gpHci_LeSetExtendedScanParametersCommand_t;

typedef struct {
    Bool                                     enable;
    gpHci_ExtScanFilterDuplicates_t          filterDuplicates;
    UInt16                                   duration;
    UInt16                                   period;
} gpHci_LeSetExtendedScanEnableCommand_t;

typedef struct {
    gpHci_InitiatorFilterPolicy_t            filterPolicy;
    gpHci_OwnAddressType_t                   ownAddressType;
    gpHci_InitPeerAddressType_t              peerAddressType;
    BtDeviceAddress_t                        peerAddress;
    gpHci_PhyMask_t                          initiatingPhys;
    gpHci_ExtendedCreateConnectionData_t*    data;
} gpHci_LeExtendedCreateConnectionCommand_t;

typedef struct {
    gpHci_SyncOptions_t                      options;
    UInt8                                    advertisingSID;
    gpHci_AdvPeerAddressType_t               advertisingAddressType;
    BtDeviceAddress_t                        advertiserAddress;
    UInt16                                   skip;
    UInt16                                   syncTimeout;
    gpHci_SyncCteMask_t                      syncCteType;
} gpHci_LePeriodicAdvertisingCreateSyncCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LePeriodicAdvertisingCreateSyncCancelCommand_t;

typedef struct {
    UInt16                                   syncHandle;
} gpHci_LePeriodicAdvertisingTerminateSyncCommand_t;

typedef struct {
    gpHci_AdvPeerAddressType_t               advertiserAddressType;
    BtDeviceAddress_t                        advertiserAddress;
    UInt8                                    advertisingSID;
} gpHci_LeAddDeviceToPeriodicAdvertiserListCommand_t;

typedef struct {
    gpHci_AdvPeerAddressType_t               advertiserAddressType;
    BtDeviceAddress_t                        advertiserAddress;
    UInt8                                    advertisingSID;
} gpHci_LeRemoveDeviceFromPeriodicAdvertiserListCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeClearPeriodicAdvertiserListCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadPeriodicAdvertiserListSizeCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadTransmitPowerCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadRfPathCompensationCommand_t;

typedef struct {
    Int16                                    RF_Tx_Path_Compensation_Value;
    Int16                                    RF_Rx_Path_Compensation_Value;
} gpHci_LeWriteRfPathCompensationCommand_t;

typedef struct {
    gpHci_AdvPeerAddressType_t               peerIdentityAddressType;
    BtDeviceAddress_t                        peerIdentityAddress;
    UInt8                                    privacyMode;
} gpHci_LeSetPrivacyModeCommand_t;

typedef struct {
    UInt8                                    rxchannel;
    gpHci_Phy_t                              phy;
    gpHci_ModulationIndex_t                  modulationIndex;
    UInt8                                    expectedCteLengthUnit;
    gpHci_CteType_t                          expectedCteType;
    UInt8                                    expectedSlotDurations;
    UInt8                                    switchingPatternLength;
    UInt8*                                   antennaIDs;
} gpHci_LeReceiverTest_v3Command_t;

typedef struct {
    UInt8                                    txchannel;
    UInt8                                    length;
    UInt8                                    payload;
    gpHci_PhyWithCoding_t                    phy;
    UInt8                                    expectedCteLengthUnit;
    gpHci_CteType_t                          expectedCteType;
    UInt8                                    switchingPatternLength;
    UInt8*                                   antennaIDs;
} gpHci_LeTransmitterTest_v3Command_t;

typedef struct {
    UInt8                                    advertisingHandle;
    UInt8                                    CteLength;
    UInt8                                    CteTypes;
    UInt8                                    CteCount;
    UInt8                                    lengthOfSwitchingPattern;
    UInt8*                                   antennaIDs;
} gpHci_LeSetConnectionlessCteTransmitParametersCommand_t;

typedef struct {
    UInt8                                    advertisingHandle;
    Bool                                     Enable;
} gpHci_LeSetConnectionlessCteTransmitEnableCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    Bool                                     samplingEnable;
    UInt8                                    slotDurations;
    UInt8                                    lengthOfSwitchingPattern;
    UInt8*                                   antennaIDs;
} gpHci_LeSetConnectionCteReceiveParametersCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt8                                    CteTypes;
    UInt8                                    lengthOfSwitchingPattern;
    UInt8*                                   antennaIDs;
} gpHci_LeSetConnectionCteTransmitParametersCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    Bool                                     Enable;
    UInt16                                   cteRequestInterval;
    UInt8                                    requestedCteLength;
    UInt8                                    requestedCteType;
} gpHci_LeConnectionCteRequestEnableCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    Bool                                     Enable;
} gpHci_LeConnectionCteResponseEnableCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadAntennaInformationCommand_t;

typedef struct {
    gpHci_SyncHandle_t                       syncHandle;
    Bool                                     enable;
} gpHci_LeSetPeriodicAdvertisingReceiveEnableCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt16                                   serviceData;
    gpHci_SyncHandle_t                       syncHandle;
} gpHci_LePeriodicAdvertisingSyncTransferCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt16                                   serviceData;
    gpHci_AdvertisingHandle_t                advertisingHandle;
} gpHci_LePeriodicAdvertisingSetInfoTransferCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    gpHci_PastSyncMode_t                     mode;
    UInt16                                   skip;
    UInt16                                   syncTimeout;
    gpHci_SyncCteMask_t                      cteType;
} gpHci_LeSetPeriodicAdvertisingSyncTransferParametersCommand_t;

typedef struct {
    gpHci_PastSyncMode_t                     mode;
    UInt16                                   skip;
    UInt16                                   syncTimeout;
    gpHci_SyncCteMask_t                      cteType;
} gpHci_LeSetDefaultPeriodicAdvertisingSyncTransferParametersCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_LeReadBufferSize_v2Command_t;

typedef struct {
    gpHci_CigId_t                            cigId;
    UInt24                                   sduIntervalCtoP;
    UInt24                                   sduIntervalPtoC;
    gpHci_ClockAccuracy_t                    worstCaseSca;
    gpHci_IsoPacking_t                       packing;
    gpHci_IsoFraming_t                       framing;
    UInt16                                   maxTransportLatencyCtoP;
    UInt16                                   maxTransportLatencyPtoC;
    UInt8                                    cisCount;
    gpHci_CigParamsCisData_t*                cisData;
} gpHci_LeSetCigParametersCommand_t;

typedef struct {
    gpHci_CigId_t                            cigId;
    UInt24                                   sduIntervalCtoP;
    UInt24                                   sduIntervalPtoC;
    UInt8                                    ftCtoP;
    UInt8                                    ftPtoC;
    UInt16                                   isoInterval;
    gpHci_ClockAccuracy_t                    worstCaseSca;
    gpHci_IsoPacking_t                       packing;
    gpHci_IsoFraming_t                       framing;
    UInt8                                    cisCount;
    gpHci_CigParamsTestCisData_t*            cisData;
} gpHci_LeSetCigParametersTestCommand_t;

typedef struct {
    UInt8                                    cisCount;
    gpHci_CreateCisData_t*                   cisData;
} gpHci_LeCreateCisCommand_t;

typedef struct {
    gpHci_CigId_t                            cigId;
} gpHci_LeRemoveCigCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_LeAcceptCisRequestCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    gpHci_Result_t                           reason;
} gpHci_LeRejectCisRequestCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    gpHci_DataPathDirection_t                DataPathDirection;
    UInt8                                    DataPathId;
    gpHci_CodecId_t                          CodecId;
    gpHci_ControllerDelay_t                  ControllerDelay;
    UInt8                                    CodecConfigurationLength;
    UInt8*                                   CodecConfiguration;
} gpHci_LeSetupIsoDataPathCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt8                                    DataPathDirection;
} gpHci_LeRemoveIsoDataPathCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_LeReadIsoLinkQualityCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_LeReadIsoTxSyncCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    gpHci_PayloadType_t                      PayloadType;
} gpHci_LeIsoTxTestCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    gpHci_PayloadType_t                      PayloadType;
} gpHci_LeIsoRxTestCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_LeIsoReadTestCountersCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
} gpHci_LeIsoTestEndCommand_t;

typedef struct {
    UInt8                                    bitNumber;
    UInt8                                    bitValue;
} gpHci_LeSetHostFeatureCommand_t;

typedef struct {
    BtDeviceAddress_t                        address;
} gpHci_VsdWriteDeviceAddressCommand_t;

typedef struct {
    Bool                                     codedPhy;
} gpHci_VsdGenerateAccessAddressCommand_t;

typedef struct {
    gpHci_TLVType_t                          type;
    UInt16                                   length;
    UInt8*                                   value;
} gpHci_SetVsdTestParamsCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connHandle;
    UInt16                                   nrOfPackets;
    UInt8                                    sizeOfPackets;
    gpHci_VsdDataPumpPayloadType_t           payloadType;
    UInt8                                    payloadStartByte;
} gpHci_VsdSetDataPumpParametersCommand_t;

typedef struct {
    Bool                                     enable;
} gpHci_VsdSetDataPumpEnableCommand_t;

typedef struct {
    gpHci_VsdSinkMode_t                      mode;
} gpHci_VsdSetNullSinkEnableCommand_t;

typedef struct {
    UInt32                                   AccessCode;
} gpHci_VsdSetAccessCodeCommand_t;

typedef struct {
    Bool                                     validity;
    UInt8                                    validation_n;
    UInt8                                    fake_preamble_present;
    UInt8                                    fake_preamble_n;
    UInt16                                   validation_threshold;
} gpHci_VsdSetAccessCodeValidationParametersCommand_t;

typedef struct {
    Int8                                     transmitPower;
} gpHci_VsdSetTransmitPowerCommand_t;

typedef struct {
    gpHci_SleepMode_t                        mode;
    Bool                                     enable;
} gpHci_VsdSetSleepCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connHandle;
    Bool                                     disabled;
} gpHci_VsdDisableSlaveLatencyCommand_t;

typedef struct {
    UInt8                                    enable;
} gpHci_VsdEnCBByDefaultCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connHandle;
    UInt16                                   maxRxOctetsRemote;
    UInt16                                   maxRxTimeRemote;
} gpHci_VsdOverruleRemoteMaxRxOctetsAndTimeCommand_t;

typedef struct {
    UInt32                                   prand;
    Bool                                     enableTimeout;
} gpHci_VsdSetRpaPrandCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_VsdGetBuildP4ChangelistCommand_t;

typedef struct {
    gpHci_CoexParams_t                       adv_tx_rx;
} gpHci_VsdSetAdvertisingCoexParamsCommand_t;

typedef struct {
    gpHci_CoexParams_t                       scan_rx_wd;
    gpHci_CoexParams_t                       scan_rx;
    gpHci_CoexParams_t                       scan_tx;
} gpHci_VsdSetScanCoexParamsCommand_t;

typedef struct {
    gpHci_CoexParams_t                       init_rx_wd;
    gpHci_CoexParams_t                       init_rx;
    gpHci_CoexParams_t                       init_tx;
} gpHci_VsdSetInitCoexParamsCommand_t;

typedef struct {
    gpHci_AdvPeerAddressType_t               peerIdentityAddressType;
    BtDeviceAddress_t                        peerIdentityAddress;
} gpHci_VsdGeneratePeerResolvableAddressCommand_t;

typedef struct {
    gpHci_AdvPeerAddressType_t               peerIdentityAddressType;
    BtDeviceAddress_t                        peerIdentityAddress;
} gpHci_VsdGenerateLocalResolvableAddressCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    gpHci_PhyMask_t                          phys;
    UInt8                                    count;
} gpHci_VsdSetMinUsedChannelsCommand_t;

typedef struct {
    gpHci_CoexParams_t                       slave_conn;
} gpHci_VsdSetConnSlaCoexParamsCommand_t;

typedef struct {
    gpHci_CoexParams_t                       master_conn;
} gpHci_VsdSetConnMasCoexParamsCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    Bool                                     Enable;
    UInt16                                   cteTxInterval;
    UInt8                                    cteLength;
    UInt8                                    cteType;
} gpHci_VsdUnsolicitedCteTxEnableCommand_t;

typedef struct {
    UInt8                                    RNG_Source;
    UInt8                                    numRandomBytes;
} gpHci_VsdRNGCommand_t;

typedef struct {
    gpHci_VsdSubEventCode_t                  eventCode;
} gpHci_SetLeMetaVSDEventCommand_t;

typedef struct {
    gpHci_VsdSubEventCode_t                  eventCode;
} gpHci_ResetLeMetaVSDEventCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt8                                    triggerAfterNrRetries;
    UInt8                                    restoreAfterNrRetries;
    gpHci_CoexParams_t                       alternate_coex_settings;
} gpHci_VsdBleCoexAlternatePriorityCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_VsdReadResolvingListCurrentSizeCommand_t;

typedef struct {
    UInt8                                    maxSize;
} gpHci_VsdSetResolvingListMaxSizeCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    gpHci_CoexParams_t                       slave_conn;
} gpHci_VsdSetConnSlaCoexUpdateParamsCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    gpHci_CoexParams_t                       master_conn;
} gpHci_VsdSetConnMasCoexUpdateParamsCommand_t;

typedef struct {
    gpHci_ConnectionHandle_t                 connectionHandle;
    UInt8                                    exponentialBase;
} gpHci_VsdSetExponentialBaseCommand_t;

typedef struct {
    UInt16                                   distanceUs;
} gpHci_VsdSetMinimalSubeventDistanceCommand_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_VsdGetRtMgrVersionCommand_t;

typedef struct {
    UInt8                                    rxchannel;
    gpHci_PhyMask_t                          phyMask;
    gpHci_ModulationIndex_t                  modulationIndex;
    UInt32                                   accesscode;
    UInt8                                    antenna;
} gpHci_VsdEnhancedReceiverTestCommand_t;

typedef struct {
    UInt8                                    rxchannel;
    gpHci_PhyMask_t                          phyMask;
    gpHci_ModulationIndex_t                  modulationIndex;
    UInt8                                    expectedCteLengthUnit;
    gpHci_CteType_t                          expectedCteType;
    UInt8                                    expectedSlotDurations;
    UInt8                                    switchingPatternLength;
    UInt8*                                   antennaIDs;
    UInt32                                   accesscode;
    UInt8                                    antenna;
} gpHci_VsdLeReceiverTest_v3Command_t;

typedef struct {
    char                                     _unused_dummy;
} gpHci_UnknownOpCodeCommand_t;


typedef union {
    gpHci_DisconnectCommand_t                                              Disconnect;
    gpHci_ReadRemoteVersionInfoCommand_t                                   ReadRemoteVersionInfo;
    gpHci_SetEventMaskCommand_t                                            SetEventMask;
    gpHci_ResetCommand_t                                                   Reset;
    gpHci_ReadConnectionAcceptTimeoutCommand_t                             ReadConnectionAcceptTimeout;
    gpHci_WriteConnectionAcceptTimeoutCommand_t                            WriteConnectionAcceptTimeout;
    gpHci_ReadTransmitPowerLevelCommand_t                                  ReadTransmitPowerLevel;
    gpHci_ConfigureDataPathCommand_t                                       ConfigureDataPath;
    gpHci_HostBufferSizeCommand_t                                          HostBufferSize;
    gpHci_SetEventMaskPage2Command_t                                       SetEventMaskPage2;
    gpHci_ReadAuthenticatedPayloadTOCommand_t                              ReadAuthenticatedPayloadTO;
    gpHci_WriteAuthenticatedPayloadTOCommand_t                             WriteAuthenticatedPayloadTO;
    gpHci_ReadLocalVersionInformationCommand_t                             ReadLocalVersionInformation;
    gpHci_ReadLocalSupportedCommandsCommand_t                              ReadLocalSupportedCommands;
    gpHci_ReadLocalSupportedFeaturesCommand_t                              ReadLocalSupportedFeatures;
    gpHci_ReadBufferSizeCommand_t                                          ReadBufferSize;
    gpHci_ReadBdAddrCommand_t                                              ReadBdAddr;
    gpHci_ReadLocalSupportedCodecsCommand_t                                ReadLocalSupportedCodecs;
    gpHci_ReadLocalSupportedCodecCapabilitiesCommand_t                     ReadLocalSupportedCodecCapabilities;
    gpHci_ReadLocalSupportedControllerDelayCommand_t                       ReadLocalSupportedControllerDelay;
    gpHci_ReadRSSICommand_t                                                ReadRSSI;
    gpHci_LeSetEventMaskCommand_t                                          LeSetEventMask;
    gpHci_LeReadBufferSizeCommand_t                                        LeReadBufferSize;
    gpHci_LeReadLocalSupportedFeaturesCommand_t                            LeReadLocalSupportedFeatures;
    gpHci_LeSetRandomAddressCommand_t                                      LeSetRandomAddress;
    gpHci_LeSetAdvertisingParametersCommand_t                              LeSetAdvertisingParameters;
    gpHci_LeReadAdvertisingChannelTxPowerCommand_t                         LeReadAdvertisingChannelTxPower;
    gpHci_LeSetAdvertisingDataCommand_t                                    LeSetAdvertisingData;
    gpHci_LeSetScanResponseDataCommand_t                                   LeSetScanResponseData;
    gpHci_LeSetAdvertiseEnableCommand_t                                    LeSetAdvertiseEnable;
    gpHci_LeSetScanParametersCommand_t                                     LeSetScanParameters;
    gpHci_LeSetScanEnableCommand_t                                         LeSetScanEnable;
    gpHci_LeCreateConnectionCommand_t                                      LeCreateConnection;
    gpHci_LeCreateConnectionCancelCommand_t                                LeCreateConnectionCancel;
    gpHci_LeReadWhiteListSizeCommand_t                                     LeReadWhiteListSize;
    gpHci_LeClearWhiteListCommand_t                                        LeClearWhiteList;
    gpHci_LeAddDeviceToWhiteListCommand_t                                  LeAddDeviceToWhiteList;
    gpHci_LeRemoveDeviceFromWhiteListCommand_t                             LeRemoveDeviceFromWhiteList;
    gpHci_LeConnectionUpdateCommand_t                                      LeConnectionUpdate;
    gpHci_LeSetHostChannelClassificationCommand_t                          LeSetHostChannelClassification;
    gpHci_LeReadChannelMapCommand_t                                        LeReadChannelMap;
    gpHci_LeReadRemoteFeaturesCommand_t                                    LeReadRemoteFeatures;
    gpHci_LeEncryptCommand_t                                               LeEncrypt;
    gpHci_LeRandCommand_t                                                  LeRand;
    gpHci_LeEnableEncryptionCommand_t                                      LeEnableEncryption;
    gpHci_LeLongTermKeyRequestReplyCommand_t                               LeLongTermKeyRequestReply;
    gpHci_LeLongTermKeyRequestNegativeReplyCommand_t                       LeLongTermKeyRequestNegativeReply;
    gpHci_LeReadSupportedStatesCommand_t                                   LeReadSupportedStates;
    gpHci_LeReceiverTestCommand_t                                          LeReceiverTest;
    gpHci_LeTransmitterTestCommand_t                                       LeTransmitterTest;
    gpHci_LeTestEndCommand_t                                               LeTestEnd;
    gpHci_LeRemoteConnectionParamRequestReplyCommand_t                     LeRemoteConnectionParamRequestReply;
    gpHci_LeRemoteConnectionParamRequestNegReplyCommand_t                  LeRemoteConnectionParamRequestNegReply;
    gpHci_LeSetDataLengthCommand_t                                         LeSetDataLength;
    gpHci_LeReadSuggestedDefDataLengthCommand_t                            LeReadSuggestedDefDataLength;
    gpHci_LeWriteSuggestedDefDataLengthCommand_t                           LeWriteSuggestedDefDataLength;
    gpHci_LeAddDeviceToResolvingListCommand_t                              LeAddDeviceToResolvingList;
    gpHci_LeRemoveDeviceFromResolvingListCommand_t                         LeRemoveDeviceFromResolvingList;
    gpHci_LeClearResolvingListCommand_t                                    LeClearResolvingList;
    gpHci_LeReadResolvingListSizeCommand_t                                 LeReadResolvingListSize;
    gpHci_LeSetAddressResolutionEnableCommand_t                            LeSetAddressResolutionEnable;
    gpHci_LeSetResolvablePrivateAddressTimeoutCommand_t                    LeSetResolvablePrivateAddressTimeout;
    gpHci_LeReadMaxDataLengthCommand_t                                     LeReadMaxDataLength;
    gpHci_LeReadPhyCommand_t                                               LeReadPhy;
    gpHci_LeSetDefaultPhyCommand_t                                         LeSetDefaultPhy;
    gpHci_LeSetPhyCommand_t                                                LeSetPhy;
    gpHci_LeEnhancedReceiverTestCommand_t                                  LeEnhancedReceiverTest;
    gpHci_LeEnhancedTransmitterTestCommand_t                               LeEnhancedTransmitterTest;
    gpHci_LeSetAdvertisingSetRandomAddressCommand_t                        LeSetAdvertisingSetRandomAddress;
    gpHci_LeSetExtendedAdvertisingParametersCommand_t                      LeSetExtendedAdvertisingParameters;
    gpHci_LeSetExtendedAdvertisingDataCommand_t                            LeSetExtendedAdvertisingData;
    gpHci_LeSetExtendedScanResponseDataCommand_t                           LeSetExtendedScanResponseData;
    gpHci_LeSetExtendedAdvertisingEnableCommand_t                          LeSetExtendedAdvertisingEnable;
    gpHci_LeReadMaximumAdvertisingDataLengthCommand_t                      LeReadMaximumAdvertisingDataLength;
    gpHci_LeReadNumberSupportedAdvertisingSetsCommand_t                    LeReadNumberSupportedAdvertisingSets;
    gpHci_LeRemoveAdvertisingSetCommand_t                                  LeRemoveAdvertisingSet;
    gpHci_LeClearAdvertisingSetsCommand_t                                  LeClearAdvertisingSets;
    gpHci_LeSetPeriodicAdvertisingParametersCommand_t                      LeSetPeriodicAdvertisingParameters;
    gpHci_LeSetPeriodicAdvertisingDataCommand_t                            LeSetPeriodicAdvertisingData;
    gpHci_LeSetPeriodicAdvertisingEnableCommand_t                          LeSetPeriodicAdvertisingEnable;
    gpHci_LeSetExtendedScanParametersCommand_t                             LeSetExtendedScanParameters;
    gpHci_LeSetExtendedScanEnableCommand_t                                 LeSetExtendedScanEnable;
    gpHci_LeExtendedCreateConnectionCommand_t                              LeExtendedCreateConnection;
    gpHci_LePeriodicAdvertisingCreateSyncCommand_t                         LePeriodicAdvertisingCreateSync;
    gpHci_LePeriodicAdvertisingCreateSyncCancelCommand_t                   LePeriodicAdvertisingCreateSyncCancel;
    gpHci_LePeriodicAdvertisingTerminateSyncCommand_t                      LePeriodicAdvertisingTerminateSync;
    gpHci_LeAddDeviceToPeriodicAdvertiserListCommand_t                     LeAddDeviceToPeriodicAdvertiserList;
    gpHci_LeRemoveDeviceFromPeriodicAdvertiserListCommand_t                LeRemoveDeviceFromPeriodicAdvertiserList;
    gpHci_LeClearPeriodicAdvertiserListCommand_t                           LeClearPeriodicAdvertiserList;
    gpHci_LeReadPeriodicAdvertiserListSizeCommand_t                        LeReadPeriodicAdvertiserListSize;
    gpHci_LeReadTransmitPowerCommand_t                                     LeReadTransmitPower;
    gpHci_LeReadRfPathCompensationCommand_t                                LeReadRfPathCompensation;
    gpHci_LeWriteRfPathCompensationCommand_t                               LeWriteRfPathCompensation;
    gpHci_LeSetPrivacyModeCommand_t                                        LeSetPrivacyMode;
    gpHci_LeReceiverTest_v3Command_t                                       LeReceiverTest_v3;
    gpHci_LeTransmitterTest_v3Command_t                                    LeTransmitterTest_v3;
    gpHci_LeSetConnectionlessCteTransmitParametersCommand_t                LeSetConnectionlessCteTransmitParameters;
    gpHci_LeSetConnectionlessCteTransmitEnableCommand_t                    LeSetConnectionlessCteTransmitEnable;
    gpHci_LeSetConnectionCteReceiveParametersCommand_t                     LeSetConnectionCteReceiveParameters;
    gpHci_LeSetConnectionCteTransmitParametersCommand_t                    LeSetConnectionCteTransmitParameters;
    gpHci_LeConnectionCteRequestEnableCommand_t                            LeConnectionCteRequestEnable;
    gpHci_LeConnectionCteResponseEnableCommand_t                           LeConnectionCteResponseEnable;
    gpHci_LeReadAntennaInformationCommand_t                                LeReadAntennaInformation;
    gpHci_LeSetPeriodicAdvertisingReceiveEnableCommand_t                   LeSetPeriodicAdvertisingReceiveEnable;
    gpHci_LePeriodicAdvertisingSyncTransferCommand_t                       LePeriodicAdvertisingSyncTransfer;
    gpHci_LePeriodicAdvertisingSetInfoTransferCommand_t                    LePeriodicAdvertisingSetInfoTransfer;
    gpHci_LeSetPeriodicAdvertisingSyncTransferParametersCommand_t          LeSetPeriodicAdvertisingSyncTransferParameters;
    gpHci_LeSetDefaultPeriodicAdvertisingSyncTransferParametersCommand_t   LeSetDefaultPeriodicAdvertisingSyncTransferParameters;
    gpHci_LeReadBufferSize_v2Command_t                                     LeReadBufferSize_v2;
    gpHci_LeSetCigParametersCommand_t                                      LeSetCigParameters;
    gpHci_LeSetCigParametersTestCommand_t                                  LeSetCigParametersTest;
    gpHci_LeCreateCisCommand_t                                             LeCreateCis;
    gpHci_LeRemoveCigCommand_t                                             LeRemoveCig;
    gpHci_LeAcceptCisRequestCommand_t                                      LeAcceptCisRequest;
    gpHci_LeRejectCisRequestCommand_t                                      LeRejectCisRequest;
    gpHci_LeSetupIsoDataPathCommand_t                                      LeSetupIsoDataPath;
    gpHci_LeRemoveIsoDataPathCommand_t                                     LeRemoveIsoDataPath;
    gpHci_LeReadIsoLinkQualityCommand_t                                    LeReadIsoLinkQuality;
    gpHci_LeReadIsoTxSyncCommand_t                                         LeReadIsoTxSync;
    gpHci_LeIsoTxTestCommand_t                                             LeIsoTxTest;
    gpHci_LeIsoRxTestCommand_t                                             LeIsoRxTest;
    gpHci_LeIsoReadTestCountersCommand_t                                   LeIsoReadTestCounters;
    gpHci_LeIsoTestEndCommand_t                                            LeIsoTestEnd;
    gpHci_LeSetHostFeatureCommand_t                                        LeSetHostFeature;
    gpHci_VsdWriteDeviceAddressCommand_t                                   VsdWriteDeviceAddress;
    gpHci_VsdGenerateAccessAddressCommand_t                                VsdGenerateAccessAddress;
    gpHci_SetVsdTestParamsCommand_t                                        SetVsdTestParams;
    gpHci_VsdSetDataPumpParametersCommand_t                                VsdSetDataPumpParameters;
    gpHci_VsdSetDataPumpEnableCommand_t                                    VsdSetDataPumpEnable;
    gpHci_VsdSetNullSinkEnableCommand_t                                    VsdSetNullSinkEnable;
    gpHci_VsdSetAccessCodeCommand_t                                        VsdSetAccessCode;
    gpHci_VsdSetAccessCodeValidationParametersCommand_t                    VsdSetAccessCodeValidationParameters;
    gpHci_VsdSetTransmitPowerCommand_t                                     VsdSetTransmitPower;
    gpHci_VsdSetSleepCommand_t                                             VsdSetSleep;
    gpHci_VsdDisableSlaveLatencyCommand_t                                  VsdDisableSlaveLatency;
    gpHci_VsdEnCBByDefaultCommand_t                                        VsdEnCBByDefault;
    gpHci_VsdOverruleRemoteMaxRxOctetsAndTimeCommand_t                     VsdOverruleRemoteMaxRxOctetsAndTime;
    gpHci_VsdSetRpaPrandCommand_t                                          VsdSetRpaPrand;
    gpHci_VsdGetBuildP4ChangelistCommand_t                                 VsdGetBuildP4Changelist;
    gpHci_VsdSetAdvertisingCoexParamsCommand_t                             VsdSetAdvertisingCoexParams;
    gpHci_VsdSetScanCoexParamsCommand_t                                    VsdSetScanCoexParams;
    gpHci_VsdSetInitCoexParamsCommand_t                                    VsdSetInitCoexParams;
    gpHci_VsdGeneratePeerResolvableAddressCommand_t                        VsdGeneratePeerResolvableAddress;
    gpHci_VsdGenerateLocalResolvableAddressCommand_t                       VsdGenerateLocalResolvableAddress;
    gpHci_VsdSetMinUsedChannelsCommand_t                                   VsdSetMinUsedChannels;
    gpHci_VsdSetConnSlaCoexParamsCommand_t                                 VsdSetConnSlaCoexParams;
    gpHci_VsdSetConnMasCoexParamsCommand_t                                 VsdSetConnMasCoexParams;
    gpHci_VsdUnsolicitedCteTxEnableCommand_t                               VsdUnsolicitedCteTxEnable;
    gpHci_VsdRNGCommand_t                                                  VsdRNG;
    gpHci_SetLeMetaVSDEventCommand_t                                       SetLeMetaVSDEvent;
    gpHci_ResetLeMetaVSDEventCommand_t                                     ResetLeMetaVSDEvent;
    gpHci_VsdBleCoexAlternatePriorityCommand_t                             VsdBleCoexAlternatePriority;
    gpHci_VsdReadResolvingListCurrentSizeCommand_t                         VsdReadResolvingListCurrentSize;
    gpHci_VsdSetResolvingListMaxSizeCommand_t                              VsdSetResolvingListMaxSize;
    gpHci_VsdSetConnSlaCoexUpdateParamsCommand_t                           VsdSetConnSlaCoexUpdateParams;
    gpHci_VsdSetConnMasCoexUpdateParamsCommand_t                           VsdSetConnMasCoexUpdateParams;
    gpHci_VsdSetExponentialBaseCommand_t                                   VsdSetExponentialBase;
    gpHci_VsdSetMinimalSubeventDistanceCommand_t                           VsdSetMinimalSubeventDistance;
    gpHci_VsdGetRtMgrVersionCommand_t                                      VsdGetRtMgrVersion;
    gpHci_VsdEnhancedReceiverTestCommand_t                                 VsdEnhancedReceiverTest;
    gpHci_VsdLeReceiverTest_v3Command_t                                    VsdLeReceiverTest_v3;
    gpHci_UnknownOpCodeCommand_t                                           UnknownOpCode;
} gpHci_CommandParameters_t;

#endif //_GPHCI_STRUCTURES_H_

