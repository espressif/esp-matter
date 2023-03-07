/***************************************************************************//**
 * @file
 * @brief Example Network Core Initialisation - Ethernet & Wifi Interface
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <ex_description.h>

#include  "ex_net_core_init.h"

#include  <net_cfg.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_str.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_utils.h>

#include  <net/include/net.h>
#include  <net/include/net_type.h>
#include  <net/include/net_if.h>
#include  <net/include/net_ascii.h>
#include  <net/include/dhcp_client.h>
#include  <net/include/dhcp_client_types.h>
#include  <net/include/net_ipv4.h>
#include  <net/include/net_ipv6.h>

#ifdef  RTOS_MODULE_NET_IF_ETHER_AVAIL
#include  <net/include/net_if_ether.h>
#endif

#ifdef  RTOS_MODULE_NET_IF_WIFI_AVAIL
#include  <net/include/net_if_wifi.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (defined(RTOS_MODULE_NET_IF_ETHER_AVAIL) \
  || defined(RTOS_MODULE_NET_IF_WIFI_AVAIL))
#define  EX_NET_CORE_IF_EXT_EN
#endif

#ifndef  EX_NET_CORE_IF_ETHER_MAC_ADDR
#define  EX_NET_CORE_IF_ETHER_MAC_ADDR                      DEF_NULL
#endif

#ifndef  EX_NET_CORE_IF_ETHER_IPv4_DHCP_EN
#define  EX_NET_CORE_IF_ETHER_IPv4_DHCP_EN                  DEF_ENABLED
#endif

#ifndef  EX_NET_CORE_IF_ETHER_IPv4_STATIC_ADDR
#define  EX_NET_CORE_IF_ETHER_IPv4_STATIC_ADDR              "10.10.10.111"
#endif

#ifndef  EX_NET_CORE_IF_ETHER_IPv4_STATIC_MASK
#define  EX_NET_CORE_IF_ETHER_IPv4_STATIC_MASK              "255.255.255.0"
#endif

#ifndef  EX_NET_CORE_IF_ETHER_IPv4_STATIC_GATEWAY
#define  EX_NET_CORE_IF_ETHER_IPv4_STATIC_GATEWAY           "10.10.10.1"
#endif

#ifndef  EX_NET_CORE_IF_ETHER_IPv6_DAD_EN
#define  EX_NET_CORE_IF_ETHER_IPv6_DAD_EN                    DEF_DISABLED
#endif

#ifndef  EX_NET_CORE_IF_ETHER_IPv6_AUTOCONF_EN
#define  EX_NET_CORE_IF_ETHER_IPv6_AUTOCONF_EN               DEF_ENABLED
#endif
#ifndef  EX_NET_CORE_IF_ETHER_IPv6_AUTOCONF_DAD_EN
#define  EX_NET_CORE_IF_ETHER_IPv6_AUTOCONF_DAD_EN           DEF_ENABLED
#endif

#ifndef  EX_NET_CORE_IF_ETHER_IPv6_STATIC_LOCAL_ADDR
#define  EX_NET_CORE_IF_ETHER_IPv6_STATIC_LOCAL_ADDR        "fe80::1111:1111"
#endif

#ifndef  EX_NET_CORE_IF_ETHER_IPv6_STATIC_LOCAL_PREFIX_LEN
#define  EX_NET_CORE_IF_ETHER_IPv6_STATIC_LOCAL_PREFIX_LEN   64u
#endif

#ifndef  EX_NET_CORE_IF_WIFI_MAC_ADDR
#define  EX_NET_CORE_IF_WIFI_MAC_ADDR                        DEF_NULL
#endif

#ifndef  EX_NET_CORE_IF_WIFI_IPv4_DHCP_EN
#define  EX_NET_CORE_IF_WIFI_IPv4_DHCP_EN                    DEF_ENABLED
#endif

#ifndef  EX_NET_CORE_IF_WIFI_SSID
#define  EX_NET_CORE_IF_WIFI_SSID                           "Wifi_AP_SSID"
#endif

#ifndef  EX_NET_CORE_IF_WIFI_PASSWORD
#define  EX_NET_CORE_IF_WIFI_PASSWORD                       "password"
#endif

#ifndef  EX_NET_CORE_IF_WIFI_SCAN_EN
#define  EX_NET_CORE_IF_WIFI_SCAN_EN                         DEF_ENABLED
#endif

#ifndef  EX_NET_CORE_IF_WIFI_NET_TYPE                           // Specify NET_TYPE if WIFI_SCAN_EN == DEF_DISABLED
#define  EX_NET_CORE_IF_WIFI_NET_TYPE                        NET_IF_WIFI_NET_TYPE_INFRASTRUCTURE
#endif

#ifndef  EX_NET_CORE_IF_WIFI_SECURITY_TYPE                      // Specify SECURITY_TYPE if WIFI_SCAN_EN==DEF_DISABLED
#define  EX_NET_CORE_IF_WIFI_SECURITY_TYPE                   NET_IF_WIFI_SECURITY_WPA2
#endif

#ifndef  EX_NET_CORE_IF_WIFI_IPv4_STATIC_ADDR
#define  EX_NET_CORE_IF_WIFI_IPv4_STATIC_ADDR               "192.168.1.222"
#endif

#ifndef  EX_NET_CORE_IF_WIFI_IPv4_STATIC_MASK
#define  EX_NET_CORE_IF_WIFI_IPv4_STATIC_MASK               "255.255.255.0"
#endif

#ifndef  EX_NET_CORE_IF_WIFI_IPv4_STATIC_GATEWAY
#define  EX_NET_CORE_IF_WIFI_IPv4_STATIC_GATEWAY            "192.168.1.1"
#endif

#ifndef  EX_NET_CORE_IF_WIFI_IPv6_DAD_EN
#define  EX_NET_CORE_IF_WIFI_IPv6_DAD_EN                     DEF_DISABLED
#endif

#ifndef  EX_NET_CORE_IF_WIFI_IPv6_AUTOCONF_EN
#define  EX_NET_CORE_IF_WIFI_IPv6_AUTOCONF_EN                DEF_ENABLED
#endif
#ifndef  EX_NET_CORE_IF_WIFI_IPv6_AUTOCONF_DAD_EN
#define  EX_NET_CORE_IF_WIFI_IPv6_AUTOCONF_DAD_EN            DEF_ENABLED
#endif

#ifndef  EX_NET_CORE_IF_WIFI_IPv6_STATIC_LOCAL_ADDR
#define  EX_NET_CORE_IF_WIFI_IPv6_STATIC_LOCAL_ADDR         "fe80::1111:2222"
#endif
#ifndef  EX_NET_CORE_IF_WIFI_IPv6_STATIC_LOCAL_PREFIX_LEN
#define  EX_NET_CORE_IF_WIFI_IPv6_STATIC_LOCAL_PREFIX_LEN    64u
#endif

#if (!defined(EX_NET_CORE_IF_WIFI_BAND) && defined(RTOS_MODULE_NET_IF_WIFI_AVAIL))
#define  EX_NET_CORE_IF_WIFI_BAND                            NET_DEV_BAND_2_4_GHZ
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG == DEF_ENABLED)
extern const NET_INIT_CFG Net_InitCfgDflt;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  EX_NET_CORE_IF_EXT_EN
#if  (NET_IPv6_CFG_EN == DEF_ENABLED)
static void Ex_IPv6_AddrCfgResult(NET_IF_NBR               if_nbr,
                                  NET_IPv6_CFG_ADDR_TYPE   addr_type,
                                  const NET_IPv6_ADDR      *p_addr_cfgd,
                                  NET_IPv6_ADDR_CFG_STATUS addr_cfg_status);
#endif

#if  (NET_IPv4_CFG_EN == DEF_ENABLED)
static void Ex_DHCPc_SetupResult(NET_IF_NBR    if_nbr,
                                 DHCPc_STATUS  status,
                                 NET_IPv4_ADDR addr,
                                 NET_IPv4_ADDR mask,
                                 NET_IPv4_ADDR gateway,
                                 RTOS_ERR      err);

static void Ex_IPv4_LinkLocalAddrCfgResult(NET_IF_NBR                 if_nbr,
                                           NET_IPv4_ADDR              link_local_addr,
                                           NET_IPv4_LINK_LOCAL_STATUS status,
                                           RTOS_ERR                   err);
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                 START SETUP FOR ETHERNET INTERFACE
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_NET_IF_ETHER_AVAIL
#if  (defined(EX_NET_CORE_IF_ETHER_BUF_RX_LARGE_NBR) \
  || defined(EX_NET_CORE_IF_ETHER_BUF_TX_LARGE_NBR)  \
  || defined(EX_NET_CORE_IF_ETHER_BUF_TX_SMALL_NBR))

#ifndef  EX_NET_CORE_IF_ETHER_BUF_RX_LARGE_NBR
#define  EX_NET_CORE_IF_ETHER_BUF_RX_LARGE_NBR                10u
#endif

#ifndef  EX_NET_CORE_IF_ETHER_BUF_TX_LARGE_NBR
#define  EX_NET_CORE_IF_ETHER_BUF_TX_LARGE_NBR                8u
#endif

#ifndef  EX_NET_CORE_IF_ETHER_BUF_TX_SMALL_NBR
#define  EX_NET_CORE_IF_ETHER_BUF_TX_SMALL_NBR                4u
#endif

NET_IF_BUF_CFG Ex_NetEther_IF_BufCfg = {
  .RxBufLargeNbr = EX_NET_CORE_IF_ETHER_BUF_RX_LARGE_NBR,
  .TxBufLargeNbr = EX_NET_CORE_IF_ETHER_BUF_TX_LARGE_NBR,
  .TxBufSmallNbr = EX_NET_CORE_IF_ETHER_BUF_TX_SMALL_NBR,
};

NET_IF_BUF_CFG *Ex_NetEther_CfgPtr = &Ex_NetEther_IF_BufCfg;

#else
NET_IF_BUF_CFG *Ex_NetEther_CfgPtr = DEF_NULL;
#endif

NET_IF_ETHER_CFG Ex_NetIF_CfgDflt_Ether = {
  .HW_AddrStr = EX_NET_CORE_IF_ETHER_MAC_ADDR,

#if  (NET_IPv4_CFG_EN == DEF_ENABLED)
  .IPv4.Static.Addr = EX_NET_CORE_IF_ETHER_IPv4_STATIC_ADDR,
  .IPv4.Static.Mask = EX_NET_CORE_IF_ETHER_IPv4_STATIC_MASK,
  .IPv4.Static.Gateway = EX_NET_CORE_IF_ETHER_IPv4_STATIC_GATEWAY,

#ifdef  NET_DHCP_CLIENT_MODULE_EN
  .IPv4.DHCPc.En = EX_NET_CORE_IF_ETHER_IPv4_DHCP_EN,
#else
  .IPv4.DHCPc.En = DEF_NO,
#endif
  .IPv4.DHCPc.Cfg = DHCPc_CFG_DFLT,
  .IPv4.DHCPc.OnCompleteHook = Ex_DHCPc_SetupResult,

  .IPv4.LinkLocal.En = DEF_NO,
  .IPv4.LinkLocal.OnCompleteHook = Ex_IPv4_LinkLocalAddrCfgResult,
#else
  .IPv4.Static.Addr = DEF_NULL,
  .IPv4.Static.Mask = DEF_NULL,
  .IPv4.Static.Gateway = DEF_NULL,
  .IPv4.DHCPc.En = DEF_NO,
  .IPv4.LinkLocal.En = DEF_NO,
#endif

#if  (NET_IPv6_CFG_EN == DEF_ENABLED)
  .IPv6.Static.Addr = EX_NET_CORE_IF_ETHER_IPv6_STATIC_LOCAL_ADDR,
  .IPv6.Static.PrefixLen = EX_NET_CORE_IF_ETHER_IPv6_STATIC_LOCAL_PREFIX_LEN,
  .IPv6.Static.DAD_En = EX_NET_CORE_IF_ETHER_IPv6_DAD_EN,
  .IPv6.AutoCfg.En = EX_NET_CORE_IF_ETHER_IPv6_AUTOCONF_EN,
  .IPv6.AutoCfg.DAD_En = EX_NET_CORE_IF_ETHER_IPv6_AUTOCONF_DAD_EN,
  .IPv6.Hook = Ex_IPv6_AddrCfgResult
#else
  .IPv6.Static.Addr = DEF_NULL,
  .IPv6.AutoCfg.En = DEF_NO,
#endif
};
#endif

/********************************************************************************************************
 *                                   START SETUP FOR WIFI INTERFACE
 *******************************************************************************************************/

