// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//**********************************************************************
// NOTE: THIS HEADER IS FOR INTERNAL USE ONLY
// 
// Applications should use iothub_device_client_ll.h or iothub_module_client_ll.h 
// instead for the official API.
// 
//**********************************************************************

#ifndef IOTHUB_CLIENT_CORE_LL_H
#define IOTHUB_CLIENT_CORE_LL_H

typedef struct IOTHUB_CLIENT_CORE_LL_HANDLE_DATA_TAG* IOTHUB_CLIENT_CORE_LL_HANDLE;

#include <time.h>
#include "umock_c/umock_c_prod.h"
#include "iothub_transport_ll.h"
#include "iothub_client_core_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_CORE_LL_HANDLE, IoTHubClientCore_LL_CreateFromConnectionString, const char*, connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_CORE_LL_HANDLE, IoTHubClientCore_LL_Create, const IOTHUB_CLIENT_CONFIG*, config);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_CORE_LL_HANDLE, IoTHubClientCore_LL_CreateWithTransport, const IOTHUB_CLIENT_DEVICE_CONFIG*, config);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_CORE_LL_HANDLE, IoTHubClientCore_LL_CreateFromDeviceAuth, const char*, iothub_uri, const char*, device_id, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol);
     MOCKABLE_FUNCTION(, void, IoTHubClientCore_LL_Destroy, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SendEventAsync, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_MESSAGE_HANDLE, eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, eventConfirmationCallback, void*, userContextCallback);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_GetSendStatus, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SetMessageCallback, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC, messageCallback, void*, userContextCallback);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SetConnectionStatusCallback, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK, connectionStatusCallback, void*, userContextCallback);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SetRetryPolicy, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY, retryPolicy, size_t, retryTimeoutLimitInSeconds);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_GetRetryPolicy, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY*, retryPolicy, size_t*, retryTimeoutLimitInSeconds);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_GetLastMessageReceiveTime, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, time_t*, lastMessageReceiveTime);
     MOCKABLE_FUNCTION(, void, IoTHubClientCore_LL_DoWork, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SetOption, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, const char*, optionName, const void*, value);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SetDeviceTwinCallback, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK, deviceTwinCallback, void*, userContextCallback);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SendReportedState, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, const unsigned char*, reportedState, size_t, size, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK, reportedStateCallback, void*, userContextCallback);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_GetTwinAsync, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK, deviceTwinCallback, void*, userContextCallback);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SetDeviceMethodCallback, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC, deviceMethodCallback, void*, userContextCallback);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SetDeviceMethodCallback_Ex, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK, inboundDeviceMethodCallback, void*, userContextCallback);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_DeviceMethodResponse, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, METHOD_HANDLE, methodId, const unsigned char*, response, size_t, respSize, int, statusCode);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SendEventToOutputAsync, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, IOTHUB_MESSAGE_HANDLE, eventMessageHandle, const char*, outputName, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, eventConfirmationCallback, void*, userContextCallback);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_SetInputMessageCallback, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, const char*, inputName, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC, eventHandlerCallback, void*, userContextCallback);

#ifndef DONT_USE_UPLOADTOBLOB
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_UploadToBlob, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, const char*, destinationFileName, const unsigned char*, source, size_t, size);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_UploadMultipleBlocksToBlob, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, const char*, destinationFileName, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK, getDataCallback, void*, context);
     MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClientCore_LL_UploadMultipleBlocksToBlobEx, IOTHUB_CLIENT_CORE_LL_HANDLE, iotHubClientHandle, const char*, destinationFileName, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX, getDataCallbackEx, void*, context);
#endif /*DONT_USE_UPLOADTOBLOB*/

#ifdef USE_EDGE_MODULES
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_CORE_LL_HANDLE, IoTHubClientCore_LL_CreateFromEnvironment, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol);
#endif
#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_CORE_LL_H */
