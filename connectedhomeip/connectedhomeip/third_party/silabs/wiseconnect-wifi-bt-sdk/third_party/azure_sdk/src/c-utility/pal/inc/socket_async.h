// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file socket_async.h
 *    @brief     Abstracts non-blocking sockets.
 */

#ifndef AZURE_SOCKET_ASYNC_H
#define AZURE_SOCKET_ASYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"

// socket_async exposes asynchronous socket operations while hiding OS-specifics. Committing to
// asynchronous operation also simplifies the interface compared to generic sockets.

#define SOCKET_ASYNC_INVALID_SOCKET  -1

typedef struct
{
    // Keepalive is disabled by default at the TCP level. If keepalive is desired, it
    // is strongly recommended to use one of the higher level keepalive (ping) options rather
    // than the TCP level because the higher level options provide server connection status
    // in addition to keeping the connection open.
    int keep_alive;     // < 0 for system defaults, >= 0 to use supplied keep_alive, idle, interval, and count
    int keep_idle;      // seconds before first keepalive packet (ignored if keep_alive <= 0)
    int keep_interval;  // seconds between keepalive packets (ignored if keep_alive <= 0)
    int keep_count;     // number of times to try before declaring failure (ignored if keep_alive <= 0)
    // It is acceptable to extend this struct by adding members for future enhancement,
    // but existing members must not be altered to ensure back-compatibility.
} SOCKET_ASYNC_OPTIONS;
typedef SOCKET_ASYNC_OPTIONS* SOCKET_ASYNC_OPTIONS_HANDLE;

typedef int SOCKET_ASYNC_HANDLE;

/**
* @brief    Create a non-blocking socket that is correctly configured for asynchronous use.
*
* @param   sock    Receives the created SOCKET_ASYNC_HANDLE.
*
* @param   host_ipv4    The IPv4 of the SSL server to be contacted.
*
* @param   port    The port of the SSL server to use.
*
* @param   is_UDP True for UDP, false for TCP.
*
* @param   options A pointer to SOCKET_ASYNC_OPTIONS used during socket_async_create
*          for TCP connections only. May be NULL. Ignored for UDP sockets.
*          Need only exist for the duration of the socket_async_create call.
*
* @return   @c The created and configured SOCKET_ASYNC_HANDLE if the API call is successful
*           or SOCKET_ASYNC_INVALID_SOCKET in case it fails. Error logging is
*           performed by the underlying concrete implementation, so no
*           further error logging is necessary.
*/
MOCKABLE_FUNCTION(, SOCKET_ASYNC_HANDLE, socket_async_create, uint32_t, host_ipv4, uint16_t, port, bool, is_UDP, SOCKET_ASYNC_OPTIONS_HANDLE, options);

/**
* @brief    Check whether a newly-created socket_async has completed its initial connection.
*
* @param   sock    The created SOCKET_ASYNC_HANDLE to check for connection completion.
*
* @param   is_created    Receives the completion state if successful, set to false on failure.
*
* @return   @c 0 if the API call is successful.
*           MU_FAILURE means an unexpected error has occurred and the socket must be destroyed.
*/
MOCKABLE_FUNCTION(, int, socket_async_is_create_complete, SOCKET_ASYNC_HANDLE, sock, bool*, is_complete);

/**
* @brief    Send a message on the specified socket.
*
* @param    sock The socket to be used.
*
* @param    buffer The buffer containing the message to transmit.
*
* @param    size The number of bytes to transmit.
*
* @param    sent_count Receives the number of bytes transmitted. The N == 0
*           case means normal operation but the socket's outgoing buffer was full.
*
* @return   @c 0 if successful.
*           MU_FAILURE means an unexpected error has occurred and the socket must be destroyed.
*/
MOCKABLE_FUNCTION(, int, socket_async_send, SOCKET_ASYNC_HANDLE, sock, const void*, buffer, size_t, size, size_t*, sent_count);

/**
* @brief    Receive a message on the specified socket.
*
* @param    sock The socket to be used.
*
* @param    buffer The buffer containing the message to receive.
*
* @param    size The buffer size in bytes.
*
* @param    received_count Receives the number of bytes received into the buffer.
*
* @return   @c 0 if successful.
*           MU_FAILURE means an unexpected error has occurred and the socket must be destroyed.
*/
MOCKABLE_FUNCTION(, int, socket_async_receive, SOCKET_ASYNC_HANDLE, sock, void*, buffer, size_t, size, size_t*, received_count);


/**
* @brief    Close the socket returned by socket_async_create.
*
* @param   sock     The socket to be destroyed (closed, in standard socket terms).
*/
MOCKABLE_FUNCTION(, void, socket_async_destroy, SOCKET_ASYNC_HANDLE, sock);


#ifdef __cplusplus
}
#endif

#endif /* AZURE_SOCKET_ASYNC_H */
