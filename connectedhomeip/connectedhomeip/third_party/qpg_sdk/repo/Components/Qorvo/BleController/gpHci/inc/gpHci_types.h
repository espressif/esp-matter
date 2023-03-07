/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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

/** @file "gpHci_types.h"
 *
 *  Bluetooth Low Energy interface
 *
 *  Declarations of the public functions and enumerations of gpHci_types.
*/

#ifndef _GPHCI_TYPES_H_
#define _GPHCI_TYPES_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/* <CodeGenerator Placeholder> AdditionalIncludes */
#include "gpHci_Opcodes.h"
/* </CodeGenerator Placeholder> AdditionalIncludes */

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum gpHci_Result_t */
//@{
/** @brief  V4.2 Part D: 2 Error Code Descriptions */
#define gpHci_ResultSuccess                                    0x00
#define gpHci_ResultUnknownHCICommand                          0x01
#define gpHci_ResultUnknownConnectionIdentifier                0x02
#define gpHci_ResultHardwareFailure                            0x03
#define gpHci_ResultPageTimeout                                0x04
#define gpHci_ResultAuthenticationFailure                      0x05
#define gpHci_ResultPINorKeyMissing                            0x06
#define gpHci_ResultMemoryCapacityExceeded                     0x07
#define gpHci_ResultConnectionTimeout                          0x08
#define gpHci_ResultConnectionLimitExceeded                    0x09
#define gpHci_ResultSynchronousConnectionLimitToADeviceExceeded     0x0A
#define gpHci_ResultConnectionAlreadyExists                    0x0B
#define gpHci_ResultCommandDisallowed                          0x0C
#define gpHci_ResultConnectionRejectedduetoLimitedResources     0x0D
#define gpHci_ResultConnectionRejectedDueToSecurityReasons     0x0E
#define gpHci_ResultConnectionRejectedduetoUnacceptableBdAddr     0x0F
#define gpHci_ResultConnectionAcceptTimeoutExceeded            0x10
#define gpHci_ResultUnsupportedFeatureOrParameterValue         0x11
#define gpHci_ResultInvalidHCICommandParameters                0x12
#define gpHci_ResultRemoteUserTerminatedConnection             0x13
#define gpHci_ResultRemoteDeviceTerminatedConnectionduetoLowResources     0x14
#define gpHci_ResultRemoteDeviceTerminatedConnectionduetoPowerOff     0x15
#define gpHci_ResultConnectionTerminatedByLocalHost            0x16
#define gpHci_ResultRepeatedAttempts                           0x17
#define gpHci_ResultPairingNotAllowed                          0x18
#define gpHci_ResultUnknownLMPPDU                              0x19
#define gpHci_ResultUnsupportedRemoteFeatureUnsupportedLmpFeature     0x1A
#define gpHci_ResultSCOOffsetRejected                          0x1B
#define gpHci_ResultSCOIntervalRejected                        0x1C
#define gpHci_ResultSCOAirModeRejected                         0x1D
#define gpHci_ResultInvalidLMPParametersInvalidLLParameters     0x1E
#define gpHci_ResultUnspecifiedError                           0x1F
#define gpHci_ResultUnsupportedLMPParameterValueUnsupportedLLParameterValue     0x20
#define gpHci_ResultRoleChangeNotAllowed                       0x21
#define gpHci_ResultLMPResponseTimeoutLLResponseTimeout        0x22
#define gpHci_ResultLLProcedureCollision                       0x23
#define gpHci_ResultLMPPDUNotAllowed                           0x24
#define gpHci_ResultEncryptionModeNotAcceptable                0x25
#define gpHci_ResultLinkKeycannotbeChanged                     0x26
#define gpHci_ResultRequestedQoSNotSupported                   0x27
#define gpHci_ResultInstantPassed                              0x28
#define gpHci_ResultPairingWithUnitKeyNotSupported             0x29
#define gpHci_ResultDifferentTransactionCollision              0x2A
#define gpHci_ResultReservedForFutureUse1                      0x2B
#define gpHci_ResultQoSUnacceptableParameter                   0x2C
#define gpHci_ResultQoSRejected                                0x2D
#define gpHci_ResultChannelClassificationNotSupported          0x2E
#define gpHci_ResultInsufficientSecurity                       0x2F
#define gpHci_ResultParameterOutOfMandatoryRange               0x30
#define gpHci_ResultReservedForFutureUse2                      0x31
#define gpHci_ResultRoleSwitchPending                          0x32
#define gpHci_ResultReservedForFutureUse3                      0x33
#define gpHci_ResultReservedSlotViolation                      0x34
#define gpHci_ResultRoleSwitchFailed                           0x35
#define gpHci_ResultExtendedInquiryResponseTooLarge            0x36
#define gpHci_ResultSecureSimplePairingNotSupportedByHost      0x37
#define gpHci_ResultHostBusyPairing                            0x38
#define gpHci_ResultConnectionRejectedduetoNoSuitableChannelFound     0x39
#define gpHci_ResultControllerBusy                             0x3A
#define gpHci_ResultUnacceptableConnectionParameters           0x3B
#define gpHci_ResultAdvertisingTimeout                         0x3C
#define gpHci_ResultConnectionTerminatedduetoMICFailure        0x3D
#define gpHci_ResultConnectionFailedtobeEstablished            0x3E
#define gpHci_ResultMACConnectionFailed                        0x3F
#define gpHci_ResultCoarseClockAdjustmentRejected              0x40
#define gpHci_ResultType0SubmapNotDefined                      0x41
#define gpHci_ResultUnknownAdvertisingIdentifier               0x42
#define gpHci_ResultLimitReached                               0x43
#define gpHci_ResultOperationCancelledByHost                   0x44
#define gpHci_ResultPacketTooLong                              0x45
#define gpHci_ResultInvalid                                    0xFF
/** @typedef gpHci_Result_t
 *  @brief Command return codes - V4.2 Part D: 2 Error Code Descriptions
*/
typedef UInt8                             gpHci_Result_t;
//@}

/** @enum gpHci_Ogf_t */
//@{
#define gpHci_OgfLinkControlCommands                           0x01
#define gpHci_OgfLinkPolicyCommands                            0x02
#define gpHci_OgfControllerBasebandCommands                    0x03
#define gpHci_OgfInformationalParameters                       0x04
#define gpHci_OgfStatusParameters                              0x05
#define gpHci_OgfTestingCommands                               0x06
#define gpHci_OgfEvents                                        0x07
#define gpHci_OgfLeControllerCommands                          0x08
#define gpHci_OgfVendorSpecificDebugCommands                   0x3F
/** @typedef gpHci_Ogf_t
 *  @brief HCI Command Opcode Group Field (OGF) codes
*/
typedef UInt8                             gpHci_Ogf_t;
//@}

/** @enum gpHci_CommandOpCode_t */
//@{
#define gpHci_OpcodeNoOperation                                0x0000
#define gpHci_OpcodeInvalid                                    0xFFFF
/** @typedef gpHci_CommandOpCode_t
 *  @brief Special opCodes
*/
typedef UInt16                            gpHci_CommandOpCode_t;
//@}

/** @enum gpHci_TLVType_t */
//@{
#define gpBle_SetVsdWinoffsetType                              0x00
#define gpBle_SetVsdPreferredCodedPhyCoding                    0x01
#define gpBle_SetVsdConnEstabBlackoutEnableType                0x02
#define gpBle_SetVsdDataRxFlushAtEndOfEventType                0x03
#define gpBle_SetVsdAuthenticatedPayloadTimeoutEnableType      0x04
#define gpBle_SetVsdGeneratePacketWithCorruptedMIC_OnConnIdType     0x05
#define gpBle_SetVsdConnReqWinSizeType                         0x06
#define gpBle_SetVsdMasterConnEstabFirstMToSSignedOffsetType     0x07
#define gpBle_SetVsdAutomaticFeatureExchangeEnableType         0x08
#define gpBle_SetVsdSleepClockAccuracyType                     0x09
#define gpBle_SetVsdDirectTestTxPacketCountType                0x0A
#define gpBle_SetVsdArtificialDriftAsSignedNbrMicrosecType     0x0D
#define gpBle_SetVsdFeatureSetUsedType                         0x0E
#define gpBle_SetVsdTriggerForAnchorMoveType                   0x10
#define gpBle_SetVsdOverruleLocalSupportedFeatures             0x11
#define gpBle_SetVsdPER_TestMode                               0x12
#define gpBle_SetVsdDualModeTimeFor15Dot4Type                  0x13
#define gpBle_SetVsdProcessorClockSpeed                        0x14
#define gpBle_SetVsdConnectionEventPriority                    0x15
#define gpBle_SetVsdScanEventPriority                          0x16
#define gpBle_SetVsdAdvEventPriority                           0x17
#define gpBle_SetVsdDataChannelRxQueueLatencyType              0x18
#define gpBle_SetVsdfixedRxWindowThreshold                     0x19
#define gpBle_SetVsdDirectTestModeAntenna                      0x1B
#define gpBle_TLVTypeInvalid                                   0xFF
/** @typedef gpHci_TLVType_t
 *  @brief Special opCodes http://wiki.greenpeak.com/index.php/List_Of_Vendor_Specific_Debug_settings
*/
typedef UInt8                             gpHci_TLVType_t;
//@}

/** @enum gpHci_EventCode_t */
//@{
#define gpHci_EventCode_ConnectionComplete                     0x0003
#define gpHci_EventCode_DisconnectionComplete                  0x0005
#define gpHci_EventCode_EncryptionChange                       0x0008
#define gpHci_EventCode_ReadRemoteVersionInfoComplete          0x000C
#define gpHci_EventCode_CommandComplete                        0x000E
#define gpHci_EventCode_CommandStatus                          0x000F
#define gpHci_EventCode_HardwareError                          0x0010
#define gpHci_EventCode_NumberOfCompletedPackets               0x0013
#define gpHci_EventCode_DataBufferOverflow                     0x001A
#define gpHci_EventCode_EncryptionKeyRefreshComplete           0x0030
#define gpHci_EventCode_LEMeta                                 0x003E
#define gpHci_EventCode_AuthenticatedPayloadToExpired          0x0057
#define gpHci_EventCode_VsdSinkRxIndication                    0x0059
#define gpHci_EventCode_VsdMeta                                0x00FF
#define gpHci_EventCode_Invalid                                0xFFFE
/** @brief Test */
#define gpHci_EventCode_Nothing                                0xFFFF
/** @typedef gpHci_EventCode_t
 *  @brief Event codes..add missing the spec only dictates one byte, but we use 2 for our own additions (like eventCode nothing)
*/
typedef UInt16                            gpHci_EventCode_t;
//@}

