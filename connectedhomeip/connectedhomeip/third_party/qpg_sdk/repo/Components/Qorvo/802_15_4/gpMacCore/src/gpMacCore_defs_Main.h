/*
 * Copyright (c) 2017-2019, Qorvo Inc
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GPMAC_CORE_DEFS_MAIN_H_
#define _GPMAC_CORE_DEFS_MAIN_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpMacCore_RomCode_defs_Main.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_DataTable.h"
#include "gpMacCore_CodeJumpTableRom_Defs_defs.h"
#include "gpMacCore_CodeJumpTableFlash_Defs_defs.h"
#include "gpMacCore_CodeJumpTableRom_Defs_defs_Main.h"
#include "gpMacCore_CodeJumpTableFlash_Defs_defs_Main.h"
#include "gpHal_CodeJumpTableRom_Defs_gpglobal.h" // make sure the gpHal_cb.... go into ROM
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */


#if defined (GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR) || defined (GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)
STATIC_FUNC void MacCore_DoActiveScan(void);
#endif // #if defined (GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR) || defined (GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)
#if defined (GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR)
STATIC_FUNC void MacCore_DoEDScan(void);
void MacCore_DoIteratedScan(void);
#endif // #if defined (GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR)
#if defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR || defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
STATIC_FUNC void MacCore_DoOrphanScan(void);
#endif // #if defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR || defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT

//Assocation Response related
/** @fn MacCore_cbAssocResp(void)
 *  @brief This function constructs and sends the association response
 *
 *  @param void*                    data
 *  @param gpMacCore_AddressInfo_t *dstAddrInfo
 *
 *  @return void
 */
#if defined(GP_MACCORE_DIVERSITY_FFD) || defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)
STATIC_FUNC void MacCore_cbAssocConfirm(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t txTime);
STATIC_FUNC void MacCore_cbAssocResp(void* arg);
STATIC_FUNC void MacCore_AssociateResponseTimeOut(void* arg);
STATIC_FUNC void MacCore_AssocRespStopRequests(gpMacCore_StackId_t stackId);
STATIC_FUNC MacCore_AssocRsp_t* MacCore_GetAssocRespPointer(MACAddress_t extendedAddress);
STATIC_FUNC void MacCore_FreeAssocRespPointer(MacCore_AssocRsp_t* pAssocRsp);
STATIC_FUNC Bool MacCore_AssocRespPointerValid(MacCore_AssocRsp_t* pAssocRsp);
STATIC_FUNC MacCore_AssocRsp_t* MacCore_GetFreeAssocRespPointer(void);
#endif // defined(GP_MACCORE_DIVERSITY_FFD) || defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)

#if defined(GP_MACCORE_DIVERSITY_FFD) || defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR)
//Assocation Request related
STATIC_FUNC void MacCore_AssociateSendCommandDataRequest(void);
STATIC_FUNC void MacCore_HandleAssocConf(void);
STATIC_FUNC void MacCore_AssociateTimeout(void);
#endif // defined(GP_MACCORE_DIVERSITY_FFD) || defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR)

#if defined(GP_MACCORE_DIVERSITY_FFD) || defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)
//Indirect transmission related functions
STATIC_FUNC Bool MacCore_IndTxAddElement( gpMacCore_AddressMode_t srcAddrMode, gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 txOptions, gpMacCore_Security_t *pSecOptions, gpMacCore_MultiChannelOptions_t multiChannelOptions, gpPd_Loh_t pdLoh, gpMacCore_StackId_t stackId);
STATIC_FUNC Bool MacCore_cbIndDataConfirm (gpMacCore_Result_t result, gpPd_Handle_t pdHandle);

#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
STATIC_FUNC void MacCore_cbPollIndication( gpMacCore_AddressInfo_t* pAddrInfo, UInt8 stackId, gpPd_TimeStamp_t rxTime, Bool fp /*frame pending*/);
#endif //GP_MACCORE_DIVERSITY_POLL_RECIPIENT
STATIC_FUNC void MacCore_IndTxInit( void );
//STATIC_FUNC void MacCore_IndTxBufferCleanUp(gpMacCore_StackId_t stackId);
//UInt8 MacCore_IndTxBufferNbrActiveElements(gpMacCore_StackId_t stackId);
#endif // defined(GP_MACCORE_DIVERSITY_FFD) || defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)

STATIC_FUNC void MacCore_WriteMacHeaderInPd(gpMacCore_FrameType_t type, gpMacCore_AddressInfo_t* pSrcAddrInfo, const gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 txOptions, UInt8 securityLevel,  gpPd_Loh_t *pPdLoh , gpMacCore_StackId_t stackId );
STATIC_FUNC void MacCore_InitSrcAddressInfo(gpMacCore_AddressInfo_t *srcAddressInfo, gpMacCore_AddressMode_t addrMode, gpMacCore_StackId_t stackId);
STATIC_FUNC Bool MacCore_AllocatePdLoh(gpPd_Loh_t* p_PdLoh);

