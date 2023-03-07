/*
 * Copyright (c) 2018-2021, Texas Instruments Incorporated
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

#include <ti/drivers/AESCBC.h>
#include <ti/drivers/aescbc/AESCBCCC26XX.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>
#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>
#include <ti/drivers/cryptoutils/utils/CryptoUtils.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_crypto.h)
#include DeviceFamily_constructPath(driverlib/aes.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)

#define AES_NON_BLOCK_MULTIPLE_MASK 0x0F

/* Forward declarations */
static void AESCBC_hwiFxn(uintptr_t arg0);
static int_fast16_t AESCBC_startOperation(AESCBC_Handle handle,
                                          AESCBC_OneStepOperation *operation,
                                          AESCBC_OperationType operationType);
static int_fast16_t AESCBC_waitForResult(AESCBC_Handle handle);
static void AESCBC_cleanup(AESCBCCC26XX_Object *object);
static int_fast16_t AESCBC_addDataInternal(AESCBC_Handle handle,
                                           uint8_t *input,
                                           uint8_t *output,
                                           size_t inputLength,
                                           AESCBC_Mode direction);
static int_fast16_t AESCBC_setupSegmentedOperation(AESCBC_Handle handle, const CryptoKey *key);
static int_fast16_t AESCBC_setOperationInProgress(AESCBCCC26XX_Object *object);

/* Static globals */
static bool isInitialized = false;

/*
 *  ======== AESCBC_hwiFxn ========
 */
static void AESCBC_hwiFxn(uintptr_t arg0)
{
    AESCBCCC26XX_Object *object = ((AESCBC_Handle)arg0)->object;

    uintptr_t interruptKey = HwiP_disable();

    /*
     * Mark that we are done with the operation so that AESCBC_cancelOperation
     * knows not to try canceling.
     */
    object->hwBusy = false;

    if (object->operationType != AESCBC_OP_TYPE_DECRYPT_SEGMENTED &&
        object->operationType != AESCBC_OP_TYPE_ENCRYPT_SEGMENTED)
    {
        /* One shot and finalize operations are done at this point */
        object->operationInProgress = false;
    }

    HwiP_restore(interruptKey);

    /* Propagate the DMA error from driverlib to the application */
    if (AESIntStatusRaw() & AES_DMA_BUS_ERR)
    {
        object->returnStatus = AESCBC_STATUS_ERROR;
    }
    else
    {
        if (AESGetCtrl() & CRYPTO_AESCTL_SAVE_CONTEXT_M)
        {
            /*
             * Store the new iv into object for the next block should we want to
             * continue the chain of blocks in a later segmented operation.
             */
            AESReadNonAuthenticationModeIV(object->iv);
        }
        else
        {
            /*
             * For one-shot and finalize operations, clear the IV buffer
             * to prevent data leakage
             */
            CryptoUtils_memset(object->iv, sizeof(object->iv), 0, sizeof(object->iv));
        }
    }

    AESIntClear(AES_RESULT_RDY | AES_DMA_BUS_ERR);

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
    AESCBC_cleanup(object);

    if (object->returnBehavior == AESCBC_RETURN_BEHAVIOR_BLOCKING)
    {
        /* Unblock the pending task to signal that the operation is complete. */
        SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
    }
    else
    {
        /* Call the callback function provided by the application. */
        object->callbackFxn((AESCBC_Handle)arg0, object->returnStatus, object->operation, object->operationType);
    }
}

/*
 *  ======== AESCBC_cleanup ========
 */
static void AESCBC_cleanup(AESCBCCC26XX_Object *object)
{
    /*
     * Since plaintext keys use two reserved (by convention) slots in the keystore,
     * the slots must be invalidated to prevent its re-use without reloading
     * the key material again.
     */
    AESInvalidateKey(AES_KEY_AREA_6);
    AESInvalidateKey(AES_KEY_AREA_7);

    /*
     * This powers down all sub-modules of the crypto module until needed.
     * It does not power down the crypto module at PRCM level and provides small
     * power savings.
     */
    AESSelectAlgorithm(0x00);

    Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

    /*
     * Grant access for other threads to use the crypto module.
     * The semaphore must be posted before the callbackFxn to allow the chaining
     * of operations.
     */
    if (object->cryptoResourceLocked)
    {
        CryptoResourceCC26XX_releaseLock();
        object->cryptoResourceLocked = false;
    }
}

