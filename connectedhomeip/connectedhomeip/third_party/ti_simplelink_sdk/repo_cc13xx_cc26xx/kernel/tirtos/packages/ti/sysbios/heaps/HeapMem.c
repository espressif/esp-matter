/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Startup.h>

#include "package/internal/HeapMem.xdc.h"

/*
 *  ======== HeapMem_init ========
 */
Void HeapMem_init()
{
    Int i;
    HeapMem_Object *obj;

    /*
     * HeapMem inherits from IHeap. Is part of runtime.
     * Will be called during first pass.
     * All APIs need to be ready after first pass.
     */

    for (i = 0; i < HeapMem_Object_count(); i++) {
        obj = HeapMem_Object_get(NULL, i);

        /*
         * Some targets don't support the align directive (28x, Linux) and
         * the buffer must be aligned at runtime. The best solution is to
         * simply perform the below math on all targets.
         *
         * On targets that don't support the align directive, the static
         * create code will pad the buffer with enough extra space to align
         * the buffer. That way, the HeapMem size remains valid (there's
         * at least that much space) and does not have to be adjusted here.
         */
        obj->buf = (Char *)xdc_uargToPtr(((Memory_Size)(obj->buf) + (obj->align - 1)) &
                            ~(obj->align - 1));

        /* Place the initial header */
        HeapMem_restore(obj);
    }
}

/*
 *  ======== HeapMem_initPrimary ========
 */
Void HeapMem_initPrimary()
{
    Int i;
    HeapMem_Object *obj;
    Bool initializingPrimaryHeap;

    /*
     * HeapMem inherits from IHeap. Is part of runtime.
     * Will be called during first pass.
     * All APIs need to be ready after first pass.
     */

    for (i = 0; i < HeapMem_Object_count(); i++) {
        initializingPrimaryHeap = FALSE;

        obj = HeapMem_Object_get(NULL, i);

        /*
         * use of the 'primaryHeap' buffer is flagged by obj->buf being
         * statically set to null in instance$static$init().
         */
        if (obj->buf == NULL) {
            obj->buf = HeapMem_primaryHeapBaseAddr;
            initializingPrimaryHeap = TRUE;
        }

        /*
         * Some targets don't support the align directive (28x, Linux) and
         * the buffer must be aligned at runtime. The best solution is to
         * simply perform the below math on all targets.
         *
         * On targets that don't support the align directive, the static
         * create code will pad the buffer with enough extra space to align
         * the buffer. That way, the HeapMem size remains valid (there's
         * at least that much space) and does not have to be adjusted here.
         */
        obj->buf = (Char *)xdc_uargToPtr(((Memory_Size)(obj->buf) + (obj->align - 1)) &
                            ~(obj->align - 1));

        if (initializingPrimaryHeap) {
            /* HeapMem.primaryHeapEndAddress was encoded in the object's head.next field */
            obj->head.size = (UArg)HeapMem_primaryHeapEndAddr - (UArg)obj->buf;
            /* Make sure the size is a multiple of minBlockAlign */
            obj->head.size = (obj->head.size / obj->minBlockAlign) * obj->minBlockAlign;
        }

        /* Place the initial header */
        HeapMem_restore(obj);
    }
}

/*
 *  ======== HeapMem_Instance_init ========
 */
