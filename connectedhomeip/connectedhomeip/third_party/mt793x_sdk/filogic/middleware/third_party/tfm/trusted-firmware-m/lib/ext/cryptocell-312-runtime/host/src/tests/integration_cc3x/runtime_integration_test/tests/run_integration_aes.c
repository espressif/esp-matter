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
#include "mbedtls/aes.h"
#include "mbedtls/timing.h"
#include "mbedtls/gcm.h"

/* CC */
#include "mbedtls_cc_aes_key_wrap.h"


/* pal */
#include "test_pal_mem.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"

#if defined(MBEDTLS_AES_C)

#define AES_IV_SIZE     16
#define AES_BLOCK_SIZE  16

/************************************************************
 *
 * static functions prototypes
 *
 ************************************************************/
static RunItError_t runIt_aesOfbTest(void);
static RunItError_t runIt_aesCtrTestProfiling(size_t data_len);
static RunItError_t runIt_aesCtrTest(void);
static RunItError_t runIt_aesCbcTest(void);
static RunItError_t runIt_aesEcbTest(void);

/************************************************************
 *
 * static functions
 *
 ************************************************************/

static RunItError_t runIt_aesOfbTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if defined(MBEDTLS_CIPHER_MODE_OFB) && defined(MBEDTLS_AES_ALT)
    const uint32_t KEY_SIZE = 256;

    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_OFB.pdf */
    static const uint8_t IV[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    static const uint8_t PLAIN[] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };
    static const uint8_t KEY[] = { 0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE, 0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81, 0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7, 0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4 };
    static const uint8_t CYPHER[] = { 0xDC, 0x7E, 0x84, 0xBF, 0xDA, 0x79, 0x16, 0x4B, 0x7E, 0xCD, 0x84, 0x86, 0x98, 0x5D, 0x38, 0x60 };

    uint8_t key[KEY_SIZE / 8];
    uint8_t iv[AES_IV_SIZE];
    uint8_t buf[AES_BLOCK_SIZE];
    mbedtls_aes_context ctx;

    const char* TEST_NAME = "AES-OFB-256";
    RUNIT_SUB_TEST_START(TEST_NAME);

    /* Initialize aes engine */
    RUNIT_API(mbedtls_aes_init(&ctx));

    memcpy(key, KEY, sizeof(key));

    /*
     * encrypt
     */
    memcpy(buf, PLAIN, sizeof(buf));
    memcpy(iv, IV, sizeof(iv));

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "init_enc");

    /* set key into context */
    RUNIT_ASSERT_API(mbedtls_aes_setkey_enc(&ctx, key, KEY_SIZE) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_aes_crypt_ofb(&ctx, sizeof(buf), 0, iv, buf, buf) == 0);

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "encrypted");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, CYPHER, AES_BLOCK_SIZE) == 0);

    /*
     * decrypt
     */
    memcpy(buf, CYPHER, sizeof(buf));
    memcpy(iv, IV, sizeof(iv));

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "init_dec");

    /* set key into context */
    RUNIT_ASSERT_API(mbedtls_aes_setkey_dec(&ctx, key, KEY_SIZE) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_aes_crypt_ofb(&ctx, sizeof(buf), 0, iv, buf, buf) == 0);

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "decrypted");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, PLAIN, AES_BLOCK_SIZE) == 0);

bail:
    RUNIT_API(mbedtls_aes_free(&ctx));

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%ub] IV[%uB] PLAIN[%uB]", sizeof(key) * 8, sizeof(iv), sizeof(buf));
#endif /* MBEDTLS_CIPHER_MODE_OFB && MBEDTLS_AES_ALT */
    return rc;

}

