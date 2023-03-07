/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_AES_C)

#include <string.h>
#include "mbedtls/aes.h"

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#define mbedtls_calloc calloc
#define mbedtls_free free
#endif /* MBEDTLS_PLATFORM_C */

#ifdef MBEDTLS_AES_ALT

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
#include "top.h"
#endif


/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = (unsigned char*)v; while( n-- ) *p++ = 0;
}

void mbedtls_aes_init( mbedtls_aes_context *ctx )
{
    memset( ctx, 0, sizeof(mbedtls_aes_context) );
}

void mbedtls_aes_free( mbedtls_aes_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_aes_context ) );
}

int mbedtls_aes_setkey_enc( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    switch( keybits )
    {
        case 128: ctx->key_byte_length = 16; break;
        case 192: ctx->key_byte_length = 24; break;
        case 256: ctx->key_byte_length = 32; break;
        default : return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }
    memcpy( ctx->key, key, ctx->key_byte_length );
    return( 0 );
}

int mbedtls_aes_setkey_dec( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    return mbedtls_aes_setkey_enc( ctx, key, keybits );
}


/*
 * AES-ECB block encryption
 */
int mbedtls_internal_aes_encrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    return mbedtls_aes_crypt_ecb(ctx,
                                 MBEDTLS_AES_ENCRYPT,
                                 input,
                                 output);
}


#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_aes_encrypt( mbedtls_aes_context *ctx,
                          const unsigned char input[16],
                          unsigned char output[16] )
{
    mbedtls_internal_aes_encrypt( ctx, input, output );
}
#endif /* !MBEDTLS_DEPRECATED_REMOVED */


/*
 * AES-ECB block decryption
 */
int mbedtls_internal_aes_decrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    return mbedtls_aes_crypt_ecb(ctx,
                                 MBEDTLS_AES_DECRYPT,
                                 input,
                                 output);
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_aes_decrypt( mbedtls_aes_context *ctx,
                          const unsigned char input[16],
                          unsigned char output[16] )
{
    mbedtls_internal_aes_decrypt( ctx, input, output );
}
#endif /* !MBEDTLS_DEPRECATED_REMOVED */

int mbedtls_aes_crypt_ecb( mbedtls_aes_context *ctx,
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] )
{
    int ret;
    hal_aes_buffer_t output_buf, input_buf, key_buf;
    unsigned char output_temp[48]={0};

    output_buf.buffer = (uint8_t*)output_temp;
    output_buf.length = 32;
    input_buf.buffer = (uint8_t*)input;
    input_buf.length = 16;
    key_buf.buffer = (uint8_t*)ctx->key;
    key_buf.length = (uint32_t)ctx->key_byte_length;

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo96M();
#endif

    if ( mode == 0 ) /* decrypt */
    {
        do {
    	    ret = (int)hal_aes_ecb_decrypt( &output_buf, &input_buf, &key_buf );
    	    if (ret == -100){
     	        hal_gpt_delay_ms(1);
    	    }
    	} while(ret == -100);
    }
    else
    {
        do {
            ret = (int)hal_aes_ecb_encrypt( &output_buf, &input_buf, &key_buf );
            if (ret == -100){
                hal_gpt_delay_ms(1);
            }
        } while(ret == -100);
    }
    memcpy( output, output_temp, 16);

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo192M();
#endif
    return HAL_AES_STATUS_OK == ret ? 0 : -1;
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)

#ifndef MTK_BOOTLOADER_USE_MBEDTLS
/* It's more efficient to call hal algorithm only once. So keep this implementation. */
int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output )
{
    int ret;
    unsigned char iv_temp[16];
    unsigned char *input_temp, *output_temp;
    hal_aes_buffer_t input_buf, output_buf, key_buf;

    if( length % 16 )
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );

    output_temp = (unsigned char *)mbedtls_calloc( 1, length + 32 );
    if( output_temp == NULL )
    {
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }
    input_temp = (unsigned char *)mbedtls_calloc(1, length );
    if( input_temp == NULL )
    {
        mbedtls_free(output_temp);
    	return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    }

    memset(output_temp, 0, length+32);
    memcpy(input_temp, input, length);

    output_buf.buffer = output_temp;
    output_buf.length = length + 32;
    input_buf.buffer = input_temp;
    input_buf.length = length;
    key_buf.buffer = ctx->key;
    key_buf.length = ctx->key_byte_length;
    
