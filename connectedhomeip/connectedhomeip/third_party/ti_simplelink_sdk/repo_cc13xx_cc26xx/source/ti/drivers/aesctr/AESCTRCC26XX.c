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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/AESCTR.h>
#include <ti/drivers/aesctr/AESCTRCC26XX.h>
#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_crypto.h)
#include DeviceFamily_constructPath(driverlib/aes.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)

#define AES_NON_BLOCK_MULTIPLE_MASK 0x0F

/* Forward declarations */
static void AESCTR_hwiFxn (uintptr_t arg0);
static int_fast16_t AESCTR_startOneStepOperation(AESCTR_Handle handle,
                                                 AESCTR_OneStepOperation *operation,
                                                 AESCTR_OperationType operationType);
static inline int_fast16_t AESCTR_waitForResult(AESCTRCC26XX_Object *object);
static void AESCTR_cleanup(AESCTRCC26XX_Object *object);
static void AESCTR_initCounter(AESCTRCC26XX_Object *object,
                               const uint8_t *initialCounter);
static int_fast16_t AESCTR_setOperationInProgress(AESCTRCC26XX_Object *object);

/* Static globals */
static bool AESCTR_isInitialized = false;

/*
 *  ======== AESCTR_hwiFxn ========
 */
static void AESCTR_hwiFxn (uintptr_t arg0)
{
    AESCTRCC26XX_Object *object = ((AESCTR_Handle)arg0)->object;
    uintptr_t interruptKey = HwiP_disable();

    /* Mark that we are done with the operation so that
     * AESCTR_cancelOperation knows not to try canceling.
     */
    object->hwBusy = false;

    /* Check for one-step or final operation */
    if (!(object->operationType & AESCTR_OP_FLAG_SEGMENTED))
    {
        /* Operation is complete */
        object->operationInProgress = false;
    }

    HwiP_restore(interruptKey);

    if (AESIntStatusRaw() & AES_DMA_BUS_ERR)
    {
        /* Propagate the DMA error from driverlib to the application */
        object->returnStatus = AESCTR_STATUS_ERROR;
    }
    else /* Must be RESULT_AVAIL because DMA_IN_DONE was disabled */
    {
        if (AESGetCtrl() & CRYPTO_AESCTL_SAVE_CONTEXT_M)
        {
            /* Save counter value if the context was saved */
            AESReadNonAuthenticationModeIV(object->counter);
        }
    }

    AESIntClear(AES_RESULT_RDY | AES_DMA_BUS_ERR);

    /* Handle cleaning up of the operation: Invalidate the key,
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
    AESCTR_cleanup(object);

    if (object->returnBehavior == AESCTR_RETURN_BEHAVIOR_BLOCKING)
    {
        /* Unblock the pending task to signal that the operation is complete. */
        SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
    }
    else /* AESCTR_RETURN_BEHAVIOR_CALLBACK */
    {
        /* Call the callback function provided by the application */
        object->callbackFxn((AESCTR_Handle)arg0, object->returnStatus,
                            object->operation, object->operationType);
    }
}

/*
 *  ======== AESCTR_cleanup ========
 */
static void AESCTR_cleanup(AESCTRCC26XX_Object *object)
{
    /* Since plaintext keys use two reserved (by convention) slots in the keystore,
     * the slots must be invalidated to prevent its re-use without reloading
     * the key material again.
     */
    AESInvalidateKey(AES_KEY_AREA_6);
    AESInvalidateKey(AES_KEY_AREA_7);

    /*  This powers down all sub-modules of the crypto module until needed.
     *  It does not power down the crypto module at PRCM level and provides small
     *  power savings.
     */
    AESSelectAlgorithm(0x00);

    if (object->returnBehavior != AESCTR_RETURN_BEHAVIOR_POLLING)
    {
        Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
    }

    /*  Grant access for other threads to use the crypto module.
     *  The semaphore must be posted before the callbackFxn to allow the chaining
     *  of operations.
     */
    if (object->cryptoResourceLocked)
    {
        CryptoResourceCC26XX_releaseLock();
        object->cryptoResourceLocked = false;
    }
}

/*
 *  ======== AESCTR_init ========
 */
