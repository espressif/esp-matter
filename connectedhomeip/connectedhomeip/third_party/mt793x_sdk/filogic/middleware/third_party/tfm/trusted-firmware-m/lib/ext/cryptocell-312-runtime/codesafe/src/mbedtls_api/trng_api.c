/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "llf_rnd_trng.h"
#include "cc_rng_plat.h"
#include "cc_rnd_common.h"
#include "cc_pal_log.h"
#include "mbedtls_common.h"
#include "cc_pal_mem.h"


#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_fprintf fprintf
#define mbedtls_calloc calloc
#define mbedtls_free   free
#endif

#ifdef DEBUG
#define GOTO_END(ERR) \
    do { \
        Error = ERR; \
        mbedtls_fprintf(stderr, "%s:%d - %s: %d (0x%08x)\n", __FILE__, __LINE__, __func__, (int)Error, (int)Error); \
        goto End; \
    } while (0)

#define GOTO_CLEANUP(ERR) \
    do { \
        Error = ERR; \
        mbedtls_fprintf(stderr, "%s:%d - %s: %d (0x%08x)\n", __FILE__, __LINE__, __func__, (int)Error, (int)Error); \
        goto Cleanup; \
    } while (0)
#else // DEBUG
#define GOTO_END(ERR) \
    do { \
        Error = ERR; \
        goto End; \
    } while (0)

#define GOTO_CLEANUP(ERR) \
    do { \
        Error = ERR; \
        goto Cleanup; \
    } while (0)
#endif // DEBUG


int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    CCRndWorkBuff_t  *rndWorkBuff_ptr;
    CCRndState_t rndState;
    CCRndParams_t trngParams;
    int ret, Error = 0;
    uint32_t  *entrSource_ptr;

    CC_UNUSED_PARAM(data);

    if ( NULL == output )
    {
        CC_PAL_LOG_ERR( "output cannot be NULL\n" );
        GOTO_END( -1 );
    }
    if ( NULL == olen )
    {
        CC_PAL_LOG_ERR( "olen cannot be NULL\n" );
        GOTO_END( -1 );

    }
    if ( 0 == len )
    {
        CC_PAL_LOG_ERR( "len cannot be zero\n" );
        GOTO_END( -1 );
    }

    rndWorkBuff_ptr = ( CCRndWorkBuff_t * )mbedtls_calloc( 1, sizeof ( CCRndWorkBuff_t ) );
    if ( NULL == rndWorkBuff_ptr )
    {
        CC_PAL_LOG_ERR( "Error: cannot allocate memory for rndWorkbuff\n" );
        GOTO_END ( -1 );
    }
    CC_PalMemSetZero( &rndState, sizeof( CCRndState_t ) );
    CC_PalMemSetZero( &trngParams, sizeof( CCRndParams_t ) );

    ret = RNG_PLAT_SetUserRngParameters(  &trngParams );
    if ( ret != 0 )
    {
        CC_PAL_LOG_ERR( "Error: RNG_PLAT_SetUserRngParameters() failed.\n" );
        GOTO_CLEANUP( -1 );
    }

    ret = LLF_RND_GetTrngSource(
                &rndState ,    /*in/out*/
                &trngParams,       /*in/out*/
                0,                 /*in  -  isContinued - false*/
                (uint32_t*)&len,  /*in/out*/
                &entrSource_ptr,   /*out*/
                (uint32_t*)olen,               /*out*/
                (uint32_t*)rndWorkBuff_ptr,   /*in*/
                0                  /*in - isFipsSupport false*/ );
    if ( ret != 0 )
    {
        CC_PAL_LOG_ERR( "Error: LLF_RND_GetTrngSource() failed.\n" );
        GOTO_CLEANUP( -1 );
    }

    if (*olen <= len ){
        CC_PalMemCopy ( output, entrSource_ptr + CC_RND_TRNG_SRC_INNER_OFFSET_WORDS , *olen );
    } else{
        CC_PAL_LOG_ERR( "buffer length is smaller than LLF_RND_GetTrngSource output length\n" );
        GOTO_CLEANUP( -1 );
    }

Cleanup:
    mbedtls_zeroize_internal( rndWorkBuff_ptr, sizeof( CCRndWorkBuff_t ) );
    mbedtls_free( rndWorkBuff_ptr );
    mbedtls_zeroize_internal( &rndState, sizeof( CCRndState_t ) );
End:
    return Error;
}
