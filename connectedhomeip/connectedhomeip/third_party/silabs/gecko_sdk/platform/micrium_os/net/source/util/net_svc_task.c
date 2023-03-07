/***************************************************************************//**
 * @file
 * @brief Network Service Task Module
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

#include  "net_svc_task_priv.h"

#include  "../tcpip/net_util_priv.h"
#include  "../tcpip/net_sock_priv.h"

#include  <net/include/net_util.h>

#include  <cpu/include/cpu.h>
#include  <common/include/kal.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_path.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/collections/slist_priv.h>

#include  <common_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (NET)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_NET

#define  NET_SVC_TASK_INIT_STATUS_NONE                   0u
#define  NET_SVC_TASK_INIT_STATUS_OK                     1u
#define  NET_SVC_TASK_INIT_STATUS_FAIL                   2u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  enum  net_svc_task_msg_type {
  NET_SVC_TASK_MSG_TYPE_START_CHILD,
  NET_SVC_TASK_MSG_TYPE_STOP_CHILD,
  NET_SVC_TASK_MSG_TYPE_CHILD_POST,
  NET_SVC_TASK_MSG_TYPE_CHILD_POST_SOCK_CLIENT,
  NET_SVC_TASK_MSG_TYPE_CHILD_POST_SOCK_SERVER
} NET_SVC_TASK_MSG_TYPE;

typedef  enum  net_svc_task_pend_state {
  NET_SVC_TASK_PEND_STATE_SOCK_SEL,
  NET_SVC_TASK_PEND_STATE_SIGNAL,
}NET_SVC_TASK_PEND_STATE;

typedef  struct  net_svc_task_ctx {                             // --------------------- GEN CFG ----------------------
  CPU_INT32U      TaskPrio;
  CPU_STK_SIZE    StkSizeElements;
  void            *StkPtr;
  MEM_SEG         *MemSegPtr;                                   // Mem seg to use for alloc.

  KAL_TASK_HANDLE Task;
  KAL_LOCK_HANDLE Lock;
  KAL_SEM_HANDLE  SemStopReq;
  KAL_SEM_HANDLE  SemStopCompleted;

  CPU_INT32U      TS1;
  CPU_INT32U      TS2;

  CPU_BOOLEAN     IsStarted;

  SLIST_MEMBER    *Childs;

  SLIST_MEMBER    *MsgQ;
  SLIST_MEMBER    *MsgList;

  SLIST_MEMBER    *TmrsQ;
  SLIST_MEMBER    *Tmrs;
  SLIST_MEMBER    *TmrsFree;

  MEM_DYN_POOL    MsgPool;

  KAL_SEM_HANDLE  TaskSignal;
} NET_SVC_TASK_CTX;

typedef  struct  net_svc_task_msg {
  NET_SVC_TASK_MSG_TYPE Type;
  void                  *ArgPtr;

  KAL_SEM_HANDLE        MsgEndSignal;
  void                  *BlkPtr;
  RTOS_ERR              *MsgErrPtr;

  SLIST_MEMBER          ListNode;
} NET_SVC_TASK_MSG;

typedef  struct  net_svc_task_msg_child {
  NET_SVC_TASK_CHILD *ChildPtr;
  CPU_INT32U         ChildMsg;
  void               *ChildMsgArgPtr;
  CPU_INT32U         *ChildErrPtr;
} NET_SVC_TASK_MSG_CHILD;

typedef  struct  net_svc_task_tmr {
  NET_SVC_TASK_TMR_TYPE Type;
  NET_SVC_TASK_CHILD    *ChildPtr;
  CPU_INT32U            Timeout_ms;
  CPU_INT32U            Delta_ms;
  CPU_INT32U            TS_ms;
  NET_SVC_TASK_TMR_FNCT Fnct;
  void                  *FnctArg;
  SLIST_MEMBER          ListNode;
} NET_SVC_TASK_TMR;

typedef  enum  net_svc_task_sock_type {
  NET_SVC_TASK_SOCK_TYPE_CLIENT,
  NET_SVC_TASK_SOCK_TYPE_SERVER
} NET_SVC_TASK_SOCK_TYPE;

typedef  struct  net_svc_task_msg_sock {
  NET_SVC_TASK_SOCK_TYPE   Type;
  NET_SOCK_PROTOCOL_FAMILY ProtocolFamily;
  NET_SOCK_TYPE            SockType;
  NET_SOCK_PROTOCOL        Protocol;
  NET_FLAGS                Flags;
  NET_SOCK_ADDR            *AddrBindPtr;
  NET_SOCK_ADDR_LEN        AddrLen;
  NET_SOCK_ID              SockID;
  NET_SVC_TASK_CHILD       *ChildPtr;
} NET_SVC_TASK_MSG_SOCK;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void NetSvcTask(void *p_arg);

static void NetSvcTask_Pend(NET_SVC_TASK_CTX  *p_ctx,
                            NET_SOCK_DESC     *p_sock_rd,
                            NET_SOCK_DESC     *p_sock_wr,
                            NET_SOCK_DESC     *p_sock_err,
                            NET_SOCK_RTN_CODE *p_sock_rdy_ctn);

static void NetSvcTask_WakeUp(NET_SVC_TASK_CTX *p_ctx);

static void NetSvcTask_MsgPost(NET_SVC_TASK_CTX      *p_ctx,
                               NET_SVC_TASK_MSG_TYPE msg_type,
                               void                  *p_arg,
                               CPU_BOOLEAN           block,
                               RTOS_ERR              *p_err);

static void NetSvcTask_Msg(NET_SVC_TASK_CTX *p_ctx);

static void NetSvcTask_ChildAdd(NET_SVC_TASK_CTX *p_ctx,
                                void             *p_msg_arg,
                                RTOS_ERR         *p_err);

static void NetSvcTask_ChildMsgDemux(void     *p_arg,
                                     RTOS_ERR *p_err);

static void NetSvcTask_Tmr(NET_SVC_TASK_CTX *p_ctx);

static CPU_INT32U NetSvcTask_TmrDeltaGet(NET_SVC_TASK_CTX *p_ctx);

static CPU_INT32U NetSvcTask_TmrTimeoutGet(NET_SVC_TASK_CTX *p_ctx);

static void NetSvcTask_TmrListInsert(NET_SVC_TASK_CTX *p_ctx,
                                     NET_SVC_TASK_TMR *p_tmr);

static void NetSvcTask_TmrListRem(NET_SVC_TASK_CTX *p_ctx,
                                  NET_SVC_TASK_TMR *p_tmr);

static NET_SVC_TASK_TMR *NetSvcTask_TmrObjAlloc(NET_SVC_TASK_CHILD *p_child,
                                                RTOS_ERR           *p_err);

static void NetSvcTask_TmrObjFree(NET_SVC_TASK_TMR *p_tmr,
                                  RTOS_ERR         *p_err);

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static void NetSvcTask_SockAddClient(void     *p_arg,
                                     RTOS_ERR *p_err);

static void NetSvcTask_SockAddServer(void     *p_arg,
                                     RTOS_ERR *p_err);

static NET_SOCK_QTY NetSvcTask_SockDescInit(NET_SVC_TASK_CTX *p_ctx,
                                            NET_SOCK_DESC    *p_sock_rd,
                                            NET_SOCK_DESC    *p_sock_wr,
                                            NET_SOCK_DESC    *p_sock_err);

static void NetSvcTask_Sock(NET_SVC_TASK_CTX *p_ctx,
                            NET_SOCK_DESC    *p_sock_rd,
                            NET_SOCK_DESC    *p_sock_wr,
                            NET_SOCK_DESC    *p_sock_err);
#endif

#if 0
static void NetSvcTask_StopReqSignal(NET_SVC_TASK_CTX *p_ctx,
                                     RTOS_ERR         *p_err);

static CPU_BOOLEAN NetSvcTask_IsStopReqPending(NET_SVC_TASK_CTX *p_ctx);

static void NetSvcTask_StopCompletedSignal(NET_SVC_TASK_CTX *p_ctx);

static void NetSvcTask_WaitStopCompleted(NET_SVC_TASK_CTX *p_ctx,
                                         RTOS_ERR         *p_err);
#endif

static void NetSvcTask_ObjsAlloc(NET_SVC_TASK_CTX *p_ctx,
                                 RTOS_ERR         *p_err);

static void NetSvcTask_ObjsFree(NET_SVC_TASK_CTX *p_ctx);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetSvcTask_Init()
 *
 * @brief    Initialize the Network Service Task module.
 *
 * @param    p_task_cfg  Pointer to task configuration.
 *
 * @param    p_mem_seg   Memory segment from which internal data will be allocated.
 *                       If DEF_NULL, will be allocated from the global heap.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   The Network Service Task handle.
 *******************************************************************************************************/
