/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * References:
 *
 * - NIST SP 800-38B Recommendation for Block Cipher Modes of Operation: The
 *      CMAC Mode for Authentication
 *   http://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-38b.pdf
 *
 * - RFC 4493 - The AES-CMAC Algorithm
 *   https://tools.ietf.org/html/rfc4493
 *
 * - RFC 4615 - The Advanced Encryption Standard-Cipher-based Message
 *      Authentication Code-Pseudo-Random Function-128 (AES-CMAC-PRF-128)
 *      Algorithm for the Internet Key Exchange Protocol (IKE)
 *   https://tools.ietf.org/html/rfc4615
 *
 *   Additional test vectors: ISO/IEC 9797-1
 *
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_CMAC_C) && defined (MBEDTLS_CMAC_ALT)

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#define mbedtls_calloc calloc
#define mbedtls_free   free
#endif


#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "cc_common.h"
#include "aes_driver.h"
#include "cc_aes_defs.h"
#include "cc_aes_error.h"
#include "mbedtls_common.h"
#include "mbedtls/cmac.h"
#include "memory_buffer_alloc.h"


/**
 * \brief        CMAC control context structure
 */
typedef struct {
    /** Internal state of the CMAC algorithm  */
    unsigned char   state[MBEDTLS_AES_BLOCK_SIZE];

    /** Unprocessed data - either data that was not block aligned and is still
     *  pending to be processed, or the final block */
    uint8_t         unprocessed_block[MBEDTLS_AES_BLOCK_SIZE];

    /** Length of data pending to be processed */
    uint32_t        unprocessed_len;
}
mbedtls_cmac_control_context_t;


/**
 * \brief        CMAC private context structure
 */
typedef struct {
    AesContext_t                    aes_ctx;
    mbedtls_cmac_control_context_t  cmac_ctrl_ctx;
}
mbedtls_cmac_private_context_t;

/*
 * Initialize a context
 */