/********************************************************************************************************
 *                           NETWORK BUFFER CONFIGURATION FOR WIFI INTERFACE
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_NET_IF_WIFI_AVAIL

#ifndef  EX_NET_CORE_IF_WIFI_BUF_RX_LARGE_NBR
#define  EX_NET_CORE_IF_WIFI_BUF_RX_LARGE_NBR                10u
#endif

#ifndef  EX_NET_CORE_IF_WIFI_BUF_TX_LARGE_NBR
#define  EX_NET_CORE_IF_WIFI_BUF_TX_LARGE_NBR                8u
#endif

#ifndef  EX_NET_CORE_IF_WIFI_BUF_TX_SMALL_NBR
#define  EX_NET_CORE_IF_WIFI_BUF_TX_SMALL_NBR                4u
#endif

NET_IF_BUF_CFG Ex_NetWiFi_IF_BufCfg = {
  .RxBufLargeNbr = EX_NET_CORE_IF_WIFI_BUF_RX_LARGE_NBR,
  .TxBufLargeNbr = EX_NET_CORE_IF_WIFI_BUF_TX_LARGE_NBR,
  .TxBufSmallNbr = EX_NET_CORE_IF_WIFI_BUF_TX_SMALL_NBR,
};

NET_IF_WIFI_CFG Ex_NetIF_CfgDflt_WiFi = {
  .HW_AddrStr = EX_NET_CORE_IF_WIFI_MAC_ADDR,

#if  (NET_IPv4_CFG_EN == DEF_ENABLED)
  .IPv4.Static.Addr = EX_NET_CORE_IF_WIFI_IPv4_STATIC_ADDR,
  .IPv4.Static.Mask = EX_NET_CORE_IF_WIFI_IPv4_STATIC_MASK,
  .IPv4.Static.Gateway = EX_NET_CORE_IF_WIFI_IPv4_STATIC_GATEWAY,

#ifdef  NET_DHCP_CLIENT_MODULE_EN
  .IPv4.DHCPc.En = EX_NET_CORE_IF_WIFI_IPv4_DHCP_EN,
#else
  .IPv4.DHCPc.En = DEF_NO,
#endif

  .IPv4.DHCPc.Cfg = DHCPc_CFG_DFLT,
  .IPv4.DHCPc.OnCompleteHook = Ex_DHCPc_SetupResult,

  .IPv4.LinkLocal.En = DEF_NO,
  .IPv4.LinkLocal.OnCompleteHook = Ex_IPv4_LinkLocalAddrCfgResult,
#else
  .IPv4.Static.Addr = DEF_NULL,
  .IPv4.Static.Mask = DEF_NULL,
  .IPv4.Static.Gateway = DEF_NULL,
  .IPv4.DHCPc.En = DEF_NO,
  .IPv4.LinkLocal.En = DEF_NO,
#endif

#if  (NET_IPv6_CFG_EN == DEF_ENABLED)
  .IPv6.Static.Addr = EX_NET_CORE_IF_WIFI_IPv6_STATIC_LOCAL_ADDR,
  .IPv6.Static.PrefixLen = EX_NET_CORE_IF_WIFI_IPv6_STATIC_LOCAL_PREFIX_LEN,
  .IPv6.Static.DAD_En = EX_NET_CORE_IF_WIFI_IPv6_DAD_EN,
  .IPv6.AutoCfg.En = EX_NET_CORE_IF_WIFI_IPv6_AUTOCONF_EN,
  .IPv6.AutoCfg.DAD_En = EX_NET_CORE_IF_WIFI_IPv6_AUTOCONF_DAD_EN,
  .IPv6.Hook = Ex_IPv6_AddrCfgResult,
#else
  .IPv6.Static.Addr = DEF_NULL,
  .IPv6.AutoCfg.En = DEF_NO,
#endif

  .Band = EX_NET_CORE_IF_WIFI_BAND
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                            Ex_Net_CoreInit()
 *
 * @brief  Initialize the Network TCP-IP core and add an Ethernet Interface and/or a WiFi Interface.
 *
 * @note   (1) Simplest initialization without any overwrite of default configuration.
 *******************************************************************************************************/
