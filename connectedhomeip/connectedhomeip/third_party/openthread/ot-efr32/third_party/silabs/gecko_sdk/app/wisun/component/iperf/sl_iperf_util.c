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

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "sl_iperf_util.h"
#include "sl_iperf_config.h"
#include "sl_iperf_log.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Define not valid string for outputs
#define SL_IPERF_LOG_NOT_VALID_STR              "N/A"

/// iPerf UDP Server report header
#define SL_IPERF_SERVER_REPORT_HEADER \
  " Interval       Transfer       Bandwidth        Jitter   Lost/Total Datagrams\n"

/// UDP Server statistic format
#define SL_IPERF_UDP_SRV_STATISTIC_FORMAT_STR \
  " %lu.%.2lu-%lu.%.2lu sec  %lu.%u%u%u %-7s  %lu.%u%u%u %-10s %lu ms    %lu/%lu (%u%%)\n"

/// UDP Server statistic format
#define SL_IPERF_UDP_SRV_OUT_OF_ORDER_STAT_FORMAT_STR \
  " %lu.%.2lu-%lu.%.2lu sec  %lu datagrams received out-of-order\n"

/// iPerf UDP Server report header
#define SL_IPERF_CLIENT_REPORT_HEADER \
  " Interval       Transfer       Bandwidth\n"

/// UDP Client statistic format
#define SL_IPERF_UDP_CLNT_STATISTIC_FORMAT_STR \
  " %lu.%.2lu-%lu.%.2lu sec  %lu.%u%u%u %-7s  %lu.%u%u%u %-10s\n"

#define SL_IPERF_UDP_SRV_CONNECTION_FORMAT_STR \
  " local %s port %u connected with %s port %u\n"

///
#define SL_IPERF_UDP_CLNT_CONNECTION_FORMAT_STR \
  " local %s connected with %s port %u\n"

/// Create json indent macro function
#define __indent(__val)                         _calc_ind(__val), ""

/// Integer 32 bit converter union definition
typedef union i32_converter {
  /// Integer value
  int32_t i32val;
  /// Byte values
  uint8_t bytes[4U];
} i32_converter_t;

/// Integer 16 bit converter union definition
typedef union i16_converter {
  /// Integer value
  int16_t i16val;
  /// Byte values
  uint8_t bytes[2U];
} i16_converter_t;

/// Integer representation of floating point numbers
typedef struct ifloat {
  /// Integer part
  uint32_t int_val;
  /// Fractional part digit0
  uint8_t f0_val;
  /// Fractional part digit1
  uint8_t f1_val;
  /// Fractional part digit2
  uint8_t f2_val;
} ifloat_t;

/// Status update parameters/variables
typedef struct stat_update_params {
  /// Delta time
  sl_iperf_ts_ms_t ts_ms_delta;
  /// Current time
  sl_iperf_ts_ms_t ts_ms_cur;
  /// Start time
  sl_iperf_time_t start_time;
  /// End time
  sl_iperf_time_t end_time;
  /// Delta bytes
  size_t data_bytes_delta;
  /// Current bytes
  size_t data_bytes_cur;
  /// Current lost packets
  uint32_t lost_pkt_curr;
  /// Formatted value of bandwidth
  ifloat_t fval_bw;
  /// Formatted= value of data
  ifloat_t fval_data;
  /// Current packet count
  uint32_t pkt_cnt;
  /// Current Out-of-order packet count
  uint32_t out_of_order_pkt_cnt;
} stat_update_params_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Convert bool int to json bool
 * @details helper function
 * @param[in] val Value
 * @return const char* String json value
 *****************************************************************************/
static inline const char * _bool_to_json(const bool val);

/**************************************************************************//**
 * @brief Calculate json indent
 * @details Helper function
 * @param[in] val Indent value
 * @return uint8_t Space count
 *****************************************************************************/
static inline uint8_t _calc_ind(const uint8_t val);

/**************************************************************************//**
 * @brief Calculate formatted bandwidth
 * @details Helper function
 * @param[in] bw_format Bandwidth format/unit
 * @param[in] bytes Bytes
 * @param[in] time_ms Time ms
 * @param[out] dst Destination ptr
 *****************************************************************************/
static void _calculate_formatted_bw(const sl_iperf_opt_bw_format bw_format,
                                    const uint32_t bytes,
                                    const uint32_t time_ms,
                                    ifloat_t * const dst);

/**************************************************************************//**
 * @brief Convert data
 * @details Helper function
 * @param[in] amount Amount of data
 * @param[in] divider Divider, like 1024 or 1000
 * @param[out] dst Destination ptr
 *****************************************************************************/
static void _data_converter(const uint32_t amount, const uint32_t divider, ifloat_t * const dst);

/**************************************************************************//**
 * @brief Print UDP server status report
 * @details Helper function
 * @param[in,out] test Test descriptor
 * @param[in] params Parameters
 *****************************************************************************/
static void _print_udp_srv_status(sl_iperf_test_t * const test,
                                  const stat_update_params_t * const params);

/**************************************************************************//**
 * @brief Print UDP client status report
 * @details Helper function
 * @param[in,out] test Test descriptor
 * @param[in] params Parameters
 *****************************************************************************/
