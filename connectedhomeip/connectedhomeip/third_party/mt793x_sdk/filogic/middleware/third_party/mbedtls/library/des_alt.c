/*
 *  FIPS-46-3 compliant Triple-DES implementation
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
 *  DES, on which TDES is based, was originally designed by Horst Feistel
 *  at IBM in 1974, and was adopted as a standard by NIST (formerly NBS).
 *
 *  http://csrc.nist.gov/publications/fips/fips46-3/fips46-3.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_DES_C)

#include "mbedtls/des.h"
#include <string.h>

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#define mbedtls_calloc calloc
#define mbedtls_free free
#endif /* MBEDTLS_PLATFORM_C */

#if defined(MBEDTLS_DES_ALT)

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
#include "top.h"
#endif

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = (unsigned char*)v; while( n-- ) *p++ = 0;
}

void mbedtls_des_init( mbedtls_des_context *ctx )
{
    memset( ctx, 0, sizeof(mbedtls_des_context) );
}

void mbedtls_des_free( mbedtls_des_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_des_context ) );
}

void mbedtls_des3_init( mbedtls_des3_context *ctx )
{
    memset( ctx, 0, sizeof(mbedtls_des3_context) );
}

void mbedtls_des3_free( mbedtls_des3_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_des3_context ) );
}

static const unsigned char odd_parity_table[128] = { 1,  2,  4,  7,  8,
        11, 13, 14, 16, 19, 21, 22, 25, 26, 28, 31, 32, 35, 37, 38, 41, 42, 44,
        47, 49, 50, 52, 55, 56, 59, 61, 62, 64, 67, 69, 70, 73, 74, 76, 79, 81,
        82, 84, 87, 88, 91, 93, 94, 97, 98, 100, 103, 104, 107, 109, 110, 112,
        115, 117, 118, 121, 122, 124, 127, 128, 131, 133, 134, 137, 138, 140,
        143, 145, 146, 148, 151, 152, 155, 157, 158, 161, 162, 164, 167, 168,
        171, 173, 174, 176, 179, 181, 182, 185, 186, 188, 191, 193, 194, 196,
        199, 200, 203, 205, 206, 208, 211, 213, 214, 217, 218, 220, 223, 224,
        227, 229, 230, 233, 234, 236, 239, 241, 242, 244, 247, 248, 251, 253,
        254 };

void mbedtls_des_key_set_parity( unsigned char key[MBEDTLS_DES_KEY_SIZE] )
{
    int i;

    for( i = 0; i < MBEDTLS_DES_KEY_SIZE; i++ )
        key[i] = odd_parity_table[key[i] / 2];
}


/*
 * Check the given key's parity, returns 1 on failure, 0 on SUCCESS
 */
int mbedtls_des_key_check_key_parity( const unsigned char key[MBEDTLS_DES_KEY_SIZE] )
{
    int i;

    for( i = 0; i < MBEDTLS_DES_KEY_SIZE; i++ )
        if( key[i] != odd_parity_table[key[i] / 2] )
            return( 1 );

    return( 0 );
}


/*
 * Table of weak and semi-weak keys
 *
 * Source: http://en.wikipedia.org/wiki/Weak_key
 *
 * Weak:
 * Alternating ones + zeros (0x0101010101010101)
 * Alternating 'F' + 'E' (0xFEFEFEFEFEFEFEFE)
 * '0xE0E0E0E0F1F1F1F1'
 * '0x1F1F1F1F0E0E0E0E'
 *
 * Semi-weak:
 * 0x011F011F010E010E and 0x1F011F010E010E01
 * 0x01E001E001F101F1 and 0xE001E001F101F101
 * 0x01FE01FE01FE01FE and 0xFE01FE01FE01FE01
 * 0x1FE01FE00EF10EF1 and 0xE01FE01FF10EF10E
 * 0x1FFE1FFE0EFE0EFE and 0xFE1FFE1FFE0EFE0E
 * 0xE0FEE0FEF1FEF1FE and 0xFEE0FEE0FEF1FEF1
 *
 */

