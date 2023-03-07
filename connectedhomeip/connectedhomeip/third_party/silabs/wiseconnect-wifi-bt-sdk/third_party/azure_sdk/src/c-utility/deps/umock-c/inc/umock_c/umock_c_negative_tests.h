// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCK_C_NEGATIVE_TESTS_H
#define UMOCK_C_NEGATIVE_TESTS_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

    int umock_c_negative_tests_init(void);
    void umock_c_negative_tests_deinit(void);
    void umock_c_negative_tests_snapshot(void);
    void umock_c_negative_tests_reset(void);
    void umock_c_negative_tests_fail_call(size_t index);
    size_t umock_c_negative_tests_call_count(void);
    int umock_c_negative_tests_can_call_fail(size_t index);

#ifdef __cplusplus
}
#endif

#endif /* UMOCK_C_NEGATIVE_TESTS_H */