/** @enum gpHci_LEMetaSubEventCode_t */
//@{
#define gpHci_LEMetaSubEventCodeConnectionComplete             0x01
#define gpHci_LEMetaSubEventCodeAdvertisingReport              0x02
#define gpHci_LEMetaSubEventCodeConnectionUpdateComplete       0x03
#define gpHci_LEMetaSubEventCodeReadFeaturesComplete           0x04
#define gpHci_LEMetaSubEventCodeLongTermKeyRequest             0x05
#define gpHci_LEMetaSubEventCodeRemoteConnectionParameter      0x06
#define gpHci_LEMetaSubEventCodeDataLengthChange               0x07
#define gpHci_LEMetaSubEventCodeReadLocalPublicKey             0x08
#define gpHci_LEMetaSubEventCodeGenerateDHKeyComplete          0x09
#define gpHci_LEMetaSubEventCodeEnhancedConnectionComplete     0x0A
#define gpHci_LEMetaSubEventCodeDirectAdvertisingReport        0x0B
#define gpHci_LEMetaSubEventCodePhyUpdateComplete              0x0C
#define gpHci_LEMetaSubEventCodeExtendedAdvertisingReport      0x0D
#define gpHci_LEMetaSubEventCodePeriodicAdvSyncEstablished     0x0E
#define gpHci_LEMetaSubEventCodePeriodicAdvReportEvent         0x0F
#define gpHci_LEMetaSubEventCodePeriodicAdvSyncLost            0x10
#define gpHci_LEMetaSubEventCodeScanTimeout                    0x11
#define gpHci_LEMetaSubEventCodeAdvertisingSetTerminated       0x12
#define gpHci_LEMetaSubEventCodeScanRequestReceived            0x13
#define gpHci_LEMetaSubEventCodeChannelSelectionAlgorithm      0x14
#define gpHci_LEMetaSubEventCodeConnectionlessIqReport         0x15
#define gpHci_LEMetaSubEventCodeConnectionIqReport             0x16
#define gpHci_LEMetaSubEventCodeCteRequestFailed               0x17
#define gpHci_LEMetaSubEventCodePastReceived                   0x18
#define gpHci_LEMetaSubEventCodeCisEstablished                 0x19
#define gpHci_LEMetaSubEventCodeCisRequest                     0x1A
typedef UInt8                             gpHci_LEMetaSubEventCode_t;
//@}

/** @enum gpHci_VsdSubEventCode_t */
//@{
#define gpHci_VsdSubEventForwardEventProcessedMessages         0x01
#define gpHci_VsdSubEventWhiteListModified                     0x02
#define gpHci_VsdSubEventAngleUpdateInd                        0x03
#define gpHci_VsdSubEventConnEvtCount                          0xFE
typedef UInt8                             gpHci_VsdSubEventCode_t;
//@}

/** @enum gpHci_AdvertisingType_t */
//@{
#define gpHci_AdvertisingType_ConnectableUndirected            0x00
#define gpHci_AdvertisingType_ConnectableHighDutyCycleDirected     0x01
#define gpHci_AdvertisingType_ScannableUndirected              0x02
#define gpHci_AdvertisingType_NonConnectableUndirected         0x03
#define gpHci_AdvertisingType_ConnectableLowDutyCycleDirected     0x04
#define gpHci_AdvertisingType_Invalid                          0x05
/** @typedef gpHci_AdvertisingType_t
 *  @brief Advertising types
*/
typedef UInt8                             gpHci_AdvertisingType_t;
//@}

/** @enum gpHci_AdvertisingReportEventType_t */
//@{
#define gpHci_AdvertisingReportEventType_AdvInd                0x00
#define gpHci_AdvertisingReportEventType_AdvDirectInd          0x01
#define gpHci_AdvertisingReportEventType_AdvScanInd            0x02
#define gpHci_AdvertisingReportEventType_AdvNonConnInd         0x03
#define gpHci_AdvertisingReportEventType_ScanRsp               0x04
#define gpHci_AdvertisingReportEventType_AdvExtInd             0x05
#define gpHci_AdvertisingReportEventType_Invalid               0x06
/** @typedef gpHci_AdvertisingReportEventType_t
 *  @brief Advertising Report event types
*/
typedef UInt8                             gpHci_AdvertisingReportEventType_t;
//@}

/** @enum gpHci_ExtAdvEventType_t */
//@{
#define gpHci_ExtAdvEventTypeNonConnNonScan                    0x00
#define gpHci_ExtAdvEventTypeConnectable                       0x01
#define gpHci_ExtAdvEventTypeScannable                         0x02
#define gpHci_ExtAdvEventTypeDirected                          0x04
#define gpHci_ExtAdvEventTypeScanRsp                           0x08
#define gpHci_ExtAdvEventTypeLegacy                            0x10
#define gpHci_ExtAdvEventTypeIncompleteMoreToCome              0x20
#define gpHci_ExtAdvEventTypeIncompleteTruncated               0x40
/** @typedef gpHci_ExtAdvEventType_t
 *  @brief  Ext Advertising Report event types: rest of bits are data status
*/
typedef UInt16                            gpHci_ExtAdvEventType_t;
//@}

/** @enum gpHci_ExtAdvLegacyEventType_t */
//@{
#define gpHci_ExtAdvLegacyEventType_AdvInd                     0x13
#define gpHci_ExtAdvLegacyEventType_AdvDirectInd               0x15
#define gpHci_ExtAdvLegacyEventType_AdvScanInd                 0x12
#define gpHci_ExtAdvLegacyEventType_AdvNonConnInd              0x10
#define gpHci_ExtAdvLegacyEventType_ScanRsp2AdvInd             0x1B
#define gpHci_ExtAdvLegacyEventType_ScanRsp2AdvScanInd         0x1A
/** @typedef gpHci_ExtAdvLegacyEventType_t
 *  @brief Ext Adv Report Legacy event types table 7.1
*/
typedef UInt8                             gpHci_ExtAdvLegacyEventType_t;
//@}

/** @enum gpHci_ScanType_t */
//@{
#define gpHci_ScanType_Passive                                 0x00
#define gpHci_ScanType_Active                                  0x01
#define gpHci_ScanType_Invalid                                 0x02
typedef UInt8                             gpHci_ScanType_t;
//@}

/** @enum gpHci_OwnAddressType_t */
//@{
#define gpHci_OwnAddressType_PublicDevice                      0x00
#define gpHci_OwnAddressType_RandomDevice                      0x01
#define gpHci_OwnAddressType_RPAPublicIfUnavailable            0x02
#define gpHci_OwnAddressType_RPARandomIfUnavailable            0x03
#define gpHci_OwnAddressType_Invalid                           0x04
/** @typedef gpHci_OwnAddressType_t
 *  @brief Own Address types
*/
typedef UInt8                             gpHci_OwnAddressType_t;
//@}

/** @enum gpHci_AdvertiserAddressType_t */
//@{
#define gpHci_AdvertiserAddressType_PublicDevice               0x00
#define gpHci_AdvertiserAddressType_RandomDevice               0x01
#define gpHci_AdvertiserAddressType_PublicIdentity             0x02
#define gpHci_AdvertiserAddressType_RandomIdentity             0x03
#define gpHci_AdvertiserAddressType_Invalid                    0x04
/** @typedef gpHci_AdvertiserAddressType_t
 *  @brief Advertiser addresss
*/
typedef UInt8                             gpHci_AdvertiserAddressType_t;
//@}

/** @enum gpHci_AdvPeerAddressType_t */
//@{
#define gpHci_AdvPeerAddressType_Public                        0x00
#define gpHci_AdvPeerAddressType_Random                        0x01
#define gpHci_AdvPeerAddressType_Invalid                       0x02
/** @typedef gpHci_AdvPeerAddressType_t
 *  @brief Advertiser peer address types
*/
typedef UInt8                             gpHci_AdvPeerAddressType_t;
//@}

/** @enum gpHci_AdvFilterPolicy_t */
//@{
#define gpHci_AdvFilterPolicy_All                              0x00
#define gpHci_AdvFilterPolicy_ConnAll_ScanWl                   0x01
#define gpHci_AdvFilterPolicy_ScanAll_ConnWl                   0x02
#define gpHci_AdvFilterPolicy_ScanWl_ConnWl                    0x03
#define gpHci_AdvFilterPolicy_Invalid                          0x04
/** @typedef gpHci_AdvFilterPolicy_t
 *  @brief Advertising filter policies
*/
typedef UInt8                             gpHci_AdvFilterPolicy_t;
//@}

/** @enum gpHci_ScanFilterPolicy_t */
//@{
#define gpHci_ScanFilterPolicy_AcceptAdvAll                    0x00
#define gpHci_ScanFilterPolicy_AcceptOnlyWl                    0x01
#define gpHci_ScanFilterPolicy_AcceptAllUndirected_RpaDirected     0x02
#define gpHci_ScanFilterPolicy_AcceptAllWl_RpaDirected         0x03
#define gpHci_ScanFilterPolicy_Invalid                         0x04
/** @typedef gpHci_ScanFilterPolicy_t
 *  @brief Scan filter policies
*/
typedef UInt8                             gpHci_ScanFilterPolicy_t;
//@}

/** @enum gpHci_ExtScanFilterDuplicates_t */
//@{
#define gpHci_ExtScanFilterDuplicates_Disabled                 0x00
#define gpHci_ExtScanFilterDuplicates_Enabled                  0x01
#define gpHci_ExtScanFilterDuplicates_EnabledWithReset         0x02
#define gpHci_ExtScanFilterDuplicates_Invalid                  0x03
/** @typedef gpHci_ExtScanFilterDuplicates_t
 *  @brief Ext Scan filter duplicates
*/
typedef UInt8                             gpHci_ExtScanFilterDuplicates_t;
//@}

/** @enum gpHci_InitiatorFilterPolicy_t */
//@{
#define gpHci_InitiatorFilterPolicy_NoWL                       0x00
#define gpHci_InitiatorFilterPolicy_UseWL                      0x01
#define gpHci_InitiatorFilterPolicy_Invalid                    0x02
/** @typedef gpHci_InitiatorFilterPolicy_t
 *  @brief Initiator filter policies
*/
typedef UInt8                             gpHci_InitiatorFilterPolicy_t;
//@}

