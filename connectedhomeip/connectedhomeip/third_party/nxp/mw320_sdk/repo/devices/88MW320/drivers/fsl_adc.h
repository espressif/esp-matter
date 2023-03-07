/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_ADC_H_
#define _FSL_ADC_H_

#include "fsl_common.h"

/*!
 * @addtogroup adc
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief ADC driver version */
#define FSL_ADC_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0. */

/*!
 * @brief The enumeration of interrupts, this enumeration can be used to enable/disable interrupts.
 */
enum _adc_interrupt_enable
{
    kADC_DataReadyInterruptEnable      = ADC_ADC_REG_IMR_RDY_MASK_MASK,     /*!< Conversion data ready interrupt. */
    kADC_GainSaturationInterruptEnable = ADC_ADC_REG_IMR_GAINSAT_MASK_MASK, /*!< Gain correction saturation interrupt */
    kADC_OffsetSaturationInterruptEnable = ADC_ADC_REG_IMR_OFFSAT_MASK_MASK,        /*!< Offset correction
                                                                                      saturation interupt enable. */
    kADC_NegativeSaturationInterruptEnable = ADC_ADC_REG_IMR_DATASAT_NEG_MASK_MASK, /*!< ADC data negative side
                                                                                       saturation interrupt enable. */
    kADC_PositiveSaturationInterruptEnable = ADC_ADC_REG_IMR_DATASAT_POS_MASK_MASK, /*!< ADC data positive side
                                                                                       saturation interrupt enable. */
    kADC_FifoOverrunInterruptEnable  = ADC_ADC_REG_IMR_FIFO_OVERRUN_MASK_MASK,  /*!< FIFO overrun interrupt enable. */
    kADC_FifoUnderrunInterruptEnable = ADC_ADC_REG_IMR_FIFO_UNDERRUN_MASK_MASK, /*!< FIFO underrun interrupt enable. */
};

/*!
 * @brief The enumeration of adc status flags, including interrupt flags, raw flags, and so on.
 *
 * @note The raw flags will be captured regardless the interrupt mask. Both interrupt flags and raw
 * flags can be cleared.
 */
enum _adc_status_flags
{
    /* Interrupt flags. */
    kADC_DataReadyInterruptFlag          = 1UL << 0UL, /*!< Conversion Data Ready interrupt flag. */
    kADC_GainSaturationInterruptFlag     = 1UL << 1UL, /*!< Gain correction saturation interrupt flag. */
    kADC_OffsetSaturationInterruptFlag   = 1UL << 2UL, /*!< Offset correction saturation interupt flag. */
    kADC_NegativeSaturationInterruptFlag = 1UL << 3UL, /*!< ADC data negative side saturation interrupt flag. */
    kADC_PositiveSaturationInterruptFlag = 1UL << 4UL, /*!< ADC data positive side saturation interrupt flag. */
    kADC_FifoOverrunInterruptFlag        = 1UL << 5UL, /*!< FIFO overrun interrupt flag. */
    kADC_FifoUnderrunInterruptFlag       = 1UL << 6UL, /*!< FIFO underrun interrupt flag. */

    /* Raw flags. */
    kADC_DataReadyRawFlag = 1UL << 7UL,           /*!< Conversion data ready raw flag, this flag will be captured
                                                      regardless the interrupt mask. */
    kADC_GainSaturationRawFlag = 1UL << 8UL,      /*!< Gain correction saturation raw flag, this flag will be
                                                      captured regardless the interrupt mask. */
    kADC_OffsetSaturationRawFlag = 1UL << 9UL,    /*!< Offset correction saturation raw flag, this flag will be
                                                      captured regardless the interrupt mask. */
    kADC_NegativeSaturationRawFlag = 1UL << 10UL, /*!< ADC data negative side saturation raw flag, this flag will
                                                      be captured regardless the interrupt mask. */
    kADC_PositiveSaturationRawFlag = 1UL << 11UL, /*!< ADC data positive side saturation raw flag, this flag will
                                                      be captured regardless the interrupt mask. */
    kADC_FifoOverrunRawFlag = 1UL << 12UL,        /*!< FIFO overrun raw flag, this flag will be captured
                                                      regardless the interrupt mask. */
    kADC_FifoUnderrunRawFlag = 1UL << 13UL,       /*!< FIFO underrun interrupt mask, this flag will be captured
                                                      regardless the interrupt mask. */

    kADC_ActiveStatusFlag       = 1UL << 14UL, /*!< ADC conversion active status flag. */
    kADC_FIFONotEmptyStatusFlag = 1UL << 15UL, /*!< FIFO not empty status flag. */
    kADC_FifoFullStatusFlag     = 1UL << 16UL, /*!< FIFO full status flag. */
};

