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
#include "mbedtls/hkdf.h"
#include "mbedtls/md.h"
#include "mbedtls/timing.h"

/* CC */
#include "cc_aes_defs.h"
#include "cc_pal_types.h"
#include "mbedtls_cc_util_defs.h"
#include "cc_util_error.h"
#include "mbedtls_cc_util_key_derivation.h"
#include "cc_hash_defs.h"

/* pal */
#include "test_pal_mem.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"
#include "run_integration_otp.h"

#ifdef RUNIT_PIE_ENABLED
/* include sbrom data file to determine whether we are running system flows */
#include "bsv_integration_data_def.h"
#endif /* RUNIT_PIE_ENABLED */

#ifndef RUNIT_KDF_SKIP_COMPARE
#define RUNIT_KDF_SKIP_COMPARE 1
#endif

#ifndef RUNIT_KDF_ROOT_KEY_SKIP_TEST
#define RUNIT_KDF_ROOT_KEY_SKIP_TEST 1
#endif

/************************************************************
 *
 * static functions prototypes
 *
 ************************************************************/
static RunItError_t runIt_hmacKeyDerivTest(void);
static RunItError_t runIt_cmacKeyDerivTest(void);
static RunItError_t runIt_cmacRootKeyDerivTest(void);
static RunItError_t runIt_hkdfKeyDerivTest(void);

/************************************************************
 *
 * static functions
 *
 ************************************************************/
static RunItError_t runIt_hmacKeyDerivTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    static const uint8_t userKeyDataOut[] = { 0xba, 0x58, 0xa5, 0xc5, 0x88, 0x59, 0x88, 0x37, 0x72, 0xb6, 0x93, 0x68, 0xee, 0x53, 0xf4, 0x7c  };
    static const uint8_t label[] = { 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xEF, 0x3E, 0x3E, 0xE8, 0xEF };
    static const uint8_t context[] = { 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3D, 0xFB, 0xAF, 0x7F };
    static const uint8_t userKey[] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

    uint8_t* pLabel = NULL;
    uint8_t* pContext = NULL;
    uint8_t* pUserKey = NULL;
    uint8_t* pDataOut = NULL;

    RunItPtr labelPtr;
    RunItPtr contextPtr;
    RunItPtr userKeyPtr;
    RunItPtr dataOutPtr;

    mbedtls_util_keydata keyData = {0};
    uint32_t dataOutLength = sizeof(userKeyDataOut);

    const char* TEST_NAME = "HMAC Key Derivation";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_AND_COPY(labelPtr, pLabel, label, sizeof(label));
    ALLOC_AND_COPY(contextPtr, pContext, context, sizeof(context));
    ALLOC_AND_COPY(userKeyPtr, pUserKey, userKey, sizeof(userKey));
    ALLOC(dataOutPtr, pDataOut, dataOutLength);

    keyData.pKey = pUserKey;
    keyData.keySize = sizeof(userKey);

    RUNIT_ASSERT_WITH_RESULT(mbedtls_util_key_derivation_hmac(CC_UTIL_USER_KEY,
                                                              (void*)&keyData,
                                                              CC_HASH_SHA256_mode,
                                                              (uint8_t *)pLabel,
                                                              sizeof(label),
                                                              (uint8_t *)pContext,
                                                              sizeof(context),
                                                              (uint8_t *)pDataOut,
                                                              dataOutLength),
                             CC_UTIL_OK);

    RUNIT_PRINT_BUF(keyData.pKey, keyData.keySize, "keyData.pKey");
    RUNIT_PRINT_BUF(label, sizeof(label), "label");
    RUNIT_PRINT_BUF(context, sizeof(context), "context");
    RUNIT_PRINT_BUF(pDataOut, dataOutLength, "dataOutBuff");

    RUNIT_ASSERT(memcmp(pDataOut, userKeyDataOut, dataOutLength) == 0);

bail:

    FREE_IF_NOT_NULL(labelPtr);
    FREE_IF_NOT_NULL(contextPtr);
    FREE_IF_NOT_NULL(userKeyPtr);
    FREE_IF_NOT_NULL(dataOutPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "MODE[SHA256] KEY[%ub] CONTEXT[%uB] LABEL[%uB]", keyData.keySize * 8, sizeof(context), sizeof(label));
    return rc;
}

