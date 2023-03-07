/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>

#include "core_ns_tests.h"
#include "tfm_api.h"
#include "tfm_plat_test.h"
#include "test/suites/core/non_secure/core_test_api.h"
#include "test/test_services/tfm_core_test/core_test_defs.h"
#ifdef TFM_PSA_API
#include "psa_manifest/sid.h"
#else  /* TFM_PSA_API */
#include "tfm_veneers.h"
#endif /* TFM_PSA_API */
#ifdef TFM_ENABLE_IRQ_TEST
#include "mt7933.h"
#include "tfm_peripherals_def.h"
#endif

/* Define test suite for core tests */
/* List of tests */

#define TOSTRING(x) #x
#define CORE_TEST_DESCRIPTION(number, fn, description) \
    {fn, "TFM_CORE_TEST_"TOSTRING(number),\
     description, {TEST_PASSED} }

#ifndef TFM_PSA_API
static void tfm_core_test_get_caller_client_id(struct test_result_t *ret);
static void tfm_core_test_spm_request(struct test_result_t *ret);
#endif /* TFM_PSA_API */
static void tfm_core_test_ns_thread(struct test_result_t *ret);
static void tfm_core_test_check_init(struct test_result_t *ret);
#ifdef ENABLE_TFM_CORE_RECURSION_TESTS
static void tfm_core_test_recursion(struct test_result_t *ret);
#endif
static void tfm_core_test_buffer_check(struct test_result_t *ret);
static void tfm_core_test_ss_to_ss(struct test_result_t *ret);
static void tfm_core_test_ss_to_ss_buffer(struct test_result_t *ret);
#ifdef TFM_ENABLE_PERIPH_ACCESS_TEST
static void tfm_core_test_peripheral_access(struct test_result_t *ret);
#endif
static void tfm_core_test_iovec_sanitization(struct test_result_t *ret);
static void tfm_core_test_outvec_write(struct test_result_t *ret);
#ifdef TFM_ENABLE_IRQ_TEST
static void tfm_core_test_irq(struct test_result_t *ret);

static enum irq_test_scenario_t executing_irq_test_scenario = IRQ_TEST_SCENARIO_NONE;
static struct irq_test_execution_data_t irq_test_execution_data = {0};
#endif

static struct test_t core_tests[] = {
CORE_TEST_DESCRIPTION(CORE_TEST_ID_NS_THREAD, tfm_core_test_ns_thread,
    "Test service request from NS thread mode"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_CHECK_INIT, tfm_core_test_check_init,
    "Test the success of service init"),
#ifdef ENABLE_TFM_CORE_RECURSION_TESTS
CORE_TEST_DESCRIPTION(CORE_TEST_ID_RECURSION, tfm_core_test_recursion,
    "Test direct recursion of secure services"),
#endif
#ifdef TFM_ENABLE_IRQ_TEST
CORE_TEST_DESCRIPTION(CORE_TEST_ID_SECURE_IRQ,
    tfm_core_test_irq,
    "Test secure irq"),
#endif
CORE_TEST_DESCRIPTION(CORE_TEST_ID_BUFFER_CHECK, tfm_core_test_buffer_check,
    "Test secure service buffer accesses"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_SS_TO_SS, tfm_core_test_ss_to_ss,
    "Test secure service to service call"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_SS_TO_SS_BUFFER,
    tfm_core_test_ss_to_ss_buffer,
    "Test secure service to service call with buffer handling"),
#ifdef TFM_ENABLE_PERIPH_ACCESS_TEST
CORE_TEST_DESCRIPTION(CORE_TEST_ID_PERIPHERAL_ACCESS,
    tfm_core_test_peripheral_access,
    "Test service peripheral access"),
#endif
#ifndef TFM_PSA_API
CORE_TEST_DESCRIPTION(CORE_TEST_ID_GET_CALLER_CLIENT_ID,
    tfm_core_test_get_caller_client_id,
    "Test get caller client ID function"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_SPM_REQUEST,
    tfm_core_test_spm_request,
    "Test SPM request function"),
#endif /* TFM_PSA_API */
CORE_TEST_DESCRIPTION(CORE_TEST_ID_IOVEC_SANITIZATION,
    tfm_core_test_iovec_sanitization,
    "Test service parameter sanitization"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_OUTVEC_WRITE,
    tfm_core_test_outvec_write,
    "Test outvec write"),
};

