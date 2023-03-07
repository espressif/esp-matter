/*
 * Copyright (c) 2014-2017, Texas Instruments Incorporated
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
 *
 */

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Types.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/devices/DeviceFamily.h>

#include DeviceFamily_constructPath(driverlib/aon_rtc.h)
#include DeviceFamily_constructPath(driverlib/aon_event.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)

#include "package/internal/Timer.xdc.h"

#define COMPARE_MARGIN 6

#define MAX_SKIP  (0x7E9000000000)    /* 32400 seconds (9 hours) */


/*
 *  ======== Timer_getNumTimers ========
 *  Get number of timer peripherals available on the device.
 */
UInt Timer_getNumTimers()
{
    return (1);
}

/*
 *  ======== Timer_getStatus ========
 *  Get the FREE/INUSE status of the timer.
 */
Timer_Status Timer_getStatus(UInt timerId)
{
    Assert_isTrue(timerId < 1, NULL);

    if (Timer_module->availMask & (0x1 << timerId)) {
        return (Timer_Status_FREE);
    }
    else {
        return (Timer_Status_INUSE);
    }
}

/*
 *  ======== Timer_getMaxTicks ========
 */
UInt32 Timer_getMaxTicks(Timer_Object *obj)
{
    UInt32 ticks;
    UInt64 temp;

    temp = (UInt64)(MAX_SKIP) / obj->period64;

    /* clip value to Clock tick count limit of 32-bits */
    if (temp > 0xFFFFFFFF) {
        ticks = 0xFFFFFFFF;
    }
    else {
        ticks = (UInt32) temp;
    }

    return (ticks);
}

/*
 *  ======== Timer_setThreshold ========
 */
