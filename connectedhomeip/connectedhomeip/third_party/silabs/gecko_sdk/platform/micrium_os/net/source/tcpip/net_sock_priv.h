/***************************************************************************//**
 * @file
 * @brief Network Socket Layer
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

#ifndef  _NET_SOCK_PRIV_H_
#define  _NET_SOCK_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_cfg_net.h"

#include  "../../include/net_sock.h"

#include  "net_def_priv.h"
#include  "net_type_priv.h"
#include  "net_stat_priv.h"
#include  "net_if_priv.h"

#ifdef  NET_IPv4_MODULE_EN
#include  "../../include/net_ipv4.h"
#include  "net_ipv4_priv.h"
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  "../../include/net_ipv6.h"
#include  "net_ipv6_priv.h"
#endif

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (NET_SOCK_CFG_SOCK_NBR_TCP > 0)
#define  NET_SOCK_CONN_NBR                          NET_SOCK_CFG_SOCK_NBR_UDP \
  + (NET_SOCK_CFG_SOCK_NBR_TCP * NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX)
#else
#define  NET_SOCK_CONN_NBR                          NET_SOCK_CFG_SOCK_NBR_UDP
#endif

#if (defined(NET_IPv6_MODULE_EN))
#define  NET_SOCK_ADDR_LEN_MAX                      NET_SOCK_ADDR_LEN_IP_V6

#elif (defined(NET_IPv4_MODULE_EN))
#define  NET_SOCK_ADDR_LEN_MAX                      NET_SOCK_ADDR_LEN_IP_V4

#else
#define  NET_SOCK_ADDR_LEN_MAX                      0
#endif

/********************************************************************************************************
 *                                   NETWORK SOCKET PORT NUMBER DEFINES
 *
 * Note(s) : (1) Socket port numbers defined in host-order.
 *
 *               See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
 *******************************************************************************************************/

#define  NET_SOCK_PORT_NBR_RESERVED                      NET_PORT_NBR_RESERVED
#define  NET_SOCK_PORT_NBR_NONE                          NET_SOCK_PORT_NBR_RESERVED
#define  NET_SOCK_PORT_NBR_RANDOM                        NET_SOCK_PORT_NBR_RESERVED

#ifndef  NET_SOCK_DFLT_NO_BLOCK_EN
    #define  NET_SOCK_DFLT_NO_BLOCK_EN  DEF_DISABLED
#endif

#ifndef  NET_SOCK_DFLT_PORT_NBR_RANDOM_BASE
    #define  NET_SOCK_DFLT_PORT_NBR_RANDOM_BASE             49152u
#endif

#ifndef  NET_SOCK_DFLT_PORT_NBR_RANDOM_END
    #define  NET_SOCK_DFLT_PORT_NBR_RANDOM_END              65535u
#endif

#define  NET_SOCK_PORT_NBR_RANDOM_MIN                    NET_SOCK_DFLT_PORT_NBR_RANDOM_BASE
#define  NET_SOCK_PORT_NBR_RANDOM_MAX                    NET_SOCK_DFLT_PORT_NBR_RANDOM_END

#ifndef  NET_SOCK_DFLT_TIMEOUT_RX_Q_MS
//                                                                 Configure socket timeout values (see Note #5) :
//                                                                 Configure (datagram) socket receive queue timeout.
    #define  NET_SOCK_DFLT_TIMEOUT_RX_Q_MS               NET_TMR_TIME_INFINITE
#endif

#ifndef  NET_SOCK_DFLT_TIMEOUT_CONN_REQ_MS
//                                                                 Configure socket connection request timeout.
    #define  NET_SOCK_DFLT_TIMEOUT_CONN_REQ_MS           NET_TMR_TIME_INFINITE
#endif

#ifndef  NET_SOCK_DFLT_TIMEOUT_CONN_ACCEPT_MS
//                                                                 Configure socket connection accept  timeout.
    #define  NET_SOCK_DFLT_TIMEOUT_CONN_ACCEPT_MS        NET_TMR_TIME_INFINITE
#endif

#ifndef  NET_SOCK_DFLT_TIMEOUT_CONN_CLOSE_MS
//                                                                 Configure socket connection close   timeout.
    #define  NET_SOCK_DFLT_TIMEOUT_CONN_CLOSE_MS         10000u
#endif

/********************************************************************************************************
 *                                           NETWORK SOCKET STATES
 *******************************************************************************************************/

/********************************************************************************************************
 *                               NETWORK SOCKET BLOCKING MODE SELECT DEFINES
 *
 * Note(s) : (1) The following socket values MUST be pre-#define'd in 'net_def.h' PRIOR to 'net_cfg.h'
 *               so that the developer can configure sockets for the desired socket blocking mode (see
 *               'net_def.h  BSD 4.x & NETWORK SOCKET LAYER DEFINES  Note #1b' & 'net_cfg_net.h  NETWORK
 *               SOCKET LAYER CONFIGURATION') :
 *
 *               (a) NET_SOCK_BLOCK_SEL_DFLT
 *               (b) NET_SOCK_BLOCK_SEL_BLOCK
 *               (c) NET_SOCK_BLOCK_SEL_NO_BLOCK
 *
 *           (2) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing,
 *               Section 6.2 'Blocking I/O Model', Page 154 states that "by default, all sockets are
 *               blocking".
 *******************************************************************************************************/

