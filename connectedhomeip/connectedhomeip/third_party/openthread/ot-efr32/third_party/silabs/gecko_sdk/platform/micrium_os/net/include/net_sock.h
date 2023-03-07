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

/****************************************************************************************************//**
 * @defgroup NET_CORE_SOCK Socket Functions API
 * @ingroup  NET_CORE
 * @brief    Socket Functions API
 *
 * @addtogroup NET_CORE_SOCK
 * @{
 ********************************************************************************************************
 * @note     (1) Supports BSD 4.x Socket Layer with the following restrictions/constraints :
 *               (a) ONLY supports a single address family from the following families :
 *                   (1) IPv4 (AF_INET)
 *               (b) ONLY supports the following socket types :
 *                   (1) Datagram (SOCK_DGRAM)
 *                   (2) Stream   (SOCK_STREAM)
 *               (c) ONLY supports a single protocol family from the following families :
 *                   (1) IPv4 (PF_INET) & IPv6 (PF_INET6)
 *                       (A) ONLY supports the following protocols :
 *                           (1) UDP (IPPROTO_UDP)
 *                           (2) TCP (IPPROTO_TCP)
 *               (d) ONLY supports the following socket options :
 *                       Blocking
 *                       Secure (TLS/SSL)
 *                       Rx Queue size
 *                       Tx Queue size
 *                       Time of server (IPv4-TOS)
 *                       Time to life   (IPv4-TTL)
 *                       Time to life multicast
 *                       UDP connection receive         timeout
 *                       TCP connection accept          timeout
 *                       TCP connection close           timeout
 *                       TCP connection connect request timeout
 *                       TCP connection receive         timeout
 *                       TCP connection transmit        timeout
 *                       TCP keep alive
 *                       TCP MSL
 *                       Force connection using a specific Interface
 *               (e) Multiple socket connections with the same local & remote address -- both
 *                   addresses & port numbers -- OR multiple socket connections with only a
 *                   local address but the same local address -- both address & port number --
 *                   is NOT currently supported.
 *
 * @note     (2) The Institute of Electrical and Electronics Engineers and The Open Group, have given
 *               us permission to reprint portions of their documentation.  Portions of this text are
 *               reprinted and reproduced in electronic form from the IEEE Std 1003.1, 2004 Edition,
 *               Standard for Information Technology -- Portable Operating System Interface (POSIX),
 *               The Open Group Base Specifications Issue 6, Copyright (C) 2001-2004 by the Institute
 *               of Electrical and Electronics Engineers, Inc and The Open Group.  In the event of any
 *               discrepancy between these versions and the original IEEE and The Open Group Standard,
 *               the original IEEE and The Open Group Standard is the referee document.  The original
 *               Standard can be obtained online at http://www.opengroup.org/unix/online.html.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_SOCK_H_
#define  _NET_SOCK_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net_cfg.h>

#include  <net/include/net_cfg_net.h>
#include  <net/include/net_stat.h>
#include  <net/include/net_type.h>
#include  <net/include/net_def.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_SOCK_NBR_SOCK                         (NET_SOCK_CFG_SOCK_NBR_UDP \
                                                    + (NET_SOCK_CFG_SOCK_NBR_TCP))

#define  NET_SOCK_ID_NONE                                 -1

/********************************************************************************************************
 *                                     NETWORK SOCKET API DEFINES
 *******************************************************************************************************/

#define  NET_SOCK_BSD_ERR_NONE                           NET_BSD_ERR_NONE
#define  NET_SOCK_BSD_ERR_DFLT                           NET_BSD_ERR_DFLT

#define  NET_SOCK_BSD_ERR_OPEN                           NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_CLOSE                          NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_BIND                           NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_CONN                           NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_LISTEN                         NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_ACCEPT                         NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_RX                             NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_TX                             NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_SEL                            NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_OPT_SET                        NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_OPT_GET                        NET_SOCK_BSD_ERR_DFLT

#define  NET_SOCK_BSD_RTN_CODE_OK                        NET_BSD_RTN_CODE_OK
#define  NET_SOCK_BSD_RTN_CODE_TIMEOUT                   NET_BSD_RTN_CODE_TIMEOUT
#define  NET_SOCK_BSD_RTN_CODE_CONN_CLOSED               NET_BSD_RTN_CODE_CONN_CLOSED

/********************************************************************************************************
 *                               NETWORK SOCKET (ARGUMENT) FLAG DEFINES
 *******************************************************************************************************/

#define  NET_SOCK_FLAG_NONE                       DEF_BIT_NONE

#define  NET_SOCK_FLAG_RX_DATA_PEEK               MSG_PEEK

#define  NET_SOCK_FLAG_NO_BLOCK                   MSG_DONTWAIT
#define  NET_SOCK_FLAG_RX_NO_BLOCK                NET_SOCK_FLAG_NO_BLOCK
#define  NET_SOCK_FLAG_TX_NO_BLOCK                NET_SOCK_FLAG_NO_BLOCK

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               NETWORK SOCKET IDENTIFICATION DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT16S NET_SOCK_ID;

