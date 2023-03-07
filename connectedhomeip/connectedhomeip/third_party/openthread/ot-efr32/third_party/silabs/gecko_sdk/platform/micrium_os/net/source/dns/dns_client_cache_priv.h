/***************************************************************************//**
 * @file
 * @brief Network - DNS Client Cache Module
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

#ifndef  _DNS_CLIENT_CACHE_PRIV_H_
#define  _DNS_CLIENT_CACHE_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "dns_client_priv.h"

#include  "../../include/dns_client.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

struct DNSc_cache_item {
  DNSc_HOST_OBJ   *HostPtr;
  DNSc_CACHE_ITEM *NextPtr;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void DNScCache_Init(const DNSc_CFG *p_cfg,
                    DNSc_DATA      *p_dns_data,
                    MEM_SEG        *p_mem_seg,
                    RTOS_ERR       *p_err);

void DNScCache_Clr(void);

void DNScCache_HostInsert(DNSc_HOST_OBJ *p_host,
                          RTOS_ERR      *p_err);

void DNScCache_HostRemove(DNSc_HOST_OBJ *p_host);

void DNScCache_HostSrchRemove(CPU_CHAR *p_host_name,
                              RTOS_ERR *p_err);

DNSc_STATUS DNScCache_Srch(CPU_CHAR        *p_host_name,
                           NET_IP_ADDR_OBJ *p_addrs,
                           CPU_INT08U      addr_nbr_max,
                           CPU_INT08U      *p_addr_nbr_rtn,
                           DNSc_FLAGS      flags,
                           RTOS_ERR        *p_err);

DNSc_HOST_OBJ *DNScCache_HostObjGet(CPU_CHAR     *p_host_name,
                                    DNSc_FLAGS   flags,
                                    DNSc_REQ_CFG *p_cfg,
                                    RTOS_ERR     *p_err);

void DNScCache_HostObjFree(DNSc_HOST_OBJ *p_host);

void DNScCache_HostAddrInsert(DNSc_HOST_OBJ   *p_host,
                              NET_IP_ADDR_OBJ *p_addr,
                              RTOS_ERR        *p_err);

NET_HOST_IP_ADDR *DNScCache_HostAddrAlloc(RTOS_ERR *p_err);

DNSc_STATUS DNScCache_HostAddrsGet(CPU_CHAR         *p_host_name,
                                   NET_HOST_IP_ADDR **p_addrs,
                                   CPU_INT08U       *p_addr_nbr_rtn,
                                   DNSc_FLAGS       flags,
                                   RTOS_ERR         *p_err);

void DNScCache_HostAddrsFree(NET_HOST_IP_ADDR *p_addrs);

NET_IP_ADDR_OBJ *DNScCache_AddrObjGet(RTOS_ERR *p_err);

void DNScCache_AddrObjFree(NET_IP_ADDR_OBJ *p_addr);

void DNScCache_AddrObjSet(NET_IP_ADDR_OBJ *p_addr,
                          CPU_CHAR        *p_str_addr,
                          RTOS_ERR        *p_err);

DNSc_STATUS DNScCache_ResolveHost(DNSc_HOST_OBJ *p_host,
                                  RTOS_ERR      *p_err);

CPU_INT16U DNScCache_ResolveAll(RTOS_ERR *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _DNS_CLIENT_CACHE_PRIV_H_