static void _print_udp_clnt_status(sl_iperf_test_t * const test,
                                   const stat_update_params_t * const params);
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void sl_iperf_print_test_log_json(sl_iperf_test_t * const test)
{
  if (!sl_iperf_check_test_logger(test)) {
    return;
  }
  sl_iperf_log_print(test->log, "{\n");
  sl_iperf_log_print(test->log, "%*s\"Test-%lu\": {\n", __indent(1U), test->id);
  sl_iperf_log_print(test->log, "%*s\"id\":     %u,\n", __indent(2U), test->id);
  sl_iperf_log_print(test->log, "%*s\"status\": \"%s\",\n", __indent(2U), sl_iperf_test_status_to_str(test->status));
  sl_iperf_log_print(test->log, "%*s\"error\":  \"%s\",\n", __indent(2U), sl_iperf_test_error_to_str(test->err));
  sl_iperf_log_print(test->log, "%*s\"options\": {\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s\"mode\":        \"%s\",\n", __indent(3U), sl_iperf_opt_mode_to_str(test->opt.mode));
  sl_iperf_log_print(test->log, "%*s\"protocol\":    \"%s\",\n", __indent(3U), sl_iperf_opt_protocol_to_str(test->opt.protocol));
  sl_iperf_log_print(test->log, "%*s\"port\":        %u,\n", __indent(3U), test->opt.port);
  sl_iperf_log_print(test->log, "%*s\"remote_addr\": \"%s\",\n", __indent(3U), test->opt.remote_addr);
  sl_iperf_log_print(test->log, "%*s\"bandwidth\":   %lu,\n", __indent(3U), test->opt.bandwidth);
  sl_iperf_log_print(test->log, "%*s\"packet_nbr\":  %u,\n", __indent(3U), test->opt.packet_nbr);
  sl_iperf_log_print(test->log, "%*s\"buf_len\":     %u,\n", __indent(3U), test->opt.buf_len);
  sl_iperf_log_print(test->log, "%*s\"duration_ms\": %u,\n", __indent(3U), test->opt.duration_ms);
  sl_iperf_log_print(test->log, "%*s\"win_size\":    %u,\n", __indent(3U), test->opt.win_size);
  sl_iperf_log_print(test->log, "%*s\"persistent\":  %s,\n", __indent(3U), _bool_to_json(test->opt.persistent));
  sl_iperf_log_print(test->log, "%*s\"interval_ms\": %u,\n", __indent(3U), test->opt.interval_ms);
  sl_iperf_log_print(test->log, "%*s\"bw_format\": \"%s\"\n", __indent(3U), sl_iperf_opt_bw_format_to_str(test->opt.bw_format));
  sl_iperf_log_print(test->log, "%*s},\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s\"statistic\": {\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s\"nbr_calls\":             %lu,\n", __indent(3U), test->statistic.nbr_calls);
  sl_iperf_log_print(test->log, "%*s\"bytes\":                 %lu,\n", __indent(3U), test->statistic.bytes);
  sl_iperf_log_print(test->log, "%*s\"tot_packets\":           %lu,\n", __indent(3U), test->statistic.tot_packets);
  sl_iperf_log_print(test->log, "%*s\"nbr_rcv_snt_packets\":   %lu,\n", __indent(3U), test->statistic.nbr_rcv_snt_packets);
  sl_iperf_log_print(test->log, "%*s\"errs\":                  %lu,\n", __indent(3U), test->statistic.errs);
  sl_iperf_log_print(test->log, "%*s\"transitory_error_cnts\": %lu,\n", __indent(3U), test->statistic.transitory_error_cnts);
  sl_iperf_log_print(test->log, "%*s\"last_recv_pkt_cnt\":     %lu,\n", __indent(3U), test->statistic.last_recv_pkt_cnt);
  sl_iperf_log_print(test->log, "%*s\"ts_curr_recv_ms\":       %lu,\n", __indent(3U), test->statistic.ts_curr_recv_ms);
  sl_iperf_log_print(test->log, "%*s\"ts_prev_recv_ms\":       %lu,\n", __indent(3U), test->statistic.ts_prev_recv_ms);
  sl_iperf_log_print(test->log, "%*s\"ts_curr_sent_ms\":       %lu,\n", __indent(3U), test->statistic.ts_curr_sent_ms);
  sl_iperf_log_print(test->log, "%*s\"ts_prev_sent_ms\":       %lu,\n", __indent(3U), test->statistic.ts_prev_sent_ms);
  sl_iperf_log_print(test->log, "%*s\"udp_jitter\":            %lld,\n", __indent(3U), test->statistic.udp_jitter);
  sl_iperf_log_print(test->log, "%*s\"udp_rx_last_pkt\":       %ld,\n", __indent(3U), test->statistic.udp_rx_last_pkt);
  sl_iperf_log_print(test->log, "%*s\"udp_lost_pkt\":          %lu,\n", __indent(3U), test->statistic.udp_lost_pkt);
  sl_iperf_log_print(test->log, "%*s\"udp_out_of_order\":      %lu,\n", __indent(3U), test->statistic.udp_out_of_order);
  sl_iperf_log_print(test->log, "%*s\"udp_dup_pkt\":           %lu,\n", __indent(3U), test->statistic.udp_dup_pkt);
  sl_iperf_log_print(test->log, "%*s\"udp_async_error\":       %s,\n", __indent(3U), _bool_to_json(test->statistic.udp_async_error));
  sl_iperf_log_print(test->log, "%*s\"end_err\":               %s,\n", __indent(3U), _bool_to_json(test->statistic.end_err));
  sl_iperf_log_print(test->log, "%*s\"ts_start_ms\":           %lu,\n", __indent(3U), test->statistic.ts_start_ms);
  sl_iperf_log_print(test->log, "%*s\"ts_end_ms\":             %lu,\n", __indent(3U), test->statistic.ts_end_ms);
  sl_iperf_log_print(test->log, "%*s\"bandwidth\":             %lu,\n", __indent(3U), test->statistic.bandwidth);
  sl_iperf_log_print(test->log, "%*s\"finack_tot_len\":        %lu,\n", __indent(3U), test->statistic.finack_tot_len);
  sl_iperf_log_print(test->log, "%*s\"finack_duration_ms\":    %lu,\n", __indent(3U), test->statistic.finack_duration_ms);
  sl_iperf_log_print(test->log, "%*s\"finack_pkt\":            %lu\n", __indent(3U), test->statistic.finack_pkt);
  sl_iperf_log_print(test->log, "%*s}\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s}\n", __indent(1U));
  sl_iperf_log_print(test->log, "}\n");
}

