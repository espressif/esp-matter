// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef VECTOR_TYPES_INTERNAL_H
#define VECTOR_TYPES_INTERNAL_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct VECTOR_TAG
    {
        void* storage;
        size_t count;
        size_t elementSize;
    } VECTOR;

#ifdef __cplusplus
}
#endif

#endif /* VECTOR_TYPES_INTERNAL_H */
