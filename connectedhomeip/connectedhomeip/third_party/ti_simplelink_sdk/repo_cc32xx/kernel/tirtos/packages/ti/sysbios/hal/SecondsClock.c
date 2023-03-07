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
 *  ======== SecondsClock.c ========
 */

#include <xdc/std.h>

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Clock.h>

#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Types.h>

#include "package/internal/SecondsClock.xdc.h"

#define NSECSPERSEC 1000000000  /* nanoseconds per second */

#define SECONDS_MAX 0xFFFFFFFF  /* Maximum number of seconds in 32 bits */

/*
 *  ======== Clock_Module_startup ========
 */
Int SecondsClock_Module_startup(Int phase)
{
    Clock_TimerProxy_Handle timer;
    Types_FreqHz freq;
    UInt32 period;
    UInt32 absDrift;
    UInt32 c1, c2;
    Int32  drift;

    if (!Clock_Module_startupDone()) {
        return Startup_NOTDONE;
    }

    timer = Clock_getTimerHandle();
    Clock_TimerProxy_getFreq(timer, &freq);
    period = Clock_TimerProxy_getPeriod(timer);

    /*
     *  Calculate the clock drift:
     *
     *  drift = timerFreq - clockFreq * clockTimerPeriod
     *
     *  Example: The timer frequency is 32Khz (frequency is actually 32768),
     *  and clock tick is 1 millisecond.  The timer period register will be
     *  set to 32.  The drift is:
     *      32768 - 1000 * 32 = 768
     *
     *  This means that our 'second' is really short by 768 timer ticks, or
     *  768 / 32768 of a second.  (This problem would be easily solved by just
     *  setting the period of the SecondsClock to 1024 msecs instead of 1000
     *  msecs, but it serves as a simple example.)
     *
     *  If the drift is negative, our seconds are too long.  For example, if
     *  the timer period register were 33 in the above example instead of 32,
     *  we would get
     *      drift = 32768 - 1000 * 33 = -768
     *
     *  To adjust the seconds, we will periodically add (drift > 0) or subtract
     *  (drift < 0) a second.  The period to do this will be:
     *
     *      c1 = floor(timer freq / |drift|)
     *
     *  In the above example (for both 32 and 33 timer periods), we will adjust
     *  every floor(32768 / 768) = 42 seconds.
     *
     *  In this example, we are adjusting a little too quickly, since
     *  32768 / 768 = 42 2/3.  Too compensate, periodically, we will adjust by
     *  not adding or subtracting a second.
     *
     *  To calculate the period for not adjusting:  Calculate what the
     *  2nd drift would be after the first adjustment:
     *
     *      drift2 = timerFreq - c1 * |drift|
     *
     *  drift2 is accumulated every c1 seconds.  If drift2 != 0, set
     *
     *      c2 = floor(timerFreq / drift2)
     *
     *  and every c1 * c2 seconds, do not do the adjustment.
     *
     *  If we wanted, we could continue in this manner, calculating
     *      drift3 = timerFreq - c2 * |drift2|
     *      c3 = floor(timerFreq / drift3)
     *
     *  and every c1 * c2 * c3 seconds, do the adjustment.
     */
    drift = freq.lo - (1000000 / Clock_tickPeriod) * period;
    absDrift = (drift < 0) ? -drift : drift;

    /* Only calculate drift for frequencies less than 4GHz */
    if (freq.lo && drift) {
        c1 = c2 = 0;
        c1 = freq.lo / absDrift;
        c2 = (freq.lo != c1 * absDrift) ? freq.lo / (freq.lo - c1 * absDrift) : 0;

        SecondsClock_module->c1Inc = (drift > 0) ? -1 : 1;
        SecondsClock_module->c1 = c1;
        SecondsClock_module->c2 = c2;
    }

    Clock_start(SecondsClock_Module_State_clock());

    return Startup_DONE;
}

/*
 *  ======== SecondsClock_get ========
 */
