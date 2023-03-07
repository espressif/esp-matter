/*
 * Copyright (c) 2017-2021, Texas Instruments Incorporated
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

#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/sha2/SHA2CC26X2.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_crypto.h)
#include DeviceFamily_constructPath(driverlib/sha2.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/smph.h)

/* Defines and enumerations */
#define SHA2_UNUSED(value)    ((void)(value))

/* Outer and inner padding bytes used in HMAC */
#define HMAC_OPAD_BYTE 0x5C
#define HMAC_IPAD_BYTE 0x36

/* Though the DMA Length Registers are 32-bit, they can only take a 16-bit
 * length while the bits [31:16] are reserved.
 */
#define DMA_MAX_TXN_LENGTH  0xFFFFU

typedef enum {
    SHA2_OperationType_SingleStep,
    SHA2_OperationType_MultiStep,
    SHA2_OperationType_Finalize,
} SHA2_OperationType;

/* Forward declarations */
static uint32_t floorUint32(uint32_t value, uint32_t divider);
static void SHA2_hwiFxn (uintptr_t arg0);
static int_fast16_t SHA2CC26X2_waitForAccess(SHA2_Handle handle);
static int_fast16_t SHA2CC26X2_waitForResult(SHA2_Handle handle);
static void SHA2CC26X2_configureInterrupts(SHA2_Handle handle,
                                           SHA2CC26X2_Object *object,
                                           SHA2CC26X2_HWAttrs const *hwAttrs,
                                           void (*callbackFxn)(uintptr_t arg0));
static void SHA2CC26X2_xorBufferWithByte(uint8_t *buffer,
                                         size_t bufferLength,
                                         uint8_t byte);
static void SHA2CC26X2_cleanUpAfterOperation(SHA2CC26X2_Object *object);
static int_fast16_t SHA2CC26X2_addData(SHA2_Handle handle,
                                       const void* data,
                                       size_t length);
static int_fast16_t SHA2CC26X2_finalize(SHA2_Handle handle, void *digest);
static int_fast16_t SHA2CC26X2_hashData(SHA2_Handle handle,
                                        const void *data,
                                        size_t length,
                                        void *digest);
static void SHA2CC26X2_emptyFinalize(SHA2CC26X2_Object *object,
                                     void *digest);
static void SHA2CC26X2_computeIntermediateHash(SHA2CC26X2_Object *object);

/* Static globals */
static const uint32_t hashModeTable[] = {
    SHA2_MODE_SELECT_SHA224,
    SHA2_MODE_SELECT_SHA256,
    SHA2_MODE_SELECT_SHA384,
    SHA2_MODE_SELECT_SHA512
};

static const uint8_t blockSizeTable[] = {
    SHA2_BLOCK_SIZE_BYTES_224,
    SHA2_BLOCK_SIZE_BYTES_256,
    SHA2_BLOCK_SIZE_BYTES_384,
    SHA2_BLOCK_SIZE_BYTES_512
};

static const uint8_t digestSizeTable[] = {
    SHA2_DIGEST_LENGTH_BYTES_224,
    SHA2_DIGEST_LENGTH_BYTES_256,
    SHA2_DIGEST_LENGTH_BYTES_384,
    SHA2_DIGEST_LENGTH_BYTES_512
};

static const uint8_t intermediateDigestSizeTable[] = {
    SHA2_DIGEST_LENGTH_BYTES_256,
    SHA2_DIGEST_LENGTH_BYTES_256,
    SHA2_DIGEST_LENGTH_BYTES_512,
    SHA2_DIGEST_LENGTH_BYTES_512
};

static const uint8_t *SHA2_data;

/* Used only for single-step operations to track the final destination of
 * the output digest, in case the operation is internally segmented to
 * tackle the limitation in the max length the DMA controller can handle
 * per DMA transaction. */
static void *SHA2_finalDigest;

static uint32_t SHA2_dataBytesRemaining;

static SHA2_OperationType SHA2_operationType;

static bool isInitialized = false;

static bool readIntermediateDigest = false;

/*
 *  ======== floorUint32 helper ========
 */
uint32_t floorUint32(uint32_t value, uint32_t divider) {
    return (value / divider) * divider;
}

/*
 *  ======== SHA2_hwiFxn ========
 */
static void SHA2_hwiFxn(uintptr_t arg0) {
    SHA2CC26X2_Object *object = ((SHA2_Handle)arg0)->object;

    SHA2CC26X2_cleanUpAfterOperation(object);

    /* Ensure all DMA transactions are done before releasing the resources or
     * posting a callback. If there's more data remaining tracked by
     * SHA2_dataBytesRemaining, the operation is not complete yet, unless
     * the remaining data is smaller than a block size in which case it will
     * be buffered in.
     */
    if (!object->operationInProgress) {
        if (object->retainAccessCounter == 0) {
            /*  Grant access for other threads to use the crypto module.
             *  The semaphore must be posted before the callbackFxn to allow the chaining
             *  of operations.
             */
            SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
        }

        Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

        if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_BLOCKING) {
            /* Unblock the pending task to signal that the operation is complete. */
            SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
        }
        else if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_CALLBACK)
        {
            object->callbackFxn((SHA2_Handle)arg0, object->returnStatus);
        }
    }
}

