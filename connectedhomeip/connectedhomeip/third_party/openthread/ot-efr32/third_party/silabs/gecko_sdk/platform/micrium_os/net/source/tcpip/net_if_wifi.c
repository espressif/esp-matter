/***************************************************************************//**
 * @file
 * @brief Network Interface Layer - Wireless
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

#include  <net/include/net_cfg_net.h>

#ifdef  NET_IF_WIFI_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "net_if_wifi_priv.h"
#include  "net_if_priv.h"
#include  "net_priv.h"
#include  "net_if_802x_priv.h"
#include  "net_type_priv.h"

#include  <common/include/lib_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/platform_mgr/platform_mgr_priv.h>

#include  <net/include/net_if.h>
#include  <net/include/net_type.h>
#include  <net/include/net_ascii.h>
#include  <net/include/dhcp_client.h>

#ifdef NET_IPv4_MODULE_EN
#include  <net/include/net_ipv4.h>
#endif

#ifdef NET_IPv6_MODULE_EN
#include  <net/include/net_ipv6.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------------------- RX FNCTS ---------------------
static void NetIF_WiFi_Rx(NET_IF   *p_if,
                          NET_BUF  *p_buf,
                          RTOS_ERR *p_err);

static void NetIF_WiFi_RxPktHandler(NET_IF   *p_if,
                                    NET_BUF  *p_buf,
                                    RTOS_ERR *p_err);

static void NetIF_WiFi_RxMgmtFrameHandler(NET_IF   *p_if,
                                          NET_BUF  *p_buf,
                                          RTOS_ERR *p_err);

//                                                                 --------------------- TX FNCTS ---------------------
static void NetIF_WiFi_Tx(NET_IF   *p_if,
                          NET_BUF  *p_buf,
                          RTOS_ERR *p_err);

//                                                                 -------------------- API FNCTS ---------------------
static void NetIF_WiFi_IF_Add(NET_IF   *p_if,
                              RTOS_ERR *p_err);

static void NetIF_WiFi_IF_Start(NET_IF   *p_if,
                                RTOS_ERR *p_err);

static void NetIF_WiFi_IF_Stop(NET_IF   *p_if,
                               RTOS_ERR *p_err);

//                                                                 -------------------- MGMT FNCTS --------------------
static void NetIF_WiFi_IO_CtrlHandler(NET_IF     *p_if,
                                      CPU_INT08U opt,
                                      void       *p_data,
                                      RTOS_ERR   *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const NET_WIFI_MGR_API NetWiFiMgr_API_Generic;

/********************************************************************************************************
 *                                           INTERFACE CFG DEFAULT
 *******************************************************************************************************/

const NET_IF_WIFI_CFG NetIF_WiFi_CfgDflt = {
  .HW_AddrStr = DEF_NULL,
  .IPv4.Static.Addr = DEF_NULL,
  .IPv4.Static.Mask = DEF_NULL,
  .IPv4.Static.Gateway = DEF_NULL,
  .IPv4.DHCPc.En = DEF_YES,
  .IPv4.DHCPc.Cfg = DHCPc_CFG_DFLT,
  .IPv4.DHCPc.OnCompleteHook = DEF_NULL,
  .IPv4.LinkLocal.En = DEF_NO,
  .IPv4.LinkLocal.OnCompleteHook = DEF_NULL,
  .IPv6.Static.Addr = DEF_NULL,
  .IPv6.Static.PrefixLen = 0,
  .IPv6.Static.DAD_En = DEF_NO,
  .IPv6.AutoCfg.En = DEF_YES,
  .IPv6.AutoCfg.DAD_En = DEF_YES,
  .IPv6.Hook = DEF_NULL,
  .Band = NET_DEV_BAND_2_4_GHZ
};

/********************************************************************************************************
 *                                               INTERFACE API
 *******************************************************************************************************/

