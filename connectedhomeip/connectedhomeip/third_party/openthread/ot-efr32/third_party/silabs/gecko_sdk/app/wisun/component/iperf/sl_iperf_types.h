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

#ifndef __SL_IPERF_TYPES_H__
#define __SL_IPERF_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stddef.h>
#include <stdint.h>

#include "sl_iperf_network_interface.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#if  defined(SL_IPERF_UNIT_TEST)
#warning Unit test enabled
#define SL_IPERF_SERVICE_LOOP()       for (uint8_t i = 0U; i < SL_IPERF_SERVICE_LOOP_EXPECTED_CNT; ++i)
#else
#define SL_IPERF_SERVICE_LOOP()       while (1)
#endif

/**************************************************************************//**
 * @addtogroup SL_IPERF_TYPES iPerf type definitions
 * @ingroup SL_IPERF
 * @{
 *****************************************************************************/

/// Iperf mode
typedef enum sl_iperf_mode {
  /// Server mode
  SL_IPERF_MODE_SERVER,
  /// Client mode
  SL_IPERF_MODE_CLIENT,
} sl_iperf_mode_t;

/// Test ID type definition
typedef uint16_t sl_iperf_test_id_t;

/// Test status type definition
typedef enum sl_iperf_status {
  /// Test unused.
  SL_IPERF_TEST_STATUS_FREE,
  /// Test queued.
  SL_IPERF_TEST_STATUS_QUEUED,
  /// Test running
  SL_IPERF_TEST_STATUS_RUNNING,
  /// Test done with no error
  SL_IPERF_TEST_STATUS_DONE,
  /// Test done with error
  SL_IPERF_TEST_STATUS_ERR,
} sl_iperf_status_t;

/// iPerf ip address string max length definition
#define SL_IPERF_IP_STR_BUFF_LEN   (40U)

/// Bandwidth format enumeration type definition
typedef enum sl_iperf_opt_bw_format {
  /// Bits/sec format
  SL_IPERF_OPT_BW_FORMAT_BITS_PER_SEC,
  /// KBits/sec format
  SL_IPERF_OPT_BW_FORMAT_KBITS_PER_SEC,
  /// MBits/sec format
  SL_IPERF_OPT_BW_FORMAT_MBITS_PER_SEC,
  /// GBits/sec format
  SL_IPERF_OPT_BW_FORMAT_GBITS_PER_SEC,
  /// Bytes/sec format
  SL_IPERF_OPT_BW_FORMAT_BYTES_PER_SEC,
  /// KBytes/sec format
  SL_IPERF_OPT_BW_FORMAT_KBYTES_PER_SEC,
  /// MBytes/sec format
  SL_IPERF_OPT_BW_FORMAT_MBYTES_PER_SEC,
  /// GBytes/sec format
  SL_IPERF_OPT_BW_FORMAT_GBYTES_PER_SEC
} sl_iperf_opt_bw_format;

/// iPerf test option type definition
typedef struct sl_iperf_opt {
  /// Server or client mode.
  sl_iperf_mode_t mode;
  /// UDP or TCP protocol
  sl_iperf_protocol_t protocol;
  /// Server or client port.
  uint16_t port;
  /// Remote address for TX.
  char remote_addr[SL_IPERF_IP_STR_BUFF_LEN];
  /// expected bandwidth in bits/sec
  uint32_t bandwidth;
  /// Nbr of packets to tx.
  uint16_t packet_nbr;
  /// Buf len to tx or rx.
  uint16_t buf_len;
  /// Time in sec to tx.
  uint16_t duration_ms;
  /// Win size to tx or rx.
  uint16_t win_size;
  /// Server in persistent mode.
  bool persistent;
  /// Interval (ms) between bandwidth update.
  uint16_t interval_ms;
  /// Bandwidth format
  sl_iperf_opt_bw_format bw_format;
} sl_iperf_opt_t;

