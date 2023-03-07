/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include "mbedtls/aes_alt.h"

#if defined(MBEDTLS_AES_C) && defined (MBEDTLS_AES_ALT)

#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "aes_driver.h"

/**
 * \brief          Initialize AES context
 *
 *
 * \note           Context block should be pre-allocated by the caller.
 *
 * \param ctx      AES context to be initialized
 */
void mbedtls_aes_init(mbedtls_aes_context * ctx)
{
    AesContext_t *aesCtx = NULL;

    if (NULL == ctx)
    {
        CC_PalAbort("ctx cannot be NULL");
    }

    /* check size of structs match */
    if (sizeof(mbedtls_aes_context) != sizeof(AesContext_t))
    {
        CC_PalAbort("!!!!AES context sizes mismatch!!!\n");
    }

    aesCtx = (AesContext_t *)ctx;

    aesCtx->padType = CRYPTO_PADDING_NONE;
    aesCtx->dataBlockType = FIRST_BLOCK;
    aesCtx->inputDataAddrType = DLLI_ADDR;
    aesCtx->outputDataAddrType = DLLI_ADDR;
}


/**
 * \brief          Clear AES context
 *
 * \param ctx      AES context to be cleared
 */
void mbedtls_aes_free(mbedtls_aes_context * ctx)
{
    if (NULL == ctx)
    {
        CC_PAL_LOG_ERR("ctx cannot be NULL\n");
        return;
    }
    CC_PalMemSet(ctx, 0, sizeof(mbedtls_aes_context));
}


/**
 * @brief Internal function:
 * This function checks the validity of inputs and set the encrypt/decript key & direction
 * called by mbedtls_aes_setkey_* functions.
 *
 * @returns: 0 on success, verious error in case of error.
 *
 */
