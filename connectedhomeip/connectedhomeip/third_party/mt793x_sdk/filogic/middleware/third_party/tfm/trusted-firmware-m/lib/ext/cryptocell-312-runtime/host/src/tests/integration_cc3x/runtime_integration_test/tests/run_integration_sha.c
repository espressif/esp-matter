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
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/timing.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"

/************************************************************
 *
 * static functions prototypes
 *
 ************************************************************/
static RunItError_t runIt_sha1Test(void);
static RunItError_t runIt_sha224Test(void);
static RunItError_t runIt_sha256Test(void);
static RunItError_t runIt_sha384Test(void);
static RunItError_t runIt_sha512Test(void);

/************************************************************
 *
 * static functions
 *
 ************************************************************/
static RunItError_t runIt_sha1Test(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

#if defined(MBEDTLS_SHA1_C)
    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA1.pdf */
    static const char* INPUT_MESSAGE = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    static const uint8_t DIGEST[] = { 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE, 0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1 };

    unsigned char sha1sum[20];
    RunItPtr ctxPtr;
    RunItPtr ctxClonedPtr;

    mbedtls_sha1_context *pCtx = NULL;
    mbedtls_sha1_context *pCtxCloned = NULL;

    const char* TEST_NAME = "SHA1";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_sha1_context, ctxPtr, pCtx);
    ALLOC_STRUCT(mbedtls_sha1_context, ctxClonedPtr, pCtxCloned);

    /* Initialize sha engine */
    RUNIT_API(mbedtls_sha1_init(pCtx));
    RUNIT_ASSERT(mbedtls_sha1_starts_ret(pCtx) == 0);
    RUNIT_ASSERT(mbedtls_sha1_update_ret(pCtx, (const unsigned char*)INPUT_MESSAGE, strlen(INPUT_MESSAGE)) == 0);
    RUNIT_API(mbedtls_sha1_clone(pCtxCloned, pCtx));
    RUNIT_ASSERT(mbedtls_sha1_finish_ret(pCtxCloned, sha1sum) == 0);

    RUNIT_PRINT_BUF(sha1sum, 20, "result");

    /* compare result */
    RUNIT_ASSERT(memcmp(sha1sum, DIGEST, 20) == 0);

    /* test the wrapper function too */
    memset(sha1sum, 0, sizeof(sha1sum));

    RUNIT_API(mbedtls_sha1((const unsigned char*)INPUT_MESSAGE, strlen(INPUT_MESSAGE), sha1sum));

    /* compare result */
    RUNIT_ASSERT(memcmp(sha1sum, DIGEST, 20) == 0);

bail:
    RUNIT_API(mbedtls_sha1_free(pCtx));
    RUNIT_API(mbedtls_sha1_free(pCtxCloned));

    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(ctxClonedPtr);

    RUNIT_SUB_TEST_RESULT(TEST_NAME);
#endif /* MBEDTLS_SHA1_C */
    return rc;
}

static RunItError_t runIt_sha224Test(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

#if defined(MBEDTLS_SHA256_C)
    const int IS_SHA_224 = 1;

    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA224.pdf */
    static const char* INPUT_MESSAGE = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    static const uint8_t DIGEST[] = { 0x75, 0x38, 0x8B, 0x16, 0x51, 0x27, 0x76, 0xCC, 0x5D, 0xBA, 0x5D, 0xA1, 0xFD, 0x89, 0x01, 0x50, 0xB0, 0xC6, 0x45, 0x5C, 0xB4, 0xF5, 0x8B, 0x19, 0x52, 0x52, 0x25, 0x25 };

    unsigned char sha224sum[28];
    RunItPtr ctxPtr;
    RunItPtr ctxClonedPtr;

    mbedtls_sha256_context *pCtx = NULL;
    mbedtls_sha256_context *pCtxCloned = NULL;

    const char* TEST_NAME = "SHA224";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_sha256_context, ctxPtr, pCtx);
    ALLOC_STRUCT(mbedtls_sha256_context, ctxClonedPtr, pCtxCloned);

    /* Initialize sha engine */
    RUNIT_API(mbedtls_sha256_init(pCtx));
    RUNIT_ASSERT(mbedtls_sha256_starts_ret(pCtx, IS_SHA_224) == 0);
    RUNIT_ASSERT(mbedtls_sha256_update_ret(pCtx, (const unsigned char*)INPUT_MESSAGE, strlen(INPUT_MESSAGE)) == 0);
    RUNIT_API(mbedtls_sha256_clone(pCtxCloned, pCtx));
    RUNIT_ASSERT(mbedtls_sha256_finish_ret(pCtxCloned, sha224sum) == 0);

    RUNIT_PRINT_BUF(sha224sum, 28, "result");

    /* compare result */
    RUNIT_ASSERT(memcmp(sha224sum, DIGEST, 28) == 0);

    /* test the wrapper function too */
    memset(sha224sum, 0, sizeof(sha224sum));

    RUNIT_API(mbedtls_sha256((const unsigned char*)INPUT_MESSAGE, strlen(INPUT_MESSAGE), sha224sum, IS_SHA_224));

    /* compare result */
    RUNIT_ASSERT(memcmp(sha224sum, DIGEST, 28) == 0);

