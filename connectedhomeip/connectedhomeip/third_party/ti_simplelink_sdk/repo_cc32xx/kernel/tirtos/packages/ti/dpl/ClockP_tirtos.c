/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
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
 *  ======== ClockP_tirtos.c ========
 */

#include <ti/drivers/dpl/ClockP.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Types.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>


/*
 *  ======== ClockP_construct ========
 */
ClockP_Handle ClockP_construct(ClockP_Struct *handle, ClockP_Fxn clockFxn,
        uint32_t timeout, ClockP_Params *params)
{
    Clock_Params clockParams;
    Clock_Handle clock;

    if (params == NULL) {
        Clock_construct((Clock_Struct *)handle, (Clock_FuncPtr)clockFxn,
                timeout, NULL);
    }
    else {
        Clock_Params_init(&clockParams);
        clockParams.arg = params->arg;
        clockParams.startFlag = params->startFlag;
        clockParams.period = params->period;
        Clock_construct((Clock_Struct *)handle, (Clock_FuncPtr)clockFxn,
                timeout, &clockParams);
    }

    clock = Clock_handle((Clock_Struct *)handle);

    return ((ClockP_Handle)clock);
}

/*
 *  ======== ClockP_create ========
 */
ClockP_Handle ClockP_create(ClockP_Fxn clockFxn, uint32_t timeout,
        ClockP_Params *params)
{
    Clock_Handle handle;
    Clock_Params clockParams;

    /* Use 0 for timeout since we are only doing one-shot clocks */
    if (params == NULL) {
        handle = Clock_create((Clock_FuncPtr)clockFxn, timeout, NULL,
                Error_IGNORE);
    }
    else {
        Clock_Params_init(&clockParams);
        clockParams.arg = params->arg;
        clockParams.startFlag = params->startFlag;
        clockParams.period = params->period;
        handle = Clock_create((Clock_FuncPtr)clockFxn, timeout, &clockParams,
                Error_IGNORE);
    }

    return ((ClockP_Handle)handle);
}

/*
 *  ======== ClockP_delete ========
 */
void ClockP_delete(ClockP_Handle handle)
{
    Clock_Handle clock = (Clock_Handle)handle;

    Clock_delete(&clock);
}

/*
 *  ======== ClockP_destruct ========
 */
void ClockP_destruct(ClockP_Struct *clockP)
{
    Clock_destruct((Clock_Struct *)clockP);
}

/*
 *  ======== ClockP_getCpuFreq ========
 */
void ClockP_getCpuFreq(ClockP_FreqHz *freq)
{
    BIOS_getCpuFreq((Types_FreqHz *)freq);
}

/*
 *  ======== ClockP_getSystemTickPeriod ========
 */
uint32_t ClockP_getSystemTickPeriod(void)
{
    return (Clock_tickPeriod);
}

/*
 *  ======== ClockP_getSystemTicks ========
 */
uint32_t ClockP_getSystemTicks(void)
{
    return (Clock_getTicks());
}

/*
 *  ======== ClockP_getTimeout ========
 */
uint32_t ClockP_getTimeout(ClockP_Handle handle)
{
    return (Clock_getTimeout((Clock_Handle)handle));
}

/*
 *  ======== ClockP_isActive ========
 */
bool ClockP_isActive(ClockP_Handle handle)
{
    return (Clock_isActive((Clock_Handle)handle));
}

/*
 *  ======== ClockP_Params_init ========
 */
void ClockP_Params_init(ClockP_Params *params)
{
    params->arg = 0;
    params->startFlag = false;
    params->period = 0;
}

/*
 *  ======== ClockP_setTimeout ========
 */
void ClockP_setTimeout(ClockP_Handle handle, uint32_t timeout)
{
    Clock_setTimeout((Clock_Handle)handle, timeout);
}

/*
 *  ======== ClockP_setPeriod ========
 */
void ClockP_setPeriod(ClockP_Handle handle, uint32_t period)
{
    Clock_setPeriod((Clock_Handle)handle, period);
}

/*
 *  ======== ClockP_start ========
 */
void ClockP_start(ClockP_Handle handle)
{
    Clock_start((Clock_Handle)handle);
}

/*
 *  ======== ClockP_stop ========
 */
void ClockP_stop(ClockP_Handle handle)
{
    Clock_stop((Clock_Handle)handle);
}

/*
 *  ======== ClockP_sleep ========
 */
void ClockP_sleep(uint32_t sec)
{
    unsigned long timeout;

    timeout = ((unsigned long)sec * 1000000L) / Clock_tickPeriod;

    Task_sleep((uint32_t)timeout);
}

/*
 *  ======== ClockP_usleep ========
 */
void ClockP_usleep(uint32_t usec)
{
    uint32_t timeout;

    /* Clock_tickPeriod is the Clock period in microsecnds */
    timeout = (uint32_t)((usec + Clock_tickPeriod / 2) / Clock_tickPeriod);

    Task_sleep(timeout);
}

/*
 *  ======== ClockP_staticObjectSize ========
 */
size_t ClockP_staticObjectSize(void)
{
    return (sizeof(Clock_Struct));
}
