/*
 * Copyright (c) 2014-2020, Texas Instruments Incorporated
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
 *  ======== Clock.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/hal/Hwi.h>

#include "package/internal/Clock.xdc.h"

#ifdef ti_sysbios_knl_Clock_TICK_SOURCE
#define CLOCK_TICK_SOURCE ti_sysbios_knl_Clock_TICK_SOURCE
#else
#define CLOCK_TICK_SOURCE (Clock_tickSource)
#endif

#ifdef ti_sysbios_knl_Clock_TICK_MODE
#define CLOCK_TICK_MODE ti_sysbios_knl_Clock_TICK_MODE
#else
#define CLOCK_TICK_MODE (Clock_tickMode)
#endif

#ifndef ti_sysbios_knl_Clock_stopCheckNext__D
#define ti_sysbios_knl_Clock_stopCheckNext__D (FALSE)
#endif

/* MODULE LEVEL FUNCTIONS */

/*
 *  ======== Clock_Module_startup ========
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
Int Clock_Module_startup(Int phase)
{
    Int rv;

    /*
     * Clock uses Swi and Timer. Swi APIs safe before
     * BIOS_start() except for hooks. Needs to wait for Timer.
     */
    if (Clock_TimerProxy_Module_startupDone() == FALSE) {
        rv = Startup_NOTDONE;
    }
    else {
        if ((Clock_TickSource)CLOCK_TICK_SOURCE == Clock_TickSource_TIMER) {
            /* get the max ticks that can be skipped by the timer */
            Clock_module->maxSkippable =
                    Clock_TimerProxy_getMaxTicks(Clock_module->timer);
        }

        rv = Startup_DONE;
    }

    return (rv);
}

/*
 *  ======== Clock_getCompletedTicks  ========
 */
UInt32 Clock_getCompletedTicks(Void)
{
    return (((ti_sysbios_knl_Clock_Module_State *)Clock_module)->ticks);
}

/*
 *  ======== Clock_getTickPeriod  ========
 */
UInt32 Clock_getTickPeriod(Void)
{
    return (Clock_TimerProxy_getPeriod(Clock_module->timer));
}

/*
 *  ======== Clock_getTicks ========
 */
UInt32 Clock_getTicks(Void)
{
    UInt32 rv;

    if ((Clock_TickMode)CLOCK_TICK_MODE == Clock_TickMode_DYNAMIC) {
        UInt32 ticks;
        UInt hwiKey;

        hwiKey = Hwi_disable();

        /* do not ask Timer to save NOW */
        ticks = Clock_TimerProxy_getCurrentTick(Clock_module->timer, FALSE);

        Hwi_restore(hwiKey);

        rv = ticks;
    }
    else {
        rv = Clock_module->ticks;
    }

    return (rv);
}

/*
 *  ======== Clock_setTicks  ========
 */
Void Clock_setTicks(UInt32 ticks)
{
    Clock_module->ticks = ticks;
}

/*
 *  ======== Clock_ticksUntilInterrupt  ========
 */
UInt32 Clock_getTicksUntilInterrupt(Void)
{
    UInt32 ticks;

    if ((Clock_TickSource)CLOCK_TICK_SOURCE == Clock_TickSource_TIMER) {

        UInt32 current;
        UInt key;

        key = Hwi_disable();

        /* do not ask Timer to save NOW */
        current = Clock_TimerProxy_getCurrentTick(Clock_module->timer, FALSE);

        ticks = Clock_module->nextScheduledTick - current;

        /* clamp value to zero if nextScheduledTick is less than current */
        if (ticks > Clock_module->maxSkippable) {
            ticks = (UInt32)0;
        }

        Hwi_restore(key);

    }
    else if ((Clock_TickSource)CLOCK_TICK_SOURCE == Clock_TickSource_NULL) {
        ticks = 0xFFFFFFFFU;
    }
    else {
        ticks = (UInt32)0;
    }

    return (ticks);
}

/*
 *  ======== Clock_getTicksUntilTimeout  ========
 */
UInt32 Clock_getTicksUntilTimeout(Void)
{
    return (Clock_walkQueuePeriodic());
}

/*
 *  ======== Clock_getTimerHandle  ========
 */
Clock_TimerProxy_Handle Clock_getTimerHandle(Void)
{
    return (Clock_module->timer);
}

