/*
* Copyright (c) 2014 - 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Clocks v6.0
processor: JN5189
mcu_data: ksdk2_0
processor_version: 0.0.0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
#include "fsl_common.h"
#include "clock_config.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 * Code
 ******************************************************************************/
void BOARD_BootClockVLPR(void)
{
}

void BOARD_BootClockRUN(void)
{
    /* Set PMC FRO selection */
    CLOCK_EnableClock(kCLOCK_Fro32M);
    CLOCK_EnableClock(kCLOCK_Fro48M);
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Rtc);

    /* INMUX and IOCON are used by many apps, enable both INMUX and IOCON clock bits here. */
    CLOCK_AttachClk(kOSC32M_to_FRG_CLK);
    CLOCK_AttachClk(kMAIN_CLK_to_DMI_CLK);

    CLOCK_EnableAPBBridge();
    SYSCON->DMICCLKDIV=0;
    CLOCK_SetClkDiv(kCLOCK_DivClkout, 1, false);

    CLOCK_EnableClock(kCLOCK_Xtal32M);

    /* 32KHz clock */
    CLOCK_EnableClock(CLOCK_32k_source); /* CLOCK_32k_source can be either Fro32k or Xtal32k */
#if gClkUseFro32K
    CLOCK_AttachClk(kFRO32K_to_OSC32K_CLK);
#else
    CLOCK_AttachClk(kXTAL32K_to_OSC32K_CLK);
#endif

    /* Enable 48MHz CPU freq */
    CLOCK_AttachClk(kFRO48M_to_MAIN_CLK);
    /* Enable 32MHZ XTAL to Ctimer */
    CLOCK_AttachClk(kXTAL32M_to_ASYNC_APB);

    /* WWDT clock config (32k oscillator, no division) */
    CLOCK_AttachClk(kOSC32K_to_WDT_CLK);
    CLOCK_SetClkDiv(kCLOCK_DivWdtClk, 1, true);

    /* enable the clocks for the cryto blocks */
    CLOCK_EnableClock(kCLOCK_Aes);

    RESET_PeripheralReset(kUSART0_RST_SHIFT_RSTn);

    /* attach clock for USART0 */
    CLOCK_AttachClk(kOSC32M_to_USART_CLK);
    /* Hold UART in Reset as clock is started */
    RESET_SetPeripheralReset(kUSART0_RST_SHIFT_RSTn);
    CLOCK_EnableClock(kCLOCK_Usart0);
    RESET_ClearPeripheralReset(kUSART0_RST_SHIFT_RSTn);

    SystemCoreClockUpdate();
}

void BOARD_BootClockHSRUN(void)
{
}
