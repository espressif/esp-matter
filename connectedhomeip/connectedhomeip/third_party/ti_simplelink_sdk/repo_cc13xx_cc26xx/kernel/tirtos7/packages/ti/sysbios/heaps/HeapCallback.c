/*
 * Copyright (c) 2013-2020, Texas Instruments Incorporated
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
 *  ======== HeapCallback.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/heaps/HeapCallback.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/Startup.h>

HeapCallback_Module_State HeapCallback_Module_state = {
    .objQ.next = &HeapCallback_Module_state.objQ,
    .objQ.prev = &HeapCallback_Module_state.objQ
};

static const HeapCallback_Params HeapCallback_Params_default = {
    .arg = 0
};

extern void * HeapCallback_alloc_D(uintptr_t context, size_t size, size_t align);
extern uintptr_t HeapCallback_create_D(uintptr_t arg);
extern void HeapCallback_delete_D(uintptr_t context);
extern void HeapCallback_free_D(uintptr_t context, void * addr, size_t size);
extern void HeapCallback_getStats_D(uintptr_t context, Memory_Stats *stats);
extern void HeapCallback_init_D(void);
extern bool HeapCallback_isBlocking_D(uintptr_t context);

/*
 *  ======== HeapCallback_Instance_finalize ========
 */
void HeapCallback_Instance_finalize(HeapCallback_Object *obj)
{
    HeapCallback_delete_D(obj->context);
}

/*
 *  ======== HeapCallback_Instance_init ========
 */
void HeapCallback_Instance_init(HeapCallback_Object *obj,
        const HeapCallback_Params *params)
{
    obj->context = HeapCallback_create_D(params->arg);
}

/*
 *  ======== HeapCallback_alloc ========
 */
void * HeapCallback_alloc(HeapCallback_Object *obj, size_t size,
        size_t align, Error_Block *eb)
{
    void * allocAddr;

    allocAddr = HeapCallback_alloc_D(obj->context, size, align);

    return (allocAddr);
}

/*
 *  ======== HeapCallback_free ========
 */
void HeapCallback_free(HeapCallback_Object *obj, void * addr, size_t size)
{
    HeapCallback_free_D(obj->context, addr, size);
}

/*
 *  ======== HeapCallback_getContext ========
 */
uintptr_t HeapCallback_getContext(HeapCallback_Object *obj)
{
    return (obj->context);
}

/*
 *  ======== HeapCallback_getStats ========
 */
void HeapCallback_getStats(HeapCallback_Object *obj, Memory_Stats *stats)
{
    HeapCallback_getStats_D(obj->context, stats);
}

/*
 *  ======== HeapCallback_isBlocking ========
 */
bool HeapCallback_isBlocking(HeapCallback_Object *obj)
{
    return (HeapCallback_isBlocking_D(obj->context));
}

/*
 *  ======== HeapCallback_defaultAlloc ========
 */
void * HeapCallback_defaultAlloc(uintptr_t context, size_t size, size_t align)
{
    return (NULL);
}

/*
 *  ======== HeapCallback_defaultCreate  ========
 *  Default implementation of create callback function
 */
uintptr_t HeapCallback_defaultCreate(uintptr_t arg)
{
    return (0);
}

/*
 *  ======== HeapCallback_defaultDelete  ========
 *  Default implementation of delete callback function
 */
void HeapCallback_defaultDelete(uintptr_t context)
{
}

/*
 *  ======== HeapCallback_defaultFree ========
 */
void HeapCallback_defaultFree(uintptr_t context, void * addr, size_t size)
{
}

/*
 *  ======== HeapCallback_defaultGetStats ========
 */
void HeapCallback_defaultGetStats(uintptr_t context, Memory_Stats *stats)
{
}

/*
 *  ======== HeapCallback_defaultInit ========
 */
void HeapCallback_defaultInit(void)
{
}

/*
 *  ======== HeapCallback_defaultIsBlocking ========
 */
bool HeapCallback_defaultIsBlocking(uintptr_t context)
{
    return (false);
}

/*
 *  ======== HeapCallback_init ========
 */
void HeapCallback_init(void)
{
    HeapCallback_init_D();
}

/*
 *  ======== HeapCallback_construct ========
 */
HeapCallback_Handle HeapCallback_construct(HeapCallback_Object *obj,
        const HeapCallback_Params *params)
{
    if (params == NULL) {
        params = &HeapCallback_Params_default;
    }

    HeapCallback_Instance_init(obj, params);

    obj->iheap.alloc = (IHeap_AllocFxn)HeapCallback_alloc;
    obj->iheap.free = (IHeap_FreeFxn)HeapCallback_free;
    obj->iheap.isBlocking = (IHeap_IsBlockingFxn)HeapCallback_isBlocking;
    obj->iheap.getStats = (IHeap_GetStatsFxn)HeapCallback_getStats;
    Queue_put(&HeapCallback_module->objQ, &obj->objElem);

    return (obj);
}

/*
 *  ======== HeapCallback_create ========
 */
HeapCallback_Handle HeapCallback_create(
        const HeapCallback_Params *params, Error_Block *eb)
{
    HeapCallback_Handle heap;

    heap = Memory_alloc(NULL, sizeof(HeapCallback_Object), 0, eb);

    if (heap != NULL) {
        HeapCallback_construct(heap, params);
    }

    return (heap);
}

/*
 *  ======== HeapCallback_destruct ========
 */
void HeapCallback_destruct(HeapCallback_Object *obj)
{
    unsigned int hwiKey;

    HeapCallback_Instance_finalize(obj);

    /*
     *  remove HeapCallback object from global HeapCallback object list
     *  (Queue_remove is not atomic)
     */
    hwiKey = Hwi_disable();
    Queue_remove(&obj->objElem);
    Hwi_restore(hwiKey);
}

/*
 *  ======== HeapCallback_delete ========
 */
void HeapCallback_delete(HeapCallback_Handle *handle)
{
    HeapCallback_destruct(*handle);

    Memory_free(NULL, *handle, sizeof(HeapCallback_Object));

    *handle = NULL;
}

/*
 *  ======== HeapCallback_Params_init ========
 */
void HeapCallback_Params_init(HeapCallback_Params *params)
{
    *params = HeapCallback_Params_default;
}
/*
 *  ======== HeapCallback_getHeapCallbackFromObjElem ========
 */
HeapCallback_Handle HeapCallback_getHeapCallbackFromObjElem(Queue_Elem *heapQelem)
{
    if (heapQelem == (Queue_Elem *)&HeapCallback_module->objQ) {
        return (NULL);
    }

    return ((HeapCallback_Handle)((char *)heapQelem -
               offsetof(HeapCallback_Struct, objElem)));
}

/*
 *  ======== HeapCallback_Object_first ========
 */
HeapCallback_Handle HeapCallback_Object_first()
{
    return (HeapCallback_getHeapCallbackFromObjElem(Queue_head(
                &(HeapCallback_module->objQ))));
}

/*
 *  ======== HeapCallback_Object_next ========
 */
HeapCallback_Handle HeapCallback_Object_next(HeapCallback_Handle handle)
{
    return (HeapCallback_getHeapCallbackFromObjElem(Queue_next(
                &handle->objElem)));
}
