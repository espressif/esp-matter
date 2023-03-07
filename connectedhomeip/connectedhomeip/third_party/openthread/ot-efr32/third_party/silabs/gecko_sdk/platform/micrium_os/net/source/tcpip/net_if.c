/***************************************************************************//**
 * @file
 * @brief Network Interface Management
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

#include  <em_core.h>

#include  <net/include/net_if.h>
#include  <net/include/net_util.h>
#include  <net/include/net_cfg_net.h>

#include  "net_if_priv.h"
#include  "net_priv.h"
#include  "net_udp_priv.h"
#include  "net_tcp_priv.h"
#include  "net_task_priv.h"

#ifdef  NET_IPv4_MODULE_EN
#include  "net_arp_priv.h"
#include  "net_ipv4_priv.h"
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  "net_ipv6_priv.h"
#include  "net_mldp_priv.h"
#endif

#ifdef  NET_IF_802x_MODULE_EN
#include  "net_if_802x_priv.h"
#endif

#ifdef  NET_IF_ETHER_MODULE_EN
#include  "net_if_ether_priv.h"
#endif

#ifdef  NET_IF_LOOPBACK_MODULE_EN
#include  "net_if_loopback_priv.h"
#endif

#ifdef  NET_IF_WIFI_MODULE_EN
#include  "net_if_wifi_priv.h"
#endif

#ifdef  NET_IGMP_MODULE_EN
#include  "net_igmp_priv.h"
#endif

#ifdef  NET_DHCP_CLIENT_MODULE_EN
#include  <net/include/dhcp_client.h>
#endif

#include  <common/include/lib_utils.h>

#include  <common/source/collections/slist_priv.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

//                                                                 -------------------- OBJ NAMES ---------------------
#define  NET_IF_RX_Q_NAME                   "Net IF Rx Q"
#define  NET_IF_TX_DEALLOC_Q_NAME           "Net IF Tx Dealloc Q"
#define  NET_IF_TX_SUSPEND_NAME             "Net IF Tx Suspend"

#define  NET_IF_LINK_SUBSCRIBER             "Net IF Link subscriber pool"

#define  NET_IF_CFG_NAME                    "Cfg lock"

#define  NET_IF_DEV_TX_RDY_NAME             "Net IF Dev Tx Rdy"

#define  NET_IF_TX_SUSPEND_TIMEOUT_MIN_MS                  0
#define  NET_IF_TX_SUSPEND_TIMEOUT_MAX_MS                100

#define  NET_IF_SETUP_READY_TIMEOUT_MAX_MS             30000u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct net_if_event_rx {
  NET_IF_NBR   IF_Nbr;
  SLIST_MEMBER ListNode;
} NET_IF_EVENT_RX;

typedef  struct  net_if_event_tx {
  CPU_INT08U   *BufDataPtr;
  SLIST_MEMBER ListNode;
} NET_IF_EVENT_TX;

/********************************************************************************************************
 *                                           INTERFACE MODULE DATA
 *******************************************************************************************************/

typedef  struct  net_if_data {
  NET_IF       *IF_Tbl;

  SLIST_MEMBER *ListEventRxHead;
  SLIST_MEMBER *ListEventRxTail;
  SLIST_MEMBER *ListFreeEventRxHead;
  SLIST_MEMBER *ListEventTxHead;
  SLIST_MEMBER *ListEventTxTail;
  SLIST_MEMBER *ListFreeEventTxHead;

  NET_IF_NBR   NbrBase;
  NET_IF_NBR   NbrNext;

  NET_STAT_CTR RxTaskPktCtr;                                    // Net IF rx task q'd pkts ctr.
  NET_BUF      *TxListHead;                                     // Ptr to net IF tx list head.
  NET_BUF      *TxListTail;                                     // Ptr to net IF tx list tail.

  NET_TMR      *PhyLinkStateTmr;                                // Phy link state tmr.
  CPU_INT16U   PhyLinkStateTime_ms;                             // Phy link state time (in ms   ).

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  NET_TMR      *PerfMonTmr;                                     // Perf mon       tmr.
  CPU_INT16U   PerfMonTime_ms;                                  // Perf mon       time (in ms   ).
#endif
} NET_IF_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_IF_DATA *NetIF_DataPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void NetIF_Q_Update(MEM_SEG     *p_mem_seg,
                           NET_BUF_QTY nbr_rx,
                           NET_BUF_QTY nbr_tx,
                           RTOS_ERR    *p_err);

static void NetIF_ObjInit(NET_IF   *p_if,
                          RTOS_ERR *p_err);

static void NetIF_ObjDel(NET_IF *p_if);

static void NetIF_DevTxRdyWait(NET_IF   *p_if,
                               RTOS_ERR *p_err);

//                                                                 --------------------- RX FNCTS ---------------------
#ifdef  NET_LOAD_BAL_MODULE_EN
static void NetIF_RxHandlerLoadBal(NET_IF *p_if);
#endif

static NET_BUF_SIZE NetIF_RxPkt(NET_IF   *p_if,
                                RTOS_ERR *p_err);

#ifdef  NET_LOAD_BAL_MODULE_EN
static void NetIF_RxPktDec(NET_IF *p_if);
#endif

//                                                                 --------------------- TX FNCTS ---------------------
static void NetIF_TxHandler(NET_BUF  *p_buf,
                            RTOS_ERR *p_err);

static NET_BUF_SIZE NetIF_TxPkt(NET_IF   *p_if,
                                NET_BUF  *p_buf,
                                RTOS_ERR *p_err);

static NET_BUF *NetIF_TxPktListSrch(CPU_INT08U *p_buf_data);

static void NetIF_TxPktListInsert(NET_BUF *p_buf);

static void NetIF_TxPktListRemove(NET_BUF *p_buf);

static void NetIF_TxPktFree(NET_BUF *p_buf);

#ifdef  NET_LOAD_BAL_MODULE_EN
static void NetIF_TxSuspendTimeoutInit(NET_IF *p_if);

static void NetIF_TxSuspendSignal(NET_IF *p_if);

static void NetIF_TxSuspendWait(NET_IF *p_if);
#endif

//                                                                 ------------------ HANDLER FNCTS -------------------
static void NetIF_BufPoolCfgValidate(NET_IF_NBR  if_nbr,
                                     NET_DEV_CFG *p_dev_cfg);

static void *NetIF_GetDataAlignPtr(NET_IF_NBR      if_nbr,
                                   NET_TRANSACTION transaction,
                                   void            *p_data,
                                   RTOS_ERR        *p_err);

static CPU_INT16U NetIF_GetProtocolHdrSize(NET_IF            *p_if,
                                           NET_PROTOCOL_TYPE protocol);

static void NetIF_IO_CtrlHandler(NET_IF_NBR if_nbr,
                                 CPU_INT08U opt,
                                 void       *p_data,
                                 RTOS_ERR   *p_err);

#ifndef  NET_CFG_LINK_STATE_POLL_DISABLED
static void NetIF_PhyLinkStateHandler(void *p_obj);
#endif

#if  (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
static void NetIF_PerfMonHandler(void *p_obj);
#endif

#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static void NetIF_IPv4_LinkLocalCompleteHook(NET_IF_NBR                 if_nbr,
                                             NET_IPv4_ADDR              link_local_addr,
                                             NET_IPv4_LINK_LOCAL_STATUS status,
                                             RTOS_ERR                   err);
#endif

#ifdef NET_DHCP_CLIENT_MODULE_EN
static void NetIF_DHCPc_CompleteHook(NET_IF_NBR    if_nbr,
                                     DHCPc_STATUS  status,
                                     NET_IPv4_ADDR addr,
                                     NET_IPv4_ADDR mask,
                                     NET_IPv4_ADDR gateway,
                                     RTOS_ERR      err);
#endif

#ifdef NET_IPv6_MODULE_EN
static void NetIF_IPv6_AddrHook(NET_IF_NBR               if_nbr,
                                NET_IPv6_CFG_ADDR_TYPE   addr_type,
                                const NET_IPv6_ADDR      *p_addr_cfgd,
                                NET_IPv6_ADDR_CFG_STATUS addr_cfg_status);
#endif

#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
static void NetIF_WaitSetupLinkUpHook(NET_IF_NBR        if_nbr,
                                      NET_IF_LINK_STATE link_state);
#endif

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIF_ValidateAPI(NET_IF_API  *p_if_api,
                              NET_IF_TYPE type,
                              RTOS_ERR    *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  NET_LOAD_BAL_MODULE_EN

#ifndef  NET_IF_CFG_TX_SUSPEND_TIMEOUT_MS
#error  "NET_IF_CFG_TX_SUSPEND_TIMEOUT_MS        not #define'd in 'net_cfg.h'            "
#error  "                                  [MUST be  >= NET_IF_TX_SUSPEND_TIMEOUT_MIN_MS]"
#error  "                                  [     &&  <= NET_IF_TX_SUSPEND_TIMEOUT_MAX_MS]"

#elif   (DEF_CHK_VAL(NET_IF_CFG_TX_SUSPEND_TIMEOUT_MS, \
                     NET_IF_TX_SUSPEND_TIMEOUT_MIN_MS, \
                     NET_IF_TX_SUSPEND_TIMEOUT_MAX_MS) != DEF_OK)
#error  "NET_IF_CFG_TX_SUSPEND_TIMEOUT_MS  illegally #define'd in 'net_cfg.h'            "
#error  "                                  [MUST be  >= NET_IF_TX_SUSPEND_TIMEOUT_MIN_MS]"
#error  "                                  [     &&  <= NET_IF_TX_SUSPEND_TIMEOUT_MAX_MS]"
#endif

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetIF_NbrGetFromName()
 *
 * @brief    Get network interface number from it's name.
 *
 * @param    p_name  Pointer to a string containing interface controller's name.
 *
 * @return   Network interface number associated with controller name.
 *******************************************************************************************************/
NET_IF_NBR NetIF_NbrGetFromName(CPU_CHAR *p_name)
{
  NET_IF_NBR if_nbr;

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_NbrGetFromName);

  //                                                               ---------- SEARCH NAME IN INTERFACE LIST -----------
  for (if_nbr = 0; if_nbr < Net_CoreDataPtr->IF_NbrCfgd; if_nbr++) {
    CPU_INT16S cmp_result;

    cmp_result = Str_Cmp(p_name, NetIF_DataPtr->IF_Tbl[if_nbr].Name);
    if (cmp_result == 0) {
      goto exit_release;
    }
  }

  if_nbr = NET_IF_NBR_NONE;                                     // Return invalid interface number if not found.

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (if_nbr);
}

/****************************************************************************************************//**
 *                                           NetIF_WaitSetupReady()
 *
 * @brief    Wait for the interface setup to be complete after a call to NetIF_xxx_Start().
 *
 * @param    if_nbr      Network interface number on which to wait.
 *
 * @param    p_info      Pointer to structure in which the setup information will be copied.
 *                       DEF_NULL, to not receive the setup information.
 *
 * @param    timeout_ms  Timeout value after which the function will stop waiting, in milliseconds.
 *                       A value of 0 will timed out after the max timeout setup by the stack.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_ABORT
 *******************************************************************************************************/
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
void NetIF_WaitSetupReady(NET_IF_NBR      if_nbr,
                          NET_IF_APP_INFO *p_info,
                          CPU_INT32U      timeout_ms,
                          RTOS_ERR        *p_err)
{
  NET_IF           *p_if = DEF_NULL;
  NET_IF_START_OBJ *p_start_obj = DEF_NULL;
  SLIST_MEMBER     *p_node;
  NET_IF_START_MSG *p_msg;
  CPU_BOOLEAN      done = DEF_NO;
  CPU_INT32U       timeout;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_WaitSetupReady);

  if (p_info != DEF_NULL) {
    Mem_Clr(p_info, sizeof(p_info));
  }

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               ------- INITIALIZE IF INFO STRUCT TO RETURN --------
  if (p_info != DEF_NULL) {
    p_info->StatusIPv4LinkLocal = NET_IPv4_LINK_LOCAL_STATUS_NONE;
    p_info->AddrLinkLocalIPv4 = NET_IPv4_ADDR_NONE;
    p_info->StatusDHCP = DHCPc_STATUS_NONE;
    p_info->AddrDynIPv4 = NET_IPv4_ADDR_NONE;
    p_info->AddrDynMaskIPv4 = NET_IPv4_ADDR_NONE;
    p_info->AddrDynGatewayIPv4 = NET_IPv4_ADDR_NONE;
    p_info->StatusStaticIPv6 = NET_IPv6_ADDR_CFG_STATUS_NONE;
    p_info->StatusLinkLocalIPv6 = NET_IPv6_ADDR_CFG_STATUS_NONE;
    p_info->StatusGlobalIPv6 = NET_IPv6_ADDR_CFG_STATUS_NONE;
    NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(p_info->AddrStaticIPv6);
    NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(p_info->AddrLinkLocalIPv6);
    NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(p_info->AddrGlobalIPv6);
  }

  p_start_obj = p_if->StartObj;

  p_start_obj->AppInfoPtr = p_info;

  //                                                               -------- CHECK IF ALL SETUPS ALREADY READY ---------
  if ((p_start_obj->RdyFlags.IF_LinkUpRdy == DEF_YES)
      && (p_start_obj->RdyFlags.IPv4_LinkLocalRdy == DEF_YES)
      && (p_start_obj->RdyFlags.DHCPc_Rdy == DEF_YES)
      && (p_start_obj->RdyFlags.IPv6_StaticRdy == DEF_YES)
      && (p_start_obj->RdyFlags.IPv6_LinkLocalRdy == DEF_YES)
      && (p_start_obj->RdyFlags.IPv6_GlobalRdy == DEF_YES)) {
    goto exit_release;
  }

  //                                                               ------------ CALCULATE MAXIMUM TIMEOUT -------------
  timeout = (timeout_ms == 0) ? NET_IF_SETUP_READY_TIMEOUT_MAX_MS : timeout_ms;

  //                                                               ---- WAIT FOR ALL THE MODULE SETUP READY SIGNAL ----
  while (done != DEF_YES) {
    Net_GlobalLockRelease();

    KAL_SemPend(p_start_obj->SemHandle, KAL_OPT_PEND_BLOCKING, timeout, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      Net_GlobalLockAcquire((void *)NetIF_WaitSetupReady);
      goto exit_release;
    }

    Net_GlobalLockAcquire((void *)NetIF_WaitSetupReady);

    CORE_ENTER_ATOMIC();
    p_node = SList_Pop(&p_start_obj->MsgListPtr);
    CORE_EXIT_ATOMIC();

    if (p_node != DEF_NULL) {
      p_msg = SLIST_ENTRY(p_node, NET_IF_START_MSG, ListNode);

      switch (p_msg->Type) {
        case NET_IF_START_MSG_TYPE_LINK_UP:
          if (p_msg->IF_Nbr == if_nbr) {
            p_start_obj->RdyFlags.IF_LinkUpRdy = DEF_YES;

            NetIF_LinkStateUnSubscribeHandler(if_nbr, NetIF_WaitSetupLinkUpHook, p_err);
            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              goto exit_release;
            }
          }
          break;

        case NET_IF_START_MSG_TYPE_IPv4_LINK_LOCAL:
          if (p_msg->IF_Nbr == if_nbr) {
            p_start_obj->RdyFlags.IPv4_LinkLocalRdy = DEF_YES;
          }
          break;

        case NET_IF_START_MSG_TYPE_DHCP:
          if (p_msg->IF_Nbr == if_nbr) {
            p_start_obj->RdyFlags.DHCPc_Rdy = DEF_YES;
          }
          break;

        case NET_IF_START_MSG_TYPE_IPv6_STATIC:
          if (p_msg->IF_Nbr == if_nbr) {
            p_start_obj->RdyFlags.IPv6_StaticRdy = DEF_YES;
          }
          break;

        case NET_IF_START_MSG_TYPE_IPv6_LINK_LOCAL:
          if (p_msg->IF_Nbr == if_nbr) {
            p_start_obj->RdyFlags.IPv6_LinkLocalRdy = DEF_YES;
          }
          break;

        case NET_IF_START_MSG_TYPE_IPv6_GLOBAL:
          if (p_msg->IF_Nbr == if_nbr) {
            p_start_obj->RdyFlags.IPv6_GlobalRdy = DEF_YES;
          }
          break;

        default:
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);
      goto exit_release;
    }

    //                                                             ------- STOP WAITING IF ALL SETUPS ARE READY -------
    if ((p_start_obj->RdyFlags.IF_LinkUpRdy == DEF_YES)
        && (p_start_obj->RdyFlags.IPv4_LinkLocalRdy == DEF_YES)
        && (p_start_obj->RdyFlags.DHCPc_Rdy == DEF_YES)
        && (p_start_obj->RdyFlags.IPv6_StaticRdy == DEF_YES)
        && (p_start_obj->RdyFlags.IPv6_LinkLocalRdy == DEF_YES)
        && (p_start_obj->RdyFlags.IPv6_GlobalRdy == DEF_YES)) {
      done = DEF_YES;
    }
  }

exit_release:
  //                                                               ----------------- DELETE SEMAPHORE -----------------
  if (KAL_SEM_HANDLE_IS_NULL(p_start_obj->SemHandle) != DEF_YES) {
    KAL_SemDel(p_start_obj->SemHandle);
    p_start_obj->SemHandle = KAL_SemHandleNull;
  }

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
}
#endif

/****************************************************************************************************//**
 *                                               NetIF_Add()
 *
 * @brief    Add & initialize a specific instance of a network interface.
 *
 * @param    p_if_api    Pointer to specific network interface API.
 *
 * @param    p_dev_api   Pointer to specific network device driver API.
 *
 * @param    p_dev_bsp   Pointer to specific network device board-specific API.
 *
 * @param    p_dev_cfg   Pointer to specific network device hardware configuration.
 *
 * @param    p_ext_api   Pointer to specific network extension layer API.
 *
 * @param    p_ext_cfg   Pointer to specific network extension layer configuration.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_NO_MORE_RSRC
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *
 * @return   Interface number of the added interface, if NO error(s).
 *           NET_IF_NBR_NONE, otherwise.
 *
 * @note     (1) This function is DEPRECATED and will be removed in a future version of this product.
 *               Instead, use NetIF_Ether_Add() or NetIF_WiFi_Add().
 *
 * @note     (2) Network physical layer arguments MAY be NULL for any of the following :
 *               - The network device does not require   Physical layer support.
 *               - The network device uses an integrated Physical layer supported from within
 *               the network device &/or the network device driver.
 *               @n
 *               However, if network physical layer API is available, then ALL network physical
 *               layer API arguments MUST be provided.
 *
 * @internal
 * @note     (3) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 *
 * @note     (4) [INTERNAL]
 *               - (a) The following parameters MUST be configured PRIOR to initializing the specific
 *                     network interface/device so that the initialized network interface is valid :
 *                   - (1) The network interface's initialization flag MUST be set; ...
 *                   - (2) The next available interface number MUST be incremented.
 *               - (b) On ANY error(s), network interface parameters MUST be appropriately reset :
 *                   - (1) The network interface's initialization flag SHOULD be cleared; ...
 *                   - (2) The next available interface number MUST be decremented.
 * @endinternal
 *******************************************************************************************************/
NET_IF_NBR NetIF_Add(void     *p_if_api,
                     void     *p_dev_api,
                     void     *p_dev_bsp,
                     void     *p_dev_cfg,
                     void     *p_ext_api,
                     void     *p_ext_cfg,
                     RTOS_ERR *p_err)
{
  NET_IF      *p_if = DEF_NULL;
  NET_IF_API  *p_if_api_local = DEF_NULL;
  NET_IF_NBR  if_nbr = NET_IF_NBR_NONE;
  NET_DEV_CFG *p_dev_cfg_local = DEF_NULL;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_IF_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_if_api != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IF_NBR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_dev_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IF_NBR_NONE);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_Add);

  //                                                               -------------- VALIDATE NET IF AVAIL ---------------
  CORE_ENTER_ATOMIC();
  if_nbr = NetIF_DataPtr->NbrNext;                              // Get cur net IF nbr.
  CORE_EXIT_ATOMIC();
  if (if_nbr >= Net_CoreDataPtr->IF_NbrTot) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NO_MORE_RSRC);
    goto exit_fail_init;
  }

  CORE_ENTER_ATOMIC();
  NetIF_DataPtr->NbrNext++;                                     // Inc to next avail net IF nbr (see Note #5a2).
  Net_CoreDataPtr->IF_NbrCfgd = NetIF_DataPtr->NbrNext;
  CORE_EXIT_ATOMIC();

  if ((if_nbr != NET_IF_NBR_LOOPBACK)
      && (p_dev_api == DEF_NULL)           ) {
    Net_GlobalLockRelease();
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NULL_PTR, NET_IF_NBR_NONE);
  }

  //                                                               ------------------- INIT NET IF --------------------
  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];
  p_if->Nbr = if_nbr;
  p_if->IF_API = p_if_api;
  p_if->Dev_API = p_dev_api;
  p_if->Dev_BSP = p_dev_bsp;
  p_if->Dev_Cfg = p_dev_cfg;
  p_if->Ext_API = p_ext_api;
  p_if->Ext_Cfg = p_ext_cfg;
  p_if->En = DEF_DISABLED;
  p_if->Link = NET_IF_LINK_DOWN;
  p_if->LinkPrev = NET_IF_LINK_DOWN;

  p_if->StartModulesCfgFlags.DHCPc = 0u;
  p_if->StartModulesCfgFlags.IPv4_LinkLocal = 0u;
  p_if->StartModulesCfgFlags.IPv6_AutoCfg = 0u;
  p_if->StartModulesCfgFlags.IPv6_Static = 0u;

  CORE_ENTER_ATOMIC();
  p_if->Init = DEF_YES;
  CORE_EXIT_ATOMIC();

  //                                                               --------------- INIT SPECIFIC NET IF ---------------
  p_if_api_local = (NET_IF_API *)p_if->IF_API;

  NetIF_ObjInit(p_if, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail;
  }

  RTOS_ASSERT_DBG_ERR_SET((p_if_api_local->Add != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IF_NBR_NONE);

  p_if_api_local->Add(p_if, p_err);                             // Init/add IF & dev.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail_deinit;
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  NetIF_ValidateAPI(p_if_api_local, p_if->Type, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail;
  }
#endif

  p_dev_cfg_local = (NET_DEV_CFG *)p_dev_cfg;
  NetIF_Q_Update(Net_CoreDataPtr->CoreMemSegPtr,
                 p_dev_cfg_local->RxBufLargeNbr,
                 p_dev_cfg_local->TxBufLargeNbr + p_dev_cfg_local->TxBufSmallNbr,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail_deinit;
  }

  //                                                               -------- INIT IP LAYER FOR ADDED INTERFACE ---------
  NetIP_IF_Init(if_nbr, DEF_NULL, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail_deinit;
  }

  goto exit_release;

exit_fail_deinit:
  NetIF_ObjDel(p_if);

exit_fail:
  if (p_if != DEF_NULL) {
    CORE_ENTER_ATOMIC();                                        // On any err(s);                                  ...
    p_if->Init = DEF_NO;                                        // ... Clr net IF init                           & ...
    NetIF_DataPtr->NbrNext--;                                   // ... dec next avail net IF nbr.
    CORE_EXIT_ATOMIC();
  }

exit_fail_init:
  if_nbr = NET_IF_NBR_NONE;

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (if_nbr);
}

/****************************************************************************************************//**
 *                                               NetIF_Start()
 *
 * @brief    Start a network interface.
 *
 * @param    if_nbr  Network interface number to start.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_TYPE
 *                       - RTOS_ERR_NOT_INIT
 *                       - RTOS_ERR_TX
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_RX
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_NOT_SUPPORTED
 *                       - RTOS_ERR_NOT_READY
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_NET_IF_LINK_DOWN
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_NET_NEXT_HOP
 *                       - RTOS_ERR_INVALID_STATE
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @note     (1) This function is DEPRECATED and will be removed in a future version of this product.
 *                   Instead, use NetIF_Ether_Start() or NetIF_WiFi_Start().
 *
 * @internal
 * @note     (2) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 *
 * @note     (3) [INTERNAL] Each specific network interface 'Start()' is responsible for setting the
 *               initial link state after starting a network interface.
 *                   - (a) Specific network interface that do not require link state MUST set the network
 *                         interface's link state to 'UP'.
 * @endinternal
 *******************************************************************************************************/
void NetIF_Start(NET_IF_NBR if_nbr,
                 RTOS_ERR   *p_err)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;
#ifdef  NET_MLDP_MODULE_EN
  NET_IPv6_ADDR addr_allnode_mcast;
#endif

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_Start);                   // See Note #2b.

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  if (p_if->En != DEF_DISABLED) {                                // If net IF already started, ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);                // ... rtn err.
    goto exit_release;
  }

  //                                                               ------------------- START NET IF -------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;                        // Get Interface from  API NET_IF_API object.

  p_if_api->Start(p_if, p_err);                                 // Init/start Interface & device HW.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_if->En = DEF_ENABLED;                                       // Enable Interface after device start.

  //                                                               -------- CREATE IPv6 ALL-NODE MCAST ADDRESS --------
#ifdef   NET_MLDP_MODULE_EN
  if (p_if->Type != NET_IF_TYPE_LOOPBACK) {
    NetIPv6_AddrMcastAllNodesSet(&addr_allnode_mcast);

    NetMLDP_HostGrpJoinHandler(if_nbr,
                               &addr_allnode_mcast,
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_stop;
    }
  }
#endif

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  p_if->PerfMonState = NET_IF_PERF_MON_STATE_START;             // Start performance monitor.
#endif

  goto exit_release;

#ifdef   NET_MLDP_MODULE_EN
exit_stop:
  p_if->En = DEF_DISABLED;                                      // Disable Interface after in case of error(s).
  p_if_api->Stop(p_if, p_err);
#endif

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
  return;
}

/****************************************************************************************************//**
 *                                               NetIF_Stop()
 *
 * @brief    (1) Stop a network interface.
 *
 * @param    if_nbr  Network interface number to stop.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_INVALID_STATE
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 *
 * @note     (2) [INTERNAL] Each specific network interface 'Stop()' SHOULD be responsible for clearing
 *               the link state after stopping a network interface.  However, clearing the link state
 *               to 'DOWN' is included for completeness & as an extra precaution in case the specific
 *               network interface 'Stop()' fails to clear the link state.
 * @endinternal
 *******************************************************************************************************/
