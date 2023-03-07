// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_uamqp_c/socket_listener.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/socketio.h"

typedef struct SOCKET_LISTENER_INSTANCE_TAG
{
    int port;
    int socket;
    ON_SOCKET_ACCEPTED on_socket_accepted;
    void* callback_context;
} SOCKET_LISTENER_INSTANCE;

SOCKET_LISTENER_HANDLE socketlistener_create(int port)
{
    SOCKET_LISTENER_INSTANCE* result = (SOCKET_LISTENER_INSTANCE*)malloc(sizeof(SOCKET_LISTENER_INSTANCE));
    if (result != NULL)
    {
        result->port = port;
        result->on_socket_accepted = NULL;
        result->callback_context = NULL;
    }

    return (SOCKET_LISTENER_HANDLE)result;
}

void socketlistener_destroy(SOCKET_LISTENER_HANDLE socket_listener)
{
    if (socket_listener != NULL)
    {
        socketlistener_stop(socket_listener);
        free(socket_listener);
    }
}

int socketlistener_start(SOCKET_LISTENER_HANDLE socket_listener, ON_SOCKET_ACCEPTED on_socket_accepted, void* callback_context)
{
    int result;

    if (socket_listener == NULL)
    {
        LogError("NULL socket_listener");
        result = MU_FAILURE;
    }
    else
    {
        SOCKET_LISTENER_INSTANCE* socket_listener_instance = (SOCKET_LISTENER_INSTANCE*)socket_listener;

        socket_listener_instance->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_listener_instance->socket == -1)
        {
            LogError("Creating socket failed");
            result = MU_FAILURE;
        }
        else
        {
            struct sockaddr_in sa;

            socket_listener_instance->on_socket_accepted = on_socket_accepted;
            socket_listener_instance->callback_context = callback_context;

            sa.sin_family = AF_INET;
            sa.sin_port = htons(socket_listener_instance->port);
            sa.sin_addr.s_addr = htonl(INADDR_ANY);

            int flags;
            if ((-1 == (flags = fcntl(socket_listener_instance->socket, F_GETFL, 0))) ||
                (fcntl(socket_listener_instance->socket, F_SETFL, flags | O_NONBLOCK) == -1))
            {
                LogError("Failure: fcntl failure.");
                close(socket_listener_instance->socket);
                socket_listener_instance->socket = -1;
                result = MU_FAILURE;
            }
            else if (bind(socket_listener_instance->socket, (const struct sockaddr*)&sa, sizeof(sa)) == -1)
            {
                LogError("bind socket failed");
                (void)close(socket_listener_instance->socket);
                socket_listener_instance->socket = -1;
                result = MU_FAILURE;
            }
            else
            {
                if (listen(socket_listener_instance->socket, SOMAXCONN) == -1)
                {
                    LogError("listen on socket failed");
                    (void)close(socket_listener_instance->socket);
                    socket_listener_instance->socket = -1;
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
        }
    }

    return result;
}

int socketlistener_stop(SOCKET_LISTENER_HANDLE socket_listener)
{
    int result;

    if (socket_listener == NULL)
    {
        LogError("NULL socket_listener");
        result = MU_FAILURE;
    }
    else
    {
        SOCKET_LISTENER_INSTANCE* socket_listener_instance = (SOCKET_LISTENER_INSTANCE*)socket_listener;

        socket_listener_instance->on_socket_accepted = NULL;
        socket_listener_instance->callback_context = NULL;

        (void)close(socket_listener_instance->socket);
        socket_listener_instance->socket = -1;

        result = 0;
    }

    return result;
}

void socketlistener_dowork(SOCKET_LISTENER_HANDLE socket_listener)
{
    if (socket_listener != NULL)
    {
        SOCKET_LISTENER_INSTANCE* socket_listener_instance = (SOCKET_LISTENER_INSTANCE*)socket_listener;
        int accepted_socket = accept(socket_listener_instance->socket, NULL, NULL);
        if (accepted_socket != -1)
        {
            int flags;
            if ((-1 == (flags = fcntl(accepted_socket, F_GETFL, 0))) ||
                (fcntl(accepted_socket, F_SETFL, flags | O_NONBLOCK) == -1))
            {
                LogError("Failure: fcntl failure on accepted socket.");
                close(accepted_socket);
            }
            else if (socket_listener_instance->on_socket_accepted != NULL)
            {
                SOCKETIO_CONFIG socketio_config;
                socketio_config.hostname = NULL;
                socketio_config.port = socket_listener_instance->port;
                socketio_config.accepted_socket = &accepted_socket;
                socket_listener_instance->on_socket_accepted(socket_listener_instance->callback_context, socketio_get_interface_description(), &socketio_config);
            }
            else
            {
                (void)close(accepted_socket);
            }
        }
    }
}
