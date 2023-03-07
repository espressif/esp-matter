/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __IPERF_NETWORK_INTERFACE_H__
#define __IPERF_NETWORK_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "sl_iperf_config.h"

/**************************************************************************//**
 * @addtogroup SL_IPERF_NETWORK_INTERFACE_API iPerf - Network interface
 * @ingroup SL_IPERF
 * @{
 *****************************************************************************/

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Size definition of IPv6 address
#define SL_IPERF_IN6ADDR_SIZE     (16U)

/// iPerf timestamp milisecond data type deifinition
typedef uint64_t sl_iperf_ts_ms_t;

/// iPerf time data type definition
typedef struct sl_iperf_time {
  /// Seconds
  uint32_t sec;
  /// Microseconds
  uint32_t usec;
} sl_iperf_time_t;

/// Iperf socket IP protocol type definition enum
typedef enum sl_iperf_protocol {
  /// UDP IPv6 protocol
  SL_IPERF_IPROTOV6_UDP = 0,
  /// TCP IPv6 protocol
  SL_IPERF_IPROTOV6_TCP,
  /// UDP IPv4 protocol
  SL_IPERF_IPROTOV4_UDP,
  /// TCP IPv4 protocol
  SL_IPERF_IPROTOV4_TCP
} sl_iperf_protocol_t;

/// Raw ip address byes
typedef struct sl_iperf_socket_in6addr {
  /// IP address bytes
  uint8_t ip[SL_IPERF_IN6ADDR_SIZE];
} sl_iperf_socket_in6addr_t;

/// Socket address type definition wrapper
typedef struct  sl_iperf_socket_addr {
  /// Addres structure bytes
  uint8_t addr[SL_IPERF_SOCKET_ADDR_LEN];
} sl_iperf_socket_addr_t;

/// Assert socket address length macro function
#define sl_iperf_assert_sock_addr_len(__type) \
  _Static_assert(sizeof(__type) <= sizeof(sl_iperf_socket_addr_t), "SL_IPERF_SOCKET_ADDR_LEN not set properly")

/// Define network api ERROR value
#define SL_IPERF_NW_API_ERROR        (-1)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

/// Anyaddress constant
extern const sl_iperf_socket_in6addr_t sl_iperf_socket_inaddr_any;

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Init iPerf Network Interface
 * @details Initialize network interface
 *****************************************************************************/
void sl_iperf_nw_interface_init(void);

/**************************************************************************//**
 * @brief Create iPerf socket
 * @details Create a socket by selected protocol and return with the socket ID
 * @param[in] protocol Protocol
 * @return int32_t Socket ID
 *****************************************************************************/
int32_t sl_iperf_socket_create(sl_iperf_protocol_t protocol);

/**************************************************************************//**
 * @brief Close socket.
 * @details Close socket and remove from the socket handler storage
 * @param[in] sockid socket id
 * @return if any error occurred, return with -1, otherwise return with the socket id
 *****************************************************************************/
int32_t sl_iperf_socket_close(int32_t sockid);

/**************************************************************************//**
 * @brief Bind a name to an iPerf socket.
 * @details Assigns the address specified by addr to the socket referred to by the socket id.
 *          It is normally necessary to assign a local address using bind()
 *          before a SOCK_STREAM socket may receive connections
 * @param[in] sockid socket id
 * @param[in] addr address structure ptr
 * @return On success, zero is returned.  On error, -1 is returned
 *****************************************************************************/
int32_t sl_iperf_socket_bind(int32_t sockid, const sl_iperf_socket_addr_t *addr);

/**************************************************************************//**
 * @brief Listen for connections on an iPerf socket.
 * @details Marks the socket referred to by sockid as a passive socket,
 *          that is, as a socket that will be used to accept incoming
 *          connection requests using accept
 * @param[in] sockid socket id
 * @param[in] backlog Argument defines the maximum length to
 *                    which the queue of pending connections for sockid may grow.
 *                    Not implemented for Wi-SUN, the connection queue size is always 1
 * @return On success, zero is returned.  On error, -1 is returned
 *****************************************************************************/
int32_t sl_iperf_socket_listen(int32_t sockid, int32_t backlog);

/**************************************************************************//**
 * @brief Accept a connection on a socket
 * @details It is used with connection-based socket types (TCP).
 *          It extracts the first connection request on the queue of pending connections for the
 *          listening socket.
 * @param[in] sockid socket id
 * @param[in,out] addr Is filled in with the address of the peer (remote) socket
 *                     In Wi-SUN case, it shouldn't be NULL ptr, but for external sockets should be.
 * @return On success, return with the socket id for accepted socket, on error -1 is returned.
 *****************************************************************************/
int32_t sl_iperf_socket_accept(int32_t sockid, sl_iperf_socket_addr_t *addr);