void Ex_Net_CoreInit(void)
{
  NET_IF_NBR if_nbr;
  RTOS_ERR   err;

  //                                                               ---------- CHANGES DFLT TASKS STACK SIZE -----------
#ifdef  EX_NET_CORE_TASK_STK_SIZE
  Net_ConfigureCoreTaskStk(EX_NET_CORE_TASK_STK_SIZE, DEF_NULL);
#endif

#ifdef  EX_NET_CORE_SRV_TASK_STK_SIZE
  Net_ConfigureCoreSvcTaskStk(EX_NET_CORE_SRV_TASK_STK_SIZE, DEF_NULL);
#endif

  //                                                               -------- INITIALIZE NETWORK TASKS & OBJECTS --------
  Net_Init(&err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );

  //                                                               ------------- CHANGES DFLT TASKS PRIO --------------
#ifdef  EX_NET_CORE_TASK_PRIO
  Net_CoreTaskPrioSet(EX_NET_CORE_TASK_PRIO, DEF_NULL);
#endif

#ifdef  EX_NET_CORE_SRV_TASK_PRIO
  Net_CoreSvcTaskPrioSet(EX_NET_CORE_SRV_TASK_PRIO, DEF_NULL);
#endif

#ifdef  RTOS_MODULE_NET_IF_ETHER_AVAIL
  //                                                               -------------- ADD ETHERNET INTERFACE --------------
  if_nbr = NetIF_Ether_Add("eth0",
                           Ex_NetEther_CfgPtr,
                           DEF_NULL,
                           &err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );
#endif

#ifdef  RTOS_MODULE_NET_IF_WIFI_AVAIL

  /* Before you can add a WiFi Interface, you need to make sure that the interface was
     registered in the BSP. Since WiFi part are often daughter-boards, the BSP cannot
     assume which model is connected to the main board. Therefore, you must manually
     modify the BSP file by enabling the code related to the WiFi part you are using
     in your project. */

  //                                                               -------------- ADD WIRELESS INTERFACE --------------
  if_nbr = NetIF_WiFi_Add("wifi0",
                          &Ex_NetWiFi_IF_BufCfg,
                          DEF_NULL,
                          DEF_NULL,
                          &err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );
#endif

  PP_UNUSED_PARAM(if_nbr);
}

