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
#include "mbedtls/md.h"
#include "cc_general_defs.h"
#include "mbedtls/cmac.h"
/* pal */
#include "test_pal_mem.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"

/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static RunItError_t runIt_hmac(void);
static RunItError_t runIt_cmac(void);
static RunItError_t runIt_cmacProfiling(size_t data_len);

/************************************************************
 *
 * static functions
 *
 ************************************************************/
static RunItError_t runIt_hmac(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    static uint8_t hmacTest_Key[] = { 0x15, 0xb2, 0x9a, 0xd8, 0xae, 0x2a, 0xad, 0x73, 0xa7, 0x26, 0x43, 0x50, 0x70, 0xe8, 0xe9, 0xda, 0x9b, 0x47, 0x69, 0xc3, 0xe3, 0xa4, 0xee, 0x99, 0x6e, 0x20, 0x6a, 0x9b, 0x4f, 0x0c, 0x35, 0xca, 0x4f, 0xa2, 0xf7, 0x43, 0xed, 0xf2, 0xc7, 0xcb, 0xa3, 0x1e, 0x94, 0xac, 0x6b, 0xca, 0xc4, 0xc0, 0x82, 0xcf, 0x1c, 0xcb, 0x6c, 0x2f, 0xe0, 0x0d, 0x38, 0x4e, 0x3b, 0x18, 0x05, 0x5f, 0xe0, 0xe0 };
    static const uint16_t hmacTest_KeySize = sizeof(hmacTest_Key);
    static uint8_t hmacTest_InputData[] = { 0x99, 0xfd, 0x18, 0xa3, 0x5d, 0x50, 0x81, 0x84, 0xa6, 0xf3, 0x61, 0xc6, 0x7c, 0xd9, 0xb1, 0x0b, 0x4c, 0xd1, 0xd8, 0xb2, 0x46, 0x57, 0x2a, 0x4d, 0x03, 0xb0, 0xae, 0x55, 0x6b, 0x36, 0x24, 0x1d, 0xd6, 0xf0, 0x46, 0x05, 0x71, 0x65, 0x4f, 0xf0, 0xe4, 0xb2, 0xba, 0xf8, 0x31, 0xdb, 0x4c, 0x60, 0xdf, 0x5f, 0x54, 0xc9, 0x59, 0x0f, 0x32, 0xa9, 0x91, 0x1f, 0x16, 0xfa, 0xe8, 0x7e, 0x0a, 0x2f, 0x52 };
    static const uint32_t hmacTest_InputDataSize = sizeof(hmacTest_InputData);
    static CCHashResultBuf_t hmacTest_ExpOutData = { 0xE0903CC8, 0x24C89469, 0x71B12528, 0x6DEFD88C, 0xF662C7FC, 0x971C4DD1, 0x5755CB85, 0x8E72FD6F };
    static const uint32_t hmacTest_ExpOutDataSize = CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES * sizeof(uint8_t);

    uint8_t *pDataInBuff = NULL;
    uint8_t *pKey = NULL;

    RunItPtr dataInBuffPtr;
    RunItPtr keyPtr;

    const mbedtls_md_info_t *md_info = NULL;
    mbedtls_md_context_t ctx;
    unsigned char hmacOutBuff[64] = { 0 };

    const char* TEST_NAME = "HMAC";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_AND_COPY(dataInBuffPtr, pDataInBuff, hmacTest_InputData, hmacTest_InputDataSize);
    ALLOC_AND_COPY(keyPtr, pKey, hmacTest_Key, hmacTest_KeySize);

    /***************************************************************************************
     *
     *                  Integrated operation
     *
     ***************************************************************************************/
    RUNIT_API_ASSIGNMENT(md_info, mbedtls_md_info_from_string(HashAlgMode2mbedtlsString[CC_HASH_SHA256_mode]));
    RUNIT_ASSERT(md_info != NULL);

    RUNIT_ASSERT_WITH_RESULT(mbedtls_md_hmac( md_info,
                                              pKey,
                                              hmacTest_KeySize,
                                              pDataInBuff,
                                              hmacTest_InputDataSize,
                                              hmacOutBuff ), CC_OK);

    RUNIT_ASSERT(memcmp(hmacOutBuff, hmacTest_ExpOutData, hmacTest_ExpOutDataSize) == 0);


    /***************************************************************************************
     *
     *                  Non integrated operation
     *
     ***************************************************************************************/
    memset(hmacOutBuff, 0, 64);

    RUNIT_API(mbedtls_md_init(&ctx));

    RUNIT_ASSERT_WITH_RESULT(mbedtls_md_setup(&ctx, md_info, 1), CC_OK); // 1 = HMAC

    RUNIT_ASSERT_WITH_RESULT(mbedtls_md_hmac_starts(&ctx, pKey, hmacTest_KeySize), CC_OK);

    RUNIT_ASSERT_WITH_RESULT(mbedtls_md_hmac_update(&ctx, pDataInBuff, hmacTest_InputDataSize), CC_OK);

    RUNIT_ASSERT_WITH_RESULT(mbedtls_md_hmac_finish(&ctx, hmacOutBuff), CC_OK);

    RUNIT_ASSERT(memcmp(hmacOutBuff, hmacTest_ExpOutData, hmacTest_ExpOutDataSize) == 0);
    /***************************************************************************************/

bail:

    FREE_IF_NOT_NULL(dataInBuffPtr);
    FREE_IF_NOT_NULL(keyPtr);

    mbedtls_md_free(&ctx);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "MODE[SHA256] KEY[%"PRIu32"b] PLAIN[%"PRIu32"B] ",
                                   (uint32_t)hmacTest_KeySize * 8, (uint32_t)hmacTest_InputDataSize);
    return rc;
}

