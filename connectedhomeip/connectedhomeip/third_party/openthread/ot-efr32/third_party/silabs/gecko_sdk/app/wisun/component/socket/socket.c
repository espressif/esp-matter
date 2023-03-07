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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdio.h>
#include "socket.h"
#include "socket_hnd.h"
#include "external_socket.h"
#include "sl_wisun_ip6string.h"
#include "sl_wisun_trace_util.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Check status value, set errno and return with error or retval
 * @details Parsing sl_status, set errno and return
 * @param[in] status Status value
 * @param[in] retval Return value to retrun if the status is SL_STATUS_OK
 * @param[in] errno_val errno number value if there is any error
 * @return return value by status check
 *****************************************************************************/
static inline int32_t _check_status(sl_status_t status, int32_t retval, int32_t errno_val);

/**************************************************************************//**
 * @brief Convert POSIX protocol ID to Wi-SUN eqvivalent
 * @details Default is UDP, if the protocol is correct
 * @param[in] protocol protocol ID, it can be:
 *                 IPPROTO_ICMP
 *                 IPPROTO_TCP
 *                 IPPROTO_UDP
 * @return Wi-SUN protocol id
 *****************************************************************************/
static inline sl_wisun_socket_protocol_t _proto2wisun(proto_type_t protocol);

/**************************************************************************//**
 * @brief Socket type to Wi-SUN.
 * @details For Wi-SUN:
 *             SOCK_STREAM -> TCP
 *             SOCK_DGRAM  -> UDP
 *             SOCK_RAW    -> by the protocol converter _proto2wisun
 * @param[in] type Socket type ID
 * @param[in] protocol Protocol ID
 * @return Converted protocol. Default is SL_WISUN_SOCKET_PROTOCOL_UDP
 *****************************************************************************/
static inline sl_wisun_socket_protocol_t _socktype2wisun(sock_type_t type, proto_type_t protocol);

/**************************************************************************//**
 * @brief Set special Wi-SUN socket options for socket handler
 * @details helper function
 * @param[in,out] hnd Handler
 * @param[in] optname Option name ID
 * @param[in] optval Option value ptr
 * @param[in] optlen Option length
 * @return int32_t Return with -1 on error, 0 on success.
 *****************************************************************************/
static int32_t _set_socket_handler_opt(_socket_handler_t *hnd,
                                       const int32_t optname,
                                       const void * const optval,
                                       const socklen_t optlen);

/**************************************************************************//**
 * @brief Get special Wi-SUN socket options for socket handler
 * @details helper function
 * @param[in] hnd Handler
 * @param[in] optname Option name ID
 * @param[out] optval Destination option value ptr
 * @param[in] optlen Option length
 * @return int32_t Return with -1 on error, 0 on success.
 *****************************************************************************/
static int32_t _get_socket_handler_opt(const _socket_handler_t * const hnd,
                                       const int32_t optname,
                                       void * const optval,
                                       socklen_t * const optlen);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

/**
 * @brief Any address for IPv6
 */
const in6_addr_t in6addr_any = {
  .s6_addr = {
    .address = { 0 }
  }
};

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* socket */
int32_t socket(sock_domain_t domain, sock_type_t type, proto_type_t protocol)
{
  sl_status_t stat = SL_STATUS_FAIL;
  sl_wisun_socket_id_t sock_id;

  switch (domain) {
    // Wi-SUN socket
    case AF_WISUN:
      stat = sl_wisun_open_socket(_socktype2wisun(type, protocol), &sock_id);
      if (stat == SL_STATUS_OK
          && socket_handler_add_sockid((uint16_t) domain, sock_id) != RETVAL_ERROR) {
        return sock_id;
      } else {
        __CHECK_FOR_STATUS(stat);
        _set_errno_ret_error(EINVAL);
      }

    // External socket
    case AF_INET:
    case AF_INET6: return _external_socket(domain, type, protocol);
    default:
      _set_errno_ret_error(EINVAL);
  }
}

