/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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

#include "package/internal/Timer.xdc.h"

/* 
 * TIMER_DELETED (0) is passed in when you call Timer_delete 
 * BAD_PERIOD is currently not used
 */
#define TIMER_DELETED   0
#define BAD_TIMER_ID    1
#define NO_TIMER_AVAIL  2
#define NO_HWI_OBJ      3
#define BAD_PERIOD      4

#if defined(__GNUC__) && !defined(__ti__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

/*
 *  ======== Timer_getNumTimers ========
 */
UInt Timer_getNumTimers()
{
    return (Timer_NUM_TIMER_DEVICES);
}

/*
 *  ======== Timer_getStatus ========
 */
Timer_Status Timer_getStatus(UInt timerId)
{
    Assert_isTrue(timerId < Timer_NUM_TIMER_DEVICES, NULL);
    
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
    return (0);
}

/*
 *  ======== Timer_setNextTick ========
 */
Void Timer_setNextTick(Timer_Object *obj, UInt32 ticks)
{
}

/*
 *  ======== Timer_Module_startup ========
 *  Calls postInit for all statically-created & constructed
 *  timers to initialize them.
 */
Int Timer_Module_startup(Int status)
{
    Timer_Object *obj;

    obj = Timer_module->handle;
    /* if timer was statically created/constructed */
    if ((obj != NULL) && (obj->staticInst)) {
        /* This function currently only returns 0. Adjust if this changes */
        Timer_postInit(obj, NULL);
    }
    return (Startup_DONE);
}

/*
 *  ======== Timer_startup ========
 *  Here after main(). Called from BIOS_start()
 */
Void Timer_startup()
{
    Timer_Object *obj;

    /* If TimestampProvider is used, it's Module_startup will start the Timer */
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
    Assert_isTrue((id == 0), NULL);
    return (Timer_module->handle);
}

/*
 *  ======== Timer_Instance_init ========
 * 1. Select timer based on id
 * 2. Mark timer as in use
 * 3. Save timer handle if necessary (needed by TimestampProvider on 64).
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
    Hwi_Params hwiParams;
    UInt tempId = 0xffff;

    if ((id != 0) && (id != Timer_ANY)) {
        Error_raise(eb, Timer_E_invalidTimer, id, 0);
        return (1);
    }

    key = Hwi_disable();

    if (id == Timer_ANY) {
        if ((Timer_anyMask & 1) && (Timer_module->availMask & 1)) {
            Timer_module->availMask &= ~(1);
            tempId = 0;
        }
    }
    else if (Timer_module->availMask & 1) {
        Timer_module->availMask &= ~(1);
        tempId = id;
    }

    Hwi_restore(key);

    obj->staticInst = FALSE;

    if (tempId == 0xffff) {
        Error_raise(eb, Timer_E_notAvailable, id, 0);
        return (NO_TIMER_AVAIL);
    }
    else {
        obj->id = tempId;
    }

    Timer_module->handle = obj;

    obj->runMode = params->runMode;
    obj->startMode = params->startMode;
    obj->period = params->period;
    obj->periodType = params->periodType;
    obj->extFreq.lo = params->extFreq.lo;
    obj->extFreq.hi = params->extFreq.hi;

    if (obj->periodType == Timer_PeriodType_MICROSECS) {
        /* This function currently only returns TRUE. Adjust if this changes */
        Timer_setPeriodMicroSecs(obj, obj->period);
    }
  
    obj->arg = params->arg;
    obj->intNum = 15;
    obj->tickFxn = tickFxn;

    if (obj->tickFxn) {
        if (params->hwiParams) {
            Hwi_Params_copy(&hwiParams, (params->hwiParams));
        }
        else {
            Hwi_Params_init(&hwiParams);
        }

        hwiParams.arg = (UArg)obj;

        if (obj->runMode == Timer_RunMode_CONTINUOUS) {
            obj->hwi = Hwi_create (obj->intNum, Timer_periodicStub, 
                &hwiParams, eb);
        }
        else {
            obj->hwi = Hwi_create (obj->intNum, Timer_oneShotStub, 
                &hwiParams, eb);
        }

        if (obj->hwi == NULL) {
            return (NO_HWI_OBJ);
        }
    }
    else {
        obj->hwi = NULL;
    }

    /* This function currently only returns 0. Adjust if this changes */
    Timer_postInit(obj, eb);

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
Void Timer_reconfig (Timer_Object *obj, Timer_FuncPtr tickFxn, const Timer_Params *params, 
    Error_Block *eb)
{
    Hwi_Params hwiParams;

    obj->runMode = params->runMode;
    obj->startMode = params->startMode;
    obj->period = params->period;
    obj->periodType = params->periodType;

    if (obj->periodType == Timer_PeriodType_MICROSECS) {
        /* This function currently only returns TRUE. Adjust if this changes */
        Timer_setPeriodMicroSecs(obj, obj->period);        
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

        if (obj->runMode == Timer_RunMode_CONTINUOUS) {
            Hwi_reconfig (obj->hwi, Timer_periodicStub, &hwiParams);
        }
        else {
            Hwi_reconfig (obj->hwi, Timer_oneShotStub, &hwiParams);
        }
    }

    /* This function currently only returns 0. Adjust if this changes */
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
    
    Timer_initDevice(obj);

    Timer_setPeriod(obj, obj->period);

    Hwi_restore(hwiKey);

    /*
     *  Several places in this file assume that only 0 is returned.
     *  Adjust them accordingly if this is changes.
     */
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
        case TIMER_DELETED:
            Timer_initDevice(obj);
            if (obj->hwi) {
                Hwi_delete(&obj->hwi);
            }
        
        /* Hwi create failed */
        case NO_HWI_OBJ:

        /* timer not available */
        case NO_TIMER_AVAIL:

        /* invalid timer id */
        case BAD_TIMER_ID:

        default:
            break;
    }

    key = Hwi_disable();
    Timer_module->availMask |= 1;
    Timer_module->handle = NULL;
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

    key = Hwi_disable();
    
    Hwi_nvic.STCSR = 0; /* stop the timer */
    Hwi_nvic.STRVR = 0; /* reset reload value */
    Hwi_nvic.STCVR = 0; /* reset current value */

    if (obj->hwi) {
        Hwi_disableInterrupt(obj->intNum);
        Hwi_clearInterrupt(obj->intNum);
    }

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

    key = Hwi_disable();

    if (obj->hwi) {
        Hwi_clearInterrupt(obj->intNum);
        Hwi_enableInterrupt(obj->intNum);
    }

    Hwi_nvic.STCVR = 0; /* reset counter, forces reload of period value */

    if (obj->extFreq.lo) {
        Hwi_nvic.STCSR |= 0x1;  /* start timer, select ext clock */
    }
    else {
        Hwi_nvic.STCSR |= 0x5;  /* start timer, select int clock */
    }

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
    Hwi_nvic.STCSR = 0;                 /* stop the timer */
    Hwi_clearInterrupt(obj->intNum);    /* clear any pending interrupts */
    Hwi_nvic.STRVR = insts;             /* set the period */
    Hwi_nvic.STCVR = 0; /* reset counter, forces reload of period value */
    if (obj->extFreq.lo) {
        Hwi_nvic.STCSR = 0x3;   /* start timer, select ext clock */
    }
    else {
        Hwi_nvic.STCSR = 0x7;   /* start timer, select int clock */
    }
    Hwi_restore(key);
}

