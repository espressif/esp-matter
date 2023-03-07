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

#ifndef _GPBLELLCPFRAMEWORK_H_
#define _GPBLELLCPFRAMEWORK_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpBleLlcpFramework_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpBleLlcp.h"
#include "gpBleSecurityCoprocessor.h"
#include "gpBle_defs.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// Consider using proper mask/id macro's (to allow more fields to be added to a UInt8)
#define GPBLELLCPFRAMEWORK_PDU_ROLE_MASK_CENTRAL        0x01
#define GPBLELLCPFRAMEWORK_PDU_ROLE_MASK_PERIPHERAL     0x02
#define GPBLELLCPFRAMEWORK_PDU_ROLE_MASK_BOTH           (GPBLELLCPFRAMEWORK_PDU_ROLE_MASK_CENTRAL | GPBLELLCPFRAMEWORK_PDU_ROLE_MASK_PERIPHERAL)


// Properties for a procedure
#define GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_INSTANT_IDX      0x00
#define GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_LONG_PDUS_IDX    0x01

#define GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_INSTANT_BM       0x01
#define GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_LONG_PDUS_BM     0x02

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_INSTANT_SET(flags, instant)      BLE_BM_SET(flags, GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_INSTANT_BM, GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_INSTANT_IDX, instant)
#define GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_INSTANT_GET(flags)               BLE_BM_GET(flags, GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_INSTANT_BM, GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_INSTANT_IDX)
#define GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_LONG_PDUS_SET(flags, instant)    BLE_BM_SET(flags, GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_LONG_PDUS_BM, GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_LONG_PDUS_IDX, instant)
#define GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_LONG_PDUS_GET(flags)             BLE_BM_GET(flags, GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_LONG_PDUS_BM, GPBLELLCPFRAMEWORK_PROCEDURE_FLAGS_LONG_PDUS_IDX)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

// List of possible actions for the LLCP framework, after calling procedure-specific callbacks
#define Ble_LlcpFrameworkActionContinue             0x00
#define Ble_LlcpFrameworkActionPause                0x01
#define Ble_LlcpFrameworkActionWaitForEmptyTxQueue  0x02
#define Ble_LlcpFrameworkActionStop                 0x03
#define Ble_LlcpFrameworkActionReject               0x04
#define Ble_LlcpFrameworkActionRejectWithUnknownRsp 0x05
typedef UInt8 Ble_LlcpFrameworkAction_t;

typedef Ble_LlcpFrameworkAction_t   (* Ble_LlcpQueueingNeededCb_t)          (Ble_LlcpLinkContext_t* pContext);
typedef Ble_LlcpFrameworkAction_t   (* Ble_LlcpStartCb_t)                   (Ble_LlcpLinkContext_t* pContext, Ble_LlcpProcedureContext_t* pProcedure);
typedef void                        (* Ble_LlcpGetCtrDataCb_t)              (Ble_LlcpLinkContext_t* pContext, Ble_LlcpProcedureContext_t* pProcedure, gpBleLlcp_Opcode_t* pOpcode, UInt8* ctrDataLength, UInt8* pCtrData);
typedef Ble_LlcpFrameworkAction_t   (* Ble_LlcpStoreRxPduDataCb_t)          (Ble_LlcpLinkContext_t* pContext, Ble_LlcpProcedureContext_t* pProcedure, gpPd_Loh_t* pPdLoh, gpBleLlcp_Opcode_t opcode);
typedef Ble_LlcpFrameworkAction_t   (* Ble_LlcpPduQueuedCb_t)               (Ble_LlcpLinkContext_t* pContext, Ble_LlcpProcedureContext_t* pProcedure, gpBleLlcp_Opcode_t txOpcode);
typedef Ble_LlcpFrameworkAction_t   (* Ble_LlcpTxPduFollowupCb_t)           (Ble_LlcpLinkContext_t* pContext, Ble_LlcpProcedureContext_t* pProcedure, gpBleLlcp_Opcode_t txOpcode);
typedef Ble_LlcpFrameworkAction_t   (* Ble_LlcpRxPduFollowupCb_t)           (Ble_LlcpLinkContext_t* pContext, Ble_LlcpProcedureContext_t* pProcedure, gpBleLlcp_Opcode_t rxOpcode);
typedef Ble_LlcpFrameworkAction_t   (* Ble_LlcpUnexpectedPduReceivedCb_t)   (Ble_LlcpLinkContext_t* pContext, Ble_LlcpProcedureContext_t* pProcedure, gpBleLlcp_Opcode_t rxOpcode, gpPd_Loh_t* pPdLoh);
typedef void                        (* Ble_LlcpProcedureFinishFunc_t)       (Ble_LlcpLinkContext_t* pContext, Ble_LlcpProcedureContext_t* pProcedure, Bool notifyHost);

