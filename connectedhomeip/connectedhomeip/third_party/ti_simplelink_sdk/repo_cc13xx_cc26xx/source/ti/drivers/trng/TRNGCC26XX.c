/*
 * Copyright (c) 2018-2020, Texas Instruments Incorporated
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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/DebugP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/TRNG.h>
#include <ti/drivers/trng/TRNGCC26XX.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>
#include <ti/drivers/utils/StructRingBuf.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/trng.h)

/* Macros */
#define MIN(x,y)   (((x) < (y)) ?  (x) : (y))

/* Forward declarations */
static void TRNGCC26XX_hwiFxn(uintptr_t arg0);
static int_fast16_t TRNGCC26XX_waitForResult(TRNG_Handle handle);
static void TRNGCC26XX_fillEntropyPool(uint32_t interruptStatus);
static void TRNGCC26XX_copyToClient(TRNGCC26XX_Object *object);
static void TRNGCC26XX_serviceJob(TRNG_Handle handle);
static void TRNGCC26XX_startJob(TRNG_Handle handle);
static void TRNGCC26XX_startTrng(uint32_t intPriority,
                                 bool enableInterrupts,
                                 uint32_t samplesPerCycle);
static void TRNGCC26XX_stopTrng(void);
/* TRNG_getRandom()
 *  The underlying function that executes the operation
 *  of getting random entropy data.
 *
 *  IMPORTANT:
 *  If random data is requested to be output to a CryptoKey, the caller
 *  must ensure both the entropyKey and entropyBuffer are non-null values.
 *  If random data is requested to be output to an array/buffer, the caller
 *  must set the entropyKey field to null and ensure the entropyBuffer field
 *  is not null.
 */
static int_fast16_t TRNG_getRandom(TRNG_Handle handle);

static HwiP_Struct TRNGCC26XX_hwi;

static bool isInitialized = false;

/* Keeps track of whether the TRNG is in ongoing use or if we should start
 * the TRNG.
 */
static volatile bool trngActive = false;

/* Ring buffer that keeps track of each 64-bit block of entropy that we keep
 * as our entropy pool. We use 64-bit blocks because that is the minimum
 * amount of entropy the TRNG hardware can generate. Using a smaller size would
 * only waste generated entropy. This should be fine though because almost all
 * requests should be a multiple of 64 bits; mostly 128 or 256 bits.
 */
static StructRingBuf_Object entropyPool;
static uint64_t entropyPoolBuffer[TRNGCC26XX_ENTROPY_POOL_SIZE] = {0};

/* Queue that keeps track of outstanding client jobs. Callback and blocking
 * jobs are enqueued at the tail while polling jobs are enqueued at the head.
 * This queue is worked off in FIFO order. After which, the driver will
 * begin filling the depleted entropy pool.
 */
static List_List jobList = {0};

/*
 *  ======== TRNGCC26XX_fillEntropyPool ========
 *  Copies freshly generated entropy from the TRNG hardware to the entropy pool.
 */
static void TRNGCC26XX_fillEntropyPool(uint32_t interruptStatus) {
    uint8_t tmpEntropyBuf[TRNGCC26XX_MIN_BYTES_PER_ITERATION];

    ((uint32_t *)tmpEntropyBuf)[0] = TRNGNumberGet(TRNG_LOW_WORD);
    ((uint32_t *)tmpEntropyBuf)[1] = TRNGNumberGet(TRNG_HI_WORD);

    StructRingBuf_put(&entropyPool, tmpEntropyBuf);
}

/*
 *  ======== TRNGCC26XX_copyToClient ========
 *  Copies entropy from the pool to the client destination.
 */
