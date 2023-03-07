/*
 *  FIPS-180-2 compliant SHA-256 implementation
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
 * Copyright (C) 2018 Silex Inside
 *      hardware offload with CryptoSoc.
 */
/*
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SHA256_C)

#include <mbedtls/sha256.h>
#include <sx_generic.h>


#include "libsilex.h"

#include <string.h>

#if defined(MBEDTLS_SELF_TEST)
#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#warning("Warning using stdio and stdlib")
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_printf printf
#define mbedtls_calloc    calloc
#define mbedtls_free       free
#endif /* MBEDTLS_PLATFORM_C */
#endif /* MBEDTLS_SELF_TEST */

#if defined(MBEDTLS_SHA256_ALT)

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
do {                                                    \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
} while( 0 )
#endif

void mbedtls_sha256_init( mbedtls_sha256_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_free( mbedtls_sha256_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_clone( mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src )
{
    *dst = *src;
}

static const uint8_t esec_sha224_initial_state[32] =
      "\xc1\x05\x9e\xd8\x36\x7c\xd5\x07\x30\x70\xdd\x17\xf7\x0e\x59\x39"
      "\xff\xc0\x0b\x31\x68\x58\x15\x11\x64\xf9\x8f\xa7\xbe\xfa\x4f\xa4";

static const uint8_t esec_sha256_initial_state[32] =
      "\x6a\x09\xe6\x67\xbb\x67\xae\x85\x3c\x6e\xf3\x72\xa5\x4f\xf5\x3a"
      "\x51\x0e\x52\x7f\x9b\x05\x68\x8c\x1f\x83\xd9\xab\x5b\xe0\xcd\x19";

/*
 * SHA-256 context setup
 */
int mbedtls_sha256_starts_ret( mbedtls_sha256_context *ctx, int is224 )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    if( is224 == 0 )
    {
        /* SHA-256 */
        memcpy(ctx->state, esec_sha256_initial_state, sizeof(esec_sha256_initial_state));
    }
    else
    {
        /* SHA-224 */
        memcpy(ctx->state, esec_sha224_initial_state, sizeof(esec_sha224_initial_state));
    }

    ctx->is224 = is224;

    return 0;
}

#define SX_SHA256_MAX_CHUNK_SIZE  ((1 << 24) - 1)

int mbedtls_internal_sha256_process( mbedtls_sha256_context *ctx, const unsigned char data[64] )
{
    uint32_t ret;

    sx_enable_clock();
    ret = sx_hash_update_blk(ctx->is224 ? e_SHA224 : e_SHA256
        , block_t_convert(ctx->state, 32 )
        , block_t_convert(data, 64 ) );
    sx_disable_clock();
    return (ret) ? MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED : 0;
}

/*
 * SHA-256 process buffer
 */
int mbedtls_sha256_update_ret( mbedtls_sha256_context *ctx, const unsigned char *input,
                    size_t ilen )
{
    size_t fill;
    uint32_t left;
    sx_hash_fct_t hash_algo = ctx->is224 ? e_SHA224 : e_SHA256;

    if( ilen == 0 )
        return 0;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (uint32_t) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill )
    {
        memcpy( (void *) (ctx->buffer + left), input, fill );
        mbedtls_sha256_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    sx_enable_clock();

    while( ilen >= 64 )
    {
        size_t steplen = ilen;
        uint32_t ret;

        if (steplen > SX_SHA256_MAX_CHUNK_SIZE)
            steplen = SX_SHA256_MAX_CHUNK_SIZE;
        steplen &= ~0x3F;

        ret = sx_hash_update_blk(hash_algo, block_t_convert(ctx->state, 32)
            , block_t_convert(input, steplen) );

        input += steplen;
        ilen  -= steplen;
        if (ret)
        {
            sx_disable_clock();
            return MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED;
        }
    }

    sx_disable_clock();

    if( ilen > 0 )
        memcpy( (void *) (ctx->buffer + left), input, ilen );

    return 0;
}

static const unsigned char sha256_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-256 final digest
 */
int mbedtls_sha256_finish_ret( mbedtls_sha256_context *ctx, unsigned char output[32] )
{
    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];
    uint32_t ret;

    high = ( ctx->total[0] >> 29 )
         | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_UINT32_BE( high, msglen, 0 );
    PUT_UINT32_BE( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    ret = mbedtls_sha256_update_ret( ctx, sha256_padding, padn );
    ret |= mbedtls_sha256_update_ret( ctx, msglen, 8 );

    memcpy(output,
        ctx->state,
        ctx->is224 ? SHA224_DIGESTSIZE : SHA256_DIGESTSIZE);

    return (ret) ? MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED : 0;
}

#endif /* MBEDTLS_SHA256_ALT */

#endif /* MBEDTLS_SHA256_C */
