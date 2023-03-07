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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

#include  <net/include/net_cfg_net.h>

#ifdef  NET_DNS_CLIENT_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  "dns_client_priv.h"
#include  "dns_client_req_priv.h"
#include  "dns_client_cache_priv.h"
#include  "dns_client_task_priv.h"

#include  <net/include/dns_client.h>
#include  <net/include/net_ascii.h>

#include  <net/source/cmd/dns_client_cmd_priv.h>
#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (NET, DNS)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_NET_APP

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

DNSc_DATA *DNSc_DataPtr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         DNSc_CfgServerByStr()
 *
 * @brief    Configures the DNS server to use by default using a string.
 *
 * @param    p_server    Pointer to a string that contains the IP address of the DNS server.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NET_STR_ADDR_INVALID
 *******************************************************************************************************/
void DNSc_CfgServerByStr(CPU_CHAR *p_server,
                         RTOS_ERR *p_err)
{
  NET_IP_ADDR_OBJ ip_addr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_server != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  DNScCache_AddrObjSet(&ip_addr, p_server, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  DNScReq_ServerSet(DNSc_SERVER_ADDR_TYPE_STATIC, &ip_addr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                        DNSc_CfgServerByAddr()
 *
 * @brief    Gets the default DNS server in a address object format.
 *
 * @param    p_addr  Pointer to structure that will receive the IP address of the DNS server.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *******************************************************************************************************/
void DNSc_CfgServerByAddr(NET_IP_ADDR_OBJ *p_addr,
                          RTOS_ERR        *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  switch (p_addr->AddrLen) {
    case NET_IPv4_ADDR_SIZE:
    case NET_IPv6_ADDR_SIZE:
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  DNScReq_ServerSet(DNSc_SERVER_ADDR_TYPE_STATIC, p_addr);
}

/****************************************************************************************************//**
 *                                         DNSc_GetServerByStr()
 *
 * @brief    Gets the default DNS server in string format.
 *
 * @param    p_str           Pointer to string that will receive the IP address of the DNS server.
 *
 * @param    str_len_max     Maximum string length.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *******************************************************************************************************/
void DNSc_GetServerByStr(CPU_CHAR   *p_str,
                         CPU_INT08U str_len_max,
                         RTOS_ERR   *p_err)
{
  NET_IP_ADDR_OBJ addr;
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR *srv_addr = DEF_NULL;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_str != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  DNScReq_ServerGet(&addr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#if !defined(NET_IPv4_MODULE_EN)
  if (addr.AddrLen == NET_IPv4_ADDR_LEN) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
  }
#endif

#if !defined(NET_IPv6_MODULE_EN)
  if (addr.AddrLen == NET_IPv6_ADDR_LEN) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
  }
#endif

  switch (addr.AddrLen) {
    case NET_IPv4_ADDR_LEN:
#ifdef  NET_IPv4_MODULE_EN
      RTOS_ASSERT_DBG_ERR_SET((str_len_max >= NET_ASCII_LEN_MAX_ADDR_IPv4), *p_err, RTOS_ERR_INVALID_ARG,; );

      srv_addr = (NET_IPv4_ADDR *) &addr.Addr.Array[0];
      NetASCII_IPv4_to_Str(*srv_addr, p_str, DEF_NO, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

    case NET_IPv6_ADDR_LEN:
#ifdef  NET_IPv6_MODULE_EN
      RTOS_ASSERT_DBG_ERR_SET((str_len_max >= NET_ASCII_LEN_MAX_ADDR_IPv6), *p_err, RTOS_ERR_INVALID_ARG,; );

      NetASCII_IPv6_to_Str((NET_IPv6_ADDR *)&addr.Addr.Array[0], p_str, DEF_NO, DEF_NO, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        ;
        goto exit;
      }
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                        DNSc_CfgServerByAddr()
 *
 * @brief    Gets the default DNS server in a address object format.
 *
 * @param    p_addr  Pointer to structure that will receive the IP address of the DNS server.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *******************************************************************************************************/
void DNSc_GetServerByAddr(NET_IP_ADDR_OBJ *p_addr,
                          RTOS_ERR        *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  DNScReq_ServerGet(p_addr, p_err);
}

/****************************************************************************************************//**
 *                                            DNSc_GetHost()
 *
 * @brief    Converts a string representation of a host name to its corresponding IP address using DNS
 *           service.
 *
 * @param    p_host_name     Pointer to a string that contains the host name.
 *
 * @param    p_addr_tbl      Pointer to array that will receive the IP address(es).
 *
 * @param    p_addr_nbr      Pointer to a variable that contains how many addresses can be contained
 *                           in the array.
 *
 * @param    flags           DNS client flag:
 *                               - DNSc_FLAG_NONE              By default, this function is blocking.
 *                               - DNSc_FLAG_NO_BLOCK          Do not block (only possible if DNSc's task
 *                                                             is enabled).
 *                               - DNSc_FLAG_FORCE_CACHE       Take host from the cache, do not send new
 *                                                             DNS request.
 *                               - DNSc_FLAG_FORCE_RENEW       Force DNS request and remove existing
 *                                                             entry in the cache.
 *                               - DNSc_FLAG_FORCE_RESOLUTION  Force DNS to resolve given host name.
 *                               - DNSc_FLAG_IPv4_ONLY         Return only the IPv4 address(es).
 *                               - DNSc_FLAG_IPv6_ONLY         Return only the IPv6 address(es).
 *
 * @param    p_cfg           Pointer to a request configuration. Should be set to overwrite the default
 *                           DNS configuration.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @return   Resolution status:
 *                 - DNSc_STATUS_PENDING         Host resolution is pending, call again to see the
 *                                               status. (Processed by DNSc's task)
 *                 - DNSc_STATUS_RESOLVED        Host is resolved.
 *                 - DNSc_STATUS_FAILED          Host resolution has failed.
 *******************************************************************************************************/
DNSc_STATUS DNSc_GetHost(CPU_CHAR        *p_host_name,
                         NET_IP_ADDR_OBJ *p_addr_tbl,
                         CPU_INT08U      *p_addr_nbr,
                         DNSc_FLAGS      flags,
                         DNSc_REQ_CFG    *p_cfg,
                         RTOS_ERR        *p_err)
{
  DNSc_HOST_OBJ      *p_host;
  NET_IP_ADDR_FAMILY ip_family;
  DNSc_STATUS        status = DNSc_STATUS_FAILED;
  CPU_BOOLEAN        flag_set = DEF_NO;
  CPU_INT08U         addr_nbr = *p_addr_nbr;
  RTOS_ERR           local_err;

  //                                                               ------------------ VALIDATE ARGS -------------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, status);
  RTOS_ASSERT_DBG_ERR_SET((DNSc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT, status);
  RTOS_ASSERT_DBG_ERR_SET((p_host_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, status);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, status);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_nbr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, status);
  RTOS_ASSERT_DBG_ERR_SET((*p_addr_nbr > 0u), *p_err, RTOS_ERR_INVALID_ARG, status);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
#ifndef  DNSc_TASK_MODULE_EN
  if (DEF_BIT_IS_SET(flags, DNSc_FLAG_NO_BLOCK)) {
#ifndef DNSc_SIGNAL_TASK_MODULE_EN
    RTOS_ASSERT_DBG(DEF_FAIL, RTOS_ERR_INVALID_CFG, status);
#endif
  }

  if (DEF_BIT_IS_SET(flags, DNSc_FLAG_FORCE_CACHE)
      && DEF_BIT_IS_SET(flags, DNSc_FLAG_FORCE_RENEW)) {
    RTOS_ASSERT_DBG(DEF_FAIL, RTOS_ERR_INVALID_CFG, status);
  }
#endif
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  flag_set = DEF_BIT_IS_SET(flags, DNSc_FLAG_FORCE_RESOLUTION);
  if (flag_set == DEF_NO) {
    //                                                             First check to see if the incoming host name is
    //                                                             simply a decimal-dot-formatted IP address. If it
    //                                                             is, then just convert it and return.
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    ip_family = NetASCII_Str_to_IP(p_host_name,
                                   &p_addr_tbl->Addr.Array[0],
                                   sizeof(p_addr_tbl[0].Addr),
                                   &local_err);
    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
      switch (ip_family) {
        case NET_IP_ADDR_FAMILY_IPv4:
          p_addr_tbl[0].AddrLen = NET_IPv4_ADDR_LEN;
          status = DNSc_STATUS_RESOLVED;
          *p_addr_nbr = 1u;
          goto exit;

        case NET_IP_ADDR_FAMILY_IPv6:
          p_addr_tbl[0].AddrLen = NET_IPv6_ADDR_LEN;
          status = DNSc_STATUS_RESOLVED;
          *p_addr_nbr = 1u;
          goto exit;

        default:
          break;
      }
    }
  }

  flag_set = DEF_BIT_IS_SET(flags, DNSc_FLAG_FORCE_CACHE);
  if (flag_set == DEF_YES) {
    status = DNScCache_Srch(p_host_name, p_addr_tbl, addr_nbr, p_addr_nbr, flags, p_err);
    goto exit;
  }

  flag_set = DEF_BIT_IS_SET(flags, DNSc_FLAG_FORCE_RENEW);
  if (flag_set == DEF_NO) {
    //                                                             ---------- SRCH IN EXISTING CACHE ENTRIES ----------
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    status = DNScCache_Srch(p_host_name, p_addr_tbl, addr_nbr, p_addr_nbr, flags, &local_err);
    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:
        switch (status) {
          case DNSc_STATUS_PENDING:
          case DNSc_STATUS_RESOLVED:
            goto exit;

          case DNSc_STATUS_FAILED:
            DNScCache_HostSrchRemove(p_host_name, &local_err);
            break;

          default:
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DNSc_STATUS_FAILED);
        }
        break;

      case RTOS_ERR_NOT_FOUND:
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DNSc_STATUS_FAILED);
    }
  } else {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    DNScCache_HostSrchRemove(p_host_name, &local_err);
  }

  //                                                               ----------- ACQUIRE HOST OBJ FOR THE REQ -----------
  p_host = DNScCache_HostObjGet(p_host_name, flags, p_cfg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    status = DNSc_STATUS_FAILED;
    goto exit;
  }

  status = DNScTask_HostResolve(p_host, flags, p_cfg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  switch (status) {
    case DNSc_STATUS_PENDING:
      goto exit;

    case DNSc_STATUS_RESOLVED:
    case DNSc_STATUS_UNKNOWN:
      break;

    case DNSc_STATUS_FAILED:
      goto exit_free_host;

    default:
      DNScCache_HostObjFree(p_host);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DNSc_STATUS_FAILED);
  }

  status = DNScCache_Srch(p_host_name, p_addr_tbl, addr_nbr, p_addr_nbr, flags, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_free_host;
  }

  goto exit;

exit_free_host:
  DNScCache_HostObjFree(p_host);

exit:
  return (status);
}

/****************************************************************************************************//**
 *                                          DNS_HostAddrsGet()
 *
 * @brief    Get a list of IP addresses assigned to a host.
 *
 * @param    p_host_name     Pointer to a string that contains the host name.
 *
 * @param    p_addr_tbl      Pointer to link list that will receive the IP address(es).
 *
 * @param    p_addr_nbr      Pointer to a variable that contains how many addresses can be returned
 *
 * @param    flags           DNS client flag:
 *                               - DNSc_FLAG_NONE              By default, this function is blocking.
 *                               - DNSc_FLAG_NO_BLOCK          Do not block (only possible if DNSc's task
 *                                                             is enabled).
 *                               - DNSc_FLAG_FORCE_CACHE       Take host from the cache, do not send new
 *                                                             DNS request.
 *                               - DNSc_FLAG_FORCE_RENEW       Force DNS request and remove existing
 *                                                             entry in the cache.
 *                               - DNSc_FLAG_FORCE_RESOLUTION  Force DNS to resolve given host name.
 *                               - DNSc_FLAG_IPv4_ONLY         Return only the IPv4 address(es).
 *                               - DNSc_FLAG_IPv6_ONLY         Return only the IPv6 address(es).
 *
 * @param    p_cfg           Pointer to a request configuration. Should be set to overwrite the default
 *                           DNS configuration.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function.
 *
 * @return   Resolution status:
 *                 - DNSc_STATUS_PENDING         Host resolution is pending, call again to see the
 *                                               status. (Processed by DNSc's task)
 *                 - DNSc_STATUS_RESOLVED        Host is resolved.
 *                 - DNSc_STATUS_FAILED          Host resolution has failed.
 *******************************************************************************************************/
DNSc_STATUS DNSc_GetHostAddrs(CPU_CHAR         *p_host_name,
                              NET_HOST_IP_ADDR **p_addrs,
                              CPU_INT08U       *p_addr_nbr,
                              DNSc_FLAGS       flags,
                              DNSc_REQ_CFG     *p_cfg,
                              RTOS_ERR         *p_err)
{
  NET_IP_ADDR_FAMILY ip_family;
  DNSc_STATUS        status = DNSc_STATUS_FAILED;
  CPU_BOOLEAN        flag_set = DEF_NO;
  DNSc_HOST_OBJ      *p_host;
  RTOS_ERR           err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, status);
  RTOS_ASSERT_DBG_ERR_SET((p_host_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, status);
  RTOS_ASSERT_DBG_ERR_SET((p_addrs != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, status);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_nbr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, status);
  RTOS_ASSERT_DBG_ERR_SET((*p_addr_nbr > 0u), *p_err, RTOS_ERR_INVALID_ARG, status);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
#ifndef  DNSc_TASK_MODULE_EN
  if (DEF_BIT_IS_SET(flags, DNSc_FLAG_NO_BLOCK)) {
#ifndef DNSc_SIGNAL_TASK_MODULE_EN
    RTOS_ASSERT_DBG(DEF_FAIL, RTOS_ERR_INVALID_CFG, status);
#endif
  }

  if (DEF_BIT_IS_SET(flags, DNSc_FLAG_FORCE_CACHE)
      && DEF_BIT_IS_SET(flags, DNSc_FLAG_FORCE_RENEW)) {
    RTOS_ASSERT_DBG(DEF_FAIL, RTOS_ERR_INVALID_CFG, status);
  }
#endif
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  flag_set = DEF_BIT_IS_SET(flags, DNSc_FLAG_FORCE_RESOLUTION);
  if (flag_set == DEF_NO) {
    CPU_INT08U addr[NET_IP_MAX_ADDR_LEN];
    //                                                             First check to see if the incoming host name is
    //                                                             simply a decimal-dot-formatted IP address. If it
    //                                                             is, then just convert it and return.
    ip_family = NetASCII_Str_to_IP(p_host_name,
                                   addr,
                                   sizeof(addr),
                                   &err);
    if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
      NET_HOST_IP_ADDR *p_addr;

      p_addr = DNScCache_HostAddrAlloc(p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }

      switch (ip_family) {
        case NET_IP_ADDR_FAMILY_IPv4:
          p_addr->AddrObj.AddrLen = NET_IPv4_ADDR_LEN;
          break;;

        case NET_IP_ADDR_FAMILY_IPv6:
          p_addr->AddrObj.AddrLen = NET_IPv6_ADDR_LEN;
          break;

        default:
          DNScCache_HostAddrsFree(p_addr);
          status = DNSc_STATUS_FAILED;
          RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
          goto exit;
      }

      Mem_Copy(&p_addr->AddrObj.Addr, addr, p_addr->AddrObj.AddrLen);
      *p_addrs = p_addr;
      *p_addr_nbr = 1u;
      status = DNSc_STATUS_RESOLVED;
      goto exit;
    }
  }

  flag_set = DEF_BIT_IS_SET(flags, DNSc_FLAG_FORCE_CACHE);
  if (flag_set == DEF_YES) {
    status = DNScCache_HostAddrsGet(p_host_name, p_addrs, p_addr_nbr, flags, p_err);
    goto exit;
  }

  flag_set = DEF_BIT_IS_SET(flags, DNSc_FLAG_FORCE_RENEW);
  if (flag_set == DEF_NO) {
    //                                                             ---------- SRCH IN EXISTING CACHE ENTRIES ----------
    status = DNScCache_HostAddrsGet(p_host_name, p_addrs, p_addr_nbr, flags, p_err);
    switch (status) {
      case DNSc_STATUS_PENDING:
      case DNSc_STATUS_RESOLVED:
        goto exit;

      case DNSc_STATUS_FAILED:
        break;

      default:
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
        goto exit;
    }
  } else {
    DNScCache_HostSrchRemove(p_host_name, p_err);
  }

  //                                                               ----------- ACQUIRE HOST OBJ FOR THE REQ -----------
  p_host = DNScCache_HostObjGet(p_host_name, flags, p_cfg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    status = DNSc_STATUS_FAILED;
    goto exit;
  }

  status = DNScTask_HostResolve(p_host, flags, p_cfg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  switch (status) {
    case DNSc_STATUS_PENDING:
      goto exit;

    case DNSc_STATUS_RESOLVED:
    case DNSc_STATUS_UNKNOWN:
      break;

    case DNSc_STATUS_FAILED:
      goto exit_free_host;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      goto exit;
  }

  status = DNScCache_HostAddrsGet(p_host_name, p_addrs, p_addr_nbr, flags, p_err);

  goto exit;

exit_free_host:
  DNScCache_HostObjFree(p_host);

exit:
  return (status);
}

/****************************************************************************************************//**
 *                                         DNSc_HostAddrsFree()
 *
 * @brief    Free Host addresses allocated by DNS_HostAddrsGet().
 *
 * @param    p_addr  Pointer to the host addresses link-list to free.
 *******************************************************************************************************/
void DNSc_FreeHostAddrs(NET_HOST_IP_ADDR *p_addr)
{
  DNScCache_HostAddrsFree(p_addr);
}

/****************************************************************************************************//**
 *                                            DNSc_CacheClr()
 *
 * @brief    Flushes the DNS cache.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_INIT
 *******************************************************************************************************/
void DNSc_CacheClrAll(RTOS_ERR *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((DNSc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  DNScCache_Clr();
}

/****************************************************************************************************//**
 *                                          DNSc_CacheClrHost()
 *
 * @brief    Removes a host from the cache.
 *
 * @param    p_host_name     Pointer to a string that contains the host name to remove from the cache.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_NET_OP_IN_PROGRESS
 *                               - RTOS_ERR_NOT_FOUND
 *******************************************************************************************************/
void DNSc_CacheClrHost(CPU_CHAR *p_host_name,
                       RTOS_ERR *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((DNSc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT,; );
  RTOS_ASSERT_DBG_ERR_SET((p_host_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  DNScCache_HostSrchRemove(p_host_name, p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                             DNSc_Init()
 *
 * @brief    Initializes DNS client module.
 *
 * @param    p_cfg       Pointer to DNS client's configuration.
 *
 * @param    p_mem_seg   Memory segment from which internal data will be allocated.
 *                       If DEF_NULL, it will be allocated from the global heap.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function.
 *
 * @note     (1) DNSc_Init() MUST be called PRIOR to using other DNSc functions.
 *******************************************************************************************************/
void DNSc_Init(const DNSc_CFG *p_cfg,
               MEM_SEG        *p_mem_seg,
               RTOS_ERR       *p_err)
{
  DNSc_DATA   *p_dnsc_data;
  DNSc_SERVER server_addr;
#if ((RTOS_ARG_CHK_EXT_EN == DEF_ENABLED) \
  && defined(DNSc_TASK_MODULE_EN)         \
  && defined(DNSc_SIGNAL_TASK_MODULE_EN))
  CPU_BOOLEAN is_en = DEF_NO;
#endif
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((DNSc_DataPtr == DEF_NULL), *p_err, RTOS_ERR_ALREADY_INIT,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if ((RTOS_ARG_CHK_EXT_EN == DEF_ENABLED) \
  && defined(DNSc_TASK_MODULE_EN)         \
  && defined(DNSc_SIGNAL_TASK_MODULE_EN))
  is_en = KAL_FeatureQuery(KAL_FEATURE_SEM_DEL, KAL_OPT_CREATE_NONE);
  if (is_en == DEF_NO) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
  }
#endif

  //                                                               ------ CREATE DNS CLIENT INTERNAL DATA OBJECT ------
  p_dnsc_data = (DNSc_DATA *)Mem_SegAlloc("DNS Client Module",
                                          p_mem_seg,
                                          sizeof(DNSc_DATA),
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               --------- SAVE NEEDED CONFIGURATION VALUES ---------
  p_dnsc_data->CfgDfltServerAddrFallbackEn = p_cfg->DfltServerAddrFallbackEn;
  p_dnsc_data->CfgOpDly_ms = p_cfg->OpDly_ms;
  p_dnsc_data->CfgAddrIPv4MaxPerHost = p_cfg->AddrIPv4MaxPerHost;
  p_dnsc_data->CfgAddrIPv6MaxPerHost = p_cfg->AddrIPv6MaxPerHost;
  p_dnsc_data->CfgReqRetryNbrMax = p_cfg->ReqRetryNbrMax;
  p_dnsc_data->CfgReqRetryTimeout_ms = p_cfg->ReqRetryTimeout_ms;

  //                                                               --------- INITIALIZE DNS SERVER ADDRESSES ----------
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
  Mem_Clr(&server_addr.AddrObj, sizeof(NET_IP_ADDR_OBJ));
#endif

  server_addr.IsValid = DEF_NO;

  CORE_ENTER_ATOMIC();
  p_dnsc_data->ServerAddrStatic = server_addr;
  p_dnsc_data->ServerAddrAuto = server_addr;
  CORE_EXIT_ATOMIC();

  //                                                               ------------- INITIALIZE CACHE MODULE --------------
  DNScCache_Init(p_cfg, p_dnsc_data, p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------------ INITIALIZE DNS CLIENT TASK ------------
#if 0
  DNScTask_Init(p_dnsc_data, task_prio, stk_size_elements, p_stk, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
  DNScCmd_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  CORE_ENTER_ATOMIC();
  DNSc_DataPtr = p_dnsc_data;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                     DNSc_SetServerAddrInternal()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    p_server_addr   $$$$ Add description for 'p_server_addr'
 *
 * @param    p_err           $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *
 *           none.
 *******************************************************************************************************/
void DNSc_SetServerAddrInternal(NET_IP_ADDR_OBJ *p_server_addr,
                                RTOS_ERR        *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_server_addr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  switch (p_server_addr->AddrLen) {
    case NET_IPv4_ADDR_SIZE:
    case NET_IPv6_ADDR_SIZE:
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  DNScReq_ServerSet(DNSc_SERVER_ADDR_TYPE_AUTO, p_server_addr);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_DNS_CLIENT_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
