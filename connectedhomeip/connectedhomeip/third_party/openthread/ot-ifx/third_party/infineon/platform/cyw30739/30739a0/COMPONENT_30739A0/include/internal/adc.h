/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/*
********************************************************************
*    File Name: adc.h
*
*    Abstract: This file defines the ADC driver
*
*
********************************************************************
*/

#ifndef __ADC__H__
#define __ADC__H__

#include "brcm_fw_types.h"
#include "adc_cs.h"
#ifdef __cplusplus
extern "C" {
#endif

/**  \addtogroup ADC
 *  \ingroup HardwareDrivers
*/

/***************************************************************
* the bits of Register cr_pulse_reset_peri_adr (0x00320100) to reset adc circuit
****************************************************************/
typedef enum
{
    RST_ADC_DMA_HCLK    = (1 << 15),
    RST_ADC_CLK_ADC     = (1 << 17),
    RST_ADC_CLK_API     = (1 << 18),
} PULSE_RESET_PERI_BITS;

#define EQ_FILTER_COEFFICIENT_SIZE_N  116
#define NUMBER_OF_AUDIO_AUX_FILTER_COEF 19

#define BIQUAD_EQ_FILTER_COEFF_COUNT       5  // number of 16-bit coefficients per biquad
#define BIQUAD_EQ_FILTER_COUNT             4  // 4 biquads to be used as EQ filters

#pragma pack(1)
typedef PACKED struct
{
    UINT8       enable;                   // 1 Enable DRC, 0 Disable DRC
    UINT16      waitTime;                 // Wait time in mSec, 0x2EE = 750 mSec.
    UINT8       knee1;                    // Knee 1, 68.5dB,       2660, in 1/2 dB steps.  10^((RSSI_target/2 + 30)/20).
    UINT8       knee2;                    // Knee 2, 75dB,         5623, in 1/2 dB steps.  10^((RSSI_target/2 + 30)/20).
    UINT8       knee3;                    // Knee 3, 81dB,        11220, in 1/2 dB steps.  10^((RSSI_target/2 + 30)/20).
    UINT16      attackTime;               // Attack time in mSec.  0x03E8 = 1000 mSec
    UINT16      decayTime;                // Decay time in mSec.  0x001F = 31 mSec.
    UINT16      saturationLevel;          // Saturation Level, 0x6800 = 26624.  This will be the max output level.
                                          // The DRC will behave like an AGC when the DRC curve exceeds this amount.
                                          // This value will be used when the pga gain is set lower than 18dB by the DRC loop.
}AdcAudioDrcSettings;

typedef PACKED struct {
    INT16               coeff[EQ_FILTER_COEFFICIENT_SIZE_N];
} filter_coeff_data_t;

typedef PACKED struct {
    UINT16              audio_aux_filter_coef[NUMBER_OF_AUDIO_AUX_FILTER_COEF];
    UINT16              biQuadEqFilterCoeffs[BIQUAD_EQ_FILTER_COEFF_COUNT*BIQUAD_EQ_FILTER_COUNT];
    filter_coeff_data_t eqFilter;
} AdcAudioFilterCfg_t;
#pragma pack()

#define READY_DELAY     10
extern UINT8 g_dma_ActiveChannels;

// config ADC block to default/userconfig state
void adc_config(void);

// config ADC audio to setup memory for adcAudioFifo and adcAudioFifo2
void adc_audioConfig(AdcAudioFifo *gAdcAudioFifo, UINT8 fifoNum, AdcAudioFifo *gAdcAudioFifo2);

// calibration for the referenceInput
void adc_adcCalibrate(UINT32 refVoltageInMicroVolts, ADC_INPUT_CHANNEL_SEL referenceInput);

// read the adc value from hardware register as it is
INT16 adc_readSampleRaw(ADC_INPUT_CHANNEL_SEL channel_idx);

// read converted adc voltage
UINT32 adc_readVoltage(ADC_INPUT_CHANNEL_SEL idx);

ADC_POWER_TYPE adc_setPower(ADC_POWER_TYPE pwr);

void adc_SetInputRange(ADC_INPUT_RANGE_SEL range_idx);

/// Convert GPIO to ADC input
UINT8 adc_convertGPIOtoADCInput(UINT8 gpio);

/// Convert ADC input to GPIO
UINT8 adc_convertADCInputtoGPIO(UINT8 adcInput);

/// assign PDM audio input pin
void adc_pdm_pinconfig(UINT8 ch1, UINT8 risingEdge1, UINT8 ch2, UINT8 risingEdge2, UINT8 clk);

/// reset adc clk and adc related circuit
void adc_resetAdcClk(void);

extern AdcState adcState;

/* @} */

#ifdef __cplusplus
}
#endif

#endif