void AESCTR_init(void)
{
    CryptoResourceCC26XX_constructRTOSObjects();

    AESCTR_isInitialized = true;
}

/*
 *  ======== AESCTR_open ========
 */
AESCTR_Handle AESCTR_construct(AESCTR_Config *config, const AESCTR_Params *params)
{
    DebugP_assert(config);
    DebugP_assert(params);

    AESCTR_Handle handle = config;
    AESCTRCC26XX_Object *object = handle->object;

    DebugP_assert(object);

    uintptr_t interruptKey = HwiP_disable();

    if (!AESCTR_isInitialized || object->isOpen)
    {
        HwiP_restore(interruptKey);
        return(NULL);
    }

    object->isOpen = true;

    HwiP_restore(interruptKey);

    /* If params are NULL, use defaults */
    if (params == NULL)
    {
        params = (AESCTR_Params *)&AESCTR_defaultParams;
    }

    DebugP_assert((params->returnBehavior != AESCTR_RETURN_BEHAVIOR_CALLBACK) ||
                  (params->callbackFxn != NULL));

    object->returnBehavior = params->returnBehavior;
    object->callbackFxn = params->callbackFxn;
    if (params->returnBehavior == AESCTR_RETURN_BEHAVIOR_BLOCKING)
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

    /* Set power dependency - i.e. power up and enable clock for Crypto
     * (CryptoResourceCC26XX) module. */
    Power_setDependency(PowerCC26XX_PERIPH_CRYPTO);

    return(handle);
}

/*
 *  ======== AESCTR_close ========
 */
void AESCTR_close(AESCTR_Handle handle)
{
    DebugP_assert(handle);

    /* Get the pointer to the object and hwAttrs */
    AESCTRCC26XX_Object *object = handle->object;

    /* Mark the module as available */
    object->isOpen = false;

    /* Release power dependency on Crypto Module */
    Power_releaseDependency(PowerCC26XX_PERIPH_CRYPTO);
}


/*
 *  ======== AESCTR_processData ========
 */
