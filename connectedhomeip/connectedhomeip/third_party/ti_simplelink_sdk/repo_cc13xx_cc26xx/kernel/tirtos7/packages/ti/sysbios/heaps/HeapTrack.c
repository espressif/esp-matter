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
 *  ======== HeapTrack.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/heaps/HeapTrack.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Queue.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/IHeap.h>
#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/System.h>

HeapTrack_Module_State HeapTrack_Module_state = {
    .objQ.next = &HeapTrack_Module_state.objQ,
    .objQ.prev = &HeapTrack_Module_state.objQ
};

static const HeapTrack_Params HeapTrack_Params_default = {
    .heap = NULL,
};

/*
 *  ======== HeapTrack_printTrack ========
 */
bool HeapTrack_printTrack(HeapTrack_Tracker *tracker, HeapTrack_Handle handle)
{
    size_t rem, buf;
// TODO
//  char * name = Task_Handle_name(tracker->taskHandle);
    char * name = "";


    /* Scribble was word aligned, reverse our logic to get buf */
    rem = tracker->size % sizeof(uint32_t) ?
                         sizeof(uint32_t) - (tracker->size % sizeof(uint32_t)) : 0;
    buf = (uintptr_t)tracker - tracker->size - rem;

    /* Print task name if it's there */
// TODO
//  if (name != Text_nameEmpty) {
    if (name != NULL) {
        System_printf("HeapTrack: HeapTrack_Obj: 0x%x, addr = 0x%x, "
                      "taskHandle: 0x%x, taskName: %s, size = %d, tick = %d\n",
                      handle, buf, tracker->taskHandle, name,
                      tracker->size, tracker->tick);
    }
    else {
        System_printf("HeapTrack: HeapTrack_Obj: 0x%x, addr = 0x%x, "
                      "taskHandle: 0x%x, size = %d, tick = %d\n",
                      handle, buf, tracker->taskHandle,
                      tracker->size, tracker->tick);
    }

    /* Check scribble and print if it is corrupted */
    if (tracker->scribble != HeapTrack_STARTSCRIBBLE) {
        System_printf("Memory at 0x%x has a corrupted scribble (0x%x)\n",
            buf, tracker->scribble);
        return (false);
    }
    return (true);
}

/*
 *  ======== HeapTrack_alloc ========
 */
void *HeapTrack_alloc(HeapTrack_Object *obj, size_t size, size_t align,
                                                               Error_Block *eb)
{
    char *buf;
    size_t rem;
    size_t allocSize;
    Queue_Handle trackQueue;
    HeapTrack_Tracker *tracker;
    unsigned int key;

    trackQueue = &obj->trackQueue;

    /* Scribble must be uintptr_t aligned, calculate remainder to add in */
    rem = size % sizeof(uintptr_t) ? sizeof(uintptr_t) - (size % sizeof(uintptr_t)) : 0;

    allocSize = size + rem + sizeof(HeapTrack_Tracker);

    /*
     * If size is very large and allocSize overflows, the result will be
     * smaller than size. In this case, don't try to allocate.
     */
    if (allocSize < size) {
        return (NULL);
    }

    /* Add in the Tracker structure and remainder */
    buf = Memory_alloc(obj->internalHeap, allocSize, align, eb);
    if (buf == NULL) {
        return (NULL);
    }

    /* Save new heap size and update peak sizes */
    key = Hwi_disable();
    obj->size += allocSize;
    if (obj->size > obj->peak) {
        obj->peak = obj->size;
    }

    obj->sizeWithoutTracker += (size + rem);
    if (obj->sizeWithoutTracker > obj->peakWithoutTracker) {
        obj->peakWithoutTracker = obj->sizeWithoutTracker;
    }
    Hwi_restore(key);

    /* Fill in the tracker structure at the end of the buffer */
    tracker             = (HeapTrack_Tracker *)((char *)buf + size + rem);
    tracker->scribble   = HeapTrack_STARTSCRIBBLE;
    tracker->taskHandle = Task_self();
    tracker->size       = size;
    tracker->tick       = Clock_getTicks();

    /* Enqueue into the heap's linked list */
    Queue_put(trackQueue, &(tracker->queElem));

    return (buf);
}

/*
 *  ======== HeapTrack_free ========
 */