Void HeapMem_Instance_init(HeapMem_Object *obj, const HeapMem_Params *params)
{
    Memory_Size offset;
    SizeT       bufAlign;

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
    if ((offset = (Memory_Size)(obj->buf) & (bufAlign - 1)) != 0) {
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

    /* Place the initial header */
    HeapMem_restore(obj);
}

/*
 *  ======== HeapMem_restore ========
 *  The buffer should have the properly alignment at this
 *  point (either from instance$static$init in HeapMem.xs or
 *  from the above HeapMem_Instance_init).
 */
Void HeapMem_restore(HeapMem_Object *obj)
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
Ptr HeapMem_alloc(HeapMem_Object *obj, SizeT reqSize,
                    SizeT reqAlign, Error_Block *eb)
{
    IArg key;
    Ptr buffer;

    key = Gate_enterModule();

    buffer = HeapMem_allocUnprotected(obj, reqSize, reqAlign);

    Gate_leaveModule(key);

    if (buffer == NULL) {
        Error_raise(eb, HeapMem_E_memory, (IArg)obj, (IArg)reqSize);
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
Ptr HeapMem_allocUnprotected(HeapMem_Object *obj, SizeT reqSize, SizeT reqAlign)
{
    HeapMem_Header *prevHeader, *newHeader, *curHeader;
    Memory_Size curSize;
    Memory_Size adjSize;
    SizeT remainSize; /* free memory after allocated memory      */
    Char *allocAddr;
    SizeT offset;
    SizeT adjAlign;

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

    adjSize = (Memory_Size)reqSize;

    /* Make size requested a multiple of minBlockAlign */
    if ((offset = (adjSize & (obj->minBlockAlign - 1))) != 0) {
        adjSize = adjSize + (obj->minBlockAlign - offset);
    }

    /*
     * If size is very large and adjSize overflows, the result will be
     * smaller than reqSize. In this case, don't try to allocate.
     */
    if (adjSize < (Memory_Size)reqSize) {
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
        offset = (Memory_Size)curHeader & (adjAlign - 1);
        if (offset) {
            offset = adjAlign - offset;
        }

        /* Internal Assert that offset is a multiple of minBlockAlign */
        Assert_isTrue(((offset & (obj->minBlockAlign - 1)) == 0), NULL);

        /* big enough? */
        if (curSize >= (adjSize + offset)) {

            /* Set the pointer that will be returned. Alloc from front */
            allocAddr = (Char *)xdc_uargToPtr((Memory_Size)curHeader + offset);

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
                        xdc_uargToPtr(((Memory_Size)allocAddr + adjSize));
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
                        xdc_uargToPtr(((Memory_Size)allocAddr + adjSize));
                    newHeader->next  = curHeader->next;
                    newHeader->size  = remainSize;
                    prevHeader->next = newHeader;
                }
                else {
                    prevHeader->next = curHeader->next;
                }
            }

            /* Success, return the allocated memory */
            return ((Ptr)allocAddr);
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
Void HeapMem_free(HeapMem_Object *obj, Ptr addr, SizeT size)
{
    IArg key;

    key = Gate_enterModule();

    HeapMem_freeUnprotected(obj, addr, size);

    Gate_leaveModule(key);
}

/*
 *  ======== HeapMem_freeUnprotected ========
 */
Void HeapMem_freeUnprotected(HeapMem_Object *obj, Ptr addr, SizeT size)
{
    SizeT offset;
    HeapMem_Header *curHeader, *newHeader, *nextHeader;

    /* Make sure the addr is aligned properly */
    Assert_isTrue((((UArg)addr & (obj->minBlockAlign - 1)) == 0),
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
    Assert_isTrue((((UArg)newHeader >= (UArg)(obj->buf)) &&
                   ((UArg)newHeader + (size - 1) <=
                    (UArg)(obj->buf) + (obj->head.size - 1))),
                   HeapMem_A_invalidFree);

    /* Go down freelist and find right place for buf */
    while (nextHeader != NULL && nextHeader < newHeader) {
        /* Make sure the addr is not in this free block */
        Assert_isTrue(((UArg)newHeader >= (UArg)nextHeader + nextHeader->size),
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
        Assert_isTrue((((UArg)newHeader + size) <= (UArg)nextHeader),
                      HeapMem_A_invalidFree);

        /* Join with upper block */
        if (((Memory_Size)newHeader + size) == (Memory_Size)nextHeader) {
            newHeader->next = nextHeader->next;
            newHeader->size += nextHeader->size;
        }
    }
    /*
     *  Join with lower block. Make sure to check to see if not the
     *  first block.
     */
    if ((curHeader != &obj->head) &&
        (((Memory_Size)curHeader + curHeader->size) == (Memory_Size)newHeader)) {
        curHeader->next = newHeader->next;
        curHeader->size += newHeader->size;
    }

    return;
}

/*
 *  ======== HeapMem_isBlocking ========
 */
Bool HeapMem_isBlocking(HeapMem_Object *obj)
{
    return (Gate_canBlock());
}

/*
 *  ======== HeapMem_getStats ========
 */
Void HeapMem_getStats(HeapMem_Object *obj, Memory_Stats *stats)
{
    IArg key;
    HeapMem_Header *curHeader;

    stats->totalSize         = obj->head.size;
    stats->totalFreeSize     = 0;                   /* determined later */
    stats->largestFreeSize   = 0;                   /* determined later */

    key = Gate_enterModule();

    curHeader = obj->head.next;

    while (curHeader != NULL) {
        stats->totalFreeSize += curHeader->size;
        if (stats->largestFreeSize < curHeader->size) {
            stats->largestFreeSize = curHeader->size;
        }
        curHeader = curHeader->next;
    }

    Gate_leaveModule(key);

    return;
}

/*
 *  ======== HeapMem_getExtendedStats ========
 */
Void HeapMem_getExtendedStats(HeapMem_Object *obj, HeapMem_ExtendedStats *stats)
{
    stats->buf   = obj->buf;
    stats->size  = obj->head.size;

    return;
}

/*
 *  ======== HeapMem_enter ========
 *  Enter the HeapMem module's gate.
 */
IArg HeapMem_enter()
{
    return (Gate_enterModule());
}

/*
 *  ======== HeapMem_leave ========
 *  Leave the HeapMem module's gate.
 */
Void HeapMem_leave(IArg key)
{
    Gate_leaveModule(key);
}


