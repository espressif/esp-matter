/*
 * Copyright (c) 2013-2020, Texas Instruments Incorporated
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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Types.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/family/arm/v8m/Hwi.h>
#include <ti/sysbios/family/arm/v8m/Timer.h>
#include <ti/sysbios/family/arm/v8m/TimestampProvider.h>

#include <ti/sysbios/knl/ClockSupport.h>

Timer_Struct TimestampProvider_timerStruct;

TimestampProvider_Module_State TimestampProvider_Module_state;

/*
 *  ======== TimestampProvider_init ========
 */
void TimestampProvider_init()
{
#if ti_sysbios_family_arm_v8m_TimestampProvider_useClockTimer_D
    extern Timer_Struct ClockSupport_timerStruct;
    TimestampProvider_module->timer = &ClockSupport_timerStruct;
#else
    Timer_Params timerParams;

    Timer_startup();

    Timer_Params_init(&timerParams);
    timerParams.period = Timer_MAX_PERIOD;
    timerParams.periodType = Timer_PeriodType_COUNTS;
    timerParams.startMode = Timer_StartMode_USER;

    Timer_construct(&TimestampProvider_timerStruct, 0,
        TimestampProvider_rolloverFunc, &timerParams, NULL);
    TimestampProvider_module->timer = &TimestampProvider_timerStruct;
    Timer_start(TimestampProvider_module->timer);
#endif
}

/*
 *  ======== TimestampProvider_rolloverFunc ========
 *  Hwi_create() needed something. The tickCount is maintained
 *  by Timer_periodicStub().
 */
void TimestampProvider_rolloverFunc(uintptr_t unused)
{
}

/*
 *  ======== TimestampProvider_get32 ========
 */
uint32_t TimestampProvider_get32()
{
    unsigned int key;
    uint32_t timestamp;

    key = Hwi_disable();

    /*
     * The order of calling getTickCount() and getExpiredCounts()
     * MUST be preserved to maintain rollover count coherency
     */
    timestamp = Timer_getTickCount();
    timestamp *= Timer_getPeriod(TimestampProvider_module->timer);
    timestamp += Timer_getExpiredCounts(TimestampProvider_module->timer);

    Hwi_restore(key);

    return (timestamp);
}

/*
 *  ======== TimestampProvider_get64 ========
 */
void TimestampProvider_get64(Types_Timestamp64 *result)
{
    unsigned int key;
    uint64_t timestamp;

    key = Hwi_disable();

    /*
     * The order of calling getTickCount() and getExpiredCounts()
     * MUST be preserved to maintain rollover count coherency
     */
    timestamp = (uint64_t)Timer_getTickCount();
    timestamp *= (uint64_t)Timer_getPeriod(TimestampProvider_module->timer);
    timestamp += (uint64_t)Timer_getExpiredCounts(TimestampProvider_module->timer);

    Hwi_restore(key);

    result->hi = (uint32_t)(timestamp >> 32);
    result->lo = (uint32_t)timestamp;
}

/*
 *  ======== TimestampProvider_getFreq ========
 */
void TimestampProvider_getFreq(Types_FreqHz *freq)
{
    BIOS_getCpuFreq(freq);
}



