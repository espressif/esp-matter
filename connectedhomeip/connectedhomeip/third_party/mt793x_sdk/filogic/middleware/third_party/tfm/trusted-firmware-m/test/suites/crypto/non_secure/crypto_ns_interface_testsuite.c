/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test/framework/test_framework_helpers.h"
#include "tfm_api.h"
#include "../crypto_tests_common.h"

/* List of tests */
static void tfm_crypto_test_6001(struct test_result_t *ret);
static void tfm_crypto_test_6002(struct test_result_t *ret);
static void tfm_crypto_test_6003(struct test_result_t *ret);
static void tfm_crypto_test_6005(struct test_result_t *ret);
static void tfm_crypto_test_6007(struct test_result_t *ret);
static void tfm_crypto_test_6008(struct test_result_t *ret);
static void tfm_crypto_test_6009(struct test_result_t *ret);
static void tfm_crypto_test_6010(struct test_result_t *ret);
static void tfm_crypto_test_6011(struct test_result_t *ret);
static void tfm_crypto_test_6012(struct test_result_t *ret);
static void tfm_crypto_test_6013(struct test_result_t *ret);
static void tfm_crypto_test_6014(struct test_result_t *ret);
static void tfm_crypto_test_6019(struct test_result_t *ret);
static void tfm_crypto_test_6020(struct test_result_t *ret);
static void tfm_crypto_test_6021(struct test_result_t *ret);
static void tfm_crypto_test_6022(struct test_result_t *ret);
static void tfm_crypto_test_6024(struct test_result_t *ret);
static void tfm_crypto_test_6030(struct test_result_t *ret);
static void tfm_crypto_test_6031(struct test_result_t *ret);
static void tfm_crypto_test_6032(struct test_result_t *ret);
static void tfm_crypto_test_6033(struct test_result_t *ret);
static void tfm_crypto_test_6034(struct test_result_t *ret);

static struct test_t crypto_tests[] = {
    {&tfm_crypto_test_6001, "TFM_CRYPTO_TEST_6001",
     "Non Secure Key management interface", {TEST_PASSED} },
    {&tfm_crypto_test_6002, "TFM_CRYPTO_TEST_6002",
     "Non Secure Symmetric encryption (AES-128-CBC) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6003, "TFM_CRYPTO_TEST_6003",
     "Non Secure Symmetric encryption (AES-128-CFB) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6005, "TFM_CRYPTO_TEST_6005",
     "Non Secure Symmetric encryption (AES-128-CTR) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6007, "TFM_CRYPTO_TEST_6007",
     "Non Secure Symmetric encryption invalid cipher (AES-128-GCM)", {TEST_PASSED} },
    {&tfm_crypto_test_6008, "TFM_CRYPTO_TEST_6008",
     "Non Secure Symmetric encryption invalid cipher (AES-152-CBC)", {TEST_PASSED} },
    {&tfm_crypto_test_6009, "TFM_CRYPTO_TEST_6009",
     "Non Secure Symmetric encryption invalid cipher (HMAC-128-CFB)", {TEST_PASSED} },
    {&tfm_crypto_test_6010, "TFM_CRYPTO_TEST_6010",
     "Non Secure Unsupported Hash (SHA-1) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6011, "TFM_CRYPTO_TEST_6011",
     "Non Secure Hash (SHA-224) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6012, "TFM_CRYPTO_TEST_6012",
     "Non Secure Hash (SHA-256) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6013, "TFM_CRYPTO_TEST_6013",
     "Non Secure Hash (SHA-384) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6014, "TFM_CRYPTO_TEST_6014",
     "Non Secure Hash (SHA-512) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6019, "TFM_CRYPTO_TEST_6019",
     "Non Secure Unsupported HMAC (SHA-1) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6020, "TFM_CRYPTO_TEST_6020",
     "Non Secure HMAC (SHA-256) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6021, "TFM_CRYPTO_TEST_6021",
     "Non Secure HMAC (SHA-384) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6022, "TFM_CRYPTO_TEST_6022",
     "Non Secure HMAC (SHA-512) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6024, "TFM_CRYPTO_TEST_6024",
     "Non Secure HMAC with long key (SHA-224) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6030, "TFM_CRYPTO_TEST_6030",
     "Non Secure AEAD (AES-128-CCM) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6031, "TFM_CRYPTO_TEST_6031",
     "Non Secure AEAD (AES-128-GCM) interface", {TEST_PASSED} },
    {&tfm_crypto_test_6032, "TFM_CRYPTO_TEST_6032",
     "Non Secure key policy interface", {TEST_PASSED} },
    {&tfm_crypto_test_6033, "TFM_CRYPTO_TEST_6033",
     "Non Secure key policy check permissions", {TEST_PASSED} },
    {&tfm_crypto_test_6034, "TFM_CRYPTO_TEST_6034",
     "Non Secure persistent key interface", {TEST_PASSED} },
};

