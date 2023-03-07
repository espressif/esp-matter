/*
 * Copyright (c) 2014-2019, Texas Instruments Incorporated
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
 *  ======== Seconds.c ========
 */

#include <xdc/std.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/devices/DeviceFamily.h>

#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_aon_rtc.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(driverlib/aon_rtc.h)
#include DeviceFamily_constructPath(driverlib/aon_event.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)

#include "package/internal/Seconds.xdc.h"

volatile UInt32 curShadow;

static Void getTime(Seconds_Time *ts);

/*
 *  ======== Seconds_get ========
 */
UInt32 Seconds_get(Void)
{
    Seconds_Time ts;

    Seconds_getTime(&ts);
    return (ts.secs);
}

/*
 *  ======== Seconds_getTime ========
 */
UInt32 Seconds_getTime(Seconds_Time *ts)
{
    Seconds_Time curTs;
    UInt         key;

    key = Hwi_disable();

    getTime(&curTs);

    ts->secsHi = Seconds_module->setSecondsHi;
    ts->secs = curTs.secs - Seconds_module->refSeconds +
            Seconds_module->setSeconds;
    ts->nsecs = curTs.nsecs + Seconds_module->deltaNSecs;
    if (ts->nsecs >= 1000000000) {
        ts->secs = ts->secs + 1;
        ts->nsecs = ts->nsecs - 1000000000;
    }

    /* Add 0 or -1, depending on relation of ref nsecs to nsecs at set time */
    ts->secs = ts->secs + Seconds_module->deltaSecs;

    if (ts->secs < Seconds_module->setSeconds) {
        /*
         *  Seconds have wrapped because setSeconds is close to 0xFFFFFFFF.
         *  This shouldn't happen again for another 136 years.
         */
        ts->secsHi++;
    }

    Hwi_restore(key);

    return (0);
}

/*
 *  ======== Seconds_set ========
 */
Void Seconds_set(UInt32 seconds)
{
    Seconds_Time ts;

    ts.secsHi = 0;
    ts.secs = seconds;
    ts.nsecs = 0;

    Seconds_setTime(&ts);
}

/*
 *  ======== Seconds_setTime ========
 */
UInt32 Seconds_setTime(Seconds_Time *ts)
{
    Seconds_Time refTs;
    UInt         key;

    key = Hwi_disable();

    getTime(&refTs);

    Seconds_module->setSecondsHi = ts->secsHi;
    Seconds_module->setSeconds = ts->secs;
    Seconds_module->refSeconds = refTs.secs;

    if (refTs.nsecs > ts->nsecs) {
        Seconds_module->deltaNSecs = 1000000000 + ts->nsecs - refTs.nsecs;
        Seconds_module->deltaSecs = -1;
    }
    else {
        Seconds_module->deltaNSecs = ts->nsecs - refTs.nsecs;
        Seconds_module->deltaSecs = 0;
    }

    Hwi_restore(key);

    return (0);
}

/*
 *  ======== getTime ========
 */
static Void getTime(Seconds_Time *ts)
{
    volatile UInt32 seconds;
    volatile UInt32 subseconds;
    UInt64 temp;
    UInt64 nsecs;

    /* read current RTC count */
    temp = AONRTCCurrent64BitValueGet();
    seconds = (UInt32) (temp >> 32);
    subseconds = (UInt32) (temp & 0xFFFFFFFF);
    ts->secs = seconds;
    ts->secsHi = 0;  // It will take 136 years for seconds to roll over.

    /*
     *  Throw away the lower 16 bits of the subseconds since this
     *  is used for temparature correction and does not accurately
     *  reflect the time.
     */
    subseconds = subseconds >> 16;

    nsecs = ((UInt64)1000000000 * (UInt64)subseconds) / (UInt64)65536;
    ts->nsecs = (UInt32)nsecs;
}
