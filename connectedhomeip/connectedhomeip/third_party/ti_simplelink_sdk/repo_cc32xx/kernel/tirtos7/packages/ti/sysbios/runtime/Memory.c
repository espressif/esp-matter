/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
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
 *
 */
/*
 *  ======== Memory.c ======== 
 */
 
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/IHeap.h>

#include <string.h>

IHeap_Handle Memory_defaultHeapInstance = NULL;

/*
 *  ======== Memory_alloc ========
 *  If eb has an error already set, we preserve the error if we can.  We
 *  could do better by creating a separate error block for the proxy
 *  allocation, but this would cost more than it's worth; the normal case
 *  would pay a constant time overhead for the rare case of allocation
 *  failing when eb already has an error set.
 */
void * Memory_alloc(IHeap_Handle heap, size_t size, size_t align, Error_Block *eb)
{
    void * block;
    bool prior = Error_check(eb);

    if (heap == NULL) {
        heap = Memory_defaultHeapInstance;
    }
    
    /* if align == 0, use default alignment */
    if (align == 0) {
        align = Memory_getMaxDefaultTypeAlign();
    }

    /* allocate using a non-NULL appropriate heap */
    block = heap->alloc(heap, size, align, eb);

    /* if the allocator returned NULL and either
     *     the error was already set or
     *     it didn't set the error
     */
    if (block == NULL && (prior || !Error_check(eb))) {
        Error_raise(eb, Error_E_memory, heap, size);
    }

    return (block);
}

/*
 *  ======== Memory_calloc ========
 */
void * Memory_calloc(IHeap_Handle heap, size_t size, size_t align, Error_Block *eb)
{
    /* Call valloc with a value of zero */
    return (Memory_valloc(heap, size, align, 0, eb));
}

/*
 *  ======== Memory_getStats ========
 */
void Memory_getStats(IHeap_Handle heap, Memory_Stats *stats)
{
    if (heap == NULL) {
        heap = Memory_defaultHeapInstance;
    }

    heap->getStats(heap, stats);
}

/*
 *  ======== Memory_getMaxDefaultTypeAlign ========
 */
size_t Memory_getMaxDefaultTypeAlign(void)
{
//    return (module->maxDefaultTypeAlign);
    return (sizeof (long long));
}

/*
 *  ======== Memory_free ========
 */
void Memory_free(IHeap_Handle heap, void * block, size_t size)
{
    if (heap == NULL) {
        heap = Memory_defaultHeapInstance;
    }

    heap->free(heap, block, size);
}

/*
 *  ======== Memory_query ======== 
 */
bool Memory_query(IHeap_Handle heap, int qual)
{
    bool flag = false;

    if (heap == NULL) {
        heap = Memory_defaultHeapInstance;
    }
    
    switch (qual) {
        case Memory_Q_BLOCKING:
            flag = heap->isBlocking(heap);
            break;
        default: 
            break;
    }
    
    return (flag);
}

/*
 *  ======== Memory_getDefaultHeap ========
 */
IHeap_Handle Memory_getDefaultHeap(void)
{
    return (Memory_defaultHeapInstance);
}

/*
 *  ======== Memory_setDefaultHeap ========
 */
void Memory_setDefaultHeap(IHeap_Handle heap)
{
    Memory_defaultHeapInstance = heap;
}

/*
 *  ======== Memory_valloc ========
 */
void * Memory_valloc(IHeap_Handle heap, size_t size, size_t align, char value, 
                  Error_Block *eb)
{
    void * block = Memory_alloc(heap, size, align, eb);
    if (block != NULL) {
        memset(block, value, size);
    }
    return (block);
}
