/***************************************************************************//**
 * @file
 * @brief Network Icmp V6 Layer - (Internet Control Message Protocol)
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

#include  <net/include/net_cfg_net.h>

#ifdef  NET_ICMPv6_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "net_icmpv6_priv.h"
#include  "net_ndp_priv.h"
#include  "net_mldp_priv.h"
#include  "net_if_priv.h"
#include  "net_if_ether_priv.h"
#include  "net_if_802x_priv.h"
#include  "net_icmp_priv.h"
#include  "net_ip_priv.h"
#include  "net_buf_priv.h"
#include  "net_stat_priv.h"
#include  "net_priv.h"
#include  "net_util_priv.h"

#include  <net/include/net_util.h>

#include  <common/include/lib_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 *                                           ICMPv6 FLAG DEFINES
 *
 * Notes(s) : (1) NET_ICMPv6_FLAG_USED indicates that the ICMPv6 entry is currently used and is not in
 *                   free pool.
 *******************************************************************************************************/

//                                                                 ----------------- NET ICMPv6 FLAGS -----------------
#define  NET_ICMPv6_FLAG_NONE                            DEF_BIT_NONE
#define  NET_ICMPv6_FLAG_USED                            DEF_BIT_00     // See Note #1.

/********************************************************************************************************
 *                                       ICMPv6 MESSAGE DEFINES
 *******************************************************************************************************/

#define  NET_ICMPv6_HDR_NBR_OCTETS_UNUSED                  4
#define  NET_ICMPv6_HDR_NBR_OCTETS_UNUSED_PARAM_PROB       3

#define  NET_ICMPv6_MSG_ERR_HDR_SIZE_MIN               NET_IPv6_HDR_SIZE
#define  NET_ICMPv6_MSG_ERR_HDR_SIZE_MAX               NET_IPv6_HDR_SIZE_MAX

#define  NET_ICMPv6_MSG_ERR_DATA_SIZE_MIN_BITS            64
#define  NET_ICMPv6_MSG_ERR_DATA_SIZE_MIN_OCTETS      (((NET_ICMPv6_MSG_ERR_DATA_SIZE_MIN_BITS - 1) / DEF_OCTET_NBR_BITS) + 1)

#define  NET_ICMPv6_MSG_ERR_LEN_MIN                     (NET_ICMPv6_MSG_ERR_HDR_SIZE_MIN  + NET_ICMPv6_MSG_ERR_DATA_SIZE_MIN_OCTETS)
#define  NET_ICMPv6_MSG_ERR_LEN_MAX                     (NET_ICMPv6_MSG_ERR_HDR_SIZE_MAX  + NET_ICMPv6_MSG_ERR_DATA_SIZE_MIN_OCTETS)

#define  NET_ICMPv6_MSG_LEN_MIN_DFLT                     NET_ICMPv6_HDR_SIZE_DFLT

#define  NET_ICMPv6_MSG_LEN_MIN_DEST_UNREACH            (NET_ICMPv6_HDR_SIZE_DEST_UNREACH + NET_ICMPv6_MSG_ERR_LEN_MIN)
#define  NET_ICMPv6_MSG_LEN_MIN_TIME_EXCEED             (NET_ICMPv6_HDR_SIZE_TIME_EXCEED  + NET_ICMPv6_MSG_ERR_LEN_MIN)
#define  NET_ICMPv6_MSG_LEN_MIN_PARAM_PROB              (NET_ICMPv6_HDR_SIZE_PARAM_PROB   + NET_ICMPv6_MSG_ERR_LEN_MIN)
#define  NET_ICMPv6_MSG_LEN_MIN_ECHO                     NET_ICMPv6_HDR_SIZE_ECHO

#define  NET_ICMPv6_MSG_LEN_MAX_NONE                     DEF_INT_16U_MAX_VAL

#define  NET_ICMPv6_MSG_LEN_MAX_DEST_UNREACH             NET_ICMPv6_MSG_LEN_MAX_NONE
#define  NET_ICMPv6_MSG_LEN_MAX_TIME_EXCEED              NET_ICMPv6_MSG_LEN_MAX_NONE
#define  NET_ICMPv6_MSG_LEN_MAX_PARAM_PROB               NET_ICMPv6_MSG_LEN_MAX_NONE
#define  NET_ICMPv6_MSG_LEN_MAX_ECHO                     NET_ICMPv6_MSG_LEN_MAX_NONE

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       ICMPv6 FLAGS DATA TYPE
 *******************************************************************************************************/

typedef  NET_FLAGS NET_ICMPv6_FLAGS;

/********************************************************************************************************
 *                                           ICMPv6 ERROR HEADER
 *
 * Note(s) : (1) See RFC #4443, Sections 'Destination Unreachable Message', 'Packet Too Big Message', 'Time
 *               Exceeded Message' and 'Parameter Problem Message' for ICMP 'Error Message' header format.
 *
 *           (2) 'Unused' field MUST be cleared (i.e. ALL 'Unused' field octets MUST be set to 0x00).
 *******************************************************************************************************/

//                                                                 ------------ NET ICMPv6 ERR HDR ------------
typedef  struct  net_ICMPv6_hdr_err {
  CPU_INT08U Type;                                                      // ICMPv6 msg type.
  CPU_INT08U Code;                                                      // ICMPv6 msg code.
  CPU_INT16U ChkSum;                                                    // ICMPv6 msg chk sum.

  CPU_INT08U Unused[NET_ICMPv6_HDR_NBR_OCTETS_UNUSED];                  // See Note #2.

  CPU_INT08U Data[NET_ICMPv6_MSG_ERR_LEN_MAX];
} NET_ICMPv6_HDR_ERR;

/********************************************************************************************************
 *                                   ICMPv6 PARAMETER PROBLEM HEADER
 *
 * Note(s) : (1) See RFC #4443, Section 3.4 'Parameter Problem Message' for ICMPv6 'Parameter Problem Message'
 *               header format.
 *******************************************************************************************************/

//                                                                 --------- NET ICMPv6 PARAM PROB HDR --------
typedef  struct  net_ICMPv6_hdr_param_prob {
  CPU_INT08U Type;                                                      // ICMPv6 msg type.
  CPU_INT08U Code;                                                      // ICMPv6 msg code.
  CPU_INT16U ChkSum;                                                    // ICMPv6 msg chk sum.
  CPU_INT32U Ptr;                                                       // Ptr into ICMPv6 err msg.

  CPU_INT08U Data[NET_ICMPv6_MSG_ERR_LEN_MAX];
} NET_ICMPv6_HDR_PARAM_PROB;

/********************************************************************************************************
 *                                   ICMPv6 ECHO REQUEST/REPLY HEADER
 *
 * Note(s) : (1) See RFC #4443, Section 'Echo or Echo Reply Message' for ICMP 'Echo Request/Reply Message'
 *               header format.
 *
 *           (2) 'Data' declared with 1 entry; prevents removal by compiler optimization.
 *******************************************************************************************************/

//                                                                 ------ NET ICMPv6 ECHO REQ/REPLY HDR -------
typedef  struct  net_ICMPv6_hdr_echo {
  CPU_INT08U Type;                                                      // ICMPv6 msg type.
  CPU_INT08U Code;                                                      // ICMPv6 msg code.
  CPU_INT16U ChkSum;                                                    // ICMPv6 msg chk sum.

  CPU_INT16U ID;                                                        // ICMPv6 msg ID.
  CPU_INT16U SeqNbr;                                                    // ICMPv6 seq nbr.

  CPU_INT08U Data[1];                                                   // ICMPv6 msg data (see Note #2).
} NET_ICMPv6_HDR_ECHO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT16U NetICMPv6_TxSeqNbrCtr;                          // Global tx seq nbr field ctr.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------- RX FNCTS -------------------
static void NetICMPv6_RxReplyDemux(NET_BUF        *p_buf,
                                   NET_BUF_HDR    *p_buf_hdr,
                                   NET_ICMPv6_HDR *p_icmp_hdr,
                                   RTOS_ERR       *p_err);

static NET_ICMP_MSG_TYPE NetICMPv6_RxPktValidate(NET_BUF        *p_buf,
                                                 NET_BUF_HDR    *p_buf_hdr,
                                                 NET_ICMPv6_HDR *p_icmp_hdr,
                                                 RTOS_ERR       *p_err);

static void NetICMPv6_RxPktFree(NET_BUF *p_buf);

//                                                                 ------------------- TX FNCTS -------------------
static void NetICMPv6_TxMsgErrValidate(NET_BUF      *p_buf,
                                       NET_BUF_HDR  *p_buf_hdr,
                                       NET_IPv6_HDR *p_ip_hdr,
                                       CPU_INT08U   type,
                                       CPU_INT08U   code,
                                       CPU_INT32U   ptr,
                                       RTOS_ERR     *p_err);

static void NetICMPv6_TxMsgReqValidate(CPU_INT08U type,
                                       CPU_INT08U code,
                                       RTOS_ERR   *p_err);

static void NetICMPv6_TxPktFree(NET_BUF *p_buf);

static void NetICMPv6_TxPktDiscard(NET_BUF *p_buf);

//                                                                 ---------------- HANDLER FNCTS -----------------
static void NetICMPv6_GetTxDataIx(NET_IF_NBR       if_nbr,
                                  CPU_INT32U       data_len,
                                  NET_IPv6_EXT_HDR *p_ext_hdr_list,
                                  CPU_INT16U       *p_ix,
                                  RTOS_ERR         *p_err);

static void NetICMPv6_CpyData(NET_BUF    *p_buf_src,
                              NET_BUF    *p_buf_dst,
                              CPU_INT32U msg_len,
                              RTOS_ERR   *p_err);

