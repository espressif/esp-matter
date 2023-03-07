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
#include <ti/drivers/dpl/SwiP.h>

#include <ti/drivers/AESCMAC.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/aescmac/AESCMACCC26XX.h>
#include <ti/drivers/cryptoutils/utils/CryptoUtils.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>
#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_crypto.h)
#include DeviceFamily_constructPath(driverlib/aes.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)

#define AES_BLOCK_SIZE_WORDS  (AES_BLOCK_SIZE / 4)

#define AES_NON_BLOCK_MULTIPLE_MASK 0x0FU

#define AESCMAC_CONST_RB    ((uint8_t)0x87U)
#define AESCMAC_MSB_CHECK   ((uint8_t)0x80U)
#define AESCMAC_PADDING     ((uint8_t)0x80U)

typedef enum
{
    AESCMAC_SUBKEY1,
    AESCMAC_SUBKEY2
} AESCMAC_SUBKEY_NUM;

/* Forward declarations */
static void AESCMAC_hwiFxn(uintptr_t arg0);
static int_fast16_t AESCMAC_setOperationInProgress(AESCMACCC26XX_Object *object);
static int_fast16_t AESCMAC_startOneStepOperation(AESCMAC_Handle handle,
                                                  AESCMAC_Operation *operation,
                                                  CryptoKey *key,
                                                  AESCMAC_OperationType operationType);
static int_fast16_t AESCMAC_processData(AESCMAC_Handle handle);
static inline int_fast16_t AESCMAC_waitForResult(AESCMACCC26XX_Object *object);
static void AESCMAC_cleanup(AESCMACCC26XX_Object *object);
static void AESCMAC_getResult(AESCMACCC26XX_Object *object,
                              bool isAfterDMAExecution);
static inline void AESCMAC_prepareFinalInputBlock(AESCMACCC26XX_Object *object,
                                                  size_t *transactionLength);
static inline void AESCMAC_processFinalInputBlock(AESCMACCC26XX_Object *object);
static int_fast16_t AESCMAC_setupSegmentedOperation(AESCMACCC26XX_Object *object,
                                                    const CryptoKey *key);
static int_fast16_t AESCMAC_loadKey(AESCMACCC26XX_Object *object);
static int_fast16_t AESCMAC_loadContext(AESCMACCC26XX_Object *object,
                                        size_t inputLength);
static void AESCMAC_deriveSubKey(uint8_t *buffer);
static inline void AESCMAC_xorBlock(uint32_t *block1_dst,
                                    const uint32_t *block2);
static int_fast16_t AESCMAC_generateSubKey(AESCMACCC26XX_Object *object,
                                           AESCMAC_SUBKEY_NUM subKeyNum,
                                           uint32_t *subKey);

/* Static globals */
static bool AESCMAC_isInitialized = false;

/*
 *  ======== AESCMAC_processFinalInputBlock ========
 */
static inline void AESCMAC_processFinalInputBlock(AESCMACCC26XX_Object *object)
{
    object->returnStatus = AESCMAC_loadContext(object, AES_BLOCK_SIZE);

    /* AESCMAC_loadContext() calls AESWriteKeyStore() which enables
     * CRYPTO IRQ so we must disable it to poll for completion of
     * the final input block.
     */
    IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);

    if (object->returnStatus == AESCMAC_STATUS_SUCCESS)
    {
        /* Start the input DMA. There is no output DMA. */
        AESStartDMAOperation((uint8_t *)object->buffer, AES_BLOCK_SIZE, NULL, 0U);

        /* Wait until the operation is complete and check for DMA errors */
        if (AESWaitForIRQFlags(AES_RESULT_RDY | AES_DMA_BUS_ERR) & AES_DMA_BUS_ERR)
        {
            object->returnStatus = AESCMAC_STATUS_ERROR;
        }
        else
        {
            if (AESReadTag((uint8_t *)object->intermediateTag, AES_BLOCK_SIZE) != AES_SUCCESS)
            {
                object->returnStatus = AESCMAC_STATUS_ERROR;
            }
        }

        IntPendClear(INT_CRYPTO_RESULT_AVAIL_IRQ);
    }
}

/*
 *  ======== AESCMAC_getResult ========
 */
