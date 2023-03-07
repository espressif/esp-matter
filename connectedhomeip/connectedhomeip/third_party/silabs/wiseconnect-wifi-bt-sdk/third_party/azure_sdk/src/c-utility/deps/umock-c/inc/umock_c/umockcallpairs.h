// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKCALLPAIRS_H
#define UMOCKCALLPAIRS_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PAIRED_HANDLE_TAG
{
    void* handle_value;
    char* handle_type;
} PAIRED_HANDLE;

typedef struct PAIRED_HANDLES_TAG
{
    PAIRED_HANDLE* paired_handles;
    size_t paired_handle_count;
} PAIRED_HANDLES;

int umockcallpairs_track_create_paired_call(PAIRED_HANDLES* paired_handles, const void* handle, const char* handle_type, size_t handle_type_size);
int umockcallpairs_track_destroy_paired_call(PAIRED_HANDLES* paired_handles, const void* handle);

#ifdef __cplusplus
}
#endif

#endif /* UMOCKCALLPAIRS_H */