/*
 *  ======== AESCBC_init ========
 */
void AESCBC_init(void)
{
    CryptoResourceCC26XX_constructRTOSObjects();

    isInitialized = true;
}

/*
 *  ======== AESCBC_construct ========
 */
AESCBC_Handle AESCBC_construct(AESCBC_Config *config, const AESCBC_Params *params)
{
    AESCBC_Handle handle;
    AESCBCCC26XX_Object *object;
    uintptr_t key;

    handle = config;
    object = handle->object;

    key = HwiP_disable();

    if (!isInitialized || object->isOpen)
    {
        HwiP_restore(key);
        return NULL;
    }

    object->isOpen = true;

    HwiP_restore(key);

    /* If params are NULL, use defaults */
    if (params == NULL)
    {
        params = (AESCBC_Params *)&AESCBC_defaultParams;
    }

    DebugP_assert(params->returnBehavior == AESCBC_RETURN_BEHAVIOR_CALLBACK ? params->callbackFxn : true);

    object->returnBehavior = params->returnBehavior;
    object->callbackFxn = params->callbackFxn;

    if (params->returnBehavior == AESCBC_RETURN_BEHAVIOR_BLOCKING)
    {
        object->semaphoreTimeout = params->timeout;
    }
    else
    {
        object->semaphoreTimeout = SemaphoreP_NO_WAIT;
    }

    object->threadSafe = true;
    object->cryptoResourceLocked = false;
    object->hwBusy = false;
    object->operationInProgress = false;

    /* Set power dependency - i.e. power up and enable clock for Crypto (CryptoResourceCC26XX) module. */
    Power_setDependency(PowerCC26XX_PERIPH_CRYPTO);

    return handle;
}

/*
 *  ======== AESCBC_close ========
 */
void AESCBC_close(AESCBC_Handle handle)
{
    AESCBCCC26XX_Object *object;

    DebugP_assert(handle);

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;

    /* Mark the module as available */
    object->isOpen = false;

    /* Release power dependency on Crypto Module. */
    Power_releaseDependency(PowerCC26XX_PERIPH_CRYPTO);
}

/*
 *  ======== AESCBC_startOperation ========
 */
static int_fast16_t AESCBC_startOperation(AESCBC_Handle handle,
                                          AESCBC_OneStepOperation *operation,
                                          AESCBC_OperationType operationType)
{
    /*
     * Even though this function is not public, assert handle and operation
     * since those are used in this function
     */
    DebugP_assert(handle);
    DebugP_assert(operation);

    /* Internal generation of IVs is currently not supported */
    DebugP_assert(!operation->ivInternallyGenerated);

    AESCBCCC26XX_Object *object = handle->object;

    int_fast16_t result = AESCBC_STATUS_SUCCESS;

    /*
     * Check that there is no operation in progress for this driver
     * instance
     */
    result = AESCBC_setOperationInProgress(object);

    if (result != AESCBC_STATUS_SUCCESS)
    {
        return result;
    }

    AESCBC_Mode direction = AESCBC_MODE_ENCRYPT;

    if (operationType == AESCBC_OPERATION_TYPE_DECRYPT)
    {
        direction = AESCBC_MODE_DECRYPT;
    }

    if (object->threadSafe)
    {
        if (!CryptoResourceCC26XX_acquireLock(object->semaphoreTimeout))
        {
            return AESCBC_STATUS_RESOURCE_UNAVAILABLE;
        }

        object->cryptoResourceLocked = true;
    }

    object->operation = (AESCBC_OperationUnion *)operation;
    object->operationType = operationType;
    /* We will only change the returnStatus if there is an error */
    object->returnStatus = AESCBC_STATUS_SUCCESS;

    memcpy(object->iv, operation->iv, sizeof(object->iv));

    object->key = *(operation->key);

    result = AESCBC_addDataInternal(handle, operation->input, operation->output, operation->inputLength, direction);

    if ((result != AESCBC_STATUS_SUCCESS) && (object->cryptoResourceLocked))
    {
        CryptoResourceCC26XX_releaseLock();
        object->cryptoResourceLocked = false;
    }

    return result;
}

/*
 *  ======== AESCBC_setOperationInProgress ========
 */
