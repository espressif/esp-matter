/***************************************************************************//**
 * @file
 * @brief Socket
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
#ifndef __SOCKET_H__
#define __SOCKET_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stddef.h>
#include <inttypes.h>
#include "sl_status.h"
#include "sl_wisun_api.h"
#include "sl_wisun_types.h"
#include "errno.h"

/**************************************************************************//**
 * @addtogroup SL_WISUN_SOCKET_API POSIX-compliant Socket
 * @{
 *****************************************************************************/

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @addtogroup SL_WISUN_SOCKET_API_TYPES Socket API type definitions
 * @ingroup SL_WISUN_SOCKET_API
 * @{
 *****************************************************************************/

/// Socket address length type definition
typedef uint32_t socklen_t;

/// Socket domain types enum
typedef enum {
  /// Wi-SUN FAN
  AF_WISUN = 1,
  /// External IPv4
  AF_INET,
  /// External IPv6
  AF_INET6
} sock_domain_t;

/// Socket type definitions enum
typedef enum {
  /// Stream socket (TCP)
  SOCK_STREAM,
  /// Datagram socket (UDP)
  SOCK_DGRAM,
  /// RAW sockets without transfer protocol (ICMP)
  SOCK_RAW
} sock_type_t;

/// Socket protocol types enum
typedef enum {
  /// Default IP protocol
  IPPROTO_AUTO = 0,
  /// Default IP protocol
  IPPROTO_IP   = 0,
  /// ICMP protocol
  IPPROTO_ICMP = 1,
  /// TCP protocol
  IPPROTO_TCP  = 2,
  /// UDP protocol
  IPPROTO_UDP  = 3
} proto_type_t;

/// IPv4 Address structure storage with padding
typedef struct sockaddr {
  /// address family, AF_xxx
  uint16_t sa_family;
  /// 14 bytes of protocol address (POSIX)
  uint8_t  sa_data[14];
} sockaddr_t;

/// IPv4 Internet address
typedef struct in_addr {
  /// that's a 32-bit int (4 bytes)
  uint32_t s_addr;
} in_addr_t;

/// Internet address structure for functions
typedef struct sockaddr_in {
  /// Address family, AF_INET
  uint16_t       sin_family;
  /// Port number
  uint16_t       sin_port;
  /// Internet address
  struct in_addr sin_addr;
  /// Same size as struct sockaddr
  uint8_t        sin_zero[8];
} sockaddr_in_t;

/// IPv6 Internet address
typedef struct in6_addr {
  /// IPv6 address
  sl_wisun_ip_address_t s6_addr;
} in6_addr_t;

/// IPv6 structure
typedef struct sockaddr_in6 {
  /// address family, AF_INET6
  uint16_t        sin6_family;
  /// port number, Network Byte Order
  uint16_t        sin6_port;
  /// IPv6 flow information
  uint32_t        sin6_flowinfo;
  /// IPv6 address
  struct in6_addr sin6_addr;
  /// Scope ID
  uint32_t        sin6_scope_id;
} sockaddr_in6_t;

/// IPv6 Internet address storage with padding
typedef struct sockaddr_storage {
  /// address family
  uint16_t  ss_family;
  /// padding implementation: size of the reminder of sockaddr_in6_t
  uint8_t _data[26];
} sockaddr_storage_t;

///  Wi-SUN address structure type definition
typedef struct sockaddr_in6 wisun_addr_t;

/** @} (end SL_WISUN_SOCKET_API_TYPES) */

/**************************************************************************//**
 * @brief Wi-SUN address structure offsets
 *****************************************************************************/
#define SOCKADDR_WISUN_FAMILY_OFFSET     (offsetof(wisun_addr_t, sin6_family))
#define SOCKADDR_WISUN_PORT_OFFSET       (offsetof(wisun_addr_t, sin6_port))
#define SOCKADDR_WISUN_FLOWINFO_OFFSET   (offsetof(wisun_addr_t, sin6_flowinfo))
#define SOCKADDR_WISUN_ADDRESS_OFFSET    (offsetof(wisun_addr_t, sin6_addr))
#define SOCKADDR_WISUN_SCOPEID_OFFSET    (offsetof(wisun_addr_t, sin6_scope_id))

/**************************************************************************//**
 * @brief Definition of any address for IPv4
 *****************************************************************************/
#define INADDR_ANY    (0UL) // IPv4

/**************************************************************************//**
 * @brief Socket option level enum type definition
 *****************************************************************************/
typedef enum socket_option_level {
  /// Special Wi-SUN Socket handler options
  SOL_SOCKET_HANDLER = 0,
  /// POSIX style
  SOL_SOCKET         = 1,
} socket_option_level_t;

/**************************************************************************//**
 * @brief Socket option level enum type definition
 *****************************************************************************/
typedef enum socket_handler_opt {
  SO_HANDLER_OVERWRITE_PREV_RCV_BUFF = -1,
} socket_handler_opt_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Definition of any address for IPv6
 *****************************************************************************/
extern const in6_addr_t in6addr_any;

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Create an endpoint for ocket ID communication and return.
 * @details Socket API supports Wi-SUN FAN and external IP network communications.
 *          External handlers can be implemented; Default implementations are weak functions.
 *          Created sockets are added to the socket handler automatically.
 * @param[in] domain The communication domain, this selects the protocol family.
 *                   It can be:
 *                      AF_WISUN     - Wi-SUN FAN
 *                      AF_INET6     - External IPv6 network socket (future implementation)
 *                      AF_INET      - External IPv4 network address (future implementation)
 * @param[in] type The communication semantics
 *                 It can be:
 *                      SOCK_STREAM  - Stream Socket type (TCP)
 *                      SOCK_DGRAM   - Datagram Socket type (UDP)
 *                      SOCK_RAW     - Raw Socket type (ICMP)
 * @param[in] protocol Specifies the particular protocol to be used.
 *                     It can be:
 *                      IPPROTO_AUTO - Auto selection SOCK_STREAM -> TCP, SOCK_DGRAM -> UDP
 *                      IPPROTO_IP   - Same then IPPROTO_AUTO
 *                      IPPROTO_ICMP - Ping
 *                      IPPROTO_TCP  - TCP protocol
 *                      IPPROTO_UDP  - UDP protocol
 *
 * @return if any error occurred, return with -1, otherwise return with the socket id
 *****************************************************************************/
int32_t socket(sock_domain_t domain, sock_type_t type, proto_type_t protocol);

/**************************************************************************//**
 * @brief Close a socket.
 * @details Close a socket and remove from the socket handler storage.
 * @param[in] sockid socket id
 * @return if any error occurred, return with -1, otherwise return with the socket id
 *****************************************************************************/
int32_t close(int32_t sockid);

/**************************************************************************//**
 * @brief Bind a name to a socket.
 * @details Assigns the address to the socket, referred to by the socket ID, as specified by addr.
 *          It is normally necessary to assign a local address using bind()
 *          before a SOCK_STREAM socket may receive connections.
 * @param[in] sockid socket id
 * @param[in] addr address structure ptr
 * @param[in] addrlen address structure size
 * @return On success, zero is returned.  On error, -1 is returned
 *****************************************************************************/
int32_t bind(int32_t sockid, const struct sockaddr *addr,
             socklen_t addrlen);

/**************************************************************************//**
 * @brief Listen for connections on a socket.
 * @details Marks the socket referred to by sockid as a passive socket,
 *          that is, as a socket that will be used to accept incoming
 *          connection requests using accept.
 * @param[in] sockid socket id
 * @param[in] backlog Argument defines the maximum length to
 *                    which the queue of pending connections for sockid may grow.
 *                    Not implemented for Wi-SUN, the connection queue size is always 1
 * @return On success, zero is returned.  On error, -1 is returned
 *****************************************************************************/
int32_t listen(int32_t sockid, int32_t backlog);

/**************************************************************************//**
 * @brief Accept a connection on a socket.
 * @details Used with connection-based socket types (TCP).
 *          It extracts the first connection request on the queue of pending connections for the
 *          listening socket.
 * @param[in] sockid socket ID
 * @param[in,out] addr Is filled in with the address of the peer (remote) socket
 *                     For Wi-SUN, it shouldn't be NULL ptr, but for external sockets should be.
 * @param[in,out] addrlen It is the byte size of the address.
 *                        For Wi-SUN, it won't be set to the actual size on return.
 * @return On success, return with the socket id for accepted socket, on error -1 is returned.
 *****************************************************************************/
int32_t accept(int32_t sockid, struct sockaddr *addr, socklen_t *addrlen);

/**************************************************************************//**
 * @brief Initiate a connection on a socket.
 * @details Connects the socket referred to by the socketid to the address specified by addr.
 * @param[in] sockid socket id
 * @param[in,out] addr If the socket sockid is of type SOCK_DGRAM, addr is the
 *                     address to which datagrams are sent by default and the only
 *                     address from which datagrams are received.
 *                     If the socket is of type SOCK_STREAM, this call attempts to make a
 *                     connection to the socket that is bound to the address specified by addr.
 * @param[in,out] addrlen It is the byte size of the address.
 *                For Wi-SUN, it won't be set to the actual size on return.
 * @return If the connection or binding succeeds, zero is returned. On error, -1 is returned.
 *****************************************************************************/
int32_t connect(int32_t sockid, const struct sockaddr *addr, socklen_t addrlen);

/**************************************************************************//**
 * @brief Send a message on a socket.
 * @details Preferred with connection-oriented sockets (TCP).
 * @param[in] sockid socket ID
 * @param[in] buff buffer pointer to send
 * @param[in] len length of buffer to send
 * @param[in] flags In Wi-SUN domain, the flags is not used.
 * @return On success, these calls return the number of bytes sent. On error, -1 is returned
 *****************************************************************************/
int32_t send(int32_t sockid, const void *buff, uint32_t len, int32_t flags);

/**************************************************************************//**
 * @brief Send a message on a socket.
 * @details Preferred in datagram mode (UDP).
 * @param[in] sockid socket ID
 * @param[in] buff buffer pointer to send
 * @param[in] len length of buffer to send
 * @param[in] flags In Wi-SUN domain, the flags parameter is not used.
 * @param[in] dest_addr destination address ptr, the structure must be prepared for UDP sockets
 * @param[in] addr_len destination address length
 * @return On success, these calls return the number of bytes sent. On error, -1 is returned
 *****************************************************************************/
int32_t sendto(int32_t sockid, const void *buff, uint32_t len,
               int32_t flags, const struct sockaddr *dest_addr, socklen_t addr_len);

/**************************************************************************//**
 * @brief Receive a message from a socket.
 * @details Should be used for connection-oriented protocol (TCP)
 * @param[in] sockid socket id
 * @param[out] buf destination buffer ptr
 * @param[in] len length of data to read
 * @param[in] flags In Wi-SUN domain, the flags is not used.
 * @return return the number of bytes received, or -1 if an error occurred
 *****************************************************************************/
int32_t recv(int32_t sockid, void *buf, uint32_t len, int32_t flags);

/**************************************************************************//**
 * @brief Receive messages from a socket
 * @details Receives data on a socket whether or not it is connection-oriented.
 * @param[in] sockid socket id
 * @param[out] buf destination buffer ptr
 * @param[in] len length of data to read
 * @param[in] flags In Wi-SUN domain, the flags is not used.
 * @param[in] src_addr Source address
 * @param[in] addrlen length of the source address
 * @return return the number of bytes received, or -1 if an error occurred
 *****************************************************************************/
int32_t recvfrom(int32_t sockid, void *buf, uint32_t len, int32_t flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);

/**************************************************************************//**
 * @brief Set socket option.
 * @details This function can set socket properties.
 *          Limitation for the Wi-SUN domain are that optlen and level args are not used.
 *          optname and optval are cast to the corresponding Wi-SUN types:
 *          sl_wisun_socket_option_t and  sl_wisun_socket_option_data_t.
 *          sl_wisun_socket_option_data_t is a union type to include all of implemented Wi-SUN socket options
 * @param[in] sockid socket ID
 * @param[in] level SO_SOCKET for Wi-SUN socket options, or SO_SOCKET_HANDLER for socket handler options.
 * @param[in] optname Option name.
 *                For Wi-SUN:
 *                SL_WISUN_SOCKET_OPTION_EVENT_MODE        - Option for socket event mode
 *                SL_WISUN_SOCKET_OPTION_MULTICAST_GROUP   - Option for multicast group
 *                SL_WISUN_SOCKET_OPTION_SEND_BUFFER_LIMIT - Option for send buffer limit
 * @param[in] optval Option value structure pointer. For Wi-SUN it is casted to sl_wisun_socket_option_t
 * @param[in] optlen Must be the size of sl_wisun_socket_option_data_t union
 * @return Return 0 on success, other wise -1
 *****************************************************************************/
int32_t setsockopt(int32_t sockid, int32_t level, int32_t optname,
                   const void *optval, socklen_t optlen);

/**************************************************************************//**
 * @brief Get socket option.
 * @details The function gets socket option by optname, and copies option data to optval ptr.
 * @param[in] sockid socket ID
 * @param[in] level SO_SOCKET for Wi-SUN socket options, or SO_SOCKET_HANDLER for socket handler options.
 * @param[in] optname Option name.
 *                For Wi-SUN:
 *                SL_WISUN_SOCKET_OPTION_EVENT_MODE        - Option for socket event mode
 *                SL_WISUN_SOCKET_OPTION_MULTICAST_GROUP   - Option for multicast group
 *                SL_WISUN_SOCKET_OPTION_SEND_BUFFER_LIMIT - Option for send buffer limit
 * @param[out] optval Destination structure pointer. For Wi-SUN it is casted to sl_wisun_socket_option_t
 * @param[in] optlen Must be the size of sl_wisun_socket_option_data_t union
 * @return Return 0 on success, other wise -1
 *****************************************************************************/
int32_t getsockopt(int32_t sockid, int32_t level, int32_t optname,
                   void *optval, socklen_t *optlen);

/**************************************************************************//**
 * @brief Convert the long host byte order to network order.
 * @details This function converts the unsigned integer hostlong from host byte order to network byte order.
 *          For Wi-SUN, the conversion is not needed. Dummy implementation
 * @param[in] hostlong Long host integer
 * @return Long network integer
 *****************************************************************************/
static inline uint32_t htonl(uint32_t hostlong)
{
  return hostlong;
}

/**************************************************************************//**
 * @brief Convert the short host byte order to network order.
 * @details This function converts the unsigned short integer hostshort from host byte order to network byte order.
 *          For Wi-SUN, the conversion is not needed. Dummy implementation
 * @param[in] hostshort Short host integer
 * @return Short network integer
 *****************************************************************************/
static inline uint16_t htons(uint16_t hostshort)
{
  return hostshort;
}

/**************************************************************************//**
 * @brief Convert the long network byte order to host byte order.
 * @details This function converts the unsigned integer netlong from network byte order to host byte order.
 *          For Wi-SUN, the conversion is not needed. Dummy implementation
 *  @param[in] netlong Long network integer
 * @return Long host integer
 *****************************************************************************/
static inline uint32_t ntohl(uint32_t netlong)
{
  return netlong;
}

/**************************************************************************//**
 * @brief Convert the short network byte order to host byte order.
 * @details This function converts the unsigned short integer netshort from the network byte order to host byte order.
 *          For Wi-SUN, the conversion is not needed. Dummy implementation
 * @param[in] netshort
 * @return Short host integer
 *****************************************************************************/
static inline uint16_t ntohs(uint16_t netshort)
{
  return netshort;
}

/**************************************************************************//**
 * @brief Convert the IPv4 and IPv6 addresses from text to binary form.
 * @details This function converts the character string src into a network address structure
 *          in the af address family, then copies the network address structure to dst.
 *          For AF_WISUN or AF_INET6, stoip6(src, len, dst) called within the implementation
 *          AF_INET (IPv4) case not implemented.
 * @param[in] af Address family. The af argument must be either AF_WISUN, AF_INET6 or AF_INET
 * @param[in] src Source string
 * @param[out] dst Destination address pointer
 * @return 1 on succes, -1 on error (POSIX described the 0 value too)
 *****************************************************************************/
int32_t inet_pton(sock_domain_t af, const char *src, void *dst);

/**************************************************************************//**
 * @brief Convert IPv4 and IPv6 addresses from binary to text form.
 * @details Converts the network address structure src in the af address family into a character string.
 *          The resulting string is copied to the buffer pointed to by dst,
 *          which must be a non-null pointer.
 *          The caller specifies the number of bytes available in this buffer in the argument size.
 *          For AF_WISUN or AF_INET6, stoip6(src, len, dst) called within the implementation
 *          AF_INET (IPv4) case not implemented.
 * @param[in] af Address family. The af argument must be either AF_WISUN, AF_INET6 or AF_INET
 * @param[in] src Source address in byte form
 * @param[out] dst Destination buffer ptr
 * @param[in] size Size of the destination buffer.
 * @return It returns a non-null pointer to dst. NULL is returned if there was an error
 *****************************************************************************/
const char *inet_ntop(sock_domain_t af, const void *src, char *dst, socklen_t size);

/** @}*/
#ifdef __cplusplus
}
#endif
#endif /* End socket.h */
