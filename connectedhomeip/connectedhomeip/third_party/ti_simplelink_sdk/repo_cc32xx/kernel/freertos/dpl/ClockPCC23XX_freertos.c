/*
 * Copyright (c) 2021-2022, Texas Instruments Incorporated
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
 *  ======== ClockPCC23XX_freertos.c ========
 */

#include <stdlib.h>

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/utils/List.h>

#include "ClockPCC23XX.h"

#include <FreeRTOS.h>

/* Driverlib includes*/
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_evtsvt.h)
#include DeviceFamily_constructPath(inc/hw_systim.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)

/* Defines */
#define CLOCK_FREQUENCY_DIVIDER  (48000000U / configCPU_CLOCK_HZ)

/** Max number of ClockP ticks into the future supported by this ClockP
 * implementation.
 *
 * Under the hood, ClockP uses the SysTimer whose events trigger immediately if
 * the compare value is less than 2^22 systimer ticks in the past
 * (4.194sec at 1us resolution). Therefore, the max number of SysTimer ticks you
 * can schedule into the future is 2^32 - 2^22 - 1 ticks (~= 4290 sec at 1us
 * resolution). */
#define CLOCKP_PERIOD_MAX       (0xFFBFFFFFU / ClockP_tickPeriod)
/** Max number of seconds into the future supported by this ClockP
 * implementation.
 *
 * This limit affects ClockP_sleep() */
#define CLOCKP_PERIOD_MAX_SEC   4290U

/* Processing overhead.
 *
 * Empirically deduced processing overhead to ensure
 * ClockP_usleep() to be more accurate.
 */
#define CLOCKP_PROC_OVERHEAD_US (99U * CLOCK_FREQUENCY_DIVIDER)

/* Get the current ClockP tick value */
#define getClockPTick()         (HWREG(SYSTIM_BASE + SYSTIM_O_CNT1) / ClockP_tickPeriod)

typedef struct ClockP_Obj {
    List_Elem elem;             ///< Clock's List element. Must be first in struct
    uint32_t timeout;           ///< Timeout value (used for one-shot)
    uint32_t currTimeout;       ///< Next timeout value in number of tick periods
    uint32_t period;            ///< Period of periodic clock. 0 for one-shot.
    volatile bool active;       ///< Clock is active
    ClockP_Fxn fxn;             ///< Callback function
    uintptr_t arg;              ///< Argument passed to callback function
} ClockP_Obj;

/* Shared variables */
/* ClockP and Power policy share interrupt CPUIRQ16, and therefore Hwi object. */
extern HwiP_Struct clockHwi;

/* Local variables */
static bool ClockP_initialized = false;
static List_List ClockP_list;
static volatile uint32_t ClockP_ticks;
static uint32_t ClockP_nextScheduledTick;
static bool ClockP_inWorkFunc;
static bool ClockP_startDuringWorkFunc;
static ClockP_Params ClockP_defaultParams = {
    .startFlag = false,
    .period = 0,
    .arg = 0,
};

/* Function declarations */
static void ClockP_workFuncDynamic(uintptr_t arg);
static void ClockP_hwiCallback(uintptr_t arg0);
static void sleepTicks(uint32_t ticks);
static void sleepClkFxn(uintptr_t arg0);
static void ClockP_scheduleNextTick(uint32_t absTick);

/*
 *  ======== ClockP_Params_init ========
 */
void ClockP_Params_init(ClockP_Params *params)
{
    /* structure copy */
    *params = ClockP_defaultParams;
}

/*
 *  ======== ClockP_startup ========
 */