static void TRNGCC26XX_copyToClient(TRNGCC26XX_Object *object) {
    uint8_t tmpEntropyBuf[TRNGCC26XX_MIN_BYTES_PER_ITERATION];
    size_t bytesToCopy = 0;

    /* Do not do anything if this job does not need more entropy */
    while (object->entropyGenerated < object->entropyRequested) {

        /* If there is entropy in the pool, dequeue it and copy to the job's
         * target.
         */
        if (StructRingBuf_get(&entropyPool, tmpEntropyBuf) != -1) {

            bytesToCopy =  MIN(object->entropyRequested - object->entropyGenerated, sizeof(tmpEntropyBuf));

            memcpy(object->entropyBuffer + object->entropyGenerated,
                   tmpEntropyBuf,
                   bytesToCopy);

            object->entropyGenerated += bytesToCopy;
        }
        /* If the pool is depleted, stop the process regardless of if the job
         * is done yet.
         */
        else {
            break;
        }
    }
}

/*
 *  ======== TRNGCC26XX_serviceJob ========
 *  Clean up a completed job and let the application know it is done.
 */
static void TRNGCC26XX_serviceJob(TRNG_Handle handle) {
    TRNGCC26XX_Object *object = handle->object;

    object->returnStatus = TRNG_STATUS_SUCCESS;

    /* Mark the CryptoKey as non-empty */
    if (object->entropyKey != NULL) {
        object->entropyKey->encoding = CryptoKey_PLAINTEXT;
    }

    if (object->returnBehavior == TRNG_RETURN_BEHAVIOR_BLOCKING) {
        SemaphoreP_post(&object->operationSemaphore);
    }
    else if (object->returnBehavior == TRNG_RETURN_BEHAVIOR_CALLBACK) {
        if (object->entropyKey != NULL) {
            object->cryptoKeyCallbackFxn(handle,
                                         object->returnStatus,
                                         object->entropyKey);
        }
        else {
            object->randomBytesCallbackFxn(handle,
                                           object->returnStatus,
                                           object->entropyBuffer,
                                           object->entropyRequested);
        }
    }
}

/*
 *  ======== TRNGCC26XX_hwiFxn ========
 */
static void TRNGCC26XX_hwiFxn (uintptr_t arg0) {
    uint32_t            interruptStatus;
    bool                oldJobComplete = false;
    TRNGCC26XX_Object   *oldObject;
    List_Elem           *oldJob;
    List_Elem           *newJob;
    uintptr_t           key;

    /* We need to perform the TRNG HW -> entropy pool -> job copying atomically.
     * The TRNGCC26XX_hwiFxn() is used by TRNG_generateEntropy() to kick off
     * the entropy generation and deplete the entropy pool from any context.
     * If we did not do this atomically, we might interrupt an ongoing job and
     * change minimum entropy content settings or ISR priorities, call a
     * callback function twice, mess up the queue, etc.
     */
    key = HwiP_disable();

    /* Only access the TRNG hardware if it is already powered and running.
     * Otherwise, we might cause a hard fault.
     */
    if (trngActive == true) {
        interruptStatus = TRNGStatusGet();
        TRNGIntClear(TRNG_NUMBER_READY | TRNG_FRO_SHUTDOWN);

        if (interruptStatus & TRNG_NUMBER_READY) {

            TRNGCC26XX_fillEntropyPool(interruptStatus);

            /* Stop the TRNG. It is fairly inexpensive to start/restart the
             * TRNG after ever 64-bits and simplifies our implementation and
             * makes it easier to keep the TRNG off while not in use to save
             * power.
             */
            TRNGCC26XX_stopTrng();
        }
    }

    /* Handle the head of the job queue. This may be an ongoing job or a job
     * that was just added and the TRNG is idle.
     */
    oldJob = List_head(&jobList);
    if (oldJob != NULL) {
        oldObject = (TRNGCC26XX_Object *)oldJob;

        /* Copy from the pool to the job destination. There may be no entropy
         * in the pool. In that case, this call does nothing.
         */
        TRNGCC26XX_copyToClient(oldObject);

        /* If we managed to fulfill the request either because the pool
         * contained enough entropy to do so immediately or because we just
         * finished generating the last bytes needed, removed the job from the
         * head of the queue and mark the old job as completed.
         */
        if (oldObject->entropyGenerated >= oldObject->entropyRequested) {
            oldJobComplete = true;

            List_remove(&jobList, oldJob);
            oldObject->isEnqueued = false;
        }
    }

    /* Only attempt to start a new job if there is no active job in progress */
    if (trngActive == false) {
        /* Check if there is a job in the queue after we potentially completed
         * the one above.
         */
        newJob = List_head(&jobList);
        if (newJob != NULL) {
            /* If there is another job queued, start it. */
            TRNGCC26XX_startJob(((TRNGCC26XX_Object *)newJob)->handle);
        }
        else if (StructRingBuf_isFull(&entropyPool) == false) {
            /* If no other job is queued, start the TRNG asynchronously to refill
             * the pool.
             */
            TRNGCC26XX_startTrng(~0,
                                 true,
                                 TRNGCC26XX_SAMPLES_PER_CYCLE_DEFAULT);
        }
    }

    HwiP_restore(key);

    /* If we finished the old job above, unblock the job's task in blocking
     * mode or call the callback function in callback mode. This is purposefully
     * done outside the critical section to avoid the callback function being
     * called with interrupts disabled.
     */
    if (oldJobComplete) {
        TRNGCC26XX_serviceJob(((TRNGCC26XX_Object *)oldJob)->handle);
    }
}

