/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file implements AES-CCM using mbedtls CCM.
 */

#include <limits.h>

#include "aes_ccm_alt.hpp"
#include <string.h>
#include <openthread/error.h>
#include "common/code_utils.hpp"
#include "common/message.hpp"
#include "openthread/platform/crypto.h"

#if defined(MBEDTLS_CCM_ALT)

#define aes_ccm_calloc calloc
#define aes_ccm_free free

otError otPlatCryptoCcmCreate(otCryptoContext *aContext)
{
    platform_ccm_ctx_t *ccm_ctx;
    otError             error = OT_ERROR_NONE;

    ccm_ctx = (platform_ccm_ctx_t *)aes_ccm_calloc(1, sizeof(platform_ccm_ctx_t));
    VerifyOrExit(ccm_ctx != nullptr, error = OT_ERROR_NO_BUFS);

    ccm_ctx->mOriginal_ctx =
        (ot::Crypto::AesCcm::aesccm_context_t *)aContext->mContext; /* store the original context */
    aContext->mContext     = (void *)ccm_ctx;
    aContext->mContextSize = sizeof(platform_ccm_ctx_t);

    mbedtls_ccm_init(&ccm_ctx->mMbed_ccm_ctx);

exit:
    return error;
}

otError otPlatCryptoCcmFree(otCryptoContext *aContext)
{
    platform_ccm_ctx_t *ccm_ctx = static_cast<platform_ccm_ctx_t *>(aContext->mContext);

    if (ccm_ctx != nullptr)
    {
        if (ccm_ctx->mAadPtr != nullptr)
            aes_ccm_free(ccm_ctx->mAadPtr);

        aes_ccm_free(ccm_ctx);
    }

    return OT_ERROR_NONE;
}

otError otPlatCryptoCcmSetKey(otCryptoContext *aContext, const otCryptoKey *aKey)
{
    platform_ccm_ctx_t *ccm_ctx = static_cast<platform_ccm_ctx_t *>(aContext->mContext);

    mbedtls_ccm_setkey(&ccm_ctx->mMbed_ccm_ctx, MBEDTLS_CIPHER_ID_AES, aKey->mKey, aKey->mKeyLength << 3);

    return OT_ERROR_NONE;
}

otError otPlatCryptoCcmInit(otCryptoContext *aContext,
                            uint32_t         aHeaderLength,
                            uint32_t         aPlainTextLength,
                            uint8_t          aTagLength,
                            const void *     aNonce,
                            uint8_t          aNonceLength)
{
    platform_ccm_ctx_t *ccm_ctx = static_cast<platform_ccm_ctx_t *>(aContext->mContext);
    otError             error   = OT_ERROR_NONE;

    ccm_ctx->mAadLength       = aHeaderLength;
    ccm_ctx->mAadCurLength    = 0;
    ccm_ctx->mNonceLength     = aNonceLength;
    ccm_ctx->mNoncePtr        = (uint8_t *)aNonce;
    ccm_ctx->mInputTextLength = aPlainTextLength;
    ccm_ctx->mTagLength       = aTagLength;
    ccm_ctx->mAadPtr          = NULL;

    /* allocate memory for aad input */
    ccm_ctx->mAadPtr = (uint8_t *)aes_ccm_calloc(1, ccm_ctx->mAadLength);
    VerifyOrExit(ccm_ctx->mAadPtr != nullptr, error = OT_ERROR_NO_BUFS);

exit:
    return error;
}

otError otPlatCryptoCcmHeader(otCryptoContext *aContext, const void *aHeader, uint32_t aHeaderLength)
{
    platform_ccm_ctx_t *ccm_ctx = static_cast<platform_ccm_ctx_t *>(aContext->mContext);
    otError             error   = OT_ERROR_NONE;

    if ((ccm_ctx->mAadCurLength + aHeaderLength) <= ccm_ctx->mAadLength)
    {
        memcpy(ccm_ctx->mAadPtr + ccm_ctx->mAadCurLength, aHeader, aHeaderLength);
        ccm_ctx->mAadCurLength += aHeaderLength;
    }
    else
    {
        error = OT_ERROR_INVALID_ARGS;
    }

    return error;
}

