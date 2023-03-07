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

/// Client FinACK Receive Timeout
#define SL_IPERF_FINACK_RECV_TIMEOUT_MS       10000UL

/// Client TX parameters
typedef struct client_tx_params {
  /// Packet count
  uint16_t packet_count;
  /// Packet size
  uint16_t packet_size;
  /// Delay ms
  uint32_t delay_ms;
} client_tx_params_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * @brief Prepare UDP packet header
 * @details Helper function
 * @param[in,out] test Test
 * @param[in] pkt_id Packet ID
 * @param[in] time Time structure
 *****************************************************************************/
static void _prepare_udp_clnt_header(sl_iperf_test_t * const test, const int32_t pkt_id);

/**************************************************************************//**
 * @brief UDP client calculate TX parameters
 * @details Helper function
 * @param[in,out] test Test
 * @param[in,out] tx_info TX info
 *****************************************************************************/
static void _udp_client_calc_tx(sl_iperf_test_t * const test,
                                client_tx_params_t * const tx_info);

/**************************************************************************//**
 * @brief Prepare Server FinACK
 * @details helper function
 * @param[in,out] test
 *****************************************************************************/
static void _parse_srv_finack(sl_iperf_test_t * const test);

/**************************************************************************//**
 * @brief Get 64bit unsigned integer from High-Low values
 * @details Helper function
 * @param[in] high High 32 bit word
 * @param[in] low Low 32 bit word
 * @return uint64_t 64 bit value
 *****************************************************************************/
static inline uint64_t _get_u64_val_from_hl(const uint32_t high, const uint32_t low);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

