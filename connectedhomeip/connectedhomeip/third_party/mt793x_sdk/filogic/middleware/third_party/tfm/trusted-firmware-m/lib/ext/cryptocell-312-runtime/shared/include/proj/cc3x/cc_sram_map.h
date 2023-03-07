/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_sram_map
 @{
 */

/*!
 @file
 @brief This file contains internal SRAM mapping definitions.
 */

#ifndef _CC_SRAM_MAP_H_
#define _CC_SRAM_MAP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*! The base address of the PKA in the PKA SRAM. */
#define CC_SRAM_PKA_BASE_ADDRESS                                0x0
/*! The size of the PKA SRAM in KB. */
#define CC_PKA_SRAM_SIZE_IN_KBYTES                6

/*! The SRAM address of the RND. */
#define CC_SRAM_RND_HW_DMA_ADDRESS                              0x0
/*! Addresses 0K-2KB in SRAM. Reserved for RND operations. */
#define CC_SRAM_RND_MAX_SIZE                                    0x800
/*! The maximal size of SRAM. */
#define CC_SRAM_MAX_SIZE                                        0x1000

#ifdef __cplusplus
}
#endif

/*!
 @}
 */
#endif /*_CC_SRAM_MAP_H_*/