/****************************************************************************************************//**
 *                                          Ex_Net_CoreStartIF()
 *
 * @brief  Starts network interface(s).
 *
 * @note   (1) If the Ethernet module is available and the "eth0" was registered and added,
 *             this function will start that network interface.
 *
 * @note   (2) If the WiFi module is available and the "wifi0" was registered and added,
 *             this function will start that network interface.
 *******************************************************************************************************/
void Ex_Net_CoreStartIF(void)
{
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  NET_IF_NBR if_nbr;
  RTOS_ERR   err;
#endif

  //                                                               ------------- START ETHERNET INTERFACE -------------
#ifdef  RTOS_MODULE_NET_IF_ETHER_AVAIL
  Ex_Net_CoreStartEther();

#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  //                                                               -------- WAIT FOR INTEFACE SETUP TO FINISH ---------
  if_nbr = NetIF_NbrGetFromName("eth0");                        // Recover interface id from interface's name.
  APP_RTOS_ASSERT_CRITICAL((if_nbr != NET_IF_NBR_NONE),; );

  NetIF_WaitSetupReady(if_nbr, DEF_NULL, 0, &err);
#endif
#endif

  //                                                               --------------- START WIFI INTERFACE ---------------
#ifdef  RTOS_MODULE_NET_IF_WIFI_AVAIL
  Ex_Net_CoreStartWiFi();

#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  //                                                               -------- WAIT FOR INTEFACE SETUP TO FINISH ---------
  if_nbr = NetIF_NbrGetFromName("wifi0");                       // Recover interface id from interface's name.
  APP_RTOS_ASSERT_CRITICAL((if_nbr != NET_IF_NBR_NONE),; );

  NetIF_WaitSetupReady(if_nbr, DEF_NULL, 0, &err);
#endif
#endif
}

