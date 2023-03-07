/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_general_defs
 @{
 */

/*!
 @file
 @brief This file contains general definitions for CryptoCell APIs.
 */



#ifndef _CC_ADDRESS_DEFS_H
#define _CC_ADDRESS_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/* Address types within CryptoCell. */
/*! The SRAM address type. */
typedef uint32_t CCSramAddr_t;
/*! The DMA address type. */
typedef uint32_t CCDmaAddr_t;

#ifdef __cplusplus
}
#endif

/*!
@}
 */

#endif

