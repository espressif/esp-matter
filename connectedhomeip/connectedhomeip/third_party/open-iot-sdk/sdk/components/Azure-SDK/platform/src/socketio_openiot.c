/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/tcpsocketconnection_c.h"
#include "azure_c_shared_utility/xlogging.h"
#include "iot_socket.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum IO_STATE_TAG {
    IO_STATE_CLOSED,
    IO_STATE_OPENING,
    IO_STATE_OPEN,
    IO_STATE_CLOSING,
    IO_STATE_ERROR
} IO_STATE;

typedef struct PENDING_SOCKET_IO_TAG {
    unsigned char *bytes;
    size_t size;
    ON_SEND_COMPLETE on_send_complete;
    void *callback_context;
    SINGLYLINKEDLIST_HANDLE pending_io_list;
} PENDING_SOCKET_IO;

typedef struct SOCKET_IO_INSTANCE_TAG {
    TCPSOCKETCONNECTION_HANDLE tcp_socket_connection;
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_ERROR on_io_error;
    void *on_bytes_received_context;
    void *on_io_error_context;
    char *hostname;
    int port;
    IO_STATE io_state;
    SINGLYLINKEDLIST_HANDLE pending_io_list;
    unsigned char recv_bytes[XIO_RECEIVE_BUFFER_SIZE];
} SOCKET_IO_INSTANCE;

/** This function will clone an option given by name and value
 * Empty implementation - no Open IoT Socket options are supported.
 */
static void *socketio_CloneOption(const char *name, const void *value)
{
    (void)name;
    (void)value;
    return NULL;
}

/** This function destroys an option previously created
 * Empty implementation - now no Open IoT Socket options are supported.
 */
static void socketio_DestroyOption(const char *name, const void *value)
{
    (void)name;
    (void)value;
}

static OPTIONHANDLER_HANDLE socketio_retrieveoptions(CONCRETE_IO_HANDLE socket_io)
{
    OPTIONHANDLER_HANDLE result;

    if (socket_io == NULL) {
        LogError("Failed retrieving options (handle is NULL)");
        result = NULL;
    } else {
        result = OptionHandler_Create(socketio_CloneOption, socketio_DestroyOption, socketio_setoption);
        if (result == NULL) {
            LogError("OptionHandler_Create failed");
        } else {
            /* Add the options to "result" handle using OptionHandler_AddOption() here.
               Empty implementation - now no Open IoT Socket options are supported. */
        }
    }

    return result;
}

static const IO_INTERFACE_DESCRIPTION socket_io_interface_description = {socketio_retrieveoptions,
                                                                         socketio_create,
                                                                         socketio_destroy,
                                                                         socketio_open,
                                                                         socketio_close,
                                                                         socketio_send,
                                                                         socketio_dowork,
                                                                         socketio_setoption};

static void indicate_error(SOCKET_IO_INSTANCE *socket_io_instance)
{
    if ((socket_io_instance->io_state == IO_STATE_CLOSED) || (socket_io_instance->io_state == IO_STATE_ERROR)) {
        return;
    }
    socket_io_instance->io_state = IO_STATE_ERROR;
    if (socket_io_instance->on_io_error != NULL) {
        socket_io_instance->on_io_error(socket_io_instance->on_io_error_context);
    }
}

