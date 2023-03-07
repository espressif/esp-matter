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
#include "mbedtls/timing.h"

/* CC lib */
#if defined (CC_CONFIG_SUPPORT_SRP)
#include "mbedtls_cc_srp.h"
#endif

/* pal */
#include "test_pal_mem.h"

/* local */
#include "run_integration_pal_log.h"
#include "run_integration_test.h"
#include "run_integration_helper.h"


/************************************************************
 *
 * defines
 *
 ************************************************************/
#define RUNIT_SRP_DATA_SIZE_MAX 30

/************************************************************
 *
 * static functions prototypes
 *
 ************************************************************/
static RunItError_t runIt_srp(void);

/************************************************************
 *
 * static functions
 *
 ************************************************************/
static RunItError_t runIt_srp(void)
{

    RunItError_t rc = RUNIT_ERROR__OK;

#if defined (CC_CONFIG_SUPPORT_SRP)
    static char userName[] = "alice";
    static size_t userNameSize = 5;
    static char userPwd[] = "password123";
    static size_t userPwdSize = 11;
    static mbedtls_srp_version_t srpVer = CC_SRP_VER_HK;
    static mbedtls_srp_group_param groupParams = {{0xEE,0xAF,0x0A,0xB9,0xAD,0xB3,0x8D,0xD6,0x9C,0x33,0xF8,0x0A,0xFA,0x8F,0xC5,0xE8,0x60,0x72,0x61,0x87,0x75,0xFF,0x3C,0x0B,0x9E,0xA2,0x31,0x4C,0x9C,0x25,0x65,0x76,0xD6,0x74,0xDF,0x74,0x96,0xEA,0x81,0xD3,0x38,0x3B,0x48,0x13,0xD6,0x92,0xC6,0xE0,0xE0,0xD5,0xD8,0xE2,0x50,0xB9,0x8B,0xE4,0x8E,0x49,0x5C,0x1D,0x60,0x89,0xDA,0xD1,0x5D,0xC7,0xD7,0xB4,0x61,0x54,0xD6,0xB6,0xCE,0x8E,0xF4,0xAD,0x69,0xB1,0x5D,0x49,0x82,0x55,0x9B,0x29,0x7B,0xCF,0x18,0x85,0xC5,0x29,0xF5,0x66,0x66,0x0E,0x57,0xEC,0x68,0xED,0xBC,0x3C,0x05,0x72,0x6C,0xC0,0x2F,0xD4,0xCB,0xF4,0x97,0x6E,0xAA,0x9A,0xFD,0x51,0x38,0xFE,0x83,0x76,0x43,0x5B,0x9F,0xC6,0x1D,0x2F,0xC0,0xEB,0x06,0xE3},0x2,1024,0,{0x0,0x0,0x0,0x0,0x0}};
    static CCHashOperationMode_t hashMode = CC_HASH_SHA1_mode;
    static size_t saltSize = 16;
    static size_t privKeySize = 32;

    // set the addresses in phys contig memory
    RunItPtr hostCtxPtr;
    RunItPtr userCtxPtr;
    RunItPtr xBuffPtr;
    RunItPtr uBuffPtr;
    RunItPtr pwdVerifierPtr;
    RunItPtr pubKeyAPtr;
    RunItPtr pubKeyBPtr;
    RunItPtr sessionKeyPtr;
    RunItPtr userNamePtr;
    RunItPtr userPwdPtr;
    RunItPtr saltPtr;

    mbedtls_srp_context *pHostCtx = NULL;
    mbedtls_srp_context *pUserCtx = NULL;
    mbedtls_srp_digest *x_Buff = NULL;
    mbedtls_srp_digest *u_Buff = NULL;
    mbedtls_srp_modulus *pwdVerifier = NULL;
    mbedtls_srp_modulus *pubKeyA = NULL;
    mbedtls_srp_modulus *pubKeyB = NULL;
    mbedtls_srp_modulus *sessionKey = NULL;
    char *pUserName = NULL;
    char *pUserPwd = NULL;
    uint8_t *pSalt = NULL;

    uint8_t *pPwdVerifier = NULL;
    uint8_t *pPubKeyA = NULL;
    uint8_t *pPubKeyB = NULL;
    uint8_t *pSessionKey = NULL;
    uint8_t *pX_Buff = NULL;
    uint8_t *pU_Buff = NULL;

    const char* TEST_NAME = "SRP";
    RUNIT_SUB_TEST_START(TEST_NAME);

    ALLOC_STRUCT(mbedtls_srp_context, hostCtxPtr, pHostCtx);
    ALLOC_STRUCT(mbedtls_srp_context, userCtxPtr, pUserCtx);
    ALLOC_STRUCT(mbedtls_srp_digest, xBuffPtr, x_Buff);
    ALLOC_STRUCT(mbedtls_srp_digest, uBuffPtr, u_Buff);
    ALLOC_STRUCT(mbedtls_srp_modulus, pwdVerifierPtr, pwdVerifier);
    ALLOC_STRUCT(mbedtls_srp_modulus, pubKeyAPtr, pubKeyA);
    ALLOC_STRUCT(mbedtls_srp_modulus, pubKeyBPtr, pubKeyB);
    ALLOC_STRUCT(mbedtls_srp_modulus, sessionKeyPtr, sessionKey);
    ALLOC(userNamePtr, pUserName, RUNIT_SRP_DATA_SIZE_MAX);
    ALLOC(userPwdPtr, pUserPwd, RUNIT_SRP_DATA_SIZE_MAX);
    ALLOC(saltPtr, pSalt, saltSize);

    pPwdVerifier = (uint8_t*)*pwdVerifier;
    pPubKeyA = (uint8_t*)*pubKeyA;
    pPubKeyB = (uint8_t*)*pubKeyB;
    pSessionKey = (uint8_t*)*sessionKey;
    pX_Buff = (uint8_t*)*x_Buff;
    pU_Buff = (uint8_t*)*u_Buff;


    // copy buffers to contig mememory
    memcpy(pUserName, (uint8_t *) userName, userNameSize);
    memcpy(pUserPwd, (uint8_t *) userPwd, userPwdSize);

    // init host ctx
    RUNIT_ASSERT_WITH_RESULT(mbedtls_srp_init(CC_SRP_HOST,
                                        srpVer,
                                        groupParams.modulus,
                                        groupParams.gen,
                                        groupParams.modSizeInBits,
                                        hashMode,
                                        (uint8_t *)pUserName,
                                        userNameSize,
                                        (uint8_t *)pUserPwd,
                                        userPwdSize,
                                        gpRndContext,
                                        pHostCtx),
                             0);

    // init user ctx
    RUNIT_ASSERT_WITH_RESULT(mbedtls_srp_init(CC_SRP_USER,
                                        srpVer,
                                        groupParams.modulus,
                                        groupParams.gen,
                                        groupParams.modSizeInBits,
                                        hashMode,
                                        (uint8_t *)pUserName,
                                        userNameSize,
                                        (uint8_t *)pUserPwd,
                                        userPwdSize,
                                        gpRndContext,
                                        pUserCtx),
                             0);

    // calculates pSalt & password verifier
    RUNIT_ASSERT_WITH_RESULT(mbedtls_srp_pwd_ver_create(saltSize, pSalt, pPwdVerifier, pUserCtx), 0);

    // generate host public & private ephemeral key, known as B & b in RFC
    RUNIT_ASSERT_WITH_RESULT(mbedtls_srp_host_pub_key_create(privKeySize, pPwdVerifier, pPubKeyB, pHostCtx), 0);

    // generate user public & private ephemeral key, known as A & a in RFC
    RUNIT_ASSERT_WITH_RESULT(mbedtls_srp_user_pub_key_create(privKeySize, pPubKeyA, pUserCtx), 0);

    // calculates the user proof
    RUNIT_ASSERT_WITH_RESULT(mbedtls_srp_user_proof_calc(saltSize, pSalt, pPubKeyA, pPubKeyB, pU_Buff, pSessionKey, pUserCtx), 0);

    // verify host session key
    RUNIT_ASSERT_WITH_RESULT(mbedtls_srp_host_proof_verify_and_calc(saltSize, pSalt, pPwdVerifier, pPubKeyA, pPubKeyB, pU_Buff, pX_Buff, pSessionKey, pHostCtx), 0);

    // Verify the host proof
    RUNIT_ASSERT_WITH_RESULT(mbedtls_srp_user_proof_verify(pSessionKey, pPubKeyA, pU_Buff, pX_Buff, pUserCtx), 0);

bail:

    mbedtls_srp_clear(pUserCtx);
    mbedtls_srp_clear(pHostCtx);

    FREE_IF_NOT_NULL(hostCtxPtr);
    FREE_IF_NOT_NULL(userCtxPtr);
    FREE_IF_NOT_NULL(xBuffPtr);
    FREE_IF_NOT_NULL(uBuffPtr);
    FREE_IF_NOT_NULL(pwdVerifierPtr);
    FREE_IF_NOT_NULL(pubKeyAPtr);
    FREE_IF_NOT_NULL(pubKeyBPtr);
    FREE_IF_NOT_NULL(sessionKeyPtr);
    FREE_IF_NOT_NULL(userNamePtr);
    FREE_IF_NOT_NULL(userPwdPtr);
    FREE_IF_NOT_NULL(saltPtr);

    RUNIT_SUB_TEST_RESULT_W_PARAMS(TEST_NAME, "MODE[SHA1] SRP[HK] USER[%"PRIu32"B] PWD[%"PRIu32"B] SALT[%"PRIu32"B] KEY[%"PRIu32"B]",
                                   (uint32_t)userNameSize, (uint32_t)userPwdSize, (uint32_t)saltSize, (uint32_t)privKeySize);
#endif
    return rc;

}


/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_srpTest(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
#if defined (CC_CONFIG_SUPPORT_SRP)
    const char* TEST_NAME = "SRP";
    RUNIT_TEST_START(TEST_NAME);

    RUNIT_ASSERT(runIt_srp() == RUNIT_ERROR__OK);

bail:

    RUNIT_TEST_RESULT(TEST_NAME);
#else
    (void) runIt_srp;
#endif
    return rc;
}