/*
 *  ======== SHA2CC26X2_cleanUpAfterOperation ========
 */
static void SHA2CC26X2_cleanUpAfterOperation(SHA2CC26X2_Object *object) {
    uint32_t blockSize = blockSizeTable[object->hashType];
    uint32_t irqStatus;
    uint32_t key;

    irqStatus = SHA2IntStatusRaw();
    SHA2IntClear(SHA2_RESULT_RDY | SHA2_DMA_IN_DONE | SHA2_DMA_BUS_ERR);

    /*
     * Prevent the following section from being interrupted by
     * SHA2_cancelOperation().
     */
    key = HwiP_disable();

    if (object->operationCanceled) {
        /*
         * If the operation has been canceled we can end here.
         * Cleanup is done by SHA2_cancelOperation()
         */
        HwiP_restore(key);
        return;

    }
    else if (irqStatus & SHA2_DMA_BUS_ERR) {
        /*
         * In the unlikely event of an error we can stop here.
         */
        object->returnStatus = SHA2_STATUS_ERROR;

    }
    else {
        if (readIntermediateDigest == true) {
            /*
             * Last transaction has finished and we need to store an intermediate
             * digest.
             */
            SHA2GetDigest(object->digest,
                          intermediateDigestSizeTable[object->hashType]);

            readIntermediateDigest = false;
        }

        if (SHA2_operationType == SHA2_OperationType_SingleStep) {
            if (SHA2_dataBytesRemaining > blockSize) {
                SHA2CC26X2_computeIntermediateHash(object);

                HwiP_restore(key);
                return;
            }
            else if (SHA2_dataBytesRemaining > 0) {
                /* SHA2ComputeIntermediateHash reads out and writes back
                 * intermediate digest sizes of the the final digest size. For
                 * SHA-224 and SHA-384, this produces incorrect results. Instead,
                 * the digest size of SHA-256 and SHA-512 should be used instead.
                 * This is why we are writing the digest to the accelerator here
                 * first to cover the difference in digest sizes.
                 */
                SHA2SetDigest(object->digest,
                              intermediateDigestSizeTable[object->hashType]);

                SHA2ComputeFinalHash(SHA2_data,
                                     SHA2_finalDigest,
                                     object->digest,
                                     object->bytesProcessed + SHA2_dataBytesRemaining,
                                     SHA2_dataBytesRemaining,
                                     hashModeTable[object->hashType]);

                /* No need to update object->bytesProcessed for finalization
                 * as it would be cleared upon reentry into SHA2CC26X2_cleanUpAfterOperation
                 * and will anyways not be used anymore. */
                SHA2_dataBytesRemaining = 0;
                SHA2_data = NULL;
                readIntermediateDigest = false;

                HwiP_restore(key);
                return;
            }
        }
        else { /* Multi-step operation */
            if (SHA2_dataBytesRemaining >= blockSize) {
                SHA2CC26X2_computeIntermediateHash(object);

                HwiP_restore(key);
                return;
            }
            else if (SHA2_dataBytesRemaining > 0) {
                /* Copy remaining data into buffer if it's a multi-step
                 * operation and only less than a block-size of data is
                 * remaining.
                 */
                memcpy(object->buffer, SHA2_data, SHA2_dataBytesRemaining);
                object->bytesInBuffer += SHA2_dataBytesRemaining;
                SHA2_dataBytesRemaining = 0;
            }
        }
    }

    /*
     * Since we got here, every transaction has been finished
     */
    object->operationInProgress = false;

    /*
     * Reset byte counter if a hash has been finalized
     */
    if (SHA2_operationType != SHA2_OperationType_MultiStep) {
        object->bytesProcessed = 0;
        object->bytesInBuffer = 0;
    }

    HwiP_restore(key);
}

/*
 *  ======== SHA2CC26X2_computeIntermediateHash ========
 *  NOTE: This function should only be used within a critical section.
 */
static void SHA2CC26X2_computeIntermediateHash(SHA2CC26X2_Object *object) {
    uint32_t blockSize = blockSizeTable[object->hashType];
    uint32_t transactionLength;

    /* DMA Controller has a limitation of processing max 0xFFFF bytes per transaction */
    if (SHA2_dataBytesRemaining > DMA_MAX_TXN_LENGTH) {
        transactionLength = floorUint32(DMA_MAX_TXN_LENGTH, blockSize);
    }
    else {
        transactionLength = floorUint32(SHA2_dataBytesRemaining, blockSize);
    }

    /* SHA2ComputeIntermediateHash reads out and writes back
     * intermediate digest sizes of the the final digest size. For
     * SHA-224 and SHA-384, this produces incorrect results. Instead,
     * the digest size of SHA-256 and SHA-512 should be used instead.
     * This is why we are writing the digest to the accelerator here
     * first to cover the difference in digest sizes.
     */
    SHA2SetDigest(object->digest,
                  intermediateDigestSizeTable[object->hashType]);

    SHA2ComputeIntermediateHash(SHA2_data,
                                object->digest,
                                hashModeTable[object->hashType],
                                transactionLength);

    SHA2_dataBytesRemaining -= transactionLength;
    SHA2_data += transactionLength;
    object->bytesProcessed += transactionLength;
    readIntermediateDigest = true;

    return;
}

