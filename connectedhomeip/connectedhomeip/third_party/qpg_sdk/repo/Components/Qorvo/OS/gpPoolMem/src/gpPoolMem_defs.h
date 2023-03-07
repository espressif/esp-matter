/*
 * Copyright (c) 2012, 2014, 2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
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

#ifndef _GPPOOLMEM_DEFS_H_
#define _GPPOOLMEM_DEFS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>
#include "gpPoolMem.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

 /** @ingroup DEF_POOLMEM
 *  @brief Size of Chunk size 1
*/
#ifndef GP_POOLMEM_CHUNK_SIZE1
#define GP_POOLMEM_CHUNK_SIZE1 50
#endif //GP_POOLMEM_CHUNK_SIZE1
 /** @ingroup DEF_POOLMEM
 *  @brief Size of Chunk size 2
*/
#ifndef GP_POOLMEM_CHUNK_SIZE2
#define GP_POOLMEM_CHUNK_SIZE2 100
#endif //GP_POOLMEM_CHUNK_SIZE2
 /** @ingroup DEF_POOLMEM
 *  @brief Size of Chunk size 3
*/
#ifndef GP_POOLMEM_CHUNK_SIZE3
#define GP_POOLMEM_CHUNK_SIZE3 150
#endif //GP_POOLMEM_CHUNK_SIZE3
  /** @ingroup DEF_POOLMEM
 *  @brief Amount of Chunk Size 1
*/
#ifndef GP_POOLMEM_CHUNK_AMOUNT1
#define GP_POOLMEM_CHUNK_AMOUNT1 1
#endif //GP_POOLMEM_CHUNK_AMOUNT1
  /** @ingroup DEF_POOLMEM
 *  @brief Amount of Chunk Size 2
*/
#ifndef GP_POOLMEM_CHUNK_AMOUNT2
#define GP_POOLMEM_CHUNK_AMOUNT2 1
#endif //GP_POOLMEM_CHUNK_AMOUNT2
  /** @ingroup DEF_POOLMEM
 *  @brief Amount of Chunk Size 3
*/
#ifndef GP_POOLMEM_CHUNK_AMOUNT3
#define GP_POOLMEM_CHUNK_AMOUNT3 0
#endif //GP_POOLMEM_CHUNK_AMOUNT3

/** @ingroup DEF_POOLMEM
 *  @brief Amount of PD Size
*/

#if defined (GP_DIVERSITY_PD_USE_PBM_VARIANT)
    #define GP_POOLMEM_PD_AMOUNT         0
#else // defined (GP_DIVERSITY_PD_USE_RAM_VARIANT)
#ifndef GP_POOLMEM_PD_AMOUNT
    #ifdef GP_PD_NR_OF_HANDLES
        #define GP_POOLMEM_PD_AMOUNT GP_PD_NR_OF_HANDLES
    #else
        #define GP_POOLMEM_PD_AMOUNT 5
    #endif //GP_PD_NR_OF_HANDLES
#endif // GP_POOLMEM_PD_AMOUNT
#endif


#ifdef ALIGNMENT_NEEDED
typedef UInt32 GUARD;
#else
typedef UInt16 GUARD;
#endif
  /** @ingroup DEF_POOLMEM
 *  @brief Guard Pattern
*/

#ifdef ALIGNMENT_NEEDED
#define GP_POOLMEM_GUARD 0xCA4EE4AC
#else
#define GP_POOLMEM_GUARD 0xCA4E
#endif
  /** @ingroup DEF_POOLMEM
 *  @brief NotUsed pattern
*/
#define GP_POOLMEM_NOTUSED 0xFF

/* Chunk structure, max 3 different sizes to use */

/** @ingroup DEF_POOLMEM
 *  @brief Structure of Chunk s1
*/
typedef    struct gpPoolMem_Chunks1 {
    GUARD GuardStart;
    UInt8 Data[GP_POOLMEM_CHUNK_SIZE1];
    GUARD GuardEnd;
    Bool InUse;
}  gpPoolMem_Chunks1_t;


/** @ingroup DEF_POOLMEM
 *  @brief Structure of Chunk s2
*/
typedef    struct gpPoolMem_Chunks2 {
    GUARD GuardStart;
    UInt8 Data[GP_POOLMEM_CHUNK_SIZE2];
    GUARD GuardEnd;
    Bool InUse;
}  gpPoolMem_Chunks2_t;


/** @ingroup DEF_POOLMEM
 *  @brief Structure of Chunk s3
*/
typedef    struct gpPoolMem_Chunks3 {
    GUARD GuardStart;
    UInt8 Data[GP_POOLMEM_CHUNK_SIZE3];
    GUARD GuardEnd;
    Bool InUse;
}  gpPoolMem_Chunks3_t;

/** @ingroup DEF_POOLMEM
 *  @brief Structure of Chunk pd
*/
typedef    struct gpPoolMem_Chunkpd {
    GUARD GuardStart;
    UInt8 Data[GP_POOLMEM_PD_SIZE];
    GUARD GuardEnd;
    Bool InUse;
}  gpPoolMem_Chunkpd_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void PoolMem_Init(void);
void *PoolMem_Malloc (UInt8 comp_id, UInt32 nbytes, Bool try_);
void PoolMem_Free (void* pData);

#endif // _GPPOOLMEM_DEFS_H_
