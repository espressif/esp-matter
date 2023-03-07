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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/Types.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>

#include "TimerRegs.h"

#if defined(__IAR_SYSTEMS_ICC__)

#include <intrinsics.h>

#endif

#ifdef ti_sysbios_family_arm_cc26xx_Boot_driverlibVersion

#include <ti/devices/DeviceFamily.h>

#include DeviceFamily_constructPath(driverlib/prcm.h)

#endif

#include <ti/sysbios/family/arm/lm4/Timer.h>

/* definitions from StellarisWare */
#ifndef HWREG
#define HWREG(x) (*((volatile uint32_t *)(x)))
#endif
#define SYSCTL_DID0                 0x400FE000
#define SYSCTL_DID0_CLASS_M         0x00FF0000
#define SYSCTL_DID0_CLASS_FLURRY    0x00090000

/* legacy reset, run, and sleep control registers */
#define SRCR1 ((volatile uint32_t *)0x400FE044) /* Software reset control 1 */
#define RCGC1 ((volatile uint32_t *)0x400FE104) /* Run mode Clock Gate 1 */
#define SCGC1 ((volatile uint32_t *)0x400FE114) /* Sleep mode Clock Gate 1 */
#define DCGC1 ((volatile uint32_t *)0x400FE124) /* Deep sleep mode Clock Gate 1 */

/* bitband alias for RCGC1 TIMER0 bit (used for pre-Flurry devices) */
#define RCGC1_BB_TIMER0 0x43FC20C0

/* newer reset, run, and sleep control registers */
#define SRTIMER     ((volatile uint32_t *)0x400FE504)
#define RCGCTIMERS  ((volatile uint32_t *)0x400FE604)
#define SCGCTIMERS  ((volatile uint32_t *)0x400FE704)
#define DCGCTIMERS  ((volatile uint32_t *)0x400FE804)

/* CC32xx ARCM GPT Clock Gating registers */
#define GPT_A0_CLK_GATING   (*(volatile uint32_t *)0x44025090)
#define GPT_A1_CLK_GATING   (*(volatile uint32_t *)0x44025098)
#define GPT_A2_CLK_GATING   (*(volatile uint32_t *)0x440250A0)
#define GPT_A3_CLK_GATING   (*(volatile uint32_t *)0x440250A8)

/* CC32xx ARCM GPT Soft Reset registers */
#define GPT_A0_SOFT_RESET   (*(volatile uint32_t *)0x44025094)
#define GPT_A1_SOFT_RESET   (*(volatile uint32_t *)0x4402509C)
#define GPT_A2_SOFT_RESET   (*(volatile uint32_t *)0x440250A4)
#define GPT_A3_SOFT_RESET   (*(volatile uint32_t *)0x440250AC)

/*
 * CC13/14/26/27 Timer Device Info
 */
Timer_TimerDevice TimerDevices_CC26XX[4] = {
    {
        .intNum = 31,
        .baseAddr = (void *)0x40010000
    },
    {
        .intNum = 33,
        .baseAddr = (void *)0x40011000
    },
    {
        .intNum = 35,
        .baseAddr = (void *)0x40012000
    },
    {
        .intNum = 37,
        .baseAddr = (void *)0x40013000
    }
};

/*
 * CC32XX Timer Device Info
 */
Timer_TimerDevice TimerDevices_CC32XX[4] = {
    {
        .intNum = 35,
        .baseAddr = (void *)0x40030000
    },
    {
        .intNum = 37,
        .baseAddr = (void *)0x40031000
    },
    {
        .intNum = 39,
        .baseAddr = (void *)0x40032000
    },
    {
        .intNum = 51,
        .baseAddr = (void *)0x40033000
    }
};

/* Params */
static const struct Timer_Params Timer_Params_default = {
    .runMode = Timer_RunMode_CONTINUOUS,
    .startMode = Timer_StartMode_AUTO,
    .arg = 0,
    .period = 1000,
    .periodType = Timer_PeriodType_MICROSECS,
    .extFreq.lo = 0,
    .extFreq.hi = 0,
    .hwiParams = NULL,
    .prevThreshold = 0,
    .altclk = false,
};

Timer_Handle Timer_handles[Timer_NUM_LM4_TIMERS_D];

