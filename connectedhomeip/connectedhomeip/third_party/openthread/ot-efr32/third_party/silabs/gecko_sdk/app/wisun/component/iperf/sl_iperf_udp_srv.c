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

#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "sl_iperf_network_interface.h"
#include "sl_iperf.h"
#include "sl_iperf_util.h"
#include "sl_iperf_udp_srv.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief iPerf UDP FINACK packet send
 * @details Assembling and sending final ack packate
 * @param[in,out] test Test
 *****************************************************************************/
static void _iperf_udp_finack(sl_iperf_test_t * const test);

/**************************************************************************//**
 * @brief iPerf get timestamp from packet
 * @details get timestamp from packet and convert to milisec
 * @param[in] dtg Datagram header
 * @return sl_iperf_ts_ms_t Converted timestamp in ms
 *****************************************************************************/
static inline sl_iperf_ts_ms_t _get_ms_ts_from_clnt_header(const sl_iperf_udp_clnt_hdr_t
                                                           * const hdr);

/**************************************************************************//**
 * @brief iPerf get absolute delta time
 * @details Helper function for Jitter calculation
 * @param[in] test Test
 * @return uint32_t Delta time value
 *****************************************************************************/
static uint32_t _calc_jitter_abs_delta(const sl_iperf_test_t * const test);

/**************************************************************************//**
 * @brief Jitter calculation
 * @details Calculate interarrival estimated jitter based on RFC 1889
 * @param[in,out] test Test
 *****************************************************************************/
static void _iperf_calc_jitter(sl_iperf_test_t * const test);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

