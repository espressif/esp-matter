/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_power.h"
#include "fsl_debug_console.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "lpm.h"
#include "power_mode_switch.h"
#include "fsl_uart.h"

/*******************************************************************************
 * Struct Definitions
 ******************************************************************************/


/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint32_t s_wakeupTimeout;           /* Wakeup timeout. (Unit: Millisecond) */
static app_wakeup_source_t s_wakeupSource; /* Wakeup source. */

/*******************************************************************************
 * Function Code
 ******************************************************************************/
void BOARD_UART_IRQ_HANDLER(void)
{
    /* If new data arrived. */
    if (((uint16_t)kUART_RxDataReadyInterruptFlag & UART_GetStatusFlags(BOARD_DEBUG_UART)) != 0U)
    {
        (void)UART_ReadByte(BOARD_DEBUG_UART);
    }

    PRINTF("Woken up by UART\r\n");

    UART_DisableInterrupts(BOARD_DEBUG_UART, (uint8_t)kUART_RxDataReadyInterruptEnable);
    DisableIRQ(BOARD_UART_IRQ);

    SDK_ISR_EXIT_BARRIER;
}

/* Get input from user about wakeup timeout. */
static uint8_t APP_GetWakeupTimeout(void)
{
    uint8_t timeout;

    while (1)
    {
        PRINTF("Select the wake up timeout in seconds.\r\n");
        PRINTF("The allowed range is 1s ~ 9s.\r\n");
        PRINTF("Eg. enter 5 to wake up in 5 seconds.\r\n");
        PRINTF("\r\nWaiting for input timeout value...\r\n\r\n");

        timeout = GETCHAR();
        PRINTF("%c\r\n", timeout);
        if ((timeout > '0') && (timeout <= '9'))
        {
            return timeout - '0';
        }
        PRINTF("Wrong value!\r\n");
    }
}

/* Get wakeup source by user input. */
static app_wakeup_source_t APP_GetWakeupSource(uint32_t mode)
{
    uint8_t ch;

    while (true)
    {
        PRINTF("Select the wake up source:\r\n");
        PRINTF("Press T for RTC.\r\n");
        if (mode != 1U)
        {
            PRINTF("Press 0 for Pin0 wakeup.\r\n");
            PRINTF("Press 1 for both Pin0/Pin1 wakeup.\r\n");
        }
        else
        {
            PRINTF("Press U for UART wakeup.\r\n");
        }

        PRINTF("\r\nWaiting for key press..\r\n\r\n");

        ch = GETCHAR();
        PRINTF("%c\r\n", ch);

        if ((ch >= 'a') && (ch <= 'z'))
        {
            ch -= 'a' - 'A';
        }

        if (ch == 'T')
        {
            return kAPP_WakeupSourceRtc;
        }
        else if (ch == 'U' && mode == 1U)
        {
            return kAPP_WakeupSourceUart;
        }
        else if (ch == '0' && mode != 1U)
        {
            return kAPP_WakeupSourcePin0;
        }
        else if (ch == '1' && mode != 1U)
        {
            return kAPP_WakeupSourcePin1;
        }
        else
        {
            PRINTF("Wrong value!\r\n");
        }
    }
}

void APP_WakeupHandler(IRQn_Type irq)
{
    switch (irq)
    {
        case EXTPIN0_IRQn:
            PRINTF("Woken up by PIN0\r\n");
            break;
        case EXTPIN1_IRQn:
            PRINTF("Woken up by PIN1\r\n");
            break;
        case RTC_IRQn:
            PRINTF("Woken up by RTC\r\n");
            break;
        default:
            PRINTF("Unexpected wakeup by %d\r\n", irq);
            break;
    }
}

static uint32_t APP_GetIoExcludeMask(void)
{
    uint8_t ch;
    uint32_t excludeIo;

    PRINTF("Input the IO domain bitmap unaffected by PM2:\r\n");
    PRINTF("  - e.g. 5 means VDDIO0 and VDDIO2 will not be powered off during PM2\r\n");
    PRINTF("  -      A means VDDIO1 and VDDIO3 will not be powered off during PM2\r\n");

    do
    {
        ch = GETCHAR();
        PRINTF("%c\r\n", ch);
        if ((ch >= 'a') && (ch <= 'f'))
        {
            ch -= 'a' - 'A';
        }

        if ((ch >= 'A') && (ch <= 'F'))
        {
            excludeIo = 10UL + ch - 'A';
            break;
        }
        else if ((ch >= '0') && (ch <= '9'))
        {
            excludeIo = ch - '0';
            break;
        }
        else
        {
            PRINTF("Wrong value, input again\r\n");
        }
    } while (true);

    return excludeIo;
}

