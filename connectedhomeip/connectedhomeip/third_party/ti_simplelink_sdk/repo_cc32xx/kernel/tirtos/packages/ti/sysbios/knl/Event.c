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
 *  ======== Event.c ========
 *  Implementation of functions specified in Event.xdc.
 */

#include <xdc/std.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Assert.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Swi.h>
#define ti_sysbios_knl_Task__internalaccess
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include "package/internal/Event.xdc.h"

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
Void Event_Instance_init(Event_Object *event, const Event_Params *params)
{
    Queue_Handle pendQ;

    event->postedEvents = 0U;

    pendQ = Event_Instance_State_pendQ(event);
    Queue_construct(Queue_struct(pendQ), NULL);
}

/*
 *  ======== Event_pendTimeout ========
 *  called by Clock when timeout for a event expires
 */
Void Event_pendTimeout(UArg arg)
{
    UInt hwiKey;
    Event_PendElem *elem = (Event_PendElem *)xdc_uargToPtr(arg);

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
        Task_unblockI(elem->tpElem.task, hwiKey);
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
UInt Event_checkEvents (Event_Object *event, UInt andMask, UInt orMask)
{
    UInt matchingEvents;

    matchingEvents = orMask & event->postedEvents;

    if ((andMask & event->postedEvents) == andMask) {
        matchingEvents |= andMask;
    }

    if (matchingEvents != (UInt)0) {
        /* consume ALL the matching events */
        event->postedEvents &= ~matchingEvents;
    }

    return (matchingEvents);
}

/*
 *  ======== Event_pend ========
 */
UInt Event_pend(Event_Object *event, UInt andMask, UInt orMask, UInt32 timeout)
{
    UInt hwiKey, tskKey;
    Event_PendElem elem;
    UInt matchingEvents;
    Queue_Handle pendQ;
    Clock_Struct clockStruct;

    Assert_isTrue(((andMask | orMask) != 0U), Event_A_nullEventMasks);

    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write5(Event_LM_pend, (UArg)event, (UArg)event->postedEvents,
                (UArg)andMask, (UArg)orMask, (IArg)((Int)timeout));

    /*
     * elem is filled in entirely before interrupts are disabled.
     * This significantly reduces latency at the potential cost of wasted time
     * if it turns out that there is already an event match.
     */

    /* add Clock event if timeout is not FOREVER nor NO_WAIT */
    if ((BIOS_clockEnabled != FALSE)
            && (timeout != BIOS_WAIT_FOREVER)
            && (timeout != BIOS_NO_WAIT)) {
        Clock_addI(Clock_handle(&clockStruct), (Clock_FuncPtr)Event_pendTimeout, timeout, (UArg)&elem);
        elem.tpElem.clock = Clock_handle(&clockStruct);
        elem.pendState = Event_PendState_CLOCK_WAIT;
    }
    else {
        elem.tpElem.clock = NULL;
        elem.pendState = Event_PendState_WAIT_FOREVER;
    }

    /* fill in this task's Event_PendElem */
    elem.andMask = andMask;
    elem.orMask = orMask;
    elem.matchingEvents = 0U;

    pendQ = Event_Instance_State_pendQ(event);

    /* get task handle */
    elem.tpElem.task = Task_self();

    /* Atomically check for a match and block if none */
    hwiKey = Hwi_disable();

    /* check if events are already available */
    matchingEvents = Event_checkEvents(event, andMask, orMask);

    if (matchingEvents != 0U) {
        /* remove Clock object from Clock Q */
        if (BIOS_clockEnabled && (elem.tpElem.clock != NULL)) {
            Clock_removeI(elem.tpElem.clock);
            elem.tpElem.clock = NULL;
        }

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
    Assert_isTrue(Task_enabled() != FALSE, Event_A_pendTaskDisabled);

    /* lock scheduler */
    tskKey = Task_disable();

    /* only one Task allowed!!! */
    Assert_isTrue(Queue_empty(pendQ) != FALSE, Event_A_eventInUse);

    /* leave a pointer for Task_delete() */
    elem.tpElem.task->pendElem = (Task_PendElem *)&(elem);

    /* add it to Event_PendElem queue */
    Queue_enqueue(pendQ, &(elem.tpElem.qElem));

    Task_blockI(elem.tpElem.task);

    if ((BIOS_clockEnabled != FALSE) &&
            (elem.tpElem.clock != NULL) &&
            (elem.pendState == Event_PendState_CLOCK_WAIT)) {
        Clock_startI(elem.tpElem.clock);
    }

    Hwi_restore(hwiKey);

    /* unlock task scheduler and block */
    Task_restore(tskKey);       /* the calling task will switch out here */

    /* Here on unblock due to Event_post or Event_pendTimeout */

    hwiKey = Hwi_disable();

    /* remove Clock object from Clock Q */
    if (BIOS_clockEnabled && (elem.tpElem.clock != NULL)) {
        Clock_removeI(elem.tpElem.clock);
        elem.tpElem.clock = NULL;
    }
        
    elem.tpElem.task->pendElem = NULL;

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
Void Event_post(Event_Object *event, UInt eventId)
{
    UInt tskKey, hwiKey;
    Event_PendElem *elem;
    Queue_Handle pendQ;

    Assert_isTrue((eventId != 0U), Event_A_nullEventId);

    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write3(Event_LM_post, (UArg)event, (UArg)event->postedEvents, (UArg)eventId);

    pendQ = Event_Instance_State_pendQ(event);

    /* atomically post this event */
    hwiKey = Hwi_disable();

    /* or in this eventId */
    event->postedEvents |= eventId;

    /* confirm that ANY tasks are pending on this event */
    if (Queue_empty(pendQ) == TRUE) {
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
        if (BIOS_clockEnabled && (elem->tpElem.clock != NULL)) {
            Clock_stop(elem->tpElem.clock);
        }

        /* put task back into readyQ */
        Task_unblockI(elem->tpElem.task, hwiKey);
    }

    Hwi_restore(hwiKey);

    /* context switch may occur here */
    Task_restore(tskKey);
}

/*
 *  ======== Event_sync ========
 *  Must be called with interrupts disabled!
 */
Void Event_sync(Event_Object *event, UInt eventId, UInt count)
{
    if (count != FALSE) {
        event->postedEvents |= eventId;
    }
    else {
        event->postedEvents &= ~eventId;
    }
}

/*
 *  ======== Event_getPostedEvents ========
 */
UInt Event_getPostedEvents(Event_Object *event)
{
    return (event->postedEvents);
}