/**************************************************************************//**
 * @brief Initiate a connection on a socket.
 * @details Connects the socket referred to by the socketid to the address specified by addr.
 * @param[in] sockid socket id
 * @param[in,out] addr If the socket sockid is of type SOCK_DGRAM, then addr is the
 *                     address to which datagrams are sent by default, and the only
 *                     address from which datagrams are received.
 *                     If the socket is of type SOCK_STREAM, this call attempts to make a
 *                     connection to the socket that is bound to the address specified by addr.
 * @param[in,out] addrlen It is the byte size of the address.
 *                In Wi-SUN, it won't be set to the actual size on return.
 * @return If the connection or binding succeeds, zero is returned. On error, -1 is returned.
 *****************************************************************************/
int32_t sl_iperf_socket_connect(int32_t sockid, const sl_iperf_socket_addr_t *addr);

/**************************************************************************//**
 * @brief Send a message on a socket,
 * @details It is preferred with connection-oriented sockets (TCP).
 * @param[in] sockid socket id
 * @param[in] buff buffer pointer to send
 * @param[in] len length of buffer to send
 * @return On success, these calls return the number of bytes sent. On error, -1 is returned
 *****************************************************************************/
int32_t sl_iperf_socket_send(int32_t sockid, const void *buff, size_t len);

/**************************************************************************//**
 * @brief Send a message on a socket.
 * @details It is preferred in datagram mode (UDP).
 * @param[in] sockid socket id
 * @param[in] buff buffer pointer to send
 * @param[in] len length of buffer to send
 * @param[in] dest_addr destination address ptr, the structure must be prepared for UDP sockets
 * @param[in] addr_len destination address length
 * @return On success, these calls return the number of bytes sent. On error, -1 is returned
 *****************************************************************************/
int32_t sl_iperf_socket_sendto(int32_t sockid, const void *buff, uint32_t len, const sl_iperf_socket_addr_t *dest_addr);

/**************************************************************************//**
 * @brief Receive a message from a socket.
 * @details It should be used for connection-oriented protocol (TCP)
 * @param[in] sockid socket id
 * @param[out] buf destination buffer ptr
 * @param[in] len length of data to read
 * @return return the number of bytes received, or -1 if an error occurred
 *****************************************************************************/
int32_t sl_iperf_socket_recv(int32_t sockid, void *buff, size_t len);

/**************************************************************************//**
 * @brief Receive messages from a socket
 * @details It can be used to receive data on a socket whether or not it is connection-oriented.
 * @param[in] sockid socket id
 * @param[out] buf destination buffer ptr
 * @param[in] len length of data to read
 * @param[in] src_addr Source address
 * @param[in] addrlen length of the source address
 * @return return the number of bytes received, or -1 if an error occurred
 *****************************************************************************/
int32_t sl_iperf_socket_recvfrom(int32_t sockid, void *buf, uint32_t len,
                                 sl_iperf_socket_addr_t *src_addr);

/**************************************************************************//**
 * @brief Set socket option.
 * @details This function can set socket properties.
 *          There are some limitation in Wi-SUN domain: optlen and level args are not used.
 *          optname and optval are casted to the corresponding Wi-SUN types:
 *          sl_wisun_socket_option_t and  sl_wisun_socket_option_data_t.
 *          sl_wisun_socket_option_data_t is a union type to include all of implemented Wi-SUN socket options
 * @param[in] sockid socket id
 * @param[in] level Not used in Wi-SUN domain.
 * @param[in] optname Option name.
 * @param[in] optval Option value structure pointer. For Wi-SUN it is casted to sl_wisun_socket_option_t
 * @param[in] optlen Must be the size of sl_wisun_socket_option_data_t union
 * @return Return 0 on succes, other wise -1
 *****************************************************************************/
int32_t sl_iperf_socket_setsockopt(int32_t sockid, int32_t level, int32_t optname,
                                   const void *optval, size_t optlen);

/**************************************************************************//**
 * @brief Get socket option.
 * @details The function gets socket option by optname, and copy option data to optval ptr
 * @param[in] sockid socket id
 * @param[in] level Not used in Wi-SUN domain.
 * @param[in] optname Option name.
 * @param[in] optlen Must be the size of sl_wisun_socket_option_data_t union
 * @return Return 0 on success, other wise -1
 *****************************************************************************/
int32_t sl_iperf_socket_getsockopt(int32_t sockid, int32_t level, int32_t optname,
                                   void *optval, size_t *optlen);

/**************************************************************************//**
 * @brief Set iPerf socket address family
 * @details Set address family in socket address structure
 * @param[out] addr Socket address structure
 *****************************************************************************/
void sl_iperf_set_socket_addr_family(sl_iperf_socket_addr_t * const addr);

/**************************************************************************//**
 * @brief Set port in iperf address structure
 * @details Setter function of port property
 * @param[in,out] addr iPerf address
 * @param[out] port Port number
 *****************************************************************************/
void sl_iperf_set_socket_addr_port(sl_iperf_socket_addr_t * const addr, const uint16_t port);

