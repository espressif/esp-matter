/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_pal_sb_plat
 @{
 */

/*!
 @file
 @brief This file contains platform-dependent definitions used in the Boot Services code.
 */

#ifndef _CC_PAL_SB_PLAT_H
#define _CC_PAL_SB_PLAT_H

#include "cc_pal_types.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*! DMA address types: 32 bits or 64 bits, according to platform. */
typedef uint32_t        CCDmaAddr_t;
/*! CryptocCell address types: 32 bits or 64 bits, according to platform. */
typedef uint32_t        CCAddr_t;


#ifdef __cplusplus
}
#endif
/*!
 @}
 */

#endif

