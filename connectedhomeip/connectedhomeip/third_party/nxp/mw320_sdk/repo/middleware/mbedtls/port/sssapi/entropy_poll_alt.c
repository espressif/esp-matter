/*
 * Copyright 2019-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "sssapi_mbedtls.h"

/* Entropy poll callback for a hardware source */
#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    status_t result = kStatus_Fail;
    sss_sscp_rng_t ctx;

    CRYPTO_InitHardware();

    if (sss_sscp_rng_context_init(&g_sssSession, &ctx, 0u) != kStatus_SSS_Success)
    {
    }
    else if (sss_sscp_rng_get_random(&ctx, output, len) != kStatus_SSS_Success)
    {
    }
    else if (sss_sscp_rng_free(&ctx) != kStatus_SSS_Success)
    {
    }
    else
    {
        result = kStatus_Success;
    }

    if (result == kStatus_Success)
    {
        *olen = len;
        return 0;
    }
    else
    {
        return result;
    }
}
#endif /* MBEDTLS_ENTROPY_HARDWARE_ALT */
