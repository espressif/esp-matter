/***************************************************************************//**
 * @file
 * @brief Network Source File
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
#include  <net/include/net_conn.h>
#include  <net/include/net.h>
#include  <net/include/net_cfg_net.h>
#include  <net/include/dhcp_client.h>
#include  <net/include/dns_client.h>

#include  "net_priv.h"
#include  "net_conn_priv.h"
#include  "net_icmp_priv.h"
#include  "net_tcp_priv.h"
#include  "net_udp_priv.h"
#include  "net_util_priv.h"
#include  "net_task_priv.h"
#include  "../cmd/net_cmd_priv.h"
#include  "../dhcp/dhcp_client_priv.h"
#include  "../dns/dns_client_priv.h"

#ifdef  NET_IPv4_MODULE_EN
#include  <net/include/net_arp.h>
#include  "net_arp_priv.h"
#include  "net_igmp_priv.h"
#endif

#ifdef  NET_IPv6_MODULE_EN
#include  <net/include/net_ndp.h>
#include  "net_ndp_priv.h"
#include  "net_mldp_priv.h"
#endif

#ifdef NET_SECURE_MODULE_EN
#include  "../ssl_tls/net_secure_priv.h"
#endif

#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_path.h>
#include  <common/include/rtos_prio.h>
#include  <net_cfg.h>

#include  "net_bsd_priv.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

#define  NET_LOCK_GLOBAL_NAME               "Net Global Lock"

#define  NET_INIT_CFG_DFLT                  {                               \
    .DNSc_Cfg = DNSc_DFLT_CFG,                                              \
    .CoreStkSizeElements = NET_CORE_TASK_CFG_STK_SIZE_ELEMENTS_DFLT,        \
    .CoreStkPtr = NET_CORE_TASK_CFG_STK_PTR_DFLT,                           \
    .CoreSvcStkSizeElements = NET_CORE_SVC_TASK_CFG_STK_SIZE_ELEMENTS_DFLT, \
    .CoreSvcStkPtr = NET_CORE_SVC_TASK_CFG_STK_PTR_DFLT,                    \
    .MemSegPtr = NET_CORE_MEM_SEG_PTR_DFLT                                  \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

NET_CORE_DATA *Net_CoreDataPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const NET_INIT_CFG  Net_InitCfgDflt = NET_INIT_CFG_DFLT;
static NET_INIT_CFG Net_InitCfg = NET_INIT_CFG_DFLT;
#else
extern const NET_INIT_CFG Net_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Net_InitDflt(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       Net_ConfigureCoreTaskStk()
 *
 * @brief    Configure the network core task stack properties to use the parameters contained in
 *           the passed structure instead of the default parameters.
 *
 * @param    stk_size_elements   Size of the stack, in CPU_STK elements.
 *
 * @param    p_stk_base          Pointer to base of the stack.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Net_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Net_ConfigureCoreTaskStk(CPU_STK_SIZE stk_size_elements,
                              void         *p_stk_base)
{
  RTOS_ASSERT_DBG((Net_CoreDataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  Net_InitCfg.CoreStkSizeElements = stk_size_elements;
  Net_InitCfg.CoreStkPtr = p_stk_base;
}
#endif

/****************************************************************************************************//**
 *                                       Net_ConfigureCoreSvcTaskStk()
 *
 * @brief    Configure the network core services task stack properties to use the parameters contained
 *           in the passed structure instead of the default parameters.
 *
 * @param    stk_size_elements   Size of the stack, in CPU_STK elements.
 *
 * @param    p_stk_base          Pointer to base of the stack.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Net_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Net_ConfigureCoreSvcTaskStk(CPU_STK_SIZE stk_size_elements,
                                 void         *p_stk_base)
{
  RTOS_ASSERT_DBG((Net_CoreDataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  Net_InitCfg.CoreSvcStkSizeElements = stk_size_elements;
  Net_InitCfg.CoreSvcStkPtr = p_stk_base;
}
#endif

/****************************************************************************************************//**
 *                                           Net_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by Net
 *           instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Net_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Net_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_DBG((Net_CoreDataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  Net_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                           Net_ConfigureDNS_Client()
 *
 * @brief    Configure the DNS Client module parameters.
 *
 * @param    p_cfg   Pointer to the structure containing the new DNS client parameters.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Net_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Net_ConfigureDNS_Client(DNSc_CFG *p_cfg)
{
  RTOS_ASSERT_DBG((Net_CoreDataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  Net_InitCfg.DNSc_Cfg = *p_cfg;
}
#endif

/****************************************************************************************************//**
 *                                               Net_Init()
 *
 * @brief    Initialize network core TCP-IP module and task and if necessary the DNS and DHCP client
 *           services.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *
 * @note     (1) NetInit() MUST be called :
 *               - (a) ONLY ONCE from a product's application;
 *               - (b) AFTER: Product's OS has been initialized & Memory library has been initialized.
 *               - (c) BEFORE product's application calls any network protocol suite function(s).
 *
 * @note     (2) If any network initialization error occurs, any remaining network initialization
 *               is immediately aborted & the specific initialization error code is returned.
 *******************************************************************************************************/
