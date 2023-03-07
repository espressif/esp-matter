/***************************************************************************//**
 * @file
 * @brief Initializes the network
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "network.h"
#include "webserver.h"

#include <rtos_description.h>
#include <net_cfg.h>
#include <cpu/include/cpu.h>
#include <common/include/lib_str.h>
#include <common/include/lib_mem.h>
#include <common/include/rtos_err.h>
#include <common/include/rtos_utils.h>
#include <net/include/net.h>
#include <net/include/net_type.h>
#include <net/include/net_if.h>
#include <net/include/net_ascii.h>
#include <net/include/dhcp_client.h>
#include <net/include/dhcp_client_types.h>
#include <net/include/net_ipv4.h>
#include <net/include/net_ipv6.h>
#include <net/include/net_if_ether.h>

#include "em_device.h"
#include "common_declarations.h"

#include <stdbool.h>

/***************************************************************************//**
 * @brief logging: This example outputs information to the console via 
 *                 the function printf() via a macro called EX_TRACE().
 *                 This can be modified or disabled if printf() is not supported
 ******************************************************************************/
#ifndef EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

// -----------------------------------------------------------------------------
// Global Variables

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG == DEF_ENABLED)
extern  const   NET_INIT_CFG  Net_InitCfgDflt;
#endif

NET_IF_NBR ifnNbr;

// -----------------------------------------------------------------------------
// Local Function Prototypes

static void DHCPComplete           (NET_IF_NBR                  if_nbr,
                                            DHCPc_STATUS                status,
                                            NET_IPv4_ADDR               addr,
                                            NET_IPv4_ADDR               mask,
                                            NET_IPv4_ADDR               gateway,
                                            RTOS_ERR                    err);

static void Ex_IPv4_LinkLocalAddrCfgResult (NET_IF_NBR                  if_nbr,
                                            NET_IPv4_ADDR               link_local_addr,
                                            NET_IPv4_LINK_LOCAL_STATUS  status,
                                            RTOS_ERR                    err);

static void Ex_MacAddrGet(CPU_CHAR * addr_mac_str);

// -----------------------------------------------------------------------------
// Local Global Variables

static  const  NET_IF_ETHER_CFG  Ex_NetIF_CfgDflt_Ether = {
  .HW_AddrStr                    = "00:00:00:00:00:00",

  .IPv4.Static.Addr              = DEF_NULL,
  .IPv4.Static.Mask              = DEF_NULL,
  .IPv4.Static.Gateway           = DEF_NULL,

  .IPv4.DHCPc.En                 = DEF_YES,
  .IPv4.DHCPc.Cfg                = DHCPc_CFG_DFLT,
  .IPv4.DHCPc.OnCompleteHook     = DHCPComplete,

  .IPv4.LinkLocal.En             = DEF_YES,
  .IPv4.LinkLocal.OnCompleteHook = Ex_IPv4_LinkLocalAddrCfgResult,

  .IPv6.Static.Addr              = DEF_NULL,
  .IPv6.Static.PrefixLen         = 0,
  .IPv6.Static.DAD_En            = DEF_NO,
  .IPv6.AutoCfg.En               = DEF_NO,
  .IPv6.AutoCfg.DAD_En           = DEF_NO,
  .IPv6.Hook                     = DEF_NULL
};

/***************************************************************************//**
 * @brief Initialize network and ethernet interface
 ******************************************************************************/
void Network_Init(void)
{
  RTOS_ERR err;

  Net_Init(&err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );
  NetIF_Ether_Add("eth0", DEF_NULL, DEF_NULL, &err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );
}

/***************************************************************************//**
 * @brief Starts ethernet network interface.
 ******************************************************************************/
