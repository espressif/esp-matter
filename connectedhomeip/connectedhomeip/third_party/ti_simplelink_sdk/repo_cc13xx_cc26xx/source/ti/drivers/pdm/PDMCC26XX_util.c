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

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Assert.h>

#include <ti/sysbios/BIOS.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/drivers/pdm/PDMCC26XX_util.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/ioc.h)
#include DeviceFamily_constructPath(driverlib/rom.h)
#include DeviceFamily_constructPath(driverlib/prcm.h)
#include DeviceFamily_constructPath(driverlib/i2s.h)


/* PDMCC26XX_I2S functions */
void                 PDMCC26XX_I2S_init(PDMCC26XX_I2S_Handle handle);
PDMCC26XX_I2S_Handle PDMCC26XX_I2S_open(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_Params *params);
void                 PDMCC26XX_I2S_close(PDMCC26XX_I2S_Handle handle);
bool                 PDMCC26XX_I2S_startStream(PDMCC26XX_I2S_Handle handle);
bool                 PDMCC26XX_I2S_stopStream(PDMCC26XX_I2S_Handle handle);
bool                 PDMCC26XX_I2S_requestBuffer(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_BufferRequest *bufferRequest);
void                 PDMCC26XX_I2S_releaseBuffer(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_BufferRelease *bufferRelease);

/* Internal functions */
static void PDMCC26XX_I2S_initHw(PDMCC26XX_I2S_Handle handle);
static bool PDMCC26XX_I2S_initIO(PDMCC26XX_I2S_Handle handle);
static void PDMCC26XX_I2S_hwiFxn (UArg arg);
static void PDMCC26XX_I2S_callback(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_StreamNotification *msg);
static void PDMCC26XX_I2S_deallocateBuffers(PDMCC26XX_I2S_Handle handle);
static bool PDMCC26XX_I2S_allocateBuffers(PDMCC26XX_I2S_Handle handle);

/*! Struct that contains an I2S queue element and a pointer to the PDM buffer it is responsible for */
typedef struct {
  Queue_Elem    queueElement;       /*!< Queue element */
  void          *buffer;            /*!< Buffer pointer */
} PDMCC26XX_I2S_QueueNode;

/*! PDMCC26XX_I2S_Config struct defined in the board file */
extern const PDMCC26XX_I2S_Config PDMCC26XX_I2S_config[];

PDMCC26XX_I2S_QueueNode *i2sBlockActive = NULL;     /* Reference to the element which is currently being filled by I2S DMA In */
PDMCC26XX_I2S_QueueNode *i2sBlockNext = NULL;       /* Reference to the next element which will be filled by I2S DMA In */

Queue_Struct i2sBlockEmptyQueueStruct;
Queue_Handle i2sBlockEmptyQueue;

Queue_Struct i2sBlockFullQueueStruct;
Queue_Handle i2sBlockFullQueue;


I2SControlTable g_ControlTable;

/*
 *  ======== PDMCC26XX_I2S_init ========
 *  @pre    Function assumes that the handle is not NULL
 */
void PDMCC26XX_I2S_init(PDMCC26XX_I2S_Handle handle) {
    PDMCC26XX_I2S_Object         *object;

    Assert_isTrue(handle, NULL);

    /* Get the pointer to the object */
    object = handle->object;

    /* Mark the object as available */
    object->isOpen = false;

    /* Make sure struct in driverlib I2S driver is initialized */
    g_pControlTable = &g_ControlTable;
}

/*
 *  ======== PDMCC26XX_I2S_open ========
 *  @pre    Function assumes that the handle is not NULL
 */
