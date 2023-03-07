// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#error  CycloneSSL is NOT supported for the Azure C shared utilities.
#error  The code here is provided for reference purposes.
#error
#error  A security audit is required if you attempt to bring this code back.

#include <stdlib.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"
#include "tls.h"
#include "azure_c_shared_utility/tlsio_cyclonessl_socket.h"

/* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_01_001: [ tlsio_cyclonessl_socket_create shall create a new socket to be used by CycloneSSL. ]*/
int tlsio_cyclonessl_socket_create(const char* hostname, unsigned int port, TlsSocket* new_socket)
{
    int result;

    /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_01_003: [ If hostname or socket is NULL, then tlsio_cyclonessl_socket_create shall fail and it shall return a non-zero value. ]*/
    if ((hostname == NULL) ||
        (new_socket == NULL))
    {
        LogError("Invalid arguments: hostname = %p, new_socket = %p", hostname, new_socket);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_01_004: [ tlsio_cyclonessl_socket_create shall call socketOpen to create a TCP socket. ]*/
        Socket* socket = socketOpen(SOCKET_TYPE_STREAM, SOCKET_IP_PROTO_TCP);
        if (socket == NULL)
        {
            /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_01_007: [ If any of the socket calls fails, then tlsio_cyclonessl_socket_create shall fail and return a non-zero value. ]*/
            LogError("socketOpen failed, cannot create socket");
            result = MU_FAILURE;
        }
        else
        {
            IpAddr ipAddr;

            /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_01_005: [ tlsio_cyclonessl_socket_create shall call getHostByName to obtain an IpAddr structure filled with the address of the hostname. ]*/
            if (getHostByName(NULL, hostname, &ipAddr, 0) != 0)
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_01_007: [ If any of the socket calls fails, then tlsio_cyclonessl_socket_create shall fail and return a non-zero value. ]*/
                socketClose(socket);
                LogError("Cannot resolve host");
                result = MU_FAILURE;
            }
            /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_01_006: [ tlsio_cyclonessl_socket_create shall call socketConnect and pass the obtained address in order to connect the socket. ]*/
            else if (socketConnect(socket, &ipAddr, port) != 0)
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_01_007: [ If any of the socket calls fails, then tlsio_cyclonessl_socket_create shall fail and return a non-zero value. ]*/
                socketClose(socket);
                LogError("Failed to connect");
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_01_002: [ On success tlsio_cyclonessl_socket_create shall return 0 and fill in the socket handle in the new_socket out argument. ]*/
                *new_socket = (TlsSocket)socket;
                result = 0;
            }
        }
    }

    return result;
}

void tlsio_cyclonessl_socket_destroy(TlsSocket socket)
{
    /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_01_009: [ If socket is NULL, tlsio_cyclonessl_socket_destroy shall do nothing. ]*/
    if (socket == NULL)
    {
        LogError("tlsio_cyclonessl_socket_destroy: NULL socket");
    }
    else
    {
        /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_01_008: [ tlsio_cyclonessl_socket_destroy shall close the socket passed as argument by calling the function socketClose. ]*/
        socketClose(socket);
    }
}
