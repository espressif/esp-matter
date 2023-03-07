// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "iothubtransportmqtt.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"
#include "internal/iothubtransport_mqtt_common.h"
#include "azure_c_shared_utility/xlogging.h"

static XIO_HANDLE getIoTransportProvider(const char* fully_qualified_name, const MQTT_TRANSPORT_PROXY_OPTIONS* mqtt_transport_proxy_options)
{
    XIO_HANDLE result;

    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_01_001: [ `getIoTransportProvider` shall obtain the TLS IO interface handle by calling `platform_get_default_tlsio`. ]*/
    const IO_INTERFACE_DESCRIPTION* io_interface_description = platform_get_default_tlsio();
    (void)mqtt_transport_proxy_options;

    if (io_interface_description == NULL)
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_013: [ If `platform_get_default_tlsio` returns NULL, `getIoTransportProvider` shall return NULL. ] */
        LogError("Failure constructing the provider interface");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_01_002: [ The TLS IO parameters shall be a `TLSIO_CONFIG` structure filled as below: ]*/
        TLSIO_CONFIG tls_io_config;

        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_01_003: [ - `hostname` shall be set to `fully_qualified_name`. ]*/
        tls_io_config.hostname = fully_qualified_name;
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_01_004: [ - `port` shall be set to 8883. ]*/
        tls_io_config.port = 8883;
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_01_005: [ - `underlying_io_interface` shall be set to NULL. ]*/
        tls_io_config.underlying_io_interface = NULL;
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_01_006: [ - `underlying_io_parameters` shall be set to NULL. ]*/
        tls_io_config.underlying_io_parameters = NULL;

        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_012: [ `getIoTransportProvider` shall return the `XIO_HANDLE` returned by `xio_create`. ] */
        result = xio_create(io_interface_description, &tls_io_config);
    }
    return result;
}

static TRANSPORT_LL_HANDLE IoTHubTransportMqtt_Create(const IOTHUBTRANSPORT_CONFIG* config, TRANSPORT_CALLBACKS_INFO* cb_info, void* ctx)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_001: [IoTHubTransportMqtt_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Create function.] */
    return IoTHubTransport_MQTT_Common_Create(config, getIoTransportProvider, cb_info, ctx);
}

static void IoTHubTransportMqtt_Destroy(TRANSPORT_LL_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_002: [ IoTHubTransportMqtt_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Destroy function. ] */
    IoTHubTransport_MQTT_Common_Destroy(handle);
}

static int IoTHubTransportMqtt_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_005: [ IoTHubTransportMqtt_Subscribe shall subscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Subscribe function. ] */
    return IoTHubTransport_MQTT_Common_Subscribe(handle);
}

static void IoTHubTransportMqtt_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_006: [ IoTHubTransportMqtt_Unsubscribe shall unsubscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unsubscribe function. ] */
    IoTHubTransport_MQTT_Common_Unsubscribe(handle);
}

/* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_026: [ IoTHubTransportMqtt_Subscribe_DeviceMethod shall subscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Subscribe_DeviceMethod function. ] */
static int IoTHubTransportMqtt_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    return IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod(handle);
}

/* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [ IoTHubTransportMqtt_Unsubscribe_DeviceMethod shall call into the IoTHubMqttAbstract_Unsubscribe_DeviceMethod function. ] */
static void IoTHubTransportMqtt_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod(handle);
}

/* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_025: [ IoTHubTransportMqtt_Subscribe_DeviceTwin shall call into the IoTHubMqttAbstract_Subscribe_DeviceTwin function. ] */
static int IoTHubTransportMqtt_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    return IoTHubTransport_MQTT_Common_Subscribe_DeviceTwin(handle);
}

/* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_024: [ IoTHubTransportMqtt_Unsubscribe_DeviceTwin shall shall call into the IoTHubMqttAbstract_Unsubscribe_DeviceTwin function. ] */
static void IoTHubTransportMqtt_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    IoTHubTransport_MQTT_Common_Unsubscribe_DeviceTwin(handle);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_GetTwinAsync(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK completionCallback, void* callbackContext)
{
    // Codes_SRS_IOTHUB_MQTT_TRANSPORT_09_001: [ IoTHubTransportMqtt_GetTwinAsync shall call into the IoTHubTransport_MQTT_Common_GetTwinAsync function. ]
    return IoTHubTransport_MQTT_Common_GetTwinAsync(handle, completionCallback, callbackContext);
}

/* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_023: [ IoTHubTransportMqtt_DeviceMethod_Response shall call into the IoTHubMqttAbstract_DeviceMethod_Response function. ] */
static int IoTHubTransportMqtt_DeviceMethod_Response(IOTHUB_DEVICE_HANDLE handle, METHOD_HANDLE methodId, const unsigned char* response, size_t response_size, int status_response)
{
    return IoTHubTransport_MQTT_Common_DeviceMethod_Response(handle, methodId, response, response_size, status_response);
}

/* Codes_SRS_IOTHUB_MQTT_TRANSPORT_10_001: [IoTHubTransportMqtt_SendMessageDisposition shall send the message disposition by calling into the IoTHubMqttAbstract_SendMessageDisposition function.] */
static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_SendMessageDisposition(MESSAGE_CALLBACK_INFO* message_data, IOTHUBMESSAGE_DISPOSITION_RESULT disposition)
{
    return IoTHubTransport_MQTT_Common_SendMessageDisposition(message_data, disposition);
}

static IOTHUB_PROCESS_ITEM_RESULT IoTHubTransportMqtt_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
{
    return IoTHubTransport_MQTT_Common_ProcessItem(handle, item_type, iothub_item);
}

/* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_054: [ IoTHubTransportMqtt_DoWork shall subscribe to the Notification and get_state Topics if they are defined. ] */
static void IoTHubTransportMqtt_DoWork(TRANSPORT_LL_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_007: [ IoTHubTransportMqtt_DoWork shall call into the IoTHubMqttAbstract_DoWork function. ] */
    IoTHubTransport_MQTT_Common_DoWork(handle);
}

static int IoTHubTransportMqtt_SetRetryPolicy(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_25_012: [** IoTHubTransportMqtt_SetRetryPolicy shall call into the IoTHubMqttAbstract_SetRetryPolicy function. ] */
    return IoTHubTransport_MQTT_Common_SetRetryPolicy(handle, retryPolicy, retryTimeoutLimitInSeconds);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_008: [ IoTHubTransportMqtt_GetSendStatus shall get the send status by calling into the IoTHubMqttAbstract_GetSendStatus function. ] */
    return IoTHubTransport_MQTT_Common_GetSendStatus(handle, iotHubClientStatus);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_009: [ IoTHubTransportMqtt_SetOption shall set the options by calling into the IoTHubMqttAbstract_SetOption function. ] */
    return IoTHubTransport_MQTT_Common_SetOption(handle, option, value);
}

static IOTHUB_DEVICE_HANDLE IoTHubTransportMqtt_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, PDLIST_ENTRY waitingToSend)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [ IoTHubTransportMqtt_Register shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Register function. ] */
    return IoTHubTransport_MQTT_Common_Register(handle, device, waitingToSend);
}

static void IoTHubTransportMqtt_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_004: [ IoTHubTransportMqtt_Unregister shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unregister function. ] */
    IoTHubTransport_MQTT_Common_Unregister(deviceHandle);
}

static STRING_HANDLE IoTHubTransportMqtt_GetHostname(TRANSPORT_LL_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_010: [ IoTHubTransportMqtt_GetHostname shall get the hostname by calling into the IoTHubMqttAbstract_GetHostname function. ] */
    return IoTHubTransport_MQTT_Common_GetHostname(handle);
}

static int IotHubTransportMqtt_Subscribe_InputQueue(IOTHUB_DEVICE_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_31_14: [ IoTHubTransportMqtt_Subscribe shall subscribe the TRANSPORT_LL_HANDLE by calling into IoTHubTransport_MQTT_Common_Subscribe_InputQueue. ] */
    return IoTHubTransport_MQTT_Common_Subscribe_InputQueue(handle);
}

static void IotHubTransportMqtt_Unsubscribe_InputQueue(IOTHUB_DEVICE_HANDLE handle)
{
    /* Codes_SRS_IOTHUBTRANSPORTAMQP_31_015: [IotHubTransportMQTT_Unsubscribe_InputQueue shall unsubscribe by calling into IoTHubTransport_MQTT_Common_Unsubscribe_InputQueue. ] */
    IoTHubTransport_MQTT_Common_Unsubscribe_InputQueue(handle);
}