/*
 *  ======== Clock_scheduleNextTick  ========
 *  Must be called with global interrupts disabled!
 */
/* LCOV_EXCL_START */
Void Clock_scheduleNextTick(UInt32 deltaTicks, UInt32 absTick)
{
    /* now reprogram the timer for the new period and next interrupt */
    Clock_TimerProxy_setNextTick(Clock_module->timer, deltaTicks);

    /* remember this */
    Clock_module->numTickSkip = (UInt)deltaTicks;
    Clock_module->nextScheduledTick = absTick;
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Clock_tickStop  ========
 */
Void Clock_tickStop(Void)
{
    if ((Clock_TickSource)CLOCK_TICK_SOURCE == Clock_TickSource_TIMER) {
        if ((Clock_TickMode)CLOCK_TICK_MODE != Clock_TickMode_DYNAMIC) {
            Clock_TimerProxy_stop(Clock_module->timer);
        }
    }
}

/*
 *  ======== Clock_tickReconfig  ========
 */
Bool Clock_tickReconfig(Void)
{
    Bool rv;

    if (((Clock_TickSource)CLOCK_TICK_SOURCE == Clock_TickSource_TIMER) &&
        ((Clock_TickMode)CLOCK_TICK_MODE != Clock_TickMode_DYNAMIC)) {
        rv = Clock_TimerProxy_setPeriodMicroSecs(Clock_module->timer,
                 Clock_tickPeriod);
    }
    else {
       rv = (Bool)FALSE;
    }

    return (rv);
}

/*
 *  ======== Clock_tickStart  ========
 */
Void Clock_tickStart(Void)
{
    if ((Clock_TickSource)CLOCK_TICK_SOURCE == Clock_TickSource_TIMER) {
        if ((Clock_TickMode)CLOCK_TICK_MODE != Clock_TickMode_DYNAMIC) {
            Clock_TimerProxy_start(Clock_module->timer);
        }
    }
}

/*
 *  ======== Clock_walkQueuePeriodic ========
 *  Walk the Clock Queue for TickMode_PERIODIC to determine the number of
 *  ticks until the next active timeout
 */
UInt32 Clock_walkQueuePeriodic(Void)
{
    UInt32 distance = ~(0U);
    Queue_Handle clockQ;
    Queue_Elem  *elem;
    Queue_Elem  *clockQElem;
    Clock_Object *obj;
    UInt32 thisTick;
    UInt32 delta;

    clockQ = Clock_Module_State_clockQ();
    elem = (Queue_Elem *)Queue_head(clockQ);
    clockQElem = Queue_prev(elem);
    thisTick = Clock_module->ticks;

    /* traverse clock queue */
    while (elem != clockQElem) {

        /* MISRA.CAST.OBJ_PTR_TO_OBJ_PTR.2012 */
        obj = (Clock_Object *)elem;
        elem = Queue_next(elem);

        /* if  the object is active ... */
        if (obj->active == TRUE) {

            delta = obj->currTimeout - thisTick;

            /* if this is the soonest tick update distance to soonest */
            if (delta < distance) {
                distance = delta;
            }
        }
    }

    return (distance);
}

/*
 *  ======== Clock_workFunc ========
 *  Service Clock Queue for TickMode_PERIODIC
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
Void Clock_workFunc(UArg arg0, UArg arg1)
{
    Queue_Elem  *elem;
    UInt hwiKey, count;
    UInt32 ticks, compare;
    Clock_Object *obj;
    Queue_Handle clockQ;
    Queue_Elem  *clockQElem;

    hwiKey = Hwi_disable();

    ticks = Clock_module->ticks;
    count = Clock_module->swiCount;
    Clock_module->swiCount = 0;

    Hwi_restore(hwiKey);

    /* Log when count > 1, meaning Clock_swi is delayed */
    if (count > 1U) {
        /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
        Log_write1(Clock_LW_delayed, (UArg)count);
    }

    compare = ticks - count;

    /*
     * Here count can be zero. When Clock_tick() runs it increments
     * swiCount and posts the Clock_workFunc. In Clock_workFunc we
     * get the value of swiCount atomically. Before we read swiCount, an
     * interrupt could occur, Clock_tick() will post the swi again.
     * That post is unnecessary as we are getting ready to process that
     * tick. The next time this swi runs the count will be zero.
     */

    while (count) {

        compare = compare + 1U;
        count = count - 1U;

        /* Traverse clock queue */

        clockQ = Clock_Module_State_clockQ();
        elem = Queue_head(clockQ);
        clockQElem = Queue_prev(elem);

        while (elem != clockQElem) {
            /* MISRA.CAST.OBJ_PTR_TO_OBJ_PTR.2012 */
            obj = (Clock_Object *)elem;
            elem = Queue_next(elem);
            /* if event has timed out */
            if ((obj->active == TRUE) && (obj->currTimeout == compare)) {

                if (obj->period == 0U) { /* oneshot? */
                    /* mark object idle */
                    obj->active = FALSE;
                }
                else {                  /* periodic */
                    /* refresh timeout */
                    obj->currTimeout += obj->period;
                }

                /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 MISRA.CAST.FUNC_PTR.2012 */
                Log_write2(Clock_LM_begin, (UArg)obj, (UArg)obj->fxn);

                /* call handler */
                obj->fxn(obj->arg);
             }
         }
     }
}

