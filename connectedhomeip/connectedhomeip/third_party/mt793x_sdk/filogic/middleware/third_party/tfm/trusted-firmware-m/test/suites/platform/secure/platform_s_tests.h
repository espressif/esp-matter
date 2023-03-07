/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_S_TESTS_H__
#define __PLATFORM_S_TESTS_H__

#include "test/framework/test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Register testsuite for the platform service.
 *
 * \param[in] "p_test_suite" The test suite to be executed.
 */
void
register_testsuite_s_platform_interface(struct test_suite_t *p_test_suite);

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_S_TESTS_H__ */