/*!
 *  @brief ADC clock divider ratio type
 */
typedef enum _adc_clock_divider
{
    kADC_ClockDivider1  = 0U,  /*!< Clock divider ratio is 1 */
    kADC_ClockDivider2  = 1U,  /*!< Clock divider ratio is 2 */
    kADC_ClockDivider3  = 2U,  /*!< Clock divider ratio is 3 */
    kADC_ClockDivider4  = 3U,  /*!< Clock divider ratio is 4 */
    kADC_ClockDivider5  = 4U,  /*!< Clock divider ratio is 5 */
    kADC_ClockDivider6  = 5U,  /*!< Clock divider ratio is 6 */
    kADC_ClockDivider7  = 6U,  /*!< Clock divider ratio is 7 */
    kADC_ClockDivider8  = 7U,  /*!< Clock divider ratio is 8 */
    kADC_ClockDivider9  = 8U,  /*!< Clock divider ratio is 9 */
    kADC_ClockDivider10 = 9U,  /*!< Clock divider ratio is 10 */
    kADC_ClockDivider11 = 10U, /*!< Clock divider ratio is 11 */
    kADC_ClockDivider12 = 11U, /*!< Clock divider ratio is 12 */
    kADC_ClockDivider13 = 12U, /*!< Clock divider ratio is 13 */
    kADC_ClockDivider14 = 13U, /*!< Clock divider ratio is 14 */
    kADC_ClockDivider15 = 14U, /*!< Clock divider ratio is 15 */
    kADC_ClockDivider16 = 15U, /*!< Clock divider ratio is 16 */
    kADC_ClockDivider17 = 16U, /*!< Clock divider ratio is 17 */
    kADC_ClockDivider18 = 17U, /*!< Clock divider ratio is 18 */
    kADC_ClockDivider19 = 18U, /*!< Clock divider ratio is 19 */
    kADC_ClockDivider20 = 19U, /*!< Clock divider ratio is 20 */
    kADC_ClockDivider21 = 20U, /*!< Clock divider ratio is 21 */
    kADC_ClockDivider22 = 21U, /*!< Clock divider ratio is 22 */
    kADC_ClockDivider23 = 22U, /*!< Clock divider ratio is 23 */
    kADC_ClockDivider24 = 23U, /*!< Clock divider ratio is 24 */
    kADC_ClockDivider25 = 24U, /*!< Clock divider ratio is 25 */
    kADC_ClockDivider26 = 25U, /*!< Clock divider ratio is 26 */
    kADC_ClockDivider27 = 26U, /*!< Clock divider ratio is 27 */
    kADC_ClockDivider28 = 27U, /*!< Clock divider ratio is 28 */
    kADC_ClockDivider29 = 28U, /*!< Clock divider ratio is 29 */
    kADC_ClockDivider30 = 29U, /*!< Clock divider ratio is 30 */
    kADC_ClockDivider31 = 30U, /*!< Clock divider ratio is 31 */
    kADC_ClockDivider32 = 31U, /*!< Clock divider ratio is 32 */
} adc_clock_divider_t;

/*!
 * @brief ADC analog portion low-power mode selection.
 */
typedef enum _adc_analog_portion_power_mode
{
    kADC_PowerModeFullBiasingCurrent = 0U, /*!< Full biasing current. */
    kADC_PowerModeHalfBiasingCurrent,      /*!< Half biasing current. */
} adc_analog_portion_power_mode_t;

/*!
 *  @brief ADC resolution type
 */
typedef enum _adc_resolution
{
    kADC_Resolution12Bit      = 0U, /*!< 12-bit resolution */
    kADC_Resolution14Bit      = 1U, /*!< 14-bit resolution */
    kADC_Resolution16Bit      = 2U, /*!< 16-bit resolution */
    kADC_Resolution16BitAudio = 3U, /*!< 16-bit resolution for audio application */
} adc_resolution_t;

/*!
 * @brief The enumeration of adc warm up time, the ADC warm-up state can also bypassed.
 */