bail:
    RUNIT_API(mbedtls_sha256_free(pCtx));
    RUNIT_API(mbedtls_sha256_free(pCtxCloned));

    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(ctxClonedPtr);

    RUNIT_SUB_TEST_RESULT(TEST_NAME);
#endif /* MBEDTLS_SHA256_C */
    return rc;
}

static RunItError_t runIt_sha256Test(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if defined(MBEDTLS_SHA256_C)
    const int IS_SHA_224 = 0;

    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA256.pdf */
    static const char* INPUT_MESSAGE = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    static const uint8_t DIGEST[] = { 0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8, 0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39, 0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67, 0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1 };

    unsigned char sha256sum[32];
    RunItPtr ctxPtr;
    RunItPtr ctxClonedPtr;

    mbedtls_sha256_context *pCtx = NULL;
    mbedtls_sha256_context *pCtxCloned = NULL;

    const char* TEST_NAME = "SHA256";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_sha256_context, ctxPtr, pCtx);
    ALLOC_STRUCT(mbedtls_sha256_context, ctxClonedPtr, pCtxCloned);

    /* Initialize sha engine */
    RUNIT_API(mbedtls_sha256_init(pCtx));
    RUNIT_ASSERT(mbedtls_sha256_starts_ret(pCtx, IS_SHA_224) == 0);
    RUNIT_ASSERT(mbedtls_sha256_update_ret(pCtx, (const unsigned char*)INPUT_MESSAGE, strlen(INPUT_MESSAGE)) == 0);
    RUNIT_API(mbedtls_sha256_clone(pCtxCloned, pCtx));
    RUNIT_ASSERT(mbedtls_sha256_finish_ret(pCtxCloned, sha256sum) == 0);

    RUNIT_PRINT_BUF(sha256sum, 32, "result");

    /* compare result */
    RUNIT_ASSERT(memcmp(sha256sum, DIGEST, 32) == 0);

    /* test the wrapper function too */
    memset(sha256sum, 0, sizeof(sha256sum));
    RUNIT_API(mbedtls_sha256((const unsigned char*)INPUT_MESSAGE, strlen(INPUT_MESSAGE), sha256sum, IS_SHA_224));

    /* compare result */
    RUNIT_ASSERT(memcmp(sha256sum, DIGEST, 32) == 0);

bail:
    RUNIT_API(mbedtls_sha256_free(pCtx));
    RUNIT_API(mbedtls_sha256_free(pCtxCloned));

    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(ctxClonedPtr);

    RUNIT_SUB_TEST_RESULT(TEST_NAME);
#endif /* MBEDTLS_SHA256_C */
    return rc;
}

static RunItError_t runIt_sha384Test(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if defined(MBEDTLS_SHA512_C)
    const int IS_SHA_384 = 1;

    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA384.pdf */
    static const char* INPUT_MESSAGE = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
    static const uint8_t DIGEST[] = { 0x09,0x33,0x0C,0x33,0xF7,0x11,0x47,0xE8,0x3D,0x19,0x2F,0xC7,0x82,0xCD,0x1B,0x47,0x53,0x11,0x1B,0x17,0x3B,0x3B,0x05,0xD2,0x2F,0xA0,0x80,0x86,0xE3,0xB0,0xF7,0x12,0xFC,0xC7,0xC7,0x1A,0x55,0x7E,0x2D,0xB9,0x66,0xC3,0xE9,0xFA,0x91,0x74,0x60,0x39};

    unsigned char sha512sum[48];
    RunItPtr ctxPtr;
    RunItPtr ctxClonedPtr;

    mbedtls_sha512_context *pCtx = NULL;
    mbedtls_sha512_context *pCtxCloned = NULL;

    const char* TEST_NAME = "SHA384";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_sha512_context, ctxPtr, pCtx);
    ALLOC_STRUCT(mbedtls_sha512_context, ctxClonedPtr, pCtxCloned);

    /* Initialize sha engine */
    RUNIT_API(mbedtls_sha512_init(pCtx));
    RUNIT_API(mbedtls_sha512_starts(pCtx, IS_SHA_384));
    RUNIT_API(mbedtls_sha512_update(pCtx, (const unsigned char*)INPUT_MESSAGE, strlen(INPUT_MESSAGE)));
    RUNIT_API(mbedtls_sha512_clone(pCtxCloned, pCtx));
    RUNIT_API(mbedtls_sha512_finish(pCtxCloned, sha512sum));

    RUNIT_PRINT_BUF(sha512sum, 48, "result");

    /* compare result */
    RUNIT_ASSERT(memcmp(sha512sum, DIGEST, 48) == 0);

    /* test the wrapper function too */
    memset(sha512sum, 0, sizeof(sha512sum));
    RUNIT_API(mbedtls_sha512((const unsigned char*)INPUT_MESSAGE, strlen(INPUT_MESSAGE), sha512sum, IS_SHA_384));

    /* compare result */
    RUNIT_ASSERT(memcmp(sha512sum, DIGEST, 48) == 0);