/********************************************************************************************************
 *                               NETWORK SOCKET ADDRESS FAMILY DATA TYPE
 *******************************************************************************************************/

#define  NET_SOCK_ADDR_FAMILY_IP_V4         AF_INET             // TCP/IPv4 sock addr     family type.
#define  NET_SOCK_ADDR_FAMILY_IP_V6         AF_INET6            // TCP/IPv6 sock addr     family type.

typedef  CPU_INT16U NET_SOCK_ADDR_FAMILY;

/********************************************************************************************************
 *                                  NETWORK SOCKET ADDRESS DATA TYPES
 *******************************************************************************************************/

//                                                                 ------------ NET SOCK ADDR IPv4 ------------
#define  NET_SOCK_ADDR_IPv4_NBR_OCTETS_UNUSED        8

typedef  struct  net_sock_addr_ipv4 {
  NET_SOCK_ADDR_FAMILY AddrFamily;                                      ///< Sock addr family type
  NET_PORT_NBR         Port;                                            ///< UDP/TCP port nbr
  NET_IPv4_ADDR        Addr;                                            ///< IPv4 addr
  CPU_INT08U           Unused[NET_SOCK_ADDR_IPv4_NBR_OCTETS_UNUSED];    ///< Unused (MUST be zero).
} NET_SOCK_ADDR_IPv4;

#define  NET_SOCK_ADDR_IPv4_SIZE            (sizeof(NET_SOCK_ADDR_IPv4))

//                                                                 ------------ NET SOCK ADDR IPv6 ------------
typedef  struct  net_sock_addr_ipv6 {
  NET_SOCK_ADDR_FAMILY AddrFamily;                              ///< Sock addr family type
  NET_PORT_NBR         Port;                                    ///< UDP/TCP port nbr
  CPU_INT32U           FlowInfo;
  NET_IPv6_ADDR        Addr;                                    ///< IPv6 addr
  CPU_INT32U           ScopeID;                                 ///< Unused (MUST be zero).
} NET_SOCK_ADDR_IPv6;

#define  NET_SOCK_ADDR_IPv6_SIZE            (sizeof(NET_SOCK_ADDR_IPv6))

#ifdef NET_IPv6_MODULE_EN

#define  NET_SOCK_BSD_ADDR_LEN_MAX                  (NET_SOCK_ADDR_IPv6_SIZE  - sizeof(NET_SOCK_ADDR_FAMILY))

#elif (defined(NET_IPv4_MODULE_EN))

#define  NET_SOCK_BSD_ADDR_LEN_MAX                  (NET_SOCK_ADDR_IPv4_SIZE  - sizeof(NET_SOCK_ADDR_FAMILY))

#else
#define  NET_SOCK_BSD_ADDR_LEN_MAX                  0
#error
#endif

typedef  struct  net_sock_addr {
  NET_SOCK_ADDR_FAMILY AddrFamily;
  CPU_INT08U           Addr[NET_SOCK_BSD_ADDR_LEN_MAX];
} NET_SOCK_ADDR;

#define  NET_SOCK_ADDR_SIZE                 (sizeof(NET_SOCK_ADDR))

/********************************************************************************************************
 *                               NETWORK SOCKET ADDRESS LENGTH DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_SOCK_ADDR_LEN;

/********************************************************************************************************
 *                              NETWORK SOCKET PROTOCOL FAMILY DATA TYPE
 *******************************************************************************************************/

typedef  enum  net_sock_protocol_family {
  NET_SOCK_PROTOCOL_FAMILY_NONE = 0,
  NET_SOCK_PROTOCOL_FAMILY_IP_V4 = PF_INET,                     ///< TCP/IPv4 sock protocol family type.
  NET_SOCK_PROTOCOL_FAMILY_IP_V6 = PF_INET6                     ///< TCP/IPv6 sock protocol family type.
} NET_SOCK_PROTOCOL_FAMILY;

/********************************************************************************************************
 *                                  NETWORK SOCKET PROTOCOL DATA TYPE
 *******************************************************************************************************/

typedef  enum  net_sock_protocol {
  NET_SOCK_PROTOCOL_DFLT = 0,
  NET_SOCK_PROTOCOL_NONE = NET_SOCK_PROTOCOL_DFLT,

  NET_SOCK_PROTOCOL_TCP = IPPROTO_TCP,
  NET_SOCK_PROTOCOL_UDP = IPPROTO_UDP,
  NET_SOCK_PROTOCOL_IP = IPPROTO_IP,
  NET_SOCK_PROTOCOL_SOCK = SOL_SOCKET
} NET_SOCK_PROTOCOL;

/********************************************************************************************************
 *                              NETWORK SOCKET FAMILY DATA TYPE
 *******************************************************************************************************/

