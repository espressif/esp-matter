/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "core_ns_tests.h"
#include "tfm_api.h"
#include "app/tfm_integ_test.h"
#include "test/suites/core/non_secure/core_test_api.h"
#include "test/test_services/tfm_core_test/core_test_defs.h"

#include <stdio.h>
#include <string.h>

/* Define test suite for core interactive tests */
/* List of tests */
static void tfm_core_test_2001(struct test_result_t *ret);

static struct test_t core_tests[] = {
    {&tfm_core_test_2001, "TFM_CORE_TEST_2001",
     "Interactive tests", {TEST_PASSED} },
};

void register_testsuite_ns_core_interactive(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(core_tests) / sizeof(core_tests[0]));

    set_testsuite("Core non-secure interactive tests (TFM_CORE_TEST_2XXX)",
                  core_tests, list_size, p_test_suite);
}

/**
 * \brief Tests core function with interactive test cases
 */
static void tfm_core_test_2001(struct test_result_t *ret)
{
    execute_ns_interactive_tests();

    ret->val = TEST_PASSED;
}
