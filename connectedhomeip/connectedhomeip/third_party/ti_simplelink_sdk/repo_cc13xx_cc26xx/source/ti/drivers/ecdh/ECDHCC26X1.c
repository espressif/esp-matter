/*
 * Copyright (c) 2020-2021, Texas Instruments Incorporated
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

#include <ti/drivers/ECDH.h>
#include <ti/drivers/ecdh/ECDHCC26X1.h>
#include <ti/drivers/cryptoutils/utils/CryptoUtils.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/rom_ecc.h)


/* Defines */

/* Octet string format requires an extra byte at the start of the public key */
#define OCTET_STRING_OFFSET 1

#define ECC_PARAM_LENGTH (ECC_NISTP256_PARAM_LENGTH_BYTES + ECC_LENGTH_OFFSET_BYTES)

/* Typedefs */
typedef union {
    uint32_t word[ECC_PARAM_LENGTH / sizeof(uint32_t)];
    uint8_t byte[ECC_PARAM_LENGTH];
} ECC_Param;

/*
 *  ======== writeEccPoint ========
 */
void writeEccPoint(const ECC_Param *x, const ECC_Param *y, CryptoKey *key) {
    /* Reverse and copy the X and Y coordinates back to the CryptoKey buffer.
     * Octet string format requires big-endian formatting of X and Y.*/
    CryptoUtils_reverseCopy(&x->word[1],
                            key->u.plaintext.keyMaterial
                             + OCTET_STRING_OFFSET,
                            ECC_NISTP256_PARAM_LENGTH_BYTES);

    CryptoUtils_reverseCopy(&y->word[1],
                            key->u.plaintext.keyMaterial
                             + ECC_NISTP256_PARAM_LENGTH_BYTES
                             + OCTET_STRING_OFFSET,
                            ECC_NISTP256_PARAM_LENGTH_BYTES);

    /* Write the octet string format constant to the CryptoKey buffer */
    key->u.plaintext.keyMaterial[0] = 0x04;

    /* Mark the public key CryptoKey as non-empty */
    key->encoding = CryptoKey_PLAINTEXT;
}

/*
 *  ======== ECDH_init ========
 */
void ECDH_init(void) {
}

/*
 *  ======== ECDH_Params_init ========
 */
void ECDH_Params_init(ECDH_Params *params){
    *params = ECDH_defaultParams;
}

/*
 *  ======== ECDH_construct ========
 */
ECDH_Handle ECDH_construct(ECDH_Config *config, const ECDH_Params *params) {
    ECDH_Handle                  handle;
    ECDHCC26X1_Object           *object;
    uint_fast8_t                key;

    handle = (ECDH_Handle)config;
    object = handle->object;

    key = HwiP_disable();

    if (object->isOpen) {
        HwiP_restore(key);
        return NULL;
    }

    object->isOpen = true;

    HwiP_restore(key);

    // If params are NULL, use defaults
    if (params == NULL) {
        params = (ECDH_Params *)&ECDH_defaultParams;
    }

    object->returnBehavior = params->returnBehavior;
    object->callbackFxn = params->callbackFxn;

    /* Initialise object with NIST-P256 curve */
    ECC_initialize(&(object->eccState), object->eccWorkZone);

    return handle;
}

/*
 *  ======== ECDH_close ========
 */
void ECDH_close(ECDH_Handle handle) {
    ECDHCC26X1_Object         *object;

    /* Get the pointer to the object */
    object = handle->object;

    /* Mark the module as available */
    object->isOpen = false;
}


/*
 *  ======== ECDH_generatePublicKey ========
 */
int_fast16_t ECDH_generatePublicKey(ECDH_Handle handle,
                                    ECDH_OperationGeneratePublicKey *operation) {
    ECDHCC26X1_Object *object = handle->object;
    int_fast16_t returnStatus = ECDH_STATUS_ERROR;
    uint8_t eccStatus;

    /* We need to allocate local copies of the private and public keys because
     * the ECC in ROM implementation requires the word 0x08 to be prepended to
     * every array input.
     * The length word is prepended during initialisation here.
     */
    ECC_Param privateKeyUnion;
    ECC_Param publicKeyUnionX;
    ECC_Param publicKeyUnionY;

    /* We need to set the first word to 0x08 in code. Otherwise, the compiler
     * will allocate an individual copy of the entire array in the const section
     * and copy that in. That is a significant amount of data.
     */
    privateKeyUnion.word[0] = 0x08;
    publicKeyUnionX.word[0] = 0x08;
    publicKeyUnionY.word[0] = 0x08;

    /* For now, only the NIST-P256 curve is supported */
    if (operation->curve->curveType != ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_AN3) {
        return ECDH_STATUS_ERROR;
    }

    /* Validate key sizes to make sure octet string format is used */
    if (operation->myPrivateKey->u.plaintext.keyLength != operation->curve->length ||
        operation->myPublicKey->u.plaintext.keyLength != 2 * operation->curve->length + OCTET_STRING_OFFSET) {
        return ECDH_STATUS_INVALID_KEY_SIZE;
    }

    /* Since we are receiving the private and public keys in octet string format,
     * we need to convert them to little-endian form for use with the ECC in
     * ROM functions
     */
    CryptoUtils_reverseCopyPad(operation->myPrivateKey->u.plaintext.keyMaterial,
                               &privateKeyUnion.word[1],
                               ECC_NISTP256_PARAM_LENGTH_BYTES);

    /* Check if private key in [1, n-1] */
    eccStatus = ECC_validatePrivateKey(&(object->eccState),
                                       privateKeyUnion.word);

    if (eccStatus == STATUS_PRIVATE_VALID) {
         eccStatus = ECC_generateKey(&(object->eccState),
                                    privateKeyUnion.word,
                                    privateKeyUnion.word,
                                    publicKeyUnionX.word,
                                    publicKeyUnionY.word);

        /* Check the ECC in ROM return code and set the driver status accordingly */
        if (eccStatus == STATUS_ECDH_KEYGEN_OK) {
            returnStatus = ECDH_STATUS_SUCCESS;
        }

        writeEccPoint(&publicKeyUnionX, &publicKeyUnionY, operation->myPublicKey);
    }
    else {
        returnStatus = ECDH_STATUS_ERROR;
    }

    /* If the application uses callback return behaviour, emulate it */
    if (object->returnBehavior == ECDH_RETURN_BEHAVIOR_CALLBACK) {
        ECDH_Operation operationUnion = {.generatePublicKey = operation};

        object->callbackFxn(handle,
                            returnStatus,
                            operationUnion,
                            ECDH_OPERATION_TYPE_GENERATE_PUBLIC_KEY);

        return ECDH_STATUS_SUCCESS;
    }
    else {
        return returnStatus;
    }
}