void Network_Start(void)
{
  NET_IF_NBR         if_nbr;
  RTOS_ERR           err;
  NET_IF_ETHER_CFG   if_cfg_ether;
  CPU_CHAR           addr_mac_str[NET_IF_802x_ADDR_SIZE_STR];
  
  // Interface configuration
  if_cfg_ether = Ex_NetIF_CfgDflt_Ether;
  Ex_MacAddrGet(addr_mac_str);
  if_cfg_ether.HW_AddrStr = addr_mac_str;

  if_nbr = NetIF_NbrGetFromName("eth0");
  APP_RTOS_ASSERT_CRITICAL((if_nbr != NET_IF_NBR_NONE),; );
  NetIF_Ether_Start(if_nbr, &if_cfg_ether, &err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );

#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  NET_IF_APP_INFO if_app_info;
  printf("eth0: Waiting for interface to be ready\n");
  NetIF_WaitSetupReady(if_nbr, &if_app_info, 0, &err);
  printf("eth0: interface ready\n");
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );
#endif
}

/***************************************************************************//**
 *  @brief Hook function called when DHCP process 
 *         has been complete on an network interface.
 *
 *  @param if_nbr Network Interface number on which DHCP process occurred.
 *
 *  @param status DHCP process status:
 *                               DHCPc_STATUS_SUCCESS
 *                               DHCPc_STATUS_FAIL_ADDR_USED
 *                               DHCPc_STATUS_FAIL_OFFER_DECLINE
 *                               DHCPc_STATUS_FAIL_NAK_RX
 *                               DHCPc_STATUS_FAIL_NO_SERVER
 *                               DHCPc_STATUS_FAIL_ERR_FAULT
 *
 *  @param addr IPv4 address configured, if any.
 *              Else, NET_IPv4_ADDR_NONE.
 *
 *  @param mask IPv4 mask configured, if any.
 *              Else, NET_IPv4_ADDR_NONE.
 *
 *  @param gateway IPv4 gateway configured, if any.
 *                 Else, NET_IPv4_ADDR_NONE.
 *
 *  @param err Error object in case an error occurred.
 *             Else, RTOS_ERR_NONE.
 ******************************************************************************/
static void DHCPComplete(NET_IF_NBR     if_nbr,
                         DHCPc_STATUS   status,
                         NET_IPv4_ADDR  addr,
                         NET_IPv4_ADDR  mask,
                         NET_IPv4_ADDR  gateway,
                         RTOS_ERR       err)
{
  CPU_CHAR   addr_string[NET_ASCII_LEN_MAX_ADDR_IPv4];
  CPU_CHAR   mask_string[NET_ASCII_LEN_MAX_ADDR_IPv4];
  CPU_CHAR   gateway_string[NET_ASCII_LEN_MAX_ADDR_IPv4];
  RTOS_ERR   local_err;

  PP_UNUSED_PARAM(if_nbr);

  switch (status) {
    case DHCPc_STATUS_SUCCESS:
      NetASCII_IPv4_to_Str(addr, addr_string, DEF_NO, &local_err);
      APP_RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE),; );

      NetASCII_IPv4_to_Str(mask, mask_string, DEF_NO, &local_err);
      APP_RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE),; );

      NetASCII_IPv4_to_Str(gateway, gateway_string, DEF_NO, &local_err);
      APP_RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE),; );

      EX_TRACE("DHCP client process succeeded.\n");
      EX_TRACE("IP address configured : %s\n", addr_string);
      EX_TRACE("IP address mask       : %s\n", mask_string);
      EX_TRACE("IP address gateway    : %s\n", gateway_string);
      break;

    case DHCPc_STATUS_FAIL_ADDR_USED:
      EX_TRACE("DHCP client process failed: Address already used on network.\n");
      break;

    case DHCPc_STATUS_FAIL_OFFER_DECLINE:
      EX_TRACE("DHCP client process failed: DHCP client declined the offer.\n");
      break;

    case DHCPc_STATUS_FAIL_NAK_RX:
      EX_TRACE("DHCP client process failed: The DHCP server refused the client.\n");
      break;

    case DHCPc_STATUS_FAIL_NO_SERVER:
      EX_TRACE("DHCP client process failed: No DHCP server was detected on the network.\n");
      break;

    case DHCPc_STATUS_FAIL_ERR_FAULT:
      EX_TRACE("DHCP client process failed: Fatal error %i.\n", err.Code);
      break;

    default:
      EX_TRACE("DHCP client invalid result.\n");
      break;
  }
}

