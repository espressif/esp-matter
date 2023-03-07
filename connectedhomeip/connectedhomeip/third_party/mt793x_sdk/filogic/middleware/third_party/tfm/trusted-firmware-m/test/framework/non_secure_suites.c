/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test_framework_integ_test.h"
#include "test_framework_integ_test_helper.h"
#include "test_framework.h"

/* Service specific includes */
#include "test/suites/sst/non_secure/sst_ns_tests.h"
#include "test/suites/its/non_secure/its_ns_tests.h"
#include "test/suites/audit/non_secure/audit_ns_tests.h"
#include "test/suites/crypto/non_secure/crypto_ns_tests.h"
#include "test/suites/attestation/non_secure/attestation_ns_tests.h"
#include "test/suites/qcbor/non_secure/qcbor_ns_tests.h"
#include "test/suites/t_cose/non_secure/t_cose_ns_tests.h"
#include "test/suites/core/non_secure/core_ns_tests.h"
#include "test/suites/ipc/non_secure/ipc_ns_tests.h"
#include "test/suites/platform/non_secure/platform_ns_tests.h"
#include "test/suites/multi_core/non_secure/multi_core_ns_test.h"

static struct test_suite_t test_suites[] = {
#ifdef SERVICES_TEST_NS
    /* List test cases which are compliant with level 1 isolation */

#ifdef ENABLE_SECURE_STORAGE_SERVICE_TESTS
    {&register_testsuite_ns_psa_ps_interface, 0, 0, 0},
#endif

#ifdef ENABLE_INTERNAL_TRUSTED_STORAGE_SERVICE_TESTS
    /* Non-secure ITS test cases */
    {&register_testsuite_ns_psa_its_interface, 0, 0, 0},
#endif

#ifdef ENABLE_CRYPTO_SERVICE_TESTS
    /* Non-secure Crypto test cases */
    {&register_testsuite_ns_crypto_interface, 0, 0, 0},
#endif

#ifdef ENABLE_ATTESTATION_SERVICE_TESTS
    /* Non-secure initial attestation service test cases */
    {&register_testsuite_ns_attestation_interface, 0, 0, 0},
#endif

#ifdef ENABLE_PLATFORM_SERVICE_TESTS
    /* Non-secure platform service test cases */
    {&register_testsuite_ns_platform_interface, 0, 0, 0},
#endif

#ifdef ENABLE_QCBOR_TESTS
    /* Non-secure QCBOR library test cases */
    {&register_testsuite_ns_qcbor, 0, 0, 0},
#endif

#ifdef ENABLE_T_COSE_TESTS
    /* Non-secure T_COSE library test cases */
    {&register_testsuite_ns_t_cose, 0, 0, 0},
#endif

#ifdef ENABLE_AUDIT_LOGGING_SERVICE_TESTS
    /* Non-secure Audit Logging test cases */
    {&register_testsuite_ns_audit_interface, 0, 0, 0},
#endif

#endif /* SERVICES_TEST_NS */

#ifdef CORE_TEST_POSITIVE
    /* Non-secure core test cases */
    {&register_testsuite_ns_core_positive, 0, 0, 0},
#endif

#ifdef CORE_TEST_INTERACTIVE
    /* Non-secure interactive test cases */
    {&register_testsuite_ns_core_interactive, 0, 0, 0},
#endif

#ifdef ENABLE_IPC_TEST
    /* Non-secure IPC test cases */
    {&register_testsuite_ns_ipc_interface, 0, 0, 0},
#endif

#ifdef TFM_MULTI_CORE_TEST
    /* Multi-core topology test cases */
    {&register_testsuite_multi_core_ns_interface, 0, 0, 0},
#endif

    /* End of test suites */
    {0, 0, 0, 0}
};

enum test_suite_err_t start_integ_test(void)
{
    return integ_test("Non-secure", test_suites);
}

/* Service stand-in for NS tests. To be called from a non-secure context */
enum test_suite_err_t tfm_non_secure_client_run_tests(void)
{
    return start_integ_test();
}