PDMCC26XX_I2S_Handle PDMCC26XX_I2S_open(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_Params *params) {
    /* Use union to save on stack allocation */
    HwiP_Params                   hwiParams;
    PDMCC26XX_I2S_Object         *object;
    PDMCC26XX_I2S_HWAttrs const  *hwAttrs;

    Assert_isTrue(params->blockCount >= 3, NULL);
    Assert_isTrue(handle, NULL);

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Disable preemption while checking if the I2S is open. */
    uint32_t key = HwiP_disable();
    /* Check if the I2S is open already with the base addr. */
    if (object->isOpen == true) {
        HwiP_restore(key);

        return (NULL);
    }
    /* Mark the handle as in use */
    object->isOpen = true;
    HwiP_restore(key);

    /* Initialize the I2S object */
    object->requestMode                 = params->requestMode;
    object->requestTimeout              = params->requestTimeout;
    object->mallocFxn                   = params->mallocFxn;
    object->freeFxn                     = params->freeFxn;
    object->blockCount                  = params->blockCount;
    object->blockSizeInSamples          = params->blockSizeInSamples;
    object->currentStream               = params->currentStream;
    object->currentStream->status       = PDMCC26XX_I2S_STREAM_IDLE;

    /* The following are constants that apply to PDM */
    object->sampleRate = -1;                                                  /* If negative then use user configured clock division */
    object->audioClkCfg.wclkDiv = 16;                                            /* I2S Word Clock divider override*/
    object->audioClkCfg.sampleOnPositiveEdge = PDMCC26XX_I2S_SampleEdge_Postive; /* I2S Sample Edge */
    object->audioClkCfg.wclkPhase = PDMCC26XX_I2S_WordClockPhase_Dual;           /* I2S Word Clock Phase */
    object->audioClkCfg.wclkInverted = PDMCC26XX_I2S_ClockSource_Normal;         /* I2S Invert Word Clock */
    object->audioClkCfg.wclkSource = PDMCC26XX_I2S_WordClockSource_Int;          /* I2S Word Clock source */
    object->audioClkCfg.bclkDiv = 47;                                            /* I2S Bit Clock divider override */
    object->audioClkCfg.reserved = 0;
    object->audioClkCfg.bclkSource = PDMCC26XX_I2S_BitClockSource_Int;           /* I2S Bit Clock source */
    object->audioClkCfg.mclkDiv = 6;                                             /* I2S Master Clock divider override */

    object->audioPinCfg.bitFields.ad1Usage = PDMCC26XX_I2S_ADUsageDisabled;      /* I2S AD1 usage (0: Disabled, 1: Input, 2: Output) */
    object->audioPinCfg.bitFields.enableMclkPin = PDMCC26XX_I2S_GENERIC_DISABLED;/* I2S Enable Master clock output on pin */
    object->audioPinCfg.bitFields.reserved = 0;
    object->audioPinCfg.bitFields.ad1NumOfChannels = 0;                          /* I2S AD1 number of channels (1 - 8). !Must match channel mask */
    object->audioPinCfg.bitFields.ad1ChannelMask = PDMCC26XX_I2S_DISABLED_MODE;  /* I2S AD1 Channel Mask */
    object->audioPinCfg.bitFields.ad0Usage = PDMCC26XX_I2S_ADUsageInput;         /* I2S AD0 usage (0: Disabled, 1: Input, 2: Output) */
    object->audioPinCfg.bitFields.enableWclkPin = PDMCC26XX_I2S_GENERIC_DISABLED;/* I2S Enable Word clock output on pin */
    object->audioPinCfg.bitFields.enableBclkPin = PDMCC26XX_I2S_GENERIC_ENABLED; /* I2S Enable Bit clock output on pin */
    object->audioPinCfg.bitFields.ad0NumOfChannels = 2;                          /* I2S AD0 number of channels (1 - 8). !Must match channel mask. \sa PDM_NUM_OF_CHANNELS */
    object->audioPinCfg.bitFields.ad0ChannelMask = PDMCC26XX_I2S_STEREO_MODE;    /* I2S AD0 Channel Mask */

    object->audioFmtCfg.wordLength = PDMCC26XX_I2S_WordLength16;                 /* Number of bits per word (8-24). Exact for single phase, max for dual phase */
    object->audioFmtCfg.sampleEdge = PDMCC26XX_I2S_PositiveEdge;                 /* Data and Word clock is samples, and clocked out, on opposite edges of BCLK */
    object->audioFmtCfg.dualPhase = PDMCC26XX_I2S_SinglePhase;                   /* Selects dual- or single phase format (0: Single, 1: Dual) */
    object->audioFmtCfg.memLen = PDMCC26XX_I2S_MemLen16bit;                      /* Size of each word stored to or loaded from memory (0: 16, 1: 24) */
    object->audioFmtCfg.dataDelay = PDMCC26XX_I2S_FormatLJF;                     /* Number of BCLK perids between a WCLK edge and MSB of the first word in a phase */


    /* Find out how many channels are In and Out respectively */
    uint8_t totalNumberOfChannelsIn = 0;
    totalNumberOfChannelsIn += object->audioPinCfg.bitFields.ad0NumOfChannels;

    object->blockSizeInBytes = (object->blockSizeInSamples * ( (object->audioFmtCfg.memLen) ? 3 : 2 ) * totalNumberOfChannelsIn);

    /* Setup queues now that we now whether they are needed */
    Queue_construct(&i2sBlockFullQueueStruct, NULL);
    i2sBlockFullQueue = Queue_handle(&i2sBlockFullQueueStruct);

    Queue_construct(&i2sBlockEmptyQueueStruct, NULL);
    i2sBlockEmptyQueue = Queue_handle(&i2sBlockEmptyQueueStruct);

    /* Try to allocate memory for the PDM buffers */
    if (!PDMCC26XX_I2S_allocateBuffers(handle)){
        HwiP_restore(key);

        return NULL;
    }

    /* Register power dependency - i.e. power up and enable clock for I2S. */
    Power_setDependency(hwAttrs->powerMngrId);

    /* Configure IOs after hardware has been initialized so that IOs aren't */
    /* toggled unnecessary and make sure it was successful */
    if (!PDMCC26XX_I2S_initIO(handle)) {
        /* Trying to use I2S driver when some other driver or application
        *  has already allocated these pins, error! */

        /* Deallocate all memory used by the driver */
        PDMCC26XX_I2S_deallocateBuffers(handle);

        /* Release power dependency - i.e. potentially power down serial domain. */
        Power_releaseDependency(hwAttrs->powerMngrId);

        /* Mark the module as available */
        key = HwiP_disable();
        object->isOpen = false;
        HwiP_restore(key);

        /* Signal back to application that I2S driver was not succesfully opened */
        return (NULL);
    }

    /* Create the Hwi for this I2S peripheral. */
    HwiP_Params_init(&hwiParams);
    hwiParams.arg = (UArg) handle;
    hwiParams.priority = hwAttrs->intPriority;
    HwiP_construct(&(object->hwi), (int) hwAttrs->intNum, PDMCC26XX_I2S_hwiFxn,
            &hwiParams);

    /* Create a semaphore to block task execution while stopping the stream */
    SemaphoreP_constructBinary(&(object->semStopping), 0);

    /* Check the transfer mode */
    if (object->requestMode == PDMCC26XX_I2S_MODE_BLOCKING) {
        /*
         *  Create a semaphore to block task execution for the duration of the
         *  I2S transfer.  This is a counting semaphore.
         */
        SemaphoreP_construct(&(object->blockComplete), 0, NULL);

        /* Store internal callback function */
        object->callbackFxn = PDMCC26XX_I2S_callback;
    }
    else {
        /* Check to see if a callback function was defined for async mode */
        Assert_isTrue(params->callbackFxn != NULL, NULL);

        /* Save the callback function pointer */
        object->callbackFxn = params->callbackFxn;
    }

    return (handle);
}

