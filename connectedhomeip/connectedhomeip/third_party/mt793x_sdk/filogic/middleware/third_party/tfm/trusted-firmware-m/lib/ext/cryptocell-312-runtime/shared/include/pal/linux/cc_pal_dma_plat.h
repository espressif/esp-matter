/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_PAL_DMA_PLAT_H
#define _CC_PAL_DMA_PLAT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_address_defs.h"

/**
 * @brief   Initializes contiguous memory pool required for CC_PalDmaContigBufferAllocate() and CC_PalDmaContigBufferFree(). Our
 *      example implementation is to mmap 0x30000000 and call to bpool(), for use of bget() in CC_PalDmaContigBufferAllocate(),
 *          and brel() in CC_PalDmaContigBufferFree().
 *
 * @return A non-zero value in case of failure.
 */
extern uint32_t CC_PalDmaInit(uint32_t  buffSize,    /*!< [in] Buffer size in Bytes. */
                  CCDmaAddr_t  physBuffAddr /*!< [in] Physical start address of the memory to map. */);

/**
 * @brief   free system resources created in CC_PalDmaInit()
 *
 * @param[in] buffSize - buffer size in Bytes
 *
 * @return void
 */
extern void CC_PalDmaTerminate(void);
#ifdef __cplusplus
}
#endif

#endif


