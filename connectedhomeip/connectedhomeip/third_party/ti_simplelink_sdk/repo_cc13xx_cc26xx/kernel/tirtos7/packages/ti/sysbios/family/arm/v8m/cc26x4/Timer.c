/*
 * Copyright (c) 2014-2021, Texas Instruments Incorporated
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
 *  ======== Timer.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/knl/ClockSupport.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/Types.h>

#include <ti/devices/DeviceFamily.h>

#include DeviceFamily_constructPath(driverlib/aon_rtc.h)
#include DeviceFamily_constructPath(driverlib/aon_event.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)

#include <ti/sysbios/family/arm/v8m/cc26x4/Timer.h>

#define COMPARE_MARGIN 6

#define MAX_SKIP  (0x7E9000000000)    /* 32400 seconds (9 hours) */

Timer_Module_State Timer_Module_state = {
    .objQ.next = &Timer_Module_state.objQ,
    .objQ.prev = &Timer_Module_state.objQ,
};

/* funcHookCH1 */
const Timer_FuncPtr Timer_funcHookCH1;

/* funcHookCH2 */
const Timer_FuncPtr Timer_funcHookCH2;

/* startupNeeded */
const unsigned int Timer_startupNeeded;

/* Params */
static const struct Timer_Params Timer_Params_default = {
    .runMode = Timer_RunMode_CONTINUOUS,
    .startMode = Timer_StartMode_AUTO,
    .arg = 0,
    .period = 1000,
    .periodType = Timer_PeriodType_MICROSECS,
    .extFreq.lo = 0,
    .extFreq.hi = 0,
    .hwiParams = NULL
};

/*
 *  ======== Timer_getNumTimers ========
 *  Get number of timer peripherals available on the device.
 */
unsigned int Timer_getNumTimers()
{
    return (1);
}

/*
 *  ======== Timer_getStatus ========
 *  Get the FREE/INUSE status of the timer.
 */
Timer_Status Timer_getStatus(unsigned int timerId)
{
    Assert_isTrue(timerId < 1, NULL);

    return (Timer_Status_INUSE);
}

/*
 *  ======== Timer_getMaxTicks ========
 */
uint32_t Timer_getMaxTicks(Timer_Object *obj)
{
    uint32_t ticks;
    uint64_t temp;

    temp = (uint64_t)(MAX_SKIP) / obj->period64;

    /* clip value to Clock tick count limit of 32-bits */
    if (temp > 0xFFFFFFFF) {
        ticks = 0xFFFFFFFF;
    }
    else {
        ticks = (uint32_t) temp;
    }

    return (ticks);
}

/*
 *  ======== Timer_setThreshold ========
 */
void Timer_setThreshold(Timer_Object *obj, uint32_t next, bool wrap)
{
    uint32_t now;
    bool key;

    /* prevent preemption by setting PRIMASK */
    key = IntMasterDisable();

    /* get the current RTC count corresponding to compare window */
    now = AONRTCCurrentCompareValueGet();

    /* else if next is too soon, set at least one RTC tick in future */
    /* assume next never be more than half the maximum 32 bit count value */
    if ((next - now) > (uint32_t)0x80000000) {
	/* now is past next */
        next = now + COMPARE_MARGIN;
    }
    else if ((now + COMPARE_MARGIN - next) < (uint32_t)0x80000000) {
        if (next < now + COMPARE_MARGIN) {
            next = now + COMPARE_MARGIN;
        }
    }

    /* set next compare threshold in RTC */
    AONRTCCompareValueSet(AON_RTC_CH0, next);

    /* restore PRIMASK */
    if (!key) {
        IntMasterEnable();
    }
}

/*
 *  ======== Timer_setNextTick ========
 */
void Timer_setNextTick(Timer_Object *obj, uint32_t ticks)
{
    bool wrap = false;
    uint32_t next;
    uint64_t newThreshold;

    /* calculate new 64-bit RTC count for next interrupt */
    newThreshold = obj->savedCurrCount + (uint64_t)ticks * obj->period64;

    /* isolate the new 32-bit compare value to write to RTC */
    next = (uint32_t)(newThreshold >> 16);

    /* set the compare threshold at the RTC */
    Timer_setThreshold(obj, next, wrap);

    /* save the threshold for next interrupt */
    obj->prevThreshold = newThreshold;
}

