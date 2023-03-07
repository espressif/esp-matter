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
 *  ======== HeapMem.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/heaps/HeapMem.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/Types.h>

void * HeapMem_allocUnprotected(HeapMem_Object *obj, size_t reqSize, size_t reqAlign);
void HeapMem_freeUnprotected(HeapMem_Object *obj, void * addr, size_t size);
void HeapMem_restore(HeapMem_Object *obj);

HeapMem_Module_State HeapMem_Module_state = {
    .objQ.next = &HeapMem_Module_state.objQ,
    .objQ.prev = &HeapMem_Module_state.objQ
};

HeapMem_GateStruct HeapMem_gate;

static const HeapMem_Params HeapMem_Params_default = {
    .minBlockAlign = 0,
    .buf = NULL,
    .size = 0
};

/*
 *  ======== HeapMem_Instance_init ========
 */
void HeapMem_Instance_init(HeapMem_Object *obj, const HeapMem_Params *params)
{
    size_t       offset;
    size_t       bufAlign;

    /* Assert that minBlockAlign is a power of 2 */
    Assert_isTrue(((params->minBlockAlign == 0) ||
                  ((params->minBlockAlign & (params->minBlockAlign - 1)) == 0)),
                  HeapMem_A_align);

    if (params->minBlockAlign <= HeapMem_reqAlign) {
        obj->minBlockAlign = HeapMem_reqAlign;
        bufAlign = HeapMem_reqAlign;
    }
    else {
        obj->minBlockAlign = params->minBlockAlign;
        bufAlign = params->minBlockAlign;
    }

    /*
     *  Check minimum required alignment. If the buffer is not aligned,
     *  force the alignment. This reduces the size of the buffer.
     */
    obj->buf = params->buf;
    obj->head.size = params->size;
    if ((offset = (size_t)(obj->buf) & (bufAlign - 1)) != 0) {
        /* Not nicely aligned */
        offset = bufAlign - offset;

        /* Add the offset to make buf aligned */
        obj->buf += offset;

        /* Reduce size by the offset */
        obj->head.size -= offset;
    }

    /* Verify the buffer is large enough */
    Assert_isTrue((obj->head.size >= bufAlign), HeapMem_A_heapSize);

    /* Make sure the size is a multiple of bufAlign */
    obj->head.size = (obj->head.size / bufAlign) * bufAlign;

    /* put HeapMem object on global HeapMem Object list (Queue_put is atomic) */
    Queue_put(&HeapMem_module->objQ, &obj->objElem);

    /* Place the initial header */
    HeapMem_restore(obj);
}

/*
 *  ======== HeapMem_restore ========
 *  The buffer should have the properly alignment at this
 *  point (either from instance$static$init in HeapMem.xs or
 *  from the above HeapMem_Instance_init).
 */
void HeapMem_restore(HeapMem_Object *obj)
{
    HeapMem_Header *begHeader;

    /*
     * Fill in the top of the memory block
     * next: pointer will be NULL (end of the list)
     * size: size of this block
     */
    begHeader = (HeapMem_Header *)(obj->buf);
    begHeader->next = NULL;
    begHeader->size = obj->head.size;

    obj->head.next = begHeader;
}

/*
 *  ======== HeapMem_alloc ========
 */
void * HeapMem_alloc(HeapMem_Object *obj, size_t reqSize,
                    size_t reqAlign, Error_Block *eb)
{
    intptr_t key;
    void * buffer;

    key = HeapMem_gateEnter();

    buffer = HeapMem_allocUnprotected(obj, reqSize, reqAlign);

    HeapMem_gateLeave(key);

    if (buffer == NULL) {
        Error_raise(eb, HeapMem_E_memory, (intptr_t)obj, (intptr_t)reqSize);
    }
    return (buffer);
}

