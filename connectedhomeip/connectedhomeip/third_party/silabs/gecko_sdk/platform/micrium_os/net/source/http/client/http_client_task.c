/***************************************************************************//**
 * @file
 * @brief Network - HTTP Client Task Module
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

#include  <net/include/http_client.h>
#include  <net/include/http.h>

#include  <common/include/rtos_types.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#ifdef  HTTPc_WEBSOCK_MODULE_EN
#include  "http_client_websock_priv.h"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  HTTPc_TASK_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

#define  HTTPc_TASK_DLY_MS_DFLT                  5u                    // Task delay.
#define  HTTPc_TASK_DLY_MS_MAX                  20u
#define  HTTPc_TASK_DLY_MS_MIN                   0u

#define  HTTPc_TASK_SIGNAL_CONN_CONNECT_DONE     "HTTPc Conn Connect Done"
#define  HTTPc_TASK_SIGNAL_CONN_CLOSE_DONE       "HTTPc Conn Close Done"
#define  HTTPc_TASK_SIGNAL_TRANS_DONE            "HTTPc Transaction Done"

#define  HTTPc_TASK_TIMEOUT_MS_CONN_CONNECT_DFLT  30000u
#define  HTTPc_TASK_TIMEOUT_MS_CONN_CLOSE_DFLT    30000u
#define  HTTPc_TASK_TIMEOUT_MS_TRANS_DFLT         30000u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIAVBLE
 ********************************************************************************************************
 *******************************************************************************************************/

extern HTTPc_DATA *HTTPc_DataPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void HTTPcTask_Handler(void);

static HTTPc_TASK_MSG *HTTPcTask_MsgDequeue(RTOS_ERR *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          HTTPcTask()
 *
 * @brief    HTTP Client main loop.
 *
 * @param    p_data  Pointer to task initialization (required by Micrium OS Kernel).
 *******************************************************************************************************/
void HTTPcTask(void *p_data)
{
  HTTPc_CONN     *p_conn;
  HTTPc_REQ      *p_req;
  RTOS_ERR       local_err;
  HTTPc_TASK_MSG *p_msg;
#ifdef  HTTPc_WEBSOCK_MODULE_EN
  HTTPc_CONN_OBJ    *p_conn_const;
  HTTPc_WEBSOCK_MSG *p_ws_msg;
  HTTPc_WEBSOCK     *p_ws;
#endif

  PP_UNUSED_PARAM(p_data);

  while (DEF_ON) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    //                                                             --------------- CHECK FOR CONN READY ---------------
    p_msg = HTTPcTask_MsgDequeue(&local_err);
    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:
        switch (p_msg->Type) {
          case HTTPc_MSG_TYPE_CONN_OPEN:
            p_conn = (HTTPc_CONN *)p_msg->DataPtr;
            HTTPcConn_Add(p_conn);
            break;

          case HTTPc_MSG_TYPE_CONN_CLOSE:
            p_conn = (HTTPc_CONN *)p_msg->DataPtr;
            p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_APP;
            p_conn->State = HTTPc_CONN_STATE_CLOSE;
            break;

          case HTTPc_MSG_TYPE_REQ:
            p_req = (HTTPc_REQ *)p_msg->DataPtr;
            HTTPcConn_ReqAdd(p_req);
            break;
#ifdef  HTTPc_WEBSOCK_MODULE_EN
          case HTTPc_MSG_TYPE_WEBSOCK_MSG:
            RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
            p_ws_msg = (HTTPc_WEBSOCK_MSG *)p_msg->DataPtr;
            HTTPcWebSock_TxMsgAdd(p_ws_msg, &local_err);
            if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
              p_conn = p_ws_msg->ConnPtr;
              p_ws = p_conn->WebSockPtr;
              if (p_ws != DEF_NULL) {
                if (p_ws->OnErr != DEF_NULL) {
                  p_conn_const = (HTTPc_CONN_OBJ *)p_ws_msg->ConnPtr;
                  p_ws->OnErr(p_conn_const, local_err);
                }
              } else {
                RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NULL_PTR,; );
              }
            }
            break;
#endif
          default:
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_TYPE,; );
        }
        HTTPc_Mem_TaskMsgRelease(p_msg);
        break;

      case RTOS_ERR_TIMEOUT:
      case RTOS_ERR_WOULD_BLOCK:
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    //                                                             -------- SOCKET SELECT ON OPEN CONNECTIONS ---------
    p_conn = HTTPc_DataPtr->TaskConnFirstPtr;
    HTTPcSock_Sel(p_conn, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    //                                                             --------------- HTTP CLIENT HANDLER ----------------
    HTTPcTask_Handler();

    //                                                             -------------- DELAY HTTP CLIENT TASK --------------
    if (HTTPc_DataPtr->TaskDly_ms > 0u) {
      KAL_Dly(HTTPc_DataPtr->TaskDly_ms);
    }
  }
}

