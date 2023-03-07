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
 *  ======== Timer.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Types.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>

#include <ti/catalog/arm/peripherals/timers/timer.h>

#if defined(__IAR_SYSTEMS_ICC__)

#include <intrinsics.h>

#endif

#ifdef ti_sysbios_family_arm_cc26xx_Boot_driverlibVersion

#include <ti/devices/DeviceFamily.h>

#include DeviceFamily_constructPath(driverlib/prcm.h)

#endif

#include "package/internal/Timer.xdc.h"

/* definitions from StellarisWare */
#define HWREG(x) (*((volatile unsigned long *)(x)))
#define SYSCTL_DID0                 0x400FE000
#define SYSCTL_DID0_CLASS_M         0x00FF0000
#define SYSCTL_DID0_CLASS_FLURRY    0x00090000

/* legacy reset, run, and sleep control registers */
#define SRCR1 ((volatile UInt32 *)0x400FE044) /* Software reset control 1 */
#define RCGC1 ((volatile UInt32 *)0x400FE104) /* Run mode Clock Gate 1 */
#define SCGC1 ((volatile UInt32 *)0x400FE114) /* Sleep mode Clock Gate 1 */
#define DCGC1 ((volatile UInt32 *)0x400FE124) /* Deep sleep mode Clock Gate 1 */

/* bitband alias for RCGC1 TIMER0 bit (used for pre-Flurry devices) */
#define RCGC1_BB_TIMER0 0x43FC20C0

/* newer reset, run, and sleep control registers */
#define SRTIMER     ((volatile UInt32 *)0x400FE504)
#define RCGCTIMERS  ((volatile UInt32 *)0x400FE604)
#define SCGCTIMERS  ((volatile UInt32 *)0x400FE704)
#define DCGCTIMERS  ((volatile UInt32 *)0x400FE804)

/* CC32xx ARCM GPT Clock Gating registers */
#define GPT_A0_CLK_GATING   (*(volatile UInt32 *)0x44025090)
#define GPT_A1_CLK_GATING   (*(volatile UInt32 *)0x44025098)
#define GPT_A2_CLK_GATING   (*(volatile UInt32 *)0x440250A0)
#define GPT_A3_CLK_GATING   (*(volatile UInt32 *)0x440250A8)

/* CC32xx ARCM GPT Soft Reset registers */
#define GPT_A0_SOFT_RESET   (*(volatile UInt32 *)0x44025094)
#define GPT_A1_SOFT_RESET   (*(volatile UInt32 *)0x4402509C)
#define GPT_A2_SOFT_RESET   (*(volatile UInt32 *)0x440250A4)
#define GPT_A3_SOFT_RESET   (*(volatile UInt32 *)0x440250AC)

/*
 *  ======== Timer_getNumTimers ========
 */
UInt Timer_getNumTimers()
{
    return (Timer_numTimerDevices);
}

/*
 *  ======== Timer_getStatus ========
 */
Timer_Status Timer_getStatus(UInt timerId)
{
    Assert_isTrue(timerId < Timer_numTimerDevices, NULL);

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
    return (0xffffffff/obj->period);
}

/*
 *  ======== Timer_setNextTick ========
 */
Void Timer_setNextTick(Timer_Object *obj, UInt32 ticks)
{
    ti_catalog_arm_peripherals_timers_TimerRegsM4 *timer;
    UInt32 now, next, prev;
    UInt32 countsPerTick = obj->period;
    UInt32 newPeriodCounts = countsPerTick * ticks;

    timer = (ti_catalog_arm_peripherals_timers_TimerRegsM4 *)
        Timer_module->device[obj->id].baseAddr;

    prev = obj->prevThreshold;

    /*
     * We could go negative here but we're unsigned so we'll roll over and end
     * up "bigger" than where we were...
     */

    /* next is relative to savedCurrCount */
    next = obj->savedCurrCount - newPeriodCounts;
    Timer_write(obj->altclk, &timer->GPTMTAMATCHR, next);

    now = timer->GPTMTAR;

    /*
     * If the next match is less than the previous match then we haven't
     * wrapped through 0 yet
     */
    if (next <= prev) {

        if (now <= next) {

            /*
             * The timer is counting down.  So if "now" is less than "next"
             * (or what would have been next) then we missed a match.  If we're
             * within one tick, fire the ISR
             */
            if ((next - now) < countsPerTick) {
                /* fire the isr... */
                *SRTIMER; Hwi_post(obj->intNum);
            }
        }
        else {

            /* If we're so far past the match that 'now' wrapped... */
            if ((now > prev) && (now > (next - countsPerTick))) {
                /* fire the isr... */
                *SRTIMER; Hwi_post(obj->intNum);
            }
        }
    }
    /* Else, next > prev, and timer will wrap thru 0 */
    else {

        /*
         * If next > prev then the match has wrapped through 0. If 'now' has
         * already passed the 'next' match and we're within one tick, fire the
         * isr...
         */
        if ((now < next) && (now < (next - countsPerTick))) {
            /* fire the isr... */
            *SRTIMER; Hwi_post(obj->intNum);
        }
    }
}

