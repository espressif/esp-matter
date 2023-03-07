/*
 * Copyright 2020 NXP
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
#define DEMO_DAC_BASE    DAC
#define DEMO_DAC_CHANNEL kDAC_ChannelB

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static uint16_t DEMO_GetInputCode(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

int main(void)
{
    dac_config_t dacConfig;
    dac_channel_config_t channelConfig;
    uint16_t inputCode = 0U;

    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /* DAC reference voltage selected as intenal voltage, so
     * power up bandgap in advance. */
    POWER_PowerUpBandgap();
    while (!POWER_CheckBandgapReady())
        ;

    PRINTF("\r\nDAC Normal Driver Example!\r\n");
    PRINTF("\r\nOutput = 0.18V + (1.42V * input_code / 1023)\r\n");

    /*
     * Get DAC module default configuration.
     *  config->conversionRate = kDAC_ConversionRate62P5KHZ;
     *  config->refSource = kDAC_ReferenceInternalVoltageSource;
     *  config->rangeSelect = kDAC_RangeLarge;
     */
    DAC_GetDefaultConfig(&dacConfig);
    dacConfig.conversionRate = kDAC_ConversionRate500KHZ;
    DAC_Init(DEMO_DAC_BASE, &dacConfig);

    channelConfig.enableConversion = true;
    channelConfig.enableDMA        = false;
    channelConfig.enableTrigger    = false;
    channelConfig.outMode          = kDAC_ChannelOutputPAD;
    channelConfig.timingMode       = kDAC_NonTimingCorrelated;
    channelConfig.waveType         = kDAC_WaveNormal;

    DAC_SetChannelConfig(DEMO_DAC_BASE, DEMO_DAC_CHANNEL, &channelConfig);

    while (1)
    {
        inputCode = DEMO_GetInputCode();
        DAC_SetChannelData(DEMO_DAC_BASE, DEMO_DAC_CHANNEL, inputCode);
    }
}

/*!
 * @brief Get input code from terminal.
 */
static uint16_t DEMO_GetInputCode(void)
{
    uint16_t ret = 0U;
    char ch      = 0U;
    PRINTF("\r\nPlease input the value(Ranges from 0 to  1023) to be converted.\r\n");

    while (ch != 0x0DU)
    {
        ch = GETCHAR();
        if ((ch >= '0') && (ch <= '9'))
        {
            PUTCHAR(ch);
            ret = ret * 10 + (ch - '0');
        }
    }

    return ret;
}
