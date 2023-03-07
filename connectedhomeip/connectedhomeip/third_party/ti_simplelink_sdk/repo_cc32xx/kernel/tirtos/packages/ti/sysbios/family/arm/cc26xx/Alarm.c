/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 *  ======== Alarm.c ========
 */
#include <xdc/std.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/family/arm/cc26xx/Timer.h>

#include "package/internal/Alarm.xdc.h"

/*
 *  ======== Alarm_Instance_init ========
 */
Void Alarm_Instance_init(Alarm_Object *obj, Alarm_FuncPtr func, UArg arg,
    const Alarm_Params* params)
{
    Clock_Params clockParams;
    Clock_Handle clockObj;

    clockObj = Alarm_Instance_State_clockObj(obj);

    /* construct Clock object */
    Clock_Params_init(&clockParams);
    clockParams.period = 0;
    clockParams.startFlag = FALSE;
    clockParams.arg = arg;
    Clock_construct(Clock_struct(clockObj), func, 0, &clockParams);
}

/*
 *  ======== Alarm_Instance_finalize ========
 */
Void Alarm_Instance_finalize(Alarm_Object *obj)
{
    Clock_Handle clockObj;

    clockObj = Alarm_Instance_State_clockObj(obj);

    Clock_destruct(Clock_struct(clockObj));
}

/*
 *  ======== Alarm_getCount ========
 */
UInt64 Alarm_getCount(Void)
{
    Clock_TimerProxy_Handle clockTimer;

    clockTimer = Clock_getTimerHandle();

    return (Timer_getCount64((Timer_Handle)clockTimer));
}

/*
 *  ======== Alarm_set ========
 */
Bool Alarm_set(Alarm_Object *obj, UInt64 rtcCount)
{
    Clock_Handle clockObj;
    Bool status = TRUE;
    UInt32 delta;
    UInt64 tick;
    UInt key;

    key = Hwi_disable();

    /* ensure target RTC count is greater than current count */
    if (rtcCount > Alarm_getCount()) {

        /* compute corresponding Clock tick */
        tick = (UInt32) ((UInt64) rtcCount / (UInt64) Clock_getTickPeriod());
        delta = tick - Clock_getTicks();

        /* set timeout and start the clock object */
        clockObj = Alarm_Instance_State_clockObj(obj);
        Clock_setTimeout(clockObj, delta);
        Clock_startI(clockObj);

        obj->rtcCount = rtcCount;
    }

    else {
        status = FALSE;
    }

    Hwi_restore(key);

    return (status);
}

/*
 *  ======== Alarm_setFunc ========
 */
Void Alarm_setFunc(Alarm_Object *obj, Alarm_FuncPtr fxn, UArg arg)
{
    Clock_Handle clockObj;

    clockObj = Alarm_Instance_State_clockObj(obj);

    Clock_setFunc(clockObj, (Clock_FuncPtr) fxn, arg);
}

/*
 *  ======== Alarm_stop ========
 */
Void Alarm_stop(Alarm_Object *obj)
{
    Clock_Handle clockObj;

    clockObj = Alarm_Instance_State_clockObj(obj);

    Clock_stop(clockObj);
}
