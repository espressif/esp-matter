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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti/drivers/AESGCM.h>
#include <ti/drivers/aesgcm/AESGCMCC26X4.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>
#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>
#include <ti/drivers/cryptoutils/utils/CryptoUtils.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_crypto.h)
#include DeviceFamily_constructPath(driverlib/aes.h)

#define AES_NON_BLOCK_MULTIPLE_MASK 0x0F

/* Forward declarations */
static void AESGCM_generateIntermediateDigest(AESGCM_Handle handle);
static uint32_t AESGetDefaultGCMCtrlValue(bool encrypt);
static void AESGCM_hwiFxn(uintptr_t arg0);
static void AESGCM_cleanup(AESGCMCC26X4_Object *object);
static void AESGCM_getResult(AESGCM_Handle handle);
static int_fast16_t AESGCM_startOperation(AESGCM_Handle handle,
                                          AESGCM_OneStepOperation *operation,
                                          AESGCM_OperationType operationType);
static int_fast16_t AESGCM_setOperationInProgress(AESGCMCC26X4_Object *object);
static int_fast16_t AESGCM_waitForResult(AESGCM_Handle handle);
static int_fast16_t AESGCM_waitForDMAInDone(AESGCM_Handle handle);
static int_fast16_t AESGCM_setupSegmentedOperation(AESGCM_Handle handle,
                                                   const CryptoKey *key,
                                                   size_t aadLength,
                                                   size_t plaintextLength);
static int_fast16_t AESGCM_addAADInternal(AESGCM_Handle handle, uint8_t *aad, size_t aadLength, AESGCM_Mode direction);
static int_fast16_t AESGCM_addDataInternal(AESGCM_Handle handle,
                                           uint8_t *input,
                                           uint8_t *output,
                                           size_t inputLength,
                                           AESGCM_Mode direction);

/* Static globals */
static bool isInitialized = false;

static void AESGCM_generateIntermediateDigest(AESGCM_Handle handle)
{
    AESGCMCC26X4_Object *object = handle->object;

    uint32_t ctrlVal = AESGetDefaultGCMCtrlValue(object->operationType == AESGCM_OP_TYPE_AAD_ENCRYPT ||
                                                 object->operationType == AESGCM_OP_TYPE_DATA_ENCRYPT ||
                                                 object->operationType == AESGCM_OP_TYPE_FINALIZE_ENCRYPT ||
                                                 object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT);

    /*
     * Setting get_digest allows an intermediate digest to be generated.
     * This is necessary so that the saved context ready bit is set,
     * and the intermediate values can be read.
     */
    AESSetCtrl(ctrlVal | CRYPTO_AESCTL_GET_DIGEST);
}

/* Function for configuring GCM mode in the AESCTL register */
static uint32_t AESGetDefaultGCMCtrlValue(bool encrypt)
{
    uint32_t ctrlVal = 0;

    /*
     * We need to split building ctrlVal into multiple calls.
     * Trying to combine all of the below leads to the compiler removing
     * the GCM flag for some reason.
     * Unlike CCM, GCM CTR only increments the 32-bit counter at the end of
     * the IV not the entire 16-byte IV itself.
     * The CTR width should be 32-bits since IVs of length 12 bytes are
     * only supported for now. The entire IV will be 16 bytes, 12 bytes
     * from the passed-in IV and 4 bytes from the counter.
     */
    ctrlVal = CRYPTO_AESCTL_GCM_M | CRYPTO_AESCTL_CTR | CRYPTO_AESCTL_SAVE_CONTEXT | CRYPTO_AESCTL_CTR_WIDTH_32_BIT;
    ctrlVal |= encrypt ? CRYPTO_AESCTL_DIR : 0;

    return ctrlVal;
}

/*
 *  ======== AESGCM_hwiFxn ========
 */
static void AESGCM_hwiFxn(uintptr_t arg0)
{
    AESGCMCC26X4_Object *object = ((AESGCM_Handle)arg0)->object;

    /* Propagate the DMA error from driverlib to the application */
    uint32_t irqStatus = AESIntStatusRaw();

    if (irqStatus & AES_DMA_BUS_ERR)
    {
        object->returnStatus = AESGCM_STATUS_ERROR;
    }
    else
    {
        /* Generate an intermediate digest if there is leftover data to process */
        if ((object->actualAADLength != object->expectedAADLength) ||
            (object->actualPlaintextLength != object->expectedPlaintextLength))
        {
            AESGCM_generateIntermediateDigest((AESGCM_Handle)arg0);
        }
    }

    uintptr_t interruptKey = HwiP_disable();

    /*
     * Mark that we are done with the operation so that AESGCM_cancelOperation
     * knows not to try canceling.
     */
    object->hwBusy = false;

    if (object->operationType == AESGCM_OP_TYPE_FINALIZE_ENCRYPT ||
        object->operationType == AESGCM_OP_TYPE_FINALIZE_DECRYPT ||
        object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT ||
        object->operationType == AESGCM_OPERATION_TYPE_DECRYPT)
    {
        /* One-shot and finalize operations are done at this point */
        object->operationInProgress = false;
    }

    HwiP_restore(interruptKey);

    AESIntClear(AES_RESULT_RDY | AES_DMA_IN_DONE | AES_DMA_BUS_ERR);

    if (object->returnStatus != AESGCM_STATUS_ERROR)
    {
        /*
         * Read out the intermediate values. If a finalize or one-shot
         * operation is occurring, the tag is read out and verified against
         * a provided MAC or stored
         */
        AESGCM_getResult((AESGCM_Handle)arg0);

        /*
         * Clear the pending interrupt from setting get_digest when generating
         * an intermediate digest
         */
        AESIntClear(AES_RESULT_RDY | AES_DMA_IN_DONE | AES_DMA_BUS_ERR);
        IntPendClear(INT_CRYPTO_RESULT_AVAIL_IRQ);
    }

    /*
     * Handle cleaning up of the operation: Invalidate the key,
     * release power constraint, and post access semaphore to allow
     * callback to chain operations.
     *
     * When the access semaphore is freed during cleanup,
     * if a higher priority ISR shares this driver instance and wishes
     * to start a new operation, it must handle synchronization with
     * the other thread(s) sharing this driver instance to avoid
     * corrupting the driver's object data by starting a new operation
     * before the callback is executed for the current operation.
     */
    AESGCM_cleanup(object);

    if (object->returnBehavior == AESGCM_RETURN_BEHAVIOR_BLOCKING)
    {
        /* Unblock the pending task to signal that the operation is complete. */
        SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
    }
    else
    {
        /* Call the callback function provided by the application. */
        object->callbackFxn((AESGCM_Handle)arg0, object->returnStatus, object->operation, object->operationType);
    }
}

static void AESGCM_cleanup(AESGCMCC26X4_Object *object)
{
    /*
     * Since plaintext keys use two reserved (by convention) slots in the
     * keystore, the slots must be invalidated to prevent its re-use without
     * reloading the key material again.
     */
    AESInvalidateKey(AES_KEY_AREA_6);
    AESInvalidateKey(AES_KEY_AREA_7);

    /*
     * This powers down all sub-modules of the crypto module until needed.
     * It does not power down the crypto module at PRCM level and provides
     * small power savings.
     */
    AESSelectAlgorithm(0x00);

    Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

    /*
     * Grant access for other threads to use the crypto module.
     * The semaphore must be posted before the callbackFxn to allow the
     * chaining of operations.
     */
    if (object->cryptoResourceLocked)
    {
        SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
        object->cryptoResourceLocked = false;
    }
}

