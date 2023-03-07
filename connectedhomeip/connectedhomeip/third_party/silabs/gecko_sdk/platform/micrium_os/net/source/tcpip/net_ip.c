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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "net_ip_priv.h"
#include  <net/include/net_cfg_net.h>

#ifdef  NET_IPv4_MODULE_EN
#include  "net_ipv4_priv.h"
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  "net_ipv6_priv.h"
#endif

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIP_Init()
 *
 * @brief    Initialize IP modules.
 *
 * @param    p_mem_seg   Pointer to memory segment use for allocation
 *
 * @param    p_err       Error pointer
 *******************************************************************************************************/
void NetIP_Init(MEM_SEG  *p_mem_seg,
                RTOS_ERR *p_err)
{
#ifdef  NET_IPv4_MODULE_EN
  NetIPv4_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

#ifdef  NET_IPv6_MODULE_EN
  NetIPv6_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIP_IF_Init()
 *
 * @brief    Initialize IP objects for given Interface.
 *
 * @param    if_nbr      Network Interface number to initialize.
 *
 * @param    p_mem_seg   Pointer to memory segment use for allocation
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetIP_IF_Init(NET_IF_NBR if_nbr,
                   MEM_SEG    *p_mem_seg,
                   RTOS_ERR   *p_err)
{
  NET_IF        *p_if;
  NET_IP_IF_OBJ *p_ip_obj;

  PP_UNUSED_PARAM(p_mem_seg);

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  p_ip_obj = (NET_IP_IF_OBJ *)(p_if->IP_Obj);

#ifdef  NET_IPv4_MODULE_EN
  SList_Init(&p_ip_obj->IPv4_AddrListPtr);
#endif

#ifdef  NET_IPv6_MODULE_EN
  SList_Init(&p_ip_obj->IPv6_AddrListPtr);

  //                                                               --------- INIT IPv6 LINK CHANGE SUBSCRIBE ----------
  NetIF_LinkStateSubscribeHandler(if_nbr,
                                  &NetIPv6_LinkStateSubscriber,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIP_IsAddrCfgd()
 *
 * @brief    Check if an IP address is configured on the given network interface.
 *
 * @param    if_nbr  Network Interface number
 *
 * @param    p_addr  Pointer to IP address
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if address is configured.
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIP_IsAddrCfgd(NET_IF_NBR         if_nbr,
                             NET_IP_ADDR        *p_addr,
                             NET_IP_ADDR_FAMILY addr_type,
                             RTOS_ERR           *p_err)
{
  NET_IF        *p_if = DEF_NULL;
  NET_IP_IF_OBJ *p_ip_obj = DEF_NULL;
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR_OBJ *p_addr_ipv4_obj = DEF_NULL;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_IPv6_ADDR_OBJ *p_addr_ipv6_obj = DEF_NULL;
#endif
  CPU_BOOLEAN cmp_result = DEF_NO;
  CPU_BOOLEAN rtn_val = DEF_NO;

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_ip_obj = (NET_IP_IF_OBJ *)(p_if->IP_Obj);

  switch (addr_type) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv4:
      SLIST_FOR_EACH_ENTRY(p_ip_obj->IPv4_AddrListPtr, p_addr_ipv4_obj, NET_IPv4_ADDR_OBJ, ListNode) {
        cmp_result = Mem_Cmp(p_addr, &p_addr_ipv4_obj->AddrHost, NET_IPv4_ADDR_SIZE);
        if (cmp_result == DEF_YES) {
          if (p_addr_ipv4_obj->IsValid == DEF_YES) {
            rtn_val = DEF_YES;
          }
          break;
        }
      }
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv6:
      SLIST_FOR_EACH_ENTRY(p_ip_obj->IPv6_AddrListPtr, p_addr_ipv6_obj, NET_IPv6_ADDR_OBJ, ListNode) {
        cmp_result = Mem_Cmp(p_addr, &p_addr_ipv6_obj->AddrHost, NET_IPv6_ADDR_SIZE);
        if (cmp_result == DEF_YES) {
          if (p_addr_ipv6_obj->IsValid == DEF_YES) {
            rtn_val = DEF_YES;
          }
          break;
        }
      }
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NO);
  }

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                           NetIP_IsDynAddrCfgd()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    if_nbr      $$$$ Add description for 'if_nbr'
 *
 * @param    addr_type   $$$$ Add description for 'addr_type'
 *
 * @param    p_err       $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
CPU_BOOLEAN NetIP_IsDynAddrCfgd(NET_IF_NBR         if_nbr,
                                NET_IP_ADDR_FAMILY addr_type,
                                RTOS_ERR           *p_err)
{
  NET_IF        *p_if = DEF_NULL;
  NET_IP_IF_OBJ *p_ip_obj = DEF_NULL;
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR_OBJ *p_addr_ipv4_obj = DEF_NULL;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_IPv6_ADDR_OBJ *p_addr_ipv6_obj = DEF_NULL;
#endif
  CPU_BOOLEAN rtn_val = DEF_NO;

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_ip_obj = (NET_IP_IF_OBJ *)(p_if->IP_Obj);

  switch (addr_type) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv4:
      SLIST_FOR_EACH_ENTRY(p_ip_obj->IPv4_AddrListPtr, p_addr_ipv4_obj, NET_IPv4_ADDR_OBJ, ListNode) {
        switch (p_addr_ipv4_obj->CfgMode) {
          case NET_IP_ADDR_CFG_MODE_DYN:
          case NET_IP_ADDR_CFG_MODE_DYN_INIT:
            rtn_val = DEF_YES;
            goto exit;

          default:
            break;
        }
      }
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv6:
      SLIST_FOR_EACH_ENTRY(p_ip_obj->IPv6_AddrListPtr, p_addr_ipv6_obj, NET_IPv6_ADDR_OBJ, ListNode) {
        switch (p_addr_ipv6_obj->AddrCfgMode) {
          case NET_IP_ADDR_CFG_MODE_DYN:
          case NET_IP_ADDR_CFG_MODE_DYN_INIT:
            rtn_val = DEF_YES;
            goto exit;

          default:
            break;
        }
      }
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NO);
  }

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                           NetIP_IsAddrsCfgdOnIF()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    if_nbr  $$$$ Add description for 'if_nbr'
 *
 * @param    p_err   $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
CPU_BOOLEAN NetIP_IsAddrsCfgdOnIF(NET_IF_NBR if_nbr,
                                  RTOS_ERR   *p_err)
{
  CPU_BOOLEAN addr_availv4;
  CPU_BOOLEAN addr_availv6;

#ifdef  NET_IPv4_MODULE_EN
  addr_availv4 = NetIPv4_IsAddrsCfgdOnIF_Handler(if_nbr, p_err);
#else
  addr_availv4 = DEF_NO;
#endif

#ifdef  NET_IPv6_MODULE_EN
  addr_availv6 = NetIPv6_IsAddrsCfgdOnIF_Handler(if_nbr, p_err);
#else
  addr_availv6 = DEF_NO;
#endif

  return ((addr_availv4 || addr_availv6));
}

/****************************************************************************************************//**
 *                                           NetIP_IsAddrValid()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    addr_type   $$$$ Add description for 'addr_type'
 *
 * @param    p_addr      $$$$ Add description for 'p_addr'
 *
 * @param    addr_len    $$$$ Add description for 'addr_len'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
CPU_BOOLEAN NetIP_IsAddrValid(NET_IP_ADDR_FAMILY addr_type,
                              CPU_INT08U         *p_addr,
                              CPU_INT08U         addr_len)
{
  CPU_BOOLEAN valid;

  switch (addr_type) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv4:
      valid = NetIPv4_IsValidAddrProtocol(p_addr, addr_len);
      break;
#endif
#ifdef  NET_IPv6_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv6:
      valid = NetIPv6_IsValidAddrProtocol(p_addr, addr_len);
      break;
#endif

    default:
      valid = DEF_NO;
      break;
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                           NetIP_IsAddrMulticast()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    addr_type   $$$$ Add description for 'addr_type'
 *
 * @param    p_addr      $$$$ Add description for 'p_addr'
 *
 * @param    addr_len    $$$$ Add description for 'addr_len'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
#ifdef  NET_MCAST_MODULE_EN
CPU_BOOLEAN NetIP_IsAddrMulticast(NET_IP_ADDR_FAMILY addr_type,
                                  CPU_INT08U         *p_addr,
                                  CPU_INT08U         addr_len)
{
  CPU_BOOLEAN addr_multicast;

  switch (addr_type) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv4:
      addr_multicast = NetIPv4_IsAddrProtocolMulticast(p_addr, addr_len);
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv6:
      addr_multicast = NetIPv6_IsAddrProtocolMulticast(p_addr, addr_len);
      break;
#endif

    default:
      addr_multicast = DEF_NO;
      break;
  }

  return (addr_multicast);
}
#endif

/****************************************************************************************************//**
 *                                           NetIP_IsAddrConflict()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    if_nbr  $$$$ Add description for 'if_nbr'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
CPU_BOOLEAN NetIP_IsAddrConflict(NET_IF_NBR if_nbr)
{
  CPU_BOOLEAN addr_conflict = DEF_NO;

  PP_UNUSED_PARAM(if_nbr);

#ifdef  NET_IPv4_MODULE_EN
  addr_conflict = NetIPv4_IsAddrProtocolConflict(if_nbr);
#endif

  return (addr_conflict);
}

/****************************************************************************************************//**
 *                                           NetIP_InvalidateAddr()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    if_nbr      $$$$ Add description for 'if_nbr'
 *
 * @param    p_addr      $$$$ Add description for 'p_addr'
 *
 * @param    addr_type   $$$$ Add description for 'addr_type'
 *
 * @param    p_err       $$$$ Add description for 'p_err'
 *
 * @return   DEF_OK
 *           DEF_FAIL
 *******************************************************************************************************/
CPU_BOOLEAN NetIP_InvalidateAddr(NET_IF_NBR         if_nbr,
                                 NET_IP_ADDR        *p_addr,
                                 NET_IP_ADDR_FAMILY addr_type,
                                 RTOS_ERR           *p_err)
{
  NET_IF        *p_if = DEF_NULL;
  NET_IP_IF_OBJ *p_ip_obj = DEF_NULL;
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR_OBJ *p_addr_ipv4_obj = DEF_NULL;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_IPv6_ADDR_OBJ *p_addr_ipv6_obj = DEF_NULL;
#endif
  CPU_BOOLEAN cmp_result = DEF_NO;
  CPU_BOOLEAN rtn_val = DEF_FAIL;

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_ip_obj = (NET_IP_IF_OBJ *)(p_if->IP_Obj);

  switch (addr_type) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv4:
      SLIST_FOR_EACH_ENTRY(p_ip_obj->IPv4_AddrListPtr, p_addr_ipv4_obj, NET_IPv4_ADDR_OBJ, ListNode) {
        cmp_result = Mem_Cmp(p_addr, &p_addr_ipv4_obj->AddrHost, NET_IPv4_ADDR_SIZE);
        if (cmp_result == DEF_YES) {
          p_addr_ipv4_obj->IsValid = DEF_NO;
          break;
        }
      }
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv6:
      SLIST_FOR_EACH_ENTRY(p_ip_obj->IPv6_AddrListPtr, p_addr_ipv6_obj, NET_IPv6_ADDR_OBJ, ListNode) {
        cmp_result = Mem_Cmp(p_addr, &p_addr_ipv6_obj->AddrHost, NET_IPv6_ADDR_SIZE);
        if (cmp_result == DEF_YES) {
          p_addr_ipv6_obj->IsValid = DEF_NO;
          break;
        }
      }
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_FAIL);
  }

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                           NetIP_GetHostAddr()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    if_nbr                      $$$$ Add description for 'if_nbr'
 *
 * @param    protocol_type               $$$$ Add description for 'protocol_type'
 *
 * @param    p_addr_protocol_tbl         $$$$ Add description for 'p_addr_protocol_tbl'
 *
 * @param    p_addr_protocol_tbl_qty     $$$$ Add description for 'p_addr_protocol_tbl_qty'
 *
 * @param    p_addr_protocol_len         $$$$ Add description for 'p_addr_protocol_len'
 *
 * @param    p_err                       $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
