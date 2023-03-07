/***************************************************************************//**
 * @file
 * @brief Network Tcp Layer - (Transmission Control Protocol)
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

#ifndef  _NET_TCP_PRIV_H_
#define  _NET_TCP_PRIV_H_

#include  "../../include/net_cfg_net.h"

#ifdef   NET_TCP_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_tcp.h"

#include  "net_def_priv.h"
#include  "net_type_priv.h"
#include  "net_tmr_priv.h"

#ifdef  NET_IPv4_MODULE_EN
#include  "net_ipv4_priv.h"
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  "net_ipv6_priv.h"
#endif

#include  <cpu/include/cpu.h>
#include  <common/source/kal/kal_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef NET_TCP_MODULE
#define  NET_TCP_EXT
#else
#define  NET_TCP_EXT  extern
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_TCP_CFG_NEW_WINDOW_MGMT_EN
#define  NET_TCP_CFG_OLD_WINDOW_MGMT_EN
#endif

#ifndef  NET_TCP_CFG_NBR_CONN
#define  NET_TCP_CFG_NBR_CONN                           0u
#endif

#define  NET_TCP_NBR_CONN                              (NET_TCP_CFG_NBR_CONN         \
                                                        + NET_SOCK_CFG_SOCK_NBR_TCP  \
                                                        + (NET_SOCK_CFG_SOCK_NBR_TCP \
                                                           * NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX))

#ifndef  NET_TCP_DFLT_RX_WIN_SIZE_OCTET
    #define  NET_TCP_DFLT_RX_WIN_SIZE_OCTET              NET_SOCK_CFG_RX_Q_SIZE_OCTET
#endif

#ifndef  NET_TCP_DFLT_TX_WIN_SIZE_OCTET
    #define  NET_TCP_DFLT_TX_WIN_SIZE_OCTET              NET_SOCK_CFG_TX_Q_SIZE_OCTET
#endif

#ifndef  NET_TCP_DFLT_TIMEOUT_CONN_MAX_SEG_SEC
//                                                                 Configure TCP connections' default maximum ...
//                                                                 ... segment lifetime timeout (MSL) value,  ...
    #define  NET_TCP_DFLT_TIMEOUT_CONN_MAX_SEG_SEC          0u  // ... in integer seconds (see Note #3).
#endif

#ifndef  NET_TCP_DFLT_TIMEOUT_CONN_FIN_WAIT_2_SEC
//                                                                 Configure TCP connections' default FIN-WAIT-2 ...
    #define  NET_TCP_DFLT_TIMEOUT_CONN_FIN_WAIT_2_SEC      15u  // ... timeout value, in integer seconds (see Note #4).
#endif

#ifndef  NET_TCP_DFLT_TIMEOUT_CONN_ACK_DLY_MS

//                                                                 Configure TCP acknowledgement delay ...
    #define  NET_TCP_DFLT_TIMEOUT_CONN_ACK_DLY_MS         400u  // ... in integer milliseconds (see Note #5).
#endif

#ifndef  NET_TCP_DFLT_TIMEOUT_CONN_RX_Q_MS
//                                                                 Configure TCP timeouts (see Note #6) :
//                                                                 Configure TCP connection receive  queue timeout.
    #define  NET_TCP_DFLT_TIMEOUT_CONN_RX_Q_MS           1000u
#endif

#ifndef  NET_TCP_DFLT_TIMEOUT_CONN_TX_Q_MS
//                                                                 Configure TCP connection transmit queue timeout.
    #define  NET_TCP_DFLT_TIMEOUT_CONN_TX_Q_MS           1000u
#endif

/********************************************************************************************************
 *                                           TCP HEADER DEFINES
 *
 * Note(s) : (1) The following TCP value MUST be pre-#define'd in 'net_def.h' PRIOR to 'net_buf.h' so that
 *               the Network Buffer Module can configure maximum buffer header size (see 'net_def.h  TCP
 *               LAYER DEFINES' & 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1a2') :
 *
 *               (a) NET_TCP_HDR_SIZE_MAX                  60        (NET_TCP_HDR_LEN_MAX
 *                                                                   * NET_TCP_HDR_LEN_WORD_SIZE)
 *
 *           (2) Urgent pointer & data NOT supported (see 'net_tcp.h  Note #1b').
 *******************************************************************************************************/

#define  NET_TCP_HDR_LEN_MASK                         0xF000u
#define  NET_TCP_HDR_LEN_SHIFT                            12u
#define  NET_TCP_HDR_LEN_NONE                              0

#if 0                                                           // See Note #1a.
#define  NET_TCP_HDR_SIZE_MAX                           (NET_TCP_HDR_LEN_MAX * NET_TCP_HDR_LEN_WORD_SIZE)
#endif

#define  NET_TCP_HDR_SIZE_TOT_MIN                       (NET_IP_HDR_SIZE_TOT_MIN + NET_TCP_HDR_SIZE_MIN)
#define  NET_TCP_HDR_SIZE_TOT_MAX                       (NET_IP_HDR_SIZE_TOT_MAX + NET_TCP_HDR_SIZE_MAX)

#define  NET_TCP_PSEUDO_HDR_SIZE                          12    // = sizeof(NET_TCP_PSEUDO_HDR)

#define  NET_TCP_PORT_NBR_RESERVED                       NET_PORT_NBR_RESERVED
#define  NET_TCP_PORT_NBR_NONE                           NET_TCP_PORT_NBR_RESERVED

#define  NET_TCP_HDR_URG_PTR_NONE                     0x0000u   // See Note #2.

/********************************************************************************************************
 *                                       TCP HEADER FLAG DEFINES
 *
 * Note(s) : (1) See 'TCP HEADER  Note #2' for flag fields.
 *
 *           (2) Urgent pointer & data NOT supported (see 'net_tcp.h  Note #1b').
 *******************************************************************************************************/

#define  NET_TCP_HDR_FLAG_MASK                        0x0FFFu

#define  NET_TCP_HDR_FLAG_NONE                    DEF_BIT_NONE
#define  NET_TCP_HDR_FLAG_RESERVED                    0x0FE0u   // MUST be '0'.
#define  NET_TCP_HDR_FLAG_URGENT                  DEF_BIT_05    // See Note #2.
#define  NET_TCP_HDR_FLAG_ACK                     DEF_BIT_04
#define  NET_TCP_HDR_FLAG_PUSH                    DEF_BIT_03
#define  NET_TCP_HDR_FLAG_RESET                   DEF_BIT_02
#define  NET_TCP_HDR_FLAG_SYNC                    DEF_BIT_01
#define  NET_TCP_HDR_FLAG_FIN                     DEF_BIT_00
#define  NET_TCP_HDR_FLAG_CLOSE                   NET_TCP_HDR_FLAG_FIN

/********************************************************************************************************
 *                                           TCP FLAG DEFINES
 *******************************************************************************************************/

//                                                                 ------------------ NET TCP FLAGS -------------------
#define  NET_TCP_FLAG_NONE                        DEF_BIT_NONE
#define  NET_TCP_FLAG_USED                        DEF_BIT_00    // TCP conn cur used; i.e. NOT in free TCP conn pool.

//                                                                 ------------------ TCP TX  FLAGS -------------------
//                                                                 TCP tx flags copied from TCP hdr flags.
#define  NET_TCP_FLAG_TX_FIN                      NET_TCP_HDR_FLAG_FIN
#define  NET_TCP_FLAG_TX_CLOSE                    NET_TCP_FLAG_TX_FIN
#define  NET_TCP_FLAG_TX_SYNC                     NET_TCP_HDR_FLAG_SYNC
#define  NET_TCP_FLAG_TX_RESET                    NET_TCP_HDR_FLAG_RESET
#define  NET_TCP_FLAG_TX_PUSH                     NET_TCP_HDR_FLAG_PUSH
#define  NET_TCP_FLAG_TX_ACK                      NET_TCP_HDR_FLAG_ACK
#define  NET_TCP_FLAG_TX_URGENT                   NET_TCP_HDR_FLAG_URGENT

#define  NET_TCP_FLAG_TX_BLOCK                    DEF_BIT_07

//                                                                 ------------------ TCP RX  FLAGS -------------------
#define  NET_TCP_FLAG_RX_DATA_PEEK                DEF_BIT_08
#define  NET_TCP_FLAG_RX_BLOCK                    DEF_BIT_15

/********************************************************************************************************
 *                                       TCP SEQUENCE NUMBER DEFINES
 *
 * Note(s) : (1) TCP initial transmit sequence number is incremented by a fixed value, preferably a large
 *               prime value or a large value with multiple unique factors.
 *
 *               (a) One reasonable TCP initial transmit sequence number increment value example :
 *
 *                       65527  =  37 * 23 * 11 * 7
 *
 *               NET_TCP_TX_SEQ_NBR_CTR_INC could be developer-configured in 'net_cfg.h'.
 *
 *               See also 'NET_TCP_TX_GET_SEQ_NBR()  Notes #1b2 & #1c2'.
 *******************************************************************************************************/

#define  NET_TCP_SEQ_NBR_NONE                          0u
#define  NET_TCP_ACK_NBR_NONE                          NET_TCP_SEQ_NBR_NONE

#ifndef  NET_TCP_DFLT_TX_SEQ_NBR_CTR_INC
#define  NET_TCP_TX_SEQ_NBR_CTR_INC                    65527u   // See Note #1.
#else
#define  NET_TCP_TX_SEQ_NBR_CTR_INC                    NET_TCP_DFLT_TX_SEQ_NBR_CTR_INC
#endif

#define  NET_TCP_ACK_NBR_DUP_WIN_SIZE_SCALE                4

/********************************************************************************************************
 *                                       TCP DATA/TOTAL LENGTH DEFINES
 *
 * Note(s) : (1) (a) TCP total length #define's (NET_TCP_TOT_LEN)  relate to the total size of a complete
 *                   TCP packet, including the packet's TCP header.  Note that a complete TCP packet MAY
 *                   be fragmented in multiple Internet Protocol packets.
 *
 *               (b) TCP data  length #define's (NET_TCP_DATA_LEN) relate to the data  size of a complete
 *                   TCP packet, equal to the total TCP packet length minus its TCP header size.  Note
 *                   that a complete TCP packet MAY be fragmented in multiple Internet Protocol packets.
 *******************************************************************************************************/

//                                                                 See Notes #1a & #1b.
#define  NET_TCP_DATA_LEN_MIN                              0

#define  NET_TCP_TOT_LEN_MIN                            (NET_TCP_HDR_SIZE_MIN  + NET_TCP_DATA_LEN_MIN)
#if    defined(NET_IPv4_MODULE_EN)
#define  NET_TCP_TOT_LEN_MAX                            (NET_IPv4_TOT_LEN_MAX  - NET_IPv4_HDR_SIZE_MIN)
#elif  defined(NET_IPv6_MODULE_EN)
#define  NET_TCP_TOT_LEN_MAX                            (NET_IPv6_TOT_LEN_MAX  - NET_IPv6_HDR_SIZE)
#endif

#define  NET_TCP_DATA_LEN_MAX                           (NET_TCP_TOT_LEN_MAX  - NET_TCP_HDR_SIZE_MIN)

/********************************************************************************************************
 *                                       TCP SEGMENT SIZE DEFINES
 *
 * Note(s) : (1) (a) RFC # 879, Section 3 states that the TCP Maximum Segment Size "counts only
 *                   data octets in the segment, ... not the TCP header or the IP header".
 *
 *               (b) RFC #1122, Section 4.2.2.6 requires that :
 *
 *                   (1) "The MSS value to be sent in an MSS option must be less than or equal to
 *
 *                           (A) MMS_R - 20
 *
 *                           where MMS_R is the maximum size for a transport-layer message that can
 *                           be received."
 *
 *                   (2) "If an MSS option is not received at connection setup, TCP MUST assume a
 *                           default send MSS of 536 (576 - 40)."
 *
 *                   See also 'net_ip.h  IP DATA/TOTAL LENGTH DEFINES  Note #1'.
 *******************************************************************************************************/

//                                                                 See Note #1.
#ifdef   NET_IPv4_MODULE_EN
#define  NET_TCP_MAX_SEG_SIZE_DFLT_V4                   (NET_IPv4_MAX_DATAGRAM_SIZE_DFLT - NET_IPv4_HDR_SIZE_MIN - NET_TCP_HDR_SIZE_MIN)
#endif

#ifdef   NET_IPv6_MODULE_EN
#define  NET_TCP_MAX_SEG_SIZE_DFLT_V6                   (NET_IPv6_MAX_DATAGRAM_SIZE_DFLT - NET_IPv6_HDR_SIZE - NET_TCP_HDR_SIZE_MIN)
#endif

#if    defined(NET_IPv4_MODULE_EN) && defined(NET_IPv6_MODULE_EN)
#define  NET_TCP_MAX_SEG_SIZE_DFLT                       NET_TCP_MAX_SEG_SIZE_DFLT_V6
#elif  defined(NET_IPv4_MODULE_EN)
#define  NET_TCP_MAX_SEG_SIZE_DFLT                       NET_TCP_MAX_SEG_SIZE_DFLT_V4
#elif  defined(NET_IPv6_MODULE_EN)
#define  NET_TCP_MAX_SEG_SIZE_DFLT                       NET_TCP_MAX_SEG_SIZE_DFLT_V6
#endif

#define  NET_TCP_MAX_SEG_SIZE_DFLT_RX                    NET_TCP_DATA_LEN_MAX           // See Note #1b1.
#define  NET_TCP_MAX_SEG_SIZE_DFLT_TX                    NET_TCP_MAX_SEG_SIZE_DFLT      // See Note #1b2.

