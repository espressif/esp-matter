// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/gballoc.h"

#include <signal.h>
#include <stddef.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "iothub_client_core.h"
#include "iothub_client_core_ll.h"
#include "internal/iothubtransport.h"
#include "internal/iothub_client_private.h"
#include "internal/iothubtransport.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/vector.h"
#include "iothub_client_options.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/agenttime.h"


#define DO_WORK_FREQ_DEFAULT 1
#define DO_WORK_MAXIMUM_ALLOWED_FREQUENCY 100

struct IOTHUB_QUEUE_CONTEXT_TAG;

typedef struct IOTHUB_CLIENT_CORE_INSTANCE_TAG
{
    IOTHUB_CLIENT_CORE_LL_HANDLE IoTHubClientLLHandle;
    TRANSPORT_HANDLE TransportHandle;
    THREAD_HANDLE ThreadHandle;
    LOCK_HANDLE LockHandle;
    sig_atomic_t StopThread;
    SINGLYLINKEDLIST_HANDLE httpWorkerThreadInfoList; /*list containing HTTPWORKER_THREAD_INFO*/
    int created_with_transport_handle;
    VECTOR_HANDLE saved_user_callback_list;
    IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK desired_state_callback;
    IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK connection_status_callback;
    IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC device_method_callback;
    IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK inbound_device_method_callback;
    IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC message_callback;
    struct IOTHUB_QUEUE_CONTEXT_TAG* devicetwin_user_context;
    struct IOTHUB_QUEUE_CONTEXT_TAG* connection_status_user_context;
    struct IOTHUB_QUEUE_CONTEXT_TAG* message_user_context;
    struct IOTHUB_QUEUE_CONTEXT_TAG* method_user_context;
    tickcounter_ms_t do_work_freq_ms;
    tickcounter_ms_t currentMessageTimeout;
} IOTHUB_CLIENT_CORE_INSTANCE;

typedef enum HTTPWORKER_THREAD_TYPE_TAG
{
    HTTPWORKER_THREAD_UPLOAD_TO_BLOB,
    HTTPWORKER_THREAD_INVOKE_METHOD
} HTTPWORKER_THREAD_TYPE;

typedef struct UPLOADTOBLOB_SAVED_DATA_TAG
{
    unsigned char* source;
    size_t size;
    IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK iotHubClientFileUploadCallback;
}UPLOADTOBLOB_SAVED_DATA;

typedef struct UPLOADTOBLOB_MULTIBLOCK_SAVED_DATA_TAG
{
    IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK getDataCallback;
    IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX getDataCallbackEx;
}UPLOADTOBLOB_MULTIBLOCK_SAVED_DATA;

typedef struct INVOKE_METHOD_SAVED_DATA_TAG
{
    const char* deviceId;
    const char* moduleId;
    const char* methodName;
    const char* methodPayload;
    unsigned int timeout;
    IOTHUB_METHOD_INVOKE_CALLBACK methodInvokeCallback;
} INVOKE_METHOD_SAVED_DATA;

typedef struct HTTPWORKER_THREAD_INFO_TAG
{
    HTTPWORKER_THREAD_TYPE workerThreadType;
    char* destinationFileName;
    THREAD_HANDLE threadHandle;
    LOCK_HANDLE lockGarbage;
    int canBeGarbageCollected; /*flag indicating that the structure can be freed because the thread deadling with it finished*/
    IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle;
    void* context;
    UPLOADTOBLOB_SAVED_DATA uploadBlobSavedData;
    INVOKE_METHOD_SAVED_DATA invokeMethodSavedData;
    UPLOADTOBLOB_MULTIBLOCK_SAVED_DATA uploadBlobMultiblockSavedData;
}HTTPWORKER_THREAD_INFO;

#define USER_CALLBACK_TYPE_VALUES       \
    CALLBACK_TYPE_DEVICE_TWIN,          \
    CALLBACK_TYPE_EVENT_CONFIRM,        \
    CALLBACK_TYPE_REPORTED_STATE,       \
    CALLBACK_TYPE_CONNECTION_STATUS,    \
    CALLBACK_TYPE_DEVICE_METHOD,        \
    CALLBACK_TYPE_INBOUD_DEVICE_METHOD, \
    CALLBACK_TYPE_MESSAGE,              \
    CALLBACK_TYPE_INPUTMESSAGE

MU_DEFINE_ENUM_WITHOUT_INVALID(USER_CALLBACK_TYPE, USER_CALLBACK_TYPE_VALUES)
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(USER_CALLBACK_TYPE, USER_CALLBACK_TYPE_VALUES)

typedef struct DEVICE_TWIN_CALLBACK_INFO_TAG
{
    DEVICE_TWIN_UPDATE_STATE update_state;
    unsigned char* payLoad;
    size_t size;
    IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK userCallback;
    void* userContext;
} DEVICE_TWIN_CALLBACK_INFO;

typedef struct EVENT_CONFIRM_CALLBACK_INFO_TAG
{
    IOTHUB_CLIENT_CONFIRMATION_RESULT confirm_result;
    IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback;
} EVENT_CONFIRM_CALLBACK_INFO;

typedef struct REPORTED_STATE_CALLBACK_INFO_TAG
{
    int status_code;
    IOTHUB_CLIENT_REPORTED_STATE_CALLBACK reportedStateCallback;
} REPORTED_STATE_CALLBACK_INFO;

typedef struct CONNECTION_STATUS_CALLBACK_INFO_TAG
{
    IOTHUB_CLIENT_CONNECTION_STATUS connection_status;
    IOTHUB_CLIENT_CONNECTION_STATUS_REASON status_reason;
} CONNECTION_STATUS_CALLBACK_INFO;

typedef struct METHOD_CALLBACK_INFO_TAG
{
    STRING_HANDLE method_name;
    BUFFER_HANDLE payload;
    METHOD_HANDLE method_id;
} METHOD_CALLBACK_INFO;

typedef struct INPUTMESSAGE_CALLBACK_INFO_TAG
{
    IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC eventHandlerCallback;
    MESSAGE_CALLBACK_INFO* message_cb_info;
} INPUTMESSAGE_CALLBACK_INFO;

typedef struct USER_CALLBACK_INFO_TAG
{
    USER_CALLBACK_TYPE type;
    void* userContextCallback;
    union IOTHUB_CALLBACK
    {
        DEVICE_TWIN_CALLBACK_INFO dev_twin_cb_info;
        EVENT_CONFIRM_CALLBACK_INFO event_confirm_cb_info;
        REPORTED_STATE_CALLBACK_INFO reported_state_cb_info;
        CONNECTION_STATUS_CALLBACK_INFO connection_status_cb_info;
        METHOD_CALLBACK_INFO method_cb_info;
        MESSAGE_CALLBACK_INFO* message_cb_info;
        INPUTMESSAGE_CALLBACK_INFO inputmessage_cb_info;
    } iothub_callback;
} USER_CALLBACK_INFO;

typedef struct IOTHUB_QUEUE_CONTEXT_TAG
{
    IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientHandle;
    void* userContextCallback;
    union IOTHUB_CALLBACK_FUNCTION
    {
        IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback;
        IOTHUB_CLIENT_REPORTED_STATE_CALLBACK reportedStateCallback;
    } callbackFunction;
} IOTHUB_QUEUE_CONTEXT;

typedef struct IOTHUB_QUEUE_CONSOLIDATED_CONTEXT_TAG
{
    IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientHandle;

    union USER_CALLBACK_TAG
    {
        IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK getTwin;
    } userCallback;

    void* userContext;
} IOTHUB_QUEUE_CONSOLIDATED_CONTEXT;

typedef struct IOTHUB_INPUTMESSAGE_CALLBACK_CONTEXT_TAG
{
    IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle;
    IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC eventHandlerCallback;
    void* userContextCallback;
} IOTHUB_INPUTMESSAGE_CALLBACK_CONTEXT;

/*used by unittests only*/
const size_t IoTHubClientCore_ThreadTerminationOffset = offsetof(IOTHUB_CLIENT_CORE_INSTANCE, StopThread);

typedef enum CREATE_HUB_INSTANCE_TYPE_TAG
{
    CREATE_HUB_INSTANCE_FROM_CONNECTION_STRING,
    CREATE_HUB_INSTANCE_FROM_EDGE_ENVIRONMENT,
    CREATE_HUB_INSTANCE_FROM_TRANSPORT,
    CREATE_HUB_INSTANCE_FROM_CLIENT_CONFIG,
    CREATE_HUB_INSTANCE_FROM_DEVICE_AUTH
} CREATE_HUB_INSTANCE_TYPE;

static void freeHttpWorkerThreadInfo(HTTPWORKER_THREAD_INFO* threadInfo)
{
    Lock_Deinit(threadInfo->lockGarbage);
    if (threadInfo->workerThreadType == HTTPWORKER_THREAD_UPLOAD_TO_BLOB)
    {
        free(threadInfo->uploadBlobSavedData.source);
        free(threadInfo->destinationFileName);
    }
    else if (threadInfo->workerThreadType == HTTPWORKER_THREAD_INVOKE_METHOD)
    {
        free((char*)threadInfo->invokeMethodSavedData.deviceId);
        free((char*)threadInfo->invokeMethodSavedData.moduleId);
        free((char*)threadInfo->invokeMethodSavedData.methodName);
        free((char*)threadInfo->invokeMethodSavedData.methodPayload);
    }

    free(threadInfo);
}

/*this function is called from _Destroy and from ScheduleWork_Thread to join finished blobUpload threads and free that memory*/
static void garbageCollectorImpl(IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance)
{
    /*see if any savedData structures can be disposed of*/
    /*Codes_SRS_IOTHUBCLIENT_02_072: [ All threads marked as disposable (upon completion of a file upload) shall be joined and the data structures build for them shall be freed. ]*/
    LIST_ITEM_HANDLE item = singlylinkedlist_get_head_item(iotHubClientInstance->httpWorkerThreadInfoList);
    while (item != NULL)
    {
        HTTPWORKER_THREAD_INFO* threadInfo = (HTTPWORKER_THREAD_INFO*)singlylinkedlist_item_get_value(item);
        LIST_ITEM_HANDLE old_item = item;
        item = singlylinkedlist_get_next_item(item);

        if (threadInfo == NULL || Lock(threadInfo->lockGarbage) != LOCK_OK)
        {
            LogError("unable to Lock");
        }
        else
        {
            if (threadInfo->canBeGarbageCollected == 1)
            {
                int notUsed;
                if (ThreadAPI_Join(threadInfo->threadHandle, &notUsed) != THREADAPI_OK)
                {
                    LogError("unable to ThreadAPI_Join");
                }
                (void)singlylinkedlist_remove(iotHubClientInstance->httpWorkerThreadInfoList, old_item);

                if (Unlock(threadInfo->lockGarbage) != LOCK_OK)
                {
                    LogError("unable to unlock after locking");
                }
                freeHttpWorkerThreadInfo(threadInfo);
            }
            else
            {
                if (Unlock(threadInfo->lockGarbage) != LOCK_OK)
                {
                    LogError("unable to unlock after locking");
                }
            }
        }
    }
}


static bool iothub_ll_message_callback(MESSAGE_CALLBACK_INFO* messageData, void* userContextCallback)
{
    bool result;
    IOTHUB_QUEUE_CONTEXT* queue_context = (IOTHUB_QUEUE_CONTEXT*)userContextCallback;
    if (queue_context == NULL)
    {
        LogError("invalid parameter userContextCallback(NULL)");
        result = false;
    }
    else
    {
        USER_CALLBACK_INFO queue_cb_info;
        queue_cb_info.type = CALLBACK_TYPE_MESSAGE;
        queue_cb_info.userContextCallback = queue_context->userContextCallback;
        queue_cb_info.iothub_callback.message_cb_info = messageData;
        if (VECTOR_push_back(queue_context->iotHubClientHandle->saved_user_callback_list, &queue_cb_info, 1) == 0)
        {
            result = true;
        }
        else
        {
            LogError("message callback vector push failed.");
            result = false;
        }
    }
    return result;
}

static bool iothub_ll_inputmessage_callback(MESSAGE_CALLBACK_INFO* message_cb_info, void* userContextCallback)
{
    bool result;
    IOTHUB_INPUTMESSAGE_CALLBACK_CONTEXT *inputMessageCallbackContext = (IOTHUB_INPUTMESSAGE_CALLBACK_CONTEXT *)userContextCallback;
    if (inputMessageCallbackContext == NULL)
    {
        LogError("invalid parameter userContextCallback(NULL)");
        result = false;
    }
    else
    {
        USER_CALLBACK_INFO queue_cb_info;
        queue_cb_info.type = CALLBACK_TYPE_INPUTMESSAGE;
        queue_cb_info.userContextCallback = inputMessageCallbackContext->userContextCallback;
        queue_cb_info.iothub_callback.inputmessage_cb_info.eventHandlerCallback = inputMessageCallbackContext->eventHandlerCallback;
        queue_cb_info.iothub_callback.inputmessage_cb_info.message_cb_info = message_cb_info;

        if (VECTOR_push_back(inputMessageCallbackContext->iotHubClientHandle->saved_user_callback_list, &queue_cb_info, 1) == 0)
        {
            result = true;
        }
        else
        {
            LogError("message callback vector push failed.");
            result = false;
        }
    }

    return result;
}

static int make_method_calback_queue_context(USER_CALLBACK_INFO* queue_cb_info, const char* method_name, const unsigned char* payload, size_t size, METHOD_HANDLE method_id, IOTHUB_QUEUE_CONTEXT* queue_context)
{
    int result;
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_002: [ IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK shall copy the method_name and payload. ] */
    queue_cb_info->userContextCallback = queue_context->userContextCallback;
    queue_cb_info->iothub_callback.method_cb_info.method_id = method_id;
    if ((queue_cb_info->iothub_callback.method_cb_info.method_name = STRING_construct(method_name)) == NULL)
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [ If a failure is encountered IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK shall return a non-NULL value. ]*/
        LogError("STRING_construct failed");
        result = MU_FAILURE;
    }
    else
    {
        if ((queue_cb_info->iothub_callback.method_cb_info.payload = BUFFER_create(payload, size)) == NULL)
        {
            STRING_delete(queue_cb_info->iothub_callback.method_cb_info.method_name);
            /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [ If a failure is encountered IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK shall return a non-NULL value. ]*/
            LogError("BUFFER_create failed");
            result = MU_FAILURE;
        }
        else
        {
            if (VECTOR_push_back(queue_context->iotHubClientHandle->saved_user_callback_list, queue_cb_info, 1) == 0)
            {
                result = 0;
            }
            else
            {
                STRING_delete(queue_cb_info->iothub_callback.method_cb_info.method_name);
                BUFFER_delete(queue_cb_info->iothub_callback.method_cb_info.payload);
                /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [ If a failure is encountered IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK shall return a non-NULL value. ]*/
                LogError("VECTOR_push_back failed");
                result = MU_FAILURE;
            }
        }
    }
    return result;
}