/*
 *  ======== Clock_walkQueueDynamic ========
 *  Walk the Clock Queue for TickMode_DYNAMIC, optionally servicing a
 *  specific tick
 */
/* LCOV_EXCL_START */
UInt32 Clock_walkQueueDynamic(Bool service, UInt32 thisTick)
{
    UInt32 distance = ~(0U);
    Queue_Handle clockQ;
    Queue_Elem  *elem;
    Queue_Elem  *clockQElem;
    Clock_Object *obj;
    UInt32 delta;

    /* Traverse clock queue */
    clockQ = Clock_Module_State_clockQ();
    elem = Queue_head(clockQ);
    clockQElem = Queue_prev(elem);

    while (elem != clockQElem) {

        /* MISRA.CAST.OBJ_PTR_TO_OBJ_PTR.2012 */
        obj = (Clock_Object *)elem;
        elem = Queue_next(elem);

        /* if  the object is active ... */
        if (obj->active == TRUE) {

            /* optionally service if tick matches timeout */
            if (service == TRUE) {

                /* if this object is timing out update its state */
                if (obj->currTimeout == thisTick) {

                    if (obj->period == 0U) { /* oneshot? */
                        /* mark object idle */
                        obj->active = FALSE;
                    }
                    else {                      /* periodic */
                        /* refresh timeout */
                        obj->currTimeout += obj->period;
                    }

                    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 MISRA.CAST.FUNC_PTR.2012 */
                    Log_write2(Clock_LM_begin, (UArg)obj, (UArg)obj->fxn);

                    /* call handler */
                    obj->fxn(obj->arg);
                }
            }

            /* if object still active update distance to soonest tick */
            if (obj->active == TRUE) {

                delta = obj->currTimeout - thisTick;

                /* if this is the soonest tick update distance to soonest */
                if (delta < distance) {
                    distance = delta;
                }

            }
        }
    }

    return (distance);
}

/*
 *  ======== Clock_workFuncDynamic ========
 *  Service Clock Queue for TickMode_DYNAMIC
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
Void Clock_workFuncDynamic(UArg arg0, UArg arg1)
{
    UInt32 distance;
    UInt32 serviceTick, serviceDelta;
    UInt32 ticksToService;
    UInt32 skippable;
    UInt32 nowTick, nowDelta, nextTick;
    UInt hwiKey;

    hwiKey = Hwi_disable();

    /* get current tick count, signal Timer to save corresponding NOW info */
    nowTick = Clock_TimerProxy_getCurrentTick(Clock_module->timer, TRUE);

    /* set flags while actively servicing queue */
    Clock_module->inWorkFunc = TRUE;
    Clock_module->startDuringWorkFunc = FALSE;

    /* determine first tick expiration to service (the anticipated next tick) */
    serviceTick = Clock_module->nextScheduledTick;
    ticksToService = nowTick - serviceTick;

    /*
     * if now hasn't caught up to nextScheduledTick,
     * a spurious interrupt has probably occurred.
     * ignore for now...
     */

    serviceDelta = serviceTick - Clock_module->ticks;
    nowDelta = nowTick - Clock_module->ticks;
    if (serviceDelta > nowDelta) {
        Clock_module->inWorkFunc = FALSE;
        Hwi_restore(hwiKey);
        /* MISRA.RETURN.NOT_LAST */
        return;
    }

    Hwi_restore(hwiKey);

    distance = 0;

    /* walk queue until catch up to current tick count */
    while (ticksToService >= distance) {
        serviceTick = serviceTick + distance;
        ticksToService -= distance;
        distance = Clock_walkQueueDynamic(TRUE, serviceTick);
    }

    /* now determine next needed tick and setup timer for that tick ... */
    hwiKey = Hwi_disable();

    /* if Clock_start() during processing of Q, re-walk to update distance */
    if (Clock_module->startDuringWorkFunc == TRUE) {
        /* UNREACH.GEN */
        distance = Clock_walkQueueDynamic(FALSE, serviceTick);
    }

    /* if no active timeouts then skip the maximum supported by the timer */
    if (distance == ~(0U)) {
        skippable = Clock_module->maxSkippable;
        nextTick = serviceTick + skippable;
    }
    /* else, finalize how many ticks can skip */
    else {
        skippable = distance - ticksToService;
        if (skippable > Clock_module->maxSkippable) {
            skippable = Clock_module->maxSkippable;
        }
        nextTick = serviceTick + skippable;
    }

    /* reprogram timer for next expected tick */
    Clock_scheduleNextTick(skippable, nextTick);

    Clock_module->ticking = TRUE;
    Clock_module->inWorkFunc = FALSE;
    Clock_module->ticks = serviceTick;

    Hwi_restore(hwiKey);
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Clock_tick ========
 *  interrupt handler called by user interrupt when
 *  CLOCK_TICK_SOURCE = USER or NULL
 */