#define  NET_TCP_MAX_SEG_SIZE_NONE                         0
#define  NET_TCP_MAX_SEG_SIZE_MIN                        NET_TCP_MAX_SEG_SIZE_DFLT
#define  NET_TCP_MAX_SEG_SIZE_MAX                        NET_TCP_DATA_LEN_MAX

#define  NET_TCP_SEG_LEN_MIN                             NET_TCP_DATA_LEN_MIN
#define  NET_TCP_SEG_LEN_MAX                             NET_TCP_DATA_LEN_MAX

#define  NET_TCP_SEG_LEN_SYNC                              1
#define  NET_TCP_SEG_LEN_FIN                               1
#define  NET_TCP_SEG_LEN_CLOSE                           NET_TCP_SEG_LEN_FIN
#define  NET_TCP_SEG_LEN_ACK                               0
#define  NET_TCP_SEG_LEN_RESET                             0
#define  NET_TCP_SEG_LEN_PROBE                             0

#define  NET_TCP_DATA_LEN_TX_SYNC                          0
#define  NET_TCP_DATA_LEN_TX_FIN                           0
#define  NET_TCP_DATA_LEN_TX_CLOSE                       NET_TCP_DATA_LEN_TX_FIN
#define  NET_TCP_DATA_LEN_TX_ACK                           0
#define  NET_TCP_DATA_LEN_TX_PROBE_NO_DATA                 0
#define  NET_TCP_DATA_LEN_TX_PROBE_DATA                    1
#define  NET_TCP_DATA_LEN_TX_RESET                         0

/********************************************************************************************************
 *                                       TCP WINDOW SIZE DEFINES
 *
 * Note(s) : (1) Although NO RFC specifies the absolute minimum TCP connection window size value allowed,
 *               RFC #793, Section 3.7 'Data Communication : Managing the Window' states that for "the
 *               window ... there is an assumption that this is related to the currently available data
 *               buffer space available for this connection".
 *******************************************************************************************************/

#define  NET_TCP_WIN_SIZE_NONE                             0

#define  NET_TCP_WIN_SIZE_MIN                            NET_TCP_MAX_SEG_SIZE_MIN
#define  NET_TCP_WIN_SIZE_MAX                            DEF_INT_16U_MAX_VAL

/********************************************************************************************************
 *                                       TCP HEADER OPTIONS DEFINES
 *
 * Note(s) : (1) See the following RFC's for TCP options summary :
 *
 *               (a) RFC # 793, Section  3.1 'Header Format : Options'
 *               (b) RFC #1122; Sections 4.2.2.5, 4.2.2.6
 *
 *           (2) TCP option types are encoded in the first octet for each TCP option as follows :
 *
 *                           --------
 *                           | TYPE |
 *                           --------
 *
 *               The TCP option type value determines the TCP option format :
 *
 *               (a) The following TCP option types are single-octet TCP options -- i.e. the option type
 *                   octet is the ONLY octet for the TCP option.
 *
 *                   (1) TYPE =  0   End of Options List
 *                   (2) TYPE =  1   No Operation
 *
 *               (b) All other TCP options MUST be multi-octet TCP options (see RFC #1122, Section 4.2.2.5) :
 *
 *                           ------------------------------
 *                           | TYPE | LEN  |   TCP OPT    |
 *                           ------------------------------
 *
 *                       where
 *                               TYPE        Indicates the specific TCP option type
 *                               LEN         Indicates the total    TCP option length, in octets, including
 *                                                   the option type & the option length octets
 *                               TCP OPT     Additional TCP option octets, if any, that contain the remaining
 *                                                   TCP option information
 *
 *                   The following TCP option types are multi-octet TCP options where the option's second
 *                   octet specify the total TCP option length, in octets, including the option type & the
 *                   option length octets :
 *
 *                   (1) TYPE =  2   Maximum Segment Size        See RFC # 793, Section  3.1 'Header Format :
 *                                                                   Options : Maximum Segment Size';
 *                                                                   RFC #1122, Section 4.2.2.6;
 *                                                                   RFC # 879, Section 3
 *
 *                   (2) TYPE =  3   Window  Scale               See 'net_tcp.h  Note #1c1'
 *                   (3) TYPE =  4   SACK Allowed                See 'net_tcp.h  Note #1c2'
 *                   (4) TYPE =  5   SACK Option                 See 'net_tcp.h  Note #1c2'
 *                   (5) TYPE =  6   Echo Request                See 'net_tcp.h  Note #1c3'
 *                   (6) TYPE =  7   Echo Reply                  See 'net_tcp.h  Note #1c3'
 *                   (7) TYPE =  8   Timestamp                   See 'net_tcp.h  Note #1c4'
 *
 *           (3) TCP header allows for a maximum option list length of 40 octets :
 *
 *                   NET_TCP_HDR_OPT_SIZE_MAX = NET_TCP_HDR_SIZE_MAX - NET_TCP_HDR_SIZE_MIN
 *
 *                                               = 60 - 20
 *
 *                                               = 40
 *
 *           (4) 'NET_TCP_OPT_SIZE'  MUST be pre-defined PRIOR to all definitions that require TCP option
 *                   size data type.
 *******************************************************************************************************/

#define  NET_TCP_HDR_OPT_END_LIST                          0u
#define  NET_TCP_HDR_OPT_NOP                               1u
#define  NET_TCP_HDR_OPT_MAX_SEG_SIZE                      2u
#define  NET_TCP_HDR_OPT_WIN_SCALE                         3u
#define  NET_TCP_HDR_OPT_SACK_PERMIT                       4u
#define  NET_TCP_HDR_OPT_SACK                              5u
#define  NET_TCP_HDR_OPT_ECHO_REQ                          6u
#define  NET_TCP_HDR_OPT_ECHO_REPLY                        7u
#define  NET_TCP_HDR_OPT_TS                                8u

#define  NET_TCP_HDR_OPT_PAD                             NET_TCP_HDR_OPT_END_LIST

#define  NET_TCP_HDR_OPT_LEN_END_LIST                      1
#define  NET_TCP_HDR_OPT_LEN_NOP                           1
#define  NET_TCP_HDR_OPT_LEN_MAX_SEG_SIZE                  4
#define  NET_TCP_HDR_OPT_LEN_WIN_SCALE                     3
#define  NET_TCP_HDR_OPT_LEN_SACK_PERMIT                   2
#define  NET_TCP_HDR_OPT_LEN_ECHO_REQ                      6
#define  NET_TCP_HDR_OPT_LEN_ECHO_REPLY                    6
#define  NET_TCP_HDR_OPT_LEN_TS                           10

#define  NET_TCP_HDR_OPT_LEN_SACK_MIN                      6
#define  NET_TCP_HDR_OPT_LEN_SACK_MAX                     38

#define  NET_TCP_HDR_OPT_LEN_MIN                           1
#define  NET_TCP_HDR_OPT_LEN_MIN_LEN                       2
#define  NET_TCP_HDR_OPT_LEN_MAX                          38

typedef  CPU_INT32U NET_TCP_OPT_SIZE;                           // TCP opt size data type (see Note #4).

#define  NET_TCP_HDR_OPT_SIZE_WORD               (sizeof(NET_TCP_OPT_SIZE))
#define  NET_TCP_HDR_OPT_SIZE_MAX                       (NET_TCP_HDR_SIZE_MAX - NET_TCP_HDR_SIZE_MIN)

#define  NET_TCP_HDR_OPT_NBR_MIN                           0
#define  NET_TCP_HDR_OPT_NBR_MAX                        (NET_TCP_HDR_OPT_SIZE_MAX / NET_TCP_HDR_OPT_SIZE_WORD)

#define  NET_TCP_HDR_OPT_IX                              NET_TCP_HDR_SIZE_MIN

/********************************************************************************************************
 *                                   TCP OPTION CONFIGURATION TYPE DEFINES
 *
 * Note(s) : (1) NET_TCP_OPT_CFG_TYPE_&&& #define values specifically chosen as ASCII representations of
 *               the TCP option configuration types.  Memory displays of TCP option configuration buffers
 *               will display the TCP option configuration TYPEs with their chosen ASCII names.
 *******************************************************************************************************/

typedef  enum  net_tcp_opt_type {
  NET_TCP_OPT_TYPE_NONE,
  NET_TCP_OPT_TYPE_MAX_SEG_SIZE,
  NET_TCP_OPT_TYPE_WIN_SCALE,                               // See 'net_tcp.h  Note #1c1'.
  NET_TCP_OPT_TYPE_SACK_PERMIT,                             // See 'net_tcp.h  Note #1c2'.
  NET_TCP_OPT_TYPE_SACK,                                    // See 'net_tcp.h  Note #1c2'.
  NET_TCP_OPT_TYPE_ECHO_REQ,                                // See 'net_tcp.h  Note #1c3'.
  NET_TCP_OPT_TYPE_ECHO_REPLY,                              // See 'net_tcp.h  Note #1c3'.
  NET_TCP_OPT_TYPE_TS                                       // See 'net_tcp.h  Note #1c4'.
} NET_TCP_OPT_TYPE;

/********************************************************************************************************
 *                                   TCP CONNECTION TIMEOUT DEFINES
 *
 * Note(s) : (1) (a) (1) RFC #1122, Section 4.2.2.13 'DISCUSSION' states that "the graceful close algorithm
 *                       of TCP requires that the connection state remain defined on (at least) one end of
 *                       the connection, for a timeout period of 2xMSL ... During this period, the (remote
 *                       socket, local socket) pair that defines the connection is busy and cannot be reused".
 *
 *                   (2) The following sections reiterate that the TIME-WAIT state timeout scalar is two
 *                       maximum segment lifetimes (2 MSL) :
 *
 *                       (A) RFC #793, Section 3.9 'Event Processing : SEGMENT ARRIVES :
 *                               Check Sequence Number : TIME-WAIT STATE'
 *                       (B) RFC #793, Section 3.9 'Event Processing : SEGMENT ARRIVES :
 *                               Check FIN Bit         : TIME-WAIT STATE'
 *
 *               (b) (1) RFC #793, Section 3.3 'Sequence Numbers : Knowing When to Keep Quiet' states that
 *                       "the Maximum Segment Lifetime (MSL) is ... to be 2 minutes.  This is an engineering
 *                       choice, and may be changed if experience indicates it is desirable to do so".
 *
 *                   (2) Microsoft Corporation's Windows XP defaults MSL to 15 seconds.
 *
 *           (2) RFC #1122, Section 4.2.3.6 states that a "connection ... [may be] dropped ... when no data
 *               or acknowledgement packets have been received for the connection within an interval ... [of]
 *               no less than two hours".
 *
 *           (3) (a) (1) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 18.6 'FIN_WAIT_2 State',
 *                       Page 246 states that "many Berkeley-derived implementations prevent [an] infinite
 *                       wait in the FIN_WAIT_2 state ... [by] mov[ing] the connection into the CLOSED state
 *                       ... if the connection is idle for 10 minutes plus 75 seconds".
 *
 *                   (2) Wright/Stevens, TCP/IP Illustrated, Volume 2, 3rd Printing, Section 25.1 'A FIN_WAIT_2
 *                       timer', Page 818 reiterates that "to avoid leaving a connection in the FIN_WAIT_2
 *                       state forever ... when a connection moves from the FIN_WAIT_1 state to the FIN_WAIT_2
 *                       state ... [the TCP connection's] timer is set to 10 minutes ... [and] 75 seconds, and
 *                       when it expires ... the connection is dropped".
 *
 *               (b) 'FIN_WAIT_2' abbreviated to 'FW2' for TCP FIN-WAIT-2 state constants to enforce
 *                       ANSI-compliance of 31-character symbol length uniqueness.
 *******************************************************************************************************/

//                                                                 Max seg timeout (see Note #1b) :
#define  NET_TCP_CONN_TIMEOUT_MAX_SEG_MIN_SEC    (0)                                // ... min  =  0 sec
#define  NET_TCP_CONN_TIMEOUT_MAX_SEG_MAX_SEC    (2  *  DEF_TIME_NBR_SEC_PER_MIN)   // ... max  =  2 min
#define  NET_TCP_CONN_TIMEOUT_MAX_SEG_DFLT_SEC   (15)                               // ... dflt = 15 sec (see Note #1b2)

#define  NET_TCP_CONN_TIMEOUT_MAX_SEG_SCALAR       2                                // ... scalar (see Note #1a)

//                                                                 TCP conn timeout :
#define  NET_TCP_CONN_TIMEOUT_IDLE_MIN_SEC         1                                // ... min  = 1s
#define  NET_TCP_CONN_TIMEOUT_IDLE_MAX_SEC       (12  *  DEF_TIME_NBR_SEC_PER_HR)   // ... max  = 12 hr
#define  NET_TCP_CONN_TIMEOUT_IDLE_DFLT_SEC      (2  *  DEF_TIME_NBR_SEC_PER_HR)    // ... dflt =  2 hr (see Note #2)

//                                                                 Dflt user timeout
#define  NET_TCP_CONN_TIMEOUT_USER_DFLT_SEC      (30  *  DEF_TIME_NBR_SEC_PER_MIN)  // ...      = 30 min

//                                                                 FIN-WAIT-2 timeout (see Note #3)
#define  NET_TCP_CONN_TIMEOUT_FW2_MIN_SEC        (0)
#define  NET_TCP_CONN_TIMEOUT_FW2_MAX_SEC        NET_TCP_CONN_TIMEOUT_IDLE_MAX_SEC
#define  NET_TCP_CONN_TIMEOUT_FW2_DFLT_SEC      ((10  *  DEF_TIME_NBR_SEC_PER_MIN) \
                                                 + 75)                              // ... dflt = 675 sec (see Note #3a)

