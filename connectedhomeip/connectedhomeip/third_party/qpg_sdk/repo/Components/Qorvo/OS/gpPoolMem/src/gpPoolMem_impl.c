
/*
 * Copyright (c) 2012-2014, 2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
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

#define GP_COMPONENT_ID GP_COMPONENT_ID_POOLMEM
#define GP_MODULE_ID GP_MODULE_ID_POOLMEM

//#define GP_LOCAL_LOG

#include "gpLog.h"
#include "gpAssert.h"
#include "gpWmrk.h"
#include "gpPoolMem.h"
#include "gpPoolMem_defs.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define POOLMEM_CHECK(size,amount) do { \
    UIntLoop i; \
    Bool failure = false; \
    for (i = 0; i < amount; i++) \
    { \
        if(!(Chunk##size[i].GuardStart == GP_POOLMEM_GUARD && Chunk##size[i].GuardEnd == GP_POOLMEM_GUARD)) \
        { \
            GP_LOG_SYSTEM_PRINTF("BA-%u[%u]-grd fail",8,(UInt16)sizeof(Chunk##size[i].Data),(UInt16)i);\
            failure = true; \
        } \
    } \
    GP_ASSERT_SYSTEM(!failure); \
    } while(false)

#define POOLMEM_DUMP(size,amount) do { \
    UIntLoop i; \
    for (i = 0; i < amount; i++) \
    { \
        gpLog_PrintBuffer(sizeof(Chunk##size[i]), (UInt8*)&Chunk##size[i]); \
    } \
    } while(false)

/* Init chunks, put all InUse flags on false and fill Guard with pattern */
/* InUse flag on GP_POOLMEM_NOTUSED when not used. If used we will place CompId inside */
#define POOLMEM_INIT(size,amount) do { \
    UIntLoop i; \
    for (i = 0; i < amount; i++) \
    { \
        Chunk##size[i].InUse = GP_POOLMEM_NOTUSED; \
        Chunk##size[i].GuardStart = GP_POOLMEM_GUARD;\
        Chunk##size[i].GuardEnd = GP_POOLMEM_GUARD; \
    } \
    } while(false)