/****************************************************************************************************//**
 *                                         Ex_Net_CoreStartEther()
 *
 * @brief  Start an Ethernet Interface.
 *
 * @note   (1) In this example no NET_IF_ETHER_CFG structure is passed to the start function.
 *             Therefore, no address setup is done by the start function and it must be done
 *             explicitly with DHCP client, IPv4 and IPv6 API calls, and after the call to
 *             NetIF_Ether_Start().
 *******************************************************************************************************/
#ifdef  RTOS_MODULE_NET_IF_ETHER_AVAIL
void Ex_Net_CoreStartEther(void)
{
  NET_IF_NBR if_nbr;
  RTOS_ERR   err;

  //                                                               --- RECOVER INTERFACE NUMBER FROM INTEFACE NAME ----
  if_nbr = NetIF_NbrGetFromName("eth0");
  APP_RTOS_ASSERT_CRITICAL((if_nbr != NET_IF_NBR_NONE),; );

  //                                                               ------------- START ETHERNET INTERFACE -------------
  NetIF_Ether_Start(if_nbr, &Ex_NetIF_CfgDflt_Ether, &err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );
}
#endif

/****************************************************************************************************//**
 *                                         Ex_Net_CoreStartWiFi()
 *
 * @brief  Start an WiFi Interface.
 *
 * @note   (1) In this example no NET_IF_WIFI_CFG structure is passed to the start function.
 *             Therefore, no address setup is done by the start function and it must be done
 *             explicitly with DHCP client, IPv4 and IPv6 API calls, and after the call to
 *             NetIF_Ether_Start().
 *******************************************************************************************************/
