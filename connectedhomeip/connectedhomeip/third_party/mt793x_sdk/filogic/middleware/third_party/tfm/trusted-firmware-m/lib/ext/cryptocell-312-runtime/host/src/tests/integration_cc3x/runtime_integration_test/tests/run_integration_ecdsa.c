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
#include "mbedtls/ecdsa.h"
#include "mbedtls/ecp.h"
#include "mbedtls/timing.h"
#include "mbedtls/bignum.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls_cc_ecdsa_edwards.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"

#if defined MBEDTLS_ECDSA_C
/************************************************************
 *
 * defines
 *
 ************************************************************/
#define DIGEST_SIZE         64
#define ECP_GROUP_ID        MBEDTLS_ECP_DP_SECP256R1
#define ECP_GROUP_ID_STR    "DP_SECP256R1"
#define MD_TYPE_ID          MBEDTLS_MD_SHA256
#define MD_TYPE_ID_STR      "SHA256"

#define RUNIT_ECDSA_TEST_R521 0
/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static RunItError_t runIt_ecdsaPrimRandomTest(void);
static RunItError_t runIt_ecdsaPrimVectorsTest(void);
static RunItError_t runIt_ecdsaDetVectorsTest(void);
static RunItError_t runIt_ecdsaWriteReadRandomTest(void);
static RunItError_t runIt_ecdsaPrimRandomEdwTest(void);

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
static RunItError_t runIt_ecdsaPrimRandomTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
    mbedtls_ecp_group_id id = RUNIT_ECDSA_TEST_R521 ? MBEDTLS_ECP_DP_SECP521R1 : MBEDTLS_ECP_DP_SECP256R1;
    mbedtls_ctr_drbg_context *pCtrDrbg = (mbedtls_ctr_drbg_context *)gpRndState;
    mbedtls_ecp_group *pGrp = NULL;
    mbedtls_ecp_point *pQ = NULL;
    mbedtls_mpi d, r, s;
    unsigned char *pBuf = NULL;

    RunItPtr grpPtr;
    RunItPtr qPtr;
    RunItPtr bufPtr;

    const char* TEST_NAME = "ECDSA Primary Random";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC(bufPtr, pBuf, DIGEST_SIZE);
    ALLOC_STRUCT(mbedtls_ecp_group, grpPtr, pGrp);
    ALLOC_STRUCT(mbedtls_ecp_point, qPtr, pQ);

    mbedtls_ecp_group_init(pGrp);
    mbedtls_ecp_point_init(pQ);
    mbedtls_mpi_init(&d);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    memset(pBuf, 0, DIGEST_SIZE);

    RUNIT_ASSERT(mbedtls_ctr_drbg_random(pCtrDrbg, pBuf, DIGEST_SIZE) == 0);
    RUNIT_ASSERT(mbedtls_ecp_group_load(pGrp, id) == 0);
    RUNIT_ASSERT(mbedtls_ecp_gen_keypair(pGrp, &d, pQ, mbedtls_ctr_drbg_random, pCtrDrbg) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdsa_sign(pGrp, &r, &s, &d, pBuf, DIGEST_SIZE, mbedtls_ctr_drbg_random, pCtrDrbg) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdsa_verify(pGrp, pBuf, DIGEST_SIZE, pQ, &r, &s) == 0);

bail:
    mbedtls_ecp_group_free(pGrp);
    mbedtls_ecp_point_free(pQ);

    mbedtls_mpi_free(&d);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);

    FREE_IF_NOT_NULL(grpPtr);
    FREE_IF_NOT_NULL(qPtr);
    FREE_IF_NOT_NULL(bufPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "ID[%s]", mbedtls_ecp_curve_info_from_grp_id(id)->name);
#endif /* defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) */
    return rc;
}

