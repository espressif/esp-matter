// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef VECTOR_TYPES_H
#define VECTOR_TYPES_H

#ifdef __cplusplus
#else
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct VECTOR_TAG* VECTOR_HANDLE;

typedef bool(*PREDICATE_FUNCTION)(const void* element, const void* value);

#ifdef __cplusplus
}
#endif

#endif /* VECTOR_TYPES_H */
