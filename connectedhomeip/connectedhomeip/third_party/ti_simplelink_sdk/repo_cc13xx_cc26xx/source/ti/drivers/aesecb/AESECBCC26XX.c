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
#include <ti/drivers/dpl/SwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/AESECB.h>
#include <ti/drivers/aesecb/AESECBCC26XX.h>
#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_crypto.h)
#include DeviceFamily_constructPath(driverlib/aes.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/smph.h)

/* Forward declarations */
static void AESECB_hwiFxn(uintptr_t arg0);
static int_fast16_t AESECB_waitForResult(AESECB_Handle handle);
static void AESECB_cleanup(AESECB_Handle handle);
static int_fast16_t AESECB_startOperation(AESECB_Handle handle,
                                          AESECB_Operation *operation,
                                          AESECB_OperationType operationType);
static int_fast16_t AESECB_addDataInternal(AESECB_Handle handle,
                                           AESECB_Operation *operation,
                                           AESECB_OperationType operationType);
static int_fast16_t AESECB_setupSegmentedOperation(AESECB_Handle handle,
                                                   const CryptoKey *key);

#define AES_NON_BLOCK_MULTIPLE_MASK                 0x0F

/* Extern globals */
extern const AESECB_Config AESECB_config[];
extern const uint_least8_t AESECB_count;

/* Static globals */
static bool isInitialized = false;

/*
 *  ======== AESECB_hwiFxn ========
 */
static void AESECB_hwiFxn(uintptr_t arg0)
{
    AESECBCC26XX_Object *object = ((AESECB_Handle)arg0)->object;
    uintptr_t interruptKey;

    interruptKey = HwiP_disable();

    /* Mark the crypto HW is no longer needed for the current operation */
    object->hwBusy = false;

    /* Mark that the current single-step or multi-step operation is complete */
    if ((object->operationType != AESECB_OPERATION_TYPE_ENCRYPT_SEGMENTED) &&
        (object->operationType != AESECB_OPERATION_TYPE_DECRYPT_SEGMENTED))
    {
        object->operationInProgress = false;
    }

    HwiP_restore(interruptKey);

    if (AESIntStatusRaw() & AES_DMA_BUS_ERR)
    {
        object->returnStatus = AESECB_STATUS_ERROR;
    }

    AESIntClear(AES_RESULT_RDY | AES_DMA_BUS_ERR);

    AESECB_cleanup((AESECB_Handle)arg0);

    if (object->returnBehavior == AESECB_RETURN_BEHAVIOR_BLOCKING)
    {
        SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
    }
    else
    {
        object->callbackFxn((AESECB_Handle)arg0,
                            object->returnStatus,
                            object->operation,
                            object->operationType);
    }
}

/*
 *  ======== AESECB_cleanup ========
 */
static void AESECB_cleanup(AESECB_Handle handle)
{
    AESECBCC26XX_Object *object = handle->object;

    /*
     * Since plaintext keys use two reserved (by convention) slots in
     * the keystore, the slots must be invalidated to prevent its re-use
     * without reloading the key material again.
     */
    AESInvalidateKey(AES_KEY_AREA_6);
    AESInvalidateKey(AES_KEY_AREA_7);

    /*
     * This powers down all sub-modules of the crypto module until needed.
     * It does not power down the crypto module at PRCM level and
     * provides small power savings.
     */
    AESSelectAlgorithm(0x00);

    if (object->cryptoResourceLocked)
    {
        /*
         * Grant access for other threads to use the crypto module.
         * The semaphore must be posted before the callbackFxn to allow
         * the chaining of operations.
         */
        SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
        object->cryptoResourceLocked = false;
    }

    Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
}

/*
 *  ======== AESECB_init ========
 */
void AESECB_init(void)
{
    CryptoResourceCC26XX_constructRTOSObjects();

    isInitialized = true;
}

/*
 *  ======== AESECB_construct ========
 */
