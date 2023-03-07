/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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
 *  ======== dacoutputbuffer.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* POSIX Header files */
#include <pthread.h>

/* Driver Header files */
#include <ti/drivers/DAC.h>
#include <ti/display/Display.h>
#include <ti/drivers/timer/GPTimerCC26XX.h>
#include <ti/drivers/ADC.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#define THREADSTACKSIZE   1024

#define TIMEBASE_RATE     7680
#define SYSTEM_FREQ       48000000
#define BUFFER_SIZE       128
#define REPETITIONS       20
#define OUTPUT_REF        1500000

static Display_Handle     display;
static DAC_Handle         dacHandle0;
static DAC_Handle         dacHandle1;

/* Global counter to keep track of the element in the data buffer that is to be output. */
static uint32_t sineTableCounter = 0;

static volatile uint8_t repCounter = 0;

/* Buffer containing the data that will be output with the DAC. In this case a sine-wave. */
static uint8_t sineTable128[BUFFER_SIZE] = {
                                            0x80,0x86,0x8c,0x92,0x98,0x9e,0xa5,0xaa,
                                            0xb0,0xb6,0xbc,0xc1,0xc6,0xcb,0xd0,0xd5,
                                            0xda,0xde,0xe2,0xe6,0xea,0xed,0xf0,0xf3,
                                            0xf5,0xf8,0xfa,0xfb,0xfd,0xfe,0xfe,0xff,
                                            0xff,0xff,0xfe,0xfe,0xfd,0xfb,0xfa,0xf8,
                                            0xf5,0xf3,0xf0,0xed,0xea,0xe6,0xe2,0xde,
                                            0xda,0xd5,0xd0,0xcb,0xc6,0xc1,0xbc,0xb6,
                                            0xb0,0xaa,0xa5,0x9e,0x98,0x92,0x8c,0x86,
                                            0x80,0x79,0x73,0x6d,0x67,0x61,0x5a,0x55,
                                            0x4f,0x49,0x43,0x3e,0x39,0x34,0x2f,0x2a,
                                            0x25,0x21,0x1d,0x19,0x15,0x12,0x0f,0x0c,
                                            0x0a,0x07,0x05,0x04,0x02,0x01,0x01,0x00,
                                            0x00,0x00,0x01,0x01,0x02,0x04,0x05,0x07,
                                            0x0a,0x0c,0x0f,0x12,0x15,0x19,0x1d,0x21,
                                            0x25,0x2a,0x2f,0x34,0x39,0x3e,0x43,0x49,
                                            0x4f,0x55,0x5a,0x61,0x67,0x6d,0x73,0x79};

/*
 *  ======== timerCallback ========
 *  Uses the gpTimer to trigger an update of the DAC code.
 *
 */
void timerCallback(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask)
{
    /* Timer interrupt callback */
    if (sineTableCounter < (BUFFER_SIZE - 1) ) {
        DAC_setCode(dacHandle0, sineTable128[sineTableCounter++]);
    }
    else {
        DAC_setCode(dacHandle0, sineTable128[sineTableCounter]);
        sineTableCounter = 0;
        repCounter++;
    }
}

/*
 *  ======== threadFxn0 ========
 *  This example shows how to generate a 60 Hz sine wave using the 8-bit reference DAC
 *  and a 16-bit gptimer to provide a time base.
 *
 *  The frequency of the sine wave will depend on the time base and the size of the
 *  data buffer representing the sine wave.
 *
 *  Frequency = (time base rate / buffer size)
 *  For this particular example:  Frequency  = (7680 / 128) = 60 Hz
 *
 *  Furthermore, a second DAC handle is used to set an output reference voltage of
 *  1.5 [V], which is measured by a 12-bit ADC. The result of the conversion is
 *  visualized with the Display driver.
 *
 */
