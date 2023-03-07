/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* system includes */
#include <stdlib.h>


/* cerberus pal */
#include "cc_pal_types.h"
#include "cc_pal_types_plat.h"
#include "cc_pal_mem.h"
#include "cc_pal_perf.h"
#include "cc_regs.h"
#include "cc_otp_defs.h"
#include "cc_lib.h"
#include "cc_rnd_common.h"
#include "mbedtls_cc_mng.h"

/* tests pal and hal */
#include "board_configs.h"
#include "test_pal_mem.h"
#include "test_pal_mem_s.h"
#include "test_pal_thread.h"
#include "test_pal_log.h"
#include "test_proj_otp.h"

/* mbedtls */
#include "mbedtls/memory_buffer_alloc.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

/* internal files */
#include "run_integration_test.h"
#include "run_integration_test_api.h"
#include "run_integration_helper.h"
#include "run_integration_pal_log.h"
#include "run_integration_pal_otp.h"
#include "run_integration_pal_reg.h"
#include "run_integration_profiler.h"
#include "run_integration_flash.h"
#include "run_integration_otp.h"

/************************************************************
 *
 * defines
 *
 ************************************************************/
#define RUNIT_FLASH_SIZE                        0x10000
#define RUNIT_TEST_NAME_LENGTH                  50
#define RUNIT_PTHREAD_STACK_MIN                 16384
#define RUNIT_MBEDTLS_HEAP_MIN                  1024*512

#ifndef RUNIT_TEST_ITER_MAX
#define RUNIT_TEST_ITER_MAX 1
#endif

/************************************************************
 *
 * macros
 *
 ************************************************************/
typedef struct ImpCong_t
{
    uint8_t md2_process;
    uint8_t md4_process;
    uint8_t md5_process;
    uint8_t ripemd160_process;
    uint8_t sha1_process;
    uint8_t sha256_process;
    uint8_t sha512_process;
    uint8_t des_setkey;
    uint8_t des_crypt;
    uint8_t des3_crypt;
    uint8_t aes_setkey;
    uint8_t aes_setkey_dec;
    uint8_t aes_encrypt;
    uint8_t aes_decrypt;
    uint8_t aes;
    uint8_t ccm;
    uint8_t gcm;
    uint8_t sha1;
    uint8_t sha256;
    uint8_t sha512;
    uint8_t rsa;
    uint8_t dhm;
    uint8_t ecc;
    uint8_t ecp;
    uint8_t entropy_hardware;
    uint8_t pk_rsa_alt;
    uint8_t cmac;
    uint8_t mbedtls_ecdh_gen_public_alt;
    uint8_t mbedtls_ecdh_compute_shared_alt;
    uint8_t mbedtls_ecdsa_verify_alt;
    uint8_t mbedtls_ecdsa_sign_alt;
    uint8_t mbedtls_ecdsa_genkey_alt;
} ImpCong_t;

/************************************************************
 *
 * static function prototypes
 *
 ************************************************************/
static void runIt_finish(void);
static RunItError_t runIt_init(void);
static void* runIt_executer(void *params);

/************************************************************
 *
 * variables
 *
 ************************************************************/
/* pass result between executer and main thread */
RunItError_t g_runIt_executerRc = RUNIT_ERROR__FAIL;

/* ptr to the heap managed by the mbedtls library. to be freed at a later point */
uint8_t* gRunIt_mbedtlsHeap_ptr = NULL;

/* RndContext and RndState to be used by the tests */
CCRndContext_t* gpRndContext = NULL;
mbedtls_ctr_drbg_context* gpRndState = NULL;

/* RunItPtr to be freed at a later point */
RunItPtr gRunIt_rndContext_ptr;
RunItPtr gRunIt_rndWorkBuff_ptr;
RunItPtr gRunIt_rndState_ptr;
RunItPtr gRunIt_entropy_ptr;

/* for debugging */
uint32_t gRunItStackSize = 0;
uint8_t gRunItPrintEnable = 1;
ImpCong_t gCompConf = { 0 };
/************************************************************
 *
 * static functions
 *
 ************************************************************/