static int_fast16_t AESCBC_setOperationInProgress(AESCBCCC26XX_Object *object)
{
    uintptr_t interruptKey = HwiP_disable();

    if (object->operationInProgress)
    {
        HwiP_restore(interruptKey);

        return AESCBC_STATUS_ERROR;
    }

    object->operationInProgress = true;

    HwiP_restore(interruptKey);

    return AESCBC_STATUS_SUCCESS;
}

/*
 *  ======== AESCBC_waitForResult ========
 */
static int_fast16_t AESCBC_waitForResult(AESCBC_Handle handle)
{
    AESCBCCC26XX_Object *object = handle->object;

    int_fast16_t result = AESCBC_STATUS_ERROR;

    if (object->returnBehavior == AESCBC_RETURN_BEHAVIOR_POLLING)
    {
        /* Wait until the operation is complete and check for DMA errors. */
        if (AESWaitForIRQFlags(AES_RESULT_RDY | AES_DMA_BUS_ERR) & AES_DMA_BUS_ERR)
        {
            object->returnStatus = AESCBC_STATUS_ERROR;
        }
        else
        {
            if (AESGetCtrl() & CRYPTO_AESCTL_SAVE_CONTEXT_M)
            {
                /*
                 * Store the new iv into object for the next block should we want to
                 * continue the chain of blocks in a later segmented operation.
                 */
                AESReadNonAuthenticationModeIV(object->iv);
            }
            else
            {
                /*
                 * For one-shot or finalize operations, clear the IV buffer
                 * to prevent data leakage
                 */
                CryptoUtils_memset((uint8_t *)object->iv, sizeof(object->iv), 0, sizeof(object->iv));
            }
        }

        /*
         * Save the returnStatus prior clearing operationInProgress or
         * releasing the access semaphore in case it's overwritten
         */
        result = object->returnStatus;

        /* Mark that we are done with the operation */
        object->hwBusy = false;

        if (object->operationType != AESCBC_OP_TYPE_DECRYPT_SEGMENTED &&
            object->operationType != AESCBC_OP_TYPE_ENCRYPT_SEGMENTED)
        {
            /* One shot and finalize operations are done at this point */
            object->operationInProgress = false;
        }

        /*
         * Instead of posting the swi to handle cleanup, we will execute
         * the core of the function here
         */
        AESCBC_cleanup(object);
    }
    else if (object->returnBehavior == AESCBC_RETURN_BEHAVIOR_BLOCKING)
    {
        SemaphoreP_pend(&CryptoResourceCC26XX_operationSemaphore, SemaphoreP_WAIT_FOREVER);

        result = object->returnStatus;
    }
    else
    {
        /* Success is always returned in callback mode */
        result = AESCBC_STATUS_SUCCESS;
    }

    return result;
}

/*
 *  ======== AESCBC_oneStepEncrypt ========
 */
int_fast16_t AESCBC_oneStepEncrypt(AESCBC_Handle handle, AESCBC_OneStepOperation *operationStruct)
{

    return AESCBC_startOperation(handle, operationStruct, AESCBC_OPERATION_TYPE_ENCRYPT);
}

/*
 *  ======== AESCBC_oneStepDecrypt ========
 */
int_fast16_t AESCBC_oneStepDecrypt(AESCBC_Handle handle, AESCBC_OneStepOperation *operationStruct)
{

    return AESCBC_startOperation(handle, operationStruct, AESCBC_OPERATION_TYPE_DECRYPT);
}

/*
 *  ======== AESCBC_setupSegmentedOperation ========
 */
static int_fast16_t AESCBC_setupSegmentedOperation(AESCBC_Handle handle, const CryptoKey *key)
{
    /*
     * Not all of the input values will be asserted here since they will be
     * asserted/checked later. Asserts checking the key's validity will occur
     * in addDataInternal for maintainability purposes.
     */
    DebugP_assert(handle);

    AESCBCCC26XX_Object *object = handle->object;

    int_fast16_t result = AESCBC_STATUS_SUCCESS;

    /*
     * Check that there is no operation in progress for this driver
     * instance
     */
    result = AESCBC_setOperationInProgress(object);

    if (result != AESCBC_STATUS_ERROR)
    {
        object->key = *key;
        /* returnStatus is changed in the case of an error or cancellation */
        object->returnStatus = AESCBC_STATUS_SUCCESS;
    }

    return result;
}

/*
 *  ======== AESCBC_setupEncrypt ========
 */