/**
 * This test is meant to measure the execution times of the API.
 * This test doesn't compare the results with expected result.
 * I uses the return value to verify everything went well.
 *
 * @param block1_len
 * @return
 */
static RunItError_t runIt_cmacProfiling(size_t data_len)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    mbedtls_cipher_type_t cipher_type = MBEDTLS_CIPHER_AES_128_ECB;
    const char *key_string = "2b7e151628aed2a6abf7158809cf4f3c";
    int keybits = 128;

    unsigned char *key = NULL;
    unsigned char *data = NULL;
    const mbedtls_cipher_info_t *cipher_info = NULL;
    mbedtls_cipher_context_t *ctx = NULL;
    unsigned char *output = NULL;
    char testParam[PARAM_LEN] = { 0 };

    RunItPtr ketPtr;
    RunItPtr dataPtr;
    RunItPtr ctxPtr;
    RunItPtr outputPtr;

    const char* TEST_NAME = "CMAC-Profile";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC(ketPtr, key, keybits / 8);
    ALLOC(dataPtr, data, data_len);
    ALLOC_STRUCT(mbedtls_cipher_context_t, ctxPtr, ctx);
    ALLOC(outputPtr, output, MBEDTLS_CIPHER_BLKSIZE_MAX);

    snprintf(testParam, PARAM_LEN - 1, "%"PRIu32"B", (uint32_t)data_len);

    /* Convert the test parameters to binary data */
    runIt_unhexify( key, key_string );

    /* this is temporary patch */
    RUNIT_API(mbedtls_cipher_init( ctx ));

    /* Set up */
    RUNIT_API_ASSIGNMENT(cipher_info, mbedtls_cipher_info_from_type(cipher_type));
    RUNIT_ASSERT(cipher_info != NULL);

    RUNIT_ASSERT_API(mbedtls_cipher_setup(ctx, cipher_info) == 0);

    RUNIT_ASSERT_API(mbedtls_cipher_cmac_starts(ctx, (const unsigned char* )key, keybits) == 0);

    RUNIT_ASSERT_API(mbedtls_cipher_cmac_reset(ctx) == 0);

    /* Multiple partial and complete blocks. A negative length means skip the
     * update operation */
    RUNIT_ASSERT_W_PARAM(testParam, mbedtls_cipher_cmac_update(ctx, (unsigned char* )data, data_len) == 0);

    RUNIT_ASSERT_API(mbedtls_cipher_cmac_finish(ctx, output) == 0);

bail:

    RUNIT_API(mbedtls_cipher_free( ctx ));

    FREE_IF_NOT_NULL(ketPtr);
    FREE_IF_NOT_NULL(dataPtr);
    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(outputPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "TYPE[AES_128_ECB] KEY[%"PRIu32"b] PLAIN[%"PRIu32"B] ",
                                   (uint32_t)keybits, (uint32_t)data_len);
    return rc;
}

