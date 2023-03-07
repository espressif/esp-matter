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
 *  ======== HeapMin.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/heaps/HeapMin.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Memory.h>

HeapMin_Module_State HeapMin_Module_state = {
    .objQ.next = &HeapMin_Module_state.objQ,
    .objQ.prev = &HeapMin_Module_state.objQ
};

static const HeapMin_Params HeapMin_Params_default = {
    .buf = NULL,
    .size = 0
};

/*
 *  ======== HeapMin_Instance_init ========
 *  Set-up the object's fields.
 */
void HeapMin_Instance_init(HeapMin_Object *obj, const HeapMin_Params *params)
{
    /* Make sure size is non-zero */
    Assert_isTrue((params->size != 0), HeapMin_A_zeroSize);

    obj->buf                = params->buf;
    obj->remainSize         = params->size;
    obj->startSize          = params->size;

    /* put HeapMin object on global HeapMin Object list (Queue_put is atomic) */
    Queue_put(&HeapMin_module->objQ, &obj->objElem);
}

/*
 *  ======== HeapMin_alloc ========
 *  This is a simple growth-only heap. This function returns the current
 *  location of the top of unused buffer.
 */
void * HeapMin_alloc(void *handle, size_t size, size_t align,
    Error_Block *eb)
{
    HeapMin_Object *obj = (HeapMin_Object *)handle;
    void * block;
    int key;
    size_t offset = 0;

    key = Hwi_disable();
    /*
     * If the current buffer is not on the requested alignment,
     * determine the offset needed.
     */
    if ((uintptr_t)(obj->buf) & (align - 1)) {
        offset = align - ((uintptr_t)(obj->buf) & (align - 1));
    }

    /* Make sure there is enough memory.  Must factor in the offset. */
    if (((size_t)obj->remainSize) < size + offset) {
        Hwi_restore(key);
        return (NULL);
    }

    /* Determine the addr based off the current buf and the needed offset. */
    block = obj->buf + offset;

    /* Update the two instance fields accordingly */
    obj->remainSize -= (size + offset);
    obj->buf        += (size + offset);
    Hwi_restore(key);

    return (block);
}

/*
 *  ======== HeapMin_free ========
 *  This is a growth-only heap, so free does nothing.
 */
void HeapMin_free(void *handle, void * block, size_t size)
{
    if (HeapMin_freeError == true) {
        Error_raise(NULL, HeapMin_E_freeError, 0, 0);
    }
}

/*
 *  ======== HeapMin_isBlocking ========
 */
bool HeapMin_isBlocking(void *handle)
{
    return (false);
}

/*
 *  ======== HeapMin_getStats ========
 *  Return the stats.
 */
void HeapMin_getStats(void *handle, Memory_Stats *stats)
{
    HeapMin_Object *obj = (HeapMin_Object *)handle;

    stats->totalSize         = obj->startSize;

    /* try to ensure that obj->remainSize is accessed just once to avoid
     * the need for a lock; two reads may result in different values of
     * largestFreeSize and totalFreeSize being observed by the caller
     */
    stats->totalFreeSize     = obj->remainSize;
    stats->largestFreeSize   = stats->totalFreeSize;
}

/*
 *  ======== HeapMin_construct ========
 */
HeapMin_Handle HeapMin_construct(HeapMin_Struct *obj, const HeapMin_Params *params)
{
    if (params == NULL) {
        params = &HeapMin_Params_default;
    }

    HeapMin_Instance_init(obj, params);

    obj->iheap.alloc = (IHeap_AllocFxn)HeapMin_alloc;
    obj->iheap.free = (IHeap_FreeFxn)HeapMin_free;
    obj->iheap.isBlocking = (IHeap_IsBlockingFxn)HeapMin_isBlocking;
    obj->iheap.getStats = (IHeap_GetStatsFxn)HeapMin_getStats;

    return (obj);
}

/*
 *  ======== HeapMin_create ========
 */
HeapMin_Handle HeapMin_create(
        const HeapMin_Params *params, Error_Block *eb)
{
    HeapMin_Handle heap;

    heap = Memory_alloc(NULL, sizeof(HeapMin_Object), 0, eb);

    if (heap != NULL) {
        HeapMin_construct(heap, params);
    }

    return (heap);
}

/*
 *  ======== HeapMin_destruct ========
 */
void HeapMin_destruct(HeapMin_Object *obj)
{
    unsigned int hwiKey;

    /* remove HeapMin object from global HeapMin object list (Queue_remove is not atomic) */
    hwiKey = Hwi_disable();
    Queue_remove(&obj->objElem);
    Hwi_restore(hwiKey);
}

/*
 *  ======== HeapMin_delete ========
 */
void HeapMin_delete(HeapMin_Handle *handle)
{
    HeapMin_destruct(*handle);

    Memory_free(NULL, *handle, sizeof(HeapMin_Object));

    *handle = NULL;
}

/*
 *  ======== HeapMin_Params_init ========
 */
void HeapMin_Params_init(HeapMin_Params *params)
{
    *params = HeapMin_Params_default;
}

/*
 *  ======== HeapMin_getHeapMinFromObjElem ========
 */
HeapMin_Handle HeapMin_getHeapMinFromObjElem(Queue_Elem *heapQelem)
{
    if (heapQelem == (Queue_Elem *)&HeapMin_module->objQ) {
        return (NULL);
    }

    return ((HeapMin_Handle)((char *)heapQelem -
               offsetof(HeapMin_Struct, objElem)));
}

/*
 *  ======== HeapMin_Object_first ========
 */
HeapMin_Handle HeapMin_Object_first()
{
    return (HeapMin_getHeapMinFromObjElem(Queue_head(&(HeapMin_module->objQ))));
}

/*
 *  ======== HeapMin_Object_next ========
 */
HeapMin_Handle HeapMin_Object_next(HeapMin_Handle handle)
{
    return (HeapMin_getHeapMinFromObjElem(Queue_next(&handle->objElem)));
}

/*
 *  ======== HeapMin_init ========
 */
void HeapMin_init(void)
{
}
