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
 *  ======== HeapBuf.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/heaps/HeapBuf.h>

#include <ti/sysbios/knl/Queue.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/IHeap.h>
#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/Types.h>

HeapBuf_Module_State HeapBuf_Module_state = {
    .objQ.next = &HeapBuf_Module_state.objQ,
    .objQ.prev = &HeapBuf_Module_state.objQ
};

static const HeapBuf_Params HeapBuf_Params_default = {
    .align = 0,
    .numBlocks = 0,
    .blockSize = 0,
    .bufSize = 0,
    .buf = NULL
};

const int HeapBuf_trackMaxAllocs = HeapBuf_trackMaxAllocs_D;

void HeapBuf_postInit(HeapBuf_Object *obj);

/*
 *  ======== HeapBuf_Instance_init ========
 *  Initializes a dynamically created HeapBuf.
 *
 *  Asserts:
 *  1. Parameters non-null, non-zero
 *  2. Buffer aligned on requested alignment
 *  3. bufSize is large enough
 *  4. blockSize large enough to contain two data pointers
 *
 *  HeapBuf will not adjust the buffer to satisfy the alignment request,
 *  the buffer must be aligned manually.
 *
 *  Dynamic HeapBuf create will not assert that alignment is a power of 2.
 */
int HeapBuf_Instance_init(HeapBuf_Object *obj, const HeapBuf_Params *params,
                          Error_Block *eb)
{
    size_t align, blockSize;
    Queue_Handle freeList;

    /* Assert that buf is not null and parameters are non-zero */
    Assert_isTrue((params->buf != NULL), HeapBuf_A_nullBuf);
    Assert_isTrue((params->numBlocks != 0), HeapBuf_A_zeroBlocks);
    Assert_isTrue((params->blockSize >= (2 * sizeof(void *))), HeapBuf_A_invalidBlockSize);
    Assert_isTrue((params->bufSize != 0), HeapBuf_A_zeroBufSize);

    /* Use the min alignment, unless an alignment has been requested. */
    if (params->align == 0) {
        align = Memory_getMaxDefaultTypeAlign();
    }
    else {
        align = params->align;
    }

    /* Assert valid align parameter (must be power of 2) */
    Assert_isTrue(((align & (align - 1)) == 0), HeapBuf_A_invalidAlign);

    /* Assert the buffer has a proper alignment */
    Assert_isTrue((((uintptr_t)(params->buf) & (align - 1)) == 0), HeapBuf_A_bufAlign);

    /* Assert align parameter >= Memory_getMaxDefaultTypeAlign */
    Assert_isTrue((align >= Memory_getMaxDefaultTypeAlign()),
            HeapBuf_A_invalidAlign);

    /* Ensure block size is a multiple of align. */
    blockSize = (params->blockSize + (align - 1)) & ~(align - 1);

    /* Assert that bufSize is not too small. */
    Assert_isTrue((params->bufSize >= (params->numBlocks * blockSize)),
            HeapBuf_A_invalidBufSize);

    freeList = &obj->freeList;
    Queue_construct(freeList, NULL);

    /* Fill-in the object's fields */
    obj->buf             = params->buf;
    obj->blockSize       = blockSize;
    obj->align           = align;
    obj->numBlocks       = params->numBlocks;
    obj->numFreeBlocks   = params->numBlocks;
    obj->minFreeBlocks   = (unsigned int)-1;
    obj->bufSize         = blockSize * obj->numBlocks;

    HeapBuf_postInit(obj);

    /* put HeapBuf object on global HeapBuf Object list (Queue_put is atomic) */
    Queue_put(&HeapBuf_module->objQ, &obj->objElem);

    return(0); /* Success */
}

/*
 *  ======== HeapBuf_postInit ========
 *  Slice and dice the buffer up into the correct size blocks and
 *  add to the freelist.
 */
void HeapBuf_postInit(HeapBuf_Object *obj)
{
    unsigned int i;
    int align = obj->align;
    char *buf = obj->buf;
    Queue_Handle freeList;

    freeList = &obj->freeList;

    /*
     * Make sure buffer is aligned properly. This is only necessary for static
     * creates on targets that do not support the align directive (e.g. gcc),
     * but it's easiest just to do the math.
     */
    if (align > 1) {
        buf = (char *)Types_uargToPtr(((size_t)buf + (align - 1)) & ~(align - 1));
    }

    obj->buf = buf;

    /*
     * Split the buffer into blocks that are length "blockSize" and
     * add into the freeList Queue.
     */
    for (i = 0; i < obj->numBlocks; i++) {
        Queue_put(freeList, (Queue_Elem *)buf);
        buf += obj->blockSize;
    }
}

/*
 *  ======== HeapBuf_Instance_finalize ========
 */
void HeapBuf_Instance_finalize(HeapBuf_Object *obj, int status)
{
    Queue_destruct(&obj->freeList);
}

/*
 *  ======== HeapBuf_getBlockSize ========
 */
size_t HeapBuf_getBlockSize(HeapBuf_Object *obj)
{
    return (obj->blockSize);
}

/*
 *  ======== HeapBuf_getAlign ========
 */
