/***************************************************************************//**
 * @file
 * @brief Network Udp Layer - (User Datagram Protocol)
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

#include  <net/include/net_util.h>
#include  <net/include/net_cfg_net.h>

#include  "net_udp_priv.h"
#include  "net_priv.h"
#include  "net_stat_priv.h"
#include  "net_buf_priv.h"
#include  "net_ip_priv.h"
#include  "net_sock_priv.h"
#include  "net_if_priv.h"
#include  "net_util_priv.h"

#ifdef  NET_IPv4_MODULE_EN
#include  "net_ipv4_priv.h"
#include  "net_icmpv4_priv.h"
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  "net_ipv6_priv.h"
#include  "net_icmpv6_priv.h"
#endif

#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  enum  net_udp_err_type {
  NET_UDP_ERR_TYPE_NONE,
  NET_UDP_ERR_TYPE_DEST_UNREACHABLE,
  NET_UDP_ERR_TYPE_UNKNOW
} NET_UDP_ERR_TYPE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------------- RX FNCTS ---------------
static NET_UDP_ERR_TYPE NetUDP_RxPktValidate(NET_BUF     *p_buf,
                                             NET_BUF_HDR *p_buf_hdr,
                                             NET_UDP_HDR *p_udp_hdr,
                                             RTOS_ERR    *p_err);

static void NetUDP_RxPktDemuxDatagram(NET_BUF  *p_buf,
                                      RTOS_ERR *p_err);

static void NetUDP_RxPktFree(NET_BUF *p_buf);

//                                                                 --------------- TX FNCTS ---------------
#ifdef  NET_IPv4_MODULE_EN
static void NetUDP_TxIPv4(NET_BUF        *p_buf,
                          NET_IPv4_ADDR  src_addr,
                          NET_PORT_NBR   src_port,
                          NET_IPv4_ADDR  dest_addr,
                          NET_PORT_NBR   dest_port,
                          NET_IPv4_TOS   TOS,
                          NET_IPv4_TTL   TTL,
                          NET_UDP_FLAGS  flags_udp,
                          NET_IPv4_FLAGS flags_ip,
                          void           *p_opts_ip,
                          RTOS_ERR       *p_err);
#endif

#ifdef  NET_IPv6_MODULE_EN
static void NetUDP_TxIPv6(NET_BUF                *p_buf,
                          NET_IPv6_ADDR          *p_src_addr,
                          NET_PORT_NBR           src_port,
                          NET_IPv6_ADDR          *p_dest_addr,
                          NET_PORT_NBR           dest_port,
                          NET_IPv6_TRAFFIC_CLASS traffic_class,
                          NET_IPv6_FLOW_LABEL    flow_label,
                          NET_IPv6_HOP_LIM       hop_lim,
                          NET_UDP_FLAGS          flags_udp,
                          RTOS_ERR               *p_err);
#endif

static void NetUDP_TxPktPrepareHdr(NET_BUF       *p_buf,
                                   NET_BUF_HDR   *p_buf_hdr,
                                   void          *p_src_addr,
                                   NET_PORT_NBR  src_port,
                                   void          *p_dest_addr,
                                   NET_PORT_NBR  dest_port,
                                   NET_UDP_FLAGS flags_udp);

static void NetUDP_TxPktFree(NET_BUF *p_buf);

static void NetUDP_TxPktDiscard(NET_BUF *p_buf);

static void NetUDP_GetTxDataIx(NET_IF_NBR        if_nbr,
                               NET_PROTOCOL_TYPE protocol,
                               CPU_INT16U        data_len,
                               NET_UDP_FLAGS     flags,
                               CPU_INT16U        *p_ix);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetUDP_Init()
 *
 * @brief    Initialize User Datagram Protocol Layer.
 *******************************************************************************************************/
void NetUDP_Init(void)
{
}

/****************************************************************************************************//**
 *                                               NetUDP_Rx()
 *
 * @brief    - (1) Process received datagrams & forward to socket or application layer :
 *               - (a) Validate UDP packet
 *               - (b) Demultiplex datagram to socket/application connection
 *               - (c) Update receive statistics
 *           - (2) Although UDP data units are typically referred to as 'datagrams' (see RFC #768, Section
 *                 'Introduction'), the term 'UDP packet' (see RFC #1983, 'packet') is used for UDP Receive
 *                 until the packet is validated as a UDP datagram.
 *
 * @param    p_buf   Pointer to network buffer that received UDP packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (3) Network buffer already freed by higher layer; only increment error counter.
 *
 * @note     (4) RFC #792, Section 'Destination Unreachable Message : Description' states that
 *               "if, in the destination host, the IP module cannot deliver the datagram because
 *               the indicated ... process port is not active, the destination host may send a
 *               destination unreachable message to the source host".
 *******************************************************************************************************/