/// iPerf statistic data type definition
typedef struct sl_iperf_stats {
  /// Nbr of I/O sys calls.
  uint32_t nbr_calls;
  /// Nbr of bytes rx'd or tx'd on net.
  uint32_t bytes;
  /// Nbr of rx'd or tx'd and lost packets
  uint32_t tot_packets;
  /// Nbr of rx'd or tx'd packets
  uint32_t nbr_rcv_snt_packets;
  /// Nbr of rx or tx errs.
  uint32_t errs;
  /// Nbr of transitory err.
  uint32_t transitory_error_cnts;
  /// Last received packets (for update)
  uint32_t last_recv_pkt_cnt;
  /// Current received packet timestamp
  sl_iperf_ts_ms_t ts_curr_recv_ms;
  /// Previous received packt timestamp
  sl_iperf_ts_ms_t ts_prev_recv_ms;
  /// Current sent packet timestamp
  sl_iperf_ts_ms_t ts_curr_sent_ms;
  /// Previous sent packet timestamp
  sl_iperf_ts_ms_t ts_prev_sent_ms;
  /// UDP jitter
  int64_t udp_jitter;
  /// Prev pkt ID rx'd
  int32_t udp_rx_last_pkt;
  /// Nbr of UDP pkt lost
  uint32_t udp_lost_pkt;
  /// Nbr of pkt rx'd out of order.
  uint32_t udp_out_of_order;
  /// Nbr of pkt ID rx'd more than once.
  uint32_t udp_dup_pkt;
  /// First UDP pkt rx'd.
  bool udp_async_error;
  /// Err with UDP FIN or FINACK.
  bool end_err;
  /// Start timestamp (ms).
  sl_iperf_ts_ms_t ts_start_ms;
  /// End timestamp (ms).
  sl_iperf_ts_ms_t   ts_end_ms;
  /// Rx or Tx cur bandwidth in bits/s.
  uint32_t bandwidth;
  /// Total length of received bytes in Final ACK
  uint32_t finack_tot_len;
  /// Time duration in Final ACK
  sl_iperf_ts_ms_t finack_duration_ms;
  /// Packet count in Final ACK
  uint32_t finack_pkt;
} sl_iperf_stats_t;

/// iPerf connection descriptor type definition
typedef struct sl_iperf_conn {
  /// local socket id
  int32_t socket_id;
  /// Accepted sock used by TCP server to rx.
  int32_t socket_id_clnt;
  /// Server sock addr IP.
  sl_iperf_socket_addr_t srv_addr;
  /// Client sock addr IP.
  sl_iperf_socket_addr_t clnt_addr;
  /// Server (rx'd) or client (tx'd) started.
  bool run;
  /// Buffer ptr to receive/transmit
  uint8_t *buff;
  /// RX/TX buffer transmit
  size_t buff_size;
} sl_iperf_conn_t;

/// iPerf error enumeration type definition
typedef enum sl_iperf_err {
  /// No error
  SL_IPERF_ERR_NONE,
  /// Network connection error
  SL_IPERF_ERR_NETWORK_CONNECTION,
  /// Server socket bind error
  SL_IPERF_ERR_SERVER_SOCK_BIND,
  /// Server socket open error
  SL_IPERF_ERR_SERVER_SOCK_OPEN,
  /// Server socket close error
  SL_IPERF_ERR_SERVER_SOCK_CLOSE,
  /// Server socket listen error
  SL_IPERF_ERR_SERVER_SOCK_LISTEN,
  /// Server socket accept error
  SL_IPERF_ERR_SERVER_SOCK_ACCEPT,
  /// Server socket RX error
  SL_IPERF_ERR_SERVER_SOCK_RX,
  /// Server socket windows size error
  SL_IPERF_ERR_SERVER_SOCK_WIN_SIZE,
  /// Client socket open error
  SL_IPERF_ERR_CLIENT_SOCK_OPEN,
  /// Client socket bind error
  SL_IPERF_ERR_CLIENT_SOCK_BIND,
  /// Client socket connect error
  SL_IPERF_ERR_CLIENT_SOCK_CONN,
  /// Client socket TX error
  SL_IPERF_ERR_CLIENT_SOCK_TX,
  /// Client socket TX invalid argument error
  SL_IPERF_ERR_CLIENT_SOCK_TX_INV_ARG,
  /// Client socket socket close error
  SL_IPERF_ERR_CLIENT_SOCK_CLOSE,
} sl_iperf_error_t;

/// Log string buffer type definition
typedef struct sl_iperf_log_str_buff {
  /// Position ptr
  char *pos;
  /// Buff ptr
  char * buff;
  /// Size
  size_t size;
} sl_iperf_log_str_buff_t;

/// iPerf log type definition
typedef struct sl_iperf_log {
  /// Colored
  bool colored;
  /// Buffered
  bool buffered;
  /// Last result
  int32_t last_res;
  /// Stdout and buffer printer
  int32_t (*print)(struct sl_iperf_log * const log, const char *format, ...);
  /// Buff string instance
  sl_iperf_log_str_buff_t buff;
} sl_iperf_log_t;

/// Printer function type definition
typedef int32_t (*sl_iperf_log_print_t) (sl_iperf_log_t * const log, const char *format, ...);

/// iPerf test descriptor
typedef struct sl_iperf_test {
  /// Test ID
  sl_iperf_test_id_t id;
  /// Status
  sl_iperf_status_t status;
  /// Error
  sl_iperf_error_t err;
  /// Options
  sl_iperf_opt_t opt;
  /// Statistics
  sl_iperf_stats_t statistic;
  /// Connection
  sl_iperf_conn_t conn;
  /// Callback
  void (*cb)(struct sl_iperf_test *);
  /// Log object ptr
  sl_iperf_log_t *log;
} sl_iperf_test_t;

/// iPerf Test callback type definition
typedef void (* sl_iperf_test_callback_t)     (sl_iperf_test_t *);

// UDP packed structure definitions
#pragma pack(4)

