
/*
 * Copyright (c) 2011-2012, 2014, 2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
 *
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


#ifndef _GPPOOLMEM_H_
#define _GPPOOLMEM_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpPoolMem_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

//DOCUMENTATION GENERATION: no @file required as all documented items are refered to a group

/**
 *@defgroup DEF_POOLMEM General PoolMem Definitions
 * @brief The general PoolMem Definitions
 *
 * Maximal 3 different sizes of chunks available
 *
 * @image html chunk.png
 * @image latex chunk.png "Single Chunk structure"
 *
 *@defgroup GEN_POOLMEM General PoolMem Public Functions
 * @brief The general public PoolMem functionality is implemented in these functions
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include <global.h>
#include "gpPd.h" // for gpPd_Descriptor_t
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_POOLMEM_MALLOC(nbytes)    gpPoolMem_Malloc(GP_COMPONENT_ID,(nbytes), false)
#define GP_POOLMEM_TRYMALLOC(nbytes) gpPoolMem_Malloc(GP_COMPONENT_ID,(nbytes), true)


#define GP_POOLMEM_PD_SIZE (sizeof(gpPd_Descriptor_t))

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpPoolMem_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

/**@ingroup GEN_POOLMEM
 * @brief This function initialize the chunks, it basically sets all the guards and the InUse flags on false.
 *
*/
void gpPoolMem_Init(void);

/**@ingroup GEN_POOLMEM
 * @brief This function is almost similar as PoolMem_Init; invoked at warm restart.
 *
*/
void gpPoolMem_Reset(void);

/**@ingroup GEN_POOLMEM
 * @brief This function allocates a free chunk
 *
* @param nbytes    Minimal Chunk size
* @param try       avoid assert and returning a NULL is handled
* @return void*    Returns pointer to allocated chunk
* @return Assert   When no chunk free (depends on try)
* @return Assert   When nbytes > size of biggest chunk
* @return Assert   When GuardEnd or GuardStart overwritten of free chunk
*/
void* gpPoolMem_Malloc (UInt8 comp_id, UInt32 nbytes, Bool try_);


/**@ingroup GEN_POOLMEM
 * @brief This function will free an allocated chunk
 *
* @param pData     Pointer to Chunk
* @return Assert   When wrong pointer given
* @return Assert   When GuardEnd or GuardStart overwritten
*/
void gpPoolMem_Free (void* pData);

/**@ingroup GEN_POOLMEM
 * @brief This function will dump all PoolMem content and check consistency
 *
* @param checkConsistency  Assert when consistency check fails
* @return Assert   When consistency is compromised
*/
void gpPoolMem_Dump(Bool checkConsistency);

#ifdef GP_COMP_UNIT_TEST
// Backdoor interface for Unit Tests
void PoolMem_Init(void);
void* PoolMem_Malloc (UInt8 comp_id, UInt32 nbytes, Bool try_);
void PoolMem_Free (void* pData);
void PoolMem_Free_ByCompId(UInt8 comp_id);
UInt8 PoolMem_InUse(void);
#endif //GP_COMP_UNIT_TEST

UInt32 gpPoolMem_GetMaxAvailableChunkSize(void);


/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif // _GPPOOLMEM_H_


