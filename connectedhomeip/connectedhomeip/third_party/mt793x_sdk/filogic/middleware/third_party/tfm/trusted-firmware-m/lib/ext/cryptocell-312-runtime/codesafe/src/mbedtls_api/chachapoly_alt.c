/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_CHACHAPOLY_C)
#include "mbedtls/chachapoly.h"
#include "poly.h"
#include "mbedtls/chacha20.h"
#include "mbedtls/platform_util.h"
#include "chacha_driver.h"
#include "cc_pal_abort.h"
#include "cc_pal_mem.h"
#include "cc_pal_types.h"


#if defined(MBEDTLS_SELF_TEST)
#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#define mbedtls_printf printf
#endif /* MBEDTLS_PLATFORM_C */
#endif /* MBEDTLS_SELF_TEST */

#if defined(MBEDTLS_CHACHAPOLY_ALT)

void mbedtls_chachapoly_init( mbedtls_chachapoly_context *ctx )
{
    if (NULL == ctx)
    {
        CC_PalAbort("ctx cannot be NULL");
    }

    mbedtls_platform_zeroize( ctx->key, sizeof( ctx->key ) );

}

void mbedtls_chachapoly_free( mbedtls_chachapoly_context *ctx )
{
    if (ctx == NULL) {
        return;
    }
    mbedtls_platform_zeroize( ctx->key, sizeof( ctx->key ) );
}

int mbedtls_chachapoly_setkey( mbedtls_chachapoly_context *ctx,
                               const unsigned char key[32] )
{
    if ( (ctx == NULL) || (key == NULL)) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }
    CC_PalMemCopy(ctx->key, key, MBEDTLS_CHACHA_KEY_SIZE_BYTES);
    return ( 0 );
}

/* Cryptocell only supports integrated chachapoly operations  */
int mbedtls_chachapoly_starts( mbedtls_chachapoly_context *ctx,
                               const unsigned char nonce[12],
                               mbedtls_chachapoly_mode_t mode  )
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(nonce);
    CC_UNUSED_PARAM(mode);
    return MBEDTLS_ERR_POLY1305_FEATURE_UNAVAILABLE;
}
/* Cryptocell only supports integrated chachapoly operations  */
int mbedtls_chachapoly_update_aad( mbedtls_chachapoly_context *ctx,
                                   const unsigned char *aad,
                                   size_t aad_len )
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(aad);
    CC_UNUSED_PARAM(aad_len);
    return MBEDTLS_ERR_POLY1305_FEATURE_UNAVAILABLE;
}

/* Cryptocell only supports integrated chachapoly operations  */
int mbedtls_chachapoly_update( mbedtls_chachapoly_context *ctx,
                               size_t len,
                               const unsigned char *input,
                               unsigned char *output )
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(len);
    CC_UNUSED_PARAM(input);
    CC_UNUSED_PARAM(output);
    return MBEDTLS_ERR_POLY1305_FEATURE_UNAVAILABLE;
}

/* Cryptocell only supports integrated chachapoly operations  */
int mbedtls_chachapoly_finish( mbedtls_chachapoly_context *ctx,
                               unsigned char mac[16] )
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(mac);
    return MBEDTLS_ERR_POLY1305_FEATURE_UNAVAILABLE;
}