#ifdef  RTOS_MODULE_NET_IF_WIFI_AVAIL
void Ex_Net_CoreStartWiFi(void)
{
  NET_IF_NBR       if_nbr;
  NET_IF_WIFI_SSID ssid;
  NET_IF_WIFI_PSK  psk;
#if  (EX_NET_CORE_IF_WIFI_SCAN_EN == DEF_ENABLED)
  NET_IF_WIFI_SSID *p_ssid;
  NET_IF_WIFI_AP   ap_tbl[10];
  CPU_INT16U       ctn;
  CPU_INT16S       cmp_result;
  CPU_INT08U       i;
  CPU_BOOLEAN      found;
#endif
  RTOS_ERR err;

  //                                                               --- RECOVER INTERFACE NUMBER FROM INTEFACE NAME ----
  if_nbr = NetIF_NbrGetFromName("wifi0");
  APP_RTOS_ASSERT_CRITICAL((if_nbr != NET_IF_NBR_NONE),; );

  //                                                               --------------- START WIFI INTERFACE ---------------
  NetIF_WiFi_Start(if_nbr, &Ex_NetIF_CfgDflt_WiFi, &err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );

  //                                                               ------------ SCAN FOR WIRELESS NETWORKS ------------
#if  (EX_NET_CORE_IF_WIFI_SCAN_EN == DEF_ENABLED)
  ctn = NetIF_WiFi_Scan(if_nbr,
                        ap_tbl,
                        10,
                        DEF_NULL,
                        NET_IF_WIFI_CH_ALL,
                        &err);
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );

  //                                                               --------- ANALYSE WIRELESS NETWORKS FOUND ----------
  found = DEF_NO;
  for (i = 0u; i < ctn - 1; i++) {                              // Browse table of access point found.
    p_ssid = &ap_tbl[i].SSID;
    cmp_result = Str_Cmp_N((CPU_CHAR *)p_ssid,                  // Search for a specific Wireless Network SSID.
                           EX_NET_CORE_IF_WIFI_SSID,            // WiFi Network SSID.
                           NET_IF_WIFI_STR_LEN_MAX_SSID);
    if (cmp_result == 0) {
      found = DEF_YES;
      break;
    }
  }

  if (found == DEF_NO) {
    return;
  }
#endif
  //                                                               ------------- JOIN A WIRELESS NETWORK --------------
  Mem_Clr(&ssid, sizeof(ssid));
  Mem_Clr(&psk, sizeof(psk));
  Str_Copy_N((CPU_CHAR *)&ssid,
             EX_NET_CORE_IF_WIFI_SSID,                          // WiFi Network SSID.
             sizeof(EX_NET_CORE_IF_WIFI_SSID));                 // SSID string length.
  Str_Copy_N((CPU_CHAR *)&psk,
             EX_NET_CORE_IF_WIFI_PASSWORD,                      // WiFi Network Password.
             sizeof(EX_NET_CORE_IF_WIFI_PASSWORD));                // PSK string length.

#if  (EX_NET_CORE_IF_WIFI_SCAN_EN == DEF_ENABLED)
  NetIF_WiFi_Join(if_nbr,
                  ap_tbl[i].NetType,
                  NET_IF_WIFI_DATA_RATE_AUTO,
                  ap_tbl[i].SecurityType,
                  NET_IF_WIFI_PWR_LEVEL_HI,
                  ssid,
                  psk,
                  &err);
