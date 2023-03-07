/***************************************************************************//**
 * @file
 * @brief Network Ip Layer - (Internet Protocol)
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

#ifndef  _NET_IP_PRIV_H_
#define  _NET_IP_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_ip.h"

#include  "net_def_priv.h"
#include  "net_type_priv.h"
#include  "net_if_priv.h"

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   IP HEADER PROTOCOL FIELD DEFINES
 *
 * Note(s) : (1) Supports ONLY a subset of allowed protocol numbers :
 *
 *               (a) ICMP
 *               (b) IGMP
 *               (c) UDP
 *               (d) TCP
 *               (e) ICMPv6
 *
 *               See also 'net.h  Note #2a';
 *                   & see 'RFC #1340  Assigned Numbers' for a complete list of protocol numbers,
 *                   & see 'RFC #2463,  Section 1' for ICMPv6 protocol number.
 *******************************************************************************************************/

#define  NET_IP_HDR_PROTOCOL_EXT_HOP_BY_HOP                0u
#define  NET_IP_HDR_PROTOCOL_ICMP                          1u
#define  NET_IP_HDR_PROTOCOL_IGMP                          2u
#define  NET_IP_HDR_PROTOCOL_TCP                           6u
#define  NET_IP_HDR_PROTOCOL_UDP                          17u
#define  NET_IP_HDR_PROTOCOL_EXT_ROUTING                  43u
#define  NET_IP_HDR_PROTOCOL_EXT_FRAG                     44u
#define  NET_IP_HDR_PROTOCOL_EXT_ESP                      50u
#define  NET_IP_HDR_PROTOCOL_EXT_AUTH                     51u
#define  NET_IP_HDR_PROTOCOL_ICMPv6                       58u
#define  NET_IP_HDR_PROTOCOL_EXT_NONE                     59u
#define  NET_IP_HDR_PROTOCOL_EXT_DEST                     60u
#define  NET_IP_HDR_PROTOCOL_EXT_MOBILITY                 135u

/********************************************************************************************************
 *                                           IP FLAG DEFINES
 *******************************************************************************************************/

//                                                                 ------------------- NET IP FLAGS -------------------
#define  NET_IPv4_FLAG_NONE                           DEF_BIT_NONE

#define  NET_IPv6_FLAG_NONE                           DEF_BIT_NONE
#define  NET_IPv6_FLAG                                DEF_BIT_00

//                                                                 IPv6 tx flags copied from IP hdr flags.
#define  NET_IPv6_FLAG_TX_DONT_FRAG                   NET_IPv6_HDR_FLAG_FRAG_DONT

#define  NET_IP_FRAG_SIZE_NONE                        DEF_INT_16U_MAX_VAL

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           IP ADDRESS DATA TYPES
 *
 * Note(s) : (1) 'NET_IP_ADDR' pre-defined in 'net_type.h' PRIOR to all other network modules that require
 *                   IP address data type.
 *
 *           (2) 'NET_IP_ADDRS_QTY_MAX'  SHOULD be #define'd based on 'NET_IP_ADDRS_QTY' data type declared.
 *******************************************************************************************************/

#if 0                                                           // See Note #1.
typedef  CPU_INT32U NET_IP_ADDR;                                // Defines IP IPv4 addr size.
#endif

#define  NET_IP_ADDRS_QTY_MIN                              1
#define  NET_IP_ADDRS_QTY_MAX            DEF_INT_08U_MAX_VAL    // See Note #2.

/********************************************************************************************************
 *                                       IP TX DESTINATION STATUS
 *******************************************************************************************************/

typedef  enum  net_ip_tx_dest_status {
  NET_IP_TX_DEST_STATUS_NONE,
  NET_IP_TX_DEST_STATUS_BROADCAST,
  NET_IP_TX_DEST_STATUS_MULTICAST,
  NET_IP_TX_DEST_STATUS_LOCAL_HOST,
  NET_IP_TX_DEST_STATUS_LOCAL_NET,
  NET_IP_TX_DEST_STATUS_DFLT_GATEWAY,
  NET_IP_TX_DEST_STATUS_NO_DFLT_GATEWAY
} NET_IP_TX_DEST_STATUS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 *******************************************************************************************************/