typedef void (* gpBleLlcpFramework_cbProcedureCb_t) (Ble_LlcpProcedureContext_t* pProcedure);


typedef struct {
    gpBleLlcp_Opcode_t opcode;
    UInt8 ctrDataLength;
    UInt8 pduFlags;
} gpBleLlcpFramework_PduDescriptor_t;

typedef struct {
    gpBleLlcpFramework_cbProcedureCb_t cbProcedureStart;
    gpBleLlcpFramework_cbProcedureCb_t cbProcedureDone;
} gpBleLlcpFramework_ProcedureUserCallbacks_t;

// Procedure description
typedef struct {
    // Procedure description
    UInt8                                       procedureFlags;
    // The size of procedure-specific data that will be used
    UInt8                                       procedureDataLength;
    // The length of the associated PDU descriptor list
    UInt8                                       nrOfPduDescriptors;
    // Pointer to all PDU descriptors
    // The first pdu descriptor will always be considered the starting PDU of a procedure!
    const gpBleLlcpFramework_PduDescriptor_t*   pPduDescriptors;
    // Any feature inside this mask allows you to start the procedure on a link
    // e.g. PHY update can be started when LE 2M is supported OR when LE Coded PHY is supported
    // If no feature is specified, the procedure is always allowed to start
    // Make sure to switch to UInt64 when number of features grows beyond 32 bits
    // See also compile-time check in gpBleLlcpFramework_Init
    UInt64                                      featureMask;
    Ble_LlcpQueueingNeededCb_t                  cbQueueingNeeded;
    // Function step description
    Ble_LlcpStartCb_t                           cbProcedureStart;
    Ble_LlcpGetCtrDataCb_t                      cbGetCtrData;
    // The store callback only makes permanent changes when there is valid data in the PDU
    Ble_LlcpStoreRxPduDataCb_t                  cbStoreCtrData;
    // The pdu received callback is only called when a valid PDU is received
    Ble_LlcpRxPduFollowupCb_t                   cbPduReceived;
    Ble_LlcpUnexpectedPduReceivedCb_t           cbUnexpectedPduReceived;
    Ble_LlcpPduQueuedCb_t                       cbPduQueued;
    Ble_LlcpTxPduFollowupCb_t                   cbPduTransmitted;
    Ble_LlcpProcedureFinishFunc_t               cbFinished;
} Ble_LlcpProcedureDescriptor_t;

#define BLE_LLCP_PROCEDURE_RUNNING_IDX                      0
#define BLE_LLCP_PROCEDURE_PURGED_IDX                       1
#define BLE_LLCP_PROCEDURE_WAITING_ON_EMPTY_QUEUE_IDX       2
#define BLE_LLCP_PROCEDURE_WAITING_ON_HOST_IDX              3
#define BLE_LLCP_PROCEDURE_WAITING_ON_INSTANT_IDX           4
#define BLE_LLCP_PROCEDURE_LOCALLY_REJECTED_IDX             5
#define BLE_LLCP_PROCEDURE_INTERRUPTED_BY_TERMINATION_IDX   6

typedef UInt8 Ble_LlcpProcedureStateId_t;

/** @struct Ble_LengthUpdateData_t */
typedef struct {
    UInt16                         localTxOctets;
    UInt16                         localTxTime;
    UInt16                         effectiveTxOctets;
    UInt16                         effectiveTxTime;
    UInt16                         effectiveRxOctets;
    UInt16                         effectiveRxTime;
} Ble_LengthUpdateData_t;

typedef struct {
    Ble_LlcpConnParamReqRspPdu_t pdu;
    UInt16 interval;
    UInt16 instant;
    UInt8 winSize;
    UInt16 winOffset;
    UInt16 minCELength;
    UInt16 maxCELength;
    UInt32 firstAnchorPoint;    // master-only
    UInt16 anchorMoveOffset;
} Ble_LlcpConnParamReqRspData_t;

/** @struct Ble_CteProcedureData_t */
typedef struct {
    gpHci_CteType_t                cteType;
    UInt8                          minCteLengthUnit;
} Ble_CteProcedureData_t;


typedef struct {
    UInt8 reason;
} gpBleLlcpProcedureTerminationData_t;