/*
 *  ======== SHA2CC26X2_waitForAccess ========
 */
static int_fast16_t SHA2CC26X2_waitForAccess(SHA2_Handle handle) {
    SHA2CC26X2_Object *object = handle->object;

    return SemaphoreP_pend(&CryptoResourceCC26XX_accessSemaphore, object->accessTimeout);
}

/*
 *  ======== SHA2CC26X2_waitForResult ========
 */
static int_fast16_t SHA2CC26X2_waitForResult(SHA2_Handle handle){
    SHA2CC26X2_Object *object = handle->object;

    if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_POLLING) {
        do {
            SHA2WaitForIRQFlags(SHA2_RESULT_RDY | SHA2_DMA_BUS_ERR);
            SHA2CC26X2_cleanUpAfterOperation(object);
        } while (object->operationInProgress);

        Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

        if (object->retainAccessCounter == 0) {
            SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
        }

        return object->returnStatus;
    }
    else if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_BLOCKING) {
        SemaphoreP_pend(&CryptoResourceCC26XX_operationSemaphore, (uint32_t)SemaphoreP_WAIT_FOREVER);

        return object->returnStatus;
    }
    else {
        return SHA2_STATUS_SUCCESS;
    }

}

/*
 *  ======== SHA2CC26X2_configureInterrupts ========
 */
static void SHA2CC26X2_configureInterrupts(SHA2_Handle handle,
                                           SHA2CC26X2_Object *object,
                                           SHA2CC26X2_HWAttrs const *hwAttrs,
                                           void (*callbackFxn)(uintptr_t arg0)) {
    /* If we are in SHA2_RETURN_BEHAVIOR_POLLING, we do not want an interrupt to trigger.
     * We need to disable it before kicking off the operation.
     */
    if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_POLLING)  {
        IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);
    }
    else {
        /* We need to set the HWI function and priority since the same physical interrupt is shared by multiple
         * drivers and they all need to coexist. Whenever a driver starts an operation, it
         * registers its HWI callback with the OS.
         */
        HwiP_setFunc(&CryptoResourceCC26XX_hwi, callbackFxn, (uintptr_t)handle);
        HwiP_setPriority(INT_CRYPTO_RESULT_AVAIL_IRQ, hwAttrs->intPriority);

        IntPendClear(INT_CRYPTO_RESULT_AVAIL_IRQ);
        IntEnable(INT_CRYPTO_RESULT_AVAIL_IRQ);
    }
}

/*
 *  ======== SHA2CC26X2_xorBufferWithByte ========
 */
static void SHA2CC26X2_xorBufferWithByte(uint8_t *buffer,
                                         size_t bufferLength,
                                         uint8_t byte) {
    size_t i;

    for (i = 0; i < bufferLength; i++) {
        buffer[i] = buffer[i] ^ byte;
    }
}

/*
 *  ======== SHA2_init ========
 */
void SHA2_init(void) {
    CryptoResourceCC26XX_constructRTOSObjects();

    isInitialized = true;
}

/*
 *  ======== SHA2_construct ========
 */
SHA2_Handle SHA2_construct(SHA2_Config *config, const SHA2_Params *params) {
    SHA2_Handle                 handle;
    SHA2CC26X2_Object           *object;
    uint_fast8_t                key;

    handle = (SHA2_Config*)config;
    object = handle->object;

    key = HwiP_disable();

    if (object->isOpen || !isInitialized) {
        HwiP_restore(key);
        return NULL;
    }

    object->isOpen = true;
    object->operationInProgress = false;
    object->operationCanceled = false;

    HwiP_restore(key);

    if (params == NULL) {
        params = &SHA2_defaultParams;
    }

    DebugP_assert(params->returnBehavior == SHA2_RETURN_BEHAVIOR_CALLBACK ? params->callbackFxn : true);

    object->bytesInBuffer   = 0;
    object->bytesProcessed  = 0;
    object->returnBehavior  = params->returnBehavior;
    object->callbackFxn     = params->callbackFxn;
    object->hashType        = params->hashType;

    if (params->returnBehavior == SHA2_RETURN_BEHAVIOR_BLOCKING) {
        object->accessTimeout = params->timeout;
    } else {
        object->accessTimeout = SemaphoreP_NO_WAIT;
    }

    /* Set power dependency - i.e. power up and enable clock for Crypto (CryptoResourceCC26XX) module. */
    Power_setDependency(PowerCC26XX_PERIPH_CRYPTO);

    return handle;
}

/*
 *  ======== SHA2_close ========
 */
void SHA2_close(SHA2_Handle handle) {
    SHA2CC26X2_Object         *object;
    uintptr_t key;

    DebugP_assert(handle);

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;

    /* If there is still an operation ongoing, abort it now. */
    key = HwiP_disable();
    if (object->operationInProgress) {
        SHA2_cancelOperation(handle);
    }
    object->isOpen = false;
    HwiP_restore(key);

    /* Release power dependency on Crypto Module. */
    Power_releaseDependency(PowerCC26XX_PERIPH_CRYPTO);
}