AESECB_Handle AESECB_construct(AESECB_Config *config,
                               const AESECB_Params *params)
{
    AESECB_Handle       handle;
    AESECBCC26XX_Object *object;
    uintptr_t           interruptKey;

    handle = config;
    object = handle->object;

    interruptKey = HwiP_disable();

    if (!isInitialized || object->isOpen)
    {
        HwiP_restore(interruptKey);

        return NULL;
    }

    object->isOpen = true;

    HwiP_restore(interruptKey);

    /* If params are NULL, use defaults */
    if (params == NULL)
    {
        params = (AESECB_Params *)&AESECB_defaultParams;
    }

    DebugP_assert(params->returnBehavior == AESECB_RETURN_BEHAVIOR_CALLBACK ? params->callbackFxn : true);

    object->returnBehavior = params->returnBehavior;
    object->callbackFxn = params->callbackFxn;
    object->semaphoreTimeout = params->returnBehavior == AESECB_RETURN_BEHAVIOR_BLOCKING ? params->timeout : SemaphoreP_NO_WAIT;
    object->threadSafe = true;
    object->hwBusy = false;
    object->operationInProgress = false;
    object->cryptoResourceLocked = false;

    /*
     * Set power dependency - that is power up and enable clock
     * for Crypto (CryptoResourceCC26XX) module.
     */
    Power_setDependency(PowerCC26XX_PERIPH_CRYPTO);

    return handle;
}

/*
 *  ======== AESECB_close ========
 */
void AESECB_close(AESECB_Handle handle)
{
    AESECBCC26XX_Object         *object;

    DebugP_assert(handle);

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;

    /* Mark the module as available */
    object->isOpen = false;

    /* Release power dependency on Crypto Module. */
    Power_releaseDependency(PowerCC26XX_PERIPH_CRYPTO);
}

/*
 *  ======== AESECB_waitForResult ========
 */
static int_fast16_t AESECB_waitForResult(AESECB_Handle handle)
{
    AESECBCC26XX_Object *object = handle->object;

    int_fast16_t result = AESECB_STATUS_ERROR;

    if (object->returnBehavior == AESECB_RETURN_BEHAVIOR_POLLING)
    {
        /* Wait until the operation is complete and check for DMA errors. */
        if (AESWaitForIRQFlags(AES_RESULT_RDY | AES_DMA_BUS_ERR) & AES_DMA_BUS_ERR)
        {
            object->returnStatus = AESECB_STATUS_ERROR;
        }

        /*
         * Save the returnStatus prior clearing operationInProgress or
         * releasing the access semaphore in case it's overwritten
         */
        result = object->returnStatus;

        /* Mark the crypto HW is no longer needed for the current operation */
        object->hwBusy = false;

        /* Mark that the current single-step or multi-step operation is complete */
        if ((object->operationType != AESECB_OPERATION_TYPE_ENCRYPT_SEGMENTED) &&
            (object->operationType != AESECB_OPERATION_TYPE_DECRYPT_SEGMENTED))
        {
            object->operationInProgress = false;
        }

        /*
         * Instead of posting the swi to handle cleanup, we will execute
         * the core of the function here
         */
        AESECB_cleanup(handle);
    }
    else if (object->returnBehavior == AESECB_RETURN_BEHAVIOR_BLOCKING)
    {
        SemaphoreP_pend(&CryptoResourceCC26XX_operationSemaphore,
                        SemaphoreP_WAIT_FOREVER);

        result = object->returnStatus;
    }
    else
    {
        result = AESECB_STATUS_SUCCESS;
    }

    return result;
}

/*
 *  ======== AESECB_startOperation ========
 */
static int_fast16_t AESECB_startOperation(AESECB_Handle handle,
                                          AESECB_Operation *operation,
                                          AESECB_OperationType operationType)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESECBCC26XX_Object *object = handle->object;

    uintptr_t interruptKey;
    interruptKey = HwiP_disable();

    /* Ensure that no other operation is already in progress for this driver instance */
    if (object->operationInProgress)
    {
        HwiP_restore(interruptKey);

        return AESECB_STATUS_ERROR;
    }
    else
    {
        /* Mark the current operation (single-step) is in progress */
        object->operationInProgress = true;
        HwiP_restore(interruptKey);

        /* Create an internal copy of the key */
        object->key = *(operation->key);
    }

    return AESECB_addDataInternal(handle, operation, operationType);
}

/*
 *  ======== AESECB_oneStepEncrypt ========
 */
int_fast16_t AESECB_oneStepEncrypt(AESECB_Handle handle,
                                   AESECB_Operation *operationStruct)
{

    return AESECB_startOperation(handle, operationStruct,
                                 AESECB_OPERATION_TYPE_ENCRYPT);
}

/*
 *  ======== AESECB_oneStepDecrypt ========
 */