/** @enum gpHci_InitPeerAddressType_t */
//@{
#define gpHci_InitPeerAddressType_PublicDevice                 0x00
#define gpHci_InitPeerAddressType_RandomDevice                 0x01
#define gpHci_InitPeerAddressType_PublicIdentity               0x02
#define gpHci_InitPeerAddressType_RandomIdentity               0x03
#define gpHci_InitPeerAddressType_Invalid                      0x04
#define gpHci_InitPeerAddressType_NoAddress                    0xFF
/** @typedef gpHci_InitPeerAddressType_t
 *  @brief Initiator peer address types
*/
typedef UInt8                             gpHci_InitPeerAddressType_t;
//@}

/** @enum gpHci_WhitelistAddressType_t */
//@{
#define gpHci_WhitelistAddressType_PublicDevice                0x00
#define gpHci_WhitelistAddressType_RandomDevice                0x01
#define gpHci_WhitelistAddressType_Anonymous                   0xFF
/** @typedef gpHci_WhitelistAddressType_t
 *  @brief White list address types
*/
typedef UInt8                             gpHci_WhitelistAddressType_t;
//@}

/** @enum gpHci_DirectAddressType_t */
//@{
#define gpHci_DirectAddressType_PublicDevice                   0x00
#define gpHci_DirectAddressType_RandomDevice                   0x01
#define gpHci_DirectAddressType_PublicIdentity                 0x02
#define gpHci_DirectAddressType_RandomIdentity                 0x03
#define gpHci_DirectAddressType_Invalid                        0x04
#define gpHci_DirectAddressType_RandomUnableToResolve          0xFE
/** @typedef gpHci_DirectAddressType_t
 *  @brief Direct address types
*/
typedef UInt8                             gpHci_DirectAddressType_t;
//@}

/** @enum gpHci_ConnectionRole_t */
//@{
#define gpHci_ConnectionRoleMaster                             0x00
#define gpHci_ConnectionRoleSlave                              0x01
#define gpHci_ConnectionRoleInvalid                            0x02
typedef UInt8                             gpHci_ConnectionRole_t;
//@}

/** @enum gpHci_FlowControlToHost_t */
//@{
#define gpHci_FlowControlToHostOff                             0x00
#define gpHci_FlowControlToHostAclOn                           0x01
#define gpHci_FlowControlToHostSyncOn                          0x02
#define gpHci_FlowControlToHostAllOn                           0x03
#define gpHci_FlowControlToHostInvalid                         0x04
typedef UInt8                             gpHci_FlowControlToHost_t;
//@}

/** @enum gpHci_EncryptionLevel_t */
//@{
#define gpHci_EncryptionLevelOff                               0x00
#define gpHci_EncryptionLevelOn                                0x01
typedef UInt8                             gpHci_EncryptionLevel_t;
//@}

/** @enum gpHci_PacketBoundaryFlag_t */
//@{
#define gpHci_L2CAP_Start                                      0x0
#define gpHci_L2CAP_Continue                                   0x1
#define gpHci_L2CAP_Start_AutoFlush                            0x2
#define gpHci_L2CAP_Invalid                                    0xFF
typedef UInt8                             gpHci_PacketBoundaryFlag_t;
//@}

/** @enum gpHci_IsoPacketBoundaryFlag_t */
//@{
#define gpHci_ISO_SduFirstFrag                                 0x0
#define gpHci_ISO_SduContinue                                  0x1
#define gpHci_ISO_SduComplete                                  0x2
#define gpHci_ISO_SduLastFrag                                  0x3
typedef UInt8                             gpHci_IsoPacketBoundaryFlag_t;
//@}

/** @enum gpHci_EventMaskType_t */
//@{
#define gpHci_CB                                               0x01
#define gpHci_CBPage2                                          0x02
#define gpHci_LEMeta                                           0x03
/** @typedef gpHci_EventMaskType_t
 *  @brief  Event Mask type
*/
typedef UInt8                             gpHci_EventMaskType_t;
//@}

/** @enum gpHci_PrivacyMode_t */
//@{
#define gpHci_PrivacyModeNetwork                               0x00
#define gpHci_PrivacyModeDevice                                0x01
#define gpHci_PrivacyModeInvalid                               0x02
/** @typedef gpHci_PrivacyMode_t
 *  @brief Privacy mode
*/
typedef UInt8                             gpHci_PrivacyMode_t;
//@}

/** @enum gpHci_LinkType_t */
//@{
#define gpHci_LinkType_Synchronous                             0x00
#define gpHci_LinkType_Asynchronous                            0x01
#define gpHci_LinkType_Invalid                                 0x02
/** @typedef gpHci_LinkType_t
 *  @brief Data buffer overflow - link type
*/
typedef UInt8                             gpHci_LinkType_t;
//@}

/** @enum gpHci_VsdSinkMode_t */
//@{
#define gpHci_VsdSinkMode_Off                                  0x00
#define gpHci_VsdSinkMode_SinkRxIndication                     0x01
#define gpHci_VsdSinkMode_Null                                 0x02
#define gpHci_VsdSinkMode_Invalid                              0x03
/** @typedef gpHci_VsdSinkMode_t
 *  @brief HCI Mode of the data sink
*/
typedef UInt8                             gpHci_VsdSinkMode_t;
//@}

/** @enum gpHci_SleepMode_t */
//@{
#define gpHci_SleepMode_RC                                     0x00
#define gpHci_SleepMode_32kHzXtal                              0x01
#define gpHci_SleepMode_16Mhz                                  0x02
#define gpHci_SleepMode_Off                                    0x03
#define gpHci_SleepMode_Invalid                                0x04
/** @typedef gpHci_SleepMode_t
 *  @brief Sleep mode for the controller
*/
typedef UInt8                             gpHci_SleepMode_t;
//@}

/** @enum gpHci_Phy_t */
//@{
#define gpHci_Phy_None                                         0x00
#define gpHci_Phy_1Mb                                          0x01
#define gpHci_Phy_2Mb                                          0x02
#define gpHci_Phy_Coded                                        0x03
#define gpHci_Phy_Invalid                                      0x04
/** @typedef gpHci_Phy_t
 *  @brief Phy type used when it is not needed to distinguish between different types of coded phy.
*/
typedef UInt8                             gpHci_Phy_t;
//@}

/** @enum gpHci_PhyWithCoding_t */
//@{
#define gpHci_PhyWithCoding_Reserved                           0x00
#define gpHci_PhyWithCoding_1Mb                                0x01
#define gpHci_PhyWithCoding_2Mb                                0x02
#define gpHci_PhyWithCoding_Coded125kb                         0x03
#define gpHci_PhyWithCoding_Coded500kb                         0x04
#define gpHci_PhyWithCoding_Invalid                            0x05
/** @typedef gpHci_PhyWithCoding_t
 *  @brief Phy type used when it is needed to distinguish between different types of coded phy.
*/
typedef UInt8                             gpHci_PhyWithCoding_t;
//@}

/** @enum gpHci_PhyDirectionMask_t */
//@{
#define gpHci_PhyDirectionMask_Tx                              0x00
#define gpHci_PhyDirectionMask_Rx                              0x01
/** @typedef gpHci_PhyDirectionMask_t
 *  @brief Prefence mask for PHYs.
*/
typedef UInt8                             gpHci_PhyDirectionMask_t;
//@}

/** @enum gpHci_PhyOptions_t */
//@{
#define gpHci_PhyOptions_NoPreference                          0x00
#define gpHci_PhyOptions_S2Coded                               0x01
#define gpHci_PhyOptions_S8Coded                               0x02
#define gpHci_PhyOptions_Reserved                              0x03
/** @typedef gpHci_PhyOptions_t
 *  @brief Phy Options.
*/
typedef UInt16                            gpHci_PhyOptions_t;
//@}

/** @enum gpHci_ModulationIndex_t */
//@{
#define gpHci_ModulationIndex_Standard                         0x00
#define gpHci_ModulationIndex_Stable                           0x01
#define gpHci_ModulationIndex_Invalid                          0x02
/** @typedef gpHci_ModulationIndex_t
 *  @brief Modulation index
*/
typedef UInt8                             gpHci_ModulationIndex_t;
//@}

/** @enum gpHci_ChannelSelectionAlgorithmId_t */
//@{
#define gpHci_ChannelSelectionAlgorithmId1                     0x00
#define gpHci_ChannelSelectionAlgorithmId2                     0x01
#define gpHci_ChannelSelectionAlgorithmIdInvalid               0x02
/** @typedef gpHci_ChannelSelectionAlgorithmId_t
 *  @brief Channel selection algorithm id
*/
typedef UInt8                             gpHci_ChannelSelectionAlgorithmId_t;
//@}

/** @enum gpHci_CteRxPhy_t */
//@{
#define gpHci_CteRxPhyReserved                                 0x00
#define gpHci_CteRxPhy1Mb                                      0x01
#define gpHci_CteRxPhy2Mb                                      0x02
#define gpHci_CteRxPhyInvalid                                  0x03
typedef UInt8                             gpHci_CteRxPhy_t;
//@}

/** @enum gpHci_CteType_t */
//@{
#define gpHci_CteTypeAoAConstantToneExt                        0x00
#define gpHci_CteTypeAoDConstantToneExt1us                     0x01
#define gpHci_CteTypeAoDConstantToneExt2us                     0x02
#define gpHci_CteTypeInvalid                                   0x03
typedef UInt8                             gpHci_CteType_t;
//@}

/** @enum gpHci_CtePacketStatus_t */
//@{
#define gpHci_CtePacketStatusCrcCorrect                        0x00
#define gpHci_CtePacketStatusLengthAndCteTimeUsed              0x01
#define gpHci_CtePacketStatusOther                             0x02
#define gpHci_CtePacketStatusNotSampled                        0xFF
typedef UInt8                             gpHci_CtePacketStatus_t;
//@}

/** @enum gpHci_VsdDataPumpPayloadType_t */
//@{
#define gpHci_VsdDataPumpPayloadTypeFixed                      0x00
#define gpHci_VsdDataPumpPayloadTypeIncrementalInPacket        0x01
#define gpHci_VsdDataPumpPayloadTypeIncrementalPerPacket       0x02
#define gpHci_VsdDataPumpPayloadTypeRandom                     0x03
typedef UInt8                             gpHci_VsdDataPumpPayloadType_t;
//@}

/** @enum gpHci_AdvDataOperation_t */
//@{
#define gpHci_AdvDataOperationIntermediate                     0x00
#define gpHci_AdvDataOperationFirst                            0x01
#define gpHci_AdvDataOperationLast                             0x02
#define gpHci_AdvDataOperationComplete                         0x03
#define gpHci_AdvDataOperationUnchanged                        0x04
#define gpHci_AdvDataOperationInvalid                          0xFF
typedef UInt8                             gpHci_AdvDataOperation_t;
//@}