/*
 *  ======== ECDH_computeSharedSecret ========
 */
int_fast16_t ECDH_computeSharedSecret(ECDH_Handle handle, ECDH_OperationComputeSharedSecret *operation) {
    ECDHCC26X1_Object *object = handle->object;
    int_fast16_t returnStatus = ECDH_STATUS_ERROR;
    uint8_t eccStatus;

    /* We need to allocate local copies of the private key, public key, and
     * shared secret because the ECC in ROM implementation requires the word
     * 0x08 to be prepended to every array input.
     * The length word is prepended during initialisation here.
     */
    ECC_Param privateKeyUnion;
    ECC_Param publicKeyUnionX;
    ECC_Param publicKeyUnionY;
    ECC_Param sharedSecretUnionX;
    ECC_Param sharedSecretUnionY;

    /* For now, only the NIST-P256 curve is supported */
    if (operation->curve->curveType != ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_AN3) {
        return ECDH_STATUS_ERROR;
    }

    /* Validate key sizes to make sure octet string format is used */
    if (operation->myPrivateKey->u.plaintext.keyLength != operation->curve->length ||
        operation->theirPublicKey->u.plaintext.keyLength != 2 * operation->curve->length + OCTET_STRING_OFFSET ||
        operation->theirPublicKey->u.plaintext.keyMaterial[0] != 0x04 ||
        operation->sharedSecret->u.plaintext.keyLength != 2 * operation->curve->length + OCTET_STRING_OFFSET) {
        return ECDH_STATUS_INVALID_KEY_SIZE;
    }

    /* We need to set the first word to 0x08 in code. Otherwise, the compiler
     * will allocate an individual copy of the entire array in the const section
     * and copy that in. That is a significant amount of data.
     */
    privateKeyUnion.word[0] = 0x08;
    publicKeyUnionX.word[0] = 0x08;
    publicKeyUnionY.word[0] = 0x08;
    sharedSecretUnionX.word[0] = 0x08;
    sharedSecretUnionY.word[0] = 0x08;

    /* Since we are receiving the private and public keys in octet string format,
     * we need to convert them to little-endian form for use with the ECC in
     * ROM functions
     */
    CryptoUtils_reverseCopyPad(operation->myPrivateKey->u.plaintext.keyMaterial,
                               &privateKeyUnion.word[1],
                               ECC_NISTP256_PARAM_LENGTH_BYTES);

    CryptoUtils_reverseCopyPad(operation->theirPublicKey->u.plaintext.keyMaterial
                                + OCTET_STRING_OFFSET,
                               &publicKeyUnionX.word[1],
                               ECC_NISTP256_PARAM_LENGTH_BYTES);

    CryptoUtils_reverseCopyPad(operation->theirPublicKey->u.plaintext.keyMaterial
                                + ECC_NISTP256_PARAM_LENGTH_BYTES
                                + OCTET_STRING_OFFSET,
                               &publicKeyUnionY.word[1],
                               ECC_NISTP256_PARAM_LENGTH_BYTES);

    eccStatus = ECC_validatePublicKey(&(object->eccState),
                                      publicKeyUnionX.word,
                                      publicKeyUnionY.word);

    if (eccStatus == STATUS_ECC_POINT_ON_CURVE) {
        eccStatus = ECC_ECDH_computeSharedSecret(&(object->eccState),
                                                 privateKeyUnion.word,
                                                 publicKeyUnionX.word,
                                                 publicKeyUnionY.word,
                                                 sharedSecretUnionX.word,
                                                 sharedSecretUnionY.word);

        /* Check the ECC in ROM return code and set the driver status accordingly */
        if (eccStatus == STATUS_ECDH_COMMON_KEY_OK) {
            returnStatus = ECDH_STATUS_SUCCESS;
        }

        writeEccPoint(&sharedSecretUnionX,
                      &sharedSecretUnionY,
                      operation->sharedSecret);
    }
    else {
        returnStatus = ECDH_STATUS_ERROR;
    }

    /* If the application uses callback return behaviour, emulate it */
    if (object->returnBehavior == ECDH_RETURN_BEHAVIOR_CALLBACK) {
        ECDH_Operation operationUnion = {.computeSharedSecret = operation};

        object->callbackFxn(handle,
                            returnStatus,
                            operationUnion,
                            ECDH_OPERATION_TYPE_COMPUTE_SHARED_SECRET);

        return ECDH_STATUS_SUCCESS;
    }
    else {
        return returnStatus;
    }
}