void register_testsuite_ns_core_positive(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(core_tests) / sizeof(core_tests[0]));

    set_testsuite("Core non-secure positive tests (TFM_CORE_TEST_1XXX)",
                  core_tests, list_size, p_test_suite);
}

#ifdef TFM_PSA_API
static psa_status_t psa_test_common(uint32_t sid, uint32_t version,
                                    const psa_invec *in_vecs, size_t in_len,
                                    psa_outvec *out_vecs, size_t out_len)
{
    psa_handle_t handle;
    psa_status_t status;

    handle = psa_connect(sid, version);
    if (handle <= 0) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    status = psa_call(handle, PSA_IPC_CALL, in_vecs, in_len, out_vecs, out_len);
    if (status < 0) {
        status = CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR;
    }

    psa_close(handle);
    return status;
}
#endif /* TFM_PSA_API */

static void tfm_core_test_ns_thread(struct test_result_t *ret)
{
    int32_t err;
#ifndef TFM_PSA_API
    int32_t test_case_id = CORE_TEST_ID_NS_THREAD;
    psa_invec in_vec[] = { {&test_case_id, sizeof(int32_t)} };
    struct tfm_core_test_call_args_t args = {in_vec, 1, NULL, 0};

    err = tfm_core_test_call(tfm_spm_core_test_sfn_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_NS_THREAD_SID,
                          SPM_CORE_TEST_NS_THREAD_VERSION,
                          NULL, 0, NULL, 0);
#endif /* TFM_PSA_API */

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("Secure function call from thread mode should be successful");
        return;
    }

    ret->val = TEST_PASSED;
}

#ifdef TFM_ENABLE_PERIPH_ACCESS_TEST
static void tfm_core_test_peripheral_access(struct test_result_t *ret)
{
    int32_t err;

#ifndef TFM_PSA_API
    int32_t test_case_id = CORE_TEST_ID_PERIPHERAL_ACCESS;
    psa_invec in_vec[] = { {&test_case_id, sizeof(int32_t)} };
    struct tfm_core_test_call_args_t args = {in_vec, 1, NULL, 0};

    err = tfm_core_test_call(tfm_spm_core_test_sfn_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_PERIPHERAL_ACCESS_SID,
                          SPM_CORE_TEST_PERIPHERAL_ACCESS_VERSION,
                          NULL, 0, NULL, 0);
#endif /* TFM_PSA_API */

    switch (err) {
    case CORE_TEST_ERRNO_SUCCESS:
        ret->val = TEST_PASSED;
        return;
    case CORE_TEST_ERRNO_PERIPHERAL_ACCESS_FAILED:
        TEST_FAIL("Service peripheral access failed.");
        return;
    default:
        TEST_FAIL("Unexpected return value received.");
        return;
    }
}
#endif

static void empty_iovecs(psa_invec invec[], psa_outvec outvec[])
{
    int i = 0;

    for (i = 0; i < PSA_MAX_IOVEC; ++i) {
        invec[i].len = 0;
        invec[i].base = NULL;
        outvec[i].len = 0;
        outvec[i].base = NULL;
    }
}

static void full_iovecs(psa_invec invec[], psa_outvec outvec[])
{
    int i = 0;

    for (i = 0; i < PSA_MAX_IOVEC; ++i) {
        invec[i].len = sizeof(psa_invec);
        invec[i].base = invec + i;
        outvec[i].len = PSA_MAX_IOVEC * sizeof(psa_outvec);
        outvec[i].base = outvec;
    }
}

