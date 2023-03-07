/*
 * Copyright (c) 2016-2020, Texas Instruments Incorporated
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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/ClockP.h>

#include <ti/drivers/Timer.h>
#include <ti/drivers/timer/TimerSupport.h>

/* Default Parameters */
static const Timer_Params defaultParams = {
    .timerMode     = Timer_ONESHOT_BLOCKING,
    .periodUnits   = Timer_PERIOD_COUNTS,
    .timerCallback = NULL,
    .period        = (uint16_t) ~0
};

/*
 *  ======== Timer_init ========
 */
void Timer_init(void)
{
    /* Do nothing */
}

/*
 *  ======== Timer_Params_init ========
 */
void Timer_Params_init(Timer_Params *params)
{
    *params = defaultParams;
}

/*
 *  ======== Timer_setPeriod ========
 */
int32_t Timer_setPeriod(Timer_Handle handle, Timer_PeriodUnits periodUnits, uint32_t period)
 {
    Timer_Object        *object = handle->object;
    ClockP_FreqHz        clockFreq;

    ClockP_getCpuFreq(&clockFreq);

    if (periodUnits == Timer_PERIOD_US) {
        /* Checks if the calculated period will fit in 32-bits */
        if (period >= ((uint32_t) ~0) / (clockFreq.lo / 1000000)) {
            return (Timer_STATUS_ERROR);
        }
        period = period * (clockFreq.lo / 1000000);
    }
    else if (periodUnits == Timer_PERIOD_HZ) {
        /* If period > clockFreq */
        if ((period = clockFreq.lo / period) == 0) {
            return (Timer_STATUS_ERROR);
        }
    }

    /* If using a half timer */
    if (!TimerSupport_timerFullWidth(handle)) {
        if (period > 0xFFFF) {
            /* 24-bit resolution for the half timer */
            if (period >= (1 << 24)) {
                return (Timer_STATUS_ERROR);
            }
        }
    }

    object->period = period;

    TimerSupport_timerLoad(handle);

    return (Timer_STATUS_SUCCESS);
 }

/*
 *  ======== Timer_start ========
 */
int32_t Timer_start(Timer_Handle handle)
{
    Timer_Object              *object = handle->object;
    uintptr_t                  key;

    /* Check if timer is already running */
    key = HwiP_disable();

    if (object->isRunning) {
        HwiP_restore(key);
        return (Timer_STATUS_ERROR);
    }

    object->isRunning = true;

    TimerSupport_timerEnable(handle);

    HwiP_restore(key);

    if (object->mode == Timer_ONESHOT_BLOCKING) {
        /* Pend forever, ~0 */
        SemaphoreP_pend(object->semHandle, SemaphoreP_WAIT_FOREVER);
    }

    return (Timer_STATUS_SUCCESS);
}

/*
 *  ======== Timer_stop ========
 */
void Timer_stop(Timer_Handle handle)
{
    Timer_Object              *object = handle->object;
    uintptr_t                  key;
    bool                       flag = false;

    key = HwiP_disable();

    if (object->isRunning) {
        object->isRunning = false;
        /* Post the Semaphore when called from the Hwi */
        if (object->mode == Timer_ONESHOT_BLOCKING) {
            flag = true;
        }
        TimerSupport_timerDisable(handle);
    }

    HwiP_restore(key);

    if (flag) {
        SemaphoreP_post(object->semHandle);
    }
}
