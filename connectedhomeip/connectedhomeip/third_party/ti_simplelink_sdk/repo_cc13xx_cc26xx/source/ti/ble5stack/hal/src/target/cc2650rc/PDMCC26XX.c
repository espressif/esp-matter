/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/drivers/pdm/Codec1.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/pdm/PDMCC26XX_util.h>
#include <ti/drivers/pdm/PDMCC26XX.h>

#include <string.h>


/*********************************************************************
 * CONSTANTS
 */
const int32_t PDMCC26XX_aBqCoeffs[] = {
 //--v--  Adjust overall gain by changing this coefficient
    331,     0, -1024, -1356,   342,     // DC-notch, halfband LP filter (@32 kHz)
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

const uint16_t PDMCC26XX_gainTable[PDMCC26XX_GAIN_END] = {
    1318, //PDMCC26XX_GAIN_24,
    660, //PDMCC26XX_GAIN_18,
    331, //PDMCC26XX_GAIN_12,  // Default
    166, //PDMCC26XX_GAIN_6,
    83, //PDMCC26XX_GAIN_0,
//    42, //PDMCC26XX_GAIN_N6,
//    21, //PDMCC26XX_GAIN_N12,
//    7, //PDMCC26XX_GAIN_N22,
//    2, //PDMCC26XX_GAIN_N32
};

/*********************************************************************
 * MACROS
 */
#define MIN(a,b) ((a) < (b) ? a : b)
#define MAX(a,b) ((a) > (b) ? a : b)

/*********************************************************************
 * TYPEDEFS
*/

/*! Struct that contains a PCM queue element and a pointer to the data buffer it is responsible for */
typedef struct {
    Queue_Elem  _elem;                  /*!< Queue element */
    PDMCC26XX_pcmBuffer *pBufferPCM;    /*!< Pointer to a ::PDMCC26XX_pcmBuffer */
} PDMCC26XX_queuePCM;




/*********************************************************************
 * LOCAL VARIABLES
 */
/* PDM Task Configuration */
Task_Struct pdmTask;
Char pdmTaskStack[PDM_TASK_STACK_SIZE];

/* PDM Hwi Configuration */
Hwi_Struct pdmHwi;
Hwi_Params pdmHwiParams;

/* Static allocated memory for PDM data stream
 * Used instead of dynamic allocation due to heap fragmentation seen during test.
 */
pdmSample_t pdmContinuousBuffer[PDM_BUFFER_SIZE_IN_BLOCKS * PDM_BLOCK_SIZE_IN_SAMPLES * PDM_NUM_OF_CHANNELS];
uint8_t pdmContMgtBuffer[PDM_BUFFER_SIZE_IN_BLOCKS * I2S_BLOCK_OVERHEAD_IN_BYTES];

/* Keep track of compression variables */
static PDMCC26XX_metaData metaDataForNextFrame = {0};

static int32_t decimationFilterInRAM[sizeof(PDMCC26XX_aBqCoeffs) / sizeof(PDMCC26XX_aBqCoeffs[0])];
static uint32_t decimationState[6+5*2] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static Queue_Struct pcmMsgReady;
static Queue_Handle pcmMsgReadyQueue;

static Event_Struct sPDMEvents;
static Event_Handle pdmEvents;

static PDMCC26XX_Handle pdmHandle = NULL;

static int byteCount = 0;
static int currTempBufIndex = 0;

static PDMCC26XX_queuePCM *activePcmBuffer;

PDMCC26XX_StreamNotification streamNotification = {
    .arg = NULL,
    .status = PDMCC26XX_STREAM_IDLE
};

PDMCC26XX_I2S_StreamNotification pdmStream;
PDMCC26XX_I2S_Handle i2sHandle;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void PDMCC26XX_taskFxn(UArg a0, UArg a1);
static bool PDMCC26XX_initIO(PDMCC26XX_Handle handle);
static void PDMCC26XX_setPcmBufferReady(PDMCC26XX_Handle handle, PDMCC26XX_queuePCM *pcmBuffer, PDMCC26XX_I2S_BufferRequest *bufReq);
static PDMCC26XX_queuePCM * PDMCC26XX_getNewPcmBuffer(PDMCC26XX_Handle handle);
static void PDMCC26XX_i2sCallbackFxn(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_StreamNotification *notification);
static void PDMCC26XX_rollbackDriverInitialisation(PDMCC26XX_Handle handle, uint32_t rollbackVector);

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern bool pdm2pcm16k(const void* pIn, uint32_t* pState, const int32_t* pBqCoeffs, int16_t* pOut);
extern uint8_t tic1_EncodeBuff(uint8_t* dst, int16_t* src, int16_t srcSize, int8_t *si, int16_t *pv);

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
    PDMCC26XX_Handle         handle;
    PDMCC26XX_Object         *object;
    unsigned int             key;

    /* Get handle for this driver instance */
    handle = pdmHandle;
    /* Get the pointer to the object */
    object = handle->object;

    /* Disable preemption while checking if the PDM is open. */
    key = Hwi_disable();

    /* Check if the PDM is open already with the base addr. */
    if (object->isOpen == true) {
        Hwi_restore(key);

        Log_warning0("PDM: already in use.");

        return (NULL);
    }

    /* Mark the handle as being used */
    object->isOpen = true;
    Hwi_restore(key);

    /* Initialize the PDM object */
    object->callbackFxn                    = params->callbackFxn;
    object->useDefaultFilter               = params->useDefaultFilter;
    object->micGain                        = params->micGain;
    object->micPowerActiveHigh             = params->micPowerActiveHigh;
    object->applyCompression               = params->applyCompression;
    object->startupDelayWithClockInSamples = params->startupDelayWithClockInSamples;
    object->bStreamStarted                 = false;
    object->streamNotification             = &streamNotification;
    object->mallocFxn                      = params->mallocFxn;
    object->freeFxn                        = params->freeFxn;
    object->retBufSizeInBytes              = params->retBufSizeInBytes;

    /* Get first buffer for PCM data so that we don't get NULL
     * pointer exception.
     */
    if ((activePcmBuffer = PDMCC26XX_getNewPcmBuffer(pdmHandle)) == NULL) {
        /* We didn't manage to allocate enough space on the heap for the activePcmBuffer.
         * Exit with return value NULL to prevent the driver running with activePcmBuffer == NULL
         */

        PDMCC26XX_rollbackDriverInitialisation(handle, PDM_ROLLBACK_OPEN);

        Log_warning0("PDM: heap is full could not allocate space for activePcmBuffer.");
        return (NULL);
    }

    /* Default PDMCC26XX_I2S parameters structure */
    PDMCC26XX_I2S_Params PDMCC26XX_I2S_params = {
        .requestMode            = PDMCC26XX_I2S_CALLBACK_MODE,
        .ui32requestTimeout     = BIOS_WAIT_FOREVER,
        .callbackFxn            = PDMCC26XX_i2sCallbackFxn,
        .blockSize              = PDM_BLOCK_SIZE_IN_SAMPLES,
        .pvContBuffer           = (void *) pdmContinuousBuffer,
        .ui32conBufTotalSize    = sizeof(pdmContinuousBuffer),
        .pvContMgtBuffer        = (void *) pdmContMgtBuffer,
        .ui32conMgtBufTotalSize = sizeof(pdmContMgtBuffer),
        .currentStream          = &pdmStream
    };

    /* Init IOs */
    if (PDMCC26XX_initIO(handle) == false){
        /* We couldn't allocate the necessary pins though the PIN driver. */

        PDMCC26XX_rollbackDriverInitialisation(pdmHandle, PDM_ROLLBACK_OPEN | PDM_ROLLBACK_ACTIVE_PCM_BUFFER);

        Log_warning0("PDM: PIN driver could not allocate necessary i/o's.");
        return (NULL);
    }

    /* Then open the interface with these parameters */
    if ((i2sHandle = PDMCC26XX_I2S_open(i2sHandle, &PDMCC26XX_I2S_params)) == NULL){

        PDMCC26XX_rollbackDriverInitialisation(pdmHandle, PDM_ROLLBACK_OPEN | PDM_ROLLBACK_ACTIVE_PCM_BUFFER | PDM_ROLLBACK_PIN);

        Log_print0(Diags_USER1, "Failed to open the I2S driver");
        return (NULL);
    }

    /* Set dependency on the UDMA. This makes sure it's clocked, which in turn keeps the system bus active. */
    Power_setDependency(PowerCC26XX_PERIPH_UDMA);

    /* Construct ready and available queues */
    Queue_construct(&pcmMsgReady, NULL);
    pcmMsgReadyQueue = Queue_handle(&pcmMsgReady);

    /* Initialize filters */
    if (object->useDefaultFilter) {
        /* Use default */
        object->decimationFilter = decimationFilterInRAM;
        memcpy(object->decimationFilter, PDMCC26XX_aBqCoeffs, sizeof(PDMCC26XX_aBqCoeffs));
    }
    else {
        object->decimationFilter = params->decimationFilter;
    }
    /* Apply gain, if set */
    if (object->micGain < PDMCC26XX_GAIN_END) {
        object->decimationFilter[0] = PDMCC26XX_gainTable[object->micGain];
    }

    return (handle);
}

