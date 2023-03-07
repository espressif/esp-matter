/*
 * Copyright (c) 2012, Texas Instruments Incorporated
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

#include "package/internal/TimerNull.xdc.h"

/*
 *  ======== TimerNull_Module_startup ========
 */
Int TimerNull_Module_startup(Int phase)
{
    /* okay to proceed with initialization */

    return Startup_DONE;
}

/*
 *  ======== TimerNull_getNumTimers ========
 */
UInt TimerNull_getNumTimers()
{
    return (0);
}

/*
 *  ======== TimerNull_getStatus ========
 */
TimerNull_Status TimerNull_getStatus(UInt timerId)
{
    return (TimerNull_Status_FREE);
}

/*
 *  ======== TimerNull_startup ========
 */
Void TimerNull_startup()
{
}

/*
 *  ======== TimerNull_Instance_init ========
 */
Int TimerNull_Instance_init(TimerNull_Object *timer, Int id, TimerNull_FuncPtr tickFxn, const TimerNull_Params *params, Error_Block *eb)
{
    return (0);
}

/*
 *  ======== TimerNull_Instance_finalize ========
 */
Void TimerNull_Instance_finalize(TimerNull_Object *timer, Int status)
{
}

/*
 *  ======== TimerNull_getCurrentTick ========
 */
UInt32 TimerNull_getCurrentTick(TimerNull_Object *obj, Bool saveFlag)
{
    return (0);
}

/*
 *  ======== TimerNull_setNextTick ========
 */
Void TimerNull_setNextTick(TimerNull_Object *timer, UInt32 ticks)
{
}

/*
 *  ======== TimerNull_start ========
 */
Void TimerNull_start(TimerNull_Object *timer)
{
}

/*
 *  ======== TimerNull_trigger ========
 */
Void TimerNull_trigger(TimerNull_Object *timer, UInt32 insts)
{
}

/*
 *  ======== TimerNull_stop ========
 */
Void TimerNull_stop(TimerNull_Object *timer)
{
}

/*
 *  ======== TimerNull_setPeriod ========
 */
Void TimerNull_setPeriod(TimerNull_Object *timer, UInt32 period)
{
}

/*
 *  ======== TimerNull_setPeriodMicroSecs ========
 */
Bool TimerNull_setPeriodMicroSecs(TimerNull_Object *timer, UInt32 usecs)
{
    return(TRUE);
}
/*
 *  ======== TimerNull_getPeriod ========
 */
UInt32 TimerNull_getPeriod(TimerNull_Object *timer)
{
    return (0);
}

/*
 *  ======== TimerNull_getFreq ========
 */
Void TimerNull_getFreq(TimerNull_Object *timer, Types_FreqHz *freq)
{
}

/*
 *  ======== TimerNull_getFunc ========
 */
TimerNull_FuncPtr TimerNull_getFunc(TimerNull_Object *timer, UArg *arg)
{
    return (NULL);
}

/*
 *  ======== TimerNull_getMaxTicks ========
 */
UInt32 TimerNull_getMaxTicks(TimerNull_Object *timer)
{
    return (0);
}

/*
 *  ======== TimerNull_setFunc ========
 */
Void TimerNull_setFunc(TimerNull_Object *timer, TimerNull_FuncPtr fxn, UArg arg)
{
}

/*
 *  ======== TimerNull_getCount ========
 */
UInt32 TimerNull_getCount(TimerNull_Object *timer)
{
    return (0);
}

/*
 *  ======== TimerNull_getExpiredCounts ========
 */
UInt32 TimerNull_getExpiredCounts(TimerNull_Object *timer)
{
    return (0);
}

/*
 *  ======== TimerNull_getExpiredTicks ========
 */
UInt32 TimerNull_getExpiredTicks(TimerNull_Object *timer, UInt32 tickPeriod)
{
    return (0);
}
