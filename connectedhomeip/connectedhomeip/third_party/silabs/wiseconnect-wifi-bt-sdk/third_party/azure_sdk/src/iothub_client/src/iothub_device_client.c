// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/gballoc.h"

#include "iothub_client_core.h"
#include "iothub_device_client.h"

IOTHUB_DEVICE_CLIENT_HANDLE IoTHubDeviceClient_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    return (IOTHUB_DEVICE_CLIENT_HANDLE)IoTHubClientCore_CreateFromConnectionString(connectionString, protocol);
}

IOTHUB_DEVICE_CLIENT_HANDLE IoTHubDeviceClient_Create(const IOTHUB_CLIENT_CONFIG* config)
{
    return (IOTHUB_DEVICE_CLIENT_HANDLE)IoTHubClientCore_Create(config);
}

IOTHUB_DEVICE_CLIENT_HANDLE IoTHubDeviceClient_CreateWithTransport(TRANSPORT_HANDLE transportHandle, const IOTHUB_CLIENT_CONFIG* config)
{
    return (IOTHUB_DEVICE_CLIENT_HANDLE)IoTHubClientCore_CreateWithTransport(transportHandle, config);
}

IOTHUB_DEVICE_CLIENT_HANDLE IoTHubDeviceClient_CreateFromDeviceAuth(const char* iothub_uri, const char* device_id, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    return (IOTHUB_DEVICE_CLIENT_HANDLE)IoTHubClientCore_CreateFromDeviceAuth(iothub_uri, device_id, protocol);
}

void IoTHubDeviceClient_Destroy(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle)
{
    IoTHubClientCore_Destroy((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_SendEventAsync(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback)
{
    return IoTHubClientCore_SendEventAsync((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, eventMessageHandle, eventConfirmationCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_GetSendStatus(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    return IoTHubClientCore_GetSendStatus((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, iotHubClientStatus);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_SetMessageCallback(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback)
{
    return IoTHubClientCore_SetMessageCallback((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, messageCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_SetConnectionStatusCallback(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK connectionStatusCallback, void * userContextCallback)
{
    return IoTHubClientCore_SetConnectionStatusCallback((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, connectionStatusCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_SetRetryPolicy(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
    return IoTHubClientCore_SetRetryPolicy((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, retryPolicy, retryTimeoutLimitInSeconds);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_GetRetryPolicy(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY* retryPolicy, size_t* retryTimeoutLimitInSeconds)
{
    return IoTHubClientCore_GetRetryPolicy((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, retryPolicy, retryTimeoutLimitInSeconds);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_GetLastMessageReceiveTime(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime)
{
    return IoTHubClientCore_GetLastMessageReceiveTime((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, lastMessageReceiveTime);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_SetOption(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, const char* optionName, const void* value)
{
    return IoTHubClientCore_SetOption((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, optionName, value);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_SetDeviceTwinCallback(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback)
{
    return IoTHubClientCore_SetDeviceTwinCallback((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, deviceTwinCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_GetTwinAsync(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback)
{
    return IoTHubClientCore_GetTwinAsync((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, deviceTwinCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_SendReportedState(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, const unsigned char* reportedState, size_t size, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK reportedStateCallback, void* userContextCallback)
{
    return IoTHubClientCore_SendReportedState((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, reportedState, size, reportedStateCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_SetDeviceMethodCallback(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC deviceMethodCallback, void* userContextCallback)
{
    return IoTHubClientCore_SetDeviceMethodCallback((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, deviceMethodCallback, userContextCallback);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_DeviceMethodResponse(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, METHOD_HANDLE methodId, const unsigned char* response, size_t respSize, int statusCode)
{
    return IoTHubClientCore_DeviceMethodResponse((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, methodId, response, respSize, statusCode);
}

#ifndef DONT_USE_UPLOADTOBLOB

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_UploadToBlobAsync(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, const char* destinationFileName, const unsigned char* source, size_t size, IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK iotHubClientFileUploadCallback, void* context)
{
    return IoTHubClientCore_UploadToBlobAsync((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, destinationFileName, source, size, iotHubClientFileUploadCallback, context);
}

IOTHUB_CLIENT_RESULT IoTHubDeviceClient_UploadMultipleBlocksToBlobAsync(IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle, const char* destinationFileName, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX getDataCallbackEx, void* context)
{
    return IoTHubClientCore_UploadMultipleBlocksToBlobAsync((IOTHUB_CLIENT_CORE_HANDLE)iotHubClientHandle, destinationFileName, NULL, getDataCallbackEx, context);
}

#endif /*DONT_USE_UPLOADTOBLOB*/
