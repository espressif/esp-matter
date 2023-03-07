// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file iothub_device_client.h
*    @brief Extends the IoTHubCLient_LL with additional features.
*
*    @details IoTHubDeviceClient extends the IoTHubDeviceClient_LL
*             with 2 features:
*                - scheduling the work for the IoTHubDeviceClient from a
*                  thread, so that the user does not need to create their
*                  own thread
*                - thread-safe APIs
*/

#ifndef IOTHUB_DEVICE_CLIENT_H
#define IOTHUB_DEVICE_CLIENT_H

#include <stddef.h>
#include <stdint.h>

#include "umock_c/umock_c_prod.h"
#include "iothub_transport_ll.h"
#include "iothub_client_core_ll.h"
#include "iothub_client_core.h"
#include "iothub_device_client_ll.h"

#ifndef IOTHUB_DEVICE_CLIENT_INSTANCE_TYPE
typedef IOTHUB_CLIENT_CORE_HANDLE IOTHUB_DEVICE_CLIENT_HANDLE;
#define IOTHUB_DEVICE_CLIENT_INSTANCE_TYPE
#endif // IOTHUB_CLIENT_INSTANCE


#ifdef __cplusplus
extern "C"
{
#endif

    /**
    * @brief    Creates a IoT Hub client for communication with an existing
    *           IoT Hub using the specified connection string parameter.
    *
    * @param    connectionString    Pointer to a character string
    * @param    protocol            Function pointer for protocol implementation
    *
    *            Sample connection string:
    *                <blockquote>
    *                    <pre>HostName=[IoT Hub name goes here].[IoT Hub suffix goes here, e.g., private.azure-devices-int.net];DeviceId=[Device ID goes here];SharedAccessKey=[Device key goes here];</pre>
    *                   <pre>HostName=[IoT Hub name goes here].[IoT Hub suffix goes here, e.g., private.azure-devices-int.net];DeviceId=[Device ID goes here];SharedAccessSignature=SharedAccessSignature sr=[IoT Hub name goes here].[IoT Hub suffix goes here, e.g., private.azure-devices-int.net]/devices/[Device ID goes here]&sig=[SAS Token goes here]&se=[Expiry Time goes here];</pre>
    *                </blockquote>
    *
    * @return    A non-NULL @c IOTHUB_DEVICE_CLIENT_HANDLE value that is used when
    *             invoking other functions for IoT Hub client and @c NULL on failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_DEVICE_CLIENT_HANDLE, IoTHubDeviceClient_CreateFromConnectionString, const char*, connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol);

    /**
    * @brief    Creates a IoT Hub client for communication with an existing IoT
    *           Hub using the specified parameters.
    *
    * @param    config    Pointer to an @c IOTHUB_CLIENT_CONFIG structure
    *
    *           The API does not allow sharing of a connection across multiple
    *           devices. This is a blocking call.
    *
    * @return   A non-NULL @c IOTHUB_DEVICE_CLIENT_HANDLE value that is used when
    *           invoking other functions for IoT Hub client and @c NULL on failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_DEVICE_CLIENT_HANDLE, IoTHubDeviceClient_Create, const IOTHUB_CLIENT_CONFIG*, config);

    /**
    * @brief    Creates a IoT Hub client for communication with an existing IoT
    *           Hub using the specified parameters.
    *
    * @param    transportHandle     TRANSPORT_HANDLE which represents a connection.
    * @param    config              Pointer to an @c IOTHUB_CLIENT_CONFIG structure
    *
    *           The API allows sharing of a connection across multiple
    *           devices. This is a blocking call.
    *
    * @return   A non-NULL @c IOTHUB_DEVICE_CLIENT_HANDLE value that is used when
    *           invoking other functions for IoT Hub client and @c NULL on failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_DEVICE_CLIENT_HANDLE, IoTHubDeviceClient_CreateWithTransport, TRANSPORT_HANDLE, transportHandle, const IOTHUB_CLIENT_CONFIG*, config);

    /**
    * @brief    Creates a IoT Hub client for communication with an existing IoT
    *           Hub using the device auth.
    *
    * @param    iothub_uri      Pointer to an ioThub hostname received in the registration process
    * @param    device_id       Pointer to the device Id of the device
    * @param    protocol        Function pointer for protocol implementation
    *
    * @return    A non-NULL @c IOTHUB_DEVICE_CLIENT_HANDLE value that is used when
    *            invoking other functions for IoT Hub client and @c NULL on failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_DEVICE_CLIENT_HANDLE, IoTHubDeviceClient_CreateFromDeviceAuth, const char*, iothub_uri, const char*, device_id, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol);

    /**
    * @brief    Disposes of resources allocated by the IoT Hub client. This is a
    *           blocking call.
    *
    * @param    iotHubClientHandle    The handle created by a call to the create function.
    */
    MOCKABLE_FUNCTION(, void, IoTHubDeviceClient_Destroy, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle);

    /**
    * @brief    Asynchronous call to send the message specified by @p eventMessageHandle.
    *
    * @param    iotHubClientHandle              The handle created by a call to the create function.
    * @param    eventMessageHandle              The handle to an IoT Hub message.
    * @param    eventConfirmationCallback       The callback specified by the device for receiving
    *                                           confirmation of the delivery of the IoT Hub message.
    *                                           This callback can be expected to invoke the
    *                                           IoTHubDeviceClient_SendEventAsync function for the
    *                                           same message in an attempt to retry sending a failing
    *                                           message. The user can specify a @c NULL value here to
    *                                           indicate that no callback is required.
    * @param    userContextCallback             User specified context that will be provided to the
    *                                           callback. This can be @c NULL.
    *
    *           @b NOTE: The application behavior is undefined if the user calls
    *           the IoTHubDeviceClient_Destroy function from within any callback.
    * @remarks
    *           The IOTHUB_MESSAGE_HANDLE instance provided as argument is copied by the function,
    *           so this argument can be destroyed by the calling application right after IoTHubDeviceClient_SendEventAsync returns.
    *           The copy of @c eventMessageHandle is later destroyed by the iothub client when the message is effectively sent, if a failure sending it occurs, or if the client is destroyed.
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_SendEventAsync, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_MESSAGE_HANDLE, eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, eventConfirmationCallback, void*, userContextCallback);

    /**
    * @brief    This function returns the current sending status for IoTHubClient.
    *
    * @param    iotHubClientHandle        The handle created by a call to the create function.
    * @param    iotHubClientStatus        The sending state is populated at the address pointed
    *                                     at by this parameter. The value will be set to
    *                                     @c IOTHUB_CLIENT_SEND_STATUS_IDLE if there is currently
    *                                     no item to be sent and @c IOTHUB_CLIENT_SEND_STATUS_BUSY
    *                                     if there are.
    *
    * @remark    Does not return information related to uploads initiated by IoTHubDeviceClient_UploadToBlob or IoTHubDeviceClient_UploadMultipleBlocksToBlob.
    *
    * @return    IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_GetSendStatus, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus);

    /**
    * @brief    Sets up the message callback to be invoked when IoT Hub issues a
    *           message to the device. This is a blocking call.
    *
    * @param    iotHubClientHandle          The handle created by a call to the create function.
    * @param    messageCallback             The callback specified by the device for receiving
    *                                       messages from IoT Hub.
    * @param    userContextCallback         User specified context that will be provided to the
    *                                       callback. This can be @c NULL.
    *
    *           @b NOTE: The application behavior is undefined if the user calls
    *           the IoTHubDeviceClient_Destroy function from within any callback.
    *
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_SetMessageCallback, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC, messageCallback, void*, userContextCallback);

    /**
    * @brief    Sets up the connection status callback to be invoked representing the status of
    *           the connection to IOT Hub. This is a blocking call.
    *
    * @param    iotHubClientHandle              The handle created by a call to the create function.
    * @param    connectionStatusCallback        The callback specified by the device for receiving
    *                                           updates about the status of the connection to IoT Hub.
    * @param    userContextCallback             User specified context that will be provided to the
    *                                           callback. This can be @c NULL.
    *
    *           @b NOTE: The application behavior is undefined if the user calls
    *           the IoTHubDeviceClient_Destroy function from within any callback.
    *
    * @remark   Callback specified will not receive connection status change notifications for upload connections created with IoTHubDeviceClient_UploadToBlob or IoTHubDeviceClient_UploadMultipleBlocksToBlob.
    *
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_SetConnectionStatusCallback, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK, connectionStatusCallback, void*, userContextCallback);

    /**
    * @brief    Sets up the connection status callback to be invoked representing the status of
    *           the connection to IOT Hub. This is a blocking call.
    *
    * @param    iotHubClientHandle              The handle created by a call to the create function.
    * @param    retryPolicy                     The policy to use to reconnect to IoT Hub when a
    *                                           connection drops.
    * @param    retryTimeoutLimitInSeconds      Maximum amount of time(seconds) to attempt reconnection when a
    *                                           connection drops to IOT Hub.
    *
    *           @b NOTE: The application behavior is undefined if the user calls
    *           the IoTHubDeviceClient_Destroy function from within any callback.
    *
    * @remark   Uploads initiated by IoTHubDeviceClient_UploadToBlob or IoTHubDeviceClient_UploadMultipleBlocksToBlob do not have automatic retries and do not honor the retryPolicy settings.
    *
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_SetRetryPolicy, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY, retryPolicy, size_t, retryTimeoutLimitInSeconds);

    /**
    * @brief    Sets up the connection status callback to be invoked representing the status of
    * the connection to IOT Hub. This is a blocking call.
    *
    * @param    iotHubClientHandle              The handle created by a call to the create function.
    * @param    retryPolicy                     Out parameter containing the policy to use to reconnect to IoT Hub.
    * @param    retryTimeoutLimitInSeconds      Out parameter containing maximum amount of time in seconds to attempt reconnection
    *                                           to IOT Hub.
    *
    *           @b NOTE: The application behavior is undefined if the user calls
    *           the IoTHubDeviceClient_Destroy function from within any callback.
    *
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_GetRetryPolicy, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY*, retryPolicy, size_t*, retryTimeoutLimitInSeconds);

    /**
    * @brief    This function returns in the out parameter @p lastMessageReceiveTime
    *           what was the value of the @c time function when the last message was
    *           received at the client.
    *
    * @param    iotHubClientHandle           The handle created by a call to the create function.
    * @param    lastMessageReceiveTime       Out parameter containing the value of @c time function
    *                                        when the last message was received.
    *
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_GetLastMessageReceiveTime, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, time_t*, lastMessageReceiveTime);

    /**
    * @brief    This API sets a runtime option identified by parameter @p optionName
    *           to a value pointed to by @p value. @p optionName and the data type
    *           @p value is pointing to are specific for every option.
    *
    * @param    iotHubClientHandle      The handle created by a call to the create function.
    * @param    optionName              Name of the option.
    * @param    value                   The value.
    *
    * @remarks  Documentation for configuration options is available at https://github.com/Azure/azure-iot-sdk-c/blob/master/doc/Iothub_sdk_options.md.
    * 
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_SetOption, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, const char*, optionName, const void*, value);

    /**
    * @brief    This API specifies a callback to be used when the device receives a state update.
    *
    * @param    iotHubClientHandle          The handle created by a call to the create function.
    * @param    deviceTwinCallback          The callback specified by the device client to be used for updating
    *                                       the desired state. The callback will be called in response to a
    *                                       request send by the IoTHub services. The payload will be passed to the
    *                                       callback, along with two version numbers:
    *                                           - Desired:
    *                                           - LastSeenReported:
    * @param    userContextCallback         User specified context that will be provided to the
    *                                       callback. This can be @c NULL.
    *
    *           @b NOTE: The application behavior is undefined if the user calls
    *           the IoTHubDeviceClient_Destroy function from within any callback.
    *
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_SetDeviceTwinCallback, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK, deviceTwinCallback, void*, userContextCallback);

    /**
    * @brief    This API sends a report of the device's properties and their current values.
    *
    * @param    iotHubClientHandle          The handle created by a call to the create function.
    * @param    reportedState               The current device property values to be 'reported' to the IoTHub.
    * @param    reportedStateCallback       The callback specified by the device client to be called with the
    *                                       result of the transaction.
    * @param    userContextCallback         User specified context that will be provided to the
    *                                       callback. This can be @c NULL.
    *
    *           @b NOTE: The application behavior is undefined if the user calls
    *           the IoTHubDeviceClient_Destroy function from within any callback.
    *
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_SendReportedState, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, const unsigned char*, reportedState, size_t, size, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK, reportedStateCallback, void*, userContextCallback);

    /**
    * @brief    This API provides a way to retrieve the complete device Twin properties on-demand.
    *
    * @param    iotHubClientHandle       The handle created by a call to the create function.
    * @param    deviceTwinCallback       The callback invoked to provide the complete Device Twin properties once its retrieval is completed by the client.
    *                                    If any failures occur, the callback is invoked passing @c NULL as payLoad and zero as size.
    * @param    userContextCallback      User specified context that will be provided to the
    *                                    callback. This can be @c NULL.
    *
    *            @b NOTE: The application behavior is undefined if the user calls
    *            the IoTHubDeviceClient_Destroy function from within any callback.
    *
    * @return    IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_GetTwinAsync, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK, deviceTwinCallback, void*, userContextCallback);

    /**
    * @brief    This API sets the callback for async cloud to device method calls.
    *
    * @param    iotHubClientHandle              The handle created by a call to the create function.
    * @param    inboundDeviceMethodCallback     The callback which will be called by IoTHub.
    * @param    userContextCallback             User specified context that will be provided to the
    *                                           callback. This can be @c NULL.
    *
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_SetDeviceMethodCallback, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC, deviceMethodCallback, void*, userContextCallback);

    /**
    * @brief    This API responds to an asnyc method callback identified the methodId.
    *
    * @param    iotHubClientHandle      The handle created by a call to the create function.
    * @param    methodId                The methodId of the Device Method callback.
    * @param    response                The response data for the method callback.
    * @param    response_size           The size of the response data buffer.
    * @param    status_response         The status response of the method callback.
    *
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_DeviceMethodResponse, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, METHOD_HANDLE, methodId, const unsigned char*, response, size_t, response_size, int, statusCode);

#ifndef DONT_USE_UPLOADTOBLOB
    /**
    * @brief    IoTHubDeviceClient_UploadToBlobAsync uploads data from memory to a file in Azure Blob Storage.
    *
    * @param    iotHubClientHandle                  The handle created by a call to the IoTHubDeviceClient_Create function.
    * @param    destinationFileName                 The name of the file to be created in Azure Blob Storage.
    * @param    source                              The source of data.
    * @param    size                                The size of data.
    * @param    iotHubClientFileUploadCallback      A callback to be invoked when the file upload operation has finished.
    * @param    context                             A user-provided context to be passed to the file upload callback.
    *
    * @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_UploadToBlobAsync, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, const char*, destinationFileName, const unsigned char*, source, size_t, size, IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK, iotHubClientFileUploadCallback, void*, context);

    /**
    * @brief                          Uploads a file to a Blob storage in chunks, fed through the callback function provided by the user.
    * @remarks                        This function allows users to upload large files in chunks, not requiring the whole file content to be passed in memory.
    * @param iotHubClientHandle       The handle created by a call to the IoTHubDeviceClient_Create function.
    * @param destinationFileName      The name of the file to be created in Azure Blob Storage.
    * @param getDataCallbackEx        A callback to be invoked to acquire the file chunks to be uploaded, as well as to indicate the status of the upload of the previous block.
    * @param context                  Any data provided by the user to serve as context on getDataCallback.
    * @returns                        An IOTHUB_CLIENT_RESULT value indicating the success or failure of the API call.
    */
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubDeviceClient_UploadMultipleBlocksToBlobAsync, IOTHUB_DEVICE_CLIENT_HANDLE, iotHubClientHandle, const char*, destinationFileName, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX, getDataCallbackEx, void*, context);

#endif /* DONT_USE_UPLOADTOBLOB */

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_DEVICE_CLIENT_H */
