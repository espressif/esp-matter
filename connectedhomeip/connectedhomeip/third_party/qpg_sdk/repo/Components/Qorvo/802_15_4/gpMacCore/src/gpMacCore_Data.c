/*
 * Copyright (c) 2017-2018, Qorvo Inc
 *
 * gpMacCore_Data.c
 *   This file global gpMacCore variables.
 *
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG
// General includes
#include "gpAssert.h"
#include "gpLog.h"
#include "gpHal.h"
#include "gpMacCore.h"
#include "gpMacCore_defs.h"

#if defined(GP_DIVERSITY_JUMPTABLES)
#include "gpJumpTables_DataTable.h"
#endif// defined(GP_DIVERSITY_JUMPTABLES)

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_MACCORE

#define MACCORE_ADDRESS_MODE_INVALID ((gpMacCore_AddressMode_t)(-1))

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#if defined(GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR)
STATIC Int8                        MacCore_MinInterferenceLevels[GP_MACCORE_NUMBER_OF_CHANNELS];
#endif //defined(GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR)
#if defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)
STATIC gpMacCore_AddressInfo_t     MacCore_NeighbourList[GP_DIVERSITY_NR_OF_STACKS][GP_MACCORE_MAX_NEIGHBOURS];
STATIC gpMacCore_DataPendingMode_t MacCore_DataPendingModePerStack[GP_DIVERSITY_NR_OF_STACKS];
STATIC MacCore_IndTxElement_t*     MacCore_IndTxBuffer[GP_MACCORE_INDTX_ENTRIES];
STATIC gpMacCore_StackId_t         pStackId[GP_DIVERSITY_NR_OF_STACKS];
#endif // defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)
#if defined(GP_MACCORE_DIVERSITY_RAW_FRAMES)
STATIC MacCore_RawFrameInfo_t      MacCore_RawFrameInfo_local;
#endif //defined(GP_MACCORE_DIVERSITY_RAW_FRAMES)

gpMacCore_GlobalVars_t      gpMacCore_GlobalVars = {
    .gpMacCore_pScanState                       = NULL,
#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)  || defined(GP_DIVERSITY_JUMPTABLES)
    .gpMacCore_pAssocRspNotUsed                 = NULL,
    .gpMacCore_pAssocRspMultiple                = NULL,
#endif //defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)  || defined(GP_DIVERSITY_JUMPTABLES)
#if defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR) || defined(GP_DIVERSITY_JUMPTABLES)
    .gpMacCore_pPollReqArgs                     = NULL,
#endif //defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR) || defined(GP_DIVERSITY_JUMPTABLES)

#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT) || defined(GP_DIVERSITY_JUMPTABLES)
    .MacCore_NumberAssocRsp                     = 0,
#endif //defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT) || defined(GP_DIVERSITY_JUMPTABLES)

};

const gpMacCore_ConstGlobalVars_t gpMacCore_ConstGlobalVars =
{
#if defined(GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR)
    MacCore_MinInterferenceLevels,
#elif defined(GP_DIVERSITY_JUMPTABLES)
    NULL,
#endif //defined(GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR)
#if defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)
    &(MacCore_DataPendingModePerStack[0]),
    &(pStackId[0]),
    &(MacCore_IndTxBuffer[0]),
#elif defined(GP_DIVERSITY_JUMPTABLES)
    NULL,
    NULL,
    NULL,
#endif // defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)
#if defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION) || defined(GP_DIVERSITY_JUMPTABLES)
    GP_MACCORE_INDTX_ENTRIES,
    GP_MACCORE_MAX_NEIGHBOURS,
#endif // defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION) || defined(GP_DIVERSITY_JUMPTABLES)
    GP_DIVERSITY_NR_OF_STACKS,
#if defined(GP_DIVERSITY_JUMPTABLES)
    DIVERSITY_ASSOCIATION_ORIGINATOR(),
    DIVERSITY_ASSOCIATION_RECIPIENT(),
    DIVERSITY_POLL_ORIGINATOR(),
    DIVERSITY_POLL_RECIPIENT(),
    DIVERSITY_SCAN_ED_ORIGINATOR(),
    DIVERSITY_SCAN_ACTIVE_ORIGINATOR(),
    DIVERSITY_SCAN_ORPHAN_ORIGINATOR(),
    DIVERSITY_SCAN_ACTIVE_RECIPIENT(),
    DIVERSITY_SCAN_ORPHAN_RECIPIENT(),
    DIVERSITY_INDIRECT_TRANSMISSION(),
    DIVERSITY_ADDRESSMODEOVERRIDEFORBEACONS(),
    DIVERSITY_RAW_FRAMES(),
    DIVERSITY_REGIONALDOMAINSETTINGS(),
    DIVERSITY_DIAGCNTRS(),
    DIVERSITY_TIMEDTX(),
#endif // defined(GP_DIVERSITY_JUMPTABLES)
#if defined(GP_MACCORE_DIVERSITY_RAW_FRAMES)
    &(MacCore_RawFrameInfo_local),
#elif defined(GP_DIVERSITY_JUMPTABLES)
    NULL,
#endif //defined(GP_MACCORE_DIVERSITY_RAW_FRAMES)  || defined(GP_DIVERSITY_JUMPTABLES)
};


/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Function Definitions
 *****************************************************************************/

#if defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)
gpMacCore_AddressInfo_t* MacCore_GetNeighbour(UInt8 stackId, UInt8 neighbour)
{
    return &(MacCore_NeighbourList[stackId][neighbour]);
}
#endif // defined(GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION)
