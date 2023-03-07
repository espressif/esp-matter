// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_uamqp_c/async_operation.h"

typedef struct ASYNC_OPERATION_INSTANCE_TAG
{
    ASYNC_OPERATION_CANCEL_HANDLER_FUNC async_operation_cancel_handler;
} ASYNC_OPERATION_INSTANCE;

ASYNC_OPERATION_HANDLE async_operation_create(ASYNC_OPERATION_CANCEL_HANDLER_FUNC async_operation_cancel_handler, size_t context_size)
{
    ASYNC_OPERATION_INSTANCE* async_operation;

    if (async_operation_cancel_handler == NULL)
    {
        /* Codes_SRS_ASYNC_OPERATION_01_002: [ If `async_operation_cancel_handler` is NULL, `async_operation_create` shall fail and return NULL.]*/
        LogError("Cannot allocate memory for async operation");
        async_operation = NULL;
    }
    else if (context_size < sizeof(ASYNC_OPERATION_INSTANCE))
    {
        /* Codes_SRS_ASYNC_OPERATION_01_003: [ If `context_size` is less than the size of the `async_operation_cancel_handler` argument, `async_operation_create` shall fail and return NULL.]*/
        LogError("Context size too small");
        async_operation = NULL;
    }
    else
    {
        async_operation = (ASYNC_OPERATION_INSTANCE*)malloc(context_size);
        if (async_operation == NULL)
        {
            /* Codes_SRS_ASYNC_OPERATION_01_004: [ If allocating memory for the new asynchronous operation instance fails, `async_operation_create` shall fail and return NULL.]*/
            LogError("Cannot allocate memory for async operation");
        }
        else
        {
            /* Codes_SRS_ASYNC_OPERATION_01_001: [ `async_operation_create` shall return a non-NULL handle to a newly created asynchronous operation instance.]*/
            async_operation->async_operation_cancel_handler = async_operation_cancel_handler;
        }
    }

    return async_operation;
}

void async_operation_destroy(ASYNC_OPERATION_HANDLE async_operation)
{
    if (async_operation == NULL)
    {
        /* Codes_SRS_ASYNC_OPERATION_01_006: [ If `async_operation` is NULL, `async_operation_destroy` shall do nothing.]*/
        LogError("NULL async_operation");
    }
    else
    {
        /* Codes_SRS_ASYNC_OPERATION_01_005: [ `async_operation_destroy` shall free all recources associated with the asyncronous operation instance.]*/
        free(async_operation);
    }
}

int async_operation_cancel(ASYNC_OPERATION_HANDLE async_operation)
{
    int result;

    if (async_operation == NULL)
    {
        LogError("NULL async_operation");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_ASYNC_OPERATION_01_007: [ `async_operation_cancel` shall cancel the operation by calling the cancel handler function passed to `async_operation_create`.]*/
        async_operation->async_operation_cancel_handler(async_operation);

        /* Codes_SRS_ASYNC_OPERATION_01_008: [ On success `async_operation_cancel` shall return 0.]*/
        result = 0;
    }

    return result;
}