/*
 *  ======== TRNGCC26XX_startJob ========
 *  Kick off the TRNG with the settings specified by the job.
 */
static void TRNGCC26XX_startJob(TRNG_Handle handle) {
    TRNGCC26XX_Object *object = handle->object;
    TRNGCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;

    TRNGCC26XX_startTrng(hwAttrs->intPriority,
                         object->returnBehavior != TRNG_RETURN_BEHAVIOR_POLLING,
                         object->samplesPerCycle);
}

/*
 *  ======== TRNGCC26XX_startTrng ========
 *  Start the TRNG with specified settings.
 */
static void TRNGCC26XX_startTrng(uint32_t intPriority,
                                 bool enableInterrupts,
                                 uint32_t samplesPerCycle) {

    trngActive = true;

    Power_setDependency(PowerCC26XX_PERIPH_TRNG);

    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    HwiP_setPriority(INT_TRNG_IRQ, intPriority);

    /* We need to set the HWI function and priority since the same physical interrupt is shared by multiple
     * driver instances and they all need to coexist. Whenever a driver starts an operation, it
     * registers its HWI callback with the OS.
     */
    TRNGIntClear(TRNG_NUMBER_READY | TRNG_FRO_SHUTDOWN);
    if (enableInterrupts) {
        TRNGIntEnable(TRNG_NUMBER_READY | TRNG_FRO_SHUTDOWN);
    }
    else {
        TRNGIntDisable(TRNG_NUMBER_READY | TRNG_FRO_SHUTDOWN);
    }

    /* The first argument copies arg2 when set to zero - this instructs
     * the TRNG to sample exactly samplesPerCycle times. The final argument
     * causes the samples to happen each clock cycle.
     */
    TRNGConfigure(0, samplesPerCycle, 0);
    TRNGEnable();
}

/*
 *  ======== TRNGCC26XX_stopTrng ========
 */
static void TRNGCC26XX_stopTrng(void) {

        TRNGDisable();

        TRNGIntClear(TRNG_NUMBER_READY | TRNG_FRO_SHUTDOWN);

        Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

        Power_releaseDependency(PowerCC26XX_PERIPH_TRNG);

        trngActive = false;
}

/*
 *  ======== TRNG_init ========
 */
