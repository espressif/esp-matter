/***************************************************************************//**
 * @file
 * @brief Network Interface Layer - Ethernet
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

#ifdef  NET_IF_ETHER_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "net_if_ether_priv.h"
#include  "net_if_priv.h"
#include  "net_type_priv.h"

#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/platform_mgr/platform_mgr_priv.h>

#include  <net/include/net_if_ether.h>
#include  <net/include/dhcp_client.h>
#include  <net/include/net_ascii.h>

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
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------------------- RX FNCT ----------------------
static void NetIF_Ether_Rx(NET_IF   *p_if,
                           NET_BUF  *p_buf,
                           RTOS_ERR *p_err);

//                                                                 --------------------- TX FNCT ----------------------
static void NetIF_Ether_Tx(NET_IF   *p_if,
                           NET_BUF  *p_buf,
                           RTOS_ERR *p_err);

//                                                                 -------------------- API FNCTS ---------------------
static void NetIF_Ether_IF_Add(NET_IF   *p_if,
                               RTOS_ERR *p_err);

static void NetIF_Ether_IF_Start(NET_IF   *p_if,
                                 RTOS_ERR *p_err);

static void NetIF_Ether_IF_Stop(NET_IF   *p_if,
                                RTOS_ERR *p_err);

//                                                                 -------------------- MGMT FNCTS --------------------
static void NetIF_Ether_IO_CtrlHandler(NET_IF     *p_if,
                                       CPU_INT08U opt,
                                       void       *p_data,
                                       RTOS_ERR   *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           INTERFACE CFG DEFAULT
 *******************************************************************************************************/

const NET_IF_ETHER_CFG NetIF_Ether_CfgDflt = {
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
  .IPv6.Hook = DEF_NULL
};

/********************************************************************************************************
 *                                               INTERFACE API
 *******************************************************************************************************/