typedef enum _adc_warm_up_time
{
    kADC_WarmUpTime1us = 0U,        /*!< ADC warm-up time is 1 us.  */
    kADC_WarmUpTime2us,             /*!< ADC warm-up time is 2 us.  */
    kADC_WarmUpTime3us,             /*!< ADC warm-up time is 3 us.  */
    kADC_WarmUpTime4us,             /*!< ADC warm-up time is 4 us.  */
    kADC_WarmUpTime5us,             /*!< ADC warm-up time is 5 us.  */
    kADC_WarmUpTime6us,             /*!< ADC warm-up time is 6 us.  */
    kADC_WarmUpTime7us,             /*!< ADC warm-up time is 7 us.  */
    kADC_WarmUpTime8us,             /*!< ADC warm-up time is 8 us.  */
    kADC_WarmUpTime9us,             /*!< ADC warm-up time is 9 us.  */
    kADC_WarmUpTime10us,            /*!< ADC warm-up time is 10 us. */
    kADC_WarmUpTime11us,            /*!< ADC warm-up time is 11 us. */
    kADC_WarmUpTime12us,            /*!< ADC warm-up time is 12 us. */
    kADC_WarmUpTime13us,            /*!< ADC warm-up time is 13 us. */
    kADC_WarmUpTime14us,            /*!< ADC warm-up time is 14 us. */
    kADC_WarmUpTime15us,            /*!< ADC warm-up time is 15 us. */
    kADC_WarmUpTime16us,            /*!< ADC warm-up time is 16 us. */
    kADC_WarmUpTime17us,            /*!< ADC warm-up time is 17 us. */
    kADC_WarmUpTime18us,            /*!< ADC warm-up time is 18 us. */
    kADC_WarmUpTime19us,            /*!< ADC warm-up time is 19 us. */
    kADC_WarmUpTime20us,            /*!< ADC warm-up time is 20 us. */
    kADC_WarmUpTime21us,            /*!< ADC warm-up time is 21 us. */
    kADC_WarmUpTime22us,            /*!< ADC warm-up time is 22 us. */
    kADC_WarmUpTime23us,            /*!< ADC warm-up time is 23 us. */
    kADC_WarmUpTime24us,            /*!< ADC warm-up time is 24 us. */
    kADC_WarmUpTime25us,            /*!< ADC warm-up time is 25 us. */
    kADC_WarmUpTime26us,            /*!< ADC warm-up time is 26 us. */
    kADC_WarmUpTime27us,            /*!< ADC warm-up time is 27 us. */
    kADC_WarmUpTime28us,            /*!< ADC warm-up time is 28 us. */
    kADC_WarmUpTime29us,            /*!< ADC warm-up time is 29 us. */
    kADC_WarmUpTime30us,            /*!< ADC warm-up time is 30 us. */
    kADC_WarmUpTime31us,            /*!< ADC warm-up time is 31 us. */
    kADC_WarmUpTime32us,            /*!< ADC warm-up time is 32 us. */
    kADC_WarmUpStateBypass = 0x20U, /*!< ADC warm-up state bypassed. */
} adc_warm_up_time_t;

/*!
 *  @brief ADC voltage reference source type
 */
typedef enum _adc_vref_source
{
    kADC_Vref1P8V         = 0U, /*!< Internal 1.8V reference */
    kADC_Vref1P2V         = 1U, /*!< Internal 1.2V reference */
    kADC_VrefExternal     = 2U, /*!< External single-ended reference though ADC_CH3 */
    kADC_VrefInternal1P2V = 3U, /*!< Internal 1.2V reference with cap filter though ADC_CH3 */
} adc_vref_source_t;

/*!
 *  @brief ADC input mode type
 */
typedef enum _adc_input_mode
{
    kADC_InputSingleEnded  = 0U, /*!< Single-ended mode */
    kADC_InputDifferential = 1U, /*!< Differential mode */
} adc_input_mode_t;

/*!
 *  @brief ADC conversion mode type
 */
typedef enum _adc_conversion_mode
{
    kADC_ConversionOneShot    = 0U, /*!< One shot mode */
    kADC_ConversionContinuous = 1U, /*!< Continuous mode */
} adc_conversion_mode_t;

/*!
 *  @brief ADC scan length type
 */
typedef enum _adc_scan_length
{
    kADC_ScanLength_1  = 0U,  /*!< Scan length is 1 */
    kADC_ScanLength_2  = 1U,  /*!< Scan length is 2 */
    kADC_ScanLength_3  = 2U,  /*!< Scan length is 3 */
    kADC_ScanLength_4  = 3U,  /*!< Scan length is 4 */
    kADC_ScanLength_5  = 4U,  /*!< Scan length is 5 */
    kADC_ScanLength_6  = 5U,  /*!< Scan length is 6 */
    kADC_ScanLength_7  = 6U,  /*!< Scan length is 7 */
    kADC_ScanLength_8  = 7U,  /*!< Scan length is 8 */
    kADC_ScanLength_9  = 8U,  /*!< Scan length is 9 */
    kADC_ScanLength_10 = 9U,  /*!< Scan length is 10 */
    kADC_ScanLength_11 = 10U, /*!< Scan length is 11 */
    kADC_ScanLength_12 = 11U, /*!< Scan length is 12 */
    kADC_ScanLength_13 = 12U, /*!< Scan length is 13 */
    kADC_ScanLength_14 = 13U, /*!< Scan length is 14 */
    kADC_ScanLength_15 = 14U, /*!< Scan length is 15 */
    kADC_ScanLength_16 = 15U, /*!< Scan length is 16 */
} adc_scan_length_t;