void *threadFxn0(void *arg0)
{
    GPTimerCC26XX_Handle   gptimerHandle;
    ADC_Handle             adcHandle;
    DAC_Params             dacParams;
    GPTimerCC26XX_Params   gptimerParams;
    ADC_Params             adcParams;

    GPTimerCC26XX_Value    loadVal;
    uint16_t               adcValue;
    uint32_t               adcValueMicroVolt;
    int_fast16_t           resConvert;

    /* Open the first DAC handle */
    DAC_Params_init(&dacParams);
    dacHandle0 = DAC_open(CONFIG_DAC_0, &dacParams);
    if (dacHandle0 == NULL) {
        /* Error opening the DAC driver */
        Display_printf(display, 0, 0, "Error initializing CONFIG_DAC_0\n");
        while (1);
    }

    /* Open the second DAC handle */
    dacHandle1 = DAC_open(CONFIG_DAC_1, &dacParams);
    if (dacHandle1 == NULL) {
        /* Error opening the DAC driver */
        Display_printf(display, 0, 0, "Error initializing CONFIG_DAC_1\n");
        while (1);
    }

    /* Open the ADC */
    ADC_Params_init(&adcParams);
    adcHandle = ADC_open(CONFIG_ADC_0, &adcParams);
    if (adcHandle == NULL) {
        /* Error opening the DAC driver */
        Display_printf(display, 0, 0, "Error initializing CONFIG_ADC_0\n");
        while (1);
    }

    /* Open the GPTimer driver */
    GPTimerCC26XX_Params_init(&gptimerParams);
    gptimerParams.width          = GPT_CONFIG_16BIT;
    gptimerParams.mode           = GPT_MODE_PERIODIC;
    gptimerParams.direction      = GPTimerCC26XX_DIRECTION_UP;
    /* Disable timer debug stall mode since it's only necessary for debugging. */
    gptimerParams.debugStallMode = GPTimerCC26XX_DEBUG_STALL_OFF;
    gptimerHandle = GPTimerCC26XX_open(CONFIG_GPTIMER_0, &gptimerParams);
    if (gptimerHandle == NULL) {
        /* Error opening the GPTimer driver */
        Display_printf(display, 0, 0, "Error initializing CONFIG_GPTIMER_0\n");
        while (1);
    }

    /* Setup timer load value considering desired sample rate */
    loadVal = (uint32_t)(SYSTEM_FREQ / TIMEBASE_RATE) - 1;
    GPTimerCC26XX_setLoadValue(gptimerHandle, loadVal);
    GPTimerCC26XX_registerInterrupt(gptimerHandle, timerCallback, GPT_INT_TIMEOUT);


    /* Enable DAC for the first handle */
    DAC_enable(dacHandle0);

    /* Start the timer */
    GPTimerCC26XX_start(gptimerHandle);

    /* Output the data buffer a defined number of times and then stop the timer. */
    while (1) {
        if (repCounter == REPETITIONS) {
            GPTimerCC26XX_stop(gptimerHandle);
            break;
        }
    }

    /* Disable the DAC for the first handle. */
    DAC_disable(dacHandle0);

    /* Enable DAC for the second handle and set an output reference voltage of 1500000 [uV]. */
    DAC_enable(dacHandle1);
    DAC_setVoltage(dacHandle1, 1500000);

    /* Use 12-bit ADC to measure the voltage set by the DAC. */
    resConvert = ADC_convert(adcHandle, &adcValue);
        if (resConvert == ADC_STATUS_SUCCESS) {
            adcValueMicroVolt = ADC_convertRawToMicroVolts(adcHandle, adcValue);
            Display_printf(display, 0, 0, "The measured voltage with the ADC is: %d [uV]\n", adcValueMicroVolt);
        }
        else {
            Display_printf(display, 0, 0, "The ADC conversion failed.\n");
        }

    /* Disable the DAC for the second handle. */
    DAC_disable(dacHandle1);

    /* Close DAC handles, ADC handle and Timer handle. */
    DAC_close(dacHandle0);
    DAC_close(dacHandle1);
    GPTimerCC26XX_close(gptimerHandle);
    ADC_close(adcHandle);

    return (NULL);
}


/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    pthread_t           thread0;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;
    int                 detachState;

    /* Call driver init functions */
    Display_init();
    DAC_init();
    ADC_init();

    /* Open the display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        /* Failed to open display driver */
        while (1);
    }

    /* Create application threads */
    pthread_attr_init(&attrs);

    detachState = PTHREAD_CREATE_DETACHED;
    /* Set priority and stack size attributes */
    retc = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }

    /* Create threadFxn0 thread */
    priParam.sched_priority = 1;
    pthread_attr_setschedparam(&attrs, &priParam);

    retc = pthread_create(&thread0, &attrs, threadFxn0, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    return (NULL);
}
