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
#include "mbedtls/dhm.h"
#include "mbedtls/timing.h"
#include "mbedtls/ctr_drbg.h"

/* CC pal */

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"

/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static RunItError_t runIt_dhm(void);
/************************************************************
 *
 * static functions
 *
 ************************************************************/
static RunItError_t runIt_dhm(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if defined(MBEDTLS_DHM_C)

    static const int radix_P = 10;
    static const char *input_P = "93450983094850938450983409623982317398171298719873918739182739712938719287391879381271";
    static const int radix_G = 10;
    static const char *input_G = "9345098309485093845098340962223981329819812792137312973297123912791271";

    static const int BUF_SIZE = 1000;

    mbedtls_dhm_context *pCtxSrv = NULL;
    mbedtls_dhm_context *pCtxCli = NULL;
    unsigned char *pSke = NULL;
    unsigned char *pPubCli = NULL;
    unsigned char *pSecSrv = NULL;
    unsigned char *pSecCli = NULL;

    RunItPtr ctxSrvPtr;
    RunItPtr ctxCliPtr;
    RunItPtr skePrt;
    RunItPtr pubCliPrt;
    RunItPtr secSrvPrt;
    RunItPtr secCliPrt;

    unsigned char *p = NULL;
    size_t ske_len = 0;
    size_t pub_cli_len = 0;
    size_t sec_srv_len;
    size_t sec_cli_len;
    int x_size, i;

    const char* TEST_NAME = "DH";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_dhm_context, ctxSrvPtr, pCtxSrv);
    ALLOC_STRUCT(mbedtls_dhm_context, ctxCliPtr, pCtxCli);
    ALLOC(skePrt, pSke, BUF_SIZE);
    ALLOC(pubCliPrt, pPubCli, BUF_SIZE);
    ALLOC(secSrvPrt, pSecSrv, BUF_SIZE);
    ALLOC(secCliPrt, pSecCli, BUF_SIZE);


    RUNIT_API(mbedtls_dhm_init(pCtxSrv));
    RUNIT_API(mbedtls_dhm_init(pCtxCli));
    memset(pSke, 0x00, BUF_SIZE);
    memset(pPubCli, 0x00, BUF_SIZE);
    memset(pSecSrv, 0x00, BUF_SIZE);
    memset(pSecCli, 0x00, BUF_SIZE);

    /*
     * Set params
     */
    RUNIT_ASSERT(mbedtls_mpi_read_string(&pCtxSrv->P, radix_P, input_P) == 0);
    RUNIT_ASSERT(mbedtls_mpi_read_string(&pCtxSrv->G, radix_G, input_G) == 0);
    x_size = mbedtls_mpi_size(&pCtxSrv->P);
    pub_cli_len = x_size;

    /*
     * First key exchange
     */
    p = pSke;

    RUNIT_ASSERT_API(mbedtls_dhm_make_params(pCtxSrv, x_size, pSke, &ske_len, mbedtls_ctr_drbg_random, gpRndState) == 0);
    pSke[ske_len++] = 0;
    pSke[ske_len++] = 0;

    RUNIT_ASSERT_API(mbedtls_dhm_read_params(pCtxCli, &p, pSke + ske_len) == 0);

    RUNIT_ASSERT_API(mbedtls_dhm_make_public(pCtxCli, x_size, pPubCli, pub_cli_len, mbedtls_ctr_drbg_random, gpRndState) == 0);
    RUNIT_ASSERT_API(mbedtls_dhm_read_public(pCtxSrv, pPubCli, pub_cli_len) == 0);

    RUNIT_ASSERT_API(mbedtls_dhm_calc_secret(pCtxSrv, pSecSrv, BUF_SIZE, &sec_srv_len, mbedtls_ctr_drbg_random, gpRndState) == 0);
    RUNIT_ASSERT_API(mbedtls_dhm_calc_secret(pCtxCli, pSecCli, BUF_SIZE, &sec_cli_len, NULL, NULL ) == 0);

    RUNIT_ASSERT(sec_srv_len == sec_cli_len);
    RUNIT_ASSERT(sec_srv_len != 0);
    RUNIT_ASSERT(memcmp(pSecSrv, pSecCli, sec_srv_len) == 0);

    /* Re-do calc_secret on server a few times to test update of blinding values */
    for (i = 0; i < 3; i++)
    {
        sec_srv_len = 1000;
        RUNIT_ASSERT_API(mbedtls_dhm_calc_secret(pCtxSrv, pSecSrv, BUF_SIZE, &sec_srv_len, mbedtls_ctr_drbg_random, gpRndState) == 0);

        RUNIT_ASSERT(sec_srv_len == sec_cli_len);
        RUNIT_ASSERT(sec_srv_len != 0);
        RUNIT_ASSERT(memcmp(pSecSrv, pSecCli, sec_srv_len) == 0);
    }

    /*
     * Second key exchange to test change of blinding values on server
     */
    p = pSke;

    RUNIT_ASSERT_API(mbedtls_dhm_make_params(pCtxSrv, x_size, pSke, &ske_len, mbedtls_ctr_drbg_random, gpRndState) == 0);
    pSke[ske_len++] = 0;
    pSke[ske_len++] = 0;
    RUNIT_ASSERT_API(mbedtls_dhm_read_params(pCtxCli, &p, pSke + ske_len) == 0);

    RUNIT_ASSERT_API(mbedtls_dhm_make_public(pCtxCli, x_size, pPubCli, pub_cli_len, mbedtls_ctr_drbg_random, gpRndState) == 0);
    RUNIT_ASSERT_API(mbedtls_dhm_read_public(pCtxSrv, pPubCli, pub_cli_len) == 0);

    RUNIT_ASSERT_API(mbedtls_dhm_calc_secret(pCtxSrv, pSecSrv, BUF_SIZE, &sec_srv_len, mbedtls_ctr_drbg_random, gpRndState) == 0);
    RUNIT_ASSERT_API(mbedtls_dhm_calc_secret(pCtxCli, pSecCli, BUF_SIZE, &sec_cli_len, NULL, NULL ) == 0);

    RUNIT_ASSERT(sec_srv_len == sec_cli_len);
    RUNIT_ASSERT(sec_srv_len != 0);
    RUNIT_ASSERT(memcmp(pSecSrv, pSecCli, sec_srv_len) == 0);

bail:
    RUNIT_API(mbedtls_dhm_free(pCtxSrv));
    RUNIT_API(mbedtls_dhm_free(pCtxCli));

    FREE_IF_NOT_NULL(ctxSrvPtr);
    FREE_IF_NOT_NULL(ctxCliPtr);
    FREE_IF_NOT_NULL(skePrt);
    FREE_IF_NOT_NULL(pubCliPrt);
    FREE_IF_NOT_NULL(secSrvPrt);
    FREE_IF_NOT_NULL(secCliPrt);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "G[%"PRIu32"b]", (uint32_t)strlen(input_G) * 8);
#endif /* MBEDTLS_DHM_C */
    return rc;
}

RunItError_t runIt_dhmTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    const char* TEST_NAME = "DH";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_dhm() == RUNIT_ERROR__OK);

bail:
    RUNIT_TEST_RESULT(TEST_NAME);
    return rc;

}
