/***************************************************************************//**
 * @file
 * @brief
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

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include "cmsis_os2.h"
#include "socket.h"
#include "sl_iperf_network_interface.h"
#include "sl_iperf_util.h"
#include "sl_sleeptimer.h"
#include "sl_wisun_app_core_util.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void sl_iperf_nw_interface_init(void)
{
  // Assert address type length
  sl_iperf_assert_sock_addr_len(wisun_addr_t);
}

int32_t sl_iperf_socket_create(sl_iperf_protocol_t protocol)
{
  int32_t sockid = SL_IPERF_NW_API_ERROR;
  bool overwrite_buff = true;

  switch (protocol) {
    case SL_IPERF_IPROTOV6_UDP:
      sockid = socket(AF_WISUN, SOCK_DGRAM, IPPROTO_UDP);
      break;
    case SL_IPERF_IPROTOV6_TCP:
      sockid = socket(AF_WISUN, SOCK_STREAM, IPPROTO_TCP);
      break;
    // IPv4 is not supported on Wi-SUN
    default:                    return SL_IPERF_NW_API_ERROR;
  }
  (void) setsockopt(sockid, SOL_SOCKET_HANDLER,
                    SO_HANDLER_OVERWRITE_PREV_RCV_BUFF,
                    &overwrite_buff, sizeof(bool));
  return sockid;
}

int32_t sl_iperf_socket_close(int32_t sockid)
{
  return close(sockid);
}

int32_t sl_iperf_socket_bind(int32_t sockid, const sl_iperf_socket_addr_t *addr)
{
  return bind(sockid, (const sockaddr_t *)addr, sizeof(wisun_addr_t));
}

int32_t sl_iperf_socket_listen(int32_t sockid, int32_t backlog)
{
  // Backlog is always 1 for Wi-SUN (anyway don't care)
  return listen(sockid, backlog);
}

int32_t sl_iperf_socket_accept(int32_t sockid, sl_iperf_socket_addr_t *addr)
{
  int32_t res = SL_IPERF_NW_API_ERROR;
  socklen_t addrlen  = sizeof(wisun_addr_t);
  res = accept(sockid, (sockaddr_t *)addr, &addrlen);
  return res;
}

int32_t sl_iperf_socket_connect(int32_t sockid, const sl_iperf_socket_addr_t *addr)
{
  return connect(sockid, (const sockaddr_t *)addr, sizeof(wisun_addr_t));
}

int32_t sl_iperf_socket_send(int32_t sockid, const void *buff, size_t len)
{
  return send(sockid, buff, len, 0U);
}

int32_t sl_iperf_socket_sendto(int32_t sockid, const void *buff, uint32_t len, const sl_iperf_socket_addr_t *dest_addr)
{
  return sendto(sockid, buff, len, 0U, (const sockaddr_t *) dest_addr, sizeof(wisun_addr_t));
}

int32_t sl_iperf_socket_recv(int32_t sockid, void *buff, size_t len)
{
  return recv(sockid, buff, len, 0U);
}

int32_t sl_iperf_socket_recvfrom(int32_t sockid, void *buf, uint32_t len,
                                 sl_iperf_socket_addr_t *src_addr)
{
  int32_t res = SL_IPERF_NW_API_ERROR;
  socklen_t addrlen  = sizeof(wisun_addr_t);
  assert(buf != NULL);
  res = recvfrom(sockid, buf, len, 0U, (sockaddr_t *) src_addr, &addrlen);
  return res;
}

int32_t sl_iperf_socket_setsockopt(int32_t sockid, int32_t level, int32_t optname,
                                   const void *optval, size_t optlen)
{
  return setsockopt(sockid, level, optname, optval, optlen);
}

int32_t sl_iperf_socket_getsockopt(int32_t sockid, int32_t level, int32_t optname,
                                   void *optval, size_t *optlen)
{
  return getsockopt(sockid, level, optname, optval, (socklen_t *) optlen);
}

void sl_iperf_set_socket_addr_family(sl_iperf_socket_addr_t * const addr)
{
  wisun_addr_t *waddr = (wisun_addr_t *) addr;
  waddr->sin6_family = AF_WISUN;
}

void sl_iperf_set_socket_addr_port(sl_iperf_socket_addr_t * const addr, const uint16_t port)
{
  wisun_addr_t *waddr = (wisun_addr_t *) addr;
  waddr->sin6_port = port;
}

uint16_t sl_iperf_get_socket_addr_port(const sl_iperf_socket_addr_t * const addr)
{
  const wisun_addr_t *waddr = (const wisun_addr_t *) addr;
  return (uint16_t) waddr->sin6_port;
}

void sl_iperf_set_socket_addr_ip(sl_iperf_socket_addr_t * const addr, const void * const ip)
{
  wisun_addr_t *waddr = (wisun_addr_t *) addr;

  memcpy(&waddr->sin6_addr, ip, sizeof(in6_addr_t));
}

void sl_iperf_get_socket_addr_ip(const sl_iperf_socket_addr_t * const addr, void * const ip)
{
  const wisun_addr_t *waddr = (const wisun_addr_t *) addr;
  memcpy(ip, &waddr->sin6_addr, sizeof(in6_addr_t));
}

bool sl_iperf_network_is_connected(void)
{
  return app_wisun_network_is_connected();
}

uint16_t sl_iperf_network_htons(uint16_t val)
{
  return (uint16_t) sl_iperf_i16_change_byte_order((int16_t)val);
}
uint16_t sl_iperf_network_ntohs(uint16_t val)
{
  return (uint16_t) sl_iperf_i16_change_byte_order((int16_t)val);
}
uint32_t sl_iperf_network_htonl(uint32_t val)
{
  return (uint32_t) sl_iperf_i32_change_byte_order((int32_t)val);
}
uint32_t sl_iperf_network_ntohl(uint32_t val)
{
  return (uint32_t) sl_iperf_i32_change_byte_order((int32_t)val);
}

int32_t sl_iperf_inet_pton(const char *src_str,
                           sl_iperf_socket_addr_t * const dst_addr)
{
  wisun_addr_t * waddr = (wisun_addr_t *) dst_addr;
  return inet_pton(AF_WISUN, src_str, (void *) &waddr->sin6_addr);
}

const char *sl_iperf_inet_ntop(const  sl_iperf_socket_addr_t * const src_addr,
                               char * const dst_str,
                               const size_t size)
{
  const wisun_addr_t * waddr = (const wisun_addr_t *) src_addr;
  return inet_ntop(AF_WISUN, (const void *)&waddr->sin6_addr, dst_str, size);
}

sl_iperf_ts_ms_t sl_iperf_get_timestamp_ms(void)
{
  uint32_t tick_cnt = sl_sleeptimer_get_tick_count();
  sl_iperf_ts_ms_t time_stamp = (sl_iperf_ts_ms_t) sl_sleeptimer_tick_to_ms(tick_cnt);
  return time_stamp;
}

void sl_iperf_delay_ms(const uint32_t ms)
{
  (void) osDelay(ms);
}
// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
