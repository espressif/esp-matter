/*
 * Copyright (c) 2017-2020, Texas Instruments Incorporated
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
#include <stdlib.h>
#include <string.h>

#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

/*
 *  ======== CryptoKey_getCryptoKeyType ========
 */
int_fast16_t CryptoKey_getCryptoKeyType(CryptoKey *keyHandle,
                                        CryptoKey_Encoding *keyType) {
    *keyType = keyHandle->encoding;

    return CryptoKey_STATUS_SUCCESS;
}

/*
 *  ======== CryptoKey_isBlank ========
 */
int_fast16_t CryptoKey_isBlank(CryptoKey *keyHandle, bool *isBlank) {
    if (keyHandle->encoding == CryptoKey_BLANK_PLAINTEXT) {
        *isBlank = true;
    }
    else {
        *isBlank = false;
    }

    return CryptoKey_STATUS_SUCCESS;
}

/*
 *  ======== CryptoKey_markAsBlank ========
 */
int_fast16_t CryptoKey_markAsBlank(CryptoKey *keyHandle) {
    keyHandle->encoding = CryptoKey_BLANK_PLAINTEXT;

    return CryptoKey_STATUS_SUCCESS;
}

/*
 *  ======== CryptoKeyPlaintext_initKey ========
 */
int_fast16_t CryptoKeyPlaintext_initKey(CryptoKey *keyHandle,
                                        uint8_t *key,
                                        size_t keyLength){
    keyHandle->encoding = CryptoKey_PLAINTEXT;
    keyHandle->u.plaintext.keyMaterial = key;
    keyHandle->u.plaintext.keyLength = keyLength;

    return CryptoKey_STATUS_SUCCESS;
}

/*
 *  ======== CryptoKeyPlaintext_initBlankKey ========
 */
int_fast16_t CryptoKeyPlaintext_initBlankKey(CryptoKey *keyHandle,
                                             uint8_t *keyLocation,
                                             size_t keyLength){

    keyHandle->encoding = CryptoKey_BLANK_PLAINTEXT;
    keyHandle->u.plaintext.keyMaterial = keyLocation;
    keyHandle->u.plaintext.keyLength = keyLength;

    return CryptoKey_STATUS_SUCCESS;
}

/*
 *  ======== CryptoKeyPlaintext_setKeyLocation ========
 */
int_fast16_t CryptoKeyPlaintext_setKeyLocation(CryptoKey *keyHandle,
                                               uint8_t *location){
    keyHandle->u.plaintext.keyMaterial = location;

    return CryptoKey_STATUS_SUCCESS;
}

/*
 *  ======== CryptoKeyPlaintext_setKeyLocation ========
 */
int_fast16_t CryptoKeyPlaintext_getKeyLocation(CryptoKey *keyHandle,
                                               uint8_t **location){
    *location = keyHandle->u.plaintext.keyMaterial;

    return CryptoKey_STATUS_SUCCESS;
}

/*
 *  ======== CryptoKeyPlaintext_getKeyLength ========
 */
int_fast16_t CryptoKeyPlaintext_getKeyLength(CryptoKey *keyHandle,
                                             size_t *length){
    *length = keyHandle->u.plaintext.keyLength;

    return CryptoKey_STATUS_SUCCESS;
}

/*
 *  ======== CryptoKeyPlaintext_setKeyLength ========
 */
int_fast16_t CryptoKeyPlaintext_setKeyLength(CryptoKey *keyHandle,
                                             size_t length) {
    keyHandle->u.plaintext.keyLength = length;

    return CryptoKey_STATUS_SUCCESS;
}