static int chachapoly_crypt_and_tag( mbedtls_chachapoly_mode_t mode,
                                     size_t length,
                                     const unsigned char nonce[12],
                                     const unsigned char key[32],
                                     const unsigned char *aad,
                                     size_t aad_len,
                                     const unsigned char *input,
                                     unsigned char *output,
                                     unsigned char tag[16] )
{
    int rc;
    uint8_t chachaInState[MBEDTLS_CHACHA_BLOCK_SIZE_BYTES] = {0};
    uint8_t chachaOutState[MBEDTLS_CHACHA_BLOCK_SIZE_BYTES] = {0};
    mbedtls_poly_key polyKey = {0};
    mbedtls_poly_mac polyMac = {0};
    const uint8_t *pCipherData = NULL;

    if (key == NULL) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }

    if (mode == MBEDTLS_CHACHAPOLY_ENCRYPT) {
        pCipherData = output;
    } else if (mode == MBEDTLS_CHACHAPOLY_DECRYPT) {
        pCipherData = input;
    } else {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }

    // 1. Generate poly key
    // Calling mbedtls_chacha with data=0 is like performing the chacha block function without the encryption
    rc = mbedtls_chacha20_crypt( key, nonce, 0, sizeof(chachaInState), chachaInState, chachaOutState );
    if (rc != 0) {
        goto end_with_error;
    }
    // poly key defined as the first 32 bytes of chacha output.
    CC_PalMemCopy(polyKey, chachaOutState, sizeof(polyKey));

    // 2. Encryption pDataIn
    if (mode == MBEDTLS_CHACHAPOLY_ENCRYPT) {
        rc = mbedtls_chacha20_crypt( key, nonce, 1, length, (uint8_t *)input, (uint8_t *)output );
        if (rc != 0) {
            goto end_with_error;
        }
    }

    // 3. Authentication
    rc = PolyMacCalc(polyKey, aad, aad_len, pCipherData, length, polyMac, true);
    if (rc != 0) {
        rc = MBEDTLS_ERR_CHACHAPOLY_AUTH_FAILED;
        goto end_with_error;
    }

    CC_PalMemCopy(tag, polyMac, sizeof(polyMac));

    if (mode == MBEDTLS_CHACHAPOLY_DECRYPT) {
        rc = mbedtls_chacha20_crypt( key, nonce, 1, length, (uint8_t *)input, (uint8_t *)output );
        if (rc != 0) {
            goto end_with_error;
        }
    }

    return ( 0 );

    end_with_error:
    if (output != NULL) {
        CC_PalMemSetZero(output, length);
    }
    return rc;
}

int mbedtls_chachapoly_encrypt_and_tag( mbedtls_chachapoly_context *ctx,
                                        size_t length,
                                        const unsigned char nonce[12],
                                        const unsigned char *aad,
                                        size_t aad_len,
                                        const unsigned char *input,
                                        unsigned char *output,
                                        unsigned char tag[16] )
{

    if ( (ctx == NULL) || (nonce == NULL) || (tag == NULL) ) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }

    if ( aad_len != 0 && aad == NULL ) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }
    if ( length != 0 && input == NULL ) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }
    if ( length != 0 && output == NULL ) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }

    return( chachapoly_crypt_and_tag( MBEDTLS_CHACHAPOLY_ENCRYPT,
                                      length, nonce, ctx->key, aad, aad_len,
                                      input, output, tag ) );
}

int mbedtls_chachapoly_auth_decrypt( mbedtls_chachapoly_context *ctx,
                                     size_t length,
                                     const unsigned char nonce[12],
                                     const unsigned char *aad,
                                     size_t aad_len,
                                     const unsigned char tag[16],
                                     const unsigned char *input,
                                     unsigned char *output )
{
    unsigned char check_tag[16];
    int ret;
    int diff;
    size_t i;

    if ( (nonce == NULL) || (tag == NULL) ) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }

    if ( aad_len != 0 && aad == NULL ) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }
    if ( length != 0 && input == NULL ) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }
    if ( length != 0 && output == NULL ) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }

    if ( (ret = chachapoly_crypt_and_tag( MBEDTLS_CHACHAPOLY_DECRYPT,
                                      length, nonce, ctx->key, aad, aad_len,
                                      input, output, check_tag ) ) != 0 ) {
        return( ret );
    }

    /* Check tag in "constant-time" */
    for( diff = 0, i = 0; i < sizeof( check_tag ); i++ )
        diff |= tag[i] ^ check_tag[i];

    if( diff != 0 )
    {
        mbedtls_platform_zeroize( output, length );
        return( MBEDTLS_ERR_CHACHAPOLY_AUTH_FAILED );
    }
    return ( 0 );

}

#endif /* !MBEDTLS_CHACHAPOLY_ALT */

#endif /* !MBEDTLS_CHACHAPOLY_C */
