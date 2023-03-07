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
 *  ======== Semaphore.c ========
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/Types.h>

#if 0
#include <ti/utils/runtime/Log.h>
#else
#define Log_write(module, level, ...)
#endif

#define Semaphore_module ((Semaphore_Module_State *) &(Semaphore_Module_state))

const bool Semaphore_supportsEvents = Semaphore_supportsEvents_D;
const bool Semaphore_supportsPriority = Semaphore_supportsPriority_D;
const Semaphore_EventPost Semaphore_eventPost = Semaphore_eventPost_D;
const Semaphore_EventSync Semaphore_eventSync = Semaphore_eventSync_D;

Semaphore_Module_State Semaphore_Module_state = {
    .objQ.next = &Semaphore_Module_state.objQ,
    .objQ.prev = &Semaphore_Module_state.objQ
};

static const Semaphore_Params Semaphore_Params_default = {
    .event = NULL,
    .eventId = 0,
    .mode = Semaphore_Mode_COUNTING
};

/*
 *  Semaphore uses Clock, Task and Event APIs. Queue and Hwi are fully
 *  unconstrained. Clock uses Swi. Swi and Task APIs are safe before
 *  BIOS_start() except for the user hooks.  Clock and Event APIs are
 *  therefore also safe. No startup needed in Semaphore.
 */
/* REQ_TAG(SYSBIOS-500) */

/*
 *  ======== Semaphore_Instance_init ========
 */