static int add_pending_io(SOCKET_IO_INSTANCE *socket_io_instance,
                          const unsigned char *buffer,
                          size_t size,
                          ON_SEND_COMPLETE on_send_complete,
                          void *callback_context)
{
    int result = 0;
    PENDING_SOCKET_IO *pending_socket_io = (PENDING_SOCKET_IO *)malloc(sizeof(PENDING_SOCKET_IO));
    if (pending_socket_io == NULL) {
        LogError("pending_socket_io malloc failed");
        result = MU_FAILURE;
        goto exit;
    }

    pending_socket_io->bytes = (unsigned char *)malloc(size);
    if (pending_socket_io->bytes == NULL) {
        LogError("pending_socket_io->bytes malloc failed");
        result = MU_FAILURE;
        goto exit;
    }

    pending_socket_io->size = size;
    pending_socket_io->on_send_complete = on_send_complete;
    pending_socket_io->callback_context = callback_context;
    pending_socket_io->pending_io_list = socket_io_instance->pending_io_list;
    (void)memcpy(pending_socket_io->bytes, buffer, size);
    if (singlylinkedlist_add(socket_io_instance->pending_io_list, pending_socket_io) == NULL) {
        LogError("Add pending_socket_io to list failed");
        result = MU_FAILURE;
    }

exit:
    if (result != 0) {
        if (pending_socket_io != NULL) {
            free(pending_socket_io);
        }

        if (pending_socket_io->bytes != NULL) {
            free(pending_socket_io->bytes);
        }
    }

    return result;
}

static void close_tcp_connection(SOCKET_IO_INSTANCE *socket_io_instance)
{
    if (socket_io_instance->io_state != IO_STATE_CLOSED) {
        if (socket_io_instance->tcp_socket_connection != NULL) {
            tcpsocketconnection_close(socket_io_instance->tcp_socket_connection);
            tcpsocketconnection_destroy(socket_io_instance->tcp_socket_connection);
            socket_io_instance->tcp_socket_connection = NULL;
        }
        socket_io_instance->io_state = IO_STATE_CLOSED;
    }
}

static int retrieve_data(SOCKET_IO_INSTANCE *socket_io_instance)
{
    int received = 0;

    do {
        received = tcpsocketconnection_receive(
            socket_io_instance->tcp_socket_connection, (char *)socket_io_instance->recv_bytes, XIO_RECEIVE_BUFFER_SIZE);
        if (received > 0) {
            if (socket_io_instance->on_bytes_received != NULL) {
                socket_io_instance->on_bytes_received(
                    socket_io_instance->on_bytes_received_context, socket_io_instance->recv_bytes, (size_t)received);
            }
        } else if (received == 0) {
            // Do not log error here due to this is probably the socket being closed on the other end
            indicate_error(socket_io_instance);
        } else if (received != IOT_SOCKET_EAGAIN) {
            LogError("Socketio_Failure: receive IO error %d", received);
            indicate_error(socket_io_instance);
            return MU_FAILURE;
        }
    } while (received > 0 && socket_io_instance->io_state == IO_STATE_OPEN);

    return 0;
}

static int send_queued_data(SOCKET_IO_INSTANCE *socket_io_instance)
{
    LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
    while (first_pending_io != NULL) {
        PENDING_SOCKET_IO *pending_socket_io = (PENDING_SOCKET_IO *)singlylinkedlist_item_get_value(first_pending_io);
        if (pending_socket_io == NULL) {
            LogError("Socketio_Failure: pending_socket_io is NULL");
            indicate_error(socket_io_instance);
            return MU_FAILURE;
        }

        int send_result = tcpsocketconnection_send(
            socket_io_instance->tcp_socket_connection, (const char *)pending_socket_io->bytes, pending_socket_io->size);
        if (send_result != (int)pending_socket_io->size) {
            if (send_result < 0) {
                LogError("Socketio_Failure: send IO error %d", send_result);
                indicate_error(socket_io_instance);
                return MU_FAILURE;
            } else {
                /* send something, wait for the rest */
                memmove(pending_socket_io->bytes,
                        pending_socket_io->bytes + send_result,
                        pending_socket_io->size - send_result);
            }
        } else {
            if (pending_socket_io->on_send_complete != NULL) {
                pending_socket_io->on_send_complete(pending_socket_io->callback_context, IO_SEND_OK);
            }

            free(pending_socket_io->bytes);
            free(pending_socket_io);
            if (singlylinkedlist_remove(socket_io_instance->pending_io_list, first_pending_io) != 0) {
                LogError("Socketio_Failure: remove pending for list failed");
                indicate_error(socket_io_instance);
                return MU_FAILURE;
            }
        }

        first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
    }

    return 0;
}