void ClockP_startup(void)
{
    if (!ClockP_initialized) {
        uint32_t nowTick;
        intptr_t key;

        /* Get current value as early as possible */
        nowTick = getClockPTick();

        /* Clear any pending interrupts on SysTimer channel 1 */
        HWREG(SYSTIM_BASE + SYSTIM_O_ICLR) = SYSTIM_ICLR_EVT1_CLR;

        /* Configure SysTimer channel 1 to compare mode */
        HWREG(SYSTIM_BASE + SYSTIM_O_CH1CFG) = 0;

        /* Make SysTimer halt on CPU debug halt */
        HWREG(SYSTIM_BASE + SYSTIM_O_EMU) = SYSTIM_EMU_HALT_STOP;

        /* HWI clockHwi is owned by the Power driver, but multiplexed between
        * ClockP and the Power policy (this is handled by the Power driver).
        * All ClockP must do is to set the callback function and mux the
        * SysTimer channel 1 signal to CPUIRQ16.
        */
        HwiP_setFunc(&clockHwi, ClockP_hwiCallback, (uintptr_t)NULL);
        HWREG(EVTSVT_BASE + EVTSVT_O_CPUIRQ16SEL) = EVTSVT_CPUIRQ16SEL_PUBID_SYSTIM1;

        /* Set IMASK for channel 1. IMASK is used by the power driver to know
         * which systimer channels are active.
         */
        HWREG(SYSTIM_BASE + SYSTIM_O_IMSET) = SYSTIM_IMSET_EVT1_SET;

        /* Initialize ClockP variables */
        List_clearList(&ClockP_list);
        ClockP_ticks = nowTick;
        ClockP_nextScheduledTick = (uint32_t)(nowTick + CLOCKP_PERIOD_MAX);
        ClockP_inWorkFunc = false;
        ClockP_startDuringWorkFunc = false;

        ClockP_initialized = true;

        /* Start the clock */
        key = HwiP_disable();
        ClockP_scheduleNextTick(ClockP_nextScheduledTick);
        HwiP_restore(key);
    }
}

/*
 *  ======== ClockP_getTicksUntilInterrupt  ========
 */
uint32_t ClockP_getTicksUntilInterrupt(void)
{
    uint32_t ticks;

    ticks = ClockP_nextScheduledTick - getClockPTick();

    /* Clamp value to zero if nextScheduledTick is less than current */
    if (ticks > CLOCKP_PERIOD_MAX) {
        ticks = 0;
    }

    return (ticks);
}

/*
 *  ======== ClockP_scheduleNextTick  ========
 *  Must be called with global interrupts disabled!
 */
void ClockP_scheduleNextTick(uint32_t absTick)
{
    /* Reprogram the timer for the new period and next interrupt */
    uint32_t newSystim = (uint32_t)(absTick * ClockP_tickPeriod);

    /* Note: Channel interrupt flag is automatically cleared when writing a
     * compare value */
    HWREG(SYSTIM_BASE + SYSTIM_O_CH1VAL) = newSystim;

    /* Remember this */
    ClockP_nextScheduledTick = absTick;
}

/*
 *  ======== ClockP_walkQueueDynamic ========
 *  Walk the Clock Queue for TickMode_DYNAMIC, optionally servicing a
 *  specific tick
 */
uint32_t ClockP_walkQueueDynamic(bool service, uint32_t thisTick)
{
    uint32_t distance = ~0;
    List_List *list = &ClockP_list;
    List_Elem *elem;
    ClockP_Obj *obj;
    uint32_t delta;

    /* Traverse clock queue */
    for (elem = List_head(list); elem != NULL; elem = List_next(elem)) {

        obj = (ClockP_Obj *)elem;

        /* If  the object is active ... */
        if (obj->active == true) {

            /* Optionally service if tick matches timeout */
            if (service == true) {

                /* If this object is timing out update its state */
                if (obj->currTimeout == thisTick) {

                    if (obj->period == 0) {
                        /* Oneshot: Mark object idle */
                        obj->active = false;
                    }
                    else {
                        /* Periodic: Refresh timeout */
                        obj->currTimeout += (obj->period / CLOCK_FREQUENCY_DIVIDER);
                    }

                    /* Call handler */
                    obj->fxn(obj->arg);
                }
            }

            /* If object still active update distance to soonest tick */
            if (obj->active == true) {

                delta = obj->currTimeout - thisTick;

                /* If this is the soonest tick update distance to soonest */
                if (delta < distance) {
                    distance = delta;
                }
            }
        }
    }

    return (distance);
}