static void tfm_core_test_iovec_sanitization(struct test_result_t *ret)
{
    int32_t err;
    psa_invec in_vec[PSA_MAX_IOVEC] = {
                                   {NULL, 0}, {NULL, 0}, {NULL, 0}, {NULL, 0} };
    psa_outvec out_vec[PSA_MAX_IOVEC] = {
                                   {NULL, 0}, {NULL, 0}, {NULL, 0}, {NULL, 0} };

#ifndef TFM_PSA_API
    struct tfm_core_test_call_args_t args = {NULL, 0, NULL, 0};
#endif /* TFM_PSA_API */

    /* Check a few valid cases */

    /* Execute a call with valid iovecs (empty) */
    empty_iovecs(in_vec, out_vec);

#ifndef TFM_PSA_API
    args.in_vec = NULL;
    args.in_len = 0;
    args.out_vec = NULL;
    args.out_len = 0;
    err = tfm_core_test_call(tfm_spm_core_test_2_slave_service_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_2_SLAVE_SERVICE_SID,
                          SPM_CORE_TEST_2_SLAVE_SERVICE_VERSION,
                          NULL, 0, NULL, 0);
#endif /* TFM_PSA_API */
    if (err != CORE_TEST_ERRNO_SUCCESS_2) {
        TEST_FAIL("iovec sanitization failed on empty vectors.");
        return;
    }

    /* Execute a call with valid iovecs (full) */
    full_iovecs(in_vec, out_vec);
#ifndef TFM_PSA_API
    args.in_vec = in_vec;
    args.in_len = 2;
    args.out_vec = out_vec;
    args.out_len = PSA_MAX_IOVEC - args.in_len;
    err = tfm_core_test_call(tfm_spm_core_test_2_slave_service_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_2_SLAVE_SERVICE_SID,
                          SPM_CORE_TEST_2_SLAVE_SERVICE_VERSION,
                          in_vec, 2, out_vec, 2);
#endif /* TFM_PSA_API */
    if (err != CORE_TEST_ERRNO_SUCCESS_2) {
        TEST_FAIL("iovec sanitization failed on full vectors.");
        return;
    }

    /* Execute a call with valid iovecs (different number of vectors) */
    full_iovecs(in_vec, out_vec);
#ifndef TFM_PSA_API
    args.in_vec = in_vec;
    args.in_len = 2;
    args.out_vec = out_vec;
    args.out_len = 1;
    err = tfm_core_test_call(tfm_spm_core_test_2_slave_service_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_2_SLAVE_SERVICE_SID,
                          SPM_CORE_TEST_2_SLAVE_SERVICE_VERSION,
                          in_vec, 2, out_vec, 1);
#endif /* TFM_PSA_API */
    if (err != CORE_TEST_ERRNO_SUCCESS_2) {
        TEST_FAIL(
                 "iovec sanitization failed on valid, partially full vectors.");
        return;
    }

    /* Check some further valid cases to be sure that checks happen only iovecs
     * that specified valid by the parameters
     */

    /* Execute a call with base = 0 in single vector in outvec that is out of
     * range
     */
    full_iovecs(in_vec, out_vec);
    out_vec[1].base = NULL;
#ifndef TFM_PSA_API
    args.in_vec = in_vec;
    args.in_len = 2;
    args.out_vec = out_vec;
    args.out_len = 1;
    err = tfm_core_test_call(tfm_spm_core_test_2_slave_service_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_2_SLAVE_SERVICE_SID,
                          SPM_CORE_TEST_2_SLAVE_SERVICE_VERSION,
                          in_vec, 2, out_vec, 1);
#endif /* TFM_PSA_API */
    if (err != CORE_TEST_ERRNO_SUCCESS_2) {
        TEST_FAIL("content of an outvec out of range should not be checked");
        return;
    }

    /* Execute a call with len = 0 in single vector in invec that is out of
     * range
     */
    full_iovecs(in_vec, out_vec);
    in_vec[2].len = 0;
#ifndef TFM_PSA_API
    args.in_vec = in_vec;
    args.in_len = 2;
    args.out_vec = out_vec;
    args.out_len = 1;
    err = tfm_core_test_call(tfm_spm_core_test_2_slave_service_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_2_SLAVE_SERVICE_SID,
                          SPM_CORE_TEST_2_SLAVE_SERVICE_VERSION,
                          in_vec, 2, out_vec, 1);
#endif /* TFM_PSA_API */
    if (err != CORE_TEST_ERRNO_SUCCESS_2) {
        TEST_FAIL("content of an outvec out of range should not be checked");
        return;
    }

    /* Execute a call with len = 0 in single vector in invec */
    full_iovecs(in_vec, out_vec);
    in_vec[1].len = 0;
    in_vec[1].base = NULL;
#ifndef TFM_PSA_API
    args.in_vec = in_vec;
    args.in_len = 2;
    args.out_vec = out_vec;
    args.out_len = 2;
    err = tfm_core_test_call(tfm_spm_core_test_2_slave_service_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_2_SLAVE_SERVICE_SID,
                          SPM_CORE_TEST_2_SLAVE_SERVICE_VERSION,
                          in_vec, 2, out_vec, 2);
#endif /* TFM_PSA_API */
    if (err != CORE_TEST_ERRNO_SUCCESS_2) {
        TEST_FAIL("If the len of an invec is 0, the base should be ignored");
        return;
    }

    /* Execute a call with len = 0 in single vector in outvec */
    full_iovecs(in_vec, out_vec);
    out_vec[1].len = 0;
    out_vec[1].base = NULL;
#ifndef TFM_PSA_API
    args.in_vec = in_vec;
    args.in_len = 2;
    args.out_vec = out_vec;
    args.out_len = 2;
    err = tfm_core_test_call(tfm_spm_core_test_2_slave_service_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_2_SLAVE_SERVICE_SID,
                          SPM_CORE_TEST_2_SLAVE_SERVICE_VERSION,
                          in_vec, 2, out_vec, 2);
#endif /* TFM_PSA_API */
    if (err != CORE_TEST_ERRNO_SUCCESS_2) {
        TEST_FAIL("If the len of an outvec is 0, the base should be ignored");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_core_test_outvec_write(struct test_result_t *ret)
{
    int32_t err;
    int i;
    uint8_t in_buf_0[] = {0, 1, 2, 3, 4};
    uint8_t in_buf_1[] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89};
    uint8_t out_buf_0[sizeof(in_buf_0)];
    uint8_t out_buf_1[sizeof(in_buf_1)];

    psa_invec in_vec[PSA_MAX_IOVEC] = { {in_buf_0, sizeof(in_buf_0)},
                                        {in_buf_1, sizeof(in_buf_1)} };
    psa_outvec out_vec[PSA_MAX_IOVEC] = { {out_buf_0, sizeof(out_buf_0) },
                                        {out_buf_1, sizeof(out_buf_1)} };
#ifndef TFM_PSA_API
    int32_t test_case_id = CORE_TEST_ID_OUTVEC_WRITE;
    struct tfm_core_test_call_args_t args1 = {in_vec, 2, out_vec, 2};
    struct tfm_core_test_call_args_t args2 = {in_vec, 1, NULL, 0};

    err = tfm_core_test_call(tfm_spm_core_test_2_get_every_second_byte_veneer,
                                                                        &args1);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_2_GET_EVERY_SECOND_BYTE_SID,
                          SPM_CORE_TEST_2_GET_EVERY_SECOND_BYTE_VERSION,
                          in_vec, 2, out_vec, 2);
#endif /* TFM_PSA_API */

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("call to secure function should be successful");
        return;
    }

    if (out_vec[0].len != sizeof(in_buf_0)/2 ||
        out_vec[1].len != sizeof(in_buf_1)/2) {
        TEST_FAIL("Number of elements in outvec is not set properly");
        return;
    }
    for (i = 1; i < sizeof(in_buf_0); i += 2) {
        if (((uint8_t *)out_vec[0].base)[i/2] != in_buf_0[i]) {
            TEST_FAIL("result is not correct");
            return;
        }
    }
    for (i = 1; i < sizeof(in_buf_1); i += 2) {
        if (((uint8_t *)out_vec[1].base)[i/2] != in_buf_1[i]) {
            TEST_FAIL("result is not correct");
            return;
        }
    }

    /* do the same test on the secure side */