typedef struct {
    UInt8   randomNumber[GP_HCI_RANDOM_DATA_LENGTH];
    UInt16  encryptedDiversifier;
    UInt8   longTermKey[GP_HCI_ENCRYPTION_KEY_LENGTH];
    UInt8   sessionKeyDivMaster[BLE_SEC_KEY_DIV_PART_LENGTH];
    UInt8   sessionKeyDivSlave[BLE_SEC_KEY_DIV_PART_LENGTH];
    UInt8   initVectorMaster[BLE_SEC_INIT_VECTOR_PART_LENGTH];
    UInt8   initVectorSlave[BLE_SEC_INIT_VECTOR_PART_LENGTH];
} Ble_StartEncryptionProcedureData_t;

typedef struct {
    gpHci_PhyMask_t txPhys;
    gpHci_PhyMask_t rxPhys;
    gpHci_PhyMask_t txPhysRemote;
    gpHci_PhyMask_t rxPhysRemote;
    gpHci_Phy_t selectedTxPhy;
    gpHci_Phy_t selectedRxPhy;
    UInt16 instant;
    UInt16 pduConnEventCount;
    gpHci_PhyOptions_t phyOptions;
    UInt16 effectiveTxOctets;
    UInt16 effectiveTxTime;
    UInt16 effectiveRxOctets;
    UInt16 effectiveRxTime;
} Ble_LlcpPhyUpdateData_t;

typedef union {
    Ble_StartEncryptionProcedureData_t startEncryption;
    gpBleLlcpProcedureTerminationData_t termination;
    Ble_LengthUpdateData_t dataLengthUpdate;
    Ble_CteProcedureData_t cte;
    Ble_LlcpConnParamReqRspData_t connParamReq;
    Ble_LlcpPhyUpdateData_t phyUpdate;
    gpBle_PastInfoData_t pastData;
} gpBleLlcpFramework_ProcedureSpecificData_t;

typedef struct {
    gpBleLlcp_ProcedureId_t procedureId;
    Bool controllerInit;
    UInt8 dataLength;
    gpBleLlcpFramework_ProcedureSpecificData_t procedureData;
} gpBleLlcpFramework_StartProcedureDescriptor_t;

/*****************************************************************************
 *                    Component Function Definitions
 *****************************************************************************/

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpBleLlcpFrameWork_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

void gpBleLlcpFramework_Init(void);
void gpBleLlcpFramework_Reset(Bool firstReset);
void gpBleLlcpFramework_OpenConnection(Ble_IntConnId_t connId);
void gpBleLlcpFramework_CloseConnection(Ble_IntConnId_t connId);
void gpBleLlcpFramework_RegisterProcedure(gpBleLlcp_ProcedureId_t procedureId, const Ble_LlcpProcedureDescriptor_t* pDescriptor);
void gpBleLlcpFramework_RegisterProcedureCallbacks(gpBleLlcp_ProcedureId_t procedureId, const gpBleLlcpFramework_ProcedureUserCallbacks_t* pCallbacks);
void gpBleLlcpFramework_RegisterInvalidProcedureAction(gpBleLlcp_ProcedureId_t procedureId, Bool masterRole);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
void gpBleLlcpFramework_AddProcedureData(Ble_LlcpProcedureContext_t* pProcedure, UInt8 length, UInt8* pData);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
Bool gpBleLlcpFramework_ProcedureStateGet(Ble_LlcpProcedureContext_t* pProcedure, Ble_LlcpProcedureStateId_t state);
void gpBleLlcpFramework_ProcedureStateSet(Ble_LlcpProcedureContext_t* pProcedure, Ble_LlcpProcedureStateId_t state);
void gpBleLlcpFramework_ProcedureStateClear(Ble_LlcpProcedureContext_t* pProcedure, Ble_LlcpProcedureStateId_t state);
void gpBleLlcpFramework_EnableProcedureHandling(Ble_LlcpLinkContext_t* pContext, Bool local, Bool enable);

// Procedure actions
gpHci_Result_t gpBleLlcpFramework_StartProcedure(Ble_IntConnId_t connId, gpBleLlcpFramework_StartProcedureDescriptor_t* pStart);
Ble_LlcpProcedureContext_t* gpBleLlcpFramework_GetProcedure(Ble_LlcpLinkContext_t* pContext, Bool local);
void gpBleLlcpFramework_ResumeProcedure(Ble_LlcpLinkContext_t* pContext, Ble_LlcpProcedureContext_t* pProcedure, gpBleLlcp_Opcode_t pdu);
void gpBleLlcpFramework_StopActiveProcedure(gpHci_ConnectionHandle_t connHandle, Bool local);
Ble_LlcpProcedureContext_t* gpBleLlcpFramework_PurgeFirstQueuedProcedure(Ble_LlcpLinkContext_t* pContext, gpBleLlcp_ProcedureId_t procedureId);

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_GPBLELLCPFRAMEWORK_H_