void register_testsuite_ns_crypto_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size = (sizeof(crypto_tests) / sizeof(crypto_tests[0]));

    set_testsuite("Crypto non-secure interface test (TFM_CRYPTO_TEST_6XXX)",
                  crypto_tests, list_size, p_test_suite);
}

/**
 * \brief Non-Secure interface test for Crypto
 *
 * \details The scope of this set of tests is to functionally verify
 *          the interfaces specified by psa/crypto.h are working
 *          as expected. This is not meant to cover all possible
 *          scenarios and corner cases.
 *
 */
static void tfm_crypto_test_6001(struct test_result_t *ret)
{
    psa_key_interface_test(PSA_KEY_TYPE_AES, ret);
}

static void tfm_crypto_test_6002(struct test_result_t *ret)
{
    psa_cipher_test(PSA_KEY_TYPE_AES, PSA_ALG_CBC_NO_PADDING, ret);
}

static void tfm_crypto_test_6003(struct test_result_t *ret)
{
    psa_cipher_test(PSA_KEY_TYPE_AES, PSA_ALG_CFB, ret);
}

static void tfm_crypto_test_6005(struct test_result_t *ret)
{
    psa_cipher_test(PSA_KEY_TYPE_AES, PSA_ALG_CTR, ret);
}

static void tfm_crypto_test_6007(struct test_result_t *ret)
{
    /* GCM is an AEAD mode */
    psa_invalid_cipher_test(PSA_KEY_TYPE_AES, PSA_ALG_GCM, 16, ret);
}

static void tfm_crypto_test_6008(struct test_result_t *ret)
{
    psa_invalid_key_length_test(ret);
}

static void tfm_crypto_test_6009(struct test_result_t *ret)
{
    /* HMAC is not a block cipher */
    psa_invalid_cipher_test(PSA_KEY_TYPE_HMAC, PSA_ALG_CFB, 16, ret);
}

static void tfm_crypto_test_6010(struct test_result_t *ret)
{
    psa_unsupported_hash_test(PSA_ALG_SHA_1, ret);
}

static void tfm_crypto_test_6011(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_224, ret);
}

static void tfm_crypto_test_6012(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_256, ret);
}

static void tfm_crypto_test_6013(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_384, ret);
}

static void tfm_crypto_test_6014(struct test_result_t *ret)
{
    psa_hash_test(PSA_ALG_SHA_512, ret);
}

static void tfm_crypto_test_6019(struct test_result_t *ret)
{
    psa_unsupported_mac_test(PSA_KEY_TYPE_HMAC, PSA_ALG_HMAC(PSA_ALG_SHA_1),
                             ret);
}

static void tfm_crypto_test_6020(struct test_result_t *ret)
{
    psa_mac_test(PSA_ALG_HMAC(PSA_ALG_SHA_256), 0, ret);
}

static void tfm_crypto_test_6021(struct test_result_t *ret)
{
    psa_mac_test(PSA_ALG_HMAC(PSA_ALG_SHA_384), 0, ret);
}

static void tfm_crypto_test_6022(struct test_result_t *ret)
{
    psa_mac_test(PSA_ALG_HMAC(PSA_ALG_SHA_512), 0, ret);
}
static void tfm_crypto_test_6024(struct test_result_t *ret)
{
    psa_mac_test(PSA_ALG_HMAC(PSA_ALG_SHA_224), 1, ret);
}

static void tfm_crypto_test_6030(struct test_result_t *ret)
{
    psa_aead_test(PSA_KEY_TYPE_AES, PSA_ALG_CCM, ret);
}

static void tfm_crypto_test_6031(struct test_result_t *ret)
{
    psa_aead_test(PSA_KEY_TYPE_AES, PSA_ALG_GCM, ret);
}

static void tfm_crypto_test_6032(struct test_result_t *ret)
{
    psa_policy_key_interface_test(ret);
}

static void tfm_crypto_test_6033(struct test_result_t *ret)
{
    psa_policy_invalid_policy_usage_test(ret);
}

static void tfm_crypto_test_6034(struct test_result_t *ret)
{
    psa_persistent_key_test(1, ret);
}