void TRNG_init(void) {
    uintptr_t key;

    key = HwiP_disable();

    if (!isInitialized) {

        HwiP_construct(&(TRNGCC26XX_hwi), INT_TRNG_IRQ, TRNGCC26XX_hwiFxn, NULL);

        /* Create the ring buffer that we keep our entropy pool in. It is
         * refilled asynchronously whenever depleted. It used an item-size of
         * 64 bits because the TRNG can only generate at minimum 64 bits at a
         * time.
         */
        StructRingBuf_construct(&entropyPool,
                                entropyPoolBuffer,
                                sizeof(entropyPoolBuffer) / sizeof(entropyPoolBuffer[0]),
                                sizeof(entropyPoolBuffer[0]));

        /* Kick off initial filling of entropy pool. Most of this time is
         * shared with qualification of the SCLK_LF so we do not spend much
         * additional time in IDLE as opposed to STANDBY.
         */
        TRNGCC26XX_startTrng(~0,
                             true,
                             TRNGCC26XX_SAMPLES_PER_CYCLE_DEFAULT);

        isInitialized = true;
    }

    HwiP_restore(key);
}

/*
 *  ======== TRNGCC26XX_construct ========
 */
TRNG_Handle TRNG_construct(TRNG_Config *config, const TRNG_Params *params) {
    TRNG_Handle                 handle;
    TRNGCC26XX_Object           *object;
    TRNGCC26XX_HWAttrs const    *hwAttrs;
    uintptr_t                   key;

    handle = config;
    object = handle->object;
    hwAttrs = handle->hwAttrs;


    key = HwiP_disable();

    if (object->isOpen || !isInitialized) {
        HwiP_restore(key);
        return NULL;
    }

    object->isOpen = true;
    object->isEnqueued = false;

    HwiP_restore(key);

    /* If params are NULL, use defaults */
    if (params == NULL) {
        params = &TRNG_defaultParams;
    }

    /* For callback mode, check if at least one of the
     * callback function pointers is set. */
    if ((object->returnBehavior == TRNG_RETURN_BEHAVIOR_CALLBACK) &&
        (params->cryptoKeyCallbackFxn == NULL) &&
        (params->randomBytesCallbackFxn == NULL)) {
        return NULL;
    }

    object->returnBehavior          = params->returnBehavior;
    object->semaphoreTimeout        = params->timeout;
    object->cryptoKeyCallbackFxn    = params->cryptoKeyCallbackFxn;
    object->randomBytesCallbackFxn  = params->randomBytesCallbackFxn;
    object->handle                  = handle;

    if (hwAttrs->samplesPerCycle >= TRNGCC26XX_SAMPLES_PER_CYCLE_MIN &&
        hwAttrs->samplesPerCycle <= TRNGCC26XX_SAMPLES_PER_CYCLE_MAX) {

        object->samplesPerCycle = hwAttrs->samplesPerCycle;

    } else {
        object->samplesPerCycle = TRNGCC26XX_SAMPLES_PER_CYCLE_DEFAULT;
    }

    SemaphoreP_constructBinary(&object->operationSemaphore, 0);

    return handle;
}

/*
 *  ======== TRNG_close ========
 */
void TRNG_close(TRNG_Handle handle) {
    TRNGCC26XX_Object         *object;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;

    SemaphoreP_destruct(&object->operationSemaphore);

    /* Mark the module as available */
    object->isOpen = false;
}

/*
 *  ======== TRNGCC26XX_waitForResult ========
 */
static int_fast16_t TRNGCC26XX_waitForResult(TRNG_Handle handle) {
    TRNGCC26XX_Object *object = handle->object;
    int_fast16_t status = TRNG_STATUS_ERROR;

    /* If we need to generate more entropy for a request, we will need to
     * work off the job queue to get to the current request. When that is
     * finished, the TRNGCC26XX_hwiFxn() will kick off an asynchronous refilling
     * of the entropy pool.
     */
    if (object->returnBehavior == TRNG_RETURN_BEHAVIOR_POLLING) {

        /* Repeat until we have generated enough entropy. */
        while(object->entropyGenerated < object->entropyRequested) {
            /* Wait until the TRNG has generated 64 bits of entropy */
            do {
                CPUdelay(1);
            }
            while(!(TRNGStatusGet() & (TRNG_NUMBER_READY | TRNG_FRO_SHUTDOWN)));

            TRNGCC26XX_hwiFxn((uintptr_t)NULL);
        }

        status = object->returnStatus;
    }
    /* In blocking mode, we simply pend until the ISR posts our driver's
     * semaphore when our job is complete.
     */
    else if (object->returnBehavior == TRNG_RETURN_BEHAVIOR_BLOCKING) {

        SemaphoreP_pend(&object->operationSemaphore, object->semaphoreTimeout);

        status = object->returnStatus;
    }
    /* In callback mode, we always return with status success from this
     * function.
     */
    else if (object->returnBehavior == TRNG_RETURN_BEHAVIOR_CALLBACK) {
        status = TRNG_STATUS_SUCCESS;
    }

    return status;
}

