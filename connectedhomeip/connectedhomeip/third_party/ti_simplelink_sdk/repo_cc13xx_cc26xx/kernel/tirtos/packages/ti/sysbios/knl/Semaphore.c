/*
 * Copyright (c) 2013-2019, Texas Instruments Incorporated
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
 *  ======== Semaphore.c ========
 *  Implementation of functions specified in Semaphore.xdc.
 */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Swi.h>
#define ti_sysbios_knl_Task__internalaccess
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include "package/internal/Semaphore.xdc.h"

/*
 *  Semaphore uses Clock, Task and Event APIs. Queue and Hwi are fully
 *  unconstrained. Clock uses Swi. Swi and Task APIs are safe before
 *  BIOS_start() except for the user hooks.  Clock and Event APIs are
 *  therefore also safe. No startup needed in Semaphore.
 */

/*
 *  ======== Semaphore_Instance_init ========
 */
Void Semaphore_Instance_init(Semaphore_Object *sem, Int count,
        const Semaphore_Params *params)
{
    Queue_Handle pendQ;
    UInt hwiKey;

    pendQ = Semaphore_Instance_State_pendQ(sem);

    sem->mode = params->mode;
    sem->count = (UInt16)count;

    /* Make sure that supportsEvents is TRUE if params->event is not null */
    Assert_isTrue((Semaphore_supportsEvents == TRUE) ||
               ((Semaphore_supportsEvents == FALSE) &&
                (params->event == NULL)), Semaphore_A_noEvents);


    Queue_construct(Queue_struct(pendQ), NULL);

    if ((Semaphore_supportsEvents != FALSE) && (params->event != NULL)) {

        sem->event = params->event;
        sem->eventId = params->eventId;

        hwiKey = Hwi_disable();
        if (count) {
            /*
             *  In the unlikely case that a task is already
             *  pending on the event object waiting for this
             *  Semaphore...
             */
            Hwi_restore(hwiKey);
            Semaphore_eventPost(sem->event, sem->eventId);
        }
        else {
            Semaphore_eventSync(sem->event, sem->eventId, 0);
            Hwi_restore(hwiKey);
        }
    }
    else {
        sem->event = NULL;
        sem->eventId = 1;
    }
}

/*
 *  ======== Semaphore_Instance_finalize ========
 */
Void Semaphore_Instance_finalize(Semaphore_Object *sem)
{
    Queue_Handle pendQ;

    pendQ = Semaphore_Instance_State_pendQ(sem);
    Queue_destruct(Queue_struct(pendQ));

    if ((Semaphore_supportsEvents != FALSE) && (sem->event != NULL)) {
        Semaphore_eventSync(sem->event, sem->eventId, 0);
    }
}

/*
 *  ======== Semaphore_pendTimeout ========
 *  called by Clock when timeout for a semaphore expires
 */
Void Semaphore_pendTimeout(UArg arg)
{
    UInt hwiKey;
    Semaphore_PendElem *elem = (Semaphore_PendElem *)xdc_uargToPtr(arg);

    hwiKey = Hwi_disable();

    /* Verify that Semaphore_post() hasn't already occurred */

    if (elem->pendState == Semaphore_PendState_CLOCK_WAIT) {

        /* remove task's qElem from semaphore queue */
        Queue_remove(&(elem->tpElem.qElem));

        elem->pendState = Semaphore_PendState_TIMEOUT;

        /*
         *  put task back into readyQ
         *  No need for Task_disable/restore sandwich since this
         *  is called within Swi (or Hwi) thread
         */
        Task_unblockI(elem->tpElem.task, hwiKey);
    }

    Hwi_restore(hwiKey);
}

/*
 *  ======== Semaphore_pend ========
 */