/* close */
int32_t close(int32_t sockid)
{
  sl_status_t stat = SL_STATUS_FAIL;
  const _socket_handler_t *sockhnd = NULL;
  sockhnd = socket_handler_get(sockid);

  if (sockhnd == NULL) {
    _set_errno_ret_error(EINVAL);
  }
  switch (sockhnd->_domain) {
    // Wi-SUN socket
    case AF_WISUN:
      // remove socket from the socket storage
      if (socket_handler_remove(sockid) == RETVAL_ERROR) {
        _set_errno_ret_error(EINVAL);
      }
      // close socket
      stat = sl_wisun_close_socket((sl_wisun_socket_id_t) sockid);
      __CHECK_FOR_STATUS(stat);
      return _check_status(stat, RETVAL_OK, EINVAL);

    // External socket
    case AF_INET:
    case AF_INET6: return _external_close(sockid);
    default:
      _set_errno_ret_error(EINVAL);
  }
}

/* Bind a name to a socket */
int32_t bind(int32_t sockid, const struct sockaddr *addr, socklen_t addrlen)
{
  sl_status_t stat = SL_STATUS_FAIL;
  const wisun_addr_t *wisun_addr = NULL;
  _socket_handler_t *sockhnd = NULL;

  // Get socket handler
  sockhnd = socket_handler_get(sockid);
  if (sockhnd == NULL) { // tried to bind addresss to non existing socket id
    _set_errno_ret_error(EINVAL);
  }

  switch (sockhnd->_domain) {
    case AF_WISUN:
      if (addr == NULL || addrlen != sizeof(wisun_addr_t)
          || addr->sa_family != AF_WISUN) {
        _set_errno_ret_error(EINVAL);
      }
      // cast address to wisun address
      wisun_addr = (const wisun_addr_t *)addr;

      stat = sl_wisun_bind_socket((sl_wisun_socket_id_t) sockid,  // socket id
                                  &wisun_addr->sin6_addr.s6_addr, // IPv6 address structure pointer
                                  wisun_addr->sin6_port);         // port number
      if (stat == SL_STATUS_OK) { // if the stack call return OK, add address struct
        socket_handler_set_sockaddr(sockhnd, addr, (uint8_t)addrlen);
      }
      __CHECK_FOR_STATUS(stat);
      return _check_status(stat, RETVAL_OK, EINVAL);

    // External socket
    case AF_INET:
    case AF_INET6:
      return _external_bind(sockid, addr, addrlen);
    default:
      _set_errno_ret_error(EINVAL);
  }
}

/* Listen on socket */
int32_t listen(int32_t sockid, int32_t backlog)
{
  const _socket_handler_t *sockhnd = NULL;
  sl_status_t stat = SL_STATUS_FAIL;

  // Getting socket handler
  sockhnd = socket_handler_get(sockid);

  if (sockhnd == NULL) {
    return RETVAL_ERROR;
  }

  switch (sockhnd->_domain) {
    case AF_WISUN:
      (void) backlog; // ignore backlog because wi-sun stack defined 1 backlog capability
      stat = sl_wisun_listen_on_socket((sl_wisun_socket_id_t) sockhnd->_socket_id);
      __CHECK_FOR_STATUS(stat);
      return _check_status(stat, RETVAL_OK, EINVAL);

    // External socket
    case AF_INET:
    case AF_INET6:  return _external_listen(sockid, backlog);

    default:
      _set_errno_ret_error(EINVAL);
  }
}

