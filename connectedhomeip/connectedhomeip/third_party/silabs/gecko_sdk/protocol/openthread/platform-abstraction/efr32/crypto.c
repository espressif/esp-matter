/*
 *  Copyright (c) 2020, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for PSA.
 *
 */
#include "security_manager.h"
#include <openthread-core-config.h>
#include <openthread/error.h>
#include <openthread/platform/crypto.h>
#include "utils/code_utils.h"

#if OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_PSA

// Helper function to convert otCryptoKeyType to psa_key_type_t
static psa_key_type_t getPsaKeyType(otCryptoKeyType aKeyType)
{
    psa_key_type_t aPsaKeyType = 0;

    switch (aKeyType)
    {
    case OT_CRYPTO_KEY_TYPE_RAW:
        aPsaKeyType = PSA_KEY_TYPE_RAW_DATA;
        break;

    case OT_CRYPTO_KEY_TYPE_AES:
        aPsaKeyType = PSA_KEY_TYPE_AES;
        break;

    case OT_CRYPTO_KEY_TYPE_HMAC:
        aPsaKeyType = PSA_KEY_TYPE_HMAC;
        break;
    }

    return aPsaKeyType;
}

// Helper function to convert aKeyAlgorithm to psa_algorithm_t
static psa_algorithm_t getPsaAlgorithm(otCryptoKeyAlgorithm aKeyAlgorithm)
{
    psa_algorithm_t aPsaKeyAlgorithm = 0;

    switch (aKeyAlgorithm)
    {
    case OT_CRYPTO_KEY_ALG_VENDOR:
        aPsaKeyAlgorithm = PSA_ALG_VENDOR_FLAG;
        break;

    case OT_CRYPTO_KEY_ALG_AES_ECB:
        aPsaKeyAlgorithm = PSA_ALG_ECB_NO_PADDING;
        break;

    case OT_CRYPTO_KEY_ALG_HMAC_SHA_256:
        aPsaKeyAlgorithm = PSA_ALG_HMAC(PSA_ALG_SHA_256);
        break;
    }

    return aPsaKeyAlgorithm;
}

// Helper function to convert aKeyUsage to psa_key_usage_t
static psa_key_usage_t getPsaKeyUsage(int aKeyUsage)
{
    psa_key_usage_t aPsaKeyUsage = 0;

    if (aKeyUsage & OT_CRYPTO_KEY_USAGE_EXPORT)
    {
        aPsaKeyUsage |= PSA_KEY_USAGE_EXPORT;
    }

    if (aKeyUsage & OT_CRYPTO_KEY_USAGE_ENCRYPT)
    {
        aPsaKeyUsage |= PSA_KEY_USAGE_ENCRYPT;
    }

    if (aKeyUsage & OT_CRYPTO_KEY_USAGE_DECRYPT)
    {
        aPsaKeyUsage |= PSA_KEY_USAGE_DECRYPT;
    }

    if (aKeyUsage & OT_CRYPTO_KEY_USAGE_SIGN_HASH)
    {
        aPsaKeyUsage |= PSA_KEY_USAGE_SIGN_HASH;
    }

    return aPsaKeyUsage;
}

// Helper function to convert otCryptoKeyStorage to psa_key_persistence_t
static psa_key_persistence_t getPsaKeyPersistence(otCryptoKeyStorage aKeyPersistence)
{
    psa_key_persistence_t aPsaKeyPersistence = 0;

    switch (aKeyPersistence)
    {
    case OT_CRYPTO_KEY_STORAGE_VOLATILE:
        aPsaKeyPersistence = PSA_KEY_LIFETIME_VOLATILE;
        break;

    case OT_CRYPTO_KEY_STORAGE_PERSISTENT:
        aPsaKeyPersistence = PSA_KEY_LIFETIME_PERSISTENT;
        break;
    }

    return aPsaKeyPersistence;
}

void otPlatCryptoInit(void)
{
    (void) sl_sec_man_init();
}