/********************************************************************************************************
 *                               TCP CONNECTION KEEP-ALIVE & PROBE DEFINES
 *
 * Note(s) : (1) (a) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 23.2, Page 332 states
 *                   that "if there is no activity on a given connection for 2 hours, ... [TCP] sends" :
 *
 *                   (1) "a total of 10 ... probes," ...
 *                   (2) "75 seconds apart."
 *
 *                   See also 'NetTCP_TxConnKeepAlive()  Notes #2c1A2b, #2c2A2, & #2c3'.
 *
 *               (b) RFC #1122, Section 4.2.3.6 states that "a [keep-alive] probe segment ... may or may
 *                   not contain one garbage octet of data".
 *
 *                   See also 'NetTCP_TxConnProbe()  Note #2b2'.
 *
 *           (2) 'KEEP_ALIVE' abbreviated to 'KA' for TCP keep-alive constants to enforce ANSI-compliance
 *                   of 31-character symbol length uniqueness.
 *******************************************************************************************************/

#define  NET_TCP_TX_KA_TH_MIN                              2u
#define  NET_TCP_TX_KA_TH_MAX                             20u
#define  NET_TCP_TX_KA_TH_DFLT                            10u   // See Note #1a1.

#define  NET_TCP_TX_KA_RETRY_TIMEOUT_MIN_SEC               5
#define  NET_TCP_TX_KA_RETRY_TIMEOUT_MAX_SEC             300
#define  NET_TCP_TX_KA_RETRY_TIMEOUT_DFLT_SEC             75    // See Note #1a2.

#define  NET_TCP_TX_PROBE_DATA                          0x00u   // See Note #1b.

/********************************************************************************************************
 *                                       TCP CONNECTION QUEUE STATES
 *******************************************************************************************************/

#define  NET_TCP_RX_Q_STATE_NONE                           0u
#define  NET_TCP_RX_Q_STATE_CLOSED                       100u
#define  NET_TCP_RX_Q_STATE_CLOSING                      101u
#define  NET_TCP_RX_Q_STATE_SYNC                         110u
#define  NET_TCP_RX_Q_STATE_CONN                         111u

#define  NET_TCP_TX_Q_STATE_NONE                           0u
#define  NET_TCP_TX_Q_STATE_CLOSED                       200u
#define  NET_TCP_TX_Q_STATE_CLOSING                      201u
#define  NET_TCP_TX_Q_STATE_SYNC                         210u
#define  NET_TCP_TX_Q_STATE_CONN                         211u
#define  NET_TCP_TX_Q_STATE_SUSPEND                      215u
#define  NET_TCP_TX_Q_STATE_CLOSED_SUSPEND               220u
#define  NET_TCP_TX_Q_STATE_CLOSING_SUSPEND              221u

/********************************************************************************************************
 *                                       TCP CONNECTION CODE DEFINES
 *******************************************************************************************************/

#define  NET_TCP_CONN_RX_SEQ_NONE                         10u   // NO        rx'd TCP conn seq.
#define  NET_TCP_CONN_RX_SEQ_SYNC_INVALID                 11u   // Invalid   rx'd TCP conn sync.
#define  NET_TCP_CONN_RX_SEQ_SYNC                         12u   // Valid   rx'd TCP conn sync.
#define  NET_TCP_CONN_RX_SEQ_INVALID                      15u   // Invalid   rx'd TCP conn seq.
#define  NET_TCP_CONN_RX_SEQ_VALID                        16u   // Valid   rx'd TCP conn seq.
#define  NET_TCP_CONN_RX_SEQ_KEEP_ALIVE                   17u   // Valid   rx'd TCP conn seq for a keep-alive.

#define  NET_TCP_CONN_RX_ACK_NONE                         20u   // NO        rx'd TCP conn ack.
#define  NET_TCP_CONN_RX_ACK_INVALID                      21u   // Invalid   rx'd TCP conn ack.
#define  NET_TCP_CONN_RX_ACK_VALID                        22u   // Valid   rx'd TCP conn ack.
#define  NET_TCP_CONN_RX_ACK_DUP                          23u   // Duplicate rx'd TCP conn ack.
#define  NET_TCP_CONN_RX_ACK_PREV                         24u   // Prev'ly   rx'd TCP conn ack.
#define  NET_TCP_CONN_RX_ACK_OTW                          25u   // Outside The Window rx'd TCP conn ack.

#define  NET_TCP_CONN_RX_RESET_NONE                       30u   // NO        rx'd TCP conn reset.
#define  NET_TCP_CONN_RX_RESET_INVALID                    31u   // Invalid   rx'd TCP conn reset.
#define  NET_TCP_CONN_RX_RESET_VALID                      32u   // Valid   rx'd TCP conn reset.

#define  NET_TCP_CONN_RX_WIN_RESET                        50u   // Reset cur rx win size.
#define  NET_TCP_CONN_RX_WIN_SET                          51u   // Set   cur rx win size.
#define  NET_TCP_CONN_RX_WIN_INC                          52u   // Inc   cur rx win size.
#define  NET_TCP_CONN_RX_WIN_DEC                          53u   // Dec   cur rx win size.
#define  NET_TCP_CONN_RX_WIN_INIT                         54u   // Init the current rx window size.
#define  NET_TCP_CONN_RX_WIN_ZERO                         55u

#define  NET_TCP_CONN_TX_ACK_NONE                        120u   // Do NOT tx TCP conn ack.
#define  NET_TCP_CONN_TX_ACK                             121u   // Tx TCP conn ack.
#define  NET_TCP_CONN_TX_ACK_IMMED                       122u   // Tx TCP conn ack immed'ly.
#define  NET_TCP_CONN_TX_ACK_FAULT                       123u   // Tx TCP conn ack on fault.
#define  NET_TCP_CONN_TX_ACK_TIMEOUT                     124u   // Tx TCP conn ack on timeout.
#define  NET_TCP_CONN_TX_ACK_OTW                         125u   // Outside The Window tx'd TCP conn ack.

#define  NET_TCP_CONN_TX_RESET_NONE                      130u   // Do NOT tx TCP conn reset.
#define  NET_TCP_CONN_TX_RESET                           131u   // Tx TCP conn reset.
#define  NET_TCP_CONN_TX_RESET_FAULT                     133u   // Tx TCP conn reset on fault.

#define  NET_TCP_CONN_TX_WIN_RESET                       150u   // Reset   cur tx        win size.
#define  NET_TCP_CONN_TX_WIN_SET                         151u   // Set     cur tx        win size.
#define  NET_TCP_CONN_TX_WIN_INC                         152u   // Inc     cur tx        win size.
#define  NET_TCP_CONN_TX_WIN_DEC                         153u   // Dec     cur tx        win size.

#define  NET_TCP_CONN_TX_WIN_SEG_RXD                     155u   // Update  cur tx        win size based on rx'd seg.
#define  NET_TCP_CONN_TX_WIN_TIMEOUT                     156u   // Timeout cur tx        win size.
#define  NET_TCP_CONN_TX_WIN_REMOTE_UPDATE               157u   // Update  cur tx remote win size.

#define  NET_TCP_CONN_TX_WIN_CONG_INC_SLOW_START         160u   // Inc         tx cong   win size based on slow start.
#define  NET_TCP_CONN_TX_WIN_CONG_INC_CONG_AVOID         161u   // Inc         tx cong   win size based on cong avoid.
#define  NET_TCP_CONN_TX_WIN_CONG_INC_REM                162u   // Inc     rem tx cong   win size.
#define  NET_TCP_CONN_TX_WIN_CONG_SET_SLOW_START         165u   // Set         tx cong   win size to slow start    th.
#define  NET_TCP_CONN_TX_WIN_CONG_SET_FAST_RECOVERY      166u   // Set         tx cong   win size to fast recovery th.
#define  NET_TCP_CONN_TX_WIN_CONG_SET_TIMEOUT            167u   // Set         tx cong   win size to timeout       th.

#define  NET_TCP_CONN_TX_RTT_RESET                       170u   // Reset    tx RTT     ctrls.
#define  NET_TCP_CONN_TX_RTT_RTO_INIT                    171u   // Init     tx RTT/RTO ctrls.
#define  NET_TCP_CONN_TX_RTT_RTO_CALC                    175u   // Calc     tx RTT/RTO ctrls.
#define  NET_TCP_CONN_TX_RTO_BACKOFF                     176u   // Back-off tx     RTO ctrls.

