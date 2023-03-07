/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/sha2/SHA2CC26X1.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/rom_sha256.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)

/* Outer and inner padding bytes used in HMAC */
#define HMAC_OPAD_BYTE 0x5C
#define HMAC_IPAD_BYTE 0x36

/* Forward declarations */
static void SHA2CC26X1_xorBufferWithByte(uint8_t *buffer,
                                         size_t bufferLength,
                                         uint8_t byte);

/*
 *  ======== SHA2CC26X1_xorBufferWithByte ========
 */
static void SHA2CC26X1_xorBufferWithByte(uint8_t *buffer,
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
}

/*
 *  ======== SHA2_construct ========
 */
SHA2_Handle SHA2_construct(SHA2_Config *config, const SHA2_Params *params) {
    SHA2_Handle                 handle;
    SHA2CC26X1_Object           *object;
    uint_fast8_t                key;

    handle = (SHA2_Config*)config;
    object = handle->object;

    key = HwiP_disable();

    if (object->isOpen) {
        HwiP_restore(key);
        return NULL;
    }

    object->isOpen = true;

    HwiP_restore(key);

    if (params == NULL) {
        params = &SHA2_defaultParams;
    }

    /* This implementation only supports SHA256 */
    if (params->hashType != SHA2_HASH_TYPE_256) {

        object->isOpen = false;

        return NULL;
    }

    object->returnBehavior  = params->returnBehavior;
    object->callbackFxn     = params->callbackFxn;

    return handle;
}

/*
 *  ======== SHA2_close ========
 */
void SHA2_close(SHA2_Handle handle) {
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object *)handle->object;

    /* If there is still an operation ongoing, abort it now. */
    object->isOpen = false;
}

/*
 *  ======== SHA2CC26X1_addData ========
 */
int_fast16_t SHA2CC26X1_addData(SHA2_Handle handle,
                                const void* data,
                                size_t length) {
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object *)handle->object;
    uint8_t status;

    if (object->workzone.textLen[0] == 0) {
        SHA256_init(&object->workzone);
    }

    status = SHA256_process(&object->workzone, (uint8_t *)data, length);

    return (status == SHA256_SUCCESS) ? SHA2_STATUS_SUCCESS : SHA2_STATUS_ERROR;
}

/*
 *  ======== SHA2_addData ========
 */
int_fast16_t SHA2_addData(SHA2_Handle handle, const void* data, size_t length) {
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object *)handle->object;

    int_fast16_t returnStatus = SHA2CC26X1_addData(handle, data, length);

    /* If the application uses callback return behaviour, emulate it */
    if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_CALLBACK) {
        object->callbackFxn(handle, returnStatus);

        return SHA2_STATUS_SUCCESS;
    }
    else {
        return returnStatus;
    }
}


/*
 *  ======== SHA2CC26X1_finalize ========
 */
int_fast16_t SHA2CC26X1_finalize(SHA2_Handle handle, void *digest) {
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object *)handle->object;
    uint8_t status;

    status = SHA256_final(&object->workzone, digest);

    // Reset object state
    object->workzone.textLen[0] = 0;

    return (status == SHA256_SUCCESS) ? SHA2_STATUS_SUCCESS : SHA2_STATUS_ERROR;
}


/*
 *  ======== SHA2_finalize ========
 */
int_fast16_t SHA2_finalize(SHA2_Handle handle, void *digest) {
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object *)handle->object;

    int_fast16_t returnStatus =SHA2CC26X1_finalize(handle, digest);

    /* If the application uses callback return behaviour, emulate it */
    if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_CALLBACK) {
        object->callbackFxn(handle, returnStatus);

        return SHA2_STATUS_SUCCESS;
    }
    else {
        return returnStatus;
    }
}

/*
 *  ======== SHA2CC26X1_hashData ========
 */
int_fast16_t SHA2CC26X1_hashData(SHA2_Handle handle,
                           const void *data,
                           size_t length,
                           void *digest) {
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object *)handle->object;
    uint8_t status;

    SHA256_init(&object->workzone);

    status = SHA256_full(&object->workzone, digest, (uint8_t *)data, length);

    return (status == SHA256_SUCCESS) ? SHA2_STATUS_SUCCESS : SHA2_STATUS_ERROR;

}

/*
 *  ======== SHA2_hashData ========
 */
int_fast16_t SHA2_hashData(SHA2_Handle handle,
                           const void *data,
                           size_t length,
                           void *digest) {
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object *)handle->object;

    int_fast16_t returnStatus = SHA2CC26X1_hashData(handle,
                                                    data,
                                                    length,
                                                    digest);

    /* If the application uses callback return behaviour, emulate it */
    if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_CALLBACK) {
        object->callbackFxn(handle, returnStatus);

        return SHA2_STATUS_SUCCESS;
    }
    else {
        return returnStatus;
    }
}

/*
 *  ======== SHA2CC26X1_setupHmac ========
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
 *  - It starts a segmented hash of the k0^opad part of
 *    H(k0 ^ opad || H(k0 ^ ipad || message))
 *      - The intermediate output is saved in SHA2CC26X1_Object.hmacDigest
 *  - It starts a segmented hash with the k0^ipad part of
 *    H(k0  ^ipad || message)
 *      - The intermediate output is saved by the SHA2 driver as usual in
 *        SHA2CC26X1_Object.workzone.state
 */
