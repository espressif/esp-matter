// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROV_TRANSPORT_AMQP_COMMON_H
#define PROV_TRANSPORT_AMQP_COMMON_H

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#include <cstddef>
#else
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_prov_client/prov_transport.h"
#include "azure_prov_client/internal/prov_transport_private.h"
#include "azure_uamqp_c/saslclientio.h"
#include "azure_c_shared_utility/http_proxy_io.h"

typedef struct PROV_TRANSPORT_IO_INFO_TAG
{
    XIO_HANDLE transport_handle;
    XIO_HANDLE sasl_handle;
} PROV_TRANSPORT_IO_INFO;

typedef PROV_TRANSPORT_IO_INFO*(*PROV_AMQP_TRANSPORT_IO)(const char* fully_qualified_name, SASL_MECHANISM_HANDLE* sasl_mechanism, const HTTP_PROXY_OPTIONS* proxy_info);

MOCKABLE_FUNCTION(, PROV_DEVICE_TRANSPORT_HANDLE, prov_transport_common_amqp_create, const char*, uri, TRANSPORT_HSM_TYPE, type, const char*, scope_id, const char*, api_version, PROV_AMQP_TRANSPORT_IO, transport_io, PROV_TRANSPORT_ERROR_CALLBACK, error_cb, void*, error_ctx);
MOCKABLE_FUNCTION(, void, prov_transport_common_amqp_destroy, PROV_DEVICE_TRANSPORT_HANDLE, handle);
MOCKABLE_FUNCTION(, int, prov_transport_common_amqp_open, PROV_DEVICE_TRANSPORT_HANDLE, handle, const char*, registration_id, BUFFER_HANDLE, ek, BUFFER_HANDLE, srk, PROV_DEVICE_TRANSPORT_REGISTER_CALLBACK, data_callback, void*, user_ctx, PROV_DEVICE_TRANSPORT_STATUS_CALLBACK, status_cb, void*, status_ctx, PROV_TRANSPORT_CHALLENGE_CALLBACK, reg_challenge_cb, void*, challenge_ctx);
MOCKABLE_FUNCTION(, int, prov_transport_common_amqp_close, PROV_DEVICE_TRANSPORT_HANDLE, handle);
MOCKABLE_FUNCTION(, int, prov_transport_common_amqp_register_device, PROV_DEVICE_TRANSPORT_HANDLE, handle, PROV_TRANSPORT_JSON_PARSE, json_parse_cb, PROV_TRANSPORT_CREATE_JSON_PAYLOAD, json_create_cb, void*, json_ctx);
MOCKABLE_FUNCTION(, int, prov_transport_common_amqp_get_operation_status, PROV_DEVICE_TRANSPORT_HANDLE, handle);
MOCKABLE_FUNCTION(, void, prov_transport_common_amqp_dowork, PROV_DEVICE_TRANSPORT_HANDLE, handle);
MOCKABLE_FUNCTION(, int, prov_transport_common_amqp_set_trace, PROV_DEVICE_TRANSPORT_HANDLE, handle, bool, trace_on);
MOCKABLE_FUNCTION(, int, prov_transport_common_amqp_set_proxy, PROV_DEVICE_TRANSPORT_HANDLE, handle, const HTTP_PROXY_OPTIONS*, proxy_options);
MOCKABLE_FUNCTION(, int, prov_transport_common_amqp_set_trusted_cert, PROV_DEVICE_TRANSPORT_HANDLE, handle, const char*, certificate);
MOCKABLE_FUNCTION(, int, prov_transport_common_amqp_x509_cert, PROV_DEVICE_TRANSPORT_HANDLE, handle, const char*, certificate, const char*, private_key);
MOCKABLE_FUNCTION(, int, prov_transport_common_amqp_set_option, PROV_DEVICE_TRANSPORT_HANDLE, handle, const char*, option, const void*, value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // PROV_TRANSPORT_AMQP_COMMON_H