/* accept on socket */
int32_t accept(int32_t sockid, struct sockaddr *addr, socklen_t *addrlen)
{
  _socket_handler_t *sockhnd = NULL;
  wisun_addr_t *remote_wisunaddr = NULL;
  sl_status_t stat = SL_STATUS_FAIL;
  sl_wisun_socket_id_t remote_sock_id;
  bool conn_available = false;
  sockhnd = socket_handler_get(sockid);

  // No socket in the socket socket storage with this id
  if (sockhnd == NULL) {
    return RETVAL_ERROR;
  }

  switch (sockhnd->_domain) {
    // Wi-SUN socket
    case AF_WISUN:
      if (addr == NULL || addrlen == NULL || *addrlen != sizeof(wisun_addr_t)) { // size is not correct
        _set_errno_ret_error(EINVAL);
      }
      remote_wisunaddr = (wisun_addr_t *) addr;
      remote_wisunaddr->sin6_family = AF_WISUN;

      while (!conn_available) { // waiting for connection available flag set in event handler
        socket_handler_get_state(sockhnd, SOCKET_STATE_CONNECTION_AVAILABLE, &conn_available);
        osDelay(1); // dispatch
      }
      socket_handler_set_state(sockhnd, SOCKET_STATE_CONNECTION_AVAILABLE, false);

      stat = sl_wisun_accept_on_socket((sl_wisun_socket_id_t) sockid,                           // socket listener id
                                       &remote_sock_id,                                         // client id to set
                                       (sl_wisun_ip_address_t *) &remote_wisunaddr->sin6_addr,  // IPv6 address structure
                                       &remote_wisunaddr->sin6_port);                           // Port number

      if (stat == SL_STATUS_OK
          && socket_handler_add(AF_WISUN, remote_sock_id, (void *) addr, (uint8_t)*addrlen) != RETVAL_ERROR) {
        return remote_sock_id;
      } else {
        __CHECK_FOR_STATUS(stat);
        _set_errno_ret_error(EINVAL);
      }

    // External socket
    case AF_INET:
    case AF_INET6:  return _external_accept(sockid, addr, addrlen);
    default:
      _set_errno_ret_error(EINVAL);
  }
}

/* connect */
int32_t connect(int32_t sockid, const struct sockaddr *addr, socklen_t addrlen)
{
  _socket_handler_t *sockhnd = NULL;
  const wisun_addr_t *wisun_remoteaddr = NULL;
  sl_status_t stat = SL_STATUS_FAIL;
  sockhnd = socket_handler_get(sockid);

  // No socket in the socket socket storage with this id
  if (sockhnd == NULL) {
    return RETVAL_ERROR;
  }

  switch (sockhnd->_domain) {
    // Wi-SUN socket
    case AF_WISUN:
      if (addr == NULL || addrlen != sizeof(wisun_addr_t)) { // size is not correct
        _set_errno_ret_error(EINVAL);
      }
      wisun_remoteaddr = (const wisun_addr_t *)addr;
      stat = sl_wisun_connect_socket((sl_wisun_socket_id_t) sockid,
                                     (const sl_wisun_ip_address_t *)&wisun_remoteaddr->sin6_addr,
                                     wisun_remoteaddr->sin6_port);

      if (stat == SL_STATUS_OK) {
        // storing the remote server address
        socket_handler_write_remote_addr(sockhnd, &wisun_remoteaddr->sin6_family,
                                         sizeof(wisun_remoteaddr->sin6_family),
                                         SOCKADDR_WISUN_FAMILY_OFFSET);
        socket_handler_write_remote_addr(sockhnd, &wisun_remoteaddr->sin6_addr,
                                         sizeof(wisun_remoteaddr->sin6_addr),
                                         SOCKADDR_WISUN_ADDRESS_OFFSET);
        socket_handler_write_remote_addr(sockhnd, &wisun_remoteaddr->sin6_port,
                                         sizeof(uint16_t),
                                         SOCKADDR_WISUN_PORT_OFFSET);
        return RETVAL_OK;
      } else {
        __CHECK_FOR_STATUS(stat);
        _set_errno_ret_error(EINVAL);
      }

    // External socket
    case AF_INET:
    case AF_INET6:  return _external_connect(sockid, addr, addrlen);
    default:
      _set_errno_ret_error(EINVAL);
  }
}

