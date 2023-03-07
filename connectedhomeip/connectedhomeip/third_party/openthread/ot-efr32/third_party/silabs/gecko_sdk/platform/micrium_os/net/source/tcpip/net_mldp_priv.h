/***************************************************************************//**
 * @file
 * @brief Network Mldp Layer - (Multicast Listener Discovery Protocol)
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

#ifndef  _NET_MLDP_PRIV_H_
#define  _NET_MLDP_PRIV_H_

#include  "../../include/net_cfg_net.h"

#ifdef   NET_MLDP_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "net_type_priv.h"
#include  "net_tmr_priv.h"
#include  "net_buf_priv.h"

#include  <common/include/lib_math.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       MLDP MESSAGE SIZE DEFINES
 *******************************************************************************************************/

#define  NET_MLDP_HDR_SIZE_DFLT                            8
#define  NET_MLDP_MSG_SIZE_MIN                            16
#define  NET_MLDP_MSG_SIZE_MAX                           NET_ICMPv6_MSG_LEN_MAX_NONE

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       MLDP CACHE QUANTITY DATA TYPE
 *
 * Note(s) : (1) NET_MLDP_CACHE_NBR_MAX  SHOULD be #define'd based on 'NET_MLDP_HOST_GRP_QTY' data type
 *               declared.
 *******************************************************************************************************/

typedef  CPU_INT16U NET_MLDP_HOST_GRP_QTY;

#define  NET_MLDP_HOST_GRP_NBR_MIN                         1
#define  NET_MLDP_HOST_GRP_NBR_MAX                       DEF_INT_16U_MAX_VAL

/********************************************************************************************************
 *                                       MLDP MESSAGE TYPE DATA TYPE
 *******************************************************************************************************/

typedef  enum  net_mldp_msg_type {
  NET_MLDP_MSG_TYPE_NONE,
  NET_MLDP_MSG_TYPE_QUERY,
  NET_MLDP_MSG_TYPE_REPORT
} NET_MLDP_MSG_TYPE;

/********************************************************************************************************
 *                                   MLDP HOST GROUP STATE DATA TYPE
 *
 *                                         -------------------
 *                                         |                 |
 *                                         |                 |
 *                                         |                 |
 *                                         |                 |
 *                            ------------>|      FREE       |<------------
 *                            |            |                 |            |
 *                            |            |                 |            |
 *                            |            |                 |            |
 *                            |            |                 |            |
 *                            |            -------------------            | (1e) STOP LISTENING
 *                            |                     |                     |
 *                            | (1e) STOP LISTENING | (1a)START LISTENING |
 *                            |                     |                     |
 *                   -------------------            |            -------------------
 *                   |                 |<------------            |                 |
 *                   |                 |                         |                 |
 *                   |                 |<------------------------|                 |
 *                   |                 |  (1c) QUERY  RECEIVED   |                 |
 *                   |    DELAYING     |                         |      IDLE       |
 *                   |                 |------------------------>|                 |
 *                   |                 |  (1b) REPORT RECEIVED   |                 |
 *                   |                 |                         |                 |
 *                   |                 |------------------------>|                 |
 *                   -------------------  (1d) TIMER  EXPIRED    -------------------
 *
 * Note(s) : (1) See RFC #2710
 *******************************************************************************************************/

typedef enum net_mldp_host_grp_state {
  NET_MLDP_HOST_GRP_STATE_NONE = 0u,
  NET_MLDP_HOST_GRP_STATE_FREE = 1u,
  NET_MLDP_HOST_GRP_STATE_DELAYING = 2u,
  NET_MLDP_HOST_GRP_STATE_IDLE = 3u,
} NET_MLDP_HOST_GRP_STATE;

/********************************************************************************************************
 *                                       MLDP HEADERS DATA TYPE
 *
 * Note(s) : (1) See RFC #2710 Section #3 for MLDP message header format.
 *******************************************************************************************************/

//                                                                 -------------- NET MLDP V1 HDR --------------
typedef  struct  net_mldp_v1_hdr {
  CPU_INT08U    Type;                                                   // MLDP msg type.
  CPU_INT08U    Code;                                                   // MLDP msg code.
  CPU_INT16U    ChkSum;                                                 // MLDP msg chk sum.
  CPU_INT16U    MaxResponseDly;                                         // MLDP max response dly.
  CPU_INT16U    Reserved;                                               // MLDP reserved bits.
  NET_IPv6_ADDR McastAddr;                                              // MLDP mcast addr.
} NET_MLDP_V1_HDR;

/********************************************************************************************************
 *                                   IPv6 MULTICAST GROUP DATA TYPES
 *
 * Note(s) : (1) Structure holding the group membership information of a IPv6 multicast address
 *******************************************************************************************************/

typedef  struct  net_mldp_host_grp NET_MLDP_HOST_GRP;

struct net_mldp_host_grp {
  NET_MLDP_HOST_GRP       *PrevIF_ListPtr;                      // Ptr to PREV MLDP host grp of same IF.
  NET_MLDP_HOST_GRP       *NextIF_ListPtr;                      // Ptr to NEXT MLDP host grp of same IF.

  NET_TMR                 *TmrPtr;                              // Pointer to MDLP delay timer.
  CPU_INT32U              Delay_ms;                             // Delay value.

  NET_IF_NBR              IF_Nbr;                               // IF nbr attached to the MDLP group.
  NET_IPv6_ADDR           AddrGrp;                              // Multicast address of the group.

  NET_MLDP_HOST_GRP_STATE State;                                // MLDP   host grp state.
  CPU_INT16U              RefCtr;                               // MLDP   host grp ref ctr.
  CPU_INT16U              Flags;                                // MLDP   host grp flags.

  SLIST_MEMBER            ListNode;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetMLDP_Init(MEM_SEG  *p_mem_seg,
                  RTOS_ERR *p_err);

NET_MLDP_HOST_GRP *NetMLDP_HostGrpJoinHandler(NET_IF_NBR          if_nbr,
                                              const NET_IPv6_ADDR *p_addr,
                                              RTOS_ERR            *p_err);

CPU_BOOLEAN NetMLDP_HostGrpLeaveHandler(NET_IF_NBR          if_nbr,
                                        const NET_IPv6_ADDR *p_addr,
                                        RTOS_ERR            *p_err);

CPU_BOOLEAN NetMLDP_IsGrpJoinedOnIF(NET_IF_NBR          if_nbr,
                                    const NET_IPv6_ADDR *p_addr_grp);

void NetMLDP_Rx(NET_BUF         *p_buf,
                NET_BUF_HDR     *p_buf_hdr,
                NET_MLDP_V1_HDR *p_ndp_hdr,
                RTOS_ERR        *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_MLDP_MODULE_EN
#endif // _NET_MLDP_PRIV_H_
