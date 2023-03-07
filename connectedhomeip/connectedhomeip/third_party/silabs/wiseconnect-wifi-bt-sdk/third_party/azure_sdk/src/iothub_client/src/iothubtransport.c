// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <signal.h>
#include <stddef.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "internal/iothubtransport.h"
#include "iothub_client_core.h"
#include "internal/iothub_client_private.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/vector.h"

#include "internal/iothubtransport.h"
#include "internal/iothub_client_private.h"
#include "iothub_transport_ll.h"
#include "iothub_client_core.h"

typedef struct TRANSPORT_HANDLE_DATA_TAG
{
    TRANSPORT_LL_HANDLE transportLLHandle;
    THREAD_HANDLE workerThreadHandle;
    LOCK_HANDLE lockHandle;
    sig_atomic_t stopThread;
    TRANSPORT_PROVIDER_FIELDS;
    VECTOR_HANDLE clients;
    LOCK_HANDLE clientsLockHandle;
    IOTHUB_CLIENT_MULTIPLEXED_DO_WORK clientDoWork;
} TRANSPORT_HANDLE_DATA;

/* Used for Unit test */
const size_t IoTHubTransport_ThreadTerminationOffset = offsetof(TRANSPORT_HANDLE_DATA, stopThread);

TRANSPORT_HANDLE IoTHubTransport_Create(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol, const char* iotHubName, const char* iotHubSuffix)
{
    TRANSPORT_HANDLE_DATA *result;
    TRANSPORT_CALLBACKS_INFO transport_cb;

    if (protocol == NULL || iotHubName == NULL || iotHubSuffix == NULL)
    {
        /*Codes_SRS_IOTHUBTRANSPORT_17_002: [ If protocol is NULL, this function shall return NULL. ]*/
        /*Codes_SRS_IOTHUBTRANSPORT_17_003: [ If iotHubName is NULL, this function shall return NULL. ]*/
        /*Codes_SRS_IOTHUBTRANSPORT_17_004: [ If iotHubSuffix is NULL, this function shall return NULL. ]*/
        LogError("Invalid NULL argument, protocol [%p], name [%p], suffix [%p].", protocol, iotHubName, iotHubSuffix);
        result = NULL;
    }
    else if (IoTHubClientCore_LL_GetTransportCallbacks(&transport_cb) != 0)
    {
        LogError("Failure getting transport callbacks");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBTRANSPORT_17_032: [ IoTHubTransport_Create shall allocate memory for the transport data. ]*/
        result = (TRANSPORT_HANDLE_DATA*)malloc(sizeof(TRANSPORT_HANDLE_DATA));
        if (result == NULL)
        {
            /*Codes_SRS_IOTHUBTRANSPORT_17_040: [ If memory allocation fails, IoTHubTransport_Create shall return NULL. ]*/
            LogError("Transport handle was not allocated.");
        }
        else
        {
            TRANSPORT_PROVIDER * transportProtocol = (TRANSPORT_PROVIDER*)(protocol());
            IOTHUB_CLIENT_CONFIG upperConfig;
            upperConfig.deviceId = NULL;
            upperConfig.deviceKey = NULL;
            upperConfig.iotHubName = iotHubName;
            upperConfig.iotHubSuffix = iotHubSuffix;
            upperConfig.protocol = protocol;
            upperConfig.protocolGatewayHostName = NULL;

            IOTHUBTRANSPORT_CONFIG transportLLConfig;
            memset(&transportLLConfig, 0, sizeof(IOTHUBTRANSPORT_CONFIG));
            transportLLConfig.upperConfig = &upperConfig;
            transportLLConfig.waitingToSend = NULL;

            /*Codes_SRS_IOTHUBTRANSPORT_17_005: [ IoTHubTransport_Create shall create the lower layer transport by calling the protocol's IoTHubTransport_Create function. ]*/
            result->transportLLHandle = transportProtocol->IoTHubTransport_Create(&transportLLConfig, &transport_cb, NULL);
            if (result->transportLLHandle == NULL)
            {
                /*Codes_SRS_IOTHUBTRANSPORT_17_006: [ If the creation of the transport fails, IoTHubTransport_Create shall return NULL. ]*/
                LogError("Lower Layer transport not created.");
                free(result);
                result = NULL;
            }
            else
            {
                /*Codes_SRS_IOTHUBTRANSPORT_17_007: [ IoTHubTransport_Create shall create the transport lock by Calling Lock_Init. ]*/
                result->lockHandle = Lock_Init();
                if (result->lockHandle == NULL)
                {
                    /*Codes_SRS_IOTHUBTRANSPORT_17_008: [ If the lock creation fails, IoTHubTransport_Create shall return NULL. ]*/
                    LogError("transport Lock not created.");
                    transportProtocol->IoTHubTransport_Destroy(result->transportLLHandle);
                    free(result);
                    result = NULL;
                }
                else if ((result->clientsLockHandle = Lock_Init()) == NULL)
                {
                    LogError("clients Lock not created.");
                    Lock_Deinit(result->lockHandle);
                    transportProtocol->IoTHubTransport_Destroy(result->transportLLHandle);
                    free(result);
                    result = NULL;
                }
                else
                {
                    /*Codes_SRS_IOTHUBTRANSPORT_17_038: [ IoTHubTransport_Create shall call VECTOR_Create to make a list of IOTHUB_CLIENT_CORE_HANDLE using this transport. ]*/
                    result->clients = VECTOR_create(sizeof(IOTHUB_CLIENT_CORE_HANDLE));
                    if (result->clients == NULL)
                    {
                        /*Codes_SRS_IOTHUBTRANSPORT_17_039: [ If the Vector creation fails, IoTHubTransport_Create shall return NULL. ]*/
                        /*Codes_SRS_IOTHUBTRANSPORT_17_009: [ IoTHubTransport_Create shall clean up any resources it creates if the function does not succeed. ]*/
                        LogError("clients list not created.");
                        Lock_Deinit(result->clientsLockHandle);
                        Lock_Deinit(result->lockHandle);
                        transportProtocol->IoTHubTransport_Destroy(result->transportLLHandle);
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBTRANSPORT_17_001: [ IoTHubTransport_Create shall return a non-NULL handle on success.]*/
                        result->stopThread = 1;
                        result->clientDoWork = NULL;
                        result->workerThreadHandle = NULL; /* create thread when work needs to be done */
                        result->IoTHubTransport_GetHostname = transportProtocol->IoTHubTransport_GetHostname;
                        result->IoTHubTransport_SetOption = transportProtocol->IoTHubTransport_SetOption;
                        result->IoTHubTransport_Create = transportProtocol->IoTHubTransport_Create;
                        result->IoTHubTransport_Destroy = transportProtocol->IoTHubTransport_Destroy;
                        result->IoTHubTransport_Register = transportProtocol->IoTHubTransport_Register;
                        result->IoTHubTransport_Unregister = transportProtocol->IoTHubTransport_Unregister;
                        result->IoTHubTransport_Subscribe = transportProtocol->IoTHubTransport_Subscribe;
                        result->IoTHubTransport_Unsubscribe = transportProtocol->IoTHubTransport_Unsubscribe;
                        result->IoTHubTransport_DoWork = transportProtocol->IoTHubTransport_DoWork;
                        result->IoTHubTransport_SetRetryPolicy = transportProtocol->IoTHubTransport_SetRetryPolicy;
                        result->IoTHubTransport_GetSendStatus = transportProtocol->IoTHubTransport_GetSendStatus;
                    }
                }
            }
        }
    }

    return result;
}