/*!
 *  @brief ADC average length type
 */
typedef enum _adc_average_length
{
    kADC_AverageNone = 0U, /*!< Average length: no average */
    kADC_Average2    = 1U, /*!< Average length: 2 */
    kADC_Average4    = 2U, /*!< Average length: 4 */
    kADC_Average8    = 3U, /*!< Average length: 8 */
    kADC_Average16   = 4U, /*!< Average length: 16 */
} adc_average_length_t;

/*!
 * @brief ADC trigger source, including software trigger and multiple hardware trigger sources.
 */
typedef enum _adc_trigger_source
{
    kADC_TriggerSourceGpt      = 0U, /*!< Hardware trigger, trigger source 0: GPT0 for ADC0 */
    kADC_TriggerSourceAcomp    = 1U, /*!< Hardware trigger, trigger source 1: ACOMP0 for ADC0 */
    kADC_TriggerSourceGpio40   = 2U, /*!< Hardware trigger, trigger source 2: GPIO40 */
    kADC_TriggerSourceGpio41   = 3U, /*!< Hardware trigger, trigger source 3: GPIO41 */
    kADC_TriggerSourceSoftware = 4U, /*!< Software trigger. */
} adc_trigger_source_t;

/*!
 *  @brief ADC input buffer gain type
 */
typedef enum _adc_input_gain
{
    kADC_InputGain0P5 = 0U, /*!< Input buffer gain is 0.5 */
    kADC_InputGain1   = 1U, /*!< Input buffer gain is 1 */
    kADC_InputGain2   = 2U, /*!< Input buffer gain is 2 */
} adc_input_gain_t;

/*!
 *  @brief ADC result width type
 */
typedef enum _adc_result_width
{
    kADC_ResultWidth16 = 0U, /*!< 16-bit final result buffer width */
    kADC_ResultWidth32 = 1U, /*!< 32-bit final result buffer width */
} adc_result_width_t;

/*!
 * @brief The threshold of FIFO.
 */
typedef enum _adc_fifo_threshold
{
    kADC_FifoThresholdData1 = 0U, /*!< FIFO Threshold is 1 data. */
    kADC_FifoThresholdData4,      /*!< FIFO Threshold is 4 data. */
    kADC_FifoThresholdData8,      /*!< FIFO Threshold is 8 data. */
    kADC_FifoThresholdData16,     /*!< FIFO Threshold is 16 data. */
} adc_fifo_threshold_t;

/*!
 *  @brief ADC calibration voltage reference type
 */
typedef enum _adc_calibration_ref
{
    kADC_CalibrationVrefInternal = 0, /*!< Internal vref as input for calibration */
    kADC_CalibrationVrefExternal = 1, /*!< External vref as input for calibration */
} adc_calibration_ref_t;

/*!
 *  @brief ADC scan channel type
 */
typedef enum _adc_scan_channel
{
    kADC_ScanChannel0  = 0U,  /*!< Scan channel 0 */
    kADC_ScanChannel1  = 1U,  /*!< Scan channel 1 */
    kADC_ScanChannel2  = 2U,  /*!< Scan channel 2 */
    kADC_ScanChannel3  = 3U,  /*!< Scan channel 3 */
    kADC_ScanChannel4  = 4U,  /*!< Scan channel 4 */
    kADC_ScanChannel5  = 5U,  /*!< Scan channel 5 */
    kADC_ScanChannel6  = 6U,  /*!< Scan channel 6 */
    kADC_ScanChannel7  = 7U,  /*!< Scan channel 7 */
    kADC_ScanChannel8  = 8U,  /*!< Scan channel 8 */
    kADC_ScanChannel9  = 9U,  /*!< Scan channel 9 */
    kADC_ScanChannel10 = 10U, /*!< Scan channel 10 */
    kADC_ScanChannel11 = 11U, /*!< Scan channel 11 */
    kADC_ScanChannel12 = 12U, /*!< Scan channel 12 */
    kADC_ScanChannel13 = 13U, /*!< Scan channel 13 */
    kADC_ScanChannel14 = 14U, /*!< Scan channel 14 */
    kADC_ScanChannel15 = 15U, /*!< Scan channel 15 */
} adc_scan_channel_t;

/*!
 *  @brief ADC channel source type
 */