/*
 *  ======== Timer_Module_startup ========
 *  Calls postInit for all statically-created & constructed
 *  timers to initialize them.
 */
Int Timer_Module_startup(Int status)
{
    Int i;
    Timer_Object *obj;

    if (Timer_startupNeeded) {
        for (i = 0; i < Timer_numTimerDevices; i++) {
            obj = Timer_module->handles[i];
            /* if timer was statically created/constructed */
            if ((obj != NULL) && (obj->staticInst)) {
                Timer_postInit(obj, NULL);
            }
        }
    }
    return (Startup_DONE);
}

/*
 *  ======== Timer_startup ========
 *  Here after main(). Called from BIOS_start()
 */
Void Timer_startup()
{
    Int i;
    Timer_Object *obj;

    if (Timer_startupNeeded) {
        for (i = 0; i < Timer_numTimerDevices; i++) {
            obj = Timer_module->handles[i];
            /* if timer was statically created/constructed */
            if ((obj != NULL) && (obj->staticInst)) {
                if (obj->startMode == Timer_StartMode_AUTO) {
                        Timer_start(obj);
                }
            }
        }
    }
}

/*
 *  ======== Timer_getHandle ========
 */
Timer_Handle Timer_getHandle(UInt id)
{
    Assert_isTrue((id < Timer_numTimerDevices), NULL);
    return (Timer_module->handles[id]);
}

/*
 *  ======== Timer_Instance_init ========
 * 1. Select timer based on id
 * 2. Mark timer as in use
 * 3. Save timer handle if necessary
 * 4. Init obj using params
 * 5. Create Hwi if tickFxn !=NULL
 * 6. Timer_init()
 * 7. Timer configuration (wrt emulation, external frequency etc)
 * 8. Timer_setPeriod()
 * 9. Timer_start()
 */
Int Timer_Instance_init(Timer_Object *obj, Int id, Timer_FuncPtr tickFxn, const Timer_Params *params, Error_Block *eb)
{
    UInt key;
    Int i, status;
    Hwi_Params hwiParams;
    UInt tempId = 0xffff;

    if (id >= Timer_numTimerDevices) {
        if (id != Timer_ANY) {
            Error_raise(eb, Timer_E_invalidTimer, id, 0);
            return (1);
        }
    }

    key = Hwi_disable();

    if (id == Timer_ANY) {
        for (i = 0; i < Timer_numTimerDevices; i++) {
            if ((Timer_anyMask & (1 << i))
                && (Timer_module->availMask & (1 << i))) {
                Timer_module->availMask &= ~(1 << i);
                tempId = i;
                break;
            }
        }
    }
    else if (Timer_module->availMask & (1 << id)) {
        Timer_module->availMask &= ~(1 << id);
        tempId = id;
    }

    Hwi_restore(key);

    obj->staticInst = FALSE;

    if (tempId == 0xffff) {
        Error_raise(eb, Timer_E_notAvailable, id, 0);
        return (2);
    }
    else {
        obj->id = tempId;
    }

    obj->runMode = params->runMode;
    obj->startMode = params->startMode;

    if (params->altclk && !Timer_supportsAltclk) {
        Error_raise(eb, Timer_E_noaltclk, id, 0);
        return (1);
    }
    obj->altclk = params->altclk;

    obj->period = params->period;
    obj->periodType = params->periodType;

    if (obj->altclk) {         /* if using altclk the freq is always 16MHz */
        obj->extFreq.lo = 16000000;
        obj->extFreq.hi = 0;
    }
    else {                     /* else use specified extFreq */
        obj->extFreq.lo = params->extFreq.lo;
        obj->extFreq.hi = params->extFreq.hi;
    }

    obj->arg = params->arg;
    obj->intNum = Timer_module->device[obj->id].intNum;
    obj->tickFxn = tickFxn;
    obj->prevThreshold = params->prevThreshold;
    obj->rollovers = 0;
    obj->savedCurrCount = 0;


    if (obj->tickFxn) {
        if (params->hwiParams) {
            Hwi_Params_copy(&hwiParams, (params->hwiParams));
        }
        else {
            Hwi_Params_init(&hwiParams);
        }

        hwiParams.arg = (UArg)obj;

        obj->hwi = Hwi_create (obj->intNum, Timer_isrStub,
                                &hwiParams, eb);

        if (obj->hwi == NULL) {
            return (3);
        }
    }
    else {
        obj->hwi = NULL;
    }

    Timer_module->handles[obj->id] = obj;

    /* enable and reset the timer */
    Timer_enableFunc(obj->id);
    Timer_initDevice(obj);

    if (obj->periodType == Timer_PeriodType_MICROSECS) {
        if (!Timer_setPeriodMicroSecs(obj, obj->period)) {
            Error_raise(eb, Timer_E_cannotSupport, obj->period, 0);
            Hwi_restore(key);
            return (4);
        }
    }

    status = Timer_postInit(obj, eb);

    if (status) {
        return (status);
    }

    if (obj->startMode == Timer_StartMode_AUTO) {
        Timer_start(obj);
    }

    return (0);
}

