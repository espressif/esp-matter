/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <limits.h>

/* mbedtls lib */
#include "mbedtls/timing.h"
#include "mbedtls/ctr_drbg.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"

#if defined(MBEDTLS_CTR_DRBG_C)
/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static RunItError_t runIt_ctrDrbgEntropyUsageTest(void);
static RunItError_t runIt_ctrDrbgVectorTest(void);
static int runIt_ctrDrbgSelfTestEntropy(void *data, unsigned char *buf, size_t len);

/************************************************************
 *
 * variables
 *
 ************************************************************/
static size_t test_offset;

/************************************************************
 *
 * static functions
 *
 ************************************************************/
static int runIt_ctrDrbgSelfTestEntropy(void *data, unsigned char *buf, size_t len)
{
    const unsigned char *p = data;
    memcpy(buf, p + test_offset, len);
    test_offset += len;
    return (0);
}

static RunItError_t runIt_ctrDrbgEntropyUsageTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    unsigned char out[16];
    unsigned char add[16];

    unsigned char *pEntropy = NULL;
    mbedtls_ctr_drbg_context *pCtx = NULL;

    RunItPtr entropyPtr;
    RunItPtr ctxPtr;

    size_t i, reps = 10;
    size_t last_idx;

    const char* TEST_NAME = "CTR-DRBG Entropy Usage";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC(entropyPtr, pEntropy, 1024);
    ALLOC_STRUCT(mbedtls_ctr_drbg_context, ctxPtr, pCtx);

    RUNIT_API(mbedtls_ctr_drbg_init(pCtx));
    test_offset = 0;
    memset(pEntropy, 0, 1024);
    memset(out, 0, sizeof(out));
    memset(add, 0, sizeof(add));

    /* Init must use entropy */
    last_idx = test_offset;
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_seed(pCtx, runIt_ctrDrbgSelfTestEntropy, pEntropy, NULL, 0) == 0);
    RUNIT_ASSERT(last_idx < test_offset);

    /* By default, PR is off and reseed_interval is large,
     * so the next few calls should not use entropy */
    last_idx = test_offset;
    for (i = 0; i < reps; i++)
    {
        RUNIT_ASSERT_API(mbedtls_ctr_drbg_random(pCtx, out, sizeof(out) - 4) == 0);
        RUNIT_ASSERT_API(mbedtls_ctr_drbg_random_with_add(pCtx, out, sizeof(out) - 4, add, sizeof(add)) == 0);
    }
    RUNIT_ASSERT(last_idx == test_offset);

    /* While at it, make sure we didn't write past the requested length */
    RUNIT_ASSERT(out[sizeof(out) - 4] == 0);
    RUNIT_ASSERT(out[sizeof(out) - 3] == 0);
    RUNIT_ASSERT(out[sizeof(out) - 2] == 0);
    RUNIT_ASSERT(out[sizeof(out) - 1] == 0);

    /* Set reseed_interval to the number of calls done,
     * so the next call should reseed */
    RUNIT_API(mbedtls_ctr_drbg_set_reseed_interval(pCtx, 2 * reps));
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_random(pCtx, out, sizeof(out)) == 0);
    RUNIT_ASSERT(last_idx < test_offset);

    /* The new few calls should not reseed */
    last_idx = test_offset;
    for (i = 0; i < reps / 2; i++)
    {
        RUNIT_ASSERT_API(mbedtls_ctr_drbg_random(pCtx, out, sizeof(out)) == 0);
        RUNIT_ASSERT_API(mbedtls_ctr_drbg_random_with_add(pCtx, out, sizeof(out), add, sizeof(add)) == 0);
    }
    RUNIT_ASSERT(last_idx == test_offset);

    /* Call update with too much data (sizeof entropy > MAX(_SEED)_INPUT)
     * (just make sure it doesn't cause memory corruption) */
    RUNIT_API(mbedtls_ctr_drbg_update(pCtx, pEntropy, MBEDTLS_CTR_DRBG_MAX_SEED_INPUT));

    /* Now enable PR, so the next few calls should all reseed */
    RUNIT_API(mbedtls_ctr_drbg_set_prediction_resistance(pCtx, MBEDTLS_CTR_DRBG_PR_ON));
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_random(pCtx, out, sizeof(out)) == 0);
    RUNIT_ASSERT(last_idx < test_offset);

    /* Finally, check setting entropy_len */
    RUNIT_API(mbedtls_ctr_drbg_set_entropy_len(pCtx, 42));
    last_idx = test_offset;
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_random(pCtx, out, sizeof(out)) == 0);
    RUNIT_ASSERT(test_offset - last_idx == 42);

    RUNIT_API(mbedtls_ctr_drbg_set_entropy_len(pCtx, 13));
    last_idx = test_offset;
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_random(pCtx, out, sizeof(out)) == 0);
    RUNIT_ASSERT(test_offset - last_idx == 13);

bail:
    RUNIT_API(mbedtls_ctr_drbg_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(entropyPtr);

    RUNIT_SUB_TEST_RESULT(TEST_NAME);
    return rc;
}