int_fast16_t AESECB_oneStepDecrypt(AESECB_Handle handle,
                                   AESECB_Operation *operationStruct)
{

    return AESECB_startOperation(handle, operationStruct,
                                 AESECB_OPERATION_TYPE_DECRYPT);
}

/*
 *  ======== AESECB_setupEncrypt ========
 */
int_fast16_t AESECB_setupEncrypt(AESECB_Handle handle, const CryptoKey *key)
{
    int_fast16_t result = AESECB_setupSegmentedOperation(handle, key);

    if (result != AESECB_STATUS_ERROR)
    {
        AESECBCC26XX_Object *object = handle->object;
        object->operationType = AESECB_OPERATION_TYPE_ENCRYPT_SEGMENTED;
    }

    return result;
}

/*
 *  ======== AESECB_setupDecrypt ========
 */
int_fast16_t AESECB_setupDecrypt(AESECB_Handle handle, const CryptoKey *key)
{
    int_fast16_t result = AESECB_setupSegmentedOperation(handle, key);

    if (result != AESECB_STATUS_ERROR)
    {
        AESECBCC26XX_Object *object = handle->object;
        object->operationType = AESECB_OPERATION_TYPE_DECRYPT_SEGMENTED;
    }

    return result;
}

/*
 *  ======== AESECB_setupSegmentedOperation ========
 */
int_fast16_t AESECB_setupSegmentedOperation(AESECB_Handle handle, const CryptoKey *key)
{
    DebugP_assert(handle);

    AESECBCC26XX_Object *object = handle->object;

    uintptr_t interruptKey;
    interruptKey = HwiP_disable();

    int_fast16_t result = AESECB_STATUS_SUCCESS;

    /* Ensure that no other operation is already in progress for this driver instance */
    if (object->operationInProgress)
    {
        result = AESECB_STATUS_ERROR;
        HwiP_restore(interruptKey);
    }
    else
    {
        /* Mark the current operation (multi-step) is in progress */
        object->operationInProgress = true;
        HwiP_restore(interruptKey);

        /* Create an internal copy of the key */
        object->key = *(key);
    }

    return result;
}

/*
 *  ======== AESECB_addData ========
 */
int_fast16_t AESECB_addData(AESECB_Handle handle,
                            AESECB_Operation *operation)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESECBCC26XX_Object *object = handle->object;

    /* Check for previous failure or cancellation of segmented operation */
    if (object->returnStatus != AESECB_STATUS_SUCCESS)
    {
        /* Return the status of the previous call.
         * The callback function will not be executed.
         */
        return object->returnStatus;
    }

    DebugP_assert(object->operationType == AESECB_OPERATION_TYPE_DECRYPT_SEGMENTED ||
                  object->operationType == AESECB_OPERATION_TYPE_ENCRYPT_SEGMENTED);

    /* Verify the input length is non-zero and a multiple of the block size */
    if ((operation->inputLength == 0U) ||
        (operation->inputLength & AES_NON_BLOCK_MULTIPLE_MASK))
    {
        return AESECB_STATUS_ERROR;
    }

    /*
     * Pass object->operationType so that the object's operationType is not
     * changed in between calls
     */
    return AESECB_addDataInternal(handle, operation, object->operationType);
}

/*
 *  ======== AESECB_addDataInternal ========
 */
