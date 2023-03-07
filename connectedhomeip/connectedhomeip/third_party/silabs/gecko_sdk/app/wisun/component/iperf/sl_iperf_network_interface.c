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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_iperf_network_interface.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/// Any address allocation and initialization
const sl_iperf_socket_in6addr_t sl_iperf_socket_inaddr_any = { 0 };

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
__attribute__((weak)) void sl_iperf_nw_interface_init(void)
{
  (void) 0;
}

__attribute__((weak)) int32_t sl_iperf_socket_create(sl_iperf_protocol_t protocol)
{
  (void) protocol;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) int32_t sl_iperf_socket_close(int32_t sockid)
{
  (void) sockid;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) int32_t sl_iperf_socket_bind(int32_t sockid, const sl_iperf_socket_addr_t *addr)
{
  (void) sockid;
  (void) addr;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) int32_t sl_iperf_socket_listen(int32_t sockid, int32_t backlog)
{
  (void) sockid;
  (void) backlog;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) int32_t sl_iperf_socket_accept(int32_t sockid, sl_iperf_socket_addr_t *addr)
{
  (void) sockid;
  (void) addr;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) int32_t sl_iperf_socket_connect(int32_t sockid, const sl_iperf_socket_addr_t *addr)
{
  (void) sockid;
  (void) addr;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) int32_t sl_iperf_socket_send(int32_t sockid, const void *buff, size_t len)
{
  (void) sockid;
  (void) buff;
  (void) len;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) int32_t sl_iperf_socket_sendto(int32_t sockid, const void *buff, uint32_t len, const sl_iperf_socket_addr_t *dest_addr)
{
  (void) sockid;
  (void) buff;
  (void) len;
  (void) dest_addr;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) int32_t sl_iperf_socket_recv(int32_t sockid, void *buff, size_t len)
{
  (void) sockid;
  (void) buff;
  (void) len;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) int32_t sl_iperf_socket_recvfrom(int32_t sockid, void *buff, uint32_t len,
                                                       sl_iperf_socket_addr_t *src_addr)
{
  (void) sockid;
  (void) buff;
  (void) len;
  (void) src_addr;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) int32_t sl_iperf_socket_setsockopt(int32_t sockid, int32_t level, int32_t optname,
                                                         const void *optval, size_t optlen)
{
  (void) sockid;
  (void) level;
  (void) optname;
  (void) optval;
  (void) optlen;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) int32_t sl_iperf_socket_getsockopt(int32_t sockid, int32_t level, int32_t optname,
                                                         void *optval, size_t *optlen)
{
  (void) sockid;
  (void) level;
  (void) optname;
  (void) optval;
  (void) optlen;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) void sl_iperf_set_socket_addr_family(sl_iperf_socket_addr_t * const addr)
{
  (void) addr;
}

__attribute__((weak)) void sl_iperf_set_socket_addr_port(sl_iperf_socket_addr_t * const addr, const uint16_t port)
{
  (void) addr;
  (void) port;
}

__attribute__((weak)) uint16_t sl_iperf_get_socket_addr_port(const sl_iperf_socket_addr_t * const addr)
{
  (void) addr;
  return 0;
}

__attribute__((weak)) void sl_iperf_set_socket_addr_ip(sl_iperf_socket_addr_t * const addr, const void * const ip)
{
  (void) addr;
  (void) ip;
}

__attribute__((weak)) void sl_iperf_get_socket_addr_ip(const sl_iperf_socket_addr_t * const addr, void * const ip)
{
  (void) addr;
  (void) ip;
}

__attribute__((weak)) bool sl_iperf_network_is_connected(void)
{
  return false;
}

__attribute__((weak)) uint16_t sl_iperf_network_htons(uint16_t val)
{
  return val;
}

__attribute__((weak)) uint16_t sl_iperf_network_ntohs(uint16_t val)
{
  return val;
}

__attribute__((weak)) uint32_t sl_iperf_network_htonl(uint32_t val)
{
  return val;
}

__attribute__((weak)) uint32_t sl_iperf_network_ntohl(uint32_t val)
{
  return val;
}

__attribute__((weak)) int32_t sl_iperf_inet_pton(const char *src_str,
                                                 sl_iperf_socket_addr_t * const dst_addr)
{
  (void) src_str;
  (void) dst_addr;
  return SL_IPERF_NW_API_ERROR;
}

__attribute__((weak)) const char *sl_iperf_inet_ntop(const  sl_iperf_socket_addr_t * const src_addr,
                                                     char * const dst_str,
                                                     const size_t size)
{
  (void) src_addr;
  (void) dst_str;
  (void) size;
  return NULL;
}

__attribute__((weak)) void sl_iperf_delay_ms(const uint32_t ms)
{
  (void) ms;
}

__attribute__((weak)) sl_iperf_ts_ms_t sl_iperf_get_timestamp_ms(void)
{
  return (sl_iperf_ts_ms_t)0U;
}
// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
