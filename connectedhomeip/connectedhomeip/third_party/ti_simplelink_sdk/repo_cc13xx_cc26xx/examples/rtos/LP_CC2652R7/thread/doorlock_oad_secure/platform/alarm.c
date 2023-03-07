/******************************************************************************

 @file alarm.c

 @brief TIRTOS platform specific alarm functions for OpenThread

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/******************************************************************************
 Includes
 *****************************************************************************/

#include <openthread/config.h>

/* Standard Library Header files */
#include <stdbool.h>
#include <stdint.h>

/* POSIX Header files */
#include <time.h>

/* OpenThread public API Header files */
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>

#include "system.h"

/******************************************************************************
 Local variables
 *****************************************************************************/

static uint32_t Alarm_time0   = 0;
static uint32_t Alarm_time    = 0;
static timer_t  Alarm_timerid = 0;
static bool     Alarm_running = false;

/**
 * Handler for the POSIX clock callback.
 */
void Alarm_handler(union sigval val)
{
    (void)val;
    platformAlarmSignal();
}

/**
 * Function documented in system.h
 */
void platformAlarmInit(void)
{
    struct timespec zeroTime = {0};
    struct sigevent event =
    {
        .sigev_notify_function = Alarm_handler,
        .sigev_notify = SIGEV_SIGNAL,
    };

    clock_settime(CLOCK_MONOTONIC, &zeroTime);

    timer_create(CLOCK_MONOTONIC, &event, &Alarm_timerid);

    Alarm_running = false;
}

/**
 * Function documented in platform/alarm-milli.h
 */
uint32_t otPlatAlarmMilliGetNow(void)
{
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);

    return (now.tv_sec * 1000U) + ((now.tv_nsec / 1000000U) % 1000);
}

/**
 * Function documented in platform/alarm-milli.h
 */
void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    (void)aInstance;
    struct itimerspec timerspec = {0};
    uint32_t          delta     = (otPlatAlarmMilliGetNow() - aT0);

    Alarm_time0   = aT0;
    Alarm_time    = aDt;
    Alarm_running = true;

    if (delta >= aDt)
    {
        // alarm is in the past
        platformAlarmSignal();
    }
    else
    {
        timerspec.it_value.tv_sec  = ((aDt - delta) / 1000U);
        timerspec.it_value.tv_nsec = (((aDt - delta) % 1000U) * 1000000U);

        timer_settime(Alarm_timerid, 0, &timerspec, NULL);
    }
}

/**
 * Function documented in platform/alarm-milli.h
 */
void otPlatAlarmMilliStop(otInstance *aInstance)
{
    (void)aInstance;
    struct itimerspec zeroTime = {0};

    timer_settime(Alarm_timerid, TIMER_ABSTIME, &zeroTime, NULL);
    Alarm_running = false;
}

/**
 * Function documented in system.h
 */
void platformAlarmProcess(otInstance *aInstance)
{
    if (Alarm_running)
    {
        uint32_t offsetTime = otPlatAlarmMilliGetNow() - Alarm_time0;

        if (Alarm_time <= offsetTime)
        {
            Alarm_running = false;
#if OPENTHREAD_CONFIG_DIAG_ENABLE

            if (otPlatDiagModeGet())
            {
                otPlatDiagAlarmFired(aInstance);
            }
            else
#endif /* OPENTHREAD_CONFIG_DIAG_ENABLE */
            {
                otPlatAlarmMilliFired(aInstance);
            }
        }
        else
        {
            struct itimerspec timerspec = {0};

            timer_gettime(Alarm_timerid, &timerspec);
            if (0U == timerspec.it_value.tv_sec && 0U == timerspec.it_value.tv_nsec)
            {
                /* Timer fired a bit early, notify we still need processing. */
                platformAlarmSignal();
            }
        }
    }
}
