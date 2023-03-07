/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_rtc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.rtc"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Ungates the RTC clock and configures the peripheral for basic operation.
 *
 * This function configures the RTC only if the counter is not running.
 *
 * @note This API should be called at the beginning of the application using the RTC driver.
 *
 * @param base   RTC peripheral base address
 * @param config Pointer to the user's RTC configuration structure.
 */
void RTC_Init(RTC_Type *base, const rtc_config_t *config)
{
    bool running;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_EnableClock(kCLOCK_Rtc);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    running = ((base->CNT_EN_REG & RTC_CNT_EN_REG_CNT_RUN_MASK) != 0U);
    if (running && (!config->ignoreInRunning))
    {
        RTC_StopTimer(base);
    }

    /* Reconfigure the RTC if it's not running. */
    if ((base->CNT_EN_REG & RTC_CNT_EN_REG_CNT_RUN_MASK) == 0U)
    {
        /* Ensure RTC reset is deasserted */
        while ((base->CNT_EN_REG & RTC_CNT_EN_REG_STS_RESETN_MASK) == 0U)
        {
        }
        base->CNT_CNTL_REG =
            (config->stopCntInDebug ? RTC_CNT_CNTL_REG_CNT_DBG_ACT(0U) : RTC_CNT_CNTL_REG_CNT_DBG_ACT(1U)) |
            (config->autoUpdateCntVal ? RTC_CNT_CNTL_REG_CNT_UPDT_MOD(2U) : RTC_CNT_CNTL_REG_CNT_UPDT_MOD(0U));
        base->CLK_CNTL_REG    = RTC_CLK_CNTL_REG_CLK_DIV(config->clkDiv);
        base->CNT_UPP_VAL_REG = config->cntUppVal;
        /* Reset counter */
        RTC_ResetTimer(base);
    }
}

/*!
 * @brief Stops the timer and gate the RTC clock.
 *
 * @param base RTC peripheral base address
 */