/* Get wakeup timeout and wakeup source. */
static void APP_GetWakeupConfig(uint32_t mode)
{
    /* Get wakeup source by user input. */
    if (mode == 4U)
    {
        s_wakeupSource = kAPP_WakeupSourcePin1;
    }
    else
    {
        s_wakeupSource = APP_GetWakeupSource(mode);
    }

    if (kAPP_WakeupSourceRtc == s_wakeupSource)
    {
        /* Wakeup source is RTC, user should input wakeup timeout value. */
        s_wakeupTimeout = APP_GetWakeupTimeout();
        PRINTF("Will wakeup in %d seconds.\r\n", s_wakeupTimeout);
        s_wakeupTimeout *= 1000U; /* Convert to ms */
    }
    else
    {
        s_wakeupTimeout = 0xFFFFFFFFU;
        if (kAPP_WakeupSourceUart == s_wakeupSource)
        {
            PRINTF("Input any key in terminal to wake up.\r\n");
        }
        else if (kAPP_WakeupSourcePin0 == s_wakeupSource)
        {
            PRINTF("Press wakeup pin0 to wake up.\r\n");
        }
        else
        {
            PRINTF("Press wakeup pin0 or pin1 to wake up.\r\n");
        }
    }
}

static void APP_SetWakeupConfig(void)
{
    lpm_config_t config = {
        /* System PM2/PM3 less than 50 ms will be skipped. */
        .threshold = 50U,
        /* SFLL config and  RC32M setup takes approx 14 ms. */
        .latency = 15U,
        .enableWakeupPin0 =
            ((s_wakeupSource == kAPP_WakeupSourcePin0) || (s_wakeupSource == kAPP_WakeupSourcePin1)) ? true : false,
        .enableWakeupPin1 = (s_wakeupSource == kAPP_WakeupSourcePin1) ? true : false,
        .handler          = APP_WakeupHandler,
    };

    LPM_Init(&config);

    if (s_wakeupSource == kAPP_WakeupSourceUart)
    {
        /* Enable RX interrupt. */
        UART_EnableInterrupts(BOARD_DEBUG_UART, (uint8_t)kUART_RxDataReadyInterruptEnable);
        EnableIRQ(BOARD_UART_IRQ);
    }
}

static void APP_ClearWakeupConfig(void)
{
    LPM_Deinit();
    /* Disable RX interrupt. */
    UART_DisableInterrupts(BOARD_DEBUG_UART, (uint8_t)kUART_RxDataReadyInterruptEnable);
    DisableIRQ(BOARD_UART_IRQ);
}

/*! @brief Main function */
int main(void)
{
    uint32_t resetSrc;
    uint32_t freq = 0U;
    uint32_t i;
    uint32_t pm, excludeIo;
    uint8_t ch;
    uint32_t irqMask;

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    CLOCK_EnableXtal32K(kCLOCK_Osc32k_External);
    CLOCK_AttachClk(kXTAL32K_to_RTC);

    resetSrc = POWER_GetResetCause();
    PRINTF("\r\nMCU wakeup source 0x%x...\r\n", resetSrc);

    POWER_ClearResetCause(resetSrc);

    for (;;)
    {
        freq = CLOCK_GetCoreBusFreq();
        PRINTF("\r\n####################  Power Mode Switch ####################\n\r\n");
        PRINTF("    Build Time: %s--%s \r\n", __DATE__, __TIME__);
        PRINTF("    Core Clock: %dHz \r\n", freq);
        PRINTF("\r\nSelect the desired operation \n\r\n");
        for (i = 0U; i <= 4U; i++)
        {
            PRINTF("Press %d for enter: PM%d\r\n", i, i);
        }
        PRINTF("\r\nWaiting for power mode select..\r\n\r\n");

        /* Wait for user response */
        ch = GETCHAR();
        PRINTF("%c\r\n", ch);
        if (ch >= '0' && ch <= '4')
        {
            pm = ch - '0';
        }
        else
        {
            PRINTF("No such power mode\r\n");
            continue;
        }
        if (pm == 2U)
        {
            excludeIo = APP_GetIoExcludeMask();
        }
        else
        {
            excludeIo = 0U;
        }

        if (pm >= 1U)
        {
            APP_GetWakeupConfig(pm);
            APP_SetWakeupConfig();
        }

        LPM_SetPowerMode(pm, excludeIo);
        irqMask = DisableGlobalIRQ();
        pm      = LPM_WaitForInterrupt(s_wakeupTimeout, true);
        if (pm == 3U)
        {
            /* Perihperal state lost, need reinitialize in exit from PM3 */
            BOARD_InitBootPins();
            BOARD_InitDebugConsole();
        }
        EnableGlobalIRQ(irqMask);

        PRINTF("Exit from power mode %d\r\n", pm);
        APP_ClearWakeupConfig();
    }
}