static void multiplexed_client_do_work(TRANSPORT_HANDLE_DATA* transportData)
{
    if (Lock(transportData->clientsLockHandle) != LOCK_OK)
    {
        LogError("failed to lock for multiplexed_client_do_work");
    }
    else
    {
        size_t numberOfClients;
        size_t iterator;

        numberOfClients = VECTOR_size(transportData->clients);
        for (iterator = 0; iterator < numberOfClients; iterator++)
        {
            IOTHUB_CLIENT_CORE_HANDLE* clientHandle = (IOTHUB_CLIENT_CORE_HANDLE*)VECTOR_element(transportData->clients, iterator);

            if (clientHandle != NULL)
            {
                transportData->clientDoWork(*clientHandle);
            }
        }

        if (Unlock(transportData->clientsLockHandle) != LOCK_OK)
        {
            LogError("failed to unlock on multiplexed_client_do_work");
        }
    }
}

static int transport_worker_thread(void* threadArgument)
{
    TRANSPORT_HANDLE_DATA* transportData = (TRANSPORT_HANDLE_DATA*)threadArgument;

    while (1)
    {
        /*Codes_SRS_IOTHUBTRANSPORT_17_030: [ All calls to lower layer transport DoWork shall be protected by the lock created in IoTHubTransport_Create. ]*/
        if (Lock(transportData->lockHandle) == LOCK_OK)
        {
            /*Codes_SRS_IOTHUBTRANSPORT_17_031: [ If acquiring the lock fails, lower layer transport DoWork shall not be called. ]*/
            if (transportData->stopThread)
            {
                /*Codes_SRS_IOTHUBTRANSPORT_17_028: [ The thread shall exit when IoTHubTransport_EndWorkerThread has been called for each clientHandle which invoked IoTHubTransport_StartWorkerThread. ]*/
                (void)Unlock(transportData->lockHandle);
                break;
            }
            else
            {
                (transportData->IoTHubTransport_DoWork)(transportData->transportLLHandle);

                (void)Unlock(transportData->lockHandle);
            }
        }

        multiplexed_client_do_work(transportData);

        /*Codes_SRS_IOTHUBTRANSPORT_17_029: [ The thread shall call lower layer transport DoWork every 1 ms. ]*/
        ThreadAPI_Sleep(1);
    }

    ThreadAPI_Exit(0);
    return 0;
}