static int iothub_ll_device_method_callback(const char* method_name, const unsigned char* payload, size_t size, METHOD_HANDLE method_id, void* userContextCallback)
{
    int result;
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_001: [ if userContextCallback is NULL, IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK shall return a nonNULL value. ] */
    if (userContextCallback == NULL)
    {
        LogError("invalid parameter userContextCallback(NULL)");
        result = MU_FAILURE;
    }
    else
    {
        IOTHUB_QUEUE_CONTEXT* queue_context = (IOTHUB_QUEUE_CONTEXT*)userContextCallback;

        USER_CALLBACK_INFO queue_cb_info;
        queue_cb_info.type = CALLBACK_TYPE_DEVICE_METHOD;

        result = make_method_calback_queue_context(&queue_cb_info, method_name, payload, size, method_id, queue_context);
        if (result != 0)
        {
            LogError("construction of method calback queue context failed");
            result = MU_FAILURE;
        }
    }
    return result;
}

static int iothub_ll_inbound_device_method_callback(const char* method_name, const unsigned char* payload, size_t size, METHOD_HANDLE method_id, void* userContextCallback)
{
    int result;
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_001: [ if userContextCallback is NULL, IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK shall return a nonNULL value. ] */
    if (userContextCallback == NULL)
    {
        LogError("invalid parameter userContextCallback(NULL)");
        result = MU_FAILURE;
    }
    else
    {
        IOTHUB_QUEUE_CONTEXT* queue_context = (IOTHUB_QUEUE_CONTEXT*)userContextCallback;

        USER_CALLBACK_INFO queue_cb_info;
        queue_cb_info.type = CALLBACK_TYPE_INBOUD_DEVICE_METHOD;

        result = make_method_calback_queue_context(&queue_cb_info, method_name, payload, size, method_id, queue_context);
        if (result != 0)
        {
            LogError("construction of method calback queue context failed");
            result = MU_FAILURE;
        }
    }
    return result;
}

static void iothub_ll_connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContextCallback)
{
    IOTHUB_QUEUE_CONTEXT* queue_context = (IOTHUB_QUEUE_CONTEXT*)userContextCallback;
    if (queue_context != NULL)
    {
        USER_CALLBACK_INFO queue_cb_info;
        queue_cb_info.type = CALLBACK_TYPE_CONNECTION_STATUS;
        queue_cb_info.userContextCallback = queue_context->userContextCallback;
        queue_cb_info.iothub_callback.connection_status_cb_info.status_reason = reason;
        queue_cb_info.iothub_callback.connection_status_cb_info.connection_status = result;
        if (VECTOR_push_back(queue_context->iotHubClientHandle->saved_user_callback_list, &queue_cb_info, 1) != 0)
        {
            LogError("connection status callback vector push failed.");
        }
    }
}

static void iothub_ll_event_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    IOTHUB_QUEUE_CONTEXT* queue_context = (IOTHUB_QUEUE_CONTEXT*)userContextCallback;
    if (queue_context != NULL)
    {
        USER_CALLBACK_INFO queue_cb_info;
        queue_cb_info.type = CALLBACK_TYPE_EVENT_CONFIRM;
        queue_cb_info.userContextCallback = queue_context->userContextCallback;
        queue_cb_info.iothub_callback.event_confirm_cb_info.confirm_result = result;
        queue_cb_info.iothub_callback.event_confirm_cb_info.eventConfirmationCallback = queue_context->callbackFunction.eventConfirmationCallback;
        if (VECTOR_push_back(queue_context->iotHubClientHandle->saved_user_callback_list, &queue_cb_info, 1) != 0)
        {
            LogError("event confirm callback vector push failed.");
        }
        free(queue_context);
    }
}

static void iothub_ll_reported_state_callback(int status_code, void* userContextCallback)
{
    IOTHUB_QUEUE_CONTEXT* queue_context = (IOTHUB_QUEUE_CONTEXT*)userContextCallback;
    if (queue_context != NULL)
    {
        USER_CALLBACK_INFO queue_cb_info;
        queue_cb_info.type = CALLBACK_TYPE_REPORTED_STATE;
        queue_cb_info.userContextCallback = queue_context->userContextCallback;
        queue_cb_info.iothub_callback.reported_state_cb_info.status_code = status_code;
        queue_cb_info.iothub_callback.reported_state_cb_info.reportedStateCallback = queue_context->callbackFunction.reportedStateCallback;
        if (VECTOR_push_back(queue_context->iotHubClientHandle->saved_user_callback_list, &queue_cb_info, 1) != 0)
        {
            LogError("reported state callback vector push failed.");
        }
        free(queue_context);
    }
}

static void iothub_ll_device_twin_callback(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback)
{
    IOTHUB_QUEUE_CONTEXT* queue_context = (IOTHUB_QUEUE_CONTEXT*)userContextCallback;
    if (queue_context != NULL)
    {
        int push_to_vector;

        USER_CALLBACK_INFO queue_cb_info;
        queue_cb_info.type = CALLBACK_TYPE_DEVICE_TWIN;
        queue_cb_info.userContextCallback = queue_context->userContextCallback;
        queue_cb_info.iothub_callback.dev_twin_cb_info.update_state = update_state;
        queue_cb_info.iothub_callback.dev_twin_cb_info.userCallback = NULL;
        queue_cb_info.iothub_callback.dev_twin_cb_info.userContext = NULL;

        if (payLoad == NULL)
        {
            queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad = NULL;
            queue_cb_info.iothub_callback.dev_twin_cb_info.size = 0;
            push_to_vector = 0;
        }
        else
        {
            queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad = (unsigned char*)malloc(size);
            if (queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad == NULL)
            {
                LogError("failure allocating payload in device twin callback.");
                queue_cb_info.iothub_callback.dev_twin_cb_info.size = 0;
                push_to_vector = MU_FAILURE;
            }
            else
            {
                (void)memcpy(queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad, payLoad, size);
                queue_cb_info.iothub_callback.dev_twin_cb_info.size = size;
                push_to_vector = 0;
            }
        }
        if (push_to_vector == 0)
        {
            if (VECTOR_push_back(queue_context->iotHubClientHandle->saved_user_callback_list, &queue_cb_info, 1) != 0)
            {
                if (queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad != NULL)
                {
                    free(queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad);
                }
                LogError("device twin callback userContextCallback vector push failed.");
            }
        }
    }
    else
    {
        LogError("device twin callback userContextCallback NULL");
    }
}

static void iothub_ll_get_device_twin_async_callback(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback)
{
    IOTHUB_QUEUE_CONSOLIDATED_CONTEXT* queue_context = (IOTHUB_QUEUE_CONSOLIDATED_CONTEXT*)userContextCallback;

    if (queue_context != NULL)
    {
        USER_CALLBACK_INFO queue_cb_info;
        queue_cb_info.type = CALLBACK_TYPE_DEVICE_TWIN;
        queue_cb_info.userContextCallback = queue_context->userContext;
        queue_cb_info.iothub_callback.dev_twin_cb_info.update_state = update_state;
        queue_cb_info.iothub_callback.dev_twin_cb_info.userCallback = queue_context->userCallback.getTwin;
        queue_cb_info.iothub_callback.dev_twin_cb_info.userContext = queue_context->userContext;

        if (payLoad == NULL)
        {
            queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad = NULL;
            queue_cb_info.iothub_callback.dev_twin_cb_info.size = 0;
        }
        else
        {
            queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad = (unsigned char*)malloc(size);

            if (queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad == NULL)
            {
                LogError("Failure allocating payload in get device twin callback.");
                queue_cb_info.iothub_callback.dev_twin_cb_info.size = 0;
            }
            else
            {
                (void)memcpy(queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad, payLoad, size);
                queue_cb_info.iothub_callback.dev_twin_cb_info.size = size;
            }
        }

        if (VECTOR_push_back(queue_context->iotHubClientHandle->saved_user_callback_list, &queue_cb_info, 1) != 0)
        {
            LogError("device twin callback userContextCallback vector push failed.");

            if (queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad != NULL)
            {
                free(queue_cb_info.iothub_callback.dev_twin_cb_info.payLoad);
            }
        }

        free(queue_context);
    }
    else
    {
        LogError("Get device twin callback userContextCallback NULL");
    }
}

static void dispatch_user_callbacks(IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance, VECTOR_HANDLE call_backs)
{
    size_t callbacks_length = VECTOR_size(call_backs);
    size_t index;

    IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK desired_state_callback = NULL;
    IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK connection_status_callback = NULL;
    IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC device_method_callback = NULL;
    IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK inbound_device_method_callback = NULL;
    IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC message_callback = NULL;
    IOTHUB_CLIENT_CORE_HANDLE message_user_context_handle = NULL;
    IOTHUB_CLIENT_CORE_HANDLE method_user_context_handle = NULL;

    // Make a local copy of these callbacks, as we don't run with a lock held and iotHubClientInstance may change mid-run.
    if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
    {
        LogError("failed locking for dispatch_user_callbacks");
    }
    else
    {
        desired_state_callback = iotHubClientInstance->desired_state_callback;
        connection_status_callback = iotHubClientInstance->connection_status_callback;
        device_method_callback = iotHubClientInstance->device_method_callback;
        inbound_device_method_callback = iotHubClientInstance->inbound_device_method_callback;
        message_callback = iotHubClientInstance->message_callback;
        if (iotHubClientInstance->method_user_context)
        {
            method_user_context_handle = iotHubClientInstance->method_user_context->iotHubClientHandle;
        }
        if (iotHubClientInstance->message_user_context)
        {
            message_user_context_handle = iotHubClientInstance->message_user_context->iotHubClientHandle;
        }

        (void)Unlock(iotHubClientInstance->LockHandle);
    }


    for (index = 0; index < callbacks_length; index++)
    {
        USER_CALLBACK_INFO* queued_cb = (USER_CALLBACK_INFO*)VECTOR_element(call_backs, index);
        if (queued_cb == NULL)
        {
            LogError("VECTOR_element at index %zd is NULL.", index);
        }
        else
        {
            switch (queued_cb->type)
            {
            case CALLBACK_TYPE_DEVICE_TWIN:
            {
                // Callback if for GetTwinAsync
                if (queued_cb->iothub_callback.dev_twin_cb_info.userCallback)
                {
                    queued_cb->iothub_callback.dev_twin_cb_info.userCallback(
                        queued_cb->iothub_callback.dev_twin_cb_info.update_state,
                        queued_cb->iothub_callback.dev_twin_cb_info.payLoad,
                        queued_cb->iothub_callback.dev_twin_cb_info.size,
                        queued_cb->iothub_callback.dev_twin_cb_info.userContext
                    );
                }
                // Callback if for Desired properties.
                else if (desired_state_callback)
                {
                    desired_state_callback(queued_cb->iothub_callback.dev_twin_cb_info.update_state, queued_cb->iothub_callback.dev_twin_cb_info.payLoad, queued_cb->iothub_callback.dev_twin_cb_info.size, queued_cb->userContextCallback);
                }

                if (queued_cb->iothub_callback.dev_twin_cb_info.payLoad)
                {
                    free(queued_cb->iothub_callback.dev_twin_cb_info.payLoad);
                }
                break;
            }
            case CALLBACK_TYPE_EVENT_CONFIRM:
                if (queued_cb->iothub_callback.event_confirm_cb_info.eventConfirmationCallback)
                {
                    queued_cb->iothub_callback.event_confirm_cb_info.eventConfirmationCallback(queued_cb->iothub_callback.event_confirm_cb_info.confirm_result, queued_cb->userContextCallback);
                }
                break;
            case CALLBACK_TYPE_REPORTED_STATE:
                if (queued_cb->iothub_callback.reported_state_cb_info.reportedStateCallback)
                {
                    queued_cb->iothub_callback.reported_state_cb_info.reportedStateCallback(queued_cb->iothub_callback.reported_state_cb_info.status_code, queued_cb->userContextCallback);
                }
                break;
            case CALLBACK_TYPE_CONNECTION_STATUS:
                if (connection_status_callback)
                {
                    connection_status_callback(queued_cb->iothub_callback.connection_status_cb_info.connection_status, queued_cb->iothub_callback.connection_status_cb_info.status_reason, queued_cb->userContextCallback);
                }
                break;
            case CALLBACK_TYPE_DEVICE_METHOD:
                if (device_method_callback)
                {
                    const char* method_name = STRING_c_str(queued_cb->iothub_callback.method_cb_info.method_name);
                    const unsigned char* payload = BUFFER_u_char(queued_cb->iothub_callback.method_cb_info.payload);
                    size_t payload_len = BUFFER_length(queued_cb->iothub_callback.method_cb_info.payload);

                    unsigned char* payload_resp = NULL;
                    size_t response_size = 0;
                    int status = device_method_callback(method_name, payload, payload_len, &payload_resp, &response_size, queued_cb->userContextCallback);

                    if (payload_resp && (response_size > 0))
                    {
                        IOTHUB_CLIENT_RESULT result = IoTHubClientCore_DeviceMethodResponse(method_user_context_handle, queued_cb->iothub_callback.method_cb_info.method_id, (const unsigned char*)payload_resp, response_size, status);
                        if (result != IOTHUB_CLIENT_OK)
                        {
                            LogError("IoTHubClientCore_LL_DeviceMethodResponse failed");
                        }
                    }

                    BUFFER_delete(queued_cb->iothub_callback.method_cb_info.payload);
                    STRING_delete(queued_cb->iothub_callback.method_cb_info.method_name);

                    if (payload_resp)
                    {
                        free(payload_resp);
                    }
                }
                break;
            case CALLBACK_TYPE_INBOUD_DEVICE_METHOD:
                if (inbound_device_method_callback)
                {
                    const char* method_name = STRING_c_str(queued_cb->iothub_callback.method_cb_info.method_name);
                    const unsigned char* payload = BUFFER_u_char(queued_cb->iothub_callback.method_cb_info.payload);
                    size_t payload_len = BUFFER_length(queued_cb->iothub_callback.method_cb_info.payload);

                    inbound_device_method_callback(method_name, payload, payload_len, queued_cb->iothub_callback.method_cb_info.method_id, queued_cb->userContextCallback);

                    BUFFER_delete(queued_cb->iothub_callback.method_cb_info.payload);
                    STRING_delete(queued_cb->iothub_callback.method_cb_info.method_name);
                }
                break;
            case CALLBACK_TYPE_MESSAGE:
                if (message_callback && message_user_context_handle)
                {
                    IOTHUBMESSAGE_DISPOSITION_RESULT disposition = message_callback(queued_cb->iothub_callback.message_cb_info->messageHandle, queued_cb->userContextCallback);

                    if (Lock(message_user_context_handle->LockHandle) == LOCK_OK)
                    {
                        IOTHUB_CLIENT_RESULT result = IoTHubClientCore_LL_SendMessageDisposition(message_user_context_handle->IoTHubClientLLHandle, queued_cb->iothub_callback.message_cb_info, disposition);
                        (void)Unlock(message_user_context_handle->LockHandle);
                        if (result != IOTHUB_CLIENT_OK)
                        {
                            LogError("IoTHubClientCore_LL_SendMessageDisposition failed");
                        }
                    }
                    else
                    {
                        LogError("Lock failed");
                    }
                }
                break;

                case CALLBACK_TYPE_INPUTMESSAGE:
                {
                    const INPUTMESSAGE_CALLBACK_INFO *inputmessage_cb_info = &queued_cb->iothub_callback.inputmessage_cb_info;
                    IOTHUBMESSAGE_DISPOSITION_RESULT disposition = inputmessage_cb_info->eventHandlerCallback(inputmessage_cb_info->message_cb_info->messageHandle, queued_cb->userContextCallback);

                    if (Lock(iotHubClientInstance->LockHandle) == LOCK_OK)
                    {
                        IOTHUB_CLIENT_RESULT result = IoTHubClientCore_LL_SendMessageDisposition(iotHubClientInstance->IoTHubClientLLHandle, inputmessage_cb_info->message_cb_info, disposition);
                        (void)Unlock(iotHubClientInstance->LockHandle);
                        if (result != IOTHUB_CLIENT_OK)
                        {
                            LogError("IoTHubClient_LL_SendMessageDisposition failed");
                        }
                    }
                    else
                    {
                        LogError("Lock failed");
                    }
                }
                break;

            default:
                LogError("Invalid callback type '%s'", MU_ENUM_TO_STRING(USER_CALLBACK_TYPE, queued_cb->type));
                break;
            }
        }
    }
    VECTOR_destroy(call_backs);
}

