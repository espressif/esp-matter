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
#define GPT_LED_HANDLER      GPT0_IRQHandler

/* Interrupt to enable and flag to read */
#define GPT_CHANNEL_INTERRUPT_ENABLE kGPT_Chnl1InterruptEnable
#define GPT_CHANNEL_FLAG             kGPT_Chnl1Flag

/* Get source clock for GPT driver */
#define GPT_SOURCE_CLOCK (CLOCK_GetGptClkFreq(0))

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief delay a while.
 */
void delay(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool gptIsrFlag          = false;
volatile bool brightnessUp        = true; /* Indicate LED is brighter or dimmer */
volatile uint8_t updatedDutycycle = 10U;

/*******************************************************************************
 * Code
 ******************************************************************************/
void delay(void)
{
    volatile uint32_t i = 0U;
    for (i = 0U; i < 80000U; ++i)
    {
        __NOP(); /* delay */
    }
}

void GPT_LED_HANDLER(void)
{
    gptIsrFlag = true;

    if (brightnessUp)
    {
        /* Increase duty cycle until it reach limited value, don't want to go upto 100% duty cycle
         * as channel interrupt will not be set for 100%
         */
        if (++updatedDutycycle >= 99U)
        {
            updatedDutycycle = 99U;
            brightnessUp     = false;
        }
    }
    else
    {
        /* Decrease duty cycle until it reach limited value */
        if (--updatedDutycycle == 1U)
        {
            brightnessUp = true;
        }
    }

    /* Clear interrupt flag.*/
    GPT_ClearStatusFlags(BOARD_GPT_BASEADDR, GPT_GetStatusFlags(BOARD_GPT_BASEADDR));

    SDK_ISR_EXIT_BARRIER;
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
    PRINTF("\r\nGPT example to output center-aligned PWM signal\r\n");
    PRINTF("You will see a change in LED brightness if an LED is connected to the GPT pin\r\n");
    PRINTF("If no LED is connected to the GPT pin, then probe the signal using an oscilloscope\r\n");

    GPT_GetDefaultConfig(&gptInfo);
    /* Initialize GPT module */
    GPT_Init(BOARD_GPT_BASEADDR, &gptInfo);

    GPT_StartTimer(BOARD_GPT_BASEADDR);

    GPT_SetupPwm(BOARD_GPT_BASEADDR, BOARD_GPT_CHANNEL, kGPT_ChnlPol_Low, updatedDutycycle, kGPT_CenterAlignedPwm,
                 24000U, GPT_SOURCE_CLOCK);

    /* Enable channel interrupt flag.*/
    GPT_EnableInterrupts(BOARD_GPT_BASEADDR, GPT_CHANNEL_INTERRUPT_ENABLE);

    /* Enable at the NVIC */
    EnableIRQ(GPT_INTERRUPT_NUMBER);

    while (1)
    {
        /* Use interrupt to update the PWM dutycycle */
        if (true == gptIsrFlag)
        {
            /* Disable interrupt to retain current dutycycle for a few seconds */
            GPT_DisableInterrupts(BOARD_GPT_BASEADDR, GPT_CHANNEL_INTERRUPT_ENABLE);

            gptIsrFlag = false;

            /* Update PWM duty cycle */
            GPT_UpdatePwmDutycycle(BOARD_GPT_BASEADDR, BOARD_GPT_CHANNEL, updatedDutycycle);

            /* Delay to view the updated PWM dutycycle */
            delay();

            /* Enable interrupt flag to update PWM dutycycle */
            GPT_EnableInterrupts(BOARD_GPT_BASEADDR, GPT_CHANNEL_INTERRUPT_ENABLE);
        }
    }
}