/*
 *  ======== Timer_reconfig ========
 *  1. Init obj using params
 *  2. Reconfig Hwi
 *  3. Timer_init()
 *  4. Timer configuration (wrt emulation, external frequency etc)
 *  5. Timer_setPeriod()
 *  6. Timer_start()
 */
Void Timer_reconfig (Timer_Object *obj, Timer_FuncPtr tickFxn,
const Timer_Params *params, Error_Block *eb)
{
    Hwi_Params hwiParams;

    obj->runMode = params->runMode;
    obj->startMode = params->startMode;
    obj->altclk = params->altclk;
    obj->period = params->period;
    obj->periodType = params->periodType;
    obj->prevThreshold = params->prevThreshold;
    obj->rollovers = 0;
    obj->savedCurrCount = 0;

    if (obj->altclk) {         /* if using altclk the freq is always 16MHz */
        obj->extFreq.lo = 16000000;
        obj->extFreq.hi = 0;
    }
    else {                     /* else use specified extFreq */
        obj->extFreq.lo = params->extFreq.lo;
        obj->extFreq.hi = params->extFreq.hi;
    }

    if (obj->periodType == Timer_PeriodType_MICROSECS) {
        if (!Timer_setPeriodMicroSecs(obj, obj->period)) {
            Error_raise(eb, Timer_E_cannotSupport, obj->period, 0);
        }
    }

    obj->arg = params->arg;
    obj->tickFxn = tickFxn;

    if (obj->tickFxn) {
        if (params->hwiParams) {
            Hwi_Params_copy(&hwiParams, (params->hwiParams));
        }
        else {
            Hwi_Params_init(&hwiParams);
        }

        hwiParams.arg = (UArg)obj;

        Hwi_reconfig (obj->hwi, Timer_isrStub, &hwiParams);
    }

    Timer_postInit(obj, eb);

    if (obj->startMode == Timer_StartMode_AUTO) {
        Timer_start(obj);
    }
}

/*
 *  ======== Timer_postInit ========
 */
Int Timer_postInit(Timer_Object *obj, Error_Block *eb)
{
    UInt hwiKey;

    hwiKey = Hwi_disable();

    Timer_setPeriod(obj, obj->period);

    Hwi_restore(hwiKey);

    return (0);
}

/*
 *  ======== Timer_Instance_finalize ========
 */
Void Timer_Instance_finalize(Timer_Object *obj, Int status)
{
    UInt key;

    /* fall through in switch below is intentional */
    switch (status) {
        /* Timer_delete() */
        case 0:

        /* setPeriodMicroSecs failed */
        case 4:
            Timer_initDevice(obj);
            Timer_disableFunc(obj->id);

            if (obj->hwi) {
                Hwi_delete(&obj->hwi);
            }

        /* Hwi create failed */
        case 3:

        /* timer not available */
        case 2:

        /* invalid timer id */
        case 1:

        default:
            break;
    }

    key = Hwi_disable();
    Timer_module->availMask |= (0x1 << obj->id);
    Timer_module->handles[obj->id] = NULL;
    Hwi_restore(key);
}

/* ======== Timer_initDevice ========
 * 1. stop timer
 * 2. disable timer interrupt. (IER and any timer specific interrupt enable)
 * 3. clear pending interrupt. (IFR and any timer specific interrupt flags)
 * 4. Set control registers back to reset value.
 * 5. clear counters
 * 6. clear period register.
 */
Void Timer_initDevice(Timer_Object *obj)
{
    UInt key;
    ti_catalog_arm_peripherals_timers_TimerRegsM4 *timer;

    timer = (ti_catalog_arm_peripherals_timers_TimerRegsM4 *)
        Timer_module->device[obj->id].baseAddr;

    key = Hwi_disable();

    if (obj->hwi) {
        Hwi_disableInterrupt(obj->intNum);
        Hwi_clearInterrupt(obj->intNum);
    }

    /* mode setting purposely delayed a while after finishing reset */
    Timer_write(obj->altclk, &timer->GPTMCFG, 0); /* force 32 bit timer mode */

    Hwi_restore(key);
}

/*
 *  ======== Timer_start ========
 *  1. Hwi_disable();
 *  2. Clear the counters
 *  3. Clear IFR
 *  4. Enable timer interrupt
 *  5. Start timer
 *  6. Hwi_restore()
 */
