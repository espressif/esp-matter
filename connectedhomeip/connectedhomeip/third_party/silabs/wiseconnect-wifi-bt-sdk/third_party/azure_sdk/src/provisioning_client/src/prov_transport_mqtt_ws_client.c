// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/wsio.h"
#include "azure_prov_client/prov_transport_mqtt_client.h"
#include "azure_prov_client/internal/prov_transport_mqtt_common.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/shared_util_options.h"

#define MQTT_WS_PORT_NUM               443
#define MQTT_WS_PROTOCOL_NAME "MQTT"
#define MQTT_WS_RELATIVE_PATH "/$iothub/websocket"

static XIO_HANDLE mqtt_transport_ws_io(const char* fqdn, const HTTP_PROXY_OPTIONS* proxy_info)
{
    XIO_HANDLE result;
    HTTP_PROXY_IO_CONFIG proxy_config;
    TLSIO_CONFIG tls_io_config;
    WSIO_CONFIG ws_io_config;
    const IO_INTERFACE_DESCRIPTION* ws_io_interface;
    const IO_INTERFACE_DESCRIPTION* tlsio_interface;

    if ((ws_io_interface = wsio_get_interface_description()) == NULL)
    {
        /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_013: [ If any failure is encountered amqp_transport_ws_io shall return NULL ] */
        LogError("wsio_get_interface_description return NULL IO Interface");
        result = NULL;
    }
    else if ((tlsio_interface = platform_get_default_tlsio()) == NULL)
    {
        /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_013: [ If any failure is encountered amqp_transport_ws_io shall return NULL ] */
        LogError("platform_get_default_tlsio return NULL IO Interface");
        result = NULL;
    }
    else
    {
        memset(&tls_io_config, 0, sizeof(TLSIO_CONFIG));

        ws_io_config.hostname = fqdn;
        ws_io_config.port = MQTT_WS_PORT_NUM;
        ws_io_config.protocol = MQTT_WS_PROTOCOL_NAME;
        ws_io_config.resource_name = MQTT_WS_RELATIVE_PATH;
        ws_io_config.underlying_io_interface = tlsio_interface;
        ws_io_config.underlying_io_parameters = &tls_io_config;

        tls_io_config.hostname = fqdn;
        tls_io_config.port = MQTT_WS_PORT_NUM;
        if (proxy_info != NULL)
        {
            /* Codes_PROV_TRANSPORT_MQTT_CLIENT_07_012: [ If proxy_info is not NULL, amqp_transport_io shall construct a HTTP_PROXY_IO_CONFIG object and assign it to TLSIO_CONFIG underlying_io_parameters ] */
            proxy_config.hostname = tls_io_config.hostname;
            proxy_config.port = MQTT_WS_PORT_NUM;
            proxy_config.proxy_hostname = proxy_info->host_address;
            proxy_config.proxy_port = proxy_info->port;
            proxy_config.username = proxy_info->username;
            proxy_config.password = proxy_info->password;

            tls_io_config.underlying_io_interface = http_proxy_io_get_interface_description();
            tls_io_config.underlying_io_parameters = &proxy_config;
        }

        result = xio_create(ws_io_interface, &ws_io_config);
        if (result == NULL)
        {
            LogError("failed calling xio_create on underlying io");
        }
    }
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_014: [ On success mqtt_transport_ws_io shall return an allocated XIO_HANDLE. ] */
    return result;
}

PROV_DEVICE_TRANSPORT_HANDLE prov_transport_mqtt_ws_create(const char* uri, TRANSPORT_HSM_TYPE type, const char* scope_id, const char* api_version, PROV_TRANSPORT_ERROR_CALLBACK error_cb, void* error_ctx)
{
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_001: [ prov_transport_mqtt_ws_create shall call the prov_trans_common_mqtt_create function with mqtt_transport_ws_io transport IO estabishment. ] */
    return prov_transport_common_mqtt_create(uri, type, scope_id, api_version, mqtt_transport_ws_io, error_cb, error_ctx);
}

void prov_transport_mqtt_ws_destroy(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_002: [ prov_transport_mqtt_ws_destroy shall invoke the prov_transport_common_mqtt_destroy method ] */
    prov_transport_common_mqtt_destroy(handle);
}