/********************************************************************************************************
 *                                   TCP CONGESTION CONTROL DEFINES
 *
 * Note(s) : (1) See the following RFC's for TCP Congestion Control summary :
 *
 *               (a) (1) RFC #2001                       TCP Congestion Control
 *                   (2) RFC #2581                           Slow Start                      See Note #2
 *                                                           Congestion Avoidance            See Note #3
 *                                                           Fast Re-transmit                See Note #4
 *                                                           Fast Recovery                   See Note #4
 *
 *               (b) (1) RFC # 813                       Silly Window Syndrome (SWS)         See Note #5
 *                   (2) RFC #1122
 *                       (A) Section 4.2.2.14
 *                       (B) Section 4.2.3.3             Receive  SWS (RSWS)                 See Note #5a
 *                       (C) Section 4.2.3.4             Transmit SWS (TSWS)                 See Note #5b
 *
 *               (c) (1) RFC # 896                       Nagle's Algorithm
 *                   (2) RFC #1122
 *                       (A) Section 4.2.2.14
 *                       (B) Section 4.2.3.4
 *
 *               (d) (1) RFC # 813, Section 5            Delayed TCP Acknowledgments         See Note #6
 *                   (2) RFC #1122, Section 4.2.3.2
 *                   (3) RFC #2581, Section 4.2
 *
 *               (e) RFC #1122
 *                   (1) Section 4.2.2.2                 TCP Segment PUSH
 *                   (2) Section 4.2.2.17                Probing Zero Windows /
 *                                                       TCP  Persist Timer
 *                   (3) Section 4.2.3.5                 TCP  Connection Failures            See  Note #7
 *                   (4) Section 4.2.3.9                 ICMP Error Message   /              See 'net_tcp.h
 *                                                       TCP  Congestion Recovery                 Note #1e'
 *
 *           (2) (a) RFC #2581, Section 3.1 states that "the initial value of ssthresh [slow start threshold]
 *                   MAY be arbitrarily high (for example, some implementations use the size of the advertised
 *                   window)".
 *
 *                   (1) This amends RFC #2001, Section 2.1 which previously stated that "initialization for
 *                       a given connection sets ... ssthresh to 65535 bytes".
 *
 *               (b) RFC #2581, Section 3.1, states that "when a TCP sender detects segment loss using the
 *                   retransmission timer, the value of ssthresh MUST be set to no more than" :
 *
 *                       (3) ssthresh  =  max (FlightSize / 2, 2 * SMSS)
 *
 *                               where
 *                                   (A) FlightSize          Amount of outstanding data in the network
 *                                   (B) SMSS                Sender Maximum Segment Size -- the size of the
 *                                                               largest segment that the sender can transmit
 *
 *               (c) 'SLOW_START_TH' (i.e. 'Slow Start Threshold') abbreviated to 'SST' for some TCP control
 *                       constants to enforce ANSI-compliance of 31-character symbol length uniqueness.
 *
 *               See also 'net_tcp.c  NetTCP_TxConnWinSizeHandlerCongCtrl()  Notes #2c1A1b & #2c2A1b'.
 *
 *           (3) RFC #2581, Section 3.1 states that :
 *
 *               (a) "The initial value of cwnd [congestion window], MUST be less than or equal to 2*SMSS
 *                       bytes and MUST NOT be more than 2 segments."
 *
 *                   (1) This amends RFC #2001, Section 2.1 which previously stated that "initialization
 *                       for a given connection sets cwnd to one segment".
 *
 *               (b) "During congestion avoidance, cwnd MUST NOT be increased by more than ... 1 full-sized
 *                       segment per RTT [TCP segment round-trip time]."
 *
 *               (c) "Upon a timeout cwnd MUST be set to no more than ... 1 full-sized segment."
 *
 *               See also 'net_tcp.c  NetTCP_TxConnWinSizeHandlerCongCtrl()  Notes #2c1A1a, #2c1A4b2,
 *                                                                                   #2c2A1a, & #2c2A4'.
 *
 *           (4) RFC #2581, Section 3.2 states that :
 *
 *               (a) "The fast retransmit algorithm uses the arrival of 3 duplicate ACKs ... as an indication
 *                       ... [to perform] a retransmission".
 *
 *               (b) "When the third duplicate ACK is received" :
 *
 *                   (1) "Set ssthresh to no more than the value given in equation 3" (see Note #2b).
 *
 *                   (2) "Set cwnd to ssthresh plus 3*SMSS.  This artificially 'inflates' the congestion
 *                           window by the number of segments (three) that have left the network."
 *
 *               (c) "For each additional duplicate ACK received, increment cwnd by SMSS."
 *
 *               See also 'net_tcp.c  NetTCP_TxConnWinSizeHandlerCongCtrl()  Notes #2c1B & #2c2B'.
 *
 *           (5) (a) RFC #1122, Section 4.2.3.3 states that "the suggested SWS avoidance algorithm for the
 *                   receiver is ... to avoid advancing the right window edge RCV.NXT+RCV.WND ... until the
 *                   reduction satisfies" :
 *
 *                       (1) RCV.BUFF - RCV.USER - RCV.WND  >=  min(Fr * RCV.BUFF, Eff.snd.MSS)
 *
 *                               where
 *                                   (A) RCV.BUFF        Total receive buffer space
 *                                   (B) RCV.USER        Data  received but not yet consumed
 *                                   (C) RCV.WND         Space advertised to sender
 *                                   (D) Fr              Fraction whose recommended value is 1/2
 *                                   (E) Eff.snd.MSS     Effective send MSS for the connection
 *
 *                   See also 'net_tcp.c  NetTCP_RxConnWinSizeCfgUpdateTh()  Note #1'.
 *                           & 'net_tcp.c  NetTCP_RxConnWinSizeHandler()      Note #3'.
 *
 *               (b) RFC #1122, Section 4.2.3.4 states that "the sender's SWS avoidance algorithm is ...
 *                   [to] send data" :
 *
 *                   (3) "If at least a fraction Fs of the maximum window can be sent ... Fs is a fraction
 *                           whose recommended value is 1/2."
 *
 *                   (4) "If data is PUSHed and the override timeout occurs ... The override timeout should
 *                           be in the range 0.1 - 1.0 seconds."
 *
 *                   See also 'net_tcp.c  NetTCP_TxConnTxQ()  Note #7b2'.
 *
 *           (6) The following sections state that "a TCP SHOULD implement a delayed ACK" :
 *
 *                       (A) RFC # 813, Section 5
 *                       (B) RFC #1122, Section 4.2.3.2
 *                       (C) RFC #2581, Section 4.2
 *
 *               (a) (1) (A) RFC #1122, Section 4.2.3.2 states that "in a stream of full-sized segments there
 *                           SHOULD be an ACK for at least every second segment".
 *
 *                       (B) RFC #2581, Section 4.2 reiterates that "an ACK SHOULD be generated for at least
 *                           every second full-sized segment".
 *
 *                   (2) However, RFC #2581, Section 4.2 states that "an implementation is deemed to comply
 *                       with this requirement ... by acknowledging at least every second segment, regardless
 *                       of size".
 *
 *               (b) (1) (A) RFC #1122, Section 4.2.3.2 states that "an ACK should not be excessively delayed;
 *                           in particular, the delay MUST be less than 0.5 seconds".
 *
 *                       (B) RFC #2581, Section 4.2 reiterates that "an ACK ... MUST be generated within 500 ms
 *                           of the arrival of the first unacknowledged packet".
 *
 *                   (2) However, Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 19.3, Page 265
 *                       states that "most implementations use a 200-ms delay".
 *
 *               See also 'net_tcp.c  NetTCP_TxConnAck()  Note #6'.
 *
 *           (7) RFC #1122, Section 4.2.3.5 states that "excessive retransmission of the same segment by TCP
 *               indicates some failure of the remote host or the Internet path".
 *
 *               (a) "The following procedure MUST be used to handle excessive retransmissions of data segments" :
 *
 *                   (a) "There are ... thresholds ... measuring the amount of retransmission that has occurred
 *                           for the same segment ... measured in time units or as a count of retransmissions."
 *
 *                   (c) "When the number of transmissions of the same segment reaches a threshold ... close the
 *                           connection."
 *
 *                   (d) "An application MUST be able to set the [threshold] value ... for a particular connection.
 *                           For example, an interactive application might set [the threshold value] to 'infinity'."
 *
 *                   See also 'net_tcp.c  NetTCP_TxConnReTxQ()  Note #3'.
 *
 *               (b) (1) (A) "The value of ... [the threshold] SHOULD correspond to at least 100 seconds."
 *
 *                       (B) Therefore, the minimum threshold value is calculated as follows :
 *
 *                           (1)    [                                                                ]
 *                                   [ Exponential * (Exponential ^ i)  ,  when < Maximum Exponential ]
 *                                   [   Scalar          Base                        Timeout Value    ]         Minimum
 *                       Summation     [                                                                ]  >=  Retransmission
 *                   i = 0 --> i = N  [ Maximum Exponential                                            ]        Threshold
 *                                   [    Timeout Value                 ,  otherwise                  ]
 *                                   [                                                                ]
 *
 *                                   [                                    ]
 *                                   [  3 * (2 ^ i)  ,  when < 64 seconds ]
 *                       Summation     [                                    ]  >=  100 seconds
 *                   i = 0 --> i = N  [ 64 seconds    ,  otherwise         ]
 *                                   [                                    ]
 *
 *                                                                       N  >=  4.11
 *
 *                           (2)                                         N   =  5
 *
 *                                   where
 *                                           N                           Minimum Excessive Retransmission Threshold
 *                                                                           (in number of retransmissions)
 *                                           Exponential Scalar          Exponential Scalar = 3 (see 'TCP ROUND-TRIP
 *                                                                           TIME (RTT) / RE-TRANSMIT TIMEOUT (RTO)
 *                                                                           DEFINES  Note #3a1A1b')
 *                                           Exponential Base            Exponential Base   = 2 (see 'TCP ROUND-TRIP
 *                                                                           TIME (RTT) / RE-TRANSMIT TIMEOUT (RTO)
 *                                                                           DEFINES  Note #3b2')
 *                                           Minimum Retransmission      Minimum Excessive Retransmission Threshold
 *                                                   Threshold                 (in seconds; see Note #7b1A)
 *
 *                   (2) (A) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 21.2, Page 299
 *                           states that "on most implementations this total timeout value is not tunable
 *                           ... and its default is ... the more common 9 minutes".
 *
 *                       (B) Therefore, the default threshold value is calculated as follows :
 *
 *                           (1)    [                                                                ]
 *                                   [ Exponential * (Exponential ^ i)  ,  when < Maximum Exponential ]
 *                                   [   Scalar          Base                        Timeout Value    ]          Common
 *                       Summation     [                                                                ]  >=  Retransmission
 *                   i = 0 --> i = M  [ Maximum Exponential                                            ]        Threshold
 *                                   [    Timeout Value                 ,  otherwise                  ]
 *                                   [                                                                ]
 *
 *                                   [                                    ]
 *                                   [  3 * (2 ^ i)  ,  when < 64 seconds ]
 *                       Summation     [                                    ]  >=  9 minutes
 *                   i = 0 --> i = M  [ 64 seconds    ,  otherwise         ]
 *                                   [                                    ]
 *
 *                                                                       M  >=  10.98
 *
 *                           (2)                                         M   =  11
 *
 *                                   where
 *                                           M                           Default Excessive Retransmission Threshold
 *                                                                           (in number of retransmissions)
 *                                           Exponential Scalar          Exponential Scalar = 3 (see 'TCP ROUND-TRIP
 *                                                                           TIME (RTT) / RE-TRANSMIT TIMEOUT (RTO)
 *                                                                           DEFINES  Note #3a1A1b')
 *                                           Exponential Base            Exponential Base   = 2 (see 'TCP ROUND-TRIP
 *                                                                           TIME (RTT) / RE-TRANSMIT TIMEOUT (RTO)
 *                                                                           DEFINES  Note #3b2')
 *                                           Maximum Exponential         Maximum Exponential Timeout Value (see
 *                                               Timeout Value                'TCP ROUND-TRIP TIME (RTT) / RE-TRANSMIT
 *                                                                           TIMEOUT (RTO) DEFINES  Note #3b1B')
 *                                           Common Retransmission       Common Default Retransmission Threshold
 *                                                   Threshold                  (in minutes; see Note #7b2A)
 *******************************************************************************************************/

//                                                                 See Note #2b3.
#define  NET_TCP_SST_UNACKD_DATA_NUMER                     1
#define  NET_TCP_SST_UNACKD_DATA_DENOM                     2

#define  NET_TCP_SST_MSS_SCALAR                            2

#define  NET_TCP_CONG_WIN_MSS_SCALAR_INIT                  2                    // See Note  #3a.
#define  NET_TCP_CONG_WIN_MSS_SCALAR_INC                   1                    // See Notes #3b & #4c.
#define  NET_TCP_CONG_WIN_MSS_SCALAR_TIMEOUT               1                    // See Note  #3c.

#define  NET_TCP_FAST_RE_TX_ACK_DUP_TH                     3                    // See Note #4a.
                                                                                // See Note #4b2.
#define  NET_TCP_FAST_RECOVERY_MSS_SCALAR                NET_TCP_FAST_RE_TX_ACK_DUP_TH

//                                                                 See Note #5a1D.
#define  NET_TCP_RX_SILLY_WIN_NUMER                        1
#define  NET_TCP_RX_SILLY_WIN_DENOM                        2

//                                                                 See Note #5b3.
#define  NET_TCP_TX_SILLY_WIN_NUMER                        1
#define  NET_TCP_TX_SILLY_WIN_DENOM                        2
//                                                                 See Note #5b4.
#define  NET_TCP_TX_SILLY_WIN_TIMEOUT_MIN_MS             100
#define  NET_TCP_TX_SILLY_WIN_TIMEOUT_MAX_MS            1000
#define  NET_TCP_TX_SILLY_WIN_TIMEOUT_DFLT_MS            NET_TCP_TX_SILLY_WIN_TIMEOUT_MAX_MS

#define  NET_TCP_ACK_DLY_CNT_TH                            2                    // See Note #6a.

#define  NET_TCP_ACK_DLY_TIME_MIN_MS                       0
#define  NET_TCP_ACK_DLY_TIME_MAX_MS                     500                    // See Note #6b1.
#define  NET_TCP_ACK_DLY_TIME_DFLT_MS                    200                    // See Note #6b2.

#define  NET_TCP_RE_TX_TH_MIN                              5                    // See Note #7b1B2.
#define  NET_TCP_RE_TX_TH_MAX      (DEF_INT_16U_MAX_VAL -  1)                   // See Note #7ad.
#define  NET_TCP_RE_TX_TH_DFLT                            12                    // See Note #7b2B2. Dflt val set to ...
                                                                                // .. M + 1 for validation purposes.