static void AESGCM_getResult(AESGCM_Handle handle)
{
    AESGCMCC26X4_Object *object = handle->object;

    /*
     * The ISR is not reached in polling mode, and generating an
     * intermediate digest is necessary if these is more data to process
     */
    if (object->returnBehavior == AESGCM_RETURN_BEHAVIOR_POLLING)
    {
        /* Generate an intermediate digest if there is leftover data to process */
        if ((object->actualAADLength != object->expectedAADLength) ||
            (object->actualPlaintextLength != object->expectedPlaintextLength))
        {
            AESGCM_generateIntermediateDigest(handle);
        }
    }

    /*
     * If we have done a HW operation, read the intermediate values from
     * the HW to successfully continue future operations.
     */
    if (object->continueAADOperation || object->continueDataOperation)
    {
        /* Read the tag, this will clear the saved context ready bit */
        AESReadTag((uint8_t *)object->intermediateTag, AES_BLOCK_SIZE);

        /* Read the block counter */
        object->blockCounter = AESGetBlockCounter();

        if (object->actualAADLength == object->expectedAADLength)
        {
            /*
             * Read the IV, must be read after the tag. Unlike CCM, the IV
             * should only be updated after processing the last chunk of AAD
             * or working with payload data
             */
            AESReadAuthenticationModeIV(object->intermediateIV);

            object->continueAADOperation = false;
        }

        if (object->actualPlaintextLength == object->expectedPlaintextLength)
        {
            object->continueDataOperation = false;
        }
    }

    /* Make sure that the operation can be finalized at this point */
    if ((object->actualAADLength == object->expectedAADLength) &&
        (object->actualPlaintextLength == object->expectedPlaintextLength))
    {
        uint8_t *mac = NULL;
        uint8_t macLength = 0;

        if (object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT ||
            object->operationType == AESGCM_OPERATION_TYPE_DECRYPT)
        {
            mac = object->operation->oneStepOperation.mac;
            macLength = object->operation->oneStepOperation.macLength;
        }
        else if (object->operationType == AESGCM_OP_TYPE_FINALIZE_ENCRYPT ||
                 object->operationType == AESGCM_OP_TYPE_FINALIZE_DECRYPT)
        {
            mac = object->operation->segmentedFinalizeOperation.mac;
            macLength = object->operation->segmentedFinalizeOperation.macLength;
        }

        /*
         * We need to copy / verify the MAC now so that it is not clobbered when we
         * release the CryptoResourceCC26XX_accessSemaphore semaphore.
         */
        if (object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT ||
            object->operationType == AESGCM_OP_TYPE_FINALIZE_ENCRYPT)
        {
            /*
             * If we are encrypting and authenticating a message, we only want to
             * copy the MAC to the target buffer
             */
            memcpy(mac, object->intermediateTag, macLength);
        }
        else if (object->operationType == AESGCM_OPERATION_TYPE_DECRYPT ||
                 object->operationType == AESGCM_OP_TYPE_FINALIZE_DECRYPT)
        {
            /*
             * If we are decrypting and verifying a message, we must now verify that
             * the provided MAC matches the one calculated in the decryption
             * operation.
             */
            bool macValid = CryptoUtils_buffersMatch((uint8_t *)object->intermediateTag, mac, macLength);

            if (!macValid)
            {
                object->returnStatus = AESGCM_STATUS_MAC_INVALID;
            }
        }

        /*
         * Clear intermediate buffers to prevent leakage for one-shot
         * or finalize operations
         */
        if (object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT ||
            object->operationType == AESGCM_OPERATION_TYPE_DECRYPT ||
            object->operationType == AESGCM_OP_TYPE_FINALIZE_ENCRYPT ||
            object->operationType == AESGCM_OP_TYPE_FINALIZE_DECRYPT)
        {
            CryptoUtils_memset(object->intermediateIV,
                               sizeof(object->intermediateIV),
                               0,
                               sizeof(object->intermediateIV));

            CryptoUtils_memset(object->intermediateTag,
                               sizeof(object->intermediateTag),
                               0,
                               sizeof(object->intermediateTag));
        }
    }
}

/*
 *  ======== AESGCM_init ========
 */
void AESGCM_init(void)
{
    CryptoResourceCC26XX_constructRTOSObjects();

    isInitialized = true;
}

/*
 *  ======== AESGCM_construct ========
 */
AESGCM_Handle AESGCM_construct(AESGCM_Config *config, const AESGCM_Params *params)
{
    AESGCM_Handle handle;
    AESGCMCC26X4_Object *object;
    uint_fast8_t key;

    handle = (AESGCM_Handle)config;
    object = handle->object;

    key = HwiP_disable();

    if (!isInitialized || object->isOpen)
    {
        HwiP_restore(key);
        return NULL;
    }

    object->isOpen = true;
    object->operationInProgress = false;
    object->cryptoResourceLocked = false;

    HwiP_restore(key);

    /* If params are NULL, use defaults */
    if (params == NULL)
    {
        params = (AESGCM_Params *)&AESGCM_defaultParams;
    }

    DebugP_assert(params->returnBehavior == AESGCM_RETURN_BEHAVIOR_CALLBACK ? params->callbackFxn : true);

    object->returnBehavior = params->returnBehavior;
    object->callbackFxn = params->callbackFxn;

    if (params->returnBehavior == AESGCM_RETURN_BEHAVIOR_BLOCKING)
    {
        object->semaphoreTimeout = params->timeout;
    }
    else
    {
        object->semaphoreTimeout = SemaphoreP_NO_WAIT;
    }

    /* Set power dependency - i.e. power up and enable clock for Crypto (CryptoResourceCC26XX) module. */
    Power_setDependency(PowerCC26XX_PERIPH_CRYPTO);

    return handle;
}

/*
 *  ======== AESGCM_close ========
 */
void AESGCM_close(AESGCM_Handle handle)
{
    AESGCMCC26X4_Object *object;

    DebugP_assert(handle);

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;

    /* Mark the module as available */
    object->isOpen = false;

    /* Release power dependency on Crypto Module. */
    Power_releaseDependency(PowerCC26XX_PERIPH_CRYPTO);
}

/*
 *  ======== AESGCM_startOperation ========
 */
