// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/http_proxy_io.h"

static CONCRETE_IO_HANDLE http_proxy_stub_create(void* io_create_parameters)
{
    CONCRETE_IO_HANDLE result;

    (void)io_create_parameters;
    result = NULL;

    return result;
}

static void http_proxy_stub_destroy(CONCRETE_IO_HANDLE http_proxy_io)
{
    (void)http_proxy_io;

    LogError("Function %s is a stub and should never be called", __FUNCTION__);
}

static int http_proxy_stub_open(CONCRETE_IO_HANDLE http_proxy_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;

    (void)http_proxy_io;
    (void)on_io_open_complete;
    (void)on_io_open_complete_context;
    (void)on_bytes_received;
    (void)on_bytes_received_context;
    (void)on_io_error;
    (void)on_io_error_context;

    LogError("Function %s is a stub and should never be called", __FUNCTION__);
    result = MU_FAILURE;

    return result;
}

static int http_proxy_stub_close(CONCRETE_IO_HANDLE http_proxy_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* on_io_close_complete_context)
{
    int result;

    (void)http_proxy_io;
    (void)on_io_close_complete;
    (void)on_io_close_complete_context;

    LogError("Function %s is a stub and should never be called", __FUNCTION__);
    result = MU_FAILURE;

    return result;
}

static int http_proxy_stub_send(CONCRETE_IO_HANDLE http_proxy_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* on_send_complete_context)
{
    int result;

    (void)http_proxy_io;
    (void)buffer;
    (void)size;
    (void)on_send_complete;
    (void)on_send_complete_context;

    LogError("Function %s is a stub and should never be called", __FUNCTION__);
    result = MU_FAILURE;

    return result;
}

static void http_proxy_stub_dowork(CONCRETE_IO_HANDLE http_proxy_io)
{
    (void)http_proxy_io;

    LogError("Function %s is a stub and should never be called", __FUNCTION__);
}

static int http_proxy_stub_set_option(CONCRETE_IO_HANDLE http_proxy_io, const char* option_name, const void* value)
{
    int result;

    (void)http_proxy_io;
    (void)option_name;
    (void)value;

    LogError("Function %s is a stub and should never be called", __FUNCTION__);
    result = MU_FAILURE;

    return result;
}

static OPTIONHANDLER_HANDLE http_proxy_stub_retrieve_options(CONCRETE_IO_HANDLE http_proxy_io)
{
    OPTIONHANDLER_HANDLE result;

    (void)http_proxy_io;

    LogError("Function %s is a stub and should never be called", __FUNCTION__);
    result = NULL;

    return result;
}

static const IO_INTERFACE_DESCRIPTION http_proxy_stub_interface_description =
{
    http_proxy_stub_retrieve_options,
    http_proxy_stub_create,
    http_proxy_stub_destroy,
    http_proxy_stub_open,
    http_proxy_stub_close,
    http_proxy_stub_send,
    http_proxy_stub_dowork,
    http_proxy_stub_set_option
};

const IO_INTERFACE_DESCRIPTION* http_proxy_io_get_interface_description(void)
{
    /* Codes_SRS_HTTP_PROXY_IO_01_049: [ http_proxy_io_get_interface_description shall return a pointer to an IO_INTERFACE_DESCRIPTION structure that contains pointers to the functions: http_proxy_io_retrieve_options, http_proxy_io_retrieve_create, http_proxy_io_destroy, http_proxy_io_open, http_proxy_io_close, http_proxy_io_send and http_proxy_io_dowork. ]*/
    return &http_proxy_stub_interface_description;
}
