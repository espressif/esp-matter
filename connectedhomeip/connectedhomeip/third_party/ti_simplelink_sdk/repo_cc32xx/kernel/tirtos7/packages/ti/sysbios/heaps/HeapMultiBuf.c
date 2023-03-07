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
 *  ======== HeapMultiBuf.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <stdlib.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/IHeap.h>
#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/System.h>
#include <ti/sysbios/runtime/Startup.h>

#include <ti/sysbios/heaps/HeapBuf.h>
#include <ti/sysbios/heaps/HeapMultiBuf.h>

int HeapMultiBuf_addrPairCompare(const void *a, const void *b);
int HeapMultiBuf_sizeAlignCompare(const void *a, const void *b);
void *HeapMultiBuf_borrowBlock(HeapMultiBuf_Object *obj, size_t size,
                                      size_t align, int startIndex);

HeapMultiBuf_Module_State HeapMultiBuf_Module_state = {
    .objQ.next = &HeapMultiBuf_Module_state.objQ,
    .objQ.prev = &HeapMultiBuf_Module_state.objQ
};

static const HeapMultiBuf_Params HeapMultiBuf_Params_default = {
    .blockBorrow = false,
    .numBufs = 0,
    .bufParams = NULL
};

/*
 *  ======== HeapMultiBuf_Instance_init ========
 *  Initializes a dynamically created HeapMultiBuf. Dynamic initialization
 *  requires different steps than static initialization because the buffers
 *  are provided by the user. With static allocation, buffers with matching
 *  properties can be merged before allocation. For dynamic creation, the
 *  buffers have already been allocated, so to merge them we must create a
 *  heapBuf from one buffer then add the other buffer to it.
 */
int HeapMultiBuf_Instance_init(HeapMultiBuf_Object *obj,
                                const HeapMultiBuf_Params *params,
                                Error_Block *eb)
{
    HeapBuf_Handle heapBuf;
    int i;
    HeapMultiBuf_AddrPair addrPair;

    obj->numBufs = params->numBufs;
    obj->numHeapBufs = params->numBufs;
    obj->blockBorrow = params->blockBorrow;

    /*
     * The bufsByAddr array stores the pairing between the ending address of
     * a buffer and the HeapBuf that manages it, so there is one entry per
     * provided buffer.
     */
    obj->bufsByAddr =
        Memory_alloc(NULL, params->numBufs * sizeof(HeapMultiBuf_AddrPair),
                     0, eb);
    if (obj->bufsByAddr == NULL) {
        return (1); /* Failed at 1 */
    }

    /*
     * To simplify initialization, bufsBySize is allocated to the largest
     * potential size, meaning one entry per buffer. If any of the buffers
     * are merged, there will be wasted spots in the bufsBySize array, but
     * this greatly simplifies the initialization.
     *
     * This array is calloc'd so that if one of the HeapBuf creates fails,
     * we can know how many HeapBufs to delete in finalize by checking for
     * NULL.
     */
    obj->bufsBySize =
        Memory_calloc(NULL, obj->numBufs * sizeof(HeapBuf_Object*), 0, eb);
    if (obj->bufsBySize == NULL) {
        return (2); /* Failed at 2 */
    }

    /* Create all of the HeapBufs */
    for (i = 0; i < obj->numBufs; i++) {
        heapBuf = HeapBuf_create(&(params->bufParams[i]), eb);
        if (heapBuf == NULL) {
            return (3); /* Failed at 3 */
        }

        /* Add the new heapBuf to the bufsBySize array */
        obj->bufsBySize[i] = heapBuf;

        /* Add the new heapBuf to bufsByAddr */
        addrPair.lastAddr = HeapBuf_getEndAddr(heapBuf);
        addrPair.heapBuf = heapBuf;
        /* Copy by value */
        obj->bufsByAddr[i] = addrPair;
    }

    /*
     * Sort the bufConfigs by size, then by align. This simplifies the search
     * for matching bufConfigs.
     */
    qsort(obj->bufsBySize, obj->numBufs,
          sizeof(HeapBuf_Handle), HeapMultiBuf_sizeAlignCompare);

    /*
     * Once all of the heapBufs have been created, sort the bufsByAddr
     * array. The bufConfigs param was sorted, so bufsBySize does not
     * need to be sorted here.
     */
    qsort(obj->bufsByAddr, obj->numBufs,
          sizeof(HeapMultiBuf_AddrPair), HeapMultiBuf_addrPairCompare);

    /* put HeapMultiBuf object on global HeapMultiBuf Object list (Queue_put is atomic) */
    Queue_put(&HeapMultiBuf_module->objQ, &obj->objElem);

    return (0); /* Success */
}