static RunItError_t runIt_aesCtrTestProfiling(size_t data_len)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    const uint32_t KEY_SIZE = 128;

    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CTR.pdf */
    static const uint8_t KEY[] = { 0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE, 0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81 };
    static const uint8_t PLAIN[] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };
    static const uint8_t NONCE[] = { 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF };

    size_t offset;
    uint8_t key[KEY_SIZE / 8];
    uint8_t *buf = NULL;
    unsigned char nonce_counter[16];
    unsigned char stream_block[16];
    mbedtls_aes_context ctx;
    char testParam[PARAM_LEN] = { 0 };

    RunItPtr bufPtr;

    const char* TEST_NAME = "AES-CTR-128";
    RUNIT_SUB_TEST_START(TEST_NAME);

    snprintf(testParam, PARAM_LEN - 1, "%uB", data_len);

    ALLOC32(bufPtr, buf, data_len);

    /* Initialize aes engine */
    RUNIT_API(mbedtls_aes_init(&ctx));

    memcpy(key, KEY, sizeof(key));

    /*
     * encrypt
     */
    memcpy(buf, PLAIN, sizeof(PLAIN));
    memcpy(nonce_counter, NONCE, sizeof(nonce_counter));
    offset = 0;

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "init_enc");

    /* set key into context */
    RUNIT_ASSERT_API(mbedtls_aes_setkey_enc(&ctx, key, KEY_SIZE) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_W_PARAM(testParam, mbedtls_aes_crypt_ctr(&ctx, data_len, &offset, nonce_counter, stream_block, buf, buf) == 0);

bail:
    RUNIT_API(mbedtls_aes_free(&ctx));

    FREE_IF_NOT_NULL(bufPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%ub] PLAIN[%uB]", (unsigned int)KEY_SIZE, (unsigned int)data_len);
#endif /* MBEDTLS_CIPHER_MODE_CTR */
    return rc;
}

static RunItError_t runIt_aesCtrTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    const uint32_t KEY_SIZE = 256;

    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CTR.pdf */
    static const uint8_t KEY[] = { 0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE, 0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81, 0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7, 0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4 };
    static const uint8_t PLAIN[] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };
    static const uint8_t CYPHER[] = { 0x60, 0x1E, 0xC3, 0x13, 0x77, 0x57, 0x89, 0xA5, 0xB7, 0xA7, 0xF5, 0x04, 0xBB, 0xF3, 0xD2, 0x28 };
    static const uint8_t NONCE[] = { 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF };

    size_t offset;
    uint8_t key[KEY_SIZE / 8];
    uint8_t buf[AES_BLOCK_SIZE];
    unsigned char nonce_counter[16];
    unsigned char stream_block[16];
    mbedtls_aes_context ctx;

    const char* TEST_NAME = "AES-CTR-256";
    RUNIT_SUB_TEST_START(TEST_NAME);

    /* Initialize aes engine */
    RUNIT_API(mbedtls_aes_init(&ctx));

    memcpy(key, KEY, sizeof(key));

    /*
     * encrypt
     */
    memcpy(buf, PLAIN, sizeof(buf));
    memcpy(nonce_counter, NONCE, sizeof(nonce_counter));
    offset = 0;

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "init_enc");

    /* set key into context */
    RUNIT_ASSERT_API(mbedtls_aes_setkey_enc(&ctx, key, KEY_SIZE) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_aes_crypt_ctr(&ctx, sizeof(buf), &offset, nonce_counter, stream_block, buf, buf) == 0);

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "encrypted");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, CYPHER, AES_BLOCK_SIZE) == 0);

    /*
     * decrypt
     */
    memcpy(buf, CYPHER, sizeof(buf));
    memcpy(nonce_counter, NONCE, sizeof(nonce_counter));
    offset = 0;

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "init_dec");

    /* set key into context, also in aes ctr decrypt, nonce should be encrypted */
    /* aes ctr is fully symmetric so we use the encrypt operation to decrypt */
    /* by using mbedtls_aes_setkey_enc we select an encryption operation */

    RUNIT_ASSERT_API(mbedtls_aes_setkey_enc(&ctx, key, KEY_SIZE) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_aes_crypt_ctr(&ctx, sizeof(buf), &offset, nonce_counter, stream_block, buf, buf) == 0);

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "decrypted");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, PLAIN, AES_BLOCK_SIZE) == 0);

bail:
    RUNIT_API(mbedtls_aes_free(&ctx));

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%ub] PLAIN[%uB]", (unsigned int)KEY_SIZE, sizeof(buf));
#endif /* MBEDTLS_CIPHER_MODE_CTR */
    return rc;
}