static int_fast16_t AESGCM_startOperation(AESGCM_Handle handle,
                                          AESGCM_OneStepOperation *operation,
                                          AESGCM_OperationType operationType)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESGCMCC26X4_Object *object = handle->object;

    /* Internally generated ivs aren't supported for now */
    DebugP_assert(!operation->ivInternallyGenerated);

    int_fast16_t result = AESGCM_STATUS_SUCCESS;

    /*
     * Check that there is no operation in progress for this driver
     * instance
     */
    result = AESGCM_setOperationInProgress(object);

    if (result != AESGCM_STATUS_SUCCESS)
    {
        return result;
    }

    SemaphoreP_Status resourceAcquired;

    /* Try and obtain access to the crypto module */
    resourceAcquired = SemaphoreP_pend(&CryptoResourceCC26XX_accessSemaphore, object->semaphoreTimeout);

    if (resourceAcquired != SemaphoreP_OK)
    {
        return AESGCM_STATUS_RESOURCE_UNAVAILABLE;
    }
    else
    {
        object->cryptoResourceLocked = true;
    }

    object->returnStatus = AESGCM_STATUS_SUCCESS;

    result = AESGCM_setLengths(handle, operation->aadLength, operation->inputLength);

    if (result != AESGCM_STATUS_ERROR)
    {
        result = AESGCM_setIV(handle, operation->iv, operation->ivLength);

        if (result != AESGCM_STATUS_ERROR)
        {
            AESGCM_Mode direction = AESGCM_MODE_ENCRYPT;

            if (operationType == AESGCM_OPERATION_TYPE_DECRYPT)
            {
                direction = AESGCM_MODE_DECRYPT;
            }

            object->operationType = operationType;
            object->operation = (AESGCM_OperationUnion *)operation;

            object->key = *(operation->key);

            /* Start processing from a clean slate */
            object->blockCounter = 0;

            object->continueAADOperation = false;
            object->continueDataOperation = false;

            object->actualAADLength = 0;
            object->actualPlaintextLength = 0;

            memset(object->intermediateTag, 0, sizeof(object->intermediateTag));

            if (operation->aadLength > 0)
            {
                /*
                 * If there is remaining payload data, it will be processed
                 * in addAADInternal. One-shot operations must process AAD
                 * and payload data in one go. For one-shot operations, the
                 * access semaphore shouldn't be released in between
                 * processing AAD and payload data since another thread
                 * could take the semaphore and corrupt the Object
                 */
                result = AESGCM_addAADInternal(handle, operation->aad, operation->aadLength, direction);
            }
            else if (operation->inputLength > 0)
            {
                result = AESGCM_addDataInternal(handle,
                                                operation->input,
                                                operation->output,
                                                operation->inputLength,
                                                direction);
            }
        }
    }

    if ((result != AESGCM_STATUS_SUCCESS) && (object->cryptoResourceLocked))
    {
        SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
        object->cryptoResourceLocked = false;
    }

    return result;
}

/*
 *  ======== AESGCM_setOperationInProgress ========
 */
static int_fast16_t AESGCM_setOperationInProgress(AESGCMCC26X4_Object *object)
{
    uintptr_t interruptKey = HwiP_disable();

    if (object->operationInProgress)
    {
        HwiP_restore(interruptKey);

        return AESGCM_STATUS_ERROR;
    }

    object->operationInProgress = true;

    HwiP_restore(interruptKey);

    return AESGCM_STATUS_SUCCESS;
}

/*
 *  ======== AESGCM_waitForResult ========
 */
static int_fast16_t AESGCM_waitForResult(AESGCM_Handle handle)
{
    AESGCMCC26X4_Object *object = handle->object;

    int_fast16_t result = AESGCM_STATUS_ERROR;

    if (object->returnBehavior == AESGCM_RETURN_BEHAVIOR_POLLING)
    {
        /* Wait until the operation is complete and check for DMA errors. */
        if (AESWaitForIRQFlags(AES_RESULT_RDY | AES_DMA_BUS_ERR) & AES_DMA_BUS_ERR)
        {
            object->returnStatus = AESGCM_STATUS_ERROR;
        }
        else
        {
            AESGCM_getResult(handle);
        }

        /*
         * Save the returnStatus prior clearing operationInProgress or
         * releasing the access semaphore in case it's overwritten
         */
        result = object->returnStatus;

        /* Mark that we are done with the operation */
        object->hwBusy = false;

        if (object->operationType == AESGCM_OP_TYPE_FINALIZE_ENCRYPT ||
            object->operationType == AESGCM_OP_TYPE_FINALIZE_DECRYPT ||
            object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT ||
            object->operationType == AESGCM_OPERATION_TYPE_DECRYPT)
        {
            /* One-shot and finalize operations are done at this point */
            object->operationInProgress = false;
        }

        /*
         * Instead of posting the swi to handle cleanup, we will execute
         * the core of the function here
         */
        AESGCM_cleanup(object);
    }
    else if (object->returnBehavior == AESGCM_RETURN_BEHAVIOR_BLOCKING)
    {
        SemaphoreP_pend(&CryptoResourceCC26XX_operationSemaphore, SemaphoreP_WAIT_FOREVER);

        result = object->returnStatus;
    }
    else
    {
        /* Success is always returned in callback mode */
        result = AESGCM_STATUS_SUCCESS;
    }

    return result;
}

/*
 *  ======== AESGCM_waitForDMAInDone ========
 */
static int_fast16_t AESGCM_waitForDMAInDone(AESGCM_Handle handle)
{
    AESGCMCC26X4_Object *object = handle->object;

    int_fast16_t result = AESGCM_STATUS_ERROR;

    /*
     * One-step operations containing both AAD and payload data must poll for
     * the completion of the AAD transfer and then process the payload data
     */
    if (object->returnBehavior == AESGCM_RETURN_BEHAVIOR_POLLING)
    {
        /* Wait until the AAD transfer is complete and check for DMA errors */
        if (AESWaitForIRQFlags(AES_DMA_IN_DONE | AES_DMA_BUS_ERR) & AES_DMA_BUS_ERR)
        {
            object->returnStatus = AESGCM_STATUS_ERROR;
        }
        else
        {
            AESGCM_getResult(handle);
        }

        /*
         * Save the returnStatus prior clearing operationInProgress or
         * releasing the access semaphore in case it's overwritten
         */
        result = object->returnStatus;

        /* Mark that we are done with the operation */
        object->hwBusy = false;

        if (object->operationType == AESGCM_OP_TYPE_FINALIZE_ENCRYPT ||
            object->operationType == AESGCM_OP_TYPE_FINALIZE_DECRYPT ||
            object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT ||
            object->operationType == AESGCM_OPERATION_TYPE_DECRYPT)
        {
            /* One-shot and finalize operations are done at this point */
            object->operationInProgress = false;
        }

        /*
         * Instead of posting the swi to handle cleanup, we will execute
         * the core of the function here
         */
        AESGCM_cleanup(object);
    }
    else if (object->returnBehavior == AESGCM_RETURN_BEHAVIOR_BLOCKING)
    {
        SemaphoreP_pend(&CryptoResourceCC26XX_operationSemaphore, SemaphoreP_WAIT_FOREVER);

        result = object->returnStatus;
    }
    else
    {
        /* Success is always returned in callback mode */
        result = AESGCM_STATUS_SUCCESS;
    }

    return result;
}

/*
 *  ======== AESGCM_oneStepEncrypt ========
 */
int_fast16_t AESGCM_oneStepEncrypt(AESGCM_Handle handle, AESGCM_OneStepOperation *operationStruct)
{

    return AESGCM_startOperation(handle, operationStruct, AESGCM_OPERATION_TYPE_ENCRYPT);
}

/*
 *  ======== AESGCM_oneStepDecrypt ========
 */