/*
 *  ======== Timer_stop ========
 *  1. stop timer
 *  2. disable timer interrupt
 */
Void Timer_stop(Timer_Object *obj)
{
    Hwi_nvic.STCSR = 0;
    if (obj->hwi) {
        Hwi_disableInterrupt(obj->intNum);
    }
}

/*
 *  ======== Timer_setPeriod ========
 *  1. stop timer
 *  2. set period register
 */
Void Timer_setPeriod(Timer_Object *obj, UInt32 period)
{
    Timer_stop(obj);
    obj->period = period;

    if (obj->runMode == Timer_RunMode_CONTINUOUS) {
        period = period - 1;
    }
    if (period & 0xff000000) {
        Error_raise(NULL, Timer_E_cannotSupport, obj->period, 0);
    }
    Hwi_nvic.STRVR = period;
}

/*
 *  ======== Timer_setPeriodMicroSecs ========
 *  1. stop timer
 *  2. compute counts
 *  3. set period register
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

    Timer_setPeriod(obj, counts);

    /*
     *  Several places in this file assume that only TRUE is returned.
     *  Adjust them accordingly if this is changes.
     */
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
    return (Hwi_nvic.STCVR);
}

/*
 *  ======== Timer_getExpiredCounts ========
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
 *  directly, which means the count value is going *down* on the 28x. This
 *  means that most of the time count1 > count2.
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
UInt32 Timer_getExpiredCounts(Timer_Object *obj)
{
    UInt32 count1, count2;
    UInt32 wrap, stscr;

    count1 = Hwi_nvic.STCVR;
    stscr = Hwi_nvic.STCSR;
    count2 = Hwi_nvic.STCVR;

    wrap = stscr & 0x00010000; /* wrap? */

    if (stscr & 0x1) { /* if timer is running */
        /*
         * Reading the STCSR clears the WRAP bit!
         * bump tickCount so that subsequent Timestamp_get32()
         * calls will behave properly
         */
        if (wrap) {
            Timer_module->tickCount++;
        }

        if ((count1 > count2) && wrap) {
            return ((obj->period - count1) + obj->period);
        }
        else {
            return (obj->period - count1);
        }
    }
    else { /* timer not running */
        return (0);
    }
}

/*
 *  ======== Timer_getExpiredTicks ========
 */
UInt32 Timer_getExpiredTicks(Timer_Object *obj, UInt32 tickPeriod)
{
    return (0);
}

/*
 *  ======== Timer_getCurrentTick ========
 */
UInt32 Timer_getCurrentTick(Timer_Object *obj, Bool saveFlag)
{
    return (0);
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
}

/*
 *  ======== Timer_getTickCount ========
 */
UInt32 Timer_getTickCount()
{
    return (Timer_module->tickCount);
}

/*
 *  ======== Timer_oneShotStub ========
 */
Void Timer_oneShotStub(UArg arg)
{
    Timer_Object *obj = (Timer_Object *)arg;
    volatile UInt32 dummy;

    Timer_stop(obj);

    dummy = Hwi_nvic.STCSR; /* read to ack the interrupt */
    Hwi_nvic.ICSR = 0x02000000; /* clear SysTick pending */

    obj->tickFxn(obj->arg);
}

/*
 *  ======== Timer_periodicStub ========
 */
Void Timer_periodicStub(UArg arg)
{
    Timer_Object *obj = (Timer_Object *)arg;
    volatile UInt32 dummy;

    dummy = Hwi_nvic.STCSR; /* read to ack the interrupt */

    /* 
     * Reading the STCSR clears the WRAP bit!
     * if getExpiredCounts got here first, 
     * the rollover bit will have been cleared
     * and the tickCount incremented
     */

    if (dummy & 0x10000) {
        Timer_module->tickCount++;
    }

    Hwi_nvic.ICSR = 0x02000000; /* clear SysTick pending */

    obj->tickFxn(obj->arg);
}
