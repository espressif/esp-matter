/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "its_tests_common.h"
#include "psa/internal_trusted_storage.h"
#if DOMAIN_NS == 1
#include <string.h>
#else
#include "tfm_memory_utils.h"
#endif
#include "flash_layout.h"
#ifdef TARGET_PLATFORM_MT793X
#include "mt7933_layout.h"
#endif

#define TEST_019_CYCLES    3U

static const uint8_t write_asset_data[ITS_MAX_ASSET_SIZE] = {0xBF};
static uint8_t read_asset_data[ITS_MAX_ASSET_SIZE] = {0};

void tfm_its_test_common_001(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t data_len = 0;
    const uint8_t write_data[] = {0};

    /* Set with no data and no flags and a valid UID */
    status = psa_its_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail with valid UID");
        return;
    }

    /* Attempt to set a second time */
    status = psa_its_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail the second time with valid UID");
        return;
    }

    /* Set with an invalid UID */
    status = psa_its_set(INVALID_UID, data_len, write_data, flags);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Set should not succeed with an invalid UID");
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

void tfm_its_test_common_002(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    /* Set with no flags */
    status = psa_its_set(WRITE_ONCE_UID, data_len, write_data, flags);
    if (status == PSA_SUCCESS) {
        /* Set with valid flag: PSA_STORAGE_FLAG_WRITE_ONCE (with previously
         * created UID)
         * Note: Once created, WRITE_ONCE_UID cannot be deleted. It is reused
         * across multiple tests.
         */
        status = psa_its_set(WRITE_ONCE_UID, WRITE_ONCE_DATA_SIZE,
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
    status = psa_its_set(uid, data_len, write_data, INVALID_FLAG);
    if (status != PSA_ERROR_NOT_SUPPORTED) {
        TEST_FAIL("Set should not succeed with invalid flags");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_003(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t data_len = 0;

    /* Set with NULL data pointer */
    status = psa_its_set(uid, data_len, NULL, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should succeed with NULL data pointer and zero length");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_004(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = WRITE_ONCE_UID;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t write_len = WRITE_DATA_SIZE;
    const size_t read_len = WRITE_ONCE_DATA_SIZE;
    const size_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = WRITE_ONCE_READ_DATA;
    size_t read_data_length = 0;
    int comp_result;

    /* Set a write once UID a second time */
    status = psa_its_set(uid, write_len, write_data, flags);
    if (status != PSA_ERROR_NOT_PERMITTED) {
        TEST_FAIL("Set should not rewrite a write once UID");
        return;
    }

    /* Get write once data */
    status = psa_its_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                         &read_data_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

#if DOMAIN_NS == 1U
    /* Check that write once data has not changed */
    comp_result = memcmp(read_data, WRITE_ONCE_RESULT_DATA, sizeof(read_data));
#else
    comp_result = tfm_memcmp(read_data, WRITE_ONCE_RESULT_DATA,
                             sizeof(read_data));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Write once data should not have changed");
        return;
    }

    /* Check that write once data length has not changed */
    if (read_data_length != read_len) {
        TEST_FAIL("Write once data length should not have changed");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_005(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    size_t data_len = WRITE_DATA_SIZE;
    size_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    const uint8_t *p_read_data = read_data;
    size_t read_data_length = 0;
    int comp_result;

    status = psa_its_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get the entire data */
    status = psa_its_get(uid, offset, data_len, read_data + HALF_PADDING_SIZE,
                         &read_data_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

#if DOMAIN_NS == 1U
    /* Check that the data is correct, including no illegal pre- or post-data */
    comp_result = memcmp(read_data, RESULT_DATA, sizeof(read_data));
#else
    comp_result = tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read data should be equal to result data");
        return;
    }

    /* Check that the length of data is correct */
    if (read_data_length != data_len) {
        TEST_FAIL("Read data length should be equal to requested data length");
        return;
    }

#if DOMAIN_NS == 1U
    /* Reset read data */
    memcpy(read_data, READ_DATA, sizeof(read_data));
#else
    tfm_memcpy(read_data, READ_DATA, sizeof(read_data));
#endif

    /* Read from offset 2 to 2 bytes before end of the data */
    offset = 2;
    data_len -= offset + 2;

    status = psa_its_get(uid, offset, data_len, read_data + HALF_PADDING_SIZE,
                         &read_data_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

#if DOMAIN_NS == 1U
    /* Check that the correct data was read */
    comp_result = memcmp(p_read_data, "____", HALF_PADDING_SIZE);
#else
    comp_result = tfm_memcmp(p_read_data, "____", HALF_PADDING_SIZE);
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read data contains illegal pre-data");
        return;
    }

    p_read_data += HALF_PADDING_SIZE;

#if DOMAIN_NS == 1U
    comp_result = memcmp(p_read_data, write_data + offset, data_len);
#else
    comp_result = tfm_memcmp(p_read_data, write_data + offset, data_len);
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read data incorrect");
        return;
    }

    p_read_data += data_len;

#if DOMAIN_NS == 1U
    comp_result = memcmp(p_read_data, "____", HALF_PADDING_SIZE);
#else
    comp_result = tfm_memcmp(p_read_data, "____", HALF_PADDING_SIZE);
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read data contains illegal post-data");
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

void tfm_its_test_common_006(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t write_len = WRITE_DATA_SIZE;
    const size_t read_len = 0;
    size_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_length = 1;
    int comp_result;

    status = psa_its_set(uid, write_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get zero data from zero offset */
    status = psa_its_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                         &read_data_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail with zero data len");
        return;
    }

#if DOMAIN_NS == 1U
    /* Check that the read data is unchanged */
    comp_result = memcmp(read_data, READ_DATA, sizeof(read_data));
#else
    comp_result = tfm_memcmp(read_data, READ_DATA, sizeof(read_data));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }

    /* Check that the read data length is zero */
    if (read_data_length != 0) {
        TEST_FAIL("Read data length should be equal to zero");
        return;
    }

    offset = 5;
    read_data_length = 1;

    /* Get zero data from non-zero offset */
    status = psa_its_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                         &read_data_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

#if DOMAIN_NS == 1U
    /* Check that the read data is unchanged */
    comp_result = memcmp(read_data, READ_DATA, sizeof(read_data));
#else
    comp_result = tfm_memcmp(read_data, READ_DATA, sizeof(read_data));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }

    /* Check that the read data length is zero */
    if (read_data_length != 0) {
        TEST_FAIL("Read data length should be equal to zero");
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

void tfm_its_test_common_007(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const size_t data_len = 1;
    const size_t offset = 0;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_length = 0;
    int comp_result;

    /* Get with UID that has not yet been set */
    status = psa_its_get(uid, offset, data_len, read_data + HALF_PADDING_SIZE,
                         &read_data_length);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get succeeded with non-existant UID");
        return;
    }

#if DOMAIN_NS == 1U
    /* Check that the read data is unchanged */
    comp_result = memcmp(read_data, READ_DATA, sizeof(read_data));
#else
    comp_result = tfm_memcmp(read_data, READ_DATA, sizeof(read_data));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read data not equal to original read data");
        return;
    }

    /* Get with invalid UID */
    status = psa_its_get(INVALID_UID, offset, data_len,
                         read_data + HALF_PADDING_SIZE, &read_data_length);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Get succeeded with invalid UID");
        return;
    }

#if DOMAIN_NS == 1U
    /* Check that the read data is unchanged */
    comp_result = memcmp(read_data, READ_DATA, sizeof(read_data));
#else
    comp_result = tfm_memcmp(read_data, READ_DATA, sizeof(read_data));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read data not equal to original read data");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_008(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t write_len = WRITE_DATA_SIZE;
    size_t read_len;
    size_t offset;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_length = 0;
    int comp_result;

    status = psa_its_set(uid, write_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get with offset greater than UID's length */
    read_len = 1;
    offset = write_len + 1;

    status = psa_its_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                         &read_data_length);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Get should not succeed with offset too large");
        return;
    }

#if DOMAIN_NS == 1U
    /* Check that the read data is unchanged */
    comp_result = memcmp(read_data, READ_DATA, sizeof(read_data));
#else
    comp_result = tfm_memcmp(read_data, READ_DATA, sizeof(read_data));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read data should be equal to original read data");
        return;
    }

    /* Get with data length greater than UID's length */
    read_len = write_len + 1;
    offset = 0;

    status = psa_its_get(uid, offset, read_len, read_data + HALF_PADDING_SIZE,
                         &read_data_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should succeed with data length larger than UID's "
                  "length");
        return;
    }

    if (read_data_length != write_len) {
        TEST_FAIL("Read data length should be equal to UID's length");
        return;
    }

#if DOMAIN_NS == 1U
    /* Check that the read data is changed */
    comp_result = memcmp(read_data, RESULT_DATA, sizeof(read_data));
#else
    comp_result = tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read data should be equal to newly read data");
        return;
    }

    /* Get with offset + data length greater than UID's length, but individually
     * valid
     */