static bool find_by_handle(const void* element, const void* value)
{
    /* data stored at element is device handle */
    const IOTHUB_CLIENT_CORE_HANDLE * guess = (const IOTHUB_CLIENT_CORE_HANDLE *)element;
    const IOTHUB_CLIENT_CORE_HANDLE match = (const IOTHUB_CLIENT_CORE_HANDLE)value;
    return (*guess == match);
}

static IOTHUB_CLIENT_RESULT start_worker_if_needed(TRANSPORT_HANDLE_DATA * transportData, IOTHUB_CLIENT_CORE_HANDLE clientHandle)
{
    IOTHUB_CLIENT_RESULT result;
    if (transportData->workerThreadHandle == NULL)
    {
        /*Codes_SRS_IOTHUBTRANSPORT_17_018: [ If the worker thread does not exist, IoTHubTransport_StartWorkerThread shall start the thread using ThreadAPI_Create. ]*/
        transportData->stopThread = 0;
        if (ThreadAPI_Create(&transportData->workerThreadHandle, transport_worker_thread, transportData) != THREADAPI_OK)
        {
            transportData->workerThreadHandle = NULL;
        }
    }
    if (transportData->workerThreadHandle != NULL)
    {
        if (Lock(transportData->clientsLockHandle) != LOCK_OK)
        {
            LogError("failed to lock for start_worker_if_needed");
            result = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBTRANSPORT_17_020: [ IoTHubTransport_StartWorkerThread shall search for IoTHubClient clientHandle in the list of IoTHubClient handles. ]*/
            bool addToList = ((VECTOR_size(transportData->clients) == 0) || (VECTOR_find_if(transportData->clients, find_by_handle, clientHandle) == NULL));
            if (addToList)
            {
                /*Codes_SRS_IOTHUBTRANSPORT_17_021: [ If handle is not found, then clientHandle shall be added to the list. ]*/
                if (VECTOR_push_back(transportData->clients, &clientHandle, 1) != 0)
                {
                    LogError("Failed adding device to list (VECTOR_push_back failed)");
                    /*Codes_SRS_IOTHUBTRANSPORT_17_042: [ If Adding to the client list fails, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_ERROR. ]*/
                    result = IOTHUB_CLIENT_ERROR;
                }
                else
                {
                    result = IOTHUB_CLIENT_OK;
                }
            }
            else
            {
                result = IOTHUB_CLIENT_OK;
            }

            if (Unlock(transportData->clientsLockHandle) != LOCK_OK)
            {
                LogError("failed to unlock on start_worker_if_needed");
            }
        }
    }
    else
    {
        result = IOTHUB_CLIENT_ERROR;
    }
    return result;
}

