/*
 * Copyright (c) 2012-2016, GreenPeak Technologies
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

#ifndef _GPMAC_CORE_DEFS_H_
#define _GPMAC_CORE_DEFS_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpMacCore_RomCode_defs.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpUtils.h"
#include "gpMacCore.h"
#include "gpRxArbiter.h"
#include "gpHal.h"
#include "gpStat.h"

//All macro's and typedefs
#include "gpMacCore_defsDefines.h"

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/** @struct MacCore_IndTxElement_t
 *  @brief The MacCore_IndTxElement_t structure contains all attributes required to support indirect transmission.
*/
typedef struct MacCore_IndTxElement{
    //DataRequest parameters:
    gpMacCore_AddressInfo_t             dstAddrInfo;
    gpMacCore_AddressMode_t             srcAddrMode;
    UInt8                               txOptions;
    gpMacCore_MultiChannelOptions_t     multiChannelOptions;
    gpPd_Loh_t                          pdLoh;
    UInt8                               stackId;
    /* @brief Countdown counter for expiry.
    Also used to signal pending DataRequest/Confirm for packet when set to 0. */
    UInt8                               freshnessCntr;
} MacCore_IndTxElement_t;

#if defined(GP_MACCORE_DIVERSITY_RAW_FRAMES)  || defined(GP_DIVERSITY_JUMPTABLES)
typedef struct MacCore_RawFrameInfo_s
{
    Bool raw[GP_PD_NR_OF_HANDLES];
} MacCore_RawFrameInfo_t;
#endif // defined(GP_MACCORE_DIVERSITY_RAW_FRAMES)  || defined(GP_DIVERSITY_JUMPTABLES)

/* Internal state of hardware timed TX function. */
#define gpMacCore_TimedTxState_Idle         0
#define gpMacCore_TimedTxState_Queued       1
#define gpMacCore_TimedTxState_Scheduled    2
typedef UInt8 gpMacCore_TimedTxState_t;

typedef struct gpMacCore_GlobalVars_s {
    gpMacCore_ScanState_t       *gpMacCore_pScanState;
#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)  || defined(GP_DIVERSITY_JUMPTABLES)
    MacCore_AssocRsp_t          *gpMacCore_pAssocRspNotUsed;
#endif //defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)  || defined(GP_DIVERSITY_JUMPTABLES)
#if defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR) || defined(GP_DIVERSITY_JUMPTABLES)
    MacCore_PollReqArgs_t       *gpMacCore_pPollReqArgs;
#endif //defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR) || defined(GP_DIVERSITY_JUMPTABLES)
#if defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)  || defined(GP_DIVERSITY_JUMPTABLES)
    UInt8                        MacCore_BeaconRequestStackId;
#endif //defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)  || defined(GP_DIVERSITY_JUMPTABLES)

#if defined(GP_DIVERSITY_JUMPTABLES)
    gpMacCore_TransmissionCallback_t MacCore_TransmissionStartCb;
#endif //defined(GP_MACCORE_DIVERSITY_TRANSMISSION_CALLBACKS)  || defined(GP_DIVERSITY_JUMPTABLES)

#if defined(GP_MACCORE_DIVERSITY_RAW_FRAMES)  || defined(GP_DIVERSITY_JUMPTABLES)
    MacCore_RawFrameInfo_t       MacCore_RawFrameInfo;
#endif //defined(GP_MACCORE_DIVERSITY_RAW_FRAMES)  || defined(GP_DIVERSITY_JUMPTABLES)

#if defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION) || defined(GP_DIVERSITY_JUMPTABLES)
    gpPd_Handle_t                MacCore_IndTxEmptyPacketPdHandle;
#endif // defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION) || defined(GP_DIVERSITY_JUMPTABLES)

#if defined(GP_DIVERSITY_JUMPTABLES)
    gpHal_AbsoluteEventId_t      MacCore_TimedTx_EventId;
    gpPd_Handle_t                MacCore_TimedTx_PdHandle;
    gpMacCore_TimedTxState_t     MacCore_TimedTx_State;
#endif //defined(GP_MACCORE_DIVERSITY_TIMEDTX) || defined(GP_DIVERSITY_JUMPTABLES)

#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)  || defined(GP_DIVERSITY_JUMPTABLES)
    MacCore_AssocRsp_t         **gpMacCore_pAssocRspMultiple;
#endif //defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)  || defined(GP_DIVERSITY_JUMPTABLES)

#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT) || defined(GP_DIVERSITY_JUMPTABLES)
    UInt8                        MacCore_NumberAssocRsp;
#endif //defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)  || defined(GP_DIVERSITY_JUMPTABLES)

} gpMacCore_GlobalVars_t;