int_fast16_t AESGCM_oneStepDecrypt(AESGCM_Handle handle, AESGCM_OneStepOperation *operationStruct)
{

    return AESGCM_startOperation(handle, operationStruct, AESGCM_OPERATION_TYPE_DECRYPT);
}

/*
 *  ======== AESGCM_setupSegmentedOperation ========
 */
static int_fast16_t AESGCM_setupSegmentedOperation(AESGCM_Handle handle,
                                                   const CryptoKey *key,
                                                   size_t aadLength,
                                                   size_t plaintextLength)
{
    /*
     * Not all of the input values will be asserted here since they will be
     * asserted/checked later. Asserts checking the key's validity will occur
     * in addAAD/DataInternal for maintainability purposes. The length input
     * parameters will be checked in setLengths().
     */
    DebugP_assert(handle);

    AESGCMCC26X4_Object *object = handle->object;

    int_fast16_t result = AESGCM_STATUS_SUCCESS;

    /*
     * Check that there is no operation in progress for this driver
     * instance
     */
    result = AESGCM_setOperationInProgress(object);

    if (result != AESGCM_STATUS_ERROR)
    {
        /*
         * If the user doesn't provide the total lengths in the setupXXXX()
         * calls, they must provide the lengths in setLengths().
         */
        object->expectedAADLength = aadLength;
        object->expectedPlaintextLength = plaintextLength;

        /* Start processing from a clean slate */
        object->blockCounter = 0;

        object->continueAADOperation = false;
        object->continueDataOperation = false;

        object->actualAADLength = 0;
        object->actualPlaintextLength = 0;

        object->key = *(key);

        memset(object->intermediateTag, 0, sizeof(object->intermediateTag));

        /* returnStatus is changed in the case of an error or cancellation */
        object->returnStatus = AESGCM_STATUS_SUCCESS;
    }

    return result;
}

/*
 *  ======== AESGCM_setupEncrypt ========
 */
int_fast16_t AESGCM_setupEncrypt(AESGCM_Handle handle,
                                 const CryptoKey *key,
                                 size_t totalAADLength,
                                 size_t totalPlaintextLength)
{
    /* Input values will be asserted in setupSegmentedOperation */
    int_fast16_t result = AESGCM_setupSegmentedOperation(handle, key, totalAADLength, totalPlaintextLength);

    if (result != AESGCM_STATUS_ERROR)
    {
        AESGCMCC26X4_Object *object = handle->object;

        object->operationType = AESGCM_OPERATION_TYPE_ENCRYPT;
    }

    return result;
}

/*
 *  ======== AESGCM_setupDecrypt ========
 */
int_fast16_t AESGCM_setupDecrypt(AESGCM_Handle handle,
                                 const CryptoKey *key,
                                 size_t totalAADLength,
                                 size_t totalPlaintextLength)
{
    /* Input values will be asserted in setupSegmentedOperation */
    int_fast16_t result = AESGCM_setupSegmentedOperation(handle, key, totalAADLength, totalPlaintextLength);
    if (result != AESGCM_STATUS_ERROR)
    {
        AESGCMCC26X4_Object *object = handle->object;

        object->operationType = AESGCM_OPERATION_TYPE_DECRYPT;
    }

    return result;
}

/*
 *  ======== AESGCM_setLengths ========
 */
int_fast16_t AESGCM_setLengths(AESGCM_Handle handle, size_t aadLength, size_t plaintextLength)
{
    DebugP_assert(handle);

    AESGCMCC26X4_Object *object = handle->object;

    /*
     * Don't continue the segmented operation if there
     * was an error or a cancellation
     */
    if (object->returnStatus != AESGCM_STATUS_SUCCESS)
    {
        return object->returnStatus;
    }

    /* This shouldn't be called after addXXX() or finalizeXXX() */
    DebugP_assert(object->operationType == AESGCM_OPERATION_TYPE_DECRYPT ||
                  object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT);

    /* The combined length of AAD and payload data must be non-zero */
    if (aadLength == 0 && plaintextLength == 0)
    {
        return AESGCM_STATUS_ERROR;
    }

    object->expectedAADLength = aadLength;
    object->expectedPlaintextLength = plaintextLength;

    return AESGCM_STATUS_SUCCESS;
}

/*
 *  ======== AESGCM_setIV ========
 */
int_fast16_t AESGCM_setIV(AESGCM_Handle handle, const uint8_t *iv, size_t ivLength)
{
    /* Public accessible functions should check inputs */
    DebugP_assert(handle);
    DebugP_assert(iv);

    AESGCMCC26X4_Object *object = handle->object;

    /*
     * Don't continue the segmented operation if there
     * was an error or a cancellation
     */
    if (object->returnStatus != AESGCM_STATUS_SUCCESS)
    {
        return object->returnStatus;
    }

    /* This shouldn't be called after addXXX() or finalizeXXX() */
    DebugP_assert(object->operationType == AESGCM_OPERATION_TYPE_DECRYPT ||
                  object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT);

    /* Only IVs with a length of 12 bytes are supported for now */
    if (ivLength != 12)
    {
        return AESGCM_STATUS_ERROR;
    }

    object->intermediateIV[0] = ((uint32_t *)iv)[0];
    object->intermediateIV[1] = ((uint32_t *)iv)[1];
    object->intermediateIV[2] = ((uint32_t *)iv)[2];
    /* Set initial counter value to 1.
     * Counter is interpreted as big-endian number of last 4 bytes of IV
     */
    object->intermediateIV[3] = 0x01000000;

    return AESGCM_STATUS_SUCCESS;
}

/*
 *  ======== AESGCM_generateIV ========
 */
int_fast16_t AESGCM_generateIV(AESGCM_Handle handle, uint8_t *iv, size_t ivSize, size_t *ivLength)
{
    /* This feature is not currently supported */
    return AESGCM_STATUS_FEATURE_NOT_SUPPORTED;
}

/*
 *  ======== AESGCM_addAAD ========
 */