/*
 *  ======== PDMCC26XX_I2S_deallocateBuffers ========
 *  @pre    Function assumes that the handle is not NULL
 */
void PDMCC26XX_I2S_deallocateBuffers(PDMCC26XX_I2S_Handle handle){
    PDMCC26XX_I2S_Object            *object;

    Assert_isTrue(handle, NULL);
    Assert_isTrue(i2sBlockEmptyQueue, NULL);
    Assert_isTrue(i2sBlockFullQueue, NULL);

    /* Get the pointer to the object */
    object = handle->object;

    /* If we do not use a critical section, the thread may be pre-empted between the Queue_empty() check and the Queue_dequeue call. */
    uint32_t key = HwiP_disable();

    /* Empty the available queue and free the memory of the queue elements and the buffers */
    while (!Queue_empty(i2sBlockEmptyQueue)) {
        PDMCC26XX_I2S_QueueNode *availableNode = Queue_dequeue(i2sBlockEmptyQueue);
        /* Free up memory used for PCM data buffer */
        object->freeFxn(availableNode->buffer, object->blockSizeInBytes);
        /* Then free up memory used by the queue element */
        object->freeFxn(availableNode, sizeof(PDMCC26XX_I2S_QueueNode));
    }

    /* Empty the ready queue and free the memory of the queue elements and the buffers */
    while (!Queue_empty(i2sBlockFullQueue)) {
        PDMCC26XX_I2S_QueueNode *readyNode = Queue_dequeue(i2sBlockFullQueue);
        /* Free up memory used for PCM data buffer */
        object->freeFxn(readyNode->buffer, object->blockSizeInBytes);
        /* Then free up memory used by the queue element */
        object->freeFxn(readyNode, sizeof(PDMCC26XX_I2S_QueueNode));
    }

    /* Make sure we do not keep dangling pointers alive */
    if (i2sBlockActive){
        /* Free up memory used for PCM data buffer */
        object->freeFxn(i2sBlockActive->buffer, object->blockSizeInBytes);
        /* Then free up memory used by the queue element */
        object->freeFxn(i2sBlockActive, sizeof(PDMCC26XX_I2S_QueueNode));
        i2sBlockActive = NULL;
    }

    /* Make sure we do not keep dangling pointers alive */
    if (i2sBlockNext){
        /* Free up memory used for PCM data buffer */
        object->freeFxn(i2sBlockNext->buffer, object->blockSizeInBytes);
        /* Then free up memory used by the queue element */
        object->freeFxn(i2sBlockNext, sizeof(PDMCC26XX_I2S_QueueNode));
        i2sBlockNext = NULL;
    }

    HwiP_restore(key);
}