#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR) || defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR)
STATIC_FUNC gpMacCore_Result_t MacCore_SendCommand( gpMacCore_AddressInfo_t* pDestAddrInfo, gpMacCore_AddressInfo_t* pSrcAddrInfo, UInt8 txOptions, gpMacCore_Security_t *pSecOptions, UInt8 *pData, UInt8 len , gpMacCore_StackId_t stackId, gpHal_MacScenario_t scenario );
STATIC_FUNC gpMacCore_Result_t MacCore_SendCommandDataRequest(gpMacCore_AddressInfo_t * pCoordAddrInfo, Bool associationTrigger, gpMacCore_Security_t *pSecOptions, gpMacCore_StackId_t stackId);
#endif

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */
STATIC_FUNC void MacCore_InitPad(gpMacCore_StackId_t stackId);
STATIC_FUNC void MacCore_SetDefaultStackValues(Bool resetPib, gpMacCore_StackId_t stackId);
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
STATIC_FUNC void MacCore_HandleOrphanScanEnd(gpMacCore_Result_t result);
#endif // GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
// security functions


// pd functions
STATIC_FUNC void MacCore_ReadStreamAndUpdatePd(UInt8* bytes, UInt8 length, gpPd_Loh_t* p_PdLoh);
STATIC_FUNC void MacCore_PrependStreamAndUpdatePd(UInt8* bytes, UInt8 length, gpPd_Loh_t* p_PdLoh);


#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
STATIC_FUNC void MacCore_DelayedPollConfirm(void);
#endif // GP_MACCORE_DIVERSITY_POLL_ORIGINATOR

#if (GP_MACCORE_SCAN_RXOFFWINDOW_TIME_US != 0)
STATIC_FUNC void MacCore_ScanRxOffWindow(void);
#endif // (GP_MACCORE_SCAN_RXOFFWINDOW_TIME_US != 0)

#ifdef GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR
STATIC_FUNC void MacCore_ResetMinInterferenceLevels(void);
#endif // GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR

/** @brief This function returns the stackId based on the panId.
 *  @param panId         returns MACCORE_STACK_UNDEFINED when stackId was not found - else a value below GP_DIVERSITY_NR_OF_STACKS
*/

#if (GP_DIVERSITY_NR_OF_STACKS > 1) || (defined(GP_DIVERSITY_JUMPTABLES))
STATIC_FUNC gpMacCore_StackId_t MacCore_GetStackId( gpMacCore_AddressInfo_t *pAddressInfo );
#else
#define MacCore_GetStackId(pAddressInfo)    0x0
#endif //  (GP_DIVERSITY_NR_OF_STACKS > 1) || (defined(GP_DIVERSITY_JUMPTABLES))

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */
STATIC_FUNC gpPad_Handle_t MacCore_GetPad(UInt8 stackId);


/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/* Wrapper for gpHal_DataRequest which applies Regional Domain Settings. */
STATIC_FUNC gpMacCore_Result_t MacCore_TxDataRequest(gpHal_DataReqOptions_t* pDataReqOptions, gpPd_Loh_t pdLoh, gpMacCore_StackId_t stackId);

Bool MacCore_CheckIfNeighbour   (gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId);
void MacCore_InitDataPending(void);
gpMacCore_DataPendingMode_t MacCore_GetDataPendingMode(void);
Bool MacCore_CheckIfDataPending   (gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId);


#ifdef GP_MACCORE_DIVERSITY_RAW_FRAMES
UInt8 MacCoreGetRawStack(void);
void MacCore_GetRawModeEncryptionOffsets(gpPd_Loh_t currentPdLoh, UInt16* pAuxOffset, UInt16* pDataOffset, UInt16* pCslIeOffset);
void MacCore_StoreLinkMetrics(MacCore_HeaderDescriptor_t *pmdi, gpPd_Loh_t pdLoh);

#endif //GP_MACCORE_DIVERSITY_RAW_FRAMES

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */


/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

STATIC_FUNC UInt8 MacCore_AnalyseMacHeader(gpPd_Loh_t* p_PdLoh, MacCore_HeaderDescriptor_t* pMacHeaderDecoded);
#if defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR)
STATIC_FUNC void MacCore_ProcessBeacon(gpPd_Loh_t pdLoh, MacCore_HeaderDescriptor_t* pMdi, gpHal_RxInfo_t *rxInfo);
#endif //GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR

