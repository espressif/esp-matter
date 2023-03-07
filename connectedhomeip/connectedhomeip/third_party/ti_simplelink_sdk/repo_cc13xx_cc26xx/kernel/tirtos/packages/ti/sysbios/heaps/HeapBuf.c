/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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

#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Queue.h>

#include "package/internal/HeapBuf.xdc.h"

/*
 *  ======== HeapBuf_Module_startup ========
 *  Calls instanceStartup on all of the statically created HeapBufs to split
 *  their buffers into blocks.
 */
Int HeapBuf_Module_startup( Int phase )
{
    Int i;

    /*
     * HeapBuf inherits from IHeap. Is part of runtime.
     * Will be called during first pass.
     * All APIs need to be ready after first pass.
     */
    /* okay to proceed with initialization */

    for (i = 0; i < HeapBuf_Object_count(); i++) {
        HeapBuf_postInit(HeapBuf_Object_get(NULL, i));
    }

    /* do post init on all statically Constructed Heaps */
    for (i = 0; i < (Int)HeapBuf_numConstructedHeaps; i++) {
        HeapBuf_postInit(HeapBuf_module->constructedHeaps[i]);
    }

    return Startup_DONE;
}

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
Int HeapBuf_Instance_init(HeapBuf_Object *obj, const HeapBuf_Params *params,
                          Error_Block *eb)
{
    SizeT align, blockSize;
    Queue_Handle freeList;

    /* Assert that buf is not null and parameters are non-zero */
    Assert_isTrue((params->buf != NULL), HeapBuf_A_nullBuf);
    Assert_isTrue((params->numBlocks != 0), HeapBuf_A_zeroBlocks);
    Assert_isTrue((params->blockSize >= (2 * sizeof(Ptr))), HeapBuf_A_invalidBlockSize);
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
    Assert_isTrue((((UArg)(params->buf) & (align - 1)) == 0), HeapBuf_A_bufAlign);

    /* Assert align parameter >= Memory_getMaxDefaultTypeAlign */
    Assert_isTrue((align >= Memory_getMaxDefaultTypeAlign()),
            HeapBuf_A_invalidAlign);

    /* Ensure block size is a multiple of align. */
    blockSize = (params->blockSize + (align - 1)) & ~(align - 1);

    /* Assert that bufSize is not too small. */
    Assert_isTrue((params->bufSize >= (params->numBlocks * blockSize)),
            HeapBuf_A_invalidBufSize);

    freeList = HeapBuf_Instance_State_freeList(obj);
    Queue_construct(Queue_struct(freeList), NULL);

    /* Fill-in the object's fields */
    obj->buf             = params->buf;
    obj->blockSize       = blockSize;
    obj->align           = align;
    obj->numBlocks       = params->numBlocks;
    obj->numFreeBlocks   = params->numBlocks;
    obj->minFreeBlocks   = (UInt)-1;
    obj->bufSize         = blockSize * obj->numBlocks;

    HeapBuf_postInit(obj);

    return(0); /* Success */
}

/*
 *  ======== HeapBuf_postInit ========
 *  Slice and dice the buffer up into the correct size blocks and
 *  add to the freelist.
 */