static int_fast16_t AESCTR_processData(AESCTR_Handle handle)
{
    AESCTRCC26XX_Object *object = handle->object;
    int_fast16_t status = AESCTR_STATUS_SUCCESS;
    AESCTRCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint16_t keyLength;
    const uint8_t *keyData;
    uint32_t ctrlVal;

    DebugP_assert(object->input);
    DebugP_assert(object->output);

    /* Only plaintext CryptoKeys are supported currently */
    DebugP_assert((object->key.encoding == CryptoKey_PLAINTEXT) ||
                  (object->key.encoding == CryptoKey_BLANK_PLAINTEXT));

    keyLength = object->key.u.plaintext.keyLength;
    keyData = object->key.u.plaintext.keyMaterial;

    DebugP_assert(keyData);
    DebugP_assert((keyLength == 16) ||
                  (keyLength == 24) ||
                  (keyLength == 32));

    /* We need to set the HWI function and priority since the same physical
     * interrupt is shared by multiple drivers and they all need to coexist.
     * Whenever a driver starts an operation, it registers its HWI callback with
     * the OS.
     */
    HwiP_setFunc(&CryptoResourceCC26XX_hwi, AESCTR_hwiFxn,
                 (uintptr_t)handle);
    HwiP_setPriority(INT_CRYPTO_RESULT_AVAIL_IRQ, hwAttrs->intPriority);

    /* Load the key from RAM or flash into the key store at a hardcoded and
     * reserved location.
     */
    if (AESWriteToKeyStore(keyData, keyLength, AES_KEY_AREA_6) != AES_SUCCESS)
    {
        status = AESCTR_STATUS_ERROR;
    }

    if (status == AESCTR_STATUS_SUCCESS)
    {
        /* AESWriteKeyStore() enables the CRYPTO IRQ. Disable it if polling mode */
        if (object->returnBehavior == AESCTR_RETURN_BEHAVIOR_POLLING)
        {
            IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);
        }

        /* AESWriteToKeyStore() enables both DMA_IN_DONE and RESULT_AVAIL
         * interrupts but since this driver only utilizes RESULT_AVAIL,
         * disable the DMA_IN_DONE interrupt to simplify handling.
         */
        AESIntDisable(AES_DMA_IN_DONE);

        /* Power the AES sub-module of the crypto module */
        AESSelectAlgorithm(AES_ALGSEL_AES);

        /* Load the key from the key store into the internal register banks of the
         * AES sub-module known as AES_KEY1.
         */
        if (AESReadFromKeyStore(AES_KEY_AREA_6) != AES_SUCCESS)
        {
            /* Since plaintext keys use two reserved (by convention) slots in the
             * keystore, the slots must be invalidated to prevent its re-use without
             * reloading the key material again.
             */
            AESInvalidateKey(AES_KEY_AREA_6);
            AESInvalidateKey(AES_KEY_AREA_7);

            /* This powers down all sub-modules of the crypto module until needed.
             * It does not power down the crypto module at PRCM level and provides
             * small power savings.
             */
            AESSelectAlgorithm(0x0);

            status = AESCTR_STATUS_ERROR;
        }
    }

    if (status == AESCTR_STATUS_SUCCESS)
    {
        /* Load counter value */
        AESSetInitializationVector(object->counter);

        ctrlVal = CRYPTO_AESCTL_CTR |
                  CRYPTO_AESCTL_CTR_WIDTH_128_BIT;

        if ((object->operationType & AESCTR_OP_MODE_MASK) == AESCTR_MODE_ENCRYPT)
        {
            ctrlVal |= CRYPTO_AESCTL_DIR;
        }

        /* Context save is only necessary in the middle of segmented
         * operation.
         */
        if (object->operationType & AESCTR_OP_FLAG_SEGMENTED)
        {
            ctrlVal |= CRYPTO_AESCTL_SAVE_CONTEXT;
        }

        AESSetCtrl(ctrlVal);
        AESSetDataLength(object->inputLength);
        AESSetAuthLength(0);

        object->hwBusy = true;

        if (object->returnBehavior != AESCTR_RETURN_BEHAVIOR_POLLING)
        {
            /* Prevent system from entering standby and powering down Crypto
             * peripheral while the operation is running.
             */
            Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);
        }

        /* Start the input/output DMA */
        AESStartDMAOperation(object->input, object->inputLength,
                             object->output, object->inputLength);

        status = AESCTR_waitForResult(object);
    }

    if (status != AESCTR_STATUS_SUCCESS)
    {
        /* Save the failure status in case the application ignores the return value
         * so the driver can reject any attempts to continue the failed operation.
         */
        object->returnStatus = status;
    }

    return status;
}

/*
 *  ======== AESCTR_setOperationInProgress ========
 */
static int_fast16_t AESCTR_setOperationInProgress(AESCTRCC26XX_Object *object)
{
    uintptr_t interruptKey = HwiP_disable();
    if(object->operationInProgress)
    {
        HwiP_restore(interruptKey);
        return AESCTR_STATUS_ERROR;
    }
    object->operationInProgress = true;
    HwiP_restore(interruptKey);
    return AESCTR_STATUS_SUCCESS;
}

/*
 *  ======== AESCTR_initCounter ========
 */
static void AESCTR_initCounter(AESCTRCC26XX_Object *object,
                               const uint8_t *initialCounter)
{
    if (initialCounter != NULL)
    {
        memcpy(object->counter, initialCounter, sizeof(object->counter));
    }
    else
    {
        memset(object->counter, 0, sizeof(object->counter));
    }
}

/*
 *  ======== AESCTR_startOneStepOperation ========
 */
