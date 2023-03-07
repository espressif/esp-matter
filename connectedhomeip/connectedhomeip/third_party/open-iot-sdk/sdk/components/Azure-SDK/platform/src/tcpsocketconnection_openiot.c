// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// clang-format off
// external dep tcpsocketconnection_c.h is missing the stbool header
#include <stdbool.h>
// clang-format on

#include "azure_c_shared_utility/tcpsocketconnection_c.h"
#include "azure_c_shared_utility/xlogging.h"
#include "iot_socket.h"

#include <stddef.h>

typedef struct {
    int32_t id;
    bool isConnected;
} openiot_tcp_socket_t;

static openiot_tcp_socket_t openiot_tcp_socket = {.id = IOT_SOCKET_ESOCK, .isConnected = false};

TCPSOCKETCONNECTION_HANDLE tcpsocketconnection_create(void)
{
    int32_t socket_id = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_STREAM, IOT_SOCKET_IPPROTO_TCP);
    if (socket_id < 0) {
        LogError("Create TCP socket failed %d", socket_id);
        return NULL;
    }

    openiot_tcp_socket.id = socket_id;
    return &openiot_tcp_socket;
}

void tcpsocketconnection_set_blocking(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle,
                                      bool blocking,
                                      unsigned int timeout)
{
    if (tcpSocketConnectionHandle == NULL) {
        return;
    }

    int32_t res;
    openiot_tcp_socket_t *tsc = (openiot_tcp_socket_t *)tcpSocketConnectionHandle;
    uint32_t nonblocking = blocking ? 0U : 1U;

    res = iotSocketSetOpt(tsc->id, IOT_SOCKET_IO_FIONBIO, &nonblocking, sizeof(nonblocking));
    if (res) {
        LogError("Set non-blocking option failed %d", res);
    }

    if (timeout) {
        res = iotSocketSetOpt(tsc->id, IOT_SOCKET_SO_RCVTIMEO, &timeout, sizeof(timeout));
        if (res) {
            LogError("Set recv timeout failed %d", res);
        }
        res = iotSocketSetOpt(tsc->id, IOT_SOCKET_SO_SNDTIMEO, &timeout, sizeof(timeout));
        if (res) {
            LogError("Set send timeout failed %d", res);
        }
    }
}

void tcpsocketconnection_destroy(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle)
{
    if (tcpSocketConnectionHandle == NULL) {
        return;
    }

    openiot_tcp_socket_t *tsc = (openiot_tcp_socket_t *)tcpSocketConnectionHandle;
    if (tsc->id != IOT_SOCKET_ESOCK) {
        int32_t res = iotSocketClose(tsc->id);
        if (res) {
            LogError("Close socket failed %d", res);
            return;
        }

        tsc->id = IOT_SOCKET_ESOCK;
    }
}

int tcpsocketconnection_connect(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, const char *host, const int port)
{
    if (tcpSocketConnectionHandle == NULL) {
        return MU_FAILURE;
    }

    openiot_tcp_socket_t *tsc = (openiot_tcp_socket_t *)tcpSocketConnectionHandle;
    if (tsc->isConnected) {
        LogInfo("TCP socket already connected");
        return 0;
    }

    uint8_t ip[4];
    uint32_t ip_len = sizeof(ip);
    int res = iotSocketGetHostByName(host, IOT_SOCKET_AF_INET, ip, &ip_len);
    if (res < 0) {
        LogError("iotSocketGetHostByName failed %d", res);
        return MU_FAILURE;
    }

    res = iotSocketConnect(tsc->id, (const uint8_t *)&ip, ip_len, port);
    if (res) {
        LogError("iotSocketConnect failed %d", res);
        return MU_FAILURE;
    }

    tsc->isConnected = true;
    return 0;
}

bool tcpsocketconnection_is_connected(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle)
{
    if (tcpSocketConnectionHandle == NULL) {
        return false;
    }

    openiot_tcp_socket_t *tsc = (openiot_tcp_socket_t *)tcpSocketConnectionHandle;
    return tsc->isConnected;
}

void tcpsocketconnection_close(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle)
{
    if (tcpSocketConnectionHandle == NULL) {
        return;
    }

    openiot_tcp_socket_t *tsc = (openiot_tcp_socket_t *)tcpSocketConnectionHandle;
    int32_t res = iotSocketClose(tsc->id);
    if (res) {
        LogError("Close socket failed %d", res);
        return;
    }

    tsc->isConnected = false;
    tsc->id = IOT_SOCKET_ESOCK;
}

int tcpsocketconnection_send(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, const char *data, int length)
{
    if (tcpSocketConnectionHandle == NULL) {
        return -1;
    }

    openiot_tcp_socket_t *tsc = (openiot_tcp_socket_t *)tcpSocketConnectionHandle;
    if (!tsc->isConnected) {
        LogError("TCP socket not connected");
        return -1;
    }

    return iotSocketSend(tsc->id, data, length);
}

int tcpsocketconnection_send_all(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, const char *data, int length)
{
    if (tcpSocketConnectionHandle == NULL) {
        return -1;
    }

    openiot_tcp_socket_t *tsc = (openiot_tcp_socket_t *)tcpSocketConnectionHandle;
    if (!tsc->isConnected) {
        LogError("TCP socket not connected");
        return -1;
    }

    return iotSocketSend(tsc->id, data, length);
}

int tcpsocketconnection_receive(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, char *data, int length)
{
    if (tcpSocketConnectionHandle == NULL) {
        return -1;
    }

    openiot_tcp_socket_t *tsc = (openiot_tcp_socket_t *)tcpSocketConnectionHandle;
    if (!tsc->isConnected) {
        LogError("TCP socket not connected");
        return -1;
    }

    return iotSocketRecv(tsc->id, data, length);
}

int tcpsocketconnection_receive_all(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, char *data, int length)
{
    if (tcpSocketConnectionHandle == NULL) {
        return -1;
    }

    openiot_tcp_socket_t *tsc = (openiot_tcp_socket_t *)tcpSocketConnectionHandle;
    if (!tsc->isConnected) {
        LogError("TCP socket not connected");
        return -1;
    }

    return iotSocketRecv(tsc->id, data, length);
}
