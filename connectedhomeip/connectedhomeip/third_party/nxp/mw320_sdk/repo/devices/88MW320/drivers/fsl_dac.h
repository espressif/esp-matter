/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_DAC_H_
#define _FSL_DAC_H_

#include "fsl_common.h"

/*!
 * @addtogroup dac
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief DAC driver version */
#define FSL_DAC_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0. */

/*!
 *  @brief DAC channel A wave mode check
 */
#define IS_DAC_CHANNEL_A_WAVE(CH_WAVE) ((CH_WAVE) <= kDAC_WaveNoiseDifferential)

/*!
 *  @brief DAC channel B wave mode check
 */
#define IS_DAC_CHANNEL_B_WAVE(CH_WAVE) (((CH_WAVE) == kDAC_WaveNormal) || ((CH_WAVE) == kDAC_WaveNoiseDifferential))

/*!
 * @brief The enumeration of interrupts that DAC support.
 */
enum _dac_interrupt_enable
{
    kDAC_ChannelAReadyInterruptEnable    = DAC_IMR_A_RDY_INT_MSK_MASK, /*!< Enable channel A data ready interrupt. */
    kDAC_ChannelBReadyInterruptEnable    = DAC_IMR_B_RDY_INT_MSK_MASK, /*!< Enable channel B data ready interrupt. */
    kDAC_ChannelATimeoutInterruptEnable  = DAC_IMR_A_TO_INT_MSK_MASK,  /*!< Enable channel A time out interrupt. */
    kDAC_ChannelBTimeoutInterruptEnable  = DAC_IMR_B_TO_INT_MSK_MASK,  /*!< Enable channel B time out interrupt. */
    kDAC_TriangleOverflowInterruptEnable = DAC_IMR_TRIA_OVFL_INT_MSK_MASK, /*!< Enable triangle overflow interrupt. */
};

/*!
 * @brief The enumeration of DAC status flags, including interrupt status flags, raw status flags, and conversion status
 * flags.
 *
 * @note The interrupt status flags can only be asserted upon both enabling and happening of related interrupts.
 * Comparatively, the raw status flags will be asserted as long as related events happen regardless of whether
 * related interrupts are enabled or not.
 *
 * @note Only interrupt status flags can be cleared mannually.
 */
enum _dac_status_flags
{
    kDAC_ChannelADataReadyInterruptFlag = DAC_ISR_A_RDY_INT_MASK,                 /*!< Channel A data ready. */
    kDAC_ChannelBDataReadyInterruptFlag = DAC_ISR_B_RDY_INT_MASK,                 /*!< Channel B data ready. */
    kDAC_ChannelATimeoutInterruptFlag   = DAC_ISR_A_TO_INT_MASK,                  /*!< Channel A time out. */
    kDAC_ChannelBTimeoutInterruptFlag   = DAC_ISR_B_TO_INT_MASK,                  /*!< Channel B time out. */
    kDAC_TriangleOverflowInterruptFlag  = DAC_ISR_TRIA_OVFL_INT_MASK,             /*!< Triangle overflow. */
    kDAC_RawChannelADataReadyFlag       = DAC_IRSR_A_RDY_INT_RAW_MASK << 5UL,     /*!< Channel A data ready raw. */
    kDAC_RawChannelBDataReadyFlag       = DAC_IRSR_B_RDY_INT_RAW_MASK << 5UL,     /*!< Channel B data ready raw. */
    kDAC_RawChannelATimeoutFlag         = DAC_IRSR_A_TO_INT_RAW_MASK << 5UL,      /*!< Channel A timeout raw. */
    kDAC_RawChannelBTimeoutFlag         = DAC_IRSR_B_TO_INT_RAW_MASK << 5UL,      /*!< Channel B timeout raw. */
    kDAC_RawTriangleOverflowFlag        = DAC_IRSR_TRIA_OVFL_INT_RAW_MASK << 5UL, /*!< Triangle overflow raw. */
    kDAC_ChannelAConversionCompleteFlag = DAC_STATUS_A_DV_MASK << 10UL,           /*!< Channel A conversion complete. */
    kDAC_ChannelBConversionCompleteFlag = DAC_STATUS_B_DV_MASK << 10UL,           /*!< Channel B conversion complete. */
};