int prov_transport_mqtt_ws_open(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* registration_id, BUFFER_HANDLE ek, BUFFER_HANDLE srk, PROV_DEVICE_TRANSPORT_REGISTER_CALLBACK data_callback, void* user_ctx, PROV_DEVICE_TRANSPORT_STATUS_CALLBACK status_cb, void* status_ctx, PROV_TRANSPORT_CHALLENGE_CALLBACK reg_challenge_cb, void* challenge_ctx)
{
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_003: [ prov_transport_mqtt_ws_open shall invoke the prov_transport_common_mqtt_open method ] */
    return prov_transport_common_mqtt_open(handle, registration_id, ek, srk, data_callback, user_ctx, status_cb, status_ctx, reg_challenge_cb, challenge_ctx);
}

int prov_transport_mqtt_ws_close(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_004: [ prov_transport_mqtt_ws_close shall invoke the prov_transport_common_mqtt_close method ] */
    return prov_transport_common_mqtt_close(handle);
}

int prov_transport_mqtt_ws_register_device(PROV_DEVICE_TRANSPORT_HANDLE handle, PROV_TRANSPORT_JSON_PARSE json_parse_cb, PROV_TRANSPORT_CREATE_JSON_PAYLOAD json_create_cb, void* json_ctx)
{
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_005: [ prov_transport_mqtt_ws_register_device shall invoke the prov_transport_common_mqtt_register_device method ] */
    return prov_transport_common_mqtt_register_device(handle, json_parse_cb, json_create_cb, json_ctx);
}

int prov_transport_mqtt_ws_get_operation_status(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_006: [ prov_transport_mqtt_ws_get_operation_status shall invoke the prov_transport_common_mqtt_get_operation_status method ] */
    return prov_transport_common_mqtt_get_operation_status(handle);
}

void prov_transport_mqtt_ws_dowork(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_007: [ prov_transport_mqtt_ws_dowork shall invoke the prov_transport_common_mqtt_dowork method ] */
    prov_transport_common_mqtt_dowork(handle);
}

int prov_transport_mqtt_ws_set_trace(PROV_DEVICE_TRANSPORT_HANDLE handle, bool trace_on)
{
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_008: [ prov_transport_mqtt_ws_set_trace shall invoke the prov_transport_common_mqtt_set_trace method ] */
    return prov_transport_common_mqtt_set_trace(handle, trace_on);
}

static int prov_transport_mqtt_ws_x509_cert(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* certificate, const char* private_key)
{
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_009: [ prov_transport_mqtt_ws_x509_cert shall invoke the prov_trans_common_mqtt_x509_cert method ] */
    return prov_transport_common_mqtt_x509_cert(handle, certificate, private_key);
}

static int prov_transport_mqtt_ws_set_trusted_cert(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* certificate)
{
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_010: [ prov_transport_mqtt_ws_set_trusted_cert shall invoke the prov_transport_common_mqtt_set_trusted_cert method ] */
    return prov_transport_common_mqtt_set_trusted_cert(handle, certificate);
}

static int prov_transport_mqtt_ws_set_proxy(PROV_DEVICE_TRANSPORT_HANDLE handle, const HTTP_PROXY_OPTIONS* proxy_options)
{
    /* Codes_PROV_TRANSPORT_MQTT_WS_CLIENT_07_011: [ prov_transport_mqtt_ws_set_proxy shall invoke the prov_transport_common_mqtt_set_proxy method ] */
    return prov_transport_common_mqtt_set_proxy(handle, proxy_options);
}

static int prov_transport_mqtt_ws_set_option(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* option, const void* value)
{
    return prov_transport_common_mqtt_set_option(handle, option, value);
}

static PROV_DEVICE_TRANSPORT_PROVIDER prov_mqtt_func =
{
    prov_transport_mqtt_ws_create,
    prov_transport_mqtt_ws_destroy,
    prov_transport_mqtt_ws_open,
    prov_transport_mqtt_ws_close,
    prov_transport_mqtt_ws_register_device,
    prov_transport_mqtt_ws_get_operation_status,
    prov_transport_mqtt_ws_dowork,
    prov_transport_mqtt_ws_set_trace,
    prov_transport_mqtt_ws_x509_cert,
    prov_transport_mqtt_ws_set_trusted_cert,
    prov_transport_mqtt_ws_set_proxy,
    prov_transport_mqtt_ws_set_option
};

const PROV_DEVICE_TRANSPORT_PROVIDER* Prov_Device_MQTT_WS_Protocol(void)
{
    return &prov_mqtt_func;
}
