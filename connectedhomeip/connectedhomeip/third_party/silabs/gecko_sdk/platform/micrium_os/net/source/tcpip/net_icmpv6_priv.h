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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_ICMPv6_PRIV_H_
#define  _NET_ICMPv6_PRIV_H_

#include  "../../include/net_cfg_net.h"

#ifdef   NET_ICMPv6_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "net_type_priv.h"
#include  "net_stat_priv.h"
#include  "net_buf_priv.h"
#include  "net_ipv6_priv.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_ICMPv6_HDR_SIZE_DFLT                          8

#define  NET_ICMPv6_HDR_SIZE_DEST_UNREACH                NET_ICMPv6_HDR_SIZE_DFLT
#define  NET_ICMPv6_HDR_SIZE_TIME_EXCEED                 NET_ICMPv6_HDR_SIZE_DFLT
#define  NET_ICMPv6_HDR_SIZE_PARAM_PROB                  NET_ICMPv6_HDR_SIZE_DFLT
#define  NET_ICMPv6_HDR_SIZE_ECHO                        NET_ICMPv6_HDR_SIZE_DFLT

/********************************************************************************************************
 *                                   ICMPv6 MESSAGE TYPES & CODES DEFINES
 *******************************************************************************************************/

//                                                                 ----------------- ICMPv6 MSG TYPES -----------------
#define  NET_ICMPv6_MSG_TYPE_NONE                        DEF_INT_08U_MAX_VAL

#define  NET_ICMPv6_MSG_TYPE_ECHO_REQ                    128u
#define  NET_ICMPv6_MSG_TYPE_ECHO_REPLY                  129u

#define  NET_ICMPv6_MSG_TYPE_DEST_UNREACH                  1u
#define  NET_ICMPv6_MSG_TYPE_PKT_TOO_BIG                   2u
#define  NET_ICMPv6_MSG_TYPE_TIME_EXCEED                   3u
#define  NET_ICMPv6_MSG_TYPE_PARAM_PROB                    4u

#define  NET_ICMPv6_MSG_TYPE_MLDP_QUERY                  130u
#define  NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1              131u
#define  NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V2              143u
#define  NET_ICMPv6_MSG_TYPE_MLDP_DONE                   132u

#define  NET_ICMPv6_MSG_TYPE_NDP_ROUTER_SOL              133u
#define  NET_ICMPv6_MSG_TYPE_NDP_ROUTER_ADV              134u
#define  NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL            135u
#define  NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_ADV            136u
#define  NET_ICMPv6_MSG_TYPE_NDP_REDIRECT                137u

//                                                                 ----------------- ICMPv6 MSG CODES -----------------
#define  NET_ICMPv6_MSG_CODE_NONE                        DEF_INT_08U_MAX_VAL

#define  NET_ICMPv6_MSG_CODE_ECHO                          0u
#define  NET_ICMPv6_MSG_CODE_ECHO_REQ                      0u
#define  NET_ICMPv6_MSG_CODE_ECHO_REPLY                    0u

#define  NET_ICMPv6_MSG_CODE_DEST_NO_ROUTE                 0u
#define  NET_ICMPv6_MSG_CODE_DEST_COM_PROHIBITED           1u
#define  NET_ICMPv6_MSG_CODE_DEST_BEYONG_SCOPE             2u
#define  NET_ICMPv6_MSG_CODE_DEST_ADDR_UNREACHABLE         3u
#define  NET_ICMPv6_MSG_CODE_DEST_PORT_UNREACHABLE         4u
#define  NET_ICMPv6_MSG_CODE_DEST_SRC_ADDR_FAIL_INGRESS    5u
#define  NET_ICMPv6_MSG_CODE_DEST_ROUTE_REJECT             6u

#define  NET_ICMPv6_MSG_CODE_TIME_EXCEED_HOP_LIMIT         0u
#define  NET_ICMPv6_MSG_CODE_TIME_EXCEED_FRAG_REASM        1u

#define  NET_ICMPv6_MSG_CODE_PARAM_PROB_IP_HDR             0u
#define  NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_NEXT_HDR       1u
#define  NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_OPT            2u

