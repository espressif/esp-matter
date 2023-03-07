// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <windows.h>
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"

#include "azure_macro_utils/macro_utils.h"

LOCK_HANDLE Lock_Init(void)
{
    /* Codes_SRS_LOCK_10_002: [Lock_Init on success shall return a valid lock handle which should be a non NULL value] */
    /* Codes_SRS_LOCK_10_003: [Lock_Init on error shall return NULL ] */
    SRWLOCK* result = malloc(sizeof(SRWLOCK));
    if (result == NULL)
    {
        LogError("Allocate SRWLOCK failed.");
    }
    else
    {
        InitializeSRWLock(result);
    }

    return (LOCK_HANDLE)result;
}

LOCK_RESULT Lock_Deinit(LOCK_HANDLE handle)
{
    LOCK_RESULT result;
    if (handle == NULL)
    {
        /* Codes_SRS_LOCK_10_007: [Lock_Deinit on NULL handle passed returns LOCK_ERROR] */
        LogError("Invalid argument; handle is NULL.");
        result = LOCK_ERROR;
    }
    else
    {
        /* Codes_SRS_LOCK_10_012: [Lock_Deinit frees the memory pointed by handle] */
        free(handle);
        result = LOCK_OK;
    }

    return result;
}

LOCK_RESULT Lock(LOCK_HANDLE handle)
{
    LOCK_RESULT result;
    if (handle == NULL)
    {
        /* Codes_SRS_LOCK_10_007: [Lock on NULL handle passed returns LOCK_ERROR] */
        LogError("Invalid argument; handle is NULL.");
        result = LOCK_ERROR;
    }
    else
    {
        AcquireSRWLockExclusive((SRWLOCK*)handle);
        /* Codes_SRS_LOCK_10_005: [Lock on success shall return LOCK_OK] */
        result = LOCK_OK;

        // Cannot fail
        /* Codes_SRS_LOCK_10_006: [Lock on error shall return LOCK_ERROR] */
    }

    return result;
}

LOCK_RESULT Unlock(LOCK_HANDLE handle)
{
    LOCK_RESULT result;
    if (handle == NULL)
    {
        /* Codes_SRS_LOCK_10_007: [Unlock on NULL handle passed returns LOCK_ERROR] */
        LogError("Invalid argument; handle is NULL.");
        result = LOCK_ERROR;
    }
    else
    {
        ReleaseSRWLockExclusive((SRWLOCK*)handle);
        /* Codes_SRS_LOCK_10_009: [Unlock on success shall return LOCK_OK] */
        result = LOCK_OK;

        // Cannot fail
        /* Codes_SRS_LOCK_10_010: [Unlock on error shall return LOCK_ERROR] */
    }

    return result;
}
