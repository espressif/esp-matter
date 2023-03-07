/*
 * Copyright (c) 2017, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by\
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

#ifndef _GPBLELLCP_H_
#define _GPBLELLCP_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpBleLlcp_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpBle.h"
#include "gpBle_defs.h"
#include "gpBleActivityManager.h"
#include "gpBleDataChannelTxQueue.h"
#include "gpBleConfig.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define BLE_LLCP_MAX_NR_OF_BUFFERED_RESOURCES_PER_LINK   2
#define BLE_LLCP_MAX_CONTROL_PDU_LENGTH     27

#define GPBLELLCP_FEATUREMASK_NONE          0x00000000

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

// LL Control opcodes
#define gpBleLlcp_OpcodeConnectionUpdateInd      0x00
#define gpBleLlcp_OpcodeChannelMapInd            0x01
#define gpBleLlcp_OpcodeTerminateInd             0x02
#define gpBleLlcp_OpcodeEncReq                   0x03
#define gpBleLlcp_OpcodeEncRsp                   0x04
#define gpBleLlcp_OpcodeStartEncReq              0x05
#define gpBleLlcp_OpcodeStartEncRsp              0x06
#define gpBleLlcp_OpcodeUnknownRsp               0x07
#define gpBleLlcp_OpcodeFeatureReq               0x08
#define gpBleLlcp_OpcodeFeatureRsp               0x09
#define gpBleLlcp_OpcodePauseEncReq              0x0A
#define gpBleLlcp_OpcodePauseEncRsp              0x0B
#define gpBleLlcp_OpcodeVersionInd               0x0C
#define gpBleLlcp_OpcodeRejectInd                0x0D
#define gpBleLlcp_OpcodeSlaveFeatureReq          0x0E
#define gpBleLlcp_OpcodeConnectionParamReq       0x0F
#define gpBleLlcp_OpcodeConnectionParamRsp       0x10
#define gpBleLlcp_OpcodeRejectExtInd             0x11
#define gpBleLlcp_OpcodePingReq                  0x12
#define gpBleLlcp_OpcodePingRsp                  0x13
#define gpBleLlcp_OpcodeLengthReq                0x14
#define gpBleLlcp_OpcodeLengthRsp                0x15
#define gpBleLlcp_OpcodePhyReq                   0x16
#define gpBleLlcp_OpcodePhyRsp                   0x17
#define gpBleLlcp_OpcodePhyUpdateInd             0x18
#define gpBleLlcp_OpcodeMinUsedChannelsInd       0x19
#define gpBleLlcp_OpcodeCteReq                   0x1A
#define gpBleLlcp_OpcodeCteRsp                   0x1B
#define gpBleLlcp_OpcodePeriodicSyncInd          0x1C
#define gpBleLlcp_OpcodeClockAccuracyReq         0x1D
#define gpBleLlcp_OpcodeClockAccuracyRsp         0x1E
#define gpBleLlcp_OpcodeCisReq                   0x1F
#define gpBleLlcp_OpcodeCisRsp                   0x20
#define gpBleLlcp_OpcodeCisInd                   0x21
#define gpBleLlcp_OpcodeCisTerminateInd          0x22
#define gpBleLlcp_OpcodePowerControlReq          0x23
#define gpBleLlcp_OpcodePowerControlRsp          0x24
#define gpBleLlcp_OpcodePowerChangeInd           0x25
#define gpBleLlcp_OpcodeInvalid                  0x26
typedef UInt8 gpBleLlcp_Opcode_t;

#define Ble_LlcpPduResourceOriginLocal          0x00
#define Ble_LlcpPduResourceOriginRemote         0x01
#define Ble_LlcpPduResourceOriginUnmatched      0x02
#define Ble_LlcpPduResourceOriginInvalid        0x03
typedef UInt8 Ble_LlcpPduResourceOrigin_t;

// Different LLCP procedures that can be called
#define gpBleLlcp_ProcedureIdConnectionUpdate       0x00
#define gpBleLlcp_ProcedureIdChannelMapUpdate       0x01
#define gpBleLlcp_ProcedureIdEncryptionStart        0x02
#define gpBleLlcp_ProcedureIdEncryptionPause        0x03
#define gpBleLlcp_ProcedureIdFeatureExchange        0x04
#define gpBleLlcp_ProcedureIdVersionExchange        0x05
#define gpBleLlcp_ProcedureIdTermination            0x06
#define gpBleLlcp_ProcedureIdConnectionParamRequest 0x07
#define gpBleLlcp_ProcedureIdPing                   0x08
#define gpBleLlcp_ProcedureIdDataLengthUpdate       0x09
#define gpBleLlcp_ProcedureIdPhyUpdate              0x0A
#define gpBleLlcp_ProcedureIdMinNrOfUsedChannels    0x0B
#define gpBleLlcp_ProcedureIdConstantToneExtension  0x0C
#define gpBleLlcp_ProcedureIdPerAdvSyncTransfer     0x0D
#define gpBleLlcp_ProcedureIdCisCreation            0x0F
#define gpBleLlcp_ProcedureIdCisTermination         0x10
#define gpBleLlcp_ProcedureIdInvalid                0x11
typedef UInt8 gpBleLlcp_ProcedureId_t;

// Masks to overrule slave latency on a per-block basis
#define Ble_ProhibitSlaveLatency_NoRestriction     0x00
#define Ble_ProhibitSlaveLatency_Llcp              0x01
#define Ble_ProhibitSlaveLatency_LocalProcedure    0x02
#define Ble_ProhibitSlaveLatency_RemoteProcedure   0x04
#define Ble_ProhibitSlaveLatency_DataRx            0x08
#define Ble_ProhibitSlaveLatency_HostStack         0x10
typedef UInt8 Ble_ProhibitSlaveLatency_Mask_t;

// Type definition to indicate whether a LL feature is supported
#define gpBleLlcp_FeatureStatus_NotSupported    0x00
#define gpBleLlcp_FeatureStatus_MaybeSupported  0x01
#define gpBleLlcp_FeatureStatus_Supported       0x02
typedef UInt8 gpBleLlcp_FeatureStatus_t;

// Type definition to indicate the status of a feature exchange procedure
#define gpBleLlcp_FeatureStatus_NotExchanged            0x00
#define gpBleLlcp_FeatureStatus_ExchangedNoSuccess      0x01
#define gpBleLlcp_FeatureStatus_ExchangedSuccess        0x02
typedef UInt8 gpBleLlcp_FeatureExchangedStatus_t;

// Type definition to indicate the status of the connection
#define gpBleLlcp_ConnectionStatus_NotConnected         0x00
#define gpBleLlcp_ConnectionStatus_Connecting           0x01
#define gpBleLlcp_ConnectionStatus_Connected            0x02
typedef UInt8 gpBleLlcp_ConnectionStatus_t;

typedef struct {
    UInt8 versionNr;
    UInt16 companyId;
    UInt16 subVersionNr;
} Ble_LlcpVersionInfo_t;

typedef struct {
    gpHci_PhyMask_t phys;
    UInt8           count;
}Ble_LlcpMinUsedChannels_t;

typedef struct {
    UInt8 length;
    Ble_LlcpPduResourceOrigin_t origin;
    UInt8 data[BLE_LLCP_MAX_CONTROL_PDU_LENGTH];
} Ble_LlcpPduResource_t;

typedef struct {
    gpBleLlcp_ProcedureId_t procedureId;
    UInt8 connId;
    Bool localInit;
    Bool controllerInit;
    UInt8 state;
    gpBleLlcp_Opcode_t expectedRxPdu;
    gpBleLlcp_Opcode_t currentTxPdu;
    gpPd_Handle_t pdHandle;
    gpHci_Result_t result;
    // Generic buffer, used for adding extra data to a procedure
    UInt8 dataLength;
    void* pData;
} Ble_LlcpProcedureContext_t;

typedef struct {
    UInt16                         connIntervalMin;
    UInt16                         connIntervalMax;
    UInt16                         minCELength;
    UInt16                         maxCELength;
} Ble_LlcpCreateConnectionParams_t;

typedef struct {
    Ble_IntConnId_t connId;
    Bool masterConnection;
    gpHci_ConnectionHandle_t hciHandle;
    gpBle_AccessAddress_t accessAddress;
    UInt64 featureSetLink;
    Bool versionExchanged;
    gpBleLlcp_FeatureExchangedStatus_t featuresExchangedStatus;
    Bool encryptionEnabled;
    UInt8 nrOfProceduresUsingPrst;
    UInt16 intervalUnit;
    UInt16 latency;
    UInt16 timeoutUnit;
    UInt16 combinedSca;
    Bool terminationOngoing;
    // Merge some of these Bools into 1?
    Bool localProcedureFlowEnabled;
    Bool remoteProcedureFlowEnabled;
    // The pdu that is last received
    gpBleLlcp_Opcode_t rxLlcpOpcode;
    UInt32 initialCorrelationTs;
    Ble_LlcpProcedureContext_t* pProcedureLocal;
    Ble_LlcpProcedureContext_t* pProcedureRemote;
    // bit [0 - 3] used for peerAddressType
    /*
     * 0x00 Public Device Address
     * 0x01 Random Device Address
     * 0x02 Public Identity Address (Corresponds to peerÃ¢â‚¬â„¢s Resolvable Private Address).
     * 0x03 Random (static) Identity Address
     */
    UInt8 peerAddressType;
    UInt8 connectionCompleteBufferHandle;
    gpHal_BleChannelMapHandle_t channelMapHandle;       // only relevant for slave connection
    Ble_ProhibitSlaveLatency_Mask_t prohibitSlaveLatencyMask;
    BtDeviceAddress_t peerAddress;
    Ble_LlcpPduResource_t pdus[BLE_LLCP_MAX_NR_OF_BUFFERED_RESOURCES_PER_LINK];
    Ble_LlcpVersionInfo_t remoteVersionInfo;
    gpPd_Loh_t queuedPdLoh;
