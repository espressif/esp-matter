// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/* Shared structures and enums for iothub convenience layer and LL layer */

#ifndef IOTHUB_CLIENT_CORE_COMMON_H
#define IOTHUB_CLIENT_CORE_COMMON_H

#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"

#include "iothub_transport_ll.h"
#include "iothub_message.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define IOTHUB_CLIENT_FILE_UPLOAD_RESULT_VALUES \
    FILE_UPLOAD_OK, \
    FILE_UPLOAD_ERROR

    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_CLIENT_FILE_UPLOAD_RESULT, IOTHUB_CLIENT_FILE_UPLOAD_RESULT_VALUES)
        typedef void(*IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK)(IOTHUB_CLIENT_FILE_UPLOAD_RESULT result, void* userContextCallback);

#define IOTHUB_CLIENT_RESULT_VALUES       \
    IOTHUB_CLIENT_OK,                     \
    IOTHUB_CLIENT_INVALID_ARG,            \
    IOTHUB_CLIENT_ERROR,                  \
    IOTHUB_CLIENT_INVALID_SIZE,           \
    IOTHUB_CLIENT_INDEFINITE_TIME

    /** @brief Enumeration specifying the status of calls to various APIs in this module.
    */

    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);

    typedef void(*IOTHUB_METHOD_INVOKE_CALLBACK)(IOTHUB_CLIENT_RESULT result, int responseStatus, unsigned char* responsePayload, size_t responsePayloadSize, void* context);

#define IOTHUB_CLIENT_RETRY_POLICY_VALUES     \
    IOTHUB_CLIENT_RETRY_NONE,                   \
    IOTHUB_CLIENT_RETRY_IMMEDIATE,                  \
    IOTHUB_CLIENT_RETRY_INTERVAL,      \
    IOTHUB_CLIENT_RETRY_LINEAR_BACKOFF,      \
    IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF,                 \
    IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF_WITH_JITTER,                 \
    IOTHUB_CLIENT_RETRY_RANDOM

    /** @brief Enumeration passed in by the IoT Hub when the event confirmation
    *           callback is invoked to indicate status of the event processing in
    *           the hub.
    */
    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_CLIENT_RETRY_POLICY, IOTHUB_CLIENT_RETRY_POLICY_VALUES);

    struct IOTHUBTRANSPORT_CONFIG_TAG;
    typedef struct IOTHUBTRANSPORT_CONFIG_TAG IOTHUBTRANSPORT_CONFIG;

#define IOTHUB_CLIENT_STATUS_VALUES       \
    IOTHUB_CLIENT_SEND_STATUS_IDLE,       \
    IOTHUB_CLIENT_SEND_STATUS_BUSY

    /** @brief Enumeration returned by the ::IoTHubClient_LL_GetSendStatus
    *           API to indicate the current sending status of the IoT Hub client.
    */
    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);

#define IOTHUB_IDENTITY_TYPE_VALUE  \
    IOTHUB_TYPE_TELEMETRY,          \
    IOTHUB_TYPE_DEVICE_TWIN,        \
    IOTHUB_TYPE_DEVICE_METHODS,     \
    IOTHUB_TYPE_EVENT_QUEUE
    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_IDENTITY_TYPE, IOTHUB_IDENTITY_TYPE_VALUE);

#define IOTHUB_PROCESS_ITEM_RESULT_VALUE    \
    IOTHUB_PROCESS_OK,                      \
    IOTHUB_PROCESS_ERROR,                   \
    IOTHUB_PROCESS_NOT_CONNECTED,           \
    IOTHUB_PROCESS_CONTINUE
    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_PROCESS_ITEM_RESULT, IOTHUB_PROCESS_ITEM_RESULT_VALUE);

#define IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES \
    IOTHUBMESSAGE_ACCEPTED, \
    IOTHUBMESSAGE_REJECTED, \
    IOTHUBMESSAGE_ABANDONED

    /** @brief Enumeration returned by the callback which is invoked whenever the
    *           IoT Hub sends a message to the device.
    */
    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES);

#define IOTHUB_CLIENT_IOTHUB_METHOD_STATUS_VALUES \
    IOTHUB_CLIENT_IOTHUB_METHOD_STATUS_SUCCESS,   \
    IOTHUB_CLIENT_IOTHUB_METHOD_STATUS_ERROR      \

    /** @brief Enumeration returned by remotely executed functions
    */
    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_CLIENT_IOTHUB_METHOD_STATUS, IOTHUB_CLIENT_IOTHUB_METHOD_STATUS_VALUES);


#define IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES     \
    IOTHUB_CLIENT_CONFIRMATION_OK,                   \
    IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY,      \
    IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT,      \
    IOTHUB_CLIENT_CONFIRMATION_ERROR                 \

    /** @brief Enumeration passed in by the IoT Hub when the event confirmation
    *           callback is invoked to indicate status of the event processing in
    *           the hub.
    */
    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_CLIENT_CONFIRMATION_RESULT, IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES);

#define IOTHUB_CLIENT_CONNECTION_STATUS_VALUES             \
    IOTHUB_CLIENT_CONNECTION_AUTHENTICATED,                \
    IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED               \


    /** @brief Enumeration passed in by the IoT Hub when the connection status
    *           callback is invoked to indicate status of the connection in
    *           the hub.
    */
    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_CLIENT_CONNECTION_STATUS, IOTHUB_CLIENT_CONNECTION_STATUS_VALUES);