static void AESCMAC_getResult(AESCMACCC26XX_Object *object,
                              bool isAfterDMAExecution)
{
    AESCMAC_Operation *operation = object->operation;
    uint8_t opcode = (object->operationType & AESCMAC_OP_CODE_MASK);

    /* If DMA was executed, read the output tag */
    if (isAfterDMAExecution)
    {
        if (AESReadTag((uint8_t *)object->intermediateTag, AES_BLOCK_SIZE) != AES_SUCCESS)
        {
            object->returnStatus = AESCMAC_STATUS_ERROR;
        }
    }

    /* If CMAC One-step or Final operation, process the final input block */
    if ((object->operationalMode == AESCMAC_OPMODE_CMAC)  &&
        (opcode != AESCMAC_OP_CODE_SEGMENTED) &&
        (object->returnStatus == AESCMAC_STATUS_SUCCESS))
    {
        AESCMAC_processFinalInputBlock(object);
    }

    /* If One-step or Final operation, verify or copy the MAC */
    if ((opcode != AESCMAC_OP_CODE_SEGMENTED) &&
        (object->returnStatus == AESCMAC_STATUS_SUCCESS))
    {
        if (object->operationType & AESCMAC_OP_FLAG_SIGN)
        {
            memcpy(operation->mac, object->intermediateTag,
                   operation->macLength);
        }
        else
        {
            /* Constant time comparison of output tag versus provided MAC */
            if (!CryptoUtils_buffersMatch(object->intermediateTag, operation->mac,
                                          operation->macLength))
            {
                object->returnStatus = AESCMAC_STATUS_MAC_INVALID;
            }
        }
    }
}

/*
 *  ======== AESCMAC_hwiFxn ========
 */
static void AESCMAC_hwiFxn(uintptr_t arg0)
{
    AESCMACCC26XX_Object *object = ((AESCMAC_Handle)arg0)->object;
    uint8_t opcode = (object->operationType & AESCMAC_OP_CODE_MASK);
    uintptr_t interruptKey;

    if (AESIntStatusRaw() & AES_DMA_BUS_ERR)
    {
        /* Propagate the DMA error from driverlib to the application */
        object->returnStatus = AESCMAC_STATUS_ERROR;
    }
    else /* Must be RESULT_AVAIL because DMA_IN_DONE was disabled */
    {
        AESCMAC_getResult(object, true);
    }

    /* AESCMAC_getResult() needs to perform a polling HW operation to process
     * the final input block for CMAC so hwBusy and operationInProgress flags
     * are not cleared until afterward.
     */
    interruptKey = HwiP_disable();

    /* Mark that we are done with the operation so that
     * AESCMAC_cancelOperation knows not to try canceling.
     */
    object->hwBusy = false;

    /* Mark operation as no longer in-progress if one-step or final operation */
    if (opcode != AESCMAC_OP_CODE_SEGMENTED)
    {
        object->operationInProgress = false;
    }

    HwiP_restore(interruptKey);

    AESIntClear(AES_RESULT_RDY | AES_DMA_BUS_ERR);

    /* Handle clean up of the operation: Invalidate the key,
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
    AESCMAC_cleanup(object);

    if (object->returnBehavior == AESCMAC_RETURN_BEHAVIOR_BLOCKING)
    {
        /* Unblock the pending task to signal that the operation is complete */
        SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
    }
    else /* AESCMAC_RETURN_BEHAVIOR_CALLBACK */
    {
        /* Call the callback function provided by the application */
        object->callbackFxn((AESCMAC_Handle)arg0, object->returnStatus,
                            object->operation, object->operationType);
    }
}

/*
 *  ======== AESCMAC_cleanup ========
 */
static void AESCMAC_cleanup(AESCMACCC26XX_Object *object)
{
    /* Since plaintext keys use two reserved (by convention) slots in the
     * keystore, the slots must be invalidated to prevent its re-use without
     * reloading the key material again.
     */
    AESInvalidateKey(AES_KEY_AREA_6);
    AESInvalidateKey(AES_KEY_AREA_7);

    /* NOTE: IV register may contain intermediate or final tag but we are
     * not required to clear it since this information is not valuable to
     * an attacker.
     */

    /* This powers down all sub-modules of the crypto module until needed.
     * It does not power down the crypto module at PRCM level and provides
     * small power savings.
     */
    AESSelectAlgorithm(0U);

    if (object->returnBehavior != AESCMAC_RETURN_BEHAVIOR_POLLING)
    {
        Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
    }

    /* Grant access for other threads to use the crypto module.
     * The semaphore must be posted before the callbackFxn to allow the
     * chaining of operations.
     */
    if (object->cryptoResourceLocked)
    {
        CryptoResourceCC26XX_releaseLock();
        object->cryptoResourceLocked = false;
    }
}

