// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothubtransportamqp_websockets.h"
#include "azure_c_shared_utility/wsio.h"
#include "internal/iothubtransport_amqp_common.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/shared_util_options.h"

#define DEFAULT_WS_PROTOCOL_NAME "AMQPWSB10"
#define DEFAULT_WS_RELATIVE_PATH "/$iothub/websocket"
#define DEFAULT_WS_PORT 443

static XIO_HANDLE getWebSocketsIOTransport(const char* fqdn, const AMQP_TRANSPORT_PROXY_OPTIONS* amqp_transport_proxy_options)
{
    WSIO_CONFIG ws_io_config;
    TLSIO_CONFIG tls_io_config;
    HTTP_PROXY_IO_CONFIG http_proxy_io_config;
    /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_001: [ `getIoTransportProvider` shall obtain the WebSocket IO interface handle by calling `wsio_get_interface_description`. ]*/
    const IO_INTERFACE_DESCRIPTION* io_interface_description = wsio_get_interface_description();
    XIO_HANDLE result;

    if (io_interface_description == NULL)
    {
        LogError("Failure constructing the provider interface");
        /* Codes_SRS_IoTHubTransportAMQP_WS_09_003: [If `io_interface_description` is NULL getWebSocketsIOTransport shall return NULL.] */
        result = NULL;
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_003: [ - `hostname` shall be set to `fqdn`. ]*/
        ws_io_config.hostname = fqdn;
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_004: [ - `port` shall be set to 443. ]*/
        ws_io_config.port = DEFAULT_WS_PORT;
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_005: [ - `protocol` shall be set to `AMQPWSB10`. ]*/
        ws_io_config.protocol = DEFAULT_WS_PROTOCOL_NAME;
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_006: [ - `resource_name` shall be set to `/$iothub/websocket`. ]*/
        ws_io_config.resource_name = DEFAULT_WS_RELATIVE_PATH;
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_007: [ - `underlying_io_interface` shall be set to the TLS IO interface description. ]*/
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_009: [ `getIoTransportProvider` shall obtain the TLS IO interface handle by calling `platform_get_default_tlsio`. ]*/
        ws_io_config.underlying_io_interface = platform_get_default_tlsio();

        /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_029: [ If `platform_get_default_tlsio` returns NULL, NULL shall be set in the WebSocket IO parameters structure for the interface description and parameters. ]*/
        if (ws_io_config.underlying_io_interface == NULL)
        {
            ws_io_config.underlying_io_parameters = NULL;
        }
        else
        {
            /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_008: [ - `underlying_io_parameters` shall be set to the TLS IO arguments. ]*/
            ws_io_config.underlying_io_parameters = &tls_io_config;

            /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_010: [ The TLS IO parameters shall be a `TLSIO_CONFIG` structure filled as below: ]*/
            /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_011: [ - `hostname` shall be set to `fqdn`. ]*/
            tls_io_config.hostname = fqdn;
            /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_012: [ - `port` shall be set to 443. ]*/
            tls_io_config.port = DEFAULT_WS_PORT;

            if (amqp_transport_proxy_options != NULL)
            {
                /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_015: [ - If `amqp_transport_proxy_options` is not NULL, `underlying_io_interface` shall be set to the HTTP proxy IO interface description. ]*/
                /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_022: [ `getIoTransportProvider` shall obtain the HTTP proxy IO interface handle by calling `http_proxy_io_get_interface_description`. ]*/
                tls_io_config.underlying_io_interface = http_proxy_io_get_interface_description();

                /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_028: [ If `http_proxy_io_get_interface_description` returns NULL, NULL shall be set in the TLS IO parameters structure for the interface description and parameters. ]*/
                if (tls_io_config.underlying_io_interface == NULL)
                {
                    tls_io_config.underlying_io_parameters = NULL;
                }
                else
                {
                    /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_016: [ - If `amqp_transport_proxy_options` is not NULL `underlying_io_parameters` shall be set to the HTTP proxy IO arguments. ]*/
                    tls_io_config.underlying_io_parameters = &http_proxy_io_config;

                    /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_023: [ The HTTP proxy IO arguments shall be an `HTTP_PROXY_IO_CONFIG` structure, filled as below: ]*/
                    /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_026: [ - `proxy_hostname`, `proxy_port`, `username` and `password` shall be copied from the `mqtt_transport_proxy_options` argument. ]*/
                    http_proxy_io_config.proxy_hostname = amqp_transport_proxy_options->host_address;
                    http_proxy_io_config.proxy_port = amqp_transport_proxy_options->port;
                    http_proxy_io_config.username = amqp_transport_proxy_options->username;
                    http_proxy_io_config.password = amqp_transport_proxy_options->password;
                    /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_024: [ - `hostname` shall be set to `fully_qualified_name`. ]*/
                    http_proxy_io_config.hostname = fqdn;
                    /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_025: [ - `port` shall be set to 443. ]*/
                    http_proxy_io_config.port = DEFAULT_WS_PORT;
                }
            }
            else
            {
                /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_013: [ - If `amqp_transport_proxy_options` is NULL, `underlying_io_interface` shall be set to NULL. ]*/
                tls_io_config.underlying_io_interface = NULL;
                /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_014: [ - If `amqp_transport_proxy_options` is NULL `underlying_io_parameters` shall be set to NULL. ]*/
                tls_io_config.underlying_io_parameters = NULL;
            }
        }

        /* Codes_SRS_IoTHubTransportAMQP_WS_09_004: [getWebSocketsIOTransport shall return the XIO_HANDLE created using xio_create().] */
        /* Codes_SRS_IOTHUBTRANSPORTAMQP_WS_01_002: [ `getIoTransportProvider` shall call `xio_create` while passing the WebSocket IO interface description to it and the WebSocket configuration as a WSIO_CONFIG structure, filled as below: ]*/
        result = xio_create(io_interface_description, &ws_io_config);
    }

    return result;
}

