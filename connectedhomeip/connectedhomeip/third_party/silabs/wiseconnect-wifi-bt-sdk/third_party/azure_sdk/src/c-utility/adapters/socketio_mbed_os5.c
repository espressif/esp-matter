// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/tcpsocketconnection_c.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"
#include "netsocket/nsapi_types.h"

#define MBED_XIO_RECEIVE_BUFFER_SIZE    128

typedef enum IO_STATE_TAG
{
    IO_STATE_CLOSED,
    IO_STATE_OPENING,
    IO_STATE_OPEN,
    IO_STATE_CLOSING,
    IO_STATE_ERROR
} IO_STATE;

typedef struct PENDING_SOCKET_IO_TAG
{
    unsigned char* bytes;
    size_t size;
    ON_SEND_COMPLETE on_send_complete;
    void* callback_context;
    SINGLYLINKEDLIST_HANDLE pending_io_list;
} PENDING_SOCKET_IO;

typedef struct SOCKET_IO_INSTANCE_TAG
{
    TCPSOCKETCONNECTION_HANDLE tcp_socket_connection;
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_ERROR on_io_error;
    void* on_bytes_received_context;
    void* on_io_error_context;
    char* hostname;
    int port;
    IO_STATE io_state;
    SINGLYLINKEDLIST_HANDLE pending_io_list;
} SOCKET_IO_INSTANCE;

/*this function will clone an option given by name and value*/
static void* socketio_CloneOption(const char* name, const void* value)
{
    (void)name;
    (void)value;
    return NULL;
}

/*this function destroys an option previously created*/
static void socketio_DestroyOption(const char* name, const void* value)
{
    (void)name;
    (void)value;
}

static OPTIONHANDLER_HANDLE socketio_retrieveoptions(CONCRETE_IO_HANDLE socket_io)
{
    OPTIONHANDLER_HANDLE result;
    (void)socket_io;
    result = OptionHandler_Create(socketio_CloneOption, socketio_DestroyOption, socketio_setoption);
    if (result == NULL)
    {
        /*return as is*/
    }
    else
    {
        /*insert here work to add the options to "result" handle*/
    }
    return result;
}

static const IO_INTERFACE_DESCRIPTION socket_io_interface_description =
{
    socketio_retrieveoptions,
    socketio_create,
    socketio_destroy,
    socketio_open,
    socketio_close,
    socketio_send,
    socketio_dowork,
    socketio_setoption
};

static void indicate_error(SOCKET_IO_INSTANCE* socket_io_instance)
{
    if ((socket_io_instance->io_state == IO_STATE_CLOSED)
        || (socket_io_instance->io_state == IO_STATE_ERROR))
    {
        return;
    }
    socket_io_instance->io_state = IO_STATE_ERROR;
    if (socket_io_instance->on_io_error != NULL)
    {
        socket_io_instance->on_io_error(socket_io_instance->on_io_error_context);
    }
}

