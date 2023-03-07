/*
 * Copyright (c) 2014-2020 Texas Instruments Incorporated - http://www.ti.com
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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Startup.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/ClockSupport.h>

#ifndef ti_sysbios_knl_Clock_stopCheckNext__D
#define ti_sysbios_knl_Clock_stopCheckNext__D (false)
#endif

static const Clock_Params Clock_Params_default = {
    .startFlag = 0,
    .period = 0,
    .arg = 0
};

Clock_Module_State Clock_Module_state = {
    .ticks = 0,
    .swi = NULL,
    .swiCount = 0,
    .numTickSkip = 1,
    .nextScheduledTick = 1,
    .maxSkippable = 0,
    .inWorkFunc = false,
    .ticking = false,
    .startDuringWorkFunc = false,
    .tickMode = Clock_tickMode_D,
    .tickPeriod = Clock_tickPeriod_D,
    .tickSource = Clock_tickSource_D,
    .initDone = 0
};

const unsigned int Clock_swiPriority = Clock_swiPriority_D;
const Clock_TickMode Clock_tickMode = Clock_tickMode_D;
const uint32_t Clock_tickPeriod = Clock_tickPeriod_D;
const Clock_TickSource Clock_tickSource = Clock_tickSource_D;
const Clock_Handle Clock_triggerClock;

#if 0
/*
 *  ======== instrumentation ========
 *  Must define these macros before including Log.h
 */
#undef Log_moduleName
#define Log_moduleName ti_sysbios_knl_Clock

#ifdef Clock_log_D
#define ti_utils_runtime_Log_ENABLE_PPO
#ifdef Clock_logInfo1_D
#define ti_utils_runtime_Log_ENABLE_INFO1
#endif
#ifdef Clock_logInfo2_D
#define ti_utils_runtime_Log_ENABLE_INFO2
#endif
#ifdef Clock_logInfo3_D
#define ti_utils_runtime_Log_ENABLE_INFO3
#endif
#endif

#include <ti/utils/runtime/Log.h>

Log_EVENT(Clock_tickLogEvt, Log_Type_Data, "tick: %d");
Log_EVENT(Clock_funcLogEvt, Log_Type_Data, "clk: 0x%x, func: 0x%x");
Log_EVENT(Clock_delayedLogEvt, Log_Type_Data, "delayed: %d tick(s)");

#else
#define Log_write(module, level, ...)
#endif

/* MODULE LEVEL FUNCTIONS */
/* REQ_TAG(SYSBIOS-519) */

/*
 *  ======== Clock_init ========
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
/* REQ_TAG(SYSBIOS-520), REQ_TAG(SYSBIOS-523) */
void Clock_init(void)
{
    /* Interrupts are disabled at this point */
    if (Clock_module->initDone) {
        return;
    }
    Clock_module->initDone = true;

    Queue_construct(&Clock_module->clockQ, NULL);

    if (BIOS_clockEnabled && BIOS_swiEnabled) {
        Swi_Params swiParams;
        Swi_init();
        Swi_Params_init(&swiParams);
        swiParams.priority = Clock_swiPriority;
        if (Clock_tickMode == Clock_TickMode_DYNAMIC) {
            Clock_module->swi = Swi_construct(&Clock_module->swiStruct,
                Clock_workFuncDynamic, &swiParams, Error_IGNORE);
        }
        else {
            Clock_module->swi = Swi_construct(&Clock_module->swiStruct,
                Clock_workFunc, &swiParams, Error_IGNORE);
        }
    }

    /*
     * Clock uses Swi and Timer. Swi APIs safe before
     * BIOS_start() except for hooks. Needs to wait for Timer.
     * ClockSupport_init() will initialize the appropriate timer.
     */
    if (Clock_tickSource == Clock_TickSource_TIMER) {
        ClockSupport_init();

        /* get the max ticks that can be skipped by the timer */
        Clock_module->maxSkippable = ClockSupport_getMaxTicks();
    }
}

/*
 *  ======== Clock_getCompletedTicks  ========
 */
uint32_t Clock_getCompletedTicks(void)
{
    return (Clock_module->ticks);
}

/*
 *  ======== Clock_getTickPeriod  ========
 */
uint32_t Clock_getTickPeriod(void)
{
    return (ClockSupport_getPeriod());
}

/*
 *  ======== Clock_getTicks ========
 */
