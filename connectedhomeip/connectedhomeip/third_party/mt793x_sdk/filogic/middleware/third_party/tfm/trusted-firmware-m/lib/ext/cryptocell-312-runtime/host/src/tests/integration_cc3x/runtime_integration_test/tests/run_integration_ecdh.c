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
#include "mbedtls/ecdh.h"
#include "mbedtls/ctr_drbg.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"
#include "mbedtls_cc_ecdh_edwards.h"

#if defined MBEDTLS_ECDH_C
/************************************************************
 *
 * defines
 *
 ************************************************************/

/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static RunItError_t runIt_ecdhPrimRandom(void);
static RunItError_t runIt_ecdhExchange(void);
#if defined (MBEDTLS_ECP_DP_CURVE25519_ENABLED)
static RunItError_t runIt_ecdhExange25519(void);
#endif /* MBEDTLS_ECP_DP_CURVE25519_ENABLED */
/************************************************************
 *
 * static functions
 *
 ************************************************************/
static RunItError_t runIt_ecdhPrimRandom(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    static const char *dA_str = "C88F01F510D9AC3F70A292DAA2316DE544E9AAB8AFE84049C62A9C57862D1433";
    static const char *xA_str = "DAD0B65394221CF9B051E1FECA5787D098DFE637FC90B9EF945D0C3772581180";
    static const char *yA_str = "5271A0461CDB8252D61F1C456FA3E59AB1F45B33ACCF5F58389E0577B8990BB3";
    static const char *dB_str = "C6EF9C5D78AE012A011164ACB397CE2088685D8F06BF9BE0B283AB46476BEE53";
    static const char *xB_str = "D12DFB5289C8D4F81208B70270398C342296970A0BCCB74C736FC7554494BF63";
    static const char *yB_str = "56FBF3CA366CC23E8157854C13C58D6AAC23F046ADA30F8353E74F33039872AB";
    static const char *z_str = "D6840F6B42F6EDAFD13116E0E12565202FEF8E9ECE7DCE03812464D04B9442DE";

    mbedtls_ecp_group *pGrp = NULL;
    mbedtls_ecp_point *pQA = NULL;
    mbedtls_ecp_point *pQB = NULL;
    unsigned char *pRndBufA = NULL;
    unsigned char *pRndBufB = NULL;
    mbedtls_mpi dA;
    mbedtls_mpi dB;
    mbedtls_mpi zA;
    mbedtls_mpi zB;
    mbedtls_mpi check;

    RunItPtr grpPtr;
    RunItPtr qAPtr;
    RunItPtr qBPtr;
    RunItPtr rndBufAPtr;
    RunItPtr rndBufBPtr;

    rnd_buf_info rnd_info_A;
    rnd_buf_info rnd_info_B;

    const char* TEST_NAME = "ECDH Vectors";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_ecp_group, grpPtr, pGrp);
    ALLOC_STRUCT(mbedtls_ecp_point, qAPtr, pQA);
    ALLOC_STRUCT(mbedtls_ecp_point, qBPtr, pQB);
    ALLOC(rndBufAPtr, pRndBufA, MBEDTLS_ECP_MAX_BYTES);
    ALLOC(rndBufBPtr, pRndBufB, MBEDTLS_ECP_MAX_BYTES);

    mbedtls_ecp_group_init(pGrp);
    mbedtls_ecp_point_init(pQA);
    mbedtls_ecp_point_init(pQB);
    mbedtls_mpi_init(&dA);
    mbedtls_mpi_init(&dB);
    mbedtls_mpi_init(&zA);
    mbedtls_mpi_init(&zB);
    mbedtls_mpi_init(&check);

    RUNIT_ASSERT(mbedtls_ecp_group_load(pGrp, MBEDTLS_ECP_DP_SECP256R1) == 0);

    rnd_info_A.buf = pRndBufA;
    rnd_info_A.length = runIt_unhexify(pRndBufA, dA_str);

    /* Fix rnd_buf_A by shifting it left if necessary */
    if (pGrp->nbits % 8 != 0)
    {
        unsigned char shift = 8 - (pGrp->nbits % 8);
        size_t i;

        for (i = 0; i < rnd_info_A.length - 1; i++)
            pRndBufA[i] = pRndBufA[i] << shift | pRndBufA[i + 1] >> (8 - shift);

        pRndBufA[rnd_info_A.length - 1] <<= shift;
    }

    rnd_info_B.buf = pRndBufB;
    rnd_info_B.length = runIt_unhexify(pRndBufB, dB_str);

    /* Fix rnd_buf_B by shifting it left if necessary */
    if (pGrp->nbits % 8 != 0)
    {
        unsigned char shift = 8 - (pGrp->nbits % 8);
        size_t i;

        for (i = 0; i < rnd_info_B.length - 1; i++)
            pRndBufB[i] = pRndBufB[i] << shift | pRndBufB[i + 1] >> (8 - shift);

        pRndBufB[rnd_info_B.length - 1] <<= shift;
    }

    RUNIT_ASSERT_API(mbedtls_ecdh_gen_public(pGrp, &dA, pQA, runIt_rndBufferRand, &rnd_info_A) == 0);
    RUNIT_ASSERT(mbedtls_ecp_is_zero(pQA) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&check, 16, xA_str) == 0);
    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(&pQA->X, &check) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&check, 16, yA_str) == 0);
    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(&pQA->Y, &check) == 0);

    RUNIT_ASSERT_API(mbedtls_ecdh_gen_public(pGrp, &dB, pQB, runIt_rndBufferRand, &rnd_info_B) == 0);
    RUNIT_ASSERT(mbedtls_ecp_is_zero(pQB) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&check, 16, xB_str) == 0);
    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(&pQB->X, &check) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&check, 16, yB_str) == 0);
    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(&pQB->Y, &check) == 0);

    RUNIT_ASSERT(mbedtls_mpi_read_string(&check, 16, z_str) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdh_compute_shared( pGrp, &zA, pQB, &dA, NULL, NULL ) == 0);
    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(&zA, &check) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdh_compute_shared( pGrp, &zB, pQA, &dB, NULL, NULL ) == 0);
    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(&zB, &check) == 0);

