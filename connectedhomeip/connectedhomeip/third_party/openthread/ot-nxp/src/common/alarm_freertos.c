/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "ot_platform_common.h"

#include <openthread/tasklet.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>

#include <FreeRTOS.h>
#include <assert.h>
#include <semphr.h>
#include <timers.h>

#define ALARM_TIMER_MS_2_TICKS(ms) ((ms) / portTICK_PERIOD_MS)
#define ALARM_TIMER_TICKS_2_MS(ticks) (((uint64_t)ticks) * portTICK_PERIOD_MS)

static bool              alarmFired  = false;
static TimerHandle_t     alarmTimer  = NULL;
static uint32_t          timerId     = 0;
static SemaphoreHandle_t mutexHandle = NULL;

static void alarmTimerCallback(TimerHandle_t pxTimer)
{
    xSemaphoreTake(mutexHandle, portMAX_DELAY);
    alarmFired = true;
    xSemaphoreGive(mutexHandle);
    /* notify the main loop if rtos */
    otTaskletsSignalPending(NULL);
    OT_PLAT_DBG("alarmFired = true");
}

void otPlatAlarmInit(void)
{
    mutexHandle = xSemaphoreCreateMutex();
    assert(mutexHandle != NULL);
    alarmTimer = xTimerCreate("otAlarm", 100, pdFALSE, &timerId, alarmTimerCallback);
    assert(alarmTimer != NULL);
}

void otPlatAlarmDeinit(void)
{
    otPlatAlarmMilliStop(NULL);
    xTimerDelete(alarmTimer, 0);
    vSemaphoreDelete(mutexHandle);
}

void otPlatAlarmProcess(otInstance *aInstance)
{
    xSemaphoreTake(mutexHandle, portMAX_DELAY);
    if (alarmFired)
    {
        alarmFired = false;
        xSemaphoreGive(mutexHandle);
#if OPENTHREAD_CONFIG_DIAG_ENABLE
        if (otPlatDiagModeGet())
        {
            otPlatDiagAlarmFired(aInstance);
        }
        else
#endif
        {
            OT_PLAT_DBG("otPlatAlarmMilliFired");
            otPlatAlarmMilliFired(aInstance);
        }
    }
    else
    {
        xSemaphoreGive(mutexHandle);
    }
}

uint64_t otPlatTimeGet(void)
{
    TickType_t ticks = xTaskGetTickCount();
    /* Return the value in us */
    return ALARM_TIMER_TICKS_2_MS(ticks) * 1000;
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);
    uint32_t now;
    uint32_t nextExpiryTime = 0;
    bool     startTimer     = false;
    OT_PLAT_DBG("aT0 %d duration = %d", aT0, aDt);
    xSemaphoreTake(mutexHandle, portMAX_DELAY);
    now = otPlatAlarmMilliGetNow();
    if (aT0 + aDt > now)
    {
        if (xTimerIsTimerActive(alarmTimer))
        {
            nextExpiryTime = ALARM_TIMER_TICKS_2_MS(xTimerGetExpiryTime(alarmTimer));

            if ((aT0 + aDt) < (nextExpiryTime))
            {
                startTimer = true;
            }
        }
        else
        {
            startTimer = true;
        }

        if (startTimer)
        {
            xTimerChangePeriod(alarmTimer, ALARM_TIMER_MS_2_TICKS(aT0 + aDt - now), 0);
        }
        xSemaphoreGive(mutexHandle);
    }
    else
    {
        xSemaphoreGive(mutexHandle);
        alarmTimerCallback(NULL);
    }
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    xSemaphoreTake(mutexHandle, portMAX_DELAY);
    if (xTimerIsTimerActive(alarmTimer))
    {
        xTimerStop(alarmTimer, 0);
    }
    xSemaphoreGive(mutexHandle);
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    TickType_t ticks = xTaskGetTickCount();
    /* Return the value in ms */
    return ALARM_TIMER_TICKS_2_MS(ticks);
}
