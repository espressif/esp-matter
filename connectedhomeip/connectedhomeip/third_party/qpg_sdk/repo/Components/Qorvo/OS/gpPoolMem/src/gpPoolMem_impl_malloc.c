
/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
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

/*****************************************************************************
 *                    Include Definitions
 *****************************************************************************/

#include "hal.h"
#include "gpLog.h"
#include "gpAssert.h"
#include "gpWmrk.h"
#include "gpPoolMem.h"
#include "gpPoolMem_defs.h"

#ifdef GP_DIVERSITY_LINUXKERNEL
#include "gpOsal_mem.h"
#else
#include <stdlib.h>    // malloc()
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct gpPoolMemElem_s
{
    struct gpPoolMemElem_s *next;
    struct gpPoolMemElem_s *prev;
    UInt8           comp_id;
    UInt32           nbytes;
    void            *address;
} gpPoolMemElem_t;

typedef struct {
  gpPoolMemElem_t *head;
  gpPoolMemElem_t *tail;
  unsigned int counter;
  HAL_CRITICAL_SECTION_DEF(lock)
} gpPoolMemList_t;

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#ifdef GP_DIVERSITY_LINUXKERNEL
#define GP_IMPL_MALLOC      gpOsal_MemAlloc
#define GP_IMPL_FREE        gpOsal_MemFree
#else
#define GP_IMPL_MALLOC      malloc
#define GP_IMPL_FREE        free
#endif
/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static gpPoolMemList_t* gpPoolMem_AdminList;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

gpPoolMemList_t *ll_init(void)
{
    gpPoolMemList_t *list = GP_IMPL_MALLOC(sizeof(gpPoolMemList_t));

    if(NULL != list)
    {
        list->head = NULL;
        list->tail = NULL;
        list->counter = 0;
        hal_MutexCreate(&list->lock);
        if(!hal_MutexIsValid(list->lock))
        {
            GP_IMPL_FREE(list);
            return NULL;
        }
    }
    return list;
}

void ll_add(gpPoolMemList_t *list, gpPoolMemElem_t *node)
{
    hal_MutexAcquire(list->lock);
    if (list->counter)
    {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }
    else
    {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    }
    list->counter++;
    hal_MutexRelease(list->lock);
}

gpPoolMemElem_t* ll_first (gpPoolMemList_t *list)
{
    // Expected to be used with locked Mutex
    GP_ASSERT_DEV_INT(hal_MutexIsAcquired(list->lock));
    return list->head;
}

gpPoolMemElem_t* ll_next (gpPoolMemList_t *list, gpPoolMemElem_t * node)
{
    // Expected to be used with locked Mutex
    GP_ASSERT_DEV_INT(hal_MutexIsAcquired(list->lock));
    return node->next;
}

gpPoolMemElem_t *ll_find_comp_id(gpPoolMemList_t *list, UInt8 comp_id)
{
    gpPoolMemElem_t *node;

    hal_MutexAcquire(list->lock);
    for( node = ll_first(list); NULL != node; node = ll_next(list, node))
    {
        if (comp_id == node->comp_id)
        {
            break;
        }
    }
    hal_MutexRelease(list->lock);

    return node;
}

gpPoolMemElem_t *ll_find_address(gpPoolMemList_t *list, void *address)
{
    gpPoolMemElem_t *node;

    hal_MutexAcquire(list->lock);
    for( node = ll_first(list); NULL != node; node = ll_next(list, node))
    {
        if (address == node->address)
        {
            break;
        }
    }
    hal_MutexRelease(list->lock);
    return node;
}

void ll_rem (gpPoolMemList_t *list , gpPoolMemElem_t *node)
{
    hal_MutexAcquire(list->lock);
    if (NULL != node->next)
    {
        node->next->prev = node->prev;
    }
    else
    {
        list->tail = node->prev;
    }
    if (NULL != node->prev)
    {
        node->prev->next = node->next;
    }
    else
    {
        list->head = node->next;
    }
    node->prev = node->next = NULL;
    list->counter--;
    hal_MutexRelease(list->lock);
}


void ll_cln (gpPoolMemList_t *list)
{
    gpPoolMemElem_t *node, *next;

    hal_MutexAcquire(list->lock);
    node = list->head;
    list->head = 0;
    while (NULL != node)
    {
        next = node->next;
        node->next = node->prev = NULL;
        GP_IMPL_FREE(node);
        node = next;
    }

    list->head = list->tail = NULL;
    list->counter = 0;
    hal_MutexRelease(list->lock);
}