static void stop_worker_thread(TRANSPORT_HANDLE_DATA* transportData)
{
    /*Codes_SRS_IOTHUBTRANSPORT_17_043: [** IoTHubTransport_SignalEndWorkerThread shall signal the worker thread to end.*/
    if (Lock(transportData->lockHandle) != LOCK_OK)
    {
        // Need to setup a critical error function here to inform the user that an critical error
        // has occured.
        LogError("Unable to lock - will still attempt to end thread without thread safety");
        transportData->stopThread = 1;
    }
    else
    {
        transportData->stopThread = 1;
        (void)Unlock(transportData->lockHandle);
    }

}

static void wait_worker_thread(TRANSPORT_HANDLE_DATA * transportData)
{
    if (transportData->workerThreadHandle != NULL)
    {
        int res;
        /*Codes_SRS_IOTHUBTRANSPORT_17_027: [ If handle list is empty, IoTHubTransport_EndWorkerThread shall be joined. ]*/
        if (ThreadAPI_Join(transportData->workerThreadHandle, &res) != THREADAPI_OK)
        {
            LogError("ThreadAPI_Join failed");
        }
        else
        {
            transportData->workerThreadHandle = NULL;
        }
    }
}

static bool signal_end_worker_thread(TRANSPORT_HANDLE_DATA * transportData, IOTHUB_CLIENT_CORE_HANDLE clientHandle)
{
    bool okToJoin;

    if (Lock(transportData->clientsLockHandle) != LOCK_OK)
    {
        LogError("failed to lock for signal_end_worker_thread");
        okToJoin = false;
    }
    else
    {
        void* element = VECTOR_find_if(transportData->clients, find_by_handle, clientHandle);
        if (element != NULL)
        {
            /*Codes_SRS_IOTHUBTRANSPORT_17_026: [ IoTHubTransport_EndWorkerThread shall remove clientHandlehandle from handle list. ]*/
            VECTOR_erase(transportData->clients, element, 1);
        }
        /*Codes_SRS_IOTHUBTRANSPORT_17_025: [ If the worker thread does not exist, then IoTHubTransport_EndWorkerThread shall return. ]*/
        if (transportData->workerThreadHandle != NULL)
        {
            if (VECTOR_size(transportData->clients) == 0)
            {
                stop_worker_thread(transportData);
                okToJoin = true;
            }
            else
            {
                okToJoin = false;
            }
        }
        else
        {
            okToJoin = false;
        }

        if (Unlock(transportData->clientsLockHandle) != LOCK_OK)
        {
            LogError("failed to unlock on signal_end_worker_thread");
        }
    }
    return okToJoin;
}

void IoTHubTransport_Destroy(TRANSPORT_HANDLE transportHandle)
{
    /*Codes_SRS_IOTHUBTRANSPORT_17_011: [ IoTHubTransport_Destroy shall do nothing if transportHandle is NULL. ]*/
    if (transportHandle != NULL)
    {
        TRANSPORT_HANDLE_DATA * transportData = (TRANSPORT_HANDLE_DATA*)transportHandle;
        /*Codes_SRS_IOTHUBTRANSPORT_17_033: [ IoTHubTransport_Destroy shall lock the transport lock. ]*/
        stop_worker_thread(transportData);
        wait_worker_thread(transportData);
        /*Codes_SRS_IOTHUBTRANSPORT_17_010: [ IoTHubTransport_Destroy shall free all resources. ]*/
        Lock_Deinit(transportData->lockHandle);
        (transportData->IoTHubTransport_Destroy)(transportData->transportLLHandle);
        VECTOR_destroy(transportData->clients);
        Lock_Deinit(transportData->clientsLockHandle);
        free(transportHandle);
    }
}

LOCK_HANDLE IoTHubTransport_GetLock(TRANSPORT_HANDLE transportHandle)
{
    LOCK_HANDLE lock;
    if (transportHandle == NULL)
    {
        /*Codes_SRS_IOTHUBTRANSPORT_17_013: [ If transportHandle is NULL, IoTHubTransport_GetLock shall return NULL. ]*/
        lock = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBTRANSPORT_17_012: [ IoTHubTransport_GetLock shall return a handle to the transport lock. ]*/
        TRANSPORT_HANDLE_DATA * transportData = (TRANSPORT_HANDLE_DATA*)transportHandle;
        lock = transportData->lockHandle;
    }
    return lock;
}

