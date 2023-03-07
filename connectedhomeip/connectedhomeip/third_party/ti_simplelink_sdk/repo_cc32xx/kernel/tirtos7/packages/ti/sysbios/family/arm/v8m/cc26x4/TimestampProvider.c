/*
 * Copyright (c) 2014-2020, Texas Instruments Incorporated
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

#include <xdc/runtime/Types.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/family/arm/v8m/cc26x4/Timer.h>

#include <ti/sysbios/family/arm/v8m/cc26x4/TimestampProvider.h>

#include <ti/sysbios/knl/ClockSupport.h>

Timer_Struct TimestampProvider_timerStruct;

TimestampProvider_Module_State TimestampProvider_Module_state;

/*
 *  ======== TimestampProvider_init ========
 */
void TimestampProvider_init()
{
#if ti_sysbios_family_arm_v8m_cc26x4_TimestampProvider_useClockTimer_D
    extern Timer_Struct ClockSupport_timerStruct;
    TimestampProvider_module->timer = &ClockSupport_timerStruct;
#else
    Timer_Params timerParams;

    Timer_startup();

    Timer_Params_init(&timerParams);
    timerParams.period = Timer_MAX_PERIOD;
    timerParams.periodType = Timer_PeriodType_COUNTS;
    timerParams.startMode = Timer_StartMode_USER;

    Timer_construct(&TimestampProvider_timerStruct, 0, NULL,
        &timerParams, NULL);
    TimestampProvider_module->timer = &TimestampProvider_timerStruct;
    Timer_start(TimestampProvider_module->timer);
#endif
}

/*
 *  ======== TimestampProvider_get32 ========
 */
uint32_t TimestampProvider_get32()
{
    uint64_t time;

    time = Timer_getCount64(TimestampProvider_module->timer);
    time = time >> 16;

    return ((uint32_t) time);
}

/*
 *  ======== TimestampProvider_get64 ========
 */
void TimestampProvider_get64(Types_Timestamp64 *result)
{
    uint64_t time;

    time = Timer_getCount64(TimestampProvider_module->timer);
    time = time >> 16;

    result->lo = (uint32_t) time;
    result->hi = (uint32_t) ((uint64_t) time >> 32);
}

/*
 *  ======== TimestampProvider_getCounterFreq ========
 */
void TimestampProvider_getFreq(Types_FreqHz *freq)
{
    /* reporting only the upper 48-bits of RTC count */
    freq->lo = 65536;
    freq->hi = 0;
}