static int cmac_init( AesContext_t *ctx )
{
    if( ctx == NULL )
    {
        CC_PAL_LOG_ERR("NULL pointer exception\n");
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    mbedtls_zeroize_internal(ctx, sizeof(AesContext_t));

    ctx->mode               = CIPHER_CMAC;
    ctx->padType            = CRYPTO_PADDING_NONE;
    ctx->dir                = CRYPTO_DIRECTION_ENCRYPT;
    ctx->inputDataAddrType  = DLLI_ADDR;
    ctx->outputDataAddrType = DLLI_ADDR;
    ctx->dataBlockType      = FIRST_BLOCK;

    return(0);
}

static int cmac_setkey(AesContext_t *ctx,
               const unsigned char *key,
               unsigned int keybits)
{
    if (ctx == NULL || key == NULL) {
        CC_PAL_LOG_ERR("Null pointer, ctx or key are NULL\n");
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    switch (keybits) {
        case 128:
            ctx->keySizeId = KEY_SIZE_128_BIT;
            break;
        case 192:
            ctx->keySizeId = KEY_SIZE_192_BIT;
            break;
        case 256:
            ctx->keySizeId = KEY_SIZE_256_BIT;
            break;
        default:
            return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    /* update key information in the context */
    ctx->cryptoKey = USER_KEY;

    /* Copy user key to context */
    CC_PalMemCopy(ctx->keyBuf, key, (keybits/8));

    return(0);
}

//***************************************************************************************************//
int mbedtls_cipher_cmac_starts( mbedtls_cipher_context_t *ctx, const unsigned char *key, size_t keybits )
{
    int retval;
    mbedtls_cmac_context_t *cmac_ctx = NULL;

    if( ctx == NULL )
    {
        CC_PAL_LOG_ERR("NULL pointer exception\n");
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    /* Allocated and initialize in the cipher context memory for the CMAC context */
    cmac_ctx = mbedtls_calloc( 1, sizeof( mbedtls_cmac_private_context_t ) );
    if( cmac_ctx == NULL ) {
        return( MBEDTLS_ERR_CIPHER_ALLOC_FAILED );
    }

    ctx->cmac_ctx = cmac_ctx;
    mbedtls_zeroize_internal( ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.state,
                            sizeof( ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.state ) );

    /* Init */
    if( ( retval = cmac_init( &((mbedtls_cmac_private_context_t*)cmac_ctx)->aes_ctx ) ) != 0 )
    {
       CC_PAL_LOG_ERR(" 'cmac_init' failed with return code %d\n", retval);
       return( retval );
    }

    /* Set key */
    if( ( retval = cmac_setkey( &((mbedtls_cmac_private_context_t*)cmac_ctx)->aes_ctx, key, keybits) ) != 0 )
    {
       CC_PAL_LOG_ERR(" 'cmac_setkey' failed with return code %d\n", retval);
       return( retval );
    }

    return (0);
}

int mbedtls_cipher_cmac_update( mbedtls_cipher_context_t *ctx, const unsigned char *input, size_t ilen )
{
    mbedtls_cmac_context_t *cmac_ctx = NULL;
    drvError_t ret = 0;
    unsigned int block_size;
    size_t blocks_num;
    size_t main_chunk_in_bytes = 0;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    if( (ctx == NULL) || (input == NULL) )
    {
            CC_PAL_LOG_ERR("NULL pointer exception\n");
            return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    if( ctx == NULL || ctx->cipher_info == NULL || input == NULL ||
        ctx->cmac_ctx == NULL )
    {
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    cmac_ctx = ctx->cmac_ctx;
    block_size = ctx->cipher_info->block_size;

    /* Is there data still to process from the last call, that's greater in
     * size than a block? */
    if( (((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len > 0) &&
        (ilen > (block_size - ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len)) )
    {
        CC_PalMemCopy( &((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_block[((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len],
                input,
                block_size - ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len );

        ret = SetDataBuffersInfo((const uint8_t*)&((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_block,
                                 block_size,
                                 &inBuffInfo,
                                 NULL,
                                 0,
                                 &outBuffInfo);
        if (ret != 0)
        {
             CC_PAL_LOG_ERR("illegal data buffers\n");
             return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
        }

        ret = ProcessAesDrv(&((mbedtls_cmac_private_context_t*)cmac_ctx)->aes_ctx,
                         &inBuffInfo,
                         &outBuffInfo,
                         block_size);

        if (AES_DRV_OK != ret)
        {
            CC_PAL_LOG_ERR("ProcessAesDrv failed with return code %d\n", ret);
            return MBEDTLS_ERR_CIPHER_AUTH_FAILED;
        }

        input += (block_size - ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len);
        ilen  -= (block_size - ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len);
        ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len = 0;
    }

    /* blocks_num is the number of blocks including any final partial block */
    blocks_num = ( ilen + block_size - 1 ) / block_size;
    if ( 1 < blocks_num )
    {
        main_chunk_in_bytes = (blocks_num - 1) * block_size;

        ret = SetDataBuffersInfo(input,
                                 main_chunk_in_bytes,
                                 &inBuffInfo,
                                 NULL,
                                 0,
                                 &outBuffInfo);
        if (ret != 0)
        {
             CC_PAL_LOG_ERR("illegal data buffers\n");
             return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
        }

        /* Process the input data, excluding any final partial or complete block */
        ret = ProcessAesDrv(&((mbedtls_cmac_private_context_t*)cmac_ctx)->aes_ctx,
                         &inBuffInfo,
                         &outBuffInfo,
                         main_chunk_in_bytes);
        if (AES_DRV_OK != ret)
        {
            CC_PAL_LOG_ERR("ProcessAesDrv failed with return code %d\n",ret);
            return MBEDTLS_ERR_CIPHER_AUTH_FAILED;
        }

        ilen  -= main_chunk_in_bytes;
        input += main_chunk_in_bytes;
    }

    /* If there is data left over that wasn't aligned to a block */
    if( ilen > 0 )
    {
        CC_PalMemCopy( &((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_block[((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len],
                input,
                ilen );
        ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len += ilen;
    }

    return (0);
}

int mbedtls_cipher_cmac_finish( mbedtls_cipher_context_t *ctx, unsigned char *output )
{
    mbedtls_cmac_context_t *cmac_ctx = NULL;
    drvError_t ret = 0;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    if( ctx == NULL || ctx->cipher_info == NULL || ctx->cmac_ctx == NULL ||
            output == NULL )
    {
            return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    cmac_ctx = ctx->cmac_ctx;

    ret = SetDataBuffersInfo((const uint8_t*)&((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_block,
                             ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len,
                             &inBuffInfo,
                             NULL,
                             0,
                             &outBuffInfo);
    if (ret != 0)
    {
         CC_PAL_LOG_ERR("illegal data buffers\n");
         return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    ret = FinishAesDrv(&((mbedtls_cmac_private_context_t*)cmac_ctx)->aes_ctx,
                      &inBuffInfo,
                      &outBuffInfo,
                      (uint32_t)((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len);

    if (AES_DRV_OK != ret)
    {
        CC_PAL_LOG_ERR("FinishAesDrv failed with return code 0x%x\n", ret);
        return MBEDTLS_ERR_CIPHER_AUTH_FAILED;
    }

    CC_PalMemCopy(output, ((mbedtls_cmac_private_context_t*)cmac_ctx)->aes_ctx.ivBuf, AES_IV_SIZE);

    mbedtls_zeroize_internal( ctx->cmac_ctx, sizeof( mbedtls_cmac_private_context_t ) );

    return (0);
}

int mbedtls_cipher_cmac_reset( mbedtls_cipher_context_t *ctx )
{
    mbedtls_cmac_context_t* cmac_ctx = NULL;

    if( ctx == NULL || ctx->cipher_info == NULL || ctx->cmac_ctx == NULL )
    {
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    cmac_ctx = ctx->cmac_ctx;

    /* Reset the internal state */
    ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_len = 0;
    mbedtls_zeroize_internal( ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_block,
                             sizeof( ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.unprocessed_block ) );
    mbedtls_zeroize_internal( ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.state,
                             sizeof( ((mbedtls_cmac_private_context_t*)cmac_ctx)->cmac_ctrl_ctx.state ) );

    /* Zeroize the IV in the context */
    mbedtls_zeroize_internal( ((mbedtls_cmac_private_context_t*)cmac_ctx)->aes_ctx.ivBuf,
                                 sizeof( ((mbedtls_cmac_private_context_t*)cmac_ctx)->aes_ctx.ivBuf ) );

    return( 0 );
}

int mbedtls_cipher_cmac( const mbedtls_cipher_info_t *cipher_info,
                         const unsigned char *key, size_t keylen,
                         const unsigned char *input, size_t ilen,
                         unsigned char *output )
{
    mbedtls_cipher_context_t ctx;
    int ret;

    if( cipher_info == NULL || key == NULL || input == NULL || output == NULL )
    {
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }
    if( cipher_info->base == NULL )
    {
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }
    mbedtls_cipher_init( &ctx );
    ret = mbedtls_cipher_setup( &ctx, cipher_info );
    if( ret != 0 )
    {
        goto exit;
    }

    ret = mbedtls_cipher_cmac_starts( &ctx, key, keylen );
    if( ret != 0 )
    {
        goto exit;
    }

    ret = mbedtls_cipher_cmac_update( &ctx, input, ilen );
    if( ret != 0 )
    {
        goto exit;
    }

    ret = mbedtls_cipher_cmac_finish( &ctx, output );

exit:
    mbedtls_cipher_free( &ctx );

    return( ret );
}

#if defined(MBEDTLS_AES_C)
/*
 * Implementation of AES-CMAC-PRF-128 defined in RFC 4615
 */
int mbedtls_aes_cmac_prf_128( const unsigned char *key, size_t key_length,
                              const unsigned char *input, size_t in_len,
                              unsigned char *output )
{
    int ret;
    const mbedtls_cipher_info_t *cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
    unsigned char zero_key[AES_BLOCK_SIZE];
    unsigned char int_key[AES_BLOCK_SIZE];

    if( key == NULL || input == NULL || output == NULL )
    {
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    if( key_length == AES_BLOCK_SIZE )
    {
        /* Use key as is */
        CC_PalMemCopy( int_key, key, AES_BLOCK_SIZE );
    }
    else
    {
        memset( zero_key, 0, AES_BLOCK_SIZE );

        ret = mbedtls_cipher_cmac( cipher_info, zero_key, 128, key,
                                   key_length, int_key );
        if( ret != 0 )
            goto exit;
    }

    ret = mbedtls_cipher_cmac( cipher_info, int_key, 128, input, in_len,
                               output );

exit:
    mbedtls_zeroize_internal( int_key, sizeof( int_key ) );

    return( ret );
}
#endif /* MBEDTLS_AES_C */

#endif /* defined(MBEDTLS_CMAC_C) && defined (MBEDTLS_CMAC_ALT) */