static RunItError_t runIt_ctrDrbgVectorTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    static const unsigned char nonce_pers_pr[16] = { 0xd2, 0x54, 0xfc, 0xff, 0x02, 0x1e, 0x69, 0xd2, 0x29, 0xc9, 0xcf, 0xad, 0x85, 0xfa, 0x48, 0x6c };
    static const unsigned char nonce_pers_nopr[16] = { 0x1b, 0x54, 0xb8, 0xff, 0x06, 0x42, 0xbf, 0xf5, 0x21, 0xf1, 0x5c, 0x1c, 0x0b, 0x66, 0x5f, 0x3f };
    static const unsigned char entropy_source_pr[96] = { 0xc1, 0x80, 0x81, 0xa6, 0x5d, 0x44, 0x02, 0x16, 0x19, 0xb3, 0xf1, 0x80, 0xb1, 0xc9, 0x20, 0x02, 0x6a, 0x54, 0x6f, 0x0c, 0x70, 0x81, 0x49, 0x8b, 0x6e, 0xa6, 0x62, 0x52, 0x6d, 0x51, 0xb1, 0xcb, 0x58, 0x3b, 0xfa, 0xd5, 0x37, 0x5f, 0xfb, 0xc9, 0xff, 0x46, 0xd2, 0x19, 0xc7, 0x22, 0x3e, 0x95, 0x45, 0x9d, 0x82, 0xe1, 0xe7, 0x22, 0x9f, 0x63, 0x31, 0x69, 0xd2, 0x6b, 0x57, 0x47, 0x4f, 0xa3, 0x37, 0xc9, 0x98, 0x1c, 0x0b, 0xfb, 0x91, 0x31, 0x4d, 0x55, 0xb9, 0xe9, 0x1c, 0x5a, 0x5e, 0xe4, 0x93, 0x92, 0xcf, 0xc5, 0x23, 0x12, 0xd5, 0x56, 0x2c, 0x4a, 0x6e, 0xff, 0xdc, 0x10, 0xd0, 0x68 };
    static const unsigned char entropy_source_nopr[64] = { 0x5a, 0x19, 0x4d, 0x5e, 0x2b, 0x31, 0x58, 0x14, 0x54, 0xde, 0xf6, 0x75, 0xfb, 0x79, 0x58, 0xfe, 0xc7, 0xdb, 0x87, 0x3e, 0x56, 0x89, 0xfc, 0x9d, 0x03, 0x21, 0x7c, 0x68, 0xd8, 0x03, 0x38, 0x20, 0xf9, 0xe6, 0x5e, 0x04, 0xd8, 0x56, 0xf3, 0xa9, 0xc4, 0x4a, 0x4c, 0xbd, 0xc1, 0xd0, 0x08, 0x46, 0xf5, 0x98, 0x3d, 0x77, 0x1c, 0x1b, 0x13, 0x7e, 0x4e, 0x0f, 0x9d, 0x8e, 0xf4, 0x09, 0xf9, 0x2e };
    static const unsigned char result_nopr[16] = { 0xa0, 0x54, 0x30, 0x3d, 0x8a, 0x7e, 0xa9, 0x88, 0x9d, 0x90, 0x3e, 0x07, 0x7c, 0x6f, 0x21, 0x8f };
    static const unsigned char result_pr[16] = { 0x34, 0x01, 0x16, 0x56, 0xb4, 0x29, 0x00, 0x8f, 0x35, 0x63, 0xec, 0xb5, 0xf2, 0x59, 0x07, 0x23 };

    mbedtls_ctr_drbg_context *pCtx = NULL;
    RunItPtr ctxPtr;

    unsigned char buf[16];

    const char* TEST_NAME = "CTR-DRBG Vectors";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_ctr_drbg_context, ctxPtr, pCtx);

    RUNIT_API(mbedtls_ctr_drbg_init(pCtx));

    /*
     * Based on a NIST CTR_DRBG test vector (PR = True)
     */
    test_offset = 0;
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_seed_entropy_len(pCtx, runIt_ctrDrbgSelfTestEntropy, (void * ) entropy_source_pr, nonce_pers_pr, 16, 32) == 0);
    RUNIT_API(mbedtls_ctr_drbg_set_prediction_resistance(pCtx, MBEDTLS_CTR_DRBG_PR_ON));
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_random(pCtx, buf, MBEDTLS_CTR_DRBG_BLOCKSIZE) == 0);
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_random(pCtx, buf, MBEDTLS_CTR_DRBG_BLOCKSIZE) == 0);
    RUNIT_ASSERT(memcmp( buf, result_pr, MBEDTLS_CTR_DRBG_BLOCKSIZE) == 0);

    RUNIT_API(mbedtls_ctr_drbg_free(pCtx));

    /*
     * Based on a NIST CTR_DRBG test vector (PR = FALSE)
     */
    RUNIT_API(mbedtls_ctr_drbg_init(pCtx));

    test_offset = 0;
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_seed_entropy_len(pCtx, runIt_ctrDrbgSelfTestEntropy, (void * ) entropy_source_nopr, nonce_pers_nopr, 16, 32) == 0);
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_random(pCtx, buf, 16) == 0);
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_reseed(pCtx, NULL, 0 ) == 0);
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_random(pCtx, buf, 16) == 0);
    RUNIT_ASSERT(memcmp(buf, result_nopr, 16) == 0);

bail:
    RUNIT_API(mbedtls_ctr_drbg_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);

    RUNIT_SUB_TEST_RESULT(TEST_NAME);
    return rc;
}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_ctrDrbgTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    const char* TEST_NAME = "CTR-DRBG";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_ctrDrbgEntropyUsageTest() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_ctrDrbgVectorTest() == RUNIT_ERROR__OK);

bail:
    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;

}
#endif /* MBEDTLS_CTR_DRBG_C */