static void ScheduleWork_Thread_ForMultiplexing(void* iotHubClientHandle)
{
    IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

    garbageCollectorImpl(iotHubClientInstance);
    if (Lock(iotHubClientInstance->LockHandle) == LOCK_OK)
    {
        VECTOR_HANDLE call_backs = VECTOR_move(iotHubClientInstance->saved_user_callback_list);
        (void)Unlock(iotHubClientInstance->LockHandle);

        if (call_backs == NULL)
        {
            LogError("Failed moving user callbacks");
        }
        else
        {
            dispatch_user_callbacks(iotHubClientInstance, call_backs);
        }
    }
    else
    {
        LogError("failed locking for ScheduleWork_Thread_ForMultiplexing");
    }
}

static int ScheduleWork_Thread(void* threadArgument)
{
    IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)threadArgument;
    unsigned int sleeptime_in_ms = DO_WORK_FREQ_DEFAULT;

    srand((unsigned int)get_time(NULL));

    while (1)
    {
        if (Lock(iotHubClientInstance->LockHandle) == LOCK_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_01_038: [ The thread shall exit when IoTHubClient_Destroy is called. ]*/
            if (iotHubClientInstance->StopThread)
            {
                (void)Unlock(iotHubClientInstance->LockHandle);
                break; /*gets out of the thread*/
            }
            else
            {
                /* Codes_SRS_IOTHUBCLIENT_01_037: [The thread created by IoTHubClient_SendEvent or IoTHubClient_SetMessageCallback shall call IoTHubClientCore_LL_DoWork every 1 ms by default.] */
                /* Codes_SRS_IOTHUBCLIENT_01_039: [All calls to IoTHubClientCore_LL_DoWork shall be protected by the lock created in IotHubClient_Create.] */
                IoTHubClientCore_LL_DoWork(iotHubClientInstance->IoTHubClientLLHandle);

                garbageCollectorImpl(iotHubClientInstance);
                VECTOR_HANDLE call_backs = VECTOR_move(iotHubClientInstance->saved_user_callback_list);
                sleeptime_in_ms = (unsigned int)iotHubClientInstance->do_work_freq_ms; // Update the sleepval within the locked thread.
                (void)Unlock(iotHubClientInstance->LockHandle);
                if (call_backs == NULL)
                {
                    LogError("VECTOR_move failed");
                }
                else
                {
                    dispatch_user_callbacks(iotHubClientInstance, call_backs);
                }


            }
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_01_040: [If acquiring the lock fails, IoTHubClientCore_LL_DoWork shall not be called.]*/
            /*no code, shall retry*/
        }
        /* Codes_SRS_IOTHUBCLIENT_041_02: [The thread shall sleep for a specified time in ms as provided through IoTHubClientCore_SetOption, with a default of 1 ms ] */
        (void)ThreadAPI_Sleep(sleeptime_in_ms);
    }

    ThreadAPI_Exit(0);
    return 0;
}