/*
 *  ======== PDMCC26XX_I2S_allocateBuffers ========
 *  @pre    Function assumes that the handle is not NULL
 */
bool PDMCC26XX_I2S_allocateBuffers(PDMCC26XX_I2S_Handle handle){
    PDMCC26XX_I2S_Object            *object;

    Assert_isTrue(handle, NULL);
    Assert_isTrue(i2sBlockEmptyQueue, NULL);
    Assert_isTrue(i2sBlockFullQueue, NULL);

    /* Get the pointer to the object */
    object = handle->object;

    Assert_isTrue(object->isOpen, NULL);

    /* Allocate the PDM block buffers and queue elements. The application provided malloc and free functions permit for both static and dynamic allocation.
     * The PDM block buffers are allocated individually. Despite the increased book-keeping overhead incurred for this in the malloc function,
     * the reduced likelyhood of heap fragmentation is worth it. When using a static allocation malloc implementation, there should be no extra cost incurred from
     * requesting block buffers individually. The same code that will be in the allocation function, would otherwise reside in this driver.
     */
    uint8_t i = 0;
    for (i = 0; i < object->blockCount; i++) {
        PDMCC26XX_I2S_QueueNode *tmpNode;
        tmpNode = object->mallocFxn(sizeof(PDMCC26XX_I2S_QueueNode));

        if(tmpNode){
            tmpNode->buffer = object->mallocFxn(object->blockSizeInBytes);
            if(tmpNode->buffer){
                /* Enqueue the PDM block in the available queue if the allocation was successful */
                Queue_put(i2sBlockEmptyQueue, &tmpNode->queueElement);
            }
            else{
                /* Otherwise, free the node memory, unravel all other allocations, and fail the driver initialsation */
                object->freeFxn(tmpNode, sizeof(PDMCC26XX_I2S_QueueNode));
                PDMCC26XX_I2S_deallocateBuffers(handle);
                return false;
            }
        }
        else{
            /* Unravel all other allocations and fail the function */
            PDMCC26XX_I2S_deallocateBuffers(handle);
            return false;
        }
    }

    return true;
}

/*
 *  ======== PDMCC26XX_I2S_close ========
 *  @pre    Function assumes that the handle is not NULL
 */
void PDMCC26XX_I2S_close(PDMCC26XX_I2S_Handle handle) {
    PDMCC26XX_I2S_Object            *object;
    PDMCC26XX_I2S_HWAttrs const     *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    hwAttrs = handle->hwAttrs;
    object = handle->object;

    /* Deallocate pins */
    PIN_close(object->pinHandle);

    /* Disable the I2S */
    I2SDisable(hwAttrs->baseAddr);

    /* Destroy the Hwi */
    HwiP_destruct(&(object->hwi));

    /* Release power dependency on I2S. */
    Power_releaseDependency(hwAttrs->powerMngrId);

    /* Destroy the stopping semaphore */
    SemaphoreP_destruct(&(object->semStopping));

    if (object->requestMode == PDMCC26XX_I2S_MODE_BLOCKING) {
        /* Destroy the block complete semaphore */
        SemaphoreP_destruct(&(object->blockComplete));
    }

    /* Flush any unprocessed I2S data */
    PDMCC26XX_I2S_deallocateBuffers(handle);

    Queue_destruct(&i2sBlockFullQueueStruct);
    Queue_destruct(&i2sBlockEmptyQueueStruct);

    /* Mark the module as available */
    object->isOpen = false;
}

/*
 *  ======== PDMCC26XX_I2S_hwiFxn ========
 *  ISR for the I2S
 */