uint32_t Clock_getTicks(void)
{
    uint32_t rv;

    if (Clock_tickMode == Clock_TickMode_DYNAMIC) {
        uint32_t ticks;
        unsigned int hwiKey;

        hwiKey = Hwi_disable();

        /* do not ask Timer to save NOW */
        ticks = ClockSupport_getCurrentTick(false);

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
void Clock_setTicks(uint32_t ticks)
{
    Clock_module->ticks = ticks;
}

/*
 *  ======== Clock_ticksUntilInterrupt  ========
 */
uint32_t Clock_getTicksUntilInterrupt(void)
{
    uint32_t ticks;

    if (Clock_tickSource == Clock_TickSource_TIMER) {

        uint32_t current;
        unsigned int key;

        key = Hwi_disable();

        /* do not ask Timer to save NOW */
        current = ClockSupport_getCurrentTick(false);

        ticks = Clock_module->nextScheduledTick - current;

        /* clamp value to zero if nextScheduledTick is less than current */
        if (ticks > Clock_module->maxSkippable) {
            ticks = (uint32_t)0;
        }

        Hwi_restore(key);

    }
    else if (Clock_tickSource == Clock_TickSource_NULL) {
        ticks = 0xFFFFFFFFU;
    }
    else {
        ticks = (uint32_t)0;
    }

    return (ticks);
}

/*
 *  ======== Clock_getTicksUntilTimeout  ========
 */
uint32_t Clock_getTicksUntilTimeout(void)
{
    return (Clock_walkQueuePeriodic());
}

/*
 *  ======== Clock_getTimerHandle  ========
 */
void * Clock_getTimerHandle(void)
{
//    return (Clock_module->timer);
    return (NULL);
}

/*
 *  ======== Clock_scheduleNextTick  ========
 *  Must be called with global interrupts disabled!
 */
/* LCOV_EXCL_START */
void Clock_scheduleNextTick(uint32_t deltaTicks, uint32_t absTick)
{
    /* now reprogram the timer for the new period and next interrupt */
    ClockSupport_setNextTick(deltaTicks);

    /* remember this */
    Clock_module->numTickSkip = (unsigned int)deltaTicks;
    Clock_module->nextScheduledTick = absTick;
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Clock_tickStop  ========
 */
void Clock_tickStop(void)
{
    if (Clock_tickSource == Clock_TickSource_TIMER) {
        if (Clock_tickMode != Clock_TickMode_DYNAMIC) {
            ClockSupport_stop();
        }
    }
}

/*
 *  ======== Clock_tickReconfig  ========
 */
/* REQ_TAG(SYSBIOS-522) */
bool Clock_tickReconfig(void)
{
    bool rv;

    if ((Clock_tickSource == Clock_TickSource_TIMER) &&
        (Clock_tickMode != Clock_TickMode_DYNAMIC)) {
        rv = ClockSupport_setPeriodMicroSecs(Clock_tickPeriod);
    }
    else {
       rv = (bool)false;
    }

    return (rv);
}

/*
 *  ======== Clock_tickStart  ========
 */
void Clock_tickStart(void)
{
    if (Clock_tickSource == Clock_TickSource_TIMER) {
        if (Clock_tickMode != Clock_TickMode_DYNAMIC) {
            ClockSupport_start();
        }
    }
}

/*
 *  ======== Clock_walkQueuePeriodic ========
 *  Walk the Clock Queue for TickMode_PERIODIC to determine the number of
 *  ticks until the next active timeout
 */
uint32_t Clock_walkQueuePeriodic(void)
{
    uint32_t distance = ~(0U);
    Queue_Handle clockQ;
    Queue_Elem  *elem;
    Queue_Elem  *clockQElem;
    Clock_Object *obj;
    uint32_t thisTick;
    uint32_t delta;

    clockQ = &Clock_module->clockQ;
    elem = (Queue_Elem *)Queue_head(clockQ);
    clockQElem = Queue_prev(elem);
    thisTick = Clock_module->ticks;

    /* traverse clock queue */
    while (elem != clockQElem) {

        /* MISRA.CAST.OBJ_PTR_TO_OBJ_PTR.2012 */
        obj = (Clock_Object *)elem;
        elem = Queue_next(elem);

        /* if  the object is active ... */
        if (obj->active == true) {

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
/* REQ_TAG(SYSBIOS-525) */
void Clock_workFunc(uintptr_t arg0, uintptr_t arg1)
{
    Queue_Elem  *elem;
    unsigned int hwiKey, count;
    uint32_t ticks, compare;
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
        Log_write(Log_INFO3, Clock_delayedLogEvt, count);
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

    while (count != 0U) {

        compare = compare + 1U;
        count = count - 1U;

        /* Traverse clock queue */

        clockQ = &Clock_module->clockQ;
        elem = Queue_head(clockQ);
        clockQElem = Queue_prev(elem);

        while (elem != clockQElem) {
            /* MISRA.CAST.OBJ_PTR_TO_OBJ_PTR.2012 */
            obj = (Clock_Object *)elem;
            elem = Queue_next(elem);
            /* if event has timed out */
            if ((obj->active == true) && (obj->currTimeout == compare)) {

                if (obj->period == 0U) { /* oneshot? */
                    /* mark object idle */
                    obj->active = false;
                }
                else {                  /* periodic */
                    /* refresh timeout */
                    obj->currTimeout += obj->period;
                    obj->timeoutTicks = obj->period;
                }

                /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 MISRA.CAST.FUNC_PTR.2012 */
                Log_write(Log_INFO2, Clock_funcLogEvt, obj, obj->fxn);

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
uint32_t Clock_walkQueueDynamic(bool service, uint32_t thisTick)
{
    uint32_t distance = ~(0U);
    Queue_Handle clockQ;
    Queue_Elem  *elem;
    Queue_Elem  *clockQElem;
    Clock_Object *obj;
    uint32_t delta;

    /* Traverse clock queue */
    clockQ = &Clock_module->clockQ;
    elem = Queue_head(clockQ);
    clockQElem = Queue_prev(elem);

    while (elem != clockQElem) {

        /* MISRA.CAST.OBJ_PTR_TO_OBJ_PTR.2012 */
        obj = (Clock_Object *)elem;
        elem = Queue_next(elem);

        /* if  the object is active ... */
        if (obj->active == true) {

            /* optionally service if tick matches timeout */
            if (service == true) {

                /* if this object is timing out update its state */
                if (obj->currTimeout == thisTick) {

                    if (obj->period == 0U) { /* oneshot? */
                        /* mark object idle */
                        obj->active = false;
                    }
                    else {                      /* periodic */
                        /* refresh timeout */
                        obj->currTimeout += obj->period;
                        obj->timeoutTicks = obj->period;
                    }

                    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 MISRA.CAST.FUNC_PTR.2012 */
                    Log_write(Log_INFO2, Clock_funcLogEvt, obj, obj->fxn);

                    /* call handler */
                    obj->fxn(obj->arg);
                }
            }

            /* if object still active update distance to soonest tick */
            if (obj->active == true) {

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
void Clock_workFuncDynamic(uintptr_t arg0, uintptr_t arg1)
{
    uint32_t distance;
    uint32_t serviceTick, serviceDelta;
    uint32_t ticksToService;
    uint32_t skippable;
    uint32_t nowTick, nowDelta, nextTick;
    unsigned int hwiKey;

    hwiKey = Hwi_disable();

    /* get current tick count, signal Timer to save corresponding NOW info */
    nowTick = ClockSupport_getCurrentTick(true);

    /* set flags while actively servicing queue */
    Clock_module->inWorkFunc = true;
    Clock_module->startDuringWorkFunc = false;

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
        Clock_module->inWorkFunc = false;
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
        distance = Clock_walkQueueDynamic(true, serviceTick);
    }

    /* now determine next needed tick and setup timer for that tick ... */
    hwiKey = Hwi_disable();

    /* if Clock_start() during processing of Q, re-walk to update distance */
    if (Clock_module->startDuringWorkFunc == true) {
        /* UNREACH.GEN */
        distance = Clock_walkQueueDynamic(false, serviceTick);
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

    Clock_module->ticking = true;
    Clock_module->inWorkFunc = false;
    Clock_module->ticks = serviceTick;

    Hwi_restore(hwiKey);
}
/* LCOV_EXCL_STOP */

/*
 *  ======== Clock_tick ========
 */
/* REQ_TAG(SYSBIOS-531) */
void Clock_tick(void)
{
    if (Clock_tickMode == Clock_TickMode_PERIODIC) {
        /* update system time */
        Clock_module->ticks++;
    }

    /* MISRA.ETYPE.INAPPR.OPERAND.BINOP.2012 */
    Log_write(Log_INFO1, Clock_tickLogEvt, Clock_module->ticks);

    if (Clock_tickMode == Clock_TickMode_DYNAMIC) {
    }
    else {
        if (Queue_empty(&Clock_module->clockQ) == 0U) {
            Clock_module->swiCount++;
        }
    }

    if (BIOS_swiEnabled_D) {
        Swi_post(Clock_module->swi);
    }
    else {
        if (Clock_tickMode == Clock_TickMode_DYNAMIC) {
            Clock_workFuncDynamic(0U, 0U);
        }
        else {
            Clock_workFunc(0U, 0U);
        }
    }
}

/*
 *  ======== Clock_Instance_init ========
 */
/* REQ_TAG(SYSBIOS-521) */
void Clock_Instance_init(Clock_Object *obj, Clock_FuncPtr func, unsigned int timeout,
    const Clock_Params *params)
{
    Queue_Handle clockQ;

    Assert_isTrue((BIOS_clockEnabled == true), Clock_A_clockDisabled);

    Assert_isTrue(((BIOS_getThreadType() != BIOS_ThreadType_Hwi) &&
                   (BIOS_getThreadType() != BIOS_ThreadType_Swi)),
                        Clock_A_badThreadType);

    Assert_isTrue(!((params->startFlag != false) && (timeout == 0U)),
            Clock_A_badTimeout);

    obj->timeout = (uint32_t)timeout;
    obj->period = params->period;
    obj->fxn = func;
    obj->arg = params->arg;
    obj->active = false;

    /*
     * Clock object is always placed on Clock work Q
     */
    clockQ = &Clock_module->clockQ;
    Queue_put(clockQ, &obj->objElem);

    if (params->startFlag != false) {
        Clock_start(obj);
    }
}

/*
 *  ======== Clock_Instance_finalize ========
 */
void Clock_Instance_finalize(Clock_Object *obj)
{
    unsigned int key;

    Assert_isTrue(((BIOS_getThreadType() != BIOS_ThreadType_Hwi) &&
                   (BIOS_getThreadType() != BIOS_ThreadType_Swi)),
                        Clock_A_badThreadType);

    key = Hwi_disable();

    Queue_remove(&obj->objElem);

    Hwi_restore(key);
}

/*
 *  ======== Clock_addI ========
 */
void Clock_addI(Clock_Object *obj, Clock_FuncPtr func, uint32_t timeout, uintptr_t arg)
{
    Queue_Handle clockQ;

    obj->timeout = timeout;
    obj->period = 0;
    obj->fxn = func;
    obj->arg = arg;
    obj->active = false;

    /*
     * Clock object is always placed on Clock work Q
     */
    clockQ = &Clock_module->clockQ;
    Queue_put(clockQ, &obj->objElem);
}

/*
 *  ======== Clock_initI ========
 */
void Clock_initI(Clock_Object *obj, Clock_FuncPtr func, uint32_t timeout, uintptr_t arg)
{
    obj->timeout = timeout;
    obj->period = 0;
    obj->fxn = func;
    obj->arg = arg;
    obj->active = false;
}

/*
 *  ======== Clock_enqueueI ========
 */
void Clock_enqueueI(Clock_Object *obj)
{
    Queue_Handle clockQ;

    clockQ = &Clock_module->clockQ;
    Queue_enqueue(clockQ, &obj->objElem);
}

/*
 *  ======== Clock_removeI ========
 */
void Clock_removeI(Clock_Object *obj)
{
    Queue_remove(&obj->objElem);
}

/*
 *  ======== Clock_startI ========
 *  Set the Clock object's currTimeout value and set its active flag
 *  to true.
 */
void Clock_startI(Clock_Object *obj)
{
    obj->timeoutTicks = obj->timeout;

    if (Clock_tickMode == Clock_TickMode_DYNAMIC) {
        uint32_t nowTick, nowDelta;
        uint32_t scheduledTick, scheduledDelta;
        uint32_t remainingTicks;
        bool objectServiced = false;

        /* now see if need this new timeout before next scheduled tick ... */
        /* wait till after first tick */
        if ((Clock_module->ticking == true) &&
             /* if Clock is NOT currently processing its Q */
             (Clock_module->inWorkFunc == false)) {

            /*
             * get virtual current tick count,
             * signal Timer to save corresponding NOW info
             */
            nowTick = ClockSupport_getCurrentTick(true);

            nowDelta = nowTick - Clock_module->ticks;
            scheduledDelta = Clock_module->nextScheduledTick -
                Clock_module->ticks;

            if (nowDelta <= scheduledDelta) {

                objectServiced = true;

                /* start new Clock object */
                obj->currTimeout = nowTick + obj->timeout;
                obj->active = true;

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

        if (objectServiced == false) {
            /*
             * get virtual current tick count,
             * DO NOT (!) signal Timer to save corresponding NOW info
             */
            nowTick = Clock_getTicks();

            /* start new Clock object */
            obj->currTimeout = nowTick + obj->timeout;
            obj->active = true;

            if (Clock_module->inWorkFunc == true) {
                Clock_module->startDuringWorkFunc = true;
            }
        }
    }
    /* Clock_tickMode == Clock_TickMode_PERIODIC */
    else {
        obj->currTimeout = (Clock_module->ticks + obj->timeout);
        obj->active = true;
    }
}

/*
 *  ======== Clock_start ========
 */
void Clock_start(Clock_Object *obj)
{
    unsigned int key;

    Assert_isTrue(obj->timeout != 0U, NULL);

    key = Hwi_disable();

    Clock_startI(obj);

    Hwi_restore(key);
}

/*
 *  ======== Clock_stop ========
 *  remove and clear Clock object's queue elem from clockQ
 */
void Clock_stop(Clock_Object *obj)
{
    obj->active = false;
#if ti_sysbios_knl_Clock_stopCheckNext__D==true
    if (obj->currTimeout == Clock_module->nextScheduledTick) {
        Clock_start(Clock_triggerClock);
    }
#endif
}

/*
 *  ======== Clock_setFunc ========
 */
void Clock_setFunc(Clock_Object *obj, Clock_FuncPtr fxn, uintptr_t arg)
{
    unsigned int key;

    key = Hwi_disable();

    obj->fxn = fxn;
    obj->arg = arg;

    Hwi_restore(key);
}

/*
 *  ======== Clock_setPeriod ========
 */
void Clock_setPeriod(Clock_Object *obj, uint32_t period)
{
    obj->period = period;
}

/*
 *  ======== Clock_setTimeout ========
 */
void Clock_setTimeout(Clock_Object *obj, uint32_t timeout)
{
    obj->timeout = timeout;
}

/*
 *  ======== Clock_getClockFromObjElem ========
 */
Clock_Handle Clock_getClockFromObjElem(Queue_Elem *clockQelem)
{
    if (clockQelem == (Queue_Elem *)&Clock_module->clockQ) {
        return (NULL);
    }

    return ((Clock_Handle)((char *)clockQelem - offsetof(Clock_Struct, objElem)));
}

/*
 *  ======== Clock_getPeriod ========
 */
uint32_t Clock_getPeriod(Clock_Object *obj)
{
    return (obj->period);
}

/*
 *  ======== Clock_getTimeout ========
 */
uint32_t Clock_getTimeout(Clock_Object *obj)
{
    uint32_t timeout = 0;
    unsigned int key;

    key = Hwi_disable();
    if (obj->active == true) {
        timeout = obj->currTimeout - Clock_getTicks();
        if (timeout > obj->timeoutTicks) {
            timeout = 0;
        }
    }
    Hwi_restore(key);

    return(timeout);
}

/*
 *  ======== Clock_isActive ========
 */
bool Clock_isActive(Clock_Object *obj)
{
    return (obj->active);
}

/*
 *  ======== Clock_triggerFunc ========
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
void Clock_triggerFunc(uintptr_t arg)
{
}

/* -------- The following functions were generated in BIOS 6.x -------- */

/*
 *  ======== Clock_Object_first ========
 */
Clock_Handle Clock_Object_first()
{
    return (Clock_getClockFromObjElem(Queue_head(&(Clock_module->clockQ))));
}

/*
 *  ======== Clock_Object_next ========
 */
Clock_Handle Clock_Object_next(Clock_Handle clock)
{
    return (Clock_getClockFromObjElem(Queue_next(&clock->objElem)));
}

/*
 *  ======== Clock_Params_init ========
 */
void Clock_Params_init(Clock_Params *params)
{
    *params = Clock_Params_default;
}

/*
 *  ======== Clock_construct ========
 */
Clock_Handle Clock_construct(Clock_Object *obj, Clock_FuncPtr func, unsigned int timeout,
    const Clock_Params *params)
{
    if (params == NULL) {
        params = &Clock_Params_default;
    }

    Clock_Instance_init(obj, func, timeout, params);

    return (obj);
}

/*
 *  ======== Clock_create ========
 */
Clock_Handle Clock_create(Clock_FuncPtr func, unsigned int timeout,
                const Clock_Params *params, Error_Block *eb)
{
    Clock_Handle clk;

    clk = Memory_alloc(NULL, sizeof(Clock_Object), 0, eb);

    if (clk != NULL) {
        clk = Clock_construct(clk, func, timeout, params);
    }

    return (clk);
}


/*
 *  ======== Clock_destruct ========
 */
void Clock_destruct(Clock_Object *obj)
{
    Clock_Instance_finalize(obj);
}

/*
 *  ======== Clock_delete ========
 */
void Clock_delete(Clock_Handle *obj)
{
    Clock_destruct(*obj);

    Memory_free(NULL, *obj, sizeof(Clock_Object));

    *obj = NULL;
}