#else
  NetIF_WiFi_Join(if_nbr,
                  EX_NET_CORE_IF_WIFI_NET_TYPE,
                  NET_IF_WIFI_DATA_RATE_AUTO,
                  EX_NET_CORE_IF_WIFI_SECURITY_TYPE,
                  NET_IF_WIFI_PWR_LEVEL_HI,
                  ssid,
                  psk,
                  &err);
#endif
  APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         Ex_IPv6_AddrCfgResult()
 *
 * @brief  Hook function to received any IPv6 address configuration process result.
 *
 * @param  if_nbr           Network Interface number on which address configuration occurred.
 *
 * @param  addr_type        IPv6 address type:
 *                             - NET_IPv6_CFG_ADDR_TYPE_STATIC
 *                             - NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL
 *                             - NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL
 *
 * @param  p_addr_cfgd      Pointer to IPv6 address configured, if any.
 *                          DEF_NULL, otherwise.
 *
 * @param  addr_cfg_status  Result status of the IPv6 address configuration process:
 *                             - NET_IPv6_ADDR_CFG_STATUS_SUCCEED
 *                             - NET_IPv6_ADDR_CFG_STATUS_FAIL
 *                             - NET_IPv6_ADDR_CFG_STATUS_DUPLICATE
 *******************************************************************************************************/
#if  (defined(EX_NET_CORE_IF_EXT_EN) \
  && (NET_IPv6_CFG_EN == DEF_ENABLED))
static void Ex_IPv6_AddrCfgResult(NET_IF_NBR               if_nbr,
                                  NET_IPv6_CFG_ADDR_TYPE   addr_type,
                                  const NET_IPv6_ADDR      *p_addr_cfgd,
                                  NET_IPv6_ADDR_CFG_STATUS addr_cfg_status)
{
  CPU_CHAR ip_string[NET_ASCII_LEN_MAX_ADDR_IPv6];
  RTOS_ERR err;

  PP_UNUSED_PARAM(if_nbr);

  if (p_addr_cfgd != DEF_NULL) {
    NetASCII_IPv6_to_Str((NET_IPv6_ADDR *)p_addr_cfgd, ip_string, DEF_NO, DEF_YES, &err);
    APP_RTOS_ASSERT_CRITICAL((err.Code == RTOS_ERR_NONE),; );
    EX_TRACE("IPv6 Configured Address: %s\n", ip_string);
  }

  switch (addr_type) {
    case NET_IPv6_CFG_ADDR_TYPE_STATIC:
      switch (addr_cfg_status) {
        case NET_IPv6_ADDR_CFG_STATUS_SUCCEED:
          EX_TRACE("IPv6 Address Static    : %s, configured successfully\n", ip_string);
          break;

        case NET_IPv6_ADDR_CFG_STATUS_DUPLICATE:
          EX_TRACE("IPv6 Address Static already exists on the network\n");
          break;

        default:
          EX_TRACE("IPv6 Address Static configuration failed.\n");
          break;
      }
      break;

    case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL:
      switch (addr_cfg_status) {
        case NET_IPv6_ADDR_CFG_STATUS_SUCCEED:
          EX_TRACE("IPv6 Address Link Local: %s, configured successfully\n", ip_string);
          break;

        case NET_IPv6_ADDR_CFG_STATUS_DUPLICATE:
          EX_TRACE("IPv6 Address Link Local already exists on the network\n");
          break;

        default:
          EX_TRACE("IPv6 Address Link Local configuration failed.\n");
          break;
      }
      break;

    case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL:
      switch (addr_cfg_status) {
        case NET_IPv6_ADDR_CFG_STATUS_SUCCEED:
          EX_TRACE("IPv6 Address Global    : %s, configured successfully\n", ip_string);
          break;

        case NET_IPv6_ADDR_CFG_STATUS_DUPLICATE:
          EX_TRACE("IPv6 Address Global already exists on the network\n");
          break;

        default:
          EX_TRACE("IPv6 Address Global configuration failed.\n");
          break;
      }
      break;

    default:
      break;
  }
}
#endif

