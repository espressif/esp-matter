/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __T_COSE_NS_TESTS_H__
#define __T_COSE_NS_TESTS_H__

#include "test/framework/test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Register test suite for the t_cose library
 *
 * \param[in] p_test_suite The test suite to be executed.
 */
void
register_testsuite_ns_t_cose(struct test_suite_t *p_test_suite);

#ifdef __cplusplus
}
#endif

#endif /* __T_COSE_NS_TESTS_H__ */
