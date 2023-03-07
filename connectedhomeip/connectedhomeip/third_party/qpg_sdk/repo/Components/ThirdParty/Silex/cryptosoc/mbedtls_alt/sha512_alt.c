/*
 *  FIPS-180-2 compliant SHA-384/512 implementation
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
 *  The SHA-512 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SHA512_C)

#include <mbedtls/sha512.h>
#include "libsilex.h"
#include "sx_generic.h"
#if defined(_MSC_VER) || defined(__WATCOMC__)
  #define UL64(x) x##ui64
#else
  #define UL64(x) x##ULL
#endif

#include <string.h>

#if defined(MBEDTLS_SELF_TEST)

#include <stdio.h>
#include <stdlib.h>
#define mbedtls_printf printf
#define mbedtls_calloc    calloc
#define mbedtls_free       free
#endif /* MBEDTLS_SELF_TEST */

#if defined(MBEDTLS_SHA512_ALT)
/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

/*
 * 64-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT64_BE
#define GET_UINT64_BE(n,b,i)                            \
{                                                       \
    (n) = ( (uint64_t) (b)[(i)    ] << 56 )       \
        | ( (uint64_t) (b)[(i) + 1] << 48 )       \
        | ( (uint64_t) (b)[(i) + 2] << 40 )       \
        | ( (uint64_t) (b)[(i) + 3] << 32 )       \
        | ( (uint64_t) (b)[(i) + 4] << 24 )       \
        | ( (uint64_t) (b)[(i) + 5] << 16 )       \
        | ( (uint64_t) (b)[(i) + 6] <<  8 )       \
        | ( (uint64_t) (b)[(i) + 7]       );      \
}
#endif /* GET_UINT64_BE */

#ifndef PUT_UINT64_BE
#define PUT_UINT64_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 56 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 48 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 40 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 32 );       \
    (b)[(i) + 4] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 5] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 6] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 7] = (unsigned char) ( (n)       );       \
}
#endif /* PUT_UINT64_BE */

void mbedtls_sha512_init( mbedtls_sha512_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_sha512_context ) );
}

void mbedtls_sha512_free( mbedtls_sha512_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_sha512_context ) );
}

void mbedtls_sha512_clone( mbedtls_sha512_context *dst,
                           const mbedtls_sha512_context *src )
{
    *dst = *src;
}

static const uint8_t esec_sha384_initial_state[64] =
      "\xcb\xbb\x9d\x5d\xc1\x05\x9e\xd8\x62\x9a\x29\x2a\x36\x7c\xd5\x07"
      "\x91\x59\x01\x5a\x30\x70\xdd\x17\x15\x2f\xec\xd8\xf7\x0e\x59\x39"
      "\x67\x33\x26\x67\xff\xc0\x0b\x31\x8e\xb4\x4a\x87\x68\x58\x15\x11"
      "\xdb\x0c\x2e\x0d\x64\xf9\x8f\xa7\x47\xb5\x48\x1d\xbe\xfa\x4f\xa4";

static const uint8_t esec_sha512_initial_state[64] =
      "\x6a\x09\xe6\x67\xf3\xbc\xc9\x08\xbb\x67\xae\x85\x84\xca\xa7\x3b"
      "\x3c\x6e\xf3\x72\xfe\x94\xf8\x2b\xa5\x4f\xf5\x3a\x5f\x1d\x36\xf1"
      "\x51\x0e\x52\x7f\xad\xe6\x82\xd1\x9b\x05\x68\x8c\x2b\x3e\x6c\x1f"
      "\x1f\x83\xd9\xab\xfb\x41\xbd\x6b\x5b\xe0\xcd\x19\x13\x7e\x21\x79";

/*
 * SHA-512 context setup
 */
int mbedtls_sha512_starts_ret( mbedtls_sha512_context *ctx, int is384 )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    if( is384 == 0 )
    {
        /* SHA-512 */
        memcpy(ctx->state, esec_sha512_initial_state,
               sizeof(esec_sha512_initial_state));
    }
    else
    {
        /* SHA-384 */
        memcpy(ctx->state, esec_sha384_initial_state,
               sizeof(esec_sha384_initial_state));
    }

    ctx->is384 = is384;

    return 0;
}

#if !defined(MBEDTLS_SHA512_PROCESS_ALT)


int mbedtls_internal_sha512_process( mbedtls_sha512_context *ctx, const unsigned char data[128] )
{
    uint32_t ret;
    sx_enable_clock();
    ret = sx_hash_update_blk(ctx->is384 ? e_SHA384 : e_SHA512
            , block_t_convert(ctx->state, 64 )
            , block_t_convert(data, 128 )  ) ;
    sx_disable_clock();

    return (ret) ? MBEDTLS_ERR_SHA512_HW_ACCEL_FAILED : 0;
}
#endif /* !MBEDTLS_SHA512_PROCESS_ALT */

#define SX_SHA521_MAX_CHUNK_SIZE  ((1 << 24) - 1)


/*
 * SHA-512 process buffer
 */
int mbedtls_sha512_update_ret( mbedtls_sha512_context *ctx, const unsigned char *input,
                    size_t ilen )
{
    size_t fill;
    unsigned int left;
    sx_hash_fct_t hash_algo = ctx->is384 ? e_SHA384 : e_SHA512;

    if( ilen == 0 )
        return 0;

    left = (unsigned int) (ctx->total[0] & 0x7F);
    fill = 128 - left;

    ctx->total[0] += (uint64_t) ilen;

    if( ctx->total[0] < (uint64_t) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill )
    {
        memcpy( (void *) (ctx->buffer + left), input, fill );
        mbedtls_sha512_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    sx_enable_clock();
    while( ilen >= 128 )
    {
        size_t steplen = ilen;
        uint32_t ret;

        if (steplen > SX_SHA521_MAX_CHUNK_SIZE)
            steplen = SX_SHA521_MAX_CHUNK_SIZE;
        steplen &= ~0x7F;
        ret = sx_hash_update_blk(hash_algo, block_t_convert(ctx->state, 64)
            , block_t_convert(input, steplen) );
        if (ret)
            return MBEDTLS_ERR_SHA512_HW_ACCEL_FAILED;
        input += steplen;
        ilen  -= steplen;
    }
    sx_disable_clock();

    if( ilen > 0 )
        memcpy( (void *) (ctx->buffer + left), input, ilen );

    return 0;
}

static const unsigned char sha512_padding[128] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-512 final digest
 */
int mbedtls_sha512_finish_ret( mbedtls_sha512_context *ctx, unsigned char output[64] )
{
    size_t last, padn;
    uint64_t high, low;
    uint32_t ret;
    unsigned char msglen[16];

    high = ( ctx->total[0] >> 61 )
         | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_UINT64_BE( high, msglen, 0 );
    PUT_UINT64_BE( low,  msglen, 8 );

    last = (size_t)( ctx->total[0] & 0x7F );
    padn = ( last < 112 ) ? ( 112 - last ) : ( 240 - last );

    ret = mbedtls_sha512_update_ret( ctx, sha512_padding, padn );
    ret |= mbedtls_sha512_update_ret( ctx, msglen, 16 );

    memcpy(output , ctx->state , ctx->is384 ? SHA384_DIGESTSIZE : SHA512_DIGESTSIZE) ;

    return (ret) ? MBEDTLS_ERR_SHA512_HW_ACCEL_FAILED : 0;
}

#endif /* (MBEDTLS_SHA512_ALT) */
#endif /* MBEDTLS_SHA512_C */