/****************************************************************************************************//**
 *                                         Ex_DHCPc_SetupResult()
 *
 * @brief  Hook function called when DHCP process has been complete on an network interface.
 *
 * @param  if_nbr   Network Interface number on which DHCP process occurred.
 *
 * @param  status   DHCP process status:
 *                    - DHCPc_STATUS_SUCCESS
 *                    - DHCPc_STATUS_FAIL_ADDR_USED
 *                    - DHCPc_STATUS_FAIL_OFFER_DECLINE
 *                    - DHCPc_STATUS_FAIL_NAK_RX
 *                    - DHCPc_STATUS_FAIL_NO_SERVER
 *                    - DHCPc_STATUS_FAIL_ERR_FAULT
 *
 * @param  addr     IPv4 address configured, if any.
 *                  Else, NET_IPv4_ADDR_NONE.
 *
 * @param  mask     IPv4 mask configured, if any.
 *                  Else, NET_IPv4_ADDR_NONE.
 *
 * @param  gateway  IPv4 gateway configured, if any.
 *                  Else, NET_IPv4_ADDR_NONE.
 *
 * @param  err      Error object in case an error occurred.
 *                  Else, RTOS_ERR_NONE.
 *******************************************************************************************************/
#if  (defined(EX_NET_CORE_IF_EXT_EN) \
  && (NET_IPv4_CFG_EN == DEF_ENABLED))
static void Ex_DHCPc_SetupResult(NET_IF_NBR    if_nbr,
                                 DHCPc_STATUS  status,
                                 NET_IPv4_ADDR addr,
                                 NET_IPv4_ADDR mask,
                                 NET_IPv4_ADDR gateway,
                                 RTOS_ERR      err)
{
  CPU_CHAR addr_string[NET_ASCII_LEN_MAX_ADDR_IPv4];
  CPU_CHAR mask_string[NET_ASCII_LEN_MAX_ADDR_IPv4];
  CPU_CHAR gateway_string[NET_ASCII_LEN_MAX_ADDR_IPv4];
  RTOS_ERR local_err;

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
#if ((RTOS_ERR_CFG_EXT_EN == DEF_ENABLED) \
      && (RTOS_ERR_CFG_STR_EN == DEF_ENABLED))
      EX_TRACE("DHCP client process failed: Fatal error %s.\n", err.CodeText);
#else
      EX_TRACE("DHCP client process failed: Fatal error %i.\n", err.Code);
#endif
      break;

    default:
      EX_TRACE("DHCP client invalid result.\n");
      break;
  }
}
#endif

/****************************************************************************************************//**
 *                                    Ex_IPv4_LinkLocalAddrCfgResult()
 *
 * @brief  Hook function called when IPv4 Link local process has been complete on an network interface.
 *
 * @param  if_nbr           Network Interface number on which IPv4 link local process occurred.
 *
 * @param  link_local_addr  IPv4 link local address, if configured.
 *                          Else, NET_IPv4_ADDR_NONE.
 *
 * @param  status           IPv4 Link Local process status:
 *                              - NET_IPv4_LINK_LOCAL_STATUS_NONE
 *                              - NET_IPv4_LINK_LOCAL_STATUS_SUCCEEDED
 *                              - NET_IPv4_LINK_LOCAL_STATUS_FAILED
 *
 * @param  err              Error object in case an error occurred.
 *                          Else, RTOS_ERR_NONE.
 *
 * @note   (1) IPv4 link local address configuration will only be started if its enabled in the
 *             NET_IF_ETHER_CFG object, but also if no other IPv4 valid addresses are configured on
 *             the network interface.
 *             Therefore, it will occurred if :
 *               - (a)  no IPv4 static address is set up and DHCP process is disabled.
 *               - (b)  the DHCP setup fails and no IPv4 static address is set up.
 *******************************************************************************************************/
#if  (defined(EX_NET_CORE_IF_EXT_EN) \
  && (NET_IPv4_CFG_EN == DEF_ENABLED))
static void Ex_IPv4_LinkLocalAddrCfgResult(NET_IF_NBR                 if_nbr,
                                           NET_IPv4_ADDR              link_local_addr,
                                           NET_IPv4_LINK_LOCAL_STATUS status,
                                           RTOS_ERR                   err)
{
  CPU_CHAR addr_str[NET_ASCII_LEN_MAX_ADDR_IPv4];
  RTOS_ERR local_err;

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
#if ((RTOS_ERR_CFG_EXT_EN == DEF_ENABLED) \
      && (RTOS_ERR_CFG_STR_EN == DEF_ENABLED))
      EX_TRACE("IPv4 link local address configuration failed: %s.\n", err.CodeText);
#else
      EX_TRACE("IPv4 link local address configuration failed: %i.\n", err.Code);
#endif
      break;

    default:
      break;
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
