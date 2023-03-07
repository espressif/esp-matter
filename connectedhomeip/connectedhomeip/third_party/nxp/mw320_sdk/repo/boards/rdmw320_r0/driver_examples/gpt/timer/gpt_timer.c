/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_gpt.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The GPT instance/channel used for board */
#define BOARD_GPT_BASEADDR GPT0

/* Interrupt number and interrupt handler for the GPT instance used */
#define BOARD_GPT_IRQ_NUM GPT0_IRQn
#define BOARD_GPT_HANDLER GPT0_IRQHandler

/* Get source clock for GPT driver */
#define GPT_SOURCE_CLOCK (CLOCK_GetGptClkFreq(0))

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool gptIsrFlag           = false;
volatile uint32_t milisecondCounts = 0U;

/*******************************************************************************
 * Code
 ******************************************************************************/
int main(void)
{
    uint32_t cnt;
    uint32_t loop       = 2U;
    uint32_t secondLoop = 1000U;
    const char *signals = "-|";
    gpt_config_t gptInfo;

    /* Board pin, clock, debug console init */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Use 200MHz system clock */
    CLOCK_AttachClk(kSYS_CLK_to_GPT0);

    /* Print a note to terminal */
    PRINTF("\r\nGPT example to simulate a timer\r\n");
    PRINTF("\r\nYou will see a \"-\" or \"|\" in terminal every 1 second:\r\n");

    GPT_GetDefaultConfig(&gptInfo);
    /* Set divide by 48 */
    gptInfo.div      = kGPT_ClkDiv_Divide_8;
    gptInfo.prescale = 5U;
    /* Initialize GPT module */
    GPT_Init(BOARD_GPT_BASEADDR, &gptInfo);

    /* Set timer period. */
    GPT_SetTimerPeriod(BOARD_GPT_BASEADDR, USEC_TO_COUNT(1000U, GPT_SOURCE_CLOCK / 48U));
    /* Reset the counter to make the timer period valid immediately. */
    GPT_ResetTimer(BOARD_GPT_BASEADDR);

    GPT_EnableInterrupts(BOARD_GPT_BASEADDR, kGPT_TimeOverflowInterruptEnable);

    EnableIRQ(BOARD_GPT_IRQ_NUM);

    GPT_StartTimer(BOARD_GPT_BASEADDR);

    cnt = 0;
    while (true)
    {
        if (gptIsrFlag)
        {
            milisecondCounts++;
            gptIsrFlag = false;
            if (milisecondCounts >= secondLoop)
            {
                PRINTF("%c", signals[cnt & 1]);
                cnt++;
                if (cnt >= loop)
                {
                    cnt = 0;
                }
                milisecondCounts = 0U;
            }
        }
        __WFI();
    }
}

void BOARD_GPT_HANDLER(void)
{
    /* Clear interrupt flag.*/
    GPT_ClearStatusFlags(BOARD_GPT_BASEADDR, kGPT_TimeOverflowFlag);
    gptIsrFlag = true;
    SDK_ISR_EXIT_BARRIER;
}