void NetIP_Init(MEM_SEG  *p_mem_seg,
                RTOS_ERR *p_err);

void NetIP_IF_Init(NET_IF_NBR if_nbr,
                   MEM_SEG    *p_mem_seg,
                   RTOS_ERR   *p_err);

CPU_BOOLEAN NetIP_IsAddrCfgd(NET_IF_NBR         if_nbr,
                             NET_IP_ADDR        *p_addr,
                             NET_IP_ADDR_FAMILY addr_type,
                             RTOS_ERR           *p_err);

CPU_BOOLEAN NetIP_IsDynAddrCfgd(NET_IF_NBR         if_nbr,
                                NET_IP_ADDR_FAMILY addr_type,
                                RTOS_ERR           *p_err);

CPU_BOOLEAN NetIP_IsAddrsCfgdOnIF(NET_IF_NBR if_nbr,
                                  RTOS_ERR   *p_err);

CPU_BOOLEAN NetIP_IsAddrValid(NET_IP_ADDR_FAMILY addr_type,
                              CPU_INT08U         *p_addr,
                              CPU_INT08U         addr_len);

CPU_BOOLEAN NetIP_IsAddrMulticast(NET_IP_ADDR_FAMILY addr_type,
                                  CPU_INT08U         *p_addr,
                                  CPU_INT08U         addr_len);

CPU_BOOLEAN NetIP_IsAddrConflict(NET_IF_NBR if_nbr);

CPU_BOOLEAN NetIP_InvalidateAddr(NET_IF_NBR         if_nbr,
                                 NET_IP_ADDR        *p_addr,
                                 NET_IP_ADDR_FAMILY addr_type,
                                 RTOS_ERR           *p_err);

void NetIP_GetHostAddr(NET_IF_NBR         if_nbr,
                       NET_IP_ADDR_FAMILY addr_type,
                       CPU_INT08U         *p_addr_tbl,
                       CPU_INT08U         *p_addr_tbl_qty,
                       CPU_INT08U         *p_addr_len,
                       RTOS_ERR           *p_err);

NET_IF_NBR NetIP_GetHostAddrIF_Nbr(NET_IP_ADDR_FAMILY addr_type,
                                   CPU_INT08U         *p_addr,
                                   CPU_INT08U         addr_len,
                                   RTOS_ERR           *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_IPv4_CFG_IF_MAX_NBR_ADDR
#error  "NET_IPv4_CFG_IF_MAX_NBR_ADDR        not #define'd in 'net_cfg.h'"
#error  "                              [MUST be  >= NET_IP_ADDRS_QTY_MIN]"
#error  "                              [     &&  <= NET_IP_ADDRS_QTY_MAX]"

#elif   (DEF_CHK_VAL(NET_IPv4_CFG_IF_MAX_NBR_ADDR, \
                     NET_IP_ADDRS_QTY_MIN,         \
                     NET_IP_ADDRS_QTY_MAX) != DEF_OK)
#error  "NET_IPv4_CFG_IF_MAX_NBR_ADDR  illegally #define'd in 'net_cfg.h'"
#error  "                              [MUST be  >= NET_IP_ADDRS_QTY_MIN]"
#error  "                              [     &&  <= NET_IP_ADDRS_QTY_MAX]"
#endif

#ifndef  NET_IPv6_CFG_IF_MAX_NBR_ADDR
#error  "NET_IPv6_CFG_IF_MAX_NBR_ADDR        not #define'd in 'net_cfg.h'"
#error  "                              [MUST be  >= NET_IP_ADDRS_QTY_MIN]"
#error  "                              [     &&  <= NET_IP_ADDRS_QTY_MAX]"

#elif   (DEF_CHK_VAL(NET_IPv6_CFG_IF_MAX_NBR_ADDR, \
                     NET_IP_ADDRS_QTY_MIN,         \
                     NET_IP_ADDRS_QTY_MAX) != DEF_OK)
#error  "NET_IPv6_CFG_IF_MAX_NBR_ADDR  illegally #define'd in 'net_cfg.h'"
#error  "                              [MUST be  >= NET_IP_ADDRS_QTY_MIN]"
#error  "                              [     &&  <= NET_IP_ADDRS_QTY_MAX]"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_IP_PRIV_H_
