/*
 * Copyright (c) 2014-2018, Texas Instruments Incorporated
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
#include <ti/sysbios/knl/Clock.h>

#include "package/internal/Timer.xdc.h"

/* Definitions from CC3200 SDK */
#define HWREG(x)                (*((volatile unsigned long *)(x)))

#define HIB3P3_BASE                        0x4402F800
#define HIB3P3_O_MEM_HIB_RTC_TIMER_ENABLE  0x00000004
#define HIB3P3_O_MEM_HIB_RTC_TIMER_RESET   0x00000008
#define HIB3P3_O_MEM_HIB_RTC_TIMER_READ    0x0000000C
#define HIB3P3_O_MEM_HIB_RTC_TIMER_LSW     0x00000010
#define HIB3P3_O_MEM_HIB_RTC_TIMER_MSW     0x00000014
#define HIB3P3_O_MEM_HIB_RTC_WAKE_EN       0x00000018
#define HIB3P3_O_MEM_HIB_RTC_IRQ_ENABLE    0x00000054
#define HIB3P3_O_MEM_HIB_RTC_IRQ_LSW_CONF  0x00000058
#define HIB3P3_O_MEM_HIB_RTC_IRQ_MSW_CONF  0x0000005C
#define HIB3P3_O_MEM_INT_OSC_CONF          0x0000002C

#define ARCM_BASE                          0x44025000
#define APPS_RCM_O_APPS_RCM_INTERRUPT_STATUS 0x00000120
#define APPS_RCM_O_APPS_RCM_INTERRUPT_ENABLE 0x00000124
#define PRCM_INT_SLOW_CLK_CTR              0x00004000

/*
 *  The maximum number of timer ticks that can be skipped by the
 *  Clock module.  Set this to some value greater than the number
 *  of ticks required for the RTC counter LSW to rollover:
 *    (0x100000000 ticks / rollover) * (1 sec / 32768 tick) =
 *        131072 secs = 36 hrs 24 min 32 secs
 *
 *  So a value of 37 hours will be sufficient.
 */
#define MAX_SKIP  (0x104280000) /* (37 hours * 3600 secs/hr) * 32768 tics/sec */

#define COMPARE_MARGIN 12

/*
 *  Macros for accessing RTC registers in 40MHz domain.
 *
 *  The RTC registers in the 40 MHz domain are auto latched. Hence, there is
 *  no requirement to write to latch the RTC values. However, there is a
 *  caveat: If the value is read at the instant the 40 MHz clock and 32.768 KHz
 *  clock aligned, the value read could be erroneous.  As a workaround, the
 *  recommendation is to read the value thrice and identify the right value
 *  (as 2 out the 3 read values will always be correct and with a max. of
 *  1 LSB change).
 */
#define HIB1P2_BASE                        0x4402F000
#define HIB1P2_O_HIB_RTC_TIMER_LSW_1P2     0x000000CC
#define HIB1P2_O_HIB_RTC_TIMER_MSW_1P2     0x000000D0

#define MAX_ITER_TO_CONFIRM    3
#define RET_IF_WITHIN_TRESHOLD(a, b, th) {if (((a) - (b)) <= (th)) return (a);}

#define PRCMSlowClkCtrGet_HIB1p2(count) \
        count = HWREG(HIB1P2_BASE + HIB1P2_O_HIB_RTC_TIMER_MSW_1P2); \
        count = count << 32;                                         \
        count = count | (UInt64)HWREG(HIB1P2_BASE + HIB1P2_O_HIB_RTC_TIMER_LSW_1P2);


/*
 *  For testing, use a fake timer counter rollover point.  The RTC
 *  counter LSW rolls over about every 36 hours.  The values below
 *  will cut that time by a factor of (2 ** UPPERSHIFT).
 */
#ifdef CC32XX_TIMER_TEST

#define USE_LOWER_ROLLOVER 1
#define TESTING 1
#else
#define USE_LOWER_ROLLOVER 0
#define TESTING 0
#endif

#if USE_LOWER_ROLLOVER

#define ROLLOVER       0x00100000
#define LOWERMASK      0x000FFFFF
#define UPPERSHIFT     12

#else

#define ROLLOVER      0x100000000
#define LOWERMASK      0xFFFFFFFF
#define UPPERSHIFT     0

#endif

#if TESTING
Int rolloverCount = 0;
UInt32 nextThresholdUpper = 0;
UInt32 setNextThresholdUpperCnt = 0;

#endif

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

    /* Suppress for at most 1 hour */
    ticks = (UInt32)((UInt64)(MAX_SKIP) / obj->period64);

    return (ticks);
}

/*
 *  ======== Timer_setThreshold ========
 */
