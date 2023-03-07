// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef LOGGING_STACKTRACE_H
#define LOGGING_STACKTRACE_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

char* getStackAsString(void); 
void* logging_malloc(size_t size); /*same as malloc from stdlib, always.*/
void logging_free(void* ptr); /*same as free from stdlib, always.*/

#ifdef __cplusplus
}
#endif

#endif /*LOGGING_STACKTRACE_H*/
