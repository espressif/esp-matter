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
 *  ======== Event.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/Types.h>

#if 0
#include <ti/utils/runtime/Log.h>
#else
#define Log_write(module, level, ...)
#endif

Event_Module_State Event_Module_state = {
    .objQ.next = &Event_Module_state.objQ,
    .objQ.prev = &Event_Module_state.objQ
};

static const Event_Params Event_Params_default;

/*
 *  Event uses Clock and Task. Queue and Hwi are fully
 *  unconstrained. Clock uses Swi. Swi and Task APIs are safe before
 *  BIOS_start() except for the user hooks.  Clock APIs are
 *  therefore also safe. No startup checks needed in Event.
 */

/*
 *  ======== Event_Instance_init ========
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
void Event_Instance_init(Event_Object *event, const Event_Params *params)
{
    Queue_Handle pendQ;

    event->postedEvents = 0U;

    pendQ = &event->pendQ;
    Queue_construct(Queue_struct(pendQ), NULL);

    /* put Event object on global Event Object list (Queue_put is atomic) */
    Queue_put(&Event_module->objQ, &event->objElem);
}

/*
 *  ======== Event_pendTimeout ========
 *  called by Clock when timeout for a event expires
 */
void Event_pendTimeout(uintptr_t arg)
{
    unsigned int hwiKey;
    Event_PendElem *elem = (Event_PendElem *)Types_uargToPtr(arg);

    hwiKey = Hwi_disable();

    /*
     *  Verify that Event_post() hasn't already serviced this qElem.
     */
    if (elem->pendState == Event_PendState_CLOCK_WAIT) {

        /* remove eventElem from event_Elem queue */
        Queue_remove(&(elem->tpElem.qElem));

        elem->pendState = Event_PendState_TIMEOUT;

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
 *  ======== Event_checkEvents ========
 *  Checks postedEvents for matching event conditions.
 *  Returns matchingEvents if a match and consumes matched events,
 *  else returns 0 and consumes nothing.
 *  Called with ints disabled
 */
unsigned int Event_checkEvents (Event_Object *event, unsigned int andMask, unsigned int orMask)
{
    unsigned int matchingEvents;

    matchingEvents = orMask & event->postedEvents;

    if ((andMask & event->postedEvents) == andMask) {
        matchingEvents |= andMask;
    }

    if (matchingEvents != (unsigned int)0) {
        /* consume ALL the matching events */
        event->postedEvents &= ~matchingEvents;
    }

    return (matchingEvents);
}

/*
 *  ======== Event_getEventFromObjElem ========
 */
Event_Handle Event_getEventFromObjElem(Queue_Elem *eventQelem)
{
    if (eventQelem == (Queue_Elem *)&Event_module->objQ) {
        return (NULL);
    }

    return ((Event_Handle)((char *)eventQelem -
                offsetof(Event_Struct, objElem)));
}


/*
 *  ======== Event_pend ========
 */
unsigned int Event_pend(Event_Object *event, unsigned int andMask, unsigned int orMask, uint32_t timeout)
{
    unsigned int hwiKey, tskKey;
    Event_PendElem elem;
    unsigned int matchingEvents;
    Queue_Handle pendQ;
    Clock_Struct clockStruct;

    Assert_isTrue(((andMask | orMask) != 0U), Event_A_nullEventMasks);

    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Event_LM_pend, (uintptr_t)event, (uintptr_t)event->postedEvents,
                (uintptr_t)andMask, (uintptr_t)orMask, (intptr_t)((int)timeout));

    /*
     * elem is filled in entirely before interrupts are disabled.
     * This significantly reduces latency at the potential cost of wasted time
     * if it turns out that there is already an event match.
     */

    /* init Clock object if timeout is not FOREVER nor NO_WAIT */
    if ((BIOS_clockEnabled != false)
            && (timeout != BIOS_WAIT_FOREVER)
            && (timeout != BIOS_NO_WAIT)) {
        Clock_initI(Clock_handle(&clockStruct), (Clock_FuncPtr)Event_pendTimeout, timeout, (uintptr_t)&elem);
        elem.tpElem.clockHandle = Clock_handle(&clockStruct);
        elem.pendState = Event_PendState_CLOCK_WAIT;
    }
    else {
        elem.tpElem.clockHandle = NULL;
        elem.pendState = Event_PendState_WAIT_FOREVER;
    }

    /* fill in this task's Event_PendElem */
    elem.andMask = andMask;
    elem.orMask = orMask;
    elem.matchingEvents = 0U;

    pendQ = &event->pendQ;

    /* get task handle */
    elem.tpElem.taskHandle = Task_self();

    /* Atomically check for a match and block if none */
    hwiKey = Hwi_disable();

    /* check if events are already available */
    matchingEvents = Event_checkEvents(event, andMask, orMask);

    if (matchingEvents != 0U) {
        Hwi_restore(hwiKey);
        return (matchingEvents);/* yes, then return with matching bits */
    }

    if (timeout == BIOS_NO_WAIT) {
        Hwi_restore(hwiKey);
        return (0);             /* No match, no wait */
    }

    Assert_isTrue((BIOS_getThreadType() == BIOS_ThreadType_Task),
                        Event_A_badContext);

    /*
     * Verify that THIS core hasn't already disabled the scheduler
     * so that the Task_restore() call below will indeed block
     */
    Assert_isTrue(Task_enabled() != false, Event_A_pendTaskDisabled);

    /* lock scheduler */
    tskKey = Task_disable();

    /* only one Task allowed!!! */
    Assert_isTrue(Queue_empty(pendQ) != false, Event_A_eventInUse);

    /* leave a pointer for Task_delete() */
    elem.tpElem.taskHandle->pendElem = (Task_PendElem *)&(elem.tpElem);

    /* add it to Event_PendElem queue */
    Queue_enqueue(pendQ, &(elem.tpElem.qElem));

    Task_blockI(elem.tpElem.taskHandle);

    if ((BIOS_clockEnabled != false) &&
            (elem.tpElem.clockHandle != (Clock_Handle)NULL)) {
        Clock_enqueueI(elem.tpElem.clockHandle);
        Clock_startI(elem.tpElem.clockHandle);
    }

    Hwi_restore(hwiKey);

    /* unlock task scheduler and block */
    Task_restore(tskKey);       /* the calling task will switch out here */

    /* Here on unblock due to Event_post or Event_pendTimeout */

    hwiKey = Hwi_disable();

    /* remove Clock object from Clock Q */
    if ((BIOS_clockEnabled != false) && (elem.tpElem.clockHandle != (Clock_Handle)NULL)) {
        Clock_removeI(elem.tpElem.clockHandle);
        elem.tpElem.clockHandle = NULL;
    }

    elem.tpElem.taskHandle->pendElem = NULL;

    Hwi_restore(hwiKey);

    /* event match? */
    if (elem.pendState != Event_PendState_TIMEOUT) {
        return (elem.matchingEvents);
    }
    else {
        return (0);             /* timeout */
    }
}

/*
 *  ======== Event_post ========
 */
void Event_post(Event_Object *event, unsigned int eventId)
{
    unsigned int tskKey, hwiKey;
    Event_PendElem *elem;
    Queue_Handle pendQ;

    Assert_isTrue((eventId != 0U), Event_A_nullEventId);

    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Event_LM_post, (uintptr_t)event, (uintptr_t)event->postedEvents, (uintptr_t)eventId);

    pendQ = &event->pendQ;

    /* atomically post this event */
    hwiKey = Hwi_disable();

    /* or in this eventId */
    event->postedEvents |= eventId;

    /* confirm that ANY tasks are pending on this event */
    if (Queue_empty(pendQ) == true) {
        Hwi_restore(hwiKey);
        return;
    }

    tskKey = Task_disable();

    /* examine pendElem on pendQ */
    elem = (Event_PendElem *)Queue_head(pendQ);

    /* check for match, consume matching eventIds if so. */
    elem->matchingEvents = Event_checkEvents(event, elem->andMask, elem->orMask);

    if (elem->matchingEvents != 0U) {

        /* remove event elem from elem queue */
        Queue_remove(&(elem->tpElem.qElem));

        /* mark the Event as having been posted */
        elem->pendState = Event_PendState_POSTED;

        /* disable Clock object */
        if ((BIOS_clockEnabled != false) && (elem->tpElem.clockHandle != NULL)) {
            Clock_stop(elem->tpElem.clockHandle);
        }

        /* put task back into readyQ */
        Task_unblockI(elem->tpElem.taskHandle, hwiKey);
    }

    Hwi_restore(hwiKey);

    /* context switch may occur here */
    Task_restore(tskKey);
}