/*
 *  ======== ClockP_workFuncDynamic ========
 *  Service Clock Queue for TickMode_DYNAMIC
 */
void ClockP_workFuncDynamic(uintptr_t arg)
{
    uint32_t distance;
    uint32_t serviceTickAbs;
    uint32_t serviceTickDelta;
    uint32_t nowToNextService;
    uint32_t skippable;
    uint32_t nowTick;
    uint32_t nowDelta;
    uint32_t nextTick;
    uintptr_t hwiKey;

    hwiKey = HwiP_disable();

    /* Get current tick count. */
    nowTick = getClockPTick();

    /* Set flags while actively servicing queue */
    ClockP_inWorkFunc = true;
    ClockP_startDuringWorkFunc = false;

    /* Determine first tick expiration to service (the anticipated next tick) */
    serviceTickAbs = ClockP_nextScheduledTick;
    /* Number of ticks from now until next scheduled tick */
    nowToNextService = nowTick - serviceTickAbs;

    /*
     * If 'now' hasn't caught up to nextScheduledTick,
     * a spurious interrupt has probably occurred.
     * ignore for now...
     */
    serviceTickDelta = serviceTickAbs - ClockP_ticks;
    nowDelta = nowTick - ClockP_ticks;
    if (serviceTickDelta > nowDelta) {
        ClockP_inWorkFunc = false;
        HwiP_restore(hwiKey);
        return;
    }

    HwiP_restore(hwiKey);

    distance = 0;

    /* Walk queue until we catch up to current tick count */
    while (nowToNextService >= distance) {
        serviceTickAbs += distance;
        nowToNextService -= distance;
        distance = ClockP_walkQueueDynamic(true, serviceTickAbs);
    }

    hwiKey = HwiP_disable();

    /* If ClockP_start() was called during processing of Q, re-walk to
     * update distance */
    if (ClockP_startDuringWorkFunc == true) {
        distance = ClockP_walkQueueDynamic(false, serviceTickAbs);
    }

    /* If no active timeouts then skip the maximum supported by the timer */
    if (distance == ~0) {
        skippable = CLOCKP_PERIOD_MAX;
    }
    /* Else, finalize how many ticks can skip */
    else {
        skippable = distance - nowToNextService;
        if (skippable > CLOCKP_PERIOD_MAX) {
            skippable = CLOCKP_PERIOD_MAX;
        }
    }
    nextTick = serviceTickAbs + skippable;

    /* Reprogram for next expected tick */
    ClockP_scheduleNextTick(nextTick);

    ClockP_inWorkFunc = false;
    ClockP_ticks = serviceTickAbs;

    HwiP_restore(hwiKey);
}

/*
 *  ======== ClockP_hwiCallback ========
 */
void ClockP_hwiCallback(uintptr_t arg)
{
    /* ClockP is using raw SysTimer channel 1 interrupt. Clearing the channel 1
     * flag is strictly not necessary, but doing it here to avoid confusion for
     * anyone using the SysTimer combined event.
     */
    HWREG(SYSTIM_BASE + SYSTIM_O_ICLR) = SYSTIM_ICLR_EVT1_CLR;

    /* Run worker function */
    ClockP_workFuncDynamic(arg);
}

/*
 *  ======== ClockP_construct ========
 */
ClockP_Handle ClockP_construct(ClockP_Struct *handle, ClockP_Fxn fxn, uint32_t timeout, ClockP_Params *params)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    if (handle == NULL) {
        return NULL;
    }

    if (params == NULL) {
        params = &ClockP_defaultParams;
    }

    obj->period = params->period;
    obj->timeout = timeout;
    obj->fxn = fxn;
    obj->arg = params->arg;
    obj->active = false;

    ClockP_startup();

    /* Clock object is always placed on the ClockP work queue */
    List_put(&ClockP_list, &obj->elem);

    if (params->startFlag) {
        ClockP_start(obj);
    }

    return ((ClockP_Handle)handle);
}

