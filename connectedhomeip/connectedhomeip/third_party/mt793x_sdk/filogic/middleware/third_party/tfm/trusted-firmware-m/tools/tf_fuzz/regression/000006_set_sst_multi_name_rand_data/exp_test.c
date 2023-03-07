/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * Test purpose:
 *     to show a nice party trick
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

    TEST_LOG("Test to show a nice party trick");


    /* Variables (etc.) to initialize and check PSA assets: */
    static uint8_t john_data[] = "@@012@10@@********";
    static int john_data_size = \d+;
    static uint8_t paul_data[] = "@@013@10@@********";
    static int paul_data_size = \d+;
    static uint8_t george_data[] = "@@014@10@@********";
    static int george_data_size = \d+;
    static uint8_t and_data[] = "@@015@10@@********";
    static int and_data_size = \d+;
    static uint8_t ringo_data[] = "@@016@10@@********";
    static int ringo_data_size = \d+;


    /* PSA calls to test: */

    /* Creating SST asset "john," with data "@@012@10@@...". */
    sst_status = psa_ps_set(@@@001@@@, john_data_size, john_data,
                            PSA_STORAGE_FLAG_********);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    /* Creating SST asset "paul," with data "@@013@10@@...". */
    sst_status = psa_ps_set(@@@002@@@, paul_data_size, paul_data,
                            PSA_STORAGE_FLAG_********);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    /* Creating SST asset "george," with data "@@014@10@@...". */
    sst_status = psa_ps_set(@@@003@@@, george_data_size, george_data,
                            PSA_STORAGE_FLAG_********);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    /* Creating SST asset "and," with data "@@015@10@@...". */
    sst_status = psa_ps_set(@@@004@@@, and_data_size, and_data,
                            PSA_STORAGE_FLAG_********);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }

    /* Creating SST asset "ringo," with data "@@016@10@@...". */
    sst_status = psa_ps_set(@@@005@@@, ringo_data_size, ringo_data,
                            PSA_STORAGE_FLAG_********);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_SUCCESS.");
        return;
    }


    /* Removing assets left over from testing: */
    psa_ps_remove(@@@001@@@);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove(@@@002@@@);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove(@@@003@@@);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove(@@@004@@@);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }
    psa_ps_remove(@@@005@@@);
    if (sst_status != PSA_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion.");
        return;
    }

    /* Test completed */
    ret->val = TEST_PASSED;
}
