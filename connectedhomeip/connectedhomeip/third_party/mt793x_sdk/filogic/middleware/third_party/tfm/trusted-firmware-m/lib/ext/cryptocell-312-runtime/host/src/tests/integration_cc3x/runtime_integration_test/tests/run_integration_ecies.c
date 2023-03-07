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

/* cc lib */
#include "mbedtls_cc_ecies.h"

/* mbedtls lib */
#include "mbedtls/timing.h"
#include "mbedtls/bignum.h"
#include "mbedtls/ctr_drbg.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"

/************************************************************
 *
 * defines
 *
 ************************************************************/
#define RUNIT_PRINT_RANDOM_KEYS 0

#define DIGEST_SIZE         64
#define ECP_GROUP_ID        MBEDTLS_ECP_DP_SECP256R1
/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static int runIt_isEciesGrpSupported(mbedtls_ecp_group_id grpId);
static RunItError_t runIt_eciesRandom(void);
static RunItError_t runIt_eciesVector(void);

/************************************************************
 *
 * variables
 *
 ************************************************************/

/************************************************************
 *
 * static functions
 *
 ************************************************************/
static RunItError_t runIt_eciesRandom(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    mbedtls_ecp_group *pGrp = NULL;
    mbedtls_ecp_point *pQ = NULL;
    mbedtls_mpi d;

    const char * testName = NULL;
    uint8_t* pSenderSecret = NULL;
    uint8_t* pRecieverSecret = NULL;
    static const size_t SECRET_KEY_LEN_BYTES = 200;
    uint8_t* pCipher = NULL;
    size_t cipherDataSize = MBEDTLS_ECIES_MAX_CIPHER_LEN_BYTES;
    uint32_t *pBuf = NULL;

    RunItPtr grpPtr;
    RunItPtr senderSecretPtr;
    RunItPtr recieverSecretPtr;
    RunItPtr ciphertPtr;
    RunItPtr qPtr;
    RunItPtr bufPtr;

    const char* TEST_NAME = "ECIES Random";
    RUNIT_SUB_TEST_START(TEST_NAME);

    if (runIt_isEciesGrpSupported(ECP_GROUP_ID) == 0)
    {
        testName = "UNKNOWN";
        goto skip;
    }

    ALLOC_STRUCT(mbedtls_ecp_group, grpPtr, pGrp);
    ALLOC(senderSecretPtr, pSenderSecret, SECRET_KEY_LEN_BYTES);
    ALLOC(recieverSecretPtr, pRecieverSecret, SECRET_KEY_LEN_BYTES);
    ALLOC(ciphertPtr, pCipher, MBEDTLS_ECIES_MAX_CIPHER_LEN_BYTES);
    ALLOC_STRUCT(mbedtls_ecp_point, qPtr, pQ);
    ALLOC32(bufPtr, pBuf, MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES);

    mbedtls_ecp_group_init(pGrp);
    mbedtls_ecp_point_init(pQ);
    mbedtls_mpi_init(&d);
    memset(pBuf, 0, MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES);
    memset((uint8_t*)pSenderSecret, 0, SECRET_KEY_LEN_BYTES);
    memset((uint8_t*)pRecieverSecret, 0, SECRET_KEY_LEN_BYTES);

    RUNIT_ASSERT(mbedtls_ecp_group_load(pGrp, ECP_GROUP_ID) == 0);
    RUNIT_ASSERT(mbedtls_ecp_gen_keypair(pGrp, &d, pQ, gpRndContext->rndGenerateVectFunc, gpRndState) == 0);

    testName = mbedtls_ecp_curve_info_from_grp_id(ECP_GROUP_ID)->name;

#if RUNIT_PRINT_RANDOM_KEYS
    {
        size_t olen = 0;
        RUNIT_ASSERT_API(mbedtls_ecp_point_write_binary(pGrp, pQ, MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, pBuf, MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES) == 0);
        RUNIT_PRINT_BUF(pBuf, olen, "public binary");

        RUNIT_ASSERT_API(mbedtls_mpi_write_binary(&d, pBuf, mbedtls_mpi_size(&d)) == 0);
        RUNIT_PRINT_BUF(pBuf, mbedtls_mpi_size(&d), "private binary");
    }
#endif

    RUNIT_ASSERT_API(mbedtls_ecies_kem_encrypt(pGrp,
                                               pQ,
                                               CC_KDF_ISO18033_KDF1_DerivMode,
                                               CC_HKDF_HASH_SHA256_mode,
                                               1,
                                               pSenderSecret,
                                               SECRET_KEY_LEN_BYTES,
                                               pCipher,
                                               &cipherDataSize,
                                               pBuf,
                                               MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES,
                                               gpRndContext->rndGenerateVectFunc,
                                               gpRndContext->rndState) == 0);

    RUNIT_PRINT_BUF(pCipher, cipherDataSize, "pCipher");

    RUNIT_ASSERT_API(mbedtls_ecies_kem_decrypt(pGrp,
                                               &d,
                                               CC_KDF_ISO18033_KDF1_DerivMode,
                                               CC_HKDF_HASH_SHA256_mode,
                                               1,
                                               pCipher,
                                               cipherDataSize,
                                               pRecieverSecret,
                                               SECRET_KEY_LEN_BYTES,
                                               pBuf,
                                               MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES) == 0);

    RUNIT_PRINT_BUF(pSenderSecret, SECRET_KEY_LEN_BYTES, "pSenderSecret");
    RUNIT_PRINT_BUF(pRecieverSecret, SECRET_KEY_LEN_BYTES, "pRecieverSecret");

    RUNIT_ASSERT(memcmp(pSenderSecret, pRecieverSecret, SECRET_KEY_LEN_BYTES) == 0);

bail:
    mbedtls_ecp_group_free(pGrp);
    mbedtls_ecp_point_free(pQ);
    mbedtls_mpi_free(&d);

    FREE_IF_NOT_NULL(grpPtr);
    FREE_IF_NOT_NULL(senderSecretPtr);
    FREE_IF_NOT_NULL(recieverSecretPtr);
    FREE_IF_NOT_NULL(ciphertPtr);
    FREE_IF_NOT_NULL(qPtr);
    FREE_IF_NOT_NULL(bufPtr);

skip:
    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "ID[%s] DERIVE[ISO18033_KDF1] MD[SHA256]", testName);
    return rc;
}