#define WEAK_KEY_COUNT 16

static const unsigned char weak_key_table[WEAK_KEY_COUNT][MBEDTLS_DES_KEY_SIZE] =
{
    { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
    { 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE },
    { 0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x0E, 0x0E, 0x0E },
    { 0xE0, 0xE0, 0xE0, 0xE0, 0xF1, 0xF1, 0xF1, 0xF1 },

    { 0x01, 0x1F, 0x01, 0x1F, 0x01, 0x0E, 0x01, 0x0E },
    { 0x1F, 0x01, 0x1F, 0x01, 0x0E, 0x01, 0x0E, 0x01 },
    { 0x01, 0xE0, 0x01, 0xE0, 0x01, 0xF1, 0x01, 0xF1 },
    { 0xE0, 0x01, 0xE0, 0x01, 0xF1, 0x01, 0xF1, 0x01 },
    { 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE },
    { 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01 },
    { 0x1F, 0xE0, 0x1F, 0xE0, 0x0E, 0xF1, 0x0E, 0xF1 },
    { 0xE0, 0x1F, 0xE0, 0x1F, 0xF1, 0x0E, 0xF1, 0x0E },
    { 0x1F, 0xFE, 0x1F, 0xFE, 0x0E, 0xFE, 0x0E, 0xFE },
    { 0xFE, 0x1F, 0xFE, 0x1F, 0xFE, 0x0E, 0xFE, 0x0E },
    { 0xE0, 0xFE, 0xE0, 0xFE, 0xF1, 0xFE, 0xF1, 0xFE },
    { 0xFE, 0xE0, 0xFE, 0xE0, 0xFE, 0xF1, 0xFE, 0xF1 }
};

int mbedtls_des_key_check_weak( const unsigned char key[MBEDTLS_DES_KEY_SIZE] )
{
    int i;

    for( i = 0; i < WEAK_KEY_COUNT; i++ )
        if( memcmp( weak_key_table[i], key, MBEDTLS_DES_KEY_SIZE) == 0 )
            return( 1 );

    return( 0 );
}

void mbedtls_des_setkey( uint32_t SK[32], const unsigned char key[MBEDTLS_DES_KEY_SIZE] )
{
    mbedtls_printf("mbedtls_des_setkey() doing nothing.");
}

/*
 * DES key schedule (56-bit, encryption)
 */
int mbedtls_des_setkey_enc( mbedtls_des_context *ctx, const unsigned char key[MBEDTLS_DES_KEY_SIZE] )
{
    memcpy( ctx->key, key, MBEDTLS_DES_KEY_SIZE );
    ctx->mode = 1;
    return ( 0 );
}

/*
 * DES key schedule (56-bit, decryption)
 */
int mbedtls_des_setkey_dec( mbedtls_des_context *ctx, const unsigned char key[MBEDTLS_DES_KEY_SIZE] )
{
    memcpy( ctx->key, key, MBEDTLS_DES_KEY_SIZE );
    ctx->mode = 0;
    return ( 0 );
}

/*
 * Triple-DES key schedule (112-bit, encryption)
 */
int mbedtls_des3_set2key_enc( mbedtls_des3_context *ctx,
                      const unsigned char key[MBEDTLS_DES_KEY_SIZE * 2] )
{
    ctx->key_byte_length = MBEDTLS_DES_KEY_SIZE * 2;
    memcpy( ctx->key, key, ctx->key_byte_length );
    ctx->mode = 1;
    return ( 0 );
}

/*
 * Triple-DES key schedule (112-bit, decryption)
 */
int mbedtls_des3_set2key_dec( mbedtls_des3_context *ctx,
                      const unsigned char key[MBEDTLS_DES_KEY_SIZE * 2] )
{
    ctx->key_byte_length = MBEDTLS_DES_KEY_SIZE * 2;
    memcpy( ctx->key, key, ctx->key_byte_length );
    ctx->mode = 0;
    return ( 0 );
}

/*
 * Triple-DES key schedule (168-bit, encryption)
 */
int mbedtls_des3_set3key_enc( mbedtls_des3_context *ctx,
                      const unsigned char key[MBEDTLS_DES_KEY_SIZE * 3] )
{
    ctx->key_byte_length = MBEDTLS_DES_KEY_SIZE * 3;
    memcpy( ctx->key, key, ctx->key_byte_length );
    ctx->mode = 1;
    return ( 0 );
}

/*
 * Triple-DES key schedule (168-bit, decryption)
 */
int mbedtls_des3_set3key_dec( mbedtls_des3_context *ctx,
                      const unsigned char key[MBEDTLS_DES_KEY_SIZE * 3] )
{
    ctx->key_byte_length = MBEDTLS_DES_KEY_SIZE * 3;
    memcpy( ctx->key, key, ctx->key_byte_length );
    ctx->mode = 0;
    return ( 0 );
}

/*
 * DES-ECB block encryption/decryption
 */
int mbedtls_des_crypt_ecb( mbedtls_des_context *ctx,
                    const unsigned char input[8],
                    unsigned char output[8] )
{
    int ret;
    hal_des_buffer_t output_buf, input_buf, key_buf;
    unsigned char output_temp[24];

    output_buf.buffer = (uint8_t*)output_temp;
    output_buf.length = 16;
    input_buf.buffer = (uint8_t*)input;
    input_buf.length = 8;
    key_buf.buffer = (uint8_t*)ctx->key;
    key_buf.length = MBEDTLS_DES_KEY_SIZE;

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo96M();
#endif

    if ( ctx->mode == 0 ) /* decrypt */
    {
        do {
            ret = (int)hal_des_ecb_decrypt( &output_buf, &input_buf, &key_buf );
            if ( ret == -100 )
                hal_gpt_delay_ms(1);
        } while( ret == -100 );
    }
    else
    {
        do {
            ret = (int)hal_des_ecb_encrypt( &output_buf, &input_buf, &key_buf );
            if ( ret == -100 )
                hal_gpt_delay_ms(1);
        } while( ret == -100 );
    }

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo192M();
#endif

    memcpy(output, output_temp, 8);

    return HAL_DES_STATUS_OK == ret ? 0 : -1;
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * DES-CBC buffer encryption/decryption
 */
int mbedtls_des_crypt_cbc( mbedtls_des_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[8],
                    const unsigned char *input,
                    unsigned char *output )
{
    int ret;
    unsigned char iv_temp[8];
    unsigned char* output_temp;
    hal_des_buffer_t input_buf, output_buf, key_buf;

    if( length % 8 )
        return( MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH );

    output_temp = (unsigned char*)mbedtls_calloc( 1, length + 16 );
    if( output_temp == NULL )
        return( MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH );

    output_buf.buffer = (uint8_t*)output_temp;
    output_buf.length = (uint32_t)length + 8;
    input_buf.buffer = (uint8_t*)input;
    input_buf.length = (uint32_t)length;
    key_buf.buffer = (uint8_t*)ctx->key;
    key_buf.length = MBEDTLS_DES_KEY_SIZE;

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo96M();
#endif

    if( mode == 0 ) /* decrypt */
    {
        memcpy( iv_temp, input + length - 8, 8 );
        do {
            ret = (int)hal_des_cbc_decrypt( &output_buf, &input_buf, &key_buf, (uint8_t*)iv );
            if( ret == -100 )
                hal_gpt_delay_ms(1);
        } while( ret == -100 );
        memcpy( iv, iv_temp, 8 );
    }
    else
    {
        do {
            ret = (int)hal_des_cbc_encrypt( &output_buf, &input_buf, &key_buf, (uint8_t*)iv );
            if (ret == -100 )
                hal_gpt_delay_ms(1);
        } while( ret == -100 );
        memcpy( iv, output_temp + length - 8, 8 );
    }
    
#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo192M();
#endif
    
    memcpy( output, output_temp, length );
    mbedtls_free( output_temp );
    
    return HAL_DES_STATUS_OK == ret ? 0 : -1;
}

#endif /* MBEDTLS_CIPHER_MODE_CBC */

/*
 * 3DES-ECB block encryption/decryption
 */
int mbedtls_des3_crypt_ecb( mbedtls_des3_context *ctx,
                     const unsigned char input[8],
                     unsigned char output[8] )
{
    int ret;
    hal_des_buffer_t output_buf, input_buf, key_buf;
    unsigned char output_temp[24];


    output_buf.buffer = (uint8_t*)output_temp;
    output_buf.length = 16;
    input_buf.buffer = (uint8_t*)input;
    input_buf.length = 8;
    key_buf.buffer = (uint8_t*)ctx->key;
    key_buf.length = ctx->key_byte_length;

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo96M();
#endif

    if( ctx->mode == 0) /* decrypt */
    {
        do {
            ret = (int)hal_des_ecb_decrypt( &output_buf, &input_buf, &key_buf );
            if (ret == -100 )
                hal_gpt_delay_ms(1);
        } while( ret == -100 );
    }
    else
    {
        do {
            ret = (int)hal_des_ecb_encrypt( &output_buf, &input_buf, &key_buf );
            if (ret == -100 )
                hal_gpt_delay_ms(1);
        } while( ret == -100 );
    }

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo192M();
#endif

    memcpy(output, output_temp, 8);

    return HAL_DES_STATUS_OK == ret ? 0 : -1;
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * 3DES-CBC buffer encryption/decryption
 */
int mbedtls_des3_crypt_cbc( mbedtls_des3_context *ctx,
                     int mode,
                     size_t length,
                     unsigned char iv[8],
                     const unsigned char *input,
                     unsigned char *output )
{
    int ret;
    unsigned char iv_temp[8];
    unsigned char* output_temp;
    hal_des_buffer_t input_buf, output_buf, key_buf;

    if( length % 8 )
        return( MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH );

    output_temp = (unsigned char*)mbedtls_calloc( 1, length + 16 );
    if( output_temp == NULL )
        return( MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH );

    output_buf.buffer = (uint8_t*)output_temp;
    output_buf.length = (uint32_t)length + 8;
    input_buf.buffer = (uint8_t*)input;
    input_buf.length = (uint32_t)length;
    key_buf.buffer = (uint8_t*)ctx->key;
    key_buf.length = ctx->key_byte_length;

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo96M();
#endif

    if( mode == 0 ) /* decrypt */
    {
        memcpy( iv_temp, input + length - 8, 8 );
        do {
            ret = (int)hal_des_cbc_decrypt( &output_buf, &input_buf, &key_buf, (uint8_t*)iv );
            if( ret == -100 )
                hal_gpt_delay_ms(1);
        } while( ret == -100 );
        memcpy( iv, iv_temp, 8 );
    }
    else
    {
        do {
            ret = (int)hal_des_cbc_encrypt( &output_buf, &input_buf, &key_buf, (uint8_t*)iv );
            if( ret == -100 )
                hal_gpt_delay_ms(1);
        } while( ret == -100 );
        memcpy( iv, output_temp + length - 8, 8 );
    }
    
#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo192M();
#endif

    memcpy( output, output_temp, length );
    mbedtls_free( output_temp );
    return HAL_DES_STATUS_OK == ret ? 0 : -1;
}

#endif /* MBEDTLS_CIPHER_MODE_CBC */

#endif /* MBEDTLS_DES_ALT */

#endif /* MBEDTLS_DES_C */

