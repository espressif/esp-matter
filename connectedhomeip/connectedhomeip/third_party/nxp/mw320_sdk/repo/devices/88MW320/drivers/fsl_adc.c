/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_adc.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.wm_adc"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
static uint32_t ADC_GetInstance(ADC_Type *base);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Variables
 ******************************************************************************/
static ADC_Type *const s_adcBases[] = ADC_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to adc clocks for each instance. */
static const clock_ip_name_t s_adcClocks[] = ADC_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*!
 * @brief Get the ADC peripheral instance
 *
 * @param base ADC peripheral base address.
 * @return The instance of input ADC peripheral base address.
 */
static uint32_t ADC_GetInstance(ADC_Type *base)
{
    uint32_t instance;
    uint32_t adcArrayCount = (sizeof(s_adcBases) / sizeof(s_adcBases[0]));

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < adcArrayCount; instance++)
    {
        if (s_adcBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < adcArrayCount);

    return instance;
}
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*!
 * brief Initialize ADC module, including clock divider, power mode, and so on.
 *
 * param base ADC peripheral base address.
 * param config The pointer to the structure adc_config_t.
 */
void ADC_Init(ADC_Type *base, const adc_config_t *config)
{
    assert(config != NULL);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the ADC clock*/
    (void)CLOCK_EnableClock(s_adcClocks[ADC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
    uint32_t tmp32;

    ADC_DoSoftwareReset(base);

    base->ADC_REG_GENERAL = (base->ADC_REG_GENERAL & ~(ADC_ADC_REG_GENERAL_CLK_DIV_RATIO_MASK)) |
                            ADC_ADC_REG_GENERAL_CLK_DIV_RATIO(config->clockDivider);

    tmp32 = base->ADC_REG_CONFIG;
    tmp32 &=
        ~(ADC_ADC_REG_CONFIG_TRIGGER_SEL_MASK | ADC_ADC_REG_CONFIG_TRIGGER_EN_MASK |
          ADC_ADC_REG_CONFIG_CONT_CONV_EN_MASK | ADC_ADC_REG_CONFIG_AVG_SEL_MASK | ADC_ADC_REG_CONFIG_SCAN_LENGTH_MASK);
    tmp32 |= ADC_ADC_REG_CONFIG_CONT_CONV_EN(config->conversionMode) |
             ADC_ADC_REG_CONFIG_AVG_SEL(config->averageLength) | ADC_ADC_REG_CONFIG_SCAN_LENGTH(config->scanLength);
    if (config->triggerSource != kADC_TriggerSourceSoftware)
    {
        tmp32 |= ADC_ADC_REG_CONFIG_TRIGGER_EN_MASK | ADC_ADC_REG_CONFIG_TRIGGER_SEL(config->triggerSource);
    }
    base->ADC_REG_CONFIG = tmp32;

    tmp32 = base->ADC_REG_INTERVAL;
    if (config->warmupTime == kADC_WarmUpStateBypass)
    {
        tmp32 |= ADC_ADC_REG_INTERVAL_BYPASS_WARMUP_MASK;
    }
    else
    {
        tmp32 =
            (tmp32 & ~(ADC_ADC_REG_INTERVAL_WARMUP_TIME_MASK)) | ADC_ADC_REG_INTERVAL_WARMUP_TIME(config->warmupTime);
    }
    base->ADC_REG_INTERVAL = tmp32;

    tmp32 = base->ADC_REG_ANA;
    tmp32 &= ~(ADC_ADC_REG_ANA_VREF_SEL_MASK | ADC_ADC_REG_ANA_SINGLEDIFF_MASK | ADC_ADC_REG_ANA_INBUF_GAIN_MASK |
               ADC_ADC_REG_ANA_INBUF_EN_MASK | ADC_ADC_REG_ANA_BIAS_SEL_MASK | ADC_ADC_REG_ANA_RES_SEL_MASK);
    tmp32 |= ADC_ADC_REG_ANA_VREF_SEL(config->vrefSource) | ADC_ADC_REG_ANA_SINGLEDIFF(config->inputMode) |
             ADC_ADC_REG_ANA_INBUF_GAIN(config->inputGain) | ADC_ADC_REG_ANA_INBUF_EN(config->enableInputGainBuffer) |
             ADC_ADC_REG_ANA_BIAS_SEL(config->powerMode) | ADC_ADC_REG_ANA_RES_SEL(config->resolution);
    base->ADC_REG_ANA = tmp32;

    base->ADC_REG_RESULT_BUF = (base->ADC_REG_RESULT_BUF & ~(ADC_ADC_REG_RESULT_BUF_WIDTH_SEL_MASK)) |
                               ADC_ADC_REG_RESULT_BUF_WIDTH_SEL(config->resultWidth);

    tmp32 = base->ADC_REG_DMAR;
    tmp32 &= ~(ADC_ADC_REG_DMAR_DMA_EN_MASK | ADC_ADC_REG_DMAR_FIFO_THL_MASK);
    tmp32 |= ADC_ADC_REG_DMAR_FIFO_THL(config->fifoThreshold) | ADC_ADC_REG_DMAR_DMA_EN(config->enableDMA);
    base->ADC_REG_DMAR = tmp32;

    if (config->enableADC)
    {
        base->ADC_REG_GENERAL |= ADC_ADC_REG_GENERAL_GLOBAL_EN_MASK;
    }
}

/*!
 * brief Get default configuration.
 *
 * code
 *      config->clockDivider = kADC_ClockDivider1;
 *      config->powerMode = kADC_PowerModeFullBiasingCurrent;
 *      config->resolution = kADC_Resolution12Bit;
 *      config->warmupTime = kADC_WarmUpTime16us;
 *      config->vrefSource = kADC_Vref1P2V;
 *      config->inputMode = kADC_InputSingleEnded;
 *      config->conversionMode = kADC_ConversionContinuous;
 *      config->scanLength = kADC_ScanLength_1;
 *      config->averageLength = kADC_AverageNone;
 *      config->triggerSource = kADC_TriggerSourceSoftware;
 *      config->inputGain = kADC_InputGain1;
 *      config->enableInputGainBuffer = false;
 *      config->resultWidth = kADC_ResultWidth16;
 *      config->fifoThreshold = kADC_FifoThresholdData1;
 *      config->enableDMA = false;
 *      config->enableADC = false;
 * endcode
 * param config Pointer to the structure adc_config_t.
 */
void ADC_GetDefaultConfig(adc_config_t *config)
{
    assert(config != NULL);

    (void)memset(config, 0U, sizeof(adc_config_t));

    config->clockDivider = kADC_ClockDivider1;
    config->powerMode    = kADC_PowerModeFullBiasingCurrent;
    config->resolution   = kADC_Resolution12Bit;
    config->warmupTime   = kADC_WarmUpTime16us;

    config->vrefSource            = kADC_Vref1P2V;
    config->inputMode             = kADC_InputSingleEnded;
    config->conversionMode        = kADC_ConversionContinuous;
    config->scanLength            = kADC_ScanLength_1;
    config->averageLength         = kADC_AverageNone;
    config->triggerSource         = kADC_TriggerSourceSoftware;
    config->inputGain             = kADC_InputGain1;
    config->enableInputGainBuffer = false;

    config->resultWidth   = kADC_ResultWidth16;
    config->fifoThreshold = kADC_FifoThresholdData1;
    config->enableDMA     = false;
    config->enableADC     = false;
}

/*!
 * brief De-initialize the ADC module.
 *
 * param base ADC peripheral base address.
 */
void ADC_Deinit(ADC_Type *base)
{
    ADC_DoSoftwareReset(base);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the ADC clock*/
    (void)CLOCK_DisableClock(s_adcClocks[ADC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Set scan channel mux source.
 *
 * param base ADC peripheral base address.
 * param scanChannel The selected channel, please refer to adc_scan_channel_t for details.
 * param channelSource The mux source to be set to the selected channel,
 *                      please refer to adc_channel_source_t for details.
 */
void ADC_SetScanChannel(ADC_Type *base, adc_scan_channel_t scanChannel, adc_channel_source_t channelSource)
{
    if (scanChannel < kADC_ScanChannel8)
    {
        base->ADC_REG_SCN1 = (base->ADC_REG_SCN1 & ~(0xFUL << (uint32_t)scanChannel)) |
                             ((uint32_t)channelSource << (uint32_t)scanChannel);
    }
    else
    {
        base->ADC_REG_SCN2 = (base->ADC_REG_SCN2 & ~(0xFUL << ((uint32_t)scanChannel - 8UL))) |
                             ((uint32_t)channelSource << ((uint32_t)scanChannel - 8UL));
    }
}

/*!
 * brief Do automatic calibration measurement.
 *
 * note After auto calibrate successful, user can invoke ADC_GetAutoCalibrationData() to get self offset calibration
 * value and self gain calibration value.
 *
 * param base ADC peripheral base address.
 * param calVref The inpul reference channel for gain calibration,
 *                please refer to adc_calibration_ref_t for details.
 * retval kStatus_Success Auto calibrate successfully.
 * retval kStatus_Fail Auto calibrate failure.
 */
status_t ADC_DoAutoCalibration(ADC_Type *base, adc_calibration_ref_t calVref)
{
    uint32_t tmp32;

    tmp32 = base->ADC_REG_CONFIG;
    tmp32 &= ~(ADC_ADC_REG_CONFIG_CAL_VREF_SEL_MASK);
    tmp32 |= ADC_ADC_REG_CONFIG_CAL_VREF_SEL(calVref);
    base->ADC_REG_CONFIG = tmp32;

    base->ADC_REG_CMD &= ~ADC_ADC_REG_CMD_CONV_START_MASK;
    base->ADC_REG_GENERAL |= ADC_ADC_REG_GENERAL_ADC_CAL_EN_MASK;
    base->ADC_REG_CMD |= ADC_ADC_REG_CMD_CONV_START_MASK;

    /* Wait for self calibration done */
    for (volatile uint32_t i = 0UL; i < 1000000UL; i++)
    {
        if ((base->ADC_REG_GENERAL & ADC_ADC_REG_GENERAL_ADC_CAL_EN_MASK) == 0UL)
        {
            base->ADC_REG_CONFIG &= ~ADC_ADC_REG_CONFIG_CAL_DATA_SEL_MASK;
            base->ADC_REG_CMD &= ~ADC_ADC_REG_CMD_CONV_START_MASK;
            return kStatus_Success;
        }
    }

    return kStatus_Fail;
}

/*!
 * brief Configure audio voice level.
 *
 * param base ADC peripheral base address.
 * param enableDetect Used  to enable/disable voice level detection.
 *          - \b true Enable voice level detection.
 *          - \b false Disable voice level detection.
 * param voiceLevel Selected voice level, please refer to adc_audio_voice_level_t.
 */
void ADC_ConfigAudioVoiceLevel(ADC_Type *base, bool enableDetect, adc_audio_voice_level_t voiceLevel)
{
    uint32_t tmp32;

    tmp32 = base->ADC_REG_VOICE_DET;
    tmp32 &= ~(ADC_ADC_REG_VOICE_DET_DET_EN_MASK | ADC_ADC_REG_VOICE_DET_LEVEL_SEL_MASK);
    tmp32 |= ADC_ADC_REG_VOICE_DET_DET_EN(enableDetect) | ADC_ADC_REG_VOICE_DET_LEVEL_SEL(voiceLevel);
    base->ADC_REG_VOICE_DET = tmp32;
}

/*!
 * brief Get status flags, including interrupt flags, raw flags, and so on.
 *
 * param base ADC peripheral base address.
 * return The OR'ed value of ADC status flags, please refer to _adc_status_flags for details.
 */
uint32_t ADC_GetStatusFlags(ADC_Type *base)
{
    uint32_t tmp32;

    tmp32 = base->ADC_REG_ISR;
    tmp32 |= (base->ADC_REG_IRSR) << 7UL;
    tmp32 |= (base->ADC_REG_STATUS) << 14UL;

    return tmp32;
}