static int runIt_isEciesGrpSupported(mbedtls_ecp_group_id grpId)
{
    const mbedtls_ecp_group_id * pGrpIdList = mbedtls_ecp_grp_id_list();

    while (*pGrpIdList != MBEDTLS_ECP_DP_NONE)
    {
        RUNIT_PRINT_DBG("test %"PRIu32" pGrpIdList %"PRIu32"\n", (uint32_t)grpId, (uint32_t)*pGrpIdList);
        if (grpId == *pGrpIdList)
            return 1;

        pGrpIdList++;
    }

    return 0;
}

static RunItError_t runIt_eciesVector(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    static const char* INPUT_D = "895a6ff53cd45011b125cd22c3f578bd77f916402f01d96488e12ddfa7fc61ec";
    static const char* INPUT_X = "efbbaee7568701f463187d7bf88d1dbf26dd597602f860512a0dea5842fd6cfa";
    static const char* INPUT_Y = "c1276720f57d2cc777b51dea53c086fe1cf315dc899c948f169f2206da4f6b3b";

    static const char* INPUT_EPH_D = "2b74dd5215d265d8b2df0488d2a01586830a832f731ce5e1a3db9aa8f99f8cd5";
    static const char* INPUT_EPH_X = "4b27901968c47b81f88116723ddd6a02be1d3bb6a820b69ee0198bc35606cabc";
    static const char* INPUT_EPH_Y = "99e388fc709819ac66c5aafab3d250e1fbb247594ddb4b5b33bce91b91fa14f9";

    mbedtls_ecp_group *pGrp = NULL;
    mbedtls_ecp_point *pQ = NULL;
    mbedtls_mpi d;
    mbedtls_ecp_point *pQEph = NULL;
    mbedtls_mpi dEph;
    uint32_t *pBuf = NULL;

    uint8_t* pSenderSecret = NULL;
    uint8_t* pRecieverSecret = NULL;
    static const size_t SECRET_KEY_LEN_BYTES = 128;
    uint8_t* pCipher = NULL;
    size_t cipherDataSize = MBEDTLS_ECIES_MAX_CIPHER_LEN_BYTES;
    size_t olen = 0;

    RunItPtr grpPtr;
    RunItPtr senderSecretPtr;
    RunItPtr recieverSecretPtr;
    RunItPtr ciphertPtr;
    RunItPtr qPtr;
    RunItPtr qEphPtr;
    RunItPtr bufPtr;

    const char* TEST_NAME = "ECIES Vector";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_ecp_group, grpPtr, pGrp);
    ALLOC(senderSecretPtr, pSenderSecret, SECRET_KEY_LEN_BYTES);
    ALLOC(recieverSecretPtr, pRecieverSecret, SECRET_KEY_LEN_BYTES);
    ALLOC(ciphertPtr, pCipher, MBEDTLS_ECIES_MAX_CIPHER_LEN_BYTES);
    ALLOC_STRUCT(mbedtls_ecp_point, qPtr, pQ);
    ALLOC_STRUCT(mbedtls_ecp_point, qEphPtr, pQEph);
    ALLOC32(bufPtr, pBuf, MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES);

    mbedtls_ecp_group_init(pGrp);
    mbedtls_ecp_point_init(pQ);
    mbedtls_mpi_init(&d);
    mbedtls_ecp_point_init(pQEph);
    mbedtls_mpi_init(&dEph);

    memset(pBuf, 0, MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES);
    memset((uint8_t*)pSenderSecret, 0, SECRET_KEY_LEN_BYTES);
    memset((uint8_t*)pRecieverSecret, 0, SECRET_KEY_LEN_BYTES);

    RUNIT_ASSERT(mbedtls_ecp_group_load(pGrp, ECP_GROUP_ID) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&d, 16, INPUT_D) == 0);
    RUNIT_ASSERT(mbedtls_ecp_point_read_string(pQ, 16, INPUT_X, INPUT_Y) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&dEph, 16, INPUT_EPH_D) == 0);
    RUNIT_ASSERT(mbedtls_ecp_point_read_string(pQEph, 16, INPUT_EPH_X, INPUT_EPH_Y) == 0);

    RUNIT_ASSERT(mbedtls_ecp_point_write_binary(pGrp, pQ, MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, (uint8_t*)pBuf, MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES) == 0);
    RUNIT_PRINT_BUF(pBuf, olen, "public binary");

    RUNIT_ASSERT(mbedtls_mpi_write_binary(&dEph, (uint8_t*)pBuf, mbedtls_mpi_size(&dEph)) == 0);
    RUNIT_PRINT_BUF(pBuf, mbedtls_mpi_size(&dEph), "private binary");

    RUNIT_ASSERT_API(mbedtls_ecies_kem_encrypt_full(pGrp,
                                                    pQ,
                                                    CC_KDF_ISO18033_KDF1_DerivMode,
                                                    CC_HKDF_HASH_SHA256_mode,
                                                    1,
                                                    pQEph,
                                                    &dEph,
                                                    pSenderSecret,
                                                    SECRET_KEY_LEN_BYTES,
                                                    pCipher,
                                                    &cipherDataSize,
                                                    pBuf,
                                                    MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES,
                                                    gpRndContext->rndGenerateVectFunc,
                                                    gpRndContext->rndState) == 0);

    RUNIT_PRINT_BUF(pCipher, cipherDataSize, "pCipher");

    RUNIT_ASSERT_WITH_RESULT(mbedtls_ecies_kem_decrypt(pGrp,
                                                       &d,
                                                       CC_KDF_ISO18033_KDF1_DerivMode,
                                                       CC_HKDF_HASH_SHA256_mode,
                                                       1,
                                                       pCipher,
                                                       cipherDataSize,
                                                       pRecieverSecret,
                                                       SECRET_KEY_LEN_BYTES,
                                                       pBuf,
                                                       MBEDTLS_ECIES_MIN_BUFF_LEN_BYTES), 0);

    RUNIT_PRINT_BUF(pSenderSecret, SECRET_KEY_LEN_BYTES, "pSenderSecret");
    RUNIT_PRINT_BUF(pRecieverSecret, SECRET_KEY_LEN_BYTES, "pRecieverSecret");

    RUNIT_ASSERT(memcmp(pRecieverSecret, pSenderSecret, SECRET_KEY_LEN_BYTES) == 0);

bail:
    mbedtls_ecp_group_free(pGrp);
    mbedtls_ecp_point_free(pQ);
    mbedtls_ecp_point_free(pQEph);
    mbedtls_mpi_free(&d);
    mbedtls_mpi_free(&dEph);

    FREE_IF_NOT_NULL(grpPtr);
    FREE_IF_NOT_NULL(senderSecretPtr);
    FREE_IF_NOT_NULL(recieverSecretPtr);
    FREE_IF_NOT_NULL(ciphertPtr);
    FREE_IF_NOT_NULL(qPtr);
    FREE_IF_NOT_NULL(qEphPtr);
    FREE_IF_NOT_NULL(bufPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "ID[%s] DERIVE[ISO18033_KDF1] MD[SHA256]", mbedtls_ecp_curve_info_from_grp_id(ECP_GROUP_ID)->name);
    return rc;
}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_eciesTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    const char* TEST_NAME = "ECIES";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_eciesRandom() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_eciesVector() == RUNIT_ERROR__OK);

bail:
    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;
}