int_fast16_t AESGCM_addAAD(AESGCM_Handle handle, AESGCM_SegmentedAADOperation *operation)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESGCMCC26X4_Object *object = handle->object;

    /*
     * Don't continue the segmented operation if there
     * was an error or a cancellation
     */
    if (object->returnStatus != AESGCM_STATUS_SUCCESS)
    {
        return object->returnStatus;
    }

    /* This operation can be called after setup or after addAAD again */
    DebugP_assert(object->operationType == AESGCM_OPERATION_TYPE_DECRYPT ||
                  object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT ||
                  object->operationType == AESGCM_OP_TYPE_AAD_DECRYPT ||
                  object->operationType == AESGCM_OP_TYPE_AAD_ENCRYPT);

    /*
     * The input length must be a non-zero multiple of an AES block size
     * unless you are dealing with the last chunk of AAD
     */
    if (operation->aadLength == 0 ||
        ((operation->aadLength & AES_NON_BLOCK_MULTIPLE_MASK) &&
         object->actualAADLength + operation->aadLength != object->expectedAADLength))
    {
        return AESGCM_STATUS_ERROR;
    }

    /*
     * The total AAD input length must not exceed the total length specified
     * in AESGCM_setLengths() or the setupXXXX() call.
     */
    if (object->actualAADLength + operation->aadLength > object->expectedAADLength)
    {
        return AESGCM_STATUS_ERROR;
    }

    AESGCM_Mode direction = AESGCM_MODE_ENCRYPT;
    AESGCM_OperationType operationType = AESGCM_OP_TYPE_AAD_ENCRYPT;

    if (object->operationType == AESGCM_OPERATION_TYPE_DECRYPT || object->operationType == AESGCM_OP_TYPE_AAD_DECRYPT)
    {
        direction = AESGCM_MODE_DECRYPT;
        operationType = AESGCM_OP_TYPE_AAD_DECRYPT;
    }

    SemaphoreP_Status resourceAcquired;

    /* Try and obtain access to the crypto module */
    resourceAcquired = SemaphoreP_pend(&CryptoResourceCC26XX_accessSemaphore, object->semaphoreTimeout);

    if (resourceAcquired != SemaphoreP_OK)
    {
        return AESGCM_STATUS_RESOURCE_UNAVAILABLE;
    }
    else
    {
        object->cryptoResourceLocked = true;
    }

    object->operationType = operationType;
    object->operation = (AESGCM_OperationUnion *)operation;

    int_fast16_t result = AESGCM_addAADInternal(handle, operation->aad, operation->aadLength, direction);

    if ((result != AESGCM_STATUS_SUCCESS) && (object->cryptoResourceLocked))
    {
        SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
        object->cryptoResourceLocked = false;
    }

    return result;
}

/*
 *  ======== AESGCM_addAADInternal ========
 */
static int_fast16_t AESGCM_addAADInternal(AESGCM_Handle handle, uint8_t *aad, size_t aadLength, AESGCM_Mode direction)
{
    DebugP_assert(handle);
    DebugP_assert(aad && aadLength);

    AESGCMCC26X4_Object *object = handle->object;

    /*
     * The key is provided as an input in setupEncrypt/Decrypt()
     * and within the input operation struct for oneStepEncrypt/Decrypt(),
     * and users should check those inputs.
     * Only plaintext CryptoKeys are supported for now
     */
    DebugP_assert(object->key.encoding == CryptoKey_PLAINTEXT);

    uint16_t keyLength = object->key.u.plaintext.keyLength;
    uint8_t *keyingMaterial = object->key.u.plaintext.keyMaterial;

    /*
     * keyMaterial and keyLength are passed to AESWriteToKeyStore(),
     * which may be in ROM where it won't have asserts so these
     * should be checked in this function.
     */
    DebugP_assert(keyingMaterial);
    DebugP_assert(keyLength == AES_128_KEY_LENGTH_BYTES ||
                  keyLength == AES_192_KEY_LENGTH_BYTES ||
                  keyLength == AES_256_KEY_LENGTH_BYTES);

    AESGCMCC26X4_HWAttrs const *hwAttrs = handle->hwAttrs;

    /*
     * We need to set the HWI function and priority since the same physical
     * interrupt is shared by multiple drivers and they all need to coexist.
     * Whenever a driver starts an operation, it registers its HWI callback
     * with the OS.
     */
    HwiP_setFunc(&CryptoResourceCC26XX_hwi, AESGCM_hwiFxn, (uintptr_t)handle);
    HwiP_setPriority(INT_CRYPTO_RESULT_AVAIL_IRQ, hwAttrs->intPriority);

    /*
     * Load the key from RAM or flash into the key store at a hardcoded and
     * reserved location
     */
    if (AESWriteToKeyStore(keyingMaterial, keyLength, AES_KEY_AREA_6) != AES_SUCCESS)
    {
        return AESGCM_STATUS_ERROR;
    }

    /*
     * We need to disable interrupts here to prevent a race condition in
     * AESWaitForIRQFlags when inputLength == 0. AESWriteToKeyStore() above
     * has enabled the crypto interrupt.
     */
    IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);

    /*
     * Only AES_DMA_IN_DONE is asserted when processing intermediate chunks
     * of AAD. In that case, the interrupt source is expected to be
     * AES_DMA_IN_DONE instead of AES_RESULT_RDY, which is asserted when
     * the crypto result is ready after processing payload data or the very
     * last chunk of data (which could be AAD).
     */
    if ((object->actualAADLength + aadLength == object->expectedAADLength) && object->expectedPlaintextLength == 0)
    {
        /*
         * Disable this interrupt source if this AAD chunk is the very last
         * chunk of data to be processed
         */
        AESIntDisable(AES_DMA_IN_DONE);
    }

    /* Power the AES sub-module of the crypto module */
    AESSelectAlgorithm(AES_ALGSEL_AES);

    /*
     * Load the key from the key store into the internal register banks of
     * the AES sub-module
     */
    if (AESReadFromKeyStore(AES_KEY_AREA_6) != AES_SUCCESS)
    {
        /*
         * Since plaintext keys use two reserved (by convention) slots in the
         * keystore, the slots must be invalidated to prevent its re-use
         * without reloading the key material again.
         */
        AESInvalidateKey(AES_KEY_AREA_6);
        AESInvalidateKey(AES_KEY_AREA_7);

        AESSelectAlgorithm(0);

        return AESGCM_STATUS_ERROR;
    }

    /*
     * Disallow standby. We are about to configure and start the accelerator.
     * Setting the constraint should happen after all opportunities to fail
     * out of the function. This way, we do not need to undo it each time we
     * exit with a failure.
     */
    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    /* Write zeroes or the intermediate tag to the Key3 registers */
    AESCCMSetTag(object->intermediateTag);

    /* Write the initial or intermediate IV */
    AESSetInitializationVector(object->intermediateIV);

    uint32_t ctrlVal = AESGetDefaultGCMCtrlValue(direction == AESGCM_MODE_ENCRYPT);

    if (object->continueAADOperation)
    {
        AESSetCtrl(ctrlVal | CRYPTO_AESCTL_GCM_CCM_CONTINUE_AAD);
    }
    else
    {
        AESSetCtrl(ctrlVal);
    }

    AESSetDataLength(object->expectedPlaintextLength);

    if (object->continueAADOperation)
    {
        AESSetBlockCounter(object->blockCounter);
    }

    AESSetAuthLength(object->expectedAADLength);

    object->hwBusy = true;
    object->actualAADLength += aadLength;
    object->continueAADOperation = true;

    AESStartDMAOperation(aad, aadLength, NULL, 0);

    int_fast16_t result = AESGCM_STATUS_SUCCESS;

    /* Process payload data, if any, for one-shot operations */
    if ((object->operationType == AESGCM_OPERATION_TYPE_DECRYPT ||
         object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT) &&
        object->expectedPlaintextLength > 0)
    {
        /* Wait until the AAD transfer is complete and check for DMA errors */
        if (AESWaitForIRQFlags(AES_DMA_IN_DONE | AES_DMA_BUS_ERR) & AES_DMA_BUS_ERR)
        {
            result = AESGCM_STATUS_ERROR;
            object->hwBusy = false;

            /* Reset the AESCTL register */
            AESSetCtrl(0);

            /*
             * Handle cleaning up of the operation: Invalidate the key,
             * release power constraint, and post access semaphore.
             *
             * The semaphore must be posted after the callback is executed
             * to prevent another thread which shares the same driver instance
             * from starting a new operation and overwriting the object's
             * operation and operationType provided to the callback.
             */
            AESGCM_cleanup(object);
        }

        /* Disable pending interrupt from DMA_IN_DONE */
        IntPendClear(INT_CRYPTO_RESULT_AVAIL_IRQ);

        if (result != AESGCM_STATUS_ERROR)
        {
            object->actualPlaintextLength += object->operation->oneStepOperation.inputLength;
            object->continueDataOperation = true;

            AESStartDMAOperation(object->operation->oneStepOperation.input,
                                 object->operation->oneStepOperation.inputLength,
                                 object->operation->oneStepOperation.output,
                                 object->operation->oneStepOperation.inputLength);
        }
    }

    if (result != AESGCM_STATUS_ERROR)
    {
        /*
         * If we are in AESGCM_RETURN_BEHAVIOR_POLLING,
         * we do not want an interrupt to trigger.
         */
        if (object->returnBehavior != AESGCM_RETURN_BEHAVIOR_POLLING)
        {
            IntEnable(INT_CRYPTO_RESULT_AVAIL_IRQ);
        }

        if ((object->actualAADLength == object->expectedAADLength) &&
            (object->expectedPlaintextLength == object->actualPlaintextLength))
        {
            result = AESGCM_waitForResult(handle);
        }
        else
        {
            result = AESGCM_waitForDMAInDone(handle);
        }
    }

    return result;
}

