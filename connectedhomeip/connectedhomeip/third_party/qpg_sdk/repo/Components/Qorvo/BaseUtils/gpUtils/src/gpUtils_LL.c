/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017, 2019, 2021-2022, Qorvo Inc
 *
 * gpUtils_LL.c
 *
 * This file contains the link list features of the Utils component.
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_UTILS

#include "hal.h"
#include "gpUtils.h"
#include "gpLog.h"
#include "gpAssert.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

void Utils_InsertBound(gpUtils_Link_t* plnk, gpUtils_LinkList_t* plst);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
void gpUtils_LLLockCreate(gpUtils_Links_t* plst)
{
#if !defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2)
    hal_MutexCreate(&plst->common.lock);
#endif
}

void gpUtils_LLLockDestroy(gpUtils_Links_t* plst)
{
#if !defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2)
    hal_MutexDestroy(&plst->common.lock);
#endif
}

void gpUtils_LLLockAcquire(gpUtils_Links_t* plst)
{
#if !defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2)
    hal_MutexAcquire(plst->common.lock);
#else
    hal_MutexAcquire(NULL);
#endif
}

void gpUtils_LLLockRelease(gpUtils_Links_t* plst)
{
#if !defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2)
    hal_MutexRelease(plst->common.lock);
#else
    hal_MutexRelease(NULL);
#endif
}

Bool gpUtils_LLLockIsValid(gpUtils_Links_t* plst)
{
#if !defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2)
    return hal_MutexIsValid(plst->common.lock);
#else
    return hal_MutexIsValid(NULL);
#endif
}

Bool gpUtils_LLLockIsAcquired(gpUtils_Links_t* plst)
{
#if !defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2)
    return hal_MutexIsAcquired(plst->common.lock);
#else
    return hal_MutexIsAcquired(NULL);
#endif
}

