/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */
#include <xdc/std.h>

#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Assert.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/drivers/pdm/Codec1.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/pdm/PDMCC26XX_util.h>
#include <ti/drivers/pdm/PDMCC26XX.h>

#include <string.h>


/*********************************************************************
 * LOCAL DEFINES
 */

/*! Use both word clock phases to get continuous PDM stream */
#define PDM_NUM_OF_CHANNELS             2

/*! PDM block size in number of 16bit samples. Each PDM sample actually consumes
 *  64 bits, assuming ankHz sampling rate. */
#define PDM_BLOCK_SIZE_IN_SAMPLES_8K    128

/*! PDM block size in number of 16bit samples. Each PDM sample actually consumes
 *  64 bits, assuming ankHz sampling rate. */
#define PDM_BLOCK_SIZE_IN_SAMPLES_16K   64

/*! Number of GPIOs used. */
#define PDM_NUMBER_OF_PINS              1       // Only one pin to control microphone power

/*! Number of compressed bytes produced by each PDM->PCM conversion and compression */
#define PDMCC26XX_COMPR_ITER_OUTPUT_SIZE 16

/*! Number of uncompressed bytes produced by each PDM->PCM conversion and memcpy */
#define PDMCC26XX_CPY_ITER_OUTPUT_SIZE   64

/*! This value defines how many samples are discarded at minimum each time the PDM stream starts.
 *  The first few samples that are processed by the driver will not be representative of their actual value
 *  as the decimation filter has not sufficiently updated its internal state. A value < 8 is not reccomended, as the
 *  distortion of the signal is greatest there. */
#define PDM_DECIMATION_STARTUP_DELAY_IN_SAMPLES 32

/*! PDM sample size. Although there are 32 bits per sample at 16kHz we need to
 *  set it to 16 as part of the PDMCC26XX module configuration. */
#define PDM_SAMPLE_SIZE                 16 // 24



/* Events handled in the PDM task */
/*! PDM event set in the callback from the PDMCC26XX driver every time a block
 *  is ready for PDM2PCM conversion. */
#define PDM_EVT_BLK_RDY                 Event_Id_00

/*! PDM event set to kick off stream from PDM thread context.
 *   */
#define PDM_EVT_START                   Event_Id_01

/*! PDM event set in PDMCC26XX_stopStream that synchronously stops the I2S stream.
    This synchronous event is needed to make sure we properly deallocate all memory
    passed between the I2S module and the PDM driver */
#define PDM_EVT_STOP                    Event_Id_02

/*! PDM event set in the callback from the PDMCC26XX driver in case an error
 *  occurs.  */
#define PDM_EVT_BLK_ERROR               Event_Id_03

/*! PDM event set in the callback from the PDMCC26XX driver when the application
 *  calls ::PDMCC26XX_close(). */
#define PDM_EVT_CLOSE                   Event_Id_04

/*! PDM event that causes the driver to drop the current PCMBuffer and throw samples
 * because the I2S module could not get an empty buffer.
 */
#define PDM_EVT_I2S_DATA_DROPPED        Event_Id_05



/* PDM rollback vector bit masks. Each bit mask corresponds to an action that PDMCC26XX_rollbackDriverInitialisation() commits */
/*! Reverses the driver being set to open in the object  */
#define PDM_ROLLBACK_OPEN                   1 << 0
/*! Reverses the allocation of the decimationState  */
#define PDM_ROLLBACK_DECIMATION_STATE       1 << 1
/*! Reverses the allocation of the activePcmBuffer  */
#define PDM_ROLLBACK_ACTIVE_PCM_BUFFER      1 << 2
/*! Reverses the opening of the I2S driver  */
#define PDM_ROLLBACK_I2S_DRIVER             1 << 3
/*! Reverses the allocation of the PDM pin in the PIN driver  */
#define PDM_ROLLBACK_PIN                    1 << 4


/*********************************************************************
 * TYPEDEFS
*/

/*! Struct that contains a PCM queue element and a pointer to the data buffer it is responsible for */
typedef struct {
    Queue_Elem          queueElement;       /*!< Queue element */
    PDMCC26XX_pcmBuffer *pBufferPCM;        /*!< Pointer to a ::PDMCC26XX_pcmBuffer */
} PDMCC26XX_queueNodePCM;

/*! PDM sample type. Prepared for future support of 24 sample size.
 *  @note Internal use only */
#if (defined PDM_SAMPLE_SIZE) && (PDM_SAMPLE_SIZE == 24)
typedef __packed struct {
    uint8_t pdmSampleLsb;
    uint16_t pdmSampleMsb;
} pdmSample;
#elif (defined PDM_SAMPLE_SIZE) && (PDM_SAMPLE_SIZE == 16)
typedef uint16_t pdmSample;
#else
#error Unsupported PDM samples size (16 or 24)
#endif
/*! PCM sample type. @note Internal use only */
#if (defined PCM_SAMPLE_SIZE) && (PCM_SAMPLE_SIZE == 16)
typedef int16_t pcmSample;
#else
#error Unsupported PCM samples size (16)
#endif


/*********************************************************************
 * LOCAL VARIABLES
 */

/*! PDMCC26XX_Config struct defined in the board file */
extern const PDMCC26XX_Config PDMCC26XX_config[];

/*! PDMCC26XX_I2S_Config struct defined in the board file */
extern const PDMCC26XX_I2S_Config PDMCC26XX_I2S_config[];

/* PDM Task Configuration */
Task_Struct pdmTask;
Char pdmTaskStack[PDM_TASK_STACK_SIZE];

/* Keep track of compression variables */
PDMCC26XX_metaData metaDataForNextFrame = {0};

Queue_Struct pcmMsgReady;
Queue_Handle pcmMsgReadyQueue;

Event_Struct sPDMEvents;
Event_Handle pdmEvents;

/* This semaphore synchronises calls to PDMCC26XX_startStream, PDMCC26XX_stopStream, PDMCC26XX_open, and PDMCC26XX_close
 * between the application and the driver */