void RTC_Deinit(RTC_Type *base)
{
    RTC_StopTimer(base);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_DisableClock(kCLOCK_Rtc);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * @brief Fills in the RTC config struct with the default settings.
 *
 * The default values are as follows.
 * @code
 *    config->ignoreInRunning = false;
 *    config->autoUpdateCntVal = true;
 *    config->stopCntInDebug = true;
 *    config->clkDiv = kRTC_ClockDiv32;
 *    config->cntUppVal = 0xFFFFFFFFU;
 * @endcode
 * @param config Pointer to the user's RTC configuration structure.
 */
void RTC_GetDefaultConfig(rtc_config_t *config)
{
    assert(NULL != config);

    /* Initializes the configure structure to zero. */
    (void)memset(config, 0, sizeof(*config));

    /* Force update of the configuration even if the counter is running. */
    config->ignoreInRunning = false;
    /* Enable CNT_VAL_REG update for counter read. */
    config->autoUpdateCntVal = true;
    /* Stop the counter in debug mode. */
    config->stopCntInDebug = true;
    /* Use 1.024KHz clock for counter. */
    config->clkDiv = kRTC_ClockDiv32;
    /* Use 32bit full range for counter. */
    config->cntUppVal = 0xFFFFFFFFU;
}

/*!
 * @brief Gets current RTC counter value.
 *
 * @param base     RTC peripheral base address
 *
 * @return counter value in RTC.
 */
uint32_t RTC_GetCounter(RTC_Type *base)
{
    /* To get counter, autoUpdateCntVal must be configured to true. */
    assert((base->CNT_CNTL_REG & RTC_CNT_CNTL_REG_CNT_UPDT_MOD_MASK) == RTC_CNT_CNTL_REG_CNT_UPDT_MOD(2U));
    return base->CNT_VAL_REG;
}

/*!
 * @brief Sets the RTC alarm time.
 *
 * The function checks whether the specified alarm time is greater than the present
 * time. If not, the function does not set the alarm and returns an error.
 *
 * @param base      RTC peripheral base address
 * @param alarmCnt  Counter value to trigger the alarm.
 *
 * @return kStatus_Success: success in setting the RTC alarm
 *         kStatus_Fail: Error because the alarm time has already passed
 */
status_t RTC_SetAlarm(RTC_Type *base, uint32_t alarmCnt)
{
    status_t status;

    /* If we can get the counter value, validate the alarm value. */
    if (((base->CNT_CNTL_REG & RTC_CNT_CNTL_REG_CNT_UPDT_MOD_MASK) == RTC_CNT_CNTL_REG_CNT_UPDT_MOD(2U)) &&
        alarmCnt <= base->CNT_VAL_REG)
    {
        status = kStatus_Fail;
    }
    else
    {
        base->CNT_ALARM_VAL_REG = alarmCnt;
        status                  = kStatus_Success;
    }

    return status;
}

/*!
 * @brief Returns the RTC alarm counter value.
 *
 * @param base     RTC peripheral base address
 *
 * @return Alarm counter value in RTC.
 */
uint32_t RTC_GetAlarm(RTC_Type *base)
{
    return base->CNT_ALARM_VAL_REG;
}

/*!
 * brief Enables the selected RTC interrupts.
 *
 * param base RTC peripheral base address
 * param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::rtc_interrupt_enable_t
 */
void RTC_EnableInterrupts(RTC_Type *base, uint32_t mask)
{
    base->INT_MSK_REG &= ~(mask & ((uint32_t)kRTC_AllInterruptsEnable));
}

/*!
 * brief Disables the selected RTC interrupts.
 *
 * param base RTC peripheral base address
 * param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::rtc_interrupt_enable_t
 */
void RTC_DisableInterrupts(RTC_Type *base, uint32_t mask)
{
    base->INT_MSK_REG |= (mask & ((uint32_t)kRTC_AllInterruptsEnable));
}

/*!
 * brief Gets the enabled RTC interrupts.
 *
 * param base RTC peripheral base address
 *
 * return The enabled interrupts. This is the logical OR of members of the
 *         enumeration ::rtc_interrupt_enable_t
 */
uint32_t RTC_GetEnabledInterrupts(RTC_Type *base)
{
    return (~base->INT_MSK_REG) & ((uint32_t)kRTC_AllInterruptsEnable);
}

/*!
 * brief Gets the RTC status flags.
 *
 * param base RTC peripheral base address
 *
 * return The status flags. This is the logical OR of members of the
 *         enumeration ::rtc_status_flags_t
 */
uint32_t RTC_GetStatusFlags(RTC_Type *base)
{
    uint32_t tmp32 = (base->CNT_EN_REG >> RTC_CNT_EN_REG_CNT_RUN_SHIFT) & ((uint32_t)kRTC_AllNonClearableFlags);

    tmp32 |= (base->INT_RAW_REG & ((uint32_t)kRTC_AllClearableFlags));

    return tmp32;
}

/*!
 * brief  Clears the RTC status flags.
 *
 * param base RTC peripheral base address
 * param mask The status flags to clear. This is a logical OR of members of the
 *             enumeration ::rtc_status_flags_t
 */
void RTC_ClearStatusFlags(RTC_Type *base, uint32_t mask)
{
    base->INT_RAW_REG = mask & ((uint32_t)kRTC_AllClearableFlags);
}

/*!
 * @brief Starts the RTC time counter.
 *
 * @param base RTC peripheral base address
 */
void RTC_StartTimer(RTC_Type *base)
{
    if ((base->CNT_EN_REG & RTC_CNT_EN_REG_CNT_RUN_MASK) == 0U)
    {
        base->CNT_EN_REG = RTC_CNT_EN_REG_CNT_START_MASK;
        while ((base->CNT_EN_REG & RTC_CNT_EN_REG_CNT_RUN_MASK) == 0U)
        {
        }
    }
}

/*!
 * @brief Stops the RTC time counter.
 *
 * @param base RTC peripheral base address
 */
void RTC_StopTimer(RTC_Type *base)
{
    if ((base->CNT_EN_REG & RTC_CNT_EN_REG_CNT_RUN_MASK) != 0U)
    {
        base->CNT_EN_REG = RTC_CNT_EN_REG_CNT_STOP_MASK;
        while ((base->CNT_EN_REG & RTC_CNT_EN_REG_CNT_RUN_MASK) != 0U)
        {
        }
    }
}

/*!
 * @brief Performs a counter reset on the RTC module.
 *
 * @param base RTC peripheral base address
 */
void RTC_ResetTimer(RTC_Type *base)
{
    uint32_t i;

    /* If user calls RTC_ResetTimer() continously in short time, write reset bit cannot work well.
     * Need a loop to redo the reset operation until it takes effect. */
    do
    {
        /* Reset counter */
        base->CNT_EN_REG = RTC_CNT_EN_REG_CNT_RESET_MASK;

        for (i = 0U; (i < 10000U) && ((base->CNT_EN_REG & RTC_CNT_EN_REG_CNT_RST_DONE_MASK) == 0U); i++)
        {
        }
    } while ((base->CNT_EN_REG & RTC_CNT_EN_REG_CNT_RST_DONE_MASK) == 0U);
}