typedef enum _adc_channel_source
{
    kADC_CH0   = 0U,  /*!< Single-ended mode, channel[0] and vssa */
    kADC_CH1   = 1U,  /*!< Single-ended mode, channel[1] and vssa */
    kADC_CH2   = 2U,  /*!< Single-ended mode, channel[2] and vssa */
    kADC_CH3   = 3U,  /*!< Single-ended mode, channel[3] and vssa */
    kADC_CH4   = 4U,  /*!< Single-ended mode, channel[4] and vssa */
    kADC_CH5   = 5U,  /*!< Single-ended mode, channel[5] and vssa */
    kADC_CH6   = 6U,  /*!< Single-ended mode, channel[6] and vssa */
    kADC_CH7   = 7U,  /*!< Single-ended mode, channel[7] and vssa */
    kADC_VBATS = 8U,  /*!< Single-ended mode, vbat_s and vssa */
    kADC_VREF  = 9U,  /*!< Single-ended mode, vref_12 and vssa */
    kADC_DACA  = 10U, /*!< Single-ended mode, daca and vssa */
    kADC_DACB  = 11U, /*!< Single-ended mode, dacb and vssa */
    kADC_VSSA  = 12U, /*!< Single-ended mode, vssa and vssa */
    kADC_TEMPP = 15U, /*!< Single-ended mode, temp_p and vssa */

    kADC_CH0_CH1       = 0U,  /*!< Differential mode, channel[0] and channel[1] */
    kADC_CH2_CH3       = 1U,  /*!< Differential mode, channel[2] and channel[3] */
    kADC_CH4_CH5       = 2U,  /*!< Differential mode, channel[4] and channel[5] */
    kADC_CH6_CH7       = 3U,  /*!< Differential mode, channel[6] and channel[7] */
    kADC_DACA_DACB     = 4U,  /*!< Differential mode, daca and dacb */
    kADC_VOICEP_VOICEN = 5U,  /*!< Differential mode, voice_p and voice_n */
    kADC_TEMPP_TEMPN   = 15U, /*!< Differential mode, temp_p and temp_n */
} adc_channel_source_t;

/*!
 * @brief Temperature sensor mode, including internal diode mode and external diode mode.
 */
typedef enum _adc_temperature_sensor_mode
{
    kADC_TSensorInternal = 0U, /*!< Internal diode mode. */
    kADC_TSensorExternal,      /*!< External diode mode. */
} adc_temperature_sensor_mode_t;

/*!
 *  @brief ADC audio pga gain type
 */
typedef enum _adc_audio_pga_voltage_gain
{
    kADC_AudioGain4  = 0U, /*!< Audio pga gain is 4 */
    kADC_AudioGain8  = 1U, /*!< Audio pga gain is 8 */
    kADC_AudioGain16 = 2U, /*!< Audio pga gain is 16 */
    kADC_AudioGain32 = 3U, /*!< Audio pga gain is 32 */
} adc_audio_pga_voltage_gain_t;

/*!
 * @brief ADC audio voice level selection.
 */
typedef enum _adc_audio_voice_level
{
    kADC_VoiceLevel0 = 0U, /*!< Input voice level >+255LSB or <-256LSB */
    kADC_VoiceLevel1 = 1U, /*!< Input voice level >+511LSB or <-512LSB */
    kADC_VoiceLevel2 = 2U, /*!< Input voice level >+1023LSB or <-1024LSB */
    kADC_VoiceLevel3 = 3U, /*!< Input voice level >+2047LSB or <-2048LSB */
} adc_audio_voice_level_t;

/*!
 * @brief The structure of adc options, including clock divider, power mode, and so on.
 */