/** @enum gpHci_PastSyncMode_t */
//@{
#define gpHci_PastSyncModeNothing                              0x00
#define gpHci_PastSyncModeNoReports                            0x01
#define gpHci_PastSyncModeReports                              0x02
#define gpHci_PastSyncModeInvalid                              0x03
typedef UInt8                             gpHci_PastSyncMode_t;
//@}

/** @enum gpHci_TxPower_t */
//@{
#define gpHci_TxPowerInfoNotAvailable                          0x7F
typedef UInt8                             gpHci_TxPower_t;
//@}

/** @enum gpHci_ClockAccuracy_t */
//@{
#define gpHci_ClockAccuracy500ppm                              0x00
#define gpHci_ClockAccuracy250ppm                              0x01
#define gpHci_ClockAccuracy150ppm                              0x02
#define gpHci_ClockAccuracy100ppm                              0x03
#define gpHci_ClockAccuracy75ppm                               0x04
#define gpHci_ClockAccuracy50ppm                               0x05
#define gpHci_ClockAccuracy30ppm                               0x06
#define gpHci_ClockAccuracy20ppm                               0x07
typedef UInt8                             gpHci_ClockAccuracy_t;
//@}

/** @enum gpHci_IsoPacking_t */
//@{
#define gpHci_IsoPackingSequential                             0x00
#define gpHci_IsoPackingInterleaved                            0x01
typedef UInt8                             gpHci_IsoPacking_t;
//@}

/** @enum gpHci_IsoFraming_t */
//@{
#define gpHci_IsoFramingUnframed                               0x00
#define gpHci_IsoFramingFramed                                 0x01
typedef UInt8                             gpHci_IsoFraming_t;
//@}

/** @enum gpHci_PayloadType_t */
//@{
#define gpHci_ZeroLengthPayload                                0x00
#define gpHci_VariableLengthPayload                            0x01
#define gpHci_MaximumLengthPayload                             0x02
/** @typedef gpHci_PayloadType_t
 *  @brief ISO Transmit/Receive Test command : Payload_Type.
*/
typedef UInt8                             gpHci_PayloadType_t;
//@}

/** @enum gpHci_DataPathDirection_t */
//@{
#define gpHci_IsoDataPathDirection_DL                          0x00
#define gpHci_IsoDataPathDirection_UL                          0x01
/** @typedef gpHci_DataPathDirection_t
 *  @brief Setup/Remove ISO Data Path command : Data_Path_Direction
*/
typedef UInt8                             gpHci_DataPathDirection_t;
//@}

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @macro GP_HCI_PB_IDX */
#define GP_HCI_PB_IDX                                12
/** @macro GP_HCI_PB_MASK */
#define GP_HCI_PB_MASK                               0x2000
/** @macro GP_HCI_COMMAND_CREDIT_NUMBER */
/** @brief  The LE Controller provides 1 HCI command credit to the Host */
#define GP_HCI_COMMAND_CREDIT_NUMBER                 1
/** @macro GP_HCI_ADVERTISING_DATA_PAYLOAD_SIZE_MAX */
#define GP_HCI_ADVERTISING_DATA_PAYLOAD_SIZE_MAX     31
/** @macro GP_HCI_EXTADV_DATA_PAYLOAD_SIZE_MAX */
#define GP_HCI_EXTADV_DATA_PAYLOAD_SIZE_MAX          229
/** @macro GP_HCI_SCAN_RESPONSE_DATA_PAYLOAD_SIZE_MAX */
#define GP_HCI_SCAN_RESPONSE_DATA_PAYLOAD_SIZE_MAX     31
/** @macro GP_HCI_FEATURE_SET_SIZE */
#define GP_HCI_FEATURE_SET_SIZE                      8
/** @macro GP_HCI_RANDOM_DATA_LENGTH */
#define GP_HCI_RANDOM_DATA_LENGTH                    8
/** @macro GP_HCI_ENCRYPTION_KEY_LENGTH */
#define GP_HCI_ENCRYPTION_KEY_LENGTH                 16
/** @macro GP_HCI_ENCRYPTION_DATA_LENGTH */
#define GP_HCI_ENCRYPTION_DATA_LENGTH                16
/** @macro GP_HCI_OPCODE_OCF_IDX */
#define GP_HCI_OPCODE_OCF_IDX                        0
/** @macro GP_HCI_OPCODE_OGF_IDX */
#define GP_HCI_OPCODE_OGF_IDX                        10
/** @macro GP_HCI_OPCODE_OCF_MASK */
#define GP_HCI_OPCODE_OCF_MASK                       0x03FF
/** @macro GP_HCI_OPCODE_OGF_MASK */
#define GP_HCI_OPCODE_OGF_MASK                       0xFC00
/** @macro GP_HCI_CHANNEL_MAP_LENGTH */
#define GP_HCI_CHANNEL_MAP_LENGTH                    5
/** @macro GP_HCI_MAX_NR_OF_ADV_REPORTS_IN_EVENT */
#define GP_HCI_MAX_NR_OF_ADV_REPORTS_IN_EVENT        1
/** @macro GP_HCI_AUTHENTICATED_PAYLOAD_TO_DEFAULT */
/** @brief Default 30 seconds (units of 10 ms) */
#define GP_HCI_AUTHENTICATED_PAYLOAD_TO_DEFAULT      0x0BB8
/** @macro GP_HCI_MAX_NR_OF_IQ_SAMPLES */
/** @brief Total IQ samples buffer size = GP_HCI_MAX_NR_OF_IQ_SAMPLES*2 */
#define GP_HCI_MAX_NR_OF_IQ_SAMPLES                  45
/** @macro GP_EXTADV_PRIMARY_INTERVAL_SIZE */
#define GP_EXTADV_PRIMARY_INTERVAL_SIZE              3
/** @macro GP_HCI_SYNC_HANDLE_INVALID */
#define GP_HCI_SYNC_HANDLE_INVALID                   0xFFFF
/** @macro GP_HCI_CONNECTION_HANDLE_INVALID */
#define GP_HCI_CONNECTION_HANDLE_INVALID             0xFFFF
/** @macro GP_HCI_FEATURES_COMMANDS_SIZE */
#define GP_HCI_FEATURES_COMMANDS_SIZE                64
/** @macro GP_HCI_FEATURES_LIST_SIZE */
#define GP_HCI_FEATURES_LIST_SIZE                    8
/** @macro GP_HCI_PHY_MASK_1MB */
#define GP_HCI_PHY_MASK_1MB                          0x01
/** @macro GP_HCI_PHY_MASK_2MB */
#define GP_HCI_PHY_MASK_2MB                          0x02
/** @macro GP_HCI_PHY_MASK_CODED */
#define GP_HCI_PHY_MASK_CODED                        0x04
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_ID_CONNECTABLE */
#define GP_HCI_ADV_EVENT_PROPERTIES_ID_CONNECTABLE     0x00
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_ID_SCANNABLE */
#define GP_HCI_ADV_EVENT_PROPERTIES_ID_SCANNABLE     0x01
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_ID_DIRECTED */
#define GP_HCI_ADV_EVENT_PROPERTIES_ID_DIRECTED      0x02
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_ID_HIGH_DUTY_CYCLE */
#define GP_HCI_ADV_EVENT_PROPERTIES_ID_HIGH_DUTY_CYCLE     0x03
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_ID_LEGACY */
#define GP_HCI_ADV_EVENT_PROPERTIES_ID_LEGACY        0x04
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_ID_ANONYMOUS */
#define GP_HCI_ADV_EVENT_PROPERTIES_ID_ANONYMOUS     0x05
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_ID_INCLUDE_TX_POWER */
#define GP_HCI_ADV_EVENT_PROPERTIES_ID_INCLUDE_TX_POWER     0x06
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_MASK_CONNECTABLE */
#define GP_HCI_ADV_EVENT_PROPERTIES_MASK_CONNECTABLE     (1 << GP_HCI_ADV_EVENT_PROPERTIES_ID_CONNECTABLE)
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_MASK_SCANNABLE */
#define GP_HCI_ADV_EVENT_PROPERTIES_MASK_SCANNABLE     (1 << GP_HCI_ADV_EVENT_PROPERTIES_ID_SCANNABLE)
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_MASK_DIRECTED */
#define GP_HCI_ADV_EVENT_PROPERTIES_MASK_DIRECTED     (1 << GP_HCI_ADV_EVENT_PROPERTIES_ID_DIRECTED)
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_MASK_HIGH_DUTY_CYCLE */
#define GP_HCI_ADV_EVENT_PROPERTIES_MASK_HIGH_DUTY_CYCLE     (1 << GP_HCI_ADV_EVENT_PROPERTIES_ID_HIGH_DUTY_CYCLE)
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_MASK_LEGACY */
#define GP_HCI_ADV_EVENT_PROPERTIES_MASK_LEGACY      (1 << GP_HCI_ADV_EVENT_PROPERTIES_ID_LEGACY)
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_MASK_ANONYMOUS */
#define GP_HCI_ADV_EVENT_PROPERTIES_MASK_ANONYMOUS     (1 << GP_HCI_ADV_EVENT_PROPERTIES_ID_ANONYMOUS)
/** @macro GP_HCI_ADV_EVENT_PROPERTIES_MASK_INCLUDE_TX_POWER */
#define GP_HCI_ADV_EVENT_PROPERTIES_MASK_INCLUDE_TX_POWER     (1 << GP_HCI_ADV_EVENT_PROPERTIES_ID_INCLUDE_TX_POWER)
/** @macro GP_HCI_SWITCH_SAMPLE_RATE_MASK_1US_SWITCH_AOD_TX */
#define GP_HCI_SWITCH_SAMPLE_RATE_MASK_1US_SWITCH_AOD_TX     0x01
/** @macro GP_HCI_SWITCH_SAMPLE_RATE_MASK_1US_SAMPLE_AOD_RX */
#define GP_HCI_SWITCH_SAMPLE_RATE_MASK_1US_SAMPLE_AOD_RX     0x02
/** @macro GP_HCI_SWITCH_SAMPLE_RATE_MASK_1US_SWITCH_SAMPLE_AOA_RX */
#define GP_HCI_SWITCH_SAMPLE_RATE_MASK_1US_SWITCH_SAMPLE_AOA_RX     0x04
/** @macro GP_HCI_SYNC_OPTIONS_ID_USE_PERIODIC_LIST */
#define GP_HCI_SYNC_OPTIONS_ID_USE_PERIODIC_LIST     0x00
/** @macro GP_HCI_SYNC_OPTIONS_ID_REPORTING_INITIALLY_DISABLED */
#define GP_HCI_SYNC_OPTIONS_ID_REPORTING_INITIALLY_DISABLED     0x01
/** @macro GP_HCI_PAST_SYNC_CTE_TYPE_NO_PREFERENCE */
#define GP_HCI_PAST_SYNC_CTE_TYPE_NO_PREFERENCE      0x00
/** @macro GP_HCI_PAST_SYNC_CTE_TYPE_NO_AOA */
#define GP_HCI_PAST_SYNC_CTE_TYPE_NO_AOA             0x01
/** @macro GP_HCI_PAST_SYNC_CTE_TYPE_NO_AOD_1US */
#define GP_HCI_PAST_SYNC_CTE_TYPE_NO_AOD_1US         0x02
/** @macro GP_HCI_PAST_SYNC_CTE_TYPE_NO_AOD_2US */
#define GP_HCI_PAST_SYNC_CTE_TYPE_NO_AOD_2US         0x04
/** @macro GP_HCI_PAST_SYNC_CTE_TYPE_NOTHING_WITHOUT_CTE */
#define GP_HCI_PAST_SYNC_CTE_TYPE_NOTHING_WITHOUT_CTE     0x10
/** @macro GP_HCI_CODEC_ID_LENGTH */
#define GP_HCI_CODEC_ID_LENGTH                       5
/** @macro GP_HCI_CONTROLLER_DELAY_LENGTH */
#define GP_HCI_CONTROLLER_DELAY_LENGTH               3
/** @macro GP_HCI_CODEC_CONFIGURATION_MAX_LENGTH */
/** @brief HCI 7.8.109 LE Setup ISO Data Path */
#define GP_HCI_CODEC_CONFIGURATION_MAX_LENGTH        242
/** @macro GP_HCI_MAX_NUM_SUPPORTED_STD_CODECS */
#define GP_HCI_MAX_NUM_SUPPORTED_STD_CODECS          4
/** @macro GP_HCI_MAX_NUM_SUPPORTED_VSD_CODECS */
#define GP_HCI_MAX_NUM_SUPPORTED_VSD_CODECS          4
/** @macro GP_HCI_MAX_NUM_CODEC_CAPABILITIES */
#define GP_HCI_MAX_NUM_CODEC_CAPABILITIES            4
/** @macro GP_HCI_CODEC_CAPABILITY_MAX_LENGTH */
/** @brief TBD - check BAP/CAP */
#define GP_HCI_CODEC_CAPABILITY_MAX_LENGTH           50
/** @macro GP_HCI_OGF_LC */
/** @brief Link Control */
#define GP_HCI_OGF_LC                                0x01
/** @macro GP_HCI_OGF_LP */
/** @brief Link Policy */
#define GP_HCI_OGF_LP                                0x02
/** @macro GP_HCI_OGF_CB */
/** @brief Controller and Baseband */
#define GP_HCI_OGF_CB                                0x03
/** @macro GP_HCI_OGF_IP */
/** @brief Informational parameters */
#define GP_HCI_OGF_IP                                0x04
/** @macro GP_HCI_OGF_SP */
/** @brief Status Parameters */
#define GP_HCI_OGF_SP                                0x05
/** @macro GP_HCI_OGF_TC */
/** @brief Testing Commands */
#define GP_HCI_OGF_TC                                0x06
/** @macro GP_HCI_OGF_EV */
/** @brief Events */
#define GP_HCI_OGF_EV                                0x07
/** @macro GP_HCI_OGF_LE */
/** @brief LE Controller Commands */
#define GP_HCI_OGF_LE                                0x08
/** @macro GP_HCI_OGF_VS */
/** @brief VSD commands */
#define GP_HCI_OGF_VS                                0x3F
/** @macro GP_HCI_CIG_ID_INVALID */
#define GP_HCI_CIG_ID_INVALID                        0xFF
/** @macro GP_HCI_CIS_ID_INVALID */
#define GP_HCI_CIS_ID_INVALID                        0xFF
/** @macro GP_HCI_ISODATAPATHID_HCI */
#define GP_HCI_ISODATAPATHID_HCI                     0
/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/** @macro GP_HCI_RESULT_VALID(result) */
#define GP_HCI_RESULT_VALID(result)                  (result != gpHci_ResultInvalid)
/** @macro GP_HCI_IS_DIRECTED_ADV_TYPE(type) */
/** @brief  PDU type related macro's */
#define GP_HCI_IS_DIRECTED_ADV_TYPE(type)            (type == gpHci_AdvertisingType_ConnectableHighDutyCycleDirected || type == gpHci_AdvertisingType_ConnectableLowDutyCycleDirected)
/** @macro GP_HCI_IS_UNIDIRECTED_ADV_TYPE(type) */
#define GP_HCI_IS_UNIDIRECTED_ADV_TYPE(type)         (!GP_HCI_IS_DIRECTED_ADV_TYPE(type))
/** @macro GP_HCI_IS_ADV_TYPE_SPECIAL_ADV_INT(type) */
#define GP_HCI_IS_ADV_TYPE_SPECIAL_ADV_INT(type)     (type == gpHci_AdvertisingType_ScannableUndirected || type == gpHci_AdvertisingType_NonConnectableUndirected)
/** @macro  GP_HCI_IS_ADV_TYPE_HIGH_DUTY_CYCLE_DIRECTED(type) */
#define  GP_HCI_IS_ADV_TYPE_HIGH_DUTY_CYCLE_DIRECTED(type)     (type == gpHci_AdvertisingType_ConnectableHighDutyCycleDirected)
/** @macro GP_HCI_PHY_TYPE_VALID(type) */
#define GP_HCI_PHY_TYPE_VALID(type)                  (type < gpHci_Phy_Invalid && type > gpHci_Phy_None)
/** @macro GP_HCI_ENHANCED_PHY_TYPE_VALID(type) */
#define GP_HCI_ENHANCED_PHY_TYPE_VALID(type)         (type < gpHci_PhyWithCoding_Invalid && type > gpHci_PhyWithCoding_Reserved)
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt8 gpHci_AdvertisingHandle_t;