bail:

    mbedtls_ecp_group_free(pGrp);
    mbedtls_ecp_point_free(pQA);
    mbedtls_ecp_point_free(pQB);

    mbedtls_mpi_free(&dA);
    mbedtls_mpi_free(&dB);
    mbedtls_mpi_free(&zA);
    mbedtls_mpi_free(&zB);
    mbedtls_mpi_free(&check);

    FREE_IF_NOT_NULL(grpPtr);
    FREE_IF_NOT_NULL(qAPtr);
    FREE_IF_NOT_NULL(qBPtr);
    FREE_IF_NOT_NULL(rndBufAPtr);
    FREE_IF_NOT_NULL(rndBufBPtr);

    RUNIT_SUB_TEST_RESULT(TEST_NAME);

    return rc;
}

static RunItError_t runIt_ecdhExchange(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    static const uint32_t BUF_SIZE = 1000;

    mbedtls_ecdh_context *pSrv = NULL;
    mbedtls_ecdh_context *pCli = NULL;
    unsigned char *pBuf = NULL;

    RunItPtr srvPtr = {0};
    RunItPtr cliPtr = {0};
    RunItPtr bufPtr = {0};

    const unsigned char *vbuf;
    size_t len;

    const char* TEST_NAME = "ECDH Exchange";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_ecdh_context, srvPtr, pSrv);
    ALLOC_STRUCT(mbedtls_ecdh_context, cliPtr, pCli);
    ALLOC(bufPtr, pBuf, BUF_SIZE);

    RUNIT_API(mbedtls_ecdh_init(pSrv));
    RUNIT_API(mbedtls_ecdh_init(pCli));

    RUNIT_ASSERT(mbedtls_ecp_group_load(&pSrv->grp, MBEDTLS_ECP_DP_SECP256R1) == 0);

    memset(pBuf, 0x00, BUF_SIZE);
    vbuf = pBuf;
    RUNIT_ASSERT_API(mbedtls_ecdh_make_params(pSrv, &len, pBuf, BUF_SIZE, mbedtls_ctr_drbg_random, gpRndState) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdh_read_params(pCli, &vbuf, pBuf + len) == 0);

    memset(pBuf, 0x00, BUF_SIZE);
    RUNIT_ASSERT_API(mbedtls_ecdh_make_public(pCli, &len, pBuf, BUF_SIZE, mbedtls_ctr_drbg_random, gpRndState) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdh_read_public(pSrv, pBuf, len) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdh_calc_secret(pSrv, &len, pBuf, BUF_SIZE, mbedtls_ctr_drbg_random, gpRndState) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdh_calc_secret(pCli, &len, pBuf, BUF_SIZE, NULL, NULL) == 0);
    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(&pSrv->z, &pCli->z) == 0);