Void Timer_start(Timer_Object *obj)
{
    UInt key;
    UInt32 amr;
    ti_catalog_arm_peripherals_timers_TimerRegsM4 *timer;

    timer = (ti_catalog_arm_peripherals_timers_TimerRegsM4 *)
        Timer_module->device[obj->id].baseAddr;

    key = Hwi_disable();

    /* stop timer */
    Timer_write(obj->altclk, &timer->GPTMCTL, timer->GPTMCTL & ~0x1);

    /* clear all of timer's interrupt status bits */
    Timer_write(obj->altclk, &timer->GPTMICR, (UInt32)0xFFFFFFFF);

    /* setup timer's Hwi */
    if (obj->hwi) {
        Hwi_clearInterrupt(obj->intNum);
        Hwi_enableInterrupt(obj->intNum);

        /* clear match and timeout enable bits */
        Timer_write(obj->altclk, &timer->GPTMIMR, timer->GPTMIMR & ~0x11);

        /* set appropriate interrupt enable based on timer mode */
        if (obj->runMode != Timer_RunMode_DYNAMIC) {
            /* unmask the timeout interrupt */
            Timer_write(obj->altclk, &timer->GPTMIMR, timer->GPTMIMR | 0x01);
        }
        else {
            /* unmask the match interrupt */
            Timer_write(obj->altclk, &timer->GPTMIMR, timer->GPTMIMR | 0x10);
        }
    }

    /* clear timer mode bits and match interrupt enable */
    amr = timer->GPTMTAMR & ~0x23;

    /* Timer_RunMode_CONTINUOUS */
    if (obj->runMode == Timer_RunMode_CONTINUOUS) {
        /* sub 1 from period to compensate for extra count during reload */
        Timer_write(obj->altclk, &timer->GPTMTAILR, obj->period - 1);
        Timer_write(obj->altclk, &timer->GPTMTAMR, amr | 0x2); /* periodic */
    }

    /* Timer_RunMode_DYNAMIC */
    else if (obj->runMode == Timer_RunMode_DYNAMIC) {
        obj->prevThreshold = Timer_MAX_PERIOD;
        Timer_write(obj->altclk, &timer->GPTMTAV, Timer_MAX_PERIOD);
        Timer_write(obj->altclk, &timer->GPTMTAMATCHR,
            Timer_MAX_PERIOD - obj->period);
        Timer_write(obj->altclk, &timer->GPTMTAMR, amr | 0x22);/* prd & match */
    }

    /* Timer_RunMode_ONESHOT */
    else {
        Timer_write(obj->altclk, &timer->GPTMTAILR, obj->period);
        Timer_write(obj->altclk, &timer->GPTMTAMR, amr | 0x1);  /* one-shot */
    }

    if (obj->altclk) {
        timer->GPTMCC = 1; /* note: this write not affected by erratum */
    }

    /* configure timer to halt with debugger, and start it */
    Timer_write(obj->altclk, &timer->GPTMCTL, timer->GPTMCTL | 0x3);

    Hwi_restore(key);
}

/*
 *  ======== Timer_trigger ========
 *  1. stop timer
 *  2. write the period with insts
 *  3. start the timer.
 */
Void Timer_trigger(Timer_Object *obj, UInt32 insts)
{
    UInt key;

    /* follow proper procedure for dynamic period change */
    key = Hwi_disable();
    Timer_stop(obj);
    Timer_setPeriod(obj, insts);
    Timer_start(obj);
    Hwi_restore(key);
}

/*
 *  ======== Timer_stop ========
 *  1. stop timer
 *  2. disable timer interrupt
 */
Void Timer_stop(Timer_Object *obj)
{
    ti_catalog_arm_peripherals_timers_TimerRegsM4 *timer;
    timer = (ti_catalog_arm_peripherals_timers_TimerRegsM4 *)Timer_module->device[obj->id].baseAddr;

    /* stop timer by clearing bit0 (TAEN) */
    Timer_write(obj->altclk, &timer->GPTMCTL, timer->GPTMCTL & ~0x1);

    if (obj->hwi) {
        Hwi_disableInterrupt(obj->intNum);
    }
}

/*
 *  ======== Timer_setPeriod ========
 *  1. stop timer
 *  2. set period
 */
Void Timer_setPeriod(Timer_Object *obj, UInt32 period)
{
    Timer_stop(obj);
    obj->period = period;
}

/*
 *  ======== Timer_setPeriodMicroSecs ========
 *  1. stop timer
 *  2. compute counts
 *  3. set period
 */
Bool Timer_setPeriodMicroSecs(Timer_Object *obj, UInt32 period)
{
    Types_FreqHz freqHz;
    UInt64 counts;
    UInt32 freqKHz;

    Timer_stop(obj);

    Timer_getFreq(obj, &freqHz);
    freqKHz = freqHz.lo / 1000;

    counts = ((UInt64)freqKHz * (UInt64)period) / (UInt64)1000;

    obj->period = counts;
    obj->periodType = Timer_PeriodType_COUNTS;

    return(TRUE);
}

/*
 *  ======== Timer_getPeriod ========
 */
UInt32 Timer_getPeriod(Timer_Object *obj)
{
    return (obj->period);
}

/*
 *  ======== Timer_getCount ========
 */
UInt32 Timer_getCount(Timer_Object *obj)
{
    ti_catalog_arm_peripherals_timers_TimerRegsM4 *timer;

    timer = (ti_catalog_arm_peripherals_timers_TimerRegsM4 *)Timer_module->device[obj->id].baseAddr;

    return (timer->GPTMTAR);
}