static void runIt_finish(void)
{
    runIt_perfFin();

    CC_LibFini((CCRndContext_t *)GET_PTR(gRunIt_rndContext_ptr));

    FREE_IF_NOT_NULL(gRunIt_rndContext_ptr);
    FREE_IF_NOT_NULL(gRunIt_rndWorkBuff_ptr);
    FREE_IF_NOT_NULL(gRunIt_rndState_ptr);
    FREE_IF_NOT_NULL(gRunIt_entropy_ptr);

    RUNIT_TEST_PAL_FREE(gRunIt_mbedtlsHeap_ptr);
}

static RunItError_t runIt_init(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;

    uint32_t* mbedtlsHeapAligned = NULL;

    RunItPtr rndContext;
    RunItPtr rndWorkBuff;
    RunItPtr rndState;
    RunItPtr mbedtlsEntropy;

    CCRndContext_t* pRndContex = NULL;
    CCRndWorkBuff_t* pRndWorkBuff = NULL;
    mbedtls_ctr_drbg_context* pRndState = NULL;
    mbedtls_entropy_context* pMbedtlsEntropy = NULL;

    /* initilaise mbedtls heap manager */
    ALLOC_BUFF_ALIGN32(gRunIt_mbedtlsHeap_ptr, mbedtlsHeapAligned, RUNIT_MBEDTLS_HEAP_MIN);
    mbedtls_memory_buffer_alloc_init((uint8_t*)mbedtlsHeapAligned, RUNIT_MBEDTLS_HEAP_MIN);

    /* use global ptr to be able to free them on exit */
    ALLOC_STRUCT(CCRndContext_t, rndContext, pRndContex);
    ALLOC_STRUCT(CCRndWorkBuff_t, rndWorkBuff, pRndWorkBuff);
    ALLOC_STRUCT(mbedtls_ctr_drbg_context, rndState, pRndState);
    ALLOC_STRUCT(mbedtls_entropy_context, mbedtlsEntropy, pMbedtlsEntropy);

    /* init Rnd context's inner member */
    pRndContex->rndState = pRndState;
    pRndContex->entropyCtx = pMbedtlsEntropy;

    /* save pointers staticaly to free them at a  later point */
    gRunIt_rndContext_ptr = rndContext;
    gRunIt_rndWorkBuff_ptr = rndWorkBuff;
    gRunIt_rndState_ptr = rndState;
    gRunIt_entropy_ptr = mbedtlsEntropy;

    /* set global vars */
    gpRndContext = pRndContex;
    gpRndState = pRndState;


    /* initialise CC library */
    RUNIT_ASSERT(CC_LibInit(pRndContex, pRndWorkBuff) == CC_LIB_RET_OK);

    return RUNIT_ERROR__OK;

bail:
    runIt_finish();

    return rc;

}

static void runIt_printMbedtlsAltConfig(void)
{
    static const char* SOFTWARE = "software";
    static const char* HARDWARE = "hardware";
    static const char* MBEDTLS_CONFIG_FORMAT = "%-40.40s %-10.10s\n";
    static const char* DASH = "----------------------------------------------------------------------------------";

    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "define", "state");
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, DASH, DASH);

    (void)HARDWARE;
    (void)SOFTWARE;

#if defined(MBEDTLS_MD2_PROCESS_ALT)
    gCompConf.md2_process = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_MD2_PROCESS_ALT", HARDWARE);
#else
    gCompConf.md2_process = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_MD2_PROCESS_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_MD4_PROCESS_ALT)
    gCompConf.md4_process = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_MD4_PROCESS_ALT", HARDWARE);
#else
    gCompConf.md4_process = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_MD4_PROCESS_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_MD5_PROCESS_ALT)
    gCompConf.md5_process = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_MD5_PROCESS_ALT", HARDWARE);
#else
    gCompConf.md5_process = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_MD5_PROCESS_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_RIPEMD160_PROCESS_ALT)
    gCompConf.ripemd160_process = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_RIPEMD160_PROCESS_ALT", HARDWARE);
#else
    gCompConf.ripemd160_process = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_RIPEMD160_PROCESS_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_SHA1_PROCESS_ALT)
    gCompConf.sha1_process = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA1_PROCESS_ALT", HARDWARE);
