// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define _DEFAULT_SOURCE

#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/threadapi.h"

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#ifdef TI_RTOS
#include <ti/sysbios/knl/Task.h>
#else
#include <unistd.h>
#endif

#include <pthread.h>
#include <time.h>
#include "azure_c_shared_utility/xlogging.h"

MU_DEFINE_ENUM_STRINGS(THREADAPI_RESULT, THREADAPI_RESULT_VALUES);

typedef struct THREAD_INSTANCE_TAG
{
    pthread_t Pthread_handle;
    THREAD_START_FUNC ThreadStartFunc;
    void* Arg;
} THREAD_INSTANCE;

static void* ThreadWrapper(void* threadInstanceArg)
{
    THREAD_INSTANCE* threadInstance = (THREAD_INSTANCE*)threadInstanceArg;
    int result = threadInstance->ThreadStartFunc(threadInstance->Arg);
    return (void*)(intptr_t)result;
}

THREADAPI_RESULT ThreadAPI_Create(THREAD_HANDLE* threadHandle, THREAD_START_FUNC func, void* arg)
{
    THREADAPI_RESULT result;

    if ((threadHandle == NULL) ||
        (func == NULL))
    {
        result = THREADAPI_INVALID_ARG;
        LogError("(result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(THREADAPI_RESULT, result));
    }
    else
    {
        THREAD_INSTANCE* threadInstance = malloc(sizeof(THREAD_INSTANCE));
        if (threadInstance == NULL)
        {
            result = THREADAPI_NO_MEMORY;
            LogError("(result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(THREADAPI_RESULT, result));
        }
        else
        {
            threadInstance->ThreadStartFunc = func;
            threadInstance->Arg = arg;
            int createResult = pthread_create(&threadInstance->Pthread_handle, NULL, ThreadWrapper, threadInstance);
            switch (createResult)
            {
            default:
                free(threadInstance);

                result = THREADAPI_ERROR;
                LogError("(result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(THREADAPI_RESULT, result));
                break;

            case 0:
                *threadHandle = threadInstance;
                result = THREADAPI_OK;
                break;

            case EAGAIN:
                free(threadInstance);

                result = THREADAPI_NO_MEMORY;
                LogError("(result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(THREADAPI_RESULT, result));
                break;
            }
        }
    }

    return result;
}

THREADAPI_RESULT ThreadAPI_Join(THREAD_HANDLE threadHandle, int* res)
{
    THREADAPI_RESULT result;

    THREAD_INSTANCE* threadInstance = (THREAD_INSTANCE*)threadHandle;
    if (threadInstance == NULL)
    {
        result = THREADAPI_INVALID_ARG;
        LogError("(result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(THREADAPI_RESULT, result));
    }
    else
    {
        void* threadResult;
        if (pthread_join(threadInstance->Pthread_handle, &threadResult) != 0)
        {
            result = THREADAPI_ERROR;
            LogError("(result = %" PRI_MU_ENUM ")", MU_ENUM_VALUE(THREADAPI_RESULT, result));
        }
        else
        {
            if (res != NULL)
            {
                *res = (int)(intptr_t)threadResult;
            }

            result = THREADAPI_OK;
        }

        free(threadInstance);
    }

    return result;
}

void ThreadAPI_Exit(int res)
{
    pthread_exit((void*)(intptr_t)res);
}

void ThreadAPI_Sleep(unsigned int milliseconds)
{
#ifdef TI_RTOS
    Task_sleep(milliseconds);
#else
    time_t seconds = milliseconds / 1000;
    long nsRemainder = (milliseconds % 1000) * 1000000;
    struct timespec timeToSleep = { seconds, nsRemainder };
    (void)nanosleep(&timeToSleep, NULL);
#endif
}
