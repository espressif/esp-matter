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
#include <openthread/platform/alarm-micro.h>

#include "system.h"

/******************************************************************************
 Local variables
 *****************************************************************************/

static uint32_t AlarmMicro_time0   = 0;
static uint32_t AlarmMicro_time    = 0;
static timer_t  AlarmMicro_timerid = 0;
static bool     AlarmMicro_running = false;

/**
 * Handler for the POSIX clock callback.
 */
void AlarmMicro_handler(union sigval val)
{
    (void)val;
    platformAlarmMicroSignal();
}

/**
 * Function documented in system.h
 */
void platformAlarmMicroInit(void)
{
    struct timespec zeroTime = {0};
    struct sigevent event =
    {
        .sigev_notify_function = AlarmMicro_handler,
        .sigev_notify = SIGEV_SIGNAL,
    };

    clock_settime(CLOCK_MONOTONIC, &zeroTime);

    timer_create(CLOCK_MONOTONIC, &event, &AlarmMicro_timerid);

    AlarmMicro_running = false;
}
/**
 * Function documented in platform/alarm-micro.h
 */
uint32_t otPlatAlarmMicroGetNow(void)
{
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);

    return (now.tv_sec * 1000000U) + ((now.tv_nsec / 1000U) % 1000000);
}

/**
 * Function documented in platform/alarm-micro.h
 */
void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    (void)aInstance;
    struct itimerspec timerspec = {0};
    uint32_t          delta     = (otPlatAlarmMicroGetNow() - aT0);

    AlarmMicro_time0   = aT0;
    AlarmMicro_time    = aDt;
    AlarmMicro_running = true;

    if (delta >= aDt)
    {
        // alarm is in the past
        platformAlarmMicroSignal();
    }
    else
    {
        timerspec.it_value.tv_sec  = ((aDt - delta) / 1000000U);
        timerspec.it_value.tv_nsec = (((aDt - delta) % 1000000U) * 1000U);

        timer_settime(AlarmMicro_timerid, 0, &timerspec, NULL);
    }
}

/**
 * Function documented in platform/alarm-micro.h
 */
void otPlatAlarmMicroStop(otInstance *aInstance)
{
    (void)aInstance;
    struct itimerspec zeroTime = {0};

    timer_settime(AlarmMicro_timerid, TIMER_ABSTIME, &zeroTime, NULL);
    AlarmMicro_running = false;
}

/**
 * Function documented in system.h
 */
void platformAlarmMicroProcess(otInstance *aInstance)
{
    if (AlarmMicro_running)
    {
        uint32_t offsetTime = otPlatAlarmMicroGetNow() - AlarmMicro_time0;

        if (AlarmMicro_time <= offsetTime)
        {
            AlarmMicro_running = false;

            otPlatAlarmMicroFired(aInstance);
        }
        else
        {
            struct itimerspec timerspec = {0};

            timer_gettime(AlarmMicro_timerid, &timerspec);
            if (0U == timerspec.it_value.tv_sec && 0U == timerspec.it_value.tv_nsec)
            {
                /* Timer fired a bit early, notify we still need processing. */
                platformAlarmMicroSignal();
            }
        }
    }
}