/*
 *  ======== Timer_startup ========
 *  Here after call to main().  Called from BIOS_start().
 */
void Timer_startup()
{
}

/*
 *  ======== Timer_getHandle ========
 */
Timer_Handle Timer_getHandle(unsigned int id)
{
    Assert_isTrue((id < 1), NULL);

    return (Timer_module->handle);
}

/*
 *  ======== Timer_Instance_init ========
 */
int Timer_Instance_init(Timer_Object *obj, int id, Timer_FuncPtr tickFxn,
    const Timer_Params *params, Error_Block *eb)
{
    /* not implemented for this timer */
    return (1);
}

/*
 *  ======== Timer_Instance_finalize ========
 */
void Timer_Instance_finalize(Timer_Object *obj, int status)
{
    /* not implemented for this timer */
}

/*
 *  ======== Timer_start ========
 *
 * 1. Hwi_disable()
 * 2. Reset the RTC
 * 3. Clear any RTC events
 * 4. Set first compare threshold (per configured period)
 * 5. Enable the compare channel
 * 6. Configure events for CH0, and other configured channels
 * 7. Enable the RTC to start it ticking
 * 8. Hwi_restore()
 *
 */
void Timer_start(Timer_Object *obj)
{
    uint32_t compare;
    unsigned int key;

    key = Hwi_disable();

    /*
     * set the compare register to the counter start value plus one period.
     * For a very small period round up to interrupt upon 4th RTC tick
     */
    if (obj->period < 0x40000) {
        compare = 0x4;    /* 4 * 15.5us ~= 62us */
    }
    /* else, interrupt on first period expiration */
    else {
        compare = obj->period >> 16;
    }

    /* set the compare value at the RTC */
    AONRTCCompareValueSet(AON_RTC_CH0, compare);

    /* enable compare channel 0 */
    AONEventMcuWakeUpSet(AON_EVENT_MCU_WU0, AON_EVENT_RTC0);
    AONRTCEventClear(AON_RTC_CH0);
    AONRTCChannelEnable(AON_RTC_CH0);

    Hwi_restore(key);
}

/*
 *  ======== Timer_stop ========
 */
void Timer_stop(Timer_Object *obj)
{
    /* not implemented for this timer */
    obj->savedCurrCount = 0;
}

/*
 *  ======== Timer_setPeriod ========
 *
 * 1. Stop timer
 * 2. Set period value in timer obj
 *
 */
void Timer_setPeriod(Timer_Object *obj, uint32_t period)
{
    obj->period = period;
}

/*
 *  ======== Timer_setPeriodMicroSecs ========
 */
bool Timer_setPeriodMicroSecs(Timer_Object *obj, uint32_t period)
{
    /* not implemented for this timer */
    return (false);
}

/*
 *  ======== Timer_trigger ========
 */
void Timer_trigger(Timer_Object *obj, uint32_t insts)
{
    /* not implemented for this timer */
}

/*
 *  ======== Timer_getPeriod ========
 */
uint32_t Timer_getPeriod(Timer_Object *obj)
{
    return(obj->period);
}

/*
 *  ======== Timer_getCount ========
 */
uint32_t Timer_getCount(Timer_Object *obj)
{
    return(HWREG(AON_RTC_BASE + AON_RTC_O_SUBSEC));
}

/*
 *  ======== Timer_getCount64 ========
 */
uint64_t Timer_getCount64(Timer_Object *obj)
{
    return(AONRTCCurrent64BitValueGet());
}

/*
 *  ======== Timer_dynamicStub ========
 */
void Timer_dynamicStub(uintptr_t arg)
{
    Timer_Object *obj;

    obj = (Timer_Object *)arg;

    /* clear the RTC event */
    AONRTCEventClear(AON_RTC_CH0);

    /* call the tick function */
    obj->tickFxn(obj->arg);
}

/*
 *  ======== Timer_dynamicMultiStub ========
 */