void Utils_InsertBound(gpUtils_Link_t* plnk, gpUtils_LinkList_t* plst)
{
    // Insert the element that has its insertion point filled in
    // To be executed atomically before specifying the insertion point!

    // If there is a next element, point it to me
    // or adjust the last-pointer
    if(plnk->plnk_nxt)
    {
        plnk->plnk_nxt->plnk_prv = plnk;
    }
    else
    {
        plst->plnk_last = plnk;
    }
    // If there is a previous element, point it to me
    // or adjust the first-pointer
    if(plnk->plnk_prv)
    {
        plnk->plnk_prv->plnk_nxt = plnk;
    }
    else
    {
        plst->plnk_first = plnk;
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpUtils_LLInit(void* buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_LinkFree_t* pfre)
{
    void* plnk_org = buf; // Remember start of the list
    UInt32 i;
    gpUtils_Link_t* plnk = (gpUtils_Link_t*)buf;

    // Init linked list
    // Point the next-pointer of every cell to the link struct of the next cell
    // To initialize the private contents of every cell itself is up to the user
    // Every cell may have a payload or a pointer to a payload embedded
    for(i = 0; i < (n_nr_of_elements - 1); i++)
    {
        plnk->plnk_nxt = (gpUtils_Link_t*)((UIntPtr)plnk + (UInt16)n_size_cell /*+ (UInt16)sizeof(gpUtils_Link_t)*/);
        plnk = plnk->plnk_nxt;
    }
    plnk->plnk_nxt = NULL;
    // Assign the free-pointers
    // Hook the last-pointer to the last cell to be able to have circular usage of the free-list
    // Used cells will be returned to the end of the list
    pfre->plnk_free_last = plnk;
    pfre->plnk_free = plnk_org;
    if(!gpUtils_LLLockIsValid((gpUtils_Links_t*)pfre))
    {
        gpUtils_LLLockCreate((gpUtils_Links_t*)pfre);
    }
    GP_ASSERT_DEV_EXT(gpUtils_LLLockIsValid((gpUtils_Links_t*)pfre));
}

void gpUtils_LLClear(gpUtils_LinkList_t* plst)
{
    HAL_DISABLE_GLOBAL_INT();
    // Clear the list
    plst->plnk_first = plst->plnk_last = NULL;
    if(!gpUtils_LLLockIsValid((gpUtils_Links_t*)plst))
    {
        gpUtils_LLLockCreate((gpUtils_Links_t*)plst);
    }
    GP_ASSERT_DEV_EXT(gpUtils_LLLockIsValid((gpUtils_Links_t*)plst));
    HAL_ENABLE_GLOBAL_INT();
}

void* gpUtils_LLNew(gpUtils_LinkFree_t* pfre)
{
    // Get a fresh element from the free-list
    gpUtils_Link_t* p_cell;

    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(pfre && gpUtils_LLLockIsAcquired((gpUtils_Links_t*)pfre));
    if(pfre)
    {
        // Get element from start of queue
        if(!(p_cell = pfre->plnk_free))
            return NULL; // exit if no more element available

        // Move the free-pointer
        pfre->plnk_free = pfre->plnk_free->plnk_nxt;

        // This might have been the last element in the list
        if(!pfre->plnk_free)
        {
            pfre->plnk_free_last = pfre->plnk_free;
        }
    }
    else
    {
        p_cell = NULL;
    }

    if(p_cell)
    {
        return GP_UTILS_LL_GET_ELEM(p_cell);
    }
    else
    {
        return NULL;
    }
}

void gpUtils_LLAdd(void* pelem, gpUtils_LinkList_t* plst)
{
    gpUtils_Link_t* plnk;
    // Add the element to the end of the list
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(plst && gpUtils_LLLockIsAcquired((gpUtils_Links_t*)plst));
    GP_ASSERT_DEV_EXT(pelem);
    plnk = GP_UTILS_LL_GET_LINK(pelem);
    // Set the link pointers
    plnk->plnk_prv = plst->plnk_last;
    plnk->plnk_nxt = 0;
    // Insert the element
    Utils_InsertBound(plnk, plst);
}

void gpUtils_LLInsertBefore(void* pelem, void* pelem_cur, gpUtils_LinkList_t* plst)
{
    gpUtils_Link_t* plnk;
    gpUtils_Link_t* plnk_cur;
    // Insert the element after the current position
    // UNTESTED
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(plst && gpUtils_LLLockIsAcquired((gpUtils_Links_t*)plst));

    plnk = GP_UTILS_LL_GET_LINK(pelem);
    plnk_cur = GP_UTILS_LL_GET_LINK(pelem_cur);

    // Set the link pointers
    plnk->plnk_prv = plnk_cur->plnk_prv;
    plnk->plnk_nxt = plnk_cur;

    // Insert the element
    Utils_InsertBound(plnk, plst);
}

void gpUtils_LLUnlink(void* pelem, gpUtils_LinkList_t* plst)
{
    gpUtils_Link_t* plnk;
    // Remove the element from the list
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(plst && gpUtils_LLLockIsAcquired((gpUtils_Links_t*)plst));

    if(!pelem)
    {
        GP_ASSERT_DEV_EXT(false);
    }

    plnk = GP_UTILS_LL_GET_LINK(pelem);
    // If there is a previous element, point it to my next
    // or adjust the first-pointer
    if(plnk->plnk_prv)
    {
        plnk->plnk_prv->plnk_nxt = plnk->plnk_nxt;
    }
    else
    {
        plst->plnk_first = plnk->plnk_nxt;
    }
    // If there is a next element, point it to my previous
    // or adjust the last-pointer
    if(plnk->plnk_nxt)
    {
        plnk->plnk_nxt->plnk_prv = plnk->plnk_prv;
    }
    else
    {
        plst->plnk_last = plnk->plnk_prv;
    }
}

void gpUtils_LLFree(void* pelem, gpUtils_LinkFree_t* pfre)
{
    gpUtils_Link_t* plnk;
    // Return the unlinked element to end of free-queue

    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(pfre && gpUtils_LLLockIsAcquired((gpUtils_Links_t*)pfre));

    if(!pelem)
    {
        GP_ASSERT_DEV_EXT(false);
    }

    plnk = GP_UTILS_LL_GET_LINK(pelem);
    // Link this element behind the last
    if(pfre->plnk_free_last)
    {
        pfre->plnk_free_last->plnk_nxt = plnk;
    }
    // And move the last-pointer
    pfre->plnk_free_last = plnk;
    // This is the last element
    plnk->plnk_nxt = 0;
    // This might be the first element in the list
    if(!pfre->plnk_free)
    {
        pfre->plnk_free = plnk;
    }
}

void* gpUtils_LLGetFirstElem(gpUtils_LinkList_t* plst)
{
    void* pelem = (void*)((UInt8*)plst->plnk_first + sizeof(gpUtils_Link_t));
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(plst && gpUtils_LLLockIsAcquired((gpUtils_Links_t*)plst));
    return plst->plnk_first ? pelem : NULL;
}

void gpUtils_LLDeInit(gpUtils_LinkList_t* plst)
{
    if(gpUtils_LLLockIsValid((gpUtils_Links_t*)plst))
    {
        gpUtils_LLLockDestroy((gpUtils_Links_t*)plst);
    }
}

void gpUtils_LLDeInitFree(gpUtils_LinkFree_t* pfre)
{
    if(gpUtils_LLLockIsValid((gpUtils_Links_t*)pfre))
    {
        gpUtils_LLLockDestroy((gpUtils_Links_t*)pfre);
    }
}
