/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_tests.h"

#include <stdio.h>

#include "s_test_helpers.h"
#include "tfm_memory_utils.h"
#include "psa/protected_storage.h"
#include "test/framework/test_framework_helpers.h"
#include "flash_layout.h"
#ifdef TARGET_PLATFORM_MT793X
#include "mt7933_layout.h"
#endif


/* Test UIDs */
#define WRITE_ONCE_UID  1U /* Cannot be modified or deleted once created */
#define TEST_UID_1      2U
#define TEST_UID_2      3U
#define TEST_UID_3      4U

/* Invalid values */
#define INVALID_UID              0U
#define INVALID_DATA_LEN         UINT32_MAX
#define INVALID_OFFSET           UINT32_MAX
#define INVALID_FLAG             (1U << 31)
#define INVALID_THREAD_NAME      "Thread_INVALID"

/* Write once data */
#define WRITE_ONCE_DATA          "THE_FIVE_BOXING_WIZARDS_JUMP_QUICKLY"
#define WRITE_ONCE_DATA_SIZE     (sizeof(WRITE_ONCE_DATA) - 1)
#define WRITE_ONCE_READ_DATA     "############################################"
#define WRITE_ONCE_RESULT_DATA   ("####" WRITE_ONCE_DATA "####")

#define WRITE_DATA               "THEQUICKBROWNFOXJUMPSOVERALAZYDOG"
#define WRITE_DATA_SIZE          (sizeof(WRITE_DATA) - 1)
#define READ_DATA                "_________________________________________"
#define RESULT_DATA              ("____" WRITE_DATA "____")
#define OFFSET_READ_DATA         "HEQUICKBROWNFOXJUMPSOVERALAZYDOG"
#define OFFSET_RESULT_DATA       ("____" OFFSET_READ_DATA "_____")

#define TEST_1022_CYCLES         3U

static const uint8_t write_asset_data[SST_MAX_ASSET_SIZE] = {0xBF};
static uint8_t read_asset_data[SST_MAX_ASSET_SIZE] = {0};
static size_t read_asset_datal_len = 0;

/* List of tests */
static void tfm_sst_test_2001(struct test_result_t *ret);
static void tfm_sst_test_2002(struct test_result_t *ret);
static void tfm_sst_test_2003(struct test_result_t *ret);
static void tfm_sst_test_2004(struct test_result_t *ret);
static void tfm_sst_test_2005(struct test_result_t *ret);
static void tfm_sst_test_2006(struct test_result_t *ret);
static void tfm_sst_test_2007(struct test_result_t *ret);
static void tfm_sst_test_2008(struct test_result_t *ret);
static void tfm_sst_test_2009(struct test_result_t *ret);
static void tfm_sst_test_2010(struct test_result_t *ret);
static void tfm_sst_test_2011(struct test_result_t *ret);
static void tfm_sst_test_2012(struct test_result_t *ret);
static void tfm_sst_test_2013(struct test_result_t *ret);
static void tfm_sst_test_2014(struct test_result_t *ret);
static void tfm_sst_test_2015(struct test_result_t *ret);
static void tfm_sst_test_2016(struct test_result_t *ret);
static void tfm_sst_test_2017(struct test_result_t *ret);
static void tfm_sst_test_2018(struct test_result_t *ret);
static void tfm_sst_test_2019(struct test_result_t *ret);
static void tfm_sst_test_2020(struct test_result_t *ret);
static void tfm_sst_test_2021(struct test_result_t *ret);
static void tfm_sst_test_2022(struct test_result_t *ret);