static void PDMCC26XX_I2S_hwiFxn (UArg arg) {
    PDMCC26XX_I2S_StreamNotification *notification;
    PDMCC26XX_I2S_Object        *object;
    PDMCC26XX_I2S_HWAttrs const *hwAttrs;
    uint32_t                    intStatus;

    /* Get the pointer to the object and hwAttrs */
    object = ((PDMCC26XX_I2S_Handle)arg)->object;
    hwAttrs = ((PDMCC26XX_I2S_Handle)arg)->hwAttrs;

    Assert_isTrue(i2sBlockEmptyQueue, NULL);
    Assert_isTrue(i2sBlockFullQueue, NULL);

    /* Get the interrupt status of the I2S controller */
    intStatus = I2SIntStatus(hwAttrs->baseAddr, true);
    I2SIntClear(hwAttrs->baseAddr, intStatus);

    if (intStatus & I2S_IRQMASK_AIF_DMA_IN) {
        Assert_isTrue(i2sBlockActive, NULL);

        /* Move completed buffer to ready queue */
        Queue_put(i2sBlockFullQueue, &i2sBlockActive->queueElement);
        /* Setup next active buffer */
        i2sBlockActive = i2sBlockNext;
        /* Mark next buffer as empty*/
        i2sBlockNext = NULL;

        if (object->currentStream->status == PDMCC26XX_I2S_STREAM_STOPPING) {
            /* Part of shut down sequence*/
            object->currentStream->status = PDMCC26XX_I2S_STREAM_STOPPED;
        }
        else if (object->currentStream->status != PDMCC26XX_I2S_STREAM_STOPPED) {
            if (!Queue_empty(i2sBlockEmptyQueue)) {
                /* There is an empty buffer available. */
                i2sBlockNext = Queue_get(i2sBlockEmptyQueue);
                object->currentStream->status = PDMCC26XX_I2S_STREAM_BUFFER_READY;
            }
            else {
                /* If there is no empty buffer available, there should be full buffers we could drop. */
                Assert_isTrue(!Queue_empty(i2sBlockFullQueue), NULL);
                /* The PDM driver did not process the buffers in time. The I2S module needs to drop
                 * some old data and notify the PDM driver that it did so.
                 */
                i2sBlockNext = Queue_get(i2sBlockFullQueue);
                object->currentStream->status = PDMCC26XX_I2S_STREAM_BUFFER_DROPPED;
            }
            Assert_isTrue(i2sBlockNext, NULL);

            I2SPointerSet(hwAttrs->baseAddr, true, (uint32_t *)i2sBlockNext->buffer);

            /* Use a temporary stream pointer in case the callback function
            * attempts to perform another PDMCC26XX_I2S_bufferRequest call
            */
            notification = object->currentStream;

            /* Notify caller about availability of buffer */
            object->callbackFxn((PDMCC26XX_I2S_Handle)arg, notification);
        }
    }

    /* Error handling:
    * Overrun in the RX Fifo -> at least one sample in the shift
    * register has been discarded  */
    if (intStatus & I2S_IRQMASK_PTR_ERR) {
        /* disable the interrupt */
        I2SIntDisable(hwAttrs->baseAddr, I2S_INT_PTR_ERR);
        /* Check if we are expecting this interrupt as part of stopping */
        if ( object->currentStream->status == PDMCC26XX_I2S_STREAM_STOPPED ) {
            /* This happened because PDMCC26XX_I2S_stopStream was called for some reason
             * Post the semaphore
             */
            SemaphoreP_post(&(object->semStopping));
        }
        else {
            asm(" NOP");
            object->currentStream->status = PDMCC26XX_I2S_STREAM_ERROR;
            /* Use a temporary stream pointer in case the callback function
            * attempts to perform another PDMCC26XX_I2S_bufferRequest call
            */
            notification = object->currentStream;
            /* Notify caller about availability of buffer */
            object->callbackFxn((PDMCC26XX_I2S_Handle)arg, notification);
        }
    }
}

/*
 *  ======== PDMCC26XX_I2S_startStream ========
 *  @pre    Function assumes that handle is not NULL
 */