static RunItError_t runIt_ecdsaPrimRandomEdwTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    size_t data_len = 200;

    mbedtls_ctr_drbg_context *pCtrDrbg = (mbedtls_ctr_drbg_context *)gpRndState;
    mbedtls_ecdsa_context *pCtx = NULL;

    unsigned char *pDataIn = NULL;

    RunItPtr ctxPtr;
    RunItPtr dataPtr;

    mbedtls_mpi r, s;
    const char* TEST_NAME = "ECDSA Random Edwards";
    RUNIT_SUB_TEST_START(TEST_NAME);

    mbedtls_mpi_init( &r );
    mbedtls_mpi_init( &s );


    ALLOC_STRUCT(mbedtls_ecdsa_context, ctxPtr, pCtx);
    ALLOC(dataPtr, pDataIn, data_len);

    RUNIT_API(mbedtls_ecdsa_init(pCtx));

    /* prepare material for signature */
    RUNIT_ASSERT_WITH_RESULT(mbedtls_ctr_drbg_random(pCtrDrbg, pDataIn, data_len), 0);

    /* generate signing key */
    RUNIT_ASSERT_WITH_RESULT( mbedtls_ecdsa_genkey_edwards(pCtx, MBEDTLS_ECP_DP_CURVE25519, mbedtls_ctr_drbg_random, pCtrDrbg), 0);
    RUNIT_ASSERT_WITH_RESULT( mbedtls_ecdsa_sign_edwards( &pCtx->grp, &r, &s, &pCtx->d, pDataIn, data_len), 0 );
    RUNIT_ASSERT_WITH_RESULT( mbedtls_ecdsa_verify_edwards(&pCtx->grp, pDataIn, data_len, &pCtx->Q, &r, &s), 0 );

bail:
    RUNIT_API(mbedtls_ecdsa_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(dataPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "ID[DP_CURVE25519] MD[%s]", MD_TYPE_ID_STR);
    return rc;
}

static RunItError_t runIt_ecdsaPrimVectorsTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
    static const char *d_str = "DC51D3866A15BACDE33D96F992FCA99DA7E6EF0934E7097559C27F1614C88A7F";
    static const char *xQ_str = "2442A5CC0ECD015FA3CA31DC8E2BBC70BF42D60CBCA20085E0822CB04235E970";
    static const char *yQ_str = "6FC98BD7E50211A4A27102FA3549DF79EBCB4BF246B80945CDDFE7D509BBFD7D";
    static const char *k_str = "9E56F509196784D963D1C0A401510EE7ADA3DCC5DEE04B154BF61AF1D5A6DECE";
    static const char *hash_str = "BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD";
    static const char *r_str = "CB28E0999B9C7715FD0A80D8E47A77079716CBBF917DD72E97566EA1C066957C";
    static const char *s_str = "86FA3BB4E26CAD5BF90B7F81899256CE7594BB1EA0C89212748BFF3B3D5B0315";

    mbedtls_ecp_group *pGrp = NULL;
    mbedtls_ecp_point *pQ = NULL;
    mbedtls_mpi d;
    mbedtls_mpi r;
    mbedtls_mpi s;
    mbedtls_mpi rCheck;
    mbedtls_mpi sCheck;
    unsigned char *pHash = NULL;
    unsigned char *pRndBuf = NULL;

    RunItPtr grpPtr;
    RunItPtr qPtr;
    RunItPtr hashPtr;
    RunItPtr rndBufPtr;

    size_t hlen;
    rnd_buf_info rnd_info;

    const char* TEST_NAME = "ECDSA Primary Vector";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_ecp_group, grpPtr, pGrp);
    ALLOC_STRUCT(mbedtls_ecp_point, qPtr, pQ);
    ALLOC(hashPtr, pHash, 66);
    ALLOC(rndBufPtr, pRndBuf, 66);

    mbedtls_ecp_group_init(pGrp);
    mbedtls_ecp_point_init(pQ);
    mbedtls_mpi_init(&d);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    mbedtls_mpi_init(&rCheck);
    mbedtls_mpi_init(&sCheck);
    memset(pHash, 0, 66);
    memset(pRndBuf, 0, 66);

    RUNIT_ASSERT(mbedtls_ecp_group_load(pGrp, ECP_GROUP_ID) == 0);
    RUNIT_ASSERT(mbedtls_ecp_point_read_string(pQ, 16, xQ_str, yQ_str) == 0);

    RUNIT_ASSERT(mbedtls_mpi_read_string(&d, 16, d_str) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&rCheck, 16, r_str) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&sCheck, 16, s_str) == 0);

    hlen = runIt_unhexify(pHash, hash_str);

    rnd_info.buf = pRndBuf;
    rnd_info.length = runIt_unhexify(pRndBuf, k_str);

    /* Fix pRndBuf by shifting it left if necessary */
    if (pGrp->nbits % 8 != 0)
    {
        unsigned char shift = 8 - (pGrp->nbits % 8);
        size_t i;

        for (i = 0; i < rnd_info.length - 1; i++)
            pRndBuf[i] = pRndBuf[i] << shift | pRndBuf[i + 1] >> (8 - shift);

        pRndBuf[rnd_info.length - 1] <<= shift;
    }

    RUNIT_ASSERT_API(mbedtls_ecdsa_sign(pGrp, &r, &s, &d, pHash, hlen, runIt_rndBufferRand, &rnd_info) == 0);
    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(&r, &rCheck) == 0);
    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(&s, &sCheck) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdsa_verify(pGrp, pHash, hlen, pQ, &rCheck, &sCheck) == 0);

