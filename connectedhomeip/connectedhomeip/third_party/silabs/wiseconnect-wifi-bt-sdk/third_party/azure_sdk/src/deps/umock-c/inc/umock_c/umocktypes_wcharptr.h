// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKTYPES_WCHARPTR_H
#define UMOCKTYPES_WCHARPTR_H

#ifndef __cplusplus
#include <wchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /* Codes_SRS_UMOCK_C_LIB_01_067: [wchar_t\* and const wchar_t\* shall be supported out of the box through a separate header, umockvalue_wcharptr.h.]*/
    /* Codes_SRS_UMOCK_C_LIB_01_069: [The signature shall be: ...*/
    int umocktypes_wcharptr_register_types(void);

    char* umocktypes_stringify_wcharptr(const wchar_t** value);
    int umocktypes_are_equal_wcharptr(const wchar_t** left, const wchar_t** right);
    int umocktypes_copy_wcharptr(wchar_t** destination, const wchar_t** source);
    void umocktypes_free_wcharptr(wchar_t** value);

    char* umocktypes_stringify_const_wcharptr(const wchar_t** value);
    int umocktypes_are_equal_const_wcharptr(const wchar_t** left, const wchar_t** right);
    int umocktypes_copy_const_wcharptr(const wchar_t** destination, const wchar_t** source);
    void umocktypes_free_const_wcharptr(const wchar_t** value);

#ifdef __cplusplus
}
#endif

#endif /* UMOCKTYPES_WCHARPTR_H */
