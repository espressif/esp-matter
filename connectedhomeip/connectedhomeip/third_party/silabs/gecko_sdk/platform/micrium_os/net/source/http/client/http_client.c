/***************************************************************************//**
 * @file
 * @brief Network - HTTP Client
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

#if (defined(RTOS_MODULE_NET_HTTP_CLIENT_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error HTTP Client Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  "http_client_priv.h"

#ifdef  HTTPc_WEBSOCK_MODULE_EN
#include  "http_client_websock_priv.h"
#endif

#include  <net/include/http_client.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_util.h>

#include  <common/include/lib_ascii.h>
#include  <common/include/lib_str.h>
#include  <common/include/rtos_prio.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <rtos_err_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

#define  HTTPc_BUF_LEN_MIN                  256u

#define  HTTPc_TASK_MSG_Q_NAME             "HTTPc Msg Q"
#define  HTTPc_TASK_NAME                   "HTTPc Task"         // Task Name.

/********************************************************************************************************
 *                                           DEFAULT CONFIGURATION
 *******************************************************************************************************/

#define  HTTP_CLIENT_CFG_CONN_CONNECT_TIMEOUT_MS_DFLT           2000u
#define  HTTP_CLIENT_CFG_CONN_INACTIVITY_TIMEOUT_SEC_DFLT       30u
#define  HTTP_CLIENT_CFG_MSG_Q_SIZE_DFLT                        5u
#define  HTTP_CLIENT_CFG_TASK_DLY_MS_DFLT                       1u

#define  HTTP_CLIENT_TASK_CFG_STK_SIZE_ELEMENTS_DFLT            768u
#define  HTTP_CLIENT_TASK_CFG_STK_PTR_DFLT                      DEF_NULL

#ifdef  HTTPc_TASK_MODULE_EN
#define  HTTPc_INIT_CFG_DFLT             {                                        \
    .TaskDly_ms = HTTP_CLIENT_CFG_TASK_DLY_MS_DFLT,                               \
    .ConnCfg =                                                                    \
    {                                                                             \
      .ConnConnectTimeout_ms = HTTP_CLIENT_CFG_CONN_CONNECT_TIMEOUT_MS_DFLT,      \
      .ConnInactivityTimeout_s = HTTP_CLIENT_CFG_CONN_INACTIVITY_TIMEOUT_SEC_DFLT \
    },                                                                            \
    .QtyCfg =                                                                     \
    {                                                                             \
      .MsgQ_Size = HTTP_CLIENT_CFG_MSG_Q_SIZE_DFLT                                \
    },                                                                            \
    .StkSizeElements = HTTP_CLIENT_TASK_CFG_STK_SIZE_ELEMENTS_DFLT,               \
    .StkPtr = HTTP_CLIENT_TASK_CFG_STK_PTR_DFLT,                                  \
    .MemSegPtr = DEF_NULL                                                         \
}
#else
#define  HTTPc_INIT_CFG_DFLT             {                                        \
    .TaskDly_ms = HTTP_CLIENT_CFG_TASK_DLY_MS_DFLT,                               \
    .ConnCfg =                                                                    \
    {                                                                             \
      .ConnConnectTimeout_ms = HTTP_CLIENT_CFG_CONN_CONNECT_TIMEOUT_MS_DFLT,      \
      .ConnInactivityTimeout_s = HTTP_CLIENT_CFG_CONN_INACTIVITY_TIMEOUT_SEC_DFLT \
    },                                                                            \
    .QtyCfg =                                                                     \
    {                                                                             \
      .MsgQ_Size = HTTP_CLIENT_CFG_MSG_Q_SIZE_DFLT                                \
    },                                                                            \
    .MemSegPtr = DEF_NULL                                                         \
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

HTTPc_DATA *HTTPc_DataPtr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN HTTPc_InitDone = DEF_NO;

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const HTTPc_INIT_CFG  HTTPc_InitCfgDflt = HTTPc_INIT_CFG_DFLT;
static HTTPc_INIT_CFG HTTPc_InitCfg = HTTPc_INIT_CFG_DFLT;
#else
extern const HTTPc_INIT_CFG HTTPc_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if ((HTTPc_CFG_PERSISTENT_EN == DEF_ENABLED) \
  || defined(HTTPc_SIGNAL_TASK_MODULE_EN)       )
static void HTTPc_ConnCloseHandler(HTTPc_CONN  *p_conn,
                                   HTTPc_FLAGS flags,
                                   RTOS_ERR    *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         HTTPc_ConfigureTaskStk()
 *
 * @brief    Configure the HTTP client task stack properties to use the parameters contained in
 *           the passed structure instead of the default parameters.
 *
 * @param    stk_size_elements   Size of the stack, in CPU_STK elements.
 *
 * @param    p_stk_base          Pointer to base of the stack.
 *
 * @note     (1) This function is optional. If it is called, it must be called before HTTPc_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (defined(HTTPc_TASK_MODULE_EN)))
