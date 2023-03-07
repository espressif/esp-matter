/***************************************************************************//**
 * @file
 * @brief Network - MQTT Client
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

#if (defined(RTOS_MODULE_NET_MQTT_CLIENT_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error MQTT Client Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <common/include/lib_def.h>
#include  <common/include/lib_str.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>
#include  <common/include/rtos_prio.h>

#include  <rtos_cfg.h>
#include  <rtos_description.h>
#include  <cpu_cfg.h>

#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

#include  <cpu/include/cpu.h>

#include  <net/include/net_ascii.h>
#include  <net/include/net_util.h>
#include  <net/include/net_app.h>
#include  <net/source/tcpip/net_sock_priv.h>

#include  "mqtt_client_sock_priv.h"
#include  "mqtt_priv.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Offset in Rx publish buf to provide room to send ...
//                                                                 a potential ACK without copying the whole buf.
#define  MQTTc_PUBLISH_RX_MSG_BUF_OFFSET                            4u

#define  MQTTc_CFG_DBG_GLOBAL_BUF_LEN                               0u

#define  LOG_DFLT_CH                       (NET, MQTT)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_NET

#define  MQTTc_INIT_CFG_DFLT                 {                              \
    .QtyCfg =                                                               \
    {                                                                       \
      .MaxMsgNbr = MQTT_CLIENT_CFG_MSG_NBR_MAX_DFLT,                        \
    },                                                                      \
    .InactivityDfltTimeout_s = MQTT_CLIENT_CFG_INACTIVITY_TIMEOUT_SEC_DFLT, \
    .StkSizeElements = MQTT_CLIENT_TASK_CFG_STK_SIZE_ELEMENTS_DFLT,         \
    .StkPtr = MQTT_CLIENT_TASK_CFG_STK_PTR_DFLT,                            \
    .MemSegPtr = MQTT_CLIENT_MEM_SEG_PTR_DFLT                               \
}

/********************************************************************************************************
 *                                           DFLT VALUES DEFINES
 *******************************************************************************************************/

#define  MQTTc_TIMEOUT_MS_DFLT_VAL                             10000u
#define  MQTTc_BROKER_PORT_NBR_DFLT_VAL                         1883u
#define  MQTTc_KEEP_ALIVE_TIMER_SEC_DFLT_VAL                       0u

/********************************************************************************************************
 *                                                   DBG
 *******************************************************************************************************/

#if (MQTTc_CFG_DBG_GLOBAL_BUF_LEN > 0u)
#define  MQTTc_DBG_GLOBAL_BUF_COPY(p_buf, len)       Mem_Copy(MQTTc_Dbg_GlobalBuf, \
                                                              (p_buf),             \
                                                              DEF_MIN((len), MQTTc_CFG_DBG_GLOBAL_BUF_LEN))
#else
#define  MQTTc_DBG_GLOBAL_BUF_COPY(p_buf, len)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  mqttc_data {
  KAL_TASK_HANDLE TaskHandle;
  KAL_SEM_HANDLE  TaskPendSem;

  //                                                               Max nbr of msgs that will need to be processed ...
  CPU_INT16U      MaxMsgNbr;                                    // at any given time.
  CPU_INT16U      InactivityTimeout_s;                          // Inactivity timeout of sock, in seconds.
  CPU_INT32U      TaskDly;                                      // Optional internal task dly.

  MQTTc_CONN      *ConnHeadPtr;                                 // Ptr to head of conn list.

  CPU_INT16U      MsgID_BitmapTblMax;                           // Max msg ID.
  CPU_INT32U      *MsgID_BitmapTbl;                             // Bitmap tbl for msg IDs.
  MQTTc_MSG       *MsgListHeadPtr;                              // Ptr to head of msg list to process.
  MQTTc_MSG       *MsgListTailPtr;                              // Ptr to tail of msg list to process.
} MQTTc_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const MQTTc_INIT_CFG  MQTTc_InitCfgDflt = MQTTc_INIT_CFG_DFLT;
static MQTTc_INIT_CFG MQTTc_InitCfg = MQTTc_INIT_CFG_DFLT;
#else
extern const MQTTc_INIT_CFG MQTTc_InitCfg;
#endif

static MQTTc_DATA *MQTTc_DataPtr = DEF_NULL;
#if (MQTTc_CFG_DBG_GLOBAL_BUF_LEN > 0u)
CPU_CHAR MQTTc_Dbg_GlobalBuf[MQTTc_CFG_DBG_GLOBAL_BUF_LEN];
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   MSG AND TASK PROCESSING FUNCTIONS
 *******************************************************************************************************/

static void MQTTc_Task(void *p_arg);

static void MQTTc_WrSockProcess(MQTTc_MSG *p_msg);

static void MQTTc_RdSockProcess(MQTTc_CONN *p_conn);

static void MQTTc_MsgProcess(void);

static void MQTTc_MsgCallbackExec(MQTTc_MSG *p_msg);

/********************************************************************************************************
 *                                               MSG Q FUNCTIONS
 *******************************************************************************************************/

static MQTTc_MSG *MQTTc_MsgCheck(void);

static void MQTTc_MsgPost(MQTTc_CONN     *p_conn,
                          MQTTc_MSG      *p_msg,
                          MQTTc_MSG_TYPE type,
                          CPU_INT32U     xfer_len,
                          CPU_INT08U     qos_lvl,
                          CPU_INT16U     msg_id,
                          RTOS_ERR       *p_err);

static void MQTTc_MsgListClosedCallbackExec(MQTTc_MSG *p_head_msg);

/********************************************************************************************************
 *                                               BUF FUNCTIONS
 *******************************************************************************************************/

static CPU_INT08U *MQTTc_FixedHdrBufCfg(CPU_INT08U     *p_buf,
                                        MQTTc_MSG_TYPE msg_type,
                                        CPU_BOOLEAN    dup_flag,
                                        CPU_INT08U     qos_lvl,
                                        CPU_BOOLEAN    retain_flag,
                                        CPU_INT32U     rem_len,
                                        RTOS_ERR       *p_err);

/********************************************************************************************************
 *                                           MSG ID FUNCTIONS
 *******************************************************************************************************/

static CPU_INT16U MQTTc_MsgID_Get(void);

static void MQTTc_MsgID_Free(CPU_INT16U msg_id);

/********************************************************************************************************
 *                                               OTHER FUNCTIONS
 *******************************************************************************************************/

static void MQTTc_ConnNextMsgClr(MQTTc_CONN *p_conn);

static void MQTTc_ConnCloseProc(MQTTc_CONN *p_conn,
                                RTOS_ERR   *p_err);