/*
 *  ======== HeapMem_allocUnprotected ========
 *  HeapMem is implemented such that all of the memory and blocks it works
 *  with have an alignment of the minBlockAlign and have a
 *  size which is a multiple of the minBlockAlign. Maintaining this requirement
 *  throughout the implementation ensures that there are never any odd
 *  alignments or odd block sizes to deal with.
 *
 *  Specifically:
 *  The buffer managed by HeapMem:
 *    1. Is aligned on a multiple of minBlockAlign
 *    2. Has an adjusted size that is a multiple of minBlockAlign
 *  All blocks on the freelist:
 *    1. Are aligned on a multiple of minBlockAlign
 *    2. Have a size that is a multiple of minBlockAlign
 *  All allocated blocks:
 *    1. Are aligned on a multiple of minBlockAlign
 *    2. Have a size that is a multiple of minBlockAlign
 */
void * HeapMem_allocUnprotected(HeapMem_Object *obj, size_t reqSize, size_t reqAlign)
{
    HeapMem_Header *prevHeader, *newHeader, *curHeader;
    size_t curSize;
    size_t adjSize;
    size_t remainSize; /* free memory after allocated memory      */
    char *allocAddr;
    size_t offset;
    size_t adjAlign;

    /* Assert that requested align is a power of 2 */
    Assert_isTrue(((reqAlign & (reqAlign - 1)) == 0), HeapMem_A_align);

    /* Assert that requested block size is non-zero */
    Assert_isTrue((reqSize != 0), HeapMem_A_zeroBlock);

    /*
     *  Make sure the alignment is at least as large as the sizeof
     *  minBlockAlign.
     *  Note: adjAlign must be a power of 2 (by function constraint) and
     *  Header size is also a power of 2,
     */
    adjAlign = reqAlign;
    if (adjAlign & (obj->minBlockAlign - 1)) {
        /* adjAlign is less than Header size */
        adjAlign = obj->minBlockAlign;
    }

    adjSize = reqSize;

    /* Make size requested a multiple of minBlockAlign */
    if ((offset = (adjSize & (obj->minBlockAlign - 1))) != 0) {
        adjSize = adjSize + (obj->minBlockAlign - offset);
    }

    /*
     * If size is very large and adjSize overflows, the result will be
     * smaller than reqSize. In this case, don't try to allocate.
     */
    if (adjSize < reqSize) {
        return (NULL);
    }

    /*
     *  The block will be allocated from curHeader. Maintain a pointer to
     *  prevHeader so prevHeader->next can be updated after the alloc.
     */
    prevHeader  = &obj->head;
    curHeader = prevHeader->next;

    /* Loop over the free list. */
    while (curHeader != NULL) {

        curSize = curHeader->size;

        /*
         *  Determine the offset from the beginning to make sure
         *  the alignment request is honored.
         */
        offset = (size_t)curHeader & (adjAlign - 1);
        if (offset) {
            offset = adjAlign - offset;
        }

        /* Internal Assert that offset is a multiple of minBlockAlign */
        Assert_isTrue(((offset & (obj->minBlockAlign - 1)) == 0), NULL);

        /* big enough? */
        if (curSize >= (adjSize + offset)) {

            /* Set the pointer that will be returned. Alloc from front */
            allocAddr = (char *)Types_uargToPtr((size_t)curHeader + offset);

            /*
             *  Determine the remaining memory after the allocated block.
             *  Note: this cannot be negative because of above comparison.
             */
            remainSize = curSize - adjSize - offset;

            /* Internal Assert that remainSize is a multiple of header size */
            Assert_isTrue(((remainSize & (obj->minBlockAlign - 1)) == 0), NULL);

            /*
             *  If there is memory at the beginning (due to alignment
             *  requirements), maintain it in the list.
             *
             *  offset and remainSize must be multiples of
             *  sizeof(HeapMem_Header). Therefore the address of the newHeader
             *  below must be a multiple of the sizeof(HeapMem_Header), thus
             *  maintaining the requirement.
             */
            if (offset) {

                /* Adjust the curHeader size accordingly */
                curHeader->size = offset;

                /*
                 *  If there is remaining memory, add into the free list.
                 *  Note: no need to coalesce and we have HeapMem locked so
                 *        it is safe.
                 */
                if (remainSize) {
                    newHeader = (HeapMem_Header *)
                        Types_uargToPtr(((size_t)allocAddr + adjSize));
                    newHeader->next = curHeader->next;
                    newHeader->size = remainSize;
                    curHeader->next = newHeader;
                }
            }
            else {
                /*
                 *  If there is any remaining, link it in,
                 *  else point to the next free block.
                 *  Note: no need to coalesce and we have HeapMem locked so
                 *        it is safe.
                 */
                if (remainSize) {
                    newHeader = (HeapMem_Header *)
                        Types_uargToPtr(((size_t)allocAddr + adjSize));
                    newHeader->next  = curHeader->next;
                    newHeader->size  = remainSize;
                    prevHeader->next = newHeader;
                }
                else {
                    prevHeader->next = curHeader->next;
                }
            }

            /* Success, return the allocated memory */
            return ((void *)allocAddr);
        }
        else {
            prevHeader = curHeader;
            curHeader = curHeader->next;
        }
    }

    return (NULL);
}

