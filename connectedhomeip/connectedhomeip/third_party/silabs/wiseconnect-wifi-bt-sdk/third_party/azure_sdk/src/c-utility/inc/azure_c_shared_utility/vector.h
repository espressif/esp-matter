// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef VECTOR_H
#define VECTOR_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#include <stdbool.h>
#endif

#include "azure_c_shared_utility/crt_abstractions.h"
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/vector_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

MOCKABLE_INTERFACE(vector,
    /* creation */
    FUNCTION(, VECTOR_HANDLE, VECTOR_create, size_t, elementSize),
    FUNCTION(, VECTOR_HANDLE, VECTOR_move, VECTOR_HANDLE, handle),
    FUNCTION(, void, VECTOR_destroy, VECTOR_HANDLE, handle),

    /* insertion */
    FUNCTION(, int, VECTOR_push_back, VECTOR_HANDLE, handle, const void*, elements, size_t, numElements),

    /* removal */
    FUNCTION(, void, VECTOR_erase, VECTOR_HANDLE, handle, void*, elements, size_t, numElements),
    FUNCTION(, void, VECTOR_clear, VECTOR_HANDLE, handle),

    /* access */
    FUNCTION(, void*, VECTOR_element, VECTOR_HANDLE, handle, size_t, index),
    FUNCTION(, void*, VECTOR_front, VECTOR_HANDLE, handle),
    FUNCTION(, void*, VECTOR_back, VECTOR_HANDLE, handle),
    FUNCTION(, void*, VECTOR_find_if, VECTOR_HANDLE, handle, PREDICATE_FUNCTION, pred, const void*, value),

    /* capacity */
    FUNCTION(, size_t, VECTOR_size, VECTOR_HANDLE, handle)
)
#ifdef __cplusplus
}
#endif

#endif /* VECTOR_H */