static void MQTTc_ConnRemove(MQTTc_CONN *p_conn);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           MQTTc_ConfigureTaskStk()
 *
 * @brief    Configure the MQTT client task stack properties to use the parameters contained in
 *           the passed structure instead of the default parameters.
 *
 * @param    stk_size_elements   Size of the stack, in CPU_STK elements.
 *
 * @param    p_stk_base          Pointer to base of the stack.
 *
 * @note     (1) This function is optional. If it is called, it must be called before MQTTc_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void MQTTc_ConfigureTaskStk(CPU_STK_SIZE stk_size_elements,
                            void         *p_stk_base)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG((MQTTc_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  MQTTc_InitCfg.StkSizeElements = stk_size_elements;
  MQTTc_InitCfg.StkPtr = p_stk_base;
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                           MQTTc_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by the
 *           MQTT client module instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Net_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void MQTTc_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG((MQTTc_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  MQTTc_InitCfg.MemSegPtr = p_mem_seg;
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                           MQTTc_ConfigureQty()
 *
 * @brief    Overwrite the Advanced Task configuration object for MQTT client.
 *
 * @param    p_qty_cfg   Pointer to structure containing the quantity parameters.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void MQTTc_ConfigureQty(MQTTc_QTY_CFG *p_qty_cfg)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG((MQTTc_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );
  RTOS_ASSERT_DBG((p_qty_cfg->MaxMsgNbr > 0), RTOS_ERR_INVALID_CFG,; );

  Mem_Copy(&MQTTc_InitCfg.QtyCfg, p_qty_cfg, sizeof(MQTTc_QTY_CFG));
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                               MQTTc_Init()
 *
 * @brief    Initializes the MQTT client module.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *******************************************************************************************************/
void MQTTc_Init(RTOS_ERR *p_err)
{
  MQTTc_DATA  *p_temp_mqttc_data;
  CPU_BOOLEAN kal_feat_is_ok;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  if (MQTTc_DataPtr != DEF_NULL) {                              // Make sure MQTTc module is not already init.
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }
  CORE_EXIT_ATOMIC();

  kal_feat_is_ok = KAL_FeatureQuery(KAL_FEATURE_TASK_CREATE, KAL_OPT_CREATE_NONE);
  kal_feat_is_ok = KAL_FeatureQuery(KAL_FEATURE_SEM_CREATE, KAL_OPT_CREATE_NONE);
  kal_feat_is_ok = KAL_FeatureQuery(KAL_FEATURE_SEM_PEND, KAL_OPT_PEND_NONE);
  kal_feat_is_ok = KAL_FeatureQuery(KAL_FEATURE_SEM_POST, KAL_OPT_POST_NONE);
  kal_feat_is_ok = KAL_FeatureQuery(KAL_FEATURE_SEM_DEL, KAL_OPT_DEL_NONE);
  kal_feat_is_ok &= KAL_FeatureQuery(KAL_FEATURE_DLY, KAL_OPT_DLY_NONE);
  kal_feat_is_ok &= KAL_FeatureQuery(KAL_FEATURE_PEND_TIMEOUT, KAL_OPT_NONE);

  RTOS_ASSERT_DBG_ERR_SET((kal_feat_is_ok == DEF_OK), *p_err, RTOS_ERR_NOT_AVAIL,; );

  //                                                               Allocate data needed by MQTTc.
  p_temp_mqttc_data = (MQTTc_DATA *)Mem_SegAlloc("MQTTc - Data",
                                                 MQTTc_InitCfg.MemSegPtr,
                                                 sizeof(MQTTc_DATA),
                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_temp_mqttc_data->ConnHeadPtr = DEF_NULL;
  p_temp_mqttc_data->InactivityTimeout_s = MQTTc_InitCfg.InactivityDfltTimeout_s;
  p_temp_mqttc_data->MaxMsgNbr = MQTTc_InitCfg.QtyCfg.MaxMsgNbr;
  p_temp_mqttc_data->TaskDly = MQTTc_InitCfg.TaskDly_ms;

  //                                                               Allocate msg ID bitmap tbl and calculate max ix.
  p_temp_mqttc_data->MsgID_BitmapTblMax = (MQTTc_InitCfg.QtyCfg.MaxMsgNbr + (DEF_INT_32_NBR_BITS - 1u)) / DEF_INT_32_NBR_BITS;
  p_temp_mqttc_data->MsgID_BitmapTbl = (CPU_INT32U *)Mem_SegAlloc("MQTTc - Msg ID Bitmap Tbl",
                                                                  MQTTc_InitCfg.MemSegPtr,
                                                                  sizeof(CPU_INT32U) * p_temp_mqttc_data->MsgID_BitmapTblMax,
                                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_temp_mqttc_data->MsgListHeadPtr = DEF_NULL;                 // Init head of msg list.
  p_temp_mqttc_data->MsgListTailPtr = DEF_NULL;                 // Init tail of msg list.

  p_temp_mqttc_data->TaskPendSem = KAL_SemCreate("MQTT task pend sem", DEF_NULL, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Create task.
  p_temp_mqttc_data->TaskHandle = KAL_TaskAlloc("MQTTc Task",
                                                MQTTc_InitCfg.StkPtr,
                                                MQTTc_InitCfg.StkSizeElements,
                                                DEF_NULL,
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  KAL_TaskCreate(p_temp_mqttc_data->TaskHandle,
                 MQTTc_Task,
                 DEF_NULL,
                 MQTT_CLIENT_TASK_PRIO_DFLT,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  MQTTc_DataPtr = p_temp_mqttc_data;
  CORE_EXIT_ATOMIC();

  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_TaskPrioSet()
 *
 * @brief    Sets priority of the MQTT client task.
 *
 * @param    prio    New priority for the MQTT client task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void MQTTc_TaskPrioSet(RTOS_TASK_PRIO prio,
                       RTOS_ERR       *p_err)
{
  KAL_TASK_HANDLE task_handle;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((MQTTc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT,; );

  task_handle = MQTTc_DataPtr->TaskHandle;
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(task_handle,
                  prio,
                  p_err);
}

/****************************************************************************************************//**
 *                                           MQTTc_TaskDlySet()
 *
 * @brief    Sets task delay
 *
 * @param    dly_ms  New delay in millisecond for the MQTT client task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void MQTTc_TaskDlySet(CPU_INT08U dly_ms,
                      RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((MQTTc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT,; );

  MQTTc_DataPtr->TaskDly = dly_ms;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       MQTTc_InactivityTimeoutDfltSet()
 *
 * @brief    Sets default inactivity timeout in second of the MQTT client connection.
 *
 * @param    inactivity_timeout_s    New priority for the MQTT client task.
 *
 * @param    p_err                   Pointer to the variable that will receive one of the following error code(s)
 *                                   from this function:
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_INVALID_ARG
 *
 * @note     (1) It is possible to set each connection with a different inactivity timeout by calling
 *               MQTTc_ConnSetParam() before opening the connection.
 *******************************************************************************************************/
void MQTTc_InactivityTimeoutDfltSet(CPU_INT16U inactivity_timeout_s,
                                    RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((MQTTc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  CORE_ENTER_ATOMIC();
  MQTTc_DataPtr->InactivityTimeout_s = inactivity_timeout_s;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               MQTTc_ConnClr()
 *
 * @brief    Clears an MQTT client Connection before its first usage.
 *
 * @param    p_conn  Pointer to the MQTTc Connection.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function :
 *                       - RTOS_ERR_NONE
 *
 * @note     (1) This function MUST be called before the MQTTc_CONN object is used for the first time.
 *******************************************************************************************************/
void MQTTc_ConnClr(MQTTc_CONN *p_conn,
                   RTOS_ERR   *p_err)
{
  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_conn->SockId = NET_SOCK_ID_NONE;
  p_conn->SockSelFlags = DEF_BIT_NONE;

  p_conn->BrokerNamePtr = DEF_NULL;
  p_conn->BrokerPortNbr = MQTTc_BROKER_PORT_NBR_DFLT_VAL;
  p_conn->InactivityTimeout_s = MQTTc_DataPtr->InactivityTimeout_s;

  p_conn->ClientID_Str = DEF_NULL;
  p_conn->UsernameStr = DEF_NULL;
  p_conn->PasswordStr = DEF_NULL;

  p_conn->KeepAliveTimerSec = MQTTc_KEEP_ALIVE_TIMER_SEC_DFLT_VAL;
  p_conn->WillCfgPtr = DEF_NULL;

  p_conn->SecureCfgPtr = DEF_NULL;

  p_conn->OnCmpl = DEF_NULL;
  p_conn->OnConnectCmpl = DEF_NULL;
  p_conn->OnPublishCmpl = DEF_NULL;
  p_conn->OnSubscribeCmpl = DEF_NULL;
  p_conn->OnUnsubscribeCmpl = DEF_NULL;
  p_conn->OnPingReqCmpl = DEF_NULL;
  p_conn->OnDisconnectCmpl = DEF_NULL;
  p_conn->OnErrCallback = DEF_NULL;
  p_conn->OnPublishRx = DEF_NULL;
  p_conn->ArgPtr = DEF_NULL;

  p_conn->TimeoutMs = MQTTc_TIMEOUT_MS_DFLT_VAL;

  p_conn->PublishRxMsgPtr = DEF_NULL;
  p_conn->PublishRemLen = 0u;

  p_conn->TxMsgHeadPtr = DEF_NULL;
  p_conn->NextTxMsgTxLen = 0u;

  p_conn->NextPtr = DEF_NULL;

  MQTTc_ConnNextMsgClr(p_conn);                                 // Clr all the NextMsg fields.

  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_ConnSetParam()
 *
 * @brief    Sets parameters related to the TCP and MQTT Client Connection.
 *
 * @param    p_conn      Pointer to the current MQTTc Connection.
 *
 * @param    type        Parameter type :
 *                           - MQTTc_PARAM_TYPE_BROKER_IP_ADDR                 Broker's IP addr.
 *                           - MQTTc_PARAM_TYPE_BROKER_NAME                    Broker's name.
 *                           - MQTTc_PARAM_TYPE_BROKER_PORT_NBR                Broker's port nbr.
 *                           - MQTTc_PARAM_TYPE_INACTIVITY_TIMEOUT_S           Inactivity timeout, in seconds.
 *                           - MQTTc_PARAM_TYPE_CLIENT_ID_STR                  Client ID str.
 *                           - MQTTc_PARAM_TYPE_USERNAME_STR                   Client username str.
 *                           - MQTTc_PARAM_TYPE_PASSWORD_STR                   Client password str.
 *                           - MQTTc_PARAM_TYPE_KEEP_ALIVE_TMR_SEC             Keep alive tmr, in seconds.
 *                           - MQTTc_PARAM_TYPE_WILL_CFG_PTR                   Will cfg ptr, if any.
 *                           - MQTTc_PARAM_TYPE_CALLBACK_ON_COMPL              Generic on     cmpl callback.
 *                           - MQTTc_PARAM_TYPE_CALLBACK_ON_CONNECT_CMPL       On connect     cmpl callback.
 *                           - MQTTc_PARAM_TYPE_CALLBACK_ON_PUBLISH_CMPL       On publish     cmpl callback.
 *                           - MQTTc_PARAM_TYPE_CALLBACK_ON_SUBSCRIBE_CMPL     On subscribe   cmpl callback.
 *                           - MQTTc_PARAM_TYPE_CALLBACK_ON_UNSUBSCRIBE_CMPL   On unsubscribe cmpl callback.
 *                           - MQTTc_PARAM_TYPE_CALLBACK_ON_PINGREQ_CMPL       On pingreq     cmpl callback.
 *                           - MQTTc_PARAM_TYPE_CALLBACK_ON_DISCONNECT_CMPL    On disconnect  cmpl callback.
 *                           - MQTTc_PARAM_TYPE_CALLBACK_ON_PUBLISH_RX         On publish rx'd callback.
 *                           - MQTTc_PARAM_TYPE_CALLBACK_ARG_PTR               Ptr on arg passed to callback.
 *                           - MQTTc_PARAM_TYPE_TIMEOUT_MS                     'Open' timeout, in milliseconds.
 *                           - MQTTc_PARAM_TYPE_PUBLISH_RX_MSG_PTR             Ptr on msg that is used to rx
 *                                                                             publish.
 *
 * @param    p_param     Parameter's value.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s) from
 *                       this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
void MQTTc_ConnSetParam(MQTTc_CONN       *p_conn,
                        MQTTc_PARAM_TYPE type,
                        void             *p_param,
                        RTOS_ERR         *p_err)
{
  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  switch (type) {
    case MQTTc_PARAM_TYPE_BROKER_IP_ADDR:
    case MQTTc_PARAM_TYPE_BROKER_NAME:
      p_conn->BrokerNamePtr = (CPU_CHAR *)p_param;
      break;

    case MQTTc_PARAM_TYPE_BROKER_PORT_NBR:
      p_conn->BrokerPortNbr = (CPU_INT16U)(CPU_INT32U)p_param;
      break;

    case MQTTc_PARAM_TYPE_INACTIVITY_TIMEOUT_S:
      p_conn->InactivityTimeout_s = (CPU_INT16U)(CPU_INT32U)p_param;
      break;

    case MQTTc_PARAM_TYPE_CLIENT_ID_STR:
      p_conn->ClientID_Str = (CPU_CHAR *)p_param;
      break;

    case MQTTc_PARAM_TYPE_USERNAME_STR:
      p_conn->UsernameStr = (CPU_CHAR *)p_param;
      break;

    case MQTTc_PARAM_TYPE_PASSWORD_STR:
      p_conn->PasswordStr = (CPU_CHAR *)p_param;
      break;

    case MQTTc_PARAM_TYPE_KEEP_ALIVE_TMR_SEC:
      p_conn->KeepAliveTimerSec = (CPU_INT16U)(CPU_INT32U)p_param;
      break;

    case MQTTc_PARAM_TYPE_WILL_CFG_PTR:
      p_conn->WillCfgPtr = (MQTTc_WILL_CFG *)p_param;
      break;

    case MQTTc_PARAM_TYPE_SECURE_CFG_PTR:
      p_conn->SecureCfgPtr = (NET_APP_SOCK_SECURE_CFG *)p_param;
      break;

    case MQTTc_PARAM_TYPE_CALLBACK_ON_COMPL:
      p_conn->OnCmpl = (MQTTc_CMPL_CALLBACK)p_param;
      break;

    case MQTTc_PARAM_TYPE_CALLBACK_ON_CONNECT_CMPL:
      p_conn->OnConnectCmpl = (MQTTc_CMPL_CALLBACK)p_param;
      break;

    case MQTTc_PARAM_TYPE_CALLBACK_ON_PUBLISH_CMPL:
      p_conn->OnPublishCmpl = (MQTTc_CMPL_CALLBACK)p_param;
      break;

    case MQTTc_PARAM_TYPE_CALLBACK_ON_SUBSCRIBE_CMPL:
      p_conn->OnSubscribeCmpl = (MQTTc_CMPL_CALLBACK)p_param;
      break;

    case MQTTc_PARAM_TYPE_CALLBACK_ON_UNSUBSCRIBE_CMPL:
      p_conn->OnUnsubscribeCmpl = (MQTTc_CMPL_CALLBACK)p_param;
      break;

    case MQTTc_PARAM_TYPE_CALLBACK_ON_PINGREQ_CMPL:
      p_conn->OnPingReqCmpl = (MQTTc_CMPL_CALLBACK)p_param;
      break;

    case MQTTc_PARAM_TYPE_CALLBACK_ON_DISCONNECT_CMPL:
      p_conn->OnDisconnectCmpl = (MQTTc_CMPL_CALLBACK)p_param;
      break;

    case MQTTc_PARAM_TYPE_CALLBACK_ON_ERR_CALLBACK:
      p_conn->OnErrCallback = (MQTTc_ERR_CALLBACK)p_param;
      break;

    case MQTTc_PARAM_TYPE_CALLBACK_ON_PUBLISH_RX:
      p_conn->OnPublishRx = (MQTTc_PUBLISH_RX_CALLBACK)p_param;
      break;

    case MQTTc_PARAM_TYPE_CALLBACK_ARG_PTR:
      p_conn->ArgPtr = p_param;
      break;

    case MQTTc_PARAM_TYPE_TIMEOUT_MS:
      p_conn->TimeoutMs = (CPU_INT32U)p_param;
      break;

    case MQTTc_PARAM_TYPE_PUBLISH_RX_MSG_PTR:                   // Init msg to be use as RX publish msg.
      p_conn->PublishRxMsgPtr = (MQTTc_MSG *)p_param;
      p_conn->PublishRxMsgPtr->ConnPtr = p_conn;
      p_conn->PublishRxMsgPtr->Type = MQTTc_MSG_TYPE_PUBLISH;
      p_conn->PublishRxMsgPtr->State = MQTTc_MSG_STATE_WAIT_RX;
      RTOS_ERR_SET(p_conn->PublishRxMsgPtr->Err, RTOS_ERR_NONE);
      p_conn->PublishRxMsgPtr->NextPtr = DEF_NULL;
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }
}

/****************************************************************************************************//**
 *                                               MQTTc_ConnOpen()
 *
 * @brief    Opens a new MQTT Client connection.
 *
 * @param    p_conn  Pointer to the MQTT client Connection object to open.
 *
 * @param    flags   Configuration flags, reserved for future usage.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_TYPE
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_NOT_SUPPORTED
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_FAIL
 *                       - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_NET_IF_LINK_DOWN
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_INVALID_STATE
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *                       - RTOS_ERR_NET_OP_IN_PROGRESS
 *                       - RTOS_ERR_TX
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_NET_INVALID_CONN
 *                       - RTOS_ERR_RX
 *                       - RTOS_ERR_NOT_READY
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_NET_ADDR_UNRESOLVED
 *                       - RTOS_ERR_NET_NEXT_HOP
 *                       - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *******************************************************************************************************/
void MQTTc_ConnOpen(MQTTc_CONN  *p_conn,
                    MQTTc_FLAGS flags,
                    RTOS_ERR    *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.

  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((MQTTc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  PP_UNUSED_PARAM(flags);

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  MQTTc_SockConnOpen(p_conn,
                     p_err);

  p_conn->TxMsgHeadPtr = DEF_NULL;
  p_conn->NextPtr = DEF_NULL;

  return;
}

/****************************************************************************************************//**
 *                                               MQTTc_ConnClose()
 *
 * @brief    Requests to close a MQTT client Connection.
 *
 * @param    p_conn  Pointer to the MQTT client Connection to close.
 *
 * @param    flags   Configuration flags, reserved for future usage.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @internal
 * @note         (1) [INTERNAL] A local message can be used only because the buffer is not referenced,
 *               in the case of a close request message. If the close request needs a valid buffer,
 *               the message must be allocated and configured by the caller of this function.
 * @endinternal
 *******************************************************************************************************/
void MQTTc_ConnClose(MQTTc_CONN  *p_conn,
                     MQTTc_FLAGS flags,
                     RTOS_ERR    *p_err)
{
  KAL_SEM_HANDLE sem_handle;
  MQTTc_MSG      local_mqtt_msg;                                // See Note #1.
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((MQTTc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  PP_UNUSED_PARAM(flags);

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (p_conn->SockId == NET_SOCK_ID_NONE) {
    return;
  }

  sem_handle = KAL_SemCreate("MQTTc Close Sem",
                             DEF_NULL,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  local_mqtt_msg.ArgPtr = (void *)&sem_handle;                  // Pass sem instead of buf, since it's a close req.

  MQTTc_MsgPost(p_conn,
                &local_mqtt_msg,
                MQTTc_MSG_TYPE_REQ_CLOSE,                       // Indicate this msg is used to req a close.
                0u,
                0u,
                MQTT_MSG_ID_NONE,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_err;                                               // Do not pend if there was an err in msg posting.
  }

  KAL_SemPend(sem_handle,
              KAL_OPT_PEND_NONE,
              KAL_TIMEOUT_INFINITE,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    *p_err = local_mqtt_msg.Err;
  }

end_err:
  KAL_SemDel(sem_handle);
  return;
}

/****************************************************************************************************//**
 *                                               MQTTc_MsgClr()
 *
 * @brief    Clears the Message object members.
 *
 * @param    p_msg   Pointer to the message object to clear.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function :
 *                       - RTOS_ERR_NONE
 *******************************************************************************************************/
void MQTTc_MsgClr(MQTTc_MSG *p_msg,
                  RTOS_ERR  *p_err)
{
  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.

  RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_msg->ConnPtr = DEF_NULL;

  p_msg->Type = MQTTc_MSG_TYPE_NONE;
  p_msg->State = MQTTc_MSG_STATE_NONE;

  p_msg->QoS = 0u;

  p_msg->MsgID = MQTT_MSG_ID_NONE;

  p_msg->ArgPtr = DEF_NULL;
  p_msg->BufLen = 0u;
  p_msg->XferLen = 0u;

  p_msg->NextPtr = DEF_NULL;

  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_MsgSetParam()
 *
 * @brief    Sets the parameter related to a given MQTT Message.
 *
 * @param    p_msg       Pointer to a message object.
 *
 * @param    type        Parameter type :
 *                           - MQTTc_PARAM_TYPE_MSG_BUF_PTR      Msg's buf ptr.
 *                           - MQTTc_PARAM_TYPE_MSG_BUF_LEN      Msg's buf len.
 *
 * @param    p_param     Parameter's value.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
void MQTTc_MsgSetParam(MQTTc_MSG        *p_msg,
                       MQTTc_PARAM_TYPE type,
                       void             *p_param,
                       RTOS_ERR         *p_err)
{
  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_param != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  switch (type) {
    case MQTTc_PARAM_TYPE_MSG_BUF_PTR:
      p_msg->ArgPtr = (void *)p_param;
      break;

    case MQTTc_PARAM_TYPE_MSG_BUF_LEN:
      p_msg->BufLen = (CPU_INT32U)p_param;
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_TYPE,; );
  }

  return;
}

/****************************************************************************************************//**
 *                                               MQTTc_Connect()
 *
 * @brief    Sends a 'Connect' message to the MQTT server.
 *
 * @param    p_conn  Pointer to the MQTT client Connection to use.
 *
 * @param    p_msg   Pointer to the MQTT client Message object to use.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NULL_PTR
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void MQTTc_Connect(MQTTc_CONN *p_conn,
                   MQTTc_MSG  *p_msg,
                   RTOS_ERR   *p_err)
{
  MQTTc_WILL_CFG *p_will_cfg;
  CPU_INT08U     *p_buf_start;
  CPU_INT08U     *p_buf;
  CPU_INT32U     xfer_len;
  CPU_INT32U     rem_len;
  CPU_INT16U     str_len;
  CPU_INT08U     conn_flags = 0u;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((MQTTc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn->ClientID_Str != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn->PublishRxMsgPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (p_conn->SockId == NET_SOCK_ID_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SOCK_CLOSED);
    return;
  }

  if ((p_msg->State != MQTTc_MSG_STATE_NONE)
      && (p_msg->State != MQTTc_MSG_STATE_CMPL)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  {
    CPU_INT08U client_id_len;

    client_id_len = Str_Len(p_conn->ClientID_Str);              // Make sure client ID is within spec limit.
    if (client_id_len > MQTT_MSG_VAR_HDR_CONNECT_CLIENT_ID_MAX_STR_LEN) {
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
    }

    if (p_conn->WillCfgPtr != DEF_NULL) {                       // Confirm will cfg contains what is needed.
      if ((p_conn->WillCfgPtr->WillMessage == DEF_NULL)
          || (p_conn->WillCfgPtr->WillTopic == DEF_NULL)) {
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NULL_PTR,; );
      }
    }

    RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

    RTOS_ASSERT_DBG_ERR_SET((p_msg->ArgPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

    RTOS_ASSERT_DBG_ERR_SET((p_msg->BufLen >= MQTT_MSG_BASE_LEN), *p_err, RTOS_ERR_INVALID_ARG,; );
  }
#endif

  if (p_conn->PublishRxMsgPtr == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
    return;
  }

  p_will_cfg = p_conn->WillCfgPtr;

  p_buf_start = (CPU_INT08U *)p_msg->ArgPtr;

  rem_len = MQTT_MSG_VAR_HDR_CONNECT_LEN;                       // Calculate len of msg.
  rem_len += Str_Len(p_conn->ClientID_Str) + MQTT_MSG_UTF8_LEN_SIZE;
  if (p_will_cfg != DEF_NULL) {
    rem_len += Str_Len(p_will_cfg->WillTopic)   + MQTT_MSG_UTF8_LEN_SIZE;
    rem_len += Str_Len(p_will_cfg->WillMessage) + MQTT_MSG_UTF8_LEN_SIZE;
  }
  if (p_conn->UsernameStr != DEF_NULL) {
    rem_len += Str_Len(p_conn->UsernameStr) + MQTT_MSG_UTF8_LEN_SIZE;
  }
  if (p_conn->PasswordStr != DEF_NULL) {
    rem_len += Str_Len(p_conn->PasswordStr) + MQTT_MSG_UTF8_LEN_SIZE;
  }

  p_buf = MQTTc_FixedHdrBufCfg(p_buf_start,                     // Cfg fixed hdr section of msg.
                               MQTTc_MSG_TYPE_CONNECT,
                               DEF_NO,
                               0u,
                               DEF_NO,
                               rem_len,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((rem_len + (p_buf - p_buf_start)) > p_msg->BufLen) {      // Confirm that buf can hold msg len.
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    return;
  }

  *p_buf = 0x00;
  p_buf++;
  *p_buf = MQTT_MSG_VAR_HDR_PROTOCOL_NAME_LEN;
  p_buf++;

  Str_Copy_N((CPU_CHAR *)p_buf, MQTT_MSG_VAR_HDR_PROTOCOL_NAME_STR, MQTT_MSG_VAR_HDR_PROTOCOL_NAME_LEN);
  p_buf += MQTT_MSG_VAR_HDR_PROTOCOL_NAME_LEN;

  *p_buf = MQTT_MSG_VAR_HDR_PROTOCOL_VERSION;
  p_buf++;
  //                                                               Set CONNECT msg flags.
  if (p_conn->UsernameStr != DEF_NULL) {
    DEF_BIT_SET(conn_flags, MQTT_MSG_VAR_HDR_CONNECT_FLAG_USER_NAME_FLAG);
  }

  if (p_conn->PasswordStr != DEF_NULL) {
    DEF_BIT_SET(conn_flags, MQTT_MSG_VAR_HDR_CONNECT_FLAG_PSWD_FLAG);
  }

  if (p_will_cfg != DEF_NULL) {
    DEF_BIT_SET(conn_flags, MQTT_MSG_VAR_HDR_CONNECT_FLAG_WILL_FLAG);
    DEF_BIT_SET(conn_flags, p_will_cfg->WillQoS << MQTT_MSG_VAR_HDR_CONNECT_FLAG_WILL_QOS_BIT_SHIFT);
    if (p_will_cfg->WillRetain == DEF_YES) {
      DEF_BIT_SET(conn_flags, MQTT_MSG_VAR_HDR_CONNECT_FLAG_WILL_RETAIN);
    }
  }

  DEF_BIT_SET(conn_flags, MQTT_MSG_VAR_HDR_CONNECT_FLAG_CLEAN_SESSION);

  *p_buf = conn_flags;
  p_buf++;

  *p_buf = (CPU_INT08U)(p_conn->KeepAliveTimerSec >> 8u);
  p_buf++;
  *p_buf = (CPU_INT08U)(p_conn->KeepAliveTimerSec & 0xFFu);
  p_buf++;

  str_len = Str_Len(p_conn->ClientID_Str);                      // Copy client ID str.
  *p_buf = (CPU_INT08U)(str_len >> 8u);
  p_buf++;
  *p_buf = (CPU_INT08U)(str_len & 0xFFu);
  p_buf++;
  Str_Copy((CPU_CHAR *)p_buf, p_conn->ClientID_Str);
  p_buf += str_len;

  if (p_will_cfg != DEF_NULL) {                                 // Copy will infos, if any.
    str_len = Str_Len(p_will_cfg->WillTopic);
    *p_buf = (CPU_INT08U)(str_len >> 8u);
    p_buf++;
    *p_buf = (CPU_INT08U)(str_len & 0xFFu);
    p_buf++;
    Str_Copy((CPU_CHAR *)p_buf, p_will_cfg->WillTopic);
    p_buf += str_len;

    str_len = Str_Len(p_will_cfg->WillMessage);
    *p_buf = (CPU_INT08U)(str_len >> 8u);
    p_buf++;
    *p_buf = (CPU_INT08U)(str_len & 0xFFu);
    p_buf++;
    Str_Copy((CPU_CHAR *)p_buf, p_will_cfg->WillMessage);
    p_buf += str_len;
  }

  if (p_conn->UsernameStr != DEF_NULL) {                        // Copy username str, if any.
    str_len = Str_Len(p_conn->UsernameStr);
    *p_buf = (CPU_INT08U)(str_len >> 8u);
    p_buf++;
    *p_buf = (CPU_INT08U)(str_len & 0xFFu);
    p_buf++;
    Str_Copy((CPU_CHAR *)p_buf, p_conn->UsernameStr);
    p_buf += str_len;
  }

  if (p_conn->PasswordStr != DEF_NULL) {                        // Copy password str, if any.
    str_len = Str_Len(p_conn->PasswordStr);
    *p_buf = (CPU_INT08U)(str_len >> 8u);
    p_buf++;
    *p_buf = (CPU_INT08U)(str_len & 0xFFu);
    p_buf++;
    Str_Copy((CPU_CHAR *)p_buf, p_conn->PasswordStr);
    p_buf += str_len;
  }

  MQTTc_DBG_GLOBAL_BUF_COPY(p_buf_start, 150u);

  xfer_len = p_buf - p_buf_start;

  MQTTc_MsgPost(p_conn,                                         // Add msg to Q for task to process.
                p_msg,
                MQTTc_MSG_TYPE_CONNECT,
                xfer_len,
                0u,
                MQTT_MSG_ID_NONE,
                p_err);

  return;
}

/****************************************************************************************************//**
 *                                               MQTTc_Publish()
 *
 * @brief    Sends a 'Publish' message to the MQTT server.
 *
 * @param    p_conn          Pointer to the MQTT client Connection object to use.
 *
 * @param    p_msg           Pointer to the MQTT client Message object to use.
 *
 * @param    topic_str       String containing the topic on which to publish.  Must stay valid until
 *                           the message has been completely sent.
 *
 * @param    qos_lvl         Level of QoS at which to publish.
 *
 * @param    retain_flag     Flag that indicates if the retain flag in the PUBLISH header
 *                           needs to be set.
 *
 * @param    p_payload       Pointer to the payload to publish. Must stay valid until the
 *                           message has been completely sent.
 *
 * @param    payload_len     The length of the payload to publish.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void MQTTc_Publish(MQTTc_CONN       *p_conn,
                   MQTTc_MSG        *p_msg,
                   const CPU_CHAR   *topic_str,
                   CPU_INT08U       qos_lvl,
                   CPU_BOOLEAN      retain_flag,
                   const CPU_INT08U *p_payload,
                   CPU_INT32U       payload_len,
                   RTOS_ERR         *p_err)
{
  CPU_INT08U *p_buf_start;
  CPU_INT08U *p_buf;
  CPU_INT32U xfer_len;
  CPU_INT32U len;
  CPU_INT16U rem_len;
  CPU_INT16U topic_len;
  CPU_INT16U msg_id = MQTT_MSG_ID_NONE;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.

  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((MQTTc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg->ArgPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg->BufLen >= MQTT_MSG_PING_DISCONN_LEN), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET((qos_lvl <= MQTT_MSG_QOS_LVL_MAX), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET((topic_str != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_payload != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((qos_lvl <= MQTT_MSG_QOS_LVL_MAX), *p_err, RTOS_ERR_INVALID_ARG,; );

  if (p_conn->SockId == NET_SOCK_ID_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SOCK_CLOSED);
    return;
  }

  if ((p_msg->State != MQTTc_MSG_STATE_NONE)
      && (p_msg->State != MQTTc_MSG_STATE_CMPL)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

  if (p_conn->SockId == NET_SOCK_ID_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SOCK_CLOSED);
    return;
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  topic_len = Str_Len(topic_str);
  p_buf = (CPU_INT08U *)Str_Char_N(topic_str,                   // # sign not allowed in topic.
                                   topic_len,
                                   ASCII_CHAR_NUMBER_SIGN);
  RTOS_ASSERT_DBG_ERR_SET((p_buf == DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_buf = (CPU_INT08U *)Str_Char_N(topic_str,                   // + sign not allowed in topic.
                                   topic_len,
                                   ASCII_CHAR_PLUS_SIGN);
  RTOS_ASSERT_DBG_ERR_SET((p_buf == DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_buf_start = (CPU_INT08U *)p_msg->ArgPtr;

  rem_len = Str_Len(topic_str) + MQTT_MSG_UTF8_LEN_SIZE;
  if (qos_lvl > 0u) {
    rem_len += MQTT_MSG_ID_SIZE;
  }
  rem_len += payload_len;

  p_buf = MQTTc_FixedHdrBufCfg(p_buf_start,                     // Cfg fixed section of hdr.
                               MQTTc_MSG_TYPE_PUBLISH,
                               DEF_NO,
                               qos_lvl,
                               retain_flag,
                               rem_len,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  len = (rem_len + ((CPU_INT16U)(p_buf - p_buf_start)));
  if (len > p_msg->BufLen) {
    //                                                             Confirm msg fits in provided buf.
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    return;
  }

  topic_len = Str_Len(topic_str);                               // Copy topic str.
  *p_buf = (CPU_INT08U)(topic_len >> 8u);
  p_buf++;
  *p_buf = (CPU_INT08U)(topic_len & 0xFFu);
  p_buf++;
  Str_Copy((CPU_CHAR *)p_buf, topic_str);

  p_buf += topic_len;

  if (qos_lvl > 0u) {                                           // Obtain msg ID if QoS > 0.
    msg_id = MQTTc_MsgID_Get();

    *p_buf = (CPU_INT08U)(msg_id >> 8u);
    p_buf++;
    *p_buf = (CPU_INT08U)(msg_id & 0xFFu);
    p_buf++;
  }

  //                                                               Copy payload.
  Mem_Copy(p_buf, p_payload, payload_len);

  p_buf += payload_len;

  xfer_len = p_buf - p_buf_start;

  MQTTc_DBG_GLOBAL_BUF_COPY(p_buf, 150u);

  MQTTc_MsgPost(p_conn,                                         // Post msg to Q for task to process.
                p_msg,
                MQTTc_MSG_TYPE_PUBLISH,
                xfer_len,
                qos_lvl,
                msg_id,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    MQTTc_MsgID_Free(msg_id);
  }

  return;
}

/****************************************************************************************************//**
 *                                               MQTTc_Subscribe()
 *
 * @brief    Sends a 'Subscribe' message to the MQTT server.
 *
 * @param    p_conn      Pointer to the MQTT client Connection object to use.
 *
 * @param    p_msg       Pointer to the MQTT client Message object to use.
 *
 * @param    topic_str   String containing the topic at which to subscribe. Must stay valid until
 *                       the message has been completely sent.
 *
 * @param    req_qos     Requested level of QoS for this subscription.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void MQTTc_Subscribe(MQTTc_CONN     *p_conn,
                     MQTTc_MSG      *p_msg,
                     const CPU_CHAR *topic_str,
                     CPU_INT08U     req_qos,
                     RTOS_ERR       *p_err)
{
  MQTTc_SubscribeMult(p_conn,
                      p_msg,
                      &topic_str,
                      &req_qos,
                      1u,
                      p_err);
}

/****************************************************************************************************//**
 *                                           MQTTc_SubscribeMult()
 *
 * @brief    Sends a 'Subscribe' message containing multiple topics to MQTT server.
 *
 * @param    p_conn          Pointer to the MQTT client Connection object to use.
 *
 * @param    p_msg           Pointer to the MQTT client Message object to use.
 *
 * @param    topic_str_tbl   Table containing string of all the topic(s) at which to subscribe. Must
 *                           all stay valid until the message has been completely sent.
 *
 * @param    req_qos_tbl     Table of the requested level of QoS for each subscription.
 *
 * @param    topic_nbr       Number of topics and QoS' contained in tables.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_INVALID_HANDLE
 *
 * @internal
 * @note         (1) [INTENRAL] To compare with the granted QoS returned by the server with the
 *               SUBACK message, the number of topics and their QoSes are kept before the actual
 *               content to send to the server. For three topics, the buffer would look like this:
 *               @verbatim
 *               Original p_msg->BufPtr           Modified p_msg->BufPtr
 *                               |               |   Start of msg sent to server.
 *                               |               |  |
 *                               V               V  V
 *                               ------------------------------
 *                               | 2 | 0 | 1 | 3 | 0x82 | ... |
 *                               ------------------------------
 *                               ^    ^   ^   ^
 *                               |    |   |   |
 *                       QoS Topic 3    |   |   |
 *                           QoS Topic 2   |   |
 *                               QoS Topic 1   |
 *                                       Topic Nbr
 *               @endverbatim
 * @endinternal
 *******************************************************************************************************/
void MQTTc_SubscribeMult(MQTTc_CONN     *p_conn,
                         MQTTc_MSG      *p_msg,
                         const CPU_CHAR **topic_str_tbl,
                         CPU_INT08U     *req_qos_tbl,
                         CPU_INT08U     topic_nbr,
                         RTOS_ERR       *p_err)
{
  CPU_INT08U *p_buf_base;
  CPU_INT08U *p_buf_start;
  CPU_INT08U *p_buf;
  CPU_INT32U xfer_len;
  CPU_INT32U len;
  CPU_INT16U str_len;
  CPU_INT16U rem_len;
  CPU_INT16U msg_id;
  CPU_INT08U topic_ix;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.

  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((MQTTc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg->ArgPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  //                                                               Make sure buf can hold topics of 1 byte and its QoS.
  RTOS_ASSERT_DBG_ERR_SET((p_msg->BufLen >= (MQTT_MSG_BASE_LEN + 1u + (topic_nbr * (MQTT_MSG_UTF8_LEN_SIZE + 1u)))), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET((topic_str_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((req_qos_tbl != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

  if ((p_msg->State != MQTTc_MSG_STATE_NONE)
      && (p_msg->State != MQTTc_MSG_STATE_CMPL)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

  if (p_conn->SockId == NET_SOCK_ID_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SOCK_CLOSED);
    return;
  }

  p_buf_start = (CPU_INT08U *)p_msg->ArgPtr;
  p_buf_base = (CPU_INT08U *)p_msg->ArgPtr;
  rem_len = MQTT_MSG_ID_SIZE;

  for (topic_ix = 0u; topic_ix < topic_nbr; topic_ix++) {       // Calculate len of all topics and their QoS.
                                                                // Make sure each ix contains a valid str and QoS.
    RTOS_ASSERT_DBG_ERR_SET((topic_str_tbl[topic_ix] != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

    RTOS_ASSERT_DBG_ERR_SET((req_qos_tbl[topic_ix] <= MQTT_MSG_QOS_LVL_MAX), *p_err, RTOS_ERR_INVALID_ARG,; );

    str_len = Str_Len(topic_str_tbl[topic_ix]);
        #if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
    p_buf = (CPU_INT08U *)Str_Char_N(topic_str_tbl[topic_ix],
                                     str_len,
                                     ASCII_CHAR_NUMBER_SIGN);
    if (p_buf != DEF_NULL) {
      if (str_len != 1u) {                                      // If topic is '#' by itself, no err.
        if (*(p_buf - 1u) != ASCII_CHAR_SOLIDUS) {              // '#' must be preceded by a '/'.
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
        }
        //                                                         '#' must be the last character in topic.
        if (p_buf != (CPU_INT08U *)&topic_str_tbl[topic_ix][str_len - 1u]) {
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
        }
      }
    }

    p_buf = (CPU_INT08U *)Str_Char_N(topic_str_tbl[topic_ix],
                                     str_len,
                                     ASCII_CHAR_PLUS_SIGN);
    if (p_buf != DEF_NULL) {
      if (str_len != 1u) {                                      // If topic is '+' by itself, no err.
        if (*(p_buf - 1u) != ASCII_CHAR_SOLIDUS) {              // '+' must be preceded by a '/'.
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
        }
        //                                                         If '+' is not last char, it must be followed by '/'.
        if ((p_buf != (CPU_INT08U *)&topic_str_tbl[topic_ix][str_len - 1u])
            && (*(p_buf + 1u) != ASCII_CHAR_SOLIDUS)) {
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
        }
      }
    }
        #endif

    rem_len += str_len + MQTT_MSG_UTF8_LEN_SIZE;
    rem_len += 1u;
  }

  //                                                               Write topics QoS and nbr of topics before ...
  //                                                               content. See Note #1.
  for (topic_ix = topic_nbr; topic_ix > 0u; topic_ix--) {
    *p_buf_start = req_qos_tbl[topic_ix - 1u];
    p_buf_start++;
  }
  *p_buf_start = topic_nbr;
  p_buf_start++;

  MQTTc_DBG_GLOBAL_BUF_COPY(p_buf_base, 10u);

  p_buf = MQTTc_FixedHdrBufCfg(p_buf_start,                     // Cfg fixed hdr section of msg.
                               MQTTc_MSG_TYPE_SUBSCRIBE,
                               DEF_NO,
                               1u,
                               DEF_NO,
                               rem_len,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  len = (rem_len + ((CPU_INT16U)(p_buf - p_buf_base)));
  if (len > p_msg->BufLen) {
    //                                                             Confirm msg fits in provided buf.
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    return;
  }

  msg_id = MQTTc_MsgID_Get();
  *p_buf = (CPU_INT08U)(msg_id >> 8u);
  p_buf++;
  *p_buf = (CPU_INT08U)(msg_id & 0xFFu);
  p_buf++;

  for (topic_ix = 0u; topic_ix < topic_nbr; topic_ix++) {       // Calculate len of all topics and their QoS.
    str_len = Str_Len(topic_str_tbl[topic_ix]);

    *p_buf = (CPU_INT08U)(str_len >> 8u);
    p_buf++;
    *p_buf = (CPU_INT08U)(str_len & 0xFFu);
    p_buf++;

    Str_Copy((CPU_CHAR *)p_buf, topic_str_tbl[topic_ix]);

    p_buf += str_len;

    *p_buf = req_qos_tbl[topic_ix];
    p_buf++;
  }

  xfer_len = p_buf - p_buf_start;

  MQTTc_DBG_GLOBAL_BUF_COPY(p_buf_start, 150u);

  p_msg->ArgPtr = (void *)p_buf_start;                          // Adjust BufPtr to start of content to send.
  p_msg->BufLen -= (topic_nbr + 1u);                            // Adjust BufLen to account for topics Qos.

  MQTTc_MsgPost(p_conn,                                         // Post msg to Q for task to process.
                p_msg,
                MQTTc_MSG_TYPE_SUBSCRIBE,
                xfer_len,
                1u,
                msg_id,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    MQTTc_MsgID_Free(msg_id);
  }

  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_Unsubscribe()
 *
 * @brief    Sends an 'Unsubscribe' message to the MQTT server.
 *
 * @param    p_conn      Pointer to the MQTT client Connection object to use.
 *
 * @param    p_msg       Pointer to the MQTT client Message object to use.
 *
 * @param    topic_str   String containing the topic at which to unsubscribe. Must stay valid
 *                       until the message has been completely sent.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void MQTTc_Unsubscribe(MQTTc_CONN     *p_conn,
                       MQTTc_MSG      *p_msg,
                       const CPU_CHAR *topic_str,
                       RTOS_ERR       *p_err)
{
  MQTTc_UnsubscribeMult(p_conn,
                        p_msg,
                        &topic_str,
                        1u,
                        p_err);
}

/****************************************************************************************************//**
 *                                           MQTTc_UnsubscribeMult()
 *
 * @brief    Sends an 'Unsubscribe' message for multiple topics to the MQTT server.
 *
 * @param    p_conn          Pointer to the MQTT client Connection object to use.
 *
 * @param    p_msg           Pointer to the MQTT client Message object to use.
 *
 * @param    topic_str_tbl   Table containing string of all the topic(s) at which to unsubscribe. Must
 *                           stay valid until the message has been completely sent.
 *
 * @param    topic_nbr       Number of topic contained in tables.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void MQTTc_UnsubscribeMult(MQTTc_CONN     *p_conn,
                           MQTTc_MSG      *p_msg,
                           const CPU_CHAR **topic_str_tbl,
                           CPU_INT08U     topic_nbr,
                           RTOS_ERR       *p_err)
{
  CPU_INT08U *p_buf_start;
  CPU_INT08U *p_buf;
  CPU_INT32U xfer_len;
  CPU_INT32U len;
  CPU_INT16U str_len;
  CPU_INT16U rem_len;
  CPU_INT16U msg_id;
  CPU_INT08U topic_ix;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.

  CORE_ENTER_ATOMIC();
  //                                                               Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((MQTTc_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg->ArgPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg->BufLen >= MQTT_MSG_BASE_LEN), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET((topic_str_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (p_conn->SockId == NET_SOCK_ID_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SOCK_CLOSED);
    return;
  }

  p_buf_start = (CPU_INT08U *)p_msg->ArgPtr;
  rem_len = MQTT_MSG_ID_SIZE;

  for (topic_ix = 0u; topic_ix < topic_nbr; topic_ix++) {       // Calculate len of all topics.
                                                                // Make sure each ix contains a valid str.
    RTOS_ASSERT_DBG_ERR_SET((topic_str_tbl[topic_ix] != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

    str_len = Str_Len(topic_str_tbl[topic_ix]);
    rem_len += str_len + MQTT_MSG_UTF8_LEN_SIZE;
  }

  p_buf = MQTTc_FixedHdrBufCfg(p_buf_start,                     // Cfg fixed hdr section of msg.
                               MQTTc_MSG_TYPE_UNSUBSCRIBE,
                               DEF_NO,
                               1u,
                               DEF_NO,
                               rem_len,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  len = (rem_len + ((CPU_INT16U)(p_buf - p_buf_start)));
  if (len > p_msg->BufLen) {
    //                                                             Confirm msg fits in provided buf.
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    return;
  }

  msg_id = MQTTc_MsgID_Get();                                   // Obtain msg ID.
  *p_buf = (CPU_INT08U)(msg_id >> 8u);
  p_buf++;
  *p_buf = (CPU_INT08U)(msg_id & 0xFFu);
  p_buf++;

  for (topic_ix = 0u; topic_ix < topic_nbr; topic_ix++) {
    str_len = Str_Len(topic_str_tbl[topic_ix]);

    *p_buf = (CPU_INT08U)(str_len >> 8u);
    p_buf++;
    *p_buf = (CPU_INT08U)(str_len & 0xFFu);
    p_buf++;

    Str_Copy((CPU_CHAR *)p_buf, topic_str_tbl[topic_ix]);

    p_buf += str_len;
  }

  xfer_len = p_buf - p_buf_start;

  MQTTc_DBG_GLOBAL_BUF_COPY(p_buf_start, 150u);

  MQTTc_MsgPost(p_conn,                                         // Post msg in Q for task to process.
                p_msg,
                MQTTc_MSG_TYPE_UNSUBSCRIBE,
                xfer_len,
                1u,
                msg_id,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    MQTTc_MsgID_Free(msg_id);
  }

  return;
}

/****************************************************************************************************//**
 *                                               MQTTc_PingReq()
 *
 * @brief    Sends a 'PingReq' message to the MQTT server.
 *
 * @param    p_conn  Pointer to the MQTT client Connection object to use.
 *
 * @param    p_msg   Pointer to the MQTT client Message object to use.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void MQTTc_PingReq(MQTTc_CONN *p_conn,
                   MQTTc_MSG  *p_msg,
                   RTOS_ERR   *p_err)
{
  CPU_INT08U *p_buf_start;
  CPU_INT08U *p_buf;
  CPU_INT32U xfer_len;

  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((MQTTc_DataPtr != DEF_NULL), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
                                                                // Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg->ArgPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg->BufLen >= MQTT_MSG_PING_DISCONN_LEN), *p_err, RTOS_ERR_INVALID_ARG,; );

  if (p_conn->SockId == NET_SOCK_ID_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SOCK_CLOSED);
    return;
  }

  p_buf_start = (CPU_INT08U *)p_msg->ArgPtr;

  p_buf = MQTTc_FixedHdrBufCfg(p_buf_start,                     // Cfg fixed hdr section of msg.
                               MQTTc_MSG_TYPE_PINGREQ,
                               DEF_NO,
                               0u,
                               DEF_NO,
                               0u,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  xfer_len = p_buf - p_buf_start;

  MQTTc_DBG_GLOBAL_BUF_COPY(p_buf, 150u);

  MQTTc_MsgPost(p_conn,                                         // Post msg in Q for task to process.
                p_msg,
                MQTTc_MSG_TYPE_PINGREQ,
                xfer_len,
                0u,
                MQTT_MSG_ID_NONE,
                p_err);

  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_Disconnect()
 *
 * @brief    Sends a 'Disconnect' message to the MQTT server.
 *
 * @param    p_conn  Pointer to the MQTT client Connection object to use.
 *
 * @param    p_msg   Pointer to the MQTT client Message object to use.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *******************************************************************************************************/
void MQTTc_Disconnect(MQTTc_CONN *p_conn,
                      MQTTc_MSG  *p_msg,
                      RTOS_ERR   *p_err)
{
  CPU_INT08U *p_buf;
  CPU_INT08U *p_buf_start;
  CPU_INT32U xfer_len;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((MQTTc_DataPtr != DEF_NULL), RTOS_ERR_NOT_INIT,; );
  CORE_EXIT_ATOMIC();

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
                                                                // Make sure MQTTc module is init.
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg->ArgPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg->BufLen >= MQTT_MSG_PING_DISCONN_LEN), *p_err, RTOS_ERR_INVALID_ARG,; );

  if (p_conn->SockId == NET_SOCK_ID_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_SOCK_CLOSED);
    return;
  }

  p_buf_start = (CPU_INT08U *)p_msg->ArgPtr;

  p_buf = MQTTc_FixedHdrBufCfg(p_buf_start,                     // Cfg fixed hdr section of msg.
                               MQTTc_MSG_TYPE_DISCONNECT,
                               DEF_NO,
                               0u,
                               DEF_NO,
                               0u,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  xfer_len = p_buf - p_buf_start;

  MQTTc_DBG_GLOBAL_BUF_COPY(p_buf, 150u);

  MQTTc_MsgPost(p_conn,                                         // Post msg in Q for task to process.
                p_msg,
                MQTTc_MSG_TYPE_DISCONNECT,
                xfer_len,
                0u,
                MQTT_MSG_ID_NONE,
                p_err);

  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               MQTTc_Task()
 *
 * @brief    Task for MQTTc. Process select, read, write and msg Q.
 *
 * @param    p_arg   Unused argument.
 *******************************************************************************************************/
static void MQTTc_Task(void *p_arg)
{
  MQTTc_CONN  *p_conn;
  CPU_BOOLEAN proc_rd;
  CPU_BOOLEAN proc_wr;
  CPU_BOOLEAN proc_err;
  CPU_BOOLEAN is_init = DEF_NO;
  CPU_INT32U  dly;
  RTOS_ERR    local_err;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(p_arg);

  while (is_init != DEF_YES) {                                  // Wait for MQTTc module to be init.
    KAL_Dly(1u);
    CORE_ENTER_ATOMIC();
    is_init = (MQTTc_DataPtr != DEF_NULL) ? DEF_YES : DEF_NO;
    CORE_EXIT_ATOMIC();
  }

  while (DEF_TRUE) {
    CORE_ENTER_ATOMIC();
    dly = MQTTc_DataPtr->TaskDly;
    CORE_EXIT_ATOMIC();

    if (MQTTc_DataPtr->ConnHeadPtr != DEF_NULL) {
      MQTTc_SockSel(MQTTc_DataPtr->ConnHeadPtr,
                    &local_err);

      if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
        p_conn = MQTTc_DataPtr->ConnHeadPtr;

        while (p_conn != DEF_NULL) {
          MQTTc_CONN *p_conn_next = p_conn->NextPtr;

          proc_rd = MQTTc_SockSelDescProc(p_conn, MQTTc_SEL_DESC_TYPE_RD);
          proc_wr = MQTTc_SockSelDescProc(p_conn, MQTTc_SEL_DESC_TYPE_WR);
          proc_err = MQTTc_SockSelDescProc(p_conn, MQTTc_SEL_DESC_TYPE_ERR);

          if (proc_wr == DEF_YES) {
            MQTTc_MSG *p_msg = DEF_NULL;

            if (p_conn->PublishRxMsgPtr->State == MQTTc_MSG_STATE_WAIT_TX_CMPL) {
              p_msg = p_conn->PublishRxMsgPtr;
            } else if ((p_conn->TxMsgHeadPtr != DEF_NULL)
                       && (p_conn->TxMsgHeadPtr->State == MQTTc_MSG_STATE_WAIT_TX_CMPL)) {
              p_msg = p_conn->TxMsgHeadPtr;
            } else if (p_conn->PublishRxMsgPtr->State == MQTTc_MSG_STATE_MUST_TX) {
              p_msg = p_conn->PublishRxMsgPtr;
            } else if ((p_conn->TxMsgHeadPtr != DEF_NULL)
                       && (p_conn->TxMsgHeadPtr->State == MQTTc_MSG_STATE_MUST_TX)) {
              p_msg = p_conn->TxMsgHeadPtr;
            } else {
              MQTTc_SockSelDescClr(p_conn, MQTTc_SEL_DESC_TYPE_WR);
            }

            if (p_msg != DEF_NULL) {
              MQTTc_WrSockProcess(p_msg);
            }
          } else if ((proc_rd == DEF_YES)
                     || (proc_err == DEF_YES)) {
            MQTTc_RdSockProcess(p_conn);
          }

          if ((p_conn->TxMsgHeadPtr != DEF_NULL)
              && (p_conn->TxMsgHeadPtr->State == MQTTc_MSG_STATE_MUST_TX)) {
            MQTTc_SockSelDescSet(p_conn, MQTTc_SEL_DESC_TYPE_WR);
          }
          p_conn = p_conn_next;
        }
      }
    } else {
      KAL_SemPend(MQTTc_DataPtr->TaskPendSem, KAL_OPT_PEND_NONE, 0, &local_err);
    }

    MQTTc_MsgProcess();

    KAL_Dly(dly);
  }
}

/****************************************************************************************************//**
 *                                           MQTTc_WrSockProcess()
 *
 * @brief    Process write operations required for given MQTTc message.
 *
 * @param    p_msg   Pointer to MQTTc Message object for which to process write operation.
 *******************************************************************************************************/
static void MQTTc_WrSockProcess(MQTTc_MSG *p_msg)
{
  MQTTc_CONN *p_conn = p_msg->ConnPtr;
  CPU_INT32U buf_len;

  if (p_msg->State == MQTTc_MSG_STATE_MUST_TX) {                // If msg needs to be tx'd, tx it.
    switch (p_msg->Type) {
      case MQTTc_MSG_TYPE_CONNECT:
        //                                                         Set Rd and Err sel desc, to be able to rx CONNACK.
        MQTTc_SockSelDescSet(p_conn, MQTTc_SEL_DESC_TYPE_RD);
        MQTTc_SockSelDescSet(p_conn, MQTTc_SEL_DESC_TYPE_ERR);
      //                                                           Fallthrough

      case MQTTc_MSG_TYPE_PUBLISH:
      case MQTTc_MSG_TYPE_PUBACK:
      case MQTTc_MSG_TYPE_PUBREC:
      case MQTTc_MSG_TYPE_PUBREL:
      case MQTTc_MSG_TYPE_PUBCOMP:
      case MQTTc_MSG_TYPE_SUBSCRIBE:
      case MQTTc_MSG_TYPE_UNSUBSCRIBE:
      case MQTTc_MSG_TYPE_PINGREQ:
      case MQTTc_MSG_TYPE_DISCONNECT:
        buf_len = DEF_MIN((p_msg->XferLen - p_conn->NextTxMsgTxLen), DEF_INT_16U_MAX_VAL);
        LOG_DBG(("Transmitting ", (u)buf_len, " bytes on sock ID ", (d)p_conn->SockId, ". Msg Type: ", (u)(CPU_INT08U)p_msg->Type));

        p_conn->NextTxMsgTxLen += MQTTc_SockTx(p_conn,
                                               &(((CPU_INT08U *)p_msg->ArgPtr)[p_conn->NextTxMsgTxLen]),
                                               buf_len,
                                               &p_msg->Err);
        switch (RTOS_ERR_CODE_GET(p_msg->Err)) {
          case RTOS_ERR_NONE:
            break;

          default:                                              // fatal error.
            LOG_ERR(("!!! ERROR !!! Transmit error returned : ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(p_msg->Err))));
            goto err_remove_conn_close_sock;
        }

        if (RTOS_ERR_CODE_GET(p_msg->Err) != RTOS_ERR_NONE) {          // If err, exec callback and return.
          MQTTc_MsgCallbackExec(p_msg);
        }
        if (p_conn->NextTxMsgTxLen == p_msg->XferLen) {
          p_msg->State = MQTTc_MSG_STATE_WAIT_TX_CMPL;
          p_conn->NextTxMsgTxLen = 0u;
        }
        break;

      case MQTTc_MSG_TYPE_CONNACK:                              // These cases should never happen.
      case MQTTc_MSG_TYPE_SUBACK:
      case MQTTc_MSG_TYPE_UNSUBACK:
      case MQTTc_MSG_TYPE_PINGRESP:
      default:
        LOG_ERR(("!!! ERROR !!! In must Tx switch default case."));
        break;
    }
  } else if (p_msg->State == MQTTc_MSG_STATE_WAIT_TX_CMPL) {
    CPU_INT08U *p_buf_topic_nbr;
    CPU_INT08U topic_nbr;

    //                                                             Tx operation has finished. Go to next step of msg.
    switch (p_msg->Type) {
      case MQTTc_MSG_TYPE_CONNECT:                              // Finished sending a CONNECT, wait to rx CONNACK reply.
        LOG_DBG(("Finished sending Connect. Waiting to Rx Connack."));

        p_msg->Type = MQTTc_MSG_TYPE_CONNACK;
        p_msg->State = MQTTc_MSG_STATE_WAIT_RX;
        p_msg->XferLen = 2u;
        break;

      case MQTTc_MSG_TYPE_PUBLISH:
        if (p_msg->QoS == 0u) {                                 // If QoS is 0, xfer is cmpl.
          LOG_DBG(("Finished sending Publish QoS 0. Executing callback."));
          RTOS_ERR_SET(p_msg->Err, RTOS_ERR_NONE);
          MQTTc_MsgCallbackExec(p_msg);
        } else if (p_msg->QoS == 1u) {                          // If QoS is 1, send PUBACK reply.
          LOG_DBG(("Finished sending Publish QoS 1. Waiting to Rx Puback."));
          p_msg->Type = MQTTc_MSG_TYPE_PUBACK;
          p_msg->State = MQTTc_MSG_STATE_WAIT_RX;
          p_msg->XferLen = 0u;
        } else {                                                // If QoS is 2, send PUBREC reply.
          LOG_DBG(("Finished sending Publish QoS 2. Waiting to Rx Pubrec."));
          p_msg->Type = MQTTc_MSG_TYPE_PUBREC;
          p_msg->State = MQTTc_MSG_STATE_WAIT_RX;
          p_msg->XferLen = 0u;
        }
        break;

      case MQTTc_MSG_TYPE_PUBACK:                               // Finished sending a PUBACK, xfer is cmpl.
        LOG_DBG(("Finished sending a Puback. Removing msg from list."));
        p_msg->Type = MQTTc_MSG_TYPE_PUBLISH;
        p_msg->State = MQTTc_MSG_STATE_WAIT_RX;
        RTOS_ERR_SET(p_msg->Err, RTOS_ERR_NONE);
        break;

      case MQTTc_MSG_TYPE_PUBREC:                               // Finished sending a PUBREC, wait to rx PUBREL.
        LOG_DBG(("Finished sending a Pubrec. Waiting to Rx a Pubrel."));
        p_msg->Type = MQTTc_MSG_TYPE_PUBREL;
        p_msg->State = MQTTc_MSG_STATE_WAIT_RX;
        p_msg->XferLen = 0u;
        break;

      case MQTTc_MSG_TYPE_PUBREL:                               // Finished sending a PUBREL, wait to rx PUBCOMP.
        LOG_DBG(("Finished sending Pubrel. Waiting to Rx Pubcomp."));
        p_msg->Type = MQTTc_MSG_TYPE_PUBCOMP;
        p_msg->State = MQTTc_MSG_STATE_WAIT_RX;
        p_msg->XferLen = 0u;
        break;

      case MQTTc_MSG_TYPE_PUBCOMP:                              // Finished sending a PUBCOMP, xfer is cmpl.
        LOG_DBG(("Finished sending a Pubcomp. Removing msg from list."));
        p_msg->Type = MQTTc_MSG_TYPE_PUBLISH;
        p_msg->State = MQTTc_MSG_STATE_WAIT_RX;
        RTOS_ERR_SET(p_msg->Err, RTOS_ERR_NONE);
        break;

      case MQTTc_MSG_TYPE_SUBSCRIBE:                            // Finished sending a SUBSCRIBE, wait to rx SUBACK.
                                                                // Re-obtain nbr of topics in Subscribe msg. See ...
                                                                // Note #1 in MQTTc_SubscribeMult().
        LOG_DBG(("Finished sending Subscribe. Waiting to Rx Suback."));

        p_buf_topic_nbr = ((CPU_INT08U *)p_msg->ArgPtr) - 1u;
        topic_nbr = p_buf_topic_nbr[0u];

        p_msg->Type = MQTTc_MSG_TYPE_SUBACK;
        p_msg->State = MQTTc_MSG_STATE_WAIT_RX;
        p_msg->XferLen = topic_nbr;
        break;

      case MQTTc_MSG_TYPE_UNSUBSCRIBE:                          // Finished sending a UNSUBSCRIBE, wait to rx UNSUBACK.
        LOG_DBG(("Finished sending Unsubscribe. Waiting to Rx Unsuback."));

        p_msg->Type = MQTTc_MSG_TYPE_UNSUBACK;
        p_msg->State = MQTTc_MSG_STATE_WAIT_RX;
        p_msg->XferLen = 0u;
        break;

      case MQTTc_MSG_TYPE_PINGREQ:                              // Finished sending a PINGREQ, wait to rx PINGRESP.
        p_msg->Type = MQTTc_MSG_TYPE_PINGRESP;
        p_msg->State = MQTTc_MSG_STATE_WAIT_RX;
        p_msg->XferLen = 0u;
        break;

      case MQTTc_MSG_TYPE_DISCONNECT:                           // Finished sending a DISCONNECT, clear sel descs.
        MQTTc_MsgCallbackExec(p_msg);

        MQTTc_ConnRemove(p_conn);
        //                                                         Exec callbacks for msgs q'd under this conn.
        MQTTc_MsgListClosedCallbackExec(p_conn->TxMsgHeadPtr);
        break;

      default:
        LOG_ERR(("!!! ERROR !!! Wait Tx Cmpl switch, in default case."));
        break;
    }
  }

  goto exit;

err_remove_conn_close_sock:
  {
    MQTTc_ERR_CALLBACK on_err_callback;
    void               *p_arg;
    RTOS_ERR           err;

    on_err_callback = p_conn->OnErrCallback;
    p_arg = p_conn->ArgPtr;

    MQTTc_ConnCloseProc(p_conn, &err);

    if (on_err_callback != DEF_NULL) {
      on_err_callback(p_conn, p_arg, p_msg->Err);
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_RdSockProcess()
 *
 * @brief    Process read operations required for given MQTTc Connection.
 *
 * @param    p_conn  Pointer to MQTTc Connection object for which to process read operations.
 *******************************************************************************************************/
static void MQTTc_RdSockProcess(MQTTc_CONN *p_conn)
{
  MQTTc_MSG  *p_next_msg;
  CPU_INT08U *p_buf;
  CPU_INT32U rx_len;
  RTOS_ERR   local_err;

  if (p_conn->NextMsgPtr == DEF_NULL) {                         // If next msg is already known, skip this step.
    if (p_conn->NextMsgHeader == DEF_BIT_NONE) {
      (void)MQTTc_SockRx(p_conn,                                // Read header (type, DUP, QoS and retain) of rx'd msg.
                         &p_conn->NextMsgHeader,
                         1u,
                         &local_err);
      switch (RTOS_ERR_CODE_GET(local_err)) {
        case RTOS_ERR_NONE:
          break;

        case RTOS_ERR_WOULD_BLOCK:                          // Wait for more data to be avail to continue.
        case RTOS_ERR_TIMEOUT:
          return;

        case RTOS_ERR_NET_CONN_CLOSED_FAULT:                // Conn closed by peer...
        case RTOS_ERR_NET_CONN_CLOSE_RX:
        default:                                            // .. or fatal error.
          goto err_remove_conn_close_sock;
      }

      LOG_DBG(("Rx'd msg type ", (u)((CPU_INT08U)(p_conn->NextMsgHeader & MQTT_MSG_TYPE_MSK) >> 4u)));
      //                                                           Convert msg type to enum type.
      switch (p_conn->NextMsgHeader & MQTT_MSG_TYPE_MSK) {
        case MQTT_MSG_TYPE_CONNACK:
          p_conn->NextMsgType = MQTTc_MSG_TYPE_CONNACK;
          LOG_DBG(("Connack event rx'd on sock ID ", (d)p_conn->SockId));
          break;

        case MQTT_MSG_TYPE_PUBLISH:
          p_conn->NextMsgType = MQTTc_MSG_TYPE_PUBLISH;
          LOG_DBG(("Publish event rx'd on sock ID ", (d)p_conn->SockId));
          break;

        case MQTT_MSG_TYPE_PUBACK:
          p_conn->NextMsgType = MQTTc_MSG_TYPE_PUBACK;
          LOG_DBG(("Puback event rx'd on sock ID ", (d)p_conn->SockId));
          break;

        case MQTT_MSG_TYPE_PUBREC:
          p_conn->NextMsgType = MQTTc_MSG_TYPE_PUBREC;
          LOG_DBG(("Pubrec event rx'd on sock ID ", (d)p_conn->SockId));
          break;

        case MQTT_MSG_TYPE_PUBREL:
          p_conn->NextMsgType = MQTTc_MSG_TYPE_PUBREL;
          LOG_DBG(("Pubrel event rx'd on sock ID ", (d)p_conn->SockId));
          break;

        case MQTT_MSG_TYPE_PUBCOMP:
          p_conn->NextMsgType = MQTTc_MSG_TYPE_PUBCOMP;
          LOG_DBG(("Pubcomp event rx'd on sock ID ", (d)p_conn->SockId));
          break;

        case MQTT_MSG_TYPE_SUBACK:
          p_conn->NextMsgType = MQTTc_MSG_TYPE_SUBACK;
          LOG_DBG(("Suback event rx'd on sock ID ", (d)p_conn->SockId));
          break;

        case MQTT_MSG_TYPE_UNSUBACK:
          p_conn->NextMsgType = MQTTc_MSG_TYPE_UNSUBACK;
          LOG_DBG(("Unsuback event rx'd on sock ID ", (d)p_conn->SockId));
          break;

        case MQTT_MSG_TYPE_PINGRESP:
          p_conn->NextMsgType = MQTTc_MSG_TYPE_PINGRESP;
          break;

        case MQTT_MSG_TYPE_CONNECT:
        case MQTT_MSG_TYPE_SUBSCRIBE:
        case MQTT_MSG_TYPE_UNSUBSCRIBE:
        case MQTT_MSG_TYPE_PINGREQ:
        case MQTT_MSG_TYPE_DISCONNECT:
        default:
          LOG_ERR(("!!! ERROR !!! Received wrong message type: ", (u)p_conn->NextMsgHeader));
          p_conn->NextMsgHeader = 0u;
          goto err_restart;                                     // These msg types cannot be rx'd. Flush rx buf.
      }
      p_conn->NextMsgRxLen = 0u;
    }
    //                                                             Make sure msg being rx'd is expected.
    if (p_conn->NextMsgType != p_conn->PublishRxMsgPtr->Type) {
      if (p_conn->TxMsgHeadPtr != DEF_NULL) {
        if (p_conn->NextMsgType != p_conn->TxMsgHeadPtr->Type) {
          LOG_ERR(("!!! ERROR !!! p_conn->NextMsgType != p_conn->TxMsgHeadPtr->Type ", (u)p_conn->NextMsgHeader));
          goto err_restart;
        }
      } else {
        LOG_ERR(("!!! ERROR !!! p_conn->TxMsgHeadPtr == null", (u)p_conn->NextMsgHeader));
        goto err_restart;
      }
    }

    if (p_conn->NextMsgLenIsCmpl == DEF_NO) {
      CPU_INT08U rem_len;
      CPU_INT32U rx_len;
      CPU_INT32U multiplier;

      do {                                                      // Read rem len of msg. This can be a multi-byte field.
        rx_len = MQTTc_SockRx(p_conn,
                              &rem_len,
                              1u,
                              &local_err);
        switch (RTOS_ERR_CODE_GET(local_err)) {
          case RTOS_ERR_NONE:
            //                                                     Calculate the multiplier which is a power of 128.
            //                                                     It can take the value 1, 128, 16384 or 2097152.
            multiplier = 1 << (7 * p_conn->NextMsgRxLen);
            p_conn->NextMsgRxLen += rx_len;
            break;

          case RTOS_ERR_WOULD_BLOCK:                            // Wait for more data to be avail to continue.
          case RTOS_ERR_TIMEOUT:
            return;

          case RTOS_ERR_NET_CONN_CLOSED_FAULT:                  // Conn closed by peer...
          case RTOS_ERR_NET_CONN_CLOSE_RX:
          default:                                              // .. or fatal error.
            goto err_remove_conn_close_sock;
        }

        p_conn->NextMsgLen += (rem_len & MQTT_MSG_FIXED_HDR_REM_LEN_MSK) * multiplier;

        //                                                         Read msg as long as continuation bit is set, max 4x.
      }  while ((DEF_BIT_IS_SET(rem_len, MQTT_MSG_FIXED_HDR_REM_LEN_CONTINUATION_BIT) == DEF_YES)
                && (p_conn->NextMsgRxLen < MQTT_MSG_FIXED_HDR_REM_LEN_NBR_BYTES_MAX));

      //                                                           Save data remaining length to find end of payload ...
      //                                                           ... in case of a received Publish message.
      if (p_conn->NextMsgType == MQTTc_MSG_TYPE_PUBLISH) {
        p_conn->PublishRemLen = p_conn->NextMsgLen;
      }

      p_conn->NextMsgLenIsCmpl = DEF_YES;
      p_conn->NextMsgRxLen = 0u;

      LOG_DBG(("Finished reading msg len: ", (u)p_conn->NextMsgLen, " on sock ID ", (d)p_conn->SockId));
    }

    if ( (p_conn->NextMsgMsgID_IsCmpl == DEF_NO)
         && ((p_conn->NextMsgType == MQTTc_MSG_TYPE_PUBACK)
             || (p_conn->NextMsgType == MQTTc_MSG_TYPE_PUBREC)
             || (p_conn->NextMsgType == MQTTc_MSG_TYPE_PUBREL)
             || (p_conn->NextMsgType == MQTTc_MSG_TYPE_PUBCOMP)
             || (p_conn->NextMsgType == MQTTc_MSG_TYPE_SUBACK)
             || (p_conn->NextMsgType == MQTTc_MSG_TYPE_UNSUBACK))) {
      CPU_INT08U msg_id_rx[MQTT_MSG_ID_SIZE];

      if (p_conn->NextMsgRxLen == 1u) {                         // If already rx'd 1 byte of MsgID, re-put in msg_id_rx.
        msg_id_rx[0u] = (p_conn->NextMsgMsgID & 0xFF00u) >> 8u;
      }

      rx_len = MQTTc_SockRx(p_conn,                             // Rx msg ID if msg has one.
                            &msg_id_rx[p_conn->NextMsgRxLen],
                            (MQTT_MSG_ID_SIZE - p_conn->NextMsgRxLen),
                            &local_err);
      switch (RTOS_ERR_CODE_GET(local_err)) {
        case RTOS_ERR_NONE:
          p_conn->NextMsgRxLen += rx_len;
          break;

        case RTOS_ERR_WOULD_BLOCK:                              // Wait for more data to be avail to continue.
        case RTOS_ERR_TIMEOUT:
          return;

        case RTOS_ERR_NET_CONN_CLOSED_FAULT:                    // Conn closed by peer...
        case RTOS_ERR_NET_CONN_CLOSE_RX:
        default:                                                // ... or fatal error.
          goto err_remove_conn_close_sock;
      }
      if (p_conn->NextMsgRxLen < MQTT_MSG_ID_SIZE) {            // Keep first part of msg ID rx'd.
        p_conn->NextMsgMsgID = (msg_id_rx[0u] << 8u);
        return;                                                 // Wait for more data to be avail to continue.
      }
      p_conn->NextMsgMsgID = ((msg_id_rx[0u] << 8u) | msg_id_rx[1u]);
      p_conn->NextMsgLen -= MQTT_MSG_ID_SIZE;
      p_conn->NextMsgMsgID_IsCmpl = DEF_YES;
      p_conn->NextMsgRxLen = 0u;

      LOG_DBG(("Finished reading next msg msg ID."));
    }

    if (p_conn->NextMsgType == p_conn->PublishRxMsgPtr->Type) {
      p_conn->NextMsgPtr = p_conn->PublishRxMsgPtr;
      //                                                           Account for header that may need to be sent.
      //                                                           Start rx'ing useful data at offset, to leave room.
      p_conn->NextMsgRxLen = MQTTc_PUBLISH_RX_MSG_BUF_OFFSET;
      if ((p_conn->NextMsgLen + MQTTc_PUBLISH_RX_MSG_BUF_OFFSET) > p_conn->NextMsgPtr->BufLen) {
        LOG_ERR(("!!! ERROR !!! Next msg len of Publish Msg (", (u)p_conn->NextMsgLen, " (+4 from header)) too big for msg buf ", (u)p_conn->NextMsgPtr->BufLen));
        RTOS_ERR_SET(p_conn->NextMsgPtr->Err, RTOS_ERR_WOULD_OVF);
        goto err_callback_restart;
      }
    } else {
      p_conn->NextMsgPtr = p_conn->TxMsgHeadPtr;

      if (p_conn->NextMsgLen != p_conn->NextMsgPtr->XferLen) {
        LOG_ERR(("!!! ERROR !!! Next msg len (", (u)p_conn->NextMsgLen, ") not equal to expected xfer len ", (u)p_conn->NextMsgPtr->XferLen));
        RTOS_ERR_SET(p_conn->NextMsgPtr->Err, RTOS_ERR_WOULD_OVF);
        goto err_callback_restart;
      }
      if (p_conn->NextMsgLen > p_conn->NextMsgPtr->BufLen) {
        LOG_ERR(("!!! ERROR !!! Next msg len (", (u)p_conn->NextMsgLen, ") too big for msg buf ", (u)p_conn->NextMsgPtr->BufLen));
        RTOS_ERR_SET(p_conn->NextMsgPtr->Err, RTOS_ERR_WOULD_OVF);
        goto err_callback_restart;
      }
    }
  }

  //                                                               At this point, p_conn->NextMsgPtr contains the ...
  //                                                               next msg to process and all its infos have been ...
  //                                                               rx'd. Only the payload still needs to be rx'd.
  p_next_msg = p_conn->NextMsgPtr;

  if (p_conn->NextMsgLen != 0u) {                               // If there is more than the hdr to rx, rx it.
    LOG_DBG(("Rx'ing payload. Trying to read ", (u)p_conn->NextMsgLen, " bytes. Already rx'd ", (u)p_conn->NextMsgRxLen, " bytes."));

    rx_len = MQTTc_SockRx(p_conn,
                          &(((CPU_INT08U *)p_next_msg->ArgPtr)[p_conn->NextMsgRxLen]),
                          p_conn->NextMsgLen,
                          &local_err);
    p_conn->NextMsgLen -= rx_len;
    p_conn->NextMsgRxLen += rx_len;
    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:
        break;

      case RTOS_ERR_WOULD_BLOCK:                                // Wait for more data to be avail to continue.
      case RTOS_ERR_TIMEOUT:
        return;

      case RTOS_ERR_NET_CONN_CLOSED_FAULT:                      // Conn closed by peer...
      case RTOS_ERR_NET_CONN_CLOSE_RX:
      default:                                                  // .. or fatal error.
        goto err_remove_conn_close_sock;
    }
  }

  LOG_DBG(("Finished rx'ing msg payload. Rx'd ", (u)p_conn->NextMsgRxLen, " bytes."));

  //                                                               At this point, the payload has been completely rx'd.
  MQTTc_DBG_GLOBAL_BUF_COPY((CPU_INT08U *)p_next_msg->ArgPtr, p_conn->NextMsgRxLen);

  if (p_next_msg->Type == MQTTc_MSG_TYPE_PUBLISH) {             // Rx'd a Publish msg from broker.
                                                                // 'p_next_msg' points to p_conn->PublishRxMsgPtr.
    p_next_msg->QoS = (p_conn->NextMsgHeader & MQTT_MSG_FIXED_HDR_FLAGS_QOS_LVL_MSK) >> MQTT_MSG_FIXED_HDR_FLAGS_QOS_LVL_BIT_SHIFT;

    if (p_next_msg->QoS == 0u) {                                // If QoS is 0, msg is cmpl'd. Exec callback.
      LOG_DBG(("MQTTc - Read a Publish (QoS=0) successfully. Executing callback."));
      RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_NONE);
      MQTTc_MsgCallbackExec(p_next_msg);
    } else {
      MQTTc_MSG_TYPE type = MQTTc_MSG_TYPE_PUBREC;
      CPU_INT16U     msg_id;
      CPU_INT16U     len;

      len = MQTT_MSG_UTF8_LEN_RD(&(((CPU_INT08U *)p_next_msg->ArgPtr)[MQTTc_PUBLISH_RX_MSG_BUF_OFFSET])) + MQTT_MSG_UTF8_LEN_SIZE;
      msg_id = MQTT_MSG_UTF8_LEN_RD(&(((CPU_INT08U *)p_next_msg->ArgPtr)[len + MQTTc_PUBLISH_RX_MSG_BUF_OFFSET]));

      if (p_next_msg->QoS == 1u) {                              // Callback must be called now only for QoS 1.
        RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_NONE);
        MQTTc_MsgCallbackExec(p_next_msg);

        type = MQTTc_MSG_TYPE_PUBACK;
      }

      //                                                           Cfg fixed hdr section of msg for reply.
      p_buf = MQTTc_FixedHdrBufCfg((CPU_INT08U *)p_next_msg->ArgPtr,
                                   type,
                                   DEF_NO,
                                   0u,
                                   DEF_NO,
                                   MQTT_MSG_ID_SIZE,
                                   &p_next_msg->Err);

      p_buf[0u] = (msg_id >>    8u);
      p_buf[1u] = (msg_id &  0xFFu);

      LOG_DBG(("MQTTc - Read a Publish (QoS=", (u)p_next_msg->QoS, ") successfully. Sending a Puback/Pubrec with Msg ID: ", (u)msg_id));

      MQTTc_SockSelDescSet(p_conn, MQTTc_SEL_DESC_TYPE_WR);

      p_next_msg->Type = type;
      p_next_msg->State = MQTTc_MSG_STATE_MUST_TX;
      p_next_msg->XferLen = MQTT_MSG_BASE_LEN;
      p_next_msg->MsgID = msg_id;
      RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_NONE);
    }

    MQTTc_ConnNextMsgClr(p_conn);                               // Clr NextMsg fields.

    return;
  } else {
    CPU_INT08U *p_buf_topic_nbr;
    CPU_INT08U topic_nbr;
    CPU_INT08U topic_ix;

    switch (p_next_msg->Type) {
      case MQTTc_MSG_TYPE_CONNACK:
        if ((p_conn->NextMsgRxLen != 2u)
            || (((CPU_INT08U *)p_next_msg->ArgPtr)[1u] != MQTT_MSG_VAR_HDR_CONNACK_RET_CODE_ACCEPTED)) {
          LOG_DBG(("MQTTc - Connack code not OK."));
          RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_FAIL);
        } else {
          LOG_DBG(("MQTTc - Connack code OK."));
          RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_NONE);
        }
        break;

      case MQTTc_MSG_TYPE_PUBACK:
        if (p_conn->NextMsgRxLen != 0u) {
          LOG_DBG(("MQTTc - Puback rx'd code not OK."));
          RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_MQTT_MSG_FAIL);
        } else {
          RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_NONE);
        }
        break;

      case MQTTc_MSG_TYPE_SUBACK:
        RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_NONE);

        p_buf_topic_nbr = ((CPU_INT08U *)p_next_msg->ArgPtr) - 1u;
        topic_nbr = p_buf_topic_nbr[0u];

        for (topic_ix = 0u; topic_ix < topic_nbr; topic_ix++) {
          p_buf_topic_nbr--;
          if ((*p_buf_topic_nbr) != ((CPU_INT08U *)p_next_msg->ArgPtr)[topic_ix]) {
            RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_MQTTc_QoS_LEVEL_NOT_GRANTED);
            LOG_ERR(("!!! ERROR !!! QoS Level not granted by server. Asked ", (u) * p_buf_topic_nbr, ", got ", (u)((CPU_INT08U *)p_next_msg->ArgPtr)[topic_ix]));
            break;
          }
        }

        if (p_conn->NextMsgRxLen == 0u) {
          LOG_DBG(("MQTTc - Suback rx'd len not OK."));
          RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_MQTT_MSG_FAIL);
        }
        p_next_msg->ArgPtr = (void *)p_buf_topic_nbr;
        break;

      case MQTTc_MSG_TYPE_UNSUBACK:
        if (p_conn->NextMsgRxLen != 0u) {
          LOG_DBG(("MQTTc - Suback rx'd len not OK."));
          RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_MQTT_MSG_FAIL);
        } else {
          RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_NONE);
        }
        break;

      case MQTTc_MSG_TYPE_PINGRESP:
        break;

      case MQTTc_MSG_TYPE_PUBREC:
        LOG_DBG(("Pubrec event rx'd, removing msg from list."));

        p_buf = MQTTc_FixedHdrBufCfg((CPU_INT08U *)p_next_msg->ArgPtr,
                                     MQTTc_MSG_TYPE_PUBREL,
                                     DEF_NO,
                                     1u,
                                     DEF_NO,
                                     MQTT_MSG_ID_SIZE,
                                     &p_next_msg->Err);

        p_buf[0u] = (p_next_msg->MsgID >>    8u);
        p_buf[1u] = (p_next_msg->MsgID &  0xFFu);

        MQTTc_SockSelDescSet(p_conn, MQTTc_SEL_DESC_TYPE_WR);

        p_next_msg->Type = MQTTc_MSG_TYPE_PUBREL;
        p_next_msg->State = MQTTc_MSG_STATE_MUST_TX;
        p_next_msg->XferLen = MQTT_MSG_BASE_LEN;
        RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_NONE);

        MQTTc_ConnNextMsgClr(p_conn);                           // Clr NextMsg fields.
        return;

      case MQTTc_MSG_TYPE_PUBREL:
        LOG_DBG(("Pubrel event rx'd, removing msg from list."));

        MQTTc_MsgCallbackExec(p_next_msg);

        p_buf = MQTTc_FixedHdrBufCfg((CPU_INT08U *)p_next_msg->ArgPtr,
                                     MQTTc_MSG_TYPE_PUBCOMP,
                                     DEF_NO,
                                     1u,
                                     DEF_NO,
                                     MQTT_MSG_ID_SIZE,
                                     &p_next_msg->Err);

        p_buf[0u] = (p_next_msg->MsgID >>    8u);
        p_buf[1u] = (p_next_msg->MsgID &  0xFFu);

        MQTTc_SockSelDescSet(p_conn, MQTTc_SEL_DESC_TYPE_WR);

        p_next_msg->Type = MQTTc_MSG_TYPE_PUBCOMP;
        p_next_msg->State = MQTTc_MSG_STATE_MUST_TX;
        p_next_msg->XferLen = MQTT_MSG_BASE_LEN;
        RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_NONE);

        MQTTc_ConnNextMsgClr(p_conn);                           // Clr NextMsg fields.
        return;

      case MQTTc_MSG_TYPE_PUBCOMP:
        if (p_conn->NextMsgRxLen != 0u) {
          LOG_DBG(("MQTTc - Pubcomp rx'd len not OK."));
          RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_MQTT_MSG_FAIL);
        } else {
          RTOS_ERR_SET(p_next_msg->Err, RTOS_ERR_NONE);
        }
        break;

      default:
        LOG_ERR(("!!! ERROR !!! In Rx switch default case."));
        break;
    }

    MQTTc_ConnNextMsgClr(p_conn);                               // Clr NextMsg fields.

    MQTTc_MsgCallbackExec(p_next_msg);
  }

  return;

err_callback_restart:
  MQTTc_MsgCallbackExec(p_conn->NextMsgPtr);
  MQTTc_ConnNextMsgClr(p_conn);                                 // Clr NextMsg fields.

  return;

err_restart:
  MQTTc_ConnNextMsgClr(p_conn);                                 // Clr NextMsg fields.
  RTOS_ERR_SET(local_err, RTOS_ERR_RX);

err_remove_conn_close_sock:
  {
    MQTTc_ERR_CALLBACK on_err_callback;
    void               *p_arg;
    RTOS_ERR           err;

    on_err_callback = p_conn->OnErrCallback;
    p_arg = p_conn->ArgPtr;

    MQTTc_ConnCloseProc(p_conn, &err);

    if (on_err_callback != DEF_NULL) {
      on_err_callback(p_conn, p_arg, local_err);
    }
  }

  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_MsgProcess()
 *
 * @brief    Process message pending and enqueuing for MQTTc task.
 *******************************************************************************************************/
static void MQTTc_MsgProcess(void)
{
  MQTTc_MSG *p_msg;

  p_msg = MQTTc_MsgCheck();
  if (p_msg != DEF_NULL) {
    if (p_msg->Type != MQTTc_MSG_TYPE_REQ_CLOSE) {
      MQTTc_CONN     *p_conn = p_msg->ConnPtr;
      MQTTc_MSG_TYPE type = p_msg->Type;

      if (p_conn->TxMsgHeadPtr == DEF_NULL) {                       // Enqueue msg to appropriate MQTTc conn.
        p_conn->TxMsgHeadPtr = p_msg;
      } else {
        MQTTc_MSG *p_iter_msg = p_conn->TxMsgHeadPtr;

        while (p_iter_msg->NextPtr != DEF_NULL) {
          p_iter_msg = p_iter_msg->NextPtr;
        }
        p_iter_msg->NextPtr = p_msg;
      }

      switch (type) {
        case MQTTc_MSG_TYPE_CONNECT:
          if (MQTTc_DataPtr->ConnHeadPtr == DEF_NULL) {                  // Enqueue conn in MQTTc conn list.
            MQTTc_DataPtr->ConnHeadPtr = p_conn;
          } else {
            MQTTc_CONN *p_iter_conn = MQTTc_DataPtr->ConnHeadPtr;

            while (p_iter_conn->NextPtr != DEF_NULL) {
              p_iter_conn = p_iter_conn->NextPtr;
            }
            p_iter_conn->NextPtr = p_conn;
          }
        //                                                         Fallthrough

        case MQTTc_MSG_TYPE_PUBLISH:
        case MQTTc_MSG_TYPE_PUBREL:
        case MQTTc_MSG_TYPE_SUBSCRIBE:
        case MQTTc_MSG_TYPE_UNSUBSCRIBE:
        case MQTTc_MSG_TYPE_PINGREQ:
        case MQTTc_MSG_TYPE_DISCONNECT:
          MQTTc_SockSelDescSet(p_conn, MQTTc_SEL_DESC_TYPE_WR);
          break;

        case MQTTc_MSG_TYPE_PUBACK:
        case MQTTc_MSG_TYPE_PUBREC:
        case MQTTc_MSG_TYPE_PUBCOMP:
        case MQTTc_MSG_TYPE_CONNACK:
        case MQTTc_MSG_TYPE_SUBACK:
        case MQTTc_MSG_TYPE_UNSUBACK:
        case MQTTc_MSG_TYPE_PINGRESP:
        default:
          LOG_ERR(("!!! ERROR !!! In default case for event type: ", (u)(CPU_INT08U)type));
          break;
      }
    } else {                                                    // Handle special close req msg.
      RTOS_ERR local_err;

      MQTTc_ConnCloseProc(p_msg->ConnPtr,
                          &p_msg->Err);

      KAL_SemPost((*(KAL_SEM_HANDLE *)p_msg->ArgPtr),
                  KAL_OPT_POST_NONE,
                  &local_err);
      PP_UNUSED_PARAM(local_err);
    }
  }
}

/****************************************************************************************************//**
 *                                           MQTTc_MsgCallbackExec()
 *
 * @brief    Frees msg if provided by user and execute application callback(s).
 *
 * @param    p_msg   Pointer to message for which the callback must be called.
 *******************************************************************************************************/
static void MQTTc_MsgCallbackExec(MQTTc_MSG *p_msg)
{
  MQTTc_CMPL_CALLBACK callback_fnct = DEF_NULL;
  MQTTc_CONN          *p_conn = p_msg->ConnPtr;
  RTOS_ERR            local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  if (p_msg != p_conn->PublishRxMsgPtr) {
    switch (p_msg->Type) {                                      // Find type of msg and if ok to call callback for it.
      case MQTTc_MSG_TYPE_CONNECT:
        RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
      //                                                           Fallthrough

      case MQTTc_MSG_TYPE_CONNACK:
        p_msg->Type = MQTTc_MSG_TYPE_CONNECT;
        callback_fnct = p_conn->OnConnectCmpl;
        if (p_msg->State != MQTTc_MSG_STATE_WAIT_RX) {
          RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
        }
        break;

      case MQTTc_MSG_TYPE_PUBLISH:
        callback_fnct = p_conn->OnPublishCmpl;
        if ((p_msg->QoS != 0u)
            || (p_msg->State != MQTTc_MSG_STATE_WAIT_TX_CMPL)) {
          RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
        }
        break;

      case MQTTc_MSG_TYPE_PUBACK:
      case MQTTc_MSG_TYPE_PUBCOMP:
        p_msg->Type = MQTTc_MSG_TYPE_PUBLISH;
        callback_fnct = p_conn->OnPublishCmpl;
        if (p_msg->State != MQTTc_MSG_STATE_WAIT_RX) {
          RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
        }
        break;

      case MQTTc_MSG_TYPE_PUBREC:
      case MQTTc_MSG_TYPE_PUBREL:
        p_msg->Type = MQTTc_MSG_TYPE_PUBLISH;
        callback_fnct = p_conn->OnPublishCmpl;
        RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
        break;

      case MQTTc_MSG_TYPE_SUBSCRIBE:
        RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
      //                                                           Fallthrough

      case MQTTc_MSG_TYPE_SUBACK:
        p_msg->Type = MQTTc_MSG_TYPE_SUBSCRIBE;
        callback_fnct = p_conn->OnSubscribeCmpl;
        if (p_msg->State != MQTTc_MSG_STATE_WAIT_RX) {
          RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
        }
        break;

      case MQTTc_MSG_TYPE_UNSUBSCRIBE:
        RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
      //                                                           Fallthrough

      case MQTTc_MSG_TYPE_UNSUBACK:
        p_msg->Type = MQTTc_MSG_TYPE_UNSUBSCRIBE;
        callback_fnct = p_conn->OnUnsubscribeCmpl;
        if (p_msg->State != MQTTc_MSG_STATE_WAIT_RX) {
          RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
        }
        break;

      case MQTTc_MSG_TYPE_PINGREQ:
        RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
      //                                                           Fallthrough

      case MQTTc_MSG_TYPE_PINGRESP:
        p_msg->Type = MQTTc_MSG_TYPE_PINGREQ;
        callback_fnct = p_conn->OnPingReqCmpl;
        if (p_msg->State != MQTTc_MSG_STATE_WAIT_RX) {
          RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
        }
        break;

      case MQTTc_MSG_TYPE_DISCONNECT:
        p_msg->Type = MQTTc_MSG_TYPE_DISCONNECT;
        callback_fnct = p_conn->OnDisconnectCmpl;
        if (p_msg->State != MQTTc_MSG_STATE_WAIT_TX_CMPL) {
          RTOS_ERR_SET(local_err, RTOS_ERR_MQTT_MSG_FAIL);
        }
        break;

      default:
        LOG_ERR(("!!! ERROR !!! In default case for p_event->Type when executing callback: ", (u)(CPU_INT08U)p_msg->Type));
        return;
    }

    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_ERR(("!!! ERROR !!! Detected error when executing callback: ", RTOS_ERR_LOG_ARG_GET(local_err)));
    }

    if (RTOS_ERR_CODE_GET(p_msg->Err) == RTOS_ERR_NONE) {
      RTOS_ERR_COPY(p_msg->Err, local_err);
    } else {
      LOG_ERR(("MQTTc - Callback executed for Msg with error: %", RTOS_ERR_LOG_ARG_GET(p_msg->Err)));
    }

    p_msg->State = MQTTc_MSG_STATE_CMPL;

    MQTTc_MsgID_Free(p_msg->MsgID);                             // Free msg ID, if any.

    //                                                             Remove msg from conn's msg list.
    //                                                             Msg is necessarily located at head of list.
    p_msg->ConnPtr->TxMsgHeadPtr = p_msg->NextPtr;
    p_msg->NextPtr = DEF_NULL;

    if (p_conn->OnCmpl != DEF_NULL) {                           // Call generic callback, if not NULL.
      p_conn->OnCmpl(p_conn,
                     p_msg,
                     p_conn->ArgPtr,
                     p_msg->Err);
    }

    if (callback_fnct != DEF_NULL) {                            // Call action-specific callback, if not NULL.
      callback_fnct(p_conn,
                    p_msg,
                    p_conn->ArgPtr,
                    p_msg->Err);
    }
  } else if (p_conn->OnPublishRx != DEF_NULL) {                 // Call OnPublishRx callback, if not NULL.
    CPU_INT08U *p_buf_start = &(((CPU_INT08U *)p_msg->ArgPtr)[MQTTc_PUBLISH_RX_MSG_BUF_OFFSET]);
    CPU_INT08U *p_buf_topic = &p_buf_start[MQTT_MSG_UTF8_LEN_SIZE];
    CPU_INT08U *p_buf_payload;
    CPU_INT32U topic_len;
    CPU_INT32U len;

    MQTTc_DBG_GLOBAL_BUF_COPY(p_buf_start, 512u);

    topic_len = MQTT_MSG_UTF8_LEN_RD(p_buf_start);
    len = topic_len + MQTT_MSG_UTF8_LEN_SIZE;                   // Account for length.

    if (p_msg->QoS != 0u) {                                     // Account for msg ID size if needed.
      len += MQTT_MSG_ID_SIZE;
    }

    p_buf_payload = &p_buf_start[len];                          // Find start of payload.

    len = p_conn->PublishRemLen - len;                          // Find length of payload.

    p_conn->OnPublishRx(p_conn,
                        (const CPU_CHAR *)p_buf_topic,
                        topic_len,
                        (const CPU_INT08U *)p_buf_payload,
                        len,
                        p_conn->ArgPtr,
                        p_msg->Err);
  }

  return;
}

/****************************************************************************************************//**
 *                                               MQTTc_MsgCheck()
 *
 * @brief    See if a message is available in the message queue.
 *
 * @return   Pointer to obtained message, if any,
 *           DEF_NULL,                    otherwise.
 *******************************************************************************************************/
static MQTTc_MSG *MQTTc_MsgCheck(void)
{
  MQTTc_MSG *p_msg = DEF_NULL;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (MQTTc_DataPtr->MsgListHeadPtr != DEF_NULL) {
    p_msg = MQTTc_DataPtr->MsgListHeadPtr;
    MQTTc_DataPtr->MsgListHeadPtr = p_msg->NextPtr;
    p_msg->NextPtr = DEF_NULL;
  }
  CORE_EXIT_ATOMIC();

  return (p_msg);
}

/****************************************************************************************************//**
 *                                               MQTTc_MsgPost()
 *
 * @brief    Add a message in the MQTT message queue for the task to process.
 *
 * @param    p_conn      Pointer to MQTT Connection object associated with message.
 *
 * @param    p_msg       Pointer to MQTT Message object to add to queue.
 *
 * @param    type        Type of MQTT message.
 *
 * @param    xfer_len    Required len to xfer.
 *
 * @param    qos_lvl     QoS level of the message, if any.
 *
 * @param    msg_id      Message ID of the message, if any.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void MQTTc_MsgPost(MQTTc_CONN     *p_conn,
                          MQTTc_MSG      *p_msg,
                          MQTTc_MSG_TYPE type,
                          CPU_INT32U     xfer_len,
                          CPU_INT08U     qos_lvl,
                          CPU_INT16U     msg_id,
                          RTOS_ERR       *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  p_msg->ConnPtr = p_conn;                                      // Set values in msg fields.
  p_msg->Type = type;
  p_msg->State = MQTTc_MSG_STATE_MUST_TX;
  p_msg->MsgID = msg_id;
  p_msg->XferLen = xfer_len;
  p_msg->QoS = qos_lvl;
  p_msg->NextPtr = DEF_NULL;

  RTOS_ERR_SET(p_msg->Err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  if (p_conn->SockId != NET_SOCK_ID_NONE) {
    if (MQTTc_DataPtr->MsgListHeadPtr != DEF_NULL) {
      MQTTc_DataPtr->MsgListTailPtr->NextPtr = p_msg;
    } else {
      MQTTc_DataPtr->MsgListHeadPtr = p_msg;
    }
    MQTTc_DataPtr->MsgListTailPtr = p_msg;

    CORE_EXIT_ATOMIC();

    if (MQTTc_DataPtr->ConnHeadPtr == DEF_NULL) {
      KAL_SemPost(MQTTc_DataPtr->TaskPendSem, KAL_OPT_PEND_NONE, p_err);
    }

    MQTTc_SockSelDescSet(p_conn, MQTTc_SEL_DESC_TYPE_WR);
  } else {
    CORE_EXIT_ATOMIC();

    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
  }
  return;
}

/****************************************************************************************************//**
 *                                       MQTTc_MsgListClosedCallbackExec()
 *
 * @brief    Execute callback with error MQTTc_ERR_CONN_IS_CLOSED on every message in linked list.
 *
 * @param    p_head_msg  Pointer to head of list of messages to execute callback on.
 *******************************************************************************************************/
static void MQTTc_MsgListClosedCallbackExec(MQTTc_MSG *p_head_msg)
{
  MQTTc_MSG *p_iter_msg;
  MQTTc_MSG *p_next_iter_msg;

  p_iter_msg = p_head_msg;                                      // Exec callback for every msg in list passed.
  while (p_iter_msg != DEF_NULL) {
    p_next_iter_msg = p_iter_msg->NextPtr;
    //                                                             Indicate conn is closed.
    RTOS_ERR_SET(p_iter_msg->Err, RTOS_ERR_NET_SOCK_CLOSED);
    MQTTc_MsgCallbackExec(p_iter_msg);

    p_iter_msg = p_next_iter_msg;
  }
}

/****************************************************************************************************//**
 *                                           MQTTc_FixedHdrBufCfg()
 *
 * @brief    Fill buffer with correct fields of fixed header.
 *
 * @param    p_buf           Pointer to beginning of the buffer to fill.
 *
 * @param    msg_type        Type of message.
 *
 * @param    dup_flag        DUP flag.
 *
 * @param    qos_lvl         QoS level of the message.
 *
 * @param    retain_flag     Retain flag.
 *
 * @param    rem_len         Remaining length of the message.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to next location in buffer, if NO error(s),
 *           DEF_NULL,                           otherwise.
 *******************************************************************************************************/
static CPU_INT08U *MQTTc_FixedHdrBufCfg(CPU_INT08U     *p_buf,
                                        MQTTc_MSG_TYPE msg_type,
                                        CPU_BOOLEAN    dup_flag,
                                        CPU_INT08U     qos_lvl,
                                        CPU_BOOLEAN    retain_flag,
                                        CPU_INT32U     rem_len,
                                        RTOS_ERR       *p_err)
{
  CPU_INT08U *p_cur_buf;
  CPU_INT32U len = rem_len;
  CPU_INT08U encoded_byte;

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Make sure len can be encoded.
  RTOS_ASSERT_DBG_ERR_SET((rem_len <= MQTT_MSG_FIXED_HDR_REM_LEN_MAX), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  p_cur_buf = p_buf;

  switch (msg_type) {                                           // Convert msg type.
    case MQTTc_MSG_TYPE_CONNECT:
      *p_cur_buf = MQTT_MSG_TYPE_CONNECT;
      break;

    case MQTTc_MSG_TYPE_CONNACK:
      *p_cur_buf = MQTT_MSG_TYPE_CONNACK;
      break;

    case MQTTc_MSG_TYPE_PUBLISH:
      *p_cur_buf = MQTT_MSG_TYPE_PUBLISH;
      break;

    case MQTTc_MSG_TYPE_PUBACK:
      *p_cur_buf = MQTT_MSG_TYPE_PUBACK;
      break;

    case MQTTc_MSG_TYPE_PUBREC:
      *p_cur_buf = MQTT_MSG_TYPE_PUBREC;
      break;

    case MQTTc_MSG_TYPE_PUBREL:
      *p_cur_buf = MQTT_MSG_TYPE_PUBREL;
      break;

    case MQTTc_MSG_TYPE_PUBCOMP:
      *p_cur_buf = MQTT_MSG_TYPE_PUBCOMP;
      break;

    case MQTTc_MSG_TYPE_SUBSCRIBE:
      *p_cur_buf = MQTT_MSG_TYPE_SUBSCRIBE;
      break;

    case MQTTc_MSG_TYPE_SUBACK:
      *p_cur_buf = MQTT_MSG_TYPE_SUBACK;
      break;

    case MQTTc_MSG_TYPE_UNSUBSCRIBE:
      *p_cur_buf = MQTT_MSG_TYPE_UNSUBSCRIBE;
      break;

    case MQTTc_MSG_TYPE_UNSUBACK:
      *p_cur_buf = MQTT_MSG_TYPE_UNSUBACK;
      break;

    case MQTTc_MSG_TYPE_PINGREQ:
      *p_cur_buf = MQTT_MSG_TYPE_PINGREQ;
      break;

    case MQTTc_MSG_TYPE_PINGRESP:
      *p_cur_buf = MQTT_MSG_TYPE_PINGRESP;
      break;

    case MQTTc_MSG_TYPE_DISCONNECT:
      *p_cur_buf = MQTT_MSG_TYPE_DISCONNECT;
      break;

    default:
      break;
  }

  switch (msg_type) {
    case MQTTc_MSG_TYPE_PUBLISH:                                // Set every val.
      if (retain_flag == DEF_YES) {
        DEF_BIT_SET(*p_cur_buf, MQTT_MSG_FIXED_HDR_FLAGS_RETAIN_MSK);
      } else {
        DEF_BIT_CLR(*p_cur_buf, (CPU_INT08U)MQTT_MSG_FIXED_HDR_FLAGS_RETAIN_MSK);
      }
    //                                                             Fallthrough

    case MQTTc_MSG_TYPE_PUBREL:                                 // Set everything except 'retain'.
    case MQTTc_MSG_TYPE_SUBSCRIBE:
    case MQTTc_MSG_TYPE_UNSUBSCRIBE:
      if (dup_flag == DEF_YES) {
        DEF_BIT_SET(*p_cur_buf, MQTT_MSG_FIXED_HDR_FLAGS_DUP_MSK);
      } else {
        DEF_BIT_CLR(*p_cur_buf, (CPU_INT08U)MQTT_MSG_FIXED_HDR_FLAGS_DUP_MSK);
      }
      DEF_BIT_SET(*p_cur_buf, (qos_lvl << MQTT_MSG_FIXED_HDR_FLAGS_QOS_LVL_BIT_SHIFT));
    //                                                             Fallthrough

    case MQTTc_MSG_TYPE_CONNECT:                                // Set only the msg_type.
    case MQTTc_MSG_TYPE_PUBACK:
    case MQTTc_MSG_TYPE_PUBREC:
    case MQTTc_MSG_TYPE_PUBCOMP:
    case MQTTc_MSG_TYPE_PINGREQ:
    case MQTTc_MSG_TYPE_DISCONNECT:
      break;

    case MQTTc_MSG_TYPE_CONNACK:                                // Err cases.
    case MQTTc_MSG_TYPE_SUBACK:
    case MQTTc_MSG_TYPE_UNSUBACK:
    case MQTTc_MSG_TYPE_PINGRESP:
    default:
      LOG_ERR(("!!! ERROR !!! In default case for msg_type: ", (u)(CPU_INT08U)msg_type));
      break;
  }

  do {
    encoded_byte = (len) % (MQTT_MSG_FIXED_HDR_REM_LEN_MAX_LEN);

    len = len / MQTT_MSG_FIXED_HDR_REM_LEN_MAX_LEN;

    if (len > 0) {
      encoded_byte = encoded_byte | MQTT_MSG_FIXED_HDR_REM_LEN_MAX_LEN;
    }
    p_cur_buf++;
    *p_cur_buf = encoded_byte;
  } while (len > 0);

  p_cur_buf++;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (p_cur_buf);
}

/****************************************************************************************************//**
 *                                               MQTTc_MsgID_Get()
 *
 * @brief    Obtain a msg ID to use for a message requiring one.
 *
 * @return   Message ID,          if NO error(s),
 *           MQTT_MSG_ID_INVALID, otherwise.
 *
 * @note         (1) Once the message has been completed, MQTTc_MsgID_Free() must be called to release the
 *               msg ID so that other messages can use it.
 *******************************************************************************************************/
static CPU_INT16U MQTTc_MsgID_Get(void)
{
  CPU_INT16U msg_id = MQTT_MSG_ID_INVALID;
  CPU_INT08U bitmap_ix;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  for (bitmap_ix = 0u; bitmap_ix < MQTTc_DataPtr->MsgID_BitmapTblMax; bitmap_ix++) {
    if (DEF_BIT_IS_CLR_ANY(MQTTc_DataPtr->MsgID_BitmapTbl[bitmap_ix], DEF_INT_32_MASK) == DEF_YES) {
      msg_id = DEF_INT_32_NBR_BITS - 1u - CPU_CntLeadZeros(~MQTTc_DataPtr->MsgID_BitmapTbl[bitmap_ix]);

      DEF_BIT_SET(MQTTc_DataPtr->MsgID_BitmapTbl[bitmap_ix], DEF_BIT(msg_id));
      msg_id += (DEF_INT_32_NBR_BITS * bitmap_ix) + 1u;
      break;
    }
  }
  CORE_EXIT_ATOMIC();

  return (msg_id);
}

/****************************************************************************************************//**
 *                                           MQTTc_MsgID_Free()
 *
 * @brief    Free message ID, allowing other messages to use it.
 *
 * @param    msg_id  Message ID to release.
 *******************************************************************************************************/
static void MQTTc_MsgID_Free(CPU_INT16U msg_id)
{
  CORE_DECLARE_IRQ_STATE;

  if (msg_id != MQTT_MSG_ID_NONE) {
    CORE_ENTER_ATOMIC();
    DEF_BIT_CLR(MQTTc_DataPtr->MsgID_BitmapTbl[(msg_id - 1u) / 32u], DEF_BIT((msg_id - 1u) % DEF_INT_32_NBR_BITS));
    CORE_EXIT_ATOMIC();
  }

  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_ConnNextMsgClr()
 *
 * @brief    Clear next message fields in MQTTc Connection object.
 *
 * @param    p_conn  Pointer to MQTTc Connection object for which to clear the NextMsg fields.
 *******************************************************************************************************/
static void MQTTc_ConnNextMsgClr(MQTTc_CONN *p_conn)
{
  p_conn->NextMsgHeader = DEF_BIT_NONE;                         // Clr NextMsg fields.
  p_conn->NextMsgRxLen = 0u;
  p_conn->NextMsgType = MQTTc_MSG_TYPE_NONE;
  p_conn->NextMsgLen = 0u;
  p_conn->NextMsgLenIsCmpl = DEF_NO;
  p_conn->NextMsgMsgID = MQTT_MSG_ID_NONE;
  p_conn->NextMsgMsgID_IsCmpl = DEF_NO;
  p_conn->NextMsgPtr = DEF_NULL;

  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_ConnCloseProc()
 *
 * @brief    Process the close operation for a given MQTTc Connection object.
 *
 * @param    p_conn  Pointer to MQTTc Connection object to close.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void MQTTc_ConnCloseProc(MQTTc_CONN *p_conn,
                                RTOS_ERR   *p_err)
{
  MQTTc_MSG *p_head_callback_msg = DEF_NULL;
  MQTTc_MSG *p_tail_callback_msg = DEF_NULL;
  MQTTc_MSG *p_iter_msg;
  MQTTc_MSG *p_next_iter_msg;
  MQTTc_MSG *p_prev_iter_msg = DEF_NULL;
  CORE_DECLARE_IRQ_STATE;

  MQTTc_ConnRemove(p_conn);

  MQTTc_SockConnClose(p_conn,
                      p_err);

  CORE_ENTER_ATOMIC();
  p_conn->SockId = NET_SOCK_ID_NONE;                            // Mark the conn as unusable.

  p_iter_msg = MQTTc_DataPtr->MsgListHeadPtr;
  while (p_iter_msg != DEF_NULL) {                              // Iterate in list of posted msg.
    p_next_iter_msg = p_iter_msg->NextPtr;

    if (p_iter_msg->ConnPtr == p_conn) {                        // See if msg was posted on same conn that is closing.
      if (p_head_callback_msg == DEF_NULL) {                    // Append msg at list of msg to free.
        p_head_callback_msg = p_iter_msg;
        p_tail_callback_msg = p_iter_msg;
      } else {
        p_tail_callback_msg->NextPtr = p_iter_msg;
        p_tail_callback_msg = p_iter_msg;
      }

      if (p_prev_iter_msg != DEF_NULL) {                        // Make sure prev msg is point at correct next msg.
        p_prev_iter_msg->NextPtr = p_next_iter_msg;
      } else {
        MQTTc_DataPtr->MsgListHeadPtr = p_next_iter_msg;
      }
    } else {
      p_prev_iter_msg = p_iter_msg;
    }

    p_iter_msg = p_next_iter_msg;
  }
  CORE_EXIT_ATOMIC();

  MQTTc_MsgListClosedCallbackExec(p_conn->TxMsgHeadPtr);        // Exec callbacks for msgs q'd under this conn.

  //                                                               Exec callback, in order, for each msg that had ...
  MQTTc_MsgListClosedCallbackExec(p_head_callback_msg);         // been posted but not processed, for that conn.
}

/****************************************************************************************************//**
 *                                           MQTTc_ConnRemove()
 *
 * @brief    Remove MQTTc Connection object from global connection list.
 *
 * @param    p_conn  Pointer to MQTTc Connection object to remove from list.
 *******************************************************************************************************/
static void MQTTc_ConnRemove(MQTTc_CONN *p_conn)
{
  MQTTc_SockSelDescClr(p_conn, MQTTc_SEL_DESC_TYPE_RD);
  MQTTc_SockSelDescClr(p_conn, MQTTc_SEL_DESC_TYPE_WR);
  MQTTc_SockSelDescClr(p_conn, MQTTc_SEL_DESC_TYPE_ERR);

  if (MQTTc_DataPtr->ConnHeadPtr == p_conn) {                       // If conn is located at head of list.
    MQTTc_DataPtr->ConnHeadPtr = p_conn->NextPtr;
  } else {
    MQTTc_CONN *p_iter_conn = MQTTc_DataPtr->ConnHeadPtr;

    //                                                             Loop to find good conn.
    while ((p_iter_conn->NextPtr != p_conn)
           && (p_iter_conn->NextPtr != DEF_NULL)) {
      p_iter_conn = p_iter_conn->NextPtr;
    }
    if (p_iter_conn->NextPtr != DEF_NULL) {
      p_iter_conn->NextPtr = p_conn->NextPtr;
    } else {
      LOG_ERR(("!!! ERROR !!! Could not find conn in conn list."));
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_MQTT_CLIENT_AVAIL