CONCRETE_IO_HANDLE socketio_create(void *io_create_parameters)
{
    SOCKETIO_CONFIG *socket_io_config = io_create_parameters;
    SOCKET_IO_INSTANCE *ret = NULL;
    int result = 0;

    if (socket_io_config == NULL) {
        LogError("Failed socket creating (configuration is NULL)");
        return NULL;
    }

    ret = (SOCKET_IO_INSTANCE *)malloc(sizeof(SOCKET_IO_INSTANCE));
    if (ret == NULL) {
        LogError("SOCKET_IO_INSTANCE malloc failed");
        result = MU_FAILURE;
        goto exit;
    }

    ret->pending_io_list = singlylinkedlist_create();
    if (ret->pending_io_list == NULL) {
        LogError("singlylinkedlist_create failed");
        result = MU_FAILURE;
        goto exit;
    }

    size_t hostname_buf_size = strlen(socket_io_config->hostname) + 1;
    ret->hostname = malloc(hostname_buf_size);
    if (ret->hostname == NULL) {
        LogError("Hostname malloc failed");
        result = MU_FAILURE;
        goto exit;
    }

    snprintf(ret->hostname, hostname_buf_size, "%s", socket_io_config->hostname);
    ret->port = socket_io_config->port;
    ret->on_bytes_received = NULL;
    ret->on_io_error = NULL;
    ret->on_bytes_received_context = NULL;
    ret->on_io_error_context = NULL;
    ret->io_state = IO_STATE_CLOSED;
    ret->tcp_socket_connection = NULL;

exit:
    if (result != 0) {
        if (ret && ret->hostname) {
            free(ret->hostname);
        }

        if (ret && ret->pending_io_list) {
            singlylinkedlist_destroy(ret->pending_io_list);
        }

        if (ret) {
            free(ret);
        }

        ret = NULL;
    }
    return ret;
}

void socketio_destroy(CONCRETE_IO_HANDLE socket_io)
{
    if (socket_io == NULL) {
        LogError("Failed destroy (socket_io is NULL)");
        return;
    }

    SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)socket_io;

    // Close the tcp connection
    close_tcp_connection(socket_io_instance);

    // Clear all pending IOs
    LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
    while (first_pending_io != NULL) {
        PENDING_SOCKET_IO *pending_socket_io = (PENDING_SOCKET_IO *)singlylinkedlist_item_get_value(first_pending_io);
        if (pending_socket_io != NULL) {
            free(pending_socket_io->bytes);
            free(pending_socket_io);
        }

        (void)singlylinkedlist_remove(socket_io_instance->pending_io_list, first_pending_io);
        first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
    }
    singlylinkedlist_destroy(socket_io_instance->pending_io_list);

    if (socket_io_instance->hostname != NULL) {
        free(socket_io_instance->hostname);
        socket_io_instance->hostname = NULL;
    }
    free(socket_io);
}

