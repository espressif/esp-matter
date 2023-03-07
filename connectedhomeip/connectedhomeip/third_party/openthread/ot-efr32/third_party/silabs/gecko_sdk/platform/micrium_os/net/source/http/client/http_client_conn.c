/***************************************************************************//**
 * @file
 * @brief Network - HTTP Client Connectiion Module
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
 *                                     DEPENDENCIES & AVAIL CHECK(S)
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
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "http_client_priv.h"

#include  <net/include/http_client.h>
#include  <net/include/http.h>

#ifdef HTTPc_WEBSOCK_MODULE_EN
#include  "http_client_websock_priv.h"
#endif

#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          HTTPcConn_Process()
 *
 * @brief    Process the Connection State.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *******************************************************************************************************/
void HTTPcConn_Process(HTTPc_CONN *p_conn)
{
  HTTPc_CONN_OBJ *p_conn_const;
  CPU_BOOLEAN    is_connect;
  CPU_BOOLEAN    result;
#ifdef  HTTPc_TASK_MODULE_EN
  CPU_BOOLEAN no_block;
#endif
  RTOS_ERR local_err;

  p_conn_const = (HTTPc_CONN_OBJ *)p_conn;

  switch (p_conn->State) {
    case HTTPc_CONN_STATE_NONE:
      break;

    case HTTPc_CONN_STATE_CONNECT:                              // CONNECT STATE
      is_connect = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_CONNECT);
      if (is_connect == DEF_NO) {
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

        HTTPcConn_Connect(p_conn, &local_err);                  // Connection Connect.
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          p_conn->ErrCode = local_err;
          p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_NONE;
          p_conn->State = HTTPc_CONN_STATE_NONE;
          result = DEF_FAIL;
          HTTPcConn_Remove(p_conn);
        } else {
          p_conn->State = HTTPc_CONN_STATE_PARAM_VALIDATE;
          result = DEF_OK;
        }

#ifdef HTTPc_TASK_MODULE_EN
        //                                                         No-Blocking mode: notify app with callback that ...
        //                                                         ... connect is done.
        no_block = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_NO_BLOCK);
        if ((no_block == DEF_YES)
            && (p_conn->OnConnect != DEF_NULL)) {
          p_conn->OnConnect(p_conn_const, result);
        }

#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
        //                                                         Blocking mode: Signal app that connect is done.
        if (p_conn->ConnectSignalPtr != DEF_NULL) {
          HTTPcTask_ConnConnectSignal(p_conn);
        }
#endif
#endif
      } else {
        p_conn->State = HTTPc_CONN_STATE_PARAM_VALIDATE;
      }
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
  }

  PP_UNUSED_PARAM(p_conn_const);
  PP_UNUSED_PARAM(result);
}