/*
 *  ======== AESCMAC_init ========
 */
void AESCMAC_init(void)
{
    CryptoResourceCC26XX_constructRTOSObjects();

    AESCMAC_isInitialized = true;
}

/*
 *  ======== AESCMAC_construct ========
 */
AESCMAC_Handle AESCMAC_construct(AESCMAC_Config *config,
                                 const AESCMAC_Params *params)
{
    DebugP_assert(config);
    DebugP_assert(params);

    AESCMAC_Handle handle = config;
    AESCMACCC26XX_Object *object = handle->object;

    DebugP_assert(object);

    uintptr_t interruptKey = HwiP_disable();

    if (!AESCMAC_isInitialized || object->isOpen)
    {
        HwiP_restore(interruptKey);
        return NULL;
    }

    object->isOpen = true;

    HwiP_restore(interruptKey);

    /* If params are NULL, use defaults */
    if (params == NULL)
    {
        params = (AESCMAC_Params *)&AESCMAC_defaultParams;
    }

    DebugP_assert((params->returnBehavior != AESCMAC_RETURN_BEHAVIOR_CALLBACK) ||
                  (params->callbackFxn != NULL));

    object->returnBehavior = params->returnBehavior;
    object->operationalMode = params->operationalMode;
    object->callbackFxn = params->callbackFxn;
    if (params->returnBehavior == AESCMAC_RETURN_BEHAVIOR_BLOCKING)
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

    return handle;
}

/*
 *  ======== AESCMAC_close ========
 */
void AESCMAC_close(AESCMAC_Handle handle)
{
    DebugP_assert(handle);

    /* Get the pointer to the object and hwAttrs */
    AESCMACCC26XX_Object *object = handle->object;

    /* Mark the module as available */
    object->isOpen = false;

    /* Release power dependency on Crypto Module */
    Power_releaseDependency(PowerCC26XX_PERIPH_CRYPTO);
}

/*
 *  ======== AESCMAC_loadKey ========
 */
static int_fast16_t AESCMAC_loadKey(AESCMACCC26XX_Object *object)
{
    uint16_t keyLength;
    const uint8_t *keyData;
    int_fast16_t status = AESCMAC_STATUS_SUCCESS;

    /* Only plaintext CryptoKeys are supported for now */
    DebugP_assert((object->key.encoding == CryptoKey_PLAINTEXT) ||
                  (object->key.encoding == CryptoKey_BLANK_PLAINTEXT));

    keyLength = object->key.u.plaintext.keyLength;
    keyData = object->key.u.plaintext.keyMaterial;

    DebugP_assert(keyData);
    DebugP_assert((keyLength == AES_128_KEY_LENGTH_BYTES) ||
                  (keyLength == AES_192_KEY_LENGTH_BYTES) ||
                  (keyLength == AES_256_KEY_LENGTH_BYTES));

    /* Workaround for a bug in AESWriteToKeyStore() as it invalidates only
     * the key area provided as an argument and not the subsequent key
     * area which is required when using key lengths greater than 128-bits.
     */
    AESInvalidateKey(AES_KEY_AREA_7);

    /* Load the key from RAM or flash into the key store at a hardcoded and
     * reserved location.
     */
    if (AESWriteToKeyStore(keyData, keyLength, AES_KEY_AREA_6) != AES_SUCCESS)
    {
        status = AESCMAC_STATUS_ERROR;
    }

    if (status == AESCMAC_STATUS_SUCCESS)
    {
        /* AESWriteToKeyStore() enables both DMA_IN_DONE and RESULT_AVAIL
         * interrupts but since this driver only depends on RESULT_AVAIL,
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
            AESSelectAlgorithm(0U);

            status = AESCMAC_STATUS_ERROR;
        }

        /* Write zeros to AES_KEY2 and AES_KEY3 registers */
        AESCBCMACClearKeys();
    }

    return status;
}

/*
 *  ======== AESCMAC_loadContext ========
 */
static int_fast16_t AESCMAC_loadContext(AESCMACCC26XX_Object *object,
                                        size_t inputLength)
{
    int_fast16_t status;

    status = AESCMAC_loadKey(object);

    if (status == AESCMAC_STATUS_SUCCESS)
    {
        /* Set IV to intermediate tag (initialized to zero at the start
         * of a new operation).
         */
        AESSetInitializationVector(object->intermediateTag);

        AESSetCtrl(CRYPTO_AESCTL_CBC_MAC | CRYPTO_AESCTL_SAVE_CONTEXT |
                   CRYPTO_AESCTL_DIR);

        AESSetDataLength(inputLength);
        AESSetAuthLength(0U);
    }

    return status;
}