/*
 *  ======== HeapMultiBuf_Instance_finalize ========
 *  Deletes the managed HeapBuf instances and frees the memory for the
 *  sorted buffer arrays.
 */
void HeapMultiBuf_Instance_finalize(HeapMultiBuf_Object *obj, int status)
{
    int i;

    /* If a create failed, handle the finalize based on the status. */
    switch (status) {
        /* Alloc of bufsByAddr failed. */
        case 1:
            return;
        /* Alloc of bufsBySize failed. */
        case 2:
            /* bufsByAddr succeeded, so free bufsByAddr. */
            Memory_free(NULL, obj->bufsByAddr, obj->numBufs * sizeof(HeapMultiBuf_AddrPair));
            return;
        case 3:
            /* Delete any heapBufs that were successfully created. */
            for (i = 0; i < obj->numBufs; i++) {
                /*
                 * bufsBySize was calloc'd, so we can check for NULL to find
                 * the end of the created HeapBufs.
                 */
                if (&(obj->bufsBySize[i]) == NULL) {
                    break;
                }

                HeapBuf_delete(&(obj->bufsBySize[i]));
            }
            /* Free both the bufsByAddr and bufsBySize arrays. */
            Memory_free(NULL, obj->bufsByAddr, obj->numBufs * sizeof(HeapMultiBuf_AddrPair));
            Memory_free(NULL, obj->bufsBySize, obj->numBufs * sizeof(HeapBuf_Object*));
            return;
    }

    /*
     * If finalize is being called through a normal delete, rather than
     * through a failed create...
     */

    /* Delete all of the created HeapBufs */
    for (i = 0; i < obj->numBufs; i++) {
        HeapBuf_delete(&(obj->bufsBySize[i]));
    }

    /* Free the bufsByAddr and bufsBySize arrays. */
    Memory_free(NULL, obj->bufsByAddr, obj->numBufs * sizeof(HeapMultiBuf_AddrPair));
    Memory_free(NULL, obj->bufsBySize, obj->numBufs * sizeof(HeapBuf_Object*));
}


/*
 *  ======== HeapMultiBuf_alloc ========
 *  Searches through the buffers by size and returns a block from the first
 *  buffer with a large enough block size.
 *  If block borrowing is enabled and the HeapMultiBuf is out of blocks
 *  of the requested size, it will continue searching for the next larger
 *  block available. If block borrowing is disabled, it will just return
 *  a NULL ptr.
 */
void *HeapMultiBuf_alloc(HeapMultiBuf_Object *obj, size_t size, size_t align,
                    Error_Block *eb)
{
    int i;
    HeapBuf_Handle bufHandle;
    size_t heapBlockSize;
    size_t heapBlockAlign;
    void * block;

    /* Search for the HeapBuf with the correct block size for this alloc. */
    for (i = 0; i < obj->numHeapBufs; i++) {
        bufHandle = obj->bufsBySize[i];
        heapBlockSize = HeapBuf_getBlockSize(bufHandle);
        heapBlockAlign = HeapBuf_getAlign(bufHandle);

        /*
         * - The requested size must be <= to the HeapBuf's block size.
         * - The requested alignment must be <= the HeapBuf's alignment.
         *   (A HeapBuf with align=64 can service a request for align=8).
         *   The alignments must be powers of 2.
         */
        if ((size <= heapBlockSize) && (heapBlockAlign >= align)) {

            block = HeapBuf_alloc(bufHandle, size, align, eb);
            /*
             * If the last heap alloc fails do not attempt to
             * borrow from non-existent buffer.
             */
            if ((block == NULL) && (i != (obj->numHeapBufs - 1))){

                /*
                 *  If we're out of blocks of the requested size and block
                 *  borrowing is enabled so go looking.
                 *  Note: the next block might be the same size and alignment
                 *  since we don't merge the buckets.
                 */
                if (obj->blockBorrow) {
                    return (HeapMultiBuf_borrowBlock(obj, size, align, i + 1));
                }
                else {
                    /*
                     *  If the next HeapBuf has the same BlockSize and
                     *  alignment, loop back around and use it.
                     *  Note: if the next bucket has larger alignment do not
                     *  take from it (unless blockBorrow was true...which you
                     *  would have executed the above if instead).
                     */
                    bufHandle = obj->bufsBySize[i + 1];
                    if ((HeapBuf_getBlockSize(bufHandle) == heapBlockSize) &&
                        (HeapBuf_getAlign(bufHandle) == heapBlockAlign)) {
                        continue;
                    }
                }
            }

            return (block);
        }
    }

    /*
     * Return null if the requested size was larger than any
     * of the block sizes.
     */
    Error_raise(eb, HeapMultiBuf_E_size, (intptr_t)obj, (intptr_t)size);

    return (NULL);
}

