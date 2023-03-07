/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test/framework/test_framework_helpers.h"
#include "psa_audit_api.h"
#include "audit_s_tests.h"
#include "tfm_api.h"
#include "psa_audit_api.h"
#include "secure_fw/services/audit_logging/audit_core.h"

#include "../audit_tests_common.h"

/*!
 * \def BASE_RETRIEVAL_LOG_INDEX
 *
 * \brief Base index from where to start elements retrieval
 */
#define BASE_RETRIEVAL_LOG_INDEX (6)

/*!
 * \def FIRST_RETRIEVAL_LOG_INDEX
 *
 * \brief Index of the first element in the log
 */
#define FIRST_RETRIEVAL_LOG_INDEX (0)

/* List of tests */
static void tfm_audit_test_1001(struct test_result_t *ret);

static struct test_t audit_veneers_tests[] = {
    {&tfm_audit_test_1001, "TFM_AUDIT_TEST_1001",
     "Secure functional", {TEST_PASSED} },
};

void register_testsuite_s_audit_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(audit_veneers_tests) /
                 sizeof(audit_veneers_tests[0]));

    set_testsuite("Audit Logging secure interface test (TFM_AUDIT_TEST_1XXX)",
                  audit_veneers_tests, list_size, p_test_suite);
}

/**
 * \brief Functional test of the Secure interface
 *
 * \note This is a functional test only and doesn't
 *       mean to test all possible combinations of
 *       input parameters and return values.
 *       This tests will leave the log in a certain
 *       status which, in turn, will be evaluated by
 *       the Non Secure functional tests. If any tests
 *       are added here that will leave the log in a
 *       different state, Non Secure functional tests
 *       need to be amended accordingly.
 */