/****************************************************************************************************//**
 *                                          HTTPcConn_Connect()
 *
 * @brief    - (1) Validate Server Socket Address.
 *           - (2) Connect to Server.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void HTTPcConn_Connect(HTTPc_CONN *p_conn,
                       RTOS_ERR   *p_err)
{
  CPU_BOOLEAN is_connect;
#ifdef HTTPc_TASK_MODULE_EN
  CPU_BOOLEAN no_block;
#endif

  is_connect = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_CONNECT);
  if (is_connect == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }

  //                                                               ---------------- VALIDATE CALLBACKS ----------------
#ifdef HTTPc_TASK_MODULE_EN
  no_block = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_NO_BLOCK);
  if (no_block == DEF_YES) {
    RTOS_ASSERT_DBG_ERR_SET((p_conn->OnConnect != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG,; );
  }

  RTOS_ASSERT_DBG_ERR_SET((p_conn->OnClose != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG,; );
#endif

  //                                                               -------------- CONNECT TO HTTP SERVER --------------
  HTTPcSock_Connect(p_conn, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  DEF_BIT_SET(p_conn->Flags, HTTPc_FLAG_CONN_CONNECT);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               HTTPcConn_Close()
 *
 * @brief    - (1) HTTPc Connection Close Handler:
 *               - (a) Call hook function to advertise connection closing.
 *               - (b) Close Socket
 *               - (c) Release HTTPc Connection related objects.
 *               - (d) Remove connection from list.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void HTTPcConn_Close(HTTPc_CONN *p_conn,
                     RTOS_ERR   *p_err)
{
  HTTPc_CONN_OBJ *p_conn_const;
  HTTPc_REQ      *p_req;
  HTTPc_REQ_OBJ  *p_req_const;
#ifdef HTTPc_TASK_MODULE_EN
  RTOS_ERR local_err;
#endif

  p_conn_const = (HTTPc_CONN_OBJ *)p_conn;

  //                                                               ---------- CLEAR CONNECTION REQUESTS LIST ----------
  p_req = p_conn->ReqListHeadPtr;
  p_req_const = (HTTPc_REQ_OBJ *)p_req;
  while (p_req != DEF_NULL) {
#ifdef HTTPc_TASK_MODULE_EN
    if (p_req->OnTransComplete != DEF_NULL) {
      p_req->OnTransComplete(p_conn_const, p_req_const, p_req->RespPtr, DEF_FAIL);
    }
    if (p_req->OnErr != DEF_NULL) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NET_SOCK_CLOSED);
      p_req->OnErr(p_conn_const, p_req_const, local_err);
    }
#else
    PP_UNUSED_PARAM(p_conn_const);
    PP_UNUSED_PARAM(p_req_const);
#endif
    HTTPcConn_ReqRemove(p_conn);
    p_req = p_conn->ReqListHeadPtr;
  }

  DEF_BIT_CLR(p_conn->Flags, (HTTPc_FLAGS)HTTPc_FLAG_CONN_CONNECT);
  DEF_BIT_CLR(p_conn->Flags, (HTTPc_FLAGS)HTTPc_FLAG_CONN_IN_USE);
  //                                                               ------------- CLOSE CONNECTION SOCKET --------------
  HTTPcSock_Close(p_conn, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;
}

/****************************************************************************************************//**
 *                                          HTTPcConn_Process()
 *
 * @brief    Process the Connection State.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *******************************************************************************************************/
