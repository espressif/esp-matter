/***************************************************************************//**
 * @file
 * @brief Network - Bsd 4.X Layer
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_cfg_net.h>

#ifdef  NET_SOCK_BSD_EN
#include  <net/include/net_bsd.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_util.h>
#include  <net/source/util/net_dict_priv.h>
#include  <net/include/net_app.h>
#include  <net/include/net_bsd.h>

#include  "net_sock_priv.h"
#include  "net_ascii_priv.h"

#ifdef  NET_DNS_CLIENT_MODULE_EN
#include  <net/include/dns_client.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 *
 * Note(s) : (1) BSD 4.x global variables are required only for applications that call BSD 4.x functions.
 *
 *               See also 'MODULE  Note #1b'
 *                       & 'STANDARD BSD 4.x FUNCTION PROTOTYPES  Note #1'.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_BSD_SERVICE_FTP_DATA                      20
#define  NET_BSD_SERVICE_FTP                           21
#define  NET_BSD_SERVICE_TELNET                        23
#define  NET_BSD_SERVICE_SMTP                          25
#define  NET_BSD_SERVICE_DNS                           53
#define  NET_BSD_SERVICE_BOOTPS                        67
#define  NET_BSD_SERVICE_BOOTPC                        68
#define  NET_BSD_SERVICE_TFTP                          69
#define  NET_BSD_SERVICE_DNS                           53
#define  NET_BSD_SERVICE_HTTP                          80
#define  NET_BSD_SERVICE_NTP                          123
#define  NET_BSD_SERVICE_SNMP                         161
#define  NET_BSD_SERVICE_HTTPS                        443
#define  NET_BSD_SERVICE_SMTPS                        465

#define  NET_BSD_SERVICE_FTP_DATA_STR               "ftp_data"
#define  NET_BSD_SERVICE_FTP_STR                    "ftp"
#define  NET_BSD_SERVICE_TELNET_STR                 "telnet"
#define  NET_BSD_SERVICE_SMTP_STR                   "smtp"
#define  NET_BSD_SERVICE_DNS_STR                    "dns"
#define  NET_BSD_SERVICE_BOOTPS_STR                 "bootps"
#define  NET_BSD_SERVICE_BOOTPC_STR                 "bootpc"
#define  NET_BSD_SERVICE_TFTP_STR                   "tftp"
#define  NET_BSD_SERVICE_HTTP_STR                   "http"
#define  NET_BSD_SERVICE_SNMP_STR                   "snmp"
#define  NET_BSD_SERVICE_NTP_STR                    "ntp"
#define  NET_BSD_SERVICE_HTTPS_STR                  "https"
#define  NET_BSD_SERVICE_SMTPS_STR                  "smtps"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  NET_IPv4_MODULE_EN
//                                                                 See Note #1.
static CPU_CHAR NetBSD_IP_to_Str_Array[NET_ASCII_LEN_MAX_ADDR_IPv4];
#endif

static MEM_DYN_POOL NetBSD_AddrInfoPool;
static MEM_DYN_POOL NetBSD_SockAddrPool;

const NET_DICT NetBSD_ServicesStrTbl[] = {
  { NET_BSD_SERVICE_FTP_DATA, NET_BSD_SERVICE_FTP_DATA_STR, (sizeof(NET_BSD_SERVICE_FTP_DATA_STR) - 1) },
  { NET_BSD_SERVICE_FTP, NET_BSD_SERVICE_FTP_STR, (sizeof(NET_BSD_SERVICE_FTP_STR)      - 1) },
  { NET_BSD_SERVICE_TELNET, NET_BSD_SERVICE_TELNET_STR, (sizeof(NET_BSD_SERVICE_TELNET_STR)   - 1) },
  { NET_BSD_SERVICE_SMTP, NET_BSD_SERVICE_SMTP_STR, (sizeof(NET_BSD_SERVICE_SMTP_STR)     - 1) },
  { NET_BSD_SERVICE_DNS, NET_BSD_SERVICE_DNS_STR, (sizeof(NET_BSD_SERVICE_DNS_STR)      - 1) },
  { NET_BSD_SERVICE_BOOTPS, NET_BSD_SERVICE_BOOTPS_STR, (sizeof(NET_BSD_SERVICE_BOOTPS_STR)   - 1) },
  { NET_BSD_SERVICE_BOOTPC, NET_BSD_SERVICE_BOOTPC_STR, (sizeof(NET_BSD_SERVICE_BOOTPC_STR)   - 1) },
  { NET_BSD_SERVICE_TFTP, NET_BSD_SERVICE_TFTP_STR, (sizeof(NET_BSD_SERVICE_TFTP_STR)     - 1) },
  { NET_BSD_SERVICE_HTTP, NET_BSD_SERVICE_HTTP_STR, (sizeof(NET_BSD_SERVICE_HTTP_STR)     - 1) },
  { NET_BSD_SERVICE_SNMP, NET_BSD_SERVICE_SNMP_STR, (sizeof(NET_BSD_SERVICE_SNMP_STR)     - 1) },
  { NET_BSD_SERVICE_NTP, NET_BSD_SERVICE_NTP_STR, (sizeof(NET_BSD_SERVICE_NTP_STR)      - 1) },
  { NET_BSD_SERVICE_HTTPS, NET_BSD_SERVICE_HTTPS_STR, (sizeof(NET_BSD_SERVICE_HTTPS_STR)    - 1) },
  { NET_BSD_SERVICE_SMTPS, NET_BSD_SERVICE_SMTPS_STR, (sizeof(NET_BSD_SERVICE_SMTPS_STR)    - 1) },
};

typedef  enum   net_bsd_sock_protocol {
  NET_BSD_SOCK_PROTOCOL_UNKNOWN,
  NET_BSD_SOCK_PROTOCOL_UDP,
  NET_BSD_SOCK_PROTOCOL_TCP,
  NET_BSD_SOCK_PROTOCOL_TCP_UDP,
  NET_BSD_SOCK_PROTOCOL_UDP_TCP,
} NET_BSD_SOCK_PROTOCOL;

typedef  struct net_bsd_service_proto {
  NET_PORT_NBR          Port;
  NET_BSD_SOCK_PROTOCOL Protocol;
} NET_BSD_SERVICE_PROTOCOL;

const NET_BSD_SERVICE_PROTOCOL NetBSD_ServicesProtocolTbl[] = {
  { NET_BSD_SERVICE_FTP_DATA, NET_BSD_SOCK_PROTOCOL_TCP     },
  { NET_BSD_SERVICE_FTP, NET_BSD_SOCK_PROTOCOL_TCP     },
  { NET_BSD_SERVICE_TELNET, NET_BSD_SOCK_PROTOCOL_TCP     },
  { NET_BSD_SERVICE_SMTP, NET_BSD_SOCK_PROTOCOL_TCP     },
  { NET_BSD_SERVICE_DNS, NET_BSD_SOCK_PROTOCOL_UDP_TCP },
  { NET_BSD_SERVICE_BOOTPS, NET_BSD_SOCK_PROTOCOL_UDP_TCP },
  { NET_BSD_SERVICE_BOOTPC, NET_BSD_SOCK_PROTOCOL_UDP_TCP },
  { NET_BSD_SERVICE_TFTP, NET_BSD_SOCK_PROTOCOL_UDP     },
  { NET_BSD_SERVICE_HTTP, NET_BSD_SOCK_PROTOCOL_TCP_UDP },
  { NET_BSD_SERVICE_SNMP, NET_BSD_SOCK_PROTOCOL_TCP_UDP },
  { NET_BSD_SERVICE_NTP, NET_BSD_SOCK_PROTOCOL_TCP_UDP },
  { NET_BSD_SERVICE_HTTPS, NET_BSD_SOCK_PROTOCOL_TCP_UDP },
  { NET_BSD_SERVICE_SMTPS, NET_BSD_SOCK_PROTOCOL_TCP     },
};

#ifdef  NET_IPv6_MODULE_EN
const struct  in6_addr in6addr_any = NET_IPv6_ADDR_ANY_INIT;
const struct  in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;
const struct  in6_addr in6addr_linklocal_allnodes = IN6ADDR_LINKLOCAL_ALLNODES_INIT;
const struct  in6_addr in6addr_linklocal_allrouters = IN6ADDR_LINKLOCAL_ALLROUTERS_INIT;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPTES
 ********************************************************************************************************
 *******************************************************************************************************/

static struct  addrinfo *NetBSD_AddrInfoGet(struct  addrinfo **pp_head,
                                            struct  addrinfo **pp_tail);

static void NetBSD_AddrInfoFree(struct  addrinfo *p_addrinfo);

static CPU_BOOLEAN NetBSD_AddrInfoSet(struct  addrinfo     *p_addrinfo,
                                      NET_SOCK_ADDR_FAMILY family,
                                      NET_PORT_NBR         port_nbr,
                                      CPU_INT08U           *p_addr,
                                      NET_IP_ADDR_LEN      addr_len,
                                      NET_SOCK_PROTOCOL    protocol,
                                      CPU_CHAR             *p_canonname);

static void NetBSD_AddrCfgValidate(CPU_BOOLEAN *p_ipv4_cfgd,
                                   CPU_BOOLEAN *p_ipv6_cfgd);
#endif // NET_SOCK_BSD_EN

