// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include <signal.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/xlogging.h"

#include "azure_prov_client/prov_device_ll_client.h"
#include "azure_prov_client/prov_device_client.h"
#include "azure_c_shared_utility/vector.h"

#define DO_WORK_FREQ_DEFAULT 1

typedef struct PROV_DEVICE_INSTANCE_TAG
{
    PROV_DEVICE_LL_HANDLE ProvDeviceLLHandle;
    THREAD_HANDLE ThreadHandle;
    LOCK_HANDLE LockHandle;
    sig_atomic_t StopThread;
    uint16_t do_work_freq_ms;
} PROV_DEVICE_INSTANCE;

static int ScheduleWork_Thread(void* threadArgument)
{
    PROV_DEVICE_INSTANCE* prov_device_instance = (PROV_DEVICE_INSTANCE*)threadArgument;
    uint16_t sleeptime_in_ms = DO_WORK_FREQ_DEFAULT;

    srand((unsigned int)get_time(NULL));

    while (1)
    {
        if (Lock(prov_device_instance->LockHandle) == LOCK_OK)
        {
            if (prov_device_instance->StopThread)
            {
                (void)Unlock(prov_device_instance->LockHandle);
                break; /*gets out of the thread*/
            }
            else
            {
                Prov_Device_LL_DoWork(prov_device_instance->ProvDeviceLLHandle);
                sleeptime_in_ms = prov_device_instance->do_work_freq_ms; // Update the sleepval within the locked thread. 
                (void)Unlock(prov_device_instance->LockHandle);
            }
        }
        else
        {
            LogError("Lock failed, shall retry");
        }
        (void)ThreadAPI_Sleep(sleeptime_in_ms);
    }

    ThreadAPI_Exit(0);
    return 0;
}

static PROV_DEVICE_RESULT StartWorkerThreadIfNeeded(PROV_DEVICE_INSTANCE* prov_device_instance)
{
    PROV_DEVICE_RESULT result;
    if (prov_device_instance->ThreadHandle == NULL)
    {
        prov_device_instance->StopThread = 0;
        if (ThreadAPI_Create(&prov_device_instance->ThreadHandle, ScheduleWork_Thread, prov_device_instance) != THREADAPI_OK)
        {
            LogError("ThreadAPI_Create failed");
            result = PROV_DEVICE_RESULT_ERROR;
        }
        else
        {
            result = PROV_DEVICE_RESULT_OK;
        }
    }
    else
    {
        result = PROV_DEVICE_RESULT_OK;
    }
    return result;
}

PROV_DEVICE_HANDLE Prov_Device_Create(const char* uri, const char* id_scope, PROV_DEVICE_TRANSPORT_PROVIDER_FUNCTION protocol)
{
    PROV_DEVICE_INSTANCE* result;

    /* Codes_SRS_PROV_DEVICE_CLIENT_12_001: [ If any of the input parameter is NULL `Prov_Device_Create` shall return NULL.] */
    if (uri == NULL || id_scope == NULL || protocol == NULL)
    {
        LogError("Invalid parameter specified uri: %p, id_scope: %p, protocol: %p", uri, id_scope, protocol);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_PROV_DEVICE_CLIENT_12_002: [ The function shall allocate memory for PROV_DEVICE_INSTANCE data structure. ] */
        result = (PROV_DEVICE_INSTANCE*)malloc(sizeof(PROV_DEVICE_INSTANCE));
        if (result == NULL)
        {
            /* Codes_SRS_PROV_DEVICE_CLIENT_12_003: [ If the memory allocation failed the function shall return NULL. ] */
            LogError("Unable to allocate Instance Info");
        }
        else
        {
            memset(result, 0, sizeof(PROV_DEVICE_INSTANCE) );
            /* Codes_SRS_PROV_DEVICE_CLIENT_12_004: [ The function shall initialize the Lock. ] */
            result->LockHandle = Lock_Init();
            if (result->LockHandle == NULL)
            {
                /* Codes_SRS_PROV_DEVICE_CLIENT_12_005: [ If the Lock initialization failed the function shall clean up the all resources and return NULL. ] */
                LogError("Lock_Init failed");
                free(result);
                result = NULL;
            }
            else
            {
                /* Codes_SRS_PROV_DEVICE_CLIENT_12_006: [ The function shall call the LL layer Prov_Device_LL_Create function and return with it's result. ] */
                result->ProvDeviceLLHandle = Prov_Device_LL_Create(uri, id_scope, protocol);
                if (result->ProvDeviceLLHandle == NULL)
                {
                    /* Codes_SRS_PROV_DEVICE_CLIENT_12_025: [ If the Client initialization failed the function shall clean up the all resources and return NULL. ] */
                    LogError("Prov_Device_LL_Create failed");
                    free(result);
                    result = NULL;
                }
                else
                {
                    /* Codes_SRS_PROV_DEVICE_CLIENT_12_007: [ The function shall initialize the result datastructure. ] */
                    result->ThreadHandle = NULL;
                    result->StopThread = 0;
                    result->do_work_freq_ms = DO_WORK_FREQ_DEFAULT;
                }
            }
        }
    }

    return result;
}

void Prov_Device_Destroy(PROV_DEVICE_HANDLE prov_device_handle)
{
    /* Codes_SRS_PROV_DEVICE_CLIENT_12_008: [ If the input parameter is NULL `Prov_Device_Destroy` shall return. ] */
    if (prov_device_handle == NULL)
    {
        LogError("NULL prov_device_handle");
    }
    else
    {
        PROV_DEVICE_INSTANCE* prov_device_instance = (PROV_DEVICE_INSTANCE*)prov_device_handle;

        if (Lock(prov_device_handle->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_PROV_DEVICE_CLIENT_12_009: [ The function shall check the Lock status and if it is not OK set the thread signal to stop. ] */
            LogError("Could not acquire lock");
            prov_device_handle->StopThread = 1; /*setting it even when Lock fails*/
        }
        else
        {
            /* Codes_SRS_PROV_DEVICE_CLIENT_12_010: [ The function shall check the Lock status and if it is OK set the thread signal to stop and unlock the Lock. ] */
            prov_device_handle->StopThread = 1;

            (void)Unlock(prov_device_handle->LockHandle);
        }

        /* Codes_SRS_PROV_DEVICE_CLIENT_12_011: [ If there is a running worker thread the function shall call join to finish. ] */
        if (prov_device_handle->ThreadHandle != NULL)
        {
            int res;
            if (ThreadAPI_Join(prov_device_handle->ThreadHandle, &res) != THREADAPI_OK)
            {
                LogError("ThreadAPI_Join failed");
            }
        }

        /* Codes_SRS_PROV_DEVICE_CLIENT_12_012: [ The function shall call the LL layer Prov_Device_LL_Destroy with the given handle. ] */
        Prov_Device_LL_Destroy(prov_device_instance->ProvDeviceLLHandle);

        /* Codes_SRS_PROV_DEVICE_CLIENT_12_013: [ The function shall free the Lock resource with de-init. ] */
        Lock_Deinit(prov_device_instance->LockHandle);

        /* Codes_SRS_PROV_DEVICE_CLIENT_12_014: [ The function shall free the device handle resource. ] */
        free(prov_device_instance);
    }
}

PROV_DEVICE_RESULT Prov_Device_Register_Device(PROV_DEVICE_HANDLE prov_device_handle, PROV_DEVICE_CLIENT_REGISTER_DEVICE_CALLBACK register_callback, void* user_context, PROV_DEVICE_CLIENT_REGISTER_STATUS_CALLBACK register_status_callback, void* status_user_context)
{
    PROV_DEVICE_RESULT result;

    /* Codes_SRS_PROV_DEVICE_CLIENT_12_015: [ If the prov_device_handle or register_callback input parameter is NULL `Prov_Device_Register_Device` shall return with invalid argument error. ] */
    if (prov_device_handle == NULL || register_callback == NULL)
    {
        LogError("Invalid parameter specified prov_device_handle: %p, register_callback: %p", prov_device_handle, register_callback);
        result = PROV_DEVICE_RESULT_INVALID_ARG;
    }
    else
    {
        PROV_DEVICE_INSTANCE* prov_device_instance = (PROV_DEVICE_INSTANCE*)prov_device_handle;

        /* Codes_SRS_PROV_DEVICE_CLIENT_12_018: [ The function shall try to lock the Lock. ] */
        if (Lock(prov_device_instance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_PROV_DEVICE_CLIENT_12_019: [ If the locking failed the function shall return with error. ] */
            LogError("Could not acquire lock");
            result = PROV_DEVICE_RESULT_ERROR;
        }
        else
        {
            /* Codes_SRS_PROV_DEVICE_CLIENT_12_020: [ The function shall call the LL layer Prov_Device_LL_Register_Device with the given parameters and return with the result. ] */
            result = Prov_Device_LL_Register_Device(prov_device_instance->ProvDeviceLLHandle, register_callback, user_context, register_status_callback, status_user_context);

            /* Codes_SRS_PROV_DEVICE_CLIENT_12_021: [ The function shall unlock the Lock. ] */
            (void)Unlock(prov_device_instance->LockHandle);

            if (result == PROV_DEVICE_RESULT_OK)
            {
                /* Codes_SRS_PROV_DEVICE_CLIENT_12_016: [ The function shall start a worker thread with the device instance. ] */
                if ((result = StartWorkerThreadIfNeeded(prov_device_instance)) != PROV_DEVICE_RESULT_OK)
                {
                    /* Codes_SRS_PROV_DEVICE_CLIENT_12_017: [ If the thread initialization failed the function shall return error. ] */
                    LogError("Could not start worker thread");
                    result = PROV_DEVICE_RESULT_ERROR;
                }
            }
        }
    }

    return result;
}

PROV_DEVICE_RESULT Prov_Device_SetOption(PROV_DEVICE_HANDLE prov_device_handle, const char* optionName, const void* value)
{
    PROV_DEVICE_RESULT result;

    /* Codes_SRS_PROV_DEVICE_CLIENT_12_022: [ If any of the input parameter is NULL `Prov_Device_SetOption` shall return with invalid argument error. ] */
    if (
        (prov_device_handle == NULL) || 
        (optionName == NULL) || 
        (value == NULL)
        )
    {
        result = PROV_DEVICE_RESULT_INVALID_ARG;
        LogError("Invalid parameter specified prov_device_handle: %p, optionName: %p, value: %p", prov_device_handle, optionName, value);
    }
    else 
    {
        /* Codes_SRS_PROV_DEVICE_CLIENT_12_023: [ The function shall call the LL layer Prov_Device_LL_SetOption with the given parameters and return with the result. ] */
        PROV_DEVICE_INSTANCE* prov_device_instance = (PROV_DEVICE_INSTANCE*)prov_device_handle;

        /* Codes_SRS_PROV_DEVICE_CLIENT_41_002: [ `Prov_Device_SetOption` shall be made thread-safe by using the lock created in `Prov_Device_Create`. ] */
        if (Lock(prov_device_instance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_PROV_DEVICE_CLIENT_41_003: [ If acquiring the lock fails, `Prov_Device_SetOption` shall return `IOTHUB_CLIENT_ERROR`. ] */
            result = PROV_DEVICE_RESULT_ERROR;
            LogError("Could not acquire lock");
        }
        else 
        {
            /* Codes_SRS_PROV_DEVICE_CLIENT_41_001: [ If parameter `optionName` is `OPTION_DO_WORK_FREQUENCY_IN_MS` then `IoTHubClientCore_SetOption` shall set `do_work_freq_ms` parameter of `prov_device_instance` ] */
            if (strcmp(PROV_OPTION_DO_WORK_FREQUENCY_IN_MS, optionName) == 0)
            {
                prov_device_instance->do_work_freq_ms = *((uint16_t *)value);
                result = PROV_DEVICE_RESULT_OK;  
            }
            else 
            {
                result = Prov_Device_LL_SetOption(prov_device_instance->ProvDeviceLLHandle, optionName, value);
                if (result != PROV_DEVICE_RESULT_OK)
                {
                    LogError("Prov_Device_LL_SetOption failed");
                }
            }
            (void)Unlock(prov_device_instance->LockHandle);
        }
    }
    return result;
}

const char* Prov_Device_GetVersionString(void)
{
    /* Codes_SRS_PROV_DEVICE_CLIENT_12_024: [ The function shall call the LL layer Prov_Device_LL_GetVersionString and return with the result. ] */
    return Prov_Device_LL_GetVersionString();
}

PROV_DEVICE_RESULT Prov_Device_Set_Provisioning_Payload(PROV_DEVICE_HANDLE handle, const char* json)
{
    PROV_DEVICE_RESULT result;
    if (handle == NULL)
    {
        LogError("Invalid parameter specified handle: %p", handle);
        result = PROV_DEVICE_RESULT_INVALID_ARG;
    }
    else
    {
        result = Prov_Device_LL_Set_Provisioning_Payload(handle->ProvDeviceLLHandle, json);
    }
    return result;
}

const char* Prov_Device_Get_Provisioning_Payload(PROV_DEVICE_HANDLE handle)
{
    const char* result;
    if (handle == NULL)
    {
        LogError("Invalid parameter specified handle: %p", handle);
        result = NULL;
    }
    else
    {
        result = Prov_Device_LL_Get_Provisioning_Payload(handle->ProvDeviceLLHandle);
    }
    return result;
}

