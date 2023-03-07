// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/xlogging.h"

/*Codes_SRS_THREADAPI_FREERTOS_30_001: [ The threadapi_freertos shall implement the method ThreadAPI_Sleep defined in threadapi.h ]*/
#include "azure_c_shared_utility/threadapi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*Codes_SRS_THREADAPI_FREERTOS_30_002: [ The ThreadAPI_Sleep shall receive a time in milliseconds. ]*/
/*Codes_SRS_THREADAPI_FREERTOS_30_003: [ The ThreadAPI_Sleep shall stop the thread for the specified time. ]*/
void ThreadAPI_Sleep(unsigned int milliseconds)
{
    vTaskDelay((milliseconds * CONFIG_FREERTOS_HZ) / 1000);
}

/*Codes_SRS_THREADAPI_FREERTOS_30_004: [ FreeRTOS is not guaranteed to support threading, so ThreadAPI_Create shall return THREADAPI_ERROR. ]*/
THREADAPI_RESULT ThreadAPI_Create(THREAD_HANDLE* threadHandle, THREAD_START_FUNC func, void* arg)
{
    (void)threadHandle;
    (void)func;
    (void)arg;
    LogError("FreeRTOS does not support multi-threading.");
    return THREADAPI_ERROR;
}

/*Codes_SRS_THREADAPI_FREERTOS_30_005: [ FreeRTOS is not guaranteed to support threading, so ThreadAPI_Join shall return THREADAPI_ERROR. ]*/
THREADAPI_RESULT ThreadAPI_Join(THREAD_HANDLE threadHandle, int* res)
{
    (void)threadHandle;
    (void)res;
    LogError("FreeRTOS does not support multi-threading.");
    return THREADAPI_ERROR;
}

/*Codes_SRS_THREADAPI_FREERTOS_30_006: [ FreeRTOS is not guaranteed to support threading, so ThreadAPI_Exit shall do nothing. ]*/
void ThreadAPI_Exit(int res)
{
    (void)res;
    LogError("FreeRTOS does not support multi-threading.");
}