void NetIF_Stop(NET_IF_NBR if_nbr,
                RTOS_ERR   *p_err)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------ STOP PROCESS RUNNING ON IF ------------
#ifdef NET_DHCP_CLIENT_MODULE_EN
  DHCPc_IF_Remove(if_nbr, p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
    case RTOS_ERR_NOT_FOUND:
      break;

    default:
      return;
  }
#endif

  //                                                               -------- REMOVE ADDRESSES CONFIGURED ON IF ---------
#ifdef NET_IPv4_MODULE_EN
  NetIPv4_CfgAddrRemoveAll(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

#ifdef NET_IPv6_MODULE_EN
  NetIPv6_CfgAddrRemoveAll(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_Stop);                    // See Note #2b.

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  if (p_if->En != DEF_ENABLED) {                                // If net IF NOT started, ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);               // ... rtn err.
    goto exit_release;
  }

  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               ------------------- STOP NET IF --------------------
  p_if_api->Stop(p_if, p_err);                                  // Stop IF & dev hw.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_if->En = DEF_DISABLED;
  p_if->Link = NET_IF_LINK_DOWN;                                // See Note #4.
  p_if->LinkPrev = NET_IF_LINK_DOWN;
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  p_if->PerfMonState = NET_IF_PERF_MON_STATE_STOP;
#endif

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetIF_CfgPhyLinkPeriod()
 *
 * @brief    Configure Network Interface Physical Link State Handler time (i.e. scheduling period).
 *
 * @param    time_ms     Desired value for Network Interface Physical Link State Handler time
 *                       (in milliseconds).
 *
 * @return   DEF_OK,   Network Interface Physical Link State Handler timeout configured.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) Configured time does NOT reschedule the next physical link state handling but
 *               configures the scheduling of all subsequent  physical link state handling.
 *
 * @internal
 * @note     (2) [INTERNAL] 'NetIF_DataPtr->PhyLinkStateTime_ms' variables MUST ALWAYS be accessed exclusively
 *               in critical sections.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_CfgPhyLinkPeriod(CPU_INT16U time_ms)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG((NetIF_DataPtr != DEF_NULL), RTOS_ERR_NOT_INIT, DEF_FAIL);

  if (time_ms < NET_IF_PHY_LINK_TIME_MIN_MS) {
    return (DEF_FAIL);
  }
  if (time_ms > NET_IF_PHY_LINK_TIME_MAX_MS) {
    return (DEF_FAIL);
  }

  CORE_ENTER_ATOMIC();
  NetIF_DataPtr->PhyLinkStateTime_ms = time_ms;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           NetIF_CfgPerfMonPeriod()
 *
 * @brief    Configure Network Interface Performance Monitor Handler time (i.e. scheduling period).
 *
 * @param    time_ms     Desired value for Network Interface Performance Monitor Handler time
 *                       (in milliseconds).
 *
 * @return   DEF_OK,   Network Interface Performance Monitor Handler time configured.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) Configured time does NOT reschedule the next performance monitor handling but
 *               configures the scheduling of all  subsequent performance monitor handling.
 *
 * @internal
 * @note     (2) [INTERNAL] 'NetIF_DataPtr->PerfMonTime_ms' variables MUST ALWAYS be accessed exclusively
 *               in critical sections.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_CfgPerfMonPeriod(CPU_INT16U time_ms)
{
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG((NetIF_DataPtr != DEF_NULL), RTOS_ERR_NOT_INIT, DEF_FAIL);

  if (time_ms < NET_IF_PERF_MON_TIME_MIN_MS) {
    return (DEF_FAIL);
  }
  if (time_ms > NET_IF_PERF_MON_TIME_MAX_MS) {
    return (DEF_FAIL);
  }

  CORE_ENTER_ATOMIC();
  NetIF_DataPtr->PerfMonTime_ms = time_ms;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
#else
  PP_UNUSED_PARAM(time_ms);
  return (DEF_FAIL);
#endif
}

/****************************************************************************************************//**
 *                                           NetIF_GetRxDataAlignPtr()
 *
 * @brief    Get aligned pointer into a receive application data buffer.
 *
 * @param    if_nbr  Network interface number to get a receive application buffer's aligned data
 *                   pointer.
 *
 * @param    p_data  Pointer to receive application data buffer to get an aligned pointer into.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   Pointer to aligned receive application data buffer address, if NO error(s).
 *           Pointer to NULL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) Optimal alignment between application data buffer(s) & network interface's
 *               network buffer data area(s) is NOT guaranteed & is possible if & only if
 *               all of the following condition is true :
 *               @n
 *               -   Network interface's network buffer data area(s) MUST be aligned to a
 *                   multiple of the CPU's data word size.
 *               @n
 *               Otherwise, a single, fixed alignment between application data buffer(s) &
 *               network interface's buffer data area(s) is NOT possible.
 *
 * @note     (3) Even when application data buffers & network buffer data areas are aligned
 *               in the best case; optimal alignment is NOT guaranteed for every read/write
 *               of data to/from application data buffers & network buffer data areas.
 *               @n
 *               For any single read/write of data to/from application data buffers & network
 *               buffer data areas, optimal alignment occurs if & only if all of the following
 *               conditions are true :
 *               @n
 *               -   Data read/written to/from application data buffer(s) to network buffer
 *                   data area(s) MUST start on addresses with the same relative offset from
 *                   CPU word-aligned addresses.
 *               @n
 *                   In other words, the modulus of the specific read/write address in the
 *                   application data buffer with the CPU's data word size MUST be equal to
 *                   the modulus of the specific read/write address in the network buffer
 *                   data area with the CPU's data word size.
 *               @n
 *                   This condition MIGHT NOT be satisfied whenever :
 *                   -   Data is read/written to/from fragmented packets
 *                   -   Data is NOT maximally read/written to/from stream-type packets
 *                           (e.g. TCP data segments)
 *                   -   Packets include variable number of header options (e.g. IP options)
 *               @n
 *               However, even though optimal alignment between application data buffers &
 *               network buffer data areas is NOT guaranteed for every read/write; optimal
 *               alignment SHOULD occur more frequently leading to improved network data
 *               throughput.
 *               @n
 *               Since the first aligned address in the application data buffer may be 0 to
 *               (CPU_CFG_DATA_SIZE - 1) octets after the application data buffer's starting
 *               address, the application data buffer SHOULD allocate & reserve an additional
 *               (CPU_CFG_DATA_SIZE - 1) number of octets.
 *               @n
 *               However, the application data buffer's effective, usable size is still limited
 *               to its original declared size (before reserving additional octets) & SHOULD NOT
 *               be increased by the additional, reserved octets.
 *******************************************************************************************************/
void *NetIF_GetRxDataAlignPtr(NET_IF_NBR if_nbr,
                              void       *p_data,
                              RTOS_ERR   *p_err)
{
  void *p_data_align;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_data_align = NetIF_GetDataAlignPtr(if_nbr, NET_TRANSACTION_RX, p_data, p_err);

  return (p_data_align);
}

/****************************************************************************************************//**
 *                                           NetIF_GetTxDataAlignPtr()
 *
 * @brief    Get aligned pointer into a transmit application data buffer.
 *
 * @param    if_nbr  Network interface number to get a transmit application buffer's aligned data
 *                   pointer.
 *
 * @param    p_data  Pointer to transmit application data buffer to get an aligned pointer into
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   Pointer to aligned transmit application data buffer address, if NO error(s).
 *           Pointer to NULL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) Optimal alignment between application data buffer(s) & network interface's
 *               network buffer data area(s) is NOT guaranteed & is possible if & only if
 *               all of the following condition is true :
 *               @n
 *               -   Network interface's network buffer data area(s) MUST be aligned to a
 *                   multiple of the CPU's data word size.
 *               @n
 *               Otherwise, a single, fixed alignment between application data buffer(s) &
 *               network interface's buffer data area(s) is NOT possible.
 *
 * @note     (3) Even when application data buffers & network buffer data areas are aligned
 *               in the best case; optimal alignment is NOT guaranteed for every read/write
 *               of data to/from application data buffers & network buffer data areas.
 *               @n
 *               For any single read/write of data to/from application data buffers & network
 *               buffer data areas, optimal alignment occurs if & only if all of the following
 *               conditions are true :
 *               @n
 *               -   Data read/written to/from application data buffer(s) to network buffer
 *                   data area(s) MUST start on addresses with the same relative offset from
 *                   CPU word-aligned addresses.
 *               @n
 *                   In other words, the modulus of the specific read/write address in the
 *                   application data buffer with the CPU's data word size MUST be equal to
 *                   the modulus of the specific read/write address in the network buffer
 *                   data area with the CPU's data word size.
 *               @n
 *                   This condition MIGHT NOT be satisfied whenever :
 *               @n
 *                   -   Data is read/written to/from fragmented packets
 *                   -   Data is NOT maximally read/written to/from stream-type packets
 *                           (e.g. TCP data segments)
 *                   -   Packets include variable number of header options (e.g. IP options)
 *               @n
 *               However, even though optimal alignment between application data buffers &
 *               network buffer data areas is NOT guaranteed for every read/write; optimal
 *               alignment SHOULD occur more frequently leading to improved network data
 *               throughput.
 *               @n
 *               Since the first aligned address in the application data buffer may be 0 to
 *               (CPU_CFG_DATA_SIZE - 1) octets after the application data buffer's starting
 *               address, the application data buffer SHOULD allocate & reserve an additional
 *               (CPU_CFG_DATA_SIZE - 1) number of octets.
 *               @n
 *               However, the application data buffer's effective, usable size is still limited
 *               to its original declared size (before reserving additional octets) & SHOULD NOT
 *               be increased by the additional, reserved octets.
 *******************************************************************************************************/
void *NetIF_GetTxDataAlignPtr(NET_IF_NBR if_nbr,
                              void       *p_data,
                              RTOS_ERR   *p_err)
{
  void *p_data_align;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_data_align = NetIF_GetDataAlignPtr(if_nbr, NET_TRANSACTION_TX, p_data, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (p_data_align);
}

/****************************************************************************************************//**
 *                                               NetIF_IsValid()
 *
 * @brief    Validate network interface number.
 *
 * @param    if_nbr  Network interface number to validate.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *
 * @return   DEF_YES, network interface number valid.
 *           DEF_NO,  network interface number invalid / NOT yet configured.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_IsValid(NET_IF_NBR if_nbr,
                          RTOS_ERR   *p_err)
{
  CPU_BOOLEAN valid = DEF_NO;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_IsValid);                 // See Note #1b.

  //                                                               ----------------- VALIDATE IF NBR ------------------
  valid = NetIF_IsValidHandler(if_nbr);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (valid);
}

/****************************************************************************************************//**
 *                                           NetIF_IsValidCfgd()
 *
 * @brief    Validate configured network interface number.
 *
 * @param    if_nbr  Network interface number to validate.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *
 * @return   DEF_YES, network interface number valid.
 *           DEF_NO,  network interface number invalid / NOT yet configured or reserved.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_IsValidCfgd(NET_IF_NBR if_nbr,
                              RTOS_ERR   *p_err)
{
  CPU_BOOLEAN valid = DEF_NO;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_IsValidCfgd);             // See Note #1b.

  //                                                               ----------------- VALIDATE IF NBR ------------------
  valid = NetIF_IsValidCfgdHandler(if_nbr);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (valid);
}

/****************************************************************************************************//**
 *                                               NetIF_IsEn()
 *
 * @brief    Validate network interface as enabled.
 *
 * @param    if_nbr  Network interface number to validate.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_YES, network interface   valid  & enabled.
 *           DEF_NO,  network interface invalid or disabled.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_IsEn(NET_IF_NBR if_nbr,
                       RTOS_ERR   *p_err)
{
  CPU_BOOLEAN en = DEF_NO;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_IsEn);                    // See Note #1b.

  //                                                               --------------- VALIDATE NET IF EN'D ---------------
  en = NetIF_IsEnHandler(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (en);
}

/****************************************************************************************************//**
 *                                               NetIF_IsEnCfgd()
 *
 * @brief    Validate configured network interface as enabled.
 *
 * @param    if_nbr  Network interface number to validate.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_YES, network interface   valid  &  enabled.
 *           DEF_NO,  network interface invalid or disabled.
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
CPU_BOOLEAN NetIF_IsEnCfgd(NET_IF_NBR if_nbr,
                           RTOS_ERR   *p_err)
{
  CPU_BOOLEAN en = DEF_NO;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_IsEnCfgd);                // See Note #1b.

  //                                                               --------------- VALIDATE NET IF EN'D ---------------
  en = NetIF_IsEnCfgdHandler(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (en);
}

/****************************************************************************************************//**
 *                                           NetIF_GetExtAvailCtr()
 *
 * @brief    Return number of external interface configured.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *
 * @return   Number of external interface available
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
CPU_INT08U NetIF_GetExtAvailCtr(RTOS_ERR *p_err)
{
  CPU_INT08U nbr = 0u;
  CPU_INT08U init = NET_IF_NBR_BASE;

#ifdef NET_IF_LOOPBACK_MODULE_EN
  init = NET_IF_NBR_LOOPBACK;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_GetExtAvailCtr);          // See Note #1b.

  nbr = NetIF_DataPtr->NbrNext - init;

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (nbr);
}

/****************************************************************************************************//**
 *                                           NetIF_GetNbrBaseCfgd()
 *
 * @brief    Get the interface base number (first interface ID).
 *
 * @return   Interface base number.
 *******************************************************************************************************/
NET_IF_NBR NetIF_GetNbrBaseCfgd(void)
{
  return (NET_IF_NBR_BASE_CFGD);
}

/****************************************************************************************************//**
 *                                               NetIF_TypeGet()
 *
 * @brief    Get the network interface type from the interface number.
 *
 * @param    if_nbr  Network interface number.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   Network Interface type:
 *               - NET_IF_TYPE_ETHER
 *               - NET_IF_TYPE_WIFI
 *               - NET_IF_TYPE_NONE, in case of error.
 *******************************************************************************************************/
NET_IF_TYPE NetIF_TypeGet(NET_IF_NBR if_nbr,
                          RTOS_ERR   *p_err)
{
  NET_IF *p_if = DEF_NULL;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_IF_TYPE_NONE);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Net_GlobalLockAcquire((void *)NetIF_TypeGet);

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    Net_GlobalLockRelease();
    return (NET_IF_TYPE_NONE);
  }

  Net_GlobalLockRelease();

  return (p_if->Type);
}

/****************************************************************************************************//**
 *                                           NetIF_AddrHW_Get()
 *
 * @brief    Get network interface's hardware address.
 *
 * @param    if_nbr      Network interface number to get hardware address.
 *
 * @param    p_addr_hw   Pointer to variable that will receive the hardware address.
 *
 * @param    p_addr_len  Pointer to a variable that will receive the address length.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) The hardware address is returned in network-order; i.e. the pointer to the hardware
 *               address points to the highest-order octet.
 *******************************************************************************************************/
void NetIF_AddrHW_Get(NET_IF_NBR if_nbr,
                      CPU_INT08U *p_addr_hw,
                      CPU_INT08U *p_addr_len,
                      RTOS_ERR   *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_hw != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_len != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_AddrHW_Get);              // See Note #1b.

  //                                                               If init NOT complete, exit (see Note #2).

  //                                                               ---------------- GET NET IF HW ADDR ----------------
  NetIF_AddrHW_GetHandler(if_nbr, p_addr_hw, p_addr_len, p_err);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetIF_AddrHW_Set()
 *
 * @brief    Set network interface's hardware address.
 *
 * @param    if_nbr      Network interface number to set hardware address.
 *
 * @param    p_addr_hw   Pointer to hardware address.
 *
 * @param    addr_len    Length  of hardware address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_INVALID_STATE
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) The hardware address MUST be in network-order; i.e. the pointer to the hardware
 *               address MUST point to the highest-order octet.
 *
 * @note     (3) The interface MUST be stopped BEFORE setting a new hardware address, which does
 *               NOT take effect until the interface is re-started.
 *******************************************************************************************************/
void NetIF_AddrHW_Set(NET_IF_NBR if_nbr,
                      CPU_INT08U *p_addr_hw,
                      CPU_INT08U addr_len,
                      RTOS_ERR   *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_hw != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_AddrHW_Set);              // See Note #1b.

  //                                                               ---------------- SET NET IF HW ADDR ----------------
  NetIF_AddrHW_SetHandler(if_nbr, p_addr_hw, addr_len, p_err);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetIF_AddrHW_IsValid()
 *
 * @brief    Validate network interface's hardware address.
 *
 * @param    if_nbr      Interface number to validate the hardware address.
 *
 * @param    p_addr_hw   Pointer to an interface hardware address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_YES, if hardware address valid.
 *           DEF_NO,  otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) The hardware address MUST be in network-order; i.e. the pointer to the hardware
 *               address MUST point to the highest-order octet.
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_AddrHW_IsValid(NET_IF_NBR if_nbr,
                                 CPU_INT08U *p_addr_hw,
                                 RTOS_ERR   *p_err)
{
  CPU_BOOLEAN valid = DEF_NO;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_hw != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NO);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #1b.
  Net_GlobalLockAcquire((void *)NetIF_AddrHW_IsValid);

  //                                                               ------------- VALIDATE NET IF HW ADDR --------------
  valid = NetIF_AddrHW_IsValidHandler(if_nbr, p_addr_hw, p_err);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (valid);
}

/****************************************************************************************************//**
 *                                               NetIF_MTU_Get()
 *
 * @brief    Get network interface's MTU.
 *
 * @param    if_nbr  Network interface number to get MTU.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   Network interface's MTU, if NO error(s).
 *           0, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
NET_MTU NetIF_MTU_Get(NET_IF_NBR if_nbr,
                      RTOS_ERR   *p_err)
{
  NET_IF  *p_if = DEF_NULL;
  NET_MTU mtu = 0u;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_MTU_Get);                 // See Note #1b.

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               ------------------ GET NET IF MTU ------------------
  mtu = p_if->MTU;

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (mtu);
}

/****************************************************************************************************//**
 *                                               NetIF_MTU_Set()
 *
 * @brief    Set network interface's MTU.
 *
 * @param    if_nbr  Network interface number to set MTU.
 *
 * @param    mtu     Desired maximum transmission unit size to configure.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
void NetIF_MTU_Set(NET_IF_NBR if_nbr,
                   NET_MTU    mtu,
                   RTOS_ERR   *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_MTU_Set);                 // See Note #1b.

  //                                                               ------------------ SET NET IF MTU ------------------
  NetIF_MTU_SetHandler(if_nbr, mtu, p_err);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetIF_LinkStateGet()
 *
 * @brief    Get network interface's last known physical link state.
 *
 * @param    if_nbr  Network interface number to get last known physical link state.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   NET_IF_LINK_UP,   if NO error(s) & network interface's last known physical link state was 'UP'.
 *           NET_IF_LINK_DOWN, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (3) NetIF_LinkStateGet() only returns a network interface's last known physical link state
 *               since enabled network interfaces' physical link states are only periodically updated.
 *******************************************************************************************************/
NET_IF_LINK_STATE NetIF_LinkStateGet(NET_IF_NBR if_nbr,
                                     RTOS_ERR   *p_err)
{
  NET_IF_LINK_STATE link_state = NET_IF_LINK_DOWN;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_IF_LINK_DOWN);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_LinkStateGet);            // See Note #1b.

  link_state = NetIF_LinkStateGetHandler(if_nbr, p_err);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (link_state);
}

/****************************************************************************************************//**
 *                                       NetIF_LinkStateWaitUntilUp()
 *
 * @brief    Wait for a network interface's link state to be 'UP'.
 *
 * @param    if_nbr          Network interface number to check link state.
 *
 * @param    retry_max       Maximum number of consecutive wait retries.
 *
 * @param    time_dly_ms     Transitory delay value, in milliseconds.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_NET_IF_LINK_DOWN
 *
 * @return   NET_IF_LINK_UP,   if NO error(s) & network interface's link state is 'UP'.
 *           NET_IF_LINK_DOWN, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) If a non-zero number of retries is requested then a non-zero time delay SHOULD also be
 *               requested; otherwise, all retries will most likely fail immediately since no time will
 *               elapse to wait for & allow the network interface's link state to successfully be 'UP'.
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_LinkStateWaitUntilUp(NET_IF_NBR if_nbr,
                                       CPU_INT16U retry_max,
                                       CPU_INT32U time_dly_ms,
                                       RTOS_ERR   *p_err)
{
  NET_IF_LINK_STATE link_state = NET_IF_LINK_DOWN;
  CPU_BOOLEAN       done = DEF_NO;
  CPU_BOOLEAN       dly = DEF_NO;
  CPU_INT16U        retry_cnt = 0u;
  RTOS_ERR          local_err;
  RTOS_ERR          err_rtn;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_IF_LINK_DOWN);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  RTOS_ERR_SET(err_rtn, RTOS_ERR_NONE);

  //                                                               --------- WAIT FOR NET IF LINK STATE 'UP' ----------
  while ((retry_cnt <= retry_max)                               // While retry <= max retry ...
         && (done == DEF_NO)) {                                 // ... & link NOT UP,       ...
    if (dly == DEF_YES) {
      KAL_Dly(time_dly_ms);
    }

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    link_state = NetIF_LinkStateGet(if_nbr, &local_err);        // ... chk link state.
    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:
        if (link_state == NET_IF_LINK_UP) {
          done = DEF_YES;
          RTOS_ERR_SET(err_rtn, RTOS_ERR_NONE);
        } else {
          retry_cnt++;
          dly = DEF_YES;
          RTOS_ERR_SET(err_rtn, RTOS_ERR_NET_IF_LINK_DOWN);
        }
        break;

      case RTOS_ERR_NOT_INIT:                                   // If transitory err(s), ...
        retry_cnt++;
        dly = DEF_YES;                                          // ... dly retry.
        err_rtn = local_err;
        break;

      default:
        done = DEF_YES;
        err_rtn = local_err;
        break;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(err_rtn));

  return (link_state);
}

/****************************************************************************************************//**
 *                                       NetIF_LinkStateSubscribe()
 *
 * @brief    Subscribe to get notified when an interface link state changes.
 *
 * @param    if_nbr  Network interface number to check link state.
 *
 * @param    fcnt    Function to call when the link changes.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
void NetIF_LinkStateSubscribe(NET_IF_NBR                  if_nbr,
                              NET_IF_LINK_SUBSCRIBER_FNCT fcnt,
                              RTOS_ERR                    *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((fcnt != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_LinkStateSubscribe);      // See Note #1b.

  NetIF_LinkStateSubscribeHandler(if_nbr, fcnt, p_err);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                       NetIF_LinkStateUnsubscribe()
 *
 * @brief    Unsubscribe to get notified when interface link state changes.
 *
 * @param    if_nbr  Network interface number to check link state.
 *
 * @param    fcnt    Function to call when the link changes.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_POOL_FULL
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @internal
 *******************************************************************************************************/
void NetIF_LinkStateUnsubscribe(NET_IF_NBR                  if_nbr,
                                NET_IF_LINK_SUBSCRIBER_FNCT fcnt,
                                RTOS_ERR                    *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_LinkStateGet);            // See Note #1b.

  NetIF_LinkStateUnSubscribeHandler(if_nbr, fcnt, p_err);

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                               NetIF_IO_Ctrl()
 *
 * @brief    Handle network interface &/or device specific (I/O) control(s) :
 *               - (a) Device link :
 *                   - (1) Get    device link info
 *                   - (2) Get    device link state
 *                   - (3) Update device link state
 *
 * @param    if_nbr  Network interface number to handle (I/O) controls.
 *
 * @param    opt     Desired I/O control option code to perform; additional control options may be
 *                   defined by the device driver :
 *                        - NET_IF_IO_CTRL_LINK_STATE_GET       Get    device's current  physical link state,
 *                                                                     'UP' or 'DOWN'.
 *                        - NET_IF_IO_CTRL_LINK_STATE_GET_INFO  Get    device's detailed physical link state
 *                                                                     information.
 *                        - NET_IF_IO_CTRL_LINK_STATE_UPDATE    Update device's current  physical link state.
 *                        - NET_IF_IO_CTRL_EEE_GET_INFO         Retrieve information if EEE is enabled or not.
 *                        - NET_IF_IO_CTRL_EEE                  Enable/Disable EEE support on the interface.
 *
 * @param    p_data  Pointer to variable that will receive possible I/O control data.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @note     (2) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 *
 * @note     (3) 'p_data' MUST point to a variable or memory buffer that is sufficiently sized AND
 *               aligned to receive any return data. If the option is :
 *               - (a) NET_IF_IO_CTRL_LINK_STATE_GET:
 *                     (1) For Ethernet or Wireless interface: p_data MUST point to a CPU_BOOLEAN variable.
 *               - (b) NET_IF_IO_CTRL_LINK_STATE_GET_INFO
 *                     NET_IF_IO_CTRL_LINK_STATE_UPDATE
 *                   - (1) For an ethernet interface: p_data MUST point to a variable of data type
 *                         NET_DEV_LINK_ETHER.
 *                   - (2) For a  Wireless interface: p_data MUST point to a variable of data type
 *                         NET_DEV_LINK_WIFI.
 *******************************************************************************************************/
void NetIF_IO_Ctrl(NET_IF_NBR if_nbr,
                   CPU_INT08U opt,
                   void       *p_data,
                   RTOS_ERR   *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_data != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_IO_Ctrl);                 // See Note #2b.

  //                                                               ---------------- HANDLE NET IF I/O -----------------
  NetIF_IO_CtrlHandler(if_nbr, opt, p_data, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return;
}

/****************************************************************************************************//**
 *                                       NetIF_TxSuspendTimeoutSet()
 *
 * @brief    Set network interface transmit suspend timeout value.
 *
 * @param    if_nbr      Interface number to set timeout value.
 *
 * @param    timeout_ms  Timeout value (in milliseconds).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *
 * @internal
 * @note     (1) [INTERNAL] 'NetIF_TxSuspendTimeout_tick' variables MUST ALWAYS be accessed exclusively
 *               in critical sections.
 * @endinternal
 *******************************************************************************************************/

