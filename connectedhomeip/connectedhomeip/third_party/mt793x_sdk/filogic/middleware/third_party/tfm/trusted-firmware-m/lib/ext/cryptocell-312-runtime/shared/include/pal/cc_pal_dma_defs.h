/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/*!
@file
@brief This file contains the platform-dependent DMA definitions.
@defgroup ssi_pal_dma_defs CryptoCell PAL DMA specific definitions
@{
@ingroup ssi_pal
*/

#ifndef _CC_PAL_DMA_DEFS_H
#define _CC_PAL_DMA_DEFS_H


#ifdef __cplusplus
extern "C"
{
#endif

/*! Definition for DMA buffer handle.*/
typedef void *CC_PalDmaBufferHandle;

/*! DMA directions configuration. */
typedef enum {
    CC_PAL_DMA_DIR_NONE = 0, /*!< No direction. */
    CC_PAL_DMA_DIR_TO_DEVICE = 1,   /*!< The original buffer is the input to the operation. It should be copied or mapped to the temporary buffer prior to activating the HW on it. */
    CC_PAL_DMA_DIR_FROM_DEVICE = 2, /*!< The temporary buffer holds the output of the HW. This API should copy or map it to the original output buffer.*/
    CC_PAL_DMA_DIR_BI_DIRECTION = 3, /*!< The result is written over the original data at the same address. Should be treated as \p CC_PAL_DMA_DIR_TO_DEVICE and \p CC_PAL_DMA_DIR_FROM_DEVICE.*/
    CC_PAL_DMA_DIR_MAX,      /*!< Maximal DMA direction options. */
    CC_PAL_DMA_DIR_RESERVE32 = 0x7FFFFFFF  /*!< Reserved.*/
}CCPalDmaBufferDirection_t;


#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif


