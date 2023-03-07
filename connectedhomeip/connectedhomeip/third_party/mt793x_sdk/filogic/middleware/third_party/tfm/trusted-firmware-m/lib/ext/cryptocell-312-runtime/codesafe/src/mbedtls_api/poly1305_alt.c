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

#if defined(MBEDTLS_POLY1305_C)
#include "mbedtls/poly1305.h"
#include "poly.h"
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

#if defined(MBEDTLS_POLY1305_ALT)


void mbedtls_poly1305_init( mbedtls_poly1305_context *ctx )
{
    CC_UNUSED_PARAM(ctx);
    return;
}

void mbedtls_poly1305_free( mbedtls_poly1305_context *ctx )
{
    CC_UNUSED_PARAM(ctx);
    return;
}

/* Cryptocell only supports integrated poly1305 operations  */
int mbedtls_poly1305_starts( mbedtls_poly1305_context *ctx,
                             const unsigned char key[32] )
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(key);
    return MBEDTLS_ERR_POLY1305_FEATURE_UNAVAILABLE;
}

/* Cryptocell only supports integrated poly1305 operations  */
int mbedtls_poly1305_update( mbedtls_poly1305_context *ctx,
                             const unsigned char *input,
                             size_t ilen )
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(input);
    CC_UNUSED_PARAM(ilen);
    return MBEDTLS_ERR_POLY1305_FEATURE_UNAVAILABLE;
}

/* Cryptocell only supports integrated poly1305 operations  */
int mbedtls_poly1305_finish( mbedtls_poly1305_context *ctx,
                             unsigned char mac[16] )
{
    CC_UNUSED_PARAM(ctx);
    CC_UNUSED_PARAM(mac);
    return MBEDTLS_ERR_POLY1305_FEATURE_UNAVAILABLE;
}

int mbedtls_poly1305_mac( const unsigned char key[32],
                          const unsigned char *input,
                          size_t ilen,
                          unsigned char mac[16] )
{
    int rc;
    mbedtls_poly_key pKey;
    mbedtls_poly_mac macRes;

    // Verify inputs
    if (key == NULL) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }
    if ((mac == NULL) || ((input == NULL) ^ (ilen == 0)) || (ilen > CC_MAX_UINT32_VAL)) {
        return MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA;
    }

    CC_PalMemCopy((unsigned char *)pKey, key, MBEDTLS_POLY_KEY_SIZE_BYTES);

    rc = PolyMacCalc(pKey, NULL, 0, input, ilen, macRes, false);
    if (rc != 0) {
        return MBEDTLS_ERR_POLY1305_HW_ACCEL_FAILED;
    }

    CC_PalMemCopy(mac, (unsigned char *)macRes, MBEDTLS_POLY_MAC_SIZE_BYTES);

    return ( 0 );
}


#endif /* !MBEDTLS_POLY1305_ALT */

#endif /* !MBEDTLS_POLY1305_C */