static struct test_t psa_ps_s_tests[] = {
    {&tfm_sst_test_2001, "TFM_SST_TEST_2001",
     "Set interface"},
    {&tfm_sst_test_2002, "TFM_SST_TEST_2002",
     "Set interface with create flags"},
    {&tfm_sst_test_2003, "TFM_SST_TEST_2003",
     "Set interface with NULL data pointer"},
    {&tfm_sst_test_2004, "TFM_SST_TEST_2004",
     "Set interface with invalid data length"},
    {&tfm_sst_test_2005, "TFM_SST_TEST_2005",
     "Set interface with write once UID"},
    {&tfm_sst_test_2006, "TFM_SST_TEST_2006",
     "Get interface with valid data"},
    {&tfm_sst_test_2007, "TFM_SST_TEST_2007",
     "Get interface with zero data length"},
    {&tfm_sst_test_2008, "TFM_SST_TEST_2008",
     "Get interface with invalid UIDs"},
    {&tfm_sst_test_2009, "TFM_SST_TEST_2009",
     "Get interface with invalid data lengths and offsets"},
    {&tfm_sst_test_2010, "TFM_SST_TEST_2010",
     "Get interface with NULL data pointer"},
    {&tfm_sst_test_2011, "TFM_SST_TEST_2011",
     "Get info interface with write once UID"},
    {&tfm_sst_test_2012, "TFM_SST_TEST_2012",
     "Get info interface with valid UID"},
    {&tfm_sst_test_2013, "TFM_SST_TEST_2013",
     "Get info interface with invalid UIDs"},
    {&tfm_sst_test_2014, "TFM_SST_TEST_2014",
     "Get info interface with NULL info pointer"},
    {&tfm_sst_test_2015, "TFM_SST_TEST_2015",
     "Remove interface with valid UID"},
    {&tfm_sst_test_2016, "TFM_SST_TEST_2016",
     "Remove interface with write once UID"},
    {&tfm_sst_test_2017, "TFM_SST_TEST_2017",
     "Remove interface with invalid UID"},
    {&tfm_sst_test_2018, "TFM_SST_TEST_2018",
     "Block compaction after remove"},
    {&tfm_sst_test_2019, "TFM_SST_TEST_2019",
     "Multiple partial gets"},
    {&tfm_sst_test_2020, "TFM_SST_TEST_2020",
     "Multiple sets to same UID from same thread"},
    {&tfm_sst_test_2021, "TFM_SST_TEST_2021",
     "Get support interface"},
    {&tfm_sst_test_2022, "TFM_SST_TEST_2022",
     "Set, get and remove interface with different asset sizes"},
};

void register_testsuite_s_psa_ps_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(psa_ps_s_tests) / sizeof(psa_ps_s_tests[0]));

    set_testsuite("PSA protected storage S interface tests (TFM_SST_TEST_2XXX)",
                  psa_ps_s_tests, list_size, p_test_suite);

#ifdef SST_SHOW_FLASH_WARNING
    TEST_LOG("\r\n**WARNING** The SST regression tests reduce the life of the "
             "flash memory as they write/erase multiple times the memory. \r\n"
             "Please, set the SST_RAM_FS flag to use RAM instead of flash."
             "\r\n\r\n");
#endif
}

/**
 * \brief Tests set function with:
 * - Valid UID, no data, no flags
 * - Invalid UID, no data, no flags
 */
static void tfm_sst_test_2001(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = 0;
    const uint8_t write_data[] = {0};

    /* Set with no data and no flags and a valid UID */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    /* Attempt to set a second time */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail the second time with valid UID");
        return;
    }

    /* Set with an invalid UID */
    status = psa_ps_set(INVALID_UID, data_len, write_data, flags);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Set should not succeed with an invalid UID");
        return;
    }

    /* Call remove to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests set function with:
 * - Zero create flags
 * - Valid create flags (with previously created UID)
 * - Invalid create flags
 */