/********************************************************************************************************
 *                       TCP ROUND-TRIP TIME (RTT) / RE-TRANSMIT TIMEOUT (RTO) DEFINES
 *
 * Note(s) : (1) See the following RFC's for TCP Round-Trip Times (RTT) & Re-transmit Timeout (RTO) summary :
 *
 *               (a) RFC #2988                               TCP Round-Trip Time Calculations (RTT) /
 *               (b) RFC #1122                               TCP Retransmission Timeout       (RTO)
 *                   (1) Section 4.2.2.15
 *                   (2) Section 4.2.3.1
 *               (c) RFC # 793, Section 3.7 'Retransmission Timeout'
 *               (d) Jacobson/Karels, "Congestion Avoidance and Control"
 *               (e) Karn/Partridge,  "Improving Round-Trip Time Estimates in Reliable Transport Protocols"
 *
 *           (2) (a) RFC #793, Section 3.7 'Data Communication : Retransmission Timeout' states that
 *                   "because of the variability of the networks that compose an internetwork system
 *                   and the wide range of uses of TCP connections the retransmission timeout must be
 *                   dynamically determined.  One procedure for determining a retransmission time out
 *                   is given here as an illustration".
 *
 *               (b) However, RFC #1122, Section 4.2.2.15 states that "the algorithm suggested in
 *                   RFC-793 for calculating the retransmission timeout is now known to be inadequate"
 *                   & Section 4.2.3.1 states that "a host TCP MUST implement Karn's algorithm and
 *                   Jacobson's algorithm for computing the retransmission timeout".
 *
 *               (c) Further, RFC #2988, Section 1 states that "this document codifies the algorithm
 *                   for setting the RTO ... expands on the discussion in section 4.2.3.1 of RFC 1122
 *                       ... [but] does not alter the ... retransmission ... behavior outlined in RFC 2581".
 *
 *           (3) (a) (1) (A) (1) RFC #1122, Section 4.2.3.1 states that "the following values SHOULD be
 *                               used to initialize the estimation parameters for a new connection" :
 *
 *                                   (a) RTT = 0 seconds
 *                                   (b) RTO = 3 seconds
 *
 *                                           where
 *                                                   RTO                 Retransmission Timeout
 *                                                   RTT                 Round-Trip Time
 *
 *                               (A) RFC #2988, Section 2.1 reiterates that "until a round-trip time (RTT)
 *                                   measurement has been made ... the sender SHOULD set RTO <- 3 seconds".
 *
 *                               (B) Furthermore, RFC #1122, Section 4.2.3.1.(b) states that "the smoothed
 *                                   variance is to be initialized to the value that will result in" these
 *                                   values.
 *
 *                                   Since computing the RTT retransmit timer      is given by the following
 *                                   equation (see RFC #2988, Section 2.3) :
 *
 *                                       (1) RTO     =  RTT_Avg + (RTT_RTO_Gain * RTT_Dev)
 *
 *                                   then the RTT deviation, or smoothed variance, is given by the following
 *                                   equation :
 *
 *                                       (2) RTT_Dev = (RTO - RTT_Avg) / RTT_RTO_Gain
 *
 *                                               where
 *                                                       RTT_Avg                 RTT Average
 *                                                       RTT_Dev                 RTT Deviation
 *                                                       RTT_RTO_Gain            RTT-RTO Gain
 *
 *                           (2) However, since RFC #2988, Section 2.2 amends the RFC #1122, Section
 *                               4.2.3.1 RTT initialization; the smoothed RTT average & deviation do
 *                               NOT truly require explicit initialization.  Nonetheless, these RTT
 *                               values are initialized to conform with RFC #1122, Section 4.2.3.1.
 *
 *                           See also 'net_tcp.c  NetTCP_TxConnRTT_Init()  Note #1'
 *                                   & 'net_tcp.c  NetTCP_TxConnRTO_Init()  Note #2'.
 *
 *                       (B) RFC #2988, Section 2 states that "the rules governing the computation of SRTT
 *                           (smoothed round-trip time), RTTVAR RTTVAR (round-trip time variation), and RTO
 *                           are as follows" :
 *
 *                           (a) RFC #2988, Section 2.2 states that for "the first RTT measurement R ...
 *                               the host MUST set" :
 *
 *                                   (1) SRTT    <-  R
 *                                   (2) RTTVAR  <-  R/2
 *                                   (3) RTO     <-  SRTT  +  max(G, K * RTTVAR)
 *
 *                                               where
 *                                                   SRTT                    RTT Smoothed Average
 *                                                   RTTVAR                  RTT Variance/Deviation
 *                                                   RTO                         Retransmission Timeout
 *                                                   R                       RTT First Measurement
 *                                   (4)             R   = R * 1             RTT First Average   Gain
 *                                   (5)             R/2 = R * 1/2           RTT First Deviation Gain
 *                                                   G                       RTT Clock Granularity (resolution)
 *                                   (6)             K = 4                   RTT-RTO Gain
 *
 *                           (b) RFC #2988, Section 2.3 states that for "subsequent RTT measurement R' ...
 *                               a host MUST set" :
 *
 *                                   (1) RTTVAR  <-  (1 - beta ) * RTTVAR  +  beta  * |SRTT - R'|
 *                                   (2) SRTT    <-  (1 - alpha) * SRTT    +  alpha * R'
 *                                   (3) RTO     <-  SRTT  +  max(G, K * RTTVAR)
 *
 *                                               where
 *                                                   SRTT                    RTT Smoothed Average
 *                                                   RTTVAR                  RTT Variance/Deviation
 *                                                   RTO                         Retransmission Timeout
 *                                                   R'                      RTT Subsequent Measurement(s)
 *                                   (4)             alpha = 1/8             RTT-Average   Gain
 *                                   (5)             beta  = 1/4             RTT-Deviation Gain
 *                                                   G                       RTT Clock Granularity (resolution)
 *                                   (6)             K = 4                   RTT-RTO Gain
 *
 *                           See also 'net_tcp.c  NetTCP_TxConnRTT_RTO_Calc()  Note #2a1'.
 *
 *                   (2) (A) Jacobson/Karels, "Congestion Avoidance and Control", Appendix A.2 states that RTT
 *                           calculations "should be done in integer arithmetic".  RFC #2988, Section 2.3 adds
 *                           that RTT calculations "SHOULD be computed using ... 1/8 and ... 1/4" gains (see
 *                           Notes #3a1Bb4 & #3a1Bb5).
 *
 *                       (B) (a) Therefore, the RTT integer calculations SHOULD be scaled by 8, the least
 *                               common denominator for the RTT fractional gains.
 *
 *                           (b) However, ONLY the RTT measurements, average, & deviation are scaled.  RTT
 *                               gains MUST NOT be scaled since scaling the multiplicative gains incorrectly
 *                               exponentiates the RTT calculations :
 *
 *                                   (1) (RTT_Gain * RTT)  =  (RTT_Gain * RTT) * (RTT_Scale / RTT_Scale)
 *
 *                                   (2)                   =  (RTT_Gain * RTT  * RTT_Scale) / RTT_Scale
 *
 *                                   (3)                   =  [RTT_Gain * (RTT * RTT_Scale)] / RTT_Scale
 *
 *                                   (4)                  !=  [(RTT_Gain * RTT_Scale) * (RTT * RTT_Scale)] / RTT_Scale
 *
 *                       See also 'net_tcp.c  NetTCP_TxConnRTT_RTO_Calc()  Note #2a3'.
 *
 *               (b) (1) (A) (1) (a) (1) RFC #1122, Section 4.2.3.1 states that "the recommended ... RTO ...
 *                                       lower bound ... SHOULD be measured in fractions of a second".
 *
 *                                   (2) RFC #2988, Section 2.4 amends that "whenever RTO is computed, if it
 *                                       is less than 1 second then the RTO SHOULD be rounded up to 1 second".
 *
 *                               (b) However, most modern TCP/IP implementations limit RTO's lower bound
 *                                   to a range of 30-200 milliseconds.
 *
 *                           (2) RFC #2988, Section 4 states that "there is no requirement for the clock
 *                               granularity G used for computing RTT measurements ... However, if the
 *                               K*RTTVAR term in the RTO calculation equals zero, the variance term MUST
 *                               be rounded to G seconds".
 *
 *                               See also Notes #3a1Ba3 & #3a1Bb3.
 *
 *                           See also 'net_tcp.c  NetTCP_TxConnRTT_RTO_Calc()  Note #2b1A'.
 *
 *                       (B) (1) RFC #2988, Section 2.4 adds that "a maximum value MAY be placed on RTO
 *                               provided it is at least 60 seconds".
 *
 *                           (2) RFC #1122, Section 4.2.3.1 states that "the recommended ... RTO ... upper
 *                               bound should be 2*MSL".
 *
 *                           (3) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 21.2, Page 299
 *                               states that "the timeout value is doubled for each retransmission, with an
 *                               upper limit of 64 seconds".
 *
 *                           See also 'net_tcp.c  NetTCP_TxConnRTO_CfgMaxTimeout()  Note #1',
 *                                       'net_tcp.c  NetTCP_TxConnRTO_CalcBackOff()    Note #1b',
 *                                   & 'net_tcp.c  NetTCP_TxConnRTT_RTO_Calc()       Note #2b1B'.
 *
 *                   (2) RFC #1122, Section 4.2.3.1 states that an "implementation MUST also include
 *                       'exponential backoff' for successive RTO values for the same segment".
 *
 *                       (A) (1) RFC #2988, Section 5.5 states that "when the retransmission timer expires
 *                               ... the host MUST set RTO <- RTO * 2 ('back off the timer')".
 *
 *                           (2) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 21.2, Page 299
 *                               reiterates that "this doubling is called an 'exponential backoff'".
 *
 *                       (B) Thus the TCP retransmission timer exponential back-off scalar value is 2.
 *
 *                       See also 'net_tcp.c  NetTCP_TxConnRTO_CalcBackOff()  Note #1a'.
 *******************************************************************************************************/

#define  NET_TCP_TX_RTT_AVG_INIT_SEC                       0    // RTT avg init = 0 sec     (see Note #3a1A1a).
#define  NET_TCP_TX_RTT_RTO_INIT_SEC                       3    // RTT RTO init = 3 sec     (see Note #3a1A1b).

//                                                                 RTT avg init gain = 1    (see Note #3a1Ba4).
#define  NET_TCP_TX_RTT_GAIN_AVG_INIT                      1
#define  NET_TCP_TX_RTT_GAIN_AVG_INIT_NUMER                1
#define  NET_TCP_TX_RTT_GAIN_AVG_INIT_DENOM                1
//                                                                 RTT dev init gain = 1/2  (see Note #3a1Ba5).
#define  NET_TCP_TX_RTT_GAIN_DEV_INIT_NUMER                1
#define  NET_TCP_TX_RTT_GAIN_DEV_INIT_DENOM                2

//                                                                 RTT avg      gain = 1/8  (see Note #3a1Bb4).
#define  NET_TCP_TX_RTT_GAIN_AVG_NUMER                     1
#define  NET_TCP_TX_RTT_GAIN_AVG_DENOM                     8
//                                                                 RTT dev      gain = 1/4  (see Note #3a1Bb5).
#define  NET_TCP_TX_RTT_GAIN_DEV_NUMER                     1
#define  NET_TCP_TX_RTT_GAIN_DEV_DENOM                     4
//                                                                 RTT dev RTO  gain = 4    (see Note #3a1Bb6).
#define  NET_TCP_TX_RTT_GAIN_RTO                           4
#define  NET_TCP_TX_RTT_GAIN_RTO_NUMER                     4
#define  NET_TCP_TX_RTT_GAIN_RTO_DENOM                     1

#define  NET_TCP_TX_RTT_SCALE                              8    // RTT vals int-scaled-by-8 (see Note #3a2Ba).
#define  NET_TCP_TX_RTT_MS_SCALE                       ((NET_TCP_TX_RTT_MS_SCALED)DEF_TIME_NBR_mS_PER_SEC * NET_TCP_TX_RTT_SCALE)

#define  NET_TCP_TX_RTT_AVG_INIT_SEC_SCALED             (NET_TCP_TX_RTT_AVG_INIT_SEC * NET_TCP_TX_RTT_SCALE)
#define  NET_TCP_TX_RTT_RTO_INIT_SEC_SCALED             (NET_TCP_TX_RTT_RTO_INIT_SEC * NET_TCP_TX_RTT_SCALE)

//                                                                 RTT dev init scaled      (see Note #3a1A1B2).
#define  NET_TCP_TX_RTT_DEV_INIT_SEC_SCALED           (((NET_TCP_TX_RTT_RTO_INIT_SEC - NET_TCP_TX_RTT_AVG_INIT_SEC) * NET_TCP_TX_RTT_SCALE) \
                                                       / NET_TCP_TX_RTT_GAIN_RTO)

#define  NET_TCP_TX_RTT_AVG_INIT_MS_SCALED              (NET_TCP_TX_RTT_AVG_INIT_SEC_SCALED * (NET_TCP_TX_RTT_MS_SCALED)DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TCP_TX_RTT_DEV_INIT_MS_SCALED              (NET_TCP_TX_RTT_DEV_INIT_SEC_SCALED * (NET_TCP_TX_RTT_MS_SCALED)DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TCP_TX_RTT_RTO_INIT_MS_SCALED              (NET_TCP_TX_RTT_RTO_INIT_SEC_SCALED * (NET_TCP_TX_RTT_MS_SCALED)DEF_TIME_NBR_mS_PER_SEC)

#define  NET_TCP_TX_RTT_NONE                               0
#define  NET_TCP_TX_RTT_TS_NONE                            0

#define  NET_TCP_TX_RTO_NONE                               0

#define  NET_TCP_TX_RTO_MIN_TIMEOUT_MS                   100    // RTO min timeout      = 100 ms   (see Note #3b1A1).
#define  NET_TCP_TX_RTO_MIN_TIMEOUT_MS_SCALED           (NET_TCP_TX_RTO_MIN_TIMEOUT_MS  * NET_TCP_TX_RTT_SCALE)

#define  NET_TCP_TX_RTO_MAX_TIMEOUT_MIN_SEC               60    // RTO max timeout min  =  60 sec  (see Note #3b1B1).
                                                                // RTO max timeout max  =   2 MSL  (see Note #3b1B2).
#define  NET_TCP_TX_RTO_MAX_TIMEOUT_MAX_SEC              NET_TCP_CONN_TIMEOUT_MAX_SEG_MAX_SEC
#define  NET_TCP_TX_RTO_MAX_TIMEOUT_DFLT_SEC              64    // RTO max timeout dflt =  64 sec  (see Note #3b1B3).

#define  NET_TCP_TX_RTO_TIMEOUT_BACKOFF_SCALAR             2    // RTO exponential back-off scalar (see Note #3b2B).

#define  NET_TCP_TX_RTT_RTO_STATE_NONE                     0u
#define  NET_TCP_TX_RTT_RTO_STATE_INIT                    10u
#define  NET_TCP_TX_RTT_RTO_STATE_RESET                   11u
#define  NET_TCP_TX_RTT_RTO_STATE_CALC                    20u
#define  NET_TCP_TX_RTT_RTO_STATE_RE_TX                   30u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       TCP GENERAL RETURN STATUS
 *******************************************************************************************************/
typedef  enum  net_tcp_status {
  NET_TCP_STATUS_NONE,
  NET_TCP_STATUS_VALID,
  NET_TCP_STATUS_INVALID,
  NET_TCP_STATUS_DUP,
  NET_TCP_STATUS_DATA
} NET_TCP_STATUS;

/********************************************************************************************************
 *                                           TCP DATA STATUS
 *******************************************************************************************************/

typedef  enum  net_tcp_data_status {
  NET_TCP_DATA_STATUS_NONE,
  NET_TCP_DATA_STATUS_VALID,
  NET_TCP_DATA_STATUS_INVALID,
  NET_TCP_DATA_STATUS_DUP
} NET_TCP_DATA_STATUS;

/********************************************************************************************************
 *                                           TCP ACK STATUS
 *******************************************************************************************************/

typedef  enum  net_tcp_ack_status {
  NET_TCP_ACK_STATUS_NONE,
  NET_TCP_ACK_STATUS_VALID,
  NET_TCP_ACK_STATUS_INVALID,
  NET_TCP_ACK_STATUS_DLYD,
  NET_TCP_ACK_STATUS_PREV_TXD
} NET_TCP_ACK_STATUS;

/********************************************************************************************************
 *                                       TCP PORT NUMBER DATA TYPE
 *******************************************************************************************************/

typedef  NET_PORT_NBR NET_TCP_PORT_NBR;                         // Defines TCP port nbr size.

/********************************************************************************************************
 *                                       TCP SEQUENCE NUMBER DATA TYPE
 *
 * Note(s) : (1) 'NET_TCP_SEQ_NBR'  pre-defined in 'net_type.h' PRIOR to all other network modules that
 *                   require TCP sequence number data type(s).
 *******************************************************************************************************/

#if 0                                                           // See Note #1.
typedef  CPU_INT32U NET_TCP_SEQ_NBR;
#endif

/********************************************************************************************************
 *                                       TCP SEGMENT SIZE DATA TYPE
 *
 * Note(s) : (1) 'NET_TCP_SEG_SIZE' pre-defined in 'net_type.h' PRIOR to all other network modules that
 *                   require TCP segment size data type(s).
 *******************************************************************************************************/

#if 0                                                           // See Note #1.
typedef  CPU_INT16U NET_TCP_SEG_SIZE;
#endif