static int_fast16_t AESCTR_startOneStepOperation(AESCTR_Handle handle,
                                                 AESCTR_OneStepOperation *operation,
                                                 AESCTR_OperationType operationType)
{
    DebugP_assert(handle);
    DebugP_assert(operation);
    DebugP_assert(operation->key);
    /* No need to assert operationType since we control it within the driver */

    AESCTRCC26XX_Object *object = handle->object;
    int_fast16_t status;

    /* Verify input length is non-zero */
    if (operation->inputLength == 0)
    {
        return AESCTR_STATUS_ERROR;
    }

    /* Check that there is no operation already in progress for this driver
     * instance.
     */
    status = AESCTR_setOperationInProgress(object);

    if (status != AESCTR_STATUS_SUCCESS)
    {
        return status;
    }

    if (object->threadSafe)
    {
        if (!CryptoResourceCC26XX_acquireLock(object->semaphoreTimeout))
        {
            return AESCTR_STATUS_RESOURCE_UNAVAILABLE;
        }

        object->cryptoResourceLocked = true;
    }

    object->operation = (AESCTR_OperationUnion *)operation;
    object->operationType = operationType;
    /* We will only change the returnStatus if there is an error or cancellation */
    object->returnStatus = AESCTR_STATUS_SUCCESS;

    /* Make internal copy of operational params */
    object->key = *(operation->key);
    object->input = operation->input;
    object->inputLength = operation->inputLength;
    object->output = operation->output;

    AESCTR_initCounter(object, operation->initialCounter);

    status = AESCTR_processData(handle);

    if ((status != AESCTR_STATUS_SUCCESS) &&
        (object->cryptoResourceLocked))
    {
        CryptoResourceCC26XX_releaseLock();
        object->cryptoResourceLocked = false;
    }

    return status;
}

/*
 *  ======== AESCTR_waitForResult ========
 */
static inline int_fast16_t AESCTR_waitForResult(AESCTRCC26XX_Object *object)
{
    int_fast16_t status = AESCTR_STATUS_ERROR;

    if (object->returnBehavior == AESCTR_RETURN_BEHAVIOR_POLLING)
    {
        /* Wait until the operation is complete and check for DMA errors */
        if (AESWaitForIRQFlags(AES_RESULT_RDY | AES_DMA_BUS_ERR) &
            AES_DMA_BUS_ERR)
        {
            object->returnStatus = AESCTR_STATUS_ERROR;
        }
        else /* Must be RESULT_AVAIL because DMA_IN_DONE was disabled */
        {
            if (AESGetCtrl() & CRYPTO_AESCTL_SAVE_CONTEXT_M)
            {
                /* Save counter value if the context was saved */
                AESReadNonAuthenticationModeIV(object->counter);
            }
        }

        /* Mark that we are done with the operation */
        object->hwBusy = false;

        /* Save the object's returnStatus before clearing operationInProgress or
         * posting the access semaphore in case it is overwritten.
         */
        status = object->returnStatus;

        if (!(object->operationType & AESCTR_OP_FLAG_SEGMENTED))
        {
            /* One-step or finalization operation is complete */
            object->operationInProgress = false;
        }

        AESCTR_cleanup(object);
    }
    else if (object->returnBehavior == AESCTR_RETURN_BEHAVIOR_BLOCKING)
    {
        SemaphoreP_pend(&CryptoResourceCC26XX_operationSemaphore,
                        SemaphoreP_WAIT_FOREVER);

        status = object->returnStatus;
    }
    else /* AESCTR_RETURN_BEHAVIOR_CALLBACK */
    {
        /* AESCTR_STATUS_SUCCESS is always returned in callback mode */
        status = AESCTR_STATUS_SUCCESS;
    }

    return status;
}

/*
 *  ======== AESCTR_oneStepEncrypt ========
 */
int_fast16_t AESCTR_oneStepEncrypt(AESCTR_Handle handle, AESCTR_OneStepOperation *operationStruct)
{
    return AESCTR_startOneStepOperation(handle, operationStruct, AESCTR_OPERATION_TYPE_ENCRYPT);
}

/*
 *  ======== AESCTR_oneStepDecrypt ========
 */
int_fast16_t AESCTR_oneStepDecrypt(AESCTR_Handle handle, AESCTR_OneStepOperation *operationStruct)
{
    return AESCTR_startOneStepOperation(handle, operationStruct, AESCTR_OPERATION_TYPE_DECRYPT);
}


/*
 *  ======== AESCTR_setupSegmentedOperation ========
 */
static int_fast16_t AESCTR_setupSegmentedOperation(AESCTRCC26XX_Object *object,
                                                   const CryptoKey *key,
                                                   const uint8_t *initialCounter)
{
    DebugP_assert(key);

    /* Key material pointer and length are not checked until adding or
     * finalizing data.
     */

    /* Check that there is no operation already in progress for this driver
     * instance.
     */
    int_fast16_t status = AESCTR_setOperationInProgress(object);

    if (status == AESCTR_STATUS_SUCCESS)
    {
        /* We will only change the returnStatus if there is an error or cancellation */
        object->returnStatus = AESCTR_STATUS_SUCCESS;

        /* Make internal copy of crypto key */
        object->key = *key;

        AESCTR_initCounter(object, initialCounter);
    }

    return status;
}