typedef  enum  net_sock_family {
  NET_SOCK_FAMILY_IP_V4 = NET_SOCK_PROTOCOL_FAMILY_IP_V4,
  NET_SOCK_FAMILY_IP_V6 = NET_SOCK_PROTOCOL_FAMILY_IP_V6
} NET_SOCK_FAMILY;

/********************************************************************************************************
 *                                    NETWORK SOCKET TYPE DATA TYPE
 *******************************************************************************************************/

typedef  enum net_sock_type {
  NET_SOCK_TYPE_NONE = 0,
  NET_SOCK_TYPE_DATAGRAM = SOCK_DGRAM,
  NET_SOCK_TYPE_STREAM = SOCK_STREAM
} NET_SOCK_TYPE;

/********************************************************************************************************
 *                                  NETWORK SOCKET QUANTITY DATA TYPE
 *
 * Note(s) : (1) See also 'NETWORK SOCKET IDENTIFICATION DATA TYPE  Note #1'.
 *******************************************************************************************************/

typedef  CPU_INT16S NET_SOCK_QTY;                               // Defines max qty of socks to support.

/********************************************************************************************************
 *                                 NETWORK SOCKET QUEUE SIZE DATA TYPE
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
 *                          & 'net_conn.h  NETWORK CONNECTION IDENTIFICATION DATA TYPE  Note #2b'.
 *
 *           (2) (a) NET_SOCK_Q_IX   #define's SHOULD be #define'd based on 'NET_SOCK_Q_SIZE'
 *                   data type declared.
 *
 *               (b) Since socket queue size is data typed as a 16-bit unsigned integer but the
 *                   maximum queue sizes are #define'd as 16-bit signed integer values ... :
 *
 *                   (1) Valid socket queue indices are #define'd within the range of     16-bit
 *                         signed integer values, ...
 *                   (2) but   socket queue indice exception values may be #define'd with 16-bit
 *                       unsigned integer values.
 *******************************************************************************************************/

typedef  CPU_INT16U NET_SOCK_Q_SIZE;

/********************************************************************************************************
 *                                   NETWORK SOCKET FLAGS DATA TYPES
 *
 * Note(s) : (1) Ideally, network socket API argument flags data type SHOULD be defined as   an unsigned
 *               integer data type since logical bitwise operations should be performed ONLY on unsigned
 *               integer data types.
 *******************************************************************************************************/

typedef  CPU_INT16S NET_SOCK_API_FLAGS;                         // See Note #1.

/********************************************************************************************************
 *                     NETWORK SOCKET DATA LENGTH & (ERROR) RETURN CODE DATA TYPES
 *
 * Note(s) : (1) IEEE Std 1003.1, 2004 Edition, Section 'sys/types.h : DESCRIPTION' states that :
 *
 *               (a) "ssize_t - Used for a count of bytes or an error indication."
 *
 *               (b) "ssize_t shall be [a] signed integer type ... capable of storing values at least in
 *                    the range [-1, {SSIZE_MAX}]."
 *
 *                   (1) IEEE Std 1003.1, 2004 Edition, Section 'limits.h : DESCRIPTION' states that the
 *                       "Minimum Acceptable Value ... [for] {SSIZE_MAX}" is "32767".
 *
 *                   (2) To avoid possible integer overflow, the network socket return code data type MUST
 *                       be declared as a signed integer data type with a maximum positive value greater
 *                       than or equal to all transport layers' maximum positive return value.
 *
 *                       See also 'net_udp.c  NetUDP_RxAppData()      Return(s)',
 *                                'net_udp.c  NetUDP_TxAppData()      Return(s)',
 *                                'net_tcp.c  NetTCP_RxAppData()      Return(s)',
 *                              & 'net_tcp.c  NetTCP_TxConnAppData()  Return(s)'.
 *
 *           (2) NET_SOCK_DATA_SIZE_MAX  SHOULD be #define'd based on 'NET_SOCK_DATA_SIZE' data type declared.
 *******************************************************************************************************/

typedef  CPU_INT32S NET_SOCK_DATA_SIZE;
typedef  NET_SOCK_DATA_SIZE NET_SOCK_RTN_CODE;

/********************************************************************************************************
 *                               NETWORK SOCKET ADDRESS LENGTH DATA TYPE
 *
 * Note(s) : (1) IEEE Std 1003.1, 2004 Edition, Section 'sys/socket.h : DESCRIPTION' states that
 *               "socklen_t ... is an integer type of width of at least 32 bits".
 *******************************************************************************************************/

typedef  CPU_INT32S NET_SOCK_OPT_LEN;

/********************************************************************************************************
 *                                 NETWORK SOCKET OPTION NAME DATA TYPES
 *******************************************************************************************************/