void HeapTrack_free(HeapTrack_Object *obj, void * buf, size_t size)
{
    size_t rem;
    HeapTrack_Tracker *tracker;
    unsigned int key;

    /* Scribble was word aligned, reverse our logic */
    rem = size % sizeof(uintptr_t) ? sizeof(uintptr_t) - (size % sizeof(uintptr_t)) : 0;

    /* Get the tracker structure */
    tracker = (HeapTrack_Tracker *)((char *)buf + size + rem);

    /* Check for a double free */
    Assert_isTrue(tracker->scribble != HeapTrack_NOSCRIBBLE,
                                                       HeapTrack_A_doubleFree);

    /* Validate that the scribble is still good */
    Assert_isTrue(tracker->scribble == HeapTrack_STARTSCRIBBLE,
                                                      HeapTrack_A_bufOverflow);

    key = Hwi_disable();

    /* Remove it from the linked list */
    Queue_remove(&(tracker->queElem));

    /*
     *  Clean the scribble out. Using a non-zero constant to aid
     *  in the development of this module and check for double frees.
     */
    tracker->scribble = HeapTrack_NOSCRIBBLE;

    /* Update size */
    obj->size -= (size + rem + sizeof(HeapTrack_Tracker));
    obj->sizeWithoutTracker -= (size + rem);
    Hwi_restore(key);

    /* Finally free the memory back to the real heap. */
    Memory_free(obj->internalHeap, buf, size+rem + sizeof(HeapTrack_Tracker));
}

/*
 *  ======== HeapTrack_getStats ========
 */
void HeapTrack_getStats(HeapTrack_Object *obj, Memory_Stats *stats)
{
    /* call the heap's stats function */
    Memory_getStats(obj->internalHeap, stats);

    /* update free sizes to account for a HeapTrack_Tracker structure */
    if (stats->totalFreeSize > sizeof(HeapTrack_Tracker)) {
        stats->totalFreeSize -= sizeof(HeapTrack_Tracker);
    }
    else {
        stats->totalFreeSize = 0;
    }
    if (stats->largestFreeSize > sizeof(HeapTrack_Tracker)) {
        stats->largestFreeSize -= sizeof(HeapTrack_Tracker);
    }
    else {
        stats->largestFreeSize = 0;
    }
}

/*
 *  ======== HeapTrack_Instance_finalize ========
 */
void HeapTrack_Instance_finalize(HeapTrack_Object *obj)
{
    /* Check the heap is empty */
    Assert_isTrue(obj->size == 0, HeapTrack_A_notEmpty);

    Queue_destruct(&obj->trackQueue);
}

/*
 *  ======== HeapTrack_Instance_init ========
 */
void HeapTrack_Instance_init(HeapTrack_Object *obj,
        const HeapTrack_Params *params)
{
    Queue_construct(&obj->trackQueue, NULL);

    obj->internalHeap = params->heap;
    obj->peak = 0;
    obj->peakWithoutTracker = 0;
}

/*
 *  ======== HeapTrack_isBlocking ========
 */
bool HeapTrack_isBlocking(HeapTrack_Object *obj)
{
    /* Just call the heap's function */
    return (Memory_query(obj->internalHeap, Memory_Q_BLOCKING));
}

/*
 *  ======== HeapTrack_printHeap ========
 */
void HeapTrack_printHeap(HeapTrack_Object *obj)
{
    Queue_Handle trackQueue;
    HeapTrack_Tracker *tracker;
    Queue_Elem *elem;
    bool continueFlag;

    /* Check obj is not NULL */
    Assert_isTrue(obj != NULL, HeapTrack_A_nullObject);

    trackQueue = &obj->trackQueue;

    elem = Queue_head(trackQueue);

    /*
     *  Loop through all the allocated buffers and print out
     *  the details about the buffer.
     */
    while (elem != (Queue_Elem *)(trackQueue)) {
        /* The start of the struct is up above the scribble */
        tracker = (HeapTrack_Tracker *)((char *)elem - sizeof(uintptr_t));
        continueFlag = HeapTrack_printTrack(tracker, obj);
        if (continueFlag == false) {
            break;
        }
        elem = Queue_next(elem);
    }
}

/*
 *  ======== HeapTrack_printTask ========
 *  not safe to delete or create threads during this call
 */
