/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
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
/* REQ_TAG(SYSBIOS-1042), REQ_TAG(SYSBIOS-1020) */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/Types.h>

#include <ti/sysbios/family/arm/v8m/Timer.h>

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

Timer_Module_State Timer_Module_state = {
    .objQ.next = &Timer_Module_state.objQ,
    .objQ.prev = &Timer_Module_state.objQ,
    .availMask = 1
};

/* startupNeeded */
const unsigned int Timer_startupNeeded;

const unsigned int Timer_anyMask = 1U;

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
 */
unsigned int Timer_getNumTimers()
{
    return (Timer_NUM_TIMER_DEVICES);
}

/*
 *  ======== Timer_getStatus ========
 */
Timer_Status Timer_getStatus(unsigned int timerId)
{
    Assert_isTrue(timerId < Timer_NUM_TIMER_DEVICES, NULL);

    if ((Timer_module->availMask & ((unsigned int)0x1U << timerId)) != 0U) {
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
    return (0);
}

/*
 *  ======== Timer_setNextTick ========
 */
void Timer_setNextTick(Timer_Object *obj, uint32_t ticks)
{
}

/*
 *  ======== Timer_init ========
 *  Calls postInit for all statically-created & constructed
 *  timers to initialize them.
 */
void Timer_init(void)
{
    Timer_Object *obj;

    obj = Timer_module->handle;
    /* if timer was statically created/constructed */
    if ((obj != NULL) && (obj->staticInst != false)) {
        /* This function currently only returns 0. Adjust if this changes */
        Timer_postInit(obj, NULL);
    }
}

/*
 *  ======== Timer_startup ========
 *  Here after main(). Called from BIOS_start()
 */
void Timer_startup()
{
    Timer_Object *obj;

    /* If TimestampProvider is used, it's Module_startup will start the Timer */
    if (Timer_startupNeeded != false) {
        obj = Timer_module->handle;
        /* if timer was statically created/constructed */
        if ((obj != NULL) && (obj->staticInst != false)) {
            if (obj->startMode == Timer_StartMode_AUTO) {
                Timer_start(obj);
            }
        }
    }
}

/*
 *  ======== Timer_getHandle ========
 */
Timer_Handle Timer_getHandle(unsigned int id)
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
int Timer_Instance_init(Timer_Object *obj, int id, Timer_FuncPtr tickFxn, const Timer_Params *params, Error_Block *eb)
{
    unsigned int key;
    Hwi_Params hwiParams;
    unsigned int tempId = 0xffff;

    if ((id != 0) && ((unsigned int)id != Timer_ANY)) {
        Error_raise(eb, Timer_E_invalidTimer, id, 0);
        return (1);
    }

    key = Hwi_disable();

    if ((unsigned int)id == Timer_ANY) {
        if (((Timer_anyMask & 1U) != 0U) &&
                ((Timer_module->availMask & 1U) != 0U)) {
            Timer_module->availMask &= ~(1U);
            tempId = 0;
        }
    }
    else {
        if ((Timer_module->availMask & 1U) != 0U) {
            Timer_module->availMask &= ~(1U);
            tempId = (unsigned int)id;
        }
    }

    Hwi_restore(key);

    obj->staticInst = false;

    if (tempId == 0xffffU) {
        Error_raise(eb, Timer_E_notAvailable, id, 0);
        return (NO_TIMER_AVAIL);
    }
    else {
        obj->id = (int)tempId;
    }

    Timer_module->handle = obj;

    obj->runMode = params->runMode;
    obj->startMode = params->startMode;
    obj->period = params->period;
    obj->periodType = params->periodType;
    obj->extFreq.lo = params->extFreq.lo;
    obj->extFreq.hi = params->extFreq.hi;

    if (obj->periodType == Timer_PeriodType_MICROSECS) {
        /* This function currently only returns true. Adjust if this changes */
        Timer_setPeriodMicroSecs(obj, obj->period);
    }

    obj->arg = params->arg;
    obj->intNum = 15;
    obj->tickFxn = tickFxn;

    if ((obj->tickFxn) != NULL) {
        if ((params->hwiParams) != NULL) {
            Hwi_Params_copy(&hwiParams, (params->hwiParams));
        }
        else {
            Hwi_Params_init(&hwiParams);
        }

        hwiParams.arg = (uintptr_t)obj;

        if (obj->runMode == Timer_RunMode_CONTINUOUS) {
            obj->hwi = Hwi_create ((int)obj->intNum, Timer_periodicStub,
                &hwiParams, eb);
        }
        else {
            obj->hwi = Hwi_create ((int)obj->intNum, Timer_oneShotStub,
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
void Timer_reconfig (Timer_Object *obj, Timer_FuncPtr tickFxn, const Timer_Params *params,
    Error_Block *eb)
{
    Hwi_Params hwiParams;

    obj->runMode = params->runMode;
    obj->startMode = params->startMode;
    obj->period = params->period;
    obj->periodType = params->periodType;

    if (obj->periodType == Timer_PeriodType_MICROSECS) {
        /* This function currently only returns true. Adjust if this changes */
        Timer_setPeriodMicroSecs(obj, obj->period);
    }

    obj->arg = params->arg;
    obj->tickFxn = tickFxn;

    if (obj->tickFxn != NULL) {
        if (params->hwiParams != NULL) {
            Hwi_Params_copy(&hwiParams, (params->hwiParams));
        }
        else {
            Hwi_Params_init(&hwiParams);
        }

        hwiParams.arg = (uintptr_t)obj;

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
int Timer_postInit(Timer_Object *obj, Error_Block *eb)
{
    unsigned int hwiKey;

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
void Timer_Instance_finalize(Timer_Object *obj, int status)
{
    unsigned int key;

    if (status == TIMER_DELETED) {
        Timer_initDevice(obj);
        if (obj->hwi != NULL) {
            Hwi_delete(&obj->hwi);
        }
    }

    key = Hwi_disable();
    Timer_module->availMask |= 1U;
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
void Timer_initDevice(Timer_Object *obj)
{
    unsigned int key;

    key = Hwi_disable();

    Hwi_nvic.STCSR = 0; /* stop the timer */
    Hwi_nvic.STRVR = 0; /* reset reload value */
    Hwi_nvic.STCVR = 0; /* reset current value */

    if (obj->hwi != NULL) {
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
/* REQ_TAG(SYSBIOS-1021) */
void Timer_start(Timer_Object *obj)
{
    unsigned int key;

    key = Hwi_disable();

    if (obj->hwi != NULL) {
        Hwi_clearInterrupt(obj->intNum);
        Hwi_enableInterrupt(obj->intNum);
    }

    Hwi_nvic.STCVR = 0; /* reset counter, forces reload of period value */

    if (obj->extFreq.lo != 0U) {
        Hwi_nvic.STCSR |= 0x1U;  /* start timer, select ext clock */
    }
    else {
        Hwi_nvic.STCSR |= 0x5U;  /* start timer, select int clock */
    }

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
    Hwi_nvic.STCSR = 0;                 /* stop the timer */
    Hwi_clearInterrupt(obj->intNum);    /* clear any pending interrupts */
    Hwi_nvic.STRVR = insts;             /* set the period */
    Hwi_nvic.STCVR = 0; /* reset counter, forces reload of period value */
    if (obj->extFreq.lo != 0U) {
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
void Timer_stop(Timer_Object *obj)
{
    Hwi_nvic.STCSR = 0;
    if (obj->hwi != NULL) {
        Hwi_disableInterrupt(obj->intNum);
    }
}

/*
 *  ======== Timer_setPeriod ========
 *  1. stop timer
 *  2. set period register
 */
void Timer_setPeriod(Timer_Object *obj, uint32_t period)
{
    Timer_stop(obj);
    obj->period = period;

    if (obj->runMode == Timer_RunMode_CONTINUOUS) {
        period = period - 1U;
    }
    if ((period & 0xff000000U) != 0U) {
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
bool Timer_setPeriodMicroSecs(Timer_Object *obj, uint32_t period)
{
    Types_FreqHz freqHz;
    uint64_t counts;
    uint32_t freqKHz;

    Timer_stop(obj);

    Timer_getFreq(obj, &freqHz);
    freqKHz = freqHz.lo / 1000U;

    counts = ((uint64_t)freqKHz * (uint64_t)period) / (uint64_t)1000;

    obj->period = (unsigned int)counts;
    obj->periodType = Timer_PeriodType_COUNTS;

    Timer_setPeriod(obj, (uint32_t)counts);

    /*
     *  Several places in this file assume that only true is returned.
     *  Adjust them accordingly if this is changes.
     */
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
uint32_t Timer_getExpiredCounts(Timer_Object *obj)
{
    uint32_t count1, count2;
    uint32_t wrap, stscr;

    count1 = Hwi_nvic.STCVR;
    stscr = Hwi_nvic.STCSR;
    count2 = Hwi_nvic.STCVR;

    wrap = stscr & 0x00010000U; /* wrap? */

    if ((stscr & 0x1U) != 0U) { /* if timer is running */
        /*
         * Reading the STCSR clears the WRAP bit!
         * bump tickCount so that subsequent Timestamp_get32()
         * calls will behave properly
         */
        if (wrap != 0U) {
            Timer_module->tickCount++;
        }

        if ((count1 > count2) && (wrap != 0U)) {
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
uint32_t Timer_getExpiredTicks(Timer_Object *obj, uint32_t tickPeriod)
{
    return (0);
}

/*
 *  ======== Timer_getCurrentTick ========
 */
uint32_t Timer_getCurrentTick(Timer_Object *obj, bool saveFlag)
{
    return (0);
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
}

/*
 *  ======== Timer_getTickCount ========
 */
uint32_t Timer_getTickCount()
{
    return (Timer_module->tickCount);
}

/*
 *  ======== Timer_oneShotStub ========
 */
void Timer_oneShotStub(uintptr_t arg)
{
    Timer_Object *obj = (Timer_Object *)arg;
    volatile uint32_t dummy;

    Timer_stop(obj);

    dummy = Hwi_nvic.STCSR; /* read to ack the interrupt */
    Hwi_nvic.ICSR = 0x02000000; /* clear SysTick pending */

    obj->tickFxn(obj->arg);
}

/*
 *  ======== Timer_periodicStub ========
 */
void Timer_periodicStub(uintptr_t arg)
{
    Timer_Object *obj = (Timer_Object *)arg;
    volatile uint32_t dummy;

    dummy = Hwi_nvic.STCSR; /* read to ack the interrupt */

    /*
     * Reading the STCSR clears the WRAP bit!
     * if getExpiredCounts got here first,
     * the rollover bit will have been cleared
     * and the tickCount incremented
     */

    if ((dummy & 0x10000U) != 0U) {
        Timer_module->tickCount++;
    }

    Hwi_nvic.ICSR = 0x02000000; /* clear SysTick pending */

    obj->tickFxn(obj->arg);
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
