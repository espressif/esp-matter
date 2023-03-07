// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <ctype.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/sastoken.h"

#include "azure_uamqp_c/connection.h"
#include "azure_uamqp_c/message_receiver.h"
#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/messaging.h"
#include "azure_uamqp_c/sasl_mechanism.h"
#include "azure_uamqp_c/saslclientio.h"
#include "azure_uamqp_c/sasl_plain.h"
#include "azure_uamqp_c/cbs.h"
#include <signal.h>
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/lock.h"

#include "parson.h"

#include "iothub_messaging_ll.h"
#include "iothub_messaging.h"

typedef struct IOTHUB_MESSAGING_CLIENT_INSTANCE_TAG
{
    IOTHUB_MESSAGING_HANDLE IoTHubMessagingHandle;
    THREAD_HANDLE ThreadHandle;
    LOCK_HANDLE LockHandle;
    sig_atomic_t StopThread;
} IOTHUB_MESSAGING_CLIENT_INSTANCE;

static int ScheduleWork_Thread(void* threadArgument)
{
    IOTHUB_MESSAGING_CLIENT_INSTANCE* iotHubMessagingClientInstance = (IOTHUB_MESSAGING_CLIENT_INSTANCE*)threadArgument;

    while (1)
    {
        if (Lock(iotHubMessagingClientInstance->LockHandle) == LOCK_OK)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_041: [ The thread shall exit when all IoTHubServiceClients using the thread have had IoTHubMessaging_Destroy called. ]*/
            if (iotHubMessagingClientInstance->StopThread)
            {
                (void)Unlock(iotHubMessagingClientInstance->LockHandle);
                break; /*gets out of the thread*/
            }
            else
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_042: [ The thread created by IoTHubMessaging_SendAsync shall call IoTHubMessaging_LL_DoWork every 1 ms. ]*/
                /*Codes_SRS_IOTHUBMESSAGING_12_043: [ All calls to IoTHubMessaging_LL_DoWork shall be protected by the lock created in IoTHubMessaging_Create. ]*/
                IoTHubMessaging_LL_DoWork(iotHubMessagingClientInstance->IoTHubMessagingHandle);
                (void)Unlock(iotHubMessagingClientInstance->LockHandle);
            }
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_044: [ If acquiring the lock fails, `IoTHubMessaging_LL_DoWork` shall not be called. ]*/
            LogError("Lock failed, shall retry");
        }
        (void)ThreadAPI_Sleep(1);
    }

    ThreadAPI_Exit(0);
    return 0;
}

static IOTHUB_MESSAGING_RESULT StartWorkerThreadIfNeeded(IOTHUB_MESSAGING_CLIENT_INSTANCE* iotHubMessagingClientInstance)
{
    IOTHUB_MESSAGING_RESULT result;
    if (iotHubMessagingClientInstance->ThreadHandle == NULL)
    {
        iotHubMessagingClientInstance->StopThread = 0;
        if (ThreadAPI_Create(&iotHubMessagingClientInstance->ThreadHandle, ScheduleWork_Thread, iotHubMessagingClientInstance) != THREADAPI_OK)
        {
            LogError("ThreadAPI_Create failed");
            result = IOTHUB_MESSAGING_ERROR;
        }
        else
        {
            result = IOTHUB_MESSAGING_OK;
        }
    }
    else
    {
        result = IOTHUB_MESSAGING_OK;
    }
    return result;
}