/*
 * ======== PDMCC26XX_close ========
 * @brief   Function for closing the PDM driver on CC26XX devices
 *
 * @param   handle Handle to the PDM object
 */
void PDMCC26XX_close(PDMCC26XX_Handle handle) {

    /* Post close event to shutdown synchronously and prevent resetting settings and datastructures in the middle of a block ready event */
    Event_post(pdmEvents, PDM_EVT_CLOSE);

    Log_print0(Diags_USER1, "PDM: close event posted");
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
    unsigned int            key;
    PDMCC26XX_Object        *object;
    PDMCC26XX_HWAttrs const  *hwAttrs;
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Disable preemption while checking if a transfer is in progress */
    key = Hwi_disable();
    if (object->bStreamStarted) {
        Hwi_restore(key);

        Log_error0("PDM: stream in progress");

        /* Stream is in progress */
        return (false);
    }

    /* Power microphone --> It typically requires some startup time */
    PIN_setOutputValue(object->pinHandle, hwAttrs->micPower, (object->micPowerActiveHigh) ? 1 : 0);

    uint32_t i = 0;
    /* Reset decimation states */
    for (i = 0; i < sizeof(decimationState)/sizeof(decimationState[0]); ++i) {
        decimationState[i] = 0x00;
    }

    /* Move unused ready elements to available queue*/
    while (!Queue_empty(pcmMsgReadyQueue)) {
        PDMCC26XX_queuePCM *readyNode = Queue_dequeue(pcmMsgReadyQueue);
        /* Free up memory used for PCM data buffer */
        object->freeFxn(readyNode->pBufferPCM, object->retBufSizeInBytes);
        /* Then free up memory used by the queue element */
        object->freeFxn(readyNode, sizeof(PDMCC26XX_queuePCM));
    }

    /* Reset compression data */
    metaDataForNextFrame.seqNum = 0;
    metaDataForNextFrame.si = 0;
    metaDataForNextFrame.pv = 0;

    /* Then start stream */
    if (PDMCC26XX_I2S_startStream(i2sHandle)) {
        /* The starting of stream succeeded, don't allow the device to enter
         * standby.
         */
        Power_setConstraint(PowerCC26XX_SB_DISALLOW);

        /* Make sure to flag that a stream is now active */
        object->bStreamStarted = true;

        Hwi_restore(key);

        /* Let thread prepare to throw the first PDM_DECIMATION_STARTUP_DELAY_IN_SAMPLES samples */
        Event_post(pdmEvents, PDM_EVT_START);
        return true;
    } else {
        /* If the starting of stream failed, return false*/
        Hwi_restore(key);
        return false;
    }
}