#else
    gCompConf.sha1_process = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA1_PROCESS_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_SHA256_PROCESS_ALT)
    gCompConf.sha256_process = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA256_PROCESS_ALT", HARDWARE);
#else
    gCompConf.sha256_process = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA256_PROCESS_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_SHA512_PROCESS_ALT)
    gCompConf.sha512_process = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA512_PROCESS_ALT", HARDWARE);
#else
    gCompConf.sha512_process = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA512_PROCESS_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_DES_SETKEY_ALT)
    gCompConf.des_setkey = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_DES_SETKEY_ALT", HARDWARE);
#else
    gCompConf.des_setkey = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_DES_SETKEY_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_DES_CRYPT_ECB_ALT)
    gCompConf.des_crypt = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_DES_CRYPT_ECB_ALT", HARDWARE);
#else
    gCompConf.des_crypt = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_DES_CRYPT_ECB_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_DES3_CRYPT_ECB_ALT)
    gCompConf.des3_crypt = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_DES3_CRYPT_ECB_ALT", HARDWARE);
#else
    gCompConf.des3_crypt = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_DES3_CRYPT_ECB_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_AES_SETKEY_ENC_ALT)
    gCompConf.des_setkey = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_AES_SETKEY_ENC_ALT", HARDWARE);
#else
    gCompConf.des_setkey = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_AES_SETKEY_ENC_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_AES_SETKEY_DEC_ALT)
    gCompConf.aes_setkey = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_AES_SETKEY_DEC_ALT", HARDWARE);
#else
    gCompConf.aes_setkey = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_AES_SETKEY_DEC_ALT", SOFTWARE);
#endif


#if defined(MBEDTLS_AES_ENCRYPT_ALT)
    gCompConf.aes_encrypt = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_AES_ENCRYPT_ALT", HARDWARE);
#else
    gCompConf.aes_encrypt = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_AES_ENCRYPT_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_AES_DECRYPT_ALT)
    gCompConf.aes_decrypt = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_AES_DECRYPT_ALT", HARDWARE);
#else
    gCompConf.aes_decrypt = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_AES_DECRYPT_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_AES_ALT)
    gCompConf.aes = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_AES_ALT", HARDWARE);
#else
    gCompConf.aes = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_AES_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_CCM_ALT)
    gCompConf.ccm = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_CCM_ALT", HARDWARE);
#else
    gCompConf.ccm = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_CCM_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_GCM_ALT)
    gCompConf.gcm = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_GCM_ALT", HARDWARE);
#else
    gCompConf.gcm = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_GCM_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_SHA1_ALT)
    gCompConf.sha1 = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA1_ALT", HARDWARE);
#else
    gCompConf.sha1 = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA1_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_SHA256_ALT)
    gCompConf.sha256 = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA256_ALT", HARDWARE);
#else
    gCompConf.sha256 = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA256_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_SHA512_ALT)
    gCompConf.sha512 = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA512_ALT", HARDWARE);
#else
    gCompConf.sha512 = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_SHA512_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_RSA_ALT)
    gCompConf.rsa = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_RSA_ALT", HARDWARE);
#else
    gCompConf.rsa = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_RSA_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_DHM_ALT)
    gCompConf.dhm = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_DHM_ALT", HARDWARE);
#else
    gCompConf.dhm = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_DHM_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_ECC_ALT)
    gCompConf.ecc = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECC_ALT", HARDWARE);
#else
    gCompConf.ecc = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECC_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_ECP_ALT)
    gCompConf.ecp = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECP_ALT", HARDWARE);
#else
    gCompConf.ecp = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECP_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
    gCompConf.entropy_hardware = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ENTROPY_HARDWARE_ALT", HARDWARE);
#else
    gCompConf.entropy_hardware = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ENTROPY_HARDWARE_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_PK_RSA_ALT_SUPPORT)
    gCompConf.pk_rsa_alt = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_PK_RSA_ALT_SUPPORT", HARDWARE);
#else
    gCompConf.pk_rsa_alt = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_PK_RSA_ALT_SUPPORT", SOFTWARE);
#endif

#if defined(MBEDTLS_CMAC_ALT)
    gCompConf.cmac = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_CMAC_ALT", HARDWARE);