/* send */
int32_t send(int32_t sockid, const void *buf, uint32_t len, int32_t flags)
{
  const _socket_handler_t *sockhnd = NULL;
  sl_status_t stat = SL_STATUS_FAIL;

  sockhnd = socket_handler_get(sockid);
  if (sockhnd == NULL) {
    return RETVAL_ERROR;
  }

  switch (sockhnd->_domain) {
    case AF_WISUN:
      (void) flags;
      stat = sl_wisun_send_on_socket((sl_wisun_socket_id_t) sockid,
                                     (uint16_t) len,
                                     buf);
      if (stat == SL_STATUS_OK) {
        return (int32_t) len;
      } else {
        __CHECK_FOR_STATUS(stat);
        _set_errno(EINVAL);
        return RETVAL_ERROR;
      }

    // External socket
    case AF_INET:
    case AF_INET6: return _external_send(sockid, buf, len, flags);

    default:
      _set_errno_ret_error(EINVAL);
  }
}

/* sendto */
int32_t sendto(int32_t sockid, const void *buf, uint32_t len,
               int32_t flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
  const _socket_handler_t *sockhnd = NULL;
  const wisun_addr_t *wisun_remoteaddr = NULL;
  sl_status_t stat = SL_STATUS_FAIL;

  // No socket in the socket socket storage with this id
  sockhnd = socket_handler_get(sockid);
  if (sockhnd == NULL) {
    return RETVAL_ERROR;
  }

  switch (sockhnd->_domain) {
    case AF_WISUN:
      (void) flags;
      // checking length and dest addr

      if (addrlen != sizeof(wisun_addr_t)
          || dest_addr == NULL) {
        _set_errno_ret_error(EINVAL);
      }
      wisun_remoteaddr = (const wisun_addr_t *)dest_addr;
      stat = sl_wisun_sendto_on_socket((sl_wisun_socket_id_t) sockid,                          // socket descriptor
                                       (sl_wisun_ip_address_t *)&wisun_remoteaddr->sin6_addr,  // remote address
                                       wisun_remoteaddr->sin6_port,                            // remote port
                                       (uint16_t)len,                                          // length of buffer
                                       buf);                                                   // buffer
      if (stat == SL_STATUS_OK) {
        return (int32_t) len;
      } else {
        __CHECK_FOR_STATUS(stat);
        _set_errno(EINVAL);
        return RETVAL_ERROR;
      }

    // External socket
    case AF_INET:
    case AF_INET6: return _external_sendto(sockid, buf, len, flags, dest_addr, addrlen);

    default:
      _set_errno_ret_error(EINVAL);
  }
}

/* recv */
int32_t recv(int32_t sockid, void *buf, uint32_t len, int32_t flags)
{
  _socket_handler_t *sockhnd = NULL;
  uint32_t real_length = len;
  // No socket in the socket socket storage with this id
  sockhnd = socket_handler_get(sockid);
  if (sockhnd == NULL) {
    return SOCKET_EOF; // if socket is not available anymore, indicate that to the application
  }

  switch (sockhnd->_domain) {
    case AF_WISUN:
      (void) flags;

      socket_handler_fifo_read(sockhnd, buf, &real_length);

      // if zero, should bre return with ERROR -1, 0 is reserved for closed connection
      if (!real_length) {
        _set_errno_ret_error(EINVAL);
      } else {
        return real_length;
      }

    /*External socket*/
    case AF_INET:
    case AF_INET6: return _external_recv(sockid, buf, len, flags);

    default:
      _set_errno_ret_error(EINVAL);
  }
}

/* revfrom */
int32_t recvfrom(int32_t sockid, void *buf, uint32_t len, int32_t flags,
                 struct sockaddr *src_addr, socklen_t *addrlen)
{
  _socket_handler_t *sockhnd = NULL;
  uint32_t real_length = len;

