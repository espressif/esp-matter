/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Bluetooth Low Energy
 *   Declarations of the public functions and enumerations of gpBleConfig.
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
 *
 */

#ifndef _GPBLE_CONFIG_H_
#define _GPBLE_CONFIG_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpBleConfig_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpHci_Includes.h"
#include "gpBle.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

// 0x00 - 0x05 are pre 4.0 versions
// See https://www.bluetooth.com/specifications/assigned-numbers/link-layer
#define gpBleConfig_BleVersionId_4_0     0x06
#define gpBleConfig_BleVersionId_4_1     0x07
#define gpBleConfig_BleVersionId_4_2     0x08
#define gpBleConfig_BleVersionId_5_0     0x09
#define gpBleConfig_BleVersionId_5_1     0x0A
#define gpBleConfig_BleVersionId_5_2     0x0B

typedef UInt8 gpBleConfig_BleVersionId_t;

#define gpBleConfig_CompanyIdentifierGreenPeak     0x0453
typedef UInt16 gpBleConfig_CompanyIdentifier_t;

typedef struct {
    UInt8 hciVersion;
    UInt16 hciRevision;
    UInt8 lmppalVersion;
    UInt16 lmppalSubversion;
    UInt16 companyId;
} gpBle_Configuration_t;

// List of link layer features (Spec version 5.x, Vol 6, Part B, section 4.6)
#define gpBleConfig_FeatureIdLeEncryption                      0x00
#define gpBleConfig_FeatureIdConnectionParametersRequest       0x01
#define gpBleConfig_FeatureIdExtendedRejectIndication          0x02
#define gpBleConfig_FeatureIdSlaveFeatureExchange              0x03
#define gpBleConfig_FeatureIdLePing                            0x04
#define gpBleConfig_FeatureIdDataPacketLengthExtension         0x05
#define gpBleConfig_FeatureIdLlPrivacy                         0x06
#define gpBleConfig_FeatureIdExtendedScannerFilterPolicies     0x07
#define gpBleConfig_FeatureIdLe2MbitPhy                        0x08
#define gpBleConfig_FeatureIdStableModulationIndexTx           0x09
#define gpBleConfig_FeatureIdStableModulationIndexRx           0x0A
#define gpBleConfig_FeatureIdLeCodedPhy                        0x0B
#define gpBleConfig_FeatureIdLeExtendedAdvertising             0x0C
#define gpBleConfig_FeatureIdLePeriodicAdvertising             0x0D
#define gpBleConfig_FeatureIdChannelSelectionAlgorithm2        0x0E
#define gpBleConfig_FeatureIdLePowerClass1                     0x0F
#define gpBleConfig_FeatureIdMinNrOfUsedChannels               0x10
#define gpBleConfig_FeatureIdConnectionCteRequest              0x11
#define gpBleConfig_FeatureIdConnectionCteResponse             0x12
#define gpBleConfig_FeatureIdConnectionlessCteTransmitter      0x13
#define gpBleConfig_FeatureIdConnectionlessCteReceiver         0x14
#define gpBleConfig_FeatureIdAntSwitchDuringCteTransmission    0x15
#define gpBleConfig_FeatureIdAntSwitchDuringCteReception       0x16
#define gpBleConfig_FeatureIdReceivingConstantToneExtension    0x17
#define gpBleConfig_FeatureIdPastSender                        0x18
#define gpBleConfig_FeatureIdPastRecipient                     0x19
#define gpBleConfig_FeatureIdScaUpdates                        0x1A
#define gpBleConfig_FeatureIdRemotePublicKeyValidation         0x1B
#define gpBleConfig_FeatureIdCisCentral                        0x1C
#define gpBleConfig_FeatureIdCisPeripheral                     0x1D
#define gpBleConfig_FeatureIdIsochronousBroadcaster            0x1E
#define gpBleConfig_FeatureIdSynchronizedReceiver              0x1F
#define gpBleConfig_FeatureIdIsochronousChannelsHostSupport    0x20
#define gpBleConfig_FeatureIdLePowerControlRequest             0x21
#define gpBleConfig_FeatureIdLePowerChangeIndication           0x22
#define gpBleConfig_FeatureIdLePathLossMonitoring              0x23
#define gpBleConfig_FeatureIdLePerAdvAdiSupport                0x24
#define gpBleConfig_FeatureIdLeConnectionSubrating             0x25
#define gpBleConfig_FeatureIdLeConnectionSubratingHostSupport  0x26
#define gpBleConfig_FeatureIdLeChannelClassification           0x27
#define gpBleConfig_FeatureIdInvalid                           0x28

