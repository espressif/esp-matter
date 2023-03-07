/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * Safety net for asserts during startup codepath.
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
#include "gpAssert.h"
#include "gpLog.h"
#include "gpNvm_defs.h"

#define GP_COMPONENT_ID GP_COMPONENT_ID_NVM

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static Bool Nvm_SafetyNetEnabled;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void Nvm_EnableSafetyNet(void)
{
    GP_LOG_PRINTF("safetynet ENABLE",0);
    Nvm_SafetyNetEnabled = true;
}

void Nvm_DisableSafetyNet(void)
{
    GP_LOG_PRINTF("safetynet DISABLE",0);
    Nvm_SafetyNetEnabled = false;
}

Bool Nvm_IsSafetyNetEnabled(void)
{
    return Nvm_SafetyNetEnabled;
}

void Nvm_SafetyNetHandler(gpAssert_AssertInfo_t assertInfo, UInt8 componentId, FLASH_STRING filename, UInt16 line)
{

    GP_LOG_PRINTF("safetynet kicks in...",0);

    /* log assert before dump */
#ifdef GP_DIVERSITY_LOG
    gpAssert_PrintLogString(componentId, filename, line);
#endif //GP_DIVERSITY_LOG

    gpNvm_PhysicalErasePool(gpNvm_PoolId_AllPoolIds);
}