/*
 *  ======== HeapMem_free ========
 */
void HeapMem_free(HeapMem_Object *obj, void * addr, size_t size)
{
    intptr_t key;

    key = HeapMem_gateEnter();

    HeapMem_freeUnprotected(obj, addr, size);

    HeapMem_gateLeave(key);
}

/*
 *  ======== HeapMem_freeUnprotected ========
 */
void HeapMem_freeUnprotected(HeapMem_Object *obj, void * addr, size_t size)
{
    size_t offset;
    HeapMem_Header *curHeader, *newHeader, *nextHeader;

    /* Make sure the addr is aligned properly */
    Assert_isTrue((((uintptr_t)addr & (obj->minBlockAlign - 1)) == 0),
                  HeapMem_A_invalidFree);

    /* Restore size to actual allocated size */
    if ((offset = size & (obj->minBlockAlign - 1)) != 0) {
        size += obj->minBlockAlign - offset;
    }

    curHeader = &obj->head;

    newHeader = (HeapMem_Header *)addr;
    nextHeader = curHeader->next;

    /*
     *  Make sure the entire buffer is in the range of the heap.
     *  Note the obj->head.size never changes.
     *  The "- 1" is to handle the case were the buffer goes up
     *  the 0xFFFFFFFF. Without the "- 1", the buf + size is 0.
     *  This was done to address SDOCM00096968.
     */
    Assert_isTrue((((uintptr_t)newHeader >= (uintptr_t)(obj->buf)) &&
                   ((uintptr_t)newHeader + (size - 1) <=
                    (uintptr_t)(obj->buf) + (obj->head.size - 1))),
                   HeapMem_A_invalidFree);

    /* Go down freelist and find right place for buf */
    while (nextHeader != NULL && nextHeader < newHeader) {
        /* Make sure the addr is not in this free block */
        Assert_isTrue(((uintptr_t)newHeader >= (uintptr_t)nextHeader + nextHeader->size),
                       HeapMem_A_invalidFree);
        curHeader = nextHeader;
        nextHeader = nextHeader->next;
    }

    newHeader->next = nextHeader;
    newHeader->size = size;
    curHeader->next = newHeader;

    /* Join contiguous free blocks */
    if (nextHeader != NULL) {
        /*
         *  Verify the free size is not overlapping. Not all cases are
         *  detectable, but it is worth a shot. Note: only do this
         *  assert if nextHeader is non-NULL.
         */
        Assert_isTrue((((uintptr_t)newHeader + size) <= (uintptr_t)nextHeader),
                      HeapMem_A_invalidFree);

        /* Join with upper block */
        if (((size_t)newHeader + size) == (size_t)nextHeader) {
            newHeader->next = nextHeader->next;
            newHeader->size += nextHeader->size;
        }
    }
    /*
     *  Join with lower block. Make sure to check to see if not the
     *  first block.
     */
    if ((curHeader != &obj->head) &&
        (((size_t)curHeader + curHeader->size) == (size_t)newHeader)) {
        curHeader->next = newHeader->next;
        curHeader->size += newHeader->size;
    }

    return;
}

/*
 *  ======== HeapMem_isBlocking ========
 */