/* REQ_TAG(SYSBIOS-501), REQ_TAG(SYSBIOS-502) */
void Semaphore_Instance_init(Semaphore_Object *sem, int count,
        const Semaphore_Params *params)
{
    Queue_Handle pendQ;
    unsigned int hwiKey;

    pendQ = &(sem->pendQ);

    sem->mode = params->mode;
    sem->count = (uint16_t)count;

    /* Make sure that supportsEvents is true if params->event is not null */
    Assert_isTrue((Semaphore_supportsEvents == true) ||
               ((Semaphore_supportsEvents == false) &&
                (params->event == NULL)), Semaphore_A_noEvents);

    Queue_construct(pendQ, NULL);

    if ((Semaphore_supportsEvents != false) && (params->event != NULL)) {

        sem->event = params->event;
        sem->eventId = params->eventId;

        hwiKey = Hwi_disable();
        if (count != 0) {
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

    /* put Sempahore object on global Sempahore Object list (Queue_put is atomic) */
    Queue_put(&Semaphore_module->objQ, &sem->objElem);
}

/*
 *  ======== Semaphore_Instance_finalize ========
 */
/* REQ_TAG(SYSBIOS-501) */
void Semaphore_Instance_finalize(Semaphore_Object *sem)
{
    Queue_Handle pendQ;

    pendQ = &sem->pendQ;
    Queue_destruct(Queue_struct(pendQ));

    if ((Semaphore_supportsEvents != false) && (sem->event != NULL)) {
        Semaphore_eventSync(sem->event, sem->eventId, 0);
    }
}

/*
 *  ======== Semaphore_pendTimeout ========
 *  called by Clock when timeout for a semaphore expires
 */
void Semaphore_pendTimeout(uintptr_t arg)
{
    unsigned int hwiKey;
    Semaphore_PendElem *elem = (Semaphore_PendElem *)Types_uargToPtr(arg);

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
        Task_unblockI(elem->tpElem.taskHandle, hwiKey);
    }

    Hwi_restore(hwiKey);
}

/*
 *  ======== Semaphore_pend ========
 */
/* REQ_TAG(SYSBIOS-504) */
bool Semaphore_pend(Semaphore_Object *sem, uint32_t timeout)
{
    unsigned int hwiKey, tskKey;
    Semaphore_PendElem elem;
    Queue_Handle pendQ;
    Clock_Struct clockStruct;

    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Semaphore_LM_pend, (intptr_t)sem, (uintptr_t)sem->count, (intptr_t)((int)timeout));

    /*
     *  Consider fast path check for count != 0 here!!!
     */

    /*
     *  elem is filled in entirely before interrupts are disabled.
     *  This significantly reduces latency.
     */

    /* init Clock object if timeout is not FOREVER nor NO_WAIT */
    if ((BIOS_clockEnabled != 0U)
            /* MISRA.ETYPE.INAPPR.OPERAND.UNOP.2012 */
            && (timeout != BIOS_WAIT_FOREVER)
            && (timeout != BIOS_NO_WAIT)) {
        Clock_initI(Clock_handle(&clockStruct), (Clock_FuncPtr)Semaphore_pendTimeout, timeout, (uintptr_t)&elem);
        elem.tpElem.clockHandle = Clock_handle(&clockStruct);
        elem.pendState = Semaphore_PendState_CLOCK_WAIT;
    }
    else {
        elem.tpElem.clockHandle = NULL;
        elem.pendState = Semaphore_PendState_WAIT_FOREVER;
    }

    pendQ = (Queue_Handle)&sem->pendQ;

    hwiKey = Hwi_disable();

    /* check semaphore count */
    if (sem->count == 0U) {

        if (timeout == BIOS_NO_WAIT) {
            Hwi_restore(hwiKey);
            return (false);
        }

        Assert_isTrue((BIOS_getThreadType() == BIOS_ThreadType_Task),
                        Semaphore_A_badContext);

        /*
         * Verify that THIS core hasn't already disabled the scheduler
         * so that the Task_restore() call below will indeed block
         */
        Assert_isTrue((Task_enabled() != false),
                        Semaphore_A_pendTaskDisabled);

        /* lock task scheduler */
        tskKey = Task_disable();

        /* get task handle and block tsk */
        elem.tpElem.taskHandle = Task_self();

        /* leave a pointer for Task_delete() */
        elem.tpElem.taskHandle->pendElem = &(elem.tpElem);

        Task_blockI(elem.tpElem.taskHandle);

        if ((Semaphore_supportsPriority != false) &&
           (((unsigned int)sem->mode & 0x2U) != 0U)) {    /* if PRIORITY bit is set */
            Semaphore_PendElem *tmpElem;
            Task_Handle tmpTask;
            int selfPri;

            tmpElem = Queue_head(pendQ);
            selfPri = Task_getPri(elem.tpElem.taskHandle);

            while (tmpElem != (Semaphore_PendElem *)pendQ) {
                tmpTask = tmpElem->tpElem.taskHandle;
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
        if ((BIOS_clockEnabled != false) &&
                (elem.pendState == Semaphore_PendState_CLOCK_WAIT)) {
            Clock_enqueueI(elem.tpElem.clockHandle);
            Clock_startI(elem.tpElem.clockHandle);
        }

        Hwi_restore(hwiKey);

        /* unlock task scheduler and block */
        Task_restore(tskKey);   /* the calling task will block here */

        /* Here on unblock due to Semaphore_post or timeout */

        hwiKey = Hwi_disable();

        if ((Semaphore_supportsEvents != false) && (sem->event != NULL)) {
            /* synchronize Event state */
            Semaphore_eventSync(sem->event, sem->eventId, sem->count);
        }

        /* remove Clock object from Clock Q */
        if ((BIOS_clockEnabled != false) && (elem.tpElem.clockHandle != NULL)) {
            Clock_removeI(elem.tpElem.clockHandle);
            elem.tpElem.clockHandle = NULL;
        }

        elem.tpElem.taskHandle->pendElem = NULL;

        Hwi_restore(hwiKey);

        return ((bool)(elem.pendState));
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

        if ((Semaphore_supportsEvents != false) && (sem->event != NULL)) {
            /* synchronize Event state */
            Semaphore_eventSync(sem->event, sem->eventId, sem->count);
        }

        Hwi_restore(hwiKey);

        return (true);
    }
}

/*
 *  ======== Semaphore_post ========
 */
/* REQ_TAG(SYSBIOS-503) */
void Semaphore_post(Semaphore_Object *sem)
{
    unsigned int tskKey, hwiKey;
    Semaphore_PendElem *elem;
    Queue_Handle pendQ;

    /* Event_post will do a Log_write, should we do one here too? */
    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Semaphore_LM_post, (uintptr_t)sem, (uintptr_t)sem->count);

    pendQ = &sem->pendQ;

    /* lock task scheduler */
    tskKey = Task_disable();

    hwiKey = Hwi_disable();

    if (Queue_empty(pendQ) != false) {
        if (((unsigned int)sem->mode & 0x1U) != 0U) {   /* if BINARY bit is set */
            sem->count = 1;
        }
        else {
            sem->count = sem->count + 1U;
            Assert_isTrue((sem->count != 0U), Semaphore_A_overflow);
        }

        Hwi_restore(hwiKey);

        if ((Semaphore_supportsEvents != false) && (sem->event != NULL)) {
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
    if ((BIOS_clockEnabled != false) && (elem->tpElem.clockHandle != NULL)) {
        Clock_stop(elem->tpElem.clockHandle);
    }

    /* put task back into readyQ */
    Task_unblockI(elem->tpElem.taskHandle, hwiKey);

    Hwi_restore(hwiKey);

    Task_restore(tskKey);
}

/*
 *  ======== Semaphore_getCount ========
 */
/* REQ_TAG(SYSBIOS-508) */
int Semaphore_getCount(Semaphore_Object *sem)
{
    return ((int)sem->count);
}

/*
 *  ======== Semaphore_getSemaphoreFromObjElem ========
 */
Semaphore_Handle Semaphore_getSemaphoreFromObjElem(Queue_Elem *semQelem)
{
    if (semQelem == (Queue_Elem *)&Semaphore_module->objQ) {
        return (NULL);
    }

    return ((Semaphore_Handle)((char *)semQelem -
             offsetof(Semaphore_Struct, objElem)));
}

/*
 *  ======== Semaphore_reset ========
 */
void Semaphore_reset(Semaphore_Object *sem, int count)
{
    unsigned int hwiKey;

    hwiKey = Hwi_disable();

    sem->count = (uint16_t)count;

    if ((Semaphore_supportsEvents != false) && (sem->event != NULL)) {
        /* synchronize Event state */
        Semaphore_eventSync(sem->event, sem->eventId, sem->count);
    }

    Hwi_restore(hwiKey);
}

/*
 *  ======== Semaphore_registerEvent ========
 */
void Semaphore_registerEvent(Semaphore_Object *sem, Event_Handle event, unsigned int eventId)
{
    if (Semaphore_supportsEvents != false) {
        unsigned int hwiKey = Hwi_disable();
        sem->event = event;
        sem->eventId = eventId;
        Hwi_restore(hwiKey);
    }
}

/*
 *  ======== Semaphore_testStaticInlines ========
 */
void Semaphore_testStaticInlines()
{
    Semaphore_Params semParams;

    Semaphore_Params_init(NULL);
    Semaphore_Params_init(&semParams);
}

/* -------- The following functions were generated in BIOS 6.x -------- */

/*
 *  ======== Semaphore_construct ========
 */
Semaphore_Handle Semaphore_construct(Semaphore_Object *sem, int count,
        const Semaphore_Params *params)
{
    if (params == NULL) {
        params = &Semaphore_Params_default;
    }

    Semaphore_Instance_init(sem, count, params);

    return (sem);
}

/*
 *  ======== Semaphore_create ========
 */
Semaphore_Handle Semaphore_create(int count, const Semaphore_Params *params, Error_Block *eb)
{
    Semaphore_Handle sem;

    sem = Memory_alloc(NULL, sizeof(Semaphore_Object), 0, eb);

    if (sem != NULL) {
        sem = Semaphore_construct(sem, count, params);
    }

    return (sem);
}

/*
 *  ======== Semaphore_destruct ========
 */
void Semaphore_destruct(Semaphore_Object *sem)
{
    unsigned int hwiKey;

    Semaphore_Instance_finalize(sem);

    /* remove Semaphore object from global Semaphore object list (Queue_remove is not atomic) */
    hwiKey = Hwi_disable();
    Queue_remove(&sem->objElem);
    Hwi_restore(hwiKey);
}

/*
 *  ======== Semaphore_delete ========
 */
void Semaphore_delete(Semaphore_Handle *sem)
{
    Semaphore_destruct(*sem);

    Memory_free(NULL, *sem, sizeof(Semaphore_Object));

    *sem = NULL;
}

/*
 *  ======== Semaphore_Object_first ========
 */
Semaphore_Handle Semaphore_Object_first()
{
    return (Semaphore_getSemaphoreFromObjElem(
                Queue_head(&(Semaphore_module->objQ))));
}

/*
 *  ======== Semaphore_Object_next ========
 */
Semaphore_Handle Semaphore_Object_next(Semaphore_Handle sem)
{
    return (Semaphore_getSemaphoreFromObjElem(Queue_next(&sem->objElem)));
}

/*
 *  ======== Semaphore_Params_init ========
 */
void Semaphore_Params_init(Semaphore_Params *params)
{
    *params = Semaphore_Params_default;
}