#if 0                                                           // See Note #1.
                                                                // ------------------ SOCK BLOCK SEL ------------------
#define  NET_SOCK_BLOCK_SEL_NONE                           0u
#define  NET_SOCK_BLOCK_SEL_DFLT                           1u   // Sock block mode determined by run-time sock opts ...
#define  NET_SOCK_BLOCK_SEL_BLOCK                          2u   // ... but dflts to blocking (see Note #2).
#define  NET_SOCK_BLOCK_SEL_NO_BLOCK                       3u

#endif

/********************************************************************************************************
 *                                   NETWORK SOCKET (OBJECT) FLAG DEFINES
 *******************************************************************************************************/

//                                                                 ---------------- NET SOCK OBJ FLAGS ----------------
#define  NET_SOCK_FLAG_SOCK_NONE                  DEF_BIT_NONE
#define  NET_SOCK_FLAG_SOCK_USED                  DEF_BIT_08    // Sock cur used; i.e. NOT in free sock pool.
#define  NET_SOCK_FLAG_SOCK_NO_BLOCK              MSG_DONTWAIT  // Sock blocking DISABLED.
#define  NET_SOCK_FLAG_SOCK_SECURE                DEF_BIT_09    // Sock security ENABLED.
#define  NET_SOCK_FLAG_SOCK_SECURE_NEGO           DEF_BIT_10

/********************************************************************************************************
 *                                   NETWORK SOCKET EVENT TYPE DEFINES
 *
 * Note(s) : (1) 'EVENT_TYPE' abbreviated to 'EVENT' to enforce ANSI-compliance of 31-character symbol
 *                   length uniqueness.
 *******************************************************************************************************/

#define  NET_SOCK_EVENT_NONE                               0u
#define  NET_SOCK_EVENT_ERR                                1u

#define  NET_SOCK_EVENT_SOCK_RX                           10u
#define  NET_SOCK_EVENT_SOCK_TX                           11u
#define  NET_SOCK_EVENT_SOCK_ACCEPT                       12u
#define  NET_SOCK_EVENT_SOCK_CONN                         13u
#define  NET_SOCK_EVENT_SOCK_CLOSE                        14u

#define  NET_SOCK_EVENT_SOCK_ERR_RX                       20u
#define  NET_SOCK_EVENT_SOCK_ERR_TX                       21u
#define  NET_SOCK_EVENT_SOCK_ERR_ACCEPT                   22u
#define  NET_SOCK_EVENT_SOCK_ERR_CONN                     23u
#define  NET_SOCK_EVENT_SOCK_ERR_CLOSE                    24u

#define  NET_SOCK_EVENT_TRANSPORT_RX                      30u
#define  NET_SOCK_EVENT_TRANSPORT_TX                      31u

#define  NET_SOCK_EVENT_TRANSPORT_ERR_RX                  40u
#define  NET_SOCK_EVENT_TRANSPORT_ERR_TX                  41u

/********************************************************************************************************
 *                               NETWORK SOCKET FAMILY & PROTOCOL DEFINES
 *
 * Note(s) : (1) The following socket values MUST be pre-#define'd in 'net_def.h' PRIOR to 'net_cfg.h'
 *               so that the developer can configure sockets for the correct socket family values (see
 *               'net_def.h  BSD 4.x & NETWORK SOCKET LAYER DEFINES  Note #1' & 'net_cfg_net.h  NETWORK
 *               SOCKET LAYER CONFIGURATION') :
 *
 *               (a) (1) NET_SOCK_ADDR_FAMILY_IP_V4
 *                   (2) NET_SOCK_PROTOCOL_FAMILY_IP_V4
 *                   (3) NET_SOCK_FAMILY_IP_V4
 *
 *               (b) (1) NET_SOCK_ADDR_LEN_IP_V4
 *                   (2) NET_SOCK_PROTO_MAX_IP_V4
 *
 *           (2) 'NET_SOCK_PROTOCOL_MAX' abbreviated to 'NET_SOCK_PROTO_MAX' to enforce ANSI-compliance of
 *                   31-character symbol length uniqueness.
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   NETWORK SOCKET ADDRESS DEFINES
 *******************************************************************************************************/

//                                                                 ------------------ SOCK ADDR CFG -------------------

#define  NET_SOCK_ADDR_IP_LEN_PORT                  (sizeof(NET_PORT_NBR))

#define  NET_SOCK_ADDR_IP_IX_BASE                             0
#define  NET_SOCK_ADDR_IP_IX_PORT                           NET_SOCK_ADDR_IP_IX_BASE

//                                                                 ---------------- IPv4 SOCK ADDR CFG ----------------
#ifdef   NET_IPv4_MODULE_EN

#define  NET_SOCK_ADDR_IP_V4_LEN_ADDR               (sizeof(NET_IPv4_ADDR))
#define  NET_SOCK_ADDR_IP_V4_LEN_PORT_ADDR                  NET_SOCK_ADDR_IP_V4_LEN_ADDR + NET_SOCK_ADDR_IP_LEN_PORT