/*
 *  ======== SHA2_addData ========
 */
int_fast16_t SHA2_addData(SHA2_Handle handle, const void* data, size_t length) {
    /* Try and obtain access to the crypto module */
    if (SHA2CC26X2_waitForAccess(handle) != SemaphoreP_OK) {
        return SHA2_STATUS_RESOURCE_UNAVAILABLE;
    }

    return SHA2CC26X2_addData(handle, data, length);
}

/*
 *  ======== SHA2CC26X2_addData ========
 */
static int_fast16_t SHA2CC26X2_addData(SHA2_Handle handle,
                                       const void* data,
                                       size_t length) {
    SHA2CC26X2_Object *object = handle->object;
    SHA2CC26X2_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t blockSize = blockSizeTable[object->hashType];
    uintptr_t key;

    object->returnStatus = SHA2_STATUS_SUCCESS;
    object->operationCanceled = false;
    SHA2_operationType = SHA2_OperationType_MultiStep;

    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    SHA2CC26X2_configureInterrupts(handle, object, hwAttrs, SHA2_hwiFxn);

    if ((object->bytesInBuffer + length) >= blockSize) {
        /* We have accumulated enough data to start a transaction. Now the question
         * remains whether we have to merge bytes from the data stream into the
         * buffer first. If so, we do that now, then start a transaction.
         * If the buffer is empty, we can start a transaction on the data stream.
         * Once the transaction is finished, we will decide how to follow up,
         * i.e. copy remaining data into the buffer.
         */
        uint32_t transactionLength;
        const uint8_t* transactionStartAddress;

        if (object->bytesInBuffer > 0) {
            uint8_t *bufferTail = &object->buffer[object->bytesInBuffer];
            uint32_t bytesToCopyToBuffer = blockSize - object->bytesInBuffer;
            memcpy(bufferTail, data, bytesToCopyToBuffer);

            /* We reset the value already. That saves a comparison
             * in the ISR handler
             */
            object->bytesInBuffer = 0;

            transactionStartAddress = object->buffer;
            transactionLength       = blockSize;

            SHA2_data = (const uint8_t*)data + bytesToCopyToBuffer;
            SHA2_dataBytesRemaining = length - bytesToCopyToBuffer;
        }
        else {
            transactionStartAddress = data;

            /* DMA Controller has a limitation of processing max 0xFFFF bytes per transaction */
            if (length > DMA_MAX_TXN_LENGTH) {
                transactionLength = floorUint32(DMA_MAX_TXN_LENGTH, blockSize);
            }
            else {
                transactionLength = floorUint32(length, blockSize);
            }

            SHA2_data = (const uint8_t*)data + transactionLength;
            SHA2_dataBytesRemaining = length - transactionLength;
        }

        /*
         * Starting the accelerator and setting the operationInProgress
         * flag must be atomic.
         */
        key = HwiP_disable();

        /*
         * Finally we need to decide whether this is the first hash
         * operation or a follow-up from a previous one.
         */
        if (object->bytesProcessed > 0) {
            /* SHA2ComputeIntermediateHash reads out and writes back
             * intermediate digest sizes of the the final digest size. For
             * SHA-224 and SHA-384, this produces incorrect results. Instead,
             * the digest size of SHA-256 and SHA-512 should be used instead.
             * This is why we are writing the digest to the accelerator here
             * first to cover the difference in digest sizes.
             */
            SHA2SetDigest(object->digest,
                          intermediateDigestSizeTable[object->hashType]);

            SHA2ComputeIntermediateHash(transactionStartAddress,
                                        object->digest,
                                        hashModeTable[object->hashType],
                                        transactionLength);
        }
        else {
            SHA2ComputeInitialHash(transactionStartAddress,
                                   object->digest,
                                   hashModeTable[object->hashType],
                                   transactionLength);
        }

        object->bytesProcessed += transactionLength;
        object->operationInProgress = true;
        readIntermediateDigest = true;
        HwiP_restore(key);

    }
    else {
        /* There is no action required by the hardware. But we kick the
         * interrupt in order to follow the same code path as the other
         * operations.
         */
        uint8_t *bufferTail = &object->buffer[object->bytesInBuffer];
        memcpy(bufferTail, data, length);
        object->bytesInBuffer += length;
        SHA2_dataBytesRemaining = 0;

        /*
         * Asserting the IRQ and setting the operationInProgress
         * flag must be atomic.
         */
        key = HwiP_disable();
        object->operationInProgress = true;
        SHA2IntEnable(SHA2_RESULT_RDY);
        HWREG(CRYPTO_BASE + CRYPTO_O_IRQSET) = SHA2_RESULT_RDY;
        HwiP_restore(key);
    }

    return SHA2CC26X2_waitForResult(handle);
}

/*
 *  ======== SHA2_finalize ========
 */
int_fast16_t SHA2_finalize(SHA2_Handle handle, void *digest) {

    /* Try and obtain access to the crypto module */
    if (SHA2CC26X2_waitForAccess(handle) != SemaphoreP_OK) {
        return SHA2_STATUS_RESOURCE_UNAVAILABLE;
    }

    return SHA2CC26X2_finalize(handle, digest);
}

