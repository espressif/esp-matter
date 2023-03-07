/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include "fsl_acomp.h"

#include "fsl_power.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_ACOMP_IRQHANDLER ACOMP_IRQHandler
#define EXAMPLE_ACOMP_BASE       ACOMP


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

int main(void)
{
    acomp_config_t acompConfig;
    acomp_positive_input_config_t posInputConfig;
    acomp_negative_input_config_t negInputConfig;

    posInputConfig.channel     = kACOMP_PosChGPIO42;
    posInputConfig.hysterLevel = kACOMP_Hyster0MV;
    negInputConfig.channel     = kACOMP_NegChVIO_0P50;
    negInputConfig.hysterLevel = kACOMP_Hyster0MV;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* DAC reference voltage selected as intenal voltage, so
     * power up bandgap in advance. */
    POWER_PowerUpBandgap();
    while (!POWER_CheckBandgapReady())
        ;

    PRINTF("\r\nACOMP Basic Example!\r\n");

    /*
     *  config->id = kACOMP_Acomp0;
     *  config->enable = false;
     *  config->warmupTime = kACOMP_WarmUpTime1us;
     *  config->responseMode = kACOMP_SlowResponseMode;
     *  config->inactiveValue = kACOMP_ResultLogicLow;
     *  config->intTrigType = kACOMP_HighLevelTrig;
     *  config->edgeDetectTrigSrc = kACOMP_EdgePulseDis;
     *  config->outPinMode = kACOMP_PinOutDisable;
     *  config->posInput = NULL;
     *  config->negInput = NULL;
     */
    ACOMP_GetDefaultConfig(&acompConfig);
    acompConfig.enable       = true;
    acompConfig.responseMode = kACOMP_FastResponseMode;
    acompConfig.outPinMode   = kACOMP_PinOutSynInverted;
    acompConfig.posInput     = &posInputConfig;
    acompConfig.negInput     = &negInputConfig;
    ACOMP_Init(EXAMPLE_ACOMP_BASE, &acompConfig);

    while (1)
    {
    }
}