typedef  enum  net_sock_opt_name {
  NET_SOCK_OPT_SOCK_TX_BUF_SIZE = SO_SNDBUF,
  NET_SOCK_OPT_SOCK_RX_BUF_SIZE = SO_RCVBUF,
  NET_SOCK_OPT_SOCK_RX_TIMEOUT = SO_RCVTIMEO,
  NET_SOCK_OPT_SOCK_TX_TIMEOUT = SO_SNDTIMEO,
  NET_SOCK_OPT_SOCK_ERROR = SO_ERROR,
  NET_SOCK_OPT_SOCK_TYPE = SO_TYPE,
  NET_SOCK_OPT_SOCK_KEEP_ALIVE = SO_KEEPALIVE,
  NET_SOCK_OPT_SOCK_ACCEPT_CONN = SO_ACCEPTCONN,

  NET_SOCK_OPT_TCP_NO_DELAY = TCP_NODELAY,
  NET_SOCK_OPT_TCP_KEEP_CNT = TCP_KEEPCNT,
  NET_SOCK_OPT_TCP_KEEP_IDLE = TCP_KEEPIDLE,
  NET_SOCK_OPT_TCP_KEEP_INTVL = TCP_KEEPINTVL,

  NET_SOCK_OPT_IP_TOS = IP_TOS,
  NET_SOCK_OPT_IP_TTL = IP_TTL,
  NET_SOCK_OPT_IP_RX_IF = IP_RECVIF,
  NET_SOCK_OPT_IP_OPT = IP_OPTIONS,
  NET_SOCK_OPT_IP_HDR_INCL = IP_HDRINCL,
  NET_SOCK_OPT_IP_ADD_MEMBERSHIP = IP_ADD_MEMBERSHIP,
  NET_SOCK_OPT_IP_DROP_MEMBERSHIP = IP_DROP_MEMBERSHIP
} NET_SOCK_OPT_NAME;

/********************************************************************************************************
 *                                  NETWORK SOCKET TIMEOUT DATA TYPE
 *
 * Note(s) : (1) (a) IEEE Std 1003.1, 2004 Edition, Section 'sys/select.h : DESCRIPTION' states that "the
 *                   timeval structure ... includes at least the following members" :
 *
 *                   (1) time_t         tv_sec     Seconds
 *                   (2) suseconds_t    tv_usec    Microseconds
 *
 *               (b) Ideally, the Network Socket Layer's 'NET_SOCK_TIMEOUT' data type would be based on the
 *                   BSD 4.x Layer's 'timeval' data type definition.  However, since BSD 4.x Layer application
 *                   programming interface (API) is NOT guaranteed to be present in the project build (see
 *                   'net_bsd.h  MODULE  Note #1bA'); the Network Socket Layer's 'NET_SOCK_TIMEOUT' data type
 *                   MUST be independently defined.
 *
 *                   However, for correct interoperability between the BSD 4.x Layer 'timeval' data type &
 *                   the Network Socket Layer's 'NET_SOCK_TIMEOUT' data type; ANY modification to either of
 *                   these data types MUST be appropriately synchronized.
 *
 *               See also 'net_bsd.h  BSD 4.x SOCKET DATA TYPES  Note #4'.
 *******************************************************************************************************/

typedef  struct  net_sock_timeout {                                     // See Note #1a.
  CPU_INT32S timeout_sec;
  CPU_INT32S timeout_us;
} NET_SOCK_TIMEOUT;

/********************************************************************************************************
 *                      NETWORK SOCKET (IDENTIFICATION) DESCRIPTOR SET DATA TYPE
 *
 * Note(s) : (1) (a) (1) IEEE Std 1003.1, 2004 Edition, Section 'sys/select.h : DESCRIPTION' states
 *                       that the "'fd_set' type ... shall [be] define[d] ... as a structure".
 *
 *                   (2) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition,
 *                       6th Printing, Section 6.3, Pages 162-163 states that "descriptor sets [are]
 *                       typically an array of integers, with each bit in each integer corresponding
 *                       to a descriptor".
 *
 *               (b) Ideally, the Network Socket Layer's 'NET_SOCK_DESC' data type would be based on
 *                   the BSD 4.x Layer's 'fd_set' data type definition.  However, since BSD 4.x Layer
 *                   application programming interface (API) is NOT guaranteed to be present in the
 *                   project build (see 'net_bsd.h  MODULE  Note #1bA'); the Network Socket Layer's
 *                   'NET_SOCK_DESC' data type MUST be independently defined.
 *
 *                   However, for correct interoperability between the BSD 4.x Layer 'fd_set' data type
 *                   & the Network Socket Layer's 'NET_SOCK_DESC' data type; ANY modification to either
 *                   of these data types MUST be appropriately synchronized.
 *
 *               See also 'net_bsd.h  BSD 4.x SOCKET DATA TYPES  Note #5'.
 *******************************************************************************************************/

#define  NET_SOCK_DESC_NBR_MIN_DESC                               0
#define  NET_SOCK_DESC_NBR_MAX_DESC           NET_SOCK_NBR_SOCK

#define  NET_SOCK_DESC_NBR_MIN                                             0
#define  NET_SOCK_DESC_NBR_MAX               (NET_SOCK_DESC_NBR_MAX_DESC - 1)

#define  NET_SOCK_DESC_ARRAY_SIZE          (((NET_SOCK_DESC_NBR_MAX_DESC - 1) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)) + 1)

