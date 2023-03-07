/***************************************************************************//**
 * @file
 * @brief Network - IPerf Module
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _IPERF_PRIV_H_
#define  _IPERF_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/
#include  <common/include/rtos_path.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_def.h>
#include  <common/include/lib_str.h>

#include  <common/source/kal/kal_priv.h>

#include  <iperf_cfg.h>

#include  <net/include/iperf.h>
#include  <net/include/net.h>
#include  <net/include/net_tcp.h>
#include  <net/include/net_app.h>
#include  <net/include/net_ascii.h>

#include  "../tcpip/net_tcp_priv.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                        MODULE CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#if     (IPERF_CFG_SERVER_EN == DEF_ENABLED)
#define  IPERF_SERVER_MODULE_PRESENT
#endif

#if     (IPERF_CFG_CLIENT_EN == DEF_ENABLED)
#define  IPERF_CLIENT_MODULE_PRESENT
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                         CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef NET_TCP_MODULE_EN
#error  "NET_TCP_CFG_EN illegally #define'd in 'net_cfg.h' [MUST be  DEF_ENABLED]"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                        IPERF BUFFER DEFINES
 *******************************************************************************************************/

#define  IPERF_TCP_BUF_LEN_MAX             IPERF_CFG_BUF_LEN
#define  IPERF_UDP_BUF_LEN_MAX                          1472u   // Dev buf MUST be equal or greater than ...
#define  IPERF_UDP_BUF_LEN_MAX_IPv6                     1450u   // ... NET_BUF_DATA_IX_TX + IPERF_UDP_BUF_LEN_MAX.

#define  IPERF_BUF_LEN_MAX             IPERF_TCP_BUF_LEN_MAX

/********************************************************************************************************
 *                                        IPERF SERVER DEFINES
 *******************************************************************************************************/

#define  IPERF_SERVER_TCP_CONN_Q_SIZE                      1u

#define  IPERF_SERVER_UDP_TX_FINACK_COUNT                 10u
#define  IPERF_SERVER_UDP_TX_FINACK_ERR_MAX               10u

#define  IPERF_SERVER_UDP_HEADER_VERSION1         0x80000000u

/********************************************************************************************************
 *                                    IPERF STRING COMMAND DEFINES
 *******************************************************************************************************/

#define  IPERF_CMD_ARG_NBR_MAX                            12u   // Max nbr of arg(s) a cmd may pass on the string ...
                                                                // ... holding the complete command.

/********************************************************************************************************
 *                             IPERF DEFAULT CONFIGURATION OPTION DEFINES
 *******************************************************************************************************/

#define  IPERF_DFLT_PROTOCOL               IPERF_PROTOCOL_TCP   // Dflt proto       is TCP.
#define  IPERF_DFLT_TCP_BUF_LEN             IPERF_CFG_BUF_LEN   // Dflt TCP buf     is maximum buf len.
#define  IPERF_DFLT_UDP_BUF_LEN         IPERF_UDP_BUF_LEN_MAX   // Dflt UDP buf     is 1472 bytes.
#define  IPERF_DFLT_IP_REMOTE                        "0.0.0.0"  // Dflt remote   IP is NOT set.
#define  IPERF_DFLT_PORT                                5001u   // Dflt port        is 5001.
#define  IPERF_DFLT_BYTES_NBR                              0u   // Dftl send by     is duration.
#define  IPERF_DFLT_DURATION_MS                        10000u   // Dflt duration    is 10 sec.
#define  IPERF_DFLT_PERSISTENT                   DEF_DISABLED   // Dflt persistent  is NOT en'd.
#define  IPERF_DFLT_IF                                     1u   // Dflt IF          is 1.
#define  IPERF_DFLT_FMT             IPERF_ASCII_FMT_KBITS_SEC   // Dflt fmt         is kbps.
#define  IPERF_DFLT_RX_WIN      NET_TCP_DFLT_RX_WIN_SIZE_OCTET  // Dflt rx win size is max.
#define  IPERF_DFLT_TX_WIN      NET_TCP_DFLT_TX_WIN_SIZE_OCTET  // Dflt tx win size is max.
#define  IPERF_DFLT_INTERVAL_MS                         1000u   // Dflt interval    is 1000 ms.

/********************************************************************************************************
 *                                        IPERF TIMEOUT DEFINES
 *******************************************************************************************************/

/********************************************************************************************************
 *                                  IPERF RETRY & TIME DELAY DEFINES
 *******************************************************************************************************/

#define  IPERF_OPEN_MAX_RETRY                              2u   // Max nbr of  retries on open.
#define  IPERF_OPEN_MAX_DLY_MS                             5u   // Dly between retries on open.

#define  IPERF_BIND_MAX_RETRY                              2u   // Max nbr of  retries on bind.
#define  IPERF_BIND_MAX_DLY_MS                             5u   // Dly between retries on bind.

#define  IPERF_SERVER_TCP_RX_MAX_RETRY                     3u   // Max nbr of  retries on rx.
#define  IPERF_SERVER_UDP_RX_MAX_RETRY                     3u   // Max nbr of  retries on rx.