#ifdef GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT
STATIC_FUNC void MacCore_ConstructBeacon(gpPd_Loh_t* p_PdLoh, UInt8 stackId);
STATIC_FUNC void MacCore_TransmitBeacon(gpPd_Loh_t* p_PdLoh, gpHal_RxInfo_t *rxInfo);
#endif //GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT

STATIC_FUNC Bool MacCore_RxForThisStack(gpMacCore_StackId_t stackId , gpMacCore_AddressMode_t dstAddrMode, UInt8 rxChannel);

#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
UInt8 MacCore_IndTxBufferFindElemForTx(gpMacCore_AddressInfo_t *addressInfo, gpMacCore_StackId_t stackId);
#endif //GP_MACCORE_DIVERSITY_POLL_RECIPIENT

#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
STATIC_FUNC gpMacCore_Result_t MacCore_SendCommandCoordinatorRealignment(MACAddress_t* pDestAddr, UInt16 orphanShortAddress, Bool broadCast, gpMacCore_StackId_t stackId);
#endif

STATIC_FUNC gpMacCore_Result_t MacCore_ValidateClearText(MacCore_HeaderDescriptor_t* dataIndicationMacValues);

STATIC_FUNC void MacCore_ProcessData(gpPd_Loh_t pdLoh, MacCore_HeaderDescriptor_t* pMdi , UInt8 macHeaderLength, gpHal_RxInfo_t *rxInfo);
#ifdef GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR
STATIC_FUNC void MacCore_IteratedScan_Confirm(UInt16 channelMask, UInt8 *proto_energy_level);
#endif
#if defined (GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR)
STATIC_FUNC void MacCore_EDScan_Confirm(UInt16 channelMask, UInt8 *proto_energy_level);
#endif //defined (GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR)
#ifdef GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR
STATIC_FUNC gpMacCore_Result_t MacCore_SendCommandBeaconRequest(UInt8 channel);
#endif //GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
STATIC_FUNC gpMacCore_Result_t MacCore_SendCommandOrphanNotification(UInt8 channel );
#endif // GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
#if (defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR) || (defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR)
STATIC_FUNC UInt8 MacCore_GetNextScanChannel(void);
STATIC_FUNC void MacCore_PrepareScanVars(UInt8 frameTypeFilterMask);
#endif //#if (defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR) || (defined GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR)
#ifdef GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR
STATIC_FUNC void MacCore_DoScanIteration(void);
STATIC_FUNC void MacCore_DoScanPrepareNextIteration(UInt8 channel);
STATIC_FUNC UInt8 MacCore_ConvertRssiToAbsoluteInterference(Int8 valueRssi);
#endif // GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR

#if defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)
STATIC_FUNC gpMacCore_Result_t MacCore_DataPendingManipulateEntry(Bool dataQueued, Bool isNeighbour, gpMacCore_AddressInfo_t* pAddrInfo, gpMacCore_StackId_t stackId);
STATIC_FUNC UInt8              MacCore_GetNeighbourIndex(gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId);
STATIC_FUNC Bool               MacCore_IsDataPending(Bool dataQueued, Bool isNeighbour);
STATIC_FUNC UInt8              MacCore_NumberOfNeighbours(gpMacCore_StackId_t stackId);
STATIC_FUNC void MacCore_IndTxBufferRemoveExpiredPacket(void* pStackId);
STATIC_FUNC Bool MacCore_ExistsElemForAddress( gpMacCore_AddressInfo_t* pAddressInfo, gpMacCore_StackId_t stackId );
STATIC_FUNC Bool MacCore_CompairAddress( gpMacCore_AddressInfo_t* pAddressA , gpMacCore_AddressInfo_t* pAddressB );
#endif // defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)

STATIC_FUNC void MacCore_HalDataIndication(gpPd_Loh_t pdLoh, gpHal_RxInfo_t *rxInfo);
STATIC_FUNC void MacCore_HalDataConfirm(gpHal_Result_t status, gpPd_Loh_t pdLoh, UInt8 lastChannelUsed);
STATIC_FUNC Bool MacCore_isValidAddressMode(UInt8 addressMode);

STATIC_FUNC void MacCore_cbDataConfirm (gpMacCore_Result_t result, gpPd_Handle_t pdHandle);

void MacCore_StopScan(void);

STATIC_FUNC void MacCore_HalDataConfirm_dummy(gpHal_Result_t status, gpPd_Loh_t pdLoh, UInt8 lastChannelUsed);
STATIC_FUNC void MacCore_HalDataIndication_dummy(gpPd_Loh_t pdLoh, gpHal_RxInfo_t *rxInfo);
#ifdef GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR
STATIC_FUNC void MacCore_cbEDConfirm_dummy(UInt16 channelMask, UInt8 *proto_energy_level_list);
#endif

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_GPMAC_CORE_DEFS_MAIN_H_