/****************************************************************************************************//**
 *                                          HTTPcTask_SetDly()
 *
 * @brief    Set the HTTP Client Task Delay.
 *
 * @param    dly_ms  Task delay value.
 *
 * @note     (1) The Task Delay allows other tasks to runs when HTTP client is set in a none blocking
 *               mode.
 *******************************************************************************************************/
void HTTPcTask_SetDly(CPU_INT08U dly_ms)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG((dly_ms <= HTTPc_TASK_DLY_MS_MAX), RTOS_ERR_INVALID_CFG,; );

  CORE_ENTER_ATOMIC();
  HTTPc_DataPtr->TaskDly_ms = dly_ms;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                         HTTPcTask_MsgQueue()
 *
 * @brief    Signal that a HTTPc message is ready to be process by adding message object to
 *           the message queue.
 *
 * @param    type    Type of the message obj to queue.
 *
 * @param    p_data  Pointer to the Message obj to queue.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void HTTPcTask_MsgQueue(HTTPc_MSG_TYPE type,
                        void           *p_data,
                        RTOS_ERR       *p_err)
{
  HTTPc_TASK_MSG *p_msg;

  p_msg = HTTPc_Mem_TaskMsgGet(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_msg->Type = type;
  p_msg->DataPtr = p_data;

  KAL_QPost(HTTPc_DataPtr->TaskMsgQ_Handle,
            p_msg,
            KAL_OPT_NONE,
            p_err);

exit:
  return;
}

/****************************************************************************************************//**
 *                                          HTTPcTask_ConnAdd()
 *
 * @brief    Add current HTTPc Connection to connection list.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *******************************************************************************************************/
void HTTPcTask_ConnAdd(HTTPc_CONN *p_conn)
{
  HTTPc_CONN *p_conn_item;

  //                                                               ------------ UPDATE INSTANCE CONN LIST -------------
  if (HTTPc_DataPtr->TaskConnFirstPtr == DEF_NULL) {
    p_conn->NextPtr = DEF_NULL;
    HTTPc_DataPtr->TaskConnFirstPtr = p_conn;
  } else {
    p_conn_item = HTTPc_DataPtr->TaskConnFirstPtr;
    p_conn->NextPtr = p_conn_item;
    HTTPc_DataPtr->TaskConnFirstPtr = p_conn;
  }
}

/****************************************************************************************************//**
 *                                        HTTPcTask_ConnRemove()
 *
 * @brief    Remove current HTTPc Connection from connection list.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *******************************************************************************************************/
void HTTPcTask_ConnRemove(HTTPc_CONN *p_conn)
{
  HTTPc_CONN *p_conn_item;
  HTTPc_CONN *p_conn_item_prev;

  p_conn_item = HTTPc_DataPtr->TaskConnFirstPtr;
  p_conn_item_prev = DEF_NULL;

  while (p_conn_item != DEF_NULL) {
    if (p_conn == p_conn_item) {
      if (p_conn == HTTPc_DataPtr->TaskConnFirstPtr) {
        HTTPc_DataPtr->TaskConnFirstPtr = p_conn->NextPtr;
      } else {
        p_conn_item_prev->NextPtr = p_conn->NextPtr;
      }
    }

    p_conn_item_prev = p_conn_item;
    p_conn_item = p_conn_item->NextPtr;
  }

  p_conn->NextPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                     HTTPcTask_ConnConnectSignalCreate()
 *
 * @brief    Create the Connection Connect Signal. The signal is used to inform that the connect
 *           process is complete.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) This signal is used when the HTTP Client task is enabled and use in blocking mode
 *               to advertise that the connect process is done.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_ConnConnectSignalCreate(HTTPc_CONN *p_conn,
                                       RTOS_ERR   *p_err)
{
  p_conn->ConnectSignalPtr = (KAL_SEM_HANDLE *)Mem_DynPoolBlkGet(&HTTPc_DataPtr->SemHandlePool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               -------- CREATE SIGNAL FOR CONNECT COMPLETE --------
  *(KAL_SEM_HANDLE *)p_conn->ConnectSignalPtr = KAL_SemCreate(HTTPc_TASK_SIGNAL_CONN_CONNECT_DONE,
                                                              DEF_NULL,
                                                              p_err);
}
#endif

/****************************************************************************************************//**
 *                                    HTTPcTask_ConnConnectSignalDel()
 *
 * @brief    Delete Connection Connect Signal.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_ConnConnectSignalDel(HTTPc_CONN *p_conn)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_SemDel(*(KAL_SEM_HANDLE *)p_conn->ConnectSignalPtr);

  Mem_DynPoolBlkFree(&HTTPc_DataPtr->SemHandlePool,
                     (KAL_SEM_HANDLE *)p_conn->ConnectSignalPtr,
                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_conn->ConnectSignalPtr = DEF_NULL;
}
#endif

/****************************************************************************************************//**
 *                                   HTTPcTask_ConnCloseSignalCreate()
 *
 * @brief    Create the Connection Close Signal. The signal is used to inform that the connection close
 *           process is complete.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) This signal is used when the HTTP Client task is enabled and use in blocking mode
 *               to advertise that the close process is done.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_ConnCloseSignalCreate(HTTPc_CONN *p_conn,
                                     RTOS_ERR   *p_err)
{
  p_conn->CloseSignalPtr = (KAL_SEM_HANDLE *)Mem_DynPoolBlkGet(&HTTPc_DataPtr->SemHandlePool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ------- CREATE SIGNAL FOR CONN HTTP CLOSING --------
  *(KAL_SEM_HANDLE *)p_conn->CloseSignalPtr = KAL_SemCreate(HTTPc_TASK_SIGNAL_CONN_CLOSE_DONE,
                                                            DEF_NULL,
                                                            p_err);
}
#endif

/****************************************************************************************************//**
 *                                    HTTPcTask_ConnCloseSignalDel()
 *
 * @brief    Delete Connection Close Signal.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_ConnCloseSignalDel(HTTPc_CONN *p_conn)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_SemDel(*(KAL_SEM_HANDLE *)p_conn->CloseSignalPtr);

  Mem_DynPoolBlkFree(&HTTPc_DataPtr->SemHandlePool,
                     (KAL_SEM_HANDLE *)p_conn->CloseSignalPtr,
                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_conn->CloseSignalPtr = DEF_NULL;
}
#endif

/****************************************************************************************************//**
 *                                     HTTPcTask_TransDoneSignalCreate()
 *
 * @brief    Create the Transaction Done Signal. The signal is used to inform that the HTTP transaction
 *           is completed.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) This signal is used when the HTTP Client task is enabled and use in blocking mode
 *               to advertise that the HTTP transaction is completed.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_TransDoneSignalCreate(HTTPc_CONN *p_conn,
                                     RTOS_ERR   *p_err)
{
  p_conn->TransDoneSignalPtr = Mem_DynPoolBlkGet(&HTTPc_DataPtr->SemHandlePool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  *(KAL_SEM_HANDLE *)p_conn->TransDoneSignalPtr = KAL_SemCreate(HTTPc_TASK_SIGNAL_TRANS_DONE,
                                                                DEF_NULL,
                                                                p_err);
}
#endif

/****************************************************************************************************//**
 *                                     HTTPcTask_TransDoneSignalDel()
 *
 * @brief    Delete Transaction Done Signal.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_TransDoneSignalDel(HTTPc_CONN *p_conn)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_SemDel(*(KAL_SEM_HANDLE *)p_conn->TransDoneSignalPtr);

  Mem_DynPoolBlkFree(&HTTPc_DataPtr->SemHandlePool,
                     (KAL_SEM_HANDLE *)p_conn->TransDoneSignalPtr,
                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_conn->TransDoneSignalPtr = DEF_NULL;
}
#endif

/****************************************************************************************************//**
 *                                  HTTPcTask_ConnConnectSignal()
 *
 * @brief    Post Connection Connect Signal.
 *
 * @param    p_conn  Pointer to current HTTPc connection.
 *
 * @note     (1) Signal to inform that the connection connect process has been completed.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_ConnConnectSignal(HTTPc_CONN *p_conn)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_SemPost(*(KAL_SEM_HANDLE *)p_conn->ConnectSignalPtr,
              KAL_OPT_PEND_NONE,
              &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}
#endif

/****************************************************************************************************//**
 *                                      HTTPcTask_ConnCloseSignal()
 *
 * @brief    Post HTTP Connection Close Signal.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @note     (1) Signal to inform that the connection close process has been completed.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_ConnCloseSignal(HTTPc_CONN *p_conn)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_SemPost(*(KAL_SEM_HANDLE *)p_conn->CloseSignalPtr,
              KAL_OPT_PEND_NONE,
              &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}
#endif

/****************************************************************************************************//**
 *                                   HTTPcTask_TransDoneSignal()
 *
 * @brief    Post HTTP Transaction Done Signal.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @note     (1) Signal to inform that the HTTP Transaction has been completed.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_TransDoneSignal(HTTPc_CONN *p_conn)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_SemPost(*(KAL_SEM_HANDLE *)p_conn->TransDoneSignalPtr,
              KAL_OPT_PEND_NONE,
              &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}
#endif

/****************************************************************************************************//**
 *                                   HTTPcTask_ConnConnectSignalWait()
 *
 * @brief    Wait for Connection Connect signal.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_ConnConnectSignalWait(HTTPc_CONN *p_conn,
                                     RTOS_ERR   *p_err)
{
  KAL_SemPend(*(KAL_SEM_HANDLE *)p_conn->ConnectSignalPtr,
              KAL_OPT_PEND_BLOCKING,
              HTTPc_TASK_TIMEOUT_MS_CONN_CONNECT_DFLT,
              p_err);
}
#endif

/****************************************************************************************************//**
 *                                     HTTPcTask_ConnCloseSignalWait()
 *
 * @brief    Wait for HTTP Connection Close signal posting.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_ConnCloseSignalWait(HTTPc_CONN *p_conn,
                                   RTOS_ERR   *p_err)
{
  KAL_SemPend(*(KAL_SEM_HANDLE *)p_conn->CloseSignalPtr,
              KAL_OPT_PEND_BLOCKING,
              HTTPc_TASK_TIMEOUT_MS_CONN_CLOSE_DFLT,
              p_err);
}
#endif

/****************************************************************************************************//**
 *                                    HTTPcTask_TransDoneSignalWait()
 *
 * @brief    Wait for HTTP Response Ready Signal.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) This is a blocking function.
 *******************************************************************************************************/
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_TransDoneSignalWait(HTTPc_CONN *p_conn,
                                   RTOS_ERR   *p_err)
{
  KAL_SemPend(*(KAL_SEM_HANDLE *)p_conn->TransDoneSignalPtr,
              KAL_OPT_PEND_BLOCKING,
              HTTPc_TASK_TIMEOUT_MS_TRANS_DFLT,
              p_err);
}
#endif

/****************************************************************************************************//**
 *                                         HTTPcTask_Wake()
 *
 * @brief    Wake HTTPc Task pending in HTTPcSock_Sel.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void HTTPcTask_Wake(HTTPc_CONN *p_conn,
                    RTOS_ERR   *p_err)
{
  if (p_conn->SockID != NET_SOCK_ID_NONE) {
    NetSock_SelAbort(p_conn->SockID, p_err);
  } else {
    if (HTTPc_DataPtr->TaskConnFirstPtr != DEF_NULL) {
      if (HTTPc_DataPtr->TaskConnFirstPtr->SockID != NET_SOCK_ID_NONE) {
        NetSock_SelAbort(HTTPc_DataPtr->TaskConnFirstPtr->SockID, p_err);
      }
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                            HTTPcTask_Handler()
 *
 * @brief    HTTP Client main task when HTTPc_CFG_MODE_ASYNC_TASK_EN is enabled.
 *           Proceed through the state machine to process all HTTP transaction on each open connection.
 *******************************************************************************************************/
static void HTTPcTask_Handler(void)
{
  HTTPc_CONN *p_conn;

  p_conn = HTTPc_DataPtr->TaskConnFirstPtr;

  while (p_conn != DEF_NULL) {
    switch (p_conn->State) {
      case HTTPc_CONN_STATE_NONE:
      case HTTPc_CONN_STATE_CONNECT:
        HTTPcConn_Process(p_conn);
        break;

      case HTTPc_CONN_STATE_PARAM_VALIDATE:
      case HTTPc_CONN_STATE_REQ_LINE_METHOD:
      case HTTPc_CONN_STATE_REQ_LINE_URI:
      case HTTPc_CONN_STATE_REQ_LINE_QUERY_STR:
      case HTTPc_CONN_STATE_REQ_LINE_PROTO_VER:
      case HTTPc_CONN_STATE_REQ_HDR_HOST:
      case HTTPc_CONN_STATE_REQ_HDR_CONTENT_TYPE:
      case HTTPc_CONN_STATE_REQ_HDR_CONTENT_LEN:
      case HTTPc_CONN_STATE_REQ_HDR_TRANSFER_ENCODE:
      case HTTPc_CONN_STATE_REQ_HDR_CONN:
      case HTTPc_CONN_STATE_REQ_HDR_EXT:
      case HTTPc_CONN_STATE_REQ_HDR_LAST:
      case HTTPc_CONN_STATE_REQ_BODY:
      case HTTPc_CONN_STATE_REQ_BODY_DATA:
      case HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_SIZE:
      case HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_DATA:
      case HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_END:
      case HTTPc_CONN_STATE_REQ_BODY_FORM_APP:
      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY:
      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_HDR_CONTENT_DISPO:
      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_HDR_CONTENT_TYPE:
      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA:
      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA_END:
      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY_END:
      case HTTPc_CONN_STATE_REQ_END:
      case HTTPc_CONN_STATE_RESP_INIT:
      case HTTPc_CONN_STATE_RESP_STATUS_LINE:
      case HTTPc_CONN_STATE_RESP_HDR:
      case HTTPc_CONN_STATE_RESP_BODY:
      case HTTPc_CONN_STATE_RESP_BODY_CHUNK_SIZE:
      case HTTPc_CONN_STATE_RESP_BODY_CHUNK_DATA:
      case HTTPc_CONN_STATE_RESP_BODY_CHUNK_CRLF:
      case HTTPc_CONN_STATE_RESP_BODY_CHUNK_LAST:
      case HTTPc_CONN_STATE_RESP_COMPLETED:
      case HTTPc_CONN_STATE_ERR:
      case HTTPc_CONN_STATE_COMPLETED:
      case HTTPc_CONN_STATE_CLOSE:
        HTTPcConn_TransProcess(p_conn);
        break;

#ifdef  HTTPc_WEBSOCK_MODULE_EN
      case HTTPc_CONN_STATE_WEBSOCK_INIT:
      case HTTPc_CONN_STATE_WEBSOCK_RXTX:
      case HTTPc_CONN_STATE_WEBSOCK_CLOSE:
      case HTTPc_CONN_STATE_WEBSOCK_ERR:
        HTTPcWebSock_Process(p_conn);
        break;
#endif

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
    }

    p_conn = p_conn->NextPtr;
  }
}

/****************************************************************************************************//**
 *                                      HTTPcTask_ConnOpenDequeue()
 *
 * @brief    Check that a HTTPc connection is ready to be connected by looking in the Connection
 *           Connect Queue.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to new connection received in queue.
 *           DEF_NULL if queue is empty.
 *******************************************************************************************************/
static HTTPc_TASK_MSG *HTTPcTask_MsgDequeue(RTOS_ERR *p_err)
{
  HTTPc_TASK_MSG *p_msg = DEF_NULL;
  KAL_OPT        option;

  if (HTTPc_DataPtr->TaskConnFirstPtr == DEF_NULL) {
    option = KAL_OPT_PEND_BLOCKING;
  } else {
    option = KAL_OPT_PEND_NON_BLOCKING;
  }

  p_msg = (HTTPc_TASK_MSG *)KAL_QPend(HTTPc_DataPtr->TaskMsgQ_Handle,
                                      option,
                                      0,
                                      p_err);

  return (p_msg);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // HTTPc_TASK_MODULE_EN
#endif // RTOS_MODULE_NET_HTTP_CLIENT_AVAIL