#if defined (GP_DIVERSITY_JUMPTABLES)
    BtDeviceAddress_t localPrivateAddress;
    BtDeviceAddress_t peerPrivateAddress;
#endif // GP_BLE_DIVERSITY_ENHANCED_CONNECTION_COMPLETE
    Ble_LlcpCreateConnectionParams_t ccParams;
    UInt64 allowedProcedures;
    gpBleLlcp_ConnectionStatus_t connectionStatus;
} Ble_LlcpLinkContext_t;

/*****************************************************************************
 *                    Component Function Definitions
 *****************************************************************************/

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_DataTable.h"
#include "gpBleLlcp_CodeJumpTableFlash_Defs.h"
#include "gpBleLlcp_CodeJumpTableRom_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)


void gpBleLlcp_Init(gpHal_BleCallbacks_t* pCallbacks);
void gpBleLlcp_Reset(Bool firstReset);
gpHci_Result_t gpBleLlcp_AllocateConnectionSlave(BtDeviceAddress_t* pPeerAddress, gpHci_InitPeerAddressType_t peerAddressType, Ble_IntConnId_t* pConnId);
gpHci_Result_t gpBleLlcp_AllocateConnectionMaster(BtDeviceAddress_t* pPeerAddress, gpHci_InitPeerAddressType_t peerAddressType, Ble_IntConnId_t* pConnId);
void Ble_LlcpFreeConnectionMaster(Ble_IntConnId_t connId);
void gpBleLlcp_FreeConnectionSlave(Ble_IntConnId_t connId);