#define  IPERF_SERVER_UDP_TX_MAX_RETRY                     3u   // Max nbr of  retries on tx.
#define  IPERF_SERVER_UDP_TX_MAX_DLY_MS                   50u   // Dly between retries on tx.

#define  IPERF_CLIENT_UDP_RX_MAX_RETRY                     3u   // Max nbr of  retries on rx.

#define  IPERF_CLIENT_TCP_TX_MAX_RETRY                     3u   // Max nbr of  retries on tx.
#define  IPERF_CLIENT_TCP_TX_MAX_DLY_MS                    1u   // Dly between retries on tx.
#define  IPERF_CLIENT_UDP_TX_MAX_RETRY                     3u   // Max nbr of  retries on tx.
#define  IPERF_CLIENT_UDP_TX_MAX_DLY_MS                    1u   // Dly between retries on tx.

#define  IPERF_RX_UDP_FINACK_MAX_RETRY                    10u   // Max nbr of retries  on rx'd in UDP FIN ACK.
#define  IPERF_RX_UDP_FINACK_MAX_DLY_MS                   50u   // Dly between retries on rx'd in UDP FIN ACK.
#define  IPERF_RX_UDP_FINACK_MAX_TIMEOUT_MS               50u   // Timeout for sock data  rx'd in UDP FIN ACK.

#define  IPERF_UDP_CPU_USAGE_CALC_DLY_MS                 200u   // Dly between end of UDP and CPU usage calc.

/********************************************************************************************************
 *                                     IPERF ASCII OPTION DEFINES
 *******************************************************************************************************/

#define  IPERF_ASCII_BEGIN_OPT                            '-'   // Opt beginning.
#define  IPERF_ASCII_IP_SEP                               '.'   // IP addr separator.
#define  IPERF_ASCII_OPT_HELP                             'h'   // Help menu            opt.
#define  IPERF_ASCII_OPT_VER                              'v'   // Ver                  opt.
#define  IPERF_ASCII_OPT_SERVER                           's'   // Server               opt.
#define  IPERF_ASCII_OPT_CLIENT                           'c'   // Client               opt.
#define  IPERF_ASCII_OPT_FMT                              'f'   // Fmt                  opt.
#define  IPERF_ASCII_OPT_LENGTH                           'l'   // Buf len              opt.
#define  IPERF_ASCII_OPT_TIME                             't'   // Tx duration          opt.
#define  IPERF_ASCII_OPT_NUMBER                           'n'   // TX nbr bytes         opt.
#define  IPERF_ASCII_OPT_PORT                             'p'   // Port                 opt.
#define  IPERF_ASCII_OPT_UDP                              'u'   // UDP protocol         opt.
#define  IPERF_ASCII_OPT_WINDOW                           'w'   // Win size             opt.
#define  IPERF_ASCII_OPT_PERSISTENT                       'D'   // Server Persistent    opt.
#define  IPERF_ASCII_OPT_IPV6                             'V'   // IPV6                 opt.
#define  IPERF_ASCII_OPT_INTERVAL                         'i'   // Rate update interval opt.

//                                                                 --------------- IPERF TRANSFER RATE ----------------
#define  IPERF_ASCII_FMT_BITS_SEC                         'b'   // bits/sec
#define  IPERF_ASCII_FMT_KBITS_SEC                        'k'   // Kbits/sec
#define  IPERF_ASCII_FMT_MBITS_SEC                        'm'   // Mbits/sec
#define  IPERF_ASCII_FMT_GBITS_SEC                        'g'   // Gbits/sec
#define  IPERF_ASCII_FMT_BYTES_SEC                        'B'   // bytes/sec
#define  IPERF_ASCII_FMT_KBYTES_SEC                       'K'   // Kbytes/sec
#define  IPERF_ASCII_FMT_MBYTES_SEC                       'M'   // Mbytes/sec
#define  IPERF_ASCII_FMT_GBYTES_SEC                       'G'   // Gbytes/sec
#define  IPERF_ASCII_FMT_ADAPTIVE_BITS_SEC                'a'   // adaptive   bits/sec
#define  IPERF_ASCII_FMT_ADAPTIVE_BYTES_SEC               'A'   // adaptive  bytes/sec

#define  IPERF_ASCII_SPACE                                ' '   // ASCII val for space
#define  IPERF_ASCII_QUOTE                                '\"'  // ASCII val for quote
#define  IPERF_ASCII_CMD_NAME_DELIMITER                   '_'   // ASCII val for underscore
#define  IPERF_ASCII_ARG_END                              '\0'  // ASCII val for arg separator.

/********************************************************************************************************
 *                                    IPERF OUTPUT MESSAGE DEFINES
 *******************************************************************************************************/

