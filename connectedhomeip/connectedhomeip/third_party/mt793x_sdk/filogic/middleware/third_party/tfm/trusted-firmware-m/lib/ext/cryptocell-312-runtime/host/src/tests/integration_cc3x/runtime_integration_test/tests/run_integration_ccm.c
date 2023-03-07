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
#include "mbedtls/ccm.h"
#include "mbedtls_cc_ccm_star.h"
#include "mbedtls_ccm_common.h"
#include "mbedtls/cipher.h"
#include "mbedtls/timing.h"
#include "mbedtls/ctr_drbg.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"

#define AESCCM_DEFAULT_NONCE_SIZE 16
/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static RunItError_t runIt_ccm(void);

/************************************************************
 *
 * static functions
 *
 ************************************************************/
static RunItError_t runIt_ccmStarEncOnly(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CCM.pdf */
    static const uint8_t KEY[16] = { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f };
    const uint8_t AD[20] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13 };
    const uint8_t MSG[24] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
    const uint8_t RES[] = {0x5a, 0x21, 0xaf, 0x13, 0x2a, 0xf4, 0x02, 0x43, 0x31, 0xba, 0x7c, 0x4d, 0x7c, 0x3d, 0xcc, 0x0e, 0x19, 0x3e, 0x4d, 0x46, 0xa5, 0xc9, 0xa6, 0x82, };

    static const size_t TAG_LEN = 0;

    uint32_t FrameCounter = 5;
    unsigned char nonceBuff[MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES];
    uint8_t srcAddr[16] = { 0x39, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e };

    uint8_t buf[32];
    mbedtls_ccm_context *pCtx = NULL;
    RunItPtr ctxPtr;

    const char* TEST_NAME = "CCM STAR - ENC ONLY";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_ccm_context, ctxPtr, pCtx);

    /* generate random buffer */
    RUNIT_ASSERT_API(mbedtls_ccm_star_nonce_generate((uint8_t*)srcAddr, FrameCounter, TAG_LEN, nonceBuff) == 0);

    /* Initialize ccm engine */
    RUNIT_API(mbedtls_ccm_init(pCtx));

    /* set key into context */
    RUNIT_ASSERT_API(mbedtls_ccm_setkey(pCtx, MBEDTLS_CIPHER_ID_AES, KEY, 8 * sizeof(KEY)) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_ccm_star_encrypt_and_tag(pCtx, sizeof(MSG), nonceBuff, MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES, AD, sizeof(AD), MSG, buf, buf + sizeof(MSG), TAG_LEN) == 0);

    RUNIT_PRINT_BUF(buf, sizeof(MSG) + TAG_LEN, "signed");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, RES, sizeof(MSG) + TAG_LEN) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_ccm_star_auth_decrypt(pCtx, sizeof(MSG), nonceBuff, MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES, AD, sizeof(AD), RES, buf, RES + sizeof(MSG), TAG_LEN) == 0);

    RUNIT_PRINT_BUF(buf, sizeof(MSG) + TAG_LEN, "verified");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, MSG, sizeof(MSG)) == 0);

bail:
    RUNIT_API(mbedtls_ccm_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b] AAD[%"PRIu32"B] MSG[%"PRIu32"B] TAG[%"PRIu32"B]",
                                   (uint32_t)8 * sizeof(KEY), (uint32_t)sizeof(AD), (uint32_t)sizeof(MSG), (uint32_t)TAG_LEN);
    return rc;
}

