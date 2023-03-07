/***************************************************************************//**
 * @file
 * @brief Network DHCP Client Module
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

#ifdef NET_DHCP_CLIENT_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  "dhcp_priv.h"

#include  "../tcpip/net_priv.h"
#include  "../tcpip/net_cache_priv.h"
#include  "../tcpip/net_arp_priv.h"
#include  "../tcpip/net_ipv4_priv.h"
#include  "../tcpip/net_sock_priv.h"
#include  "../tcpip/net_util_priv.h"

#include  <net/include/dhcp_client.h>
#include  <net/include/dhcp.h>
#include  <net/include/net_util.h>
#include  <net/include/net_arp.h>
#include  <net/include/net_sock.h>

#include  <net/include/net_type.h>

#include  <net/source/dns/dns_client_priv.h>
#include  <net/source/util/net_svc_task_priv.h>

#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/collections/slist_priv.h>

#ifdef NET_DNS_CLIENT_MODULE_EN
#include  <net/include/dns_client.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                   (NET, DHCP)
#define  RTOS_MODULE_CUR                                RTOS_CFG_MODULE_NET

//                                                                 TODO_NET Add max number of offers to cfg.
#define  DHCPc_OFFER_NBR_MAX                            2u

#define  DHCPc_MSG_NBR_MAX                              2u

#define  DHCPc_TMR_NBR_PER_IF_MAX                       2u
#define  DHCPc_SOCK_NBR_PER_IF_MAX                      1u
#define  DHCPc_MSG_NBR_PER_IF_MAX                       5u

#define  DHCPc_ATTEMPT_RETRY_NBR                        2u
#define  DHCPc_ATTEMPT_RETRY_WAIT_TIME_MS              (10u * DEF_TIME_NBR_mS_PER_SEC)

/********************************************************************************************************
 *                                               TIME DEFINES
 *******************************************************************************************************/

#define  DHCPc_START_DLY_MAX_SEC                       10u
#define  DHCPc_START_DLY_MIN_SEC                        0u

#define  DHCPc_START_RETRY_TIMEOUT_MS                  (1u * DEF_TIME_NBR_mS_PER_SEC)

//                                                                 ARP reply wait time for address validation.
#define  DHCPc_ADDR_VALIDATE_WAIT_TIME_MS              (5u * DEF_TIME_NBR_mS_PER_SEC)

#define  DHCPc_SOCK_WAIT_TIMEOUT_MS                    (5u * DEF_TIME_NBR_mS_PER_SEC)

#define  DHCPc_MIN_TIMEOUT_MS                           1u

#define  DHCPc_RELEASE_ADDRESS_DLY_MS                 500u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           MESSAGE TYPE DATA TYPE
 *******************************************************************************************************/

typedef  enum  dhcpc_msg_type {
  DHCPc_MSG_TYPE_NEW_IF,
  DHCPc_MSG_TYPE_START_IF,
  DHCPc_MSG_TYPE_STOP_IF,
  DHCPc_MSG_TYPE_REBOOT_IF
} DHCPc_MSG_TYPE;

/********************************************************************************************************
 *                                           STATE DATA TYPE
 *******************************************************************************************************/

typedef  enum  dhcpc_state {
  DHCPc_STATE_INIT,
  DHCPc_STATE_SELECTING,
  DHCPc_STATE_REQUESTING,
  DHCPc_STATE_BOUND,
  DHCPc_STATE_RENEWING,
  DHCPc_STATE_REBINDING,
  DHCPc_STATE_INIT_REBOOT,
  DHCPc_STATE_REBOOTING,
  //                                                               Added states for implementation:
  DHCPc_STATE_ADDR_VALIDATION,
  DHCPc_STATE_RELEASE
} DHCPc_STATE;

/********************************************************************************************************
 *                                       SOCKET STATE DATA TYPE
 *******************************************************************************************************/

typedef  enum  dhcpc_sock_state {
  DHCPc_SOCK_STATE_NONE,
  DHCPc_SOCK_STATE_RX,
  DHCPc_SOCK_STATE_TX
} DHCPc_SOCK_STATE;

/********************************************************************************************************
 *                                           DHCP OFFER DATA TYPE
 *******************************************************************************************************/

typedef  struct  dhcpc_offer {
  NET_IPv4_ADDR AddrServer;
  NET_IPv4_ADDR AddrOffered;
  NET_IPv4_ADDR SubnetMask;
  NET_IPv4_ADDR AddrGateway;
  SLIST_MEMBER  ListNode;
} DHCPc_OFFER;

/********************************************************************************************************
 *                                           OBJECT DATA TYPE
 *******************************************************************************************************/

typedef  struct  dhcpc_obj {
  CPU_BOOLEAN             Started;
  NET_IF_NBR              IF_Nbr;
  DHCPc_CFG               Cfg;
  DHCPc_ON_COMPLETE_HOOK  OnCompleteHook;
  DHCPc_STATE             State;
  DHCPc_SOCK_STATE        SockState;
  NET_SOCK_ID             SockID;
  NET_SVC_TASK_TMR_HANDLE SvcTaskTmr;
  NET_SVC_TASK_SOCK       *SvcTaskSockPtr;
  CPU_INT08U              TxRetryCtr;
  CPU_INT08U              AttemptRetryCtr;
  DHCP_MSG                *MsgPtr;
  CPU_INT32U              TransactionID;
  NET_TS_MS               NegoStartTime;
  CPU_INT32U              LeaseTime_sec;
  CPU_INT32U              T1_Time_sec;
  CPU_INT32U              T2_Time_sec;
  SLIST_MEMBER            *OfferListPtr;
  SLIST_MEMBER            ListNode;
  NET_IF_LINK_STATE       LinkState;
  KAL_SEM_HANDLE          StopSem;
} DHCPc_OBJ;

/********************************************************************************************************
 *                                       DHCP CLIENT MODULE DATA TYPE
 *******************************************************************************************************/
typedef  struct  dhcpc_data {
  MEM_DYN_POOL           ObjPool;
  MEM_DYN_POOL           MsgObjPool;
  MEM_DYN_POOL           OfferObjPool;

  SLIST_MEMBER           *ObjListPtr;

  NET_SVC_TASK_CHILD     *SvcTaskChildPtr;
  NET_SVC_TASK_CHILD_CFG SvcTaskChildCfg;
  NET_SVC_TASK_HOOKS     SvcHooks;
} DHCPc_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void DHCPc_HookOnStart(NET_SVC_TASK_CHILD *p_child,
                              RTOS_ERR           *p_err);

static CPU_BOOLEAN DHCPc_HookOnStop(NET_SVC_TASK_CHILD *p_child,
                                    RTOS_ERR           *p_err);

static void DHCPc_HookOnMsg(NET_SVC_TASK_CHILD *p_child,
                            CPU_INT32U         msg_type,
                            void               *p_msg_arg,
                            RTOS_ERR           *p_err);

static void DHCPc_HookOnErr(NET_SVC_TASK_CHILD *p_child,
                            RTOS_ERR           err);

static void DHCPc_ObjAdd(DHCPc_OBJ *p_obj,
                         RTOS_ERR  *p_err);

static void DHCPc_ObjRemove(DHCPc_OBJ *p_obj,
                            RTOS_ERR  *p_err);

static void DHCPc_HookOnSockRx(NET_SVC_TASK_CHILD *p_child,
                               NET_SOCK_ID        sock_id,
                               void               *p_arg,
                               RTOS_ERR           *p_err);

static void DHCPc_HookOnSockTx(NET_SVC_TASK_CHILD *p_child,
                               NET_SOCK_ID        sock_id,
                               void               *p_arg,
                               RTOS_ERR           *p_err);

static void DHCPc_HookOnSockErr(NET_SVC_TASK_CHILD *p_child,
                                NET_SOCK_ID        sock_id,
                                void               *p_dhcpc_obj);

static void DHCPc_HookOnTmrTimeout(NET_SVC_TASK_CHILD *p_child,
                                   void               *p_dhcpc_obj,
                                   RTOS_ERR           *p_err);

static void DHCPc_StartHandler(NET_SVC_TASK_CHILD *p_child,
                               DHCPc_OBJ          *p_obj,
                               RTOS_ERR           *p_err);

static void DHCPc_RetryHandler(NET_SVC_TASK_CHILD *p_child,
                               DHCPc_OBJ          *p_obj,
                               RTOS_ERR           *p_err);

static void DHCPc_StopHandler(NET_SVC_TASK_CHILD *p_child,
                              DHCPc_OBJ          *p_obj,
                              RTOS_ERR           *p_err);

static void DHCPc_FailHandler(NET_SVC_TASK_CHILD *p_child,
                              DHCPc_OBJ          *p_obj,
                              DHCPc_STATUS       status,
                              RTOS_ERR           err);

static void DHCPc_RemoveHandler(NET_SVC_TASK_CHILD *p_child,
                                DHCPc_OBJ          *p_obj,
                                RTOS_ERR           *p_err);

static void DHCPc_TmrHandler(NET_SVC_TASK_CHILD *p_child,
                             DHCPc_OBJ          *p_obj,
                             RTOS_ERR           *p_err);

static NET_SOCK_ID DHCPc_SockInit(NET_SVC_TASK_CHILD *p_child,
                                  DHCPc_CFG          *p_cfg,
                                  RTOS_ERR           *p_err);

static void DHCPc_SockSetTx(DHCPc_OBJ     *p_obj,
                            NET_IPv4_ADDR ip_addr_local,
                            RTOS_ERR      *p_err);

static void DHCPc_SockSetRx(DHCPc_OBJ *p_obj,
                            RTOS_ERR  *p_err);

static void DHCPc_PrepareTx(NET_SVC_TASK_CHILD *p_child,
                            DHCPc_OBJ          *p_obj,
                            DHCP_MSG_TYPE      type,
                            RTOS_ERR           *p_err);

static void DHCPc_PrepareTxRetry(NET_SVC_TASK_CHILD *p_child,
                                 DHCPc_OBJ          *p_obj,
                                 RTOS_ERR           *p_err);

static CPU_INT16U DHCPc_PrepareMsg(DHCPc_OBJ     *p_obj,
                                   DHCP_MSG_TYPE msg_type,
                                   CPU_INT08U    *p_msg_buf,
                                   CPU_INT16U    msg_buf_size,
                                   RTOS_ERR      *p_err);

static DHCPc_STATUS DHCPc_RxHandler(NET_SVC_TASK_CHILD *p_child,
                                    NET_SOCK_ID        sock_id,
                                    DHCPc_OBJ          *p_obj,
                                    DHCP_MSG           *p_msg,
                                    RTOS_ERR           *p_err);

static void DHCPc_RxOffer(DHCPc_OBJ *p_obj,
                          DHCP_MSG  *p_msg,
                          RTOS_ERR  *p_err);

static void DHCPc_RxACK(NET_SVC_TASK_CHILD *p_child,
                        DHCPc_OBJ          *p_obj,
                        DHCP_MSG           *p_msg,
                        RTOS_ERR           *p_err);

static void DHCPc_RxSock(NET_SOCK_ID sock_id,
                         DHCP_MSG    *p_msg,
                         RTOS_ERR    *p_err);

static CPU_BOOLEAN DHCPc_RxValidate(NET_SOCK_ID sock_id,
                                    DHCPc_OBJ   *p_obj,
                                    DHCP_MSG    *p_msg,
                                    RTOS_ERR    *p_err);

static void DHCPc_TxHandler(NET_SVC_TASK_CHILD *p_child,
                            NET_SOCK_ID        sock_id,
                            DHCPc_OBJ          *p_obj,
                            CPU_INT32U         timeout_ms,
                            RTOS_ERR           *p_err);

static void DHCPc_TxSock(NET_SOCK_ID sock_id,
                         DHCPc_OBJ   *p_obj,
                         RTOS_ERR    *p_err);

static CPU_INT08U *DHCPc_MsgOptGet(DHCP_OPT_CODE opt_code,
                                   CPU_INT08U    *p_msg_buf,
                                   CPU_INT16U    msg_buf_size,
                                   CPU_INT08U    *p_opt_val_len);

static void DHCPc_LeaseTimeCalc(DHCPc_OBJ *p_obj,
                                DHCP_MSG  *p_msg,
                                RTOS_ERR  *p_err);

static CPU_BOOLEAN DHCPc_AddrValidate(DHCPc_OBJ *p_obj,
                                      RTOS_ERR  *p_err);

static void DHCPc_LinkSubscribe(NET_IF_NBR        if_nbr,
                                NET_IF_LINK_STATE link_state);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static DHCPc_DATA *DHCPc_DataPtr;

static CPU_BOOLEAN DHCPc_InitDone;

static const DHCP_OPT_CODE DHCPc_ReqParam[] = {
  DHCP_OPT_CODE_SUBNET_MASK,
  DHCP_OPT_CODE_ROUTER,
  DHCP_OPT_CODE_DOMAIN_NAME_SERVER,
  DHCP_OPT_CODE_TIME_OFFSET
};