/*
 *  ======== AESCMAC_processData ========
 */
static int_fast16_t AESCMAC_processData(AESCMAC_Handle handle)
{
    AESCMACCC26XX_Object *object = handle->object;
    AESCMAC_Operation *operation = object->operation;
    AESCMAC_OperationType operationType;
    bool isResultHandled = false;
    int_fast16_t status = AESCMAC_STATUS_ERROR;
    uint8_t opcode = (object->operationType & AESCMAC_OP_CODE_MASK);
    AESCMACCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    size_t transactionLength = operation->inputLength;

    /* Input pointer cannot be NULL if input length is non-zero */
    DebugP_assert((operation->inputLength == 0U) || operation->input);

    /* MAC pointer cannot be NULL if performing a one-step operation or
     * finalizing a segmented operation */
    DebugP_assert((opcode == AESCMAC_OP_CODE_SEGMENTED) ||
                  (operation->mac));

    /* We need to set the HWI function and priority since the same physical
     * interrupt is shared by multiple drivers and they all need to coexist.
     * Whenever a driver starts an operation, it registers its HWI callback with
     * the OS.
     */
    HwiP_setFunc(&CryptoResourceCC26XX_hwi, AESCMAC_hwiFxn,
                 (uintptr_t)handle);
    HwiP_setPriority(INT_CRYPTO_RESULT_AVAIL_IRQ, hwAttrs->intPriority);

    /* If CMAC One-step or Finalization operation, prepare the final input
     * block and adjust the transaction length accordingly.
     */
    if ((object->operationalMode == AESCMAC_OPMODE_CMAC) &&
        (opcode != AESCMAC_OP_CODE_SEGMENTED))
    {
        AESCMAC_prepareFinalInputBlock(object, &transactionLength);
    }

    if (object->returnStatus == AESCMAC_STATUS_SUCCESS)
    {
        if (transactionLength == 0U)
        {
            /* If transaction length is zero, only the last locally buffered block
             * of data remains to be processed for CMAC. Call AESCMAC_getResult()
             * to process the last block, obtain the result, and store status of
             * the operation in object->returnStatus.
             */
            AESCMAC_getResult(object, false);
        }
        else
        {
            status = AESCMAC_loadContext(object, transactionLength);

            if (status == AESCMAC_STATUS_SUCCESS)
            {
                if (object->returnBehavior == AESCMAC_RETURN_BEHAVIOR_POLLING)
                {
                    /* AESCMAC_loadContext() calls AESWriteKeyStore() which enables
                     * CRYPTO IRQ so we must disable it when in polling mode.
                     */
                    IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);
                }
                else
                {
                    /* Prevent system from entering standby and powering down Crypto
                     * peripheral while the operation is running.
                     */
                    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);
                }

                object->hwBusy = true;

                /* Start the input DMA. There is no output DMA. */
                AESStartDMAOperation(operation->input, transactionLength, NULL, 0U);

                status = AESCMAC_waitForResult(object);
                isResultHandled = true;
            }
        }
    }

    if (!isResultHandled)
    {
        /* Save the object's data to provide to callback in case it
         * is overwritten during by the start of a new operation
         * after the operationInProgress flag is cleared or access
         * semaphore is posted.
         */
        status = object->returnStatus;
        operationType = object->operationType;

        object->operationInProgress = false;

        /* Handle clean up of the operation: Invalidate the key,
         * release power constraint, and post access semaphore to allow
         * callback to chain operations.
         */
        AESCMAC_cleanup(object);

        if (object->returnBehavior == AESCMAC_RETURN_BEHAVIOR_CALLBACK)
        {
            /* Call the callback function provided by the application
             * to provide actual status of the operation.
             */
            object->callbackFxn(handle, status,
                                operation, operationType);

            /* Always return success in callback mode */
            status = AESCMAC_STATUS_SUCCESS;
        }
    }
    else if (status != AESCMAC_STATUS_SUCCESS)
    {
        /* Save the failure status in case the application ignores the return
         * value so the driver can reject any attempts to continue a failed
         * segmented operation.
         */
        object->returnStatus = status;
    }

    return status;
}

/*
 *  ======== AESCMAC_xorBlock ========
 *  XOR's two 16-byte blocks, storing the result in block1_dst.
 */