/*
 *  ======== ClockP_create ========
 */
ClockP_Handle ClockP_create(ClockP_Fxn clkFxn, uint32_t timeout, ClockP_Params *params)
{
    ClockP_Handle handle;

    handle = (ClockP_Handle)malloc(sizeof(ClockP_Obj));

    /* ClockP_construct will check handle for NULL, no need here */
    handle = ClockP_construct((ClockP_Struct *)handle, clkFxn, timeout, params);

    return (handle);
}

/*
 *  ======== ClockP_destruct ========
 */
void ClockP_destruct(ClockP_Struct *clk)
{
    ClockP_Obj *obj = (ClockP_Obj *)clk;

    List_remove(&ClockP_list, &obj->elem);
}

/*
 *  ======== ClockP_add ========
 */
void ClockP_add(ClockP_Struct *handle, ClockP_Fxn fxn, uint32_t timeout, uintptr_t arg)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    obj->period = 0;
    obj->timeout = timeout;
    obj->fxn = fxn;
    obj->arg = arg;
    obj->active = false;

    /* Clock object is always placed on Clock work Q */
    List_put(&ClockP_list, &obj->elem);
}


/*
 *  ======== ClockP_delete ========
 */
void ClockP_delete(ClockP_Handle handle)
{
    ClockP_destruct((ClockP_Struct *)handle);

    free(handle);
}

/*
 *  ======== ClockP_start ========
 *  Set the Clock object's currTimeout value and set its active flag
 *  to true.
 */
void ClockP_start(ClockP_Handle handle)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;
    uintptr_t key = HwiP_disable();

    uint32_t nowTick;
    uint32_t nowDelta;
    uint32_t scheduledTick;
    uint32_t scheduledDelta;
    uint32_t remainingTicks;
    bool objectServiced = false;

    /* if Clock is NOT currently processing its Q */
    if (ClockP_inWorkFunc == false) {

        /* get current tick count */
        nowTick = getClockPTick();

        nowDelta = nowTick - ClockP_ticks;
        scheduledTick = ClockP_nextScheduledTick;
        scheduledDelta = scheduledTick - ClockP_ticks;

        /* Check if this new timeout is before next scheduled tick ... */
        if (nowDelta <= scheduledDelta) {
            objectServiced = true;

            /* Start new Clock object */
            obj->currTimeout = nowTick + (obj->timeout / CLOCK_FREQUENCY_DIVIDER);
            obj->active = true;

            /* How many ticks until scheduled tick? */
            remainingTicks = scheduledTick - nowTick;

            if ((obj->timeout / CLOCK_FREQUENCY_DIVIDER) < remainingTicks) {
                ClockP_scheduleNextTick(obj->currTimeout);
            }
        }
    }

    if (objectServiced == false) {
        /* Get current tick count */
        nowTick = getClockPTick();

        /* Start new Clock object */
        obj->currTimeout = nowTick + (obj->timeout / CLOCK_FREQUENCY_DIVIDER);
        obj->active = true;

        if (ClockP_inWorkFunc == true) {
            ClockP_startDuringWorkFunc = true;
        }
    }

    HwiP_restore(key);
}

/*
 *  ======== ClockP_stop ========
 *  Remove and clear Clock object's queue elem from clockQ
 */
void ClockP_stop(ClockP_Handle handle)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    obj->active = false;
}

/*
 *  ======== ClockP_setTimeout ========
 */
void ClockP_setTimeout(ClockP_Handle handle, uint32_t timeout)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    obj->timeout = timeout;
}

/*
 *  ======== ClockP_setPeriod ========
 */
void ClockP_setPeriod(ClockP_Handle handle, uint32_t period)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    obj->period = period;
}

/*
 *  ======== ClockP_getTimeout ========
 */