/********************************************************************************************************
 *                                       TCP WINDOW SIZE DATA TYPE
 *
 * Note(s) : (1) 'NET_TCP_WIN_SIZE' pre-defined in 'net_type.h' PRIOR to all other network modules that
 *                   require TCP window size data type(s).
 *******************************************************************************************************/

#if 0                                                           // See Note #1.
typedef  CPU_INT16U NET_TCP_WIN_SIZE;
#endif

/********************************************************************************************************
 *                                   TCP RECEIVE QUEUE STATE DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_TCP_RX_Q_STATE;

/********************************************************************************************************
 *                                   TCP TRANSMIT QUEUE STATE DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_TCP_TX_Q_STATE;

/********************************************************************************************************
 *                                   TCP RTT MEASUREMENT DATA TYPES
 *
 * Note(s) : (1) RTT measurement data types MUST be defined to ensure sufficient range for both scaled
 *               & un-scaled, signed & unsigned time measurement values.
 *
 *           (2) 'NET_TCP_TX_RTT_TS_MS' pre-defined in 'net_type.h' PRIOR to all other network modules
 *                   that require TCP Transmit Round-Trip Time data type(s).
 *******************************************************************************************************/

typedef  CPU_INT08U NET_TCP_TX_RTT_STATE;

//                                                                 See Note #1.
typedef  NET_TS_MS NET_TCP_TX_RTT_MS;
typedef  CPU_INT32S NET_TCP_TX_RTT_MS_SCALED;

#if 0                                                           // See Note #2.
typedef  NET_TS_MS NET_TCP_TX_RTT_TS_MS;
#endif
typedef  CPU_INT32S NET_TCP_TX_RTT_TS_MS_SCALED;

/********************************************************************************************************
 *                                   TCP CONNECTION QUANTITY DATA TYPE
 *
 * Note(s) : (1) See also 'TCP CONNECTION IDENTIFICATION DATA TYPE  Note #1'.
 *******************************************************************************************************/

typedef  CPU_INT16S NET_TCP_CONN_QTY;                           // Defines max qty of TCP conns to support.

/********************************************************************************************************
 *                               TCP CONNECTION IDENTIFICATION DATA TYPE
 *
 * Note(s) : (1) (a) NET_TCP_CONN_NBR_MAX  SHOULD be #define'd based on 'NET_TCP_CONN_QTY' data type declared.
 *
 *               (b) However, since TCP connection handle identifiers are data-typed as 16-bit signed integers;
 *                   the maximum number of valid TCP connection identifiers, & therefore the maximum number
 *                   of valid TCP connections, is the total number of non-negative values that 16-bit signed
 *                   integers support.
 *******************************************************************************************************/

#define  NET_TCP_CONN_NBR_MIN                              1
#define  NET_TCP_CONN_NBR_MAX            DEF_INT_16S_MAX_VAL    // See Note #1.

#define  NET_TCP_CONN_ID_NONE                             -1
#define  NET_TCP_CONN_ID_MIN                               0
#define  NET_TCP_CONN_ID_MAX       (NET_TCP_NBR_CONN - 1)

/********************************************************************************************************
 *                                           TCP FLAGS DATA TYPES
 *
 * Note(s) : (1) 'NET_TCP_FLAGS'/'NET_TCP_HDR_FLAGS' pre-defined in 'net_type.h' PRIOR to all other network
 *                   modules that require TCP flags data types.
 *******************************************************************************************************/

#if 0                                                           // See Note #1.
typedef  NET_FLAGS NET_TCP_FLAGS;
typedef  CPU_INT16U NET_TCP_HDR_FLAGS;
#endif

/********************************************************************************************************
 *                                               TCP HEADER
 *
 * Note(s) : (1) See RFC #793, Section 3.1 for TCP segment header format.
 *
 *           (2) TCP Header Length & Flags are encoded in the thirteenth & fourteenth octets of a TCP header
 *               as follows :
 *
 *                           15 14 13 12 11 10 9 8 7 6  5   4   3   2   1   0
 *                       ---------------------------------------------------
 *                       |  H L E N  |   Z E R O   |URG|ACK|PSH|RST|SYN|FIN|
 *                       ---------------------------------------------------
 *
 *                   where
 *                           HLEN        TCP Headers' length in 32-bit words; MUST be at least 5 (20-octet header)
 *                                           & MUST be less than or equal to 15 (60-octet header)
 *                           ZERO        MUST be zero; i.e. '000000'
 *                           URG         Segment Urgent Data Pointer Valid (see Note #3) :
 *                                           '0' - Urgent Data                NOT available (default)
 *                                           '1' - Urgent Data                    available
 *                           ACK         Segment Acknowledgement Number Valid :
 *                                           '0' - Acknowledgement Number     NOT available
 *                                           '1' - Acknowledgement Number         available
 *                           PSH         Segment Push Data Request :
 *                                           '0' - Push Data                  NOT requested (default)
 *                                           '1' - Push Data                      requested
 *                           RST         Connection Reset Request :
 *                                           '0' - Connection Reset           NOT requested (default)
 *                                           '1' - Connection Reset               requested
 *                           SYN         Connection Synchronization Request :
 *                                           '0' - Connection Synchronization NOT requested (default)
 *                                           '1' - Connection Synchronization     requested
 *                           FIN         Connection Close Request :
 *                                           '0' - Connection Close           NOT requested (default)
 *                                           '1' - Connection Close               requested
 *
 *           (3) Urgent pointer & data NOT supported (see 'net_tcp.h  Note #1b').
 *******************************************************************************************************/

//                                                                 ------------------- NET TCP HDR --------------------
typedef  struct  net_tcp_hdr {
  NET_TCP_PORT_NBR  PortSrc;                                    // TCP seg  src  port.
  NET_TCP_PORT_NBR  PortDest;                                   // TCP seg  dest port.
  NET_TCP_SEQ_NBR   SeqNbr;                                     // TCP seg  seq  nbr.
  NET_TCP_SEQ_NBR   AckNbr;                                     // TCP seg  ack  nbr.
  NET_TCP_HDR_FLAGS HdrLen_Flags;                               // TCP seg  hdr  len/flags (see Note #2).
  NET_TCP_WIN_SIZE  WinSize;                                    // TCP conn win size.
  NET_CHK_SUM       ChkSum;                                     // TCP seg  chk sum.
  CPU_INT16U        UrgentPtr;                                  // TCP seg  urgent ptr     (see Note #3).
  NET_TCP_OPT_SIZE  Opts[NET_TCP_HDR_OPT_NBR_MAX];              // TCP seg  opts (if any).
} NET_TCP_HDR;

/********************************************************************************************************
 *                                           TCP PSEUDO-HEADER
 *
 * Note(s) : (1) See RFC #793, Section 3.1 'Header Format : Checksum' for TCP pseudo-header format.
 *******************************************************************************************************/

//                                                                 ---------------- NET TCP PSEUDO-HDR ----------------
typedef  struct  net_tcp_pseudo_hdr {
  NET_IPv4_ADDR AddrSrc;                                        // TCP seg src  addr.
  NET_IPv4_ADDR AddrDest;                                       // TCP seg dest addr.
  CPU_INT08U    Zero;                                           // Field MUST be zero'd; i.e. ALL bits clr'd.
  CPU_INT08U    Protocol;                                       // TCP protocol.
  CPU_INT16U    TotLen;                                         // TCP seg tot len.
} NET_TCP_PSEUDO_HDR;

/********************************************************************************************************
 *                               TCP HEADER OPTION CONFIGURATION DATA TYPES
 *******************************************************************************************************/

/********************************************************************************************************
 *                       TCP MAXIMUM SEGMENT SIZE OPTION CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef  struct  net_tcp_opt_cfg_max_seg_size {
  NET_TCP_OPT_TYPE Type;
  NET_TCP_SEG_SIZE MaxSegSize;                                  // TCP max seg size.
  void             *NextOptPtr;                                 // Ptr to next TCP opt cfg.
} NET_TCP_OPT_CFG_MAX_SEG_SIZE;

/********************************************************************************************************
 *                                   APP CALLBACK FUNCTION DATA TYPE
 *******************************************************************************************************/

typedef  CPU_BOOLEAN (*NET_TCP_APP_LISTEN_Q_IS_AVAIL_FNCT)(NET_CONN_ID conn_id_app);

typedef  void (*NET_TCP_APP_POST_FNCT)             (NET_CONN_ID conn_id_app);

/********************************************************************************************************
 *                                       TCP CONNECTION DATA TYPE
 *
 *                                       NET_TCP_CONN
 *                                   |-------------|
 *                                   |  Conn Type  |
 *                                   |-------------|      Next TCP
 *                                   |      O----------> Connection
 *                                   |-------------|
 *                                   |     IDs     |
 *                                   |-------------|
 *                                   | Conn State  |
 *                                   |-------------|
 *                                   |   Rx Ctrl   |
 *                                   |   Rx Q's    |
 *                                   |-------------|
 *                                   |   Tx Ctrl   |
 *                                   |   Tx Q's    |
 *                                   |   Tx Tmrs   |
 *                                   |-------------|
 *                                   |   Timeouts  |
 *                                   |-------------|
 *                                   |    Flags    |
 *                                   |-------------|
 *
 * Note(s) : None
 *******************************************************************************************************/

//                                                                 ------------------- NET TCP CONN -------------------
typedef  struct  net_tcp_conn NET_TCP_CONN;

struct  net_tcp_conn {
  NET_TCP_CONN                       *NextPtr;                          // Ptr to NEXT conn.

  NET_TCP_CONN_ID                    ID;                                // TCP conn id.
  NET_CONN_ID                        ID_Conn;                           // Conn id.

  NET_TCP_CONN_STATE                 ConnState;                         // TCP conn state.

  CPU_INT08U                         ConnCloseCode;                     // Conn close code.
  CPU_BOOLEAN                        ConnCloseAppFlag;                  // Conn close app           flag.
  CPU_BOOLEAN                        ConnCloseTimeoutFaultFlag;         // Conn close timeout fault flag.

  //                                                               Max seg sizes (in octets) :
  NET_TCP_SEG_SIZE                   MaxSegSizeLocalCfgd;               // Local max seg size cfg'd.
  NET_TCP_SEG_SIZE                   MaxSegSizeLocalActual;             // Local max seg size actual/advertised to remote host.
  NET_TCP_SEG_SIZE                   MaxSegSizeRemote;                  // Max seg size        advertised by remote host.
  NET_TCP_SEG_SIZE                   MaxSegSizeConn;                    // Max seg size calc'd for conn.

  NET_TCP_SEQ_NBR                    RxSeqNbrSync;                      // Sync  rx seq nbr.
  NET_TCP_SEQ_NBR                    RxSeqNbrNext;                      // Next  rx seq nbr.
  NET_TCP_SEQ_NBR                    RxSeqNbrLast;                      // Last  rx seq nbr.
  NET_TCP_SEQ_NBR                    RxSeqNbrClose;                     // Close rx seq nbr.
                                                                        // Rx win sizes (in octets) :
  NET_TCP_WIN_SIZE                   RxWinSizeCfgd;                     // Rx win size cfg'd.
  NET_TCP_WIN_SIZE                   RxWinSizeCalcd;                    // Rx win size calc'd.
  NET_TCP_WIN_SIZE                   RxWinSizeActual;                   // Rx win size actual/advertised to remote host.

  NET_TCP_WIN_SIZE                   RxWinSizeUpdateTh;                 // Rx win size update th (rx silly win ctrl).

  NET_TCP_RX_Q_STATE                 RxQ_State;                         // Rx Q state.

  KAL_SEM_HANDLE                     RxQ_SignalObj;
  CPU_INT32U                         RxQ_SignalTimeout_ms;

  NET_BUF                            *RxQ_Transport_Head;               // Head of transport rx Q to q TCP pkts until seq'd.
  NET_BUF                            *RxQ_Transport_Tail;               // Tail of transport rx Q to q TCP pkts until seq'd.

  NET_BUF                            *RxQ_App_Head;                     // Head of app data  rx Q to q TCP pkts until app rd(s).
  NET_BUF                            *RxQ_App_Tail;                     // Tail of app data  rx Q to q TCP pkts until app rd(s).

  NET_TCP_SEQ_NBR                    TxSeqNbrSync;                      // Sync  tx seq nbr.
  NET_TCP_SEQ_NBR                    TxSeqNbrNext;                      // Next  tx seq nbr.
  NET_TCP_SEQ_NBR                    TxSeqNbrNextQ;                     // Next  tx seq nbr NOT yet tx'd; i.e. in tx Q.
  NET_TCP_SEQ_NBR                    TxSeqNbrUnReTxd;                   // Last  tx seq nbr NOT re- tx'd.
  NET_TCP_SEQ_NBR                    TxSeqNbrUnAckd;                    // Last  tx seq nbr NOT yet ack'd.
  NET_TCP_SEQ_NBR                    TxSeqNbrUnAckdPrev;                // Prev  tx seq nbr NOT yet ack'd.
  NET_TCP_SEQ_NBR                    TxSeqNbrUnAckdAlignDelta;          // Tx seq nbr NOT yet ack'd alignment delta.
  NET_TCP_SEQ_NBR                    TxSeqNbrLast;                      // Last  tx seq nbr.
  NET_TCP_SEQ_NBR                    TxSeqNbrClose;                     // Close tx seq nbr.

  NET_TCP_SEQ_NBR                    TxWinUpdateSeqNbr;                 // Last  rx seq nbr  that updated remote rx win size.
  NET_TCP_SEQ_NBR                    TxWinUpdateAckNbr;                 // Last  rx ack nbr  that updated remote rx win size.
  NET_TCP_WIN_SIZE                   TxWinUpdateWinSize;                // Last  rx win size that updated remote rx win size.