void HTTPcConn_TransProcess(HTTPc_CONN *p_conn)
{
  HTTPc_CONN_OBJ *p_conn_const;
  HTTPc_REQ      *p_req;
  HTTPc_REQ_OBJ  *p_req_const;
  CPU_BOOLEAN    result;
  CPU_BOOLEAN    success;
  CPU_BOOLEAN    to_close;
  RTOS_ERR       local_err;
#ifdef HTTPc_WEBSOCK_MODULE_EN
  CPU_BOOLEAN is_websocket;
#endif
#ifdef HTTPc_TASK_MODULE_EN
  CPU_BOOLEAN no_block;
#endif
  p_conn_const = (HTTPc_CONN_OBJ *)p_conn;

  p_req = p_conn->ReqListHeadPtr;
  p_req_const = (HTTPc_REQ_OBJ *)p_req;

  //                                                               ------------- CONNECTION STATE MACHINE -------------
  switch (p_conn->State) {
    case HTTPc_CONN_STATE_PARAM_VALIDATE:                       // REQUEST PREPARATION & VALIDATTION STATE
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      result = HTTPcSock_IsRxClosed(p_conn, &local_err);        // Check if connection half-closed by server.
      if (result == DEF_YES) {
        RTOS_ERR_SET(p_conn->ErrCode, RTOS_ERR_NONE);
        p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_SERVER;
        p_conn->State = HTTPc_CONN_STATE_COMPLETED;
      } else {
        if (p_req != DEF_NULL) {
          RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

          HTTPcConn_TransParamReset(p_conn);                    // Clear Connection internal parameters.
          HTTPcReq_Prepare(p_conn, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            p_conn->ErrCode = local_err;
            p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_NONE;
            p_conn->State = HTTPc_CONN_STATE_ERR;
          } else {
            p_conn->State = HTTPc_CONN_STATE_REQ_LINE_METHOD;
          }
        }
      }
      break;

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
    case HTTPc_CONN_STATE_REQ_END:                              // REQUEST PROCESSING STATES
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      (void)HTTPcReq(p_conn, &local_err);
      switch (RTOS_ERR_CODE_GET(local_err)) {
        case RTOS_ERR_NONE:
        case RTOS_ERR_POOL_EMPTY:
        case RTOS_ERR_NET_IF_LINK_DOWN:
        case RTOS_ERR_TIMEOUT:
        case RTOS_ERR_WOULD_BLOCK:
          break;

        case RTOS_ERR_NET_CONN_CLOSE_RX:
        case RTOS_ERR_NET_CONN_CLOSED_FAULT:
          p_conn->ErrCode = local_err;
          p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_SERVER;
          p_conn->State = HTTPc_CONN_STATE_ERR;
          break;

        default:
          p_conn->ErrCode = local_err;
          p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_ERR_INTERNAL;
          p_conn->State = HTTPc_CONN_STATE_ERR;
          break;
      }
      break;

    case HTTPc_CONN_STATE_RESP_INIT:
    case HTTPc_CONN_STATE_RESP_STATUS_LINE:
    case HTTPc_CONN_STATE_RESP_HDR:
    case HTTPc_CONN_STATE_RESP_BODY:
    case HTTPc_CONN_STATE_RESP_BODY_CHUNK_SIZE:
    case HTTPc_CONN_STATE_RESP_BODY_CHUNK_DATA:
    case HTTPc_CONN_STATE_RESP_BODY_CHUNK_CRLF:
    case HTTPc_CONN_STATE_RESP_BODY_CHUNK_LAST:
    case HTTPc_CONN_STATE_RESP_COMPLETED:                       // RESPONSE PROCESSING STATES
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      success = HTTPcResp(p_conn, &local_err);
      switch (RTOS_ERR_CODE_GET(local_err)) {
        case RTOS_ERR_NONE:
        case RTOS_ERR_WOULD_BLOCK:
        case RTOS_ERR_TIMEOUT:
          break;

        case RTOS_ERR_NET_CONN_CLOSE_RX:
        case RTOS_ERR_NET_CONN_CLOSED_FAULT:
          p_conn->ErrCode = local_err;
          p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_SERVER;
          p_conn->State = HTTPc_CONN_STATE_ERR;
          break;

        default:
          p_conn->ErrCode = local_err;
          p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_ERR_INTERNAL;
          p_conn->State = HTTPc_CONN_STATE_ERR;
          break;
      }

      if (success == DEF_YES) {
        DEF_BIT_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_COMPLETE_OK);
      } else {
        DEF_BIT_CLR(p_conn->RespFlags, (HTTPc_FLAGS)HTTPc_FLAG_RESP_COMPLETE_OK);
      }
      break;

    case HTTPc_CONN_STATE_ERR:                                  // ERROR STATE
#ifdef HTTPc_TASK_MODULE_EN
      //                                                           Call callback function for Transaction Error.
      if (p_req != DEF_NULL) {
        if (p_req->OnErr != DEF_NULL) {
          p_req->OnErr(p_conn_const, p_req_const, p_conn->ErrCode);
        }
      } else {
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
#endif
      p_conn->State = HTTPc_CONN_STATE_COMPLETED;
      break;

    case HTTPc_CONN_STATE_COMPLETED:                            // TRANSACTION COMPLETE STATE
      if (p_req != DEF_NULL) {
        success = DEF_BIT_IS_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_COMPLETE_OK);
#ifdef  HTTPc_WEBSOCK_MODULE_EN
        //                                                         Check if it is a upgrade websocket request.
        result = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_UPGRADE_WEBSOCKET);
        if ((result == DEF_YES) && (success == DEF_YES)) {
          if ((p_req->WebSockPtr->Flags & HTTPc_FLAG_WEBSOCK_ALL) == HTTPc_FLAG_WEBSOCK_ALL) {
            if (p_req->RespPtr->StatusCode == HTTP_STATUS_SWITCHING_PROTOCOLS) {
              //                                                   The connection is upgrading to websocket.
              DEF_BIT_SET(p_conn->Flags, HTTPc_FLAG_CONN_WEBSOCKET);
              p_conn->WebSockPtr = p_req->WebSockPtr->WebSockObjPtr;
            }
          }
        }

        if (p_req->WebSockPtr != DEF_NULL) {
          HTTPc_Mem_WebSockReqRelease(p_req->WebSockPtr);
        }
#endif

        //                                                         ----------- REMOVE REQUEST FROM LIST ------------
        HTTPcConn_ReqRemove(p_conn);

#ifdef HTTPc_TASK_MODULE_EN
        //                                                         No-Blocking mode: notify app with callback that ...
        //                                                         ... transaction is done.
        no_block = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_NO_BLOCK);
        if ((no_block == DEF_YES)
            && (p_req->OnTransComplete != DEF_NULL)) {
          p_req->OnTransComplete(p_conn_const, p_req_const, p_req->RespPtr, success);
        }
