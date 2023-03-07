/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 *  ======== i2secho.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2S.h>

/* Driver configuration */
#include "ti_drivers_config.h"
#include "AudioCodec.h"

#define THREADSTACKSIZE   2048

/* The higher the sampling frequency, the less time we have to process the data, but the higher the sound quality. */
#define SAMPLE_RATE                     44100   /* Supported values: 8kHz, 16kHz, 32kHz and 44.1kHz */
#define INPUT_OPTION                    AudioCodec_MIC_LINE_IN
#define OUTPUT_OPTION                   AudioCodec_SPEAKER_HP

/* The more storage space we have, the more delay we have, but the more time we have to process the data. */
#define NUMBUFS         10      /* Total number of buffers to loop through */
#define BUFSIZE         256     /* I2S buffer size */

/* Semaphore used to indicate that data must be processed */
static sem_t semDataReadyForTreatment;
static sem_t semErrorCallback;

/* Lists containing transactions. Each transaction is in turn in these three lists */
List_List i2sReadList;
List_List treatmentList;
List_List i2sWriteList;

/* Buffers containing the data: written by read-interface, modified by treatment, and read by write-interface */
static uint8_t buf1[BUFSIZE];
static uint8_t buf2[BUFSIZE];
static uint8_t buf3[BUFSIZE];
static uint8_t buf4[BUFSIZE];
static uint8_t buf5[BUFSIZE];
static uint8_t buf6[BUFSIZE];
static uint8_t buf7[BUFSIZE];
static uint8_t buf8[BUFSIZE];
static uint8_t buf9[BUFSIZE];
static uint8_t buf10[BUFSIZE];
static uint8_t* i2sBufList[NUMBUFS] = {buf1, buf2, buf3, buf4, buf5, buf6, buf7, buf8, buf9, buf10};

/* Transactions will successively be part of the i2sReadList, the treatmentList and the i2sWriteList */
I2S_Transaction i2sTransaction1;
I2S_Transaction i2sTransaction2;
I2S_Transaction i2sTransaction3;
I2S_Transaction i2sTransaction4;
I2S_Transaction i2sTransaction5;
I2S_Transaction i2sTransaction6;
I2S_Transaction i2sTransaction7;
I2S_Transaction i2sTransaction8;
I2S_Transaction i2sTransaction9;
I2S_Transaction i2sTransaction10;
static I2S_Transaction *i2sTransactionList[NUMBUFS] = {&i2sTransaction1,  &i2sTransaction2,  &i2sTransaction3,
                                                       &i2sTransaction4,  &i2sTransaction5,  &i2sTransaction6,
                                                       &i2sTransaction7,  &i2sTransaction8,  &i2sTransaction9,
                                                       &i2sTransaction10};

I2S_Handle i2sHandle;

static void errCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
    /* The content of this callback is executed if an I2S error occurs */
    sem_post(&semErrorCallback);
}

static void writeCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
    /*
     * The content of this callback is executed every time a write-transaction is started
     */

    /* We must consider the previous transaction (the current one is not over)  */
    I2S_Transaction *transactionFinished = (I2S_Transaction*)List_prev(&transactionPtr->queueElement);

    if(transactionFinished != NULL){
        /*
         * Remove the finished transaction from the write queue and feed
         * the read queue (we do not need anymore the data of this transaction)
         */
        List_remove(&i2sWriteList, (List_Elem*)transactionFinished);
        List_put(&i2sReadList, (List_Elem*)transactionFinished);

        /*
         * We do not need to queue transaction here:
         * treatment-function takes care of this :)
         */
    }
}

static void readCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
    /*
     * The content of this callback is executed every time a read-transaction
     * is started
     */

    /* We must consider the previous transaction (the current one is not over) */
    I2S_Transaction *transactionFinished = (I2S_Transaction*)List_prev(&transactionPtr->queueElement);

    if(transactionFinished != NULL){

        /* The finished transaction contains data that must be treated */
        List_remove(&i2sReadList, (List_Elem*)transactionFinished);
        List_put(&treatmentList, (List_Elem*)transactionFinished);

        /* Start the treatment of the data */
        sem_post(&semDataReadyForTreatment);

        /*
         * We do not need to queue transaction here:
         * writeCallbackFxn takes care of this :)
         */
    }
}

/*
 *  ======== echoThread ========
 */