/*
 * ======== PDMCC26XX_stopStream ========
 * @brief   Function for starting a PDM stream
 *
 * @param   handle Handle to the PDM object
 *
 * @pre     ::PDMCC26XX_startStream must have been called first.
 *
 * @return true if stream stopped correctly, false if something went wrong.
 */
bool PDMCC26XX_stopStream(PDMCC26XX_Handle handle) {
    unsigned int            key;
    bool                    retVal = false;
    PDMCC26XX_Object        *object;
    PDMCC26XX_HWAttrs const *hwAttrs;

    /* Get the pointer to the object and hwAttr */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Disable preemption while checking if a transfer is in progress */
    key = Hwi_disable();
    if (!(object->bStreamStarted)) {
        Hwi_restore(key);

        Log_error0("PDM: stream not in progress");

        /* Stream is not in progress */
        return (false);
    }
    /* Make sure to flag that a stream is no longer active */
    object->bStreamStarted = false;
    Hwi_restore(key);

    if (!PDMCC26XX_I2S_stopStream(i2sHandle)) {
        /* We failed to stop!! */
        object->bStreamStarted = true;
    }
    else {
        /* Allow system to enter standby again */
        Power_releaseConstraint(PowerCC26XX_SB_DISALLOW);

        /* Unpower microphone */
        PIN_setOutputValue(object->pinHandle, hwAttrs->micPower, (object->micPowerActiveHigh) ? 0 : 1);

        retVal = true;
    }
    return retVal;
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
    uint32_t events;
    static int16_t tempPcmBuf[32];
    PDMCC26XX_I2S_BufferRequest bufferRequest;
    PDMCC26XX_I2S_BufferRelease bufferRelease;
    static uint32_t throwAwayCount = 0;
    static bool pcmBufferFull = false;
    static bool tempBufActive = false;
    PDMCC26XX_Object            *object;
    object = pdmHandle->object;

    /* Semaphore and event for the task */
    Event_Params evParams;
    Event_Params_init(&evParams);
    Event_construct(&sPDMEvents, &evParams);
    pdmEvents = Event_handle(&sPDMEvents);

    /* Loop forever */
    for (;;) {
        events = Event_pend(pdmEvents, Event_Id_NONE, (PDM_EVT_BLK_RDY | PDM_EVT_START | PDM_EVT_BLK_ERROR | PDM_EVT_CLOSE), BIOS_WAIT_FOREVER);

        if ( events & PDM_EVT_CLOSE) {

            /* Release dependency on the UDMA, such that the bus can be deactivated as needed */
            Power_releaseDependency(PowerCC26XX_PERIPH_UDMA);

            /* Move unused ready elements to available queue */
            while (!Queue_empty(pcmMsgReadyQueue)) {
                PDMCC26XX_queuePCM *readyNode = Queue_dequeue(pcmMsgReadyQueue);
                /* Free up memory used for PCM data buffer */
                object->freeFxn(readyNode->pBufferPCM, object->retBufSizeInBytes);
                /* Then free up memory used by the queue element */
                object->freeFxn(readyNode, sizeof(PDMCC26XX_queuePCM));
            }
            Queue_destruct(&pcmMsgReady);

            Hwi_destruct(&pdmHwi);

            /*
             * Deallocate the activePcmBuffer, close down the I2S driver, release the pins back to the PIN driver, and set the PDM driver to closed.
             */
            PDMCC26XX_rollbackDriverInitialisation(pdmHandle, PDM_ROLLBACK_OPEN | PDM_ROLLBACK_ACTIVE_PCM_BUFFER | PDM_ROLLBACK_I2S_DRIVER | PDM_ROLLBACK_PIN);

            Log_print0(Diags_USER1, "PDM: close driver synchronously");

            /* Cancel all other queued events. After closed is called, we don't want the driver to do anything else without being reopened. */
            events &= 0;
        }

        if ( events & PDM_EVT_START ) {

            pcmBufferFull = false;
            byteCount = 0;
            tempBufActive = false;

            /* Some microphones require a startup delay with clock applied. The
             * throw counter is used for this.
             */
            if(object->applyCompression) {
                /* If compression is enabled, the throwAwayCount is decremented
                 * with respect to compressed data output. This number is the half
                 * of the number of samples:
                 */
                throwAwayCount = MAX(PDM_DECIMATION_STARTUP_DELAY_IN_SAMPLES / 2, object->startupDelayWithClockInSamples / 2);
            }
            else {
                /* If compression is disabled, the throwAwayCount is decremented
                 * with respect to raw data, but in bytes not samples. Each sample
                 * is two bytes and the throwAwayCount is operating
                 * in bytes, this gives us:
                 */
                throwAwayCount = MAX(PDM_DECIMATION_STARTUP_DELAY_IN_SAMPLES * 2, object->startupDelayWithClockInSamples * 2);
            }
            events &= ~PDM_EVT_START;
        }

        if ( events & PDM_EVT_BLK_ERROR ) {
            /* Notify caller of error */
            streamNotification.status = PDMCC26XX_STREAM_ERROR;
            object->callbackFxn(pdmHandle, &streamNotification);
        }

        if ( events & PDM_EVT_BLK_RDY ) {
            /* Request PDM data from I2S driver */
            while ( PDMCC26XX_I2S_requestBuffer(i2sHandle, &bufferRequest) ) {
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
                        if ((activePcmBuffer = PDMCC26XX_getNewPcmBuffer(pdmHandle)) != NULL) {
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
                            if((object->retBufSizeInBytes-PCM_METADATA_SIZE) > (object->applyCompression ? PDMCC26XX_COMPR_ITER_OUTPUT_SIZE : PDMCC26XX_CPY_ITER_OUTPUT_SIZE)){
                                throwAwayCount = (object->retBufSizeInBytes-PCM_METADATA_SIZE) - (object->applyCompression ? PDMCC26XX_COMPR_ITER_OUTPUT_SIZE : PDMCC26XX_CPY_ITER_OUTPUT_SIZE);
                            }
                            else {
                                /* If the retBufSizeInBytes without the metadata is less than the output of one iteration, we would get an underflow on the throwAwayCount.
                                 * Instead, we throw away the entire buffer and leave the throwAwayCount at 0.
                                 * Since we've thrown away more than one PCM buffer worth of PDM data, we need to increment sequence number.
                                 */
                                metaDataForNextFrame.seqNum++;
                            }
                        }
                    }
                    /* Decimate PDM data to PCM, result is stored in tempPcmBuf */
                    pdm2pcm16k(bufferRequest.bufferIn, decimationState, object->decimationFilter, (int16_t *)&tempPcmBuf);

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
                    while(tempBufActive && !pcmBufferFull) {
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
                            if(byteCount == 0) {
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
                            if (((object->retBufSizeInBytes-PCM_METADATA_SIZE) - byteCount) > PDMCC26XX_COMPR_ITER_OUTPUT_SIZE - (currTempBufIndex/2)) {
                                /* srcSize set to whatever is left in the tempPcmBuffer. */
                                srcSize = PDMCC26XX_COMPR_ITER_OUTPUT_SIZE*2 - currTempBufIndex;
                            }
                            else {
                                /* This is the last compression into the current data buffer,
                                 * mark it as full.
                                 */
                                pcmBufferFull = true;
                                srcSize = ((object->retBufSizeInBytes-PCM_METADATA_SIZE) - byteCount)*2;
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
                             * Since the compression is reduzing the size with a factor 4,
                             * and the pcm output buffer and the tempPcm buffer are of
                             * different types, the byteCount will be incremented with
                             * half the size of srcSize.
                             */
                            byteCount += srcSize/2;

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
                            if (((object->retBufSizeInBytes-PCM_METADATA_SIZE) - byteCount) > (PDMCC26XX_CPY_ITER_OUTPUT_SIZE - (currTempBufIndex*2))) {
                                srcSize = PDMCC26XX_CPY_ITER_OUTPUT_SIZE - (currTempBufIndex*2);
                            }
                            else {
                                /* This is the last compression into the current data buffer,
                                 * mark it as full.
                                 */
                                pcmBufferFull = true;
                                srcSize = (object->retBufSizeInBytes-PCM_METADATA_SIZE) - byteCount;
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
                            currTempBufIndex += srcSize/2;
                        }

                        /* Is all the data in the tempPcmBuffer consumed?
                         * If so, reset the index count and clear active flag.
                         *
                         */
                        if(currTempBufIndex >= (sizeof(tempPcmBuf)/sizeof(tempPcmBuf[0]))) {
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
                                if((object->retBufSizeInBytes-PCM_METADATA_SIZE) > (object->applyCompression ? PDMCC26XX_COMPR_ITER_OUTPUT_SIZE : PDMCC26XX_CPY_ITER_OUTPUT_SIZE) - currTempBufIndex){
                                    throwAwayCount = (object->retBufSizeInBytes-PCM_METADATA_SIZE) - ((object->applyCompression ? PDMCC26XX_COMPR_ITER_OUTPUT_SIZE : PDMCC26XX_CPY_ITER_OUTPUT_SIZE) - currTempBufIndex);
                                }
                                else {
                                    /* If the retBufSizeInBytes without the metadata is less than the output of one iteration minus the currTempBufIndex, we would get an underflow on the throwAwayCount.
                                     * Instead, we throw away the entire buffer and leave the throwAwayCount at 0.
                                     * Since we've thrown away more than one PCM buffer worth of PDM data, we need to increment sequence number.
                                     */
                                    metaDataForNextFrame.seqNum++;
                                }
                                /* Mark the tempBuf as no longer active */
                                tempBufActive = false;
                            }
                        }
                    }
                }
                else {
                    /* Still throwing away data */
                    if (throwAwayCount<=(object->applyCompression ? PDMCC26XX_COMPR_ITER_OUTPUT_SIZE : PDMCC26XX_CPY_ITER_OUTPUT_SIZE)) {
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
                        if(object->applyCompression) {
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
            events &= ~PDM_EVT_BLK_RDY;
        }
    }
}

/*
 *  ======== PDMCC26XX_requestBuffer ========
 *  @pre    Function assumes that the stream has started and that bufferRequest is not NULL.
 */
bool PDMCC26XX_requestBuffer(PDMCC26XX_Handle handle, PDMCC26XX_BufferRequest *bufferRequest) {
    PDMCC26XX_Object            *object;

    /* Get the pointer to the object */
    object = pdmHandle->object;

    /* We expect the user to call this after being notified of available
     * buffers. Hence we may directly check queue and dequeue buffer
     */
    if (!Queue_empty(pcmMsgReadyQueue)){
        PDMCC26XX_queuePCM *readyNode = Queue_get(pcmMsgReadyQueue);
        /* Provide pointer to buffer including 4 byte metadata */
        bufferRequest->buffer = readyNode->pBufferPCM;
        bufferRequest->status = streamNotification.status;
        /* free up memory used by queue element */
        object->freeFxn(readyNode, sizeof(PDMCC26XX_queuePCM));
    }
    else {
        return false;
    }

    return true;
}

static void PDMCC26XX_i2sCallbackFxn(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_StreamNotification *notification) {

    if (notification->status == PDMCC26XX_I2S_STREAM_ERROR) {
        /* Let thread process PDM error */
        Event_post(pdmEvents, PDM_EVT_BLK_ERROR);
    }
    else {
        /* Let thread process PDM data */
        Event_post(pdmEvents, PDM_EVT_BLK_RDY);
    }
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
    uint32_t                i=0;

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
static void PDMCC26XX_setPcmBufferReady(PDMCC26XX_Handle handle, PDMCC26XX_queuePCM *pcmBuffer, PDMCC26XX_I2S_BufferRequest *bufReq) {
    PDMCC26XX_Object *object;
    object = handle->object;

    /* Update sequence number */
    pcmBuffer->pBufferPCM->metaData.seqNum = metaDataForNextFrame.seqNum++;
    /* Place PCM buffer in ready queue */
    Queue_put(pcmMsgReadyQueue, &pcmBuffer->_elem);

    /* Notify caller by updating the stream status */
    if (bufReq->status == PDMCC26XX_I2S_STREAM_BUFFER_READY) {
        streamNotification.status = PDMCC26XX_STREAM_BLOCK_READY;
    }
    else if (bufReq->status == PDMCC26XX_I2S_STREAM_BUFFER_READY_BUT_NO_AVAILABLE_BUFFERS) {
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
static PDMCC26XX_queuePCM * PDMCC26XX_getNewPcmBuffer(PDMCC26XX_Handle handle) {
    PDMCC26XX_Object *object;
    PDMCC26XX_queuePCM *buf;

    object = handle->object;

    /* Allocate memory for a new queue element */
    buf = object->mallocFxn(sizeof(PDMCC26XX_queuePCM));
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
            object->freeFxn(buf, sizeof(PDMCC26XX_queuePCM));
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
    object = handle->object;


    if (rollbackVector & PDM_ROLLBACK_I2S_DRIVER) {
        /* Release ownership and revert to init settings. */
        PDMCC26XX_I2S_close(i2sHandle);
    }

    if (rollbackVector & PDM_ROLLBACK_PIN) {
        /* Release the allocated pins back to the pin driver */
        PIN_close(object->pinHandle);
    }

    if (rollbackVector & PDM_ROLLBACK_ACTIVE_PCM_BUFFER) {
        /*
         * Free the activePcmBuffer if it is not NULL. It can be NULL if either insufficient time was provided after PDMCC26XX_open for
         * the open event in the task function to run and allocate the memory or if we ran out of heap space earlier and the PDMCC26XX_getNewPcmBuffer
         * function returned NULL.
         */
        if(activePcmBuffer != NULL){
            /* Free up memory used for activePcmBuffer */
            object->freeFxn(activePcmBuffer->pBufferPCM, object->retBufSizeInBytes);
            /* Then free up memory used by the queue element */
            object->freeFxn(activePcmBuffer, sizeof(PDMCC26XX_queuePCM));
            /* Make sure the activePcmBuffer doesn't point anywhere anymore as the target no longer exists. */
            activePcmBuffer = NULL;
        }
    }

    if (rollbackVector & PDM_ROLLBACK_OPEN) {
        /* Mark the module as available */
        key = Hwi_disable();
        object->isOpen = false;
        Hwi_restore(key);
    }
}