Void HeapBuf_postInit(HeapBuf_Object *obj)
{
    UInt i;
    Int align = obj->align;
    Char *buf = obj->buf;
    Queue_Handle freeList;

    freeList = HeapBuf_Instance_State_freeList(obj);

    /*
     * Make sure buffer is aligned properly. This is only necessary for static
     * creates on targets that do not support the align directive (e.g. gcc),
     * but it's easiest just to do the math.
     */
    if (align > 1) {
        buf = (Char *)xdc_uargToPtr(((Memory_Size) buf + (align - 1)) & ~(align - 1));
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
Void HeapBuf_Instance_finalize(HeapBuf_Object *obj, int status)
{
    Queue_Handle freeList;

    freeList = HeapBuf_Instance_State_freeList(obj);
    Queue_destruct(Queue_struct(freeList));
}

/*
 *  ======== HeapBuf_getBlockSize ========
 */
SizeT HeapBuf_getBlockSize(HeapBuf_Object *obj)
{
    return (obj->blockSize);
}

/*
 *  ======== HeapBuf_getAlign ========
 */
SizeT HeapBuf_getAlign(HeapBuf_Object *obj)
{
    return (obj->align);
}

/*
 *  ======== HeapBuf_getEndAddr ========
 *  Returns the last address included in the HeapBuf.
 *  This returns the wrong value after any noncontiguous memory
 *  has been added to the HeapBuf.
 */
Ptr HeapBuf_getEndAddr(HeapBuf_Object *obj)
{
    return (obj->buf + obj->bufSize - 1);
}


/*
 *  ======== HeapBuf_alloc ========
 *  Allocate a block. The align and size parameters are ignored.
 */
Void *HeapBuf_alloc(HeapBuf_Object *obj, SizeT size, SizeT align,
                    Error_Block *eb)
{
    Char *block;
    Queue_Handle freeList;
    UInt key;

    Assert_isTrue(((align & (align - 1)) == 0), HeapBuf_A_invalidRequestedAlign);
    Assert_isTrue((align <= obj->align), HeapBuf_A_invalidRequestedAlign);

    freeList = HeapBuf_Instance_State_freeList(obj);

    if (size > obj->blockSize) {
        Error_raise(eb, HeapBuf_E_size, (IArg)obj, (IArg)size);
        return (NULL);
    }

    key = Hwi_disable();
    block = Queue_dequeue(freeList);

    /* Make sure that a valid pointer was returned. */
    if (block == (Char *)(freeList)) {
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
Void HeapBuf_free(HeapBuf_Object *obj, Ptr block, SizeT size)
{
    Queue_Handle freeList;
    UInt key;

    freeList = HeapBuf_Instance_State_freeList(obj);

    /* Make sure the entire buffer is not NULL. */
    Assert_isTrue(((UArg)block != 0U), HeapBuf_A_invalidFree);

    /* Make sure the entire buffer is in the range of the heap. */
    Assert_isTrue((((UArg)block >= (UArg)(obj->buf)) &&
                   ((UArg)block + size <=
                    (UArg)(obj->buf) + obj->bufSize)),
                   HeapBuf_A_invalidFree);

    /* Make sure the block is on a correct boundary */
    Assert_isTrue((((UArg)block - (UArg)(obj->buf)) % obj->blockSize == 0),
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
Bool HeapBuf_isBlocking(HeapBuf_Object *obj)
{
    return (FALSE);
}

/*
 *  ======== HeapBuf_getStats ========
 *  Best way to ensure getStats thread safety is to maintain count of free
 *  blocks across free and alloc (BIOS 5 approach).
 */
Void HeapBuf_getStats(HeapBuf_Object *obj, Memory_Stats *stats)
{
    UInt key;

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
Void HeapBuf_getExtendedStats(HeapBuf_Object *obj, HeapBuf_ExtendedStats *stats)
{
    UInt key;

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
    if ((Int)(obj->minFreeBlocks) == -1) {
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
Void HeapBuf_mergeHeapBufs(HeapBuf_Handle heapBuf1, HeapBuf_Handle heapBuf2)
{
    Queue_Handle freeList;
    Char *buf;
    UInt i;

    /* Convert HeapBuf Handles to Objects */
    HeapBuf_Object *heap1 = heapBuf1;
    HeapBuf_Object *heap2 = heapBuf2;

    /* This is an internal function, so these are internal asserts */
    Assert_isTrue((heap1->blockSize == heap2->blockSize), NULL);
    Assert_isTrue((heap1->align == heap2->align), NULL);

    freeList = HeapBuf_Instance_State_freeList(heap1);

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