  NET_PKT_CTR                        TxWinRxdAckDupCtr;                 // Rx'd duplicate ack ctr.
  NET_TCP_SEQ_NBR                    TxWinRxdLastSeqNbr;                // Last  rx'd seq nbr.
  NET_TCP_SEQ_NBR                    TxWinRxdLastAckNbr;                // Last  rx'd ack nbr.
  NET_TCP_WIN_SIZE                   TxWinRxdLastWinSize;               // Last  rx'd win size.
                                                                        // Tx  win sizes (in octets) :
  NET_TCP_WIN_SIZE                   TxWinSizeCfgd;                     // Tx  win size cfg'd.
  NET_TCP_WIN_SIZE                   TxWinSizeCfgdRem;                  // Tx  win size rem'ing to q tx app data.

  NET_TCP_WIN_SIZE                   TxWinSizeRemote;                   // Win size advertised by  remote host.
  NET_TCP_WIN_SIZE                   TxWinSizeRemoteMax;                // Max win size advertised by  remote host.
  NET_TCP_WIN_SIZE                   TxWinSizeRemoteActual;             // Tx  win size actual     for remote host.
  NET_TCP_WIN_SIZE                   TxWinSizeRemoteRem;                // Tx  win size rem'ing    for remote host.

  NET_TCP_WIN_SIZE                   TxWinSizeSlowStartTh;              // Tx  win size slow start      th.
  NET_TCP_WIN_SIZE                   TxWinSizeSlowStartThInit;          // Tx  win size slow start init th.
  NET_TCP_WIN_SIZE                   TxWinSizeCongInit;                 // Tx  win size        init'd by cong ctrl.
  NET_TCP_WIN_SIZE                   TxWinSizeCongCalcdActual;          // Tx  win size actual calc'd    cong ctrl.
  NET_TCP_WIN_SIZE                   TxWinSizeCongCalcdCur;             // Tx  win size cur    calc'd    cong ctrl.
  NET_TCP_WIN_SIZE                   TxWinSizeCongRem;                  // Tx  win size rem'ing       by cong ctrl.

  NET_TCP_WIN_SIZE                   TxWinSizeAvail;                    // Tx  win size avail.

  NET_TCP_WIN_SIZE                   TxWinSizeMinTh;                    // Tx  win size min th (tx silly win ctrl).
  CPU_BOOLEAN                        TxWinSizeNagleEn;                  // Tx  win size Nagle alg en.

  NET_TCP_TIMEOUT_MS                 TxWinSillyWinTimeout_ms;           // Tx  silly win timeout (in ms).
  NET_TCP_TIMEOUT_MS                 TxWinZeroWinTimeout_ms;            // Tx  zero  win timeout (in ms).

  NET_TCP_TIMEOUT_MS                 TxAckDlyTimeout_ms;                // Tx  ack dly timeout (in ms).
  NET_TMR                            *TxAckDlyTmr;                      // Tx  ack dly tmr.
  CPU_INT16U                         TxAckDlyCnt;                       // Tx  ack dly cur cnt.

  CPU_BOOLEAN                        TxAckImmedRxdPushEn;               // Tx  immed ack for rx'd TCP push seg(s) en.

  CPU_BOOLEAN                        TxKeepAliveEn;                     // Tx  keep-alive alg en.
  NET_PKT_CTR                        TxKeepAliveCtr;                    // Nbr of keep-alives tx'd ctr.
  NET_PKT_CTR                        TxKeepAliveTh;                     // Nbr of keep-alives tx'd th.
  NET_TCP_TIMEOUT_SEC                TxKeepAliveRetryTimeout_sec;       // Tx  keep-alive retry timeout (in secs).

  NET_PKT_CTR                        TxSegReTxTh;                       // Nbr of seg re-tx's th.

  NET_TCP_TX_RTT_MS_SCALED           TxRTT_Avg_ms_scaled;               // RTT smoothed avg      (in int-scaled ms   ).
  NET_TCP_TX_RTT_MS_SCALED           TxRTT_Dev_ms_scaled;               // RTT mean     dev      (in int-scaled ms   ).
  NET_TCP_TX_RTT_MS_SCALED           TxRTT_RTO_ms_scaled;               // RTT re-tx timeout     (in int-scaled ms   ).
  NET_TCP_TX_RTT_MS_SCALED           TxRTT_RTO_Max_ms_scaled;           // RTT re-tx timeout max (in int-scaled ms   ).
#if 0
  NET_TCP_TIMEOUT_MS                 TxRTT_Avg_ms;                      // RTT smoothed avg      (in            ms   ).
  NET_TCP_TIMEOUT_MS                 TxRTT_Dev_ms;                      // RTT mean     dev      (in            ms   ).
#endif
  NET_TCP_TIMEOUT_MS                 TxRTT_RTO_ms;                      // RTT re-tx timeout     (in            ms   ).
  NET_TCP_TIMEOUT_MS                 TxRTT_RTO_Max_ms;                  // RTT re-tx timeout max (in            ms   ).
#if 0
  NET_TCP_TIMEOUT_SEC                TxRTT_RTO_sec;                     // RTT re-tx timeout     (in            secs ).
#endif
  NET_TCP_TIMEOUT_SEC                TxRTT_RTO_Max_sec;                 // RTT re-tx timeout max (in            secs ).
  NET_TCP_TX_RTT_STATE               TxRTT_RTO_State;                   // RTT-RTO state.

  NET_TCP_TX_Q_STATE                 TxQ_State;                         // Tx Q state.

  KAL_SEM_HANDLE                     TxQ_SignalObj;
  CPU_INT32U                         TxQ_SignalTimeout_ms;

  NET_BUF                            *TxQ_Head;                         // Head of Q of TCP pkts to    tx.
  NET_BUF                            *TxQ_Tail;                         // Tail of Q of TCP pkts to    tx.
  NET_TMR                            *TxQ_IdleTmr;                      // Tx Q idle              tmr.
  NET_TMR                            *TxQ_SillyWinTmr;                  // Tx Q silly win persist tmr.
  NET_TMR                            *TxQ_ZeroWinTmr;                   // Tx Q zero  win persist tmr.

  NET_BUF                            *ReTxQ_Head;                       // Head of Q of TCP pkts to re-tx on timeout.
  NET_BUF                            *ReTxQ_Tail;                       // Tail of Q of TCP pkts to re-tx on timeout.
  NET_TMR                            *ReTxQ_Tmr;                        // Re-tx Q         tmr.

  NET_TMR                            *TimeoutTmr;                       // TCP conn    timeout tmr.

  NET_TCP_TIMEOUT_SEC                TimeoutConn_sec;                   // TCP conn    timeout (in        secs ).
  NET_TCP_TIMEOUT_SEC                TimeoutUser_sec;                   // TCP user    timeout (in        secs ).
  NET_TCP_TIMEOUT_SEC                TimeoutMaxSeg_sec;                 // TCP max seg timeout (in        secs ).

  NET_TCP_FLAGS                      Flags;                             // TCP conn flags.

  NET_TCP_APP_LISTEN_Q_IS_AVAIL_FNCT FnctAppListenQ_IsAvail;            // Is connection should be accepted callback function.
  NET_TCP_APP_POST_FNCT              FnctAppPostRx;                     // Notify RX Data                   callback function.
  NET_TCP_APP_POST_FNCT              FnctAppPostTx;                     // Notify TX Q  is available        callback function.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern NET_TCP_CONN *NetTCP_ConnTbl;

#if (NET_STAT_POOL_TCP_CONN_EN == DEF_ENABLED)
extern NET_STAT_POOL NetTCP_ConnPoolStat;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NET_TCP_TX_GET_SEQ_NBR()
 *
 * @brief    Get next TCP transmit sequence number.
 *
 * @param    seq_nbr     Variable that will receive the returned TCP transmit sequence number.
 *
 *                       @note         (1) (a) RFC #1122, Section 4.2.2.9 states that "a TCP MUST use the specified clock-driven
 *                       selection of initial sequence numbers".
 *
 *                           - (b) However; Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 18.2, Page 232
 *                       states that for "4.4BSD (and most Berkeley-derived implementations)" ... :
 *
 *                           - (1) "When the system is initialized the initial send sequence number is initialized
 *                       to 1.  This practice violates the Host Requirements RFC [#1122]."
 *
 *                           - (2) "This variable is then incremented by 64,000" ... :
 *                           - (A) "every half-second" and ...
 *                           - (B) "additionally, each time a connection is established."
 *
 *                           - (c) As a compromise :
 *
 *                           - (1) The developer is required to configure the TCP transmit initial sequence number
 *                       counter (see 'net_util.c  NetUtil_InitSeqNbrGet()  Note #1').
 *
 *                           - (2) However, the TCP initial transmit sequence number is incremented by a fixed
 *                       value each time a new TCP connection is established (see also Note #2b2B).
 *
 *                       @note         (2) Return TCP sequence number is NOT converted from host-order to network-order.
 *
 *                       @note         (3) Adding NET_DBG_CFG_TEST_TCP in net_cfg.h allow to remove the increment value to the
 *                       TCP sequence number, this can be use for debug purpose.
 *******************************************************************************************************/

#ifndef NET_DBG_CFG_TEST_TCP

#define  NET_TCP_TX_GET_SEQ_NBR(seq_nbr)        do { NET_UTIL_VAL_COPY_32(&(seq_nbr), &NetTCP_TxSeqNbrCtr); \
                                                     NetTCP_TxSeqNbrCtr += NET_TCP_TX_SEQ_NBR_CTR_INC;      } while (0)

#else

#define  NET_TCP_TX_GET_SEQ_NBR(seq_nbr)        do { NET_UTIL_VAL_COPY_32(&(seq_nbr), &NetTCP_TxSeqNbrCtr); } while (0)

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetTCP_Init(MEM_SEG  *p_mem_seg,
                 RTOS_ERR *p_err);

//                                                                 ----------- RX FNCTS -----------
void NetTCP_Rx(NET_BUF  *pbuf,
               RTOS_ERR *p_err);

CPU_INT16U NetTCP_RxAppData(NET_TCP_CONN_ID conn_id_tcp,
                            void            *p_data_buf,
                            CPU_INT16U      data_buf_len,
                            NET_TCP_FLAGS   flags,
                            RTOS_ERR        *p_err);

//                                                                 ----------- TX FNCTS -----------
void NetTCP_TxConnReq(NET_TCP_CONN_ID conn_id_tcp,
                      RTOS_ERR        *p_err);

void NetTCP_TxConnReqClose(NET_TCP_CONN_ID conn_id_tcp,
                           CPU_INT08U      conn_close_code,
                           RTOS_ERR        *p_err);

CPU_INT16U NetTCP_TxConnAppData(NET_TCP_CONN_ID conn_id_tcp,
                                void            *p_data,
                                CPU_INT16U      data_len,
                                NET_TCP_FLAGS   flags,
                                RTOS_ERR        *p_err);

//                                                                 ---- TCP RX/TX STATUS FNCTS ----
CPU_BOOLEAN NetTCP_ConnIsAvailRx(NET_TCP_CONN_ID conn_id_tcp,
                                 RTOS_ERR        *p_err);

CPU_BOOLEAN NetTCP_ConnIsRdyTx(NET_TCP_CONN_ID conn_id_tcp,
                               RTOS_ERR        *p_err);

//                                                                 ------ TCP CONN API FNCTS ------
NET_TCP_CONN_ID NetTCP_ConnGet(NET_TCP_APP_POST_FNCT fnct_app_post_rx,
                               NET_TCP_APP_POST_FNCT fnct_app_post_tx,
                               RTOS_ERR              *p_err);

void NetTCP_ConnFree(NET_TCP_CONN_ID conn_id_tcp);

void NetTCP_ConnCloseFromConn(NET_TCP_CONN_ID conn_id_tcp);

void NetTCP_ConnSetID_Conn(NET_TCP_CONN_ID conn_id_tcp,
                           NET_CONN_ID     conn_id);

void NetTCP_ConnSetStateListen(NET_TCP_CONN_ID                    conn_id_tcp,
                               NET_TCP_APP_LISTEN_Q_IS_AVAIL_FNCT tcp_app_listen_is_avail_fnct,
                               RTOS_ERR                           *p_err);

#ifndef  NET_TCP_CFG_OLD_WINDOW_MGMT_EN
void NetTCP_ConnAppAcceptRdySignal(NET_CONN_ID     conn_id_parent,
                                   NET_TCP_CONN_ID conn_id_tcp,
                                   RTOS_ERR        *p_err);
#endif

//                                                                 ------ TCP CONN CFG FNCTS ------
CPU_BOOLEAN NetTCP_ConnCfgIdleTimeoutHandler(NET_TCP_CONN_ID     conn_id_tcp,
                                             NET_TCP_TIMEOUT_SEC timeout_sec);

CPU_BOOLEAN NetTCP_ConnCfgMaxSegSizeLocalHandler(NET_TCP_CONN_ID  conn_id_tcp,
                                                 NET_TCP_SEG_SIZE max_seg_size,
                                                 RTOS_ERR         *p_err);

CPU_BOOLEAN NetTCP_ConnCfgRxWinSizeHandler(NET_TCP_CONN_ID  conn_id_tcp,
                                           NET_TCP_WIN_SIZE win_size,
                                           RTOS_ERR         *p_err);

CPU_BOOLEAN NetTCP_ConnCfgTxWinSizeHandler(NET_TCP_CONN_ID  conn_id_tcp,
                                           NET_TCP_WIN_SIZE win_size,
                                           RTOS_ERR         *p_err);

CPU_BOOLEAN NetTCP_ConnCfgTxNagleEnHandler(NET_TCP_CONN_ID conn_id_tcp,
                                           CPU_BOOLEAN     nagle_en);

CPU_BOOLEAN NetTCP_ConnCfgTxAckDlyTimeoutHandler(NET_TCP_CONN_ID    conn_id_tcp,
                                                 NET_TCP_TIMEOUT_MS timeout_ms);

CPU_BOOLEAN NetTCP_ConnCfgTxKeepAliveEnHandler(NET_TCP_CONN_ID conn_id_tcp,
                                               CPU_BOOLEAN     keep_alive_en);

CPU_BOOLEAN NetTCP_ConnCfgTxKeepAliveThHandler(NET_TCP_CONN_ID conn_id_tcp,
                                               NET_PKT_CTR     nbr_max_keep_alive);

CPU_BOOLEAN NetTCP_ConnCfgTxKeepAliveRetryHandler(NET_TCP_CONN_ID     conn_id_tcp,
                                                  NET_TCP_TIMEOUT_SEC timeout_sec);

CPU_BOOLEAN NetTCP_ConnCfgReTxMaxThHandler(NET_TCP_CONN_ID conn_id_tcp,
                                           NET_PKT_CTR     nbr_max_re_tx);

CPU_BOOLEAN NetTCP_ConnCfgReTxMaxTimeoutHandler(NET_TCP_CONN_ID     conn_id_tcp,
                                                NET_TCP_TIMEOUT_SEC timeout_sec);

CPU_BOOLEAN NetTCP_ConnCfgMSL_TimeoutHandler(NET_TCP_CONN_ID     conn_id_tcp,
                                             NET_TCP_TIMEOUT_SEC msl_timeout_sec);

//                                                                 ---- TCP CONN STATUS FNCTS -----
CPU_BOOLEAN NetTCP_ConnIsUsed(NET_TCP_CONN_ID conn_id_tcp);

//                                                                 --------- TCP Q FNCTS ----------
//                                                                 Clr      TCP conn rx Q signal.
void NetTCP_RxQ_Clr(NET_TCP_CONN_ID conn_id_tcp);

//                                                                 Wait for TCP conn rx Q signal.
void NetTCP_RxQ_Wait(NET_TCP_CONN_ID conn_id_tcp,
                     RTOS_ERR        *p_err);

//                                                                 Signal   TCP conn rx Q.
void NetTCP_RxQ_Signal(NET_TCP_CONN_ID conn_id_tcp);

//                                                                 Abort    TCP conn rx Q.
void NetTCP_RxQ_Abort(NET_TCP_CONN_ID conn_id_tcp);

//                                                                 Set dflt TCP conn rx Q timeout.
void NetTCP_RxQ_TimeoutDflt(NET_TCP_CONN_ID conn_id_tcp);

//                                                                 Set      TCP conn rx Q timeout.
void NetTCP_RxQ_TimeoutSet(NET_TCP_CONN_ID conn_id_tcp,
                           CPU_INT32U      timeout_ms);

//                                                                 Get      TCP conn rx Q timeout.
CPU_INT32U NetTCP_RxQ_TimeoutGet_ms(NET_TCP_CONN_ID conn_id_tcp);

//                                                                 Clr      TCP conn tx Q signal.
void NetTCP_TxQ_Clr(NET_TCP_CONN_ID conn_id_tcp);

//                                                                 Wait for TCP conn tx Q signal.
void NetTCP_TxQ_Wait(NET_TCP_CONN_ID conn_id_tcp,
                     RTOS_ERR        *p_err);

//                                                                 Signal   TCP conn tx Q.
void NetTCP_TxQ_Signal(NET_TCP_CONN_ID conn_id_tcp);

//                                                                 Abort    TCP conn tx Q.
void NetTCP_TxQ_Abort(NET_TCP_CONN_ID conn_id_tcp);

//                                                                 Set dflt TCP conn tx Q timeout.
void NetTCP_TxQ_TimeoutDflt(NET_TCP_CONN_ID conn_id_tcp);

//                                                                 Set      TCP conn tx Q timeout.
void NetTCP_TxQ_TimeoutSet(NET_TCP_CONN_ID conn_id_tcp,
                           CPU_INT32U      timeout_ms);

//                                                                 Get      TCP conn tx Q timeout.
CPU_INT32U NetTCP_TxQ_TimeoutGet_ms(NET_TCP_CONN_ID conn_id_tcp);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_TCP_NBR_CONN
#error  "NET_TCP_NBR_CONN                           not #define'd in 'net_cfg.h'"
#error  "                                         [MUST be  >= NET_TCP_CONN_NBR_MIN]"
#error  "                                         [     &&  <= NET_TCP_CONN_NBR_MAX]"

#elif   (DEF_CHK_VAL(NET_TCP_NBR_CONN,     \
                     NET_TCP_CONN_NBR_MIN, \
                     NET_TCP_CONN_NBR_MAX) != DEF_OK)