typedef  struct  net_sock_desc {                                                // See Note #1a.
  CPU_DATA SockID_DescNbrSet[NET_SOCK_DESC_ARRAY_SIZE];
} NET_SOCK_DESC;

/********************************************************************************************************
 *                       NETWORK SOCKET SECURITY CERTIFICATE & KEY INSTALLATION DEFINES
 *******************************************************************************************************/

typedef enum net_sock_secure_cert_key_fmt {
  NET_SOCK_SECURE_CERT_KEY_FMT_NONE,
  NET_SOCK_SECURE_CERT_KEY_FMT_PEM,
  NET_SOCK_SECURE_CERT_KEY_FMT_DER
} NET_SOCK_SECURE_CERT_KEY_FMT;

/********************************************************************************************************
 *                          NETWORK SECURE SOCKET FUNCTION POINTER DATA TYPE
 *******************************************************************************************************/

typedef enum net_sock_secure_untrusted_reason {
  NET_SOCK_SECURE_UNTRUSTED_BY_CA,
  NET_SOCK_SECURE_EXPIRE_DATE,
  NET_SOCK_SECURE_INVALID_DATE,
  NET_SOCK_SECURE_SELF_SIGNED,
  NET_SOCK_SECURE_UNKNOWN
} NET_SOCK_SECURE_UNTRUSTED_REASON;

typedef  CPU_BOOLEAN (*NET_SOCK_SECURE_TRUST_FNCT)(void *,
                                                   NET_SOCK_SECURE_UNTRUSTED_REASON);

/********************************************************************************************************
 ********************************************************************************************************
 *                                              MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                  NETWORK SOCKET DESCRIPTOR MACRO'S
 *
 * Description : Initialize, modify, & check network socket descriptor sets for multiplexed I/O functions.
 *
 * Argument(s) : desc_nbr    Socket descriptor number to initialize, modify, or check; when applicable.
 *
 *               p_desc_set  Pointer to a descriptor set.
 *
 * Return(s)   : Return values macro-dependent :
 *
 *                   none, for    network socket descriptor initialization & modification macro's.
 *
 *                   1,    if any network socket descriptor condition(s) satisfied.
 *
 *                   0,    otherwise.
 *
 *               See also 'net_bsd.h  BSD 4.x FILE DESCRIPTOR MACRO'S  Note #2a2'.
 *
 * Note(s)     : (1) Ideally, network socket descriptor macro's ('NET_SOCK_DESC_&&&()') would be based
 *                   on the BSD 4.x Layer's file descriptor macro ('FD_&&&()') definitions.  However,
 *                   since  BSD 4.x Layer application programming interface (API) is NOT guaranteed to
 *                   be present in the project build (see 'net_bsd.h  MODULE  Note #1bA'); the network
 *                   socket descriptor macro's MUST be independently defined.
 *
 *                   However, for correct interoperability between network socket descriptor macro's
 *                   & BSD 4.x Layer file descriptor macro's; ANY modification to any of these macro
 *                   definitions MUST be appropriately synchronized.
 *
 *                   See also 'net_bsd.h  BSD 4.x FILE DESCRIPTOR MACRO'S  Note #3'.
 *******************************************************************************************************/

#define  NET_SOCK_DESC_COPY(p_desc_set_dest, p_desc_set_src)                                   \
  do {                                                                                         \
    if ((((NET_SOCK_DESC *)(p_desc_set_dest)) != (NET_SOCK_DESC *)0)                           \
        && (((NET_SOCK_DESC *)(p_desc_set_src)) != (NET_SOCK_DESC *)0)) {                      \
      Mem_Copy((void *)     (&(((NET_SOCK_DESC *)(p_desc_set_dest))->SockID_DescNbrSet[0])),   \
               (void *)     (&(((NET_SOCK_DESC *)(p_desc_set_src))->SockID_DescNbrSet[0])),    \
               (CPU_SIZE_T)(sizeof(((NET_SOCK_DESC *)(p_desc_set_dest))->SockID_DescNbrSet))); \
    }                                                                                          \
  } while (0)

#define  NET_SOCK_DESC_INIT(p_desc_set)                                                  \
  do {                                                                                   \
    if (((NET_SOCK_DESC *)(p_desc_set)) != (NET_SOCK_DESC *)0) {                         \
      Mem_Clr((void *)     (&(((NET_SOCK_DESC *)(p_desc_set))->SockID_DescNbrSet[0])),   \
              (CPU_SIZE_T)(sizeof(((NET_SOCK_DESC *)(p_desc_set))->SockID_DescNbrSet))); \
    }                                                                                    \
  } while (0)

