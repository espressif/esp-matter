/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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
 *  ======== GateMutexPri.c ========
 */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Swi.h>
#define ti_sysbios_knl_Task__internalaccess
#include <ti/sysbios/knl/Task.h>

#include "package/internal/GateMutexPri.xdc.h"

/* Constant used to denote first level of nesting */
#define FIRST_ENTER   0
#define NESTED_ENTER  1

/*
 * Inheritd IGateProvider. Part of runtime. Called in first pass.
 * APIs ready because Task_disable() and Task_restore() do no work
 * before BIOS_start().
 * Note that initially mutex is available
 */

/*
 *  ======== GateMutexPri_Instance_init ========
 */
Void GateMutexPri_Instance_init(GateMutexPri_Object *obj,
                                 const GateMutexPri_Params *params)
{
    Queue_Handle pendQ;

    pendQ = GateMutexPri_Instance_State_pendQ(obj);

    obj->mutexCnt = 1;
    obj->owner = NULL;
    obj->ownerOrigPri = 0;
    Queue_construct(Queue_struct(pendQ), NULL);
}

/*
 *  ======== GateMutexPri_Instance_finalize ========
 */
Void GateMutexPri_Instance_finalize(GateMutexPri_Object *obj)
{
    Queue_Handle pendQ;

    pendQ = GateMutexPri_Instance_State_pendQ(obj);
    Queue_destruct(Queue_struct(pendQ));
}

/*
 *  ======== GateMutexPri_Gate ========
 *  Returns FIRST_ENTER when it gets the gate, returns NESTED_ENTER
 *  on nested calls.
 */
IArg GateMutexPri_enter(GateMutexPri_Object *obj)
{
    Task_Handle tsk;
    UInt tskKey;
    Int tskPri;
    Task_PendElem elem;
    Queue_Handle pendQ;

    /* make sure we're not calling from Hwi or Swi context */
    Assert_isTrue(((BIOS_getThreadType() == BIOS_ThreadType_Task) ||
                   (BIOS_getThreadType() == BIOS_ThreadType_Main)),
                   GateMutexPri_A_badContext);

    pendQ = GateMutexPri_Instance_State_pendQ(obj);

    tsk = Task_self();

    /*
     * Prior to tasks starting, Task_self() will return NULL.
     * Simply return NESTED_ENTER here as, by definition, there is
     * is only one thread running at this time.
     */
    if (tsk == NULL) {
        return (NESTED_ENTER);
    }

    tskPri = Task_getPri(tsk);

    /*
     * Gate may only be called from task context, so Task_disable is sufficient
     * protection.
     */
    tskKey = Task_disable();

    /* If the gate is free, take it. */
    if (obj->mutexCnt == 1) {
        obj->mutexCnt = 0;
        obj->owner = tsk;
        obj->ownerOrigPri = tskPri;

        Task_restore(tskKey);
        return (FIRST_ENTER);
    }

    /* At this point, the gate is already held by someone. */

    /* If this is a nested call to gate... */
    if (obj->owner == tsk) {
        Task_restore(tskKey);
        return (NESTED_ENTER);
    }

    /*
     * Verify that THIS core hasn't already disabled the scheduler
     * so that the Task_restore() call below will indeed block
     */
    if (BIOS_swiEnabled == TRUE) {
        Assert_isTrue(((tskKey == 0) && Swi_enabled()),
                     GateMutexPri_A_enterTaskDisabled);
    }
    else {
        Assert_isTrue((tskKey == 0),
                     GateMutexPri_A_enterTaskDisabled);
    }

    /* Remove tsk from ready list. */
    Task_block(tsk);

    elem.task = tsk;
    elem.clock = NULL;
    /* leave a pointer for Task_delete() */
    tsk->pendElem = &elem;

    /* Insert tsk in wait queue in order by priority (high pri at head) */
    GateMutexPri_insertPri(pendQ, (Queue_Elem *)&elem, tskPri);

    /*
     * Donate priority if necessary. The owner is guaranteed to have the
     * highest priority of anyone waiting on the gate, so just compare this
     * task's priority against the owner's.
     */
    if (tskPri > Task_getPri(obj->owner)) {
        Task_setPri(obj->owner, tskPri);
    }

    /* Task_restore will call the scheduler and this task will block. */
    Task_restore(tskKey);

    tsk->pendElem = NULL;

    /*
     * At this point, tsk has the gate. Initialization of the gate is handled
     * by the previous owner's call to leave.
     */
    return (FIRST_ENTER);
}

/*
 *  ======== GateMutexPri_leave ========
 *  Only releases the gate if key == FIRST_ENTER.
 */
Void GateMutexPri_leave(GateMutexPri_Object *obj, IArg key)
{
    UInt tskKey, hwiKey;
    Task_Handle owner;
    Task_Handle newOwner;
    Task_PendElem *elem;
    Queue_Handle pendQ;

    pendQ = GateMutexPri_Instance_State_pendQ(obj);

    owner = Task_self();

    /*
     * Prior to tasks starting, Task_self() will return NULL.
     * Simply return here as, by definition, there is
     * is only one thread running at this time.
     */
    if (owner == NULL) {
        return;
    }

    /*
     * Gate may only be called from task context, so Task_disable is sufficient
     * protection.
     */
    tskKey = Task_disable();

    /* Assert that caller is gate owner. */
    /* ASSERT(owner == obj->owner); */

    /* If this is not the outermost call to leave, just return. */
    if (key != FIRST_ENTER) {
        Task_restore(tskKey);
        return;
    }

    /*
     * Restore this task's priority. The if-test is worthwhile because of the
     * cost of a call to setPri.
     */
    if (obj->ownerOrigPri < Task_getPri(owner)) {
        Task_setPri(owner, obj->ownerOrigPri);
    }

    /* If the list of waiting tasks is not empty... */
    if (!Queue_empty(pendQ)) {

        /*
         * Get the next owner from the front of the queue (the task with the
         * highest priority of those waiting on the queue).
         */
        elem = (Task_PendElem *)Queue_dequeue(pendQ);
        newOwner = elem->task;

        /* Setup the gate. */
        obj->owner = newOwner;
        obj->ownerOrigPri = Task_getPri(newOwner);

        /* Task_unblockI must be called with interrupts disabled. */
        hwiKey = Hwi_disable();
        Task_unblockI(newOwner, hwiKey);
        Hwi_restore(hwiKey);

    }
    /* If the gate is to be posted... */
    else {
        obj->mutexCnt = 1;
    }

    Task_restore(tskKey);
}

/*
 *  ======== GateMutexPri_query ========
 *
 */
Bool GateMutexPri_query(Int qual)
{
    return (TRUE);
}

/*
 *  ======== GateMutexPri_insertPri ========
 *  Inserts the element in order by priority, with higher priority
 *  elements at the head of the queue.
 */
Void GateMutexPri_insertPri(Queue_Object* queue, Queue_Elem* newElem, Int newPri)
{
    Queue_Elem* qelem;

    /* Iterate over the queue. */
    for (qelem = Queue_head(queue); qelem != (Queue_Elem *)queue;
         qelem = Queue_next(qelem)) {
        /* Tasks of equal priority will be FIFO, so '>', not '>='. */
        if (newPri > Task_getPri(((Task_PendElem *)qelem)->task)) {
            /* Place the new element in front of the current qelem. */
            Queue_insert(qelem, newElem);
            return;
        }
    }

    /*
     * Put the task at the back of the queue if:
     *   1. The queue was empty.
     *   2. The task had the lowest priority in the queue.
     */
    Queue_enqueue(queue, newElem);
}



