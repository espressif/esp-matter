/*
 * Copyright (c) 2016-2021, Texas Instruments Incorporated
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
 *  ======== pdmstream.c ========
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
#include <ti/drivers/PIN.h>
#include <ti/display/Display.h>
#include <ti/drivers/apps/LED.h>
#include <ti/drivers/apps/Button.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

/* PDM Driver */
#include <ti/drivers/pdm/PDMCC26XX.h>
#include <ti/drivers/pdm/PDMCC26XX_util.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include <stdint.h>

Semaphore_Struct semStruct;
Semaphore_Handle saSem;

#define MIC_POWER       IOID_18
#define MIC_POWER_ON    1
#define MIC_POWER_OFF   0
#define ADI_PIN         IOID_20
#define BCLK_PIN        IOID_19

/* PDM objects, one for PDM driver, one for PDM/I2S helper file */
PDMCC26XX_Object pdmCC26XXObject;
PDMCC26XX_I2S_Object pdmCC26XXI2SObject;

/* PDM driver hardware attributes */
const PDMCC26XX_HWAttrs pdmCC26XXHWAttrs = {
    .micPower = MIC_POWER,
    .taskPriority = 1,
};

/* PDM configuration structure */
const PDMCC26XX_Config PDMCC26XX_config[] = {
    {
        .object = &pdmCC26XXObject,
        .hwAttrs = &pdmCC26XXHWAttrs
    },
    {NULL, NULL}
};

/* PDM_I2S hardware attributes */
const PDMCC26XX_I2S_HWAttrs pdmC26XXI2SHWAttrs = {
    .baseAddr       = I2S0_BASE,
    .intNum         = INT_I2S_IRQ,
    .powerMngrId    = PowerCC26XX_PERIPH_I2S,
    .intPriority    = ~0,
    .mclkPin        = PIN_UNASSIGNED,
    .bclkPin        = BCLK_PIN,
    .wclkPin        = PIN_UNASSIGNED,
    .ad0Pin         = ADI_PIN,
};

/* PDM_I2S configuration structure */
const PDMCC26XX_I2S_Config PDMCC26XX_I2S_config[] = {
    {
        .object  = &pdmCC26XXI2SObject,
        .hwAttrs = &pdmC26XXI2SHWAttrs
    },
    {NULL, NULL}
};

/* store the events for this application */
static uint16_t events = 0x0000;
#define SA_DEBOUNCE_COUNT_IN_MS  25

#define SA_PCM_START             0x0001
#define SA_PCM_BLOCK_READY       0x0002
#define SA_PCM_ERROR             0x0004
#define SA_PCM_STOP              0x0008
#define SA_BUTTON_EVENT          0x0010

/* Pin driver handles */
static LED_Handle led1Handle;
static LED_Handle led2Handle;

static Button_Handle btn1Handle;
static Button_Handle btn2Handle;

static int totalFrameCount = 0;
static int currentFrameCount = 0;
static int sessionCount = 0;

/* Audio Protocol Define */
#define SA_PCM_BLOCK_SIZE_IN_SAMPLES       32
#define SA_PCM_BUFFER_NODE_SIZE_IN_BLOCKS   6

#define AUDIO_BUF_COMPRESSED_SIZE   ((SA_PCM_BLOCK_SIZE_IN_SAMPLES*\
                                      SA_PCM_BUFFER_NODE_SIZE_IN_BLOCKS*\
                                     sizeof(uint16_t))/ PCM_COMPRESSION_RATE)
#define AUDIO_BUF_UNCOMPRESSED_SIZE (SA_PCM_BLOCK_SIZE_IN_SAMPLES*\
                                     SA_PCM_BUFFER_NODE_SIZE_IN_BLOCKS*\
                                     sizeof(uint16_t))

/* SA PDM drivers related */
static void SA_PDMCC26XX_callbackFxn(PDMCC26XX_Handle handle,
        PDMCC26XX_StreamNotification *streamNotification);
static void SA_processPDMData(Display_Handle display);
static int SA_envelopeDetector(int16_t *pPCMsamples, uint16_t numOfSamples);
static void *SA_audioMalloc(uint_least16_t size);
static void SA_audioFree(void *msg, size_t size);

static PDMCC26XX_Handle pdmHandle = NULL;
static PDMCC26XX_Params pdmParams;

/* SA audio streaming States */
typedef enum
{
  SA_AUDIO_IDLE,
  SA_AUDIO_STARTING,
  SA_AUDIO_STREAMING,
  SA_AUDIO_STOPPING,
  SA_AUDIO_ERROR
} saAudioState_t;
static saAudioState_t saAudioState = SA_AUDIO_IDLE;