Void Timer_setThreshold(Timer_Object *obj, UInt64 next)
{
    UInt64 curTime;
#if TESTING
    UInt32 nextUpper;
#endif
    curTime = Timer_getCount64(obj);
    if (next < curTime + COMPARE_MARGIN) {
        next = curTime + COMPARE_MARGIN;
    }

    HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_RTC_IRQ_LSW_CONF) = (UInt32)(next);

    /*
     *  Only set the upper word of the IRQ register if it needs
     *  to be changed.  This should occur about once every 36 hours:
     *  0x100000000 / 0x8000 (= 32768, the RTC frequency) seconds.
     */
    if ((next >> 32) != (Timer_module->nextThreshold >> 32)) {
        HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_RTC_IRQ_MSW_CONF)
            = (UInt32)(next >> 32);
    }

#if TESTING
    nextUpper = (UInt32)(next >> (32 - UPPERSHIFT));

    if (nextUpper != nextThresholdUpper) {
        nextThresholdUpper = nextUpper;
        setNextThresholdUpperCnt++;
    }

#endif

    /* update threshold counters saved in timer obj */
    Timer_module->nextThreshold = next;
}

/*
 *  ======== Timer_setNextMaxTick ========
 *  Timer module's Clock object's Clock function.  Called when
 *  the Clock object's timeout expires, to set a new timeout.  The
 *  new timeout is calculated to be just before the RTC Counter LSW
 *  rolls over.
 */
Void Timer_setNextMaxTick(UArg arg)
{
    UInt64 ticks;
    UInt64 t;
    UInt64 r;
    UInt32 tLower, tUpper;
    UInt key;

    key = Hwi_disable();

    /* Calculate the next timeout value */
    t = Timer_getCount64(Timer_module->handle);

    tLower = (UInt32)(t & LOWERMASK);
    tUpper = (UInt32)(t >> (32 - UPPERSHIFT));

#if TESTING
    rolloverCount++;
#endif

    if (Timer_module->timeUpper < tUpper) {
        /*
         *  We have already rolled over.  We want the next timeout to be
         *  at the last Clock tick before the lower 32 bits of the timer
         *  counter rolls over again.  Calculate the number of Clock ticks
         *  to reach that point.  It is better to be too low, since we
         *  never want to set the timeout beyond the rollover point.
         */
        ticks = (ROLLOVER - (UInt64)tLower - 1) / Timer_module->period64;
        Timer_module->timeUpper = tUpper;
    }
    else {
        /*
         *  We have not rolled over yet (we'll assume Timer_module->timeUpper
         *  equals tUpper, since the lower 32 bits of the counter roll over
         *  approximately every 36 hours).
         */
        /* Number of Clock ticks left before rollover */
        ticks = (UInt32)((ROLLOVER - (UInt64)tLower) / Timer_module->period64);

        /* Remaining timer counter ticks needed to actually roll over */
        r = (ROLLOVER - (UInt64)tLower) - ticks * Timer_module->period64;

        /* Add one Clock tick for the rollover tick */
        ticks++;

        ticks += (ROLLOVER - Timer_module->period64 + r - 1) / Timer_module->period64;
        Timer_module->timeUpper = tUpper + 1;
    }

    Clock_setTimeout(Timer_module->clock, (UInt32)ticks);
    Clock_start(Timer_module->clock);

    Hwi_restore(key);
}

/*
 *  ======== Timer_setNextTick ========
 */
Void Timer_setNextTick(Timer_Object *obj, UInt32 ticks)
{
    UInt64 newThreshold;

    /* calculate new 64-bit RTC count for next interrupt */
    newThreshold = obj->savedCurrCount + (UInt64)ticks * obj->period64;

    Timer_setThreshold(obj, newThreshold);
}

/*
 *  ======== Timer_Module_startup ========
 *  Called before main(), initializes all statically-created & constructed
 *  timers.
 */
Int Timer_Module_startup(Int status)
{
    Timer_Object *obj;

    if (Timer_startupNeeded) {
        obj = Timer_module->handle;
        /* if timer was statically created/constructed */
        if ((obj != NULL) && (obj->staticInst)) {
            Timer_initDevice(obj);
            Timer_setPeriod(obj, obj->period64);
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
    UInt64 t;
    UInt32 tLower, tUpper;
    UInt32 ticks;
    UInt key;

    if (Timer_startupNeeded) {
        obj = Timer_module->handle;
        /* if timer was statically created/constructed */
        if ((obj != NULL) && (obj->staticInst)) {
            if (obj->startMode == Timer_StartMode_AUTO) {
                Timer_start(obj);
            }
        }
    }

    if (Timer_module->clock != NULL) {
        key = Hwi_disable();

        /* Calculate the initial timeout value */
        t = Timer_getCount64(Timer_module->handle);
        tLower = (UInt32)(t & LOWERMASK);
        tUpper = (UInt32)(t >> (32 - UPPERSHIFT));
        ticks = (ROLLOVER - (UInt64)tLower - 1) / Timer_module->period64;
        Timer_module->timeUpper = tUpper;

        Clock_setTimeout(Timer_module->clock, (UInt32)ticks);
        Clock_start(Timer_module->clock);

        Hwi_restore(key);
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
Int Timer_Instance_init(Timer_Object *obj, Int id, Timer_FuncPtr tickFxn,
        const Timer_Params *params, Error_Block *eb)
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
 */
Void Timer_start(Timer_Object *obj)
{
    UInt key;
    UInt64 curCount;
    UInt64 next;

    key = Hwi_disable();

    Timer_initDevice(obj);

    /* start timer */
    curCount = Timer_getCount64(obj);
    next = curCount + obj->period64;

    /* Set the first threshold */
    HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_RTC_IRQ_LSW_CONF) = (UInt32)(next);
    HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_RTC_IRQ_MSW_CONF)
            = (UInt32)(next >> 32);

    Timer_module->nextThreshold = next;

    HWREG(ARCM_BASE + APPS_RCM_O_APPS_RCM_INTERRUPT_ENABLE) |= 0x4;
    HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_RTC_IRQ_ENABLE) |= 0x1;

    /* Enable the timer */
    HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_RTC_TIMER_ENABLE) = 0x1;

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
    obj->period64 = (UInt64)period;
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
    return((UInt32)(obj->period64));
}

