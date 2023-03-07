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

#include <ti/drivers/ECDSA.h>
#include <ti/drivers/ecdsa/ECDSACC26X1.h>
#include <ti/drivers/TRNG.h>
#include <ti/drivers/trng/TRNGCC26XX.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#include <ti/drivers/cryptoutils/utils/CryptoUtils.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/rom_ecc.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)

/* Defines */

/* Octet string format requires an extra byte at the start of the public key */
#define OCTET_STRING_OFFSET 1

/* Extern globals */
extern const ECDSA_Params ECDSA_defaultParams;

/*
 *  ======== ECDSA_init ========
 */
void ECDSA_init(void) {
}


/*
 *  ======== ECDSA_close ========
 */
void ECDSA_close(ECDSA_Handle handle) {
    ECDSACC26X1_Object *object = handle->object;

    /* Mark the module as available */
    object->isOpen = false;
}


/*
 *  ======== ECDSA_construct ========
 */
ECDSA_Handle ECDSA_construct(ECDSA_Config *config, const ECDSA_Params *params) {
    ECDSA_Handle handle         = (ECDSA_Handle)config;
    ECDSACC26X1_Object *object  = handle->object;
    uint_fast8_t key;

    /* If params are NULL, use defaults */
    if (params == NULL) {
        params = (ECDSA_Params *)&ECDSA_defaultParams;
    }

    key = HwiP_disable();

    if (object->isOpen) {
        HwiP_restore(key);
        return NULL;
    }

    object->isOpen = true;

    HwiP_restore(key);

    /* Initialise object with NIST-P256 curve */
    ECC_initialize(&(object->eccState), object->eccWorkZone);

    object->returnBehavior = params->returnBehavior;
    object->callbackFxn = params->callbackFxn;

    return handle;
}


/*
 *  ======== ECDSA_sign ========
 */
int_fast16_t ECDSA_sign(ECDSA_Handle handle, ECDSA_OperationSign *operation) {
    ECDSACC26X1_Object *object          = handle->object;
    ECDSACC26X1_HWAttrs const *hwAttrs  = handle->hwAttrs;
    int_fast16_t returnStatus           = ECDSA_STATUS_ERROR;
    uint8_t eccStatus                   = STATUS_PRIVATE_KEY_LARGER_EQUAL_ORDER;
    TRNGCC26XX_Object trngObject        = {0};
    TRNGCC26XX_HWAttrs trngHwAttrs;
    TRNG_Config trngConfig;
    TRNG_Handle trngHandle;
    TRNG_Params trngParams;
    int_fast16_t trngStatus;
    CryptoKey pmsnKey;

    /* We need to allocate local copies of the private and public keys because
     * the ECC in ROM implementation requires the word 0x08 to be prepended to
     * every array input.
     * The length word is prepended during initialisation here.
     */
    ECC_NISTP256_Param privateKeyUnion;
    ECC_NISTP256_Param pmsnUnion;
    ECC_NISTP256_Param hashUnion;
    ECC_NISTP256_Param rUnion;
    ECC_NISTP256_Param sUnion;

    /* We need to set the first word to 0x08 in code. Otherwise, the compiler
     * will allocate an individual copy of the entire array in the const section
     * and copy that in. That is a significant amount of data.
     */
    privateKeyUnion.word[0] = 0x08;
    pmsnUnion.word[0] = 0x08;
    hashUnion.word[0] = 0x08;
    rUnion.word[0] = 0x08;
    sUnion.word[0] = 0x08;

    /* We want to store the PMSN locally in SRAM */
    CryptoKeyPlaintext_initBlankKey(&pmsnKey,
                                    &pmsnUnion.byte[ECC_LENGTH_OFFSET_BYTES],
                                    ECC_NISTP256_PARAM_LENGTH_BYTES);

    /* We are calling TRNG_init() here to limit references to TRNG_xyz to sign
     * operations.
     * That means that the linker can remove all TRNG related code if only
     * ECDSA_verify functionality is used.
     */
    TRNG_init();

    trngHwAttrs.intPriority = hwAttrs->trngIntPriority;
    trngConfig.object       = &trngObject;
    trngConfig.hwAttrs      = &trngHwAttrs;

    TRNG_Params_init(&trngParams);

    if (object->returnBehavior == ECDSA_RETURN_BEHAVIOR_BLOCKING) {
        trngParams.returnBehavior = TRNG_RETURN_BEHAVIOR_BLOCKING;
    }
    else {
        trngParams.returnBehavior = TRNG_RETURN_BEHAVIOR_POLLING;
    }

    trngHandle = TRNG_construct(&trngConfig, &trngParams);

    if (trngHandle == NULL) {
        return ECDSA_STATUS_ERROR;
    }

    /* Generate the PMSN. If it is not in range [1, n-1], generate another
     * one.
     */
    do {
        trngStatus = TRNG_generateEntropy(trngHandle, &pmsnKey);

        if (trngStatus != TRNG_STATUS_SUCCESS) {
            TRNG_close(trngHandle);
            return ECDSA_STATUS_ERROR;
        }

        /* Check if pmsn in [1, n-1] */
        eccStatus = ECC_validatePrivateKey(&(object->eccState),
                                           pmsnUnion.word);
    } while (eccStatus != STATUS_PRIVATE_VALID);

    TRNG_close(trngHandle);



    /* Since we are receiving the private and public keys in octet string format,
     * we need to convert them to little-endian form for use with the ECC in
     * ROM functions
     */
    CryptoUtils_reverseCopyPad(operation->hash,
                               &hashUnion.word[1],
                               ECC_NISTP256_PARAM_LENGTH_BYTES);

    CryptoUtils_reverseCopyPad(operation->myPrivateKey->u.plaintext.keyMaterial,
                               &privateKeyUnion.word[1],
                               ECC_NISTP256_PARAM_LENGTH_BYTES);

    eccStatus = ECC_ECDSA_sign(&(object->eccState),
                               privateKeyUnion.word,
                               hashUnion.word,
                               pmsnUnion.word,
                               rUnion.word,
                               sUnion.word);

    /* Check the ECC in ROM return code and set the driver status accordingly */
    if (eccStatus == STATUS_ECDSA_SIGN_OK) {
        returnStatus = ECDSA_STATUS_SUCCESS;
    }

    /* Now that we have created r and s, we need to copy them back and reverse
     * them since the ECC in ROM implementation provides little-endian values.
     */
    CryptoUtils_reverseCopy(&rUnion.word[1],
                            operation->r,
                            ECC_NISTP256_PARAM_LENGTH_BYTES);

    CryptoUtils_reverseCopy(&sUnion.word[1],
                            operation->s,
                            ECC_NISTP256_PARAM_LENGTH_BYTES);

    /* If the application uses callback return behaviour, emulate it */
    if (object->returnBehavior == ECDSA_RETURN_BEHAVIOR_CALLBACK) {
        ECDSA_Operation operationUnion = {.sign = operation};

        object->callbackFxn(handle,
                            returnStatus,
                            operationUnion,
                            ECDSA_OPERATION_TYPE_SIGN);

        return ECDSA_STATUS_SUCCESS;
    }
    else {
        return returnStatus;
    }
}