/*
 *  ======== Timer_getExpiredCounts64 ========
 *  This API is used by the TimestampProvider as part of retrieving a timestamp
 *  using a timer and a tick counter. It returns the timer's count but also
 *  accounts for timer rollover.
 *
 *  This API must be called with interrupts disabled (the TimestampProvider
 *  should disable interrupts while retrieving the tick count and calling this
 *  API).
 *
 *  The TimestampProvider uses a 32-bit timer and 32-bit tick count to track
 *  the timestamp. The tick count either comes from the Clock module or is
 *  stored in the TimestampProvider's module state and incremented by an ISR
 *  when the timer expires.
 *
 *  This approach has a difficult edge case which this API addresses.
 *  Timestamp_get64 may be called while interrupts are disabled, and while they
 *  are disabled, the timer may expire and reset to its initial period. Because
 *  interrupts are disabled, the tick count isr has not run yet to increment
 *  the tick count. This can result in the occassional timestamp value going
 *  backwards in time because the upper bits are out of date.
 *
 *  To work around this, we need to detect the timer "rollover" and account for
 *  it by adding the timer period to the count returned.
 *
 *  To detect the rollover, we retrieve the count, check the IFR flag for the
 *  timer interrupt, then check the count again, all with interrupts disabled
 *  (the caller should disable interrupts).
 *
 *    Hwi_disable();
 *
 *    count1 = timer.TIM;
 *    ifrFlag = getIFRFlag();
 *    count2 = timer.TIM;
 *
 *    Hwi_restore();
 *
 *  For the most efficient implementation, we access the Timer register value
 *  directly, which means the counter value is counting *down* on the lm4 timer.
 *  This means that most of the time count1 > count2.
 *
 *  The following table lists the possible values of count1, count2, and
 *  ifrFlag. The third column states whether we would need to add the timer
 *  period to the result if we return count1. The fourth column states the
 *  same for count2. Count1 and count2 will be very close together, so either
 *  is acceptable to return.
 *
 *                                       Add prd to     Add prd to
 *         compare           IFR flag      count1         count2
 *  1. (count1 > count2)        0            NO             NO
 *  2. (count1 > count2)        1            YES            YES
 *  3. (count1 < count2)        0            NO             YES
 *  4. (count1 < count2)        1            NO             YES
 *
 *  1. Case 1 is by far the typical case. We're "in the middle" of the count,
 *     not close to a counter rollover, and we just return the count.
 *  2. Case 2 means that the timer rolled over before we retrieved count1, but
 *     that interrupts were disabled, so the tick isr hasn't run yet. When an
 *     isr is serviced, the hardware clears the IFR bit immediately, so it is
 *     not possible that we are in the middle of servicing the tick isr.
 *  3. Case 3 is rare. This means that the timer rolled over after checking
 *     the IFR flag but before retrieving count2.
 *  4. Case 4 is rare. This means that the timer rolled over after retrieving
 *     count1, but before we check the IFR flag.
 *
 *  Case 3 is the reason it's not sufficient to simply check the IFR flag, and
 *  case 2 is the reason it's not sufficient to simply compare count1 and
 *  count2.
 *
 *  Returning count1 appears to mean less additions, so why return count2?
 *    - The intent of the logic is more apparent in the if statement.
 *      "If an interrupt occurred OR count2 is out of sequence with count1,
 *      a rollover occurred, so add the period".
 *    - Case 2 is the most common rollover case, and the performance for case 2
 *      is the same whether we return count1 or count2.
 */
UInt64 Timer_getExpiredCounts64(Timer_Object *obj)
{
    ti_catalog_arm_peripherals_timers_TimerRegsM4 *timer;
    UInt32 count1, count2;
    UInt32 intr1, intr2;
    UInt32 wrap;

    timer = (ti_catalog_arm_peripherals_timers_TimerRegsM4 *)
        Timer_module->device[obj->id].baseAddr;

    if (obj->runMode != Timer_RunMode_DYNAMIC) {
        count1 = timer->GPTMTAR;
        wrap = timer->GPTMRIS & 0x1; /* roll-over occurred */
        count2 = timer->GPTMTAR;

        if ((count1 > count2) && wrap) {
            return ((UInt64)(obj->period - count1) + (UInt64)obj->period);
        }
        else {
            return ((UInt64)(obj->period - count1));
        }
    }
    else {
        UInt32 thresh;
        UInt64 result;

        intr1 = timer->GPTMRIS & 0x10;
        count1 = timer->GPTMTAR;
        intr2 = timer->GPTMRIS & 0x10;

        /* if the thresh interrupt was set before reading the timer ... */
        if (intr1) {
            thresh = timer->GPTMTAMATCHR;

            /* passed the threshold but not wrapped yet */
            if (count1 <= thresh) {
                result = (UInt64)(thresh - count1) + (UInt64)obj->period;
            }

            /* wrapped */
            else {
                result = (UInt64)(Timer_MAX_PERIOD - count1) + (UInt64)thresh + (UInt64)obj->period;
            }
        }

        /* else if the thresh interrupt popped after reading the timer ... */
        else if (intr2) {
            result = obj->period;
        }

        /* else if thresh not reached and and haven't wrapped thru 0 ... */
        else if (count1 < obj->prevThreshold) {
            result = obj->prevThreshold - count1;
        }

        /* else thresh not reached but DID wrap thru 0 */
        else {
            result = (UInt64)(Timer_MAX_PERIOD - count1) + (UInt64)obj->prevThreshold;
        }
        return(result);
    }
}

