/***************************************************************************//**
 * @file
 * @brief Network - DNS Client
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
 * @note     This file implements a basic DNS client based on RFC #1035. It provides the
 *           mechanism used to retrieve an IP address from a given host name.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _DNS_CLIENT_PRIV_H_
#define  _DNS_CLIENT_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../tcpip/net_priv.h"

#include  "../../include/dns_client.h"
#include  "../../include/net_sock.h"
#include  "../../include/net_type.h"

#include  <cpu/include/cpu.h>

#include  <net_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                         CONFIGURATION ERROR
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_DNSc_CFG_MODE_ASYNC_EN
#define  NET_DNSc_CFG_MODE_ASYNC_EN  DEF_DISABLED
#endif

#ifndef  NET_DNSc_CFG_MODE_BLOCK_EN
#define  NET_DNSc_CFG_MODE_BLOCK_EN  DEF_DISABLED
#endif

#if  (NET_DNSc_CFG_MODE_ASYNC_EN == DEF_ENABLED)
    #define  DNSc_TASK_MODULE_EN

    #ifndef  NET_DNSc_CFG_MODE_BLOCK_EN
        #error  "NET_DNSc_CFG_MODE_BLOCK_EN not #define'd in 'net_cfg.h' [MUST be  DEF_DISABLED ||  DEF_ENABLED ] "
    #elif  ((NET_DNSc_CFG_MODE_BLOCK_EN != DEF_ENABLED) \
  && (NET_DNSc_CFG_MODE_BLOCK_EN != DEF_DISABLED))
        #error  "NET_DNSc_CFG_MODE_BLOCK_EN not #define'd in 'net_cfg.h' [MUST be  DEF_DISABLED ||  DEF_ENABLED ] "
    #endif

    #if  (NET_DNSc_CFG_MODE_BLOCK_EN == DEF_ENABLED)
        #define  DNSc_SIGNAL_TASK_MODULE_EN
    #endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  enum  dnsc_server_addr_type {
  DNSc_SERVER_ADDR_TYPE_NONE,
  DNSc_SERVER_ADDR_TYPE_STATIC,
  DNSc_SERVER_ADDR_TYPE_AUTO
} DNSc_SERVER_ADDR_TYPE;

typedef  enum DNSc_state {
  DNSc_STATE_FREE = 0,
  DNSc_STATE_INIT_REQ,
  DNSc_STATE_IF_SEL,
  DNSc_STATE_TX_REQ_IPv4,
  DNSc_STATE_RX_RESP_IPv4,
  DNSc_STATE_TX_REQ_IPv6,
  DNSc_STATE_RX_RESP_IPv6,
  DNSc_STATE_RESOLVED,
  DNSc_STATE_FAILED,
} DNSc_STATE;

typedef  struct  DNSc_addr_item DNSc_ADDR_ITEM;
struct  DNSc_addr_item {
  NET_IP_ADDR_OBJ *AddrPtr;
  DNSc_ADDR_ITEM  *NextPtr;
};

#define  DNSc_QUERY_ID_NONE     0u

typedef  struct  DNSc_server {
  NET_IP_ADDR_OBJ AddrObj;
  CPU_BOOLEAN     IsValid;
} DNSc_SERVER;

typedef  struct  DNSc_cache_item DNSc_CACHE_ITEM;

typedef  struct  DNSc_host {
  CPU_CHAR       *NamePtr;
  CPU_INT16U     NameLenMax;
  DNSc_STATE     State;
  DNSc_ADDR_ITEM *AddrsFirstPtr;
  DNSc_ADDR_ITEM *AddrsEndPtr;
  CPU_INT08U     AddrsCount;
  CPU_INT08U     AddrsIPv4Count;
  CPU_INT08U     AddrsIPv6Count;
  CPU_INT08U     ReqCtr;
  NET_SOCK_ID    SockID;
  NET_IF_NBR     IF_Nbr;
  CPU_INT16U     QueryID;
  NET_TS_MS      TS_ms;
  DNSc_REQ_CFG   *ReqCfgPtr;
#ifdef  DNSc_SIGNAL_TASK_MODULE_EN
  KAL_SEM_HANDLE TaskSignal;
#endif
} DNSc_HOST_OBJ;

typedef  struct  dnsc_data {
  CPU_INT08U      CfgOpDly_ms;
  CPU_BOOLEAN     CfgDfltServerAddrFallbackEn;
  CPU_SIZE_T      CfgAddrIPv4MaxPerHost;
  CPU_SIZE_T      CfgAddrIPv6MaxPerHost;
  CPU_INT08U      CfgReqRetryNbrMax;
  CPU_INT16U      CfgReqRetryTimeout_ms;

  DNSc_SERVER     ServerAddrStatic;
  DNSc_SERVER     ServerAddrAuto;

  KAL_LOCK_HANDLE CacheLockHandle;
  DNSc_CACHE_ITEM *CacheItemListHead;
  MEM_DYN_POOL    CacheItemPool;
  MEM_DYN_POOL    CacheHostObjPool;
  MEM_DYN_POOL    CacheHostNamePool;
  MEM_DYN_POOL    CacheAddrItemPool;
  MEM_DYN_POOL    CacheAddrObjPool;
  MEM_DYN_POOL    CacheHostAddrObjPool;

#ifdef  DNSc_TASK_MODULE_EN
  KAL_TASK_HANDLE TaskHandle;
  KAL_SEM_HANDLE  TaskSignalHandle;
#endif
} DNSc_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void DNSc_Init(const DNSc_CFG *p_cfg,
               MEM_SEG        *p_mem_seg,
               RTOS_ERR       *p_err);

void DNSc_SetServerAddrInternal(NET_IP_ADDR_OBJ *p_server_addr,
                                RTOS_ERR        *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _DNS_CLIENT_PRIV_H_
