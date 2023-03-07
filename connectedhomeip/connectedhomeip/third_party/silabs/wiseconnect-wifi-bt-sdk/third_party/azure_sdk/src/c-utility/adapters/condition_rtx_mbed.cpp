// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/condition.h"
#include "rtos.h"

COND_HANDLE Condition_Init(void)
{
    return NULL;
}

COND_RESULT Condition_Post(COND_HANDLE handle)
{
    COND_RESULT result;
    if (handle == NULL)
    {
        result = COND_INVALID_ARG;
    }
    else
    {
        result = COND_ERROR;
    }
    return result;
}

COND_RESULT Condition_Wait(COND_HANDLE handle, LOCK_HANDLE lock, int timeout_milliseconds)
{
    COND_RESULT result;
    if (handle == NULL)
    {
        result = COND_INVALID_ARG;
    }
    else
    {
        result = COND_ERROR;
    }
    return result;
}

void Condition_Deinit(COND_HANDLE handle)
{
    if (handle != NULL)
    {
    }
}