#define  NET_SOCK_DESC_CLR(desc_nbr, p_desc_set)                                                                              \
  do {                                                                                                                        \
    if (((desc_nbr) >= (NET_SOCK_ID)NET_SOCK_DESC_NBR_MIN)                                                                    \
        && ((desc_nbr) <= (NET_SOCK_ID)NET_SOCK_DESC_NBR_MAX)                                                                 \
        && (((NET_SOCK_DESC *)(p_desc_set)) != (NET_SOCK_DESC *)0)) {                                                         \
      DEF_BIT_CLR((((NET_SOCK_DESC *)(p_desc_set))->SockID_DescNbrSet[(desc_nbr) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)]), \
                  DEF_BIT((desc_nbr) % (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)));                                             \
    }                                                                                                                         \
  } while (0)

#define  NET_SOCK_DESC_SET(desc_nbr, p_desc_set)                                                                              \
  do {                                                                                                                        \
    if (((desc_nbr) >= (NET_SOCK_ID)NET_SOCK_DESC_NBR_MIN)                                                                    \
        && ((desc_nbr) <= (NET_SOCK_ID)NET_SOCK_DESC_NBR_MAX)                                                                 \
        && (((NET_SOCK_DESC *)(p_desc_set)) != (NET_SOCK_DESC *)0)) {                                                         \
      DEF_BIT_SET((((NET_SOCK_DESC *)(p_desc_set))->SockID_DescNbrSet[(desc_nbr) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)]), \
                  DEF_BIT((desc_nbr) % (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)));                                             \
    }                                                                                                                         \
  } while (0)

#define  NET_SOCK_DESC_IS_SET(desc_nbr, p_desc_set)                                                                                \
  ((((desc_nbr) >= (NET_SOCK_ID)NET_SOCK_DESC_NBR_MIN)                                                                             \
    && ((desc_nbr) <= (NET_SOCK_ID)NET_SOCK_DESC_NBR_MAX)                                                                          \
    && (((NET_SOCK_DESC *)(p_desc_set)) != (NET_SOCK_DESC *)0))                                                                    \
   ? (((DEF_BIT_IS_SET((((NET_SOCK_DESC *)(p_desc_set))->SockID_DescNbrSet[(desc_nbr) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)]), \
                       DEF_BIT((desc_nbr) % (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS))))                                             \
       == DEF_YES) ? 1 : 0)                                                                                                        \
   : 0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//                                                                 ------ SOCK API FNCTS ------
NET_SOCK_ID NetSock_Open(NET_SOCK_PROTOCOL_FAMILY protocol_family,
                         NET_SOCK_TYPE            sock_type,
                         NET_SOCK_PROTOCOL        protocol,
                         RTOS_ERR                 *p_err);

NET_SOCK_RTN_CODE NetSock_Close(NET_SOCK_ID sock_id,
                                RTOS_ERR    *p_err);

NET_SOCK_RTN_CODE NetSock_Bind(NET_SOCK_ID       sock_id,
                               NET_SOCK_ADDR     *paddr_local,
                               NET_SOCK_ADDR_LEN addr_len,
                               RTOS_ERR          *p_err);

NET_SOCK_RTN_CODE NetSock_Conn(NET_SOCK_ID       sock_id,
                               NET_SOCK_ADDR     *paddr_remote,
                               NET_SOCK_ADDR_LEN addr_len,
                               RTOS_ERR          *p_err);

#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
NET_SOCK_RTN_CODE NetSock_Listen(NET_SOCK_ID     sock_id,
                                 NET_SOCK_Q_SIZE sock_q_size,
                                 RTOS_ERR        *p_err);

NET_SOCK_ID NetSock_Accept(NET_SOCK_ID       sock_id,
                           NET_SOCK_ADDR     *paddr_remote,
                           NET_SOCK_ADDR_LEN *paddr_len,
                           RTOS_ERR          *p_err);
#endif

NET_SOCK_RTN_CODE NetSock_RxDataFrom(NET_SOCK_ID        sock_id,
                                     void               *pdata_buf,
                                     CPU_INT16U         data_buf_len,
                                     NET_SOCK_API_FLAGS flags,
                                     NET_SOCK_ADDR      *paddr_remote,
                                     NET_SOCK_ADDR_LEN  *paddr_len,
                                     void               *pip_opts_buf,
                                     CPU_INT08U         ip_opts_buf_len,
                                     CPU_INT08U         *pip_opts_len,
                                     RTOS_ERR           *p_err);

NET_SOCK_RTN_CODE NetSock_RxData(NET_SOCK_ID        sock_id,
                                 void               *pdata_buf,
                                 CPU_INT16U         data_buf_len,
                                 NET_SOCK_API_FLAGS flags,
                                 RTOS_ERR           *p_err);

NET_SOCK_RTN_CODE NetSock_TxDataTo(NET_SOCK_ID        sock_id,
                                   void               *p_data,
                                   CPU_INT16U         data_len,
                                   NET_SOCK_API_FLAGS flags,
                                   NET_SOCK_ADDR      *paddr_remote,
                                   NET_SOCK_ADDR_LEN  addr_len,
                                   RTOS_ERR           *p_err);

