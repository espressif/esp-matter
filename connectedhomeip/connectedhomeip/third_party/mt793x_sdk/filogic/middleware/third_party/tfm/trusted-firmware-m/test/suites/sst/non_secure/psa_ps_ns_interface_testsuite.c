/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_ns_tests.h"

#include <stdio.h>
#include <string.h>

#include "ns_test_helpers.h"
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
#define OFFSET_READ_DATA         "HEQUICKBROWNFOXJUMPSOVERALAZYDOG"
#define OFFSET_RESULT_DATA       ("____" OFFSET_READ_DATA "_____")

#define WRITE_DATA               "THEQUICKBROWNFOXJUMPSOVERALAZYDOG"
#define WRITE_DATA_SIZE          (sizeof(WRITE_DATA) - 1)
#define READ_DATA                "_________________________________________"
#define RESULT_DATA              ("____" WRITE_DATA "____")

#define TEST_1025_CYCLES         3U

static const uint8_t write_asset_data[SST_MAX_ASSET_SIZE] = {0xAF};
static uint8_t read_asset_data[SST_MAX_ASSET_SIZE] = {0};
static size_t read_asset_data_len = 0;

/* List of tests */
static void tfm_sst_test_1001(struct test_result_t *ret);
static void tfm_sst_test_1002(struct test_result_t *ret);
static void tfm_sst_test_1003(struct test_result_t *ret);
static void tfm_sst_test_1004(struct test_result_t *ret);
static void tfm_sst_test_1005(struct test_result_t *ret);
static void tfm_sst_test_1006(struct test_result_t *ret);
static void tfm_sst_test_1007(struct test_result_t *ret);
static void tfm_sst_test_1008(struct test_result_t *ret);
static void tfm_sst_test_1009(struct test_result_t *ret);
static void tfm_sst_test_1010(struct test_result_t *ret);
static void tfm_sst_test_1011(struct test_result_t *ret);
static void tfm_sst_test_1012(struct test_result_t *ret);
static void tfm_sst_test_1013(struct test_result_t *ret);
static void tfm_sst_test_1014(struct test_result_t *ret);
static void tfm_sst_test_1015(struct test_result_t *ret);
#ifdef TFM_NS_CLIENT_IDENTIFICATION
static void tfm_sst_test_1016(struct test_result_t *ret);
static void tfm_sst_test_1017(struct test_result_t *ret);
static void tfm_sst_test_1018(struct test_result_t *ret);
static void tfm_sst_test_1019(struct test_result_t *ret);
static void tfm_sst_test_1020(struct test_result_t *ret);
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
static void tfm_sst_test_1021(struct test_result_t *ret);
static void tfm_sst_test_1022(struct test_result_t *ret);
static void tfm_sst_test_1023(struct test_result_t *ret);
static void tfm_sst_test_1024(struct test_result_t *ret);
static void tfm_sst_test_1025(struct test_result_t *ret);

static struct test_t psa_ps_ns_tests[] = {
    {&tfm_sst_test_1001, "TFM_SST_TEST_1001",
     "Set interface"},
    {&tfm_sst_test_1002, "TFM_SST_TEST_1002",
     "Set interface with create flags"},
    {&tfm_sst_test_1003, "TFM_SST_TEST_1003",
     "Set interface with NULL data pointer"},
    {&tfm_sst_test_1004, "TFM_SST_TEST_1004",
     "Set interface with write once UID"},
    {&tfm_sst_test_1005, "TFM_SST_TEST_1005",
     "Get interface with valid data"},
    {&tfm_sst_test_1006, "TFM_SST_TEST_1006",
     "Get interface with zero data length"},
    {&tfm_sst_test_1007, "TFM_SST_TEST_1007",
     "Get interface with invalid UIDs"},
    {&tfm_sst_test_1008, "TFM_SST_TEST_1008",
     "Get interface with invalid data lengths and offsets"},
    {&tfm_sst_test_1009, "TFM_SST_TEST_1009",
     "Get interface with NULL data pointer"},
    {&tfm_sst_test_1010, "TFM_SST_TEST_1010",
     "Get info interface with write once UID"},
    {&tfm_sst_test_1011, "TFM_SST_TEST_1011",
     "Get info interface with valid UID"},
    {&tfm_sst_test_1012, "TFM_SST_TEST_1012",
     "Get info interface with invalid UIDs"},
    {&tfm_sst_test_1013, "TFM_SST_TEST_1013",
     "Remove interface with valid UID"},
    {&tfm_sst_test_1014, "TFM_SST_TEST_1014",
     "Remove interface with write once UID"},
    {&tfm_sst_test_1015, "TFM_SST_TEST_1015",
     "Remove interface with invalid UID"},
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    {&tfm_sst_test_1016, "TFM_SST_TEST_1016",
     "Get interface with invalid thread name"},
    {&tfm_sst_test_1017, "TFM_SST_TEST_1017",
     "Get info interface with invalid thread name"},
    {&tfm_sst_test_1018, "TFM_SST_TEST_1018",
     "Remove interface with invalid thread name"},
    {&tfm_sst_test_1019, "TFM_SST_TEST_1019",
     "Attempt to access UID belonging to another thread"},
    {&tfm_sst_test_1020, "TFM_SST_TEST_1020",
     "Set UID alternately from two threads"},
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
    {&tfm_sst_test_1021, "TFM_SST_TEST_1021",
     "Block compaction after remove"},
    {&tfm_sst_test_1022, "TFM_SST_TEST_1022",
     "Multiple partial gets"},
    {&tfm_sst_test_1023, "TFM_SST_TEST_1023",
     "Multiple sets to same UID from same thread"},
    {&tfm_sst_test_1024, "TFM_SST_TEST_1024",
     "Get support interface"},
    {&tfm_sst_test_1025, "TFM_SST_TEST_1025",
     "Set, get and remove interface with different asset sizes"},
};