/*
 *  ======== SHA2CC26X2_finalize ========
 */
static int_fast16_t SHA2CC26X2_finalize(SHA2_Handle handle, void *digest) {
    SHA2CC26X2_Object *object = handle->object;
    SHA2CC26X2_HWAttrs const *hwAttrs = handle->hwAttrs;
    uintptr_t key;

    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    SHA2CC26X2_configureInterrupts(handle, object, hwAttrs, SHA2_hwiFxn);

    object->returnStatus = SHA2_STATUS_SUCCESS;
    object->operationCanceled = false;
    SHA2_operationType = SHA2_OperationType_Finalize;

    /*
     * Starting the accelerator and setting the operationInProgress
     * flag must be atomic.
     */
    key = HwiP_disable();
    object->operationInProgress = true;

    if (object->bytesProcessed == 0) {
        /*
         * Since no hash operation has been performed yet and no intermediate
         * digest is available, we have to perform a full hash operation.
         *
         * No new data can be added during a finalize operation. That means
         * in this case, the total length of all the data added using addData
         * calls is less than a blocksize for the given hashType. So the remaining
         * data is only in the buffer and only that needs to be hashed.
         */
        SHA2ComputeHash(object->buffer,
                        digest,
                        object->bytesInBuffer,
                        hashModeTable[object->hashType]);
    }
    else if (object->bytesInBuffer > 0) {
        uint32_t totalLength = object->bytesProcessed + object->bytesInBuffer;
        uint32_t chunkLength = object->bytesInBuffer;

        /* SHA2ComputeIntermediateHash and SHA2ComputeFinalHash read out and writes
         * back intermediate digest sizes of the the final digest size. For
         * SHA-224 and SHA-384, this produces incorrect results. Instead,
         * the digest size of SHA-256 and SHA-512 should be used instead.
         * This is why we are writing the digest to the accelerator here
         * first to cover the difference in digest sizes.
         */
        SHA2SetDigest(object->digest,
                      intermediateDigestSizeTable[object->hashType]);

        SHA2ComputeFinalHash(object->buffer,
                             digest,
                             object->digest,
                             totalLength,
                             chunkLength,
                             hashModeTable[object->hashType]);
    }
    else {
        SHA2CC26X2_emptyFinalize(object, digest);
    }

    HwiP_restore(key);

    return SHA2CC26X2_waitForResult(handle);
}

/*
 *  ======== SHA2_emptyFinalize ========
 */
static void SHA2CC26X2_emptyFinalize(SHA2CC26X2_Object *object, void *digest) {
    /*
     * The hardware is incapable of finalizing a SHA2 hash without additional
     * data to process. However, SW can create the final block (consisting of
     * pad and an encoding of the message length) and request that block
     * be hashed by the hardware. The resulting hash is the final SHA2 hash.
     *
     * Calculate the length in bits and put it at the end of the dummy
     * finalization block in big endian order
     */
    uint64_t lengthInBits = object->bytesProcessed * 8;
    uint32_t blockSize    = blockSizeTable[object->hashType];
    uint8_t *lengthBytes  = (uint8_t*)&lengthInBits;

    /*
     * Use the existing buffer as scratch pad
     */
    memset(object->buffer, 0, blockSize);

    /*
     * Final block starts with '10000000'.
     */
    object->buffer[0] = 0x80;

    /*
     * The length is written into the end of the finalization block
     * in big endian order. We always write only the last 8 bytes.
     */
    uint32_t i = 0;
    for (i = 0; i < 4; i++) {
        object->buffer[blockSize - 8 + i] = lengthBytes[7 - i];
        object->buffer[blockSize - 4 + i] = lengthBytes[3 - i];
    }

    /*
     * There is no singular function in the driverlib SHA2 interface
     * that can handle this situation. Thus, several lower-level
     * calls to the driverlib SHA2 library as well as direct
     * register access is performed.
     */
    SHA2ClearDigestAvailableFlag();

    if (object->hashType == SHA2_HASH_TYPE_224 ||
        object->hashType == SHA2_HASH_TYPE_256) {
        // Configure DMA to handle shorter digest outputs
        SHA2SelectAlgorithm(SHA2_ALGSEL_SHA256 | SHA2_ALGSEL_TAG);
    } else {
        // Configure DMA to handle longer digest outputs
        SHA2SelectAlgorithm(SHA2_ALGSEL_SHA512 | SHA2_ALGSEL_TAG);
    }

    SHA2IntClear(SHA2_DMA_IN_DONE | SHA2_RESULT_RDY);
    SHA2IntEnable(SHA2_DMA_IN_DONE | SHA2_RESULT_RDY);
    HWREG(CRYPTO_BASE + CRYPTO_O_HASHMODE) = hashModeTable[object->hashType];
    SHA2SetDigest(object->digest,
                  intermediateDigestSizeTable[object->hashType]);
    SHA2StartDMAOperation(object->buffer, blockSize,
                          digest, digestSizeTable[object->hashType]);

    return;
}

/*
 *  ======== SHA2_hashData ========
 */