#ifdef  NET_LOAD_BAL_MODULE_EN
void NetIF_TxSuspendTimeoutSet(NET_IF_NBR if_nbr,
                               CPU_INT32U timeout_ms,
                               RTOS_ERR   *p_err)
{
  NET_IF *p_if = DEF_NULL;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  p_if->TxSuspendTimeout_ms = timeout_ms;                       // Set transmit suspend timeout value (in OS ticks).
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                       NetIF_TxSuspendTimeoutGet_ms()
 *
 * @brief    Get network interface transmit suspend timeout value.
 *
 * @param    if_nbr  Interface number to get timeout value.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   Network transmit suspend timeout value (in milliseconds), if NO error(s).
 *           0, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] 'NetIF_TxSuspendTimeout_tick' variables MUST ALWAYS be accessed exclusively
 *               in critical sections.
 * @endinternal
 *******************************************************************************************************/

#ifdef  NET_LOAD_BAL_MODULE_EN
CPU_INT32U NetIF_TxSuspendTimeoutGet_ms(NET_IF_NBR if_nbr,
                                        RTOS_ERR   *p_err)
{
  NET_IF     *p_if = DEF_NULL;
  CPU_INT32U timeout_ms = 0u;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, timeout_ms);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  CORE_ENTER_ATOMIC();
  timeout_ms = p_if->TxSuspendTimeout_ms;                       // Get transmit suspend timeout value (in OS ticks).
  CORE_EXIT_ATOMIC();

exit:
  return (timeout_ms);
}
#endif

/****************************************************************************************************//**
 *                                           NetIF_ISR_Handler()
 *
 * @brief    Handle network interface's device interrupt service routine (ISR) function(s).
 *
 * @param    if_nbr  Network interface number  to handle ISR(s).
 *
 * @param    type    Device  interrupt type(s) to handle :
 *                       - NET_DEV_ISR_TYPE_UNKNOWN        Handle unknown device ISR(s).
 *                       - NET_DEV_ISR_TYPE_RX             Handle device receive ISR(s).
 *                       - NET_DEV_ISR_TYPE_RX_OVERRUN     Handle device receive overrun ISR(s).
 *                       - NET_DEV_ISR_TYPE_TX_RDY         Handle device transmit ready ISR(s).
 *                       - NET_DEV_ISR_TYPE_TX_COMPLETE    Handle device transmit complete ISR(s).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_INVALID_STATE
 *
 * @note     (1) Device driver(s)' Board Support Package (BSP) Interrupt Service Routine (ISR) handler(s).
 *                   This function is a network interface (IF) to network device function & SHOULD be called
 *                   only by appropriate network device driver ISR handler function(s) [see also Note #1].
 *
 * @internal
 * @note     (2) [INTERNAL] This function is called by device driver function(s) & :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST NOT block by pending on & acquiring the global network lock.
 *                   - (1) Although blocking on the global network lock is typically required since any
 *                         external API function access is asynchronous to other network protocol tasks;
 *                         interrupt service routines (ISRs) are (typically) prohibited from pending on
 *                         OS objects & therefore can NOT acquire the global network lock.
 *                   - (2) Therefore, ALL network interface & network device driver functions that may
 *                         be called by interrupt service routines MUST be able to be asynchronously
 *                         accessed without acquiring the global network lock AND without corrupting
 *                         any network data or task.
 *               - (c) Although blocking on the global network lock is typically required to verify
 *                     that network protocol suite initialization is complete; interrupt service routines
 *                     (ISRs) are (typically) prohibited from pending on OS objects & therefore can NOT
 *                     acquire the global network lock.
 *               - (d) Therefore, since network protocol suite initialization complete MUST be able to
 *                     be verified from interrupt service routines without acquiring the global network
 *                     lock; 'Net_InitDone' MUST be accessed exclusively in critical sections during
 *                     initialization & from asynchronous interrupt service routines.
 *
 * @note     (3) [INTERNAL] Network device interrupt service routines (ISR) handler(s) SHOULD be able to
 *               correctly function regardless of whether their corresponding network interface(s) are
 *               enabled.
 * @endinternal
 *******************************************************************************************************/
void NetIF_ISR_Handler(NET_IF_NBR       if_nbr,
                       NET_DEV_ISR_TYPE type,
                       RTOS_ERR         *p_err)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);                              // See Note #1b2.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

#if 0                                                           // See Note #3.
  if (p_if->En != DEF_ENABLED) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto exit;
  }
#endif

  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               --------------- HANDLE NET IF ISR(s) ---------------
  p_if_api->ISR_Handler(p_if, type, p_err);                     // See Note #1b2.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIF_Init()
 *
 * @brief    (1) Initialize the Network  Interface Management Module :
 *               - (a) Perform    Network Interface/OS initialization
 *               - (b) Initialize Network Interface table
 *               - (c) Initialize Network Interface counter(s)
 *               - (d) Initialize Network Interface transmit list pointers
 *               - (e) Initialize Network Interface timers
 *               - (f) Initialize Network Interface(s)/Layer(s) :
 *                   - (1) (A) Initialize          Loopback Interface
 *                       - (B) Initialize specific network  interface(s)
 *                   - (2) Initialize ARP Layer
 *
 * @param    p_mem_seg   Pointer to memory segment for allocation.
 *
 *
 * Argument(s) : p_mem_seg   Pointer to memory segment for allocation.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) Regardless of whether the Loopback interface is enabled or disabled, network interface
 *                   numbers MUST be initialized to reserve the lowest possible network interface number
 *                   ('NET_IF_NBR_LOOPBACK') for the Loopback interface.
 *
 *                           NetIF_NbrLoopback
 *                           NetIF_NbrBaseCfgd
 *******************************************************************************************************/