otError otPlatCryptoCcmPayload(otCryptoContext *aContext,
                               void *           aPlainText,
                               void *           aCipherText,
                               uint32_t         aLength,
                               uint8_t          aMode)
{
    platform_ccm_ctx_t *ccm_ctx = static_cast<platform_ccm_ctx_t *>(aContext->mContext);
    otError             error   = OT_ERROR_NONE;
    int                 ret     = 0;

    VerifyOrExit(aLength == ccm_ctx->mInputTextLength, error = OT_ERROR_INVALID_ARGS);

    switch (aMode)
    {
    case kEncrypt:
        ret = mbedtls_ccm_encrypt_and_tag(&ccm_ctx->mMbed_ccm_ctx, ccm_ctx->mInputTextLength, ccm_ctx->mNoncePtr,
                                          ccm_ctx->mNonceLength, ccm_ctx->mAadPtr, ccm_ctx->mAadLength,
                                          (uint8_t *)aPlainText, (uint8_t *)aCipherText, (uint8_t *)ccm_ctx->mTag,
                                          ccm_ctx->mTagLength);
        break;
    case kDecrypt:
        mbedtls_ccm_auth_decrypt(&ccm_ctx->mMbed_ccm_ctx, ccm_ctx->mInputTextLength, ccm_ctx->mNoncePtr,
                                 ccm_ctx->mNonceLength, ccm_ctx->mAadPtr, ccm_ctx->mAadLength, (uint8_t *)aCipherText,
                                 (uint8_t *)aPlainText, NULL, ccm_ctx->mTagLength);
        mbedtls_ccm_get_decrypt_tag(&ccm_ctx->mMbed_ccm_ctx, (uint8_t *)ccm_ctx->mTag, ccm_ctx->mTagLength);
        break;
    default:
        error = OT_ERROR_INVALID_ARGS;
    }

    if (ret == MBEDTLS_ERR_CCM_AUTH_FAILED)
        error = OT_ERROR_SECURITY;
    else if (ret == MBEDTLS_ERR_CCM_BAD_INPUT)
        error = OT_ERROR_INVALID_ARGS;
    if (ret != 0)
        printf("AesCcmAlt error %d\n", error);

exit:
    return error;
}

otError otPlatCryptoCcmMessagePayload(otCryptoContext *aContext, uint8_t aMode)
{
    platform_ccm_ctx_t *ccm_ctx  = static_cast<platform_ccm_ctx_t *>(aContext->mContext);
    ot::Message *       aMessage = ccm_ctx->mOriginal_ctx->mMessage;
    otError             error    = OT_ERROR_NONE;
    void *              buf;
    uint16_t            length;

    buf = aes_ccm_calloc(1, ccm_ctx->mInputTextLength);
    VerifyOrExit(buf != nullptr, error = OT_ERROR_NO_BUFS);

    length = aMessage->ReadBytes(aMessage->GetOffset(), buf, ccm_ctx->mInputTextLength);
    otPlatCryptoCcmPayload(aContext, buf, buf, length, aMode);
    aMessage->WriteBytes(aMessage->GetOffset(), buf, length);
    aMessage->MoveOffset(length);

    aes_ccm_free(buf);

exit:
    return error;
}

otError otPlatCryptoCcmFinalize(otCryptoContext *aContext, void *aTag)
{
    platform_ccm_ctx_t *ccm_ctx = static_cast<platform_ccm_ctx_t *>(aContext->mContext);

    memcpy(aTag, ccm_ctx->mTag, ccm_ctx->mTagLength);

    return OT_ERROR_NONE;
}

#endif /* !MBEDTLS_CCM_ALT */
