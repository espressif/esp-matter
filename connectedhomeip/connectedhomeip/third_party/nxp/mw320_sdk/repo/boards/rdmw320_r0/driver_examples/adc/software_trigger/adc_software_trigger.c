/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "fsl_common.h"

#include "fsl_adc.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_power.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_ADC_IRQHANDLER     ADC0_IRQHandler
#define DEMO_ADC_BASE           ADC
#define DEMO_ADC_CHANNEL_SOURCE kADC_CH0
#define DEMO_ADC_IRQn           ADC0_IRQn


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool g_conversionDataReady = false;
/*******************************************************************************
 * Code
 ******************************************************************************/
void DEMO_ADC_IRQHANDLER(void)
{
    if ((ADC_GetStatusFlags(DEMO_ADC_BASE) & kADC_DataReadyInterruptFlag) != 0UL)
    {
        g_conversionDataReady = true;
        ADC_ClearStatusFlags(DEMO_ADC_BASE, kADC_DataReadyInterruptFlag);
    }
}

int main(void)
{
    adc_config_t adcConfig;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* DAC reference voltage selected as intenal voltage, so
     * power up bandgap in advance. */
    POWER_PowerUpBandgap();
    while (!POWER_CheckBandgapReady())
        ;

    PRINTF("\r\nADC Software Trigger Example!\r\n");
    PRINTF("Resolution: 16 bit.\r\n");
    PRINTF("Input Mode: Single Ended.\r\n");
    PRINTF("Input Range: 0V to 1.8V.\r\n");
    /*
     *  config->clockDivider = kADC_ClockDivider1;
     *  config->powerMode = kADC_PowerModeFullBiasingCurrent;
     *  config->resolution = kADC_Resolution12Bit;
     *  config->warmupTime = kADC_WarmUpTime16us;
     *  config->vrefSource = kADC_Vref1P2V;
     *  config->inputMode = kADC_InputSingleEnded;
     *  config->conversionMode = kADC_ConversionContinuous;
     *  config->scanLength = kADC_ScanLength_1;
     *  config->averageLength = kADC_AverageNone;
     *  config->triggerSource = kADC_TriggerSourceSoftware;
     *  config->inputGain = kADC_InputGain1;
     *  config->enableInputGainBuffer = false;
     *  config->resultWidth = kADC_ResultWidth16;
     *  config->fifoThreshold = kADC_FifoThresholdData1;
     *  config->enableDMA = false;
     *  config->enableADC = false;
     */
    ADC_GetDefaultConfig(&adcConfig);
    adcConfig.vrefSource            = kADC_Vref1P8V;
    adcConfig.inputMode             = kADC_InputSingleEnded;
    adcConfig.conversionMode        = kADC_ConversionOneShot;
    adcConfig.inputGain             = kADC_InputGain1;
    adcConfig.resolution            = kADC_Resolution16Bit;
    adcConfig.fifoThreshold         = kADC_FifoThresholdData1;
    adcConfig.enableInputGainBuffer = true;
    adcConfig.enableADC             = true;

    ADC_Init(DEMO_ADC_BASE, &adcConfig);

    if (ADC_DoAutoCalibration(DEMO_ADC_BASE, kADC_CalibrationVrefInternal) != kStatus_Success)
    {
        PRINTF("\r\nCalibration Failed!\r\n");
        return 0;
    }

    PRINTF("\r\nCalibration Success!\r\n");
    ADC_ClearStatusFlags(DEMO_ADC_BASE, kADC_DataReadyInterruptFlag);
    ADC_SetScanChannel(DEMO_ADC_BASE, kADC_ScanChannel0, DEMO_ADC_CHANNEL_SOURCE);
    ADC_EnableInterrupts(DEMO_ADC_BASE, kADC_DataReadyInterruptEnable);
    EnableIRQ(DEMO_ADC_IRQn);
    while (1)
    {
        PRINTF("Please press any key to trigger conversion.\r\n");
        GETCHAR();
        ADC_DoSoftwareTrigger(DEMO_ADC_BASE);
        while (!g_conversionDataReady)
            ;
        PRINTF("\r\nConversion Result: %d\r\n", ADC_GetConversionResult(DEMO_ADC_BASE));
        g_conversionDataReady = false;
        ADC_StopConversion(DEMO_ADC_BASE);
    }
}
