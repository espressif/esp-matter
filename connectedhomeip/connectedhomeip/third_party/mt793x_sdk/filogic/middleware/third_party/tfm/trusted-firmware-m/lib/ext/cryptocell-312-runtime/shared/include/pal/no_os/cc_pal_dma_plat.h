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
 * @brief   stub function, the function should initialize the DMA mapping of the platform (if needed)
 *
 * @param[in] buffSize - buffer size in Bytes
 * @param[in] physBuffAddr - physical start address of the memory to map
 *
 * @return Virtual start address of contiguous memory
 */
extern uint32_t CC_PalDmaInit(uint32_t  buffSize,    /*!< [in] Buffer size in Bytes. */
                  CCDmaAddr_t  physBuffAddr /*!< [in] Physical start address of the memory to map. */);

/**
 * @brief   free system resources created in PD_PAL_DmaInit()
 *
 *
 * @return void
 */
extern void CC_PalDmaTerminate(void);
#ifdef __cplusplus
}
#endif

#endif