static int add_pending_io(SOCKET_IO_INSTANCE* socket_io_instance, const unsigned char* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;
    PENDING_SOCKET_IO* pending_socket_io = (PENDING_SOCKET_IO*)malloc(sizeof(PENDING_SOCKET_IO));
    if (pending_socket_io == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        pending_socket_io->bytes = (unsigned char*)malloc(size);
        if (pending_socket_io->bytes == NULL)
        {
            free(pending_socket_io);
            result = MU_FAILURE;
        }
        else
        {
            pending_socket_io->size = size;
            pending_socket_io->on_send_complete = on_send_complete;
            pending_socket_io->callback_context = callback_context;
            pending_socket_io->pending_io_list = socket_io_instance->pending_io_list;
            (void)memcpy(pending_socket_io->bytes, buffer, size);
            if (singlylinkedlist_add(socket_io_instance->pending_io_list, pending_socket_io) == NULL)
            {
                free(pending_socket_io->bytes);
                free(pending_socket_io);
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
    }

    return result;
}

static int retrieve_data(SOCKET_IO_INSTANCE* socket_io_instance)
{
    int received = 1;
    int total_received = 0;

    unsigned char* recv_bytes = malloc(MBED_XIO_RECEIVE_BUFFER_SIZE);
    if (recv_bytes == NULL)
    {
        LogError("Socketio_Failure: NULL allocating input buffer.");
        indicate_error(socket_io_instance);
        return -1;
    }
    
    while (received > 0)
    {
        /* Codes_SRS_SOCKETIO_MBED_OS5_99_005: [ retrieve_data shall succeed if tcp receive bytes succeed ]*/
        received = tcpsocketconnection_receive(socket_io_instance->tcp_socket_connection, (char*)recv_bytes, MBED_XIO_RECEIVE_BUFFER_SIZE);
        if (received > 0)
        {
            total_received += received;
            if (socket_io_instance->on_bytes_received != NULL)
            {
                /* explictly ignoring here the result of the callback */
                socket_io_instance->on_bytes_received(socket_io_instance->on_bytes_received_context, recv_bytes, received);
            }
        }
        else if (received < 0)
        {
            if(received != NSAPI_ERROR_WOULD_BLOCK)     // NSAPI_ERROR_WOULD_BLOCK is not a real error but pending.
            {
                indicate_error(socket_io_instance);
                LogError("Socketio_Failure: underlying IO error %d.", received);
                free(recv_bytes);
                return -1;
            }
        }
    }
    free(recv_bytes);
    
    return total_received;
}

static int send_queued_data(SOCKET_IO_INSTANCE* socket_io_instance)
{
    int errors = 0;
    int sent = 0;
    
    LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
    while (first_pending_io != NULL)
    {
        PENDING_SOCKET_IO* pending_socket_io = (PENDING_SOCKET_IO*)singlylinkedlist_item_get_value(first_pending_io);
        if (pending_socket_io == NULL)
        {
            indicate_error(socket_io_instance);
            return -1;
        }

        int send_result = tcpsocketconnection_send(socket_io_instance->tcp_socket_connection, (const char*)pending_socket_io->bytes, pending_socket_io->size);
        if (send_result != (int)pending_socket_io->size)
        {
            if (send_result == 0)
            {
                // The underlying network layer may encounter hardware / environment issues, 
                // but the driver doesn't handle it properly. So here the send API always return 0, 
                // this causes the program running into dead loop if not check it here.
                if (errors++ >= 10)
                {
                    // Treat it as a network error after try 10 times.
                    LogError("Socketio_Failure: encountered unknow connection issue, the connection will be restarted.");
                    indicate_error(socket_io_instance);
                    return -1;
                }
                wait_ms(10);
            }
            else if (send_result < 0)
            {
                indicate_error(socket_io_instance);
                return -1;
            }
            else
            {
                /* send something, wait for the rest */
                memmove(pending_socket_io->bytes, pending_socket_io->bytes + send_result, pending_socket_io->size - send_result);
                sent += send_result;
            }
        }
        else
        {
            sent += send_result;
            if (pending_socket_io->on_send_complete != NULL)
            {
                pending_socket_io->on_send_complete(pending_socket_io->callback_context, IO_SEND_OK);
            }

            free(pending_socket_io->bytes);
            free(pending_socket_io);
            if (singlylinkedlist_remove(socket_io_instance->pending_io_list, first_pending_io) != 0)
            {
                indicate_error(socket_io_instance);
                return -1;
            }
            errors = 0;
        }

        first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
    }

    return sent;
}

static void close_tcp_connection(SOCKET_IO_INSTANCE* socket_io_instance)
{
    if (socket_io_instance->io_state != IO_STATE_CLOSED)
    {
        if (socket_io_instance->tcp_socket_connection != NULL)
        {
            tcpsocketconnection_close(socket_io_instance->tcp_socket_connection);
            tcpsocketconnection_destroy(socket_io_instance->tcp_socket_connection);
            socket_io_instance->tcp_socket_connection = NULL;
        }
        socket_io_instance->io_state = IO_STATE_CLOSED;
    }
}

CONCRETE_IO_HANDLE socketio_create(void* io_create_parameters)
{
    SOCKETIO_CONFIG* socket_io_config = io_create_parameters;
    SOCKET_IO_INSTANCE* result;

    if (socket_io_config == NULL)
    {
        result = NULL;
    }
    else
    {    
        result = (SOCKET_IO_INSTANCE*)malloc(sizeof(SOCKET_IO_INSTANCE));
        if (result != NULL)
        {
            result->pending_io_list = singlylinkedlist_create();
            if (result->pending_io_list == NULL)
            {
                free(result);
                result = NULL;
            }
            else
            {
                result->hostname = strdup(socket_io_config->hostname);
                if (result->hostname == NULL)
                {
                    singlylinkedlist_destroy(result->pending_io_list);
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->port = socket_io_config->port;
                    result->on_bytes_received = NULL;
                    result->on_io_error = NULL;
                    result->on_bytes_received_context = NULL;
                    result->on_io_error_context = NULL;
                    result->io_state = IO_STATE_CLOSED;
                    result->tcp_socket_connection = NULL;
                }
            }
        }
    }

    return result;
}

void socketio_destroy(CONCRETE_IO_HANDLE socket_io)
{
    if (socket_io != NULL)
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;

        // Close the tcp connection
        close_tcp_connection(socket_io_instance);
    
        // Clear all pending IOs
        LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
        while (first_pending_io != NULL)
        {
            PENDING_SOCKET_IO* pending_socket_io = (PENDING_SOCKET_IO*)singlylinkedlist_item_get_value(first_pending_io);
            if (pending_socket_io != NULL)
            {
                free(pending_socket_io->bytes);
                free(pending_socket_io);
            }

            (void)singlylinkedlist_remove(socket_io_instance->pending_io_list, first_pending_io);
            first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
        }
        singlylinkedlist_destroy(socket_io_instance->pending_io_list);
    
        if(socket_io_instance->hostname != NULL)
        {
            free(socket_io_instance->hostname);
            socket_io_instance->hostname = NULL;
        }
        free(socket_io);
    }
}

int socketio_open(CONCRETE_IO_HANDLE socket_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;
    SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
    

    if (socket_io_instance == NULL ||
        socket_io_instance->io_state != IO_STATE_CLOSED)
    {
        result = MU_FAILURE;
    }
    else
    {
        socket_io_instance->tcp_socket_connection = tcpsocketconnection_create();
        if (socket_io_instance->tcp_socket_connection == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (tcpsocketconnection_connect(socket_io_instance->tcp_socket_connection, socket_io_instance->hostname, socket_io_instance->port) != 0)
            {
                tcpsocketconnection_destroy(socket_io_instance->tcp_socket_connection);
                socket_io_instance->tcp_socket_connection = NULL;
                result = MU_FAILURE;
            }
            else
            {
                tcpsocketconnection_set_blocking(socket_io_instance->tcp_socket_connection, false, 0);

                socket_io_instance->on_bytes_received = on_bytes_received;
                socket_io_instance->on_bytes_received_context = on_bytes_received_context;

                socket_io_instance->on_io_error = on_io_error;
                socket_io_instance->on_io_error_context = on_io_error_context;

                socket_io_instance->io_state = IO_STATE_OPEN;

                result = 0;
            }
        }
    }
    
    if (on_io_open_complete != NULL)
    {
        on_io_open_complete(on_io_open_complete_context, result == 0 ? IO_OPEN_OK : IO_OPEN_ERROR);
    }

    return result;
}

int socketio_close(CONCRETE_IO_HANDLE socket_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result = 0;

    if (socket_io == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
        if (socket_io_instance->io_state == IO_STATE_CLOSED || 
            socket_io_instance->io_state == IO_STATE_ERROR)
        {
            result = MU_FAILURE;
        }
        else
        {
            close_tcp_connection(socket_io_instance);
            if (on_io_close_complete != NULL)
            {
                on_io_close_complete(callback_context);
            }
            result = 0;
        }
    }
    
    return result;
}

int socketio_send(CONCRETE_IO_HANDLE socket_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    if ((socket_io == NULL) ||
        (buffer == NULL) ||
        (size == 0))
    {
        /* Invalid arguments */
        result = MU_FAILURE;
    }
    else
    {
        result = 0;

        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
        if (socket_io_instance->io_state != IO_STATE_OPEN)
        {
            result = MU_FAILURE;
        }
        else
        {
            // Queue the data, and the socketio_dowork sends the package later
            if (add_pending_io(socket_io_instance, buffer, size, on_send_complete, callback_context) != 0)
            {
                result = MU_FAILURE;
            }
        }
    }

    return result;
}

void socketio_dowork(CONCRETE_IO_HANDLE socket_io)
{
    if (socket_io != NULL)
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
        if (socket_io_instance->io_state == IO_STATE_OPEN)
        {
            // Retrieve all data from IoT Hub
            if (retrieve_data(socket_io_instance) < 0)
            {
                return;
            }
        
            // Send all packages in the queue
            send_queued_data(socket_io_instance);
        }
    }
}

int socketio_setoption(CONCRETE_IO_HANDLE socket_io, const char* optionName, const void* value)
{
    /* Not implementing any options, do nothing */
    return OPTIONHANDLER_OK;
}

const IO_INTERFACE_DESCRIPTION* socketio_get_interface_description(void)
{
    return &socket_io_interface_description;
}