int_fast16_t AESCBC_setupEncrypt(AESCBC_Handle handle, const CryptoKey *key)
{
    int_fast16_t result = AESCBC_setupSegmentedOperation(handle, key);

    if (result != AESCBC_STATUS_ERROR)
    {
        AESCBCCC26XX_Object *object = handle->object;
        object->operationType = AESCBC_OP_TYPE_ENCRYPT_SEGMENTED;
    }

    return result;
}

/*
 *  ======== AESCBC_setupDecrypt ========
 */
int_fast16_t AESCBC_setupDecrypt(AESCBC_Handle handle, const CryptoKey *key)
{
    int_fast16_t result = AESCBC_setupSegmentedOperation(handle, key);

    if (result != AESCBC_STATUS_ERROR)
    {
        AESCBCCC26XX_Object *object = handle->object;
        object->operationType = AESCBC_OP_TYPE_DECRYPT_SEGMENTED;
    }

    return result;
}

/*
 *  ======== AESCBC_setIV ========
 */
int_fast16_t AESCBC_setIV(AESCBC_Handle handle, const uint8_t *iv, size_t ivLength)
{
    /* Public accesible functions should check inputs */
    DebugP_assert(handle);
    DebugP_assert(iv);

    AESCBCCC26XX_Object *object = handle->object;

    /* Don't continue the operation if there was an error or cancellation */
    if (object->returnStatus != AESCBC_STATUS_SUCCESS)
    {
        return object->returnStatus;
    }

    /* This function is reserved for segmented operations only */
    DebugP_assert(object->operationType == AESCBC_OP_TYPE_DECRYPT_SEGMENTED ||
                  object->operationType == AESCBC_OP_TYPE_ENCRYPT_SEGMENTED);

    /* The length of the new IV should be 16 or AES_BLOCK_SIZE bytes */
    if (ivLength != AES_BLOCK_SIZE)
    {
        return AESCBC_STATUS_ERROR;
    }

    /*
     * Copy the user-provided IV to the object structure since the IV in
     * operation struct is reserved for one-shot operations only
     */
    memcpy(object->iv, iv, sizeof(object->iv));

    return AESCBC_STATUS_SUCCESS;
}

/*
 *  ======== AESCBC_generateIV ========
 */
int_fast16_t AESCBC_generateIV(AESCBC_Handle handle, uint8_t *iv, size_t ivSize, size_t *ivLength)
{
    /* This is not currently supported */
    return AESCBC_STATUS_FEATURE_NOT_SUPPORTED;
}

/*
 *  ======== AESCBC_addData ========
 */
int_fast16_t AESCBC_addData(AESCBC_Handle handle, AESCBC_SegmentedOperation *operation)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESCBCCC26XX_Object *object = handle->object;

    /* Don't continue the operation if there was an error or cancellation */
    if (object->returnStatus != AESCBC_STATUS_SUCCESS)
    {
        return object->returnStatus;
    }

    DebugP_assert(object->operationType == AESCBC_OP_TYPE_DECRYPT_SEGMENTED ||
                  object->operationType == AESCBC_OP_TYPE_ENCRYPT_SEGMENTED);

    AESCBC_Mode direction = AESCBC_MODE_ENCRYPT;
    if (object->operationType == AESCBC_OP_TYPE_DECRYPT_SEGMENTED)
    {
        direction = AESCBC_MODE_DECRYPT;
    }

    if (object->threadSafe)
    {
        if (!CryptoResourceCC26XX_acquireLock(object->semaphoreTimeout))
        {
            return AESCBC_STATUS_RESOURCE_UNAVAILABLE;
        }

        object->cryptoResourceLocked = true;
    }

    /*
     * Reload the operation struct, there's no guarantee that this remains the
     * same in between calls
     */
    object->operation = (AESCBC_OperationUnion *)operation;

    int_fast16_t result = AESCBC_addDataInternal(handle,
                                                 operation->input,
                                                 operation->output,
                                                 operation->inputLength,
                                                 direction);

    if ((result != AESCBC_STATUS_SUCCESS) && (object->cryptoResourceLocked))
    {
        CryptoResourceCC26XX_releaseLock();
        object->cryptoResourceLocked = false;
    }

    return result;
}

/*
 *  ======== AESCBC_addDataInternal ========
 */