bail:
    RUNIT_API(mbedtls_ecdh_free(pSrv));
    RUNIT_API(mbedtls_ecdh_free(pCli));

    FREE_IF_NOT_NULL(srvPtr);
    FREE_IF_NOT_NULL(cliPtr);
    FREE_IF_NOT_NULL(bufPtr);

    RUNIT_SUB_TEST_RESULT(TEST_NAME);
    return rc;
}

#if defined (MBEDTLS_ECP_DP_CURVE25519_ENABLED)
static RunItError_t runIt_ecdhExange25519(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    static const uint32_t BUF_SIZE = 1000;

    mbedtls_ecdh_context *pSrv = NULL;
    mbedtls_ecdh_context *pCli = NULL;
    unsigned char *pBuf = NULL;

    RunItPtr srvPtr = {0};
    RunItPtr cliPtr = {0};
    RunItPtr bufPtr = {0};

    const unsigned char *vbuf;
    size_t len;

    const char* TEST_NAME = "ECDH 25519 Exchange";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_ecdh_context, srvPtr, pSrv);
    ALLOC_STRUCT(mbedtls_ecdh_context, cliPtr, pCli);
    ALLOC(bufPtr, pBuf, BUF_SIZE);

    RUNIT_API(mbedtls_ecdh_init(pSrv));
    RUNIT_API(mbedtls_ecdh_init(pCli));

    RUNIT_ASSERT(mbedtls_ecp_group_load(&pSrv->grp, MBEDTLS_ECP_DP_CURVE25519) == 0);

    memset(pBuf, 0x00, BUF_SIZE);
    vbuf = pBuf;

    RUNIT_ASSERT_API(mbedtls_ecdh_make_params_edwards(pSrv, &len, pBuf, BUF_SIZE, mbedtls_ctr_drbg_random, gpRndState) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdh_read_params_edwards(pCli, &vbuf, pBuf + len) == 0);

    memset(pBuf, 0x00, BUF_SIZE);
    RUNIT_ASSERT_API(mbedtls_ecdh_make_public(pCli, &len, pBuf, BUF_SIZE, mbedtls_ctr_drbg_random, gpRndState) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdh_read_public(pSrv, pBuf, len) == 0);

    RUNIT_ASSERT_API(mbedtls_ecdh_calc_secret(pSrv, &len, pBuf, BUF_SIZE, mbedtls_ctr_drbg_random, gpRndState) == 0);
    RUNIT_ASSERT_API(mbedtls_ecdh_calc_secret(pCli, &len, pBuf, BUF_SIZE, NULL, NULL) == 0);

    RUNIT_ASSERT(mbedtls_mpi_cmp_mpi(&pSrv->z, &pCli->z) == 0);

bail:
    RUNIT_API(mbedtls_ecdh_free(pSrv));
    RUNIT_API(mbedtls_ecdh_free(pCli));

    FREE_IF_NOT_NULL(srvPtr);
    FREE_IF_NOT_NULL(cliPtr);
    FREE_IF_NOT_NULL(bufPtr);

    RUNIT_SUB_TEST_RESULT(TEST_NAME);
    return rc;

}
#endif /* MBEDTLS_ECP_DP_CURVE25519_ENABLED */
/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_ecdhTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    const char* TEST_NAME = "ECDH";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_ecdhPrimRandom() == RUNIT_ERROR__OK);
    RUNIT_ASSERT(runIt_ecdhExchange() == RUNIT_ERROR__OK);
#if defined (MBEDTLS_ECP_DP_CURVE25519_ENABLED)
    RUNIT_ASSERT(runIt_ecdhExange25519() == RUNIT_ERROR__OK);
#endif /* MBEDTLS_ECP_DP_CURVE25519_ENABLED */
    bail:
    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;
}
#endif /* MBEDTLS_ECDH_C */