#ifndef TFM_PSA_API
    in_vec[0].base = &test_case_id;
    in_vec[0].len = sizeof(int32_t);
    err = tfm_core_test_call(tfm_spm_core_test_sfn_veneer, &args2);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_OUTVEC_WRITE_SID,
                          SPM_CORE_TEST_OUTVEC_WRITE_VERSION,
                          in_vec, 0, out_vec, 0);
#endif /* TFM_PSA_API */

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("Failed to execute secure side test");
        return;
    }

    ret->val = TEST_PASSED;
}

#ifdef TFM_ENABLE_IRQ_TEST
static int32_t prepare_test_scenario_ns(
                               enum irq_test_scenario_t test_scenario,
                               struct irq_test_execution_data_t *execution_data)
{
    executing_irq_test_scenario = test_scenario;
    switch (test_scenario) {
    case IRQ_TEST_SCENARIO_NONE:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    case IRQ_TEST_SCENARIO_1:
    case IRQ_TEST_SCENARIO_2:
    case IRQ_TEST_SCENARIO_3:
    case IRQ_TEST_SCENARIO_4:
        /* nothing to be done here */
        break;
    case IRQ_TEST_SCENARIO_5:
        execution_data->timer1_triggered = 0;
        tfm_plat_test_non_secure_timer_start();
        break;
    default:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    return CORE_TEST_ERRNO_SUCCESS;
}

static int32_t execute_test_scenario_ns(
                               enum irq_test_scenario_t test_scenario,
                               struct irq_test_execution_data_t *execution_data)
{

