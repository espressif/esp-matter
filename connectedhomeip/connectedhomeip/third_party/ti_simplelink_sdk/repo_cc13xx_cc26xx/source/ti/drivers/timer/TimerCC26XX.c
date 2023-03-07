/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/ClockP.h>

#include <ti/drivers/Timer.h>
#include <ti/drivers/timer/GPTimerCC26XX.h>
#include <ti/drivers/timer/TimerCC26XX.h>
#include <ti/drivers/timer/TimerSupport.h>

/* Internal callback function */
static void TimerCC26XX_callbackfxn(GPTimerCC26XX_Handle gptHandle, GPTimerCC26XX_IntMask intMask);

extern uint_least8_t Timer_count;

/* Default Parameters */
static const Timer_Params defaultParams = {
    .timerMode     = Timer_ONESHOT_BLOCKING,
    .periodUnits   = Timer_PERIOD_COUNTS,
    .timerCallback = NULL,
    .period        = (uint16_t) ~0
};

/*
 *  ======== Timer_close ========
 */
void Timer_close(Timer_Handle handle)
{
    TimerCC26XX_Object *object = handle->object;

    /* Unregister Interrupts and Callback */
    GPTimerCC26XX_unregisterInterrupt(object->gptHandle);

    /* Destruct semaphore */
    if (object->semHandle != NULL) {
        SemaphoreP_destruct(&(object->semStruct));
        object->semHandle = NULL;
    }

    /* Close and delete gptHandle */
    GPTimerCC26XX_close(object->gptHandle);
    object->gptHandle = NULL;

    /* Clear isOpen flag */
    object->isOpen = false;
}

/*
 *  ======== Timer_control ========
 */
int_fast16_t Timer_control(Timer_Handle handle,
        uint_fast16_t cmd, void *arg)
{
    return (Timer_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== Timer_getCount ========
 */
uint32_t Timer_getCount(Timer_Handle handle)
{
    TimerCC26XX_Object const *object = handle->object;
    uint32_t count;

    count = GPTimerCC26XX_getValue(object->gptHandle);

    return count;
}

/*
 *  ======== TimerCC26XX_callbackfxn ========
 */
void TimerCC26XX_callbackfxn(GPTimerCC26XX_Handle gptHandle, GPTimerCC26XX_IntMask intMask)
{
    Timer_Handle handle;
    /* Get Timer Handle from arg in GPTimerCC26XX_Object */
    handle = (Timer_Handle)GPTimerCC26XX_getArg(gptHandle);
    TimerCC26XX_Object *object = handle->object;

    /* Callback not created when using Timer_FREE_RUNNING */
    if (object->mode != Timer_CONTINUOUS_CALLBACK) {
        Timer_stop(handle);
    }
    if (object->mode != Timer_ONESHOT_BLOCKING) {
        object->callBack(handle, Timer_STATUS_SUCCESS);
    }
}

/*
 *  ======== TimerCC26XX_open ========
 */
Timer_Handle Timer_open(uint_least8_t index, Timer_Params *params)
{
    Timer_Handle               handle = NULL;
    TimerCC26XX_HWAttrs const *hwAttrs;
    TimerCC26XX_Object        *object;
    GPTimerCC26XX_Params       gptParams;
    int_fast16_t               status;

    /* Verify driver index and state */
    if (index < Timer_count) {
        /* If parameters are NULL use defaults */
        if (params == NULL) {
            params = (Timer_Params *) &defaultParams;
        }

        /* Get handle for this driver instance */
        handle = (Timer_Handle) &(Timer_config[index]);
        object = handle->object;
        hwAttrs = handle->hwAttrs;
    }
    else
    {
        return (NULL);
    }

    /* Check if timer is already open */
    uint32_t key = HwiP_disable();
    if (object->isOpen)
    {
        HwiP_restore(key);
        return (NULL);
    }
    object->isOpen = true;
    HwiP_restore(key);

    /* Check for valid parameters */
    if (((params->timerMode == Timer_ONESHOT_CALLBACK ||
          params->timerMode == Timer_CONTINUOUS_CALLBACK) &&
          params->timerCallback == NULL) ||
          params->period == 0) {

        object->isOpen = false;
        return (NULL);
    }

    object->mode = params->timerMode;
    object->period = params->period;
    object->callBack = params->timerCallback;
    object->isRunning = false;

    /* Initialize gptParams to default values */
    GPTimerCC26XX_Params_init(&gptParams);

    /* Convert Timer mode to GPT mode */
    switch (params->timerMode) {
        case Timer_ONESHOT_CALLBACK:
        case Timer_ONESHOT_BLOCKING:
            gptParams.mode = GPT_MODE_ONESHOT;
            break;

        case Timer_CONTINUOUS_CALLBACK:
        case Timer_FREE_RUNNING:
        default:
            gptParams.mode = GPT_MODE_PERIODIC;
            break;
    }

    /* Convert Timer width to GPT width */
    switch (hwAttrs->subTimer) {
        case TimerCC26XX_timer32:
            gptParams.width = GPT_CONFIG_32BIT;
            break;

        case TimerCC26XX_timer16A:
        case TimerCC26XX_timer16B:
        default:
            gptParams.width = GPT_CONFIG_16BIT;
            break;
    }

    /* Pass a GPTimer index and params to GPTimer open call */
    object->gptHandle = GPTimerCC26XX_open(hwAttrs->gpTimerUnit, &gptParams);

    /* Check if returned gptHandle is null */
    if (object->gptHandle == NULL) {
        object->isOpen = false;
        return (NULL);
    }

    if (params->timerMode != Timer_FREE_RUNNING) {
        /* Set Timer Period and Units */
        status = Timer_setPeriod(handle, params->periodUnits, object->period);
        if (status != Timer_STATUS_SUCCESS) {
            Timer_close(handle);
            return (NULL);
        }

        /* Create the semaphore for blocking mode */
        if (params->timerMode == Timer_ONESHOT_BLOCKING) {
            object->semHandle = SemaphoreP_constructBinary(&(object->semStruct), 0);

            if (object->semHandle == NULL) {
                Timer_close(handle);
                return (NULL);
            }
        }

        /* Set custom arg in GPTimerCC26XX_Object to Timer handle */
        GPTimerCC26XX_setArg(object->gptHandle, (void *)handle);

        /* Register Interrupt */
        GPTimerCC26XX_registerInterrupt(object->gptHandle, TimerCC26XX_callbackfxn, GPT_INT_TIMEOUT);
    }

    return (handle);
}

/*
 *  ======== TimerSupport_timerDisable ========
 */
void TimerSupport_timerDisable(Timer_Handle handle)
{
    TimerCC26XX_Object *object = handle->object;

    GPTimerCC26XX_stop(object->gptHandle);
}

/*
 *  ======== TimerSupport_timerEnable ========
 */
void TimerSupport_timerEnable(Timer_Handle handle)
{
    TimerCC26XX_Object *object = handle->object;

    GPTimerCC26XX_start(object->gptHandle);
}

/*
 *  ======== TimerSupport_timerFullWidth ========
 */
bool TimerSupport_timerFullWidth(Timer_Handle handle)
{
    TimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;

    if (hwAttrs->subTimer == TimerCC26XX_timer32)
    {
        return (true);
    }

    return (false);
}

/*
 *  ======== TimerSupport_timerLoad ========
 */
void TimerSupport_timerLoad(Timer_Handle handle)
{
    TimerCC26XX_Object *object = handle->object;

    GPTimerCC26XX_setLoadValue(object->gptHandle, object->period);
}