bool PDMCC26XX_I2S_startStream(PDMCC26XX_I2S_Handle handle) {
    PDMCC26XX_I2S_Object        *object;
    PDMCC26XX_I2S_HWAttrs const *hwAttrs;

    Assert_isTrue(handle, NULL);
    Assert_isTrue(i2sBlockFullQueue, NULL);
    Assert_isTrue(i2sBlockEmptyQueue, NULL);

    /* Get the pointer to the object and hwAttr*/
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    Assert_isTrue(object->isOpen, NULL);

    /* Disable preemption while checking if a transfer is in progress */
    uint32_t key = HwiP_disable();
    if (object->currentStream->status != PDMCC26XX_I2S_STREAM_IDLE) {
        HwiP_restore(key);

        /* Flag that the transfer failed to start */
        object->currentStream->status = PDMCC26XX_I2S_STREAM_FAILED;

        /* Transfer is in progress */
        return false;
    }

    /* Make sure to flag that a stream is now active */
    object->currentStream->status = PDMCC26XX_I2S_STREAM_STARTED;

    i2sBlockActive = Queue_dequeue(i2sBlockEmptyQueue);
    i2sBlockNext = Queue_dequeue(i2sBlockEmptyQueue);

    /* Configure the hardware module */
    PDMCC26XX_I2S_initHw(handle);

    key = HwiP_disable();
    /* Configuring sample stamp generator will trigger the audio stream to start */
    I2SSampleStampConfigure(hwAttrs->baseAddr, true, false);
    HwiP_restore(key);

    /* Configure buffers */
    I2SBufferConfig(hwAttrs->baseAddr,
                    (uint32_t)(i2sBlockActive->buffer),
                    0U, object->blockSizeInSamples,
                    PDMCC26XX_I2S_DEFAULT_SAMPLE_STAMP_MOD);
    /* Enable the I2S module. This will set first buffer and DMA length */
    I2SEnable(hwAttrs->baseAddr);

    /* Kick off clocks */
    PRCMAudioClockEnable();
    PRCMLoadSet();

    /* Second buffer is then set in hardware after DMA length is set */
    I2SPointerSet(hwAttrs->baseAddr, true, (uint32_t *)i2sBlockNext->buffer);

    HwiP_restore(key);

    /* Enable the RX overrun interrupt in the I2S module */
    I2SIntEnable(hwAttrs->baseAddr, I2S_INT_DMA_IN | I2S_INT_PTR_ERR);

    /* Clear internal pending interrupt flags */
    I2SIntClear(I2S0_BASE, I2S_INT_ALL);

    /* Enable samplestamp */
    I2SSampleStampEnable(hwAttrs->baseAddr);

    /* Clear potential pending I2S interrupt to CM3 */
    HwiP_clearInterrupt(INT_I2S_IRQ);

    /* Enable I2S interrupt to CM3 */
    HwiP_enableInterrupt(INT_I2S_IRQ);

    return true;
}

/*
 *  ======== PDMCC26XX_I2S_stopStream ========
 *  @pre    Function assumes that handle is not NULL
 */
bool PDMCC26XX_I2S_stopStream(PDMCC26XX_I2S_Handle handle) {
    PDMCC26XX_I2S_Object                    *object;
    PDMCC26XX_I2S_HWAttrs const             *hwAttrs;

    Assert_isTrue(handle, NULL);

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    Assert_isTrue(object->isOpen, NULL);

    /* Check if a transfer is in progress */
    uint32_t key = HwiP_disable();

    /* Check if there is an active stream */
    if ( (object->currentStream->status == PDMCC26XX_I2S_STREAM_STOPPING) ||
         (object->currentStream->status == PDMCC26XX_I2S_STREAM_STOPPED) ||
         (object->currentStream->status == PDMCC26XX_I2S_STREAM_IDLE) ) {

        HwiP_restore(key);
        return false;
    }

    /* Begin stopping sequence, if not stopped because of error */
    if (object->currentStream->status != PDMCC26XX_I2S_STREAM_ERROR) {
        object->currentStream->status = PDMCC26XX_I2S_STREAM_STOPPING;

        /* Reenable the interrupt as it may have been disabled during an error*/
        I2SIntEnable(hwAttrs->baseAddr, I2S_INT_PTR_ERR);

        HwiP_restore(key);

        /* Wait for I2S module to complete all buffers*/
        if (SemaphoreP_OK != SemaphoreP_pend(&(object->semStopping), 40000)) {
            object->currentStream->status = PDMCC26XX_I2S_STREAM_FAILED_TO_STOP;
            return false;
        }
    }

    /* restore HWI */
    HwiP_restore(key);

    /* Flush the blockFullQueue and move those elements to the blockEmptyQueue.
     * Since this function shuts down the driver synchronously by letting it run out
     * of buffers, we do not need to worry about losing interesting data.
     */
    while (!Queue_empty(i2sBlockFullQueue)) {
        PDMCC26XX_I2S_QueueNode *tmpNode = Queue_get(i2sBlockFullQueue);
        Queue_put(i2sBlockEmptyQueue, &tmpNode->queueElement);
    }

    /* Disable the I2S module */
    I2SDisable(hwAttrs->baseAddr);

    /* Turn off clocks */
    PRCMAudioClockDisable();
    PRCMLoadSet();

    /* Disable and clear any pending interrupts */
    I2SIntDisable(hwAttrs->baseAddr, I2S_INT_ALL);
    I2SIntClear(hwAttrs->baseAddr, I2S_INT_ALL);
    HwiP_clearInterrupt(INT_I2S_IRQ);
    HwiP_disableInterrupt(INT_I2S_IRQ);

    /* Indicate we are done with this stream */
    object->currentStream->status = PDMCC26XX_I2S_STREAM_IDLE;

    /* Stream was successfully stopped */
    return true;
}