typedef struct _adc_config
{
    /* ADC module basic options */
    adc_clock_divider_t clockDivider : 5U; /*!< Analog 64M clock division ratio,
                                          please refer to @ref adc_clock_divider_t. */
    adc_analog_portion_power_mode_t powerMode : 1U;
    adc_resolution_t resolution : 2U;   /*!< Configure ADC resolution, please refer to
                                           @ref adc_resolution_t.  */
    adc_warm_up_time_t warmupTime : 6U; /*!< Configure warm-up time.  */

    /* ADC conversion related options. */
    adc_vref_source_t vrefSource : 2U;         /*!< Configure voltage reference source,
                                                   please refer to @ref adc_vref_source_t. */
    adc_input_mode_t inputMode : 1U;           /*!< Configure input mode, such as #kADC_InputSingleEnded
                                                   or #kADC_InputDifferential. */
    adc_conversion_mode_t conversionMode : 1U; /*!< Configure convrsion mode, such as
                                                   #kADC_ConversionOneShot or #kADC_ConversionContinuous. */
    adc_scan_length_t scanLength : 4U;         /*!< Configure the length of scan, please refer to
                                                       @ref adc_scan_length_t. */
    adc_average_length_t averageLength : 3U;   /*!< Configure hardware average number, please refer to
                                                   @ref adc_average_length_t */
    adc_trigger_source_t triggerSource : 3U;   /*!< Configure trigger source, the trigger source can be
                                                   divided into hardware trigger and software trigger,
                                                   please refer to @ref adc_trigger_source_t for details. */
    adc_input_gain_t inputGain : 2U;           /*!< Configure ADC input buffer gain, please refer to
                                              @ref adc_input_gain_t. */
    bool enableInputGainBuffer : 1U;           /*!< Enable/Disable input gain buffer.
                                                       - \b true Enable input gain buffer.
                                                       - \b false Disable input gain buffer. */

    /* ADC conversion result related options. */
    adc_result_width_t resultWidth : 1U;     /*!< Select result FIFO data packed format, please
                                                 refer to @ref adc_result_width_t.  */
    adc_fifo_threshold_t fifoThreshold : 2U; /*!< Configure FIFO threshold, please refer to
                                                 @ref adc_fifo_threshold_t. */
    bool enableDMA : 1U;                     /*!< Enable/Disable DMA reqeust.
                                                     - \b true Enable DMA request.
                                                     - \b false Disable DMA request. */
    bool enableADC : 1U;                     /*!< Enable/Disable ADC module.
                                                     - \b true Enable ADC module.
                                                     - \b false Disable ADC module. */
} adc_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name ADC Basic Control Interfaces
 * @{
 */

/*!
 * @brief Initialize ADC module, including clock divider, power mode, and so on.
 *
 * @param base ADC peripheral base address.
 * @param config The pointer to the structure @ref adc_config_t.
 */
void ADC_Init(ADC_Type *base, const adc_config_t *config);

/*!
 * @brief Get default configuration.
 *
 * @code
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
 * @endcode
 *
 * @param config The Pointer to the structure @ref adc_config_t.
 */
void ADC_GetDefaultConfig(adc_config_t *config);

/*!
 * @brief De-initialize the ADC module.
 *
 * @param base ADC peripheral base address.
 */
void ADC_Deinit(ADC_Type *base);

/*!
 * @brief Reset the whole ADC block.
 *
 * @param base ADC peripheral base address.
 */
static inline void ADC_DoSoftwareReset(ADC_Type *base)
{
    base->ADC_REG_CMD |= ADC_ADC_REG_CMD_SOFT_RST_MASK;

    for (uint8_t i = 0U; i < 10U; i++)
    {
        __NOP();
    }
    base->ADC_REG_CMD &= ~ADC_ADC_REG_CMD_SOFT_RST_MASK;
}

/*!
 * @brief Select ADC analog portion power mode.
 *
 * @param base ADC peripheral base address.
 * @param powerMode The power mode to be set, please refer to @ref adc_analog_portion_power_mode_t.
 */
static inline void ADC_SelectAnalogPortionPowerMode(ADC_Type *base, adc_analog_portion_power_mode_t powerMode)
{
    base->ADC_REG_ANA = (base->ADC_REG_ANA & (~ADC_ADC_REG_ANA_BIAS_SEL_MASK)) | ADC_ADC_REG_ANA_BIAS_SEL(powerMode);
}

/*! @} */

/*!
 * @name ADC Calibration Control Interfaces
 * @{
 */

/*!
 * @brief Do automatic calibration measurement.
 *
 * @note After auto calibrate successful, user can invoke ADC_GetAutoCalibrationData() to get self offset calibration
 * value and self gain calibration value.
 *
 * @param base ADC peripheral base address.
 * @param calVref The inpul reference channel for gain calibration,
 *                please refer to @ref adc_calibration_ref_t for details.
 * @retval #kStatus_Success Auto calibrate successfully.
 * @retval #kStatus_Fail Auto calibrate failure.
 */
status_t ADC_DoAutoCalibration(ADC_Type *base, adc_calibration_ref_t calVref);

/*!
 * @brief Get the ADC automatic calibration data.
 *
 * @param base ADC peripheral base address.
 * @param offsetCal Self offset calibration data pointer, evaluate NULL if not requried.
 * @param gainCal Self gain calibration data pointer, evaluate NULL if not requried.
 */
static inline void ADC_GetAutoCalibrationData(ADC_Type *base, uint16_t *offsetCal, uint16_t *gainCal)
{
    if (offsetCal != NULL)
    {
        *offsetCal = (uint16_t)(base->ADC_REG_OFFSET_CAL & ADC_ADC_REG_OFFSET_CAL_OFFSET_CAL_MASK);
    }

    if (gainCal != NULL)
    {
        *gainCal = (uint16_t)(base->ADC_REG_GAIN_CAL & ADC_ADC_REG_GAIN_CAL_GAIN_CAL_MASK);
    }
}

/*!
 * @brief Reset the automatic calibration data.
 *
 * @param base ADC peripheral base address.
 */
static inline void ADC_ResetAutoCalibrationData(ADC_Type *base)
{
    base->ADC_REG_CONFIG |= ADC_ADC_REG_CONFIG_CAL_DATA_RST_MASK;
    for (volatile uint8_t i = 0U; i < 10U; i++)
    {
        __NOP();
    }
    base->ADC_REG_CONFIG &= ~ADC_ADC_REG_CONFIG_CAL_DATA_RST_MASK;
}

/*!
 * @brief Do user defined calibration.
 *
 * @param base ADC peripheral base address.
 * @param offsetCal User defined offset calibration data.
 * @param gainCal User defined gain calibration date.
 */
static inline void ADC_DoUserCalibration(ADC_Type *base, uint16_t offsetCal, uint16_t gainCal)
{
    /* Load user defined offset and gain calibration data */
    base->ADC_REG_OFFSET_CAL = (base->ADC_REG_OFFSET_CAL & ~(ADC_ADC_REG_OFFSET_CAL_OFFSET_CAL_USR_MASK)) |
                               ADC_ADC_REG_OFFSET_CAL_OFFSET_CAL_USR(offsetCal);
    base->ADC_REG_GAIN_CAL = (base->ADC_REG_GAIN_CAL & ~(ADC_ADC_REG_GAIN_CAL_GAIN_CAL_USR_MASK)) |
                             ADC_ADC_REG_GAIN_CAL_GAIN_CAL_USR(gainCal);

    /* Use user defined calibration data */
    base->ADC_REG_CONFIG |= ADC_ADC_REG_CONFIG_CAL_DATA_SEL_MASK;
}

/*! @} */

/*!
 * @name ADC Temperature Sensor Control Interfaces
 * @{
 */

/*!
 * @brief Enable/disable temperature sensor.
 *
 * @note This function is useful only when the channel source is temperature sensor.
 *
 * @param base ADC peripheral base address.
 * @param enable Used to enable/disable temperature sensor.
 *               - \b true Enable temperature sensor.
 *               - \b false Disable temperature sensor.
 */
static inline void ADC_EnableTemperatureSensor(ADC_Type *base, bool enable)
{
    if (enable)
    {
        base->ADC_REG_ANA |= ADC_ADC_REG_ANA_TS_EN_MASK;
    }
    else
    {
        base->ADC_REG_ANA &= ~ADC_ADC_REG_ANA_TS_EN_MASK;
    }
}

/*!
 * @brief Set temperature sensor mode, available selections are internal diode mode and external diode mode.
 *
 * @param base ADC peripheral base address.
 * @param tSensorMode The temperature sensor mode to be set, please refer to @ref adc_temperature_sensor_mode_t.
 */
static inline void ADC_SetTemperatureSensorMode(ADC_Type *base, adc_temperature_sensor_mode_t tSensorMode)
{
    base->ADC_REG_ANA =
        (base->ADC_REG_ANA & ~(ADC_ADC_REG_ANA_TSEXT_SEL_MASK)) | ADC_ADC_REG_ANA_TSEXT_SEL(tSensorMode);
}

/*! @} */

/*!
 * @name ADC Audio Control Interfaces
 * @{
 */

/*!
 * @brief Enable/disable audio PGA and decimation rate select.
 *
 * @param base ADC peripheral base address.
 * @param enable Used to enable/disable audio PGA and decimation rate select.
 *              - \b true Enable audio PGA and decimation rate select.
 *              - \b false Disable audio PGA and decimation rate select.
 */
static inline void ADC_EnableAudio(ADC_Type *base, bool enable)
{
    if (enable)
    {
        base->ADC_REG_AUDIO |= ADC_ADC_REG_AUDIO_EN_MASK;
    }
    else
    {
        base->ADC_REG_AUDIO &= ~ADC_ADC_REG_AUDIO_EN_MASK;
    }
}

/*!
 * @brief Set audio PGA voltage gain.
 *
 * @param base ADC peripheral base address.
 * @param voltageGain The selected audio PGA voltage gain, please refer to @ref adc_audio_pga_voltage_gain_t.
 */
static inline void ADC_SetAudioPGAVoltageGain(ADC_Type *base, adc_audio_pga_voltage_gain_t voltageGain)
{
    base->ADC_REG_AUDIO =
        (base->ADC_REG_AUDIO & (~ADC_ADC_REG_AUDIO_PGA_GAIN_MASK)) | ADC_ADC_REG_AUDIO_PGA_GAIN(voltageGain);
}

/*!
 * @brief Configure audio voice level.
 *
 * @param base ADC peripheral base address.
 * @param enableDetect Used  to enable/disable voice level detection.
 *          - \b true Enable voice level detection.
 *          - \b false Disable voice level detection.
 * @param voiceLevel Selected voice level, please refer to @ref adc_audio_voice_level_t.
 */
void ADC_ConfigAudioVoiceLevel(ADC_Type *base, bool enableDetect, adc_audio_voice_level_t voiceLevel);

/*! @} */

/*!
 * @name ADC Conversion Related Interfaces
 * @{
 */

/*!
 * @brief Set scan channel mux source.
 *
 * @param base ADC peripheral base address.
 * @param scanChannel The selected channel, please refer to @ref adc_scan_channel_t for details.
 * @param channelSource The mux source to be set to the selected channel,
 *                      please refer to @ref adc_channel_source_t for details.
 */
void ADC_SetScanChannel(ADC_Type *base, adc_scan_channel_t scanChannel, adc_channel_source_t channelSource);

/*!
 * @brief If trigger mode is selected as software trigger, invoking this function to start conversion.
 *
 * @note This API will also clear the FIFO.
 *
 * @param base ADC peripheral base address.
 */
static inline void ADC_DoSoftwareTrigger(ADC_Type *base)
{
    base->ADC_REG_CMD |= ADC_ADC_REG_CMD_CONV_START_MASK;
}

/*!
 * @brief Invoke this function to stop conversion.
 *
 * @param base ADC peripheral base address.
 */
static inline void ADC_StopConversion(ADC_Type *base)
{
    base->ADC_REG_CMD &= ~ADC_ADC_REG_CMD_CONV_START_MASK;
}

/*!
 * @brief Get the 32-bit width packed ADC conversion result.
 *
 * @param base ADC peripheral base address.
 * @return 32-bit width packed ADC conversion result.
 */
static inline uint32_t ADC_GetConversionResult(ADC_Type *base)
{
    return (uint32_t)(base->ADC_REG_RESULT);
}

/*!
 * @brief Get the ADC FIFO data count.
 *
 * @param base ADC peripheral base address.
 * @return ADC FIFO data count.
 */
static inline uint8_t ADC_GetFifoDataCount(ADC_Type *base)
{
    return (uint8_t)((base->ADC_REG_STATUS & ADC_ADC_REG_STATUS_FIFO_DATA_COUNT_MASK) >>
                     ADC_ADC_REG_STATUS_FIFO_DATA_COUNT_SHIFT);
}

/*! @} */

/*!
 * @name ADC Interrupt Control Interfaces
 * @{
 */

/*!
 * @brief Enable interrupts, such as conversion data ready interrupt, gain correction saturation interrupt, FIFO
 * under run interrupt, and so on.
 *
 * @param base ADC peripheral base address.
 * @param interruptMask The interrupts to be enabled, should be the OR'ed value of @ref _adc_interrupt_enable.
 */
static inline void ADC_EnableInterrupts(ADC_Type *base, uint32_t interruptMask)
{
    base->ADC_REG_IMR &= ~(interruptMask);
}

/*!
 * @brief Disable interrupts, such as conversion data ready interrupt, gain correction saturation interrupt, FIFO
 * under run interrupt, and so on.
 *
 * @param base ADC peripheral base address.
 * @param interruptMask The interrupts to be disabled, should be the OR'ed value of @ref _adc_interrupt_enable.
 */
static inline void ADC_DisableInterrupts(ADC_Type *base, uint32_t interruptMask)
{
    base->ADC_REG_IMR |= interruptMask;
}

/*! @} */

/*!
 * @name ADC Status Control Interfaces
 * @{
 */

/*!
 * @brief Get status flags, including interrupt flags, raw flags, and so on.
 *
 * @param base ADC peripheral base address.
 * @return The OR'ed value of ADC status flags, please refer to @ref _adc_status_flags for details.
 */
uint32_t ADC_GetStatusFlags(ADC_Type *base);

/*!
 * @brief Clear status flags.
 *
 * @note Only interrupt flags and raw flags can be cleared.
 *
 * @param base ADC peripheral base address.
 * @param statusFlagsMask The OR'ed value of status flags to be cleared,
 *                        please refer to @ref _adc_status_flags for details.
 */
static inline void ADC_ClearStatusFlags(ADC_Type *base, uint32_t statusFlagsMask)
{
    base->ADC_REG_ICR = (statusFlagsMask & 0x7FUL) | ((statusFlagsMask >> 7UL) & 0x7FUL);
}

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_ADC_H_ */
