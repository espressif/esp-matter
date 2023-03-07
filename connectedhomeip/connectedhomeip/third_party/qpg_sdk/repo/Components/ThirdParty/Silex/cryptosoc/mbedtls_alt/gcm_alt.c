/*
 *  NIST SP800-38D compliant GCM implementation
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
 * http://csrc.nist.gov/publications/nistpubs/800-38D/SP-800-38D.pdf
 *
 * See also:
 * [MGV] http://csrc.nist.gov/groups/ST/toolkit/BCM/documents/proposedmodes/gcm/gcm-revised-spec.pdf
 *
 * We use the algorithm described as Shoup's method with 4-bit tables in
 * [MGV] 4.1, pp. 12-13, to enhance speed without using too much memory.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_GCM_C)

#include "mbedtls/gcm.h"
#include <sx_aes.h>
#include <cryptolib_def.h>
#include <string.h>

#if defined(MBEDTLS_AESNI_C)
#include "mbedtls/aesni.h"
#endif

#if defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_AES_C)
#include "mbedtls/aes.h"
#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#endif /* MBEDTLS_PLATFORM_C */
#endif /* MBEDTLS_SELF_TEST && MBEDTLS_AES_C */

#if defined(MBEDTLS_GCM_ALT)

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

/*
 * Initialize a context
 */
void mbedtls_gcm_init( mbedtls_gcm_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_gcm_context ) );
}

/*
 * Precompute small multiples of H, that is set
 *      HH[i] || HL[i] = H times i,
 * where i is seen as a field element as in [MGV], ie high-order bits
 * correspond to low powers of P. The result is stored in the same way, that
 * is the high-order bit of HH corresponds to P^0 and the low-order bit of HL
 * corresponds to P^127.
 */
static int gcm_gen_table( mbedtls_gcm_context *ctx )
{
    int ret, i, j;
    uint64_t hi, lo;
    uint64_t vl, vh;
    unsigned char h[16];
    size_t olen = 0;

    memset( h, 0, 16 );
    if( ( ret = mbedtls_cipher_update( &ctx->cipher_ctx, h, 16, h, &olen ) ) != 0 )
    return( ret );

    /* pack h as two 64-bits ints, big-endian */
    GET_UINT32_BE( hi, h, 0 );
    GET_UINT32_BE( lo, h, 4 );
    vh = (uint64_t) hi << 32 | lo;

    GET_UINT32_BE( hi, h, 8 );
    GET_UINT32_BE( lo, h, 12 );
    vl = (uint64_t) hi << 32 | lo;

    /* 8 = 1000 corresponds to 1 in GF(2^128) */
    ctx->HL[8] = vl;
    ctx->HH[8] = vh;

#if defined(MBEDTLS_AESNI_C) && defined(MBEDTLS_HAVE_X86_64)
    /* With CLMUL support, we need only h, not the rest of the table */
    if( mbedtls_aesni_has_support( MBEDTLS_AESNI_CLMUL ) )
    return( 0 );
#endif

    /* 0 corresponds to 0 in GF(2^128) */
    ctx->HH[0] = 0;
    ctx->HL[0] = 0;

    for( i = 4; i > 0; i >>= 1 )
    {
        uint32_t T = ( vl & 1 ) * 0xe1000000U;
        vl = ( vh << 63 ) | ( vl >> 1 );
        vh = ( vh >> 1 ) ^ ( (uint64_t) T << 32);

        ctx->HL[i] = vl;
        ctx->HH[i] = vh;
    }

    for( i = 2; i <= 8; i *= 2 )
    {
        uint64_t *HiL = ctx->HL + i, *HiH = ctx->HH + i;
        vh = *HiH;
        vl = *HiL;
        for( j = 1; j < i; j++ )
        {
            HiH[j] = vh ^ ctx->HH[j];
            HiL[j] = vl ^ ctx->HL[j];
        }
    }

    return( 0 );
}

