/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "attestation_s_tests.h"
#include "psa/initial_attestation.h"
#include "secure_fw/services/initial_attestation/attestation.h"
#include "../attestation_tests_common.h"
#include "../attest_token_test_values.h"
#include "../attest_token_test.h"

static uint8_t token_buffer[TEST_TOKEN_SIZE];
static const uint8_t challenge_buffer[TEST_CHALLENGE_OBJ_SIZE] = {
                                      TOKEN_TEST_NONCE_BYTES};

/* Define test suite for attestation service tests */
/* List of tests */
#ifdef INCLUDE_TEST_CODE /* Remove them from release build */
static void tfm_attest_test_1001(struct test_result_t *ret);
static void tfm_attest_test_1002(struct test_result_t *ret);
static void tfm_attest_test_1003(struct test_result_t *ret);
#endif
static void tfm_attest_test_1004(struct test_result_t *ret);
static void tfm_attest_test_1005(struct test_result_t *ret);

static struct test_t attestation_interface_tests[] = {
#ifdef INCLUDE_TEST_CODE /* Remove them from release build */
    {&tfm_attest_test_1001, "TFM_ATTEST_TEST_1001",
     "Minimal token test of attest token", {TEST_PASSED} },
    {&tfm_attest_test_1002, "TFM_ATTEST_TEST_1002",
     "Minimal token size test of attest token", {TEST_PASSED} },
    {&tfm_attest_test_1003, "TFM_ATTEST_TEST_1003",
     "Short circuit signature test of attest token", {TEST_PASSED} },
#endif
    {&tfm_attest_test_1004, "TFM_ATTEST_TEST_1004",
     "ECDSA signature test of attest token", {TEST_PASSED} },
    {&tfm_attest_test_1005, "TFM_ATTEST_TEST_1005",
     "Negative test cases for initial attestation service", {TEST_PASSED} },
};

void
register_testsuite_s_attestation_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(attestation_interface_tests) /
                 sizeof(attestation_interface_tests[0]));

    set_testsuite("Initial Attestation Service secure interface tests"
                  "(TFM_ATTEST_TEST_1XXX)",
                  attestation_interface_tests, list_size, p_test_suite);
}

#ifdef INCLUDE_TEST_CODE /* Remove them from release build */
/*!
 * \brief Get minimal token, only include a hard coded challenge, but omit the
 *        rest of the claims
 *
 * Calling the minimal_test, which just retrieves a specific token:
 *  - only hard coded challenge is included
 *  - token signature is the hash of the token concatenated twice
 */
static void tfm_attest_test_1001(struct test_result_t *ret)
{
    int32_t err;

    err = minimal_test();
    if (err != 0) {
        TEST_LOG("minimal_test() returned: %d\r\n", err);
        TEST_FAIL("Attest token minimal_test() has failed");
        return;
    }

    ret->val = TEST_PASSED;
}

/*!
 * \brief Get the size of the minimal token, only include a hard coded
 *        challenge, but omit the rest of the claims
 */
static void tfm_attest_test_1002(struct test_result_t *ret)
{
    int32_t err;

    err = minimal_get_size_test();
    if (err != 0) {
        TEST_LOG("minimal_get_size_test() returned: %d\r\n", err);
        TEST_FAIL("Attest token minimal_get_size_test() has failed");
        return;
    }

    ret->val = TEST_PASSED;
}

/*!
 * \brief Get an IAT with short circuit signature (signature is composed of
 *        hash of token). Parse the token, validate presence of claims and
 *        compare them against expected values in token_test_values.h
 *
 * More info in token_test.h
 */
static void tfm_attest_test_1003(struct test_result_t *ret)
{
    int32_t err;

    err = decode_test_short_circuit_sig();
    if (err != 0) {
        TEST_LOG("decode_test_short_circuit_sig() returned: %d\r\n", err);
        TEST_FAIL("Attest token decode_test_short_circuit_sig() has failed");
        return;
    }

    ret->val = TEST_PASSED;
}
#endif /* INCLUDE_TEST_CODE */

/*!
 * \brief Get an IAT with proper ECDSA signature. Parse the token, validate
 *        presence of claims and compare them against expected values in
 *        token_test_values.h
 *
 * ECDSA signing is currently not supported in TF_M.
 *
 * More info in token_test.h
 */
static void tfm_attest_test_1004(struct test_result_t *ret)
{
    int32_t err;

    err = decode_test_normal_sig();
    if (err != 0) {
        TEST_LOG("decode_test_normal_sig() returned: %d\r\n", err);
        TEST_FAIL("Attest token decode_test_normal_sig() has failed");
        return;
    }

    ret->val = TEST_PASSED;
}

/*!
 * \brief Negative tests for initial attestation service
 *
 *    - Calling initial attestation service with bigger challenge object than
 *      allowed.
 *    - Calling initial attestation service with smaller buffer size than the
 *      expected size of the token.
 */
static void tfm_attest_test_1005(struct test_result_t *ret)
{
    psa_status_t err;
    size_t token_buf_size = TEST_TOKEN_SIZE;
    size_t token_size;

    /* Call with with bigger challenge object than allowed */
    err = psa_initial_attest_get_token(challenge_buffer,
                                       INVALID_CHALLENGE_OBJECT_SIZE,
                                       token_buffer,
                                       token_buf_size,
                                       &token_size);

    if (err != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Attestation should fail with too big challenge object");
        return;
    }

    /* Call with smaller buffer size than size of test token */
    token_buf_size = TOO_SMALL_TOKEN_BUFFER;
    err = psa_initial_attest_get_token(challenge_buffer,
                                       TEST_CHALLENGE_OBJ_SIZE,
                                       token_buffer,
                                       token_buf_size,
                                       &token_size);

    if (err != PSA_ERROR_BUFFER_TOO_SMALL) {
        TEST_FAIL("Attestation should fail with too small token buffer");
        return;
    }

    ret->val = TEST_PASSED;
}
