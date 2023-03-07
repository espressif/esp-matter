/***************************************************************************//**
 * @file
 * @brief Network Ip Layer Version 4 - (Internet Protocol V4)
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
 * @defgroup NET_CORE_IPV4 IPv4 API
 * @ingroup  NET_CORE
 * @brief    IPv4 API
 *
 * @addtogroup NET_CORE_IPV4
 * @{
 ********************************************************************************************************
 * @note     (1) Supports Internet Protocol as described in RFC #791, also known as IPv4, with the
 *               following restrictions/constraints :
 *             - (a) ONLY supports a single default gateway                RFC #1122, Section 3.3.1
 *                       per interface
 *             - (b) IP forwarding/routing  NOT currently supported        RFC #1122, Sections 3.3.1,
 *                                                                                     3.3.4 & 3.3.5
 *             - (c) Transmit fragmentation NOT currently supported        RFC # 791, Section 2.3
 *                                                                                     'Fragmentation &
 *                                                                                        Reassembly'
 *             - (d) IP Security options    NOT           supported        RFC #1108
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_IPv4_H_
#define  _NET_IPv4_H_

#include  "net_cfg_net.h"

#ifdef   NET_IPv4_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_ip.h>
#include  <net/include/net_type.h>

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  net_ipv4_addr_obj {
  NET_IPv4_ADDR        AddrHost;
  NET_IPv4_ADDR        AddrHostSubnetMask;
  NET_IPv4_ADDR        AddrHostSubnetMaskHost;
  NET_IPv4_ADDR        AddrHostSubnetNet;
  NET_IPv4_ADDR        AddrDfltGateway;
  NET_IP_ADDR_CFG_MODE CfgMode;
  CPU_BOOLEAN          IsValid;
  SLIST_MEMBER         ListNode;
} NET_IPv4_ADDR_OBJ;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

CPU_BOOLEAN NetIPv4_CfgAddrAdd(NET_IF_NBR    if_nbr,
                               NET_IPv4_ADDR addr_host,
                               NET_IPv4_ADDR addr_subnet_mask,
                               NET_IPv4_ADDR addr_dflt_gateway,
                               RTOS_ERR      *p_err);

CPU_BOOLEAN NetIPv4_CfgAddrRemove(NET_IF_NBR    if_nbr,
                                  NET_IPv4_ADDR addr_host,
                                  RTOS_ERR      *p_err);

CPU_BOOLEAN NetIPv4_CfgAddrRemoveAll(NET_IF_NBR if_nbr,
                                     RTOS_ERR   *p_err);

#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
void NetIPv4_AddrLinkLocalCfg(NET_IF_NBR                        if_nbr,
                              NET_IPv4_LINK_LOCAL_COMPLETE_HOOK hook,
                              RTOS_ERR                          *p_err);

void NetIPv4_AddrLinkLocalCfgRemove(NET_IF_NBR if_nbr,
                                    RTOS_ERR   *p_err);
#endif

CPU_BOOLEAN NetIPv4_CfgFragReasmTimeout(CPU_INT08U timeout_sec);

CPU_BOOLEAN NetIPv4_GetAddrHost(NET_IF_NBR       if_nbr,
                                NET_IPv4_ADDR    *p_addr_tbl,
                                NET_IP_ADDRS_QTY *p_addr_tbl_qty,
                                RTOS_ERR         *p_err);

NET_IPv4_ADDR NetIPv4_GetAddrSrc(NET_IPv4_ADDR addr_remote,
                                 RTOS_ERR      *p_err);

NET_IPv4_ADDR NetIPv4_GetAddrSubnetMask(NET_IPv4_ADDR addr,
                                        RTOS_ERR      *p_err);

NET_IPv4_ADDR NetIPv4_GetAddrDfltGateway(NET_IPv4_ADDR addr,
                                         RTOS_ERR      *p_err);

CPU_BOOLEAN NetIPv4_IsAddrClassA(NET_IPv4_ADDR addr);

CPU_BOOLEAN NetIPv4_IsAddrClassB(NET_IPv4_ADDR addr);

CPU_BOOLEAN NetIPv4_IsAddrClassC(NET_IPv4_ADDR addr);

CPU_BOOLEAN NetIPv4_IsAddrClassD(NET_IPv4_ADDR addr);

CPU_BOOLEAN NetIPv4_IsAddrThisHost(NET_IPv4_ADDR addr);

CPU_BOOLEAN NetIPv4_IsAddrLocalHost(NET_IPv4_ADDR addr);

CPU_BOOLEAN NetIPv4_IsAddrLocalLink(NET_IPv4_ADDR addr);

CPU_BOOLEAN NetIPv4_IsAddrBroadcast(NET_IPv4_ADDR addr);

CPU_BOOLEAN NetIPv4_IsAddrMulticast(NET_IPv4_ADDR addr);

CPU_BOOLEAN NetIPv4_IsAddrHost(NET_IPv4_ADDR addr);

CPU_BOOLEAN NetIPv4_IsAddrHostCfgd(NET_IPv4_ADDR addr);

CPU_BOOLEAN NetIPv4_IsAddrsCfgdOnIF(NET_IF_NBR if_nbr,
                                    RTOS_ERR   *p_err);

CPU_BOOLEAN NetIPv4_IsValidAddrHost(NET_IPv4_ADDR addr_host);

CPU_BOOLEAN NetIPv4_IsValidAddrHostCfgd(NET_IPv4_ADDR addr_host,
                                        NET_IPv4_ADDR addr_subnet_mask);

CPU_BOOLEAN NetIPv4_IsValidAddrSubnetMask(NET_IPv4_ADDR addr_subnet_mask);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IPv4_MODULE_EN
#endif // _NET_IPv4_H_