const DHCPc_CFG DHCPc_CfgDft = {
  DHCPc_CFG_PORT_SERVER_DFLT,
  DHCPc_CFG_PORT_CLIENT_DFLT,
  DHCPc_CFG_TX_RETRY_NBR_DFLT,
  DHCPc_CFG_TX_WAIT_TIMEOUT_MS_DFLT,
  DEF_NO,
#if 0
  DEF_NO,
  DEF_NULL,
  0,
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                            DHCPc_IF_Add()
 *
 * @brief    Attaches an interface to the DHCP module and starts the DHCP process on this interface.
 *
 * @param    if_nbr  Interface number on which DHCP must be perform.
 *
 * @param    p_cfg   Pointer to DHCP configuration for the given interface.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void DHCPc_IF_Add(NET_IF_NBR             if_nbr,
                  DHCPc_CFG              *p_cfg,
                  DHCPc_ON_COMPLETE_HOOK on_complete_hook,
                  RTOS_ERR               *p_err)
{
  NET_IF      *p_if;
  DHCPc_OBJ   *p_obj;
  DHCPc_OBJ   *p_obj_tmp;
  CPU_BOOLEAN found = DEF_NO;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  SLIST_FOR_EACH_ENTRY(DHCPc_DataPtr->ObjListPtr, p_obj, DHCPc_OBJ, ListNode) {
    if (p_obj->IF_Nbr == if_nbr) {
      found = DEF_YES;
      break;
    }
  }

  if (found == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    LOG_ERR(("DHCPc_IF_Add: Interface configuration already added."));
    goto exit;
  }

  //                                                               ---------------- GET DHCP IF OBJECT ----------------
  p_obj = (DHCPc_OBJ *)Mem_DynPoolBlkGet(&DHCPc_DataPtr->ObjPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("DHCPc_IF_Add: failed to allocate data."));
    goto exit;
  }

  SList_Init(&p_obj->OfferListPtr);
  //                                                               Set DHCPc Object Parameters.
  p_obj->State = DHCPc_STATE_INIT;
  p_obj->SockState = DHCPc_SOCK_STATE_NONE;
  p_obj->IF_Nbr = if_nbr;
  p_obj->Started = DEF_NO;
  p_obj->MsgPtr = DEF_NULL;
  p_obj->LeaseTime_sec = 0;
  p_obj->NegoStartTime = 0;
  p_obj->TxRetryCtr = 0;
  p_obj->AttemptRetryCtr = 0;
  p_obj->T1_Time_sec = 0;
  p_obj->T2_Time_sec = 0;
  p_obj->SockID = NET_SOCK_ID_NONE;
  p_obj->OnCompleteHook = on_complete_hook;

  if (p_cfg != DEF_NULL) {
    RTOS_ASSERT_DBG_ERR_SET((p_cfg->ClientPortNbr >= NET_PORT_NBR_MIN), *p_err, RTOS_ERR_INVALID_CFG,; );
    RTOS_ASSERT_DBG_ERR_SET((p_cfg->ServerPortNbr >= NET_PORT_NBR_MIN), *p_err, RTOS_ERR_INVALID_CFG,; );
    RTOS_ASSERT_DBG_ERR_SET((p_cfg->TxTimeout_ms != 0), *p_err, RTOS_ERR_INVALID_CFG,; );
    Mem_Copy(&p_obj->Cfg, p_cfg, sizeof(DHCPc_CFG));
  } else {
    p_obj->Cfg.ServerPortNbr = DHCPc_CFG_PORT_SERVER_DFLT;
    p_obj->Cfg.ClientPortNbr = DHCPc_CFG_PORT_CLIENT_DFLT;
    p_obj->Cfg.TxRetryNbr = DHCPc_CFG_TX_RETRY_NBR_DFLT;
    p_obj->Cfg.TxTimeout_ms = DHCPc_CFG_TX_WAIT_TIMEOUT_MS_DFLT;
    p_obj->Cfg.ValidateAddr = DEF_NO;
  }

  p_obj->TransactionID = NetUtil_RandomRangeGet(DEF_INT_32U_MIN_VAL, (DEF_INT_32U_MAX_VAL - 1));

  //                                                               --------------- GET AND OPEN SOCKET ----------------
  SLIST_FOR_EACH_ENTRY(DHCPc_DataPtr->ObjListPtr, p_obj_tmp, DHCPc_OBJ, ListNode) {
    if ((p_cfg->ClientPortNbr == p_obj_tmp->Cfg.ClientPortNbr)
        && (p_cfg->ServerPortNbr == p_obj_tmp->Cfg.ServerPortNbr)) {
      p_obj->SockID = p_obj_tmp->SockID;
    }
  }

  if (p_obj->SockID == NET_SOCK_ID_NONE) {
    p_obj->SockID = DHCPc_SockInit(DHCPc_DataPtr->SvcTaskChildPtr, &p_obj->Cfg, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_release;
    }
  }

  //                                                               ------ SEND MESSAGE TO START NEW DHCPC IF OBJ ------
  NetSvcTask_ChildMsgPost(DHCPc_DataPtr->SvcTaskChildPtr,
                          DHCPc_MSG_TYPE_NEW_IF,
                          p_obj,
                          DEF_YES,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_obj->LinkState = NetIF_LinkStateGet(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  if (p_obj->LinkState == NET_IF_LINK_UP) {
    NetSvcTask_ChildMsgPost(DHCPc_DataPtr->SvcTaskChildPtr,
                            DHCPc_MSG_TYPE_START_IF,
                            p_obj,
                            DEF_YES,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_release;
    }
  }

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_if->StartModulesCfgFlags.DHCPc = DEF_YES;

  LOG_VRB(("DHCPc_IF_Add: Successful."));
  goto exit;

exit_release:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    Mem_DynPoolBlkFree(&DHCPc_DataPtr->ObjPool, p_obj, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           DHCPc_IF_Remove()
 *
 * @brief    Stops and removes the DHCP operation on the given network interface.
 *
 * @param    if_nbr  Interface number on which DHCP process must be stopped.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void DHCPc_IF_Remove(NET_IF_NBR if_nbr,
                     RTOS_ERR   *p_err)
{
  DHCPc_OBJ   *p_obj = DEF_NULL;
  CPU_BOOLEAN found = DEF_NO;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  SLIST_FOR_EACH_ENTRY(DHCPc_DataPtr->ObjListPtr, p_obj, DHCPc_OBJ, ListNode) {
    if (p_obj->IF_Nbr == if_nbr) {
      found = DEF_YES;
      break;
    }
  }

  if (found == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  p_obj->StopSem = KAL_SemCreate("DHCPc Wait Stop Sem",
                                 DEF_NULL,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  NetSvcTask_ChildMsgPost(DHCPc_DataPtr->SvcTaskChildPtr,
                          DHCPc_MSG_TYPE_STOP_IF,
                          p_obj,
                          DEF_YES,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  KAL_SemPend(p_obj->StopSem,
              KAL_OPT_PEND_BLOCKING,
              KAL_TIMEOUT_INFINITE,
              p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  KAL_SemDel(p_obj->StopSem);

  LOG_VRB(("DHCPc_IF_Remove: Successful."));

exit:
  return;
}

/****************************************************************************************************//**
 *                                           DHCPc_IF_Reboot()
 *
 * @brief    Reboots the DHCP Client process.
 *
 * @param    if_nbr  Interface number on which DHCP process must be rebooted.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_INVALID_STATE
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *
 * @note     (1) The reboot process will be done automatically by the DHCP client process when an
 *               interface link state changes. This API is provided if more flexibility is required by
 *               the customer application.
 *******************************************************************************************************/
void DHCPc_IF_Reboot(NET_IF_NBR if_nbr,
                     RTOS_ERR   *p_err)
{
  DHCPc_OBJ   *p_obj = DEF_NULL;
  CPU_BOOLEAN found = DEF_NO;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  SLIST_FOR_EACH_ENTRY(DHCPc_DataPtr->ObjListPtr, p_obj, DHCPc_OBJ, ListNode) {
    if (p_obj->IF_Nbr == if_nbr) {
      found = DEF_YES;
      break;
    }
  }

  if (found == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  switch (p_obj->State) {
    case DHCPc_STATE_BOUND:
    case DHCPc_STATE_RENEWING:
    case DHCPc_STATE_REBINDING:
    case DHCPc_STATE_INIT_REBOOT:
    case DHCPc_STATE_REBOOTING:
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      goto exit;
  }

  NetSvcTask_ChildMsgPost(DHCPc_DataPtr->SvcTaskChildPtr,
                          DHCPc_MSG_TYPE_REBOOT_IF,
                          p_obj,
                          DEF_YES,
                          p_err);
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
 *                                           DHCPc_Init()
 *
 * @brief    Initializes the DHCP Client Module.
 *
 * @param    task_handle     The Service task handle which to attach the DHCP Client module.
 *
 * @param    p_mem_seg       Memory segment from which internal data will be allocated.
 *                           If DEF_NULL, it will be allocated from the global heap.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function.
 *******************************************************************************************************/
void DHCPc_Init(NET_SVC_TASK_HANDLE task_handle,
                MEM_SEG             *p_mem_seg,
                RTOS_ERR            *p_err)
{
  CPU_BOOLEAN            is_init;
  NET_SVC_TASK_CHILD_CFG *p_child_cfg;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((task_handle != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ---- VALIDATE THAT DHCPc IS NOT ALREADY STARTED ----
  CORE_ENTER_ATOMIC();
  is_init = DHCPc_InitDone;
  CORE_EXIT_ATOMIC();

  if (is_init == DEF_YES) {
    goto exit;
  }

  DHCPc_DataPtr = (DHCPc_DATA *)Mem_SegAlloc("DHCP Client Module Data Seg",
                                             p_mem_seg,
                                             sizeof(DHCPc_DATA),
                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("DHCPc_Init: failed to allocate data."));
    goto exit;
  }

  //                                                               ----------- INITIALIZE DHCPC OBJECT LIST -----------
  SList_Init(&DHCPc_DataPtr->ObjListPtr);

  //                                                               ----------- CREATE POOL OF DHCPC OBJECT ------------
  Mem_DynPoolCreate("DHCPc Object Pool",
                    &DHCPc_DataPtr->ObjPool,
                    p_mem_seg,
                    sizeof(DHCPc_OBJ),
                    sizeof(CPU_ALIGN),
                    NET_IF_CFG_MAX_NBR_IF,
                    NET_IF_CFG_MAX_NBR_IF,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("DHCPc_Init: failed to allocate data."));
    goto exit;
  }

  //                                                               ------- CREATE POOL FOR DHCP MESSAGE OBJECT --------
  Mem_DynPoolCreate("DHCPc Msg Object Pool",
                    &DHCPc_DataPtr->MsgObjPool,
                    p_mem_seg,
                    sizeof(DHCP_MSG),
                    sizeof(CPU_ALIGN),
                    (DHCPc_MSG_NBR_MAX * NET_IF_CFG_MAX_NBR_IF),
                    (DHCPc_MSG_NBR_MAX * NET_IF_CFG_MAX_NBR_IF),
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("DHCPc_Init: failed to allocate data."));
    goto exit;
  }

  //                                                               -------- CREATE POOL FOR DHCP OFFER OBJECT ---------
  Mem_DynPoolCreate("DHCPc Offer Object Pool",
                    &DHCPc_DataPtr->OfferObjPool,
                    p_mem_seg,
                    sizeof(DHCPc_OFFER),
                    sizeof(CPU_ALIGN),
                    (DHCPc_OFFER_NBR_MAX * NET_IF_CFG_MAX_NBR_IF),
                    (DHCPc_OFFER_NBR_MAX * NET_IF_CFG_MAX_NBR_IF),
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("DHCPc_Init: failed to allocate data."));
    goto exit;
  }

  p_child_cfg = &DHCPc_DataPtr->SvcTaskChildCfg;
  p_child_cfg->MemSegPtr = p_mem_seg;
  p_child_cfg->TmrNbrMax = NET_IF_CFG_MAX_NBR_IF * DHCPc_TMR_NBR_PER_IF_MAX;
  p_child_cfg->SockNbrMax = NET_IF_CFG_MAX_NBR_IF * DHCPc_SOCK_NBR_PER_IF_MAX;
  p_child_cfg->MsgNbrMax = NET_IF_CFG_MAX_NBR_IF * DHCPc_MSG_NBR_PER_IF_MAX;

  //                                                               ------- CREATE SERVICE TASK CHILD FOR DHCPC --------
  DHCPc_DataPtr->SvcHooks.OnStart = DHCPc_HookOnStart;
  DHCPc_DataPtr->SvcHooks.OnStop = DHCPc_HookOnStop;
  DHCPc_DataPtr->SvcHooks.OnMsg = DHCPc_HookOnMsg;
  DHCPc_DataPtr->SvcHooks.OnErr = DHCPc_HookOnErr;

  DHCPc_DataPtr->SvcTaskChildPtr = NetSvcTask_ChildStart(task_handle,
                                                         p_child_cfg,
                                                         DEF_NULL,
                                                         &DHCPc_DataPtr->SvcHooks,
                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Unable to start the DHCPc child process."));
    goto exit;
  }

  LOG_VRB(("DHCPc_Init: Successful."));

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        DHCPc_HookOnStart()
 *
 * @brief    The start process hook function for the DHCP Network Service Task Child.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_HookOnStart(NET_SVC_TASK_CHILD *p_child,
                              RTOS_ERR           *p_err)
{
  PP_UNUSED_PARAM(p_child);
  PP_UNUSED_PARAM(p_err);
}

/****************************************************************************************************//**
 *                                        DHCPc_HookOnStop()
 *
 * @brief    The stop process hook function for the DHCP Network Service Task Child.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static CPU_BOOLEAN DHCPc_HookOnStop(NET_SVC_TASK_CHILD *p_child,
                                    RTOS_ERR           *p_err)
{
  PP_UNUSED_PARAM(p_child);
  PP_UNUSED_PARAM(p_err);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                         DHCPc_HookOnMsg()
 *
 * @brief    The hook function for a message received  by the Network Service Task and to be process
 *           by the DHCP client child.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    msg_type    The message type.
 *
 * @param    p_msg_arg   Pointer to the message arguments.
 *
 * @param    p_err       Error Pointer.
 *******************************************************************************************************/
static void DHCPc_HookOnMsg(NET_SVC_TASK_CHILD *p_child,
                            CPU_INT32U         msg_type,
                            void               *p_msg_arg,
                            RTOS_ERR           *p_err)
{
  DHCPc_OBJ  *p_obj;
  CPU_INT32U delay_ms;
  CPU_INT32U min;
  CPU_INT32U max;

  switch (msg_type) {
    case DHCPc_MSG_TYPE_NEW_IF:
      p_obj = (DHCPc_OBJ *)p_msg_arg;
      RTOS_ASSERT_CRITICAL_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

      DHCPc_ObjAdd(p_obj, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_fail;
      }

      NetIF_LinkStateSubscribe(p_obj->IF_Nbr,
                               DHCPc_LinkSubscribe,
                               p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_fail;
      }
      break;

    case DHCPc_MSG_TYPE_START_IF:
      p_obj = (DHCPc_OBJ *)p_msg_arg;
      RTOS_ASSERT_CRITICAL_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

      switch (p_obj->State) {
        case DHCPc_STATE_INIT:
        case DHCPc_STATE_SELECTING:
        case DHCPc_STATE_REQUESTING:
        case DHCPc_STATE_ADDR_VALIDATION:
          min = DHCPc_START_DLY_MIN_SEC * DEF_TIME_NBR_mS_PER_SEC;
          max = DHCPc_START_DLY_MAX_SEC * DEF_TIME_NBR_mS_PER_SEC;

          delay_ms = NetUtil_RandomRangeGet(min, max);

          if (p_obj->SvcTaskTmr == DEF_NULL) {
            p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                                     NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                                     DHCPc_HookOnTmrTimeout,
                                                     p_obj,
                                                     delay_ms,
                                                     p_err);
            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              goto exit_fail;
            }
          } else {
            NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                              DHCPc_HookOnTmrTimeout,
                              p_obj,
                              delay_ms,
                              p_err);
            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              goto exit_fail;
            }
          }

          if (p_obj->SvcTaskSockPtr == DEF_NULL) {
            //                                                     --- ADD SERVICE TASK SOCK FOR THE DHCPC IF OBJ ----
            p_obj->SvcTaskSockPtr = NetSvcTask_SockHandleCreate(p_child,
                                                                p_obj->SockID,
                                                                NET_SVC_TASK_SOCK_OP_TYPE_NONE,
                                                                DHCPc_HookOnSockRx,
                                                                DHCPc_HookOnSockTx,
                                                                DHCPc_HookOnSockErr,
                                                                p_obj,
                                                                p_err);
            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              goto exit_fail;
            }
          }
          break;

        case DHCPc_STATE_BOUND:
        case DHCPc_STATE_RENEWING:
        case DHCPc_STATE_REBINDING:
        case DHCPc_STATE_RELEASE:
        case DHCPc_STATE_INIT_REBOOT:
        case DHCPc_STATE_REBOOTING:
        default:
          DHCPc_FailHandler(p_child, p_obj, DHCPc_STATUS_FAIL_ERR_FAULT, *p_err);
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }

      p_obj->State = DHCPc_STATE_INIT;
      break;

    case DHCPc_MSG_TYPE_STOP_IF:
      p_obj = (DHCPc_OBJ *)p_msg_arg;
      RTOS_ASSERT_CRITICAL_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

      switch (p_obj->State) {
        case DHCPc_STATE_INIT:
        case DHCPc_STATE_SELECTING:
        case DHCPc_STATE_REQUESTING:
        case DHCPc_STATE_INIT_REBOOT:
        case DHCPc_STATE_REBOOTING:
        case DHCPc_STATE_RELEASE:
        case DHCPc_STATE_ADDR_VALIDATION:
          DHCPc_RemoveHandler(p_child, p_obj, p_err);
          break;

        case DHCPc_STATE_BOUND:
        case DHCPc_STATE_RENEWING:
        case DHCPc_STATE_REBINDING:
          DHCPc_PrepareTx(p_child, p_obj, DHCP_MSG_RELEASE, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit_fail;
          }
          p_obj->State = DHCPc_STATE_RELEASE;
          break;

        default:
          DHCPc_FailHandler(p_child, p_obj, DHCPc_STATUS_FAIL_ERR_FAULT, *p_err);
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }

      break;

    case DHCPc_MSG_TYPE_REBOOT_IF:
      p_obj = (DHCPc_OBJ *)p_msg_arg;
      RTOS_ASSERT_CRITICAL_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

      p_obj->State = DHCPc_STATE_INIT_REBOOT;

      if (p_obj->SvcTaskTmr != DEF_NULL) {
        NetSvcTask_TmrDel(p_obj->SvcTaskTmr, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
        p_obj->SvcTaskTmr = DEF_NULL;
      }

      if (p_obj->SvcTaskSockPtr != DEF_NULL) {
        p_obj->SvcTaskSockPtr->OpType = NET_SVC_TASK_SOCK_OP_TYPE_NONE;
        NetSvcTask_SockHandleDel(p_child, p_obj->SvcTaskSockPtr, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
      }

      NetIPv4_CfgAddrAddDynamicStop(p_obj->IF_Nbr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      NetIPv4_CfgAddrAddDynamicStart(p_obj->IF_Nbr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }

      p_obj->SvcTaskSockPtr = NetSvcTask_SockHandleCreate(p_child,
                                                          p_obj->SockID,
                                                          NET_SVC_TASK_SOCK_OP_TYPE_NONE,
                                                          DHCPc_HookOnSockRx,
                                                          DHCPc_HookOnSockTx,
                                                          DHCPc_HookOnSockErr,
                                                          p_obj,
                                                          p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_fail;
      }

      DHCPc_PrepareTx(p_child, p_obj, DHCP_MSG_REQUEST, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_fail;
      }
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  goto exit;

exit_fail:
  DHCPc_FailHandler(p_child, p_obj, DHCPc_STATUS_FAIL_ERR_FAULT, *p_err);

exit:
  return;
}

/****************************************************************************************************//**
 *                                         DHCPc_HookOnErr()
 *
 * @brief    The error process hook function for the DHCP Client Network Service Task Child.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_HookOnErr(NET_SVC_TASK_CHILD *p_child,
                            RTOS_ERR           err)
{
  PP_UNUSED_PARAM(p_child);
  PP_UNUSED_PARAM(err);
}

/****************************************************************************************************//**
 *                                          DHCPc_ObjAdd()
 *
 * @brief    Add an DHCPc-Interface object to the global list.
 *
 * @param    p_obj   Pointer to the DHCPc object to add to the list.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void DHCPc_ObjAdd(DHCPc_OBJ *p_obj,
                         RTOS_ERR  *p_err)
{
  PP_UNUSED_PARAM(p_err);

  SList_Push(&DHCPc_DataPtr->ObjListPtr, &p_obj->ListNode);
}

/****************************************************************************************************//**
 *                                         DHCPc_ObjRemove()
 *
 * @brief    Remove an DHCPc-Interface object from the global list.
 *
 * @param    p_obj   Pointer to the DHCPc object to remove from the list.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void DHCPc_ObjRemove(DHCPc_OBJ *p_obj,
                            RTOS_ERR  *p_err)
{
  PP_UNUSED_PARAM(p_err);

  SList_Rem(&DHCPc_DataPtr->ObjListPtr, &p_obj->ListNode);
}

/****************************************************************************************************//**
 *                                       DHCPc_HookOnSockRx()
 *
 * @brief    The Hook function called when data is received on the socket associated with the
 *           DHCP client process.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    sock_id     Socket ID for the DHCP Client process.
 *
 * @param    p_arg       Pointer to context arguments.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_HookOnSockRx(NET_SVC_TASK_CHILD *p_child,
                               NET_SOCK_ID        sock_id,
                               void               *p_arg,
                               RTOS_ERR           *p_err)
{
  DHCP_MSG      *p_msg;
  DHCPc_OBJ     *p_obj = DEF_NULL;
  DHCPc_OFFER   *p_offer;
  DHCPc_STATUS  status = DHCPc_STATUS_NONE;
  NET_IPv4_ADDR addr;
  NET_IPv4_ADDR mask;
  NET_IPv4_ADDR gateway;
  CPU_BOOLEAN   is_cfgd;

  PP_UNUSED_PARAM(p_arg);

  //                                                               -------------- GET DHCP MESSAGE BLOCK --------------
  p_msg = (DHCP_MSG *)Mem_DynPoolBlkGet(&DHCPc_DataPtr->MsgObjPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
    goto exit_fail;
  }

  //                                                               -------------- RETRIEVE RECEIVED DATA --------------
  DHCPc_RxSock(sock_id, p_msg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  SLIST_FOR_EACH_ENTRY(DHCPc_DataPtr->ObjListPtr, p_obj, DHCPc_OBJ, ListNode) {
    if (p_obj->SockState != DHCPc_SOCK_STATE_RX) {              // If Sock state is not in RX, continue to next object.
      continue;
    }

    if (p_obj->TransactionID != p_msg->TransactionID) {         // If Transaction ID is no match, continue.
      continue;
    }

    p_obj->TxRetryCtr = 0;

    switch (p_obj->State) {
      case DHCPc_STATE_INIT:
        break;

      case DHCPc_STATE_SELECTING:
        if (p_msg->Type == DHCP_MSG_OFFER) {
          status = DHCPc_RxHandler(p_child, sock_id, p_obj, p_msg, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit_fail;
          }
        }
        break;

      case DHCPc_STATE_REQUESTING:
        status = DHCPc_RxHandler(p_child, sock_id, p_obj, p_msg, p_err);
        switch (status) {
          case DHCPc_STATUS_SUCCESS:
            p_offer = SLIST_ENTRY(p_obj->OfferListPtr, DHCPc_OFFER, ListNode);
            NET_UTIL_VAL_COPY_GET_NET_32(&addr, &p_offer->AddrOffered);
            NET_UTIL_VAL_COPY_GET_NET_32(&mask, &p_offer->SubnetMask);
            NET_UTIL_VAL_COPY_GET_NET_32(&gateway, &p_offer->AddrGateway);
            is_cfgd = NetIPv4_CfgAddrAddDynamic(p_obj->IF_Nbr,
                                                addr,
                                                mask,
                                                gateway,
                                                p_err);
            if (is_cfgd != DEF_YES) {
              status = DHCPc_STATUS_FAIL_ERR_FAULT;
              goto exit_fail;
            }
            break;

          case DHCPc_STATUS_IN_PROGRESS:
            break;

          case DHCPc_STATUS_FAIL_NAK_RX:
            p_obj->AttemptRetryCtr++;
            if (p_obj->AttemptRetryCtr >= DHCPc_ATTEMPT_RETRY_NBR) {
              goto exit_fail;
            }
            p_obj->State = DHCPc_STATE_INIT;
            p_obj->SvcTaskSockPtr->OpType = NET_SVC_TASK_SOCK_OP_TYPE_NONE;
            p_obj->SockState = DHCPc_SOCK_STATE_NONE;
            if (p_obj->SvcTaskTmr == DEF_NULL) {
              p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                                       NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                                       DHCPc_HookOnTmrTimeout,
                                                       p_obj,
                                                       DHCPc_ATTEMPT_RETRY_WAIT_TIME_MS,
                                                       p_err);
              if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                status = DHCPc_STATUS_FAIL_ERR_FAULT;
                goto exit_fail;
              }
            } else {
              NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                                DHCPc_HookOnTmrTimeout,
                                p_obj,
                                DHCPc_ATTEMPT_RETRY_WAIT_TIME_MS,
                                p_err);
              if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                status = DHCPc_STATUS_FAIL_ERR_FAULT;
                goto exit_fail;
              }
            }
            break;

          default:
            goto exit_fail;
        }
        break;

      case DHCPc_STATE_BOUND:
        break;

      case DHCPc_STATE_RENEWING:
        status = DHCPc_RxHandler(p_child, sock_id, p_obj, p_msg, p_err);
        switch (status) {
          case DHCPc_STATUS_SUCCESS:
            p_offer = SLIST_ENTRY(p_obj->OfferListPtr, DHCPc_OFFER, ListNode);
            NET_UTIL_VAL_COPY_GET_NET_32(&addr, &p_offer->AddrOffered);
            NET_UTIL_VAL_COPY_GET_NET_32(&mask, &p_offer->SubnetMask);
            NET_UTIL_VAL_COPY_GET_NET_32(&gateway, &p_offer->AddrGateway);
            break;

          case DHCPc_STATUS_IN_PROGRESS:
            break;

          case DHCPc_STATUS_FAIL_NAK_RX:
            p_obj->AttemptRetryCtr++;
            if (p_obj->AttemptRetryCtr >= DHCPc_ATTEMPT_RETRY_NBR) {
              goto exit_fail;
            }
            p_obj->State = DHCPc_STATE_INIT;
            p_obj->SvcTaskSockPtr->OpType = NET_SVC_TASK_SOCK_OP_TYPE_NONE;
            p_obj->SockState = DHCPc_SOCK_STATE_NONE;
            if (p_obj->SvcTaskTmr == DEF_NULL) {
              p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                                       NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                                       DHCPc_HookOnTmrTimeout,
                                                       p_obj,
                                                       DHCPc_ATTEMPT_RETRY_WAIT_TIME_MS,
                                                       p_err);
              if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                status = DHCPc_STATUS_FAIL_ERR_FAULT;
                goto exit_fail;
              }
            } else {
              NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                                DHCPc_HookOnTmrTimeout,
                                p_obj,
                                DHCPc_ATTEMPT_RETRY_WAIT_TIME_MS,
                                p_err);
              if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                status = DHCPc_STATUS_FAIL_ERR_FAULT;
                goto exit_fail;
              }
            }
            break;

          default:
            goto exit_fail;
        }
        break;

      case DHCPc_STATE_REBINDING:
        status = DHCPc_RxHandler(p_child, sock_id, p_obj, p_msg, p_err);
        switch (status) {
          case DHCPc_STATUS_SUCCESS:
            p_offer = SLIST_ENTRY(p_obj->OfferListPtr, DHCPc_OFFER, ListNode);
            NET_UTIL_VAL_COPY_GET_NET_32(&addr, &p_offer->AddrOffered);
            NET_UTIL_VAL_COPY_GET_NET_32(&mask, &p_offer->SubnetMask);
            NET_UTIL_VAL_COPY_GET_NET_32(&gateway, &p_offer->AddrGateway);
            break;

          case DHCPc_STATUS_IN_PROGRESS:
            break;

          case DHCPc_STATUS_FAIL_NAK_RX:
            p_obj->AttemptRetryCtr++;
            if (p_obj->AttemptRetryCtr >= DHCPc_ATTEMPT_RETRY_NBR) {
              goto exit_fail;
            }
            p_obj->State = DHCPc_STATE_INIT;
            p_obj->SvcTaskSockPtr->OpType = NET_SVC_TASK_SOCK_OP_TYPE_NONE;
            p_obj->SockState = DHCPc_SOCK_STATE_NONE;
            if (p_obj->SvcTaskTmr == DEF_NULL) {
              p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                                       NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                                       DHCPc_HookOnTmrTimeout,
                                                       p_obj,
                                                       DHCPc_ATTEMPT_RETRY_WAIT_TIME_MS,
                                                       p_err);
              if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                status = DHCPc_STATUS_FAIL_ERR_FAULT;
                goto exit_fail;
              }
            } else {
              NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                                DHCPc_HookOnTmrTimeout,
                                p_obj,
                                DHCPc_ATTEMPT_RETRY_WAIT_TIME_MS,
                                p_err);
              if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                status = DHCPc_STATUS_FAIL_ERR_FAULT;
                goto exit_fail;
              }
            }
            break;

          default:
            goto exit_fail;
        }
        break;

      case DHCPc_STATE_INIT_REBOOT:
        break;

      case DHCPc_STATE_REBOOTING:
        status = DHCPc_RxHandler(p_child, sock_id, p_obj, p_msg, p_err);
        switch (status) {
          case DHCPc_STATUS_SUCCESS:
            p_offer = SLIST_ENTRY(p_obj->OfferListPtr, DHCPc_OFFER, ListNode);
            NET_UTIL_VAL_COPY_GET_NET_32(&addr, &p_offer->AddrOffered);
            NET_UTIL_VAL_COPY_GET_NET_32(&mask, &p_offer->SubnetMask);
            NET_UTIL_VAL_COPY_GET_NET_32(&gateway, &p_offer->AddrGateway);
            is_cfgd = NetIPv4_CfgAddrAddDynamic(p_obj->IF_Nbr,
                                                addr,
                                                mask,
                                                gateway,
                                                p_err);
            if (is_cfgd != DEF_YES) {
              status = DHCPc_STATUS_FAIL_ERR_FAULT;
              goto exit_fail;
            }
            break;

          case DHCPc_STATUS_IN_PROGRESS:
            break;

          case DHCPc_STATUS_FAIL_NAK_RX:
            p_obj->AttemptRetryCtr++;
            p_obj->State = DHCPc_STATE_INIT;
            if (p_obj->SvcTaskTmr == DEF_NULL) {
              p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                                       NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                                       DHCPc_HookOnTmrTimeout,
                                                       p_obj,
                                                       DHCPc_MIN_TIMEOUT_MS,
                                                       p_err);
              if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                status = DHCPc_STATUS_FAIL_ERR_FAULT;
                goto exit_fail;
              }
            } else {
              NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                                DHCPc_HookOnTmrTimeout,
                                p_obj,
                                DHCPc_MIN_TIMEOUT_MS,
                                p_err);
              if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                status = DHCPc_STATUS_FAIL_ERR_FAULT;
                goto exit_fail;
              }
            }
            break;

          default:
            DHCPc_FailHandler(p_child, p_obj, status, *p_err);
            Mem_DynPoolBlkFree(&DHCPc_DataPtr->MsgObjPool, p_msg, p_err);
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        }
        break;

      case DHCPc_STATE_RELEASE:
        break;

      default:
        DHCPc_FailHandler(p_child, p_obj, status, *p_err);
        Mem_DynPoolBlkFree(&DHCPc_DataPtr->MsgObjPool, p_msg, p_err);
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    if (status == DHCPc_STATUS_SUCCESS) {
      if (p_obj->OnCompleteHook != DEF_NULL) {
        p_obj->OnCompleteHook(p_obj->IF_Nbr, status, addr, mask, gateway, *p_err);
      }
    }
  }

  goto exit_release;

exit_fail:
  DHCPc_FailHandler(p_child, p_obj, status, *p_err);

exit_release:
  Mem_DynPoolBlkFree(&DHCPc_DataPtr->MsgObjPool, p_msg, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                       DHCPc_HookOnSockTx()
 *
 * @brief    The Hook function called when the socket associated with the DHCP client process is ready
 *           to transmit.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    sock_id     Socket ID for the DHCP Client process.
 *
 * @param    p_arg       Pointer to context arguments.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_HookOnSockTx(NET_SVC_TASK_CHILD *p_child,
                               NET_SOCK_ID        sock_id,
                               void               *p_arg,
                               RTOS_ERR           *p_err)
{
  DHCPc_OBJ    *p_obj;
  DHCPc_CFG    *p_cfg;
  DHCPc_STATUS status;

  PP_UNUSED_PARAM(p_arg);

  SLIST_FOR_EACH_ENTRY(DHCPc_DataPtr->ObjListPtr, p_obj, DHCPc_OBJ, ListNode) {
    p_cfg = &p_obj->Cfg;

    if (p_obj->SockState != DHCPc_SOCK_STATE_TX) {          // If Sock state is not in TX, continue to next object.
      continue;
    }

    switch (p_obj->State) {
      case DHCPc_STATE_INIT:
        DHCPc_TxHandler(p_child, sock_id, p_obj, p_cfg->TxTimeout_ms, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }
        p_obj->State = DHCPc_STATE_SELECTING;
        break;

      case DHCPc_STATE_SELECTING:
        DHCPc_TxHandler(p_child, sock_id, p_obj, p_cfg->TxTimeout_ms, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }
        p_obj->State = DHCPc_STATE_REQUESTING;
        break;

      case DHCPc_STATE_REQUESTING:
      case DHCPc_STATE_ADDR_VALIDATION:
        DHCPc_TxSock(sock_id, p_obj, p_err);                        // Send DECLINE
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }
        p_obj->AttemptRetryCtr++;
        if (p_obj->AttemptRetryCtr >= DHCPc_ATTEMPT_RETRY_NBR) {
          status = DHCPc_STATUS_FAIL_OFFER_DECLINE;
          goto exit_fail;
        }
        p_obj->State = DHCPc_STATE_INIT;
        p_obj->SvcTaskSockPtr->OpType = NET_SVC_TASK_SOCK_OP_TYPE_NONE;
        p_obj->SockState = DHCPc_SOCK_STATE_NONE;
        if (p_obj->SvcTaskTmr == DEF_NULL) {
          p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                                   NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                                   DHCPc_HookOnTmrTimeout,
                                                   p_obj,
                                                   DHCPc_ATTEMPT_RETRY_WAIT_TIME_MS,
                                                   p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            status = DHCPc_STATUS_FAIL_ERR_FAULT;
            goto exit_fail;
          }
        } else {
          NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                            DHCPc_HookOnTmrTimeout,
                            p_obj,
                            DHCPc_ATTEMPT_RETRY_WAIT_TIME_MS,
                            p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            status = DHCPc_STATUS_FAIL_ERR_FAULT;
            goto exit_fail;
          }
        }
        break;

      case DHCPc_STATE_BOUND:
        DHCPc_TxHandler(p_child, sock_id, p_obj, (p_obj->T2_Time_sec * DEF_TIME_NBR_mS_PER_SEC), p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }
        p_obj->State = DHCPc_STATE_RENEWING;
        break;

      case DHCPc_STATE_RENEWING:
        DHCPc_TxHandler(p_child, sock_id, p_obj, (p_obj->LeaseTime_sec * DEF_TIME_NBR_mS_PER_SEC), p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }
        p_obj->State = DHCPc_STATE_REBINDING;
        break;

      case DHCPc_STATE_REBINDING:
        status = DHCPc_STATUS_FAIL_ERR_FAULT;
        DHCPc_FailHandler(p_child, p_obj, status, *p_err);
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        break;

      case DHCPc_STATE_INIT_REBOOT:
        DHCPc_TxHandler(p_child, sock_id, p_obj, p_cfg->TxTimeout_ms, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }
        p_obj->State = DHCPc_STATE_REBOOTING;
        break;

      case DHCPc_STATE_RELEASE:
        DHCPc_TxSock(sock_id, p_obj, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }
        //                                                         Set timer to delay the IP address release.
        if (p_obj->SvcTaskTmr == DEF_NULL) {
          p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                                   NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                                   DHCPc_HookOnTmrTimeout,
                                                   p_obj,
                                                   DHCPc_RELEASE_ADDRESS_DLY_MS,
                                                   p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
        } else {
          NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                            DHCPc_HookOnTmrTimeout,
                            p_obj,
                            DHCPc_RELEASE_ADDRESS_DLY_MS,
                            p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
        }
        p_obj->SvcTaskSockPtr->OpType = NET_SVC_TASK_SOCK_OP_TYPE_RX_RDY;
        p_obj->SockState = DHCPc_SOCK_STATE_RX;
        break;

      case DHCPc_STATE_REBOOTING:
      default:
        status = DHCPc_STATUS_FAIL_ERR_FAULT;
        DHCPc_FailHandler(p_child, p_obj, status, *p_err);
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }
  }
  goto exit;

exit_fail:
  DHCPc_FailHandler(p_child, p_obj, status, *p_err);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       DHCPc_HookOnSockErr()
 *
 * @brief    The Hook function called when an error occurred on the socket associated with the DHCP
 *           client.
 *
 * @param    p_child         Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    sock_id         Socket ID for the DHCP Client process.
 *
 * @param    p_dhcpc_obj     Pointer to current DHCPc-Interface object.
 *******************************************************************************************************/
static void DHCPc_HookOnSockErr(NET_SVC_TASK_CHILD *p_child,
                                NET_SOCK_ID        sock_id,
                                void               *p_dhcpc_obj)
{
  PP_UNUSED_PARAM(p_child);
  PP_UNUSED_PARAM(sock_id);
  PP_UNUSED_PARAM(p_dhcpc_obj);
}

/****************************************************************************************************//**
 *                                     DHCPc_HookOnTmrTimeout()
 *
 * @brief    The hook function for a timed out timer by the Network Service Task and to be process
 *           by the DHCP client child.
 *
 * @param    p_child         Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_dhcpc_obj     Pointer to current DHCPc-Interface object.
 *
 * @param    p_err           Error Pointer.
 *******************************************************************************************************/
static void DHCPc_HookOnTmrTimeout(NET_SVC_TASK_CHILD *p_child,
                                   void               *p_dhcpc_obj,
                                   RTOS_ERR           *p_err)
{
  DHCPc_OBJ     *p_obj;
  DHCPc_OFFER   *p_offer;
  DHCPc_STATUS  status;
  NET_IPv4_ADDR addr;
  NET_IPv4_ADDR mask;
  NET_IPv4_ADDR gateway;
  CPU_BOOLEAN   is_valid;

  p_obj = (DHCPc_OBJ *)p_dhcpc_obj;
  RTOS_ASSERT_CRITICAL((p_obj != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  p_obj->SvcTaskTmr = DEF_NULL;

  switch (p_obj->State) {
    case DHCPc_STATE_INIT:
      if (p_obj->AttemptRetryCtr == 0) {
        DHCPc_StartHandler(p_child, p_obj, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }
      } else {
        DHCPc_RetryHandler(p_child, p_obj, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }
      }
      break;

    case DHCPc_STATE_SELECTING:
    case DHCPc_STATE_REQUESTING:
    case DHCPc_STATE_BOUND:
    case DHCPc_STATE_RENEWING:
      DHCPc_TmrHandler(p_child, p_obj, p_err);
      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_NONE:
          break;

        case RTOS_ERR_NET_RETRY_MAX:
          status = DHCPc_STATUS_FAIL_NO_SERVER;
          goto exit_fail;

        default:
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
      }
      break;

    case DHCPc_STATE_REBINDING:
      LOG_ERR(("DHCP client address renewal failed."));        // TODO_NET restart DHCP client process
      status = DHCPc_STATUS_FAIL_NO_SERVER;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit_fail;

    case DHCPc_STATE_INIT_REBOOT:
    case DHCPc_STATE_REBOOTING:
      DHCPc_TmrHandler(p_child, p_obj, p_err);
      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_NONE:
          break;

        case RTOS_ERR_NET_RETRY_MAX:
          status = DHCPc_STATUS_FAIL_NO_SERVER;
          goto exit_fail;

        default:
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
      }
      break;

    case DHCPc_STATE_ADDR_VALIDATION:
      //                                                           ------ CHECK IF IP ADDRESS ALREADY ON NETWORK ------
      is_valid = DHCPc_AddrValidate(p_obj, p_err);
      if (is_valid == DEF_NO) {
        DHCPc_PrepareTx(p_child, p_obj, DHCP_MSG_DECLINE, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }
      } else {
        p_offer = SLIST_ENTRY(p_obj->OfferListPtr, DHCPc_OFFER, ListNode);
        NET_UTIL_VAL_COPY_GET_NET_32(&addr, &p_offer->AddrOffered);
        NET_UTIL_VAL_COPY_GET_NET_32(&mask, &p_offer->SubnetMask);
        NET_UTIL_VAL_COPY_GET_NET_32(&gateway, &p_offer->AddrGateway);
        is_valid = NetIPv4_CfgAddrAddDynamic(p_obj->IF_Nbr, addr, mask, gateway, p_err);
        if (is_valid != DEF_OK) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }

        p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                                 NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                                 DHCPc_HookOnTmrTimeout,
                                                 p_obj,
                                                 p_obj->T1_Time_sec * DEF_TIME_NBR_mS_PER_SEC,
                                                 p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          status = DHCPc_STATUS_FAIL_ERR_FAULT;
          goto exit_fail;
        }

        p_obj->State = DHCPc_STATE_BOUND;

        if (p_obj->OnCompleteHook != DEF_NULL) {
          p_obj->OnCompleteHook(p_obj->IF_Nbr, DHCPc_STATUS_SUCCESS, addr, mask, gateway, *p_err);
        }
      }
      break;

    case DHCPc_STATE_RELEASE:
      DHCPc_RemoveHandler(p_child, p_obj, p_err);
      break;

    default:
      status = DHCPc_STATUS_FAIL_ERR_FAULT;
      DHCPc_FailHandler(p_child, p_obj, status, *p_err);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  goto exit;

exit_fail:
  DHCPc_FailHandler(p_child, p_obj, status, *p_err);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       DHCPc_StartHandler()
 *
 * @brief    Do the operations required by the DHCP process to start properly.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_StartHandler(NET_SVC_TASK_CHILD *p_child,
                               DHCPc_OBJ          *p_obj,
                               RTOS_ERR           *p_err)
{
  //                                                               ------ CHECK IF TIMEOUT CAME FROM THE TX PEND ------
  if (p_obj->SockState == DHCPc_SOCK_STATE_TX) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
    goto exit;
  }

  //                                                               --------- CHECK IF DHCP IS ALREADY STARTED ---------
  if (p_obj->Started == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    goto exit;
  }

  //                                                               ---------------- START DYNAMIC CFG -----------------
  NetIPv4_CfgAddrAddDynamicStart(p_obj->IF_Nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ---------- PREPARE DISCOVER DHCP MESSAGE -----------
  DHCPc_PrepareTx(p_child, p_obj, DHCP_MSG_DISCOVER, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_obj->Started = DEF_YES;

exit:
  return;
}

/****************************************************************************************************//**
 *                                         DHCPc_RetryHandler()
 *
 * @brief    Retry the DHCP client process after declining the offer or receiving a NACK message.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_RetryHandler(NET_SVC_TASK_CHILD *p_child,
                               DHCPc_OBJ          *p_obj,
                               RTOS_ERR           *p_err)
{
  DHCPc_OFFER  *p_offer;
  SLIST_MEMBER *p_node;

  //                                                               ------ CHECK IF TIMEOUT CAME FROM THE TX PEND ------
  if (p_obj->SockState == DHCPc_SOCK_STATE_TX) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
    goto exit;
  }

  //                                                               -------------- CLEAR PREVIOUS OFFERS ---------------
  while (p_obj->OfferListPtr != DEF_NULL) {
    p_node = SList_Pop(&p_obj->OfferListPtr);

    p_offer = SLIST_ENTRY(p_node, DHCPc_OFFER, ListNode);

    Mem_DynPoolBlkFree(&DHCPc_DataPtr->OfferObjPool, p_offer, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               ---------------- START DYNAMIC CFG -----------------
  NetIPv4_CfgAddrAddDynamicStart(p_obj->IF_Nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------- PREPARE DISCOVER MESSAGE -------------
  DHCPc_PrepareTx(p_child, p_obj, DHCP_MSG_DISCOVER, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                        DHCPc_StopHandler()
 *
 * @brief    Stop the DHCP process for the given DHCPc-Interface object.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_StopHandler(NET_SVC_TASK_CHILD *p_child,
                              DHCPc_OBJ          *p_obj,
                              RTOS_ERR           *p_err)
{
  DHCPc_OFFER  *p_offer;
  SLIST_MEMBER *p_node;

  if (p_obj->SvcTaskSockPtr != DEF_NULL) {
    p_obj->SvcTaskSockPtr->OpType = NET_SVC_TASK_SOCK_OP_TYPE_NONE;
    NetSvcTask_SockHandleDel(p_child, p_obj->SvcTaskSockPtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    p_obj->SvcTaskSockPtr = DEF_NULL;
  }

  if (p_obj->SvcTaskTmr != DEF_NULL) {
    NetSvcTask_TmrDel(p_obj->SvcTaskTmr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    p_obj->SvcTaskTmr = DEF_NULL;
  }

  while (p_obj->OfferListPtr != DEF_NULL) {
    p_node = SList_Pop(&p_obj->OfferListPtr);

    p_offer = SLIST_ENTRY(p_node, DHCPc_OFFER, ListNode);

    Mem_DynPoolBlkFree(&DHCPc_DataPtr->OfferObjPool, p_offer, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  if (p_obj->MsgPtr != DEF_NULL ) {
    Mem_DynPoolBlkFree(&DHCPc_DataPtr->MsgObjPool, p_obj->MsgPtr, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  p_obj->MsgPtr = DEF_NULL;

  p_obj->State = DHCPc_STATE_INIT;
  p_obj->Started = DEF_NO;
  p_obj->AttemptRetryCtr = 0;

  NetIPv4_CfgAddrAddDynamicStop(p_obj->IF_Nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                        DHCPc_FailHandler()
 *
 * @brief    (1) Operations required when the DHCP Client process fails:
 *               - (a) Call the application hook to notify that the DHCP process has finished.
 *               - (b) Stop the DHCP Client process.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    err         Error object with the error code that caused the fail.
 *******************************************************************************************************/
static void DHCPc_FailHandler(NET_SVC_TASK_CHILD *p_child,
                              DHCPc_OBJ          *p_obj,
                              DHCPc_STATUS       status,
                              RTOS_ERR           err)
{
  RTOS_ERR local_err;

  //                                                               -------------------- CALL HOOK ---------------------
  if (p_obj->OnCompleteHook != DEF_NULL) {
    p_obj->OnCompleteHook(p_obj->IF_Nbr,
                          status,
                          NET_IPv4_ADDR_NONE,
                          NET_IPv4_ADDR_NONE,
                          NET_IPv4_ADDR_NONE,
                          err);
  }

  //                                                               ----------- STOP DHCP PROCESS AND CLEAR ------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  DHCPc_StopHandler(p_child, p_obj, &local_err);
}

/****************************************************************************************************//**
 *                                         DHCPc_RemoveHandler()
 *
 * @brief    Stop the DHCP process and remove the DHCP object from the list.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    p_err       Error object with the error code that caused the fail.
 *******************************************************************************************************/
static void DHCPc_RemoveHandler(NET_SVC_TASK_CHILD *p_child,
                                DHCPc_OBJ          *p_obj,
                                RTOS_ERR           *p_err)
{
  DHCPc_OBJ      *p_obj_tmp;
  KAL_SEM_HANDLE sem_temp;

  //                                                               -------------- STOP THE DHCP PROCESS ---------------
  DHCPc_StopHandler(p_child, p_obj, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               --------- CLOSE SOCKET IF NOT USED ANYMORE ---------
  SLIST_FOR_EACH_ENTRY(DHCPc_DataPtr->ObjListPtr, p_obj_tmp, DHCPc_OBJ, ListNode) {
    if (p_obj != p_obj_tmp) {
      if (p_obj->SockID == p_obj_tmp->SockID) {
        p_obj->SockID = NET_SOCK_ID_NONE;
      }
    }
  }

  if (p_obj->SockID != NET_SOCK_ID_NONE) {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetSock_Close(p_obj->SockID, &local_err);
  }

  //                                                               ---- REMOVE CALLBACK FROM IF LINK SUBSCRIPTION -----
  NetIF_LinkStateUnsubscribe(p_obj->IF_Nbr,
                             DHCPc_LinkSubscribe,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit_release:
  //                                                               ------ REMOVE DHCP OBJECT FROM LIST AND FREE -------
  sem_temp = p_obj->StopSem;

  DHCPc_ObjRemove(p_obj, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  Mem_DynPoolBlkFree(&DHCPc_DataPtr->ObjPool, p_obj, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  if (KAL_SEM_HANDLE_IS_NULL(sem_temp) == DEF_NO) {
    KAL_SemPost(sem_temp, KAL_OPT_POST_NONE, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}

/****************************************************************************************************//**
 *                                        DHCPc_TmrHandler()
 *
 * @brief    DHCP Client process after a timer timed out.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_TmrHandler(NET_SVC_TASK_CHILD *p_child,
                             DHCPc_OBJ          *p_obj,
                             RTOS_ERR           *p_err)
{
  //                                                               ------ CHECK IF TIMEOUT CAME FROM THE TX PEND ------
  if (p_obj->SockState == DHCPc_SOCK_STATE_TX) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
    goto exit;
  }

  switch (p_obj->State) {
    case DHCPc_STATE_SELECTING:
      //                                                           --------- CHECK IF ANY DHCP OFFER RECEIVED ---------
      if (p_obj->OfferListPtr == DEF_NULL) {
        //                                                         if no offer received, try sending discover again.
        DHCPc_PrepareTxRetry(p_child, p_obj, p_err);
        p_obj->State = DHCPc_STATE_INIT;
      } else {
        //                                                         else prepare request message.
        DHCPc_PrepareTx(p_child, p_obj, DHCP_MSG_REQUEST, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
      }
      break;

    case DHCPc_STATE_REQUESTING:
      //                                                           -------------- RETRY SENDING REQUEST ---------------
      DHCPc_PrepareTxRetry(p_child, p_obj, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      p_obj->State = DHCPc_STATE_SELECTING;
      break;

    case DHCPc_STATE_BOUND:
    case DHCPc_STATE_RENEWING:
    case DHCPc_STATE_INIT_REBOOT:
      //                                                           ------------- PREPARE REQUEST MESSAGE --------------
      DHCPc_PrepareTx(p_child, p_obj, DHCP_MSG_REQUEST, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

    case DHCPc_STATE_REBOOTING:
      //                                                           -------------- RETRY SENDING REQUEST ---------------
      DHCPc_PrepareTxRetry(p_child, p_obj, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      p_obj->State = DHCPc_STATE_INIT_REBOOT;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           DHCPc_InitSock()
 *
 * @brief    Initialize and open a new socket for the DHCP Client process.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_cfg       Pointer to the DHCP configuration.
 *
 * @param    p_err       Error pointer.
 *
 * @return   ID for the new socket to use, if no errors.
 *           NET_SOCK_ID_NONE, otherwise.
 *******************************************************************************************************/
static NET_SOCK_ID DHCPc_SockInit(NET_SVC_TASK_CHILD *p_child,
                                  DHCPc_CFG          *p_cfg,
                                  RTOS_ERR           *p_err)
{
  NET_SOCK_ID        sock_id;
  NET_SOCK_ADDR_IPv4 local_addr;
  NET_FLAGS          flags = DEF_BIT_NONE;

  Mem_Clr(&local_addr, sizeof(NET_SOCK_ADDR_IPv4));

  local_addr.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
  local_addr.Addr = NET_IPv4_ADDR_THIS_HOST;
  local_addr.Port = NET_UTIL_HOST_TO_NET_16(p_cfg->ClientPortNbr);

  DEF_BIT_SET(flags, NET_SVC_TASK_SOCK_FLAG_BLOCK);
  DEF_BIT_SET(flags, NET_SVC_TASK_SOCK_FLAG_BIND);

  sock_id = NetSvc_SockCreateClient(p_child,
                                    NET_SOCK_PROTOCOL_FAMILY_IP_V4,
                                    NET_SOCK_TYPE_DATAGRAM,
                                    NET_SOCK_PROTOCOL_UDP,
                                    flags,
                                    (NET_SOCK_ADDR *)&local_addr,
                                    sizeof(NET_SOCK_ADDR_IPv4),
                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (sock_id);
}

/****************************************************************************************************//**
 *                                         DHCPc_SockSetTx()
 *
 * @brief    Configure the network socket before transmission.
 *
 * @param    p_obj           Pointer to current DHCPc-Interface object.
 *
 * @param    ip_addr_local   IPv4 address to use as source address for transmission.
 *
 * @param    p_err           Error pointer
 *******************************************************************************************************/
static void DHCPc_SockSetTx(DHCPc_OBJ     *p_obj,
                            NET_IPv4_ADDR ip_addr_local,
                            RTOS_ERR      *p_err)
{
  DHCPc_CFG          *p_cfg;
  NET_SOCK_ADDR_IPv4 local_addr;

  p_cfg = &p_obj->Cfg;

  //                                                               ------------ SET IF NBR FOR THE SOCKET ------------
  NetSock_CfgIF(p_obj->SockID,
                p_obj->IF_Nbr,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  Mem_Clr(&local_addr, sizeof(local_addr));
  local_addr.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
  local_addr.Addr = ip_addr_local;
  local_addr.Port = NET_UTIL_HOST_TO_NET_16(p_cfg->ClientPortNbr);

  NetSock_Bind(p_obj->SockID,
               (NET_SOCK_ADDR *) &local_addr,
               sizeof(local_addr),
               p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  goto exit;

exit_close:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetSock_Close(p_obj->SockID, &local_err);
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           DHCPc_SockSetRx()
 *
 * @brief    Set the DHCP socket to bind to the any address for reception purposes.
 *
 * @param    p_obj   Pointer to current DHCPc-Interface object.
 *
 * @param    p_err   Error pointer
 *******************************************************************************************************/
static void DHCPc_SockSetRx(DHCPc_OBJ *p_obj,
                            RTOS_ERR  *p_err)
{
  DHCPc_CFG          *p_cfg;
  NET_SOCK_ADDR_IPv4 local_addr;

  p_cfg = &p_obj->Cfg;

  //                                                               ------------ SET IF NBR FOR THE SOCKET ------------
  NetSock_CfgIF(p_obj->SockID,
                p_obj->IF_Nbr,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  Mem_Clr(&local_addr, sizeof(local_addr));
  local_addr.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
  local_addr.Addr = NET_IPv4_ADDR_THIS_HOST;
  local_addr.Port = NET_UTIL_HOST_TO_NET_16(p_cfg->ClientPortNbr);

  NetSock_Bind(p_obj->SockID,
               (NET_SOCK_ADDR *)&local_addr,
               sizeof(local_addr),
               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  goto exit;

exit_close:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetSock_Close(p_obj->SockID, &local_err);
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                        DHCPc_PrepareTx()
 *
 * @brief    Prepare for the DHCP message transmission.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    type        DHCP message type.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_PrepareTx(NET_SVC_TASK_CHILD *p_child,
                            DHCPc_OBJ          *p_obj,
                            DHCP_MSG_TYPE      type,
                            RTOS_ERR           *p_err)
{
  DHCP_MSG   *p_msg;
  CPU_INT16U msg_len;

  //                                                               -------- GET THE DHPC MESSAGE MEMORY BLOCK ---------
  if (p_obj->MsgPtr == DEF_NULL) {
    p_msg = (DHCP_MSG *)Mem_DynPoolBlkGet(&DHCPc_DataPtr->MsgObjPool, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
      goto exit;
    }
    p_obj->MsgPtr = p_msg;
  } else {
    p_msg = p_obj->MsgPtr;
  }

  //                                                               -------------- WRITE THE DHCP MESSAGE --------------
  msg_len = DHCPc_PrepareMsg(p_obj,
                             type,
                             p_msg->Buf,
                             DHCP_MSG_BUF_SIZE,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_msg->Len = msg_len;

  //                                                               ------------- SET SOCKET WAIT TIMEOUT --------------
  if (p_obj->SvcTaskTmr == DEF_NULL) {
    p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                             NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                             DHCPc_HookOnTmrTimeout,
                                             p_obj,
                                             DHCPc_SOCK_WAIT_TIMEOUT_MS,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  } else {
    NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                      DHCPc_HookOnTmrTimeout,
                      p_obj,
                      DHCPc_SOCK_WAIT_TIMEOUT_MS,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  //                                                               --------- SET SVC TASK SOCKET STATE FOR TX ---------
  p_obj->SvcTaskSockPtr->OpType = NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_RDY;
  p_obj->SockState = DHCPc_SOCK_STATE_TX;

exit:
  return;
}

/****************************************************************************************************//**
 *                                      DHCPc_PrepareTxRetry()
 *
 * @brief    Prepare the DHCP process for a transmit retry of the DHCP message.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_PrepareTxRetry(NET_SVC_TASK_CHILD *p_child,
                                 DHCPc_OBJ          *p_obj,
                                 RTOS_ERR           *p_err)
{
  const DHCPc_CFG *p_cfg;

  p_cfg = &p_obj->Cfg;

  p_obj->TxRetryCtr++;

  if (p_obj->TxRetryCtr >= p_cfg->TxRetryNbr) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_RETRY_MAX);
    p_obj->TxRetryCtr = 0;
    goto exit;
  }

  //                                                               ------------- SET SVC TASK SOCK FOR TX -------------
  p_obj->SvcTaskSockPtr->OpType = NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_RDY;
  p_obj->SockState = DHCPc_SOCK_STATE_TX;

  //                                                               ------------- SET SOCKET WAIT TIMEOUT --------------
  if (p_obj->SvcTaskTmr == DEF_NULL) {
    p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                             NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                             DHCPc_HookOnTmrTimeout,
                                             p_obj,
                                             DHCPc_SOCK_WAIT_TIMEOUT_MS,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  } else {
    NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                      DHCPc_HookOnTmrTimeout,
                      p_obj,
                      DHCPc_SOCK_WAIT_TIMEOUT_MS,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                        DHCPc_PrepareMsg()
 *
 * @brief    Prepare the DHCP message to send.
 *
 * @param    p_obj           Pointer to current DHCPc-Interface object.
 *
 * @param    msg_type        DHCP message type.
 *
 * @param    p_msg_buf       Pointer to the message buffer to fill.
 *
 * @param    msg_buf_size    Size of the message buffer.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Length of the DHPC message, if no errors.
 *           0, otherwise.
 *******************************************************************************************************/
static CPU_INT16U DHCPc_PrepareMsg(DHCPc_OBJ     *p_obj,
                                   DHCP_MSG_TYPE msg_type,
                                   CPU_INT08U    *p_msg_buf,
                                   CPU_INT16U    msg_buf_size,
                                   RTOS_ERR      *p_err)
{
  DHCPc_OFFER  *p_offer = DEF_NULL;
  DHCP_MSG_HDR *p_msg_hdr = DEF_NULL;
  CPU_INT08U   *p_msg_opt = DEF_NULL;
  CPU_INT08U   *p_opt = DEF_NULL;
  CPU_INT16U   opt_len = 0;
  CPU_INT16U   opt_pad_len = 0;
  NET_FLAGS    flags = NET_FLAG_NONE;
  CPU_BOOLEAN  get_local_addr = DEF_NO;
  CPU_BOOLEAN  wr_req_ip_addr = DEF_NO;
  CPU_BOOLEAN  wr_server_id = DEF_NO;
  CPU_BOOLEAN  req_param = DEF_NO;
  CPU_INT08U   req_param_qty;
  CPU_INT08U   addr_hw_len = 0;
  CPU_INT16U   rtn_val = 0;

  if (p_obj->OfferListPtr != DEF_NULL) {
    p_offer = SLIST_ENTRY(p_obj->OfferListPtr, DHCPc_OFFER, ListNode);
  } else {
    p_offer = DEF_NULL;
  }

  //                                                               ---------- GET SPECIFIC FIELDS & OPT VAL -----------
  switch (msg_type) {
    case DHCP_MSG_DISCOVER:
      DEF_BIT_SET(flags, DHCP_FLAG_BROADCAST);
      req_param = DEF_YES;
      get_local_addr = DEF_NO;
      wr_req_ip_addr = DEF_NO;
      wr_server_id = DEF_NO;
      break;

    case DHCP_MSG_REQUEST:
      DEF_BIT_SET(flags, DHCP_FLAG_BROADCAST);
      req_param = DEF_YES;
      switch (p_obj->State) {
        case DHCPc_STATE_INIT_REBOOT:
          get_local_addr = DEF_NO;
          wr_req_ip_addr = DEF_YES;
          wr_server_id = DEF_NO;
          break;

        case DHCPc_STATE_SELECTING:
          get_local_addr = DEF_NO;
          wr_req_ip_addr = DEF_YES;
          wr_server_id = DEF_YES;
          break;

        case DHCPc_STATE_BOUND:
        case DHCPc_STATE_RENEWING:
        case DHCPc_STATE_REBINDING:
          get_local_addr = DEF_YES;
          wr_req_ip_addr = DEF_NO;
          wr_server_id = DEF_NO;
          break;

        default:
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);
      }
      break;

    case DHCP_MSG_DECLINE:
      get_local_addr = DEF_NO;
      wr_req_ip_addr = DEF_YES;
      wr_server_id = DEF_YES;
      req_param = DEF_NO;
      break;

    case DHCP_MSG_RELEASE:
      get_local_addr = DEF_YES;
      wr_req_ip_addr = DEF_NO;
      wr_server_id = DEF_YES;
      req_param = DEF_NO;
      break;

    case DHCP_MSG_ACK:
    case DHCP_MSG_OFFER:
    case DHCP_MSG_NAK:
    case DHCP_MSG_INFORM:
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);
  }

  //                                                               --------------- SETTING DHCP MSG HDR ---------------
  Mem_Clr(p_msg_buf, msg_buf_size);                             // Clear message buffer.

  p_msg_hdr = (DHCP_MSG_HDR *)&p_msg_buf[0];

  p_msg_hdr->Op = DHCP_OP_REQUEST;
  p_msg_hdr->Hops = 0;

  NET_UTIL_VAL_COPY_SET_NET_32(&p_msg_hdr->Xid, &p_obj->TransactionID);
  NET_UTIL_VAL_SET_NET_32(&p_msg_hdr->Secs, 0);
  NET_UTIL_VAL_COPY_SET_NET_16(&p_msg_hdr->Flags, &flags);

  if (get_local_addr == DEF_YES) {
    if (p_offer != DEF_NULL) {
      NET_UTIL_VAL_COPY_32(&p_msg_hdr->Ciaddr, &p_offer->AddrOffered);
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
      goto exit;
    }
  } else {
    p_msg_hdr->Ciaddr = 0;
  }

  NET_UTIL_VAL_SET_NET_32(&p_msg_hdr->Yiaddr, 0);
  NET_UTIL_VAL_SET_NET_32(&p_msg_hdr->Siaddr, 0);
  NET_UTIL_VAL_SET_NET_32(&p_msg_hdr->Giaddr, 0);

  //                                                               Copy HW address to message header.
  addr_hw_len = NET_IF_HW_ADDR_LEN_MAX;
  NetIF_AddrHW_Get(p_obj->IF_Nbr,
                   p_msg_hdr->Chaddr,
                   &addr_hw_len,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_msg_hdr->Hlen = addr_hw_len;
  p_msg_hdr->Htype = DHCP_HTYPE_ETHER;                          // TODO_NET set type according to Link type

  //                                                               --------------- SETTING DHCP MSG OPT ---------------
  p_msg_opt = &p_msg_buf[DHCP_MSG_HDR_SIZE];
  p_opt = p_msg_opt;

  NET_UTIL_VAL_SET_NET_32(p_opt, DHCP_MAGIC_COOKIE);            // DHCP Magic cookie.
  p_opt += DHCP_MAGIC_COOKIE_SIZE;

  *p_opt++ = DHCP_OPT_CODE_DHCP_MESSAGE_TYPE;                   // DHCP Message Type.
  *p_opt++ = 1;                                                 // Msg type opt len.
  *p_opt++ = msg_type;

  if (wr_req_ip_addr == DEF_YES) {                              // Requested IP address.
    if (p_offer != DEF_NULL) {
      *p_opt++ = DHCP_OPT_CODE_REQUESTED_IP_ADDRESS;
      *p_opt++ = 4;
      NET_UTIL_VAL_COPY_32(p_opt, &p_offer->AddrOffered);
      p_opt += 4;
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
      goto exit;
    }
  }

  if (wr_server_id == DEF_YES) {                                // Server ID.
    if (p_offer != DEF_NULL) {
      *p_opt++ = DHCP_OPT_CODE_SERVER_IDENTIFIER;
      *p_opt++ = 4;
      NET_UTIL_VAL_COPY_32(p_opt, &p_offer->AddrServer);
      p_opt += 4;
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
      goto exit;
    }
  }

  if (req_param == DEF_YES) {                                   // Req'd param.
    req_param_qty = sizeof(DHCPc_ReqParam);

    *p_opt++ = DHCP_OPT_CODE_PARAMETER_REQUEST_LIST;
    *p_opt++ = req_param_qty;

    Mem_Copy((void *) p_opt,                                    // Copy system req'd param.
             (void *)&DHCPc_ReqParam[0],
             (CPU_SIZE_T) req_param_qty);
    p_opt += req_param_qty;
  }

  *p_opt++ = DHCP_OPT_CODE_END;                                 // End of options.

  //                                                               -------------------- GET MSG LEN -------------------
  opt_len = p_opt - p_msg_opt;
  if (opt_len < (DHCP_MSG_TX_MIN_LEN - DHCP_MSG_HDR_SIZE)) {
    opt_pad_len = ((DHCP_MSG_TX_MIN_LEN - DHCP_MSG_HDR_SIZE) - opt_len);

    Mem_Set(p_opt,
            DHCP_OPT_CODE_PAD,
            opt_pad_len);

    opt_len = (DHCP_MSG_TX_MIN_LEN - DHCP_MSG_HDR_SIZE);
  }

  //                                                               ------------------- GET CUR TIME -------------------
  p_obj->NegoStartTime = NetUtil_TS_Get_ms();

  rtn_val = DHCP_MSG_HDR_SIZE + opt_len;

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                         DHCPc_RxHandler()
 *
 * @brief    Process a received DHCP message.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    sock_id     Socket ID used to received data.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    p_msg       Pointer to the received message object.
 *
 * @param    p_err       Error pointer.
 *
 * @return   The status of the DHCP Client process. See enum DHCPc_STATUS
 *******************************************************************************************************/
static DHCPc_STATUS DHCPc_RxHandler(NET_SVC_TASK_CHILD *p_child,
                                    NET_SOCK_ID        sock_id,
                                    DHCPc_OBJ          *p_obj,
                                    DHCP_MSG           *p_msg,
                                    RTOS_ERR           *p_err)
{
  CPU_BOOLEAN  is_valid;
  DHCPc_STATUS status;

  //                                                               -------------- VALIDATE RECEIVED DATA --------------
  is_valid = DHCPc_RxValidate(sock_id, p_obj, p_msg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    status = DHCPc_STATUS_FAIL_ERR_FAULT;
    goto exit;
  }

  if (is_valid == DEF_NO) {
    status = DHCPc_STATUS_IN_PROGRESS;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }

  switch (p_obj->State) {
    case DHCPc_STATE_SELECTING:
      //                                                           -------------- PROCESS OFFER RECEIVED --------------
      DHCPc_RxOffer(p_obj, p_msg, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        status = DHCPc_STATUS_FAIL_ERR_FAULT;
        goto exit;
      }
      status = DHCPc_STATUS_IN_PROGRESS;
      break;

    case DHCPc_STATE_REQUESTING:
    case DHCPc_STATE_RENEWING:
    case DHCPc_STATE_REBINDING:
    case DHCPc_STATE_REBOOTING:
      //                                                           ---------------- CHECK IF VALID ACK ----------------
      switch (p_msg->Type) {
        case DHCP_MSG_ACK:
          break;

        case DHCP_MSG_NAK:
          status = DHCPc_STATUS_FAIL_NAK_RX;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          goto exit;

        default:
          status = DHCPc_STATUS_IN_PROGRESS;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          goto exit;
      }

      //                                                           --------------- PROCESS ACK RECEIVED ---------------
      DHCPc_RxACK(p_child, p_obj, p_msg, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        status = DHCPc_STATUS_FAIL_ERR_FAULT;
        goto exit;
      }

      if (p_obj->State == DHCPc_STATE_BOUND) {
        status = DHCPc_STATUS_SUCCESS;
      } else {
        status = DHCPc_STATUS_IN_PROGRESS;
      }
      break;

    default:
      status = DHCPc_STATUS_FAIL_ERR_FAULT;
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DHCPc_STATUS_FAIL_ERR_FAULT);
  }

exit:
  return (status);
}

/****************************************************************************************************//**
 *                                          DHCPc_RxOffer()
 *
 * @brief    Process a received DHCP Offer message.
 *
 * @param    p_obj   Pointer to current DHCPc-Interface object.
 *
 * @param    p_msg   Pointer to the received message object.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void DHCPc_RxOffer(DHCPc_OBJ *p_obj,
                          DHCP_MSG  *p_msg,
                          RTOS_ERR  *p_err)
{
  DHCPc_OFFER  *p_offer;
  DHCP_MSG_HDR *p_msg_hdr;
  CPU_INT08U   *p_opt;
  CPU_INT08U   opt_val_len;
  RTOS_ERR     local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_offer = (DHCPc_OFFER *)Mem_DynPoolBlkGet(&DHCPc_DataPtr->OfferObjPool, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    if (p_obj->OfferListPtr == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(local_err));
    }
    LOG_ERR(("DHCP Client RX offer dropped"));
    goto exit;
  }

  SList_Push(&p_obj->OfferListPtr, &p_offer->ListNode);         // Add offer to the Offers List.

  //                                                               --------------- SET OFFER PARAMETERS ---------------
  p_msg_hdr = (DHCP_MSG_HDR *)&p_msg->Buf[0];

  NET_UTIL_VAL_COPY_32(&p_offer->AddrOffered, &p_msg_hdr->Yiaddr);

  p_opt = DHCPc_MsgOptGet(DHCP_OPT_CODE_SERVER_IDENTIFIER,
                          &p_msg->Buf[0],
                          p_msg->Len,
                          &opt_val_len);
  if (p_opt != DEF_NULL) {
    NET_UTIL_VAL_COPY_32(&p_offer->AddrServer, p_opt);
  }

  p_opt = DHCPc_MsgOptGet(DHCP_OPT_CODE_SUBNET_MASK,
                          &p_msg->Buf[0],
                          p_msg->Len,
                          &opt_val_len);
  if (p_opt != DEF_NULL) {
    NET_UTIL_VAL_COPY_32(&p_offer->SubnetMask, p_opt);
  }

  p_opt = DHCPc_MsgOptGet(DHCP_OPT_CODE_ROUTER,
                          &p_msg->Buf[0],
                          p_msg->Len,
                          &opt_val_len);
  if (p_opt != DEF_NULL) {
    NET_UTIL_VAL_COPY_32(&p_offer->AddrGateway, p_opt);
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           DHCPc_RxACK()
 *
 * @brief    Process a received DHCP ACK message.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    p_msg       Pointer to the received message object.
 *
 * @param    p_err       Error pointer
 *******************************************************************************************************/
static void DHCPc_RxACK(NET_SVC_TASK_CHILD *p_child,
                        DHCPc_OBJ          *p_obj,
                        DHCP_MSG           *p_msg,
                        RTOS_ERR           *p_err)
{
  DHCPc_CFG *p_cfg;
#ifdef NET_DNS_CLIENT_MODULE_EN
  CPU_INT08U *p_opt;
  CPU_INT08U opt_val_len;
#endif
  CPU_BOOLEAN is_valid = DEF_YES;

  p_cfg = &p_obj->Cfg;

  DHCPc_LeaseTimeCalc(p_obj, p_msg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  switch (p_obj->State) {
    case DHCPc_STATE_REQUESTING:
      //                                                           -------- CHECK IF ADDRESS EXIST ON NETWORK ---------
      if (p_cfg->ValidateAddr == DEF_YES) {
        //                                                         ------------- START ADDRESS VALIDATION -------------
        is_valid = DHCPc_AddrValidate(p_obj, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
        //                                                         --- SET TIMER FOR WAIT DELAY IN ADDR VALIDATION ----
        if (p_obj->SvcTaskTmr == DEF_NULL) {
          p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                                   NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                                   DHCPc_HookOnTmrTimeout,
                                                   p_obj,
                                                   DHCPc_ADDR_VALIDATE_WAIT_TIME_MS,
                                                   p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
        } else {
          NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                            DHCPc_HookOnTmrTimeout,
                            p_obj,
                            DHCPc_ADDR_VALIDATE_WAIT_TIME_MS,
                            p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
        }
      }
      break;

    case DHCPc_STATE_RENEWING:
    case DHCPc_STATE_REBINDING:
    case DHCPc_STATE_REBOOTING:
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  if (is_valid == DEF_YES) {
    if (p_obj->SvcTaskTmr == DEF_NULL) {
      p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                               NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                               DHCPc_HookOnTmrTimeout,
                                               p_obj,
                                               p_obj->T1_Time_sec * DEF_TIME_NBR_mS_PER_SEC,
                                               p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
    } else {
      NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                        DHCPc_HookOnTmrTimeout,
                        p_obj,
                        p_obj->T1_Time_sec * DEF_TIME_NBR_mS_PER_SEC,
                        p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
    }

    p_obj->State = DHCPc_STATE_BOUND;
  }

  //                                                               TODO_NET : DNS and Time Offset options process
#ifdef NET_DNS_CLIENT_MODULE_EN
  p_opt = DHCPc_MsgOptGet(DHCP_OPT_CODE_DOMAIN_NAME_SERVER,
                          &p_msg->Buf[0],
                          p_msg->Len,
                          &opt_val_len);
  if (p_opt != DEF_NULL) {
    NET_IP_ADDR_OBJ ip_addr;
    RTOS_ERR        local_err;

    ip_addr.AddrLen = NET_IPv4_ADDR_SIZE;

    NET_UTIL_VAL_COPY_GET_NET_32(&ip_addr.Addr, p_opt);

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    DNSc_SetServerAddrInternal(&ip_addr, &local_err);
  }
#endif

exit:
  return;
}

/****************************************************************************************************//**
 *                                          DHCPc_RxSock()
 *
 * @brief    Received DHCP message with socket API.
 *
 * @param    sock_id     Socket ID on which data is received.
 *
 * @param    p_msg       Pointer to DHCPc message object that will be filled with data received.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_RxSock(NET_SOCK_ID sock_id,
                         DHCP_MSG    *p_msg,
                         RTOS_ERR    *p_err)
{
  DHCP_MSG_HDR      *p_msg_hdr;
  CPU_INT08U        *p_opt;
  CPU_INT08U        opt_val_len;
  NET_SOCK_ADDR     addr_remote;
  NET_SOCK_ADDR_LEN addr_remote_size;
  NET_SOCK_RTN_CODE rx_msg_len;

  //                                                               -------------- RETRIEVE RECEIVED DATA --------------
  addr_remote_size = sizeof(addr_remote);

  rx_msg_len = NetSock_RxDataFrom(sock_id,
                                  &p_msg->Buf[0],
                                  DHCP_MSG_BUF_SIZE,
                                  NET_SOCK_FLAG_NONE,
                                  &addr_remote,
                                  &addr_remote_size,
                                  DEF_NULL,
                                  0u,
                                  DEF_NULL,
                                  p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      p_msg->Len = rx_msg_len;
      break;

    default:
      goto exit;
  }

  if (rx_msg_len < DHCP_MSG_RX_MIN_LEN) {                       // Silently drop msg, if length rx is smaller than min.
    LOG_ERR(("DHCP Client RX message dropped. Length to small"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }

  p_msg_hdr = (DHCP_MSG_HDR *)&p_msg->Buf[0];

  //                                                               Validate transaction ID.
  NET_UTIL_VAL_COPY_GET_NET_32(&p_msg->TransactionID, &p_msg_hdr->Xid);

  //                                                               Retrieve msg type opt val.
  p_opt = DHCPc_MsgOptGet(DHCP_OPT_CODE_DHCP_MESSAGE_TYPE,
                          &p_msg->Buf[0],
                          rx_msg_len,
                          &opt_val_len);
  if (p_opt == DEF_NULL) {
    LOG_ERR(("DHCP Client RX message dropped. Message Type not found"));
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  p_msg->Type = (DHCP_MSG_TYPE)(*p_opt);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;
}

/****************************************************************************************************//**
 *                                        DHCPc_RxValidate()
 *
 * @brief    Check that the received packet is a valid DHCP message.
 *
 * @param    sock_id     Socket ID.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    p_msg       Pointer to received message.
 *
 * @param    p_err       Error pointer.
 *
 * @return   DEF_YES, if message received is valid.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN DHCPc_RxValidate(NET_SOCK_ID sock_id,
                                    DHCPc_OBJ   *p_obj,
                                    DHCP_MSG    *p_msg,
                                    RTOS_ERR    *p_err)
{
  DHCPc_OFFER   *p_offer;
  DHCP_MSG_HDR  *p_msg_hdr;
  CPU_BOOLEAN   opcode_reply;
  CPU_BOOLEAN   addr_hw_match;
  CPU_BOOLEAN   remote_match;
  CPU_INT08U    addr_hw[NET_IF_HW_ADDR_LEN_MAX];
  CPU_INT08U    addr_hw_len;
  CPU_INT08U    *p_opt;
  CPU_INT08U    opt_val_len;
  NET_IPv4_ADDR addr_server;
  CPU_BOOLEAN   is_valid;

  PP_UNUSED_PARAM(sock_id);

  if (p_obj->OfferListPtr != DEF_NULL) {
    p_offer = SLIST_ENTRY(p_obj->OfferListPtr, DHCPc_OFFER, ListNode);
  } else {
    p_offer = DEF_NULL;
  }

  //                                                               ----------- PARSE RECEIVED DHCP MESSAGE ------------
  p_msg_hdr = (DHCP_MSG_HDR *)&p_msg->Buf[0];

  //                                                               Validate that the message is a reply.
  opcode_reply = (p_msg_hdr->Op == DHCP_OP_REPLY) ? DEF_YES : DEF_NO;
  if (opcode_reply != DEF_YES) {
    is_valid = DEF_NO;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }

  //                                                               Validate HW address.
  addr_hw_len = NET_IF_HW_ADDR_LEN_MAX;
  NetIF_AddrHW_Get(p_obj->IF_Nbr,
                   &addr_hw[0],
                   &addr_hw_len,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    is_valid = DEF_NO;
    goto exit;
  }

  addr_hw_match = Mem_Cmp(p_msg_hdr->Chaddr,
                          &addr_hw[0],
                          addr_hw_len);

  if (addr_hw_match != DEF_YES) {
    is_valid = DEF_NO;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }

  //                                                               Validate server id.
  if (p_offer != DEF_NULL) {                                    // If server id known,    ...
                                                                // ... get server id opt, ...
    p_opt = DHCPc_MsgOptGet(DHCP_OPT_CODE_SERVER_IDENTIFIER,
                            &p_msg->Buf[0],
                            p_msg->Len,
                            &opt_val_len);

    if (p_opt == DEF_NULL) {
      remote_match = DEF_NO;
    } else {                                                    // ... & compare with lease server id.
      NET_UTIL_VAL_COPY_32(&addr_server, p_opt);
      remote_match = (addr_server == p_offer->AddrServer ? DEF_YES : DEF_NO);
    }
  } else {
    remote_match = DEF_YES;
  }

  if (remote_match != DEF_YES) {
    is_valid = DEF_NO;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }

  is_valid = DEF_YES;

exit:
  return (is_valid);
}

/****************************************************************************************************//**
 *                                         DHCPc_TxHandler()
 *
 * @brief    Process DHCP message to send.
 *
 * @param    p_child     Pointer to the DHCP Client Network Service Task Child.
 *
 * @param    sock_id     Socket ID to used.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_TxHandler(NET_SVC_TASK_CHILD *p_child,
                            NET_SOCK_ID        sock_id,
                            DHCPc_OBJ          *p_obj,
                            CPU_INT32U         timeout_ms,
                            RTOS_ERR           *p_err)
{
  //                                                               ---------- SET TIMER FOR RX WAIT TIMEOUT -----------
  if (p_obj->SvcTaskTmr == DEF_NULL) {
    p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(p_child,
                                             NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                             DHCPc_HookOnTmrTimeout,
                                             p_obj,
                                             timeout_ms,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  } else {
    NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                      DHCPc_HookOnTmrTimeout,
                      p_obj,
                      timeout_ms,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  //                                                               -------------- TRANSMIT DHCP MESSAGE ---------------
  DHCPc_TxSock(sock_id, p_obj, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ----------- SET SERVICE SOCK TO RX NEXT ------------
  p_obj->SvcTaskSockPtr->OpType = NET_SVC_TASK_SOCK_OP_TYPE_RX_RDY;
  p_obj->SockState = DHCPc_SOCK_STATE_RX;

exit:
  return;
}

/****************************************************************************************************//**
 *                                          DHCPc_TxSock()
 *
 * @brief    Transmit DHCP message with socket API.
 *
 * @param    sock_id     Socket ID on which to send the message.
 *
 * @param    p_obj       Pointer to current DHCPc-Interface object.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void DHCPc_TxSock(NET_SOCK_ID sock_id,
                         DHCPc_OBJ   *p_obj,
                         RTOS_ERR    *p_err)
{
  const DHCPc_CFG    *p_cfg;
  DHCPc_OFFER        *p_offer;
  DHCP_MSG           *p_msg;
  NET_IPv4_ADDR      ip_addr = NET_IPv4_ADDR_NONE;
  NET_SOCK_ADDR_IPv4 addr_server;
  NET_SOCK_ADDR_LEN  addr_server_size;
  NET_SOCK_RTN_CODE  tx_len;

  p_cfg = &p_obj->Cfg;

  if (p_obj->OfferListPtr != DEF_NULL) {
    p_offer = SLIST_ENTRY(p_obj->OfferListPtr, DHCPc_OFFER, ListNode);
  } else {
    p_offer = DEF_NULL;
  }

  switch (p_obj->State) {
    case DHCPc_STATE_INIT:
    case DHCPc_STATE_SELECTING:
    case DHCPc_STATE_REQUESTING:
    case DHCPc_STATE_ADDR_VALIDATION:
    case DHCPc_STATE_INIT_REBOOT:
      ip_addr = NET_IPv4_ADDR_THIS_HOST;
      addr_server_size = sizeof(addr_server);
      Mem_Clr((void *)&addr_server,
              (CPU_SIZE_T) addr_server_size);
      addr_server.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
      addr_server.Addr = NET_UTIL_HOST_TO_NET_32(NET_IPv4_ADDR_BROADCAST);
      addr_server.Port = NET_UTIL_HOST_TO_NET_16(p_cfg->ServerPortNbr);
      break;

    case DHCPc_STATE_BOUND:
    case DHCPc_STATE_RELEASE:
      if (p_offer != DEF_NULL) {
        ip_addr = p_offer->AddrOffered;
        addr_server_size = sizeof(addr_server);
        Mem_Clr((void *)&addr_server,
                (CPU_SIZE_T) addr_server_size);
        addr_server.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
        NET_UTIL_VAL_COPY_32(&addr_server.Addr, &p_offer->AddrServer);
        addr_server.Port = NET_UTIL_HOST_TO_NET_16(p_cfg->ServerPortNbr);
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
        goto exit;
      }
      break;

    case DHCPc_STATE_RENEWING:
      if (p_offer != DEF_NULL) {
        ip_addr = p_offer->AddrOffered;
        addr_server_size = sizeof(addr_server);
        Mem_Clr((void *)&addr_server,
                (CPU_SIZE_T) addr_server_size);
        addr_server.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
        addr_server.Addr = NET_UTIL_HOST_TO_NET_32(NET_IPv4_ADDR_BROADCAST);
        addr_server.Port = NET_UTIL_HOST_TO_NET_16(p_cfg->ServerPortNbr);
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
        goto exit;
      }
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               -------------- SET IF TO TRANSMIT ON ---------------
  DHCPc_SockSetTx(p_obj,
                  ip_addr,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_msg = p_obj->MsgPtr;

  //                                                               -------------- TRANSMIT DHCP MESSAGE ---------------
  tx_len = NetSock_TxDataTo(sock_id,
                            &p_msg->Buf,
                            p_msg->Len,
                            NET_SOCK_FLAG_NONE,
                            (NET_SOCK_ADDR *)&addr_server,
                            addr_server_size,
                            p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:                                         // Data transmitted.
      if (tx_len == NET_SOCK_BSD_ERR_DFLT) {
        goto exit;
      }
      break;

    case RTOS_ERR_POOL_EMPTY:                                   // Transitory Errors
    case RTOS_ERR_NET_IF_LINK_DOWN:
    case RTOS_ERR_TIMEOUT:
    case RTOS_ERR_WOULD_BLOCK:
      break;

    default:                                                    // Fatal err.
      goto exit_close;
  }

  DHCPc_SockSetRx(p_obj, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  goto exit;

exit_close:
  {
    RTOS_ERR err;

    NetSock_Close(sock_id, &err);
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       DHCPc_MsgOptGet()
 *
 * @brief    Retrieve the specified option value from a DHCP message buffer.
 *
 * @param    opt_code        Option code to return value of.
 *
 * @param    p_msg_buf       Pointer to DHCP message buffer to search.
 *
 * @param    msg_buf_size    Size of message buffer (in octets).
 *
 * @param    p_opt_val_len   Pointer to variable that will receive the length of the option value.
 *
 * @return   Pointer to the specified option value, if option found without error.
 *           Pointer to NULL,                       otherwise.
 *******************************************************************************************************/
static CPU_INT08U *DHCPc_MsgOptGet(DHCP_OPT_CODE opt_code,
                                   CPU_INT08U    *p_msg_buf,
                                   CPU_INT16U    msg_buf_size,
                                   CPU_INT08U    *p_opt_val_len)
{
  CPU_INT32U  magic_cookie;
  CPU_BOOLEAN opt_start;
  CPU_BOOLEAN opt_found;
  CPU_INT08U  *p_opt;
  CPU_INT08U  *p_opt_val;
  CPU_INT08U  *p_end_msg;

  *p_opt_val_len = 0;                                           // Cfg rtn opt val len for err.

  p_opt = p_msg_buf + DHCP_MSG_HDR_SIZE;

  //                                                               -------- VALIDATE BEGINNING OF OPT SECTION ---------
  magic_cookie = NET_UTIL_HOST_TO_NET_32(DHCP_MAGIC_COOKIE);
  opt_start = Mem_Cmp(p_opt,
                      &magic_cookie,
                      DHCP_MAGIC_COOKIE_SIZE);

  if (opt_start != DEF_YES) {                                   // If magic cookie NOT here, ...
    return (DEF_NULL);                                          // ... rtn.
  }

  p_opt += DHCP_MAGIC_COOKIE_SIZE;                              // Go to first opt.

  //                                                               --------------------- SRCH OPT ---------------------
  opt_found = DEF_NO;
  p_end_msg = p_msg_buf + msg_buf_size;

  while ((opt_found != DEF_YES)                                 // Srch until opt found,
         && (*p_opt != DHCP_OPT_CODE_END)                       // & opt end    NOT reached,
         && (p_opt <= p_end_msg)) {                             // & end of msg NOT reached.
    if (*p_opt == opt_code) {                                   // If popt equals srch'd opt code, ...
      opt_found = DEF_YES;                                      // ... opt found.
    } else if (*p_opt == DHCP_OPT_CODE_PAD) {                   // If popt is padding, ...
      p_opt++;                                                  // ... advance.
    } else {                                                    // Else, another opt found,  ...
                                                                // ... skip to next opt.
      p_opt += ((*(p_opt + DHCP_OPT_FIELD_CODE_LEN)) + DHCP_OPT_FIELD_HDR_LEN);
    }
  }

  if (opt_found != DEF_YES) {
    return (DEF_NULL);
  }

  *p_opt_val_len = *(p_opt + DHCP_OPT_FIELD_CODE_LEN);      // Set opt val len ...
  p_opt_val = p_opt + DHCP_OPT_FIELD_HDR_LEN;               // ... & set opt val ptr.

  return (p_opt_val);
}

/****************************************************************************************************//**
 *                                       DHCPc_LeaseTimeCalc()
 *
 * @brief    Calculate the DHCP Lease Time for the IP address.
 *
 * @param    p_obj   Pointer to DHCPc-Interface object.
 *
 * @param    p_msg   Pointer to the DHCP message received.
 *
 * @param    p_err   Error Pointer.
 *******************************************************************************************************/
static void DHCPc_LeaseTimeCalc(DHCPc_OBJ *p_obj,
                                DHCP_MSG  *p_msg,
                                RTOS_ERR  *p_err)
{
  CPU_INT08U *p_opt;
  CPU_INT08U opt_val_len;
  CPU_INT32U time_lease;
  CPU_INT32U time_t1;
  CPU_INT32U time_t2;
  CPU_INT32U time_nego_stop;
  CPU_INT32U time_nego_sec;

  time_lease = DHCP_LEASE_INFINITE;

  //                                                               ------------- GET LEASE TIME FROM ACK --------------
  //                                                               Get lease time.
  p_opt = DHCPc_MsgOptGet(DHCP_OPT_CODE_IP_ADDRESS_LEASE_TIME,
                          &p_msg->Buf[0],
                          p_msg->Len,
                          &opt_val_len);
  if (p_opt != DEF_NULL) {
    NET_UTIL_VAL_COPY_GET_NET_32(&time_lease, p_opt);
  }

  if (time_lease == DHCP_LEASE_INFINITE) {                      // If lease time infinite, ...
    p_obj->LeaseTime_sec = DHCP_LEASE_INFINITE;
    p_obj->T1_Time_sec = DHCP_LEASE_INFINITE;
    p_obj->T2_Time_sec = DHCP_LEASE_INFINITE;

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                        // ... NO tmr to set.
    goto exit;
  }

  //                                                               Get renewal time.
  p_opt = DHCPc_MsgOptGet(DHCP_OPT_CODE_RENEWAL_TIME_VALUE,
                          &p_msg->Buf[0],
                          p_msg->Len,
                          &opt_val_len);
  if (p_opt != DEF_NULL) {
    NET_UTIL_VAL_COPY_GET_NET_32(&time_t1, p_opt);
  } else {
    time_t1 = (CPU_INT32U)(time_lease * DHCP_T1_LEASE_FRACTION);
  }

  //                                                               Get rebinding time.
  p_opt = DHCPc_MsgOptGet(DHCP_OPT_CODE_REBINDING_TIME_VALUE,
                          &p_msg->Buf[0],
                          p_msg->Len,
                          &opt_val_len);
  if (p_opt != DEF_NULL) {
    NET_UTIL_VAL_COPY_GET_NET_32(&time_t2, p_opt);
  } else {
    time_t2 = (CPU_INT32U)(time_lease * DHCP_T2_LEASE_FRACTION);
  }

  //                                                               ----------------- CALC LEASE TIME ------------------
  time_nego_stop = NetUtil_TS_Get_ms();
  time_nego_sec = (time_nego_stop - p_obj->NegoStartTime) / 1000;

  if (time_t2 < time_t1) {
    time_t2 = time_t1;
  }

  time_lease += time_nego_sec;
  if (time_lease >= time_t2) {
    time_lease -= time_t2;
  } else {
    time_lease = 0;
  }

  p_obj->T1_Time_sec = time_t1;
  p_obj->T2_Time_sec = (time_t2 - time_t1);
  p_obj->LeaseTime_sec = time_lease;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       DHCPc_AddrValidate()
 *
 * @brief    Check if the IPv4 address already exist on the network by sending ARP messages.
 *
 * @param    p_obj   Pointer to DHCPc-Interface object.
 *
 * @param    p_err   Error pointer.
 *
 * @return   DEF_YES, address is valid because is unique on the local network.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN DHCPc_AddrValidate(DHCPc_OBJ *p_obj,
                                      RTOS_ERR  *p_err)
{
#ifdef  NET_ARP_MODULE_EN
  DHCPc_OFFER        *p_offer;
  NET_CACHE_ADDR_ARP *p_cache_addr_arp;
  NET_ARP_CACHE      *p_cache;
  CPU_INT08U         hw_addr_sender[NET_CACHE_HW_ADDR_LEN_ETHER];
  NET_IPv4_ADDR      addr_this_host;
  CPU_INT08U         addr_len;
  CPU_INT08U         rtn_len;
  CPU_BOOLEAN        rtn_val = DEF_NO;

  if (p_obj->OfferListPtr != DEF_NULL) {
    p_offer = SLIST_ENTRY(p_obj->OfferListPtr, DHCPc_OFFER, ListNode);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
    goto exit;
  }

  addr_this_host = NET_IPv4_ADDR_NONE;
  addr_len = sizeof(p_offer->AddrOffered);

  switch (p_obj->State) {
    case DHCPc_STATE_REQUESTING:
      //                                                           ---------------- PROBE ADDR ON NET -----------------
      NetARP_CacheProbeAddrOnNet(NET_PROTOCOL_TYPE_IP_V4,
                                 (CPU_INT08U *)&addr_this_host,
                                 (CPU_INT08U *)&p_offer->AddrOffered,
                                 addr_len,
                                 p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      p_obj->State = DHCPc_STATE_ADDR_VALIDATION;
      rtn_val = DEF_NO;
      break;

    case DHCPc_STATE_ADDR_VALIDATION:
      //                                                           ------------ GET HW ADDR FROM ARP CACHE ------------
      rtn_len = NetARP_CacheGetAddrHW(p_obj->IF_Nbr,
                                      &hw_addr_sender[0],
                                      NET_CACHE_HW_ADDR_LEN_ETHER,
                                      (CPU_INT08U *)&p_offer->AddrOffered,
                                      addr_len,
                                      p_err);
      if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
          && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NET_ADDR_UNRESOLVED)
          && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NOT_FOUND)) {
        goto exit;
      }
      if (rtn_len == 0) {
        rtn_val = DEF_YES;                                      // ... addr NOT used.
      } else {
        rtn_val = DEF_NO;                                       // ... addr used is used.
      }

      p_cache_addr_arp = (NET_CACHE_ADDR_ARP *)NetCache_AddrSrch(NET_CACHE_TYPE_ARP,
                                                                 p_obj->IF_Nbr,
                                                                 (CPU_INT08U *)&p_offer->AddrOffered,
                                                                 NET_IPv4_ADDR_SIZE);
      if (p_cache_addr_arp == DEF_NULL) {
        return (rtn_val);
      }

      p_cache = (NET_ARP_CACHE *)p_cache_addr_arp->ParentPtr;

      NetARP_CacheRemoveEntry(p_cache, DEF_YES);
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NO);
  }

exit:
  return (rtn_val);

#else
  PP_UNUSED_PARAM(p_obj);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
  return (DEF_NO);
#endif
}

/****************************************************************************************************//**
 *                                       DHCPc_LinkSubscribe()
 *
 * @brief    Hook function for DHCP client process when the Interface link state changes.
 *
 * @param    if_nbr      Interface number on which link state changed
 *
 * @param    link_state  Current link state of interface.
 *******************************************************************************************************/
static void DHCPc_LinkSubscribe(NET_IF_NBR        if_nbr,
                                NET_IF_LINK_STATE link_state)
{
  DHCPc_OBJ *p_obj;
  RTOS_ERR  err;

  SLIST_FOR_EACH_ENTRY(DHCPc_DataPtr->ObjListPtr, p_obj, DHCPc_OBJ, ListNode) {
    if (p_obj->IF_Nbr == if_nbr) {
      p_obj->LinkState = link_state;
      if (link_state == NET_IF_LINK_UP) {
        switch (p_obj->State) {
          case DHCPc_STATE_INIT:
          case DHCPc_STATE_SELECTING:
          case DHCPc_STATE_REQUESTING:
          case DHCPc_STATE_ADDR_VALIDATION:
            LOG_VRB(("DHCPc: Link is Up. Starting."));
            NetSvcTask_ChildMsgPost(DHCPc_DataPtr->SvcTaskChildPtr,
                                    DHCPc_MSG_TYPE_START_IF,
                                    p_obj,
                                    DEF_YES,
                                    &err);
            break;

          case DHCPc_STATE_BOUND:
          case DHCPc_STATE_RENEWING:
          case DHCPc_STATE_REBINDING:
          case DHCPc_STATE_INIT_REBOOT:
          case DHCPc_STATE_REBOOTING:
            LOG_VRB(("DHCPc: Link is Up. Rebooting."));
            NetSvcTask_ChildMsgPost(DHCPc_DataPtr->SvcTaskChildPtr,
                                    DHCPc_MSG_TYPE_REBOOT_IF,
                                    p_obj,
                                    DEF_YES,
                                    &err);
            break;

          case DHCPc_STATE_RELEASE:
          default:
            break;
        }
      }
      break;
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_DHCP_CLIENT_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
