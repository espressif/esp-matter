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
 * @file pdm_ram_storage_glue.h
 * Interface for the glue between PDM and RAM Buffer
 *
 */

#ifndef PDM_RAM_STORAGE_GLUE_H_
#define PDM_RAM_STORAGE_GLUE_H_

#include "ram_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ENABLE_STORAGE_DYNAMIC_MEMORY
/* pBuffer will be resized (if needed) in case it can't accomodate a new record */
rsError ramStorageResize(ramBufferDescriptor **pBuffer, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength);
#endif

/* Return a RAM buffer with initialSize and populated with the contents of NVM ID - if found in flash
 * Main use case is for dynamic memory allocation
 * In case static memory allocation is used, initialSize is unused
 */
ramBufferDescriptor *getRamBuffer(uint16_t nvmId, uint16_t initialSize);

#ifdef __cplusplus
}
#endif

#endif /* PDM_RAM_STORAGE_GLUE_H_ */