/*
 *  ======== AESGCM_addData ========
 */
int_fast16_t AESGCM_addData(AESGCM_Handle handle, AESGCM_SegmentedDataOperation *operation)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESGCMCC26X4_Object *object = handle->object;

    /*
     * Don't continue the segmented operation if there
     * was an error or a cancellation
     */
    if (object->returnStatus != AESGCM_STATUS_SUCCESS)
    {
        return object->returnStatus;
    }

    /* This operation can be called after setupXXXX, addAAD, or addData. */
    DebugP_assert(object->operationType != AESGCM_OP_TYPE_FINALIZE_DECRYPT ||
                  object->operationType != AESGCM_OP_TYPE_FINALIZE_ENCRYPT);

    /*
     * The input length must be a non-zero multiple of an AES block size
     * unless you are dealing with the last chunk of payload data
     */
    if (operation->inputLength == 0 ||
        ((operation->inputLength & AES_NON_BLOCK_MULTIPLE_MASK) &&
         object->actualPlaintextLength + operation->inputLength != object->expectedPlaintextLength))
    {
        return AESGCM_STATUS_ERROR;
    }

    /*
     * The AAD input length specified so far must match the total length
     * specified in the setLengths() or setupXXXX() calls.
     * All AAD input must be processed at this point.
     */
    if (object->actualAADLength != object->expectedAADLength)
    {
        return AESGCM_STATUS_ERROR;
    }

    /*
     * The total input length must not exceed the lengths specified in
     * AESGCM_setLengths() or setupXXXX().
     */
    if (object->actualPlaintextLength + operation->inputLength > object->expectedPlaintextLength)
    {
        return AESGCM_STATUS_ERROR;
    }

    AESGCM_Mode direction = AESGCM_MODE_ENCRYPT;
    AESGCM_OperationType operationType = AESGCM_OP_TYPE_DATA_ENCRYPT;

    if (object->operationType == AESGCM_OPERATION_TYPE_DECRYPT ||
        object->operationType == AESGCM_OP_TYPE_AAD_DECRYPT ||
        object->operationType == AESGCM_OP_TYPE_DATA_DECRYPT)
    {
        direction = AESGCM_MODE_DECRYPT;
        operationType = AESGCM_OP_TYPE_DATA_DECRYPT;
    }

    SemaphoreP_Status resourceAcquired;

    /* Try and obtain access to the crypto module */
    resourceAcquired = SemaphoreP_pend(&CryptoResourceCC26XX_accessSemaphore, object->semaphoreTimeout);

    if (resourceAcquired != SemaphoreP_OK)
    {
        return AESGCM_STATUS_RESOURCE_UNAVAILABLE;
    }
    else
    {
        object->cryptoResourceLocked = true;
    }

    object->operationType = operationType;
    object->operation = (AESGCM_OperationUnion *)operation;

    int_fast16_t result = AESGCM_addDataInternal(handle,
                                                 operation->input,
                                                 operation->output,
                                                 operation->inputLength,
                                                 direction);

    if ((result != AESGCM_STATUS_SUCCESS) && (object->cryptoResourceLocked))
    {
        SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
        object->cryptoResourceLocked = false;
    }

    return result;
}

/*
 *  ======== AESGCM_addDataInternal ========
 */
static int_fast16_t AESGCM_addDataInternal(AESGCM_Handle handle,
                                           uint8_t *input,
                                           uint8_t *output,
                                           size_t inputLength,
                                           AESGCM_Mode direction)
{
    DebugP_assert(handle);
    DebugP_assert(input && output && inputLength);

    AESGCMCC26X4_Object *object = handle->object;

    /*
     * The key is provided as an input in setupEncrypt/Decrypt()
     * and within the input operation struct for oneStepEncrypt/Decrypt(),
     * and users should check those inputs.
     * Only plaintext CryptoKeys are supported for now
     */
    DebugP_assert(object->key.encoding == CryptoKey_PLAINTEXT);

    uint16_t keyLength = object->key.u.plaintext.keyLength;
    uint8_t *keyingMaterial = object->key.u.plaintext.keyMaterial;

    /*
     * keyMaterial and keyLength are passed to AESWriteToKeyStore(),
     * which may be in ROM where it won't have asserts so these
     * should be checked in this function.
     */
    DebugP_assert(keyingMaterial);
    DebugP_assert(keyLength == AES_128_KEY_LENGTH_BYTES ||
                  keyLength == AES_192_KEY_LENGTH_BYTES ||
                  keyLength == AES_256_KEY_LENGTH_BYTES);

    AESGCMCC26X4_HWAttrs const *hwAttrs = handle->hwAttrs;

    /*
     * We need to set the HWI function and priority since the same physical
     * interrupt is shared by multiple drivers and they all need to coexist.
     * Whenever a driver starts an operation, it registers its HWI
     * callback with the OS.
     */
    HwiP_setFunc(&CryptoResourceCC26XX_hwi, AESGCM_hwiFxn, (uintptr_t)handle);
    HwiP_setPriority(INT_CRYPTO_RESULT_AVAIL_IRQ, hwAttrs->intPriority);

    /*
     * Load the key from RAM or flash into the key store at a hardcoded
     * and reserved location
     */
    if (AESWriteToKeyStore(keyingMaterial, keyLength, AES_KEY_AREA_6) != AES_SUCCESS)
    {
        return AESGCM_STATUS_ERROR;
    }

    /*
     * We need to disable interrupts here to prevent a race condition in
     * AESWaitForIRQFlags when inputLength == 0. AESWriteToKeyStore() above
     * has enabled the crypto interrupt.
     */
    IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);

    /*
     * Disable AES_DMA_IN_DONE to prevent triggering an interrupt
     * before the crypto result is ready.
     */
    AESIntDisable(AES_DMA_IN_DONE);

    /* Power the AES sub-module of the crypto module */
    AESSelectAlgorithm(AES_ALGSEL_AES);

    /*
     * Load the key from the key store into the internal register banks
     * of the AES sub-module
     */
    if (AESReadFromKeyStore(AES_KEY_AREA_6) != AES_SUCCESS)
    {
        /*
         * Since plaintext keys use two reserved (by convention) slots in
         * the keystore, the slots must be invalidated to prevent its re-use
         * without reloading the key material again.
         */
        AESInvalidateKey(AES_KEY_AREA_6);
        AESInvalidateKey(AES_KEY_AREA_7);

        AESSelectAlgorithm(0);

        return AESGCM_STATUS_ERROR;
    }

    /*
     * Disallow standby. We are about to configure and start the accelerator.
     * Setting the constraint should happen after all opportunities to fail
     * out of the function. This way, we do not need to undo it each time we
     * exit with a failure.
     */
    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    AESCCMSetTag(object->intermediateTag);
    AESSetInitializationVector(object->intermediateIV);

    uint32_t ctrlVal = AESGetDefaultGCMCtrlValue(direction == AESGCM_MODE_ENCRYPT);

    /* You are continuing off an operation if AAD has been processed */
    if (object->continueDataOperation || object->actualAADLength != 0)
    {
        AESSetCtrl(ctrlVal | CRYPTO_AESCTL_GCM_CCM_CONTINUE);
    }
    else
    {
        AESSetCtrl(ctrlVal);
    }

    AESSetDataLength(object->expectedPlaintextLength);

    if (object->continueDataOperation || object->actualAADLength != 0)
    {
        AESSetBlockCounter(object->blockCounter);
    }

    AESSetAuthLength(object->expectedAADLength);

    object->hwBusy = true;
    object->actualPlaintextLength += inputLength;
    object->continueDataOperation = true;

    AESStartDMAOperation(input, inputLength, output, inputLength);

    if (object->returnBehavior != AESGCM_RETURN_BEHAVIOR_POLLING)
    {
        IntEnable(INT_CRYPTO_RESULT_AVAIL_IRQ);
    }

    return AESGCM_waitForResult(handle);
}

