/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
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
 *  ======== adcBufContinuousSampling.c ========
 */
#include <stdint.h>
#include <stdio.h>
#include <mqueue.h>

/* Driver Header files */
#include <ti/drivers/ADCBuf.h>

/* Display Header files */
#include <ti/display/Display.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#define ADCSAMPLESIZE    (50)
#define MAX_QUEUED_ADC_CONVERSIONS (4)
#define QUEUED_ADC_MESSAGE_SIZE (sizeof(uint32_t) * ADCSAMPLESIZE)

uint16_t sampleBufferOne[ADCSAMPLESIZE];
uint16_t sampleBufferTwo[ADCSAMPLESIZE];
uint32_t microVoltBuffer[ADCSAMPLESIZE];
uint32_t outputBuffer[ADCSAMPLESIZE];
uint32_t buffersCompletedCounter = 0;

/* Display Driver Handle */
Display_Handle displayHandle;

/* Used to pass ADC data between callback and main task */
static mqd_t queueReceive;
static mqd_t queueSend;

/*
 * This function is called whenever an ADC buffer is full.
 * The content of the buffer is then converted into human-readable format and
 * sent to the main thread.
 */
void adcBufCallback(ADCBuf_Handle handle, ADCBuf_Conversion *conversion,
    void *completedADCBuffer, uint32_t completedChannel, int_fast16_t status)
{
    /* Adjust raw ADC values and convert them to microvolts */
    ADCBuf_adjustRawValues(handle, completedADCBuffer, ADCSAMPLESIZE,
        completedChannel);
    ADCBuf_convertAdjustedToMicroVolts(handle, completedChannel,
        completedADCBuffer, microVoltBuffer, ADCSAMPLESIZE);

    /* Send ADC data to main thread using message queue */
    mq_send(queueSend, (char *) microVoltBuffer, QUEUED_ADC_MESSAGE_SIZE, 0);
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    Display_Params displayParams;
    ADCBuf_Handle adcBuf;
    ADCBuf_Params adcBufParams;
    ADCBuf_Conversion continuousConversion;
    struct mq_attr attr;
    uint32_t average;
    uint_fast16_t i = 0;

    /* Create RTOS Queue */
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_QUEUED_ADC_CONVERSIONS;
    attr.mq_msgsize = QUEUED_ADC_MESSAGE_SIZE;
    attr.mq_curmsgs = 0;

    queueReceive = mq_open("ADCBuf", O_RDWR | O_CREAT, 0664, &attr);
    queueSend = mq_open("ADCBuf", O_RDWR | O_CREAT | O_NONBLOCK, 0664,
                          &attr);
    if ((queueReceive == (mqd_t)-1) || (queueSend == (mqd_t)-1)) {
        /* Failed to open message queue */
        while (1);
    }

    /* Call driver init functions */
    ADCBuf_init();
    Display_init();

    /* Configure & open Display driver */
    Display_Params_init(&displayParams);
    displayParams.lineClearMode = DISPLAY_CLEAR_BOTH;
    displayHandle = Display_open(Display_Type_UART, &displayParams);
    if (displayHandle == NULL) {
        Display_printf(displayHandle, 0, 0, "Error creating displayHandle\n");
        while (1);
    }

    Display_printf(displayHandle, 0, 0,
                    "Starting the ADCBufContinuous example");

    /* Set up an ADCBuf peripheral in ADCBuf_RECURRENCE_MODE_CONTINUOUS */
    ADCBuf_Params_init(&adcBufParams);
    adcBufParams.callbackFxn = adcBufCallback;
    adcBufParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_CONTINUOUS;
    adcBufParams.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
    adcBufParams.samplingFrequency = 200;
    adcBuf = ADCBuf_open(CONFIG_ADCBUF_0, &adcBufParams);

    /* Configure the conversion struct */
    continuousConversion.arg = NULL;
    continuousConversion.adcChannel = CONFIG_ADCBUF_0_CHANNEL_0;
    continuousConversion.sampleBuffer = sampleBufferOne;
    continuousConversion.sampleBufferTwo = sampleBufferTwo;
    continuousConversion.samplesRequestedCount = ADCSAMPLESIZE;

    if (adcBuf == NULL){
        /* ADCBuf failed to open. */
        while(1);
    }

    /* Start converting. */
    if (ADCBuf_convert(adcBuf, &continuousConversion, 1) !=
        ADCBuf_STATUS_SUCCESS) {
        /* Did not start conversion process correctly. */
        while(1);
    }

    /*
     * Wait for the message queue to receive ADC data from the callback
     * function. When data is received, calculate the average and print to UART
     */
    while(1) {
        if (mq_receive(queueReceive, (char *) outputBuffer,
                       QUEUED_ADC_MESSAGE_SIZE, NULL) == -1)
        {
            Display_printf(displayHandle, 0, 0, "Error receiving ADC message");
            while(1);
        }

        Display_printf(displayHandle, 0, 0, "Buffer %u finished:",
                    (unsigned int)buffersCompletedCounter++);

        /* Calculate average ADC data value in uV */
        average = 0;
        for (i = 0; i < ADCSAMPLESIZE; i++) {
            average += outputBuffer[i];
        }
        average = average/ADCSAMPLESIZE;

        /* Print average ADCBuf value */
        Display_printf(displayHandle, 0, 0, "  Average: %u uV", average);
    }
}