otError otPlatCryptoImportKey(otCryptoKeyRef *     aKeyId,
                              otCryptoKeyType      aKeyType,
                              otCryptoKeyAlgorithm aKeyAlgorithm,
                              int                  aKeyUsage,
                              otCryptoKeyStorage   aKeyPersistence,
                              const uint8_t *      aKey,
                              size_t               aKeyLen)
{
    otError             error = OT_ERROR_NONE;
    sl_sec_man_status_t status;

    status = sl_sec_man_import_key(aKeyId, getPsaKeyType(aKeyType), getPsaAlgorithm(aKeyAlgorithm),
                                   getPsaKeyUsage(aKeyUsage), getPsaKeyPersistence(aKeyPersistence), aKey, aKeyLen);

    otEXPECT_ACTION((status == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

otError otPlatCryptoExportKey(otCryptoKeyRef aKeyId, uint8_t *aBuffer, size_t aBufferLen, size_t *aKeyLen)
{
    otError             error = OT_ERROR_NONE;
    sl_sec_man_status_t status;

    status = sl_sec_man_export_key(aKeyId, aBuffer, aBufferLen, aKeyLen);

    otEXPECT_ACTION((status == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

otError otPlatCryptoDestroyKey(otCryptoKeyRef aKeyId)
{
    otError             error = OT_ERROR_NONE;
    sl_sec_man_status_t status;

    status = sl_sec_man_destroy_key(aKeyId);

    otEXPECT_ACTION((status == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

bool otPlatCryptoHasKey(otCryptoKeyRef aKeyRef)
{
    psa_key_attributes_t aAttr = PSA_KEY_ATTRIBUTES_INIT;

    return (sl_sec_man_get_key_attributes(aKeyRef, &aAttr) == SL_SECURITY_MAN_SUCCESS);
}

// AES  Implementation
otError otPlatCryptoAesInit(otCryptoContext *aContext)
{
    otError error = OT_ERROR_NONE;
    (void)aContext;
    return error;
}

otError otPlatCryptoAesSetKey(otCryptoContext *aContext, const otCryptoKey *aKey)
{
    otError         error   = OT_ERROR_NONE;
    otCryptoKeyRef *mKeyRef = NULL;

    otEXPECT_ACTION((aContext != NULL) && (aContext->mContext != NULL), error = OT_ERROR_INVALID_ARGS);

    mKeyRef  = (otCryptoKeyRef *)aContext->mContext;
    *mKeyRef = aKey->mKeyRef;

exit:
    return error;
}

otError otPlatCryptoAesEncrypt(otCryptoContext *aContext, const uint8_t *aInput, uint8_t *aOutput)
{
    otError             error = OT_ERROR_NONE;
    sl_sec_man_status_t status;
    otCryptoKeyRef *    mKeyRef = NULL;

    otEXPECT_ACTION(((aContext != NULL) && (aContext->mContext != NULL) && (aOutput != NULL) && (aInput != NULL)), error = OT_ERROR_INVALID_ARGS);
    mKeyRef = (otCryptoKeyRef *)aContext->mContext;
    status  = sl_sec_man_aes_encrypt(*mKeyRef, PSA_ALG_ECB_NO_PADDING, aInput, aOutput);

    otEXPECT_ACTION((status == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

otError otPlatCryptoAesFree(otCryptoContext *aContext)
{
    otError error = OT_ERROR_NONE;
    (void)aContext;
    return error;
}

// HMAC implementations
otError otPlatCryptoHmacSha256Init(otCryptoContext *aContext)
{
    otError error = OT_ERROR_NONE;
    psa_mac_operation_t *mMacOperation = (psa_mac_operation_t *)aContext->mContext;
    *mMacOperation                     = psa_mac_operation_init();
    return error;
}

otError otPlatCryptoHmacSha256Deinit(otCryptoContext *aContext)
{
    otError error = OT_ERROR_NONE;
    psa_mac_operation_t *mMacOperation = (psa_mac_operation_t *)aContext->mContext;
    sl_sec_man_status_t  status;

    status = sl_sec_man_hmac_deinit(mMacOperation);

    otEXPECT_ACTION((status == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

otError otPlatCryptoHmacSha256Start(otCryptoContext *aContext, const otCryptoKey *aKey)
{
    otError              error         = OT_ERROR_NONE;
    psa_mac_operation_t *mMacOperation = (psa_mac_operation_t *)aContext->mContext;
    sl_sec_man_status_t  status;

    status = sl_sec_man_hmac_start(mMacOperation, aKey->mKeyRef);

    otEXPECT_ACTION((status == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

otError otPlatCryptoHmacSha256Update(otCryptoContext *aContext, const void *aBuf, uint16_t aBufLength)
{
    otError              error         = OT_ERROR_NONE;
    psa_mac_operation_t *mMacOperation = (psa_mac_operation_t *)aContext->mContext;
    sl_sec_man_status_t  status;

    status = sl_sec_man_hmac_update(mMacOperation, (const uint8_t *)aBuf, (size_t)aBufLength);

    otEXPECT_ACTION((status == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

otError otPlatCryptoHmacSha256Finish(otCryptoContext *aContext, uint8_t *aBuf, size_t aBufLength)
{
    otError              error         = OT_ERROR_NONE;
    psa_mac_operation_t *mMacOperation = (psa_mac_operation_t *)aContext->mContext;
    sl_sec_man_status_t  status;

    status = sl_sec_man_hmac_finish(mMacOperation, aBuf, aBufLength);

    otEXPECT_ACTION((status == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

// HKDF platform implementations
// As the HKDF does not actually use mbedTLS APIs but uses HMAC module, this feature is not implemented.
otError otPlatCryptoHkdfExpand(otCryptoContext * aContext,
                               const uint8_t *   aInfo,
                               uint16_t          aInfoLength,
                               uint8_t *         aOutputKey,
                               uint16_t          aOutputKeyLength)
{
    otError             error = OT_ERROR_NONE;
    sl_sec_man_status_t status;

    otEXPECT_ACTION(((aContext != NULL) && (aContext->mContext != NULL) && (aInfo != NULL) && (aOutputKey != NULL)), error = OT_ERROR_INVALID_ARGS);

    status = sl_sec_man_key_derivation_expand(aContext->mContext, aInfo, aInfoLength, aOutputKey, aOutputKeyLength);

    otEXPECT_ACTION((status == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

otError otPlatCryptoHkdfExtract(otCryptoContext *  aContext,
                                const uint8_t *    aSalt,
                                uint16_t           aSaltLength,
                                const otCryptoKey *aKey)
{
    otError             error = OT_ERROR_NONE;
    sl_sec_man_status_t status;

    otEXPECT_ACTION(((aContext != NULL) && (aContext->mContext != NULL) && (aKey != NULL) && (aSalt != NULL) && (aSaltLength != 0)),
                    error = OT_ERROR_INVALID_ARGS);

    status = sl_sec_man_key_derivation_extract(aContext->mContext, PSA_ALG_SHA_256, aKey->mKeyRef, aSalt, aSaltLength);

    otEXPECT_ACTION((status == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

// SHA256 platform implementations
otError otPlatCryptoSha256Init(otCryptoContext *aContext)
{
    otError               error = OT_ERROR_NONE;
    otEXPECT_ACTION((aContext != NULL), error = OT_ERROR_INVALID_ARGS);
    psa_hash_operation_t *ctx   = (psa_hash_operation_t *)aContext->mContext;

    otEXPECT_ACTION((ctx != NULL), error = OT_ERROR_INVALID_ARGS);

    *ctx = sl_sec_man_hash_init();

exit:
    return error;
}

otError otPlatCryptoSha256Deinit(otCryptoContext *aContext)
{
    otError               error = OT_ERROR_NONE;
    otEXPECT_ACTION((aContext != NULL), error = OT_ERROR_INVALID_ARGS);
    psa_hash_operation_t *ctx   = (psa_hash_operation_t *)aContext->mContext;

    otEXPECT_ACTION((ctx != NULL), error = OT_ERROR_INVALID_ARGS);
    otEXPECT_ACTION((sl_sec_man_hash_deinit(ctx) == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

otError otPlatCryptoSha256Start(otCryptoContext *aContext)
{
    otError               error = OT_ERROR_NONE;
    otEXPECT_ACTION((aContext != NULL), error = OT_ERROR_INVALID_ARGS);
    psa_hash_operation_t *ctx   = (psa_hash_operation_t *)aContext->mContext;

    otEXPECT_ACTION((ctx != NULL), error = OT_ERROR_INVALID_ARGS);
    otEXPECT_ACTION((sl_sec_man_hash_start(ctx, PSA_ALG_SHA_256) == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

otError otPlatCryptoSha256Update(otCryptoContext *aContext, const void *aBuf, uint16_t aBufLength)
{
    otError               error = OT_ERROR_NONE;
    otEXPECT_ACTION((aContext != NULL), error = OT_ERROR_INVALID_ARGS);
    psa_hash_operation_t *ctx   = (psa_hash_operation_t *)aContext->mContext;

    otEXPECT_ACTION(((ctx != NULL) && (aBuf != NULL)), error = OT_ERROR_INVALID_ARGS);
    otEXPECT_ACTION((sl_sec_man_hash_update(ctx, (uint8_t *)aBuf, aBufLength) == SL_SECURITY_MAN_SUCCESS), error = OT_ERROR_FAILED);

exit:
    return error;
}

otError otPlatCryptoSha256Finish(otCryptoContext *aContext, uint8_t *aHash, uint16_t aHashSize)
{
    otError               error       = OT_ERROR_NONE;
    size_t                aHashLength = 0;
    otEXPECT_ACTION((aContext != NULL), error = OT_ERROR_INVALID_ARGS);
    psa_hash_operation_t *ctx         = (psa_hash_operation_t *)aContext->mContext;

    otEXPECT_ACTION(((ctx != NULL) && (aHash != NULL)), error = OT_ERROR_INVALID_ARGS);
    otEXPECT_ACTION((sl_sec_man_hash_finish(ctx, aHash, aHashSize, &aHashLength) == SL_SECURITY_MAN_SUCCESS),
                    error = OT_ERROR_FAILED);

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