typedef UInt16 gpHci_ConnectionHandle_t;

typedef UInt16 gpHci_SyncHandle_t;

typedef UInt8 gpHci_ExtendedAdvertisingReportEventType_t;

typedef UInt64 gpHci_EventMask_t;

typedef UInt8 gpHci_SupportedRatesMask_t;

typedef UInt8 gpHci_VsdSubEventMask_t;

/** @typedef gpHci_SyncCteMask_t
 *  @brief bit 0 = no AoA, bit 1 = no AoD 1 us, bit 2 = no AoD 2 us, bit 4 = none without CTE
*/
typedef UInt8 gpHci_SyncCteMask_t;

typedef UInt8 gpHci_SyncOptions_t;

typedef UInt8 gpHci_CigId_t;

typedef UInt8 gpHci_CisId_t;

/** @struct gpHci_PhyMask_t */
typedef struct {
    /** @brief bit 0 = 1 mbit PHY, bit 1 = 2 mbit PHY, bit 2 = coded PHY */
    UInt8                          mask;
} gpHci_PhyMask_t;

/** @struct gpHci_ChannelMap_t */
typedef struct {
    UInt8                          channels[GP_HCI_CHANNEL_MAP_LENGTH];
} gpHci_ChannelMap_t;

/** @struct gpHci_ReadLocalVersionInfoParameters_t */
typedef struct {
    UInt8                          hciVersion;
    UInt16                         hciRevision;
    UInt8                          lmppalVersion;
    UInt16                         manufacturerName;
    UInt16                         lmppalSubversion;
} gpHci_ReadLocalVersionInfoParameters_t;

/** @struct gpHci_ReadLocalSupportedCommands_t */
typedef struct {
    UInt8                          supportedCommands[GP_HCI_FEATURES_COMMANDS_SIZE];
} gpHci_ReadLocalSupportedCommands_t;

/** @struct gpHci_ReadLocalSupportedFeatures_t */
typedef struct {
    UInt8                          supportedFeatures[GP_HCI_FEATURES_LIST_SIZE];
} gpHci_ReadLocalSupportedFeatures_t;

/** @struct gpHci_AdvancedTxPower_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
    Int8                           advancedTxPower;
} gpHci_AdvancedTxPower_t;

/** @struct gpHci_ReadChannelMapReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
    gpHci_ChannelMap_t             channelMap;
} gpHci_ReadChannelMapReturn_t;

/** @struct gpHci_LeReadAntennaInformationReturn_t */
typedef struct {
    gpHci_Result_t                 status;
    gpHci_SupportedRatesMask_t     supportedSwitchingSamplingRates;
    UInt8                          nrOfAntennae;
    UInt8                          maxSwitchPatternLength;
    UInt8                          maxCteLength;
} gpHci_LeReadAntennaInformationReturn_t;

/** @struct gpHci_EncryptReturn_t */
typedef struct {
    UInt8                          encryptedData[GP_HCI_ENCRYPTION_DATA_LENGTH];
} gpHci_EncryptReturn_t;

/** @struct gpHci_RandReturn_t */
typedef struct {
    UInt8                          randomNumber[GP_HCI_RANDOM_DATA_LENGTH];
} gpHci_RandReturn_t;

/** @struct gpHci_ReadConnectionAcceptTimeoutReturn_t */
typedef struct {
    UInt16                         connectionAcceptTimeout;
} gpHci_ReadConnectionAcceptTimeoutReturn_t;

/** @struct gpHci_ReadAuthenticatedPayloadTOReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
    UInt16                         authenticatedPayloadTO;
} gpHci_ReadAuthenticatedPayloadTOReturn_t;

/** @struct gpHci_WriteAuthenticatedPayloadTOReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
} gpHci_WriteAuthenticatedPayloadTOReturn_t;

/** @struct gpHci_ReadMaxDataLength_t */
typedef struct {
    UInt16                         supportedMaxTxOctets;
    UInt16                         supportedMaxTxTime;
    UInt16                         supportedMaxRxOctets;
    UInt16                         supportedMaxRxTime;
} gpHci_ReadMaxDataLength_t;

/** @struct gpHci_LEReadBufferSize_t */
typedef struct {
    UInt16                         ACLDataPacketLength;
    UInt8                          totalNumDataPackets;
} gpHci_LEReadBufferSize_t;

/** @struct gpHci_LeReadBufferSize_v2_t */
typedef struct {
    UInt16                         leAclDataPacketLength;
    UInt8                          totalNumLeAclDataPackets;
    UInt16                         isoDataPacketLength;
    UInt8                          totalNumIsoDataPackets;
} gpHci_LeReadBufferSize_v2_t;

/** @struct gpHci_NumberOfCompletedPackets_t */
typedef struct {
    /** @brief  "Always one handle.."  */
    UInt8                          nrOfHandles;
    /** @brief  "nrOfHciPackets always one.."  */
    gpHci_ConnectionHandle_t       handle;
    UInt16                         nrOfHciPackets;
} gpHci_NumberOfCompletedPackets_t;