void Timer_dynamicMultiStub(uintptr_t arg)
{
    Timer_Object *obj;

    obj = (Timer_Object *)arg;

    /* if a hook is configured for CH1... */
    if (Timer_funcHookCH1) {

        /* check if an event occurred on CH1; if yes, clear event, call hook */
        if (AONRTCEventGet(AON_RTC_CH1)) {
            AONRTCEventClear(AON_RTC_CH1);
            Timer_funcHookCH1(1);
        }
    }

    /* if a hook is configured for CH2... */
    if (Timer_funcHookCH2) {

        /* check if an event occurred on CH2; if yes, clear event, call hook */
        if (AONRTCEventGet(AON_RTC_CH2)) {
            AONRTCEventClear(AON_RTC_CH2);
            Timer_funcHookCH2(2);
        }
    }

    /* now check if CH0 has a Timer event... */
    if (AONRTCEventGet(AON_RTC_CH0)) {

        /* clear the event and call the Timer's tick function */
        AONRTCEventClear(AON_RTC_CH0);
        obj->tickFxn(obj->arg);
    }

}

/*
 *  ======== Timer_periodicStub ========
 */
void Timer_periodicStub(uintptr_t arg)
{
    uint64_t newThreshold;
    Timer_Object *obj;
    bool wrap = false;
    uint32_t next;

    obj = (Timer_Object *)arg;

    /* clear the RTC event */
    AONRTCEventClear(AON_RTC_CH0);

    /* calculate new 64-bit RTC count for next interrupt */
    newThreshold = (uint64_t) obj->nextThreshold + (uint64_t) obj->period64;

    /* isolate the new 32-bit compare value to write to RTC */
    next = (uint32_t) ((uint64_t) newThreshold >> 16);

    /* check to see if wrapping into upper 16-bits of SEC */
    if ((newThreshold & 0xFFFFFFFFFFFF) <
        (obj->prevThreshold & 0xFFFFFFFFFFFF)) {
        wrap = true;
    }

    /* set the compare threshold at the RTC */
    Timer_setThreshold(obj, next, wrap);

    /* update threshold counters saved in timer obj */
    obj->prevThreshold = obj->nextThreshold;
    obj->nextThreshold = newThreshold;

    /* call the tick function */
    obj->tickFxn(obj->arg);
}

/*
 *  ======== Timer_getExpiredCounts ========
 *  Returns expired counts (32-bits) since the last serviced timer interrupt.
 */
uint32_t Timer_getExpiredCounts(Timer_Object *obj)
{
    return (0);
}

/*
 *  ======== Timer_getExpiredCounts64 ========
 *  Returns expired counts (64-bits) since the last serviced timer interrupt.
 */
uint64_t Timer_getExpiredCounts64(Timer_Object *obj)
{
    return (0);
}

/*
 *  ======== Timer_getCurrentTick ========
 *  Used by the Clock module for TickMode_DYNAMIC to query the corresponding
 *  Clock tick, as derived from the current timer count.
 */
uint32_t Timer_getCurrentTick(Timer_Object *obj, bool saveFlag)
{
    uint64_t tick, currCount;

    currCount = (uint64_t) Timer_getCount64(obj);

    tick = currCount / obj->period64;

    if (saveFlag != 0) {
        /*
         * to avoid accumulating drift, make currCount be an integer
         * multiple of timer periods
         */
        obj->savedCurrCount = tick * obj->period64;
    }

    return ((uint32_t) tick);
}

/*
 *  ======== Timer_getExpiredTicks ========
 *  Used by the Clock module for TickMode_DYNAMIC to determine the number of
 *  virtual ticks that have occurred since the last serviced timer interrupt.
 */
uint32_t Timer_getExpiredTicks(Timer_Object *obj, uint32_t tickPeriod)
{
    return (0);
}

/*
 *  ======== Timer_getFreq ========
 */
void Timer_getFreq(Timer_Object *obj, Types_FreqHz *freq)
{
    freq->lo = obj->frequency.lo;
    freq->hi = obj->frequency.hi;
}

/*
 *  ======== Timer_getFunc ========
 */