/*!
 * @brief The enumeration of dac channels, including channel A and channel B.
 */
typedef enum _dac_channel_id
{
    kDAC_ChannelA = 1U << 0U,
    kDAC_ChannelB = 1U << 1U,
} dac_channel_id_t;

/*!
 * @brief The enumeration of dac converion rate, including 62.5 KHz, 125 KHz, 250 KHz, and 500 KHz.
 */
typedef enum _dac_conversion_rate
{
    kDAC_ConversionRate62P5KHZ = 0U, /*!< DAC Conversion Rate selects as 62.5 KHz. */
    kDAC_ConversionRate125KHZ,       /*!< DAC Conversion Rate selects as 125 KHz. */
    kDAC_ConversionRate250KHZ,       /*!< DAC Conversion Rate selects as 250 KHz. */
    kDAC_ConversionRate500KHZ,       /*!< DAC Conversion Rate selects as 500 KHz. */
} dac_conversion_rate_t;

/*!
 * @brief The enumeration of dac reference voltage source.
 */
typedef enum _dac_reference_voltage_source
{
    kDAC_ReferenceInternalVoltageSource = 0U, /*!< Select internal voltage reference. */
    kDAC_ReferenceExternalVoltageSource,      /*!< Select external voltage reference. */
} dac_reference_voltage_source_t;

/*!
 * @brief The enumeration of dac output voltage range.
 */
typedef enum _dac_output_voltage_range
{
    kDAC_RangeSmall  = 0U, /*!< DAC output small range. */
    kDAC_RangeMiddle = 1U, /*!< DAC output middle range. */
    kDAC_RangeLarge  = 3U, /*!< DAC output large range. */
} dac_output_voltage_range_t;

/*!
 * @brief The enumeration of dac channel's output mode.
 */
typedef enum _dac_channel_output
{
    kDAC_ChannelOutputInternal = 0U, /*!< Enable internal output but disable output to pad */
    kDAC_ChannelOutputPAD,           /*!< Enable output to pad but disable internal output */
} dac_channel_output_t;

/*!
 * @brief The enumeration of dac channel's trigger type, including rising edge trigger, falling edge trigger, and both
 * edge triggers.
 */
typedef enum _dac_channel_trigger_type
{
    kDAC_RisingEdgeTrigger  = 1U, /*!< Rising edge trigger. */
    kDAC_FallingEdgeTrigger = 2U, /*!< Failing edge trigger. */
    kDAC_BothEdgeTrigger    = 3U  /*!< Rising and Failing edge trigger. */
} dac_channel_trigger_type_t;

/*!
 * @brief The enumeration of dac channel trigger source.
 */
typedef enum _dac_channel_trigger_source
{
    kDAC_GPT2Trigger   = 0U, /*!< GPT2 trigger source */
    kDAC_GPT3Trigger   = 1U, /*!< GPT3 trigger source */
    kDAC_GPIO45Trigger = 2U, /*!< GPIO45 trigger source */
    kDAC_GPIO44Trigger = 3U  /*!< GPIO44 trigger source */
} dac_channel_trigger_source_t;

/*!
 * @brief The enumeration of dac channel timing mode.
 */
typedef enum _dac_channel_timing_mode
{
    kDAC_NonTimingCorrelated = 0U, /*!< DAC non-timing-correlated mode. */
    kDAC_TimingCorrelated,         /*!< DAC timing-correlated mode. */
} dac_channel_timing_mode_t;

/*!
 * @brief The enumerator of channel output wave type, please note that not all wave types are effective to
 * A and B channel.
 */