    switch (test_scenario) {
    case IRQ_TEST_SCENARIO_NONE:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    case IRQ_TEST_SCENARIO_1:
        if (execution_data->timer0_triggered) {
            return CORE_TEST_ERRNO_TEST_FAULT;
        }
        while (!execution_data->timer0_triggered) {
            ;
        }
        break;
    case IRQ_TEST_SCENARIO_2:
    case IRQ_TEST_SCENARIO_3:
    case IRQ_TEST_SCENARIO_4:
    case IRQ_TEST_SCENARIO_5:
        /* nothing to be done here */
        break;
    default:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    return CORE_TEST_ERRNO_SUCCESS;
}

void TIMER1_Handler (void)
{
    tfm_plat_test_non_secure_timer_stop();

    switch (executing_irq_test_scenario) {
    case IRQ_TEST_SCENARIO_NONE:
    case IRQ_TEST_SCENARIO_1:
    case IRQ_TEST_SCENARIO_2:
    case IRQ_TEST_SCENARIO_3:
    case IRQ_TEST_SCENARIO_4:
        while (1) {}
        /* shouldn't happen */
        break;
    case IRQ_TEST_SCENARIO_5:
        irq_test_execution_data.timer1_triggered = 1;
        break;
    default:
        while (1) {}
        /* shouldn't happen */
        break;
    }
}

static int32_t tfm_core_test_irq_scenario(
                                         enum irq_test_scenario_t test_scenario)
{
    struct irq_test_execution_data_t *execution_data_address = &irq_test_execution_data;
    uint32_t scenario = test_scenario;

    psa_invec in_vec[] = {
                 {&scenario, sizeof(uint32_t)},
                 {&execution_data_address,
                                  sizeof(struct irq_test_execution_data_t *)} };
    int32_t err;
    struct tfm_core_test_call_args_t args = {in_vec, 2, NULL, 0};

#ifdef TFM_PSA_API
    err = psa_test_common(SPM_CORE_IRQ_TEST_1_PREPARE_TEST_SCENARIO_SID,
                          SPM_CORE_IRQ_TEST_1_PREPARE_TEST_SCENARIO_VERSION,
                          in_vec, 2, NULL, 0);
#else
    err = tfm_core_test_call(tfm_spm_irq_test_1_prepare_test_scenario_veneer, &args);
#endif
    if (err != CORE_TEST_ERRNO_SUCCESS) {
        return err;
    }

#ifdef TFM_PSA_API
    err = psa_test_common(SPM_CORE_TEST_2_PREPARE_TEST_SCENARIO_SID,
                          SPM_CORE_TEST_2_PREPARE_TEST_SCENARIO_VERSION,
                          in_vec, 2, NULL, 0);
#else
    err = tfm_core_test_call(tfm_spm_core_test_2_prepare_test_scenario_veneer, &args);
#endif
    if (err != CORE_TEST_ERRNO_SUCCESS) {
        return err;
    }

    err = prepare_test_scenario_ns(test_scenario, &irq_test_execution_data);
    if (err != CORE_TEST_ERRNO_SUCCESS) {
        return err;
    }

#ifdef TFM_PSA_API
    err = psa_test_common(SPM_CORE_IRQ_TEST_1_EXECUTE_TEST_SCENARIO_SID,
                          SPM_CORE_IRQ_TEST_1_EXECUTE_TEST_SCENARIO_VERSION,
                          in_vec, 2, NULL, 0);
#else
    args.in_len = 1;
    err = tfm_core_test_call(tfm_spm_irq_test_1_execute_test_scenario_veneer, &args);
#endif
    if (err != CORE_TEST_ERRNO_SUCCESS) {
        return err;
    }

#ifdef TFM_PSA_API
    err = psa_test_common(SPM_CORE_TEST_2_EXECUTE_TEST_SCENARIO_SID,
                          SPM_CORE_TEST_2_EXECUTE_TEST_SCENARIO_VERSION,
                          in_vec, 2, NULL, 0);
#else
    err = tfm_core_test_call(tfm_spm_core_test_2_execute_test_scenario_veneer, &args);
#endif
    if (err != CORE_TEST_ERRNO_SUCCESS) {
        return err;
    }

