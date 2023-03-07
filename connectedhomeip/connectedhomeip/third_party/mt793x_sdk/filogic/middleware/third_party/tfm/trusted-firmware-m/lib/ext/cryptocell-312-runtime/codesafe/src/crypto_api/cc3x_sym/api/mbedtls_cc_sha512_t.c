/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_CC_API

#include "cc_pal_abort.h"
#include "mbedtls_cc_sha512_t.h"
#include "sha512.h"
#include "cc_pal_mem.h"

#if defined(MBEDTLS_SHA512_C)

#include "mbedtls/sha512.h"

#if defined(_MSC_VER) || defined(__WATCOMC__)
  #define UL64(x) x##ui64
#else
  #define UL64(x) x##ULL
#endif

#include <string.h>


#define MBEDTLS_SHA512_T_224_DIGEST_SIZE_BYTES 28
#define MBEDTLS_SHA512_T_256_DIGEST_SIZE_BYTES 32

void mbedtls_sha512_t_init( mbedtls_sha512_context *ctx )
{
    if (ctx == NULL) {
        CC_PalAbort("mbedtls_sha512_context cannot be NULL");
    }
    mbedtls_sha512_init(ctx);
}

void mbedtls_sha512_t_free( mbedtls_sha512_context *ctx )
{
    if (ctx != NULL) {
        mbedtls_sha512_free(ctx);
    }
}

/*
 * SHA-512_t context setup
 */
void mbedtls_sha512_t_starts( mbedtls_sha512_context *ctx, int is224 )
{
    if (ctx == NULL) {
        CC_PalAbort("mbedtls_sha512_context cannot be NULL");
    }

    if (is224 != 0 && is224 != 1 ) {
        CC_PalAbort("mbedtls_sha512_starts: is224 must be 0 or 1");
    }

    ctx->total[0] = 0;
    ctx->total[1] = 0;

    if( is224 == 1 ) {
        /* SHA-512/224 */
        ctx->state[0] = UL64(0x8C3D37C819544DA2);
        ctx->state[1] = UL64(0x73E1996689DCD4D6);
        ctx->state[2] = UL64(0x1DFAB7AE32FF9C82);
        ctx->state[3] = UL64(0x679DD514582F9FCF);
        ctx->state[4] = UL64(0x0F6D2B697BD44DA8);
        ctx->state[5] = UL64(0x77E36F7304C48942);
        ctx->state[6] = UL64(0x3F9D85A86A1D36C8);
        ctx->state[7] = UL64(0x1112E6AD91D692A1);
    }
    else {
        /* SHA-512/256 */
        ctx->state[0] = UL64(0x22312194FC2BF72C);
        ctx->state[1] = UL64(0x9F555FA3C84C64C2);
        ctx->state[2] = UL64(0x2393B86B6F53B151);
        ctx->state[3] = UL64(0x963877195940EABD);
        ctx->state[4] = UL64(0x96283EE2A88EFFE3);
        ctx->state[5] = UL64(0xBE5E1E2553863992);
        ctx->state[6] = UL64(0x2B0199FC2C85B8AA);
        ctx->state[7] = UL64(0x0EB72DDC81C52CA2);
    }

    ctx->is384 = 0;
}


void mbedtls_sha512_t_process( mbedtls_sha512_context *ctx, const unsigned char data[128] )
{
    if (ctx == NULL || data == NULL) {
        CC_PalAbort("mbedtls_sha512_context and data buffer cannot be NULL");
    }
    mbedtls_sha512_process(ctx, data);
}

/*
 * SHA-512 process buffer
 */
void mbedtls_sha512_t_update( mbedtls_sha512_context *ctx, const unsigned char *input,
                    size_t ilen )
{
    if (ctx == NULL || input == NULL) {
        CC_PalAbort("mbedtls_sha512_context and input buffer cannot be NULL");
    }
    mbedtls_sha512_update(ctx,input,ilen);
}

/*
 * SHA-512 final digest
 */
void mbedtls_sha512_t_finish( mbedtls_sha512_context *ctx, unsigned char output[32], int is224 )
{
    unsigned char output512[64] = {0};

    if (ctx == NULL || output == NULL) {
        CC_PalAbort("mbedtls_sha512_context and output buffer cannot be NULL");
    }
    if (is224 != 0 && is224 != 1 ) {
        CC_PalAbort("mbedtls_sha512_t_finish: is224 must be 0 or 1");
    }

    mbedtls_sha512_finish(ctx, output512);
    if (is224) {
        CC_PalMemCopy(output, output512, MBEDTLS_SHA512_T_224_DIGEST_SIZE_BYTES);
    }
    else {
        CC_PalMemCopy(output, output512, MBEDTLS_SHA512_T_256_DIGEST_SIZE_BYTES);
    }
}

#endif /* !MBEDTLS_SHA512_ALT */

/*
 * output = SHA-512( input buffer )
 */
void mbedtls_sha512_t( const unsigned char *input, size_t ilen,
             unsigned char output[32], int is224 )
{
    mbedtls_sha512_context ctx;

    if (input == NULL || output == NULL) {
        CC_PalAbort("input and output buffers cannot be NULL");
    }

    mbedtls_sha512_t_init( &ctx );
    mbedtls_sha512_t_starts( &ctx, is224 );
    mbedtls_sha512_t_update( &ctx, input, ilen );
    mbedtls_sha512_t_finish( &ctx, output, is224 );
    mbedtls_sha512_t_free( &ctx );
}
