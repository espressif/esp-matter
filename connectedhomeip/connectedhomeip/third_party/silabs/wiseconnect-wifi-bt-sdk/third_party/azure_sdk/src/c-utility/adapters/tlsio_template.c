// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"

#define TLSIO_STATE_VALUES                        \
    TLSIO_STATE_NOT_OPEN,                         \
    TLSIO_STATE_OPENING_UNDERLYING_IO,            \
    TLSIO_STATE_HANDSHAKE_CLIENT_HELLO_SENT,      \
    TLSIO_STATE_HANDSHAKE_SERVER_HELLO_RECEIVED,  \
    TLSIO_STATE_RENEGOTIATE,                      \
    TLSIO_STATE_OPEN,                             \
    TLSIO_STATE_CLOSING,                          \
    TLSIO_STATE_ERROR

MU_DEFINE_ENUM(TLSIO_STATE, TLSIO_STATE_VALUES);
MU_DEFINE_ENUM_STRINGS(TLSIO_STATE, TLSIO_STATE_VALUES);

typedef struct TLS_IO_INSTANCE_TAG
{
    XIO_HANDLE socket_io;
    ON_IO_OPEN_COMPLETE on_io_open_complete;
    ON_IO_CLOSE_COMPLETE on_io_close_complete;
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_ERROR on_io_error;

    void* on_io_open_complete_context;
    void* on_io_close_complete_context;
    void* on_bytes_received_context;
    void* on_io_error_context;
} TLS_IO_INSTANCE;

/*this function will clone an option given by name and value*/
static void* tlsio_template_CloneOption(const char* name, const void* value)
{
    void* result;
    if (name == NULL || value == NULL)
    {
        LogError("invalid parameter detected: const char* name = %p, const void* value = %p", name, value);
        result = NULL;
    }
    else
    {
        result = NULL;
    }
    return result;
}

/*this function destroys an option previously created*/
static void tlsio_template_DestroyOption(const char* name, const void* value)
{
    /*since all options for this layer are actually string copies., disposing of one is just calling free*/
    if (name == NULL || value == NULL)
    {
        LogError("invalid parameter detected: const char* name = %p, const void* value = %p", name, value);
    }
}

static OPTIONHANDLER_HANDLE tlsio_template_retrieveoptions(CONCRETE_IO_HANDLE handle)
{
    OPTIONHANDLER_HANDLE result;
    if (handle == NULL)
    {
        LogError("invalid parameter detected: CONCRETE_IO_HANDLE handle = %p", handle);
        result = NULL;
    }
    else
    {
        result = OptionHandler_Create(tlsio_template_CloneOption, tlsio_template_DestroyOption, tlsio_template_setoption);
        if (result == NULL)
        {
            LogError("unable to OptionHandler_Create");
            /*return as is*/
        }
    }
    return result;
}

static const IO_INTERFACE_DESCRIPTION tlsio_template_interface_description =
{
    tlsio_template_retrieveoptions,
    tlsio_template_create,
    tlsio_template_destroy,
    tlsio_template_open,
    tlsio_template_close,
    tlsio_template_send,
    tlsio_template_dowork,
    tlsio_template_setoption
};

static void indicate_error(TLS_IO_INSTANCE* tls_io_instance)
{
    if (tls_io_instance->on_io_error != NULL)
    {
        tls_io_instance->on_io_error(tls_io_instance->on_io_error_context);
    }
}

CONCRETE_IO_HANDLE tlsio_template_create(void* io_create_parameters)
{
    TLSIO_CONFIG* tls_io_config = (TLSIO_CONFIG *)io_create_parameters;
    TLS_IO_INSTANCE* result;

    if (tls_io_config == NULL)
    {
        LogError("invalid argument detected: void* io_create_parameters = %p", tls_io_config);
        result = NULL;
    }
    else
    {
        result = (TLS_IO_INSTANCE*)malloc(sizeof(TLS_IO_INSTANCE));
        if (result == NULL)
        {
            LogError("malloc failed");
        }
        else
        {
            (void)memset(result, 0, sizeof(TLS_IO_INSTANCE));
        }
    }
    return result;
}

void tlsio_template_destroy(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io != NULL)
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
        free(tls_io_instance);
    }
}

int tlsio_template_open(CONCRETE_IO_HANDLE tls_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;
    (void)on_io_open_complete;
    (void)on_io_open_complete_context;
    (void)on_bytes_received;
    (void)on_bytes_received_context;
    (void)on_io_error;
    (void)on_io_error_context;
    if (tls_io == NULL)
    {
        LogError("invalid argument detected: CONCRETE_IO_HANDLE tls_io = %p", tls_io);
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
        (void)tls_io_instance;
        result = 0;
    }
    return result;
}

int tlsio_template_close(CONCRETE_IO_HANDLE tls_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result = 0;
    (void)callback_context;
    (void)on_io_close_complete;

    if (tls_io == NULL)
    {
        LogError("invalid argument detected: tls_io = %p", tls_io);
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
        (void)tls_io_instance;
        result = 0;
    }
    return result;
}

int tlsio_template_send(CONCRETE_IO_HANDLE tls_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;
    (void)buffer;
    (void)size;
    (void)on_send_complete;
    (void)callback_context;

    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
    (void)tls_io_instance;
    result = 0;
    return result;
}

void tlsio_template_dowork(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io != NULL)
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
        (void)tls_io_instance;
    }
}

int tlsio_template_setoption(CONCRETE_IO_HANDLE tls_io, const char* optionName, const void* value)
{
    int result;
    (void)value;

    if (tls_io == NULL || optionName == NULL)
    {
        LogError("invalid argument detected: CONCRETE_IO_HANDLE tls_io = %p, const char* optionName = %p", tls_io, optionName);
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }
    return result;
}

const IO_INTERFACE_DESCRIPTION* tlsio_template_get_interface_description(void)
{
    return &tlsio_template_interface_description;
}
