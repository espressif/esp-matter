// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <pthread.h>
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/xlogging.h"

LOCK_HANDLE Lock_Init(void)
{
    /* Codes_SRS_LOCK_10_002: [Lock_Init on success shall return a valid lock handle which should be a non NULL value] */
    pthread_mutex_t* result = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (result == NULL)
    {
        LogError("malloc failed.");
    }
    else
    {
        if (pthread_mutex_init(result, NULL) != 0)
        {
            /* Codes_SRS_LOCK_10_003: [Lock_Init on error shall return NULL ] */
            LogError("pthread_mutex_init failed.");
            free(result);
            result = NULL;
        }
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
        if (pthread_mutex_lock((pthread_mutex_t*)handle) == 0)
        {
            /* Codes_SRS_LOCK_10_005: [Lock on success shall return LOCK_OK] */
            result = LOCK_OK;
        }
        else
        {
            /* Codes_SRS_LOCK_10_006: [Lock on error shall return LOCK_ERROR] */
            LogError("pthread_mutex_lock failed.");
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
        if (pthread_mutex_unlock((pthread_mutex_t*)handle) == 0)
        {
            /* Codes_SRS_LOCK_10_009: [Unlock on success shall return LOCK_OK] */
            result = LOCK_OK;
        }
        else
        {
            /* Codes_SRS_LOCK_10_010: [Unlock on error shall return LOCK_ERROR] */
            LogError("pthread_mutex_unlock failed.");
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
        if(pthread_mutex_destroy((pthread_mutex_t*)handle) == 0)
        {
            free(handle);
            handle = NULL;
            result = LOCK_OK;
        }
        else
        {
            LogError("pthread_mutex_destroy failed;");
            result = LOCK_ERROR;
        }
    }

    return result;
}