#endif
      }

      to_close = HTTPcConn_TransComplete(p_conn);
      if (to_close == DEF_YES) {
        p_conn->State = HTTPc_CONN_STATE_CLOSE;
      } else {
#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
        //                                                         Blocking mode: Signal app that transaction is done.
        if (p_conn->TransDoneSignalPtr != DEF_NULL) {
          HTTPcTask_TransDoneSignal(p_conn);
        }
#endif
#ifdef  HTTPc_WEBSOCK_MODULE_EN
        is_websocket = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_WEBSOCKET);
        if (is_websocket == DEF_YES) {
          p_conn->State = HTTPc_CONN_STATE_WEBSOCK_INIT;
        } else {
          p_conn->State = HTTPc_CONN_STATE_PARAM_VALIDATE;
        }
#else
        p_conn->State = HTTPc_CONN_STATE_PARAM_VALIDATE;
#endif
      }
      break;

    case HTTPc_CONN_STATE_CLOSE:                                // CONNECTION CLOSE STATE
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      HTTPcConn_Close(p_conn, &local_err);                      // Close Connection.
      HTTPcConn_Remove(p_conn);                                 // Release Connection.

#ifdef HTTPc_TASK_MODULE_EN
      //                                                           Call callback function for Conn Close.
      if (p_conn->OnClose != DEF_NULL) {
        p_conn->OnClose(p_conn_const, p_conn->CloseStatus, p_conn->ErrCode);
      }

#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
      //                                                           Blocking mode: Signal app that transaction is done.
      if (p_conn->TransDoneSignalPtr != DEF_NULL) {
        HTTPcTask_TransDoneSignal(p_conn);
      }
      //                                                           Blocking mode: Signal App that Conn was closed.
      if (p_conn->CloseSignalPtr != DEF_NULL) {
        HTTPcTask_ConnCloseSignal(p_conn);
      }
#endif
#endif
      p_conn->State = HTTPc_CONN_STATE_NONE;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
  }

  PP_UNUSED_PARAM(p_conn_const);
  PP_UNUSED_PARAM(p_req_const);
}

/****************************************************************************************************//**
 *                                      HTTPcConn_TransParamReset()
 *
 * @brief    Clear HTTP Connection object members related to internal usage during a HTTP transaction.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *******************************************************************************************************/
void HTTPcConn_TransParamReset(HTTPc_CONN *p_conn)
{
  //                                                               ------------ INIT CONNECTION PARAMETERS ------------
  RTOS_ERR_SET(p_conn->ErrCode, RTOS_ERR_NONE);
  p_conn->SockFlags = DEF_BIT_NONE;
  p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_NONE;
  DEF_BIT_CLR(p_conn->Flags, (HTTPc_FLAGS)HTTPc_FLAG_CONN_TO_CLOSE);

  //                                                               ------------- INIT REQUEST PARAMETERS --------------
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
  DEF_BIT_CLR(p_conn->RespFlags, (HTTPc_FLAGS)HTTPc_FLAG_RESP_RX_MORE_DATA);
  DEF_BIT_CLR(p_conn->RespFlags, (HTTPc_FLAGS)HTTPc_FLAG_RESP_BODY_CHUNK_TRANSFER);

  //                                                               --------- INIT CONNECTION BUFFER PARAMETERS --------
  p_conn->RxBufPtr = p_conn->BufPtr;
  p_conn->RxDataLenRem = 0u;
  p_conn->RxDataLen = 0u;
  p_conn->TxBufPtr = p_conn->BufPtr;
  p_conn->TxDataLen = 0u;
  p_conn->TxDataPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                         HTTPcConn_TransComplete()
 *
 * @brief    - (1) HTTP Transaction complete operations :
 *               - (a) Check if connection must be closed.
 *               - (b) Remove connection from list.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @return   DEF_YES, if connection must be close.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPcConn_TransComplete(HTTPc_CONN *p_conn)
{
  CPU_BOOLEAN close_hdr;
  CPU_BOOLEAN is_connect;
  CPU_BOOLEAN to_close;
#ifdef  HTTPc_WEBSOCK_MODULE_EN
  CPU_BOOLEAN is_websocket;
#endif

  to_close = DEF_NO;

  close_hdr = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_TO_CLOSE);
  is_connect = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_CONNECT);
  if ((close_hdr == DEF_YES)
      && (is_connect == DEF_YES)) {
    to_close = DEF_YES;
  }