/*
 *  ======== AESGCM_finalizeEncrypt ========
 */
int_fast16_t AESGCM_finalizeEncrypt(AESGCM_Handle handle, AESGCM_SegmentedFinalizeOperation *operation)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESGCMCC26X4_Object *object = handle->object;

    /*
     * Don't continue the segmented operation if there
     * was an error or a cancellation
     */
    if (object->returnStatus != AESGCM_STATUS_SUCCESS)
    {
        return object->returnStatus;
    }

    DebugP_assert(object->operationType == AESGCM_OP_TYPE_AAD_ENCRYPT ||
                  object->operationType == AESGCM_OP_TYPE_DATA_ENCRYPT);

    /* All AAD should be processed at this point in time */
    if (object->actualAADLength != object->expectedAADLength)
    {
        return AESGCM_STATUS_ERROR;
    }

    /* Additional payload data can be passed in finalize */
    if (object->actualPlaintextLength + operation->inputLength != object->expectedPlaintextLength)
    {
        return AESGCM_STATUS_ERROR;
    }

    int_fast16_t result = AESGCM_STATUS_SUCCESS;

    if (operation->inputLength > 0)
    {
        SemaphoreP_Status resourceAcquired;

        /* Try and obtain access to the crypto module */
        resourceAcquired = SemaphoreP_pend(&CryptoResourceCC26XX_accessSemaphore, object->semaphoreTimeout);

        if (resourceAcquired != SemaphoreP_OK)
        {
            return AESGCM_STATUS_RESOURCE_UNAVAILABLE;
        }
        else
        {
            object->cryptoResourceLocked = true;
        }

        object->operationType = AESGCM_OP_TYPE_FINALIZE_ENCRYPT;
        object->operation = (AESGCM_OperationUnion *)operation;

        result = AESGCM_addDataInternal(handle,
                                        operation->input,
                                        operation->output,
                                        operation->inputLength,
                                        AESGCM_MODE_ENCRYPT);

        if ((result != AESGCM_STATUS_SUCCESS) && (object->cryptoResourceLocked))
        {
            SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
            object->cryptoResourceLocked = false;
        }
    }
    else
    {
        /*
         * An AAD-only operation has been finalized with no new data.
         * Copy the output tag computed in cleanup() to the destination
         * buffer.
         */
        memcpy(operation->mac, object->intermediateTag, operation->macLength);

        /* Clear intermediate buffers to prevent data leakage */
        CryptoUtils_memset(object->intermediateIV, sizeof(object->intermediateIV), 0, sizeof(object->intermediateIV));

        CryptoUtils_memset(object->intermediateTag,
                           sizeof(object->intermediateTag),
                           0,
                           sizeof(object->intermediateTag));

        /*
         * Save the object's returnStatus in case it's overwritten during
         * setup of a new segmented operation after operationInProgress is
         * cleared
         */
        result = object->returnStatus;

        object->operationInProgress = false;

        if (object->returnBehavior == AESGCM_RETURN_BEHAVIOR_CALLBACK)
        {
            object->callbackFxn(handle, result, (AESGCM_OperationUnion *)operation, AESGCM_OP_TYPE_FINALIZE_ENCRYPT);

            /* Always return success in callback mode */
            result = AESGCM_STATUS_SUCCESS;
        }
    }

    return result;
}

/*
 *  ======== AESGCM_finalizeDecrypt ========
 */