void NetUDP_Rx(NET_BUF  *p_buf,
               RTOS_ERR *p_err)
{
  NET_BUF_HDR      *p_buf_hdr;
  NET_UDP_HDR      *p_udp_hdr;
  NET_UDP_ERR_TYPE err_type;
  RTOS_ERR         local_err;

  p_buf_hdr = &p_buf->Hdr;

  p_udp_hdr = (NET_UDP_HDR *)&p_buf->DataPtr[p_buf_hdr->TransportHdrIx];

  err_type = NetUDP_RxPktValidate(p_buf, p_buf_hdr, p_udp_hdr, p_err);     // Validate rx'd pkt.
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_RX:
      if (err_type == NET_UDP_ERR_TYPE_DEST_UNREACHABLE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxDestCtr);
        if (DEF_BIT_IS_CLR(p_buf_hdr->Flags, NET_BUF_FLAG_IPv6_FRAME)) {
#ifdef  NET_ICMPv4_MODULE_EN
          RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
          NetICMPv4_TxMsgErr(p_buf,                                        // Tx ICMP port unreach (see Note #5).
                             NET_ICMPv4_MSG_TYPE_DEST_UNREACH,
                             NET_ICMPv4_MSG_CODE_DEST_PORT,
                             NET_ICMPv4_MSG_PTR_NONE,
                             &local_err);
#endif
        } else {
#ifdef  NET_ICMPv6_MODULE_EN
          RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
          NetICMPv6_TxMsgErr(p_buf,
                             NET_ICMPv6_MSG_TYPE_DEST_UNREACH,
                             NET_ICMPv6_MSG_CODE_DEST_PORT_UNREACHABLE,
                             NET_ICMPv6_MSG_PTR_NONE,
                             &local_err);
#endif
        }
      }
      PP_UNUSED_PARAM(local_err);
      goto exit_discard;

    default:
      goto exit_discard;
  }

  //                                                               ------------------ DEMUX DATAGRAM ------------------
  NetUDP_RxPktDemuxDatagram(p_buf, p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_RX:
      NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxDestCtr);
      if (DEF_BIT_IS_CLR(p_buf_hdr->Flags, NET_BUF_FLAG_IPv6_FRAME)) {
#ifdef  NET_ICMPv4_MODULE_EN
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        NetICMPv4_TxMsgErr(p_buf,                               // Tx ICMP port unreach (see Note #5).
                           NET_ICMPv4_MSG_TYPE_DEST_UNREACH,
                           NET_ICMPv4_MSG_CODE_DEST_PORT,
                           NET_ICMPv4_MSG_PTR_NONE,
                           &local_err);
#endif
      } else {
#ifdef  NET_ICMPv6_MODULE_EN
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        NetICMPv6_TxMsgErr(p_buf,
                           NET_ICMPv6_MSG_TYPE_DEST_UNREACH,
                           NET_ICMPv6_MSG_CODE_DEST_PORT_UNREACHABLE,
                           NET_ICMPv6_MSG_PTR_NONE,
                           &local_err);
#endif
      }
      PP_UNUSED_PARAM(local_err);
      goto exit_discard;

    default:
      goto exit_discard;
  }

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxPktDiscardedCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetUDP_RxAppData()
 *
 * @brief    (1) Deframe application data from received UDP packet buffer(s) :
 *               - (a) Validate receive packet buffer(s)
 *               - (b) Validate receive data buffer                                        See Note #4
 *               - (c) Validate receive flags                                              See Note #5
 *               - (d) Get any received IP options                                         See Note #6
 *               - (e) Deframe application data from UDP packet buffer(s)
 *               - (f) Free UDP packet buffer(s)
 *
 * @param    p_buf               Pointer to network buffer that received UDP datagram.
 *
 * @param    p_data_buf          $$$$ Add description for 'p_data_buf'
 *
 * @param    data_buf_len        Size    of application receive buffer (in octets) [see Note #4].
 *
 * @param    flags               Flags to select receive options (see Note #5); bit-field flags logically OR'd :
 *
 * @param    p_ip_opts_buf       $$$$ Add description for 'p_ip_opts_buf'
 *
 * @param    ip_opts_buf_len     Size of IP options receive buffer (in octets)    [see Note #6b].
 *
 * @param    p_ip_opts_len       $$$$ Add description for 'p_ip_opts_len'
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_WOULD_OVF
 *
 * @return   Total application data octets deframed into receive buffer, if NO error(s).
 *           0, otherwise.
 *
 * @note     (2) NetUDP_RxAppData() MUST be called with the global network lock already acquired.
 *
 * @note     (4) (a) Application data receive buffer should be large enough to receive either ...
 *               - (1) The maximum UDP datagram size (i.e. 65,507 octets)
 *                       OR
 *               - (2) The application's expected maximum UDP datagram size
 *           - (b) If the application receive buffer size is NOT large enough for the received UDP datagram,
 *                   the remaining application data octets are discarded & RTOS_ERR_WOULD_OVF error
 *                   is returned.
 *
 * @note     (5) If UDP receive flag options that are NOT implemented are requested, NetUDP_RxAppData() aborts
 *               & returns appropriate error codes so that requested flag options are NOT silently ignored.
 *
 * @note     (6) (a) If ...
 *               - (1) NO IP options were received with the UDP datagram
 *                       OR
 *               - (2) NO IP options receive buffer is provided by the application
 *                       OR
 *               - (3) IP options receive buffer NOT large enough for the received IP options
 *                   ... then NO IP options are returned & any received IP options are silently discarded.
 *           - (b) The IP options receive buffer size SHOULD be large enough to receive the maximum
 *                 IP options size, NET_IP_HDR_OPT_SIZE_MAX.
 *           - (c) IP options are received from the first packet buffer.  In other words, if multiple
 *                 packet buffers are received for a fragmented datagram, IP options are received from
 *                 the first fragment of the datagram.
 *           - (d) (1) (A) RFC #1122, Section 3.2.1.8 states that "all IP options ... received in
 *                           datagrams MUST be passed to the transport layer ... [which] MUST ... interpret
 *                           those IP options that they understand and silently ignore the others".
 *                   - (B) RFC #1122, Section 4.1.3.2 adds that "UDP MUST pass any IP option that it
 *                           receives from the IP layer transparently to the application layer".
 *
 *               - (2) Received IP options should be provided/decoded via appropriate IP layer API. #### NET-811
 *
 * @note     (7) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *
 * @note     (8) [INTERNAL] Since pointer arithmetic is based on the specific pointer data type & inherent pointer
 *               data type size, pointer arithmetic operands :
 *
 *           - (a) MUST be in terms of the specific pointer data type & data type size; ...
 *           - (b) SHOULD NOT & in some cases MUST NOT be cast to other data types or data type sizes.
 *
 * @note     (9) (a) On any internal receive     errors, UDP receive packets are     discarded.
 *           - (b) On any external application errors, UDP receive packets are NOT discarded;
 *                   the application MAY continue to attempt to receive the application data
 *                   via NetUDP_RxAppData().
 *
 *           10) IP options arguments may NOT be necessary.
 *******************************************************************************************************/
CPU_INT16U NetUDP_RxAppData(NET_BUF       *p_buf,
                            void          *p_data_buf,
                            CPU_INT16U    data_buf_len,
                            NET_UDP_FLAGS flags,
                            void          *p_ip_opts_buf,
                            CPU_INT08U    ip_opts_buf_len,
                            CPU_INT08U    *p_ip_opts_len,
                            RTOS_ERR      *p_err)
{
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_INT08U    *p_ip_opts_len_init;
  NET_UDP_FLAGS flag_mask;
#endif
#ifdef  NET_IPv4_MODULE_EN
  CPU_INT08U *p_ip_opts;
  CPU_INT08U ip_opts_len;
  CPU_INT08U ip_opts_len_unused;
#endif
  CPU_BOOLEAN  peek;
  NET_BUF      *p_buf_head;
  NET_BUF      *p_buf_next;
  NET_BUF_HDR  *p_buf_head_hdr;
  NET_BUF_HDR  *p_buf_hdr;
  NET_BUF_SIZE data_len_pkt;
  CPU_INT16U   data_len_buf_rem;
  CPU_INT16U   data_len_tot = 0u;
  CPU_INT08U   *p_data;
  RTOS_ERR     err_rtn;

  PP_UNUSED_PARAM(ip_opts_buf_len);
#if (RTOS_ARG_CHK_EXT_EN == DEF_DISABLED) \
  && !defined(NET_IPv4_MODULE_EN)
  PP_UNUSED_PARAM(p_ip_opts_buf);
  PP_UNUSED_PARAM(p_ip_opts_len);
#endif

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_data_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG_ERR_SET((data_buf_len >= 1), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  RTOS_ERR_SET(err_rtn, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  p_ip_opts_len_init = (CPU_INT08U *)p_ip_opts_len;
#endif

#ifdef  NET_IPv4_MODULE_EN
  if (p_ip_opts_len == DEF_NULL) {                              // If NOT avail, ...
    p_ip_opts_len = (CPU_INT08U *)&ip_opts_len_unused;          // ... re-cfg NULL rtn ptr to unused local var.
    PP_UNUSED_PARAM(ip_opts_len_unused);                        // Prevent possible 'variable unused' warning.
  }
  *p_ip_opts_len = 0u;                                          // Init len for err (see Note #7).
#endif

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ---------------- VALIDATE RX FLAGS -----------------
  flag_mask = NET_UDP_FLAG_NONE
              | NET_UDP_FLAG_RX_DATA_PEEK;
  //                                                               If any invalid flags req'd, rtn err (see Note #5).
  if ((flags & (NET_UDP_FLAGS) ~flag_mask) != NET_UDP_FLAG_NONE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.UDP.InvalidFlagsCtr);
    //                                                             See Note #9b.
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, 0u);
  }

  //                                                               --------------- VALIDATE RX IP OPTS ----------------
  if (((p_ip_opts_buf != DEF_NULL)                              // If (IP opts buf         avail BUT ..
       && (p_ip_opts_len_init == DEF_NULL))                     // ..  IP opts buf len NOT avail) OR ..
      || ((p_ip_opts_buf == DEF_NULL)                           // .. (IP opts buf     NOT avail BUT ..
          && (p_ip_opts_len_init != DEF_NULL))) {               // ..  IP opts buf len     avail),   ..
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, 0u);
  }
#endif

  //                                                               ----------------- GET RX'D IP OPTS -----------------
  //                                                               See Note #6.
  p_buf_hdr = &p_buf->Hdr;
#ifdef  NET_IPv4_MODULE_EN
  if (p_buf_hdr->IP_OptPtr != DEF_NULL) {                       // If IP opts rx'd,                 & ...
    if (p_ip_opts_buf != DEF_NULL) {                            // .. IP opts rx buf avail,         & ...
      if (ip_opts_buf_len >= p_buf_hdr->IP_HdrLen) {            // .. IP opts rx buf size sufficient, ...
        p_ip_opts = &p_buf->DataPtr[p_buf_hdr->IP_HdrIx];
        ip_opts_len = (CPU_INT08U)p_buf_hdr->IP_HdrLen;
        Mem_Copy(p_ip_opts_buf,                                 // .. copy IP opts into rx buf.
                 p_ip_opts,
                 ip_opts_len);

        *p_ip_opts_len = ip_opts_len;
      }
    }
  }
#endif

  //                                                               ------------- DEFRAME UDP APP RX DATA --------------
  p_buf_head = p_buf;
  p_buf_head_hdr = &p_buf_head->Hdr;
  p_data = (CPU_INT08U *)p_data_buf;
  data_len_buf_rem = data_buf_len;
  data_len_tot = 0u;

  while ((p_buf != DEF_NULL)                                    // Copy app rx data from avail pkt buf(s).
         && (data_len_buf_rem > 0)) {
    p_buf_hdr = &p_buf->Hdr;
    p_buf_next = p_buf_hdr->NextBufPtr;

    if (data_len_buf_rem >= p_buf_hdr->DataLen) {               // If rem data buf len >= pkt buf data len, ...
      data_len_pkt = (NET_BUF_SIZE)p_buf_hdr->DataLen;          // ...      copy all      pkt buf data len.
    } else {
      data_len_pkt = (NET_BUF_SIZE)data_len_buf_rem;            // Else lim copy to rem data buf len ...
      RTOS_ERR_SET(err_rtn, RTOS_ERR_WOULD_OVF);                // ... & rtn data size err code (see Note #4b).
    }

    NetBuf_DataRd(p_buf,
                  p_buf_hdr->DataIx,
                  data_len_pkt,
                  p_data);

    //                                                             Update data ptr & lens.
    p_data += data_len_pkt;                                     // MUST NOT cast ptr operand (see Note #8b).
    data_len_tot += (CPU_INT16U)data_len_pkt;
    data_len_buf_rem -= (CPU_INT16U)data_len_pkt;

    p_buf = p_buf_next;
  }

  //                                                               ----------------- FREE UDP RX PKTS -----------------
  peek = DEF_BIT_IS_SET(flags, NET_UDP_FLAG_RX_DATA_PEEK);
  if (peek != DEF_YES) {                                        // If peek opt NOT req'd, pkt buf(s) consumed : ...
    p_buf_head_hdr->NextPrimListPtr = DEF_NULL;                 // ... unlink from any other pkt bufs/chains    ...
    NetUDP_RxPktFree(p_buf_head);                               // ... & free pkt buf(s).
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(err_rtn));

  return (data_len_tot);
}

/****************************************************************************************************//**
 *                                       NetUDP_TxAppDataHandlerIPv4()
 *
 * @brief    (1) Prepare & transmit data from Application layer(s) via UDP layer :
 *               - (a) Validate application data
 *               - (b) Transmit application data via UDP Transmit :
 *                   - (1) Calculate/validate application data buffer size
 *                   - (2) Get buffer(s) for application data
 *                   - (3) Copy application data into UDP packet buffer(s)
 *                   - (4) Initialize UDP packet buffer controls
 *                   - (5) Free UDP packet buffer(s)
 *
 * @param    p_data      Pointer to application data.
 *
 * @param    data_len    Length  of application data (in octets) [see Note #5].
 *
 * @param    src_addr    Source      IP  address.
 *
 * @param    src_port    Source      UDP port.
 *
 * @param    dest_addr   Destination IP  address.
 *
 * @param    dest_port   Destination UDP port.
 *
 * @param    TOS         Specific TOS to transmit UDP/IP packet
 *                       (see 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES').
 *
 * @param    TTL         Specific TTL to transmit UDP/IP packet
 *                       (see 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES') :
 *                           - NET_IP_TTL_MIN                  Minimum TTL transmit value   (1)
 *                           - NET_IP_TTL_MAX                  Maximum TTL transmit value (255)
 *                           - NET_IP_TTL_DFLT                 Default TTL transmit value (128)
 *                           - NET_IP_TTL_NONE                 Replace with default TTL
 *
 * @param    flags_udp   Flags to select UDP transmit options (see Note #4); bit-field flags logically OR'd :
 *                           - NET_UDP_FLAG_NONE               No UDP  transmit flags selected.
 *                           - NET_UDP_FLAG_TX_CHK_SUM_DIS     DISABLE transmit check-sums.
 *                           - NET_UDP_FLAG_TX_BLOCK           Transmit UDP application data with blocking,
 *                                                             if flag set; without blocking, if clear
 *                                                             (see Note #4a).
 *
 * @param    flags_ip    Flags to select IP  transmit options; bit-field flags logically OR'd :
 *                           - NET_IP_FLAG_NONE                No  IP transmit flags selected.
 *                           - NET_IP_FLAG_TX_DONT_FRAG        Set IP 'Don't Frag' flag.
 *
 * @param    p_opts_ip   Pointer to one or more IP options configuration data structures
 *                       (see 'net_ip.h  IP HEADER OPTION CONFIGURATION DATA TYPES') :
 *                       NULL                            NO IP transmit options configuration.
 *                       NET_IP_OPT_CFG_ROUTE_TS         Route &/or Internet Timestamp options configuration.
 *                       NET_IP_OPT_CFG_SECURITY         Security options configuration
 *                       (see 'net_ip.c  Note #1e').
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Number of data octets transmitted, if NO error(s).
 *           0,  otherwise.
 *
 * @note     (2) NetUDP_TxAppDataHandler() is called by network protocol suite function(s) & MUST
 *               be called with the global network lock already acquired.
 *
 * @note     (4) Some UDP transmit flag options NOT yet implemented :
 *               - (a) NET_UDP_FLAG_TX_BLOCK
 *
 * @note     (5) (a) (1) Datagram transmission & reception MUST be atomic -- i.e. every single, complete
 *                       datagram transmitted SHOULD be received as a single, complete datagram.  Thus,
 *                       each call to transmit data MUST be transmitted in a single, complete datagram.
 *               - (2) (A) IEEE Std 1003.1, 2004 Edition, Section 'send() : DESCRIPTION' states that
 *                         "if the message is too long to pass through the underlying protocol, send()
 *                         shall fail and no data shall be transmitted".
 *                   - (B) Since IP transmit fragmentation is NOT currently supported (see 'net_ip.h
 *                         Note #1d'), if the requested datagram transmit data length is greater than
 *                         the UDP MTU, then NO data is transmitted & RTOS_ERR_WOULD_OVF
 *                         error is returned.
 **           - (b) 'data_len' of 0 octets NOT allowed.
 *
 * @note     (6) On ANY transmit error, any remaining application data transmit is immediately aborted.
 *******************************************************************************************************/
#ifdef  NET_IPv4_MODULE_EN
CPU_INT16U NetUDP_TxAppDataHandlerIPv4(NET_IF_NBR     if_nbr,
                                       void           *p_data,
                                       CPU_INT16U     data_len,
                                       NET_IPv4_ADDR  src_addr,
                                       NET_PORT_NBR   src_port,
                                       NET_IPv4_ADDR  dest_addr,
                                       NET_PORT_NBR   dest_port,
                                       NET_IPv4_TOS   TOS,
                                       NET_IPv4_TTL   TTL,
                                       NET_UDP_FLAGS  flags_udp,
                                       NET_IPv4_FLAGS flags_ip,
                                       void           *p_opts_ip,
                                       RTOS_ERR       *p_err)
{
  NET_BUF      *p_buf = DEF_NULL;
  NET_BUF_HDR  *p_buf_hdr;
  NET_MTU      udp_mtu;
  NET_BUF_SIZE buf_size_max;
  NET_BUF_SIZE buf_size_max_data;
  NET_BUF_SIZE data_ix_pkt;
  NET_BUF_SIZE data_ix_pkt_offset;
  NET_BUF_SIZE data_len_pkt;
  CPU_INT16U   data_len_tot = 0u;
  CPU_INT08U   *p_data_pkt;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_SET((p_data != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG_ERR_SET((data_len > NET_UDP_DATA_LEN_MIN), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  if (if_nbr == NET_IF_NBR_NONE) {
    if_nbr = NetIPv4_GetAddrHostIF_Nbr(src_addr);               // Get IF nbr of src addr.
    if (if_nbr == NET_IF_NBR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_INVALID_ADDR_SRC);
      goto exit_discard;
    }
  }
  //                                                               Get IF's UDP MTU.
  udp_mtu = NetIF_MTU_GetProtocol(if_nbr,
                                  NET_PROTOCOL_TYPE_UDP_V4,
                                  NET_IF_FLAG_NONE);

  //                                                               ------------------- TX APP DATA --------------------
  //                                                               Calc buf max data size.
  data_ix_pkt = 0u;
  NetUDP_GetTxDataIx(if_nbr,
                     NET_PROTOCOL_TYPE_UDP_V4,
                     data_len,
                     flags_udp,
                     &data_ix_pkt);

  buf_size_max = NetBuf_GetMaxSize(if_nbr,
                                   NET_TRANSACTION_TX,
                                   DEF_NULL,
                                   data_ix_pkt);

  buf_size_max_data = (NET_BUF_SIZE)DEF_MIN(buf_size_max, udp_mtu);

  if (data_len > buf_size_max_data) {                           // If data len > max data size, abort tx ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);                       // ... & rtn size err (see Note #5a2B).
    goto exit_discard;
  } else {                                                      // Else lim pkt data len to data len.
    data_len_pkt = (NET_BUF_SIZE)data_len;
  }

  data_len_tot = 0u;
  p_data_pkt = (CPU_INT08U *)p_data;
  //                                                               Get app data tx buf.
  p_buf = NetBuf_Get(if_nbr,
                     NET_TRANSACTION_TX,
                     data_len_pkt,
                     data_ix_pkt,
                     &data_ix_pkt_offset,
                     NET_BUF_FLAG_NONE,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  data_ix_pkt += data_ix_pkt_offset;
  NetBuf_DataWr(p_buf,                             // Wr app data into app data tx buf.
                data_ix_pkt,
                data_len_pkt,
                p_data_pkt);

  //                                                               Init app data tx buf ctrls.
  p_buf_hdr = &p_buf->Hdr;
  p_buf_hdr->DataIx = data_ix_pkt;
  p_buf_hdr->DataLen = data_len_pkt;
  p_buf_hdr->TotLen = p_buf_hdr->DataLen;
  p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_UDP_V4;

  NetUDP_TxIPv4(p_buf,                                          // Tx app data buf via UDP tx.
                src_addr,
                src_port,
                dest_addr,
                dest_port,
                TOS,
                TTL,
                flags_udp,
                flags_ip,
                p_opts_ip,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  NetUDP_TxPktFree(p_buf);                                      // Free app data tx buf.

  data_len_tot += data_len_pkt;                                 // Calc tot app data len tx'd.

  goto exit;

exit_discard:
  NetUDP_TxPktDiscard(p_buf);

exit:
  return (data_len_tot);
}
#endif

/****************************************************************************************************//**
 *                                       NetUDP_TxAppDataHandlerIPv6()
 *
 * @brief    (1) Prepare & transmit data from Application layer(s) via UDP layer :
 *               - (a) Validate application data
 *               - (b) Transmit application data via UDP Transmit :
 *                   - (1) Calculate/validate application data buffer size
 *                   - (2) Get buffer(s) for application data
 *                   - (3) Copy application data into UDP packet buffer(s)
 *                   - (4) Initialize UDP packet buffer controls
 *                   - (5) Free UDP packet buffer(s)
 *
 * @param    p_data          Pointer to application data.
 *
 * @param    data_len        Length  of application data (in octets) [see Note #5].
 *
 * @param    p_src_addr      Pointer Source IP address.
 *
 * @param    src_port        Source UDP port.
 *
 * @param    p_dest_addr     Pointer Destination IP address.
 *
 * @param    dest_port       Destination UDP port.
 *
 * @param    traffic_class   IPv6 traffic class value to use.
 *
 * @param    flow_label      IPv6 flow label value to use.
 *
 * @param    hop_lim         IPv6 hop limit value to use.
 *
 * @param    flags_udp       Flags to select UDP transmit options (see Note #3); bit-field flags logically OR'd :
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Number of data octets transmitted, if NO error(s).
 *           0, otherwise.
 *
 * @note     (2) NetUDP_TxAppDataHandler() is called by network protocol suite function(s) & MUST
 *               be called with the global network lock already acquired.
 *
 * @note     (3) Some UDP transmit flag options NOT yet implemented :
 *
 *           - (a) NET_UDP_FLAG_TX_BLOCK
 *
 * @note     (4) (a) (1) Datagram transmission & reception MUST be atomic -- i.e. every single, complete
 *                       datagram transmitted SHOULD be received as a single, complete datagram.  Thus,
 *                       each call to transmit data MUST be transmitted in a single, complete datagram.
 *
 *               - (2) (A) IEEE Std 1003.1, 2004 Edition, Section 'send() : DESCRIPTION' states that
 *                           "if the message is too long to pass through the underlying protocol, send()
 *                           shall fail and no data shall be transmitted".
 *
 *                   - (B) Since IP transmit fragmentation is NOT currently supported (see 'net_ip.h
 *                           Note #1d'), if the requested datagram transmit data length is greater than
 *                           the UDP MTU, then NO data is transmitted & RTOS_ERR_WOULD_OVF
 *                           error is returned.
 *
 *           - (b) 'data_len' of 0 octets NOT allowed.
 *
 * @note     (5) On ANY transmit error, any remaining application data transmit is immediately aborted.
 *******************************************************************************************************/
#ifdef  NET_IPv6_MODULE_EN
CPU_INT16U NetUDP_TxAppDataHandlerIPv6(void                   *p_data,
                                       CPU_INT16U             data_len,
                                       NET_IPv6_ADDR          *p_src_addr,
                                       NET_PORT_NBR           src_port,
                                       NET_IPv6_ADDR          *p_dest_addr,
                                       NET_PORT_NBR           dest_port,
                                       NET_IPv6_TRAFFIC_CLASS traffic_class,
                                       NET_IPv6_FLOW_LABEL    flow_label,
                                       NET_IPv6_HOP_LIM       hop_lim,
                                       NET_UDP_FLAGS          flags_udp,
                                       RTOS_ERR               *p_err)
{
  NET_BUF      *p_buf = DEF_NULL;
  NET_BUF_HDR  *p_buf_hdr;
  NET_IF_NBR   if_nbr;
  NET_MTU      udp_mtu;
  NET_BUF_SIZE buf_size_max;
  NET_BUF_SIZE buf_size_max_data;
  NET_BUF_SIZE data_ix_pkt;
  NET_BUF_SIZE data_ix_pkt_offset;
  NET_BUF_SIZE data_len_pkt;
  CPU_INT16U   data_len_tot = 0;
  CPU_INT08U   *p_data_pkt;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_SET((p_data != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG_ERR_SET((data_len > 1), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  if_nbr = NetIPv6_GetAddrHostIF_Nbr(p_src_addr);               // Get IF nbr of src addr.
  if (if_nbr == NET_IF_NBR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_INVALID_ADDR_SRC);
    goto exit_discard;
  }
  //                                                               Get IF's UDP MTU.
  udp_mtu = NetIF_MTU_GetProtocol(if_nbr, NET_PROTOCOL_TYPE_UDP_V6, NET_IF_FLAG_NONE);

  //                                                               ------------------- TX APP DATA --------------------
  //                                                               Calc buf max data size.
  data_ix_pkt = 0u;
  NetUDP_GetTxDataIx(if_nbr, NET_PROTOCOL_TYPE_UDP_V6, data_len, flags_udp, &data_ix_pkt);

  buf_size_max = NetBuf_GetMaxSize(if_nbr,
                                   NET_TRANSACTION_TX,
                                   DEF_NULL,
                                   data_ix_pkt);

  buf_size_max_data = (NET_BUF_SIZE)DEF_MIN(buf_size_max, udp_mtu);

  if (data_len > buf_size_max_data) {                           // If data len > max data size, abort tx ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);                       // ... & rtn size err (see Note #5a2B).
    goto exit_discard;
  } else {                                                      // Else lim pkt data len to data len.
    data_len_pkt = (NET_BUF_SIZE)data_len;
  }

  data_len_tot = 0u;
  p_data_pkt = (CPU_INT08U *)p_data;
  //                                                               Get app data tx buf.
  p_buf = NetBuf_Get(if_nbr,
                     NET_TRANSACTION_TX,
                     data_len_pkt,
                     data_ix_pkt,
                     &data_ix_pkt_offset,
                     NET_BUF_FLAG_NONE,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  data_ix_pkt += data_ix_pkt_offset;
  NetBuf_DataWr(p_buf,                                          // Wr app data into app data tx buf.
                data_ix_pkt,
                data_len_pkt,
                p_data_pkt);

  //                                                               Init app data tx buf ctrls.
  p_buf_hdr = &p_buf->Hdr;
  p_buf_hdr->DataIx = (CPU_INT16U)data_ix_pkt;
  p_buf_hdr->DataLen = (NET_BUF_SIZE)data_len_pkt;
  p_buf_hdr->TotLen = (NET_BUF_SIZE)p_buf_hdr->DataLen;
  p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_UDP_V6;

  DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_IPv6_FRAME);

  NetUDP_TxIPv6(p_buf,                                               // Tx app data buf via UDP tx.
                p_src_addr,
                src_port,
                p_dest_addr,
                dest_port,
                traffic_class,
                flow_label,
                hop_lim,
                flags_udp,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  NetUDP_TxPktFree(p_buf);                                       // Free app data tx buf.

  data_len_tot += data_len_pkt;                                 // Calc tot app data len tx'd.

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  goto exit;

exit_discard:
  NetUDP_TxPktDiscard(p_buf);

exit:
  return (data_len_tot);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetUDP_RxPktValidate()
 *
 * @brief    (1) Validate received UDP packet :
 *
 *           - (a) Validate the received packet's following UDP header fields :
 *
 *               - (1) Source      Port
 *               - (2) Destination Port
 *               - (3) Datagram Length                                 See Note #3
 *               - (4) Check-Sum                                       See Note #4
 *
 *           - (b) Convert the following UDP header fields from network-order to host-order :
 *
 *               - (1) Source      Port                                See Notes #1bB1
 *               - (2) Destination Port                                See Notes #1bB2
 *               - (3) Datagram Length                                 See Notes #1bB3
 *               - (4) Check-Sum                                       See Note  #4d
 *
 *                   - (A) These fields are NOT converted directly in the received packet buffer's
 *                           data area but are converted in local or network buffer variables ONLY.
 *
 *                   - (B) The following UDP header fields are converted & stored in network buffer
 *                           variables :
 *
 *                       - (1) Source      Port
 *                       - (2) Destination Port
 *                       - (3) Datagram Length
 *
 *           - (c) Update network buffer's protocol controls
 *
 * @param    p_buf       Pointer to network buffer that received UDP packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_udp_hdr   Pointer to received packet's UDP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See 'net_udp.h  UDP HEADER' for UDP header format.
 *
 * @note     (3) In addition to validating that the UDP Datagram Length is greater than or equal to the
 *                       minimum UDP header length,     the UDP Datagram Length is compared to the remaining IP
 *                       Datagram Length which should be identical.
 *
 * @note     (4) (a) UDP header Check-Sum field MUST be validated BEFORE (or AFTER) any multi-octet words
 *                       are converted from network-order to host-order since "the sum of 16-bit integers can
 *                       be computed in either byte order" [RFC #1071, Section 2.(B)].
 *
 *                       In other words, the UDP Datagram Check-Sum CANNOT be validated AFTER SOME but NOT ALL
 *                       multi-octet words have been converted from network-order to host-order.
 *
 *                           - (b) However, ALL received packets' multi-octet words are converted in local or network
 *                       buffer variables ONLY (see Note #1bA).  Therefore, UDP Datagram Check-Sum may be
 *                       validated at any point.
 *
 *                           - (c) The UDP Datagram Check-Sum MUST be validated AFTER the datagram length field has been
 *                       validated so that the total UDP Datagram Length (in octets) will already be calculated
 *                       for the UDP Check-Sum calculation.
 *
 *                       For efficiency, the UDP Datagram Check-Sum is validated AFTER all other UDP header
 *                       fields have been validated.  Thus the iteration-intensive UDP Datagram Check-Sum is
 *                       calculated only after all other UDP header fields have been quickly validated.
 *
 *                           - (d) (1) Before the UDP Datagram Check-Sum is validated, it is necessary to convert the
 *                       Check-Sum from network-order to host-order to verify whether the received UDP
 *                       datagram's Check-Sum is valid -- i.e. whether the UDP datagram was transmitted
 *                       with or without a computed Check-Sum (see RFC #768, Section 'Fields : Checksum').
 *
 *                           - (2) Since the value that indicates no check-sum was computed for the received UDP
 *                       datagram is one's-complement positive zero -- all check-sum bits equal to zero,
 *                       a value that is endian-order independent -- it is NOT absolutely necessary to
 *                       convert the UDP Datagram Check-Sum from network-order to host-order.
 *
 *                       However, network data value macro's inherently convert data words from network
 *                       word order to CPU word order.
 *
 *                       See also 'net_util.h  NETWORK DATA VALUE MACRO'S  Note #1a1'.
 *
 *                           - (3) (A) Any UDP datagram received with NO computed check-sum is flagged so that "an
 *                       application MAY optionally ... discard ... UDP datagrams without checksums"
 *                       (see RFC #1122, Section 4.1.3.4).
 *
 *                       Run-time API to handle/discard UDP datagrams without checksums NOT yet
 *                       implemented. #### NET-819
 *
 *                           - (B) UDP buffer flag value to clear was previously initialized in NetBuf_Get() when
 *                       the buffer was allocated.  This buffer flag value does NOT need to be re-cleared
 *                       but is shown for completeness.
 *
 *                           - (e) (1) In addition to the UDP datagram header & data, the UDP Check-Sum calculation
 *                       includes "a pseudo header of information from the IP header ... conceptually
 *                       prefixed to the UDP header [which] contains the source address, the destination
 *                       address, the protocol, and the UDP length" (see RFC #768, Section 'Fields :
 *                       Checksum').
 *
 *                           - (2) Since network check-sum functions REQUIRE that 16-bit one's-complement check-
 *                       sum calculations be performed on headers & data arranged in network-order (see
 *                       'net_util.c  NetUtil_16BitOnesCplChkSumDataVerify()  Note #4'), UDP pseudo-header
 *                       values MUST be set or converted to network-order.
 *
 *                           - (f) RFC #768, Section 'Fields : Checksum' specifies that "the data [is] padded with zero
 *                       octets at the end (if necessary) to make a multiple of two octets".
 *
 *                       See also 'net_util.c  NetUtil_16BitSumDataCalc()  Note #8'.
 *
 * @note     (5) (a) Since the minimum network buffer size MUST be configured such that the entire UDP
 *                       header MUST be received in a single packet (see 'net_buf.h  NETWORK BUFFER INDEX &
 *                       SIZE DEFINES  Note #1c'), after the UDP header size is decremented from the first
 *                       packet buffer's remaining number of data octets, any remaining octets MUST be user
 *                       &/or application data octets.
 *
 *                           - (1) Note that the 'Data' index is updated regardless of a null-size data length.
 *
 *                           - (b) If additional packet buffers exist, the remaining IP datagram 'Data' MUST be user
 *                       &/or application data.  Therefore, the 'Data' length does NOT need to be adjusted
 *                       but the 'Data' index MUST be updated.
 *
 *                           - (c) #### Total UDP Datagram Length & Data Length is duplicated in ALL fragmented packet
 *                       buffers (may NOT be necessary; remove if unnecessary).
 *
 * @note     (6) RFC #1122, Sections 3.2.1 & 3.2.2 require that IP & ICMP packets with certain invalid
 *                       header fields be "silently discarded".  However, NO RFC specifies how UDP should handle
 *                       received datagrams with invalid header fields.
 *
 *                       In addition, UDP is a "transaction oriented" protocol that does NOT guarantee "delivery
 *                       and duplicate protection" of UDP datagrams (see RFC #768, Section 'Introduction').
 *
 *                       Therefore, it is assumed that ALL UDP datagrams with ANY invalid header fields SHOULD
 *                       be silently discarded.
 *
 * @note     (7) (a) RFC #1122, Section 3.2.1.8 states that "all IP options ... received in datagrams
 *                       MUST be passed to the transport layer ... [which] MUST ... interpret those IP
 *                       options that they understand and silently ignore the others".
 *
 *                           - (b) RFC #1122, Section 4.1.3.2 adds that "UDP MUST pass any IP option that it receives
 *                       from the IP layer transparently to the application layer".
 *
 *                       See also 'NetUDP_RxAppData()  Note #6d'.
 *******************************************************************************************************/
static NET_UDP_ERR_TYPE NetUDP_RxPktValidate(NET_BUF     *p_buf,
                                             NET_BUF_HDR *p_buf_hdr,
                                             NET_UDP_HDR *p_udp_hdr,
                                             RTOS_ERR    *p_err)
{
#if defined(NET_IPv4_MODULE_EN) && !defined(NET_UDP_CHK_SUM_OFFLOAD_RX)
  NET_UDP_PSEUDO_HDR udp_pseudo_hdr;
#endif
#if defined(NET_IPv6_MODULE_EN) && !defined(NET_UDP_CHK_SUM_OFFLOAD_RX)
  NET_IPv6_PSEUDO_HDR ipv6_pseudo_hdr;
#endif
  CPU_INT16U       udp_tot_len;
  CPU_INT16U       udp_data_len;
  NET_CHK_SUM      udp_chk_sum;
  CPU_BOOLEAN      udp_chk_sum_valid;
  NET_BUF          *p_buf_next;
  NET_BUF_HDR      *p_buf_next_hdr;
  CPU_BOOLEAN      udp_chk_sum_ipv6;
  NET_UDP_ERR_TYPE err_type = NET_UDP_ERR_TYPE_UNKNOW;

#ifdef  NET_UDP_CHK_SUM_OFFLOAD_RX
  PP_UNUSED_PARAM(p_buf);
#endif
  //                                                               ---------------- VALIDATE UDP PORTS ----------------
  NET_UTIL_VAL_COPY_GET_NET_16(&p_buf_hdr->TransportPortSrc, &p_udp_hdr->PortSrc);
  if (p_buf_hdr->TransportPortSrc == NET_UDP_PORT_NBR_RESERVED) {
    err_type = NET_UDP_ERR_TYPE_DEST_UNREACHABLE;
    NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxHdrPortSrcCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  NET_UTIL_VAL_COPY_GET_NET_16(&p_buf_hdr->TransportPortDest, &p_udp_hdr->PortDest);
  if (p_buf_hdr->TransportPortDest == NET_UDP_PORT_NBR_RESERVED) {
    err_type = NET_UDP_ERR_TYPE_DEST_UNREACHABLE;
    NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxHdrPortDestCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------------ VALIDATE UDP DATAGRAM LEN -------------
  //                                                               See Note #3.
  NET_UTIL_VAL_COPY_GET_NET_16(&udp_tot_len, &p_udp_hdr->DatagramLen);
  p_buf_hdr->TransportTotLen = udp_tot_len;
  if (p_buf_hdr->TransportTotLen < NET_UDP_TOT_LEN_MIN) {        // If datagram len <  min tot     len, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxHdrDatagramLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
  if (p_buf_hdr->TransportTotLen > NET_UDP_TOT_LEN_MAX) {        // If datagram len >  max tot     len, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxHdrDatagramLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (p_buf_hdr->TransportTotLen != p_buf_hdr->IP_DatagramLen) {  // If datagram len != IP datagram len, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxHdrDatagramLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               --------------- VALIDATE UDP CHK SUM ---------------
  //                                                               See Note #4.
  NET_UTIL_VAL_COPY_GET_NET_16(&udp_chk_sum, &p_udp_hdr->ChkSum);

  udp_chk_sum_ipv6 = DEF_BIT_IS_SET(p_buf_hdr->Flags, NET_BUF_FLAG_IPv6_FRAME);

  if (udp_chk_sum != NET_UDP_HDR_CHK_SUM_NONE) {                // If chk sum rx'd, verify chk sum (see Note #4d).
                                                                // Prepare UDP chk sum pseudo-hdr  (see Note #4e).

    if (udp_chk_sum_ipv6 == DEF_NO) {
#ifdef NET_IPv4_MODULE_EN
#ifdef NET_UDP_CHK_SUM_OFFLOAD_RX
      udp_chk_sum_valid = DEF_YES;
      PP_UNUSED_PARAM(udp_chk_sum_valid);

#else
      udp_pseudo_hdr.AddrSrc = (NET_IPv4_ADDR)NET_UTIL_HOST_TO_NET_32(p_buf_hdr->IP_AddrSrc);
      udp_pseudo_hdr.AddrDest = (NET_IPv4_ADDR)NET_UTIL_HOST_TO_NET_32(p_buf_hdr->IP_AddrDest);
      udp_pseudo_hdr.Zero = (CPU_INT08U)0x00u;
      udp_pseudo_hdr.Protocol = (CPU_INT08U)NET_IP_HDR_PROTOCOL_UDP;
      udp_pseudo_hdr.DatagramLen = (CPU_INT16U)NET_UTIL_HOST_TO_NET_16(p_buf_hdr->TransportTotLen);
      udp_chk_sum_valid = NetUtil_16BitOnesCplChkSumDataVerify(p_buf,
                                                               &udp_pseudo_hdr,
                                                               NET_UDP_PSEUDO_HDR_SIZE);
      if (udp_chk_sum_valid != DEF_OK) {
        NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxHdrChkSumCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
#endif
#else
      udp_chk_sum_valid = DEF_FAIL;
#endif
    } else {
#ifdef NET_IPv6_MODULE_EN
#ifdef NET_UDP_CHK_SUM_OFFLOAD_RX
      udp_chk_sum_valid = DEF_YES;
      PP_UNUSED_PARAM(udp_chk_sum_valid);
#else
      ipv6_pseudo_hdr.AddrSrc = p_buf_hdr->IPv6_AddrSrc;
      ipv6_pseudo_hdr.AddrDest = p_buf_hdr->IPv6_AddrDest;
      ipv6_pseudo_hdr.UpperLayerPktLen = NET_UTIL_HOST_TO_NET_32(p_buf_hdr->TransportTotLen);
      ipv6_pseudo_hdr.Zero = 0x00u;
      ipv6_pseudo_hdr.NextHdr = NET_UTIL_NET_TO_HOST_16(NET_IP_HDR_PROTOCOL_UDP);
      udp_chk_sum_valid = NetUtil_16BitOnesCplChkSumDataVerify(p_buf,
                                                               &ipv6_pseudo_hdr,
                                                               NET_IPv6_PSEUDO_HDR_SIZE);
      if (udp_chk_sum_valid != DEF_OK) {
        NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxHdrChkSumCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
#endif
#else
      udp_chk_sum_valid = DEF_FAIL;
#endif
    }

    DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_RX_UDP_CHK_SUM_VALID);
  } else {                                                      // Else discard or flag NO rx'd chk sum (see Note #4d3).
#if (NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN != DEF_DISABLED)
    NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxHdrChkSumCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
#endif
#if 0                                                           // Clr'd in NetBuf_Get() [see Note #4d3B].
    DEF_BIT_CLR(p_buf_hdr->Flags, NET_BUF_FLAG_RX_UDP_CHK_SUM_VALID);
#endif
  }

  //                                                               ----------------- UPDATE BUF CTRLS -----------------
  //                                                               Calc UDP data len/ix (see Note #5a).
  p_buf_hdr->TransportHdrLen = NET_UDP_HDR_SIZE;

  if (p_buf_hdr->TransportHdrLen > udp_tot_len) {
    NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxHdrDataLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
  if (p_buf_hdr->TransportHdrLen > p_buf_hdr->DataLen) {
    NET_CTR_ERR_INC(Net_ErrCtrs.UDP.RxHdrDataLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  udp_data_len = udp_tot_len - p_buf_hdr->TransportHdrLen;
  p_buf_hdr->TransportDataLen = udp_data_len;

  p_buf_hdr->DataLen -= (NET_BUF_SIZE) p_buf_hdr->TransportHdrLen;
  p_buf_hdr->DataIx = (CPU_INT16U)(p_buf_hdr->TransportHdrIx + p_buf_hdr->TransportHdrLen);
  p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_APP;

  p_buf_next = p_buf_hdr->NextBufPtr;
  while (p_buf_next != DEF_NULL) {                              // Calc ALL pkt bufs' data len/ix    (see Note #5b).
    p_buf_next_hdr = &p_buf_next->Hdr;
    p_buf_next_hdr->DataIx = p_buf_next_hdr->TransportHdrIx;
    p_buf_next_hdr->TransportHdrLen = 0u;                       // NULL UDP hdr  len in each pkt buf.
    p_buf_next_hdr->TransportTotLen = udp_tot_len;              // Dup  UDP tot  len & ...
    p_buf_next_hdr->TransportDataLen = udp_data_len;            // ...      data len in each pkt buf (see Note #5c).
    p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_APP;
    p_buf_next = p_buf_next_hdr->NextBufPtr;
  }

  PP_UNUSED_PARAM(udp_chk_sum_valid);

exit:
  return (err_type);
}

/****************************************************************************************************//**
 *                                       NetUDP_RxPktDemuxDatagram()
 *
 * @brief    Demultiplex UDP datagram to appropriate socket or application connection.
 *
 * @param    p_buf   Pointer to network buffer that received UDP datagram.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) (a) Attempt demultiplex of received UDP datagram to socket connections first, if enabled.
 *
 *                       - (b) On any error(s), attempt demultiplex to application connections, if enabled.
 *
 * @note     (2) When network buffer is demultiplexed to socket or application receive, the buffer's reference
 *                   counter is NOT incremented since the UDP layer does NOT maintain a reference to the buffer.
 *******************************************************************************************************/
static void NetUDP_RxPktDemuxDatagram(NET_BUF  *p_buf,
                                      RTOS_ERR *p_err)
{
  NetSock_Rx(p_buf, p_err);
}

/****************************************************************************************************//**
 *                                           NetUDP_RxPktFree()
 *
 * @brief    Free network buffer(s).
 *
 * @param    p_buf   Pointer to network buffer.
 *******************************************************************************************************/
static void NetUDP_RxPktFree(NET_BUF *p_buf)
{
  (void)NetBuf_FreeBufList(p_buf, DEF_NULL);
}

/****************************************************************************************************//**
 *                                               NetUDP_Tx()
 *
 * @brief    (1) Prepare & transmit UDP datagram packet(s) :
 *
 *           - (a) Validate transmit packet
 *           - (b) Prepare  UDP datagram header
 *           - (c) Transmit UDP packet
 *           - (d) Update   transmit statistics
 *
 * @param    p_buf       Pointer to network buffer to transmit UDP packet.
 *
 * @param    src_addr    Source      IP  address.
 *
 * @param    src_port    Source      UDP port.
 *
 * @param    dest_addr   Destination IP  address.
 *
 * @param    dest_port   Destination UDP port.
 *
 * @param    TOS         Specific TOS to transmit UDP/IP packet
 *                       (see 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES').
 *
 * @param    TTL         Specific TTL to transmit UDP/IP packet
 *                       (see 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES') :
 *                       NET_IP_TTL_MIN                  Minimum TTL transmit value   (1)
 *                       NET_IP_TTL_MAX                  Maximum TTL transmit value (255)
 *                       NET_IP_TTL_DFLT                 Default TTL transmit value (128)
 *                       NET_IP_TTL_NONE                 Replace with default TTL
 *
 * @param    flags_udp   Flags to select UDP transmit options (see Note #2); bit-field flags logically OR'd :
 *                       NET_UDP_FLAG_NONE               No UDP  transmit flags selected.
 *                       NET_UDP_FLAG_TX_CHK_SUM_DIS     DISABLE transmit check-sums.
 *                       NET_UDP_FLAG_TX_BLOCK           Transmit UDP application data with blocking,
 *                       if flag set; without blocking, if clear
 *                       (see Note #2a).
 *
 * @param    flags_ip    Flags to select IP  transmit options; bit-field flags logically OR'd :
 *                       NET_IP_FLAG_NONE                No  IP transmit flags selected.
 *                       NET_IP_FLAG_TX_DONT_FRAG        Set IP 'Don't Frag' flag.
 *
 * @param    p_opts_ip   Pointer to one or more IP options configuration data structures
 *                       (see 'net_ip.h  IP HEADER OPTION CONFIGURATION DATA TYPES') :
 *                       NULL                            NO IP transmit options configuration.
 *                       NET_IP_OPT_CFG_ROUTE_TS         Route &/or Internet Timestamp options configuration.
 *                       NET_IP_OPT_CFG_SECURITY         Security options configuration
 *                       (see 'net_ip.c  Note #1e').
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Some UDP transmit flag options NOT yet implemented :
 *
 *                           - (a) NET_UDP_FLAG_TX_BLOCK
 *
 * @note     (3) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
#ifdef  NET_IPv4_MODULE_EN
static void NetUDP_TxIPv4(NET_BUF        *p_buf,
                          NET_IPv4_ADDR  src_addr,
                          NET_PORT_NBR   src_port,
                          NET_IPv4_ADDR  dest_addr,
                          NET_PORT_NBR   dest_port,
                          NET_IPv4_TOS   TOS,
                          NET_IPv4_TTL   TTL,
                          NET_UDP_FLAGS  flags_udp,
                          NET_IPv4_FLAGS flags_ip,
                          void           *p_opts_ip,
                          RTOS_ERR       *p_err)
{
  NET_BUF_HDR *p_buf_hdr = &p_buf->Hdr;

  //                                                               ------------------ PREPARE UDP HDR -----------------
  NetUDP_TxPktPrepareHdr(p_buf,
                         p_buf_hdr,
                         &src_addr,
                         src_port,
                         &dest_addr,
                         dest_port,
                         flags_udp);

  //                                                               -------------------- TX UDP PKT --------------------
  NetIPv4_Tx(p_buf,
             src_addr,
             dest_addr,
             TOS,
             TTL,
             flags_ip,
             p_opts_ip,
             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.UDP.TxDgramCtr);

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.UDP.TxPktDiscardedCtr);

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                               NetUDP_TxIPv6()
 *
 * @brief    (1) Prepare & transmit UDP datagram packet(s) :
 *
 *           - (a) Validate transmit packet
 *           - (b) Prepare  UDP datagram header
 *           - (c) Transmit UDP packet
 *           - (d) Update   transmit statistics
 *
 * @param    p_buf           Pointer to network buffer to transmit UDP packet.
 *
 * @param    p_src_addr      Pointer Source IPv6 address.
 *
 * @param    src_port        Source UDP port.
 *
 * @param    p_dest_addr     Pointer to destination IPv6  address.
 *
 * @param    dest_port       Destination UDP port.
 *
 * @param    traffic_class   IPv6 traffic class value to use.
 *
 * @param    flow_label      IPv6 flow label value to use.
 *
 * @param    hop_lim         IPv6 hop limit value to use.
 *
 * @param    flags_udp       Flags to select UDP transmit options (see Note #2); bit-field flags logically OR'd:
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Some UDP transmit flag options NOT yet implemented :
 *
 *                               - (a) NET_UDP_FLAG_TX_BLOCK
 *
 * @note     (3) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
#ifdef  NET_IPv6_MODULE_EN
static void NetUDP_TxIPv6(NET_BUF                *p_buf,
                          NET_IPv6_ADDR          *p_src_addr,
                          NET_PORT_NBR           src_port,
                          NET_IPv6_ADDR          *p_dest_addr,
                          NET_PORT_NBR           dest_port,
                          NET_IPv6_TRAFFIC_CLASS traffic_class,
                          NET_IPv6_FLOW_LABEL    flow_label,
                          NET_IPv6_HOP_LIM       hop_lim,
                          NET_UDP_FLAGS          flags_udp,
                          RTOS_ERR               *p_err)
{
  NET_BUF_HDR *p_buf_hdr = &p_buf->Hdr;

  //                                                               ------------------ PREPARE UDP HDR -----------------
  NetUDP_TxPktPrepareHdr(p_buf,
                         p_buf_hdr,
                         p_src_addr,
                         src_port,
                         p_dest_addr,
                         dest_port,
                         flags_udp);

  //                                                               -------------------- TX UDP PKT --------------------
  NetIPv6_Tx(p_buf,
             p_src_addr,
             p_dest_addr,
             DEF_NULL,
             traffic_class,
             flow_label,
             hop_lim,
             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.UDP.TxDgramCtr);

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.UDP.TxPktDiscardedCtr);

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                           NetUDP_TxPktPrepareHdr()
 *
 * @brief    (1) Prepare UDP header :
 *
 *           - (a) Update network buffer's protocol index & length controls
 *
 *           - (b) Prepare the transmit packet's following UDP header fields :
 *
 *               - (1) Source      Port
 *               - (2) Destination Port
 *               - (3) Datagram Length
 *               - (4) Check-Sum                                   See Note #3
 *
 *           - (c) Convert the following UDP header fields from host-order to network-order :
 *
 *               - (1) Source      Port
 *               - (2) Destination Port
 *               - (3) Datagram Length
 *               - (4) Check-Sum                                   See Note #3g
 *
 * @param    p_buf       Pointer to network buffer to transmit UDP packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    src_addr    Source      IP  address.
 *
 * @param    src_port    Source      UDP port.
 *
 * @param    dest_addr   Destination IP  address.
 *
 * @param    dest_port   Destination UDP port.
 *
 * @param    flags_udp   Flags to select UDP transmit options (see Note #2); bit-field flags logically OR'd :
 *
 * @param    ---------  NET_UDP_FLAG_NONE               No UDP  transmit flags selected.
 *                           NET_UDP_FLAG_TX_CHK_SUM_DIS     DISABLE transmit check-sums.
 *                           NET_UDP_FLAG_TX_BLOCK           Transmit UDP application data with blocking,
 *                                                               if flag set; without blocking, if clear
 *                                                               (see Note #2a).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Some UDP transmit flag options NOT yet implemented :
 *
 *                           - (a) NET_UDP_FLAG_TX_BLOCK
 *
 * @note     (3) (a) UDP header Check-Sum MUST be calculated AFTER the entire UDP header has been prepared.
 *                       In addition, ALL multi-octet words are converted from host-order to network-order
 *                       since "the sum of 16-bit integers can be computed in either byte order" [RFC #1071,
 *                       Section 2.(B)].
 *
 *                           - (b) RFC #1122, Section 4.1.3.4 states that "an application MAY optionally be able to
 *                       control whether a UDP checksum will be generated".
 *
 *                           - (c) Although neither RFC #768 nor RFC #1122, Sections 4.1 expressly specifies, it is
 *                       assumed that that the UDP header Check-Sum field MUST be cleared to '0' BEFORE the
 *                       UDP header Check-Sum is calculated.
 *
 *                       See also 'net_ip.c    NetIP_TxPktPrepareHdr()   Note #6b',
 *                       'net_icmp.c  NetICMP_TxMsgErr()        Note #6b',
 *                       'net_icmp.c  NetICMP_TxMsgReq()        Note #7b',
 *                       'net_icmp.c  NetICMP_TxMsgReply()      Note #5b',
 *                       'net_tcp.c   NetTCP_TxPktPrepareHdr()  Note #3b'.
 *
 *                           - (d) (1) In addition to the UDP datagram header & data, the UDP Check-Sum calculation
 *                       includes "a pseudo header of information from the IP header ... conceptually
 *                       prefixed to the UDP header [which] contains the source address, the destination
 *                       address, the protocol, and the UDP length" (see RFC #768, Section 'Fields :
 *                       Checksum').
 *
 *                           - (2) Since network check-sum functions REQUIRE that 16-bit one's-complement check-
 *                       sum calculations be performed on headers & data arranged in network-order (see
 *                       'net_util.c  NetUtil_16BitOnesCplChkSumDataCalc()  Note #3'), UDP pseudo-header
 *                       values MUST be set or converted to network-order.
 *
 *                           - (e) RFC #768, Section 'Fields : Checksum' specifies that "the data [is] padded with zero
 *                       octets at the end (if necessary) to make a multiple of two octets".
 *
 *                       See also 'net_util.c  NetUtil_16BitSumDataCalc()  Note #8'.
 *
 *                           - (f) "If the computed checksum is zero" (i.e. one's-complement positive zero -- all
 *                       bits equal to zero), then "it is transmitted as all ones (the equivalent in
 *                       one's complement arithmetic" (i.e. one's-complement negative zero -- all bits
 *                       equal to one) [RFC #768, Section 'Fields : Checksum'].
 *
 *                           - (g) The UDP header Check-Sum field is returned in network-order & MUST NOT be re-
 *                       converted back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumDataCalc()
 *                       Note #4').
 *******************************************************************************************************/
static void NetUDP_TxPktPrepareHdr(NET_BUF       *p_buf,
                                   NET_BUF_HDR   *p_buf_hdr,
                                   void          *p_src_addr,
                                   NET_PORT_NBR  src_port,
                                   void          *p_dest_addr,
                                   NET_PORT_NBR  dest_port,
                                   NET_UDP_FLAGS flags_udp)
{
#ifndef  NET_UDP_CHK_SUM_OFFLOAD_TX
#ifdef  NET_IPv4_MODULE_EN
  NET_UDP_PSEUDO_HDR udp_pseudo_hdr;
  NET_IPv4_ADDR      *p_src_addrv4;
  NET_IPv4_ADDR      *p_dest_addrv4;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_IPv6_PSEUDO_HDR ipv6_pseudo_hdr;
#endif
#else
  PP_UNUSED_PARAM(p_src_addr);
  PP_UNUSED_PARAM(p_dest_addr);
#endif
  NET_UDP_HDR *p_udp_hdr;
  NET_CHK_SUM udp_chk_sum = 0u;
  CPU_BOOLEAN tx_chk_sum;

#if (NET_UDP_CFG_TX_CHK_SUM_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(flags_udp);
#endif

  //                                                               ----------------- UPDATE BUF CTRLS -----------------
  p_buf_hdr->TransportHdrLen = NET_UDP_HDR_SIZE;
  p_buf_hdr->TransportHdrIx = p_buf_hdr->DataIx - p_buf_hdr->TransportHdrLen;

  p_buf_hdr->TotLen += (NET_BUF_SIZE)p_buf_hdr->TransportHdrLen;
  p_buf_hdr->TransportTotLen = (CPU_INT16U)p_buf_hdr->TotLen;
  p_buf_hdr->TransportDataLen = (CPU_INT16U)p_buf_hdr->DataLen;

  //                                                               ----------------- PREPARE UDP HDR ------------------
  p_udp_hdr = (NET_UDP_HDR *)&p_buf->DataPtr[p_buf_hdr->TransportHdrIx];

  //                                                               ---------------- PREPARE UDP PORTS -----------------
  NET_UTIL_VAL_COPY_SET_NET_16(&p_udp_hdr->PortSrc, &src_port);
  NET_UTIL_VAL_COPY_SET_NET_16(&p_udp_hdr->PortDest, &dest_port);

  //                                                               ------------- PREPARE UDP DATAGRAM LEN -------------
  NET_UTIL_VAL_COPY_SET_NET_16(&p_udp_hdr->DatagramLen, &p_buf_hdr->TransportTotLen);

  //                                                               --------------- PREPARE UDP CHK SUM ----------------
#if (NET_UDP_CFG_TX_CHK_SUM_EN == DEF_ENABLED)
  tx_chk_sum = DEF_BIT_IS_CLR(flags_udp, NET_UDP_FLAG_TX_CHK_SUM_DIS);
#else
  tx_chk_sum = DEF_NO;
#endif

  if (tx_chk_sum == DEF_YES) {                                  // If en'd (see Note #3b), prepare UDP tx chk sum.
    NET_UTIL_VAL_SET_NET_16(&p_udp_hdr->ChkSum, 0x0000u);        // Clr UDP chk sum            (see Note #3c).
                                                                 // Cfg UDP chk sum pseudo-hdr (see Note #3d).
    if (DEF_BIT_IS_CLR(p_buf_hdr->Flags, NET_BUF_FLAG_IPv6_FRAME)) {
#ifdef  NET_IPv4_MODULE_EN
      //                                                           Calc UDP chk sum.
#ifndef NET_UDP_CHK_SUM_OFFLOAD_TX
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_UDP_V4;
      p_buf_hdr->ProtocolHdrTypeTransport = NET_PROTOCOL_TYPE_UDP_V4;

      p_src_addrv4 = (NET_IPv4_ADDR *)p_src_addr;
      p_dest_addrv4 = (NET_IPv4_ADDR *)p_dest_addr;
      udp_pseudo_hdr.AddrSrc = (NET_IPv4_ADDR)NET_UTIL_HOST_TO_NET_32(*p_src_addrv4);
      udp_pseudo_hdr.AddrDest = (NET_IPv4_ADDR)NET_UTIL_HOST_TO_NET_32(*p_dest_addrv4);
      udp_pseudo_hdr.Zero = (CPU_INT08U)0x00u;
      udp_pseudo_hdr.Protocol = (CPU_INT08U)NET_IP_HDR_PROTOCOL_UDP;
      udp_pseudo_hdr.DatagramLen = (CPU_INT16U)NET_UTIL_HOST_TO_NET_16(p_buf_hdr->TransportTotLen);

      udp_chk_sum = NetUtil_16BitOnesCplChkSumDataCalc(p_buf,
                                                       &udp_pseudo_hdr,
                                                       NET_UDP_PSEUDO_HDR_SIZE);
#endif
#endif
    } else {
#ifdef  NET_IPv6_MODULE_EN

#ifndef NET_UDP_CHK_SUM_OFFLOAD_TX
      Mem_Copy(&p_buf_hdr->IPv6_AddrSrc, p_src_addr, NET_IPv6_ADDR_SIZE);
      Mem_Copy(&p_buf_hdr->IPv6_AddrDest, p_dest_addr, NET_IPv6_ADDR_SIZE);

      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_UDP_V6;
      p_buf_hdr->ProtocolHdrTypeTransport = NET_PROTOCOL_TYPE_UDP_V6;

      ipv6_pseudo_hdr.AddrSrc = p_buf_hdr->IPv6_AddrSrc;
      ipv6_pseudo_hdr.AddrDest = p_buf_hdr->IPv6_AddrDest;
      ipv6_pseudo_hdr.UpperLayerPktLen = (CPU_INT32U)NET_UTIL_HOST_TO_NET_32(p_buf_hdr->TransportTotLen);
      ipv6_pseudo_hdr.Zero = (CPU_INT16U)0x00u;
      ipv6_pseudo_hdr.NextHdr = (CPU_INT32U)NET_UTIL_HOST_TO_NET_16(NET_IP_HDR_PROTOCOL_UDP);
      udp_chk_sum = NetUtil_16BitOnesCplChkSumDataCalc(p_buf,
                                                       &ipv6_pseudo_hdr,
                                                       NET_IPv6_PSEUDO_HDR_SIZE);
#endif
#endif
    }

    if (udp_chk_sum == NET_UDP_HDR_CHK_SUM_POS_ZERO) {          // If equal to one's-cpl pos zero, ...
      udp_chk_sum = NET_UDP_HDR_CHK_SUM_NEG_ZERO;               // ...  set to one's-cpl neg zero (see Note #3f).
    }
  } else {                                                      // Else tx NO chk sum.
    udp_chk_sum = NET_UTIL_HOST_TO_NET_16(NET_UDP_HDR_CHK_SUM_NONE);
  }

  NET_UTIL_VAL_COPY_16(&p_udp_hdr->ChkSum, &udp_chk_sum);        // Copy UDP chk sum in net order  (see Note #3g).
}

/****************************************************************************************************//**
 *                                           NetUDP_TxPktFree()
 *
 * @brief    Free network buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *
 * @note     (1) (a) Although UDP Transmit initially requests the network buffer for transmit, the UDP
 *                   layer does NOT maintain a reference to the buffer.
 *
 *                       - (b) Also, since the network interface transmit deallocation task frees ALL unreferenced
 *                   buffers after successful transmission, the UDP layer must NOT free the buffer.
 *
 *                   See also 'net_if.c  NetIF_TxDeallocTaskHandler()  Note #1c'.
 *******************************************************************************************************/
static void NetUDP_TxPktFree(NET_BUF *p_buf)
{
  PP_UNUSED_PARAM(p_buf);                                       // Prevent 'variable unused' warning (see Note #1).
}

/****************************************************************************************************//**
 *                                           NetUDP_TxPktDiscard()
 *
 * @brief    On any UDP transmit packet error(s), discard packet & buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *******************************************************************************************************/
static void NetUDP_TxPktDiscard(NET_BUF *p_buf)
{
  NET_CTR *p_ctr;

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  p_ctr = &Net_ErrCtrs.UDP.TxPktDiscardedCtr;
#else
  p_ctr = DEF_NULL;
#endif
  (void)NetBuf_FreeBuf(p_buf, p_ctr);
}

/****************************************************************************************************//**
 *                                           NetUDP_GetTxDataIx()
 *
 * @brief    Get the offset of a buffer at which the UDP data CAN be written.
 *
 * @param    if_nbr      Interface number on which data will be transmit.
 *
 * @param    protocol    Network protocol to use:
 *                       NET_PROTOCOL_TYPE_UDP_V4
 *                       NET_PROTOCOL_TYPE_UDP_V6
 *
 * @param    data_len    Data length to send
 *
 * @param    flags       Network flags
 *
 * @param    p_ix        Pointer to the current protocol index.
 *******************************************************************************************************/
static void NetUDP_GetTxDataIx(NET_IF_NBR        if_nbr,
                               NET_PROTOCOL_TYPE protocol,
                               CPU_INT16U        data_len,
                               NET_UDP_FLAGS     flags,
                               CPU_INT16U        *p_ix)
{
  NET_MTU mtu;

  *p_ix += NET_UDP_HDR_SIZE_MAX;

  switch (protocol) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_PROTOCOL_TYPE_UDP_V4:
      mtu = NetIF_MTU_GetProtocol(if_nbr, NET_PROTOCOL_TYPE_UDP_V4, NET_IF_FLAG_NONE);

      NetIPv4_TxIxDataGet(if_nbr,
                          data_len,
                          mtu,
                          p_ix);
      break;
#endif
#ifdef  NET_IPv6_MODULE_EN
    case NET_PROTOCOL_TYPE_UDP_V6:
      mtu = NetIF_MTU_GetProtocol(if_nbr, NET_PROTOCOL_TYPE_UDP_V6, NET_IF_FLAG_NONE);

      NetIPv6_GetTxDataIx(if_nbr,
                          DEF_NULL,
                          data_len,
                          mtu,
                          p_ix);
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  PP_UNUSED_PARAM(flags);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