    err = execute_test_scenario_ns(test_scenario, &irq_test_execution_data);
    if (err != CORE_TEST_ERRNO_SUCCESS) {
        return err;
    }

    return CORE_TEST_ERRNO_SUCCESS;
}

static void tfm_core_test_irq(struct test_result_t *ret)
{
    int32_t err;

    //TODO once timer porting ready, enable the IRQ
    //NVIC_EnableIRQ(TFM_TIMER1_IRQ);

    err = tfm_core_test_irq_scenario(IRQ_TEST_SCENARIO_1);
    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("Failed to execute IRQ test scenario 1.");
        return;
    }

    err = tfm_core_test_irq_scenario(IRQ_TEST_SCENARIO_2);
    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("Failed to execute IRQ test scenario 2.");
        return;
    }

    err = tfm_core_test_irq_scenario(IRQ_TEST_SCENARIO_3);
    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("Failed to execute IRQ test scenario 3.");
        return;
    }

    err = tfm_core_test_irq_scenario(IRQ_TEST_SCENARIO_4);
    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("Failed to execute IRQ test scenario 4.");
        return;
    }

    err = tfm_core_test_irq_scenario(IRQ_TEST_SCENARIO_5);
    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("Failed to execute IRQ test scenario 5.");
        return;
    }

    ret->val = TEST_PASSED;
}
#endif

/*
 * \brief Tests whether the initialisation of the service was successful.
 *
 */
static void tfm_core_test_check_init(struct test_result_t *ret)
{
    int32_t err;
#ifndef TFM_PSA_API
    struct tfm_core_test_call_args_t args = {NULL, 0, NULL, 0};

    err = tfm_core_test_call(tfm_spm_core_test_sfn_init_success_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_INIT_SUCCESS_SID,
                          SPM_CORE_TEST_INIT_SUCCESS_VERSION,
                          NULL, 0, NULL, 0);
#endif /* TFM_PSA_API */

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("Failed to initialise test service.");
        return;
    }

    ret->val = TEST_PASSED;
}

#ifdef ENABLE_TFM_CORE_RECURSION_TESTS
/**
 * \brief Tests what happens when a service calls itself directly
 */
static void tfm_core_test_recursion(struct test_result_t *ret)
{
    /*
     * TODO
     * Recursive calls will trigger a fatal error. Current test framework cannot
     * recover from that error.
     * Leave a test case stub here for further implementation.
     */
    TEST_FAIL("The test case is not implemented yet.");
}
#endif

