/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_dac.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.dac"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static uint32_t DAC_GetInstance(DAC_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static DAC_Type *const s_dacBases[] = DAC_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to DAC clocks for each instance. */
static const clock_ip_name_t s_dacClocks[] = DAC_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*!
 * @brief Get the DAC peripheral instance
 *
 * @param base DAC peripheral base address.
 * @return The instance of input DAC peripheral base address.
 */
static uint32_t DAC_GetInstance(DAC_Type *base)
{
    uint32_t instance;
    uint32_t dacArrayCount = (sizeof(s_dacBases) / sizeof(s_dacBases[0]));

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < dacArrayCount; instance++)
    {
        if (s_dacBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < dacArrayCount);

    return instance;
}
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*!
 * brief Initializes DAC module, including set reference voltage source, set conversion range, and set output voltage
 * range.
 *
 * param base DAC peripheral base address.
 * param config Pointer to the structure which in type of dac_config_t.
 */
void DAC_Init(DAC_Type *base, const dac_config_t *config)
{
    assert(config != NULL);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the DAC clock*/
    (void)CLOCK_EnableClock(s_dacClocks[DAC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    base->CLK |= DAC_CLK_SOFT_CLK_RST_MASK;
    for (uint8_t i = 0U; i < 10U; i++)
    {
        __NOP();
    }
    base->CLK &= ~DAC_CLK_SOFT_CLK_RST_MASK;

    base->CLK = (base->CLK & ~DAC_CLK_CLK_CTRL_MASK) | DAC_CLK_CLK_CTRL(config->conversionRate);

    base->CTRL = (base->CTRL & (~DAC_CTRL_REF_SEL_MASK)) | DAC_CTRL_REF_SEL(config->refSource);

    base->ACTRL = (base->ACTRL & (~DAC_ACTRL_A_RANGE_MASK)) | DAC_ACTRL_A_RANGE(config->rangeSelect);
}

/*!
 * brief Gets the default configurations of DAC module.
 *
 * code
 *     config->conversionRate = kDAC_ConversionRate62P5KHZ;
 *     config->refSource = kDAC_ReferenceInternalVoltageSource;
 *     config->rangeSelect = kDAC_RangeLarge;
 * endcode
 *
 * param config Pointer to the structure which in the type of dac_config_t.
 */
void DAC_GetDefaultConfig(dac_config_t *config)
{
    assert(config != NULL);

    (void)memset(config, 0, sizeof(dac_config_t));

    config->conversionRate = kDAC_ConversionRate62P5KHZ;
    config->refSource      = kDAC_ReferenceInternalVoltageSource;
    config->rangeSelect    = kDAC_RangeLarge;
}

/*!
 * brief De-initializes the DAC module, including reset clock divider, reset each channel, and so on.
 *
 * param base DAC peripheral base address.
 */
void DAC_Deinit(DAC_Type *base)
{
    /* Reset each channel. */
    DAC_ResetChannel(base, kDAC_ChannelA | kDAC_ChannelB);

    /* Reset Clock divider. */
    base->CLK |= DAC_CLK_SOFT_CLK_RST_MASK;
    for (uint8_t i = 0U; i < 10U; i++)
    {
        __NOP();
    }
    base->CLK &= ~DAC_CLK_SOFT_CLK_RST_MASK;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Gate the DAC clock*/
    (void)CLOCK_DisableClock(s_dacClocks[DAC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Configures the DAC channels, including enable channel conversion, set wave type, set timing mode, and so on.
 *
 * param base DAC peripheral base address.
 * param channelMask The mask of channel, can be the OR'ed value of dac_channel_id_t.
 * param channelConfig The pointer of structure which in the type of dac_channel_config_t.
 */
void DAC_SetChannelConfig(DAC_Type *base, uint32_t channelMask, const dac_channel_config_t *channelConfig)
{
    assert(channelConfig != NULL);

    uint32_t tmp32 = 0UL;

    /* Resets DAC channel. */
    DAC_ResetChannel(base, channelMask);

    if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
    {
        assert(IS_DAC_CHANNEL_A_WAVE(channelConfig->waveType));

        tmp32 = base->ACTRL;
        tmp32 &=
            ~(DAC_ACTRL_A_EN_MASK | DAC_ACTRL_A_IO_EN_MASK | DAC_ACTRL_A_TRIG_EN_MASK | DAC_ACTRL_A_TRIG_SEL_MASK |
              DAC_ACTRL_A_TRIG_TYP_MASK | DAC_ACTRL_A_DEN_MASK | DAC_ACTRL_A_TIME_MODE_MASK | DAC_ACTRL_A_WAVE_MASK);

        tmp32 |= DAC_ACTRL_A_EN(channelConfig->enableConversion) | DAC_ACTRL_A_IO_EN(channelConfig->outMode) |
                 DAC_ACTRL_A_TRIG_EN(channelConfig->enableTrigger) |
                 DAC_ACTRL_A_TRIG_SEL(channelConfig->triggerSource) | DAC_ACTRL_A_TRIG_TYP(channelConfig->triggerType) |
                 DAC_ACTRL_A_DEN(channelConfig->enableDMA) | DAC_ACTRL_A_TIME_MODE(channelConfig->timingMode) |
                 DAC_ACTRL_A_WAVE(channelConfig->waveType);

        base->ACTRL = tmp32;
    }

    if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
    {
        assert(IS_DAC_CHANNEL_B_WAVE(channelConfig->waveType));

        tmp32 = base->BCTRL;
        tmp32 &=
            ~(DAC_BCTRL_B_EN_MASK | DAC_BCTRL_B_IO_EN_MASK | DAC_BCTRL_B_TRIG_EN_MASK | DAC_BCTRL_B_TRIG_SEL_MASK |
              DAC_BCTRL_B_TRIG_TYP_MASK | DAC_BCTRL_B_DEN_MASK | DAC_BCTRL_B_TIME_MODE_MASK | DAC_BCTRL_B_WAVE_MASK);
        tmp32 |= DAC_BCTRL_B_EN(channelConfig->enableConversion) | DAC_BCTRL_B_IO_EN(channelConfig->outMode) |
                 DAC_BCTRL_B_TRIG_EN(channelConfig->enableTrigger) |
                 DAC_BCTRL_B_TRIG_SEL(channelConfig->triggerSource) | DAC_BCTRL_B_TRIG_TYP(channelConfig->triggerType) |
                 DAC_BCTRL_B_DEN(channelConfig->enableDMA) | DAC_BCTRL_B_TIME_MODE(channelConfig->timingMode) |
                 DAC_BCTRL_B_WAVE(channelConfig->waveType);

        base->BCTRL = tmp32;
    }
}

/*!
 * brief Configures the options of triangle waveform.
 *
 * note This API should be invoked to set the options of triangle waveform when channel A's output wave type is
 * selected as kDAC_WaveTriangle.
 *
 * param base DAC peripheral base address.
 * param triangleConfig The pointer of structure which in the type of dac_triangle_config_t.
 */
void DAC_SetTriangleConfig(DAC_Type *base, const dac_triangle_config_t *triangleConfig)
{
    assert(triangleConfig != NULL);

    uint32_t tmp32;

    tmp32 = base->ACTRL;
    tmp32 &= ~(DAC_ACTRL_A_TRIA_STEP_SEL_MASK | DAC_ACTRL_A_TRIA_MAMP_SEL_MASK | DAC_ACTRL_A_TRIA_HALF_MASK);
    tmp32 |= DAC_ACTRL_A_TRIA_STEP_SEL(triangleConfig->triangleStepSize) |
             DAC_ACTRL_A_TRIA_MAMP_SEL(triangleConfig->triangleMamp) |
             DAC_ACTRL_A_TRIA_HALF(triangleConfig->triangleWaveform);
    base->ACTRL = tmp32;

    base->ADATA = DAC_ADATA_A_DATA(triangleConfig->triangleBase);
}
