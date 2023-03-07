/*
 * Copyright (c) 2019-2021, Texas Instruments Incorporated
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

#include <ti/drivers/AESCTRDRBG.h>
#include <ti/drivers/aesctrdrbg/AESCTRDRBGXX.h>
#include <ti/drivers/AESCTR.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#include <ti/drivers/cryptoutils/utils/CryptoUtils.h>

#if ((DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X0_CC26X0) || \
     (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X1_CC26X1) || \
     (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2) || \
     (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4))
    #include <ti/drivers/aesctr/AESCTRCC26XX.h>
#endif

/* Forward declarations */
static void AESCTRDRBGXX_addBigendCounter(uint8_t *counter, uint32_t increment);
static int_fast16_t AESCTRDRBGXX_updateState(AESCTRDRBG_Handle handle,
                                             const void *additionalData,
                                             size_t additionalDataLength);
static void AESCTRDRBG_uninstantiate(AESCTRDRBG_Handle handle);

/* Static globals */
static bool isInitialized = false;

#define CEIL(x, y)      (1 + (((x) - 1) / (y)))

/*
 *  ======== AESCTRDRBG_init ========
 */
void AESCTRDRBG_init(void) {
    AESCTR_init();

    isInitialized = true;
}

/*
 *  ======== AESCTRDRBGXX_updateState ========
 */
static int_fast16_t AESCTRDRBGXX_updateState(AESCTRDRBG_Handle handle,
                                             const void *additionalData,
                                             size_t additionalDataLength) {
    AESCTRDRBGXX_Object         *object;
    AESCTR_Operation            operation;
    uint8_t tmp[AESCTRDRBG_MAX_SEED_LENGTH] = {0};

    object = handle->object;

    /* We need to increment the counter here since regular AESCTR
     * only increments the counter after encrypting it while
     * AESCTRDRBG increments the counter before encrypting it.
     * We do not need to worry about the counter being 1 over afterwards
     * as we will replace the global counter with part of the
     * encrypted result.
     */
    AESCTRDRBGXX_addBigendCounter(object->counter, 1);

    /* Wrap the memcpy below in a zero-length check. Do not remove it!
     * The explicit check is necessary for klocwork to stop emitting a critical
     * warning. Theoretically, memcpy with a length argument of 0 should do
     * nothing. However klocwork emits a warning that there is an out
     * of bounds array access (underflow) on tmp if this check is not in place.
     */
    if (additionalDataLength > 0) {
        /* Copy over any additional data and operate on tmp in place.
         * This way we can have the case where additionalDataLength < seedLength.
         * This is useful in AESCTRDRBG_getBytes() to avoid allocating a spare
         * empty buffer.
         */
        memcpy(tmp,
               additionalData,
               additionalDataLength);
    }

    operation.key               = &object->key;
    operation.input             = tmp;
    operation.output            = tmp;
    operation.initialCounter    = object->counter;
    operation.inputLength       = object->key.u.plaintext.keyLength + AESCTRDRBG_AES_BLOCK_SIZE_BYTES;

    if (AESCTR_oneStepEncrypt(object->ctrHandle, &operation) != AESCTR_STATUS_SUCCESS) {
        return AESCTRDRBG_STATUS_ERROR;
    }

    /* Copy the left most keyLength bytes of the computed result */
    memcpy(object->keyingMaterial,
           tmp,
           object->key.u.plaintext.keyLength);

    /* Copy new counter value as the right most 16 bytes of the
     * computed result.
     */
    memcpy(object->counter,
           tmp + object->key.u.plaintext.keyLength,
           AESCTRDRBG_AES_BLOCK_SIZE_BYTES);

    /* Wipe the stack buffer */
    memset(tmp, 0, object->seedLength);

    return AESCTRDRBG_STATUS_SUCCESS;
}

/*
 *  ======== AESCTRDRBGXX_addBigendCounter ========
 */