#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo96M();
#endif

    if ( mode == 0 ) /* decrypt */
    {
        memcpy( iv_temp, input + length - 16, 16 );
        do {
            ret = (int)hal_aes_cbc_decrypt( &output_buf, &input_buf, &key_buf, iv );
            if (ret == -100){
                hal_gpt_delay_ms(1);
            }
        } while(ret == -100);
        memcpy( iv, iv_temp, 16 );
    }
    else
    {
        do {
            ret = (int)hal_aes_cbc_encrypt( &output_buf, &input_buf, &key_buf, iv );
            if (ret == -100){
                hal_gpt_delay_ms(1);
            }
        } while(ret == -100);
        memcpy( iv, output_temp + length - 16, 16 );
    }

    memcpy( output, output_temp, length );
    mbedtls_free( output_temp );
    mbedtls_free( input_temp );

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo192M();
#endif
    return HAL_AES_STATUS_OK == ret ? 0 : -1;
}

#else
/* Bootloader cannot use alloc/free. */
int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx,
                                 int mode,
                                 size_t length,
                                 unsigned char iv[16],
                                 const unsigned char *input,
                                 unsigned char *output )
{
    int ret, i, loop_time;
    unsigned char iv_temp[16] = {0};
    unsigned char output_temp[48] = {0};
    hal_aes_buffer_t input_buf, output_buf, key_buf;

    if (16 > length || length % 16)
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );

    output_buf.length = 48;
    input_buf.length = 16;
    key_buf.buffer = ctx->key;
    key_buf.length = ctx->key_byte_length;

    loop_time = length / 16;

#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo96M();
#endif
    
    for (i = 0; i < loop_time; i++)
    {
        memset(output_temp, 0, 48);
        output_buf.buffer = output_temp;
        input_buf.buffer = input + i * 16;
        if( mode == 0 ) /* decrypt */
        {
            /* hal_aes_cbc_decrypt() does not update iv!
                        * For CBC, current 16 bytes encoded data is the iv of the next 16 bytes encoded data.
                        */
            memcpy( iv_temp, input, 16 );
         do {
                ret = (int)hal_aes_cbc_decrypt( &output_buf, &input_buf, &key_buf, iv );
                if (ret == -100){
                    hal_gpt_delay_ms(1);
                }
            } while(ret == -100);
            memcpy( iv, iv_temp, 16 );
        }
        else
        {
            do {
                ret = (int)hal_aes_cbc_encrypt( &output_buf, &input_buf, &key_buf, iv );
                if (ret == -100){
                    hal_gpt_delay_ms(1);
                }
            } while(ret == -100);
            memcpy( iv, output_temp, 16 );
        }

        memcpy( output, output_temp, 16);
        output += 16;
    }
    
#ifdef MBEDTLS_HW_ALGORITHM_CHANGE_CPU_CLOCK
    cmnCpuClkSwitchTo192M();
#endif

    return HAL_AES_STATUS_OK == ret ? 0 : -1;
}
#endif /* MTK_BOOTLOADER_USE_MBEDTLS */
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int c;
    size_t n = *iv_off;

    if( mode == MBEDTLS_AES_DECRYPT )
    {
        while( length-- )
        {
            if( n == 0 )
                mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );

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
                mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = ( n + 1 ) & 0x0F;
        }
    }

    *iv_off = n;

    return( 0 );
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    unsigned char c;
    unsigned char ov[17];

    while( length-- )
    {
        memcpy( ov, iv, 16 );
        mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );

        if( mode == MBEDTLS_AES_DECRYPT )
            ov[16] = *input;

        c = *output++ = (unsigned char)( iv[0] ^ *input++ );

        if( mode == MBEDTLS_AES_ENCRYPT )
            ov[16] = c;

        memcpy( iv, ov + 1, 16 );
    }

    return( 0 );
}
#endif /*MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr( mbedtls_aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[16],
                       unsigned char stream_block[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int c, i;
    size_t n = *nc_off;

    while( length-- )
    {
        if( n == 0 ) {
            mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, nonce_counter, stream_block );

            for( i = 16; i > 0; i-- )
                if( ++nonce_counter[i - 1] != 0 )
                    break;
        }
        c = *input++;
        *output++ = (unsigned char)( c ^ stream_block[n] );

        n = ( n + 1 ) & 0x0F;
    }

    *nc_off = n;

    return( 0 );
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */
#endif /* MBEDTLS_AES_ALT */

#endif /* MBEDTLS_AES_C */

