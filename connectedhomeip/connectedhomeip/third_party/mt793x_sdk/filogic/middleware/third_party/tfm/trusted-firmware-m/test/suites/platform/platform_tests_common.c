/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_platform_api.h"
#include "platform_tests_common.h"

/*!
 * \brief Call the platform service with an invalid request
 */
void tfm_platform_test_common_001(struct test_result_t *ret)
{
    int32_t err;

    err = tfm_platform_ioctl((tfm_platform_ioctl_req_t) INVALID_REQUEST,
                             NULL,
                             NULL);
    if (err != TFM_PLATFORM_ERR_NOT_SUPPORTED) {
        TEST_FAIL("Call with invalid request should fail.");
        return;
    }

    ret->val = TEST_PASSED;
}
