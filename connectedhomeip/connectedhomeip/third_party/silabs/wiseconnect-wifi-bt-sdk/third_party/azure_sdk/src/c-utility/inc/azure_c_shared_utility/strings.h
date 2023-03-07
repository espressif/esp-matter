// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef STRINGS_H
#define STRINGS_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/strings_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

MOCKABLE_FUNCTION(, STRING_HANDLE, STRING_new);
MOCKABLE_FUNCTION(, STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
MOCKABLE_FUNCTION(, STRING_HANDLE, STRING_construct, const char*, psz);
MOCKABLE_FUNCTION(, STRING_HANDLE, STRING_construct_n, const char*, psz, size_t, n);
MOCKABLE_FUNCTION(, STRING_HANDLE, STRING_new_with_memory, const char*, memory);
MOCKABLE_FUNCTION(, STRING_HANDLE, STRING_new_quoted, const char*, source);
MOCKABLE_FUNCTION(, STRING_HANDLE, STRING_new_JSON, const char*, source);
MOCKABLE_FUNCTION(, STRING_HANDLE, STRING_from_byte_array, const unsigned char*, source, size_t, size);
MOCKABLE_FUNCTION(, void, STRING_delete, STRING_HANDLE, handle);
MOCKABLE_FUNCTION(, int, STRING_concat, STRING_HANDLE, handle, const char*, s2);
MOCKABLE_FUNCTION(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
MOCKABLE_FUNCTION(, int, STRING_quote, STRING_HANDLE, handle);
MOCKABLE_FUNCTION(, int, STRING_copy, STRING_HANDLE, s1, const char*, s2);
MOCKABLE_FUNCTION(, int, STRING_copy_n, STRING_HANDLE, s1, const char*, s2, size_t, n);
MOCKABLE_FUNCTION(, const char*, STRING_c_str, STRING_HANDLE, handle);
MOCKABLE_FUNCTION(, int, STRING_empty, STRING_HANDLE, handle);
MOCKABLE_FUNCTION(, size_t, STRING_length, STRING_HANDLE, handle);
MOCKABLE_FUNCTION(, int, STRING_compare, STRING_HANDLE, s1, STRING_HANDLE, s2);
MOCKABLE_FUNCTION(, int, STRING_replace, STRING_HANDLE, handle, char, target, char, replace);

extern STRING_HANDLE STRING_construct_sprintf(const char* format, ...);
extern int STRING_sprintf(STRING_HANDLE s1, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif  /*STRINGS_H*/