UInt32 SecondsClock_get(Void)
{
    return (SecondsClock_module->seconds);
}

/*
 *  ======== SecondsClock_getTime ========
 */
UInt32 SecondsClock_getTime(SecondsClock_Time *ts)
{
    UInt key;
    UInt ticks;

    key = Hwi_disable();

    ts->secsHi = SecondsClock_module->secondsHi;
    ts->secs = SecondsClock_module->seconds;
    ticks = Clock_getTicks() - SecondsClock_module->ticks;

    Hwi_restore(key);

    ts->nsecs = ticks * Clock_tickPeriod * 1000;

    return (0);
}

/*
 *  ======== SecondsClock_increment ========
 */
Void SecondsClock_increment(UArg arg)
{
    if (SecondsClock_module->seconds == SECONDS_MAX) {
        SecondsClock_module->secondsHi++;
    }
    SecondsClock_module->seconds++;
    SecondsClock_module->ticks = Clock_getTicks();

    /*
     *  If we have drift, adjust seconds if count1 is a multiple of c1
     *  and count2 is not a multiple of c2.
     */
    if (SecondsClock_module->c1) {
        SecondsClock_module->count1++;

        if (SecondsClock_module->count1 == SecondsClock_module->c1) {
            SecondsClock_module->count1 = 0;

            /* If count2 is a multiple of c2, don't adjust */
            if (SecondsClock_module->c2) {
                SecondsClock_module->count2++;

                if (SecondsClock_module->count2 == SecondsClock_module->c2) {
                    SecondsClock_module->count2 = 0;
                }
                else {
                    SecondsClock_module->seconds += SecondsClock_module->c1Inc;
               }
            }
            else {
                SecondsClock_module->seconds += SecondsClock_module->c1Inc;
            }
        }
    }
}

/*
 *  ======== SecondsClock_set ========
 */
Void SecondsClock_set(UInt32 seconds)
{
    SecondsClock_module->seconds = seconds;
}

/*
 *  ======== SecondsClock_setTime ========
 */
UInt32 SecondsClock_setTime(SecondsClock_Time *ts)
{
    UInt   key;
    UInt32 secs;
    UInt32 nsecs;
    UInt32 ticksNSecs;
    UInt32 ticksClock;
    UInt32 nsecsPerTick;
    UInt32 timeout;
    UInt32 period;

    /* Adjust seconds if nanoseconds is over a second */
    secs = ts->secs + (ts->nsecs / NSECSPERSEC);
    nsecs = ts->nsecs - (ts->nsecs / NSECSPERSEC) * NSECSPERSEC;

    key = Hwi_disable();

    nsecsPerTick = Clock_tickPeriod * 1000;

    Clock_stop(SecondsClock_Module_State_clock());

    ticksClock = Clock_getTicks();

    /*
     *  Adjust nsces to be a multiple of Clock ticks (in nanosecond units).
     *  Take the ceiling of nsecs and the next Clock tick.  This will ensure
     *  that if SecondsClock_getTime() is called immediately after
     *  SecondsClock_setTime(), the time returned will not be behind the
     *  time set.
     */
    ticksNSecs = ((nsecs + nsecsPerTick - 1) / 1000) / Clock_tickPeriod;

    SecondsClock_module->secondsHi = ts->secsHi;
    SecondsClock_module->seconds = secs;
    SecondsClock_module->ticks = ticksClock - ticksNSecs;

    /* Set the SecondsClock timeout to ticks per second - ticks */
    timeout = (1000000 / Clock_tickPeriod) - ticksNSecs;
    period = Clock_getPeriod(SecondsClock_Module_State_clock());
    if ((timeout == 0) || (timeout > period)) {
        /* Should not happen */
        timeout = period;
    }

    Clock_setTimeout(SecondsClock_Module_State_clock(), timeout);
    Clock_startI(SecondsClock_Module_State_clock());

    Hwi_restore(key);

    return (0);
}