#define  NET_SOCK_ADDR_IP_V4_IX_ADDR                       (NET_SOCK_ADDR_IP_IX_PORT + NET_SOCK_ADDR_IP_LEN_PORT)

#define  NET_SOCK_ADDR_IP_V4_WILDCARD                       NET_IPv4_ADDR_NONE
#define  NET_SOCK_ADDR_IP_V4_BROADCAST                      INADDR_BROADCAST

#endif

//                                                                 ---------------- IPv6 SOCK ADDR CFG ----------------
#ifdef   NET_IPv6_MODULE_EN

#define  NET_SOCK_ADDR_IP_V6_LEN_FLOW               (sizeof(CPU_INT32U))
#define  NET_SOCK_ADDR_IP_V6_LEN_ADDR               (sizeof(NET_IPv6_ADDR))
#define  NET_SOCK_ADDR_IP_V6_LEN_PORT_ADDR                  NET_SOCK_ADDR_IP_V6_LEN_ADDR + NET_SOCK_ADDR_IP_LEN_PORT

#define  NET_SOCK_ADDR_IP_V6_IX_FLOW                       (NET_SOCK_ADDR_IP_IX_PORT + NET_SOCK_ADDR_IP_LEN_PORT)
#define  NET_SOCK_ADDR_IP_V6_IX_ADDR                       (NET_SOCK_ADDR_IP_V6_IX_FLOW + NET_SOCK_ADDR_IP_V6_LEN_FLOW)

#define  NET_SOCK_ADDR_IP_V6_WILDCARD                       NET_IPv6_ADDR_ANY

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                       NETWORK SOCKET DATA LENGTH & (ERROR) RETURN CODE DATA TYPES
 *
 * Note(s) : (1) IEEE Std 1003.1, 2004 Edition, Section 'sys/types.h : DESCRIPTION' states that :
 *
 *               (a) "ssize_t - Used for a count of bytes or an error indication."
 *
 *               (b) "ssize_t shall be [a] signed integer type ... capable of storing values at least in
 *                       the range [-1, {SSIZE_MAX}]."
 *
 *                   (1) IEEE Std 1003.1, 2004 Edition, Section 'limits.h : DESCRIPTION' states that the
 *                       "Minimum Acceptable Value ... [for] {SSIZE_MAX}" is "32767".
 *
 *                   (2) To avoid possible integer overflow, the network socket return code data type MUST
 *                       be declared as a signed integer data type with a maximum positive value greater
 *                       than or equal to all transport layers' maximum positive return value.
 *
 *                       See also 'net_udp.c  NetUDP_RxAppData()      Return(s)',
 *                                   'net_udp.c  NetUDP_TxAppData()      Return(s)',
 *                                   'net_tcp.c  NetTCP_RxAppData()      Return(s)',
 *                               & 'net_tcp.c  NetTCP_TxConnAppData()  Return(s)'.
 *
 *           (2) NET_SOCK_DATA_SIZE_MAX  SHOULD be #define'd based on 'NET_SOCK_DATA_SIZE' data type declared.
 *******************************************************************************************************/

#define  NET_SOCK_DATA_SIZE_MIN                            0
#define  NET_SOCK_DATA_SIZE_MAX          DEF_INT_32S_MAX_VAL    // See Note #2.

/********************************************************************************************************
 *                                   NETWORK SOCKET STATE DATA TYPE
 *******************************************************************************************************/

//                                                                 typedef  CPU_INT08U  NET_SOCK_STATE;
typedef enum net_sock_state {
  NET_SOCK_STATE_NONE = 1u,

  NET_SOCK_STATE_FREE = 2u,
  NET_SOCK_STATE_DISCARD = 3u,
  NET_SOCK_STATE_CLOSED = 10u,
  NET_SOCK_STATE_CLOSED_FAULT = 11u,
  NET_SOCK_STATE_CLOSE_IN_PROGRESS = 15u,
  NET_SOCK_STATE_CLOSING_DATA_AVAIL = 16u,
  NET_SOCK_STATE_BOUND = 20u,
  NET_SOCK_STATE_LISTEN = 30u,
  NET_SOCK_STATE_CONN = 40u,
  NET_SOCK_STATE_CONN_IN_PROGRESS = 41u,
  NET_SOCK_STATE_CONN_DONE = 42u
} NET_SOCK_STATE;

/********************************************************************************************************
 *                               NETWORK SOCKET IDENTIFICATION DATA TYPE
 *
 * Note(s) : (1) (a) NET_SOCK_NBR_MAX  SHOULD be #define'd based on 'NET_SOCK_QTY' data type declared.
 *
 *               (b) However, since socket handle identifiers are data-typed as 16-bit signed integers;
 *                   the maximum unique number of valid socket handle identifiers, & therefore the
 *                   maximum number of valid sockets, is the total number of non-negative values that
 *                   16-bit signed integers support.
 *******************************************************************************************************/

#define  NET_SOCK_NBR_MIN                                  1
#define  NET_SOCK_NBR_MAX                DEF_INT_16S_MAX_VAL    // See Note #1.