/*
 *  ======== Timer_getExpiredCounts ========
 */
UInt32 Timer_getExpiredCounts(Timer_Object *obj)
{
    return (Timer_getExpiredCounts64(obj));
}

/*
 *  ======== Timer_getExpiredTicks ========
 */
UInt32 Timer_getExpiredTicks(Timer_Object *obj, UInt32 tickPeriod)
{
    UInt32 ticks;

    ticks = Timer_getExpiredCounts(obj) / tickPeriod;

    return (ticks);
}

/*
 *  ======== Timer_getCurrentTick ========
 */
UInt32 Timer_getCurrentTick(Timer_Object *obj, Bool saveFlag)
{
    UInt32 tick, rollovers;
    UInt32 currCount;
    UInt64 longCount;
    
    currCount = Timer_getCount(obj);

    rollovers = obj->rollovers;

    /*
     * If the current count is greater than the previous
     * threshold than a wrap has occurred
     */
    if (currCount > obj->prevThreshold) {
        rollovers += 1;
    }

    longCount = (UInt64)(rollovers) << 32;
    longCount += Timer_MAX_PERIOD - currCount;
    
    tick = longCount / (UInt64)(obj->period);
    
    /* save for later use by Timer_setNextTick() */
    if (saveFlag != 0) {
        obj->savedCurrCount = currCount;
    }        
 
    return (tick);
}

/*
 *  ======== Timer_getFreq ========
 *  get timer prd frequency in Hz.
 */
Void Timer_getFreq(Timer_Object *obj, Types_FreqHz *freq)
{
    if (obj->extFreq.lo != 0U) {
        *freq = obj->extFreq;
    }
    else {
        BIOS_getCpuFreq(freq);
    }
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

    Hwi_setFunc(obj->hwi, fxn, arg);
}

/*
 *  ======== Timer_isrStub ========
 */
Void Timer_isrStub(UArg arg)
{
    ti_catalog_arm_peripherals_timers_TimerRegsM4 *timer;
    Timer_Object *obj = (Timer_Object *)arg;

    timer = (ti_catalog_arm_peripherals_timers_TimerRegsM4 *)
        Timer_module->device[obj->id].baseAddr;

    /* clear all timer interrupt status bits */
    Timer_write(obj->altclk, &timer->GPTMICR, 0xFFFFFFFF);

    /* for DYNAMIC, mode latch prevThreshold and detect rollovers */
    if (obj->runMode == Timer_RunMode_DYNAMIC) {
        /* 
         * if the current threshold is greater than the prevThreshold
         * then a rollover has occurred.
         */
        if (obj->prevThreshold < timer->GPTMTAMATCHR) {
            obj->rollovers += 1;
        }
        obj->prevThreshold = timer->GPTMTAMATCHR;
    }

    obj->tickFxn(obj->arg);
}

/*
 *  ======== Timer_enableTimers ========
 */
Void Timer_enableTimers()
{
    Int i;
    Timer_Object *obj;

    for (i = 0; i < Timer_numTimerDevices; i++) {
        obj = Timer_module->handles[i];

        /* enable and reset the timer */
        if (obj != NULL) {
            Timer_enableFunc(obj->id);
            Timer_initDevice(obj);
        }
    }
}

/*
 *  ======== Timer_write ========
 *  This function exists to work around an erratum regarding the writing of
 *  timer registers when the timer module is being driven off altclk.  The value
 *  to be written must be held on the dbus for sufficient time for the timer
 *  register to latch the value.  The only way to guarantee this is to have
 *  interrupts disabled while we hold the value on the bus.  Since the Hwi
 *  module masks interrupts by elevating BASEPRI, we must use the CPSID/CPSIE
 *  instructions to toggle PRIMASK instead.
 */
Void Timer_write(Bool altclk, volatile UInt32 *pReg, UInt32 val)
{
    Bool intsDisabled;

    /* if running on altclk do workaround for erratum */
    if (altclk) {
        intsDisabled = Timer_masterDisable();
        *pReg = val;
        *pReg = val;
        *pReg = val;
        *pReg = val;
        *pReg = val;
        if (!intsDisabled) {
            Timer_masterEnable();
        }
    }

    /* else just write the timer register */
    else {
        *pReg = val;
    }
}

/*
 *  ======== Timer_masterDisable ========
 */
#if (defined(__GNUC__) && !defined(__ti__)) || (defined(__GNUC__) && defined(__clang__))
Bool __attribute__((naked)) Timer_masterDisable(Void)
{
    /* read PRIMASK bit to R0 and call CPSID to disable interrupts */
    __asm__ __volatile__(
          "    mrs    r0, PRIMASK \n"
          "    cpsid  i \n"
          "    bx     lr \n");
}
#elif defined(__ti__)
    /* read PRIMASK bit to R0 and call CPSID to disable interrupts */
    __asm("    .sect \".text:ti_sysbios_family_arm_lm4_Timer_masterDisable__I\" \n"
          "    .clink \n"
          "    .global ti_sysbios_family_arm_lm4_Timer_masterDisable__I \n"
          "    .thumbfunc ti_sysbios_family_arm_lm4_Timer_masterDisable__I \n"
          "    .thumb \n"
          "ti_sysbios_family_arm_lm4_Timer_masterDisable__I: \n"
          "    mrs    r0, PRIMASK \n"
          "    cpsid  i \n"
          "    bx     lr \n");