Timer_FuncPtr Timer_getFunc(Timer_Object *obj, uintptr_t *arg)
{
    *arg = obj->arg;
    return (obj->tickFxn);
}

/*
 *  ======== Timer_setFunc ========
 */
void Timer_setFunc(Timer_Object *obj, Timer_FuncPtr fxn, uintptr_t arg)
{
    obj->tickFxn = fxn;
    obj->arg = arg;
}

/*
 *  ======== Timer_postInit ========
 */
int Timer_postInit(Timer_Object *obj)
{
    unsigned int key;

    key = Hwi_disable();

    Timer_setPeriod(obj, obj->period);

    Hwi_restore(key);

    return (0);
}

/*
 *  ======== Timer_Params_init ========
 */
void Timer_Params_init(Timer_Params *params)
{
    *params = Timer_Params_default;
}

/*
 *  ======== Timer_construct ========
 */
Timer_Handle Timer_construct(Timer_Struct *obj, int id, Timer_FuncPtr tickFxn,
     const Timer_Params *params, Error_Block *eb)
{
    Hwi_Handle hwi;
    Hwi_Params hwiParams;
    unsigned int key;

    /* Only supports one timer. Make sure it's not already in use. */
    key = Hwi_disable();
    if (Timer_module->handle != NULL) {
        Hwi_restore(key);
        return (NULL);
    }
    Timer_module->handle = obj;
    Hwi_restore(key);

    if (params == NULL) {
        params = &Timer_Params_default;
    }

    obj->id = id;
    obj->tickFxn = tickFxn;
    obj->arg = params->arg;
    obj->frequency.lo = 65536;
    obj->frequency.hi = 0;
    obj->savedCurrCount = 0;

    if (params->periodType == Timer_PeriodType_COUNTS) {
        obj->period = params->period;
        obj->period64 = (uint64_t)params->period;
    }
    else {
        /*
         * The upper 16-bits of SUBSEC will roll over every second, so the
         * 'effective' rate the RTC for generating interrupt events is
         * 0x100000000 Hz.
         */
        obj->period64 = (uint64_t)0x100000000 * (uint64_t)params->period /
                (uint64_t)1000000;
        obj->period = obj->period64;

        if (obj->period64 > 0xffffffff) {
            return (NULL);
        }
    }

    if (params->runMode == Timer_RunMode_DYNAMIC) {
        obj->prevThreshold = obj->period;
    }
    else {
        obj->prevThreshold = 0;
    }
    obj->nextThreshold = obj->period;

    Hwi_Params_init(&hwiParams);
    hwiParams.arg = (uintptr_t)obj;

    if (params->runMode == Timer_RunMode_CONTINUOUS) {
        hwi = Hwi_construct(&obj->hwiStruct, 20, Timer_dynamicStub,
            &hwiParams, Error_IGNORE);
    }
    else {
        hwi = Hwi_construct(&obj->hwiStruct, 20, Timer_dynamicMultiStub,
            &hwiParams, Error_IGNORE);
    }

    Timer_postInit(obj);

    if (obj->startMode == Timer_StartMode_AUTO) {
        Timer_start(obj);
    }

    if (hwi != NULL) {
        /* put timer on global timer list (Queue_put is atomic) */
        Queue_put(&Timer_module->objQ, &obj->objElem);
        return (obj);
    }
    else {
        Timer_module->handle = NULL;
        return (NULL);
    }
}

/*
 *  ======== Timer_getTimerFromObjElem ========
 */
Timer_Handle Timer_getTimerFromObjElem(Queue_Elem *timerQelem)
{
    if (timerQelem == (Queue_Elem *)&Timer_module->objQ) {
        return (NULL);
    }

    return ((Timer_Handle)((char *)timerQelem -
               offsetof(Timer_Struct, objElem)));
}

/*
 *  ======== Timer_Object_first ========
 */
Timer_Handle Timer_Object_first()
{
    return (Timer_getTimerFromObjElem(Queue_head(&(Timer_module->objQ))));
}

/*
 *  ======== Timer_Object_next ========
 */
Timer_Handle Timer_Object_next(Timer_Handle handle)
{
    return (Timer_getTimerFromObjElem(Queue_next(&handle->objElem)));
}