void *echoThread(void *arg0)
{
    /* Initialize TLV320AIC3254 Codec on Audio BP */
    uint8_t status = AudioCodec_open();
    if( AudioCodec_STATUS_SUCCESS != status)
    {
        /* Error Initializing codec */
        while(1);
    }

    /* Configure Codec */
    status =  AudioCodec_config(AudioCodec_TI_3254, AudioCodec_16_BIT,
                                SAMPLE_RATE, AudioCodec_STEREO, AudioCodec_SPEAKER_HP,
                                AudioCodec_MIC_LINE_IN);
    if( AudioCodec_STATUS_SUCCESS != status)
    {
        /* Error Initializing codec */
        while(1);
    }

    /* Volume control */
    AudioCodec_speakerVolCtrl(AudioCodec_TI_3254, AudioCodec_SPEAKER_HP, 75);
    AudioCodec_micVolCtrl(AudioCodec_TI_3254, AudioCodec_MIC_ONBOARD, 75);

    /* Prepare the semaphore */
    int retc = sem_init(&semDataReadyForTreatment, 0, 0);
    if (retc == -1) {
        while (1);
    }

    /*
     *  Open the I2S driver
     */
    I2S_Params i2sParams;
    I2S_Params_init(&i2sParams);
    i2sParams.samplingFrequency =  SAMPLE_RATE;
    i2sParams.fixedBufferLength =  BUFSIZE;
    i2sParams.writeCallback     =  writeCallbackFxn ;
    i2sParams.readCallback      =  readCallbackFxn ;
    i2sParams.errorCallback     =  errCallbackFxn;
    i2sHandle = I2S_open(CONFIG_I2S_0, &i2sParams);
    if (i2sHandle == NULL) {
        /* Error Opening the I2S driver */
        while(1);
    }

    /* Initialize the queues and the I2S transactions */
    List_clearList(&i2sReadList);
    List_clearList(&treatmentList);
    List_clearList(&i2sWriteList);

    uint8_t k;
    /* Half the transactions are initially stored in the read queue */
    for(k = 0; k < NUMBUFS/2; k++) {
        I2S_Transaction_init(i2sTransactionList[k]);
        i2sTransactionList[k]->bufPtr  = i2sBufList[k];
        i2sTransactionList[k]->bufSize = BUFSIZE;
        List_put(&i2sReadList, (List_Elem*)i2sTransactionList[k]);
    }

    /* The second half of the transactions is stored in the write queue */
    for(k = NUMBUFS/2; k < NUMBUFS; k++) {
        I2S_Transaction_init(i2sTransactionList[k]);
        i2sTransactionList[k]->bufPtr  = i2sBufList[k];
        i2sTransactionList[k]->bufSize = BUFSIZE;
        List_put(&i2sWriteList, (List_Elem*)i2sTransactionList[k]);
    }

    I2S_setReadQueueHead(i2sHandle,  (I2S_Transaction*) List_head(&i2sReadList));
    I2S_setWriteQueueHead(i2sHandle, (I2S_Transaction*) List_head(&i2sWriteList));

    /* Start I2S streaming */
    I2S_startClocks(i2sHandle);
    I2S_startRead(i2sHandle);
    I2S_startWrite(i2sHandle);

    /* Treatment */
    while(1){

        /* Wait for transaction ready for treatment */
        retc = sem_wait(&semDataReadyForTreatment);
        if (retc == -1) {
            while (1);
        }

        I2S_Transaction* transactionToTreat = (I2S_Transaction*) List_head(&treatmentList);

        if(transactionToTreat != NULL){
            /*
             * Treatment:
             *   The higher the sampling frequency,
             *   the less time we have to process the data
             */
            #if SAMPLE_RATE > 16000
                /* No data processing */
            #elif SAMPLE_RATE > 8000
                /* Data processing */
                int16_t *buf = transactionToTreat->bufPtr;
                /* bufSize is expressed in bytes but samples to consider are 16 bits long */
                uint16_t numOfSamples = transactionToTreat->bufSize / sizeof(uint16_t);
                uint16_t n;
                /* We only modify Left channel's samples () */
                for(n=0; n<numOfSamples-2; n=n+2) {
                    /*
                     * Here we use a very basic filter
                     * (average to reduce noise level on left channel)
                     *
                     * Note: data coming from left channel and right channel
                     * are interleaved
                     */
                    buf[n] = (buf[n] + buf[n+2]) / 2;
                }
            #else
                /* Data processing */
                int16_t *buf = transactionToTreat->bufPtr;
                uint16_t numOfSamples = transactionToTreat->bufSize / sizeof(uint16_t);
                uint16_t n;
                /* We modify both channel's samples */
                for(n=0; n<numOfSamples-2; n=n+1) {
                    /*
                     * Here we use a very basic filter (average to reduce noise level)
                     *
                     * Note: data coming from left channel and right channel
                     * are interleaved
                     */
                    buf[n] = (buf[n] + buf[n+2]) / 2;
                }
            #endif

            /* Place in the write-list the transaction we just treated */
            List_remove(&treatmentList, (List_Elem*)transactionToTreat);
            List_put(&i2sWriteList, (List_Elem*)transactionToTreat);
        }
    }
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
    I2S_init();

/* On CC32XX, do not enable GPIO due to an LED/I2S pin conflict */
#if !(defined(DeviceFamily_CC3200) || defined(DeviceFamily_CC3220) || defined(DeviceFamily_CC3235))
    GPIO_init();

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
#endif

    /* Set priority and stack size attributes */
    pthread_attr_init(&attrs);
    priParam.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    pthread_attr_setschedparam(&attrs, &priParam);

    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }

    /* Create receive thread */
    retc = pthread_create(&thread0, &attrs, echoThread, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    retc = sem_init(&semErrorCallback, 0, 0);
    if (retc == -1) {
        /* sem_init() failed */
        while (1);
    }

    /* Wait forever for an error */
    sem_wait(&semErrorCallback);

    /* Cancel the echo thread (blocks until the thread is closed) */
    pthread_cancel(&thread0);

    /* Close the driver */
    I2S_stopClocks(i2sHandle);
    I2S_close(i2sHandle);

    /* End this task */
    return NULL;
}
