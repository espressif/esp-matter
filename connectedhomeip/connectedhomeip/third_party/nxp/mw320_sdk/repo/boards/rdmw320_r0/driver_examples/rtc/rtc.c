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
#include "fsl_rtc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool busyWait;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief ISR for Alarm interrupt
 *
 * This function changes the state of busyWait.
 */
void RTC_IRQHandler(void)
{
    uint32_t status = RTC_GetStatusFlags(RTC);

    if ((status & (uint32_t)kRTC_AlarmFlag) != 0U)
    {
        busyWait = false;
    }
    /* Clear all flags */
    RTC_ClearStatusFlags(RTC, kRTC_AllClearableFlags);

    SDK_ISR_EXIT_BARRIER;
}

/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t sec;
    uint32_t counter;
    uint8_t index;
    rtc_config_t rtcConfig;

    /* Board pin, clock, debug console init */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    CLOCK_EnableXtal32K(kCLOCK_Osc32k_External);
    CLOCK_AttachClk(kXTAL32K_to_RTC);
    /* Init RTC */
    /*
     *    config->ignoreInRunning = false;
     *    config->autoUpdateCntVal = true;
     *    config->stopCntInDebug = true;
     *    config->clkDiv = kRTC_ClockDiv32;
     *    config->cntUppVal = 0xFFFFFFFFU;
     */
    RTC_GetDefaultConfig(&rtcConfig);
    RTC_Init(RTC, &rtcConfig);

    PRINTF("RTC example: reset counter and set up an alarm\r\n");

    /* Enable RTC alarm interrupt */
    RTC_EnableInterrupts(RTC, (uint32_t)kRTC_AlarmInterruptEnable);

    /* Enable at the NVIC */
    EnableIRQ(RTC_IRQn);

    /* Start the RTC time counter */
    RTC_StartTimer(RTC);

    /* This loop will set the RTC alarm */
    while (true)
    {
        busyWait = true;
        index    = 0;
        sec      = 0;

        /* Get alarm time from user */
        PRINTF("Please input the number of second to wait for alarm \r\n");
        PRINTF("The second must be positive value\r\n");
        while (index != 0x0D)
        {
            index = GETCHAR();
            if ((index >= '0') && (index <= '9'))
            {
                PUTCHAR(index);
                sec = sec * 10U + (index - 0x30U);
            }
        }
        PRINTF("\r\n");

        /* Get current counter */
        counter = RTC_GetCounter(RTC);

        /* print default time */
        PRINTF("Current counter: %08x\r\n", counter);

        /* Add alarm seconds to current counter */
        counter += sec * (CLOCK_GetRtcClkFreq() / 32U);

        /* Set alarm counter */
        RTC_SetAlarm(RTC, counter);

        /* Get alarm time */
        counter = RTC_GetAlarm(RTC);

        /* Print alarm time */
        PRINTF("Alarm will occur at: %08x\r\n", counter);

        /* Wait until alarm occurs */
        while (busyWait)
        {
        }

        PRINTF("\r\n Alarm occurs !!!!\r\n");
    }
}
