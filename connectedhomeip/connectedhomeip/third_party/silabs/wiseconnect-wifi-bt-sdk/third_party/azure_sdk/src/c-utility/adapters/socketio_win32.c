// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <mstcpip.h>
#ifdef AF_UNIX_ON_WINDOWS
#include <afunix.h>
#endif
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/gbnetwork.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/dns_resolver.h"

typedef enum IO_STATE_TAG
{
    IO_STATE_CLOSED,
    IO_STATE_OPENING,
    IO_STATE_OPEN,
    IO_STATE_CLOSING
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
    SOCKET socket;
    SOCKETIO_ADDRESS_TYPE address_type;
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_ERROR on_io_error;
    ON_IO_OPEN_COMPLETE on_io_open_complete;
    void* on_bytes_received_context;
    void* on_io_error_context;
    void* on_io_open_complete_context;
    char* hostname;
    int port;
    IO_STATE io_state;
    SINGLYLINKEDLIST_HANDLE pending_io_list;
    struct tcp_keepalive keep_alive;
    unsigned char recv_bytes[XIO_RECEIVE_BUFFER_SIZE];
    DNSRESOLVER_HANDLE dns_resolver;
    struct addrinfo* addrInfo;
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

static OPTIONHANDLER_HANDLE socketio_retrieveoptions(CONCRETE_IO_HANDLE handle)
{
    OPTIONHANDLER_HANDLE result;
    (void)handle;
    result = OptionHandler_Create(socketio_CloneOption, socketio_DestroyOption, socketio_setoption);
    if (result == NULL)
    {
        LogError("unable to OptionHandler_Create");
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
    if (socket_io_instance->on_io_error != NULL)
    {
        socket_io_instance->on_io_error(socket_io_instance->on_io_error_context);
    }
}

static int add_pending_io(SOCKET_IO_INSTANCE* socket_io_instance, const unsigned char* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;
    PENDING_SOCKET_IO* pending_socket_io = (PENDING_SOCKET_IO*)calloc(1, sizeof(PENDING_SOCKET_IO));
    if (pending_socket_io == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        pending_socket_io->bytes = (unsigned char*)malloc(size);
        if (pending_socket_io->bytes == NULL)
        {
            LogError("Allocation Failure: Unable to allocate pending list.");
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
                LogError("Failure: Unable to add socket to pending list.");
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


static int lookup_address(SOCKET_IO_INSTANCE* socket_io_instance)
{
    int result = 0;
    
    if (socket_io_instance->address_type == ADDRESS_TYPE_IP)
    {
        if (!dns_resolver_is_lookup_complete(socket_io_instance->dns_resolver))
        {
            socket_io_instance->io_state = IO_STATE_OPENING;
        }
        else if (dns_resolver_get_ipv4(socket_io_instance->dns_resolver) == 0)
        {
            LogError("DNS resolution failure %d.", WSAGetLastError());
            result = MU_FAILURE;
        }
        else
        {
            //The hostname IP has been returned. 
            //So, the socket is ready to open because currently socket_io_instance->socket is INVALID_SOCKET.
            socket_io_instance->io_state = IO_STATE_OPEN;
        }
    }
    else //ADDRESS_TYPE_DOMAIN_SOCKET
    {
        socket_io_instance->io_state = IO_STATE_OPEN;
    }

    return result;
}

static int connect_socket(SOCKET socket, struct sockaddr* addr, size_t len)
{
    int result;
    u_long iMode = 1;

    if (connect(socket, addr, (int)len) != 0)
    {
        LogError("Failure: connect failure %d.", WSAGetLastError());
        result = MU_FAILURE;
    }
    else if (ioctlsocket(socket, FIONBIO, &iMode) != 0)
    {
        LogError("Failure: ioctlsocket failure %d.", WSAGetLastError());
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }

    return result;
}

static int initiate_socket_connection(SOCKET_IO_INSTANCE* socket_io_instance)
{
    int result;

#ifdef AF_UNIX_ON_WINDOWS
    if (socket_io_instance->address_type == ADDRESS_TYPE_IP)
    {
#endif
        struct addrinfo* addr = dns_resolver_get_addrInfo(socket_io_instance->dns_resolver);

        if (addr == NULL)
        {
            LogError("DNS resolution failed");
            result = MU_FAILURE;
        }
        else
        {
            (void)memcpy((socket_io_instance->addrInfo), addr, sizeof(*(socket_io_instance->addrInfo)));

            result = connect_socket(socket_io_instance->socket, (socket_io_instance->addrInfo)->ai_addr, sizeof(*((socket_io_instance->addrInfo)->ai_addr)));
            if (result != 0)
            {
                LogError("connect_socket failed");
            }
            else
            {
                if (socket_io_instance->on_io_open_complete != NULL)
                {
                    socket_io_instance->on_io_open_complete(socket_io_instance->on_io_open_complete_context, IO_OPEN_OK /*: IO_OPEN_ERROR*/);
                }
            }
        }
#ifdef AF_UNIX_ON_WINDOWS
    }
    else // ADDRESS_TYPE_DOMAIN_SOCKET
    {
        SOCKADDR_UN addr_un;
        const char* path = socket_io_instance->hostname;
        size_t path_len = strlen(path);

        // If the value of hostname was parsed out of a 'unix://' URL, it might have a
        // leading forward slash ('/'). That's because the domain socket path is found
        // in the path portion of the URL, not the hostname portion. The hostname is
        // empty and the forward slash delimits the start of the path. For Unix domain
        // socket paths on Windows, discard the leading forward slash.
        if (path[0] == '/')
        {
            ++path;
            --path_len;
        }

        if (path_len + 1 > sizeof(addr_un.sun_path))
        {
            LogError("Path '%s' is too long for a unix socket (max len = %lu)", path, (unsigned long)sizeof(addr_un.sun_path));
            result = MU_FAILURE;
        }
        else
        {
            (void)memset(&addr_un, 0, sizeof(addr_un));
            addr_un.sun_family = AF_UNIX;
            // No need to add NULL terminator due to the above memset
            (void)memcpy(addr_un.sun_path, path, path_len);

            result = connect_socket(socket_io_instance->socket, (struct sockaddr*)&addr_un, sizeof(addr_un));
            if (result != 0)
            {
                LogError("connect_socket failed");
                result = MU_FAILURE;
            }
            else
            {
                socket_io_instance->io_state = IO_STATE_OPEN;
                if (socket_io_instance->on_io_open_complete != NULL)
                {
                    socket_io_instance->on_io_open_complete(socket_io_instance->on_io_open_complete_context, IO_OPEN_OK);
                }
            }
        }
    }
#endif

    return result;
}

static void destroy_socket_io_instance(SOCKET_IO_INSTANCE* instance)
{
    if (instance->dns_resolver != NULL)
    {
        dns_resolver_destroy(instance->dns_resolver);
    }

    free(instance->hostname);
    free(instance->addrInfo);

    if (instance->pending_io_list != NULL)
    {
        singlylinkedlist_destroy(instance->pending_io_list);
    }

    free(instance);
}

CONCRETE_IO_HANDLE socketio_create(void* io_create_parameters)
{
    SOCKETIO_CONFIG* socket_io_config = (SOCKETIO_CONFIG*)io_create_parameters;
    SOCKET_IO_INSTANCE* result;
    struct tcp_keepalive tcp_keepalive = { 0, 0, 0 };

    if (socket_io_config == NULL)
    {
        LogError("Invalid argument: socket_io_config is NULL");
        result = NULL;
    }
    else
    {
        result = (SOCKET_IO_INSTANCE*)malloc(sizeof(SOCKET_IO_INSTANCE));

        if (result != NULL)
        {
            (void)memset(result, 0, sizeof(SOCKET_IO_INSTANCE));

            result->address_type = ADDRESS_TYPE_IP;
            result->pending_io_list = singlylinkedlist_create();
            if (result->pending_io_list == NULL)
            {
                LogError("Failure: singlylinkedlist_create unable to create pending list.");
                destroy_socket_io_instance(result);
                result = NULL;
            }
            else
            {
                if (socket_io_config->hostname != NULL)
                {
                    result->hostname = (char*)malloc(strlen(socket_io_config->hostname) + 1);
                    if (result->hostname != NULL)
                    {
                        (void)strcpy(result->hostname, socket_io_config->hostname);
                    }

                    result->socket = INVALID_SOCKET;
                }
                else
                {
                    result->hostname = NULL;
                    result->socket = *((SOCKET*)socket_io_config->accepted_socket);
                }

                if ((result->hostname == NULL) && (result->socket == INVALID_SOCKET))
                {
                    LogError("Failure: hostname == NULL and socket is invalid.");
                    destroy_socket_io_instance(result);
                    result = NULL;
                }
                else if ((result->addrInfo = calloc(1, sizeof(struct addrinfo))) == NULL)
                {
                    LogError("Failure: addrInfo == NULL.");
                    destroy_socket_io_instance(result);
                    result = NULL;
                }
                else if ((result->addrInfo->ai_addr = calloc(1, sizeof(struct sockaddr_in))) == NULL)
                {
                    LogError("Failure allocating ai_addr");
                    destroy_socket_io_instance(result);
                    result = NULL;
                }
                else
                {
                    result->port = socket_io_config->port;
                    result->on_io_open_complete = NULL;
                    result->dns_resolver = dns_resolver_create(result->hostname, socket_io_config->port, NULL);
                    result->on_bytes_received = NULL;
                    result->on_io_error = NULL;
                    result->on_bytes_received_context = NULL;
                    result->on_io_error_context = NULL;
                    result->io_state = IO_STATE_CLOSED;
                    result->keep_alive = tcp_keepalive;
                }
            }
        }
        else
        {
            LogError("Allocation Failure: SOCKET_IO_INSTANCE");
        }
    }

    return (XIO_HANDLE)result;
}

void socketio_destroy(CONCRETE_IO_HANDLE socket_io)
{
    LIST_ITEM_HANDLE first_pending_io;

    if (socket_io != NULL)
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
        /* we cannot do much if the close fails, so just ignore the result */
        (void)closesocket(socket_io_instance->socket);

        /* clear allpending IOs */

        while ((first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list)) != NULL)
        {
            PENDING_SOCKET_IO* pending_socket_io = (PENDING_SOCKET_IO*)singlylinkedlist_item_get_value(first_pending_io);
            if (pending_socket_io != NULL)
            {
                free(pending_socket_io->bytes);
                free(pending_socket_io);
            }

            singlylinkedlist_remove(socket_io_instance->pending_io_list, first_pending_io);
        }

        destroy_socket_io_instance(socket_io);
    }
}

int socketio_open(CONCRETE_IO_HANDLE socket_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;

    SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
    if (socket_io_instance == NULL)
    {
        LogError("Invalid argument: SOCKET_IO_INSTANCE is NULL");
        result = MU_FAILURE;
    }
    else
    {
        if (socket_io_instance->io_state != IO_STATE_CLOSED)
        {
            LogError("Failure: socket state is not closed.");
            result = MU_FAILURE;
        }
        else if (socket_io_instance->socket != INVALID_SOCKET)
        {
            // Opening an accepted socket
            socket_io_instance->on_bytes_received_context = on_bytes_received_context;
            socket_io_instance->on_bytes_received = on_bytes_received;
            socket_io_instance->on_io_error = on_io_error;
            socket_io_instance->on_io_error_context = on_io_error_context;
            socket_io_instance->on_io_open_complete = on_io_open_complete;
            socket_io_instance->on_io_open_complete_context = on_io_open_complete_context;

            socket_io_instance->io_state = IO_STATE_OPEN;

            result = 0;
        }
        else
        {
#ifdef AF_UNIX_ON_WINDOWS
            int addr_family = socket_io_instance->address_type == ADDRESS_TYPE_IP ? AF_INET : AF_UNIX;
#else
            int addr_family = AF_INET;
#endif
            socket_io_instance->socket = socket(addr_family, SOCK_STREAM, 0);
            if (socket_io_instance->socket == INVALID_SOCKET)
            {
                LogError("Failure: socket create failure %d.", WSAGetLastError());
                result = MU_FAILURE;
            }
            else if (lookup_address(socket_io_instance) != 0)
            {
                LogError("lookup_address failed");
                (void)closesocket(socket_io_instance->socket);
                socket_io_instance->socket = INVALID_SOCKET;
                result = MU_FAILURE;
            }
            else if (socket_io_instance->io_state == IO_STATE_OPEN && initiate_socket_connection(socket_io_instance) != 0)
            {
                LogError("initiate_socket_connection failed");
                (void)closesocket(socket_io_instance->socket);
                socket_io_instance->socket = INVALID_SOCKET;
                result = MU_FAILURE;
            }
            else
            {
                socket_io_instance->on_bytes_received_context = on_bytes_received_context;
                socket_io_instance->on_bytes_received = on_bytes_received;
                socket_io_instance->on_io_error = on_io_error;
                socket_io_instance->on_io_error_context = on_io_error_context;
                socket_io_instance->on_io_open_complete = on_io_open_complete;
                socket_io_instance->on_io_open_complete_context = on_io_open_complete_context;
                
                result = 0;
            }
        }
    }

    if (socket_io_instance != NULL && socket_io_instance->io_state != IO_STATE_OPENING)
    {
        if (on_io_open_complete != NULL)
        {
            on_io_open_complete(on_io_open_complete_context, result == 0 ? IO_OPEN_OK : IO_OPEN_ERROR);
        }
    }

    return result;
}

int socketio_close(CONCRETE_IO_HANDLE socket_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result;

    if (socket_io == NULL)
    {
        LogError("Invalid argument: socket_io is NULL");
        result = MU_FAILURE;
    }
    else
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;

        if ((socket_io_instance->io_state != IO_STATE_CLOSING) &&
            (socket_io_instance->io_state != IO_STATE_CLOSED))
        {
            (void)closesocket(socket_io_instance->socket);
            socket_io_instance->socket = INVALID_SOCKET;
            socket_io_instance->io_state = IO_STATE_CLOSED;
        }

        if (on_io_close_complete != NULL)
        {
            on_io_close_complete(callback_context);
        }
        result = 0;
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
        LogError("Invalid argument: send given invalid parameter");
        result = MU_FAILURE;
    }
    else
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
        if (socket_io_instance->io_state != IO_STATE_OPEN)
        {
            LogError("Failure: socket state is not opened.");
            result = MU_FAILURE;
        }
        else
        {
            LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
            if (first_pending_io != NULL)
            {
                if (add_pending_io(socket_io_instance, (const unsigned char*)buffer, size, on_send_complete, callback_context) != 0)
                {
                    LogError("Failure: add_pending_io failed.");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
            else
            {
                int send_result = send(socket_io_instance->socket, (const char*)buffer, (int)size, 0);
                if (send_result != (int)size)
                {
                    int last_error = WSAGetLastError();

                    if (send_result == SOCKET_ERROR && last_error != WSAEWOULDBLOCK)
                    {
                        LogError("Failure: sending socket failed %d.", last_error);
                        result = MU_FAILURE;
                    }
                    else
                    {
                        size_t bytes_sent = (send_result == SOCKET_ERROR ? 0 : send_result);

                        /* queue data */
                        if (add_pending_io(socket_io_instance, ((const unsigned char*)buffer) + bytes_sent, size - bytes_sent, on_send_complete, callback_context) != 0)
                        {
                            LogError("Failure: add_pending_io failed.");
                            result = MU_FAILURE;
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
                else
                {
                    if (on_send_complete != NULL)
                    {
                        on_send_complete(callback_context, IO_SEND_OK);
                    }

                    result = 0;
                }
            }
        }
    }

    return result;
}

void socketio_dowork(CONCRETE_IO_HANDLE socket_io)
{
    int send_result;
    if (socket_io != NULL)
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
        if (socket_io_instance->io_state == IO_STATE_OPEN)
        {
            LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
            while (first_pending_io != NULL)
            {
                PENDING_SOCKET_IO* pending_socket_io = (PENDING_SOCKET_IO*)singlylinkedlist_item_get_value(first_pending_io);
                if (pending_socket_io == NULL)
                {
                    LogError("Failure: retrieving socket from list");
                    indicate_error(socket_io_instance);
                    break;
                }

                /* TODO: we need to do more than a cast here to be 100% clean
                The following bug was filed: [WarnL4] socketio_win32 does not account for already sent bytes and there is a truncation of size from size_t to int */
                send_result = send(socket_io_instance->socket, (const char*)pending_socket_io->bytes, (int)pending_socket_io->size, 0);
                if (send_result != (int)pending_socket_io->size)
                {
                    int last_error = WSAGetLastError();
                    if (last_error != WSAEWOULDBLOCK)
                    {
                        free(pending_socket_io->bytes);
                        free(pending_socket_io);
                        (void)singlylinkedlist_remove(socket_io_instance->pending_io_list, first_pending_io);
                    }
                    else
                    {
                        /* try again */
                    }
                }
                else
                {
                    if (pending_socket_io->on_send_complete != NULL)
                    {
                        pending_socket_io->on_send_complete(pending_socket_io->callback_context, IO_SEND_OK);
                    }

                    free(pending_socket_io->bytes);
                    free(pending_socket_io);
                    if (singlylinkedlist_remove(socket_io_instance->pending_io_list, first_pending_io) != 0)
                    {
                        LogError("Failure: removing socket from list");
                        indicate_error(socket_io_instance);
                    }
                }

                first_pending_io = singlylinkedlist_get_head_item(socket_io_instance->pending_io_list);
            }

            if (socket_io_instance->io_state == IO_STATE_OPEN)
            {
                int received = 0;
                do
                {
                    received = recv(socket_io_instance->socket, (char*)socket_io_instance->recv_bytes, XIO_RECEIVE_BUFFER_SIZE, 0);
                    if ((received > 0))
                    {
                        if (socket_io_instance->on_bytes_received != NULL)
                        {
                            /* Explicitly ignoring here the result of the callback */
                            (void)socket_io_instance->on_bytes_received(socket_io_instance->on_bytes_received_context, socket_io_instance->recv_bytes, received);
                        }
                    }
                    else if (received == 0)
                    {
                        indicate_error(socket_io_instance);
                    }
                    else
                    {
                        int last_error = WSAGetLastError();
                        if (last_error != WSAEWOULDBLOCK && last_error != ERROR_SUCCESS)
                        {
                            LogError("Socketio_Failure: Receiving data from endpoint: %d.", last_error);
                            indicate_error(socket_io_instance);
                        }
                    }
                } while (received > 0 && socket_io_instance->io_state == IO_STATE_OPEN);
            }
        }
        else
        {
            //Handle async socket_open operation within socket_dowork
            if (socket_io_instance->io_state == IO_STATE_OPENING)
            {
                if (lookup_address(socket_io_instance) != 0)
                {
                    LogError("lookup_address failed");
                    (void)closesocket(socket_io_instance->socket);
                    socket_io_instance->socket = INVALID_SOCKET;
                    socket_io_instance->io_state = IO_STATE_CLOSED;
                }
                else if (socket_io_instance->io_state == IO_STATE_OPEN && initiate_socket_connection(socket_io_instance) != 0)
                {
                    LogError("initialize_socket_connection failed");
                    (void)closesocket(socket_io_instance->socket);
                    socket_io_instance->socket = INVALID_SOCKET;
                    socket_io_instance->io_state = IO_STATE_CLOSED;
                }
            }
        }
    }
}

static int set_keepalive(SOCKET_IO_INSTANCE* socket_io, struct tcp_keepalive* keepAlive)
{
    int result;
    DWORD bytesReturned;

    int err = WSAIoctl(socket_io->socket, SIO_KEEPALIVE_VALS, keepAlive,
        sizeof(struct tcp_keepalive), NULL, 0, &bytesReturned, NULL, NULL);
    if (err != 0)
    {
        LogError("Failure: setting keep-alive on the socket: %d.", err == SOCKET_ERROR ? WSAGetLastError() : err);
        result = MU_FAILURE;
    }
    else
    {
        socket_io->keep_alive = *keepAlive;
        result = 0;
    }

    return result;
}

static int socketio_setaddresstype_option(SOCKET_IO_INSTANCE* socket_io_instance, const char* addressType)
{
    int result;

    if (socket_io_instance->io_state != IO_STATE_CLOSED)
    {
        LogError("Socket's type can only be changed when in state 'IO_STATE_CLOSED'. Current state=%d", socket_io_instance->io_state);
        result = MU_FAILURE;
    }
#ifdef AF_UNIX_ON_WINDOWS
    else if (strcmp(addressType, OPTION_ADDRESS_TYPE_DOMAIN_SOCKET) == 0)
    {
        socket_io_instance->address_type = ADDRESS_TYPE_DOMAIN_SOCKET;
        result = 0;
    }
#endif
    else if (strcmp(addressType, OPTION_ADDRESS_TYPE_IP_SOCKET) == 0)
    {
        socket_io_instance->address_type = ADDRESS_TYPE_IP;
        result = 0;
    }
    else
    {
        LogError("Address type %s is not supported", addressType);
        result = MU_FAILURE;
    }

    return result;
}

int socketio_setoption(CONCRETE_IO_HANDLE socket_io, const char* optionName, const void* value)
{
    int result;

    if (socket_io == NULL ||
        optionName == NULL ||
        value == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;

        if (strcmp(optionName, "tcp_keepalive") == 0)
        {
            struct tcp_keepalive keepAlive = socket_io_instance->keep_alive;
            keepAlive.onoff = *(int *)value;

            result = set_keepalive(socket_io_instance, &keepAlive);
        }
        else if (strcmp(optionName, "tcp_keepalive_time") == 0)
        {
            unsigned long kaTime = *(int *)value * 1000; // convert to ms
            struct tcp_keepalive keepAlive = socket_io_instance->keep_alive;
            keepAlive.keepalivetime = kaTime;

            result = set_keepalive(socket_io_instance, &keepAlive);
        }
        else if (strcmp(optionName, "tcp_keepalive_interval") == 0)
        {
            unsigned long kaInterval = *(int *)value * 1000; // convert to ms
            struct tcp_keepalive keepAlive = socket_io_instance->keep_alive;
            keepAlive.keepaliveinterval = kaInterval;

            result = set_keepalive(socket_io_instance, &keepAlive);
        }
        else if (strcmp(optionName, OPTION_ADDRESS_TYPE) == 0)
        {
            result = socketio_setaddresstype_option(socket_io_instance, (const char*)value);
        }
        else
        {
            result = MU_FAILURE;
        }
    }

    return result;
}

const IO_INTERFACE_DESCRIPTION* socketio_get_interface_description(void)
{
    return &socket_io_interface_description;
}
