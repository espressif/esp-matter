// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef ASYNC_OPERATION_H
#define ASYNC_OPERATION_H

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#include <cstddef>
#else
#include <stdint.h>
#include <stddef.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"

typedef struct ASYNC_OPERATION_INSTANCE_TAG* ASYNC_OPERATION_HANDLE;

typedef void(*ASYNC_OPERATION_CANCEL_HANDLER_FUNC)(ASYNC_OPERATION_HANDLE async_operation);

#define DEFINE_ASYNC_OPERATION_CONTEXT(type) \
typedef struct MU_C3(ASYNC_OPERATION_CONTEXT_STRUCT_, type, _TAG) \
{ \
    ASYNC_OPERATION_CANCEL_HANDLER_FUNC async_operation_cancel_handler; \
    type context; \
} MU_C2(ASYNC_OPERATION_CONTEXT_STRUCT_, type);

#define GET_ASYNC_OPERATION_CONTEXT(type, async_operation) \
    (type*)((unsigned char*)async_operation + offsetof(MU_C2(ASYNC_OPERATION_CONTEXT_STRUCT_, type), context))

#define CREATE_ASYNC_OPERATION(type, async_operation_cancel_handler) \
    async_operation_create(async_operation_cancel_handler, sizeof(MU_C2(ASYNC_OPERATION_CONTEXT_STRUCT_, type)))

MOCKABLE_FUNCTION(, ASYNC_OPERATION_HANDLE, async_operation_create, ASYNC_OPERATION_CANCEL_HANDLER_FUNC, async_operation_cancel_handler, size_t, context_size);
MOCKABLE_FUNCTION(, void, async_operation_destroy, ASYNC_OPERATION_HANDLE, async_operation);
MOCKABLE_FUNCTION(, int, async_operation_cancel, ASYNC_OPERATION_HANDLE, async_operation);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASYNC_OPERATION_H */
