// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKCALLRECORDER_H
#define UMOCKCALLRECORDER_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#include "umockcall.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct UMOCKCALLRECORDER_TAG* UMOCKCALLRECORDER_HANDLE;

    UMOCKCALLRECORDER_HANDLE umockcallrecorder_create(void);
    void umockcallrecorder_destroy(UMOCKCALLRECORDER_HANDLE umock_call_recorder);
    int umockcallrecorder_reset_all_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder);
    int umockcallrecorder_add_expected_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder, UMOCKCALL_HANDLE mock_call);
    int umockcallrecorder_add_actual_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder, UMOCKCALL_HANDLE mock_call, UMOCKCALL_HANDLE* matched_call);
    const char* umockcallrecorder_get_actual_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder);
    const char* umockcallrecorder_get_expected_calls(UMOCKCALLRECORDER_HANDLE umock_call_recorder);
    UMOCKCALL_HANDLE umockcallrecorder_get_last_expected_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder);
    UMOCKCALLRECORDER_HANDLE umockcallrecorder_clone(UMOCKCALLRECORDER_HANDLE umock_call_recorder);
    int umockcallrecorder_get_expected_call_count(UMOCKCALLRECORDER_HANDLE umock_call_recorder, size_t* expected_call_count);
    int umockcallrecorder_fail_call(UMOCKCALLRECORDER_HANDLE umock_call_recorder, size_t index);
    int umockcallrecorder_can_call_fail(UMOCKCALLRECORDER_HANDLE umock_call_recorder, size_t index, int* can_call_fail);

#ifdef __cplusplus
}
#endif

#endif /* UMOCKCALLRECORDER_H */