typedef struct gpMacCore_ConstGlobalVars_s {
#if defined(GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR) || defined(GP_DIVERSITY_JUMPTABLES)
    Int8*                        MacCore_MinInterferenceLevels;
#endif //defined(GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR) || defined(GP_DIVERSITY_JUMPTABLES)
#if defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION) || defined(GP_DIVERSITY_JUMPTABLES)
    gpMacCore_DataPendingMode_t* MacCore_DataPendingModePerStack;
    gpMacCore_StackId_t*         pStackId;
    MacCore_IndTxElement_t**     MacCore_IndTxBuffer;
    UInt8                        MacCore_IndTxBuffer_NrEntries;
    UInt8                        MacCore_Max_Neighbours;
#endif // defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION) || defined(GP_DIVERSITY_JUMPTABLES)
    UInt8                        MacCore_NrOfStacks;
#if defined(GP_DIVERSITY_JUMPTABLES)
    Bool                         DiversityAssociationOriginator;
    Bool                         DiversityAssociationRecipient;
    Bool                         DiversityPollOriginator;
    Bool                         DiversityPollRecipient;
    Bool                         DiversityScanEdOriginator;
    Bool                         DiversityScanActiveOriginator;
    Bool                         DiversityScanOrphanOriginator;
    Bool                         DiversityScanActiveRecipient;
    Bool                         DiversityScanOrphanRecipient;
    Bool                         DiversityIndirectTransmission;
    Bool                         DiversityAddressModeOverrideForBeacons;
    Bool                         DiversityRawFrames;
    Bool                         DiversityRegionalDomainSettings;
    Bool                         DiversityDiagCntrs;
    Bool                         DiversityTimedTx;
#endif //defined(GP_DIVERSITY_JUMPTABLES)
#if defined(GP_MACCORE_DIVERSITY_RAW_FRAMES)  || defined(GP_DIVERSITY_JUMPTABLES)
    MacCore_RawFrameInfo_t      *MacCore_RawFrameInfoPtr;
#endif //defined(GP_MACCORE_DIVERSITY_RAW_FRAMES)  || defined(GP_DIVERSITY_JUMPTABLES)
} gpMacCore_ConstGlobalVars_t;

extern       gpMacCore_GlobalVars_t      gpMacCore_GlobalVars;
extern const gpMacCore_ConstGlobalVars_t gpMacCore_ConstGlobalVars;

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
    // in case we are in ROM, we need to retrieve the pointer via a shim table entry
#define GP_MACCORE_GET_GLOBALS()          ((gpMacCore_GlobalVars_t*)      JumpTables_DataTable.gpMacCore_GlobalVars_ptr)
#define GP_MACCORE_GET_GLOBALS_CONST()    ((gpMacCore_ConstGlobalVars_t*) JumpTables_DataTable.gpMacCore_ConstGlobalVars_ptr)
#define MACCORE_INDTX_ENTRIES               (GP_MACCORE_GET_GLOBALS_CONST()->MacCore_IndTxBuffer_NrEntries)
#define MACCORE_MAX_NEIGHBOURS()            (GP_MACCORE_GET_GLOBALS_CONST()->MacCore_Max_Neighbours)
#define DIVERSITY_NR_OF_STACKS              (GP_MACCORE_GET_GLOBALS_CONST()->MacCore_NrOfStacks)
#define DIVERSITY_ASSOCIATION_ORIGINATOR()  (GP_MACCORE_GET_GLOBALS_CONST()->DiversityAssociationOriginator)
#define DIVERSITY_ASSOCIATION_RECIPIENT()   (GP_MACCORE_GET_GLOBALS_CONST()->DiversityAssociationRecipient)
#define DIVERSITY_POLL_ORIGINATOR()         (GP_MACCORE_GET_GLOBALS_CONST()->DiversityPollOriginator)
#define DIVERSITY_POLL_RECIPIENT()          (GP_MACCORE_GET_GLOBALS_CONST()->DiversityPollRecipient)
#define DIVERSITY_SCAN_ED_ORIGINATOR()      (GP_MACCORE_GET_GLOBALS_CONST()->DiversityScanEdOriginator)
#define DIVERSITY_SCAN_ACTIVE_ORIGINATOR()  (GP_MACCORE_GET_GLOBALS_CONST()->DiversityScanActiveOriginator)
#define DIVERSITY_SCAN_ORPHAN_ORIGINATOR()  (GP_MACCORE_GET_GLOBALS_CONST()->DiversityScanOrphanOriginator)
#define DIVERSITY_SCAN_ACTIVE_RECIPIENT()   (GP_MACCORE_GET_GLOBALS_CONST()->DiversityScanActiveRecipient)
#define DIVERSITY_SCAN_ORPHAN_RECIPIENT()   (GP_MACCORE_GET_GLOBALS_CONST()->DiversityScanOrphanRecipient)
#define DIVERSITY_INDIRECT_TRANSMISSION()   (GP_MACCORE_GET_GLOBALS_CONST()->DiversityIndirectTransmission)
#define DIVERSITY_ADDRESSMODEOVERRIDEFORBEACONS() (GP_MACCORE_GET_GLOBALS_CONST()->DiversityAddressModeOverrideForBeacons)
#define DIVERSITY_RAW_FRAMES()              (GP_MACCORE_GET_GLOBALS_CONST()->DiversityRawFrames)
#define DIVERSITY_REGIONALDOMAINSETTINGS()  (GP_MACCORE_GET_GLOBALS_CONST()->DiversityRegionalDomainSettings)
#define DIVERSITY_DIAGCNTRS()               (GP_MACCORE_GET_GLOBALS_CONST()->DiversityDiagCntrs)
#define DIVERSITY_TIMEDTX()                 (GP_MACCORE_GET_GLOBALS_CONST()->DiversityTimedTx)
#else // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
    // in case we're not in ROM, we can directly fetch the address of the global vars.