bail:
    mbedtls_ecp_group_free(pGrp);
    mbedtls_ecp_point_free(pQ);

    mbedtls_mpi_free(&d);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_mpi_free(&rCheck);
    mbedtls_mpi_free(&sCheck);

    FREE_IF_NOT_NULL(grpPtr);
    FREE_IF_NOT_NULL(qPtr);
    FREE_IF_NOT_NULL(hashPtr);
    FREE_IF_NOT_NULL(rndBufPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "ID[%s]", ECP_GROUP_ID_STR);
#endif /* defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) */
    return rc;
}

static RunItError_t runIt_ecdsaDetVectorsTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) && defined(MBEDTLS_SHA256_C)

#if RUNIT_ECDSA_TEST_R521
    mbedtls_ecp_group_id id = MBEDTLS_ECP_DP_SECP521R1;
    static const char *d_str = "0100085F47B8E1B8B11B7EB33028C0B2888E304BFC98501955B45BBA1478DC184EEEDF09B86A5F7C21994406072787205E69A63709FE35AA93BA333514B24F961722";
    static const mbedtls_md_type_t md_alg = MBEDTLS_MD_SHA512;
    static const char *msg = "Example of ECDSA with P-521";
    static const char *r_str = "26B55806BC7F28FF38323A46DB837CCA018FE58B766577498AAC828A76E272D61657E3C386F2FA940CDEC5961F9A81800F1385D2AF365C88B4E341824C5203D0CD";
    static const char *s_str = "01399C2DBA893C4296C9EB06C29E2B87114EE1A8AF7EFF2312737C147AAC69BF77EB4BE520406EADAB45AE2621970B175B0213526B939FCE6B780523C20693B33601";
#else
    mbedtls_ecp_group_id id = ECP_GROUP_ID;
    static const char *d_str = "C9AFA9D845BA75166B5C215767B1D6934E50C3DB36E89B127B8A622B120F6721";
    static const mbedtls_md_type_t md_alg = MD_TYPE_ID;
    static const char *msg = "sample";
    static const char *r_str = "EFD48B2AACB6A8FD1140DD9CD45E81D69D2C877B56AAF991C34D0EA84EAF3716";
    static const char *s_str = "F7CB1C942D657C41D436C7A1B6E29F65F3E900DBB9AFF4064DC4AB2F843ACDA8";
#endif

    size_t hlen;
    const mbedtls_md_info_t *md_info;

    unsigned char *pHash = NULL;
    mbedtls_ecp_group *pGrp = NULL;
    mbedtls_mpi *pD = NULL;
    mbedtls_mpi *pR = NULL;
    mbedtls_mpi *pS = NULL;
    mbedtls_mpi *pRCheck = NULL;
    mbedtls_mpi *pSCheck = NULL;

    RunItPtr hashPtr;
    RunItPtr grpPtr;
    RunItPtr dPtr;
    RunItPtr rPtr;
    RunItPtr sPtr;
    RunItPtr rCheckPtr;
    RunItPtr sCheckPtr;

    const char* TEST_NAME = "ECDSA Det Primary Vector";
    RUNIT_SUB_TEST_START(TEST_NAME);

    md_info = mbedtls_md_info_from_type(md_alg);
    RUNIT_ASSERT(md_info != NULL);

    ALLOC(hashPtr, pHash, MBEDTLS_MD_MAX_SIZE);
    ALLOC_STRUCT(mbedtls_ecp_group, grpPtr, pGrp);
    ALLOC_STRUCT(mbedtls_mpi, dPtr, pD);
    ALLOC_STRUCT(mbedtls_mpi, rPtr, pR);
    ALLOC_STRUCT(mbedtls_mpi, sPtr, pS);
    ALLOC_STRUCT(mbedtls_mpi, rCheckPtr, pRCheck);
    ALLOC_STRUCT(mbedtls_mpi, sCheckPtr, pSCheck);

    memset(pHash, 0, MBEDTLS_MD_MAX_SIZE);
    mbedtls_ecp_group_init(pGrp);
    mbedtls_mpi_init(pD);
    mbedtls_mpi_init(pR);
    mbedtls_mpi_init(pS);
    mbedtls_mpi_init(pRCheck);
    mbedtls_mpi_init(pSCheck);

    RUNIT_ASSERT(mbedtls_ecp_group_load(pGrp, id) == 0);

    RUNIT_ASSERT(mbedtls_mpi_read_string(pD, 16, d_str) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(pRCheck, 16, r_str) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(pSCheck, 16, s_str) == 0);

    hlen = mbedtls_md_get_size(md_info);

    RUNIT_ASSERT_API(mbedtls_md(md_info, (const unsigned char *) msg, strlen(msg), pHash) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdsa_sign_det(pGrp, pR, pS, pD, pHash, hlen, md_alg) == 0);

