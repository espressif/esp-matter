// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_CLIENT_PRIVATE_H
#define IOTHUB_CLIENT_PRIVATE_H

#include <stdbool.h>

#include "azure_c_shared_utility/constbuffer.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "umock_c/umock_c_prod.h"

#include "iothub_message.h"
#include "internal/iothub_transport_ll_private.h"
#include "iothub_client_core_common.h"
#include "iothub_client_core_ll.h"

#ifdef USE_EDGE_MODULES
#include "internal/iothub_client_edge.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define CLIENT_DEVICE_TYPE_PREFIX "iothubclient"
#define CLIENT_DEVICE_BACKSLASH "/"

typedef bool(*IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC_EX)(MESSAGE_CALLBACK_INFO* messageData, void* userContextCallback);

MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SetMessageCallback_Ex, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC_EX, messageCallback, void*, userContextCallback);
MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SendMessageDisposition, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, MESSAGE_CALLBACK_INFO*, messageData, IOTHUBMESSAGE_DISPOSITION_RESULT, disposition);
MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SetInputMessageCallbackEx, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, const char*, inputName, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC_EX, eventHandlerCallbackEx, void *, userContextCallback, size_t, userContextCallbackLength);
MOCKABLE_FUNCTION(, int, IoTHubClientCore_LL_GetTransportCallbacks, TRANSPORT_CALLBACKS_INFO*, transport_cb);

#ifdef USE_EDGE_MODULES
/* (Should be replaced after iothub_client refactor)*/
MOCKABLE_FUNCTION(, IOTHUB_CLIENT_EDGE_HANDLE, IoTHubClientCore_LL_GetEdgeHandle, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle);
MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_GenericMethodInvoke, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, const char*, deviceId, const char*, moduleId, const char*, methodName, const char*, methodPayload, unsigned int, timeout, int*, responseStatus, unsigned char**, responsePayload, size_t*, responsePayloadSize);
#endif

typedef struct IOTHUB_MESSAGE_LIST_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;
    void* context;
    DLIST_ENTRY entry;
    tickcounter_ms_t ms_timesOutAfter; /* a value of "0" means "no timeout", if the IOTHUBCLIENT_LL's handle tickcounter > msTimesOutAfer then the message shall timeout*/
    tickcounter_ms_t message_timeout_value;
}IOTHUB_MESSAGE_LIST;

typedef struct IOTHUB_DEVICE_TWIN_TAG
{
    uint32_t item_id;
    tickcounter_ms_t ms_timesOutAfter; /* a value of "0" means "no timeout", if the IOTHUBCLIENT_LL's handle tickcounter > msTimesOutAfer then the message shall timeout*/
    IOTHUB_CLIENT_REPORTED_STATE_CALLBACK reported_state_callback;
    CONSTBUFFER_HANDLE report_data_handle;
    void* context;
    DLIST_ENTRY entry;
    IOTHUB_CLIENT_CORE_LL_HANDLE client_handle;
    IOTHUB_DEVICE_HANDLE device_handle;
} IOTHUB_DEVICE_TWIN;

union IOTHUB_IDENTITY_INFO_TAG
{
    IOTHUB_DEVICE_TWIN* device_twin;
    IOTHUB_MESSAGE_LIST* iothub_message;
};

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_PRIVATE_H */