typedef enum _dac_channel_wave_type
{
    kDAC_WaveNormal            = 0U, /*!< No predefined waveform, effective to A or B channel */
    kDAC_WaveTriangle          = 1U, /*!< Triangle wave, effective only to A channel */
    kDAC_WaveSine              = 2U, /*!< Sine wave, effective only to A channel */
    kDAC_WaveNoiseDifferential = 3U, /*!< Noise wave,  effective only to A channel;
                                        Differential mode, one's complemental code from A data,
                                        effective only to B channel */
} dac_channel_wave_type_t;

/*!
 *  @brief DAC triangle maximum amplitude type
 */
typedef enum _dac_triangle_mamp
{
    kDAC_TriangleAmplitude63 = 0U, /*!< DAC triangle amplitude 63 lsb */
    kDAC_TriangleAmplitude127,     /*!< DAC triangle amplitude 127 lsb */
    kDAC_TriangleAmplitude191,     /*!< DAC triangle amplitude 191 lsb */
    kDAC_TriangleAmplitude255,     /*!< DAC triangle amplitude 255 lsb */
    kDAC_TriangleAmplitude319,     /*!< DAC triangle amplitude 319 lsb */
    kDAC_TriangleAmplitude383,     /*!< DAC triangle amplitude 383 lsb */
    kDAC_TriangleAmplitude447,     /*!< DAC triangle amplitude 447 lsb */
    kDAC_TriangleAmplitude511,     /*!< DAC triangle amplitude 511 lsb */
    kDAC_TriangleAmplitude575,     /*!< DAC triangle amplitude 575 lsb */
    kDAC_TriangleAmplitude639,     /*!< DAC triangle amplitude 639 lsb */
    kDAC_TriangleAmplitude703,     /*!< DAC triangle amplitude 703 lsb */
    kDAC_TriangleAmplitude767,     /*!< DAC triangle amplitude 767 lsb */
    kDAC_TriangleAmplitude831,     /*!< DAC triangle amplitude 831 lsb */
    kDAC_TriangleAmplitude895,     /*!< DAC triangle amplitude 895 lsb */
    kDAC_TriangleAmplitude959,     /*!< DAC triangle amplitude 959 lsb */
    kDAC_TriangleAmplitude1023     /*!< DAC triangle amplitude 1023 lsb */
} dac_triangle_mamp_t;

/*!
 *  @brief DAC triangle step size type
 */
typedef enum _dac_triangle_step_size
{
    kDAC_TriangleStepSize1 = 0U, /*!< DAC triangle step size 1 lsb */
    kDAC_TriangleStepSize3,      /*!< DAC triangle step size 3 lsb */
    kDAC_TriangleStepSize15,     /*!< DAC triangle step size 15 lsb */
    kDAC_TriangleStepSize511     /*!< DAC triangle step size 511 lsb */
} dac_triangle_step_size_t;

/*!
 *  @brief DAC triangle waveform type
 */
typedef enum _dac_triangle_waveform_type
{
    kDAC_TriangleFull = 0U, /*!< DAC full triangle waveform */
    kDAC_TriangleHalf,      /*!< DAC half triangle waveform */
} dac_triangle_waveform_type_t;

/*!
 * @brief The structure of dac module basic configuration, including conversion rate, output range, and reference
 * voltage source.
 */
typedef struct _dac_config
{
    dac_conversion_rate_t conversionRate;     /*!< Configure DAC conversion rate,
                                                  please refer to @ref dac_conversion_rate_t. */
    dac_reference_voltage_source_t refSource; /*!< Configure DAC vref source,
                                                  please refer to @ref dac_reference_voltage_source_t. */
    dac_output_voltage_range_t rangeSelect;   /*!< Configure DAC channel output range,
                                                  please refer to @ref dac_output_voltage_range_t. */
} dac_config_t;

/*!
 * @brief The structure of dac channel configuration, such as trigger type, wave type, timing mode, and so on.
 *
 */
