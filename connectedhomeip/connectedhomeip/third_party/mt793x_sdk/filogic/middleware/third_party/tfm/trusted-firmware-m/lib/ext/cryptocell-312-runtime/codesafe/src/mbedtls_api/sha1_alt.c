/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sha1_alt.h"
#include "hash_driver.h"
#include "cc_pal_mem.h"
#include "cc_pal_log.h"
#include "cc_pal_abort.h"
#include "mbedtls_hash_common.h"
#include <stdio.h>

#if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)

/* Internal function*/

void mbedtls_sha1_init( mbedtls_sha1_context *ctx )
{
    if( NULL == ctx  )
    {
        CC_PalAbort("\nctx is NULL\n");
    }
    CC_PalMemSetZero(ctx, sizeof( mbedtls_sha1_context ) );
}

void mbedtls_sha1_free( mbedtls_sha1_context *ctx )
{
    if( NULL == ctx  )
    {
        CC_PAL_LOG_ERR("\nctx is NULL\n");
        return;
    }
    mbedtls_zeroize_internal( ctx, sizeof( mbedtls_sha1_context ) );
}

void mbedtls_sha1_clone( mbedtls_sha1_context *dst,
                         const mbedtls_sha1_context *src )
{
    if( NULL == src || NULL == dst )
    {
        CC_PalAbort("src or dst are NULL\n" );
    }

    *dst = *src;
}

int mbedtls_sha1_starts_ret( mbedtls_sha1_context *ctx )
{
    int ret;

    ret = mbedtls_sha_starts_internal( ctx, HASH_SHA1);
    if( ret != 0 )
    {
        return( MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED );
    }

    return( ret );
}

int mbedtls_internal_sha1_process( mbedtls_sha1_context *ctx, const unsigned char data[64] )
{
    int ret;

    ret = mbedtls_sha_process_internal( ctx, data );
    if( ret != 0 )
    {
        return( MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED );
    }

    return( ret );
}


/*
 * SHA-1 process buffer
 */
int mbedtls_sha1_update_ret( mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen )
{
    int ret;

    ret = mbedtls_sha_update_internal( ctx, input, ilen );
    if( ret != 0 )
    {
        return( MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED );
    }

    return( ret );
}


/*
 * SHA-1 final digest
 */
int mbedtls_sha1_finish_ret( mbedtls_sha1_context *ctx, unsigned char output[20] )
{
    int ret;

    HashContext_t *pHashCtx = NULL;
    if (NULL == ctx || NULL == output ){
        CC_PAL_LOG_ERR( "ctx or output buffer are NULL\n" );
        return( MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED );
    }
    pHashCtx = (HashContext_t *)ctx;

    ret = mbedtls_sha_finish_internal( ctx );
    if( ret != 0)
    {
        return( MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED );
    }

    CC_PalMemCopy(output, pHashCtx->digest, SHA1_DIGEST_SIZE_IN_BYTES);

    return( ret );
}
#endif /* #if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C) */

