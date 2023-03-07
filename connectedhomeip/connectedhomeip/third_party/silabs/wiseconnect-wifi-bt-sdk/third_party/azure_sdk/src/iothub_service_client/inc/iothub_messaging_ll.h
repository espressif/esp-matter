// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is under development and it is subject to change

#ifndef IOTHUB_MESSAGING_LL_H
#define IOTHUB_MESSAGING_LL_H

#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/map.h"
#include "iothub_message.h"
#include "iothub_service_client_auth.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C"
{
#else
#endif


#define IOTHUB_FEEDBACK_STATUS_CODE_VALUES               \
    IOTHUB_FEEDBACK_STATUS_CODE_SUCCESS,                 \
    IOTHUB_FEEDBACK_STATUS_CODE_EXPIRED,                 \
    IOTHUB_FEEDBACK_STATUS_CODE_DELIVER_COUNT_EXCEEDED,  \
    IOTHUB_FEEDBACK_STATUS_CODE_REJECTED,                \
    IOTHUB_FEEDBACK_STATUS_CODE_UNKNOWN                  \

MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_FEEDBACK_STATUS_CODE, IOTHUB_FEEDBACK_STATUS_CODE_VALUES);

#define IOTHUB_MESSAGE_SEND_STATE_VALUES                 \
    IOTHUB_MESSAGE_SEND_STATE_NOT_SENT,                  \
    IOTHUB_MESSAGE_SEND_STATE_SEND_IN_PROGRESS,          \
    IOTHUB_MESSAGE_SEND_STATE_SENT_OK,                   \
    IOTHUB_MESSAGE_SEND_STATE_SEND_FAILED                \

MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_MESSAGE_SEND_STATE, IOTHUB_MESSAGE_SEND_STATE_VALUES);

#define IOTHUB_MESSAGING_RESULT_VALUES       \
    IOTHUB_MESSAGING_OK,                     \
    IOTHUB_MESSAGING_INVALID_ARG,            \
    IOTHUB_MESSAGING_ERROR,                  \
    IOTHUB_MESSAGING_INVALID_JSON,           \
    IOTHUB_MESSAGING_DEVICE_EXIST,           \
    IOTHUB_MESSAGING_CALLBACK_NOT_SET        \

MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_MESSAGING_RESULT, IOTHUB_MESSAGING_RESULT_VALUES);

typedef struct IOTHUB_SERVICE_FEEDBACK_RECORD_TAG
{
    char* description;
    const char* deviceId;
    const char* correlationId;
    const char* generationId;
    const char* enqueuedTimeUtc;
    IOTHUB_FEEDBACK_STATUS_CODE statusCode;
    const char* originalMessageId;
} IOTHUB_SERVICE_FEEDBACK_RECORD;

typedef struct IOTHUB_SERVICE_FEEDBACK_BATCH_TAG
{
    const char* userId;
    const char* lockToken;
    SINGLYLINKEDLIST_HANDLE feedbackRecordList;
} IOTHUB_SERVICE_FEEDBACK_BATCH;

typedef struct IOTHUB_MESSAGING_TAG* IOTHUB_MESSAGING_HANDLE;

typedef void(*IOTHUB_OPEN_COMPLETE_CALLBACK)(void* context);
typedef void(*IOTHUB_SEND_COMPLETE_CALLBACK)(void* context, IOTHUB_MESSAGING_RESULT messagingResult);
typedef void(*IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK)(void* context, IOTHUB_SERVICE_FEEDBACK_BATCH* feedbackBatch);

/** @brief    Creates a IoT Hub Service Client Messaging handle for use it in consequent APIs.
*
* @param    iotHubMessagingServiceClientHandle    Service client handle.
*
* @return    A non-NULL @c IOTHUB_MESSAGING_CLIENT_HANDLE value that is used when
*             invoking other functions for IoT Hub DeviceMethod and @c NULL on failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_MESSAGING_HANDLE, IoTHubMessaging_LL_Create, IOTHUB_SERVICE_CLIENT_AUTH_HANDLE, iotHubMessagingServiceClientHandle);

/** @brief    Disposes of resources allocated by the IoT Hub Service Client Messaging.
*
* @param    messagingClientHandle    The handle created by a call to the create function.
*/
MOCKABLE_FUNCTION(, void, IoTHubMessaging_LL_Destroy, IOTHUB_MESSAGING_HANDLE, messagingHandle);

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
MOCKABLE_FUNCTION(, IOTHUB_MESSAGING_RESULT, IoTHubMessaging_LL_Open, IOTHUB_MESSAGING_HANDLE, messagingHandle, IOTHUB_OPEN_COMPLETE_CALLBACK, openCompleteCallback, void*, userContextCallback);

/** @brief    Closes connection to IoTHub.
*
* @param    messagingClientHandle    The handle created by a call to the create function.
*/
MOCKABLE_FUNCTION(, void, IoTHubMessaging_LL_Close, IOTHUB_MESSAGING_HANDLE, messagingHandle);

/**
* @brief    Synchronous call to send the message to a specified device.
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
MOCKABLE_FUNCTION(, IOTHUB_MESSAGING_RESULT, IoTHubMessaging_LL_Send, IOTHUB_MESSAGING_HANDLE, messagingHandle, const char*, deviceId, IOTHUB_MESSAGE_HANDLE, message, IOTHUB_SEND_COMPLETE_CALLBACK, sendCompleteCallback, void*, userContextCallback);

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
MOCKABLE_FUNCTION(, IOTHUB_MESSAGING_RESULT, IoTHubMessaging_LL_SetFeedbackMessageCallback, IOTHUB_MESSAGING_HANDLE, messagingHandle, IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK, feedbackMessageReceivedCallback, void*, userContextCallback);

/**
* @brief    This function is meant to be called by the user when work
*             (sending/receiving) can be done by the IoTHubServiceClient.
*
* @param    messagingHandle    The handle created by a call to the create function.
*
*            All IoTHubServiceClient interactions (in regards to network traffic
*            and/or user level callbacks) are the effect of calling this
*            function and they take place synchronously inside _DoWork.
*/
MOCKABLE_FUNCTION(, void, IoTHubMessaging_LL_DoWork, IOTHUB_MESSAGING_HANDLE, messagingHandle);

/**
* @brief    This function is meant to be called by the user when to
*           set the trusted certificate on the tls connection.
*
* @param    messagingHandle The handle created by a call to the create function.
* @param    trusted_cert    The trusted certificate that will be set.
*
* @return   IOTHUB_CLIENT_OK upon success or an error code upon failure.
*/
MOCKABLE_FUNCTION(, IOTHUB_MESSAGING_RESULT, IoTHubMessaging_LL_SetTrustedCert, IOTHUB_MESSAGING_HANDLE, messagingHandle, const char*, trusted_cert);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_MESSAGING_LL_H
