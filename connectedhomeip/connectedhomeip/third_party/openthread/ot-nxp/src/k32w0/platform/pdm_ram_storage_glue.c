/*
 *  Copyright (c) 2022, The OpenThread Authors.
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
 * @file pdm_ram_storage_glue.c
 * File used for the glue between PDM and RAM Buffer
 *
 */

#include "PDM.h"
#include "platform-k32w.h"
#include "ram_storage.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <utils/code_utils.h>
#include <openthread/platform/memory.h>

#if !ENABLE_STORAGE_DYNAMIC_MEMORY
#ifndef PDM_BUFFER_SIZE
#define PDM_BUFFER_SIZE (1024 + sizeof(ramBufferDescriptor)) /* kRamBufferInitialSize is 1024 */
#endif
static uint8_t sPdmBuffer[PDM_BUFFER_SIZE] __attribute__((aligned(4))) = {0};
#endif

#if ENABLE_STORAGE_DYNAMIC_MEMORY

extern void *otPlatRealloc(void *ptr, size_t aSize);

ramBufferDescriptor *getRamBuffer(uint16_t nvmId, uint16_t initialSize)
{
    ramBufferDescriptor *ramDescr         = NULL;
    bool_t               bLoadDataFromNvm = FALSE;
    uint16_t             bytesRead        = 0;
    uint16_t             recordSize       = 0;
    uint16_t             allocSize        = initialSize;

    /* Check if dataset is present and get its size */
    if (PDM_bDoesDataExist(nvmId, &recordSize))
    {
        bLoadDataFromNvm = TRUE;
        while (recordSize > allocSize)
        {
            // increase size until NVM data fits
            allocSize += kRamBufferReallocSize;
        }
    }

    if (allocSize <= kRamBufferMaxAllocSize)
    {
        ramDescr = (ramBufferDescriptor *)otPlatCAlloc(1, allocSize);
        if (ramDescr)
        {
            ramDescr->ramBufferLen = 0;

            if (bLoadDataFromNvm)
            {
                /* Try to load the dataset in RAM */
                if (PDM_E_STATUS_OK != PDM_eReadDataFromRecord(nvmId, ramDescr, recordSize, &bytesRead))
                {
                    memset(ramDescr, 0, allocSize);
                }
            }

            /* ramBufferMaxLen should hold the runtime allocated size */
            ramDescr->ramBufferMaxLen = allocSize - kRamDescHeaderSize;
        }
    }

    return ramDescr;
}

rsError ramStorageResize(ramBufferDescriptor **pBuffer, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    rsError              err            = RS_ERROR_NONE;
    uint16_t             allocSize      = (*pBuffer)->ramBufferMaxLen;
    const uint16_t       newBlockLength = sizeof(struct settingsBlock) + aValueLength;
    ramBufferDescriptor *ptr            = NULL;

    otEXPECT_ACTION((NULL != *pBuffer), err = RS_ERROR_NO_BUFS);

    if (allocSize < (*pBuffer)->ramBufferLen + newBlockLength)
    {
        while ((allocSize < (*pBuffer)->ramBufferLen + newBlockLength))
        {
            /* Need to realocate the memory buffer, increase size by kRamBufferReallocSize until NVM data fits */
            allocSize += kRamBufferReallocSize;
        }

        allocSize += kRamDescHeaderSize;

        if (allocSize <= kRamBufferMaxAllocSize)
        {
            ptr = (ramBufferDescriptor *)otPlatRealloc((void *)(*pBuffer), allocSize);
            otEXPECT_ACTION((NULL != ptr), err = RS_ERROR_NO_BUFS);
            *pBuffer                    = ptr;
            (*pBuffer)->ramBufferMaxLen = allocSize - kRamDescHeaderSize;
        }
        else
        {
            err = RS_ERROR_NO_BUFS;
        }
    }

exit:
    return err;
}

#else
ramBufferDescriptor *getRamBuffer(uint16_t nvmId, uint16_t initialSize)
{
    ramBufferDescriptor *ramDescr  = (ramBufferDescriptor *)&sPdmBuffer;
    uint16_t             bytesRead = 0;

    ramDescr->ramBufferMaxLen = PDM_BUFFER_SIZE - kRamDescHeaderSize;
    assert(initialSize <= ramDescr->ramBufferMaxLen);

    if (PDM_bDoesDataExist(nvmId, &bytesRead))
    {
        /* Try to load the dataset in RAM */
        if (PDM_E_STATUS_OK != PDM_eReadDataFromRecord(nvmId, ramDescr, PDM_BUFFER_SIZE, &bytesRead))
        {
            memset(ramDescr, 0, PDM_BUFFER_SIZE);
        }
    }

    return ramDescr;
}
#endif
