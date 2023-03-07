/*
 * Copyright 2020, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef FSL_RTOS_FREE_RTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

#include "fsl_rtc.h"
#include "fsl_power.h"

#include "lpm.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
static uint32_t s_curMode;
static uint32_t s_excludeIo;
static uint32_t s_pmThreshold;
static uint32_t s_pmLatency;
static wakeup_handler s_pmWakeupHandler;

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t LPM_EnterTicklessIdle(uint32_t timeoutMilliSec)
{
    uint32_t counter;
    uint32_t alarm;
    uint64_t wait = (uint64_t)timeoutMilliSec * (CLOCK_GetRtcClkFreq() / 32U) / 1000U;

    /* Get current RTC counter. */
    counter = RTC_GetCounter(RTC);

    /* Add wait count to current counter */
    wait += counter;
    if (wait >= 0xFFFFFFFFULL)
    {
        alarm = 0xFFFFFFFFUL;
    }
    else
    {
        alarm = (uint32_t)wait;
    }

    /* Set alarm counter */
    RTC_SetAlarm(RTC, alarm);
    /* Clear alarm flag if it's set. */
    RTC_ClearStatusFlags(RTC, kRTC_AllClearableFlags);
    /* Enable alarm interrupt */
    RTC_EnableInterrupts(RTC, (uint32_t)kRTC_AlarmInterruptEnable);

    return counter;
}

static void LPM_ExitTicklessIdle(uint32_t base)
{
#ifdef FSL_RTOS_FREE_RTOS
    uint32_t counter;
    uint32_t ticks;
    uint32_t expireMs;
    uint32_t freq;
#endif

    if ((RTC_GetStatusFlags(RTC) & (uint32_t)kRTC_AlarmFlag) == 0U)
    {
        /* Not woken up from RTC, further alarm is not needed. */
        RTC_DisableInterrupts(RTC, (uint32_t)kRTC_AlarmInterruptEnable);
    }

#ifdef FSL_RTOS_FREE_RTOS
    /* Get current RTC counter. */
    counter  = RTC_GetCounter(RTC);
    freq     = CLOCK_GetRtcClkFreq() / 32U;
    assert(freq != 0U);
    expireMs = (uint64_t)(counter - base) * 1000U / freq;
    ticks    = (uint64_t)expireMs * configTICK_RATE_HZ / 1000U;

    vTaskStepTick(ticks);
#endif
}

void EXTPIN0_IRQHandler()
{
    POWER_ClearWakeupSource(kPOWER_WakeupSrcPin0);
    NVIC_ClearPendingIRQ(EXTPIN0_IRQn);

    if (s_pmWakeupHandler)
    {
        s_pmWakeupHandler(EXTPIN0_IRQn);
    }
}

void EXTPIN1_IRQHandler()
{
    POWER_ClearWakeupSource(kPOWER_WakeupSrcPin1);
    NVIC_ClearPendingIRQ(EXTPIN1_IRQn);

    if (s_pmWakeupHandler)
    {
        s_pmWakeupHandler(EXTPIN1_IRQn);
    }
}

void RTC_IRQHandler()
{
    POWER_ClearWakeupSource(kPOWER_WakeupSrcRtc);
    RTC_ClearStatusFlags(RTC, kRTC_AllClearableFlags);

    /* Disable alarm interrupt */
    RTC_DisableInterrupts(RTC, (uint32_t)kRTC_AlarmInterruptEnable);
    if (s_pmWakeupHandler)
    {
        s_pmWakeupHandler(RTC_IRQn);
    }
}