#define  NET_SOCK_ID_MIN                                   0
#define  NET_SOCK_ID_MAX          (NET_SOCK_NBR_SOCK - 1)

/********************************************************************************************************
 *                                   NETWORK SOCKET FLAGS DATA TYPES
 *
 * Note(s) : (1) Ideally, network socket API argument flags data type SHOULD be defined as   an unsigned
 *               integer data type since logical bitwise operations should be performed ONLY on unsigned
 *               integer data types.
 *******************************************************************************************************/

typedef  NET_FLAGS NET_SOCK_FLAGS;

/********************************************************************************************************
 *                                   NETWORK SOCKET QUEUE SIZE DATA TYPE
 *
 * Note(s) : (1) (a) NET_SOCK_Q_SIZE #define's SHOULD be #define'd based on 'NET_SOCK_Q_SIZE'
 *                   data type declared.
 *
 *               (b) However, since socket/connection handle identifiers are data-typed as 16-bit
 *                   signed integers; the maximum unique number of valid socket/connection handle
 *                   identifiers, & therefore the maximum number of valid sockets/connections, is
 *                   the total number of non-negative values that 16-bit signed integers support.
 *
 *                   See also             'NETWORK SOCKET     IDENTIFICATION DATA TYPE  Note #1b'
 *                           & 'net_conn.h  NETWORK CONNECTION IDENTIFICATION DATA TYPE  Note #2b'.
 *
 *           (2) (a) NET_SOCK_Q_IX   #define's SHOULD be #define'd based on 'NET_SOCK_Q_SIZE'
 *                   data type declared.
 *
 *               (b) Since socket queue size is data typed as a 16-bit unsigned integer but the
 *                   maximum queue sizes are #define'd as 16-bit signed integer values ... :
 *
 *                   (1) Valid socket queue indices are #define'd within the range of     16-bit
 *                           signed integer values, ...
 *                   (2) but   socket queue indice exception values may be #define'd with 16-bit
 *                       unsigned integer values.
 *******************************************************************************************************/

//                                                                 See Note #1.
#define  NET_SOCK_Q_SIZE_NONE                              0
#define  NET_SOCK_Q_SIZE_UNLIMITED                         0
#define  NET_SOCK_Q_SIZE_MIN                NET_SOCK_NBR_MIN
#define  NET_SOCK_Q_SIZE_MAX                NET_SOCK_NBR_MAX    // See Note #1b.

//                                                                 See Note #2.
#define  NET_SOCK_Q_IX_NONE              DEF_INT_16U_MAX_VAL    // See Note #2b.
#define  NET_SOCK_Q_IX_MIN                                 0
#define  NET_SOCK_Q_IX_MAX          (NET_SOCK_Q_SIZE_MAX - 1)

/********************************************************************************************************
 *                                   NETWORK SOCKET ACCEPT Q DATA TYPE
 *******************************************************************************************************/

typedef  struct  net_sock_accept_q_obj {
  NET_CONN_ID  ConnID;
  CPU_BOOLEAN  IsRdy;
  SLIST_MEMBER ListNode;
} NET_SOCK_ACCEPT_Q_OBJ;

/********************************************************************************************************
 *                                   NETWORK SOCKET ADDRESS DATA TYPES
 *
 * Note(s) : (1) See 'net_sock.h  Note #1a' for supported socket address families.
 *
 *           (2) (a) Socket address structure 'AddrFamily' member MUST be configured in host-order & MUST
 *                   NOT be converted to/from network-order.
 *
 *               (b) Socket address structure addresses MUST be configured/converted from host-order to
 *                   network-order.
 *
 *               See also 'net_bsd.h  BSD 4.x SOCKET DATA TYPES  Note #2b'.
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   NETWORK SOCKET SEL EVENT DATA TYPES
 *******************************************************************************************************/

typedef  enum  net_sock_msg_type {
  NET_SOCK_EVENT_TYPE_CONN_REQ_SIGNAL,
  NET_SOCK_EVENT_TYPE_CONN_REQ_ABORT,
  NET_SOCK_EVENT_TYPE_CONN_CLOSE_ABORT,
  NET_SOCK_EVENT_TYPE_CONN_CLOSE_SIGNAL,
  NET_SOCK_EVENT_TYPE_CONN_ACCEPT_SIGNAL,
  NET_SOCK_EVENT_TYPE_CONN_ACCEPT_ABORT,
  NET_SOCK_EVENT_TYPE_RX_ABORT,
  NET_SOCK_EVENT_TYPE_RX,
  NET_SOCK_EVENT_TYPE_TX,
  NET_SOCK_EVENT_TYPE_SEL_ABORT
} NET_SOCK_EVENT_TYPE;

typedef  CPU_INT08U NET_SOCK_SEL_EVENT_FLAG;

#define  NET_SOCK_SEL_EVENT_FLAG_NONE           DEF_BIT_NONE
#define  NET_SOCK_SEL_EVENT_FLAG_RD             DEF_BIT_00
#define  NET_SOCK_SEL_EVENT_FLAG_WR             DEF_BIT_01
#define  NET_SOCK_SEL_EVENT_FLAG_ERR            DEF_BIT_02

