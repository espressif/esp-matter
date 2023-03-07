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
/* The GPT base address/channel used for board */
#define BOARD_GPT_BASEADDR GPT0
#define BOARD_GPT_CHANNEL  kGPT_Chnl_1

/* Interrupt number and interrupt handler for the GPT base address used */
#define GPT_INTERRUPT_NUMBER GPT0_IRQn
#define GPT_ONE_SHOT_HANDLER GPT0_IRQHandler

/* Interrupt to enable and flag to read */
#define GPT_CHANNEL_INTERRUPT_ENABLE kGPT_Chnl1InterruptEnable
#define GPT_CHANNEL_FLAG             kGPT_Chnl1Flag

/* Get source clock for GPT driver */
#define GPT_SOURCE_CLOCK (CLOCK_GetGptClkFreq(0))
/* Define one-shot example type:
   0: one-shot pulse
   1: one-shot edge
*/
#define GPT_ONE_SHOT_EDGE_EXAMPLE (1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool gptIsrFlag = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
void GPT_ONE_SHOT_HANDLER(void)
{
    uint32_t flag = GPT_GetStatusFlags(BOARD_GPT_BASEADDR);

    if ((flag & ((uint32_t)kGPT_Chnl0Flag << BOARD_GPT_CHANNEL)) != 0U)
    {
        gptIsrFlag = true;
    }

    /* Clear interrupt flag.*/
    GPT_ClearStatusFlags(BOARD_GPT_BASEADDR, flag);

    SDK_ISR_EXIT_BARRIER;
}

void APP_SetupOneshot(void)
{
#if GPT_ONE_SHOT_EDGE_EXAMPLE
    /* Toggle the edge with 0.5s delay. */
    GPT_SetupOneshotEdge(BOARD_GPT_BASEADDR, BOARD_GPT_CHANNEL, kGPT_ChnlPol_Low,
                         MSEC_TO_COUNT(500U, GPT_SOURCE_CLOCK));
#else
    /* Generate pulse with 0.5s delay and 0.5s duty. */
    GPT_SetupOneshotPulse(BOARD_GPT_BASEADDR, BOARD_GPT_CHANNEL, kGPT_ChnlPol_Low,
                          MSEC_TO_COUNT(500U, GPT_SOURCE_CLOCK), MSEC_TO_COUNT(500U, GPT_SOURCE_CLOCK));
#endif
}
/*!
 * @brief Main function
 */
int main(void)
{
    gpt_config_t gptInfo;

    /* Board pin, clock, debug console init */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Use 200MHz system clock */
    CLOCK_AttachClk(kSYS_CLK_to_GPT0);

    /* Print a note to terminal */
    PRINTF("\r\nGPT example to output square wave with 1s period.\r\n");
    PRINTF("You will see a blinking LED if an LED is connected to the GPT pin\r\n");
    PRINTF("If no LED is connected to the GPT pin, then probe the signal using an oscilloscope\r\n");

    GPT_GetDefaultConfig(&gptInfo);
    /* Initialize GPT module */
    GPT_Init(BOARD_GPT_BASEADDR, &gptInfo);

    GPT_StartTimer(BOARD_GPT_BASEADDR);

    /* Enable channel interrupt flag.*/
    GPT_EnableInterrupts(BOARD_GPT_BASEADDR, GPT_CHANNEL_INTERRUPT_ENABLE);

    /* Enable at the NVIC */
    EnableIRQ(GPT_INTERRUPT_NUMBER);

    APP_SetupOneshot();

    while (1)
    {
        /* Use interrupt to toggle the edge */
        if (true == gptIsrFlag)
        {
            gptIsrFlag = false;
            APP_SetupOneshot();
        }
        else
        {
            __WFI();
        }
    }
}