const NET_IF_API NetIF_API_Ether = {                                            // Ether IF API fnct ptrs :
  &NetIF_Ether_IF_Add,                                                          // Init/add
  &NetIF_Ether_IF_Start,                                                        // Start
  &NetIF_Ether_IF_Stop,                                                         // Stop
  &NetIF_Ether_Rx,                                                              // Rx
  &NetIF_Ether_Tx,                                                              // Tx
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
  &NetIF_Ether_IO_CtrlHandler                                                   // I/O ctrl
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIF_Ether_Add()
 *
 * @brief    Add & initialize a specific instance of a network Ethernet interface.
 *
 * @param    p_name      String identifier for the Ethernet interface to add.
 *
 * @param    p_buf_cfg   Pointer to buffer configuration (See Note #1).
 *
 * @param    p_mem_seg   Memory segment from which internal data will be allocated.
 *                       If DEF_NULL, it will be allocated from the global heap.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
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
 *
 * @note     (1) When a non null buffer configuration is passed, a copy of the device configuration
 *               that come from the BSP is completed. It is recommended to reduce the memory usage to
 *               modify the static configuration specified in the BSP.
 *******************************************************************************************************/
NET_IF_NBR NetIF_Ether_Add(CPU_CHAR       *p_name,
                           NET_IF_BUF_CFG *p_buf_cfg,
                           MEM_SEG        *p_mem_seg,
                           RTOS_ERR       *p_err)
{
  struct  _rtos_net_if_ether_hw_info *p_hw_info = DEF_NULL;
  NET_DEV_CFG_ETHER                  *p_dev_cfg = DEF_NULL;
  NET_DEV_CFG_ETHER                  *p_dev_cfg_valid = DEF_NULL;
  NET_IF                             *p_if = DEF_NULL;
  NET_IF_NBR                         if_nbr = NET_IF_NBR_NONE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_IF_NBR_NONE);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------ GET ITEM FROM PLATFORM MGR ------------
  p_hw_info = (struct _rtos_net_if_ether_hw_info *)PlatformMgrItemGetByName(p_name, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (NET_IF_NBR_NONE);
  }

  //                                                               ------------- SET DEVICE CONFIGURATION -------------
  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_hw_info->InfoPtr->DevCfgPtr;
  RTOS_ASSERT_DBG_ERR_SET((p_dev_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IF_NBR_NONE);

  if (p_buf_cfg != DEF_NULL) {
    RTOS_ASSERT_CRITICAL(p_buf_cfg->RxBufLargeNbr > 2, RTOS_ERR_INVALID_CFG, NET_IF_NBR_NONE);
    RTOS_ASSERT_CRITICAL(p_buf_cfg->TxBufLargeNbr > 1, RTOS_ERR_INVALID_CFG, NET_IF_NBR_NONE);

    p_dev_cfg_valid = Mem_SegAlloc("Ethernet cfg", p_mem_seg, sizeof(NET_DEV_CFG_ETHER), p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_CODE_GET(*p_err), NET_IF_NBR_NONE);

    Mem_Copy(p_dev_cfg_valid, p_dev_cfg, sizeof(NET_DEV_CFG_ETHER));

    p_dev_cfg_valid->RxBufLargeNbr = p_buf_cfg->RxBufLargeNbr;
    p_dev_cfg_valid->TxBufLargeNbr = p_buf_cfg->TxBufLargeNbr;
    p_dev_cfg_valid->TxBufSmallNbr = p_buf_cfg->TxBufSmallNbr;

    if (p_buf_cfg->RxBufLargeNbr > 5) {
      p_dev_cfg_valid->RxDescNbr = (p_buf_cfg->RxBufLargeNbr / 2) - 1;
    } else {
      p_dev_cfg_valid->RxDescNbr = 2;
    }

    p_dev_cfg_valid->TxDescNbr = p_buf_cfg->TxBufLargeNbr + p_buf_cfg->TxBufSmallNbr;
  } else {
    p_dev_cfg_valid = p_dev_cfg;
  }

  //                                                               -------------- ADD ETHERNET INTERFACE --------------
  if_nbr = NetIF_Add((void *)&NetIF_API_Ether,
                     (void *) p_hw_info->InfoPtr->DrvAPI_Ptr,
                     (void *) p_hw_info->InfoPtr->BSP_API_Ptr,
                     (void *) p_dev_cfg_valid,
                     (void *) p_hw_info->InfoPtr->PHY_API_Ptr,
                     (void *) p_hw_info->InfoPtr->PHY_CfgPtr,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (NET_IF_NBR_NONE);
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
 *                                           NetIF_Ether_Start()
 *
 * @brief    (1) Start an Ethernet type interface :
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
void NetIF_Ether_Start(NET_IF_NBR       if_nbr,
                       NET_IF_ETHER_CFG *p_cfg,
                       RTOS_ERR         *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  NetIF_StartInternal(if_nbr, (NET_IF_CFG *)p_cfg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetIF_Ether_Init()
 *
 * @brief    (1) Initialize Ethernet Network Interface Module :
 *               Module initialization NOT yet required/implemented
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetIF_Ether_Init(RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);
}

/****************************************************************************************************//**
 *                                               NetIF_Ether_Reg()
 *
 * @brief    Register an Ethernet Interface at the Platform Manager.
 *
 * @param    p_name          String identifier for the Ethernet interface to register.
 *
 * @param    p_ctrlr_info    Pointer to the Ethernet controller information structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *******************************************************************************************************/
void NetIF_Ether_Reg(CPU_CHAR             *p_name,
                     NET_IF_ETHER_HW_INFO *p_ctrlr_info,
                     RTOS_ERR             *p_err)
{
  struct  _rtos_net_if_ether_hw_info *p_net_mgr_item;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               -------------- VALIDATE HARDWARE INFO --------------
  RTOS_ASSERT_DBG_ERR_SET((p_ctrlr_info->DevCfgPtr != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_DBG_ERR_SET((p_ctrlr_info->DrvAPI_Ptr != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG,; );

  //                                                               ------------ CREATE A PLATFORM MGR ITEM ------------
  p_net_mgr_item = (struct _rtos_net_if_ether_hw_info *)PlatformMgrItemAlloc(sizeof(struct _rtos_net_if_ether_hw_info), p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ----------------- INITIALIZE ITEM ------------------
  p_net_mgr_item->Item.StrID = p_name;
  p_net_mgr_item->Item.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_NET_ETHER;

  p_net_mgr_item->InfoPtr = p_ctrlr_info;

  //                                                               ------------- ADD ITEM TO PLATFORM MGR -------------
  PlatformMgrItemAdd(&p_net_mgr_item->Item, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIF_Ether_Rx()
 *
 * @brief    Process received data packets & forward to network protocol layers.
 *
 * @param    p_if    Pointer to an Ethernet network interface to transmit data packet(s).
 *
 * @param    p_buf   Pointer to a network buffer that received a packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Network buffer already freed by higher layer; only increment error counter.
 *******************************************************************************************************/
static void NetIF_Ether_Rx(NET_IF   *p_if,
                           NET_BUF  *p_buf,
                           RTOS_ERR *p_err)
{
  NET_CTR_IF_802x_STATS *p_ctrs_stat;
  NET_CTR_IF_802x_ERRS  *p_ctrs_err;

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  p_ctrs_stat = &Net_StatCtrs.IFs.Ether.IF_802xCtrs;
#else
  p_ctrs_stat = DEF_NULL;
#endif
#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  p_ctrs_err = &Net_ErrCtrs.IFs.Ether.IF_802xCtrs;
#else
  p_ctrs_err = DEF_NULL;
#endif

  //                                                               ------------------- RX ETHER PKT -------------------
  NetIF_802x_Rx(p_if,
                p_buf,
                p_ctrs_stat,
                p_ctrs_err,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.Ether.RxPktDisCtr);
    goto exit;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.IFs.Ether.RxPktCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIF_Ether_Tx()
 *
 * @brief    Prepare data packets from network protocol layers for Ethernet transmit.
 *
 * @param    p_if    Pointer to a network interface to transmit data packet(s).
 *
 * @param    p_buf   Pointer to network buffer with data packet to transmit.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Network buffer already freed by higher layer; only increment error counter.
 *******************************************************************************************************/
static void NetIF_Ether_Tx(NET_IF   *p_if,
                           NET_BUF  *p_buf,
                           RTOS_ERR *p_err)
{
  NET_CTR_IF_802x_STATS *p_ctrs_stat;
  NET_CTR_IF_802x_ERRS  *p_ctrs_err;

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  p_ctrs_stat = &Net_StatCtrs.IFs.Ether.IF_802xCtrs;
#else
  p_ctrs_stat = DEF_NULL;
#endif
#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  p_ctrs_err = &Net_ErrCtrs.IFs.Ether.IF_802xCtrs;
#else
  p_ctrs_err = DEF_NULL;
#endif

  //                                                               --------------- PREPARE ETHER TX PKT ---------------
  NetIF_802x_Tx(p_if,
                p_buf,
                p_ctrs_stat,
                p_ctrs_err,
                p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      NET_CTR_STAT_INC(Net_StatCtrs.IFs.Ether.TxPktCtr);
      break;

    case RTOS_ERR_NET_OP_IN_PROGRESS:
    case RTOS_ERR_NET_ADDR_UNRESOLVED:
      goto exit;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.Ether.TxPktDisCtr);
      goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_Ether_IF_Add()
 *
 * @brief    (1) Add & initialize an Ethernet network interface :
 *               - (a) Validate   Ethernet device configuration
 *               - (b) Initialize Ethernet device data area
 *               - (c) Perform    Ethernet/OS initialization
 *               - (d) Initialize Ethernet device hardware MAC address
 *               - (e) Initialize Ethernet device hardware
 *               - (f) Initialize Ethernet device MTU
 *               - (g) Configure  Ethernet interface
 *
 * @param    p_if    Pointer to Ethernet network interface to add.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) This function sets the interface MAC address to all 0's.  This ensures that the
 *               device driver can compare the MAC for all 0 in order to check if the MAC has
 *               been configured before.
 *
 * @note     (3) The return error is not checked because there isn't anything that can be done from
 *               software in order to recover from a device hardware initialization error.  The cause
 *               is most likely associated with either a driver or hardware failure.  The best
 *               course of action it to increment the interface number & allow software to attempt
 *               to bring up the next interface.
 *
 * @note     (4) Upon adding an Ethernet interface, the highest possible Ethernet MTU is configured.
 *               If this value needs to be changed, either prior to starting the interface, or during
 *               run-time, it may be reconfigured by calling NetIF_MTU_Set() from the application.
 *******************************************************************************************************/
static void NetIF_Ether_IF_Add(NET_IF   *p_if,
                               RTOS_ERR *p_err)
{
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN       flags_invalid;
  NET_PHY_API_ETHER *p_phy_api;
  NET_PHY_CFG_ETHER *p_phy_cfg;
  CPU_BOOLEAN       phy_api_none;
  CPU_BOOLEAN       phy_api_avail;
#endif
  NET_DEV_CFG_ETHER *p_dev_cfg;
  NET_DEV_API_ETHER *p_dev_api;
  NET_IF_DATA_ETHER *p_if_data;
  void              *p_addr_hw;
  NET_BUF_SIZE      buf_size_max;
  NET_MTU           mtu_max;

  p_dev_cfg = (NET_DEV_CFG_ETHER *)p_if->Dev_Cfg;
  p_dev_api = (NET_DEV_API_ETHER *)p_if->Dev_API;

  RTOS_ASSERT_DBG_ERR_SET((p_dev_api != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_dev_api->AddrMulticastAdd != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_dev_api->AddrMulticastRemove != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_dev_api->Init != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_dev_api->IO_Ctrl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_dev_api->Rx != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_dev_api->Start != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_dev_api->Stop != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_dev_api->Tx != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ------------------- INIT DEV HW --------------------

  p_phy_api = (NET_PHY_API_ETHER *)p_if->Ext_API;
  p_phy_cfg = (NET_PHY_CFG_ETHER *)p_if->Ext_Cfg;

  phy_api_none = ((p_phy_api == DEF_NULL)
                  && (p_phy_cfg == DEF_NULL)) ? DEF_YES : DEF_NO;
  phy_api_avail = ((p_phy_api != DEF_NULL)
                   && (p_phy_cfg != DEF_NULL)) ? DEF_YES : DEF_NO;

  RTOS_ASSERT_DBG_ERR_SET(((phy_api_none == DEF_YES)
                           || (phy_api_avail == DEF_YES)), *p_err, RTOS_ERR_INVALID_CFG,; );

  //                                                               ----------------- VALIDATE DEV CFG -----------------
  flags_invalid = DEF_BIT_IS_SET_ANY(p_dev_cfg->Flags, ~NET_DEV_CFG_FLAG_MASK);
  RTOS_ASSERT_DBG_ERR_SET((flags_invalid == DEF_NO), *p_err, RTOS_ERR_INVALID_CFG,; );

  //                                                               ----------------- VALIDATE PHY CFG -----------------
  if (p_if->Ext_API != DEF_NULL) {
    switch (p_phy_cfg->Spd) {                                   // Validate phy bus spd.
      case NET_PHY_SPD_10:
      case NET_PHY_SPD_100:
      case NET_PHY_SPD_1000:
      case NET_PHY_SPD_AUTO:
        break;

      default:
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
    }

    switch (p_phy_cfg->Duplex) {                                // Validate phy bus duplex.
      case NET_PHY_DUPLEX_HALF:
      case NET_PHY_DUPLEX_FULL:
      case NET_PHY_DUPLEX_AUTO:
        break;

      default:
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
    }
  }
#endif

  //                                                               ------------------- CFG ETHER IF -------------------
  p_if->Type = NET_IF_TYPE_ETHER;                               // Set IF type to Ether.

  NetIF_BufPoolInit(p_if, p_err);                               // Init IF's buf pools.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------- INIT ETHER DEV DATA AREA -------------
  p_if->IF_Data = Mem_SegAlloc("IF Data",
                               DEF_NULL,
                               sizeof(NET_IF_DATA_ETHER),
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_if_data = (NET_IF_DATA_ETHER *)p_if->IF_Data;

  //                                                               --------------- INIT IF HW/MAC ADDR ----------------
  p_addr_hw = &p_if_data->HW_Addr[0];
  Mem_Clr(p_addr_hw, NET_IF_802x_ADDR_SIZE);                    // Clr hw addr (see Note #2).

  p_dev_api->Init(p_if, p_err);                                 // Init but don't start dev HW.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // See Note #3.
    goto exit;
  }
  //                                                               --------------------- INIT MTU ---------------------
  buf_size_max = DEF_MAX(p_dev_cfg->TxBufLargeSize,
                         p_dev_cfg->TxBufSmallSize);
  mtu_max = DEF_MIN(NET_IF_MTU_ETHER, buf_size_max);
  p_if->MTU = mtu_max;                                          // Set Ether MTU (see Note #4).

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_Ether_IF_Start()
 *
 * @brief    (1) Start an Ethernet Network Interface :
 *               - (a) Start Ethernet device
 *               - (b) Start Ethernet physical layer, if available :
 *                   - (1) Initialize physical layer
 *                   - (2) Enable     physical layer
 *                   - (3) Check      physical layer link status
 *
 * @param    p_if    Pointer to Ethernet network interface to start.
 *
 *
 * Argument(s) : p_if        Pointer to Ethernet network interface to start.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) If present, an attempt will be made to initialize the Ethernet Phy (Physical Layer).
 *                   This function assumes that the device driver has initialized the Phy (R)MII bus prior
 *                   to the Phy initialization & link state get calls.
 *
 *               (3) The MII register block remains enabled while the Phy PWRDOWN bit is set.  Thus all
 *                   parameters may be configured PRIOR to enabling the analog portions of the Phy logic.
 *
 *               (4) If the Phy enable or link state get functions return an error, they may be ignored
 *                   since the Phy may be enabled by default after reset, & the link may become established
 *                   at a later time.
 *******************************************************************************************************/
static void NetIF_Ether_IF_Start(NET_IF   *p_if,
                                 RTOS_ERR *p_err)
{
  NET_DEV_LINK_ETHER link_state;
  NET_DEV_API_ETHER  *p_dev_api;
  NET_PHY_API_ETHER  *p_phy_api;
  NET_PHY_CFG_ETHER  *p_phy_cfg;
  RTOS_ERR           local_err;

  p_dev_api = (NET_DEV_API_ETHER *)p_if->Dev_API;
  p_phy_api = (NET_PHY_API_ETHER *)p_if->Ext_API;
  p_phy_cfg = (NET_PHY_CFG_ETHER *)p_if->Ext_Cfg;

  p_dev_api->Start(p_if, p_err);                                // Start dev.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (p_phy_api != DEF_NULL) {                                  // If avail, ...
    RTOS_ASSERT_DBG_ERR_SET((p_phy_api->Init != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
    RTOS_ASSERT_DBG_ERR_SET((p_phy_api->EnDis != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
    RTOS_ASSERT_DBG_ERR_SET((p_phy_api->LinkStateGet != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
    RTOS_ASSERT_DBG_ERR_SET((p_phy_api->LinkStateSet != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
    RTOS_ASSERT_DBG_ERR_SET((p_phy_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

    p_phy_api->Init(p_if, p_err);                               // ... init Phy (see Note #2).
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
      case RTOS_ERR_NET_PHY_TIMEOUT_AUTO_NEG:                   // Initial auto-negotiation err is passive when ...
        break;                                                  // ... Phy is dis'd from reset / init.

      default:
        goto exit;
    }

    //                                                             Cfg link state (see Note #3).
    link_state.Spd = p_phy_cfg->Spd;
    link_state.Duplex = p_phy_cfg->Duplex;
    p_phy_api->LinkStateSet(p_if, &link_state, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    p_phy_api->EnDis(p_if, DEF_ENABLED, &local_err);            // En Phy.
    PP_UNUSED_PARAM(local_err);                                 // See Note #4.

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    p_phy_api->LinkStateGet(p_if, &link_state, &local_err);     // See Note #4.
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      p_if->Link = NET_IF_LINK_DOWN;
    } else {
      if (link_state.Spd > NET_PHY_SPD_0) {
        p_if->Link = NET_IF_LINK_UP;
      } else {
        p_if->Link = NET_IF_LINK_DOWN;
      }
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_Ether_IF_Stop()
 *
 * @brief    (1) Stop Specific Network Interface :
 *               - (a) Stop Ethernet device
 *               - (b) Stop Ethernet physical layer, if available
 *
 * @param    p_if    Pointer to Ethernet network interface to stop.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) If the Phy returns an error, it may be ignored since the device has been successfully
 *               stopped.  One side effect may be that the Phy remains powered on & possibly linked.
 *******************************************************************************************************/
static void NetIF_Ether_IF_Stop(NET_IF   *p_if,
                                RTOS_ERR *p_err)
{
  NET_DEV_API_ETHER *p_dev_api;
  NET_PHY_API_ETHER *p_phy_api;

  p_dev_api = (NET_DEV_API_ETHER *)p_if->Dev_API;
  p_phy_api = (NET_PHY_API_ETHER *)p_if->Ext_API;

  p_dev_api->Stop(p_if, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (p_phy_api != DEF_NULL) {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    p_phy_api->EnDis(p_if, DEF_DISABLED, &local_err);           // Disable Phy.
    PP_UNUSED_PARAM(local_err);                                 // See Note #2.
  }

  p_if->Link = NET_IF_LINK_DOWN;

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIF_Ether_IO_CtrlHandler()
 *
 * @brief    Handle an Ethernet interface's (I/O) control(s).
 *
 * @param    p_if    Pointer to an Ethernet network interface.
 *
 * @param    opt     Desired I/O control option code to perform; additional control options may be
 *                   defined by the device driver :
 *                   NET_IF_IO_CTRL_LINK_STATE_GET           Get    Ethernet interface's link state,
 *                                                           'UP' or 'DOWN'.
 *                   NET_IF_IO_CTRL_LINK_STATE_GET_INFO      Get    Ethernet interface's detailed
 *                                                           link state info.
 *                   NET_IF_IO_CTRL_LINK_STATE_UPDATE        Update Ethernet interface's link state.
 *
 * @param    p_data  Pointer to variable that will receive possible I/O control data (see Note #1).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) 'p_data' MUST point to a variable (or memory) that is sufficiently sized AND aligned
 *               to receive any return data.
 *******************************************************************************************************/
static void NetIF_Ether_IO_CtrlHandler(NET_IF     *p_if,
                                       CPU_INT08U opt,
                                       void       *p_data,
                                       RTOS_ERR   *p_err)
{
  NET_DEV_API_ETHER  *p_dev_api;
  NET_DEV_LINK_ETHER p_link_info;
  NET_IF_LINK_STATE  *p_link_state;

  RTOS_ASSERT_DBG_ERR_SET((p_data != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_dev_api = (NET_DEV_API_ETHER *)p_if->Dev_API;

  //                                                               ----------- HANDLE NET DEV I/O CTRL OPT ------------
  switch (opt) {
    case NET_IF_IO_CTRL_LINK_STATE_GET:
      p_dev_api->IO_Ctrl(p_if,
                         NET_IF_IO_CTRL_LINK_STATE_GET_INFO,
                         &p_link_info,                          // Get link state info.
                         p_err);

      p_link_state = (NET_IF_LINK_STATE *)p_data;               // See Note #1.

      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        *p_link_state = NET_IF_LINK_DOWN;
        goto exit;
      }

      switch (p_link_info.Spd) {                                // Demux link state from link spd.
        case NET_PHY_SPD_10:
        case NET_PHY_SPD_100:
        case NET_PHY_SPD_1000:
          *p_link_state = NET_IF_LINK_UP;
          break;

        case NET_PHY_SPD_0:
        default:
          *p_link_state = NET_IF_LINK_DOWN;
          break;
      }
      break;

    case NET_IF_IO_CTRL_LINK_STATE_UPDATE:
      break;                                                    // TODO_NET: Rtn err for unavail ctrl opt?

    case NET_IF_IO_CTRL_LINK_STATE_GET_INFO:
    default:                                                    // Handle other dev I/O opt(s).
      p_dev_api->IO_Ctrl(p_if,
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

#endif // NET_IF_ETHER_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