/********************************************************************************************************
 *                                       NETWORK SOCKET DATA TYPE
 *
 *                             NET_SOCK
 *                          |-------------|
 *                          |  Sock Type  |
 *                          |-------------|      Next
 *                          |      O----------> Socket     Buffer Queue
 *                          |-------------|                    Heads      -------
 *                          |      O------------------------------------> |     |
 *                          |-------------|                               |     |
 *                          |      O----------------------                -------
 *                          |-------------|              |                  | ^
 *                          |  Conn IDs   |              |                  v |
 *                          |-------------|              |                -------
 *                          |    Sock     |              |                |     |
 *                          |   Family/   |              |                |     |
 *                          |  Protocol   |              |                -------
 *                          |-------------|              |                  | ^
 *                          | Conn Ctrls  |              | Buffer Queue     v |
 *                          |-------------|              |     Tails      -------
 *                          |    Flags    |              ---------------> |     |
 *                          |-------------|                               |     |
 *                          |    State    |                               -------
 *                          |-------------|
 *
 *
 * Note(s) : (1) (a) 'TxQ_Head'/'TxQ_Tail' may NOT be necessary but are included for consistency.
 *               (b) 'TxQ_SizeCur'         may NOT be necessary but is  included for consistency.
 *******************************************************************************************************/
typedef  struct  net_sock_sel_obj NET_SOCK_SEL_OBJ;

struct  net_sock_sel_obj {
  KAL_SEM_HANDLE          SockSelTaskSignalObj;
  NET_SOCK_SEL_EVENT_FLAG SockSelPendingFlags;
  NET_SOCK_SEL_OBJ        *ObjPrevPtr;
};

//                                                                 ----------------- NET SOCK -----------------
typedef  struct  net_sock NET_SOCK;

struct  net_sock {
  NET_SOCK                 *NextPtr;                                    // Ptr to NEXT sock.

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  NET_SOCK_SEL_OBJ         *SelObjTailPtr;
#endif

  KAL_SEM_HANDLE           RxQ_SignalObj;
  CPU_INT32U               RxQ_SignalTimeout_ms;

  NET_BUF                  *RxQ_Head;                                   // Ptr to head of sock's datagram rx buf Q.
  NET_BUF                  *RxQ_Tail;                                   // Ptr to tail of sock's datagram rx buf Q.
#if 0                                                                   // See Note #2a.
  NET_BUF                  *TxQ_Head;                                   // Ptr to head of sock's datagram tx buf Q.
  NET_BUF                  *TxQ_Tail;                                   // Ptr to tail of sock's datagram tx buf Q.
#endif

  NET_SOCK_DATA_SIZE       RxQ_SizeCfgd;                                // Datagram rx buf Q size cfg'd (in octets).
  NET_SOCK_DATA_SIZE       RxQ_SizeCur;                                 // Datagram rx buf Q size cur   (in octets).

  NET_SOCK_DATA_SIZE       TxQ_SizeCfgd;                                // Datagram tx buf Q size cfg'd (in octets).
#if 0                                                                   // See Note #2b.
  NET_SOCK_DATA_SIZE       TxQ_SizeCur;                                 // Datagram tx buf Q size cur   (in octets).
#endif

  NET_SOCK_ID              ID;                                          // Sock        id.
#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
  NET_SOCK_ID              ID_SockParent;                               // Parent sock id.
#endif
  NET_CONN_ID              ID_Conn;                                     // Conn        id.

  NET_IF_NBR               IF_Nbr;                                      // IF nbr.

  NET_SOCK_PROTOCOL_FAMILY ProtocolFamily;                              // Sock protocol family.
  NET_SOCK_PROTOCOL        Protocol;                                    // Sock protocol.
  NET_SOCK_TYPE            SockType;                                    // Sock type.

#ifdef  NET_SECURE_MODULE_EN
  void                     *SecureSession;                              // Sock secure session.
#endif

#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
  KAL_SEM_HANDLE  ConnReqSignalObj;
  CPU_INT32U      ConnReqSignalTimeout_ms;

  KAL_SEM_HANDLE  ConnAcceptQSignalObj;
  CPU_INT32U      ConnAcceptQSignalTimeout_ms;

  KAL_SEM_HANDLE  ConnCloseSignalObj;
  CPU_INT32U      ConnCloseSignalTimeout_ms;

  SLIST_MEMBER    *ConnAcceptQ_Ptr;
  NET_SOCK_Q_SIZE ConnAcceptQ_SizeMax;                                  // Max Q size to accept rx'd conn reqs.
  NET_SOCK_Q_SIZE ConnAcceptQ_SizeCur;                                  // Cur Q size to accept rx'd conn reqs.

  NET_SOCK_Q_SIZE ConnChildQ_SizeMax;                                   // Max Q size to child conn.
  NET_SOCK_Q_SIZE ConnChildQ_SizeCur;                                   // Cur Q size to child conn.                                                                        // Conn accept Q (conn id's q'd into array).
#endif

  NET_SOCK_STATE  State;                                                // Sock state.

  NET_SOCK_FLAGS  Flags;                                                // Sock flags.
};