#define  IPERF_MSG_MENU                   "Server specific:\n\r"                                                   \
                                          " -s              Run in server mode\n\r"                                \
                                          " -D              Run the server as persistent\n\r"                      \
                                          " -w              Rx TCP window size (IGNORED with UDP option)\n\r"      \
                                          "\n\r"                                                                   \
                                          "Client specific:\n\r"                                                   \
                                          " -c              Run in client mode\n\r"                                \
                                          " -t              Time in seconds to transmit for (default 10 secs)\n\r" \
                                          " -n              Number of bytes to transmit (instead of -t)\n\r"       \
                                          "<host>           IP address of <host> to connect to\n\r"                \
                                          "\n\r"                                                                   \
                                          "Options:\n\r"                                                           \
                                          " -f    [kmKM]    Format to report: kbits, mbits, Kbytes, MBytes\n\r"    \
                                          " -l              Length of buffer to read or write (default 8 KB)\n\r"  \
                                          " -p              Server port to listen on/connect to\n\r"               \
                                          " -u              Use UDP rather than TCP\n\r"                           \
                                          " -i              MILISECONDS between periodic bandwidth report\n\r"     \
                                          "\n\r"                                                                   \
                                          "Miscellaneous:\n\r"                                                     \
                                          " -h              Print this message\n\r"                                \
                                          " -v              Print version information\n\r"

#define  IPERF_MSG_VER_STR_MAX_LEN                         8u

#define  IPERF_MSG_ERR_BUF_LEN            "Buffer length specified exceed maximum buffer length\n\r"

#define  IPERF_MSG_ERR_UDP_LEN            "UDP Protocol don't support fragmentation\n\r" \
                                          "UDP max buffer length is 1460 bytes"

#define  IPERF_MSG_ERR_OPT                "Invalid option\n\r"                     \
                                          "Usage: IPerf [-s|c host] [options]\n\r" \
                                          "Try IPerf -h for more information\n\r"

#define  IPERF_MSG_ERR_OPT_NOT_SUPPORTED  "Option not supported/implemented\n\r"

#define  IPERF_MSG_ERR_NOT_EN             "Option NOT Enabled\n\r"

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                    IPERF UDP DATAGRAM DATA TYPE
 *******************************************************************************************************/

typedef  struct  iperf_udp_datagram {
  CPU_INT32S ID;                                                // Pkt  ID.
  CPU_INT32U TimeVar_sec;                                       // Time var for  sec.
  CPU_INT32U TimeVar_usec;                                      // Time var for usec.
} IPERF_UDP_DATAGRAM;

/********************************************************************************************************
 *                                  IPERF UDP SERVER HEADER DATA TYPE
 *******************************************************************************************************/

typedef  struct  iperf_server_udp_hdr {
  CPU_INT32S Flags;                                             // Server flag.
  CPU_INT32U TotLen_Hi;                                         // Tot  bytes rx'd hi  part.
  CPU_INT32U TotLen_Lo;                                         // Tot  bytes rx'd low part.
  CPU_INT32S Stop_sec;                                          // Stop time  in  sec.
  CPU_INT32S Stop_usec;                                         // Stop time  in usec.
  CPU_INT32U LostPkt_ctr;                                       // Lost pkt   cnt.
  CPU_INT32U OutOfOrder_ctr;                                    // Rx   pkt   out of order cnt.
  CPU_INT32S RxLastPkt;                                         // Last pkt   ID  rx'd.
  CPU_INT32U Jitter_Hi;                                         // Jitter hi.
  CPU_INT32U Jitter_Lo;                                         // Jitter low.
} IPERF_SERVER_UDP_HDR;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

IPERF_EXT KAL_LOCK_HANDLE IPerf_LockHandle;
IPERF_EXT const IPERF_CFG *IPerf_CfgPtr;
IPERF_EXT CPU_CHAR        *IPerf_BufPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                        IPERF CORE FUNCTIONS
 *******************************************************************************************************/

void IPerf_TestTaskHandler(void *p_arg);

#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
void IPerf_UpdateBandwidth(IPERF_TEST  *p_test,
                           IPERF_TS_MS *p_ts_ms_prev,
                           CPU_INT32U  *p_data_bytes_prev);
#endif

IPERF_TEST_ID IPerf_TestShellStart(CPU_INT16U      argc,
                                   CPU_CHAR        *p_argv[],
                                   IPERF_OUT_FNCT  p_out_fnct,
                                   IPERF_OUT_PARAM *p_out_param,
                                   RTOS_ERR        *p_err);

void IPerf_TestClrStats(IPERF_STATS *p_stats);

IPERF_TS_MS IPerf_Get_TS_ms(void);

IPERF_TS_MS IPerf_Get_TS_Max_ms(void);

CPU_INT32U IPerf_GetDataFmtd(IPERF_FMT  fmt,
                             CPU_INT32U bytes_qty);

void IPerfShell_Init(RTOS_ERR *p_err);

/********************************************************************************************************
 *                                       IPERF SERVER FUNCTIONS
 *******************************************************************************************************/

#ifdef  IPERF_SERVER_MODULE_PRESENT
void IPerf_ServerStart(IPERF_TEST *p_test,
                       RTOS_ERR   *p_err);
#endif

/********************************************************************************************************
 *                                       IPERF CLIENT FUNCTIONS
 *******************************************************************************************************/
#ifdef  IPERF_CLIENT_MODULE_PRESENT
void IPerf_ClientStart(IPERF_TEST *p_test,
                       RTOS_ERR   *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _IPERF_PRIV_H_