static RunItError_t runIt_aesCbcTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if defined(MBEDTLS_CIPHER_MODE_CBC)

    const uint32_t KEY_SIZE = 256;

    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CBC.pdf */
    static const uint8_t IV[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    static const uint8_t KEY[] = { 0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE, 0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81, 0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7, 0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4 };
    static const uint8_t PLAIN[] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };
    static const uint8_t CYPHER[] = { 0xF5, 0x8C, 0x4C, 0x04, 0xD6, 0xE5, 0xF1, 0xBA, 0x77, 0x9E, 0xAB, 0xFB, 0x5F, 0x7B, 0xFB, 0xD6 };

    uint8_t key[KEY_SIZE / 8];
    uint8_t buf[AES_BLOCK_SIZE];
    uint8_t iv[AES_IV_SIZE];
    mbedtls_aes_context ctx;

    const char* TEST_NAME = "AES-CBC-256";
    RUNIT_SUB_TEST_START(TEST_NAME);

    /* Initialize aes engine */
    RUNIT_API(mbedtls_aes_init(&ctx));

    memcpy(key, KEY, sizeof(key));

    /*
     * encrypt
     */
    memcpy(iv, IV, sizeof(iv));
    memcpy(buf, PLAIN, sizeof(buf));

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "init_enc");
    RUNIT_PRINT_BUF(iv, AES_IV_SIZE, "iv_enc");

    /* set key into context */
    RUNIT_ASSERT_API(mbedtls_aes_setkey_enc(&ctx, key, KEY_SIZE) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, sizeof(buf), iv, buf, buf) == 0);

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "encrypted");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, CYPHER, AES_BLOCK_SIZE) == 0);

    /*
     * decrypt
     */
    memcpy(iv, IV, sizeof(iv));
    memcpy(buf, CYPHER, sizeof(buf));

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "init_dec");
    RUNIT_PRINT_BUF(iv, AES_IV_SIZE, "iv_dec");

    /* set key into context */
    RUNIT_ASSERT_API(mbedtls_aes_setkey_dec(&ctx, key, KEY_SIZE) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, sizeof(buf), iv, buf, buf) == 0);

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "decrypted");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, PLAIN, AES_BLOCK_SIZE) == 0);

bail:
    RUNIT_API(mbedtls_aes_free(&ctx));

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%ub] PLAIN[%uB]", sizeof(key) * 8, sizeof(buf));
#endif /* MBEDTLS_CIPHER_MODE_CBC */
    return rc;
}

static RunItError_t runIt_aesEcbTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    const uint32_t KEY_SIZE = 256;

    /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CBC.pdf */
    static const uint8_t KEY[] = { 0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE, 0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81, 0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7, 0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4 };
    static const uint8_t PLAIN[] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };
    static const uint8_t CYPHER[] = { 0xF3, 0xEE, 0xD1, 0xBD, 0xB5, 0xD2, 0xA0, 0x3C, 0x06, 0x4B, 0x5A, 0x7E, 0x3D, 0xB1, 0x81, 0xF8 };

    uint8_t key[KEY_SIZE / 8];
    uint8_t buf[AES_BLOCK_SIZE];
    mbedtls_aes_context ctx;

    const char* TEST_NAME = "AES-ECB-256";
    RUNIT_SUB_TEST_START(TEST_NAME);

    /* Initialize aes engine */
    RUNIT_API(mbedtls_aes_init(&ctx));

    memcpy(key, KEY, sizeof(key));

    /*
     * encrypt
     */
    memcpy(buf, PLAIN, sizeof(buf));

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "init");

    /* set key into context */
    RUNIT_ASSERT_API(mbedtls_aes_setkey_enc(&ctx, key, KEY_SIZE) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, buf, buf) == 0);

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, CYPHER, AES_BLOCK_SIZE) == 0);

    /*
     * decrypt
     */
    memcpy(buf, CYPHER, sizeof(buf));

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "encrypted");

    /* set key into context */
    RUNIT_ASSERT_API(mbedtls_aes_setkey_dec(&ctx, key, KEY_SIZE) == 0);

    /* perform cryptographic operation */
    RUNIT_ASSERT_API(mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_DECRYPT, buf, buf) == 0);

    RUNIT_PRINT_BUF(buf, AES_BLOCK_SIZE, "decrypted");

    /* compare result */
    RUNIT_ASSERT(memcmp(buf, PLAIN, AES_BLOCK_SIZE) == 0);

bail:
    RUNIT_API(mbedtls_aes_free(&ctx));

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "KEY[%ub] PLAIN[%uB]", sizeof(key) * 8, sizeof(buf));
    return rc;

}




/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_aesTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    const char* TEST_NAME = "AES";
    RUNIT_TEST_START(TEST_NAME);


    RUNIT_ASSERT(runIt_aesEcbTest() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_aesCbcTest() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_aesCtrTest() == RUNIT_ERROR__OK);

    RUNIT_ASSERT(runIt_aesOfbTest() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_aesCtrTestProfiling(16) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_aesCtrTestProfiling(128) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_aesCtrTestProfiling(1024) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_aesCtrTestProfiling(8192) == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_aesCtrTestProfiling(65535) == RUNIT_ERROR__OK);

bail:
    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;
}

#endif /* MBEDTLS_AES_C */
