/*
 * Copyright (c) 2012-2017, Texas Instruments Incorporated
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
 *  "Global Proxy" for target specific Timer implementaions
 */

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/BIOS.h>
#include "package/internal/Timer.xdc.h"

/*
 *  ======== Timer_Module_startup ========
 */
Int Timer_Module_startup(Int phase)
{

    /* must wait for these modules to initialize first */
    if (!Timer_TimerProxy_Module_startupDone()) {
        return Startup_NOTDONE;
    }

    /* okay to proceed with initialization */

    return Startup_DONE;
}

/*
 *  ======== Timer_getNumTimers ========
 */
UInt Timer_getNumTimers()
{
    return (Timer_TimerProxy_getNumTimers());
}

/*
 *  ======== Timer_getStatus ========
 */
Timer_Status Timer_getStatus(UInt timerId)
{
    return (Timer_TimerProxy_getStatus(timerId));
}

/*
 *  ======== Timer_startup ========
 */
Void Timer_startup()
{
    Timer_TimerProxy_startup();
}

/*
 *  ======== Timer_Instance_init ========
 */
Int Timer_Instance_init(Timer_Object *timer, Int id, Timer_FuncPtr tickFxn, const Timer_Params *params, Error_Block *eb)
{
    timer->pi = Timer_TimerProxy_create(id, tickFxn, (Timer_TimerProxy_Params *)params, eb);

    if (timer->pi == NULL) {
        return (1);
    }
    return (0);
}

/*
 *  ======== Timer_Instance_finalize ========
 */
Void Timer_Instance_finalize(Timer_Object *timer, Int status)
{
    /*
     * Only call TimerProxy_delete() on normal Timer_delete() calls.
     * If Timer_TimerProxy_create() failed, Timer_TimerProxy_Instance_finalize
     * was already called automatically.
     */
    if (status == 0) {
        Timer_TimerProxy_delete(&timer->pi);
    }
}

/*
 *  ======== Timer_setNextTick ========
 */
Void Timer_setNextTick(Timer_Object *timer, UInt32 ticks)
{
    Timer_TimerProxy_setNextTick(timer->pi, ticks);
}

/*
 *  ======== Timer_start ========
 */
Void Timer_start(Timer_Object *timer)
{
    Timer_TimerProxy_start(timer->pi);
}

/*
 *  ======== Timer_trigger ========
 */
Void Timer_trigger(Timer_Object *timer, UInt32 insts)
{
    Timer_TimerProxy_trigger(timer->pi, insts);
}

/*
 *  ======== Timer_getCount ========
 */
UInt32 Timer_getCount(Timer_Object *timer)
{
    return (Timer_TimerProxy_getCount(timer->pi));
}

/*
 *  ======== Timer_getCurrentTick ========
 */
UInt32 Timer_getCurrentTick(Timer_Object *timer, Bool saveFlag)
{
    return (Timer_TimerProxy_getCurrentTick(timer->pi, saveFlag));
}

/*
 *  ======== Timer_getExpiredCounts ========
 */
UInt32 Timer_getExpiredCounts(Timer_Object *timer)
{
    return (Timer_TimerProxy_getExpiredCounts(timer->pi));
}

/*
 *  ======== Timer_getExpiredTicks ========
 */
UInt32 Timer_getExpiredTicks(Timer_Object *timer, UInt32 tickPeriod)
{
    return (Timer_TimerProxy_getExpiredTicks(timer->pi, tickPeriod));
}

/*
 *  ======== Timer_getMaxTicks ========
 */
UInt32 Timer_getMaxTicks(Timer_Object *timer)
{
    return (Timer_TimerProxy_getMaxTicks(timer->pi));
}

/*
 *  ======== Timer_stop ========
 */
Void Timer_stop(Timer_Object *timer)
{
    Timer_TimerProxy_stop(timer->pi);
}

/*
 *  ======== Timer_setPeriod ========
 */
Void Timer_setPeriod(Timer_Object *timer, UInt32 period)
{
    Timer_TimerProxy_setPeriod(timer->pi, period);
}

/*
 *  ======== Timer_setPeriodMicroSecs ========
 */
Bool Timer_setPeriodMicroSecs(Timer_Object *timer, UInt32 usecs)
{
    return(Timer_TimerProxy_setPeriodMicroSecs(timer->pi, usecs));
}
/*
 *  ======== Timer_getPeriod ========
 */
UInt32 Timer_getPeriod(Timer_Object *timer)
{
    return (Timer_TimerProxy_getPeriod(timer->pi));
}

/*
 *  ======== Timer_getFreq ========
 */
Void Timer_getFreq(Timer_Object *timer, Types_FreqHz *freq)
{
    Timer_TimerProxy_getFreq((timer->pi), freq);
}

/*
 *  ======== Timer_getFunc ========
 */
Timer_FuncPtr Timer_getFunc(Timer_Object *timer, UArg *arg)
{
    return (Timer_TimerProxy_getFunc((timer->pi), arg));
}

/*
 *  ======== Timer_setFunc ========
 */
Void Timer_setFunc(Timer_Object *timer, Timer_FuncPtr fxn, UArg arg)
{
    Timer_TimerProxy_setFunc((timer->pi), fxn, arg);
}