/// iPerf UDP datagram structure type definition
typedef struct sl_iperf_udp_datagram {
  /// Packet id
  int32_t id;
  /// Time variable for sec
  uint32_t time_var_sec;
  /// Time variable for usec
  uint32_t time_var_usec;
  /// Packet id
  int32_t id2;
} sl_iperf_udp_datagram_t;

/// iPerf server header
typedef struct sl_iperf_udp_srv_hdr {
  // Udp datagram
  sl_iperf_udp_datagram_t dtg;
  // Server flag.
  int32_t flags;
  // Tot  bytes rx'd hi  part.
  uint32_t tot_len_u;
  // Tot  bytes rx'd low part.
  uint32_t tot_len_l;
  // Stop time  in  sec.
  uint32_t stop_sec;
  // Stop time  in usec.
  uint32_t stop_usec;
  // Lost pkt   cnt.
  uint32_t lost_pkt_cnt;
  // Rx   pkt   out of order cnt.
  uint32_t out_of_order_cnt;
  // Packet count
  uint32_t packet_cnt;
  // Jitter hi.
  uint32_t jitter_sec;
  // Jitter low.
  uint32_t jitter_usec;
} sl_iperf_udp_srv_hdr_t;

/// iPerf CLient Header v1
typedef struct sl_iperf_udp_client_hdr_v1 {
  /// Flags
  int32_t flags;
  /// Number of threads
  int32_t num_threads;
  /// Port
  int32_t port;
  /// Buffer Length
  int32_t buf_len;
  /// Win band
  int32_t win_band;
  /// Amount
  int32_t amount;
} sl_iperf_clnt_hdr_v1_t;

/// iPerf Client Header extended
typedef struct sl_iperf_clnt_hdr_ext {
  /// Type
  int32_t type;
  /// Length
  int32_t length;
  /// Upper flags
  int16_t u_flags;
  /// Lower flags
  int16_t l_flags;
  /// Upper version
  uint32_t u_version;
  /// Lower version
  uint32_t l_version;
  /// Reserved
  uint16_t reserved;
  /// Tos
  uint16_t tos;
  /// Lower rate
  uint32_t l_rate;
  /// Upper rate
  uint32_t u_rate;
  /// TCP write prefetch
  uint32_t tcp_write_prefetch;
} sl_iperf_clnt_hdr_ext_t;

/// iPerf Client Isochronus payload
typedef struct sl_iperf_clnt_hdr_isoch_payload {
  /// period units microseconds
  uint32_t burst_period;
  /// Start sec
  uint32_t start_tv_sec;
  /// Start usec
  uint32_t start_tv_usec;
  /// Previous frame ID
  uint32_t prev_frameid;
  /// Frame ID
  uint32_t frame_id;
  /// Burst size
  uint32_t burst_size;
  /// Remaining
  uint32_t remaining;
  /// Reserved
  uint32_t reserved;
} sl_iperf_clnt_hdr_isoch_payload_t;

/// iPerf Client header extended FQ start time
typedef struct sl_iperf_clnt_hdr_ext_starttime_fq {
  /// Reserved
  uint32_t reserved;
  /// Start sec
  uint32_t start_tv_sec;
  /// Start usec
  uint32_t start_tv_usec;
  /// Lower FQ rate
  uint32_t l_fq_rate;
  /// Upper FQ rate
  uint32_t u_fq_rate;
} sl_iperf_clnt_hdr_ext_starttime_fq_t;

/// iPerf Client Isochronus settings
typedef struct sl_iperf_clnt_hdr_ext_isoch_settings {
  /// Lower FPS
  int32_t l_fps;
  /// Upper FPS
  int32_t u_fps;
  /// Lower Mean
  int32_t l_mean;
  /// Upper Mean
  int32_t u_mean;
  /// Lower Variance
  int32_t l_variance;
  /// Upper Variance
  int32_t u_variance;
  /// Lower Burst IPG
  int32_t l_burst_ipg;
  /// Upper Burst IPG
  int32_t u_burst_ipg;
} sl_iperf_clnt_hdr_ext_isoch_settings_t;

/// iPerf Client UDP header
typedef struct sl_iperf_udp_clnt_hdr {
  /// Datagram
  sl_iperf_udp_datagram_t dtg;
  /// Base v1 header
  sl_iperf_clnt_hdr_v1_t base;
  /// Extended header
  sl_iperf_clnt_hdr_ext_t extend;
  /// Isochronus payload
  sl_iperf_clnt_hdr_isoch_payload_t isoch;
  /// Extended start FQ
  sl_iperf_clnt_hdr_ext_starttime_fq_t start_fq;
  /// Isochronus settings
  sl_iperf_clnt_hdr_ext_isoch_settings_t isoch_settings;
} sl_iperf_udp_clnt_hdr_t;

#pragma pack()

/** @} (end SL_IPERF_TYPES) */

#ifdef __cplusplus
}
#endif
#endif