#if DOMAIN_NS == 1U
    /* Reset read_data to original READ_DATA */
    memcpy(read_data, READ_DATA, sizeof(read_data));
#else
    tfm_memcpy(read_data, READ_DATA, sizeof(read_data));
#endif
    read_len = write_len;
    offset = 1;

    status = psa_its_get(uid, offset, read_len,
                         read_data + HALF_PADDING_SIZE,
                         &read_data_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should succeed with offset + data length too large, "
                  "but individually valid");
        return;
    }

    if (read_data_length != write_len - offset) {
        TEST_FAIL("Read data length should be equal to the UID's remaining "
                  "size starting from offset");
        return;
    }

#if DOMAIN_NS == 1U
    /* Check that the read data is changed */
    comp_result = memcmp(read_data, OFFSET_RESULT_DATA, sizeof(read_data));
#else
    comp_result = tfm_memcmp(read_data, OFFSET_RESULT_DATA, sizeof(read_data));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read data should be equal to newly read data starting at "
                  "offset");
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

void tfm_its_test_common_009(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t data_len = WRITE_DATA_SIZE;
    const size_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    size_t read_data_length = 0;

    status = psa_its_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get with NULL data pointer */
    status = psa_its_get(uid, offset, 0, NULL, &read_data_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should succeed with NULL data pointer and zero length");
        return;
    }

    /* Check that the read data length is unchanged */
    if (read_data_length != 0) {
        TEST_FAIL("Read data length should be 0 with NULL data pointer and "
                  "zero length");
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

void tfm_its_test_common_010(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = WRITE_ONCE_UID;
    struct psa_storage_info_t info = {0};

    /* Get info for write once UID */
    status = psa_its_get_info(uid, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get info should not fail for write once UID");
        return;
    }

    /* Check that the info struct contains the correct values */
    if (info.size != WRITE_ONCE_DATA_SIZE) {
        TEST_FAIL("Size incorrect for write once UID");
        return;
    }

    if (info.capacity != WRITE_ONCE_DATA_SIZE) {
        TEST_FAIL("Capacity incorrect for write once UID");
        return;
    }

    if (info.flags != PSA_STORAGE_FLAG_WRITE_ONCE) {
        TEST_FAIL("Flags incorrect for write once UID");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_011(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    struct psa_storage_info_t info = {0};
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t data_len = WRITE_DATA_SIZE;
    const uint8_t write_data[] = WRITE_DATA;

    status = psa_its_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get info for valid UID */
    status = psa_its_get_info(uid, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get info should not fail with valid UID");
        return;
    }

    /* Check that the info struct contains the correct values */
    if (info.size != data_len) {
        TEST_FAIL("Size incorrect for valid UID");
        return;
    }

    if (info.capacity != data_len) {
        TEST_FAIL("Capacity incorrect for valid UID");
        return;
    }

    if (info.flags != flags) {
        TEST_FAIL("Flags incorrect for valid UID");
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

void tfm_its_test_common_012(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    struct psa_storage_info_t info = {0};

    /* Get info with UID that has not yet been set */
    status = psa_its_get_info(uid, &info);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get info should not succeed with unset UID");
        return;
    }

    /* Check that the info struct has not been modified */
    if (info.size != 0) {
        TEST_FAIL("Size should not have changed");
        return;
    }

    /* Get info with invalid UID */
    status = psa_its_get_info(INVALID_UID, &info);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Get info should not succeed with invalid UID");
        return;
    }

    /* Check that the info struct has not been modified */
    if (info.size != 0) {
        TEST_FAIL("Size should not have changed");
        return;
    }

    if (info.capacity != 0) {
        TEST_FAIL("Capacity should not have changed");
        return;
    }

    if (info.flags != PSA_STORAGE_FLAG_NONE) {
        TEST_FAIL("Flags should not have changed");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_013(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    struct psa_storage_info_t info = {0};
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t data_len = WRITE_DATA_SIZE;
    const size_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_length = 0;

    status = psa_its_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Call remove with valid ID */
    status = psa_its_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    /* Check that get info fails for removed UID */
    status = psa_its_get_info(uid, &info);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get info should not succeed with removed UID");
        return;
    }

    /* Check that get fails for removed UID */
    status = psa_its_get(uid, offset, data_len, read_data, &read_data_length);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get should not succeed with removed UID");
        return;
    }

    /* Check that remove fails for removed UID */
    status = psa_its_remove(uid);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Remove should not succeed with removed UID");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_014(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = WRITE_ONCE_UID;

    /* Call remove with write once UID */
    status = psa_its_remove(uid);
    if (status != PSA_ERROR_NOT_PERMITTED) {
        TEST_FAIL("Remove should not succeed with write once UID");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_015(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = INVALID_UID;

    /* Call remove with an invalid UID */
    status = psa_its_remove(uid);
    if (status != PSA_ERROR_INVALID_ARGUMENT) {
        TEST_FAIL("Remove should not succeed with invalid UID");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_016(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid_1 = TEST_UID_2;
    const psa_storage_uid_t uid_2 = TEST_UID_3;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t data_len_2 = WRITE_DATA_SIZE;
    const size_t offset = 0;
    const uint8_t write_data_1[] = "UID 1 DATA";
    const uint8_t write_data_2[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_length = 0;
    int comp_result;

    /* Set UID 1 */
    status = psa_its_set(uid_1, sizeof(write_data_1), write_data_1, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail for UID 1");
        return;
    }

    /* Set UID 2 */
    status = psa_its_set(uid_2, data_len_2, write_data_2, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail for UID 2");
        return;
    }

    /* Remove UID 1. This should cause UID 2 to be compacted to the beginning of
     * the block.
     */
    status = psa_its_remove(uid_1);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail for UID 1");
        return;
    }

    /* If the compact worked as expected, the test should be able to read back
     * the data from UID 2 correctly.
     */
    status = psa_its_get(uid_2, offset, data_len_2,
                        read_data + HALF_PADDING_SIZE, &read_data_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail for UID 2");
        return;
    }

#if DOMAIN_NS == 1U
    comp_result = memcmp(read_data, RESULT_DATA, sizeof(read_data));
#else
    comp_result = tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    if (read_data_length != WRITE_DATA_SIZE) {
        TEST_FAIL("Read data length should be equal to result data length");
        return;
    }

    /* Remove UID 2 to clean up storage for the next test */
    status = psa_its_remove(uid_2);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail for UID 2");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_017(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_1;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t data_len = WRITE_DATA_SIZE;
    size_t offset = 0;
    const uint8_t write_data[] = WRITE_DATA;
    uint8_t read_data[] = READ_DATA;
    size_t read_data_length = 0;
    int comp_result;

    /* Set the entire data into UID */
    status = psa_its_set(uid, data_len, write_data, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Get the data from UID one byte at a time */
    for (offset = 0; offset < data_len; ++offset) {
        status = psa_its_get(uid, offset, 1,
                             (read_data + HALF_PADDING_SIZE + offset),
                             &read_data_length);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Get should not fail for partial read");
            return;
        }
    }

#if DOMAIN_NS == 1U
    comp_result = memcmp(read_data, RESULT_DATA, sizeof(read_data));
#else
    comp_result = tfm_memcmp(read_data, RESULT_DATA, sizeof(read_data));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    /* Remove UID to clean up storage for the next test */
    status = psa_its_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_018(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_2;
    const psa_storage_create_flags_t flags = PSA_STORAGE_FLAG_NONE;
    const size_t offset = 0;
    const uint8_t write_data_1[] = "ONE";
    const uint8_t write_data_2[] = "TWO";
    const uint8_t write_data_3[] = "THREE";
    uint8_t read_data[] = READ_DATA;
    size_t read_data_length = 0;
    int comp_result;

    /* Set write data 1 into UID */
    status = psa_its_set(uid, sizeof(write_data_1), write_data_1, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("First set should not fail");
        return;
    }

    /* Set write data 2 into UID */
    status = psa_its_set(uid, sizeof(write_data_2), write_data_2, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Second set should not fail");
        return;
    }

    /* Set write data 3 into UID */
    status = psa_its_set(uid, sizeof(write_data_3), write_data_3, flags);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Third set should not fail");
        return;
    }

    status = psa_its_get(uid, offset, sizeof(write_data_3), read_data,
                         &read_data_length);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Get should not fail");
        return;
    }

#if DOMAIN_NS == 1U
    /* Check that get returns the last data to be set */
    comp_result = memcmp(read_data, write_data_3, sizeof(write_data_3));
#else
    comp_result = tfm_memcmp(read_data, write_data_3, sizeof(write_data_3));
#endif
    if (comp_result != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    if (read_data_length != sizeof(write_data_3)) {
        TEST_FAIL("Read data length should be equal to result data length");
        return;
    }

    /* Remove UID to clean up storage for the next test */
    status = psa_its_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

void tfm_its_test_common_019(struct test_result_t *ret)
{
    uint8_t cycle;
    psa_status_t status;
    int comp_result;
    size_t read_data_length = 0;
    const psa_storage_uid_t test_uid[TEST_019_CYCLES] = {
        TEST_UID_1,
        TEST_UID_2,
        TEST_UID_3};
    const size_t test_asset_sizes[TEST_019_CYCLES] = {
        ITS_MAX_ASSET_SIZE >> 2,
        ITS_MAX_ASSET_SIZE >> 1,
        ITS_MAX_ASSET_SIZE};

    /* Loop to test different asset sizes and UID's*/
    for (cycle = 0; cycle < TEST_019_CYCLES; cycle++) {
        size_t data_size = test_asset_sizes[cycle];
        psa_storage_uid_t uid = test_uid[cycle];
        struct psa_storage_info_t info = {0};

#if DOMAIN_NS == 1U
        memset(read_asset_data, 0x00, sizeof(read_asset_data));
#else
        tfm_memset(read_asset_data, 0x00, sizeof(read_asset_data));
#endif

        /* Set with data and no flags and a valid UID */
        status = psa_its_set(uid,
                             data_size,
                             write_asset_data,
                             PSA_STORAGE_FLAG_NONE);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Set should not fail with valid UID");
            return;
        }

        /* Get info for valid UID */
        status = psa_its_get_info(uid, &info);
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
        status = psa_its_get(uid, 0, data_size, read_asset_data,
                             &read_data_length);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Get should not fail with valid UID");
            return;
        }

#if DOMAIN_NS == 1U
        /* Check that get returns the last data which was set */
        comp_result = memcmp(read_asset_data, write_asset_data, data_size);
#else
        comp_result = tfm_memcmp(read_asset_data, write_asset_data, data_size);
#endif
        if (comp_result != 0) {
            TEST_FAIL("Read data should be equal to original write data");
            return;
        }

        if (read_data_length != data_size) {
            TEST_FAIL("Read data length should be equal to result data length");
            return;
        }

        /* Call remove to clean up storage for the next test */
        status = psa_its_remove(uid);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Remove should not fail with valid UID");
            return;
        }
    }

    ret->val = TEST_PASSED;
}