typedef  enum  NET_SOCK_SECURE_TYPE {
  NET_SOCK_SECURE_TYPE_NONE,
  NET_SOCK_SECURE_TYPE_SERVER,
  NET_SOCK_SECURE_TYPE_CLIENT
} NET_SOCK_SECURE_TYPE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (NET_STAT_POOL_SOCK_EN == DEF_ENABLED)
extern NET_STAT_POOL NetSock_PoolStat;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 *
 * Note(s) : (1) Ideally, socket data handler functions should be defined as local functions.  However,
 *               since these handler functions are required as callback functions for network security
 *               manager port files; these handler functions MUST be defined as global functions.
 ********************************************************************************************************
 *******************************************************************************************************/

void NetSock_Init(MEM_SEG  *p_mem_seg,
                  RTOS_ERR *p_err);

NET_SOCK_RTN_CODE NetSock_SelInternal(KAL_SEM_HANDLE   sem_handle,
                                      NET_SOCK_QTY     sock_nbr_max,
                                      NET_SOCK_DESC    *p_sock_desc_rd,
                                      NET_SOCK_DESC    *p_sock_desc_wr,
                                      NET_SOCK_DESC    *p_sock_desc_err,
                                      NET_SOCK_TIMEOUT *p_timeout,
                                      RTOS_ERR         *p_err);

//                                                                 --------- RX FNCTS ---------
void NetSock_Rx(NET_BUF  *pbuf,
                RTOS_ERR *p_err);

void NetSock_CloseFromConn(NET_SOCK_ID sock_id);

void NetSock_FreeConnFromSock(NET_SOCK_ID sock_id,
                              NET_CONN_ID conn_id);

#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
void NetSock_ConnChildAdd(NET_SOCK_ID sock_id,
                          NET_CONN_ID conn_id,
                          RTOS_ERR    *p_err);

void NetSock_ConnSignalReq(NET_SOCK_ID sock_id);

void NetSock_ConnSignalAccept(NET_SOCK_ID sock_id,
                              NET_CONN_ID conn_id);

void NetSock_ConnSignalClose(NET_SOCK_ID sock_id,
                             CPU_BOOLEAN data_avail,
                             RTOS_ERR    *p_err);

NET_SOCK_RTN_CODE NetSock_RxDataHandlerStream(NET_SOCK_ID        sock_id,
                                              NET_SOCK           *psock,
                                              void               *pdata_buf,
                                              CPU_INT16U         data_buf_len,
                                              NET_SOCK_API_FLAGS flags,
                                              NET_SOCK_ADDR      *paddr_remote,
                                              NET_SOCK_ADDR_LEN  *paddr_len,
                                              RTOS_ERR           *p_err);

NET_SOCK_RTN_CODE NetSock_TxDataHandlerStream(NET_SOCK_ID        sock_id,
                                              NET_SOCK           *psock,
                                              void               *p_data,
                                              CPU_INT16U         data_len,
                                              NET_SOCK_API_FLAGS flags,
                                              RTOS_ERR           *p_err);
#endif // NET_SOCK_TYPE_STREAM_MODULE_EN

//                                                                 ---- SOCK STATUS FNCTS -----
CPU_BOOLEAN NetSock_IsUsed(NET_SOCK_ID sock_id);

NET_SOCK *NetSock_GetObj(NET_SOCK_ID sock_id);

//                                                                 Clr      sock rx Q signal.
void NetSock_RxQ_Clr(NET_SOCK *p_sock);

//                                                                 Wait for sock rx Q signal.
void NetSock_RxQ_Wait(NET_SOCK *p_sock,
                      RTOS_ERR *p_err);
//                                                                 Signal   sock rx Q.
void NetSock_RxQ_Signal(NET_SOCK *p_sock);

//                                                                 Abort    sock rx Q.
void NetSock_RxQ_Abort(NET_SOCK *p_sock);

//                                                                 Set dflt sock rx Q timeout.
void NetSock_RxQ_TimeoutDflt(NET_SOCK *p_sock);

//                                                                 Set      sock rx Q timeout.
void NetSock_RxQ_TimeoutSet(NET_SOCK   *p_sock,
                            CPU_INT32U timeout_ms);

//                                                                 Get      sock rx Q timeout.
CPU_INT32U NetSock_RxQ_TimeoutGet_ms(NET_SOCK *p_sock);

#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
//                                                                 Clr      sock conn signal.
void NetSock_ConnReqClr(NET_SOCK *p_sock);

//                                                                 Wait for sock conn signal.
void NetSock_ConnReqWait(NET_SOCK *p_sock,
                         RTOS_ERR *p_err);

//                                                                 Signal   sock conn.
void NetSock_ConnReqSignal(NET_SOCK *p_sock);

//                                                                 Abort    sock conn.
void NetSock_ConnReqAbort(NET_SOCK *p_sock);

//                                                                 Set dflt sock conn timeout.
void NetSock_ConnReqTimeoutDflt(NET_SOCK *p_sock);

//                                                                 Set      sock conn timeout.
void NetSock_ConnReqTimeoutSet(NET_SOCK   *p_sock,
                               CPU_INT32U timeout_ms);

