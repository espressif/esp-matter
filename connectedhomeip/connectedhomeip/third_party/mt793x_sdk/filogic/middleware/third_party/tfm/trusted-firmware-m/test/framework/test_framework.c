/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test_framework.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void test_failed(const struct test_result_t *ret)
{
    printf_set_color(RED);
    if (ret->info_msg != 0) {
        TEST_LOG("  %s", ret->info_msg);
        if (ret->filename != 0) {
            TEST_LOG(" (Failed at %s:%d)\r\n", ret->filename, ret->line);
        }
    } else {
        if (ret->filename != 0) {
            TEST_LOG("  Failed at %s:%d\r\n", ret->filename, ret->line);
        }
    }

    TEST_LOG("  TEST FAILED!\r\n");
}

static void print_error(const char *err_msg)
{
    printf_set_color(RED);
    TEST_LOG("Error ( %s )\r\n", err_msg);
}

const char *test_err_to_str(enum test_suite_err_t err)
{
    switch (err) {
    case TEST_SUITE_ERR_NO_ERROR:
        return "TEST_SUITE_ERR_NO_ERROR";
    case TEST_SUITE_ERR_INVALID_DATA:
        return "TEST_SUITE_ERR_INVALID_DATA";
    case TEST_SUITE_ERR_INVALID_TEST_DATA:
        return "TEST_SUITE_ERR_INVALID_TEST_DATA";
    case TEST_SUITE_ERR_TEST_FAILED:
        return "TEST_SUITE_ERR_TEST_FAILED";
    /* default:  The default is not defined intentionally to force the
     *           compiler to check that all the enumeration values are
     *           covered in the switch.
     */
    }
}

enum test_suite_err_t set_testsuite(const char *name,
                                    struct test_t *test_list, uint32_t size,
                                    struct test_suite_t *p_ts)
{
    if (p_ts == 0) {
        print_error("TEST_SUITE_ERR_INVALID_DATA!");
        return TEST_SUITE_ERR_INVALID_DATA;
    }

    p_ts->name = name;
    p_ts->test_list = test_list;
    p_ts->list_size = size;

    return TEST_SUITE_ERR_NO_ERROR;
}

void set_test_failed(const char *info_msg, const char *filename, uint32_t line,
                     struct test_result_t *ret)
{
    if (ret == 0) {
        print_error("TEST_SUITE_ERR_INVALID_TEST_DATA!");
        return;
    }

    ret->val = TEST_FAILED;
    ret->info_msg = info_msg;
    ret->filename = filename;
    ret->line = line;
}

enum test_suite_err_t run_testsuite(struct test_suite_t *test_suite)
{
    uint32_t failed_tests = 0;
    uint32_t i;
    struct test_t *p_test;

    if (test_suite == 0 || test_suite->freg == 0) {
        print_error("TEST_SUITE_ERR_INVALID_DATA!");
        return TEST_SUITE_ERR_INVALID_DATA;
    }

    /* Sets test suite parameters */
    test_suite->freg(test_suite);
    if (test_suite->name == 0 || test_suite->list_size == 0) {
        print_error("TEST_SUITE_ERR_INVALID_DATA!");
        return TEST_SUITE_ERR_INVALID_DATA;
    }

    printf_set_color(YELLOW);
    TEST_LOG("Running Test Suite %s...\r\n", test_suite->name);

    /* Sets pointer to the first test */
    p_test = test_suite->test_list;

    for (i = 0; i < test_suite->list_size; i++) {

        if (p_test->test == 0 || p_test->name == 0) {
            print_error("TEST_SUITE_ERR_INVALID_TEST_DATA!");
            return TEST_SUITE_ERR_INVALID_TEST_DATA;
        }

        printf_set_color(WHITE);
        TEST_LOG("> Executing '%s' \r\n  Description: '%s'\r\n",
                 p_test->name, p_test->desc);

        /* Sets the default value before the test */
        p_test->ret.val = TEST_PASSED;

        /* Executes the test */
        p_test->test(&p_test->ret);
        if (p_test->ret.val == TEST_FAILED) {
            test_failed(&p_test->ret);
            failed_tests++;
        } else {
            printf_set_color(GREEN);
            TEST_LOG("  TEST PASSED!\r\n");
        }

        /* Sets pointer to the next test */
        p_test++;
    }


    if (failed_tests == 0) {
        printf_set_color(GREEN);
        TEST_LOG("TESTSUITE PASSED!\r\n");
        test_suite->val = TEST_PASSED;
    } else {
        printf_set_color(RED);
        TEST_LOG("TESTSUITE FAILED!\r\n");
        printf_set_color(YELLOW);
        TEST_LOG("Number of failed tests: %d of %d\r\n",
                 failed_tests, test_suite->list_size);
        test_suite->val = TEST_FAILED;
    }

    return TEST_SUITE_ERR_NO_ERROR;
}