typedef struct _dac_channel_config
{
    bool enableConversion;        /*!< Enable/Disable selected channel's conversion.
                                      - \b true Enable selected channel's conversion.
                                      - \b false Disable selected channel's conversion. */
    dac_channel_output_t outMode; /*!< Configure channel output mode, please refer to @ref dac_channel_output_t */

    bool enableDMA;                             /*!< Enable/Disable channel DAM data transfer.
                                                    - \b true DMA data transfer enabled.
                                                    - \b false DMA data transfer disabled. */
    bool enableTrigger;                         /*!< Enable/Disable external event trigger. */
    dac_channel_trigger_type_t triggerType;     /*!< Configure the channel trigger type,
                                                        please refer to @ref dac_channel_trigger_type_t. */
    dac_channel_trigger_source_t triggerSource; /*!< Configure DAC channel trigger source,
                                                    please refer to @ref dac_channel_trigger_source_t. */

    dac_channel_timing_mode_t timingMode; /*!< Configure channel timing mode,
                                              please refer to @ref dac_channel_timing_mode_t. */

    dac_channel_wave_type_t waveType; /*!< Configure wave type for the selected channel,
                                          please refer to @ref dac_channel_wave_type_t. */
} dac_channel_config_t;

/*!
 * @brief The structure of triangle waveform, including maximum value, minimum value, step size, and so on.
 */
