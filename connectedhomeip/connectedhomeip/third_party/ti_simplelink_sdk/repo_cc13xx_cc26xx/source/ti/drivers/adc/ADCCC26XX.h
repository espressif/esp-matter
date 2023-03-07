/*
 * Copyright (c) 2016-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
 *  @file       ADCCC26XX.h
 *  @brief      ADC driver implementation for the ADC peripheral on CC26XX
 *
 *  This ADC driver implementation is designed to operate on a ADC peripheral
 *  for CC26XX.
 *
 *  Refer to @ref ADC.h for a complete description of APIs & example of use.
 *
 ******************************************************************************
 */
#ifndef ti_drivers_adc_ADCCC26XX__include
#define ti_drivers_adc_ADCCC26XX__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/ADC.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/aux_adc.h)

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief  Amount of time the ADC spends sampling the analogue input.
 *
 *  The analogue to digital conversion process consists of two phases in the
 *  CC26XX ADC, the sampling and conversion phases. During the sampling phase,
 *  the ADC samples the analogue input signal. The duration of the sampling
 *  phase is configurable. Larger input loads require longer sample times for
 *  the most accurate results.
 */
typedef enum {
    ADCCC26XX_SAMPLING_DURATION_2P7_US    = AUXADC_SAMPLE_TIME_2P7_US,
    ADCCC26XX_SAMPLING_DURATION_5P3_US    = AUXADC_SAMPLE_TIME_5P3_US,
    ADCCC26XX_SAMPLING_DURATION_10P6_US   = AUXADC_SAMPLE_TIME_10P6_US,
    ADCCC26XX_SAMPLING_DURATION_21P3_US   = AUXADC_SAMPLE_TIME_21P3_US,
    ADCCC26XX_SAMPLING_DURATION_42P6_US   = AUXADC_SAMPLE_TIME_42P6_US,
    ADCCC26XX_SAMPLING_DURATION_85P3_US   = AUXADC_SAMPLE_TIME_85P3_US,
    ADCCC26XX_SAMPLING_DURATION_170_US    = AUXADC_SAMPLE_TIME_170_US,
    ADCCC26XX_SAMPLING_DURATION_341_US    = AUXADC_SAMPLE_TIME_341_US,
    ADCCC26XX_SAMPLING_DURATION_682_US    = AUXADC_SAMPLE_TIME_682_US,
    ADCCC26XX_SAMPLING_DURATION_1P37_MS   = AUXADC_SAMPLE_TIME_1P37_MS,
    ADCCC26XX_SAMPLING_DURATION_2P73_MS   = AUXADC_SAMPLE_TIME_2P73_MS,
    ADCCC26XX_SAMPLING_DURATION_5P46_MS   = AUXADC_SAMPLE_TIME_5P46_MS,
    ADCCC26XX_SAMPLING_DURATION_10P9_MS   = AUXADC_SAMPLE_TIME_10P9_MS
} ADCCC26XX_Sampling_Duration;

/*!
 *  @brief  Specifies whether the internal reference of the ADC is sourced from
 *          the battery voltage or a fixed internal source.
 *
 *  - In practice, using the internal fixed voltage reference sets the upper
 *    range of the ADC to a fixed value. That value is 4.3V with input scaling
 *    enabled and ~1.4785V with input scaling disabled. In this mode, the output
 *    is a function of the input voltage multiplied by the resolution in
 *    alternatives (not bits) divided by the upper voltage range of the ADC.
 *    Output = Input (V) * 2^12 / (ADC range (V))
 *
 *  - Using VDDS as a reference scales the upper range of the ADC with the
 *    battery voltage. As the battery depletes and its voltage drops, so does the
 *    range of the ADC. This is helpful when measuring signals that are generated
 *    relative to the battery voltage. In this mode, the output is a function of
 *    the input voltage multiplied by the resolution in alternatives (not bits)
 *    divided by VDDS multiplied by a scaling factor derived from the input
 *    scaling. Output = Input (V) * 2^12 / (VDDS (V) * Scaling factor), where the
 *    scaling factor is ~1.4785/4.3 for input scaling disabled and 1 for input
 *    scaling enabled.
 *
 *  @note   The actual reference values are slightly different for each device
 *          and are higher than the values specified above. This gain is saved
 *          in the FCFG. The function ::ADC_convertToMicroVolts() must be used
 *          to derive actual voltage values. Do not attempt to compare raw
 *          values between devices or derive a voltage from them yourself. The
 *          results of doing so will only be approximately correct.
 *
 *  @warning    Even though the upper voltage range of the ADC is 4.3 volts in
 *              fixed mode with input scaling enabled, the input should never
 *              exceed VDDS as per the data sheet.
 */
typedef enum {
    ADCCC26XX_FIXED_REFERENCE       = AUXADC_REF_FIXED,
    ADCCC26XX_VDDS_REFERENCE        = AUXADC_REF_VDDS_REL
} ADCCC26XX_Reference_Source;

/*!
 *  @brief List of sources the ADC can be configured to trigger off of.
 *
 *  The ADC driver currently only supports the driver manually triggering a
 *  conversion. Support for other trigger sources may be added later.
 */
typedef enum {
    ADCCC26XX_TRIGGER_MANUAL    = AUXADC_TRIGGER_MANUAL,
} ADCCC26XX_Trigger_Source;

/* ADC function table pointer */
extern const ADC_FxnTable ADCCC26XX_fxnTable;

/*!
 *  @brief  ADCCC26XX Hardware attributes
 *  These fields are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions.
 *
 */
typedef struct {
    /*!< DIO that the ADC is routed to */
    uint8_t                     adcDIO;
    /*!< Internal signal routed to comparator B */
    uint8_t                     adcCompBInput;
    /*!< Should the raw output be trimmed before returning it */
    bool                        returnAdjustedVal;
    /*!< Is input scaling enabled */
    bool                        inputScalingEnabled;
    /*!< Reference voltage in microvolts*/
    uint_fast32_t               refVoltage;
    /*!< Reference source for the ADC to use */
    ADCCC26XX_Reference_Source  refSource;
    /*!< Time the ADC spends sampling. This is load dependent */
    ADCCC26XX_Sampling_Duration samplingDuration;
    /*!< Source that the ADC triggers off of. Currently only supports AUXADC_TRIGGER_MANUAL */
    ADCCC26XX_Trigger_Source    triggerSource;
} ADCCC26XX_HWAttrs;

/*!
 *  @brief  ADCCC26XX Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    /*!< Flag if the instance is in use */
    bool                            isOpen;
    /*!< Flag to indicate if thread safety is ensured by the driver */
    bool                            isProtected;
} ADCCC26XX_Object;



#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_adc_ADCCC26XX__include */
