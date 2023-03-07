/*
 *  Copyright (c) 2019, The OpenThread Authors.
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

/**
 * @file
 *   This file implements the OpenThread platform abstraction for the alarm.
 *
 */

#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "openthread-system.h"
#include <openthread/config.h>
#include <openthread/platform/alarm-micro.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>
#include "common/logging.hpp"

#include "platform-efr32.h"
#include "utils/code_utils.h"

#include "em_core.h"
#include "rail.h"
#include "sl_sleeptimer.h"

// According to EFR datasheets, HFXO is ± 40 ppm and LFXO (at least for MG12) is
// -8 to +40 ppm.  Assuming average as worst case.
#define HFXO_ACCURACY 40
#define LFXO_ACCURACY 24

// millisecond timer (sleeptimer)
static sl_sleeptimer_timer_handle_t sl_handle;
static uint32_t                     sMsAlarm     = 0;
static bool                         sIsMsRunning = false;

// microsecond timer (RAIL timer)
static uint32_t sUsAlarm     = 0;
static bool     sIsUsRunning = false;

static void AlarmCallback(sl_sleeptimer_timer_handle_t *aHandle, void *aData)
{
    OT_UNUSED_VARIABLE(aHandle);
    OT_UNUSED_VARIABLE(aData);
    otSysEventSignalPending();
}

static void radioTimerExpired(RAIL_Handle_t cbArg)
{
    OT_UNUSED_VARIABLE(cbArg);
    otSysEventSignalPending();
}

void efr32AlarmInit(void)
{
    memset(&sl_handle, 0, sizeof sl_handle);
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    (void) RAIL_ConfigMultiTimer(true);
#endif
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    uint64_t    ticks;
    uint64_t    now;
    sl_status_t status;

    ticks  = sl_sleeptimer_get_tick_count64();
    status = sl_sleeptimer_tick64_to_ms(ticks, &now);
    assert(status == SL_STATUS_OK);
    return (uint32_t)now;
}

uint32_t otPlatTimeGetXtalAccuracy(void)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    // For sleepies, we need to account for the low-frequency crystal
    // accuracy when they go to sleep.  Accounting for that as well,
    // for the worst case.
    if (efr32AllowSleepCallback())
    {
        return HFXO_ACCURACY + LFXO_ACCURACY;
    }
#endif
    return HFXO_ACCURACY;
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);
    sl_status_t status;
    int32_t     remaining;
    uint32_t    ticks;

    sl_sleeptimer_stop_timer(&sl_handle);

    sMsAlarm     = aT0 + aDt;
    remaining  = (int32_t)(sMsAlarm - otPlatAlarmMilliGetNow());
    sIsMsRunning = true;

    if (remaining <= 0)
    {
        otSysEventSignalPending();
    }
    else
    {
        status = sl_sleeptimer_ms32_to_tick(remaining, &ticks);
        assert(status == SL_STATUS_OK);

        status = sl_sleeptimer_start_timer(&sl_handle, ticks, AlarmCallback, NULL, 0,
                                           SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
        assert(status == SL_STATUS_OK);
    }
}

uint32_t efr32AlarmPendingTime(void)
{
    uint32_t remaining = 0;
    uint32_t now = otPlatAlarmMilliGetNow();
    if (sIsMsRunning && (sMsAlarm > now))
    {
        remaining = sMsAlarm - now;
    }
    return remaining;
}

bool efr32AlarmIsRunning(otInstance *aInstance)
{
    return (otInstanceIsInitialized(aInstance) ? sIsMsRunning :  false);
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    sl_sleeptimer_stop_timer(&sl_handle);
    sIsMsRunning = false;
}

void efr32AlarmProcess(otInstance *aInstance)
{
    int32_t remaining;
    bool alarmMilliFired = false;
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    bool alarmMicroFired = false;
#endif

    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    if (sIsMsRunning)
    {
        remaining = (int32_t)(sMsAlarm - otPlatAlarmMilliGetNow());
        if (remaining <= 0)
        {
            otPlatAlarmMilliStop(aInstance);
            alarmMilliFired = true;
        }
    }

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    if (sIsUsRunning)
    {
        remaining = (int32_t)(sUsAlarm - otPlatAlarmMicroGetNow());
        if (remaining <= 0)
        {
            otPlatAlarmMicroStop(aInstance);
            alarmMicroFired = true;
        }
    }
#endif

    CORE_EXIT_ATOMIC();

    if (alarmMilliFired)
    {
#if OPENTHREAD_CONFIG_DIAG_ENABLE
        if (otPlatDiagModeGet())
        {
            otPlatDiagAlarmFired(aInstance);
        }
        else
#endif
        {
            otPlatAlarmMilliFired(aInstance);
        }
    }

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    if (alarmMicroFired)
    {
        otPlatAlarmMicroFired(aInstance);
    }
#endif
}

uint32_t otPlatAlarmMicroGetNow(void)
{
    return RAIL_GetTime();
}

// Note: This function should be called at least once per wrap
// period for the wrap-around logic to work below
uint64_t otPlatTimeGet(void)
{
  static uint32_t timerWraps = 0U;
  static uint32_t prev32TimeUs = 0U;
  uint32_t now32TimeUs;
  uint64_t now64TimeUs;
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  now32TimeUs = RAIL_GetTime();
  if (now32TimeUs < prev32TimeUs) {
    timerWraps += 1U;
  }
  prev32TimeUs = now32TimeUs;
  now64TimeUs = ((uint64_t)timerWraps << 32) + now32TimeUs;
  CORE_EXIT_CRITICAL();
  return now64TimeUs;
}

// Note: If we ever use OpenThread in a multi-instance scenario, we need to
// switch to using RAIL_SetMultiTimer / RAIL_CancelMultiTimer calls below.
void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);
    RAIL_Status_t status;
    int32_t       remaining;

    RAIL_CancelTimer(gRailHandle);

    sUsAlarm     = aT0 + aDt;
    remaining    = (int32_t)(sUsAlarm - otPlatAlarmMicroGetNow());
    sIsUsRunning = true;

    if (remaining <= 0)
    {
        otSysEventSignalPending();
    }
    else
    {
        status = RAIL_SetTimer(gRailHandle, remaining, RAIL_TIME_DELAY, &radioTimerExpired);
        assert(status == RAIL_STATUS_NO_ERROR);
    }
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    RAIL_CancelTimer(gRailHandle);
    sIsUsRunning = false;
}