#error  "NET_TCP_NBR_CONN                     illegally #define'd in 'net_cfg.h'"
#error  "                                         [MUST be  >= NET_TCP_CONN_NBR_MIN]"
#error  "                                         [     &&  <= NET_TCP_CONN_NBR_MAX]"
#endif

#ifndef  NET_TCP_DFLT_RX_WIN_SIZE_OCTET
#error  "NET_TCP_DFLT_RX_WIN_SIZE_OCTET                  not #define'd in 'net_cfg.h'"
#error  "                                         [MUST be  >= NET_TCP_WIN_SIZE_MIN]"
#error  "                                         [     &&  <= NET_TCP_WIN_SIZE_MAX]"

#elif   (DEF_CHK_VAL(NET_TCP_DFLT_RX_WIN_SIZE_OCTET, \
                     NET_TCP_WIN_SIZE_MIN,           \
                     NET_TCP_WIN_SIZE_MAX) != DEF_OK)
#error  "NET_TCP_DFLT_RX_WIN_SIZE_OCTET            illegally #define'd in 'net_cfg.h'"
#error  "                                         [MUST be  >= NET_TCP_WIN_SIZE_MIN]"
#error  "                                         [     &&  <= NET_TCP_WIN_SIZE_MAX]"
#endif

#ifndef  NET_TCP_DFLT_TX_WIN_SIZE_OCTET
#error  "NET_TCP_DFLT_TX_WIN_SIZE_OCTET                  not #define'd in 'net_cfg.h'"
#error  "                                         [MUST be  >= NET_TCP_WIN_SIZE_MIN]"
#error  "                                         [     &&  <= NET_TCP_WIN_SIZE_MAX]"

#elif   (DEF_CHK_VAL(NET_TCP_DFLT_TX_WIN_SIZE_OCTET, \
                     NET_TCP_WIN_SIZE_MIN,           \
                     NET_TCP_WIN_SIZE_MAX) != DEF_OK)
#error  "NET_TCP_DFLT_TX_WIN_SIZE_OCTET            illegally #define'd in 'net_cfg.h'"
#error  "                                         [MUST be  >= NET_TCP_WIN_SIZE_MIN]"
#error  "                                         [     &&  <= NET_TCP_WIN_SIZE_MAX]"
#endif

#ifndef  NET_TCP_DFLT_TIMEOUT_CONN_MAX_SEG_SEC
#error  "NET_TCP_DFLT_TIMEOUT_CONN_MAX_SEG_SEC           not #define'd in 'net_cfg.h'                "
#error  "                                         [MUST be  >= NET_TCP_CONN_TIMEOUT_MAX_SEG_MIN_SEC]"
#error  "                                         [     &&  <= NET_TCP_CONN_TIMEOUT_MAX_SEG_MAX_SEC]"

#elif   (DEF_CHK_VAL(NET_TCP_DFLT_TIMEOUT_CONN_MAX_SEG_SEC, \
                     NET_TCP_CONN_TIMEOUT_MAX_SEG_MIN_SEC,  \
                     NET_TCP_CONN_TIMEOUT_MAX_SEG_MAX_SEC) != DEF_OK)
#error  "NET_TCP_DFLT_TIMEOUT_CONN_MAX_SEG_SEC     illegally #define'd in 'net_cfg.h'                "
#error  "                                         [MUST be  >= NET_TCP_CONN_TIMEOUT_MAX_SEG_MIN_SEC]"
#error  "                                         [     &&  <= NET_TCP_CONN_TIMEOUT_MAX_SEG_MAX_SEC]"
#endif

#ifndef  NET_TCP_DFLT_TIMEOUT_CONN_FIN_WAIT_2_SEC
#error  "NET_TCP_DFLT_TIMEOUT_CONN_FIN_WAIT_2_SEC        not #define'd in 'net_cfg.h'            "
#error  "                                         [MUST be  >= NET_TCP_CONN_TIMEOUT_FW2_MIN_SEC]"
#error  "                                         [     &&  <= NET_TCP_CONN_TIMEOUT_FW2_MAX_SEC]"

#elif   (DEF_CHK_VAL(NET_TCP_DFLT_TIMEOUT_CONN_FIN_WAIT_2_SEC, \
                     NET_TCP_CONN_TIMEOUT_FW2_MIN_SEC,         \
                     NET_TCP_CONN_TIMEOUT_FW2_MAX_SEC) != DEF_OK)
#error  "NET_TCP_DFLT_TIMEOUT_CONN_FIN_WAIT_2_SEC  illegally #define'd in 'net_cfg.h'            "
#error  "                                         [MUST be  >= NET_TCP_CONN_TIMEOUT_FW2_MIN_SEC]"
#error  "                                         [     &&  <= NET_TCP_CONN_TIMEOUT_FW2_MAX_SEC]"
#endif

#ifndef  NET_TCP_DFLT_TIMEOUT_CONN_ACK_DLY_MS
#error  "NET_TCP_DFLT_TIMEOUT_CONN_ACK_DLY_MS            not #define'd in 'net_cfg.h'       "
#error  "                                         [MUST be  >= NET_TCP_ACK_DLY_TIME_MIN_MS]"
#error  "                                         [     &&  <= NET_TCP_ACK_DLY_TIME_MAX_MS]"

#elif   (DEF_CHK_VAL(NET_TCP_DFLT_TIMEOUT_CONN_ACK_DLY_MS, \
                     NET_TCP_ACK_DLY_TIME_MIN_MS,          \
                     NET_TCP_ACK_DLY_TIME_MAX_MS) != DEF_OK)
#error  "NET_TCP_DFLT_TIMEOUT_CONN_ACK_DLY_MS      illegally #define'd in 'net_cfg.h'       "
#error  "                                         [MUST be  >= NET_TCP_ACK_DLY_TIME_MIN_MS]"
#error  "                                         [     &&  <= NET_TCP_ACK_DLY_TIME_MAX_MS]"
#endif

#ifndef  NET_TCP_DFLT_TIMEOUT_CONN_RX_Q_MS
#error  "NET_TCP_DFLT_TIMEOUT_CONN_RX_Q_MS               not #define'd in 'net_cfg.h' "
#error  "                                         [MUST be  >= NET_TIMEOUT_MIN_mS]   "
#error  "                                         [     &&  <= NET_TIMEOUT_MAX_mS]   "
#error  "                                         [     ||  == NET_TMR_TIME_INFINITE]"

#elif  ((DEF_CHK_VAL(NET_TCP_DFLT_TIMEOUT_CONN_RX_Q_MS,                   \
                     NET_TIMEOUT_MIN_mS,                                  \
                     NET_TIMEOUT_MAX_mS) != DEF_OK)                       \
  && (!((DEF_CHK_VAL_MIN(NET_TCP_DFLT_TIMEOUT_CONN_RX_Q_MS, 0) == DEF_OK) \
  && (NET_TCP_DFLT_TIMEOUT_CONN_RX_Q_MS == NET_TMR_TIME_INFINITE))))
#error  "NET_TCP_DFLT_TIMEOUT_CONN_RX_Q_MS         illegally #define'd in 'net_cfg.h' "
#error  "                                         [MUST be  >= NET_TIMEOUT_MIN_mS]   "
#error  "                                         [     &&  <= NET_TIMEOUT_MAX_mS]   "
#error  "                                         [     ||  == NET_TMR_TIME_INFINITE]"
#endif

#ifndef  NET_TCP_DFLT_TIMEOUT_CONN_TX_Q_MS
#error  "NET_TCP_DFLT_TIMEOUT_CONN_TX_Q_MS               not #define'd in 'net_cfg.h' "
#error  "                                         [MUST be  >= NET_TIMEOUT_MIN_mS]   "
#error  "                                         [     &&  <= NET_TIMEOUT_MAX_mS]   "
#error  "                                         [     ||  == NET_TMR_TIME_INFINITE]"

#elif  ((DEF_CHK_VAL(NET_TCP_DFLT_TIMEOUT_CONN_TX_Q_MS,                   \
                     NET_TIMEOUT_MIN_mS,                                  \
                     NET_TIMEOUT_MAX_mS) != DEF_OK)                       \
  && (!((DEF_CHK_VAL_MIN(NET_TCP_DFLT_TIMEOUT_CONN_TX_Q_MS, 0) == DEF_OK) \
  && (NET_TCP_DFLT_TIMEOUT_CONN_TX_Q_MS == NET_TMR_TIME_INFINITE))))
#error  "NET_TCP_DFLT_TIMEOUT_CONN_TX_Q_MS         illegally #define'd in 'net_cfg.h' "
#error  "                                         [MUST be  >= NET_TIMEOUT_MIN_mS]   "
#error  "                                         [     &&  <= NET_TIMEOUT_MAX_mS]   "
#error  "                                         [     ||  == NET_TMR_TIME_INFINITE]"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_TCP_MODULE_EN
#endif // _NET_TCP_PRIV_H_