int_fast16_t SHA2CC26X1_setupHmac(SHA2_Handle handle, CryptoKey *key) {
    uint8_t xorBuffer[SHA2_BLOCK_SIZE_BYTES_256];
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object*)handle->object;
    size_t keyLength          = key->u.plaintext.keyLength;
    uint8_t *keyingMaterial   = key->u.plaintext.keyMaterial;

    /* Prepare the buffer of the derived key. We set the entire buffer to 0x00
     * so we do not need to pad it to the block size after copying the keying
     * material provided or the hash thereof there.
     */
    memset(xorBuffer, 0x00, SHA2_BLOCK_SIZE_BYTES_256);

    /* If the keying material fits in the derived key buffer, copy it there.
     * Otherwise, we need to hash it first and copy the digest there. Since
     * We filled the entire buffer with 0x00, we do not need to pad to the block
     * size.
     */
    if (keyLength <= SHA2_BLOCK_SIZE_BYTES_256) {
        memcpy(xorBuffer, keyingMaterial, keyLength);
    }
    else {
        SHA2CC26X1_hashData(handle,
                            keyingMaterial,
                            keyLength,
                            xorBuffer);
    }

    SHA256_init(&object->workzone);

    /* Compute k0 ^ opad */
    SHA2CC26X1_xorBufferWithByte(xorBuffer,
                                 SHA2_BLOCK_SIZE_BYTES_256,
                                 HMAC_OPAD_BYTE);

    /* Start a hash of k0 ^ opad.
     * The intermediate result will be stored in the object for later
     * use when the application calls SHA2_addData on its actual message.
     */
    SHA2CC26X1_addData(handle,
                       xorBuffer,
                       SHA2_BLOCK_SIZE_BYTES_256);

    /* Copy the intermediate state of H(k0 ^ opad) */
    memcpy(object->hmacDigest, object->workzone.state, SHA2_DIGEST_LENGTH_BYTES_256);

    /* Undo k0 ^ opad to reconstruct k0. Use the memory of k0 instead
     * of allocating a new copy on the stack to save RAM.
     */
    SHA2CC26X1_xorBufferWithByte(xorBuffer,
                                 SHA2_BLOCK_SIZE_BYTES_256,
                                 HMAC_OPAD_BYTE);

    /* Compute k0 ^ ipad. */
    SHA2CC26X1_xorBufferWithByte(xorBuffer,
                                 SHA2_BLOCK_SIZE_BYTES_256,
                                 HMAC_IPAD_BYTE);

    /* Reset workzone to prepare to start a new hash */
    SHA256_init(&object->workzone);

    /* Start a hash of k0 ^ ipad.
     * Use top-level fxn to invoke callback if needed
     */
    SHA2CC26X1_addData(handle,
                       xorBuffer,
                       SHA2_BLOCK_SIZE_BYTES_256);

    return SHA2_STATUS_SUCCESS;
}

/*
 *  ======== SHA2_setupHmac ========
 *  The callback invocation is split out into a separate wrapper function to
 *  avoid invoking the callback multiple times during compound operations such
 *  as SHA2_hmac()
 */
int_fast16_t SHA2_setupHmac(SHA2_Handle handle, CryptoKey *key) {
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object*)handle->object;

    int_fast16_t returnStatus = SHA2CC26X1_setupHmac(handle, key);

    if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_CALLBACK) {
        /* Call the callback function provided by the application. */
        object->callbackFxn(handle, SHA2_STATUS_SUCCESS);
    }

    return returnStatus;
}

/*
 *  ======== SHA2_finalizeHmac ========
 *
 *  This function completes the HMAC operation once all application data
 *  has been added through SHA_addData().
 *
 *  - It finalizes  H((k0 ^ ipad) || data)
 *  - It adds H((k0 ^ ipad) || data) to the previously started hash that already
 *    includes k0 ^ opad.
 *  - It finalizes H(k0 ^ opad || H((k0 ^ ipad) || data))
 */
int_fast16_t SHA2_finalizeHmac(SHA2_Handle handle, void *hmac) {
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object*)handle->object;
    uint8_t tmpDigest[SHA2_DIGEST_LENGTH_BYTES_256];

    /* Finalize H((k0 ^ ipad) || data) */
    SHA2CC26X1_finalize(handle, tmpDigest);

    memcpy(object->workzone.state,
           object->hmacDigest,
           SHA2_DIGEST_LENGTH_BYTES_256);

    /* We already processed one block of input earlier */
    object->workzone.textLen[0] = SHA2_BLOCK_SIZE_BYTES_256;

    /* Add the temporary digest computed earlier to the current digest */
    SHA2CC26X1_addData(handle,
                       tmpDigest,
                       SHA2_DIGEST_LENGTH_BYTES_256);

    /* Finalize H(k0 ^ opad || H((k0 ^ ipad) || data)) */
    SHA2_finalize(handle, hmac);

    return SHA2_STATUS_SUCCESS;
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
    SHA2CC26X1_setupHmac(handle, key);

    /* Add the input message to the hash */
    SHA2CC26X1_addData(handle, data, size);

    return SHA2_finalizeHmac(handle, hmac);
}

/*
 *  ======== SHA2_reset ========
 */
void SHA2_reset(SHA2_Handle handle)
{
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object*)handle->object;

    SHA256_init(&object->workzone);
}

/*
 *  ======== SHA2_cancelOperation ========
 */
int_fast16_t SHA2_cancelOperation(SHA2_Handle handle) {
    SHA2CC26X1_Object *object = (SHA2CC26X1_Object*)handle->object;

    if (object->returnBehavior == SHA2_RETURN_BEHAVIOR_CALLBACK) {
        /* Call the callback function provided by the application. */
        object->callbackFxn(handle, SHA2_STATUS_CANCELED);
    }

    return SHA2_STATUS_SUCCESS;
}

int_fast16_t SHA2_setHashType(SHA2_Handle handle, SHA2_HashType type) {
    /* We only support SHA256. Trying to switch to any other hash type returns
     * an error.
     */
    return (type == SHA2_HASH_TYPE_256) ? SHA2_STATUS_SUCCESS : SHA2_STATUS_ERROR;
}