/*
 *  ======== AESCTR_setupEncrypt ========
 */
int_fast16_t AESCTR_setupEncrypt(AESCTR_Handle handle, const CryptoKey *key,
                                 const uint8_t *initialCounter)
{
    DebugP_assert(handle);
    AESCTRCC26XX_Object *object = handle->object;
    DebugP_assert(object);

    int_fast16_t status = AESCTR_setupSegmentedOperation(object, key,
                                                         initialCounter);

    if (status == AESCTR_STATUS_SUCCESS)
    {
        object->operationType = AESCTR_OPERATION_TYPE_ENCRYPT_SEGMENTED;
    }

    return status;
}

/*
 *  ======== AESCTR_setupDecrypt ========
 */
int_fast16_t AESCTR_setupDecrypt(AESCTR_Handle handle, const CryptoKey *key,
                                 const uint8_t *initialCounter)
{
    DebugP_assert(handle);
    AESCTRCC26XX_Object *object = handle->object;
    DebugP_assert(object);

    int_fast16_t status = AESCTR_setupSegmentedOperation(object, key,
                                                         initialCounter);

    if (status == AESCTR_STATUS_SUCCESS)
    {
        object->operationType = AESCTR_OPERATION_TYPE_DECRYPT_SEGMENTED;
    }

    return status;
}

/*
 *  ======== AESCTR_addData ========
 */
int_fast16_t AESCTR_addData(AESCTR_Handle handle,
                            AESCTR_SegmentedOperation *operation)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESCTRCC26XX_Object *object = handle->object;
    int_fast16_t status;

    /* Check for previous failure or cancellation of segmented operation */
    if (object->returnStatus != AESCTR_STATUS_SUCCESS)
    {
        /* Return the status of the previous call.
         * The callback function will not be executed.
         */
        return (object->returnStatus);
    }

    /* Assert the segmented operation was setup */
    DebugP_assert((object->operationType == AESCTR_OPERATION_TYPE_ENCRYPT_SEGMENTED) ||
                  (object->operationType == AESCTR_OPERATION_TYPE_DECRYPT_SEGMENTED));

    /* Verify the input length is non-zero and a multiple of the block size */
    if ((operation->inputLength == 0U) ||
        (operation->inputLength & AES_NON_BLOCK_MULTIPLE_MASK))
    {
        return AESCTR_STATUS_ERROR;
    }

    if (object->threadSafe)
    {
        if (!CryptoResourceCC26XX_acquireLock(object->semaphoreTimeout))
        {
            return AESCTR_STATUS_RESOURCE_UNAVAILABLE;
        }

        object->cryptoResourceLocked = true;
    }

    object->operation = (AESCTR_OperationUnion *)operation;

    /* Make internal copy of operational params */
    object->input = operation->input;
    object->inputLength = operation->inputLength;
    object->output = operation->output;

    status = AESCTR_processData(handle);

    if ((status != AESCTR_STATUS_SUCCESS) &&
        (object->cryptoResourceLocked))
    {
        CryptoResourceCC26XX_releaseLock();
        object->cryptoResourceLocked = false;
    }

    return status;
}

/*
 *  ======== AESCTR_finalize ========
 */