static RunItError_t runIt_cmacKeyDerivTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    static uint8_t context[] = { 0x54,0x45,0x53,0x54 };
    static uint8_t label[] = { 0x55,0x53,0x45,0x52,0xF2,0x4B,0x45,0x59 };
    static const uint8_t userKey[] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    static const uint8_t userKeyDataOut[] = { 0x2B, 0x05, 0xE1, 0xF8, 0xF0, 0x58, 0x78, 0xAC, 0x41, 0xB0, 0xB5, 0x5D, 0xB0, 0x42, 0x9E, 0x5C };

    uint8_t* pLabel = NULL;
    uint8_t* pContext = NULL;
    uint8_t* pUserKey = NULL;
    uint8_t* pDataOut = NULL;

    RunItPtr labelPtr;
    RunItPtr contextPtr;
    RunItPtr userKeyPtr;
    RunItPtr dataOutPtr;

    mbedtls_util_keydata keyData={0};
    uint32_t dataOutLength = sizeof(userKeyDataOut);

    const char* TEST_NAME = "CMAC Key Derivation";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_AND_COPY(labelPtr, pLabel, label, sizeof(label));
    ALLOC_AND_COPY(contextPtr, pContext, context, sizeof(context));
    ALLOC_AND_COPY(userKeyPtr, pUserKey, userKey, sizeof(userKey));
    ALLOC(dataOutPtr, pDataOut, dataOutLength);

    keyData.pKey = (uint8_t*)pUserKey;
    keyData.keySize = sizeof(userKey);

    RUNIT_ASSERT_WITH_RESULT(mbedtls_util_key_derivation_cmac(CC_UTIL_USER_KEY,
                                                              (void*)&keyData,
                                                              (uint8_t *)pLabel,
                                                              sizeof(label),
                                                              (uint8_t *)pContext,
                                                              sizeof(context),
                                                              (uint8_t *)pDataOut,
                                                              dataOutLength),
                             CC_UTIL_OK);

    RUNIT_PRINT_BUF(pDataOut, dataOutLength, "pDataOut");

    RUNIT_ASSERT(memcmp(pDataOut, userKeyDataOut, dataOutLength) == 0);

bail:

    FREE_IF_NOT_NULL(labelPtr);
    FREE_IF_NOT_NULL(contextPtr);
    FREE_IF_NOT_NULL(userKeyPtr);
    FREE_IF_NOT_NULL(dataOutPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b] CONTEXT[%"PRIu32"B] LABEL[%"PRIu32"B]",
                                   (uint32_t)keyData.keySize * 8, (uint32_t)sizeof(context), (uint32_t)sizeof(label));
    return rc;
}

static RunItError_t runIt_cmacRootKeyDerivTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

#if !RUNIT_KDF_ROOT_KEY_SKIP_TEST
    static uint8_t context[] = { 0x54,0x45,0x53,0x54 };
    static uint8_t label[] = { 0x55,0x53,0x45,0x52,0xF2,0x4B,0x45,0x59 };
    static const uint8_t userKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    static const uint8_t userKeyDataOut[] = { RUNIT_KDF_RESULT };

    uint8_t* pLabel = NULL;
    uint8_t* pContext = NULL;
    uint8_t* pUserKey = NULL;
    uint8_t* pDataOut = NULL;
    uint8_t* pDataOutRef = NULL;

    RunItPtr labelPtr;
    RunItPtr contextPtr;
    RunItPtr userKeyPtr;
    RunItPtr dataOutPtr;
    RunItPtr dataOutRefPtr;

    mbedtls_util_keydata keyData;
    uint32_t dataOutLength = sizeof(userKeyDataOut);

    const char* TEST_NAME = "CMAC Root Key Derivation";
    RUNIT_SUB_TEST_START(TEST_NAME);

    (void)userKeyDataOut;

    ALLOC_AND_COPY(labelPtr, pLabel, label, sizeof(label));
    ALLOC_AND_COPY(contextPtr, pContext, context, sizeof(context));
    ALLOC_AND_COPY(userKeyPtr, pUserKey, userKey, sizeof(userKey));
    ALLOC(dataOutPtr, pDataOut, dataOutLength);
    ALLOC(dataOutRefPtr, pDataOutRef, dataOutLength);

    keyData.pKey = (uint8_t*)pUserKey;
    keyData.keySize = sizeof(userKey);

    RUNIT_ASSERT_WITH_RESULT(mbedtls_util_key_derivation_cmac(CC_UTIL_USER_KEY,
                                                              (void*)&keyData,
                                                              (uint8_t *)pLabel,
                                                              sizeof(label),
                                                              (uint8_t *)pContext,
                                                              sizeof(context),
                                                              (uint8_t *)pDataOutRef,
                                                              dataOutLength),
                             CC_UTIL_OK);

    RUNIT_ASSERT_WITH_RESULT(mbedtls_util_key_derivation_cmac(CC_UTIL_ROOT_KEY,
                                                              (void*)&keyData,
                                                              (uint8_t *)pLabel,
                                                              sizeof(label),
                                                              (uint8_t *)pContext,
                                                              sizeof(context),
                                                              (uint8_t *)pDataOut,
                                                              dataOutLength),
                             CC_UTIL_OK);

    RUNIT_PRINT_BUF(pDataOutRef, dataOutLength, "pDataOutRef");
    RUNIT_PRINT_BUF(pDataOut, dataOutLength, "pDataOut");