void LPM_Init(const lpm_config_t *config)
{
    rtc_config_t rtcConfig;

    assert(config);

    s_curMode         = 0U;
    s_excludeIo       = 0U;
    s_pmThreshold     = config->threshold;
    s_pmLatency       = config->latency;
    s_pmWakeupHandler = config->handler;

    /* Init RTC */
    /*
     *    config->ignoreInRunning = false;
     *    config->autoUpdateCntVal = true;
     *    config->stopCntInDebug = true;
     *    config->clkDiv = kRTC_ClockDiv32;
     *    config->cntUppVal = 0xFFFFFFFFU;
     */
    RTC_GetDefaultConfig(&rtcConfig);
    RTC_Init(RTC, &rtcConfig);

    EnableIRQ(RTC_IRQn);
    /* Start RTC with 1024Hz */
    RTC_StartTimer(RTC);

    POWER_ClearWakeupSource(kPOWER_WakeupSrcRtc);
    POWER_EnableWakeup(kPOWER_WakeupMaskRtc);

    if (config->enableWakeupPin0)
    {
        POWER_ConfigWakeupPin(kPOWER_WakeupPin0, kPOWER_WakeupEdgeLow);
        POWER_ClearWakeupSource(kPOWER_WakeupSrcPin0);
        NVIC_ClearPendingIRQ(EXTPIN0_IRQn);
        EnableIRQ(EXTPIN0_IRQn);
        POWER_EnableWakeup(kPOWER_WakeupMaskPin0);
    }

    if (config->enableWakeupPin1)
    {
        POWER_ConfigWakeupPin(kPOWER_WakeupPin1, kPOWER_WakeupEdgeLow);
        POWER_ClearWakeupSource(kPOWER_WakeupSrcPin1);
        NVIC_ClearPendingIRQ(EXTPIN1_IRQn);
        EnableIRQ(EXTPIN1_IRQn);
        POWER_EnableWakeup(kPOWER_WakeupMaskPin1);
    }
}

void LPM_Deinit(void)
{
    RTC_StopTimer(RTC);
    RTC_Deinit(RTC);
    DisableIRQ(RTC_IRQn);
    DisableIRQ(EXTPIN0_IRQn);
    DisableIRQ(EXTPIN1_IRQn);
    POWER_DisableWakeup(kPOWER_WakeupMaskPin1 | kPOWER_WakeupMaskPin1 | kPOWER_WakeupMaskRtc);
}

void LPM_SetPowerMode(uint32_t mode, uint32_t excludeIo)
{
    s_curMode   = mode;
    s_excludeIo = excludeIo;
}

uint32_t LPM_GetPowerMode(void)
{
    return s_curMode;
}

uint32_t LPM_WaitForInterrupt(uint32_t timeoutMilliSec, bool allowDeepSleep)
{
    uint32_t irqMask;
    uint32_t count;
    uint32_t mode = s_curMode;
    /* tickless only when timeout longer than 2 ticks and preset threshold */
#ifdef FSL_RTOS_FREE_RTOS
    uint32_t threshold = MAX(2000U / configTICK_RATE_HZ, s_pmThreshold);
#else
    uint32_t threshold = s_pmThreshold;
#endif

    irqMask = DisableGlobalIRQ();

    if ((mode == 2U) || (mode == 3U))
    {
        if (!allowDeepSleep || (timeoutMilliSec <= threshold + s_pmLatency))
        {
            mode = 1U;
        }
    }

    switch (mode)
    {
        case 1U:
            if (timeoutMilliSec <= threshold)
            {
                /* Wait short time, no tickless idle needed. */
                POWER_SetSleepMode(1U);
                __WFI();
                break;
            }
            /* For PM1, we should wait exact timeout ms. */
            timeoutMilliSec += s_pmLatency;
            /* Fall through */
        case 2U:
        case 3U:
            count = LPM_EnterTicklessIdle(timeoutMilliSec - s_pmLatency);
            POWER_EnterPowerMode(mode, s_excludeIo);
            LPM_ExitTicklessIdle(count);
            break;
        default:
            POWER_EnterPowerMode(mode, s_excludeIo);
            break;
    }

    EnableGlobalIRQ(irqMask);

    return mode;
}