/*
 *  ======== TRNGCC26XX_setSamplesPerCycle ========
 * samplesPerCycle must be between 2^8 and 2^24 (256 and 16777216)
 */
int_fast16_t TRNGCC26XX_setSamplesPerCycle(TRNG_Handle handle, uint32_t samplesPerCycle) {
    TRNGCC26XX_Object *object = handle->object;

    object->samplesPerCycle = samplesPerCycle;
    return TRNG_STATUS_SUCCESS;
}

/*
 *  ======== TRNG_generateEntropy ========
 */
int_fast16_t TRNG_generateEntropy(TRNG_Handle handle, CryptoKey *entropy) {
    return TRNG_generateKey(handle, entropy);
}

/*
 *  ======== TRNG_generateKey ========
 */
int_fast16_t TRNG_generateKey(TRNG_Handle handle, CryptoKey *entropy) {
    TRNGCC26XX_Object           *object = handle->object;

    if ((entropy == NULL) ||
        (entropy->u.plaintext.keyMaterial == NULL) ||
        (entropy->encoding != CryptoKey_BLANK_PLAINTEXT) ||
        (entropy->u.plaintext.keyLength == 0)) {
        return TRNG_STATUS_INVALID_INPUTS;
    }

    object->entropyGenerated    = 0;
    object->entropyKey          = entropy;
    object->entropyBuffer       = entropy->u.plaintext.keyMaterial;
    object->entropyRequested    = entropy->u.plaintext.keyLength;

    return TRNG_getRandom(handle);
}

/*
 *  ======== TRNG_getRandomBytes ========
 */
int_fast16_t TRNG_getRandomBytes(TRNG_Handle handle,
                                 void *randomBytes,
                                 size_t randomBytesSize) {
    TRNGCC26XX_Object           *object = handle->object;

    if ((randomBytes == NULL) || (randomBytesSize == 0)) {
        return TRNG_STATUS_INVALID_INPUTS;
    }

    object->entropyGenerated    = 0;
    object->entropyKey          = NULL;
    object->entropyBuffer       = randomBytes;
    object->entropyRequested    = randomBytesSize;

    return TRNG_getRandom(handle);
}

/*
 *  ======== TRNG_getRandom ========
 */