IOTHUB_MESSAGING_CLIENT_HANDLE IoTHubMessaging_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle)
{
    IOTHUB_MESSAGING_CLIENT_INSTANCE* result;

    /*Codes_SRS_IOTHUBMESSAGING_12_001: [ IoTHubMessaging_Create shall verify the serviceClientHandle input parameter and if it is NULL then return NULL. ]*/
    if (serviceClientHandle == NULL)
    {
        LogError("serviceClientHandle input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_002: [ IoTHubMessaging_Create shall allocate a new IoTHubMessagingClient instance. ]*/
        if ((result = (IOTHUB_MESSAGING_CLIENT_INSTANCE*)malloc(sizeof(IOTHUB_MESSAGING_CLIENT_INSTANCE))) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_003: [If allocating memory for the new IoTHubMessagingClient instance fails, then IoTHubMessaging_Create shall return NULL. ]*/
            LogError("malloc failed for IoTHubMessagingClient");
            result = NULL;
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_004: [IoTHubMessaging_Create shall create a lock object to be used later for serializing IoTHubMessagingClient calls. ]*/
            result->LockHandle = Lock_Init();
            if (result->LockHandle == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_005: [If creating the lock fails, then IoTHubMessaging_Create shall return NULL. ]*/
                /*Codes_SRS_IOTHUBMESSAGING_12_008: [If IoTHubMessaging_Create fails, all resources allocated by it shall be freed. ]*/
                LogError("Lock_Init failed");
                free(result);
                result = NULL;
            }
            else
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_006: [IoTHubMessaging_Create shall instantiate a new IoTHubMessaging_LL instance by calling IoTHubMessaging_LL_Create and passing the serviceClientHandle argument. ]*/
                result->IoTHubMessagingHandle = IoTHubMessaging_LL_Create(serviceClientHandle);
                if (result->IoTHubMessagingHandle == NULL)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_007: [ If IoTHubMessaging_LL_Create fails, then IoTHubMessaging_Create shall return NULL. ]*/
                    /*Codes_SRS_IOTHUBMESSAGING_12_008: [If IoTHubMessaging_Create fails, all resources allocated by it shall be freed. ]*/
                    LogError("IoTHubMessaging_LL_Create failed");
                    Lock_Deinit(result->LockHandle);
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->StopThread = 0;
                    result->ThreadHandle = NULL;
                }
            }
        }
    }
    return (IOTHUB_MESSAGING_CLIENT_HANDLE)result;
}

