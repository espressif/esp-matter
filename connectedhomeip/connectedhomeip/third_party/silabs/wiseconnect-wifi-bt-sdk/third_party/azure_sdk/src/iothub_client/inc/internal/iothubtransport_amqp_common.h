// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBTRANSPORTAMQP_COMMON_H
#define IOTHUBTRANSPORTAMQP_COMMON_H

#include "azure_c_shared_utility/strings.h"
#include "umock_c/umock_c_prod.h"
#include "internal/iothub_transport_ll_private.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct AMQP_TRANSPORT_PROXY_OPTIONS_TAG
{
    const char* host_address;
    int port;
    const char* username;
    const char* password;
} AMQP_TRANSPORT_PROXY_OPTIONS;

typedef XIO_HANDLE(*AMQP_GET_IO_TRANSPORT)(const char* target_fqdn, const AMQP_TRANSPORT_PROXY_OPTIONS* amqp_transport_proxy_options);

MOCKABLE_FUNCTION(, TRANSPORT_LL_HANDLE, IoTHubTransport_AMQP_Common_Create, const IOTHUBTRANSPORT_CONFIG*, config, AMQP_GET_IO_TRANSPORT, get_io_transport, TRANSPORT_CALLBACKS_INFO*, cb_info, void*, ctx);
MOCKABLE_FUNCTION(, void, IoTHubTransport_AMQP_Common_Destroy, TRANSPORT_LL_HANDLE, handle);
MOCKABLE_FUNCTION(, int, IoTHubTransport_AMQP_Common_Subscribe, IOTHUB_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, void, IoTHubTransport_AMQP_Common_Unsubscribe, IOTHUB_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, int, IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin, IOTHUB_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, void, IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin, IOTHUB_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubTransport_AMQP_Common_GetTwinAsync, IOTHUB_DEVICE_HANDLE, handle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK, completionCallback, void*, callbackContext);
MOCKABLE_FUNCTION(, int, IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod, IOTHUB_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, void, IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod, IOTHUB_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, int, IoTHubTransport_AMQP_Common_DeviceMethod_Response, IOTHUB_DEVICE_HANDLE, handle, METHOD_HANDLE, methodId, const unsigned char*, response, size_t, response_size, int, status_response);
MOCKABLE_FUNCTION(, IOTHUB_PROCESS_ITEM_RESULT, IoTHubTransport_AMQP_Common_ProcessItem, TRANSPORT_LL_HANDLE, handle, IOTHUB_IDENTITY_TYPE, item_type, IOTHUB_IDENTITY_INFO*, iothub_item);
MOCKABLE_FUNCTION(, void, IoTHubTransport_AMQP_Common_DoWork, TRANSPORT_LL_HANDLE, handle);
MOCKABLE_FUNCTION(, int, IoTHubTransport_AMQP_Common_SetRetryPolicy, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_RETRY_POLICY, retryPolicy, size_t, retryTimeoutLimitInSeconds);
MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubTransport_AMQP_Common_GetSendStatus, IOTHUB_DEVICE_HANDLE, handle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus);
MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubTransport_AMQP_Common_SetOption, TRANSPORT_LL_HANDLE, handle, const char*, option, const void*, value);
MOCKABLE_FUNCTION(, IOTHUB_DEVICE_HANDLE, IoTHubTransport_AMQP_Common_Register, TRANSPORT_LL_HANDLE, handle, const IOTHUB_DEVICE_CONFIG*, device, PDLIST_ENTRY, waitingToSend);
MOCKABLE_FUNCTION(, void, IoTHubTransport_AMQP_Common_Unregister, IOTHUB_DEVICE_HANDLE, deviceHandle);
MOCKABLE_FUNCTION(, STRING_HANDLE, IoTHubTransport_AMQP_Common_GetHostname, TRANSPORT_LL_HANDLE, handle);
MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubTransport_AMQP_Common_SendMessageDisposition, MESSAGE_CALLBACK_INFO*, message_data, IOTHUBMESSAGE_DISPOSITION_RESULT, disposition);
MOCKABLE_FUNCTION(, int, IoTHubTransport_AMQP_SetCallbackContext, TRANSPORT_LL_HANDLE, handle, void*, ctx);
MOCKABLE_FUNCTION(, int, IoTHubTransport_AMQP_Common_GetSupportedPlatformInfo, TRANSPORT_LL_HANDLE, handle, PLATFORM_INFO_OPTION*, info);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUBTRANSPORTAMQP_COMMON_H */