static IOTHUB_CLIENT_RESULT StartWorkerThreadIfNeeded(IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance)
{
    IOTHUB_CLIENT_RESULT result;
    if (iotHubClientInstance->TransportHandle == NULL)
    {
        if (iotHubClientInstance->ThreadHandle == NULL)
        {
            iotHubClientInstance->StopThread = 0;
            if (ThreadAPI_Create(&iotHubClientInstance->ThreadHandle, ScheduleWork_Thread, iotHubClientInstance) != THREADAPI_OK)
            {
                LogError("ThreadAPI_Create failed");
                iotHubClientInstance->ThreadHandle = NULL;
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
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_17_012: [ If the transport connection is shared, the thread shall be started by calling IoTHubTransport_StartWorkerThread. ]*/
        /*Codes_SRS_IOTHUBCLIENT_17_011: [ If the transport connection is shared, the thread shall be started by calling IoTHubTransport_StartWorkerThread*/
        result = IoTHubTransport_StartWorkerThread(iotHubClientInstance->TransportHandle, iotHubClientInstance, ScheduleWork_Thread_ForMultiplexing);
    }
    return result;
}

static IOTHUB_CLIENT_CORE_INSTANCE* create_iothub_instance(CREATE_HUB_INSTANCE_TYPE create_hub_instance_type, const IOTHUB_CLIENT_CONFIG* config, TRANSPORT_HANDLE transportHandle, const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol, const char* iothub_uri, const char* device_id)
{
    /* Codes_SRS_IOTHUBCLIENT_12_020: [** `IoTHubClient_CreateFromDeviceAuth` shall allocate a new `IoTHubClient` instance. **] */
    IOTHUB_CLIENT_CORE_INSTANCE* result = (IOTHUB_CLIENT_CORE_INSTANCE*)malloc(sizeof(IOTHUB_CLIENT_CORE_INSTANCE));
    (void)create_hub_instance_type;

    /* Codes_SRS_IOTHUBCLIENT_12_021: [** If allocating memory for the new `IoTHubClient` instance fails, then `IoTHubClient_CreateFromDeviceAuth` shall return `NULL`. **] */
    /* Codes_SRS_IOTHUBCLIENT_01_004: [If allocating memory for the new IoTHubClient instance fails, then IoTHubClient_Create shall return NULL.] */
    if (result != NULL)
    {
        memset((void *)result, 0, sizeof(IOTHUB_CLIENT_CORE_INSTANCE));

        /* Codes_SRS_IOTHUBCLIENT_41_02 [] */
        result->do_work_freq_ms = DO_WORK_FREQ_DEFAULT;
        /* Default currentMessageTimeout to NULL until it is set by SetOption */
        result->currentMessageTimeout = 0;

        /* Codes_SRS_IOTHUBCLIENT_01_029: [IoTHubClient_Create shall create a lock object to be used later for serializing IoTHubClient calls.] */
        if ((result->saved_user_callback_list = VECTOR_create(sizeof(USER_CALLBACK_INFO))) == NULL)
        {
            LogError("Failed creating VECTOR");
            free(result);
            result = NULL;
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_02_060: [ IoTHubClient_Create shall create a SINGLYLINKEDLIST_HANDLE containing THREAD_HANDLE (created by future calls to IoTHubClient_UploadToBlobAsync). ]*/
            if ((result->httpWorkerThreadInfoList = singlylinkedlist_create()) == NULL)
            {
                /*Codes_SRS_IOTHUBCLIENT_02_061: [ If creating the SINGLYLINKEDLIST_HANDLE fails then IoTHubClient_Create shall fail and return NULL. ]*/
                LogError("unable to singlylinkedlist_create");
                VECTOR_destroy(result->saved_user_callback_list);
                free(result);
                result = NULL;
            }
            else
            {
                result->TransportHandle = transportHandle;
                result->created_with_transport_handle = 0;
                if (config != NULL)
                {
                    if (transportHandle != NULL)
                    {
                        /*Codes_SRS_IOTHUBCLIENT_17_005: [ IoTHubClient_CreateWithTransport shall call IoTHubTransport_GetLock to get the transport lock to be used later for serializing IoTHubClient calls. ]*/
                        result->LockHandle = IoTHubTransport_GetLock(transportHandle);
                        if (result->LockHandle == NULL)
                        {
                            LogError("unable to IoTHubTransport_GetLock");
                            result->IoTHubClientLLHandle = NULL;
                        }
                        else
                        {
                            IOTHUB_CLIENT_DEVICE_CONFIG deviceConfig;
                            deviceConfig.deviceId = config->deviceId;
                            deviceConfig.deviceKey = config->deviceKey;
                            deviceConfig.protocol = config->protocol;
                            deviceConfig.deviceSasToken = config->deviceSasToken;

                            /*Codes_SRS_IOTHUBCLIENT_17_003: [ IoTHubClient_CreateWithTransport shall call IoTHubTransport_GetLLTransport on transportHandle to get lower layer transport. ]*/
                            deviceConfig.transportHandle = IoTHubTransport_GetLLTransport(transportHandle);
                            if (deviceConfig.transportHandle == NULL)
                            {
                                LogError("unable to IoTHubTransport_GetLLTransport");
                                result->IoTHubClientLLHandle = NULL;
                            }
                            else
                            {
                                if (Lock(result->LockHandle) != LOCK_OK)
                                {
                                    LogError("unable to Lock");
                                    result->IoTHubClientLLHandle = NULL;
                                }
                                else
                                {
                                    /*Codes_SRS_IOTHUBCLIENT_17_007: [ IoTHubClient_CreateWithTransport shall instantiate a new IoTHubClientCore_LL instance by calling IoTHubClientCore_LL_CreateWithTransport and passing the lower layer transport and config argument. ]*/
                                    result->IoTHubClientLLHandle = IoTHubClientCore_LL_CreateWithTransport(&deviceConfig);
                                    result->created_with_transport_handle = 1;
                                    if (Unlock(result->LockHandle) != LOCK_OK)
                                    {
                                        LogError("unable to Unlock");
                                        result->IoTHubClientLLHandle = NULL;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        result->LockHandle = Lock_Init();
                        if (result->LockHandle == NULL)
                        {
                            /* Codes_SRS_IOTHUBCLIENT_01_030: [If creating the lock fails, then IoTHubClient_Create shall return NULL.] */
                            /* Codes_SRS_IOTHUBCLIENT_01_031: [If IoTHubClient_Create fails, all resources allocated by it shall be freed.] */
                            LogError("Failure creating Lock object");
                            result->IoTHubClientLLHandle = NULL;
                        }
                        else
                        {
                            /* Codes_SRS_IOTHUBCLIENT_01_002: [IoTHubClient_Create shall instantiate a new IoTHubClientCore_LL instance by calling IoTHubClientCore_LL_Create and passing the config argument.] */
                            result->IoTHubClientLLHandle = IoTHubClientCore_LL_Create(config);
                        }
                    }
                }
                else if (iothub_uri != NULL)
                {
#ifdef USE_PROV_MODULE
                    /* Codes_SRS_IOTHUBCLIENT_12_022: [** `IoTHubClient_CreateFromDeviceAuth` shall create a lock object to be used later for serializing IoTHubClient calls. **] */
                    result->LockHandle = Lock_Init();
                    if (result->LockHandle == NULL)
                    {
                        /* Codes_SRS_IOTHUBCLIENT_12_023: [** If creating the lock fails, then IoTHubClient_CreateFromDeviceAuth shall return NULL. **] */
                        LogError("Failure creating Lock object");
                        result->IoTHubClientLLHandle = NULL;
                    }
                    else
                    {
                        /* Codes_SRS_IOTHUBCLIENT_12_025: [** `IoTHubClient_CreateFromDeviceAuth` shall instantiate a new `IoTHubClientCore_LL` instance by calling `IoTHubClientCore_LL_CreateFromDeviceAuth` and passing iothub_uri, device_id and protocol argument.  **] */
                        result->IoTHubClientLLHandle = IoTHubClientCore_LL_CreateFromDeviceAuth(iothub_uri, device_id, protocol);
                    }
#else
                    (void)device_id;
                    LogError("Provisioning is not enabled for the build");
                    result->IoTHubClientLLHandle = NULL;
#endif
                }
#ifdef USE_EDGE_MODULES
                else if (create_hub_instance_type == CREATE_HUB_INSTANCE_FROM_EDGE_ENVIRONMENT)
                {
                    result->LockHandle = Lock_Init();
                    if (result->LockHandle == NULL)
                    {
                        /* Codes_SRS_IOTHUBCLIENT_01_030: [If creating the lock fails, then IoTHubClient_Create shall return NULL.] */
                        /* Codes_SRS_IOTHUBCLIENT_01_031: [If IoTHubClient_Create fails, all resources allocated by it shall be freed.] */
                        LogError("Failure creating Lock object");
                        result->IoTHubClientLLHandle = NULL;
                    }
                    else
                    {
                        result->IoTHubClientLLHandle = IoTHubClientCore_LL_CreateFromEnvironment(protocol);
                    }
                }
#endif
                else
                {
                    result->LockHandle = Lock_Init();
                    if (result->LockHandle == NULL)
                    {
                        /* Codes_SRS_IOTHUBCLIENT_01_030: [If creating the lock fails, then IoTHubClient_Create shall return NULL.] */
                        /* Codes_SRS_IOTHUBCLIENT_01_031: [If IoTHubClient_Create fails, all resources allocated by it shall be freed.] */
                        LogError("Failure creating Lock object");
                        result->IoTHubClientLLHandle = NULL;
                    }
                    else
                    {
                        result->IoTHubClientLLHandle = IoTHubClientCore_LL_CreateFromConnectionString(connectionString, protocol);
                    }
                }

                if (result->IoTHubClientLLHandle == NULL)
                {
                    /* Codes_SRS_IOTHUBCLIENT_01_003: [If IoTHubClientCore_LL_Create fails, then IoTHubClient_Create shall return NULL.] */
                    /* Codes_SRS_IOTHUBCLIENT_01_031: [If IoTHubClient_Create fails, all resources allocated by it shall be freed.] */
                    /* Codes_SRS_IOTHUBCLIENT_17_006: [ If IoTHubTransport_GetLock fails, then IoTHubClient_CreateWithTransport shall return NULL. ]*/
                    if ((transportHandle == NULL) && (result->LockHandle != NULL))
                    {
                        Lock_Deinit(result->LockHandle);
                    }
                    singlylinkedlist_destroy(result->httpWorkerThreadInfoList);
                    LogError("Failure creating iothub handle");
                    VECTOR_destroy(result->saved_user_callback_list);
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->ThreadHandle = NULL;
                    result->desired_state_callback = NULL;
                    result->devicetwin_user_context = NULL;
                    result->connection_status_callback = NULL;
                    result->connection_status_user_context = NULL;
                    result->message_callback = NULL;
                    result->message_user_context = NULL;
                    result->method_user_context = NULL;
                }
            }
        }
    }
    return result;
}

IOTHUB_CLIENT_CORE_HANDLE IoTHubClientCore_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    IOTHUB_CLIENT_CORE_INSTANCE* result;

    if (connectionString == NULL)
    {
        LogError("Input parameter is NULL: connectionString");
        result = NULL;
    }
    else if (protocol == NULL)
    {
        LogError("Input parameter is NULL: protocol");
        result = NULL;
    }
    else
    {
        result = create_iothub_instance(CREATE_HUB_INSTANCE_FROM_CONNECTION_STRING, NULL, NULL, connectionString, protocol, NULL, NULL);
    }
    return result;
}

IOTHUB_CLIENT_CORE_HANDLE IoTHubClientCore_Create(const IOTHUB_CLIENT_CONFIG* config)
{
    IOTHUB_CLIENT_CORE_INSTANCE* result;
    if (config == NULL)
    {
        LogError("Input parameter is NULL: IOTHUB_CLIENT_CONFIG");
        result = NULL;
    }
    else
    {
        result = create_iothub_instance(CREATE_HUB_INSTANCE_FROM_CLIENT_CONFIG, config, NULL, NULL, NULL, NULL, NULL);
    }
    return result;
}

IOTHUB_CLIENT_CORE_HANDLE IoTHubClientCore_CreateWithTransport(TRANSPORT_HANDLE transportHandle, const IOTHUB_CLIENT_CONFIG* config)
{
    IOTHUB_CLIENT_CORE_INSTANCE* result;
    /*Codes_SRS_IOTHUBCLIENT_17_013: [ IoTHubClient_CreateWithTransport shall return NULL if transportHandle is NULL. ]*/
    /*Codes_SRS_IOTHUBCLIENT_17_014: [ IoTHubClient_CreateWithTransport shall return NULL if config is NULL. ]*/
    if (transportHandle == NULL || config == NULL)
    {
        LogError("invalid parameter TRANSPORT_HANDLE transportHandle=%p, const IOTHUB_CLIENT_CONFIG* config=%p", transportHandle, config);
        result = NULL;
    }
    else
    {
        result = create_iothub_instance(CREATE_HUB_INSTANCE_FROM_TRANSPORT, config, transportHandle, NULL, NULL, NULL, NULL);
    }
    return result;
}

IOTHUB_CLIENT_CORE_HANDLE IoTHubClientCore_CreateFromDeviceAuth(const char* iothub_uri, const char* device_id, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    IOTHUB_CLIENT_CORE_INSTANCE* result;

    /* Codes_SRS_IOTHUBCLIENT_12_019: [** `IoTHubClient_CreateFromDeviceAuth` shall verify the input parameters and if any of them `NULL` then return `NULL`. **] */
    if (iothub_uri == NULL)
    {
        LogError("Input parameter is NULL: iothub_uri");
        result = NULL;
    }
    else if (device_id == NULL)
    {
        LogError("Input parameter is NULL: device_id");
        result = NULL;
    }
    else if (protocol == NULL)
    {
        LogError("Input parameter is NULL: protocol");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_IOTHUBCLIENT_12_020: [** `IoTHubClient_CreateFromDeviceAuth` shall allocate a new `IoTHubClient` instance. **] */
        /* Codes_SRS_IOTHUBCLIENT_12_021: [** If allocating memory for the new `IoTHubClient` instance fails, then `IoTHubClient_CreateFromDeviceAuth` shall return `NULL`. **] */
        /* Codes_SRS_IOTHUBCLIENT_12_022: [** `IoTHubClient_CreateFromDeviceAuth` shall create a lock object to be used later for serializing IoTHubClient calls. **] */
        /* Codes_SRS_IOTHUBCLIENT_12_023: [** If creating the lock fails, then IoTHubClient_CreateFromDeviceAuth shall return NULL. **] */
        /* Codes_SRS_IOTHUBCLIENT_12_024: [** If IoTHubClient_CreateFromDeviceAuth fails, all resources allocated by it shall be freed. **] */
        /* Codes_SRS_IOTHUBCLIENT_12_025: [** `IoTHubClient_CreateFromDeviceAuth` shall instantiate a new `IoTHubClientCore_LL` instance by calling `IoTHubClientCore_LL_CreateFromDeviceAuth` and passing iothub_uri, device_id and protocol argument.  **] */
        result = create_iothub_instance(CREATE_HUB_INSTANCE_FROM_DEVICE_AUTH, NULL, NULL, NULL, protocol, iothub_uri, device_id);
    }
    return result;
}

#ifdef USE_EDGE_MODULES
IOTHUB_CLIENT_CORE_HANDLE IoTHubClientCore_CreateFromEnvironment(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    return create_iothub_instance(CREATE_HUB_INSTANCE_FROM_EDGE_ENVIRONMENT, NULL, NULL, NULL, protocol, NULL, NULL);
}
#endif


/* Codes_SRS_IOTHUBCLIENT_01_005: [IoTHubClient_Destroy shall free all resources associated with the iotHubClientHandle instance.] */
void IoTHubClientCore_Destroy(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle)
{
    /* Codes_SRS_IOTHUBCLIENT_01_008: [IoTHubClient_Destroy shall do nothing if parameter iotHubClientHandle is NULL.] */
    if (iotHubClientHandle != NULL)
    {
        bool joinClientThread;
        bool joinTransportThread;
        size_t vector_size;

        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        if (iotHubClientInstance->TransportHandle != NULL)
        {
            /*Codes_SRS_IOTHUBCLIENT_01_007: [ The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetNotificationMessageCallback shall be joined. ]*/
            joinTransportThread = IoTHubTransport_SignalEndWorkerThread(iotHubClientInstance->TransportHandle, iotHubClientHandle);
        }
        else
        {
            joinTransportThread = false;
        }

        /*Codes_SRS_IOTHUBCLIENT_02_043: [ IoTHubClient_Destroy shall lock the serializing lock and signal the worker thread (if any) to end ]*/
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            LogError("unable to Lock - - will still proceed to try to end the thread without locking");
        }

        if (iotHubClientInstance->ThreadHandle != NULL)
        {
            iotHubClientInstance->StopThread = 1;
            joinClientThread = true;
        }
        else
        {
            joinClientThread = false;
        }

        /*Codes_SRS_IOTHUBCLIENT_02_045: [ IoTHubClient_Destroy shall unlock the serializing lock. ]*/
        if (Unlock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            LogError("unable to Unlock");
        }

        if (joinClientThread == true)
        {
            int res;
            /*Codes_SRS_IOTHUBCLIENT_01_007: [ The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetNotificationMessageCallback shall be joined. ]*/
            if (ThreadAPI_Join(iotHubClientInstance->ThreadHandle, &res) != THREADAPI_OK)
            {
                LogError("ThreadAPI_Join failed");
            }
        }

        if (joinTransportThread == true)
        {
            /*Codes_SRS_IOTHUBCLIENT_01_007: [ The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetNotificationMessageCallback shall be joined. ]*/
            IoTHubTransport_JoinWorkerThread(iotHubClientInstance->TransportHandle, iotHubClientHandle);
        }

        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            LogError("unable to Lock - - will still proceed to try to end the thread without locking");
        }

        /*Codes_SRS_IOTHUBCLIENT_02_069: [ IoTHubClient_Destroy shall free all data created by IoTHubClient_UploadToBlobAsync ]*/
        /*wait for all uploading threads to finish*/
        while (singlylinkedlist_get_head_item(iotHubClientInstance->httpWorkerThreadInfoList) != NULL)
        {
            garbageCollectorImpl(iotHubClientInstance);
        }

        if (iotHubClientInstance->httpWorkerThreadInfoList != NULL)
        {
            singlylinkedlist_destroy(iotHubClientInstance->httpWorkerThreadInfoList);
        }

        /* Codes_SRS_IOTHUBCLIENT_01_006: [That includes destroying the IoTHubClientCore_LL instance by calling IoTHubClientCore_LL_Destroy.] */
        IoTHubClientCore_LL_Destroy(iotHubClientInstance->IoTHubClientLLHandle);

        if (Unlock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            LogError("unable to Unlock");
        }


        vector_size = VECTOR_size(iotHubClientInstance->saved_user_callback_list);
        size_t index = 0;
        for (index = 0; index < vector_size; index++)
        {
            USER_CALLBACK_INFO* queue_cb_info = (USER_CALLBACK_INFO*)VECTOR_element(iotHubClientInstance->saved_user_callback_list, index);
            if (queue_cb_info != NULL)
            {
                if ((queue_cb_info->type == CALLBACK_TYPE_DEVICE_METHOD) || (queue_cb_info->type == CALLBACK_TYPE_INBOUD_DEVICE_METHOD))
                {
                    STRING_delete(queue_cb_info->iothub_callback.method_cb_info.method_name);
                    BUFFER_delete(queue_cb_info->iothub_callback.method_cb_info.payload);
                }
                else if (queue_cb_info->type == CALLBACK_TYPE_DEVICE_TWIN)
                {
                    if (queue_cb_info->iothub_callback.dev_twin_cb_info.payLoad != NULL)
                    {
                        free(queue_cb_info->iothub_callback.dev_twin_cb_info.payLoad);
                    }
                }
                else if (queue_cb_info->type == CALLBACK_TYPE_EVENT_CONFIRM)
                {
                    if (queue_cb_info->iothub_callback.event_confirm_cb_info.eventConfirmationCallback)
                    {
                        queue_cb_info->iothub_callback.event_confirm_cb_info.eventConfirmationCallback(queue_cb_info->iothub_callback.event_confirm_cb_info.confirm_result, queue_cb_info->userContextCallback);
                    }
                }
                else if (queue_cb_info->type == CALLBACK_TYPE_REPORTED_STATE)
                {
                    if (queue_cb_info->iothub_callback.reported_state_cb_info.reportedStateCallback)
                    {
                        queue_cb_info->iothub_callback.reported_state_cb_info.reportedStateCallback(queue_cb_info->iothub_callback.reported_state_cb_info.status_code, queue_cb_info->userContextCallback);
                    }
                }
            }
        }
        VECTOR_destroy(iotHubClientInstance->saved_user_callback_list);

        if (iotHubClientInstance->TransportHandle == NULL)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_032: [If the lock was allocated in IoTHubClient_Create, it shall be also freed..] */
            Lock_Deinit(iotHubClientInstance->LockHandle);
        }
        if (iotHubClientInstance->devicetwin_user_context != NULL)
        {
            free(iotHubClientInstance->devicetwin_user_context);
        }
        if (iotHubClientInstance->connection_status_user_context != NULL)
        {
            free(iotHubClientInstance->connection_status_user_context);
        }
        if (iotHubClientInstance->message_user_context != NULL)
        {
            free(iotHubClientInstance->message_user_context);
        }
        if (iotHubClientInstance->method_user_context != NULL)
        {
            free(iotHubClientInstance->method_user_context);
        }
        free(iotHubClientInstance);
    }
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_SendEventAsync(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_01_011: [If iotHubClientHandle is NULL, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_INVALID_ARG.] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_009: [IoTHubClient_SendEventAsync shall start the worker thread if it was not previously started.] */
        if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_010: [If starting the thread fails, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not start worker thread");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_025: [IoTHubClient_SendEventAsync shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
            if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
            {
                /* Codes_SRS_IOTHUBCLIENT_01_026: [If acquiring the lock fails, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_ERROR.] */
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not acquire lock");
            }
            else
            {
                if (iotHubClientInstance->created_with_transport_handle != 0 || eventConfirmationCallback == NULL)
                {
                    result = IoTHubClientCore_LL_SendEventAsync(iotHubClientInstance->IoTHubClientLLHandle, eventMessageHandle, eventConfirmationCallback, userContextCallback);
                }
                else
                {
                    /* Codes_SRS_IOTHUBCLIENT_07_001: [ IoTHubClient_SendEventAsync shall allocate a IOTHUB_QUEUE_CONTEXT object to be sent to the IoTHubClientCore_LL_SendEventAsync function as a user context. ] */
                    IOTHUB_QUEUE_CONTEXT* queue_context = (IOTHUB_QUEUE_CONTEXT*)malloc(sizeof(IOTHUB_QUEUE_CONTEXT));
                    if (queue_context == NULL)
                    {
                        result = IOTHUB_CLIENT_ERROR;
                        LogError("Failed allocating QUEUE_CONTEXT");
                    }
                    else
                    {
                        queue_context->iotHubClientHandle = iotHubClientInstance;
                        queue_context->userContextCallback = userContextCallback;
                        queue_context->callbackFunction.eventConfirmationCallback = eventConfirmationCallback;
                        /* Codes_SRS_IOTHUBCLIENT_01_012: [IoTHubClient_SendEventAsync shall call IoTHubClientCore_LL_SendEventAsync, while passing the IoTHubClientCore_LL handle created by IoTHubClient_Create and the parameters eventMessageHandle, eventConfirmationCallback and userContextCallback.] */
                        /* Codes_SRS_IOTHUBCLIENT_01_013: [When IoTHubClientCore_LL_SendEventAsync is called, IoTHubClient_SendEventAsync shall return the result of IoTHubClientCore_LL_SendEventAsync.] */
                        result = IoTHubClientCore_LL_SendEventAsync(iotHubClientInstance->IoTHubClientLLHandle, eventMessageHandle, iothub_ll_event_confirm_callback, queue_context);
                        if (result != IOTHUB_CLIENT_OK)
                        {
                            LogError("IoTHubClientCore_LL_SendEventAsync failed");
                            free(queue_context);
                        }
                    }
                }

                /* Codes_SRS_IOTHUBCLIENT_01_025: [IoTHubClient_SendEventAsync shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
                (void)Unlock(iotHubClientInstance->LockHandle);
            }
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_GetSendStatus(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_01_023: [If iotHubClientHandle is NULL, IoTHubClient_ GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG.] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_033: [IoTHubClient_GetSendStatus shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_034: [If acquiring the lock fails, IoTHubClient_GetSendStatus shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_022: [IoTHubClient_GetSendStatus shall call IoTHubClientCore_LL_GetSendStatus, while passing the IoTHubClientCore_LL handle created by IoTHubClient_Create and the parameter iotHubClientStatus.] */
            /* Codes_SRS_IOTHUBCLIENT_01_024: [Otherwise, IoTHubClient_GetSendStatus shall return the result of IoTHubClientCore_LL_GetSendStatus.] */
            result = IoTHubClientCore_LL_GetSendStatus(iotHubClientInstance->IoTHubClientLLHandle, iotHubClientStatus);

            /* Codes_SRS_IOTHUBCLIENT_01_033: [IoTHubClient_GetSendStatus shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_SetMessageCallback(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_01_016: [If iotHubClientHandle is NULL, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_INVALID_ARG.] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_014: [IoTHubClient_SetMessageCallback shall start the worker thread if it was not previously started.] */
        if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_015: [If starting the thread fails, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not start worker thread");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_027: [IoTHubClient_SetMessageCallback shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
            if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
            {
                /* Codes_SRS_IOTHUBCLIENT_01_028: [If acquiring the lock fails, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_ERROR.] */
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not acquire lock");
            }
            else
            {
                if (iotHubClientInstance->created_with_transport_handle == 0)
                {
                    iotHubClientInstance->message_callback = messageCallback;
                }
                if (iotHubClientInstance->message_user_context != NULL)
                {
                    free(iotHubClientInstance->message_user_context);
                    iotHubClientInstance->message_user_context = NULL;
                }
                if (messageCallback == NULL)
                {
                    result = IoTHubClientCore_LL_SetMessageCallback_Ex(iotHubClientInstance->IoTHubClientLLHandle, NULL, iotHubClientInstance->message_user_context);
                }
                else if (iotHubClientInstance->created_with_transport_handle != 0)
                {
                    result = IoTHubClientCore_LL_SetMessageCallback(iotHubClientInstance->IoTHubClientLLHandle, messageCallback, userContextCallback);
                }
                else
                {
                    iotHubClientInstance->message_user_context = (IOTHUB_QUEUE_CONTEXT*)malloc(sizeof(IOTHUB_QUEUE_CONTEXT));
                    if (iotHubClientInstance->message_user_context == NULL)
                    {
                        result = IOTHUB_CLIENT_ERROR;
                        LogError("Failed allocating QUEUE_CONTEXT");
                    }
                    else
                    {
                        iotHubClientInstance->message_user_context->iotHubClientHandle = iotHubClientHandle;
                        iotHubClientInstance->message_user_context->userContextCallback = userContextCallback;

                        /* Codes_SRS_IOTHUBCLIENT_01_017: [IoTHubClient_SetMessageCallback shall call IoTHubClientCore_LL_SetMessageCallback_Ex, while passing the IoTHubClientCore_LL handle created by IoTHubClient_Create and the local iothub_ll_message_callback wrapper of messageCallback and userContextCallback.] */
                        /* Codes_SRS_IOTHUBCLIENT_01_018: [When IoTHubClientCore_LL_SetMessageCallback_Ex is called, IoTHubClient_SetMessageCallback shall return the result of IoTHubClientCore_LL_SetMessageCallback_Ex.] */
                        result = IoTHubClientCore_LL_SetMessageCallback_Ex(iotHubClientInstance->IoTHubClientLLHandle, iothub_ll_message_callback, iotHubClientInstance->message_user_context);
                        if (result != IOTHUB_CLIENT_OK)
                        {
                            LogError("IoTHubClientCore_LL_SetMessageCallback failed");
                            free(iotHubClientInstance->message_user_context);
                            iotHubClientInstance->message_user_context = NULL;
                        }
                    }
                }

                /* Codes_SRS_IOTHUBCLIENT_01_027: [IoTHubClient_SetMessageCallback shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
                (void)Unlock(iotHubClientInstance->LockHandle);
            }
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_SetConnectionStatusCallback(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK connectionStatusCallback, void * userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_25_076: [** If `iotHubClientHandle` is `NULL`, `IoTHubClient_SetRetryPolicy` shall return `IOTHUB_CLIENT_INVALID_ARG`. ] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_25_081: [ `IoTHubClient_SetConnectionStatusCallback` shall start the worker thread if it was not previously started. ]*/
        if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_25_083: [ If starting the thread fails, `IoTHubClient_SetConnectionStatusCallback` shall return `IOTHUB_CLIENT_ERROR`. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not start worker thread");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_25_087: [ `IoTHubClient_SetConnectionStatusCallback` shall be made thread-safe by using the lock created in `IoTHubClient_Create`. ] */
            if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
            {
                /* Codes_SRS_IOTHUBCLIENT_25_088: [ If acquiring the lock fails, `IoTHubClient_SetConnectionStatusCallback` shall return `IOTHUB_CLIENT_ERROR`. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not acquire lock");
            }
            else
            {
                if (iotHubClientInstance->created_with_transport_handle == 0)
                {
                    iotHubClientInstance->connection_status_callback = connectionStatusCallback;
                }

                if (iotHubClientInstance->created_with_transport_handle != 0 || connectionStatusCallback == NULL)
                {
                    /* Codes_SRS_IOTHUBCLIENT_25_085: [ `IoTHubClient_SetConnectionStatusCallback` shall call `IoTHubClientCore_LL_SetConnectionStatusCallback`, while passing the `IoTHubClientCore_LL` handle created by `IoTHubClient_Create` and the parameters `connectionStatusCallback` and `userContextCallback`. ]*/
                    result = IoTHubClientCore_LL_SetConnectionStatusCallback(iotHubClientInstance->IoTHubClientLLHandle, connectionStatusCallback, userContextCallback);
                }
                else
                {
                    if (iotHubClientInstance->connection_status_user_context != NULL)
                    {
                        free(iotHubClientInstance->connection_status_user_context);
                    }
                    iotHubClientInstance->connection_status_user_context = (IOTHUB_QUEUE_CONTEXT*)malloc(sizeof(IOTHUB_QUEUE_CONTEXT));
                    if (iotHubClientInstance->connection_status_user_context == NULL)
                    {
                        result = IOTHUB_CLIENT_ERROR;
                        LogError("Failed allocating QUEUE_CONTEXT");
                    }
                    else
                    {
                        iotHubClientInstance->connection_status_user_context->iotHubClientHandle = iotHubClientInstance;
                        iotHubClientInstance->connection_status_user_context->userContextCallback = userContextCallback;

                        /* Codes_SRS_IOTHUBCLIENT_25_085: [ `IoTHubClient_SetConnectionStatusCallback` shall call `IoTHubClientCore_LL_SetConnectionStatusCallback`, while passing the `IoTHubClientCore_LL` handle created by `IoTHubClient_Create` and the parameters `connectionStatusCallback` and `userContextCallback`. ]*/
                        result = IoTHubClientCore_LL_SetConnectionStatusCallback(iotHubClientInstance->IoTHubClientLLHandle, iothub_ll_connection_status_callback, iotHubClientInstance->connection_status_user_context);
                        if (result != IOTHUB_CLIENT_OK)
                        {
                            LogError("IoTHubClientCore_LL_SetConnectionStatusCallback failed");
                            free(iotHubClientInstance->connection_status_user_context);
                            iotHubClientInstance->connection_status_user_context = NULL;
                        }
                    }
                }
                (void)Unlock(iotHubClientInstance->LockHandle);
            }
        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_SetRetryPolicy(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_25_076: [** If `iotHubClientHandle` is `NULL`, `IoTHubClient_SetRetryPolicy` shall return `IOTHUB_CLIENT_INVALID_ARG`. ] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_25_073: [ `IoTHubClient_SetRetryPolicy` shall start the worker thread if it was not previously started. ] */
        if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_25_075: [ If starting the thread fails, `IoTHubClient_SetRetryPolicy` shall return `IOTHUB_CLIENT_ERROR`. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not start worker thread");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_25_079: [ `IoTHubClient_SetRetryPolicy` shall be made thread-safe by using the lock created in `IoTHubClient_Create`.] */
            if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
            {
                /* Codes_SRS_IOTHUBCLIENT_25_080: [ If acquiring the lock fails, `IoTHubClient_SetRetryPolicy` shall return `IOTHUB_CLIENT_ERROR`. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not acquire lock");
            }
            else
            {
                /* Codes_SRS_IOTHUBCLIENT_25_077: [ `IoTHubClient_SetRetryPolicy` shall call `IoTHubClientCore_LL_SetRetryPolicy`, while passing the `IoTHubClientCore_LL` handle created by `IoTHubClient_Create` and the parameters `retryPolicy` and `retryTimeoutLimitinSeconds`.]*/
                result = IoTHubClientCore_LL_SetRetryPolicy(iotHubClientInstance->IoTHubClientLLHandle, retryPolicy, retryTimeoutLimitInSeconds);
                (void)Unlock(iotHubClientInstance->LockHandle);
            }

        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_GetRetryPolicy(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY* retryPolicy, size_t* retryTimeoutLimitInSeconds)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_25_092: [ If `iotHubClientHandle` is `NULL`, `IoTHubClient_GetRetryPolicy` shall return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_25_089: [ `IoTHubClient_GetRetryPolicy` shall start the worker thread if it was not previously started.]*/
        if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_25_091: [ If starting the thread fails, `IoTHubClient_GetRetryPolicy` shall return `IOTHUB_CLIENT_ERROR`.]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not start worker thread");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_25_095: [ `IoTHubClient_GetRetryPolicy` shall be made thread-safe by using the lock created in `IoTHubClient_Create`. ]*/
            if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
            {
                /* Codes_SRS_IOTHUBCLIENT_25_096: [ If acquiring the lock fails, `IoTHubClient_GetRetryPolicy` shall return `IOTHUB_CLIENT_ERROR`. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not acquire lock");
            }
            else
            {
                /* Codes_SRS_IOTHUBCLIENT_25_093: [ `IoTHubClient_GetRetryPolicy` shall call `IoTHubClientCore_LL_GetRetryPolicy`, while passing the `IoTHubClientCore_LL` handle created by `IoTHubClient_Create` and the parameters `connectionStatusCallback` and `userContextCallback`.]*/
                result = IoTHubClientCore_LL_GetRetryPolicy(iotHubClientInstance->IoTHubClientLLHandle, retryPolicy, retryTimeoutLimitInSeconds);
                (void)Unlock(iotHubClientInstance->LockHandle);
            }
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_GetLastMessageReceiveTime(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        /* Codes_SRS_IOTHUBCLIENT_01_020: [If iotHubClientHandle is NULL, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INVALID_ARG.] */
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_035: [IoTHubClient_GetLastMessageReceiveTime shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_036: [If acquiring the lock fails, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_ERROR.] */
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_01_019: [IoTHubClient_GetLastMessageReceiveTime shall call IoTHubClientCore_LL_GetLastMessageReceiveTime, while passing the IoTHubClientCore_LL handle created by IoTHubClient_Create and the parameter lastMessageReceiveTime.] */
            /* Codes_SRS_IOTHUBCLIENT_01_021: [Otherwise, IoTHubClient_GetLastMessageReceiveTime shall return the result of IoTHubClientCore_LL_GetLastMessageReceiveTime.] */
            result = IoTHubClientCore_LL_GetLastMessageReceiveTime(iotHubClientInstance->IoTHubClientLLHandle, lastMessageReceiveTime);

            /* Codes_SRS_IOTHUBCLIENT_01_035: [IoTHubClient_GetLastMessageReceiveTime shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_SetOption(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, const char* optionName, const void* value)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_02_034: [If parameter iotHubClientHandle is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    /*Codes_SRS_IOTHUBCLIENT_02_035: [ If parameter optionName is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
    /*Codes_SRS_IOTHUBCLIENT_02_036: [ If parameter value is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
    if (
        (iotHubClientHandle == NULL) ||
        (optionName == NULL) ||
        (value == NULL)
        )
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid arg (NULL)");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /* Codes_SRS_IOTHUBCLIENT_01_041: [ IoTHubClient_SetOption shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /* Codes_SRS_IOTHUBCLIENT_01_042: [ If acquiring the lock fails, IoTHubClient_SetOption shall return IOTHUB_CLIENT_ERROR. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            /* Codes_SRS_IOTHUBCLIENT_41_001 [ If parameter `optionName` is `OPTION_DO_WORK_FREQUENCY_IN_MS` then `IoTHubClient_SetOption` shall set `do_work_freq_ms` parameter of `IoTHubClientInstance` ]*/
            if (strcmp(OPTION_DO_WORK_FREQUENCY_IN_MS, optionName) == 0)
            {
                /* Codes_SRS_IOTHUBCLIENT_41_003: [ The value for `OPTION_DO_WORK_FREQUENCY_IN_MS` shall be limited to 100 to follow SDK best practices by not reducing the DoWork frequency below 10 Hz ]*/
                if (0 < * (unsigned int *)value && * (unsigned int *)value <= DO_WORK_MAXIMUM_ALLOWED_FREQUENCY)
                {
                    /* Codes_SRS_IOTHUBCLIENT_41_004: [ If `currentMessageTimeout` is not greater than `do_work_freq_ms`, `IotHubClientCore_SetOption` shall return `IOTHUB_CLIENT_INVALID_ARG` ]*/
                    /* Codes_SRS_IOTHUBCLIENT_41_007: [** If parameter `optionName` is `OPTION_DO_WORK_FREQUENCY_IN_MS` then `value` should be of type `tickcounter_ms_t *`. **]*/
                    if ((!iotHubClientInstance->currentMessageTimeout) || ( * (tickcounter_ms_t *)value < iotHubClientInstance->currentMessageTimeout))
                    {
                        iotHubClientInstance->do_work_freq_ms = * (tickcounter_ms_t *)value;
                        result = IOTHUB_CLIENT_OK;
                    }
                    else
                    {
                        result = IOTHUB_CLIENT_INVALID_ARG;
                        LogError("Invalid value: OPTION_DO_WORK_FREQUENCY_IN_MS cannot exceed that of OPTION_MESSAGE_TIMEOUT.");
                    }
                }
                else
                {
                    result = IOTHUB_CLIENT_INVALID_ARG;
                    LogError("Invalid value: OPTION_DO_WORK_FREQUENCY_IN_MS cannot exceed %d ms. If you wish to reduce the frequency further, consider using the LL layer.", DO_WORK_MAXIMUM_ALLOWED_FREQUENCY);
                }
            }
            /* Codes_SRS_IOTHUBCLIENT_41_005: [ If parameter `optionName` is `OPTION_MESSAGE_TIMEOUT` then `IoTHubClientCore_SetOption` shall set `currentMessageTimeout` parameter of `IoTHubClientInstance` ]*/
            else if (strcmp(OPTION_MESSAGE_TIMEOUT, optionName) == 0)
            {
                iotHubClientInstance->currentMessageTimeout = * (tickcounter_ms_t *)value;

                /* Codes_SRS_IOTHUBCLIENT_41_004: [ If `currentMessageTimeout` is not greater than `do_work_freq_ms`, `IotHubClientCore_SetOption` shall return `IOTHUB_CLIENT_INVALID_ARG` ]*/
				if (iotHubClientInstance->do_work_freq_ms < iotHubClientInstance->currentMessageTimeout)
                {
                    /*Codes_SRS_IOTHUBCLIENT_41_006: [ If parameter `optionName` is `OPTION_MESSAGE_TIMEOUT` then `IoTHubClientCore_SetOption` shall call `IoTHubClientCore_LL_SetOption` passing the same parameters and return what IoTHubClientCore_LL_SetOption returns. ] */
                    result = IoTHubClientCore_LL_SetOption(iotHubClientInstance->IoTHubClientLLHandle, optionName, value);
                    if (result != IOTHUB_CLIENT_OK)
                    {
                        LogError("IoTHubClientCore_LL_SetOption failed");
                    }
                }
                else
                {
                    result = IOTHUB_CLIENT_INVALID_ARG;
                    LogError("invalid value: OPTION_MESSAGE_TIMEOUT cannot exceed the value of OPTION_DO_WORK_FREQUENCY_IN_MS ");
                }
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_02_038: [If optionName doesn't match one of the options handled by this module then IoTHubClient_SetOption shall call IoTHubClientCore_LL_SetOption passing the same parameters and return what IoTHubClientCore_LL_SetOption returns.] */
                result = IoTHubClientCore_LL_SetOption(iotHubClientInstance->IoTHubClientLLHandle, optionName, value);
                if (result != IOTHUB_CLIENT_OK)
                {
                    LogError("IoTHubClientCore_LL_SetOption failed");
                }
            }
            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_SetDeviceTwinCallback(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    /*Codes_SRS_IOTHUBCLIENT_10_001: [** `IoTHubClient_SetDeviceTwinCallback` shall fail and return `IOTHUB_CLIENT_INVALID_ARG` if parameter `iotHubClientHandle` is `NULL`. ]*/
    if (iotHubClientHandle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid arg (NULL)");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /*Codes_SRS_IOTHUBCLIENT_10_003: [** If the transport connection is shared, the thread shall be started by calling `IoTHubTransport_StartWorkerThread`. ]*/
        if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_10_004: [** If starting the thread fails, `IoTHubClient_SetDeviceTwinCallback` shall return `IOTHUB_CLIENT_ERROR`. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not start worker thread");
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_10_020: [** `IoTHubClient_SetDeviceTwinCallback` shall be made thread - safe by using the lock created in IoTHubClient_Create. ]*/
            if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
            {
                /*Codes_SRS_IOTHUBCLIENT_10_002: [** If acquiring the lock fails, `IoTHubClient_SetDeviceTwinCallback` shall return `IOTHUB_CLIENT_ERROR`. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not acquire lock");
            }
            else
            {
                if (iotHubClientInstance->created_with_transport_handle == 0)
                {
                    iotHubClientInstance->desired_state_callback = deviceTwinCallback;
                }

                if (iotHubClientInstance->created_with_transport_handle != 0 || deviceTwinCallback == NULL)
                {
                    /*Codes_SRS_IOTHUBCLIENT_10_005: [** `IoTHubClientCore_LL_SetDeviceTwinCallback` shall call `IoTHubClientCore_LL_SetDeviceTwinCallback`, while passing the `IoTHubClientCore_LL handle` created by `IoTHubClientCore_LL_Create` along with the parameters `reportedStateCallback` and `userContextCallback`. ]*/
                    result = IoTHubClientCore_LL_SetDeviceTwinCallback(iotHubClientInstance->IoTHubClientLLHandle, deviceTwinCallback, userContextCallback);
                }
                else
                {
                    if (iotHubClientInstance->devicetwin_user_context != NULL)
                    {
                        free(iotHubClientInstance->devicetwin_user_context);
                    }

                    /*Codes_SRS_IOTHUBCLIENT_07_002: [ IoTHubClient_SetDeviceTwinCallback shall allocate a IOTHUB_QUEUE_CONTEXT object to be sent to the IoTHubClientCore_LL_SetDeviceTwinCallback function as a user context. ]*/
                    iotHubClientInstance->devicetwin_user_context = (IOTHUB_QUEUE_CONTEXT*)malloc(sizeof(IOTHUB_QUEUE_CONTEXT));
                    if (iotHubClientInstance->devicetwin_user_context == NULL)
                    {
                        result = IOTHUB_CLIENT_ERROR;
                        LogError("Failed allocating QUEUE_CONTEXT");
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBCLIENT_10_005: [** `IoTHubClientCore_LL_SetDeviceTwinCallback` shall call `IoTHubClientCore_LL_SetDeviceTwinCallback`, while passing the `IoTHubClientCore_LL handle` created by `IoTHubClientCore_LL_Create` along with the parameters `iothub_ll_device_twin_callback` and IOTHUB_QUEUE_CONTEXT variable. ]*/
                        iotHubClientInstance->devicetwin_user_context->iotHubClientHandle = iotHubClientInstance;
                        iotHubClientInstance->devicetwin_user_context->userContextCallback = userContextCallback;
                        result = IoTHubClientCore_LL_SetDeviceTwinCallback(iotHubClientInstance->IoTHubClientLLHandle, iothub_ll_device_twin_callback, iotHubClientInstance->devicetwin_user_context);
                        if (result != IOTHUB_CLIENT_OK)
                        {
                            LogError("IoTHubClientCore_LL_SetDeviceTwinCallback failed");
                            free(iotHubClientInstance->devicetwin_user_context);
                            iotHubClientInstance->devicetwin_user_context = NULL;
                        }
                    }
                }

                (void)Unlock(iotHubClientInstance->LockHandle);
            }
        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_SendReportedState(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, const unsigned char* reportedState, size_t size, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK reportedStateCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    /*Codes_SRS_IOTHUBCLIENT_10_013: [** If `iotHubClientHandle` is `NULL`, `IoTHubClient_SendReportedState` shall return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
    if (iotHubClientHandle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid arg (NULL)");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /*Codes_SRS_IOTHUBCLIENT_10_015: [** If the transport connection is shared, the thread shall be started by calling `IoTHubTransport_StartWorkerThread`. ]*/
        if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_10_016: [** If starting the thread fails, `IoTHubClient_SendReportedState` shall return `IOTHUB_CLIENT_ERROR`. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not start worker thread");
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_10_021: [** `IoTHubClient_SendReportedState` shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
            if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
            {
                /*Codes_SRS_IOTHUBCLIENT_10_014: [** If acquiring the lock fails, `IoTHubClient_SendReportedState` shall return `IOTHUB_CLIENT_ERROR`. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not acquire lock");
            }
            else
            {
                if (iotHubClientInstance->created_with_transport_handle != 0 || reportedStateCallback == NULL)
                {
                    /*Codes_SRS_IOTHUBCLIENT_10_017: [** `IoTHubClient_SendReportedState` shall call `IoTHubClientCore_LL_SendReportedState`, while passing the `IoTHubClientCore_LL handle` created by `IoTHubClientCore_LL_Create` along with the parameters `reportedState`, `size`, `reportedStateCallback`, and `userContextCallback`. ]*/
                    /*Codes_SRS_IOTHUBCLIENT_10_018: [** When `IoTHubClientCore_LL_SendReportedState` is called, `IoTHubClient_SendReportedState` shall return the result of `IoTHubClientCore_LL_SendReportedState`. **]*/
                    result = IoTHubClientCore_LL_SendReportedState(iotHubClientInstance->IoTHubClientLLHandle, reportedState, size, reportedStateCallback, userContextCallback);
                }
                else
                {
                    /* Codes_SRS_IOTHUBCLIENT_07_003: [ IoTHubClient_SendReportedState shall allocate a IOTHUB_QUEUE_CONTEXT object to be sent to the IoTHubClientCore_LL_SendReportedState function as a user context. ] */
                    IOTHUB_QUEUE_CONTEXT* queue_context = (IOTHUB_QUEUE_CONTEXT*)malloc(sizeof(IOTHUB_QUEUE_CONTEXT));
                    if (queue_context == NULL)
                    {
                        result = IOTHUB_CLIENT_ERROR;
                        LogError("Failed allocating QUEUE_CONTEXT");
                    }
                    else
                    {
                        queue_context->iotHubClientHandle = iotHubClientInstance;
                        queue_context->userContextCallback = userContextCallback;
                        queue_context->callbackFunction.reportedStateCallback = reportedStateCallback;
                        /*Codes_SRS_IOTHUBCLIENT_10_017: [** `IoTHubClient_SendReportedState` shall call `IoTHubClientCore_LL_SendReportedState`, while passing the `IoTHubClientCore_LL handle` created by `IoTHubClientCore_LL_Create` along with the parameters `reportedState`, `size`, `iothub_ll_reported_state_callback` and IOTHUB_QUEUE_CONTEXT variable. ]*/
                        /*Codes_SRS_IOTHUBCLIENT_10_018: [** When `IoTHubClientCore_LL_SendReportedState` is called, `IoTHubClient_SendReportedState` shall return the result of `IoTHubClientCore_LL_SendReportedState`. **]*/
                        result = IoTHubClientCore_LL_SendReportedState(iotHubClientInstance->IoTHubClientLLHandle, reportedState, size, iothub_ll_reported_state_callback, queue_context);
                        if (result != IOTHUB_CLIENT_OK)
                        {
                            LogError("IoTHubClientCore_LL_SendReportedState failed");
                            free(queue_context);
                        }
                    }
                }

                (void)Unlock(iotHubClientInstance->LockHandle);
            }
        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_GetTwinAsync(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    // Codes_SRS_IOTHUBCLIENT_09_009: [ If `iotHubClientHandle` or `deviceTwinCallback` are `NULL`, `IoTHubClientCore_GetTwinAsync` shall return `IOTHUB_CLIENT_INVALID_ARG`. ]
    if (iotHubClientHandle == NULL || deviceTwinCallback == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid argument (iotHubClientHandle=%p, deviceTwinCallback=%p)", iotHubClientHandle, deviceTwinCallback);
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        // Codes_SRS_IOTHUBCLIENT_09_010: [ The thread that executes the client  I/O shall be started if not running already. ]
        if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
        {
            // Codes_SRS_IOTHUBCLIENT_09_011: [ If starting the thread fails, `IoTHubClientCore_GetTwinAsync` shall return `IOTHUB_CLIENT_ERROR`. ]
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not start worker thread");
        }
        else
        {
            IOTHUB_QUEUE_CONSOLIDATED_CONTEXT* queueContext;

            if ((queueContext = (IOTHUB_QUEUE_CONSOLIDATED_CONTEXT*)malloc(sizeof(IOTHUB_QUEUE_CONSOLIDATED_CONTEXT))) == NULL)
            {
                LogError("Failed creating queue context");
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                queueContext->iotHubClientHandle = iotHubClientHandle;
                queueContext->userCallback.getTwin = deviceTwinCallback;
                queueContext->userContext = userContextCallback;

                // Codes_SRS_IOTHUBCLIENT_09_012: [ `IoTHubClientCore_GetTwinAsync` shall be made thread-safe by using the lock created in IoTHubClient_Create. ]
                if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
                {
                    // Codes_SRS_IOTHUBCLIENT_09_013: [ If acquiring the lock fails, `IoTHubClientCore_GetTwinAsync` shall return `IOTHUB_CLIENT_ERROR`. ]
                    result = IOTHUB_CLIENT_ERROR;
                    LogError("Could not acquire lock");
                    free(queueContext);
                }
                else
                {
                    // Codes_SRS_IOTHUBCLIENT_09_014: [ `IoTHubClientCore_GetTwinAsync` shall call `IoTHubClientCore_LL_GetTwinAsync`, passing the `IoTHubClient_LL handle`, `deviceTwinCallback` and `userContextCallback` as arguments ]
                    // Codes_SRS_IOTHUBCLIENT_09_015: [ When `IoTHubClientCore_LL_GetTwinAsync` is called, `IoTHubClientCore_GetTwinAsync` shall return the result of `IoTHubClientCore_LL_GetTwinAsync`. ]
                    result = IoTHubClientCore_LL_GetTwinAsync(iotHubClientInstance->IoTHubClientLLHandle, iothub_ll_get_device_twin_async_callback, queueContext);

                    if (result != IOTHUB_CLIENT_OK)
                    {
                        LogError("IoTHubClientCore_LL_GetTwinAsync failed");
                        free(queueContext);
                    }

                    (void)Unlock(iotHubClientInstance->LockHandle);
                }
            }
        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_SetDeviceMethodCallback(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC deviceMethodCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    /*Codes_SRS_IOTHUBCLIENT_12_012: [ If iotHubClientHandle is NULL, IoTHubClient_SetDeviceMethodCallback shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
    if (iotHubClientHandle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid arg (NULL)");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /*Codes_SRS_IOTHUBCLIENT_12_014: [ If the transport handle is null and the worker thread is not initialized, the thread shall be started by calling IoTHubTransport_StartWorkerThread. ]*/
        if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_12_015: [ If starting the thread fails, IoTHubClient_SetDeviceMethodCallback shall return IOTHUB_CLIENT_ERROR. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not start worker thread");
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_12_018: [ IoTHubClient_SetDeviceMethodCallback shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
            if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
            {
                /*Codes_SRS_IOTHUBCLIENT_12_013: [ If acquiring the lock fails, IoTHubClient_SetDeviceMethodCallback shall return IOTHUB_CLIENT_ERROR. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not acquire lock");
            }
            else
            {
                if (iotHubClientInstance->created_with_transport_handle == 0)
                {
                    iotHubClientInstance->device_method_callback = deviceMethodCallback;
                }

                if (iotHubClientInstance->method_user_context)
                {
                    free(iotHubClientInstance->method_user_context);
                    iotHubClientInstance->method_user_context = NULL;
                }
                if (deviceMethodCallback == NULL)
                {
                    result = IoTHubClientCore_LL_SetDeviceMethodCallback_Ex(iotHubClientInstance->IoTHubClientLLHandle, NULL, NULL);
                }
                else
                {
                    iotHubClientInstance->method_user_context = (IOTHUB_QUEUE_CONTEXT*)malloc(sizeof(IOTHUB_QUEUE_CONTEXT));
                    if (iotHubClientInstance->method_user_context == NULL)
                    {
                        result = IOTHUB_CLIENT_ERROR;
                        LogError("Failed allocating QUEUE_CONTEXT");
                    }
                    else
                    {
                        iotHubClientInstance->method_user_context->iotHubClientHandle = iotHubClientHandle;
                        iotHubClientInstance->method_user_context->userContextCallback = userContextCallback;

                        /*Codes_SRS_IOTHUBCLIENT_12_016: [ IoTHubClient_SetDeviceMethodCallback shall call IoTHubClientCore_LL_SetDeviceMethodCallback, while passing the IoTHubClientCore_LL_handle created by IoTHubClientCore_LL_Create along with the parameters deviceMethodCallback and userContextCallback. ]*/
                        /*Codes_SRS_IOTHUBCLIENT_12_017: [ When IoTHubClientCore_LL_SetDeviceMethodCallback is called, IoTHubClient_SetDeviceMethodCallback shall return the result of IoTHubClientCore_LL_SetDeviceMethodCallback. ]*/
                        result = IoTHubClientCore_LL_SetDeviceMethodCallback_Ex(iotHubClientInstance->IoTHubClientLLHandle, iothub_ll_device_method_callback, iotHubClientInstance->method_user_context);
                        if (result != IOTHUB_CLIENT_OK)
                        {
                            LogError("IoTHubClientCore_LL_SetDeviceMethodCallback_Ex failed");
                            free(iotHubClientInstance->method_user_context);
                            iotHubClientInstance->method_user_context = NULL;
                        }
                        else
                        {
                            iotHubClientInstance->device_method_callback = deviceMethodCallback;
                        }
                    }
                }

                (void)Unlock(iotHubClientInstance->LockHandle);
            }

        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_SetDeviceMethodCallback_Ex(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK inboundDeviceMethodCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    /*Codes_SRS_IOTHUBCLIENT_07_001: [ If iotHubClientHandle is NULL, IoTHubClient_SetDeviceMethodCallback_Ex shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
    if (iotHubClientHandle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid arg (NULL)");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /*Codes_SRS_IOTHUBCLIENT_07_003: [ If the transport handle is NULL and the worker thread is not initialized, the thread shall be started by calling IoTHubTransport_StartWorkerThread. ]*/
        if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_07_004: [ If starting the thread fails, IoTHubClient_SetDeviceMethodCallback_Ex shall return IOTHUB_CLIENT_ERROR. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not start worker thread");
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_07_007: [ IoTHubClient_SetDeviceMethodCallback_Ex shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
            if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
            {
                /*Codes_SRS_IOTHUBCLIENT_07_002: [ If acquiring the lock fails, IoTHubClient_SetDeviceMethodCallback_Ex shall return IOTHUB_CLIENT_ERROR. ]*/
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not acquire lock");
            }
            else
            {
                if (iotHubClientInstance->created_with_transport_handle == 0)
                {
                    iotHubClientInstance->inbound_device_method_callback = inboundDeviceMethodCallback;
                }

                if (iotHubClientInstance->method_user_context)
                {
                    free(iotHubClientInstance->method_user_context);
                    iotHubClientInstance->method_user_context = NULL;
                }
                if (inboundDeviceMethodCallback == NULL)
                {
                    /* Codes_SRS_IOTHUBCLIENT_07_008: [ If inboundDeviceMethodCallback is NULL, IoTHubClient_SetDeviceMethodCallback_Ex shall call IoTHubClientCore_LL_SetDeviceMethodCallback_Ex, passing NULL for the iothub_ll_inbound_device_method_callback. ] */
                    result = IoTHubClientCore_LL_SetDeviceMethodCallback_Ex(iotHubClientInstance->IoTHubClientLLHandle, NULL, NULL);
                }
                else
                {
                    iotHubClientInstance->method_user_context = (IOTHUB_QUEUE_CONTEXT*)malloc(sizeof(IOTHUB_QUEUE_CONTEXT));
                    if (iotHubClientInstance->method_user_context == NULL)
                    {
                        result = IOTHUB_CLIENT_ERROR;
                        LogError("Failed allocating QUEUE_CONTEXT");
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBCLIENT_07_005: [ IoTHubClient_SetDeviceMethodCallback_Ex shall call IoTHubClientCore_LL_SetDeviceMethodCallback_Ex, while passing the IoTHubClientCore_LL_handle created by IoTHubClientCore_LL_Create along with the parameters iothub_ll_inbound_device_method_callback and IOTHUB_QUEUE_CONTEXT. ]*/
                        iotHubClientInstance->method_user_context->iotHubClientHandle = iotHubClientHandle;
                        iotHubClientInstance->method_user_context->userContextCallback = userContextCallback;

                        /* Codes_SRS_IOTHUBCLIENT_07_006: [ When IoTHubClientCore_LL_SetDeviceMethodCallback_Ex is called, IoTHubClient_SetDeviceMethodCallback_Ex shall return the result of IoTHubClientCore_LL_SetDeviceMethodCallback_Ex. ] */
                        result = IoTHubClientCore_LL_SetDeviceMethodCallback_Ex(iotHubClientInstance->IoTHubClientLLHandle, iothub_ll_inbound_device_method_callback, iotHubClientInstance->method_user_context);
                        if (result != IOTHUB_CLIENT_OK)
                        {
                            LogError("IoTHubClientCore_LL_SetDeviceMethodCallback_Ex failed");
                            free(iotHubClientInstance->method_user_context);
                            iotHubClientInstance->method_user_context = NULL;
                        }
                        else
                        {
                            iotHubClientInstance->inbound_device_method_callback = inboundDeviceMethodCallback;
                        }
                    }
                }

                (void)Unlock(iotHubClientInstance->LockHandle);
            }
        }
    }
    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_DeviceMethodResponse(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, METHOD_HANDLE methodId, const unsigned char* response, size_t respSize, int statusCode)
{
    IOTHUB_CLIENT_RESULT result;

    /*Codes_SRS_IOTHUBCLIENT_12_012: [ If iotHubClientHandle is NULL, IoTHubClient_SetDeviceMethodCallback shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
    if (iotHubClientHandle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid arg (NULL)");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        /*Codes_SRS_IOTHUBCLIENT_12_018: [ IoTHubClient_SetDeviceMethodCallback shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
        if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_12_013: [ If acquiring the lock fails, IoTHubClient_SetDeviceMethodCallback shall return IOTHUB_CLIENT_ERROR. ]*/
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not acquire lock");
        }
        else
        {
            result = IoTHubClientCore_LL_DeviceMethodResponse(iotHubClientInstance->IoTHubClientLLHandle, methodId, response, respSize, statusCode);
            if (result != IOTHUB_CLIENT_OK)
            {
                LogError("IoTHubClientCore_LL_DeviceMethodResponse failed");
            }
            (void)Unlock(iotHubClientInstance->LockHandle);
        }
    }
    return result;
}

#if !defined(DONT_USE_UPLOADTOBLOB) || defined(USE_EDGE_MODULES)
static IOTHUB_CLIENT_RESULT startHttpWorkerThread(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, HTTPWORKER_THREAD_INFO* threadInfo, THREAD_START_FUNC httpWorkerThreadFunc)
{
    IOTHUB_CLIENT_RESULT result;

    LIST_ITEM_HANDLE item;

    // StartWorkerThreadIfNeeded creates the "main" worker thread used for transports.  Though its not used
    // for these HTTP based worker threads (see ThreadAPI_Create call below) the main one is needed for garbage collection.
    if ((result = StartWorkerThreadIfNeeded(iotHubClientHandle)) != IOTHUB_CLIENT_OK)
    {
        /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
        LogError("Could not start worker thread");
    }
    else if (Lock(threadInfo->iotHubClientHandle->LockHandle) != LOCK_OK)
    {
        LogError("Lock failed");
        result = IOTHUB_CLIENT_ERROR;
    }
    else
    {
        if ((item = singlylinkedlist_add(threadInfo->iotHubClientHandle->httpWorkerThreadInfoList, threadInfo)) == NULL)
        {
            LogError("Adding item to list failed");
            result = IOTHUB_CLIENT_ERROR;
        }
        else if (ThreadAPI_Create(&threadInfo->threadHandle, httpWorkerThreadFunc, threadInfo) != THREADAPI_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
            LogError("unable to ThreadAPI_Create");
            // Remove the item from linked list here, while we're still under lock.  Final garbage collector also does it under lock.
            (void)singlylinkedlist_remove(threadInfo->iotHubClientHandle->httpWorkerThreadInfoList, item);
            result = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            result = IOTHUB_CLIENT_OK;
        }
        (void)Unlock(threadInfo->iotHubClientHandle->LockHandle);
    }

    return result;
}

static int markThreadReadyToBeGarbageCollected(HTTPWORKER_THREAD_INFO* threadInfo)
{
    /*Codes_SRS_IOTHUBCLIENT_02_071: [ The thread shall mark itself as disposable. ]*/
    if (Lock(threadInfo->lockGarbage) != LOCK_OK)
    {
        LogError("unable to Lock - trying anyway");
        threadInfo->canBeGarbageCollected = 1;
    }
    else
    {
        threadInfo->canBeGarbageCollected = 1;

        if (Unlock(threadInfo->lockGarbage) != LOCK_OK)
        {
            LogError("unable to Unlock after locking");
        }
    }

    ThreadAPI_Exit(0);
    return 0;
}

#endif // !defined(DONT_USE_UPLOADTOBLOB) || defined(USE_EDGE_MODULES)

#if !defined(DONT_USE_UPLOADTOBLOB)
static HTTPWORKER_THREAD_INFO* allocateUploadToBlob(const char* destinationFileName, IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, void* context)
{
    HTTPWORKER_THREAD_INFO* threadInfo = (HTTPWORKER_THREAD_INFO*)malloc(sizeof(HTTPWORKER_THREAD_INFO));
    if (threadInfo == NULL)
    {
        LogError("unable to allocate thread object");
    }
    else
    {
        memset(threadInfo, 0, sizeof(HTTPWORKER_THREAD_INFO));
        threadInfo->workerThreadType = HTTPWORKER_THREAD_UPLOAD_TO_BLOB;
        threadInfo->iotHubClientHandle = iotHubClientHandle;
        threadInfo->context = context;

        if (mallocAndStrcpy_s(&threadInfo->destinationFileName, destinationFileName) != 0)
        {
            /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
            LogError("unable to mallocAndStrcpy_s");
            freeHttpWorkerThreadInfo(threadInfo);
            threadInfo = NULL;
        }
        else if ((threadInfo->lockGarbage = Lock_Init()) == NULL)
        {
            LogError("unable to allocate a lock");
            freeHttpWorkerThreadInfo(threadInfo);
            threadInfo = NULL;
        }
    }

    return threadInfo;
}


static IOTHUB_CLIENT_RESULT initializeUploadToBlobData(HTTPWORKER_THREAD_INFO* threadInfo, const unsigned char* source, size_t size, IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK iotHubClientFileUploadCallback)
{
    IOTHUB_CLIENT_RESULT result;

    threadInfo->uploadBlobSavedData.size = size;
    threadInfo->uploadBlobSavedData.iotHubClientFileUploadCallback = iotHubClientFileUploadCallback;

    if (size != 0)
    {
        if ((threadInfo->uploadBlobSavedData.source = (unsigned char*)malloc(size)) == NULL)
        {
            LogError("Cannot allocate source field");
            result = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            memcpy(threadInfo->uploadBlobSavedData.source, source, size);
            result = IOTHUB_CLIENT_OK;
        }
    }
    else
    {
        result = IOTHUB_CLIENT_OK;
    }

    return result;
}


static int uploadingThread(void *data)
{
    IOTHUB_CLIENT_FILE_UPLOAD_RESULT upload_result;
    HTTPWORKER_THREAD_INFO* threadInfo = (HTTPWORKER_THREAD_INFO*)data;

    srand((unsigned int)get_time(NULL));

    /*it so happens that IoTHubClientCore_LL_UploadToBlob is thread-safe because there's no saved state in the handle and there are no globals, so no need to protect it*/
    /*not having it protected means multiple simultaneous uploads can happen*/
    /*Codes_SRS_IOTHUBCLIENT_02_054: [ The thread shall call IoTHubClientCore_LL_UploadToBlob passing the information packed in the structure. ]*/
    if (IoTHubClientCore_LL_UploadToBlob(threadInfo->iotHubClientHandle->IoTHubClientLLHandle, threadInfo->destinationFileName, threadInfo->uploadBlobSavedData.source, threadInfo->uploadBlobSavedData.size) == IOTHUB_CLIENT_OK)
    {
        upload_result = FILE_UPLOAD_OK;
    }
    else
    {
        LogError("unable to IoTHubClientCore_LL_UploadToBlob");
        upload_result = FILE_UPLOAD_ERROR;
    }

    if (threadInfo->uploadBlobSavedData.iotHubClientFileUploadCallback != NULL)
    {
        /*Codes_SRS_IOTHUBCLIENT_02_055: [ If IoTHubClientCore_LL_UploadToBlob fails then the thread shall call iotHubClientFileUploadCallbackInternal passing as result FILE_UPLOAD_ERROR and as context the structure from SRS IOTHUBCLIENT 02 051. ]*/
        threadInfo->uploadBlobSavedData.iotHubClientFileUploadCallback(upload_result, threadInfo->context);
    }

    return markThreadReadyToBeGarbageCollected(threadInfo);
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_UploadToBlobAsync(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, const char* destinationFileName, const unsigned char* source, size_t size, IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK iotHubClientFileUploadCallback, void* context)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBCLIENT_02_047: [ If iotHubClientHandle is NULL then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    /*Codes_SRS_IOTHUBCLIENT_02_048: [ If destinationFileName is NULL then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    /*Codes_SRS_IOTHUBCLIENT_02_049: [ If source is NULL and size is greated than 0 then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    if (
        (iotHubClientHandle == NULL) ||
        (destinationFileName == NULL) ||
        ((source == NULL) && (size > 0))
        )
    {
        LogError("invalid parameters IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle = %p , const char* destinationFileName = %s, const unsigned char* source= %p, size_t size = %lu, IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK iotHubClientFileUploadCallback = %p, void* context = %p",
            iotHubClientHandle,
            destinationFileName,
            source,
            (unsigned long)size,
            iotHubClientFileUploadCallback,
            context
        );
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_02_051: [IoTHubClient_UploadToBlobAsync shall copy the souce, size, iotHubClientFileUploadCallback, context into a structure.]*/
        HTTPWORKER_THREAD_INFO *threadInfo = allocateUploadToBlob(destinationFileName, iotHubClientHandle, context);
        if (threadInfo == NULL)
        {
            /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
            LogError("unable to create upload thread info");
            result = IOTHUB_CLIENT_ERROR;
        }
        else if ((result = initializeUploadToBlobData(threadInfo, source, size, iotHubClientFileUploadCallback)) != IOTHUB_CLIENT_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
            LogError("unable to initialize upload blob info");
            result = IOTHUB_CLIENT_ERROR;
        }
        /*Codes_SRS_IOTHUBCLIENT_02_052: [ IoTHubClient_UploadToBlobAsync shall spawn a thread passing the structure build in SRS IOTHUBCLIENT 02 051 as thread data.]*/
        else if ((result = startHttpWorkerThread(iotHubClientHandle, threadInfo, uploadingThread)) != IOTHUB_CLIENT_OK)
        {
            /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
            LogError("unable to start upload thread");
            freeHttpWorkerThreadInfo(threadInfo);
        }
        else
        {
            result = IOTHUB_CLIENT_OK;
        }
    }

    return result;
}

static int uploadMultipleBlock_thread(void* data)
{
    HTTPWORKER_THREAD_INFO* threadInfo = (HTTPWORKER_THREAD_INFO*)data;
    IOTHUB_CLIENT_CORE_LL_HANDLE llHandle = threadInfo->iotHubClientHandle->IoTHubClientLLHandle;

    /*Codes_SRS_IOTHUBCLIENT_99_078: [ The thread shall call `IoTHubClientCore_LL_UploadMultipleBlocksToBlob` or `IoTHubClientCore_LL_UploadMultipleBlocksToBlobEx` passing the information packed in the structure. ]*/
    IOTHUB_CLIENT_RESULT result;

    srand((unsigned int)get_time(NULL));

    if (threadInfo->uploadBlobMultiblockSavedData.getDataCallback != NULL)
    {
        result = IoTHubClientCore_LL_UploadMultipleBlocksToBlob(llHandle, threadInfo->destinationFileName, threadInfo->uploadBlobMultiblockSavedData.getDataCallback, threadInfo->context);
    }
    else
    {
        result = IoTHubClientCore_LL_UploadMultipleBlocksToBlobEx(llHandle, threadInfo->destinationFileName, threadInfo->uploadBlobMultiblockSavedData.getDataCallbackEx, threadInfo->context);
    }
    (void)markThreadReadyToBeGarbageCollected(threadInfo);

    return result;
}

IOTHUB_CLIENT_RESULT IoTHubClientCore_UploadMultipleBlocksToBlobAsync(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, const char* destinationFileName, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK getDataCallback, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX getDataCallbackEx, void* context)
{
    IOTHUB_CLIENT_RESULT result;

    /*Codes_SRS_IOTHUBCLIENT_99_072: [ If `iotHubClientHandle` is `NULL` then `IoTHubClient_UploadMultipleBlocksToBlobAsync(Ex)` shall fail and return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
    /*Codes_SRS_IOTHUBCLIENT_99_073: [ If `destinationFileName` is `NULL` then `IoTHubClient_UploadMultipleBlocksToBlobAsync(Ex)` shall fail and return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
    /*Codes_SRS_IOTHUBCLIENT_99_074: [ If `getDataCallback` is `NULL` then `IoTHubClient_UploadMultipleBlocksToBlobAsync(Ex)` shall fail and return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
    if (
        (iotHubClientHandle == NULL) ||
        (destinationFileName == NULL) ||
        ((getDataCallback == NULL) && (getDataCallbackEx == NULL))
        )
    {
        LogError("invalid parameters iotHubClientHandle = %p , destinationFileName = %p, getDataCallback = %p, getDataCallbackEx = %p",
            iotHubClientHandle,
            destinationFileName,
            getDataCallback,
            getDataCallbackEx
        );
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBCLIENT_99_075: [ `IoTHubClient_UploadMultipleBlocksToBlobAsync(Ex)` shall copy the `destinationFileName`, `getDataCallback`, `context`  and `iotHubClientHandle` into a structure. ]*/
        HTTPWORKER_THREAD_INFO *threadInfo = allocateUploadToBlob(destinationFileName, iotHubClientHandle, context);
        if (threadInfo == NULL)
        {
            /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
            LogError("unable to create upload thread info");
            result = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            /*Codes_SRS_IOTHUBCLIENT_99_075: [ `IoTHubClient_UploadMultipleBlocksToBlobAsync(Ex)` shall copy the `destinationFileName`, `getDataCallback`, `context`  and `iotHubClientHandle` into a structure. ]*/
            threadInfo->uploadBlobMultiblockSavedData.getDataCallback = getDataCallback;
            threadInfo->uploadBlobMultiblockSavedData.getDataCallbackEx = getDataCallbackEx;

            if ((result = startHttpWorkerThread(iotHubClientHandle, threadInfo, uploadMultipleBlock_thread)) != IOTHUB_CLIENT_OK)
            {
                /*Codes_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
                LogError("unable to start upload thread");
                freeHttpWorkerThreadInfo(threadInfo);
            }
            else
            {
                /*Codes_SRS_IOTHUBCLIENT_99_077: [ If copying to the structure and spawning the thread succeeds, then `IoTHubClient_UploadMultipleBlocksToBlobAsync(Ex)` shall return `IOTHUB_CLIENT_OK`. ]*/
                result = IOTHUB_CLIENT_OK;
            }
        }
    }
    return result;
}

#endif /*DONT_USE_UPLOADTOBLOB*/

IOTHUB_CLIENT_RESULT IoTHubClientCore_SendEventToOutputAsync(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, const char* outputName, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    if ((iotHubClientHandle == NULL) || (outputName == NULL) || (eventMessageHandle == NULL))
    {
        // Codes_SRS_IOTHUBCLIENT_31_100: [ If `iotHubClientHandle`, `outputName`, or `eventConfirmationCallback` is `NULL`, `IoTHubClient_SendEventToOutputAsync` shall return `IOTHUB_CLIENT_INVALID_ARG`. ]
        LogError("Invalid argument (iotHubClientHandle=%p, outputName=%p, eventMessageHandle=%p)", iotHubClientHandle, outputName, eventMessageHandle);
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else
    {
        // Codes_SRS_IOTHUBCLIENT_31_101: [ `IoTHubClient_SendEventToOutputAsync` shall set the outputName of the message to send. ]
        if (IoTHubMessage_SetOutputName(eventMessageHandle, outputName) != IOTHUB_MESSAGE_OK)
        {
            LogError("IoTHubMessage_SetOutputName failed");
            result = IOTHUB_CLIENT_ERROR;
        }
        // Codes_SRS_IOTHUBCLIENT_31_102: [ `IoTHubClient_SendEventToOutputAsync` shall invoke `IoTHubClient_SendEventAsync` to send the message. ]
        else if ((result = IoTHubClientCore_SendEventAsync(iotHubClientHandle, eventMessageHandle, eventConfirmationCallback, userContextCallback)) != IOTHUB_CLIENT_OK)
        {
            LogError("Call into IoTHubClient_SendEventAsync failed, result=%d", result);
        }
    }

    return result;
}


IOTHUB_CLIENT_RESULT IoTHubClientCore_SetInputMessageCallback(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, const char* inputName, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC eventHandlerCallback, void* userContextCallback)
{
    IOTHUB_CLIENT_RESULT result;

    if (iotHubClientHandle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("NULL iothubClientHandle");
    }
    else
    {
        IOTHUB_CLIENT_CORE_INSTANCE* iotHubClientInstance = (IOTHUB_CLIENT_CORE_INSTANCE*)iotHubClientHandle;

        // Codes_SRS_IOTHUBCLIENT_31_098: [ `IoTHubClient_SetMessageCallback` shall start the worker thread if it was not previously started. ]
        if ((result = StartWorkerThreadIfNeeded(iotHubClientInstance)) != IOTHUB_CLIENT_OK)
        {
            result = IOTHUB_CLIENT_ERROR;
            LogError("Could not start worker thread");
        }
        else
        {
            if (Lock(iotHubClientInstance->LockHandle) != LOCK_OK)
            {
                result = IOTHUB_CLIENT_ERROR;
                LogError("Could not acquire lock");
            }
            else
            {
                // Codes_SRS_IOTHUBCLIENT_31_099: [ `IoTHubClient_SetMessageCallback` shall call `IoTHubClient_LL_SetInputMessageCallback`, passing its input arguments ]
                IOTHUB_INPUTMESSAGE_CALLBACK_CONTEXT inputMessageCallbackContext;
                inputMessageCallbackContext.iotHubClientHandle = iotHubClientHandle;
                inputMessageCallbackContext.eventHandlerCallback = eventHandlerCallback;
                inputMessageCallbackContext.userContextCallback = userContextCallback;

                result = IoTHubClientCore_LL_SetInputMessageCallbackEx(iotHubClientInstance->IoTHubClientLLHandle, inputName, iothub_ll_inputmessage_callback, (void*)&inputMessageCallbackContext, sizeof(inputMessageCallbackContext));
                (void)Unlock(iotHubClientInstance->LockHandle);
            }
        }
    }

    return result;
}

#ifdef USE_EDGE_MODULES

HTTPWORKER_THREAD_INFO * allocateMethodInvoke(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, const char* deviceId, const char* moduleId, const char* methodName, const char* methodPayload, unsigned int timeout, IOTHUB_METHOD_INVOKE_CALLBACK methodInvokeCallback, void* context)
{
    HTTPWORKER_THREAD_INFO* threadInfo = (HTTPWORKER_THREAD_INFO*)malloc(sizeof(HTTPWORKER_THREAD_INFO));
    if (threadInfo == NULL)
    {
        LogError("unable to allocate thread object");
    }
    else
    {
        memset(threadInfo, 0, sizeof(HTTPWORKER_THREAD_INFO));
        threadInfo->workerThreadType = HTTPWORKER_THREAD_INVOKE_METHOD;
        threadInfo->iotHubClientHandle = iotHubClientHandle;
        threadInfo->context = context;

        threadInfo->invokeMethodSavedData.timeout = timeout;
        threadInfo->invokeMethodSavedData.methodInvokeCallback = methodInvokeCallback;

        if ((mallocAndStrcpy_s((char**)&threadInfo->invokeMethodSavedData.deviceId, deviceId) != 0) ||
            ((moduleId != NULL) && mallocAndStrcpy_s((char**)&threadInfo->invokeMethodSavedData.moduleId, moduleId) != 0) ||
            (mallocAndStrcpy_s((char**)&threadInfo->invokeMethodSavedData.methodName, methodName) != 0) ||
            (mallocAndStrcpy_s((char**)&threadInfo->invokeMethodSavedData.methodPayload, methodPayload) != 0))
        {
            LogError("Allocating resources failed");
            freeHttpWorkerThreadInfo(threadInfo);
            threadInfo = NULL;
        }
        else if ((threadInfo->lockGarbage = Lock_Init()) == NULL)
        {
            LogError("unable to allocate a lock");
            freeHttpWorkerThreadInfo(threadInfo);
            threadInfo = NULL;
        }
    }

    return threadInfo;
}

static int uploadMethodInvoke_thread(void* data)
{
    IOTHUB_CLIENT_RESULT result;

    HTTPWORKER_THREAD_INFO* threadInfo = (HTTPWORKER_THREAD_INFO*)data;

    srand((unsigned int)get_time(NULL));

    int responseStatus;
    unsigned char* responsePayload = NULL;
    size_t responsePayloadSize;

    result = IoTHubClientCore_LL_GenericMethodInvoke(threadInfo->iotHubClientHandle->IoTHubClientLLHandle,
                                                     threadInfo->invokeMethodSavedData.deviceId,
                                                     threadInfo->invokeMethodSavedData.moduleId,
                                                     threadInfo->invokeMethodSavedData.methodName,
                                                     threadInfo->invokeMethodSavedData.methodPayload,
                                                     threadInfo->invokeMethodSavedData.timeout,
                                                     &responseStatus,
                                                     &responsePayload,
                                                     &responsePayloadSize);

    if (threadInfo->invokeMethodSavedData.methodInvokeCallback != NULL)
    {
        threadInfo->invokeMethodSavedData.methodInvokeCallback(result, responseStatus, responsePayload, responsePayloadSize, threadInfo->context);
    }

    if (responsePayload != NULL)
    {
        free(responsePayload);
    }

    (void)markThreadReadyToBeGarbageCollected(threadInfo);
    return result;
}


IOTHUB_CLIENT_RESULT IoTHubClientCore_GenericMethodInvoke(IOTHUB_CLIENT_CORE_HANDLE iotHubClientHandle, const char* deviceId, const char* moduleId, const char* methodName, const char* methodPayload, unsigned int timeout, IOTHUB_METHOD_INVOKE_CALLBACK methodInvokeCallback, void* context)
{
    IOTHUB_CLIENT_RESULT result;
    HTTPWORKER_THREAD_INFO *threadInfo;

    if ((iotHubClientHandle == NULL) || (deviceId == NULL) || (methodName == NULL) || (methodPayload == NULL))
    {
        LogError("Invalid argument (iotHubClientHandle=%p, deviceId=%p, methodName=%p, methodPayload=%p)", iotHubClientHandle, deviceId, methodName, methodPayload);
        result = IOTHUB_CLIENT_INVALID_ARG;
    }
    else if ((threadInfo = allocateMethodInvoke(iotHubClientHandle, deviceId, moduleId, methodName, methodPayload, timeout, methodInvokeCallback, context)) == NULL)
    {
        LogError("failed allocating method invoke thread info");
        result = IOTHUB_CLIENT_ERROR;
    }
    else if ((result = startHttpWorkerThread(iotHubClientHandle, threadInfo, uploadMethodInvoke_thread)) != IOTHUB_CLIENT_OK)
    {
        LogError("unable to start method invoke thread");
        freeHttpWorkerThreadInfo(threadInfo);
    }
    else
    {
        result = IOTHUB_CLIENT_OK;
    }
    return result;
}
#endif /* USE_EDGE_MODULES */