//                                                                 Get      sock conn timeout.
CPU_INT32U NetSock_ConnReqTimeoutGet_ms(NET_SOCK *p_sock);

//                                                                 Clr      sock accept Q signal.
void NetSock_ConnAcceptQ_SemClr(NET_SOCK *p_sock);

//                                                                 Wait for sock accept Q signal.
void NetSock_ConnAcceptQ_Wait(NET_SOCK *p_sock,
                              RTOS_ERR *p_err);
//                                                                 Signal   sock accept Q.
void NetSock_ConnAcceptQ_Signal(NET_SOCK *p_sock);

//                                                                 Abort    sock accept Q wait.
void NetSock_ConnAcceptQ_Abort(NET_SOCK *p_sock);

//                                                                 Set dflt sock accept Q timeout.
void NetSock_ConnAcceptQ_TimeoutDflt(NET_SOCK *p_sock);

//                                                                 Set      sock accept Q timeout.
void NetSock_ConnAcceptQ_TimeoutSet(NET_SOCK   *p_sock,
                                    CPU_INT32U timeout_ms);

//                                                                 Get      sock accept Q timeout.
CPU_INT32U NetSock_ConnAcceptQ_TimeoutGet_ms(NET_SOCK *p_sock);

//                                                                 Clr      sock close signal.
void NetSock_ConnCloseClr(NET_SOCK *p_sock);

//                                                                 Wait for sock close signal.
void NetSock_ConnCloseWait(NET_SOCK *p_sock,
                           RTOS_ERR *p_err);

//                                                                 Signal   sock close.
void NetSock_ConnCloseSignal(NET_SOCK *p_sock);

//                                                                 Abort    sock close.
void NetSock_ConnCloseAbort(NET_SOCK *p_sock);

//                                                                 Set dflt sock close timeout.
void NetSock_ConnCloseTimeoutDflt(NET_SOCK *p_sock);

//                                                                 Set      sock close timeout.
void NetSock_ConnCloseTimeoutSet(NET_SOCK   *p_sock,
                                 CPU_INT32U timeout_ms);

//                                                                 Get      sock close timeout.
CPU_INT32U NetSock_ConnCloseTimeoutGet_ms(NET_SOCK *p_sock);
#endif // NET_SOCK_TYPE_STREAM_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_SOCK_NBR_SOCK
#error  "NET_SOCK_NBR_SOCK                      not #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  >= NET_SOCK_NBR_MIN    ]"
#error  "                                     [     &&  <= NET_SOCK_NBR_MAX    ]"

#elif   (DEF_CHK_VAL(NET_SOCK_NBR_SOCK, \
                     NET_SOCK_NBR_MIN,  \
                     NET_SOCK_NBR_MAX) != DEF_OK)
#error  "NET_SOCK_NBR_SOCK                illegally #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  >= NET_SOCK_NBR_MIN    ]"
#error  "                                     [     &&  <= NET_SOCK_NBR_MAX    ]"
#endif

#ifndef  NET_SOCK_CFG_SEL_EN
#error  "NET_SOCK_CFG_SEL_EN                        not #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  DEF_DISABLED]           "
#error  "                                     [     ||  DEF_ENABLED ]           "

#elif  ((NET_SOCK_CFG_SEL_EN != DEF_DISABLED) \
  && (NET_SOCK_CFG_SEL_EN != DEF_ENABLED))
#error  "NET_SOCK_CFG_SEL_EN                  illegally #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  DEF_DISABLED]           "
#error  "                                     [     ||  DEF_ENABLED ]           "

#endif

#ifndef  NET_SOCK_CFG_RX_Q_SIZE_OCTET
#error  "NET_SOCK_CFG_RX_Q_SIZE_OCTET               not #define'd in 'net_cfg.h'  "
#error  "                                     [MUST be  >= NET_SOCK_DATA_SIZE_MIN]"
#error  "                                     [     &&  <= NET_SOCK_DATA_SIZE_MIN]"

#elif   (DEF_CHK_VAL(NET_SOCK_CFG_RX_Q_SIZE_OCTET, \
                     NET_SOCK_DATA_SIZE_MIN,       \
                     NET_SOCK_DATA_SIZE_MAX) != DEF_OK)
#error  "NET_SOCK_CFG_RX_Q_SIZE_OCTET         illegally #define'd in 'net_cfg.h'  "
#error  "                                     [MUST be  >= NET_SOCK_DATA_SIZE_MIN]"
#error  "                                     [     &&  <= NET_SOCK_DATA_SIZE_MIN]"
#endif

#ifndef  NET_SOCK_CFG_TX_Q_SIZE_OCTET
#error  "NET_SOCK_CFG_TX_Q_SIZE_OCTET               not #define'd in 'net_cfg.h'  "
#error  "                                     [MUST be  >= NET_SOCK_DATA_SIZE_MIN]"
#error  "                                     [     &&  <= NET_SOCK_DATA_SIZE_MIN]"

#elif   (DEF_CHK_VAL(NET_SOCK_CFG_TX_Q_SIZE_OCTET, \
                     NET_SOCK_DATA_SIZE_MIN,       \
                     NET_SOCK_DATA_SIZE_MAX) != DEF_OK)
