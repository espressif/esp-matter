/*
 *   Copyright (c) 2017, Qorvo Inc
 *
 *
 *   Declarations of the public functions and enumerations of gpBleInitiator.
 *
 *   This software is owned by Qorvo Inc
 *   and protected under applicable copyright laws.
 *   It is delivered under the terms of the license
 *   and is intended and supplied for use solely and
 *   exclusively with products manufactured by
 *   Qorvo Inc.
 *
 *
 *   THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *   CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *   IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *   LIMITED TO, IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A
 *   PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *   QORVO INC. SHALL NOT, IN ANY
 *   CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *   INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *   FOR ANY REASON WHATSOEVER.
 *
 *   $Header$
 *   $Change$
 *   $DateTime$
 */


#ifndef _GPBLEINITIATOR_H_
#define _GPBLEINITIATOR_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpBleInitiator_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/// @file "gpBleInitiator.h"
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpHci_Includes.h"
#include "gpBle.h"
#include "gpHal.h"
#include "gpBle_defs.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpBleInitiator_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

//Requests
void gpBleInitiator_Init(gpHal_BleCallbacks_t* pCallbacks);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
Bool gpBleInitiator_IsEnabled(void);

Bool gpBleInitiator_IsWhitelistUsed(void);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

void gpBleInitiator_Suspend(void);

void gpBleInitiator_Resume(gpHal_BleChannelMapHandle_t newChannelMapHandle);

UInt32 gpBleInitiator_GetScanIntervalUs(void);

UInt16 gpBleInitiator_GetScanWindowUnit(void);

void gpBleInitiator_Reset(Bool firstReset);

UInt16 gpBleInitiator_GetMinCeUnit(Ble_IntConnId_t connId);

gpHci_Result_t gpBle_LeCreateConnection(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);

gpHci_Result_t gpBle_LeCreateConnectionCancel(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);



//Indications

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_GPBLEINITIATOR_H_

