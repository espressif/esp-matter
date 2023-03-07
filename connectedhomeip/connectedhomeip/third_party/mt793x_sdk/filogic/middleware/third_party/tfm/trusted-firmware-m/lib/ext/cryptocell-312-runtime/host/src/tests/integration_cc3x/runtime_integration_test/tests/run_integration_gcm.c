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
#include "mbedtls/gcm.h"
#include "mbedtls/cipher.h"
#include "mbedtls/timing.h"

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
static RunItError_t runIt_gcmProfiling(size_t data_len);
static RunItError_t runIt_gcm(void);

/************************************************************
 *
 * static functions
 *
 ************************************************************/

static RunItError_t runIt_gcmProfiling(size_t data_len)
{

    RunItError_t rc = RUNIT_ERROR__OK;

#if defined(MBEDTLS_GCM_C)
    static const uint8_t key[] = {0xFE,0xFF,0xE9,0x92,0x86,0x65,0x73,0x1C,0x6D,0x6A,0x8F,0x94,0x67,0x30,0x83,0x08};
    static const uint8_t iv[] = {0xCA,0xFE,0xBA,0xBE,0xFA,0xCE,0xDB,0xAD,0xDE,0xCA,0xF8,0x88};
    static const size_t iv_len = sizeof(iv);
    static const uint8_t additional[] = {0x3A,0xD7,0x7B,0xB4,0x0D,0x7A,0x36,0x60,0xA8,0x9E,0xCA,0xF3,0x24,0x66,0xEF,0x97};
    static const size_t add_len = sizeof(additional);
    static const size_t tag_len = 16;
    static const int key_len = sizeof(key) * 8;

    RunItPtr ctxPtr;
    mbedtls_gcm_context *pCtx = NULL;

    RunItPtr cipherPtr, plainBufPtr, tagBufPtr, ivBufPtr, addBufPtr;
    uint8_t *pCipher = NULL, *pPlainBuf = NULL, *pTagBuf = NULL, *pIvBuf = NULL, *pAddBuf = NULL;
    char testParam[PARAM_LEN] = { 0 };

    const char* TEST_NAME = "AES-GCM Profiling";
    RUNIT_SUB_TEST_START(TEST_NAME);

    snprintf(testParam, PARAM_LEN - 1, "%"PRIu32"B", (uint32_t)data_len);

    ALLOC_STRUCT(mbedtls_gcm_context, ctxPtr, pCtx);
    ALLOC(cipherPtr, pCipher, data_len);
    ALLOC(plainBufPtr, pPlainBuf, data_len);
    ALLOC(tagBufPtr, pTagBuf, tag_len);
    ALLOC_AND_COPY(ivBufPtr, pIvBuf, iv, iv_len);
    ALLOC_AND_COPY(addBufPtr, pAddBuf, additional, add_len);



    /***************************************************************
     *                     ENCRYPTION                              *
     ***************************************************************/

    RUNIT_ASSERT(runIt_buildRandomBuffer(pPlainBuf, data_len) == 0);

    RUNIT_API(mbedtls_gcm_init(pCtx));

    RUNIT_API(mbedtls_gcm_setkey(pCtx, MBEDTLS_CIPHER_ID_AES, key, key_len));

    RUNIT_ASSERT_WITH_RESULT(mbedtls_gcm_crypt_and_tag(pCtx,
                                                       MBEDTLS_GCM_ENCRYPT,
                                                       data_len,
                                                       pIvBuf,
                                                       iv_len,
                                                       pAddBuf,
                                                       add_len,
                                                       pPlainBuf,
                                                       pCipher,
                                                       tag_len,
                                                       pTagBuf), 0);

bail:
    RUNIT_API(mbedtls_gcm_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(cipherPtr);
    FREE_IF_NOT_NULL(plainBufPtr);
    FREE_IF_NOT_NULL(tagBufPtr);
    FREE_IF_NOT_NULL(ivBufPtr);
    FREE_IF_NOT_NULL(addBufPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b] PLAIN[%"PRIu32"B] AAD[%"PRIu32"B]",
                                   (uint32_t)key_len, (uint32_t)data_len, (uint32_t)add_len);
#endif /* defined(MBEDTLS_GCM_C) */
    return rc;


}

