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


#ifndef _GPHCI_OPCODES_H_
#define _GPHCI_OPCODES_H_

/*****************************************************************************
 *                    Includes Definition
 *****************************************************************************/

#include "global.h"
#include "gpHci_types.h"

/*****************************************************************************
*                    Macro Definitions
*****************************************************************************/

// Opcode list
#define gpHci_OpCodeDisconnect                                                 0x0406
#define gpHci_OpCodeReadRemoteVersionInfo                                      0x041D
#define gpHci_OpCodeSetEventMask                                               0x0C01
#define gpHci_OpCodeReset                                                      0x0C03
#define gpHci_OpCodeReadConnectionAcceptTimeout                                0x0C15
#define gpHci_OpCodeWriteConnectionAcceptTimeout                               0x0C16
#define gpHci_OpCodeReadTransmitPowerLevel                                     0x0C2D
#define gpHci_OpCodeConfigureDataPath                                          0x1083
#define gpHci_OpCodeHostBufferSize                                             0x0C33
#define gpHci_OpCodeSetEventMaskPage2                                          0x0C63
#define gpHci_OpCodeReadAuthenticatedPayloadTO                                 0x0C7B
#define gpHci_OpCodeWriteAuthenticatedPayloadTO                                0x0C7C
#define gpHci_OpCodeReadLocalVersionInformation                                0x1001
#define gpHci_OpCodeReadLocalSupportedCommands                                 0x1002
#define gpHci_OpCodeReadLocalSupportedFeatures                                 0x1003
#define gpHci_OpCodeReadBufferSize                                             0x1005
#define gpHci_OpCodeReadBdAddr                                                 0x1009
#define gpHci_OpCodeReadLocalSupportedCodecs                                   0x100D
#define gpHci_OpCodeReadLocalSupportedCodecCapabilities                        0x100E
#define gpHci_OpCodeReadLocalSupportedControllerDelay                          0x100F
#define gpHci_OpCodeReadRSSI                                                   0x1405
#define gpHci_OpCodeLeSetEventMask                                             0x2001
#define gpHci_OpCodeLeReadBufferSize                                           0x2002
#define gpHci_OpCodeLeReadLocalSupportedFeatures                               0x2003
#define gpHci_OpCodeLeSetRandomAddress                                         0x2005
#define gpHci_OpCodeLeSetAdvertisingParameters                                 0x2006
#define gpHci_OpCodeLeReadAdvertisingChannelTxPower                            0x2007
#define gpHci_OpCodeLeSetAdvertisingData                                       0x2008
#define gpHci_OpCodeLeSetScanResponseData                                      0x2009
#define gpHci_OpCodeLeSetAdvertiseEnable                                       0x200A
#define gpHci_OpCodeLeSetScanParameters                                        0x200B
#define gpHci_OpCodeLeSetScanEnable                                            0x200C
#define gpHci_OpCodeLeCreateConnection                                         0x200D
#define gpHci_OpCodeLeCreateConnectionCancel                                   0x200E
#define gpHci_OpCodeLeReadWhiteListSize                                        0x200F
#define gpHci_OpCodeLeClearWhiteList                                           0x2010
#define gpHci_OpCodeLeAddDeviceToWhiteList                                     0x2011
#define gpHci_OpCodeLeRemoveDeviceFromWhiteList                                0x2012
#define gpHci_OpCodeLeConnectionUpdate                                         0x2013
#define gpHci_OpCodeLeSetHostChannelClassification                             0x2014
#define gpHci_OpCodeLeReadChannelMap                                           0x2015
#define gpHci_OpCodeLeReadRemoteFeatures                                       0x2016
#define gpHci_OpCodeLeEncrypt                                                  0x2017
#define gpHci_OpCodeLeRand                                                     0x2018
#define gpHci_OpCodeLeEnableEncryption                                         0x2019
#define gpHci_OpCodeLeLongTermKeyRequestReply                                  0x201A
#define gpHci_OpCodeLeLongTermKeyRequestNegativeReply                          0x201B
#define gpHci_OpCodeLeReadSupportedStates                                      0x201C
#define gpHci_OpCodeLeReceiverTest                                             0x201D
#define gpHci_OpCodeLeTransmitterTest                                          0x201E
#define gpHci_OpCodeLeTestEnd                                                  0x201F
#define gpHci_OpCodeLeRemoteConnectionParamRequestReply                        0x2020
#define gpHci_OpCodeLeRemoteConnectionParamRequestNegReply                     0x2021
#define gpHci_OpCodeLeSetDataLength                                            0x2022
#define gpHci_OpCodeLeReadSuggestedDefDataLength                               0x2023
#define gpHci_OpCodeLeWriteSuggestedDefDataLength                              0x2024
#define gpHci_OpCodeLeAddDeviceToResolvingList                                 0x2027
#define gpHci_OpCodeLeRemoveDeviceFromResolvingList                            0x2028
#define gpHci_OpCodeLeClearResolvingList                                       0x2029
#define gpHci_OpCodeLeReadResolvingListSize                                    0x202A
#define gpHci_OpCodeLeSetAddressResolutionEnable                               0x202D
#define gpHci_OpCodeLeSetResolvablePrivateAddressTimeout                       0x202E
#define gpHci_OpCodeLeReadMaxDataLength                                        0x202F
#define gpHci_OpCodeLeReadPhy                                                  0x2030
#define gpHci_OpCodeLeSetDefaultPhy                                            0x2031
#define gpHci_OpCodeLeSetPhy                                                   0x2032
#define gpHci_OpCodeLeEnhancedReceiverTest                                     0x2033
#define gpHci_OpCodeLeEnhancedTransmitterTest                                  0x2034
#define gpHci_OpCodeLeSetAdvertisingSetRandomAddress                           0x2035
#define gpHci_OpCodeLeSetExtendedAdvertisingParameters                         0x2036
#define gpHci_OpCodeLeSetExtendedAdvertisingData                               0x2037
#define gpHci_OpCodeLeSetExtendedScanResponseData                              0x2038
#define gpHci_OpCodeLeSetExtendedAdvertisingEnable                             0x2039
#define gpHci_OpCodeLeReadMaximumAdvertisingDataLength                         0x203A
#define gpHci_OpCodeLeReadNumberSupportedAdvertisingSets                       0x203B
#define gpHci_OpCodeLeRemoveAdvertisingSet                                     0x203C
#define gpHci_OpCodeLeClearAdvertisingSets                                     0x203D
#define gpHci_OpCodeLeSetPeriodicAdvertisingParameters                         0x203E
#define gpHci_OpCodeLeSetPeriodicAdvertisingData                               0x203F
#define gpHci_OpCodeLeSetPeriodicAdvertisingEnable                             0x2040
#define gpHci_OpCodeLeSetExtendedScanParameters                                0x2041
#define gpHci_OpCodeLeSetExtendedScanEnable                                    0x2042
#define gpHci_OpCodeLeExtendedCreateConnection                                 0x2043
#define gpHci_OpCodeLePeriodicAdvertisingCreateSync                            0x2044
#define gpHci_OpCodeLePeriodicAdvertisingCreateSyncCancel                      0x2045
#define gpHci_OpCodeLePeriodicAdvertisingTerminateSync                         0x2046
#define gpHci_OpCodeLeAddDeviceToPeriodicAdvertiserList                        0x2047
#define gpHci_OpCodeLeRemoveDeviceFromPeriodicAdvertiserList                   0x2048
#define gpHci_OpCodeLeClearPeriodicAdvertiserList                              0x2049
#define gpHci_OpCodeLeReadPeriodicAdvertiserListSize                           0x204A
#define gpHci_OpCodeLeReadTransmitPower                                        0x204B
#define gpHci_OpCodeLeReadRfPathCompensation                                   0x204C
#define gpHci_OpCodeLeWriteRfPathCompensation                                  0x204D
#define gpHci_OpCodeLeSetPrivacyMode                                           0x204E
#define gpHci_OpCodeLeReceiverTest_v3                                          0x204F
#define gpHci_OpCodeLeTransmitterTest_v3                                       0x2050
#define gpHci_OpCodeLeSetConnectionlessCteTransmitParameters                   0x2051
#define gpHci_OpCodeLeSetConnectionlessCteTransmitEnable                       0x2052
#define gpHci_OpCodeLeSetConnectionCteReceiveParameters                        0x2054
#define gpHci_OpCodeLeSetConnectionCteTransmitParameters                       0x2055
#define gpHci_OpCodeLeConnectionCteRequestEnable                               0x2056
#define gpHci_OpCodeLeConnectionCteResponseEnable                              0x2057
#define gpHci_OpCodeLeReadAntennaInformation                                   0x2058
#define gpHci_OpCodeLeSetPeriodicAdvertisingReceiveEnable                      0x2059
#define gpHci_OpCodeLePeriodicAdvertisingSyncTransfer                          0x205A
#define gpHci_OpCodeLePeriodicAdvertisingSetInfoTransfer                       0x205B
#define gpHci_OpCodeLeSetPeriodicAdvertisingSyncTransferParameters             0x205C
#define gpHci_OpCodeLeSetDefaultPeriodicAdvertisingSyncTransferParameters      0x205D
#define gpHci_OpCodeLeReadBufferSize_v2                                        0x2060
#define gpHci_OpCodeLeSetCigParameters                                         0x2062
#define gpHci_OpCodeLeSetCigParametersTest                                     0x2063
#define gpHci_OpCodeLeCreateCis                                                0x2064
#define gpHci_OpCodeLeRemoveCig                                                0x2065
#define gpHci_OpCodeLeAcceptCisRequest                                         0x2066
#define gpHci_OpCodeLeRejectCisRequest                                         0x2067
#define gpHci_OpCodeLeSetupIsoDataPath                                         0x206E
#define gpHci_OpCodeLeRemoveIsoDataPath                                        0x206F
#define gpHci_OpCodeLeReadIsoLinkQuality                                       0x2075
#define gpHci_OpCodeLeReadIsoTxSync                                            0x2061
#define gpHci_OpCodeLeIsoTxTest                                                0x2070
#define gpHci_OpCodeLeIsoRxTest                                                0x2071
#define gpHci_OpCodeLeIsoReadTestCounters                                      0x2072
#define gpHci_OpCodeLeIsoTestEnd                                               0x2073
#define gpHci_OpCodeLeSetHostFeature                                           0x2074
#define gpHci_OpCodeVsdWriteDeviceAddress                                      0xFC01
#define gpHci_OpCodeVsdGenerateAccessAddress                                   0xFC02
#define gpHci_OpCodeSetVsdTestParams                                           0xFC03
#define gpHci_OpCodeVsdSetDataPumpParameters                                   0xFC04
#define gpHci_OpCodeVsdSetDataPumpEnable                                       0xFC05
#define gpHci_OpCodeVsdSetNullSinkEnable                                       0xFC06
#define gpHci_OpCodeVsdSetAccessCode                                           0xFC07
#define gpHci_OpCodeVsdSetAccessCodeValidationParameters                       0xFC09
#define gpHci_OpCodeVsdSetTransmitPower                                        0xFC0A
#define gpHci_OpCodeVsdSetSleep                                                0xFC0B
#define gpHci_OpCodeVsdDisableSlaveLatency                                     0xFC0C
#define gpHci_OpCodeVsdEnCBByDefault                                           0xFC0D
#define gpHci_OpCodeVsdOverruleRemoteMaxRxOctetsAndTime                        0xFC0E
#define gpHci_OpCodeVsdSetRpaPrand                                             0xFC0F
#define gpHci_OpCodeVsdGetBuildP4Changelist                                    0xFC11
#define gpHci_OpCodeVsdSetAdvertisingCoexParams                                0xFC13
#define gpHci_OpCodeVsdSetScanCoexParams                                       0xFC14
#define gpHci_OpCodeVsdSetInitCoexParams                                       0xFC15
#define gpHci_OpCodeVsdGeneratePeerResolvableAddress                           0xFC16
#define gpHci_OpCodeVsdGenerateLocalResolvableAddress                          0xFC17
#define gpHci_OpCodeVsdSetMinUsedChannels                                      0xFC18
#define gpHci_OpCodeVsdSetConnSlaCoexParams                                    0xFC19
#define gpHci_OpCodeVsdSetConnMasCoexParams                                    0xFC1A
#define gpHci_OpCodeVsdUnsolicitedCteTxEnable                                  0xFC1B
#define gpHci_OpCodeVsdRNG                                                     0xFC1C
#define gpHci_OpCodeSetLeMetaVSDEvent                                          0xFC1D
#define gpHci_OpCodeResetLeMetaVSDEvent                                        0xFC1E
#define gpHci_OpCodeVsdBleCoexAlternatePriority                                0xFC1F
#define gpHci_OpCodeVsdReadResolvingListCurrentSize                            0xFC20
#define gpHci_OpCodeVsdSetResolvingListMaxSize                                 0xFC21
#define gpHci_OpCodeVsdSetConnSlaCoexUpdateParams                              0xFC22
#define gpHci_OpCodeVsdSetConnMasCoexUpdateParams                              0xFC23
#define gpHci_OpCodeVsdSetExponentialBase                                      0xFC24
#define gpHci_OpCodeVsdSetMinimalSubeventDistance                              0xFC25
#define gpHci_OpCodeVsdGetRtMgrVersion                                         0xFC26
#define gpHci_OpCodeVsdEnhancedReceiverTest                                    0xFC27
#define gpHci_OpCodeVsdLeReceiverTest_v3                                       0xFC28
#define gpHci_OpCodeUnknownOpCode                                              0xFCFF

#endif //_GPHCI_OPCODES_H_
