/*
 *  Copyright (c) 2016-2022, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction
 *    for non-volatile storage of settings on K32W platform.
 *
 */
#include <openthread-core-config.h>

#include "fsl_os_abstraction.h"
#include <string.h>
#include <openthread/instance.h>
#include <openthread/platform/memory.h>
#include <openthread/platform/settings.h>
#include "utils/code_utils.h"

#include "PDM.h"
#include "pdm_ram_storage_glue.h"
#include "ram_storage.h"

static ramBufferDescriptor *ramDescr       = NULL;
static osaMutexId_t         pdmMutexHandle = NULL;

#define kNvmIdOTConfigData 0x4F00
#define kRamBufferInitialSize 1024

static otError mapRamStorageStatus(rsError rsStatus)
{
    otError error;

    switch (rsStatus)
    {
    case RS_ERROR_NONE:
        error = OT_ERROR_NONE;
        break;
    case RS_ERROR_NOT_FOUND:
        error = OT_ERROR_NOT_FOUND;
        break;
    default:
        error = OT_ERROR_NO_BUFS;
        break;
    }

    return error;
}

void otPlatSettingsInit(otInstance *aInstance, const uint16_t *aSensitiveKeys, uint16_t aSensitiveKeysLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aSensitiveKeys);
    OT_UNUSED_VARIABLE(aSensitiveKeysLength);
    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION((PDM_E_STATUS_OK == PDM_Init()), error = OT_ERROR_NO_BUFS);

    pdmMutexHandle = OSA_MutexCreate();
    otEXPECT_ACTION((NULL != pdmMutexHandle), error = OT_ERROR_NO_BUFS);

    ramDescr = getRamBuffer(kNvmIdOTConfigData, kRamBufferInitialSize);
    otEXPECT_ACTION(ramDescr != NULL, error = OT_ERROR_NO_BUFS);

exit:
    if ((error != OT_ERROR_NONE) && pdmMutexHandle)
    {
        OSA_MutexDestroy(pdmMutexHandle);
    }
    return;
}

void otPlatSettingsDeinit(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

otError otPlatSettingsGet(otInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    rsError ramStatus = RS_ERROR_NONE;

    OSA_MutexLock(pdmMutexHandle, osaWaitForever_c);
    ramStatus = ramStorageGet(ramDescr, aKey, aIndex, aValue, aValueLength);
    OSA_MutexUnlock(pdmMutexHandle);
    return mapRamStorageStatus(ramStatus);
}

otError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    rsError      ramStatus = RS_ERROR_NONE;
    PDM_teStatus pdmStatus = PDM_E_STATUS_OK;
    otError      error     = OT_ERROR_NONE;

    OSA_MutexLock(pdmMutexHandle, osaWaitForever_c);

#if ENABLE_STORAGE_DYNAMIC_MEMORY
    ramStatus = ramStorageResize(&ramDescr, aKey, aValue, aValueLength);
    otEXPECT_ACTION((RS_ERROR_NONE == ramStatus), error = mapRamStorageStatus(ramStatus));
#endif
    ramStatus = ramStorageSet(ramDescr, aKey, aValue, aValueLength);
    otEXPECT_ACTION((RS_ERROR_NONE == ramStatus), error = mapRamStorageStatus(ramStatus));

#if PDM_SAVE_IDLE
    pdmStatus = PDM_eSaveRecordDataInIdleTask((uint16_t)kNvmIdOTConfigData, ramDescr,
                                              ramDescr->ramBufferLen + kRamDescHeaderSize);
#else
    pdmStatus =
        PDM_eSaveRecordData((uint16_t)kNvmIdOTConfigData, ramDescr, ramDescr->ramBufferLen + kRamDescHeaderSize);
#endif

    otEXPECT_ACTION((PDM_E_STATUS_OK == pdmStatus), error = OT_ERROR_NO_BUFS);

exit:
    OSA_MutexUnlock(pdmMutexHandle);
    return error;
}

otError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    rsError      ramStatus = RS_ERROR_NONE;
    PDM_teStatus pdmStatus = PDM_E_STATUS_OK;
    otError      error     = OT_ERROR_NONE;

    OSA_MutexLock(pdmMutexHandle, osaWaitForever_c);

#if ENABLE_STORAGE_DYNAMIC_MEMORY
    ramStatus = ramStorageResize(&ramDescr, aKey, aValue, aValueLength);
    otEXPECT_ACTION((RS_ERROR_NONE == ramStatus), error = mapRamStorageStatus(ramStatus));
#endif
    ramStatus = ramStorageAdd(ramDescr, aKey, aValue, aValueLength);
    otEXPECT_ACTION((RS_ERROR_NONE == ramStatus), error = mapRamStorageStatus(ramStatus));

#if PDM_SAVE_IDLE
    pdmStatus = PDM_eSaveRecordDataInIdleTask((uint16_t)kNvmIdOTConfigData, ramDescr,
                                              ramDescr->ramBufferLen + kRamDescHeaderSize);
#else
    pdmStatus =
        PDM_eSaveRecordData((uint16_t)kNvmIdOTConfigData, ramDescr, ramDescr->ramBufferLen + kRamDescHeaderSize);
#endif

    otEXPECT_ACTION((PDM_E_STATUS_OK == pdmStatus), error = OT_ERROR_NO_BUFS);

exit:
    OSA_MutexUnlock(pdmMutexHandle);
    return error;
}

otError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex)
{
    OT_UNUSED_VARIABLE(aInstance);
    rsError      ramStatus = RS_ERROR_NONE;
    PDM_teStatus pdmStatus = PDM_E_STATUS_OK;
    otError      error     = OT_ERROR_NONE;

    OSA_MutexLock(pdmMutexHandle, osaWaitForever_c);
    ramStatus = ramStorageDelete(ramDescr, aKey, aIndex);
    otEXPECT_ACTION((RS_ERROR_NONE == ramStatus), error = mapRamStorageStatus(ramStatus));

#if PDM_SAVE_IDLE
    pdmStatus = PDM_eSaveRecordDataInIdleTask((uint16_t)kNvmIdOTConfigData, ramDescr,
                                              ramDescr->ramBufferLen + kRamDescHeaderSize);
#else
    pdmStatus =
        PDM_eSaveRecordData((uint16_t)kNvmIdOTConfigData, ramDescr, ramDescr->ramBufferLen + kRamDescHeaderSize);
#endif

    otEXPECT_ACTION((PDM_E_STATUS_OK == pdmStatus), error = OT_ERROR_NO_BUFS);

exit:
    OSA_MutexUnlock(pdmMutexHandle);
    return error;
}

void otPlatSettingsWipe(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    OSA_MutexLock(pdmMutexHandle, osaWaitForever_c);
    memset(ramDescr, 0, ramDescr->ramBufferLen + kRamDescHeaderSize);
#if ENABLE_STORAGE_DYNAMIC_MEMORY
    if (ramDescr)
    {
        otPlatFree(ramDescr);
        ramDescr = NULL;
    }
#endif
    OSA_MutexUnlock(pdmMutexHandle);
    OSA_MutexDestroy(pdmMutexHandle);

    PDM_vDeleteDataRecord(kNvmIdOTConfigData);
}