#elif defined(__IAR_SYSTEMS_ICC__)
Bool Timer_masterDisable(Void)
{
    Bool retVal;

    retVal = (Bool)__get_interrupt_state();
    __disable_interrupt();

    return retVal;
}
#endif

/*
 *  ======== Timer_masterEnable ========
 */
#if (defined(__GNUC__) && !defined(__ti__)) || (defined(__GNUC__) && defined(__clang__))
Void __attribute__((naked)) Timer_masterEnable(Void)
{
    __asm__ __volatile__(
          "    cpsie  i \n"
          "    bx     lr \n");
}
#elif defined(__ti__)
    __asm("    .sect \".text:ti_sysbios_family_arm_lm4_Timer_masterEnable__I\" \n"
          "    .clink \n"
          "    .global ti_sysbios_family_arm_lm4_Timer_masterEnable__I \n"
          "    .thumbfunc ti_sysbios_family_arm_lm4_Timer_masterEnable__I \n"
          "    .thumb \n"
          "ti_sysbios_family_arm_lm4_Timer_masterEnable__I: \n"
          "    cpsie  i \n"
          "    bx     lr \n");
#elif defined(__IAR_SYSTEMS_ICC__)
Void Timer_masterEnable(Void)
{
    __enable_interrupt();
}
#endif

#ifdef ti_sysbios_family_arm_cc26xx_Boot_driverlibVersion
/*
 *  ======== Timer_enableCC26xx ========
 *  Note: This function implementation is intended only for test cases where
 *  the Power manager module (in ti/drivers/power) is not being used.  This
 *  function will directly enable a GPT so that it can be programmed,
 *  without giving any considerations to power management of the PERIPH domain
 *  and the GPT clocks.
 */