gpHci_Result_t gpBleLlcp_IsHostConnectionHandleValid(gpHci_ConnectionHandle_t connHandle);
Bool Ble_LlcpIsMasterConnection(Ble_IntConnId_t connId);
Bool Ble_LlcpIsConnectionAllocated(Ble_IntConnId_t connId);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
Bool Ble_LlcpIsConnectionCreated(Ble_IntConnId_t connId);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
gpBleLlcp_FeatureStatus_t gpBleLlcp_IsFeatureSupported(gpHci_ConnectionHandle_t connectionHandle, gpBleConfig_FeatureId_t featureId);
void Ble_GetAccessAddress(Ble_IntConnId_t connId, gpBle_AccessAddress_t* pAccessAddress);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
void Ble_LlcpGetPeerAddressInfo(Ble_IntConnId_t connId, Bool* pAddressType, BtDeviceAddress_t* pAddress);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
Bool Ble_IsConnectionActive(BtDeviceAddress_t* pAddress);
Bool Ble_IsAccessAddressUsed(gpBle_AccessAddress_t accessAddress);
void gpBleLlcp_FinishConnectionCreation(Ble_IntConnId_t connHandle,  Bool sendConnectionComplete, Ble_ConnEstablishParams_t* pConnEstablishParams);

// Channel map handling
void Ble_LlcpPopulateChannelRemapTable(gpHal_ChannelMap_t* pChanMap, UInt8* pUsedChannels);
void Ble_LlcpPopulateDefaultMasterChannelMap(void);
void Ble_LlcpUpdateMasterChannelMapHandle(gpHal_BleChannelMapHandle_t chanMapHandle);

