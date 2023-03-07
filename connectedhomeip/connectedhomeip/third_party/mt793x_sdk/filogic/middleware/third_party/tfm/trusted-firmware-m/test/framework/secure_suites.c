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
#include "test/suites/sst/secure/sst_tests.h"
#include "test/suites/its/secure/its_s_tests.h"
#include "test/suites/audit/secure/audit_s_tests.h"
#include "test/suites/attestation/secure/attestation_s_tests.h"
#include "test/suites/crypto/secure/crypto_s_tests.h"
#include "test/suites/ipc/secure/ipc_s_tests.h"
#include "test/suites/platform/secure/platform_s_tests.h"

static struct test_suite_t test_suites[] = {
#ifdef SERVICES_TEST_S
    /* List test cases which are compliant with level 1 isolation */

#ifdef ENABLE_SECURE_STORAGE_SERVICE_TESTS
    {&register_testsuite_s_psa_ps_interface, 0, 0, 0},
    {&register_testsuite_s_psa_ps_reliability, 0, 0, 0},

#ifdef SST_TEST_NV_COUNTERS
    {&register_testsuite_s_rollback_protection, 0, 0, 0},
#endif
#endif

#ifdef ENABLE_INTERNAL_TRUSTED_STORAGE_SERVICE_TESTS
    /* Secure ITS test cases */
    {&register_testsuite_s_psa_its_interface, 0, 0, 0},
    {&register_testsuite_s_psa_its_reliability, 0, 0, 0},
#endif

#ifdef ENABLE_CRYPTO_SERVICE_TESTS
    /* Crypto test cases */
    {&register_testsuite_s_crypto_interface, 0, 0, 0},
#endif

#ifdef ENABLE_ATTESTATION_SERVICE_TESTS
    /* Secure initial attestation service test cases */
    {&register_testsuite_s_attestation_interface, 0, 0, 0},
#endif

#ifdef ENABLE_PLATFORM_SERVICE_TESTS
    /* Secure platform service test cases */
    {&register_testsuite_s_platform_interface, 0, 0, 0},
#endif

#ifdef ENABLE_AUDIT_LOGGING_SERVICE_TESTS
    /* Secure Audit Logging test cases */
    {&register_testsuite_s_audit_interface, 0, 0, 0},
#endif

#ifdef ENABLE_IPC_TEST
    /* Secure IPC test cases */
    {&register_testsuite_s_ipc_interface, 0, 0, 0},
#endif
#endif /* SERVICES_TEST_S */
    /* End of test suites */
    {0, 0, 0, 0}
};

static void setup_integ_test(void)
{
    /* Left empty intentionally, currently implemented
     * test suites require no setup
     */
}

static void tear_down_integ_test(void)
{
    /* Left empty intentionally, currently implemented
     * test suites require no tear down
     */
}

enum test_suite_err_t start_integ_test(void)
{
    enum test_suite_err_t retval;

    setup_integ_test();
    retval = integ_test("Secure", test_suites);
    tear_down_integ_test();
    return retval;
}