static void tfm_audit_test_1001(struct test_result_t *ret)
{
    psa_status_t status;
    uint8_t local_buffer[LOCAL_BUFFER_SIZE], idx;
    struct psa_audit_record *record = (struct psa_audit_record *)
                                                  &local_buffer[0];
    uint32_t num_records, stored_size, record_size;
    struct psa_audit_record *retrieved_buffer;

    /* Fill the log with 36 records, each record is 28 bytes
     * we end up filling the log without wrapping
     */
    for (idx=0; idx<INITIAL_LOGGING_REQUESTS; idx++) {
        record->size = sizeof(struct psa_audit_record) - 4;
        record->id = DUMMY_TEST_RECORD_ID_BASE + idx;

        /* The record doesn't contain any payload */
        status = psa_audit_add_record(record);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Record addition has returned an error");
            return;
        }
    }

    /* Get the log size */
    status = psa_audit_get_info(&num_records, &stored_size);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Getting log info has returned error");
        return;
    }

    if (stored_size != INITIAL_LOGGING_SIZE) {
        TEST_FAIL("Expected log size is " STR(INITIAL_LOGGING_SIZE));
        return;
    }

    if (num_records != INITIAL_LOGGING_REQUESTS) {
        TEST_FAIL("Expected log records are " STR(INITIAL_LOGGING_REQUESTS));
        return;
    }

    /* Retrieve two log records starting from a given index */
    for (idx=BASE_RETRIEVAL_LOG_INDEX; idx<BASE_RETRIEVAL_LOG_INDEX+2; idx++) {
        uint8_t *p_buf =
          &local_buffer[(idx-BASE_RETRIEVAL_LOG_INDEX)*STANDARD_LOG_ENTRY_SIZE];

        status = psa_audit_retrieve_record(idx,
                                           LOCAL_BUFFER_SIZE,
                                           NULL,
                                           0,
                                           p_buf,
                                           &record_size);

        if (status != PSA_SUCCESS) {
            TEST_FAIL("Retrieve indexes 6 or 7 has returned an error");
            return;
        }

        if (record_size != STANDARD_LOG_ENTRY_SIZE) {
            TEST_FAIL("Expected log size is " STR(STANDARD_LOG_ENTRY_SIZE));
            return;
        }
    }

    /* Inspect the content of the second log record retrieved */
    retrieved_buffer = (struct psa_audit_record *)
        &local_buffer[offsetof(struct log_hdr,size)+STANDARD_LOG_ENTRY_SIZE];

    if (retrieved_buffer->id != ( DUMMY_TEST_RECORD_ID_BASE +
                                  (BASE_RETRIEVAL_LOG_INDEX+1) )) {
        TEST_FAIL("Unexpected argument in the index 7 entry");
        return;
    }

    /* Retrieve the last two log records */
    for (idx=num_records-2; idx<num_records; idx++) {
        uint8_t *p_buf =
            &local_buffer[(idx-(num_records-2))*STANDARD_LOG_ENTRY_SIZE];

        status = psa_audit_retrieve_record(idx,
                                           LOCAL_BUFFER_SIZE,
                                           NULL,
                                           0,
                                           p_buf,
                                           &record_size);

        if (status != PSA_SUCCESS) {
            TEST_FAIL("Retrieve of last two log records has returned error");
            return;
        }

        if (record_size != STANDARD_LOG_ENTRY_SIZE) {
            TEST_FAIL("Expected log size is " STR(STANDARD_LOG_ENTRY_SIZE));
            return;
        }
    }

    /* Inspect the first record retrieved in the local buffer */
    retrieved_buffer = (struct psa_audit_record *)
                           &local_buffer[offsetof(struct log_hdr,size)];

    if (retrieved_buffer->id != ( DUMMY_TEST_RECORD_ID_BASE +
                                  (INITIAL_LOGGING_REQUESTS-2) )) {
        TEST_FAIL("Unexpected argument in the second last entry");
        return;
    }

    /* Retrieve the first log item */
    status = psa_audit_retrieve_record(0,
                                       LOCAL_BUFFER_SIZE,
                                       NULL,
                                       0,
                                       &local_buffer[0],
                                       &record_size);

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Retrieve of the first log entry has returned error");
        return;
    }

    if (record_size != STANDARD_LOG_ENTRY_SIZE) {
        TEST_FAIL("Expected log size is " STR(STANDARD_LOG_ENTRY_SIZE));
        return;
    }

    if (retrieved_buffer->id != DUMMY_TEST_RECORD_ID_BASE) {
        TEST_FAIL("Unexpected argument in the first entry");
        return;
    }

    status = psa_audit_retrieve_record(num_records - 1,
                                       LOCAL_BUFFER_SIZE,
                                       NULL,
                                       0,
                                       &local_buffer[0],
                                       &record_size);

    if (status != PSA_SUCCESS) {
        TEST_FAIL("Retrieve of last two log entries has returned error");
        return;
    }

    if (record_size != STANDARD_LOG_ENTRY_SIZE) {
        TEST_FAIL("Expected log size is " STR(STANDARD_LOG_ENTRY_SIZE));
        return;
    }

    /* Inspect the item just retrieved */
    if (retrieved_buffer->id != ( DUMMY_TEST_RECORD_ID_BASE +
                                  (INITIAL_LOGGING_REQUESTS-1) )) {
        TEST_FAIL("Unexpected argument in the second last entry");
        return;
    }

    /* Fill one more log record, this will wrap */
    record->size = sizeof(struct psa_audit_record) - 4;
    record->id = DUMMY_TEST_RECORD_ID_BASE + INITIAL_LOGGING_REQUESTS;

    /* The addition of this new log item will wrap the log ending */
    status = psa_audit_add_record(record);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Record addition has returned an error");
        return;
    }

    /* Get the log size */
    status = psa_audit_get_info(&num_records, &stored_size);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Getting log info has returned error");
        return;
    }

    /* Check that the log state is the same, the item addition just performed
     * is resulted into the removal of the oldest entry, so log size and number
     * of log records is still the same as before
     */
    if (stored_size != INITIAL_LOGGING_SIZE) {
        TEST_FAIL("Expected log size is " STR(INITIAL_LOGGING_SIZE));
        return;
    }

    if (num_records != INITIAL_LOGGING_REQUESTS) {
        TEST_FAIL("Expected log records are " STR(INITIAL_LOGGING_REQUESTS));
        return;
    }

    /* Retrieve the last two log records */
    for (idx=num_records-2; idx<num_records; idx++) {
        uint8_t *p_buf =
            &local_buffer[(idx-(num_records-2))*STANDARD_LOG_ENTRY_SIZE];

        /* Retrieve the last two items */
        status = psa_audit_retrieve_record(idx,
                                           LOCAL_BUFFER_SIZE,
                                           NULL,
                                           0,
                                           p_buf,
                                           &record_size);

        if (status != PSA_SUCCESS) {
            TEST_FAIL("Retrieve of last two log records has returned error");
            return;
        }

        if (record_size != STANDARD_LOG_ENTRY_SIZE) {
            TEST_FAIL("Expected record size is " STR(STANDARD_LOG_ENTRY_SIZE));
            return;
        }
    }

    /* Inspect the first record retrieved */
    if (retrieved_buffer->id != ( DUMMY_TEST_RECORD_ID_BASE +
                                  (INITIAL_LOGGING_REQUESTS-1) )) {
        TEST_FAIL("Unexpected argument in the second last entry");
        return;
    }

    /* Inspect the second record retrieved in the local buffer */
    retrieved_buffer = (struct psa_audit_record *)
        &local_buffer[offsetof(struct log_hdr,size)+STANDARD_LOG_ENTRY_SIZE];

    if (retrieved_buffer->id != ( DUMMY_TEST_RECORD_ID_BASE +
                                  (INITIAL_LOGGING_REQUESTS) )) {
        TEST_FAIL("Unexpected argument in the last entry");
        return;
    }

    /* Fill now one big record that will invalidate all existing records */
    record->size = MAX_LOG_RECORD_SIZE;
    record->id = DUMMY_TEST_RECORD_ID_BASE + INITIAL_LOGGING_REQUESTS + 1;

    /* The record has maximum possible payload for log size of 1024 */
    status = psa_audit_add_record(record);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Record addition has returned an error");
        return;
    }

    /* Get the log size */
    status = psa_audit_get_info(&num_records, &stored_size);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Getting log info has returned error");
        return;
    }

    /* Check that the log state has one element with maximum size */
    if (stored_size != MAX_LOG_SIZE) {
        TEST_FAIL("Expected log size is " STR(MAX_LOG_SIZE));
        return;
    }

    if (num_records != 1) {
        TEST_FAIL("Expected log records are 1");
        return;
    }

    /* Try to retrieve the maximum possible size that fits our buffer.
     * As there is just one big record filling the whole space, nothing
     * will be returned and the API will fail
     */
    status = psa_audit_retrieve_record(0,
                                       LOCAL_BUFFER_SIZE,
                                       NULL,
                                       0,
                                       &local_buffer[0],
                                       &record_size);

    if (status == PSA_SUCCESS) {
        TEST_FAIL("Retrieve of index 0 should fail as it's too big");
        return;
    }

    if (record_size != 0) {
        TEST_FAIL("Retrieved log size has unexpected size instead of 0");
        return;
    }

    /* Add two standard length records again */
    for (idx=0; idx<2; idx++) {
        record->size = sizeof(struct psa_audit_record) - 4;
        record->id = DUMMY_TEST_RECORD_ID_BASE +
                     INITIAL_LOGGING_REQUESTS + 2 + idx;

        /* The record doesn't contain any payload */
        status = psa_audit_add_record(record);
        if (status != PSA_SUCCESS) {
            TEST_FAIL("Record addition has returned an error");
            return;
        }
    }

    /* Get the log size */
    status = psa_audit_get_info(&num_records, &stored_size);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Getting log info has returned error");
        return;
    }

    /* As the log was full, the addition of the last two log records results
     * in the resetting of the log completely. The log will contain only
     * the last two items we have just added.
     */
    if (stored_size != FINAL_LOGGING_SIZE) {
        TEST_FAIL("Expected log size is " STR(FINAL_LOGGING_SIZE));
        return;
    }

    if (num_records != FINAL_LOGGING_REQUESTS) {
        TEST_FAIL("Expected log records are " STR(FINAL_LOGGING_REQUESTS));
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
            TEST_FAIL("Unexpected log record size for a single standard item");
            return;
        }
    }

    /* Check that if requesting length of a record which is not there fails */
    status = psa_audit_get_record_info(num_records, &stored_size);
    if (status == PSA_SUCCESS) {
        TEST_FAIL("Getting record size for non-existent record has not failed");
        return;
    }

    ret->val = TEST_PASSED;
}
