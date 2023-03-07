/*
 *  NIST SP800-38C compliant CCM implementation
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 * Copyright (C) 2018 Beerten Engineering
 * Copyright (C) 2018 Silex Inside
 *      hardware offload with CryptoSoc.
 */

/*
 * Definition of CCM:
 * http://csrc.nist.gov/publications/nistpubs/800-38C/SP800-38C_updated-July20_2007.pdf
 * RFC 3610 "Counter with CBC-MAC (CCM)"
 *
 * Related:
 * RFC 5116 "An Interface and Algorithms for Authenticated Encryption"
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_CCM_C)

#include "mbedtls/ccm.h"

#include <string.h>

#if defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_AES_C)
#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#endif /* MBEDTLS_PLATFORM_C */
#endif /* MBEDTLS_SELF_TEST && MBEDTLS_AES_C */

#if defined(MBEDTLS_CCM_ALT)
#include <sx_generic.h>

#include <sx_aes.h>
#include <cryptolib_def.h>

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = (unsigned char*)v; while( n-- ) *p++ = 0;
}

#define CCM_ENCRYPT 0
#define CCM_DECRYPT 1

/*
 * Initialize context
 */
void mbedtls_ccm_init( mbedtls_ccm_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_ccm_context ) );
}


int mbedtls_ccm_setkey( mbedtls_ccm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        const unsigned char *key,
                        unsigned int keybits )
{
    if ( cipher != MBEDTLS_CIPHER_ID_AES )
        return ( MBEDTLS_ERR_CCM_BAD_INPUT );

#if defined(GP_DIVERSITY_GPHAL_K8E)
/* 192-bit AES keys not supported on K8E */
    if ( keybits != 128 && keybits != 256)
        return ( MBEDTLS_ERR_CCM_BAD_INPUT );
#else
    if ( keybits != 128 && keybits != 192 && keybits != 256)
        return ( MBEDTLS_ERR_CCM_BAD_INPUT );
#endif

    memcpy( ctx->key, key, keybits / 8 );
    ctx->keysize = keybits / 8;

    return( 0 );
}

/*
 * Free context
 */
void mbedtls_ccm_free( mbedtls_ccm_context *ctx )
{
    mbedtls_zeroize( ctx, sizeof( mbedtls_ccm_context ) );
}

/*
 * Authenticated encryption or decryption
 */
static int ccm_auth_crypt( mbedtls_ccm_context *ctx, int mode, size_t length,
                           const unsigned char *iv, size_t iv_len,
                           const unsigned char *add, size_t add_len,
                           const unsigned char *input, unsigned char *output,
                           unsigned char *tag, size_t tag_len )
{
    int ret;

    /*
     * Check length requirements: SP800-38C A.1
     * Additional requirement: a < 2^16 - 2^8 to simplify the code.
     * 'length' checked later (when writing it to the first block)
     */
    if( tag_len < 4 || tag_len > 16 || tag_len % 2 != 0 )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    /* Also implies q is within bounds */
    if( iv_len < 7 || iv_len > 13 )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    if (add_len > 0xFF00)
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    sx_enable_clock();

    ret = sx_aes_blk(
        CCM,
        (mode == CCM_ENCRYPT) ? ENC : DEC,
        CTX_WHOLE,
        block_t_convert(ctx->key, ctx->keysize),
        NULL_blk, // no xts key for CCM
        NULL_blk,  // no IV for CCM
        block_t_convert(input, length),
        block_t_convert(output, length),
        block_t_convert(add, add_len),
        block_t_convert(tag, tag_len),
        NULL_blk, // no ctx_ptr
        block_t_convert(iv, iv_len)
    );

    sx_disable_clock();

    if (ret == CRYPTOLIB_INVALID_SIGN_ERR )
        return( MBEDTLS_ERR_CCM_AUTH_FAILED );
    if ( ret == CRYPTOLIB_INVALID_PARAM )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );
    if ( ret )
        return( MBEDTLS_ERR_CCM_HW_ACCEL_FAILED );

    return( 0 );
}

/*
 * Authenticated encryption
 */
int mbedtls_ccm_encrypt_and_tag( mbedtls_ccm_context *ctx, size_t length,
                         const unsigned char *iv, size_t iv_len,
                         const unsigned char *add, size_t add_len,
                         const unsigned char *input, unsigned char *output,
                         unsigned char *tag, size_t tag_len )
{
    return( ccm_auth_crypt( ctx, CCM_ENCRYPT, length, iv, iv_len,
                            add, add_len, input, output, tag, tag_len ) );
}

/*
 * Authenticated decryption
 */
int mbedtls_ccm_auth_decrypt( mbedtls_ccm_context *ctx, size_t length,
                      const unsigned char *iv, size_t iv_len,
                      const unsigned char *add, size_t add_len,
                      const unsigned char *input, unsigned char *output,
                      const unsigned char *tag, size_t tag_len )
{
    int ret;

    ret = ccm_auth_crypt( ctx, CCM_DECRYPT, length,
                          iv, iv_len, add, add_len,
                          input, output, (unsigned char*)tag, tag_len );

    if( ret == MBEDTLS_ERR_CCM_AUTH_FAILED )
    {
        mbedtls_zeroize( output, length );
        return( MBEDTLS_ERR_CCM_AUTH_FAILED );
    }

    return( ret );
}

#endif /* MBEDTLS_CCM_ALT */

#endif /* MBEDTLS_CCM_C */