static inline void AESCMAC_xorBlock(uint32_t *block1_dst,
                                    const uint32_t *block2)
{
    uint_fast8_t i;

    for (i = 0U; i < AES_BLOCK_SIZE_WORDS; i++)
    {
        block1_dst[i] = block1_dst[i] ^ block2[i];
    }
}

/*
 *  ======== AESCMAC_prepareFinalInputBlock ========
 */
static inline void AESCMAC_prepareFinalInputBlock(AESCMACCC26XX_Object *object,
                                                  size_t *transactionLength)
{
    AESCMAC_Operation *operation = object->operation;
    size_t finalInputLength = 0U;
    size_t truncatedInputLength = 0U;
    uint32_t subKey[AES_BLOCK_SIZE_WORDS];

    /* Copy last partial or full block of input into local buffer */
    memset(object->buffer, 0, AES_BLOCK_SIZE);

    if (operation->inputLength != 0U)
    {
        finalInputLength = operation->inputLength & AES_NON_BLOCK_MULTIPLE_MASK;

        if (finalInputLength == 0U)
        {
            finalInputLength = AES_BLOCK_SIZE;
        }

        truncatedInputLength = operation->inputLength - finalInputLength;

        memcpy(object->buffer, &operation->input[truncatedInputLength],
               finalInputLength);
    }

    /* Check if input message length is a positive block multiple */
    if ((operation->inputLength != 0U) && (finalInputLength == AES_BLOCK_SIZE))
    {
        /* Generate subkey1 */
        object->returnStatus = AESCMAC_generateSubKey(object, AESCMAC_SUBKEY1, subKey);
    }
    else
    {
        /* Generate subkey2 */
        object->returnStatus = AESCMAC_generateSubKey(object, AESCMAC_SUBKEY2, subKey);

        /* Set padding byte if partial block */
        ((uint8_t *)object->buffer)[finalInputLength] = AESCMAC_PADDING;
    }

    if (object->returnStatus == AESCMAC_STATUS_SUCCESS)
    {
        /* XOR final block with subkey */
        AESCMAC_xorBlock(object->buffer, subKey);

        *transactionLength = truncatedInputLength;
    }
}


/*
 *  ======== AESCMAC_startOneStepOperation ========
 */
static int_fast16_t AESCMAC_startOneStepOperation(AESCMAC_Handle handle,
                                                  AESCMAC_Operation *operation,
                                                  CryptoKey *key,
                                                  AESCMAC_OperationType operationType)
{
    DebugP_assert(handle);
    DebugP_assert(operation);
    DebugP_assert(key);
    /* No need to assert operationType since we control it within the driver */

    AESCMACCC26XX_Object *object = handle->object;
    int_fast16_t status;

    /* CBC-MAC is not permitted for zero length messages */
    if ((object->operationalMode == AESCMAC_OPMODE_CBCMAC) &&
        (operation->inputLength == 0))
    {
        return AESCMAC_STATUS_ERROR;
    }

    /* Check that there is no operation already in progress for this driver
     * instance.
     */
    status = AESCMAC_setOperationInProgress(object);

    if (status != AESCMAC_STATUS_SUCCESS)
    {
        return status;
    }

    if (object->threadSafe)
    {
        if (!CryptoResourceCC26XX_acquireLock(object->semaphoreTimeout))
        {
            return AESCMAC_STATUS_RESOURCE_UNAVAILABLE;
        }

        object->cryptoResourceLocked = true;
    }

    object->operation = operation;
    object->operationType = operationType;
    /* We will only change the returnStatus if there is an error or cancellation */
    object->returnStatus = AESCMAC_STATUS_SUCCESS;
    /* Make internal copy of crypto key */
    object->key = *key;

    /* Zero the intermediate tag because it will be used as the IV */
    memset(object->intermediateTag, 0, sizeof(object->intermediateTag));

    status = AESCMAC_processData(handle);

    if ((status != AESCMAC_STATUS_SUCCESS) &&
        (object->cryptoResourceLocked))
    {
        CryptoResourceCC26XX_releaseLock();
        object->cryptoResourceLocked = false;
    }

    return status;
}

/*
 *  ======== AESCMAC_waitForResult ========
 */