#if 0
void NetIP_GetHostAddr(NET_IF_NBR         if_nbr,
                       NET_IP_ADDR_FAMILY addr_type,
                       CPU_INT08U         *p_addr_tbl,
                       CPU_INT08U         *p_addr_tbl_qty,
                       CPU_INT08U         *p_addr_len,
                       RTOS_ERR           *p_err)
{
  switch (addr_type) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv4:
      NetIPv4_GetHostAddrProtocol(if_nbr,
                                  p_addr_tbl,
                                  p_addr_tbl_qty,
                                  p_addr_len,
                                  p_err);
      break;
#endif
#ifdef  NET_IPv6_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv6:
      NetIPv6_GetHostAddrProtocol(if_nbr,
                                  p_addr_tbl,
                                  p_addr_tbl_qty,
                                  p_addr_len,
                                  p_err);
      break;
#endif

    default:
      *p_addr_tbl_qty = 0u;
      *p_addr_len = 0u;
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}
#endif

/****************************************************************************************************//**
 *                                           NetIP_GetHostAddrIF_Nbr()
 *
 * @brief    Get the interface number for a host's IP address.
 *
 * @param    addr_type   IP address family:
 *                       NET_IP_ADDR_FAMILY_IPv4
 *                       NET_IP_ADDR_FAMILY_IPv6
 *
 * @param    p_addr      Pointer to host's IP address.
 *
 * @param    addr_len    Length of the host's IP address
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   The interface number on which the host IP address is configured.
 *           NET_IF_NBR_NONE, otherwise.
 *******************************************************************************************************/
NET_IF_NBR NetIP_GetHostAddrIF_Nbr(NET_IP_ADDR_FAMILY addr_type,
                                   CPU_INT08U         *p_addr,
                                   CPU_INT08U         addr_len,
                                   RTOS_ERR           *p_err)
{
  NET_IF_NBR if_nbr = NET_IF_NBR_NONE;

  switch (addr_type) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv4:
      if_nbr = NetIPv4_GetAddrProtocolIF_Nbr(p_addr,
                                             addr_len,
                                             p_err);
      break;
#endif
#ifdef  NET_IPv6_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv6:
      if_nbr = NetIPv6_GetAddrProtocolIF_Nbr(p_addr,
                                             addr_len,
                                             p_err);
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_IF_NBR_NONE);
  }

  return (if_nbr);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
