/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <string.h>
#include "aes_alt.h"
#include "hal_aes.h"
#include "crypto_hw.h"

#if defined(MBEDTLS_AES_C) && defined (MBEDTLS_AES_ALT)

#define AES_BLOCK_SIZE 16
#define AES_IV_SIZE 16

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
    if((ctx == NULL) || (sizeof(mbedtls_aes_context) == 0))
    {
        printf("invalid parameters!!\n");
        return;
    }

    memset(ctx, 0, sizeof(mbedtls_aes_context));
}


/**
 * \brief          Clear AES context
 *
 * \param ctx      AES context to be cleared
 */
void mbedtls_aes_free(mbedtls_aes_context * ctx)
{
    if((ctx == NULL) || (sizeof(mbedtls_aes_context) == 0))
    {
        printf("invalid parameters!!\n");
        return;
    }

    memset(ctx, 0, sizeof(mbedtls_aes_context));
}


/**
 * @brief Internal function:
 * This function checks the validity of inputs and set the encrypt/decript key
 * called by mbedtls_aes_setkey_* functions.
 *
 * @returns: 0 on success, verious error in case of error.
 *
*/
static int aes_setkey(mbedtls_aes_context * ctx, const unsigned char * key,
        unsigned int keybits)
{
    AesContext_t *aesCtx = NULL;

    if((ctx == NULL) || (key == NULL))
    {
        printf("invalid parameters!!\n");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }
    if((keybits == 0) || (keybits % AES_BLOCK_SIZE))
    {
        printf("keybits should be a multiple of the block size (16 bytes)\n");
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    aesCtx = (AesContext_t*)ctx;
    aesCtx->keySizeByte = keybits/8;
    memcpy(aesCtx->keyBuf, key, aesCtx->keySizeByte);

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
    return aes_setkey(ctx, key, keybits);
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
    return aes_setkey(ctx, key, keybits);
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
    int ret = 0;
    hal_aes_status_t gcpu_ret = HAL_AES_STATUS_ERROR;
    hal_aes_buffer_t enc_data, plain_data, key_data;
    AesContext_t *aesCtx = NULL;

    //input param check
    if((ctx == NULL) || (input == NULL) || (output == NULL))
    {
        printf("invalid parameters!!\n");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }
    if((mode != MBEDTLS_AES_ENCRYPT) && (mode != MBEDTLS_AES_DECRYPT))
    {
        printf("invalid parameters!!\n");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    aesCtx = (AesContext_t*)ctx;
    enc_data.length = AES_BLOCK_SIZE;
    plain_data.length = AES_BLOCK_SIZE;
    key_data.buffer = aesCtx->keyBuf;
    key_data.length = aesCtx->keySizeByte;

    switch (mode)
    {
        case MBEDTLS_AES_ENCRYPT:
            enc_data.buffer = (uint8_t*)output;
            plain_data.buffer = (uint8_t*)input;
            gcpu_ret = hal_aes_ecb_encrypt(&enc_data, &plain_data, &key_data);
            break;
        case MBEDTLS_AES_DECRYPT:
            enc_data.buffer = (uint8_t*)input;
            plain_data.buffer = (uint8_t*)output;
            gcpu_ret = hal_aes_ecb_decrypt(&plain_data, &enc_data, &key_data);
            break;
        default:
            printf("invliad mode\n");
    }

    if(gcpu_ret != HAL_AES_STATUS_OK)
    {
        ret = MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    }

    return ret;
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
    int ret = 0;
    hal_aes_status_t gcpu_ret = HAL_AES_STATUS_ERROR;
    hal_aes_buffer_t enc_data, plain_data, key_data;
    AesContext_t *aesCtx = NULL;

    /* In case input size is 0 - do nothing and return with success*/
    if(length == 0)
    {
        return 0;
    }

    //input param check
    if((ctx == NULL) || (input == NULL) || (output == NULL) || (iv == NULL))
    {
        printf("invalid parameters!!\n");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if( length % AES_BLOCK_SIZE )
    {
        printf("Length should be a multiple of the block size (16 bytes)\n");
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }
    if((mode != MBEDTLS_AES_ENCRYPT) && (mode != MBEDTLS_AES_DECRYPT))
    {
        printf("invalid parameters!!\n");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    aesCtx = (AesContext_t*)ctx;
    plain_data.length = length;
    enc_data.length = length;
    key_data.buffer = aesCtx->keyBuf;
    key_data.length = aesCtx->keySizeByte;

    switch (mode)
    {
        case MBEDTLS_AES_ENCRYPT:
            enc_data.buffer = (uint8_t*)output;
            plain_data.buffer = (uint8_t*)input;
            gcpu_ret = hal_aes_cbc_encrypt(&enc_data, &plain_data, &key_data, iv);
            break;
        case MBEDTLS_AES_DECRYPT:
            enc_data.buffer = (uint8_t*)input;
            plain_data.buffer = (uint8_t*)output;
            gcpu_ret = hal_aes_cbc_decrypt(&plain_data, &enc_data, &key_data, iv);
            break;
        default:
            printf("invliad mode\n");
    }

    if(gcpu_ret != HAL_AES_STATUS_OK)
    {
        ret = MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    }

    return ret;
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
    int c;
    size_t n;
    int ret = 0;

    //input param check
    if((ctx == NULL) || (input == NULL) || (output == NULL) || (iv_off == NULL) || (iv == NULL))
    {
        printf("invalid parameters!!\n");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }
    if((mode != MBEDTLS_AES_ENCRYPT) && (mode != MBEDTLS_AES_DECRYPT))
    {
        printf("invalid parameters!!\n");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    n = *iv_off;
    if( n > 15 )
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );

    if( mode == MBEDTLS_AES_DECRYPT )
    {
        while( length-- )
        {
            if( n == 0 )
            {
                ret = mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );
                if( ret != 0 )
                    goto exit;
            }

            c = *input++;
            *output++ = (unsigned char)( c ^ iv[n] );
            iv[n] = (unsigned char) c;

            n = ( n + 1 ) & 0x0F;
        }
    }
    else
    {
        while( length-- )
        {
            if( n == 0 )
            {
                ret = mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );
                if( ret != 0 )
                    goto exit;
            }

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = ( n + 1 ) & 0x0F;
        }
    }

    *iv_off = n;

exit:
    return ret;

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
    return MBEDTLS_ERR_AES_FEATURE_UNAVAILABLE;
}

#endif /*MBEDTLS_CIPHER_MODE_CFB */

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
    int ret = 0;
    hal_aes_status_t gcpu_ret = HAL_AES_STATUS_ERROR;
    hal_aes_buffer_t enc_data, plain_data, key_data;
    AesContext_t *aesCtx = NULL;

    //unused variable
    (void)stream_block;

    /* In case input size is 0 - do nothing and return with success*/
    if(length == 0)
    {
        return 0;
    }

    //input param check
    if((ctx == NULL) || (input == NULL) || (output == NULL) || (nonce_counter == NULL) || (nc_off == NULL))
    {
        printf("invalid parameters!!\n");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }
    if((*nc_off != 0))
    {
        printf("offset other then 0 is not supported!!\n");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    aesCtx = (AesContext_t*)ctx;
    key_data.buffer = aesCtx->keyBuf;
    key_data.length = aesCtx->keySizeByte;

    plain_data.buffer = (uint8_t*)input;
    plain_data.length = length;
    enc_data.buffer = (uint8_t*)output;
    enc_data.length = length;

    gcpu_ret = hal_aes_ctr(&enc_data, &plain_data, &key_data, nonce_counter);
    if(gcpu_ret != HAL_AES_STATUS_OK)
    {
        ret = MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    }

    return ret;
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
    int ret = 0;
    size_t n;

    //input param check
    if((ctx == NULL) || (input == NULL) || (output == NULL) || (iv_off == NULL) || (iv == NULL))
    {
        printf("invalid parameters!!\n");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    n = *iv_off;

    if( n > 15 )
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );

    while( length-- )
    {
        if( n == 0 )
        {
            ret = mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );
            if( ret != 0 )
                goto exit;
        }
        *output++ =  *input++ ^ iv[n];

        n = ( n + 1 ) & 0x0F;
    }

    *iv_off = n;

exit:
    return( ret );
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */

#endif //defined(MBEDTLS_AES_C) && defined (MBEDTLS_AES_ALT)