static int aes_setkey(mbedtls_aes_context * ctx, const unsigned char * key,
        unsigned int keybits, cryptoDirection_t dir)
{
    AesContext_t *aesCtx = NULL;

    /* if the users context ID pointer is NULL return an error */
    if (NULL == ctx)
    {
        CC_PAL_LOG_ERR("ctx cannot be NULL\n");
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    /* check the validity of the key data pointer */
    if (NULL == key)
    {
        CC_PAL_LOG_ERR("key cannot be NULL\n");
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    aesCtx = (AesContext_t *)ctx;
    aesCtx->dir = dir;
    aesCtx->cryptoKey = USER_KEY;

    switch (keybits) {
        case 128:
            aesCtx->keySizeId = KEY_SIZE_128_BIT;
            break;
        case 192:
            aesCtx->keySizeId = KEY_SIZE_192_BIT;
            break;
        case 256:
            aesCtx->keySizeId = KEY_SIZE_256_BIT;
            break;
        default:
            CC_PAL_LOG_ERR("key length (%d) not supported\n", keybits);
            return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
        }

    CC_PalMemCopy(aesCtx->keyBuf, key, keybits/8);

    return (0);   // no mbedTLS const for OK.

}

/*
 * Copy the key into the context, and set the direction to encryption.
 * A lot of the initialization needed by CC, will be done in the actual crypt function.
 *
 * mbedTLS error codes are much more limited then CC, so have to map a bit.
 *
 *
 */
int mbedtls_aes_setkey_enc(mbedtls_aes_context * ctx, const unsigned char * key,
        unsigned int keybits)
{
    return aes_setkey(ctx, key, keybits, CRYPTO_DIRECTION_ENCRYPT);
}

/**
 * \brief          AES key schedule (decryption)
 *
 * \param ctx      AES context to be initialized
 * \param key      decryption key
 * \param keybits  must be 128, 192 or 256
 *
 * \return         0 if successful, or MBEDTLS_ERR_AES_INVALID_KEY_LENGTH/
 *                 MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH
 */


int mbedtls_aes_setkey_dec(mbedtls_aes_context * ctx, const unsigned char * key,
        unsigned int keybits)
{
    return aes_setkey(ctx, key, keybits, CRYPTO_DIRECTION_DECRYPT);
}

/**
 * \brief          AES-ECB block encryption/decryption
 *
 * \param ctx      AES context
 * \param mode     MBEDTLS_AES_ENCRYPT or MBEDTLS_AES_DECRYPT
 * \param input    16-byte input block
 * \param output   16-byte output block
 *
 * \return         0 if successful, MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH otherwise
 */


int mbedtls_aes_crypt_ecb(mbedtls_aes_context * ctx,
        int mode,
        const unsigned char input[AES_BLOCK_SIZE],
        unsigned char output[AES_BLOCK_SIZE])
{
    AesContext_t *aesCtx = NULL;
    drvError_t drvRet;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    if (NULL == ctx || NULL == input || NULL == output)
    {
        CC_PAL_LOG_ERR("Null pointer exception\n");
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }
    if (MBEDTLS_AES_ENCRYPT != mode && MBEDTLS_AES_DECRYPT != mode)
    {
        CC_PAL_LOG_ERR("Mode %d is not supported\n", mode);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    aesCtx = (AesContext_t *)ctx;

    if (((MBEDTLS_AES_ENCRYPT == mode) && (CRYPTO_DIRECTION_ENCRYPT != aesCtx->dir))  ||
            ((MBEDTLS_AES_DECRYPT == mode) && (CRYPTO_DIRECTION_DECRYPT != aesCtx->dir)))
    {
        //someone made a mistake - set key in the wrong direction
        CC_PAL_LOG_ERR("Key & operation mode mismatch: mode = %d. aesCtx->dir = %d\n", mode, aesCtx->dir);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    aesCtx->mode = CIPHER_ECB;

    drvRet = SetDataBuffersInfo(input, AES_BLOCK_SIZE, &inBuffInfo,
                                output, AES_BLOCK_SIZE, &outBuffInfo);
    if (drvRet != 0) {
         CC_PAL_LOG_ERR("illegal data buffers\n");
         return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
     }

    drvRet = ProcessAesDrv(aesCtx, &inBuffInfo, &outBuffInfo, AES_BLOCK_SIZE);

    if (drvRet != AES_DRV_OK)
    {
        CC_PAL_LOG_ERR("ecb crypt failed with error code %d\n", drvRet);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }
    return (0);
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * AES-CBC buffer encryption/decryption
 */
int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[AES_IV_SIZE],
                    const unsigned char *input,
                    unsigned char *output )
{

    AesContext_t *aesCtx = NULL;
    drvError_t drvRet;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    if (0 == length) /* In case input size is 0 - do nothing and return with success*/
    {
        return (0);
    }

    if (NULL == ctx || NULL == input || NULL == output || NULL == iv)
    {
        CC_PAL_LOG_ERR("Null pointer exception\n");
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    if (MBEDTLS_AES_ENCRYPT != mode && MBEDTLS_AES_DECRYPT != mode)
    {
        CC_PAL_LOG_ERR("Mode %d is not supported\n", mode);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    if( length % AES_BLOCK_SIZE )
    {
        CC_PAL_LOG_ERR("Length should be a multiple of the block size (16 bytes)\n");
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    aesCtx = (AesContext_t *)ctx;

    if (((MBEDTLS_AES_ENCRYPT == mode) && (CRYPTO_DIRECTION_ENCRYPT != aesCtx->dir))  ||
            ((MBEDTLS_AES_DECRYPT == mode) && (CRYPTO_DIRECTION_DECRYPT != aesCtx->dir)))
    {
        //someone made a mistake - set key in the wrong direction
        CC_PAL_LOG_ERR("Key & operation mode mismatch: operation %d key %d\n", mode, aesCtx->dir);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    CC_PalMemCopy(aesCtx->ivBuf, iv, AES_IV_SIZE);
    aesCtx->mode = CIPHER_CBC;

    drvRet = SetDataBuffersInfo(input, length, &inBuffInfo,
                                output, length, &outBuffInfo);
    if (drvRet != 0) {
         CC_PAL_LOG_ERR("illegal data buffers\n");
         return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
     }

    drvRet = ProcessAesDrv(aesCtx, &inBuffInfo, &outBuffInfo, length);

    if (drvRet != AES_DRV_OK)
    {
        CC_PAL_LOG_ERR("cbc crypt failed with error code %d\n", drvRet);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    CC_PalMemCopy(iv, aesCtx->ivBuf, AES_IV_SIZE);

    return (0);
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[AES_IV_SIZE],
                       const unsigned char *input,
                       unsigned char *output )
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(mode);
    CC_UNUSED_PARAM(length);
    CC_UNUSED_PARAM(iv_off);
    CC_UNUSED_PARAM(iv);
    CC_UNUSED_PARAM(input);
    CC_UNUSED_PARAM(output);
    return MBEDTLS_ERR_AES_FEATURE_UNAVAILABLE;
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       unsigned char iv[AES_IV_SIZE],
                       const unsigned char *input,
                       unsigned char *output )
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(mode);
    CC_UNUSED_PARAM(length);
    CC_UNUSED_PARAM(iv);
    CC_UNUSED_PARAM(input);
    CC_UNUSED_PARAM(output);
    return MBEDTLS_ERR_AES_FEATURE_UNAVAILABLE;
}

#endif /*MBEDTLS_CIPHER_MODE_CFB */
#if defined(MBEDTLS_CIPHER_MODE_CTR) || defined(MBEDTLS_CIPHER_MODE_OFB)
/*
 * AES-CTR buffer encryption/decryption
 */
static int aes_crypt_ctr_ofb( mbedtls_aes_context *ctx,
                              aesMode_t mode,
                              size_t length,
                              size_t *iv_off,
                              unsigned char iv[AES_BLOCK_SIZE],
                              const unsigned char *input,
                              unsigned char *output )
{
    AesContext_t *aesCtx = NULL;
    drvError_t drvRet;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    if (0 == length) /* In case input size is 0 - do nothing and return with success*/
    {
        return (0);
    }

    if (NULL == ctx || NULL == iv || NULL == input || NULL == output)
    {
        CC_PAL_LOG_ERR("Null pointer exception\n");
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    if ((iv_off != NULL) && (*iv_off != 0))
    {
        CC_PAL_LOG_ERR("offset other then 0 is not supported\n");
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    aesCtx = (AesContext_t *)ctx;

    aesCtx->mode = mode;
    CC_PalMemCopy(aesCtx->ivBuf, iv, AES_BLOCK_SIZE);

    drvRet = SetDataBuffersInfo(input, length, &inBuffInfo,
                               output, length, &outBuffInfo);
    if (drvRet != 0) {
         CC_PAL_LOG_ERR("illegal data buffers\n");
         return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    drvRet = ProcessAesDrv(aesCtx, &inBuffInfo, &outBuffInfo, length);
    if (drvRet != AES_DRV_OK)
    {
        CC_PAL_LOG_ERR("ctr/ofb crypt failed with error code %d\n", drvRet);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }
    CC_PalMemCopy(iv, aesCtx->ivBuf, AES_BLOCK_SIZE);

    return (0);
}
#endif /* MBEDTLS_CIPHER_MODE_CTR || MBEDTLS_CIPHER_MODE_OFB*/

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr( mbedtls_aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[AES_BLOCK_SIZE],
                       unsigned char stream_block[AES_BLOCK_SIZE],
                       const unsigned char *input,
                       unsigned char *output )
{
    CC_UNUSED_PARAM(stream_block);
    return aes_crypt_ctr_ofb(ctx, CIPHER_CTR, length, nc_off, nonce_counter, input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ofb( mbedtls_aes_context *ctx,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[AES_BLOCK_SIZE],
                       const unsigned char *input,
                       unsigned char *output )
{
    return aes_crypt_ctr_ofb(ctx, CIPHER_OFB, length, iv_off, iv, input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */

/*
 * AES-ECB block encryption
 */
int mbedtls_internal_aes_encrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    return( mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT,  input, output) );
}

/**
 * \brief           Internal AES block encryption function
 *                  (Only exposed to allow overriding it,
 *                  see MBEDTLS_AES_ENCRYPT_ALT)
 *
 * \param ctx       AES context
 * \param input     Plaintext block
 * \param output    Output (ciphertext) block
 */
void mbedtls_aes_encrypt( mbedtls_aes_context *ctx,
        const unsigned char input[16],
        unsigned char output[16] )
{
    mbedtls_internal_aes_encrypt( ctx, input, output );
}

/*
 * AES-ECB block decryption
 */
int mbedtls_internal_aes_decrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    return( mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_DECRYPT,  input, output) );
}

void mbedtls_aes_decrypt( mbedtls_aes_context *ctx,
        const unsigned char input[16],
        unsigned char output[16] )
{
    mbedtls_internal_aes_decrypt( ctx, input, output );
}

#if defined(MBEDTLS_CIPHER_MODE_XTS)

void mbedtls_aes_xts_init( mbedtls_aes_xts_context *ctx )
{
    CC_UNUSED_PARAM(ctx);
    return ;
}

void mbedtls_aes_xts_free( mbedtls_aes_xts_context *ctx )
{
    CC_UNUSED_PARAM(ctx);
    return ;
}


int mbedtls_aes_xts_setkey_enc( mbedtls_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(key);
    CC_UNUSED_PARAM(keybits);
    return MBEDTLS_ERR_AES_FEATURE_UNAVAILABLE;
}

int mbedtls_aes_xts_setkey_dec( mbedtls_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(key);
    CC_UNUSED_PARAM(keybits);
    return MBEDTLS_ERR_AES_FEATURE_UNAVAILABLE;
}


int mbedtls_aes_crypt_xts( mbedtls_aes_xts_context *ctx,
                       int mode,
                       size_t length,
                       const unsigned char data_unit[AES_BLOCK_SIZE],
                       const unsigned char *input,
                       unsigned char *output )

{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(mode);
    CC_UNUSED_PARAM(length);
    CC_UNUSED_PARAM(data_unit);
    CC_UNUSED_PARAM(input);
    CC_UNUSED_PARAM(output);
    return MBEDTLS_ERR_AES_FEATURE_UNAVAILABLE;
}
#endif //defined(MBEDTLS_CIPHER_MODE_XTS)

#endif //defined(MBEDTLS_AES_C) && defined (MBEDTLS_AES_ALT)
