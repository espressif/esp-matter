// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//**********************************************************************
// NOTE: THIS HEADER IS DEPRECATED
// 
// Functions in this header will be maintained for backward compatability.
// New applications should use iothub_device_client.h.
// 
//**********************************************************************

/** @file iothub_client.h
*    @brief Extends the IoTHubClient_LL with additional features.
*
*    @note  DEPRECATED. New users use iothub_device_client.h for IoTHubClient APIs.
*    @details IoTHubClient is a module that extends the IoTHubCLient_LL
*             module with 2 features:
*                - scheduling the work for the IoTHubCLient from a
*                  thread, so that the user does not need to create their
*                  own thread
*                - thread-safe APIs
*/

#ifndef IOTHUB_CLIENT_H
#define IOTHUB_CLIENT_H

#include <stddef.h>
#include <stdint.h>

#include "umock_c/umock_c_prod.h"
#include "iothub_transport_ll.h"
#include "iothub_client_core_ll.h"
#include "iothub_client_core.h"
#include "iothub_client_ll.h"

#ifndef IOTHUB_CLIENT_INSTANCE_TYPE
typedef IOTHUB_CLIENT_CORE_HANDLE IOTHUB_CLIENT_HANDLE;
#define IOTHUB_CLIENT_INSTANCE_TYPE
#endif // IOTHUB_CLIENT_INSTANCE


#ifdef __cplusplus
extern "C"
{
#endif

    /**
    * @deprecated IoTHubClient_CreateFromConnectionString is deprecated.  Use IoTHubDeviceClient_CreateFromConnectionString instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_HANDLE, IoTHubClient_CreateFromConnectionString, const char*, connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol);

    /**
    * @deprecated IoTHubClient_Create is deprecated.  Use IoTHubDeviceClient_Create instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_HANDLE, IoTHubClient_Create, const IOTHUB_CLIENT_CONFIG*, config);

    /**
    * @deprecated IoTHubClient_CreateWithTransport is deprecated.  Use IoTHubDeviceClient_CreateWithTransport instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_HANDLE, IoTHubClient_CreateWithTransport, TRANSPORT_HANDLE, transportHandle, const IOTHUB_CLIENT_CONFIG*, config);

    /**
    * @deprecated IoTHubClient_CreateFromDeviceAuth is deprecated.  Use IoTHubDeviceClient_CreateFromDeviceAuth instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_HANDLE, IoTHubClient_CreateFromDeviceAuth, const char*, iothub_uri, const char*, device_id, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol);

    /**
    * @deprecated IoTHubClient_Destroy is deprecated.  Use IoTHubDeviceClient_Destroy instead.
    */
    MOCKABLE_FUNCTION(, void, IoTHubClient_Destroy, IOTHUB_CLIENT_HANDLE, iotHubClientHandle);

    /**
    * @deprecated IoTHubClient_SendEventAsync is deprecated.  Use IoTHubDeviceClient_SendEventAsync instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_SendEventAsync, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_MESSAGE_HANDLE, eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, eventConfirmationCallback, void*, userContextCallback);

    /**
    * @deprecated IoTHubClient_GetSendStatus is deprecated.  Use IoTHubDeviceClient_GetSendStatus instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_GetSendStatus, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus);

    /**
    * @deprecated IoTHubClient_SetMessageCallback is deprecated.  Use IoTHubDeviceClient_SetMessageCallback instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_SetMessageCallback, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC, messageCallback, void*, userContextCallback);

    /**
    * @deprecated IoTHubClient_SetConnectionStatusCallback is deprecated.  Use IoTHubDeviceClient_SetConnectionStatusCallback instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_SetConnectionStatusCallback, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK, connectionStatusCallback, void*, userContextCallback);

    /**
    * @deprecated IoTHubClient_SetRetryPolicy is deprecated.  Use IoTHubDeviceClient_SetRetryPolicy instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_SetRetryPolicy, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY, retryPolicy, size_t, retryTimeoutLimitInSeconds);

    /**
    * @deprecated IoTHubClient_GetRetryPolicy is deprecated.  Use IoTHubDeviceClient_GetRetryPolicy instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_GetRetryPolicy, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY*, retryPolicy, size_t*, retryTimeoutLimitInSeconds);

    /**
    * @deprecated IoTHubClient_GetLastMessageReceiveTime is deprecated.  Use IoTHubDeviceClient_GetLastMessageReceiveTime instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_GetLastMessageReceiveTime, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, time_t*, lastMessageReceiveTime);

    /**
    * @deprecated IoTHubClient_SetOption is deprecated.  Use IoTHubDeviceClient_SetOption instead.
    */   
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_SetOption, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, const char*, optionName, const void*, value);

    /**
    * @deprecated IoTHubClient_SetDeviceTwinCallback is deprecated.  Use IoTHubDeviceClient_SetDeviceTwinCallback instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_SetDeviceTwinCallback, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK, deviceTwinCallback, void*, userContextCallback);

    /**
    * @deprecated IoTHubClient_SendReportedState is deprecated.  Use IoTHubDeviceClient_SendReportedState instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_SendReportedState, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, const unsigned char*, reportedState, size_t, size, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK, reportedStateCallback, void*, userContextCallback);

    /**
    * @deprecated IoTHubClient_SetDeviceMethodCallback is deprecated.  Use IoTHubDeviceClient_SetDeviceMethodCallback instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_SetDeviceMethodCallback, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC, deviceMethodCallback, void*, userContextCallback);

    /**
    * @deprecated IoTHubClient_SetDeviceMethodCallback_Ex is deprecated.  Use IoTHubDeviceClient_SetDeviceMethodCallback instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_SetDeviceMethodCallback_Ex, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK, inboundDeviceMethodCallback, void*, userContextCallback);

    /**
    * @deprecated IoTHubClient_DeviceMethodResponse is deprecated.  Use IoTHubDeviceClient_SetDeviceMethodCallback instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_DeviceMethodResponse, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, METHOD_HANDLE, methodId, const unsigned char*, response, size_t, response_size, int, statusCode);

#ifndef DONT_USE_UPLOADTOBLOB
    /**
    * @deprecated IoTHubClient_UploadToBlobAsync is deprecated.  Use IoTHubDeviceClient_UploadToBlobAsync instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_UploadToBlobAsync, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, const char*, destinationFileName, const unsigned char*, source, size_t, size, IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK, iotHubClientFileUploadCallback, void*, context);

    /**
    * @deprecated IoTHubClient_UploadMultipleBlocksToBlobAsync is deprecated.  Use IoTHubDeviceClient_UploadMultipleBlocksToBlobAsync instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_UploadMultipleBlocksToBlobAsync, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, const char*, destinationFileName, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK, getDataCallback, void*, context);

    /**
    * @deprecated IoTHubClient_UploadMultipleBlocksToBlobAsyncEx is deprecated.  Use IoTHubDeviceClient_UploadMultipleBlocksToBlobAsync instead.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_UploadMultipleBlocksToBlobAsyncEx, IOTHUB_CLIENT_HANDLE, iotHubClientHandle, const char*, destinationFileName, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX, getDataCallbackEx, void*, context);

#endif /* DONT_USE_UPLOADTOBLOB */

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_H */
