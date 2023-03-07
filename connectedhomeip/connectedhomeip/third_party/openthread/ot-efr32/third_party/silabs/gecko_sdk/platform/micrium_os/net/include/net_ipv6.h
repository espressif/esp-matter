/***************************************************************************//**
 * @file
 * @brief Network Ip Layer Version 6 - (Internet Protocol V6)
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
 * @defgroup NET_CORE_IPV6 IPv6 API
 * @ingroup  NET_CORE
 * @brief    IPv6 API
 *
 * @addtogroup NET_CORE_IPV6
 * @{
 ********************************************************************************************************
 * @note     (1) Supports Internet Protocol as described in RFC #2460, also known as IPv6, with the
 *               following restrictions/constraints :
 *             - (a) IPv6 forwarding/routing NOT currently supported       RFC #2460
 *             - (b) Transmit fragmentation  NOT currently supported       RFC #2460, Section 4.5
 *                                                                        'Fragment Header'
 *             - (c) IPv6 Security options   NOT           supported       RFC #4301
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_IPv6_H_
#define  _NET_IPv6_H_

#include  "net_cfg_net.h"

#ifdef   NET_IPv6_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_ip.h>
#include  <net/include/net_type.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           IPv6 FLAG DEFINES
 *******************************************************************************************************/

#define  NET_IPv6_FLAG_BLOCK_EN             DEF_BIT_01
#define  NET_IPv6_FLAG_DAD_EN               DEF_BIT_02

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                        IPv6 PREFIX ADDR TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IPv6_ADDR_PREFIX_TYPE;

/********************************************************************************************************
 *                                IPv6 AUTO CONFIGURATION ADDR ID TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IPv6_ADDR_ID_TYPE;

/********************************************************************************************************
 *                                    IPv6 ADDRESS SCOPE DATA TYPE
 *******************************************************************************************************/

typedef enum net_ipv6_scope {
  NET_IPv6_ADDR_SCOPE_RESERVED = 0u,
  NET_IPv6_ADDR_SCOPE_IF_LOCAL = 1u,
  NET_IPv6_ADDR_SCOPE_LINK_LOCAL = 2u,
  NET_IPv6_ADDR_SCOPE_ADMIN_LOCAL = 4u,
  NET_IPv6_ADDR_SCOPE_SITE_LOCAL = 5u,
  NET_IPv6_ADDR_SCOPE_ORG_LOCAL = 8u,
  NET_IPv6_ADDR_SCOPE_GLOBAL = 14u,
} NET_IPv6_SCOPE;

/********************************************************************************************************
 *                                       IPv6 ADDRESS DATA TYPES
 *
 * Note(s) : (1) 'NET_IPv6_ADDR' pre-defined in 'net_type.h' PRIOR to all other network modules that
 *                require IPv6 address data type.
 *******************************************************************************************************/

typedef  struct  net_ipv6_addr_obj NET_IPv6_ADDR_OBJ;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const NET_IPv6_ADDR NetIPv6_AddrAny;
extern const NET_IPv6_ADDR NetIPv6_AddrNone;
extern const NET_IPv6_ADDR NetIPv6_AddrWildcard;
extern const NET_IPv6_ADDR NetIPv6_AddrLoopback;
extern const NET_IPv6_ADDR NetIPv6_AddrLinkLocalAllNodes;
extern const NET_IPv6_ADDR NetIPv6_AddrLinkLocalAllRouters;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//                                                                 ------------ AUTO CFG FNCTS ------------
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
CPU_BOOLEAN NetIPv6_AddrAutoCfgEn(NET_IF_NBR  if_nbr,
                                  CPU_BOOLEAN dad_en,
                                  RTOS_ERR    *p_err);

CPU_BOOLEAN NetIPv6_AddrAutoCfgDis(NET_IF_NBR if_nbr,
                                   RTOS_ERR   *p_err);
#endif

//                                                                 -------------- CFG FNCTS ---------------
void NetIPv6_AddrSubscribe(NET_IPv6_ADDR_HOOK_FNCT fnct,
                           RTOS_ERR                *p_err);

void NetIPv6_AddrUnsubscribe(NET_IPv6_ADDR_HOOK_FNCT fnct);

CPU_BOOLEAN NetIPv6_CfgAddrAdd(NET_IF_NBR    if_nbr,
                               NET_IPv6_ADDR *p_addr,
                               CPU_INT08U    prefix_len,
                               NET_FLAGS     flags,
                               RTOS_ERR      *p_err);

CPU_BOOLEAN NetIPv6_CfgAddrRemove(NET_IF_NBR    if_nbr,
                                  NET_IPv6_ADDR *p_addr_host,
                                  RTOS_ERR      *p_err);

