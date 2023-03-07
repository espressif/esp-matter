/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TEST_FRAMEWORK_H__
#define __TEST_FRAMEWORK_H__

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "log/tfm_log_raw.h"
#include "test_framework_helpers.h"

#ifdef __cplusplus
extern "C" {
#endif

enum test_status_t {
    TEST_PASSED = 0,  /*!< Test has passed */
    TEST_FAILED = 1,  /*!< Test has failed */
};

struct test_result_t {
    enum test_status_t val;  /*!< Test result \ref test_status_t */
    const char *info_msg;    /*!< Information message to show in case of
                              *   failure
                              */
    const char *filename;    /*!< Filename where the failure has occured */
    uint32_t    line;        /*!< Line where the failure has occured */
};

/**
 * \brief Runs the test.
 *
 * \param[out] ret  Test result value
 */
typedef void TEST_FUN(struct test_result_t *ret);

struct test_t {
    TEST_FUN * const test;         /*!< Test function to call */
    const char *name;              /*!< Test name */
    const char *desc;              /*!< Test description */
    struct test_result_t ret;      /*!< Test result */
};

struct test_suite_t;

/**
 * \brief Registers test in the testsuite structure and sets the name.
 *
 * \param[in] p_test_suite  Pointer to the p_test_suite_location.
 */
typedef void TESTSUITE_REG(struct test_suite_t *p_test_suite);

struct test_suite_t {
    TESTSUITE_REG * const freg;     /*!< Function to set all follow fields
                                     *   of the current test suite
                                     */
    struct test_t *test_list;      /*!< List of tests */
    uint32_t list_size;            /*!< List size */
    const char *name;              /*!< Test suite name */
    enum test_status_t val;        /*!< Test suite result \ref test_result_t */
};

enum test_suite_err_t {
    TEST_SUITE_ERR_NO_ERROR = 0,           /*!< No error */
    TEST_SUITE_ERR_INVALID_DATA = 1,       /*!< Invalid test suite if any of the
                                            *   pointers is NULL
                                            */
    TEST_SUITE_ERR_INVALID_TEST_DATA = 2,  /*!< Invalid test if any of the
                                            *  pointers is NULL
                                            */
    TEST_SUITE_ERR_TEST_FAILED  = 3,       /*!< Last executed test has failed */
};

/**
 * \brief Translates the test suite error into a string.
 *
 * \param[in] err  Error value \ref test_suite_err_t
 *
 * \returns error as string.
 */
const char *test_err_to_str(enum test_suite_err_t err);

/**
 * \brief Sets test suite parameters.
 *
 * \param[in] name       Test suite name
 * \param[in] test_list  Pointer to the test list
 * \param[in] size       Test list size
 * \param[in,out] p_ts   Pointer to test suite object to fill in the
 *                       parameters
 *
 * \returns Returns error code as specified in \ref test_suite_err_t
 */
enum test_suite_err_t set_testsuite(const char *name,
                                    struct test_t *test_list, uint32_t size,
                                    struct test_suite_t *p_ts);

/**
 * \brief Runs the given test suite.
 *
 * \param[in,out] test_suite  Test suite to run the list of tests and
 *                            store test results.
 *
 * \returns Returns error code as specified in \ref test_suite_err_t
 */
enum test_suite_err_t run_testsuite(struct test_suite_t *test_suite);

/**
 * \brief Prints all test in the the given test suite.
 *
 * \param[in] ts  Test suite to print the list of tests
 */
void show_tests(const struct test_suite_t *ts);

/**
 * \brief Sets test failure state and information in the \ref test_result_t
 *        structure.
 *
 * \param[in]  info_msg  Information message to show
 * \param[in]  filename  Filename where the error has ocurred
 * \param[in]  line      Line in the file where the error has ocurred
 * \param[out] ret       Pointer to \ref test_result_t structure to
 *                       set the values
 *
 * \note: If info_msg is "" or , info message is not shown. If filename is "",
 *        filename and line are not shown.
 */
void set_test_failed(const char *info_msg, const char *filename, uint32_t line,
                     struct test_result_t *ret);

#define TEST_FAIL(info_msg)  set_test_failed(info_msg, __FILE__, __LINE__, ret)

#define TEST_LOG(...) tfm_log_printf(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* __TEST_FRAMEWORK_H__ */