int mbedtls_gcm_setkey( mbedtls_gcm_context *ctx,
        mbedtls_cipher_id_t cipher,
        const unsigned char *key,
        unsigned int keybits )
{
#if 1
    int ret;
    const mbedtls_cipher_info_t *cipher_info;

    cipher_info = mbedtls_cipher_info_from_values( cipher, keybits, MBEDTLS_MODE_ECB );
    if( cipher_info == NULL )
    return( MBEDTLS_ERR_GCM_BAD_INPUT );

    if( cipher_info->block_size != 16 )
    return( MBEDTLS_ERR_GCM_BAD_INPUT );

    mbedtls_cipher_free( &ctx->cipher_ctx );

    if( ( ret = mbedtls_cipher_setup( &ctx->cipher_ctx, cipher_info ) ) != 0 )
    return( ret );

    if( ( ret = mbedtls_cipher_setkey( &ctx->cipher_ctx, key, keybits,
                            MBEDTLS_ENCRYPT ) ) != 0 )
    {
        return( ret );
    }

    if( ( ret = gcm_gen_table( ctx ) ) != 0 )
    return( ret );

    memcpy(ctx->key , key , keybits / 8);
    ctx->keysize = keybits/8;
    return 0;
#endif
}

static const uint64_t last4[16] =
{
    0x0000, 0x1c20, 0x3840, 0x2460,
    0x7080, 0x6ca0, 0x48c0, 0x54e0,
    0xe100, 0xfd20, 0xd940, 0xc560,
    0x9180, 0x8da0, 0xa9c0, 0xb5e0
};

/*
 * Sets output to x times H using the precomputed tables.
 * x and output are seen as elements of GF(2^128) as in [MGV].
 */
static void gcm_mult( mbedtls_gcm_context *ctx, const unsigned char x[16],
        unsigned char output[16] )
{
    int i = 0;
    unsigned char lo, hi, rem;
    uint64_t zh, zl;

#if defined(MBEDTLS_AESNI_C) && defined(MBEDTLS_HAVE_X86_64)
    if( mbedtls_aesni_has_support( MBEDTLS_AESNI_CLMUL ) ) {
        unsigned char h[16];

        PUT_UINT32_BE( ctx->HH[8] >> 32, h, 0 );
        PUT_UINT32_BE( ctx->HH[8], h, 4 );
        PUT_UINT32_BE( ctx->HL[8] >> 32, h, 8 );
        PUT_UINT32_BE( ctx->HL[8], h, 12 );

        mbedtls_aesni_gcm_mult( output, x, h );
        return;
    }
#endif /* MBEDTLS_AESNI_C && MBEDTLS_HAVE_X86_64 */

    lo = x[15] & 0xf;

    zh = ctx->HH[lo];
    zl = ctx->HL[lo];

    for( i = 15; i >= 0; i-- )
    {
        lo = x[i] & 0xf;
        hi = x[i] >> 4;

        if( i != 15 )
        {
            rem = (unsigned char) zl & 0xf;
            zl = ( zh << 60 ) | ( zl >> 4 );
            zh = ( zh >> 4 );
            zh ^= (uint64_t) last4[rem] << 48;
            zh ^= ctx->HH[lo];
            zl ^= ctx->HL[lo];

        }

        rem = (unsigned char) zl & 0xf;
        zl = ( zh << 60 ) | ( zl >> 4 );
        zh = ( zh >> 4 );
        zh ^= (uint64_t) last4[rem] << 48;
        zh ^= ctx->HH[hi];
        zl ^= ctx->HL[hi];
    }

    PUT_UINT32_BE( zh >> 32, output, 0 );
    PUT_UINT32_BE( zh, output, 4 );
    PUT_UINT32_BE( zl >> 32, output, 8 );
    PUT_UINT32_BE( zl, output, 12 );
}