// API functions
static TRANSPORT_LL_HANDLE IoTHubTransportAMQP_WS_Create(const IOTHUBTRANSPORT_CONFIG* config, TRANSPORT_CALLBACKS_INFO* cb_info, void* ctx)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_001: [IoTHubTransportAMQP_WS_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubTransport_AMQP_Common_Create function, passing `config` and getWebSocketsIOTransport.]
    return IoTHubTransport_AMQP_Common_Create(config, getWebSocketsIOTransport, cb_info, ctx);
}

static IOTHUB_PROCESS_ITEM_RESULT IoTHubTransportAMQP_WS_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_014: [IoTHubTransportAMQP_WS_ProcessItem shall invoke IoTHubTransport_AMQP_Common_ProcessItem() and return its result.]
    return IoTHubTransport_AMQP_Common_ProcessItem(handle, item_type, iothub_item);
}

static void IoTHubTransportAMQP_WS_DoWork(TRANSPORT_LL_HANDLE handle)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_015: [IoTHubTransportAMQP_WS_DoWork shall call into the IoTHubTransport_AMQP_Common_DoWork()]
    IoTHubTransport_AMQP_Common_DoWork(handle);
}

static int IoTHubTransportAMQP_WS_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_012: [IoTHubTransportAMQP_WS_Subscribe shall subscribe for D2C messages by calling into the IoTHubTransport_AMQP_Common_Subscribe().]
    return IoTHubTransport_AMQP_Common_Subscribe(handle);
}

static void IoTHubTransportAMQP_WS_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_013: [IoTHubTransportAMQP_WS_Unsubscribe shall subscribe for D2C messages by calling into the IoTHubTransport_AMQP_Common_Unsubscribe().]
    IoTHubTransport_AMQP_Common_Unsubscribe(handle);
}

static int IoTHubTransportAMQP_WS_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_008: [IoTHubTransportAMQP_WS_Subscribe_DeviceTwin shall invoke IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin() and return its result.]
    return IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin(handle);
}

static void IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_009: [IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin shall invoke IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin()]
    IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin(handle);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_WS_GetTwinAsync(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK completionCallback, void* callbackContext)
{
    // Codes_SRS_IOTHUBTRANSPORTAMQP_WS_09_020: [IoTHubTransportAMQP_WS_GetTwinAsync shall invoke IoTHubTransport_AMQP_Common_GetTwinAsync()]
    return IoTHubTransport_AMQP_Common_GetTwinAsync(handle, completionCallback, callbackContext);
}

static int IoTHubTransportAMQP_WS_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_010: [IoTHubTransportAMQP_WS_Subscribe_DeviceMethod shall invoke IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod() and return its result.]
    return IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(handle);
}

static void IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_011: [IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod shall invoke IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod()]
    IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod(handle);
}