int_fast16_t SHA2_hashData(SHA2_Handle handle,
                           const void *data,
                           size_t length,
                           void *digest) {

    /* Try and obtain access to the crypto module */
    if (SHA2CC26X2_waitForAccess(handle) != SemaphoreP_OK) {
        return SHA2_STATUS_RESOURCE_UNAVAILABLE;
    }

    return SHA2CC26X2_hashData(handle, data, length, digest);
}

/*
 *  ======== SHA2CC26X2_hashData ========
 */
static int_fast16_t SHA2CC26X2_hashData(SHA2_Handle handle,
                                        const void *data,
                                        size_t length,
                                        void *digest) {
    SHA2CC26X2_Object *object = handle->object;
    SHA2CC26X2_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t blockSize = blockSizeTable[object->hashType];
    uintptr_t key;
    uint32_t transactionLength;

    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    SHA2CC26X2_configureInterrupts(handle, object, hwAttrs, SHA2_hwiFxn);

    SHA2_operationType = SHA2_OperationType_SingleStep;
    SHA2_dataBytesRemaining = 0;

    object->returnStatus = SHA2_STATUS_SUCCESS;
    object->operationCanceled = false;
    object->bytesInBuffer = 0;
    object->bytesProcessed = 0;

    /*
     * Starting the accelerator and setting the operationInProgress
     * flag must be atomic.
     */
    key = HwiP_disable();

    /* DMA Controller has a limitation of processing max 0xFFFF bytes per transaction.
     * Segment the operation internally catering to that limitation to process all the
     * available data successfully. */
    if (length <= DMA_MAX_TXN_LENGTH) {
        SHA2ComputeHash(data,
                        digest,
                        length,
                        hashModeTable[object->hashType]);
    }
    else {
        transactionLength = floorUint32(DMA_MAX_TXN_LENGTH, blockSize);

        SHA2ComputeInitialHash(data,
                               object->digest,
                               hashModeTable[object->hashType],
                               transactionLength);

        SHA2_data = (const uint8_t*)data + transactionLength;
        SHA2_dataBytesRemaining = length - transactionLength;
        object->bytesProcessed += transactionLength;
        readIntermediateDigest = true;
        SHA2_finalDigest = digest;
    }

    object->operationInProgress = true;
    HwiP_restore(key);

    return SHA2CC26X2_waitForResult(handle);
}

/*
 *  ======== SHA2CC26X2_setupHmac ========
 *
 *  This function starts an HMAC operation and computes as much of the
 *  intermediate results as it can using only the key.
 *
 *  HMAC requires concatenation of intermediate results and the application's
 *  message. We do not have the memory to do that kind of concatenation nor
 *  would it be runtime efficient to do that much copying.
 *  Instead, we use segmented hashes to start the computation of the hashes
 *  and then add in each segment without moving it in memory.
 *
 *  We can compute all operations where the keying material is required.
 *  That way, we do not need to store the intermediate keying material
 *  for future use but only store the intermediate hash result.
 *
 *  - It computes the intermediate key, k0 based on the input key's length.
 *  - It starts a segmented hash with the k0^ipad part of
 *    H(k0  ^ipad || message)
 *      - The intermediate output is saved by the SHA2 driver as usual in
 *        SHA2CC26X2_Object.digest
 *  - It starts a segmented hash of the k0^opad part of
 *    H(k0 ^ opad || H(k0 ^ ipad || message))
 *      - The intermediate output is saved in SHA2CC26X2_Object.hmacDigest
 */
