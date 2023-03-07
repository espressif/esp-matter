/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * Test purpose:
 *     to show a more-interesting removal case
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
/* For now, just a single test_result_t struct is sufficient.*/
static struct test_result_t ret;

/* This is not yet right for how to run a test;  need to register tests, etc. */

void test_thread (struct test_result_t *ret) {
    psa_status_t crypto_status;  // result from Crypto calls
    psa_ps_status_t sst_status;

    crypto_status = psa_crypto_init();
    if (crypto_status != PSA_SUCCESS) {
        TEST_FAIL("Could not initialize Crypto.");
        return;
    }

    TEST_LOG("Test to show a more-interesting removal case");


    /* Variables (etc.) to initialize and check PSA assets: */
    static uint8_t president_data[] = "no new taxes";
    int president_data_size = 12;
    static uint8_t george_data[] = "no new taxes";
    int george_data_size = 12;
    static uint8_t herbert_data[] = "no new taxes";
    int herbert_data_size = 12;
    static uint8_t walker_data[] = "no new taxes";
    int walker_data_size = 12;
    static uint8_t bush_data[] = "no new taxes";
    int bush_data_size = 12;


    /* PSA calls to test: */

    /* Creating SST asset "president," with data "no new tax...". */
    sst_status = psa_ps_set(4713, president_data_size, president_data,
                            PSA_PS_FLAG_WRITE_ONCE);
    if (sst_status != PSA_PS_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_PS_SUCCESS, got #%d, (int) sst_status);
        return;
    }

    /* Creating SST asset "george," with data "no new tax...". */
    sst_status = psa_ps_set(5517, george_data_size, george_data, PSA_PS_FLAG_WRITE_ONCE);
    if (sst_status != PSA_PS_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_PS_SUCCESS, got #%d, (int) sst_status);
        return;
    }

    /* Creating SST asset "herbert," with data "no new tax...". */
    sst_status = psa_ps_set(4661, herbert_data_size, herbert_data, PSA_PS_FLAG_WRITE_ONCE);
    if (sst_status != PSA_PS_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_PS_SUCCESS, got #%d, (int) sst_status);
        return;
    }

    /* Creating SST asset "walker," with data "no new tax...". */
    sst_status = psa_ps_set(3441, walker_data_size, walker_data, PSA_PS_FLAG_WRITE_ONCE);
    if (sst_status != PSA_PS_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_PS_SUCCESS, got #%d, (int) sst_status);
        return;
    }

    /* Creating SST asset "bush," with data "no new tax...". */
    sst_status = psa_ps_set(5446, bush_data_size, bush_data, PSA_PS_FLAG_WRITE_ONCE);
    if (sst_status != PSA_PS_SUCCESS) {
        TEST_FAIL("psa_ps_set() expected PSA_PS_SUCCESS, got #%d, (int) sst_status);
        return;
    }

    sst_status = psa_ps_remove(5517);
    if (sst_status != PSA_PS_SUCCESS) {
        TEST_FAIL("psa_ps_remove() expected PSA_PS_SUCCESS, got #%d", (int) sst_status);
    }

    sst_status = psa_ps_remove(4713);
    if (sst_status != PSA_PS_SUCCESS) {
        TEST_FAIL("psa_ps_remove() expected PSA_PS_SUCCESS, got #%d", (int) sst_status);
    }

    sst_status = psa_ps_remove(3441);
    if (sst_status != PSA_PS_SUCCESS) {
        TEST_FAIL("psa_ps_remove() expected PSA_PS_SUCCESS, got #%d", (int) sst_status);
    }

    sst_status = psa_ps_remove(3441);
    if (sst_status != PSA_PS_ERROR_UID_NOT_FOUND) {
        TEST_FAIL("psa_ps_remove() expected PSA_PS_ERROR_UID_NOT_FOUND, got #%d", (int) sst_status);
    }


    /* Removing assets left over from testing: */
    psa_ps_remove(4661);
    if (sst_status != PSA_PS_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion!");
        return;
    }
    psa_ps_remove(5446);
    if (sst_status != PSA_PS_SUCCESS) {
        TEST_FAIL("Failed to tear down an SST asset upon test completion!");
        return;
    }

    /* Test completed */
    ret->val = TEST_PASSED;
}
