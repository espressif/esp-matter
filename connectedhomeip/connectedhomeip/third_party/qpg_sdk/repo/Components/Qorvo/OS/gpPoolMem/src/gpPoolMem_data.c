
/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * This file defines the pools used by the pool memory allocator
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright law
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc
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

/*****************************************************************************
 *                    Include Definitions
 *****************************************************************************/

#include "gpPoolMem.h"
#include "gpPoolMem_defs.h"

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
#if GP_POOLMEM_CHUNK_AMOUNT1 == 0
#error Minimum need 1 buffer to play with
#endif

//Various data
gpPoolMem_Chunks1_t Chunks1[GP_POOLMEM_CHUNK_AMOUNT1] GP_EXTRAM_SECTION_ATTR;
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
gpPoolMem_Chunks2_t Chunks2[GP_POOLMEM_CHUNK_AMOUNT2] GP_EXTRAM_SECTION_ATTR;
#endif
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
gpPoolMem_Chunks3_t Chunks3[GP_POOLMEM_CHUNK_AMOUNT3] GP_EXTRAM_SECTION_ATTR;
#endif