typedef UInt8 gpBleConfig_FeatureId_t;

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifndef GP_DIVERSITY_BLECONFIG_VERSION_ID
#define GP_DIVERSITY_BLECONFIG_VERSION_ID       (gpBleConfig_BleVersionId_5_2)
#endif //GP_DIVERSITY_BLECONFIG_VERSION_ID

#ifndef GP_DIVERSITY_BLECONFIG_HCI_REVISION_ID
#define GP_DIVERSITY_BLECONFIG_HCI_REVISION_ID  (0x0001)
#endif //GP_DIVERSITY_BLECONFIG_HCI_REVISION_ID

#ifndef GP_DIVERSITY_BLECONFIG_SUBVERSION_ID
#define GP_DIVERSITY_BLECONFIG_SUBVERSION_ID    (0x0001)
#endif //BLE_LMPPAL_SUBVERSION

#ifndef GP_DIVERSITY_BLECONFIG_COMPANY_ID
#define GP_DIVERSITY_BLECONFIG_COMPANY_ID       (gpBleConfig_CompanyIdentifierGreenPeak)
#endif //GP_DIVERSITY_BLECONFIG_COMPANY_ID

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define GPBLELLCP_IS_FEATURE_VALID(featureId)       (featureId < gpBleConfig_FeatureIdInvalid)

/*****************************************************************************
 *                    Component Function Definitions
 *****************************************************************************/

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
//#include "gpJumpTables_DataTable.h"
#include "gpBleConfig_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

void gpBleConfig_Init(void);
void gpBleConfig_Reset(Bool firstReset);
void gpBleConfig_RegisterHciResetCallback(gpBle_HciResetCallback_t cb);
void gpBleConfig_GetEventMask(gpHci_EventMask_t* pMask, gpHci_EventMaskType_t type);


/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
UInt8 gpBleConfig_GetLlVersion(void);
UInt16 gpBleConfig_GetCompanyId(void);
UInt16 gpBleConfig_GetLlSubversion(void);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
void gpBleConfig_GetConfig(gpBle_Configuration_t* pConfig);

gpHci_Result_t gpBleConfig_GetOwnAddress(BtDeviceAddress_t* pAddress, gpHci_OwnAddressType_t ownAddressType);
void gpBleConfig_GetLocalSupportedFeatures(UInt64* pFeatures);
Bool gpBleConfig_HasRandomAddress(void);

Bool gpBleConfig_IsEventMasked(gpHci_EventCode_t event);
Bool gpBleConfig_IsLeMetaEventMasked(gpHci_LEMetaSubEventCode_t event);
Bool gpBleConfig_IsChannelIndexUsed(UInt8 ChannelIndex);
Bool gpBleConfig_IsLeMetaVSDEventMasked(UInt8 event);



/*****************************************************************************
 *                    Service Function Definitions
 *****************************************************************************/


// Configuration and Information service
gpHci_Result_t gpBle_Reset(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_ReadLocalVersionInformation(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_ReadLocalSupportedCommands(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_ReadLocalSupportedFeatures(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_ReadBufferSize(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_ReadBdAddr(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeSetRandomAddress(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeReadLocalSupportedFeatures(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeReadSupportedStates(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_SetEventMask(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_SetEventMaskPage2(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeSetEventMask(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_VsdWriteDeviceAddress(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeReadRfPathCompensation(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeWriteRfPathCompensation(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeReadTransmitPower(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeSetHostChannelClassification(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_SetLeMetaVSDEvent(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_ResetLeMetaVSDEvent(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
#endif //defined(GP_DIVERSITY_ROM_CODE)
#endif // _GPBLE_CONFIG_H_