NET_SVC_TASK_HANDLE NetSvcTask_Init(CPU_INT32U   task_prio,
                                    CPU_STK_SIZE stk_size_elements,
                                    void         *p_stk,
                                    MEM_SEG      *p_mem_seg,
                                    RTOS_ERR     *p_err)
{
  MEM_SEG             *p_seg;
  NET_SVC_TASK_CTX    *p_ctx;
  NET_SVC_TASK_HANDLE svc_instance = DEF_NULL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, svc_instance);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_seg = DEF_NULL;
  if (p_mem_seg != DEF_NULL) {                                  // Load cfg if given.
    p_seg = p_mem_seg;
  }

  p_ctx = (NET_SVC_TASK_CTX *)Mem_SegAlloc("Net Service Task internal data",
                                           p_seg,
                                           sizeof(NET_SVC_TASK_CTX),
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_DynPoolCreate("Net Service Msg pool",
                    &p_ctx->MsgPool,
                    p_seg,
                    sizeof(NET_SVC_TASK_MSG),
                    sizeof(CPU_ALIGN),
                    0u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_ctx->MemSegPtr = p_seg;
  p_ctx->TaskPrio = task_prio;
  p_ctx->StkSizeElements = stk_size_elements;
  p_ctx->StkPtr = p_stk;
  p_ctx->IsStarted = DEF_NO;

  NetSvcTask_ObjsAlloc(p_ctx, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_ctx->TS1 = 0;
  p_ctx->TS2 = 0;

  SList_Init(&p_ctx->Childs);
  SList_Init(&p_ctx->MsgQ);
  SList_Init(&p_ctx->MsgList);
  SList_Init(&p_ctx->TmrsQ);
  SList_Init(&p_ctx->Tmrs);
  SList_Init(&p_ctx->TmrsFree);

  svc_instance = (NET_SVC_TASK_HANDLE)p_ctx;

exit:
  return (svc_instance);
}

/****************************************************************************************************//**
 *                                           NetSvcTask_Start()
 *
 * @brief    Start the service task associated with the given handle.
 *
 * @param    task_handle     Service task handle of task to start.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetSvcTask_Start(NET_SVC_TASK_HANDLE task_handle,
                      RTOS_ERR            *p_err)
{
  NET_SVC_TASK_CTX *p_ctx = (NET_SVC_TASK_CTX *)task_handle;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((task_handle != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               -------------- ACQUIRE INSTANCE LOCK. --------------
  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  if (p_ctx->IsStarted == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit_release;
  }

  //                                                               ----------- CREATE INSTANCE SERVICE TASK -----------
  KAL_TaskCreate(p_ctx->Task,
                 NetSvcTask,
                 p_ctx,
                 p_ctx->TaskPrio,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_ctx->IsStarted = DEF_YES;

exit_release:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    KAL_LockRelease(p_ctx->Lock, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}

/****************************************************************************************************//**
 *                                               NetSvcTask_Stop()
 *
 * @brief    Stop the service task associated with the given handle.
 *
 * @param    task_handle     Service task handle of task to stop
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetSvcTask_Stop(NET_SVC_TASK_HANDLE task_handle,
                     RTOS_ERR            *p_err)
{
  NET_SVC_TASK_CTX *p_ctx = (NET_SVC_TASK_CTX *)task_handle;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((task_handle != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               -------------- ACQUIRE INSTANCE LOCK. --------------
  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               ---------- CHECK IF TASK IS ALREADY STOP -----------
  if (p_ctx->IsStarted != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit_release;
  }

  //                                                               ------------- SIGNAL INSTANCE TO STOP --------------

  KAL_SemPost(p_ctx->SemStopReq, KAL_OPT_POST_NONE, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               Release Instance lock before pending.
  KAL_LockRelease(p_ctx->Lock, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               --------------- WAIT STOP COMPLETED ----------------
  KAL_SemPend(p_ctx->SemStopCompleted, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               Re-acquire Instance lock.
  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               ---------------- DEL INSTANCE TASK -----------------
  p_ctx->IsStarted = DEF_NO;                                    // Stop Instance.

  KAL_TaskDel(p_ctx->Task);

exit_release:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    KAL_LockRelease(p_ctx->Lock, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}

/****************************************************************************************************//**
 *                                           NetSvcTask_PrioSet()
 *
 * @brief    Sets priority of the network service task.
 *
 * @param    task_handle     Service task handle.
 *
 * @param    prio            Priority of the network service task.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetSvcTask_PrioSet(NET_SVC_TASK_HANDLE task_handle,
                        RTOS_TASK_PRIO      prio,
                        RTOS_ERR            *p_err)
{
  NET_SVC_TASK_CTX *p_ctx = (NET_SVC_TASK_CTX *)task_handle;
  KAL_TASK_HANDLE  kal_task_handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  kal_task_handle = p_ctx->Task;
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(kal_task_handle,
                  prio,
                  p_err);
}

/****************************************************************************************************//**
 *                                           NetSvcTask_ChildStart()
 *
 * @brief    Create and start a service task's child.
 *
 * @param    task_handle     Task handle on which the child will be attached.
 *
 * @param    p_child_cfg     Pointer to the child configuration.
 *
 * @param    p_child_data    Pointer to specific data for the child.
 *
 * @param    p_hooks         Pointer to set of hook functions for the child.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to the new child.
 *******************************************************************************************************/
NET_SVC_TASK_CHILD *NetSvcTask_ChildStart(NET_SVC_TASK_HANDLE    task_handle,
                                          NET_SVC_TASK_CHILD_CFG *p_child_cfg,
                                          void                   *p_child_data,
                                          NET_SVC_TASK_HOOKS     *p_hooks,
                                          RTOS_ERR               *p_err)
{
  NET_SVC_TASK_CTX   *p_ctx = (NET_SVC_TASK_CTX *)task_handle;
  NET_SVC_TASK_CHILD *p_child = DEF_NULL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, p_child);
  RTOS_ASSERT_DBG_ERR_SET((task_handle != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  //                                                               ----------- ALLOCATE CHILD MEMORY BLOCK ------------
  p_child = (NET_SVC_TASK_CHILD *)Mem_SegAlloc("Net Service Task internal data",
                                               p_child_cfg->MemSegPtr,
                                               sizeof(NET_SVC_TASK_CHILD),
                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               --------------- SET CHILD PARAMETERS ---------------
  p_child->CfgPtr = p_child_cfg;
  p_child->HooksPtr = p_hooks;
  p_child->ChildDataPtr = p_child_data;
  p_child->TaskHandle = task_handle;

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  SList_Init(&p_child->SockListPtr);
#endif

  //                                                               --------- SIGNAL SERVICE TASK TO ADD CHILD ---------
  NetSvcTask_MsgPost(p_ctx, NET_SVC_TASK_MSG_TYPE_START_CHILD, p_child, DEF_YES, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (p_child);
}

/****************************************************************************************************//**
 *                                           NetSvcTask_ChildStop()
 *
 * @brief    Stop a service task's child.
 *
 * @param    p_child     Pointer to the child.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetSvcTask_ChildStop(NET_SVC_TASK_CHILD *p_child,
                          RTOS_ERR           *p_err)
{
  NET_SVC_TASK_CTX *p_ctx = (NET_SVC_TASK_CTX *)p_child->TaskHandle;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_child != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               --------- SIGNAL SERVICE TASK TO STOP CHILD --------
  NetSvcTask_MsgPost(p_ctx, NET_SVC_TASK_MSG_TYPE_STOP_CHILD, p_child, DEF_YES, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  goto exit;

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetSvcTask_ChildMsgPost()
 *
 * @brief    Send a message to a specific service task's child.
 *
 * @param    p_child             Pointer to child.
 *
 * @param    child_msg_type      Message type specific to the child.
 *
 * @param    p_child_msg_arg     Pointer to message argument.
 *
 * @param    block               DEF_YES, the function will wait for the message processing end.
 *                               DEF_NO,  otherwise.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetSvcTask_ChildMsgPost(NET_SVC_TASK_CHILD *p_child,
                             CPU_INT32U         child_msg_type,
                             void               *p_child_msg_arg,
                             CPU_BOOLEAN        block,
                             RTOS_ERR           *p_err)
{
  NET_SVC_TASK_CTX       *p_ctx;
  NET_SVC_TASK_MSG_CHILD *p_child_msg;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_child != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_ctx = (NET_SVC_TASK_CTX *)p_child->TaskHandle;

  p_child_msg = (NET_SVC_TASK_MSG_CHILD *)Mem_DynPoolBlkGet(p_child->PoolMsgPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               -------------- SET MESSAGE PARAMETERS --------------
  p_child_msg->ChildPtr = p_child;
  p_child_msg->ChildMsg = child_msg_type;
  p_child_msg->ChildMsgArgPtr = p_child_msg_arg;

  //                                                               -------- POST MESSAGE TO THE SVC TASK CHILD --------
  NetSvcTask_MsgPost(p_ctx, NET_SVC_TASK_MSG_TYPE_CHILD_POST, p_child_msg, block, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;

exit_release:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    Mem_DynPoolBlkFree(p_child->PoolMsgPtr, p_child_msg, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}

/****************************************************************************************************//**
 *                                           NetSvcTask_TmrCreate()
 *
 * @brief    Create new timer for given service task's child.
 *
 * @param    p_child     Pointer to service task's child.
 *
 * @param    type        Timer type :
 *                       NET_SVC_TASK_TMR_TYPE_ONE_SHOT
 *                       NET_SVC_TASK_TMR_TYPE_PERIODIC
 *
 * @param    fnct        Timeout callback function of timer.
 *
 * @param    p_arg       Pointer to object to pass as argument in the timeout callback.
 *
 * @param    timeout_ms  Timeout value of timer.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Timer handle.
 *******************************************************************************************************/
NET_SVC_TASK_TMR_HANDLE NetSvcTask_TmrCreate(NET_SVC_TASK_CHILD    *p_child,
                                             NET_SVC_TASK_TMR_TYPE type,
                                             NET_SVC_TASK_TMR_FNCT fnct,
                                             void                  *p_arg,
                                             CPU_INT32U            timeout_ms,
                                             RTOS_ERR              *p_err)
{
  NET_SVC_TASK_TMR        *p_tmr;
  NET_SVC_TASK_CTX        *p_ctx;
  NET_SVC_TASK_TMR_HANDLE handle = DEF_NULL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_child != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((timeout_ms != 0), *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);

  p_ctx = (NET_SVC_TASK_CTX *)p_child->TaskHandle;

  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

  p_tmr = NetSvcTask_TmrObjAlloc(p_child, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_tmr->ChildPtr = p_child;
  p_tmr->Type = type;
  p_tmr->Fnct = fnct;
  p_tmr->FnctArg = p_arg;
  p_tmr->Timeout_ms = timeout_ms;
  p_tmr->Delta_ms = timeout_ms;

  p_tmr->TS_ms = NetUtil_TS_Get_ms_Internal();

  SList_Push(&p_ctx->TmrsQ, &p_tmr->ListNode);

  handle = (NET_SVC_TASK_TMR_HANDLE)p_tmr;

  KAL_LockRelease(p_ctx->Lock, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

  NetSvcTask_WakeUp(p_ctx);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  goto exit;

exit_release:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    KAL_LockRelease(p_ctx->Lock, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
  }

exit:
  return (handle);
}

/****************************************************************************************************//**
 *                                           NetSvcTask_TmrSet()
 *
 * @brief    Set the value of a already existing service task's timer.
 *
 * @param    tmr_handle  Timer handle.
 *
 * @param    fnct        Timeout callback function of timer.
 *
 * @param    p_arg       Pointer to object to pass as argument in the timeout callback.
 *
 * @param    timeout_ms  Timeout value of timer.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetSvcTask_TmrSet(NET_SVC_TASK_TMR_HANDLE tmr_handle,
                       NET_SVC_TASK_TMR_FNCT   fnct,
                       void                    *p_arg,
                       CPU_INT32U              timeout_ms,
                       RTOS_ERR                *p_err)
{
  NET_SVC_TASK_CTX   *p_ctx;
  NET_SVC_TASK_CHILD *p_child;
  NET_SVC_TASK_TMR   *p_tmr;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((timeout_ms != 0), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_tmr = (NET_SVC_TASK_TMR *)tmr_handle;
  p_child = p_tmr->ChildPtr;
  p_ctx = (NET_SVC_TASK_CTX *)p_child->TaskHandle;

  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  NetSvcTask_TmrListRem(p_ctx, p_tmr);

  p_tmr->Fnct = fnct;
  p_tmr->FnctArg = p_arg;
  p_tmr->Timeout_ms = timeout_ms;
  p_tmr->Delta_ms = timeout_ms;

  p_tmr->TS_ms = NetUtil_TS_Get_ms_Internal();

  SList_Push(&p_ctx->TmrsQ, &p_tmr->ListNode);

  KAL_LockRelease(p_ctx->Lock, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  NetSvcTask_WakeUp(p_ctx);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                           NetSvcTask_TmrDel()
 *
 * @brief    Delete a service task's timer.
 *
 * @param    tmr_handle  Timer handle.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetSvcTask_TmrDel(NET_SVC_TASK_TMR_HANDLE tmr_handle,
                       RTOS_ERR                *p_err)
{
  NET_SVC_TASK_CTX   *p_ctx;
  NET_SVC_TASK_CHILD *p_child;
  NET_SVC_TASK_TMR   *p_tmr;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((tmr_handle != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_tmr = (NET_SVC_TASK_TMR *)tmr_handle;
  p_child = p_tmr->ChildPtr;
  p_ctx = (NET_SVC_TASK_CTX *)p_child->TaskHandle;

  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  NetSvcTask_TmrListRem(p_ctx, p_tmr);

  NetSvcTask_TmrObjFree(p_tmr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  KAL_LockRelease(p_ctx->Lock, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  NetSvcTask_WakeUp(p_ctx);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  goto exit;

exit_release:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    KAL_LockRelease(p_ctx->Lock, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetSvc_SockCreateClient()
 *
 * @brief    Open a network socket for a client type application.
 *
 * @param    p_child             Pointer to service task's child.
 *
 * @param    protocol_family     Socket protocol family :
 *                                   - NET_SOCK_PROTOCOL_FAMILY_IP_V4
 *                                   - NET_SOCK_PROTOCOL_FAMILY_IP_V6
 *
 * @param    sock_type           Socket type :
 *                                   - NET_SOCK_TYPE_DATAGRAM
 *                                   - NET_SOCK_TYPE_STREAM
 *
 * @param    protocol            _family     Socket protocol family :
 *                                   - NET_SOCK_PROTOCOL_FAMILY_IP_V4
 *                                   - NET_SOCK_PROTOCOL_FAMILY_IP_V6
 *
 * @param    flags               Flags :
 *                                   - NET_SVC_TASK_SOCK_FLAG_NONE
 *                                   - NET_SVC_TASK_SOCK_FLAG_BLOCK
 *                                   - NET_SVC_TASK_SOCK_FLAG_BIND
 *
 * @param    p_addr_bind         Pointer to socket address structure containing the address to bind to.
 *
 * @param    addr_len            Length  of socket address structure (in octets).
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @return   Network socket ID, if no errors.
 *           NET_SOCK_ID_NONE. otherwise.
 *******************************************************************************************************/
NET_SOCK_ID NetSvc_SockCreateClient(NET_SVC_TASK_CHILD       *p_child,
                                    NET_SOCK_PROTOCOL_FAMILY protocol_family,
                                    NET_SOCK_TYPE            sock_type,
                                    NET_SOCK_PROTOCOL        protocol,
                                    NET_FLAGS                flags,
                                    NET_SOCK_ADDR            *p_addr_bind,
                                    NET_SOCK_ADDR_LEN        addr_len,
                                    RTOS_ERR                 *p_err)
{
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  NET_SVC_TASK_CTX      *p_ctx;
  NET_SVC_TASK_MSG_SOCK sock_msg;
  NET_SOCK_ID           sock_id = NET_SOCK_ID_NONE;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_SOCK_ID_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_child != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_SOCK_ID_NONE);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_ctx = (NET_SVC_TASK_CTX *)p_child->TaskHandle;

  //                                                               ----------- SET SOCK MESSAGE PARAMETERS ------------
  sock_msg.Type = NET_SVC_TASK_SOCK_TYPE_CLIENT;
  sock_msg.ProtocolFamily = protocol_family;
  sock_msg.SockType = sock_type;
  sock_msg.Protocol = protocol;
  sock_msg.Flags = flags;
  sock_msg.AddrBindPtr = p_addr_bind;
  sock_msg.AddrLen = addr_len;
  sock_msg.SockID = NET_SOCK_ID_NONE;
  sock_msg.ChildPtr = p_child;

  //                                                               ----- POST MESSAGE TO SERVICE TASK TO ADD SOCK -----
  NetSvcTask_MsgPost(p_ctx, NET_SVC_TASK_MSG_TYPE_CHILD_POST_SOCK_CLIENT, &sock_msg, DEF_YES, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return(NET_SOCK_ID_NONE);
  }

  sock_id = sock_msg.SockID;

  return (sock_id);

#else
  PP_UNUSED_PARAM(p_child);
  PP_UNUSED_PARAM(protocol_family);
  PP_UNUSED_PARAM(sock_type);
  PP_UNUSED_PARAM(protocol);
  PP_UNUSED_PARAM(flags);
  PP_UNUSED_PARAM(p_addr_bind);
  PP_UNUSED_PARAM(addr_len);

  RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, NET_SOCK_ID_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_NO)
  return (NET_SOCK_ID_NONE);
#endif

#endif
}

/****************************************************************************************************//**
 *                                           NetSvc_SockCreateServer()
 *
 * @brief    Open a network socket for a server type application.
 *
 * @param    p_child             Pointer to service task's child.
 *
 * @param    protocol_family     Socket protocol family :
 *                                    - NET_SOCK_PROTOCOL_FAMILY_IP_V4
 *                                    - NET_SOCK_PROTOCOL_FAMILY_IP_V6
 *
 * @param    sock_type           Socket type :
 *                                    - NET_SOCK_TYPE_DATAGRAM
 *                                    - NET_SOCK_TYPE_STREAM
 *
 * @param    protocol            _family     Socket protocol family :
 *                                    - NET_SOCK_PROTOCOL_FAMILY_IP_V4
 *                                    - NET_SOCK_PROTOCOL_FAMILY_IP_V6
 *
 * @param    flags               Flags :
 *                                    - NET_SVC_TASK_SOCK_FLAG_NONE
 *                                    - NET_SVC_TASK_SOCK_FLAG_BLOCK
 *                                    - NET_SVC_TASK_SOCK_FLAG_BIND
 *
 * @param    p_addr_bind         Pointer to socket address structure containing the address to bind to.
 *
 * @param    addr_len            Length  of socket address structure (in octets).
 *
 * @param    sock_q_size         Size of the Listen Q for the server type socket.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @return   Network socket ID, if no errors.
 *           NET_SOCK_ID_NONE. otherwise.
 *******************************************************************************************************/
NET_SOCK_ID NetSvc_SockCreateServer(NET_SVC_TASK_CHILD       *p_child,
                                    NET_SOCK_PROTOCOL_FAMILY protocol_family,
                                    NET_SOCK_TYPE            sock_type,
                                    NET_SOCK_PROTOCOL        protocol,
                                    NET_FLAGS                flags,
                                    NET_SOCK_ADDR            *p_addr_bind,
                                    NET_SOCK_ADDR_LEN        addr_len,
                                    NET_SOCK_Q_SIZE          sock_q_size,
                                    RTOS_ERR                 *p_err)
{
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  NET_SVC_TASK_CTX      *p_ctx;
  NET_SVC_TASK_MSG_SOCK sock_msg;
  NET_SOCK_ID           sock_id = NET_SOCK_ID_NONE;

  PP_UNUSED_PARAM(sock_q_size);

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_SOCK_ID_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_child != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_SOCK_ID_NONE);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_ctx = (NET_SVC_TASK_CTX *)p_child->TaskHandle;
  //                                                               ----------- SET SOCK MESSAGE PARAMETERS ------------
  sock_msg.Type = NET_SVC_TASK_SOCK_TYPE_CLIENT;
  sock_msg.ProtocolFamily = protocol_family;
  sock_msg.SockType = sock_type;
  sock_msg.Protocol = protocol;
  sock_msg.Flags = flags;
  sock_msg.AddrBindPtr = p_addr_bind;
  sock_msg.AddrLen = addr_len;
  sock_msg.ChildPtr = p_child;
  sock_msg.SockID = NET_SOCK_ID_NONE;

  //                                                               ----- POST MESSAGE TO SERVICE TASK TO ADD SOCK -----
  NetSvcTask_MsgPost(p_ctx, NET_SVC_TASK_MSG_TYPE_CHILD_POST_SOCK_SERVER, &sock_msg, DEF_YES, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (NET_SOCK_ID_NONE);
  }

  sock_id = sock_msg.SockID;

  return (sock_id);
#else
  PP_UNUSED_PARAM(p_child);
  PP_UNUSED_PARAM(protocol_family);
  PP_UNUSED_PARAM(sock_type);
  PP_UNUSED_PARAM(protocol);
  PP_UNUSED_PARAM(flags);
  PP_UNUSED_PARAM(p_addr_bind);
  PP_UNUSED_PARAM(addr_len);
  PP_UNUSED_PARAM(sock_q_size);

  RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, NET_SOCK_ID_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_NO)
  return (NET_SOCK_ID_NONE);
#endif
#endif
}

/****************************************************************************************************//**
 *                                       NetSvcTask_SockHandleCreate()
 *
 * @brief    Create a new service task socket handle for a child to associate with an existing network
 *           socket.
 *
 * @param    p_child         Pointer to service task's child.
 *
 * @param    sock_id         Network socket id to attach the handle to.
 *
 * @param    type            Socket operation type for the socket :
 *                               - NET_SVC_TASK_SOCK_OP_TYPE_NONE
 *                               - NET_SVC_TASK_SOCK_OP_TYPE_RX_RDY
 *                               - NET_SVC_TASK_SOCK_OP_TYPE_RX_DATA
 *                               - NET_SVC_TASK_SOCK_OP_TYPE_TX_RDY
 *                               - NET_SVC_TASK_SOCK_OP_TYPE_TX_DATA
 *                               - NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_RDY
 *                               - NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_DATA
 *
 * @param    rx_hook         Reception hook.
 *
 * @param    tx_hook         Transmission hook.
 *
 * @param    err_hook        Error hook.
 *
 * @param    p_hook_args     Pointer to object to pass as argument in hooks.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to socket handle, if no errors.
 *           DEF_NULL, otherwise.
 *******************************************************************************************************/
NET_SVC_TASK_SOCK *NetSvcTask_SockHandleCreate(NET_SVC_TASK_CHILD        *p_child,
                                               NET_SOCK_ID               sock_id,
                                               NET_SVC_TASK_SOCK_OP_TYPE type,
                                               NET_SVC_TASK_ON_SOCK_OP   rx_hook,
                                               NET_SVC_TASK_ON_SOCK_OP   tx_hook,
                                               NET_SVC_TASK_ON_SOCK_ERR  err_hook,
                                               void                      *p_hook_args,
                                               RTOS_ERR                  *p_err)
{
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  NET_SVC_TASK_SOCK *p_svc_task_sock = DEF_NULL;
  NET_SVC_TASK_CTX  *p_ctx = DEF_NULL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_child != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (p_child->PoolSockPtr == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return (DEF_NULL);
  }

  p_ctx = (NET_SVC_TASK_CTX *)p_child->TaskHandle;

  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

  p_svc_task_sock = (NET_SVC_TASK_SOCK *)Mem_DynPoolBlkGet(p_child->PoolSockPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_svc_task_sock->SockID = sock_id;
  p_svc_task_sock->OpType = type;
  p_svc_task_sock->OnRx = rx_hook;
  p_svc_task_sock->OnTx = tx_hook;
  p_svc_task_sock->OnErr = err_hook;
  p_svc_task_sock->FnctArgPtr = p_hook_args;

  SList_Push(&p_child->SockListPtr, &p_svc_task_sock->ListNode);

exit_release:
  KAL_LockRelease(p_ctx->Lock, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  return (p_svc_task_sock);

#else
  PP_UNUSED_PARAM(p_child);
  PP_UNUSED_PARAM(sock_id);
  PP_UNUSED_PARAM(type);
  PP_UNUSED_PARAM(rx_hook);
  PP_UNUSED_PARAM(tx_hook);
  PP_UNUSED_PARAM(err_hook);
  PP_UNUSED_PARAM(p_hook_args);

  RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_NULL);

#if (RTOS_ARG_CHK_EXT_EN == DEF_NO)
  return (DEF_NULL);
#endif
#endif
}

/****************************************************************************************************//**
 *                                       NetSvcTask_SockHandleTypeSet()
 *
 * @brief    Change the socket operation type for the given service task's socket handle.
 *
 * @param    p_svc_task_sock     Pointer to service task's socket.
 *
 * @param    type                New value of the socket operation type :
 *                                   - NET_SVC_TASK_SOCK_OP_TYPE_RX_RDY
 *                                   - NET_SVC_TASK_SOCK_OP_TYPE_RX_DATA
 *                                   - NET_SVC_TASK_SOCK_OP_TYPE_TX_RDY
 *                                   - NET_SVC_TASK_SOCK_OP_TYPE_TX_DATA
 *                                   - NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_RDY
 *                                   - NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_DATA
 *******************************************************************************************************/
void NetSvcTask_SockHandleTypeSet(NET_SVC_TASK_SOCK         *p_svc_task_sock,
                                  NET_SVC_TASK_SOCK_OP_TYPE type)
{
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  p_svc_task_sock->OpType = type;
#else
  PP_UNUSED_PARAM(p_svc_task_sock);
  PP_UNUSED_PARAM(type);

  RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
#endif
}

/****************************************************************************************************//**
 *                                       NetSvcTask_SockHandleDel()
 *
 * @brief    Delete a service task's socket on a specific child.
 *
 * @param    p_child             Pointer to service task's child.
 *
 * @param    p_svc_task_sock     Pointer to service task's socket.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetSvcTask_SockHandleDel(NET_SVC_TASK_CHILD *p_child,
                              NET_SVC_TASK_SOCK  *p_svc_task_sock,
                              RTOS_ERR           *p_err)
{
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  NET_SVC_TASK_CTX *p_ctx;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_child != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_svc_task_sock != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_ctx = (NET_SVC_TASK_CTX *)p_child->TaskHandle;

  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

  SList_Rem(&p_child->SockListPtr, &p_svc_task_sock->ListNode);

  Mem_DynPoolBlkFree(p_child->PoolSockPtr, p_svc_task_sock, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  KAL_LockRelease(p_ctx->Lock, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#else
  PP_UNUSED_PARAM(p_child);
  PP_UNUSED_PARAM(p_svc_task_sock);

  RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetSvcTask()
 *
 * @brief    Service task main loop.
 *
 * @param    p_arg   Pointer to task argument.
 *******************************************************************************************************/
static void NetSvcTask(void *p_arg)
{
  NET_SVC_TASK_CTX *p_ctx = (NET_SVC_TASK_CTX *)p_arg;
  RTOS_ERR         local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  while (DEF_ON) {
    NET_SOCK_DESC     sock_rd;
    NET_SOCK_DESC     sock_wr;
    NET_SOCK_DESC     sock_err;
    NET_SOCK_RTN_CODE sock_rdy_ctn = 0;

    NetSvcTask_Pend(p_ctx, &sock_rd, &sock_wr, &sock_err, &sock_rdy_ctn);

    NetSvcTask_Tmr(p_ctx);

    NetSvcTask_Msg(p_ctx);

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
    if (sock_rdy_ctn > 0) {
      NetSvcTask_Sock(p_ctx, &sock_rd, &sock_wr, &sock_err);
    }
#endif

    p_ctx->TS2 = NetUtil_TS_Get_ms_Internal();
  }
}

/****************************************************************************************************//**
 *                                               NetSvcTask_Pend()
 *
 * @brief    Pend for signal from a new message, new timer or socket operations.
 *
 * @param    p_ctx           Pointer to service task context data.
 *
 * @param    p_sock_rd       Pointer to socket descriptor for read operations (RX).
 *
 * @param    p_sock_wr       Pointer to socket descriptor for write operations (TX).
 *
 * @param    p_sock_err      Pointer to socket descriptor for error check.
 *
 * @param    p_sock_rdy_ctn  Pointer to variable that will receive the number of sockets ready.
 *******************************************************************************************************/
static void NetSvcTask_Pend(NET_SVC_TASK_CTX  *p_ctx,
                            NET_SOCK_DESC     *p_sock_rd,
                            NET_SOCK_DESC     *p_sock_wr,
                            NET_SOCK_DESC     *p_sock_err,
                            NET_SOCK_RTN_CODE *p_sock_rdy_ctn)
{
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  NET_SOCK_TIMEOUT timeout_sock;
  NET_SOCK_TIMEOUT *p_timeout_sock;
#else
  CPU_INT32U timeout;
  KAL_OPT    kal_opt;
#endif
  CPU_INT32U   timeout_ms;
  NET_SOCK_QTY nb_sock_max;
  RTOS_ERR     local_err;

  //                                                               ---------- INITIALIZE SOCKET DESCRIPTORS -----------
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  nb_sock_max = NetSvcTask_SockDescInit(p_ctx, p_sock_rd, p_sock_wr, p_sock_err);
#else
  nb_sock_max = 0;
#endif

  *p_sock_rdy_ctn = 0;

  //                                                               ------------ GET THE PEND TIMEOUT VALUE ------------
  timeout_ms = NetSvcTask_TmrTimeoutGet(p_ctx);

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  //                                                               Set timeout variable for socket select.
  if (timeout_ms == NET_TMR_TIME_INFINITE) {
    p_timeout_sock = DEF_NULL;
  } else if (timeout_ms > 0) {
    if (timeout_ms > DEF_TIME_NBR_mS_PER_SEC) {
      timeout_sock.timeout_sec = timeout_ms / DEF_TIME_NBR_mS_PER_SEC;
      timeout_sock.timeout_us = (timeout_ms % DEF_TIME_NBR_mS_PER_SEC) * DEF_TIME_NBR_mS_PER_SEC;
    } else {
      timeout_sock.timeout_sec = 0;
      timeout_sock.timeout_us = timeout_ms * DEF_TIME_NBR_uS_PER_SEC / DEF_TIME_NBR_mS_PER_SEC;
    }
    p_timeout_sock = &timeout_sock;
  } else {
    timeout_sock.timeout_sec = 0;
    timeout_sock.timeout_us = 0;
    p_timeout_sock = &timeout_sock;
  }
#else
  if (timeout_ms == NET_TMR_TIME_INFINITE) {
    kal_opt = KAL_OPT_PEND_NONE;
    timeout = KAL_TIMEOUT_INFINITE;
  } else if (timeout_ms > 0) {
    kal_opt = KAL_OPT_PEND_NONE;
    timeout = timeout_ms;
  } else {
    kal_opt = KAL_OPT_PEND_NON_BLOCKING;
    timeout = 0;
  }
#endif

  //                                                               Lock release before socket select/pend.
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  KAL_LockRelease(p_ctx->Lock, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
  //                                                               Do Socket Select with timeout.
  *p_sock_rdy_ctn = NetSock_SelInternal(p_ctx->TaskSignal,
                                        nb_sock_max,
                                        p_sock_rd,
                                        p_sock_wr,
                                        p_sock_err,
                                        p_timeout_sock,
                                        &local_err);

#else
  KAL_SemPend(p_ctx->TaskSignal, kal_opt, timeout, &local_err);
#endif

  //                                                               Re-Acquire Lock after pend/select.
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  PP_UNUSED_PARAM(p_sock_rd);
  PP_UNUSED_PARAM(p_sock_wr);
  PP_UNUSED_PARAM(p_sock_err);
  PP_UNUSED_PARAM(nb_sock_max);
}

/****************************************************************************************************//**
 *                                           NetSvcTask_WakeUp()
 *
 * @brief    Wake up service task by posting semaphore.
 *
 * @param    p_ctx   Pointer to the service task context.
 *******************************************************************************************************/
static void NetSvcTask_WakeUp(NET_SVC_TASK_CTX *p_ctx)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_SemPost(p_ctx->TaskSignal, KAL_OPT_POST_NONE, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           NetSvcTask_ChildPostMsg()
 *
 * @brief    Post a message to a given service task.
 *
 * @param    p_ctx       Pointer to the service task context.
 *
 * @param    msg_type    Type of the message.
 *
 * @param    p_arg       Pointer to message data.
 *
 * @param    block       DEF_YES, the function will wait for the message processing end.
 *                       DEF_NO,  otherwise.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetSvcTask_MsgPost(NET_SVC_TASK_CTX      *p_ctx,
                               NET_SVC_TASK_MSG_TYPE msg_type,
                               void                  *p_arg,
                               CPU_BOOLEAN           block,
                               RTOS_ERR              *p_err)
{
  NET_SVC_TASK_MSG msg;
  NET_SVC_TASK_MSG *p_msg;
  RTOS_ERR         local_err;

  if (block == DEF_NO) {
    //                                                             ------------ GET MESSAGE BLOCK IN POOL -------------
    p_msg = (NET_SVC_TASK_MSG *)Mem_DynPoolBlkGet(&p_ctx->MsgPool, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    p_msg->MsgEndSignal = KAL_SemHandleNull;

    p_msg->MsgErrPtr = DEF_NULL;
  } else {
    //                                                             ----------- CREATE TASK MESSAGE ON STACK -----------
    p_msg = &msg;
    p_msg->MsgEndSignal = KAL_SemCreate("Net Service Task Child Message Post Signal", DEF_NULL, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
    p_msg->MsgErrPtr = p_err;
  }

  p_msg->Type = msg_type;
  p_msg->ArgPtr = p_arg;

  //                                                               -------------- ACQUIRE INSTANCE LOCK. --------------
  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               ------- ADD MESSAGE TO THE END OF THE QUEUE --------
  SList_PushBack(&p_ctx->MsgQ, &p_msg->ListNode);

  //                                                               -------------- RELEASE INSTANCE LOCK ---------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  KAL_LockRelease(p_ctx->Lock, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               --------------- SIGNAL SERVICE TASK ----------------
  NetSvcTask_WakeUp(p_ctx);

  if (block == DEF_YES) {                                       // ---------- WAIT FOR SERVICE TASK RESPONSE ----------
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    KAL_SemPend(msg.MsgEndSignal, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      SList_Rem(&p_ctx->MsgQ, &msg.ListNode);

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      KAL_LockRelease(p_ctx->Lock, &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      goto exit;
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetSvcTask_Msg()
 *
 * @brief    Process messages received by the service task.
 *
 * @param    p_ctx   Pointer to service task context data.
 *******************************************************************************************************/
static void NetSvcTask_Msg(NET_SVC_TASK_CTX *p_ctx)
{
  SLIST_MEMBER *p_node;
  RTOS_ERR     local_err;

  do {
    p_node = SList_Pop(&p_ctx->MsgQ);
    if (p_node == DEF_NULL) {
      break;
    }

    SList_PushBack(&p_ctx->MsgList, p_node);
  } while (p_node != DEF_NULL);

  //                                                               ------------- RESTORE SEMAPHORE COUNT --------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  KAL_SemSet(p_ctx->TaskSignal, 0, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0);

  do {
    NET_SVC_TASK_MSG *p_msg;
    RTOS_ERR         *p_err;
    RTOS_ERR         err;

    p_node = SList_Pop(&p_ctx->MsgList);
    if (p_node == DEF_NULL) {
      break;
    }

    p_msg = SLIST_ENTRY(p_node, NET_SVC_TASK_MSG, ListNode);
    if (p_msg->MsgErrPtr == DEF_NULL) {
      RTOS_ERR_SET(err, RTOS_ERR_NONE);
      p_err = &err;
    } else {
      p_err = p_msg->MsgErrPtr;
    }

    switch (p_msg->Type) {
      case NET_SVC_TASK_MSG_TYPE_START_CHILD:
        NetSvcTask_ChildAdd(p_ctx,
                            p_msg->ArgPtr,
                            p_err);
        break;

      case NET_SVC_TASK_MSG_TYPE_STOP_CHILD:
        /* TODO_NET
           NetSvcTask_ChildRem(p_ctx,
                             p_msg->ArgPtr,
                 (RTOS_ERR *)p_err);
         */
        break;

      case NET_SVC_TASK_MSG_TYPE_CHILD_POST:
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        KAL_LockRelease(p_ctx->Lock, &local_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        NetSvcTask_ChildMsgDemux(p_msg->ArgPtr, p_err);

        KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, &local_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        break;

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
      case NET_SVC_TASK_MSG_TYPE_CHILD_POST_SOCK_CLIENT:
        NetSvcTask_SockAddClient(p_msg->ArgPtr, p_err);
        break;

      case NET_SVC_TASK_MSG_TYPE_CHILD_POST_SOCK_SERVER:
        NetSvcTask_SockAddServer(p_msg->ArgPtr, p_err);
        break;
#endif

      default:
        break;
    }

    if (KAL_SEM_HANDLE_IS_NULL(p_msg->MsgEndSignal) == DEF_NO) {
      KAL_SemPost(p_msg->MsgEndSignal, KAL_OPT_POST_NONE, &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    } else if (p_msg->BlkPtr != DEF_NULL) {
      Mem_DynPoolBlkFree(&p_ctx->MsgPool, p_msg->BlkPtr, &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }
  } while (p_node != DEF_NULL);
}

/****************************************************************************************************//**
 *                                           NetSvcTask_ChildAdd()
 *
 * @brief    Create and add a child to the given service task.
 *
 * @param    p_ctx       Pointer to service task context data.
 *
 * @param    p_msg_arg   Pointer to the message data.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetSvcTask_ChildAdd(NET_SVC_TASK_CTX *p_ctx,
                                void             *p_msg_arg,
                                RTOS_ERR         *p_err)
{
  NET_SVC_TASK_CHILD     *p_child = (NET_SVC_TASK_CHILD *)p_msg_arg;
  NET_SVC_TASK_HOOKS     *p_hooks = p_child->HooksPtr;
  NET_SVC_TASK_CHILD_CFG *p_cfg = p_child->CfgPtr;

  RTOS_ASSERT_DBG_ERR_SET((p_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               ------------- CREATE CHILD TIMER POOL --------------
  if (p_cfg->TmrNbrMax != 0) {
    p_child->PoolTmrPtr = (MEM_DYN_POOL *)Mem_SegAlloc("Net Service Child Timer pool object",
                                                       p_child->CfgPtr->MemSegPtr,
                                                       sizeof(MEM_DYN_POOL),
                                                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    Mem_DynPoolCreate("Net Service Child Timer pool",
                      p_child->PoolTmrPtr,
                      p_child->CfgPtr->MemSegPtr,
                      sizeof(NET_SVC_TASK_TMR),
                      sizeof(CPU_ALIGN),
                      p_cfg->TmrNbrMax,
                      p_cfg->TmrNbrMax,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  //                                                               ------------- CREATE CHILD SOCKET POOL -------------
  if (p_cfg->SockNbrMax != 0) {
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
    p_child->PoolSockPtr = (MEM_DYN_POOL *)Mem_SegAlloc("Net Service Child Sock pool object",
                                                        p_child->CfgPtr->MemSegPtr,
                                                        sizeof(MEM_DYN_POOL),
                                                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    Mem_DynPoolCreate("Net Service Child Sock pool",
                      p_child->PoolSockPtr,
                      p_child->CfgPtr->MemSegPtr,
                      sizeof(NET_SVC_TASK_SOCK),
                      sizeof(CPU_ALIGN),
                      p_cfg->SockNbrMax,
                      p_cfg->SockNbrMax,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
  }

  //                                                               ------------ CREATE CHILD MESSAGE POOL -------------
  if (p_cfg->MsgNbrMax != 0) {
    p_child->PoolMsgPtr = (MEM_DYN_POOL *)Mem_SegAlloc("Net Service Child Message pool object",
                                                       p_child->CfgPtr->MemSegPtr,
                                                       sizeof(MEM_DYN_POOL),
                                                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    Mem_DynPoolCreate("Net Service Child Timer pool",
                      p_child->PoolMsgPtr,
                      p_child->CfgPtr->MemSegPtr,
                      sizeof(NET_SVC_TASK_MSG_CHILD),
                      sizeof(CPU_ALIGN),
                      p_cfg->MsgNbrMax,
                      p_cfg->MsgNbrMax,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  //                                                               ------------- CALL CHILD'S START HOOK --------------
  if (p_hooks->OnStart != DEF_NULL) {
    p_hooks->OnStart(p_child, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  //                                                               -------- ADD CHILD TO SVC TASK CHILDS' LIST --------
  SList_PushBack(&p_ctx->Childs, &p_child->ListNode);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    if (p_hooks->OnErr != DEF_NULL) {
      p_hooks->OnErr(p_child, *p_err);
    }
  }
}

/****************************************************************************************************//**
 *                                       NetSvcTask_ChildMsgDemux()
 *
 * @brief    Process a message addressed to a specific child.
 *
 * @param    p_arg   Pointer to message data.
 *
 * @param    p_err   Pointer to the variable that will receive the error code from this function.
 *******************************************************************************************************/
static void NetSvcTask_ChildMsgDemux(void     *p_arg,
                                     RTOS_ERR *p_err)
{
  NET_SVC_TASK_MSG_CHILD *p_child_msg;
  NET_SVC_TASK_CHILD     *p_child;
  NET_SVC_TASK_HOOKS     *p_hooks;
  RTOS_ERR               local_err;

  p_child_msg = (NET_SVC_TASK_MSG_CHILD *)p_arg;
  p_child = p_child_msg->ChildPtr;
  p_hooks = p_child->HooksPtr;

  if (p_hooks->OnMsg != DEF_NULL) {
    p_hooks->OnMsg(p_child, p_child_msg->ChildMsg, p_child_msg->ChildMsgArgPtr, p_err);
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    if (p_hooks->OnErr != DEF_NULL) {
      p_hooks->OnErr(p_child, *p_err);
    }
  }

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  Mem_DynPoolBlkFree(p_child->PoolMsgPtr, p_child_msg, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               NetSvcTask_Tmr()
 *
 * @brief    This function update the timer list at each iteration of the main loop.
 *
 * @param    p_ctx       Pointer to service task context data.
 *
 * @param    p_ts_prev   Pointer to variable that contains the previous TS and will receive the current
 *                       TS for the next time.
 *******************************************************************************************************/
static void NetSvcTask_Tmr(NET_SVC_TASK_CTX *p_ctx)
{
  NET_SVC_TASK_TMR *p_tmr;
  SLIST_MEMBER     *p_node;
  CPU_INT32U       ts_delta;
  RTOS_ERR         local_err;

  //                                                               Get ms value elapse since last timers process.
  ts_delta = NetSvcTask_TmrDeltaGet(p_ctx);

  //                                                               Parse Timers' list for timed out timer.
  SLIST_FOR_EACH_ENTRY(p_ctx->Tmrs, p_tmr, NET_SVC_TASK_TMR, ListNode) {
    if (p_tmr->Delta_ms > ts_delta) {
      p_tmr->Delta_ms -= ts_delta;
      break;
    } else {
      ts_delta -= p_tmr->Delta_ms;
      p_tmr->Delta_ms = 0;

      NetSvcTask_TmrListRem(p_ctx, p_tmr);
      SList_Push(&p_ctx->TmrsFree, &p_tmr->ListNode);
    }
  }

  //                                                               Add new timers to the Timers' list.
  while (p_ctx->TmrsQ != DEF_NULL) {
    CPU_INT32U delta_ms;

    p_node = SList_Pop(&p_ctx->TmrsQ);

    p_tmr = SLIST_ENTRY(p_node, NET_SVC_TASK_TMR, ListNode);

    //                                                             Remove the time elapse since the timer creation and
    //                                                             the moment the timer is added to the list.
    if (p_tmr->TS_ms > p_ctx->TS1) {
      delta_ms = (DEF_INT_32U_MAX_VAL - p_tmr->TS_ms) + p_ctx->TS1;
    } else {
      delta_ms = p_ctx->TS1 - p_tmr->TS_ms;
    }

    if (p_tmr->Delta_ms > delta_ms) {
      p_tmr->Delta_ms -= delta_ms;

      NetSvcTask_TmrListInsert(p_ctx, p_tmr);
    } else {
      p_tmr->Delta_ms = 0;

      SList_Push(&p_ctx->TmrsFree, &p_tmr->ListNode);
    }
  }

  //                                                               Release Service task lock.
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  KAL_LockRelease(p_ctx->Lock, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               Parse the timed out timers and call their hook fnct.
  while (p_ctx->TmrsFree != DEF_NULL) {
    p_node = SList_Pop(&p_ctx->TmrsFree);

    p_tmr = SLIST_ENTRY(p_node, NET_SVC_TASK_TMR, ListNode);

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    p_tmr->Fnct(p_tmr->ChildPtr, p_tmr->FnctArg, &local_err);

    switch (p_tmr->Type) {
      case NET_SVC_TASK_TMR_TYPE_PERIODIC:
        p_tmr->Delta_ms = p_tmr->Timeout_ms;
        NetSvcTask_TmrListInsert(p_ctx, p_tmr);
        break;

      case NET_SVC_TASK_TMR_TYPE_ONE_SHOT:
      default:
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        NetSvcTask_TmrObjFree(p_tmr, &local_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        break;
    }
  }

  //                                                               Re-acquire Service Task lock.
  KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           NetSvcTask_TmrDeltaGet()
 *
 * @brief    Get the milliseconds value elapse since last TS acquired in the timers processing.
 *
 * @param    p_ctx   Pointer to the current service task context data.
 *
 * @return   Milliseconds delta since last TS acquired in the timers processing.
 *******************************************************************************************************/
static CPU_INT32U NetSvcTask_TmrDeltaGet(NET_SVC_TASK_CTX *p_ctx)
{
  CPU_INT32U ts_cur;
  CPU_INT32U ts_delta;

  //                                                               Get current timestamp.
  ts_cur = NetUtil_TS_Get_ms_Internal();

  //                                                               Get timestamp delta.
  if (p_ctx->TS1 > ts_cur) {
    ts_delta = (DEF_INT_32U_MAX_VAL - p_ctx->TS1) + ts_cur;
  } else {
    ts_delta = ts_cur - p_ctx->TS1;
  }

  //                                                               Save the latest Timestamp.
  p_ctx->TS1 = ts_cur;

  return (ts_delta);
}

/****************************************************************************************************//**
 *                                       NetSvcTask_TmrTimeoutGet()
 *
 * @brief    Get the smallest timeout value in the timer list or the maximum timeout value possible
 *           if the smallest timer timeout is to big.
 *
 * @param    p_ctx   Pointer to service task context data.
 *
 * @return   Timeout value.
 *******************************************************************************************************/
static CPU_INT32U NetSvcTask_TmrTimeoutGet(NET_SVC_TASK_CTX *p_ctx)
{
  CPU_INT32U timeout_ms = NET_TMR_TIME_INFINITE;

  if (p_ctx->Tmrs != DEF_NULL) {
    NET_SVC_TASK_TMR *p_tmr = DEF_NULL;
    CPU_INT32U       timeout_max_ms = 0;
    CPU_INT32U       delta_ts_process = 0;

    p_tmr = SLIST_ENTRY(p_ctx->Tmrs, NET_SVC_TASK_TMR, ListNode);
    timeout_ms = p_tmr->Delta_ms;

    if (p_ctx->TS1 > p_ctx->TS2) {
      delta_ts_process = (DEF_INT_32U_MAX_VAL - p_ctx->TS1) + p_ctx->TS2;
    } else {
      delta_ts_process = p_ctx->TS2 - p_ctx->TS1;
    }

    timeout_max_ms = NetUtil_TS_GetMaxDly_ms() - delta_ts_process;

    timeout_ms = DEF_MIN(timeout_ms, timeout_max_ms);
  }

  return (timeout_ms);
}

/****************************************************************************************************//**
 *                                       NetSvcTask_TmrListInsert()
 *
 * @brief    Add a new timer to the timers list.
 *
 * @param    p_ctx   Pointer to service task context data.
 *
 * @param    p_tmr   Pointer to the new timer to add.
 *******************************************************************************************************/
static void NetSvcTask_TmrListInsert(NET_SVC_TASK_CTX *p_ctx,
                                     NET_SVC_TASK_TMR *p_tmr)
{
  NET_SVC_TASK_TMR *p_tmr_node = DEF_NULL;
  NET_SVC_TASK_TMR *p_tmr_prev = DEF_NULL;

  if (p_ctx->Tmrs != DEF_NULL) {
    SLIST_FOR_EACH_ENTRY(p_ctx->Tmrs, p_tmr_node, NET_SVC_TASK_TMR, ListNode) {
      if (p_tmr->Delta_ms >= p_tmr_node->Delta_ms) {
        p_tmr->Delta_ms -= p_tmr_node->Delta_ms;
        p_tmr_prev = p_tmr_node;

        if (p_tmr_node->ListNode.p_next != DEF_NULL) {
          continue;
        } else {
          SList_PushBack(&p_ctx->Tmrs, &p_tmr->ListNode);
          break;
        }
      } else {
        p_tmr_node->Delta_ms -= p_tmr->Delta_ms;

        if (p_tmr_prev != DEF_NULL) {
          SList_Add(&p_tmr->ListNode, &p_tmr_prev->ListNode);
        } else {
          SList_Push(&p_ctx->Tmrs, &p_tmr->ListNode);
        }
        break;
      }
    }
  } else {
    SList_Push(&p_ctx->Tmrs, &p_tmr->ListNode);
  }
}

/****************************************************************************************************//**
 *                                           NetSvcTask_TmrListRem()
 *
 * @brief    Remove a timer from the timers list.
 *
 * @param    p_ctx   Pointer to service task context data.
 *
 * @param    p_tmr   Pointer to the timer to remove.
 *******************************************************************************************************/
static void NetSvcTask_TmrListRem(NET_SVC_TASK_CTX *p_ctx,
                                  NET_SVC_TASK_TMR *p_tmr)
{
  NET_SVC_TASK_TMR *p_tmr_node;
  CPU_BOOLEAN      found = DEF_NO;

  if (p_ctx->Tmrs != DEF_NULL) {
    SLIST_FOR_EACH_ENTRY(p_ctx->Tmrs, p_tmr_node, NET_SVC_TASK_TMR, ListNode) {
      if (p_tmr == p_tmr_node) {
        found = DEF_YES;
        SList_Rem(&p_ctx->Tmrs, &p_tmr->ListNode);
        continue;
      }

      if (found == DEF_YES) {
        p_tmr_node->Delta_ms += p_tmr->Delta_ms;
        break;
      }
    }
  }

  if (p_ctx->TmrsQ != DEF_NULL) {
    SLIST_FOR_EACH_ENTRY(p_ctx->TmrsQ, p_tmr_node, NET_SVC_TASK_TMR, ListNode) {
      if (p_tmr == p_tmr_node) {
        SList_Rem(&p_ctx->TmrsQ, &p_tmr->ListNode);
        break;
      }
    }
  }

  if (p_ctx->TmrsFree != DEF_NULL) {
    SLIST_FOR_EACH_ENTRY(p_ctx->TmrsFree, p_tmr_node, NET_SVC_TASK_TMR, ListNode) {
      if (p_tmr == p_tmr_node) {
        SList_Rem(&p_ctx->TmrsFree, &p_tmr->ListNode);
        break;
      }
    }
  }
}

/****************************************************************************************************//**
 *                                           NetSvcTask_TmrObjAlloc()
 *
 * @brief    Get a timer object from the service task's child timer pool.
 *
 * @param    p_child     Pointer to child.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to timer object, if no errors.
 *           DEF_NULL, otherwise.
 *******************************************************************************************************/
static NET_SVC_TASK_TMR *NetSvcTask_TmrObjAlloc(NET_SVC_TASK_CHILD *p_child,
                                                RTOS_ERR           *p_err)
{
  NET_SVC_TASK_TMR *p_tmr = DEF_NULL;

  if (p_child->PoolTmrPtr == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    goto exit;
  }

  p_tmr = (NET_SVC_TASK_TMR *)Mem_DynPoolBlkGet(p_child->PoolTmrPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (p_tmr);
}

/****************************************************************************************************//**
 *                                           NetSvcTask_TmrFree()
 *
 * @brief    Free a timer object.
 *
 * @param    p_tmr   Pointer to timer object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetSvcTask_TmrObjFree(NET_SVC_TASK_TMR *p_tmr,
                                  RTOS_ERR         *p_err)
{
  NET_SVC_TASK_CHILD *p_child = p_tmr->ChildPtr;

  if (p_child->PoolTmrPtr == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    goto exit;
  }

  Mem_DynPoolBlkFree(p_tmr->ChildPtr->PoolTmrPtr, p_tmr, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetSvcTask_SockAddClient()
 *
 * @brief    Open a socket in client mode for a service task's child.
 *
 * @param    p_arg   Pointer to the message received by the service task.
 *
 * @param    p_err   Pointer to the variable that will receive the error code(s) from this function.
 *******************************************************************************************************/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static void NetSvcTask_SockAddClient(void     *p_arg,
                                     RTOS_ERR *p_err)
{
  NET_SVC_TASK_CHILD    *p_child;
  NET_SVC_TASK_MSG_SOCK *p_msg;
  NET_SVC_TASK_HOOKS    *p_hooks;
  NET_SOCK_ID           sock_id;
  CPU_INT08U            block;
  CPU_BOOLEAN           is_blocking;
  CPU_BOOLEAN           bind;
  RTOS_ERR              local_err;

  p_msg = (NET_SVC_TASK_MSG_SOCK *)p_arg;
  p_child = p_msg->ChildPtr;
  p_hooks = p_child->HooksPtr;

  //                                                               -------------------- OPEN SOCK ---------------------
  sock_id = NetSock_Open(p_msg->ProtocolFamily,
                         p_msg->SockType,
                         p_msg->Protocol,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_err;
  }

  //                                                               ------------- SET SOCKET BLOCKING MODE -------------
  is_blocking = DEF_BIT_IS_SET(p_msg->Flags, NET_SVC_TASK_SOCK_FLAG_BLOCK);
  block = (is_blocking == DEF_YES) ? NET_SOCK_BLOCK_SEL_BLOCK : NET_SOCK_BLOCK_SEL_NO_BLOCK;

  (void)NetSock_CfgBlock(sock_id, block, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_err;
  }

  //                                                               ------------ BIND SOCKET TO LOCAL ADDR -------------
  bind = DEF_BIT_IS_SET(p_msg->Flags, NET_SVC_TASK_SOCK_FLAG_BIND);
  if (bind == DEF_YES) {
    (void)NetSock_Bind(sock_id,
                       p_msg->AddrBindPtr,
                       p_msg->AddrLen,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_err;
    }
  }

  p_msg->SockID = sock_id;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  goto exit;

exit_err:
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    if (p_hooks->OnErr != DEF_NULL) {
      p_hooks->OnErr(p_child, *p_err);
    }
  }

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetSock_Close(sock_id,
                &local_err);

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                       NetSvcTask_SockAddServer()
 *
 * @brief    Open a socket in server mode for a service task's child.
 *
 * @param    p_arg   Pointer to the message received by the service task.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static void NetSvcTask_SockAddServer(void     *p_arg,
                                     RTOS_ERR *p_err)
{
  NET_SVC_TASK_CHILD    *p_child;
  NET_SVC_TASK_MSG_SOCK *p_msg;
  NET_SVC_TASK_HOOKS    *p_hooks;
  NET_SOCK_ID           sock_id;
  CPU_INT08U            block;
  CPU_BOOLEAN           is_blocking;
  CPU_BOOLEAN           bind;
  RTOS_ERR              local_err;

  p_msg = (NET_SVC_TASK_MSG_SOCK *)p_arg;
  p_child = p_msg->ChildPtr;
  p_hooks = p_child->HooksPtr;

  //                                                               -------------------- OPEN SOCK ---------------------
  sock_id = NetSock_Open(p_msg->ProtocolFamily,
                         p_msg->SockType,
                         p_msg->Protocol,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_err;
  }

  //                                                               ------------- SET SOCKET BLOCKING MODE -------------
  is_blocking = DEF_BIT_IS_SET(p_msg->Flags, NET_SVC_TASK_SOCK_FLAG_BLOCK);
  block = (is_blocking == DEF_YES) ? NET_SOCK_BLOCK_SEL_BLOCK : NET_SOCK_BLOCK_SEL_NO_BLOCK;

  (void)NetSock_CfgBlock(sock_id, block, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_err;
  }

  //                                                               ------------ BIND SOCKET TO LOCAL ADDR -------------
  bind = DEF_BIT_IS_SET(p_msg->Flags, NET_SVC_TASK_SOCK_FLAG_BIND);
  if (bind == DEF_YES) {
    (void)NetSock_Bind(sock_id,
                       p_msg->AddrBindPtr,
                       p_msg->AddrLen,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_err;
    }
  }

  //                                                               TODO_NET : Listen Socket

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  goto exit;

exit_err:
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    if (p_hooks->OnErr != DEF_NULL) {
      p_hooks->OnErr(p_child, *p_err);
    }
  }

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetSock_Close(sock_id,
                &local_err);

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                           NetSvcTask_SockDescInit()
 *
 * @brief    Initialize the socket descriptors before the socket select operations.
 *
 * @param    p_ctx       Pointer to the service task context.
 *
 * @param    p_sock_rd   Pointer to a set of socket descriptors to check for available read operation(s).
 *                       Will return the set of descriptors with the socket ready for read operation.
 *
 * @param    p_sock_wr   Pointer to a set of socket descriptors to check for available write operation(s).
 *                       Will return the set of descriptors with the socket ready for write operation.
 *
 * @param    p_sock_err  Pointer to a set of socket descriptors to check for available error(s).
 *                       Will return the set of descriptors for socket with errors.
 *
 * @return   Number of socket ready for read/write operations or with error.
 *******************************************************************************************************/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static NET_SOCK_QTY NetSvcTask_SockDescInit(NET_SVC_TASK_CTX *p_ctx,
                                            NET_SOCK_DESC    *p_sock_rd,
                                            NET_SOCK_DESC    *p_sock_wr,
                                            NET_SOCK_DESC    *p_sock_err)
{
  NET_SOCK_QTY      sock_nbr_max = 0;
  NET_SOCK_RTN_CODE ctr = 0;
  SLIST_MEMBER      *p_node;

  NET_SOCK_DESC_INIT(p_sock_rd);
  NET_SOCK_DESC_INIT(p_sock_wr);
  NET_SOCK_DESC_INIT(p_sock_err);

  SLIST_FOR_EACH(p_ctx->Childs, p_node) {
    NET_SVC_TASK_CHILD *p_child;
    NET_SVC_TASK_SOCK  *p_sock;
    SLIST_MEMBER       *p_sock_node;

    p_child = SLIST_ENTRY(p_node, NET_SVC_TASK_CHILD, ListNode);

    SLIST_FOR_EACH(p_child->SockListPtr, p_sock_node) {
      p_sock = SLIST_ENTRY(p_sock_node, NET_SVC_TASK_SOCK, ListNode);
      switch (p_sock->OpType) {
        case NET_SVC_TASK_SOCK_OP_TYPE_RX_RDY:
        case NET_SVC_TASK_SOCK_OP_TYPE_RX_DATA:
          NET_SOCK_DESC_SET(p_sock->SockID, p_sock_rd);
          NET_SOCK_DESC_SET(p_sock->SockID, p_sock_err);
          ctr = p_sock->SockID + 1;
          break;

        case NET_SVC_TASK_SOCK_OP_TYPE_TX_RDY:
        case NET_SVC_TASK_SOCK_OP_TYPE_TX_DATA:
          NET_SOCK_DESC_SET(p_sock->SockID, p_sock_wr);
          NET_SOCK_DESC_SET(p_sock->SockID, p_sock_err);
          ctr = p_sock->SockID + 1;
          break;

        case NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_RDY:
        case NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_DATA:
          NET_SOCK_DESC_SET(p_sock->SockID, p_sock_rd);
          NET_SOCK_DESC_SET(p_sock->SockID, p_sock_wr);
          NET_SOCK_DESC_SET(p_sock->SockID, p_sock_err);
          ctr = p_sock->SockID + 1;
          break;

        case NET_SVC_TASK_SOCK_OP_TYPE_NONE:
        default:
          break;
      }

      if (sock_nbr_max <= ctr) {
        sock_nbr_max = ctr;
      }
    }
  }

  return (sock_nbr_max);
}
#endif

/****************************************************************************************************//**
 *                                               NetSvcTask_Sock()
 *
 * @brief    Process all the sockets on a service task that are ready for operations.
 *
 * @param    p_ctx       Pointer to the service task context.
 *
 * @param    p_sock_rd   Pointer to a set of socket descriptors containing socket ready for read operation.
 *
 * @param    p_sock_wr   Pointer to a set of socket descriptors containing socket ready for write operation.
 *
 * @param    p_sock_err  Pointer to a set of socket descriptors containing socket with errors.
 *******************************************************************************************************/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static void NetSvcTask_Sock(NET_SVC_TASK_CTX *p_ctx,
                            NET_SOCK_DESC    *p_sock_rd,
                            NET_SOCK_DESC    *p_sock_wr,
                            NET_SOCK_DESC    *p_sock_err)
{
  SLIST_MEMBER *p_node;

  SLIST_FOR_EACH(p_ctx->Childs, p_node) {
    NET_SVC_TASK_CHILD *p_child;
    NET_SVC_TASK_SOCK  *p_sock;
    SLIST_MEMBER       *p_sock_node;

    p_child = SLIST_ENTRY(p_node, NET_SVC_TASK_CHILD, ListNode);

    SLIST_FOR_EACH(p_child->SockListPtr, p_sock_node) {
      CPU_BOOLEAN rx_rdy = DEF_NO;
      CPU_BOOLEAN tx_rdy = DEF_NO;
      CPU_BOOLEAN err = DEF_NO;
      RTOS_ERR    local_err;

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      KAL_LockRelease(p_ctx->Lock, &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      p_sock = SLIST_ENTRY(p_sock_node, NET_SVC_TASK_SOCK, ListNode);

      rx_rdy = NET_SOCK_DESC_IS_SET(p_sock->SockID, p_sock_rd);
      tx_rdy = NET_SOCK_DESC_IS_SET(p_sock->SockID, p_sock_wr);
      err = NET_SOCK_DESC_IS_SET(p_sock->SockID, p_sock_err);

      if (err == DEF_YES) {
        if (p_sock->OnErr != DEF_NULL) {
          p_sock->OnErr(p_child, p_sock->SockID, p_sock->FnctArgPtr);
        }
      }

      switch (p_sock->OpType) {
        case NET_SVC_TASK_SOCK_OP_TYPE_RX_RDY:
          if (rx_rdy == DEF_YES) {
            RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
            p_sock->OnRx(p_child, p_sock->SockID, p_sock->FnctArgPtr, &local_err);
          }
          break;

        case NET_SVC_TASK_SOCK_OP_TYPE_TX_RDY:
          if (tx_rdy == DEF_YES) {
            RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
            p_sock->OnTx(p_child, p_sock->SockID, p_sock->FnctArgPtr, &local_err);
          }
          break;

        case NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_RDY:
          if (rx_rdy == DEF_YES) {
            RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
            p_sock->OnRx(p_child, p_sock->SockID, p_sock->FnctArgPtr, &local_err);
          }
          if (tx_rdy == DEF_YES) {
            RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
            p_sock->OnTx(p_child, p_sock->SockID, p_sock->FnctArgPtr, &local_err);
          }
          break;

        case NET_SVC_TASK_SOCK_OP_TYPE_RX_DATA:
          if (rx_rdy == DEF_YES) {
            /* TODO_NET
               NetSvcTask_SockRxData(p_child, p_sock);
             */
          }
          break;

        case NET_SVC_TASK_SOCK_OP_TYPE_TX_DATA:
          if (tx_rdy == DEF_YES) {
            /* TODO_NET
               NetSvcTask_SockTxData(p_child, p_sock);
             */
          }
          break;

        case NET_SVC_TASK_SOCK_OP_TYPE_RX_TX_DATA:
          //                                                       TODO_NET
          break;

        case NET_SVC_TASK_SOCK_OP_TYPE_NONE:
        default:
          break;
      }

      KAL_LockAcquire(p_ctx->Lock, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }
  }
}
#endif

#if 0
static void NetSvcTask_SockTxData(NET_SVC_TASK_CHILD *p_child,
                                  NET_SVC_TASK_SOCK  *p_sock)
{
  NET_SOCK_RTN_CODE rtn_code;

  rtn_code = NetSock_TxDataTo();
}
#endif

/****************************************************************************************************//**
 *                                       NetSvcTask_StopReqSignal()
 *
 * @brief    Signal that the instance must be stopped.
 *
 * @param    p_ctx   Pointer to the service task context.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#if 0
static void NetSvcTask_StopReqSignal(NET_SVC_TASK_CTX *p_ctx,           // TODO_NET : keep function ?
                                     RTOS_ERR         *p_err)
{
  KAL_SemPost(p_ctx->SemStopReq,                               // Signal instance that stop is requested.
              KAL_OPT_POST_NONE,
              p_err);
}
#endif

/****************************************************************************************************//**
 *                                       NetSvcTask_IsStopReqPending()
 *
 * @brief    Get stop request.
 *
 * @param    p_ctx   Pointer to the service task context.
 *
 * @return   DEF_YES, if stop is requested or a fatal error occurred.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
#if 0
static CPU_BOOLEAN NetSvcTask_IsStopReqPending(NET_SVC_TASK_CTX *p_ctx)
{
  RTOS_ERR local_err;

  KAL_SemPend(p_ctx->SemStopReq,
              KAL_OPT_PEND_NON_BLOCKING,
              1u,
              &local_err);

  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_WOULD_BLOCK:
    case RTOS_ERR_OS:
    case RTOS_ERR_TIMEOUT:
      return (DEF_NO);

    case RTOS_ERR_NONE:                                         // Event occurred.
    case RTOS_ERR_NULL_PTR:                                     // Should not be null.
    case RTOS_ERR_NOT_AVAIL:
    case RTOS_ERR_INVALID_ARG:
    case RTOS_ERR_ABORT:
    case RTOS_ERR_ISR:
    default:
      return (DEF_YES);
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetSvcTask_StopCompletedSignal()
 *
 * @brief    Signal that the stop request has been completed.
 *
 * @param    p_ctx   Pointer to the service task context.
 *******************************************************************************************************/
#if 0
static void NetSvcTask_StopCompletedSignal(NET_SVC_TASK_CTX *p_ctx)
{
  RTOS_ERR local_err;

  KAL_SemPost(p_ctx->SemStopCompleted,
              KAL_OPT_POST_NONE,
              &local_err);
}
#endif

/****************************************************************************************************//**
 *                                       NetSvcTask_WaitStopCompleted()
 *
 * @brief    Wait until the stop request has been completed.
 *
 * @param    p_ctx   Pointer to the service task context.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#if 0
static void NetSvcTask_WaitStopCompleted(NET_SVC_TASK_CTX *p_ctx,
                                         RTOS_ERR         *p_err)
{
  KAL_SemPend(p_ctx->SemStopCompleted,
              KAL_OPT_PEND_BLOCKING,
              0u,                                               // Infinite timeout.
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
  }
}
#endif

/****************************************************************************************************//**
 *                                           NetSvcTask_ObjsAlloc()
 *
 * Description : Allocate required objects for the service task.
 *
 * Argument(s) : p_ctx   Pointer to service task context.
 *
 *               p_err   Pointer to variable that will receive the return error code from this function.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void NetSvcTask_ObjsAlloc(NET_SVC_TASK_CTX *p_ctx,
                                 RTOS_ERR         *p_err)
{
  //                                                               ------ ACQUIRE TASK AND TASK STACK MEM SPACE -------
  p_ctx->Task = KAL_TaskAlloc("Network Service task",
                              (CPU_STK *)p_ctx->StkPtr,
                              p_ctx->StkSizeElements,
                              DEF_NULL,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ---------- ACQUIRE OS_LOCK_OBJ MEM SPACE -----------
  p_ctx->Lock = KAL_LockCreate("Net Svc Task Lock",
                               DEF_NULL,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               -------- CREATE TASK WAKE-UP/PENDING SIGNAL --------
  p_ctx->TaskSignal = KAL_SemCreate("Net service task pending signal",
                                    DEF_NULL,
                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               --------------- CREATE STOP REQ SEM ----------------
  p_ctx->SemStopReq = KAL_SemCreate("Net Svc Task Stop Request Signal",
                                    DEF_NULL,
                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               ------------- CREATE STOP COMPLETE SEM -------------
  p_ctx->SemStopCompleted = KAL_SemCreate("Net Svc Task Stop Completed Signal",
                                          DEF_NULL,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  goto exit;

exit_release:
  NetSvcTask_ObjsFree(p_ctx);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetSvcTask_ObjsFree()
 *
 * @brief    Free all the allocated objects of the given service task.
 *
 * @param    p_ctx   Pointer to the service task context.
 *******************************************************************************************************/
static void NetSvcTask_ObjsFree(NET_SVC_TASK_CTX *p_ctx)
{
  if (KAL_LOCK_HANDLE_IS_NULL(p_ctx->Lock) == DEF_NO) {
    KAL_LockDel(p_ctx->Lock);
  }

  if (KAL_SEM_HANDLE_IS_NULL(p_ctx->TaskSignal) == DEF_NO) {
    KAL_SemDel(p_ctx->TaskSignal);
  }

  if (KAL_SEM_HANDLE_IS_NULL(p_ctx->SemStopReq) == DEF_NO) {
    KAL_SemDel(p_ctx->SemStopReq);
  }

  if (KAL_SEM_HANDLE_IS_NULL(p_ctx->SemStopCompleted) == DEF_NO) {
    KAL_SemDel(p_ctx->SemStopCompleted);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