/**************************************************************************//**
 * @brief Get port number of iperf address structure
 * @details Getter function of port property
 * @param[in] addr iPerf address
 * @return uint16_t Port number
 *****************************************************************************/
uint16_t sl_iperf_get_socket_addr_port(const sl_iperf_socket_addr_t * const addr);

/**************************************************************************//**
 * @brief Set IP address bytes in iPerf address structure
 * @details Setter function of IP address
 * @param[in, out] addr iPerf address
 * @param[in] ip IP address source buffer
 *****************************************************************************/
void sl_iperf_set_socket_addr_ip(sl_iperf_socket_addr_t * const addr, const void * const ip);

/**************************************************************************//**
 * @brief Get IP address bytes from iPerf address structure
 * @details Getter function of IP address
 * @param[in] addr iPerf address
 * @param[out] ip IP address destination buffer
 *****************************************************************************/
void sl_iperf_get_socket_addr_ip(const sl_iperf_socket_addr_t * const addr, void * const ip);

/**************************************************************************//**
 * @brief Check network connection
 * @details Checking the connection status.
 * @return true Network is connected
 * @return false Network is NOT connected
 *****************************************************************************/
bool sl_iperf_network_is_connected(void);

/**************************************************************************//**
 * @brief iPerf host to network short
 * @details htons implementation for iPerf
 * @param[in] val Value
 * @return uint16_t Converted value
 *****************************************************************************/
uint16_t sl_iperf_network_htons(uint16_t val);

/**************************************************************************//**
 * @brief iPerf network to host short
 * @details ntohs implementation for iPerf
 * @param[in] val Value
 * @return uint16_t Converted value
 *****************************************************************************/
uint16_t sl_iperf_network_ntohs(uint16_t val);

/**************************************************************************//**
 * @brief iPerf host to network long
 * @details htonl implementation for iPerf
 * @param[in] val Value
 * @return uint32_t Converted value
 *****************************************************************************/
uint32_t sl_iperf_network_htonl(uint32_t val);

/**************************************************************************//**
 * @brief iPerf network to host long
 * @details ntohl implementatino for iPerf
 * @param[in] val Value
 * @return uint32_t Converted value
 *****************************************************************************/
uint32_t sl_iperf_network_ntohl(uint32_t val);

/**************************************************************************//**
 * @brief Get milisec timestamp
 * @details Get CPU tick convert to milisec
 * @return sl_iperf_ts_ms_t Timestamp in milisec
 *****************************************************************************/
sl_iperf_ts_ms_t sl_iperf_get_timestamp_ms(void);

/**************************************************************************//**
 * @brief iPerf milisec dely
 * @details Customizable delay function
 * @param [in] ms Milisec value
 *****************************************************************************/
void sl_iperf_delay_ms(const uint32_t ms);

/**************************************************************************//**
 * @brief Convert addresses from text to binary form.
 * @details It converts the character string src into a network address structure.
 * @param[in] src_str Source string
 * @param[out] dst_addr Destination address pointer
 * @return 1 on succes, -1 on error (POSIX described the 0 value too)
 *****************************************************************************/
int32_t sl_iperf_inet_pton(const char *src_str,
                           sl_iperf_socket_addr_t * const dst_addr);

/**************************************************************************//**
 * @brief Convert IPv4 and IPv6 addresses from binary to text form.
 * @details Converts the network address structure src in the af address family
 *          into a character string.
 * @param[in] src_str Source address in byte form
 * @param[out] dst_addr Destination buffer ptr
 * @param[in] size Size of the destination buffer.
 * @return It returns a non-null pointer to dst.
 *         NULL is returned if there was an error
 *****************************************************************************/
const char *sl_iperf_inet_ntop(const  sl_iperf_socket_addr_t * const src_addr,
                               char * const dst_str,
                               const size_t size);

/**************************************************************************//**
 * @brief Set iPerf socket address
 * @details Cerate a full copy of custom source address to the destination iperf address.
 * @param[in,out] dest_addr Destination address (custom type)
 * @param[in] src_addr Source address (iPerf type)
 *****************************************************************************/
static inline void sl_iperf_set_socket_addr(sl_iperf_socket_addr_t * const dest_addr, const void * const src_addr)
{
  memcpy(dest_addr, src_addr, sizeof(sl_iperf_socket_addr_t));
}

/**************************************************************************//**
 * @brief Get socket address
 * @details Create a full copy from iperf source address into the custom destination address.
 * @param[in] src_addr Source address (iPerf type)
 * @param[in, out] dest_addr Destination address (custom type)
 *****************************************************************************/
static inline void sl_iperf_get_socket_addr(const sl_iperf_socket_addr_t * const src_addr, void * const dest_addr)
{
  memcpy(dest_addr, src_addr, sizeof(sl_iperf_socket_addr_t));
}

/** @}*/

#ifdef __cplusplus
}
#endif
#endif
