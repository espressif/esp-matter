/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 *  ======== ClockSupport.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/family/arm/v8m/cc26x4/Timer.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/ClockSupport.h>

#include <ti/sysbios/runtime/Error.h>

Timer_Struct ClockSupport_timerStruct;

/*
 * ======== ClockSupport_init ========
 */
void ClockSupport_init(void)
{
    Timer_Params timerParams;

    Timer_startup();

    Timer_Params_init(&timerParams);
    timerParams.period = Clock_tickPeriod;
    timerParams.periodType = Timer_PeriodType_MICROSECS;

    Timer_construct(&ClockSupport_timerStruct, 0, (Timer_FuncPtr)Clock_tick,
        &timerParams, NULL);
}

/*
 * ======== ClockSupport_getMaxTicks ========
 */
uint32_t ClockSupport_getMaxTicks(void)
{
    return (Timer_getMaxTicks(&ClockSupport_timerStruct));
}

/*
 * ======== ClockSupport_setNextTick ========
 */
void ClockSupport_setNextTick(uint32_t ticks)
{
    Timer_setNextTick(&ClockSupport_timerStruct, ticks);
}

/*
 * ======== ClockSupport_start ========
 */
void ClockSupport_start(void)
{
    Timer_start(&ClockSupport_timerStruct);
}

/*
 * ======== ClockSupport_stop ========
 */
void ClockSupport_stop(void)
{
    Timer_stop(&ClockSupport_timerStruct);
}

/*
 * ======== ClockSupport_setPeriod ========
 */
void ClockSupport_setPeriod(uint32_t period)
{
    Timer_setPeriod(&ClockSupport_timerStruct, period);
}

/*
 * ======== ClockSupport_setPeriodMicroSecs ========
 */
bool ClockSupport_setPeriodMicroSecs(uint32_t period)
{
    return (Timer_setPeriodMicroSecs(&ClockSupport_timerStruct, period));
}

/*
 * ======== ClockSupport_getPeriod ========
 */
uint32_t ClockSupport_getPeriod(void)
{
    return (Timer_getPeriod(&ClockSupport_timerStruct));
}

/*
 * ======== ClockSupport_getCurrentTick ========
 */
uint32_t ClockSupport_getCurrentTick(bool save)
{
    return (Timer_getCurrentTick(&ClockSupport_timerStruct, save));
}

/*
 * ======== ClockSupport_getFreq ========
 */
void ClockSupport_getFreq(Types_FreqHz *freq)
{
    Timer_getFreq(&ClockSupport_timerStruct, freq);
}