/*
 *  ======== addrPairCompare ========
 *  Comparison function for qsort. Compares AddrPairs by lastAddr to
 *  sort them in ascending order.
 */
int HeapMultiBuf_addrPairCompare(const void *a, const void *b)
{
    char *ap = ((HeapMultiBuf_AddrPair *) a)->lastAddr;
    char *bp = ((HeapMultiBuf_AddrPair *) b)->lastAddr;
    return (ap - bp);
}

/*
 *  ======== sizeAlignCompare ========
 *  Comparison function for qsort. Compares HeapBufs first by blockSize, then
 *  by align, in ascending order.
 */
int HeapMultiBuf_sizeAlignCompare(const void *a, const void *b)
{
    int diff;

    diff = HeapBuf_getBlockSize(*((HeapBuf_Handle *) a)) -
           HeapBuf_getBlockSize(*((HeapBuf_Handle *) b));

    /* If the blockSizes match, sort them by ascending align */
    if (diff == 0) {
        diff = HeapBuf_getAlign(*((HeapBuf_Handle *) a)) -
               HeapBuf_getAlign(*((HeapBuf_Handle *) b));
    }

    return (diff);
}

/*
 *  ======== blockBorrow ========
 *  Search for the next available block with the requested alignment
 *  starting at startIndex.
 */
void *HeapMultiBuf_borrowBlock(HeapMultiBuf_Object *obj, size_t size,
                                      size_t align, int startIndex)
{
    int i;
    void * block;

    for (i = startIndex; i < obj->numHeapBufs; i++) {
        /*
         * All other HeapBufs have larger blocks, so we don't check size.
         * Since the alignments must be powers of 2, we can just compare them
         * with '>='.
         */
        if (HeapBuf_getAlign(obj->bufsBySize[i]) >= align) {
            block = HeapBuf_alloc(obj->bufsBySize[i], size, align, NULL);
            if (block != NULL) {
                return (block);
            }
        }
    }

    return (NULL);
}

/*
 *  ======== HeapMultiBuf_free ========
 *  Since the block must be returned to the buffer with both the correct size
 *  and alignment, and since we aren't given the alignment, we must find the
 *  proper buffer to free to by comparing addresses.
 *  bufsByAddr maps the ending addresses of the buffers to the heapBuf that
 *  manages them, and is sorted by address.
 *  Freeing by comparing addresses also allows us to implement block borrowing,
 *  since the issue with block borrowing is knowing which buffer to free to.
 */
void HeapMultiBuf_free(HeapMultiBuf_Object *obj, void * block, size_t size)
{
    int i;
    HeapMultiBuf_AddrPair pair;

    /* Search for the block's HeapBuf by comparing addresses. */
    for (i = 0; i < obj->numBufs; i++) {
        pair = obj->bufsByAddr[i];
        if (block <= pair.lastAddr) { /* <=, not < */
            /* ASSERT(HeapBuf_getBlockSize(pair.heapBuf) >= size) */
            HeapBuf_free(pair.heapBuf, block, size);
            return;
        }
    }

    /* Raise an error if the block wasn't freed. */
    Assert_isTrue((false), HeapMultiBuf_A_blockNotFreed);
}

/*
 *  ======== HeapMultiBuf_isBlocking ========
 */
bool HeapMultiBuf_isBlocking(HeapMultiBuf_Object *obj)
{
    return (false);
}

/*
 *  ======== HeapMultiBuf_getStats ========
 *  getStats is thread safe, though it's possible that the actual
 *  state could change before getStats returns.
 */
