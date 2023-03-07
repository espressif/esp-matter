/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_tests.h"

#include <stdio.h>

#include "secure_fw/services/secure_storage/nv_counters/sst_nv_counters.h"
#include "nv_counters/test_sst_nv_counters.h"
#include "psa/protected_storage.h"
#include "tfm_memory_utils.h"
#include "s_test_helpers.h"

/* This include is required to expose the sst_system_prepare function, via the
 * tfm_sst_test_system_prepare API, to simulate a reboot in the system.
 * sst_system_prepare is called when the SST service is initialized.
 */
#include "test/test_services/tfm_sst_test_service/tfm_sst_test_service_api.h"

#include "test/framework/test_framework_helpers.h"

/* Test UIDs */
#define TEST_UID 2UL  /* UID 1 cannot be used as it references a write once
                       * asset, created in psa_ps_s_interface_testsuite.c
                       */

/* Write data */
#define WRITE_DATA       "THE_FIVE_BOXING_WIZARDS_JUMP_QUICKLY"
#define WRITE_DATA_SIZE  (sizeof(WRITE_DATA) - 1)
#define READ_DATA        "############################################"
#define RESULT_DATA      ("####" WRITE_DATA "####")

/*
 * Summary of tests covered by the test suite.
 *
 * SST version | NVC1 | NVC2 | NVC3 |  Result  |  Test Num
 * ------------|------|------|------|----------|------------
 *      X      |   X  |   X  |   X  |  Valid   |     1
 *      N      |   X  |   X  |   X  |  Invalid |     2
 *      X      |   X  |   X  |   N  |  Valid   |     3
 *      N      |   X  |   N  |   N  |  Valid   |     4
 *      X      |   X  |   N  |   N  |  Valid   |     5
 *      X      |   X  |   M  |   N  |  Valid   |     6
 *      M      |   X  |   M  |   N  |  Invalid |     7
 *      N      |   X  |   M  |   N  |  Invalid |     8
 *
 * Test 9 checks the SST result when the non-volatile (NV) counter 1 cannot be
 * incremented (e.g it has reached its maximum value).
 */

/* List of tests */
static void tfm_sst_test_4001(struct test_result_t *ret);
static void tfm_sst_test_4002(struct test_result_t *ret);
static void tfm_sst_test_4003(struct test_result_t *ret);
static void tfm_sst_test_4004(struct test_result_t *ret);
static void tfm_sst_test_4005(struct test_result_t *ret);
static void tfm_sst_test_4006(struct test_result_t *ret);
static void tfm_sst_test_4007(struct test_result_t *ret);
static void tfm_sst_test_4008(struct test_result_t *ret);
static void tfm_sst_test_4009(struct test_result_t *ret);

static struct test_t interface_tests[] = {
    {&tfm_sst_test_4001, "TFM_SST_TEST_4001",
     "Check SST area version when NV counters 1/2/3 have the same value", {TEST_PASSED}},
    {&tfm_sst_test_4002, "TFM_SST_TEST_4002",
     "Check SST area version when it is different from NV counters 1/2/3", {TEST_PASSED}},
    {&tfm_sst_test_4003, "TFM_SST_TEST_4003",
     "Check SST area version when NV counters 1 and 2 are equals, 3 is "
     "different, and SST area version match NV counters 1 and 2", {TEST_PASSED}},
    {&tfm_sst_test_4004, "TFM_SST_TEST_4004",
     "Check SST area version when NV counters 2 and 3 are equals, 1 is "
     "different and SST area version match NV counter 2 and 3", {TEST_PASSED}},
    {&tfm_sst_test_4005, "TFM_SST_TEST_4005",
     "Check SST area version when NV counters 2 and 3 are equals, 1 is "
     "different and SST area version match NV counter 1", {TEST_PASSED}},
    {&tfm_sst_test_4006, "TFM_SST_TEST_4006",
     "Check SST area version when NV counters 1, 2 and 3 have different values "
     "and SST area version match NV counter 1 value", {TEST_PASSED}},
    {&tfm_sst_test_4007, "TFM_SST_TEST_4007",
     "Check SST area version when NV counters 1, 2 and 3 have different values "
     "and SST area version match NV counter 2 value", {TEST_PASSED}},
    {&tfm_sst_test_4008, "TFM_SST_TEST_4008",
     "Check SST area version when NV counters 1, 2 and 3 have different values "
     "and SST area version match NV counter 3 value", {TEST_PASSED}},
    {&tfm_sst_test_4009, "TFM_SST_TEST_4009",
     "Check SST area version when NV counter 1 cannot be incremented", {TEST_PASSED}},
};

void register_testsuite_s_rollback_protection(struct test_suite_t *p_test_suite)
{
    uint32_t list_size = (sizeof(interface_tests) / sizeof(interface_tests[0]));

    set_testsuite("SST rollback protection tests (TFM_SST_TEST_4XXX)",
                  interface_tests, list_size, p_test_suite);
}

/**
 * \brief Check SST area version when NV counters 1/2/3 have the same value.
 *        It also checks that the 3 NV counters are aligned and they have been
 *        increased by 1 unit.
 */
