// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/xlogging.h"
#include "rtos.h"


LOCK_HANDLE Lock_Init(void)
{
    /* Codes_SRS_LOCK_10_002: [Lock_Init on success shall return a valid lock handle which should be a non NULL value] */
    /* Codes_SRS_LOCK_10_003: [Lock_Init on error shall return NULL] */
    Mutex* result = new Mutex();
    if (result == NULL)
    {
        LogError("Failed to instantiate a new Mutex object.");
    }

    return (LOCK_HANDLE)result;
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
        Mutex* lock_mtx = (Mutex*)handle;
        if (lock_mtx->lock() == osOK)
        {
            /* Codes_SRS_LOCK_10_005: [Lock on success shall return LOCK_OK] */
            result = LOCK_OK;
        }
        else
        {
            /* Codes_SRS_LOCK_10_006: [Lock on error shall return LOCK_ERROR] */
            LogError("Mutex(%p)->lock() failed.", handle);
            result = LOCK_ERROR;
        }
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
        Mutex* lock_mtx = (Mutex*)handle;
        if (lock_mtx->unlock() == osOK)
        {
            /* Codes_SRS_LOCK_10_009: [Unlock on success shall return LOCK_OK] */
            result = LOCK_OK;
        }
        else
        {
            /* Codes_SRS_LOCK_10_010: [Unlock on error shall return LOCK_ERROR] */
            LogError("Mutex(%p)->unlock() failed.", handle);
            result = LOCK_ERROR;
        }
    }

    return result;
}

LOCK_RESULT Lock_Deinit(LOCK_HANDLE handle)
{
    LOCK_RESULT result;
    if (NULL == handle)
    {
        /* Codes_SRS_LOCK_10_007: [Lock_Deinit on NULL handle passed returns LOCK_ERROR] */
        LogError("Invalid argument; handle is NULL.");
        result = LOCK_ERROR;
    }
    else
    {
        /* Codes_SRS_LOCK_10_012: [Lock_Deinit frees the memory pointed by handle] */
        Mutex* lock_mtx = (Mutex*)handle;
        delete lock_mtx;
        result = LOCK_OK;
    }
    
    return result;
}