#else
    gCompConf.cmac = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_CMAC_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT)
    gCompConf.mbedtls_ecdh_gen_public_alt = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECDH_GEN_PUBLIC_ALT", HARDWARE);
#else
    gCompConf.mbedtls_ecdh_gen_public_alt = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECDH_GEN_PUBLIC_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT)
    gCompConf.mbedtls_ecdh_compute_shared_alt = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECDH_COMPUTE_SHARED_ALT", HARDWARE);
#else
    gCompConf.mbedtls_ecdh_compute_shared_alt = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECDH_COMPUTE_SHARED_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_ECDSA_VERIFY_ALT)
    gCompConf.mbedtls_ecdsa_verify_alt = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECDSA_VERIFY_ALT", HARDWARE);
#else
    gCompConf.mbedtls_ecdsa_verify_alt = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECDSA_VERIFY_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_ECDSA_SIGN_ALT)
    gCompConf.mbedtls_ecdsa_sign_alt = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECDSA_SIGN_ALT", HARDWARE);
#else
    gCompConf.mbedtls_ecdsa_sign_alt = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECDSA_SIGN_ALT", SOFTWARE);
#endif

#if defined(MBEDTLS_ECDSA_GENKEY_ALT)
    gCompConf.mbedtls_ecdsa_genkey_alt = 1;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECDSA_GENKEY_ALT", HARDWARE);
#else
    gCompConf.mbedtls_ecdsa_genkey_alt = 0;
    RUNIT_PRINT(MBEDTLS_CONFIG_FORMAT, "MBEDTLS_ECDSA_GENKEY_ALT", SOFTWARE);
#endif

}