static void tfm_sst_test_2002(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    /* Set with no flags */
    status = psa_ps_set(WRITE_ONCE_UID, data_len, write_data, flags);
    if (status == PSA_SUCCESS) {
        /* Set with valid flag: PSA_STORAGE_FLAG_WRITE_ONCE (with previously
         * created UID)
         * Note: Once created, WRITE_ONCE_UID cannot be deleted. It is reused
         * across multiple tests.
         */
        status = psa_ps_set(WRITE_ONCE_UID, WRITE_ONCE_DATA_SIZE,
                            WRITE_ONCE_DATA, PSA_STORAGE_FLAG_WRITE_ONCE);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Set should not fail with valid flags (existing UID)");
            return;
        }
    } else if (status == PSA_ERROR_NOT_PERMITTED) {
        /* The UID has already been created with the PSA_STORAGE_FLAG_WRITE_ONCE
         * flag in a previous test run, so skip creating it again and emit a
         * warning.
         */
        TEST_LOG("Note: The UID in this test has already been created with\r\n"
                "the PSA_STORAGE_FLAG_WRITE_ONCE flag in a previous test\r\n"
                "run. Wipe the storage area to run the full test.\r\n");
    } else {
        TEST_FAIL("Set should not fail with no flags");
        return;
    }

    /* Set with invalid flags */
    status = psa_ps_set(uid, data_len, write_data, INVALID_FLAG);
    if (status != PSA_ERROR_NOT_SUPPORTED) {
        TEST_FAIL("Set should not succeed with invalid flags");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests set function with:
 * - NULL data pointer and zero data length
 *
 * \note A request with a null data pointer and data length not equal to zero is
 *       treated as a secure violation. TF-M framework will reject such requests
 *       so this case is not tested here.
 *
 */
static void tfm_sst_test_2003(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = 0;

    /* Set with NULL data pointer */
    status = psa_ps_set(uid, data_len, NULL, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should succeed with NULL data pointer and zero length");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests set function with:
 * - Data length longer than maximum permitted
 */
static void tfm_sst_test_2004(struct test_result_t *ret)
{
#ifndef TFM_PSA_API
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = INVALID_DATA_LEN;
    const uint8_t write_data[] = WRITE_DATA;

    /* A parameter with a buffer pointer where its data length is longer than
     * maximum permitted, it is treated as a secure violation.
     * TF-M framework rejects the request with a proper error code.
     * The SST secure PSA PS implementation returns
     * PSA_ERROR_INVALID_ARGUMENT in that case.
     */

    /* Set with data length longer than the maximum supported */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Set should not succeed with invalid data length");
        return;
    }

#endif
    ret->val = TEST_PASSED;
}

/**
 * \brief Tests set function with:
 * - Write once UID that has already been created
 */
static void tfm_sst_test_2005(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = WRITE_ONCE_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t write_len = WRITE_DATA_SIZE;
    const uint32_t read_len = WRITE_ONCE_DATA_SIZE;
    const uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = WRITE_ONCE_READ_DATA;
    size_t read_data_len = 0;

    /* Set a write once UID a second time */
    status = psa_ps_set(uid, write_len, write_data, flags);
    if (status != PSA_ERROR_NOT_PERMITTED) {
        TEST_FAIL("Set should not rewrite a write once UID");
        return;
    }

    /* Get write once data */
    status = psa_ps_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

    /* Check that write once data has not changed */
    if (tfm_memcmp(read_data, WRITE_ONCE_RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Write once data should not have changed");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get function with:
 * - Valid data, zero offset
 * - Valid data, non-zero offset
 */
static void tfm_sst_test_2006(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    uint32_t data_len = WRITE_DATA_SIZE;
    uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    const uint8_t *p_read_data = read_data;
    size_t read_data_len = 0;

    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get the entire data */
    status = psa_ps_get(uid, offset, data_len, read_data + HALF_PADDING_SIZE,
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

    /* Check that the data is correct, including no illegal pre- or post-data */
    if (tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to result data");
        return;
    }

    /* Reset read data */
    tfm_memcpy(read_data, READ_DATA, sizeof(read_data));

    /* Read from offset 2 to 2 bytes before end of the data */
    offset = 2;
    data_len -= offset + 2;

    status = psa_ps_get(uid, offset, data_len, read_data + HALF_PADDING_SIZE,
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

    /* Check that the correct data was read */
    if (tfm_memcmp(p_read_data, "____", HALF_PADDING_SIZE) != 0) {
        TEST_FAIL("Read data contains illegal pre-data");
        return;
    }

    p_read_data += HALF_PADDING_SIZE;

    if (tfm_memcmp(p_read_data, write_data + offset, data_len) != 0) {
        TEST_FAIL("Read data incorrect");
        return;
    }

    p_read_data += data_len;

    if (tfm_memcmp(p_read_data, "____", HALF_PADDING_SIZE) != 0) {
        TEST_FAIL("Read data contains illegal post-data");
        return;
    }

    /* Call remove to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get function with:
 * - Zero data length, zero offset
 * - Zero data length, non-zero offset
 */
static void tfm_sst_test_2007(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t write_len = WRITE_DATA_SIZE;
    const uint32_t read_len = 0;
    uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    status = psa_ps_set(uid, write_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get zero data from zero offset */
    status = psa_ps_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail with zero data len");
        return;
    }

    /* Check that the read data is unchanged */
    if (tfm_memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }

    offset = 5;

    /* Get zero data from non-zero offset */
    status = psa_ps_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

    /* Check that the read data is unchanged */
    if (tfm_memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }

    /* Call remove to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get function with:
 * - Unset UID
 * - Invalid UID
 */
static void tfm_sst_test_2008(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const uint32_t data_len = 1;
    const uint32_t offset = 0;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    /* Get with UID that has not yet been set */
    status = psa_ps_get(uid, offset, data_len, read_data + HALF_PADDING_SIZE,
                        &read_data_len);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get succeeded with non-existant UID");
        return;
    }

    /* Check that the read data is unchanged */
    if (tfm_memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data not equal to original read data");
        return;
    }

    /* Get with invalid UID */
    status = psa_ps_get(INVALID_UID, offset, data_len,
                        read_data + HALF_PADDING_SIZE, &read_data_len);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Get succeeded with invalid UID");
        return;
    }

    /* Check that the read data is unchanged */
    if (tfm_memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data not equal to original read data");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get function with:
 * - Offset greater than UID length
 * - Data length greater than UID length
 * - Data length + offset greater than UID length
 * - Invalid data len and offset
 */
static void tfm_sst_test_2009(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t write_len = WRITE_DATA_SIZE;
    uint32_t read_len;
    uint32_t offset;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    status = psa_ps_set(uid, write_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get with offset greater than UID's length */
    read_len = 1;
    offset = write_len + 1;

    status = psa_ps_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                        &read_data_len);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Get should not succeed with offset too large");
        return;
    }

    /* Check that the read data is unchanged */
    if (tfm_memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }

    /* Get with data length greater than UID's length */
    read_len = write_len + 1;
    offset = 0;

    status = psa_ps_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should succeed with data length larger than UID's "
                  "length");
        return;
    }

    if (read_data_len != write_len) {
        TEST_FAIL("Read data length should be equal to UID's length");
        return;
    }

    /* Check that the read data is unchanged */
    if (tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }

    /* Get with offset + data length greater than UID's length, but individually
     * valid
     */
    read_len = write_len;
    offset = 1;

    /* Reset read_data to original READ_DATA */
    tfm_memcpy(read_data, READ_DATA, sizeof(read_data));

    status = psa_ps_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should succeed with offset + data length too large, "
                  "but individually valid");
        return;
    }

    if (read_data_len != write_len - offset) {
        TEST_FAIL("Read data length should be equal to the UID's remaining "
                  "size starting from offset");
        return;
    }

    /* Check that the read data is unchanged */
    if (tfm_memcmp(read_data, OFFSET_RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }

#ifndef TFM_PSA_API
    /* Get with data length and offset set to invalid values */
    read_len = INVALID_DATA_LEN;
    offset = INVALID_OFFSET;

    /* Reset read_data to original READ_DATA */
    tfm_memcpy(read_data, READ_DATA, sizeof(read_data));

    /* A parameter with a buffer pointer where its data length is longer than
     * maximum permitted, it is treated as a secure violation.
     * TF-M framework rejects the request with a proper error code.
     * The SST secure PSA PS implementation returns
     * PSA_ERROR_INVALID_ARGUMENT in that case.
     */

    status = psa_ps_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                        &read_data_len);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Get should not succeed with invalid arguments");
        return;
    }

    /* Check that the read data is unchanged */
    if (tfm_memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }
#endif

    /* Call remove to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get function with:
 * - NULL data pointer and zero data length
 *
 * \note A request with a null data pointer and data length not equal to zero is
 *       treated as a secure violation. TF-M framework will reject such requests
 *       so this case is not tested here.
 *
 */
static void tfm_sst_test_2010(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    size_t read_data_len = 0;

    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get with NULL data pointer */
    status = psa_ps_get(uid, offset, 0, NULL, &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should succeed with NULL data pointer and zero length");
        return;
    }

    /* Call remove to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get info function with:
 * - Write once UID
 */
static void tfm_sst_test_2011(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = WRITE_ONCE_UID;
    struct psa_storage_info_t info = {0};

    /* Get info for write once UID */
    status = psa_ps_get_info(uid, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get info should not fail for write once UID");
        return;
    }

    /* Check that the info struct contains the correct values */
    if (info.size != WRITE_ONCE_DATA_SIZE) {
        TEST_FAIL("Size incorrect for write once UID");
        return;
    }

    if (info.flags != PSA_STORAGE_FLAG_WRITE_ONCE) {
        TEST_FAIL("Flags incorrect for write once UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get info function with:
 * - Valid UID
 */
static void tfm_sst_test_2012(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    struct psa_storage_info_t info = {0};
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get info for valid UID */
    status = psa_ps_get_info(uid, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get info should not fail with valid UID");
        return;
    }

    /* Check that the info struct contains the correct values */
    if (info.size != data_len) {
        TEST_FAIL("Size incorrect for valid UID");
        return;
    }

    if (info.flags != flags) {
        TEST_FAIL("Flags incorrect for valid UID");
        return;
    }

    /* Call remove to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get info function with:
 * - Unset UID
 * - Invalid UID
 */
static void tfm_sst_test_2013(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    struct psa_storage_info_t info = {0};

    /* Get info with UID that has not yet been set */
    status = psa_ps_get_info(uid, &info);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get info should not succeed with unset UID");
        return;
    }

    /* Check that the info struct has not been modified */
    if (info.size != 0) {
        TEST_FAIL("Size should not have changed");
        return;
    }

    if (info.flags != 0) {
        TEST_FAIL("Flags should not have changed");
        return;
    }

    /* Get info with invalid UID */
    status = psa_ps_get_info(INVALID_UID, &info);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Get info should not succeed with invalid UID");
        return;
    }

    /* Check that the info struct has not been modified */
    if (info.size != 0) {
        TEST_FAIL("Size should not have changed");
        return;
    }

    if (info.flags != 0) {
        TEST_FAIL("Flags should not have changed");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get info function with:
 * - NULL info pointer
 */
static void tfm_sst_test_2014(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* A parameter with a null pointer is treated as a secure violation.
     * TF-M framework rejects the request with a proper error code.
     * The SST secure PSA PS implementation returns
     * PSA_ERROR_GENERIC_ERROR in that case.
     */

    /* Get info with NULL info pointer */
#ifndef TFM_PSA_API
    status = psa_ps_get_info(uid, NULL);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Get info should not succeed with NULL info pointer");
        return;
    }
#endif

    /* Call remove to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests remove function with:
 * - Valid UID
 */
static void tfm_sst_test_2015(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    struct psa_storage_info_t info = {0};
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Call remove with valid ID */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    /* Check that get info fails for removed UID */
    status = psa_ps_get_info(uid, &info);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get info should not succeed with removed UID");
        return;
    }

    /* Check that get fails for removed UID */
    status = psa_ps_get(uid, offset, data_len, read_data, &read_data_len);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get should not succeed with removed UID");
        return;
    }

    /* Check that remove fails for removed UID */
    status = psa_ps_remove(uid);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Remove should not succeed with removed UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests remove function with:
 * - Write once UID
 */
static void tfm_sst_test_2016(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = WRITE_ONCE_UID;

    /* Call remove with write once UID */
    status = psa_ps_remove(uid);
    if (status != PSA_ERROR_NOT_PERMITTED) {
        TEST_FAIL("Remove should not succeed with write once UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests remove function with:
 * - Invalid UID
 */
static void tfm_sst_test_2017(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = INVALID_UID;

    /* Call remove with an invalid UID */
    status = psa_ps_remove(uid);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Remove should not succeed with invalid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests data block compact feature.
 *        Set UID 1 to locate it at the beginning of the block. Then set UID 2
 *        to be located after UID 1 and remove UID 1. UID 2 will be compacted to
 *        the beginning of the block. This test verifies that the compaction
 *        works correctly by reading back UID 2.
 */
static void tfm_sst_test_2018(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid_1 = TEST_UID_2;
    const psa_storage_uid_t uid_2 = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len_2 = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    const uint8_t write_data_1[] = "UID 1 DATA";
    const uint8_t write_data_2[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    /* Set UID 1 */
    status = psa_ps_set(uid_1, sizeof(write_data_1), write_data_1, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail for UID 1");
        return;
    }

    /* Set UID 2 */
    status = psa_ps_set(uid_2, data_len_2, write_data_2, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail for UID 2");
        return;
    }

    /* Remove UID 1. This should cause UID 2 to be compacted to the beginning of
     * the block.
     */
    status = psa_ps_remove(uid_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail for UID 1");
        return;
    }

    /* If the compact worked as expected, the test should be able to read back
     * the data from UID 2 correctly.
     */
    status = psa_ps_get(uid_2, offset, data_len_2,
                        read_data + HALF_PADDING_SIZE, &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail for UID 2");
        return;
    }

    if (tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    /* Remove UID 2 to clean up storage for the next test */
    status = psa_ps_remove(uid_2);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail for UID 2");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests set and multiple partial gets.
 */
static void tfm_sst_test_2019(struct test_result_t *ret)
{
    psa_status_t status;
    uint32_t i;
    const psa_storage_uid_t uid = TEST_UID_1;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    /* Set the entire data into UID */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get the data from UID one byte at a time */
    for (i = 0; i < data_len; ++i) {
        status = psa_ps_get(uid, offset, 1,
                            (read_data + HALF_PADDING_SIZE + i),
                             &read_data_len);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Get should not fail for partial read");
            return;
        }

        ++offset;
    }

    if (tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    /* Remove UID to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests multiple sets to the same UID.
 */
static void tfm_sst_test_2020(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t offset = 0;
    const uint8_t write_data_1[] = "ONE";
    const uint8_t write_data_2[] = "TWO";
    const uint8_t write_data_3[] = "THREE";
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    /* Set write data 1 into UID */
    status = psa_ps_set(uid, sizeof(write_data_1), write_data_1, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("First set should not fail");
        return;
    }

    /* Set write data 2 into UID */
    status = psa_ps_set(uid, sizeof(write_data_2), write_data_2, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Second set should not fail");
        return;
    }

    /* Set write data 3 into UID */
    status = psa_ps_set(uid, sizeof(write_data_3), write_data_3, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Third set should not fail");
        return;
    }

    status = psa_ps_get(uid, offset, sizeof(write_data_3), read_data,
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

    /* Check that get returns the last data to be set */
    if (tfm_memcmp(read_data, write_data_3, sizeof(write_data_3)) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    /* Remove UID to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get support function.
 */
static void tfm_sst_test_2021(struct test_result_t *ret)
{
    uint32_t support_flags;

    support_flags = psa_ps_get_support();
    if (support_flags != 0) {
        TEST_FAIL("Support flags should be 0");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests set, get_info, get and remove function with:
 * - Valid UID's
 * - Data length of different asset sizes
 * - No flags
 */
static void tfm_sst_test_2022(struct test_result_t *ret)
{
    uint8_t cycle;
    psa_status_t status;
    const psa_storage_uid_t test_uid[TEST_1022_CYCLES] = {
        TEST_UID_1,
        TEST_UID_2,
        TEST_UID_3};
    const uint32_t test_asset_sizes[TEST_1022_CYCLES] = {
        SST_MAX_ASSET_SIZE >> 2,
        SST_MAX_ASSET_SIZE >> 1,
        SST_MAX_ASSET_SIZE};

    /* Loop to test different asset sizes and UID's*/
    for (cycle = 0; cycle < TEST_1022_CYCLES; cycle++) {
        uint32_t data_size = test_asset_sizes[cycle];
        psa_storage_uid_t uid = test_uid[cycle];
        struct psa_storage_info_t info = {0};

        tfm_memset(read_asset_data, 0x00, sizeof(read_asset_data));

        /* Set with data and no flags and a valid UID */
        status = psa_ps_set(uid,
                            data_size,
                            write_asset_data,
                            PSA_STORAGE_FLAG_NONE);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Set should not fail with valid UID");
            return;
        }

        /* Get info for valid UID */
        status = psa_ps_get_info(uid, &info);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Get info should not fail with valid UID");
            return;
        }

        /* Check that the info struct contains the correct values */
        if (info.size != data_size) {
            TEST_FAIL("Size incorrect for valid UID");
            return;
        }

        if (info.flags != PSA_STORAGE_FLAG_NONE) {
            TEST_FAIL("Flags incorrect for valid UID");
            return;
        }

        /* Check that thread can still get UID */
        status = psa_ps_get(uid, 0, data_size, read_asset_data,
                            &read_asset_datal_len);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Get should not fail with valid UID");
            return;
        }

        /* Check that thread's UID data has not been modified */
        if (tfm_memcmp(read_asset_data, write_asset_data, data_size) != 0) {
            TEST_FAIL("Read data should be equal to original write data");
            return;
        }

        /* Call remove to clean up storage for the next test */
        status = psa_ps_remove(uid);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Remove should not fail with valid UID");
            return;
        }
    }

    ret->val = TEST_PASSED;
}