static int IotHubTransportMqtt_SetCallbackContext(TRANSPORT_LL_HANDLE handle, void* ctx)
{
    return IoTHubTransport_MQTT_SetCallbackContext(handle, ctx);
}

static int IotHubTransportMqtt_GetSupportedPlatformInfo(TRANSPORT_LL_HANDLE handle, PLATFORM_INFO_OPTION* info)
{
    return IoTHubTransport_MQTT_GetSupportedPlatformInfo(handle, info);
}

static TRANSPORT_PROVIDER myfunc =
{
    IoTHubTransportMqtt_SendMessageDisposition,     /*pfIotHubTransport_SendMessageDisposition IoTHubTransport_SendMessageDisposition;*/
    IoTHubTransportMqtt_Subscribe_DeviceMethod,     /*pfIoTHubTransport_Subscribe_DeviceMethod IoTHubTransport_Subscribe_DeviceMethod;*/
    IoTHubTransportMqtt_Unsubscribe_DeviceMethod,   /*pfIoTHubTransport_Unsubscribe_DeviceMethod IoTHubTransport_Unsubscribe_DeviceMethod;*/
    IoTHubTransportMqtt_DeviceMethod_Response,      /*pfIoTHubTransport_DeviceMethod_Response IoTHubTransport_DeviceMethod_Response;*/
    IoTHubTransportMqtt_Subscribe_DeviceTwin,       /*pfIoTHubTransport_Subscribe_DeviceTwin IoTHubTransport_Subscribe_DeviceTwin;*/
    IoTHubTransportMqtt_Unsubscribe_DeviceTwin,     /*pfIoTHubTransport_Unsubscribe_DeviceTwin IoTHubTransport_Unsubscribe_DeviceTwin;*/
    IoTHubTransportMqtt_ProcessItem,                /*pfIoTHubTransport_ProcessItem IoTHubTransport_ProcessItem;*/
    IoTHubTransportMqtt_GetHostname,                /*pfIoTHubTransport_GetHostname IoTHubTransport_GetHostname;*/
    IoTHubTransportMqtt_SetOption,                  /*pfIoTHubTransport_SetOption IoTHubTransport_SetOption;*/
    IoTHubTransportMqtt_Create,                     /*pfIoTHubTransport_Create IoTHubTransport_Create;*/
    IoTHubTransportMqtt_Destroy,                    /*pfIoTHubTransport_Destroy IoTHubTransport_Destroy;*/
    IoTHubTransportMqtt_Register,                   /*pfIotHubTransport_Register IoTHubTransport_Register;*/
    IoTHubTransportMqtt_Unregister,                 /*pfIotHubTransport_Unregister IoTHubTransport_Unegister;*/
    IoTHubTransportMqtt_Subscribe,                  /*pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;*/
    IoTHubTransportMqtt_Unsubscribe,                /*pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;*/
    IoTHubTransportMqtt_DoWork,                     /*pfIoTHubTransport_DoWork IoTHubTransport_DoWork;*/
    IoTHubTransportMqtt_SetRetryPolicy,             /*pfIoTHubTransport_DoWork IoTHubTransport_SetRetryPolicy;*/
    IoTHubTransportMqtt_GetSendStatus,              /*pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus;*/
    IotHubTransportMqtt_Subscribe_InputQueue,       /*pfIoTHubTransport_Subscribe_InputQueue IoTHubTransport_Subscribe_InputQueue; */
    IotHubTransportMqtt_Unsubscribe_InputQueue,     /*pfIoTHubTransport_Unsubscribe_InputQueue IoTHubTransport_Unsubscribe_InputQueue; */
    IotHubTransportMqtt_SetCallbackContext,         /*pfIoTHubTransport_SetCallbackContext IoTHubTransport_SetCallbackContext; */
    IoTHubTransportMqtt_GetTwinAsync,               /*pfIoTHubTransport_GetTwinAsync IoTHubTransport_GetTwinAsync;*/
    IotHubTransportMqtt_GetSupportedPlatformInfo      /*pfIoTHubTransport_GetSupportedPlatformInfo IoTHubTransport_GetSupportedPlatformInfo;*/
};

/* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_022: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER */
extern const TRANSPORT_PROVIDER* MQTT_Protocol(void)
{
    return &myfunc;
}