static inline int_fast16_t AESCMAC_waitForResult(AESCMACCC26XX_Object *object)
{
    int_fast16_t status = AESCMAC_STATUS_ERROR;
    uint8_t opcode = (object->operationType & AESCMAC_OP_CODE_MASK);

    if (object->returnBehavior == AESCMAC_RETURN_BEHAVIOR_POLLING)
    {
        /* Wait until the operation is complete and check for DMA errors */
        if (AESWaitForIRQFlags(AES_RESULT_RDY | AES_DMA_BUS_ERR) &
            AES_DMA_BUS_ERR)
        {
            object->returnStatus = AESCMAC_STATUS_ERROR;
        }
        else
        {
            AESCMAC_getResult(object, true);
        }

        /* Mark that we are done with the operation */
        object->hwBusy = false;

        /* Save the object's returnStatus before clearing operationInProgress or
         * posting the access semaphore in case it is overwritten.
         */
        status = object->returnStatus;

        /* Mark operation as no longer in progress if one-step or final operation */
        if (opcode != AESCMAC_OP_CODE_SEGMENTED)
        {
            object->operationInProgress = false;
        }

        AESCMAC_cleanup(object);
    }
    else if (object->returnBehavior == AESCMAC_RETURN_BEHAVIOR_BLOCKING)
    {
        SemaphoreP_pend(&CryptoResourceCC26XX_operationSemaphore,
                        SemaphoreP_WAIT_FOREVER);

        status = object->returnStatus;
    }
    else /* AESCMAC_RETURN_BEHAVIOR_CALLBACK */
    {
        /* AESCMAC_STATUS_SUCCESS is always returned in callback mode */
        status = AESCMAC_STATUS_SUCCESS;
    }

    return status;
}

/*
 *  ======== AESCMAC_oneStepSign ========
 */
int_fast16_t AESCMAC_oneStepSign(AESCMAC_Handle handle,
                                 AESCMAC_Operation *operation,
                                 CryptoKey *key)
{
    return AESCMAC_startOneStepOperation(handle, operation, key,
                                         AESCMAC_OP_TYPE_SIGN);
}

/*
 *  ======== AESCMAC_oneStepVerify ========
 */
int_fast16_t AESCMAC_oneStepVerify(AESCMAC_Handle handle,
                                   AESCMAC_Operation *operation,
                                   CryptoKey *key)
{
    return AESCMAC_startOneStepOperation(handle, operation, key,
                                         AESCMAC_OP_TYPE_VERIFY);
}

/*
 *  ======== AESCMAC_setOperationInProgress ========
 */
static int_fast16_t AESCMAC_setOperationInProgress(AESCMACCC26XX_Object *object)
{
    uintptr_t interruptKey = HwiP_disable();
    if(object->operationInProgress)
    {
        HwiP_restore(interruptKey);
        return AESCMAC_STATUS_ERROR;
    }
    object->operationInProgress = true;
    HwiP_restore(interruptKey);
    return AESCMAC_STATUS_SUCCESS;
}


/*
 *  ======== AESCMAC_setupSegmentedOperation ========
 */
static int_fast16_t AESCMAC_setupSegmentedOperation(AESCMACCC26XX_Object *object,
                                                    const CryptoKey *key)
{
    DebugP_assert(key);

    /* Key material pointer and length are not asserted until adding or
     * finalizing data.
     */

    /* Check that there is no operation already in progress for this driver
     * instance.
     */
    int_fast16_t status = AESCMAC_setOperationInProgress(object);

    if (status == AESCMAC_STATUS_SUCCESS)
    {
        /* We will only change the returnStatus if there is an error or cancellation */
        object->returnStatus = AESCMAC_STATUS_SUCCESS;
        /* Make internal copy of crypto key */
        object->key = *key;

        /* Zero the intermediate tag because it will be used as the IV */
        memset(object->intermediateTag, 0, sizeof(object->intermediateTag));
    }

    return status;
}

/*
 *  ======== AESCMAC_setupSign ========
 */
int_fast16_t AESCMAC_setupSign(AESCMAC_Handle handle, const CryptoKey *key)
{
    DebugP_assert(handle);
    AESCMACCC26XX_Object *object = handle->object;
    DebugP_assert(object);

    int_fast16_t status = AESCMAC_setupSegmentedOperation(object, key);

    if (status == AESCMAC_STATUS_SUCCESS)
    {
        object->operationType = AESCMAC_OP_TYPE_SEGMENTED_SIGN;
    }

    return status;
}

/*
 *  ======== AESCMAC_setupVerify ========
 */
int_fast16_t AESCMAC_setupVerify(AESCMAC_Handle handle, const CryptoKey *key)
{
    DebugP_assert(handle);
    AESCMACCC26XX_Object *object = handle->object;
    DebugP_assert(object);

    int_fast16_t status = AESCMAC_setupSegmentedOperation(object, key);

    if (status == AESCMAC_STATUS_SUCCESS)
    {
        object->operationType = AESCMAC_OP_TYPE_SEGMENTED_VERIFY;
    }

    return status;
}