static void AESCTRDRBGXX_addBigendCounter(uint8_t *counter, uint32_t increment) {
    uint64_t *counter64 = (uint64_t *)counter;
    uint64_t prior;

    /* Turn it into a little-endian counter */
    CryptoUtils_reverseBufferBytewise(counter64,
                                      AESCTRDRBG_AES_BLOCK_SIZE_BYTES);

    prior = counter64[0];

    /* Increment as a 64-bit number */
    counter64[0] += increment;

    /* Check if we wrapped and need to increment the upper 64 bits */
    if (counter64[0] < prior) {
        counter64[1]++;
    }

    /* Turn it back into a big-endian integer */
    CryptoUtils_reverseBufferBytewise(counter64,
                                      AESCTRDRBG_AES_BLOCK_SIZE_BYTES);
}

/*
 *  ======== AESCTRDRBG_uninstantiate ========
 *
 *  Per the NIST Recommendation SP 800-90A Rev. 1 for DRBG, uninstantiate is the operation
 *  of clearing the internal state {keyingMaterial, counter, reseedCounter} by writing
 *  all 0's. Once uninstantiated, the DRBG instance shall not be used until it's
 *  instantiated again with a fresh seed.
 *
 *  This implementation also sets the isInstantiated flag to false. This function
 *  should be called if any of the AESCTR operations fail so that the DRBG instance
 *  will never be usable when its internal state is potentially corrupt.
 */
static void AESCTRDRBG_uninstantiate(AESCTRDRBG_Handle handle) {
    AESCTRDRBGXX_Object         *object;

    object = handle->object;

    object->isInstantiated = false;
    memset(object->keyingMaterial, 0, object->key.u.plaintext.keyLength);
    memset(object->counter, 0, AESCTRDRBG_AES_BLOCK_SIZE_BYTES);
    object->reseedCounter = 0;
}

/*
 *  ======== AESCTRDRBG_construct ========
 */
AESCTRDRBG_Handle AESCTRDRBG_construct(AESCTRDRBG_Config *config, const AESCTRDRBG_Params *params) {
    AESCTRDRBG_Handle               handle;
    AESCTRDRBGXX_Object             *object;
    const AESCTRDRBGXX_HWAttrs      *hwAttrs;
    AESCTR_Params                   ctrParams;
    uintptr_t                       key;
    int_fast16_t                    status;

    handle = (AESCTRDRBG_Handle)config;
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    key = HwiP_disable();

    if (!isInitialized || object->isOpen) {
        HwiP_restore(key);
        return NULL;
    }

    object->isOpen = true;

    HwiP_restore(key);

    /* There are no valid default params for this driver */
    if (params == NULL) {
        return NULL;
    }

    /* personalizationDataLength must be within
     * [0, AESCTRDRBG_AES_BLOCK_SIZE_BYTES] bytes.
     */
    if (params->personalizationDataLength >
        params->keyLength + AESCTRDRBG_AES_BLOCK_SIZE_BYTES) {
        return NULL;
    }

    /* Open the driver's AESCTR instance */
    AESCTR_Params_init(&ctrParams);
    ctrParams.returnBehavior = (AESCTR_ReturnBehavior)(params->returnBehavior);

    object->ctrHandle = AESCTR_open(hwAttrs->aesctrIndex, &ctrParams);

    if (object->ctrHandle == NULL) {
        object->isOpen = false;

        return NULL;
    }

    /* Initialise CryptoKey for later use */
    CryptoKeyPlaintext_initKey(&object->key, object->keyingMaterial, params->keyLength);

    /* Zero-out counter and keyingMaterial */
    memset(object->counter, 0, AESCTRDRBG_AES_BLOCK_SIZE_BYTES);
    memset(object->keyingMaterial, 0, params->keyLength);

    /* Store constants for later */
    object->seedLength = params->keyLength + AESCTRDRBG_AES_BLOCK_SIZE_BYTES;
    object->reseedInterval = params->reseedInterval;

    /* Ideally this should be set only after instantiation is complete. However
     * since this implementation uses the reseed function, this flag is set here
     * to ensure it doesn't fail with AESCTRDRBG_STATUS_UNINSTANTIATED.
     * Note that if reesed fails due to other reasons, the following call to
     * uninstantiate will clear this flag.
     */
    object->isInstantiated = true;

    /* Reseed the instance to generate the initial (counter, keyingMaterial) pair */
    status = AESCTRDRBG_reseed(handle,
                               params->seed,
                               params->personalizationData,
                               params->personalizationDataLength);

    if (status != AESCTRDRBG_STATUS_SUCCESS) {
        AESCTR_close(object->ctrHandle);
        AESCTRDRBG_uninstantiate(handle);
        object->isOpen = false;

        return NULL;
    }

    return handle;
}