bail:
    RUNIT_API(mbedtls_sha512_free(pCtx));
    RUNIT_API(mbedtls_sha512_free(pCtxCloned));

    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(ctxClonedPtr);

    RUNIT_SUB_TEST_RESULT(TEST_NAME);
#endif /* MBEDTLS_SHA512_C */
    return rc;
}

static RunItError_t runIt_sha512Test(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if defined(MBEDTLS_SHA512_C)
    const int IS_SHA_384 = 0;

    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA256.pdf */
    static const char* INPUT_MESSAGE = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
    static const uint8_t DIGEST[] = { 0x8E,0x95,0x9B,0x75,0xDA,0xE3,0x13,0xDA,0x8C,0xF4,0xF7,0x28,0x14,0xFC,0x14,0x3F,0x8F,0x77,0x79,0xC6,0xEB,0x9F,0x7F,0xA1,0x72,0x99,0xAE,0xAD,0xB6,0x88,0x90,0x18,0x50,0x1D,0x28,0x9E,0x49,0x00,0xF7,0xE4,0x33,0x1B,0x99,0xDE,0xC4,0xB5,0x43,0x3A,0xC7,0xD3,0x29,0xEE,0xB6,0xDD,0x26,0x54,0x5E,0x96,0xE5,0x5B,0x87,0x4B,0xE9,0x09 };

    unsigned char sha512sum[64];
    RunItPtr ctxPtr;
    RunItPtr ctxClonedPtr;

    mbedtls_sha512_context *pCtx = NULL;
    mbedtls_sha512_context *pCtxCloned = NULL;

    const char* TEST_NAME = "SHA512";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_sha512_context, ctxPtr, pCtx);
    ALLOC_STRUCT(mbedtls_sha512_context, ctxClonedPtr, pCtxCloned);

    /* Initialize sha engine */
    RUNIT_API(mbedtls_sha512_init(pCtx));
    RUNIT_API(mbedtls_sha512_starts(pCtx, IS_SHA_384));
    RUNIT_API(mbedtls_sha512_update(pCtx, (const unsigned char*)INPUT_MESSAGE, strlen(INPUT_MESSAGE)));
    RUNIT_API(mbedtls_sha512_clone(pCtxCloned, pCtx));
    RUNIT_API(mbedtls_sha512_finish(pCtxCloned, sha512sum));

    RUNIT_PRINT_BUF(sha512sum, 64, "result");

    /* compare result */
    RUNIT_ASSERT(memcmp(sha512sum, DIGEST, 64) == 0);

    /* test the wrapper function too */
    memset(sha512sum, 0, sizeof(sha512sum));
    RUNIT_API(mbedtls_sha512((const unsigned char*)INPUT_MESSAGE, strlen(INPUT_MESSAGE), sha512sum, IS_SHA_384));

    /* compare result */
    RUNIT_ASSERT(memcmp(sha512sum, DIGEST, 64) == 0);

bail:
    RUNIT_API(mbedtls_sha512_free(pCtx));
    RUNIT_API(mbedtls_sha512_free(pCtxCloned));

    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(ctxClonedPtr);

    RUNIT_SUB_TEST_RESULT(TEST_NAME);
#endif /* MBEDTLS_SHA512_C */
    return rc;
}
/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_shaTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    const char* TEST_NAME = "HASH";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_sha1Test() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_sha224Test() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_sha256Test() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_sha384Test() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_sha512Test() == RUNIT_ERROR__OK);

bail:
    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;
}
