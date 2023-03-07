/***************************************************************************//**
 * @file
 * @brief Network Arp Layer - (Address Resolution Protocol)
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
 * @defgroup NET_CORE_ARP ARP API
 * @ingroup  NET_CORE
 * @brief    ARP API
 *
 * @addtogroup NET_CORE_ARP
 * @{
 ********************************************************************************************************
 * @note     (1) Address Resolution Protocol ONLY required for network interfaces that require
 *               network-address-to-hardware-address bindings (see RFC #826 'Abstract').
 *
 * @note     (2) Supports Address Resolution Protocol as described in RFC #826 with the following
 *               restrictions/constraints :
 *             - (a) ONLY supports the following hardware types :
 *                   - (1) 48-bit Ethernet
 *             - (b) ONLY supports the following protocol types :
 *                   - (1) 32-bit IP
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_ARP_H_
#define  _NET_ARP_H_

#include  <net/include/net_cfg_net.h>

#ifdef   NET_ARP_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_stat.h>
#include  <net/include/net_type.h>

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void NetARP_CfgAddrFilterEn(CPU_BOOLEAN en);

void NetARP_TxReqGratuitous(NET_PROTOCOL_TYPE  protocol_type,
                            CPU_INT08U         *p_addr_protocol,
                            NET_CACHE_ADDR_LEN addr_protocol_len,
                            RTOS_ERR           *p_err);

//                                                                 ---------- CFG FNCTS -----------
CPU_BOOLEAN NetARP_CfgCacheTimeout(CPU_INT16U timeout_sec);

CPU_BOOLEAN NetARP_CfgCacheTxQ_MaxTh(NET_BUF_QTY nbr_buf_max);

CPU_BOOLEAN NetARP_CfgCacheAccessedTh(CPU_INT16U nbr_access);

CPU_BOOLEAN NetARP_CfgPendReqTimeout(CPU_INT08U timeout_sec);

CPU_BOOLEAN NetARP_CfgPendReqMaxRetries(CPU_INT08U max_nbr_retries);

CPU_BOOLEAN NetARP_CfgRenewReqTimeout(CPU_INT08U timeout_sec);

CPU_BOOLEAN NetARP_CfgRenewReqMaxRetries(CPU_INT08U max_nbr_retries);

//                                                                 --------- STATUS FNCTS ---------

CPU_BOOLEAN NetARP_IsAddrProtocolConflict(NET_IF_NBR if_nbr,
                                          RTOS_ERR   *p_err);

NET_CACHE_ADDR_LEN NetARP_CacheGetAddrHW(NET_IF_NBR         if_nbr,
                                         CPU_INT08U         *p_addr_hw,
                                         NET_CACHE_ADDR_LEN addr_hw_len_buf,
                                         CPU_INT08U         *p_addr_protocol,
                                         NET_CACHE_ADDR_LEN addr_protocol_len,
                                         RTOS_ERR           *p_err);

void NetARP_CacheProbeAddrOnNet(NET_PROTOCOL_TYPE  protocol_type,
                                CPU_INT08U         *p_addr_protocol_sender,
                                CPU_INT08U         *p_addr_protocol_target,
                                NET_CACHE_ADDR_LEN addr_protocol_len,
                                RTOS_ERR           *p_err);

//                                                                 ---- ARP CACHE STATUS FNCTS ----
NET_STAT_POOL NetARP_CachePoolStatGet(void);

void NetARP_CachePoolStatResetMaxUsed(void);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_ARP_MODULE_EN
#endif // _NET_ARP_H_