/********************************************************************************************************
 *                                       STANDARD BSD 4.x FUNCTIONS
 *
 * Note(s) : (1) BSD 4.x function definitions are required only for applications that call BSD 4.x functions.
 *               See 'net_bsd.h  MODULE  Note #1b3'
 *               & 'net_bsd.h  STANDARD BSD 4.x FUNCTION PROTOTYPES  Note #1'.
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetBSD_Init()
 *
 * @brief    Initialize BSD module.
 *
 * @param    p_err   Pointer to variable that will hold the return error code from this function.
 *******************************************************************************************************/

#ifdef  NET_SOCK_BSD_EN
void NetBSD_Init(RTOS_ERR *p_err)
{
  RTOS_ERR err;

  Mem_DynPoolCreate("BSD AddrInfo Pool",
                    &NetBSD_AddrInfoPool,
                    DEF_NULL,
                    sizeof(struct addrinfo),
                    sizeof(CPU_SIZE_T),
                    0,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    goto exit;
  }

  Mem_DynPoolCreate("BSD AddrInfo Pool",
                    &NetBSD_SockAddrPool,
                    DEF_NULL,
                    sizeof(struct sockaddr),
                    sizeof(CPU_SIZE_T),
                    0,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    goto exit;
  }

exit:
  return;
}
#endif

#ifdef NET_SOCK_BSD_EN
/****************************************************************************************************//**
 *                                                   socket()
 *
 * @brief    Create a socket.
 *
 * @param    protocol_family     Socket protocol family :
 *                                   - PF_INET     Internet Protocol version 4 (IPv4).
 *                                   - PF_INET6    Internet Protocol version 6 (IPv6).
 *
 * @param    sock_type           Socket type :
 *                                   - SOCK_DGRAM              Datagram-type socket.
 *                                   - SOCK_STREAM             Stream  -type socket.
 *
 * @param    protocol            _family     Socket protocol family :
 *                                   - PF_INET     Internet Protocol version 4 (IPv4).
 *                                   - PF_INET6    Internet Protocol version 6 (IPv6).
 *
 * @return   Socket descriptor/handle identifier, if NO error(s).
 *           -1, otherwise.
 *******************************************************************************************************/
int socket(int protocol_family,
           int sock_type,
           int protocol)
{
  int      rtn_code;
  RTOS_ERR local_err;

  rtn_code = (int)NetSock_Open((NET_SOCK_PROTOCOL_FAMILY)protocol_family,
                               (NET_SOCK_TYPE)sock_type,
                               (NET_SOCK_PROTOCOL)protocol,
                               &local_err);

  return (rtn_code);
}

/****************************************************************************************************//**
 *                                                   close()
 *
 * @brief    Close a socket.
 *
 * @param    sock_id     Socket descriptor/handle identifier of socket to close.
 *
 * @return    0, if NO error(s).
 *           -1, otherwise.
 *
 * @note     (1) Once an application closes its socket, NO further operations on the socket are allowed
 *               & the application MUST NOT continue to access the socket.
 *******************************************************************************************************/
int close(int sock_id)
{
  int      rtn_code;
  RTOS_ERR local_err;

  rtn_code = (int)NetSock_Close(sock_id,
                                &local_err);

  return (rtn_code);
}

/****************************************************************************************************//**
 *                                                   bind()
 *
 * @brief    Bind a socket to a local address.
 *
 * @param    sock_id         Socket descriptor/handle identifier of socket to bind to a local address.
 *
 * @param    p_addr_local    Pointer to socket address structure.
 *
 * @param    addr_len        Length  of socket address structure (in octets).
 *
 * @return    0, if NO error(s).
 *           -1, otherwise.
 *
 * @note     (1) Socket address structure 'sa_family' member MUST be configured in host-order &
 *               MUST NOT be converted to/from network-order.
 *
 * @note     (2) Socket address structure addresses MUST be configured/converted from host-order
 *               to network-order.
 *******************************************************************************************************/
int bind(int              sock_id,
         struct  sockaddr *p_addr_local,
         socklen_t        addr_len)
{
  int      rtn_code;
  RTOS_ERR local_err;

  rtn_code = (int)NetSock_Bind(sock_id,
                               (NET_SOCK_ADDR *)p_addr_local,
                               addr_len,
                               &local_err);

  return (rtn_code);
}

/****************************************************************************************************//**
 *                                                   connect()
 *
 * @brief    Connect a socket to a remote server.
 *
 * @param    sock_id         Socket descriptor/handle identifier of socket to connect.
 *
 * @param    p_addr_remote   Pointer to socket address structure (see Note #1).
 *
 * @param    addr_len        Length  of socket address structure (in octets).
 *
 * @return    0, if NO error(s).
 *           -1, otherwise.
 *
 * @note     (1) Socket address structure 'sa_family' member MUST be configured in host-order &
 *               MUST NOT be converted to/from network-order.
 *
 * @note     (2) Socket address structure addresses MUST be configured/converted from host-order
 *               to network-order.
 *******************************************************************************************************/
int connect(int              sock_id,
            struct  sockaddr *p_addr_remote,
            socklen_t        addr_len)
{
  int      rtn_code;
  RTOS_ERR local_err;

  rtn_code = (int)NetSock_Conn(sock_id,
                               (NET_SOCK_ADDR *)p_addr_remote,
                               addr_len,
                               &local_err);

  return (rtn_code);
}

/****************************************************************************************************//**
 *                                                   listen()
 *
 * @brief    Set socket to listen for connection requests.
 *
 * @param    sock_id         Socket descriptor/handle identifier of socket to listen.
 *
 * @param    sock_q_size     Number of connection requests to queue on listen socket.
 *
 * @return    0, if NO error(s).
 *           -1, otherwise.
 *******************************************************************************************************/

#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
int listen(int sock_id,
           int sock_q_size)
{
  int      rtn_code;
  RTOS_ERR local_err;

  rtn_code = (int)NetSock_Listen(sock_id,
                                 sock_q_size,
                                 &local_err);

  return (rtn_code);
}
#endif

/****************************************************************************************************//**
 *                                                   accept()
 *
 * @brief    Get a new socket accepted from a socket set to listen for connection requests.
 *
 * @param    sock_id         Socket descriptor/handle identifier of listen socket.
 *
 * @param    p_addr_remote   Pointer to an address buffer that will receive the socket address structure
 *                           of the accepted socket's remote address, if NO error(s).
 *
 * @param    p_addr_len      Pointer to a variable to pass the size of the address buffer pointed to
 *                           by 'p_addr_remote' and return the actual size of socket address
 *                           structure with the accepted socket's remote address, if NO error(s);
 *                           Return 0, otherwise.
 *
 * @return   Socket descriptor/handle identifier of new accepted socket, if NO error(s).
 *           -1, otherwise.
 *
 * @note     (1) Socket address structure 'sa_family' member returned in host-order & SHOULD NOT
 *               be converted to network-order.
 *
 * @note     (2) Socket address structure addresses returned in network-order & SHOULD be converted
 *               from network-order to host-order.
 *******************************************************************************************************/
#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
int accept(int              sock_id,
           struct  sockaddr *p_addr_remote,
           socklen_t        *p_addr_len)
{
  int               rtn_code;
  NET_SOCK_ADDR_LEN addr_len;
  RTOS_ERR          local_err;

  addr_len = (NET_SOCK_ADDR_LEN)*p_addr_len;
  rtn_code = (int)NetSock_Accept((NET_SOCK_ID) sock_id,
                                 (NET_SOCK_ADDR *) p_addr_remote,
                                 (NET_SOCK_ADDR_LEN *)&addr_len,
                                 &local_err);

  *p_addr_len = (socklen_t)addr_len;

  return (rtn_code);
}
#endif

/****************************************************************************************************//**
 *                                               recvfrom()
 *
 * @brief    Receive data from a socket.
 *
 * @param    sock_id         Socket descriptor/handle identifier of socket to receive data.
 *
 * @param    p_data_buf      Pointer to an application data buffer that will receive the socket's received
 *                           data.
 *
 * @param    data_buf_len    Size of the   application data buffer (in octets).
 *
 * @param    flags           Flags to select receive options; bit-field flags logically OR'd :
 *                               - 0                           No socket flags selected.
 *                               - MSG_PEEK                    Receive socket data without consuming the socket data.
 *                               - MSG_DONTWAIT                Receive socket data without blocking.
 *
 * @param    p_addr_remote   Pointer to an address buffer that will receive the socket address structure
 *                           with the received data's remote address, if NO error(s).
 *
 * @param    p_addr_len      Pointer to a variable to pass the size of the address buffer pointed to
 *                           by 'p_addr_remote' and return the actual size of socket address structure
 *                           with the received data's remote address, if NO error(s). Return 0, otherwise.
 *
 * @return   Number of positive data octets received, if NO error(s).
 *           0,  if socket connection closed.
 *           -1,  otherwise.
 *
 * @note     (1) Socket types:
 *           - (a) Datagram-type sockets
 *               - (1) Datagram-type sockets transmit & receive all data atomically -- i.e. every
 *                     single, complete datagram transmitted MUST be received as a single, complete
 *                     datagram.
 *               - (2) Thus if the socket's type is datagram & the receive data buffer size is
 *                     NOT large enough for the received data, the receive data buffer is maximally
 *                     filled with receive data but the remaining data octets are discarded &
 *                     NET_SOCK_ERR_INVALID_DATA_SIZE error is returned.
 *           - (b) Stream-type sockets
 *               - (1) Stream-type sockets transmit & receive all data octets in one or more
 *                     non-distinct packets.  In other words, the application data is NOT
 *                     bounded by any specific packet(s); rather, it is contiguous & sequenced
 *                     from one packet to the next.
 *                     @n
 *                     Thus if the socket's type is stream & the receive data buffer size is NOT
 *                     large enough for the received data, the receive data buffer is maximally
 *                     filled with receive data & the remaining data octets remain queued for
 *                     later application-socket receives.
 *               - (2) Thus it is typical -- but NOT absolutely required -- that a single application
 *                     task ONLY receive or request to receive data from a stream-type socket.
 *
 * @note     (2) Only some socket receive flag options are implemented.  If other flag options are requested,
 *               socket receive handler function(s) abort & return appropriate error codes so that requested
 *               flag options are NOT silently ignored.
 *
 * @note     (3) Socket address structure 'sa_family' member returned in host-order & SHOULD NOT
 *               be converted to network-order.
 *               @n
 *               Socket address structure addresses returned in network-order & SHOULD be converted
 *               from network-order to host-order.
 *
 * @note     (4) IEEE Std 1003.1, 2004 Edition, Section 'recvfrom() : RETURN VALUE' states that :
 *           - (a) "Upon successful completion, recvfrom() shall return the length of the message in
 *                 bytes."
 *           - (b) "If no messages are available to be received and the peer has performed an orderly
 *                 shutdown, recvfrom() shall return 0."
 *           - (c) "Otherwise, [-1 shall be returned] and 'errno' set to indicate the error."
 *                 'errno' NOT currently supported.
 *******************************************************************************************************/
