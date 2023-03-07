/*
 * Copyright (c) 2021, Qorvo Inc
 *
 * gpNvm_NvmProtect.h
 *   Mutex or critical section Nvm protection against simultaneous access from many tasks for ARM devices.
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _HAL_NVMPROTECTFREERTOS_H_
#define _HAL_NVMPROTECTFREERTOS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#ifdef GP_NVM_DIVERSITY_ELEMIF_KEYMAP
#include "gpNvm_ElemIf.h"
#endif
/*****************************************************************************
 *                    Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Function Prototype Definitions
 *****************************************************************************/
#ifdef GP_NVM_DIVERSITY_ELEMIF_KEYMAP
void gpNvm_InitProtection(void);
void gpNvm_BackupProtected(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation);
void gpNvm_RestoreProtected(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation);
void gpNvm_ClearProtected(UInt8 componentId, UInt8 tagId);
void gpNvm_RegisterElementsProtected(const ROM gpNvm_IdentifiableTag_t* pTag, UInt8 nbrOfTags);
#endif //GP_NVM_DIVERSITY_ELEMIF_KEYMAP

#ifdef GP_NVM_DIVERSITY_SUBPAGEDFLASH_V2
gpNvm_Result_t gpNvm_BuildLookupProtected(
    gpNvm_LookupTable_Handle_t* pHandle,
    gpNvm_PoolId_t poolIdSpec,
    gpNvm_UpdateFrequency_t updateFrequencySpec,
    UInt8 tokenMaskLengthSpec,
    UInt8* pTokenMask,
    gpNvm_KeyIndex_t maxNrMatches,
    gpNvm_KeyIndex_t *pNrOfMatches);

gpNvm_Result_t gpNvm_AcquireLutHandleProtected(
    gpNvm_LookupTable_Handle_t* pHandle,
    gpNvm_PoolId_t poolId,
    gpNvm_UpdateFrequency_t updateFrequencySpec,
    UInt8 tokenLength,
    UInt8* pToken,
    Bool* freeAfterUse,
    gpNvm_KeyIndex_t maxNbrOfMatches);

void gpNvm_FreeLookupProtected(gpNvm_LookupTable_Handle_t h);

gpNvm_Result_t gpNvm_ReadUniqueProtected(
    gpNvm_LookupTable_Handle_t handle,
    gpNvm_PoolId_t poolId,
    gpNvm_UpdateFrequency_t updateFrequencySpec,
    gpNvm_UpdateFrequency_t *pUpdateFrequency,
    UInt8 tokenMaskLength,
    UInt8* pTokenMask,
    UInt8 maxDataLength,
    UInt8* pDataLength,
    UInt8* pData);

gpNvm_Result_t gpNvm_ReadNextProtected(
    gpNvm_LookupTable_Handle_t handle,
    gpNvm_PoolId_t poolId,
    gpNvm_UpdateFrequency_t* pUpdateFrequency,
    UInt8 maxTokenLength,
    UInt8* pTokenLength,
    UInt8* pToken,
    UInt8 maxDataLength,
    UInt8* pDataLength,
    UInt8* pData);

gpNvm_Result_t gpNvm_WriteProtected(
    gpNvm_PoolId_t poolId,
    gpNvm_UpdateFrequency_t updateFrequency,
    UInt8 tokenLength,
    UInt8* pToken,
    UInt8 dataLength,
    UInt8* pData);

gpNvm_Result_t gpNvm_RemoveProtected(
    gpNvm_PoolId_t poolId,
    gpNvm_UpdateFrequency_t updateFrequencySpec,
    UInt8 tokenLength,
    UInt8* pToken);

#endif //GP_NVM_DIVERSITY_SUBPAGEDFLASH_V2
#endif //_HAL_NVMPROTECTFREERTOS_H_
