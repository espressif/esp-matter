
/*
 * Copyright (c) 2011-2012, 2014, 2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * This file implements a pool memory allocator
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
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_POOLMEM

// General includes
#include "gpPoolMem.h"
#include "gpPoolMem_defs.h"


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

 /* Init amount of chunks inside memory */
 void gpPoolMem_Init(void)
 {
    PoolMem_Init();
 }

  /* Implement malloc function */
void *gpPoolMem_Malloc (UInt8 comp_id, UInt32 nbytes, Bool try_)
{
    void *handle;
    handle = PoolMem_Malloc(comp_id,nbytes, try_);
    return handle;
}

/* Implement free function */
void gpPoolMem_Free (void* pData)
/* Check guards and free chunk, if guard error -> assert */
{
    // If chunk not found, wrong pointer is given or InUse flag overwritten -> assert
    PoolMem_Free(pData);
}

#if   defined(GP_DIVERSITY_JUMPTABLES)
// Full debugging disabled in application, but ROM code will still call this function.
void gpPoolMem_SetDebugLine(UInt16 lineNumber)
{
    NOT_USED(lineNumber);
}
#endif // GP_POOLMEM_DIVERSITY_DUMP_FULL_POOL