void NetIF_Init(MEM_SEG  *p_mem_seg,
                RTOS_ERR *p_err)
{
  NET_IF     *p_if;
  NET_IF_NBR if_nbr;
  CPU_INT32U timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ---------- ALLOCATE SEGMENT FOR IF MODULE ----------
  NetIF_DataPtr = (NET_IF_DATA *)Mem_SegAlloc("IF Module Data",
                                              p_mem_seg,
                                              sizeof(NET_IF_DATA),
                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------ CFG NET IF INIT DFLT VALS -------------
#if 1
  (void)NetIF_CfgPhyLinkPeriod(NET_IF_PHY_LINK_TIME_DFLT_MS);
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  (void)NetIF_CfgPerfMonPeriod(NET_IF_PERF_MON_TIME_DFLT_MS);
#endif
#endif

  //                                                               ---------- ALLOCATE SEGMENT FOR IF TABLE -----------
  NetIF_DataPtr->IF_Tbl = (NET_IF *)Mem_SegAlloc("IF Table",
                                                 p_mem_seg,
                                                 sizeof (NET_IF) * Net_CoreDataPtr->IF_NbrTot,
                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               -------------- INITIALIZE EVENT LISTS --------------
  SList_Init(&NetIF_DataPtr->ListEventRxHead);
  SList_Init(&NetIF_DataPtr->ListFreeEventRxHead);
  SList_Init(&NetIF_DataPtr->ListEventTxHead);
  SList_Init(&NetIF_DataPtr->ListFreeEventTxHead);
  NetIF_DataPtr->ListEventRxTail = DEF_NULL;
  NetIF_DataPtr->ListEventTxTail = DEF_NULL;

  //                                                               ------------------- INIT IF TBL --------------------
  for (if_nbr = 0u; if_nbr < Net_CoreDataPtr->IF_NbrTot; if_nbr++) {
    p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];
    p_if->Nbr = NET_IF_NBR_NONE;
    p_if->Type = NET_IF_TYPE_NONE;
    p_if->Init = DEF_NO;
    p_if->En = DEF_DISABLED;
    p_if->Link = NET_IF_LINK_DOWN;
    p_if->MTU = 0u;
    p_if->IF_API = DEF_NULL;
    p_if->IF_Data = DEF_NULL;
    p_if->Dev_API = DEF_NULL;
    p_if->Dev_Cfg = DEF_NULL;
    p_if->Dev_Data = DEF_NULL;
    p_if->Ext_API = DEF_NULL;
    p_if->Ext_Cfg = DEF_NULL;

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
    p_if->PerfMonState = NET_IF_PERF_MON_STATE_STOP;
    p_if->PerfMonTS_Prev_ms = NET_TS_NONE;
#endif

#ifdef  NET_LOAD_BAL_MODULE_EN
    NetStat_CtrInit(&p_if->RxPktCtr);
    NetStat_CtrInit(&p_if->TxSuspendCtr);
#endif
  }
  //                                                               Init base/next IF nbrs (see Note #3).
#ifdef NET_IF_LOOPBACK_MODULE_EN
  NetIF_DataPtr->NbrBase = NET_IF_NBR_BASE;
  NetIF_DataPtr->NbrNext = NET_IF_NBR_LOOPBACK;
#else
  NetIF_DataPtr->NbrBase = NET_IF_NBR_BASE_CFGD;
  NetIF_DataPtr->NbrNext = NET_IF_NBR_BASE_CFGD;
#endif

  //                                                               ------------ INIT NET IF CTR's -------------
  NetStat_CtrInit(&NetIF_DataPtr->RxTaskPktCtr);

  //                                                               ----------- INIT NET IF TX LIST ------------
  NetIF_DataPtr->TxListHead = DEF_NULL;
  NetIF_DataPtr->TxListTail = DEF_NULL;

  //                                                               ------------ INIT NET IF TMR's -------------
  CORE_ENTER_ATOMIC();
  timeout_ms = NetIF_DataPtr->PhyLinkStateTime_ms;
  CORE_EXIT_ATOMIC();

#ifndef  NET_CFG_LINK_STATE_POLL_DISABLED
  NetIF_DataPtr->PhyLinkStateTmr = NetTmr_Get(NetIF_PhyLinkStateHandler,
                                              0,
                                              timeout_ms,
                                              NET_TMR_OPT_PERIODIC,
                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  timeout_ms = NetIF_DataPtr->PerfMonTime_ms;
  CORE_EXIT_ATOMIC();
  NetIF_DataPtr->PerfMonTmr = NetTmr_Get(NetIF_PerfMonHandler,
                                         0,
                                         timeout_ms,
                                         NET_TMR_OPT_PERIODIC,
                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

  //                                                               -------------- INIT NET IF(s) --------------
  //                                                               See Note #2b.
#ifdef  NET_IF_LOOPBACK_MODULE_EN
  NetIF_Loopback_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

#ifdef  NET_IF_ETHER_MODULE_EN
  NetIF_Ether_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

#ifdef  NET_IF_WIFI_MODULE_EN
  NetIF_WiFi_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

#ifdef  NET_ARP_MODULE_EN
  NetARP_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

  PP_UNUSED_PARAM(p_mem_seg);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_StartInternal()
 *
 * @brief    Start network interface with given setup structure.
 *
 * @param    if_nbr  Network interface number.
 *
 * @param    p_cfg   Pointer to configuration structure for the interface setup.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetIF_StartInternal(NET_IF_NBR if_nbr,
                         NET_IF_CFG *p_cfg,
                         RTOS_ERR   *p_err)
{
  NET_IF            *p_if;
  NET_IF_START_OBJ  *p_if_start_obj;
  NET_IF_LINK_STATE link_state = NET_IF_LINK_DOWN;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------- SET HW ADDRESS FROM APP IF AVAILABLE -------
  if (p_cfg != DEF_NULL) {
    if (p_cfg->HW_AddrStr != DEF_NULL) {
      CPU_INT08U hw_addr[NET_IF_802x_ADDR_SIZE];

      NetASCII_Str_to_MAC((CPU_CHAR *)p_cfg->HW_AddrStr, &hw_addr[0], p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      NetIF_AddrHW_Set(if_nbr, &hw_addr[0], sizeof(hw_addr), p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    }
  }

  //                                                               ----------- INTERFACE START OBJECT INIT ------------
  Net_GlobalLockAcquire((void *)NetIF_Start);

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    Net_GlobalLockRelease();
    return;
  }

  p_if_start_obj = p_if->StartObj;

  if (p_cfg != DEF_NULL) {
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
    //                                                             --- CREATE/SET SEMAPHORE FOR SETUP WAIT FEATURE ----
    if (KAL_SEM_HANDLE_IS_NULL(p_if->StartObj->SemHandle) == DEF_YES) {
      p_if_start_obj->SemHandle = KAL_SemCreate("Start IF Sem", DEF_NULL, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        Net_GlobalLockRelease();
        return;
      }
    } else {
      KAL_SemPendAbort(p_if_start_obj->SemHandle, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        Net_GlobalLockRelease();
        return;
      }
      KAL_SemSet(p_if_start_obj->SemHandle, 0, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        Net_GlobalLockRelease();
        return;
      }
    }

    //                                                             ----- INIT MESSAGE LIST FOR SETUP WAIT FEATURE -----
    SList_Init(&p_if_start_obj->MsgListPtr);

    //                                                             ------ SET READY FLAGS FOR SETUP WAIT FEATURE ------
    //                                                             Link UP ready flag setup.
    link_state = NetIF_LinkStateGetHandler(if_nbr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    if (link_state == NET_IF_LINK_UP) {
      p_if_start_obj->RdyFlags.IF_LinkUpRdy = DEF_YES;
    } else {
      p_if_start_obj->RdyFlags.IF_LinkUpRdy = DEF_NO;

      NetIF_LinkStateSubscribeHandler(if_nbr, NetIF_WaitSetupLinkUpHook, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        Net_GlobalLockRelease();
        return;
      }
    }

    //                                                             DHCP client ready flag setup.
#ifdef NET_DHCP_CLIENT_MODULE_EN
    if (p_cfg->IPv4.DHCPc.En == DEF_YES) {
      p_if_start_obj->RdyFlags.DHCPc_Rdy = DEF_NO;
    } else {
      p_if_start_obj->RdyFlags.DHCPc_Rdy = DEF_YES;
    }
#else
    p_if_start_obj->RdyFlags.DHCPc_Rdy = DEF_YES;
#endif

    //                                                             IPv4 Link Local ready flag setup.
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
    if ((p_cfg->IPv4.LinkLocal.En == DEF_YES)
        && (p_cfg->IPv4.Static.Addr == DEF_NULL)) {
      p_if_start_obj->RdyFlags.IPv4_LinkLocalRdy = DEF_NO;
    } else {
      p_if_start_obj->RdyFlags.IPv4_LinkLocalRdy = DEF_YES;
    }
#else
    p_if_start_obj->RdyFlags.IPv4_LinkLocalRdy = DEF_YES;
#endif

    //                                                             IPv6 Static address ready flag setup.
#ifdef NET_IPv6_MODULE_EN
    if (p_cfg->IPv6.Static.Addr != DEF_NULL) {
      p_if_start_obj->RdyFlags.IPv6_StaticRdy = DEF_NO;
    } else {
      p_if_start_obj->RdyFlags.IPv6_StaticRdy = DEF_YES;
    }
#else
    p_if_start_obj->RdyFlags.IPv6_StaticRdy = DEF_YES;
#endif

    //                                                             IPv6 Auto-Cfg addresses ready flags setup.
#ifdef NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
    if (p_cfg->IPv6.AutoCfg.En == DEF_YES) {
      p_if_start_obj->RdyFlags.IPv6_LinkLocalRdy = DEF_NO;
      p_if_start_obj->RdyFlags.IPv6_GlobalRdy = DEF_NO;
    } else {
      p_if_start_obj->RdyFlags.IPv6_LinkLocalRdy = DEF_YES;
      p_if_start_obj->RdyFlags.IPv6_GlobalRdy = DEF_YES;
    }
#else
    p_if_start_obj->RdyFlags.IPv6_LinkLocalRdy = DEF_YES;
    p_if_start_obj->RdyFlags.IPv6_GlobalRdy = DEF_YES;
#endif
#endif

    //                                                             ------------- SAVE CALLBACK FUNCTIONS --------------
    if (p_cfg->IPv6.Hook != DEF_NULL) {
      p_if_start_obj->IPv6_Hook = p_cfg->IPv6.Hook;
    }

    if (p_cfg->IPv4.LinkLocal.OnCompleteHook != DEF_NULL) {
      p_if_start_obj->IPv4_LinkLocalHook = p_cfg->IPv4.LinkLocal.OnCompleteHook;
    }

    if (p_cfg->IPv4.DHCPc.OnCompleteHook != DEF_NULL) {
      p_if_start_obj->DHCPc_Hook = p_cfg->IPv4.DHCPc.OnCompleteHook;
    }

    if ((p_cfg->IPv4.LinkLocal.En == DEF_YES)
        && (p_cfg->IPv4.Static.Addr == DEF_NULL)) {
      p_if_start_obj->IPv4_LinkLocalEn = DEF_YES;
    } else {
      p_if_start_obj->IPv4_LinkLocalEn = DEF_NO;
    }
  } else {
 #if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
    //                                                             --------- SET FLAGS TO READY IF NO CONFIG ----------
    p_if_start_obj->RdyFlags.IF_LinkUpRdy = DEF_YES;
    p_if_start_obj->RdyFlags.DHCPc_Rdy = DEF_YES;
    p_if_start_obj->RdyFlags.IPv4_LinkLocalRdy = DEF_YES;
    p_if_start_obj->RdyFlags.IPv6_StaticRdy = DEF_YES;
    p_if_start_obj->RdyFlags.IPv6_LinkLocalRdy = DEF_YES;
    p_if_start_obj->RdyFlags.IPv6_GlobalRdy = DEF_YES;
#endif
  }

  Net_GlobalLockRelease();

  //                                                               -------- LOW LEVELS NETWORK INTERFACE START --------
  NetIF_Start(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_cfg != DEF_NULL) {
    //                                                             ------- CONFIGURE IPV4 ADDRESS IF AVAILABLE --------
    if (p_cfg->IPv4.Static.Addr != DEF_NULL) {
#ifdef NET_IPv4_MODULE_EN
      NET_IPv4_ADDR addr;
      NET_IPv4_ADDR mask;
      NET_IPv4_ADDR gateway;

      addr = NetASCII_Str_to_IPv4((CPU_CHAR *)p_cfg->IPv4.Static.Addr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      mask = NetASCII_Str_to_IPv4((CPU_CHAR *)p_cfg->IPv4.Static.Mask, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      if (p_cfg->IPv4.Static.Gateway != DEF_NULL) {
        gateway = NetASCII_Str_to_IPv4((CPU_CHAR *)p_cfg->IPv4.Static.Gateway, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          gateway = NET_IPv4_ADDR_NONE;
        }
      } else {
        gateway = NET_IPv4_ADDR_NONE;
      }

      (void)NetIPv4_CfgAddrAdd(if_nbr, addr, mask, gateway, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
    } else {
      //                                                           -- START IPV4 LINK LOCAL IF CONDITIONS ARE FILLED --
      if ((p_cfg->IPv4.DHCPc.En == DEF_NO)
          && (p_cfg->IPv4.LinkLocal.En == DEF_YES)) {
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
        NetIPv4_AddrLinkLocalCfg(if_nbr, NetIF_IPv4_LinkLocalCompleteHook, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
          p_if_start_obj->RdyFlags.IPv4_LinkLocalRdy = DEF_YES;
#endif
          return;
        }
#else
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
      }
    }

    //                                                             ---------- START DHCP CLIENT IF AVAILABLE ----------

    if (p_cfg->IPv4.DHCPc.En == DEF_YES) {
#ifdef NET_DHCP_CLIENT_MODULE_EN

      DHCPc_IF_Add(if_nbr, &p_cfg->IPv4.DHCPc.Cfg, NetIF_DHCPc_CompleteHook, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
        p_if_start_obj->RdyFlags.DHCPc_Rdy = DEF_YES;
#endif
        return;
      }
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
    }

    //                                                             ----- START IPV6 ADDR CONFIGURATIONS IF AVAIL ------
    if ((p_cfg->IPv6.Static.Addr != DEF_NULL)
        || (p_cfg->IPv6.AutoCfg.En == DEF_YES)) {
#ifdef NET_IPv6_MODULE_EN
      NetIPv6_AddrSubscribe(NetIF_IPv6_AddrHook, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
        p_if_start_obj->RdyFlags.IPv6_StaticRdy = DEF_YES;
        p_if_start_obj->RdyFlags.IPv6_LinkLocalRdy = DEF_YES;
        p_if_start_obj->RdyFlags.IPv6_GlobalRdy = DEF_YES;
#endif
        return;
      }
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
    }

    if (p_cfg->IPv6.Static.Addr != DEF_NULL) {
#ifdef NET_IPv6_MODULE_EN
      NET_IPv6_ADDR addr;
      NET_FLAGS     flags = NET_FLAG_NONE;

      addr = NetASCII_Str_to_IPv6((CPU_CHAR *)p_cfg->IPv6.Static.Addr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
        p_if_start_obj->RdyFlags.IPv6_StaticRdy = DEF_YES;
#endif
        return;
      }

      if (p_cfg->IPv6.Static.DAD_En == DEF_YES) {
        DEF_BIT_SET(flags, NET_IPv6_FLAG_DAD_EN);
      }

      (void)NetIPv6_CfgAddrAdd(if_nbr,
                               &addr,
                               p_cfg->IPv6.Static.PrefixLen,
                               flags,
                               p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
        p_if_start_obj->RdyFlags.IPv6_StaticRdy = DEF_YES;
#endif
        return;
      }
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
    }

    if (p_cfg->IPv6.AutoCfg.En == DEF_YES) {
#ifdef NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
      NetIPv6_AddrAutoCfgEnHandler(if_nbr, p_cfg->IPv6.AutoCfg.DAD_En, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
        p_if_start_obj->RdyFlags.IPv6_LinkLocalRdy = DEF_YES;
        p_if_start_obj->RdyFlags.IPv6_GlobalRdy = DEF_YES;
#endif
        return;
      }
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
    }
  }

  PP_UNUSED_PARAM(link_state);
}

/****************************************************************************************************//**
 *                                           NetIF_BufPoolInit()
 *
 * @brief    (1) Create network interface buffer memory pools :
 *               - (a) Validate network interface buffer configuration
 *               - (b) Create   network buffer memory pools :
 *                   - (1) Create receive  large buffer pool
 *                   - (2) Create transmit large buffer pool
 *                   - (3) Create transmit small buffer pool
 *                   - (4) Create network buffer header pool
 *
 * @param    p_if    Pointer to network interface.
 *
 *
 * Argument(s) : p_if        Pointer to network interface.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) Each added network interfaces MUST decrement its total number of configured receive
 *                   buffers from the remaining network interface receive queue configured size.
 *
 *                   Each added network interfaces MUST decrement its total number of configured transmit
 *                   buffers from the remaining network interface transmit deallocation queue configured
 *                   size.
 *
 *                   (a) However, since the network loopback interface does NOT deallocate transmit
 *                       packets via the network interface transmit deallocation task (see
 *                       'net_if_loopback.c  NetIF_Loopback_Tx()  Note #4'); then the network interface
 *                       transmit deallocation queue size does NOT need to be adjusted by the network
 *                       loopback interface's number of configured transmit buffers.
 *
 *               (3) Each network buffer data area allocates additional octets for its configured offset
 *                   (see 'net_dev_cfg.c  EXAMPLE NETWORK INTERFACE / DEVICE CONFIGURATION  Note #5').  This
 *                   ensures that each data area's effective, useable size still equals its configured size
 *                   (see 'net_dev_cfg.c  EXAMPLE NETWORK INTERFACE / DEVICE CONFIGURATION  Note #2').
 *******************************************************************************************************/
void NetIF_BufPoolInit(NET_IF   *p_if,
                       RTOS_ERR *p_err)
{
  NET_IF_NBR  if_nbr;
  NET_IF_API  *p_if_api;
  NET_DEV_CFG *p_dev_cfg;
  void        *p_addr;
  CPU_ADDR    size;
  CPU_SIZE_T  size_buf;
  NET_BUF_QTY nbr_bufs_tot;

  //                                                               ------------- VALIDATE NET IF BUF CFG --------------
  if_nbr = (NET_IF_NBR)p_if->Nbr;
  p_if_api = (NET_IF_API *)p_if->IF_API;
  p_dev_cfg = (NET_DEV_CFG *)p_if->Dev_Cfg;

  NetBuf_PoolCfgValidate(p_if->Type, p_dev_cfg);

  NetIF_BufPoolCfgValidate(if_nbr, p_dev_cfg);

  p_if_api->BufPoolCfgValidate(p_if, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               --------- INIT NET IF RX BUF POOL ----------
  if (p_dev_cfg->RxBufPoolType == NET_IF_MEM_TYPE_MAIN) {
    p_addr = DEF_NULL;
    size = 0u;
  } else {
    p_addr = (void *)p_dev_cfg->MemAddr;
    size = (CPU_ADDR)p_dev_cfg->MemSize;
  }

  if (p_dev_cfg->RxBufLargeNbr > 0) {
    size_buf = p_dev_cfg->RxBufLargeSize                        // Inc cfg'd rx buf size by ...
               + p_dev_cfg->RxBufIxOffset;                      // ... cfg'd rx buf offset (see Note #3).
    NetBuf_PoolInit(if_nbr,
                    NET_BUF_TYPE_RX_LARGE,                      // Create  large rx buf data area pool.
                    "Rx_large_buf_pool",                        // Name of the pool.
                    p_addr,                                     // Create  pool  in dedicated mem, if avail.
                    size,                                       // Size  of dedicated mem, if avail.
                    p_dev_cfg->RxBufLargeNbr,                   // Nbr  of large rx bufs to create.
                    size_buf,                                   // Size of large rx bufs to create.
                    p_dev_cfg->RxBufAlignOctets,                // Align   large rx bufs to octet boundary.
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  //                                                               ------------- INIT NET IF TX BUF POOLS -------------
  if (p_dev_cfg->TxBufPoolType == NET_IF_MEM_TYPE_MAIN) {
    p_addr = DEF_NULL;
    size = 0u;
  } else {
    p_addr = (void *)p_dev_cfg->MemAddr;
    size = (CPU_ADDR)p_dev_cfg->MemSize;
  }

  if (p_dev_cfg->TxBufLargeNbr > 0) {
    size_buf = p_dev_cfg->TxBufLargeSize                        // Inc cfg'd tx buf size by ...
               + p_dev_cfg->TxBufIxOffset;                      // ... cfg'd tx buf offset (see Note #3).
    NetBuf_PoolInit(if_nbr,
                    NET_BUF_TYPE_TX_LARGE,                      // Create  large tx buf data area pool.
                    "Tx_large_buf_pool",                        // Name of the pool.
                    p_addr,                                     // Create  pool  in dedicated mem, if avail.
                    size,                                       // Size  of dedicated mem, if avail.
                    p_dev_cfg->TxBufLargeNbr,                   // Nbr  of large tx bufs to create.
                    size_buf,                                   // Size of large tx bufs to create.
                    p_dev_cfg->TxBufAlignOctets,                // Align   large tx bufs to octet boundary.
                    p_err);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  if (p_dev_cfg->TxBufSmallNbr > 0) {
    size_buf = p_dev_cfg->TxBufSmallSize                        // Inc cfg'd tx buf size by ...
               + p_dev_cfg->TxBufIxOffset;                      // ... cfg'd tx buf offset (see Note #3).
    NetBuf_PoolInit(if_nbr,
                    NET_BUF_TYPE_TX_SMALL,                      // Create  small tx buf data area pool.
                    "Tx_small_buf_pool",                        // Name of the pool.
                    p_addr,                                     // Create  pool  in dedicated mem, if avail.
                    size,                                       // Size  of dedicated mem, if avail.
                    p_dev_cfg->TxBufSmallNbr,                   // Nbr  of small tx bufs to create.
                    size_buf,                                   // Size of small tx bufs to create.
                    p_dev_cfg->TxBufAlignOctets,                // Align   small tx bufs to octet boundary.
                    p_err);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  //                                                               --------------- INIT NET IF BUF POOL ---------------
  //                                                               Calc IF's tot nbr net bufs.
  nbr_bufs_tot = p_dev_cfg->RxBufLargeNbr
                 + p_dev_cfg->TxBufLargeNbr
                 + p_dev_cfg->TxBufSmallNbr;

  if (nbr_bufs_tot > 0) {
    NetBuf_PoolInit(if_nbr,
                    NET_BUF_TYPE_BUF,                           // Create  net buf pool.
                    "Net_buf_pool",                             // Name of the pool.
                    0,                                          // Create  net bufs from main mem heap.
                    0u,
                    nbr_bufs_tot,                               // Nbr  of net bufs to create.
                    sizeof(NET_BUF),                            // Size of net bufs.
                    sizeof(CPU_DATA),                           // Align   net bufs to CPU data word size.
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIF_Get()
 *
 * @brief    Get a pointer to a network interface.
 *
 * @param    if_nbr  Network interface number to get.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to corresponding network interface, if NO error(s).
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) NetIF_Get() is called by network protocol suite function(s) & SHOULD be called
 *               with the global network lock already acquired.
 *           - (a) However, although acquiring the global network lock is typically required;
 *                 interrupt service routines (ISRs) are (typically) prohibited from pending
 *                 on OS objects & therefore can NOT acquire the global network lock.
 *           - (b) Therefore, ALL network interface & network device driver functions that may
 *                 be called by interrupt service routines MUST be able to be asynchronously
 *                 accessed without acquiring the global network lock AND without corrupting
 *                 any network data or task.
 *******************************************************************************************************/
NET_IF *NetIF_Get(NET_IF_NBR if_nbr,
                  RTOS_ERR   *p_err)
{
  NET_IF *p_if = DEF_NULL;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_valid = DEF_NO;
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  //                                                               --------------- VALIDATE NET IF NBR ----------------
  if (if_nbr == NET_IF_NBR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }
#endif

  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];

exit:
  return (p_if);
}

/****************************************************************************************************//**
 *                                               NetIF_GetDflt()
 *
 * @brief    Get the interface number of the default network interface; i.e. the first enabled network
 *           interface with at least one configured host address.
 *
 * @return   Interface number of the first enabled network interface, if any.
 *           NET_IF_NBR_NONE, otherwise.
 *
 * @note     (1) NetIF_GetDflt() is called by network protocol suite function(s) & MUST be called with
 *               the global network lock already acquired.
 *******************************************************************************************************/
NET_IF_NBR NetIF_GetDflt(void)
{
  NET_IF      *p_if;
  NET_IF_NBR  if_nbr;
  NET_IF_NBR  if_nbr_ix;
  NET_IF_NBR  if_nbr_next;
  CPU_BOOLEAN init;
  CPU_BOOLEAN addr_avail;
  RTOS_ERR    local_err;
  CORE_DECLARE_IRQ_STATE;

  if_nbr = NET_IF_NBR_NONE;
  if_nbr_ix = NET_IF_NBR_BASE_CFGD;
  CORE_ENTER_ATOMIC();
  if_nbr_next = NetIF_DataPtr->NbrNext;
  CORE_EXIT_ATOMIC();

  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr_ix];

  while ((if_nbr_ix < if_nbr_next)                              // Srch ALL cfg'd IF's            ...
         && (if_nbr == NET_IF_NBR_NONE)) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    CORE_ENTER_ATOMIC();
    init = p_if->Init;
    CORE_EXIT_ATOMIC();
    if ((init == DEF_YES)                                       // ... for first init'd & en'd IF ...
        && (p_if->En == DEF_ENABLED)) {
      addr_avail = NetIP_IsAddrsCfgdOnIF(if_nbr_ix, &local_err);
      if (addr_avail == DEF_YES) {                              // ... with cfg'd host addr(s) avail.
        if_nbr = p_if->Nbr;
      }
#if 1                                                           // !!!! Req'd temp'rly for sock datagram wildcard.
      if_nbr = p_if->Nbr;
#endif
    }

    if (if_nbr == NET_IF_NBR_NONE) {                            // If NO cfg'd host addr(s) found, ...
      p_if++;                                                   // ... adv to next IF.
      if_nbr_ix++;
    }
  }

#ifdef NET_IF_LOOPBACK_MODULE_EN
  if (if_nbr == NET_IF_NBR_NONE) {                              // If NO init'd, en'd, & cfg'd IF found; ...
    if_nbr = NET_IF_NBR_LOOPBACK;                               // ... rtn loopback IF, if en'd/avail.
  }
#endif

  return (if_nbr);
}

/****************************************************************************************************//**
 *                                           NetIF_IsValidHandler()
 *
 * @brief    Validate network interface number.
 *
 * @param    if_nbr  Network interface number to validate.
 *
 * @return   DEF_YES, network interface number   valid.
 *           DEF_NO,  network interface number invalid / NOT yet configured.
 *
 * @note     (1) NetIF_IsValidHandler() is called by network protocol suite function(s) & SHOULD
 *               be called with the global network lock already acquired.
 *               @n
 *               However, although acquiring the global network lock is typically required;
 *               interrupt service routines (ISRs) are (typically) prohibited from pending
 *               on OS objects & therefore can NOT acquire the global network lock.
 *               @n
 *               Therefore, ALL network interface & network device driver functions that may
 *               be called by interrupt service routines MUST be able to be asynchronously
 *               accessed without acquiring the global network lock AND without corrupting
 *               any network data or task.
 *               @n
 *               Thus the following variables MUST ALWAYS be accessed exclusively in critical
 *               sections :
 *               @n
 *               -   'NetIF_DataPtr->NbrNext'
 *               -   Network interfaces 'Init'
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_IsValidHandler(NET_IF_NBR if_nbr)
{
  NET_IF      *p_if;
  NET_IF_NBR  if_nbr_next;
  CPU_BOOLEAN init;
  CPU_BOOLEAN rtn_val = DEF_NO;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- VALIDATE NET IF NBR ----------------
  if (if_nbr == NET_IF_NBR_NONE) {
    goto exit;
  }

  CORE_ENTER_ATOMIC();
  if_nbr_next = NetIF_DataPtr->NbrNext;
  CORE_EXIT_ATOMIC();
  if (if_nbr >= if_nbr_next) {
    RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_ARG, DEF_NO);
  }

  //                                                               ----------------- VALIDATE NET IF ------------------
  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];
  CORE_ENTER_ATOMIC();
  init = p_if->Init;
  CORE_EXIT_ATOMIC();
  if (init != DEF_YES) {
    goto exit;
  }

  rtn_val = DEF_YES;

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                       NetIF_IsValidCfgdHandler()
 *
 * @brief    Validate configured network interface number.
 *
 * @param    if_nbr  Network interface number to validate.
 *
 * @return   DEF_YES, network interface number   valid.
 *           DEF_NO,  network interface number invalid / NOT yet configured or reserved.
 *
 * @note     (1) NetIF_IsValidCfgdHandler() is called by network protocol suite function(s) & MUST be
 *               called with the global network lock already acquired.
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_IsValidCfgdHandler(NET_IF_NBR if_nbr)
{
  NET_IF      *p_if;
  NET_IF_NBR  if_nbr_next;
  CPU_BOOLEAN init;
  CPU_BOOLEAN rtn_val = DEF_NO;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- VALIDATE NET IF NBR ----------------
  if (if_nbr == NET_IF_NBR_NONE) {
    goto exit;
  }

  CORE_ENTER_ATOMIC();
  if_nbr_next = NetIF_DataPtr->NbrNext;
  CORE_EXIT_ATOMIC();
  if ((if_nbr < NET_IF_NBR_BASE_CFGD)
      || (if_nbr >= if_nbr_next)) {
    goto exit;
  }

  //                                                               ----------------- VALIDATE NET IF ------------------
  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];
  CORE_ENTER_ATOMIC();
  init = p_if->Init;
  CORE_EXIT_ATOMIC();
  if (init != DEF_YES) {
    goto exit;
  }

  rtn_val = DEF_YES;

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                           NetIF_IsEnHandler()
 *
 * @brief    Validate network interface as enabled.
 *
 * @param    if_nbr  Network interface number to validate.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, network interface   valid  &  enabled.
 *           DEF_NO,  network interface invalid or disabled.
 *
 * @note     (1) NetIF_IsEnHandler() is called by network protocol suite function(s) & MUST be called
 *               with the global network lock already acquired.
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_IsEnHandler(NET_IF_NBR if_nbr,
                              RTOS_ERR   *p_err)
{
  NET_IF      *p_if = DEF_NULL;
  CPU_BOOLEAN is_valid = DEF_NO;
  CPU_BOOLEAN en = DEF_NO;

  //                                                               --------------- VALIDATE NET IF NBR ----------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }

  //                                                               --------------- VALIDATE NET IF EN'D ---------------
  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];
  if (p_if->En == DEF_ENABLED) {
    en = DEF_YES;
  } else {
    en = DEF_NO;
  }

exit:
  return (en);
}

/****************************************************************************************************//**
 *                                           NetIF_IsEnCfgdHandler()
 *
 * @brief    Validate configured network interface as enabled.
 *
 * @param    if_nbr  Network interface number to validate.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, network interface   valid  &  enabled.
 *           DEF_NO,  network interface invalid or disabled.
 *
 * @note     (1) NetIF_IsEnCfgdHandler() is called by network protocol suite function(s) & MUST be
 *               called with the global network lock already acquired.
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_IsEnCfgdHandler(NET_IF_NBR if_nbr,
                                  RTOS_ERR   *p_err)
{
  NET_IF      *p_if = DEF_NULL;
  CPU_BOOLEAN en = DEF_NO;;
  CPU_BOOLEAN is_cfgd = DEF_NO;

  //                                                               --------------- VALIDATE NET IF NBR ----------------
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }

  //                                                               --------------- VALIDATE NET IF EN'D ---------------
  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];
  if (p_if->En == DEF_ENABLED) {
    en = DEF_YES;
  } else {
    en = DEF_NO;
  }

exit:
  return (en);
}

/****************************************************************************************************//**
 *                                           NetIF_AddrHW_GetHandler()
 *
 * @brief    Get network interface's hardware address.
 *
 * @param    if_nbr      Network interface number to get hardware address.
 *
 * @param    p_addr_hw   Pointer to variable that will receive the hardware address (see Note #2).
 *
 * @param    p_addr_len  Pointer to a variable to ... :
 *                           - (a) Pass the length of the address buffer pointed to by 'p_addr_hw'.
 *                           - (b) Return the actual size of the protocol address, if NO error(s);
 *                           - (c) Return 0,                                       otherwise.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) NetIF_AddrHW_GetHandler() is called by network protocol suite function(s) & MUST be
 *               called with the global network lock already acquired.
 *
 * @note     (2) The hardware address is returned in network-order; i.e. the pointer to the hardware
 *               address points to the highest-order octet.
 *
 * @note     (3) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
void NetIF_AddrHW_GetHandler(NET_IF_NBR if_nbr,
                             CPU_INT08U *p_addr_hw,
                             CPU_INT08U *p_addr_len,
                             RTOS_ERR   *p_err)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;
  CPU_INT08U addr_len;

  addr_len = *p_addr_len;
  *p_addr_len = 0u;                                             // Init len for err (see Note #3).

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               ---------------- GET NET IF HW ADDR ----------------
  p_if_api->AddrHW_Get(p_if, p_addr_hw, &addr_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  *p_addr_len = addr_len;

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_AddrHW_SetHandler()
 *
 * @brief    Set network interface's hardware address.
 *
 * @param    if_nbr      Network interface number to set hardware address.
 *
 * @param    p_addr_hw   Pointer to hardware address (see Note #2).
 *
 * @param    addr_len    Length  of hardware address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) NetIF_AddrHW_SetHandler() is called by network protocol suite function(s) & MUST
 *               be called with the global network lock already acquired.
 *
 * @note     (2) The hardware address MUST be in network-order; i.e. the pointer to the hardware
 *               address MUST point to the highest-order octet.
 *
 * @note     (3) The interface MUST be stopped BEFORE setting a new hardware address, which does
 *               NOT take effect until the interface is re-started.
 *******************************************************************************************************/
void NetIF_AddrHW_SetHandler(NET_IF_NBR if_nbr,
                             CPU_INT08U *p_addr_hw,
                             CPU_INT08U addr_len,
                             RTOS_ERR   *p_err)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (p_if->En != DEF_DISABLED) {                               // If net IF NOT dis'd (see Note #3), ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);               // ... rtn err.
    goto exit;
  }

  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               ---------------- SET NET IF HW ADDR ----------------
  p_if_api->AddrHW_Set(p_if, p_addr_hw, addr_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIF_AddrHW_IsValidHandler()
 *
 * @brief    Validate network interface's hardware address.
 *
 * @param    if_nbr      Interface number to validate the hardware address.
 *
 * @param    p_addr_hw   Pointer to an interface hardware address (see Note #2).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if hardware address valid.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) NetIF_AddrHW_IsValidHandler() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *
 * @note     (2) The hardware address MUST be in network-order; i.e. the pointer to the hardware
 *               address MUST point to the highest-order octet.
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_AddrHW_IsValidHandler(NET_IF_NBR if_nbr,
                                        CPU_INT08U *p_addr_hw,
                                        RTOS_ERR   *p_err)
{
  NET_IF      *p_if = DEF_NULL;
  NET_IF_API  *p_if_api = DEF_NULL;
  CPU_BOOLEAN valid = DEF_NO;

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               ------------- VALIDATE NET IF HW ADDR --------------
  valid = p_if_api->AddrHW_IsValid(p_if, p_addr_hw);

exit:
  return (valid);
}

/****************************************************************************************************//**
 *                                           NetIF_AddrMulticastAdd()
 *
 * @brief    Add a multicast address to a network interface.
 *
 * @param    if_nbr              Interface number to add a multicast address.
 *
 * @param    p_addr_protocol     Pointer to a multicast protocol address to add (see Note #1).
 *
 * @param    addr_protocol_len   Length of the protocol address, in octets.
 *
 * @param    addr_protocol_type  Protocol address type.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) The multicast protocol address MUST be in network-order.
 *******************************************************************************************************/
#ifdef  NET_MCAST_MODULE_EN
void NetIF_AddrMulticastAdd(NET_IF_NBR        if_nbr,
                            CPU_INT08U        *p_addr_protocol,
                            CPU_INT08U        addr_protocol_len,
                            NET_PROTOCOL_TYPE addr_protocol_type,
                            RTOS_ERR          *p_err)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               ----------- ADD MULTICAST ADDR TO NET IF -----------
  p_if_api->AddrMulticastAdd(p_if,
                             p_addr_protocol,
                             addr_protocol_len,
                             addr_protocol_type,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                       NetIF_AddrMulticastRemove()
 *
 * @brief    Remove a multicast address from a network interface.
 *
 * @param    if_nbr              Interface number to remove a multicast address.
 *
 * @param    p_addr_protocol     Pointer to a multicast protocol address to remove (see Note #1).
 *
 * @param    addr_protocol_len   Length of the protocol address, in octets.
 *
 * @param    addr_protocol_type  Protocol address type.
 *
 * @param    p_err               Error Pointer.
 *
 * @note     (1) The multicast protocol address MUST be in network-order.
 *******************************************************************************************************/

#ifdef  NET_MCAST_MODULE_EN
void NetIF_AddrMulticastRemove(NET_IF_NBR        if_nbr,
                               CPU_INT08U        *p_addr_protocol,
                               CPU_INT08U        addr_protocol_len,
                               NET_PROTOCOL_TYPE addr_protocol_type,
                               RTOS_ERR          *p_err)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               -------- REMOVE MULTICAST ADDR FROM NET IF ---------
  p_if_api->AddrMulticastRemove(p_if,
                                p_addr_protocol,
                                addr_protocol_len,
                                addr_protocol_type,
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                       NetIF_AddrMulticastProtocolToHW()
 *
 * @brief    Convert a multicast protocol address into a hardware address.
 *
 * @param    if_nbr              Interface number to convert address.
 *
 * @param    p_addr_protocol     Pointer to a multicast protocol address to convert (see Note #1a).
 *
 * @param    addr_protocol_len   Length of the protocol address, in octets.
 *
 * @param    addr_protocol_type  Protocol address type.
 *
 * @param    p_addr_hw           Pointer to a variable that will receive the hardware address
 *                               (see Note #1b).
 *
 * @param    p_addr_hw_len       Pointer to a variable to ... :
 *                                   - (a) Pass the length of the hardware address, in octets.
 *                                   - (b) Return the actual length of the hardware address,
 *                                         in octets, if NO error(s);
 *                                   - (c) Return 0, otherwise.
 *
 * @param    p_err               Error pointer.
 *
 * @note     (1) The multicast protocol address MUST be in network-order.
 *               The hardware address is returned in network-order; i.e. the pointer to the
 *               hardware address points to the highest-order octet.
 *
 * @note     (2) Since 'p_addr_hw_len' argument is both an input & output argument (see 'Argument(s) :
 *               p_addr_hw_len'), ... :
 *                   - (a) Its input value SHOULD be validated prior to use; ...
 *                       - (1) In the case that the 'p_addr_len' argument is passed a null pointer,
 *                             NO input value is validated or used.
 *                   - (b) While its output value MUST be initially configured to return a default value
 *                         PRIOR to all other validation or function handling in case of any error(s).
 *******************************************************************************************************/
#ifdef  NET_MCAST_TX_MODULE_EN
void NetIF_AddrMulticastProtocolToHW(NET_IF_NBR        if_nbr,
                                     CPU_INT08U        *p_addr_protocol,
                                     CPU_INT08U        addr_protocol_len,
                                     NET_PROTOCOL_TYPE addr_protocol_type,
                                     CPU_INT08U        *p_addr_hw,
                                     CPU_INT08U        *p_addr_hw_len,
                                     RTOS_ERR          *p_err)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;
  CPU_INT08U addr_hw_len;

  addr_hw_len = *p_addr_hw_len;
  *p_addr_hw_len = 0u;                                          // Cfg dflt addr len for err (see Note #2b).

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               -------------- CONVERT MULTICAST ADDR --------------
  p_if_api->AddrMulticastProtocolToHW(p_if,
                                      p_addr_protocol,
                                      addr_protocol_len,
                                      addr_protocol_type,
                                      p_addr_hw,
                                      &addr_hw_len,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  *p_addr_hw_len = addr_hw_len;

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                           NetIF_MTU_GetProtocol()
 *
 * @brief    Get network interface's MTU for desired protocol layer.
 *
 * @param    if_nbr      Network interface number to get MTU.
 *
 * @param    protocol    Desired protocol layer of network interface MTU.
 *
 * @param    opt         Option flags.
 *                       NET_IF_FLAG_NONE
 *                       NET_IF_FLAG_FRAG
 *
 * @return   Network interface's MTU at desired protocol, if NO error(s).
 *           0, otherwise.
 *******************************************************************************************************/
NET_MTU NetIF_MTU_GetProtocol(NET_IF_NBR        if_nbr,
                              NET_PROTOCOL_TYPE protocol,
                              NET_IF_FLAG       opt)
{
  NET_IF     *p_if = DEF_NULL;
  NET_IF_API *p_if_api = DEF_NULL;
  NET_MTU    mtu = 0u;
  CPU_INT16U pkt_size_hdr = 0u;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, mtu);

  //                                                               ------------- CALC PROTOCOL LAYER MTU --------------

  pkt_size_hdr = NetIF_GetProtocolHdrSize(DEF_NULL, protocol);

  if (protocol == NET_PROTOCOL_TYPE_LINK) {
    p_if_api = (NET_IF_API *)p_if->IF_API;

    pkt_size_hdr += p_if_api->GetPktSizeHdr(p_if);
  }

  mtu = p_if->MTU - pkt_size_hdr;

  PP_UNUSED_PARAM(opt);

  return (mtu);
}

/****************************************************************************************************//**
 *                                           NetIF_GetPayloadRxMax()
 *
 * @brief    Get maximum payload that CAN be received.
 *
 * @param    if_nbr      Network interface number to get payload.
 *
 * @param    protocol    Desired protocol layer of network interface.
 *
 * @return   Maximum received payload.
 *******************************************************************************************************/
CPU_INT16U NetIF_GetPayloadRxMax(NET_IF_NBR        if_nbr,
                                 NET_PROTOCOL_TYPE protocol)
{
  NET_IF       *p_if = DEF_NULL;
  NET_BUF_SIZE buf_size = 0u;
  CPU_INT16U   hdr_size = 0u;
  CPU_INT16U   payload = 0u;
  RTOS_ERR     local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

  //                                                               -------------------- GET NET IF --------------------
  buf_size = NetIF_GetPktSizeMax(if_nbr);

  hdr_size = NetIF_GetProtocolHdrSize(p_if, protocol);

  payload = buf_size - hdr_size;

  return (payload);
}

/****************************************************************************************************//**
 *                                           NetIF_GetPayloadTxMax()
 *
 * @brief    Get maximum payload that CAN be transmitted.
 *
 * @param    if_nbr      Network interface number to get payload.
 *
 * @param    protocol    Desired protocol layer of network interface MTU.
 *
 * @return   Maximum Transmit payload.
 *******************************************************************************************************/
CPU_INT16U NetIF_GetPayloadTxMax(NET_IF_NBR        if_nbr,
                                 NET_PROTOCOL_TYPE protocol)
{
  NET_IF     *p_if = DEF_NULL;
  CPU_INT16U hdr_size = 0u;
  CPU_INT16U payload = 0u;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

  //                                                               -------------------- GET NET IF --------------------
  hdr_size = NetIF_GetProtocolHdrSize(DEF_NULL, protocol);
  payload = p_if->MTU - hdr_size;

  return (payload);
}

/****************************************************************************************************//**
 *                                           NetIF_GetPktSizeMin()
 *
 * @brief    Get network interface's minimum packet size.
 *
 * @param    if_nbr  Network interface number to get minimum packet size.
 *
 * @return   Network interface's minimum packet size, if NO error(s).
 *           0, otherwise.
 *******************************************************************************************************/
CPU_INT16U NetIF_GetPktSizeMin(NET_IF_NBR if_nbr)
{
  NET_IF     *p_if = DEF_NULL;
  NET_IF_API *p_if_api = DEF_NULL;
  CPU_INT16U pkt_size_min = 0;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               ------------- GET NET IF MIN PKT SIZE --------------
  pkt_size_min = p_if_api->GetPktSizeMin(p_if);

  return (pkt_size_min);
}

/****************************************************************************************************//**
 *                                           NetIF_GetPktSizeMax()
 *
 * @brief    Get network interface's maximum packet size.
 *
 * @param    if_nbr  Network interface number to get minimum packet size.
 *
 * @return   Network interface's minimum packet size, if NO error(s).
 *           0, otherwise.
 *******************************************************************************************************/
CPU_INT16U NetIF_GetPktSizeMax(NET_IF_NBR if_nbr)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;
  CPU_INT16U pkt_size_max;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               ------------- GET NET IF MIN PKT SIZE --------------
  pkt_size_max = p_if_api->GetPktSizeMax(p_if);

  return (pkt_size_max);
}

/****************************************************************************************************//**
 *                                       NetIF_LinkStateGetHandler()
 *
 * @brief    Get network interface's last known physical link state (see also Note #3).
 *
 * @param    if_nbr  Network interface number to get last known physical link state.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   NET_IF_LINK_UP,   if NO error(s) & network interface's last known physical link state was 'UP'.
 *           NET_IF_LINK_DOWN, otherwise.
 *******************************************************************************************************/
NET_IF_LINK_STATE NetIF_LinkStateGetHandler(NET_IF_NBR if_nbr,
                                            RTOS_ERR   *p_err)
{
  NET_IF *p_if = DEF_NULL;

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (NET_IF_LINK_DOWN);
  }

  //                                                               -------------- GET NET IF LINK STATE ---------------
  return (p_if->Link);
}

/****************************************************************************************************//**
 *                                       NetIF_LinkStateSubscribeHandler()
 *
 * @brief    Subscribe to get notified when an interface link state changes.
 *
 * @param    if_nbr  Network interface number to check link state.
 *
 * @param    fcnt    Function to call when the link changes.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetIF_LinkStateSubscribeHandler(NET_IF_NBR                  if_nbr,
                                     NET_IF_LINK_SUBSCRIBER_FNCT fcnt,
                                     RTOS_ERR                    *p_err)
{
  NET_IF                     *p_if;
  NET_IF_LINK_SUBSCRIBER_OBJ *p_obj;

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               -------- VALIDATE SUBSCRIBER DOESN'T EXIST ---------
  p_obj = p_if->LinkSubscriberListHeadPtr;
  while (p_obj != DEF_NULL) {
    if (p_obj->Fnct == fcnt) {
      p_obj->RefCtn++;
      goto exit;
    }

    p_obj = p_obj->NextPtr;
  }

  //                                                               -------- GET MEMORY TO STORE NEW SUBSCRIBER --------
  p_obj = (NET_IF_LINK_SUBSCRIBER_OBJ *)Mem_DynPoolBlkGet(&p_if->LinkSubscriberPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_obj->RefCtn = 0u;
  p_obj->Fnct = fcnt;
  p_obj->NextPtr = DEF_NULL;

  //                                                               -------------- UPDATE SUBSCRIBER LIST --------------
  if (p_if->LinkSubscriberListHeadPtr == DEF_NULL) {
    p_if->LinkSubscriberListHeadPtr = p_obj;
    p_if->LinkSubscriberListEndPtr = p_obj;
  } else {
    p_if->LinkSubscriberListEndPtr->NextPtr = p_obj;
    p_if->LinkSubscriberListEndPtr = p_obj;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                   NetIF_LinkStateUnsubscribeHandler()
 *
 * @brief    Unsubscribe to get notified when interface link state changes.
 *
 * @param    if_nbr  Network interface number to check link state.
 *
 * @param    fcnt    Function to call when the link changes.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetIF_LinkStateUnSubscribeHandler(NET_IF_NBR                  if_nbr,
                                       NET_IF_LINK_SUBSCRIBER_FNCT fcnt,
                                       RTOS_ERR                    *p_err)
{
  NET_IF                     *p_if;
  NET_IF_LINK_SUBSCRIBER_OBJ *p_obj;
  NET_IF_LINK_SUBSCRIBER_OBJ *p_obj_prev;

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_obj_prev = DEF_NULL;
  p_obj = p_if->LinkSubscriberListHeadPtr;
  while (p_obj != DEF_NULL) {
    //                                                             ----------- FIND FNCT IN SUBSCRIBER LIST -----------
    if (p_obj->Fnct == fcnt) {
      if (p_obj->RefCtn == 0u) {
        if (p_obj == p_if->LinkSubscriberListHeadPtr) {
          p_if->LinkSubscriberListHeadPtr = p_if->LinkSubscriberListHeadPtr->NextPtr;

          if (p_if->LinkSubscriberListEndPtr == p_obj) {
            p_if->LinkSubscriberListHeadPtr = DEF_NULL;
            p_if->LinkSubscriberListEndPtr = DEF_NULL;
          }
        } else if (p_obj == p_if->LinkSubscriberListEndPtr) {
          p_obj_prev->NextPtr = DEF_NULL;
          p_if->LinkSubscriberListEndPtr = p_obj_prev;
        } else {
          p_obj_prev->NextPtr = p_obj->NextPtr;
        }

        //                                                         Release memory blk.
        Mem_DynPoolBlkFree(&p_if->LinkSubscriberPool, p_obj, p_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      } else {
        p_obj->RefCtn--;
      }

      goto exit;
    }

    p_obj_prev = p_obj;
    p_obj = p_obj->NextPtr;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIF_RxQPost()
 *
 * @brief    Signal that a new packet was received by adding an event in the RX event list.
 *
 * @param    if_nbr  Network interface on which packet was received.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) This function is a network protocol suite to network device function & SHOULD be called
 *               only by appropriate network device driver function(s).
 *
 * @note     (2) To balance network receive versus transmit packet loads for certain network connection
 *               types (e.g. stream-type connections), network receive & transmit packets SHOULD be
 *               handled in an APPROXIMATELY balanced ratio.
 *               - (a) To implement network receive versus transmit load balancing :
 *                   - (1) The availability of network receive packets MUST be managed for each network
 *                         interface :
 *                       - (A) Increment the number of available network receive packets queued to a
 *                             network interface for each packet received.
 *               See also 'net_if.c  NetIF_RxPktInc()  Note #1'.
 *******************************************************************************************************/
void NetIF_RxQPost(NET_IF_NBR if_nbr,
                   RTOS_ERR   *p_err)
{
  NET_IF_EVENT_RX *p_event_rx;
  SLIST_MEMBER    *p_node;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_valid;
#endif
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)

  is_valid = NetIF_IsValidHandler(if_nbr);                  // Validate interface number.
  if (is_valid != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
#endif

  CORE_ENTER_ATOMIC();
  p_node = SList_Pop(&NetIF_DataPtr->ListFreeEventRxHead);
  CORE_EXIT_ATOMIC();
  if (p_node == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    goto exit;
  }

  p_event_rx = SLIST_ENTRY(p_node, NET_IF_EVENT_RX, ListNode);

  p_event_rx->IF_Nbr = if_nbr;

  CORE_ENTER_ATOMIC();
  if (NetIF_DataPtr->ListEventRxHead == DEF_NULL) {
    SList_PushBack(&NetIF_DataPtr->ListEventRxHead, &(p_event_rx->ListNode));
  } else {
    SList_Add(&(p_event_rx->ListNode), NetIF_DataPtr->ListEventRxTail);
  }
  NetIF_DataPtr->ListEventRxTail = &(p_event_rx->ListNode);
  CORE_EXIT_ATOMIC();

  NetTask_SignalPost();

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIF_RxQPend()
 *
 * @brief    Check for a received packet event in the Rx event list.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Interface number on which packet was received, if NO error(s).
 *           NET_IF_NBR_NONE, otherwise.
 *******************************************************************************************************/
NET_IF_NBR NetIF_RxQPend(RTOS_ERR *p_err)
{
  NET_IF_EVENT_RX *p_event_rx = DEF_NULL;
  SLIST_MEMBER    *p_node = DEF_NULL;
  NET_IF_NBR      if_nbr = NET_IF_NBR_NONE;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_node = SList_Pop(&NetIF_DataPtr->ListEventRxHead);
  if (NetIF_DataPtr->ListEventRxHead == DEF_NULL) {
    NetIF_DataPtr->ListEventRxTail = DEF_NULL;
  }
  CORE_EXIT_ATOMIC();

  if (p_node == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_POOL_EMPTY);
    goto exit;
  }

  p_event_rx = SLIST_ENTRY(p_node, NET_IF_EVENT_RX, ListNode);

  if_nbr = p_event_rx->IF_Nbr;

  CORE_ENTER_ATOMIC();
  SList_Push(&NetIF_DataPtr->ListFreeEventRxHead, p_node);
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (if_nbr);
}

/******************************************************************************************************
 *                                           NetIF_RxHandler()
 *
 * @brief    - (1) Receive data packets from network interface(s)/device(s) :
 *               - (a) Receive packet from interface/device :
 *                   - (1) Get    receive packet's   network interface
 *                   - (2) Update receive packet counters
 *                   - (3)        Receive packet via network interface
 *               - (b) Handle network load balancing  #### NET-821
 *               - (c) Update receive statistics
 *
 * @param    if_nbr  Network interface number that received a packet.
 *******************************************************************************************************/
void NetIF_RxHandler(NET_IF_NBR if_nbr)
{
  NET_IF       *p_if;
  NET_BUF_SIZE size = 0u;
  RTOS_ERR     local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               --------------- GET RX PKT's NET IF ----------------
  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               ------------------ RX NET IF PKT -------------------
  NetStat_CtrDec(&NetIF_DataPtr->RxTaskPktCtr);                 // Dec rx task's nbr q'd rx pkts avail.

  NET_CTR_STAT_INC(Net_StatCtrs.IFs.RxPktCtr);
  NET_CTR_STAT_INC(Net_StatCtrs.IFs.IF[if_nbr].RxNbrPktCtr);

  if (if_nbr == NET_IF_NBR_LOOPBACK) {
#ifdef NET_IF_LOOPBACK_MODULE_EN
    size = NetIF_Loopback_Rx(p_if, &local_err);
#else
    RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
#endif
  } else {
    size = NetIF_RxPkt(p_if, &local_err);
  }

#ifdef  NET_LOAD_BAL_MODULE_EN                                  // --------------- HANDLE NET LOAD BAL ----------------
  NetIF_RxHandlerLoadBal(p_if);
#endif
  //                                                               ----------------- UPDATE RX STATS ------------------
  //                                                               Chk err from NetIF_Loopback_Rx() / NetIF_RxPkt().
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    //                                                             See Note #2.
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.RxPktDisCtr);
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IF[if_nbr].RxPktDisCtr);
    goto exit;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.IFs.IF[if_nbr].RxNbrPktCtrProcessed);
  NET_CTR_STAT_ADD(Net_StatCtrs.IFs.IF[if_nbr].RxNbrOctets, size);

  PP_UNUSED_PARAM(size);                                        // Prevent possible 'variable unused' warning.

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIF_RxPktInc()
 *
 * @brief    Increment number receive packet(s) queued & available on a network interface.
 *
 * @param    if_nbr  Interface number that received packet(s).
 *
 * @note     (1) To balance network receive versus transmit packet loads for certain network connection
 *               types (e.g. stream-type connections), network receive & transmit packets on each
 *               network interface SHOULD be handled in an APPROXIMATELY balanced ratio.
 *               - (a) Network task priorities & lock mechanisms partially maintain a balanced ratio
 *                     between network receive versus transmit packet handling.
 *                     However, the handling of network receive & transmit packets :
 *                   - (1) SHOULD be interleaved so that for every few packet(s) received & handled,
 *                         several packet(s) should be transmitted; & vice versa.
 *                   - (2) SHOULD NOT exclusively handle receive nor transmit packets, even for a
 *                         short period of time, but especially for a prolonged period of time.
 *               - (b) To implement network receive versus transmit load balancing :
 *                   - (1) The availability of network receive packets MUST be managed for each network
 *                         interface :
 *                       - (A) Increment the number of available network receive packets queued to a
 *                             network interface for each packet received.
 *                       - (B) Decrement the number of available network receive packets queued to a
 *                             network interface for each received packet processed.
 *                   - (2) Certain network connections MUST periodically suspend network transmit(s)
 *                         to handle network interface(s)' receive packet(s) :
 *                       - (A) Suspend network connection transmit(s) if any receive packets are
 *                             available on a network interface.
 *                       - (B) Signal or timeout network connection transmit suspend(s) to restart
 *                             transmit(s).
 *
 * @note     (2) Network interfaces' 'RxPktCtr' variables MUST ALWAYS be accessed exclusively in
 *                   critical sections.
 *******************************************************************************************************/
void NetIF_RxPktInc(NET_IF_NBR if_nbr)
{
#ifdef  NET_LOAD_BAL_MODULE_EN
  NET_IF *p_if;

  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];
  NetStat_CtrInc(&p_if->RxPktCtr);                              // Inc net IF's  nbr q'd rx pkts avail (see Note #1b1A).

#else
  PP_UNUSED_PARAM(if_nbr);                                      // Prevent 'variable unused' compiler warning.
#endif

  NetStat_CtrInc(&NetIF_DataPtr->RxTaskPktCtr);                 // Inc rx task's nbr q'd rx pkts avail.
}

/****************************************************************************************************//**
 *                                           NetIF_RxPktIsAvail()
 *
 * @brief    Determine if any network interface receive packet(s) are available.
 *
 * @param    if_nbr      Interface number that received packet(s).
 *
 * @param    rx_chk_nbr  Number of consecutive times that network interface's receive packet
 *                       availability has been checked (see Note #2b1).
 *
 * @return   DEF_YES, network interface receive packet(s)     available (see Note #2a1).
 *           DEF_NO,  network interface receive packet(s) NOT available (see Note #2a2).
 *
 * @note     (1) To balance network receive versus transmit packet loads for certain network connection
 *               types (e.g. stream-type connections), network receive & transmit packets on each
 *               network interface SHOULD be handled in an APPROXIMATELY balanced ratio.
 *               - (a) Network task priorities & lock mechanisms partially maintain a balanced ratio
 *                     between network receive versus transmit packet handling.
 *                     However, the handling of network receive & transmit packets :
 *                   - (1) SHOULD be interleaved so that for every few packet(s) received & handled,
 *                         several packet(s) should be transmitted; & vice versa.
 *                   - (2) SHOULD NOT exclusively handle receive nor transmit packets, even for a
 *                         short period of time, but especially for a prolonged period of time.
 *               - (b) To implement network receive versus transmit load balancing :
 *                   - (1) The availability of network receive packets MUST be managed for each network
 *                         interface :
 *                       - (A) Increment the number of available network receive packets queued to a
 *                             network interface for each packet received.
 *                       - (B) Decrement the number of available network receive packets queued to a
 *                             network interface for each received packet processed.
 *
 * @note     (2) To approximate a balanced ratio of network receive versus transmit packets
 *               handled; the availability of network receive packets returned is conditionally
 *               based on the consecutive number of times the availability is checked :
 *               - (a) If the number of available network receive packets queued ('NetIF_RxPktCtr')
 *                     is greater than the consecutive number of times the availability is checked
 *                     ('rx_chk_nbr'), then the actual availability of network receive packet is
 *                     returned.
 *               - (b) Otherwise, no available network receive packets is returned -- even if
 *                     network receive packets ARE available.
 *
 * @note     (3) The number of consecutive times that the network receive availability
 *               is checked ('rx_chk_nbr') SHOULD correspond to the consecutive number
 *               of times that a network connection transmit suspends itself to check
 *               for & handle any network receive packet(s).
 *               - (a) To check actual network receive packet availability,
 *                     call NetIF_RxPktIsAvail() with 'rx_chk_nbr' always    set to 0.
 *               - (b) To check        network receive packet availability consecutively,
 *                     call NetIF_RxPktIsAvail() with 'rx_chk_nbr' initially set to 0 &
 *                     incremented by 1 for each consecutive call thereafter.
 *
 * @note     (4) Network interfaces' 'RxPktCtr' variables MUST ALWAYS be accessed exclusively in
 *               critical sections.
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_RxPktIsAvail(NET_IF_NBR if_nbr,
                               NET_CTR    rx_chk_nbr)
{
  CPU_BOOLEAN rx_pkt_avail;
#ifdef  NET_LOAD_BAL_MODULE_EN
  NET_IF       *p_if;
  NET_STAT_CTR *pstat_ctr;
  CORE_DECLARE_IRQ_STATE;

  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];
  pstat_ctr = &p_if->RxPktCtr;

  CORE_ENTER_ATOMIC();
  rx_pkt_avail = (pstat_ctr->CurCtr > rx_chk_nbr) ? DEF_YES : DEF_NO;       // See Note #2.
  CORE_EXIT_ATOMIC();

#else
  PP_UNUSED_PARAM(if_nbr);                                      // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(rx_chk_nbr);
  rx_pkt_avail = DEF_NO;
#endif

  return (rx_pkt_avail);
}

/****************************************************************************************************//**
 *                                           NetIF_TxDeallocQkPost()
 *
 * @brief    Signal that a new buffer is ready to be de-allocated by adding an event in the TX event list.
 *           The event contains the network buffer transmit data areas to deallocate from device(s).
 *
 * @param    p_buf_data  Pointer to transmit buffer data area to deallocate.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) This function is a network protocol suite to network device function & SHOULD be called
 *               only by appropriate network interface/device controller function(s).
 *******************************************************************************************************/
void NetIF_TxDeallocQPost(CPU_INT08U *p_buf_data,
                          RTOS_ERR   *p_err)
{
  NET_IF_EVENT_TX *p_event_tx;
  SLIST_MEMBER    *p_node;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  p_node = SList_Pop(&NetIF_DataPtr->ListFreeEventTxHead);
  CORE_EXIT_ATOMIC();

  if (p_node == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    goto exit;
  }

  p_event_tx = SLIST_ENTRY(p_node, NET_IF_EVENT_TX, ListNode);

  p_event_tx->BufDataPtr = p_buf_data;

  CORE_ENTER_ATOMIC();
  if (NetIF_DataPtr->ListEventTxHead == DEF_NULL) {
    SList_PushBack(&NetIF_DataPtr->ListEventTxHead, &(p_event_tx->ListNode));
  } else {
    SList_Add(&(p_event_tx->ListNode), NetIF_DataPtr->ListEventTxTail);
  }
  NetIF_DataPtr->ListEventTxTail = &(p_event_tx->ListNode);
  CORE_EXIT_ATOMIC();

#if 0 /* TODO_NET : Was remove because the dealloc is done in the GetBuf and in the core task periodically
         when waked up by other signals. */
  NetTask_SignalPost();
#endif

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_TxDeallocQPend()
 *
 * @brief    Check for a buffer transmitted to be deallocated in the Tx event list.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to deallocated transmit buffer data area, if NO error(s).
 *           Pointer to NULL, otherwise.
 *******************************************************************************************************/
CPU_INT08U *NetIF_TxDeallocQPend(RTOS_ERR *p_err)
{
  NET_IF_EVENT_TX *p_event_tx = DEF_NULL;
  SLIST_MEMBER    *p_node = DEF_NULL;
  CPU_INT08U      *p_buf_data = DEF_NULL;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  p_node = SList_Pop(&NetIF_DataPtr->ListEventTxHead);
  if (NetIF_DataPtr->ListEventTxHead == DEF_NULL) {
    NetIF_DataPtr->ListEventTxTail = DEF_NULL;
  }
  CORE_EXIT_ATOMIC();

  if (p_node == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_POOL_EMPTY);
    goto exit;
  }

  p_event_tx = SLIST_ENTRY(p_node, NET_IF_EVENT_TX, ListNode);

  p_buf_data = p_event_tx->BufDataPtr;

  CORE_ENTER_ATOMIC();
  SList_Push(&NetIF_DataPtr->ListFreeEventTxHead, p_node);
  CORE_EXIT_ATOMIC();

exit:
  return (p_buf_data);
}

/****************************************************************************************************//**
 *                                               NetIF_Tx()
 *
 * @brief    Transmit data packets to network interface(s)/device(s).
 *
 * @param    p_buf_list  Pointer to network buffer data packet(s) to transmit via network interface(s)/
 *                       device(s) [see Note #1a].
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) On any error(s), the current transmit packet may be discarded by handler functions;
 *               but any remaining transmit packet(s) are still transmitted.
 *               However, while IP transmit fragmentation is NOT currently supported,
 *               transmit data packet lists are limited to a single transmit data packet.
 *******************************************************************************************************/
void NetIF_Tx(NET_BUF  *p_buf_list,
              RTOS_ERR *p_err)
{
  NET_BUF     *p_buf;
  NET_BUF     *p_buf_next;
  NET_BUF_HDR *p_buf_hdr;

  //                                                               ----------------- TX NET IF PKT(S) -----------------
  p_buf = p_buf_list;

  while (p_buf != DEF_NULL) {                                   // Tx ALL pkt bufs in list.
    p_buf_hdr = &p_buf->Hdr;
    p_buf_next = p_buf_hdr->NextBufPtr;

    NetIF_TxHandler(p_buf,
                    p_err);                                     // See Note #1a.
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    //                                                             --------------- UNLINK CHAINED BUFS ----------------
    if (p_buf_next != DEF_NULL) {
      p_buf_hdr->NextBufPtr = DEF_NULL;
      p_buf_hdr = &p_buf_next->Hdr;
      p_buf_hdr->PrevBufPtr = DEF_NULL;
    }

    p_buf = p_buf_next;                                         // Adv to next tx pkt buf.
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_TxIxDataGet()
 *
 * @brief    Get the offset of a buffer at which the IPv6 packet CAN be written.
 *
 * @param    if_nbr      Network interface number to transmit data.
 *
 * @param    data_len    IPv6 payload size.
 *
 * @param    mtu         MTU for the upper-layer protocol.
 *
 * @param    p_ix        Pointer to the current protocol index.
 *******************************************************************************************************/
void NetIF_TxIxDataGet(NET_IF_NBR if_nbr,
                       CPU_INT32U data_len,
                       CPU_INT16U *p_ix)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_if_api = (NET_IF_API *)p_if->IF_API;

  *p_ix += p_if_api->GetPktSizeHdr(p_if);

  PP_UNUSED_PARAM(data_len);
}

/****************************************************************************************************//**
 *                                               NetIF_TxSuspend()
 *
 * @brief    Suspend transmit on network interface connection(s).
 *
 * @param    if_nbr  Interface number to suspend transmit.
 *
 * @note     (1) To balance network receive versus transmit packet loads for certain network connection
 *               types (e.g. stream-type connections), network receive & transmit packets on each
 *               network interface SHOULD be handled in an APPROXIMATELY balanced ratio.
 *               - (a) Network task priorities & lock mechanisms partially maintain a balanced ratio
 *                     between network receive versus transmit packet handling.
 *                     However, the handling of network receive & transmit packets :
 *                   - (1) SHOULD be interleaved so that for every few packet(s) received & handled,
 *                         several packet(s) should be transmitted; & vice versa.
 *                   - (2) SHOULD NOT exclusively handle receive nor transmit packets, even for a
 *                         short period of time, but especially for a prolonged period of time.
 *               - (b) To implement network receive versus transmit load balancing :
 *                   - (1) The availability of network receive packets MUST be managed for each network
 *                         interface :
 *                      - (A) Increment the number of available network receive packets queued to a
 *                            network interface for each packet received.
 *                      - (B) Decrement the number of available network receive packets queued to a
 *                            network interface for each received packet processed.
 *                   - (2) Certain network connections MUST periodically suspend network transmit(s)
 *                         to handle network interface(s)' receive packet(s) :
 *                       - (A) Suspend network connection transmit(s) if any receive packets are
 *                             available on a network interface.
 *                       - (B) Signal or timeout network connection transmit suspend(s) to restart
 *                             transmit(s).
 *
 * @note     (2) To approximate a balanced ratio of network receive versus transmit packets handled;
 *               the number of consecutive times that a network connection transmit suspends itself
 *               to check for & handle any network receive packet(s) SHOULD APPROXIMATELY correspond
 *               to the number of queued receive packet(s) available on a network interface.
 *               To protect connections from transmit corruption while suspended, ALL transmit
 *               operations for suspended connections MUST be blocked until the connection is no
 *               longer suspended.
 *
 * @note     (3) Network interfaces' 'TxSuspendCtr' variables may be accessed with only the global
 *               network lock acquired & are NOT required to be accessed exclusively in critical
 *               sections.
 *******************************************************************************************************/
void NetIF_TxSuspend(NET_IF_NBR if_nbr)
{
#ifdef  NET_LOAD_BAL_MODULE_EN
  NET_IF *p_if;

  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];
  NetStat_CtrInc(&p_if->TxSuspendCtr);                          // Inc net IF's tx suspend ctr.
  NetIF_TxSuspendWait(p_if);                                    // Wait on      tx suspend signal (see Note #1b2A).
  NetStat_CtrDec(&p_if->TxSuspendCtr);                          // Dec net IF's tx suspend ctr.
#else
  PP_UNUSED_PARAM(if_nbr);                                      // Prevent 'variable unused' compiler warning.
#endif
}

/****************************************************************************************************//**
 *                                           NetIF_TxPktListDealloc()
 *
 * @brief    (1) Deallocate transmitted packet's network buffer & data area :
 *               - (a) Search Network Interface Transmit List for transmitted network buffer
 *               - (b) Deallocate transmit packet buffer :
 *                   - (1) Remove transmit packet buffer from network interface transmit list
 *                   - (2) Free network buffer
 *
 * @param    p_buf_data  Pointer to a network packet buffer's transmitted data area (see Note #2b1).
 *******************************************************************************************************/
void NetIF_TxPktListDealloc(CPU_INT08U *p_buf_data)
{
  NET_BUF *p_buf;

  p_buf = NetIF_TxPktListSrch(p_buf_data);                      // Srch tx list for tx'd pkt buf.
  if (p_buf != DEF_NULL) {                                      // If   tx'd  pkt buf found,       ...
    NetIF_TxPktListRemove(p_buf);                               // ... remove pkt buf from tx list ...
    NetIF_TxPktFree(p_buf);                                     // ... & free net buf.
    NET_CTR_STAT_INC(Net_StatCtrs.IFs.TxPktDeallocCtr);
  } else {
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.TxPktDeallocCtr);
  }
}

/****************************************************************************************************//**
 *                                           NetIF_DevCfgTxRdySignal()
 *
 * @brief    (1) Configure the value of a network device transmit ready signal :
 *               - (a) The value of the transmit ready signal should be configured with either the
 *                     number of available transmit descriptors for a DMA device or the number of
 *                     packet that can be buffered within a non-DMA device.
 *
 * @param    if_nbr  Interface number of the network device transmit ready signal.
 *
 * @param    cnt     Desired count    of the network device transmit ready signal.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) This function is a network interface (IF) to network device function & SHOULD (optionally)
 *               be called only by appropriate network device driver function(s).
 *******************************************************************************************************/
void NetIF_DevCfgTxRdySignal(NET_IF     *p_if,
                             CPU_INT16U cnt)
{
  RTOS_ERR local_err;

  KAL_SEM_HANDLE *p_sem_obj = (KAL_SEM_HANDLE *)p_if->DevTxRdySignalObj;

  RTOS_ASSERT_DBG((cnt >= 1), RTOS_ERR_INVALID_CFG,; );

  KAL_SemSet(*p_sem_obj, cnt, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           NetIF_DevTxRdySignal()
 *
 * @brief    Signal that device transmit is ready.
 *
 * @param    p_if    Pointer to interface to signal transmit ready.
 *
 * @note     (1) This function is a network interface (IF) to network device function & SHOULD be called
 *               only by appropriate network device driver function(s).
 *
 * @note     (2) Device transmit ready MUST be signaled--i.e. MUST signal without failure.
 *               - (a) Failure to signal device transmit ready will prevent device from transmitting
 *                     packets.  Thus, device transmit ready is assumed to be successfully signaled
 *                     since NO Micrium OS Kernel error handling could be performed to counteract failure.
 *******************************************************************************************************/
void NetIF_DevTxRdySignal(NET_IF *p_if)
{
  KAL_SEM_HANDLE *p_sem_obj = (KAL_SEM_HANDLE *)p_if->DevTxRdySignalObj;
  RTOS_ERR       local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_SemPost(*p_sem_obj,                          // Signal device that transmit ready.
              KAL_OPT_PEND_NONE,
              &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           NetIF_MTU_SetHandler()
 *
 * @brief    Set network interface's MTU.
 *
 * @param    if_nbr  Network interface number to set MTU.
 *
 * @param    mtu     Desired maximum transmission unit size to set.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetIF_MTU_SetHandler(NET_IF_NBR if_nbr,
                          NET_MTU    mtu,
                          RTOS_ERR   *p_err)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               ------------------ SET NET IF MTU ------------------
  p_if_api->MTU_Set(p_if, mtu, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIF_Q_Update()
 *
 * @brief    When a new interface is added, this function adds new events to the Rx and Tx Free Event Lists.
 *           The number of new events depends on the number of Rx and Tx buffers configured in
 *           the device configuration.
 *
 * @param    p_mem_seg   Pointer to memory segment to use to allocate the object for the Rx & Tx Q.
 *                       DEF_NULL to use the configured HEAP memory.
 *
 * @param    nbr_rx      Number of events to add in the Free Rx event list.
 *
 * @param    nbr_tx      Number of events to add in the Free Tx event list.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIF_Q_Update(MEM_SEG     *p_mem_seg,
                           NET_BUF_QTY nbr_rx,
                           NET_BUF_QTY nbr_tx,
                           RTOS_ERR    *p_err)
{
  NET_IF_EVENT_RX *p_rx_event;
  NET_IF_EVENT_TX *p_tx_event;
  NET_BUF_QTY     i;
  CORE_DECLARE_IRQ_STATE;

  for (i = 0; i < nbr_rx; i++) {
    p_rx_event = (NET_IF_EVENT_RX *)Mem_SegAlloc("IF Rx Event",
                                                 p_mem_seg,
                                                 sizeof(NET_IF_EVENT_RX),
                                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    CORE_ENTER_ATOMIC();
    SList_Push(&NetIF_DataPtr->ListFreeEventRxHead, &(p_rx_event->ListNode));
    CORE_EXIT_ATOMIC();
  }

  for (i = 0; i < nbr_tx; i++) {
    p_tx_event = (NET_IF_EVENT_TX *)Mem_SegAlloc("IF Tx Event",
                                                 p_mem_seg,
                                                 sizeof(NET_IF_EVENT_TX),
                                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    CORE_ENTER_ATOMIC();
    SList_Push(&NetIF_DataPtr->ListFreeEventTxHead, &(p_tx_event->ListNode));
    CORE_EXIT_ATOMIC();
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIF_ObjInit()
 *
 * @brief    Create and initialize interface's OS objects.
 *
 * @param    p_if    Pointer to network interface.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIF_ObjInit(NET_IF   *p_if,
                          RTOS_ERR *p_err)
{
  KAL_SEM_HANDLE *p_sem_obj;

  p_if->IP_Obj = (NET_IP_IF_OBJ *)Mem_SegAlloc("IF IP Obj",
                                               DEF_NULL,
                                               sizeof(NET_IP_IF_OBJ),
                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_if->DevTxRdySignalObj = Mem_SegAlloc("IF Tx Rdy Signal Obj",
                                         DEF_NULL,
                                         sizeof(KAL_SEM_HANDLE),
                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  p_sem_obj = (KAL_SEM_HANDLE *)p_if->DevTxRdySignalObj;

  *p_sem_obj = KAL_SemCreate(NET_IF_DEV_TX_RDY_NAME,
                             DEF_NULL,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

#ifdef  NET_LOAD_BAL_MODULE_EN                                  // ------ INITIALIZE NETWORK INTERFACE TRANSMIT SUSPEND -------

  //                                                               Initialize transmit suspend signals' timeout values.
  NetIF_TxSuspendTimeoutInit(p_if);

  p_if->TxSuspendSignalObj = Mem_SegAlloc("IF Tx Suspend Signal Obj",
                                          DEF_NULL,
                                          sizeof(KAL_SEM_HANDLE),
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  p_sem_obj = (KAL_SEM_HANDLE *)p_if->TxSuspendSignalObj;

  *p_sem_obj = KAL_SemCreate(NET_IF_TX_SUSPEND_NAME,
                             DEF_NULL,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail_tx_signal;
  }
#endif

  Mem_DynPoolCreate(NET_IF_LINK_SUBSCRIBER,
                    &p_if->LinkSubscriberPool,
                    DEF_NULL,
                    sizeof(NET_IF_LINK_SUBSCRIBER_OBJ),
                    sizeof(CPU_DATA),
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail_tx_suspend;
  }

  p_if->LinkSubscriberListHeadPtr = DEF_NULL;
  p_if->LinkSubscriberListEndPtr = DEF_NULL;

  //                                                               ---------- CREATE INTERFACE START OBJECT -----------
  p_if->StartObj = (NET_IF_START_OBJ *)Mem_SegAlloc("IF start object",
                                                    DEF_NULL,
                                                    sizeof(NET_IF_START_OBJ),
                                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail_tx_suspend;
  }

#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  Mem_DynPoolCreate("IF Start msg",
                    &p_if->StartObj->StartMsgPool,
                    DEF_NULL,
                    sizeof(NET_IF_START_MSG),
                    sizeof(CPU_ALIGN),
                    1,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail_tx_suspend;
  }
#endif

  goto exit;

exit_fail_tx_suspend:
#ifdef  NET_LOAD_BAL_MODULE_EN
  p_sem_obj = (KAL_SEM_HANDLE *)p_if->TxSuspendSignalObj;
  KAL_SemDel(*p_sem_obj);

exit_fail_tx_signal:

#endif
  p_sem_obj = (KAL_SEM_HANDLE *)p_if->DevTxRdySignalObj;
  KAL_SemDel(*p_sem_obj);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIF_ObjDel()
 *
 * @brief    Delete interface's OS objects.
 *
 * @param    p_if    Pointer to network interface.
 *******************************************************************************************************/
static void NetIF_ObjDel(NET_IF *p_if)
{
  KAL_SEM_HANDLE *p_sem_obj;

#ifdef  NET_LOAD_BAL_MODULE_EN
  p_sem_obj = (KAL_SEM_HANDLE *)p_if->TxSuspendSignalObj;
  KAL_SemDel(*p_sem_obj);
#endif
  p_sem_obj = (KAL_SEM_HANDLE *)p_if->DevTxRdySignalObj;
  KAL_SemDel(*p_sem_obj);
}

/****************************************************************************************************//**
 *                                           NetIF_DevTxRdyWait()
 *
 * @brief    Wait on device transmit ready signal.
 *
 * @param    if_nbr  Interface number to wait on transmit ready signal.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) If timeouts NOT desired, wait for device transmit ready signal.
 *               If timeout      desired, return RTOS_ERR_TIMEOUT error on transmit
 *               ready timeout.  Implement timeout with OS-dependent functionality.
 *******************************************************************************************************/
static void NetIF_DevTxRdyWait(NET_IF   *p_if,
                               RTOS_ERR *p_err)
{
  KAL_SEM_HANDLE *p_sem_obj = (KAL_SEM_HANDLE *)p_if->DevTxRdySignalObj;

  KAL_SemPend(*p_sem_obj, KAL_OPT_PEND_NONE, 15, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
  }
}

/****************************************************************************************************//**
 *                                           NetIF_RxHandlerLoadBal()
 *
 * @brief    Handle network receive versus transmit load balancing.
 *
 * @param    p_if    Pointer to network interface to handle load balancing.
 *
 * @note     (1) To balance network receive versus transmit packet loads for certain network connection
 *               types (e.g. stream-type connections), network receive & transmit packets on each
 *               network interface SHOULD be handled in an APPROXIMATELY balanced ratio.
 *               - (a) Network task priorities & lock mechanisms partially maintain a balanced ratio
 *                     between network receive versus transmit packet handling.
 *                     However, the handling of network receive & transmit packets :
 *                   - (1) SHOULD be interleaved so that for every few packet(s) received & handled,
 *                         several packet(s) should be transmitted; & vice versa.
 *                   - (2) SHOULD NOT exclusively handle receive nor transmit packets, even for a
 *                         short period of time, but especially for a prolonged period of time.
 *               - (b) To implement network receive versus transmit load balancing :
 *                   - (1) The availability of network receive packets MUST be managed for each network
 *                         interface :
 *                       - (A) Increment the number of available network receive packets queued to a
 *                             network interface for each packet received.
 *                       - (B) Decrement the number of available network receive packets queued to a
 *                             network interface for each received packet processed.
 *                   - (2) Certain network connections MUST periodically suspend network transmit(s)
 *                         to handle network interface(s)' receive packet(s) :
 *                       - (A) Suspend network connection transmit(s) if any receive packets are
 *                             available on a network interface.
 *                       - (B) Signal or timeout network connection transmit suspend(s) to restart
 *                             transmit(s).
 *******************************************************************************************************/

#ifdef  NET_LOAD_BAL_MODULE_EN
static void NetIF_RxHandlerLoadBal(NET_IF *p_if)
{
  NetIF_RxPktDec(p_if);                                          // Dec net IF's nbr q'd rx pkts avail (see Note #1b1B).
  NetIF_TxSuspendSignal(p_if);                                   // Signal net tx suspend              (see Note #1b2B).
}
#endif

/****************************************************************************************************//**
 *                                               NetIF_RxPkt()
 *
 * @brief    (1) Receive data packets from devices & demultiplex to network interface layer :
 *               - (a) Update network interface's link status                          See Note #2
 *               - (b) Receive packet from device :
 *                   - (1) Get receive packet from device
 *                   - (2) Get receive packet network buffer
 *               - (c) Demultiplex receive packet to specific network interface
 *
 * @param    p_if    Pointer to network interface that received a packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Size of received packet, if NO error(s).
 *           0, otherwise.
 *
 * @note     (2) If a network interface receives a packet, its physical link must be 'UP' & the
 *               interface's physical link state is set accordingly.
 *               - (a) An attempt to check for link state is made after an interface has been started.
 *                     However, many physical layer devices, such as Ethernet physical layers require
 *                     several seconds for Auto-Negotiation to complete before the link becomes
 *                     established.  Thus the interface link flag is not updated until the link state
 *                     timer expires & one or more attempts to check for link state have been completed.
 *
 * @note     (3) When network buffer is demultiplexed to network IF receive, the buffer's reference
 *               counter is NOT incremented since the packet interface layer does NOT maintain a
 *               reference to the buffer.
 *******************************************************************************************************/
static NET_BUF_SIZE NetIF_RxPkt(NET_IF   *p_if,
                                RTOS_ERR *p_err)
{
  NET_IF_API   *p_if_api = DEF_NULL;
  NET_DEV_API  *p_dev_api = DEF_NULL;
  CPU_INT08U   *p_buf_data = DEF_NULL;
  NET_BUF      *p_buf = DEF_NULL;
  NET_BUF_HDR  *p_buf_hdr = DEF_NULL;
  NET_BUF_SIZE rtn_val = 0;
  NET_BUF_SIZE size = 0;
  NET_BUF_SIZE ix_rx;
  NET_BUF_SIZE ix_offset;
  RTOS_ERR     local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_dev_api = (NET_DEV_API *)p_if->Dev_API;
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               ----------------- RX PKT FROM DEV ------------------
  p_dev_api->Rx(p_if, &p_buf_data, &size, &local_err);          // Get rx'd buf data area from dev.
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               Get net buf for rx'd buf data area.
  ix_rx = NET_IF_IX_RX;
  p_buf = NetBuf_Get(p_if->Nbr,
                     NET_TRANSACTION_RX,
                     size,
                     ix_rx,
                     &ix_offset,
                     NET_BUF_FLAG_NONE,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NetBuf_FreeBufDataAreaRx(p_if->Nbr, p_buf_data);
    goto exit;
  }

  p_buf->DataPtr = p_buf_data;
  ix_rx += ix_offset;

  //                                                               ------------------- DEMUX RX PKT -------------------
  p_buf_hdr = &p_buf->Hdr;
  p_buf_hdr->TotLen = (NET_BUF_SIZE)size;                       // Set pkt size as buf tot len & data len.
  p_buf_hdr->DataLen = (NET_BUF_SIZE)p_buf_hdr->TotLen;
  p_buf_hdr->IF_HdrIx = (CPU_INT16U)ix_rx;
  p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IF;
  p_buf_hdr->ProtocolHdrTypeIF = NET_PROTOCOL_TYPE_IF;
  DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_RX_REMOTE);

  p_if_api->Rx(p_if, p_buf, p_err);                             // Demux rx pkt to appropriate net IF rx handler.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NetBuf_FreeBufQ_PrimList(p_buf, DEF_NULL);      // TODO_NET : validate free of list. NetBuf_FreeBuf should be enough
    goto exit;
  }

  //                                                               ---------------- RTN RX'D DATA SIZE ----------------
  rtn_val = size;

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                               NetIF_RxPktDec()
 *
 * @brief    Decrement number receive packet(s) queued & available for a network interface.
 *
 * @param    p_if    Pointer to network interface that received a packet.
 *
 * @note     (1) To balance network receive versus transmit packet loads for certain network connection
 *               types (e.g. stream-type connections), network receive & transmit packets on each
 *               network interface SHOULD be handled in an APPROXIMATELY balanced ratio.
 *               - (a) Network task priorities & lock mechanisms partially maintain a balanced ratio
 *                     between network receive versus transmit packet handling.
 *                     However, the handling of network receive & transmit packets :
 *                   - (1) SHOULD be interleaved so that for every few packet(s) received & handled,
 *                         several packet(s) should be transmitted; & vice versa.
 *                   - (2) SHOULD NOT exclusively handle receive nor transmit packets, even for a
 *                         short period of time, but especially for a prolonged period of time.
 *               - (b) To implement network receive versus transmit load balancing :
 *                   - (1) The availability of network receive packets MUST be managed for each network
 *                         interface :
 *                       - (A) Increment the number of available network receive packets queued to a
 *                             network interface for each packet received.
 *                       - (B) Decrement the number of available network receive packets queued to a
 *                             network interface for each received packet processed.
 *                   - (2) Certain network connections MUST periodically suspend network transmit(s)
 *                         to handle network interface(s)' receive packet(s) :
 *                       - (A) Suspend network connection transmit(s) if any receive packets are
 *                             available on a network interface.
 *                       - (B) Signal or timeout network connection transmit suspend(s) to restart
 *                             transmit(s).
 *
 * @note     (2) Network interfaces' 'RxPktCtr' variables MUST ALWAYS be accessed exclusively in
 *               critical sections.
 *******************************************************************************************************/

#ifdef  NET_LOAD_BAL_MODULE_EN
static void NetIF_RxPktDec(NET_IF *p_if)
{
  NetStat_CtrDec(&p_if->RxPktCtr);                              // Dec net IF's nbr q'd rx pkts avail (see Note #1b1B).
}
#endif

/****************************************************************************************************//**
 *                                               NetIF_TxHandler()
 *
 * @brief    (1) Transmit data packets to  network interface(s)/device(s) :
 *
 *           - (a) Get transmit packet's network interface
 *           - (b) Check network interface's/device's link state
 *           - (c) Transmit packet via   network interface
 *           - (d) Update transmit statistics
 *
 * @param    p_buf   Pointer to network buffer data packet to transmit.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIF_TxHandler(NET_BUF  *p_buf,
                            RTOS_ERR *p_err)
{
  NET_BUF_HDR  *p_buf_hdr;
  NET_IF       *p_if_tx;
  NET_IF_NBR   if_nbr;
  NET_IF_NBR   if_nbr_tx;
  NET_BUF_SIZE size = 0u;

  //                                                               ------------ VALIDATE TX PKT's NET IF's ------------
  p_buf_hdr = &p_buf->Hdr;
  if_nbr = p_buf_hdr->IF_Nbr;
  if_nbr_tx = p_buf_hdr->IF_NbrTx;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (if_nbr != if_nbr_tx) {                                    // If net IF to tx to NOT same as tx'ing net IF & ..
    if (if_nbr_tx != NET_IF_NBR_LOOPBACK) {                     // .. net IF to tx to NOT loopback IF,            ..
                                                                // .. discard tx pkt & rtn err.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
    }
  }
#endif

  //                                                               --------------- GET TX PKT's NET IF ----------------
  p_if_tx = NetIF_Get(if_nbr_tx, p_err);                        // Get net IF to tx to.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               -------------- CHK NET IF LINK STATE ---------------
  switch (p_if_tx->Link) {                                      // Chk link state of net IF to tx to.
    case NET_IF_LINK_UP:
      break;

    case NET_IF_LINK_DOWN:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_IF_LINK_DOWN);
      goto exit_discard;
  }

  //                                                               ------------------ TX NET IF PKT -------------------
  if (if_nbr_tx == NET_IF_NBR_LOOPBACK) {
#ifdef NET_IF_LOOPBACK_MODULE_EN
    size = NetIF_Loopback_Tx(p_if_tx,
                             p_buf,
                             p_err);
#else
    RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
#endif
  } else {
    size = NetIF_TxPkt(p_if_tx,
                       p_buf,
                       p_err);
  }

  //                                                               ----------------- UPDATE TX STATS ------------------
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      NET_CTR_STAT_INC(Net_StatCtrs.IFs.TxPktCtr);
      NET_CTR_STAT_INC(Net_StatCtrs.IFs.IF[if_nbr].TxNbrPktCtr);
      NET_CTR_STAT_INC(Net_StatCtrs.IFs.IF[if_nbr].TxNbrPktCtrProcessed);
      NET_CTR_STAT_ADD(Net_StatCtrs.IFs.IF[if_nbr].TxNbrOctets, size);
      PP_UNUSED_PARAM(if_nbr);                                  // Prevent possible 'variable unused' warnings.
      PP_UNUSED_PARAM(size);
      break;

    case RTOS_ERR_NET_ADDR_UNRESOLVED:
    default:
      goto exit_discard;
  }

  goto exit;

exit_discard:
#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  NET_CTR_ERR_INC(Net_ErrCtrs.IFs.TxPktDisCtr);
  NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IF[if_nbr].TxPktDisCtr);
#endif

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIF_TxPkt()
 *
 * @brief    (1) Transmit data packets from network interface layer to network device(s) :
 *               - (a) Validate   transmit data packet
 *               - (b) Prepare    transmit data packet(s) via network interface layer
 *               - (c) Wait for   transmit ready signal from  network device
 *               - (d) Prepare    transmit data packet(s) via network device driver :
 *                   - (1) Set    transmit data packet(s)' transmit lock
 *                   - (2) Insert transmit data packet(s) into network interface transmit list
 *               - (e)            Transmit data packet(s) via network device driver
 *
 * @param    p_if    Pointer to network interface to transmit a packet.
 *
 * @param    p_buf   Pointer to network buffer data packet to transmit.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Size of transmitted packet, if NO error(s).
 *           0, otherwise.
 *
 * @note     (2) On ANY error(s), network resources MUST be appropriately freed :
 *               - (a) After  transmit packet buffer queued to Network Interface Transmit List,
 *                     remove transmit packet buffer from      Network Interface Transmit List.
 *
 * @note     (3) Network buffer already freed by lower layer.
 *
 * @note     (4) Error codes from network interface/device driver handler functions returned as is.
 *******************************************************************************************************/
static NET_BUF_SIZE NetIF_TxPkt(NET_IF   *p_if,
                                NET_BUF  *p_buf,
                                RTOS_ERR *p_err)
{
  NET_IF_API   *p_if_api;
  NET_DEV_API  *p_dev_api;
  NET_BUF_HDR  *p_buf_hdr;
  CPU_INT08U   *p_data;
  NET_BUF_SIZE size = 0u;

  p_buf_hdr = &p_buf->Hdr;

  //                                                               ------------ PREPARE TX PKT VIA NET IF -------------
  p_if_api = (NET_IF_API *)p_if->IF_API;
  p_if_api->Tx(p_if, p_buf, p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_NET_OP_IN_PROGRESS:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit;

    case RTOS_ERR_NET_ADDR_UNRESOLVED:                          // Tx pending on hw addr; will tx when addr resolved.
    default:
      goto exit;
  }

  //                                                               ------------- WAIT FOR DEV TX RDY SIGNAL -----------
  NetIF_DevTxRdyWait(p_if, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;                                                  // Rtn err from NetIF_Dev_TxRdyWait() [see Note #4].
  }

  //                                                               ------------ PREPARE TX PKT VIA NET DEV ------------
  p_data = &p_buf->DataPtr[p_buf_hdr->IF_HdrIx];
  size = p_buf_hdr->TotLen;
  DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_TX_LOCK);          // Protect tx pkt buf from concurrent access by dev hw.
  NetIF_TxPktListInsert(p_buf);                                 // Insert  tx pkt buf into tx list.

  //                                                               ---------------- TX PKT VIA NET DEV ----------------
  p_dev_api = (NET_DEV_API *)p_if->Dev_API;
  p_dev_api->Tx(p_if, p_data, size, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NetIF_TxPktListRemove(p_buf);                               // See Note #2a.
    goto exit;                                                  // Rtn err from 'pdev_api->Tx()' [see Note #4].
  }

  //                                                               ---------------- RTN TX'D DATA SIZE ----------------
exit:
  return (size);
}

/****************************************************************************************************//**
 *                                           NetIF_TxPktListSrch()
 *
 * @brief    Search Network Interface Transmit List for transmitted network buffer.
 *
 *           - (1) Network buffers whose data areas have been transmitted via network interface(s)/device(s)
 *                 are queued to await acknowledgement of transmission complete & subsequent deallocation.
 *               - (a) Transmitted network buffers are linked to form a Network Interface Transmit List.
 *                     In the diagram below, ... :
 *                   - (1) The horizontal row represents the list of transmitted network buffers.
 *                   - (2) (A) 'NetIF_DataPtr->TxListHead' points to the head of the Network Interface Transmit List;
 *                       - (B) 'NetIF_DataPtr->TxListTail' points to the tail of the Network Interface Transmit List.
 *                   - (3) Network buffers' 'PrevTxListPtr' & 'NextTxListPtr' doubly-link each network buffer
 *                           to form the Network Interface Transmit List.
 *               - (b) (1) (A) For each network buffer data area that has been transmitted, all network buffers
 *                               are searched in order to find (& deallocate) the corresponding network buffer.
 *                       - (B) The network buffer corresponding to the transmitted data area has a network
 *                               interface index into its data area that points to the address of the transmitted
 *                               data area.
 *                   - (2) To expedite faster network buffer searches :
 *                       - (A) (1) Network buffers are added             at the tail of the Network Interface
 *                                       Transmit List;
 *                           - (2) Network buffers are searched starting at the head of the Network Interface
 *                                       Transmit List.
 *                       - (B) As network buffers are added into the list, older network buffers migrate to the
 *                               head of the Network Interface Transmit List.  Once a network buffer's data area
 *                               has been transmitted, the network buffer is removed from the Network Interface
 *                               Transmit List & deallocated.
 *
 *                                       |                                               |
 *                                       |<------ Network Interface Transmit List ------>|
 *                                       |               (see Note #1a1)                 |
 *
 *                           Transmitted network buffers                      New transmit
 *                               awaiting acknowledgement                     network buffers
 *                                   of transmission                          inserted at tail
 *                                   (see Note #1b2A2)                         (see Note #1b2A1)
 *
 *                                         |              NextTxListPtr              |
 *                                         |             (see Note #1a3)             |
 *                                         v                    |                    v
 *                                                              |
 *                   Head of            -------       -------   v   -------       -------       (see Note #1a2B)
 *               etwork Interface  ---->|     |------>|     |------>|     |------>|     |
 *               Transmit Lis t         |     |       |     |       |     |       |     |            Tail of
 *                                      |     |<------|     |<------|     |<------|     |<----  Network Interface
 *               see Note #1a2A)        |     |       |     |   ^   |     |       |     |         Transmit List
 *                                      |     |       |     |   |   |     |       |     |
 *                                      -------       -------   |   -------       -------
 *                                                              |
 *                                                       PrevTxListPtr
 *                                                       (see Note #1a3)
 *
 * @param    p_buf_data  Pointer to a network packet buffer's transmitted data area (see Note #2).
 *
 * @return   Pointer to transmitted data area's network buffer, if found.
 *           Pointer to NULL, otherwise.
 *
 * @note     (2) The network buffer corresponding to the transmitted data area has a network interface
 *               index into its data area that points to the address of the transmitted data area.
 *******************************************************************************************************/
static NET_BUF *NetIF_TxPktListSrch(CPU_INT08U *p_buf_data)
{
  NET_BUF     *p_buf;
  NET_BUF_HDR *p_buf_hdr;
  CPU_INT08U  *p_buf_data_if;
  CPU_BOOLEAN found;

  p_buf = NetIF_DataPtr->TxListHead;                            // Start @ Net IF Tx List head (see Note #1b2A2).
  found = DEF_NO;

  while ((p_buf != DEF_NULL)                                    // Srch    Net IF Tx List ...
         && (found == DEF_NO)) {                                // ... until tx'd pkt buf found.
    p_buf_hdr = &p_buf->Hdr;
    p_buf_data_if = &p_buf->DataPtr[p_buf_hdr->IF_HdrIx];
    //                                                             Cmp tx data area ptrs (see Note #1b1B).
    found = (p_buf_data_if == p_buf_data) ? DEF_YES : DEF_NO;

    if (found != DEF_YES) {                                     // If NOT found, ...
      p_buf = p_buf_hdr->NextTxListPtr;                         // ... adv to next tx pkt buf.
    }
  }

  return (p_buf);
}

/****************************************************************************************************//**
 *                                           NetIF_TxPktListInsert()
 *
 * @brief    Insert a network packet buffer into the Network Interface Transmit List.
 *
 * @param    p_buf   Pointer to a network buffer.
 *
 * @note     (1) Some buffer controls were previously initialized in NetBuf_Get() when the buffer was
 *               allocated.  These buffer controls do NOT need to be re-initialized but are shown for
 *               completeness.
 *******************************************************************************************************/
static void NetIF_TxPktListInsert(NET_BUF *p_buf)
{
  NET_BUF_HDR *p_buf_hdr;
  NET_BUF_HDR *p_buf_hdr_tail;

  //                                                               ----------------- CFG NET BUF PTRS -----------------
  p_buf_hdr = &p_buf->Hdr;
  p_buf_hdr->PrevTxListPtr = NetIF_DataPtr->TxListTail;
#if 0                                                           // Init'd in NetBuf_Get() [see Note #1].
  p_buf_hdr->NextTxListPtr = DEF_NULL;
#endif

  //                                                               -------- INSERT PKT BUF INTO NET IF TX LIST --------
  if (NetIF_DataPtr->TxListTail != DEF_NULL) {                  // If list NOT empty, insert after tail.
    p_buf_hdr_tail = &NetIF_DataPtr->TxListTail->Hdr;
    p_buf_hdr_tail->NextTxListPtr = p_buf;
  } else {                                                      // Else add first pkt buf to list.
    NetIF_DataPtr->TxListHead = p_buf;
  }
  NetIF_DataPtr->TxListTail = p_buf;                            // Insert pkt buf @ list tail (see Note #2).
}

/****************************************************************************************************//**
 *                                           NetIF_TxPktListRemove()
 *
 * @brief    Remove a network packet buffer from the Network Interface Transmit List.
 *
 * @param    p_buf   Pointer to a network buffer.
 *******************************************************************************************************/
static void NetIF_TxPktListRemove(NET_BUF *p_buf)
{
  NET_BUF     *p_buf_list_prev;
  NET_BUF     *p_buf_list_next;
  NET_BUF_HDR *p_buf_hdr;
  NET_BUF_HDR *p_buf_list_prev_hdr;
  NET_BUF_HDR *p_buf_list_next_hdr;

  //                                                               -------- REMOVE PKT BUF FROM NET IF TX LIST --------
  p_buf_hdr = &p_buf->Hdr;
  p_buf_list_prev = p_buf_hdr->PrevTxListPtr;
  p_buf_list_next = p_buf_hdr->NextTxListPtr;

  //                                                               Point prev pkt buf to next pkt buf.
  if (p_buf_list_prev != DEF_NULL) {
    p_buf_list_prev_hdr = &p_buf_list_prev->Hdr;
    p_buf_list_prev_hdr->NextTxListPtr = p_buf_list_next;
  } else {
    NetIF_DataPtr->TxListHead = p_buf_list_next;
  }
  //                                                               Point next pkt buf to prev pkt buf.
  if (p_buf_list_next != DEF_NULL) {
    p_buf_list_next_hdr = &p_buf_list_next->Hdr;
    p_buf_list_next_hdr->PrevTxListPtr = p_buf_list_prev;
  } else {
    NetIF_DataPtr->TxListTail = p_buf_list_prev;
  }

  //                                                               ----------------- CLR NET BUF PTRS -----------------
  p_buf_hdr->PrevTxListPtr = DEF_NULL;
  p_buf_hdr->NextTxListPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                               NetIF_TxPktFree()
 *
 * @brief    (1) Free network buffer :
 *               - (a) Unlock network buffer's transmit lock
 *               - (b) Free   network buffer
 *
 * @param    p_buf   Pointer to network buffer.
 *******************************************************************************************************/
static void NetIF_TxPktFree(NET_BUF *p_buf)
{
  NET_BUF_HDR *p_buf_hdr;

  p_buf_hdr = &p_buf->Hdr;
  //                                                               Clr  net buf's tx lock.
  DEF_BIT_CLR(p_buf_hdr->Flags, (NET_BUF_FLAGS)NET_BUF_FLAG_TX_LOCK);

  (void)NetBuf_FreeBuf(p_buf, DEF_NULL);                        // Free net buf.
}

/****************************************************************************************************//**
 *                                       NetIF_TxSuspendTimeoutInit()
 *
 * @brief    Initialize network interface transmit suspend timeout value.
 *
 * @param    p_if    Pointer to network interface.
 *
 * @note     (1) 'NetIF_TxSuspendTimeout_tick' variables MUST ALWAYS be accessed exclusively
 *               in critical sections.
 *******************************************************************************************************/

#ifdef  NET_LOAD_BAL_MODULE_EN
static void NetIF_TxSuspendTimeoutInit(NET_IF *p_if)
{
  CORE_DECLARE_IRQ_STATE;

  //                                                               Set transmit suspend timeout value (in OS ticks).
  CORE_ENTER_ATOMIC();
  p_if->TxSuspendTimeout_ms = Net_CoreDataPtr->IF_TxSuspendTimeout_ms;
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                           NetIF_TxSuspendSignal()
 *
 * @brief    Signal suspended network interface connection(s)' transmit(s).
 *
 * @param    p_if    Pointer to network interface to signal suspended transmit(s).
 *
 * @note     (1) To balance network receive versus transmit packet loads for certain network connection
 *               types (e.g. stream-type connections), network receive & transmit packets SHOULD be
 *               handled in an APPROXIMATELY balanced ratio.
 *
 *             - (a) Network task priorities & lock mechanisms partially maintain a balanced ratio
 *                   between network receive versus transmit packet handling.
 *                   However, the handling of network receive & transmit packets :
 *                 - (1) SHOULD be interleaved so that for every few packet(s) received & handled,
 *                       several packet(s) should be transmitted; & vice versa.
 *                 - (2) SHOULD NOT exclusively handle receive nor transmit packets, even for a
 *                       short period of time, but especially for a prolonged period of time.
 *             - (b) To implement network receive versus transmit load balancing :
 *                 - (1) The availability of network receive packets MUST be managed for each network
 *                       interface :
 *                     - (A) Increment the number of available network receive packets queued to a
 *                           network interface for each packet received.
 *                     - (B) Decrement the number of available network receive packets queued to a
 *                           network interface for each received packet processed.
 *                 - (2) Certain network connections MUST periodically suspend network transmit(s)
 *                       to handle network interface(s)' receive packet(s) :
 *                     - (A) Suspend network connection transmit(s) if any receive packets are
 *                           available on a network interface.
 *                     - (B) Signal or timeout network connection transmit suspend(s) to restart
 *                           transmit(s).
 *
 * @note     (3) Network interfaces' 'TxSuspendCtr' variables may be accessed with only the global
 *                   network lock acquired & are NOT required to be accessed exclusively in critical
 *                   sections.
 *******************************************************************************************************/

#ifdef  NET_LOAD_BAL_MODULE_EN
static void NetIF_TxSuspendSignal(NET_IF *p_if)
{
  NET_STAT_CTR   *p_stat_ctr;
  NET_CTR        nbr_tx_suspend;
  NET_CTR        i;
  KAL_SEM_HANDLE *p_sem_obj = (KAL_SEM_HANDLE *)p_if->TxSuspendSignalObj;
  RTOS_ERR       local_err;
  CORE_DECLARE_IRQ_STATE;

  p_stat_ctr = &p_if->TxSuspendCtr;
  CORE_ENTER_ATOMIC();
  nbr_tx_suspend = p_stat_ctr->CurCtr;
  CORE_EXIT_ATOMIC();

  for (i = 0u; i < nbr_tx_suspend; i++) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    //                                                             Signal ALL suspended net conn tx's (see Note #1b2B).
    KAL_SemPost(*p_sem_obj, KAL_OPT_PEND_NONE, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}
#endif

/****************************************************************************************************//**
 *                                           NetIF_TxSuspendWait()
 *
 * @brief    Wait on network interface transmit suspend signal.
 *
 * @param    p_if    Pointer to network interface.
 *
 * @note     (1) Network interface transmit suspend waits until :
 *               - (a) Signaled
 *               - (b) Timed out
 *               - (c) Any OS fault occurs
 *
 * @note     (2) Implement timeout with OS-dependent functionality.
 *******************************************************************************************************/

#ifdef  NET_LOAD_BAL_MODULE_EN
static void NetIF_TxSuspendWait(NET_IF *p_if)
{
  CPU_INT32U     timeout_ms;
  KAL_SEM_HANDLE *p_sem_obj = (KAL_SEM_HANDLE *)p_if->TxSuspendSignalObj;
  RTOS_ERR       local_err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  timeout_ms = p_if->TxSuspendTimeout_ms;
  CORE_EXIT_ATOMIC();

  //                                                               Wait on network interface transmit suspend signal.
  KAL_SemPend(*p_sem_obj, KAL_OPT_PEND_NONE, timeout_ms, &local_err);

  PP_UNUSED_PARAM(local_err);                                   // See Note #1c.
}
#endif

/****************************************************************************************************//**
 *                                       NetIF_BufPoolCfgValidate()
 *
 * @brief    (1) Validate network interface buffer pool configuration :
 *               - (a) Validate network interface buffer memory pool types
 *               - (b) Validate configured number of network interface buffers
 *
 * @param    if_nbr      Interface number to initialize network buffer pools.
 *
 *
 * Argument(s) : if_nbr      Interface number to initialize network buffer pools.
 *
 *               p_dev_cfg   Pointer to network interface's device configuration.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) All added network interfaces MUST NOT configure a total number of receive  buffers
 *                   greater than the configured network interface receive queue size.
 *
 *                   All added network interfaces MUST NOT configure a total number of transmit buffers
 *                   greater than the configured network interface transmit deallocation queue size.
 *
 *                   (1) However, since the network loopback interface does NOT deallocate transmit
 *                       packets via the network interface transmit deallocation task (see
 *                       'net_if_loopback.c  NetIF_Loopback_Tx()  Note #4'); then the network interface
 *                       transmit deallocation queue size does NOT need to be adjusted by the network
 *                       loopback interface's number of configured transmit buffers.
 *******************************************************************************************************/
static void NetIF_BufPoolCfgValidate(NET_IF_NBR  if_nbr,
                                     NET_DEV_CFG *p_dev_cfg)
{
  PP_UNUSED_PARAM(if_nbr);

  //                                                               ------------- VALIDATE MEM POOL TYPES --------------
  switch (p_dev_cfg->RxBufPoolType) {                           // Validate rx buf mem pool type.
    case NET_IF_MEM_TYPE_MAIN:
      break;

    case NET_IF_MEM_TYPE_DEDICATED:
      RTOS_ASSERT_DBG((p_dev_cfg->MemAddr != DEF_NULL), RTOS_ERR_INVALID_CFG,; );
      RTOS_ASSERT_DBG((p_dev_cfg->MemSize >= 1), RTOS_ERR_INVALID_CFG,; );
      break;

    default:
      RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_CFG,; );
  }

  switch (p_dev_cfg->TxBufPoolType) {                            // Validate tx buf mem pool type.
    case NET_IF_MEM_TYPE_MAIN:
      break;

    case NET_IF_MEM_TYPE_DEDICATED:
      RTOS_ASSERT_DBG((p_dev_cfg->MemAddr != DEF_NULL), RTOS_ERR_INVALID_CFG,; );
      RTOS_ASSERT_DBG((p_dev_cfg->MemSize >= 1), RTOS_ERR_INVALID_CFG,; );
      break;

    default:
      RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_CFG,; );
  }
}

/****************************************************************************************************//**
 *                                           NetIF_GetDataAlignPtr()
 *
 * @brief    (1) Get aligned pointer into application data buffer :
 *               - (a) Acquire network lock
 *               - (b) Calculate pointer to aligned application data buffer address        See Note #3
 *               - (c) Release network lock
 *               - (d) Return    pointer to aligned application data buffer address
 *                     OR
 *                     Null pointer & error code, on failure
 *
 * @param    if_nbr          Network interface number to get an application buffer's aligned data pointer.
 *
 * @param    transaction     Transaction type :
 *                               - NET_TRANSACTION_RX        Receive  transaction.
 *                               - NET_TRANSACTION_TX        Transmit transaction.
 *
 * @param    p_data          Pointer to application data buffer to get an aligned pointer into (see also
 *                           Note #3c).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to aligned application data buffer address, if available.
 *           Pointer to application data buffer address, if aligned address NOT available.
 *           Pointer to NULL, otherwise.
 *
 * @note     (2) NetIF_GetDataAlignPtr() blocked until network initialization completes.
 *
 * @note     (3) (a) The first aligned address in the application data buffer is calculated based on
 *                   the following equations :
 *               - (1) Addr         =  Addr  %  Word
 *                     Offset                   Size
 *
 *               - (2) Align        =  [ (Word     -  Addr      )  +  ([Ix     + Ix      ]  %  Word    ) ]  %  Word
 *                     Offset            Size        Offset          Data     Offset          Size            Size
 *
 *                                     { (A) Addr  +  Align       , if optimal alignment between application data
 *                                     {                   Offset      buffer & network interface's network buffer
 *                                     {                               data area(s) is  possible (see Note #3b1A)
 *               - (3) Addr         =  {
 *                         Align       { (B) Addr                 , if optimal alignment between application data
 *                                     {                               buffer & network interface's network buffer
 *                                     {                               data area(s) NOT possible (see Note #3b1B)
 *
 *                       where
 *
 *                       - (A) Addr            Application data buffer's address ('p_data')
 *
 *                       - (B) Addr            Non-negative offset from application data buffer's
 *                             Offset          address to previous CPU word-aligned address
 *
 *                       - (C) Align           Non-negative offset from application data buffer's
 *                             Offset          address to first address that is aligned with
 *                                             network interface's network buffer data area(s)
 *
 *                       - (D) Addr            First address in application data buffer that is aligned
 *                             Align           with network interface's network buffer data area(s)
 *
 *                       - (E) Word            CPU's data word size (see 'cpu.h  CPU WORD CONFIGURATION
 *                             Size            Note #1')
 *
 *                       - (F) Ix              Network buffer's base data index (see 'net_buf.h
 *                             Data            NETWORK BUFFER INDEX & SIZE DEFINES  Note #2b')
 *
 *                       - (G) Ix              Network interface's configured network buffer receive/
 *                             Offset          transmit data offset (see 'net_dev_cfg.c  EXAMPLE
 *                                             NETWORK DEVICE CONFIGURATION  Note #5')
 *
 *           - (b) (1) (A) Optimal alignment between application data buffer(s) & network interface's
 *                         network buffer data area(s) is NOT guaranteed & is possible if & only if
 *                         all of the following conditions are true :
 *                       - (1) Network interface's network buffer data area(s) MUST be aligned to a
 *                             multiple of the CPU's data word size (see 'net_buf.h  NETWORK BUFFER
 *                             INDEX & SIZE DEFINES  Note #2b2').
 *                   - (B) Otherwise, a single, fixed alignment between application data buffer(s) &
 *                         network interface's buffer data area(s) is NOT possible.
 *               - (2) (A) Even when application data buffers & network buffer data areas are aligned
 *                         in the best case; optimal alignment is NOT guaranteed for every read/write
 *                         of data to/from application data buffers & network buffer data areas.
 *
 *                         For any single read/write of data to/from application data buffers & network
 *                         buffer data areas, optimal alignment occurs if & only if all of the following
 *                         conditions are true :
 *
 *                       - (1) Data read/written to/from application data buffer(s) to network buffer
 *                             data area(s) MUST start on addresses with the same relative offset from
 *                             CPU word-aligned addresses.
 *                             @n
 *                             In other words, the modulus of the specific read/write address in the
 *                             application data buffer with the CPU's data word size MUST be equal to
 *                             the modulus of the specific read/write address in the network buffer
 *                             data area with the CPU's data word size.
 *                             @n
 *                             This condition MIGHT NOT be satisfied whenever :
 *                           - (a) Data is read/written to/from fragmented packets
 *                           - (b) Data is NOT maximally read/written to/from stream-type packets
 *                                       (e.g. TCP data segments)
 *                           - (c) Packets include variable number of header options (e.g. IP options)
 *                   - (B) However, even though optimal alignment between application data buffers &
 *                         network buffer data areas is NOT guaranteed for every read/write; optimal
 *                         alignment SHOULD occur more frequently leading to improved network data
 *                         throughput.
 *           - (c) Since the first aligned address in the application data buffer may be 0 to
 *                 (CPU_CFG_DATA_SIZE - 1) octets after the application data buffer's starting
 *                 address, the application data buffer SHOULD allocate & reserve an additional
 *                 (CPU_CFG_DATA_SIZE - 1) number of octets.
 *
 *                 However, the application data buffer's effective, useable size is still limited
 *                 to its original declared size (before reserving additional octets) & SHOULD NOT
 *                 be increased by the additional, reserved octets.
 *******************************************************************************************************/
static void *NetIF_GetDataAlignPtr(NET_IF_NBR      if_nbr,
                                   NET_TRANSACTION transaction,
                                   void            *p_data,
                                   RTOS_ERR        *p_err)
{
  NET_IF       *p_if;
  NET_DEV_CFG  *p_dev_cfg;
  void         *p_data_align = DEF_NULL;
  CPU_ADDR     addr;
  NET_BUF_SIZE addr_offset;
  NET_BUF_SIZE align = 0;
  NET_BUF_SIZE ix_align;
  NET_BUF_SIZE ix_offset = 0;
  NET_BUF_SIZE data_ix = 0;
  NET_BUF_SIZE data_size;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_data != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIF_GetDataAlignPtr);

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail_null;
  }

  //                                                               --------------- GET NET IF ALIGN/IX ----------------
  p_dev_cfg = (NET_DEV_CFG *)p_if->Dev_Cfg;
  switch (transaction) {
    case NET_TRANSACTION_RX:
      align = p_dev_cfg->RxBufAlignOctets;
      ix_offset = p_dev_cfg->RxBufIxOffset;
      data_ix = NET_BUF_DATA_IX_RX;
      break;

    case NET_TRANSACTION_TX:
      align = p_dev_cfg->TxBufAlignOctets;
      ix_offset = p_dev_cfg->TxBufIxOffset;
      NetIF_TxIxDataGet(if_nbr,
                        0,
                        &data_ix);
      break;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.InvTransactionTypeCtr);
      Net_GlobalLockRelease();
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
  }

  data_size = CPU_CFG_DATA_SIZE;

  if (((align %  data_size) != 0u)                              // If net  buf align NOT multiple of CPU data word size,
      || (align == 0u)) {
    //                                                             .. data buf align NOT possible (see Note #3b1A1).
    Net_GlobalLockRelease();
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, p_data);
  }

  //                                                               ------------ CALC ALIGN'D DATA BUF PTR -------------
  addr = (CPU_ADDR) p_data;
  addr_offset = (NET_BUF_SIZE)(addr % data_size);               // Calc data addr  offset (see Note #3a1).
                                                                // Calc data align offset (see Note #3a2).
  ix_align = data_ix    +  ix_offset;
  ix_align %= data_size;
  ix_align += data_size  -  addr_offset;
  ix_align %= data_size;

  p_data_align = (void *)((CPU_INT08U *)p_data + ix_align);     // Calc data align'd ptr  (see Note #3a3A).

  goto exit_release;

exit_fail_null:
  p_data_align = DEF_NULL;

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (p_data_align);
}

/****************************************************************************************************//**
 *                                       NetIF_GetProtocolHdrSize()
 *
 * @brief    Get the header length required by protocols.
 *
 * @param    p_if        Pointer to network interface.
 *
 * @param    protocol    Desired protocol layer of network interface MTU.
 *
 * @return   Headers size.
 *******************************************************************************************************/
static CPU_INT16U NetIF_GetProtocolHdrSize(NET_IF            *p_if,
                                           NET_PROTOCOL_TYPE protocol)
{
  NET_IF_API *p_if_api;
  CPU_INT16U hdr_size = 0u;

  if (p_if != DEF_NULL) {
    switch (p_if->Type) {
      case NET_IF_TYPE_NONE:
        break;

      case NET_IF_TYPE_ETHER:
      case NET_IF_TYPE_WIFI:
        hdr_size += NET_IF_HDR_SIZE_ETHER;
        break;

      case NET_IF_TYPE_LOOPBACK:
        hdr_size += NET_IF_HDR_SIZE_LOOPBACK;
        break;

      case NET_IF_TYPE_PPP:
      case NET_IF_TYPE_SERIAL:
      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);
    }
  }

  //                                                               ------------- CALC PROTOCOL LAYER MTU --------------
  switch (protocol) {
    case NET_PROTOCOL_TYPE_LINK:
      if (p_if == DEF_NULL) {
        goto exit;
      }

      p_if_api = (NET_IF_API *)p_if->IF_API;

      hdr_size = p_if_api->GetPktSizeHdr(p_if);
      break;

    case NET_PROTOCOL_TYPE_IF:
    case NET_PROTOCOL_TYPE_IF_FRAME:
    case NET_PROTOCOL_TYPE_IF_ETHER:
    case NET_PROTOCOL_TYPE_IF_IEEE_802:
      break;

    case NET_PROTOCOL_TYPE_ARP:
      hdr_size += NET_ARP_HDR_SIZE;
      break;

    case NET_PROTOCOL_TYPE_IP_V4:
    case NET_PROTOCOL_TYPE_ICMP_V4:
    case NET_PROTOCOL_TYPE_UDP_V4:
    case NET_PROTOCOL_TYPE_IGMP:
    case NET_PROTOCOL_TYPE_TCP_V4:
      hdr_size += NET_IPv4_HDR_SIZE_MIN;
      switch (protocol) {
        case NET_PROTOCOL_TYPE_IP_V4:
        case NET_PROTOCOL_TYPE_ICMP_V4:
          break;

        case NET_PROTOCOL_TYPE_UDP_V4:
          hdr_size += NET_UDP_HDR_SIZE;
          break;

        case NET_PROTOCOL_TYPE_IGMP:
          hdr_size += NET_IGMP_HDR_SIZE;
          break;

        case NET_PROTOCOL_TYPE_TCP_V4:
          hdr_size += NET_TCP_HDR_SIZE_MIN;
          break;

        default:
          break;
      }
      break;

    case NET_PROTOCOL_TYPE_IP_V6:
    case NET_PROTOCOL_TYPE_ICMP_V6:
    case NET_PROTOCOL_TYPE_UDP_V6:
    case NET_PROTOCOL_TYPE_TCP_V6:
      hdr_size += NET_IPv6_HDR_SIZE;
      switch (protocol) {
        case NET_PROTOCOL_TYPE_IP_V6:
        case NET_PROTOCOL_TYPE_ICMP_V6:
          break;

        case NET_PROTOCOL_TYPE_UDP_V6:
          hdr_size += NET_UDP_HDR_SIZE;
          break;

        case NET_PROTOCOL_TYPE_TCP_V6:
          hdr_size += NET_TCP_HDR_SIZE_MIN;
          break;

        default:
          break;
      }
      break;

    case NET_PROTOCOL_TYPE_NONE:
    case NET_PROTOCOL_TYPE_APP:
    case NET_PROTOCOL_TYPE_SOCK:
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);
  }

exit:
  return (hdr_size);
}

/****************************************************************************************************//**
 *                                           NetIF_IO_CtrlHandler()
 *
 * @brief    (1) Handle network interface &/or device specific (I/O) control(s) :
 *               - (a) Device link :
 *                   - (1) Get    device link info
 *                   - (2) Get    device link state
 *                   - (3) Update device link state
 *
 * @param    if_nbr  Network interface number to handle (I/O) controls.
 *
 * @param    opt     Desired I/O control option code to perform; additional control options may be
 *                   defined by the device driver :
 *                   NET_IF_IO_CTRL_LINK_STATE_GET       Get    device's current  physical link state,
 *                                                       'UP' or 'DOWN' (see Note #3).
 *                   NET_IF_IO_CTRL_LINK_STATE_GET_INFO  Get    device's detailed physical link state
 *                                                       information.
 *                   NET_IF_IO_CTRL_LINK_STATE_UPDATE    Update device's current  physical link state.
 *                   NET_IF_IO_CTRL_EEE_GET_INFO         Retrieve information if EEE is enabled or not.
 *                   NET_IF_IO_CTRL_EEE                  Enable/Disable EEE support on the interface.
 *
 * @param    p_data  Pointer to variable that will receive possible I/O control data (see Note #3).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) 'p_data' MUST point to a variable or memory buffer that is sufficiently sized AND
 *               aligned to receive any return data.
 *******************************************************************************************************/
static void NetIF_IO_CtrlHandler(NET_IF_NBR if_nbr,
                                 CPU_INT08U opt,
                                 void       *p_data,
                                 RTOS_ERR   *p_err)
{
  NET_IF     *p_if;
  NET_IF_API *p_if_api;

  //                                                               -------------------- GET NET IF --------------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  //                                                               ------------------ GET NET IF API ------------------
  p_if_api = (NET_IF_API *)p_if->IF_API;

  //                                                               ----------------- HANDLE NET IF I/O ----------------
  p_if_api->IO_Ctrl(p_if, opt, p_data, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIF_PhyLinkStateHandler()
 *
 * @brief    (1) Monitor network interfaces' physical layer link state :
 *               - (a) Poll devices for current link state
 *               - (b) Get Physical Link State Handler timer
 *
 * @param    p_obj   Pointer to this Network Interface Physical Link State Handler function
 *                   (see Note #2).
 *
 * @note     (2) Network timer module requires a pointer to an object when allocating a timer.
 *               However, since the Physical Link State Handler does NOT use or require any
 *               object in order to execute, a NULL object pointer is passed instead.
 *
 * @note     (3) This function is a network timer callback function :
 *               - (a) Clear the timer pointer ... :
 *                     - (1) Reset by NetTmr_Get().
 *               - (b) but do NOT re-free the timer.
 *
 * @note     (4) If a network interface's physical link state cannot be determined, it should NOT
 *               be updated until the interface's physical link state can be correctly determined.
 *               This allows the interface to continue to transmit packets despite any transitory
 *               error(s) in determining network interface's physical link state.
 *               - (a) Network interfaces' 'Link' variables MUST ALWAYS be accessed exclusively with
 *                     the global network lock already acquired.
 *               - (b) Therefore, physical layer link states CANNOT be asynchronously updated by any
 *                     network interface, device driver, or physical layer functions; including  any
 *                     interrupt service routines (ISRs).
 *******************************************************************************************************/
#ifndef  NET_CFG_LINK_STATE_POLL_DISABLED
static void NetIF_PhyLinkStateHandler(void *p_obj)
{
  NET_IF                     *p_if;
  NET_IF_NBR                 if_nbr;
  NET_IF_NBR                 if_nbr_next;
  NET_IF_LINK_STATE          link_state = NET_IF_LINK_UP;
  NET_IF_LINK_SUBSCRIBER_OBJ *p_subsciber_obj;
  RTOS_ERR                   local_err;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(p_obj);                                           // Prevent 'variable unused' warning (see Note #2).

  //                                                               ------- GET ALL NET IF's PHY LINK STATE --------
  if_nbr = NET_IF_NBR_BASE_CFGD;
  CORE_ENTER_ATOMIC();
  if_nbr_next = NetIF_DataPtr->NbrNext;
  CORE_EXIT_ATOMIC();

  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];

  for (; if_nbr < if_nbr_next; if_nbr++) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    if (p_if->En == DEF_ENABLED) {
      NetIF_IO_CtrlHandler(if_nbr,
                           NET_IF_IO_CTRL_LINK_STATE_GET,
                           &link_state,
                           &local_err);

      if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {          // If NO err(s)               [see Note #4a], ...
        p_if->Link = link_state;                                    // ... update IF's link state (see Note #4b).

        if (link_state != p_if->LinkPrev) {                         // If Link state changed since last read ...
                                                                    // ...  notify subscriber.
          p_subsciber_obj = p_if->LinkSubscriberListHeadPtr;

          //                                                       --------------- RELEASE NET LOCK ---------------
          Net_GlobalLockRelease();

          while (p_subsciber_obj != DEF_NULL) {
            if (p_subsciber_obj->Fnct != DEF_NULL) {
              p_subsciber_obj->Fnct(p_if->Nbr, link_state);
            }

            p_subsciber_obj = p_subsciber_obj->NextPtr;
          }

          //                                                       --------------- ACQUIRE NET LOCK ---------------
          Net_GlobalLockAcquire((void *)NetIF_PhyLinkStateHandler);
        }

        //                                                         Don't update previous link state if link state has
        //                                                         change inside the lock release.
        if (p_if->Link == link_state) {
          p_if->LinkPrev = link_state;
        }
      }
    }

    p_if++;
  }
}
#endif

/****************************************************************************************************//**
 *                                           NetIF_PerfMonHandler()
 *
 * @brief    (1) Monitor network interfaces' performance :
 *               - (a) Calculate & update network interfaces' performance statistics
 *               - (b) Get Performance Monitor Handler timer
 *
 * @param    p_obj   Pointer to this Network Interface Performance Monitor Handler function
 *                   (see Note #2).
 *
 * @note     (2) Network timer module requires a pointer to an object when allocating a timer.
 *               However, since the Performance Monitor Handler does NOT use or require any
 *               object in order to execute, a NULL object pointer is passed instead.
 *
 * @note     (3) This function is a network timer callback function :
 *               - (a) Clear the timer pointer ... :
 *                   - (1) Reset by NetTmr_Get().
 *               - (b) but do NOT re-free the timer.
 *******************************************************************************************************/

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
static void NetIF_PerfMonHandler(void *p_obj)
{
  NET_IF           *p_if;
  NET_CTR_IF_STATS *p_if_stats;
  NET_IF_NBR       if_nbr;
  NET_IF_NBR       if_nbr_next;
  NET_CTR          rx_octets_cur;
  NET_CTR          rx_octets_prev;
  NET_CTR          rx_octets_per_sec;
  NET_CTR          rx_pkt_cnt_cur;
  NET_CTR          rx_pkt_cnt_prev;
  NET_CTR          rx_pkt_per_sec;
  NET_CTR          tx_octets_cur;
  NET_CTR          tx_octets_prev;
  NET_CTR          tx_octets_per_sec;
  NET_CTR          tx_pkt_cnt_prev;
  NET_CTR          tx_pkt_cnt_cur;
  NET_CTR          tx_pkt_per_sec;
  NET_TS_MS        ts_ms_cur;
  NET_TS_MS        ts_ms_prev;
  NET_TS_MS        ts_ms_delta;
  CPU_BOOLEAN      update_dlyd;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(p_obj);                                                   // Prevent 'variable unused' (see Note #2).

  //                                                               ------- UPDATE NET IF PERF STATS -------
  if_nbr = NetIF_DataPtr->NbrBase;
  CORE_ENTER_ATOMIC();
  if_nbr_next = NetIF_DataPtr->NbrNext;
  CORE_EXIT_ATOMIC();

  p_if = &NetIF_DataPtr->IF_Tbl[if_nbr];
  p_if_stats = &Net_StatCtrs.IFs.IF[if_nbr];

  ts_ms_cur = NetUtil_TS_Get_ms();

  for (; if_nbr < if_nbr_next; if_nbr++) {
    if (p_if->En == DEF_ENABLED) {
      rx_octets_cur = p_if_stats->RxNbrOctets;
      tx_octets_cur = p_if_stats->TxNbrOctets;
      rx_pkt_cnt_cur = p_if_stats->RxNbrPktCtrProcessed;
      tx_pkt_cnt_cur = p_if_stats->TxNbrPktCtrProcessed;

      update_dlyd = DEF_NO;

      if (p_if->PerfMonState == NET_IF_PERF_MON_STATE_RUN) {                // If perf mon already running &       ...
        ts_ms_prev = p_if->PerfMonTS_Prev_ms;
        if (ts_ms_cur > ts_ms_prev) {                                       // ... cur ts > prev ts,               ...
                                                                            // ... update   perf mon stats :       ...
                                                                            // ... get prev perf mon vals,         ...
          rx_octets_prev = p_if_stats->RxNbrOctetsPrev;
          tx_octets_prev = p_if_stats->TxNbrOctetsPrev;
          rx_pkt_cnt_prev = p_if_stats->RxNbrPktCtrProcessedPrev;
          tx_pkt_cnt_prev = p_if_stats->TxNbrPktCtrProcessedPrev;

          ts_ms_delta = ts_ms_cur - ts_ms_prev;                             // ... calc delta ts (in ms), &        ...

          //                                                       ... calc/update cur perf mon stats; ...
          rx_octets_per_sec = ((rx_octets_cur  - rx_octets_prev) * DEF_TIME_NBR_mS_PER_SEC) / ts_ms_delta;
          tx_octets_per_sec = ((tx_octets_cur  - tx_octets_prev) * DEF_TIME_NBR_mS_PER_SEC) / ts_ms_delta;
          rx_pkt_per_sec = ((rx_pkt_cnt_cur - rx_pkt_cnt_prev) * DEF_TIME_NBR_mS_PER_SEC) / ts_ms_delta;
          tx_pkt_per_sec = ((tx_pkt_cnt_cur - tx_pkt_cnt_prev) * DEF_TIME_NBR_mS_PER_SEC) / ts_ms_delta;

          p_if_stats->RxNbrOctetsPerSec = rx_octets_per_sec;
          p_if_stats->TxNbrOctetsPerSec = tx_octets_per_sec;
          p_if_stats->RxNbrPktCtrPerSec = rx_pkt_per_sec;
          p_if_stats->TxNbrPktCtrPerSec = tx_pkt_per_sec;

          if (p_if_stats->RxNbrOctetsPerSecMax < rx_octets_per_sec) {
            p_if_stats->RxNbrOctetsPerSecMax = rx_octets_per_sec;
          }
          if (p_if_stats->TxNbrOctetsPerSecMax < tx_octets_per_sec) {
            p_if_stats->TxNbrOctetsPerSecMax = tx_octets_per_sec;
          }
          if (p_if_stats->RxNbrPktCtrPerSecMax < rx_pkt_per_sec) {
            p_if_stats->RxNbrPktCtrPerSecMax = rx_pkt_per_sec;
          }
          if (p_if_stats->TxNbrPktCtrPerSecMax < tx_pkt_per_sec) {
            p_if_stats->TxNbrPktCtrPerSecMax = tx_pkt_per_sec;
          }
        } else {                                                            // ... else dly perf mon stats update.
          update_dlyd = DEF_YES;
        }
      }

      if (update_dlyd != DEF_YES) {                                         // If update NOT dly'd, ...
                                                                            // ... save cur stats for next update.
        p_if_stats->RxNbrOctetsPrev = rx_octets_cur;
        p_if_stats->TxNbrOctetsPrev = tx_octets_cur;
        p_if_stats->RxNbrPktCtrProcessedPrev = rx_pkt_cnt_cur;
        p_if_stats->TxNbrPktCtrProcessedPrev = tx_pkt_cnt_cur;

        p_if->PerfMonTS_Prev_ms = ts_ms_cur;
      }

      p_if->PerfMonState = NET_IF_PERF_MON_STATE_RUN;
    } else {
      p_if->PerfMonState = NET_IF_PERF_MON_STATE_STOP;
    }

    p_if++;
    p_if_stats++;
  }
}
#endif

/****************************************************************************************************//**
 *                                   NetIF_IPv4_LinkLocalCompleteHook()
 *
 * @brief    Interface hook function for IPv4 link-local address process ending.
 *
 * @param    if_nbr              Interface number.
 *
 * @param    link_local_addr     IPv4 Link-local address configured, if any.
 *                                   - NET_IPv4_ADDR_NONE, otherwise.
 *
 * @param    status              IPv4 Link-local process status:
 *                                   - NET_IPv4_LINK_LOCAL_STATUS_SUCCEEDED
 *                                   - NET_IPv4_LINK_LOCAL_STATUS_FAILED
 *
 * @param    err                 Error code if error during the IPv4 link-local process.
 *                                   - RTOS_ERR_NONE, otherwise.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static void NetIF_IPv4_LinkLocalCompleteHook(NET_IF_NBR                 if_nbr,
                                             NET_IPv4_ADDR              link_local_addr,
                                             NET_IPv4_LINK_LOCAL_STATUS status,
                                             RTOS_ERR                   err)
{
  NET_IF           *p_if;
  NET_IF_START_OBJ *p_start_obj;
  RTOS_ERR         local_err;
#if LOG_DBG_IS_EN()
  CPU_CHAR addr_str[NET_ASCII_LEN_MAX_ADDR_IPv4];
#endif
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  NET_IF_START_MSG *p_msg;
  NET_IF_APP_INFO  *p_info;
  CORE_DECLARE_IRQ_STATE;
#endif

  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_start_obj = p_if->StartObj;

  if (p_start_obj->IPv4_LinkLocalHook != DEF_NULL) {
    p_start_obj->IPv4_LinkLocalHook(if_nbr, link_local_addr, status, err);
  }

#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  if (KAL_SEM_HANDLE_IS_NULL(p_if->StartObj->SemHandle) != DEF_YES) {
    p_info = p_start_obj->AppInfoPtr;

    p_msg = (NET_IF_START_MSG *)Mem_DynPoolBlkGet(&p_start_obj->StartMsgPool, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    if (p_info != DEF_NULL) {
      p_info->StatusIPv4LinkLocal = status;
      p_info->AddrLinkLocalIPv4 = link_local_addr;
    }

    p_msg->Type = NET_IF_START_MSG_TYPE_IPv4_LINK_LOCAL;
    p_msg->IF_Nbr = if_nbr;

    CORE_ENTER_ATOMIC();
    SList_Push(&p_start_obj->MsgListPtr, &p_msg->ListNode);
    CORE_EXIT_ATOMIC();

    KAL_SemPost(p_start_obj->SemHandle, KAL_OPT_POST_NONE, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
#endif

#if LOG_DBG_IS_EN()
  switch (status) {
    case NET_IPv4_LINK_LOCAL_STATUS_SUCCEEDED:
      if (link_local_addr != NET_IPv4_ADDR_NONE) {
        NetASCII_IPv4_to_Str(link_local_addr, &addr_str[0], DEF_YES, &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          return;
        }
      }
      LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv4 link local address: ", (s)addr_str, ", was configured successfully!"));
      break;

    case NET_IPv4_LINK_LOCAL_STATUS_FAILED:
#if ((RTOS_ERR_CFG_EXT_EN == DEF_ENABLED) \
      && (RTOS_ERR_CFG_STR_EN == DEF_ENABLED))
      LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv4 link local address configuration failed: ", (s)err.CodeText));
#else
      LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv4 link local address configuration failed: ", (u)err.Code));
#endif
      break;

    default:
      break;
  }
#endif
}
#endif

/****************************************************************************************************//**
 *                                       NetIF_DHCPc_CompleteHook()
 *
 * @brief    Interface hook function for DHCP client process ending.
 *
 * @param    if_nbr      Interface number.
 *
 * @param    status      DHCP client status:
 *                           - DHCPc_STATUS_SUCCESS
 *                           - DHCPc_STATUS_FAIL_ADDR_USED
 *                           - DHCPc_STATUS_FAIL_OFFER_DECLINE
 *                           - DHCPc_STATUS_FAIL_NAK_RX
 *                           - DHCPc_STATUS_FAIL_NO_SERVER
 *                           - DHCPc_STATUS_FAIL_ERR_FAULT
 *
 * @param    addr        IPv4 address configured, if any.
 *                       NET_IPv4_ADDR_NONE, otherwise.
 *
 * @param    mask        IPv4 address mask for address configured, if any.
 *                       NET_IPv4_ADDR_NONE, otherwise.
 *
 * @param    gateway     IPv4 address gateway for address configured, if any.
 *                       NET_IPv4_ADDR_NONE, otherwise.
 *
 * @param    err         Error code if error during the DHCP process.
 *                           - RTOS_ERR_NONE, otherwise.
 *******************************************************************************************************/
#ifdef NET_DHCP_CLIENT_MODULE_EN
static void NetIF_DHCPc_CompleteHook(NET_IF_NBR    if_nbr,
                                     DHCPc_STATUS  status,
                                     NET_IPv4_ADDR addr,
                                     NET_IPv4_ADDR mask,
                                     NET_IPv4_ADDR gateway,
                                     RTOS_ERR      err)
{
  NET_IF           *p_if;
  NET_IF_START_OBJ *p_start_obj;
  RTOS_ERR         local_err;
#if LOG_DBG_IS_EN()
  CPU_CHAR addr_str[NET_ASCII_LEN_MAX_ADDR_IPv4];
#endif
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  NET_IF_START_MSG *p_msg;
  NET_IF_APP_INFO  *p_info;
  CORE_DECLARE_IRQ_STATE;
#endif

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_start_obj = p_if->StartObj;

  if (p_start_obj->DHCPc_Hook != DEF_NULL) {
    p_start_obj->DHCPc_Hook(if_nbr, status, addr, mask, gateway, err);
  }

#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  if (KAL_SEM_HANDLE_IS_NULL(p_if->StartObj->SemHandle) != DEF_YES) {
    p_info = p_start_obj->AppInfoPtr;

    p_msg = (NET_IF_START_MSG *)Mem_DynPoolBlkGet(&p_start_obj->StartMsgPool, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    if (p_info != DEF_NULL) {
      p_info->StatusDHCP = status;
      p_info->AddrDynIPv4 = addr;
      p_info->AddrDynMaskIPv4 = mask;
      p_info->AddrDynGatewayIPv4 = gateway;
    }

    p_msg->Type = NET_IF_START_MSG_TYPE_DHCP;
    p_msg->IF_Nbr = if_nbr;

    CORE_ENTER_ATOMIC();
    SList_Push(&p_start_obj->MsgListPtr, &p_msg->ListNode);
    CORE_EXIT_ATOMIC();

    KAL_SemPost(p_start_obj->SemHandle, KAL_OPT_POST_NONE, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
#endif

#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
  if (p_start_obj->IPv4_LinkLocalEn == DEF_YES) {
    if (status == DHCPc_STATUS_SUCCESS) {
      RTOS_ERR rtn_err;

      RTOS_ERR_SET(rtn_err, RTOS_ERR_NONE);

      NetIF_IPv4_LinkLocalCompleteHook(if_nbr, NET_IPv4_ADDR_NONE, NET_IPv4_LINK_LOCAL_STATUS_NONE, rtn_err);
    } else {
      NetIPv4_AddrLinkLocalCfg(if_nbr, NetIF_IPv4_LinkLocalCompleteHook, &local_err);
    }
  }
#endif

#if LOG_DBG_IS_EN()
  switch (status) {
    case DHCPc_STATUS_SUCCESS:
      if (addr != NET_IPv4_ADDR_NONE) {
        NetASCII_IPv4_to_Str(addr, &addr_str[0], DEF_YES, &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          return;
        }
      }
      LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv4 dynamic address: ", (s)addr_str, ", was configured successfully!"));
      break;

    case DHCPc_STATUS_FAIL_ADDR_USED:
      LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv4 dynamic address was already used on the network."));
      break;

    case DHCPc_STATUS_FAIL_OFFER_DECLINE:
      LOG_DBG(("On Interface number #", (u)if_nbr, ", DHCP module declined the server offers."));
      break;

    case DHCPc_STATUS_FAIL_NAK_RX:
      LOG_DBG(("On Interface number #", (u)if_nbr, ", DHCP module received a NAK from the server."));
      break;

    case DHCPc_STATUS_FAIL_NO_SERVER:
      LOG_DBG(("On Interface number #", (u)if_nbr, ", DHCP module did not received any response from servers."));
      break;

    case DHCPc_STATUS_FAIL_ERR_FAULT:
      LOG_DBG(("On Interface number #", (u)if_nbr, ", DHCP module process failed because of an internal error: ", RTOS_ERR_LOG_ARG_GET(err)));
      break;

    case DHCPc_STATUS_NONE:
    case DHCPc_STATUS_LINK_LOCAL:
    case DHCPc_STATUS_IN_PROGRESS:
    default:
      break;
  }
#endif
}
#endif

/****************************************************************************************************//**
 *                                           NetIF_IPv6_AddrHook()
 *
 * @brief    Interface hook function for IPv6 address configuration process.
 *
 * @param    if_nbr              Interface number.
 *
 * @param    addr_type           IPv6 address type:
 *                                   - NET_IPv6_CFG_ADDR_TYPE_STATIC
 *                                   - NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL
 *                                   - NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL
 *
 * @param    p_addr_cfgd         Pointer to IPv6 address configured, if any.
 *                               DEF_NULL, otherwise.
 *
 * @param    addr_cfg_status     Status of the IPv6 address configuration process:
 *                                   - NET_IPv6_ADDR_CFG_STATUS_NOT_DONE
 *                                   - NET_IPv6_ADDR_CFG_STATUS_SUCCEED
 *                                   - NET_IPv6_ADDR_CFG_STATUS_FAIL
 *                                   - NET_IPv6_ADDR_CFG_STATUS_DUPLICATE
 *******************************************************************************************************/
#ifdef NET_IPv6_MODULE_EN
static void NetIF_IPv6_AddrHook(NET_IF_NBR               if_nbr,
                                NET_IPv6_CFG_ADDR_TYPE   addr_type,
                                const NET_IPv6_ADDR      *p_addr_cfgd,
                                NET_IPv6_ADDR_CFG_STATUS addr_cfg_status)
{
  NET_IF           *p_if;
  NET_IF_START_OBJ *p_start_obj;
  RTOS_ERR         local_err;
#if LOG_DBG_IS_EN()
  CPU_CHAR addr_str[NET_ASCII_LEN_MAX_ADDR_IPv6];
#endif
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  NET_IF_START_MSG *p_msg;
  NET_IF_APP_INFO  *p_info;
  CORE_DECLARE_IRQ_STATE;
#endif

  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_start_obj = p_if->StartObj;

  if (p_start_obj->IPv6_Hook != DEF_NULL) {
    p_start_obj->IPv6_Hook(if_nbr, addr_type, p_addr_cfgd, addr_cfg_status);
  }

#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  if (KAL_SEM_HANDLE_IS_NULL(p_if->StartObj->SemHandle) != DEF_YES) {
    p_info = p_start_obj->AppInfoPtr;

    p_msg = (NET_IF_START_MSG *)Mem_DynPoolBlkGet(&p_start_obj->StartMsgPool, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    p_msg->IF_Nbr = if_nbr;

    switch (addr_type) {
      case NET_IPv6_CFG_ADDR_TYPE_STATIC:
        if (p_info != DEF_NULL) {
          p_info->StatusStaticIPv6 = addr_cfg_status;
          Mem_Copy(&p_info->AddrStaticIPv6, p_addr_cfgd, NET_IPv6_ADDR_SIZE);
        }
        p_msg->Type = NET_IF_START_MSG_TYPE_IPv6_STATIC;
        break;

      case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL:
        if (p_info != DEF_NULL) {
          p_info->StatusLinkLocalIPv6 = addr_cfg_status;
          Mem_Copy(&p_info->AddrLinkLocalIPv6, p_addr_cfgd, NET_IPv6_ADDR_SIZE);
        }
        p_msg->Type = NET_IF_START_MSG_TYPE_IPv6_LINK_LOCAL;
        break;

      case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL:
        if (p_info != DEF_NULL) {
          p_info->StatusGlobalIPv6 = addr_cfg_status;
          Mem_Copy(&p_info->AddrGlobalIPv6, p_addr_cfgd, NET_IPv6_ADDR_SIZE);
        }
        p_msg->Type = NET_IF_START_MSG_TYPE_IPv6_GLOBAL;
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_TYPE,; );
    }

    CORE_ENTER_ATOMIC();
    SList_Push(&p_start_obj->MsgListPtr, &p_msg->ListNode);
    CORE_EXIT_ATOMIC();

    KAL_SemPost(p_start_obj->SemHandle, KAL_OPT_POST_NONE, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
#endif

#if LOG_DBG_IS_EN()
  switch (addr_cfg_status) {
    case NET_IPv6_ADDR_CFG_STATUS_SUCCEED:
      if (p_addr_cfgd != DEF_NULL) {
        NetASCII_IPv6_to_Str((NET_IPv6_ADDR *)p_addr_cfgd, &addr_str[0], DEF_NO, DEF_YES, &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          return;
        }
      } else {
        return;
      }
      switch (addr_type) {
        case NET_IPv6_CFG_ADDR_TYPE_STATIC:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 static address: ", (s)addr_str, ", was configured successfully!"));
          break;

        case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 link local address: ", (s)addr_str, ", was configured successfully!"));
          break;

        case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 global address: ", (s)addr_str, ", was configured successfully!"));
          break;

        default:
          break;
      }
      break;

    case NET_IPv6_ADDR_CFG_STATUS_FAIL:
      switch (addr_type) {
        case NET_IPv6_CFG_ADDR_TYPE_STATIC:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 static address configuration failed."));
          break;

        case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 link local address configuration failed."));
          break;

        case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 global address configuration failed."));
          break;

        default:
          break;
      }
      break;

    case NET_IPv6_ADDR_CFG_STATUS_DUPLICATE:
      switch (addr_type) {
        case NET_IPv6_CFG_ADDR_TYPE_STATIC:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 static address configuration failed because address already exists on the network."));
          break;

        case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 link local address configuration failed because address already exists on the network."));
          break;

        case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 global address configuration failed because address already exists on the network."));
          break;

        default:
          break;
      }
      break;

    case NET_IPv6_ADDR_CFG_STATUS_NOT_DONE:
      switch (addr_type) {
        case NET_IPv6_CFG_ADDR_TYPE_STATIC:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 static address configuration was not done."));
          break;

        case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 link local address configuration was not done."));
          break;

        case NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL:
          LOG_DBG(("On Interface number #", (u)if_nbr, ", IPv6 global address configuration was not done."));
          break;

        default:
          break;
      }
      break;

    default:
      break;
  }
#endif
}
#endif

/****************************************************************************************************//**
 *                                       NetIF_WaitSetupLinkUpHook()
 *
 * @brief    Hook function for network interface link up associated with the network interface start
 *           setup wait feature.
 *
 * @param    if_nbr      Network interface number.
 *
 * @param    link_state  Network interface link state:
 *                           - NET_IF_LINK_UP
 *                           - NET_IF_LINK_DOWN
 *******************************************************************************************************/
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
static void NetIF_WaitSetupLinkUpHook(NET_IF_NBR        if_nbr,
                                      NET_IF_LINK_STATE link_state)
{
  NET_IF           *p_if;
  NET_IF_START_OBJ *p_start_obj;
  NET_IF_START_MSG *p_msg;
  RTOS_ERR         local_err;
  CORE_DECLARE_IRQ_STATE;

  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_start_obj = p_if->StartObj;

  if (link_state == NET_IF_LINK_UP) {
    if (KAL_SEM_HANDLE_IS_NULL(p_if->StartObj->SemHandle) != DEF_YES) {
      p_msg = (NET_IF_START_MSG *)Mem_DynPoolBlkGet(&p_start_obj->StartMsgPool, &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      p_msg->IF_Nbr = if_nbr;
      p_msg->Type = NET_IF_START_MSG_TYPE_LINK_UP;

      CORE_ENTER_ATOMIC();
      SList_Push(&p_start_obj->MsgListPtr, &p_msg->ListNode);
      CORE_EXIT_ATOMIC();

      KAL_SemPost(p_start_obj->SemHandle, KAL_OPT_POST_NONE, &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                           NetIF_ValidateAPI()
 *
 * @brief    Validate if there is no Null pointer in the interface API.
 *
 * @param    p_if_api    Pointer to the Interface API to validate
 *
 * @param    type        Interface type
 *                           - NET_IF_TYPE_NONE
 *                           - NET_IF_TYPE_LOOPBACK
 *                           - NET_IF_TYPE_ETHER
 *                           - NET_IF_TYPE_WIFI
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#if  (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIF_ValidateAPI(NET_IF_API  *p_if_api,
                              NET_IF_TYPE type,
                              RTOS_ERR    *p_err)
{
  RTOS_ASSERT_DBG_ERR_SET((p_if_api != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->AddrHW_Get != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->AddrHW_IsValid != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->AddrHW_Set != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->AddrMulticastAdd != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->AddrMulticastProtocolToHW != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->AddrMulticastRemove != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->BufPoolCfgValidate != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->GetPktSizeHdr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->GetPktSizeMin != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->GetPktSizeMax != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->IO_Ctrl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->ISR_Handler != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->MTU_Set != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->Start != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_if_api->Stop != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (type != NET_IF_TYPE_LOOPBACK) {
    RTOS_ASSERT_DBG_ERR_SET((p_if_api->Rx != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
    RTOS_ASSERT_DBG_ERR_SET((p_if_api->Tx != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