#define  NET_ICMPv6_MSG_CODE_MLDP_QUERY                    0u
#define  NET_ICMPv6_MSG_CODE_MLDP_REPORT                   0u
#define  NET_ICMPv6_MSG_CODE_MLDP_DONE                     0u

#define  NET_ICMPv6_MSG_CODE_NDP_ROUTER_SOL                0u
#define  NET_ICMPv6_MSG_CODE_NDP_ROUTER_ADV                0u
#define  NET_ICMPv6_MSG_CODE_NDP_NEIGHBOR_SOL              0u
#define  NET_ICMPv6_MSG_CODE_NDP_NEIGHBOR_ADV              0u
#define  NET_ICMPv6_MSG_CODE_NDP_REDIRECT                  0u

/********************************************************************************************************
 *                                       ICMPv6 POINTER DEFINES
 *
 * Note(s) : (1) In RFC #4443, the Parameter Problem Message defines a pointer (PTR) as an index (IX) into
 *               an option or message.
 *
 *           (2) ICMPv6 Parameter Problem Message pointer validation currently ONLY supports the following
 *               protocols :
 *
 *               (a) IPv6
 *******************************************************************************************************/

#define  NET_ICMPv6_PTR_IX_BASE                            0

//                                                                 --------- IPv6 HDR PTR IXs ---------
#define  NET_ICMPv6_PTR_IX_IP_BASE                       NET_ICMPv6_PTR_IX_BASE

#define  NET_ICMPv6_PTR_IX_IP_VER                       (NET_ICMPv6_PTR_IX_IP_BASE +  0)
#define  NET_ICMPv6_PTR_IX_IP_TRAFFIC_CLASS             (NET_ICMPv6_PTR_IX_IP_BASE +  0)
#define  NET_ICMPv6_PTR_IX_IP_FLOW_LABEL                (NET_ICMPv6_PTR_IX_IP_BASE +  1)
#define  NET_ICMPv6_PTR_IX_IP_PAYLOAD_LEN               (NET_ICMPv6_PTR_IX_IP_BASE +  4)
#define  NET_ICMPv6_PTR_IX_IP_NEXT_HDR                  (NET_ICMPv6_PTR_IX_IP_BASE +  6)
#define  NET_ICMPv6_PTR_IX_IP_HOP_LIM                   (NET_ICMPv6_PTR_IX_IP_BASE +  7)
#define  NET_ICMPv6_PTR_IX_IP_ADDR_SRC                  (NET_ICMPv6_PTR_IX_IP_BASE +  8)
#define  NET_ICMPv6_PTR_IX_IP_ADDR_DEST                 (NET_ICMPv6_PTR_IX_IP_BASE + 24)

//                                                                 -------- ICMPv6 MSG PTR IXs --------
#define  NET_ICMPv6_PTR_IX_ICMP_BASE                       0

#define  NET_ICMPv6_PTR_IX_ICMP_TYPE                       0
#define  NET_ICMPv6_PTR_IX_ICMP_CODE                       1
#define  NET_ICMPv6_PTR_IX_ICMP_CHK_SUM                    2

#define  NET_ICMPv6_PTR_IX_ICMP_PTR                        4
#define  NET_ICMPv6_PTR_IX_ICMP_UNUSED                     4
#define  NET_ICMPv6_PTR_IX_ICMP_UNUSED_PARAM_PROB          5

#define  NET_ICMPv6_PTR_IX_IP_FRAG_OFFSET                  2

/********************************************************************************************************
 *                                       ICMPv6 MESSAGE DEFINES
 *******************************************************************************************************/

#define  NET_ICMPv6_MSG_PTR_NONE                         DEF_INT_08U_MAX_VAL

#define  NET_ICMPv6_MSG_PTR_MIN_PARAM_PROB               NET_ICMPv6_MSG_LEN_MIN_DFLT

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               ICMPv6 HEADER
 *
 * Note(s) : (1) See RFC #4443 for ICMPv6 message header formats.
 *******************************************************************************************************/