static int IoTHubTransportAMQP_WS_DeviceMethod_Response(IOTHUB_DEVICE_HANDLE handle, METHOD_HANDLE methodId, const unsigned char* response, size_t response_size, int status_response)
{
    return IoTHubTransport_AMQP_Common_DeviceMethod_Response(handle, methodId, response, response_size, status_response);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_WS_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_016: [IoTHubTransportAMQP_WS_GetSendStatus shall get the send status by calling into the IoTHubTransport_AMQP_Common_GetSendStatus()]
    return IoTHubTransport_AMQP_Common_GetSendStatus(handle, iotHubClientStatus);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_WS_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_017: [IoTHubTransportAMQP_WS_SetOption shall set the options by calling into the IoTHubTransport_AMQP_Common_SetOption()]
    return IoTHubTransport_AMQP_Common_SetOption(handle, option, value);
}

static IOTHUB_DEVICE_HANDLE IoTHubTransportAMQP_WS_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, PDLIST_ENTRY waitingToSend)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_006: [IoTHubTransportAMQP_WS_Register shall register the device by calling into the IoTHubTransport_AMQP_Common_Register().]
    return IoTHubTransport_AMQP_Common_Register(handle, device, waitingToSend);
}

static void IoTHubTransportAMQP_WS_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_007: [IoTHubTransportAMQP_WS_Unregister shall unregister the device by calling into the IoTHubTransport_AMQP_Common_Unregister().]
    IoTHubTransport_AMQP_Common_Unregister(deviceHandle);
}

static void IoTHubTransportAMQP_WS_Destroy(TRANSPORT_LL_HANDLE handle)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_005: [IoTHubTransportAMQP_WS_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubTransport_AMQP_Common_Destroy().]
    IoTHubTransport_AMQP_Common_Destroy(handle);
}

static int IoTHubTransportAMQP_WS_SetRetryPolicy(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
    return IoTHubTransport_AMQP_Common_SetRetryPolicy(handle, retryPolicy, retryTimeoutLimitInSeconds);
}

static STRING_HANDLE IoTHubTransportAMQP_WS_GetHostname(TRANSPORT_LL_HANDLE handle)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_09_018: [IoTHubTransportAMQP_WS_GetHostname shall get the hostname by calling into the IoTHubTransport_AMQP_Common_GetHostname()]
    return IoTHubTransport_AMQP_Common_GetHostname(handle);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_WS_SendMessageDisposition(MESSAGE_CALLBACK_INFO* message_data, IOTHUBMESSAGE_DISPOSITION_RESULT disposition)
{
    // Codes_SRS_IoTHubTransportAMQP_WS_10_001[**IoTHubTransportAMQP_WS_SendMessageDisposition shall sned the message disposition by calling into the IoTHubTransport_AMQP_Common_SendMessageDisposition()]
    return IoTHubTransport_AMQP_Common_SendMessageDisposition(message_data, disposition);
}

static int IotHubTransportAMQP_WS_Subscribe_InputQueue(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    LogError("AMQP WS does not support input queues");
    return (int)-1;
}

static void IotHubTransportAMQP_WS_Unsubscribe_InputQueue(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    LogError("AMQP WS does not support input queues");
}

static int IoTHubTransportAMQP_WS_SetCallbackContext(TRANSPORT_LL_HANDLE handle, void* ctx)
{
    return IoTHubTransport_AMQP_SetCallbackContext(handle, ctx);
}

static int IoTHubTransportAMQP_WS_GetSupportedPlatformInfo(TRANSPORT_LL_HANDLE handle, PLATFORM_INFO_OPTION* info)
{
    return IoTHubTransport_AMQP_Common_GetSupportedPlatformInfo(handle, info);
}

