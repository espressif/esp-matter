// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef BUFFER__H
#define BUFFER__H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#include <stdbool.h>
#endif

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct BUFFER_TAG* BUFFER_HANDLE;

MOCKABLE_FUNCTION(, BUFFER_HANDLE, BUFFER_new);
MOCKABLE_FUNCTION(, BUFFER_HANDLE, BUFFER_create, const unsigned char*, source, size_t, size);
MOCKABLE_FUNCTION(, BUFFER_HANDLE, BUFFER_create_with_size, size_t, buff_size);
MOCKABLE_FUNCTION(, void, BUFFER_delete, BUFFER_HANDLE, handle);
MOCKABLE_FUNCTION(, int, BUFFER_pre_build, BUFFER_HANDLE, handle, size_t, size);
MOCKABLE_FUNCTION(, int, BUFFER_build, BUFFER_HANDLE, handle, const unsigned char*, source, size_t, size);
MOCKABLE_FUNCTION(, int, BUFFER_append_build, BUFFER_HANDLE, handle, const unsigned char*, source, size_t, size);
MOCKABLE_FUNCTION(, int, BUFFER_unbuild, BUFFER_HANDLE, handle);
MOCKABLE_FUNCTION(, int, BUFFER_enlarge, BUFFER_HANDLE, handle, size_t, enlargeSize);
MOCKABLE_FUNCTION(, int, BUFFER_shrink, BUFFER_HANDLE, handle, size_t, decreaseSize, bool, fromEnd);
MOCKABLE_FUNCTION(, int, BUFFER_content, BUFFER_HANDLE, handle, const unsigned char**, content);
MOCKABLE_FUNCTION(, int, BUFFER_size, BUFFER_HANDLE, handle, size_t*, size);
MOCKABLE_FUNCTION(, int, BUFFER_append, BUFFER_HANDLE, handle1, BUFFER_HANDLE, handle2);
MOCKABLE_FUNCTION(, int, BUFFER_prepend, BUFFER_HANDLE, handle1, BUFFER_HANDLE, handle2);
MOCKABLE_FUNCTION(, int, BUFFER_fill, BUFFER_HANDLE, handle, unsigned char, fill_char);
MOCKABLE_FUNCTION(, unsigned char*, BUFFER_u_char, BUFFER_HANDLE, handle);
MOCKABLE_FUNCTION(, size_t, BUFFER_length, BUFFER_HANDLE, handle);
MOCKABLE_FUNCTION(, BUFFER_HANDLE, BUFFER_clone, BUFFER_HANDLE, handle);

#ifdef __cplusplus
}
#endif

#endif  /* BUFFER__H */