  // No socket in the socket socket storage with this id
  sockhnd = socket_handler_get(sockid);
  if (sockhnd == NULL) {
    return SOCKET_EOF; // if socket is not available anymore, indicate that to the application
  }

  switch (sockhnd->_domain) {
    case AF_WISUN:
      (void) flags;
      // checking length and source addr
      if (addrlen == NULL || *addrlen != sizeof(wisun_addr_t) || src_addr == NULL) {
        _set_errno_ret_error(EINVAL);
      }

      if (socket_handler_read_remote_addr(sockhnd, src_addr, sizeof(wisun_addr_t), 0) == RETVAL_ERROR) {
        _set_errno_ret_error(EINVAL);
      } else {
        socket_handler_fifo_read(sockhnd, buf, &real_length); // underflow not handled, real_lenght contains the read data
      }
      return real_length;

    /*External socket*/
    case AF_INET:
    case AF_INET6: return _external_recvfrom(sockid, buf, len, flags, src_addr, addrlen);

    default:
      _set_errno_ret_error(EINVAL);
  }
}

/* set sock opt */
int32_t setsockopt(int32_t sockid, int32_t level, int32_t optname,
                   const void *optval, socklen_t optlen)
{
  _socket_handler_t *sockhnd = NULL;
  sl_status_t stat = SL_STATUS_FAIL;

  sockhnd = socket_handler_get(sockid);
  if (sockhnd == NULL) {
    return RETVAL_ERROR;
  }

  switch (sockhnd->_domain) {
    case AF_WISUN:
      if (level == SOL_SOCKET_HANDLER) {
        return _set_socket_handler_opt(sockhnd, optname, optval, optlen);
      } else if (level == SOL_SOCKET) {
        // if the opt lent is not set to the sizeof sl_wisun_socket_option_data_t
        // return with erro in Wi-SUN mode
        if (optlen != sizeof(sl_wisun_socket_option_data_t)) {
          _set_errno_ret_error(EINVAL);
        }
        stat = sl_wisun_set_socket_option((sl_wisun_socket_id_t) sockhnd->_socket_id,
                                          (sl_wisun_socket_option_t) optname,
                                          (const sl_wisun_socket_option_data_t *)optval);
        __CHECK_FOR_STATUS(stat);
        return _check_status(stat, RETVAL_OK, EINVAL);
      } else {
        _set_errno_ret_error(EINVAL);
      }
    case AF_INET:
    case AF_INET6: return _external_setsockopt(sockid, level, optname, optval, optlen);
    default:
      _set_errno_ret_error(EINVAL);
  }
}

/* getsockopt */
int32_t getsockopt(int32_t sockid, int32_t level, int32_t optname,
                   void *optval, socklen_t *optlen)
{
  const _socket_handler_t *sockhnd = NULL;
  sl_status_t stat = SL_STATUS_FAIL;

  sockhnd = socket_handler_get(sockid);
  if (sockhnd == NULL) {
    return RETVAL_ERROR;
  }

  switch (sockhnd->_domain) {
    case AF_WISUN:
      if (level == SOL_SOCKET_HANDLER) {
        return _get_socket_handler_opt(sockhnd, optname, optval, optlen);
      } else if (level == SOL_SOCKET) {
        // if the opt lent is not set to the sizeof sl_wisun_socket_option_data_t
        // return with erro in Wi-SUN mode
        if (*optlen != sizeof(sl_wisun_socket_option_data_t)) {
          _set_errno_ret_error(EINVAL);
        }
        stat = sl_wisun_get_socket_option((sl_wisun_socket_id_t) sockhnd->_socket_id,
                                          (sl_wisun_socket_option_t) optname,
                                          (sl_wisun_socket_option_data_t *) optval);
        __CHECK_FOR_STATUS(stat);
        return _check_status(stat, RETVAL_OK, EINVAL);
      } else {
        _set_errno_ret_error(EINVAL);
      }
    case AF_INET:
    case AF_INET6: return _external_getsockopt(sockid, level, optname, optval, optlen);
    default:
      _set_errno_ret_error(EINVAL);
  }
}