static TRANSPORT_PROVIDER thisTransportProvider_WebSocketsOverTls =
{
    IoTHubTransportAMQP_WS_SendMessageDisposition,                     /*pfIotHubTransport_Send_Message_Disposition IoTHubTransport_Send_Message_Disposition;*/
    IoTHubTransportAMQP_WS_Subscribe_DeviceMethod,                     /*pfIoTHubTransport_Subscribe_DeviceMethod IoTHubTransport_Subscribe_DeviceMethod;*/
    IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod,                   /*pfIoTHubTransport_Unsubscribe_DeviceMethod IoTHubTransport_Unsubscribe_DeviceMethod;*/
    IoTHubTransportAMQP_WS_DeviceMethod_Response,
    IoTHubTransportAMQP_WS_Subscribe_DeviceTwin,                       /*pfIoTHubTransport_Subscribe_DeviceTwin IoTHubTransport_Subscribe_DeviceTwin;*/
    IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin,                     /*pfIoTHubTransport_Unsubscribe_DeviceTwin IoTHubTransport_Unsubscribe_DeviceTwin;*/
    IoTHubTransportAMQP_WS_ProcessItem,                                /*pfIoTHubTransport_ProcessItem IoTHubTransport_ProcessItem;*/
    IoTHubTransportAMQP_WS_GetHostname,                                /*pfIoTHubTransport_GetHostname IoTHubTransport_GetHostname;*/
    IoTHubTransportAMQP_WS_SetOption,                                  /*pfIoTHubTransport_SetOption IoTHubTransport_SetOption;*/
    IoTHubTransportAMQP_WS_Create,                                     /*pfIoTHubTransport_Create IoTHubTransport_Create;*/
    IoTHubTransportAMQP_WS_Destroy,                                    /*pfIoTHubTransport_Destroy IoTHubTransport_Destroy;*/
    IoTHubTransportAMQP_WS_Register,                                   /*pfIotHubTransport_Register IoTHubTransport_Register;*/
    IoTHubTransportAMQP_WS_Unregister,                                 /*pfIotHubTransport_Unregister IoTHubTransport_Unegister;*/
    IoTHubTransportAMQP_WS_Subscribe,                                  /*pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;*/
    IoTHubTransportAMQP_WS_Unsubscribe,                                /*pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;*/
    IoTHubTransportAMQP_WS_DoWork,                                     /*pfIoTHubTransport_DoWork IoTHubTransport_DoWork;*/
    IoTHubTransportAMQP_WS_SetRetryPolicy,                             /*pfIoTHubTransport_SetRetryLogic IoTHubTransport_SetRetryPolicy;*/
    IoTHubTransportAMQP_WS_GetSendStatus,                              /*pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus;*/
    IotHubTransportAMQP_WS_Subscribe_InputQueue,                       /*pfIoTHubTransport_Subscribe_InputQueue IoTHubTransport_Subscribe_InputQueue; */
    IotHubTransportAMQP_WS_Unsubscribe_InputQueue,                     /*pfIoTHubTransport_Unsubscribe_InputQueue IoTHubTransport_Unsubscribe_InputQueue; */
    IoTHubTransportAMQP_WS_SetCallbackContext,                         /*pfIoTHubTransport_SetCallbackContext IoTHubTransport_SetCallbackContext; */
    IoTHubTransportAMQP_WS_GetTwinAsync,                               /*pfIoTHubTransport_GetTwinAsync IoTHubTransport_GetTwinAsync;*/
    IoTHubTransportAMQP_WS_GetSupportedPlatformInfo                         /*pfIoTHubTransport_GetSupportedPlatformInfo IoTHubTransport_GetSupportedPlatformInfo;*/
};

/* Codes_SRS_IoTHubTransportAMQP_WS_09_019: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it's fields:
IoTHubTransport_Send_Message_Disposition = IoTHubTransportAMQP_Send_Message_Disposition
IoTHubTransport_Subscribe_DeviceMethod = IoTHubTransportAMQP_WS_Subscribe_DeviceMethod
IoTHubTransport_Unsubscribe_DeviceMethod = IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod
IoTHubTransport_Subscribe_DeviceTwin = IoTHubTransportAMQP_WS_Subscribe_DeviceTwin
IoTHubTransport_Unsubscribe_DeviceTwin = IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin
IoTHubTransport_ProcessItem - IoTHubTransportAMQP_WS_ProcessItem
IoTHubTransport_GetHostname = IoTHubTransportAMQP_WS_GetHostname
IoTHubTransport_Create = IoTHubTransportAMQP_WS_Create
IoTHubTransport_Destroy = IoTHubTransportAMQP_WS_Destroy
IoTHubTransport_Register = IoTHubTransportAMQP_WS_Register
IoTHubTransport_Unregister = IoTHubTransportAMQP_WS_Unregister
IoTHubTransport_Subscribe = IoTHubTransportAMQP_WS_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportAMQP_WS_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportAMQP_WS_DoWork
IoTHubTransport_SetRetryLogic = IoTHubTransportAMQP_WS_SetRetryLogic
IoTHubTransport_SetOption = IoTHubTransportAMQP_WS_SetOption
IoTHubTransport_GetSendStatus = IoTHubTransportAMQP_WS_GetSendStatus
IoTHubTransport_GetSupportedPlatformInfo = IoTHubTransportAMQP_WS_GetSupportedPlatformInfo] */
extern const TRANSPORT_PROVIDER* AMQP_Protocol_over_WebSocketsTls(void)
{
    return &thisTransportProvider_WebSocketsOverTls;
}
