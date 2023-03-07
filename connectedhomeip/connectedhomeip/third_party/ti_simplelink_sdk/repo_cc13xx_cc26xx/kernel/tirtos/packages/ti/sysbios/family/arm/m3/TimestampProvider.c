/*
 * Copyright (c) 2013-2017, Texas Instruments Incorporated
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
 *  ======== TimestampProvider.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Types.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/family/arm/m3/Timer.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>

#include "package/internal/TimestampProvider.xdc.h"

#define MOD     TimestampProvider_module

/*
 *  ======== TimestampProvider_Module_startup ========
 */
Int TimestampProvider_Module_startup( Int phase )
{
    /* Make sure Timer has been initialized */
    if (!Timer_Module_startupDone()) {
        return (Startup_NOTDONE);
    }

    /* If we are not the creator of the Timer instance */
    if (MOD->timer == NULL) {
        MOD->timer = Timer_getHandle(0);
    }

    /* We start the timer if we're used */
    Timer_start(MOD->timer);

    return Startup_DONE;
}

/*
 *  ======== TimestampProvider_rolloverFunc ========
 *  Hwi_create() needed something. The tickCount is maintained
 *  by Timer_periodicStub().
 */
Void TimestampProvider_rolloverFunc(UArg unused)
{
}

/*
 *  ======== TimestampProvider_get32 ========
 */
Bits32 TimestampProvider_get32()
{
    UInt key;
    UInt32 timestamp;

    key = Hwi_disable();

    /*
     * The order of calling getTickCount() and getExpiredCounts()
     * MUST be preserved to maintain rollover count coherency
     */
    timestamp = Timer_getTickCount();
    timestamp *= Timer_getPeriod(MOD->timer);
    timestamp += Timer_getExpiredCounts(MOD->timer);

    Hwi_restore(key);

    return (timestamp);
}

/*
 *  ======== TimestampProvider_get64 ========
 */
Void TimestampProvider_get64(Types_Timestamp64 *result)
{
    UInt key;
    UInt64 timestamp;

    key = Hwi_disable();

    /*
     * The order of calling getTickCount() and getExpiredCounts()
     * MUST be preserved to maintain rollover count coherency
     */
    timestamp = (UInt64)Timer_getTickCount();
    timestamp *= (UInt64)Timer_getPeriod(MOD->timer);
    timestamp += (UInt64)Timer_getExpiredCounts(MOD->timer);

    Hwi_restore(key);

    result->hi = timestamp >> 32;
    result->lo = timestamp;
}

/*
 *  ======== TimestampProvider_getCounterFreq ========
 */
Void TimestampProvider_getFreq(Types_FreqHz *freq)
{
    BIOS_getCpuFreq(freq);
}