#define INVALID_TIMER_ID ~0
#if defined(__IAR_SYSTEMS_ICC__)
__weak Void Timer_enableCC26xx(Int id)
#elif (defined(__GNUC__) && !defined(__ti__)) || (defined(__GNUC__) && defined(__clang__))
Void __attribute__((weak)) Timer_enableCC26xx(Int id)
#else
#pragma WEAK (Timer_enableCC26xx)
Void Timer_enableCC26xx(Int id)
#endif
{
    UInt gpt = INVALID_TIMER_ID;

    /* map timer ID to the appropriate driverlib ID */
    switch (id) {
        case 0: gpt = PRCM_PERIPH_TIMER0;
                break;

        case 1: gpt = PRCM_PERIPH_TIMER1;
                break;

        case 2: gpt = PRCM_PERIPH_TIMER2;
                break;

        case 3: gpt = PRCM_PERIPH_TIMER3;
                break;

        default:
                break;
    }

    /* if a valid GPT timer ID, enable the GPT ... */
    if (gpt != INVALID_TIMER_ID) {

        /* if it is not already on, turn on the PERIPH domain */
        if (PRCMPowerDomainsAllOn(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON) {
            PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
            while (PRCMPowerDomainsAllOn(PRCM_DOMAIN_PERIPH) !=
                PRCM_DOMAIN_POWER_ON) {};
        }

        /* now enable the GPT clocks */
        PRCMPeripheralRunEnable(gpt);
        PRCMPeripheralSleepEnable(gpt);
        PRCMPeripheralDeepSleepEnable(gpt);
        PRCMLoadSet();
        while(!PRCMLoadGet()){};
    }
}
#endif

/*
 *  ======== Timer_enableCC3200 ========
 */
#if defined(__IAR_SYSTEMS_ICC__)
__weak Void Timer_enableCC3200(Int id)
#elif (defined(__GNUC__) && !defined(__ti__)) || (defined(__GNUC__) && defined(__clang__))
Void __attribute__((weak)) Timer_enableCC3200(Int id)
#else
#pragma WEAK (Timer_enableCC3200)
Void Timer_enableCC3200(Int id)
#endif
{
    UInt key;

    key = Hwi_disable();

    switch (id) {
        case 0: GPT_A0_CLK_GATING |= 0x1;
                GPT_A0_SOFT_RESET |= 0x1;
                GPT_A0_SOFT_RESET &= ~(0x1);
                break;
        case 1: GPT_A1_CLK_GATING |= 0x1;
                GPT_A1_SOFT_RESET |= 0x1;
                GPT_A1_SOFT_RESET &= ~(0x1);
                break;
        case 2: GPT_A2_CLK_GATING |= 0x1;
                GPT_A2_SOFT_RESET |= 0x1;
                GPT_A2_SOFT_RESET &= ~(0x1);
                break;
        case 3: GPT_A3_CLK_GATING |= 0x1;
                GPT_A3_SOFT_RESET |= 0x1;
                GPT_A3_SOFT_RESET &= ~(0x1);
                break;
        default:
                break;
    }

    Hwi_restore(key);
}

/*
 *  ======== Timer_enableTiva ========
 */
Void Timer_enableTiva(Int id)
{
    UInt key;

    key = Hwi_disable();

    /* if a pre-Flurry class device, and one of the first four timers ... */
    if (((HWREG(SYSCTL_DID0) & SYSCTL_DID0_CLASS_M) <
        SYSCTL_DID0_CLASS_FLURRY) && (id < 4)) {

        /* enable run mode clock (per SYSBIOS-185, use bitband alias) */
        *((UInt32 *)(RCGC1_BB_TIMER0 + (id * 4))) = 1;

        /* ensure at least 5 clock cycle delay for clock enable */
        *RCGC1;
        *RCGC1;
        *RCGC1;
        *RCGC1;
        *RCGC1;

        /* do a sw reset on the timer */
        *SRCR1 |= (UInt32)(1 << (id + 16));
        *SRCR1 &= ~(UInt32)(1 << (id + 16));
    }
    /* else, Flurry or later device, or 5th timer or above ... */
    else {
        /* enable run mode clock */
        *RCGCTIMERS |= (UInt32)(1 << id);
        *SCGCTIMERS |= (UInt32)(1 << id);
        *DCGCTIMERS |= (UInt32)(1 << id);

        /* ensure at least 5 clock cycle delay for clock enable */
        *RCGCTIMERS;
        *RCGCTIMERS;
        *RCGCTIMERS;
        *RCGCTIMERS;
        *RCGCTIMERS;

        /* do a sw reset on the timer */
        *SRTIMER |= (UInt32)(1 << id);
        *SRTIMER &= ~(UInt32)(1 << id);
    }

    Hwi_restore(key);
}

#ifdef ti_sysbios_family_arm_cc26xx_Boot_driverlibVersion
/*
 *  ======== Timer_disableCC26xx ========
 */
#if defined(__IAR_SYSTEMS_ICC__)
__weak Void Timer_disableCC26xx(Int id)
#elif (defined(__GNUC__) && !defined(__ti__)) || (defined(__GNUC__) && defined(__clang__))
Void __attribute__((weak)) Timer_disableCC26xx(Int id)
#else
#pragma WEAK (Timer_disableCC26xx)
Void Timer_disableCC26xx(Int id)
#endif
{
}
#endif

/*
 *  ======== Timer_disableCC3200 ========
 */
#if defined(__IAR_SYSTEMS_ICC__)
__weak Void Timer_disableCC3200(Int id)
#elif (defined(__GNUC__) && !defined(__ti__)) || (defined(__GNUC__) && defined(__clang__))
Void __attribute__((weak)) Timer_disableCC3200(Int id)
#else
#pragma WEAK (Timer_disableCC3200)
Void Timer_disableCC3200(Int id)
#endif
{
    UInt key;

    key = Hwi_disable();

    switch (id) {
        case 0: GPT_A0_CLK_GATING &= ~(0x1);
                break;
        case 1: GPT_A1_CLK_GATING &= ~(0x1);
                break;
        case 2: GPT_A2_CLK_GATING &= ~(0x1);
                break;
        case 3: GPT_A3_CLK_GATING &= ~(0x1);
                break;
        default:
                break;
    }

    Hwi_restore(key);
}

/*
 *  ======== Timer_disableTiva ========
 */
Void Timer_disableTiva(Int id)
{
    UInt key;

    key = Hwi_disable();

    //TODO: Can we remove the delays from the disable code ?

    /* if a pre-Flurry class device, and one of the first four timers ... */
    if (((HWREG(SYSCTL_DID0) & SYSCTL_DID0_CLASS_M) <
        SYSCTL_DID0_CLASS_FLURRY) && (id < 4)) {

        /* disable run mode clock (per SYSBIOS-185, use bitband alias) */
        *((UInt32 *)(RCGC1_BB_TIMER0 + (id * 4))) = 0;

        /* ensure at least 5 clock cycle delay for clock disable */
        *RCGC1;
        *RCGC1;
        *RCGC1;
        *RCGC1;
        *RCGC1;
    }
    /* else, Flurry or later device, or 5th timer or above ... */
    else {
        /* enable run mode clock */
        *RCGCTIMERS &= ~(UInt32)(1 << id);
        *SCGCTIMERS &= ~(UInt32)(1 << id);
        *DCGCTIMERS &= ~(UInt32)(1 << id);

        /* ensure at least 5 clock cycle delay for clock disable */
        *RCGCTIMERS;
        *RCGCTIMERS;
        *RCGCTIMERS;
        *RCGCTIMERS;
        *RCGCTIMERS;
    }

    Hwi_restore(key);
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
    UInt i;
    UInt key;
    UInt tmpMask;

    key = Hwi_disable();
    tmpMask = mask;
    for (i = 0; i < Timer_numTimerDevices; i++) {
        /* Check if mask is setting any currently used timer as available */
        if ((tmpMask & 0x1) && (Timer_module->handles[i] != NULL)) {
            Hwi_restore(key);
            return (FALSE);
        }
        tmpMask = tmpMask >> 1;
    }
    Timer_module->availMask = mask;
    Hwi_restore(key);

    return (TRUE);
}