const char * sl_iperf_test_status_to_str(const sl_iperf_status_t status)
{
  switch (status) {
    case SL_IPERF_TEST_STATUS_FREE:      return "FREE";
    case SL_IPERF_TEST_STATUS_QUEUED:    return "QUEUED";
    case SL_IPERF_TEST_STATUS_RUNNING:   return "RUNNING";
    case SL_IPERF_TEST_STATUS_DONE:      return "DONE";
    case SL_IPERF_TEST_STATUS_ERR:       return "ERROR";
    default:                             return SL_IPERF_LOG_NOT_VALID_STR;
  }
}

const char * sl_iperf_test_error_to_str(const sl_iperf_error_t error)
{
  switch (error) {
    case SL_IPERF_ERR_NONE:                   return "NONE";
    case SL_IPERF_ERR_NETWORK_CONNECTION:     return "NETWORK_CONNECTION";
    case SL_IPERF_ERR_SERVER_SOCK_BIND:       return "SERVER_SOCK_BIND";
    case SL_IPERF_ERR_SERVER_SOCK_OPEN:       return "SERVER_SOCK_OPEN";
    case SL_IPERF_ERR_SERVER_SOCK_CLOSE:      return "SERVER_SOCK_CLOSE";
    case SL_IPERF_ERR_SERVER_SOCK_LISTEN:     return "SERVER_SOCK_LISTEN";
    case SL_IPERF_ERR_SERVER_SOCK_ACCEPT:     return "SERVER_SOCK_ACCEPT";
    case SL_IPERF_ERR_SERVER_SOCK_RX:         return "SERVER_SOCK_RX";
    case SL_IPERF_ERR_SERVER_SOCK_WIN_SIZE:   return "SERVER_SOCK_WIN_SIZE";
    case SL_IPERF_ERR_CLIENT_SOCK_OPEN:       return "CLIENT_SOCK_OPEN";
    case SL_IPERF_ERR_CLIENT_SOCK_BIND:       return "CLIENT_SOCK_BIND";
    case SL_IPERF_ERR_CLIENT_SOCK_CONN:       return "CLIENT_SOCK_CONN";
    case SL_IPERF_ERR_CLIENT_SOCK_TX:         return "CLIENT_SOCK_TX";
    case SL_IPERF_ERR_CLIENT_SOCK_TX_INV_ARG: return "CLIENT_SOCK_TX_INV_ARG";
    case SL_IPERF_ERR_CLIENT_SOCK_CLOSE:      return "CLIENT_SOCK_CLOSE";
    default:                                  return SL_IPERF_LOG_NOT_VALID_STR;
  }
}

const char * sl_iperf_opt_mode_to_str(const sl_iperf_mode_t mode)
{
  switch (mode) {
    case SL_IPERF_MODE_SERVER:   return "SERVER";
    case SL_IPERF_MODE_CLIENT:   return "CLIENT";
    default:                     return SL_IPERF_LOG_NOT_VALID_STR;
  }
}

const char * sl_iperf_opt_protocol_to_str(const sl_iperf_protocol_t protocol)
{
  switch (protocol) {
    case SL_IPERF_IPROTOV6_UDP:  return "IPV6_UDP";
    case SL_IPERF_IPROTOV6_TCP:  return "IPV6_TCP";
    case SL_IPERF_IPROTOV4_UDP:  return "IPV4_UDP";
    case SL_IPERF_IPROTOV4_TCP:  return "IPV4_TCP";
    default:                     return SL_IPERF_LOG_NOT_VALID_STR;
  }
}

int32_t sl_iperf_i32_change_byte_order(const int32_t i32val)
{
  i32_converter_t conv = { .i32val = i32val };

  return (  (int32_t) conv.bytes[3U] << 0U)
         | ((int32_t) conv.bytes[2U] << 8U)
         | ((int32_t) conv.bytes[1U] << 16U)
         | ((int32_t) conv.bytes[0U] << 24U);
}

int16_t sl_iperf_i16_change_byte_order(const int16_t i16val)
{
  i16_converter_t conv = { .i16val = i16val };
  return ((int16_t) conv.bytes[1] << 0) | ((int16_t) conv.bytes[0] << 8);
}

#if SL_IPERF_VERBOSE_MODE

void sl_iper_test_dump_buff(sl_iperf_test_t * const test, const size_t size)
{
  uint8_t *buff    = NULL;
  size_t buff_size = 0U;
  uint8_t cnt      = 0U;

  if (!sl_iperf_check_test_logger(test) || !size) {
    return;
  }

  buff = test->conn.buff;
  buff_size = size > test->conn.buff_size ? test->conn.buff_size : size;

  sl_iperf_log_print(test->log, "--- Test-%lu buffer: %lu bytes ---\n", test->id, buff_size);
  for (size_t i = 0U; i < buff_size; ++i) {
    sl_iperf_log_print(test->log, "0x%.2x%s", buff[i], i == (buff_size - 1) ? "" : ", ");
    if (cnt == 9U) {
      printf("\n");
      cnt = 0U;
    } else {
      cnt++;
    }
  }
  sl_iperf_log_print(test->log, "\n");
  sl_iperf_log_print(test->log, "--- End buffer ---\n");
}