const NET_IF_API NetIF_API_WiFi = {                                             // WiFi IF API fnct ptrs :
  &NetIF_WiFi_IF_Add,                                                           // Init/add
  &NetIF_WiFi_IF_Start,                                                         // Start
  &NetIF_WiFi_IF_Stop,                                                          // Stop
  &NetIF_WiFi_Rx,                                                               // Rx
  &NetIF_WiFi_Tx,                                                               // Tx
  &NetIF_802x_AddrHW_Get,                                                       // Hw        addr get
  &NetIF_802x_AddrHW_Set,                                                       // Hw        addr set
  &NetIF_802x_AddrHW_IsValid,                                                   // Hw        addr valid
  &NetIF_802x_AddrMulticastAdd,                                                 // Multicast addr add
  &NetIF_802x_AddrMulticastRemove,                                              // Multicast addr remove
  &NetIF_802x_AddrMulticastProtocolToHW,                                        // Multicast addr protocol-to-hw
  &NetIF_802x_BufPoolCfgValidate,                                               // Buf cfg validation
  &NetIF_802x_MTU_Set,                                                          // MTU set
  &NetIF_802x_GetPktSizeHdr,                                                    // Get pkt hdr size
  &NetIF_802x_GetPktSizeMin,                                                    // Get pkt min size
  &NetIF_802x_GetPktSizeMax,
  &NetIF_802x_ISR_Handler,                                                      // ISR handler
  &NetIF_WiFi_IO_CtrlHandler                                                    // I/O ctrl
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIF_WiFi_Add()
 *
 * @brief    Add & initialize a specific instance of a network WiFi interface.
 *
 * @param    p_str_id    String identifier for the WiFi interface to add.
 *
 * @param    p_buf_cfg   Pointer to buffer configuration.
 *
 * @param    p_ext_cfg   Pointer to extended configuration.
 *                       DEF_NULL, if not need by WiFi part.
 *
 * @param    p_mem_seg   Memory segment from which internal data will be allocated.
 *                       If DEF_NULL, it will be allocated from the global heap.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_NO_MORE_RSRC
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *
 * @return   Interface number of the added interface, if NO error(s).
 *           NET_IF_NBR_NONE, otherwise.
 *******************************************************************************************************/
NET_IF_NBR NetIF_WiFi_Add(CPU_CHAR             *p_name,
                          const NET_IF_BUF_CFG *p_buf_cfg,
                          void                 *p_ext_cfg,
                          MEM_SEG              *p_mem_seg,
                          RTOS_ERR             *p_err)
{
  NET_IF_WIFI_HW_INFO *p_hw_info = DEF_NULL;
  NET_DEV_CFG_WIFI    *p_dev_cfg = DEF_NULL;
  NET_IF              *p_if = DEF_NULL;
  NET_IF_NBR          if_nbr = NET_IF_NBR_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_IF_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_buf_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IF_NBR_NONE);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------ GET ITEM FROM PLATFORM MGR ------------
  p_hw_info = (NET_IF_WIFI_HW_INFO *)PlatformMgrItemGetByName(p_name, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (if_nbr);
  }

  //                                                               ------- ALLOCATE INTERNAL DEVICE CFG OBJECT --------
  p_dev_cfg = (NET_DEV_CFG_WIFI *)Mem_SegAlloc("WiFi device cfg",
                                               p_mem_seg,
                                               sizeof(NET_DEV_CFG_WIFI),
                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (if_nbr);
  }

  //                                                               -------- INIT OF THE DEVICE CFG PARAMETERS ---------
  p_dev_cfg->CfgExtPtr = p_ext_cfg;
  p_dev_cfg->Flags = NET_FLAG_NONE;
  p_dev_cfg->MemAddr = 0;
  p_dev_cfg->MemSize = 0;
  p_dev_cfg->RxBufPoolType = NET_IF_MEM_TYPE_MAIN;
  p_dev_cfg->TxBufPoolType = NET_IF_MEM_TYPE_MAIN;
  p_dev_cfg->RxBufIxOffset = p_hw_info->PartInfoPtr->RxBufIxOffset;
  p_dev_cfg->TxBufIxOffset = p_hw_info->PartInfoPtr->TxBufIxOffset;
  p_dev_cfg->RxBufAlignOctets = sizeof(CPU_ALIGN);
  p_dev_cfg->TxBufAlignOctets = sizeof(CPU_ALIGN);
  p_dev_cfg->RxBufLargeNbr = p_buf_cfg->RxBufLargeNbr;
  p_dev_cfg->TxBufLargeNbr = p_buf_cfg->TxBufLargeNbr;
  p_dev_cfg->TxBufSmallNbr = p_buf_cfg->TxBufSmallNbr;
  p_dev_cfg->RxBufLargeSize = NET_IF_WIFI_CFG_RX_BUF_LARGE_SIZE;
  p_dev_cfg->TxBufLargeSize = NET_IF_WIFI_CFG_TX_BUF_LARGE_SIZE;
  p_dev_cfg->TxBufSmallSize = NET_IF_WIFI_CFG_RX_BUF_SMALL_SIZE;

  //                                                               -------------- ADD NEW WIFI INTERFACE --------------
  if_nbr = NetIF_Add((void *)&NetIF_API_WiFi,
                     (void *) p_hw_info->PartInfoPtr->DrvAPI_Ptr,
                     (void *) p_hw_info->BSP_API_Ptr,
                     (void *) p_dev_cfg,
                     (void *)&NetWiFiMgr_API_Generic,
                     (void *) p_hw_info,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (if_nbr);
  }

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_Add);

  p_if = NetIF_Get(if_nbr, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(*p_err), NET_IF_NBR_NONE);

  p_if->Name = (CPU_CHAR *)p_name;

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  PP_UNUSED_PARAM(p_mem_seg);

  return (if_nbr);
}

/****************************************************************************************************//**
 *                                           NetIF_WiFi_Start()
 *
 * @brief    (1) Start an WiFi type interface :
 *               - (a) Set the MAC address of the interface, if available.
 *               - (b) Start the controller interface.
 *               - (b) Set IPv4 and IPv6 address, if available.
 *               - (c) Enable DHCP client process, if available.
 *
 * @param    if_nbr  Network interface number to start.
 *
 * @param    p_cfg   Pointer to interface configuration.
 *                   DEF_NULL, to just start the interface without Address or DHCP setup.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_TYPE
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_NOT_SUPPORTED
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_FAIL
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_NET_IF_LINK_DOWN
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_INVALID_STATE
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *                       - RTOS_ERR_NOT_INIT
 *                       - RTOS_ERR_TX
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_NET_STR_ADDR_INVALID
 *                       - RTOS_ERR_RX
 *                       - RTOS_ERR_NOT_READY
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_NET_NEXT_HOP
 *                       - RTOS_ERR_IS_OWNER
 *******************************************************************************************************/