static void runIt_regApi(void)
{
    RUNIT_PERF_REG_API(mbedtls_aes_crypt_cbc                              , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_crypt_cfb128                           , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_crypt_cfb8                             , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_crypt_ctr                              , gCompConf.aes);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_aes_crypt_ctr, "16B"               , gCompConf.aes);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_aes_crypt_ctr, "128B"              , gCompConf.aes);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_aes_crypt_ctr, "1024B"             , gCompConf.aes);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_aes_crypt_ctr, "8192B"             , gCompConf.aes);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_aes_crypt_ctr, "65535B"            , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_crypt_ecb                              , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_crypt_ofb                              , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_ext_dma_init                           , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_ext_dma_set_key                        , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_ext_dma_set_iv                         , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_ext_dma_finish                         , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_free                                   , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_init                                   , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_key_wrap                               , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_key_unwrap                             , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_setkey_dec                             , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_aes_setkey_enc                             , gCompConf.aes);
    RUNIT_PERF_REG_API(mbedtls_ccm_init                                   , gCompConf.ccm);
    RUNIT_PERF_REG_API(mbedtls_ccm_setkey                                 , gCompConf.ccm);
    RUNIT_PERF_REG_API(mbedtls_ccm_encrypt_and_tag                        , gCompConf.ccm);
    RUNIT_PERF_REG_API(mbedtls_ccm_auth_decrypt                           , gCompConf.ccm);
    RUNIT_PERF_REG_API(mbedtls_ccm_free                                   , gCompConf.ccm);
    RUNIT_PERF_REG_API(mbedtls_ccm_star_nonce_generate                    , 1);
    RUNIT_PERF_REG_API(mbedtls_ccm_star_encrypt_and_tag                   , 1);
    RUNIT_PERF_REG_API(mbedtls_ccm_star_auth_decrypt                      , 1);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_chacha, "encrypt"                  , 1);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_chacha, "decrypt"                  , 1);
    RUNIT_PERF_REG_API(mbedtls_chacha_init                                , 1);
    RUNIT_PERF_REG_API(mbedtls_chacha_finish                              , 1);
    RUNIT_PERF_REG_API(mbedtls_chacha_free                                , 1);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_chacha_poly, "encrypt"             , 1);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_chacha_poly, "decrypt"             , 1);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_chacha20_crypt, "encrypt"          , 1);
    RUNIT_PERF_REG_API(mbedtls_chacha20_init                              , 1);
    RUNIT_PERF_REG_API(mbedtls_chacha20_free                              , 1);
    RUNIT_PERF_REG_API(mbedtls_chacha20_setkey                            , 1);
    RUNIT_PERF_REG_API(mbedtls_chacha20_starts                            , 1);
    RUNIT_PERF_REG_API(mbedtls_chacha20_update                            , 1);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_chachapoly_auth_decrypt, "decrypt" , 1);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_chachapoly_encrypt_and_tag, "encrypt" , 1);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_chachapoly_encrypt_and_tag, "decrypt" , 1);
    RUNIT_PERF_REG_API(mbedtls_chachapoly_init                            , 1);
    RUNIT_PERF_REG_API(mbedtls_chachapoly_free                            , 1);
    RUNIT_PERF_REG_API(mbedtls_chachapoly_setkey                          , 1);
    RUNIT_PERF_REG_API(mbedtls_cipher_cmac_finish                         , gCompConf.cmac);
    RUNIT_PERF_REG_API(mbedtls_cipher_cmac_starts                         , gCompConf.cmac);
    RUNIT_PERF_REG_API(mbedtls_cipher_cmac_update                         , gCompConf.cmac);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_cipher_cmac_update, "16B"          , gCompConf.cmac);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_cipher_cmac_update, "128B"         , gCompConf.cmac);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_cipher_cmac_update, "1024B"        , gCompConf.cmac);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_cipher_cmac_update, "8192B"        , gCompConf.cmac);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_cipher_cmac_update, "65535B"       , gCompConf.cmac);
    RUNIT_PERF_REG_API(mbedtls_cipher_cmac_reset                          , gCompConf.cmac);
    RUNIT_PERF_REG_API(mbedtls_cipher_cmac                                , gCompConf.cmac);
    RUNIT_PERF_REG_API(mbedtls_cipher_free                                , 0);
    RUNIT_PERF_REG_API(mbedtls_cipher_info_from_type                      , 0);
    RUNIT_PERF_REG_API(mbedtls_cipher_init                                , 0);
    RUNIT_PERF_REG_API(mbedtls_cipher_setup                               , 0);
    RUNIT_PERF_REG_API(mbedtls_ctr_drbg_free                              , 1);
    RUNIT_PERF_REG_API(mbedtls_ctr_drbg_init                              , 1);
    RUNIT_PERF_REG_API(mbedtls_ctr_drbg_random                            , 1);
    RUNIT_PERF_REG_API(mbedtls_ctr_drbg_random_with_add                   , 1);
    RUNIT_PERF_REG_API(mbedtls_ctr_drbg_reseed                            , 1);
    RUNIT_PERF_REG_API(mbedtls_ctr_drbg_seed                              , 1);
    RUNIT_PERF_REG_API(mbedtls_ctr_drbg_seed_entropy_len                  , 1);
    RUNIT_PERF_REG_API(mbedtls_ctr_drbg_set_entropy_len                   , 1);
    RUNIT_PERF_REG_API(mbedtls_ctr_drbg_set_prediction_resistance         , 1);
    RUNIT_PERF_REG_API(mbedtls_ctr_drbg_set_reseed_interval               , 1);
    RUNIT_PERF_REG_API(mbedtls_ctr_drbg_update                            , 1);
    RUNIT_PERF_REG_API(mbedtls_dhm_calc_secret                            , gCompConf.dhm);
    RUNIT_PERF_REG_API(mbedtls_dhm_free                                   , gCompConf.dhm);
    RUNIT_PERF_REG_API(mbedtls_dhm_init                                   , gCompConf.dhm);
    RUNIT_PERF_REG_API(mbedtls_dhm_make_params                            , gCompConf.dhm);
    RUNIT_PERF_REG_API(mbedtls_dhm_make_public                            , gCompConf.dhm);
    RUNIT_PERF_REG_API(mbedtls_dhm_read_params                            , gCompConf.dhm);
    RUNIT_PERF_REG_API(mbedtls_dhm_read_public                            , gCompConf.dhm);
    RUNIT_PERF_REG_API(mbedtls_ecdh_calc_secret                           , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdh_compute_shared                        , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdh_free                                  , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdh_gen_public                            , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdh_init                                  , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdh_make_params                           , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdh_make_public                           , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdh_read_params                           , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdh_read_public                           , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdh_read_params_edwards                   , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdh_make_params_edwards                   , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdsa_free                                 , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdsa_genkey                               , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdsa_genkey_edwards                       , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdsa_init                                 , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdsa_read_signature                       , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdsa_sign                                 , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdsa_sign_det                             , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdsa_sign_edwards                         , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdsa_verify                               , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdsa_verify_edwards                       , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecdsa_write_signature                      , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecies_kem_encrypt                          , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecies_kem_encrypt_full                     , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_ecies_kem_decrypt                          , gCompConf.ecc);
    RUNIT_PERF_REG_API(mbedtls_entropy_init                               , 0);
    RUNIT_PERF_REG_API(mbedtls_gcm_auth_decrypt                           , gCompConf.gcm);
    RUNIT_PERF_REG_API(mbedtls_gcm_crypt_and_tag                          , gCompConf.gcm);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_gcm_crypt_and_tag, "encrypt"       , gCompConf.gcm);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_gcm_crypt_and_tag, "decrypt"       , gCompConf.gcm);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_gcm_crypt_and_tag, "16B"           , gCompConf.gcm);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_gcm_crypt_and_tag, "128B"          , gCompConf.gcm);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_gcm_crypt_and_tag, "1024B"         , gCompConf.gcm);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_gcm_crypt_and_tag, "8192B"         , gCompConf.gcm);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_gcm_crypt_and_tag, "65534B"        , gCompConf.gcm);
    RUNIT_PERF_REG_API(mbedtls_gcm_free                                   , gCompConf.gcm);
    RUNIT_PERF_REG_API(mbedtls_gcm_init                                   , gCompConf.gcm);
    RUNIT_PERF_REG_API(mbedtls_gcm_setkey                                 , gCompConf.gcm);
    RUNIT_PERF_REG_API(mbedtls_hkdf                                       , 1);
    RUNIT_PERF_REG_API(mbedtls_hkdf_key_derivation                        , 1);
    RUNIT_PERF_REG_API(mbedtls_md                                         , 0);
    RUNIT_PERF_REG_API(mbedtls_md_hmac                                    , 0);
    RUNIT_PERF_REG_API(mbedtls_md_hmac_finish                             , 0);
    RUNIT_PERF_REG_API(mbedtls_md_hmac_starts                             , 0);
    RUNIT_PERF_REG_API(mbedtls_md_hmac_update                             , 0);
    RUNIT_PERF_REG_API(mbedtls_md_info_from_string                        , 0);
    RUNIT_PERF_REG_API(mbedtls_md_init                                    , 0);
    RUNIT_PERF_REG_API(mbedtls_md_setup                                   , 0);
    RUNIT_PERF_REG_API(mbedtls_poly                                       , 1);
    RUNIT_PERF_REG_API(mbedtls_poly1305_mac                               , 1);
    RUNIT_PERF_REG_API(mbedtls_rsa_check_privkey                          , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_check_privkey, "2048"          , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_check_privkey, "3072"          , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_check_privkey, "4096"          , gCompConf.rsa);
    RUNIT_PERF_REG_API(mbedtls_rsa_check_pubkey                           , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_check_pubkey, "2048"           , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_check_pubkey, "3072"           , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_check_pubkey, "4096"           , gCompConf.rsa);
    RUNIT_PERF_REG_API(mbedtls_rsa_check_pub_priv                         , gCompConf.rsa);
    RUNIT_PERF_REG_API(mbedtls_rsa_complete                               , gCompConf.rsa);
    RUNIT_PERF_REG_API(mbedtls_rsa_copy                                   , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_copy, "2048"                   , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_copy, "3072"                   , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_copy, "4096"                   , gCompConf.rsa);
    RUNIT_PERF_REG_API(mbedtls_rsa_free                                   , gCompConf.rsa);
    RUNIT_PERF_REG_API(mbedtls_rsa_gen_key                                , gCompConf.rsa);
    RUNIT_PERF_REG_API(mbedtls_rsa_init                                   , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_decrypt, "2048"          , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_decrypt, "3072"          , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_decrypt, "4096"          , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_encrypt, "2048"          , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_encrypt, "3072"          , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_encrypt, "4096"          , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_sign, "2048"             , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_sign, "3072"             , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_sign, "4096"             , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_verify, "2048"           , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_verify, "3072"           , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_pkcs1_verify, "4096"           , gCompConf.rsa);
    RUNIT_PERF_REG_API(mbedtls_rsa_private                                , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_private, "2048"                , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_private, "3072"                , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_private, "4096"                , gCompConf.rsa);
    RUNIT_PERF_REG_API(mbedtls_rsa_public                                 , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_public, "2048"                 , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_public, "3072"                 , gCompConf.rsa);
    RUNIT_PERF_REG_API_W_PARAM(mbedtls_rsa_public, "4096"                 , gCompConf.rsa);
    RUNIT_PERF_REG_API(mbedtls_sb_cert_chain_cerification_init            , 1);
    RUNIT_PERF_REG_API(mbedtls_sb_cert_verify_single                      , 1);
    RUNIT_PERF_REG_API(mbedtls_sb_sw_image_store_address_change           , 1);
    RUNIT_PERF_REG_API(mbedtls_sha1                                       , gCompConf.sha1);
    RUNIT_PERF_REG_API(mbedtls_sha1_clone                                 , gCompConf.sha1);
    RUNIT_PERF_REG_API(mbedtls_sha1_finish_ret                            , gCompConf.sha1);
    RUNIT_PERF_REG_API(mbedtls_sha1_free                                  , gCompConf.sha1);
    RUNIT_PERF_REG_API(mbedtls_sha1_init                                  , gCompConf.sha1);
    RUNIT_PERF_REG_API(mbedtls_sha1_starts_ret                            , gCompConf.sha1);
    RUNIT_PERF_REG_API(mbedtls_sha1_update_ret                            , gCompConf.sha1);
    RUNIT_PERF_REG_API(mbedtls_sha256                                     , gCompConf.sha256);
    RUNIT_PERF_REG_API(mbedtls_sha256_clone                               , gCompConf.sha256);
    RUNIT_PERF_REG_API(mbedtls_sha256_finish_ret                          , gCompConf.sha256);
    RUNIT_PERF_REG_API(mbedtls_sha256_free                                , gCompConf.sha256);
    RUNIT_PERF_REG_API(mbedtls_sha256_init                                , gCompConf.sha256);
    RUNIT_PERF_REG_API(mbedtls_sha256_starts_ret                          , gCompConf.sha256);
    RUNIT_PERF_REG_API(mbedtls_sha256_update_ret                          , gCompConf.sha256);
    RUNIT_PERF_REG_API(mbedtls_sha512                                     , gCompConf.sha512);
    RUNIT_PERF_REG_API(mbedtls_sha512_clone                               , gCompConf.sha512);
    RUNIT_PERF_REG_API(mbedtls_sha512_finish                              , gCompConf.sha512);
    RUNIT_PERF_REG_API(mbedtls_sha512_free                                , gCompConf.sha512);
    RUNIT_PERF_REG_API(mbedtls_sha512_init                                , gCompConf.sha512);
    RUNIT_PERF_REG_API(mbedtls_sha512_starts                              , gCompConf.sha512);
    RUNIT_PERF_REG_API(mbedtls_sha512_update                              , gCompConf.sha512);
    RUNIT_PERF_REG_API(mbedtls_srp_init                                   , 1);
    RUNIT_PERF_REG_API(mbedtls_srp_pwd_ver_create                         , 1);
    RUNIT_PERF_REG_API(mbedtls_srp_host_pub_key_create                    , 1);
    RUNIT_PERF_REG_API(mbedtls_srp_user_proof_calc                        , 1);
    RUNIT_PERF_REG_API(mbedtls_srp_user_pub_key_create                    , 1);
    RUNIT_PERF_REG_API(mbedtls_srp_host_proof_verify_and_calc             , 1);
    RUNIT_PERF_REG_API(mbedtls_srp_user_proof_verify                      , 1);
    RUNIT_PERF_REG_API(mbedtls_util_asset_pkg_unpack                      , 1);
    RUNIT_PERF_REG_API(mbedtls_util_key_derivation_cmac                   , 1);
    RUNIT_PERF_REG_API(mbedtls_util_key_derivation_hmac                   , 1);
}
/**
 * @brief               Executor function. Called from a side thread to perform the sequence of tests.
 * @note                This is a workaround the issue that CC API requires DMA-able stack.
 *                      When using Test_PalThreadCreate we are able to ensure that the stack is DMA-able.
 * @param params        Not used
 */
