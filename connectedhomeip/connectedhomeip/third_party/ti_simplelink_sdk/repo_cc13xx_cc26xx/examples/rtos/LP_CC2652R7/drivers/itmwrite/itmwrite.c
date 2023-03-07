/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== itmwrite.c ========
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>

/* Driver Header files */
#include <ti/drivers/ITM.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Power.h>

/*Posix header files*/

#include <time.h>
#include <signal.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#define WORK_INTERVAL (1U) /* 1 sec */
#define RESET_FRAME_PORT (12)
#define RESET_FRAME   (0xBBBBBBBB)

uint32_t variableToWatch = 0;

void clockTimeoutFunction(union sigval val)
{
    /* This will create a DWT event on variable write*/
    variableToWatch++;

    /* Toggle the LED for good measure */
    GPIO_toggle(CONFIG_GPIO_LED_0);
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    timer_t clk_s;

    struct sigevent   appSev;
    struct itimerspec its;
    int               retc;
    const char msg[] = "Hello World from ITM";

    GPIO_init();
    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    /* Open the ITM driver, spin here on fail */
    if (false == ITM_open())
    {
        /* Failed to open ITM driver, check for SWO pin conflicts */
        while(1);
    }

    /* ITM host tooling expects a reset frame to reset the parser to a known
     * state. This frame must be the first traffic from ITM. It also means that
     * you need to reset the device after starting the parser to be sure
     * that the reset frame is caught
     */
    ITM_send32Atomic(RESET_FRAME_PORT, RESET_FRAME);

    /* Configure ITM driver to generate timestamps, and watch the variable */
    ITM_enableTimestamps(ITM_TS_DIV_16, false);

    ITM_enableWatchpoint(ITM_EmitDataOnReadWrite, (uintptr_t)&variableToWatch);

    /* Create a timer to notify the task */
    appSev.sigev_notify = SIGEV_SIGNAL;
    appSev.sigev_notify_function = &clockTimeoutFunction;
    appSev.sigev_notify_attributes = NULL;
    retc = timer_create(CLOCK_REALTIME, &appSev, &clk_s);
    if (retc != 0)
    {
        /* Timer creation failed */
        while(1);
    }

    /* Setup period of timer */
    its.it_interval.tv_sec = WORK_INTERVAL;
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = WORK_INTERVAL;
    its.it_value.tv_nsec = 0;
    retc = timer_settime(clk_s, 0, &its, NULL);
    if (retc != 0)
    {
        /* Setting timer period failed */
        timer_delete(clk_s);
        while(1);
    }

    /* Greet the user */
    ITM_sendBufferAtomic(0, msg, sizeof(msg));

    while (1);
}