NET_SOCK_RTN_CODE NetSock_TxData(NET_SOCK_ID        sock_id,
                                 void               *p_data,
                                 CPU_INT16U         data_len,
                                 NET_SOCK_API_FLAGS flags,
                                 RTOS_ERR           *p_err);

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
NET_SOCK_RTN_CODE NetSock_Sel(NET_SOCK_QTY     sock_nbr_max,
                              NET_SOCK_DESC    *psock_desc_rd,
                              NET_SOCK_DESC    *psock_desc_wr,
                              NET_SOCK_DESC    *psock_desc_err,
                              NET_SOCK_TIMEOUT *ptimeout,
                              RTOS_ERR         *p_err);

void NetSock_SelAbort(NET_SOCK_ID sock_id,
                      RTOS_ERR    *p_err);
#endif

CPU_BOOLEAN NetSock_IsConn(NET_SOCK_ID sock_id,
                           RTOS_ERR    *p_err);

NET_CONN_ID NetSock_GetConnTransportID(NET_SOCK_ID sock_id,
                                       RTOS_ERR    *p_err);

//                                                                 ------ SOCK CFG FNCTS ------
//                                                                 Cfg sock block  mode.
CPU_BOOLEAN NetSock_CfgBlock(NET_SOCK_ID sock_id,
                             CPU_INT08U  block,
                             RTOS_ERR    *p_err);

CPU_INT08U NetSock_BlockGet(NET_SOCK_ID sock_id,
                            RTOS_ERR    *p_err);

#ifdef  NET_SECURE_MODULE_EN
//                                                                 Cfg sock secure mode.
CPU_BOOLEAN NetSock_CfgSecure(NET_SOCK_ID sock_id,
                              CPU_BOOLEAN secure,
                              RTOS_ERR    *p_err);

CPU_BOOLEAN NetSock_CfgSecureServerCertKeyInstall(NET_SOCK_ID                  sock_id,
                                                  const void                   *p_cert,
                                                  CPU_INT32U                   cert_len,
                                                  const void                   *p_key,
                                                  CPU_INT32U                   key_len,
                                                  NET_SOCK_SECURE_CERT_KEY_FMT fmt,
                                                  CPU_BOOLEAN                  cert_chain,
                                                  RTOS_ERR                     *p_err);

CPU_BOOLEAN NetSock_CfgSecureClientCertKeyInstall(NET_SOCK_ID                  sock_id,
                                                  const void                   *p_cert,
                                                  CPU_INT32U                   cert_len,
                                                  const void                   *p_key,
                                                  CPU_INT32U                   key_len,
                                                  NET_SOCK_SECURE_CERT_KEY_FMT fmt,
                                                  CPU_BOOLEAN                  cert_chain,
                                                  RTOS_ERR                     *p_err);

CPU_BOOLEAN NetSock_CfgSecureClientCommonName(NET_SOCK_ID sock_id,
                                              CPU_CHAR    *p_common_name,
                                              RTOS_ERR    *p_err);

CPU_BOOLEAN NetSock_CfgSecureClientTrustCallBack(NET_SOCK_ID                sock_id,
                                                 NET_SOCK_SECURE_TRUST_FNCT call_back_fnct,
                                                 RTOS_ERR                   *p_err);
#endif

//                                                                 Cfg interface socket.
CPU_BOOLEAN NetSock_CfgIF(NET_SOCK_ID sock_id,
                          NET_IF_NBR  if_nbr,
                          RTOS_ERR    *p_err);

//                                                                 Cfg sock rx Q size.
CPU_BOOLEAN NetSock_CfgRxQ_Size(NET_SOCK_ID        sock_id,
                                NET_SOCK_DATA_SIZE size,
                                RTOS_ERR           *p_err);
//                                                                 Cfg sock tx Q size.
CPU_BOOLEAN NetSock_CfgTxQ_Size(NET_SOCK_ID        sock_id,
                                NET_SOCK_DATA_SIZE size,
                                RTOS_ERR           *p_err);

//                                                                 Cfg/set sock conn child Q size.
CPU_BOOLEAN NetSock_CfgConnChildQ_SizeSet(NET_SOCK_ID     sock_id,
                                          NET_SOCK_Q_SIZE queue_size,
                                          RTOS_ERR        *p_err);

//                                                                 Get     sock conn child Q size.
NET_SOCK_Q_SIZE NetSock_CfgConnChildQ_SizeGet(NET_SOCK_ID sock_id,
                                              RTOS_ERR    *p_err);

#ifdef  NET_IPv4_MODULE_EN
//                                                                 Cfg sock tx IP TOS.
CPU_BOOLEAN NetSock_CfgTxIP_TOS(NET_SOCK_ID  sock_id,
                                NET_IPv4_TOS ip_tos,
                                RTOS_ERR     *p_err);

//                                                                 Cfg sock tx IP TTL.
CPU_BOOLEAN NetSock_CfgTxIP_TTL(NET_SOCK_ID  sock_id,
                                NET_IPv4_TTL ip_ttl,
                                RTOS_ERR     *p_err);