int mbedtls_gcm_starts( mbedtls_gcm_context *ctx,
        int mode,
        const unsigned char *iv,
        size_t iv_len,
        const unsigned char *add,
        size_t add_len )
{

    if (iv_len != 12 || add_len == 0 ) {
        int ret;
        unsigned char work_buf[16];
        size_t i;
        const unsigned char *p;
        size_t use_len, olen = 0;

        /* IV and AD are limited to 2^64 bits, so 2^61 bytes */
        /* IV is not allowed to be zero length */
        if( iv_len == 0 ||
                ( (uint64_t) iv_len ) >> 61 != 0 ||
                ( (uint64_t) add_len ) >> 61 != 0 )
        {
            return( MBEDTLS_ERR_GCM_BAD_INPUT );
        }

        memset( ctx->y, 0x00, sizeof(ctx->y) );
        memset( ctx->buf, 0x00, sizeof(ctx->buf) );

        ctx->mode = mode;
        ctx->len = 0;
        ctx->add_len = 0;

        ctx->iv_len = iv_len;

        if( iv_len == 12 )
        {
            memcpy( ctx->y, iv, iv_len );
            ctx->y[15] = 1;
        }
        else
        {
            memset( work_buf, 0x00, 16 );
            PUT_UINT32_BE( iv_len * 8, work_buf, 12 );

            p = iv;
            while( iv_len > 0 )
            {
                use_len = ( iv_len < 16 ) ? iv_len : 16;

                for( i = 0; i < use_len; i++ )
                ctx->y[i] ^= p[i];

                gcm_mult( ctx, ctx->y, ctx->y );

                iv_len -= use_len;
                p += use_len;
            }

            for( i = 0; i < 16; i++ )
            ctx->y[i] ^= work_buf[i];

            gcm_mult( ctx, ctx->y, ctx->y );
        }

        if( ( ret = mbedtls_cipher_update( &ctx->cipher_ctx, ctx->y, 16, ctx->base_ectr,
                                &olen ) ) != 0 )
        {
            return( ret );
        }

        ctx->add_len = add_len;
        p = add;
        while( add_len > 0 )
        {
            use_len = ( add_len < 16 ) ? add_len : 16;

            for( i = 0; i < use_len; i++ )
            ctx->buf[i] ^= p[i];

            gcm_mult( ctx, ctx->buf, ctx->buf );

            add_len -= use_len;
            p += use_len;
        }
        memcpy(ctx->iv , ctx->y , 12);

        memcpy(ctx->ctxBuff ,ctx->y , 16);
        memcpy(ctx->ctxBuff + 16 ,ctx->buf , 16);
        return( 0 );
    } else {

        unsigned char input[1] , output[256];
        uint32_t ret;
        sx_enable_clock();
        ret = sx_aes_blk(
                GCM,
                mode == MBEDTLS_GCM_ENCRYPT ? ENC : DEC ,
                CTX_BEGIN,
                block_t_convert(ctx->key , ctx->keysize),
                NULL_blk, // no xts key
                block_t_convert(iv , iv_len),
                block_t_convert(input, 0),
                block_t_convert(output, 0),
                block_t_convert(add, add_len),
                NULL_blk,
                block_t_convert(ctx->ctxBuff , 32),// no ctx_ptr
                NULL_blk// no nonce_len
        );
        sx_disable_clock();
        memcpy(ctx->y , ctx->ctxBuff , 16);
        memcpy(ctx->buf , ctx->ctxBuff+16 , 16);

        ctx->add_len = add_len;
        ctx->len = 0;
        ctx->iv_len = iv_len;
        ctx->mode = mode;
        return ret;

    }
}

