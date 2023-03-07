/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "fsl_wdt.h"

#include "fsl_device_registers.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_WDT WDT
/* Make APB clock frequency 50M to generate about 1.345s timeout interval */
#define EXAMPLE_WDT_COUNT      kWDT_TimeoutVal2ToThePowerOf26
#define EXAMPLE_WDT_IRQ        WDT_IRQn
#define EXAMPLE_WDT_IRQHandler WDT_IRQHandler

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static volatile bool g_wdtFlag     = false;
static volatile bool g_cleanStatus = false;
/*******************************************************************************
 * Code
 ******************************************************************************/
void EXAMPLE_WDT_IRQHandler(void)
{
    g_wdtFlag = true;
    if (g_cleanStatus)
    {
        WDT_ClearInterruptStatus(EXAMPLE_WDT);
    }
    else
    {
        WDT_Refresh(EXAMPLE_WDT);
    }
}

void Delay(uint32_t count_ms)
{
    uint32_t count = (SystemCoreClock / 1000) * count_ms;
    while (count--)
    {
        __NOP();
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    wdt_config_t config;

    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    PRINTF("\r\n WDT Driver example\r\n");

    WDT_GetDefaultConfig(&config);

    config.timeoutValue = EXAMPLE_WDT_COUNT;
    config.timeoutMode  = kWDT_ModeTimeoutInterrupt;
    config.enableWDT    = true;

    WDT_Init(EXAMPLE_WDT, &config);
    EnableIRQ(EXAMPLE_WDT_IRQ);

    PRINTF("\r\n WDT will enter timeout interrupt, clear status and wait for next interrupt.\r\n");

    /* Wait watch dog interrupt happens. */
    g_cleanStatus = true;
    while (!g_wdtFlag)
    {
        __NOP();
    }
    g_wdtFlag = false;

    PRINTF("\r\n WDT interrupt again, this time refresh the counter.\r\n");

    g_cleanStatus = false;
    /* Wait watchdog interrupt happens, at this time refresh the counter. */
    while (!g_wdtFlag)
    {
        __NOP();
    }
    g_wdtFlag = false;
    PRINTF("\r\n WDT refreshed, disable interrupt and refresh periodically to avoid reset.\r\n");

    WDT_DisableInterrupt(EXAMPLE_WDT);

    uint32_t refreshCount = 100U;
    while ((!g_wdtFlag) && (refreshCount > 0))
    {
        Delay(10);
        WDT_Refresh(EXAMPLE_WDT);
        refreshCount--;
    }

    PRINTF("\r\n Stop refreshing, WDOG will reset.\r\n");

    while (1)
    {
    }
}
