/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpBsp.h
 *   This file manages how the BSP header file is included
 *
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _BSP_H_
#define _BSP_H_

#include "global.h"


#if defined(GP_COMP_CHIPEMU)
// Chip emulation definitions
#if   defined(GP_DIVERSITY_GPHAL_K8E)
// Almost identical chips. Same BSP is used for emulation.
#include "gpBsp_chipemu_k8c.h"
#else
#error Unsupported chip for chip emulation
#endif

#elif defined(GP_COMP_EXTSTORAGE)
// Storage device definitions
#if defined(GP_DIVERSITY_M25P80_SPIFLASH)
#include "gpBsp_m25p80_flash.h"
#elif defined(GP_DIVERSITY_A25L080_SPIFLASH)
#include "gpBsp_a25l080_flash.h"
#elif defined(GP_DIVERSITY_AT25SF081_SPIFLASH)
#include "gpBsp_at25sf081_flash.h"
#else
#error No external storage device definition
#endif

#endif //GP_COMP_EXTSTORAGE

#if defined(__SDCC)
/* the "#include GP_BSP_FILENAME" construction doesn't seem to */
/* work for the SDCC compiler. In this case we fall back to the old bsp selection */
#else

#if defined(GP_BSP_FILENAME)
/* Include BSP Header */
#include GP_BSP_FILENAME
#endif
#endif //defined(__SDCC)
#endif // _BSP_H_
