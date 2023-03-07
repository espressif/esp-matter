/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CORE_NS_TESTS_H__
#define __CORE_NS_TESTS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "test/framework/test_framework.h"

/**
 * \brief Register testsuite for the core positive tests.
 *
 * \param[in] p_test_suite  The test suite to be executed.
 */
void register_testsuite_ns_core_positive(struct test_suite_t *p_test_suite);

/**
 * \brief Register testsuite for the core interactive tests.
 *
 * \param[in] p_test_suite  The test suite to be executed.
 */
void register_testsuite_ns_core_interactive(struct test_suite_t *p_test_suite);

#ifdef __cplusplus
}
#endif

#endif /* __CORE_NS_TESTS_H__ */