/*
 *  ======== ECDSA_verify ========
 */
int_fast16_t ECDSA_verify(ECDSA_Handle handle, ECDSA_OperationVerify *operation) {
    ECDSACC26X1_Object *object = handle->object;
    int_fast16_t returnStatus = ECDSA_STATUS_ERROR;
    uint8_t eccStatus;

    /* Validate key sizes to make sure octet string format is used */
    if (operation->theirPublicKey->u.plaintext.keyLength != 2 * operation->curve->length + OCTET_STRING_OFFSET ||
        operation->theirPublicKey->u.plaintext.keyMaterial[0] != 0x04) {
        return ECDSA_STATUS_INVALID_KEY_SIZE;
    }

    /* We need to allocate local copies of the private and public keys because
     * the ECC in ROM implementation requires the word 0x08 to be prepended to
     * every array input.
     * The length word is prepended during initialisation here.
     */
    ECC_NISTP256_Param publicKeyUnionX;
    ECC_NISTP256_Param publicKeyUnionY;
    ECC_NISTP256_Param hashUnion;
    ECC_NISTP256_Param rUnion;
    ECC_NISTP256_Param sUnion;

    /* We need to set the first word to 0x08 in code. Otherwise, the compiler
     * will allocate an individual copy of the entire array in the const section
     * and copy that in. That is a significant amount of data.
     */
    publicKeyUnionX.word[0] = 0x08;
    publicKeyUnionY.word[0] = 0x08;
    hashUnion.word[0] = 0x08;
    rUnion.word[0] = 0x08;
    sUnion.word[0] = 0x08;


    /* Since we are receiving the private and public keys in octet string format,
     * we need to convert them to little-endian form for use with the ECC in
     * ROM functions
     */
    CryptoUtils_reverseCopyPad(operation->hash,
                               &hashUnion.word[1],
                               ECC_NISTP256_PARAM_LENGTH_BYTES);

    CryptoUtils_reverseCopyPad(operation->r,
                               &rUnion.word[1],
                               ECC_NISTP256_PARAM_LENGTH_BYTES);

    CryptoUtils_reverseCopyPad(operation->s,
                               &sUnion.word[1],
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

        eccStatus = ECC_ECDSA_verify(&(object->eccState),
                                     publicKeyUnionX.word,
                                     publicKeyUnionY.word,
                                     hashUnion.word,
                                     rUnion.word,
                                     sUnion.word);

        /* Check the ECC in ROM return code and set the driver status accordingly */
        if (eccStatus == STATUS_ECDSA_VALID_SIGNATURE) {
            returnStatus = ECDSA_STATUS_SUCCESS;
        }
    }
    else {
        returnStatus = ECDSA_STATUS_ERROR;
    }

    /* If the application uses callback return behaviour, emulate it */
    if (object->returnBehavior == ECDSA_RETURN_BEHAVIOR_CALLBACK) {
        ECDSA_Operation operationUnion = {.verify = operation};

        object->callbackFxn(handle,
                            returnStatus,
                            operationUnion,
                            ECDSA_OPERATION_TYPE_VERIFY);

        return ECDSA_STATUS_SUCCESS;
    }
    else {
        return returnStatus;
    }
}