int_fast16_t AESGCM_finalizeDecrypt(AESGCM_Handle handle, AESGCM_SegmentedFinalizeOperation *operation)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESGCMCC26X4_Object *object = handle->object;

    /*
     * Don't continue the segmented operation if there
     * was an error or a cancellation
     */
    if (object->returnStatus != AESGCM_STATUS_SUCCESS)
    {
        return object->returnStatus;
    }

    DebugP_assert(object->operationType == AESGCM_OP_TYPE_AAD_DECRYPT ||
                  object->operationType == AESGCM_OP_TYPE_DATA_DECRYPT);

    /* All AAD should be processed at this point in time */
    if (object->actualAADLength != object->expectedAADLength)
    {
        return AESGCM_STATUS_ERROR;
    }

    /* Additional payload data can be passed in finalize */
    if (object->actualPlaintextLength + operation->inputLength != object->expectedPlaintextLength)
    {
        return AESGCM_STATUS_ERROR;
    }

    int_fast16_t result = AESGCM_STATUS_SUCCESS;

    if (operation->inputLength > 0)
    {
        SemaphoreP_Status resourceAcquired;

        /* Try and obtain access to the crypto module */
        resourceAcquired = SemaphoreP_pend(&CryptoResourceCC26XX_accessSemaphore, object->semaphoreTimeout);

        if (resourceAcquired != SemaphoreP_OK)
        {
            return AESGCM_STATUS_RESOURCE_UNAVAILABLE;
        }
        else
        {
            object->cryptoResourceLocked = true;
        }

        object->operationType = AESGCM_OP_TYPE_FINALIZE_DECRYPT;
        object->operation = (AESGCM_OperationUnion *)operation;

        result = AESGCM_addDataInternal(handle,
                                        operation->input,
                                        operation->output,
                                        operation->inputLength,
                                        AESGCM_MODE_DECRYPT);

        if ((result != AESGCM_STATUS_SUCCESS) && (object->cryptoResourceLocked))
        {
            SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
            object->cryptoResourceLocked = false;
        }
    }
    else
    {
        /*
         * An AAD_only operation has been finalized with no new data.
         * Compare the provided MAC with the MAC that was calculated in
         * cleanup() and stored in Object.
         */
        bool macValid = CryptoUtils_buffersMatch(object->intermediateTag, operation->mac, operation->macLength);

        if (!macValid)
        {
            object->returnStatus = AESGCM_STATUS_MAC_INVALID;
        }

        /* Clear intermediate buffers to prevent data leakage */
        CryptoUtils_memset(object->intermediateIV, sizeof(object->intermediateIV), 0, sizeof(object->intermediateIV));

        CryptoUtils_memset(object->intermediateTag,
                           sizeof(object->intermediateTag),
                           0,
                           sizeof(object->intermediateTag));

        /*
         * Save the object's returnStatus in case it's overwritten during
         * setup of a new segmented operation after operationInProgress is
         * cleared
         */
        result = object->returnStatus;

        object->operationInProgress = false;

        if (object->returnBehavior == AESGCM_RETURN_BEHAVIOR_CALLBACK)
        {
            object->callbackFxn(handle, result, (AESGCM_OperationUnion *)operation, AESGCM_OP_TYPE_FINALIZE_DECRYPT);
        }
    }

    return result;
}

/*
 *  ======== AESGCM_cancelOperation ========
 */
int_fast16_t AESGCM_cancelOperation(AESGCM_Handle handle)
{
    AESGCMCC26X4_Object *object = handle->object;

    uint32_t key;

    key = HwiP_disable();

    /* Cancel is only supported for callback mode */
    if (object->returnBehavior != AESGCM_RETURN_BEHAVIOR_CALLBACK)
    {
        HwiP_restore(key);

        return AESGCM_STATUS_ERROR;
    }

    if (!object->hwBusy)
    {
        object->returnStatus = AESGCM_STATUS_CANCELED;
        object->operationInProgress = false;
        HwiP_restore(key);

        /* Canceling returns success if no HW operations are in progress */
        return AESGCM_STATUS_SUCCESS;
    }

    IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);

    HwiP_restore(key);

    /*
     * When canceling an operation, data associated with
     * the operation should be wiped for security purposes.
     */
    CryptoUtils_memset(object->intermediateIV, sizeof(object->intermediateIV), 0, sizeof(object->intermediateIV));
    CryptoUtils_memset(object->intermediateTag, sizeof(object->intermediateTag), 0, sizeof(object->intermediateTag));

    uint8_t *outputBuffer = NULL;
    size_t outputLength = 0;
    uint8_t *mac = NULL;
    uint8_t macLength = 0;

    /*
     * Clear the output buffer. In-progress one-step
     * and finalize operations should also clear the MAC
     */
    if (object->operationType == AESGCM_OPERATION_TYPE_DECRYPT ||
        object->operationType == AESGCM_OPERATION_TYPE_ENCRYPT)
    {
        outputBuffer = object->operation->oneStepOperation.output;
        outputLength = object->operation->oneStepOperation.inputLength;
        mac = object->operation->oneStepOperation.mac;
        macLength = object->operation->oneStepOperation.macLength;
    }
    else if (object->operationType == AESGCM_OP_TYPE_DATA_ENCRYPT ||
             object->operationType == AESGCM_OP_TYPE_DATA_DECRYPT)
    {
        /*
         * The output is not guaranteed to be contiguous. Therefore, only
         * the passed in output buffer is cleared
         */
        outputBuffer = object->operation->segmentedDataOperation.output;
        outputLength = object->operation->segmentedDataOperation.inputLength;
    }
    else if (object->operationType == AESGCM_OP_TYPE_FINALIZE_DECRYPT ||
             object->operationType == AESGCM_OP_TYPE_FINALIZE_ENCRYPT)
    {
        outputBuffer = object->operation->segmentedFinalizeOperation.output;
        outputLength = object->operation->segmentedFinalizeOperation.inputLength;
        mac = object->operation->segmentedFinalizeOperation.mac;
        macLength = object->operation->segmentedFinalizeOperation.macLength;
    }

    bool clearedBuffer = true;

    int_fast16_t result = AESGCM_STATUS_SUCCESS;

    if (outputBuffer && outputLength)
    {
        if (object->operationType == AESGCM_OP_TYPE_AAD_ENCRYPT || object->operationType == AESGCM_OP_TYPE_AAD_DECRYPT)
        {
            /*
             * Attempting to clear the output during an AAD operation
             * results in a failure
             */
            result = AESGCM_STATUS_ERROR;
        }

        CryptoUtils_memset(outputBuffer, outputLength, 0, outputLength);
        clearedBuffer = CryptoUtils_isBufferAllZeros(outputBuffer, outputLength);

        if (!clearedBuffer)
        {
            result = AESGCM_STATUS_ERROR;
        }
    }

    if (mac && macLength)
    {
        if (object->operationType == AESGCM_OP_TYPE_DATA_ENCRYPT ||
            object->operationType == AESGCM_OP_TYPE_DATA_DECRYPT ||
            object->operationType == AESGCM_OP_TYPE_AAD_ENCRYPT ||
            object->operationType == AESGCM_OP_TYPE_AAD_DECRYPT)
        {
            /*
             * Attempting to clear the MAC during a non one-shot or finalize
             * operation results in a failure
             */
            result = AESGCM_STATUS_ERROR;
        }

        CryptoUtils_memset(mac, macLength, 0, macLength);
        clearedBuffer = CryptoUtils_isBufferAllZeros(mac, macLength);

        if (!clearedBuffer)
        {
            result = AESGCM_STATUS_ERROR;
        }
    }

    AESDMAReset();

    AESReset();

    /*
     * Consume any outstanding interrupts we may have accrued
     * since disabling interrupts.
     */
    AESIntClear(AES_RESULT_RDY | AES_DMA_IN_DONE | AES_DMA_BUS_ERR);
    IntPendClear(INT_CRYPTO_RESULT_AVAIL_IRQ);

    /*
     * The operation in progress state must be cleared so a user can
     * start another operation after canceling.
     */
    object->operationInProgress = false;
    object->hwBusy = false;
    object->returnStatus = AESGCM_STATUS_CANCELED;

    if (object->returnBehavior == AESGCM_RETURN_BEHAVIOR_BLOCKING)
    {
        /* Unblock the pending task to signal that the operation is complete. */
        SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
    }
    else
    {
        /* Call the callback function provided by the application. */
        object->callbackFxn(handle, AESGCM_STATUS_CANCELED, object->operation, object->operationType);
    }

    /*
     * Power down and reset the crypto module, release the power constraint,
     * and post the access semaphore
     */
    AESGCM_cleanup(object);

    return result;
}