void buttonCallbackFxn(Button_Handle handle, Button_EventMask events);

/*
 *  ======== main ========
 */
void *mainThread(void *arg0) {
    Semaphore_Params    semParams;
    Display_Handle      display;
    Display_Params      displayParams;
    Button_Params       btnParams;

    /* Call driver init functions */
    Display_init();
    PDMCC26XX_init((PDMCC26XX_Handle) &(PDMCC26XX_config));

    PDMCC26XX_Params_init(&pdmParams);
    pdmParams.callbackFxn = SA_PDMCC26XX_callbackFxn;
    pdmParams.decimationFilter = NULL;
    pdmParams.defaultFilterGain = PDMCC26XX_GAIN_12;
    pdmParams.micPowerActiveHigh = true;
    pdmParams.applyCompression = false;
    pdmParams.startupDelayWithClockInSamples = 512;
    pdmParams.retBufSizeInBytes = AUDIO_BUF_UNCOMPRESSED_SIZE + PCM_METADATA_SIZE;
    pdmParams.mallocFxn = (PDMCC26XX_MallocFxn) SA_audioMalloc;
    pdmParams.freeFxn = (PDMCC26XX_FreeFxn) SA_audioFree;
    pdmParams.pcmSampleRate = PDMCC26XX_PCM_SAMPLE_RATE_16K;


    /* Open LED and Button pins - passing NULL uses the default parameters */
    led1Handle = LED_open(CONFIG_LED_0, NULL);
    led2Handle = LED_open(CONFIG_LED_1, NULL);
    if (!led1Handle || !led2Handle)
    {
        System_abort("Error initializing board LED pins\n");
    }
    /* Second argument is brightness, ignored for non-dimmable LEDs */
    LED_setOn(led1Handle, 0);

    Button_Params_init(&btnParams);

    /* Set the buttons' callback function */
    btnParams.buttonCallback = buttonCallbackFxn;

    /* Only subscribe to debounced press-release events */
    btnParams.buttonEventMask = Button_EV_PRESSED;

    /* Set the minimum press duration */
    btnParams.debounceDuration = SA_DEBOUNCE_COUNT_IN_MS;

    btn1Handle = Button_open(CONFIG_BUTTON_0, &btnParams);
    btn2Handle = Button_open(CONFIG_BUTTON_1, &btnParams);
    if(!btn1Handle || !btn2Handle)
    {
        System_abort("Error initializing button pins\n");
    }

    /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&semStruct, 1, &semParams);

    /* Obtain instance handle */
    saSem = Semaphore_handle(&semStruct);

    Display_Params_init(&displayParams);
    display = Display_open(Display_Type_UART, &displayParams);
    if (!display)
    {
        System_abort("Could not open Display driver!\n");
    }

    Display_clear(display);
    Display_printf(display, 1, 1, "PDM Stream. Waiting for BUTTON0 press.");

    /* This example has logging and many other debug capabilities enabled */
    System_printf("This example does not attempt to minimize code or data "
                  "footprint\n");
    System_flush();

    System_printf("Starting the PDM Stream example\nSystem provider is set to "
                  "SysMin. Halt the target to view any SysMin contents in "
                  "ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Loop forever */
    while (1) {

        /*
         * Do not pend on semaphore until all events have been processed
         */
        if (events == 0)
        {
            if (Semaphore_getCount(saSem) == 0) {
                System_printf("Sem blocked in task1\n");
            }

            /* Get access to resource */
            Semaphore_pend(saSem, BIOS_WAIT_FOREVER);
        }

        /*
         * Process the PDM stream start event
         */
        if (events & SA_PCM_START) {
            if (pdmHandle == NULL)
            {
                /* Open PDM driver */
                pdmHandle = PDMCC26XX_open(&pdmParams);
            }
            if (pdmHandle && (saAudioState == SA_AUDIO_IDLE)) {
                saAudioState = SA_AUDIO_STARTING;
                currentFrameCount = 0;
                sessionCount++;
                Display_printf(display, 2, 1, "Session: %d", sessionCount);
                /* Stream immediately if we simply dump over UART. */
                PDMCC26XX_startStream(pdmHandle);
                LED_setOff(led1Handle);
                saAudioState = SA_AUDIO_STREAMING;
            }
            /* Mark event as processed */
            events &= ~SA_PCM_START;
        }

        /*
         * Process event to stop PDM stream
         */
        if (events & SA_PCM_STOP)
        {
            if ((saAudioState != SA_AUDIO_IDLE) && (pdmHandle))
            {
                Display_printf(display, 1, 1, "Stopping");
                saAudioState = SA_AUDIO_STOPPING;
                /* Stop PDM stream */
                PDMCC26XX_stopStream(pdmHandle);
                /* In case no more callbacks will be made, attempt to flush remaining data now. */
                events |= SA_PCM_BLOCK_READY;
            }
            events &= ~SA_PCM_STOP;
        }

        /*
         * Process PDM block ready event. This event is set in the
         * PDM driver callback function.
         */
        if (events & SA_PCM_BLOCK_READY) {
            SA_processPDMData(display);
            /* Mark event as processed */
            events &= ~SA_PCM_BLOCK_READY;
        }

        /*
         * Process the PCM error event.
         */
        if (events & SA_PCM_ERROR) {
            /* Stop stream if not already stopped */
            if ((saAudioState == SA_AUDIO_STREAMING) ||
                (saAudioState == SA_AUDIO_STARTING)) {

                events |= SA_PCM_STOP;
            }
            events &= ~SA_PCM_ERROR;
        }
    }
}