bool HeapMem_isBlocking(HeapMem_Object *obj)
{
    return (HeapMem_gateCanBlock());
}

/*
 *  ======== HeapMem_getStats ========
 */
void HeapMem_getStats(HeapMem_Object *obj, Memory_Stats *stats)
{
    intptr_t key;
    HeapMem_Header *curHeader;

    stats->totalSize         = obj->head.size;
    stats->totalFreeSize     = 0;                   /* determined later */
    stats->largestFreeSize   = 0;                   /* determined later */

    key = HeapMem_gateEnter();

    curHeader = obj->head.next;

    while (curHeader != NULL) {
        stats->totalFreeSize += curHeader->size;
        if (stats->largestFreeSize < curHeader->size) {
            stats->largestFreeSize = curHeader->size;
        }
        curHeader = curHeader->next;
    }

    HeapMem_gateLeave(key);

    return;
}

/*
 *  ======== HeapMem_getExtendedStats ========
 */
void HeapMem_getExtendedStats(HeapMem_Object *obj, HeapMem_ExtendedStats *stats)
{
    stats->buf   = obj->buf;
    stats->size  = obj->head.size;

    return;
}

/*
 *  ======== HeapMem_init ========
 */
void HeapMem_init(void)
{
    HeapMem_gateConstruct(NULL);
}

/*
 *  ======== HeapMem_construct ========
 */
HeapMem_Handle HeapMem_construct(HeapMem_Object *obj,
        const HeapMem_Params *params)
{
    if (params == NULL) {
        params = &HeapMem_Params_default;
    }

    HeapMem_Instance_init(obj, params);

    obj->iheap.alloc = (IHeap_AllocFxn)HeapMem_alloc;
    obj->iheap.free = (IHeap_FreeFxn)HeapMem_free;
    obj->iheap.isBlocking = (IHeap_IsBlockingFxn)HeapMem_isBlocking;
    obj->iheap.getStats = (IHeap_GetStatsFxn)HeapMem_getStats;

    return (obj);
}

/*
 *  ======== HeapMem_create ========
 */
HeapMem_Handle HeapMem_create(
        const HeapMem_Params *params, Error_Block *eb)
{
    HeapMem_Handle heap;

    heap = Memory_alloc(NULL, sizeof(HeapMem_Object), 0, eb);

    if (heap != NULL) {
        HeapMem_construct(heap, params);
    }

    return (heap);
}

/*
 *  ======== HeapMem_destruct ========
 */
void HeapMem_destruct(HeapMem_Object *obj)
{
    unsigned int hwiKey;

    /* remove HeapMem object from global HeapMem object list (Queue_remove is not atomic) */
    hwiKey = Hwi_disable();
    Queue_remove(&obj->objElem);
    Hwi_restore(hwiKey);
}

/*
 *  ======== HeapMem_delete ========
 */
void HeapMem_delete(HeapMem_Handle *handle)
{
    HeapMem_destruct(*handle);

    Memory_free(NULL, *handle, sizeof(HeapMem_Object));

    *handle = NULL;
}

/*
 *  ======== HeapMem_Params_init ========
 */
void HeapMem_Params_init(HeapMem_Params *params)
{
    *params = HeapMem_Params_default;
}

/*
 *  ======== HeapMem_getHeapMemFromObjElem ========
 */
HeapMem_Handle HeapMem_getHeapMemFromObjElem(Queue_Elem *heapQelem)
{
    if (heapQelem == (Queue_Elem *)&HeapMem_module->objQ) {
        return (NULL);
    }

    return ((HeapMem_Handle)((char *)heapQelem -
               offsetof(HeapMem_Struct, objElem)));
}

/*
 *  ======== HeapMem_Object_first ========
 */
HeapMem_Handle HeapMem_Object_first()
{
    return (HeapMem_getHeapMemFromObjElem(Queue_head(&(HeapMem_module->objQ))));
}

/*
 *  ======== HeapMem_Object_next ========
 */
HeapMem_Handle HeapMem_Object_next(HeapMem_Handle handle)
{
    return (HeapMem_getHeapMemFromObjElem(Queue_next(&handle->objElem)));
}