// UDP Client test
void sl_iperf_test_udp_client(sl_iperf_test_t * test)
{
  int32_t pkt_id            =   0L;
  client_tx_params_t params = { 0U };
  int32_t r                 = SL_IPERF_NW_API_ERROR;
  bool finack_received      = false;

  if (test == NULL) {
    return;
  }

  // assert minimum buff size for client header
  assert(test->conn.buff_size >= sizeof(sl_iperf_udp_clnt_hdr_t));

  sl_iperf_test_log_verbose(test, "UDP Client: started.\n");
  sl_iperf_test_log_verbose(test, "UDP Client: Remote address is '%s'.\n", test->opt.remote_addr);

  // prepare client pattern
  sl_iperf_test_fill_buffer_with_pattern(test);

  // create socket
  test->conn.socket_id = sl_iperf_socket_create(test->opt.protocol);
  if (test->conn.socket_id  == SL_IPERF_NW_API_ERROR) {
    sl_iperf_test_set_err_and_stat(test, SL_IPERF_ERR_CLIENT_SOCK_OPEN, SL_IPERF_TEST_STATUS_ERR);
    sl_iperf_test_log(test, "UDP Client: sl_iperf_socket_create failed.\n");
    return;
  }
  sl_iperf_test_log_verbose(test, "UDP Client: socket created.\n");

  // set remote address
  sl_iperf_set_socket_addr_family(&test->conn.srv_addr);
  sl_iperf_inet_pton(test->opt.remote_addr, &test->conn.srv_addr);
  sl_iperf_test_log_verbose(test, "UDP Client: Listener port is %u.\n", test->opt.port ? test->opt.port : SL_IPERF_SERVER_DEFAULT_PORT);
  sl_iperf_set_socket_addr_port(&test->conn.srv_addr,
                                test->opt.port ? test->opt.port : SL_IPERF_SERVER_DEFAULT_PORT);

  test->statistic.ts_start_ms = sl_iperf_get_timestamp_ms();

  _udp_client_calc_tx(test, &params);

  sl_iperf_test_print_udp_conn_str(test);
  sl_iperf_test_print_udp_clnt_report_hdr(test);

  while (params.packet_count) {
    sl_iperf_test_update_status(test);

    // Get timestamp
    test->statistic.ts_curr_sent_ms = sl_iperf_get_timestamp_ms();

    ++pkt_id;

    // last packet
    if (params.packet_count == 1U) {
      pkt_id = -pkt_id;
    }

    _prepare_udp_clnt_header(test, pkt_id);

    sl_iperf_test_log_verbose(test, "UDP Client: sending packet_id = %d\n", pkt_id);
    r = sl_iperf_socket_sendto(test->conn.socket_id,
                               test->conn.buff,
                               params.packet_size,
                               &test->conn.srv_addr);

    ++test->statistic.nbr_calls;
    ++test->statistic.nbr_rcv_snt_packets;

    if (r == SL_IPERF_NW_API_ERROR) {
      ++test->statistic.errs;
      ++test->statistic.udp_lost_pkt;
    } else {
      test->statistic.bytes += r;
    }
    params.packet_count--;

    if (params.packet_count) {
      sl_iperf_delay_ms(params.delay_ms);
    }
  }

  test->statistic.ts_end_ms = sl_iperf_get_timestamp_ms();

  for (uint8_t i = 0; i < SL_IPERF_SERVER_UDP_TX_FINACK_COUNT; ++i) {
    r = sl_iperf_socket_recvfrom(test->conn.socket_id,
                                 test->conn.buff,
                                 SL_IPERF_UDP_SERVER_FIN_ACK_SIZE,
                                 &test->conn.srv_addr);
    if (r > 0L) {
      // parse FinACK response
      _parse_srv_finack(test);
      finack_received = true;
      break;
    } else if ( r == SL_IPERF_NW_API_ERROR ) {
      _prepare_udp_clnt_header(test, --pkt_id);
      (void) sl_iperf_socket_sendto(test->conn.socket_id,
                                    test->conn.buff,
                                    params.packet_size,
                                    &test->conn.srv_addr);
    }
    sl_iperf_delay_ms(SL_IPERF_FINACK_RECV_TIMEOUT_MS / SL_IPERF_SERVER_UDP_TX_FINACK_COUNT);
  }

  if (!finack_received) {
    sl_iperf_test_log(test, "FinACK has not been received:\n");
  }

  // calculate total packets
  test->statistic.tot_packets = test->statistic.nbr_rcv_snt_packets + test->statistic.udp_lost_pkt;

  sl_iperf_test_calculate_average_bandwidth(test);

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

static void _prepare_udp_clnt_header(sl_iperf_test_t * const test, const int32_t pkt_id)
{
  sl_iperf_udp_clnt_hdr_t * hdr = NULL;
  uint32_t flags                = 0UL;
  sl_iperf_time_t t             = { 0U };

  hdr = (sl_iperf_udp_clnt_hdr_t * ) test->conn.buff;

  // reset header
  memset(hdr, 0U, sizeof(sl_iperf_udp_clnt_hdr_t));

  hdr->dtg.id            = sl_iperf_network_htonl(pkt_id);
  sl_iperf_calc_time_from_ms(&t, test->statistic.ts_curr_sent_ms);
  hdr->dtg.time_var_sec  = sl_iperf_network_htonl(t.sec);
  hdr->dtg.time_var_usec = sl_iperf_network_htonl(t.usec);
  if (pkt_id < 0) {
    hdr->dtg.id2 = -1L;
  } else {
    hdr->dtg.id2 = 0L;
  }

  flags = (uint32_t)SL_IPERF_HEADER_EXTEND | (uint32_t)SL_IPERF_HEADER_UDPTEST;
  hdr->base.flags       = sl_iperf_network_htonl(flags);
  hdr->base.num_threads = sl_iperf_network_htonl(1U);
  hdr->base.buf_len     = sl_iperf_network_htonl(test->conn.buff_size);
  hdr->base.port        = (int32_t)sl_iperf_network_htons(sl_iperf_get_socket_addr_port(&test->conn.srv_addr));
  hdr->base.amount      = sl_iperf_network_htonl((test->opt.packet_nbr * test->opt.buf_len)
                                                 & (uint32_t) ~SL_IPERF_HEADER_TIME_MODE);

#if SL_IPERF_VERBOSE_MODE
  sl_iperf_print_test_clnt_header_json(test, hdr);
#endif
}

static void _udp_client_calc_tx(sl_iperf_test_t * const test,
                                client_tx_params_t * const tx_info)
{
  uint32_t val = 0;

  // set send time if it's necessary
  if (!test->opt.duration_ms) {
    test->opt.duration_ms = SL_IPERF_CLIENT_DEFAULT_SEND_TIME_MS;
  }

  if (!test->opt.bandwidth) {
    test->opt.bandwidth = SL_IPERF_CLIENT_DEFAULT_BANDWIDTH_BPS;
  }

  // trim buff size if it's necessary
  if (!test->opt.buf_len || test->opt.buf_len > test->conn.buff_size) {
    test->opt.buf_len = test->conn.buff_size;
  }

  tx_info->packet_size = test->opt.buf_len;

  // If packet number is explicitly set
  if (test->opt.packet_nbr) {
    tx_info->packet_count = test->opt.packet_nbr;
    // Calculate expected packet count
    // if bandwidth is explicitly set
  } else {
    val = sl_iperf_bytes_from_formatted_bandwidth(test->opt.bw_format, test->opt.bandwidth);
    val *= (test->opt.duration_ms / SL_IPERF_TIME_S_TO_MS_ML);
    tx_info->packet_count = (uint16_t)(val / tx_info->packet_size);
  }

  val = (uint32_t)tx_info->packet_count - 1;
  if (val) {
    tx_info->delay_ms = test->opt.duration_ms / val;
  } else {
    tx_info->delay_ms = 0LU;
  }

  test->opt.packet_nbr = tx_info->packet_count;
  if (tx_info->delay_ms < 10LU) {
    tx_info->delay_ms = 0;
  }
}

static inline uint64_t _get_u64_val_from_hl(const uint32_t high, const uint32_t low)
{
  return ((uint64_t) sl_iperf_network_ntohl(high) << 32U)
         | (uint64_t) sl_iperf_network_ntohl(low);
}

static void _parse_srv_finack(sl_iperf_test_t * const test)
{
  sl_iperf_udp_srv_hdr_t *hdr = NULL;
  sl_iperf_time_t t           = { 0U };

  if (test->conn.buff == NULL || !test->conn.buff_size) {
    sl_iperf_test_log(test, "UDP Client: error, connection buffer is NULL.\n");
    return;
  }

  hdr = (sl_iperf_udp_srv_hdr_t *) test->conn.buff;
#if SL_IPERF_VERBOSE_MODE
  sl_iperf_print_test_srv_header_json(test, hdr);
#endif
  test->statistic.finack_tot_len = (uint32_t)_get_u64_val_from_hl(hdr->tot_len_u, hdr->tot_len_l);
  t.sec = sl_iperf_network_ntohl(hdr->stop_sec);
  t.usec = sl_iperf_network_ntohl(hdr->stop_usec);
  test->statistic.finack_duration_ms = sl_iperf_calc_ms_from_time(&t);
  test->statistic.finack_pkt = sl_iperf_network_ntohl(hdr->packet_cnt);
  test->statistic.udp_lost_pkt = sl_iperf_network_ntohl(hdr->lost_pkt_cnt);
  test->statistic.udp_out_of_order = sl_iperf_network_ntohl(hdr->out_of_order_cnt);
  t.sec = sl_iperf_network_ntohl(hdr->jitter_sec);
  t.usec = sl_iperf_network_ntohl(hdr->jitter_usec);
  test->statistic.udp_jitter = sl_iperf_calc_ms_from_time(&t);
}
