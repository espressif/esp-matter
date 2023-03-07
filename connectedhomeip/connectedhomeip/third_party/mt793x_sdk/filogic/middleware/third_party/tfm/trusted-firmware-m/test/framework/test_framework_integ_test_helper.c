/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>

#include "test_framework.h"
#include "test_framework_integ_test_helper.h"

enum test_suite_err_t integ_test(const char *suite_type,
                                 struct test_suite_t test_suites[])
{
    uint32_t i;
    enum test_suite_err_t retval = TEST_SUITE_ERR_NO_ERROR;

    printf_set_color(YELLOW);
    TEST_LOG("\r\n#### Execute test suites for the %s area ####\r\n",
             suite_type);

    /* Executes test suites */
    for (i = 0; test_suites[i].freg != NULL; i++) {
        retval = run_testsuite(&test_suites[i]);
        if (retval != TEST_SUITE_ERR_NO_ERROR) {
            /* End function execution */
            return retval;
        }
    }

    /* Prints test suites summary */
    printf_set_color(YELLOW);
    TEST_LOG("\r\n*** %s test suites summary ***\r\n", suite_type);
    for (i = 0; test_suites[i].freg != NULL; i++) {
        printf_set_color(WHITE);
        TEST_LOG("Test suite '%s' has ", test_suites[i].name);
        if (test_suites[i].val == TEST_PASSED) {
            printf_set_color(GREEN);
            TEST_LOG(" PASSED\r\n");
        } else {
            printf_set_color(RED);
            TEST_LOG(" FAILED\r\n");
            retval = TEST_SUITE_ERR_TEST_FAILED;
        }
    }

    printf_set_color(YELLOW);
    TEST_LOG("\r\n*** End of %s test suites ***\r\n", suite_type);
    return retval;
}