/** @struct gpHci_DisconnectCompleteParams_t */
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
    gpHci_Result_t                 reason;
} gpHci_DisconnectCompleteParams_t;

/** @struct gpHci_EncryptionChangeParams_t */
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
    gpHci_EncryptionLevel_t        encryptionEnabled;
} gpHci_EncryptionChangeParams_t;

/** @struct gpHci_ReadRemoteVersionInfoComplete_t */
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
    UInt8                          versionNr;
    UInt16                         companyId;
    UInt16                         subVersionNr;
} gpHci_ReadRemoteVersionInfoComplete_t;

/** @struct gpHci_EncryptionKeyRefreshComplete_t */
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
} gpHci_EncryptionKeyRefreshComplete_t;

/** @struct gpHci_LEReadPhy_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
    gpHci_Phy_t                    txPhy;
    gpHci_Phy_t                    rxPhy;
} gpHci_LEReadPhy_t;

/** @struct gpHci_VsdGetBuildP4Changelist_t */
typedef struct {
    UInt32                         changelistNumber;
} gpHci_VsdGetBuildP4Changelist_t;

/** @struct gpHci_VsdGetRtMgrVersion_t */
typedef struct {
    UInt8                          version;
} gpHci_VsdGetRtMgrVersion_t;

/** @struct gpHci_LeReadTransmitPowerReturn_t */
typedef struct {
    Int8                           Min_Tx_Power;
    Int8                           Max_Tx_Power;
} gpHci_LeReadTransmitPowerReturn_t;

/** @struct gpHci_LeReadRfPathCompensationReturn_t */
typedef struct {
    Int16                          RF_Tx_Path_Compensation_Value;
    Int16                          RF_Rx_Path_Compensation_Value;
} gpHci_LeReadRfPathCompensationReturn_t;

/** @struct gpHci_VsdRandDataReturn_t */
typedef struct {
    UInt8                          RNG_Source;
    UInt8                          numRandomBytes;
    UInt8*                         pRandomData;
} gpHci_VsdRandDataReturn_t;

/** @struct gpHci_LeReadSuggestedDefDataLengthManual_t */
typedef struct {
    UInt16                         suggestedMaxTxOctets;
    UInt16                         suggestedMaxTxTime;
} gpHci_LeReadSuggestedDefDataLengthManual_t;

/** @struct gpHci_LeSetCigParametersReturn_t */
typedef struct {
    gpHci_CigId_t                  cigId;
    UInt8                          cisCount;
    gpHci_ConnectionHandle_t       connHandles[8];
} gpHci_LeSetCigParametersReturn_t;

/** @struct gpHci_LeRejectCisReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connHandle;
} gpHci_LeRejectCisReturn_t;

/** @struct gpHci_CodecId_t
 *  @brief HCI $7.4.10 ReadLocalSupportedCodecCapabilities
*/
typedef struct {
    /** @brief HCI $7.4.10: if StandardCodecId != 0xFF then rest is ignored */
    UInt8                          StandardCodecId;
    UInt16                         companyId;
    UInt16                         VendorDefinedCodecId;
} gpHci_CodecId_t;

/** @struct gpHci_SupportedStandardCodec_t */
typedef struct {
    UInt8                          StandardCodecId;
    UInt8                          StandardCodecTransport;
} gpHci_SupportedStandardCodec_t;

/** @struct gpHci_VendorSpecificCodecId_t
 *  @brief HCI $7.4.8 return params use VendorSpecificCodecId in an array - unlike $7.4.10 and $7.4.11
*/
typedef struct {
    UInt16                         companyId;
    UInt16                         VendorDefinedCodecId;
} gpHci_VendorSpecificCodecId_t;

/** @struct gpHci_SupportedVendorSpecificCodec_t */
typedef struct {
    gpHci_VendorSpecificCodecId_t  VendorSpecificCodecId_;
    UInt8                          VendorSpecificCodecTransport;
} gpHci_SupportedVendorSpecificCodec_t;

/** @struct gpHci_ReadLocalSupportedCodecsReturn_t */
typedef struct {
    UInt8                          NumSupportedStandardCodecs;
    gpHci_SupportedStandardCodec_t SupportedStandardCodecs[GP_HCI_MAX_NUM_SUPPORTED_STD_CODECS];
    UInt8                          NumSupportedVendorSpecificCodecs;
    gpHci_SupportedVendorSpecificCodec_t SupportedVendorSpecificCodecs[GP_HCI_MAX_NUM_SUPPORTED_VSD_CODECS];
} gpHci_ReadLocalSupportedCodecsReturn_t;

/** @struct gpHci_CodecCapability_t */
typedef struct {
    UInt8                          CodecCapabilityLength;
    UInt8                          CodecCapability[GP_HCI_CODEC_CAPABILITY_MAX_LENGTH];
} gpHci_CodecCapability_t;

/** @struct gpHci_ReadLocalSupportedCodecCapabilitiesReturn_t */
typedef struct {
    UInt8                          NumCodecCapabilities;
    gpHci_CodecCapability_t        CodecCapabilities[GP_HCI_MAX_NUM_CODEC_CAPABILITIES];
} gpHci_ReadLocalSupportedCodecCapabilitiesReturn_t;

/** @struct gpHci_ControllerDelay_t
 *  @brief HCI $7.4.11 Read Local Supported Controller Delay
*/
typedef struct {
    UInt8                          data[GP_HCI_CONTROLLER_DELAY_LENGTH];
} gpHci_ControllerDelay_t;

/** @struct gpHci_ReadLocalSupportedControllerDelayReturn_t */
typedef struct {
    gpHci_ControllerDelay_t        MinControllerDelay;
    gpHci_ControllerDelay_t        MaxControllerDelay;
} gpHci_ReadLocalSupportedControllerDelayReturn_t;

/** @struct gpHci_LeSetupIsoDataPathReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connHandle;
} gpHci_LeSetupIsoDataPathReturn_t;

/** @struct gpHci_LeRemovesoDataPathReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connHandle;
} gpHci_LeRemovesoDataPathReturn_t;

/** @struct gpHci_LeReadIsoLinkQualityReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connHandle;
    UInt32                         TxUnackedPackets;
    UInt32                         TxFlushedPackets;
    UInt32                         TxLastSubeventPackets;
    UInt32                         RetransmittedPackets;
    UInt32                         CrcErrorPackets;
    UInt32                         RxUnreceivedPackets;
    UInt32                         Duplicate_Packets;
} gpHci_LeReadIsoLinkQualityReturn_t;

/** @struct gpHci_LeReadIsoTxSyncReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connHandle;
    UInt16                         PacketSequenceNumber;
    UInt32                         TxTimeStamp;
    UInt8                          TimeOffset[3];
} gpHci_LeReadIsoTxSyncReturn_t;

/** @struct gpHci_LeIsoTxTestReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connHandle;
} gpHci_LeIsoTxTestReturn_t;

/** @struct gpHci_LeIsoRxTestReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connHandle;
} gpHci_LeIsoRxTestReturn_t;

/** @struct gpHci_LeIsoReadTestCountersReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connHandle;
    UInt32                         RxedSduCount;
    UInt32                         MissedSduCount;
    UInt32                         FailedSduCount;
} gpHci_LeIsoReadTestCountersReturn_t;

/** @struct gpHci_LeIsoTestEndReturn_t */
typedef struct {
    gpHci_ConnectionHandle_t       connHandle;
    UInt32                         RxedSduCount;
    UInt32                         MissedSduCount;
    UInt32                         FailedSduCount;
} gpHci_LeIsoTestEndReturn_t;

/** @union gpHci_CommandCompleteParamsUnion_t */
typedef union {
    Int8                           advChannelTxPower;
    gpHci_EncryptReturn_t          encryptedData;
    gpHci_RandReturn_t             randData;
    UInt8                          whiteListSize;
    gpHci_LeReadSuggestedDefDataLengthManual_t readSuggestedDefDataLength;
    gpHci_ReadMaxDataLength_t      readMaxDataLength;
    gpHci_LEReadBufferSize_t       leReadBufferSize;
    gpHci_LeReadBufferSize_v2_t    leReadBufferSize_v2;
    gpHci_ReadLocalVersionInfoParameters_t leReadLocalVersion;
    gpHci_ReadConnectionAcceptTimeoutReturn_t cbReadConnectionAcceptTimeout;
    gpHci_ReadAuthenticatedPayloadTOReturn_t readAuthenticatedPayloadTO;
    gpHci_WriteAuthenticatedPayloadTOReturn_t writeAuthenticatedPayloadTO;
    BtDeviceAddress_t              bdAddress;
    gpHci_ConnectionHandle_t       connectionHandle;
    gpHci_ReadLocalSupportedCommands_t supportedCommands;
    gpHci_ReadLocalSupportedFeatures_t supportedFeatures;
    gpHci_AdvancedTxPower_t        advanceTxPower;
    gpHci_ReadChannelMapReturn_t   leReadChannelMap;
    gpHci_LEReadPhy_t              leReadPhy;
    gpHci_VsdGetBuildP4Changelist_t vsdGetBuildP4Changelist;
    UInt16                         maxSupportedAdvDataLength;
    UInt8                          numberOfSupportedAdvSets;
    gpHci_CigId_t                  cigId;
    /** @brief direct test mode result */
    UInt16                         testResult;
    UInt8                          periodicAdvertiserListSize;
    Int8                           txPower;
    gpHci_LeReadAntennaInformationReturn_t leReadAntennaInformation;
    UInt8                          resolvingListSize;
    gpHci_LeReadTransmitPowerReturn_t LeReadTransmitPower;
    gpHci_LeReadRfPathCompensationReturn_t LeReadRfPathCompensation;
    UInt32                         accessAddress;
    gpHci_VsdRandDataReturn_t      VsdRandData;
    gpHci_VsdGetRtMgrVersion_t     RtMgrVersion;
} gpHci_CommandCompleteParamsUnion_t;

/** @struct gpHci_CommandCompleteParams_t */
typedef struct {
    UInt8                          numHciCmdPackets;
    gpHci_CommandOpCode_t          opCode;
    gpHci_Result_t                 result;
    gpHci_CommandCompleteParamsUnion_t returnParams;
} gpHci_CommandCompleteParams_t;

/** @struct gpHci_CommandStatusParams_t */
typedef struct {
    gpHci_Result_t                 status;
    UInt8                          numHciCmdPackets;
    gpHci_CommandOpCode_t          opCode;
} gpHci_CommandStatusParams_t;