/*
 *  ======== AESCMAC_addData ========
 */
int_fast16_t AESCMAC_addData(AESCMAC_Handle handle,
                             AESCMAC_Operation *operation)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESCMACCC26XX_Object *object = handle->object;
    int_fast16_t status;

    /* Check for previous failure or cancellation of segmented operation */
    if (object->returnStatus != AESCMAC_STATUS_SUCCESS)
    {
        /* Return the status of the previous call.
         * The callback function will not be executed.
         */
        return object->returnStatus;
    }

    /* Verify the input length is non-zero and a multiple of the block size */
    if ((operation->inputLength == 0U) ||
        (operation->inputLength & AES_NON_BLOCK_MULTIPLE_MASK))
    {
        return AESCMAC_STATUS_ERROR;
    }

    if (object->threadSafe)
    {
        if (!CryptoResourceCC26XX_acquireLock(object->semaphoreTimeout))
        {
            return AESCMAC_STATUS_RESOURCE_UNAVAILABLE;
        }

        object->cryptoResourceLocked = true;
    }

    if ((object->operationType == AESCMAC_OP_TYPE_SEGMENTED_SIGN) ||
        (object->operationType == AESCMAC_OP_TYPE_SEGMENTED_VERIFY))
    {
        object->operation = operation;
        status = AESCMAC_processData(handle);
    }
    else
    {
        status = AESCMAC_STATUS_ERROR;
    }

    if ((status != AESCMAC_STATUS_SUCCESS) &&
        (object->cryptoResourceLocked))
    {
        CryptoResourceCC26XX_releaseLock();
        object->cryptoResourceLocked = false;
    }

    return status;
}

/*
 *  ======== AESCMAC_generateSubKey ========
 *  Generate AES CMAC subkey based on
 *  https://tools.ietf.org/html/rfc4493#section-2.3
 */
static int_fast16_t AESCMAC_generateSubKey(AESCMACCC26XX_Object *object,
                                           AESCMAC_SUBKEY_NUM subKeyNum,
                                           uint32_t *subKey)
{
    uint32_t zeroBlock[AES_BLOCK_SIZE_WORDS] = {0};
    int_fast16_t status = AESCMAC_loadKey(object);

    if (status == AESCMAC_STATUS_SUCCESS)
    {
        /* AESWriteKeyStore() enables the CRYPTO IRQ so we must disable it
         * to poll for completion.
         */
        IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);

        /* Perform AES ECB encryption on a block of 0's */
        AESSetDataLength(AES_BLOCK_SIZE);
        AESSetAuthLength(0U);
        AESSetCtrl(CRYPTO_AESCTL_DIR);

        /* Start the DMA input of a zeroed block and output to the subKey buffer */
        AESStartDMAOperation((uint8_t *)zeroBlock, AES_BLOCK_SIZE, (uint8_t *)subKey, AES_BLOCK_SIZE);

        /* Wait until the operation is complete and check for DMA errors */
        if (AESWaitForIRQFlags(AES_RESULT_RDY | AES_DMA_BUS_ERR) &
            AES_DMA_BUS_ERR)
        {
            status = AESCMAC_STATUS_ERROR;
        }

        IntPendClear(INT_CRYPTO_RESULT_AVAIL_IRQ);
    }

    if (status == AESCMAC_STATUS_SUCCESS)
    {
        /* At this point, subKey buffer only has the ciphertext
         * generated by encrypting a block of 0's.
         * Derive SubKey1. */
        AESCMAC_deriveSubKey((uint8_t *)subKey);

        if (subKeyNum == AESCMAC_SUBKEY2)
        {
            /* At this point, subKey buffer contains SubKey1.
             * Derive SubKey2. */
            AESCMAC_deriveSubKey((uint8_t *)subKey);
        }
    }

    return status;
}

/*
 *  ======== AESCMAC_deriveSubKey ========
 */
static void AESCMAC_deriveSubKey(uint8_t *buffer)
{
    uint_fast8_t i;
    uint8_t xorMask = 0U;

    if ((buffer[0] & AESCMAC_MSB_CHECK) != 0U)
    {
        xorMask = AESCMAC_CONST_RB;
    }

    /* Left shift buffer by 1 position */
    for (i = 0U; i < AES_BLOCK_SIZE; i += 1U)
    {
        if (i != 0U)
        {
            buffer[i - 1U] += (buffer[i] >> 7U);
        }

        buffer[i] = buffer[i] << 1U;
    }

    buffer[AES_BLOCK_SIZE  - 1U] ^= xorMask;
}

