/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "its_s_tests.h"
#include "psa/internal_trusted_storage.h"
#include "test/framework/test_framework_helpers.h"
#include "test/test_services/tfm_secure_client_2/tfm_secure_client_2_api.h"
#include "../its_tests_common.h"
#include "tfm_memory_utils.h"

/* UID to test partition access control */
#define TEST_UID_ACCESS_CONTROL 42U

/* List of tests */
static void tfm_its_test_2020(struct test_result_t *ret);
static void tfm_its_test_2021(struct test_result_t *ret);
static void tfm_its_test_2022(struct test_result_t *ret);
static void tfm_its_test_2023(struct test_result_t *ret);

static struct test_t psa_its_s_tests[] = {
    {&tfm_its_test_common_001, "TFM_ITS_TEST_2001",
     "Set interface"},
    {&tfm_its_test_common_002, "TFM_ITS_TEST_2002",
     "Set interface with create flags"},
    {&tfm_its_test_common_003, "TFM_ITS_TEST_2003",
     "Set interface with NULL data pointer"},
    {&tfm_its_test_common_004, "TFM_ITS_TEST_2004",
     "Set interface with write once UID"},
    {&tfm_its_test_common_005, "TFM_ITS_TEST_2005",
     "Get interface with valid data"},
    {&tfm_its_test_common_006, "TFM_ITS_TEST_2006",
     "Get interface with zero data length"},
    {&tfm_its_test_common_007, "TFM_ITS_TEST_2007",
     "Get interface with invalid UIDs"},
    {&tfm_its_test_common_008, "TFM_ITS_TEST_2008",
     "Get interface with data lengths and offsets greater than UID length"},
    {&tfm_its_test_common_009, "TFM_ITS_TEST_2009",
     "Get interface with NULL data pointer"},
    {&tfm_its_test_common_010, "TFM_ITS_TEST_2010",
     "Get info interface with write once UID"},
    {&tfm_its_test_common_011, "TFM_ITS_TEST_2011",
     "Get info interface with valid UID"},
    {&tfm_its_test_common_012, "TFM_ITS_TEST_2012",
     "Get info interface with invalid UIDs"},
    {&tfm_its_test_common_013, "TFM_ITS_TEST_2013",
     "Remove interface with valid UID"},
    {&tfm_its_test_common_014, "TFM_ITS_TEST_2014",
     "Remove interface with write once UID"},
    {&tfm_its_test_common_015, "TFM_ITS_TEST_2015",
     "Remove interface with invalid UID"},
    {&tfm_its_test_common_016, "TFM_ITS_TEST_2016",
     "Block compaction after remove"},
    {&tfm_its_test_common_017, "TFM_ITS_TEST_2017",
     "Multiple partial gets"},
    {&tfm_its_test_common_018, "TFM_ITS_TEST_2018",
     "Multiple sets to same UID from same thread"},
    {&tfm_its_test_common_019, "TFM_ITS_TEST_2019",
     "Set, get and remove interface with different asset sizes"},
    {&tfm_its_test_2020, "TFM_ITS_TEST_2020",
     "Set interface with invalid data length"},
    {&tfm_its_test_2021, "TFM_ITS_TEST_2021",
     "Get interface with invalid data lengths and offsets"},
    {&tfm_its_test_2022, "TFM_ITS_TEST_2022",
     "Get info interface with NULL info pointer"},
    {&tfm_its_test_2023, "TFM_ITS_TEST_2023",
     "Attempt to get a UID set by a different partition"},
};

void register_testsuite_s_psa_its_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(psa_its_s_tests) / sizeof(psa_its_s_tests[0]));

    set_testsuite("PSA internal trusted storage S interface tests "
                  "(TFM_ITS_TEST_2XXX)",
                  psa_its_s_tests, list_size, p_test_suite);

#ifdef ITS_SHOW_FLASH_WARNING
    TEST_LOG("\r\n**WARNING** The ITS regression tests reduce the life of the "
             "flash memory as they write/erase multiple times the memory. \r\n"
             "Please, set the ITS_RAM_FS flag to use RAM instead of flash."
             "\r\n\r\n");
#endif
}

/**
 * \brief Tests set function with:
 *        - Data length longer than maximum permitted
 *
 * \param[out] ret  Test result
 */
static void tfm_its_test_2020(struct test_result_t *ret)
{
#ifndef TFM_PSA_API
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t data_len = INVALID_DATA_LEN;
    const uint8_t write_data[] = WRITE_DATA;

    /* A parameter with a buffer pointer where its data length is longer than
     * maximum permitted, it is treated as a secure violation.
     * TF-M framework rejects the request with a proper error code.
     * The ITS secure PSA implementation returns
     * PSA_ERROR_INVALID_ARGUMENT in that case.
     */

    /* Set with data length longer than the maximum supported */
    status = psa_its_set(uid, data_len, write_data, flags);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Set should not succeed with invalid data length");
        return;
    }

#endif
    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get function with:
 *        - Invalid data len and offset
 *        - NULL read data length pointer
 *
 * \param[out] ret  Test result
 */
static void tfm_its_test_2021(struct test_result_t *ret)
{
#ifndef TFM_PSA_API
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t write_len = WRITE_DATA_SIZE;
    size_t read_len;
    size_t offset;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_length = 0;

    status = psa_its_set(uid, write_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get with data length and offset set to invalid values */
    read_len = INVALID_DATA_LEN;
    offset = INVALID_OFFSET;

    /* A parameter with a buffer pointer where its data length is longer than
     * maximum permitted, it is treated as a secure violation.
     * TF-M framework rejects the request with a proper error code.
     * The ITS secure PSA implementation returns
     * PSA_ERROR_INVALID_ARGUMENT in that case.
     */

    status = psa_its_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                         &read_data_length);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Get should not succeed with invalid arguments");
        return;
    }

    /* Check that the read data is unchanged */
    if (tfm_memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }

    read_len = 1;
    offset = 0;

    status = psa_its_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                         NULL);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Get should not succeed with invalid arguments");
        return;
    }

    /* Check that the read data is unchanged */
    if (tfm_memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }

    /* Call remove to clean up storage for the next test */
    status = psa_its_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

#endif
    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get info function with:
 *        - NULL info pointer
 *
 * \param[out] ret  Test result
 */
static void tfm_its_test_2022(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    status = psa_its_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* A parameter with a null pointer is treated as a secure violation.
     * TF-M framework rejects the request with a proper error code.
     * The secure PSA ITS implementation returns
     * PSA_ERROR_INVALID_ARGUMENT in that case.
     */

    /* Get info with NULL info pointer */
#ifndef TFM_PSA_API
    status = psa_its_get_info(uid, NULL);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Get info should not succeed with NULL info pointer");
        return;
    }
#endif

    /* Call remove to clean up storage for the next test */
    status = psa_its_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Attempt to get a UID set by a different partition.
 *
 * \param[out] ret  Test result
 */
static void tfm_its_test_2023(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_ACCESS_CONTROL;

    /* Set the UID from this partition's context */
    status = psa_its_set(uid, WRITE_DATA_SIZE, WRITE_DATA,
                         PSA_STORAGE_FLAG_NONE);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Attempt to get the UID from the Secure Client 2 partition */
    status = tfm_secure_client_2_call_test(
                                         TFM_SECURE_CLIENT_2_ID_ITS_ACCESS_CTRL,
                                         &uid, sizeof(uid));
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get should not succeed from a different partition");
        return;
    }

    /* Call remove to clean up storage for the next test */
    status = psa_its_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}