/***************************************************************************//**
 *  @brief Hook function called when IPv4 Link local process 
 *         has been complete on an network interface.
 *
 *  @param if_nbr Network Interface number on which 
 *                IPv4 link local process occurred.
 *
 *  @param link_local_addr IPv4 link local address, if configured.
 *                         Else, NET_IPv4_ADDR_NONE.
 *
 *  @param status IPv4 Link Local process status:
 *                                       NET_IPv4_LINK_LOCAL_STATUS_NONE
 *                                       NET_IPv4_LINK_LOCAL_STATUS_SUCCEEDED
 *                                       NET_IPv4_LINK_LOCAL_STATUS_FAILED
 *
 *  @param err Error object in case an error occurred.
 *             Else, RTOS_ERR_NONE.
 *  @note IPv4 link local address configuration will only be started 
 *        if its enabled in the
 *                   NET_IF_ETHER_CFG object, but also if no other 
 *                   IPv4 valid addresses are configured on the network interface.
 *                   Therefore, it will occurred if :
 *                       (a)  no IPv4 static address is set up and DHCP process is disabled.
 *                       (b)  the DHCP setup fails and no IPv4 static address is set up.
 ******************************************************************************/
static  void  Ex_IPv4_LinkLocalAddrCfgResult(NET_IF_NBR                  if_nbr,
                                             NET_IPv4_ADDR               link_local_addr,
                                             NET_IPv4_LINK_LOCAL_STATUS  status,
                                             RTOS_ERR                    err)
{
  CPU_CHAR  addr_str[NET_ASCII_LEN_MAX_ADDR_IPv4];
  RTOS_ERR  local_err;

  EX_TRACE("On Interface number #%i, \n", if_nbr);

  switch (status) {
    case NET_IPv4_LINK_LOCAL_STATUS_SUCCEEDED:
      if (link_local_addr != NET_IPv4_ADDR_NONE) {
        NetASCII_IPv4_to_Str(link_local_addr, addr_str, DEF_YES, &local_err);
        if (local_err.Code != RTOS_ERR_NONE) {
          return;
        }
      }
      EX_TRACE("IPv4 link local address: %s, was configured successfully!\n", addr_str);
      break;

    case NET_IPv4_LINK_LOCAL_STATUS_FAILED:
      EX_TRACE("IPv4 link local address configuration failed: %i.\n", err.Code);
      break;

    default:
      break;
  }
}

static void Ex_MacAddrGet(CPU_CHAR * addr_mac_str)
{
  CPU_INT08U         addr_mac[NET_IF_802x_HW_ADDR_LEN];
  RTOS_ERR           err;

  if (DEVINFO->EUI48L != 0xFFFFFFFF) {
    addr_mac[0] = DEVINFO->EUI48H >> 8;
    addr_mac[1] = DEVINFO->EUI48H >> 0;
    addr_mac[2] = DEVINFO->EUI48L >> 24;
    addr_mac[3] = DEVINFO->EUI48L >> 16;
    addr_mac[4] = DEVINFO->EUI48L >> 8;
    addr_mac[5] = DEVINFO->EUI48L >> 0;
  } else {
    addr_mac[0] = DEVINFO->UNIQUEH >> 24;
    addr_mac[1] = DEVINFO->UNIQUEH >> 16;
    addr_mac[2] = DEVINFO->UNIQUEH >> 8;
    addr_mac[3] = DEVINFO->UNIQUEL >> 16;
    addr_mac[4] = DEVINFO->UNIQUEL >> 8;
    addr_mac[5] = DEVINFO->UNIQUEL >> 0;
  }

  NetASCII_MAC_to_Str(addr_mac, addr_mac_str, DEF_FALSE, DEF_TRUE, &err);
}