static void NetICMPv6_CpyDataToBuf(CPU_INT08U *p_data,
                                   NET_BUF    *p_buf_dest,
                                   CPU_INT32U msg_len,
                                   RTOS_ERR   *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetICMPv6_Init()
 *
 * @brief    (1) Initialize Internet Control Message Protocol Layer V6 :
 *
 *           - (a) Initialize ICMP transmit sequence number counter
 *******************************************************************************************************/
void NetICMPv6_Init(void)
{
  //                                                               ----------- INIT ICMPv6 TX SEQ NBR CTR -----------
  NetICMPv6_TxSeqNbrCtr = 0u;
}

/****************************************************************************************************//**
 *                                               NetICMPv6_Rx()
 *
 * @brief    (1) Process received messages :
 *               - (a) Validate ICMPv6 packet
 *               - (b) Demultiplex ICMPv6 message
 *               - (c) Free ICMPv6 packet
 *               - (d) Update receive statistics
 *           - (2) Although ICMPv6 data units are typically referred to as 'messages' (see RFC #792, Section
 *                 'Introduction'), the term 'ICMP packet' (see RFC #1983, 'packet') is used for ICMP
 *                 Receive until the packet is validated as an ICMPv6 message.
 *
 * @param    p_buf   Pointer to network buffer that received ICMPv6 packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (3) ICMPv6 Receive Error/Reply Messages NOT yet implemented. #### NET-796
 *               - (a) Define "User Process" to report ICMPv6 Error Messages to Transport &/or Application Layers.
 *               - (b) Define procedure to demultiplex & enqueue ICMPv6 Reply Messages to Application.
 *                   - (1) MUST implement mechanism to de-queue ICMPv6 message data from single, complete
 *                         datagram packet buffers or multiple, fragmented packet buffers.
 *
 * @note     (4) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
void NetICMPv6_Rx(NET_BUF  *p_buf,
                  RTOS_ERR *p_err)
{
  NET_BUF_HDR       *p_buf_hdr;
  NET_ICMPv6_HDR    *p_icmp_hdr;
  NET_ICMP_MSG_TYPE msg_type;

  p_buf_hdr = &p_buf->Hdr;

  NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.RxMsgCtr);

  //                                                               Validate rx'd pkt.
  p_icmp_hdr = (NET_ICMPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->ICMP_MsgIx];

  msg_type = NetICMPv6_RxPktValidate(p_buf, p_buf_hdr, p_icmp_hdr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               ------------------ DEMUX ICMP MSG ------------------
  switch (msg_type) {
    case NET_ICMP_MSG_TYPE_REQ:
      NetICMPv6_TxMsgReply(p_buf, p_buf_hdr, p_icmp_hdr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_discard;
      }
      NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.RxMsgReqCtr);
      break;

    case NET_ICMP_MSG_TYPE_REPLY:
      NetICMPv6_RxReplyDemux(p_buf, p_buf_hdr, p_icmp_hdr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_discard;
      }
      NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.RxMsgReplyCtr);
      break;

    case NET_ICMP_MSG_TYPE_NDP:
      NetNDP_Rx(p_buf, p_buf_hdr, p_icmp_hdr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_discard;
      }
      NET_CTR_STAT_INC(Net_StatCtrs.NDP.RxMsgCtr);
      break;

    case NET_ICMP_MSG_TYPE_MLDP:
      NetMLDP_Rx(p_buf, p_buf_hdr, (NET_MLDP_V1_HDR *)p_icmp_hdr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_discard;
      }
      NET_CTR_STAT_INC(Net_StatCtrs.MLDP.RxMsgCtr);
      break;

    case NET_ICMP_MSG_TYPE_ERR:
      NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.RxMsgErrCtr);
      break;

    default:
      NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.RxMsgUnknownCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      goto exit_discard;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.RxMsgCompCtr);

  NetICMPv6_RxPktFree(p_buf);

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxPktDiscardedCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetICMPv6_RxReplyDemux()
 *
 * @brief    Demultiplex received ICMPv6 reply message.
 *
 * @param    p_buf       Pointer to network buffer that received ICMPv6 packet.
 *
 * @param    ----        Argument checked in caller(s).
 *
 * @param    p_buf_hdr   Pointer to network buffer header that received ICMPv6 packet.
 *
 * @param    --------    Argument checked in caller(s).
 *
 * @param    p_icmp_hdr  Pointer to received packet's ICMPv6 header.
 *
 * @param    ---------   Argument checked in caller(s).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetICMPv6_RxReplyDemux(NET_BUF        *p_buf,
                                   NET_BUF_HDR    *p_buf_hdr,
                                   NET_ICMPv6_HDR *p_icmp_hdr,
                                   RTOS_ERR       *p_err)
{
  NET_ICMPv6_HDR_ECHO *p_icmp_echo_hdr;
  CPU_INT16U          id;
  CPU_INT16U          seq;
  CPU_INT16U          data_len;

  switch (p_icmp_hdr->Type) {
    case NET_ICMPv6_MSG_TYPE_ECHO_REPLY:
      p_icmp_echo_hdr = (NET_ICMPv6_HDR_ECHO *)p_icmp_hdr;
      data_len = p_buf_hdr->ICMP_MsgLen - p_buf_hdr->ICMP_HdrLen;
      id = NET_UTIL_NET_TO_HOST_16(p_icmp_echo_hdr->ID);
      seq = NET_UTIL_NET_TO_HOST_16(p_icmp_echo_hdr->SeqNbr);

      NetICMP_RxEchoReply(id,
                          seq,
                          p_icmp_echo_hdr->Data,
                          data_len,
                          p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxInvEchoReplyCtr);
        goto exit;
      }
      break;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxInvTypeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  PP_UNUSED_PARAM(p_buf);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetICMPv6_TxEchoReq()
 *
 * @brief    Transmit ICMPv6 echo request message.
 *
 * @param    p_addr_dest     Pointer to IPv6 destination address to send the ICMP echo request.
 *
 * @param    timeout_ms      Timeout value :
 *                           NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
 *                           In number of milliseconds, otherwise.
 *
 * @param    p_data          Pointer to the data buffer        to include in     the ICMP echo request.
 *
 * @param    data_len        Number of      data buffer octets to include in     the ICMP echo request.
 *
 * @param    task_id         Task ID that will be send as                        the ICMP echo request ID.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if ICMPv6 echo reply message successfully received from remote host.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_INT16U NetICMPv6_TxEchoReq(NET_IPv6_ADDR *p_addr_dest,
                               CPU_INT16U    id,
                               void          *p_data,
                               CPU_INT16U    data_len,
                               RTOS_ERR      *p_err)
{
  NET_ICMPv6_REQ_ID_SEQ req_id;

  req_id.ID = NET_ICMPv6_REQ_ID_NONE;
  req_id.SeqNbr = NET_ICMPv6_REQ_SEQ_NONE;

  RTOS_ASSERT_DBG_ERR_SET((p_addr_dest != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_ICMPv6_REQ_SEQ_NONE);
  RTOS_ASSERT_DBG_ERR_SET(((p_data != DEF_NULL) || (data_len <= 0)), *p_err, RTOS_ERR_NULL_PTR, NET_ICMPv6_REQ_SEQ_NONE);

  //                                                               ------------------ TX ICMPv6 REQ -------------------
  req_id = NetICMPv6_TxMsgReq(NET_IF_NBR_NONE,
                              NET_ICMPv6_MSG_TYPE_ECHO_REQ,
                              NET_ICMPv6_MSG_CODE_ECHO_REQ,
                              id,
                              DEF_NULL,
                              p_addr_dest,
                              NET_IPv6_HDR_HOP_LIM_MAX,
                              DEF_NULL,
                              p_data,
                              data_len,
                              p_err);

  return (req_id.SeqNbr);
}

/****************************************************************************************************//**
 *                                           NetICMPv6_TxMsgErr()
 *
 * @brief    (1) Transmit ICMPv6 Error Message in response to received packet with one or more errors :
 *               - (a) Validate ICMPv6 Error Message.
 *               - (b) Get buffer for ICMPv6 Error Message :
 *                   - (1) Calculate  ICMPv6 Error Message buffer size
 *                   - (2) Copy received packet's IP header & data into ICMPv6 Error Message
 *                   - (3) Initialize ICMPv6 Error Message buffer controls
 *               - (c) Prepare ICMPv6 Error Message :
 *                   - (1) Type                           See 'net_icmp.h  ICMP MESSAGE TYPES & CODES'
 *                   - (2) Code                           See 'net_icmp.h  ICMP MESSAGE TYPES & CODES'
 *                   - (3) Pointer
 *                   - (4) Unused
 *                   - (5) Check-Sum
 *               - (d) Transmit ICMPv6 Error Message.
 *               - (e) Free ICMPv6 Error Message buffer.
 *               - (f) Update transmit statistics.
 *
 * @param    p_buf   Pointer to network buffer that received a packet with error(s).
 *
 * @param    ----    Argument checked in caller(s).
 *
 * @param    type    ICMPv6 Error Message type (see Note #1c1) :
 *                       - NET_ICMPv6_MSG_TYPE_DEST_UNREACH
 *                       - NET_ICMPv6_MSG_TYPE_PKT_TOO_BIG
 *                       - NET_ICMPv6_MSG_TYPE_TIME_EXCEED
 *                       - NET_ICMPv6_MSG_TYPE_PARAM_PROB
 *
 * @param    code    ICMPv6 Error Message code (see Note #1c2).
 *
 * @param    ptr     Pointer to received packet's ICMPv6 error (optional).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Default case already invalidated in NetICMPv6_TxMsgErrValidate().  However, the default
 *               case is included as an extra precaution in case 'type' is incorrectly modified.
 *
 * @note     (3) Assumes network buffer's protocol header size is large enough to accommodate ICMP header
 *               size (see 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1').
 *
 * @note     (4) Some buffer controls were previously initialized in NetBuf_Get() when the buffer was
 *               allocated.  These buffer controls do NOT need to be re-initialized but are shown for
 *               completeness.
 *
 * @note     (5) (a) ICMPv6 message Check-Sum MUST be calculated AFTER the entire ICMPv6 message has been
 *               prepared.  In addition, ALL multi-octet words are converted from host-order to
 *               network-order since "the sum of 16-bit integers can be computed in either byte
 *               order" [RFC #1071, Section 2.(B)].
 *               - (b) ICMPv6 message Check-Sum field MUST be cleared to '0' BEFORE the ICMP message Check-Sum
 *                     is calculated.
 *               - (c) The ICMPv6 message Check-Sum field is returned in network-order & MUST NOT be re-converted
 *                     back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()  Note #3b').
 *
 * @note     (6) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
void NetICMPv6_TxMsgErr(NET_BUF    *p_buf,
                        CPU_INT08U type,
                        CPU_INT08U code,
                        CPU_INT32U ptr,
                        RTOS_ERR   *p_err)
{
  NET_BUF_HDR               *p_buf_hdr = DEF_NULL;
  NET_IPv6_HDR              *p_ip_hdr = DEF_NULL;
  NET_BUF                   *p_msg_err = DEF_NULL;
  NET_BUF_HDR               *p_msg_err_hdr = DEF_NULL;
  NET_ICMPv6_HDR_ERR        *p_icmp_hdr_err = DEF_NULL;
  NET_ICMPv6_HDR_PARAM_PROB *p_icmp_hdr_param_prob = DEF_NULL;
  NET_IPv6_PSEUDO_HDR       pseudo_hdr;
  NET_MTU                   icmp_pld_maxlen;
  CPU_INT16U                msg_size_hdr;
  CPU_INT16U                msg_size_data;
  CPU_INT16U                msg_size_tot;
  CPU_INT16U                msg_ix;
  CPU_INT16U                msg_ix_offset;
  CPU_INT16U                msg_ix_data;
  CPU_INT16U                msg_chk_sum;
  CPU_INT16U                data_ix;

  p_buf_hdr = &p_buf->Hdr;

  //                                                               ------------ VALIDATE ICMPv6 TX ERR MSG ------------
  p_ip_hdr = (NET_IPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];

  NetICMPv6_TxMsgErrValidate(p_buf, p_buf_hdr, p_ip_hdr, type, code, ptr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               -------------- GET ICMPv6 ERR MSG BUF --------------
  //                                                               Calc err msg buf size.
  switch (type) {
    case NET_ICMPv6_MSG_TYPE_DEST_UNREACH:
      msg_size_hdr = NET_ICMPv6_HDR_SIZE_DEST_UNREACH;
      break;

    case NET_ICMPv6_MSG_TYPE_TIME_EXCEED:
      msg_size_hdr = NET_ICMPv6_HDR_SIZE_TIME_EXCEED;
      break;

    case NET_ICMPv6_MSG_TYPE_PARAM_PROB:
      msg_size_hdr = NET_ICMPv6_HDR_SIZE_PARAM_PROB;
      break;

    default:                                                    // See Note #3.
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxHdrTypeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      goto exit_discard;
  }

  //                                                               Get maximum payload length of ICMPv6 message
  icmp_pld_maxlen = NET_IPv6_MAX_DATAGRAM_SIZE_DFLT - p_buf_hdr->IP_HdrLen - msg_size_hdr;
  //                                                               Get message data lenght of ICMPv6 message
  if ((p_buf_hdr->IP_TotLen + p_buf_hdr->IP_HdrLen) > icmp_pld_maxlen) {
    msg_size_data = icmp_pld_maxlen;
  } else {
    msg_size_data = p_buf_hdr->IP_TotLen + p_buf_hdr->IP_HdrLen;
  }

  msg_size_tot = msg_size_hdr + msg_size_data;

  data_ix = msg_size_hdr;

  NetICMPv6_GetTxDataIx(p_buf_hdr->IF_Nbr,
                        msg_size_data,
                        DEF_NULL,
                        &data_ix,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               Get err msg buf.
  msg_ix = data_ix - msg_size_hdr;
  p_msg_err = NetBuf_Get(p_buf_hdr->IF_Nbr,
                         NET_TRANSACTION_TX,
                         msg_size_tot,
                         msg_ix,
                         &msg_ix_offset,
                         NET_BUF_FLAG_NONE,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  msg_ix += msg_ix_offset;
  msg_ix_data = msg_ix + msg_size_hdr;
  NetBuf_DataWr(p_msg_err,                                      // Copy rx'd IPv6 hdr & data into ICMPv6 err tx buf.
                msg_ix_data,
                msg_size_data,
                (CPU_INT08U *) p_ip_hdr);

  //                                                               Init err msg buf ctrls.
  p_msg_err_hdr = &p_msg_err->Hdr;
  p_msg_err_hdr->ICMP_MsgIx = (CPU_INT16U)msg_ix;
  p_msg_err_hdr->ICMP_MsgLen = (CPU_INT16U)msg_size_tot;
  p_msg_err_hdr->ICMP_HdrLen = (CPU_INT16U)msg_size_hdr;
  p_msg_err_hdr->TotLen = (NET_BUF_SIZE)p_msg_err_hdr->ICMP_MsgLen;
  p_msg_err_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_ICMP_V6;
  p_msg_err_hdr->ProtocolHdrTypeNetSub = NET_PROTOCOL_TYPE_ICMP_V6;

  //                                                               -------------- PREPARE ICMPv6 ERR MSG --------------
  switch (type) {
    case NET_ICMPv6_MSG_TYPE_DEST_UNREACH:
    case NET_ICMPv6_MSG_TYPE_TIME_EXCEED:
      p_icmp_hdr_err = (NET_ICMPv6_HDR_ERR *)&p_msg_err->DataPtr[p_msg_err_hdr->ICMP_MsgIx];
      p_icmp_hdr_err->Type = type;
      p_icmp_hdr_err->Code = code;
      Mem_Clr(&p_icmp_hdr_err->Unused, sizeof(NET_ICMPv6_HDR_NBR_OCTETS_UNUSED));
      //                                                           Calc msg chk sum (see Note #6).
      //                                                           Prepare IPv6 pseudo-hdr.
      Mem_Clr(&pseudo_hdr, sizeof(NET_IPv6_PSEUDO_HDR));
      pseudo_hdr.AddrSrc = p_buf_hdr->IPv6_AddrDest;
      pseudo_hdr.AddrDest = p_buf_hdr->IPv6_AddrSrc;
      pseudo_hdr.UpperLayerPktLen = NET_UTIL_HOST_TO_NET_32(p_msg_err_hdr->ICMP_MsgLen);
      pseudo_hdr.NextHdr = NET_UTIL_HOST_TO_NET_16(NET_IP_HDR_PROTOCOL_ICMPv6);

      msg_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc(&pseudo_hdr,
                                                      sizeof(NET_IPv6_PSEUDO_HDR));

      p_icmp_hdr_err->ChkSum = ~(msg_chk_sum);                               // Copy chk sum in ICMPv6 chk sum field.

      msg_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc(p_icmp_hdr_err,
                                                      p_msg_err_hdr->ICMP_MsgLen);

      p_icmp_hdr_err->ChkSum = msg_chk_sum;
      break;

    case NET_ICMPv6_MSG_TYPE_PARAM_PROB:
      p_icmp_hdr_param_prob = (NET_ICMPv6_HDR_PARAM_PROB *)&p_msg_err->DataPtr[p_msg_err_hdr->ICMP_MsgIx];
      p_icmp_hdr_param_prob->Type = type;
      p_icmp_hdr_param_prob->Code = code;
      p_icmp_hdr_param_prob->Ptr = NET_UTIL_HOST_TO_NET_32(ptr);

      //                                                           Calc msg chk sum (see Note #6).
      //                                                           Prepare IPv6 pseudo-hdr.
      Mem_Clr(&pseudo_hdr, sizeof(NET_IPv6_PSEUDO_HDR));
      pseudo_hdr.AddrSrc = p_buf_hdr->IPv6_AddrDest;
      pseudo_hdr.AddrDest = p_buf_hdr->IPv6_AddrSrc;
      pseudo_hdr.UpperLayerPktLen = NET_UTIL_HOST_TO_NET_32(p_msg_err_hdr->ICMP_MsgLen);
      pseudo_hdr.NextHdr = NET_UTIL_HOST_TO_NET_16(NET_IP_HDR_PROTOCOL_ICMPv6);

      msg_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc(&pseudo_hdr,
                                                      sizeof(NET_IPv6_PSEUDO_HDR));

      p_icmp_hdr_param_prob->ChkSum = ~(msg_chk_sum);                        // Copy chk sum in ICMPv6 chk sum field.

      msg_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc(p_icmp_hdr_param_prob,
                                                      p_msg_err_hdr->ICMP_MsgLen);

      //                                                           Copy chk sum in net order (see Note #6c).
      p_icmp_hdr_param_prob->ChkSum = msg_chk_sum;
      break;

    default:                                                    // See Note #3.
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxHdrTypeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      goto exit_discard;
  }

  //                                                               ---------------- TX ICMPv6 ERR MSG -----------------
  NetIPv6_Tx(p_msg_err,
             &p_buf_hdr->IPv6_AddrDest,
             &p_buf_hdr->IPv6_AddrSrc,
             DEF_NULL,
             NET_IPv6_HDR_TRAFFIC_CLASS,
             NET_IPv6_HDR_FLOW_LABEL,
             NET_IPv6_HOP_LIM_DFLT,
             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  NetICMPv6_TxPktFree(p_msg_err);

  NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.TxMsgCtr);
  NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.TxMsgErrCtr);

  goto exit;

exit_discard:
  NetICMPv6_TxPktDiscard(p_msg_err);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetICMPv6_TxMsgReply()
 *
 * @brief    (1) Transmit ICMPv6 Reply Message in response to received ICMPv6 Request Message :
 *               - (a) Process ICMPv6 Reply Message.
 *               - (b) Get buffer for ICMPv6 Reply Message :
 *                   - (1) Copy ICMPv6 Request Message into ICMPv6 Reply Message
 *                   - (2) Initialize ICMPv6 Reply Message buffer controls
 *               - (c) Prepare ICMPv6 Reply Message's IPv6 header :
 *                   - (1) RFC #1349, Section 5.1 specifies that "an ICMP reply message is sent with the same
 *                         value in the TOS field as was used in the corresponding ICMP request message".
 *                   - (2) RFC #1122, Sections 3.2.2.6 & 3.2.2.8 specify that "if a Record Route and/or Time
 *                         Stamp option is received in [an ICMP Request, these options] SHOULD be updated ...
 *                         and included in the IP header of the ... Reply message".  Also "if a Source Route
 *                         option is received ... the return route MUST be reversed and used as a Source Route
 *                         option for the ... Reply message".
 *                         #### These IPv6 header option requirements for ICMPv6 Reply Messages are NOT yet implemented.
 *               - (d) Prepare ICMPv6 Reply Message :
 *                   - (1) Echo Reply Message
 *                       - (A) "To form an echo reply message, the source and destination addresses are simply
 *                             reversed, the type code changed to [reply], and the checksum recomputed"
 *                             (RFC #792, Section 'Echo or Echo Reply Message : Addresses').
 *                   - (2) NDP Neighbor Advertisement Reply Message
 *                       - (A) "To form an echo reply message, the source and destination addresses are simply
 *                             reversed, the type code changed to [reply], and the checksum recomputed".
 *                   - (3) Some ICMPv6 Reply Message fields are copied directly from the ICMPv6 Request Message.
 *                       - (A) ICMPv6 Reply Message Identification & Sequence Number fields were NOT validated
 *                             or converted from network-order to host-order (see 'NetICMPv6_RxPktValidate()
 *                             Notes #1b2C & #1b2D') & therefore do NOT need to be converted from host-order
 *                             to network-order.
 *               - (e) Transmit ICMPv6 Reply Message.
 *               - (f) Free     ICMPv6 Reply Message buffer.
 *               - (g) Update transmit statistics.
 *
 * @param    p_buf       Pointer to network buffer that received ICMP packet.
 *
 * @param    ----        Argument checked   in NetICMPv6_Rx().
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    --------    Argument validated in NetICMPv6_Rx().
 *
 * @param    p_icmp_hdr  Pointer to received packet's ICMP header.
 *
 * @param    ---------   Argument validated in NetICMPv6_Rx().
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Default case already invalidated in NetICMPv6_RxPktValidate().  However, the default case
 *               is included as an extra precaution in case 'Type' is incorrectly modified.
 *
 * @note     (3) (a) RFC #1122, Section 3.2.2.6 states that "data received in an ICMP Echo Request MUST
 *                   be entirely included in the resulting Echo Reply.  However, if sending the Echo Reply
 *                   requires intentional fragmentation that is not implemented, the datagram MUST be
 *                   truncated to maximum transmission size ... and sent".
 *
 *                   See also 'net_IPv6.h  Note #1c'.
 *
 *                       - (b) In case the maximum network buffer size is smaller than the ICMPv6 message data length,
 *                   the ICMPv6 Echo Request Message data should be similarly truncated in order to transmit
 *                   the ICMPv6 Echo Reply Message.
 *
 * @note     (4) Some buffer controls were previously initialized in NetBuf_Get() when the buffer
 *               was allocated earlier in this function. These buffer controls do NOT need to be
 *               re-initialized but are shown for completeness.
 *
 * @note     (5) (a) ICMPv6 message Check-Sum MUST be calculated AFTER the entire ICMP message has been
 *                   prepared.  In addition, ALL multi-octet words are converted from host-order to
 *                   network-order since "the sum of 16-bit integers can be computed in either byte
 *                   order" [RFC #1071, Section 2.(B)].
 *                   - (b) ICMPv6 message Check-Sum field MUST be cleared to '0' BEFORE the ICMPv6 message
 *                         Check-Sum is calculated.
 *                   - (c) The ICMPv6 message Check-Sum field is returned in network-order & MUST NOT be re-
 *                         converted back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()
 *                         Note #3b' & 'net_util.c  NetUtil_16BitOnesCplChkSumDataCalc()  Note #4b').
 *
 * @note     (6) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
void NetICMPv6_TxMsgReply(NET_BUF        *p_buf,
                          NET_BUF_HDR    *p_buf_hdr,
                          NET_ICMPv6_HDR *p_icmp_hdr,
                          RTOS_ERR       *p_err)
{
  CPU_INT08U               *p_mem;
  NET_ICMPv6_HDR_ECHO      *p_icmp_hdr_echo;
  NET_NDP_NEIGHBOR_ADV_HDR *p_icmp_hdr_neighbor_adv;
  CPU_INT08U               hw_addr[NET_IF_HW_ADDR_LEN_MAX];
  CPU_INT08U               hw_addr_len;
  NET_BUF                  *p_msg_reply = DEF_NULL;
  NET_BUF                  *p_msg_reply_head = DEF_NULL;
  NET_BUF                  *p_chain_buf;
  NET_BUF                  *p_chain_buf_next;
  NET_BUF_HDR              *p_msg_req_hdr;
  NET_BUF_HDR              *p_msg_reply_hdr = DEF_NULL;
  NET_BUF_HDR              *p_chain_buf_hdr;
  NET_IPv6_HOP_LIM         hop_limit;
  NET_BUF_SIZE             buf_size_max;
  NET_BUF_SIZE             buf_size_max_data;
  NET_MTU                  icmp_mtu;
  CPU_INT16U               msg_size_hdr;
  CPU_INT16U               msg_len;
  CPU_INT16U               msg_len_min;
  CPU_INT16U               msg_ix;
  CPU_INT16U               msg_ix_offset;
  CPU_INT16U               msg_chk_sum;
  CPU_INT32U               flags;
  NET_IF_NBR               if_nbr;
  const NET_IPv6_ADDR_OBJ  *p_ipv6_addrs;
  NET_IPv6_ADDR            ipv6_addr_src;
  NET_IPv6_ADDR            ipv6_addr_dest;
  NET_IPv6_PSEUDO_HDR      ipv6_pseudo_hdr;
  NET_IPv6_EXT_HDR         frag_hdr;
  NET_IPv6_EXT_HDR         *p_ext_hdr_head;
  CPU_INT16U               data_ix;
  CPU_INT32U               payload_size;
  CPU_INT32S               req_len;
  CPU_BOOLEAN              frag_used;
  CPU_BOOLEAN              addr_unspecified;
  CPU_BOOLEAN              addr_is_mcast;

  flags = 0u;
  //                                                               ------------ PROCESS ICMPv6 REPLY MSGs -------------
  switch (p_icmp_hdr->Type) {
    case NET_ICMPv6_MSG_TYPE_ECHO_REQ:
      msg_size_hdr = NET_ICMPv6_HDR_SIZE_ECHO;
      msg_len_min = NET_ICMPv6_MSG_LEN_MIN_ECHO;
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL:
      msg_size_hdr = NET_NDP_HDR_SIZE_NEIGHBOR_SOL;
      msg_len_min = NET_NDP_MSG_LEN_MIN_NEIGHBOR_SOL;
      break;

    default:                                                    // See Note #2.
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxHdrTypeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit_discard;
  }

  //                                                               ------------- GET ICMPv6 REPLY MSG BUF -------------
  msg_len = p_buf_hdr->ICMP_MsgLen;                             // Adj req msg len for reply msg len.

  //                                                               Get IF's ICMPv6 MTU.
  frag_used = DEF_NO;
  icmp_mtu = NetIF_MTU_GetProtocol(p_buf_hdr->IF_Nbr, NET_PROTOCOL_TYPE_ICMP_V6, NET_IF_FLAG_NONE);

  //                                                               Check if fragmentation is needed.
  p_ext_hdr_head = DEF_NULL;
  if (msg_len > icmp_mtu) {
    icmp_mtu -= NET_IPv6_FRAG_HDR_SIZE;
    icmp_mtu -= (icmp_mtu % NET_IPv6_FRAG_SIZE_UNIT);
    frag_used = DEF_YES;
    p_ext_hdr_head = NetIPv6_ExtHdrAddToList(DEF_NULL,
                                             &frag_hdr,
                                             NET_IP_HDR_PROTOCOL_EXT_FRAG,
                                             NET_IPv6_FRAG_HDR_SIZE,
                                             NetIPv6_PrepareFragHdr,
                                             NET_IPv6_EXT_HDR_KEY_FRAG,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_discard;
    }
  }

  data_ix = msg_size_hdr;
  payload_size = msg_len;

  NetICMPv6_GetTxDataIx(p_buf_hdr->IF_Nbr,
                        msg_len,
                        p_ext_hdr_head,
                        &data_ix,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  msg_ix = (data_ix - msg_size_hdr);
  buf_size_max = NetBuf_GetMaxSize(p_buf_hdr->IF_Nbr,
                                   NET_TRANSACTION_TX,
                                   DEF_NULL,
                                   msg_ix);

  buf_size_max_data = DEF_MIN(buf_size_max, icmp_mtu);

  if (frag_used == DEF_YES) {
    buf_size_max_data -= (buf_size_max_data % NET_IPv6_FRAG_SIZE_UNIT);
  }

  if (msg_len > buf_size_max_data) {                            // If msg len > max data size, ...
    if (p_icmp_hdr->Type != NET_ICMPv6_MSG_TYPE_ECHO_REQ) {     // ... for Echo Req Msg ONLY,  ...
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxInvalidLenCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit_discard;
    }
  }

  if (msg_len < msg_len_min) {                                  // If msg len < min msg len, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxInvalidLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit_discard;
  }

  //                                                               Get reply msg buf.
  p_msg_reply_head = DEF_NULL;

  while (msg_len > 0) {
    req_len = (CPU_INT32S)DEF_MIN(msg_len, buf_size_max_data);

    p_msg_reply = NetBuf_Get(p_buf_hdr->IF_Nbr,
                             NET_TRANSACTION_TX,
                             (NET_BUF_SIZE) req_len,
                             (NET_BUF_SIZE) msg_ix,
                             (NET_BUF_SIZE *)&msg_ix_offset,
                             NET_BUF_FLAG_NONE,
                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      p_msg_reply_head = p_msg_reply;
      goto exit_discard;
    }

    msg_ix += msg_ix_offset;
    //                                                             If buf is 1st aquired, set p_msg_reply_head to it ...
    if (p_msg_reply_head == DEF_NULL) {
      p_msg_reply_head = p_msg_reply;
    } else {                                                    // ... else add it to the end NextBufPtr chain of ...
                                                                // ... p_msg_reply_head.
      p_chain_buf = p_msg_reply_head;
      p_chain_buf_hdr = &p_msg_reply_head->Hdr;

      while (p_chain_buf_hdr->NextBufPtr != DEF_NULL) {
        p_chain_buf = p_chain_buf_hdr->NextBufPtr;
        p_chain_buf_hdr = &p_chain_buf->Hdr;
      }

      p_chain_buf_hdr->NextBufPtr = p_msg_reply;
      p_chain_buf_hdr = &p_msg_reply->Hdr;
      p_chain_buf_hdr->PrevBufPtr = p_chain_buf;
    }

    //                                                             Init reply msg buf ctrls.
    p_msg_reply_hdr = &p_msg_reply->Hdr;
    p_msg_reply_hdr->ICMP_MsgIx = (CPU_INT16U) msg_ix;
    p_msg_reply_hdr->ICMP_MsgLen = (CPU_INT16U) req_len;
    p_msg_reply_hdr->ICMP_HdrLen = (CPU_INT16U) msg_size_hdr;
    p_msg_reply_hdr->DataLen = (CPU_INT16U)(req_len - msg_size_hdr);
    p_msg_reply_hdr->TotLen = (NET_BUF_SIZE)p_msg_reply_hdr->ICMP_MsgLen;
    p_msg_reply_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_ICMP_V6;
    p_msg_reply_hdr->ProtocolHdrTypeNetSub = NET_PROTOCOL_TYPE_ICMP_V6;

    msg_len -= req_len;
    msg_size_hdr = 0;
    buf_size_max_data = (NET_BUF_SIZE)DEF_MIN(buf_size_max, icmp_mtu);
  }

  p_msg_req_hdr = &p_buf->Hdr;

  NetICMPv6_CpyData(p_buf, p_msg_reply_head, payload_size, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  p_buf_hdr = &p_buf->Hdr;
  //                                                               ------------- PREPARE IPv6 PSEUDO HDR --------------
  ipv6_pseudo_hdr.UpperLayerPktLen = (CPU_INT32U)NET_UTIL_NET_TO_HOST_32(p_buf_hdr->ICMP_MsgLen);
  ipv6_pseudo_hdr.Zero = (CPU_INT16U)0x00u;
  ipv6_pseudo_hdr.NextHdr = (CPU_INT32U)NET_UTIL_NET_TO_HOST_16(NET_IP_HDR_PROTOCOL_ICMPv6);

  //                                                               ------------- PREPARE ICMPv6 REPLY MSG -------------
  switch (p_icmp_hdr->Type) {
    case NET_ICMPv6_MSG_TYPE_ECHO_REQ:                          // See Note #1d1.
      p_icmp_hdr_echo = (NET_ICMPv6_HDR_ECHO *)&p_msg_reply_head->DataPtr[p_msg_reply_hdr->ICMP_MsgIx];
      p_icmp_hdr_echo->Type = NET_ICMPv6_MSG_TYPE_ECHO_REPLY;
      hop_limit = NET_IPv6_HDR_HOP_LIM_DFLT;
#if 0                                                           // Copied from ICMPv6 req msg (see Note #1d3).
      p_icmp_hdr_echo->Code = NET_ICMP_MSG_CODE_ECHO;
      //                                                           See Note #1d3A.
      PP_UNUSED_PARAM(p_icmp_hdr_echo->ID);
      PP_UNUSED_PARAM(p_icmp_hdr_echo->SeqNbr);
#endif
      //                                                           Calc ICMP msg data len.
      if (p_buf_hdr->ICMP_MsgLen > p_msg_reply_hdr->ICMP_HdrLen) {
        p_msg_reply_hdr->DataIx = (CPU_INT16U)(p_msg_reply_hdr->ICMP_MsgIx  + msg_len_min);
        p_msg_reply_hdr->DataLen = (NET_BUF_SIZE)(p_msg_reply_hdr->ICMP_MsgLen - p_msg_reply_hdr->ICMP_HdrLen);
#if 0                                                           // Init'd in NetBuf_Get() [see Note #4].
      } else {
        p_msg_reply_hdr->DataIx = NET_BUF_IX_NONE;
        p_msg_reply_hdr->DataLen = 0u;
#endif
      }

      //                                                           Source Address Selection.
      addr_is_mcast = NetIPv6_IsAddrMcast(&p_buf_hdr->IPv6_AddrDest);
      if (addr_is_mcast == DEF_YES) {
        p_ipv6_addrs = NetIPv6_GetAddrSrcHandler(&p_buf_hdr->IF_Nbr,
                                                 (const  NET_IPv6_ADDR *) DEF_NULL,
                                                 (const  NET_IPv6_ADDR *)&p_buf_hdr->IPv6_AddrSrc,
                                                 DEF_NULL,
                                                 p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit_discard;
        }
        NET_UTIL_IPv6_ADDR_COPY(p_ipv6_addrs->AddrHost, ipv6_addr_src);
      } else {
        NET_UTIL_IPv6_ADDR_COPY(p_buf_hdr->IPv6_AddrDest, ipv6_addr_src);
      }

      //                                                           Destination Address Selection.
      NET_UTIL_IPv6_ADDR_COPY(p_buf_hdr->IPv6_AddrSrc, ipv6_addr_dest);

      //                                                           Calc ICMPv6 msg chk sum (see Note #5).
      NET_UTIL_VAL_SET_NET_16(&p_icmp_hdr_echo->ChkSum, 0x0000u);           // Clr             chk sum (see Note #5b).

      NET_UTIL_IPv6_ADDR_COPY(ipv6_addr_src, ipv6_pseudo_hdr.AddrSrc);
      NET_UTIL_IPv6_ADDR_COPY(ipv6_addr_dest, ipv6_pseudo_hdr.AddrDest);
#ifdef NET_ICMP_CHK_SUM_OFFLOAD_TX
      msg_chk_sum = 0u;
#else
      msg_chk_sum = NetUtil_16BitOnesCplChkSumDataCalc(p_msg_reply_head,
                                                       &ipv6_pseudo_hdr,
                                                       NET_IPv6_PSEUDO_HDR_SIZE);
#endif
      NET_UTIL_VAL_COPY_16(&p_icmp_hdr_echo->ChkSum, &msg_chk_sum);          // Copy chk sum in net order (see Note #5c).
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL:
      p_icmp_hdr_neighbor_adv = (NET_NDP_NEIGHBOR_ADV_HDR *)&p_msg_reply_head->DataPtr[p_msg_reply_hdr->ICMP_MsgIx];

      addr_unspecified = NetIPv6_IsAddrUnspecified(&p_buf_hdr->IPv6_AddrSrc);

      //                                                           Destination Address Selection.
      if (addr_unspecified == DEF_YES) {                        // RFC#4861 s7.24 p.63
        DEF_BIT_CLR(flags, NET_NDP_HDR_FLAG_ROUTER);
        DEF_BIT_CLR(flags, NET_NDP_HDR_FLAG_SOL);
        DEF_BIT_CLR(flags, NET_NDP_HDR_FLAG_OVRD);
        NET_UTIL_IPv6_ADDR_SET_MCAST_ALL_NODES(ipv6_addr_dest);
        NET_UTIL_IPv6_ADDR_SET_MCAST_ALL_NODES(ipv6_pseudo_hdr.AddrDest);
      } else {
        flags = NET_NDP_HDR_FLAG_SOL;
        flags |= NET_NDP_HDR_FLAG_OVRD;

        NET_UTIL_IPv6_ADDR_COPY(p_buf_hdr->IPv6_AddrSrc, ipv6_addr_dest);
        NET_UTIL_IPv6_ADDR_COPY(p_buf_hdr->IPv6_AddrSrc, ipv6_pseudo_hdr.AddrDest);
      }

      //                                                           Source Address Selection.

      NET_UTIL_IPv6_ADDR_COPY(p_icmp_hdr_neighbor_adv->TargetAddr, ipv6_pseudo_hdr.AddrSrc);
      NET_UTIL_IPv6_ADDR_COPY(p_icmp_hdr_neighbor_adv->TargetAddr, ipv6_addr_src);

      //                                                           Set Neighbor Advertisement Header.
      p_icmp_hdr_neighbor_adv->Type = NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_ADV;
      hop_limit = NET_IPv6_HDR_HOP_LIM_MAX;
      flags = NET_UTIL_NET_TO_HOST_32(flags);
      NET_UTIL_VAL_COPY_32(&p_icmp_hdr_neighbor_adv->Flags, &flags);
      if_nbr = p_buf_hdr->IF_Nbr;
      hw_addr_len = sizeof(hw_addr);

      NetIF_AddrHW_GetHandler(if_nbr, hw_addr, &hw_addr_len, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_discard;
      }

      p_mem = (CPU_INT08U *)&p_icmp_hdr_neighbor_adv->Opt;
      p_mem += NET_NDP_OPT_DATA_OFFSET;
      Mem_Copy(p_mem,
               &hw_addr[0],
               hw_addr_len);

      //                                                           Calc ICMP msg data len.
      if (p_buf_hdr->ICMP_MsgLen > p_msg_reply_hdr->ICMP_HdrLen) {
        p_msg_reply_hdr->DataIx = (CPU_INT16U)(p_msg_reply_hdr->ICMP_MsgIx  + msg_len_min);
        p_msg_reply_hdr->DataLen = (NET_BUF_SIZE)(p_msg_reply_hdr->ICMP_MsgLen - p_msg_reply_hdr->ICMP_HdrLen);
#if 0                                                           // Init'd in NetBuf_Get() [see Note #4].
      } else {
        p_msg_reply_hdr->DataIx = NET_BUF_IX_NONE;
        p_msg_reply_hdr->DataLen = 0u;
#endif
      }

      //                                                           Calc ICMPv6 msg chk sum (see Note #5).
      NET_UTIL_VAL_SET_NET_16(&p_icmp_hdr_neighbor_adv->ChkSum, 0x0000u);        // Clr             chk sum (see Note #5b).
#ifdef NET_ICMP_CHK_SUM_OFFLOAD_TX
      msg_chk_sum = 0u;
#else
      msg_chk_sum = NetUtil_16BitOnesCplChkSumDataCalc(p_msg_reply_head,
                                                       &ipv6_pseudo_hdr,
                                                       NET_IPv6_PSEUDO_HDR_SIZE);
#endif
      NET_UTIL_VAL_COPY_16(&p_icmp_hdr_neighbor_adv->ChkSum, &msg_chk_sum);
      break;

    default:                                                    // See Note #2.
      PP_UNUSED_PARAM(p_msg_req_hdr);
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxHdrTypeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit_discard;
  }

  //                                                               --------------- TX ICMPv6 REPLY MSG ----------------
  NetIPv6_Tx(p_msg_reply_head,
             &ipv6_addr_src,
             &ipv6_addr_dest,
             p_ext_hdr_head,
             NET_IPv6_HDR_TRAFFIC_CLASS,
             NET_IPv6_HDR_FLOW_LABEL,
             hop_limit,
             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  NetICMPv6_TxPktFree(p_msg_reply_head);

  NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.TxMsgCtr);
  NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.TxMsgReplyCtr);

  PP_UNUSED_PARAM(ipv6_pseudo_hdr);

  goto exit;

exit_discard:
  //                                                               If a buf list has ben assembled, discard it.
  p_chain_buf = p_msg_reply_head;
  //                                                               Discard each buf of the list.
  while (p_chain_buf != DEF_NULL) {
    p_chain_buf_next = p_chain_buf->Hdr.NextBufPtr;
    NetICMPv6_TxPktDiscard(p_chain_buf);
    p_chain_buf = p_chain_buf_next;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetICMPv6_TxMsgReq()
 *
 * @brief    (1) Transmit ICMPv6 Request Message :
 *               - (a) Acquire  network lock.
 *               - (b) Validate ICMPv6 Request Message :
 *                   - (1) Validate the following arguments :
 *                       - (A) Type
 *                       - (B) Code
 *                       - (C) Source Address
 *                   - (2) Validation of the following arguments deferred to NetIPv6_Tx() :
 *                       - (A) Hop Limit
 *                       - (B) Destination Address
 *                   - (3) Validation ignores the following arguments :
 *                       - (A) Data
 *                       - (B) Payload length
 *               - (c) Get buffer for ICMPv6 Request Message :
 *                   - (1) Calculate      ICMPv6 Request Message buffer size
 *                   - (2) Copy data into ICMPv6 Request Message
 *                   - (3) Initialize     ICMPv6 Request Message buffer controls
 *               - (d) Prepare ICMPv6 Request Message :
 *                   - (1) Type                               See 'net_icmpv6.h  ICMPv6 MESSAGE TYPES & CODES'
 *                   - (2) Code                               See 'net_icmpv6.h  ICMPv6 MESSAGE TYPES & CODES'
 *                   - (3) Identification (ID)
 *                   - (4) Sequence Number
 *                   - (5) Data
 *               - (e) Transmit ICMPv6 Request Message.
 *               - (f) Free     ICMPv6 Request Message buffer.
 *               - (g) Update   ICMPv6 transmit statistics.
 *
 *               - (h) Release network lock.
 *
 *               - (i) Return   ICMPv6 Request Message Identification & Sequence Number
 *                     OR
 *                     NULL id & sequence number structure, on failure
 *
 * @param    type        ICMPv6 Request Message type (see Note #1d1) :
 *                       NET_ICMPv6_MSG_TYPE_ECHO_REQ
 *
 * @param    code        ICMPv6 Request Message code (see Note #1d2).
 *
 * @param    id          ICMPv6 Request Message id (see 'NetICMPv6_TxMsgReq()  Note #3b').
 *
 * @param    addr_src    Source      IPv6 address.
 *
 * @param    addr_dest   Destination IPv6 address.
 *
 * @param    hop_lim     Specify the hop limit to transmit ICMP/IP packet.
 *
 * @param    dest_mcast  Specify if the IPv6 destination address is multicast.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   ICMPv6 Request Message's Identification (ID) & Sequence Numbers, if NO error(s).
 *           NULL                     Identification (ID) & Sequence Numbers, otherwise.
 *
 * @note     (2) NetICMPv6_TxMsgReq() blocked until network initialization completes.
 *
 * @note     (3) NetICMPv6_TxMsgReq() is the internal ICMPv6 handler for ICMPv6 Request Messages.  Its
 *               global declaration is required since NetICMPv6_TxMsgReq() calls the handler function
 *               from the OS port file (see also 'NetICMPv6_TxMsgReq()  Note #1').
 *
 * @note     (4) Default case already invalidated in NetICMPv6_TxMsgReqValidate().  However, the default
 *               case is included as an extra precaution in case 'type' is incorrectly modified.
 *
 * @note     (5) Assumes network buffer's protocol header size is large enough to accommodate ICMPv6 header
 *               size (see 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1').
 *
 * @note     (6) Some buffer controls were previously initialized in NetBuf_Get() when the buffer
 *               was allocated earlier in this function.  These buffer controls do NOT need to be
 *               re-initialized but are shown for completeness.
 *
 * @note     (7) (a) ICMPv6 message Check-Sum MUST be calculated AFTER the entire ICMPv6 message has been
 *                   prepared.  In addition, ALL multi-octet words are converted from host-order to
 *                   network-order since "the sum of 16-bit integers can be computed in either byte
 *                   order" [RFC #1071, Section 2.(B)].
 *                 - (b) ICMPv6 message Check-Sum field MUST be cleared to '0' BEFORE the ICMPv6 message
 *                       Check-Sum is calculated (see RFC #792, Sections 'Echo or Echo Reply Message :
 *                       Checksum', 'Timestamp or Timestamp Reply Message : Checksum'; & RFC #950,
 *                       Appendix I 'Address Mask ICMP', Section 'ICMP Fields : Checksum').
 *                 - (c) The ICMPv6 message Check-Sum field is returned in network-order & MUST NOT be re-
 *                       converted back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()
 *                       Note #3b' & 'net_util.c  NetUtil_16BitOnesCplChkSumDataCalc()  Note #4b').
 *
 * @note     (8) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
NET_ICMPv6_REQ_ID_SEQ NetICMPv6_TxMsgReq(NET_IF_NBR       if_nbr,
                                         CPU_INT08U       type,
                                         CPU_INT08U       code,
                                         CPU_INT16U       id,
                                         NET_IPv6_ADDR    *p_addr_src,
                                         NET_IPv6_ADDR    *p_addr_dest,
                                         NET_IPv6_HOP_LIM hop_lim,
                                         CPU_BOOLEAN      dest_mcast,
                                         void             *p_data,
                                         CPU_INT16U       data_len,
                                         RTOS_ERR         *p_err)
{
  NET_ICMPv6_REQ_ID_SEQ   id_seq;
  const NET_IPv6_ADDR_OBJ *p_ipv6_addrs;

  //                                                               Prepare err rtn val.
  id_seq.ID = id;
  id_seq.SeqNbr = 0u;

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetICMPv6_TxMsgReq);

  //                                                               Found Source Address
  if (p_addr_src == DEF_NULL) {
    p_ipv6_addrs = NetIPv6_GetAddrSrcHandler(&if_nbr,
                                             (const  NET_IPv6_ADDR *)p_addr_src,
                                             (const  NET_IPv6_ADDR *)p_addr_dest,
                                             DEF_NULL,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_INVALID_ADDR_SRC);
      goto exit;
    }

    p_addr_src = (NET_IPv6_ADDR *)&p_ipv6_addrs->AddrHost;
  }

  id_seq = NetICMPv6_TxMsgReqHandler(if_nbr,                    // TX ICMPv6 req msg.
                                     type,
                                     code,
                                     id,
                                     p_addr_src,
                                     p_addr_dest,
                                     hop_lim,
                                     dest_mcast,
                                     DEF_NULL,
                                     p_data,
                                     data_len,
                                     p_err);

exit:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (id_seq);
}

/****************************************************************************************************//**
 *                                       NetICMPv6_TxMsgReqHandler()
 *
 * @brief    (1) Transmit ICMPv6 Request Message :
 *               - (a) Validate ICMPv6 Request Message :
 *                   - (1) Validate the following arguments :
 *                       - (A) Type
 *                       - (B) Code
 *                       - (C) Source Address
 *                   - (2) Validation of the following arguments deferred to NetIPv6_Tx() :
 *                       - (B) Hop Limit
 *                       - (C) Destination Address
 *                   - (3) Validation ignores the following arguments :
 *                       - (A) Data
 *                       - (B) Payload length
 *               - (b) Get buffer for ICMPv6 Request Message :
 *                   - (1) Calculate      ICMPv6 Request Message buffer size
 *                   - (2) Copy data into ICMPv6 Request Message
 *                   - (3) Initialize     ICMPv6 Request Message buffer controls
 *               - (c) Prepare ICMPv6 Request Message :
 *                   - (1) Type                               See 'net_icmpv6.h  ICMPv6 MESSAGE TYPES & CODES'
 *                   - (2) Code                               See 'net_icmpv6.h  ICMPv6 MESSAGE TYPES & CODES'
 *                   - (3) Identification (ID)
 *                   - (4) Sequence Number
 *                   - (5) Data
 *               - (d) Transmit ICMPv6 Request Message.
 *               - (e) Free     ICMPv6 Request Message buffer.
 *               - (f) Update   ICMPv6 transmit statistics.
 *               - (g) Return   ICMPv6 Request Message Identification & Sequence Number
 *                     OR
 *                     NULL id & sequence number structure, on failure
 *
 * @param    if_nbr          Number of the interface to transmit the ICMPv6 request message.
 *                           NET_IF_NBR_NONE should be used when the interface is unknown.
 *
 * @param    type            ICMPv6 Request Message type (see Note #1d1) :
 *                           NET_ICMPv6_MSG_TYPE_ECHO_REQ
 *
 * @param    code            ICMPv6 Request Message code (see Note #1d2).
 *
 * @param    id              ICMPv6 Request Message id (see 'NetICMPv6_TxMsgReq()  Note #3b').
 *
 * @param    p_addr_src      Pointer to source      IPv6 address.
 *
 * @param    p_addr_dest     Pointer to Destination IPv6 address.
 *
 * @param    hop_lim         Specify the hop limit to transmit ICMP/IP packet.
 *
 * @param    dest_mcast      Specify if the IPv6 destination address is multicast.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   ICMPv6 Request Message's Identification (ID) & Sequence Numbers, if NO error(s).
 *           NULL                     Identification (ID) & Sequence Numbers, otherwise.
 *
 * @note     (2) NetICMPv6_TxMsgReqHandler() blocked until network initialization completes.
 *
 * @note     (3) NetICMPv6_TxMsgReqHandler() is the internal ICMPv6 handler for ICMPv6 Request Messages.  Its
 *               global declaration is required since it is used by the MLDP and NDP layers.
 *
 * @note     (4) Default case already invalidated in NetICMPv6_TxMsgReqValidate().  However, the default
 *               case is included as an extra precaution in case 'type' is incorrectly modified.
 *
 * @note     (5) Assumes network buffer's protocol header size is large enough to accommodate ICMPv6 header
 *               size (see 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1').
 *
 * @note     (6) Some buffer controls were previously initialized in NetBuf_Get() when the buffer
 *               was allocated earlier in this function.  These buffer controls do NOT need to be
 *               re-initialized but are shown for completeness.
 *
 * @note     (7) (a) ICMPv6 message Check-Sum MUST be calculated AFTER the entire ICMPv6 message has been
 *                   prepared.  In addition, ALL multi-octet words are converted from host-order to
 *                   network-order since "the sum of 16-bit integers can be computed in either byte
 *                   order" [RFC #1071, Section 2.(B)].
 *                   - (b) ICMPv6 message Check-Sum field MUST be cleared to '0' BEFORE the ICMPv6 message
 *                         Check-Sum is calculated (see RFC #792, Sections 'Echo or Echo Reply Message :
 *                         Checksum', 'Timestamp or Timestamp Reply Message : Checksum'; & RFC #950,
 *                         Appendix I 'Address Mask ICMP', Section 'ICMP Fields : Checksum').
 *                   - (c) The ICMPv6 message Check-Sum field is returned in network-order & MUST NOT be re-
 *                         converted back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()
 *                         Note #3b' & 'net_util.c  NetUtil_16BitOnesCplChkSumDataCalc()  Note #4b').
 *
 * @note     (8) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
NET_ICMPv6_REQ_ID_SEQ NetICMPv6_TxMsgReqHandler(NET_IF_NBR       if_nbr,
                                                CPU_INT08U       type,
                                                CPU_INT08U       code,
                                                CPU_INT16U       id,
                                                NET_IPv6_ADDR    *p_addr_src,
                                                NET_IPv6_ADDR    *p_addr_dest,
                                                NET_IPv6_HOP_LIM hop_lim,
                                                CPU_BOOLEAN      dest_mcast,
                                                NET_IPv6_EXT_HDR *p_ext_hdr_list,
                                                void             *p_data,
                                                CPU_INT16U       data_len,
                                                RTOS_ERR         *p_err)
{
  NET_ICMPv6_REQ_ID_SEQ    id_seq;
  CPU_INT16U               msg_size_hdr = 0u;
  CPU_INT16U               msg_size_data = 0u;
  CPU_INT16U               msg_size_tot;
  CPU_INT16U               msg_seq_nbr;
  CPU_INT16U               msg_ix;
  CPU_INT16U               msg_ix_offset;
  CPU_INT16U               msg_chk_sum;
  CPU_BOOLEAN              is_multicast;
  CPU_BOOLEAN              is_unspecified;
  NET_BUF                  *p_msg_req;
  NET_BUF                  *p_msg_req_head = DEF_NULL;
  NET_BUF                  *p_chain_buf;
  NET_BUF                  *p_chain_buf_next;
  NET_BUF_HDR              *p_msg_req_hdr = DEF_NULL;
  NET_BUF_HDR              *p_chain_buf_hdr;
  NET_ICMPv6_HDR_ECHO      *p_icmp_hdr_echo;
  NET_NDP_NEIGHBOR_SOL_HDR *p_icmp_hdr_neighbor_sol;
  NET_NDP_ROUTER_SOL_HDR   *p_icmp_hdr_router_sol;
  NET_NDP_OPT_HDR          *p_ndp_opt_hdr;
  NET_MLDP_V1_HDR          *p_icmp_hdr_v1;
  NET_IPv6_PSEUDO_HDR      ipv6_pseudo_hdr;
  NET_IPv6_ADDR            ipv6_mcast_sol_addr;
  const NET_IPv6_ADDR_OBJ  *p_ipv6_addrs;
  NET_IPv6_EXT_HDR         *p_ext_hdr_head;
  NET_IPv6_EXT_HDR         *p_ext_hdr;
  NET_IPv6_EXT_HDR         frag_hdr;
  NET_BUF_SIZE             buf_size_max;
  NET_BUF_SIZE             buf_size_max_data;
  CPU_INT16U               data_ix;
  CPU_INT32S               req_len;
  CPU_BOOLEAN              frag_used;
  CPU_BOOLEAN              is_valid;
  NET_MTU                  mtu;
  CPU_INT08U               hw_addr[NET_IP_HW_ADDR_LEN];
  CPU_INT08U               hw_addr_len;

  id_seq.ID = NET_ICMPv6_REQ_ID_NONE;
  id_seq.SeqNbr = NET_ICMPv6_REQ_SEQ_NONE;

  //                                                               ------------ VALIDATE ICMPv6 TX REQ MSG ------------
  NetICMPv6_TxMsgReqValidate(type, code, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------- VALIDATE ICMPv6 TX REQ MSG SRC ADDR --------
  if (if_nbr == NET_IF_NBR_NONE) {
    if_nbr = NET_IF_NBR_BASE_CFGD;                              // Set IF nbr to first configured IF (default IF).

    is_valid = NetIF_IsValidHandler(if_nbr);
    if (is_valid != DEF_YES) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      goto exit;
    }
  }

  p_ipv6_addrs = NetIPv6_GetAddrSrcHandler(&if_nbr,
                                           (const  NET_IPv6_ADDR *)p_addr_src,
                                           (const  NET_IPv6_ADDR *)p_addr_dest,
                                           DEF_NULL,
                                           p_err);

  is_unspecified = NetIPv6_IsAddrUnspecified(p_addr_src);
  if (is_unspecified != DEF_YES) {
    p_addr_src = (NET_IPv6_ADDR *)&p_ipv6_addrs->AddrHost;
  }

  if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
      && (is_unspecified != DEF_YES)      ) {
    goto exit;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               -------------- GET ICMPv6 REQ MSG BUF --------------
  //                                                               Calc req msg buf size.
  switch (type) {
    case NET_ICMPv6_MSG_TYPE_ECHO_REQ:
      msg_size_hdr = NET_ICMPv6_HDR_SIZE_ECHO;
      msg_size_data = (p_data != DEF_NULL) ? data_len : 0u;
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL:
      msg_size_hdr = NET_NDP_HDR_SIZE_NEIGHBOR_SOL;
      msg_size_data = (p_data != DEF_NULL) ? data_len : 0u;
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_ROUTER_SOL:
      msg_size_hdr = NET_NDP_HDR_SIZE_ROUTER_SOL;
      msg_size_data = (p_data != DEF_NULL) ? data_len : 0u;
      break;

    case NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1:
    case NET_ICMPv6_MSG_TYPE_MLDP_DONE:
      msg_size_hdr = NET_MLDP_HDR_SIZE_DFLT;
      msg_size_data = (p_data != DEF_NULL) ? data_len : 0u;
      break;

    default:                                                    // See Note #4.
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, id_seq);
  }

  msg_size_tot = msg_size_hdr + msg_size_data;

  //                                                               Get Size of Data space.
  frag_used = DEF_NO;
  mtu = NetIF_MTU_GetProtocol(if_nbr, NET_PROTOCOL_TYPE_ICMP_V6, NET_IF_FLAG_NONE);

  //                                                               Remove extension headers size from Data space.
  if (p_ext_hdr_list != DEF_NULL) {
    p_ext_hdr = p_ext_hdr_list;
    while (p_ext_hdr != DEF_NULL) {
      mtu -= p_ext_hdr->Len;
      p_ext_hdr = p_ext_hdr->NextHdrPtr;
    }
  }

  //                                                               Check if fragmentation is needed.
  if (msg_size_tot > mtu) {
    frag_used = DEF_YES;
    mtu -= NET_IPv6_FRAG_HDR_SIZE;
    mtu -= (mtu % NET_IPv6_FRAG_SIZE_UNIT);
    p_ext_hdr_head = NetIPv6_ExtHdrAddToList(p_ext_hdr_list,
                                             &frag_hdr,
                                             NET_IP_HDR_PROTOCOL_EXT_FRAG,
                                             NET_IPv6_FRAG_HDR_SIZE,
                                             NetIPv6_PrepareFragHdr,
                                             NET_IPv6_EXT_HDR_KEY_FRAG,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  } else {
    p_ext_hdr_head = p_ext_hdr_list;
  }

  //                                                               Get Index in Data Area by taking into account the ...
  //                                                               ... lower layer headers.
  data_ix = msg_size_hdr;
  NetIPv6_GetTxDataIx(if_nbr,
                      p_ext_hdr_list,
                      msg_size_tot,
                      mtu,
                      &data_ix);

  //                                                               Get req msg buf.
  msg_ix = data_ix - msg_size_hdr;

  buf_size_max = NetBuf_GetMaxSize(if_nbr,
                                   NET_TRANSACTION_TX,
                                   DEF_NULL,
                                   msg_ix);

  buf_size_max_data = (NET_BUF_SIZE)DEF_MIN(buf_size_max, mtu);

  if (frag_used == DEF_YES) {
    buf_size_max_data -= (buf_size_max_data % NET_IPv6_FRAG_SIZE_UNIT);
  }

  p_msg_req_head = DEF_NULL;

  while (msg_size_tot > 0) {
    req_len = (CPU_INT32S)DEF_MIN(msg_size_tot, buf_size_max_data);

    p_msg_req = NetBuf_Get(if_nbr,
                           NET_TRANSACTION_TX,
                           req_len,
                           msg_ix,
                           &msg_ix_offset,
                           NET_BUF_FLAG_NONE,
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      p_msg_req_head = p_msg_req;
      goto exit_discard;
    }

    msg_ix += msg_ix_offset;
    //                                                             If buf is 1st aquired, set p_msg_req_head to it ...
    if (p_msg_req_head == DEF_NULL) {
      p_msg_req_head = p_msg_req;
    } else {                                                    // ... else add it to the end NextBufPtr chain of ...
                                                                // ... p_msg_req_head.
      p_chain_buf = p_msg_req_head;
      p_chain_buf_hdr = &p_msg_req_head->Hdr;

      while (p_chain_buf_hdr->NextBufPtr != DEF_NULL) {
        p_chain_buf = p_chain_buf_hdr->NextBufPtr;
        p_chain_buf_hdr = &p_chain_buf->Hdr;
      }

      p_chain_buf_hdr->NextBufPtr = p_msg_req;
      p_chain_buf_hdr = &p_msg_req->Hdr;
      p_chain_buf_hdr->PrevBufPtr = p_chain_buf;
    }

    //                                                             Init req msg buf ctrls.
    p_msg_req_hdr = &p_msg_req->Hdr;
    p_msg_req_hdr->ICMP_MsgIx = (CPU_INT16U)msg_ix;
    p_msg_req_hdr->ICMP_MsgLen = (CPU_INT16U)req_len;
    p_msg_req_hdr->ICMP_HdrLen = (CPU_INT16U)msg_size_hdr;
    p_msg_req_hdr->TotLen = (NET_BUF_SIZE)p_msg_req_hdr->ICMP_MsgLen;
    p_msg_req_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_ICMP_V6;
    p_msg_req_hdr->ProtocolHdrTypeNetSub = NET_PROTOCOL_TYPE_ICMP_V6;

    msg_size_tot -= req_len;
    msg_size_hdr = 0;
    buf_size_max_data = (NET_BUF_SIZE)DEF_MIN(buf_size_max, mtu);
  }

  //                                                               Copy data into ICMPv6 req tx buf.
  if (msg_size_data > 0) {
    NetICMPv6_CpyDataToBuf((CPU_INT08U *)p_data,
                           p_msg_req_head,
                           msg_size_data,
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_discard;
    }
  }

  //                                                               ------------- PREPARE IPv6 PSEUDO HDR --------------
  Mem_Copy(&ipv6_pseudo_hdr.AddrSrc,
           p_addr_src,
           NET_IPv6_ADDR_SIZE);

  Mem_Copy(&ipv6_pseudo_hdr.AddrDest,
           p_addr_dest,
           NET_IPv6_ADDR_SIZE);

  ipv6_pseudo_hdr.UpperLayerPktLen = (CPU_INT32U)NET_UTIL_NET_TO_HOST_32(p_msg_req_hdr->ICMP_MsgLen);
  ipv6_pseudo_hdr.Zero = (CPU_INT16U)0x00u;
  ipv6_pseudo_hdr.NextHdr = (CPU_INT16U)NET_UTIL_NET_TO_HOST_16(NET_IP_HDR_PROTOCOL_ICMPv6);

  p_msg_req_hdr = &p_msg_req_head->Hdr;

  //                                                               --------------- PREPARE ICMP REQ MSG ---------------
  NET_ICMPv6_TX_GET_SEQ_NBR(msg_seq_nbr);

  switch (type) {
    case NET_ICMPv6_MSG_TYPE_ECHO_REQ:
      p_icmp_hdr_echo = (NET_ICMPv6_HDR_ECHO *)&p_msg_req_head->DataPtr[p_msg_req_hdr->ICMP_MsgIx];
      p_icmp_hdr_echo->Type = NET_ICMPv6_MSG_TYPE_ECHO_REQ;
      p_icmp_hdr_echo->Code = NET_ICMPv6_MSG_CODE_ECHO_REQ;
      NET_UTIL_VAL_COPY_SET_NET_16(&p_icmp_hdr_echo->ID, &id);
      NET_UTIL_VAL_COPY_SET_NET_16(&p_icmp_hdr_echo->SeqNbr, &msg_seq_nbr);
      //                                                           Calc ICMPv6 msg chk sum (see Note #7).
      NET_UTIL_VAL_SET_NET_16(&p_icmp_hdr_echo->ChkSum, 0x0000u);           // Clr             chk sum (see Note #7b).
#ifdef NET_ICMP_CHK_SUM_OFFLOAD_TX
      msg_chk_sum = 0u;
#else
      msg_chk_sum = NetUtil_16BitOnesCplChkSumDataCalc(p_msg_req_head,
                                                       &ipv6_pseudo_hdr,
                                                       NET_IPv6_PSEUDO_HDR_SIZE);
#endif
      NET_UTIL_VAL_COPY_16(&p_icmp_hdr_echo->ChkSum, &msg_chk_sum);         // Copy chk sum in net order (see Note #7c).
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL:
      DEF_BIT_SET(p_msg_req_hdr->Flags, NET_BUF_FLAG_TX_MULTICAST);

      p_icmp_hdr_neighbor_sol = (NET_NDP_NEIGHBOR_SOL_HDR *)&p_msg_req_head->DataPtr[p_msg_req_hdr->ICMP_MsgIx];
      p_icmp_hdr_neighbor_sol->Type = NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL;
      p_icmp_hdr_neighbor_sol->Code = NET_ICMPv6_MSG_CODE_NDP_NEIGHBOR_SOL;
      Mem_Clr(&p_icmp_hdr_neighbor_sol->Reserved, sizeof(p_icmp_hdr_neighbor_sol->Reserved));

      is_multicast = NetIPv6_IsAddrMcast(p_addr_dest);
      if (is_multicast == DEF_YES) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
        goto exit_discard;
      }

      Mem_Copy(&p_icmp_hdr_neighbor_sol->TargetAddr,
               p_addr_dest,
               NET_IPv6_ADDR_SIZE);

      //                                                           Destination address of a Neighbor Solicitation   ...
      //                                                           ... Message must be the solicited-node multicast ...
      //                                                           ... when sender is performing address resolution ...
      //                                                           ... and DAD.
      if (dest_mcast == DEF_YES) {
        NetIPv6_AddrMcastSolicitedSet(&ipv6_mcast_sol_addr, p_addr_dest, if_nbr, p_err);
        p_addr_dest = &ipv6_mcast_sol_addr;
        Mem_Copy(&ipv6_pseudo_hdr.AddrDest,
                 &ipv6_mcast_sol_addr,
                 NET_IPv6_ADDR_SIZE);
      }

      //                                                           Calc ICMPv6 msg chk sum (see Note #5).
      NET_UTIL_VAL_SET_NET_16(&p_icmp_hdr_neighbor_sol->ChkSum, 0x0000u);       // Clr         chk sum (see Note #5b).
#ifdef NET_ICMP_CHK_SUM_OFFLOAD_TX
      msg_chk_sum = 0u;
#else
      msg_chk_sum = NetUtil_16BitOnesCplChkSumDataCalc(p_msg_req_head,
                                                       &ipv6_pseudo_hdr,
                                                       NET_IPv6_PSEUDO_HDR_SIZE);
#endif
      NET_UTIL_VAL_COPY_16(&p_icmp_hdr_neighbor_sol->ChkSum, &msg_chk_sum);
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_ROUTER_SOL:
      DEF_BIT_SET(p_msg_req_hdr->Flags, NET_BUF_FLAG_TX_MULTICAST);

      p_icmp_hdr_router_sol = (NET_NDP_ROUTER_SOL_HDR *)&p_msg_req_head->DataPtr[p_msg_req_hdr->ICMP_MsgIx];
      p_icmp_hdr_router_sol->Type = NET_ICMPv6_MSG_TYPE_NDP_ROUTER_SOL;
      p_icmp_hdr_router_sol->Code = NET_ICMPv6_MSG_CODE_NDP_ROUTER_SOL;
      p_icmp_hdr_router_sol->Reserved = 0;

      p_ndp_opt_hdr = (NET_NDP_OPT_HDR *)&p_icmp_hdr_router_sol->Opt;

      if (is_unspecified == DEF_NO) {
        p_ndp_opt_hdr->Type = NET_NDP_OPT_TYPE_ADDR_SRC;
        p_ndp_opt_hdr->Len = 1u;

        hw_addr_len = sizeof(hw_addr);
        NetIF_AddrHW_GetHandler(if_nbr, hw_addr, &hw_addr_len, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit_discard;
        }

        Mem_Copy((&p_icmp_hdr_router_sol->Opt + (NET_NDP_OPT_DATA_OFFSET)),
                 &hw_addr[0],
                 hw_addr_len);
      } else {
        p_ndp_opt_hdr->Type = NET_NDP_OPT_TYPE_NONE;
        p_ndp_opt_hdr->Len = 0u;
      }

      //                                                           Calc ICMPv6 msg chk sum (see Note #5).
      NET_UTIL_VAL_SET_NET_16(&p_icmp_hdr_router_sol->ChkSum, 0x0000u);         // Clr             chk sum (see Note #5b).
#ifdef NET_ICMP_CHK_SUM_OFFLOAD_TX
      msg_chk_sum = 0u;
#else
      msg_chk_sum = NetUtil_16BitOnesCplChkSumDataCalc(p_msg_req_head,
                                                       &ipv6_pseudo_hdr,
                                                       NET_IPv6_PSEUDO_HDR_SIZE);
#endif
      NET_UTIL_VAL_COPY_16(&p_icmp_hdr_router_sol->ChkSum, &msg_chk_sum);
      break;

    case NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1:
    case NET_ICMPv6_MSG_TYPE_MLDP_DONE:
      DEF_BIT_SET(p_msg_req_hdr->Flags, NET_BUF_FLAG_TX_MULTICAST);

      p_icmp_hdr_v1 = (NET_MLDP_V1_HDR *)&p_msg_req_head->DataPtr[p_msg_req_hdr->ICMP_MsgIx];
      p_icmp_hdr_v1->Type = type;
      p_icmp_hdr_v1->Code = code;
      Mem_Clr(&p_icmp_hdr_v1->MaxResponseDly, sizeof(p_icmp_hdr_v1->MaxResponseDly));
      Mem_Clr(&p_icmp_hdr_v1->Reserved, sizeof(p_icmp_hdr_v1->Reserved));

      Mem_Copy(&p_icmp_hdr_v1->McastAddr,
               p_data,
               NET_IPv6_ADDR_SIZE);

      //                                                           Calc ICMPv6 msg chk sum (see Note #5).
      NET_UTIL_VAL_SET_NET_16(&p_icmp_hdr_v1->ChkSum, 0x0000u);             // Clr             chk sum (see Note #5b).
#ifdef NET_ICMP_CHK_SUM_OFFLOAD_TX
      msg_chk_sum = 0u;
#else
      msg_chk_sum = NetUtil_16BitOnesCplChkSumDataCalc(p_msg_req_head,
                                                       &ipv6_pseudo_hdr,
                                                       NET_IPv6_PSEUDO_HDR_SIZE);
#endif

      NET_UTIL_VAL_COPY_16(&p_icmp_hdr_v1->ChkSum, &msg_chk_sum);
      break;

    default:                                                    // See Note #4.
      goto exit_discard;
  }

  //                                                               ---------------- TX ICMPv6 REQ MSG -----------------
  NetIPv6_Tx(p_msg_req_head,
             p_addr_src,
             p_addr_dest,
             p_ext_hdr_head,
             NET_IPv6_HDR_TRAFFIC_CLASS,
             NET_IPv6_HDR_FLOW_LABEL,
             hop_lim,
             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  NetICMPv6_TxPktFree(p_msg_req);
  NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.TxMsgCtr);
  NET_CTR_STAT_INC(Net_StatCtrs.ICMPv6.TxMsgReqCtr);

  //                                                               ---------- RTN ICMPv6 REQ MSG ID/SEQ NBR -----------
  id_seq.ID = id;
  id_seq.SeqNbr = msg_seq_nbr;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  goto exit;

exit_discard:
  p_chain_buf = p_msg_req_head;
  p_chain_buf_next = DEF_NULL;
  do {
    if (p_chain_buf != DEF_NULL) {
      p_chain_buf_next = p_chain_buf->Hdr.NextBufPtr;
    }
    NetICMPv6_TxPktDiscard(p_chain_buf);
    p_chain_buf = p_chain_buf_next;
  } while (p_chain_buf != DEF_NULL);

exit:
  return (id_seq);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetICMPv6_RxPktValidate()
 *
 * @brief    (1) Validate received ICMPv6 packet :
 *
 *           - (a) Validate the received packet's destination                  See Note #3
 *
 *           - (b) (1) Validate the received packet's following ICMPv6 header fields :
 *
 *                   - (A) Type
 *                   - (B) Code
 *                   - (C) Pointer                         See RFC  #4443, Section 3.4 'Parameter Problem Message'
 *                   - (D) Message Length
 *                   - (E) Check-Sum                       See Note #7a
 *
 *           - (c) Convert the following ICMPv6 message fields from network-order to host-order :
 *
 *               - (1) Check-Sum                                               See Note #7c
 *
 *                   - (A) These fields are NOT converted directly in the received packet buffer's
 *                           data area but are converted in local or network buffer variables ONLY.
 *
 *           - (d) Update network buffer's length controls
 *
 *           - (e) Demultiplex ICMPv6 message type
 *
 * @param    p_buf       Pointer to network buffer that received ICMP packet.
 *
 * @param    ----        Argument checked   in NetICMPv6_Rx().
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    --------    Argument validated in NetICMPv6_Rx().
 *
 * @param    p_icmp_hdr  Pointer to received packet's ICMP header.
 *
 * @param    ---------   Argument validated in NetICMPv6_Rx().
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See 'net_icmpv6.h  ICMPv6 HEADER' for ICMPv6 header format.
 *
 * @note     (3) (a) RFC #1122, Sections 3.2.2.6 & 3.2.2.8 state that "an ICMP ... Request destined to an
 *                       IP broadcast or IP multicast address MAY be silently discarded".
 *
 *                       However, NO RFC specifies how OTHER ICMPv6 messages should handle messages received by
 *                       broadcast or multicast. Therefore, it is assumed that ALL ICMPv6 messages destined
 *                       to an IP broadcast or IP multicast address SHOULD be silently discarded.
 *
 *                           - (b) Since a packet destined to a valid IPv6 broadcast address MUST also have been received as
 *                       a link-layer broadcast (see RFC #1122, Section 3.3.6 & 'net_ip.c  NetIPv6_RxPktValidate()
 *                       Note #9d3B1b'), the determination of an IPv6 broadcast destination address need only verify
 *                       that the received packet was received as a link-layer broadcast packet.
 *
 * @note     (4) (a) RFC #1122, Section 3.2.2 requires that ICMPv6 messages with the following invalid ICMPv6
 *                       header fields be "silently discarded" :
 *
 *                           - (1) Type                                    RFC #1122, Section 3.2.2
 *
 *                           - (b) Assumes that ICMPv6 messages with the following invalid ICMPv6 header fields should also
 *                       be "silently discarded" :
 *
 *                           - (1) Code
 *                           - (2) Message Length
 *                           - (3) Pointer
 *                           - (4) Check-Sum
 *
 * @note     (5) See 'net_icmpv6.h  ICMPv6 MESSAGE TYPES & CODES' for supported ICMPv6 message types/codes.
 *
 * @note     (6) Since ICMPv6 message headers do NOT contain a message length field, the ICMPv6 Message Length
 *                       is assumed to be the remaining IPv6 Datagram Length.
 *
 * @note     (7) (a) ICMPv6 message Check-Sum field MUST be validated AFTER (or BEFORE) any multi-octet
 *                       words are converted from network-order to host-order since "the sum of 16-bit
 *                       integers can be computed in either byte order" [RFC #1071, Section 2.(B)].
 *
 *                       In other words, the ICMPv6 message Check-Sum CANNOT be validated AFTER SOME but NOT
 *                       ALL multi-octet words have been converted from network-order to host-order.
 *
 *                       However, ALL received packets' multi-octet words are converted in local or network
 *                       buffer variables ONLY (see Note #1cA).  Therefore, ICMPv6 message Check-Sum may be
 *                       validated at any point.
 *
 *                           - (b) RFC #792, Section 'Echo or Echo Reply Message : Checksum' specifies that "if the
 *                       total length is odd, the received data is padded with one octet of zeros for
 *                       computing the checksum".
 *
 *                       However, NO RFC specifies how OTHER ICMP message types should handle odd-length
 *                       check-sums.  Therefore, it is assumed that ICMPv6 Echo Request & Echo Reply Messages
 *                       should handle odd-length check-sums according to RFC #792's 'Echo or Echo Reply
 *                       Message : Checksum' specification, while ALL other ICMPv6 message types should handle
 *                       odd-length check-sums according to RFC #1071, Section 4.1.
 *
 *                       See also 'net_util.c  NetUtil_16BitSumDataCalc()  Note #8'.
 *
 *                           - (c) After the ICMPv6 message Check-Sum is validated, it is NOT necessary to convert the
 *                       Check-Sum from network-order to host-order since it is NOT required for further
 *                       processing.
 *
 * @note     (8) Default case already invalidated earlier in this function.  However, the default case
 *                       is included as an extra precaution in case 'Type' is incorrectly modified.
 *
 * @note     (9) (a) (1) Except for ICMPv6 Echo & Echo Reply Messages (see Note #9b), most ICMP messages do
 *                       NOT permit user &/or application data (see RFC #792 & RFC #1122, Sections 3.2.2).
 *
 *                           - (2) Most ICMPv6 messages that do NOT contain user &/or application data will NOT be
 *                       received in separate packet buffers since most ICMP messages are NOT large enough
 *                       to be fragmented since the minimum network buffer size MUST be configured such
 *                       that most ICMPv6 messages fit within a single packet buffer (see 'net_buf.h
 *                       NETWORK BUFFER INDEX & SIZE DEFINES  Note #1d').
 *
 *                       However, RFC #1122, Section 3.2.2 states that "every ICMPv6 error message includes
 *                       the Internet header and at least the first 8 data octets of the datagram that
 *                       triggered the error; more than 8 octets MAY be sent".  Thus, it is possible that
 *                       some received ICMPv6 error messages MAY contain more than 8 octets of the Internet
 *                       header & may therefore be received in one or more fragmented packet buffers.
 *
 *                       Furthermore, these additional error message octets SHOULD NOT contain user &/or
 *                       application data.
 *
 *                           - (3) ICMPv6 data index value to clear was previously initialized in NetBuf_Get() when
 *                       the buffer was allocated.  This index value does NOT need to be re-cleared but
 *                       is shown for completeness.
 *
 *                           - (b) (1) ICMPv6 Echo & Echo Reply Messages permit the transmission & receipt of user &/or
 *                       application data (see RFC #792, Section 'Echo or Echo Reply Message' & RFC #1122,
 *                       Section 3.2.2.6).
 *
 *                       Since the minimum network buffer size MUST be configured such that the entire
 *                       ICMPv6 Echo Message header MUST be received in a single packet (see 'net_buf.h
 *                       NETWORK BUFFER INDEX & SIZE DEFINES  Note #1d'), after the ICMPv6 Echo Message
 *                       header size is decremented from the first packet buffer's remaining number of
 *                       data octets, any remaining octets MUST be user &/or application data octets.
 *
 *                           - (A) Note that the 'Data' index is updated regardless of a null-size data length.
 *
 *                           - (2) If additional packet buffers exist, the remaining IP datagram 'Data' MUST be
 *                       user &/or application data.  Therefore, the 'Data' length does NOT need to
 *                       be adjusted but the 'Data' index MUST be updated.
 *
 *                           - (c) #### Total ICMPv6 Message Length is duplicated in ALL fragmented packet buffers
 *                       (may NOT be necessary; remove if unnecessary).
 *******************************************************************************************************/
static NET_ICMP_MSG_TYPE NetICMPv6_RxPktValidate(NET_BUF        *p_buf,
                                                 NET_BUF_HDR    *p_buf_hdr,
                                                 NET_ICMPv6_HDR *p_icmp_hdr,
                                                 RTOS_ERR       *p_err)
{
  CPU_BOOLEAN               addr_multicast;
  CPU_BOOLEAN               icmp_chk_sum_valid;
  CPU_INT16U                icmp_msg_len_hdr;
  CPU_INT16U                icmp_msg_len_min;
  CPU_INT16U                icmp_msg_len_max;
  CPU_INT16U                icmp_msg_len;
  NET_ICMPv6_HDR_PARAM_PROB *p_icmp_param_prob = DEF_NULL;
  NET_BUF                   *p_buf_next;
  NET_BUF_HDR               *p_buf_next_hdr;
  NET_IPv6_PSEUDO_HDR       ipv6_pseudo_hdr;
  NET_ICMP_MSG_TYPE         msg_type = NET_ICMP_MSG_TYPE_NONE;

  addr_multicast = NetIPv6_IsAddrMcast(&p_buf_hdr->IPv6_AddrDest);
  if (addr_multicast != DEF_NO) {                               // If ICMPv6 rx'd as multicast, rtn err (see Note #3).
    switch (p_icmp_hdr->Type) {
      case NET_ICMPv6_MSG_TYPE_ECHO_REQ:
      case NET_ICMPv6_MSG_TYPE_PKT_TOO_BIG:
      case NET_ICMPv6_MSG_TYPE_PARAM_PROB:
      case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL:
      case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_ADV:
      case NET_ICMPv6_MSG_TYPE_NDP_ROUTER_SOL:
      case NET_ICMPv6_MSG_TYPE_NDP_ROUTER_ADV:
      case NET_ICMPv6_MSG_TYPE_NDP_REDIRECT:
      case NET_ICMPv6_MSG_TYPE_MLDP_QUERY:
      case NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1:
        break;

      default:
        NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxMcastCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
    }
  }

  //                                                               ------------ VALIDATE ICMPv6 TYPE/CODE -------------
  switch (p_icmp_hdr->Type) {                                   // See Notes #4a1 & #4b1.
    case NET_ICMPv6_MSG_TYPE_DEST_UNREACH:
      switch (p_icmp_hdr->Code) {
        case NET_ICMPv6_MSG_CODE_DEST_NO_ROUTE:
        case NET_ICMPv6_MSG_CODE_DEST_COM_PROHIBITED:
        case NET_ICMPv6_MSG_CODE_DEST_BEYONG_SCOPE:
        case NET_ICMPv6_MSG_CODE_DEST_ADDR_UNREACHABLE:
        case NET_ICMPv6_MSG_CODE_DEST_PORT_UNREACHABLE:
        case NET_ICMPv6_MSG_CODE_DEST_SRC_ADDR_FAIL_INGRESS:
        case NET_ICMPv6_MSG_CODE_DEST_ROUTE_REJECT:
          icmp_msg_len_hdr = NET_ICMPv6_HDR_SIZE_DEST_UNREACH;
          icmp_msg_len_min = NET_ICMPv6_MSG_LEN_MIN_DEST_UNREACH;
          icmp_msg_len_max = NET_ICMPv6_MSG_LEN_MAX_DEST_UNREACH;
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrCodeCtr);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_TIME_EXCEED:
      switch (p_icmp_hdr->Code) {
        case NET_ICMPv6_MSG_CODE_TIME_EXCEED_HOP_LIMIT:
        case NET_ICMPv6_MSG_CODE_TIME_EXCEED_FRAG_REASM:
          icmp_msg_len_hdr = NET_ICMPv6_HDR_SIZE_TIME_EXCEED;
          icmp_msg_len_min = NET_ICMPv6_MSG_LEN_MIN_TIME_EXCEED;
          icmp_msg_len_max = NET_ICMPv6_MSG_LEN_MAX_TIME_EXCEED;
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrCodeCtr);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_PARAM_PROB:
      switch (p_icmp_hdr->Code) {
        case NET_ICMPv6_MSG_CODE_PARAM_PROB_IP_HDR:
        case NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_NEXT_HDR:
        case NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_OPT:
          icmp_msg_len_hdr = NET_ICMPv6_HDR_SIZE_PARAM_PROB;
          icmp_msg_len_min = NET_ICMPv6_MSG_LEN_MIN_PARAM_PROB;
          icmp_msg_len_max = NET_ICMPv6_MSG_LEN_MAX_PARAM_PROB;

          p_icmp_param_prob = (NET_ICMPv6_HDR_PARAM_PROB *)p_icmp_hdr;
          if (p_icmp_param_prob->Ptr < NET_ICMPv6_MSG_PTR_MIN_PARAM_PROB) {             // If ptr val < min ptr val, ..
            NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrPtrCtr);
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                                              // ... rtn err (see Note #4b3).
            goto exit;
          }
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrCodeCtr);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_ECHO_REQ:
    case NET_ICMPv6_MSG_TYPE_ECHO_REPLY:
      switch (p_icmp_hdr->Code) {
        case NET_ICMPv6_MSG_CODE_ECHO:
          icmp_msg_len_hdr = NET_ICMPv6_HDR_SIZE_ECHO;
          icmp_msg_len_min = NET_ICMPv6_MSG_LEN_MIN_ECHO;
          icmp_msg_len_max = NET_ICMPv6_MSG_LEN_MAX_ECHO;
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrCodeCtr);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL:
      switch (p_icmp_hdr->Code) {
        case NET_ICMPv6_MSG_CODE_NDP_NEIGHBOR_SOL:
          icmp_msg_len_hdr = NET_NDP_HDR_SIZE_NEIGHBOR_SOL;
          icmp_msg_len_min = NET_NDP_MSG_LEN_MIN_NEIGHBOR_SOL;
          icmp_msg_len_max = NET_NDP_MSG_LEN_MAX_NEIGHBOR_SOL;
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrCodeCtr);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_ADV:
      switch (p_icmp_hdr->Code) {
        case NET_ICMPv6_MSG_CODE_NDP_NEIGHBOR_ADV:
          icmp_msg_len_hdr = NET_NDP_HDR_SIZE_NEIGHBOR_ADV;
          icmp_msg_len_min = NET_NDP_MSG_LEN_MIN_NEIGHBOR_ADV;
          icmp_msg_len_max = NET_NDP_MSG_LEN_MAX_NEIGHBOR_ADV;
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrCodeCtr);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_ROUTER_ADV:
      switch (p_icmp_hdr->Code) {
        case NET_ICMPv6_MSG_CODE_NDP_ROUTER_ADV:
          icmp_msg_len_hdr = NET_NDP_HDR_SIZE_ROUTER_ADV;
          icmp_msg_len_min = NET_NDP_MSG_LEN_MIN_ROUTER_ADV;
          icmp_msg_len_max = NET_NDP_MSG_LEN_MAX_ROUTER_ADV;
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrCodeCtr);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_REDIRECT:
      switch (p_icmp_hdr->Code) {
        case NET_ICMPv6_MSG_CODE_NDP_REDIRECT:
          icmp_msg_len_hdr = NET_NDP_HDR_SIZE_REDIRECT;
          icmp_msg_len_min = NET_NDP_MSG_LEN_MIN_REDIRECT;
          icmp_msg_len_max = NET_NDP_MSG_LEN_MAX_REDIRECT;
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrCodeCtr);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_MLDP_QUERY:
    case NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1:                    // Code is ignored for MLDP messages.
      icmp_msg_len_hdr = NET_MLDP_HDR_SIZE_DFLT;
      icmp_msg_len_min = NET_MLDP_MSG_SIZE_MIN;
      icmp_msg_len_max = NET_MLDP_MSG_SIZE_MAX;
      break;

    default:                                                    // ------------------ INVALID TYPES -------------------
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrTypeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  //                                                               ------------- VALIDATE ICMPv6 MSG LEN --------------
  icmp_msg_len = p_buf_hdr->IP_DatagramLen;                     // See Note #6.
  p_buf_hdr->ICMP_MsgLen = icmp_msg_len;
  if (p_buf_hdr->ICMP_MsgLen < icmp_msg_len_min) {              // If msg len < min msg len, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrMsgLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
  if (icmp_msg_len_max != NET_ICMPv6_MSG_LEN_MAX_NONE) {
    if (p_buf_hdr->ICMP_MsgLen > icmp_msg_len_max) {            // If msg len > max msg len, rtn err.
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrMsgLenCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  }

  if (p_icmp_hdr->Type == NET_ICMPv6_MSG_TYPE_PARAM_PROB) {     // For ICMP Param Prob msg,   ...
    if (p_icmp_param_prob->Ptr >= p_buf_hdr->ICMP_MsgLen) {     // ... if ptr val >= msg len, ...
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrPtrCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                            // ... rtn err (see Note #4b3).
      goto exit;
    }
  }

  //                                                               ------------- VALIDATE ICMPv6 CHK SUM --------------
  //                                                               See Note #7.
  //                                                               See Note #7b.
  ipv6_pseudo_hdr.AddrSrc = p_buf_hdr->IPv6_AddrSrc;
  ipv6_pseudo_hdr.AddrDest = p_buf_hdr->IPv6_AddrDest;
  ipv6_pseudo_hdr.UpperLayerPktLen = (CPU_INT32U)NET_UTIL_HOST_TO_NET_32(p_buf_hdr->ICMP_MsgLen);
  ipv6_pseudo_hdr.Zero = (CPU_INT16U)0x00u;
  ipv6_pseudo_hdr.NextHdr = (CPU_INT16U)NET_UTIL_HOST_TO_NET_16(NET_IP_HDR_PROTOCOL_ICMPv6);

  switch (p_icmp_hdr->Type) {
    case NET_ICMPv6_MSG_TYPE_DEST_UNREACH:
    case NET_ICMPv6_MSG_TYPE_TIME_EXCEED:
    case NET_ICMPv6_MSG_TYPE_PARAM_PROB:
      icmp_chk_sum_valid = NetUtil_16BitOnesCplChkSumHdrVerify(p_icmp_hdr,
                                                               p_buf_hdr->ICMP_MsgLen);
      break;

    case NET_ICMPv6_MSG_TYPE_ECHO_REQ:
    case NET_ICMPv6_MSG_TYPE_ECHO_REPLY:
    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL:
    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_ADV:
    case NET_ICMPv6_MSG_TYPE_NDP_ROUTER_ADV:
    case NET_ICMPv6_MSG_TYPE_NDP_REDIRECT:
    case NET_ICMPv6_MSG_TYPE_MLDP_QUERY:
    case NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1:
#ifdef  NET_ICMP_CHK_SUM_OFFLOAD_RX_EN
      icmp_chk_sum_valid = DEF_YES;
#else
      icmp_chk_sum_valid = NetUtil_16BitOnesCplChkSumDataVerify(p_buf,
                                                                &ipv6_pseudo_hdr,
                                                                NET_IPv6_PSEUDO_HDR_SIZE);
#endif
      break;

    default:                                                    // See Note #8.
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrTypeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  if (icmp_chk_sum_valid != DEF_OK) {
    NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrChkSumCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
#if 0                                                           // Conv to host-order NOT necessary (see Note #7c).
  (void)NET_UTIL_VAL_GET_NET_16(&p_icmp_hdr->ChkSum);
#endif

  //                                                               ----------------- UPDATE BUF CTRLS -----------------
  p_buf_hdr->ICMP_HdrLen = icmp_msg_len_hdr;

  switch (p_icmp_hdr->Type) {
    case NET_ICMPv6_MSG_TYPE_DEST_UNREACH:
    case NET_ICMPv6_MSG_TYPE_TIME_EXCEED:
    case NET_ICMPv6_MSG_TYPE_PARAM_PROB:
      p_buf_hdr->DataLen = 0u;                                  // Clr data len/ix       (see Note #9a1).
#if 0                                                           // Clr'd in NetBuf_Get() [see Note #9a3].
      p_buf_hdr->DataIx = NET_BUF_IX_NONE;
#endif

      p_buf_next = p_buf_hdr->NextBufPtr;
      while (p_buf_next != DEF_NULL) {                          // Clr ALL pkt bufs' data len/ix (see Note #9a2).
        p_buf_next_hdr = &p_buf_next->Hdr;
        p_buf_next_hdr->DataLen = 0u;
#if 0                                                           // Clr'd in NetBuf_Get() [see Note #9a3].
        p_buf_next_hdr->DataIx = NET_BUF_IX_NONE;
#endif
        p_buf_next_hdr->ICMP_HdrLen = 0u;                       // NULL ICMPv6 hdr len in each pkt buf.
        p_buf_next_hdr->ICMP_MsgLen = icmp_msg_len;             // Dup  ICMPv6 msg len in each pkt buf (see Note #9c).
        p_buf_next = p_buf_next_hdr->NextBufPtr;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_ECHO_REQ:
    case NET_ICMPv6_MSG_TYPE_ECHO_REPLY:
      //                                                           Calc ICMP Echo Msg data len/ix (see Note #9b).
      p_buf_hdr->DataLen -= (NET_BUF_SIZE) p_buf_hdr->ICMP_HdrLen;
      p_buf_hdr->DataIx = (CPU_INT16U)(p_buf_hdr->ICMP_MsgIx + NET_ICMPv6_MSG_LEN_MIN_ECHO);

      p_buf_next = p_buf_hdr->NextBufPtr;
      while (p_buf_next != DEF_NULL) {                          // Calc ALL pkt bufs' data len/ix   (see Note #9b2).
        p_buf_next_hdr = &p_buf_next->Hdr;
        p_buf_next_hdr->DataIx = p_buf_next_hdr->ICMP_MsgIx;
        p_buf_next_hdr->ICMP_HdrLen = 0u;                       // NULL ICMP hdr len in each pkt buf.
        p_buf_next_hdr->ICMP_MsgLen = icmp_msg_len;             // Dup  ICMP msg len in each pkt buf (see Note #9c).
        p_buf_next = p_buf_next_hdr->NextBufPtr;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL:
    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_ADV:
    case NET_ICMPv6_MSG_TYPE_NDP_ROUTER_ADV:
    case NET_ICMPv6_MSG_TYPE_NDP_REDIRECT:
    case NET_ICMPv6_MSG_TYPE_MLDP_QUERY:
    case NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1:
      break;

    default:                                                    // See Note #8.
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrTypeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  //                                                               -------------- DEMUX ICMPv6 MSG TYPE ---------------
  switch (p_icmp_hdr->Type) {
    case NET_ICMPv6_MSG_TYPE_DEST_UNREACH:
    case NET_ICMPv6_MSG_TYPE_TIME_EXCEED:
    case NET_ICMPv6_MSG_TYPE_PARAM_PROB:
      msg_type = NET_ICMP_MSG_TYPE_ERR;
      break;

    case NET_ICMPv6_MSG_TYPE_ECHO_REQ:
      msg_type = NET_ICMP_MSG_TYPE_REQ;
      break;

    case NET_ICMPv6_MSG_TYPE_ECHO_REPLY:
      msg_type = NET_ICMP_MSG_TYPE_REPLY;
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL:
      msg_type = NET_ICMP_MSG_TYPE_NDP;
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_ADV:
      msg_type = NET_ICMP_MSG_TYPE_NDP;
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_ROUTER_ADV:
      msg_type = NET_ICMP_MSG_TYPE_NDP;
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_REDIRECT:
      msg_type = NET_ICMP_MSG_TYPE_NDP;
      break;

    case NET_ICMPv6_MSG_TYPE_MLDP_QUERY:
      msg_type = NET_ICMP_MSG_TYPE_MLDP;
      break;

    case NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1:
      msg_type = NET_ICMP_MSG_TYPE_MLDP;
      break;

    default:                                                    // See Note #8.
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.RxHdrTypeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

exit:
  return (msg_type);
}

/****************************************************************************************************//**
 *                                           NetICMPv6_RxPktFree()
 *
 * @brief    Free network buffer(s).
 *
 * @param    p_buf   Pointer to network buffer.
 *******************************************************************************************************/
static void NetICMPv6_RxPktFree(NET_BUF *p_buf)
{
  (void)NetBuf_FreeBufList(p_buf, DEF_NULL);
}

/****************************************************************************************************//**
 *                                       NetICMPv6_TxMsgErrValidate()
 *
 * @brief    (1) Validate received packet & transmit error parameters for ICMPv6 Error Message transmit :
 *
 *           - (a) RFC #4443, Section 2.4 (e) specifies that "an ICMP error message MUST NOT be sent as
 *                   the result of receiving the following" :
 *
 *               - (1) "An ICMPv6 Error Message",                                                      ...
 *
 *               - (2) "An ICMPv6 redirect message",                                                   ...
 *
 *               - (3) "A packet destined to an IPv6 multicast address. (There are two exceptions to
 *                       this rule: (1) the Packet Too Big Message (Section 3.2) to allow Path MTU
 *                       discovery to work for IPv6 multicast, and (2) the Parameter Problem Message,
 *                       Code 2 (Section 3.4) reporting an unrecognized IPv6 option (see Section 4.2
 *                       of [IPv6]) that has the Option Type highest-order two bits set to 10)",        ...
 *
 *               - (4) "A packet sent as a link-layer multicast (the exceptions from (3) apply to
 *                       this case, too)",                                                              ...
 *
 *               - (5) "A packet sent as a link-layer broadcast (the exceptions from (3) apply to
 *                       this case, too)",                                                              ...
 *
 *               - (6) "A packet whose source address does not uniquely identify a single node
 *                       -- e.g., the IPv6 Unspecified Address, an IPv6 multicast address, or an
 *                       address known by the ICMP message originator to be an IPv6 anycast address"
 *
 *                   - (A) ALL IPv6 source addresses already validated (see 'net_ip.c  NetIPv6_RxPktValidate()
 *                           Note #9c') except 'This Host' & 'Specified Host' addresses.
 *
 *           - (b) Validate ICMPv6 Error Message transmit parameters :
 *
 *               - (1) Type
 *               - (2) Code
 *               - (3) Pointer                                         See Note #6
 *
 * @param    p_buf       Pointer to network buffer that received a packet with error(s).
 *
 * @param    -----      Argument checked   in NetICMPv6_TxMsgErr().
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    ---------  Argument validated in NetICMPv6_TxMsgErr().
 *
 * @param    p_ip_hdr    Pointer to received packet's IP header.
 *
 * @param    --------   -   Argument validated in NetICMPv6_TxMsgErr().
 *
 * @param    type        ICMPv6 Error Message type (see Note #5) :
 *                       NET_ICMPv6_MSG_TYPE_DEST_UNREACH
 *                       NET_ICMPv6_MSG_TYPE_PKT_TOO_BIG
 *                       NET_ICMPv6_MSG_TYPE_TIME_EXCEED
 *                       NET_ICMPv6_MSG_TYPE_PARAM_PROB
 *
 * @param    code        ICMPv6 Error Message code (see Note #5).
 *
 * @param    ptr         Pointer to received packet's ICMPv6 error (optional).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See 'net_ipv6.h  IPv6 ADDRESS DEFINES for supported IP addresses.
 *
 * @note     (3) Default case already invalidated in NetIPv6_RxPktValidate().  However, the default case
 *                       is included as an extra precaution in case 'IP_AddrSrc' is incorrectly modified.
 *
 * @note     (4) See 'net_icmpv6.h  ICMPv6 MESSAGE TYPES & CODES' for supported ICMPv6 message types & codes.
 *
 * @note     (5) (a) ICMPv6 Parameter Problem Messages' pointer fields validated by comparing the pointer
 *                       field value to minimum & maximum pointer field values.
 *
 *                           - (b) Since an ICMPv6 Parameter Problem Message's minimum pointer field value is NOT less
 *                       than zero (see 'net_icmpv6.h  ICMPv6 POINTER DEFINES'), a minimum pointer field value
 *                       check is NOT required unless native data type 'CPU_INT16U' is incorrectly configured
 *                       as a signed integer in 'cpu.h'.
 *
 *                           - (c) Since an ICMPv6 Parameter Problem Message may be received for an IPv6 or higher-layer
 *                       protocol, the maximum pointer field value is specific to each received ICMPv6 packets'
 *                       IPv6 header length & demultiplexed protocol header length :
 *
 *                       Pointer Field Value < Maximum Pointer Field Value = IPv6 Header Length
 *                       [ + Other Protocol Header Length ]
 *
 *                           - (d) See 'net_icmpv6.h  ICMP POINTER DEFINES  Note #2' for supported ICMPv6 Parameter Problem
 *                       Message protocols.
 *******************************************************************************************************/
static void NetICMPv6_TxMsgErrValidate(NET_BUF      *p_buf,
                                       NET_BUF_HDR  *p_buf_hdr,
                                       NET_IPv6_HDR *p_ip_hdr,
                                       CPU_INT08U   type,
                                       CPU_INT08U   code,
                                       CPU_INT32U   ptr,
                                       RTOS_ERR     *p_err)
{
#if 0                                                           // See Note #1a2B.
  CPU_BOOLEAN addr_broadcast;
#endif
  CPU_BOOLEAN    addr_multicast;
  CPU_BOOLEAN    rx_broadcast;
  CPU_BOOLEAN    ip_flag_frags_more;
  NET_IPv6_FLAGS ip_flags;
  CPU_INT16U     ip_frag_offset;
  NET_ICMPv6_HDR *p_icmp_hdr;

  PP_UNUSED_PARAM(ptr);

  //                                                               ------------- CHK LINK-LAYER BROADCAST -------------
  rx_broadcast = DEF_BIT_IS_SET(p_buf_hdr->Flags, NET_BUF_FLAG_RX_BROADCAST);
  //                                                               If pkt rx'd via broadcast and no exception, ...
  if (rx_broadcast != DEF_NO && (type != NET_ICMPv6_MSG_TYPE_PKT_TOO_BIG
                                 && (type != NET_ICMPv6_MSG_TYPE_PARAM_PROB  || code != NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_OPT))) {
    NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxInvalidBroadcast);
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);                              // ... rtn err (see Note #1a3).
    goto exit;
  }

  //                                                               ---------------- CHK IPv6 DEST ADDR ----------------
  addr_multicast = NetIPv6_IsAddrMcast(&p_buf_hdr->IPv6_AddrDest);
  //                                                               If pkt rx'd via multicast and no exceptions, ...
  if (addr_multicast != DEF_NO && (type != NET_ICMPv6_MSG_TYPE_PKT_TOO_BIG
                                   && (type != NET_ICMPv6_MSG_TYPE_PARAM_PROB  || code != NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_OPT))) {
    NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxInvalidMulticast);
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);                              // ... rtn err (see Note #1a2).
    goto exit;
  }

  //                                                               ------------------ CHK IPv6 FRAG -------------------

  ip_flags = p_buf_hdr->IPv6_Flags_FragOffset & NET_IPv6_FRAG_FLAGS_MASK;
  ip_flag_frags_more = DEF_BIT_IS_SET(ip_flags, NET_IPv6_FRAG_FLAG_FRAG_MORE);
  if (ip_flag_frags_more != DEF_NO) {                           // If 'More Frags' flag set ...
    ip_frag_offset = p_buf_hdr->IPv6_Flags_FragOffset & NET_IPv6_FRAG_OFFSET_MASK;
    if (ip_frag_offset != NET_IPv6_FRAG_OFFSET_NONE) {          // ... & frag offset != 0,  ...
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxInvalidFrag);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);                            // ... rtn err for non-initial frag (see Note #1a4).
      goto exit;
    }
  }

  //                                                               ---------------- CHK ICMPv6 ERR MSG ----------------
  if (p_ip_hdr->NextHdr == NET_IP_HDR_PROTOCOL_ICMPv6) {        // If rx'd IP datagram is ICMPv6, ...
    p_icmp_hdr = (NET_ICMPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->ICMP_MsgIx];

    switch (p_icmp_hdr->Type) {                                 // ... chk ICMPv6 msg type & ...
      case NET_ICMPv6_MSG_TYPE_DEST_UNREACH:
      case NET_ICMPv6_MSG_TYPE_TIME_EXCEED:
      case NET_ICMPv6_MSG_TYPE_PARAM_PROB:
        RTOS_ERR_SET(*p_err, RTOS_ERR_TX);                          // ... rtn err for ICMPv6 err msgs (see Note #1a1)
        goto exit;

      case NET_ICMPv6_MSG_TYPE_ECHO_REQ:
      case NET_ICMPv6_MSG_TYPE_ECHO_REPLY:
        break;

      default:                                                  // ------------------ INVALID TYPES -------------------
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
        goto exit;
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetICMPv6_TxMsgReqValidate()
 *
 * @brief    (1) Validate parameters for ICMPv6 Request Message transmit :
 *
 *           - (a) Type                                                        See Note #2
 *           - (b) Code                                                        See Note #2
 *
 * @param    type    ICMPv6 Request Message type (see Note #1a) :
 *                   NET_ICMPv6_MSG_TYPE_ECHO_REQ
 *                   NET_ICMPv6_MSG_TYPE_NDP_ROUTER_SOL
 *                   NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL
 *                   NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1
 *                   NET_ICMPv6_MSG_TYPE_MLDP_DONE
 *
 * @param    code    ICMPv6 Request Message code (see Note #1b).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See 'net_icmpv6.h  ICMPv6 MESSAGE TYPES & CODES' for supported ICMPv6 message types & codes.
 *******************************************************************************************************/
static void NetICMPv6_TxMsgReqValidate(CPU_INT08U type,
                                       CPU_INT08U code,
                                       RTOS_ERR   *p_err)
{
  //                                                               -------- VALIDATE ICMPv6 REQ MSG TYPE/CODE ---------
  switch (type) {
    case NET_ICMPv6_MSG_TYPE_ECHO_REQ:
      switch (code) {
        case NET_ICMPv6_MSG_CODE_ECHO_REQ:
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxHdrCodeCtr);
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
      }
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL:
      switch (code) {
        case NET_ICMPv6_MSG_CODE_NDP_NEIGHBOR_SOL:
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxHdrCodeCtr);
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
      }
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_ROUTER_SOL:
      switch (code) {
        case NET_ICMPv6_MSG_CODE_NDP_ROUTER_SOL:
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxHdrCodeCtr);
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
      }
      break;

    case NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1:
      switch (code) {
        case NET_ICMPv6_MSG_CODE_MLDP_REPORT:
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxHdrCodeCtr);
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
      }
      break;

    case NET_ICMPv6_MSG_TYPE_MLDP_DONE:
      switch (code) {
        case NET_ICMPv6_MSG_CODE_MLDP_DONE:
          break;

        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxHdrCodeCtr);
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
      }
      break;

    case NET_ICMPv6_MSG_TYPE_ECHO_REPLY:
    case NET_ICMPv6_MSG_TYPE_NDP_ROUTER_ADV:
    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_ADV:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxHdrTypeCtr);
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }
}

/****************************************************************************************************//**
 *                                           NetICMPv6_TxPktFree()
 *
 * @brief    Free network buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *
 * @note     (1) (a) Although ICMPv6 Transmit initially requests the network buffer for transmit,
 *                   the ICMPv6 layer does NOT maintain a reference to the buffer.
 *
 *                       - (b) Also, since the network interface deallocation task frees ALL unreferenced buffers
 *                   after successful transmission, the ICMP layer must NOT free the transmit buffer.
 *
 *                   See also 'net_if.c  NetIF_TxDeallocTaskHandler()  Note #1c'.
 *******************************************************************************************************/
static void NetICMPv6_TxPktFree(NET_BUF *p_buf)
{
  PP_UNUSED_PARAM(p_buf);                                       // Prevent 'variable unused' warning (see Note #1).
}

/****************************************************************************************************//**
 *                                           NetICMPv6_TxPktDiscard()
 *
 * @brief    On any ICMPv6 transmit packet error(s), discard packet & buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *******************************************************************************************************/
static void NetICMPv6_TxPktDiscard(NET_BUF *p_buf)
{
  NET_CTR *p_ctr;

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  p_ctr = &Net_ErrCtrs.ICMPv6.TxPktDiscardedCtr;
#else
  p_ctr = DEF_NULL;
#endif
  (void)NetBuf_FreeBuf(p_buf, p_ctr);
}

/****************************************************************************************************//**
 *                                           NetICMPv6_GetTxDataIx()
 *
 * @brief    (1) Solves the starting index of the ICMPv6 data from the data buffer beginning.
 *
 *       - (2) Starting index if found by adding up the header sizes of the lower-level
 *               protocol headers.
 *
 * @param    if_nbr      Network interface number to transmit data.
 *
 * @param    hdr_len     Length of the ICMPv6 header.
 *
 * @param    data_len    Length of the ICMPv6 payload.
 *
 * @param    p_ix        Pointer to the current protocol index.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (3) If the payload is greater than the largest fragmentable IPv6 datagram (65535 bytes) the packet
 *                       must be discarded (see RFC #2460, Section 4.5).
 *******************************************************************************************************/
static void NetICMPv6_GetTxDataIx(NET_IF_NBR       if_nbr,
                                  CPU_INT32U       data_len,
                                  NET_IPv6_EXT_HDR *p_ext_hdr_list,
                                  CPU_INT16U       *p_ix,
                                  RTOS_ERR         *p_err)
{
  NET_MTU mtu;

  mtu = NetIF_MTU_GetProtocol(if_nbr, NET_PROTOCOL_TYPE_ICMP_V6, NET_IF_FLAG_NONE);

  if (data_len > NET_IPv6_FRAG_SIZE_MAX) {
    NET_CTR_ERR_INC(Net_ErrCtrs.ICMPv6.TxInvalidLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);                              // See Note #2.
    goto exit;
  }
  //                                                               Add the lower-level header offsets.
  NetIPv6_GetTxDataIx(if_nbr,
                      p_ext_hdr_list,
                      data_len,
                      mtu,
                      p_ix);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetICMPv6_CpyData()
 *
 * @brief    Copy data from a source buffer to a destination buffer. Source and destination buffers
 *           can be chained to other buffers to form a fragmented message.
 *
 * @param    p_buf_src   Pointer to a source      buffer.
 *
 * @param    p_buf_dest  Pointer to a destination buffer.
 *
 * @param    msg_len     Length of the message to copy from one buffer to another.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) If destination is a fragment and it is not the last of its chain, then the data written
 *                       to that buffer must be a multiple of 8 bytes. (See RFC #2460, Section 4.5).
 *******************************************************************************************************/
static void NetICMPv6_CpyData(NET_BUF    *p_buf_src,
                              NET_BUF    *p_buf_dest,
                              CPU_INT32U msg_len,
                              RTOS_ERR   *p_err)
{
  NET_BUF_HDR  *p_buf_hdr_src;
  NET_BUF_HDR  *p_buf_hdr_dest;
  NET_BUF_SIZE ix_dest;
  NET_BUF_SIZE ix_src;
  NET_BUF_SIZE len_src;
  NET_BUF_SIZE len_dest;
  NET_BUF_SIZE len;
  NET_BUF_SIZE write_total;

  p_buf_hdr_src = &p_buf_src->Hdr;
  p_buf_hdr_dest = &p_buf_dest->Hdr;

  ix_src = p_buf_hdr_src->ICMP_MsgIx;
  len_src = p_buf_hdr_src->DataLen + p_buf_hdr_src->ICMP_HdrLen;
  ix_dest = p_buf_hdr_dest->ICMP_MsgIx;
  len_dest = p_buf_hdr_dest->TotLen;

  write_total = 0u;

  if (ix_src == NET_BUF_IX_NONE) {
    goto exit;
  }

  //                                                               Copy data until all data is moved.
  while (write_total < msg_len) {
    len = (NET_BUF_SIZE)DEF_MIN(len_src, len_dest);

    if (len == 0) {
      goto exit;
    }

    NetBuf_DataCopy(p_buf_dest,
                    p_buf_src,
                    ix_dest,
                    ix_src,
                    len,
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
    //                                                             Find new source index.
    if (len_src == len) {
      p_buf_src = p_buf_hdr_src->NextBufPtr;                    // Source buffer done.
      if (p_buf_src == DEF_NULL) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        goto exit;
      }

      p_buf_hdr_src = &p_buf_src->Hdr;
      ix_src = p_buf_hdr_src->DataIx;
      len_src = p_buf_hdr_src->DataLen;
    } else {
      ix_src += len;
      len_src -= len;
    }

    //                                                             Find new destination index.
    if (len_dest == len) {
      p_buf_dest = p_buf_hdr_dest->NextBufPtr;                  // Destination buffer done.
      if (p_buf_dest == DEF_NULL) {
        goto exit;
      }

      p_buf_hdr_dest = &p_buf_dest->Hdr;
      ix_dest = p_buf_hdr_dest->ICMP_MsgIx;
      len_dest = p_buf_hdr_dest->TotLen - p_buf_hdr_dest->ICMP_HdrLen;
    } else {
      ix_dest += len;
      len_dest -= len;
    }

    write_total += len;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetICMPv6_CpyDataToBuf()
 *
 * @brief    Copy data to a destination buffer. Destination buffer CAN be chained to other buffers
 *           to form a fragmented message.
 *
 * @param    p_data      Pointer to the data      buffer.
 *
 * @param    p_buf_dest  Pointer to a destination buffer.
 *
 * @param    msg_len     Length of the message to copy in the destination buffer(s).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) If destination is a fragment and it is not the last of its chain, then the data written
 *                       to that buffer must be a multiple of 8 bytes. (See RFC #2460, Section 4.5).
 *******************************************************************************************************/
static void NetICMPv6_CpyDataToBuf(CPU_INT08U *p_data,
                                   NET_BUF    *p_buf_dest,
                                   CPU_INT32U msg_len,
                                   RTOS_ERR   *p_err)
{
  NET_BUF_HDR  *p_buf_hdr_dest;
  NET_BUF_SIZE ix_dest;
  NET_BUF_SIZE len_dest;
  NET_BUF_SIZE len;
  NET_BUF_SIZE len_rem;
  NET_BUF_SIZE write_total;

  p_buf_hdr_dest = &p_buf_dest->Hdr;

  ix_dest = p_buf_hdr_dest->ICMP_MsgIx + p_buf_hdr_dest->ICMP_HdrLen;
  len_dest = p_buf_hdr_dest->ICMP_MsgLen - p_buf_hdr_dest->ICMP_HdrLen;

  write_total = 0u;

  //                                                               Copy data until all data is moved.
  while (write_total < msg_len) {
    len_rem = msg_len - write_total;

    len = (NET_BUF_SIZE)DEF_MIN(len_rem, len_dest);

    if (len == 0) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
      goto exit;
    }

    NetBuf_DataWr(p_buf_dest,
                  ix_dest,
                  len,
                  p_data);

    p_buf_hdr_dest->DataIx = (CPU_INT16U)ix_dest;
    p_buf_hdr_dest->DataLen = (NET_BUF_SIZE)len;

    //                                                             Find new destination index.
    if (len_dest == len) {
      p_buf_dest = p_buf_hdr_dest->NextBufPtr;                  // Destination buffer done.
      if (p_buf_dest == DEF_NULL) {
        goto exit;
      }

      p_buf_hdr_dest = &p_buf_dest->Hdr;
      ix_dest = p_buf_hdr_dest->ICMP_MsgIx;
      len_dest = p_buf_hdr_dest->ICMP_MsgLen - p_buf_hdr_dest->ICMP_HdrLen;
    } else {
      ix_dest += len;
      len_dest -= len;
    }

    write_total += len;
  }

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_ICMPv6_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