/** @struct gpHci_ConnectionCompleteParams_t */
typedef struct {
    gpHci_Result_t                 status;
    UInt16                         connectionHandle;
    BtDeviceAddress_t              bdAddress;
    UInt8                          linkType;
    UInt8                          encryptionEnabled;
} gpHci_ConnectionCompleteParams_t;

/** @struct gpHci_LEConnectionCompleteEventParams_t */
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
    gpHci_ConnectionRole_t         role;
    gpHci_AdvPeerAddressType_t     peerAddressType;
    BtDeviceAddress_t              peerAddress;
    UInt16                         connInterval;
    UInt16                         connLatency;
    UInt16                         supervisionTo;
    gpHci_ClockAccuracy_t          masterClockAccuracy;
} gpHci_LEConnectionCompleteEventParams_t;

/** @struct gpHci_LEConnectionUpdateCompleteEventParams_t */
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
    UInt16                         connInterval;
    UInt16                         connLatency;
    UInt16                         supervisionTo;
} gpHci_LEConnectionUpdateCompleteEventParams_t;

/** @struct gpHci_LEReadRemoteFeaturesCompleteParams_t */
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
    UInt8                          features[GP_HCI_FEATURE_SET_SIZE];
} gpHci_LEReadRemoteFeaturesCompleteParams_t;

/** @struct gpHci_LERemoteConnectionParamsEventParams_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
    UInt16                         connIntervalMin;
    UInt16                         connIntervalMax;
    UInt16                         connLatency;
    UInt16                         supervisionTimeout;
} gpHci_LERemoteConnectionParamsEventParams_t;

/** @struct gpHci_LEEnhancedConnectionCompleteEventParams_t */
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
    gpHci_ConnectionRole_t         role;
    gpHci_InitPeerAddressType_t    peerAddressType;
    BtDeviceAddress_t              peerAddress;
    BtDeviceAddress_t              localPrivateAddress;
    BtDeviceAddress_t              peerPrivateAddress;
    UInt16                         connInterval;
    UInt16                         connLatency;
    UInt16                         supervisionTo;
    gpHci_ClockAccuracy_t          masterClockAccuracy;
} gpHci_LEEnhancedConnectionCompleteEventParams_t;

/** @struct gpHci_LELongTermKeyRequestParams_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
    UInt8                          randomNumber[GP_HCI_RANDOM_DATA_LENGTH];
    UInt16                         encryptedDiversifier;
} gpHci_LELongTermKeyRequestParams_t;

/** @struct gpHci_ReportDataUndirected_t */
typedef struct {
    UInt8                          dataLength;
    UInt8                          data[31];
    UInt8                          rssi;
} gpHci_ReportDataUndirected_t;

/** @struct gpHci_ReportDataDirected_t */
typedef struct {
    gpHci_InitPeerAddressType_t    directAddressType;
    BtDeviceAddress_t              directAddress;
    UInt8                          rssi;
} gpHci_ReportDataDirected_t;

/** @union gpHci_ReportData_t */
typedef union {
    gpHci_ReportDataDirected_t     directed;
    gpHci_ReportDataUndirected_t   undirected;
} gpHci_ReportData_t;

/** @struct gpHci_ReportPayload_t */
typedef struct {
    gpHci_AdvertisingReportEventType_t eventType;
    gpHci_InitPeerAddressType_t    addressType;
    BtDeviceAddress_t              address;
    gpHci_ReportData_t             data;
} gpHci_ReportPayload_t;

/** @struct gpHci_ExtReportPayload_t */
typedef struct {
    gpHci_ExtAdvEventType_t        eventType;
    gpHci_InitPeerAddressType_t    addressType;
    BtDeviceAddress_t              address;
    UInt8                          primaryPhy;
    UInt8                          secondaryPhy;
    UInt8                          advertisingSID;
    UInt8                          txPower;
    UInt8                          rssi;
    UInt16                         periodicAdvertisingInterval;
    gpHci_DirectAddressType_t      directAddressType;
    BtDeviceAddress_t              directAddress;
    UInt8                          dataLength;
    UInt8*                         data;
} gpHci_ExtReportPayload_t;

/** @struct gpHci_LeMetaAdvertisingReportParams_t */
typedef struct {
    UInt8                          nrOfReports;
    gpHci_ReportPayload_t          reports[GP_HCI_MAX_NR_OF_ADV_REPORTS_IN_EVENT];
} gpHci_LeMetaAdvertisingReportParams_t;

/** @struct gpHci_LeMetaExtAdvertisingReportParams_t */
typedef struct {
    UInt8                          nrOfReports;
    gpHci_ExtReportPayload_t       reports[GP_HCI_MAX_NR_OF_ADV_REPORTS_IN_EVENT];
} gpHci_LeMetaExtAdvertisingReportParams_t;

/** @struct gpHci_LeMetaDataLengthChange_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
    UInt16                         MaxTxOctets;
    UInt16                         MaxTxTime;
    UInt16                         MaxRxOctets;
    UInt16                         MaxRxTime;
} gpHci_LeMetaDataLengthChange_t;

/** @struct gpHci_LEPhyUpdateCompleteEventParams_t */
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
    gpHci_Phy_t                    txPhy;
    gpHci_Phy_t                    rxPhy;
} gpHci_LEPhyUpdateCompleteEventParams_t;

/** @struct gpHci_LeAdvertisingSetTerminatedEventParams_t */
typedef struct {
    gpHci_Result_t                 status;
    /** @brief  handle to identify an advertisement set  */
    UInt8                          advertisingHandle;
    gpHci_ConnectionHandle_t       connectionHandle;
    UInt8                          numCompletedExtAdvEvents;
} gpHci_LeAdvertisingSetTerminatedEventParams_t;

/** @struct gpHci_LEScanRequestReceivedEventParams_t */
typedef struct {
    /** @brief  handle to identify an advertisement set  */
    UInt8                          advertisingHandle;
    gpHci_AdvertiserAddressType_t  scannerAddressType;
    BtDeviceAddress_t              scannerAddress;
} gpHci_LEScanRequestReceivedEventParams_t;

/** @struct gpHci_LEChannelSelectionAlgorithmEventParams_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
    gpHci_ChannelSelectionAlgorithmId_t selectedAlgorithm;
} gpHci_LEChannelSelectionAlgorithmEventParams_t;

/** @struct gpHci_IQSample_t */
typedef struct {
    UInt8                          I_Sample;
    UInt8                          Q_Sample;
} gpHci_IQSample_t;

/** @struct gpHci_LEConnectionlessIqReportEventParams_t */
typedef struct {
    gpHci_ConnectionHandle_t       syncHandle;
    UInt8                          channelIndex;
    Int16                          rssi;
    UInt8                          rssi_antenna_id;
    gpHci_CteType_t                cteType;
    UInt8                          slotDurations;
    gpHci_CtePacketStatus_t        packetStatus;
    UInt16                         paEventCounter;
    UInt8                          sampleCount;
    gpHci_IQSample_t               iqSamples[GP_HCI_MAX_NR_OF_IQ_SAMPLES];
} gpHci_LEConnectionlessIqReportEventParams_t;

/** @struct gpHci_LEConnectionIqReportEventParams_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
    gpHci_CteRxPhy_t               rxPhy;
    UInt8                          dataChannelIndex;
    Int16                          rssi;
    UInt8                          rssi_antenna_id;
    gpHci_CteType_t                cteType;
    UInt8                          slotDurations;
    gpHci_CtePacketStatus_t        packetStatus;
    UInt16                         connEventCounter;
    UInt8                          sampleCount;
    gpHci_IQSample_t               iqSamples[GP_HCI_MAX_NR_OF_IQ_SAMPLES];
} gpHci_LEConnectionIqReportEventParams_t;

/** @struct gpHci_LECteRequestFailedEventParams_t */
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
} gpHci_LECteRequestFailedEventParams_t;

/** @struct gpHci_LePeriodicAdvertisingSyncEstablishedEvent_t
 *  @brief BLE Spec: 7.7.65.14 LE Periodic Advertising Sync Established Event
*/
typedef struct {
    gpHci_Result_t                 status;
    gpHci_SyncHandle_t             syncHandle;
    UInt8                          advertisingSID;
    gpHci_AdvertiserAddressType_t  advertiserAddressType;
    BtDeviceAddress_t              advertiserAddress;
    gpHci_Phy_t                    advertiserPHY;
    UInt16                         periodicAdvertisingInterval;
    gpHci_ClockAccuracy_t          advertiserClockAccuracy;
} gpHci_LePeriodicAdvertisingSyncEstablishedEvent_t;

/** @struct gpHci_LePeriodicAdvertisingReportEvent_t
 *  @brief BLE Spec: 7.7.65.15 LE Periodic Advertising Report Event
*/
typedef struct {
    gpHci_SyncHandle_t             syncHandle;
    UInt8                          txPower;
    UInt8                          rssi;
    UInt8                          unused;
    UInt8                          dataStatus;
    UInt8                          dataLength;
    UInt8*                         data;
} gpHci_LePeriodicAdvertisingReportEvent_t;

/** @struct gpHci_LePeriodicAdvertisingSyncLostEvent_t
 *  @brief 7.7.65.16 LE Periodic Advertising Sync Lost Event
*/
typedef struct {
    gpHci_SyncHandle_t             syncHandle;
} gpHci_LePeriodicAdvertisingSyncLostEvent_t;

/** @struct gpHci_LePeriodicAdvertisingSyncTransferReceivedEvent_t
 *  @brief 7.7.65.24 LE Periodic Advertising Sync Transfer Received Event
*/
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
    UInt16                         serviceData;
    gpHci_SyncHandle_t             syncHandle;
    UInt8                          advertisingSID;
    gpHci_AdvertiserAddressType_t  advertiserAddressType;
    BtDeviceAddress_t              advertiserAddress;
    gpHci_Phy_t                    advertiserPhy;
    UInt16                         periodicAdvertisingInterval;
    gpHci_ClockAccuracy_t          advertiserClockAccuracy;
} gpHci_LePeriodicAdvertisingSyncTransferReceivedEvent_t;

/** @struct gpHci_LeCisEstablishedEvent_t
 *  @brief 7.7.65.25 LE CIS Established Event
*/
typedef struct {
    gpHci_Result_t                 status;
    gpHci_ConnectionHandle_t       connectionHandle;
    UInt24                         cigSyncDelay;
    UInt24                         cisSyncDelay;
    UInt24                         transportLatencyCtoP;
    UInt24                         transportLatencyPtoC;
    gpHci_Phy_t                    phyCtoP;
    gpHci_Phy_t                    phyPtoC;
    UInt8                          nse;
    UInt8                          bnCtoP;
    UInt8                          bnPtoC;
    UInt8                          ftCtoP;
    UInt8                          ftPtoC;
    UInt16                         maxPduCtoP;
    UInt16                         maxPduPtoC;
    UInt16                         isoIntervalUnit;
} gpHci_LeCisEstablishedEvent_t;