/**
 *  @fn          SA_processPDMData
 *
 * @brief        Processes the received audio packetd from PDM driver and measures
 *               the amplitude of the audio stream.
 *
 * @param[in]    None.
 *
 * @param[out]   None.
 *
 * @return       None.
 *
 */
static void SA_processPDMData(Display_Handle display) {
#define MAX_VOLUME_IND        15
#define TOTAL_COUNT_STR_MAX    15
#define TOTAL_COUNT_BEG_IDX     7
    static int maxEnvSinceLastReport = 0;
    int i = 0;
    static char volumeIndicator[MAX_VOLUME_IND + 1] = "-";
    PDMCC26XX_BufferRequest bufferRequest;

    if ( (saAudioState == SA_AUDIO_STREAMING) ||
         (saAudioState == SA_AUDIO_STARTING) ||
         (saAudioState == SA_AUDIO_STOPPING) ) {
        // Block ready, read it out and send it,
        // if we're not already sending
        while (PDMCC26XX_requestBuffer(pdmHandle, &bufferRequest)) {
            if (bufferRequest.status == PDMCC26XX_STREAM_BLOCK_READY) {
                int current;
                if (!pdmParams.applyCompression) {
                    current = SA_envelopeDetector((int16_t *)bufferRequest.buffer->pBuffer, AUDIO_BUF_UNCOMPRESSED_SIZE/sizeof(int16_t));
                    if (current > maxEnvSinceLastReport)
                    {
                        maxEnvSinceLastReport = current;
                    }
                }
                totalFrameCount++;
                currentFrameCount++;
                if ((bufferRequest.buffer->metaData.seqNum & 0x0000000F) == 0x00)
                {
                    Display_printf(display, 3, 1, "Frames %d", currentFrameCount);
                    if (totalFrameCount < 1000)
                    {
                        Display_printf(display, 4, 1, "Total  %d", totalFrameCount);
                    }
                    else
                    {
                        Display_printf(display, 4, 1, "Total  %dk", totalFrameCount/1000);
                    }
                    Display_printf(display, 6, 1, "Volume %d", maxEnvSinceLastReport);
                    volumeIndicator[0] = '-';
                    for (i = 1; i < ((maxEnvSinceLastReport > MAX_VOLUME_IND) ? MAX_VOLUME_IND : maxEnvSinceLastReport); i++)
                    {
                        volumeIndicator[i] = '-';
                    }
                    volumeIndicator[i] = '\0'; // Terminate string
                    Display_printf(display, 7, 1, volumeIndicator);
                    maxEnvSinceLastReport = 0;
                    if ((bufferRequest.buffer->metaData.seqNum & 0x0000001F) == 0x00)
                    {
                        LED_setOn(led2Handle, 0);
                    }
                    else if ((bufferRequest.buffer->metaData.seqNum & 0x0000001F) == 0x10)
                    {
                        LED_setOff(led2Handle);
                    }
                }
                if (pdmParams.applyCompression) {
                    SA_audioFree(bufferRequest.buffer, PCM_METADATA_SIZE + AUDIO_BUF_COMPRESSED_SIZE);
                } else {
                    SA_audioFree(bufferRequest.buffer, PCM_METADATA_SIZE + AUDIO_BUF_UNCOMPRESSED_SIZE);
                }
            }
        }
        /*
         * We close the driver after flushing samples, after stopping the stream.
         */
        if (saAudioState == SA_AUDIO_STOPPING) {
            saAudioState = SA_AUDIO_IDLE;

            /* Close PDM driver */
            PDMCC26XX_close(pdmHandle);
            pdmHandle = NULL;
            LED_setOff(led1Handle);
            LED_setOff(led2Handle);
        }
    }
    else if (saAudioState == SA_AUDIO_IDLE)
    {
        // We may have received a callback for data after stopping the stream. Simply flush it.
        while (PDMCC26XX_requestBuffer(pdmHandle, &bufferRequest)) {
            if (pdmParams.applyCompression) {
                SA_audioFree(bufferRequest.buffer, PCM_METADATA_SIZE + AUDIO_BUF_COMPRESSED_SIZE);
            } else {
                SA_audioFree(bufferRequest.buffer, PCM_METADATA_SIZE + AUDIO_BUF_UNCOMPRESSED_SIZE);
            }
        }
    }
}