Bool Semaphore_pend(Semaphore_Object *sem, UInt32 timeout)
{
    UInt hwiKey, tskKey;
    Semaphore_PendElem elem;
    Queue_Handle pendQ;
    Clock_Struct clockStruct;

    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write3(Semaphore_LM_pend, (IArg)sem, (UArg)sem->count, (IArg)((Int)timeout));

    /*
     *  Consider fast path check for count != 0 here!!!
     */

    /*
     *  elem is filled in entirely before interrupts are disabled.
     *  This significantly reduces latency.
     */

    /* add Clock event if timeout is not FOREVER nor NO_WAIT */
    if ((BIOS_clockEnabled != 0U)
            /* MISRA.ETYPE.INAPPR.OPERAND.UNOP.2012 */
            && (timeout != BIOS_WAIT_FOREVER)
            && (timeout != BIOS_NO_WAIT)) {
        Clock_addI(Clock_handle(&clockStruct), (Clock_FuncPtr)Semaphore_pendTimeout, timeout, (UArg)&elem);
        elem.tpElem.clock = Clock_handle(&clockStruct);
        elem.pendState = Semaphore_PendState_CLOCK_WAIT;
    }
    else {
        elem.tpElem.clock = NULL;
        elem.pendState = Semaphore_PendState_WAIT_FOREVER;
    }

    pendQ = Semaphore_Instance_State_pendQ(sem);

    hwiKey = Hwi_disable();

    /* check semaphore count */
    if (sem->count == 0U) {

        if (timeout == BIOS_NO_WAIT) {
            Hwi_restore(hwiKey);
            return (FALSE);
        }

        Assert_isTrue((BIOS_getThreadType() == BIOS_ThreadType_Task),
                        Semaphore_A_badContext);

        /*
         * Verify that THIS core hasn't already disabled the scheduler
         * so that the Task_restore() call below will indeed block
         */
        Assert_isTrue((Task_enabled() != FALSE),
                        Semaphore_A_pendTaskDisabled);

        /* lock task scheduler */
        tskKey = Task_disable();

        /* get task handle and block tsk */
        elem.tpElem.task = Task_self();

        /* leave a pointer for Task_delete() */
        elem.tpElem.task->pendElem = (Task_PendElem *)&(elem);

        Task_blockI(elem.tpElem.task);

        if ((Semaphore_supportsPriority != FALSE) &&
           (((UInt)sem->mode & 0x2U) != 0U)) {    /* if PRIORITY bit is set */
            Semaphore_PendElem *tmpElem;
            Task_Handle tmpTask;
            Int selfPri;

            tmpElem = Queue_head(pendQ);
            selfPri = Task_getPri(elem.tpElem.task);

            while (tmpElem != (Semaphore_PendElem *)pendQ) {
                tmpTask = tmpElem->tpElem.task;
                /* use '>' here so tasks wait FIFO for same priority */
                if (selfPri > Task_getPri(tmpTask)) {
                    break;
                }
                else {
                    tmpElem = Queue_next(&(tmpElem->tpElem.qElem));
                }
            }

            Queue_insert(&(tmpElem->tpElem.qElem),
                    (Queue_Elem *)&(elem.tpElem.qElem));
        }
        else {
            /* put task at the end of the pendQ */
            Queue_enqueue(pendQ, &(elem.tpElem.qElem));
        }

        /* start Clock if appropriate */
        if ((BIOS_clockEnabled != FALSE) &&
                (elem.pendState == Semaphore_PendState_CLOCK_WAIT)) {
            Clock_startI(elem.tpElem.clock);
        }

        Hwi_restore(hwiKey);

        /* unlock task scheduler and block */
        Task_restore(tskKey);   /* the calling task will block here */

        /* Here on unblock due to Semaphore_post or timeout */

        hwiKey = Hwi_disable();

        if ((Semaphore_supportsEvents != FALSE) && (sem->event != NULL)) {
            /* synchronize Event state */
            Semaphore_eventSync(sem->event, sem->eventId, sem->count);
        }

        /* remove Clock object from Clock Q */
        if (BIOS_clockEnabled && (elem.tpElem.clock != NULL)) {
            Clock_removeI(elem.tpElem.clock);
            elem.tpElem.clock = NULL;
        }
        
        elem.tpElem.task->pendElem = NULL;

        Hwi_restore(hwiKey);

        return ((Bool)(elem.pendState));
    }
    else {
        /*
         * Assert catches Semaphore_pend calls from Hwi and Swi
         * with non-zero timeout.
         */
        Assert_isTrue((timeout == BIOS_NO_WAIT) ||
                ((BIOS_getThreadType() == BIOS_ThreadType_Task) ||
                (BIOS_getThreadType() == BIOS_ThreadType_Main)),
                Semaphore_A_badContext);

        sem->count = sem->count - 1U;

        if ((Semaphore_supportsEvents != FALSE) && (sem->event != NULL)) {
            /* synchronize Event state */
            Semaphore_eventSync(sem->event, sem->eventId, sem->count);
        }

        /* remove Clock object from Clock Q */
        if (BIOS_clockEnabled && (elem.tpElem.clock != NULL)) {
            Clock_removeI(elem.tpElem.clock);
            elem.tpElem.clock = NULL;
        }

        Hwi_restore(hwiKey);

        return (TRUE);
    }
}

