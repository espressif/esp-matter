/***************************************************************************//**
 * @file
 * @brief Network Icmp V4 Layer - (Internet Control Message Protocol)
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

#ifndef  _NET_ICMPv4_PRIV_H_
#define  _NET_ICMPv4_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_cfg_net.h"
#include  "net_type_priv.h"
#include  "net_stat_priv.h"
#include  "net_priv.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_ICMPv4_HDR_SIZE_DFLT                          8

#define  NET_ICMPv4_HDR_SIZE_DEST_UNREACH                NET_ICMPv4_HDR_SIZE_DFLT
#define  NET_ICMPv4_HDR_SIZE_SRC_QUENCH                  NET_ICMPv4_HDR_SIZE_DFLT
#define  NET_ICMPv4_HDR_SIZE_TIME_EXCEED                 NET_ICMPv4_HDR_SIZE_DFLT
#define  NET_ICMPv4_HDR_SIZE_PARAM_PROB                  NET_ICMPv4_HDR_SIZE_DFLT
#define  NET_ICMPv4_HDR_SIZE_ECHO                        NET_ICMPv4_HDR_SIZE_DFLT

/********************************************************************************************************
 *                                       ICMPv4 MESSAGE TYPES & CODES
 *
 * Note(s) : (1) 'DEST_UNREACH' abbreviated to 'DEST' for ICMP 'Destination Unreachable' message
 *                   error codes to enforce ANSI-compliance of 31-character symbol length uniqueness.
 *
 *           (2) ICMPv4 'Redirect' & 'Router' messages are NOT supported (see 'net_icmp.h  Note #1').
 *
 *           (3) ICMPv4 'Address Mask Request' messages received by this host are NOT supported (see
 *               'net_icmp.h  Note #3').
 *******************************************************************************************************/

//                                                                 ----------------- ICMPv4 MSG TYPES -----------------
#define  NET_ICMPv4_MSG_TYPE_NONE                        DEF_INT_08U_MAX_VAL

#define  NET_ICMPv4_MSG_TYPE_ECHO_REPLY                    0u
#define  NET_ICMPv4_MSG_TYPE_DEST_UNREACH                  3u
#define  NET_ICMPv4_MSG_TYPE_SRC_QUENCH                    4u
#define  NET_ICMPv4_MSG_TYPE_REDIRECT                      5u   // See Note #2.
#define  NET_ICMPv4_MSG_TYPE_ECHO_REQ                      8u
#define  NET_ICMPv4_MSG_TYPE_ROUTE_AD                      9u   // See Note #2.
#define  NET_ICMPv4_MSG_TYPE_ROUTE_REQ                    10u   // See Note #2.
#define  NET_ICMPv4_MSG_TYPE_TIME_EXCEED                  11u
#define  NET_ICMPv4_MSG_TYPE_PARAM_PROB                   12u
#define  NET_ICMPv4_MSG_TYPE_TS_REQ                       13u
#define  NET_ICMPv4_MSG_TYPE_TS_REPLY                     14u
#define  NET_ICMPv4_MSG_TYPE_ADDR_MASK_REQ                17u
#define  NET_ICMPv4_MSG_TYPE_ADDR_MASK_REPLY              18u

//                                                                 ----------------- ICMPv4 MSG CODES -----------------
#define  NET_ICMPv4_MSG_CODE_NONE                        DEF_INT_08U_MAX_VAL

#define  NET_ICMPv4_MSG_CODE_ECHO                          0u
#define  NET_ICMPv4_MSG_CODE_ECHO_REQ                      0u
#define  NET_ICMPv4_MSG_CODE_ECHO_REPLY                    0u

#define  NET_ICMPv4_MSG_CODE_DEST_NET                      0u   // See Note #1.
#define  NET_ICMPv4_MSG_CODE_DEST_HOST                     1u
#define  NET_ICMPv4_MSG_CODE_DEST_PROTOCOL                 2u
#define  NET_ICMPv4_MSG_CODE_DEST_PORT                     3u
#define  NET_ICMPv4_MSG_CODE_DEST_FRAG_NEEDED              4u
#define  NET_ICMPv4_MSG_CODE_DEST_ROUTE_FAIL               5u
#define  NET_ICMPv4_MSG_CODE_DEST_NET_UNKNOWN              6u
#define  NET_ICMPv4_MSG_CODE_DEST_HOST_UNKNOWN             7u
#define  NET_ICMPv4_MSG_CODE_DEST_HOST_ISOLATED            8u
#define  NET_ICMPv4_MSG_CODE_DEST_NET_TOS                 11u
#define  NET_ICMPv4_MSG_CODE_DEST_HOST_TOS                12u

