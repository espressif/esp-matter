// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/wsio.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "iothubtransportmqtt_websockets.h"
#include "internal/iothubtransport_mqtt_common.h"

static XIO_HANDLE getWebSocketsIOTransport(const char* fully_qualified_name, const MQTT_TRANSPORT_PROXY_OPTIONS* mqtt_transport_proxy_options)
{
    XIO_HANDLE result;
    /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_001: [ `getIoTransportProvider` shall obtain the WebSocket IO interface handle by calling `wsio_get_interface_description`. ]*/
    const IO_INTERFACE_DESCRIPTION* io_interface_description = wsio_get_interface_description();
    TLSIO_CONFIG tls_io_config;
    HTTP_PROXY_IO_CONFIG http_proxy_io_config;

    if (io_interface_description == NULL)
    {
        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_013: [ If `wsio_get_interface_description` returns NULL `getIoTransportProvider` shall return NULL. ] */
        LogError("Failure constructing the provider interface");
        result = NULL;
    }
    else
    {
        WSIO_CONFIG ws_io_config;

        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_003: [ - `hostname` shall be set to `fully_qualified_name`. ]*/
        ws_io_config.hostname = fully_qualified_name;
        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_004: [ - `port` shall be set to 443. ]*/
        ws_io_config.port = 443;
        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_005: [ - `protocol` shall be set to `MQTT`. ]*/
        ws_io_config.protocol = "MQTT";
        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_006: [ - `resource_name` shall be set to `/$iothub/websocket`. ]*/
        ws_io_config.resource_name = "/$iothub/websocket";
        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_007: [ - `underlying_io_interface` shall be set to the TLS IO interface description. ]*/
        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_009: [ `getIoTransportProvider` shall obtain the TLS IO interface handle by calling `platform_get_default_tlsio`. ]*/
        ws_io_config.underlying_io_interface = platform_get_default_tlsio();

        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_029: [ If `platform_get_default_tlsio` returns NULL, NULL shall be set in the WebSocket IO parameters structure for the interface description and parameters. ]*/
        if (ws_io_config.underlying_io_interface == NULL)
        {
            ws_io_config.underlying_io_parameters = NULL;
        }
        else
        {
            /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_008: [ - `underlying_io_parameters` shall be set to the TLS IO arguments. ]*/
            /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_010: [ The TLS IO parameters shall be a TLSIO_CONFIG structure filled as below: ]*/
            ws_io_config.underlying_io_parameters = &tls_io_config;

            /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_011: [ - `hostname` shall be set to `fully_qualified_name`. ]*/
            tls_io_config.hostname = fully_qualified_name;
            /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_012: [ - `port` shall be set to 443. ]*/
            tls_io_config.port = 443;

            if (mqtt_transport_proxy_options != NULL)
            {
                /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_015: [ - If `mqtt_transport_proxy_options` is not NULL, `underlying_io_interface` shall be set to the HTTP proxy IO interface description. ]*/
                /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_022: [ `getIoTransportProvider` shall obtain the HTTP proxy IO interface handle by calling `http_proxy_io_get_interface_description`. ]*/
                tls_io_config.underlying_io_interface = http_proxy_io_get_interface_description();

                /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_028: [ If `http_proxy_io_get_interface_description` returns NULL, NULL shall be set in the TLS IO parameters structure for the interface description and parameters. ]*/
                if (tls_io_config.underlying_io_interface == NULL)
                {
                    tls_io_config.underlying_io_parameters = NULL;
                }
                else
                {
                    /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_016: [ - If `mqtt_transport_proxy_options` is not NULL `underlying_io_parameters` shall be set to the HTTP proxy IO arguments. ]*/
                    tls_io_config.underlying_io_parameters = &http_proxy_io_config;

                    /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_023: [ The HTTP proxy IO arguments shall be an `HTTP_PROXY_IO_CONFIG` structure, filled as below: ]*/
                    /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_026: [ - `proxy_hostname`, `proxy_port`, `username` and `password` shall be copied from the `mqtt_transport_proxy_options` argument. ]*/
                    http_proxy_io_config.proxy_hostname = mqtt_transport_proxy_options->host_address;
                    http_proxy_io_config.proxy_port = mqtt_transport_proxy_options->port;
                    http_proxy_io_config.username = mqtt_transport_proxy_options->username;
                    http_proxy_io_config.password = mqtt_transport_proxy_options->password;
                    /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_024: [ - `hostname` shall be set to `fully_qualified_name`. ]*/
                    http_proxy_io_config.hostname = fully_qualified_name;
                    /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_025: [ - `port` shall be set to 443. ]*/
                    http_proxy_io_config.port = 443;
                }
            }
            else
            {
                /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_013: [ - If `mqtt_transport_proxy_options` is NULL, `underlying_io_interface` shall be set to NULL ]*/
                tls_io_config.underlying_io_interface = NULL;
                /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_014: [ - If `mqtt_transport_proxy_options` is NULL `underlying_io_parameters` shall be set to NULL. ]*/
                tls_io_config.underlying_io_parameters = NULL;
            }
        }

        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_012: [ `getIoTransportProvider` shall return the `XIO_HANDLE` returned by `xio_create`. ] */
        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_01_002: [ `getIoTransportProvider` shall call `xio_create` while passing the WebSocket IO interface description to it and the WebSocket configuration as a WSIO_CONFIG structure, filled as below ]*/
        result = xio_create(io_interface_description, &ws_io_config);
    }
    return result;
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_001: [ IoTHubTransportMqtt_WS_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Create function. ] */
static TRANSPORT_LL_HANDLE IoTHubTransportMqtt_WS_Create(const IOTHUBTRANSPORT_CONFIG* config, TRANSPORT_CALLBACKS_INFO* cb_info, void* ctx)
{
    return IoTHubTransport_MQTT_Common_Create(config, getWebSocketsIOTransport, cb_info, ctx);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_002: [ IoTHubTransportMqtt_WS_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Destroy function. ] */
static void IoTHubTransportMqtt_WS_Destroy(TRANSPORT_LL_HANDLE handle)
{
    IoTHubTransport_MQTT_Common_Destroy(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_005: [ IoTHubTransportMqtt_WS_Subscribe shall subscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Subscribe function. ] */
static int IoTHubTransportMqtt_WS_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
    return IoTHubTransport_MQTT_Common_Subscribe(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_006: [ IoTHubTransportMqtt_WS_Unsubscribe shall unsubscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unsubscribe function. ] */
static void IoTHubTransportMqtt_WS_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
    IoTHubTransport_MQTT_Common_Unsubscribe(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_015: [ IoTHubTransportMqtt_WS_Subscribe_DeviceMethod shall call into the IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod function ] */
static int IoTHubTransportMqtt_WS_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    return IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_016: [ IoTHubTransportMqtt_WS_Unsubscribe_DeviceMethod shall call into the IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod ] */
static void IoTHubTransportMqtt_WS_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod(handle);
}

static int IoTHubTransportMqtt_WS_DeviceMethod_Response(IOTHUB_DEVICE_HANDLE handle, METHOD_HANDLE methodId, const unsigned char* response, size_t response_size, int status_response)
{
    return IoTHubTransport_MQTT_Common_DeviceMethod_Response(handle, methodId, response, response_size, status_response);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_017: [ IoTHubTransportMqtt_WS_Subscribe_DeviceTwin shall call into the IoTHubTransport_MQTT_Common_Subscribe_DeviceTwin ] */
static int IoTHubTransportMqtt_WS_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    return IoTHubTransport_MQTT_Common_Subscribe_DeviceTwin(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_018: [ IoTHubTransportMqtt_WS_Unsubscribe_DeviceTwin shall call into the IoTHubTransport_MQTT_Common_Unsubscribe_DeviceTwin ] */
static void IoTHubTransportMqtt_WS_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    IoTHubTransport_MQTT_Common_Unsubscribe_DeviceTwin(handle);
}

// Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_09_001: [ IoTHubTransportMqtt_WS_GetTwinAsync shall call into the IoTHubTransport_MQTT_Common_GetTwinAsync ]
static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_WS_GetTwinAsync(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK completionCallback, void* callbackContext)
{
    return IoTHubTransport_MQTT_Common_GetTwinAsync(handle, completionCallback, callbackContext);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_014: [ IoTHubTransportMqtt_WS_ProcessItem shall call into the IoTHubTransport_MQTT_Common_DoWork function ] */
static IOTHUB_PROCESS_ITEM_RESULT IoTHubTransportMqtt_WS_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
{
    return IoTHubTransport_MQTT_Common_ProcessItem(handle, item_type, iothub_item);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_007: [ IoTHubTransportMqtt_WS_DoWork shall call into the IoTHubMqttAbstract_DoWork function. ] */
static void IoTHubTransportMqtt_WS_DoWork(TRANSPORT_LL_HANDLE handle)
{
    IoTHubTransport_MQTT_Common_DoWork(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_008: [ IoTHubTransportMqtt_WS_GetSendStatus shall get the send status by calling into the IoTHubMqttAbstract_GetSendStatus function. ] */
static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_WS_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    return IoTHubTransport_MQTT_Common_GetSendStatus(handle, iotHubClientStatus);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_009: [ IoTHubTransportMqtt_WS_SetOption shall set the options by calling into the IoTHubMqttAbstract_SetOption function. ] */
static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_WS_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
{
    return IoTHubTransport_MQTT_Common_SetOption(handle, option, value);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_003: [ IoTHubTransportMqtt_WS_Register shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Register function. ]*/
static IOTHUB_DEVICE_HANDLE IoTHubTransportMqtt_WS_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, PDLIST_ENTRY waitingToSend)
{
    return IoTHubTransport_MQTT_Common_Register(handle, device, waitingToSend);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_004: [ IoTHubTransportMqtt_WS_Unregister shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unregister function. ] */
static void IoTHubTransportMqtt_WS_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
    IoTHubTransport_MQTT_Common_Unregister(deviceHandle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_010: [ IoTHubTransportMqtt_WS_GetHostname shall get the hostname by calling into the IoTHubMqttAbstract_GetHostname function. ] */
static STRING_HANDLE IoTHubTransportMqtt_WS_GetHostname(TRANSPORT_LL_HANDLE handle)
{
    return IoTHubTransport_MQTT_Common_GetHostname(handle);
}

static int IoTHubTransportMqtt_WS_SetRetryPolicy(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitinSeconds)
{
    /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_25_012: [** IoTHubTransportMqtt_WS_SetRetryPolicy shall call into the IoTHubMqttAbstract_SetRetryPolicy function.]*/
    return IoTHubTransport_MQTT_Common_SetRetryPolicy(handle, retryPolicy, retryTimeoutLimitinSeconds);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_WS_SendMessageDisposition(MESSAGE_CALLBACK_INFO* message_data, IOTHUBMESSAGE_DISPOSITION_RESULT disposition)
{
    /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_10_001: [IoTHubTransportMqtt_WS_SendMessageDisposition shall send the message disposition by calling into the IoTHubTransport_MQTT_Common_SendMessageDisposition()]*/
    return IoTHubTransport_MQTT_Common_SendMessageDisposition(message_data, disposition);
}

static int IoTHubTransportMqtt_WS_Subscribe_InputQueue(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    LogError("IoTHubTransportMqtt_WS_Subscribe_InputQueue not implemented\n");
    return MU_FAILURE;
}

static void IoTHubTransportMqtt_WS_Unsubscribe_InputQueue(IOTHUB_DEVICE_HANDLE handle)
{
    LogError("IoTHubTransportMqtt_WS_Unsubscribe_InputQueue not implemented\n");
    (void)handle;
}

static int IotHubTransportMqtt_WS_SetCallbackContext(TRANSPORT_LL_HANDLE handle, void* ctx)
{
    return IoTHubTransport_MQTT_SetCallbackContext(handle, ctx);
}

static int IotHubTransportMqtt_WS_GetSupportedPlatformInfo(TRANSPORT_LL_HANDLE handle, PLATFORM_INFO_OPTION* info)
{
    return IoTHubTransport_MQTT_GetSupportedPlatformInfo(handle, info);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_011: [ This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for its fields:
IoTHubTransport_SendMessageDisposition = IoTHubTransport_WS_SendMessageDisposition
IoTHubTransport_Subscribe_DeviceMethod = IoTHubTransport_WS_Subscribe_DeviceMethod
IoTHubTransport_Unsubscribe_DeviceMethod IoTHubTransport_WS_Unsubscribe_DeviceMethod
IoTHubTransport_Subscribe_DeviceTwin IoTHubTransport_WS_Subscribe_DeviceTwin
IoTHubTransport_Unsubscribe_DeviceTwin IoTHubTransport_WS_Unsubscribe_DeviceTwin
IoTHubTransport_GetHostname = IoTHubTransportMqtt_WS_GetHostname
IoTHubTransport_Create = IoTHubTransportMqtt_WS_Create
IoTHubTransport_Destroy = IoTHubTransportMqtt_WS_Destroy
IoTHubTransport_Subscribe = IoTHubTransportMqtt_WS_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportMqtt_WS_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportMqtt_WS_DoWork
IoTHubTransport_SetOption = IoTHubTransportMqtt_WS_SetOption 
IoTHubTransport_GetSupportedPlatformInfo = IoTHubTransportMqtt_WS_GetSupportedPlatformInfo ] */
static TRANSPORT_PROVIDER thisTransportProvider_WebSocketsOverTls = {
    IoTHubTransportMqtt_WS_SendMessageDisposition,
    IoTHubTransportMqtt_WS_Subscribe_DeviceMethod,
    IoTHubTransportMqtt_WS_Unsubscribe_DeviceMethod,
    IoTHubTransportMqtt_WS_DeviceMethod_Response,
    IoTHubTransportMqtt_WS_Subscribe_DeviceTwin,
    IoTHubTransportMqtt_WS_Unsubscribe_DeviceTwin,
    IoTHubTransportMqtt_WS_ProcessItem,
    IoTHubTransportMqtt_WS_GetHostname,
    IoTHubTransportMqtt_WS_SetOption,
    IoTHubTransportMqtt_WS_Create,
    IoTHubTransportMqtt_WS_Destroy,
    IoTHubTransportMqtt_WS_Register,
    IoTHubTransportMqtt_WS_Unregister,
    IoTHubTransportMqtt_WS_Subscribe,
    IoTHubTransportMqtt_WS_Unsubscribe,
    IoTHubTransportMqtt_WS_DoWork,
    IoTHubTransportMqtt_WS_SetRetryPolicy,
    IoTHubTransportMqtt_WS_GetSendStatus,
    IoTHubTransportMqtt_WS_Subscribe_InputQueue,
    IoTHubTransportMqtt_WS_Unsubscribe_InputQueue,
    IotHubTransportMqtt_WS_SetCallbackContext,
    IoTHubTransportMqtt_WS_GetTwinAsync,
    IotHubTransportMqtt_WS_GetSupportedPlatformInfo
};

const TRANSPORT_PROVIDER* MQTT_WebSocket_Protocol(void)
{
    return &thisTransportProvider_WebSocketsOverTls;
}