/*
 *  ======== PDMCC26XX_I2S_requestBuffer ========
 *  @pre    Function assumes that stream has started and that bufferRequest is not NULL
 */
bool PDMCC26XX_I2S_requestBuffer(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_BufferRequest *bufferRequest) {
    PDMCC26XX_I2S_Object        *object;
    bool                        retVal = false;

    Assert_isTrue(handle, NULL);
    Assert_isTrue(bufferRequest, NULL);

    /* Get the pointer to the object */
    object = handle->object;

    Assert_isTrue(object->isOpen, NULL);

    if (object->requestMode == PDMCC26XX_I2S_MODE_BLOCKING) {
        if (SemaphoreP_OK != SemaphoreP_pend(&(object->blockComplete),
                    object->requestTimeout)) {
            /* Stop stream, if we experience a timeout */
            PDMCC26XX_I2S_stopStream(handle);

            bufferRequest->status = object->currentStream->status;

            return false;
        }
    };
    bufferRequest->bufferHandleIn = NULL;
    /* When in callback mode we typically expect the user to call this
    * after being notified of available buffers. Hence we may directly
    * check queue and dequeue buffer
    */
    if (!Queue_empty(i2sBlockFullQueue)) {
        PDMCC26XX_I2S_QueueNode *readyNode = Queue_get(i2sBlockFullQueue);
        bufferRequest->bufferIn = readyNode->buffer;
        bufferRequest->status = object->currentStream->status;
        bufferRequest->bufferHandleIn = readyNode;
        retVal = true;
    }


    return retVal;
}

/*
 *  ======== PDMCC26XX_I2S_releaseBuffer ========
 *  @pre    Function assumes bufferRelease contains a valid bufferHandle (identical to
 *          the one provided in the bufferRequest in PDMCC26XX_I2S_requestBuffer
 */
void PDMCC26XX_I2S_releaseBuffer(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_BufferRelease *bufferRelease) {
    Assert_isTrue(handle, NULL);
    Assert_isTrue(bufferRelease, NULL);
    Assert_isTrue(bufferRelease->bufferHandleIn, NULL);
    Assert_isTrue(i2sBlockEmptyQueue, NULL);

    /* Place released buffer back in available queue */
    Queue_put(i2sBlockEmptyQueue, &((PDMCC26XX_I2S_QueueNode *)bufferRelease->bufferHandleIn)->queueElement);
}

/*
 *  ======== PDMCC26XX_I2S_callback ========
 *  Callback function for when the I2S is in PDMCC26XX_I2S_MODE_BLOCKING
 *
 *  @pre    Function assumes that the handle is not NULL
 */
static void PDMCC26XX_I2S_callback(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_StreamNotification *msg) {
    PDMCC26XX_I2S_Object         *object;

    /* Get the pointer to the object */
    object = handle->object;

    /* Post the semaphore */
    SemaphoreP_post(&(object->blockComplete));
}

/*
*  ======== PDMCC26XX_I2S_hwInit ========
*  This functions initializes the I2S hardware module.
*
*  @pre    Function assumes that the I2S handle is pointing to a hardware
*          module which has already been opened.
*/
static void PDMCC26XX_I2S_initHw(PDMCC26XX_I2S_Handle handle) {
    PDMCC26XX_I2S_Object        *object;
    PDMCC26XX_I2S_HWAttrs const *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Disable I2S operation */
    I2SDisable(hwAttrs->baseAddr);

    /* Configure Audio format */
    I2SAudioFormatConfigure(hwAttrs->baseAddr,
                            *(uint32_t *)&object->audioFmtCfg,
                            object->audioFmtCfg.dataDelay);

    /* Configure Channels */
    I2SChannelConfigure(hwAttrs->baseAddr,
                        object->audioPinCfg.driverLibParams.ad0,
                        object->audioPinCfg.driverLibParams.ad1);

    /* Configure Clocks*/
    uint32_t clkCfg = object->audioClkCfg.wclkSource;
    clkCfg |= (object->audioClkCfg.wclkInverted) ? I2S_INVERT_WCLK : 0;
    I2SClockConfigure(hwAttrs->baseAddr, clkCfg);
    uint32_t mstDiv = object->audioClkCfg.mclkDiv;
    uint32_t bitDiv = object->audioClkCfg.bclkDiv;
    uint32_t wordDiv = object->audioClkCfg.wclkDiv;
    clkCfg = (object->audioClkCfg.wclkPhase << PRCM_I2SCLKCTL_WCLK_PHASE_S);
    clkCfg |= (object->audioClkCfg.sampleOnPositiveEdge << PRCM_I2SCLKCTL_SMPL_ON_POSEDGE_S);
    if ( (object->sampleRate >= I2S_SAMPLE_RATE_16K) &&
        (object->sampleRate <= I2S_SAMPLE_RATE_48K)) {
        PRCMAudioClockConfigSet(clkCfg, object->sampleRate);
    }
    else{
        PRCMAudioClockConfigSetOverride(clkCfg, mstDiv, bitDiv, wordDiv);
    }
    /* TODO: Replace this with Driverlib code */
    HWREG(PRCM_BASE + PRCM_O_I2SBCLKSEL) = (object->audioClkCfg.bclkSource & PRCM_I2SBCLKSEL_SRC_M);
    /* Apply configuration */
    PRCMLoadSet();

    /* Disable I2S module interrupts */
    I2SIntDisable(hwAttrs->baseAddr, I2S_INT_ALL);
}