static RunItError_t runIt_cmac(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    mbedtls_cipher_type_t cipher_type = MBEDTLS_CIPHER_AES_128_ECB;
    const char *key_string = "2b7e151628aed2a6abf7158809cf4f3c";
    int keybits = 128;
    int block_size = 16;
    const char *block1_string = "6BC1BEE22E409F96E93D7E117393172AAE2D8A571E03AC9C9EB76FAC45AF8E5130C81C46A35CE411E5FBC1191A0A52EFF69F2445DF4F9B17AD2B417BE66C3710";
    int block1_len = 64;
    const char *expected_result_string = "51f0bebf7e3b9d92fc49741779363cfe";

    unsigned char *key = NULL;
    unsigned char *block1 = NULL;
    unsigned char *expected_result = NULL;
    const mbedtls_cipher_info_t *cipher_info = NULL;
    mbedtls_cipher_context_t *ctx = NULL;
    unsigned char *output = NULL;

    RunItPtr ketPtr;
    RunItPtr block1Ptr;
    RunItPtr expectedResultPtr;
    RunItPtr ctxPtr;
    RunItPtr outputPtr;

    const char* TEST_NAME = "CMAC";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC(ketPtr, key, keybits / 8);
    ALLOC(block1Ptr, block1, block1_len);
    ALLOC(expectedResultPtr, expected_result, block_size);
    ALLOC_STRUCT(mbedtls_cipher_context_t, ctxPtr, ctx);
    ALLOC(outputPtr, output, MBEDTLS_CIPHER_BLKSIZE_MAX);

    /* Convert the test parameters to binary data */
    runIt_unhexify( key, key_string );
    runIt_unhexify( block1, block1_string );
    runIt_unhexify( expected_result, expected_result_string );

    RUNIT_API(mbedtls_cipher_init( ctx ));

    /* Set up */
    RUNIT_API_ASSIGNMENT(cipher_info, mbedtls_cipher_info_from_type(cipher_type));
    RUNIT_ASSERT(cipher_info != NULL);

    /*
     * Non Integrated
     */
    RUNIT_ASSERT_API(mbedtls_cipher_setup(ctx, cipher_info) == 0);

    RUNIT_ASSERT_API(mbedtls_cipher_cmac_starts(ctx, (const unsigned char* )key, keybits) == 0);

    /* this is temporary patch */
    RUNIT_ASSERT_API(mbedtls_cipher_cmac_reset(ctx) == 0);

    /* Multiple partial and complete blocks. A negative length means skip the
     * update operation */
    RUNIT_ASSERT_API(mbedtls_cipher_cmac_update(ctx, (unsigned char* )block1, block1_len) == 0);

    RUNIT_ASSERT_API(mbedtls_cipher_cmac_finish(ctx, output) == 0);

    RUNIT_PRINT_BUF(output, MBEDTLS_CIPHER_BLKSIZE_MAX, "output");
    RUNIT_PRINT_BUF(expected_result, MBEDTLS_CIPHER_BLKSIZE_MAX, "expected_result");

    RUNIT_ASSERT(memcmp(output, expected_result, block_size) == 0);

    /*
     * Integrated
     */
    memset(output, 0, MBEDTLS_CIPHER_BLKSIZE_MAX);

    RUNIT_ASSERT_API(mbedtls_cipher_cmac(cipher_info,
                        key, keybits,
                        block1, block1_len,
                        output) == 0);

    RUNIT_ASSERT(memcmp(output, expected_result, block_size) == 0);

bail:

    RUNIT_API(mbedtls_cipher_free( ctx ));

    FREE_IF_NOT_NULL(ketPtr);
    FREE_IF_NOT_NULL(block1Ptr);
    FREE_IF_NOT_NULL(expectedResultPtr);
    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(outputPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "TYPE[AES_128_ECB] KEY[%"PRIu32"b] PLAIN[%"PRIu32"B] ",
                                   (uint32_t)keybits, (uint32_t)block1_len);
    return rc;
}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_macTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    const char* TEST_NAME = "HMAC";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_hmac() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_cmac() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_cmacProfiling(16) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_cmacProfiling(128) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_cmacProfiling(1024) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_cmacProfiling(8192) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_cmacProfiling(65535) == RUNIT_ERROR__OK);

bail:

    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;
}
