/*
 * Copyright (c) 2022, Qorvo Inc
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

/**
 * @file gpNvm_RW_Kx_SubpagedFlash_v2_patch.c
 *
 * @brief This file gives an implementation of the Non Volatile Memory component for internal FLASH on Kx chips with random write possibility.
 * Only required patch functions are captured here.
 */

//#define GP_LOCAL_LOG

#ifndef GP_DIVERSITY_ROMUSAGE_FOR_MATTER
#error Patch file only to be used when compiling full application with Matter ROM
#endif //GP_DIVERSITY_ROMUSAGE_FOR_MATTER

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_NVM

#include "global.h"
#include "gpLog.h"
#include "gpNvm.h"
#include "gpNvm_RW_Kx_SubpagedFlash.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// Check Matter ROM compilation settings compatibility
#if GP_NVM_MAX_TOKENLENGTH != 13
#error Token length defined to 13 in Matter ROM
#endif //GP_NVM_MAX_TOKENLENGTH

#ifndef GP_NVM_DIVERSITY_VARIABLE_SIZE
#error Variable size feature expected to be used with Matter ROM
#endif

#ifndef GP_NVM_DIVERSITY_VARIABLE_SETTINGS
#error Variable settings feature expected to be used with Matter ROM
#endif //GP_NVM_DIVERSITY_VARIABLE_SETTINGS

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    External Function Prototypes
 *****************************************************************************/

extern void Nvm_Pool_PhysicalErase(gpNvm_PoolId_t poolId);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/** @brief Physical erase the NVM pool.
 *  Warning : This function should be used only inside Nvm_SafetyNetHandler
 *
 *  @param poolId    Pool Id to be erased
 */
gpNvm_Result_t gpNvm_PhysicalErasePool(gpNvm_PoolId_t poolId)
{
    GP_LOG_PRINTF("gpNvm_PhysicalErasePool poolId:%d", 0, poolId);

    if ((poolId >= GP_NVM_NBR_OF_POOLS) && (poolId != gpNvm_PoolId_AllPoolIds))
    {
        return gpNvm_Result_Error;
    }
    Nvm_Pool_PhysicalErase(poolId);

    return gpNvm_Result_DataAvailable;
}