int socketio_open(CONCRETE_IO_HANDLE socket_io,
                  ON_IO_OPEN_COMPLETE on_io_open_complete,
                  void *on_io_open_complete_context,
                  ON_BYTES_RECEIVED on_bytes_received,
                  void *on_bytes_received_context,
                  ON_IO_ERROR on_io_error,
                  void *on_io_error_context)
{
    int result = 0;

    if (socket_io == NULL) {
        LogError("Failed open (socket_io is NULL)");
        result = MU_FAILURE;
        goto exit;
    }

    SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)socket_io;
    if (socket_io_instance->io_state != IO_STATE_CLOSED) {
        LogError("Failed open (wrong state)");
        result = MU_FAILURE;
        goto exit;
    }

    socket_io_instance->tcp_socket_connection = tcpsocketconnection_create();
    if (socket_io_instance->tcp_socket_connection == NULL) {
        LogError("Failed open (create TCP socket failed)");
        result = MU_FAILURE;
        goto exit;
    }

    if (tcpsocketconnection_connect(
            socket_io_instance->tcp_socket_connection, socket_io_instance->hostname, socket_io_instance->port)
        != 0) {
        LogError("Failed open (TCP socket connection failed)");
        result = MU_FAILURE;
        goto exit;
    }

    tcpsocketconnection_set_blocking(socket_io_instance->tcp_socket_connection, false, 0);

    socket_io_instance->on_bytes_received = on_bytes_received;
    socket_io_instance->on_bytes_received_context = on_bytes_received_context;

    socket_io_instance->on_io_error = on_io_error;
    socket_io_instance->on_io_error_context = on_io_error_context;

    socket_io_instance->io_state = IO_STATE_OPEN;

exit:
    if (result != 0 && socket_io_instance->tcp_socket_connection != NULL) {
        tcpsocketconnection_destroy(socket_io_instance->tcp_socket_connection);
        socket_io_instance->tcp_socket_connection = NULL;
    }

    if (on_io_open_complete != NULL) {
        on_io_open_complete(on_io_open_complete_context, result == 0 ? IO_OPEN_OK : IO_OPEN_ERROR);
    }

    return result;
}

int socketio_close(CONCRETE_IO_HANDLE socket_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void *callback_context)
{
    int result = 0;

    if (socket_io == NULL) {
        LogError("Failed close (socket_io is NULL)");
        result = MU_FAILURE;
        goto exit;
    }

    SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)socket_io;
    if (socket_io_instance->io_state != IO_STATE_CLOSED) {
        close_tcp_connection(socket_io_instance);
    }

exit:
    if (on_io_close_complete != NULL) {
        on_io_close_complete(callback_context);
    }

    return result;
}

int socketio_send(CONCRETE_IO_HANDLE socket_io,
                  const void *buffer,
                  size_t size,
                  ON_SEND_COMPLETE on_send_complete,
                  void *callback_context)
{
    int result = 0;

    if ((socket_io == NULL) || (buffer == NULL) || (size == 0)) {
        LogError("Failed send (invalid arguments)");
        result = MU_FAILURE;
        goto exit;
    }

    SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)socket_io;
    if (socket_io_instance->io_state != IO_STATE_OPEN) {
        LogError("Failed send (wrong state)");
        result = MU_FAILURE;
        goto exit;
    }

    // Queue the data, and the socketio_dowork sends the package later
    if (add_pending_io(socket_io_instance, buffer, size, on_send_complete, callback_context) != 0) {
        LogError("add_pending_io failed");
        result = MU_FAILURE;
    }

exit:
    if (on_send_complete != NULL) {
        on_send_complete(callback_context, result == 0 ? IO_SEND_OK : IO_SEND_ERROR);
    }

    return result;
}

void socketio_dowork(CONCRETE_IO_HANDLE socket_io)
{
    if (socket_io == NULL) {
        LogError("Failed dowork (socket_io is NULL)");
        return;
    }

    SOCKET_IO_INSTANCE *socket_io_instance = (SOCKET_IO_INSTANCE *)socket_io;
    if (socket_io_instance->io_state == IO_STATE_OPEN) {
        // Send all packages in the queue
        send_queued_data(socket_io_instance);

        // Retrieve all data from IoT Hub
        retrieve_data(socket_io_instance);
    }
}

int socketio_setoption(CONCRETE_IO_HANDLE socket_io, const char *optionName, const void *value)
{
    /* Not implementing any options, do nothing */
    return OPTIONHANDLER_OK;
}

const IO_INTERFACE_DESCRIPTION *socketio_get_interface_description(void)
{
    return &socket_io_interface_description;
}