static SemaphoreP_Struct synchronisationSemaphore;

static PDMCC26XX_Handle pdmHandle = NULL;

PDMCC26XX_queueNodePCM *activePcmBuffer;

PDMCC26XX_StreamNotification streamNotification = {
    .arg = NULL,
    .status = PDMCC26XX_STREAM_IDLE
};

volatile uint32_t droppedPdmBlockCount = 0;

static PDMCC26XX_I2S_StreamNotification pdmStream;
static PDMCC26XX_I2S_Handle i2sHandle;

/* Gain coefficient of the first filter stage. We cannot make it part of the object
 * as the default filter coefficients must be able to encode a pointer to the variable
 * in the first entry and the location of the object is not known within this compilation
 * unit at compile time.
 */
static int32_t gainCoefficient;

/*********************************************************************
 * CONSTANTS
 */

/* Default filter coefficients. Includes configurable gain setting in first
 * filter stage. In order to allow for dynamic gain control, the first coefficient
 * may be a pointer to a RAM (!) address. The value at that address is then used
 * as initial gain coeffiencient. The default filter points to a coeffiecient
 * configured via PDMCC26XX_Params.defaultFilterGain.
 */
static const int32_t PDMCC26XX_aBqCoeffs[] = {
 //--v--  Adjust overall gain by changing this coefficient
    (int32_t)(&gainCoefficient),
             0, -1024, -1356,   342,     // DC-notch, halfband LP filter (@32 kHz)
    200,   789,   934,  -994,   508,
    538,   381,   944,  -519,   722,
    732,   124,   987,  -386,   886,
    763,    11,  1014,  -386,   886,
    0, // Terminate first filter
    // Insert optional second filter here (@16 kHz). Some examples:
    //1147,-1516,   522, -1699,   708,    // +5dB peak filter (F0=500 Hz, BW=3 octaves)
    //1313, -565,    -6,  -725,   281,    // +5dB peak filter (F0=2.5 kHz, BW=2 octaves)
    //1335,  532,   -66,   694,   225,    // +5 dB peak filter (F0=5.5 kHz, BW=1 octave)
    0, // Terminate second filter
};

const PDMCC26XX_Params PDMCC26XX_defaultParams = {
    .callbackFxn                    = NULL,
    .decimationFilter               = PDMCC26XX_aBqCoeffs,
    .decimationFilterStateSize      = sizeof(uint32_t) * (6 + 5 * 2),
    .defaultFilterGain              = PDMCC26XX_GAIN_6,
    .micPowerActiveHigh             = true,
    .applyCompression               = true,
    .startupDelayWithClockInSamples = 0,
    .retBufSizeInBytes              = 260,
    .mallocFxn                      = NULL,
    .freeFxn                        = NULL,
    .pcmSampleRate                  = PDMCC26XX_PCM_SAMPLE_RATE_16K,
    .pdmBufferQueueDepth            = MINIMUM_PDM_BUFFER_QUEUE_DEPTH,
    .custom                         = (uintptr_t) NULL
};

