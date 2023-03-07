// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)~(size_t)0)
#endif

typedef enum GBNETWORK_STATE_TAG
{
    GBNETWORK_STATE_INIT,
    GBNETWORK_STATE_NOT_INIT
} GBNETWORK_STATE;

static GBNETWORK_STATE gbnetworkState = GBNETWORK_STATE_NOT_INIT;

static uint64_t g_send_bytes = 0;
static uint64_t g_send_number = 0;
static uint64_t g_recv_bytes = 0;
static uint64_t g_recv_number = 0;

static LOCK_HANDLE gbnetworkThreadSafeLock = NULL;

int gbnetwork_init(void)
{
    int result;

    if (gbnetworkState != GBNETWORK_STATE_NOT_INIT)
    {
        result = MU_FAILURE;
    }
    else if ((gbnetworkThreadSafeLock = Lock_Init()) == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        gbnetworkState = GBNETWORK_STATE_INIT;

        g_send_bytes = 0;
        g_send_number = 0;
        g_recv_bytes = 0;
        g_recv_number = 0;

        result = 0;
    }

    return result;
}

void gbnetwork_deinit(void)
{
    if (gbnetworkState == GBNETWORK_STATE_INIT)
    {
        (void)Lock_Deinit(gbnetworkThreadSafeLock);
    }
    gbnetworkState = GBNETWORK_STATE_NOT_INIT;
}

void gbnetwork_resetMetrics()
{
    if (gbnetworkState != GBNETWORK_STATE_INIT)
    {
        LogError("gbnetwork is not initialized.");
    }
    else if (LOCK_OK != Lock(gbnetworkThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
    }
    else
    {
        g_send_bytes = 0;
        g_send_number = 0;
        g_recv_bytes = 0;
        g_recv_number = 0;
        (void)Unlock(gbnetworkThreadSafeLock);
    }
}

#ifdef WIN32
int gbnetwork_send(SOCKET sock, const char* buf, int len, int flags)
#else
ssize_t gbnetwork_send(int sock, const void* buf, size_t len, int flags)
#endif
{
    if (gbnetworkState != GBNETWORK_STATE_INIT)
    {
        // Don't log here by design
    }
    else if (LOCK_OK != Lock(gbnetworkThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
    }
    else
    {
        g_send_number++;
        g_send_bytes += len;
        (void)Unlock(gbnetworkThreadSafeLock);
    }
    return send(sock, buf, len, flags);
}

#ifdef WIN32
int gbnetwork_recv(SOCKET sock, char* buf, int len, int flags)
#else
ssize_t gbnetwork_recv(int sock, void* buf, size_t len, int flags)
#endif
{
#ifdef WIN32
    int result;
#else
    ssize_t result;
#endif

    result = recv(sock, buf, len, flags);

    if (gbnetworkState != GBNETWORK_STATE_INIT)
    {
        // Don't log here by design
    }
    else if (LOCK_OK != Lock(gbnetworkThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
    }
    else
    {
        if (result > 0)
        {
            g_recv_bytes += result;
            g_recv_number++;
        }
        (void)Unlock(gbnetworkThreadSafeLock);
    }
    return result;
}

uint64_t gbnetwork_getBytesSent(void)
{
    uint64_t result;

    if (gbnetworkState != GBNETWORK_STATE_INIT)
    {
        LogError("gbnetwork is not initialized.");
        result = 0;
    }
    else if (LOCK_OK != Lock(gbnetworkThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
        result = 0;
    }
    else
    {
        result = g_send_bytes;
        (void)Unlock(gbnetworkThreadSafeLock);
    }
    return result;
}

uint64_t gbnetwork_getNumSends(void)
{
    uint64_t result;

    if (gbnetworkState != GBNETWORK_STATE_INIT)
    {
        LogError("gbnetwork is not initialized.");
        result = 0;
    }
    else if (LOCK_OK != Lock(gbnetworkThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
        result = 0;
    }
    else
    {
        result = g_send_number;
        (void)Unlock(gbnetworkThreadSafeLock);
    }
    return result;
}

uint64_t gbnetwork_getBytesRecv()
{
    uint64_t result;

    if (gbnetworkState != GBNETWORK_STATE_INIT)
    {
        LogError("gbnetwork is not initialized.");
        result = 0;
    }
    else if (LOCK_OK != Lock(gbnetworkThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
        result = 0;
    }
    else
    {
        result = g_recv_bytes;
        (void)Unlock(gbnetworkThreadSafeLock);
    }
    return result;
}

uint64_t gbnetwork_getNumRecv()
{
    uint64_t result;

    if (gbnetworkState != GBNETWORK_STATE_INIT)
    {
        LogError("gbnetwork is not initialized.");
        result = 0;
    }
    else if (LOCK_OK != Lock(gbnetworkThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
        result = 0;
    }
    else
    {
        result = g_recv_number;
        (void)Unlock(gbnetworkThreadSafeLock);
    }
    return result;
}