typedef struct _dac_triangle_config
{
    dac_triangle_mamp_t triangleMamp;              /*!< Configure triangle maximum value. */
    dac_triangle_step_size_t triangleStepSize;     /*!< Configure triangle step size. */
    dac_triangle_waveform_type_t triangleWaveform; /*!< Configure triangle waveform type. */
    uint32_t triangleBase;                         /*!< Configure triangle minimum value. */
} dac_triangle_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Module Initialization Interfaces
 * @{
 */

/*!
 * @brief Initializes DAC module, including set reference voltage source, set conversion range, and set output voltage
 * range.
 *
 * @param base DAC peripheral base address.
 * @param config Pointer to the structure which in type of @ref dac_config_t.
 */
void DAC_Init(DAC_Type *base, const dac_config_t *config);

/*!
 * @brief Gets the default configurations of DAC module.
 *
 * @code
 *     config->conversionRate = kDAC_ConversionRate62P5KHZ;
 *     config->refSource = kDAC_ReferenceInternalVoltageSource;
 *     config->rangeSelect = kDAC_RangeLarge;
 * @endcode
 *
 * @param config Pointer to the structure which in the type of @ref dac_config_t.
 */
void DAC_GetDefaultConfig(dac_config_t *config);

/*!
 * @brief De-initializes the DAC module, including reset clock divider, reset each channel, and so on.
 *
 * @param base DAC peripheral base address.
 */
void DAC_Deinit(DAC_Type *base);

/* @} */

/*!
 * @name Channels Control Interfaces
 * @{
 */

/*!
 * @brief Configures the DAC channels, including enable channel conversion, set wave type, set timing mode, and so on.
 *
 * @param base DAC peripheral base address.
 * @param channelMask The mask of channel, can be the OR'ed value of @ref dac_channel_id_t.
 * @param channelConfig The pointer of structure which in the type of @ref dac_channel_config_t.
 */
void DAC_SetChannelConfig(DAC_Type *base, uint32_t channelMask, const dac_channel_config_t *channelConfig);

/*!
 * @brief Does software reset for the selected DAC channels.
 *
 * @param base DAC peripheral base address.
 * @param channelMask The mask of channel to be reset, should be the OR'ed value of @ref dac_channel_id_t.
 */
static inline void DAC_ResetChannel(DAC_Type *base, uint32_t channelMask)
{
    base->RST = channelMask;
    for (uint8_t i = 0U; i < 10U; i++)
    {
        __NOP();
    }
    base->RST = 0UL;
}

/*!
 * @brief Enables/Disables selected channel conversion.
 *
 * @note To enable/disable the conversions of both channels, invoking this API with the parameter \b channelMask
 * set as \b kDAC_ChannelA|kDAC_ChannelB .
 *
 * @param base DAC peripheral base address.
 * @param channelMask The mask of channel to be reset, can be the OR'ed value of @ref dac_channel_id_t.
 * @param enable Enable/Disable channel conversion.
 *              - \b true Enable selected channels' conversion.
 *              - \b false Disable selected channels' conversion.
 */
static inline void DAC_EnableChannelConversion(DAC_Type *base, uint32_t channelMask, bool enable)
{
    if (enable)
    {
        if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
        {
            base->ACTRL |= DAC_ACTRL_A_EN_MASK;
        }

        if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
        {
            base->BCTRL |= DAC_BCTRL_B_EN_MASK;
        }
    }
    else
    {
        if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
        {
            base->ACTRL &= ~DAC_ACTRL_A_EN_MASK;
        }

        if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
        {
            base->BCTRL &= ~DAC_BCTRL_B_EN_MASK;
        }
    }
}

/*!
 * @brief Sets channels out mode, including kDAC_ChannelOutputInternal and kDAC_ChannelOutputPad.
 *
 * @param base DAC peripheral base address.
 * @param channelMask The mask of channel, can be the OR'ed value of @ref dac_channel_id_t.
 * @param outMode The out mode of selected channels, please refer to @ref dac_channel_output_t for details.
 */
static inline void DAC_SetChannelOutMode(DAC_Type *base, uint32_t channelMask, dac_channel_output_t outMode)
{
    if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
    {
        base->ACTRL = (base->ACTRL & ~(DAC_ACTRL_A_IO_EN_MASK)) | DAC_ACTRL_A_IO_EN(outMode);
    }

    if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
    {
        base->BCTRL = (base->BCTRL & ~(DAC_BCTRL_B_IO_EN_MASK)) | DAC_BCTRL_B_IO_EN(outMode);
    }
}

/*!
 * @brief Enables/Disables channels trigger mode.
 *
 * @note To enable/disable the trigger mode of both two channels, invoking this API with the parameter \b channelMask
 * set as \b kDAC_ChannelA|kDAC_ChannelB .
 *
 * @param base DAC peripheral base address.
 * @param channelMask The mask of channel, can be the OR'ed value of @ref dac_channel_id_t.
 * @param enable Enable/Disable channel trigger mode.
 *              - \b true Channels' conversion triggered by external event enabled.
 *              - \b false Channels' conversion trigged by external event disabled.
 */
static inline void DAC_EnableChannelTriggerMode(DAC_Type *base, uint32_t channelMask, bool enable)
{
    if (enable)
    {
        if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
        {
            base->ACTRL |= DAC_ACTRL_A_TRIG_EN_MASK;
        }

        if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
        {
            base->BCTRL |= DAC_BCTRL_B_TRIG_EN_MASK;
        }
    }
    else
    {
        if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
        {
            base->ACTRL &= ~DAC_ACTRL_A_TRIG_EN_MASK;
        }

        if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
        {
            base->BCTRL &= ~DAC_BCTRL_B_TRIG_EN_MASK;
        }
    }
}

/*!
 * @brief Sets channels trigger source.
 *
 * @note To set the same trigger source to both two channels, invoking this API with the parameter \b channelMask
 * set as \b kDAC_ChannelA|kDAC_ChannelB .
 *
 * @param base DAC peripheral base address.
 * @param channelMask The mask of channel, can be the OR'ed value of @ref dac_channel_id_t.
 * @param trigSource The selected trigger source, please refer to @ref dac_channel_trigger_source_t for details.
 */
static inline void DAC_SetChannelTrigSource(DAC_Type *base,
                                            uint32_t channelMask,
                                            dac_channel_trigger_source_t trigSource)
{
    if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
    {
        base->ACTRL = (base->ACTRL & ~(DAC_ACTRL_A_TRIG_SEL_MASK)) | DAC_ACTRL_A_TRIG_SEL(trigSource);
    }

    if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
    {
        base->BCTRL = (base->BCTRL & ~(DAC_BCTRL_B_TRIG_SEL_MASK)) | DAC_BCTRL_B_TRIG_SEL(trigSource);
    }
}

/*!
 * @brief Sets channels trigger type, such as rising edge trigger, falling edge trigger, or both edge trigger.
 *
 * @note To set the same trigger type to both two channels, invoking this API with the parameter \b channelMask
 * set as \b kDAC_ChannelA|kDAC_ChannelB .
 *
 * @param base DAC peripheral base address.
 * @param channelMask The mask of channel, can be the OR'ed value of @ref dac_channel_id_t;
 * @param trigType The selected trigger type, please refer to @ref dac_channel_trigger_type_t;
 */
static inline void DAC_SetChannelTrigType(DAC_Type *base, uint32_t channelMask, dac_channel_trigger_type_t trigType)
{
    if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
    {
        base->ACTRL = (base->ACTRL & ~(DAC_ACTRL_A_TRIG_TYP_MASK)) | DAC_ACTRL_A_TRIG_TYP(trigType);
    }

    if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
    {
        base->BCTRL = (base->BCTRL & ~(DAC_BCTRL_B_TRIG_TYP_MASK)) | DAC_BCTRL_B_TRIG_TYP(trigType);
    }
}

/*!
 * @brief Sets channels timing mode, including not-timing related or timing related.
 *
 * @note To the same timing mode to both two channels, invoking this API with the parameter \b channelMask
 * set as \b kDAC_ChannelA|kDAC_ChannelB .
 *
 * @param base DAC peripheral base address.
 * @param channelMask The mask of channel, can be the OR'ed value of @ref dac_channel_id_t.
 * @param timingMode The selected timing mode, please refer to @ref dac_channel_timing_mode_t for details.
 */
static inline void DAC_SetChannelTimingMode(DAC_Type *base, uint32_t channelMask, dac_channel_timing_mode_t timingMode)
{
    if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
    {
        base->ACTRL = (base->ACTRL & ~(DAC_ACTRL_A_TIME_MODE_MASK)) | DAC_ACTRL_A_TIME_MODE(timingMode);
    }

    if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
    {
        base->BCTRL = (base->BCTRL & ~(DAC_BCTRL_B_TIME_MODE_MASK)) | DAC_BCTRL_B_TIME_MODE(timingMode);
    }
}

/*!
 * @brief Enables/Disables channels DMA.
 *
 * @param base DAC peripheral base address.
 * @param channelMask The mask of channel, can be the OR'ed value of @ref dac_channel_id_t.
 * @param enable Enable/Disable channel DMA data transfer.
 *          - \b true DMA data transfer enabled.
 *          - \b false DMA data transfer disabled.
 */
static inline void DAC_EnableChannelDMA(DAC_Type *base, uint32_t channelMask, bool enable)
{
    if (enable)
    {
        if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
        {
            base->ACTRL |= DAC_ACTRL_A_DEN_MASK;
        }

        if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
        {
            base->BCTRL |= DAC_BCTRL_B_DEN_MASK;
        }
    }
    else
    {
        if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
        {
            base->ACTRL &= ~DAC_ACTRL_A_DEN_MASK;
        }

        if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
        {
            base->BCTRL &= ~DAC_BCTRL_B_DEN_MASK;
        }
    }
}

/*!
 * @brief Sets channels wave type, such as sine, noise, or triangle.
 *
 * @note To set the same wave type to both channel, invoking this API with the parameter \b channelMask
 * set as \b kDAC_ChannelA|kDAC_ChannelB .
 *
 * @param base DAC peripheral base address.
 * @param channelMask The mask of channel, should be the OR'ed value of @ref dac_channel_id_t.
 * @param waveType The wave type to set, please refer to @ref dac_channel_wave_type_t.
 */
static inline void DAC_SetChannelWaveType(DAC_Type *base, uint32_t channelMask, dac_channel_wave_type_t waveType)
{
    if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
    {
        assert(IS_DAC_CHANNEL_A_WAVE(waveType));
        base->ACTRL = (base->ACTRL & ~(DAC_ACTRL_A_WAVE_MASK)) | DAC_ACTRL_A_WAVE(waveType);
    }

    if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
    {
        assert(IS_DAC_CHANNEL_B_WAVE(waveType));
        base->BCTRL = (base->BCTRL & ~(DAC_BCTRL_B_WAVE_MASK)) | DAC_BCTRL_B_WAVE(waveType);
    }
}

/*!
 * @brief Sets DAC channels data.
 *
 * @note To set the same data to both channel, invoking this API with the parameter \b channelMask
 * set as \b kDAC_ChannelA|kDAC_ChannelB .
 *
 * @param base DAC peripheral base address.
 * @param channelMask The mask of channel, can be the OR'ed value of @ref dac_channel_id_t.
 * @param data
 */
static inline void DAC_SetChannelData(DAC_Type *base, uint32_t channelMask, uint16_t data)
{
    if ((channelMask & kDAC_ChannelA) == kDAC_ChannelA)
    {
        base->ADATA = DAC_ADATA_A_DATA(data);
    }

    if ((channelMask & kDAC_ChannelB) == kDAC_ChannelB)
    {
        base->BDATA = DAC_BDATA_B_DATA(data);
    }
}

/*! @}  */

/*!
 * @name Triangle Waveform Configuration Interface
 * @{
 */

/*!
 * @brief Configures the options of triangle waveform.
 *
 * @note This API should be invoked to set the options of triangle waveform when channel A's output wave type is
 * selected as @ref kDAC_WaveTriangle.
 *
 * @param base DAC peripheral base address.
 * @param triangleConfig The pointer of structure which in the type of @ref dac_triangle_config_t.
 */
void DAC_SetTriangleConfig(DAC_Type *base, const dac_triangle_config_t *triangleConfig);

/*! @} */

/*!
 * @name Interrupts Control Interfaces.
 * @{
 */

/*!
 * @brief Enables interrupts, such as channel A data ready interupt, channel A timeout interrupt, and so on.
 *
 * @param base DAC peripheral base address.
 * @param interruptMask The or'ed value of the interrupts to be enabled, please refer to @ref _dac_interrupt_enable.
 */
static inline void DAC_EnableInterrupts(DAC_Type *base, uint32_t interruptMask)
{
    base->IMR &= ~interruptMask;
}

/*!
 * @brief Disables interrupts, such as channel B data ready interrupt, channel B timeout interrupt, and so on.
 *
 * @param base DAC peripheral base address.
 * @param interruptMask The or'ed value of the interrupts to be disabled, please refer to @ref _dac_interrupt_enable.
 */
static inline void DAC_DisableInterrupts(DAC_Type *base, uint32_t interruptMask)
{
    base->IMR |= interruptMask;
}

/*! @} */

/*!
 * @name Status Flags Control Interfaces
 * @{
 */

/*!
 * @brief Gets the status flags, including interrupt status flags, raw status flags, and conversion status flags.
 *
 * @param base DAC peripheral base address.
 * @return The mask of status flags, please refer to @ref _dac_status_flags.
 */

static inline uint32_t DAC_GetStatusFlags(DAC_Type *base)
{
    return ((base->ISR) | ((base->IRSR) << 5UL) | ((base->STATUS) << 10UL));
}

/*!
 * @brief Clears the interrups status flags, such as channel A data ready interrupt flag, channel B data ready interrupt
 * flag, and so on.
 *
 * @param base DAC peripheral base address.
 * @param statusFlagsMask The mask of the status flags to be cleared, please refer to @ref _dac_status_flags.
 */
static inline void DAC_ClearStatusFlags(DAC_Type *base, uint32_t statusFlagsMask)
{
    assert(statusFlagsMask <= 0x1FUL);

    base->ICR = statusFlagsMask;
}

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_DAC_H_ */