#if RUNIT_KDF_COMPARE_EXPECTED_RESULT
    RUNIT_ASSERT(memcmp(pDataOut, pDataOutRef, dataOutLength) != 0);
#else
    RUNIT_ASSERT(memcmp(pDataOut, pDataOutRef, dataOutLength) == 0);
#endif

    if (sizeof(userKeyDataOut) != 1)
        RUNIT_ASSERT(memcmp(pDataOut, userKeyDataOut, dataOutLength) == 0);

bail:

    FREE_IF_NOT_NULL(labelPtr);
    FREE_IF_NOT_NULL(contextPtr);
    FREE_IF_NOT_NULL(userKeyPtr);
    FREE_IF_NOT_NULL(dataOutPtr);
    FREE_IF_NOT_NULL(dataOutRefPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b] CONTEXT[%"PRIu32"B] LABEL[%"PRIu32"B]",
                                   (uint32_t)keyData.keySize * 8, (uint32_t)sizeof(context), (uint32_t)sizeof(label));

#endif /* !RUNIT_KDF_ROOT_KEY_SKIP_TEST */
    return rc;
}

static RunItError_t runIt_hkdfKeyDerivTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    static uint8_t hkdf_IKM[] = { 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b };
    static const uint32_t hkdf_IKMSize = 22;
    static uint8_t hkdf_Salt[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c };
    static const uint32_t hkdf_SaltSize = 13;
    static uint8_t hkdf_Info[] = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9 };
    static const uint32_t hkdf_InfoSize = 10;
    static const uint8_t hkdf_ExpectOKM[] = { 0x3c, 0xb2, 0x5f, 0x25, 0xfa, 0xac, 0xd5, 0x7a, 0x90, 0x43, 0x4f, 0x64, 0xd0, 0x36, 0x2f, 0x2a, 0x2d, 0x2d, 0x0a, 0x90, 0xcf, 0x1a, 0x5a, 0x4c, 0x5d, 0xb0, 0x2d, 0x56, 0xec, 0xc4, 0xc5, 0xbf, 0x34, 0x00, 0x72, 0x08, 0xd5, 0xb8, 0x87, 0x18, 0x58, 0x65 };
    static const uint32_t hkdf_LSize = 42;
    const mbedtls_md_info_t *md = mbedtls_md_info_from_type( MBEDTLS_MD_SHA256 );
    uint8_t* pOKMDataBuff = NULL;

    RunItPtr okmDataBuff;

    const char* TEST_NAME = "HKDF Key Derivation";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC(okmDataBuff, pOKMDataBuff, 4096);

    RUNIT_ASSERT_WITH_RESULT(mbedtls_hkdf(md,
                                         hkdf_Salt,
                                         (size_t)hkdf_SaltSize,
                                         hkdf_IKM,
                                         hkdf_IKMSize,
                                         hkdf_Info,
                                         hkdf_InfoSize,
                                         pOKMDataBuff,
                                         hkdf_LSize),
                             CC_OK);

    /* compare the result and print result*/
    RUNIT_ASSERT(memcmp(pOKMDataBuff, hkdf_ExpectOKM, hkdf_LSize) == 0);

bail:
    FREE_IF_NOT_NULL(okmDataBuff);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "MODE[SHA256] IKM[%uB] SALT[%uB] INFO[%uB] OKM[%uB]",
                                   (unsigned int)hkdf_IKMSize, (unsigned int)hkdf_SaltSize,
                                   (unsigned int)hkdf_InfoSize, (unsigned int)hkdf_LSize);
    return rc;
}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_keyDerivationTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    const char* TEST_NAME = "Key Derivation";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_hmacKeyDerivTest() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_cmacKeyDerivTest() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_cmacRootKeyDerivTest() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_hkdfKeyDerivTest() == RUNIT_ERROR__OK);

bail:

    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;
}