ssize_t recvfrom(int              sock_id,
                 void             *p_data_buf,
                 _size_t          data_buf_len,
                 int              flags,
                 struct  sockaddr *p_addr_remote,
                 socklen_t        *p_addr_len)
{
  ssize_t           rtn_code;
  NET_SOCK_ADDR_LEN addr_len;
  RTOS_ERR          local_err;

  if (data_buf_len > DEF_INT_16U_MAX_VAL) {
    return (NET_BSD_ERR_DFLT);
  }

  addr_len = (NET_SOCK_ADDR_LEN)*p_addr_len;
  rtn_code = (ssize_t)NetSock_RxDataFrom((NET_SOCK_ID) sock_id,
                                         (void *) p_data_buf,
                                         (CPU_INT16U) data_buf_len,
                                         (NET_SOCK_API_FLAGS) flags,
                                         (NET_SOCK_ADDR *) p_addr_remote,
                                         (NET_SOCK_ADDR_LEN *)&addr_len,
                                         DEF_NULL,
                                         (CPU_INT08U) 0u,
                                         DEF_NULL,
                                         &local_err);

  *p_addr_len = (socklen_t)addr_len;

  return (rtn_code);
}

/****************************************************************************************************//**
 *                                                   recv()
 *
 * @brief    Receive data from a socket.
 *
 * @param    sock_id         Socket descriptor/handle identifier of socket to receive data.
 *
 * @param    p_data_buf      Pointer to an application data buffer that will receive the socket's received
 *                           data.
 *
 * @param    data_buf_len    Size of the application data buffer (in octets).
 *
 * @param    flags           Flags to select receive options; bit-field flags logically OR'd :
 *                               - 0               No socket flags selected.
 *                               - MSG_PEEK        Receive socket data without consuming the socket data.
 *                               - MSG_DONTWAIT    Receive socket data without blocking.
 *
 * @return   Number of positive data octets received, if NO error(s).
 *           0, if socket connection closed.
 *           -1, otherwise.
 *
 * @note     (1) Socket types:
 *               - (a) Datagram-type sockets
 *                   - (1) Datagram-type sockets transmit & receive all data atomically -- i.e. every
 *                           single, complete datagram transmitted MUST be received as a single, complete
 *                           datagram.
 *                   - (2) Thus if the socket's type is datagram & the receive data buffer size is
 *                           NOT large enough for the received data, the receive data buffer is maximally
 *                           filled with receive data but the remaining data octets are discarded &
 *                           RTOS_ERR_WOULD_OVF error is returned.
 *               - (b) Stream-type sockets
 *                   - (1) Stream-type sockets transmit & receive all data octets in one or more
 *                           non-distinct packets.  In other words, the application data is NOT
 *                           bounded by any specific packet(s); rather, it is contiguous & sequenced
 *                           from one packet to the next.
 *                           @n
 *                           Thus if the socket's type is stream & the receive data buffer size is NOT
 *                           large enough for the received data, the receive data buffer is maximally
 *                           filled with receive data & the remaining data octets remain queued for
 *                           later application-socket receives.
 *                   - (2) Thus it is typical -- but NOT absolutely required -- that a single application
 *                           task ONLY receive or request to receive data from a stream-type socket.
 *
 * @note     (2) Only some socket receive flag options are implemented. If other flag options are requested,
 *               socket receive handler function(s) abort & return appropriate error codes so that requested
 *               flag options are NOT silently ignored.
 *
 * @note     (3) IEEE Std 1003.1, 2004 Edition, Section 'recv() : RETURN VALUE' states that :
 *               - (a) "Upon successful completion, recv() shall return the length of the message in bytes."
 *               - (b) "If no messages are available to be received and the peer has performed an orderly
 *                     shutdown, recv() shall return 0."
 *               - (c) "Otherwise, [-1 shall be returned] and 'errno' set to indicate the error."
 *                     'errno' NOT currently supported.
 *******************************************************************************************************/
ssize_t recv(int     sock_id,
             void    *p_data_buf,
             _size_t data_buf_len,
             int     flags)
{
  ssize_t  rtn_code;
  RTOS_ERR local_err;

  if (data_buf_len > DEF_INT_16U_MAX_VAL) {
    return (NET_BSD_ERR_DFLT);
  }

  rtn_code = (ssize_t)NetSock_RxData((NET_SOCK_ID) sock_id,
                                     (void *) p_data_buf,
                                     (CPU_INT16U) data_buf_len,
                                     (NET_SOCK_API_FLAGS) flags,
                                     &local_err);

  return (rtn_code);
}

/****************************************************************************************************//**
 *                                                   sendto()
 *
 * @brief    Send data through a socket.
 *
 * @param    sock_id         Socket descriptor/handle identifier of socket to send data.
 *
 * @param    p_data          Pointer to application data to send.
 *
 * @param    data_len        Length of  application data to send (in octets).
 *
 * @param    flags           Flags to select send options; bit-field flags logically OR'd :
 *                           0               No socket flags selected.
 *                           MSG_DONTWAIT    Send socket data without blocking.
 *
 * @param    p_addr_remote   Pointer to destination address buffer;
 *                           required for datagram sockets, optional for stream sockets.
 *
 * @param    addr_len        Length of  destination address buffer (in octets).
 *
 * @return   Number of positive data octets sent, if NO error(s).
 *           0, if socket connection closed.
 *           -1, otherwise.
 *
 * @note     (1) Socket Types:
 *               - (a) Datagram-type sockets
 *                   - (1) Datagram-type sockets send & receive all data atomically -- i.e. every single,
 *                         complete datagram  sent MUST be received as a single, complete datagram.
 *                         Thus each call to send data MUST be transmitted in a single, complete datagram.
 *                         @n
 *                         Since IP transmit fragmentation is NOT currently supported, if the socket's
 *                         type is datagram & the requested send data length is greater than the
 *                         socket/transport layer MTU, then NO data is sent & RTOS_ERR_WOULD_OVF
 *                         error is returned.
 *               - (b) Stream-type sockets
 *                   - (1) Stream-type sockets send & receive all data octets in one or more non-
 *                         distinct packets.  In other words, the application data is NOT bounded
 *                         by any specific packet(s); rather, it is contiguous & sequenced from
 *                         one packet to the next.
 *                         @n
 *                         Thus if the socket's type is stream & the socket's send data queue(s) are
 *                         NOT large enough for the send data, the send data queue(s) are maximally
 *                         filled with send data & the remaining data octets are discarded but may be
 *                         re-sent by later application-socket sends.
 *                         @n
 *                         Therefore, NO stream-type socket send data length should be "too long to
 *                         pass through the underlying protocol" & cause the socket send to "fail ...
 *                         [with] no data ... transmitted".
 *                         @n
 *                         Thus it is typical -- but NOT absolutely required -- that a single application
 *                         task ONLY send or request to send data to a stream-type socket.
 *               - (c) 'data_len' of 0 octets NOT allowed.
 *
 * @note     (2) Only some socket send flag options are implemented.  If other flag options are requested,
 *               socket send handler function(s) abort & return appropriate error codes so that requested
 *               flag options are NOT silently ignored.
 *
 * @note     (3) Socket address structure 'sa_family' member MUST be configured in host-order &
 *               MUST NOT be converted to/from network-order.
 *
 *               Socket address structure addresses MUST be configured/converted from host-order
 *               to network-order.
 *
 * @note     (4) See IEEE Std 1003.1, 2004 Edition, Section 'sendto() : RETURN VALUE'
 *
 * @note     (5) Although NO socket send() specification states to return '0' when the socket's
 *               connection is closed, it seems reasonable to return '0' since it is possible for the
 *               socket connection to be close()'d or shutdown() by the remote host.
 *******************************************************************************************************/
