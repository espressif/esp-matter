/*
 * Copyright (c) 2021-2022, Qorvo Inc
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

/** @file Implementation of Nvm access protection at multi access case */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4

#include "gpAssert.h"
#include "gpNvm.h"

#ifdef HAL_MUTEX_SUPPORTED
#include "hal_Mutex.h"
#endif //HAL_MUTEX_SUPPORTED
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Local Variable
 *****************************************************************************/
#ifdef HAL_MUTEX_SUPPORTED
#ifdef GP_NVM_DIVERSITY_ELEMIF_KEYMAP
HAL_CRITICAL_SECTION_DEF(gpNvm_ElemIfProtectionMutex)
#endif
#ifdef GP_NVM_DIVERSITY_SUBPAGEDFLASH_V2
HAL_CRITICAL_SECTION_DEF(gpNvm_SubpagedFlashProtectionMutex)
#endif
#endif //HAL_MUTEX_SUPPORTED

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
void gpNvm_InitProtection(void)
{
#ifdef HAL_MUTEX_SUPPORTED
#ifdef GP_NVM_DIVERSITY_ELEMIF_KEYMAP
    hal_MutexCreate(&gpNvm_ElemIfProtectionMutex);

    if(!hal_MutexIsValid(gpNvm_ElemIfProtectionMutex))
    {
      GP_ASSERT(GP_DIVERSITY_ASSERT_LEVEL_SYSTEM, false);
    }
#endif //GP_NVM_DIVERSITY_ELEMIF_KEYMAP

#ifdef GP_NVM_DIVERSITY_SUBPAGEDFLASH_V2
    hal_MutexCreate(&gpNvm_SubpagedFlashProtectionMutex);

    if(!hal_MutexIsValid(gpNvm_SubpagedFlashProtectionMutex))
    {
      GP_ASSERT(GP_DIVERSITY_ASSERT_LEVEL_SYSTEM, false);
    }
#endif //GP_NVM_DIVERSITY_SUBPAGEDFLASH_V2
#endif //HAL_MUTEX_SUPPORTED
}

#ifdef GP_NVM_DIVERSITY_ELEMIF_KEYMAP
void gpNvm_BackupProtected(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation)
{
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexAcquire(gpNvm_ElemIfProtectionMutex);
#endif
    gpNvm_Backup(componentId, tagId, pRamLocation);
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexRelease(gpNvm_ElemIfProtectionMutex);
#endif
}

void gpNvm_RestoreProtected(UInt8 componentId, UInt8 tagId, UInt8* pRamLocation)
{
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexAcquire(gpNvm_ElemIfProtectionMutex);
#endif
    gpNvm_Restore(componentId, tagId, pRamLocation);
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexRelease(gpNvm_ElemIfProtectionMutex);
#endif
}

void gpNvm_ClearProtected(UInt8 componentId, UInt8 tagId)
{
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexAcquire(gpNvm_ElemIfProtectionMutex);
#endif
    gpNvm_Clear(componentId, tagId);
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexRelease(gpNvm_ElemIfProtectionMutex);
#endif
}

void gpNvm_RegisterElementsProtected(const ROM gpNvm_IdentifiableTag_t* pTag, UInt8 nbrOfTags)
{
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexAcquire(gpNvm_ElemIfProtectionMutex);
#endif
    gpNvm_RegisterElements(pTag, nbrOfTags);
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexRelease(gpNvm_ElemIfProtectionMutex);
#endif
}
#endif //GP_NVM_DIVERSITY_ELEMIF_KEYMAP

#ifdef GP_NVM_DIVERSITY_SUBPAGEDFLASH_V2
gpNvm_Result_t gpNvm_BuildLookupProtected(
    gpNvm_LookupTable_Handle_t* pHandle,
    gpNvm_PoolId_t poolIdSpec,
    gpNvm_UpdateFrequency_t updateFrequencySpec,
    UInt8 tokenMaskLengthSpec,
    UInt8* pTokenMask,
    gpNvm_KeyIndex_t maxNrMatches,
    gpNvm_KeyIndex_t *pNrOfMatches)
{
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexAcquire(gpNvm_SubpagedFlashProtectionMutex);
#endif
    gpNvm_Result_t ret = gpNvm_BuildLookup(pHandle, poolIdSpec, updateFrequencySpec,
                            tokenMaskLengthSpec, pTokenMask, maxNrMatches, pNrOfMatches);
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexRelease(gpNvm_SubpagedFlashProtectionMutex);
#endif
    return ret;
}