/*
 *  ======== AESCTRDRBG_close ========
 */
void AESCTRDRBG_close(AESCTRDRBG_Handle handle) {
    AESCTRDRBGXX_Object         *object;

    DebugP_assert(handle);

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;

    AESCTR_close(object->ctrHandle);

    AESCTRDRBG_uninstantiate(handle);

    /* Mark the module as available */
    object->isOpen = false;
}

/*
 *  ======== AESCTRDRBG_getBytes ========
 */
int_fast16_t AESCTRDRBG_getBytes(AESCTRDRBG_Handle handle,
                                 CryptoKey *randomBytes) {
    return AESCTRDRBG_generateKey(handle, randomBytes);
}

/*
 *  ======== AESCTRDRBG_generateKey ========
 */
int_fast16_t AESCTRDRBG_generateKey(AESCTRDRBG_Handle handle,
                                    CryptoKey *randomKey) {
    int_fast16_t status = AESCTRDRBG_STATUS_ERROR;

    status = AESCTRDRBG_getRandomBytes(handle,
                                       randomKey->u.plaintext.keyMaterial,
                                       randomKey->u.plaintext.keyLength);

    if (status == AESCTRDRBG_STATUS_SUCCESS) {
        randomKey->encoding = CryptoKey_PLAINTEXT;
    }

    return status;
}

/*
 *  ======== AESCTRDRBG_getRandomBytes ========
 */
int_fast16_t AESCTRDRBG_getRandomBytes(AESCTRDRBG_Handle handle,
                                       void *randomBytes,
                                       size_t randomBytesSize) {
    AESCTRDRBGXX_Object         *object;
    AESCTR_Operation            operation;
    int_fast16_t                status;
    bool                        lockAcquired;
    uint32_t                    lockAcquireTimeout;

    object = handle->object;

    if (object->isInstantiated == false) {
        return AESCTRDRBG_STATUS_UNINSTANTIATED;
    }

    if (object->reseedCounter >= object->reseedInterval) {
        return AESCTRDRBG_STATUS_RESEED_REQUIRED;
    }

    if (SwiP_inISR() || HwiP_inISR()) {
        lockAcquireTimeout = SemaphoreP_NO_WAIT;
    }
    else {
        lockAcquireTimeout = SemaphoreP_WAIT_FOREVER;
    }

    lockAcquired = AESCTR_acquireLock(object->ctrHandle, lockAcquireTimeout);
    if (!lockAcquired) {
        return AESCTRDRBG_STATUS_RESOURCE_UNAVAILABLE;
    }

    AESCTR_disableThreadSafety(object->ctrHandle);

    /* Set the keying material of the CryptoKey to 0.
     * If we use AESCTR to encrypt a buffer full of zeros,
     * the resultant output will be the bitstream of the
     * encrypted counters. That is what is used as
     * random bits by AESCTRDRBG.
     * Zeroing out the keying material and performing
     * the AESCTR encryption in place saves us from
     * allocating a buffer of the right length full
     * of zeros or repeatedly encrypting a 16-byte
     * buffer full of zeros.
     */
    memset(randomBytes, 0, randomBytesSize);

    /* We need to increment the counter here since regular AESCTR
     * only increments the counter after encrypting it while
     * AESCTRDRBG increments the counter before encrypting it.
     */
    AESCTRDRBGXX_addBigendCounter(object->counter, 1);

    operation.key               = &object->key;
    operation.input             = randomBytes;
    operation.output            = randomBytes;
    operation.initialCounter    = object->counter;
    operation.inputLength       = randomBytesSize;

    status = AESCTR_oneStepEncrypt(object->ctrHandle, &operation);

    if (status != AESCTR_STATUS_SUCCESS) {
        AESCTR_releaseLock(object->ctrHandle);
        AESCTRDRBG_uninstantiate(handle);
        return AESCTRDRBG_STATUS_UNINSTANTIATED;
    }

    /* Add the number of counter blocks we produced to the
     * internal counter. We already incremented by one above
     * so we increment by one less here.
     */
    AESCTRDRBGXX_addBigendCounter(object->counter,
                                  CEIL(randomBytesSize, AESCTRDRBG_AES_BLOCK_SIZE_BYTES) - 1);

    status = AESCTRDRBGXX_updateState(handle, NULL, 0);


    AESCTR_enableThreadSafety(object->ctrHandle);
    AESCTR_releaseLock(object->ctrHandle);

    if (status != AESCTRDRBG_STATUS_SUCCESS) {
        AESCTRDRBG_uninstantiate(handle);
        return AESCTRDRBG_STATUS_UNINSTANTIATED;
    }

    object->reseedCounter += 1;

    return AESCTRDRBG_STATUS_SUCCESS;
}

