// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKALLOC_H
#define UMOCKALLOC_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* umockalloc_malloc(size_t size);
void* umockalloc_calloc(size_t nmemb, size_t size);
void* umockalloc_realloc(void* ptr, size_t size);
void umockalloc_free(void* ptr);

char* umockc_stringify_buffer(const void* bytes, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* UMOCKALLOC_H */