int_fast16_t AESCTR_finalize(AESCTR_Handle handle,
                             AESCTR_SegmentedOperation *operation)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESCTRCC26XX_Object *object = handle->object;
    AESCTR_OperationType operationType = object->operationType;
    int_fast16_t status = AESCTR_STATUS_ERROR;

    /* Check for previous failure of segmented operation */
    if (object->returnStatus != AESCTR_STATUS_SUCCESS)
    {
        /* Return the failure status of previous call.
         * The callback will not be called.
         */
        return (object->returnStatus);
    }

    /* Assert the segmented operation was setup */
    DebugP_assert((object->operationType == AESCTR_OPERATION_TYPE_ENCRYPT_SEGMENTED) ||
                  (object->operationType == AESCTR_OPERATION_TYPE_DECRYPT_SEGMENTED));

    /* Determine final operation type but do not save to object until
     * we have obtained access to CRYPTO resource or there is no input
     * to process. This allows app to retry finalization if the CRYPTO
     * resource is unavailable.
     */
    if (operationType == AESCTR_OPERATION_TYPE_ENCRYPT_SEGMENTED)
    {
        operationType = AESCTR_OPERATION_TYPE_ENCRYPT_FINALIZE;
    }
    else
    {
        operationType = AESCTR_OPERATION_TYPE_DECRYPT_FINALIZE;
    }

    if (operation->inputLength > 0)
    {
        /* Try and obtain access to the crypto module */
        if (object->threadSafe)
        {
            if (!CryptoResourceCC26XX_acquireLock(object->semaphoreTimeout))
            {
                return AESCTR_STATUS_RESOURCE_UNAVAILABLE;
            }

            object->cryptoResourceLocked = true;
        }

        object->operationType = operationType;
        object->operation = (AESCTR_OperationUnion *)operation;

        /* Make internal copy of operational params */
        object->input = operation->input;
        object->inputLength = operation->inputLength;
        object->output = operation->output;

        status = AESCTR_processData(handle);

        if ((status != AESCTR_STATUS_SUCCESS) &&
            (object->cryptoResourceLocked))
        {
            CryptoResourceCC26XX_releaseLock();
            object->cryptoResourceLocked = false;
        }
    }
    else /* Operation was finalized without additional data to process */
    {
        /* Save the object's returnStatus in case it is
         * overwritten during setup of a new segmented operation
         * after the operationInProgress flag is cleared.
         */
        status = object->returnStatus;

        object->operationInProgress = false;

        if (object->returnBehavior == AESCTR_RETURN_BEHAVIOR_CALLBACK)
        {
            /* Call the callback function provided by the application */
            object->callbackFxn(handle, status,
                                (AESCTR_OperationUnion *)operation,
                                operationType);

            /* Always return success in callback mode */
            status = AESCTR_STATUS_SUCCESS;
        }
    }

    return status;
}

/*
 *  ======== AESCTR_cancelOperation ========
 */
int_fast16_t AESCTR_cancelOperation(AESCTR_Handle handle)
{
    AESCTRCC26XX_Object *object = handle->object;
    uintptr_t interruptKey;

    interruptKey = HwiP_disable();

    /* Check if the HW operation already completed or was never started */
    if (!object->hwBusy)
    {
        object->returnStatus = AESCTR_STATUS_CANCELED;
        object->operationInProgress = false;

        HwiP_restore(interruptKey);

        /* No need to call the callback function provided by the application
         * since it would have already been called when the HW operation
         * completed.
         */

        return AESCTR_STATUS_SUCCESS;
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

    object->returnStatus = AESCTR_STATUS_CANCELED;
    object->hwBusy = false;
    object->operationInProgress = false;

    if (object->returnBehavior == AESCTR_RETURN_BEHAVIOR_BLOCKING)
    {
        /* Unblock the pending task to signal that the operation is complete. */
        SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
    }
    else /* AESCTR_RETURN_BEHAVIOR_CALLBACK */
    {
        /* Call the callback function provided by the application. */
        object->callbackFxn(handle, AESCTR_STATUS_CANCELED,
                            object->operation, object->operationType);
    }

    /* Cleanup posts the crypto access semaphore and must be done after the
     * operational semaphore is posted to avoid a potential race condition
     * when starting a new operation using a different driver instance.
     */
    AESCTR_cleanup(object);

    /* Always return success */
    return AESCTR_STATUS_SUCCESS;
}

bool AESCTR_acquireLock(AESCTR_Handle handle, uint32_t timeout)
{
    return CryptoResourceCC26XX_acquireLock(timeout);
}

void AESCTR_releaseLock(AESCTR_Handle handle)
{
    CryptoResourceCC26XX_releaseLock();
}

void AESCTR_enableThreadSafety(AESCTR_Handle handle)
{
    AESCTRCC26XX_Object *object = handle->object;

    object->threadSafe = true;
}

void AESCTR_disableThreadSafety(AESCTR_Handle handle)
{
    AESCTRCC26XX_Object *object = handle->object;

    object->threadSafe = false;
}
