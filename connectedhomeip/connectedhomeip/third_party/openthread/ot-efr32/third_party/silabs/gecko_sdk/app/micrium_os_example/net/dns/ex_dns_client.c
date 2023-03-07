/***************************************************************************//**
 * @file
 * @brief Network Core Example - DNS Client
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

#include  <net_cfg.h>
#include  <lib_def.h>

#if (NET_DNS_CLIENT_CFG_MODULE_EN == DEF_ENABLED)

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_ip.h>
#include  <net/include/net_type.h>
#include  <net/include/dns_client.h>
#include  <net/include/net_ascii.h>

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          Ex_DNSc_GetHostname()
 *
 * @brief  This function resolve the provided hostname and returns the IP address of the host.
 *
 * @param  p_hostname  Pointer to a string that contains the hostname to be resolved.
 *
 * @param  p_addr_str  Pointer to a string that will receive the IP address.
 *
 * @note   (1) Prior to do any call to DNSc the module must be initialized via the Net_Init()
 *             function.
 *******************************************************************************************************/
void Ex_DNSc_GetHostname(CPU_CHAR *p_hostname,
                         CPU_CHAR *p_addr_str)
{
  NET_IP_ADDR_OBJ addrs[2u];
  CPU_INT08U      addr_nbr = 2u;
  CPU_INT08U      ix;
  RTOS_ERR        err;

  DNSc_GetHost(p_hostname, addrs, &addr_nbr, DNSc_FLAG_NONE, DEF_NULL, &err);
  if (err.Code != RTOS_ERR_NONE) {
    return;
  }

  for (ix = 0u; ix < addr_nbr; ix++) {
    if (addrs[ix].AddrLen == NET_IPv4_ADDR_LEN) {
#if (NET_IPv4_CFG_EN == DEF_ENABLED)
      NET_IPv4_ADDR *p_addr = (NET_IPv4_ADDR *)&addrs[ix].Addr.Array[0];

      NetASCII_IPv4_to_Str(*p_addr, p_addr_str, NET_ASCII_LEN_MAX_ADDR_IP, &err);
#endif
    } else {
#if (NET_IPv6_CFG_EN == DEF_ENABLED)
      NET_IPv6_ADDR *p_addr = (NET_IPv6_ADDR *)&addrs[ix].Addr.Array[0];

      NetASCII_IPv6_to_Str(p_addr, p_addr_str, DEF_NO, DEF_YES, &err);
#endif
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_DNS_CLIENT_CFG_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
