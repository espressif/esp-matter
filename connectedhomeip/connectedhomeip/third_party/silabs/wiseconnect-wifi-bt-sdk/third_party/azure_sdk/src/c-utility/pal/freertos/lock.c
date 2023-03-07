// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/xlogging.h"


#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

LOCK_HANDLE Lock_Init(void)
{
    /* Codes_SRS_LOCK_10_002: [Lock_Init on success shall return a valid lock handle which should be a non NULL value] */
    /* Codes_SRS_LOCK_10_003: [Lock_Init on error shall return NULL ] */
    // xSemaphoreCreateMutex is the obvious choice, but it returns a recursive
    // sync object, which we do not want for the lock adapter.
    SemaphoreHandle_t result = xSemaphoreCreateBinary();
    if (result != NULL)
    {
        int gv = xSemaphoreGive(result);
        if (gv != pdTRUE)
        {
            LogError("xSemaphoreGive failed after create.");
            vSemaphoreDelete(result);
            result = NULL;
        }
    }
    else
    {
        LogError("xSemaphoreCreateBinary failed.");
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
        vSemaphoreDelete((SemaphoreHandle_t)handle);
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
        int rv = xSemaphoreTake((SemaphoreHandle_t)handle, portMAX_DELAY);
        switch (rv)
        {
            case pdTRUE:
                /* Codes_SRS_LOCK_10_005: [Lock on success shall return LOCK_OK] */
                result = LOCK_OK;
                break;
            default:
                LogError("xSemaphoreTake failed.");
                /* Codes_SRS_LOCK_10_006: [Lock on error shall return LOCK_ERROR] */
                result = LOCK_ERROR;
                break;
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
        int rv = xSemaphoreGive((SemaphoreHandle_t)handle);
        switch (rv)
        {
            case pdTRUE:
                /* Codes_SRS_LOCK_10_005: [Lock on success shall return LOCK_OK] */
                result = LOCK_OK;
                break;
            default:
                LogError("xSemaphoreGive failed.");
                /* Codes_SRS_LOCK_10_006: [Lock on error shall return LOCK_ERROR] */
                result = LOCK_ERROR;
                break;
        }
    }

    return result;
}