static void tfm_sst_test_4001(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    uint32_t old_nvc_1, nvc_1, nvc_2, nvc_3;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    /* Creates an asset in the SST area to generate a new SST area version */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    /* Reads NV counter 1 to get the saved value to compare it later */
    status = test_sst_read_nv_counter(TFM_SST_NV_COUNTER_1, &old_nvc_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Read should not fail");
        return;
    }

    /* Sets new data in the asset to generate a new SST area version */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    /* Validates the 3 NV counters have the same value and it has been increased
     * by 1 unit.
     */

    /* Reads NV counter 1 to get the current value */
    status = test_sst_read_nv_counter(TFM_SST_NV_COUNTER_1, &nvc_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Read should not fail");
        return;
    }

    /* Checks if NV counter 1 value has been increased by 1 unit as result of
     * process the write request.
     */
    if (nvc_1 != (old_nvc_1 + 1)) {
        TEST_FAIL("NV counter 1 has been increased more than 1 unit");
        return;
    }

    /* Reads NV counter 2 to get the current value */
    status = test_sst_read_nv_counter(TFM_SST_NV_COUNTER_2, &nvc_2);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Read should not fail");
        return;
    }

    if (nvc_1 != nvc_2) {
        TEST_FAIL("NV counter 1 and 2 should have the same value");
        return;
    }

    /* Reads NV counter 3 to get the current value */
    status = test_sst_read_nv_counter(TFM_SST_NV_COUNTER_3, &nvc_3);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Read should not fail");
        return;
    }

    if (nvc_2 != nvc_3) {
        TEST_FAIL("NV counter 2 and 3 should have the same value");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should not fail as the NV counters has the same values and
     * the SST area authentication is aligned with those values.
     */
    status = tfm_sst_test_system_prepare();
    if (status != PSA_SUCCESS) {
        TEST_FAIL("AM prepare should not fail");
        return;
    }

    /* Gets data from the asset */
    status = psa_ps_get(uid, offset, data_len, (read_data + HALF_PADDING_SIZE),
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

    /* Checks that the data has not changed */
    if (tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("The data should not have changed");
        return;
    }

    /* Removes the asset to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when it is different from NV counters
 *        1/2/3.
 */
static void tfm_sst_test_4002(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    /* Creates an asset in the SST area to generate a new SST area version */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    /* Increments all counters to make that SST area version old/invalid */
    status = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    status = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_2);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    status = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_3);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should fail as the SST area version does not match the
     * NV counters values.
     */
    status = tfm_sst_test_system_prepare();
    if (status != PSA_ERROR_GENERIC_ERROR) {
        TEST_FAIL("SST system prepare should fail as version is old");
        return;
    }

    /* Removes the asset to clean up storage for the next test.
     *
     * To be able to remove the asset, the SST area version should match
     * with the counter values. So, it is required to:
     *
     * 1. align the counters with the SST area version
     * 2. re-call sst_system_prepare to mark the SST area as a valid image
     * 3. remove the asset.
     */

    /* Aligns NV counters with the SST area version */
    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_2);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_3);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    /* Calls sst_system_prepare to mark the SST area as a valid image */
    status = tfm_sst_test_system_prepare();
    if (status != PSA_SUCCESS) {
        TEST_FAIL("SST system prepare should not fail");
        return;
    }

    /* Removes the asset to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 1 and 2 are equals, 3 is
 *        different and SST area version match NV counter 1 and 2 values.
 *        It simulates a power cut during write action while the counter 3 is
 *        being increased.
 */
