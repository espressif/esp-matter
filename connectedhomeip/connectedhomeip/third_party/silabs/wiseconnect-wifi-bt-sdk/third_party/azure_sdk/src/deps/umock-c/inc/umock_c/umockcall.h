// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKCALL_H
#define UMOCKCALL_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct UMOCKCALL_TAG* UMOCKCALL_HANDLE;
    typedef void*(*UMOCKCALL_DATA_COPY_FUNC)(void* umockcall_data);
    typedef void(*UMOCKCALL_DATA_FREE_FUNC)(void* umockcall_data);
    typedef char*(*UMOCKCALL_DATA_STRINGIFY_FUNC)(void* umockcall_data);
    typedef int(*UMOCKCALL_DATA_ARE_EQUAL_FUNC)(void* left, void* right);

    UMOCKCALL_HANDLE umockcall_create(const char* function_name, void* umockcall_data, UMOCKCALL_DATA_COPY_FUNC umockcall_data_copy, UMOCKCALL_DATA_FREE_FUNC umockcall_data_free, UMOCKCALL_DATA_STRINGIFY_FUNC umockcall_data_stringify, UMOCKCALL_DATA_ARE_EQUAL_FUNC umockcall_data_are_equal);
    void umockcall_destroy(UMOCKCALL_HANDLE umockcall);
    int umockcall_are_equal(UMOCKCALL_HANDLE left, UMOCKCALL_HANDLE right);
    char* umockcall_stringify(UMOCKCALL_HANDLE umockcall);
    void* umockcall_get_call_data(UMOCKCALL_HANDLE umockcall);
    UMOCKCALL_HANDLE umockcall_clone(UMOCKCALL_HANDLE umockcall);
    int umockcall_set_fail_call(UMOCKCALL_HANDLE umockcall, int fail_call);
    int umockcall_get_fail_call(UMOCKCALL_HANDLE umockcall);
    int umockcall_set_ignore_all_calls(UMOCKCALL_HANDLE umockcall, int ignore_all_calls);
    int umockcall_get_ignore_all_calls(UMOCKCALL_HANDLE umockcall);
    int umockcall_set_call_can_fail(UMOCKCALL_HANDLE umockcall, int call_can_fail);
    int umockcall_get_call_can_fail(UMOCKCALL_HANDLE umockcall);

#ifdef __cplusplus
}
#endif

#endif /* UMOCKCALL_H */