//                                                                 -------------- NET ICMPv6 HDR --------------
typedef  struct  net_ICMPv6_hdr {
  CPU_INT08U Type;                                                      // ICMPv6 msg type.
  CPU_INT08U Code;                                                      // ICMPv6 msg code.
  CPU_INT16U ChkSum;                                                    // ICMPv6 msg chk sum.
} NET_ICMPv6_HDR;

/********************************************************************************************************
 *                   ICMP REQUEST MESSAGE IDENTIFICATION & SEQUENCE NUMBER DATA TYPE
 *******************************************************************************************************/

#define  NET_ICMPv6_REQ_ID_NONE                            0u
#define  NET_ICMPv6_REQ_SEQ_NONE                           0u

//                                                                 ------- NET ICMP REQ MSG ID/SEQ NBR --------
typedef  struct  net_icmpv6_req_id_seq {
  CPU_INT16U ID;                                                        // ICMP Req Msg ID.
  CPU_INT16U SeqNbr;                                                    // ICMP Req Msg Seq Nbr.
} NET_ICMPv6_REQ_ID_SEQ;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NET_ICMPv6_TX_GET_SEQ_NBR()
 *
 * @brief    Get next ICMPv6 transmit message sequence number.
 *
 * @param    seq_nbr     Variable that will receive the returned ICMPv6 transmit message sequence number.
 *
 *           NetICMPv6_TxMsgReq().
 *
 *           This macro is an INTERNAL network protocol suite macro & SHOULD NOT be called by
 *           application function(s).
 *
 * @note         (1) Return ICMP sequence number is NOT converted from host-order to network-order.
 *******************************************************************************************************/

#define  NET_ICMPv6_TX_GET_SEQ_NBR(seq_nbr)     do { NET_UTIL_VAL_COPY_16(&(seq_nbr), &NetICMPv6_TxSeqNbrCtr); \
                                                     NetICMPv6_TxSeqNbrCtr++;                                } while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetICMPv6_Init(void);

void NetICMPv6_Rx(NET_BUF  *p_buf,
                  RTOS_ERR *p_err);

void NetICMPv6_TxMsgErr(NET_BUF    *p_buf,
                        CPU_INT08U type,
                        CPU_INT08U code,
                        CPU_INT32U ptr,
                        RTOS_ERR   *p_err);

NET_ICMPv6_REQ_ID_SEQ NetICMPv6_TxMsgReq(NET_IF_NBR       if_nbr,                   // See Note #1.
                                         CPU_INT08U       type,
                                         CPU_INT08U       code,
                                         CPU_INT16U       id,
                                         NET_IPv6_ADDR    *p_addr_src,
                                         NET_IPv6_ADDR    *p_addr_dest,
                                         NET_IPv6_HOP_LIM hop_limit,
                                         CPU_BOOLEAN      dest_mcast,
                                         void             *p_data,
                                         CPU_INT16U       data_len,
                                         RTOS_ERR         *p_err);

NET_ICMPv6_REQ_ID_SEQ NetICMPv6_TxMsgReqHandler(NET_IF_NBR       if_nbr,          // See Note #1.
                                                CPU_INT08U       type,
                                                CPU_INT08U       code,
                                                CPU_INT16U       id,
                                                NET_IPv6_ADDR    *p_addr_src,
                                                NET_IPv6_ADDR    *p_addr_dest,
                                                NET_IPv6_HOP_LIM TTL,
                                                CPU_BOOLEAN      dest_mcast,
                                                NET_IPv6_EXT_HDR *p_ext_hdr_list,
                                                void             *p_data,
                                                CPU_INT16U       data_len,
                                                RTOS_ERR         *p_err);

CPU_INT16U NetICMPv6_TxEchoReq(NET_IPv6_ADDR *p_addr_dest,
                               CPU_INT16U    id,
                               void          *p_msg_data,
                               CPU_INT16U    p_data,
                               RTOS_ERR      *p_err);

void NetICMPv6_TxMsgReply(NET_BUF        *p_buf,
                          NET_BUF_HDR    *p_buf_hdr,
                          NET_ICMPv6_HDR *p_icmp_hdr,
                          RTOS_ERR       *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_ICMPv6_MODULE_EN
#endif // _NET_ICMPv6_PRIV_H_