extern gpMacCore_GlobalVars_t gpMacCore_globals;
#define GP_MACCORE_GET_GLOBALS()          ((gpMacCore_GlobalVars_t*)     &gpMacCore_GlobalVars)
#define GP_MACCORE_GET_GLOBALS_CONST()    ((gpMacCore_ConstGlobalVars_t*)&gpMacCore_ConstGlobalVars)
#define MACCORE_INDTX_ENTRIES             GP_MACCORE_INDTX_ENTRIES
#define MACCORE_MAX_NEIGHBOURS()          GP_MACCORE_MAX_NEIGHBOURS
#define DIVERSITY_NR_OF_STACKS            GP_DIVERSITY_NR_OF_STACKS
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
#define DIVERSITY_ASSOCIATION_ORIGINATOR()  (true)
#else
#define DIVERSITY_ASSOCIATION_ORIGINATOR()  (false)
#endif
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
#define DIVERSITY_ASSOCIATION_RECIPIENT()   (true)
#else
#define DIVERSITY_ASSOCIATION_RECIPIENT()   (false)
#endif
#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
#define DIVERSITY_POLL_ORIGINATOR()         (true)
#else
#define DIVERSITY_POLL_ORIGINATOR()         (false)
#endif
#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
#define DIVERSITY_POLL_RECIPIENT()          (true)
#else
#define DIVERSITY_POLL_RECIPIENT()          (false)
#endif
#ifdef GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR
#define DIVERSITY_SCAN_ED_ORIGINATOR()      (true)
#else
#define DIVERSITY_SCAN_ED_ORIGINATOR()      (false)
#endif
#ifdef GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR
#define DIVERSITY_SCAN_ACTIVE_ORIGINATOR()  (true)
#else
#define DIVERSITY_SCAN_ACTIVE_ORIGINATOR()  (false)
#endif
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
#define DIVERSITY_SCAN_ORPHAN_ORIGINATOR()  (true)
#else
#define DIVERSITY_SCAN_ORPHAN_ORIGINATOR()  (false)
#endif
#ifdef GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT
#define DIVERSITY_SCAN_ACTIVE_RECIPIENT()   (true)
#else
#define DIVERSITY_SCAN_ACTIVE_RECIPIENT()   (false)
#endif
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
#define DIVERSITY_SCAN_ORPHAN_RECIPIENT()   (true)
#else
#define DIVERSITY_SCAN_ORPHAN_RECIPIENT()   (false)
#endif
#ifdef GP_DIVERSITY_GPHAL_INDIRECT_TRANSMISSION
#define DIVERSITY_INDIRECT_TRANSMISSION()   (true)
#else
#define DIVERSITY_INDIRECT_TRANSMISSION()   (false)
#endif
#define DIVERSITY_ADDRESSMODEOVERRIDEFORBEACONS() (false)
#ifdef GP_MACCORE_DIVERSITY_RAW_FRAMES
#define DIVERSITY_RAW_FRAMES()              (true)
#else
#define DIVERSITY_RAW_FRAMES()              (false)
#endif
#define DIVERSITY_REGIONALDOMAINSETTINGS()  (false)
#define DIVERSITY_DIAGCNTRS()               (false)
#define DIVERSITY_TIMEDTX()                 (false)
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_DataTable.h"
#include "gpMacCore_CodeJumpTableRom_Defs_defs.h"
#include "gpMacCore_CodeJumpTableFlash_Defs_defs.h"
#include "gpMacCore_CodeJumpTableRom_Defs_defs.h"
#include "gpMacCore_CodeJumpTableFlash_Defs_defs_Main.h"
#include "gpHal_CodeJumpTableRom_Defs_gpglobal.h" // make sure the gpHal_cb.... go into ROM
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

#if defined(GP_MACCORE_DIVERSITY_FFD) || defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)
void MacCore_IndTxBufferCleanUp(gpMacCore_StackId_t stackId);
UInt8 MacCore_IndTxBufferNbrActiveElements(gpMacCore_StackId_t stackId);
#endif // defined(GP_MACCORE_DIVERSITY_FFD) || defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)

void MacCore_StopRunningRequests(gpMacCore_StackId_t stackId);

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

gpMacCore_AddressInfo_t* MacCore_GetNeighbour(UInt8 stackId, UInt8 neighbour);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */


/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
Bool MacCore_IndicateBeaconNotificationsNeeded(void);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_GPMAC_CORE_DEFS_H_