/* Lookup table of pdm block sizes. Use PDMCC26XX_PcmSampleRate as index */
const uint8_t pdmBlockSizeLut[] = {
    PDM_BLOCK_SIZE_IN_SAMPLES_16K,
    PDM_BLOCK_SIZE_IN_SAMPLES_8K,
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void PDMCC26XX_taskFxn(UArg a0, UArg a1);
static bool PDMCC26XX_initIO(PDMCC26XX_Handle handle);
static void PDMCC26XX_setPcmBufferReady(PDMCC26XX_Handle handle, PDMCC26XX_queueNodePCM *pcmBuffer, PDMCC26XX_I2S_BufferRequest *bufReq);
static PDMCC26XX_queueNodePCM *PDMCC26XX_getNewPcmBuffer(PDMCC26XX_Handle handle);
static void PDMCC26XX_i2sCallbackFxn(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_StreamNotification *notification);
static void PDMCC26XX_rollbackDriverInitialisation(PDMCC26XX_Handle handle, uint32_t rollbackVector);
static inline uint32_t PDMCC26XX_maxUInt32(uint32_t a, uint32_t b);

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern uint8_t tic1_EncodeBuff(uint8_t *dst, int16_t *src, int16_t srcSize, int8_t *si, int16_t *pv);
extern bool pdm2pcm16k(const void *pIn, uint32_t *pState, const int32_t *pBqCoeffs, int16_t *pOut);
extern bool pdm2pcm8k(const void *pIn, uint32_t *pState, const int32_t *pBqCoeffs, int16_t *pOut);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void             PDMCC26XX_init(PDMCC26XX_Handle handle);
PDMCC26XX_Handle PDMCC26XX_open(PDMCC26XX_Params *params);
void             PDMCC26XX_close(PDMCC26XX_Handle handle);
bool             PDMCC26XX_startStream(PDMCC26XX_Handle handle);
bool             PDMCC26XX_stopStream(PDMCC26XX_Handle handle);
bool             PDMCC26XX_requestBuffer(PDMCC26XX_Handle handle, PDMCC26XX_BufferRequest *bufferRequest);

/*
 * ======== PDMCC26XX_init ========
 * @pre    Function assumes that it is called *once* on startup before BIOS init
 *
 * @param handle handle to the PDM object
 */
void PDMCC26XX_init(PDMCC26XX_Handle handle) {
    /* Locals */
    PDMCC26XX_Object         *object;
    PDMCC26XX_HWAttrs const  *pdmCC26XXHWAttrs;
    Task_Params taskParams;

    /* Set local reference to return to callers */
    pdmHandle = handle;

    /* Get object for this handle */
    object = handle->object;
    pdmCC26XXHWAttrs = handle->hwAttrs;

    /* Mark the objects as available */
    object->isOpen = false;

    /* Then initialize I2S driver */
    i2sHandle = (PDMCC26XX_I2S_Handle)&(PDMCC26XX_I2S_config);
    PDMCC26XX_I2S_init(i2sHandle);

    /* Configure task */
    Task_Params_init(&taskParams);
    taskParams.stack = pdmTaskStack;
    taskParams.stackSize = PDM_TASK_STACK_SIZE;
    taskParams.priority = pdmCC26XXHWAttrs->taskPriority;

    /* Construct task */
    Task_construct(&pdmTask, PDMCC26XX_taskFxn, &taskParams, NULL);
}

/*
 * ======== PDMCC26XX_open ========
 * @brief   Function for opening the PDM driver on CC26XX devices
 *
 * @param   params Parameters needed to configure the driver
 *
 * @return  handle to the opened PDM driver
 */
PDMCC26XX_Handle PDMCC26XX_open(PDMCC26XX_Params *params) {
    PDMCC26XX_Handle        handle;
    PDMCC26XX_Object        *object;
    PDMCC26XX_I2S_Params    i2sParams = {0};

    Assert_isTrue(params, NULL);
    Assert_isTrue(params->callbackFxn, NULL);
    Assert_isTrue(params->mallocFxn, NULL);
    Assert_isTrue(params->freeFxn, NULL);
    Assert_isTrue((params->retBufSizeInBytes - PCM_METADATA_SIZE) >= (params->applyCompression ? PDMCC26XX_COMPR_ITER_OUTPUT_SIZE : PDMCC26XX_CPY_ITER_OUTPUT_SIZE), NULL);
    Assert_isTrue(params->pdmBufferQueueDepth >= MINIMUM_PDM_BUFFER_QUEUE_DEPTH, NULL);

    /* Get handle for this driver instance */
    handle = pdmHandle;
    /* Get the pointer to the object */
    object = handle->object;

    /* Disable preemption while checking if the PDM is open. */
    uint32_t key = HwiP_disable();
    /* Check if the PDM is open already with the base addr. */
    if (object->isOpen == true) {
        HwiP_restore(key);

        return (NULL);
    }

    /* Mark the handle as being used */
    object->isOpen = true;
    HwiP_restore(key);

    /* Initialize the PDM object */
    object->callbackFxn                     = params->callbackFxn;
    object->micPowerActiveHigh              = params->micPowerActiveHigh;
    object->applyCompression                = params->applyCompression;
    object->startupDelayWithClockInSamples  = params->startupDelayWithClockInSamples;
    object->streamStarted                   = false;
    object->streamNotification              = &streamNotification;
    object->mallocFxn                       = params->mallocFxn;
    object->freeFxn                         = params->freeFxn;
    object->pdm2pcmFxn                      = params->pcmSampleRate == PDMCC26XX_PCM_SAMPLE_RATE_16K ? pdm2pcm16k : pdm2pcm8k;
    object->retBufSizeInBytes               = params->retBufSizeInBytes;
    object->pcmBufferSizeInBytes            = params->retBufSizeInBytes - PCM_METADATA_SIZE;
    object->decimationFilter                = params->decimationFilter ? params->decimationFilter : PDMCC26XX_aBqCoeffs;
    gainCoefficient                         = params->defaultFilterGain;

    object->decimationFilterStateSize       = params->decimationFilterStateSize;
    object->decimationFilterState           = object->mallocFxn(params->decimationFilterStateSize);
    if(!object->decimationFilterState){
        /* We didn't manage to allocate enough space on the heap for the decimationFilterState.
         * Exit with return value NULL to prevent the driver running with decimationFilterState == NULL
         */
        PDMCC26XX_rollbackDriverInitialisation(handle, PDM_ROLLBACK_OPEN);
    }

    /* Get first buffer for PCM data so that we don't get NULL
     * pointer exception.
     */
    activePcmBuffer = PDMCC26XX_getNewPcmBuffer(pdmHandle);
    if (!activePcmBuffer) {
        /* We didn't manage to allocate enough space on the heap for the activePcmBuffer.
         * Exit with return value NULL to prevent the driver running with activePcmBuffer == NULL
         */
        PDMCC26XX_rollbackDriverInitialisation(handle,
                                               PDM_ROLLBACK_OPEN | PDM_ROLLBACK_DECIMATION_STATE);

        return (NULL);
    }

    i2sParams.requestMode            = PDMCC26XX_I2S_CALLBACK_MODE;
    i2sParams.requestTimeout         = BIOS_WAIT_FOREVER;
    i2sParams.callbackFxn            = PDMCC26XX_i2sCallbackFxn;
    i2sParams.mallocFxn              = params->mallocFxn;
    i2sParams.freeFxn                = params->freeFxn;
    i2sParams.blockCount             = params->pdmBufferQueueDepth;
    i2sParams.currentStream          = &pdmStream;
    i2sParams.blockSizeInSamples     = pdmBlockSizeLut[params->pcmSampleRate];

    /* Init IOs */
    if (!PDMCC26XX_initIO(handle)){
        /* We couldn't allocate the necessary pins though the PIN driver. */
        PDMCC26XX_rollbackDriverInitialisation(pdmHandle,
                                               (PDM_ROLLBACK_OPEN |
                                                PDM_ROLLBACK_DECIMATION_STATE |
                                                PDM_ROLLBACK_ACTIVE_PCM_BUFFER));

        return (NULL);
    }

    /* Then open the interface with these parameters */
    i2sHandle = PDMCC26XX_I2S_open(i2sHandle, &i2sParams);
    if (!i2sHandle){
        PDMCC26XX_rollbackDriverInitialisation(pdmHandle,
                                               (PDM_ROLLBACK_OPEN |
                                                PDM_ROLLBACK_DECIMATION_STATE |
                                                PDM_ROLLBACK_ACTIVE_PCM_BUFFER |
                                                PDM_ROLLBACK_PIN));

        return (NULL);
    }

    /* Construct ready and available queues */
    Queue_construct(&pcmMsgReady, NULL);
    pcmMsgReadyQueue = Queue_handle(&pcmMsgReady);

    return (handle);
}

/*
 * ======== PDMCC26XX_close ========
 * @brief   Function for closing the PDM driver on CC26XX devices
 *
 * @param   handle Handle to the PDM object
 */
void PDMCC26XX_close(PDMCC26XX_Handle handle) {
    Assert_isTrue(handle, NULL);

    /* Post close event to shutdown synchronously and prevent resetting settings and datastructures in the middle of a block ready event */
    Event_post(pdmEvents, PDM_EVT_CLOSE);

    /* Wait for PDM_EVT_CLOSE to take effect */
    SemaphoreP_pend(&synchronisationSemaphore, BIOS_WAIT_FOREVER);
}

/*
 * ======== PDMCC26XX_startStream ========
 * @brief   Function for starting a PDM stream
 *
 * @param   handle Handle to the PDM object
 *
 * @pre     ::PDMCC26XX_open() must be called first and there must not already be a stream in progress.
 *
 * @return  true if stream started, false if something went wrong.
 */
bool PDMCC26XX_startStream(PDMCC26XX_Handle handle) {
    PDMCC26XX_Object            *object;
    PDMCC26XX_HWAttrs const     *hwAttrs;

    Assert_isTrue(handle, NULL);

    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Disable preemption while checking if a transfer is in progress */
    uint32_t key = HwiP_disable();
    if (object->streamStarted) {
        HwiP_restore(key);

        /* Stream is in progress */
        return (false);
    }

    /* Reset decimation states */
    memset(object->decimationFilterState, 0x00, object->decimationFilterStateSize);

    /* Free ready elements*/
    while (!Queue_empty(pcmMsgReadyQueue)) {
        PDMCC26XX_queueNodePCM *readyNode = Queue_dequeue(pcmMsgReadyQueue);
        /* Free up memory used for PCM data buffer */
        object->freeFxn(readyNode->pBufferPCM, object->retBufSizeInBytes);
        /* Then free up memory used by the queue element */
        object->freeFxn(readyNode, sizeof(PDMCC26XX_queueNodePCM));
    }

    /* Reset compression data */
    metaDataForNextFrame.seqNum = 0;
    metaDataForNextFrame.si = 0;
    metaDataForNextFrame.pv = 0;

    /* Then start stream */
    if (PDMCC26XX_I2S_startStream(i2sHandle)) {
        /* Power microphone --> It typically requires some startup time */
        PIN_setOutputValue(object->pinHandle, hwAttrs->micPower, (object->micPowerActiveHigh) ? 1 : 0);

        /* The starting of stream succeeded, don't allow the device to enter
         * standby.
         */
        Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

        /* Make sure to flag that a stream is now active */
        object->streamStarted = true;

        HwiP_restore(key);

        /* Let thread prepare to throw the first PDM_DECIMATION_STARTUP_DELAY_IN_SAMPLES samples */
        Event_post(pdmEvents, PDM_EVT_START);

        return true;
    } else {
        /* If the starting of stream failed, return false*/
        HwiP_restore(key);
        return false;
    }
}

/*
 * ======== PDMCC26XX_stopStream ========
 * @brief   Function for stopping a PDM stream
 *
 * @param   handle Handle to the PDM object
 *
 * @pre     ::PDMCC26XX_startStream must have been called first.
 *
 * @return true if stream stopped correctly, false if something went wrong.
 */
bool PDMCC26XX_stopStream(PDMCC26XX_Handle handle) {
    PDMCC26XX_Object        *object;

    Assert_isTrue(handle, NULL);

    /* Get the pointer to the object */
    object = handle->object;

    /* Disable preemption while checking if a transfer is in progress */
    uint32_t key = HwiP_disable();
    if (!(object->streamStarted)) {
        HwiP_restore(key);

        /* Stream is not in progress */
        return (false);
    }
    HwiP_restore(key);

    /* Post event to synchronously shut down the stream. */
    Event_post(pdmEvents, PDM_EVT_STOP);

    /* Wait for PDM_EVT_STOP to take effect */
    SemaphoreP_pend(&synchronisationSemaphore, BIOS_WAIT_FOREVER);

    return true;
}

/*
 * ======== PDMCC26XX_Params_init ========
 * @brief   Function for initialising a PDMCC26XX_Params instance to its default value
 *
 * @param   params Pointer to a set of uninitialised params
 *
 */
void PDMCC26XX_Params_init(PDMCC26XX_Params *params) {
    *params = PDMCC26XX_defaultParams;
}

/*********************************************************************
* @fn      PDMCC26XX_taskFxn
*
* @brief   PDM task function which is processing the PDM events from the
*          driver (e.g. callback).
*
* @param   none
*
* @return  none
*/
static void PDMCC26XX_taskFxn(UArg a0, UArg a1) {
    Event_Params eventParams;
    PDMCC26XX_Object            *object = pdmHandle->object;;
    PDMCC26XX_HWAttrs const     *hwAttrs = pdmHandle->hwAttrs;
    uint32_t events             = 0;
    int16_t tempPcmBuf[32]      = {0};
    uint32_t throwAwayCount     = 0;    /* Number of bytes the driver should drop from the processed PCM data stream */
    uint32_t byteCount          = 0;    /* Index of the PCMBuffer currently being filled specifying how many bytes have been filled */
    uint32_t currTempBufIndex   = 0;    /* Index of the tempPcmBuf specifying how many bytes have been filled */
    bool pcmBufferFull          = false;
    bool tempBufActive          = false;
    PDMCC26XX_I2S_BufferRequest bufferRequest = {0};
    PDMCC26XX_I2S_BufferRelease bufferRelease = {0};

    /* Semaphore and event for the task */
    Event_Params_init(&eventParams);
    Event_construct(&sPDMEvents, &eventParams);
    pdmEvents = Event_handle(&sPDMEvents);

    /* Setup semaphore */
    SemaphoreP_constructBinary(&synchronisationSemaphore, 0);

    /* Loop forever */
    while (true) {
        events = Event_pend(pdmEvents,
                            Event_Id_NONE,
                            (PDM_EVT_BLK_RDY |
                             PDM_EVT_START |
                             PDM_EVT_STOP |
                             PDM_EVT_BLK_ERROR |
                             PDM_EVT_CLOSE |
                             PDM_EVT_I2S_DATA_DROPPED),
                            BIOS_WAIT_FOREVER);

        if (events & PDM_EVT_STOP) {

            if (!PDMCC26XX_I2S_stopStream(i2sHandle)) {
                /* We failed to stop!! */
                object->streamStarted = true;
            }
            else {
                 /* Make sure to flag that a stream is no longer active */
                object->streamStarted = false;

                /* Allow system to enter standby again */
                Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

                /* Unpower microphone */
                PIN_setOutputValue(object->pinHandle, hwAttrs->micPower, (object->micPowerActiveHigh) ? 0 : 1);
            }

            SemaphoreP_post(&synchronisationSemaphore);
        }

        if (events & PDM_EVT_CLOSE) {
            /* Move unused ready elements to available queue */
            while (!Queue_empty(pcmMsgReadyQueue)) {
                PDMCC26XX_queueNodePCM *readyNode = Queue_dequeue(pcmMsgReadyQueue);
                /* Free up memory used for PCM data buffer */
                object->freeFxn(readyNode->pBufferPCM, object->retBufSizeInBytes);
                /* Then free up memory used by the queue element */
                object->freeFxn(readyNode, sizeof(PDMCC26XX_queueNodePCM));
            }
            Queue_destruct(&pcmMsgReady);

            /*
             * Deallocate the activePcmBuffer, close down the I2S driver, release the pins back to the PIN driver, and set the PDM driver to closed.
             */
            PDMCC26XX_rollbackDriverInitialisation(pdmHandle,
                                                   (PDM_ROLLBACK_OPEN |
                                                    PDM_ROLLBACK_DECIMATION_STATE |
                                                    PDM_ROLLBACK_ACTIVE_PCM_BUFFER |
                                                    PDM_ROLLBACK_I2S_DRIVER |
                                                    PDM_ROLLBACK_PIN));

            /* Cancel all other queued events. After closed is called, we don't want the driver to do anything else without being reopened. */
            events = 0;

            SemaphoreP_post(&synchronisationSemaphore);
        }

        if (events & PDM_EVT_START) {

            pcmBufferFull = false;
            byteCount = 0;
            tempBufActive = false;
            currTempBufIndex = 0;

            /* Some microphones require a startup delay with clock applied. The
             * throw counter is used for this.
             */
            if (object->applyCompression) {
                /* If compression is enabled, the throwAwayCount is decremented
                 * with respect to compressed data output. This number is the half
                 * of the number of samples:
                 */
                throwAwayCount = PDMCC26XX_maxUInt32(PDM_DECIMATION_STARTUP_DELAY_IN_SAMPLES / 2, object->startupDelayWithClockInSamples / 2);
            }
            else {
                /* If compression is disabled, the throwAwayCount is decremented
                 * with respect to raw data, but in bytes not samples. Each sample
                 * is two bytes and the throwAwayCount is operating
                 * in bytes, this gives us:
                 */
                throwAwayCount = PDMCC26XX_maxUInt32(PDM_DECIMATION_STARTUP_DELAY_IN_SAMPLES * 2, object->startupDelayWithClockInSamples * 2);
            }
        }

        if (events & PDM_EVT_BLK_ERROR) {
            /* Notify caller of error */
            streamNotification.status = PDMCC26XX_STREAM_ERROR;
            object->callbackFxn(pdmHandle, &streamNotification);
            events &= ~PDM_EVT_BLK_ERROR;
        }

        if (events & PDM_EVT_I2S_DATA_DROPPED) {
            uint32_t key = HwiP_disable();
            /* The total number of PCM bytes that were dropped from the system. Includes those in the blocks dropped
             * by the I2S module and those already in the PCM buffer
             */
            uint32_t pcmBytesDropped = byteCount + droppedPdmBlockCount * (object->applyCompression ? PDMCC26XX_COMPR_ITER_OUTPUT_SIZE : PDMCC26XX_CPY_ITER_OUTPUT_SIZE);
            droppedPdmBlockCount = 0;

            HwiP_restore(key);

            /* Throw away enough PCM bytes to synchronise the PDM data stream with the PCMBuffer sequence number */
            if (pcmBytesDropped != object->pcmBufferSizeInBytes) {
                throwAwayCount += object->pcmBufferSizeInBytes - (pcmBytesDropped % object->pcmBufferSizeInBytes);
            }
            else {
                throwAwayCount = 0;
            }

            /* Since we must drop the current PCM buffer, we are dropping at minimum 1. That is taken care of by dropping new samples though. If it has been long enough since we last processed PDM data, we may need to drop more buffers. */
            metaDataForNextFrame.seqNum += pcmBytesDropped / object->pcmBufferSizeInBytes;

            byteCount = 0;
            tempBufActive = false;
            currTempBufIndex = 0;
        }

        if (events & PDM_EVT_BLK_RDY) {

            /* Request PDM data from I2S driver */
            while (PDMCC26XX_I2S_requestBuffer(i2sHandle, &bufferRequest)) {
                /* Buffer is available as long as it returns true */
                if (throwAwayCount == 0) {
                    /* Get new buffer from queue if active PCM buffer
                     * is full.
                     */
                    if (pcmBufferFull) {
                        /* PDMCC26XX_getNewPcmBuffer()
                         * Get new container from available queue and allocate
                         * memory for new buffer.
                         */
                        activePcmBuffer = PDMCC26XX_getNewPcmBuffer(pdmHandle);
                        if (activePcmBuffer) {
                            pcmBufferFull = false;
                            byteCount = 0;
                        }
                        else {
                            /* If we did not succeed getting a new pcm buffer, we
                             * need to start throwing data.
                             *
                             * Update throwAwayCount
                             * In this case throw bytes equal to the size of the
                             * PCM buffer minus the data output size of one iteration
                             * (which is dependent on compression/no compression).
                             *
                             * Note: Assuming that the (object->retBufSizeInBytes-PCM_METADATA_SIZE) is
                             * larger than the data output of one iteration.
                             */
                            throwAwayCount = object->pcmBufferSizeInBytes - (object->applyCompression ? PDMCC26XX_COMPR_ITER_OUTPUT_SIZE : PDMCC26XX_CPY_ITER_OUTPUT_SIZE);
                        }
                    }
                    /* Decimate PDM data to PCM, result is stored in tempPcmBuf */
                    object->pdm2pcmFxn(bufferRequest.bufferIn,
                                       object->decimationFilterState,
                                       object->decimationFilter,
                                       (int16_t *)&tempPcmBuf);

                    /* Mark the temp buf as active */
                    tempBufActive = true;
                    /* Since the tempPcmBuffer might fill up the current pcm
                     * buffer, we might have to perform the operation (compression
                     * or memcpy) on the tempPcmBuffer in more than one iteration.
                     *
                     * If we did not allocate a new buffer succesfully above, we
                     * are about to throw away data. In that case we should not
                     * perform an operation on the PDM data.
                     */
                    while (tempBufActive && !pcmBufferFull) {
                        int srcSize;

                        /* Next step is to handle the data in the tempPcmBuffer
                         *   - If compression is enabled, the compression function
                         *     will read in data from tempPcmBuffer and output the
                         *     compressed data to the dynamically allocated pcmBuffer.
                         *   - If compression is disabled, we use memcpy to move the pcm
                         *     data from the tempPcmBuffer to the dynamically allocated
                         *     pcmBuffer.
                         */
                        if (object->applyCompression) {
                           /* Prepare metadata.
                            * This is done before the first compression into the
                            * allocated PCM buffer.
                            */
                            if (byteCount == 0) {
                                activePcmBuffer->pBufferPCM->metaData.si = metaDataForNextFrame.si;
                                activePcmBuffer->pBufferPCM->metaData.pv = metaDataForNextFrame.pv;
                            }
                            /* If the current PCM buffer can fit the output of a
                             * compression of what is left in tempPcmBuf, set srcSize
                             * to that size. If not, set the srcSize to whatever will
                             * fit and mark the current pcm buffer as full.
                             *
                             * Note: currTempBufIndex will always be multiple of 2.
                             */
                            if ((object->pcmBufferSizeInBytes - byteCount) > PDMCC26XX_COMPR_ITER_OUTPUT_SIZE - (currTempBufIndex / 2)) {
                                /* srcSize set to whatever is left in the tempPcmBuffer. */
                                srcSize = (PDMCC26XX_COMPR_ITER_OUTPUT_SIZE * 2) - currTempBufIndex;
                            }
                            else {
                                /* This is the last compression into the current data buffer,
                                 * mark it as full.
                                 */
                                pcmBufferFull = true;
                                srcSize = ((object->retBufSizeInBytes-PCM_METADATA_SIZE) - byteCount) * 2;
                            }

                            /* Perform compression
                             *
                             * Since the source (tempPcmBuf) is operated as int16_t,
                             * while the output as uint8_t srcSize is size in samples
                             * (int16), it must be multiple of 2.
                             */
                            Codec1_encodeBuff((uint8_t *)&(activePcmBuffer->pBufferPCM->pBuffer[byteCount]),
                                            (int16_t *)&(tempPcmBuf[currTempBufIndex]),
                                            srcSize,
                                            (int8_t *)&metaDataForNextFrame.si,
                                            (int16_t *)&metaDataForNextFrame.pv);

                            /* Update byteCount for next iteration.
                             *
                             * Since the compression is reducing the size with a factor 4,
                             * and the pcm output buffer and the tempPcm buffer are of
                             * different types, the byteCount will be incremented with
                             * half the size of srcSize.
                             */
                            byteCount += srcSize / 2;

                            /* Prepare currTempBufIndex for next iteration */
                            currTempBufIndex += srcSize;
                        }
                        else {
                            /* Compression is disabled */
                            /* Compression will not be performed, so we copy data from
                             * temporary pcm buffer to allocated memory. Uncompressed
                             * PCM data buffer is 64 Bytes wide.
                             *
                             * Output and input are handled as bytes.
                             */
                            if ((object->pcmBufferSizeInBytes - byteCount) > (PDMCC26XX_CPY_ITER_OUTPUT_SIZE - (currTempBufIndex * 2))) {
                                srcSize = PDMCC26XX_CPY_ITER_OUTPUT_SIZE - (currTempBufIndex * 2);
                            }
                            else {
                                /* This is the last compression into the current data buffer,
                                 * mark it as full.
                                 */
                                pcmBufferFull = true;
                                srcSize = object->pcmBufferSizeInBytes - byteCount;
                            }

                            /* Copy PCM data from temp buffer to allocated memory */
                            memcpy(&(activePcmBuffer->pBufferPCM->pBuffer[byteCount]),
                                   &tempPcmBuf[currTempBufIndex],
                                   srcSize);/* <- size in bytes */

                           /* Prepare byteCount for next iteration, for memcpy the
                            * byteCount is equal to the srcSize.
                            */
                            byteCount += srcSize;

                            /* Prepare currTempBufIndex for next iteration.
                             * Since the srcSize is byte aligned, but the tempBuffer
                             * is sample (int16_t) aligned, divide srcSize with 2.
                             */
                            currTempBufIndex += srcSize / 2;
                        }

                        /* Is all the data in the tempPcmBuffer consumed?
                         * If so, reset the index count and clear active flag.
                         *
                         */
                        if (currTempBufIndex >= (sizeof(tempPcmBuf) / sizeof(tempPcmBuf[0]))) {
                            tempBufActive = false;
                            currTempBufIndex = 0;
                        }

                        if (pcmBufferFull) {
                            /* If the allocated PCM buffer is full, we need to flag
                             * that the PCM buffer is ready (and making the callback).
                             *
                             * And last, allocate a new PCM buffer to be used.
                             */
                            PDMCC26XX_setPcmBufferReady(pdmHandle, activePcmBuffer, (PDMCC26XX_I2S_BufferRequest *)&bufferRequest);

                            /* Get new PCM buffer from available queue and allocate
                             * memory for new buffer.
                             */
                            if ((activePcmBuffer = PDMCC26XX_getNewPcmBuffer(pdmHandle)) != NULL) {
                                pcmBufferFull = false;
                                byteCount = 0;
                            }
                            else {

                                /* if we did not succeed getting a new pcm buffer,
                                 * we need to start throwing data.
                                 *
                                 * Update ThrowCount
                                 * In this case throw count is equal to data fitting
                                 * in one allocated PCM buffer minus the output data
                                 * which will be thrown in this iteration.
                                 * Data thrown amount is data size of one iteration (which is dependent on
                                 * compression/no compression), minus the data already
                                 * consumed by the previous buffer.
                                 */
                                throwAwayCount = object->pcmBufferSizeInBytes - ((object->applyCompression ? PDMCC26XX_COMPR_ITER_OUTPUT_SIZE : PDMCC26XX_CPY_ITER_OUTPUT_SIZE) - currTempBufIndex);

                                /* Mark the tempBuf as no longer active */
                                tempBufActive = false;
                            }
                        }
                    }
                }
                else {
                    /* Still throwing away data */
                    if (throwAwayCount <= (object->applyCompression ? PDMCC26XX_COMPR_ITER_OUTPUT_SIZE : PDMCC26XX_CPY_ITER_OUTPUT_SIZE)) {
                        /* if the amount to be thrown away is less than or equal to
                         * the output of one iteration of the data operation, the
                         * count must be set to zero, the sequence number must be
                         * incremented, and the index to be used by next data operation
                         * must be updated correspondingly.
                         */

                        /* Note: Currently the startup delay will use the throwAwayCount
                         * and that means the sequence number will be incremented
                         * after the startup throwing has finished.
                         */
                        metaDataForNextFrame.seqNum++;
                        /* tempPcmBuf is int16 array, but the throwAwayCount is
                         * count in bytes.
                         */
                        if (object->applyCompression) {
                            /* If compression is enabled, the throwAwayCount number
                             * is the half of the number of samples:
                             */
                            currTempBufIndex = throwAwayCount * 2;
                        }
                        else {
                            /* If compression is disabled, the throwAwayCount is decremented
                             * with respect to raw data, but in bytes not samples. Since
                             * the currTempBufIndex is sample oriented, we need to divide
                             * that number with 2.
                             */
                            currTempBufIndex = (throwAwayCount + 1) / 2;
                        }
                        throwAwayCount = 0;
                    }
                    else {
                        /* Decrement the throw counter with amount corresponding to
                         * output data size of one iteration.
                         */
                        throwAwayCount -= (object->applyCompression ? PDMCC26XX_COMPR_ITER_OUTPUT_SIZE : PDMCC26XX_CPY_ITER_OUTPUT_SIZE);
                    }
                }
                /* Release PDM buffer */
                bufferRelease.bufferHandleIn = bufferRequest.bufferHandleIn;
                PDMCC26XX_I2S_releaseBuffer(i2sHandle, &bufferRelease);
            }
        }
    }
}

/*
 *  ======== PDMCC26XX_requestBuffer ========
 *  @pre    Function assumes that the stream has started and that bufferRequest is not NULL.
 */
bool PDMCC26XX_requestBuffer(PDMCC26XX_Handle handle, PDMCC26XX_BufferRequest *bufferRequest) {
    PDMCC26XX_Object            *object;

    Assert_isTrue(handle, NULL);
    Assert_isTrue(bufferRequest, NULL);

    /* Get the pointer to the object */
    object = pdmHandle->object;

    /* We expect the user to call this after being notified of available
     * buffers. Hence we may directly check queue and dequeue buffer
     */
    if (!Queue_empty(pcmMsgReadyQueue)){
        PDMCC26XX_queueNodePCM *readyNode = Queue_get(pcmMsgReadyQueue);
        /* Provide pointer to buffer including 4 byte metadata */
        bufferRequest->buffer = readyNode->pBufferPCM;
        bufferRequest->status = streamNotification.status;
        /* free up memory used by queue element */
        object->freeFxn(readyNode, sizeof(PDMCC26XX_queueNodePCM));
    }
    else {
        return false;
    }

    return true;
}

static void PDMCC26XX_i2sCallbackFxn(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_StreamNotification *notification) {
    uint32_t event = 0;

    switch (notification->status) {
        case PDMCC26XX_I2S_STREAM_BUFFER_READY:
            event = PDM_EVT_BLK_RDY;
            break;
        case PDMCC26XX_I2S_STREAM_ERROR:
            event = PDM_EVT_BLK_ERROR;
            break;
        case PDMCC26XX_I2S_STREAM_BUFFER_DROPPED:
            /* A block of old PDM data was dropped to make space for new data */
            event = PDM_EVT_I2S_DATA_DROPPED;
            /* This function is called in a hwi context. Hence, there is not need for a critical
             * section to guard against accesses by the PDM task */
            droppedPdmBlockCount++;
            break;
        default:
            break;
    }

    Event_post(pdmEvents, event);
}

/*
 *  ======== PDMCC26XX_initIO ========
 *  This functions initializes the PDM IOs.
 *
 *  @pre    Function assumes that the PDM handle is pointing to a hardware
 *          module which has already been opened.
 */
static bool PDMCC26XX_initIO(PDMCC26XX_Handle handle) {
    PDMCC26XX_Object        *object;
    PDMCC26XX_HWAttrs const *hwAttrs;
    PIN_Config              micPinTable[PDM_NUMBER_OF_PINS + 1];
    uint32_t                i = 0;

    Assert_isTrue(handle, NULL);

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Configure IOs */
    /* Build local list of pins, allocate through PIN driver and map HW ports */
    if (hwAttrs->micPower != PIN_UNASSIGNED) {
        micPinTable[i++] = hwAttrs->micPower | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH  | PIN_PUSHPULL | PIN_DRVSTR_MAX;
    }
    micPinTable[i] = PIN_TERMINATE;

    /* Open and assign pins through pin driver */
    if (!(object->pinHandle = PIN_open(&(object->pinState), micPinTable))) {
      return false;
    }

    return true;
}

/*
 *  ======== PDMCC26XX_setPcmBufferReady ========
 *  This function prepares metadata, puts the buffer in ready queue, sets stream
 *  status and makes the callback.
 *
 *  @pre    Function assumes that the PDM handle is pointing to a hardware
 *          module which has already been opened.
 */
static void PDMCC26XX_setPcmBufferReady(PDMCC26XX_Handle handle, PDMCC26XX_queueNodePCM *pcmBuffer, PDMCC26XX_I2S_BufferRequest *bufReq) {
    PDMCC26XX_Object *object;
    object = handle->object;

    Assert_isTrue(handle, NULL);
    Assert_isTrue(pcmBuffer, NULL);
    Assert_isTrue(bufReq, NULL);

    /* Update sequence number */
    pcmBuffer->pBufferPCM->metaData.seqNum = metaDataForNextFrame.seqNum++;
    /* Place PCM buffer in ready queue */
    Queue_put(pcmMsgReadyQueue, &pcmBuffer->queueElement);

    /* Notify caller by updating the stream status */
    if (bufReq->status == PDMCC26XX_I2S_STREAM_BUFFER_READY) {
        streamNotification.status = PDMCC26XX_STREAM_BLOCK_READY;
    }
    else if (bufReq->status == PDMCC26XX_I2S_STREAM_BUFFER_DROPPED) {
        streamNotification.status = PDMCC26XX_STREAM_BLOCK_READY_BUT_PDM_OVERFLOW;
    }
    else {
        streamNotification.status = PDMCC26XX_STREAM_STOPPING;
    }
    /* Only notify when PCM buffer is complete */
    object->callbackFxn(pdmHandle, &streamNotification);
}

/*
 *  ======== PDMCC26XX_getNewPcmBuffer ========
 *  This function gets a new queue element from the available queue and then
 *  tries to allocate the memory space needed for a new buffer.
 *
 *  @return true if a new PCM buffer was succesfully allocated, false if the
 *          available queue was empty or the memory allocation function did not
 *          succeed.
 */
static PDMCC26XX_queueNodePCM * PDMCC26XX_getNewPcmBuffer(PDMCC26XX_Handle handle) {
    PDMCC26XX_Object *object;
    PDMCC26XX_queueNodePCM *buf;

    Assert_isTrue(handle, NULL);

    object = handle->object;

    /* Allocate memory for a new queue element */
    buf = object->mallocFxn(sizeof(PDMCC26XX_queueNodePCM));
    /* If allocation went OK, allocate more... */
    if (buf != NULL) {
        /* Dynamically allocated memory for new pcm buffer */
        buf->pBufferPCM = object->mallocFxn(object->retBufSizeInBytes);
        /* If new memory was allocated correctly, return pointer. */
        if (buf->pBufferPCM != NULL) {
            return buf;
        }
        else {
            /* Was not able to allocate memory for the pcm buffer, deallocate
             * the memory used by queue element.
             */
            object->freeFxn(buf, sizeof(PDMCC26XX_queueNodePCM));
        }
    }

    return NULL;
}

/*
 * ======== PDMCC26XX_rollbackDriverInitialisation ========
 * This function rolls back different parts of the PDM driver initialisation depending on the rollbackVector.
 * Passing ~0 as the rollbackVector will reverse all failable initialisations.
 * Only those parts of the driver that can fail when calling PDMCC26XX_open can be included as entries in the rollbackVector.
 */
static void PDMCC26XX_rollbackDriverInitialisation(PDMCC26XX_Handle handle, uint32_t rollbackVector){
    unsigned int            key;
    PDMCC26XX_Object        *object;

    Assert_isTrue(handle, NULL);
    Assert_isTrue(rollbackVector, NULL);

    object = handle->object;


    if (rollbackVector & PDM_ROLLBACK_I2S_DRIVER) {
        /* Release ownership and revert to init settings. */
        PDMCC26XX_I2S_close(i2sHandle);
    }

    if (rollbackVector & PDM_ROLLBACK_PIN) {
        /* Release the allocated pins back to the pin driver */
        PIN_close(object->pinHandle);
    }

    if (rollbackVector & PDM_ROLLBACK_DECIMATION_STATE) {
        /* Free up memory used for decimationFilterState */
        object->freeFxn(object->decimationFilterState, object->decimationFilterStateSize);
    }

    if (rollbackVector & PDM_ROLLBACK_ACTIVE_PCM_BUFFER) {
        /*
         * Free the activePcmBuffer if it is not NULL. It can be NULL if either insufficient time was provided after PDMCC26XX_open for
         * the open event in the task function to run and allocate the memory or if we ran out of heap space earlier and the PDMCC26XX_getNewPcmBuffer
         * function returned NULL.
         */
        if (activePcmBuffer != NULL){
            /* Free up memory used for activePcmBuffer */
            object->freeFxn(activePcmBuffer->pBufferPCM, object->retBufSizeInBytes);
            /* Then free up memory used by the queue element */
            object->freeFxn(activePcmBuffer, sizeof(PDMCC26XX_queueNodePCM));
            /* Make sure the activePcmBuffer doesn't point anywhere anymore as the target no longer exists. */
            activePcmBuffer = NULL;
        }
    }

    if (rollbackVector & PDM_ROLLBACK_OPEN) {
        /* Mark the module as available */
        key = HwiP_disable();
        object->isOpen = false;
        HwiP_restore(key);
    }
}

static inline uint32_t PDMCC26XX_maxUInt32(uint32_t a, uint32_t b){
    return ((a > b) ? a : b);
}