void sl_iperf_print_test_srv_header_json(sl_iperf_test_t * const test, const sl_iperf_udp_srv_hdr_t * const hdr)
{
  if (!sl_iperf_check_test_logger(test) || hdr == NULL) {
    return;
  }
  sl_iperf_log_print(test->log, "{\n");
  sl_iperf_log_print(test->log, "%*s\"Test-%lu-Srv\": {\n", __indent(1U), test->id);
  sl_iperf_log_print(test->log, "%*s\"dtg\": {\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s\"id\":            %ld,\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->dtg.id));
  sl_iperf_log_print(test->log, "%*s\"time_var_sec\":  %lu,\n", __indent(3U), sl_iperf_network_ntohl(hdr->dtg.time_var_sec));
  sl_iperf_log_print(test->log, "%*s\"time_var_usec\": %lu,\n", __indent(3U), sl_iperf_network_ntohl(hdr->dtg.time_var_usec));
  sl_iperf_log_print(test->log, "%*s\"id2\":           %ld\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->dtg.id2));
  sl_iperf_log_print(test->log, "%*s},\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s\"flags\":            %ld,\n", __indent(2U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->flags));
  sl_iperf_log_print(test->log, "%*s\"tot_len_u\":        %lu,\n", __indent(2U), sl_iperf_network_ntohl(hdr->tot_len_u));
  sl_iperf_log_print(test->log, "%*s\"tot_len_l\":        %lu,\n", __indent(2U), sl_iperf_network_ntohl(hdr->tot_len_l));
  sl_iperf_log_print(test->log, "%*s\"stop_sec\":         %lu,\n", __indent(2U), sl_iperf_network_ntohl(hdr->stop_sec));
  sl_iperf_log_print(test->log, "%*s\"stop_usec\":        %lu,\n", __indent(2U), sl_iperf_network_ntohl(hdr->stop_usec));
  sl_iperf_log_print(test->log, "%*s\"lost_pkt_cnt\":     %lu,\n", __indent(2U), sl_iperf_network_ntohl(hdr->lost_pkt_cnt));
  sl_iperf_log_print(test->log, "%*s\"out_of_order_cnt\": %lu,\n", __indent(2U), sl_iperf_network_ntohl(hdr->out_of_order_cnt));
  sl_iperf_log_print(test->log, "%*s\"packet_cnt\":       %lu,\n", __indent(2U), sl_iperf_network_ntohl(hdr->packet_cnt));
  sl_iperf_log_print(test->log, "%*s\"jitter_sec\":       %lu,\n", __indent(2U), sl_iperf_network_ntohl(hdr->jitter_sec));
  sl_iperf_log_print(test->log, "%*s\"jitter_usec\":      %lu\n", __indent(2U), sl_iperf_network_ntohl(hdr->jitter_usec));
  sl_iperf_log_print(test->log, "%*s}\n", __indent(1U));
  sl_iperf_log_print(test->log, "}\n");
}

void sl_iperf_print_test_clnt_header_json(sl_iperf_test_t * const test, const sl_iperf_udp_clnt_hdr_t * const hdr)
{
  if (!sl_iperf_check_test_logger(test) || hdr == NULL) {
    return;
  }
  sl_iperf_log_print(test->log, "{\n");
  sl_iperf_log_print(test->log, "%*s\"Test-%lu-Clnt\": {\n", __indent(1U), test->id);
  sl_iperf_log_print(test->log, "%*s\"dtg\": {\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s\"id\":            %ld,\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->dtg.id));
  sl_iperf_log_print(test->log, "%*s\"time_var_sec\":  %lu,\n", __indent(3U), sl_iperf_network_ntohl(hdr->dtg.time_var_sec));
  sl_iperf_log_print(test->log, "%*s\"time_var_usec\": %lu,\n", __indent(3U), sl_iperf_network_ntohl(hdr->dtg.time_var_usec));
  sl_iperf_log_print(test->log, "%*s\"id2\":           %ld\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->dtg.id2));
  sl_iperf_log_print(test->log, "%*s},\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s\"base\": {\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s\"flags\":       %ld,\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->base.flags));
  sl_iperf_log_print(test->log, "%*s\"num_threads\": %ld,\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->base.num_threads));
  sl_iperf_log_print(test->log, "%*s\"port\":        %ld,\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->base.port));
  sl_iperf_log_print(test->log, "%*s\"buf_len\":     %ld,\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->base.buf_len));
  sl_iperf_log_print(test->log, "%*s\"win_band\":    %ld,\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->base.win_band));
  sl_iperf_log_print(test->log, "%*s\"amount\":      %ld\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->base.amount));
  sl_iperf_log_print(test->log, "%*s},\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s\"extend\": {\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s\"type\":               %ld,\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->extend.type));
  sl_iperf_log_print(test->log, "%*s\"length\":             %ld,\n", __indent(3U), (int32_t) sl_iperf_network_ntohl((uint32_t)hdr->extend.length));
  sl_iperf_log_print(test->log, "%*s\"u_flags\":            %d,\n", __indent(3U), (int16_t) sl_iperf_network_ntohs((uint16_t)hdr->extend.u_flags));
  sl_iperf_log_print(test->log, "%*s\"l_flags\":            %d,\n", __indent(3U), (int16_t) sl_iperf_network_ntohs((uint16_t)hdr->extend.l_flags));
  sl_iperf_log_print(test->log, "%*s\"u_version\":          %lu,\n", __indent(3U), sl_iperf_network_ntohl(hdr->extend.u_version));
  sl_iperf_log_print(test->log, "%*s\"l_version\":          %lu,\n", __indent(3U), sl_iperf_network_ntohl(hdr->extend.l_version));
  sl_iperf_log_print(test->log, "%*s\"tos\":                %d,\n", __indent(3U), (int16_t) sl_iperf_network_ntohs((uint16_t)hdr->extend.tos));
  sl_iperf_log_print(test->log, "%*s\"l_rate\":             %lu,\n", __indent(3U), sl_iperf_network_ntohl(hdr->extend.l_rate));
  sl_iperf_log_print(test->log, "%*s\"u_rate\":             %lu,\n", __indent(3U), sl_iperf_network_ntohl(hdr->extend.u_rate));
  sl_iperf_log_print(test->log, "%*s\"tcp_write_prefetch\": %lu\n", __indent(3U), sl_iperf_network_ntohl(hdr->extend.tcp_write_prefetch));
  sl_iperf_log_print(test->log, "%*s}\n", __indent(2U));
  sl_iperf_log_print(test->log, "%*s}\n", __indent(1U));
  sl_iperf_log_print(test->log, "}\n");
}
#endif

const char * sl_iperf_opt_bw_format_to_str(const sl_iperf_opt_bw_format format)
{
  switch (format) {
    case SL_IPERF_OPT_BW_FORMAT_BITS_PER_SEC:   return "bits/s";
    case SL_IPERF_OPT_BW_FORMAT_KBITS_PER_SEC:  return "Kbits/s";
    case SL_IPERF_OPT_BW_FORMAT_MBITS_PER_SEC:  return "Mbits/s";
    case SL_IPERF_OPT_BW_FORMAT_GBITS_PER_SEC:  return "Gbits/s";
    case SL_IPERF_OPT_BW_FORMAT_BYTES_PER_SEC:  return "bytes/s";
    case SL_IPERF_OPT_BW_FORMAT_KBYTES_PER_SEC: return "Kbytes/s";
    case SL_IPERF_OPT_BW_FORMAT_MBYTES_PER_SEC: return "Mbytes/s";
    case SL_IPERF_OPT_BW_FORMAT_GBYTES_PER_SEC: return "Gbytes/s";
    default:                                    return SL_IPERF_LOG_NOT_VALID_STR;
  }
}

uint32_t sl_iperf_formatted_bandwidth_from_bytes(const sl_iperf_opt_bw_format format, const uint32_t amount_bytes)
{
  switch (format) {
    case SL_IPERF_OPT_BW_FORMAT_BITS_PER_SEC:
      return amount_bytes * SL_IPERF_DATA_BYTE_TO_BIT_ML;
    case SL_IPERF_OPT_BW_FORMAT_KBITS_PER_SEC:
      return amount_bytes * SL_IPERF_DATA_BYTE_TO_BIT_ML / SL_IPERF_DATA_KBIT_TO_BIT_ML;
    case SL_IPERF_OPT_BW_FORMAT_MBITS_PER_SEC:
      return amount_bytes * SL_IPERF_DATA_BYTE_TO_BIT_ML / SL_IPERF_DATA_MBIT_TO_BIT_ML;
    case SL_IPERF_OPT_BW_FORMAT_GBITS_PER_SEC:
      return amount_bytes * SL_IPERF_DATA_BYTE_TO_BIT_ML / SL_IPERF_DATA_GBIT_TO_BIT_ML;
    case SL_IPERF_OPT_BW_FORMAT_BYTES_PER_SEC:
      return amount_bytes;
    case SL_IPERF_OPT_BW_FORMAT_KBYTES_PER_SEC:
      return amount_bytes / SL_IPERF_DATA_KBYTE_TO_BYTE_ML;
    case SL_IPERF_OPT_BW_FORMAT_MBYTES_PER_SEC:
      return amount_bytes / SL_IPERF_DATA_MBYTE_TO_BYTE_ML;
    case SL_IPERF_OPT_BW_FORMAT_GBYTES_PER_SEC:
      return amount_bytes / SL_IPERF_DATA_GBYTE_TO_BYTE_ML;
    default:
      return 0LU;
  }
}

#if 1
uint32_t sl_iperf_bytes_from_formatted_bandwidth(const sl_iperf_opt_bw_format format, const uint32_t amount_bandwidth)
{
  switch (format) {
    case SL_IPERF_OPT_BW_FORMAT_BITS_PER_SEC:
      return amount_bandwidth / SL_IPERF_DATA_BYTE_TO_BIT_ML;
    case SL_IPERF_OPT_BW_FORMAT_KBITS_PER_SEC:
      return amount_bandwidth / SL_IPERF_DATA_BYTE_TO_BIT_ML * SL_IPERF_DATA_KBIT_TO_BIT_ML;
    case SL_IPERF_OPT_BW_FORMAT_MBITS_PER_SEC:
      return amount_bandwidth / SL_IPERF_DATA_BYTE_TO_BIT_ML * SL_IPERF_DATA_MBIT_TO_BIT_ML;
    case SL_IPERF_OPT_BW_FORMAT_GBITS_PER_SEC:
      return amount_bandwidth / SL_IPERF_DATA_BYTE_TO_BIT_ML * SL_IPERF_DATA_GBIT_TO_BIT_ML;
    case SL_IPERF_OPT_BW_FORMAT_BYTES_PER_SEC:
      return amount_bandwidth;
    case SL_IPERF_OPT_BW_FORMAT_KBYTES_PER_SEC:
      return amount_bandwidth * SL_IPERF_DATA_KBYTE_TO_BYTE_ML;
    case SL_IPERF_OPT_BW_FORMAT_MBYTES_PER_SEC:
      return amount_bandwidth * SL_IPERF_DATA_MBYTE_TO_BYTE_ML;
    case SL_IPERF_OPT_BW_FORMAT_GBYTES_PER_SEC:
      return amount_bandwidth * SL_IPERF_DATA_GBYTE_TO_BYTE_ML;
    default:
      return 0LU;
  }
}
#endif

void sl_iperf_test_fill_buffer_with_pattern(sl_iperf_test_t * const test)
{
  char patt = '0';

  if (test == NULL || test->conn.buff == NULL) {
    return;
  }

  for (size_t i = 0U; i < test->conn.buff_size; ++i) {
    if (patt > '9') {
      patt = '0';
    }
    test->conn.buff[i] = patt++;
  }
}

void sl_iperf_test_print_udp_conn_str(sl_iperf_test_t * const test)
{
  static char clnt_ip_str[SL_IPERF_IP_STR_BUFF_LEN] = { 0U };
  static char srv_ip_str[SL_IPERF_IP_STR_BUFF_LEN]  = { 0U };

  sl_iperf_inet_ntop(&test->conn.clnt_addr, clnt_ip_str, sizeof(clnt_ip_str));
  sl_iperf_inet_ntop(&test->conn.srv_addr, srv_ip_str, sizeof(srv_ip_str));
  if (test->opt.mode == SL_IPERF_MODE_SERVER) {
    sl_iperf_test_log(test, SL_IPERF_UDP_SRV_CONNECTION_FORMAT_STR,
                      srv_ip_str,
                      sl_iperf_get_socket_addr_port(&test->conn.srv_addr),
                      clnt_ip_str,
                      sl_iperf_get_socket_addr_port(&test->conn.clnt_addr));
  } else {
    sl_iperf_test_log(test, SL_IPERF_UDP_CLNT_CONNECTION_FORMAT_STR,
                      clnt_ip_str,
                      srv_ip_str,
                      sl_iperf_get_socket_addr_port(&test->conn.srv_addr));
  }
}

void sl_iperf_test_update_status(sl_iperf_test_t * const test)
{
  static sl_iperf_ts_ms_t ts_ms_prev     =   0UL;
  static size_t data_bytes_prev          =   0UL;
  static sl_iperf_ts_ms_t update_time_ms =   0UL;
  static uint32_t lost_pkt_prev          =   0UL;
  static uint32_t out_of_order_pkt_prev  =   0UL;
  stat_update_params_t params            = { 0UL };

  // init previous status if test started
  if (test->statistic.nbr_rcv_snt_packets <= 1U) {
    ts_ms_prev = test->opt.mode == SL_IPERF_MODE_SERVER ? test->statistic.ts_curr_recv_ms : test->statistic.ts_curr_sent_ms;
    data_bytes_prev = test->statistic.bytes;
    update_time_ms = 0U;
    lost_pkt_prev = 0U;
    out_of_order_pkt_prev = 0UL;
    return;
  }

  // update if interval is set
  if (!test->opt.interval_ms) {
    return;
  }

  // Elapsed time calculation
  if (test->statistic.ts_end_ms) {
    ts_ms_prev = test->statistic.ts_start_ms;
    params.ts_ms_cur = test->statistic.ts_end_ms;
    params.data_bytes_delta = test->statistic.bytes;
  } else {
    params.data_bytes_cur = test->statistic.bytes;
    params.data_bytes_delta = params.data_bytes_cur - data_bytes_prev;
    params.ts_ms_cur = sl_iperf_get_timestamp_ms();
  }

  params.ts_ms_delta = (sl_iperf_ts_ms_t) (params.ts_ms_cur - ts_ms_prev);
  // Prevent division by zero
  if (!params.ts_ms_delta) {
    return;
  }

  if (params.ts_ms_delta >= test->opt.interval_ms) {
    ts_ms_prev = params.ts_ms_cur;
    data_bytes_prev = params.data_bytes_cur;
    sl_iperf_calc_time_from_ms(&params.start_time, update_time_ms);
    update_time_ms += params.ts_ms_delta;
    sl_iperf_calc_time_from_ms(&params.end_time, update_time_ms);

    _calculate_formatted_bw(test->opt.bw_format, params.data_bytes_delta, params.ts_ms_delta, &params.fval_bw);
    _data_converter(params.data_bytes_delta, SL_IPERF_DATA_KBYTE_TO_BYTE_ML, &params.fval_data);

    if (sl_iperf_test_is_udp_srv(test)) {
      params.out_of_order_pkt_cnt = test->statistic.udp_out_of_order - out_of_order_pkt_prev;
      out_of_order_pkt_prev = test->statistic.udp_out_of_order;
      params.lost_pkt_curr = test->statistic.udp_lost_pkt - lost_pkt_prev;
      lost_pkt_prev = test->statistic.udp_lost_pkt;
      params.pkt_cnt = test->statistic.last_recv_pkt_cnt + params.lost_pkt_curr;
      // reset when update it 
      test->statistic.last_recv_pkt_cnt = 0UL;
      
      _print_udp_srv_status(test, &params);
    } else if (sl_iperf_test_is_udp_clnt(test)) {
      _print_udp_clnt_status(test, &params);
    } else if (sl_iperf_test_is_tcp_srv(test)) {
      (void) 0U;
    } else if (sl_iperf_test_is_tcp_clnt(test)) {
      (void) 0U;
    }
  }
}

void sl_iperf_test_print_udp_srv_report_hdr(sl_iperf_test_t * const test)
{
  sl_iperf_test_log(test, SL_IPERF_SERVER_REPORT_HEADER);
}

void sl_iperf_test_print_udp_clnt_report_hdr(sl_iperf_test_t * const test)
{
  sl_iperf_test_log(test, SL_IPERF_CLIENT_REPORT_HEADER);
}

void sl_iperf_test_calculate_average_bandwidth(sl_iperf_test_t * const test)
{
  stat_update_params_t params       = { 0UL };
  sl_iperf_ts_ms_t time_duration_ms =   0UL;

  time_duration_ms = sl_iperf_test_calc_time_duration_ms(test);

  _calculate_formatted_bw(test->opt.bw_format,
                          test->statistic.bytes,
                          time_duration_ms,
                          &params.fval_bw);

  _data_converter(test->statistic.bytes,
                  SL_IPERF_DATA_KBYTE_TO_BYTE_ML,
                  &params.fval_data);
  if (time_duration_ms) {
    // calculate bandwidth for statistic in bits/sec
    test->statistic.bandwidth = (uint32_t)(((uint64_t)test->statistic.bytes * SL_IPERF_DATA_BYTE_TO_BIT_ML * 
                              SL_IPERF_TIME_S_TO_MS_ML) / time_duration_ms);
  } else {
    test->statistic.bandwidth = 0UL;
  }
  
  sl_iperf_calc_time_from_ms(&params.end_time, time_duration_ms);
  params.pkt_cnt = test->statistic.tot_packets;
  params.lost_pkt_curr = test->statistic.udp_lost_pkt;
  params.start_time.sec  = 0UL;
  params.start_time.usec = 0UL;

  // sl_iperf_test_log(test, "  ---\n");
  if (sl_iperf_test_is_udp_srv(test)) {
    _print_udp_srv_status(test, &params);
  } else if (sl_iperf_test_is_udp_clnt(test)) {
    // print client report
    _print_udp_clnt_status(test, &params);
    sl_iperf_test_log(test, " Sent %lu datagrams\n", test->statistic.nbr_rcv_snt_packets);

    // print server report
    sl_iperf_test_log(test, " Server Report:\n");
    _calculate_formatted_bw(test->opt.bw_format,
                            test->statistic.finack_tot_len,
                            test->statistic.finack_duration_ms,
                            &params.fval_bw);

    _data_converter(test->statistic.finack_tot_len,
                    SL_IPERF_DATA_KBYTE_TO_BYTE_ML,
                    &params.fval_data);

    sl_iperf_calc_time_from_ms(&params.end_time, test->statistic.finack_duration_ms);
    params.pkt_cnt = test->statistic.finack_pkt;
    params.out_of_order_pkt_cnt = test->statistic.udp_out_of_order;
    sl_iperf_test_print_udp_srv_report_hdr(test);
    _print_udp_srv_status(test, &params);
  } else if (sl_iperf_test_is_tcp_srv(test)) {
    (void) 0U;
  } else if (sl_iperf_test_is_tcp_clnt(test)) {
    (void) 0U;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static inline const char *_bool_to_json(const bool val)
{
  return val ? "true" : "false";
}

static void _data_converter(const uint32_t amount, const uint32_t divider, ifloat_t * const dst)
{
  uint32_t rem = 0UL;

  if (!divider) {
    return;
  }

  // 3 points precision
  dst->int_val = amount / divider;
  rem = amount - dst->int_val * divider;
  dst->f0_val = ((rem * 10) / divider) % 10;
  dst->f1_val = ((rem * 100) / divider) % 10;
  dst->f2_val = ((rem * 1000) / divider) % 10;
}

static void _calculate_formatted_bw(const sl_iperf_opt_bw_format bw_format,
                                    const uint32_t bytes,
                                    const uint32_t time_ms,
                                    ifloat_t * const res)
{
  uint32_t byteps_val = 0UL;
  uint32_t bitps_val  = 0UL;

  // reset res
  memset(res, 0U, sizeof(ifloat_t));
  
  if (!time_ms) {
    return;
  }

  byteps_val = (bytes * SL_IPERF_TIME_S_TO_MS_ML) / time_ms;
  bitps_val = (byteps_val * SL_IPERF_DATA_BYTE_TO_BIT_ML);

  switch (bw_format) {
    case SL_IPERF_OPT_BW_FORMAT_BITS_PER_SEC:
      res->int_val = bitps_val;
      res->f0_val = 0UL;
      res->f1_val = 0UL;
      res->f2_val = 0UL;
      break;

    case SL_IPERF_OPT_BW_FORMAT_KBITS_PER_SEC:
      _data_converter(bitps_val, SL_IPERF_DATA_KBIT_TO_BIT_ML, res);
      break;

    case SL_IPERF_OPT_BW_FORMAT_MBITS_PER_SEC:
      _data_converter(bitps_val, SL_IPERF_DATA_MBIT_TO_BIT_ML, res);
      break;

    case SL_IPERF_OPT_BW_FORMAT_GBITS_PER_SEC:
      _data_converter(bitps_val, SL_IPERF_DATA_GBIT_TO_BIT_ML, res);
      break;

    case SL_IPERF_OPT_BW_FORMAT_BYTES_PER_SEC:
      res->int_val = byteps_val;
      res->f0_val = 0UL;
      res->f1_val = 0UL;
      res->f2_val = 0UL;
      break;
    case SL_IPERF_OPT_BW_FORMAT_KBYTES_PER_SEC:
      _data_converter(byteps_val, SL_IPERF_DATA_KBYTE_TO_BYTE_ML, res);
      break;

    case SL_IPERF_OPT_BW_FORMAT_MBYTES_PER_SEC:
      _data_converter(byteps_val, SL_IPERF_DATA_MBYTE_TO_BYTE_ML, res);
      break;

    case SL_IPERF_OPT_BW_FORMAT_GBYTES_PER_SEC:
      _data_converter(byteps_val, SL_IPERF_DATA_KBYTE_TO_BYTE_ML, res);
      break;

    default:
      break;
  }
}

static inline uint8_t _calc_ind(const uint8_t val)
{
  return (uint8_t)(val * SL_IPERF_LOG_JSON_INDENT);
}

bool sl_iperf_test_check_time(const sl_iperf_ts_ms_t test_start_ts)
{
#if (0U < SL_IPERF_MAX_TEST_TIMEOUT_MS)
  return (bool)((sl_iperf_get_timestamp_ms() - test_start_ts) < SL_IPERF_MAX_TEST_TIMEOUT_MS);
#else
  return true;
#endif
}

bool sl_iperf_opt_bw_format_from_str(const char *str, sl_iperf_opt_bw_format * const bw_format)
{
  if (!strncmp(str, sl_iperf_opt_bw_format_to_str(SL_IPERF_OPT_BW_FORMAT_BITS_PER_SEC),
               SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *bw_format = SL_IPERF_OPT_BW_FORMAT_BITS_PER_SEC;
  } else if (!strncmp(str, sl_iperf_opt_bw_format_to_str(SL_IPERF_OPT_BW_FORMAT_KBITS_PER_SEC),
                      SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *bw_format = SL_IPERF_OPT_BW_FORMAT_KBITS_PER_SEC;
  } else if (!strncmp(str, sl_iperf_opt_bw_format_to_str(SL_IPERF_OPT_BW_FORMAT_MBITS_PER_SEC),
                      SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *bw_format = SL_IPERF_OPT_BW_FORMAT_MBITS_PER_SEC;
  } else if (!strncmp(str, sl_iperf_opt_bw_format_to_str(SL_IPERF_OPT_BW_FORMAT_GBITS_PER_SEC),
                      SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *bw_format = SL_IPERF_OPT_BW_FORMAT_GBITS_PER_SEC;
  } else if (!strncmp(str, sl_iperf_opt_bw_format_to_str(SL_IPERF_OPT_BW_FORMAT_BYTES_PER_SEC),
                      SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *bw_format = SL_IPERF_OPT_BW_FORMAT_BYTES_PER_SEC;
  } else if (!strncmp(str, sl_iperf_opt_bw_format_to_str(SL_IPERF_OPT_BW_FORMAT_KBYTES_PER_SEC),
                      SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *bw_format = SL_IPERF_OPT_BW_FORMAT_KBYTES_PER_SEC;
  } else if (!strncmp(str, sl_iperf_opt_bw_format_to_str(SL_IPERF_OPT_BW_FORMAT_MBYTES_PER_SEC),
                      SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *bw_format = SL_IPERF_OPT_BW_FORMAT_MBYTES_PER_SEC;
  } else if (!strncmp(str, sl_iperf_opt_bw_format_to_str(SL_IPERF_OPT_BW_FORMAT_GBYTES_PER_SEC),
                      SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *bw_format = SL_IPERF_OPT_BW_FORMAT_GBYTES_PER_SEC;
  } else {
    return false;
  }
  return true;
}

bool sl_iperf_opt_protocol_from_str(const char *str, sl_iperf_protocol_t * const dest_protocol)
{
  if (!strncmp(str, sl_iperf_opt_protocol_to_str(SL_IPERF_IPROTOV6_UDP),
               SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *dest_protocol = SL_IPERF_IPROTOV6_UDP;
  } else if (!strncmp(str, sl_iperf_opt_protocol_to_str(SL_IPERF_IPROTOV6_TCP),
                      SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *dest_protocol = SL_IPERF_IPROTOV6_TCP;
  } else if (!strncmp(str, sl_iperf_opt_protocol_to_str(SL_IPERF_IPROTOV4_UDP),
                      SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *dest_protocol = SL_IPERF_IPROTOV4_UDP;
  } else if (!strncmp(str, sl_iperf_opt_protocol_to_str(SL_IPERF_IPROTOV4_TCP),
                      SL_IPERF_UTIL_MAX_STRING_LENGTH)) {
    *dest_protocol = SL_IPERF_IPROTOV4_TCP;
  } else {
    return false;
  }
  return true;
}

static void _print_udp_srv_status(sl_iperf_test_t * const test,
                                  const stat_update_params_t * const params)
{
  sl_iperf_test_log(test, SL_IPERF_UDP_SRV_STATISTIC_FORMAT_STR,
                    params->start_time.sec,
                    params->start_time.usec / 10000UL,
                    params->end_time.sec,
                    params->end_time.usec / 10000UL,
                    params->fval_data.int_val,
                    params->fval_data.f0_val,
                    params->fval_data.f1_val,
                    params->fval_data.f2_val,
                    "Kbytes",
                    params->fval_bw.int_val,
                    params->fval_bw.f0_val,
                    params->fval_bw.f1_val,
                    params->fval_bw.f2_val,
                    sl_iperf_opt_bw_format_to_str(test->opt.bw_format),
                    (uint32_t)test->statistic.udp_jitter,
                    params->lost_pkt_curr,
                    params->pkt_cnt,
                    params->pkt_cnt ? (params->lost_pkt_curr * 100UL) / params->pkt_cnt : 0UL);
  if (params->out_of_order_pkt_cnt) {
    sl_iperf_test_log(test, SL_IPERF_UDP_SRV_OUT_OF_ORDER_STAT_FORMAT_STR,
                      params->start_time.sec,
                      params->start_time.usec / 10000UL,
                      params->end_time.sec,
                      params->end_time.usec / 10000UL,
                      params->out_of_order_pkt_cnt);
  }
}

static void _print_udp_clnt_status(sl_iperf_test_t * const test,
                                   const stat_update_params_t * const params)
{
  sl_iperf_test_log(test, SL_IPERF_UDP_CLNT_STATISTIC_FORMAT_STR,
                    params->start_time.sec,
                    params->start_time.usec / 10000UL,
                    params->end_time.sec,
                    params->end_time.usec / 10000UL,
                    params->fval_data.int_val,
                    params->fval_data.f0_val,
                    params->fval_data.f1_val,
                    params->fval_data.f2_val,
                    "Kbytes",
                    params->fval_bw.int_val,
                    params->fval_bw.f0_val,
                    params->fval_bw.f1_val,
                    params->fval_bw.f2_val,
                    sl_iperf_opt_bw_format_to_str(test->opt.bw_format));
}