size_t HeapBuf_getAlign(HeapBuf_Object *obj)
{
    return (obj->align);
}

/*
 *  ======== HeapBuf_getEndAddr ========
 *  Returns the last address included in the HeapBuf.
 *  This returns the wrong value after any noncontiguous memory
 *  has been added to the HeapBuf.
 */
void * HeapBuf_getEndAddr(HeapBuf_Object *obj)
{
    return (obj->buf + obj->bufSize - 1);
}

/*
 *  ======== HeapBuf_alloc ========
 *  Allocate a block. The align and size parameters are ignored.
 */
void *HeapBuf_alloc(HeapBuf_Object *obj, size_t size, size_t align,
                    Error_Block *eb)
{
    char *block;
    Queue_Handle freeList;
    unsigned int key;

    Assert_isTrue(((align & (align - 1)) == 0), HeapBuf_A_invalidRequestedAlign);
    Assert_isTrue((align <= obj->align), HeapBuf_A_invalidRequestedAlign);

    freeList = &obj->freeList;

    if (size > obj->blockSize) {
        Error_raise(eb, HeapBuf_E_size, (intptr_t)obj, (intptr_t)size);
        return (NULL);
    }

    key = Hwi_disable();
    block = Queue_dequeue(freeList);

    /* Make sure that a valid pointer was returned. */
    if (block == (char *)(freeList)) {
        Hwi_restore(key);
        return (NULL);
    }

    obj->numFreeBlocks--;

    /*
     *  Keep track of the min number of free for this HeapBuf, if user
     *  has set the config variable trackMaxAllocs to true.
     *
     *  The min number of free blocks, 'minFreeBlocks', will be used to compute
     *  the "all time" maximum number of allocated blocks in getExtendedStats().
     */
    if (HeapBuf_trackMaxAllocs) {
        if (obj->numFreeBlocks < obj->minFreeBlocks) {
            /* save the new minimum */
            obj->minFreeBlocks = obj->numFreeBlocks;
        }
    }

    Hwi_restore(key);

    return (block);
}

/*
 *  ======== HeapBuf_free ========
 *  Frees the block to this HeapBuf.
 */
void HeapBuf_free(HeapBuf_Object *obj, void * block, size_t size)
{
    Queue_Handle freeList;
    unsigned int key;

    freeList = &obj->freeList;

    /* Make sure the entire buffer is not NULL. */
    Assert_isTrue(((uintptr_t)block != 0U), HeapBuf_A_invalidFree);

    /* Make sure the entire buffer is in the range of the heap. */
    Assert_isTrue((((uintptr_t)block >= (uintptr_t)(obj->buf)) &&
                   ((uintptr_t)block + size <=
                    (uintptr_t)(obj->buf) + obj->bufSize)),
                   HeapBuf_A_invalidFree);

    /* Make sure the block is on a correct boundary */
    Assert_isTrue((((uintptr_t)block - (uintptr_t)(obj->buf)) % obj->blockSize == 0),
                   HeapBuf_A_invalidFree);

    key = Hwi_disable();

    /* Must have fewer freeblock than numBlocks */
    Assert_isTrue((obj->numFreeBlocks < obj->numBlocks),
            HeapBuf_A_noBlocksToFree);

    obj->numFreeBlocks++;

    Queue_enqueue(freeList, block);

    Hwi_restore(key);
}

/*
 *  ======== HeapBuf_isBlocking ========
 */
bool HeapBuf_isBlocking(HeapBuf_Object *obj)
{
    return (false);
}

/*
 *  ======== HeapBuf_getStats ========
 *  Best way to ensure getStats thread safety is to maintain count of free
 *  blocks across free and alloc (BIOS 5 approach).
 */
void HeapBuf_getStats(HeapBuf_Object *obj, Memory_Stats *stats)
{
    unsigned int key;

    /* Total size is constant */
    stats->totalSize         = obj->blockSize * obj->numBlocks;

    /*
     * Protect this section so that numFreeBlocks doesn't change between
     * totalFreeSize and largestFreeSize
     */
    key = Hwi_disable();

    stats->totalFreeSize     = obj->blockSize * obj->numFreeBlocks;
    stats->largestFreeSize   = (obj->numFreeBlocks > 0) ? obj->blockSize : 0;

    Hwi_restore(key);
}

/*
 *  ======== HeapBuf_getExtendedStats ========
 */
void HeapBuf_getExtendedStats(HeapBuf_Object *obj, HeapBuf_ExtendedStats *stats)
{
    unsigned int key;

    key = Hwi_disable();

    /*
     *  The maximum number of allocations for this HeapBuf (for any given
     *  instance of time during its liftime) is computed as follows:
     *
     *  maxAllocatedBlocks = obj->numBlocks - obj->minFreeBlocks
     *
     *  Note that maxAllocatedBlocks is *not* the maximum allocation count, but
     *  rather the maximum allocations seen at any snapshot of time in the
     *  HeapBuf instance.
     */

    /* if nothing has been alloc'ed yet, return 0 */
    if ((int)(obj->minFreeBlocks) == -1) {
        stats->maxAllocatedBlocks = 0;
    }
    else {
        stats->maxAllocatedBlocks = obj->numBlocks - obj->minFreeBlocks;
    }
    /* current number of alloc'ed blocks is computed using curr # free blocks */
    stats->numAllocatedBlocks = obj->numBlocks - obj->numFreeBlocks;

    Hwi_restore(key);
}