int_fast16_t SHA2CC26X2_setupHmac(SHA2_Handle handle, CryptoKey *key) {
    uint8_t xorBuffer[SHA2CC26X2_MAX_BLOCK_SIZE_BYTES];
    SHA2CC26X2_Object *object           = handle->object;
    SHA2CC26X2_HWAttrs const *hwAttrs   = handle->hwAttrs;
    size_t keyLength                    = key->u.plaintext.keyLength;
    uint8_t *keyingMaterial             = key->u.plaintext.keyMaterial;

    /* Since we will be making multiple calls to SHA2 driver APIs, we need
     * to ensure we retain access across those calls. Otherwise another
     * client could come in and start an operation while our later ones are
     * ongoing. This is because the first SHA2 driver API call would
     * release the access semaphore.
     */
    object->retainAccessCounter++;

    /* Put the driver into polling mode. This allows us to implement a linear
     * flow of other calls to SHA2 APIs. Otherwise, we would need to construct
     * a state machine executed and further kicked off from a registered
     * callback function if SHA2_RETURN_BEHAVIOR_CALLBACK were used.
     * This is highly inefficient in terms of code size, complexity, and
     * runtime as the inputs to the hash function are short.
     */
    SHA2_ReturnBehavior originalReturnBehavior = object->returnBehavior;
    object->returnBehavior = SHA2_RETURN_BEHAVIOR_POLLING;
    SHA2CC26X2_configureInterrupts(handle, object, hwAttrs, SHA2_hwiFxn);

    /* Reset segmented processing state to ensure we start a fresh
     * transaction
     */
    object->bytesInBuffer = 0;
    object->bytesProcessed = 0;

    /* Prepare the buffer of the derived key. We set the entire buffer to 0x00
     * so we do not need to pad it to the block size after copying the keying
     * material provided or the hash thereof there.
     */
    memset(xorBuffer, 0x00, blockSizeTable[object->hashType]);

    /* If the keying material fits in the derived key buffer, copy it there.
     * Otherwise, we need to hash it first and copy the digest there. Since
     * We filled the entire buffer with 0x00, we do not need to pad to the block
     * size.
     */
    if (keyLength <= blockSizeTable[object->hashType]) {
        memcpy(xorBuffer, keyingMaterial, keyLength);
    }
    else {
        SHA2CC26X2_hashData(handle,
                            keyingMaterial,
                            keyLength,
                            xorBuffer);
    }

    /* Compute k0 ^ ipad */
    SHA2CC26X2_xorBufferWithByte(xorBuffer,
                                 blockSizeTable[object->hashType],
                                 HMAC_IPAD_BYTE);

    /* Start a hash of k0 ^ ipad.
     * The intermediate result will be stored in the object for later
     * use when the application calls SHA2_addData on its actual message.
     */
    SHA2CC26X2_addData(handle,
                       xorBuffer,
                       blockSizeTable[object->hashType]);


    /* Undo k0 ^ ipad to reconstruct k0. Use the memory of k0 instead
     * of allocating a new copy on the stack to save RAM.
     */
    SHA2CC26X2_xorBufferWithByte(xorBuffer,
                                 blockSizeTable[object->hashType],
                                 HMAC_IPAD_BYTE);

    /* Compute k0 ^ opad. */
    SHA2CC26X2_xorBufferWithByte(xorBuffer,
                                 blockSizeTable[object->hashType],
                                 HMAC_OPAD_BYTE);

    /* Start a hash of k0 ^ opad.
     * We are using driverlib here since using the interal SHA2 driver APIs
     * would corrupt our previously stored intermediate results.
     * This lets us save a second intermediate result.
     */
    SHA2ComputeInitialHash(xorBuffer,
                           object->hmacDigest,
                           hashModeTable[object->hashType],
                           blockSizeTable[object->hashType]);

    SHA2WaitForIRQFlags(SHA2_RESULT_RDY | SHA2_DMA_BUS_ERR);

    SHA2GetDigest(object->hmacDigest,
                  intermediateDigestSizeTable[object->hashType]);

    /* Restore original return behaviour */
    object->returnBehavior = originalReturnBehavior;
    SHA2CC26X2_configureInterrupts(handle, object, hwAttrs, SHA2_hwiFxn);

    object->retainAccessCounter--;
    if (object->retainAccessCounter == 0) {
        SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);

        /* Only call callbackFxn if this is going to be the last sub-operation
         * of this HMAC call.
         */
        if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_CALLBACK) {
            /* Call the callback function provided by the application. */
            object->callbackFxn(handle, SHA2_STATUS_SUCCESS);
        }
    }

    return SHA2_STATUS_SUCCESS;
}

/*
 *  ======== SHA2_setupHmac ========
 */
int_fast16_t SHA2_setupHmac(SHA2_Handle handle, CryptoKey *key) {
    /* Try and obtain access to the crypto module.
     * We will be keeping this for multiple operations
     */
    if (SHA2CC26X2_waitForAccess(handle) != SemaphoreP_OK) {
        return SHA2_STATUS_RESOURCE_UNAVAILABLE;
    }

    return SHA2CC26X2_setupHmac(handle, key);
}

/*
 *  ======== SHA2CC26X2_finalizeHmac ========
 *
 *  This function completes the HMAC operation once all application data
 *  has been added through SHA_addData().
 *
 *  - It finalizes  H((k0 ^ ipad) || data)
 *  - It adds H((k0 ^ ipad) || data) to the previously started hash that already
 *    includes k0 ^ opad.
 *  - It finalizes H(k0 ^ opad || H((k0 ^ ipad) || data))
 */
int_fast16_t SHA2CC26X2_finalizeHmac(SHA2_Handle handle, void *hmac) {
    uint8_t tmpDigest[SHA2CC26X2_MAX_DIGEST_LENGTH_BYTES];
    SHA2CC26X2_Object *object           = handle->object;
    SHA2CC26X2_HWAttrs const *hwAttrs   = handle->hwAttrs;

    SHA2_ReturnBehavior originalReturnBehavior = object->returnBehavior;
    object->returnBehavior = SHA2_RETURN_BEHAVIOR_POLLING;
    SHA2CC26X2_configureInterrupts(handle, object, hwAttrs, SHA2_hwiFxn);

    /* Retain access over multiple SHA2 driver calls */
    object->retainAccessCounter++;

    /* Finalize H((k0 ^ ipad) || data) */
    SHA2CC26X2_finalize(handle, tmpDigest);

    memcpy(object->digest,
           object->hmacDigest,
           intermediateDigestSizeTable[object->hashType]);

    object->bytesProcessed = blockSizeTable[object->hashType];

    SHA2_operationType = SHA2_OperationType_MultiStep;

    /* Add the temporary digest computed earlier to the current digest */
    SHA2CC26X2_addData(handle,
                       tmpDigest,
                       digestSizeTable[object->hashType]);

    object->returnBehavior = originalReturnBehavior;
    SHA2CC26X2_configureInterrupts(handle, object, hwAttrs, SHA2_hwiFxn);

    object->retainAccessCounter--;

    /* Finalize H(k0 ^ opad || H((k0 ^ ipad) || data))
     * Posting of access semaphore and other cleanup handled by
     * SHA2CC26X2_finalize call
     */
    SHA2CC26X2_finalize(handle, hmac);

    return SHA2_STATUS_SUCCESS;
}