#define  NET_ICMPv4_MSG_CODE_SRC_QUENCH                    0u

#define  NET_ICMPv4_MSG_CODE_TIME_EXCEED_TTL               0u
#define  NET_ICMPv4_MSG_CODE_TIME_EXCEED_FRAG_REASM        1u

#define  NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR             0u
#define  NET_ICMPv4_MSG_CODE_PARAM_PROB_OPT_MISSING        1u

#define  NET_ICMPv4_MSG_CODE_TS                            0u
#define  NET_ICMPv4_MSG_CODE_TS_REQ                        0u
#define  NET_ICMPv4_MSG_CODE_TS_REPLY                      0u

#define  NET_ICMPv4_MSG_CODE_ADDR_MASK                     0u
#define  NET_ICMPv4_MSG_CODE_ADDR_MASK_REQ                 0u
#define  NET_ICMPv4_MSG_CODE_ADDR_MASK_REPLY               0u

//                                                                 --------- IPv4 HDR PTR IXs ---------
#define  NET_ICMPv4_PTR_IX_BASE                            0
#define  NET_ICMPv4_PTR_IX_IP_BASE                       NET_ICMPv4_PTR_IX_BASE

#define  NET_ICMPv4_PTR_IX_IP_VER                       (NET_ICMPv4_PTR_IX_IP_BASE +  0)
#define  NET_ICMPv4_PTR_IX_IP_HDR_LEN                   (NET_ICMPv4_PTR_IX_IP_BASE +  0)
#define  NET_ICMPv4_PTR_IX_IP_TOS                       (NET_ICMPv4_PTR_IX_IP_BASE +  1)
#define  NET_ICMPv4_PTR_IX_IP_TOT_LEN                   (NET_ICMPv4_PTR_IX_IP_BASE +  2)
#define  NET_ICMPv4_PTR_IX_IP_ID                        (NET_ICMPv4_PTR_IX_IP_BASE +  4)
#define  NET_ICMPv4_PTR_IX_IP_FLAGS                     (NET_ICMPv4_PTR_IX_IP_BASE +  6)
#define  NET_ICMPv4_PTR_IX_IP_FRAG_OFFSET               (NET_ICMPv4_PTR_IX_IP_BASE +  6)
#define  NET_ICMPv4_PTR_IX_IP_TTL                       (NET_ICMPv4_PTR_IX_IP_BASE +  8)
#define  NET_ICMPv4_PTR_IX_IP_PROTOCOL                  (NET_ICMPv4_PTR_IX_IP_BASE +  9)
#define  NET_ICMPv4_PTR_IX_IP_CHK_SUM                   (NET_ICMPv4_PTR_IX_IP_BASE + 10)
#define  NET_ICMPv4_PTR_IX_IP_ADDR_SRC                  (NET_ICMPv4_PTR_IX_IP_BASE + 12)
#define  NET_ICMPv4_PTR_IX_IP_ADDR_DEST                 (NET_ICMPv4_PTR_IX_IP_BASE + 16)
#define  NET_ICMPv4_PTR_IX_IP_OPTS                      (NET_ICMPv4_PTR_IX_IP_BASE + 20)

#define  NET_ICMPv4_MSG_PTR_NONE                         DEF_INT_08U_MAX_VAL

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                   ICMP REQUEST MESSAGE IDENTIFICATION & SEQUENCE NUMBER DATA TYPE
 *******************************************************************************************************/

#define  NET_ICMPv4_REQ_ID_NONE                               0u
#define  NET_ICMPv4_REQ_SEQ_NONE                              0u

//                                                                 ------- NET ICMP REQ MSG ID/SEQ NBR --------
typedef  struct  net_icmpv4_req_id_seq {
  CPU_INT16U ID;                                                        // ICMP Req Msg ID.
  CPU_INT16U SeqNbr;                                                    // ICMP Req Msg Seq Nbr.
} NET_ICMPv4_REQ_ID_SEQ;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetICMPv4_Init(RTOS_ERR *p_err);

void NetICMPv4_Rx(NET_BUF  *p_buf,
                  RTOS_ERR *p_err);

void NetICMPv4_TxMsgErr(NET_BUF    *p_buf,
                        CPU_INT08U type,
                        CPU_INT08U code,
                        CPU_INT08U ptr,
                        RTOS_ERR   *p_err);

CPU_INT16U NetICMPv4_TxEchoReq(NET_IPv4_ADDR *p_addr_dest,
                               CPU_INT16U    id,
                               void          *p_data,
                               CPU_INT16U    data_len,
                               RTOS_ERR      *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_ICMPv4_PRIV_H_