ssize_t sendto(int              sock_id,
               void             *p_data,
               _size_t          data_len,
               int              flags,
               struct  sockaddr *p_addr_remote,
               socklen_t        addr_len)
{
  ssize_t  rtn_code;
  RTOS_ERR local_err;

  rtn_code = (ssize_t)NetSock_TxDataTo((NET_SOCK_ID) sock_id,
                                       (void *) p_data,
                                       (CPU_INT16U) data_len,
                                       (NET_SOCK_API_FLAGS) flags,
                                       (NET_SOCK_ADDR *) p_addr_remote,
                                       (NET_SOCK_ADDR_LEN) addr_len,
                                       &local_err);

  return (rtn_code);
}

/****************************************************************************************************//**
 *                                                   send()
 *
 * @brief    Send data through a socket.
 *
 * @param    sock_id     Socket descriptor/handle identifier of socket to send data.
 *
 * @param    p_data      Pointer to application data to send.
 *
 * @param    data_len    Length of  application data to send (in octets).
 *
 * @param    flags       Flags to select send options; bit-field flags logically OR'd :
 *                       0               No socket flags selected.
 *                       MSG_DONTWAIT    Send socket data without blocking.
 *
 * @return   Number of positive data octets sent, if NO error(s).
 *           0, if socket connection closed.
 *           -1, otherwise.
 *
 * @note     (1) Socket types:
 *               - (a) Datagram-type sockets send & receive all data atomically -- i.e. every single,
 *                     complete datagram  sent MUST be received as a single, complete datagram.
 *                     Thus each call to send data MUST be transmitted in a single, complete datagram.
 *                     @n
 *                     IEEE Std 1003.1, 2004 Edition, Section 'send() : DESCRIPTION' states that
 *                     "if the message is too long to pass through the underlying protocol, send()
 *                     shall fail and no data shall be transmitted".
 *                     @n
 *                     Since IP transmit fragmentation is NOT currently supported (see 'net_ip.h
 *                     Note #1d'), if the socket's type is datagram & the requested send data
 *                     length is greater than the socket/transport layer MTU, then NO data is
 *                     sent & NET_SOCK_ERR_INVALID_DATA_SIZE error is returned.
 *               - (b) Stream-type sockets send & receive all data octets in one or more non-
 *                     distinct packets.  In other words, the application data is NOT bounded
 *                     by any specific packet(s); rather, it is contiguous & sequenced from
 *                     one packet to the next.
 *                     @n
 *                     Thus if the socket's type is stream & the socket's send data queue(s) are
 *                     NOT large enough for the send data, the send data queue(s) are maximally
 *                     filled with send data & the remaining data octets are discarded but may be
 *                     re-sent by later application-socket sends.
 *                     @n
 *                     Therefore, NO stream-type socket send data length should be "too long to
 *                     pass through the underlying protocol" & cause the socket send to "fail ...
 *                     [with] no data ... transmitted".
 *                     @n
 *                     Thus it is typical -- but NOT absolutely required -- that a single application
 *                     task ONLY send or request to send data to a stream-type socket.
 *               - (c) 'data_len' of 0 octets NOT allowed.
 *
 * @note     (2) Only some socket send flag options are implemented.  If other flag options are requested,
 *               socket send handler function(s) abort & return appropriate error codes so that requested
 *               flag options are NOT silently ignored.
 *
 * @note     (3) See IEEE Std 1003.1, 2004 Edition, Section 'send() : RETURN VALUE'
 *
 * @note     (4) Although NO socket send() specification states to return '0' when the socket's
 *               connection is closed, it seems reasonable to return '0' since it is possible for the
 *               socket connection to be close()'d or shutdown() by the remote host.
 *******************************************************************************************************/
ssize_t send(int     sock_id,
             void    *p_data,
             _size_t data_len,
             int     flags)
{
  ssize_t  rtn_code;
  RTOS_ERR local_err;

  rtn_code = (ssize_t)NetSock_TxData((NET_SOCK_ID) sock_id,
                                     (void *) p_data,
                                     (CPU_INT16U) data_len,
                                     (NET_SOCK_API_FLAGS) flags,
                                     &local_err);

  return (rtn_code);
}

/****************************************************************************************************//**
 *                                                   select()
 *
 * @brief    Check multiple file descriptors for available resources &/or operations.
 *
 * @param    desc_nbr_max    Maximum number of file descriptors in the file descriptor sets.
 *
 * @param    p_desc_rd       Pointer to a set of file descriptors to :
 *                               - (a) Check for available read operation(s).
 *                               - (b) Either:
 *                               - (1) Return the actual file descriptors ready for available
 *                                     read  operation(s), if NO error(s);
 *                               - (2) Return the initial, non-modified set of file descriptors,
 *                                     on any error(s);
 *                               - (3) Return a null-valued (i.e. zero-cleared) descriptor set,
 *                                     if any timeout expires.
 *
 * @param    p_desc_wr       Pointer to a set of file descriptors to :
 *                               - (a) Check for available write operation(s).
 *                               - (b) Either:
 *                                   - (1) Return the actual file descriptors ready for available
 *                                         write operation(s), if NO error(s);
 *                                   - (2) Return the initial, non-modified set of file descriptors,
 *                                         on any error(s);
 *                                   - (3) Return a null-valued (i.e. zero-cleared) descriptor set,
 *                                         if any timeout expires .
 *
 * @param    p_desc_err      Pointer to a set of file descriptors to :
 *                               - (a) Check for any error(s) &/or exception(s).
 *                               - (b) Either:
 *                                   - (1) Return the actual file descriptors flagged with any error(s)
 *                                         &/or exception(s), if NO non-descriptor-related error(s);
 *                                   - (2) Return the initial, non-modified set of file descriptors,
 *                                         on any error(s);
 *                                   - (3) Return a null-valued (i.e. zero-cleared) descriptor set,
 *                                         if any timeout expires.
 *
 * @param    p_timeout       Pointer to a timeout.
 *
 * @return   Number of file descriptors with available resources &/or operations, if any.
 *           0, on timeout.
 *           -1, otherwise.
 *
 * @note     (1) See notes in NetSock_Sel()
 *******************************************************************************************************/

#if    (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
int select(int             desc_nbr_max,
           struct  fd_set  *p_desc_rd,
           struct  fd_set  *p_desc_wr,
           struct  fd_set  *p_desc_err,
           struct  timeval *p_timeout)
{
  int      rtn_code;
  RTOS_ERR local_err;

  rtn_code = (int)NetSock_Sel((NET_SOCK_QTY) desc_nbr_max,
                              (NET_SOCK_DESC *) p_desc_rd,
                              (NET_SOCK_DESC *) p_desc_wr,
                              (NET_SOCK_DESC *) p_desc_err,
                              (NET_SOCK_TIMEOUT *) p_timeout,
                              &local_err);

  return (rtn_code);
}
#endif

/****************************************************************************************************//**
 *                                               inet_addr()
 *
 * @brief    Convert an IPv4 address in ASCII dotted-decimal notation to a network protocol IPv4 address
 *           in network-order.
 *
 * @param    p_addr  Pointer to an ASCII string that contains a dotted-decimal IPv4 address (see Note #2).
 *
 * @return   Network-order IPv4 address represented by ASCII string, if NO error(s).
 *           -1, otherwise.
 *
 * @note     (1) RFC #1983 states that "dotted decimal notation ... refers [to] IP addresses of the
 *               form A.B.C.D; where each letter represents, in decimal, one byte of a four byte IP
 *               address".
 *               @n
 *               In other words, the dotted-decimal notation separates four decimal octet values by
 *               the dot, or period, character ('.').  Each decimal value represents one octet of
 *               the IP address starting with the most significant octet in network-order.
 *               @n
 *                   IP Address Examples : 192.168.1.64 = 0xC0A80140
 *
 * @note     (2) The dotted-decimal ASCII string MUST :
 *               - (a) Include ONLY decimal values & the dot, or period, character ('.') ; ALL other
 *                     characters trapped as invalid, including any leading or trailing characters.
 *               - (b) Include UP TO four decimal values separated by UP TO three dot characters
 *                     & MUST be terminated with the NULL character.
 *               - (c) Ensure that each decimal value does NOT exceed the maximum octet value (i.e. 255).
 *               - (d) Ensure that each decimal value does NOT include leading zeros.
 *******************************************************************************************************/

#ifdef  NET_IPv4_MODULE_EN
in_addr_t inet_addr(char *p_addr)
{
  in_addr_t addr;
  RTOS_ERR  local_err;

  addr = (in_addr_t)NetASCII_Str_to_IPv4((CPU_CHAR *)p_addr,
                                         &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    addr = (in_addr_t)NET_BSD_ERR_DFLT;
  }

  addr = NET_UTIL_HOST_TO_NET_32(addr);

  return (addr);
}
#endif

