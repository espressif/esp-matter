/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test/framework/test_framework_helpers.h"
#include "psa_audit_api.h"
#include "audit_ns_tests.h"
#include "tfm_api.h"
#include "secure_fw/services/audit_logging/audit_core.h"

#include "../audit_tests_common.h"

/*!
 * \def EMPTY_RETRIEVED_LOG_SIZE
 *
 * \brief Log size when the retrieved buffer is empty
 */
#define EMPTY_RETRIEVED_LOG_SIZE (0)

/*!
 * \def EMPTY_RETRIEVED_LOG_ITEMS
 *
 * \brief Number of log items when retrieved buffer is empty
 */
#define EMPTY_RETRIEVED_LOG_ITEMS (0)

/*!
 * \def SINGLE_RETRIEVED_LOG_SIZE
 *
 * \brief Log size when the retrieved buffer has 1 item
 *        of standard size (no payload)
 */
#define SINGLE_RETRIEVED_LOG_SIZE (STANDARD_LOG_ENTRY_SIZE)

/*!
 * \def SINGLE_RETRIEVED_LOG_ITEMS
 *
 * \brief Number of log items when retrieved buffer has 1 item
 */
#define SINGLE_RETRIEVED_LOG_ITEMS (1)

/*!
 * \def SECOND_ELEMENT_START_INDEX
 *
 * \brief Index of the second item in the log
 */
#define SECOND_ELEMENT_START_INDEX (1)

/* List of tests */
static void tfm_audit_test_1001(struct test_result_t *ret);

static struct test_t audit_veneers_tests[] = {
    {&tfm_audit_test_1001, "TFM_AUDIT_TEST_1001",
     "Non Secure functional", {TEST_PASSED} },
};

void register_testsuite_ns_audit_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(audit_veneers_tests) /
                 sizeof(audit_veneers_tests[0]));

    set_testsuite("AuditLog non-secure interface test (TFM_AUDIT_TEST_1XXX)",
                  audit_veneers_tests, list_size, p_test_suite);
}

/**
 * \brief Functional test of NS API
 *
 * \note This is a functional test only and doesn't
 *       mean to test all possible combinations of
 *       input parameters and return values.
 *       This tests the current status of the log as
 *       it's been left from the Secure tests. In case
 *       other tests are added in the Secure test suite,
 *       the status of the log will change and these
 *       tests may start failing.
 */
static void tfm_audit_test_1001(struct test_result_t *ret)
{
    psa_status_t status;

    uint8_t local_buffer[LOCAL_BUFFER_SIZE];
    uint32_t idx, stored_size, num_records, retrieved_size;

    struct psa_audit_record *retrieved_buffer;

    /* Get the log size (current state) */
    status = psa_audit_get_info(&num_records, &stored_size);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Getting log info has returned error");
        return;
    }

    if (stored_size != INITIAL_LOG_SIZE) {
        TEST_FAIL("Stored size different from " STR(INITIAL_LOG_SIZE));
        return;
    }

    if (num_records != INITIAL_LOG_RECORDS) {
        TEST_FAIL("Stored records different from " STR(INITIAL_LOG_RECORDS));
        return;
    }

    /* Check the length of each record individually */
    for (idx=0; idx<num_records; idx++) {
        status = psa_audit_get_record_info(idx, &stored_size);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Getting record size individually has returned error");
            return;
        }

        if (stored_size != STANDARD_LOG_ENTRY_SIZE) {
            TEST_FAIL("Unexpected record size for a single standard record");
            return;
        }
    }

    /* Check that if requesting length of a record which is not there fails */
    status = psa_audit_get_record_info(num_records, &stored_size);
    if (status == PSA_SUCCESS) {
        TEST_FAIL("Getting record size for non-existent record has not failed");
        return;
    }

    /* Log contains 2 items. Retrieve into buffer which is able to contain the
     * the full contents of the log, one record at a time
     */
    for (idx=0; idx<INITIAL_LOG_RECORDS; idx++) {
        status = psa_audit_retrieve_record(
                                     idx,
                                     LOCAL_BUFFER_SIZE,
                                     NULL,
                                     0,
                                     &local_buffer[idx*STANDARD_LOG_ENTRY_SIZE],
                                     &retrieved_size);

        if (status != PSA_SUCCESS) {
            TEST_FAIL("Log retrieval from NS returned error");
            return;
        }

        if (retrieved_size != STANDARD_LOG_ENTRY_SIZE) {
            TEST_FAIL("Expected retrieve size: " STR(STANDARD_LOG_ENTRY_SIZE));
            return;
        }
    }

    /* Retrieve into a small buffer. It's not enough to store a single
     * item so the provided buffer must be empty after retrieval. We
     * check the info structure to count how many items and bytes have
     * been returned, and if they're zeros items / zero bytes, there is
     * no point in checking the contents of the local_buffer.
     */
    status = psa_audit_retrieve_record(0,
                                       LOCAL_BUFFER_SIZE/4,
                                       NULL,
                                       0,
                                       &local_buffer[0],
                                       &retrieved_size);

    if (status == PSA_SUCCESS) {
        TEST_FAIL("Log retrieval from NS should fail, buffer too small");
        return;
    }

    if (retrieved_size != EMPTY_RETRIEVED_LOG_SIZE) {
        TEST_FAIL("Expected log size is " STR(EMPTY_RETRIEVED_LOG_SIZE));
        return;
    }

    /* Retrieve into a buffer which can hold a single element, but start from
     * the second element that is stored in the log
     */
    status = psa_audit_retrieve_record(1,
                                       STANDARD_LOG_ENTRY_SIZE,
                                       NULL,
                                       0,
                                       &local_buffer[0],
                                       &retrieved_size);

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Log retrieval from NS returned error");
        return;
    }

    if (retrieved_size != SINGLE_RETRIEVED_LOG_SIZE) {
        TEST_FAIL("Expected log size is " STR(SINGLE_RETRIEVED_LOG_SIZE));
        return;
    }

    /* Inspect the contents of the retrieved buffer, i.e. check the
     * retrieved log record contents
     */
    retrieved_buffer = (struct psa_audit_record *)
                           &local_buffer[offsetof(struct log_hdr, size)];

    if (retrieved_buffer->id != SECOND_ELEMENT_EXPECTED_CONTENT) {
        TEST_FAIL("Unexpected argument in the first entry");
        return;
    }

    /* Delete oldest element in the log */
    status = psa_audit_delete_record(0, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Log record deletion from NS returned error");
        return;
    }

    /* Get the log size (current state) */
    status = psa_audit_get_info(&num_records, &stored_size);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Getting log info has returned error");
        return;
    }

    if (num_records != 1) {
        TEST_FAIL("Unexpected number of records in the log after delete");
        return;
    }

    if (stored_size != STANDARD_LOG_ENTRY_SIZE) {
        TEST_FAIL("Unexpected size in the log after deletion");
        return;
    }

    /* Delete oldest element in the log. After this, the log will be empty */
    status = psa_audit_delete_record(0, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Log record deletion from NS returned error");
        return;
    }

    /* Get the log size (current state) */
    status = psa_audit_get_info(&num_records, &stored_size);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Getting log info has returned error");
        return;
    }

    if (num_records != 0) {
        TEST_FAIL("Unexpected number of records in the log after deletion");
        return;
    }

    if (stored_size != 0) {
        TEST_FAIL("Unexpected size in the log after deletion");
        return;
    }

    ret->val = TEST_PASSED;
}