#if 0
    {
        char buff[300] = {0};
        uint32_t len = 0;
        RUNIT_ASSERT_WITH_RESULT(mbedtls_mpi_write_string(pR, 16, buff, 299, &len), 0);
        RUNIT_PRINT_DBG("pR[%s]\n", buff);
        RUNIT_ASSERT_WITH_RESULT(mbedtls_mpi_write_string(pRCheck, 16, buff, 299, &len), 0);
        RUNIT_PRINT_DBG("pRCheck[%s]\n", buff);

        RUNIT_ASSERT_WITH_RESULT(mbedtls_mpi_write_string(pS, 16, buff, 299, &len), 0);
        RUNIT_PRINT_DBG("pS[%s]\n", buff);
        RUNIT_ASSERT_WITH_RESULT(mbedtls_mpi_write_string(pSCheck, 16, buff, 299, &len), 0);
        RUNIT_PRINT_DBG("pSCheck[%s]\n", buff);
    }
#endif /* 0 */

    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(pR, pRCheck) == 0);
    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(pS, pSCheck) == 0);

bail:
    mbedtls_ecp_group_free(pGrp);

    mbedtls_mpi_free(pD);
    mbedtls_mpi_free(pR);
    mbedtls_mpi_free(pS);
    mbedtls_mpi_free(pRCheck);
    mbedtls_mpi_free(pSCheck);

    FREE_IF_NOT_NULL(hashPtr);
    FREE_IF_NOT_NULL(grpPtr);
    FREE_IF_NOT_NULL(dPtr);
    FREE_IF_NOT_NULL(rPtr);
    FREE_IF_NOT_NULL(sPtr);
    FREE_IF_NOT_NULL(rCheckPtr);
    FREE_IF_NOT_NULL(sCheckPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "ID[%s] MD[%s]", (mbedtls_ecp_curve_info_from_grp_id(id)->name), mbedtls_md_get_name(md_info));
#endif /* defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) && defined(MBEDTLS_SHA256_C) */
    return rc;
}

static RunItError_t runIt_ecdsaWriteReadRandomTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) && defined(MBEDTLS_SHA256_C)
    unsigned char hash[32];
    size_t sig_len;

    mbedtls_ctr_drbg_context *pCtrDrbg = (mbedtls_ctr_drbg_context *)gpRndState;
    mbedtls_ecdsa_context *pCtx = NULL;
    unsigned char *pSig = NULL;

    RunItPtr ctxPtr;
    RunItPtr sigPtr;

    const char* TEST_NAME = "ECDSA Write Read Random";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_ecdsa_context, ctxPtr, pCtx);
    ALLOC(sigPtr, pSig, 200);

    RUNIT_API(mbedtls_ecdsa_init(pCtx));
    memset(hash, 0, sizeof(hash));
    memset(pSig, 0, 200);

    /* prepare material for signature */
    RUNIT_ASSERT_API(mbedtls_ctr_drbg_random(pCtrDrbg, hash, sizeof(hash)) == 0);

    /* generate signing key */
    RUNIT_ASSERT_API(mbedtls_ecdsa_genkey(pCtx, ECP_GROUP_ID, mbedtls_ctr_drbg_random, pCtrDrbg) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdsa_write_signature(pCtx, MD_TYPE_ID, hash, sizeof(hash), pSig, &sig_len, mbedtls_ctr_drbg_random, pCtrDrbg) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdsa_read_signature(pCtx, hash, sizeof(hash), pSig, sig_len) == 0);

bail:
    RUNIT_API(mbedtls_ecdsa_free(pCtx));

    FREE_IF_NOT_NULL(ctxPtr);
    FREE_IF_NOT_NULL(sigPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "ID[%s] MD[%s]", ECP_GROUP_ID_STR, MD_TYPE_ID_STR);
#endif /* defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) && defined(MBEDTLS_SHA256_C) */
    return rc;
}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_ecdsaTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    const char* TEST_NAME = "ECDSA";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_ecdsaPrimRandomTest() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_ecdsaPrimVectorsTest() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_ecdsaDetVectorsTest() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_ecdsaWriteReadRandomTest() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_ecdsaPrimRandomEdwTest() == RUNIT_ERROR__OK);

bail:
    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;
}
#endif /* MBEDTLS_ECDSA_C */
