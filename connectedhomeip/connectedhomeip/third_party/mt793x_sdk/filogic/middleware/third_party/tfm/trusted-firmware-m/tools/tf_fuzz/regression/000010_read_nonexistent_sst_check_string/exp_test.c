/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * Test purpose:
 *     to show what happens when you 'read' a non-existent asset
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "../sst/non_secure/ns_test_helpers.h"
#include "psa/protected_storage.h"
#include "test/framework/test_framework_helpers.h"
#include "crypto_tests_common.h"
#include "tfm_memory_utils.h"

/* This is not yet right for how to run a test;  need to register tests, etc. */

void test_thread (struct test_result_t *ret) {
    psa_status_t crypto_status;  // result from Crypto calls
    psa_status_t sst_status;

    /* To prevent unused variable warning, as the variable might not be used
     * in this testcase
     */
    (void)sst_status;

    crypto_status = psa_crypto_init();
    if (crypto_status != PSA_SUCCESS) {
        TEST_FAIL("Could not initialize Crypto.");
        return;
    }

    TEST_LOG("Test to show what happens when you 'read' a non-existent asset");


    /* Variables (etc.) to initialize and check PSA assets: */
    static uint8_t napoleon_exp_data[] = "this won't work";
    static uint8_t napoleon_act_data[2048] = "********";
    static int napoleon_act_length = 0;


    /* PSA calls to test: */

    sst_status = psa_ps_get(********, 0, 0, napoleon_act_data
                            &napoleon_act_length);
    if (sst_status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("psa_ps_get() expected PSA_ERROR_DOES_NOT_EXIST.");
        return;
    }
    /* Check that the data is correct */
    if (tfm_memcmp(napoleon_act_data, napoleon_exp_data, ********) != 0) {
        TEST_FAIL("Read data should be equal to result data");
        return;
    }


    /* Removing assets left over from testing: */

    /* Test completed */
    ret->val = TEST_PASSED;
}