/****************************************************************************************************//**
 *                                               inet_ntoa()
 *
 * @brief    Convert a network protocol IPv4 address into a dotted-decimal notation ASCII string.
 *
 * @param    addr    IPv4 address.
 *
 * @return   Pointer to ASCII string of converted IPv4 address, if NO error(s).
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) RFC #1983 states that "dotted decimal notation ... refers [to] IP addresses of the
 *               form A.B.C.D; where each letter represents, in decimal, one byte of a four byte IP
 *               address".
 *               @n
 *               In other words, the dotted-decimal notation separates four decimal octet values by
 *               the dot, or period, character ('.').  Each decimal value represents one octet of
 *               the IP address starting with the most significant octet in network-order.
 *               @n
 *                   IP Address Examples : 192.168.1.64 = 0xC0A80140
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'inet_ntoa() : DESCRIPTION' states that
 *               "inet_ntoa() ... need not be reentrant ... [and] is not required to be thread-safe".
 *               @n
 *               Since the character string is returned in a single, global character string array,
 *               this conversion function is NOT re-entrant.
 *******************************************************************************************************/

#ifdef  NET_IPv4_MODULE_EN
char *inet_ntoa(struct  in_addr addr)
{
  in_addr_t addr_ip;
  RTOS_ERR  local_err;

  addr_ip = addr.s_addr;
  addr_ip = NET_UTIL_NET_TO_HOST_32(addr_ip);

  NetASCII_IPv4_to_Str((NET_IPv4_ADDR) addr_ip,
                       (CPU_CHAR *)&NetBSD_IP_to_Str_Array[0],
                       (CPU_BOOLEAN) DEF_NO,
                       &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  return ((char *)&NetBSD_IP_to_Str_Array[0]);
}
#endif

/****************************************************************************************************//**
 *                                               inet_aton()
 *
 * @brief    Convert an IPv4 address in ASCII dotted-decimal notation to a network protocol IPv4 address
 *           in network-order.
 *
 * @param    p_addr_in   Pointer to an ASCII string that contains a dotted-decimal IPv4 address.
 *
 * @param    p_addr      _in   Pointer to an ASCII string that contains a dotted-decimal IPv4 address.
 *
 * @return   1  if the supplied address is valid,
 *           0, otherwise.
 *
 * @note     (1) RFC #1983 states that "dotted decimal notation ... refers [to] IP addresses of the
 *               form A.B.C.D; where each letter represents, in decimal, one byte of a four byte IP
 *               address".
 *               @n
 *               In other words, the dotted-decimal notation separates four decimal octet values by
 *               the dot, or period, character ('.').  Each decimal value represents one octet of
 *               the IP address starting with the most significant octet in network-order.
 *               @n
 *                   IP Address Examples : 192.168.1.64 = 0xC0A80140
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition - inet_addr, inet_ntoa - IPv4 address manipulation:
 *               - (a) Values specified using IPv4 dotted decimal notation take one of the following forms:
 *                   - (1) a.b.c.d - When four parts are specified, each shall be interpreted
 *                                   as a byte of data and assigned, from left to right,
 *                                   to the four bytes of an Internet address.
 *                   - (2) a.b.c   - When a three-part address is specified, the last part shall
 *                                   be interpreted as a 16-bit quantity and placed in the
 *                                   rightmost two bytes of the network address. This makes
 *                                   the three-part address format convenient for specifying
 *                                   Class B network addresses as "128.net.host".
 *                   - (3) a.b     - When a two-part address is supplied, the last part shall be
 *                                   interpreted as a 24-bit quantity and placed in the
 *                                   rightmost three bytes of the network address. This makes
 *                                   the two-part address format convenient for specifying
 *                                   Class A network addresses as "net.host".
 *                   - (4) a       - When only one part is given, the value shall be stored
 *                                   directly in the network address without any byte rearrangement.
 *
 * @note     (3) The dotted-decimal ASCII string MUST :
 *               - (a) Include ONLY decimal values & the dot, or period, character ('.') ; ALL other
 *                     characters trapped as invalid, including any leading or trailing characters.
 *               - (b) Include UP TO four decimal values separated by UP TO three dot characters
 *                     & MUST be terminated with the NULL character.
 *               - (c) Ensure that each decimal value does NOT exceed the maximum value for its form:
 *                     @verbatim
 *                     - (1) a.b.c.d - 255.255.255.255
 *                     - (2) a.b.c   - 255.255.65535
 *                     - (3) a.b     - 255.16777215
 *                     - (4) a       - 4294967295
 *                     @endverbatim
 *               - (d) Ensure that each decimal value does NOT include leading zeros.
 *******************************************************************************************************/
#ifdef  NET_IPv4_MODULE_EN
int inet_aton(char           *p_addr_in,
              struct in_addr *p_addr)
{
  in_addr_t  addr;
  CPU_INT08U pdot_nbr;
  RTOS_ERR   local_err;

  addr = (in_addr_t)NetASCII_Str_to_IPv4_Handler(p_addr_in,
                                                 &pdot_nbr,
                                                 &local_err);

  if ((RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE)
      || (pdot_nbr > NET_ASCII_NBR_MAX_DOT_ADDR_IP)) {
    addr = (in_addr_t)NET_BSD_ERR_NONE;
    p_addr->s_addr = addr;

    return (DEF_FAIL);
  }

  addr = NET_UTIL_HOST_TO_NET_32(addr);
  p_addr->s_addr = addr;

  return (DEF_OK);
}
#endif

/****************************************************************************************************//**
 *                                               setsockopt()
 *
 * @brief    Set socket option.
 *
 * @param    sock_id     Socket descriptor/handle identifier of socket to set the option.
 *
 * @param    protocol    Protocol level at which the option resides.
 *
 * @param    opt_name    Name of the single socket option to set.
 *
 * @param    p_opt_val   Pointer to the socket option value to set.
 *
 * @param    opt_len     Option length.
 *
 * @return    0, if NO error(s).
 *           -1, otherwise.
 *******************************************************************************************************/
int setsockopt(int       sock_id,
               int       protocol,
               int       opt_name,
               void      *p_opt_val,
               socklen_t opt_len)
{
  int      rtn_code;
  RTOS_ERR local_err;

  rtn_code = (int)NetSock_OptSet((NET_SOCK_ID) sock_id,
                                 (NET_SOCK_PROTOCOL) protocol,
                                 (NET_SOCK_OPT_NAME) opt_name,
                                 (void *) p_opt_val,
                                 (NET_SOCK_OPT_LEN) opt_len,
                                 &local_err);

  return (rtn_code);
}

/****************************************************************************************************//**
 *                                               getsockopt()
 *
 * @brief    Get socket option.
 *
 * @param    sock_id     Socket descriptor/handle identifier of socket to get the option.
 *
 * @param    protocol    Protocol level at which the option resides.
 *
 * @param    opt_name    Name of the single socket option to get.
 *
 * @param    p_opt_val   Pointer to the socket option value to get.
 *
 * @param    opt_len     Option length.
 *
 * @return    0, if NO error(s).
 *           -1, otherwise.
 *******************************************************************************************************/
int getsockopt(int       sock_id,
               int       protocol,
               int       opt_name,
               void      *p_opt_val,
               socklen_t *p_opt_len)
{
  int      rtn_code;
  RTOS_ERR local_err;

  rtn_code = (int)NetSock_OptGet((NET_SOCK_ID) sock_id,
                                 (NET_SOCK_PROTOCOL) protocol,
                                 (NET_SOCK_OPT_NAME) opt_name,
                                 (void *) p_opt_val,
                                 (NET_SOCK_OPT_LEN *) p_opt_len,
                                 &local_err);

  return (rtn_code);
}

/****************************************************************************************************//**
 *                                               getaddrinfo()
 *
 * @brief    Converts human-readable text strings representing hostnames or IP addresses into a
 *           dynamically allocated linked list of struct addrinfo structures.
 *
 * @param    p_node_name     A pointer to a string that contains a host (node) name or a numeric host
 *                           address string. For the Internet protocol, the numeric host address string
 *                           is a dotted-decimal IPv4 address or an IPv6 hex address.
 *
 * @param    p_service_name  A pointer to a string that contains either a service name or port number
 *                           represented as a string.
 *
 * @param    p_hints         A pointer to an addrinfo structure that provides hints about the type of
 *                           socket the caller supports.
 *
 * @param    res             A pointer to a linked list of one or more addrinfo structures that contains
 *                           response information about the host.
 *
 * @return   0, if no error,
 *           Most nonzero error codes returned map to the set of errors outlined by Internet Engineering
 *           Task Force (IETF) recommendations:
 *               - EAI_ADDRFAMILY
 *               - EAI_AGAIN
 *               - EAI_BADFLAGS
 *               - EAI_FAIL
 *               - EAI_FAMILY
 *               - EAI_MEMORY
 *               - EAI_NONAME
 *               - EAI_OVERFLOW
 *               - EAI_SERVICE
 *               - EAI_SOCKTYPE
 *               - EAI_SYSTEM
 *******************************************************************************************************/
int getaddrinfo(const char             *p_node_name,
                const char             *p_service_name,
                const struct  addrinfo *p_hints,
                struct  addrinfo       **pp_res)
{
#ifdef  NET_DNS_CLIENT_MODULE_EN
  NET_HOST_IP_ADDR *p_hosts_addr = DEF_NULL;
#endif
  struct addrinfo       *p_addrinfo;
  struct addrinfo       *p_addrinfo_head = DEF_NULL;
  struct addrinfo       *p_addrinfo_tail = DEF_NULL;
  NET_IP_ADDR_FAMILY    hints_addr_family = NET_IP_ADDR_FAMILY_UNKNOWN;
  NET_BSD_SOCK_PROTOCOL hints_protocol = NET_BSD_SOCK_PROTOCOL_UNKNOWN;
  NET_PORT_NBR          service_port = NET_PORT_NBR_NONE;
  CPU_BOOLEAN           service_port_num = DEF_NO;
  NET_BSD_SOCK_PROTOCOL service_protocol = NET_BSD_SOCK_PROTOCOL_UNKNOWN;
  NET_SOCK_PROTOCOL     protocol;
  CPU_CHAR              *p_canonname = DEF_NULL;
  CPU_BOOLEAN           valid_cfdg = DEF_NO;
  CPU_BOOLEAN           host_num = DEF_NO;
  CPU_BOOLEAN           wildcard = DEF_NO;
  CPU_BOOLEAN           result;
  int                   rtn_val = 0;
  CPU_BOOLEAN           set_ipv4;
  CPU_BOOLEAN           set_ipv6;
#ifndef  NET_DNS_CLIENT_MODULE_EN
  NET_IP_ADDR_FAMILY ip_family;
  RTOS_ERR           err_net;
#endif

  PP_UNUSED_PARAM(pp_res);

#ifdef  NET_IPv4_MODULE_EN
  set_ipv4 = DEF_YES;
#else
  set_ipv4 = DEF_NO;
#endif

#ifdef  NET_IPv6_MODULE_EN
  set_ipv6 = DEF_YES;
#else
  set_ipv6 = DEF_NO;
#endif

  if ((p_node_name == DEF_NULL)
      && (p_service_name == DEF_NULL)) {
    rtn_val = EAI_NONAME;
    goto exit;
  }

  if (p_hints != DEF_NULL) {
    set_ipv4 = DEF_NO;
    set_ipv6 = DEF_NO;

    switch (p_hints->ai_socktype) {
      case 0:                                                       // When ai_socktype is zero the caller will accept
        hints_protocol = NET_BSD_SOCK_PROTOCOL_UNKNOWN;             // any socket type.
        break;

#ifdef  NET_TCP_MODULE_EN
      case SOCK_STREAM:
        hints_protocol = NET_BSD_SOCK_PROTOCOL_TCP;
        break;
#endif

      case SOCK_DGRAM:
        hints_protocol = NET_BSD_SOCK_PROTOCOL_UDP;
        break;

      default:
        rtn_val = EAI_SOCKTYPE;
        goto exit;
    }

    switch (p_hints->ai_family) {
      case AF_UNSPEC:                                               // When ai_family is set to AF_UNSPEC, it means
        hints_addr_family = NET_IP_ADDR_FAMILY_UNKNOWN;             // the caller will accept any address family
        break;                                                      // supported by the operating system.

#ifdef  NET_IPv4_MODULE_EN
      case AF_INET:
        hints_addr_family = NET_IP_ADDR_FAMILY_IPv4;
        break;
#endif

#ifdef  NET_IPv6_MODULE_EN
      case AF_INET6:
        hints_addr_family = NET_IP_ADDR_FAMILY_IPv6;
        break;
#endif

      default:
        rtn_val = EAI_FAMILY;
        goto exit;
    }

    switch (hints_addr_family) {
#ifdef  NET_IPv4_MODULE_EN
      case NET_IP_ADDR_FAMILY_IPv4:
        set_ipv6 = DEF_NO;
        break;
#endif

#ifdef  NET_IPv6_MODULE_EN
      case NET_IP_ADDR_FAMILY_IPv6:
        set_ipv4 = DEF_NO;
        break;
#endif

      case NET_IP_ADDR_FAMILY_UNKNOWN:
        break;

      default:
        rtn_val = EAI_SYSTEM;
        goto exit;
    }

    wildcard = DEF_BIT_IS_SET(p_hints->ai_flags, AI_PASSIVE);
    //                                                             If the AI_PASSIVE flag is specified in
    //                                                             hints.ai_flags, and node is NULL, then the returned
    //                                                             socket addresses will be suitable for bind(2)ing a
    //                                                             socket that will accept(2) connections.

    host_num = DEF_BIT_IS_SET(p_hints->ai_flags, AI_NUMERICHOST);
    //                                                             If hints.ai_flags contains the AI_NUMERICHOST flag,
    //                                                             then node must be a numerical network address. The
    //                                                             AI_NUMERICHOST flag suppresses any potentially
    //                                                             lengthy network host address lookups.

    service_port_num = DEF_BIT_IS_SET(p_hints->ai_flags, AI_NUMERICSERV);
    //                                                             If AI_NUMERICSERV is specified in hints.ai_flags
    //                                                             and service is not NULL, then service must point to
    //                                                             a string containing a numeric port number.  This
    //                                                             flag is used to inhibit the invocation of a name
    //                                                             resolution service in cases where it is known not to
    //                                                             be required.

    valid_cfdg = DEF_BIT_IS_SET(p_hints->ai_flags, AI_ADDRCONFIG);
    //                                                             If the AI_ADDRCONFIG bit is set, IPv4 addresses
    //                                                             shall be returned only if an IPv4 address is
    //                                                             configured on the local system, and IPv6 addresses
    //                                                             shall be returned only if an IPv6 address is
    //                                                             configured on the local system.
    if (valid_cfdg == DEF_YES) {
      NetBSD_AddrCfgValidate(&set_ipv4, &set_ipv6);
    }
  }     // if (hints != DEF_NULL)

  if (p_service_name != DEF_NULL) {                                    // service sets the port in each returned address
    //                                                             structure.  If this argument is a service name , it
    //                                                             is translated to the corresponding port number.
    CPU_SIZE_T len;
    CPU_INT32U i;
    CPU_INT32U obj_ctn = sizeof(NetBSD_ServicesProtocolTbl) / sizeof(NET_BSD_SERVICE_PROTOCOL);

    if (service_port_num == DEF_NO) {
      len = Str_Len_N(p_service_name, 255);
      service_port = NetDict_KeyGet(NetBSD_ServicesStrTbl, sizeof(NetBSD_ServicesStrTbl), p_service_name, DEF_NO, len);
      if (service_port >= NET_PORT_NBR_MAX) {
        service_port_num = DEF_YES;                             // service argument can also be specified as a decimal
                                                                // number, which is simply converted to binary.
      }
    }

    if (service_port_num == DEF_YES) {
      CPU_INT32U val;

      val = Str_ParseNbr_Int32U(p_service_name, DEF_NULL, DEF_NBR_BASE_DEC);
      if ((val < NET_PORT_NBR_MIN)
          || (val > NET_PORT_NBR_MAX)) {
        rtn_val = EAI_NONAME;
        goto exit;
      }

      service_port = (NET_PORT_NBR)val;
    }

    for (i = 0; i < obj_ctn; i++) {
      const NET_BSD_SERVICE_PROTOCOL *p_obj = &NetBSD_ServicesProtocolTbl[i];

      if (service_port == p_obj->Port) {
        service_protocol = p_obj->Protocol;
        break;
      }
    }

    if (service_protocol == NET_BSD_SOCK_PROTOCOL_UNKNOWN) {
      rtn_val = EAI_SERVICE;
      goto exit;
    }
  } else {                                                      // If service is NULL, then the port number of the
                                                                // returned socket addresses will be left uninitialized
  }

  switch (hints_protocol) {
    case NET_BSD_SOCK_PROTOCOL_UNKNOWN:
      switch (service_protocol) {
        case NET_BSD_SOCK_PROTOCOL_UNKNOWN:
          protocol = NET_SOCK_PROTOCOL_NONE;
          break;

        case NET_BSD_SOCK_PROTOCOL_UDP:
        case NET_BSD_SOCK_PROTOCOL_UDP_TCP:
          protocol = NET_SOCK_PROTOCOL_UDP;
          break;

        case NET_BSD_SOCK_PROTOCOL_TCP:
        case NET_BSD_SOCK_PROTOCOL_TCP_UDP:
          protocol = NET_SOCK_PROTOCOL_TCP;
          break;

        default:
          rtn_val = EAI_SERVICE;
          goto exit;
      }
      break;

    case NET_BSD_SOCK_PROTOCOL_UDP:
    case NET_BSD_SOCK_PROTOCOL_UDP_TCP:
      protocol = NET_SOCK_PROTOCOL_UDP;
      break;

    case NET_BSD_SOCK_PROTOCOL_TCP:
    case NET_BSD_SOCK_PROTOCOL_TCP_UDP:
      protocol = NET_SOCK_PROTOCOL_TCP;
      break;

    default:
      rtn_val = EAI_SERVICE;
      goto exit;
  }

  if (p_node_name == DEF_NULL) {
    if (set_ipv4 == DEF_YES) {
#ifdef  NET_IPv4_MODULE_EN
      if (wildcard == DEF_YES) {
        NET_IPv4_ADDR addr = INADDR_ANY;
        //                                                         If the AI_PASSIVE flag is specified in
        //                                                         hints.ai_flags, and node is NULL, then the returned
        //                                                         socket addresses will be suitable for binding a
        //                                                         socket that will accept(2) connections.  The
        //                                                         returned socket address will contain the "wildcard
        //                                                         address" (INADDR_ANY for IPv4 addresses,
        //                                                         IN6ADDR_ANY_INIT for IPv6 address).

        p_addrinfo = NetBSD_AddrInfoGet(&p_addrinfo_head, &p_addrinfo_tail);
        if (p_addrinfo == DEF_NULL) {
          rtn_val = EAI_MEMORY;
          goto exit_error;
        }

        result = NetBSD_AddrInfoSet(p_addrinfo,
                                    NET_SOCK_ADDR_FAMILY_IP_V4,
                                    service_port,
                                    (CPU_INT08U *)&addr,
                                    sizeof(addr),
                                    protocol,
                                    p_canonname);
        if (result != DEF_OK) {
          rtn_val = EAI_SYSTEM;
          goto exit_error;
        }
      } else {
        NET_IPv4_ADDR addr = INADDR_LOOPBACK;
        //                                                         If the AI_PASSIVE flag is not set in hints.ai_flags,
        //                                                         then the returned socket addresses will be suitable
        //                                                         for use with connect(2), sendto(2), or sendmsg(2).
        //                                                         If node is NULL, then the network address will be
        //                                                         set to the loopback interface address (
        //                                                         INADDR_LOOPBACK for IPv4 addresses,
        //                                                         IN6ADDR_LOOPBACK_INIT for IPv6 address); this is
        //                                                         used by applications that intend to communicate with
        //                                                         peers running on the same host.

        p_addrinfo = NetBSD_AddrInfoGet(&p_addrinfo_head, &p_addrinfo_tail);
        if (p_addrinfo == DEF_NULL) {
          rtn_val = EAI_MEMORY;
          goto exit_error;
        }

        result = NetBSD_AddrInfoSet(p_addrinfo,
                                    NET_SOCK_ADDR_FAMILY_IP_V4,
                                    service_port,
                                    (CPU_INT08U *)&addr,
                                    sizeof(INADDR_LOOPBACK),
                                    protocol,
                                    p_canonname);
        if (result != DEF_OK) {
          rtn_val = EAI_SYSTEM;
          goto exit_error;
        }
      }
#endif
    }     // if (ipv4_set == DEF_YES)

    if (set_ipv6 == DEF_YES) {
#ifdef  NET_IPv6_MODULE_EN
      if (wildcard == DEF_YES) {
        //                                                         If the AI_PASSIVE flag is specified in
        //                                                         hints.ai_flags, and node is NULL, then the returned
        //                                                         socket addresses will be suitable for binding a
        //                                                         socket that will accept(2) connections.  The
        //                                                         returned socket address will contain the "wildcard
        //                                                         address" (INADDR_ANY for IPv4 addresses,
        //                                                         IN6ADDR_ANY_INIT for IPv6 address).
        p_addrinfo = NetBSD_AddrInfoGet(&p_addrinfo_head, &p_addrinfo_tail);
        if (p_addrinfo == DEF_NULL) {
          rtn_val = EAI_MEMORY;
          goto exit_error;
        }

        result = NetBSD_AddrInfoSet(p_addrinfo,
                                    NET_SOCK_ADDR_FAMILY_IP_V6,
                                    service_port,
                                    (CPU_INT08U *)&in6addr_any,
                                    sizeof(in6addr_any),
                                    protocol,
                                    p_canonname);
        if (result != DEF_OK) {
          rtn_val = EAI_SYSTEM;
          goto exit_error;
        }
      } else {
        p_addrinfo = NetBSD_AddrInfoGet(&p_addrinfo_head, &p_addrinfo_tail);
        if (p_addrinfo == DEF_NULL) {
          rtn_val = EAI_MEMORY;
          goto exit_error;
        }

        result = NetBSD_AddrInfoSet(p_addrinfo,
                                    NET_SOCK_ADDR_FAMILY_IP_V6,
                                    service_port,
                                    (CPU_INT08U *)&in6addr_loopback,
                                    sizeof(in6addr_loopback),
                                    protocol,
                                    p_canonname);
        if (result != DEF_OK) {
          rtn_val = EAI_SYSTEM;
          goto exit_error;
        }
      }
#endif
    }     // if (ipv6_set == DEF_YES)
  } else {   // if (node == DEF_NULL)
#ifdef  NET_DNS_CLIENT_MODULE_EN
    NET_HOST_IP_ADDR *p_hosts_addr_cur;
    CPU_INT08U       host_addr_nbr;
    DNSc_FLAGS       dns_flags = DNSc_FLAG_NONE;
    DNSc_STATUS      status;
    RTOS_ERR         err_dns;

    if ((set_ipv4 == DEF_YES)
        && (set_ipv6 == DEF_NO)) {
      DEF_BIT_SET(dns_flags, DNSc_FLAG_IPv4_ONLY);
    } else if ((set_ipv4 == DEF_YES)
               && (set_ipv6 == DEF_NO)) {
      DEF_BIT_SET(dns_flags, DNSc_FLAG_IPv6_ONLY);
    }

    status = DNSc_GetHostAddrs((CPU_CHAR *)p_node_name,
                               &p_hosts_addr,
                               &host_addr_nbr,
                               dns_flags,
                               DEF_NULL,
                               &err_dns);
    switch (status) {
      case DNSc_STATUS_RESOLVED:
        p_hosts_addr_cur = p_hosts_addr;
        while (p_hosts_addr_cur != DEF_NULL) {
          NET_SOCK_ADDR_FAMILY sock_family = NET_SOCK_ADDR_FAMILY_IP_V4;

          p_addrinfo = NetBSD_AddrInfoGet(&p_addrinfo_head, &p_addrinfo_tail);
          if (p_addrinfo == DEF_NULL) {
            rtn_val = EAI_MEMORY;
            goto exit_error;
          }

          switch (p_hosts_addr_cur->AddrObj.AddrLen) {
            case NET_IPv4_ADDR_LEN:
              sock_family = NET_SOCK_ADDR_FAMILY_IP_V4;
              break;

            case NET_IPv6_ADDR_LEN:
              sock_family = NET_SOCK_ADDR_FAMILY_IP_V6;
              break;
          }

          result = NetBSD_AddrInfoSet(p_addrinfo,
                                      sock_family,
                                      service_port,
                                      (CPU_INT08U *)&p_hosts_addr_cur->AddrObj.Addr,
                                      p_hosts_addr_cur->AddrObj.AddrLen,
                                      protocol,
                                      p_canonname);
          if (result != DEF_OK) {
            rtn_val = EAI_SYSTEM;
            goto exit_error;
          }

          p_hosts_addr_cur = p_hosts_addr_cur->NextPtr;
        }

        *pp_res = p_addrinfo_head;
        DNSc_FreeHostAddrs(p_hosts_addr);
        break;

      case DNSc_STATUS_PENDING:
        rtn_val = EAI_AGAIN;
        goto exit;

      case DNSc_STATUS_FAILED:
        rtn_val = EAI_AGAIN;
        goto exit;

      case DNSc_STATUS_UNKNOWN:
      case DNSc_STATUS_NONE:
      default:
        rtn_val = EAI_SYSTEM;
        goto exit;
    }

    (void)&host_num;                                            // Prevent variable unused. Numeric conversion is done
                                                                // by DNSc_GetHostAddrs as first step

#else
    if (host_num == DEF_NO) {
      rtn_val = EAI_SYSTEM;
      goto exit_error;
    }

    p_addrinfo = NetBSD_AddrInfoGet(&p_addrinfo_head, &p_addrinfo_tail);
    if (p_addrinfo == DEF_NULL) {
      rtn_val = EAI_MEMORY;
      goto exit_error;
    }

    ip_family = NetASCII_Str_to_IP((CPU_CHAR *)p_node_name,
                                   p_addrinfo->ai_addr,
                                   sizeof(struct  sockaddr),
                                   &err_net);
    if (RTOS_ERR_CODE_GET(err_net) == RTOS_ERR_NONE) {
      NET_SOCK_ADDR_FAMILY sock_family = NET_SOCK_ADDR_FAMILY_IP_V4;
      NET_IP_ADDR_LEN      addr_len = NET_IPv4_ADDR_LEN;

      if (RTOS_ERR_CODE_GET(err_net) != RTOS_ERR_NONE) {
        rtn_val = EAI_SYSTEM;
        goto exit_error;
      }

      switch (ip_family) {
        case NET_IP_ADDR_FAMILY_IPv4:
          sock_family = NET_SOCK_ADDR_FAMILY_IP_V4;
          addr_len = NET_IPv4_ADDR_LEN;
          break;

        case NET_IP_ADDR_FAMILY_IPv6:
          sock_family = NET_SOCK_ADDR_FAMILY_IP_V6;
          addr_len = NET_IPv6_ADDR_LEN;
          break;

        default:
          break;
      }

      result = NetBSD_AddrInfoSet(p_addrinfo,
                                  sock_family,
                                  service_port,
                                  (CPU_INT08U *)p_addrinfo->ai_addr,
                                  addr_len,
                                  protocol,
                                  p_canonname);
      if (result != DEF_OK) {
        rtn_val = EAI_SYSTEM;
        goto exit_error;
      }
    }
#endif
  }

  goto exit;

exit_error:
#ifdef  NET_DNS_CLIENT_MODULE_EN
  DNSc_FreeHostAddrs(p_hosts_addr);
#endif
  NetBSD_AddrInfoFree(p_addrinfo_head);

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                               freeaddrinfo()
 *
 * @brief    Frees addrinfo structures information that getaddrinfo has allocated.
 *
 * @param    res     A pointer to the addrinfo structure or linked list of addrinfo structures to be freed.
 *******************************************************************************************************/
void freeaddrinfo(struct addrinfo *res)
{
  NetBSD_AddrInfoFree(res);
}

/****************************************************************************************************//**
 *                                               inet_ntop()
 *
 * @brief    Converts an IPv4 or IPv6 Internet network address into a string in Internet standard format.
 *
 * @param    af      Address family:
 *                       - AF_INET     Ipv4 Address Family
 *                       - AF_INET6    Ipv6 Address Family
 *
 * @param    src     A pointer to the IP address in network byte to convert to a string.
 *
 * @param    dst     A pointer to a buffer in which to store the NULL-terminated string representation of the IP address.
 *
 * @param    size    Length, in characters, of the buffer pointed to by dst.
 *
 * @return   Pointer to a buffer containing the string representation of IP address in standard format , If no error occurs.
 *           DEF_NULL, otherwise.
 *******************************************************************************************************/
const char *inet_ntop(int        af,
                      const void *src,
                      char       *dst,
                      socklen_t  size)
{
  char     *p_rtn = DEF_NULL;
  RTOS_ERR err;

  switch (af) {
#ifdef  NET_IPv4_MODULE_EN
    case AF_INET:
      if (size < INET_ADDRSTRLEN) {
        goto exit;
      }
      NetASCII_IPv4_to_Str(*((NET_IPv4_ADDR *)src), dst, DEF_NO, &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case AF_INET6:
      if (size < INET6_ADDRSTRLEN) {
        goto exit;
      }
      NetASCII_IPv6_to_Str((NET_IPv6_ADDR *)src, dst, DEF_NO, DEF_NO, &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

    default:
      goto exit;
  }

  p_rtn = dst;

exit:
  return ((const char *)p_rtn);
}

/****************************************************************************************************//**
 *                                               inet_pton()
 *
 * @brief    Converts an IPv4 or IPv6 Internet network address in its standard text presentation form
 *           into its numeric binary form.
 *
 * @param    af      Address family:
 *                       - AF_INET     Ipv4 Address Family
 *                       - AF_INET6    Ipv6 Address Family
 *
 * @param    src     A pointer to the NULL-terminated string that contains the text representation of
 *                   the IP address to convert to numeric binary form.
 *
 * @param    dst     A pointer to a buffer that will receive the numeric binary representation of the
 *                   IP address.
 *
 * @return   1, if no error.
 *           0, if src does not contain a character string representing a valid network address in
 *           the specified address family.
 *           -1, if af does not contain a valid address family.
 *******************************************************************************************************/
int inet_pton(int        af,
              const char *src,
              void       *dst)
{
  int      rtn = 0;
  RTOS_ERR err;

#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR *p_addr_ipv4 = (NET_IPv4_ADDR *)dst;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_IPv6_ADDR *p_addr_ipv6 = (NET_IPv6_ADDR *)dst;
#endif

  switch (af) {
#ifdef  NET_IPv4_MODULE_EN
    case AF_INET:
      *p_addr_ipv4 = NetASCII_Str_to_IPv4((CPU_CHAR *)src, &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case AF_INET6:

      *p_addr_ipv6 = NetASCII_Str_to_IPv6((CPU_CHAR *)src, &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

    default:
      rtn = -1;
      goto exit;
  }

  rtn = 1;

exit:
  return (rtn);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetBSD_AddrInfoGet()
 *
 * @brief    Allocate and link to a list an addrinfo structure
 *
 * @param    pp_head     Pointer to the head pointer list
 *
 * @param    pp_tail     Pointer to the tail pointer list
 *
 * @return   Pointer to the allocated addrinfo structure.
 *******************************************************************************************************/
static struct  addrinfo *NetBSD_AddrInfoGet(struct  addrinfo **pp_head,
                                            struct  addrinfo **pp_tail)
{
  struct  addrinfo *p_addrinfo;
  struct  sockaddr *p_sockaddr;
  struct  addrinfo *p_addrinfo_rtn = DEF_NULL;
  RTOS_ERR         err;

  p_addrinfo = (struct  addrinfo *)Mem_DynPoolBlkGet(&NetBSD_AddrInfoPool, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_sockaddr = (struct  sockaddr *)Mem_DynPoolBlkGet(&NetBSD_SockAddrPool, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    NetBSD_AddrInfoFree(p_addrinfo);
    goto exit;
  }

  p_addrinfo->ai_addr = p_sockaddr;
  p_addrinfo_rtn = p_addrinfo;

  if (*pp_head == DEF_NULL) {
    *pp_head = p_addrinfo_rtn;
  }

  if (*pp_tail != DEF_NULL) {
    (*pp_tail)->ai_next = p_addrinfo_rtn;
  }

  *pp_tail = p_addrinfo_rtn;

exit:
  return (p_addrinfo_rtn);
}

/****************************************************************************************************//**
 *                                           NetBSD_AddrInfoFree()
 *
 * @brief    Free an addrinfo structure
 *
 * @param    p_addrinfo  Pointer to the addrinfo structure to free.
 *******************************************************************************************************/
static void NetBSD_AddrInfoFree(struct  addrinfo *p_addrinfo)
{
  struct  addrinfo *p_blk = p_addrinfo;
  RTOS_ERR         err;

  while (p_blk != DEF_NULL) {
    if (p_blk->ai_addr != DEF_NULL) {
      Mem_DynPoolBlkFree(&NetBSD_SockAddrPool, p_blk->ai_addr, &err);
      p_blk->ai_addr = DEF_NULL;
    }

    Mem_DynPoolBlkFree(&NetBSD_AddrInfoPool, p_blk, &err);

    p_blk = p_blk->ai_next;

    (void)&err;
  }
}

/****************************************************************************************************//**
 *                                           NetBSD_AddrInfoSet()
 *
 * @brief    Set addrinfo's field.
 *
 * @param    p_addrinfo      Pointer to the addrinfo structure to be filled.
 *
 * @param    family          Socket family
 *
 * @param    port_nbr        Port number
 *
 * @param    p_addr          info      Pointer to the addrinfo structure to be filled.
 *
 * @param    addr_len        IP address length
 *
 * @param    protocol        Socket protocol
 *
 * @param    p_canonname     $$$$ Add description for 'p_canonname'
 *
 * @return   $$$$ Add return value description.
 *
 * Caller(s)   : getaddrinfo().
 *
 * Note(s)     : none.
 *
 *******************************************************************************************************/
static CPU_BOOLEAN NetBSD_AddrInfoSet(struct  addrinfo     *p_addrinfo,
                                      NET_SOCK_ADDR_FAMILY family,
                                      NET_PORT_NBR         port_nbr,
                                      CPU_INT08U           *p_addr,
                                      NET_IP_ADDR_LEN      addr_len,
                                      NET_SOCK_PROTOCOL    protocol,
                                      CPU_CHAR             *p_canonname)
{
  CPU_BOOLEAN result = DEF_FAIL;
  RTOS_ERR    err;

  NetApp_SetSockAddr((NET_SOCK_ADDR *)p_addrinfo->ai_addr,
                     family,
                     port_nbr,
                     p_addr,
                     addr_len,
                     &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_addrinfo->ai_family = family;
  p_addrinfo->ai_addrlen = addr_len;
  p_addrinfo->ai_protocol = (int)protocol;
  p_addrinfo->ai_canonname = p_canonname;
  p_addrinfo->ai_protocol = protocol;
  result = DEF_OK;

exit:
  return (result);
}

/****************************************************************************************************//**
 *                                           NetBSD_AddrCfgValidate()
 *
 * @brief    Validate that IPs are configured on interface(s).
 *
 * @param    p_ipv4_cfgd     Pointer to a variable that will receive the IPv4 address configuration status
 *
 * @param    p_ipv6_cfgd     Pointer to a variable that will receive the IPv6 address configuration status
 *******************************************************************************************************/
static void NetBSD_AddrCfgValidate(CPU_BOOLEAN *p_ipv4_cfgd,
                                   CPU_BOOLEAN *p_ipv6_cfgd)
{
  NET_IF_NBR if_nbr_ix;
  NET_IF_NBR if_nbr_cfgd;
  NET_IF_NBR if_nbr_base;
  RTOS_ERR   err;

  if_nbr_base = NetIF_GetNbrBaseCfgd();
  if_nbr_cfgd = NetIF_GetExtAvailCtr(&err);
  if_nbr_cfgd -= if_nbr_base;

#ifdef  NET_IPv4_MODULE_EN
  if (*p_ipv4_cfgd == DEF_YES) {
    if_nbr_ix = if_nbr_base;

    for (if_nbr_ix = 0; if_nbr_ix <= if_nbr_cfgd; if_nbr_ix++) {
      *p_ipv4_cfgd = NetIPv4_IsAddrsCfgdOnIF(if_nbr_ix, &err);
      if (*p_ipv4_cfgd == DEF_YES) {
        break;
      }
    }
  }
#else
  *p_ipv4_cfgd = DEF_NO;
#endif

#ifdef  NET_IPv6_MODULE_EN
  if (*p_ipv4_cfgd == DEF_YES) {
    if_nbr_ix = if_nbr_base;

    for (if_nbr_ix = 0; if_nbr_ix <= if_nbr_cfgd; if_nbr_ix++) {
      *p_ipv6_cfgd = NetIPv6_IsAddrsCfgdOnIF(if_nbr_ix, &err);
      if (*p_ipv6_cfgd == DEF_YES) {
        break;
      }
    }
  }
#else
  *p_ipv6_cfgd = DEF_NO;
#endif
}

#endif // NET_SOCK_BSD_DIS

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
