/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_gpt.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.gpt"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Gets the instance from the base address
 *
 * @param base GPT peripheral base address
 *
 * @return The GPT instance
 */
static uint32_t GPT_GetInstance(GPT_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to GPT bases for each instance. */
static GPT_Type *const s_gptBases[] = GPT_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to GPT clocks for each instance. */
static const clock_ip_name_t s_gptClocks[] = GPT_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t GPT_GetInstance(GPT_Type *base)
{
    uint32_t instance;
    uint32_t gptArrayCount = (sizeof(s_gptBases) / sizeof(s_gptBases[0]));

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < gptArrayCount; instance++)
    {
        if (s_gptBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < gptArrayCount);

    return instance;
}

/*!
 * @brief Ungates the GPT clock and configures the peripheral for basic operation.
 *
 * @note This API should be called at the beginning of the application which is using the GPT driver.
 *       If the GPT instance has only TPM features, please use the TPM driver.
 *
 * @param base   GPT peripheral base address
 * @param config Pointer to the user configuration structure.
 *
 * @return kStatus_Success indicates success; Else indicates failure.
 */
status_t GPT_Init(GPT_Type *base, const gpt_config_t *config)
{
    uint32_t i;

    assert(config);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the GPT clock*/
    (void)CLOCK_EnableClock(s_gptClocks[GPT_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Ensure GPT is stopped. */
    GPT_StopTimer(base);

    /* Set all channels to no function mode. */
    for (i = 0U; i < 6U; i++)
    {
        base->CH[i].CH_CNTL_REG = 0U;
    }

    /* Clear all status */
    GPT_ClearStatusFlags(base, (uint32_t)kGPT_AllFlags);

    /* Configure counter clock. */
    base->CLK_CNTL_REG = GPT_CLK_CNTL_REG_CLK_SRC(config->src) | GPT_CLK_CNTL_REG_CLK_DIV(config->div) |
                         GPT_CLK_CNTL_REG_CLK_PRE(config->prescale);
    /* Configure counter update mode and debug behaivor. */
    base->CNT_CNTL_REG =
        (config->stopCntInDebug ? GPT_CNT_CNTL_REG_CNT_DBG_ACT(0U) : GPT_CNT_CNTL_REG_CNT_DBG_ACT(1U)) |
        GPT_CNT_CNTL_REG_CNT_UPDT_MOD(config->cntUpdateMode);
    /* Configure input capture clock and filter. */
    base->IC_CNTL_REG = GPT_IC_CNTL_REG_CHX_IC_DIV(config->icDiv) | GPT_IC_CNTL_REG_CHX_IC_WIDTH(config->icFilter);
    /* Set up counter overflow threshold. */
    GPT_SetTimerPeriod(base, config->cntUppVal);
    /* Reset the counter to make the timer period valid immediately. */
    GPT_ResetTimer(base);

    return kStatus_Success;
}

/*!
 * brief Gates the GPT clock.
 *
 * param base GPT peripheral base address
 */
void GPT_Deinit(GPT_Type *base)
{
    /* Ensure GPT is stopped. */
    GPT_StopTimer(base);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Gate the GPT clock */
    (void)CLOCK_DisableClock(s_gptClocks[GPT_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * @brief  Fills in the GPT configuration structure with the default settings.
 *
 * The default values are:
 * @code
 *   config->src = kGPT_SystemClock;
 *   config->div = kGPT_ClkDiv_Divide_1;
 *   config->prescale = 0U;
 *   config->cntUpdateMode = kGPT_CntUpdateMode_Normal;
 *   config->stopCntInDebug = true;
 *   config->cntUppVal = 0xFFFFFFFFU;
 *   config->icDiv = kGPT_SampClkDiv_Divide_1;
 *   config->icFilter = kGPT_InputCapFilter_None;
 * @endcode
 * @param config Pointer to the user configuration structure.
 */
void GPT_GetDefaultConfig(gpt_config_t *config)
{
    assert(config != NULL);

    /* Initializes the configure structure to zero. */
    (void)memset(config, 0, sizeof(*config));

    /* Set clock source to system clock (not from pad). */
    config->src = kGPT_SystemClock;
    /* Set clock source divider to 1 for counter. */
    config->div = kGPT_ClkDiv_Divide_1;
    /* Set clock prescalar divider to 1 for counter. */
    config->prescale = 0U;
    /* Set counter update mode to normal. */
    config->cntUpdateMode = kGPT_CntUpdateMode_Normal;
    /* Stop counter in debug mode. */
    config->stopCntInDebug = true;
    /* Set counter overflow threshold to maximum. */
    config->cntUppVal = 0xFFFFFFFFU;
    /* Set input capture sample clock divider to 1. */
    config->icDiv = kGPT_SampClkDiv_Divide_1;
    /* Set input capture filter cycles to 1 (no filter). */
    config->icFilter = kGPT_InputCapFilter_None;
}

/*!
 * @brief Enables capturing an input signal on the channel using the function parameters.
 *
 * When the edge specified in the captureMode argument occurs on the channel, the GPT counter is
 * captured into the CMR0 register. The user has to read the CMR0 register separately to get this
 * value.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number
 * @param captureMode Specifies which edge to capture
 */
void GPT_SetupInputCapture(GPT_Type *base, gpt_chnl_t chnlNumber, gpt_input_capture_edge_t captureMode)
{
    base->CH[chnlNumber].CH_CNTL_REG = GPT_CH_CNTL_REG_IC_EDGE(captureMode) | GPT_CH_CNTL_REG_CHX_IO(1U);
}

/*!
 * @brief Trigger input capturing by software. Edge and filter is ignored.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number
 */
void GPT_TriggerInputCapture(GPT_Type *base, gpt_chnl_t chnlNumber)
{
    base->USER_REQ_REG = ((uint32_t)GPT_USER_REQ_REG_CH0_USER_ITRIG_MASK) << ((uint8_t)chnlNumber);
}

/*!
 * @brief Get captured value.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number
 */
uint32_t GPT_GetCapturedValue(GPT_Type *base, gpt_chnl_t chnlNumber)
{
    return base->CH[chnlNumber].CH_CMR0_REG;
}

/*!
 * @brief Configures one-shot pulse on the channel.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number
 * @param pol         Channel output polarity reset value.
 * @param delayTicks  Delay width before triggering the pulse, the uint of this value is timer ticks.
 * @param dutyTicks   Pulse width, the uint of this value is timer ticks.
 */
void GPT_SetupOneshotPulse(
    GPT_Type *base, gpt_chnl_t chnlNumber, gpt_chnl_pol_t pol, uint32_t delayTicks, uint32_t dutyTicks)
{
    /* counter must be started */
    assert((base->CNT_EN_REG & GPT_CNT_EN_REG_CNT_RUN_MASK) != 0U);

    base->CH[chnlNumber].CH_CNTL_REG = GPT_CH_CNTL_REG_POL(pol) | GPT_CH_CNTL_REG_CHX_IO(4U);
    base->CH[chnlNumber].CH_CMR0_REG = dutyTicks;
    base->CH[chnlNumber].CH_CMR1_REG = delayTicks;

    /* Update CMRs */
    base->USER_REQ_REG = GPT_USER_REQ_REG_CH0_CMR_UPDT_MASK << ((uint8_t)chnlNumber);
    /* Shouldn't cause channel error. */
    assert((base->STS_REG & (((uint32_t)GPT_STS_REG_CH0_ERR_STS_MASK) << ((uint8_t)chnlNumber))) == 0U);

    /* Now trigger the channel reset for one shot pulse */
    base->USER_REQ_REG = ((uint32_t)GPT_USER_REQ_REG_CH0_RST_MASK) << ((uint8_t)chnlNumber);
}

/*!
 * @brief Configures one-shot edge on the channel.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number
 * @param pol         Channel output polarity reset value.
 * @param delayTicks  Delay width before changing the edge, the uint of this value is timer ticks.
 */
void GPT_SetupOneshotEdge(GPT_Type *base, gpt_chnl_t chnlNumber, gpt_chnl_pol_t pol, uint32_t delayTicks)
{
    /* counter must be started */
    assert((base->CNT_EN_REG & GPT_CNT_EN_REG_CNT_RUN_MASK) != 0U);

    base->CH[chnlNumber].CH_CNTL_REG = GPT_CH_CNTL_REG_POL(pol) | GPT_CH_CNTL_REG_CHX_IO(5U);
    base->CH[chnlNumber].CH_CMR1_REG = delayTicks;

    /* Update CMRs */
    base->USER_REQ_REG = GPT_USER_REQ_REG_CH0_CMR_UPDT_MASK << ((uint8_t)chnlNumber);
    /* Shouldn't cause channel error. */
    assert((base->STS_REG & (((uint32_t)GPT_STS_REG_CH0_ERR_STS_MASK) << ((uint8_t)chnlNumber))) == 0U);

    /* Now trigger the channel reset for one shot pulse */
    base->USER_REQ_REG = ((uint32_t)GPT_USER_REQ_REG_CH0_RST_MASK) << ((uint8_t)chnlNumber);
}

/*!
 * @brief Configures the PWM signal parameters.
 *
 * Call this function to configure the PWM signal period, mode, duty cycle, and edge. Use this
 * function to configure all GPT channels that are used to output a PWM signal.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number.
 * @param pol         PWM output polarity reset value.
 * @param dutyPercent PWM pulse width, value should be between 0 to 100
 * @param mode        PWM operation mode, options available in enumeration ::gpt_pwm_mode_t
 * @param pwmFreq_Hz  PWM signal frequency in Hz
 * @param srcClock_Hz GPT counter clock in Hz
 *
 * @return kStatus_Success if the PWM setup was successful
 *         kStatus_Fail on failure
 */
status_t GPT_SetupPwm(GPT_Type *base,
                      gpt_chnl_t chnlNumber,
                      gpt_chnl_pol_t pol,
                      uint32_t dutyPercent,
                      gpt_pwm_mode_t mode,
                      uint32_t pwmFreq_Hz,
                      uint32_t srcClock_Hz)
{
    uint32_t reg, periodTicks;
    uint32_t gptClock;

    assert(0U != srcClock_Hz);
    assert(0U != pwmFreq_Hz);
    assert(dutyPercent <= 100U);

    reg      = base->CLK_CNTL_REG;
    gptClock = srcClock_Hz / (1UL << ((reg & GPT_CLK_CNTL_REG_CLK_DIV_MASK) >> GPT_CLK_CNTL_REG_CLK_DIV_SHIFT)) /
               (((reg & GPT_CLK_CNTL_REG_CLK_PRE_MASK) >> GPT_CLK_CNTL_REG_CLK_PRE_SHIFT) + 1U);
    periodTicks = (gptClock + pwmFreq_Hz - 1U) / pwmFreq_Hz;

    return GPT_SetupPwmMode(base, chnlNumber, pol, periodTicks, periodTicks * dutyPercent / 100U, mode);
}

/*!
 * @brief Configures the PWM mode parameters.
 *
 * Call this function to configure the PWM signal mode, duty cycle in ticks, and edge. Use this
 * function to configure all GPT channels that are used to output a PWM signal.
 * Please note that: This API is similar with GPT_SetupPwm() API, but will not set the timer period,
 *                   and this API will set channel match value in timer ticks, not period percent.
 *
 * @param base        GPT peripheral base address
 * @param chnlNumber  The channel number.
 * @param pol         PWM output polarity reset value.
 * @param periodTicks PWM period width, the uint of this value is timer ticks.
 * @param dutyTicks   PWM pulse width, the uint of this value is timer ticks.
 * @param mode        PWM operation mode, options available in enumeration ::gpt_pwm_mode_t
 *
 * @return kStatus_Success if the PWM setup was successful
 *         kStatus_Fail on failure
 */
status_t GPT_SetupPwmMode(GPT_Type *base,
                          gpt_chnl_t chnlNumber,
                          gpt_chnl_pol_t pol,
                          uint32_t periodTicks,
                          uint32_t dutyTicks,
                          gpt_pwm_mode_t mode)
{
    /* counter must be started */
    assert((base->CNT_EN_REG & GPT_CNT_EN_REG_CNT_RUN_MASK) != 0U);
    assert(periodTicks > 0U);
    assert(periodTicks >= dutyTicks);

    if (mode == kGPT_CenterAlignedPwm)
    {
        base->CH[chnlNumber].CH_CNTL_REG = GPT_CH_CNTL_REG_POL(pol) | GPT_CH_CNTL_REG_CHX_IO(7U);
        base->CH[chnlNumber].CH_CMR0_REG = (dutyTicks + 1U) / 2U;
        base->CH[chnlNumber].CH_CMR1_REG = (periodTicks - dutyTicks + 1U) / 2U;
    }
    else /* Edge aligned */
    {
        base->CH[chnlNumber].CH_CNTL_REG = GPT_CH_CNTL_REG_POL(pol) | GPT_CH_CNTL_REG_CHX_IO(6U);
        base->CH[chnlNumber].CH_CMR0_REG = dutyTicks;
        base->CH[chnlNumber].CH_CMR1_REG = periodTicks - dutyTicks;
    }

    /* Update CMRs */
    base->USER_REQ_REG = GPT_USER_REQ_REG_CH0_CMR_UPDT_MASK << ((uint8_t)chnlNumber);
    /* Shouldn't cause channel error. */
    assert((base->STS_REG & (((uint32_t)GPT_STS_REG_CH0_ERR_STS_MASK) << ((uint8_t)chnlNumber))) == 0U);

    /* Now trigger the channel reset for one shot pulse */
    base->USER_REQ_REG = ((uint32_t)GPT_USER_REQ_REG_CH0_RST_MASK) << ((uint8_t)chnlNumber);

    return kStatus_Success;
}

/*!
 * @brief Updates the duty cycle of an active PWM signal.
 *
 * @param base              GPT peripheral base address
 * @param chnlNumber        The channel number.
 * @param dutyPercent       New PWM pulse width; The value should be between 0 to 100
 *                          0=inactive signal(0% duty cycle)...
 *                          100=active signal (100% duty cycle)
 *
 * @return kStatus_Success if the PWM update was successful
 *         kStatus_Fail on failure
 */
status_t GPT_UpdatePwmDutycycle(GPT_Type *base, gpt_chnl_t chnlNumber, uint8_t dutyPercent)
{
    uint32_t periodTicks, dutyTicks;
    status_t status = kStatus_Success;

    /* counter must be started */
    assert((base->CNT_EN_REG & GPT_CNT_EN_REG_CNT_RUN_MASK) != 0U);
    assert(dutyPercent <= 100U);

    periodTicks = base->CH[chnlNumber].CH_CMR0_REG + base->CH[chnlNumber].CH_CMR1_REG;
    dutyTicks   = periodTicks * dutyPercent / 100U;

    base->CH[chnlNumber].CH_CMR0_REG = dutyTicks;
    base->CH[chnlNumber].CH_CMR1_REG = periodTicks - dutyTicks;

    /* Update CMRs */
    base->USER_REQ_REG = GPT_USER_REQ_REG_CH0_CMR_UPDT_MASK << ((uint8_t)chnlNumber);

    if ((base->STS_REG & (((uint32_t)GPT_STS_REG_CH0_ERR_STS_MASK) << ((uint8_t)chnlNumber))) != 0U)
    {
        /* Clear error and return */
        base->STS_REG = (((uint32_t)GPT_STS_REG_CH0_ERR_STS_MASK) << ((uint8_t)chnlNumber));
        status        = kStatus_Fail;
    }

    return status;
}

/*!
 * @brief Pause the PWM signal.
 *
 * @param base              GPT peripheral base address
 * @param chnlNumber        The channel number.
 *
 * @return kStatus_Success if the PWM update was successful
 *         kStatus_Fail on failure
 */
status_t GPT_PausePwm(GPT_Type *base, gpt_chnl_t chnlNumber)
{
    status_t status = kStatus_Success;

    /* counter must be started */
    assert((base->CNT_EN_REG & GPT_CNT_EN_REG_CNT_RUN_MASK) != 0U);

    base->CH[chnlNumber].CH_CMR0_REG = 0U;
    base->CH[chnlNumber].CH_CMR1_REG = 0U;

    /* Update CMRs */
    base->USER_REQ_REG = GPT_USER_REQ_REG_CH0_CMR_UPDT_MASK << ((uint8_t)chnlNumber);

    if ((base->STS_REG & (((uint32_t)GPT_STS_REG_CH0_ERR_STS_MASK) << ((uint8_t)chnlNumber))) != 0U)
    {
        /* Clear error and return */
        base->STS_REG = (((uint32_t)GPT_STS_REG_CH0_ERR_STS_MASK) << ((uint8_t)chnlNumber));
        status        = kStatus_Fail;
    }

    return status;
}

/*!
 * brief Enables the selected GPT interrupts.
 *
 * param base GPT peripheral base address
 * param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::gpt_interrupt_enable_t
 */
void GPT_EnableInterrupts(GPT_Type *base, uint32_t mask)
{
    base->INT_MSK_REG &= ~(mask & (uint32_t)kGPT_AllInterruptsEnable);
}

/*!
 * brief Disables the selected GPT interrupts.
 *
 * param base GPT peripheral base address
 * param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::gpt_interrupt_enable_t
 */
void GPT_DisableInterrupts(GPT_Type *base, uint32_t mask)
{
    base->INT_MSK_REG |= (mask & (uint32_t)kGPT_AllInterruptsEnable);
}

/*!
 * brief Gets the enabled GPT interrupts.
 *
 * param base GPT peripheral base address
 *
 * return The enabled interrupts. This is the logical OR of members of the
 *         enumeration ::gpt_interrupt_enable_t
 */
uint32_t GPT_GetEnabledInterrupts(GPT_Type *base)
{
    return (~base->INT_MSK_REG) & (uint32_t)kGPT_AllInterruptsEnable;
}

/*!
 * brief Gets the GPT status flags.
 *
 * param base GPT peripheral base address
 *
 * return The status flags. This is the logical OR of members of the
 *         enumeration ::gpt_status_flags_t
 */
uint32_t GPT_GetStatusFlags(GPT_Type *base)
{
    return base->STS_REG & (uint32_t)kGPT_AllFlags;
}

/*!
 * brief Clears the GPT status flags.
 *
 * param base GPT peripheral base address
 * param mask The status flags to clear. This is a logical OR of members of the
 *             enumeration ::gpt_status_flags_t
 */
void GPT_ClearStatusFlags(GPT_Type *base, uint32_t mask)
{
    base->STS_REG = mask & (uint32_t)kGPT_AllFlags;
}

/*!
 * @brief Sets the timer period in units of ticks for the GPT instance.
 *
 * Timers counts from 0 until it equals the count value set here. The count value is written to
 * the CNT_UPP_VAL register.
 *
 * @note
 * Call the utility macros provided in the fsl_common.h to convert usec or msec to ticks.
 *
 * @param base GPT peripheral base address
 * @param ticks A timer period in units of ticks, which should be equal or greater than 1.
 */
void GPT_SetTimerPeriod(GPT_Type *base, uint32_t ticks)
{
    assert(ticks > 0U);

    base->CNT_UPP_VAL_REG = ticks;
}

/*!
 * @brief Reads the current timer counting value.
 *
 * This function returns the real-time timer counting value in a range from 0 to a
 * timer period.
 *
 * @note Call the utility macros provided in the fsl_common.h to convert ticks to usec or msec.
 *
 * @param base GPT peripheral base address
 *
 * @return The current counter value in ticks
 */
uint32_t GPT_GetCurrentTimerCount(GPT_Type *base)
{
    /* To get counter, autoUpdateCntVal must be configured to auto-update. */
    assert((base->CNT_CNTL_REG & GPT_CNT_CNTL_REG_CNT_UPDT_MOD_MASK) != GPT_CNT_CNTL_REG_CNT_UPDT_MOD(3U));
    return base->CNT_VAL_REG;
}

/*!
 * @brief Starts the GPT counter.
 *
 * @param base        GPT peripheral base address
 */
void GPT_StartTimer(GPT_Type *base)
{
    if ((base->CNT_EN_REG & GPT_CNT_EN_REG_CNT_RUN_MASK) == 0U)
    {
        base->CNT_EN_REG = GPT_CNT_EN_REG_CNT_START_MASK;
        while ((base->CNT_EN_REG & GPT_CNT_EN_REG_CNT_RUN_MASK) == 0U)
        {
        }
    }
}

/*!
 * @brief Stops the GPT counter.
 *
 * @param base GPT peripheral base address
 */
void GPT_StopTimer(GPT_Type *base)
{
    if ((base->CNT_EN_REG & GPT_CNT_EN_REG_CNT_RUN_MASK) != 0U)
    {
        base->CNT_EN_REG = GPT_CNT_EN_REG_CNT_STOP_MASK;
        while ((base->CNT_EN_REG & GPT_CNT_EN_REG_CNT_RUN_MASK) != 0U)
        {
        }
    }
}

/*!
 * @brief Performs a GPT counter reset.
 *
 * @param base GPT peripheral base address
 */
void GPT_ResetTimer(GPT_Type *base)
{
    /* Reset counter */
    base->CNT_EN_REG = GPT_CNT_EN_REG_CNT_RESET_MASK;
    while ((base->CNT_EN_REG & GPT_CNT_EN_REG_CNT_RST_DONE_MASK) == 0U)
    {
    }
}