/*
 *  ======== HeapBuf_mergeHeapBufs ========
 *  Internal function used for absorbing the memory managed by heapBuf2 into
 *  heapBuf1. This API is used by HeapMultiBuf for dynamically created
 *  instances to merge buffers with matching block sizes and alignment.
 */
void HeapBuf_mergeHeapBufs(HeapBuf_Handle heapBuf1, HeapBuf_Handle heapBuf2)
{
    Queue_Handle freeList;
    char *buf;
    unsigned int i;

    /* Convert HeapBuf Handles to Objects */
    HeapBuf_Object *heap1 = heapBuf1;
    HeapBuf_Object *heap2 = heapBuf2;

    /* This is an internal function, so these are internal asserts */
    Assert_isTrue((heap1->blockSize == heap2->blockSize), NULL);
    Assert_isTrue((heap1->align == heap2->align), NULL);

    freeList = &(heap1->freeList);

    /* Slice and dice the buffer into blocks that are length
     * "blockSize" and add into the freeList Queue. */
    buf = heap2->buf;
    for (i = 0; i < heap2->numBlocks; i++) {
        Queue_put(freeList, (Queue_Elem *)buf);
        buf += heap2->blockSize;
    }

    heap1->numBlocks += heap2->numBlocks;
    heap1->numFreeBlocks += heap2->numFreeBlocks;
    heap1->bufSize += heap2->bufSize;
}

/*
 *  ======== HeapBuf_init ========
 */
void HeapBuf_init(void)
{
}

/*
 *  ======== HeapBuf_construct ========
 */
HeapBuf_Handle HeapBuf_construct(HeapBuf_Object *obj,
        const HeapBuf_Params *params, Error_Block *eb)
{
    if (params == NULL) {
        params = &HeapBuf_Params_default;
    }

    /*
     * HeapBuf_Instance_init() cannot fail. The 'eb' should not have
     * been part of the HeapBuf_construct() signature, but it's there and
     * it needs to remain for BIOS 6.x compatibility.
     */
    HeapBuf_Instance_init(obj, params, eb);

    obj->iheap.alloc = (IHeap_AllocFxn)HeapBuf_alloc;
    obj->iheap.free = (IHeap_FreeFxn)HeapBuf_free;
    obj->iheap.isBlocking = (IHeap_IsBlockingFxn)HeapBuf_isBlocking;
    obj->iheap.getStats = (IHeap_GetStatsFxn)HeapBuf_getStats;

    return (obj);
}

/*
 *  ======== HeapBuf_create ========
 */
HeapBuf_Handle HeapBuf_create(
        const HeapBuf_Params *params, Error_Block *eb)
{
    HeapBuf_Handle heap;

    heap = Memory_alloc(NULL, sizeof(HeapBuf_Object), 0, eb);

    if (heap != NULL) {
        HeapBuf_construct(heap, params, eb);
    }

    return (heap);
}

/*
 *  ======== HeapBuf_destruct ========
 */
void HeapBuf_destruct(HeapBuf_Object *obj)
{
    unsigned int hwiKey;

    HeapBuf_Instance_finalize(obj, 0);

    /* remove HeapBuf object from global HeapBuf object list (Queue_remove is not atomic) */
    hwiKey = Hwi_disable();
    Queue_remove(&obj->objElem);
    Hwi_restore(hwiKey);
}

/*
 *  ======== HeapBuf_delete ========
 */
void HeapBuf_delete(HeapBuf_Handle *handle)
{
    HeapBuf_destruct(*handle);

    Memory_free(NULL, *handle, sizeof(HeapBuf_Object));

    *handle = NULL;
}

/*
 *  ======== HeapBuf_Params_init ========
 */
void HeapBuf_Params_init(HeapBuf_Params *params)
{
    *params = HeapBuf_Params_default;
}

/*
 *  ======== HeapBuf_getHeapBufFromObjElem ========
 */
HeapBuf_Handle HeapBuf_getHeapBufFromObjElem(Queue_Elem *heapQelem)
{
    if (heapQelem == (Queue_Elem *)&HeapBuf_module->objQ) {
        return (NULL);
    }

    return ((HeapBuf_Handle)((char *)heapQelem -
               offsetof(HeapBuf_Struct, objElem)));
}

/*
 *  ======== HeapBuf_Object_first ========
 */
HeapBuf_Handle HeapBuf_Object_first()
{
    return (HeapBuf_getHeapBufFromObjElem(Queue_head(&(HeapBuf_module->objQ))));
}

/*
 *  ======== HeapBuf_Object_next ========
 */
HeapBuf_Handle HeapBuf_Object_next(HeapBuf_Handle handle)
{
    return (HeapBuf_getHeapBufFromObjElem(Queue_next(&handle->objElem)));
}