void HTTPc_ConfigureTaskStk(CPU_STK_SIZE stk_size_elements,
                            void         *p_stk_base)
{
  RTOS_ASSERT_DBG((HTTPc_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  HTTPc_InitCfg.StkSizeElements = stk_size_elements;
  HTTPc_InitCfg.StkPtr = p_stk_base;
}
#endif

/****************************************************************************************************//**
 *                                         HTTPc_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by HTTP
 *           client module instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *
 * @note     (1) This function is optional. If it is called, it must be called before HTTPc_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void HTTPc_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_DBG((HTTPc_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  HTTPc_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                        HTTPc_ConfigureQty()
 *
 * @brief    Overwrite the  quantity configuration object for HTTP client.
 *
 * @param    p_qty_cfg   Pointer to a structure containing the advanced task parameters.
 *******************************************************************************************************/
#if ((RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (defined(HTTPc_TASK_MODULE_EN)))
void HTTPc_ConfigureQty(HTTPc_QTY_CFG *p_qty_cfg)
{
  RTOS_ASSERT_DBG((HTTPc_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );
  RTOS_ASSERT_DBG((p_qty_cfg->MsgQ_Size > 0), RTOS_ERR_INVALID_CFG,; );

  Mem_Copy(&HTTPc_InitCfg.QtyCfg, p_qty_cfg, sizeof(HTTPc_QTY_CFG));
}
#endif

/****************************************************************************************************//**
 *                                     HTTPc_ConfigureConnParam()
 *
 * @brief    Overwrite the Connection configuration object for HTTP client.
 *
 * @param    p_conn_cfg  Pointer to structure containing the connection parameters.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void HTTPc_ConfigureConnParam(HTTPc_CONN_CFG *p_conn_cfg)
{
  RTOS_ASSERT_DBG((HTTPc_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  HTTPc_InitCfg.ConnCfg = *p_conn_cfg;
}
#endif

/****************************************************************************************************//**
 *                                        HTTPc_Init()
 *
 * @brief    - (1) Initializes the HTTP Client Suite by following these steps :
 *               - (a) Validate Configuration.
 *               - (b) Create HTTP Client Task, if necessary.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_ALREADY_INIT
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *******************************************************************************************************/
void HTTPc_Init(RTOS_ERR *p_err)
{
  CORE_DECLARE_IRQ_STATE;
  HTTPc_DATA *httpc_data;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone == DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_INIT);
    return;
  }
  CORE_EXIT_ATOMIC();

  //                                                               Allocate necessary objects on memory segment.
  httpc_data = (HTTPc_DATA *)Mem_SegAlloc("HTTP Client Module",
                                          HTTPc_InitCfg.MemSegPtr,
                                          sizeof(HTTPc_DATA),
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_DynPoolCreate("HTTPc KAL Sem Handle Pool",
                    &httpc_data->SemHandlePool,
                    HTTPc_InitCfg.MemSegPtr,
                    sizeof(KAL_SEM_HANDLE),
                    sizeof(CPU_SIZE_T),
                    3,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#ifdef HTTPc_TASK_MODULE_EN
  Mem_DynPoolCreate("HTTPc Msg Pool",
                    &httpc_data->MsgPool,
                    HTTPc_InitCfg.MemSegPtr,
                    sizeof(HTTPc_TASK_MSG),
                    sizeof(CPU_SIZE_T),
                    0u,
                    HTTPc_InitCfg.QtyCfg.MsgQ_Size,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#ifdef  HTTPc_WEBSOCK_MODULE_EN
  Mem_DynPoolCreate("HTTPc WebSocket Request Pool",
                    &httpc_data->WebSockReqPool,
                    HTTPc_InitCfg.MemSegPtr,
                    sizeof(HTTPc_WEBSOCK_REQ),
                    sizeof(CPU_SIZE_T),
                    0,
                    HTTPc_InitCfg.QtyCfg.MsgQ_Size,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif
#endif

  httpc_data->CfgConnConnectTimeout_ms = HTTPc_InitCfg.ConnCfg.ConnConnectTimeout_ms;
  httpc_data->CfgConnInactivityTimeout_s = HTTPc_InitCfg.ConnCfg.ConnInactivityTimeout_s;

#ifdef HTTPc_TASK_MODULE_EN
  httpc_data->TaskHandle = KAL_TaskHandleNull;
#endif

  CORE_ENTER_ATOMIC();
  HTTPc_DataPtr = httpc_data;
  CORE_EXIT_ATOMIC();

#ifdef HTTPc_TASK_MODULE_EN
  {
    KAL_TASK_HANDLE task_handle;

    HTTPcTask_SetDly(HTTPc_InitCfg.TaskDly_ms);

    //                                                             HTTPc Task creation.
    httpc_data->TaskConnFirstPtr = DEF_NULL;

    httpc_data->TaskMsgQ_Handle = KAL_QCreate(HTTPc_TASK_MSG_Q_NAME,
                                              HTTPc_InitCfg.QtyCfg.MsgQ_Size,
                                              DEF_NULL,
                                              p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    //                                                             ------- ALLOCATE MEMORY SPACE FOR HTTPc TASK  ------
    task_handle = KAL_TaskAlloc((const  CPU_CHAR *)HTTPc_TASK_NAME,
                                (CPU_STK *)HTTPc_InitCfg.StkPtr,
                                HTTPc_InitCfg.StkSizeElements,
                                DEF_NULL,
                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    CORE_ENTER_ATOMIC();
    HTTPc_DataPtr->TaskHandle = task_handle;
    CORE_EXIT_ATOMIC();

    //                                                             ---------------- CREATE HTTPc TASK -----------------
    KAL_TaskCreate(httpc_data->TaskHandle,
                   HTTPcTask,
                   DEF_NULL,
                   HTTP_CLIENT_TASK_PRIO_DFLT,
                   DEF_NULL,
                   p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
#endif

  CORE_ENTER_ATOMIC();
  HTTPc_InitDone = DEF_YES;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                         HTTPc_TaskPrioSet()
 *
 * @brief    Sets priority of the HTTP client task.
 *
 * @param    prio    New priority for the HTTP client task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/

#ifdef  HTTPc_TASK_MODULE_EN
void HTTPc_TaskPrioSet(RTOS_TASK_PRIO prio,
                       RTOS_ERR       *p_err)
{
  KAL_TASK_HANDLE task_handle;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((HTTPc_DataPtr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  task_handle = HTTPc_DataPtr->TaskHandle;
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(task_handle,
                  prio,
                  p_err);
}
#endif

/****************************************************************************************************//**
 *                                         HTTPc_TaskDlySet()
 *
 * @brief    Sets delay of the HTTP client task.
 *
 * @param    dly_ms  New delay in millisecond for the HTTP client task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/

#ifdef  HTTPc_TASK_MODULE_EN
void HTTPc_TaskDlySet(CPU_INT08U dly_ms,
                      RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((HTTPc_DataPtr != DEF_NULL), RTOS_ERR_NOT_INIT,; );
  HTTPc_DataPtr->TaskDly_ms = dly_ms;
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                           HTTPc_ConnClr()
 *
 * @brief    Clears an HTTP client connection object before the first usage.
 *
 * @param    p_conn_obj  Pointer to the current HTTPc Connection object.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_INIT
 *
 * @note     (1) This function MUST be called before the HTTPc_CONN object is used for the first time.
 *******************************************************************************************************/
void HTTPc_ConnClr(HTTPc_CONN_OBJ *p_conn_obj,
                   RTOS_ERR       *p_err)
{
  HTTPc_CONN *p_conn;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_DECLARE_IRQ_STATE;
#endif

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    return;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_conn = (HTTPc_CONN *)p_conn_obj;

  //                                                               ------------ INIT CONNECTION PARAMETERS ------------
  Mem_Clr(&p_conn->ServerSockAddr, sizeof (NET_SOCK_ADDR));
  p_conn->SockID = NET_SOCK_ID_NONE;
  p_conn->SockFlags = DEF_BIT_NONE;
  p_conn->ConnectTimeout_ms = HTTPc_DataPtr->CfgConnConnectTimeout_ms;
  p_conn->InactivityTimeout_s = HTTPc_DataPtr->CfgConnInactivityTimeout_s;
  p_conn->ServerPort = HTTP_DFLT_PORT_NBR;
#ifdef NET_SECURE_MODULE_EN
  p_conn->SockSecureCfgPtr = DEF_NULL;
#endif
  p_conn->HostNamePtr = DEF_NULL;
  p_conn->HostNameLen = 0;
  p_conn->State = HTTPc_CONN_STATE_NONE;
  p_conn->Flags = DEF_BIT_NONE;
  RTOS_ERR_SET(p_conn->ErrCode, RTOS_ERR_NONE);
  p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_NONE;
#ifdef HTTPc_TASK_MODULE_EN
  p_conn->OnConnect = DEF_NULL;
  p_conn->OnClose = DEF_NULL;
#endif

  //                                                               ------------- INIT REQUEST PARAMETERS --------------
  p_conn->ReqListHeadPtr = DEF_NULL;
  p_conn->ReqListEndPtr = DEF_NULL;
  p_conn->ReqFlags = 0u;
#if (HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED)
  p_conn->ReqQueryStrTxIx = 0u;
  p_conn->ReqQueryStrTempPtr = DEF_NULL;
#endif
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
  p_conn->ReqHdrTxIx = 0u;
  p_conn->ReqHdrTempPtr = DEF_NULL;
#endif
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
  p_conn->ReqFormDataTxIx = 0u;
#endif
  p_conn->ReqDataOffset = 0u;

  //                                                               ------------- INIT RESPONSE PARAMETERS -------------
  p_conn->RespFlags = 0u;

  //                                                               --------- INIT CONNECTION BUFFER PARAMETERS --------
  p_conn->RxBufPtr = p_conn->BufPtr;
  p_conn->RxDataLenRem = 0u;
  p_conn->RxDataLen = 0u;
  p_conn->TxBufPtr = p_conn->BufPtr;
  p_conn->TxDataLen = 0u;
  p_conn->TxDataPtr = DEF_NULL;
  p_conn->BufPtr = DEF_NULL;
  p_conn->BufLen = 0;
  p_conn->NextPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                         HTTPc_ConnSetParam()
 *
 * @brief    Sets the parameters related to the HTTP Client Connection.
 *
 * @param    p_conn      Pointer to the current HTTPc Connection object.
 *
 * @param    type        Parameter type :
 *                           - HTTPc_PARAM_TYPE_SERVER_PORT
 *                           - HTTPc_PARAM_TYPE_PERSISTENT
 *                           - HTTPc_PARAM_TYPE_CONNECT_TIMEOUT
 *                           - HTTPc_PARAM_TYPE_INACTIVITY_TIMEOUT
 *                           - HTTPc_PARAM_TYPE_SECURE_COMMON_NAME
 *                           - HTTPc_PARAM_TYPE_SECURE_TRUST_CALLBACK
 *                           - HTTPc_PARAM_TYPE_CONN_CONNECT_CALLBACK
 *                           - HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK
 *
 * @param    p_param     Pointer to parameter.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_INIT
 *                           - RTOS_ERR_OWNERSHIP
 *******************************************************************************************************/
void HTTPc_ConnSetParam(HTTPc_CONN_OBJ   *p_conn_obj,
                        HTTPc_PARAM_TYPE type,
                        void             *p_param,
                        RTOS_ERR         *p_err)
{
  HTTPc_CONN  *p_conn;
  CPU_BOOLEAN in_use;
#if (HTTPc_CFG_PERSISTENT_EN == DEF_ENABLED)
  CPU_BOOLEAN persistent;
#endif
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_param != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    goto exit;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_conn = (HTTPc_CONN *)p_conn_obj;

  //                                                               ------ VALIDATE THAT CONN IS NOT USED ALREADY ------
  CORE_ENTER_ATOMIC();
  in_use = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_IN_USE);
  CORE_EXIT_ATOMIC();
  if (in_use == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OWNERSHIP);
    goto exit;
  }

  switch (type) {
    case HTTPc_PARAM_TYPE_SERVER_PORT:
      p_conn->ServerPort = *(NET_PORT_NBR *)p_param;
      break;

#if (HTTPc_CFG_PERSISTENT_EN == DEF_ENABLED)
    case HTTPc_PARAM_TYPE_PERSISTENT:
      persistent = *(CPU_BOOLEAN *)p_param;
      if (persistent == DEF_YES) {
        DEF_BIT_SET(p_conn->Flags, HTTPc_FLAG_CONN_PERSISTENT);
      }
      break;
#endif

    case HTTPc_PARAM_TYPE_CONNECT_TIMEOUT:
      p_conn->ConnectTimeout_ms = *(CPU_INT16U *)p_param;
      break;

    case HTTPc_PARAM_TYPE_INACTIVITY_TIMEOUT:
      p_conn->InactivityTimeout_s = *(CPU_INT16U *)p_param;
      break;

#ifdef NET_SECURE_MODULE_EN
    case HTTPc_PARAM_TYPE_SECURE_CFG_PTR:
      p_conn->SockSecureCfgPtr = (NET_APP_SOCK_SECURE_CFG *)p_param;
      break;
#endif

#ifdef HTTPc_TASK_MODULE_EN
    case HTTPc_PARAM_TYPE_CONN_CONNECT_CALLBACK:
      p_conn->OnConnect = (HTTPc_CONNECT_CALLBACK)p_param;
      break;
#endif

#ifdef HTTPc_TASK_MODULE_EN
    case HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK:
      p_conn->OnClose = (HTTPc_CONN_CLOSE_CALLBACK)p_param;
      break;
#endif

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_TYPE,; );
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                            HTTPc_ConnOpen()
 *
 * @brief    Opens a new HTTP connection.
 *
 * @param    p_conn_obj          Pointer to HTTPc Connection object to open.
 *
 * @param    p_buf               Pointer to the HTTP buffer that sends (Tx) and receives (Rx) data.
 *
 * @param    buf_len             Length of the HTTP buffer.
 *
 * @param    p_hostname_str      Pointer to the hostname string.
 *
 * @param    hostname_str_len    Length of the hostname string.
 *
 * @param    flags               Configuration flags :
 *                               HTTPc_FLAG_CONN_NO_BLOCK
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NOT_INIT
 *                                   - RTOS_ERR_OWNERSHIP
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                                   - RTOS_ERR_NO_MORE_RSRC
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_INVALID_TYPE
 *                                   - RTOS_ERR_NOT_SUPPORTED
 *                                   - RTOS_ERR_FAIL
 *                                   - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                                   - RTOS_ERR_NET_IF_LINK_DOWN
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_NET_OP_IN_PROGRESS
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_ALREADY_EXISTS
 *                                   - RTOS_ERR_NOT_FOUND
 *                                   - RTOS_ERR_NET_INVALID_CONN
 *                                   - RTOS_ERR_RX
 *                                   - RTOS_ERR_NOT_READY
 *                                   - RTOS_ERR_NET_ADDR_UNRESOLVED
 *                                   - RTOS_ERR_NET_NEXT_HOP
 *                                   - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *
 * @return   DEF_OK,   if the connection opening has successfully completed.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPc_ConnOpen(HTTPc_CONN_OBJ *p_conn_obj,
                           CPU_CHAR       *p_buf,
                           CPU_INT16U     buf_len,
                           CPU_CHAR       *p_hostname_str,
                           CPU_INT16U     hostname_str_len,
                           HTTPc_FLAGS    flags,
                           RTOS_ERR       *p_err)
{
  HTTPc_CONN  *p_conn;
  CPU_BOOLEAN in_use;
  CPU_BOOLEAN no_block;
  CPU_BOOLEAN result = DEF_FAIL;
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
  RTOS_ERR local_err;
#endif
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, result);
  RTOS_ASSERT_DBG_ERR_SET((p_conn_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, result);
  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, result);
  RTOS_ASSERT_DBG_ERR_SET((p_hostname_str != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, result);
  RTOS_ASSERT_DBG_ERR_SET((buf_len >= HTTPc_BUF_LEN_MIN), *p_err, RTOS_ERR_INVALID_CFG, result);
  RTOS_ASSERT_DBG_ERR_SET((hostname_str_len > 0), *p_err, RTOS_ERR_INVALID_CFG, result);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    goto exit;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_conn = (HTTPc_CONN *)p_conn_obj;
  //                                                               ------ VALIDATE THAT CONN IS NOT USED ALREADY ------
  CORE_ENTER_ATOMIC();
  in_use = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_IN_USE);
  CORE_EXIT_ATOMIC();
  if (in_use == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OWNERSHIP);
    goto exit;
  }

  //                                                               -------------- SET BUFFER PARAMETERS ---------------
  p_conn->BufPtr = p_buf;
  p_conn->TxBufPtr = p_buf;
  p_conn->RxBufPtr = p_buf;
  p_conn->BufLen = buf_len;
  p_conn->NextPtr = DEF_NULL;

  //                                                               ------------- SET HOSTNAME PARAMETERS --------------
  p_conn->HostNamePtr = p_hostname_str;
  p_conn->HostNameLen = hostname_str_len;

  //                                                               ---------------- SET BLOCKING MODE -----------------
  no_block = DEF_BIT_IS_SET(flags, HTTPc_FLAG_CONN_NO_BLOCK);
  if (no_block == DEF_YES) {
    DEF_BIT_SET(p_conn->Flags, HTTPc_FLAG_CONN_NO_BLOCK);
  } else {
    DEF_BIT_CLR(p_conn->Flags, (HTTPc_FLAGS)HTTPc_FLAG_CONN_NO_BLOCK);
  }

#ifdef HTTPc_TASK_MODULE_EN

  if (no_block == DEF_NO) {
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
    HTTPcTask_ConnConnectSignalCreate(p_conn, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
  }

  HTTPcTask_MsgQueue(HTTPc_MSG_TYPE_CONN_OPEN,
                     p_conn,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  HTTPcTask_Wake(p_conn, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_FAIL);

  if (no_block == DEF_NO) {
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
    HTTPcTask_ConnConnectSignalWait(p_conn, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      HTTPc_ConnCloseHandler(p_conn, HTTPc_FLAG_NONE, &local_err);
      goto exit;
    }

    HTTPcTask_ConnConnectSignalDel(p_conn);

    CORE_ENTER_ATOMIC();
    result = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_CONNECT);
    CORE_EXIT_ATOMIC();
    if (result == DEF_FAIL) {
      *p_err = p_conn->ErrCode;
      goto exit;
    }

#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
  }

#else

  RTOS_ASSERT_DBG_ERR_SET((no_block != DEF_YES), *p_err, RTOS_ERR_NOT_AVAIL, DEF_FAIL);

  HTTPcConn_Add(p_conn);

  while ((p_conn->State != HTTPc_CONN_STATE_PARAM_VALIDATE)
         && (p_conn->State != HTTPc_CONN_STATE_NONE)) {
    HTTPcConn_Process(p_conn);
  }

  *p_err = p_conn->ErrCode;
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  result = DEF_OK;

#endif

exit:
  return (result);
}

/****************************************************************************************************//**
 *                                          HTTPc_ConnClose()
 *
 * @brief    Closes a persistent HTTP client connection.
 *
 * @param    p_conn_obj  Pointer to HTTPc Connection to close.
 *
 * @param    flags       Configuration flags :
 *                           - HTTPc_FLAG_CONN_NO_BLOCK
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_INIT
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_NO_MORE_RSRC
 *                           - RTOS_ERR_INVALID_TYPE
 *                           - RTOS_ERR_NET_RETRY_MAX
 *                           - RTOS_ERR_NET_SOCK_CLOSED
 *                           - RTOS_ERR_NOT_SUPPORTED
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_NET_IF_LINK_DOWN
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_NET_INVALID_CONN
 *                           - RTOS_ERR_RX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_NET_NEXT_HOP
 *******************************************************************************************************/
void HTTPc_ConnClose(HTTPc_CONN_OBJ *p_conn_obj,
                     HTTPc_FLAGS    flags,
                     RTOS_ERR       *p_err)
{
#if (HTTPc_CFG_PERSISTENT_EN == DEF_ENABLED)
  HTTPc_CONN *p_conn;
#endif
#if (HTTPc_CFG_PERSISTENT_EN == DEF_ENABLED \
     && RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_DECLARE_IRQ_STATE;
#endif

#if (HTTPc_CFG_PERSISTENT_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(flags);
#endif

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (HTTPc_CFG_PERSISTENT_EN == DEF_ENABLED)

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    goto exit;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_conn = (HTTPc_CONN *)p_conn_obj;

  HTTPc_ConnCloseHandler(p_conn, flags, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

#else
  PP_UNUSED_PARAM(p_conn_obj);

  RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif

#if (HTTPc_CFG_PERSISTENT_EN == DEF_ENABLED)
exit:
  return;
#endif
}

/****************************************************************************************************//**
 *                                            HTTPc_ReqClr()
 *
 * @brief    Clears the Request object members.
 *
 * @param    p_req_obj   Pointer to request object to clear.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_INIT
 *******************************************************************************************************/
void HTTPc_ReqClr(HTTPc_REQ_OBJ *p_req_obj,
                  RTOS_ERR      *p_err)
{
  HTTPc_REQ *p_req;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_DECLARE_IRQ_STATE;
#endif

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_req_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    return;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_req = (HTTPc_REQ *)p_req_obj;

  p_req->Flags = DEF_BIT_NONE;
  p_req->HdrFlags = DEF_BIT_NONE;
  p_req->Method = HTTP_METHOD_UNKNOWN;
  p_req->ResourcePathPtr = DEF_NULL;
  p_req->ResourcePathLen = 0;
  p_req->ContentType = HTTP_CONTENT_TYPE_UNKNOWN;
  p_req->ContentLen = 0u;
#if (HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED)
  p_req->QueryStrTbl = DEF_NULL;
  p_req->QueryStrNbr = 0u;
  p_req->OnQueryStrTx = DEF_NULL;
#endif
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
  p_req->HdrTbl = DEF_NULL;
  p_req->HdrNbr = 0;
  p_req->OnHdrTx = DEF_NULL;
#endif
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
  p_req->FormFieldTbl = DEF_NULL;
  p_req->FormFieldNbr = 0;
#endif
  p_req->DataPtr = DEF_NULL;
#if (HTTPc_CFG_CHUNK_TX_EN == DEF_ENABLED)
  p_req->OnBodyTx = DEF_NULL;
#endif

#if (HTTPc_CFG_HDR_RX_EN == DEF_ENABLED)
  p_req->OnHdrRx = DEF_NULL;
#endif
  p_req->OnBodyRx = DEF_NULL;

#ifdef HTTPc_TASK_MODULE_EN
  p_req->OnTransComplete = DEF_NULL;
  p_req->OnErr = DEF_NULL;
#endif

  p_req->ConnPtr = DEF_NULL;
  p_req->RespPtr = DEF_NULL;

  p_req->NextPtr = DEF_NULL;

  p_req->WebSockPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                          HTTPc_ReqSetParam()
 *
 * @brief    Sets a parameter related to a given HTTP Request.
 *
 * @param    p_req_obj   Pointer to request object.
 *
 * @param    type        Parameter type :
 *                           - HTTPc_PARAM_TYPE_REQ_QUERY_STR_TBL
 *                           - HTTPc_PARAM_TYPE_REQ_QUERY_STR_HOOK
 *                           - HTTPc_PARAM_TYPE_REQ_HDR_TBL
 *                           - HTTPc_PARAM_TYPE_REQ_HDR_HOOK
 *                           - HTTPc_PARAM_TYPE_REQ_FORM_TBL
 *                           - HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_TYPE
 *                           - HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_LEN
 *                           - HTTPc_PARAM_TYPE_REQ_BODY_CHUNK
 *                           - HTTPc_PARAM_TYPE_REQ_BODY_HOOK
 *                           - HTTPc_PARAM_TYPE_RESP_HDR_HOOK
 *                           - HTTPc_PARAM_TYPE_RESP_BODY_HOOK
 *                           - HTTPc_PARAM_TYPE_TRANS_COMPLETE_CALLBACK
 *                           - HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK
 *
 * @param    p_param     Pointer to parameter.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_INIT
 *                           - RTOS_ERR_OWNERSHIP
 *******************************************************************************************************/
void HTTPc_ReqSetParam(HTTPc_REQ_OBJ    *p_req_obj,
                       HTTPc_PARAM_TYPE type,
                       void             *p_param,
                       RTOS_ERR         *p_err)
{
  HTTPc_REQ       *p_req;
  HTTPc_PARAM_TBL *p_tbl_obj = DEF_NULL;
  CPU_BOOLEAN     in_use;
#if (HTTPc_CFG_CHUNK_TX_EN == DEF_ENABLED)
  CPU_BOOLEAN chunk_en;
#endif
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_req_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_param != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    goto exit;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_req = (HTTPc_REQ *)p_req_obj;

  //                                                               ------ VALIDATE THAT REQ IS NOT USED ALREADY -------
  CORE_ENTER_ATOMIC();
  in_use = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_IN_USE);
  CORE_EXIT_ATOMIC();
  if (in_use == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OWNERSHIP);
    goto exit;
  }

  //                                                               ------------- REQUEST PARAMETER SETUP --------------
  switch (type) {
#if (HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED)
    case HTTPc_PARAM_TYPE_REQ_QUERY_STR_TBL:
      p_tbl_obj = (HTTPc_PARAM_TBL *)p_param;
      p_req->QueryStrTbl = (HTTPc_KEY_VAL *)p_tbl_obj->TblPtr;
      p_req->QueryStrNbr = p_tbl_obj->EntryNbr;
      p_req->OnQueryStrTx = DEF_NULL;
      break;
#endif

#if (HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED)
    case HTTPc_PARAM_TYPE_REQ_QUERY_STR_HOOK:
      p_req->OnQueryStrTx = (HTTPc_REQ_QUERY_STR_HOOK)p_param;
      p_req->QueryStrTbl = DEF_NULL;
      p_req->QueryStrNbr = 0;
      break;
#endif

#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
    case HTTPc_PARAM_TYPE_REQ_HDR_TBL:
      p_tbl_obj = (HTTPc_PARAM_TBL *)p_param;
      p_req->HdrNbr = p_tbl_obj->EntryNbr;
      p_req->HdrTbl = (HTTPc_HDR *)p_tbl_obj->TblPtr;
      p_req->OnHdrTx = DEF_NULL;
      break;
#endif

#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
    case HTTPc_PARAM_TYPE_REQ_HDR_HOOK:
      p_req->OnHdrTx = (HTTPc_REQ_HDR_HOOK)p_param;
      p_req->HdrNbr = 0;
      p_req->HdrTbl = DEF_NULL;
      break;
#endif

#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
    case HTTPc_PARAM_TYPE_REQ_FORM_TBL:
      p_tbl_obj = (HTTPc_PARAM_TBL *)p_param;
      DEF_BIT_SET(p_req->Flags, HTTPc_FLAG_REQ_BODY_PRESENT);
      DEF_BIT_SET(p_req->Flags, HTTPc_FLAG_REQ_FORM_PRESENT);
      p_req->FormFieldNbr = p_tbl_obj->EntryNbr;
      p_req->FormFieldTbl = (HTTPc_FORM_TBL_FIELD *)p_tbl_obj->TblPtr;
      break;
#endif

    case HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_TYPE:
      DEF_BIT_SET(p_req->Flags, HTTPc_FLAG_REQ_BODY_PRESENT);
      p_req->ContentType = *(HTTP_CONTENT_TYPE *)p_param;
      break;

    case HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_LEN:
      DEF_BIT_SET(p_req->Flags, HTTPc_FLAG_REQ_BODY_PRESENT);
      p_req->ContentLen = *(CPU_INT32U *)p_param;
      break;

#if (HTTPc_CFG_CHUNK_TX_EN == DEF_ENABLED)
    case HTTPc_PARAM_TYPE_REQ_BODY_CHUNK:
      chunk_en = *(CPU_BOOLEAN *)p_param;
      if (chunk_en == DEF_YES) {
        DEF_BIT_SET(p_req->Flags, HTTPc_FLAG_REQ_BODY_CHUNK_TRANSFER);
      }
      DEF_BIT_SET(p_req->Flags, HTTPc_FLAG_REQ_BODY_PRESENT);
      break;
#endif

    case HTTPc_PARAM_TYPE_REQ_BODY_HOOK:
      p_req->OnBodyTx = (HTTPc_REQ_BODY_HOOK)p_param;
      break;

#if (HTTPc_CFG_HDR_RX_EN == DEF_ENABLED)
    case HTTPc_PARAM_TYPE_RESP_HDR_HOOK:
      p_req->OnHdrRx = (HTTPc_RESP_HDR_HOOK)p_param;
      break;
#endif

    case HTTPc_PARAM_TYPE_RESP_BODY_HOOK:
      p_req->OnBodyRx = (HTTPc_RESP_BODY_HOOK)p_param;
      break;

#ifdef HTTPc_TASK_MODULE_EN
    case HTTPc_PARAM_TYPE_TRANS_COMPLETE_CALLBACK:
      p_req->OnTransComplete = (HTTPc_COMPLETE_CALLBACK)p_param;
      break;
#endif

#ifdef HTTPc_TASK_MODULE_EN
    case HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK:
      p_req->OnErr = (HTTPc_TRANS_ERR_CALLBACK)p_param;
      break;
#endif

#ifdef HTTPc_WEBSOCK_MODULE_EN
    case HTTPc_PARAM_TYPE_REQ_UPGRADE_WEBSOCKET:
      DEF_BIT_SET(p_req->Flags, HTTPc_FLAG_REQ_UPGRADE_WEBSOCKET);
      break;
#endif

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_TYPE,; );
  }

  PP_UNUSED_PARAM(p_tbl_obj);

exit:
  return;
}

/****************************************************************************************************//**
 *                                            HTTPc_ReqSend()
 *
 * @brief    Sends a HTTP request.
 *
 * @param    p_conn_obj          Pointer to valid HTTPc Connection on which request will be sent.
 *
 * @param    p_req_obj           Pointer to request to send.
 *
 * @param    p_resp_obj          Pointer to response object that will be filled with the received
 *                               response.
 *
 * @param    method              HTTP method of the request.
 *
 * @param    p_resource_path     Pointer to complete URI (or only resource path) of the request.
 *
 * @param    resource_path_len   Resource path length.
 *
 * @param    flags               Configuration flags :
 *                                   - HTTPc_FLAG_REQ_NO_BLOCK
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NOT_INIT
 *                                   - RTOS_ERR_OWNERSHIP
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_OVF
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_NO_MORE_RSRC
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_TIMEOUT
 *
 * @return   DEF_YES, if HTTP Response received successfully.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPc_ReqSend(HTTPc_CONN_OBJ *p_conn_obj,
                          HTTPc_REQ_OBJ  *p_req_obj,
                          HTTPc_RESP_OBJ *p_resp_obj,
                          HTTP_METHOD    method,
                          CPU_CHAR       *p_resource_path,
                          CPU_INT16U     resource_path_len,
                          HTTPc_FLAGS    flags,
                          RTOS_ERR       *p_err)
{
  HTTPc_CONN  *p_conn;
  HTTPc_REQ   *p_req;
  CPU_BOOLEAN in_use;
  CPU_BOOLEAN no_block;
  CPU_BOOLEAN result = DEF_FAIL;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_conn_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_req_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_resp_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_resource_path != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    goto exit;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_conn = (HTTPc_CONN *)p_conn_obj;
  p_req = (HTTPc_REQ *)p_req_obj;

  //                                                               ------ VALIDATE THAT REQ IS NOT USED ALREADY -------
  CORE_ENTER_ATOMIC();
  in_use = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_IN_USE);
  CORE_EXIT_ATOMIC();
  if (in_use == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OWNERSHIP);
    goto exit;
  }

  p_req->ConnPtr = p_conn;
  p_req->RespPtr = p_resp_obj;

  //                                                               ----------------- SET HTTP METHOD ------------------
  switch (method) {
    case HTTP_METHOD_GET:
    case HTTP_METHOD_POST:
    case HTTP_METHOD_HEAD:
    case HTTP_METHOD_PUT:
    case HTTP_METHOD_DELETE:
    case HTTP_METHOD_TRACE:
    case HTTP_METHOD_CONNECT:
    case HTTP_METHOD_OPTIONS:
      p_req->Method = method;
      break;

    case HTTP_METHOD_UNKNOWN:
    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_SUPPORTED, DEF_FAIL);
  }

  //                                                               ---------------- SET RESOURCE PATH -----------------
  p_req->ResourcePathPtr = p_resource_path;
  p_req->ResourcePathLen = resource_path_len;

  //                                                               ---------------- SET BLOCKING MODE -----------------
  no_block = DEF_BIT_IS_SET(flags, HTTPc_FLAG_REQ_NO_BLOCK);
  if (no_block == DEF_YES) {
    DEF_BIT_SET(p_req->Flags, HTTPc_FLAG_REQ_NO_BLOCK);
  } else {
    DEF_BIT_CLR(p_req->Flags, (HTTPc_FLAGS)HTTPc_FLAG_REQ_NO_BLOCK);
  }

#ifdef HTTPc_TASK_MODULE_EN
  if (no_block == DEF_NO) {
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
    HTTPcTask_TransDoneSignalCreate(p_conn, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_conn_close;
    }
#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_FAIL);
#endif
  }

  HTTPcTask_MsgQueue(HTTPc_MSG_TYPE_REQ,
                     p_req,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  HTTPcTask_Wake(p_conn, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (no_block == DEF_NO) {
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
    //                                                             -------- WAIT FOR RESPONSE IN BLOCKING MODE --------
    HTTPcTask_TransDoneSignalWait(p_conn, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_conn_close;
    }

    CORE_ENTER_ATOMIC();
    result = DEF_BIT_IS_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_COMPLETE_OK);
    CORE_EXIT_ATOMIC();
    if (result == DEF_FAIL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(p_conn->ErrCode));
      goto exit;
    }

    HTTPcTask_TransDoneSignalDel(p_conn);

#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_FAIL);
#endif
  }

  goto exit;

#else
  RTOS_ASSERT_DBG_ERR_SET((no_block != DEF_YES), *p_err, RTOS_ERR_NOT_AVAIL, DEF_FAIL);

  HTTPcConn_ReqAdd(p_req);                                      // Add Request to Connection.

  do {
    HTTPcSock_Sel(p_conn, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
    HTTPcConn_TransProcess(p_conn);
  } while ((p_conn->State != HTTPc_CONN_STATE_PARAM_VALIDATE)
           && (p_conn->State != HTTPc_CONN_STATE_NONE));

  result = DEF_BIT_IS_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_COMPLETE_OK);

  RTOS_ERR_COPY(*p_err, p_conn->ErrCode);

  goto exit;

#endif

#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
exit_conn_close:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    HTTPc_ConnCloseHandler(p_conn, HTTPc_FLAG_NONE, &local_err);
  }
#endif

exit:
  return (result);
}

/****************************************************************************************************//**
 *                                         HTTPc_FormAppFmt()
 *
 * @brief    Formats an application type form based on an array containing the form fields.
 *
 * @param    p_buf           Pointer to buffer where the form will be written.
 *
 * @param    buf_len         Buffer length.
 *
 * @param    p_form_tbl      Pointer to form field's table.
 *
 * @param    form_tbl_size   Table size.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_TYPE
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_FAIL
 *
 * @return   Length of the formatted form, if no errors.
 *           0 , otherwise.
 *
 * @note     (1) To format the form, only standard Key-Value Pair objects are supported in the table.
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
CPU_INT32U HTTPc_FormAppFmt(CPU_CHAR             *p_buf,
                            CPU_INT16U           buf_len,
                            HTTPc_FORM_TBL_FIELD *p_form_tbl,
                            CPU_INT16U           form_tbl_size,
                            RTOS_ERR             *p_err)
{
  CPU_CHAR             *p_buf_wr;
  HTTPc_FORM_TBL_FIELD *p_tbl_field;
  HTTPc_KEY_VAL        *p_form_field;
  CPU_INT16U           key_char_encode_nbr;
  CPU_INT16U           val_char_encode_nbr;
  CPU_SIZE_T           str_len_key;
  CPU_SIZE_T           str_len_val;
  CPU_SIZE_T           data_size = 0;
  CPU_BOOLEAN          url_encode;
  CPU_INT16U           i;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_form_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG_ERR_SET((buf_len > 0u), *p_err, RTOS_ERR_INVALID_ARG, 0u);
  RTOS_ASSERT_DBG_ERR_SET((form_tbl_size > 0), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_buf_wr = p_buf;

  //                                                               ----------------- PARSE FORM TABLE -----------------
  for (i = 0u; i < form_tbl_size; i++) {
    p_tbl_field = &p_form_tbl[i];

    switch (p_tbl_field->Type) {
      case HTTPc_FORM_FIELD_TYPE_KEY_VAL:
        break;

      case HTTPc_FORM_FIELD_TYPE_KEY_VAL_EXT:
      case HTTPc_FORM_FIELD_TYPE_FILE:
      default:
        data_size = 0;
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
        goto exit;
    }

    p_form_field = (HTTPc_KEY_VAL *)p_tbl_field->FieldObjPtr;

    //                                                             Calculate length of key and value without encoding.
    str_len_key = p_form_field->KeyLen;
    str_len_val = p_form_field->ValLen;

    //                                                             Found number of character needing URL encoding.
    key_char_encode_nbr = HTTP_URL_CharEncodeNbr(p_form_field->KeyPtr, str_len_key);
    val_char_encode_nbr = HTTP_URL_CharEncodeNbr(p_form_field->ValPtr, str_len_val);

    //                                                             Calculate total size needed for key & value encoded.
    data_size += str_len_key
                 + str_len_val
                 + HTTP_URL_ENCODING_JUMP * key_char_encode_nbr
                 + HTTP_URL_ENCODING_JUMP * val_char_encode_nbr
                 + 1;                                           // + 1 for the "=" between key & value.

    if (i < (form_tbl_size - 1)) {
      data_size++;                                              // + 1 for the "&" between each key&value pair.
    }

    if (data_size > buf_len) {                                  // Return if no more space in buf.
      data_size = 0;
      RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
      goto exit;
    }

    url_encode = HTTP_URL_EncodeStr(p_form_field->KeyPtr,       // Encode and Write to buffer the Key.
                                    p_buf_wr,
                                    &str_len_key,
                                    buf_len);
    if (url_encode == DEF_FAIL) {
      data_size = 0;
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      goto exit;
    }

    p_buf_wr += str_len_key;
    buf_len -= str_len_key;

    *p_buf_wr = ASCII_CHAR_EQUALS_SIGN;                         // Write the "=" sign.
    p_buf_wr++;
    buf_len--;

    url_encode = HTTP_URL_EncodeStr(p_form_field->ValPtr,       // Encode and Write to buffer the Value.
                                    p_buf_wr,
                                    &str_len_val,
                                    buf_len);
    if (url_encode == DEF_FAIL) {
      data_size = 0;
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      goto exit;
    }

    p_buf_wr += str_len_val;
    buf_len -= str_len_val;

    if (i < (form_tbl_size - 1)) {
      *p_buf_wr = ASCII_CHAR_AMPERSAND;                         // Write the "&" sign between pairs.
      p_buf_wr++;
      buf_len--;
    }

    p_form_field++;
  }

exit:
  return (data_size);
}
#endif

/****************************************************************************************************//**
 *                                        HTTPc_FormMultipartFmt()
 *
 * @brief    Formats a multipart type form based on an array containing the form fields.
 *
 * @param    p_buf           Pointer to buffer where the form will be written.
 *
 * @param    buf_len         Buffer length.
 *
 * @param    p_form_tbl      Pointer to form field's table.
 *
 * @param    form_tbl_size   Table size.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_TYPE
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_FAIL
 *
 * @return   Length of the formatted form, if no errors.
 *           0 , otherwise.
 *
 * @note     (1) To format the form, only standard Key-Value Pair object are supported in the table.
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
CPU_INT32U HTTPc_FormMultipartFmt(CPU_CHAR             *p_buf,
                                  CPU_INT16U           buf_len,
                                  HTTPc_FORM_TBL_FIELD *p_form_tbl,
                                  CPU_INT16U           form_tbl_size,
                                  RTOS_ERR             *p_err)
{
  CPU_CHAR             *p_buf_wr;
  CPU_CHAR             *p_str;
  HTTPc_FORM_TBL_FIELD *p_tbl_field;
  HTTPc_KEY_VAL        *p_form_field;
  CPU_SIZE_T           data_size = 0;
  CPU_SIZE_T           name_field_len;
  CPU_INT08U           name_char_encode_nbr;
  CPU_INT16U           tot_buf_len;
  CPU_INT16U           i;
  CPU_BOOLEAN          url_encode;
  HTTPc_STATUS         rtn_status = HTTPc_STATUS_NONE;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_form_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG_ERR_SET((buf_len > 0u), *p_err, RTOS_ERR_INVALID_ARG, 0u);
  RTOS_ASSERT_DBG_ERR_SET((form_tbl_size > 0), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  tot_buf_len = buf_len;
  p_buf_wr = p_buf;

  //                                                               ----------------- PARSE FORM TABLE -----------------
  for (i = 0u; i < form_tbl_size; i++) {
    p_tbl_field = &p_form_tbl[i];

    switch (p_tbl_field->Type) {
      case HTTPc_FORM_FIELD_TYPE_KEY_VAL:
        break;

      case HTTPc_FORM_FIELD_TYPE_KEY_VAL_EXT:
      case HTTPc_FORM_FIELD_TYPE_FILE:
      default:
        data_size = 0;
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
        goto exit;
    }

    p_form_field = (HTTPc_KEY_VAL *)p_tbl_field->FieldObjPtr;

    data_size += HTTPc_STR_BOUNDARY_START_LEN + STR_CR_LF_LEN;

    //                                                             Find nbr of chars to encode in name.
    name_field_len = p_form_field->KeyLen;
    name_char_encode_nbr = HTTP_URL_CharEncodeNbr(p_form_field->KeyPtr, name_field_len);

    data_size += HTTP_STR_HDR_FIELD_CONTENT_DISPOSITION_LEN                 // "Content-Disposition"
                 + 2                                                        // ": "
                 + HTTP_STR_CONTENT_DISPOSITION_FORM_DATA_LEN               // "form-data"
                 + 2                                                        // "; "
                 + HTTP_STR_MULTIPART_FIELD_NAME_LEN                        // "name"
                 + 3                                                        // "=\"\""
                 + name_field_len                                           // "name_rx_in_form"
                 + HTTP_URL_ENCODING_JUMP * name_char_encode_nbr            // nbr of char to encode.
                 + STR_CR_LF_LEN;                                           // "\r\n"

    data_size += 2 * STR_CR_LF_LEN
                 + p_form_field->ValLen;

    if (i == (form_tbl_size - 1)) {
      data_size += (HTTPc_STR_BOUNDARY_END_LEN + STR_CR_LF_LEN);
    }

    if (data_size > buf_len) {                                 // Return if no more space in buf.
      data_size = 0;
      RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
      goto exit;
    }

    //                                                             Write start of boundary.
    p_str = Str_Copy_N(p_buf_wr, HTTPc_STR_BOUNDARY_START, data_size);
    if (p_str == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      goto exit;
    }

    p_str += HTTPc_STR_BOUNDARY_START_LEN;

    //                                                             Write CRLF after boundary.
    p_str = Str_Copy_N(p_str, STR_CR_LF, STR_CR_LF_LEN);
    if (p_str == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      goto exit;
    }

    p_str += STR_CR_LF_LEN;
    buf_len -= (p_str - p_buf_wr);
    p_buf_wr = p_str;

    //                                                             Write Content-Disposition header.
    p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                  tot_buf_len,
                                  buf_len,
                                  HTTP_HDR_FIELD_CONTENT_DISPOSITION,
                                  HTTP_STR_CONTENT_DISPOSITION_FORM_DATA,
                                  HTTP_STR_CONTENT_DISPOSITION_FORM_DATA_LEN,
                                  DEF_NO,
                                  &rtn_status,
                                  p_err);
    if (p_str == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      goto exit;
    }

    *p_str = ASCII_CHAR_SEMICOLON;
    p_str++;
    *p_str = ASCII_CHAR_SPACE;
    p_str++;

    //                                                             Write string "name".
    p_str = Str_Copy_N(p_str, HTTP_STR_MULTIPART_FIELD_NAME, HTTP_STR_MULTIPART_FIELD_NAME_LEN);
    if (p_str == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      goto exit;
    }

    p_str += HTTP_STR_MULTIPART_FIELD_NAME_LEN;
    *p_str = ASCII_CHAR_EQUALS_SIGN;
    p_str++;
    *p_str = ASCII_CHAR_QUOTATION_MARK;
    p_str++;

    //                                                             Write name's value.
    url_encode = HTTP_URL_EncodeStr(p_form_field->KeyPtr,
                                    p_str,
                                    &name_field_len,
                                    buf_len);
    if (url_encode == DEF_FAIL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      goto exit;
    }

    p_str += name_field_len;
    *p_str = ASCII_CHAR_QUOTATION_MARK;
    p_str++;

    //                                                             Write 2 CRLF.
    p_str = Str_Copy_N(p_str, STR_CR_LF, STR_CR_LF_LEN);
    if (p_str == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      goto exit;
    }

    p_str += STR_CR_LF_LEN;

    buf_len -= (p_str - p_buf_wr);
    p_buf_wr = p_str;

    p_str = Str_Copy_N(p_str, STR_CR_LF, STR_CR_LF_LEN);
    if (p_str == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      goto exit;
    }

    p_str += STR_CR_LF_LEN;
    buf_len -= (p_str - p_buf_wr);
    p_buf_wr = p_str;

    //                                                             Write Data.
    Mem_Copy(p_buf_wr, p_form_field->ValPtr, p_form_field->ValLen);

    p_str += p_form_field->ValLen;
    buf_len -= (p_str - p_buf_wr);
    p_buf_wr = p_str;

    //                                                             Write CRLF.
    p_str = Str_Copy_N(p_buf_wr, STR_CR_LF, STR_CR_LF_LEN);
    if (p_str == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      goto exit;
    }

    p_str += STR_CR_LF_LEN;
    buf_len -= (p_str - p_buf_wr);
    p_buf_wr = p_str;

    if (i == (form_tbl_size - 1)) {
      //                                                           Write end of boundary.
      p_str = Str_Copy_N(p_buf_wr, HTTPc_STR_BOUNDARY_END, HTTPc_STR_BOUNDARY_END_LEN);
      if (p_str == DEF_NULL) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
        goto exit;
      }

      p_str += HTTPc_STR_BOUNDARY_END_LEN;
      //                                                           Write CRLF after last boundary.
      p_str = Str_Copy_N(p_str, STR_CR_LF, STR_CR_LF_LEN);
      if (p_str == DEF_NULL) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
        goto exit;
      }

      p_str += STR_CR_LF_LEN;
      buf_len -= (p_str - p_buf_wr);
      p_buf_wr = p_str;
    }
  }

exit:
  return (data_size);
}
#endif

/****************************************************************************************************//**
 *                                          HTTPc_FormAddKeyVal()
 *
 * @brief    Adds a Key-Value Pair object to the form table.
 *
 * @param    p_form_tbl  Pointer to the form table.
 *
 * @param    p_kvp       Pointer to Key-Value Pair object to put in table.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
void HTTPc_FormAddKeyVal(HTTPc_FORM_TBL_FIELD *p_form_tbl,
                         HTTPc_KEY_VAL        *p_key_val,
                         RTOS_ERR             *p_err)
{
  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_form_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_key_val != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_key_val->KeyPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_key_val->ValPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- SET TABLE FIELD ------------------
  p_form_tbl->Type = HTTPc_FORM_FIELD_TYPE_KEY_VAL;
  p_form_tbl->FieldObjPtr = (void *)p_key_val;
}
#endif

/****************************************************************************************************//**
 *                                        HTTPc_FormAddKeyValExt()
 *
 * @brief    Adds an Extended Key-Value Pair object to the form table.
 *
 * @param    p_form_tbl      p_key_val_ext
 *
 * @param    p_key_val_ext   Pointer to the Extended Key-Value Pair object to put in table.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *
 * @note     (1) HTTPc_KVP_BIG type object allows you to setup a Hook function that will be called when
 *               the form is sent. This lets the application write the value directly into the buffer.
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
void HTTPc_FormAddKeyValExt(HTTPc_FORM_TBL_FIELD *p_form_tbl,
                            HTTPc_KEY_VAL_EXT    *p_key_val_ext,
                            RTOS_ERR             *p_err)
{
  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_form_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_key_val_ext != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_key_val_ext->KeyPtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_key_val_ext->OnValTx != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- SET TABLE FIELD ------------------
  p_form_tbl->Type = HTTPc_FORM_FIELD_TYPE_KEY_VAL_EXT;
  p_form_tbl->FieldObjPtr = (void *)p_key_val_ext;
}
#endif

/****************************************************************************************************//**
 *                                          HTTPc_FormAddFile()
 *
 * @brief    Adds a multipart file object to the form table.
 *
 * @param    p_form_tbl  Pointer to the form table.
 *
 * @param    p_file_obj  Pointer to the multipart file object to put in table.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
void HTTPc_FormAddFile(HTTPc_FORM_TBL_FIELD *p_form_tbl,
                       HTTPc_MULTIPART_FILE *p_file_obj,
                       RTOS_ERR             *p_err)
{
  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_form_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_file_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_file_obj->NamePtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_file_obj->FileNamePtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_file_obj->OnFileTx != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- SET TABLE FIELD ------------------
  p_form_tbl->Type = HTTPc_FORM_FIELD_TYPE_FILE;
  p_form_tbl->FieldObjPtr = (void *)p_file_obj;
}
#endif

/****************************************************************************************************//**
 *                                        HTTPc_WebSockSetParam()
 *
 * @brief    Sets a parameter related to a given WebSocket object.
 *
 * @param    p_ws_obj    Pointer to the WebSocket object.
 *
 * @param    type        Parameter type :
 *                           - HTTPc_PARAM_TYPE_WEBSOCK_ON_OPEN
 *                           - HTTPc_PARAM_TYPE_WEBSOCK_ON_CLOSE
 *                           - HTTPc_PARAM_TYPE_WEBSOCK_ON_MSG_RX_INIT
 *                           - HTTPc_PARAM_TYPE_WEBSOCK_ON_MSG_RX_DATA
 *                           - HTTPc_PARAM_TYPE_WEBSOCK_ON_MSG_RX_COMPLETE
 *                           - HTTPc_PARAM_TYPE_WEBSOCK_ON_ERR
 *                           - HTTPc_PARAM_TYPE_WEBSOCK_ON_PONG
 *
 * @param    p_param     Pointer to parameter.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_INIT
 *                           - RTOS_ERR_OWNERSHIP
 *******************************************************************************************************/
#ifdef  HTTPc_WEBSOCK_MODULE_EN
void HTTPc_WebSockSetParam(HTTPc_WEBSOCK_OBJ *p_ws_obj,
                           HTTPc_PARAM_TYPE  type,
                           void              *p_param,
                           RTOS_ERR          *p_err)
{
  HTTPc_WEBSOCK *p_ws;
  CPU_BOOLEAN   in_use;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_ws_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_param != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    goto exit;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_ws = (HTTPc_WEBSOCK *)p_ws_obj;
  //                                                               ------ VALIDATE THAT REQ IS NOT USED ALREADY -------
  CORE_ENTER_ATOMIC();
  in_use = p_ws->Flags.IsWebsockUsed;
  CORE_EXIT_ATOMIC();
  if (in_use == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OWNERSHIP);
    goto exit;
  }

  //                                                               ------------- WEBSOCK PARAMETER SETUP --------------
  switch (type) {
    case HTTPc_PARAM_TYPE_WEBSOCK_ON_OPEN:
      p_ws->OnOpen = (HTTPc_WEBSOCK_ON_OPEN)p_param;
      break;

    case HTTPc_PARAM_TYPE_WEBSOCK_ON_CLOSE:
      p_ws->OnClose = (HTTPc_WEBSOCK_ON_CLOSE)p_param;
      break;

    case HTTPc_PARAM_TYPE_WEBSOCK_ON_MSG_RX_INIT:
      p_ws->OnMsgRxInit = (HTTPc_WEBSOCK_ON_RX_INIT)p_param;
      break;

    case HTTPc_PARAM_TYPE_WEBSOCK_ON_MSG_RX_DATA:
      p_ws->OnMsgRxData = (HTTPc_WEBSOCK_ON_RX_DATA)p_param;
      break;

    case HTTPc_PARAM_TYPE_WEBSOCK_ON_MSG_RX_COMPLETE:
      p_ws->OnMsgRxComplete = (HTTPc_WEBSOCK_ON_RX_COMPLETE)p_param;
      break;

    case HTTPc_PARAM_TYPE_WEBSOCK_ON_ERR:
      p_ws->OnErr = (HTTPc_WEBSOCK_ON_ERR)p_param;
      break;

    case HTTPc_PARAM_TYPE_WEBSOCK_ON_PONG:
      p_ws->OnPong = (HTTPc_WEBSOCK_ON_PONG)p_param;
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_TYPE,; );
  }

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                      HTTPc_WebSockMsgSetParam()
 *
 * @brief    Sets a parameter related to a given WebSocket message object.
 *
 * @param    p_msg_obj   Pointer to the WebSocket message object.
 *
 * @param    type        Parameter type :
 *                           - HTTPc_PARAM_TYPE_WEBSOCK_MSG_USER_DATA
 *                           - HTTPc_PARAM_TYPE_WEBSOCK_MSG_ON_TX_INIT
 *                           - HTTPc_PARAM_TYPE_WEBSOCK_MSG_ON_TX_DATA
 *                           - HTTPc_PARAM_TYPE_WEBSOCK_MSG_ON_TX_COMPLETE
 *
 * @param    p_param     Pointer to parameter.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_INIT
 *                           - RTOS_ERR_OWNERSHIP
 *******************************************************************************************************/
#ifdef  HTTPc_WEBSOCK_MODULE_EN
void HTTPc_WebSockMsgSetParam(HTTPc_WEBSOCK_MSG_OBJ *p_msg_obj,
                              HTTPc_PARAM_TYPE      type,
                              void                  *p_param,
                              RTOS_ERR              *p_err)
{
  HTTPc_WEBSOCK_MSG *p_msg;
  CPU_BOOLEAN       in_use;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_param != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    goto exit;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_msg = (HTTPc_WEBSOCK_MSG *)p_msg_obj;
  //                                                               ------ VALIDATE THAT REQ IS NOT USED ALREADY -------
  CORE_ENTER_ATOMIC();
  in_use = p_msg->Flags.IsUsed;
  CORE_EXIT_ATOMIC();
  if (in_use == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OWNERSHIP);
    goto exit;
  }
  //                                                               ------------- WEBSOCK PARAMETER SETUP --------------
  switch (type) {
    case HTTPc_PARAM_TYPE_WEBSOCK_MSG_USER_DATA:
      p_msg->UserDataPtr = p_param;
      break;

    case HTTPc_PARAM_TYPE_WEBSOCK_MSG_ON_TX_INIT:
      p_msg->OnMsgTxInit = (HTTPc_WEBSOCK_ON_TX_INIT)p_param;
      break;

    case HTTPc_PARAM_TYPE_WEBSOCK_MSG_ON_TX_DATA:
      p_msg->OnMsgTxData = (HTTPc_WEBSOCK_ON_TX_DATA)p_param;
      break;

    case HTTPc_PARAM_TYPE_WEBSOCK_MSG_ON_TX_COMPLETE:
      p_msg->OnMsgTxComplete = (HTTPc_WEBSOCK_ON_TX_COMPLETE)p_param;
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_TYPE,; );
  }

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                        HTTPc_WebSockUpgrade()
 *
 * @brief    Upgrades an HTTP client connection to a WebSocket.
 *
 * @param    p_conn_obj          Pointer to a valid HTTPc Connection on which request will occurred.
 *
 * @param    p_req_obj           Pointer to a request to send.
 *
 * @param    p_resp_obj          Pointer to a response object that will be filled with the received
 *                               response.
 *
 * @param    p_ws_obj            Pointer to a WebSocket object.
 *
 * @param    p_resource_path     Pointer to a complete URI (or only resource path) of the request.
 *
 * @param    resource_path_len   Resource path length.
 *
 * @param    flags               Configuration flags :
 *                                   - HTTPc_FLAG_WEBSOCK_NO_BLOCK
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NOT_INIT
 *                                   - RTOS_ERR_OWNERSHIP
 *                                   - RTOS_ERR_ALREADY_EXISTS
 *                                   - RTOS_ERR_NET_INVALID_CONN
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_OVF
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_NO_MORE_RSRC
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_TIMEOUT
 *
 * @return   DEF_OK,   if the operation is successful.
 *           DEF_FAIL, if the operation has failed.
 *
 * @note     (1) Connection Object:
 *               - (a) Since the WebSocket Upgrade handshake is based on a HTTP request, the connection
 *                     object p_conn_obj MUST have already established a connection to the desired host
 *                     server.
 *
 * @note     (2) Request and Response Object.
 *               - (a) Any HTTP request/response features available can be used during the WebSocket
 *                     Upgrade Request.
 *
 * @note     (3) WebSocket Upgrade related Header field
 *               - (a) During a WebSocket Upgrade handshake, the following mandatory header fields are
 *                     managed by the WebSocket module and MUST NOT be set by the Application:
 *                     - Connection
 *                     - Upgrade
 *                     - Sec-WebSocket-Key
 *                     - Sec-WebSocket-Accept
 *                     - Sec-WebSocket-Version
 *               - (b) During a WebSocket Upgrade Handshake, the following optional header fields MAY be
 *                     managed by the Application using the standard HTTPc Request/Response API:
 *                     - Sec-WebSocket-Protocol
 *                     - Sec-WebSocket-Extensions
 *
 * @note     (4) If the WebSocket Upgrade Handshake is successful, the Status Code in the response
 *               object should be '101'. This means that the Host server has switched to the WebSocket
 *               protocol. Otherwise, the response object will TYPICALLY be described the reason of the
 *               failure.
 *
 * @note     (5) When the WebSocket Upgrade is completed, any HTTP request is no more allowed and it is
 *               not possible to switch the connection protocol to HTTP. However, if an HTTP request
 *               is required to be sent by the Application, it SHOULD do one of the following
 *               procedure:
 *               - (1) Close the current WebSocket Connection by sending a Close Frame and open again the
 *                     HTTP connection.
 *               - (2) Open a different HTTP connection with the Host Server.
 *******************************************************************************************************/
#ifdef  HTTPc_WEBSOCK_MODULE_EN
CPU_BOOLEAN HTTPc_WebSockUpgrade(HTTPc_CONN_OBJ    *p_conn_obj,
                                 HTTPc_REQ_OBJ     *p_req_obj,
                                 HTTPc_RESP_OBJ    *p_resp_obj,
                                 HTTPc_WEBSOCK_OBJ *p_ws_obj,
                                 CPU_CHAR          *p_resource_path,
                                 CPU_INT16U        resource_path_len,
                                 HTTPc_FLAGS       flags,
                                 RTOS_ERR          *p_err)
{
  HTTPc_REQ         *p_req;
  HTTPc_WEBSOCK     *p_ws;
  HTTPc_WEBSOCK_REQ *p_ws_req;
  HTTPc_CONN        *p_conn;
  CPU_BOOLEAN       is_websocket;
  CPU_BOOLEAN       is_connected;
  CPU_BOOLEAN       no_block;
  CPU_BOOLEAN       result = DEF_NO;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_conn_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_req_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_resp_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_ws_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_resource_path != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    goto exit;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_req = (HTTPc_REQ *)p_req_obj;
  p_ws = (HTTPc_WEBSOCK *)p_ws_obj;
  p_conn = (HTTPc_CONN *)p_conn_obj;
  //                                                               -------------------- VALIDATION --------------------
  //                                                               Validate if the conn is eligible to upgrade.
  //                                                               Check if the conn is already upgraded to Websock.
  CORE_ENTER_ATOMIC();
  is_websocket = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_WEBSOCKET);
  CORE_EXIT_ATOMIC();
  if (is_websocket == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    result = DEF_NO;
    goto exit;
  }
  //                                                               Check if the connn is still open.
  CORE_ENTER_ATOMIC();
  is_connected = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_CONNECT);
  CORE_EXIT_ATOMIC();
  if (is_connected == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_INVALID_CONN);
    result = DEF_NO;
    goto exit;
  }
  //                                                               If no blocking mode, validate callbacks.
  no_block = DEF_BIT_IS_SET(flags, HTTPc_FLAG_WEBSOCK_NO_BLOCK);
  if (no_block == DEF_YES) {
    RTOS_ASSERT_DBG_ERR_SET((p_ws->OnOpen != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG, DEF_FAIL);
  }
  //                                                               -------------- REQUEST INTIALIZATION ---------------
  //                                                               Set as a Upgrade Request to WebSocket.
  is_websocket = DEF_YES;
  HTTPc_ReqSetParam(p_req_obj,
                    HTTPc_PARAM_TYPE_REQ_UPGRADE_WEBSOCKET,
                    &is_websocket,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    result = DEF_FAIL;
    goto exit;
  }
  //                                                               Initialize the WebSocket Request object.
  p_ws_req = HTTPcWebSock_InitReqObj(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    result = DEF_FAIL;
    goto exit;
  }

  p_ws_req->WebSockObjPtr = p_ws;
  p_req->WebSockPtr = p_ws_req;
  //                                                               ---------------- SEND HTTP REQUEST -----------------
  result = HTTPc_ReqSend(p_conn_obj,
                         p_req_obj,
                         p_resp_obj,
                         HTTP_METHOD_GET,
                         p_resource_path,
                         resource_path_len,
                         flags,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    result = DEF_FAIL;
  }

exit:
  return (result);
}
#endif

/****************************************************************************************************//**
 *                                          HTTPc_WebSockSend()
 *
 * @brief    Sends a WebSocket message.
 *
 * @param    p_conn_obj      Pointer to a valid HTTPc Connection upgraded to WebSocket.
 *
 * @param    p_msg_obj       Pointer to a valid WebSocket message object.
 *
 * @param    msg_type        Type of message to send.
 *
 * @param    p_data          Pointer to the payload to send.
 *
 * @param    payload_len     Length of the payload to send.
 *
 * @param    flags           Configuration flags :
 *                           HTTPc_FLAG_WEBSOCK_NO_BLOCK
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_OWNERSHIP
 *                               - RTOS_ERR_NET_INVALID_CONN
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                               - RTOS_ERR_FAIL
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_NO_MORE_RSRC
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @return   DEF_OK,     the operation is successful.
 *           DEF_FAIL,   the operation has failed.
 *
 * @note     (1) The connection object p_conn MUST has been previously upgraded to WebSocket to use
 *               this function. Refer to the HTTPc_WebSockUpgrade() function.
 *
 * @note     (2) The available Message Types are as follows :
 *               - HTTPc_WEBSOCK_MSG_TYPE_TXT_FRAME (Data msg)
 *               - HTTPc_WEBSOCK_MSG_TYPE_BIN_FRAME (Data msg)
 *               - HTTPc_WEBSOCK_MSG_TYPE_CLOSE     (Ctrl msg)
 *               - HTTPc_WEBSOCK_MSG_TYPE_PING      (Ctrl msg)
 *               - HTTPc_WEBSOCK_MSG_TYPE_PONG      (Ctrl msg)
 *
 * @note     (3) Payload Content:
 *               - (a) The "Payload data" (argument p_data) is defined as "Extension data" concatenated
 *                     with "Application data".
 *               - (b) Extension data length is 0 Bytes unless an extension has been negotiated during
 *                     the handshake.
 *               - (c) "Extension data" content and length are defines by the extension negotiated.
 *               - (d) If negotiated, the "Extension data" must be handled by the application.
 *
 * @note     (4) Data message Restrictions:
 *               - (b) Even if the RFC6455 allows you to sent a message payload of up to 2^64 bytes.
 *                     Only 2^32 bytes is allowed in the current implementation.
 *
 * @note     (5) Control message Restrictions:
 *               - (a) According to the RFC 6455 section 5.5:
 *                     @n
 *                     "All control frames MUST have a payload length of 125 bytes or less
 *                     and MUST NOT be fragmented."
 *               - (b) Closed frames have a specific payload format. For more information, refer to the
 *                     HTTPc_WebSockFmtCloseMsg() function.
 *
 * @note     (6) Client-to-Server Masking:
 *               - (a) According to the RFC 6455 section 5.2:
 *                     @n
 *                     "All frames sent from the client to the server are masked by a 32-bit value that is
 *                     contained within the frame."
 *               - (b) The application DO NOT need to mask the payload since it's handled by the WebSocket
 *                     module.
 *******************************************************************************************************/
#ifdef  HTTPc_WEBSOCK_MODULE_EN
CPU_BOOLEAN HTTPc_WebSockSend(HTTPc_CONN_OBJ         *p_conn_obj,
                              HTTPc_WEBSOCK_MSG_OBJ  *p_msg_obj,
                              HTTPc_WEBSOCK_MSG_TYPE msg_type,
                              CPU_CHAR               *p_data,
                              CPU_INT32U             payload_len,
                              HTTPc_FLAGS            flags,
                              RTOS_ERR               *p_err)
{
  HTTPc_CONN        *p_conn;
  HTTPc_WEBSOCK_MSG *p_msg;
  CPU_BOOLEAN       is_websocket;
  CPU_BOOLEAN       no_block;
  CPU_BOOLEAN       in_use;
  CPU_BOOLEAN       result = DEF_NO;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_conn_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_msg_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_data != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    goto exit;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_conn = (HTTPc_CONN *)p_conn_obj;
  p_msg = (HTTPc_WEBSOCK_MSG *)p_msg_obj;
  //                                                               Validate the connection state.
  CORE_ENTER_ATOMIC();
  is_websocket = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_WEBSOCKET);
  CORE_EXIT_ATOMIC();
  if (is_websocket == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_INVALID_CONN);
    result = DEF_NO;
    goto exit;
  }
  //                                                               Check if the Websock msg obj is already used.
  CORE_ENTER_ATOMIC();
  in_use = p_msg->Flags.IsUsed;
  CORE_EXIT_ATOMIC();
  if (in_use == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OWNERSHIP);
    goto exit;
  }
  //                                                               Validate callback if no blocking mode.
  no_block = DEF_BIT_IS_SET(flags, HTTPc_FLAG_WEBSOCK_NO_BLOCK);
  if (no_block == DEF_YES) {
    RTOS_ASSERT_DBG_ERR_SET((p_msg->OnMsgTxComplete != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG, DEF_FAIL);
  }
  //                                                               Check if the message will be set on callback.
  if (p_data == DEF_NULL) {
    RTOS_ASSERT_DBG_ERR_SET((p_msg->OnMsgTxData != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG, DEF_FAIL);
  }
  //                                                               Validate the opcode.
  switch (msg_type) {
    case HTTPc_WEBSOCK_MSG_TYPE_TXT_FRAME:
    case HTTPc_WEBSOCK_MSG_TYPE_BIN_FRAME:
      break;

    case HTTPc_WEBSOCK_MSG_TYPE_CLOSE:
    case HTTPc_WEBSOCK_MSG_TYPE_PING:
    case HTTPc_WEBSOCK_MSG_TYPE_PONG:
      //                                                           Control msg can't be longer than 125 bytes.
      RTOS_ASSERT_DBG_ERR_SET((payload_len <= HTTPc_WEBSOCK_MAX_CTRL_FRAME_LEN), *p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_TYPE, DEF_NO);
  }

  //                                                               --------------- SET WEBSOCK MESSAGE ----------------
  p_msg->DataPtr = p_data;
  p_msg->Len = payload_len;
  p_msg->LenSent = 0u;
  p_msg->OpCode = (HTTPc_WEBSOCK_MSG_TYPE) msg_type;
  p_msg->ConnPtr = p_conn;

  p_msg->Flags.IsFin = DEF_YES;
  p_msg->Flags.IsMasked = DEF_YES;
  p_msg->Flags.IsHdrSet = DEF_NO;
  p_msg->Flags.IsCompleted = DEF_NO;

  //                                                               ---------------- SET BLOCKING MODE -----------------
  no_block = DEF_BIT_IS_SET(flags, HTTPc_FLAG_WEBSOCK_NO_BLOCK);
  if (no_block == DEF_NO) {
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
    HTTPcTask_TransDoneSignalCreate(p_conn, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_conn_close;
    }
#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_FAIL);
#endif
  }

  HTTPcTask_MsgQueue(HTTPc_MSG_TYPE_WEBSOCK_MSG,
                     p_msg,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  HTTPcTask_Wake(p_conn, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (no_block == DEF_NO) {
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
    //                                                             -------- WAIT FOR RESPONSE IN BLOCKING MODE --------
    HTTPcTask_TransDoneSignalWait(p_conn, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_conn_close;
    }
    CORE_ENTER_ATOMIC();
    result = p_msg->Flags.IsCompleted;
    CORE_EXIT_ATOMIC();
    if (result == DEF_FAIL) {
      if (RTOS_ERR_CODE_GET(p_conn->ErrCode) == RTOS_ERR_NONE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(p_conn->ErrCode));
      }
      goto exit;
    }

    HTTPcTask_TransDoneSignalDel(p_conn);

#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_FAIL);
#endif
  }

  result = DEF_OK;

exit:
  return (result);

#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
exit_conn_close:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    HTTPc_ConnCloseHandler(p_conn, HTTPc_FLAG_NONE, &local_err);
    return (result);
  }
#endif
}
#endif

/****************************************************************************************************//**
 *                                          HTTPc_WebSockClr()
 *
 * @brief    Clears an HTTPc WebSock object before its first usage.
 *
 * @param    p_ws_obj    Pointer to the current HTTPc Websock object.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_INIT
 *******************************************************************************************************/
#ifdef  HTTPc_WEBSOCK_MODULE_EN
void HTTPc_WebSockClr(HTTPc_WEBSOCK_OBJ *p_ws_obj,
                      RTOS_ERR          *p_err)
{
  HTTPc_WEBSOCK *p_ws;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_DECLARE_IRQ_STATE;
#endif

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_ws_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    return;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_ws = (HTTPc_WEBSOCK *)p_ws_obj;

  //                                                               -------------- WEBSOCK INITIALIZATION --------------
  p_ws->CloseCode = HTTPc_WEBSOCK_CLOSE_CODE_NONE;
  p_ws->CloseReason.DataPtr = DEF_NULL;
  p_ws->CloseReason.Len = 0u;
  p_ws->Flags.IsCloseStarted = DEF_NO;
  p_ws->Flags.IsPongStarted = DEF_NO;
  p_ws->Flags.IsRxDataCached = DEF_NO;
  p_ws->Flags.IsTxMsgCtrlUsed = DEF_NO;
  p_ws->Flags.IsWebsockUsed = DEF_NO;
  p_ws->OnClose = DEF_NULL;
  p_ws->OnMsgRxInit = DEF_NULL;
  p_ws->OnMsgRxData = DEF_NULL;
  p_ws->OnMsgRxComplete = DEF_NULL;
  p_ws->OnOpen = DEF_NULL;
  p_ws->OnErr = DEF_NULL;
  p_ws->OnPong = DEF_NULL;
  p_ws->RxMsgLen = 0u;
  p_ws->RxMsgLenRead = 0u;
  p_ws->RxMsgOpCode = HTTPc_WEBSOCK_OPCODE_NONE;
  p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_WAIT;
  p_ws->TxMsgListEndPtr = DEF_NULL;
  p_ws->TxMsgListHeadPtr = DEF_NULL;
  p_ws->TxState = HTTPc_WEBSOCK_TX_STATE_MSG_INIT;
  p_ws->TotalMsgLen = 0u;
  p_ws->OrigOpCode = HTTPc_WEBSOCK_OPCODE_NONE;

  Mem_Clr(&p_ws->TxMsgCtrl, sizeof(HTTPc_WEBSOCK_MSG));
}
#endif

/****************************************************************************************************//**
 *                                         HTTPc_WebSockMsgClr()
 *
 * @brief    Clears an HTTPc WebSock Message object before its first usage.
 *
 * @param    p_msg_obj   Pointer to the WebSock Message object to clear.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_INIT
 *******************************************************************************************************/
#ifdef  HTTPc_WEBSOCK_MODULE_EN
void HTTPc_WebSockMsgClr(HTTPc_WEBSOCK_MSG_OBJ *p_msg_obj,
                         RTOS_ERR              *p_err)
{
  HTTPc_WEBSOCK_MSG *p_msg;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_DECLARE_IRQ_STATE;
#endif

  //                                                               --------------- ARGUMENTS VALIDATION ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_msg_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    return;
  }
  CORE_EXIT_ATOMIC();
#endif

  p_msg = (HTTPc_WEBSOCK_MSG *)p_msg_obj;
  //                                                               ------------ WEBSOCK REQ INITIALIZATION ------------
  p_msg->ConnPtr = DEF_NULL;
  p_msg->DataPtr = DEF_NULL;
  p_msg->Len = 0u;
  p_msg->Flags.IsFin = DEF_NO;
  p_msg->Flags.IsMasked = DEF_NO;
  p_msg->Flags.IsUsed = DEF_NO;
  p_msg->Flags.IsNoBlock = DEF_NO;
  p_msg->Flags.IsHdrSet = DEF_NO;
  p_msg->LenSent = 0u;
  p_msg->MskKey = 0u;
  p_msg->NextPtr = DEF_NULL;
  p_msg->OpCode = HTTPc_WEBSOCK_OPCODE_NONE;
  p_msg->UserDataPtr = DEF_NULL;
  p_msg->DataLen = 0u;
  p_msg->OnMsgTxInit = DEF_NULL;
  p_msg->OnMsgTxData = DEF_NULL;
  p_msg->OnMsgTxComplete = DEF_NULL;
}
#endif

/****************************************************************************************************//**
 *                                      HTTPc_WebSockFmtCloseMsg()
 *
 * @brief    Formats a Close Frame.
 *
 * @param    close_code  Value that defines the origin of connection closure.
 *
 * @param    p_reason    Pointer to a string that contains a reason of the connection closure.
 *
 * @param    p_buf       Pointer to the destination buffer.
 *
 * @param    buf_len     Length of the destination buffer.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_INIT
 *                           - RTOS_ERR_WOULD_OVF
 *
 * @return   Payload length, if the operation is successful.
 *           0, if the operation has failed.
 *
 * @note     (1) Close Frames have a specific format. According to the RFC 6455 section :
 *               @n
 *               "If there is a body, the first two bytes of the body MUST be a 2-byte unsigned
 *               integer (in network byte order) representing a status code with value /code/
 *               defined in Section 7.4. Following the 2-byte integer,the body MAY contain
 *               UTF-8-encoded data with value /reason/, the interpretation of which is not
 *               defined by this specification."
 *
 * @note     (2) The following Close Codes are available :
 *               - HTTPc_WEBSOCK_CLOSE_CODE_NORMAL
 *               - HTTPc_WEBSOCK_CLOSE_CODE_GOING_AWAY
 *               - HTTPc_WEBSOCK_CLOSE_CODE_PROTOCOL_ERR
 *               - HTTPc_WEBSOCK_CLOSE_CODE_DATA_TYPE_NOT_ALLOWED
 *               - HTTPc_WEBSOCK_CLOSE_CODE_DATA_TYPE_ERR
 *               - HTTPc_WEBSOCK_CLOSE_CODE_POLICY_VIOLATION
 *               - HTTPc_WEBSOCK_CLOSE_CODE_MSG_TOO_BIG
 *               - HTTPc_WEBSOCK_CLOSE_CODE_INVALID_EXT
 *               - HTTPc_WEBSOCK_CLOSE_CODE_UNEXPECTED_CONDITION
 *
 * @note     (3) Close Reason:
 *               - (a) Except for its length, the reason string has no restriction and is
 *                     user-definable. This should be used for debugging purposes.
 *               - (b) This field can be empty.
 *******************************************************************************************************/
#if (HTTPc_CFG_WEBSOCKET_EN == DEF_ENABLED)
CPU_INT16U HTTPc_WebSockFmtCloseMsg(HTTPc_WEBSOCK_CLOSE_CODE close_code,
                                    CPU_CHAR                 *p_reason,
                                    CPU_CHAR                 *p_buf,
                                    CPU_INT16U               buf_len,
                                    RTOS_ERR                 *p_err)
{
  CPU_INT16U str_len = 0u;
  CPU_INT16U msg_len = 0u;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_DECLARE_IRQ_STATE;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_reason != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  if (HTTPc_InitDone != DEF_YES) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    goto exit;
  }
  CORE_EXIT_ATOMIC();

  switch (close_code) {
    case HTTPc_WEBSOCK_CLOSE_CODE_NORMAL:
    case HTTPc_WEBSOCK_CLOSE_CODE_GOING_AWAY:
    case HTTPc_WEBSOCK_CLOSE_CODE_PROTOCOL_ERR:
    case HTTPc_WEBSOCK_CLOSE_CODE_DATA_TYPE_NOT_ALLOWED:
    case HTTPc_WEBSOCK_CLOSE_CODE_DATA_TYPE_ERR:
    case HTTPc_WEBSOCK_CLOSE_CODE_POLICY_VIOLATION:
    case HTTPc_WEBSOCK_CLOSE_CODE_MSG_TOO_BIG:
    case HTTPc_WEBSOCK_CLOSE_CODE_INVALID_EXT:
    case HTTPc_WEBSOCK_CLOSE_CODE_UNEXPECTED_CONDITION:
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, 0u);
  }
#endif

  str_len = Str_Len(p_reason);
  RTOS_ASSERT_DBG_ERR_SET((str_len <= HTTPc_WEBSOCK_MAX_CLOSE_REASON_LEN), *p_err, RTOS_ERR_INVALID_ARG, 0u);

  close_code = MEM_VAL_GET_INT16U_BIG(&close_code);
  Mem_Copy(p_buf, (CPU_CHAR *)&close_code, sizeof(CPU_INT16U));

  p_buf += sizeof(CPU_INT16U);
  buf_len -= HTTPc_WEBSOCK_CLOSE_CODE_LEN;

  if (buf_len < str_len) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    goto exit;
  }

  Str_Copy_N(p_buf, p_reason, str_len);
  msg_len = str_len + sizeof(CPU_INT16U);

exit:
  return (msg_len);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       HTTPc_ConnCloseHandler()
 *
 * @brief    Close an HTTPc Connection.
 *
 * @param    p_conn  Pointer to Connection to close.
 *
 * @param    flags   Configuration flags :
 *                   HTTPc_FLAG_CONN_NO_BLOCK
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#if ((HTTPc_CFG_PERSISTENT_EN == DEF_ENABLED) \
  || defined(HTTPc_SIGNAL_TASK_MODULE_EN)       )
static void HTTPc_ConnCloseHandler(HTTPc_CONN  *p_conn,
                                   HTTPc_FLAGS flags,
                                   RTOS_ERR    *p_err)
{
  CPU_BOOLEAN in_use;
  CPU_BOOLEAN no_block;
#if  (defined(HTTPc_TASK_MODULE_EN) && defined(HTTPc_SIGNAL_TASK_MODULE_EN))
  RTOS_ERR local_err;
#endif
  CORE_DECLARE_IRQ_STATE;

  //                                                               ------------ VALIDATE THAT CONN IS USED ------------
  CORE_ENTER_ATOMIC();
  in_use = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_IN_USE);
  CORE_EXIT_ATOMIC();
  if (in_use == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }

  //                                                               ---------------- SET BLOCKING MODE -----------------
  no_block = DEF_BIT_IS_SET(flags, HTTPc_FLAG_CONN_NO_BLOCK);
  if (no_block == DEF_YES) {
    DEF_BIT_SET(p_conn->Flags, HTTPc_FLAG_CONN_NO_BLOCK);
  } else {
    DEF_BIT_CLR(p_conn->Flags, (HTTPc_FLAGS)HTTPc_FLAG_CONN_NO_BLOCK);
  }

#ifdef HTTPc_TASK_MODULE_EN
  if (no_block == DEF_NO) {
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    HTTPcTask_ConnCloseSignalCreate(p_conn, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
  }

  HTTPcTask_MsgQueue(HTTPc_MSG_TYPE_CONN_CLOSE,
                     (void *)p_conn,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  HTTPcTask_Wake(p_conn, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (no_block == DEF_NO) {
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
    HTTPcTask_ConnCloseSignalWait(p_conn, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    HTTPcTask_ConnCloseSignalDel(p_conn);

#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
  }

#else

  HTTPcConn_Close(p_conn, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  HTTPcConn_Remove(p_conn);

#endif

exit:
  return;
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_CLIENT_AVAIL