Void Clock_tick(Void)
{
    Clock_doTickFunc(0);
}

/* INSTANCE LEVEL FUNCTIONS */

/*
 *  ======== Clock_Instance_init ========
 */
Void Clock_Instance_init(Clock_Object *obj, Clock_FuncPtr func, UInt timeout,
    const Clock_Params *params)
{
    Queue_Handle clockQ;

    Assert_isTrue((BIOS_clockEnabled == TRUE), Clock_A_clockDisabled);

    Assert_isTrue(((BIOS_getThreadType() != BIOS_ThreadType_Hwi) &&
                   (BIOS_getThreadType() != BIOS_ThreadType_Swi)),
                        Clock_A_badThreadType);

    Assert_isTrue(!((params->startFlag != FALSE) && (timeout == 0U)),
            (Assert_Id)0U);

    obj->timeout = (UInt32)timeout;
    obj->period = params->period;
    obj->fxn = func;
    obj->arg = params->arg;
    obj->active = FALSE;

    /*
     * Clock object is always placed on Clock work Q
     */
    clockQ = Clock_Module_State_clockQ();
    Queue_put(clockQ, &obj->elem);

    if (params->startFlag) {
        Clock_start(obj);
    }
}

/*
 *  ======== Clock_Instance_finalize ========
 */
Void Clock_Instance_finalize(Clock_Object *obj)
{
    UInt key;

    Assert_isTrue(((BIOS_getThreadType() != BIOS_ThreadType_Hwi) &&
                   (BIOS_getThreadType() != BIOS_ThreadType_Swi)),
                        Clock_A_badThreadType);

    key = Hwi_disable();

    Queue_remove(&obj->elem);

    Hwi_restore(key);
}

/*
 *  ======== Clock_addI ========
 */
Void Clock_addI(Clock_Object *obj, Clock_FuncPtr func, UInt32 timeout, UArg arg)
{
    Queue_Handle clockQ;

    obj->timeout = timeout;
    obj->period = 0;
    obj->fxn = func;
    obj->arg = arg;
    obj->active = FALSE;

    /*
     * Clock object is always placed on Clock work Q
     */
    clockQ = Clock_Module_State_clockQ();
    Queue_put(clockQ, &obj->elem);
}

/*
 *  ======== Clock_removeI ========
 */
Void Clock_removeI(Clock_Object *obj)
{
    Queue_remove(&obj->elem);
}

/*
 *  ======== Clock_startI ========
 *  Set the Clock object's currTimeout value and set its active flag
 *  to TRUE.
 */
