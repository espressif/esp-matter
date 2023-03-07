/*
 * Copyright 2020-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "fsl_common.h"

#include "fsl_dac.h"

#include "fsl_device_registers.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_power.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_DAC_BASE               DAC
#define DEMO_DAC_CHANNEL            kDAC_ChannelB
#define DEMO_DAC_TRIANGLE_MAX_VALUE kDAC_TriangleAmplitude1023
#define DEMO_DAC_TRIANGLE_STEP_SIZE kDAC_TriangleStepSize1
#define DEMO_DAC_TRIANGLE_MIN_VALUE 0U

typedef enum _dac_available_waveform_type
{
    kDEMO_SelectFullTriangle = 'A',
    kDEMO_SelectHalfTriangle = 'B',
    kDEMO_SelectSine         = 'C',
    kDEMO_SelectNoise        = 'D',
} dac_available_waveform_type_t;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void DEMO_SelectWaveformType(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

int main(void)
{
    dac_config_t dacConfig;
    dac_channel_config_t dacChannelConfig;

    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /* DAC reference voltage selected as intenal voltage, so
     * power up bandgap in advance. */
    POWER_PowerUpBandgap();
    while (!POWER_CheckBandgapReady())
        ;

    PRINTF("\r\n DAC Waveform Generator Example!\r\n");

    /*
     * Get DAC module default configuration.
     *  config->conversionRate = kDAC_ConversionRate62P5KHZ;
     *  config->refSource = kDAC_ReferenceInternalVoltageSource;
     *  config->rangeSelect = kDAC_RangeLarge;
     */
    DAC_GetDefaultConfig(&dacConfig);
    dacConfig.conversionRate = kDAC_ConversionRate500KHZ;
    DAC_Init(DEMO_DAC_BASE, &dacConfig);

    dacChannelConfig.enableConversion = true;
    dacChannelConfig.outMode          = kDAC_ChannelOutputPAD;
    dacChannelConfig.enableDMA        = false;
    dacChannelConfig.enableTrigger    = false;
    dacChannelConfig.timingMode       = kDAC_NonTimingCorrelated;
    dacChannelConfig.waveType         = kDAC_WaveNormal;
    DAC_SetChannelConfig(DEMO_DAC_BASE, DEMO_DAC_CHANNEL, &dacChannelConfig);

    if (DEMO_DAC_CHANNEL != kDAC_ChannelA)
    {
        DAC_EnableChannelConversion(DEMO_DAC_BASE, kDAC_ChannelA, true);
    }
    while (1)
    {
        DEMO_SelectWaveformType();
    }
}

/*!
 * @brief Select waveform type based on users input.
 */
static void DEMO_SelectWaveformType(void)
{
    char ch;
    dac_available_waveform_type_t waveType;
    dac_triangle_config_t triangleConfig;

    PRINTF("\r\nPlease select the waveform to be generated:\r\n");
    PRINTF("\t %c -- Full Triangle wave\r\n", kDEMO_SelectFullTriangle);
    PRINTF("\t %c -- Half Triangle wave\r\n", kDEMO_SelectHalfTriangle);
    PRINTF("\t %c -- Sine wave\r\n", kDEMO_SelectSine);
    PRINTF("\t %c -- Noise wave\r\n", kDEMO_SelectNoise);
    ch = GETCHAR();

    if (ch >= 'a')
    {
        ch -= 'a' - 'A';
    }

    waveType = (dac_available_waveform_type_t)ch;

    switch (waveType)
    {
        case kDEMO_SelectFullTriangle:
        case kDEMO_SelectHalfTriangle:
            if (DEMO_DAC_CHANNEL != kDAC_ChannelA)
            {
                DAC_SetChannelWaveType(DEMO_DAC_BASE, kDAC_ChannelA, kDAC_WaveTriangle);
                DAC_SetChannelWaveType(DEMO_DAC_BASE, kDAC_ChannelB, kDAC_WaveNoiseDifferential);
            }
            triangleConfig.triangleMamp     = DEMO_DAC_TRIANGLE_MAX_VALUE;
            triangleConfig.triangleStepSize = DEMO_DAC_TRIANGLE_STEP_SIZE;
            triangleConfig.triangleBase     = DEMO_DAC_TRIANGLE_MIN_VALUE;
            triangleConfig.triangleWaveform = kDAC_TriangleFull;
            if (waveType == kDEMO_SelectHalfTriangle)
            {
                triangleConfig.triangleWaveform = kDAC_TriangleHalf;
            }
            DAC_SetTriangleConfig(DEMO_DAC_BASE, &triangleConfig);
            break;
        case kDEMO_SelectSine:
            if (DEMO_DAC_CHANNEL != kDAC_ChannelA)
            {
                DAC_SetChannelWaveType(DEMO_DAC_BASE, kDAC_ChannelA, kDAC_WaveSine);
                DAC_SetChannelWaveType(DEMO_DAC_BASE, kDAC_ChannelB, kDAC_WaveNoiseDifferential);
            }
            break;
        case kDEMO_SelectNoise:
            if (DEMO_DAC_CHANNEL != kDAC_ChannelA)
            {
                DAC_SetChannelWaveType(DEMO_DAC_BASE, kDAC_ChannelA, kDAC_WaveNoiseDifferential);
                DAC_SetChannelWaveType(DEMO_DAC_BASE, kDAC_ChannelB, kDAC_WaveNoiseDifferential);
            }
            break;
        default:
            PRINTF("\r\nError Input Please Retry!\r\n");
            break;
    }
}