static void tfm_sst_test_4003(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    /* Creates an asset in the SST area to generate a new SST area version */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    /* Decrements NV counters 3 to make it different from the other two counters
     * and make the current SST area version match NV counter 1 and 2 values.
     */
    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_3);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should not fail as the SST area version match NV counters 1 and
     * 2 values.
     */
    status = tfm_sst_test_system_prepare();
    if (status != PSA_SUCCESS) {
        TEST_FAIL("SST system prepare should not fail");
        return;
    }

    /* Gets the data from the asset */
    status = psa_ps_get(uid, offset, data_len, (read_data + HALF_PADDING_SIZE),
                                                &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

    /* Checks that the data has not changed */
    if (tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("The data should not have changed");
        return;
    }

    /* Removes the asset to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 2 and 3 are equals, 1 is
 *        different and SST area version match NV counter 2 and 3 values.
 *        It simulates a power cut during write action before increment counter
 *        2 and 3, and the new SST area version is corrupted and only the old
 *        version match the NV counters.
 */
static void tfm_sst_test_4004(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    /* Creates an asset in the SST area to generate a new SST area version */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    /* Increments NV counters 1 to make it different from the other two counters
     * and make the current SST area version match NV counter 2 and 3 values.
     */
    status = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should not fail as the SST area version match the NV counter 2
     * and 3 values.
     */
    status = tfm_sst_test_system_prepare();
    if (status != PSA_SUCCESS) {
        TEST_FAIL("SST system prepare should not fail");
        return;
    }

    /* Gets the data from the asset */
    status = psa_ps_get(uid, offset, data_len, (read_data + HALF_PADDING_SIZE),
                                                &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

    /* Checks that the data has not changed */
    if (tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("The data should not have changed");
        return;
    }

    /* Removes the asset to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 2 and 3 are equals, 1 is
 *        different and SST area version match NV counter 1 value.
 *        It simulates a power cut during write action before increment counter
 *        2 and 3, and the new SST area version is corrupted and only the old
 *        version match the NV counters.
 */
static void tfm_sst_test_4005(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    /* Creates an asset in the SST area to generate a new SST area version */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    /* Decrements NV counter 2 and 3 to make the SST area version match NV
     * counter 1 only.
     */
    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_2);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_3);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should not fail as the SST area version match the NV counter 1.
     */
    status = tfm_sst_test_system_prepare();
    if (status != PSA_SUCCESS) {
        TEST_FAIL("SST system prepare should not fail");
        return;
    }

    /* Gets the data from the asset */
    status = psa_ps_get(uid, offset, data_len, (read_data + HALF_PADDING_SIZE),
                                                &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

    /* Checks that the data has not changed */
    if (tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("The data should not have changed");
        return;
    }

    /* Removes the asset to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 1, 2 and 3 have different
 *        values and SST area version match NV counter 1 value.
 */
static void tfm_sst_test_4006(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    /* Creates an asset in the SST area to generate a new SST area version */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    /* Decrements NV counter 2 (1 time) and 3 (2 times) to make the SST area
     * version match NV counter 1 only.
     */
    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_2);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_3);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_3);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should not fail as the SST area version match the NV counter 1.
     */
    status = tfm_sst_test_system_prepare();
    if (status != PSA_SUCCESS) {
        TEST_FAIL("SST system prepare should not fail");
        return;
    }

    /* Gets data from the asset */
    status = psa_ps_get(uid, offset, data_len, (read_data + HALF_PADDING_SIZE),
                                                &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

    /* Checks that the data has not changed */
    if (tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("The data should not have changed");
        return;
    }

    /* Removes the asset to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 1, 2 and 3 have different
 *        values and SST area version match NV counter 2 value.
 */
static void tfm_sst_test_4007(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    /* Creates an asset in the SST area to generate a new SST area version */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    /* Increments NV counter 1 and decrements 3 to make the SST area
     * version match NV counter 2 only.
     */
    status = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_3);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should fail as the SST area version match the NV counter 2 and
     * the other counters are different.
     */
    status = tfm_sst_test_system_prepare();
    if (status != PSA_ERROR_GENERIC_ERROR) {
        TEST_FAIL("SST system prepare should fail");
        return;
    }

    /* Removes the asset to clean up storage for the next test.
     *
     * To be able to remove the asset, the SST area version should match
     * with the counter values. So, it is required to:
     *
     * 1. align the counters with the SST area version
     * 2. re-call sst_system_prepare to mark the SST area as a valid image
     * 3. remove the asset.
     */

    /* Aligns NV counters with the SST area version */
    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    status = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_3);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    /* Calls sst_system_prepare to mark the SST area as a valid image */
    status = tfm_sst_test_system_prepare();
    if (status != PSA_SUCCESS) {
        TEST_FAIL("SST system prepare should not fail");
        return;
    }

    /* Removes the asset to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 1, 2 and 3 have different
 *        values and SST area version match NV counter 3 value.
 */
static void tfm_sst_test_4008(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    /* Creates an asset in the SST area to generate a new SST area version */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    /* Increments NV counter 1 (2 times) and 2 (1 time) to make the SST area
     * version match NV counter 3 only.
     */
    status = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    status = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    status = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_2);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should fail as the SST area version match the NV counter 2 and
     * the other counters are different.
     */
    status = tfm_sst_test_system_prepare();
    if (status != PSA_ERROR_GENERIC_ERROR) {
        TEST_FAIL("AM prepare should fail");
        return;
    }

    /* Removes the asset to clean up storage for the next test.
     *
     * To be able to remove the asset, the SST area version should match
     * with the counter values. So, it is required to:
     *
     * 1. align the counters with the SST area version
     * 2. re-call sst_system_prepare to mark the SST area as a valid image
     * 3. remove the asset.
     */

    /* Align NV counters with the SST area version */
    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    status = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_2);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    /* Calls sst_system_prepare to mark the SST area as a valid image */
    status = tfm_sst_test_system_prepare();
    if (status != PSA_SUCCESS) {
        TEST_FAIL("SST system prepare should not fail");
        return;
    }

    /* Removes the asset to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counter 1 cannot be incremented
 *        (e.g it has reached its maximum value)
 */
static void tfm_sst_test_4009(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    /* Disables increment function to simulate that NV counter 1 has
     * reached its maximum value.
     */
    test_sst_disable_increment_nv_counter();

    /* Creates an asset in the SST area to generate a new SST area version */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_ERROR_GENERIC_ERROR) {
        TEST_FAIL("Set should fail as the non-volatile counters can not be"
                  " increased");
        return;
    }

    /* Enables counter again to not affect the next tests, if any */
    test_sst_enable_increment_nv_counter();

    ret->val = TEST_PASSED;
}