static RunItError_t runIt_ccmStar(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CCM.pdf */
    static const uint8_t KEY[16] = { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f };
    const uint8_t AD[20] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13 };
    const uint8_t MSG[24] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
    const uint8_t RES[] = {0x07, 0x11, 0x7e, 0x35, 0x39, 0xb2, 0xed, 0x4c, 0x84, 0x91, 0x66, 0x07, 0x0b, 0x33, 0xe6, 0x39, 0x0c, 0xf8, 0xda, 0xcc, 0x37, 0x44, 0x00, 0x9d, 0xb1, 0xac, 0x43, 0x81, 0x81, 0x8e, 0x17, 0x6d, 0x54, 0x63, 0x78, 0x4b, 0xd7, 0xd0, 0xc5, 0x46, };

    static const size_t TAG_LEN = 16;

    uint32_t FrameCounter = 5;
    unsigned char nonceBuff[MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES];
    uint8_t srcAddr[16] = { 0x39, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e };

    uint8_t buf[40];
    mbedtls_ccm_context *pCtx = NULL;
    RunItPtr ctxPtr;

    const char* TEST_NAME = "CCM STAR";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_ccm_context, ctxPtr, pCtx);

    /* generate random buffer */
    RUNIT_ASSERT_API(mbedtls_ccm_star_nonce_generate((uint8_t*)srcAddr, FrameCounter, TAG_LEN, nonceBuff) == 0);

    /* Initialize ccm engine */
    RUNIT_API(mbedtls_ccm_init(pCtx));

    /* set key into context */
    RUNIT_ASSERT_API(mbedtls_ccm_setkey(pCtx, MBEDTLS_CIPHER_ID_AES, KEY, 8 * sizeof(KEY)) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_ccm_star_encrypt_and_tag(pCtx, sizeof(MSG), nonceBuff, MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES, AD, sizeof(AD), MSG, buf, buf + sizeof(MSG), TAG_LEN) == 0);

    RUNIT_PRINT_BUF(buf, sizeof(MSG) + TAG_LEN, "signed");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, RES, sizeof(MSG) + TAG_LEN) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_ccm_star_auth_decrypt(pCtx, sizeof(MSG), nonceBuff, MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES, AD, sizeof(AD), RES, buf, RES + sizeof(MSG), TAG_LEN) == 0);

    RUNIT_PRINT_BUF(buf, sizeof(MSG) + TAG_LEN, "verified");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, MSG, sizeof(MSG)) == 0);

bail:
    RUNIT_API(mbedtls_ccm_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b] AAD[%"PRIu32"B] MSG[%"PRIu32"B] TAG[%"PRIu32"B]",
                                   (uint32_t)8 * sizeof(KEY), (uint32_t)sizeof(AD), (uint32_t)sizeof(MSG), (uint32_t)TAG_LEN);
    return rc;
}

static RunItError_t runIt_ccm(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if defined(MBEDTLS_CCM_C)
    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CCM.pdf */
    static const uint8_t KEY[16] = { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f };
    static const uint8_t IV[12] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b };
    const uint8_t AD[20] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13 };
    const uint8_t MSG[24] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
    const uint8_t RES[32] = { 0xE3, 0xB2, 0x01, 0xA9, 0xF5, 0xB7, 0x1A, 0x7A, 0x9B, 0x1C, 0xEA, 0xEC, 0xCD, 0x97, 0xE7, 0x0B, 0x61, 0x76, 0xAA, 0xD9, 0xA4, 0x42, 0x8A, 0xA5, 0x48, 0x43, 0x92, 0xFB, 0xC1, 0xB0, 0x99, 0x51 };
    static const size_t TAG_LEN = 8;

    uint8_t buf[32];
    RunItPtr ctxPtr;
    mbedtls_ccm_context *pCtx = NULL;

    const char* TEST_NAME = "CCM";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_ccm_context, ctxPtr, pCtx);

    /* Initialize ccm engine */
    RUNIT_API(mbedtls_ccm_init(pCtx));

    /* set key into context */
    RUNIT_ASSERT_API(mbedtls_ccm_setkey(pCtx, MBEDTLS_CIPHER_ID_AES, KEY, 8 * sizeof(KEY)) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_ccm_encrypt_and_tag(pCtx, sizeof(MSG), IV, sizeof(IV), AD, sizeof(AD), MSG, buf, buf + sizeof(MSG), TAG_LEN) == 0);

    RUNIT_PRINT_BUF(buf, sizeof(MSG) + TAG_LEN, "signed");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, RES, sizeof(MSG) + TAG_LEN) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_ccm_auth_decrypt(pCtx, sizeof(MSG), IV, sizeof(IV), AD, sizeof(AD), RES, buf, RES + sizeof(MSG), TAG_LEN) == 0);

    RUNIT_PRINT_BUF(buf, sizeof(MSG) + TAG_LEN, "verified");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, MSG, sizeof(MSG)) == 0);

bail:
    RUNIT_API(mbedtls_ccm_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b] AAD[%"PRIu32"B] MSG[%"PRIu32"B]",
                                   (uint32_t)8 * sizeof(KEY), (uint32_t)sizeof(AD), (uint32_t)sizeof(MSG));
#endif /* MBEDTLS_CCM_C */
    return rc;
}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_ccmTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    const char* TEST_NAME = "CCM";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_ccm() == RUNIT_ERROR__OK);
#ifdef MBEDTLS_CCM_ALT
    RUNIT_ASSERT(runIt_ccmStar() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_ccmStarEncOnly() == RUNIT_ERROR__OK);
#else
    (void)runIt_ccmStar;
    (void)runIt_ccmStarEncOnly;
#endif

bail:

    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;
}