int mbedtls_gcm_update( mbedtls_gcm_context *ctx,
        size_t length,
        const unsigned char *input,
        unsigned char *output )
{
    if(ctx->add_len == 0 || length == 0 ) {
        int ret;
        unsigned char ectr[16];
        size_t i;
        const unsigned char *p;
        unsigned char *out_p = output;
        size_t use_len, olen = 0;

        if( output > input && (size_t) ( output - input ) < length )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );

        /* Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes
         * Also check for possible overflow */
        if( ctx->len + length < ctx->len ||
                (uint64_t) ctx->len + length > 0xFFFFFFFE0ull )
        {
            return( MBEDTLS_ERR_GCM_BAD_INPUT );
        }

        ctx->len += length;

        p = input;
        while( length > 0 )
        {
            use_len = ( length < 16 ) ? length : 16;

            for( i = 16; i > 12; i-- )
            if( ++ctx->y[i - 1] != 0 )
            break;

            if( ( ret = mbedtls_cipher_update( &ctx->cipher_ctx, ctx->y, 16, ectr,
                                    &olen ) ) != 0 )
            {
                return( ret );
            }

            for( i = 0; i < use_len; i++ )
            {
                if( ctx->mode == MBEDTLS_GCM_DECRYPT )
                ctx->buf[i] ^= p[i];
                out_p[i] = ectr[i] ^ p[i];
                if( ctx->mode == MBEDTLS_GCM_ENCRYPT )
                ctx->buf[i] ^= out_p[i];
            }

            gcm_mult( ctx, ctx->buf, ctx->buf );

            length -= use_len;
            p += use_len;
            out_p += use_len;
        }

        return( 0 );
    }
    else {
        if (ctx->len==0 && (ctx->iv_len != 12 || ctx->add_len ==0) ) {
            for(int i = 16; i > 12; i-- ) {
                if( ++ctx->y[i - 1] != 0 )
                break;
            }
            memcpy(ctx->ctxBuff ,ctx->y , 16);
            memcpy(ctx->ctxBuff + 16 ,ctx->buf , 16);
        }
        uint32_t ret;
        ctx->len += length;

        if (length % 16 == 0 )
        {
            sx_enable_clock();
            ret = sx_aes_blk(
                    GCM,
                    ctx->mode == MBEDTLS_GCM_ENCRYPT ? ENC : DEC ,
                    CTX_MIDDLE,
                    block_t_convert(ctx->key , ctx->keysize),
                    NULL_blk, // no xts key
                    block_t_convert(ctx->ctxBuff , 32 ),
                    block_t_convert(input, length),
                    block_t_convert(output, length),
                    NULL_blk,
                    NULL_blk,
                    block_t_convert(ctx->ctxBuff , 32 ),// no ctx_ptr
                    NULL_blk// no nonce_len
            );
            sx_disable_clock();
            memcpy(ctx->buf , ctx->ctxBuff+16 ,16);
        } else {
            ctx->hasInput = 1;
            ctx->input = input;
            ctx->inputSize = length;
            ctx->output = output;
            ret = 0;
        }

        return ret;
    }

}

int mbedtls_gcm_finish( mbedtls_gcm_context *ctx,
        unsigned char *tag,
        size_t tag_len )
{

    if(ctx->add_len == 0 ) {

        unsigned char work_buf[16];
        size_t i;
        uint64_t orig_len = ctx->len * 8;
        uint64_t orig_add_len = ctx->add_len * 8;

        if( tag_len > 16 || tag_len < 4 )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );

        memcpy( tag, ctx->base_ectr, tag_len );

        if( orig_len || orig_add_len )
        {
            memset( work_buf, 0x00, 16 );

            PUT_UINT32_BE( ( orig_add_len >> 32 ), work_buf, 0 );
            PUT_UINT32_BE( ( orig_add_len ), work_buf, 4 );
            PUT_UINT32_BE( ( orig_len >> 32 ), work_buf, 8 );
            PUT_UINT32_BE( ( orig_len ), work_buf, 12 );

            for( i = 0; i < 16; i++ )
            ctx->buf[i] ^= work_buf[i];

            gcm_mult( ctx, ctx->buf, ctx->buf );

            for( i = 0; i < tag_len; i++ )
            tag[i] ^= ctx->buf[i];
        }

        return( 0 );
    }
    else {
        uint32_t ret;

        unsigned char work_buf[16];
        uint64_t orig_len = ctx->len * 8;
        uint64_t orig_add_len = ctx->add_len * 8;

        if( tag_len > 16 || tag_len < 4 )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );

        memset( work_buf, 0x00, 16 );
        if( orig_len || orig_add_len )
        {

            PUT_UINT32_BE( ( orig_add_len >> 32 ), work_buf, 0 );
            PUT_UINT32_BE( ( orig_add_len ), work_buf, 4 );
            PUT_UINT32_BE( ( orig_len >> 32 ), work_buf, 8 );
            PUT_UINT32_BE( ( orig_len ), work_buf, 12 );
        }

        const unsigned char * input;
        unsigned char * output;
        if (ctx->hasInput == 1 ) {
            input = ctx->input;
            output = ctx->output;
        }
        uint8_t tagout[16] = {0} ;
        sx_enable_clock();
        ret = sx_aes_blk(
                GCM,
                ctx->mode == MBEDTLS_GCM_ENCRYPT ? ENC : DEC ,
                CTX_END,
                block_t_convert(ctx->key , ctx->keysize),
                NULL_blk, // no xts key
                block_t_convert(ctx->ctxBuff , 32 ),
                block_t_convert(input,ctx->hasInput? ctx->inputSize : 0),
                block_t_convert(output,ctx->hasInput? ctx->inputSize : 0),
                NULL_blk,
                block_t_convert(tagout, 16),
                NULL_blk,// no ctx_ptr
                block_t_convert(work_buf, 16)
        );
        sx_disable_clock();
        ctx->hasInput = 0;
        ctx->inputSize = 0;

        if (ret == CRYPTOLIB_INVALID_SIGN_ERR) {
            ret = 0;
        }

        if (ctx->iv_len != 12 ) {

            unsigned char ctr0[16] = {0}, ctr0Enc[16];
            size_t olen;
            memcpy(ctr0 , &ctx->iv , 12);
            ctr0[15] = 0x01;
            ret = mbedtls_cipher_update( &ctx->cipher_ctx, ctr0 , 16, ctr0Enc,
                    &olen );

            for(int i =0; i < 16; i++) {
                tagout[i] ^= ctr0Enc[i];
            }
            for(int i =0; i < 16; i++) {
                tagout[i] ^= ctx->base_ectr[i];
            }
        }
        memcpy(tag , tagout , tag_len);
        return( ret );
    }
}