/** @struct gpHci_LeCisRequestEvent_t
 *  @brief 7.7.65.26 LE CIS Request Event
*/
typedef struct {
    gpHci_ConnectionHandle_t       aclConnectionHandle;
    gpHci_ConnectionHandle_t       cisConnectionHandle;
    gpHci_CigId_t                  cigId;
    gpHci_CisId_t                  cisId;
} gpHci_LeCisRequestEvent_t;

/** @union gpHci_LEMetaInfo_t */
typedef union {
    UInt8                          tmp;
    gpHci_LEConnectionCompleteEventParams_t connectionComplete;
    gpHci_LEReadRemoteFeaturesCompleteParams_t readRemoteFeaturesComplete;
    gpHci_LELongTermKeyRequestParams_t longTermKeyRequest;
    gpHci_LeMetaAdvertisingReportParams_t advReport;
    gpHci_LeMetaExtAdvertisingReportParams_t extAdvReport;
    gpHci_LeMetaDataLengthChange_t dataLengthChange;
    gpHci_LEConnectionUpdateCompleteEventParams_t connectionUpdateComplete;
    gpHci_LERemoteConnectionParamsEventParams_t remoteConnectionParams;
    gpHci_LEEnhancedConnectionCompleteEventParams_t enhancedConnectionComplete;
    gpHci_LEPhyUpdateCompleteEventParams_t phyUpdateComplete;
    gpHci_LEChannelSelectionAlgorithmEventParams_t channelSelectionAlgorithm;
    gpHci_LEConnectionlessIqReportEventParams_t connectionlessIqReport;
    gpHci_LEConnectionIqReportEventParams_t connectionIqReport;
    gpHci_LECteRequestFailedEventParams_t cteRequestFailed;
    gpHci_LeAdvertisingSetTerminatedEventParams_t advSetTerminated;
    gpHci_LEScanRequestReceivedEventParams_t scanRequestReceived;
    gpHci_LePeriodicAdvertisingSyncEstablishedEvent_t perAdvSyncEstablished;
    gpHci_LePeriodicAdvertisingReportEvent_t perAdvAdvReport;
    gpHci_LePeriodicAdvertisingSyncLostEvent_t perAdvSyncLost;
    gpHci_LePeriodicAdvertisingSyncTransferReceivedEvent_t pastReceived;
    gpHci_LeCisEstablishedEvent_t  cisEstablished;
    gpHci_LeCisRequestEvent_t      cisRequest;
} gpHci_LEMetaInfo_t;

/** @struct gpHci_LEMetaEventParams_t */
typedef struct {
    gpHci_LEMetaSubEventCode_t     subEventCode;
    gpHci_LEMetaInfo_t             params;
} gpHci_LEMetaEventParams_t;

/** @struct gpHci_VsdMetaConnEventCountParams_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
    UInt16                         connEvtCount;
} gpHci_VsdMetaConnEventCountParams_t;

/** @struct gpHci_VsdMetaWhiteListModified_t */
typedef struct {
    BtDeviceAddress_t              address;
    Bool                           isAdded;
} gpHci_VsdMetaWhiteListModified_t;

/** @struct gpHci_VsdMetaEventProcessedParams_t */
typedef struct {
    UInt8                          connId;
    UInt16                         eventCounter;
    UInt32                         tsLastValidPacketReceived;
    UInt32                         tsLastPacketReceived;
} gpHci_VsdMetaEventProcessedParams_t;

/** @union gpBle_DF_Algo_Result_t */
typedef union {
    UInt8                          DummyResult;
} gpBle_DF_Algo_Result_t;

/** @union gpHci_VsdMetaInfo_t */
typedef union {
    UInt8                          tmp;
    gpHci_VsdMetaConnEventCountParams_t ConnEventCount;
    gpHci_VsdMetaWhiteListModified_t whiteListModified;
    gpHci_VsdMetaEventProcessedParams_t eventProcessed;
} gpHci_VsdMetaInfo_t;

/** @struct gpHci_VsdMetaEventParams_t */
typedef struct {
    gpHci_VsdSubEventCode_t        subEventCode;
    gpHci_VsdMetaInfo_t            params;
} gpHci_VsdMetaEventParams_t;

/** @struct gpHci_AuthenticatedPayloadToExpired_t */
typedef struct {
    gpHci_ConnectionHandle_t       connectionHandle;
} gpHci_AuthenticatedPayloadToExpired_t;

/** @struct gpHci_VsdSinkRxIndication_t */
typedef struct {
    /** @brief  "Connection Handle"  */
    gpHci_ConnectionHandle_t       connHandle;
    /** @brief  "Length of the received data"  */
    UInt16                         dataLength;
    /** @brief  "RX timestamp"  */
    UInt32                         rxTs;
} gpHci_VsdSinkRxIndication_t;

/** @struct gpHci_VsdOverruleRemoteMaxRxOctetsAndTime_t */
typedef struct {
    /** @brief  "Connection Handle"  */
    gpHci_ConnectionHandle_t       connHandle;
    UInt16                         maxRxOctetsRemote;
    UInt16                         maxRxTimeRemote;
} gpHci_VsdOverruleRemoteMaxRxOctetsAndTime_t;

/** @struct gpHci_SetExtendedAdvScanRspData_t */
typedef struct {
    /** @brief  handle to identify an advertisement set  */
    UInt8                          advertisingHandle;
    /** @brief  value 0x00 intermediate fragment, 0x01 first or continued fragment, 0x02 last fragment, 0x03 complete extended adv data, 0x04 unchanged data (update DID)  */
    UInt8                          operation;
    /** @brief  0x00 fragmentation allowed, 0x01 fragmentation not allowed  */
    UInt8                          fragmentPreference;
    UInt8                          dataLength;
    UInt8*                         data;
} gpHci_SetExtendedAdvScanRspData_t;

/** @struct gpHci_ExtendedAdvEnableData_t
 *  @brief handles to identify an advertisement set
*/
typedef struct {
    UInt8                          handle;
    UInt16                         duration;
    UInt8                          max_extadv_events;
} gpHci_ExtendedAdvEnableData_t;

/** @struct gpHci_IRK_t
 *  @brief BLE Spec: 7.8.45 LE Set Resolvable Private Address Timeout Command
*/
typedef struct {
    UInt8                          data[16];
} gpHci_IRK_t;

/** @struct gpHci_SetExtendedScanData_t */
typedef struct {
    gpHci_ScanType_t               scanType;
    UInt16                         scanInterval;
    UInt16                         scanWindow;
} gpHci_SetExtendedScanData_t;

/** @struct gpHci_ExtendedCreateConnectionData_t */
typedef struct {
    UInt16                         scanInterval;
    UInt16                         scanWindow;
    UInt16                         connIntervalMin;
    UInt16                         connIntervalMax;
    UInt16                         connLatency;
    UInt16                         supervisionTimeout;
    UInt16                         minCELength;
    UInt16                         maxCELength;
} gpHci_ExtendedCreateConnectionData_t;

/** @struct gpHci_ExtendedCreateConnection_t */
typedef struct {
    gpHci_InitiatorFilterPolicy_t  filterPolicy;
    gpHci_OwnAddressType_t         ownAddressType;
    gpHci_InitPeerAddressType_t    peerAddressType;
    BtDeviceAddress_t              peerAddress;
    gpHci_PhyMask_t                initiatingPhys;
    gpHci_ExtendedCreateConnectionData_t* data;
} gpHci_ExtendedCreateConnection_t;

/** @struct gpHci_CoexParams_t */
typedef struct {
    Bool                           req;
    UInt8                          prio;
    Bool                           grant;
} gpHci_CoexParams_t;

/** @struct gpHci_CigParamsCisData_t */
typedef PACKED_PRE struct {
    gpHci_CisId_t                  cisId;
    UInt16                         maxSduCtoP;
    UInt16                         maxSduPtoC;
    gpHci_PhyMask_t                phyCtoP;
    gpHci_PhyMask_t                phyPtoC;
    UInt8                          rtnCtoP;
    UInt8                          rtnPtoC;
} PACKED_POST gpHci_CigParamsCisData_t;

/** @struct gpHci_CigParamsTestCisData_t */
typedef PACKED_PRE struct {
    gpHci_CisId_t                  cisId;
    UInt8                          nse;
    UInt16                         maxSduCtoP;
    UInt16                         maxSduPtoC;
    UInt16                         maxPduCtoP;
    UInt16                         maxPduPtoC;
    gpHci_PhyMask_t                phyCtoP;
    gpHci_PhyMask_t                phyPtoC;
    UInt8                          bnCtoP;
    UInt8                          bnPtoC;
} PACKED_POST gpHci_CigParamsTestCisData_t;

/** @struct gpHci_CreateCisData_t */
typedef PACKED_PRE struct {
    gpHci_ConnectionHandle_t       cisConnectionHandle;
    gpHci_ConnectionHandle_t       aclConnectionHandle;
} PACKED_POST gpHci_CreateCisData_t;

/** @union gpHci_EventCbPayload_t */
typedef union {
    gpHci_DisconnectCompleteParams_t disconnectCompleteParams;
    gpHci_CommandCompleteParams_t  commandCompleteParams;
    gpHci_CommandStatusParams_t    commandStatusParams;
    gpHci_ConnectionCompleteParams_t connectionCompleteParams;
    gpHci_LEMetaEventParams_t      metaEventParams;
    gpHci_AuthenticatedPayloadToExpired_t authenticatedPayloadToExpired;
    gpHci_EncryptionChangeParams_t encryptionChangeParams;
    gpHci_EncryptionKeyRefreshComplete_t keyRefreshComplete;
    gpHci_ReadRemoteVersionInfoComplete_t readRemoteVersionInfoComplete;
    gpHci_AuthenticatedPayloadToExpired_t authPayloadToExpired;
    gpHci_NumberOfCompletedPackets_t numberOfCompletedPackets;
    gpHci_VsdMetaEventParams_t     VsdMetaEventParams;
    UInt8                          hwErrorCode;
    /** @brief  dataBufferOverflow  */
    gpHci_LinkType_t               linkType;
    gpHci_VsdSinkRxIndication_t    vsdSinkRxIndication;
} gpHci_EventCbPayload_t;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//Requests
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPHCI_TYPES_H_