void NetIF_WiFi_Start(NET_IF_NBR      if_nbr,
                      NET_IF_WIFI_CFG *p_cfg,
                      RTOS_ERR        *p_err)
{
  NET_IF           *p_if = DEF_NULL;
  NET_IF_DATA_WIFI *p_if_data = DEF_NULL;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_if_data = (NET_IF_DATA_WIFI *)p_if->IF_Data;

  if (p_cfg != DEF_NULL) {
    p_if_data->Band = p_cfg->Band;
  } else {
    p_if_data->Band = NET_DEV_BAND_2_4_GHZ;
  }

  NetIF_StartInternal(if_nbr, (NET_IF_CFG *)p_cfg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                               NetIF_WiFi_Scan()
 *
 * @brief    Scan available wireless access point.
 *
 * @param    if_nbr              Wireless network interface number.
 *
 * @param    p_buf_scan          Pointer to a buffer that will receive available access point.
 *
 * @param    buf_scan_len_max    Maximum number of access point that can be stored
 *
 * @param    p_ssid              Pointer to a string that contains the SSID to scan.
 *                               DEF_NULL to scan for all access point.
 *
 * @param    ch                  Channel number:
 *                                   - NET_IF_WIFI_CH_ALL
 *                                   - NET_IF_WIFI_CH_1
 *                                   - NET_IF_WIFI_CH_2
 *                                   - NET_IF_WIFI_CH_3
 *                                   - NET_IF_WIFI_CH_4
 *                                   - NET_IF_WIFI_CH_5
 *                                   - NET_IF_WIFI_CH_6
 *                                   - NET_IF_WIFI_CH_7
 *                                   - NET_IF_WIFI_CH_8
 *                                   - NET_IF_WIFI_CH_9
 *                                   - NET_IF_WIFI_CH_10
 *                                   - NET_IF_WIFI_CH_11
 *                                   - NET_IF_WIFI_CH_12
 *                                   - NET_IF_WIFI_CH_13
 *                                   - NET_IF_WIFI_CH_14
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_NOT_READY
 *
 * @return   Number of wireless access point found.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *           - (a) MUST NOT be called with the global network lock already acquired;
 *           - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                   network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
CPU_INT16U NetIF_WiFi_Scan(NET_IF_NBR             if_nbr,
                           NET_IF_WIFI_AP         *p_buf_scan,
                           CPU_INT16U             buf_scan_len_max,
                           const NET_IF_WIFI_SSID *p_ssid,
                           NET_IF_WIFI_CH         ch,
                           RTOS_ERR               *p_err)
{
  NET_IF           *p_if = DEF_NULL;
  NET_WIFI_MGR_API *p_mgr_api = DEF_NULL;
  CPU_INT16U       ctn = 0u;

  //                                                               ------------------- VALIDATE ARG -------------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_buf_scan != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, ctn);
  RTOS_ASSERT_DBG_ERR_SET((ch <= NET_IF_WIFI_CH_MAX), *p_err, RTOS_ERR_INVALID_ARG, ctn);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Net_GlobalLockAcquire((void *)NetIF_WiFi_Scan);

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_mgr_api = (NET_WIFI_MGR_API *)p_if->Ext_API;

  ctn = p_mgr_api->AP_Scan(p_if, p_buf_scan, buf_scan_len_max, p_ssid, ch, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    ctn = 0u;
    goto exit_release;
  }

exit_release:
  Net_GlobalLockRelease();
  return (ctn);
}

/****************************************************************************************************//**
 *                                               NetIF_WiFi_Join()
 *
 * @brief    Join a wireless access point.
 *
 * @param    if_nbr          Wireless network interface number.
 *
 * @param    net_type        Wireless network type:
 *                               - NET_IF_WIFI_NET_TYPE_INFRASTRUCTURE
 *                               - NET_IF_WIFI_NET_TYPE_ADHOC
 *
 * @param    data_rate       Wireless date rate to configure:
 *                               - NET_IF_WIFI_DATA_RATE_AUTO
 *                               - NET_IF_WIFI_DATA_RATE_1_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_2_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_5_5_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_6_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_9_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_11_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_12_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_18_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_24_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_36_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_48_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_54_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_MCS0
 *                               - NET_IF_WIFI_DATA_RATE_MCS1
 *                               - NET_IF_WIFI_DATA_RATE_MCS2
 *                               - NET_IF_WIFI_DATA_RATE_MCS3
 *                               - NET_IF_WIFI_DATA_RATE_MCS4
 *                               - NET_IF_WIFI_DATA_RATE_MCS5
 *                               - NET_IF_WIFI_DATA_RATE_MCS6
 *                               - NET_IF_WIFI_DATA_RATE_MCS7
 *                               - NET_IF_WIFI_DATA_RATE_MCS8
 *                               - NET_IF_WIFI_DATA_RATE_MCS9
 *                               - NET_IF_WIFI_DATA_RATE_MCS10
 *                               - NET_IF_WIFI_DATA_RATE_MCS11
 *                               - NET_IF_WIFI_DATA_RATE_MCS12
 *                               - NET_IF_WIFI_DATA_RATE_MCS13
 *                               - NET_IF_WIFI_DATA_RATE_MCS14
 *                               - NET_IF_WIFI_DATA_RATE_MCS15
 *
 * @param    security_type   Wireless security type:
 *                               - NET_IF_WIFI_SECURITY_OPEN
 *                               - NET_IF_WIFI_SECURITY_WEP
 *                               - NET_IF_WIFI_SECURITY_WPA
 *                               - NET_IF_WIFI_SECURITY_WPA2
 *
 * @param    pwr_level       Wireless radio power to configure:
 *                               - NET_IF_WIFI_PWR_LEVEL_LO
 *                               - NET_IF_WIFI_PWR_LEVEL_MED
 *                               - NET_IF_WIFI_PWR_LEVEL_HI
 *
 * @param    ssid            SSID of the access point to join.
 *
 * @param    psk             Pre shared key of the access point.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *           - (a) MUST NOT be called with the global network lock already acquired;
 *           - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                   network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) Before join an access point, the access point should have been found during a
 *               previous scan.
 *******************************************************************************************************/
void NetIF_WiFi_Join(NET_IF_NBR                if_nbr,
                     NET_IF_WIFI_NET_TYPE      net_type,
                     NET_IF_WIFI_DATA_RATE     data_rate,
                     NET_IF_WIFI_SECURITY_TYPE security_type,
                     NET_IF_WIFI_PWR_LEVEL     pwr_level,
                     NET_IF_WIFI_SSID          ssid,
                     NET_IF_WIFI_PSK           psk,
                     RTOS_ERR                  *p_err)
{
  NET_IF             *p_if;
  NET_WIFI_MGR_API   *p_mgr_api;
  NET_IF_WIFI_AP_CFG ap_cfg;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; )

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)                        // ------------------- VALIDATE ARG -------------------
  switch (net_type) {
    case NET_IF_WIFI_NET_TYPE_INFRASTRUCTURE:
    case NET_IF_WIFI_NET_TYPE_ADHOC:
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  switch (data_rate) {
    case NET_IF_WIFI_DATA_RATE_AUTO:
    case NET_IF_WIFI_DATA_RATE_1_MBPS:
    case NET_IF_WIFI_DATA_RATE_2_MBPS:
    case NET_IF_WIFI_DATA_RATE_5_5_MBPS:
    case NET_IF_WIFI_DATA_RATE_6_MBPS:
    case NET_IF_WIFI_DATA_RATE_9_MBPS:
    case NET_IF_WIFI_DATA_RATE_11_MBPS:
    case NET_IF_WIFI_DATA_RATE_12_MBPS:
    case NET_IF_WIFI_DATA_RATE_18_MBPS:
    case NET_IF_WIFI_DATA_RATE_24_MBPS:
    case NET_IF_WIFI_DATA_RATE_36_MBPS:
    case NET_IF_WIFI_DATA_RATE_48_MBPS:
    case NET_IF_WIFI_DATA_RATE_54_MBPS:
    case NET_IF_WIFI_DATA_RATE_MCS0:
    case NET_IF_WIFI_DATA_RATE_MCS1:
    case NET_IF_WIFI_DATA_RATE_MCS2:
    case NET_IF_WIFI_DATA_RATE_MCS3:
    case NET_IF_WIFI_DATA_RATE_MCS4:
    case NET_IF_WIFI_DATA_RATE_MCS5:
    case NET_IF_WIFI_DATA_RATE_MCS6:
    case NET_IF_WIFI_DATA_RATE_MCS7:
    case NET_IF_WIFI_DATA_RATE_MCS8:
    case NET_IF_WIFI_DATA_RATE_MCS9:
    case NET_IF_WIFI_DATA_RATE_MCS10:
    case NET_IF_WIFI_DATA_RATE_MCS11:
    case NET_IF_WIFI_DATA_RATE_MCS12:
    case NET_IF_WIFI_DATA_RATE_MCS13:
    case NET_IF_WIFI_DATA_RATE_MCS14:
    case NET_IF_WIFI_DATA_RATE_MCS15:
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  switch (security_type) {
    case NET_IF_WIFI_SECURITY_OPEN:
    case NET_IF_WIFI_SECURITY_WEP:
    case NET_IF_WIFI_SECURITY_WPA:
    case NET_IF_WIFI_SECURITY_WPA2:
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  switch (pwr_level) {
    case NET_IF_WIFI_PWR_LEVEL_LO:
    case NET_IF_WIFI_PWR_LEVEL_MED:
    case NET_IF_WIFI_PWR_LEVEL_HI:
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }
#endif

  Net_GlobalLockAcquire((void *)NetIF_WiFi_Join);

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_mgr_api = (NET_WIFI_MGR_API *)p_if->Ext_API;

  Mem_Clr(&ap_cfg, sizeof(ap_cfg));
  ap_cfg.NetType = net_type;
  ap_cfg.DataRate = data_rate;
  ap_cfg.SecurityType = security_type;
  ap_cfg.PwrLevel = pwr_level;
  ap_cfg.SSID = ssid;
  ap_cfg.PSK = psk;
  ap_cfg.Ch = NET_IF_WIFI_CH_ALL;

  p_mgr_api->AP_Join(p_if, &ap_cfg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_if->Link = NET_IF_LINK_UP;

exit_release:
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetIF_WiFi_CreateAP()
 *
 * @brief    Create a wireless access point.
 *
 * @param    if_nbr          Wireless network interface number.
 *
 * @param    net_type        Wireless network type:
 *                               - NET_IF_WIFI_NET_TYPE_INFRASTRUCTURE
 *                               - NET_IF_WIFI_NET_TYPE_ADHOC
 *
 * @param    data_rate       Wireless date rate to configure:
 *                               - NET_IF_WIFI_DATA_RATE_AUTO
 *                               - NET_IF_WIFI_DATA_RATE_1_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_2_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_5_5_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_6_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_9_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_11_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_12_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_18_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_24_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_36_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_48_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_54_MBPS
 *                               - NET_IF_WIFI_DATA_RATE_MCS0
 *                               - NET_IF_WIFI_DATA_RATE_MCS1
 *                               - NET_IF_WIFI_DATA_RATE_MCS2
 *                               - NET_IF_WIFI_DATA_RATE_MCS3
 *                               - NET_IF_WIFI_DATA_RATE_MCS4
 *                               - NET_IF_WIFI_DATA_RATE_MCS5
 *                               - NET_IF_WIFI_DATA_RATE_MCS6
 *                               - NET_IF_WIFI_DATA_RATE_MCS7
 *                               - NET_IF_WIFI_DATA_RATE_MCS8
 *                               - NET_IF_WIFI_DATA_RATE_MCS9
 *                               - NET_IF_WIFI_DATA_RATE_MCS10
 *                               - NET_IF_WIFI_DATA_RATE_MCS11
 *                               - NET_IF_WIFI_DATA_RATE_MCS12
 *                               - NET_IF_WIFI_DATA_RATE_MCS13
 *                               - NET_IF_WIFI_DATA_RATE_MCS14
 *                               - NET_IF_WIFI_DATA_RATE_MCS15
 *
 * @param    security_type   Wireless security type:
 *                               - NET_IF_WIFI_SECURITY_OPEN
 *                               - NET_IF_WIFI_SECURITY_WEP
 *                               - NET_IF_WIFI_SECURITY_WPA
 *                               - NET_IF_WIFI_SECURITY_WPA2
 *
 * @param    pwr_level       Wireless radio power to configure:
 *                               - NET_IF_WIFI_PWR_LEVEL_LO
 *                               - NET_IF_WIFI_PWR_LEVEL_MED
 *                               - NET_IF_WIFI_PWR_LEVEL_HI
 *
 * @param    ch              Channel of the wireless network to create:
 *                               - NET_IF_WIFI_CH_1
 *                               - NET_IF_WIFI_CH_2
 *                               - NET_IF_WIFI_CH_3
 *                               - NET_IF_WIFI_CH_4
 *                               - NET_IF_WIFI_CH_5
 *                               - NET_IF_WIFI_CH_6
 *                               - NET_IF_WIFI_CH_7
 *                               - NET_IF_WIFI_CH_8
 *                               - NET_IF_WIFI_CH_9
 *                               - NET_IF_WIFI_CH_10
 *                               - NET_IF_WIFI_CH_11
 *                               - NET_IF_WIFI_CH_12
 *                               - NET_IF_WIFI_CH_13
 *                               - NET_IF_WIFI_CH_14
 *
 * @param    ssid            SSID of the access point to create.
 *
 * @param    psk             Pre shared key of the access point.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *           - (a) MUST NOT be called with the global network lock already acquired;
 *           - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                   network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
void NetIF_WiFi_CreateAP(NET_IF_NBR                if_nbr,
                         NET_IF_WIFI_NET_TYPE      net_type,
                         NET_IF_WIFI_DATA_RATE     data_rate,
                         NET_IF_WIFI_SECURITY_TYPE security_type,
                         NET_IF_WIFI_PWR_LEVEL     pwr_level,
                         NET_IF_WIFI_CH            ch,
                         NET_IF_WIFI_SSID          ssid,
                         NET_IF_WIFI_PSK           psk,
                         RTOS_ERR                  *p_err)
{
  NET_IF             *p_if;
  NET_WIFI_MGR_API   *p_mgr_api;
  NET_IF_WIFI_AP_CFG ap_cfg;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; )

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)                        // ------------------- VALIDATE ARG -------------------
  switch (data_rate) {
    case NET_IF_WIFI_DATA_RATE_AUTO:
    case NET_IF_WIFI_DATA_RATE_1_MBPS:
    case NET_IF_WIFI_DATA_RATE_2_MBPS:
    case NET_IF_WIFI_DATA_RATE_5_5_MBPS:
    case NET_IF_WIFI_DATA_RATE_6_MBPS:
    case NET_IF_WIFI_DATA_RATE_9_MBPS:
    case NET_IF_WIFI_DATA_RATE_11_MBPS:
    case NET_IF_WIFI_DATA_RATE_12_MBPS:
    case NET_IF_WIFI_DATA_RATE_18_MBPS:
    case NET_IF_WIFI_DATA_RATE_24_MBPS:
    case NET_IF_WIFI_DATA_RATE_36_MBPS:
    case NET_IF_WIFI_DATA_RATE_48_MBPS:
    case NET_IF_WIFI_DATA_RATE_54_MBPS:
    case NET_IF_WIFI_DATA_RATE_MCS0:
    case NET_IF_WIFI_DATA_RATE_MCS1:
    case NET_IF_WIFI_DATA_RATE_MCS2:
    case NET_IF_WIFI_DATA_RATE_MCS3:
    case NET_IF_WIFI_DATA_RATE_MCS4:
    case NET_IF_WIFI_DATA_RATE_MCS5:
    case NET_IF_WIFI_DATA_RATE_MCS6:
    case NET_IF_WIFI_DATA_RATE_MCS7:
    case NET_IF_WIFI_DATA_RATE_MCS8:
    case NET_IF_WIFI_DATA_RATE_MCS9:
    case NET_IF_WIFI_DATA_RATE_MCS10:
    case NET_IF_WIFI_DATA_RATE_MCS11:
    case NET_IF_WIFI_DATA_RATE_MCS12:
    case NET_IF_WIFI_DATA_RATE_MCS13:
    case NET_IF_WIFI_DATA_RATE_MCS14:
    case NET_IF_WIFI_DATA_RATE_MCS15:
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  switch (security_type) {
    case NET_IF_WIFI_SECURITY_OPEN:
    case NET_IF_WIFI_SECURITY_WEP:
    case NET_IF_WIFI_SECURITY_WPA:
    case NET_IF_WIFI_SECURITY_WPA2:
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  switch (pwr_level) {
    case NET_IF_WIFI_PWR_LEVEL_LO:
    case NET_IF_WIFI_PWR_LEVEL_MED:
    case NET_IF_WIFI_PWR_LEVEL_HI:
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  switch (ch) {
    case NET_IF_WIFI_CH_1:
    case NET_IF_WIFI_CH_2:
    case NET_IF_WIFI_CH_3:
    case NET_IF_WIFI_CH_4:
    case NET_IF_WIFI_CH_5:
    case NET_IF_WIFI_CH_6:
    case NET_IF_WIFI_CH_7:
    case NET_IF_WIFI_CH_8:
    case NET_IF_WIFI_CH_9:
    case NET_IF_WIFI_CH_10:
    case NET_IF_WIFI_CH_11:
    case NET_IF_WIFI_CH_12:
    case NET_IF_WIFI_CH_13:
    case NET_IF_WIFI_CH_14:
      break;

    case NET_IF_WIFI_CH_ALL:
    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }
#endif

  Net_GlobalLockAcquire((void *)NetIF_WiFi_CreateAP);

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_mgr_api = (NET_WIFI_MGR_API *)p_if->Ext_API;

  Mem_Clr(&ap_cfg, sizeof(ap_cfg));
  ap_cfg.NetType = net_type;
  ap_cfg.DataRate = data_rate;
  ap_cfg.SecurityType = security_type;
  ap_cfg.PwrLevel = pwr_level;
  ap_cfg.SSID = ssid;
  ap_cfg.PSK = psk;
  ap_cfg.Ch = ch;

  p_mgr_api->AP_Create(p_if, &ap_cfg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_if->Link = NET_IF_LINK_DOWN;
    goto exit_release;
  }

  p_if->Link = NET_IF_LINK_UP;

exit_release:
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetIF_WiFi_Leave()
 *
 * @brief    Leave the access point previously joined.
 *
 * @param    if_nbr  Wireless network interface number.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *           - (a) MUST NOT be called with the global network lock already acquired;
 *           - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                   network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
void NetIF_WiFi_Leave(NET_IF_NBR if_nbr,
                      RTOS_ERR   *p_err)
{
  NET_IF           *p_if;
  NET_WIFI_MGR_API *p_mgr_api;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; )

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Net_GlobalLockAcquire((void *)NetIF_WiFi_Leave);

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_mgr_api = (NET_WIFI_MGR_API *)p_if->Ext_API;

  p_mgr_api->AP_Leave(p_if, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_if->Link = NET_IF_LINK_DOWN;

exit_release:
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetIF_WiFi_GetPeerInfo()
 *
 * @brief    Get the info of peers connected to the access point (When acting as an access point).
 *
 * @param    if_nbr              Wireless network interface number.
 *
 * @param    p_buf_peer          Pointer to the buffer to save the peer information.
 *
 * @param    buf_peer_len_max    Length in bytes of p_buf_peer.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_INVALID_HANDLE
 *
 * @return   Number of peers on the network and that are set in the buffer.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *           - (a) MUST NOT be called with the global network lock already acquired;
 *           - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                   network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
CPU_INT16U NetIF_WiFi_GetPeerInfo(NET_IF_NBR       if_nbr,
                                  NET_IF_WIFI_PEER *p_buf_peer,
                                  CPU_INT16U       buf_peer_len_max,
                                  RTOS_ERR         *p_err)
{
  NET_IF           *p_if = DEF_NULL;
  NET_WIFI_MGR_API *p_mgr_api = DEF_NULL;
  CPU_INT16U       ctn = 0u;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL)
  RTOS_ASSERT_DBG_ERR_SET((p_buf_peer != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, ctn);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Net_GlobalLockAcquire((void *)NetIF_WiFi_GetPeerInfo);

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_mgr_api = (NET_WIFI_MGR_API *)p_if->Ext_API;

  ctn = p_mgr_api->AP_GetPeerInfo(p_if,
                                  p_buf_peer,
                                  buf_peer_len_max,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    ctn = 0u;
    goto exit_release;
  }

exit_release:
  Net_GlobalLockRelease();
  return (ctn);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIF_WiFi_Init()
 *
 * @brief    (1) Initialize Wireless Network Interface Module :
 *               Module initialization NOT yet required/implemented
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetIF_WiFi_Init(RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIF_WiFi_Rx()
 *
 * @brief    Process received data packets or wireless management frame.
 *
 * @param    p_if    Pointer to an network interface that received a packet.
 *
 * @param    p_buf   Pointer to a network buffer that received a packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Network buffer already freed by higher layer; only increment error counter.
 *******************************************************************************************************/
static void NetIF_WiFi_Rx(NET_IF   *p_if,
                          NET_BUF  *p_buf,
                          RTOS_ERR *p_err)
{
  NET_IF_WIFI_FRAME_TYPE *p_frame_type;

  //                                                               --------------- DEMUX WIFI PKT/FRAME ---------------
  p_frame_type = (NET_IF_WIFI_FRAME_TYPE *)p_buf->DataPtr;
  switch (*p_frame_type) {
    case NET_IF_WIFI_DATA_PKT:
      NetIF_WiFi_RxPktHandler(p_if, p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_discard;
      }
      break;

    case NET_IF_WIFI_MGMT_FRAME:
      NetIF_WiFi_RxMgmtFrameHandler(p_if, p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_discard;
      }
      break;

    default:
      goto exit_discard;
  }

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.IFs.WiFi.RxDisCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_WiFi_RxPktHandler()
 *
 * @brief    (1) Process received data packets & forward to network protocol layers :
 *               - (a) Validate & demultiplex packet to higher-layer protocols
 *               - (b) Update receive statistics
 *
 * @param    p_if    Pointer to an network interface that received a packet.
 *
 * @param    p_buf   Pointer to a network buffer that received a packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Network buffer already freed by higher layer.
 *******************************************************************************************************/
static void NetIF_WiFi_RxPktHandler(NET_IF   *p_if,
                                    NET_BUF  *p_buf,
                                    RTOS_ERR *p_err)
{
  NET_CTR_IF_802x_STATS *p_ctrs_stat;
  NET_CTR_IF_802x_ERRS  *p_ctrs_err;

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  p_ctrs_stat = &Net_StatCtrs.IFs.WiFi.IF_802xCtrs;
#else
  p_ctrs_stat = DEF_NULL;
#endif
#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  p_ctrs_err = &Net_ErrCtrs.IFs.WiFi.IF_802xCtrs;
#else
  p_ctrs_err = DEF_NULL;
#endif

  //                                                               ------------------- RX WIFI PKT --------------------
  NetIF_802x_Rx(p_if,
                p_buf,
                p_ctrs_stat,
                p_ctrs_err,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.IFs.WiFi.RxPktCtr);

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.IFs.WiFi.RxPktDisCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIF_WiFi_RxMgmtFrameHandler()
 *
 * @brief    Demultiplex management wireless frame.
 *
 * @param    p_if    Pointer to an network interface that received a packet.
 *
 * @param    p_buf   Pointer to a network buffer that received a packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) If a network interface receives a packet, its physical link must be 'UP' & the
 *               interface's physical link state is set accordingly.
 *               - (a) An attempt to check for link state is made after an interface has been started.
 *                     However, many physical layer devices, such as Ethernet physical layers require
 *                     several seconds for Auto-Negotiation to complete before the link becomes
 *                     established.  Thus the interface link flag is not updated until the link state
 *                     timer expires & one or more attempts to check for link state have been completed.
 *
 * @note     (3) Network buffer already freed by higher layer; only increment error counter.
 *******************************************************************************************************/
static void NetIF_WiFi_RxMgmtFrameHandler(NET_IF   *p_if,
                                          NET_BUF  *p_buf,
                                          RTOS_ERR *p_err)
{
  NET_DEV_API_IF_WIFI *p_dev_api;

  if (p_if->Init != DEF_YES) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.WiFi.RxMgmtDisCtr);
    goto exit;
  }

  p_dev_api = (NET_DEV_API_IF_WIFI *)p_if->Dev_API;

  NET_CTR_STAT_INC(Net_StatCtrs.IFs.WiFi.RxMgmtCtr);
  //                                                               -------------- DEMUX WIFI MGMT FRAME ---------------
  p_dev_api->MgmtDemux(p_if, p_buf, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.WiFi.RxMgmtDisCtr);
    goto exit;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.IFs.WiFi.RxMgmtCompCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIF_WiFi_Tx()
 *
 * @brief    Prepare data packets from network protocol layers for Wireless transmit.
 *
 * @param    p_if    Pointer to a network interface to transmit data packet(s).
 *
 * @param    p_buf   Pointer to network buffer with data packet to transmit.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIF_WiFi_Tx(NET_IF   *p_if,
                          NET_BUF  *p_buf,
                          RTOS_ERR *p_err)
{
  NET_CTR_IF_802x_STATS *p_ctrs_stat;
  NET_CTR_IF_802x_ERRS  *p_ctrs_err;

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  p_ctrs_stat = &Net_StatCtrs.IFs.WiFi.IF_802xCtrs;
#else
  p_ctrs_stat = DEF_NULL;
#endif

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  p_ctrs_err = &Net_ErrCtrs.IFs.WiFi.IF_802xCtrs;
#else
  p_ctrs_err = DEF_NULL;
#endif

  //                                                               --------------- PREPARE WIFI TX PKT ----------------
  NetIF_802x_Tx(p_if,
                p_buf,
                p_ctrs_stat,
                p_ctrs_err,
                p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      NET_CTR_STAT_INC(Net_StatCtrs.IFs.WiFi.TxPktCtr);
      break;

    case RTOS_ERR_NET_ADDR_UNRESOLVED:
      goto exit;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.WiFi.TxPktDisCtr);
      goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_WiFi_IF_Add()
 *
 * @brief    (1) Add & initialize an Wireless network interface :
 *               - (a) Validate   Wireless device configuration
 *               - (b) Initialize Wireless device data area
 *               - (c) Perform    Wireless/OS initialization
 *               - (d) Initialize Wireless device hardware MAC address
 *               - (e) Initialize Wireless device hardware
 *               - (f) Initialize Wireless device MTU
 *               - (g) Configure  Wireless interface
 *
 * @param    p_if    Pointer to Wireless network interface to add.
 *
 *
 * Argument(s) : p_if        Pointer to Wireless network interface to add.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) This function sets the interface MAC address to all 0's.  This ensures that the
 *                   device driver can compare the MAC for all 0 in order to check if the MAC has
 *                   been configured before.
 *
 *               (3) The return error is not checked because there isn't anything that can be done from
 *                   software in order to recover from a device hardware initializtion error.  The cause
 *                   is most likely associated with either a driver or hardware failure.  The best
 *                   course of action it to increment the interface number & allow software to attempt
 *                   to bring up the next interface.
 *
 *               (4) Upon adding an Wireless interface, the highest possible Wireless MTU is configured.
 *                   If this value needs to be changed, either prior to starting the interface, or during
 *                   run-time, it may be reconfigured by calling NetIF_MTU_Set() from the application.
 *******************************************************************************************************/
static void NetIF_WiFi_IF_Add(NET_IF   *p_if,
                              RTOS_ERR *p_err)
{
  NET_DEV_CFG_WIFI    *p_dev_cfg;
  NET_DEV_API_IF_WIFI *p_dev_api;
  NET_IF_DATA_WIFI    *p_if_data;
  NET_WIFI_MGR_API    *p_mgr_api;
  void                *p_addr_hw;
  NET_BUF_SIZE        buf_size_max;
  NET_MTU             mtu_max;

  p_dev_cfg = (NET_DEV_CFG_WIFI *)p_if->Dev_Cfg;
  p_mgr_api = (NET_WIFI_MGR_API *)p_if->Ext_API;

  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api->Init != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api->AP_Create != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api->AP_GetPeerInfo != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api->AP_Join != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api->AP_Leave != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api->AP_Scan != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api->IO_Ctrl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api->Mgmt != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api->Signal != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api->Start != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mgr_api->Stop != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               ------------------- CFG WIFI IF --------------------
  p_if->Type = NET_IF_TYPE_WIFI;                                // Set IF type to WiFi.

  NetIF_BufPoolInit(p_if, p_err);                               // Init IF's buf pools.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------- INIT WIFI DEV DATA AREA --------------
  p_if->IF_Data = Mem_SegAlloc("IF data",
                               DEF_NULL,
                               sizeof(NET_IF_DATA_WIFI),
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_if_data = (NET_IF_DATA_WIFI *)p_if->IF_Data;

  //                                                               --------------- INIT IF HW/MAC ADDR ----------------
  p_addr_hw = &p_if_data->HW_Addr[0];
  Mem_Clr(p_addr_hw, NET_IF_802x_ADDR_SIZE);                    // Clr hw addr (see Note #2).

  //                                                               ------------------ INIT WIFI MGR -------------------
  p_mgr_api->Init(p_if, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------------- INIT DEV HW --------------------
  p_dev_api = (NET_DEV_API_IF_WIFI *)p_if->Dev_API;

  p_dev_api->Init(p_if, p_err);                                 // Init but don't start dev HW.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // See Note #3.
    goto exit;
  }

  //                                                               --------------------- INIT MTU ---------------------
  buf_size_max = DEF_MAX(p_dev_cfg->TxBufLargeSize,
                         p_dev_cfg->TxBufSmallSize);
  mtu_max = DEF_MIN(NET_IF_MTU_ETHER, buf_size_max);
  p_if->MTU = mtu_max;                                          // Set WiFi MTU (see Note #4).

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_WiFi_IF_Start()
 *
 * @brief    (1) Start an Wireless Network Interface :
 *               - (a) Start WiFi manager
 *               - (b) Start WiFi device
 *
 * @param    p_if    Pointer to Wireless network interface to start.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) If present, an attempt will be made to initialize the Ethernet Phy (Physical Layer).
 *                   This function assumes that the device driver has initialized the Phy (R)MII bus prior
 *                   to the Phy initialization & link state get calls.
 *
 * @note     (3) The MII register block remains enabled while the Phy PWRDOWN bit is set.  Thus, all
 *                   parameters may be configured PRIOR to enabling the analog portions of the Phy logic.
 *
 * @note     (4) If the Phy enable or link state get functions return an error, they may be ignored
 *                   since the Phy may be enabled by default after reset, & the link may become established
 *                   at a later time.
 *******************************************************************************************************/
static void NetIF_WiFi_IF_Start(NET_IF   *p_if,
                                RTOS_ERR *p_err)
{
  NET_DEV_API_IF_WIFI *p_dev_api;
  NET_WIFI_MGR_API    *p_mgr_api;

  p_dev_api = (NET_DEV_API_IF_WIFI *)p_if->Dev_API;
  p_mgr_api = (NET_WIFI_MGR_API *)p_if->Ext_API;

  p_mgr_api->Start(p_if, p_err);                                // Start wifi mgr.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_dev_api->Start(p_if, p_err);                                // Start dev.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_if->Link = NET_IF_LINK_DOWN;

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_WiFi_IF_Stop()
 *
 * @brief    (1) Stop Specific Network Interface :
 *               - (a) Stop Wireless device
 *               - (b) Stop Wireless physical layer, if available
 *
 * @param    p_if    Pointer to Wireless network interface to stop.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) If the Phy returns an error, it may be ignored since the device has been successfully
 *                   stopped.  One side effect may be that the Phy remains powered on & possibly linked.
 *******************************************************************************************************/
static void NetIF_WiFi_IF_Stop(NET_IF   *p_if,
                               RTOS_ERR *p_err)
{
  NET_DEV_API_IF_WIFI *p_dev_api;
  NET_WIFI_MGR_API    *p_mgr_api;

  p_dev_api = (NET_DEV_API_IF_WIFI *)p_if->Dev_API;
  p_mgr_api = (NET_WIFI_MGR_API *)p_if->Ext_API;

  p_dev_api->Stop(p_if, p_err);                                 // Stop dev.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_mgr_api->Stop(p_if, p_err);                                 // Stop wifi mgr.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_if->Link = NET_IF_LINK_DOWN;

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIF_WiFi_IO_CtrlHandler()
 *
 * @brief    Handle an Wireless interface's (I/O) control(s).
 *
 * @param    p_if    Pointer to an Wireless network interface.
 *
 * @param    opt     Desired I/O control option code to perform; additional control options may be
 *                   defined by the device driver :
 *                   NET_IF_IO_CTRL_LINK_STATE_GET           Get    Wireless interface's link state,
 *                                                           'UP' or 'DOWN'.
 *                   NET_IF_IO_CTRL_LINK_STATE_GET_INFO      Get    Wireless interface's detailed
 *                                                           link state info.
 *                   NET_IF_IO_CTRL_LINK_STATE_UPDATE        Update Wireless interface's link state.
 *
 * @param    p_data  Pointer to variable that will receive possible I/O control data (see Note #1).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) 'p_data' MUST point to a variable (or memory) that is sufficiently sized AND aligned
 *                   to receive any return data.
 *******************************************************************************************************/
static void NetIF_WiFi_IO_CtrlHandler(NET_IF     *p_if,
                                      CPU_INT08U opt,
                                      void       *p_data,
                                      RTOS_ERR   *p_err)
{
  NET_IF_LINK_STATE *p_link_state;
  NET_DEV_LINK_WIFI link_info;
  NET_WIFI_MGR_API  *p_mgr_api;

  p_mgr_api = (NET_WIFI_MGR_API *)p_if->Ext_API;

  //                                                               ------------ VALIDATE EXT API I/O PTRS -------------
  RTOS_ASSERT_DBG_ERR_SET((p_data != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               ----------- HANDLE NET DEV I/O CTRL OPT ------------
  switch (opt) {
    case NET_IF_IO_CTRL_LINK_STATE_GET:
      p_mgr_api->IO_Ctrl(p_if,
                         NET_IF_IO_CTRL_LINK_STATE_GET,
                         &link_info.LinkState,                  // Get link state info.
                         p_err);

      p_link_state = (NET_IF_LINK_STATE *)p_data;               // See Note #1.
      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_NONE:
          *p_link_state = link_info.LinkState;
          break;

        case RTOS_ERR_WOULD_BLOCK:
        case RTOS_ERR_TIMEOUT:
          *p_link_state = p_if->LinkPrev;
          break;

        default:
          *p_link_state = NET_IF_LINK_DOWN;
          goto exit;
      }
      break;

    case NET_IF_IO_CTRL_LINK_STATE_UPDATE:
      //                                                           Rtn err for unavail ctrl opt?
      break;

    case NET_IF_IO_CTRL_LINK_STATE_GET_INFO:
    default:                                                    // Handle other dev I/O opt(s).
      p_mgr_api->IO_Ctrl(p_if,
                         opt,
                         p_data,                                // See Note #1.
                         p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
  }

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IF_WIFI_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