/*
 *  ======== SHA2_finalizeHmac ========
 */
int_fast16_t SHA2_finalizeHmac(SHA2_Handle handle, void *hmac) {
    /* Try and obtain access to the crypto module.
     * We will be keeping this for multiple operations
     */
    if (SHA2CC26X2_waitForAccess(handle) != SemaphoreP_OK) {
        return SHA2_STATUS_RESOURCE_UNAVAILABLE;
    }

    return SHA2CC26X2_finalizeHmac(handle, hmac);
}

/*
 *  ======== SHA2_hmac ========
 *
 *  This is practically just a convenience function. Because of the need for
 *  segmented hashes to construct the HMAC without actually allocating memory
 *  to concatenate intermediate results and the message, this function is not
 *  actually faster than an application using the segmented APIs.
 */
int_fast16_t SHA2_hmac(SHA2_Handle handle,
                       CryptoKey *key,
                       const void* data,
                       size_t size,
                       void *hmac) {
    SHA2CC26X2_Object *object           = handle->object;
    SHA2CC26X2_HWAttrs const *hwAttrs   = handle->hwAttrs;

    /* Try and obtain access to the crypto module.
     * We will be keeping this for multiple operations
     */
    if (SHA2CC26X2_waitForAccess(handle) != SemaphoreP_OK) {
        return SHA2_STATUS_RESOURCE_UNAVAILABLE;
    }

    /* Retain access over multiple SHA2 driver calls */
    object->retainAccessCounter++;

    SHA2CC26X2_setupHmac(handle, key);

    /* For now, only polling return behaviour is supported for the main data
     * segment
     * */
    SHA2_ReturnBehavior originalReturnBehavior = object->returnBehavior;
    object->returnBehavior = SHA2_RETURN_BEHAVIOR_POLLING;
    SHA2CC26X2_configureInterrupts(handle, object, hwAttrs, SHA2_hwiFxn);

    /* Add the input message to the hash */
    SHA2CC26X2_addData(handle, data, size);

    object->returnBehavior = originalReturnBehavior;
    SHA2CC26X2_configureInterrupts(handle, object, hwAttrs, SHA2_hwiFxn);

    /* Allow the SHA2CC26X2_finalizeHmac() call below to release the access
     * semaphore when it completes;
     */
    object->retainAccessCounter--;

    return SHA2CC26X2_finalizeHmac(handle, hmac);
}

/*
 *  ======== SHA2_reset ========
 */
void SHA2_reset(SHA2_Handle handle)
{
    SHA2CC26X2_Object *object = (SHA2CC26X2_Object*)handle->object;

    uint32_t key = HwiP_disable();

    if (object->operationInProgress == true)
    {
        SHA2_cancelOperation(handle);
    }

    object->bytesInBuffer  = 0;
    object->bytesProcessed = 0;

    HwiP_restore(key);
}

/*
 *  ======== SHA2_cancelOperation ========
 */
int_fast16_t SHA2_cancelOperation(SHA2_Handle handle) {
    SHA2CC26X2_Object *object         = handle->object;
    uint32_t key;

    key = HwiP_disable();

    if (!object->operationInProgress) {
        HwiP_restore(key);
        return SHA2_STATUS_SUCCESS;
    }

    /* Reset the accelerator. Immediately stops ongoing operations. */
    HWREG(CRYPTO_BASE + CRYPTO_O_SWRESET) = CRYPTO_SWRESET_SW_RESET;

    /* Consume any outstanding interrupts we may have accrued
     * since disabling interrupts.
     */
    IntPendClear(INT_CRYPTO_RESULT_AVAIL_IRQ);

    Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

    object->bytesInBuffer = 0;
    object->bytesProcessed = 0;
    object->operationCanceled = true;
    object->returnStatus = SHA2_STATUS_CANCELED;
    object->retainAccessCounter = 0;

    HwiP_restore(key);

    /*  Grant access for other threads to use the crypto module.
     *  The semaphore must be posted before the callbackFxn to allow the chaining
     *  of operations.
     */
    SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);


    if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_BLOCKING) {
        /* Unblock the pending task to signal that the operation is complete. */
        SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
    }
    else {
        /* Call the callback function provided by the application. */
        object->callbackFxn(handle, SHA2_STATUS_CANCELED);
    }

    return SHA2_STATUS_SUCCESS;
}

int_fast16_t SHA2_setHashType(SHA2_Handle handle, SHA2_HashType type) {

    SHA2CC26X2_Object *object = (SHA2CC26X2_Object*)handle->object;

    if (object->operationInProgress) {
        return SHA2_STATUS_ERROR;
    }

    object->hashType = type;

    return SHA2_STATUS_SUCCESS;
}