/*
*  ======== PDMCC26XX_I2S_initIO ========
*  This functions initializes the I2S IOs.
*
*  @pre    Function assumes that the I2S handle is pointing to a hardware
*          module which has already been opened.
*/
static bool PDMCC26XX_I2S_initIO(PDMCC26XX_I2S_Handle handle) {
    PDMCC26XX_I2S_Object        *object;
    PDMCC26XX_I2S_HWAttrs const *hwAttrs;
    PIN_Config                  i2sPinTable[6];
    uint8_t                     i = 0;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Configure IOs */
    /* Build local list of pins, allocate through PIN driver and map HW ports */
    if (object->audioPinCfg.bitFields.enableMclkPin) {
      i2sPinTable[i++] = hwAttrs->mclkPin | IOC_STD_OUTPUT;
    }
    if (object->audioPinCfg.bitFields.enableWclkPin) {
      i2sPinTable[i++] = hwAttrs->wclkPin | IOC_STD_OUTPUT;
    }
    if (object->audioPinCfg.bitFields.enableBclkPin) {
      i2sPinTable[i++] = hwAttrs->bclkPin | IOC_STD_OUTPUT;
    }
    if (object->audioPinCfg.bitFields.ad0Usage == PDMCC26XX_I2S_ADUsageInput) {
      i2sPinTable[i++] = hwAttrs->ad0Pin | PIN_INPUT_EN | PIN_NOPULL;
    }
    else if (object->audioPinCfg.bitFields.ad0Usage == PDMCC26XX_I2S_ADUsageOutput) {
      i2sPinTable[i++] = hwAttrs->ad0Pin | IOC_STD_OUTPUT;
    }
    i2sPinTable[i++] = PIN_TERMINATE;

    /* Open and assign pins through pin driver */
    if (!(object->pinHandle = PIN_open(&(object->pinState), i2sPinTable))) {
      return false;
    }

    /* Set IO muxing for the I2S pins */
    if (object->audioPinCfg.bitFields.enableMclkPin) {
        PINCC26XX_setMux(object->pinHandle, hwAttrs->mclkPin,  IOC_PORT_MCU_I2S_MCLK);
    }
    if (object->audioPinCfg.bitFields.enableWclkPin) {
        PINCC26XX_setMux(object->pinHandle, hwAttrs->wclkPin,  IOC_PORT_MCU_I2S_WCLK);
    }
    if (object->audioPinCfg.bitFields.enableBclkPin) {
        PINCC26XX_setMux(object->pinHandle, hwAttrs->bclkPin,  IOC_PORT_MCU_I2S_BCLK);
    }
    if (object->audioPinCfg.bitFields.ad0Usage != PDMCC26XX_I2S_ADUsageDisabled) {
        PINCC26XX_setMux(object->pinHandle, hwAttrs->ad0Pin,  IOC_PORT_MCU_I2S_AD0);
    }

    return true;
}


/*
 *  ======== i2sPostNotify ========
 *  This functions is called to notify the I2S driver of an ongoing transition
 *  out of sleep mode.
 *
 *  @pre    Function assumes that the I2S handle (clientArg) is pointing to a
 *          hardware module which has already been opened.
 */
int i2sPostNotify(char eventType, uint32_t clientArg) {
    PDMCC26XX_I2S_Handle i2sHandle;

    /* Get the pointers to I2S objects */
    i2sHandle = (PDMCC26XX_I2S_Handle) clientArg;

    /* Reconfigure the hardware when returning from standby */
    PDMCC26XX_I2S_initHw(i2sHandle);

    return Power_NOTIFYDONE;
}