TRANSPORT_LL_HANDLE IoTHubTransport_GetLLTransport(TRANSPORT_HANDLE transportHandle)
{
    TRANSPORT_LL_HANDLE llTransport;
    if (transportHandle == NULL)
    {
        /*Codes_SRS_IOTHUBTRANSPORT_17_015: [ If transportHandle is NULL, IoTHubTransport_GetLLTransport shall return NULL. ]*/
        llTransport = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBTRANSPORT_17_014: [ IoTHubTransport_GetLLTransport shall return a handle to the lower layer transport. ]*/
        TRANSPORT_HANDLE_DATA * transportData = (TRANSPORT_HANDLE_DATA*)transportHandle;
        llTransport = transportData->transportLLHandle;
    }
    return llTransport;
}

IOTHUB_CLIENT_RESULT IoTHubTransport_StartWorkerThread(TRANSPORT_HANDLE transportHandle, IOTHUB_CLIENT_CORE_HANDLE clientHandle, IOTHUB_CLIENT_MULTIPLEXED_DO_WORK muxDoWork)
{
    IOTHUB_CLIENT_RESULT result;
    if (transportHandle == NULL || clientHandle == NULL)
    {
        /*Codes_SRS_IOTHUBTRANSPORT_17_016: [ If transportHandle is NULL, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
        /*Codes_SRS_IOTHUBTRANSPORT_17_017: [ If clientHandle is NULL, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        TRANSPORT_HANDLE_DATA * transportData = (TRANSPORT_HANDLE_DATA*)transportHandle;

        if (transportData->clientDoWork == NULL)
        {
            transportData->clientDoWork = muxDoWork;
        }

        if ((result = start_worker_if_needed(transportData, clientHandle)) != IOTHUB_CLIENT_OK)
        {
            /*Codes_SRS_IOTHUBTRANSPORT_17_019: [ If thread creation fails, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_ERROR. */
            LogError("Unable to start thread safely");
        }
        else
        {
            /*Codes_SRS_IOTHUBTRANSPORT_17_022: [ Upon success, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_OK. ]*/
            result = IOTHUB_CLIENT_OK;
        }
    }
    return result;
}

bool IoTHubTransport_SignalEndWorkerThread(TRANSPORT_HANDLE transportHandle, IOTHUB_CLIENT_CORE_HANDLE clientHandle)
{
    bool okToJoin;
    /*Codes_SRS_IOTHUBTRANSPORT_17_023: [ If transportHandle is NULL, IoTHubTransport_EndWorkerThread shall return. ]*/
    /*Codes_SRS_IOTHUBTRANSPORT_17_024: [ If clientHandle is NULL, IoTHubTransport_EndWorkerThread shall return. ]*/
    if (!(transportHandle == NULL || clientHandle == NULL))
    {
        TRANSPORT_HANDLE_DATA * transportData = (TRANSPORT_HANDLE_DATA*)transportHandle;
        okToJoin = signal_end_worker_thread(transportData, clientHandle);
    }
    else
    {
        okToJoin = false;
    }
    return okToJoin;
}

void IoTHubTransport_JoinWorkerThread(TRANSPORT_HANDLE transportHandle, IOTHUB_CLIENT_CORE_HANDLE clientHandle)
{
    /*Codes_SRS_IOTHUBTRANSPORT_17_044: [ If transportHandle is NULL, IoTHubTransport_JoinWorkerThread shall do nothing. ]*/
    /*Codes_SRS_IOTHUBTRANSPORT_17_045: [ If clientHandle is NULL, IoTHubTransport_JoinWorkerThread shall do nothing. ]*/
    if (!(transportHandle == NULL || clientHandle == NULL))
    {
        TRANSPORT_HANDLE_DATA * transportData = (TRANSPORT_HANDLE_DATA*)transportHandle;
        /*Codes_SRS_IOTHUBTRANSPORT_17_027: [ The worker thread shall be joined. ]*/
        wait_worker_thread(transportData);
    }
}