static int_fast16_t AESCBC_addDataInternal(AESCBC_Handle handle,
                                           uint8_t *input,
                                           uint8_t *output,
                                           size_t inputLength,
                                           AESCBC_Mode direction)
{
    /* Check for these since the function uses them */
    DebugP_assert(handle);
    DebugP_assert(input);
    DebugP_assert(output);

    AESCBCCC26XX_Object *object = handle->object;

    /*
     * The key is provided as an input in setupEncrypt/Decrypt()
     * and within the input operation struct for oneStepEncrypt/Decrypt(),
     * and users should check those inputs.
     */
    DebugP_assert(object->key);

    /* Only plaintext CryptoKeys are supported for now */
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

    if (inputLength == 0 || (inputLength & AES_NON_BLOCK_MULTIPLE_MASK))
    {
        return AESCBC_STATUS_ERROR;
    }

    AESCBCCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;

    /*
     * We need to set the HWI function and priority since the same physical
     * interrupt is shared by multiple drivers and they all need to coexist.
     * Whenever a driver starts an operation, it registers its HWI callback
     * with the OS.
     */
    HwiP_setFunc(&CryptoResourceCC26XX_hwi, AESCBC_hwiFxn, (uintptr_t)handle);
    HwiP_setPriority(INT_CRYPTO_RESULT_AVAIL_IRQ, hwAttrs->intPriority);

    /*
     * Load the key from RAM or flash into the key store at
     * a hardcoded and reserved location
     */
    if (AESWriteToKeyStore(keyingMaterial, keyLength, AES_KEY_AREA_6) != AES_SUCCESS)
    {
        return AESCBC_STATUS_ERROR;
    }

    /*
     * DMA_IN_DONE must be disabled, since hwiFxn() is assuming that
     * the interrupt source is RESULT_AVAIL
     */
    AESIntDisable(AES_DMA_IN_DONE);

    /*
     * If we are in AESCBC_RETURN_BEHAVIOR_POLLING, we do not want an
     * interrupt to trigger. AESWriteToKeyStore() disables and then re-enables
     * the CRYPTO IRQ in the NVIC so we need to disable it
     * before kicking off the operation.
     */
    if (object->returnBehavior == AESCBC_RETURN_BEHAVIOR_POLLING)
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

        AESSelectAlgorithm(0);

        return AESCBC_STATUS_ERROR;
    }

    /*
     * Disallow standby. We are about to configure and start the accelerator.
     * Setting the constraint should happen after all opportunities to fail
     * out of the function. This way, we do not need to undo it each time we
     * exit with a failure.
     */
    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    AESSetInitializationVector(object->iv);

    uint32_t ctrlVal = CRYPTO_AESCTL_CBC | (direction == AESCBC_MODE_ENCRYPT ? CRYPTO_AESCTL_DIR : 0);

    if (object->operationType == AESCBC_OP_TYPE_DECRYPT_SEGMENTED ||
        object->operationType == AESCBC_OP_TYPE_ENCRYPT_SEGMENTED)
    {
        /* Save the context to continue a segmented operation */
        ctrlVal |= CRYPTO_AESCTL_SAVE_CONTEXT;
    }

    AESSetCtrl(ctrlVal);

    AESSetDataLength(inputLength);
    AESSetAuthLength(0);

    object->hwBusy = true;

    AESStartDMAOperation(input, inputLength, output, inputLength);

    return AESCBC_waitForResult(handle);
}

/*
 *  ======== AESCBC_finalize ========
 */