#define IOTHUB_CLIENT_CONNECTION_STATUS_REASON_VALUES      \
    IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN,            \
    IOTHUB_CLIENT_CONNECTION_DEVICE_DISABLED,              \
    IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL,               \
    IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED,                \
    IOTHUB_CLIENT_CONNECTION_NO_NETWORK,                   \
    IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR,          \
    IOTHUB_CLIENT_CONNECTION_OK,                           \
    IOTHUB_CLIENT_CONNECTION_NO_PING_RESPONSE              \

    /** @brief Enumeration passed in by the IoT Hub when the connection status
    *           callback is invoked to indicate status of the connection in
    *           the hub.
    */
    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_CLIENT_CONNECTION_STATUS_REASON, IOTHUB_CLIENT_CONNECTION_STATUS_REASON_VALUES);

#define TRANSPORT_TYPE_VALUES \
    TRANSPORT_LL, /*LL comes from "LowLevel" */ \
    TRANSPORT_THREADED

    MU_DEFINE_ENUM_WITHOUT_INVALID(TRANSPORT_TYPE, TRANSPORT_TYPE_VALUES);

#define DEVICE_TWIN_UPDATE_STATE_VALUES \
    DEVICE_TWIN_UPDATE_COMPLETE, \
    DEVICE_TWIN_UPDATE_PARTIAL

    MU_DEFINE_ENUM_WITHOUT_INVALID(DEVICE_TWIN_UPDATE_STATE, DEVICE_TWIN_UPDATE_STATE_VALUES);

    typedef void(*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK)(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback);
    typedef void(*IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK)(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContextCallback);
    typedef IOTHUBMESSAGE_DISPOSITION_RESULT (*IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC)(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback);

    typedef void(*IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK)(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback);
    typedef void(*IOTHUB_CLIENT_REPORTED_STATE_CALLBACK)(int status_code, void* userContextCallback);
    typedef int(*IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC)(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback);
    typedef int(*IOTHUB_CLIENT_INBOUND_DEVICE_METHOD_CALLBACK)(const char* method_name, const unsigned char* payload, size_t size, METHOD_HANDLE method_id, void* userContextCallback);


#define IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_RESULT_VALUES \
    IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_OK, \
    IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_ABORT

    MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_RESULT, IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_RESULT_VALUES);

    /**
    *  @brief           Callback invoked by IoTHubClient_UploadMultipleBlocksToBlobAsync requesting the chunks of data to be uploaded.
    *  @param result    The result of the upload of the previous block of data provided by the user (IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX only)
    *  @param data      Next block of data to be uploaded, to be provided by the user when this callback is invoked.
    *  @param size      Size of the data parameter.
    *  @param context   User context provided on the call to IoTHubClient_UploadMultipleBlocksToBlobAsync.
    *  @remarks         If the user wants to abort the upload, the callback should return IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_ABORT
    *                   It should return IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_OK otherwise.
    *                   If a NULL is provided for parameter "data" and/or zero is provided for "size", the user indicates to the client that the complete file has been uploaded.
    *                   In such case this callback will be invoked only once more to indicate the status of the final block upload.
    *                   If result is not FILE_UPLOAD_OK, the upload is cancelled and this callback stops being invoked.
    *                   When this callback is called for the last time, no data or size is expected, so data and size are NULL
    */
    typedef void(*IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK)(IOTHUB_CLIENT_FILE_UPLOAD_RESULT result, unsigned char const ** data, size_t* size, void* context);
    typedef IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_RESULT(*IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_CALLBACK_EX)(IOTHUB_CLIENT_FILE_UPLOAD_RESULT result, unsigned char const ** data, size_t* size, void* context);

    /** @brief    This struct captures IoTHub client configuration. */
    typedef struct IOTHUB_CLIENT_CONFIG_TAG
    {
        /** @brief A function pointer that is passed into the @c IoTHubClientCreate.
        *    A function definition for AMQP is defined in the include @c iothubtransportamqp.h.
        *   A function definition for HTTP is defined in the include @c iothubtransporthttp.h
        *   A function definition for MQTT is defined in the include @c iothubtransportmqtt.h */
        IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;

        /** @brief    A string that identifies the device. */
        const char* deviceId;

        /** @brief    The device key used to authenticate the device.
        If neither deviceSasToken nor deviceKey is present then the authentication is assumed x509.*/
        const char* deviceKey;

        /** @brief    The device SAS Token used to authenticate the device in place of device key.
        If neither deviceSasToken nor deviceKey is present then the authentication is assumed x509.*/
        const char* deviceSasToken;

        /** @brief    The IoT Hub name to which the device is connecting. */
        const char* iotHubName;

        /** @brief    IoT Hub suffix goes here, e.g., private.azure-devices-int.net. */
        const char* iotHubSuffix;

        const char* protocolGatewayHostName;
    } IOTHUB_CLIENT_CONFIG;

    /** @brief    This struct captures IoTHub client device configuration. */
    typedef struct IOTHUB_CLIENT_DEVICE_CONFIG_TAG
    {
        /** @brief A function pointer that is passed into the @c IoTHubClientCreate.
        *    A function definition for AMQP is defined in the include @c iothubtransportamqp.h.
        *   A function definition for HTTP is defined in the include @c iothubtransporthttp.h
        *   A function definition for MQTT is defined in the include @c iothubtransportmqtt.h */
        IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;

        /** @brief a transport handle implementing the protocol */
        void * transportHandle;

        /** @brief    A string that identifies the device. */
        const char* deviceId;

        /** @brief    The device key used to authenticate the device.
        x509 authentication is is not supported for multiplexed connections*/
        const char* deviceKey;

        /** @brief    The device SAS Token used to authenticate the device in place of device key.
        x509 authentication is is not supported for multiplexed connections.*/
        const char* deviceSasToken;
    } IOTHUB_CLIENT_DEVICE_CONFIG;

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_CORE_COMMON_H */