Void Timer_setThreshold(Timer_Object *obj, UInt32 next, Bool wrap)
{
    UInt32 now;
    Bool key;

    /* prevent preemption by setting PRIMASK */
    key = IntMasterDisable();

    /* get the current RTC count corresponding to compare window */
    now = AONRTCCurrentCompareValueGet();
    
    /* else if next is too soon, set at least one RTC tick in future */
    /* assume next never be more than half the maximum 32 bit count value */
    if ((next - now) > (UInt32)0x80000000) {
	/* now is past next */
        next = now + COMPARE_MARGIN;
    }
    else if ((now + COMPARE_MARGIN - next) < (UInt32)0x80000000) {
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
Void Timer_setNextTick(Timer_Object *obj, UInt32 ticks)
{
    Bool wrap = FALSE;
    UInt32 next;
    UInt64 newThreshold;

    /* calculate new 64-bit RTC count for next interrupt */
    newThreshold = obj->savedCurrCount + (UInt64)ticks * obj->period64;

    /* isolate the new 32-bit compare value to write to RTC */
    next = (UInt32)(newThreshold >> 16);

    /* set the compare threshold at the RTC */
    Timer_setThreshold(obj, next, wrap);

    /* save the threshold for next interrupt */
    obj->prevThreshold = newThreshold;
}

/*
 *  ======== Timer_Module_startup ========
 *  Calls postInit for all statically-created & constructed
 *  timers to initialize them.
 */
Int Timer_Module_startup(Int status)
{
    Timer_Object *obj;

    if (Timer_startupNeeded) {
        obj = Timer_module->handle;
        /* if timer was statically created/constructed */
        if ((obj != NULL) && (obj->staticInst)) {
            Timer_postInit(obj, NULL);
        }
    }

    return (Startup_DONE);
}

/*
 *  ======== Timer_startup ========
 *  Here after call to main().  Called from BIOS_start().
 */
Void Timer_startup()
{
    Timer_Object *obj;

    if (Timer_startupNeeded) {
        obj = Timer_module->handle;
        /* if timer was statically created/constructed */
        if ((obj != NULL) && (obj->staticInst)) {
            if (obj->startMode == Timer_StartMode_AUTO) {
                Timer_start(obj);
            }
        }
    }
}

/*
 *  ======== Timer_getHandle ========
 */
Timer_Handle Timer_getHandle(UInt id)
{
    Assert_isTrue((id < 1), NULL);
    return (Timer_module->handle);
}

/*
 *  ======== Timer_Instance_init ========
 */
Int Timer_Instance_init(Timer_Object *obj, Int id, Timer_FuncPtr tickFxn, const Timer_Params *params, Error_Block *eb)
{
    /* not implemented for this timer */
    return (1);
}

/*
 *  ======== Timer_Instance_finalize ========
 */
Void Timer_Instance_finalize(Timer_Object *obj, Int status)
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
Void Timer_start(Timer_Object *obj)
{
    UInt32 events = AON_RTC_CH0;
    UInt32 compare;
    UInt key;

    key = Hwi_disable();

    /* reset timer */
    AONRTCReset();
    AONRTCEventClear(AON_RTC_CH0);
    IntPendClear(INT_AON_RTC_COMB);

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
    AONRTCChannelEnable(AON_RTC_CH0);

    /* configure CH0 events, plus CH1 and CH2 too if hooks are defined */
    if (Timer_funcHookCH1) {
        events |= AON_RTC_CH1;
    }
    if (Timer_funcHookCH2) {
        events |= AON_RTC_CH2;
    }
    AONRTCCombinedEventConfig(events);

    /* start timer */
    AONRTCEnable();

    Hwi_restore(key);
}

/*
 *  ======== Timer_stop ========
 */
Void Timer_stop(Timer_Object *obj)
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
Void Timer_setPeriod(Timer_Object *obj, UInt32 period)
{
    obj->period = period;
}

/*
 *  ======== Timer_setPeriodMicroSecs ========
 */
Bool Timer_setPeriodMicroSecs(Timer_Object *obj, UInt32 period)
{
    /* not implemented for this timer */
    return (FALSE);
}

/*
 *  ======== Timer_trigger ========
 */
Void Timer_trigger(Timer_Object *obj, UInt32 insts)
{
    /* not implemented for this timer */
}

/*
 *  ======== Timer_getPeriod ========
 */
UInt32 Timer_getPeriod(Timer_Object *obj)
{
    return(obj->period);
}

/*
 *  ======== Timer_getCount ========
 */
UInt32 Timer_getCount(Timer_Object *obj)
{
    return(HWREG(AON_RTC_BASE + AON_RTC_O_SUBSEC));
}

/*
 *  ======== Timer_getCount64 ========
 */
UInt64 Timer_getCount64(Timer_Object *obj)
{
    return(AONRTCCurrent64BitValueGet());
}

/*
 *  ======== Timer_dynamicStub ========
 */
Void Timer_dynamicStub(UArg arg)
{
    Timer_Object *obj;

    obj = Timer_module->handle;

    /* clear the RTC event */
    AONRTCEventClear(AON_RTC_CH0);

    /* call the tick function */
    obj->tickFxn(obj->arg);
}

/*
 *  ======== Timer_dynamicMultiStub ========
 */
Void Timer_dynamicMultiStub(UArg arg)
{
    Timer_Object *obj;

    obj = Timer_module->handle;

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
Void Timer_periodicStub(UArg arg)
{
    UInt64 newThreshold;
    Timer_Object *obj;
    Bool wrap = FALSE;
    UInt32 next;

    obj = Timer_module->handle;

    /* clear the RTC event */
    AONRTCEventClear(AON_RTC_CH0);

    /* calculate new 64-bit RTC count for next interrupt */
    newThreshold = (UInt64) obj->nextThreshold + (UInt64) obj->period64;

    /* isolate the new 32-bit compare value to write to RTC */
    next = (UInt32) ((UInt64) newThreshold >> 16);

    /* check to see if wrapping into upper 16-bits of SEC */
    if ((newThreshold & 0xFFFFFFFFFFFF) <
        (obj->prevThreshold & 0xFFFFFFFFFFFF)) {
        wrap = TRUE;
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
UInt32 Timer_getExpiredCounts(Timer_Object *obj)
{
    return (0);
}

/*
 *  ======== Timer_getExpiredCounts64 ========
 *  Returns expired counts (64-bits) since the last serviced timer interrupt.
 */
UInt64 Timer_getExpiredCounts64(Timer_Object *obj)
{
    return (0);
}

/*
 *  ======== Timer_getCurrentTick ========
 *  Used by the Clock module for TickMode_DYNAMIC to query the corresponding
 *  Clock tick, as derived from the current timer count.
 */
UInt32 Timer_getCurrentTick(Timer_Object *obj, Bool saveFlag)
{
    UInt64 tick, currCount;

    currCount = (UInt64) Timer_getCount64(obj);

    tick = currCount / obj->period64;

    /* 
     * to avoid accumulating drift, make currCount be an integer
     * multiple of timer periods
     */
    currCount = tick * obj->period64;
    
    if (saveFlag != 0) {
        obj->savedCurrCount = currCount;
    }        

    return ((UInt32) tick);
}

/*
 *  ======== Timer_getExpiredTicks ========
 *  Used by the Clock module for TickMode_DYNAMIC to determine the number of
 *  virtual ticks that have occurred since the last serviced timer interrupt.
 */
UInt32 Timer_getExpiredTicks(Timer_Object *obj, UInt32 tickPeriod)
{
    return (0);
}

/*
 *  ======== Timer_getFreq ========
 */
Void Timer_getFreq(Timer_Object *obj, Types_FreqHz *freq)
{
    freq->lo = obj->frequency.lo;
    freq->hi = obj->frequency.hi;
}

/*
 *  ======== Timer_getFunc ========
 */
Timer_FuncPtr Timer_getFunc(Timer_Object *obj, UArg *arg)
{
    *arg = obj->arg;
    return (obj->tickFxn);
}

/*
 *  ======== Timer_setFunc ========
 */
Void Timer_setFunc(Timer_Object *obj, Timer_FuncPtr fxn, UArg arg)
{
    obj->tickFxn = fxn;
    obj->arg = arg;
}

/* ======== Timer_initDevice ========
 * Disable and reset the RTC.
 */
Void Timer_initDevice(Timer_Object *obj)
{
    AONRTCDisable();
    AONRTCReset();

    HWREG(AON_RTC_BASE + AON_RTC_O_SYNC) = 1;
    /* read sync register to complete reset */
    HWREG(AON_RTC_BASE + AON_RTC_O_SYNC);

    AONRTCEventClear(AON_RTC_CH0);
    IntPendClear(INT_AON_RTC_COMB);

    HWREG(AON_RTC_BASE + AON_RTC_O_SYNC);

}

/*
 *  ======== Timer_postInit ========
 */
Int Timer_postInit (Timer_Object *obj, Error_Block *eb)
{
    UInt key;

    key = Hwi_disable();

    Timer_initDevice(obj);

    Timer_setPeriod(obj, obj->period);

    Hwi_restore(key);

    return (0);
}