static void* runIt_executer(void *params)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    const char* TEST_NAME = "All Tests";
    uint32_t testIter = 0;

    CC_UNUSED_PARAM(params);

    RUNIT_PRINT("cc312 runtime integration test\n");
    RUNIT_PRINT("---------------------------------------------------------------\n");

    /* Initialise runtime integration perf engine */
    runIt_perfInit();

    runIt_printMbedtlsAltConfig();

    RUNIT_PRINT_HEADER();

    gRunItPrintEnable = 1;

    runIt_regApi();

    for (testIter = 1; testIter <= RUNIT_TEST_ITER_MAX; ++testIter)
    {

        RUNIT_TEST_START(TEST_NAME);

        /* Init libraries */
        RUNIT_ASSERT(runIt_init() == RUNIT_ERROR__OK);

#if !defined(RUNIT_PIE_ENABLED)
        rc += runIt_aesTest();
        rc += runIt_srpTest();
        rc += runIt_shaTest();
        rc += runIt_ccmTest();
        rc += runIt_gcmTest();
        rc += runIt_rsaTest();
        rc += runIt_ecdsaTest();
        rc += runIt_ecdhTest();
        rc += runIt_eciesTest();
        rc += runIt_ctrDrbgTest();
        rc += runIt_ChachaTest();
        rc += runIt_macTest();
        rc += runIt_dhmTest();
        rc += runIt_extDmaTest();
#endif /* RUNIT_PIE_ENABLED */

        rc += runIt_keyDerivationTest();
        rc += runIt_assetProvTest();
        rc += runIt_secureBootTest();

        g_runIt_executerRc = rc;

        RUNIT_TEST_RESULT(TEST_NAME);

        /* continue to next test only if all passed */
        RUNIT_ASSERT(rc == RUNIT_ERROR__OK);

        /* disable printing of next tests */
        gRunItPrintEnable = 0;

        /* indicate progress */
        RUNIT_PRINT("Test %u/%u completed\n", (unsigned int)testIter, RUNIT_TEST_ITER_MAX);

        runIt_finish();
    }