void Net_Init(RTOS_ERR *p_err)
{
  CPU_BOOLEAN is_en = DEF_NO;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((Net_CoreDataPtr == DEF_NULL), *p_err, RTOS_ERR_ALREADY_INIT,; );

  //                                                               --------------- VALIDATE OS SERVICE ----------------
  is_en = KAL_FeatureQuery(KAL_FEATURE_DLY, KAL_OPT_DLY_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_PEND_TIMEOUT, KAL_OPT_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_CREATE_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_PEND_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_POST_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_Q_CREATE, KAL_OPT_CREATE_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_Q_PEND, KAL_OPT_PEND_BLOCKING);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_Q_POST, KAL_OPT_POST_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_SEM_CREATE, KAL_OPT_CREATE_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_SEM_PEND, KAL_OPT_PEND_BLOCKING);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_SEM_POST, KAL_OPT_PEND_BLOCKING);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_SEM_ABORT, KAL_OPT_ABORT_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_TASK_CREATE, KAL_OPT_CREATE_NONE);

  RTOS_ASSERT_DBG_ERR_SET((is_en == DEF_YES), *p_err, RTOS_ERR_NOT_AVAIL,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------ CREATE SEGMENT FOR NETWORK MODULE DATA ------
  Net_CoreDataPtr = (NET_CORE_DATA *)Mem_SegAlloc("Net module data segment",
                                                  Net_InitCfg.MemSegPtr,
                                                  sizeof(NET_CORE_DATA),
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail;
  }

  //                                                               -------------- INITIALIZE PARAMETERS ---------------
  Net_CoreDataPtr->IF_NbrTot = NET_IF_CFG_MAX_NBR_IF + NET_IF_NBR_IF_RESERVED;
  Net_CoreDataPtr->CoreMemSegPtr = Net_InitCfg.MemSegPtr;
  Net_CoreDataPtr->IF_TxSuspendTimeout_ms = 1u;

  //                                                               ----------- INITIALIZE THE NETWORK TASK ------------
  NetTask_Init(NET_CORE_TASK_CFG_PRIO_DFLT,
               Net_InitCfg.CoreStkSizeElements,
               Net_InitCfg.CoreStkPtr,
               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------- INITIALIZE CORE MODULES --------------
  NetCore_InitModules(Net_InitCfg.MemSegPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail;
  }

  //                                                               ---------- INITIALIZE SERVICE TASK MODULE ----------
#if (defined(NET_DHCP_CLIENT_MODULE_EN)) \
  || (defined(NET_IPv4_LINK_LOCAL_MODULE_EN))
  //                                                               TODO_NET: defined(NET_DNS_CLIENT_MODULE_EN))
  Net_CoreDataPtr->SvcTaskHandle = NetSvcTask_Init(NET_CORE_SVC_TASK_CFG_PRIO_DFLT,
                                                   Net_InitCfg.CoreSvcStkSizeElements,
                                                   Net_InitCfg.CoreSvcStkPtr,
                                                   Net_InitCfg.MemSegPtr,
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail;
  }
  NetSvcTask_Start(Net_CoreDataPtr->SvcTaskHandle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail;
  }
#endif

  //                                                               -------- INITIALIZE IPV4 LINK LOCAL MODULE ---------
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
  NetIPv4_AddrLinkLocalInit(Net_InitCfg.MemSegPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

  //                                                               ---------- INITIALIZE DHCP CLIENT MODULE -----------
#ifdef NET_DHCP_CLIENT_MODULE_EN
  LOG_VRB(("Initializing DHCP Client"));
  DHCPc_Init(Net_CoreDataPtr->SvcTaskHandle, Net_InitCfg.MemSegPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail;
  }
#endif

  //                                                               ----------- INITIALIZE DNS CLIENT MODULE -----------
#ifdef NET_DNS_CLIENT_MODULE_EN
  LOG_VRB(("Initializing DNS Client"));
  DNSc_Init(&Net_InitCfg.DNSc_Cfg,
            Net_InitCfg.MemSegPtr,
            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail;
  }
#endif

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
  NetCmd_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_fail;
  }
#endif

  goto exit;

exit_fail:
  //                                                               Not yet possible to free Pools, delete task, etc.
exit:
  return;
}

/****************************************************************************************************//**
 *                                           Net_CoreTaskPrioSet()
 *
 * @brief    Sets priority of the network core task.
 *
 * @param    prio    New priority for the network core task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void Net_CoreTaskPrioSet(RTOS_TASK_PRIO prio,
                         RTOS_ERR       *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((Net_CoreDataPtr != DEF_NULL), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  NetTask_PrioSet(prio, p_err);
}

/****************************************************************************************************//**
 *                                           Net_CoreSvcTaskPrioSet()
 *
 * @brief    Sets priority of the network core services task.
 *
 * @param    prio    New priority for the network service task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void Net_CoreSvcTaskPrioSet(RTOS_TASK_PRIO prio,
                            RTOS_ERR       *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((Net_CoreDataPtr != DEF_NULL), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  NetSvcTask_PrioSet(Net_CoreDataPtr->SvcTaskHandle,
                     prio,
                     p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetCore_InitModules()
 *
 * @brief    Initialize all the Network modules on the different layers.
 *
 * @param    p_mem_seg   Pointer to the memory segment used to allocate network core related data.
 *                       If DEF_NULL, the global heap is used.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetCore_InitModules(MEM_SEG  *p_mem_seg,
                         RTOS_ERR *p_err)
{
  Net_CoreDataPtr->GlobaLockFcntPtr = DEF_NULL;
  PP_UNUSED_PARAM(Net_CoreDataPtr->GlobaLockFcntPtr);

  //                                                               ------------ CREATE GLOBAL NETWORK LOCK ------------
  Net_CoreDataPtr->GlobalLock = KAL_LockCreate(NET_LOCK_GLOBAL_NAME,
                                               DEF_NULL,
                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               -------------- INIT NET DEFAULT VALUES -------------
  Net_InitDflt();

  //                                                               ----------------- INIT NET MODULES -----------------
  NetCtr_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  NetStat_Init();

  NetTmr_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  NetBuf_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  NetConn_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  NetIP_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  NetICMP_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               Init net transport layers.
  NetUDP_Init();

#ifdef  NET_TCP_MODULE_EN
  NetTCP_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }
#endif

  //                                                               Init net app layers.
  NetSock_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

#ifdef  NET_SOCK_BSD_EN
  NetBSD_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }
#endif
  //                                                               Init net secure module.
#ifdef  NET_SECURE_MODULE_EN
  NetSecure_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }
#endif

  //                                                               Init   net IF module(s)
  NetIF_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  return;

exit_release:
  KAL_LockDel(Net_CoreDataPtr->GlobalLock);
}

/****************************************************************************************************//**
 *                                           Net_GlobalLockAcquire()
 *
 * @brief    Acquire mutually exclusive access to network protocol suite.
 *
 * @param    p_fcnt  Pointer to the caller.
 *
 * @note     (1) Network access MUST be acquired--i.e. MUST wait for access; do NOT timeout.
 *               Failure to acquire network access will prevent network task(s)/operation(s)
 *               from functioning.
 *
 * @note     (2) Network access MUST be acquired exclusively by only a single task at any one time.
 *******************************************************************************************************/
void Net_GlobalLockAcquire(void *p_fcnt)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  //                                                               Acquire exclusive network access (see Note #1b) ...
  //                                                               ... without timeout              (see Note #1a) ...
  KAL_LockAcquire(Net_CoreDataPtr->GlobalLock, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  Net_CoreDataPtr->GlobaLockFcntPtr = p_fcnt;
}

/****************************************************************************************************//**
 *                                           Net_GlobalLockRelease()
 *
 * @brief    Release mutually exclusive access to network protocol suite.
 *
 * @note     (1) Network access MUST be released--i.e. MUST unlock access without failure.
 *               Failure to release network access will prevent network task(s)/operation(s) from
 *               functioning.  Thus, network access is assumed to be successfully released since
 *               NO Micrium OS Kernel error handling could be performed to counteract failure.
 *******************************************************************************************************/
void Net_GlobalLockRelease(void)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  Net_CoreDataPtr->GlobaLockFcntPtr = DEF_NULL;

  KAL_LockRelease(Net_CoreDataPtr->GlobalLock, &local_err);     // Release exclusive network access.
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               Net_TimeDly()
 *
 * @brief    Delay for specified time, in seconds & microseconds.
 *
 * @param    time_dly_sec    Time delay value, in      seconds (see Note #1).
 *
 * @param    time_dly_us     Time delay value, in microseconds (see Note #1).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Time delay of 0 seconds/microseconds allowed.
 *               - (a) Time delay limited to the maximum possible OS time delay
 *                     if greater than the maximum possible OS time delay.
 *
 * @note     (2) KAL does NOT support microsecond time values :
 *               - (a) Microsecond timeout    values provided for network microsecond timeout values, but
 *                     are rounded to millisecond timeout values.
 *               - (b) Microsecond time delay values NOT supported.
 *
 * @note     (3) To avoid macro integer overflow, an OS timeout tick threshold value MUST be configured
 *               to avoid values that overflow the target CPU &/or compiler environment.
 *******************************************************************************************************/
void Net_TimeDly(CPU_INT32U time_dly_sec,
                 CPU_INT32U time_dly_us,
                 RTOS_ERR   *p_err)
{
  CPU_INT32U time_dly_ms;

  if ((time_dly_sec < 1)                                        // If zero time delay requested, ..
      && (time_dly_us < 1)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                        // .. exit time delay (see Note #1a).
    goto exit;
  }

  //                                                               Calculate us time delay's millisecond value, ..
  //                                                               .. rounded up to next millisecond.
  time_dly_ms = NetUtil_TimeSec_uS_To_ms(time_dly_sec, time_dly_us);
  if (time_dly_ms == NET_TMR_TIME_INFINITE) {
    goto exit;
  }

  //                                                               Delay for calculated time delay.
  KAL_Dly(time_dly_ms);

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               Net_InitDflt()
 *
 * @brief    Initialize default values for network protocol suite configurable parameters.
 *           - (1) Network protocol suite configurable parameters MUST be initialized PRIOR to all other
 *                 network initialization.
 *
 * @note     (2) Ignores configuration functions' return value indicating configuration success/failure.
 *******************************************************************************************************/
static void Net_InitDflt(void)
{
  //                                                               ----------- CFG NET CONN INIT DFLT VALS ------------
  (void)NetConn_CfgAccessedTh(NET_CONN_ACCESSED_TH_DFLT);

  //                                                               -------------- CFG IP INIT DFLT VALS ---------------
#ifdef  NET_IPv4_MODULE_EN
  (void)NetIPv4_CfgFragReasmTimeout(NET_IPv4_FRAG_REASM_TIMEOUT_DFLT_SEC);
#endif

  //                                                               ------------- CFG ICMP INIT DFLT VALS --------------

#ifdef  NET_TCP_MODULE_EN                                       // -------------- CFG TCP INIT DFLT VALS --------------
                                                                // NOT yet implemented (remove if unnecessary).
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