void ll_rls (gpPoolMemList_t *list)
{
    ll_cln (list);
    if(hal_MutexIsValid(list->lock))
    {
        hal_MutexDestroy(&list->lock);
    }
    GP_IMPL_FREE(list);
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/* Init of chunks inside memory */
void PoolMem_Init(void)
{
    if(NULL == gpPoolMem_AdminList)
    {
        gpPoolMem_AdminList = ll_init();
        if(NULL == gpPoolMem_AdminList)
        {
            GP_ASSERT_DEV_EXT(false);
        }
    }
}

void gpPoolMem_Reset(void)
{
    if(gpPoolMem_AdminList)
    {
        //Free all allocated memory
        ll_rls(gpPoolMem_AdminList);
        gpPoolMem_AdminList = NULL;
    }
}

/* Implementation Malloc function */
void *PoolMem_Malloc ( UInt8 comp_id, UInt32 nbytes, Bool try_)
{
    gpPoolMemElem_t *node;
    GUARD val = GP_POOLMEM_GUARD;

    PoolMem_Init();

    node = GP_IMPL_MALLOC( sizeof( gpPoolMemElem_t ) + nbytes + sizeof(GUARD) );
    if(NULL == node)
    {
        if(!try_)
        {
            GP_LOG_SYSTEM_PRINTF("malloc failed comp:%x %lu",0, comp_id, (unsigned long)nbytes);
            gpPoolMem_Dump(false);
            GP_ASSERT_DEV_EXT(false);
        }
        return NULL;
    }
    GP_LOG_PRINTF("alloc node 0x%p - %lu",0, (void*)node, (unsigned long)nbytes);

    node->nbytes  = nbytes;
    node->comp_id = comp_id;
    node->address = (UInt8*) node + sizeof( gpPoolMemElem_t );

    MEMCPY((void*)((UIntPtr)node->address + nbytes), &val, sizeof(GUARD));
    ll_add(gpPoolMem_AdminList, node);

    GP_LOG_PRINTF("alloc address 0x%p",0, (void*)node->address);
    return (void*)node->address;
}

/* Implementation Free function */
void PoolMem_Free (void* pData)
{
    gpPoolMemElem_t *node;
    GUARD val = GP_POOLMEM_GUARD;

    GP_ASSERT_DEV_EXT(gpPoolMem_AdminList);
    node = ll_find_address(gpPoolMem_AdminList, pData);

    GP_LOG_PRINTF("free address 0x%p",2, pData);

    if(NULL != node)
    {
        GP_LOG_PRINTF("free node 0x%p",0, (void*)node);
        GP_ASSERT_DEV_EXT(0 == memcmp((void*)((UIntPtr)(pData) + node->nbytes), &val, sizeof(GUARD)));

        ll_rem(gpPoolMem_AdminList, node);
        GP_IMPL_FREE(node);
    }
    else
    {
        gpPoolMem_Dump(false);
        GP_ASSERT_DEV_EXT(false);
    }
}

void gpPoolMem_Dump(Bool checkConsistency)
{
#ifdef GP_DIVERSITY_LOG
    UInt32 inUse;
    UInt32 reserved;
    UInt32 maxSize;

#if  \
     defined(GP_DIVERSITY_GPHAL_K8E)
    hal_GetHeapInUse(&inUse, &reserved, &maxSize);
#else
    inUse   = 0x0;
    reserved = 0x0;
    maxSize = 0x0;
#endif //GP_DIVERSITY_GPHAL_K8x

    GP_LOG_SYSTEM_PRINTF("Heap [Used/Res/Max]: %lu/%lu/%lu", 0, (unsigned long)(inUse), (unsigned long)(reserved), (unsigned long)(maxSize));
    if(gpPoolMem_AdminList)
    {
        gpPoolMemElem_t *node;

        GP_LOG_SYSTEM_PRINTF("Num PoolMem alloc:%u", 0, gpPoolMem_AdminList->counter);

        hal_MutexAcquire(gpPoolMem_AdminList->lock);
        for(node = ll_first(gpPoolMem_AdminList); NULL != node; node = ll_next(gpPoolMem_AdminList, node))
        {
            GP_LOG_SYSTEM_PRINTF("addr:%lx - compId:%x - size:%lu ", 0, (unsigned long)(UIntPtr)node->address,
                                                                        node->comp_id,
                                                                        (unsigned long int)node->nbytes);
            gpLog_Flush();
        }
        hal_MutexRelease(gpPoolMem_AdminList->lock);
    }
    gpLog_Flush();

#endif //GP_DIVERSITY_LOG
}

#ifdef GP_COMP_UNIT_TEST

void PoolMem_Free_ByCompId (UInt8 comp_id)
{
    gpPoolMemElem_t *node;

    for(node = ll_find_comp_id(gpPoolMem_AdminList, comp_id); NULL != node; node = ll_find_comp_id(gpPoolMem_AdminList, comp_id))
    {
        GUARD *guard;

        guard = (GUARD*)((UIntPtr)node->address + node->nbytes);

        GP_LOG_PRINTF("Guard 0x%x",0, *guard);
        GP_ASSERT_DEV_EXT(*guard == GP_POOLMEM_GUARD);
        ll_rem(gpPoolMem_AdminList, node);
        GP_IMPL_FREE(node);
    }
}
/*
    returns the number of allocations done by the system
*/
UInt8 PoolMem_InUse(void)
{
    UInt8 inUse;

    if(gpPoolMem_AdminList && hal_MutexIsValid(gpPoolMem_AdminList->lock))
    {
        hal_MutexAcquire(gpPoolMem_AdminList->lock);
        inUse = gpPoolMem_AdminList->counter;
        hal_MutexRelease(gpPoolMem_AdminList->lock);
    }
    else
    {
        inUse = 0;
    }
    return inUse;
}
#endif //GP_COMP_UNIT_TEST

UInt32 gpPoolMem_GetMaxAvailableChunkSize(void)
{
    UInt32 maxSize = 0;
    void* pTest;

    //Take starting point from build-time defines
    maxSize = max(maxSize, (UInt32) GP_POOLMEM_CHUNK_SIZE1);
    maxSize = max(maxSize, (UInt32) GP_POOLMEM_CHUNK_SIZE2);
    maxSize = max(maxSize, (UInt32) GP_POOLMEM_CHUNK_SIZE3);

    //Check overhead and possible fragmentation for actual malloc
    do {
        //Try allocation of maxSize
        pTest = PoolMem_Malloc(GP_COMPONENT_ID, maxSize, true);
        if(pTest != NULL)
        {
            GP_LOG_PRINTF("malloc %u possible", 0, maxSize);
            PoolMem_Free(pTest);
        }
        else
        {
            GP_LOG_PRINTF("malloc %u not possible", 0, maxSize);
            //Divide and retry
            maxSize/=2;
        }
    } while(pTest == NULL && (maxSize != 0));

    return maxSize;
}