Timer_Module_State Timer_Module_state = {
    .objQ.next = &Timer_Module_state.objQ,
    .objQ.prev = &Timer_Module_state.objQ,
    .availMask = (1 << Timer_NUM_LM4_TIMERS_D) - 1,
    .device = Timer_timerDevices_D,
    .handles = {(Timer_Handle)Timer_handles}
};

/* startupNeeded */
const unsigned int Timer_startupNeeded;
const int Timer_numTimerDevices = Timer_NUM_LM4_TIMERS_D;
const bool Timer_supportsAltclk;
const unsigned int Timer_anyMask = (1 << Timer_NUM_LM4_TIMERS_D) - 1;
const Timer_TimerDisableFuncPtr Timer_disableFunc = Timer_disableFunc_D;
const Timer_TimerEnableFuncPtr Timer_enableFunc = Timer_enableFunc_D;

/*
 *  ======== Timer_getNumTimers ========
 */
unsigned int Timer_getNumTimers()
{
    return (Timer_numTimerDevices);
}

/*
 *  ======== Timer_getStatus ========
 */
Timer_Status Timer_getStatus(unsigned int timerId)
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
uint32_t Timer_getMaxTicks(Timer_Object *obj)
{
    return (0xffffffff/obj->period);
}

/*
 *  ======== Timer_setNextTick ========
 */
