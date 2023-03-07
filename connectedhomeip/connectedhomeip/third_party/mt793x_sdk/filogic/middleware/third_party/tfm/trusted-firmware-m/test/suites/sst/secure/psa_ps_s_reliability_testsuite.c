/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_tests.h"

#include <stdio.h>

#include "psa/protected_storage.h"
#include "tfm_memory_utils.h"
#include "s_test_helpers.h"
#include "test/framework/test_framework_helpers.h"

/* Test UIDs */
#define TEST_UID 2UL  /* UID 1 cannot be used as it references a write once
                       * asset, created in psa_ps_s_interface_testsuite.c
                       */

/* Test suite defines */
#define LOOP_ITERATIONS_001 15U
#define LOOP_ITERATIONS_002 15U

/* Write data */
#define WRITE_DATA       "THE_FIVE_BOXING_WIZARDS_JUMP_QUICKLY"
#define WRITE_DATA_SIZE  (sizeof(WRITE_DATA) - 1)
#define READ_DATA        "############################################"
#define RESULT_DATA      ("####" WRITE_DATA "####")

/* Define test suite for SST reliability tests */
/* List of tests */
static void tfm_sst_test_3001(struct test_result_t *ret);
static void tfm_sst_test_3002(struct test_result_t *ret);

static struct test_t reliability_tests[] = {
    {&tfm_sst_test_3001, "TFM_SST_TEST_3001",
     "repetitive sets and gets in/from an asset", {TEST_PASSED} },
    {&tfm_sst_test_3002, "TFM_SST_TEST_3002",
     "repetitive sets, gets and removes", {TEST_PASSED} },
};

void register_testsuite_s_psa_ps_reliability(struct test_suite_t *p_test_suite)
{
    uint32_t list_size = (sizeof(reliability_tests) /
                          sizeof(reliability_tests[0]));

    set_testsuite("SST reliability tests (TFM_SST_TEST_3XXX)",
                  reliability_tests, list_size, p_test_suite);
}

/**
 * \brief Tests repetitive sets and gets in/from an asset.
 */
static void tfm_sst_test_3001(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    uint32_t itr;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    for (itr = 0; itr < LOOP_ITERATIONS_001; itr++) {
        TEST_LOG("  > Iteration %d of %d\r", itr + 1, LOOP_ITERATIONS_001);

        /* Set a data in the asset */
        status = psa_ps_set(uid, data_len, write_data, flags);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Set should not fail with valid UID");
            return;
        }

        /* Get data from the asset */
        status = psa_ps_get(uid, offset, data_len, (read_data +
                                                    HALF_PADDING_SIZE),
                                                    &read_data_len);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Get should not fail");
            return;
        }

        /* Check that the data has not changed */
        if (tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
            TEST_FAIL("The data should not have changed");
            return;
        }

        /* Set the original data into read buffer */
        tfm_memcpy(read_data, READ_DATA, sizeof(read_data));
    }

    TEST_LOG("\n");

    /* Remove the asset to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests repetitive sets, gets and removes.
 */
static void tfm_sst_test_3002(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    uint32_t itr;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    for (itr = 0; itr < LOOP_ITERATIONS_002; itr++) {
        TEST_LOG("  > Iteration %d of %d\r", itr + 1, LOOP_ITERATIONS_002);

        /* Set a data in the asset */
        status = psa_ps_set(uid, data_len, write_data, flags);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Set should not fail with valid UID");
            return;
        }

        /* Get data from the asset */
        status = psa_ps_get(uid, offset, data_len, (read_data +
                                                    HALF_PADDING_SIZE),
                                                    &read_data_len);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Get should not fail");
            return;
        }

        /* Check that the data has not changed */
        if (tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
            TEST_FAIL("The data should not have changed");
            return;
        }

        /* Remove the asset from the secure storage */
        status = psa_ps_remove(uid);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Remove should not fail with valid UID");
            return;
        }

        /* Set the original data into read buffer */
        tfm_memcpy(read_data, READ_DATA, sizeof(read_data));
    }

    TEST_LOG("\n");

    ret->val = TEST_PASSED;
}