uint32_t ClockP_getTimeout(ClockP_Handle handle)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    if (obj->active == true) {
        return (obj->currTimeout - getClockPTick());
    }
    else {
        return (obj->timeout);
    }
}

/*
 *  ======== ClockP_isActive ========
 */
bool ClockP_isActive(ClockP_Handle handle)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    return (obj->active);
}

/*
 *  ======== ClockP_getCpuFreq ========
 */
void ClockP_getCpuFreq(ClockP_FreqHz *freq)
{
    freq->lo = (uint32_t)configCPU_CLOCK_HZ;
    freq->hi = 0;
}

/*
 *  ======== ClockP_getSystemTickPeriod ========
 */
uint32_t ClockP_getSystemTickPeriod(void)
{
    return (ClockP_tickPeriod);
}

/*
 *  ======== ClockP_getSystemTicks ========
 */
uint32_t ClockP_getSystemTicks(void)
{
    /* SysTimer is always running */
    return (getClockPTick());
}

/*
 *  ======== ClockP_sleep ========
 */
void ClockP_sleep(uint32_t sec)
{
    uint32_t ticksToSleep;

    if (sec > CLOCKP_PERIOD_MAX_SEC) {
	    sec = CLOCKP_PERIOD_MAX_SEC;
    }
    /* Convert from seconds to number of ticks */
    ticksToSleep = (sec * 1000000U) / ClockP_tickPeriod;
    sleepTicks(ticksToSleep);
}

/*
 *  ======== ClockP_usleep ========
 */
void ClockP_usleep(uint32_t usec)
{
    uint32_t currTick;
    uint32_t endTick;
    uint32_t ticksToSleep;

    /* Systimer is always running, get tick as soon as possible */
    currTick = getClockPTick();

    /* Make sure we sleep at least one tick if usec > 0 */
    endTick = currTick + ((usec + ClockP_tickPeriod - 1) / (ClockP_tickPeriod * CLOCK_FREQUENCY_DIVIDER));

    /* If usec large enough, sleep for the appropriate number of clock ticks. */
    if (usec > CLOCKP_PROC_OVERHEAD_US) {
        ClockP_startup();
        ticksToSleep = (usec - CLOCKP_PROC_OVERHEAD_US) / ClockP_tickPeriod;
        sleepTicks(ticksToSleep);
    }

    /* Spin remaining time */
    do {
        currTick = getClockPTick();
    } while (currTick < endTick);
}

/*
 *  ======== ClockP_staticObjectSize ========
 *  Internal function for testing that ClockP_Struct is large enough
 *  to hold ClockP object.
 */
size_t ClockP_staticObjectSize(void)
{
    return (sizeof(ClockP_Obj));
}

/*
 *  ======== sleepTicks ========
 *  Sleep for a given number of ClockP ticks.
 */
static void sleepTicks(uint32_t ticks)
{
    /* Cap to max number of ticks supported */
    if (ticks > CLOCKP_PERIOD_MAX) {
        ticks = CLOCKP_PERIOD_MAX;
    }

    SemaphoreP_Struct semStruct;
    ClockP_Struct clkStruct;
    ClockP_Params clkParams;
    SemaphoreP_Handle sem;

    /* Construct a semaphore, and a clock object to post the semaphore */
    sem = SemaphoreP_construct(&semStruct, 0, NULL);
    ClockP_Params_init(&clkParams);
    clkParams.startFlag = true;
    clkParams.arg = (uintptr_t)sem;
    ClockP_construct(&clkStruct, sleepClkFxn, ticks, &clkParams);

    /* Pend forever on the semaphore, wait for ClockP callback to post it */
    SemaphoreP_pend(sem, SemaphoreP_WAIT_FOREVER);

    /* Clean up */
    SemaphoreP_destruct(&semStruct);
    ClockP_destruct(&clkStruct);
}

/*
 *  ======== sleepClkFxn ========
 *  Timeout function for sleepTicks().
 */
static void sleepClkFxn(uintptr_t arg0)
{
    SemaphoreP_post((SemaphoreP_Handle)arg0);
}