void HeapMultiBuf_getStats(HeapMultiBuf_Object *obj, Memory_Stats *stats)
{
    int i;
    Memory_Stats bufStats;

    stats->totalSize         = 0;
    stats->totalFreeSize     = 0;
    stats->largestFreeSize   = 0;

    for (i = 0; i < obj->numHeapBufs; i++) {
        /* HeapBuf_getStats is atomic */
        HeapBuf_getStats(obj->bufsBySize[i], &bufStats);

        stats->totalSize         += bufStats.totalSize;
        stats->totalFreeSize     += bufStats.totalFreeSize;
        if (bufStats.largestFreeSize > stats->largestFreeSize) {
            stats->largestFreeSize = bufStats.largestFreeSize;
        }
    }
}

/*
 *  ======== HeapMultiBuf_init ========
 */
void HeapMultiBuf_init(void)
{
}

/*
 *  ======== HeapMultiBuf_construct ========
 */
HeapMultiBuf_Handle HeapMultiBuf_construct(HeapMultiBuf_Object *obj,
        const HeapMultiBuf_Params *params, Error_Block *eb)
{
    int status;

    if (params == NULL) {
        params = &HeapMultiBuf_Params_default;
    }

    status = HeapMultiBuf_Instance_init(obj, params, eb);

    if (status != 0) {
        HeapMultiBuf_Instance_finalize(obj, status);

        return (NULL);
    }

    obj->iheap.alloc = (IHeap_AllocFxn)HeapMultiBuf_alloc;
    obj->iheap.free = (IHeap_FreeFxn)HeapMultiBuf_free;
    obj->iheap.isBlocking = (IHeap_IsBlockingFxn)HeapMultiBuf_isBlocking;
    obj->iheap.getStats = (IHeap_GetStatsFxn)HeapMultiBuf_getStats;

    return (obj);
}

/*
 *  ======== HeapMultiBuf_create ========
 */
HeapMultiBuf_Handle HeapMultiBuf_create(
        const HeapMultiBuf_Params *params, Error_Block *eb)
{
    HeapMultiBuf_Handle heap;

    heap = Memory_alloc(NULL, sizeof(HeapMultiBuf_Object), 0, eb);

    if (heap != NULL) {
        if (HeapMultiBuf_construct(heap, params, eb) == NULL) {
            Memory_free(NULL, heap, sizeof(HeapMultiBuf_Object));
            heap = NULL;
        }
    }

    return (heap);
}

/*
 *  ======== HeapMultiBuf_destruct ========
 */
void HeapMultiBuf_destruct(HeapMultiBuf_Object *obj)
{
    unsigned int hwiKey;

    HeapMultiBuf_Instance_finalize(obj, 0);

    /* remove HeapMultiBuf object from global HeapMultiBuf object list (Queue_remove is not atomic) */
    hwiKey = Hwi_disable();
    Queue_remove(&obj->objElem);
    Hwi_restore(hwiKey);
}

/*
 *  ======== HeapMultiBuf_delete ========
 */
void HeapMultiBuf_delete(HeapMultiBuf_Handle *handle)
{
    HeapMultiBuf_destruct(*handle);

    Memory_free(NULL, *handle, sizeof(HeapMultiBuf_Object));

    *handle = NULL;
}

/*
 *  ======== HeapMultiBuf_Params_init ========
 */
void HeapMultiBuf_Params_init(HeapMultiBuf_Params *params)
{
    *params = HeapMultiBuf_Params_default;
}

/*
 *  ======== HeapMultiBuf_getHeapMultiBufFromObjElem ========
 */
HeapMultiBuf_Handle HeapMultiBuf_getHeapMultiBufFromObjElem(Queue_Elem *heapQelem)
{
    if (heapQelem == (Queue_Elem *)&HeapMultiBuf_module->objQ) {
        return (NULL);
    }

    return ((HeapMultiBuf_Handle)((char *)heapQelem -
               offsetof(HeapMultiBuf_Struct, objElem)));
}

/*
 *  ======== HeapMultiBuf_Object_first ========
 */
HeapMultiBuf_Handle HeapMultiBuf_Object_first()
{
    return (HeapMultiBuf_getHeapMultiBufFromObjElem(Queue_head(
                &(HeapMultiBuf_module->objQ))));
}

/*
 *  ======== HeapMultiBuf_Object_next ========
 */
HeapMultiBuf_Handle HeapMultiBuf_Object_next(HeapMultiBuf_Handle handle)
{
    return (HeapMultiBuf_getHeapMultiBufFromObjElem(Queue_next(
                &handle->objElem)));
}
