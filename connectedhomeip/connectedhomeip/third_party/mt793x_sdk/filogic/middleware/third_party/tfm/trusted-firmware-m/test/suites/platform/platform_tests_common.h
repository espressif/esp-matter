/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_TESTS_COMMON_H__
#define __PLATFORM_TESTS_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "test/framework/test_framework.h"

#define INVALID_REQUEST 0xffffffff

/*!
 * \brief Call the platform service with an invalid request
 *
 * \param[out] ret  Test results
 */
void tfm_platform_test_common_001(struct test_result_t *ret);

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_TESTS_COMMON_H__ */