// Connection update handling
UInt32 Ble_DriftCompensation(UInt16 combinedSca, UInt32 elapsedTimeUs);
Bool Ble_LlcpAnchorMoveRequested(Ble_IntConnId_t connId, UInt16 interval, UInt16 latency, UInt16 timeout);
Bool Ble_LlcpConnParamChangeRequested(Ble_IntConnId_t connId, UInt16 intervalMin, UInt16 intervalMax, UInt16 latency, UInt16 timeout);
Bool Ble_ConnectionParametersValid(UInt16 connIntervalMin, UInt16 connIntervalMax, UInt16 latency, UInt16 timeout, UInt16 minCe, UInt16 maxCe);

// Get link context
Ble_LlcpLinkContext_t* Ble_GetLinkContext(Ble_IntConnId_t connId);
Ble_IntConnId_t gpBleLlcp_HciHandleToIntHandle(gpHci_ConnectionHandle_t hciHandle);
gpHci_ConnectionHandle_t gpBleLlcp_IntHandleToHciHandle(Ble_IntConnId_t connId);

void gpBleLlcp_UpdateSlaveLatency(Ble_IntConnId_t connId, UInt16 slaveLatency);
void gpBleLlcp_ProhibitSlaveLatency(Ble_IntConnId_t connId, Bool set, Ble_ProhibitSlaveLatency_Mask_t bitmask_owner);

Bool Ble_LlcpTxWinOffsetValid(UInt16 offsetUnit, UInt16 intervalUnit);
Bool Ble_LlcpTxWinSizeValid(UInt16 sizeUnit, UInt16 intervalUnit);

// Calculates the (earliest) start time for an RT event
UInt32 gpBleLlcp_CalculateEarliestAnchorPoint(Ble_IntConnId_t connId, UInt32 correlationTs, UInt32 blackoutUs, UInt32 winOffsetUs, UInt32 winSizeUs, UInt32* pWindowDuration);

gpBle_AccessAddress_t gpBleLlcp_CreateAccessAddress(void);

gpHal_BleChannelMapHandle_t Ble_LlcpGetChannelMapHandle(void);
void gpBleLlcp_GetConnectionChannelMap(Ble_IntConnId_t connId, gpHci_ChannelMap_t* pChannelMap);

Bool gpBleLlcp_QueueEmptyPacket(Ble_IntConnId_t connId, gpBleData_CteOptions_t* pCteOptions);

Bool gpBleLlcp_IsSlaveLatencyAllowed(Ble_IntConnId_t connId);

gpHci_Result_t gpBleLlcp_GetAnchorPointFromEventCounterInPast(Ble_IntConnId_t connId, UInt16 eventCounter, UInt32* tAnchor);

UInt8 gpBleLlcp_GetNumConnections(void);

UInt8 gpBleLlcp_GetNumberOfEstablishedConnections(void);

Bool gpBleLlcp_IsAclHandleInUse(gpHci_ConnectionHandle_t connectionHandle);
gpHci_Result_t gpBleLlcp_DisconnectAcl(gpHci_ConnectionHandle_t cisHandle, gpHci_Result_t reason);
void gpBleLlcp_AclConnectionStop(Ble_IntConnId_t connId, gpHci_Result_t reason);

/*****************************************************************************
 *                    Service Function Definitions
 *****************************************************************************/

gpHci_Result_t gpBle_ReadRemoteVersionInfo(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeReadRemoteFeatures(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_ReadAuthenticatedPayloadTO(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_WriteAuthenticatedPayloadTO(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_ReadTransmitPowerLevel(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_ReadRSSI(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeReadChannelMap(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
// vsd
gpHci_Result_t gpBle_VsdGenerateAccessAddress(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_VsdSetSleep(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
void gpBle_SetVsdfixedRxWindowThresholdParam(UInt16 threshold);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

Bool Ble_IsConnectionIntervalValid(UInt16 connIntervalMin, UInt16 connIntervalMax);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_GPBLELLCP_H_