/*
 *  ======== Semaphore_post ========
 */
Void Semaphore_post(Semaphore_Object *sem)
{
    UInt tskKey, hwiKey;
    Semaphore_PendElem *elem;
    Queue_Handle pendQ;

    /* Event_post will do a Log_write, should we do one here too? */
    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write2(Semaphore_LM_post, (UArg)sem, (UArg)sem->count);

    pendQ = Semaphore_Instance_State_pendQ(sem);

    /* lock task scheduler */
    tskKey = Task_disable();

    hwiKey = Hwi_disable();

    if (Queue_empty(pendQ)) {
        if (((UInt)sem->mode & 0x1U) != 0U) {   /* if BINARY bit is set */
            sem->count = 1;
        }
        else {
            sem->count = sem->count + 1U;
            Assert_isTrue((sem->count != 0U), Semaphore_A_overflow);
        }

        Hwi_restore(hwiKey);

        if ((Semaphore_supportsEvents != FALSE) && (sem->event != NULL)) {
            Semaphore_eventPost(sem->event, sem->eventId);
        }
        /* unlock/restore task scheduler */
        Task_restore(tskKey);

        return;
    }

    
    /* dequeue tsk from semaphore queue */
    elem = (Semaphore_PendElem *)Queue_dequeue(pendQ);

    /* mark the Semaphore as having been posted */
    elem->pendState = Semaphore_PendState_POSTED;

    /* disable Clock object */
    if (BIOS_clockEnabled && (elem->tpElem.clock != NULL)) {
        Clock_stop(elem->tpElem.clock);
    }

    /* put task back into readyQ */
    Task_unblockI(elem->tpElem.task, hwiKey);

    Hwi_restore(hwiKey);

    Task_restore(tskKey);
}

/*
 *  ======== Semaphore_getCount ========
 */
Int Semaphore_getCount(Semaphore_Object *sem)
{
    return ((Int)sem->count);
}

/*
 *  ======== Semaphore_reset ========
 */
void Semaphore_reset(Semaphore_Object *sem, Int count)
{
    UInt hwiKey;

    hwiKey = Hwi_disable();

    sem->count = (UInt16)count;

    if ((Semaphore_supportsEvents != FALSE) && (sem->event != NULL)) {
        /* synchronize Event state */
        Semaphore_eventSync(sem->event, sem->eventId, sem->count);
    }

    Hwi_restore(hwiKey);
}

/*
 *  ======== Semaphore_registerEvent ========
 */
void Semaphore_registerEvent(Semaphore_Object *sem, Event_Handle event, UInt eventId)
{
    if (Semaphore_supportsEvents) {
        UInt hwiKey = Hwi_disable();
        sem->event = event;
        sem->eventId = eventId;
        Hwi_restore(hwiKey);
    }
}