static int_fast16_t AESECB_addDataInternal(AESECB_Handle handle,
                                           AESECB_Operation *operation,
                                           AESECB_OperationType operationType)
{
    /* Check for these since the function uses them */
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESECBCC26XX_Object *object = handle->object;
    AESECBCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    SemaphoreP_Status resourceAcquired;

    /* Only plaintext CryptoKeys are supported for now */
    uint16_t keyLength = object->key.u.plaintext.keyLength;
    uint8_t *keyingMaterial = object->key.u.plaintext.keyMaterial;

    /* Only plaintext keys are supported in the current implementation */
    DebugP_assert(object->key.encoding == CryptoKey_PLAINTEXT);

    /*
     * keyMaterial and keyLength are passed to AESWriteToKeyStore(),
     * which may be in ROM where it won't have asserts so these
     * should be checked in this function.
     */
    DebugP_assert(keyingMaterial);
    DebugP_assert(keyLength == AES_128_KEY_LENGTH_BYTES ||
                  keyLength == AES_192_KEY_LENGTH_BYTES ||
                  keyLength == AES_256_KEY_LENGTH_BYTES);

    if (object->threadSafe)
    {
        /* Try and obtain access to the crypto module */
        resourceAcquired = SemaphoreP_pend(&CryptoResourceCC26XX_accessSemaphore,
                                           object->semaphoreTimeout);

        if (resourceAcquired != SemaphoreP_OK)
        {
            /* Do not capture this status in object->returnStatus to facilitate
             * a retry later. */
            return AESECB_STATUS_RESOURCE_UNAVAILABLE;
        }

        object->cryptoResourceLocked = true;
    }

    object->operationType = operationType;
    object->operation = operation;
    /* We will only change the returnStatus if there is an error */
    object->returnStatus = AESECB_STATUS_SUCCESS;

    /*
     * We need to set the HWI function and priority since the same physical
     * interrupt is shared by multiple drivers and they all need to coexist.
     * Whenever a driver starts an operation, it registers its HWI callback
     * with the OS.
     */
    HwiP_setFunc(&CryptoResourceCC26XX_hwi, AESECB_hwiFxn, (uintptr_t)handle);
    HwiP_setPriority(INT_CRYPTO_RESULT_AVAIL_IRQ, hwAttrs->intPriority);

    /*
     * Load the key from RAM or flash into the key store at a hardcoded
     * and reserved location
     */
    if (AESWriteToKeyStore(keyingMaterial, keyLength, AES_KEY_AREA_6) != AES_SUCCESS)
    {
        if (object->cryptoResourceLocked)
        {
            /* Release the CRYPTO mutex */
            SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
            object->cryptoResourceLocked = false;
        }

        object->returnStatus = AESECB_STATUS_ERROR;

        return object->returnStatus;
    }

    /*
     * DMA_IN_DONE must be disabled, since hwiFxn() is assuming that
     * the interrupt source is RESULT_AVAIL
     */
    AESIntDisable(AES_DMA_IN_DONE);

    /*
     * If we are in AESECB_RETURN_BEHAVIOR_POLLING, we do not want an
     * interrupt to trigger. AESWriteToKeyStore() disables and then
     * re-enables the CRYPTO IRQ in the NVIC so we need to disable it before
     * kicking off the operation.
     */
    if (object->returnBehavior == AESECB_RETURN_BEHAVIOR_POLLING)
    {
        IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);
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
         * Since plaintext keys use two reserved (by convention) slots in
         * the keystore, the slots must be invalidated to prevent its re-use
         * without reloading the key material again.
         */
        AESInvalidateKey(AES_KEY_AREA_6);
        AESInvalidateKey(AES_KEY_AREA_7);

        if (object->cryptoResourceLocked)
        {
            /* Release the CRYPTO mutex */
            SemaphoreP_post(&CryptoResourceCC26XX_accessSemaphore);
            object->cryptoResourceLocked = false;
        }

        AESSelectAlgorithm(0);

        object->returnStatus = AESECB_STATUS_ERROR;

        return object->returnStatus;
    }

    /*
     * Disallow standby. We are about to configure and start the accelerator.
     * Setting the constraint should happen after all opportunities to fail
     * out of the function. This way, we do not need to undo it each time we
     * exit with a failure.
     */
    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    /* Set direction of operation. */
    AESSetCtrl((operationType == AESECB_OPERATION_TYPE_DECRYPT ||
                operationType == AESECB_OPERATION_TYPE_DECRYPT_SEGMENTED ||
                operationType == AESECB_OPERATION_TYPE_FINALIZE_DECRYPT_SEGMENTED) ? 0 : CRYPTO_AESCTL_DIR);

    AESSetDataLength(operation->inputLength);

    object->hwBusy = true;

    AESStartDMAOperation(operation->input, operation->inputLength,
                         operation->output, operation->inputLength);

    return AESECB_waitForResult(handle);
}

/*
 *  ======== AESECB_finalize ========
 */