bail:
    runIt_perfDump();
    CC_PAL_PERF_DUMP();

    return NULL;
}

/************************************************************
 *
 * public functions
 *
 ************************************************************/
RunItError_t runIt_all(void)
{
    RunItError_t rc = RUNIT_ERROR__OK;
    ThreadHandle threadHandle = NULL;
    const char* TASK_NAME = "runIt_executer";
    uint32_t priotity = Test_PalGetDefaultPriority();

    /* init platform and map memory */
    RUNIT_ASSERT(Test_ProjInit() == 0);

    RUNIT_PRINT("FPGA version: 0x%08"PRIx32"\n", TEST_READ_TEE_ENV_REG(DX_ENV_VERSION_REG_OFFSET));

    gRunItStackSize = RUNIT_PTHREAD_STACK_MIN;

    /* Create a task that will allocate a DMA -able stack */
    threadHandle = Test_PalThreadCreate(RUNIT_PTHREAD_STACK_MIN,
                                        runIt_executer,
                                        priotity,
                                        NULL,
                                        (char*) TASK_NAME,
                                        sizeof(TASK_NAME),
                                        true);

    /* Verify task was created successfully */
    RUNIT_ASSERT(threadHandle != NULL);

    /* Wait for task to complete */
    Test_PalThreadJoin(threadHandle, NULL);

    /* Finalize task's resources */
    Test_PalThreadDestroy(threadHandle);

    /* Read result code */
    rc = g_runIt_executerRc;

bail:
    /* Free platform */
    Test_ProjFree();

    return rc;
}

#if defined(DX_PLAT_ZYNQ7000)
int main(int argc, char** argv)
{
    ((void)argc);
    ((void)argv);

    runIt_all();

    return 0;
}
#endif
