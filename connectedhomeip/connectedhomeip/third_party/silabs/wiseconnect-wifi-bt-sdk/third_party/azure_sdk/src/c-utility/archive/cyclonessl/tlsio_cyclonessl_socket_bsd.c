// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#error  CycloneSSL is NOT supported for the Azure C shared utilities.
#error  The code here is provided for reference purposes.
#error
#error  A security audit is required if you attempt to bring this code back.

#include <stdlib.h>

#if _WIN32
#define _WINERROR_
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <stdio.h>
#include "tls.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/tlsio_cyclonessl_socket.h"

/* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_BSD_01_001: [ tlsio_cyclonessl_socket_create shall create a new socket to be used by CycloneSSL. ]*/
int tlsio_cyclonessl_socket_create(const char* hostname, unsigned int port, TlsSocket* new_socket)
{
    int result;

    /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_BSD_01_002: [ If hostname or new_socket is NULL, then tlsio_cyclonessl_socket_create shall fail and it shall return a non-zero value. ]*/
    if ((hostname == NULL) ||
        (new_socket == NULL))
    {
        LogError("Invalid arguments: hostname = %p, new_socket = %p", hostname, new_socket);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_BSD_01_003: [ tlsio_cyclonessl_socket_create shall call socket to create a TCP socket. ]*/
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == (SOCKET)-1)
        {
            /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_BSD_01_007: [ If any of the socket calls fails, then tlsio_cyclonessl_socket_create shall fail and return a non-zero value. ]*/
            LogError("Error: Cannot create socket (%d)\r\n", WSAGetLastError());
            result = MU_FAILURE;
        }
        else
        {
            char portString[16];
            ADDRINFO addrHint = { 0 };
            ADDRINFO* addrInfo = NULL;

            addrHint.ai_family = AF_INET;
            addrHint.ai_socktype = SOCK_STREAM;
            addrHint.ai_protocol = 0;

            /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_BSD_01_004: [ tlsio_cyclonessl_socket_create shall call getaddrinfo to obtain a hint ADDRINFO. ]*/
            if ((sprintf(portString, "%u", port) < 0) ||
                (getaddrinfo(hostname, portString, &addrHint, &addrInfo) != 0))
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_BSD_01_007: [ If any of the socket calls fails, then tlsio_cyclonessl_socket_create shall fail and return a non-zero value. ]*/
                LogError("Failure: getaddrinfo failure %d.", WSAGetLastError());
                (void)closesocket(sock);
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_BSD_01_006: [ tlsio_cyclonessl_socket_create shall call connect and pass the constructed address in order to connect the socket. ]*/
                if (connect(sock, addrInfo->ai_addr, (int)addrInfo->ai_addrlen) < 0)
                {
                    /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_BSD_01_007: [ If any of the socket calls fails, then tlsio_cyclonessl_socket_create shall fail and return a non-zero value. ]*/
                    LogError("Error: Failed to connect (%d)\r\n", WSAGetLastError());
                    closesocket(sock);
                    result = MU_FAILURE;
                }
                else
                {
                    /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_BSD_01_008: [ On success tlsio_cyclonessl_socket_create shall return 0 and fill in the socket handle in the new_socket out argument. ]*/
                    *new_socket = (TlsSocket)sock;
                    result = 0;
                }
            }
        }
    }

    return result;
}

void tlsio_cyclonessl_socket_destroy(TlsSocket socket)
{
    /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_BSD_01_010: [ If socket is INVALID_SOCKET (-1), tlsio_cyclonessl_socket_destroy shall do nothing. ]*/
    if (socket == (SOCKET)-1)
    {
        LogError("Invalid socket\r\n");
    }
    else
    {
        /* Codes_SRS_TLSIO_CYCLONESSL_SOCKET_BSD_01_009: [ tlsio_cyclonessl_socket_destroy shall close the socket passed as argument by calling the function close. ]*/
        (void)closesocket((SOCKET)socket);
    }
}
