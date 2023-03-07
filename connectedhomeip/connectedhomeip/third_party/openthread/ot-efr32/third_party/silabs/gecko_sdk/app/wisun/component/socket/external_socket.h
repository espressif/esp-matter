/***************************************************************************//**
 * @file
 * @brief External socket
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef _EXTERNAL_SOCKET_H_
#define _EXTERNAL_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <inttypes.h>
#include "socket_hnd.h"
#include "socket.h"

/**************************************************************************//**
 * @defgroup SL_WISUN_EXTERNAL_SOCKET_API External socket API (weak)
 * @ingroup SL_WISUN_SOCKET
 * @{
 *****************************************************************************/

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/*External socket handlers*/
/**************************************************************************//**
 * @brief Create and endpoint for communication and return a socket ID. (External socket)
 * @details Future implementation
 * @param[in] domain The communication domain, this selects the protocol family.
 * @param[in] type The communication semantics
 * @param[in] protocol Specifies the particular protocol to be used.
 * @return if any error occurred, return with -1, otherwise return with the socket id
 *****************************************************************************/
int32_t _external_socket(sock_domain_t domain, sock_type_t type, int32_t protocol);

/**************************************************************************//**
 * @brief Close socket (External socket).
 * @details Future implementation
 * @param[in] sockfd socket id
 * @return if any error occurred, return with -1, otherwise return with the socket id
 *****************************************************************************/
int32_t _external_close(int32_t sockfd);

/**************************************************************************//**
 * @brief Bind a name to a socket. (External socket).
 * @details Future implementation
 * @param[in] sockfd socket id
 * @param[in] addr address structure ptr
 * @param[in] addrlen address structure size
 * @return On success, zero is returned.  On error, -1 is returned
 *****************************************************************************/
int32_t _external_bind(int32_t sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**************************************************************************//**
 * @brief Listen for connections on a socket. (External socket).
 * @details Future implementation
 * @param[in] sockfd socket id
 * @param[in] backlog Argument defines the maximum length to
 *                    which the queue of pending connections for sockfd may grow.
 * @return On success, zero is returned.  On error, -1 is returned
 *****************************************************************************/
int32_t _external_listen(int32_t sockfd, int32_t backlog);

/**************************************************************************//**
 * @brief Accept a connection on a socket. (External socket).
 * @details Future implementation
 * @param[in] sockfd socket id
 * @param[in,out] addr Is filled in with the address of the peer (remote) socket
 * @param[in,out] addrlen It is the byte size of the address.
 * @return On success, return with the socket id for accepted socket, on error -1 is returned.
 *****************************************************************************/
int32_t _external_accept(int32_t sockfd, struct sockaddr *addr, socklen_t *addrlen);

/**************************************************************************//**
 * @brief Initiate a connection on a socket (External socket).
 * @details Future implementation
 * @param[in] sockfd socket id
 * @param[in,out] addr If the socket sockfd is of type SOCK_DGRAM, then addr is the
 *                     address to which datagrams are sent by default, and the only
 *                     address from which datagrams are received.
 *                     If the socket is of type SOCK_STREAM, this call attempts to make a
 *                     connection to the socket that is bound to the address specified by addr.
 * @param[in,out] addrlen It is the byte size of the address.
 * @return If the connection or binding succeeds, zero is returned. On error, -1 is returned.
 *****************************************************************************/
int32_t _external_connect(int32_t sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**************************************************************************//**
 * @brief Send a message on a socket (External socket).
 * @details Future implementation
 * @param[in] sockfd socket id
 * @param[in] buf buffer pointer to send
 * @param[in] len length of buffer to send
 * @param[in] flags flags
 * @return On success, these calls return the number of bytes sent. On error, -1 is returned
 *****************************************************************************/
int32_t _external_send(int32_t sockfd, const void *buf, uint32_t len, int32_t flags);

/**************************************************************************//**
 * @brief Send a message on a socket. (External socket).
 * @details Future implementation
 * @param[in] sockfd socket id
 * @param[in] buf buffer pointer to send
 * @param[in] len length of buffer to send
 * @param[in] flags flags
 * @param[in] dest_addr destination address ptr, the structure must be prepared for UDP sockets
 * @param[in] addrlen destination address length
 * @return On success, these calls return the number of bytes sent. On error, -1 is returned
 *****************************************************************************/
int32_t _external_sendto(int32_t sockfd, const void *buf, uint32_t len,
                         int32_t flags, const struct sockaddr *dest_addr, socklen_t addrlen);

/**************************************************************************//**
 * @brief Receive a message from a socket. (External socket).
 * @details Future implementation
 * @param[in] sockfd socket id
 * @param[out] buf destination buffer ptr
 * @param[in] len length of data to read
 * @param[in] flags flags
 * @return return the number of bytes received, or -1 if an error occurred
 *****************************************************************************/
int32_t _external_recv(int32_t sockfd, const void *buf, uint32_t len, int32_t flags);

/**************************************************************************//**
 * @brief Receive messages from a socket (External socket).
 * @details Future implementation
 * @param[in] sockfd socket id
 * @param[out] buf destination buffer ptr
 * @param[in] len length of data to read
 * @param[in] flags flags
 * @param[in] src_addr Source address
 * @param[in] addrlen length of the source address
 * @return return the number of bytes received, or -1 if an error occurred
 *****************************************************************************/
int32_t _external_recvfrom(int32_t sockfd, void *buf, uint32_t len, int32_t flags,
                           struct sockaddr *src_addr, socklen_t *addrlen);

/**************************************************************************//**
 * @brief Set socket option.
 * @details Future implementation
 * @param[in] socket socket id
 * @param[in] level level
 * @param[in] optname Option name
 * @param[in] optval Option value ptr
 * @param[in] optlen Must be the size of sl_wisun_socket_option_data_t union
 * @return Return 0 on success, other wise -1
 *****************************************************************************/
int32_t _external_setsockopt(int32_t socket, int32_t level, int32_t optname,
                             const void *optval, socklen_t optlen);

/**************************************************************************//**
 * @brief Get socket option (External socket).
 * @details Future implementation
 * @param[in] sockfd socket id
 * @param[in] level Level.
 * @param[in] optname Option name.
 * @param[out] optval Destination structure pointer.
 * @param[in] optlen Must be the size of sl_wisun_socket_option_data_t union
 * @return Return 0 on success, other wise -1
 *****************************************************************************/
int32_t _external_getsockopt(int32_t sockfd, int32_t level, int32_t optname,
                             void *optval, socklen_t *optlen);

/** @}*/

#ifdef __cplusplus
}
#endif
#endif
