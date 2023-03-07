/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== HeapNull.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/heaps/HeapNull.h>

#include <ti/sysbios/runtime/Memory.h>

/*
 *  ======== HeapNull_alloc ========
 */
void * HeapNull_alloc(HeapNull_Object *obj, size_t reqSize,
                    size_t reqAlign, Error_Block *eb)
{
    return (NULL);
}

/*
 *  ======== HeapNull_free ========
 */
void HeapNull_free(HeapNull_Object *obj, void * addr, size_t size)
{
}

/*
 *  ======== HeapNull_isBlocking ========
 */
bool HeapNull_isBlocking(HeapNull_Object *obj)
{
    return (false);
}

/*
 *  ======== HeapNull_getStats ========
 */
void HeapNull_getStats(HeapNull_Object *obj, Memory_Stats *stats)
{
    stats->totalSize         = 0;
    stats->totalFreeSize     = 0;
    stats->largestFreeSize   = 0;
}

/*
 *  ======== HeapNull_init ========
 */
void HeapNull_init(void)
{
}

/*
 *  ======== HeapNull_construct ========
 */
HeapNull_Handle HeapNull_construct(HeapNull_Object *obj,
        const HeapNull_Params *params)
{
    obj->iheap.alloc = (IHeap_AllocFxn)HeapNull_alloc;
    obj->iheap.free = (IHeap_FreeFxn)HeapNull_free;
    obj->iheap.isBlocking = (IHeap_IsBlockingFxn)HeapNull_isBlocking;
    obj->iheap.getStats = (IHeap_GetStatsFxn)HeapNull_getStats;

    return (obj);
}

/*
 *  ======== HeapNull_create ========
 */
HeapNull_Handle HeapNull_create(
        const HeapNull_Params *params, Error_Block *eb)
{
    HeapNull_Handle heap;

    heap = Memory_alloc(NULL, sizeof(HeapNull_Object), 0, eb);

    if (heap != NULL) {
        HeapNull_construct(heap, params);
    }

    return (heap);
}

/*
 *  ======== HeapNull_destruct ========
 */
void HeapNull_destruct(HeapNull_Object *obj)
{
}

/*
 *  ======== HeapNull_delete ========
 */
void HeapNull_delete(HeapNull_Handle *handle)
{
    Memory_free(NULL, *handle, sizeof(HeapNull_Object));

    *handle = NULL;
}

/*
 *  ======== HeapNull_Params_init ========
 */
void HeapNull_Params_init(HeapNull_Params *params)
{
}
