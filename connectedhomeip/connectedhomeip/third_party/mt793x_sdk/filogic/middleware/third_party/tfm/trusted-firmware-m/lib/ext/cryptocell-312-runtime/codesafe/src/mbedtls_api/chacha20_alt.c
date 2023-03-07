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

#if defined(MBEDTLS_CHACHA20_C)
#include "mbedtls/chacha20.h"
#include "chacha20_alt.h"
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

#if defined(MBEDTLS_CHACHA20_ALT)


void mbedtls_chacha20_init( mbedtls_chacha20_context *ctx )
{
    ChachaContext_t *chachaCtx = NULL;

    /* ............... checking the parameters validity ................... */
    /* -------------------------------------------------------------------- */
    if (NULL == ctx)
    {
        CC_PalAbort("ctx cannot be NULL");
    }

    if (sizeof(mbedtls_chacha20_context) != sizeof(ChachaContext_t)) {
        CC_PalAbort("\nChacha context sizes mismatch\n");
    }

    chachaCtx = (ChachaContext_t *)ctx;

    chachaCtx->inputDataAddrType = DLLI_ADDR;
    chachaCtx->outputDataAddrType = DLLI_ADDR;
}

void mbedtls_chacha20_free( mbedtls_chacha20_context *ctx )
{
    if( ctx != NULL )
    {
        mbedtls_platform_zeroize( ctx, sizeof( mbedtls_chacha20_context ) );
    }
}

int mbedtls_chacha20_setkey( mbedtls_chacha20_context *ctx,
                            const unsigned char key[32] )
{

    ChachaContext_t *chachaCtx = NULL;
    if( ( ctx == NULL ) || ( key == NULL ) )
    {
        return( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA );
    }

    chachaCtx = (ChachaContext_t *)ctx;
    CC_PalMemCopy(chachaCtx->keyBuf, key, MBEDTLS_CHACHA_KEY_SIZE_BYTES);

    return( 0 );
}

int mbedtls_chacha20_starts( mbedtls_chacha20_context* ctx,
                             const unsigned char nonce[12],
                             uint32_t counter )
{
    ChachaContext_t *chachaCtx = NULL;
    if( ( ctx == NULL ) || ( nonce == NULL ) )
    {
        return( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA );
    }
    if (sizeof(mbedtls_chacha20_context) != sizeof(ChachaContext_t)) {
        return MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA;
    }
    chachaCtx = (ChachaContext_t *)ctx;

    chachaCtx->nonceSize = (chachaNonceSize_t)MBEDTLS_CHACHA_NONCE_SIZE_12BYTE_TYPE;

    /* Copy the nonce to the context */
    CC_PalMemCopy(chachaCtx->nonceBuf, nonce, MBEDTLS_CHACHA_NONCE_SIZE_BYTES);

    /* init the block counter */
    chachaCtx->blockCounterLsb = counter;
    chachaCtx->blockCounterMsb = 0;

    return( 0 );
}

int mbedtls_chacha20_update( mbedtls_chacha20_context *ctx,
                              size_t size,
                              const unsigned char *input,
                              unsigned char *output )
{
    ChachaContext_t *chachaCtx = NULL;
    drvError_t drvRc;
    uintptr_t upDataOut = 0;
    uintptr_t upDataIn = 0;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    if( ctx == NULL )
    {
        return( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA );
    }

    if ( size != 0 && input == NULL ) {
        return MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA;
    }
    if ( size != 0 && output == NULL ) {
        return MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA;
    }
    if (size == 0) {
        return ( 0 );
    }

    upDataOut = (uintptr_t)input;
    upDataIn = (uintptr_t)output;
    if ((((upDataOut > upDataIn) && (upDataOut - upDataIn < size))) ||
        (((upDataIn > upDataOut) && (upDataIn - upDataOut < size)))) {
        return MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA;
    }

    drvRc = SetDataBuffersInfo(input, size, &inBuffInfo,
                               output, size, &outBuffInfo);
    if (drvRc != 0) {
        CC_PAL_LOG_ERR("illegal data buffers\n");
        return MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA;
    }

    chachaCtx = (ChachaContext_t *)ctx;

    drvRc = ProcessChacha(chachaCtx, &inBuffInfo, &outBuffInfo, size);
    if (drvRc != 0) {
        CC_PAL_LOG_ERR("\nProcessChacha failed %d", drvRc);
        return MBEDTLS_ERR_CHACHA20_HW_ACCEL_FAILED;
    }

    return ( 0 );
}

int mbedtls_chacha20_crypt( const unsigned char key[32],
                            const unsigned char nonce[12],
                            uint32_t counter,
                            size_t data_len,
                            const unsigned char* input,
                            unsigned char* output )
{
    mbedtls_chacha20_context ctx;
    int ret;

    mbedtls_chacha20_init( &ctx );

    ret = mbedtls_chacha20_setkey( &ctx, key );
    if( ret != 0 )
        goto cleanup;

    ret = mbedtls_chacha20_starts( &ctx, nonce, counter );
    if( ret != 0 )
        goto cleanup;

    ret = mbedtls_chacha20_update( &ctx, data_len, input, output );

cleanup:
    mbedtls_chacha20_free( &ctx );
    return( ret );
}

#endif /* !MBEDTLS_CHACHA20_ALT */

#endif /* !MBEDTLS_CHACHA20_C */