CPU_BOOLEAN NetIPv6_CfgAddrRemoveAll(NET_IF_NBR if_nbr,
                                     RTOS_ERR   *p_err);

CPU_BOOLEAN NetIPv6_CfgFragReasmTimeout(CPU_INT08U timeout_sec);

//                                                                 -------------- GET FNCTS ---------------
CPU_BOOLEAN NetIPv6_GetAddrHost(NET_IF_NBR       if_nbr,
                                NET_IPv6_ADDR    *p_addr_tbl,
                                NET_IP_ADDRS_QTY *p_addr_tbl_qty,
                                RTOS_ERR         *p_err);

const NET_IPv6_ADDR_OBJ *NetIPv6_GetAddrSrc(NET_IF_NBR          *p_if_nbr,
                                            const NET_IPv6_ADDR *p_addr_src,
                                            const NET_IPv6_ADDR *p_addr_dest,
                                            NET_IPv6_ADDR       *p_addr_nexthop,
                                            RTOS_ERR            *p_err);

CPU_INT08U NetIPv6_GetAddrMatchingLen(const NET_IPv6_ADDR *p_addr_1,
                                      const NET_IPv6_ADDR *p_addr_2,
                                      RTOS_ERR            *p_err);

NET_IPv6_SCOPE NetIPv6_GetAddrScope(const NET_IPv6_ADDR *p_addr);

//                                                                 ------------- STATUS FNCTS -------------
CPU_BOOLEAN NetIPv6_IsAddrHostCfgd(const NET_IPv6_ADDR *p_addr);

CPU_BOOLEAN NetIPv6_IsAddrsCfgdOnIF(NET_IF_NBR if_nbr,
                                    RTOS_ERR   *p_err);

CPU_BOOLEAN NetIPv6_IsValidAddrHost(const NET_IPv6_ADDR *p_addr_host);

CPU_BOOLEAN NetIPv6_IsAddrLinkLocal(const NET_IPv6_ADDR *p_addr);

CPU_BOOLEAN NetIPv6_IsAddrSiteLocal(const NET_IPv6_ADDR *p_addr);

CPU_BOOLEAN NetIPv6_IsAddrMcast(const NET_IPv6_ADDR *p_addr);

CPU_BOOLEAN NetIPv6_IsAddrMcastAllRouters(const NET_IPv6_ADDR *p_addr);

CPU_BOOLEAN NetIPv6_IsAddrMcastAllNodes(const NET_IPv6_ADDR *p_addr);

CPU_BOOLEAN NetIPv6_IsAddrMcastSolNode(const NET_IPv6_ADDR *p_addr,
                                       const NET_IPv6_ADDR *p_addr_input);

CPU_BOOLEAN NetIPv6_IsAddrMcastRsvd(const NET_IPv6_ADDR *p_addr);

CPU_BOOLEAN NetIPv6_IsAddrUnspecified(const NET_IPv6_ADDR *p_addr);

CPU_BOOLEAN NetIPv6_IsAddrLoopback(const NET_IPv6_ADDR *p_addr);

NET_IPv6_ADDR_TYPE NetIPv6_AddrTypeValidate(const NET_IPv6_ADDR *p_addr,
                                            NET_IF_NBR          if_nbr);

//                                                                 ------------- SETUP FNCTS --------------
CPU_INT08U NetIPv6_CreateIF_ID(NET_IF_NBR            if_nbr,
                               NET_IPv6_ADDR         *p_if_ID,
                               NET_IPv6_ADDR_ID_TYPE id_type,
                               RTOS_ERR              *p_err);

void NetIPv6_CreateAddrFromID(NET_IPv6_ADDR             *p_addr_ID,
                              NET_IPv6_ADDR             *p_addr_prefix,
                              NET_IPv6_ADDR_PREFIX_TYPE prefix_type,
                              CPU_SIZE_T                prefix_len,
                              RTOS_ERR                  *p_err);

void NetIPv6_MaskGet(NET_IPv6_ADDR *p_mask_rtn,
                     CPU_INT08U    prefix_len,
                     RTOS_ERR      *p_err);

void NetIPv6_AddrMaskByPrefixLen(const NET_IPv6_ADDR *p_addr,
                                 CPU_INT08U          prefix_len,
                                 NET_IPv6_ADDR       *p_addr_rtn,
                                 RTOS_ERR            *p_err);

void NetIPv6_AddrMask(const NET_IPv6_ADDR *p_addr,
                      const NET_IPv6_ADDR *p_mask,
                      NET_IPv6_ADDR       *p_addr_rtn);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IPv6_MODULE_EN
#endif // _NET_IPv6_H_
