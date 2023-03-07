// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is under development and it is subject to change

#ifndef IOTHUB_MESSAGING_H
#define IOTHUB_MESSAGING_H

#include "umock_c/umock_c_prod.h"

#include "iothub_messaging_ll.h"

#ifdef __cplusplus
extern "C"
{
#else
#endif

typedef struct IOTHUB_MESSAGING_CLIENT_INSTANCE_TAG* IOTHUB_MESSAGING_CLIENT_HANDLE;

/** @brief    Creates a IoT Hub Service Client Messaging handle for use it in consequent APIs.
*
* @param    serviceClientHandle    Service client handle.
*
* @return    A non-NULL @c IOTHUB_MESSAGING_CLIENT_HANDLE value that is used when
*             invoking other functions for IoT Hub DeviceMethod and @c NULL on failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_MESSAGING_CLIENT_HANDLE, IoTHubMessaging_Create, IOTHUB_SERVICE_CLIENT_AUTH_HANDLE, serviceClientHandle);

/** @brief    Disposes of resources allocated by the IoT Hub Service Client Messaging.
*
* @param    messagingClientHandle    The handle created by a call to the create function.
*/
MOCKABLE_FUNCTION(, void, IoTHubMessaging_Destroy, IOTHUB_MESSAGING_CLIENT_HANDLE, messagingClientHandle);

/** @brief    Opens connection to IoTHub.
*
* @param    messagingClientHandle    The handle created by a call to the create function.
* @param    openCompleteCallback      The callback specified by the user for receiving
*                                     confirmation of the connection opened.
*                                     The user can specify a @c NULL value here to
*                                     indicate that no callback is required.
* @param    userContextCallback        User specified context that will be provided to the
*                                     callback. This can be @c NULL.
*
* @return    IOTHUB_MESSAGING_OK upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_MESSAGING_RESULT, IoTHubMessaging_Open, IOTHUB_MESSAGING_CLIENT_HANDLE, messagingClientHandle, IOTHUB_OPEN_COMPLETE_CALLBACK, openCompleteCallback, void*, userContextCallback);

/** @brief    Closes connection to IoTHub.
*
* @param    messagingClientHandle    The handle created by a call to the create function.
*/
MOCKABLE_FUNCTION(, void, IoTHubMessaging_Close, IOTHUB_MESSAGING_CLIENT_HANDLE, messagingClientHandle);

/**
* @brief    Asynchronous call to send the message to a specified device.
*
* @param    messagingClientHandle        The handle created by a call to the create function.
* @param    deviceId                          The name (Id) of the device to send the message to.
* @param    message                           The message to send.
* @param    sendCompleteCallback          The callback specified by the user for receiving
*                                         confirmation of the delivery of the message.
*                                         The user can specify a @c NULL value here to
*                                         indicate that no callback is required.
* @param    userContextCallback            User specified context that will be provided to the
*                                         callback. This can be @c NULL.
*
*            @b NOTE: The application behavior is undefined if the user calls
*            the ::IoTHubMessaging_Destroy or IoTHubMessaging_Close function from within any callback.
*
* @return    IOTHUB_MESSAGING_OK upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_MESSAGING_RESULT, IoTHubMessaging_SendAsync, IOTHUB_MESSAGING_CLIENT_HANDLE, messagingClientHandle, const char*, deviceId, IOTHUB_MESSAGE_HANDLE, message, IOTHUB_SEND_COMPLETE_CALLBACK, sendCompleteCallback, void*, userContextCallback);

/**
* @brief    This API specifies a callback to be used when the device receives the message.
*
* @param    messagingClientHandle                The handle created by a call to the create function.
* @param    feedbackMessageReceivedCallback        The callback specified by the user to be used for receiveng
*                                                confirmation feedback from the deice about the recevied message.
*
* @param    userContextCallback                    User specified context that will be provided to the
*                                                 callback. This can be @c NULL.
*
*            @b NOTE: The application behavior is undefined if the user calls
*            the ::IoTHubMessaging_Destroy or IoTHubMessaging_Close function from within any callback.
*
* @return    IOTHUB_CLIENT_OK upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_MESSAGING_RESULT, IoTHubMessaging_SetFeedbackMessageCallback, IOTHUB_MESSAGING_CLIENT_HANDLE, messagingClientHandle, IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK, feedbackMessageReceivedCallback, void*, userContextCallback);

/**
* @brief    This function is meant to be called by the user when to
*           set the trusted certificate on the tls connection.
*
* @param    messagingHandle The handle created by a call to the create function.
* @param    trusted_cert    The trusted certificate that will be set.
*
* @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_MESSAGING_RESULT, IoTHubMessaging_SetTrustedCert, IOTHUB_MESSAGING_CLIENT_HANDLE, messagingClientHandle, const char*, trusted_cert);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_MESSAGING_H