#ifdef  HTTPc_WEBSOCK_MODULE_EN
  is_websocket = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_WEBSOCKET);
  if (is_websocket == DEF_YES) {
    to_close = DEF_NO;
  }
#endif
  if (p_conn->CloseStatus != HTTPc_CONN_CLOSE_STATUS_NONE) {
    to_close = DEF_YES;
  }

  return (to_close);
}

/****************************************************************************************************//**
 *                                            HTTPcConn_Add()
 *
 * @brief    - (1) Add current HTTPc Connection to connection list if the task is enabled.
 *           - (2) Tag Connection as being used.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *******************************************************************************************************/
void HTTPcConn_Add(HTTPc_CONN *p_conn)
{
  DEF_BIT_SET(p_conn->Flags, HTTPc_FLAG_CONN_IN_USE);

#ifdef HTTPc_TASK_MODULE_EN
  HTTPcTask_ConnAdd(p_conn);
#endif

  p_conn->State = HTTPc_CONN_STATE_CONNECT;
}

/****************************************************************************************************//**
 *                                          HTTPcConn_Remove()
 *
 * @brief    - (1) Remove current HTTPc Connection from connection list if the task is enabled.
 *           - (2) Release Connection.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *******************************************************************************************************/
void HTTPcConn_Remove(HTTPc_CONN *p_conn)
{
#ifdef HTTPc_TASK_MODULE_EN
  HTTPcTask_ConnRemove(p_conn);
#endif

  DEF_BIT_CLR(p_conn->Flags, (HTTPc_FLAGS)HTTPc_FLAG_CONN_IN_USE);
}

/****************************************************************************************************//**
 *                                          HTTPcConn_ReqAdd()
 *
 * @brief    Add new Request to the Connection Request list.
 *
 * @param    p_req   Pointer to the HTTPc Request.
 *
 * @note     (1) New Request is added at the end of the list.
 *******************************************************************************************************/
void HTTPcConn_ReqAdd(HTTPc_REQ *p_req)
{
  HTTPc_CONN  *p_conn;
  HTTPc_REQ   *p_req_item;
  CPU_BOOLEAN in_use;

  p_conn = p_req->ConnPtr;

  in_use = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_IN_USE);

  if (in_use == DEF_YES) {
    DEF_BIT_SET(p_req->Flags, HTTPc_FLAG_REQ_IN_USE);

    if (p_conn->ReqListHeadPtr == DEF_NULL) {
      p_conn->ReqListHeadPtr = p_req;
      p_conn->ReqListEndPtr = p_req;
      p_req->NextPtr = 0;
    } else {
      p_req_item = p_conn->ReqListEndPtr;
      p_req_item->NextPtr = p_req;
      p_conn->ReqListEndPtr = p_req;
      p_req->NextPtr = 0;
    }
  }
}

/****************************************************************************************************//**
 *                                         HTTPcConn_ReqRemove()
 *
 * @brief    Remove the first Request from the Connection Request list.
 *
 * @param    p_conn  Pointer to the HTTPc Connection.
 *
 * @note     (1) The Head of the Request list is removed.
 *******************************************************************************************************/
void HTTPcConn_ReqRemove(HTTPc_CONN *p_conn)
{
  HTTPc_REQ *p_req_item;

  p_req_item = p_conn->ReqListHeadPtr;

  if (p_req_item == DEF_NULL) {
    p_conn->ReqListEndPtr = DEF_NULL;
  } else if (p_req_item->NextPtr == DEF_NULL) {
    p_conn->ReqListHeadPtr = DEF_NULL;
    p_conn->ReqListEndPtr = DEF_NULL;
    p_req_item->NextPtr = 0;
  } else {
    p_conn->ReqListHeadPtr = p_req_item->NextPtr;
    p_req_item->NextPtr = 0;
  }

  if (p_req_item != DEF_NULL) {
    DEF_BIT_CLR(p_req_item->Flags, (HTTPc_FLAGS)HTTPc_FLAG_REQ_IN_USE);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_CLIENT_AVAIL
