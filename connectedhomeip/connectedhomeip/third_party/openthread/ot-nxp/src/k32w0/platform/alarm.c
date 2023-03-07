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

#include "MicroSpecific_arm_sdk2.h"
#include "TMR_Adapter.h"
#include "TimersManager.h"
#include "fsl_clock.h"
#include "fsl_ctimer.h"
#include "fsl_device_registers.h"
#include "fsl_os_abstraction.h"
#include "fsl_wtimer.h"
#include "openthread-system.h"
#include "platform-k32w.h"
#include <common/logging.hpp>
#include <openthread/platform/alarm-micro.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/time.h>

#ifdef ALARM_LOG_ENABLED
#include "dbg_logging.h"
#define ALARM_LOG(fmt, ...)                                                    \
    do                                                                         \
    {                                                                          \
        DbgLogAdd(__FUNCTION__, fmt, VA_NUM_ARGS(__VA_ARGS__), ##__VA_ARGS__); \
    } while (0);
#else
#define ALARM_LOG(...)
#endif

#define ALARM_USE_CTIMER 0
#define ALARM_USE_WTIMER 1

/* Timer frequency in Hz needed for 1ms tick */
#define TARGET_FREQ 1000U

#define US_PER_MS 1000ULL

/* 32768 ticks = 1s */
#define US_TO_TICKS32K(x) ((((uint64_t)(x)) << 9) / 15625)

/* Overflow ticks for WTIMER0_LSB to us */
#define WTIMER0_LSB_OVF ((uint64_t)1 << 32)
#define OVF_32bit_US TICKS32kHz_TO_USEC(WTIMER0_LSB_OVF)

/* Every time WTIMER0_LSB overflows (36 hours) we add OVF_32bit_US */
static uint64_t tstp_ovf;

/* Since WTIMER0_LSB overflows quite slow, comparing current timestamp (ticks)
   with previous timestamp it's a reliable way to detect it */
static uint32_t prev_tstp;

static bool     sEventFired = false;
static uint32_t refClk;

#if ALARM_USE_CTIMER
/* Match Configuration for Channel 0 */
static ctimer_match_config_t sMatchConfig = {.enableCounterReset = false,
                                             .enableCounterStop  = false,
                                             .matchValue         = 0x00,
                                             .outControl         = kCTIMER_Output_NoAction,
                                             .outPinInitState    = false,
                                             .enableInterrupt    = true};
#else
static TMR_tsActivityWakeTimerEvent otTimer;
static void                         TMR_ScheduleActivityCallback(void);

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
static bool                         sMicroEventFired = false;
static TMR_tsActivityWakeTimerEvent otMicroTimer;
#endif
#endif

/* Stub function for notifying application of wakeup */
WEAK void App_NotifyWakeup(void);

/**
 * Stub function for notifying application of wakeup
 *
 */
WEAK void App_NotifyWakeup(void)
{
}

void K32WAlarmInit(void)
{
#if ALARM_USE_CTIMER
    ctimer_config_t config;
    CTIMER_GetDefaultConfig(&config);

    /* Get clk frequency and use prescale to lower it */
    refClk = CLOCK_GetFreq(kCLOCK_Timer0);

    config.prescale = refClk / TARGET_FREQ;
    CTIMER_Init(CTIMER0, &config);
    CTIMER_StartTimer(CTIMER0);

    CTIMER_EnableInterrupts(CTIMER0, kCTIMER_Match0InterruptEnable);
    NVIC_ClearPendingIRQ(Timer0_IRQn);
    NVIC_EnableIRQ(Timer0_IRQn);

#else

    /* Handles WTIMER init inside, Wake timer 0 is 41-bit counter and is used for keeping the timestamp */
    Timestamp_Init();

    /* Get clk frequency and use prescale to lower it */
    refClk                = CLOCK_GetFreq(CLOCK_32k_source);
    otTimer.u8Status      = TMR_E_ACTIVITY_FREE;

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    otMicroTimer.u8Status = TMR_E_ACTIVITY_FREE;
#endif
#endif
}

void K32WAlarmClean(void)
{
#if ALARM_USE_CTIMER
    CTIMER_StopTimer(CTIMER0);
    CTIMER_Deinit(CTIMER0);
    CTIMER_DisableInterrupts(CTIMER0, kCTIMER_Match0InterruptEnable);
    NVIC_ClearPendingIRQ(Timer0_IRQn);
#else
    Timestamp_Deinit();
    TMR_eRemoveActivity(&otTimer);

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    TMR_eRemoveActivity(&otMicroTimer);
#endif
#endif
}

void K32WAlarmProcess(otInstance *aInstance)
{
    bool ev1;
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    bool ev2;
#endif

    OSA_InterruptDisable();

    ev1         = sEventFired;
    sEventFired = false;

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    ev2              = sMicroEventFired;
    sMicroEventFired = false;
#endif

    OSA_InterruptEnable();

    if (ev1)
    {
#if OPENTHREAD_ENABLE_DIAG

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
    if (ev2)
    {
        otPlatAlarmMicroFired(aInstance);
    }
#endif
}

#if !ALARM_USE_CTIMER
void alarmStartAt(TMR_tsActivityWakeTimerEvent *t, void (*cb)(void), bool *ev, uint32_t t0, uint32_t dt, bool ms)
{
    uint32_t targetTicks = 0;
    uint32_t now         = 0;
    uint32_t t1          = t0 + dt;

    if (ms)
    {
        now = otPlatAlarmMilliGetNow();
    }
    else
    {
        now = otPlatAlarmMicroGetNow();
    }
    otLogInfoPlat("Start timer: timestamp:%ld, aTo:%ld, aDt:%ld", now, t0, dt);

    /* Check 'now' position in range [t0, t1] */
    if ((now - t0 <= dt) && (t1 - now <= dt))
    {
        /* inside the range: t1 is in the future */
        targetTicks = t1 - now;
    }
    else if ((now - t1) > (t0 - now))
    {
        /* on the left: both t0 and t1 are in the future */
        targetTicks = t1 - now;
    }

    if (ms)
    {
        targetTicks = (uint32_t)MILLISECONDS_TO_TICKS32K(targetTicks);
    }
    else
    {
        targetTicks = (uint32_t)US_TO_TICKS32K(targetTicks);
    }

    if (targetTicks > 0)
    {
        TMR_eRemoveActivity(t);
        if (targetTicks > WTIMER1_MAX_VALUE)
        {
            /* Because timer 1 is only 28-bit counter we need to take into account and event longer than this
            so we arm the timer with the maximum value and re-arm with the remaining time once it fires */
            targetTicks = WTIMER1_MAX_VALUE;
        }
        TMR_eScheduleActivity32kTicks(t, targetTicks, cb);
    }
    else
    {
        *ev = true;
        otSysEventSignalPending();
        App_NotifyWakeup();
    }
}
#endif

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

#if ALARM_USE_CTIMER
    /* Load match register with current counter + app time */
    sMatchConfig.matchValue = aT0 + aDt;

    CTIMER_SetupMatch(CTIMER0, kCTIMER_Match_0, &sMatchConfig);
#else
    alarmStartAt(&otTimer, TMR_ScheduleActivityCallback, &sEventFired, aT0, aDt, true);
#endif
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    sEventFired = false;

#if ALARM_USE_CTIMER
    sMatchConfig.matchValue = 0;
    CTIMER_SetupMatch(CTIMER0, kCTIMER_Match_0, &sMatchConfig);
#else
    TMR_eRemoveActivity(&otTimer);
#endif
}

uint32_t otPlatAlarmMilliGetNow(void)
{
#if ALARM_USE_CTIMER
    return CTIMER0->TC;
#else

    return (uint32_t)(otPlatTimeGet() / US_PER_MS);
#endif
}

#if ALARM_USE_CTIMER
/**
 * Timer interrupt handler function.
 *
 */
void CTIMER0_IRQHandler(void)
{
    uint32_t flags = CTIMER_GetStatusFlags(CTIMER0);
    CTIMER_ClearStatusFlags(CTIMER0, flags);
    sEventFired = true;
    otSysEventSignalPending();
}
#else

static void TMR_ScheduleActivityCallback(void)
{
    ALARM_LOG("");
    sEventFired = true;
    App_NotifyWakeup();
    otSysEventSignalPending();
}
#endif

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
void MicroTimerCallback(void)
{
    ALARM_LOG("");
    sMicroEventFired = true;
    App_NotifyWakeup();
    otSysEventSignalPending();
}

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    alarmStartAt(&otMicroTimer, MicroTimerCallback, &sMicroEventFired, aT0, aDt, false);
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    sMicroEventFired = false;
    TMR_eRemoveActivity(&otMicroTimer);
}

uint32_t otPlatAlarmMicroGetNow(void)
{
    return (uint32_t)otPlatTimeGet();
}
#endif

uint64_t otPlatTimeGet(void)
{
    OSA_InterruptDisable();

    uint64_t us_tstp = 0;

    /* Make the us timestamp wrap around on 64-bit */
    uint32_t tstp = Timestamp_GetCounter32bit();

    if (prev_tstp > tstp)
    {
        /* 32-bit counter */
        tstp_ovf += OVF_32bit_US;
    }
    prev_tstp = tstp;
    us_tstp   = TICKS32kHz_TO_USEC(tstp) + tstp_ovf;

    OSA_InterruptEnable();

    return us_tstp;
}