static void tfm_core_test_buffer_check(struct test_result_t *ret)
{
    int32_t res, i;

    uint32_t inbuf[] = {1, 2, 3, 4, 0xAAAFFF, 0xFFFFFFFF};
    uint32_t outbuf[16] = {0};
    int32_t result = 1;
    psa_invec in_vec[] = { {inbuf, sizeof(inbuf)} };
    psa_outvec outvec[] = { {outbuf, sizeof(outbuf)},
                           {&result, sizeof(int32_t)} };
#ifndef TFM_PSA_API
    struct tfm_core_test_call_args_t args = {in_vec, 1, outvec, 2};

    res = tfm_core_test_call(tfm_spm_core_test_2_sfn_invert_veneer, &args);
#else /* TFM_PSA_API */
    res = psa_test_common(SPM_CORE_TEST_2_INVERT_SID,
                          SPM_CORE_TEST_2_INVERT_VERSION,
                          in_vec, 1, outvec, 2);
#endif /* TFM_PSA_API */
    if (res != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("Call to secure service should be successful.");
        return;
    }
    if (result == 0) {
        for (i = 0; i < sizeof(inbuf) >> 2; i++) {
            if (outbuf[i] != ~inbuf[i]) {
                TEST_FAIL("Secure function failed to modify buffer.");
                return;
            }
        }
        for (; i < sizeof(outbuf) >> 2; i++) {
            if (outbuf[i] != 0) {
                TEST_FAIL("Secure function buffer access overflow.");
                return;
            }
        }
    } else {
        TEST_FAIL("Secure service returned error.");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_core_test_ss_to_ss(struct test_result_t *ret)
{
    int32_t err;

#ifndef TFM_PSA_API
    int32_t test_case_id = CORE_TEST_ID_SS_TO_SS;
    psa_invec in_vec[] = { {&test_case_id, sizeof(int32_t)} };
    struct tfm_core_test_call_args_t args = {in_vec, 1, NULL, 0};

    err = tfm_core_test_call(tfm_spm_core_test_sfn_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_SS_TO_SS_SID,
                          SPM_CORE_TEST_SS_TO_SS_VERSION,
                          NULL, 0, NULL, 0);
#endif /* TFM_PSA_API */

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("The internal service call failed.");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_core_test_ss_to_ss_buffer(struct test_result_t *ret)
{
    int32_t res, i;

    uint32_t inbuf[] = {1, 2, 3, 4, 0xAAAFFF, 0xFFFFFFFF};
    uint32_t outbuf[16] = {0};
    int32_t len = (int32_t)sizeof(inbuf) >> 2;
    psa_outvec out_vec[] = { {outbuf, sizeof(outbuf)} };
#ifndef TFM_PSA_API
    int32_t test_case_id = CORE_TEST_ID_SS_TO_SS_BUFFER;
    psa_invec in_vec[] = { {&test_case_id, sizeof(int32_t)},
                          {inbuf, sizeof(inbuf)},
                          {&len, sizeof(int32_t)} };
    struct tfm_core_test_call_args_t args = {in_vec, 3, out_vec, 1};

    res = tfm_core_test_call(tfm_spm_core_test_sfn_veneer, &args);
#else /* TFM_PSA_API */
    psa_invec in_vec[] = {{inbuf, sizeof(inbuf)},
                          {&len, sizeof(int32_t)} };

    res = psa_test_common(SPM_CORE_TEST_SS_TO_SS_BUFFER_SID,
                          SPM_CORE_TEST_SS_TO_SS_BUFFER_VERSION,
                          in_vec, 2, out_vec, 1);
#endif /* TFM_PSA_API */
    switch (res) {
    case CORE_TEST_ERRNO_SUCCESS:
        for (i = 0; i < sizeof(inbuf) >> 2; i++) {
            if (outbuf[i] != ~inbuf[i]) {
                TEST_FAIL("Secure function failed to modify buffer.");
                return;
            }
        }
        for (; i < sizeof(outbuf) >> 2; i++) {
            if (outbuf[i] != 0) {
                TEST_FAIL("Secure function buffer access overflow.");
                return;
            }
        }
        ret->val = TEST_PASSED;
        return;
    case CORE_TEST_ERRNO_INVALID_BUFFER:
        TEST_FAIL("NS buffer rejected by TF-M core.");
        return;
    case CORE_TEST_ERRNO_SLAVE_SP_CALL_FAILURE:
        TEST_FAIL("Slave service call failed.");
        return;
    case CORE_TEST_ERRNO_SLAVE_SP_BUFFER_FAILURE:
        TEST_FAIL("Slave secure function failed to modify buffer.");
        return;
    default:
        TEST_FAIL("Secure service returned error.");
        return;
    }
}

#ifndef TFM_PSA_API
static void tfm_core_test_get_caller_client_id(struct test_result_t *ret)
{
    int32_t err;
    int32_t test_case_id = CORE_TEST_ID_GET_CALLER_CLIENT_ID;
    psa_invec in_vec[] = { {&test_case_id, sizeof(int32_t)} };
    struct tfm_core_test_call_args_t args = {in_vec, 1, NULL, 0};

    err = tfm_core_test_call(tfm_spm_core_test_sfn_veneer, &args);

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("The internal service call failed.");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_core_test_spm_request(struct test_result_t *ret)
{
    int32_t err;
#ifndef TFM_PSA_API
    int32_t test_case_id = CORE_TEST_ID_SPM_REQUEST;
    psa_invec in_vec[] = { {&test_case_id, sizeof(int32_t)} };
    struct tfm_core_test_call_args_t args = {in_vec, 1, NULL, 0};

    err = tfm_core_test_call(tfm_spm_core_test_sfn_veneer, &args);
#else /* TFM_PSA_API */
    err = psa_test_common(SPM_CORE_TEST_SPM_REQUEST_SID,
                          SPM_CORE_TEST_SPM_REQUEST_VERSION,
                          NULL, 0, NULL, 0);
#endif /* TFM_PSA_API */

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("The SPM request failed.");
        return;
    }

    ret->val = TEST_PASSED;
}

#endif /* TFM_PSA_API */
