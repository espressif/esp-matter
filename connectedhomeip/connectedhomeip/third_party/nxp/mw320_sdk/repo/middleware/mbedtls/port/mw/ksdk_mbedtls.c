/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "fsl_common.h"
#include "mbedtls/entropy.h"
#include "mbedtls/entropy_poll.h"
#include "mbedtls/ctr_drbg.h"

#define SHA256_HASH_SIZE (32U)

static uint8_t s_hashBuf[SHA256_HASH_SIZE];
static uint32_t s_hashLen;
static bool s_internalEntropyContextSet;
static struct
{
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctrDrbg;
} s_internalMbedtlsGdata;

/******************************************************************************/
/******************** CRYPTO_InitHardware **************************************/
/******************************************************************************/
/*!
 * @brief Application init for various Crypto blocks.
 *
 * This function is provided to be called by MCUXpresso SDK applications.
 * It calls basic init for Crypto Hw acceleration and Hw entropy modules.
 */
void CRYPTO_InitHardware(void)
{
}

static int internal_entropy_poll(void *data,
		unsigned char *output,
		size_t len,
		size_t *olen)
{
    (void)data;

    if (s_hashLen > 0)
    {
        *olen = MIN(len, s_hashLen);
        memcpy(output, s_hashBuf, *olen);
    }
    else
    {   
        return -1;
    }

    return 0;
}

static int mbedtls_entropy_remove_source( mbedtls_entropy_context *ctx,
                        mbedtls_entropy_f_source_ptr f_source)
{
    int i;
    int ret = -1;

    for(i = 0; i < ctx->source_count; i++)
    {
        if (ctx->source[i].f_source == f_source)
        {
            ret = 0;
            ctx->source_count--;
            break;
        }
    }

    for(; i < ctx->source_count; i++)
    {
        ctx->source[i].f_source  = ctx->source[i + 1].f_source;
        ctx->source[i].p_source  = ctx->source[i + 1].p_source;
        ctx->source[i].threshold = ctx->source[i + 1].threshold;
        ctx->source[i].strong    = ctx->source[i + 1].strong;
    }
    ctx->source[i].f_source = NULL;
    ctx->source[i].p_source = NULL;

    return ret;
}

static int internal_entropy_ctr_drbg_setup(void)
{
    int ret = 0;

    if (!s_internalEntropyContextSet)
    {
        mbedtls_entropy_init(&s_internalMbedtlsGdata.entropy);

        mbedtls_entropy_remove_source(&s_internalMbedtlsGdata.entropy, mbedtls_hardware_poll);

        mbedtls_entropy_add_source(&s_internalMbedtlsGdata.entropy,
                internal_entropy_poll,
                NULL,
                MBEDTLS_ENTROPY_MIN_HARDWARE,
                MBEDTLS_ENTROPY_SOURCE_STRONG);

        mbedtls_ctr_drbg_init(&s_internalMbedtlsGdata.ctrDrbg);

        if ((ret = mbedtls_ctr_drbg_seed(&s_internalMbedtlsGdata.ctrDrbg, mbedtls_entropy_func,
                                &s_internalMbedtlsGdata.entropy, NULL, 0)) != 0)
        {
            PRINTF("mbedtls_ctr_drbg_seed returned, ret = -0x%02X", -ret);
            return -1;
        }

        mbedtls_ctr_drbg_set_reseed_interval(&s_internalMbedtlsGdata.ctrDrbg, INT_MAX);
        mbedtls_ctr_drbg_set_prediction_resistance(&s_internalMbedtlsGdata.ctrDrbg, MBEDTLS_CTR_DRBG_PR_OFF );
        s_internalEntropyContextSet = true;
    }

    return 0;
}

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    int ret;

    (void)data;

    ret = internal_entropy_ctr_drbg_setup();
    assert(ret == 0);

    ret = mbedtls_ctr_drbg_random(&s_internalMbedtlsGdata.ctrDrbg, output, len);
    *olen = len;

    return ret;
}

void mbedtls_hardware_init_hash(uint8_t *entropy, size_t len)
{
    assert(entropy != NULL);
    assert(len > 0U);

    s_hashLen = MIN(len, SHA256_HASH_SIZE);
    memcpy(s_hashBuf, entropy, s_hashLen);
}

/******************************************************************************/
/*************************** FreeRTOS ********************************************/
/******************************************************************************/
#if USE_RTOS && defined(FSL_RTOS_FREE_RTOS) && defined(MBEDTLS_FREESCALE_FREERTOS_CALLOC_ALT)
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

void *pvPortCalloc(size_t num, size_t size)
{
    void *mem;
    mem = pvPortMalloc(num * size);

    if (mem != NULL)
    {
        memset(mem, 0, num * size);
    }

    return mem;
}
#endif /* USE_RTOS && defined(FSL_RTOS_FREE_RTOS) && defined(MBEDTLS_FREESCALE_FREERTOS_CALLOC_ALT) */
