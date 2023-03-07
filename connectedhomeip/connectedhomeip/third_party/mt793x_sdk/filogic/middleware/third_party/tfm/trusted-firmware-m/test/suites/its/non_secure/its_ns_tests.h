/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ITS_NS_TESTS_H__
#define __ITS_NS_TESTS_H__

#include "test/framework/test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Register testsuite for the PSA internal trusted storage NS interface
 *        tests.
 *
 * \param[in] p_test_suite  The test suite to be executed.
 */
void register_testsuite_ns_psa_its_interface(struct test_suite_t *p_test_suite);

#ifdef __cplusplus
}
#endif

#endif /* __ITS_NS_TESTS_H__ */
