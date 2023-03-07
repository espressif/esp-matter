/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sha256_alt.h"
#include "hash_driver.h"
#include "cc_pal_mem.h"
#include "cc_pal_log.h"
#include "cc_pal_abort.h"
#include "mbedtls_hash_common.h"
#include <stdio.h>

#if defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)


void mbedtls_sha256_init( mbedtls_sha256_context *ctx )
{
    if( NULL == ctx  )
    {
        CC_PalAbort("\nctx is NULL\n");
    }
    CC_PalMemSetZero(ctx, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_free( mbedtls_sha256_context *ctx )
{
    if( NULL == ctx  )
    {
        CC_PAL_LOG_ERR("\nctx is NULL\n");
        return;
    }
    mbedtls_zeroize_internal( ctx, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_clone( mbedtls_sha256_context *dst,
                         const mbedtls_sha256_context *src )
{
    if( NULL == src || NULL == dst )
    {
        CC_PalAbort("src or dst are NULL\n" );
    }
    *dst = *src;
}

/*
 * SHA-256 context setup
 */
int mbedtls_sha256_starts_ret( mbedtls_sha256_context *ctx, int is224 )
{
    int ret;

    switch (is224){
        case 0:
            ret = mbedtls_sha_starts_internal( ctx, HASH_SHA256);
            break;
        case 1 :
            ret = mbedtls_sha_starts_internal( ctx, HASH_SHA224);
            break;
        default:
            return MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED;
    };

    if( ret != 0 )
    {
        return( MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED );
    }

    return( ret );
}

int mbedtls_internal_sha256_process( mbedtls_sha256_context *ctx, const unsigned char data[64] )
{
    int ret;

    ret = mbedtls_sha_process_internal( ctx, data );
    if( ret != 0 )
    {
        return( MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED );
    }

    return( ret );
}


int mbedtls_sha256_update_ret( mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen )
{
    int ret;

    ret = mbedtls_sha_update_internal( ctx, input, ilen );
    if( ret != 0 )
    {
        return( MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED );
    }

    return( ret );
}


/*
 * SHA-256 final digest
 */
int mbedtls_sha256_finish_ret( mbedtls_sha256_context *ctx, unsigned char output[32] )
{
    int ret;
    HashContext_t *pHashCtx = NULL;

    if (NULL == ctx || NULL == output){
        CC_PAL_LOG_ERR( "ctx or output buffer are NULL\n" );
        return( MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED );
    }

    if (NULL == output){
        CC_PAL_LOG_ERR( "output is NULL\n" );
        return( MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED );
    }

    pHashCtx = (HashContext_t *)ctx;
    ret = mbedtls_sha_finish_internal( ctx );
    if( ret != 0 )
    {
        return( MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED );
    }

    /* Copy the result to the user buffer  */
    switch (pHashCtx->mode)
    {
        case HASH_SHA224:
            CC_PalMemCopy(output, pHashCtx->digest, SHA224_DIGEST_SIZE_IN_BYTES);
            break;
        case HASH_SHA256:
            CC_PalMemCopy(output, pHashCtx->digest, SHA256_DIGEST_SIZE_IN_BYTES);
            break;
        default:
            CC_PAL_LOG_ERR( "Unsupported HASH type (%d)\n", pHashCtx->mode );
    }

    return( 0 );

}
#endif /* #if defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C) */



