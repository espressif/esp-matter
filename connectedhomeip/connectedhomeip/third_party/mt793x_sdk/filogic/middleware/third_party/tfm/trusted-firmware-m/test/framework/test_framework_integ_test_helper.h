/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __INTEG_TEST_HELPER_H__
#define __INTEG_TEST_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "test_framework.h"

/**
 * \brief Executes integration test suites provided in the parameters.
 *
 * \param[in] suite_type      A string containing the type of the suite
 *                            (used for printing).
 * \param[in] test_suites     The suites to be executed.
 * \param[in] test_suite_cnt  The number of test suites to be executed.
 *
 * \returns Returns error code as specified in \ref test_suite_err_t
 */
enum test_suite_err_t integ_test(const char *suite_type,
                                 struct test_suite_t test_suites[]);

#ifdef __cplusplus
}
#endif

#endif /* __INTEG_TEST_HELPER_H__ */