static RunItError_t runIt_gcm(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

#if defined(MBEDTLS_GCM_C)
    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_GCM.pdf */
    static const uint8_t key[] = {0xFE,0xFF,0xE9,0x92,0x86,0x65,0x73,0x1C,0x6D,0x6A,0x8F,0x94,0x67,0x30,0x83,0x08};
    static const uint8_t iv[] = {0xCA,0xFE,0xBA,0xBE,0xFA,0xCE,0xDB,0xAD,0xDE,0xCA,0xF8,0x88};
    static const size_t iv_len = sizeof(iv);
    static const uint8_t additional[] = {0x3A,0xD7,0x7B,0xB4,0x0D,0x7A,0x36,0x60,0xA8,0x9E,0xCA,0xF3,0x24,0x66,0xEF,0x97,0xF5,0xD3,0xD5,0x85,0x03,0xB9,0x69,0x9D,0xE7,0x85,0x89,0x5A,0x96,0xFD,0xBA,0xAF,0x43,0xB1,0xCD,0x7F,0x59,0x8E,0xCE,0x23,0x88,0x1B,0x00,0xE3,0xED,0x03,0x06,0x88,0x7B,0x0C,0x78,0x5E,0x27,0xE8,0xAD,0x3F,0x82,0x23,0x20,0x71,0x04,0x72,0x5D,0xD4};
    static const size_t add_len = sizeof(additional);
    static const uint8_t pt[] = {0xD9,0x31,0x32,0x25,0xF8,0x84,0x06,0xE5,0xA5,0x59,0x09,0xC5,0xAF,0xF5,0x26,0x9A,0x86,0xA7,0xA9,0x53,0x15,0x34,0xF7,0xDA,0x2E,0x4C,0x30,0x3D,0x8A,0x31,0x8A,0x72,0x1C,0x3C,0x0C,0x95,0x95,0x68,0x09,0x53,0x2F,0xCF,0x0E,0x24,0x49,0xA6,0xB5,0x25,0xB1,0x6A,0xED,0xF5,0xAA,0x0D,0xE6,0x57,0xBA,0x63,0x7B,0x39,0x1A,0xAF,0xD2,0x55};
    static const size_t pt_len = sizeof(pt);
    static const uint8_t ct[] = {0x42,0x83,0x1E,0xC2,0x21,0x77,0x74,0x24,0x4B,0x72,0x21,0xB7,0x84,0xD0,0xD4,0x9C,0xE3,0xAA,0x21,0x2F,0x2C,0x02,0xA4,0xE0,0x35,0xC1,0x7E,0x23,0x29,0xAC,0xA1,0x2E,0x21,0xD5,0x14,0xB2,0x54,0x66,0x93,0x1C,0x7D,0x8F,0x6A,0x5A,0xAC,0x84,0xAA,0x05,0x1B,0xA3,0x0B,0x39,0x6A,0x0A,0xAC,0x97,0x3D,0x58,0xE0,0x91,0x47,0x3F,0x59,0x85};
    static const size_t ct_len = sizeof(ct);
    static const uint8_t tag[] = {0x64,0xC0,0x23,0x29,0x04,0xAF,0x39,0x8A,0x5B,0x67,0xC1,0x0B,0x53,0xA5,0x02,0x4D};
    static const size_t tag_len = sizeof(tag);
    static const int key_len = sizeof(key) * 8;

    RunItPtr ctxPtr;
    mbedtls_gcm_context *pCtx = NULL;

    RunItPtr bufPtr, cipherPtr, plainBufPtr, tagBufPtr, ivBufPtr, addBufPtr;
    uint8_t *pBuf = NULL, *pCipher = NULL, *pPlainBuf = NULL, *pTagBuf = NULL, *pIvBuf = NULL, *pAddBuf = NULL;

    const char* TEST_NAME = "AES-GCM";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_gcm_context, ctxPtr, pCtx);
    ALLOC(bufPtr, pBuf, ct_len);
    ALLOC_AND_COPY(cipherPtr, pCipher, ct, ct_len);
    ALLOC_AND_COPY(plainBufPtr, pPlainBuf, pt, pt_len);
    ALLOC(tagBufPtr, pTagBuf, tag_len);
    ALLOC_AND_COPY(ivBufPtr, pIvBuf, iv, iv_len);
    ALLOC_AND_COPY(addBufPtr, pAddBuf, additional, add_len);

    memset(pBuf, 0, sizeof(ct));

    /***************************************************************
     *                     ENCRYPTION                              *
     ***************************************************************/
    RUNIT_API(mbedtls_gcm_init(pCtx));

    RUNIT_API(mbedtls_gcm_setkey(pCtx, MBEDTLS_CIPHER_ID_AES, key, key_len));

    RUNIT_ASSERT_W_PARAM("encrypt", mbedtls_gcm_crypt_and_tag(pCtx,
                                                              MBEDTLS_GCM_ENCRYPT,
                                                              pt_len,
                                                              pIvBuf,
                                                              iv_len,
                                                              pAddBuf,
                                                              add_len,
                                                              pPlainBuf,
                                                              pBuf,
                                                              sizeof(tag),
                                                              pTagBuf) == 0);

    RUNIT_PRINT_BUF(pBuf, ct_len, "pBuf");
    RUNIT_PRINT_BUF(ct, ct_len, "ct");

    RUNIT_PRINT_BUF(pTagBuf, tag_len, "pTagBuf");
    RUNIT_PRINT_BUF(tag, tag_len, "tag");

    RUNIT_ASSERT(memcmp(pBuf, ct, pt_len) == 0);
    RUNIT_ASSERT(memcmp(pTagBuf, tag, tag_len) == 0);

    RUNIT_API(mbedtls_gcm_free(pCtx));

    /***************************************************************
     *                     DECRYPTION                              *
     ***************************************************************/
    memset(pBuf, 0, sizeof(ct));
    memcpy(pTagBuf, tag, tag_len);

    RUNIT_API(mbedtls_gcm_init(pCtx));

    RUNIT_API(mbedtls_gcm_setkey(pCtx, MBEDTLS_CIPHER_ID_AES, key, key_len));

    RUNIT_ASSERT_W_PARAM("decrypt", mbedtls_gcm_crypt_and_tag(pCtx,
                                                              MBEDTLS_GCM_DECRYPT,
                                                              pt_len,
                                                              pIvBuf,
                                                              iv_len,
                                                              pAddBuf,
                                                              add_len,
                                                              pCipher,
                                                              pBuf,
                                                              tag_len,
                                                              pTagBuf) == 0);


    RUNIT_ASSERT(memcmp(pBuf, pt, pt_len) == 0);

    RUNIT_API(mbedtls_gcm_free(pCtx));

    /***************************************************************
     *            ALTERNATIVE DECRYPTION API                       *
     ***************************************************************/
    memset(pBuf, 0, sizeof(ct));
    memcpy(pTagBuf, tag, tag_len);

    RUNIT_API(mbedtls_gcm_init(pCtx));

    RUNIT_API(mbedtls_gcm_setkey(pCtx, MBEDTLS_CIPHER_ID_AES, key, key_len));

    RUNIT_ASSERT_API(mbedtls_gcm_auth_decrypt(pCtx,
                                              pt_len,
                                              pIvBuf,
                                              iv_len,
                                              pAddBuf,
                                              add_len,
                                              pTagBuf,
                                              tag_len,
                                              pCipher,
                                              pBuf) == 0);

    RUNIT_ASSERT(memcmp(pBuf, pt, pt_len) == 0);

bail:
    RUNIT_API(mbedtls_gcm_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(bufPtr);
    FREE_IF_NOT_NULL(cipherPtr);
    FREE_IF_NOT_NULL(plainBufPtr);
    FREE_IF_NOT_NULL(tagBufPtr);
    FREE_IF_NOT_NULL(ivBufPtr);
    FREE_IF_NOT_NULL(addBufPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%"PRIu32"b] PLAIN[%"PRIu32"B] AAD[%"PRIu32"B]",
                                   (uint32_t)key_len, (uint32_t)pt_len, (uint32_t)add_len);
#endif /* defined(MBEDTLS_GCM_C) */
    return rc;

}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_gcmTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    const char* TEST_NAME = "GCM";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_gcm() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_gcmProfiling(16) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_gcmProfiling(128) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_gcmProfiling(1024) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_gcmProfiling(8192) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_gcmProfiling(65535) == RUNIT_ERROR__OK);

bail:

    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;
}