/* Look for free chunk and return pointer to data. Check guardStart before checking InUse flag. */
/* Component Id will be used as InUse flag */
#define POOLMEM_MALLOC(size,amount,nbytes,comp_id)    do { \
    UIntLoop i; \
    for (i = 0; i < amount; ++i) \
    { \
        if (Chunk##size[i].InUse == GP_POOLMEM_NOTUSED) \
        { \
            GP_LOG_PRINTF("BA-%u[%u]-CId:%u-S:%lu",8,(UInt16)sizeof(Chunk##size[i].Data),(UInt16)i,(UInt16)comp_id,(UInt32)nbytes);\
            GP_ASSERT_SYSTEM(Chunk##size[i].GuardStart == GP_POOLMEM_GUARD && Chunk##size[i].GuardEnd == GP_POOLMEM_GUARD);\
            Chunk##size[i].InUse = comp_id; \
            gpWmrk_CntrToLimit(gpPoolMemWmrk##size); \
            gpWmrk_CntrSet(gpPoolMemWmrkSize, nbytes); \
            return &(Chunk##size[i].Data[0]); \
        } \
    } \
    } while(false)

/* Look for chunk to be freed */
#define POOLMEM_FREE(size,amount) do { \
    UIntLoop i; \
    for (i = 0; i < amount; i++) \
    { \
        if (pData == &(Chunk##size[i].Data[0])) \
        { \
            GP_LOG_PRINTF("BF-%u[%u]:%u",6,(UInt16)sizeof(Chunk##size[i].Data),(UInt16)i,(UInt16)Chunk##size[i].InUse);\
            GP_ASSERT_SYSTEM(Chunk##size[i].GuardStart == GP_POOLMEM_GUARD\
                && Chunk##size[i].GuardEnd == GP_POOLMEM_GUARD && Chunk##size[i].InUse != GP_POOLMEM_NOTUSED);\
            Chunk##size[i].InUse = GP_POOLMEM_NOTUSED; \
            gpWmrk_CntrFromLimit(gpPoolMemWmrk##size); \
            return; \
        } \
     }\
    } while(false)

#define POOLMEM_FREE_BY_COMPID(size,amount, comp_id) do { \
    UIntLoop i; \
    for (i = 0; i < amount; i++) \
    { \
        if (comp_id == Chunk##size[i].InUse) \
        { \
            GP_LOG_PRINTF("BF-%u[%u]:CId:%u %u",6,(UInt16)sizeof(Chunk##size[i].Data),(UInt16)i, comp_id, (UInt16)Chunk##size[i].InUse);\
            GP_ASSERT_SYSTEM(Chunk##size[i].GuardStart == GP_POOLMEM_GUARD\
                && Chunk##size[i].GuardEnd == GP_POOLMEM_GUARD );\
            Chunk##size[i].InUse = GP_POOLMEM_NOTUSED; \
            gpWmrk_CntrFromLimit(gpPoolMemWmrk##size); \
        } \
     }\
    } while(false)

#define POOLMEM_IN_USE(size,amount,amountInUse) do { \
    UIntLoop i; \
    for (i = 0; i < amount; i++) \
    { \
        if (GP_POOLMEM_NOTUSED != Chunk##size[i].InUse) \
        { \
            GP_ASSERT_SYSTEM(Chunk##size[i].GuardStart == GP_POOLMEM_GUARD\
                && Chunk##size[i].GuardEnd == GP_POOLMEM_GUARD );\
            amountInUse++; \
        } \
     }\
    } while(false)
/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#if defined GP_POOLMEM_CHUNK_AMOUNT1 == 0
#error Minimum need 1 buffer to play with
#endif

//Data pools
extern gpPoolMem_Chunks1_t Chunks1[];
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
extern gpPoolMem_Chunks2_t Chunks2[];
#endif
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
extern gpPoolMem_Chunks3_t Chunks3[];
#endif


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/* Init of chunks inside memory */
 void PoolMem_Init(void)
 {
    GP_LOG_PRINTF("Init PoolMem",0);
    POOLMEM_INIT(s1,GP_POOLMEM_CHUNK_AMOUNT1);
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    POOLMEM_INIT(s2,GP_POOLMEM_CHUNK_AMOUNT2);
#endif // GP_POOLMEM_CHUNK_AMOUNT2 > 0
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    POOLMEM_INIT(s3,GP_POOLMEM_CHUNK_AMOUNT3);
#endif
 }

void gpPoolMem_Reset(void)
 {
    GP_LOG_PRINTF("Reset PoolMem",0);
    POOLMEM_INIT(s1,GP_POOLMEM_CHUNK_AMOUNT1);

#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    POOLMEM_INIT(s2,GP_POOLMEM_CHUNK_AMOUNT2);
#endif // GP_POOLMEM_CHUNK_AMOUNT2 > 0

#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    POOLMEM_INIT(s3,GP_POOLMEM_CHUNK_AMOUNT3);
#endif // GP_POOLMEM_CHUNK_AMOUNT3 > 0
 }

/* Implementation Malloc function */
void *PoolMem_Malloc (UInt8 comp_id, UInt32 nbytes, Bool try_)
{
    /* Look for free chunk
    Check GuardStart before checking InUse flag */
    if ((GP_POOLMEM_CHUNK_SIZE1 < GP_POOLMEM_PD_SIZE) && (GP_POOLMEM_CHUNK_AMOUNT2 == 0) && (GP_POOLMEM_CHUNK_AMOUNT3 == 0))
    {
        if (nbytes <= GP_POOLMEM_CHUNK_SIZE1) POOLMEM_MALLOC(s1,GP_POOLMEM_CHUNK_AMOUNT1,nbytes,comp_id);
    }
    else if((GP_POOLMEM_CHUNK_SIZE1 > GP_POOLMEM_PD_SIZE) && (GP_POOLMEM_CHUNK_AMOUNT2 == 0) && (GP_POOLMEM_CHUNK_AMOUNT3 == 0))
    {
        if (nbytes <= GP_POOLMEM_CHUNK_SIZE1) POOLMEM_MALLOC(s1,GP_POOLMEM_CHUNK_AMOUNT1,nbytes,comp_id);
    }
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    else if((GP_POOLMEM_CHUNK_AMOUNT2 > 0) && (GP_POOLMEM_CHUNK_AMOUNT3 == 0))
    {
        if (nbytes <= GP_POOLMEM_CHUNK_SIZE1) POOLMEM_MALLOC(s1,GP_POOLMEM_CHUNK_AMOUNT1,nbytes,comp_id);

        if (GP_POOLMEM_PD_SIZE < GP_POOLMEM_CHUNK_AMOUNT2)
        {
            if (nbytes <= GP_POOLMEM_CHUNK_SIZE2) POOLMEM_MALLOC(s2,GP_POOLMEM_CHUNK_AMOUNT2,nbytes,comp_id);
        }
        else
        {
            if (nbytes <= GP_POOLMEM_CHUNK_SIZE2) POOLMEM_MALLOC(s2,GP_POOLMEM_CHUNK_AMOUNT2,nbytes,comp_id);
        }
    }
#endif // GP_POOLMEM_CHUNK_AMOUNT2 > 0
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    else // (GP_POOLMEM_CHUNK_AMOUNT3 > 0) && (GP_POOLMEM_CHUNK_AMOUNT2 > 0)
    {
        if (nbytes <= GP_POOLMEM_CHUNK_SIZE1) POOLMEM_MALLOC(s1,GP_POOLMEM_CHUNK_AMOUNT1,nbytes,comp_id);
        if (GP_POOLMEM_PD_SIZE < GP_POOLMEM_CHUNK_AMOUNT2)
        {
            if (nbytes <= GP_POOLMEM_CHUNK_SIZE2) POOLMEM_MALLOC(s2,GP_POOLMEM_CHUNK_AMOUNT2,nbytes,comp_id);
            if (nbytes <= GP_POOLMEM_CHUNK_SIZE3) POOLMEM_MALLOC(s3,GP_POOLMEM_CHUNK_AMOUNT3,nbytes,comp_id);
        }
        else
        {
            if (nbytes <= GP_POOLMEM_CHUNK_SIZE2) POOLMEM_MALLOC(s2,GP_POOLMEM_CHUNK_AMOUNT2,nbytes,comp_id);
            if (GP_POOLMEM_PD_SIZE < GP_POOLMEM_CHUNK_AMOUNT3)
            {
                if (nbytes <= GP_POOLMEM_CHUNK_SIZE3) POOLMEM_MALLOC(s3,GP_POOLMEM_CHUNK_AMOUNT3,nbytes,comp_id);
            }
            else
            {
                if (nbytes <= GP_POOLMEM_CHUNK_SIZE3) POOLMEM_MALLOC(s3,GP_POOLMEM_CHUNK_AMOUNT3,nbytes,comp_id);
            }
        }
    }
#endif // GP_POOLMEM_CHUNK_AMOUNT3 > 0
    //If no buffer available or buffer sizes too small:
    GP_LOG_PRINTF("Chunk size too small: CId:%u - size:%lu",4,(UInt16)comp_id,(UInt32)nbytes);

    if(!try_)
    {
        GP_ASSERT_DEV_EXT(false);
    }
    return NULL;
}

/* Implementation Free function */
void PoolMem_Free (void* pData)
/* Check guards and free chunk, if guard error -> assert */
{
    // Release chunk
    POOLMEM_FREE(s1,GP_POOLMEM_CHUNK_AMOUNT1);
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    POOLMEM_FREE(s2,GP_POOLMEM_CHUNK_AMOUNT2);
#endif
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    POOLMEM_FREE(s3,GP_POOLMEM_CHUNK_AMOUNT3);
#endif
    // If chunk not found, wrong pointer is given:
    GP_ASSERT_DEV_EXT(false);
}

#ifdef GP_COMP_UNIT_TEST

void PoolMem_Free_ByCompId (UInt8 comp_id)
{
    // Release chunk by CompId
    POOLMEM_FREE_BY_COMPID(s1,GP_POOLMEM_CHUNK_AMOUNT1, comp_id);
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    POOLMEM_FREE_BY_COMPID(s2,GP_POOLMEM_CHUNK_AMOUNT2, comp_id);
#endif
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    POOLMEM_FREE_BY_COMPID(s3,GP_POOLMEM_CHUNK_AMOUNT3, comp_id);
#endif
    return;
}

UInt8 PoolMem_InUse (void)
{
    UInt8 amountInUse = 0;

    POOLMEM_IN_USE(s1,GP_POOLMEM_CHUNK_AMOUNT1,amountInUse);
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    POOLMEM_IN_USE(s2,GP_POOLMEM_CHUNK_AMOUNT2,amountInUse);
#endif
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    POOLMEM_IN_USE(s3,GP_POOLMEM_CHUNK_AMOUNT3,amountInUse);
#endif
    return amountInUse;
}
#endif //GP_COMP_UNIT_TEST

void gpPoolMem_Dump(Bool checkConsistency)
{
#if GP_POOLMEM_CHUNK_AMOUNT1 > 0
    POOLMEM_DUMP(s1, GP_POOLMEM_CHUNK_AMOUNT1);
#endif
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    POOLMEM_DUMP(s2, GP_POOLMEM_CHUNK_AMOUNT2);
#endif
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    POOLMEM_DUMP(s3, GP_POOLMEM_CHUNK_AMOUNT3);
#endif

    if(checkConsistency)
    {
#if GP_POOLMEM_CHUNK_AMOUNT1 > 0
        POOLMEM_CHECK(s1, GP_POOLMEM_CHUNK_AMOUNT1);
#endif
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
        POOLMEM_CHECK(s2, GP_POOLMEM_CHUNK_AMOUNT2);
#endif
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
        POOLMEM_CHECK(s3, GP_POOLMEM_CHUNK_AMOUNT3);
#endif
    }
}

UInt32 gpPoolMem_GetMaxAvailableChunkSize(void)
{
    UIntLoop i;
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    for (i = 0; i < GP_POOLMEM_CHUNK_AMOUNT3; i++)
    {
        if (GP_POOLMEM_NOTUSED == Chunks3[i].InUse)
        {
            return GP_POOLMEM_CHUNK_SIZE3;
        }
     }
#endif
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    for (i = 0; i < GP_POOLMEM_CHUNK_AMOUNT2; i++)
    {
        if (GP_POOLMEM_NOTUSED == Chunks2[i].InUse)
        {
            return GP_POOLMEM_CHUNK_SIZE2;
        }
     }
#endif
#if GP_POOLMEM_CHUNK_AMOUNT1 > 0
    for (i = 0; i < GP_POOLMEM_CHUNK_AMOUNT1; i++)
    {
        if (GP_POOLMEM_NOTUSED == Chunks1[i].InUse)
        {
            return GP_POOLMEM_CHUNK_SIZE1;
        }
     }
#endif
    return 0;
}