/**
 *  @fn          SA_PDMCC26XX_callbackFxn
 *
 *  @brief       Application callback function to handle notifications from PDM
 *               driver.
 *
 *  @param[in]   handle - PDM driver handle
 *               pStreamNotification - voice data stream
 *  @param[out]  None
 *
 *  @return  None.
 */
static int callbackCount = 0;
static void SA_PDMCC26XX_callbackFxn(PDMCC26XX_Handle handle, PDMCC26XX_StreamNotification *pStreamNotification) {
    if (pStreamNotification->status == PDMCC26XX_STREAM_BLOCK_READY) {
        events |= SA_PCM_BLOCK_READY;
        callbackCount++;
    } else if (pStreamNotification->status == PDMCC26XX_STREAM_BLOCK_READY_BUT_PDM_OVERFLOW) {
        events |= SA_PCM_BLOCK_READY;
        events |= SA_PCM_ERROR;
    } else if (pStreamNotification->status == PDMCC26XX_STREAM_STOPPING) {
        events |= SA_PCM_BLOCK_READY;
        events |= SA_PCM_ERROR;
    } else {
        events |= SA_PCM_ERROR;
    }

    Semaphore_post(saSem);
}

static int mallocCount = 0;
static void *SA_audioMalloc(uint_least16_t size)
{
    Error_Block eb;
    Error_init(&eb);
    if (size > 64)
    {
        mallocCount++;
    }
    return Memory_alloc(NULL, size, 0, &eb);
}

static int freeCount = 0;
static void SA_audioFree(void *msg, size_t size)
{
    if (size > 64)
    {
        freeCount++;
    }
    Memory_free(NULL, msg, size);
}

static int SA_envelopeDetector(int16_t *pPCMsamples, uint16_t numOfSamples)
{
#define FILTER_COEFF_B        3
#define FILTER_COEFF_A        1
    static int filteredValue = 0;
    static int slowFilteredValue = 0;
    int i, newSample;
//    // Scale value
//    filteredValue = filteredValue * (FILTER_COEFF_B + FILTER_COEFF_A);
    for (i = 0; i < numOfSamples; i++)
    {
        newSample = (pPCMsamples[i] > 0) ? pPCMsamples[i] : -pPCMsamples[i];
        // Weigh 3/4 on new value
        filteredValue += (newSample + (newSample << 1)) - (filteredValue * FILTER_COEFF_A);
        // Normalize
        filteredValue = (filteredValue)/(FILTER_COEFF_B + FILTER_COEFF_A);

        slowFilteredValue +=  (filteredValue + (filteredValue << 1)) - (slowFilteredValue * FILTER_COEFF_A);
        // Normalize
        slowFilteredValue = (slowFilteredValue)/(FILTER_COEFF_B + FILTER_COEFF_A);
    }

#define VOL_OFFSET    5
    for (i = 31; i > VOL_OFFSET; i--)
    {
        if ((slowFilteredValue >> i) & 0x00000001)
        {
            break;
        }
    }
    i = (i < 0) ? 0 : (i - VOL_OFFSET);

    return i;
}

/*
 *  ======== buttonCallbackFxn ========
 *  Pin interrupt Callback function board buttons configured in the pinTable.
 *  If Board_PIN_LED3 and Board_PIN_LED4 are defined, then we'll add them to the PIN
 *  callback function.
 */
void buttonCallbackFxn(Button_Handle handle, Button_EventMask mask)
{
    if (handle == btn1Handle)
    {
        events |= SA_PCM_START;
    }
    else if (handle == btn2Handle)
    {
        events |= SA_PCM_STOP;
    }

    if (Semaphore_getCount(saSem) == 0)
    {
        Semaphore_post(saSem);
    }
}