Void Clock_startI(Clock_Object *obj)
{
    if ((Clock_TickMode)CLOCK_TICK_MODE == Clock_TickMode_DYNAMIC) {
        UInt32 nowTick, nowDelta;
        UInt32 scheduledTick, scheduledDelta;
        UInt32 remainingTicks;
        Bool objectServiced = FALSE;

        /* now see if need this new timeout before next scheduled tick ... */
        /* wait till after first tick */
        if ((Clock_module->ticking == TRUE) &&
             /* if Clock is NOT currently processing its Q */
             (Clock_module->inWorkFunc == FALSE)) {

            /*
             * get virtual current tick count,
             * signal Timer to save corresponding NOW info
             */
            nowTick = Clock_TimerProxy_getCurrentTick(Clock_module->timer, TRUE);

            nowDelta = nowTick - Clock_module->ticks;
            scheduledDelta = Clock_module->nextScheduledTick -
                Clock_module->ticks;

            if (nowDelta <= scheduledDelta) {

                objectServiced = TRUE;

                /* start new Clock object */
                obj->currTimeout = nowTick + obj->timeout;
                obj->active = TRUE;

                /* get the next scheduled tick */
                scheduledTick = Clock_module->nextScheduledTick;

                /* how many ticks until scheduled tick? */
                remainingTicks = scheduledTick - nowTick;

                if (obj->timeout < remainingTicks) {
                    Clock_scheduleNextTick(obj->timeout,
                       obj->currTimeout);
                }
            }
        }

        if (objectServiced == FALSE) {
            /*
             * get virtual current tick count,
             * DO NOT (!) signal Timer to save corresponding NOW info
             */
            nowTick = Clock_getTicks();

            /* start new Clock object */
            obj->currTimeout = nowTick + obj->timeout;
            obj->active = TRUE;

            if (Clock_module->inWorkFunc == TRUE) {
                Clock_module->startDuringWorkFunc = TRUE;
            }
        }
    }
    /* CLOCK_TICK_MODE == Clock_TickMode_PERIODIC */
    else {
        obj->currTimeout = (Clock_module->ticks + obj->timeout);
        obj->active = TRUE;
    }
}

/*
 *  ======== Clock_start ========
 */
Void Clock_start(Clock_Object *obj)
{
    UInt key;

    Assert_isTrue(obj->timeout != 0U, (Assert_Id)0U);

    key = Hwi_disable();

    Clock_startI(obj);

    Hwi_restore(key);
}

/*
 *  ======== Clock_stop ========
 *  remove and clear Clock object's queue elem from clockQ
 */
Void Clock_stop(Clock_Object *obj)
{
    obj->active = FALSE;
#if ti_sysbios_knl_Clock_stopCheckNext__D==TRUE
    if (obj->currTimeout == Clock_module->nextScheduledTick) {
        Clock_start(Clock_triggerClock);
    }
#endif
}

/*
 *  ======== Clock_setFunc ========
 */
Void Clock_setFunc(Clock_Object *obj, Clock_FuncPtr fxn, UArg arg)
{
    UInt key;

    key = Hwi_disable();

    obj->fxn = fxn;
    obj->arg = arg;

    Hwi_restore(key);
}

/*
 *  ======== Clock_setPeriod ========
 */
Void Clock_setPeriod(Clock_Object *obj, UInt32 period)
{
    obj->period = period;
}

/*
 *  ======== Clock_setTimeout ========
 */
Void Clock_setTimeout(Clock_Object *obj, UInt32 timeout)
{
    obj->timeout = timeout;
}

/*
 *  ======== Clock_getPeriod ========
 */
UInt32 Clock_getPeriod(Clock_Object *obj)
{
    return (obj->period);
}

/*
 *  ======== Clock_getTimeout ========
 */
UInt32 Clock_getTimeout(Clock_Object *obj)
{
    UInt32 timeout = 0;
    UInt   key;

    key = Hwi_disable();
    if (obj->active == TRUE) {
        timeout = obj->currTimeout - Clock_getTicks();

        /*
         *  Check for current Clock tick count surpassing the
         *  Clock object's current timeout.
         */
        if ((timeout > obj->timeout) && (timeout > obj->period)) {
            timeout = 0;
        }
    }
    Hwi_restore(key);

    return(timeout);
}

/*
 *  ======== Clock_logTick ========
 */
Void Clock_logTick(Void)
{
    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write1(Clock_LM_tick, (UArg)Clock_module->ticks);
}

/*
 *  ======== Clock_isActive ========
 */
Bool Clock_isActive(Clock_Object *obj)
{
    return (obj->active);
}

/*
 *  ======== Clock_triggerFunc ========
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
Void Clock_triggerFunc(UArg arg)
{
}