int_fast16_t AESECB_finalize(AESECB_Handle handle,
                             AESECB_Operation *operation)
{
    /* Publicly accessible functions should check their inputs */
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESECBCC26XX_Object *object = handle->object;

    /* Check for previous failure or cancellation of segmented operation */
    if (object->returnStatus != AESECB_STATUS_SUCCESS)
    {
        /* Return the status of the previous call.
         * The callback function will not be executed.
         */
        return object->returnStatus;
    }

    /* finalize() should only be used in segmented operations */
    DebugP_assert(object->operationType == AESECB_OPERATION_TYPE_DECRYPT_SEGMENTED ||
                  object->operationType == AESECB_OPERATION_TYPE_ENCRYPT_SEGMENTED);

    AESECB_OperationType operationType;

    if (object->operationType == AESECB_OPERATION_TYPE_ENCRYPT_SEGMENTED)
    {
        operationType = AESECB_OPERATION_TYPE_FINALIZE_ENCRYPT_SEGMENTED;
    }
    else if (object->operationType == AESECB_OPERATION_TYPE_DECRYPT_SEGMENTED)
    {
        operationType = AESECB_OPERATION_TYPE_FINALIZE_DECRYPT_SEGMENTED;
    }
    else
    {
        return AESECB_STATUS_ERROR;
    }

    int_fast16_t result = AESECB_STATUS_SUCCESS;

    /*
     * Must call addData() with an inputLength > 0 otherwise the crypto core
     * will assume the length to be infinite.
     */
    if (operation->inputLength > 0)
    {
        result = AESECB_addDataInternal(handle, operation, operationType);
    }
    else
    {
        /*
         * Save the object's returnStatus in case it's overwritten during
         * setup of a new operation after operationInProgress is cleared
         */
        result = object->returnStatus;

        /* Since there's no more data to process, mark that the multi-step
         * operation is complete here. */
        object->operationInProgress = false;

        if (object->returnBehavior == AESECB_RETURN_BEHAVIOR_CALLBACK)
        {
            /* Invoke the application callback function */
            object->callbackFxn(handle,
                                result,
                                operation,
                                operationType);

            /* Always return success in callback mode */
            result = AESECB_STATUS_SUCCESS;
        }
    }

    return result;
}

/*
 *  ======== AESECB_cancelOperation ========
 */
int_fast16_t AESECB_cancelOperation(AESECB_Handle handle)
{
    AESECBCC26XX_Object *object = handle->object;
    uintptr_t interruptKey;

    interruptKey = HwiP_disable();

    /* Check if the HW operation already completed */
    if (!object->hwBusy)
    {
        object->returnStatus = AESECB_STATUS_CANCELED;
        object->operationInProgress = false;

        HwiP_restore(interruptKey);

        /* No need to call the callback function provided by the application
         * since it would have already been called when the HW operation
         * completed.
         */

        return AESECB_STATUS_SUCCESS;
    }

    IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);

    HwiP_restore(interruptKey);

    /* Reset the DMA to stop transfers */
    AESDMAReset();

    /* Issue SW reset to recover the AES engine */
    AESReset();

    /* Consume any outstanding interrupts we may have accrued since disabling
     * interrupts.
     */
    IntPendClear(INT_CRYPTO_RESULT_AVAIL_IRQ);

    object->returnStatus = AESECB_STATUS_CANCELED;
    object->hwBusy = false;
    object->operationInProgress = false;

    if (object->returnBehavior == AESECB_RETURN_BEHAVIOR_BLOCKING)
    {
        /* Unblock the pending task to signal that the operation is complete */
        SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
    }
    else /* AESECB_RETURN_BEHAVIOR_CALLBACK */
    {
        /* Call the callback function provided by the application */
        object->callbackFxn(handle,
                            AESECB_STATUS_CANCELED,
                            object->operation,
                            object->operationType);
    }

    /* Cleanup posts the crypto access semaphore and must be done after the
     * operational semaphore is posted to avoid a potential race condition
     * when starting a new operation using a different driver instance.
     */
    AESECB_cleanup(handle);

    /* Always return success */
    return AESECB_STATUS_SUCCESS;
}

bool AESECB_acquireLock(AESECB_Handle handle, uint32_t timeout)
{

    return CryptoResourceCC26XX_acquireLock(timeout);
}

void AESECB_releaseLock(AESECB_Handle handle)
{
    CryptoResourceCC26XX_releaseLock();
}

void AESECB_enableThreadSafety(AESECB_Handle handle)
{
    AESECBCC26XX_Object *object = handle->object;

    object->threadSafe = true;
}
void AESECB_disableThreadSafety(AESECB_Handle handle)
{
    AESECBCC26XX_Object *object = handle->object;

    object->threadSafe = false;
}