void register_testsuite_ns_psa_ps_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(psa_ps_ns_tests) / sizeof(psa_ps_ns_tests[0]));

    set_testsuite(
                 "PSA protected storage NS interface tests (TFM_SST_TEST_1XXX)",
                  psa_ps_ns_tests, list_size, p_test_suite);

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
TFM_SST_NS_TEST(1001, "Thread_A")
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
TFM_SST_NS_TEST(1002, "Thread_A")
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
 *       and not return to the NSPE so this case is not tested here.
 *
 */
TFM_SST_NS_TEST(1003, "Thread_A")
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
 * - Write once UID that has already been created
 */
TFM_SST_NS_TEST(1004, "Thread_A")
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
    if (memcmp(read_data, WRITE_ONCE_RESULT_DATA, sizeof(read_data)) != 0) {
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
TFM_SST_NS_TEST(1005, "Thread_A")
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    uint32_t data_len = WRITE_DATA_SIZE;
    uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    const uint8_t *p_read_data = read_data;

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
    if (memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to result data");
        return;
    }

    /* Reset read data */
    memcpy(read_data, READ_DATA, sizeof(read_data));

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
    if (memcmp(p_read_data, "____", HALF_PADDING_SIZE) != 0) {
        TEST_FAIL("Read data contains illegal pre-data");
        return;
    }

    p_read_data += HALF_PADDING_SIZE;

    if (memcmp(p_read_data, write_data + offset, data_len) != 0) {
        TEST_FAIL("Read data incorrect");
        return;
    }

    p_read_data += data_len;

    if (memcmp(p_read_data, "____", HALF_PADDING_SIZE) != 0) {
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
TFM_SST_NS_TEST(1006, "Thread_A")
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
    if (memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
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
    if (memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
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
TFM_SST_NS_TEST(1007, "Thread_A")
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
    if (memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
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
    if (memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
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
 */
TFM_SST_NS_TEST(1008, "Thread_A")
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
    if (memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
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
    if (memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }

    /* Get with offset + data length greater than UID's length, but individually
     * valid
     */
    read_len = write_len;
    offset = 1;

    /* Reset read_data to original READ_DATA */
    memcpy(read_data, READ_DATA, sizeof(read_data));

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
    if (memcmp(read_data, OFFSET_RESULT_DATA, sizeof(read_data)) != 0) {
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
 * - NULL data pointer and zero data length
 *
 * \note A request with a null data pointer and data length not equal to zero is
 *       treated as a secure violation. TF-M framework will reject such requests
 *       and not return to the NSPE so this case is not tested here.
 *
 */
TFM_SST_NS_TEST(1009, "Thread_A")
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    size_t read_data_length = 0;

    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get with NULL data pointer */
    status = psa_ps_get(uid, offset, 0, NULL, &read_data_length);
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
TFM_SST_NS_TEST(1010, "Thread_A")
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
TFM_SST_NS_TEST(1011, "Thread_A")
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
TFM_SST_NS_TEST(1012, "Thread_A")
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
 * \brief Tests remove function with:
 * - Valid UID
 */
TFM_SST_NS_TEST(1013, "Thread_A")
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
TFM_SST_NS_TEST(1014, "Thread_A")
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
TFM_SST_NS_TEST(1015, "Thread_A")
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

#ifdef TFM_NS_CLIENT_IDENTIFICATION
/**
 * \brief Sets UID with a valid thread name.
 */
static void tfm_sst_test_1016_task_1(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Calls get with an invalid thread name.
 */
static void tfm_sst_test_1016_task_2(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    status = psa_ps_get(uid, offset, data_len, read_data, &read_data_len);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get should not succeed with invalid thread name");
        return;
    }

    /* Check that read data has not been modified */
    if (memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should not have changed");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Removes UID with a valid thread name to clean up storage.
 */
static void tfm_sst_test_1016_task_3(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;

    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get function with an invalid thread name.
 */
static void tfm_sst_test_1016(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1016_task_1);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test(INVALID_THREAD_NAME, ret, tfm_sst_test_1016_task_2);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1016_task_3);
}

/**
 * \brief Sets UID with a valid thread name.
 */
static void tfm_sst_test_1017_task_1(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Calls get info with an invalid thread name.
 */
static void tfm_sst_test_1017_task_2(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    struct psa_storage_info_t info = {0};

    status = psa_ps_get_info(uid, &info);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get info should not succeed with invalid thread name");
        return;
    }

    /* Check that info has not been modified */
    if (info.size != 0 || info.flags != 0) {
        TEST_FAIL("Info should not have changed");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Removes UID with a valid thread name to clean up storage.
 */
static void tfm_sst_test_1017_task_3(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;

    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get info function with an invalid thread name.
 */
static void tfm_sst_test_1017(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1017_task_1);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test(INVALID_THREAD_NAME, ret, tfm_sst_test_1017_task_2);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1017_task_3);
}

/**
 * \brief Sets UID with a valid thread name.
 */
static void tfm_sst_test_1018_task_1(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Calls remove with an invalid thread name.
 */
static void tfm_sst_test_1018_task_2(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;

    status = psa_ps_remove(uid);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Remove should not succeed with invalid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Removes UID with a valid thread name to clean up storage.
 */
static void tfm_sst_test_1018_task_3(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;

    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests remove function with an invalid thread name.
 */
static void tfm_sst_test_1018(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1018_task_1);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test(INVALID_THREAD_NAME, ret, tfm_sst_test_1018_task_2);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1018_task_3);
}

/**
 * \brief Sets UID with first thread.
 */
static void tfm_sst_test_1019_task_1(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint8_t write_data[] = "Thread A data";

    status = psa_ps_set(uid, sizeof(write_data), write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Attempts to access same UID from second thread.
 */
static void tfm_sst_test_1019_task_2(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    struct psa_storage_info_t info = {0};
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t data_len = WRITE_DATA_SIZE;
    const uint32_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    /* Attempt to access the other thread's UID */
    status = psa_ps_get(uid, offset, data_len, read_data, &read_data_len);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get should not find another thread's UID");
        return;
    }

    /* Check that read data has not been modified */
    if (memcmp(read_data, READ_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should not have changed");
        return;
    }

    status = psa_ps_get_info(uid, &info);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get info should not find another thread's UID");
        return;
    }

    /* Check that info has not been modified */
    if (info.size != 0 || info.flags != 0) {
        TEST_FAIL("Info should not have changed");
        return;
    }

    status = psa_ps_remove(uid);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Remove should not find another thread's UID");
        return;
    }

    /* Create the same UID, but belonging to this thread */
    status = psa_ps_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    status = psa_ps_get(uid, offset, data_len, read_data + HALF_PADDING_SIZE,
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail with valid UID");
        return;
    }

    /* Check that the data read belongs to this thread, not the other one */
    if (memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
        TEST_FAIL("Read data should be equal to result data");
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
 * \brief Checks that first thread's UID has not been modified.
 */
static void tfm_sst_test_1019_task_3(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    struct psa_storage_info_t info = {0};
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint32_t offset = 0;
    const uint8_t write_data[] = "Thread A data";
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    const uint32_t data_len = sizeof(write_data);

    /* Check that first thread can still get info for UID */
    status = psa_ps_get_info(uid, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get info should not fail with valid UID");
        return;
    }

    /* Check that first thread's UID info has not been modified */
    if (info.size != data_len || info.flags != flags) {
        TEST_FAIL("Info should be equal to original info");
        return;
    }

    /* Check that first thread can still get UID */
    status = psa_ps_get(uid, offset, data_len, read_data, &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail with valid UID");
        return;
    }

    /* Check that first thread's UID data has not been modified */
    if (memcmp(read_data, write_data, data_len) != 0) {
        TEST_FAIL("Read data should be equal to original write data");
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
 * \brief Tests attempting to access UID belonging to another thread.
 */
static void tfm_sst_test_1019(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1019_task_1);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1019_task_2);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1019_task_3);
}

/**
 * \brief Sets TEST_UID_1 from Thread_A.
 */
static void tfm_sst_test_1020_task_1(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint8_t write_data[] = "A";

    status = psa_ps_set(uid, sizeof(write_data), write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should succeed for Thread_A");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Sets TEST_UID_1 from Thread_B.
 */
static void tfm_sst_test_1020_task_2(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint8_t write_data[] = "B";

    status = psa_ps_set(uid, sizeof(write_data), write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should succeed for Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Sets TEST_UID_1 again from Thread_A.
 */
static void tfm_sst_test_1020_task_3(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint8_t write_data[] = "HELLO";

    status = psa_ps_set(uid, sizeof(write_data), write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Second set should succeed for Thread_A");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Sets TEST_UID_1 again from Thread_B.
 */
static void tfm_sst_test_1020_task_4(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const uint8_t write_data[] = "WORLD_1234";

    status = psa_ps_set(uid, sizeof(write_data), write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Second set should succeed for Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Gets TEST_UID_1 from Thread_A.
 */
static void tfm_sst_test_1020_task_5(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const uint32_t offset = 0;
    const uint8_t write_data[] = "HELLO";
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    status = psa_ps_get(uid, offset, sizeof(write_data), read_data,
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should succeed for Thread_A");
        return;
    }

    /* Check that UID contains Thread_A's data */
    if (memcmp(read_data, write_data, sizeof(write_data)) != 0) {
        TEST_FAIL("Read data incorrect for Thread_A");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Gets TEST_UID_1 from Thread_B.
 */
static void tfm_sst_test_1020_task_6(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const uint32_t offset = 0;
    const uint8_t write_data[] = "WORLD_1234";
    uint8_t read_data[] = READ_DATA;
    size_t read_data_len = 0;

    status = psa_ps_get(uid, offset, sizeof(write_data), read_data,
                        &read_data_len);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should succeed for Thread_B");
        return;
    }

    /* Check that UID contains Thread_B's data */
    if (memcmp(read_data, write_data, sizeof(write_data)) != 0) {
        TEST_FAIL("Read data incorrect for Thread_B");
        return;
    }

    /* Call remove to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should work form Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Calls removes from Thread_B to clean up storage for the next test.
 */
static void tfm_sst_test_1020_task_7(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;

    /* Call remove to clean up storage for the next test */
    status = psa_ps_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should work form Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests writing data to a UID alternately from two threads before
 *        read-back.
 */
static void tfm_sst_test_1020(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1020_task_1);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1020_task_2);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1020_task_3);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1020_task_4);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1020_task_5);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1020_task_6);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1020_task_7);
}
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

/**
 * \brief Tests data block compact feature.
 *        Set UID 1 to locate it at the beginning of the block. Then set UID 2
 *        to be located after UID 1 and remove UID 1. UID 2 will be compacted to
 *        the beginning of the block. This test verifies that the compaction
 *        works correctly by reading back UID 2.
 */
TFM_SST_NS_TEST(1021, "Thread_A")
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

    if (memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
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
TFM_SST_NS_TEST(1022, "Thread_A")
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

    if (memcmp(read_data, RESULT_DATA, sizeof(read_data)) != 0) {
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
TFM_SST_NS_TEST(1023, "Thread_A")
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
    if (memcmp(read_data, write_data_3, sizeof(write_data_3)) != 0) {
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
TFM_SST_NS_TEST(1024, "Thread_A")
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
TFM_SST_NS_TEST(1025, "Thread_A")
{
    uint8_t cycle;
    psa_status_t status;
    const psa_storage_uid_t test_uid[TEST_1025_CYCLES] = {
        TEST_UID_1,
        TEST_UID_2,
        TEST_UID_3};
    const uint32_t test_asset_sizes[TEST_1025_CYCLES] = {
        SST_MAX_ASSET_SIZE >> 2,
        SST_MAX_ASSET_SIZE >> 1,
        SST_MAX_ASSET_SIZE};

    /* Loop to test different asset sizes and UID's*/
    for (cycle = 0; cycle < TEST_1025_CYCLES; cycle++) {
        uint32_t data_size = test_asset_sizes[cycle];
        psa_storage_uid_t uid = test_uid[cycle];
        struct psa_storage_info_t info = {0};

        memset(read_asset_data, 0x00, sizeof(read_asset_data));

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
                            &read_asset_data_len);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Get should not fail with valid UID");
            return;
        }

        /* Check that thread's UID data has not been modified */
        if (memcmp(read_asset_data, write_asset_data, data_size) != 0) {
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