// UDP Server test
void sl_iperf_test_udp_server(sl_iperf_test_t * test)
{
  int32_t r                            = SL_IPERF_NW_API_ERROR;
  sl_iperf_udp_clnt_hdr_t *clnt_hdr    = NULL;
  int32_t pkt_id                       = 0;
  sl_iperf_time_t time                 = { 0U };
  sl_iperf_ts_ms_t test_start          = 0;

  // reset error, set status queued
  sl_iperf_test_set_err_and_stat(test, SL_IPERF_ERR_NONE,
                                 SL_IPERF_TEST_STATUS_QUEUED);
  sl_iperf_test_log_verbose(test, "UDP Server: started.\n");
  test_start = sl_iperf_get_timestamp_ms();
  
  if (test->conn.buff == NULL || !test->conn.buff_size) {
    sl_iperf_test_log_verbose(test, "UDP Server: connection buffer error.\n");
    return;
  }

  test->conn.socket_id  = sl_iperf_socket_create(test->opt.protocol);
  if (test->conn.socket_id  == SL_IPERF_NW_API_ERROR) {
    sl_iperf_test_set_err_and_stat(test, SL_IPERF_ERR_SERVER_SOCK_OPEN, SL_IPERF_TEST_STATUS_ERR);
    sl_iperf_test_log(test, "UDP Server: sl_iperf_socket_create failed.\n");
    return;
  }
  sl_iperf_test_log_verbose(test, "UDP Server: socket created.\n");

  if (!test->opt.port) {
    test->opt.port = SL_IPERF_SERVER_DEFAULT_PORT;
  }
  
  sl_iperf_test_log(test, "Server listening on UDP port %u\n", test->opt.port);
  sl_iperf_test_log(test, "UDP buffer size: %lu bytes\n", test->conn.buff_size);

  sl_iperf_set_socket_addr_family(&test->conn.srv_addr);
  sl_iperf_set_socket_addr_ip(&test->conn.srv_addr, &sl_iperf_socket_inaddr_any);
  sl_iperf_set_socket_addr_port(&test->conn.srv_addr, test->opt.port);

  r = sl_iperf_socket_bind(test->conn.socket_id, &test->conn.srv_addr);
  if (r == SL_IPERF_NW_API_ERROR) {
    sl_iperf_test_set_err_and_stat(test, SL_IPERF_ERR_SERVER_SOCK_BIND, SL_IPERF_TEST_STATUS_ERR);
    sl_iperf_test_log(test, "UDP Server: sl_iperf_socket_bind failed.\n");
    return;
  }
  sl_iperf_test_log_verbose(test, "UDP Server: bind done.\n");

  test->statistic.nbr_rcv_snt_packets = 0U;

  while (sl_iperf_test_check_time(test_start)) {
    sl_iperf_test_update_status(test);
    r = sl_iperf_socket_recvfrom(test->conn.socket_id, test->conn.buff,
                                 test->conn.buff_size, &test->conn.clnt_addr);

    // Empty buff or error
    if (!r || r == SL_IPERF_NW_API_ERROR) {
      sl_iperf_delay_ms(1);
      continue;
    }

    // store previous timestamps
    test->statistic.ts_prev_recv_ms = test->statistic.ts_curr_recv_ms;
    test->statistic.ts_prev_sent_ms = test->statistic.ts_curr_sent_ms;

    // get current time stamp
    test->statistic.ts_curr_recv_ms = sl_iperf_get_timestamp_ms();
    // set next start time
    test_start = test->statistic.ts_curr_recv_ms;

    ++test->statistic.nbr_rcv_snt_packets;
    ++test->statistic.nbr_calls;
    // store currently received packet counts, update resets it
    ++test->statistic.last_recv_pkt_cnt;
    test->statistic.bytes += r;
    if (test->conn.run == false) {
      test->statistic.ts_start_ms = test->statistic.ts_curr_recv_ms;
      test->statistic.ts_end_ms = 0;
      test->conn.run = true;
    }

    clnt_hdr = (sl_iperf_udp_clnt_hdr_t *) test->conn.buff;
#if SL_IPERF_VERBOSE_MODE
    sl_iperf_print_test_clnt_header_json(test, clnt_hdr);
#endif
    // get sent timestamp
    test->statistic.ts_curr_sent_ms = _get_ms_ts_from_clnt_header(clnt_hdr);

    pkt_id = sl_iperf_network_ntohl(clnt_hdr->dtg.id);
    sl_iperf_test_log_verbose(test, "UDP Server: packet received. pkt_id = %d (%d bytes).\n", pkt_id, r);

    _iperf_calc_jitter(test);

    // 1. First paket received
    if (test->statistic.nbr_rcv_snt_packets == 1U && pkt_id == 1) {
      sl_iperf_test_print_udp_conn_str(test);
      sl_iperf_test_print_udp_srv_report_hdr(test);
      sl_iperf_test_log_verbose(test, "UDP Server: first packet received.\n");
      
      if (pkt_id > 1) {
        sl_iperf_test_log_verbose(test, "UDP Server: Packet ID not synchronized (first packet not received).\n");
        test->statistic.udp_async_error = true;
        ++test->statistic.udp_lost_pkt;
      }
      test->statistic.udp_rx_last_pkt = pkt_id;
    }
    // 2. Received old upd fin ack (discard packet)
    else if (test->statistic.nbr_rcv_snt_packets == 1U && pkt_id < 0) {
      sl_iperf_test_log_verbose(test, "UDP Server: Received old upd fin ack (discard packet).\n");
      test->statistic.nbr_rcv_snt_packets = 0U;
    }
    // 3. Received end packet
    else if (pkt_id < 0) {
      sl_iperf_test_log_verbose(test, "UDP Server: Received end packet.\n");
      test->statistic.ts_end_ms = sl_iperf_get_timestamp_ms();
      test->conn.run = false;
      _iperf_udp_finack(test);
      break;
    }
    // 4. Packet out of order
    else if (pkt_id != (test->statistic.udp_rx_last_pkt + 1)
             && test->statistic.nbr_rcv_snt_packets > 1U) {
      if (pkt_id < test->statistic.udp_rx_last_pkt + 1) {
        sl_iperf_test_log_verbose(test, "UDP Server: Packet out of order.\n");
        // Packet out of order
        ++test->statistic.udp_out_of_order;
      } else {
        // Packet lost
        sl_iperf_test_log_verbose(test, "UDP Server: Packet lost.\n");
        test->statistic.udp_lost_pkt += (pkt_id - (test->statistic.udp_rx_last_pkt + 1U));
      }
    }
    test->statistic.udp_rx_last_pkt = pkt_id;
  }

  // calculate total packets
  test->statistic.tot_packets = test->statistic.nbr_rcv_snt_packets + test->statistic.udp_lost_pkt;

  // calculate final band width
  sl_iperf_test_calculate_average_bandwidth(test);

  sl_iperf_test_log_verbose(test, "UDP Server: close socket.\n");
  sl_iperf_calc_time_from_ms(&time, sl_iperf_test_calc_time_duration_ms(test));
  sl_iperf_test_log_verbose(test, "UDP Server: Done (Time: %lu.%.4lus)\n", time.sec, time.usec);

  // close socket
  r = sl_iperf_socket_close(test->conn.socket_id);
  if (r == SL_IPERF_NW_API_ERROR) {
    sl_iperf_test_set_err_and_stat(test, SL_IPERF_ERR_SERVER_SOCK_CLOSE, SL_IPERF_TEST_STATUS_ERR);
  } else {
    sl_iperf_test_set_err_and_stat(test, SL_IPERF_ERR_NONE, SL_IPERF_TEST_STATUS_DONE);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void _iperf_udp_finack(sl_iperf_test_t * const test)
{
  sl_iperf_udp_srv_hdr_t *hdr  = NULL;
  sl_iperf_time_t time        = { 0U };
  int32_t r                   = SL_IPERF_NW_API_ERROR;

  // assert minimum buff size for fin ack
  assert(test->conn.buff_size >= SL_IPERF_UDP_SERVER_FIN_ACK_SIZE);

  if (test->conn.buff == NULL || !test->conn.buff_size) {
    sl_iperf_test_log(test, "UDP Server: error, connection buffer is NULL.\n");
    return;
  }

  hdr = (sl_iperf_udp_srv_hdr_t *) test->conn.buff;

  // reset datagram
  memset(&hdr->dtg, 0, sizeof(sl_iperf_udp_datagram_t));
  hdr->flags            = sl_iperf_network_htonl(SL_IPERF_HEADER_VERSION1);
  hdr->tot_len_u        = 0U;
  hdr->tot_len_l        = sl_iperf_network_htonl(test->statistic.bytes);
  sl_iperf_calc_time_from_ms(&time, sl_iperf_test_calc_time_duration_ms(test));
  hdr->stop_sec         = sl_iperf_network_htonl(time.sec);
  hdr->stop_usec        = sl_iperf_network_htonl(time.usec);
  hdr->lost_pkt_cnt     = sl_iperf_network_htonl(test->statistic.udp_lost_pkt);
  hdr->out_of_order_cnt = sl_iperf_network_htonl(test->statistic.udp_out_of_order);
  hdr->packet_cnt       = sl_iperf_network_htonl(test->statistic.nbr_rcv_snt_packets);
  sl_iperf_calc_time_from_ms(&time, (sl_iperf_ts_ms_t)(test->statistic.udp_jitter));
  hdr->jitter_sec       = sl_iperf_network_htonl(time.sec);  // sec
  hdr->jitter_usec      = sl_iperf_network_htonl(time.usec); // usec

#if SL_IPERF_VERBOSE_MODE
  sl_iperf_print_test_srv_header_json(test, hdr);
#endif

  for (uint32_t i = 0; i < SL_IPERF_SERVER_UDP_TX_FINACK_COUNT; ++i) {
    if (sl_iperf_socket_sendto(test->conn.socket_id,
                               test->conn.buff,
                               SL_IPERF_UDP_SERVER_FIN_ACK_SIZE,
                               &test->conn.clnt_addr) == SL_IPERF_NW_API_ERROR) {
      sl_iperf_test_log_verbose(test, "UDP Server: FINACK send error.\n");
      return;
    }
  }

  sl_iperf_test_log_verbose(test, "UDP Server: Waiting for end of client stream.\n");

  do {
    sl_iperf_delay_ms(3000UL);
    r = sl_iperf_socket_recvfrom(test->conn.socket_id,
                                 test->conn.buff,
                                 test->conn.buff_size,
                                 &test->conn.clnt_addr);
  } while (r > 0L);

  sl_iperf_test_log_verbose(test, "UDP Server: FINACK has been sent.\n");
}

static inline sl_iperf_ts_ms_t _get_ms_ts_from_clnt_header(const sl_iperf_udp_clnt_hdr_t * const hdr)
{
  return sl_iperf_network_ntohl(hdr->dtg.time_var_sec) * SL_IPERF_TIME_S_TO_MS_ML
         + sl_iperf_network_ntohl(hdr->dtg.time_var_usec) / SL_IPERF_TIME_MS_TO_US_ML;
}

static uint32_t _calc_jitter_abs_delta(const sl_iperf_test_t * const test)
{
  uint32_t sender_d   = 0U;
  uint32_t receiver_d = 0U;
  sender_d   = (uint32_t)(test->statistic.ts_curr_sent_ms - test->statistic.ts_prev_sent_ms);
  receiver_d = (uint32_t)(test->statistic.ts_curr_recv_ms - test->statistic.ts_prev_recv_ms);

  return sender_d >= receiver_d ? (sender_d - receiver_d) : (receiver_d - sender_d);
}

static void _iperf_calc_jitter(sl_iperf_test_t * const test)
{
  // Jitter calculation by RFC-1889
  // Estimated jitter type is int64_t
  // Time stamps format is ms
  // D(i,j)=(Rj-Ri)-(Sj-Si)=(Rj-Sj)-(Ri-Si)
  // J=J+(|D(i-1,i)|-J)/16

  uint32_t abs_d = 0;
  int64_t v1 = 0;

  // skip if there isn't previous packet info
  if (test->statistic.nbr_rcv_snt_packets <= 1) {
    return;
  }
  abs_d = _calc_jitter_abs_delta(test);
  v1 = ((int64_t)abs_d - test->statistic.udp_jitter);
  v1 = ((v1 + 8L) >> 4UL);
  test->statistic.udp_jitter += v1;
}