/*
 *  ======== Timer_getCount ========
 */
UInt32 Timer_getCount(Timer_Object *obj)
{
    UInt64 count = Timer_getCount64(obj);

    return ((UInt32)count);
}

/*
 *  ======== Timer_getCount64 ========
 */
UInt64 Timer_getCount64(Timer_Object *obj)
{
    UInt64 count[3];
    Int    i;
    UInt key;

    key = Hwi_disable();

    for (i = 0; i < 3; i++) {
        PRCMSlowClkCtrGet_HIB1p2(count[i]);
    }

    Hwi_restore(key);

    RET_IF_WITHIN_TRESHOLD(count[1], count[0], 1);
    RET_IF_WITHIN_TRESHOLD(count[2], count[1], 1);
    RET_IF_WITHIN_TRESHOLD(count[2], count[0], 1);

    /* Should never get here */
    return ((UInt64)-1);
}


/*
 *  ======== Timer_getMatchShadowReg ========
 */
UInt64 Timer_getMatchShadowReg()
{
    return (Timer_module->nextThreshold);
}

/*
 *  ======== Timer_dynamicStub ========
 */
Void Timer_dynamicStub(UArg arg)
{
    UInt32        status;
    Timer_Object *obj;

    obj = Timer_module->handle;

    /* Reading the interrupt status clears the interrupt */
    status = HWREG(ARCM_BASE + APPS_RCM_O_APPS_RCM_INTERRUPT_STATUS);

    if (status == PRCM_INT_SLOW_CLK_CTR) {
        /* call the tick function */
        obj->tickFxn(obj->arg);
    }
}

/*
 *  ======== Timer_periodicStub ========
 */
Void Timer_periodicStub(UArg arg)
{
    UInt64        newThreshold;
    Timer_Object *obj;
    UInt32        status;

    obj = Timer_module->handle;

    /* Reading the interrupt status clears the interrupt */
    status = HWREG(ARCM_BASE + APPS_RCM_O_APPS_RCM_INTERRUPT_STATUS);

    if (status & PRCM_INT_SLOW_CLK_CTR) {

        /* calculate new 64-bit RTC count for next interrupt */
        newThreshold = (UInt64)Timer_module->nextThreshold +
                (UInt64)obj->period64;

        /* set the compare threshold at the RTC */
        Timer_setThreshold(obj, newThreshold);

        /* call the tick function */
        obj->tickFxn(obj->arg);
    }
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
    currCount = currCount - (currCount % obj->period64);

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

/*
 *  ======== Timer_initDevice ========
 *  Disable and reset the RTC.
 */
Void Timer_initDevice(Timer_Object *obj)
{
    volatile UInt32        status;

    /*
     *  Only start the RTC if it is not already running.
     */
    if ((HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_RTC_TIMER_ENABLE) & 0x1) == 0) {
        /* Read the interrupt status to clear any pending interrupt */
        status = HWREG(ARCM_BASE + APPS_RCM_O_APPS_RCM_INTERRUPT_STATUS);

        (void)status;  /* suppress unused variable warning */
    }
}

/*
 *  ======== Timer_getAvailMask ========
 */
UInt Timer_getAvailMask()
{
    return (Timer_module->availMask);
}

/*
 *  ======== Timer_setAvailMask ========
 */
Bool Timer_setAvailMask(UInt mask)
{
    UInt key;

    key = Hwi_disable();

    /* There is only one timer */
    if ((mask & 0x1) && (Timer_module->handle != NULL)) {
        Hwi_restore(key);
        return (FALSE);
    }

    if (mask >> 1) {
        Hwi_restore(key);
        return (FALSE);
    }

    Timer_module->availMask = mask;
    Hwi_restore(key);

    return (TRUE);
}
