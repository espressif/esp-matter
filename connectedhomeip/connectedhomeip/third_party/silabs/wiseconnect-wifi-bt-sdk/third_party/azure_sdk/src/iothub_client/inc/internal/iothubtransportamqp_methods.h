// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBTRANSPORTAMQP_METHODS_H
#define IOTHUBTRANSPORTAMQP_METHODS_H

#include "azure_uamqp_c/session.h"
#include "azure_c_shared_utility/buffer_.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
#include <cstddef>

extern "C"
{
#else
#include <stddef.h>
#endif

    typedef struct IOTHUBTRANSPORT_AMQP_METHOD_TAG* IOTHUBTRANSPORT_AMQP_METHOD_HANDLE;
    typedef struct IOTHUBTRANSPORT_AMQP_METHODS_TAG* IOTHUBTRANSPORT_AMQP_METHODS_HANDLE;
    typedef void(*ON_METHODS_ERROR)(void* context);
    typedef int(*ON_METHOD_REQUEST_RECEIVED)(void* context, const char* method_name, const unsigned char* request, size_t request_size, IOTHUBTRANSPORT_AMQP_METHOD_HANDLE response);
    typedef void(*ON_METHODS_UNSUBSCRIBED)(void* context);

    MOCKABLE_FUNCTION(, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransportamqp_methods_create, const char*, hostname, const char*, device_id, const char*, module_id);
    MOCKABLE_FUNCTION(, void, iothubtransportamqp_methods_destroy, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransport_amqp_methods_handle);
    MOCKABLE_FUNCTION(, int, iothubtransportamqp_methods_subscribe, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransport_amqp_methods_handle,
        SESSION_HANDLE, session_handle, ON_METHODS_ERROR, on_methods_error, void*, on_methods_error_context,
        ON_METHOD_REQUEST_RECEIVED, on_method_request_received, void*, on_method_request_received_context,
        ON_METHODS_UNSUBSCRIBED, on_methods_unsubscribed, void*, on_methods_unsubscribed_context);
    MOCKABLE_FUNCTION(, int, iothubtransportamqp_methods_respond, IOTHUBTRANSPORT_AMQP_METHOD_HANDLE, method_handle,
        const unsigned char*, response, size_t, response_size, int, status_code);
    MOCKABLE_FUNCTION(, void, iothubtransportamqp_methods_unsubscribe, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransport_amqp_methods_handle);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUBTRANSPORTAMQP_METHODS_H */