int_fast16_t AESCBC_finalize(AESCBC_Handle handle, AESCBC_SegmentedOperation *operation)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESCBCCC26XX_Object *object = handle->object;

    /* Don't continue the operation if there was an error or cancellation */
    if (object->returnStatus != AESCBC_STATUS_SUCCESS)
    {
        return object->returnStatus;
    }

    DebugP_assert(object->operationType == AESCBC_OP_TYPE_DECRYPT_SEGMENTED ||
                  object->operationType == AESCBC_OP_TYPE_ENCRYPT_SEGMENTED);

    AESCBC_OperationType operationType = AESCBC_OP_TYPE_FINALIZE_ENCRYPT_SEGMENTED;
    AESCBC_Mode direction = AESCBC_MODE_ENCRYPT;

    if (object->operationType == AESCBC_OP_TYPE_DECRYPT_SEGMENTED)
    {
        operationType = AESCBC_OP_TYPE_FINALIZE_DECRYPT_SEGMENTED;
        direction = AESCBC_MODE_DECRYPT;
    }

    int_fast16_t result = AESCBC_STATUS_SUCCESS;

    if (operation->inputLength > 0)
    {
        if (object->threadSafe)
        {
            if (!CryptoResourceCC26XX_acquireLock(object->semaphoreTimeout))
            {
                return AESCBC_STATUS_RESOURCE_UNAVAILABLE;
            }

            object->cryptoResourceLocked = true;
        }

        /*
         * operationType in Object should be updated to a finalize type
         * for the application callback
         */
        object->operationType = operationType;
        object->operation = (AESCBC_OperationUnion *)operation;

        result = AESCBC_addDataInternal(handle, operation->input, operation->output, operation->inputLength, direction);

        if ((result != AESCBC_STATUS_SUCCESS) && (object->cryptoResourceLocked))
        {
            CryptoResourceCC26XX_releaseLock();
            object->cryptoResourceLocked = false;
        }
    }
    else
    {
        /* Clear the IV buffer to prevent data leakage */
        CryptoUtils_memset(object->iv, sizeof(object->iv), 0, sizeof(object->iv));

        /*
         * Save the object's returnStatus in case it's overwritten during
         * setup of a new segmented operation after operationInProgress is
         * cleared
         */
        result = object->returnStatus;

        object->operationInProgress = false;

        if (object->returnBehavior == AESCBC_RETURN_BEHAVIOR_CALLBACK)
        {
            object->callbackFxn(handle, result, (AESCBC_OperationUnion *)operation, operationType);

            /* Always return success in callback mode */
            result = AESCBC_STATUS_SUCCESS;
        }
    }

    return result;
}

/*
 *  ======== AESCBC_cancelOperation ========
 */
int_fast16_t AESCBC_cancelOperation(AESCBC_Handle handle)
{
    AESCBCCC26XX_Object *object = handle->object;

    /* Cancel is only supported for callback mode */
    if (object->returnBehavior != AESCBC_RETURN_BEHAVIOR_CALLBACK)
    {
        return AESCBC_STATUS_ERROR;
    }

    uintptr_t key;
    key = HwiP_disable();

    if (!object->hwBusy)
    {
        object->returnStatus = AESCBC_STATUS_CANCELED;
        object->operationInProgress = false;

        HwiP_restore(key);

        /* A success should be returned if no hw operations are in progress */
        return AESCBC_STATUS_SUCCESS;
    }

    IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);

    HwiP_restore(key);

    /*
     * The IV buffer in Object should be cleared if a segmented operation is
     * taking place for security purposes
     */
    CryptoUtils_memset(object->iv, sizeof(object->iv), 0, sizeof(object->iv));

    bool clearedIV = CryptoUtils_isBufferAllZeros(object->iv, AES_BLOCK_SIZE);

    int_fast16_t result = AESCBC_STATUS_SUCCESS;

    if (!clearedIV)
    {
        result = AESCBC_STATUS_ERROR;
    }

    AESDMAReset();

    AESReset();

    /*
     * Consume any outstanding interrupts we may have accrued
     * since disabling interrupts.
     */
    IntPendClear(INT_CRYPTO_RESULT_AVAIL_IRQ);

    /*
     * The operation in progress states should be cleared so that the user
     * can start another operation after canceling
     */
    object->returnStatus = AESCBC_STATUS_CANCELED;
    object->operationInProgress = false;
    object->hwBusy = false;

    if (object->returnBehavior == AESCBC_RETURN_BEHAVIOR_BLOCKING)
    {
        /* Unblock the pending task to signal that the operation is complete. */
        SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
    }
    else
    {
        /* Call the callback function provided by the application. */
        object->callbackFxn(handle, AESCBC_STATUS_CANCELED, object->operation, object->operationType);
    }

    AESCBC_cleanup(object);

    return result;
}

bool AESCBC_acquireLock(AESCBC_Handle handle, uint32_t timeout)
{
    return CryptoResourceCC26XX_acquireLock(timeout);
}

void AESCBC_releaseLock(AESCBC_Handle handle)
{
    CryptoResourceCC26XX_releaseLock();
}

void AESCBC_enableThreadSafety(AESCBC_Handle handle)
{
    AESCBCCC26XX_Object *object = handle->object;

    object->threadSafe = true;
}
void AESCBC_disableThreadSafety(AESCBC_Handle handle)
{
    AESCBCCC26XX_Object *object = handle->object;

    object->threadSafe = false;
}
