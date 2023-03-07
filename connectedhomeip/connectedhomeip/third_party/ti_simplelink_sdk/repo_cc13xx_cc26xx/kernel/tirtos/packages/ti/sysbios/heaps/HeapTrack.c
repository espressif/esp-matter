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

#include <xdc/std.h>
#include <xdc/runtime/Text.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Assert.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/hal/Hwi.h>

#include "package/internal/HeapTrack.xdc.h"

/*
 *  ======== HeapTrack_printTrack ========
 */
Bool HeapTrack_printTrack(HeapTrack_Tracker *tracker, HeapTrack_Handle handle)
{
    SizeT rem, buf;
    String name = Task_Handle_name(tracker->taskHandle);

    /* Scribble was word aligned, reverse our logic to get buf */
    rem = tracker->size % sizeof(UInt32) ?
                         sizeof(UInt32) - (tracker->size % sizeof(UInt32)) : 0;
    buf = (UArg)tracker - tracker->size - rem;

    /* Print task name if it's there */
    if (name != Text_nameEmpty) {
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
        return (FALSE);
    }
    return (TRUE);
}

/*
 *  ======== HeapTrack_alloc ========
 */
Void *HeapTrack_alloc(HeapTrack_Object *obj, SizeT size, SizeT align,
                                                               Error_Block *eb)
{
    Char *buf;
    SizeT rem;
    SizeT allocSize;
    Queue_Handle trackQueue;
    HeapTrack_Tracker *tracker;
    UInt key;

    trackQueue = HeapTrack_Instance_State_trackQueue(obj);

    /* Scribble must be UArg aligned, calculate remainder to add in */
    rem = size % sizeof(UArg) ? sizeof(UArg) - (size % sizeof(UArg)) : 0;

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
    tracker             = (HeapTrack_Tracker *)((Char *)buf + size + rem);
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
Void HeapTrack_free(HeapTrack_Object *obj, Ptr buf, SizeT size)
{
    SizeT rem;
    HeapTrack_Tracker *tracker;
    UInt key;

    /* Scribble was word aligned, reverse our logic */
    rem = size % sizeof(UArg) ? sizeof(UArg) - (size % sizeof(UArg)) : 0;

    /* Get the tracker structure */
    tracker = (HeapTrack_Tracker *)((Char *)buf + size + rem);

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
Void HeapTrack_getStats(HeapTrack_Object *obj, Memory_Stats *stats)
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
Void HeapTrack_Instance_finalize(HeapTrack_Object *obj)
{
    Queue_Handle trackQueue;

    /* Check the heap is empty */
    Assert_isTrue(obj->size == 0, HeapTrack_A_notEmpty);

    trackQueue = HeapTrack_Instance_State_trackQueue(obj);
    Queue_destruct(Queue_struct(trackQueue));
}

/*
 *  ======== HeapTrack_Instance_init ========
 */
Void HeapTrack_Instance_init(HeapTrack_Object *obj,
                                                const HeapTrack_Params *params)
{
    Queue_Handle trackQueue;

    trackQueue = HeapTrack_Instance_State_trackQueue(obj);
    Queue_construct(Queue_struct(trackQueue), NULL);

    obj->internalHeap = params->heap;
    obj->peak = 0;
    obj->peakWithoutTracker = 0;
}

/*
 *  ======== HeapTrack_isBlocking ========
 */
Bool HeapTrack_isBlocking(HeapTrack_Object *obj)
{
    /* Just call the heap's function */
    return (IHeap_isBlocking(obj->internalHeap));
}

/*
 *  ======== HeapTrack_printHeap ========
 */
Void HeapTrack_printHeap(HeapTrack_Object *obj)
{
    Queue_Handle trackQueue;
    HeapTrack_Tracker *tracker;
    Queue_Elem *elem;
    Bool continueFlag;

    /* Check obj is not NULL */
    Assert_isTrue(obj != NULL, HeapTrack_A_nullObject);

    trackQueue = HeapTrack_Instance_State_trackQueue(obj);

    elem = Queue_head(trackQueue);

    /*
     *  Loop through all the allocated buffers and print out
     *  the details about the buffer.
     */
    while (elem != (Queue_Elem *)(trackQueue)) {
        /* The start of the struct is up above the scribble */
        tracker = (HeapTrack_Tracker *)((Char *)elem - sizeof(UArg));
        continueFlag = HeapTrack_printTrack(tracker, obj);
        if (continueFlag == FALSE) {
            break;
        }
        elem = Queue_next(elem);
    }
}

/*
 *  ======== HeapTrack_printTask ========
 *  not safe to delete or create threads during this call
 */
Void HeapTrack_printTask(Task_Handle task)
{
    Int i;
    Queue_Elem *elem;
    HeapTrack_Object *obj;
    Queue_Handle trackQueue;
    HeapTrack_Tracker *tracker;
    Bool continueFlag;

    /*
     *  Loop through all the statically created heaps to find the
     *  allocated buffers that are owned by the task and print out
     *  the details about the buffer.
     */
    for (i = 0; i < HeapTrack_Object_count(); i++) {
        obj = HeapTrack_Object_get(NULL, i);

        trackQueue = HeapTrack_Instance_State_trackQueue(obj);

        /* Get the head elem */
        elem = Queue_head(trackQueue);

        /* Loop until we get back to the Queue Object*/
        while (elem != (Queue_Elem *)(trackQueue)) {

            /*
             *  The start of the struct is up above the scribble.  Subtract
             *  sizeof(UArg) from the Queue_Elem address to get the start of
             *  the structure, since scribble is a UArg.  Do not use
             *  sizeof(HeapTrack_STARTSCRIBBLE), since xdctools generates
             *  a #define for this, which on 64-bit targets has a size of 4.
             */
            tracker = (HeapTrack_Tracker *)((Char *)elem - sizeof(UArg));

            if (tracker->taskHandle == task) {
                continueFlag = HeapTrack_printTrack(tracker, obj);
                if (continueFlag == FALSE) {
                    break;
                }
            }
            elem = Queue_next(elem);
        }
    }

    /*
     *  Loop through all the dynamically created heaps to find the
     *  allocated buffers that are owned by the task and print out
     *  the details about the buffer.
     */
    for (obj = HeapTrack_Object_first(); obj != NULL;
         obj = HeapTrack_Object_next(obj)) {
        trackQueue = HeapTrack_Instance_State_trackQueue(obj);

        /* Get the head elem */
        elem = Queue_head(trackQueue);

        /* Loop until we get back to the Queue Object */
        while (elem != (Queue_Elem *)(trackQueue)) {

            /*
             *  The start of the struct is up above the scribble.  Subtract
             *  sizeof(UArg) from the Queue_Elem address to get the start of
             *  the structure, since scribble is a UArg.  Do not use
             *  sizeof(HeapTrack_STARTSCRIBBLE), since xdctools generates
             *  a #define for this, which on 64-bit targets has a size of 4.
             */
            tracker = (HeapTrack_Tracker *)((Char *)elem - sizeof(UArg));

            if (tracker->taskHandle == task) {
                continueFlag = HeapTrack_printTrack(tracker, obj);
                if (continueFlag == FALSE) {
                    break;
                }
            }
            elem = Queue_next(elem);
        }
    }
}