int mbedtls_gcm_crypt_and_tag( mbedtls_gcm_context *ctx,
        int mode,
        size_t length,
        const unsigned char *iv,
        size_t iv_len,
        const unsigned char *add,
        size_t add_len,
        const unsigned char *input,
        unsigned char *output,
        size_t tag_len,
        unsigned char *tag )
{
    int ret;
    if (iv_len != 12) {

        if( ( ret = mbedtls_gcm_starts( ctx, mode, iv, iv_len, add, add_len ) ) != 0 )
        return( ret );

        if( ( ret = mbedtls_gcm_update( ctx, length, input, output ) ) != 0 )
        return( ret );

        if( ( ret = mbedtls_gcm_finish( ctx, tag, tag_len ) ) != 0 )
        return( ret );

        return( 0 );
    }
    else {
        sx_enable_clock();
        ret = sx_aes_blk(
                GCM,
                mode == MBEDTLS_GCM_ENCRYPT ? ENC : DEC ,
                CTX_WHOLE,
                block_t_convert(ctx->key , ctx->keysize),
                NULL_blk, // no xts key
                block_t_convert(iv , iv_len),
                block_t_convert(input, length),
                block_t_convert(output, length),
                block_t_convert(add,add_len),// no AAD
                block_t_convert(tag, tag_len),
                NULL_blk,// no ctx_ptr
                NULL_blk// no nonce_len
        );
        sx_disable_clock();
        if (ret == CRYPTOLIB_INVALID_SIGN_ERR) {
            ret = MBEDTLS_ERR_CIPHER_AUTH_FAILED;
        }
        return ret;
    }

}

int mbedtls_gcm_auth_decrypt( mbedtls_gcm_context *ctx,
        size_t length,
        const unsigned char *iv,
        size_t iv_len,
        const unsigned char *add,
        size_t add_len,
        const unsigned char *tag,
        size_t tag_len,
        const unsigned char *input,
        unsigned char *output )
{
    int ret;
    unsigned char check_tag[16];
    size_t i;
    int diff;

    if( ( ret = mbedtls_gcm_crypt_and_tag( ctx, MBEDTLS_GCM_DECRYPT, length,
                            iv, iv_len, add, add_len,
                            input, output, tag_len, check_tag ) ) != 0 )
    {
        return( ret );
    }

    /* Check tag in "constant-time" */
    for( diff = 0, i = 0; i < tag_len; i++ )
    diff |= tag[i] ^ check_tag[i];

    if( diff != 0 )
    {
        mbedtls_zeroize( output, length );
        return( MBEDTLS_ERR_GCM_AUTH_FAILED );
    }

    return( 0 );
}

void mbedtls_gcm_free( mbedtls_gcm_context *ctx )
{
    mbedtls_cipher_free( &ctx->cipher_ctx );
    mbedtls_zeroize( ctx, sizeof( mbedtls_gcm_context ) );
}

#endif /* !MBEDTLS_GCM_ALT */

#endif /* MBEDTLS_GCM_C */