//                                                                 Cfg sock tx IP TTL multicast.
CPU_BOOLEAN NetSock_CfgTxIP_TTL_Multicast(NET_SOCK_ID  sock_id,
                                          NET_IPv4_TTL ip_ttl,
                                          RTOS_ERR     *p_err);
#endif // NET_IPv4_MODULE_EN

//                                                                 Cfg dflt sock rx Q   timeout.
CPU_BOOLEAN NetSock_CfgTimeoutRxQ_Dflt(NET_SOCK_ID sock_id,
                                       RTOS_ERR    *p_err);
//                                                                 Cfg/set  sock rx Q   timeout.
CPU_BOOLEAN NetSock_CfgTimeoutRxQ_Set(NET_SOCK_ID sock_id,
                                      CPU_INT32U  timeout_ms,
                                      RTOS_ERR    *p_err);
//                                                                 Get      sock rx Q   timeout.
CPU_INT32U NetSock_CfgTimeoutRxQ_Get_ms(NET_SOCK_ID sock_id,
                                        RTOS_ERR    *p_err);

//                                                                 Cfg dflt sock tx Q   timeout.
CPU_BOOLEAN NetSock_CfgTimeoutTxQ_Dflt(NET_SOCK_ID sock_id,
                                       RTOS_ERR    *p_err);
//                                                                 Cfg/set  sock tx Q   timeout.
CPU_BOOLEAN NetSock_CfgTimeoutTxQ_Set(NET_SOCK_ID sock_id,
                                      CPU_INT32U  timeout_ms,
                                      RTOS_ERR    *p_err);
//                                                                 Get      sock tx Q   timeout.
CPU_INT32U NetSock_CfgTimeoutTxQ_Get_ms(NET_SOCK_ID sock_id,
                                        RTOS_ERR    *p_err);

//                                                                 Cfg dflt sock conn   timeout.
CPU_BOOLEAN NetSock_CfgTimeoutConnReqDflt(NET_SOCK_ID sock_id,
                                          RTOS_ERR    *p_err);
//                                                                 Cfg/set  sock conn   timeout.
CPU_BOOLEAN NetSock_CfgTimeoutConnReqSet(NET_SOCK_ID sock_id,
                                         CPU_INT32U  timeout_ms,
                                         RTOS_ERR    *p_err);
//                                                                 Get      sock conn   timeout.
CPU_INT32U NetSock_CfgTimeoutConnReqGet_ms(NET_SOCK_ID sock_id,
                                           RTOS_ERR    *p_err);

//                                                                 Cfg dflt sock accept timeout.
CPU_BOOLEAN NetSock_CfgTimeoutConnAcceptDflt(NET_SOCK_ID sock_id,
                                             RTOS_ERR    *p_err);
//                                                                 Cfg/set  sock accept timeout.
CPU_BOOLEAN NetSock_CfgTimeoutConnAcceptSet(NET_SOCK_ID sock_id,
                                            CPU_INT32U  timeout_ms,
                                            RTOS_ERR    *p_err);
//                                                                 Get      sock accept timeout.
CPU_INT32U NetSock_CfgTimeoutConnAcceptGet_ms(NET_SOCK_ID sock_id,
                                              RTOS_ERR    *p_err);

//                                                                 Cfg dflt sock close  timeout.
CPU_BOOLEAN NetSock_CfgTimeoutConnCloseDflt(NET_SOCK_ID sock_id,
                                            RTOS_ERR    *p_err);
//                                                                 Cfg/set  sock close  timeout.
CPU_BOOLEAN NetSock_CfgTimeoutConnCloseSet(NET_SOCK_ID sock_id,
                                           CPU_INT32U  timeout_ms,
                                           RTOS_ERR    *p_err);
//                                                                 Get      sock close  timeout.
CPU_INT32U NetSock_CfgTimeoutConnCloseGet_ms(NET_SOCK_ID sock_id,
                                             RTOS_ERR    *p_err);

NET_SOCK_RTN_CODE NetSock_OptGet(NET_SOCK_ID       sock_id,
                                 NET_SOCK_PROTOCOL level,
                                 NET_SOCK_OPT_NAME opt_name,
                                 void              *popt_val,
                                 NET_SOCK_OPT_LEN  *popt_len,
                                 RTOS_ERR          *p_err);

NET_SOCK_RTN_CODE NetSock_OptSet(NET_SOCK_ID       sock_id,
                                 NET_SOCK_PROTOCOL level,
                                 NET_SOCK_OPT_NAME opt_name,
                                 void              *popt_val,
                                 NET_SOCK_OPT_LEN  opt_len,
                                 RTOS_ERR          *p_err);

NET_STAT_POOL NetSock_PoolStatGet(void);

void NetSock_PoolStatResetMaxUsed(void);

void NetSock_GetLocalIPAddr(NET_SOCK_ID     sock_id,
                            CPU_INT08U      *p_buf_addr,
                            NET_SOCK_FAMILY *p_family,
                            RTOS_ERR        *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_SOCK_H_