void HeapTrack_printTask(Task_Handle task)
{
    Queue_Elem *elem;
    HeapTrack_Object *obj;
    Queue_Handle trackQueue;
    HeapTrack_Tracker *tracker;
    bool continueFlag;

    /*
     *  Loop through all the dynamically created heaps to find the
     *  allocated buffers that are owned by the task and print out
     *  the details about the buffer.
     */
    for (obj = HeapTrack_Object_first(); obj != NULL;
         obj = HeapTrack_Object_next(obj)) {
        trackQueue = &obj->trackQueue;

        /* Get the head elem */
        elem = Queue_head(trackQueue);

        /* Loop until we get back to the Queue Object */
        while (elem != (Queue_Elem *)(trackQueue)) {

            /*
             *  The start of the struct is up above the scribble.  Subtract
             *  sizeof(uintptr_t) from the Queue_Elem address to get the start of
             *  the structure, since scribble is a uintptr_t.  Do not use
             *  sizeof(HeapTrack_STARTSCRIBBLE), since xdctools generates
             *  a #define for this, which on 64-bit targets has a size of 4.
             */
            tracker = (HeapTrack_Tracker *)((char *)elem - sizeof(uintptr_t));

            if (tracker->taskHandle == task) {
                continueFlag = HeapTrack_printTrack(tracker, obj);
                if (continueFlag == false) {
                    break;
                }
            }
            elem = Queue_next(elem);
        }
    }
}

/*
 *  ======== HeapTrack_init ========
 */
void HeapTrack_init(void)
{
    Queue_construct(&HeapTrack_Module_state.objQ, NULL);
}

/*
 *  ======== HeapTrack_construct ========
 */
HeapTrack_Handle HeapTrack_construct(HeapTrack_Object *obj,
        const HeapTrack_Params *params)
{
    if (params == NULL) {
        params = &HeapTrack_Params_default;
    }

    HeapTrack_Instance_init(obj, params);

    obj->iheap.alloc = (IHeap_AllocFxn)HeapTrack_alloc;
    obj->iheap.free = (IHeap_FreeFxn)HeapTrack_free;
    obj->iheap.isBlocking = (IHeap_IsBlockingFxn)HeapTrack_isBlocking;
    obj->iheap.getStats = (IHeap_GetStatsFxn)HeapTrack_getStats;

    /* put object on HeapTrack object list (atomic) */
    Queue_put(&HeapTrack_Module_state.objQ, &obj->objElem);

    return (obj);
}

/*
 *  ======== HeapTrack_create ========
 */
HeapTrack_Handle HeapTrack_create(
        const HeapTrack_Params *params, Error_Block *eb)
{
    HeapTrack_Handle heap;

    heap = Memory_alloc(NULL, sizeof(HeapTrack_Object), 0, eb);

    if (heap != NULL) {
        HeapTrack_construct(heap, params);
    }

    return (heap);
}

/*
 *  ======== HeapTrack_destruct ========
 */
void HeapTrack_destruct(HeapTrack_Object *obj)
{
    unsigned int key;

    HeapTrack_Instance_finalize(obj);

    /* Queue_remove() is not atomic */
    key = Hwi_disable();
    Queue_remove(&obj->objElem);
    Hwi_restore(key);
}

/*
 *  ======== HeapTrack_delete ========
 */
void HeapTrack_delete(HeapTrack_Handle *handle)
{
    HeapTrack_Instance_finalize(*handle);

    Memory_free(NULL, *handle, sizeof(HeapTrack_Object));

    *handle = NULL;
}

/*
 *  ======== HeapTrack_Params_init ========
 */
void HeapTrack_Params_init(HeapTrack_Params *params)
{
    *params = HeapTrack_Params_default;
}

/*
 *  ======== HeapTrack_getHeapTrackFromObjElem ========
 */
HeapTrack_Handle HeapTrack_getHeapTrackFromObjElem(Queue_Elem *heapQelem)
{
    if (heapQelem == (Queue_Elem *)&HeapTrack_module->objQ) {
        return (NULL);
    }

    return ((HeapTrack_Handle)((char *)heapQelem -
               offsetof(HeapTrack_Struct, objElem)));
}

/*
 *  ======== HeapTrack_Object_first ========
 */
HeapTrack_Handle HeapTrack_Object_first()
{
    return (HeapTrack_getHeapTrackFromObjElem(Queue_head(
                &(HeapTrack_module->objQ))));
}

/*
 *  ======== HeapTrack_Object_next ========
 */
HeapTrack_Handle HeapTrack_Object_next(HeapTrack_Handle handle)
{
    return (HeapTrack_getHeapTrackFromObjElem(Queue_next(&handle->objElem)));
}