void Timer_setNextTick(Timer_Object *obj, uint32_t ticks)
{
    ti_catalog_arm_peripherals_timers_TimerRegsM4 *timer;
    uint32_t now, next, prev;
    uint32_t countsPerTick = obj->period;
    uint32_t newPeriodCounts = countsPerTick * ticks;

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
 *  ======== Timer_startup ========
 *  Here after main(). Called from BIOS_start()
 */
void Timer_startup()
{
    int i;
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
Timer_Handle Timer_getHandle(unsigned int id)
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
int Timer_Instance_init(Timer_Object *obj, int id, Timer_FuncPtr tickFxn, const Timer_Params *params, Error_Block *eb)
{
    unsigned int key;
    int i, status;
    Hwi_Params hwiParams;
    unsigned int tempId = 0xffff;

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

    obj->staticInst = false;

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

        hwiParams.arg = (uintptr_t)obj;

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

    /* put timer on global timer list (Queue_put is atomic) */
    Queue_put(&Timer_module->objQ, &obj->objElem);

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
void Timer_reconfig (Timer_Object *obj, Timer_FuncPtr tickFxn,
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

        hwiParams.arg = (uintptr_t)obj;

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
int Timer_postInit(Timer_Object *obj, Error_Block *eb)
{
    unsigned int hwiKey;

    hwiKey = Hwi_disable();

    Timer_setPeriod(obj, obj->period);

    Hwi_restore(hwiKey);

    return (0);
}

/*
 *  ======== Timer_Instance_finalize ========
 */
void Timer_Instance_finalize(Timer_Object *obj, int status)
{
    unsigned int key;

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
void Timer_initDevice(Timer_Object *obj)
{
    unsigned int key;
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
void Timer_start(Timer_Object *obj)
{
    unsigned int key;
    uint32_t amr;
    ti_catalog_arm_peripherals_timers_TimerRegsM4 *timer;

    timer = (ti_catalog_arm_peripherals_timers_TimerRegsM4 *)
        Timer_module->device[obj->id].baseAddr;

    key = Hwi_disable();

    /* stop timer */
    Timer_write(obj->altclk, &timer->GPTMCTL, timer->GPTMCTL & ~0x1);

    /* clear all of timer's interrupt status bits */
    Timer_write(obj->altclk, &timer->GPTMICR, (uint32_t)0xFFFFFFFF);

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
void Timer_trigger(Timer_Object *obj, uint32_t insts)
{
    unsigned int key;

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
void Timer_stop(Timer_Object *obj)
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
void Timer_setPeriod(Timer_Object *obj, uint32_t period)
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
bool Timer_setPeriodMicroSecs(Timer_Object *obj, uint32_t period)
{
    Types_FreqHz freqHz;
    uint64_t counts;
    uint32_t freqKHz;

    Timer_stop(obj);

    Timer_getFreq(obj, &freqHz);
    freqKHz = freqHz.lo / 1000;

    counts = ((uint64_t)freqKHz * (uint64_t)period) / (uint64_t)1000;

    obj->period = counts;
    obj->periodType = Timer_PeriodType_COUNTS;

    return(true);
}

/*
 *  ======== Timer_getPeriod ========
 */
uint32_t Timer_getPeriod(Timer_Object *obj)
{
    return (obj->period);
}

/*
 *  ======== Timer_getCount ========
 */
uint32_t Timer_getCount(Timer_Object *obj)
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
uint64_t Timer_getExpiredCounts64(Timer_Object *obj)
{
    ti_catalog_arm_peripherals_timers_TimerRegsM4 *timer;
    uint32_t count1, count2;
    uint32_t intr1, intr2;
    uint32_t wrap;

    timer = (ti_catalog_arm_peripherals_timers_TimerRegsM4 *)
        Timer_module->device[obj->id].baseAddr;

    if (obj->runMode != Timer_RunMode_DYNAMIC) {
        count1 = timer->GPTMTAR;
        wrap = timer->GPTMRIS & 0x1; /* roll-over occurred */
        count2 = timer->GPTMTAR;

        if ((count1 > count2) && wrap) {
            return ((uint64_t)(obj->period - count1) + (uint64_t)obj->period);
        }
        else {
            return ((uint64_t)(obj->period - count1));
        }
    }
    else {
        uint32_t thresh;
        uint64_t result;

        intr1 = timer->GPTMRIS & 0x10;
        count1 = timer->GPTMTAR;
        intr2 = timer->GPTMRIS & 0x10;

        /* if the thresh interrupt was set before reading the timer ... */
        if (intr1) {
            thresh = timer->GPTMTAMATCHR;

            /* passed the threshold but not wrapped yet */
            if (count1 <= thresh) {
                result = (uint64_t)(thresh - count1) + (uint64_t)obj->period;
            }

            /* wrapped */
            else {
                result = (uint64_t)(Timer_MAX_PERIOD - count1) + (uint64_t)thresh + (uint64_t)obj->period;
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
            result = (uint64_t)(Timer_MAX_PERIOD - count1) + (uint64_t)obj->prevThreshold;
        }
        return(result);
    }
}

/*
 *  ======== Timer_getExpiredCounts ========
 */
uint32_t Timer_getExpiredCounts(Timer_Object *obj)
{
    return (Timer_getExpiredCounts64(obj));
}

/*
 *  ======== Timer_getExpiredTicks ========
 */
uint32_t Timer_getExpiredTicks(Timer_Object *obj, uint32_t tickPeriod)
{
    uint32_t ticks;

    ticks = Timer_getExpiredCounts(obj) / tickPeriod;

    return (ticks);
}

/*
 *  ======== Timer_getCurrentTick ========
 */
uint32_t Timer_getCurrentTick(Timer_Object *obj, bool saveFlag)
{
    uint32_t tick, rollovers;
    uint32_t currCount;
    uint64_t longCount;

    currCount = Timer_getCount(obj);

    rollovers = obj->rollovers;

    /*
     * If the current count is greater than the previous
     * threshold than a wrap has occurred
     */
    if (currCount > obj->prevThreshold) {
        rollovers += 1;
    }

    longCount = (uint64_t)(rollovers) << 32;
    longCount += Timer_MAX_PERIOD - currCount;

    tick = longCount / (uint64_t)(obj->period);

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
void Timer_getFreq(Timer_Object *obj, Types_FreqHz *freq)
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

    Hwi_setFunc(obj->hwi, fxn, arg);
}

/*
 *  ======== Timer_isrStub ========
 */
void Timer_isrStub(uintptr_t arg)
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
void Timer_enableTimers()
{
    int i;
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
void Timer_write(bool altclk, volatile uint32_t *pReg, uint32_t val)
{
    bool intsDisabled;

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
bool __attribute__((naked)) Timer_masterDisable(void)
{
    /* read PRIMASK bit to R0 and call CPSID to disable interrupts */
    __asm__ __volatile__(
          "    mrs    r0, PRIMASK \n"
          "    cpsid  i \n"
          "    bx     lr \n");
}
#elif defined(__ti__)
    /* read PRIMASK bit to R0 and call CPSID to disable interrupts */
    __asm("    .sect \".text:ti_sysbios_family_arm_lm4_Timer_masterDisable\" \n"
          "    .clink \n"
          "    .global ti_sysbios_family_arm_lm4_Timer_masterDisable \n"
          "    .thumbfunc ti_sysbios_family_arm_lm4_Timer_masterDisable \n"
          "    .thumb \n"
          "ti_sysbios_family_arm_lm4_Timer_masterDisable: \n"
          "    mrs    r0, PRIMASK \n"
          "    cpsid  i \n"
          "    bx     lr \n");
#elif defined(__IAR_SYSTEMS_ICC__)
bool Timer_masterDisable(void)
{
    bool retVal;

    retVal = (bool)__get_interrupt_state();
    __disable_interrupt();

    return retVal;
}
#endif

/*
 *  ======== Timer_masterEnable ========
 */
#if (defined(__GNUC__) && !defined(__ti__)) || (defined(__GNUC__) && defined(__clang__))
void __attribute__((naked)) Timer_masterEnable(void)
{
    __asm__ __volatile__(
          "    cpsie  i \n"
          "    bx     lr \n");
}
#elif defined(__ti__)
    __asm("    .sect \".text:ti_sysbios_family_arm_lm4_Timer_masterEnable\" \n"
          "    .clink \n"
          "    .global ti_sysbios_family_arm_lm4_Timer_masterEnable \n"
          "    .thumbfunc ti_sysbios_family_arm_lm4_Timer_masterEnable \n"
          "    .thumb \n"
          "ti_sysbios_family_arm_lm4_Timer_masterEnable: \n"
          "    cpsie  i \n"
          "    bx     lr \n");
#elif defined(__IAR_SYSTEMS_ICC__)
void Timer_masterEnable(void)
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
__weak void Timer_enableCC26xx(int id)
#elif (defined(__GNUC__) && !defined(__ti__)) || (defined(__GNUC__) && defined(__clang__))
void __attribute__((weak)) Timer_enableCC26xx(int id)
#else
#pragma WEAK (Timer_enableCC26xx)
void Timer_enableCC26xx(int id)
#endif
{
    unsigned int gpt = INVALID_TIMER_ID;

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
#endif // ti_sysbios_family_arm_cc26xx_Boot_driverlibVersion

/*
 *  ======== Timer_enableCC3200 ========
 */
#if defined(__IAR_SYSTEMS_ICC__)
__weak void Timer_enableCC3200(int id)
#elif (defined(__GNUC__) && !defined(__ti__)) || (defined(__GNUC__) && defined(__clang__))
void __attribute__((weak)) Timer_enableCC3200(int id)
#else
#pragma WEAK (Timer_enableCC3200)
void Timer_enableCC3200(int id)
#endif
{
    unsigned int key;

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
void Timer_enableTiva(int id)
{
    unsigned int key;

    key = Hwi_disable();

    /* if a pre-Flurry class device, and one of the first four timers ... */
    if (((HWREG(SYSCTL_DID0) & SYSCTL_DID0_CLASS_M) <
        SYSCTL_DID0_CLASS_FLURRY) && (id < 4)) {

        /* enable run mode clock (per SYSBIOS-185, use bitband alias) */
        *((uint32_t *)(RCGC1_BB_TIMER0 + (id * 4))) = 1;

        /* ensure at least 5 clock cycle delay for clock enable */
        *RCGC1;
        *RCGC1;
        *RCGC1;
        *RCGC1;
        *RCGC1;

        /* do a sw reset on the timer */
        *SRCR1 |= (uint32_t)(1 << (id + 16));
        *SRCR1 &= ~(uint32_t)(1 << (id + 16));
    }
    /* else, Flurry or later device, or 5th timer or above ... */
    else {
        /* enable run mode clock */
        *RCGCTIMERS |= (uint32_t)(1 << id);
        *SCGCTIMERS |= (uint32_t)(1 << id);
        *DCGCTIMERS |= (uint32_t)(1 << id);

        /* ensure at least 5 clock cycle delay for clock enable */
        *RCGCTIMERS;
        *RCGCTIMERS;
        *RCGCTIMERS;
        *RCGCTIMERS;
        *RCGCTIMERS;

        /* do a sw reset on the timer */
        *SRTIMER |= (uint32_t)(1 << id);
        *SRTIMER &= ~(uint32_t)(1 << id);
    }

    Hwi_restore(key);
}

/*
 *  ======== Timer_disableCC26xx ========
 */
#if defined(__IAR_SYSTEMS_ICC__)
__weak void Timer_disableCC26xx(int id)
#elif (defined(__GNUC__) && !defined(__ti__)) || (defined(__GNUC__) && defined(__clang__))
void __attribute__((weak)) Timer_disableCC26xx(int id)
#else
#pragma WEAK (Timer_disableCC26xx)
void Timer_disableCC26xx(int id)
#endif
{
}

/*
 *  ======== Timer_disableCC3200 ========
 */
#if defined(__IAR_SYSTEMS_ICC__)
__weak void Timer_disableCC3200(int id)
#elif (defined(__GNUC__) && !defined(__ti__)) || (defined(__GNUC__) && defined(__clang__))
void __attribute__((weak)) Timer_disableCC3200(int id)
#else
#pragma WEAK (Timer_disableCC3200)
void Timer_disableCC3200(int id)
#endif
{
    unsigned int key;

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
void Timer_disableTiva(int id)
{
    unsigned int key;

    key = Hwi_disable();

    //TODO: Can we remove the delays from the disable code ?

    /* if a pre-Flurry class device, and one of the first four timers ... */
    if (((HWREG(SYSCTL_DID0) & SYSCTL_DID0_CLASS_M) <
        SYSCTL_DID0_CLASS_FLURRY) && (id < 4)) {

        /* disable run mode clock (per SYSBIOS-185, use bitband alias) */
        *((uint32_t *)(RCGC1_BB_TIMER0 + (id * 4))) = 0;

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
        *RCGCTIMERS &= ~(uint32_t)(1 << id);
        *SCGCTIMERS &= ~(uint32_t)(1 << id);
        *DCGCTIMERS &= ~(uint32_t)(1 << id);

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
unsigned int Timer_getAvailMask()
{
    return (Timer_module->availMask);
}

/*
 *  ======== Timer_setAvailMask ========
 */
bool Timer_setAvailMask(unsigned int mask)
{
    unsigned int i;
    unsigned int key;
    unsigned int tmpMask;

    key = Hwi_disable();
    tmpMask = mask;
    for (i = 0; i < Timer_numTimerDevices; i++) {
        /* Check if mask is setting any currently used timer as available */
        if ((tmpMask & 0x1) && (Timer_module->handles[i] != NULL)) {
            Hwi_restore(key);
            return (false);
        }
        tmpMask = tmpMask >> 1;
    }
    Timer_module->availMask = mask;
    Hwi_restore(key);

    return (true);
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
Timer_Handle Timer_construct(Timer_Struct *timer, int id, Timer_FuncPtr tickFxn,
     const Timer_Params *params, Error_Block *eb)
{
    int status = 0;

    if (params == NULL) {
        params = &Timer_Params_default;
    }

    status = Timer_Instance_init(timer, id, tickFxn, params, eb);

    if (status != 0) {
        Timer_Instance_finalize(timer, status);
        timer = NULL;
    }

    return (timer);
}

/*
 *  ======== Timer_create ========
 */
Timer_Handle Timer_create(int id, Timer_FuncPtr tickFxn,
     const Timer_Params *params, Error_Block *eb)
{
    Timer_Handle timer, status;

    timer = Memory_alloc(NULL, sizeof(Timer_Object), 0, eb);

    if (timer != NULL) {
       status = Timer_construct(timer, id, tickFxn, params, eb);
       if (status == NULL) {
           Memory_free(NULL, timer, sizeof(Timer_Object));
           timer = NULL;
       }
    }

    return (timer);
}

/*
 *  ======== Timer_destruct ========
 */
void Timer_destruct(Timer_Object *timer)
{
    unsigned int hwiKey;

    /* remove timer from global timer list (Queue_remove is not atomic) */
    hwiKey = Hwi_disable();
    Queue_remove(&timer->objElem);
    Hwi_restore(hwiKey);

    Timer_Instance_finalize(timer, 0);
}

/*
 *  ======== Timer_delete ========
 */
void Timer_delete(Timer_Handle *timer)
{
    Timer_destruct(*timer);

    Memory_free(NULL, *timer, sizeof(Timer_Object));

    *timer = NULL;
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
