/*
 * Copyright (c) 2008-2013, 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpUtils_LLext.c
 *
 * This file contains the less used functions of the Linked Lists
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

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

extern void Utils_InsertBound(gpUtils_Link_t * plnk, gpUtils_LinkList_t * plst);

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void
gpUtils_LLInsert (void * pelem, void * pelem_cur, gpUtils_LinkList_t * plst) {
    gpUtils_Link_t* plnk;
    gpUtils_Link_t* plnk_cur;
    // Insert the element at the current position
    // UNTESTED
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    //HAL_DISABLE_GLOBAL_INT();
    plnk = GP_UTILS_LL_GET_LINK(pelem);
    plnk_cur = GP_UTILS_LL_GET_LINK(pelem_cur);
    // Set the link pointers
    plnk->plnk_prv = plnk_cur;
    plnk->plnk_nxt = plnk_cur->plnk_nxt;
    // Insert the element
    Utils_InsertBound (plnk, plst);
    //HAL_ENABLE_GLOBAL_INT();
}

void
gpUtils_LLInsertSorted        (void * pelem, gpUtils_LinkList_t * plst, gpUtils_ElemCompare_t callback)
{
    gpUtils_Link_t* plnk;
    void* pelemFound = NULL;

    GP_ASSERT_DEV_EXT(pelem);

    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    //HAL_DISABLE_GLOBAL_INT();

    // Search position in the sorted list
    for (plnk = plst->plnk_first; plnk; plnk = plnk->plnk_nxt)
    {
        pelemFound = GP_UTILS_LL_GET_ELEM(plnk);
        if (callback(pelem,pelemFound))
            break;
    }
    if (plnk)
    {
        gpUtils_LLInsertBefore(pelem, pelemFound, plst);
    }
    else
    {
        gpUtils_LLAdd(pelem,plst);
    }
}

void*
gpUtils_LLNewSorted (void * pelem, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre, gpUtils_ElemCompareDelete_t callback)
{
    void* preturn = gpUtils_LLNew(pfre);
    Bool betterElemThanLast = true;
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    //HAL_DISABLE_GLOBAL_INT();

    // If last element is better than last, element can always be added (it can alays replace the last element)
    // If it is not better, then it can only be added if a free element exists in the free list
    if (!preturn)
    {
        gpUtils_Link_t* plnk;
        void* pelem_last;
        Bool canBeDeleted;
        for (plnk = plst->plnk_last; plnk; plnk = plnk->plnk_prv)
        {
            pelem_last = GP_UTILS_LL_GET_ELEM(plnk);

            betterElemThanLast = callback(pelem,pelem_last,&canBeDeleted);
            if (betterElemThanLast)
            {
                if (canBeDeleted)
                {
                    // return last element
                    preturn = pelem_last;
                    gpUtils_LLUnlink(preturn, plst);
                    break;
                }
            }
            else
                break;
        }
    }
    return preturn;
}

void
gpUtils_LLRelease (void * pelem, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre) {
    // Remove the element from the list and return to the free-list
    gpUtils_LLUnlink (pelem, plst);
    gpUtils_LLFree (pelem, pfre);
}

void  gpUtils_LLDump     (gpUtils_LinkList_t * plst)
{
    gpUtils_Link_t* plnk;
    HAL_DISABLE_GLOBAL_INT();
    GP_LOG_PRINTF("Dump LL:",0);
    GP_LOG_PRINTF("First:%p, Last:%p",4,plst->plnk_first,plst->plnk_last);
    for (plnk = plst->plnk_first;plnk;plnk = plnk->plnk_nxt)
    {
        GP_LOG_PRINTF("this:%p/%p,prev:%p,nxt:%p,data:%i",10,plnk,GP_UTILS_LL_GET_ELEM(plnk),plnk->plnk_prv,plnk->plnk_nxt,*(Char*)(GP_UTILS_LL_GET_ELEM(plnk)));
    }
    GP_LOG_PRINTF("End Dump LL:",0);
    HAL_ENABLE_GLOBAL_INT();
}

void  gpUtils_LLDumpFree (gpUtils_LinkFree_t * pfree)
{
    UInt8 Counter = 0;
    gpUtils_Link_t* plnk;
    HAL_DISABLE_GLOBAL_INT();
    GP_LOG_PRINTF("DumpFree LL:",0);
    GP_LOG_PRINTF("First:%p, Last:%p",4,pfree->plnk_free,pfree->plnk_free_last);
    for (plnk = pfree->plnk_free;plnk && Counter<10;plnk = plnk->plnk_nxt)
    {
        GP_LOG_PRINTF("this:%p/%p,prev:%p,nxt:%p,data:%i",10,plnk,GP_UTILS_LL_GET_ELEM(plnk),plnk->plnk_prv,plnk->plnk_nxt,*(Char*)(GP_UTILS_LL_GET_ELEM(plnk)));
        Counter++;
    }
    GP_LOG_PRINTF("End Dump LL:",0);
    HAL_ENABLE_GLOBAL_INT();
}

void
gpUtils_LLDumpMemory (void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre ) {
    UInt32 i;
    gpUtils_Link_t* plnk = (gpUtils_Link_t*)buf;

    NOT_USED(pfre);
    NOT_USED(plst);

    GP_LOG_PRINTF("Lf%p Ll%p Ff%p Fl%p",8,plst->plnk_first,plst->plnk_last,pfre->plnk_free,pfre->plnk_free_last);
    for (i = 0; i < n_nr_of_elements; i++) {
        GP_LOG_PRINTF("@%p <%p %p>",6,plnk,plnk->plnk_prv, plnk->plnk_nxt);
        //GP_LOG_PRINTF("d:%x %x %x %x %x",10, plnk->plnk_nxt[1], plnk->plnk_nxt[2], plnk->plnk_nxt[3], plnk->plnk_nxt[4], plnk->plnk_nxt[5]);
        plnk = (gpUtils_Link_t *) ((UIntPtr) plnk + (UInt16)n_size_cell);
    }
}

Bool gpUtils_LLCheckConsistency(void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre)
{
    UInt32 ElementCounter = 0;
    gpUtils_Link_t* plnk;
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    if (plst)
    {
        for (plnk = plst->plnk_first; plnk; plnk = plnk->plnk_nxt)
        {
            ElementCounter++;
            if (ElementCounter > n_nr_of_elements)
            {
                GP_LOG_PRINTF("Err1 %"PRIu32" %"PRIu32,6, ElementCounter, n_nr_of_elements);
                gpUtils_LLDumpMemory(buf, n_size_cell, n_nr_of_elements, plst, pfre);
                return false;
            }
        }
    }
    if (pfre)
    {
        ElementCounter = 0;
        for (plnk = pfre->plnk_free; plnk; plnk = plnk->plnk_nxt)
        {
            ElementCounter++;
            if (ElementCounter > n_nr_of_elements)
            {
                GP_LOG_PRINTF("Err2 %"PRIu32" %"PRIu32,6, (long unsigned int)ElementCounter, (long unsigned int)n_nr_of_elements);
                gpUtils_LLDumpMemory(buf, n_size_cell, n_nr_of_elements, plst, pfre);
                return false;
            }
        }
    }
    return true;
}

void* gpUtils_LLGetNext(void* pelem)
{
    gpUtils_Link_t* plnk = (gpUtils_Link_t*)((UIntPtr)pelem - sizeof(gpUtils_Link_t));
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());

    return plnk->plnk_nxt?GP_UTILS_LL_GET_ELEM(plnk->plnk_nxt):NULL;
}

void* gpUtils_LLGetPrev(void* pelem)
{
    gpUtils_Link_t* plnk = (gpUtils_Link_t*)((UIntPtr)pelem - sizeof(gpUtils_Link_t));
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());

    return plnk->plnk_prv?GP_UTILS_LL_GET_ELEM(plnk->plnk_prv):NULL;
}

gpUtils_Link_t* gpUtils_LLGetLink(void* pelem)
{
    gpUtils_Link_t* plnk = (gpUtils_Link_t*)((UIntPtr)pelem - sizeof(gpUtils_Link_t));
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    return pelem?plnk:NULL;
}

void* gpUtils_LLGetElem(gpUtils_Link_t* plnk)
{
    void* pelem = (void*)((UIntPtr)plnk + sizeof(gpUtils_Link_t));
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    return plnk?pelem:NULL;
}

void* gpUtils_LLGetLastElem(gpUtils_LinkList_t* plst)
{
    void* pelem = (void*)((UIntPtr)plst->plnk_last + sizeof(gpUtils_Link_t));
    // Check if AtomicOn
    GP_ASSERT_DEV_EXT(!HAL_GLOBAL_INT_ENABLED());
    return plst->plnk_last?pelem:NULL;
}



