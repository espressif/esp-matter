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
#define BOARD_GPT_BASEADDR GPT0

/* GPT channel used for input capture */
#define BOARD_GPT_INPUT_CAPTURE_CHANNEL kGPT_Chnl_1

/* Interrupt number and interrupt handler for the GPT base address used */
#define GPT_INTERRUPT_NUMBER      GPT0_IRQn
#define GPT_INPUT_CAPTURE_HANDLER GPT0_IRQHandler

/* Interrupt to enable and flag to read */
#define GPT_CHANNEL_INTERRUPT_ENABLE kGPT_Chnl1InterruptEnable
#define GPT_CHANNEL_FLAG             kGPT_Chnl1Flag

/* Get source clock for GPT driver */
#define GPT_SOURCE_CLOCK (CLOCK_GetGptClkFreq(0))

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
void GPT_INPUT_CAPTURE_HANDLER(void)
{
    if ((GPT_GetStatusFlags(BOARD_GPT_BASEADDR) & GPT_CHANNEL_FLAG) == GPT_CHANNEL_FLAG)
    {
        /* Clear interrupt flag.*/
        GPT_ClearStatusFlags(BOARD_GPT_BASEADDR, GPT_CHANNEL_FLAG);
    }
    gptIsrFlag = true;
    __DSB();
}

/*!
 * @brief Main function
 */
int main(void)
{
    gpt_config_t gptInfo;
    uint32_t captureVal;

    /* Board pin, clock, debug console init */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Use 200MHz system clock */
    CLOCK_AttachClk(kSYS_CLK_to_GPT0);

    /* Print a note to terminal */
    PRINTF("\r\nGPT input capture example\r\n");
    PRINTF("\r\nOnce the input signal is received the input capture value is printed\r\n");

    GPT_GetDefaultConfig(&gptInfo);
    /* Set up input filter */
    gptInfo.icFilter = kGPT_InputCapFilter_7Cycles;
    /* Initialize GPT module */
    GPT_Init(BOARD_GPT_BASEADDR, &gptInfo);

    /* Setup dual-edge capture on a GPT channel pair */
    GPT_SetupInputCapture(BOARD_GPT_BASEADDR, BOARD_GPT_INPUT_CAPTURE_CHANNEL, kGPT_FallingEdge);

    /* Enable channel interrupt when the second edge is detected */
    GPT_EnableInterrupts(BOARD_GPT_BASEADDR, GPT_CHANNEL_INTERRUPT_ENABLE);

    /* Enable at the NVIC */
    EnableIRQ(GPT_INTERRUPT_NUMBER);

    GPT_StartTimer(BOARD_GPT_BASEADDR);

    while (gptIsrFlag != true)
    {
    }

    captureVal = GPT_GetCapturedValue(BOARD_GPT_BASEADDR, BOARD_GPT_INPUT_CAPTURE_CHANNEL);
    PRINTF("\r\nCapture counter value is 0x%x\r\n", captureVal);

    while (1)
    {
    }
}