/*convert IPv4 and IPv6 addresses from text to binary form*/
int32_t inet_pton(sock_domain_t af, const char *src, void *dst)
{
  bool ipv6_res = false;
  uint8_t len;
  const char *p;
  switch (af) {
    case AF_WISUN:
    case AF_INET6:
      for (p = src, len = 0; *p; ++p, ++len) {
        ;
      }
      ipv6_res = sl_wisun_stoip6(src, len, dst); // convert address text to binary for wisun and ipv6
      return ipv6_res ? 1 : -1;                  // 1: success, -1: error
    case AF_INET:
      (void) 0;
    default:
      _set_errno_ret_error(EINVAL);
  }
}

/*convert IPv4 and IPv6 addresses from binary to text form*/
const char *inet_ntop(sock_domain_t af, const void *src, char *dst, socklen_t size)
{
  bool ipv6_res = false;
  switch (af) {
    case AF_WISUN:
    case AF_INET6:
      (void) size;
      ipv6_res = sl_wisun_ip6tos(src, dst);  // convert address binary to text for wisun and ipv6
      return ipv6_res ? dst : NULL;          // dst: success, NULL: error
    case AF_INET:
      (void) 0;
    default:
      _set_errno(EINVAL);
      return NULL;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static int32_t _set_socket_handler_opt(_socket_handler_t *hnd,
                                       const int32_t optname,
                                       const void * const optval,
                                       socklen_t optlen)
{
  switch (optname) {
    case SO_HANDLER_OVERWRITE_PREV_RCV_BUFF:
      if (optlen < sizeof(bool)) {
        return RETVAL_ERROR;
      }
      return socket_handler_fifo_set_overwrite(hnd, (bool *)optval);
    default: return RETVAL_ERROR;
  }
}

static int32_t _get_socket_handler_opt(const _socket_handler_t * const hnd,
                                       const int32_t optname,
                                       void * const optval,
                                       socklen_t * const optlen)
{
  switch (optname) {
    case SO_HANDLER_OVERWRITE_PREV_RCV_BUFF:
      if (*optlen < sizeof(bool)) {
        return RETVAL_ERROR;
      }
      *optlen = sizeof(bool);
      return socket_handler_fifo_get_overwrite(hnd, (bool *)optval);

    default: return RETVAL_ERROR;
  }
}

static inline int32_t _check_status(sl_status_t status, int32_t retval, int32_t errno_val)
{
  if (status == SL_STATUS_OK) {
    return retval;
  }
  _set_errno(errno_val);
  return RETVAL_ERROR;
}

static inline sl_wisun_socket_protocol_t _proto2wisun(proto_type_t protocol)
{
  switch (protocol) {
    case IPPROTO_ICMP: return SL_WISUN_SOCKET_PROTOCOL_ICMP;
    case IPPROTO_UDP:  return SL_WISUN_SOCKET_PROTOCOL_UDP;
    case IPPROTO_TCP:  return SL_WISUN_SOCKET_PROTOCOL_TCP;
    default:           return SL_WISUN_SOCKET_PROTOCOL_UDP; // default
  }
}

static inline sl_wisun_socket_protocol_t _socktype2wisun(sock_type_t type, proto_type_t protocol)
{
  // Wi-SUN domain only
  switch (type) {
    case SOCK_STREAM: return SL_WISUN_SOCKET_PROTOCOL_TCP;
    case SOCK_DGRAM:  return SL_WISUN_SOCKET_PROTOCOL_UDP;
    case SOCK_RAW:    return _proto2wisun(protocol);
    default:          return SL_WISUN_SOCKET_PROTOCOL_UDP;
  }
}