static int_fast16_t TRNG_getRandom(TRNG_Handle handle) {
    TRNGCC26XX_Object           *object = handle->object;
    uintptr_t                   key;
    bool                        preemptRefilling;
    bool                        preemptJob;

    /* Initialize returnStatus */
    object->returnStatus        = TRNG_STATUS_ERROR;

    key = HwiP_disable();

    /* We need to turn off the TRNG interrupt if we are about to start a new
     * polling job and the TRNG is already running. Otherwise, we might get
     * into the situation where we start polling in waitForResult but interrupts
     * are turned on. Then it is a race condition whether the loop's
     * TRNGStatusGet() call manages to catch the updated status or the ISR
     * comes in and clears it first. At best, the latter would cause the polling
     * job to require longer than needed. At worst, it could cause a fault.
     */
    if (object->returnBehavior == TRNG_RETURN_BEHAVIOR_POLLING &&
        trngActive == true) {

        TRNGIntClear(TRNG_NUMBER_READY | TRNG_FRO_SHUTDOWN);
        TRNGIntDisable(TRNG_NUMBER_READY | TRNG_FRO_SHUTDOWN);
    }

    /* If we are filling up the entropy pool in the background and are starting
     * a new job with a non-default samplesPerCycle setting, we need to stop
     * the ongoing refill operation and restart with the new entropy setting.
     * Otherwise, we might have to wait up to 5ms until the refill operation
     * completes when we only want the minimum amount of entropy.
     */
    preemptRefilling = object->samplesPerCycle != TRNGCC26XX_SAMPLES_PER_CYCLE_DEFAULT &&
                       List_empty(&jobList) &&
                       trngActive == true;

    /* If we start a job with polling return behaviour from a critical section
     * while a callback job is currently executing, the callback might post a
     * semaphore which globally turns on interrupts and would break the critical
     * section this polling job was called from. Thus, we need to preempt the
     * previously executing job and immediately handle this job to avoid calling
     * any callback functions from within TRNGCC26XX_waitForResult() in this call.
     */
    preemptJob = object->returnBehavior == TRNG_RETURN_BEHAVIOR_POLLING &&
                 !List_empty(&jobList) &&
                 trngActive == true;

    if (preemptRefilling || preemptJob) {
        TRNGCC26XX_stopTrng();
    }

    if (object->isEnqueued == false) {
        if (object->returnBehavior == TRNG_RETURN_BEHAVIOR_POLLING) {
            List_putHead(&jobList, &object->listElement);
        }
        else {
            List_put(&jobList, &object->listElement);
        }
        object->isEnqueued = true;
    }
    else {
        /* Throw an error as a new job was never enqueued even though
         * the getRandom operation was invoked.
         */
        HwiP_restore(key);
        return TRNG_STATUS_ERROR;
    }

    HwiP_restore(key);

    TRNGCC26XX_hwiFxn((uintptr_t)NULL);

    return TRNGCC26XX_waitForResult(handle);
}

/*
 *  ======== TRNG_cancelOperation ========
 */
int_fast16_t TRNG_cancelOperation(TRNG_Handle handle) {
    TRNGCC26XX_Object *object   = handle->object;
    uintptr_t         key;
    List_Elem         *newJob;

    key = HwiP_disable();

    if ((trngActive == false) ||
        (object->isEnqueued == false)) {
        HwiP_restore(key);
        return TRNG_STATUS_SUCCESS;
    }

    if (&object->listElement == List_head(&jobList)) {
        TRNGCC26XX_stopTrng();

        List_remove(&jobList, &object->listElement);

        /* Check if there is a job in the queue after we potentially completed
         * the one above.
         */
        newJob = List_head(&jobList);
        if (newJob != NULL) {
            /* If there is another job queued, start it. */
            TRNGCC26XX_startJob(((TRNGCC26XX_Object *)newJob)->handle);
        }
        else if (StructRingBuf_isFull(&entropyPool) == false) {
            /* If no other job is queued, start the TRNG asynchronously to refill
             * the pool.
             */
            TRNGCC26XX_startTrng(~0,
                                 true,
                                 TRNGCC26XX_SAMPLES_PER_CYCLE_DEFAULT);
        }
    }
    else {
        List_remove(&jobList, &object->listElement);
    }
    object->isEnqueued = false;

    // Clear the entropy generated so far
    memset(object->entropyBuffer, 0, object->entropyRequested);
    object->entropyGenerated = 0;
    object->returnStatus = TRNG_STATUS_CANCELED;

    HwiP_restore(key);

    if (object->returnBehavior == TRNG_RETURN_BEHAVIOR_BLOCKING) {
        SemaphoreP_post(&object->operationSemaphore);
    }
    else if (object->returnBehavior == TRNG_RETURN_BEHAVIOR_CALLBACK) {
        if (object->entropyKey != NULL) {
            object->cryptoKeyCallbackFxn(handle,
                                         object->returnStatus,
                                         object->entropyKey);
        }
        else {
            object->randomBytesCallbackFxn(handle,
                                           object->returnStatus,
                                           object->entropyBuffer,
                                           object->entropyRequested);
        }
    }

    return TRNG_STATUS_SUCCESS;
}