#error  "NET_SOCK_CFG_TX_Q_SIZE_OCTET         illegally #define'd in 'net_cfg.h'  "
#error  "                                     [MUST be  >= NET_SOCK_DATA_SIZE_MIN]"
#error  "                                     [     &&  <= NET_SOCK_DATA_SIZE_MIN]"
#endif

#ifndef  NET_SOCK_DFLT_TIMEOUT_RX_Q_MS
#error  "NET_SOCK_DFLT_TIMEOUT_RX_Q_MS               not #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_TIMEOUT_MIN_mS]   "
#error  "                                     [     &&  <= NET_TIMEOUT_MAX_mS]   "
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"

#elif  ((DEF_CHK_VAL(NET_SOCK_DFLT_TIMEOUT_RX_Q_MS,                   \
                     NET_TIMEOUT_MIN_mS,                              \
                     NET_TIMEOUT_MAX_mS) != DEF_OK)                   \
  && (!((DEF_CHK_VAL_MIN(NET_SOCK_DFLT_TIMEOUT_RX_Q_MS, 0) == DEF_OK) \
  && (NET_SOCK_DFLT_TIMEOUT_RX_Q_MS == NET_TMR_TIME_INFINITE))))
#error  "NET_SOCK_DFLT_TIMEOUT_RX_Q_MS         illegally #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_TIMEOUT_MIN_mS]   "
#error  "                                     [     &&  <= NET_TIMEOUT_MAX_mS]   "
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"
#endif

#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN

#ifndef  NET_SOCK_DFLT_TIMEOUT_CONN_REQ_MS
#error  "NET_SOCK_DFLT_TIMEOUT_CONN_REQ_MS           not #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_TIMEOUT_MIN_mS]   "
#error  "                                     [     &&  <= NET_TIMEOUT_MAX_mS]   "
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"

#elif  ((DEF_CHK_VAL(NET_SOCK_DFLT_TIMEOUT_CONN_REQ_MS,                   \
                     NET_TIMEOUT_MIN_mS,                                  \
                     NET_TIMEOUT_MAX_mS) != DEF_OK)                       \
  && (!((DEF_CHK_VAL_MIN(NET_SOCK_DFLT_TIMEOUT_CONN_REQ_MS, 0) == DEF_OK) \
  && (NET_SOCK_DFLT_TIMEOUT_CONN_REQ_MS == NET_TMR_TIME_INFINITE))))
#error  "NET_SOCK_DFLT_TIMEOUT_CONN_REQ_MS     illegally #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_TIMEOUT_MIN_mS]   "
#error  "                                     [     &&  <= NET_TIMEOUT_MAX_mS]   "
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"
#endif

#ifndef  NET_SOCK_DFLT_TIMEOUT_CONN_ACCEPT_MS
#error  "NET_SOCK_DFLT_TIMEOUT_CONN_ACCEPT_MS        not #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_TIMEOUT_MIN_mS]   "
#error  "                                     [     &&  <= NET_TIMEOUT_MAX_mS]   "
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"

#elif  ((DEF_CHK_VAL(NET_SOCK_DFLT_TIMEOUT_CONN_ACCEPT_MS,                   \
                     NET_TIMEOUT_MIN_mS,                                     \
                     NET_TIMEOUT_MAX_mS) != DEF_OK)                          \
  && (!((DEF_CHK_VAL_MIN(NET_SOCK_DFLT_TIMEOUT_CONN_ACCEPT_MS, 0) == DEF_OK) \
  && (NET_SOCK_DFLT_TIMEOUT_CONN_ACCEPT_MS == NET_TMR_TIME_INFINITE))))
#error  "NET_SOCK_DFLT_TIMEOUT_CONN_ACCEPT_MS  illegally #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_TIMEOUT_MIN_mS]   "
#error  "                                     [     &&  <= NET_TIMEOUT_MAX_mS]   "
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"
#endif

#if 0
#ifndef  NET_SOCK_DFLT_TIMEOUT_CONN_CLOSE_MS
#error  "NET_SOCK_DFLT_TIMEOUT_CONN_CLOSE_MS         not #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_TIMEOUT_MIN_mS]   "
#error  "                                     [     &&  <= NET_TIMEOUT_MAX_mS]   "
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"

#elif  ((DEF_CHK_VAL(NET_SOCK_DFLT_TIMEOUT_CONN_CLOSE_MS,                   \
                     NET_TIMEOUT_MIN_mS,                                    \
                     NET_TIMEOUT_MAX_mS) != DEF_OK)                         \
  && (!((DEF_CHK_VAL_MIN(NET_SOCK_DFLT_TIMEOUT_CONN_CLOSE_MS, 0) == DEF_OK) \
  && (NET_SOCK_DFLT_TIMEOUT_CONN_CLOSE_MS == NET_TMR_TIME_INFINITE))))
#error  "NET_SOCK_DFLT_TIMEOUT_CONN_CLOSE_MS   illegally #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_TIMEOUT_MIN_mS]"
#error  "                                     [     &&  <= NET_TIMEOUT_MAX_mS]"
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"
#endif
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_SOCK_PRIV_H_