void IoTHubMessaging_Destroy(IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle)
{
    /*Codes_SRS_IOTHUBMESSAGING_12_009: [ IoTHubMessaging_Destroy shall do nothing if parameter messagingClientHandle is NULL. ]*/
    if (messagingClientHandle == NULL)
    {
        LogError("messagingClientHandle input parameter is NULL");
    }
    else
    {
        IOTHUB_MESSAGING_CLIENT_INSTANCE* messagingClientInstance = (IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;

        /*Codes_SRS_IOTHUBMESSAGING_12_011: [ IoTHubMessaging_Destroy shall destroy IoTHubMessagingHandle by call IoTHubMessaging_LL_Destroy. ]*/
        IoTHubMessaging_LL_Destroy(messagingClientInstance->IoTHubMessagingHandle);

        /*Codes_SRS_IOTHUBMESSAGING_12_014: [ If the lock was allocated in IoTHubMessaging_Create, it shall be also freed. ]*/
        Lock_Deinit(messagingClientInstance->LockHandle);

        free(messagingClientInstance);
    }
}

IOTHUB_MESSAGING_RESULT IoTHubMessaging_Open(IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle, IOTHUB_OPEN_COMPLETE_CALLBACK openCompleteCallback, void* userContextCallback)
{
    IOTHUB_MESSAGING_RESULT result;

    if (messagingClientHandle == NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_015: [ If messagingClientHandle is NULL, IoTHubMessaging_Open shall return IOTHUB_MESSAGING_INVALID_ARG. ]*/
        LogError("NULL messagingClientHandle");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    else
    {
        IOTHUB_MESSAGING_CLIENT_INSTANCE* iotHubMessagingClientInstance = (IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;

        /*Codes_SRS_IOTHUBMESSAGING_12_016: [ IoTHubMessaging_Open shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
        if (Lock(iotHubMessagingClientInstance->LockHandle) != LOCK_OK)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_017: [ If acquiring the lock fails, IoTHubMessaging_Open shall return IOTHUB_MESSAGING_ERROR. ]*/
            LogError("Could not acquire lock");
            result = IOTHUB_MESSAGING_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_018: [ IoTHubMessaging_Open shall call IoTHubMessaging_LL_Open, while passing the IOTHUB_MESSAGING_HANDLE handle created by IoTHubMessaging_Create and the parameters openCompleteCallback and userContextCallback. ]*/
            /*Codes_SRS_IOTHUBMESSAGING_12_019: [ When IoTHubMessaging_LL_Open is called, IoTHubMessaging_Open shall return the result of IoTHubMessaging_LL_Open. ]*/
            result = IoTHubMessaging_LL_Open(messagingClientHandle->IoTHubMessagingHandle, openCompleteCallback, userContextCallback);

            /*Codes_SRS_IOTHUBMESSAGING_12_016: [ IoTHubMessaging_Open shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
            (void)Unlock(iotHubMessagingClientInstance->LockHandle);
        }
    }

    return result;
}

void IoTHubMessaging_Close(IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle)
{
    if (messagingClientHandle == NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_021: [ If messagingClientHandle is NULL, IoTHubMessaging_Close shall return IOTHUB_MESSAGING_INVALID_ARG. ]*/
        LogError("NULL messagingClientHandle");
    }
    else
    {
        IOTHUB_MESSAGING_CLIENT_INSTANCE* iotHubMessagingClientInstance = (IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;

        /*Codes_SRS_IOTHUBMESSAGING_12_022: [ IoTHubMessaging_Close shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
        if (Lock(iotHubMessagingClientInstance->LockHandle) != LOCK_OK)
        {
            LogError("Could not acquire lock");
            iotHubMessagingClientInstance->StopThread = 1; /*setting it even when Lock fails*/
        }
        else
        {
            iotHubMessagingClientInstance->StopThread = 1;

            /*Codes_SRS_IOTHUBMESSAGING_12_022: [ IoTHubMessaging_Close shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
            (void)Unlock(iotHubMessagingClientInstance->LockHandle);
        }

        if (iotHubMessagingClientInstance->ThreadHandle != NULL)
        {
            int res;
            /*Codes_SRS_IOTHUBMESSAGING_12_013: [ The thread created as part of executing IoTHubMessaging_SendAsync shall be joined. ]*/
            if (ThreadAPI_Join(iotHubMessagingClientInstance->ThreadHandle, &res) != THREADAPI_OK)
            {
                LogError("ThreadAPI_Join failed");
            }
        }

        /*Codes_SRS_IOTHUBMESSAGING_12_024: [ IoTHubMessaging_Close shall call IoTHubMessaging_LL_Close, while passing the IOTHUB_MESSAGING_HANDLE handle created by IoTHubMessaging_Create ]*/
        IoTHubMessaging_LL_Close(messagingClientHandle->IoTHubMessagingHandle);
    }
}

IOTHUB_MESSAGING_RESULT IoTHubMessaging_SetFeedbackMessageCallback(IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle, IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK feedbackMessageReceivedCallback, void* userContextCallback)
{
    IOTHUB_MESSAGING_RESULT result;

    if (messagingClientHandle == NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_027: [ If messagingClientHandle is NULL, IoTHubMessaging_SetFeedbackMessageCallback shall return IOTHUB_MESSAGING_INVALID_ARG. ]*/
        LogError("NULL messagingClientHandle");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    else
    {
        IOTHUB_MESSAGING_CLIENT_INSTANCE* iotHubMessagingClientInstance = (IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;

        /*Codes_SRS_IOTHUBMESSAGING_12_028: [ IoTHubMessaging_SetFeedbackMessageCallback shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
        if (Lock(iotHubMessagingClientInstance->LockHandle) != LOCK_OK)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_029: [ If acquiring the lock fails, IoTHubMessaging_SetFeedbackMessageCallback shall return IOTHUB_MESSAGING_ERROR. ]*/
            LogError("Could not acquire lock");
            result = IOTHUB_MESSAGING_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_030: [ IoTHubMessaging_SetFeedbackMessageCallback shall call IoTHubMessaging_LL_SetFeedbackMessageCallback, while passing the IOTHUB_MESSAGING_HANDLE handle created by IoTHubMessaging_Create, feedbackMessageReceivedCallback and userContextCallback ]*/
            /*Codes_SRS_IOTHUBMESSAGING_12_031: [ When IoTHubMessaging_LL_SetFeedbackMessageCallback is called, IoTHubMessaging_SetFeedbackMessageCallback shall return the result of IoTHubMessaging_LL_SetFeedbackMessageCallback. ]*/
            result = IoTHubMessaging_LL_SetFeedbackMessageCallback(messagingClientHandle->IoTHubMessagingHandle, feedbackMessageReceivedCallback, userContextCallback);

            /*Codes_SRS_IOTHUBMESSAGING_12_032: [ IoTHubMessaging_SetFeedbackMessageCallback shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
            (void)Unlock(iotHubMessagingClientInstance->LockHandle);
        }
    }

    return result;
}

IOTHUB_MESSAGING_RESULT IoTHubMessaging_SendAsync(IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle, const char* deviceId, IOTHUB_MESSAGE_HANDLE message, IOTHUB_SEND_COMPLETE_CALLBACK sendCompleteCallback, void* userContextCallback)
{
    IOTHUB_MESSAGING_RESULT result;

    if (messagingClientHandle == NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_033: [ If messagingClientHandle is NULL, IoTHubMessaging_SendAsync shall return IOTHUB_MESSAGING_INVALID_ARG. ]*/
        LogError("NULL iothubClientHandle");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    else
    {
        IOTHUB_MESSAGING_CLIENT_INSTANCE* iotHubMessagingClientInstance = (IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;

        /*Codes_SRS_IOTHUBMESSAGING_12_034: [ IoTHubMessaging_SendAsync shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
        if (Lock(iotHubMessagingClientInstance->LockHandle) != LOCK_OK)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_035: [ If acquiring the lock fails, IoTHubMessaging_SendAsync shall return IOTHUB_MESSAGING_ERROR. ]*/
            LogError("Could not acquire lock");
            result = IOTHUB_MESSAGING_INVALID_ARG;
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_036: [ IoTHubClient_SendEventAsync shall start the worker thread if it was not previously started. ]*/
            if ((result = StartWorkerThreadIfNeeded(iotHubMessagingClientInstance)) != IOTHUB_MESSAGING_OK)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_037: [ If starting the thread fails, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_ERROR. ]*/
                LogError("Could not start worker thread");
                result = IOTHUB_MESSAGING_ERROR;
            }
            else
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_SendAsync shall call IoTHubMessaging_LL_Send, while passing the IOTHUB_MESSAGING_HANDLE handle created by IoTHubClient_Create and the parameters deviceId, message, sendCompleteCallback and userContextCallback.*/
                /*Codes_SRS_IOTHUBMESSAGING_12_039: [ When IoTHubMessaging_LL_Send is called, IoTHubMessaging_SendAsync shall return the result of IoTHubMessaging_LL_Send. ]*/
                result = IoTHubMessaging_LL_Send(iotHubMessagingClientInstance->IoTHubMessagingHandle, deviceId, message, sendCompleteCallback, userContextCallback);
            }

            /*Codes_SRS_IOTHUBMESSAGING_12_040: [ IoTHubClient_SendEventAsync shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
            (void)Unlock(iotHubMessagingClientInstance->LockHandle);
        }
    }

    return result;
}

IOTHUB_MESSAGING_RESULT IoTHubMessaging_SetTrustedCert(IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle, const char* trusted_cert)
{
    IOTHUB_MESSAGING_RESULT result;

    if (messagingClientHandle == NULL)
    {
        LogError("NULL iothubClientHandle");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    else
    {
        IOTHUB_MESSAGING_CLIENT_INSTANCE* iotHubMessagingClientInstance = (IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;

        if (Lock(iotHubMessagingClientInstance->LockHandle) != LOCK_OK)
        {
            LogError("Could not acquire lock");
            result = IOTHUB_MESSAGING_ERROR;
        }
        else
        {
            result = IoTHubMessaging_LL_SetTrustedCert(iotHubMessagingClientInstance->IoTHubMessagingHandle, trusted_cert);
            (void)Unlock(iotHubMessagingClientInstance->LockHandle);
        }
    }
    return result;
}