/*
 *  ======== Event_sync ========
 *  Must be called with interrupts disabled!
 */
void Event_sync(Event_Object *event, unsigned int eventId, unsigned int count)
{
    if (count != false) {
        event->postedEvents |= eventId;
    }
    else {
        event->postedEvents &= ~eventId;
    }
}

/*
 *  ======== Event_getPostedEvents ========
 */
unsigned int Event_getPostedEvents(Event_Object *event)
{
    return (event->postedEvents);
}

/* -------- The following functions were generated in BIOS 6.x -------- */

/*
 *  ======== Event_Object_first ========
 */
Event_Handle Event_Object_first()
{
    return (Event_getEventFromObjElem(Queue_head(&(Event_module->objQ))));
}

/*
 *  ======== Event_Object_next ========
 */
Event_Handle Event_Object_next(Event_Handle event)
{
    return (Event_getEventFromObjElem(Queue_next(&event->objElem)));
}

/*
 *  ======== Event_Params_init ========
 */
void Event_Params_init(Event_Params *params)
{
    *params = Event_Params_default;
}

/*
 *  ======== Event_construct ========
 */
Event_Handle Event_construct(Event_Object *event, const Event_Params *params)
{
    if (params == NULL) {
        params = &Event_Params_default;
    }

    Event_Instance_init(event, params);

    return (event);
}

/*
 *  ======== Event_create ========
 */
Event_Handle Event_create(const Event_Params *params, Error_Block *eb)
{
    Event_Handle event;

    event = Memory_alloc(NULL, sizeof(Event_Object), 0, eb);

    if (event != NULL) {
        event = Event_construct(event, params);
    }

    return (event);
}

/*
 *  ======== Event_destruct ========
 */
void Event_destruct(Event_Handle event)
{
    unsigned int hwiKey;

    /* remove Event object from global Event object list (Queue_remove is not atomic) */
    hwiKey = Hwi_disable();
    Queue_remove(&event->objElem);
    Hwi_restore(hwiKey);
}

/*
 *  ======== Event_delete ========
 */
void Event_delete(Event_Handle *event)
{
    Event_destruct(*event);

    Memory_free(NULL, *event, sizeof(Event_Object));

    *event = NULL;
}

