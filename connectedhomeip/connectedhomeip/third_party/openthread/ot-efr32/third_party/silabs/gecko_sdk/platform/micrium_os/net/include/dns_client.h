/***************************************************************************//**
 * @file
 * @brief Network - Dns Client Module
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
 * @defgroup NET_CORE_DNSC DNS Client API
 * @ingroup  NET_CORE
 * @brief    DNS Client API
 *
 * @addtogroup NET_CORE_DNSC
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _DNS_CLIENT_H_
#define  _DNS_CLIENT_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_type.h>
#include  <net/include/net_ip.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_types.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                         DEFAULT CFG DEFINES
 *******************************************************************************************************/

#define  DNS_CLIENT_CFG_SERVER_ADDR_DFLT                        "8.8.8.8"
#define  DNS_CLIENT_CFG_HOSTNAME_LEN_MAX_DFLT                   255u
#define  DNS_CLIENT_CFG_CACHE_ENTRIES_NBR_MAX_DFLT              LIB_MEM_BLK_QTY_UNLIMITED
#define  DNS_CLIENT_CFG_ADDR_IPv4_PER_HOST_NBR_MAX_DFLT         LIB_MEM_BLK_QTY_UNLIMITED
#define  DNS_CLIENT_CFG_ADDR_IPv6_PER_HOST_NBR_MAX_DFLT         LIB_MEM_BLK_QTY_UNLIMITED
#define  DNS_CLIENT_CFG_OP_DLY_MS_DFLT                          100u
#define  DNS_CLIENT_CFG_REQ_RETRY_NBR_MAX_DFLT                  3u
#define  DNS_CLIENT_CFG_REQ_RETRY_TIMEOUT_MS_DFLT               5000u

#define  DNSc_DFLT_CFG  {                                                 \
    .HostNameLenMax = DNS_CLIENT_CFG_HOSTNAME_LEN_MAX_DFLT,               \
    .CacheEntriesMaxNbr = DNS_CLIENT_CFG_CACHE_ENTRIES_NBR_MAX_DFLT,      \
    .AddrIPv4MaxPerHost = DNS_CLIENT_CFG_ADDR_IPv4_PER_HOST_NBR_MAX_DFLT, \
    .AddrIPv6MaxPerHost = DNS_CLIENT_CFG_ADDR_IPv6_PER_HOST_NBR_MAX_DFLT, \
    .OpDly_ms = DNS_CLIENT_CFG_OP_DLY_MS_DFLT,                            \
    .ReqRetryNbrMax = DNS_CLIENT_CFG_REQ_RETRY_NBR_MAX_DFLT,              \
    .ReqRetryTimeout_ms = DNS_CLIENT_CFG_REQ_RETRY_TIMEOUT_MS_DFLT,       \
    .DfltServerAddrFallbackEn = DEF_YES                                   \
}

/********************************************************************************************************
 *                                            FLAG DEFINES
 *******************************************************************************************************/

typedef  CPU_INT08U DNSc_FLAGS;

#define  DNSc_FLAG_NONE              DEF_BIT_NONE
#define  DNSc_FLAG_NO_BLOCK          DEF_BIT_00
#define  DNSc_FLAG_FORCE_CACHE       DEF_BIT_01
#define  DNSc_FLAG_FORCE_RENEW       DEF_BIT_02
#define  DNSc_FLAG_FORCE_RESOLUTION  DEF_BIT_03
#define  DNSc_FLAG_IPv4_ONLY         DEF_BIT_04
#define  DNSc_FLAG_IPv6_ONLY         DEF_BIT_05

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                         DNS CFG DATA TYPES
 * @brief DNS client configuration
 *******************************************************************************************************/

typedef  RTOS_TASK_CFG DNSc_CFG_TASK;

typedef  struct DNSc_cfg {
  CPU_INT16U  HostNameLenMax;

  CPU_SIZE_T  CacheEntriesMaxNbr;

  CPU_SIZE_T  AddrIPv4MaxPerHost;
  CPU_SIZE_T  AddrIPv6MaxPerHost;

  CPU_INT08U  ReqRetryNbrMax;
  CPU_INT16U  ReqRetryTimeout_ms;

  CPU_INT08U  OpDly_ms;

  CPU_BOOLEAN DfltServerAddrFallbackEn;
} DNSc_CFG;

/********************************************************************************************************
 *                                         DNS REQ CFG DATA TYPE
 * @brief DNS client request configuration
 *******************************************************************************************************/

typedef  struct  DNSc_req_cfg {
  NET_IP_ADDR_OBJ *ServerAddrPtr;
  NET_PORT_NBR    ServerPort;
  CPU_INT16U      OpDly_ms;
  CPU_INT16U      ReqTimeout_ms;
  CPU_INT08U      ReqRetry;
} DNSc_REQ_CFG;

/********************************************************************************************************
 *                                         DNS STATUS DATA TYPE
 * @brief DNS client status
 *******************************************************************************************************/

typedef  enum  DNSc_status {
  DNSc_STATUS_PENDING,
  DNSc_STATUS_RESOLVED,
  DNSc_STATUS_FAILED,
  DNSc_STATUS_UNKNOWN,
  DNSc_STATUS_NONE,
} DNSc_STATUS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void DNSc_CfgServerByStr(CPU_CHAR *p_server,
                         RTOS_ERR *p_err);

void DNSc_CfgServerByAddr(NET_IP_ADDR_OBJ *p_addr,
                          RTOS_ERR        *p_err);

void DNSc_GetServerByStr(CPU_CHAR   *p_str,
                         CPU_INT08U str_len_max,
                         RTOS_ERR   *p_err);

void DNSc_GetServerByAddr(NET_IP_ADDR_OBJ *p_addr,
                          RTOS_ERR        *p_err);

DNSc_STATUS DNSc_GetHost(CPU_CHAR        *p_host_name,
                         NET_IP_ADDR_OBJ *p_addr_obj,
                         CPU_INT08U      *p_addr_nbr,
                         DNSc_FLAGS      flags,
                         DNSc_REQ_CFG    *p_cfg,
                         RTOS_ERR        *p_err);

DNSc_STATUS DNSc_GetHostAddrs(CPU_CHAR         *p_host_name,
                              NET_HOST_IP_ADDR **p_addrs,
                              CPU_INT08U       *p_addr_nbr,
                              DNSc_FLAGS       flags,
                              DNSc_REQ_CFG     *p_cfg,
                              RTOS_ERR         *p_err);

void DNSc_FreeHostAddrs(NET_HOST_IP_ADDR *p_addr);

void DNSc_CacheClrAll(RTOS_ERR *p_err);

void DNSc_CacheClrHost(CPU_CHAR *p_host_name,
                       RTOS_ERR *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _DNS_CLIENT_H_
