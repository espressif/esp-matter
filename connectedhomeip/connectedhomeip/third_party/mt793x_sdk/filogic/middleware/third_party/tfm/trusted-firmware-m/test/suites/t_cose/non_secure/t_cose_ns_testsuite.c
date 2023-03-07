/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "t_cose_ns_tests.h"
#include <stdio.h>
#include "lib/ext/t_cose/test/run_tests.h"

/*
 * The t_cose test cases are defined in: lib/ext/t_cose/test/run_test.c
 *
 * There are two types of configuration to execute test cases:
 *  - All tests: Just pass an array containing a NULL pointer to RunTest():
 *               t_cose_test_cases_all[]. This is the default configuration.
 *
 *  - Selected tests: Edit the s_tests[] array in
 *                    lib/ext/t_cose/test/run_tests.c.
 */

/* Define test suite for t_cose library */
static void tfm_t_cose_test_8001(struct test_result_t *ret);

static struct test_t t_cose_regression_test[] = {
    {&tfm_t_cose_test_8001, "TFM_T_COSE_TEST_8001",
     "Regression test of t_cose library", {TEST_PASSED} },
};

/* To execute all test cases, then pass this array to RunTestsTCose()
 * Test cases are defined in: lib/ext/t_cose/test/run_tests.c
 */
const static char *t_cose_test_cases_all[] = {
    0, /* Indicates to run all enabled test case */
};

/**
 * \brief Print t_cose test results per test case
 *
 * \param[in]  szString  String to print out
 * \param[in]  ctx       Where to print
 */

static void fputs_wrapper(const char *string, void *out_ctx, int new_line)
{
    (void)out_ctx;
    (void)new_line;
    /*
     * To get test result per test case, change
     * the preprocessor '#if 0' to '#if 1'.
     */
# if 0
    TEST_LOG("%s\r\n", string);
#else
    (void)string;
#endif
}

static void tfm_t_cose_test_8001(struct test_result_t *ret)
{
    int32_t test_failed_cnt = 0;

    test_failed_cnt = RunTestsTCose(t_cose_test_cases_all, fputs_wrapper,
                                    NULL, NULL);
    if (test_failed_cnt != 0) {
        TEST_FAIL("t_cose test failed");
        return;
    }

    ret->val = TEST_PASSED;
}

void
register_testsuite_ns_t_cose(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(t_cose_regression_test) /
                 sizeof(t_cose_regression_test[0]));

    set_testsuite("T_COSE regression test"
                  "(TFM_T_COSE_TEST_8XXX)",
                  t_cose_regression_test, list_size, p_test_suite);
}