/*
 *  ======== AESCTRDRBG_reseed ========
 */
int_fast16_t AESCTRDRBG_reseed(AESCTRDRBG_Handle handle,
                               const void *seed,
                               const void *additionalData,
                               size_t additionalDataLength) {
    AESCTRDRBGXX_Object *object;
    int_fast16_t        status;
    uint8_t             tmp[AESCTRDRBG_MAX_SEED_LENGTH];
    uint32_t            i;
    bool                lockAcquired;
    uint32_t            lockAcquireTimeout;

    object = handle->object;

    if (object->isInstantiated == false) {
        return AESCTRDRBG_STATUS_UNINSTANTIATED;
    }

    if (additionalDataLength > object->seedLength) {
        return AESCTRDRBG_STATUS_ERROR;
    }

    if (SwiP_inISR() || HwiP_inISR()) {
        lockAcquireTimeout = SemaphoreP_NO_WAIT;
    }
    else {
        lockAcquireTimeout = SemaphoreP_WAIT_FOREVER;
    }

    lockAcquired = AESCTR_acquireLock(object->ctrHandle, lockAcquireTimeout);
    if (!lockAcquired) {
        return AESCTRDRBG_STATUS_RESOURCE_UNAVAILABLE;
    }

    AESCTR_disableThreadSafety(object->ctrHandle);

    /* Set temporary buffer as additionalData padded with zeros */
    memset(tmp, 0, object->seedLength);
    memcpy(tmp, additionalData, additionalDataLength);

    /* XOR-in the seed. It should always be a multiple of 32 bits */
    for (i = 0; i < object->seedLength / sizeof(uint32_t); i++){
        ((uint32_t *)tmp)[i] ^= ((uint32_t *)seed)[i];
    }

    /* Use the combined seed to generate a new (counter, keyingMaterial) pair */
    status = AESCTRDRBGXX_updateState(handle, tmp, object->seedLength);

    AESCTR_enableThreadSafety(object->ctrHandle);
    AESCTR_releaseLock(object->ctrHandle);

    if (status != AESCTRDRBG_STATUS_SUCCESS) {
        AESCTRDRBG_uninstantiate(handle);
        return AESCTRDRBG_STATUS_UNINSTANTIATED;
    }

    object->reseedCounter = 1;

    return AESCTRDRBG_STATUS_SUCCESS;
}
