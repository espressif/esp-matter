// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "winsock2.h"
#include "ws2tcpip.h"
#include "windows.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/socketio.h"
#include "azure_uamqp_c/socket_listener.h"

typedef struct SOCKET_LISTENER_INSTANCE_TAG
{
    int port;
    SOCKET socket;
    ON_SOCKET_ACCEPTED on_socket_accepted;
    void* callback_context;
} SOCKET_LISTENER_INSTANCE;

SOCKET_LISTENER_HANDLE socketlistener_create(int port)
{
    SOCKET_LISTENER_INSTANCE* result = (SOCKET_LISTENER_INSTANCE*)malloc(sizeof(SOCKET_LISTENER_INSTANCE));
    if (result == NULL)
    {
        LogError("Cannot allocate memory for socket listener");
    }
    else
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

    if ((socket_listener == NULL) ||
        (on_socket_accepted == NULL))
    {
        LogError("Bad arguments: socket_listener = %p, on_socket_accepted = %p",
            socket_listener, on_socket_accepted);
        result = MU_FAILURE;
    }
    else
    {
        socket_listener->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_listener->socket == INVALID_SOCKET)
        {
            LogError("Could not create socket");
            result = MU_FAILURE;
        }
        else
        {
            u_long iMode = 1;
            struct sockaddr_in service;

            socket_listener->on_socket_accepted = on_socket_accepted;
            socket_listener->callback_context = callback_context;

            // The sockaddr_in structure specifies the address family,
            // IP address, and port for the socket that is being bound.
            service.sin_family = AF_INET;
            service.sin_addr.s_addr = INADDR_ANY;
            service.sin_port = htons((u_short)socket_listener->port);

            if (bind(socket_listener->socket, (SOCKADDR *)&service, sizeof(service)) == SOCKET_ERROR)
            {
                LogError("Could not bind socket");
                (void)closesocket(socket_listener->socket);
                socket_listener->socket = INVALID_SOCKET;
                result = MU_FAILURE;
            }
            else if (ioctlsocket(socket_listener->socket, FIONBIO, &iMode) != 0)
            {
                LogError("Could not set listening socket in non-blocking mode");
                (void)closesocket(socket_listener->socket);
                socket_listener->socket = INVALID_SOCKET;
                result = MU_FAILURE;
            }
            else
            {
                if (listen(socket_listener->socket, SOMAXCONN) == SOCKET_ERROR)
                {
                    LogError("Could not start listening for connections");
                    (void)closesocket(socket_listener->socket);
                    socket_listener->socket = INVALID_SOCKET;
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
        socket_listener->on_socket_accepted = NULL;
        socket_listener->callback_context = NULL;

        (void)closesocket(socket_listener->socket);
        socket_listener->socket = INVALID_SOCKET;

        result = 0;
    }

    return result;
}

void socketlistener_dowork(SOCKET_LISTENER_HANDLE socket_listener)
{
    if (socket_listener == NULL)
    {
        LogError("NULL socket_listener");
    }
    else
    {
        SOCKET accepted_socket = accept(socket_listener->socket, NULL, NULL);
        if (accepted_socket != INVALID_SOCKET)
        {
            SOCKETIO_CONFIG socketio_config;
            socketio_config.hostname = NULL;
            socketio_config.port = socket_listener->port;
            socketio_config.accepted_socket = &accepted_socket;
            socket_listener->on_socket_accepted(socket_listener->callback_context, socketio_get_interface_description(), &socketio_config);
        }
    }
}