/*
 *  ======== AESCMAC_finalize ========
 */
int_fast16_t AESCMAC_finalize(AESCMAC_Handle handle,
                              AESCMAC_Operation *operation)
{
    DebugP_assert(handle);
    DebugP_assert(operation);

    AESCMACCC26XX_Object *object = handle->object;
    int_fast16_t status = AESCMAC_STATUS_SUCCESS;

    /* Check for previous failure or cancellation of segmented operation */
    if (object->returnStatus != AESCMAC_STATUS_SUCCESS)
    {
        /* Return the failure status of previous call.
         * The callback will not be called.
         */
        return object->returnStatus;
    }

    if (operation->inputLength == 0U)
    {
        /* Finalizing an operation without providing data to process is not
         * supported. Return an error.
         */
        status = AESCMAC_STATUS_ERROR;
    }
    else
    {
        /* Try and obtain access to the crypto module */
        if (object->threadSafe)
        {
            if (!CryptoResourceCC26XX_acquireLock(object->semaphoreTimeout))
            {
                return AESCMAC_STATUS_RESOURCE_UNAVAILABLE;
            }

            object->cryptoResourceLocked = true;
        }

        if (object->operationType == AESCMAC_OP_TYPE_SEGMENTED_SIGN)
        {
             object->operationType = AESCMAC_OP_TYPE_FINALIZE_SIGN;
        }
        else if (object->operationType == AESCMAC_OP_TYPE_SEGMENTED_VERIFY)
        {
             object->operationType = AESCMAC_OP_TYPE_FINALIZE_VERIFY;
        }
        else
        {
            status = AESCMAC_STATUS_ERROR;
        }

        if (status == AESCMAC_STATUS_SUCCESS)
        {
            object->operation = operation;
            status = AESCMAC_processData(handle);
        }

        if ((status != AESCMAC_STATUS_SUCCESS) &&
            (object->cryptoResourceLocked))
        {
            CryptoResourceCC26XX_releaseLock();
            object->cryptoResourceLocked = false;
        }
    }

    return status;
}

/*
 *  ======== AESCMAC_cancelOperation ========
 */
int_fast16_t AESCMAC_cancelOperation(AESCMAC_Handle handle)
{
    AESCMACCC26XX_Object *object = handle->object;
    uintptr_t interruptKey;

    interruptKey = HwiP_disable();

    /* Check if the HW operation already completed */
    if (!object->hwBusy)
    {
        object->returnStatus = AESCMAC_STATUS_CANCELED;
        object->operationInProgress = false;

        HwiP_restore(interruptKey);

        /* No need to call the callback function provided by the application
         * since it would have already been called when the HW operation
         * completed.
         */

        return AESCMAC_STATUS_SUCCESS;
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

    object->returnStatus = AESCMAC_STATUS_CANCELED;
    object->hwBusy = false;
    object->operationInProgress = false;

    if (object->returnBehavior == AESCMAC_RETURN_BEHAVIOR_BLOCKING)
    {
        /* Unblock the pending task to signal that the operation is complete */
        SemaphoreP_post(&CryptoResourceCC26XX_operationSemaphore);
    }
    else /* AESCMAC_RETURN_BEHAVIOR_CALLBACK */
    {
        /* Call the callback function provided by the application */
        object->callbackFxn(handle, AESCMAC_STATUS_CANCELED,
                            object->operation, object->operationType);
    }

    /* Cleanup posts the crypto access semaphore and must be done after the
     * operational semaphore is posted to avoid a potential race condition
     * when starting a new operation using a different driver instance.
     */
    AESCMAC_cleanup(object);

    /* Always return success */
    return AESCMAC_STATUS_SUCCESS;
}

bool AESCMAC_acquireLock(AESCMAC_Handle handle, uint32_t timeout)
{
    return CryptoResourceCC26XX_acquireLock(timeout);
}

void AESCMAC_releaseLock(AESCMAC_Handle handle)
{
    CryptoResourceCC26XX_releaseLock();
}

void AESCMAC_enableThreadSafety(AESCMAC_Handle handle)
{
    AESCMACCC26XX_Object *object = handle->object;

    object->threadSafe = true;
}

void AESCMAC_disableThreadSafety(AESCMAC_Handle handle)
{
    AESCMACCC26XX_Object *object = handle->object;

    object->threadSafe = false;
}