gpNvm_Result_t gpNvm_AcquireLutHandleProtected(
    gpNvm_LookupTable_Handle_t* pHandle,
    gpNvm_PoolId_t poolId,
    gpNvm_UpdateFrequency_t updateFrequencySpec,
    UInt8 tokenLength,
    UInt8* pToken,
    Bool* freeAfterUse,
    gpNvm_KeyIndex_t maxNbrOfMatches)
{
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexAcquire(gpNvm_SubpagedFlashProtectionMutex);
#endif
    gpNvm_Result_t ret = gpNvm_AcquireLutHandle(pHandle, poolId, updateFrequencySpec,
                            tokenLength, pToken, freeAfterUse, maxNbrOfMatches);
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexRelease(gpNvm_SubpagedFlashProtectionMutex);
#endif
    return ret;
}

void gpNvm_FreeLookupProtected(gpNvm_LookupTable_Handle_t h)
{
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexAcquire(gpNvm_SubpagedFlashProtectionMutex);
#endif
    gpNvm_FreeLookup(h);
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexRelease(gpNvm_SubpagedFlashProtectionMutex);
#endif
}

gpNvm_Result_t gpNvm_ReadUniqueProtected(
    gpNvm_LookupTable_Handle_t handle,
    gpNvm_PoolId_t poolId,
    gpNvm_UpdateFrequency_t updateFrequencySpec,
    gpNvm_UpdateFrequency_t *pUpdateFrequency,
    UInt8 tokenMaskLength,
    UInt8* pTokenMask,
    UInt8 maxDataLength,
    UInt8* pDataLength,
    UInt8* pData)
{
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexAcquire(gpNvm_SubpagedFlashProtectionMutex);
#endif
    gpNvm_Result_t ret = gpNvm_ReadUnique(handle, poolId, updateFrequencySpec, pUpdateFrequency,
                            tokenMaskLength, pTokenMask, maxDataLength, pDataLength, pData);
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexRelease(gpNvm_SubpagedFlashProtectionMutex);
#endif
    return ret;
}

gpNvm_Result_t gpNvm_ReadNextProtected(
    gpNvm_LookupTable_Handle_t handle,
    gpNvm_PoolId_t poolId,
    gpNvm_UpdateFrequency_t* pUpdateFrequency,
    UInt8 maxTokenLength,
    UInt8* pTokenLength,
    UInt8* pToken,
    UInt8 maxDataLength,
    UInt8* pDataLength,
    UInt8* pData)
{
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexAcquire(gpNvm_SubpagedFlashProtectionMutex);
#endif
    gpNvm_Result_t ret = gpNvm_ReadNext(handle, poolId, pUpdateFrequency, maxTokenLength,
                            pTokenLength, pToken, maxDataLength, pDataLength, pData);
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexRelease(gpNvm_SubpagedFlashProtectionMutex);
#endif
    return ret;
}

gpNvm_Result_t gpNvm_WriteProtected(
    gpNvm_PoolId_t poolId,
    gpNvm_UpdateFrequency_t updateFrequency,
    UInt8 tokenLength,
    UInt8* pToken,
    UInt8 dataLength,
    UInt8* pData)
{
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexAcquire(gpNvm_SubpagedFlashProtectionMutex);
#endif
    gpNvm_Result_t ret = gpNvm_Write( poolId, updateFrequency, tokenLength, pToken,
                                      dataLength, pData);
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexRelease(gpNvm_SubpagedFlashProtectionMutex);
#endif
    return ret;

}

gpNvm_Result_t gpNvm_RemoveProtected(
    gpNvm_PoolId_t poolId,
    gpNvm_UpdateFrequency_t updateFrequencySpec,
    UInt8 tokenLength,
    UInt8* pToken)
{
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexAcquire(gpNvm_SubpagedFlashProtectionMutex);
#endif
    gpNvm_Result_t ret = gpNvm_Remove(poolId, updateFrequencySpec, tokenLength, pToken);
#ifdef HAL_MUTEX_SUPPORTED
    hal_MutexRelease(gpNvm_SubpagedFlashProtectionMutex);
#endif
    return ret;
}
#endif //GP_NVM_DIVERSITY_SUBPAGEDFLASH_V2
