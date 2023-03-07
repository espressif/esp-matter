/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Connection Module
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

#if (defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error HTTP Server Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "http_server_priv.h"

#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void HTTPsConn_Close(HTTPs_INSTANCE *p_instance,
                            HTTPs_CONN     *p_conn);

static void HTTPsConn_ErrInternal(HTTPs_INSTANCE *p_instance,
                                  HTTPs_CONN     *p_conn);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          HTTPsConn_Process()
 *
 * @brief    (1) Process each accepted connection:
 *               - (a) Receive, transmit, process data or close connection, if the connection is ready.
 *               - (b) Update connection state, parse received data or prepare data to transmit.
 *
 * @param    p_instance  Pointer to the instance.
 *******************************************************************************************************/
void HTTPsConn_Process(HTTPs_INSTANCE *p_instance)
{
  const HTTPs_CFG *p_cfg;
  HTTPs_CONN      *p_conn;
  HTTPs_CONN      *p_conn_next;
  CPU_BOOLEAN     done;
  CPU_BOOLEAN     hook_def;
  CPU_BOOLEAN     process;
  CPU_BOOLEAN     rdy_rd;
  CPU_BOOLEAN     rdy_wr;
  CPU_BOOLEAN     rdy_err;
#if (HTTPs_CFG_PERSISTENT_CONN_EN == DEF_ENABLED)
  CPU_BOOLEAN persistent;
#endif

  p_cfg = p_instance->CfgPtr;
  p_conn = p_instance->ConnFirstPtr;

  while (p_conn != DEF_NULL) {                                  // For each accepted conn.
    rdy_rd = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPs_FLAG_SOCK_RDY_RD);
    rdy_wr = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPs_FLAG_SOCK_RDY_WR);
    rdy_err = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPs_FLAG_SOCK_RDY_ERR);

    p_conn_next = p_conn->ConnNextPtr;                          // Req'd since cur conn can be closed.

    if ((rdy_rd == DEF_YES)                                     // If conn sock rdy.
        || (rdy_wr == DEF_YES)
        || (rdy_err == DEF_YES)) {
      //                                                           ---------------- CONN SOCK PROCESS -----------------
      switch (p_conn->SockState) {
        case HTTPs_SOCK_STATE_NONE:                             // No data to rx or tx.
          process = DEF_YES;
          break;

        case HTTPs_SOCK_STATE_RX:                               // Rx data.
          process = HTTPsSock_ConnDataRx(p_instance, p_conn);
          break;

        case HTTPs_SOCK_STATE_TX:                               // Tx data from buf.
          process = HTTPsSock_ConnDataTx(p_instance, p_conn);
          break;

        case HTTPs_SOCK_STATE_ERR:                              // Fatal err.
        case HTTPs_SOCK_STATE_CLOSE:                            // Transaction completed.
        default:
          HTTPsConn_Close(p_instance, p_conn);
          process = DEF_NO;
          break;
      }

      //                                                           ------------ UPDATE CONN & PREPARE DATA ------------
      if (process == DEF_YES) {
        switch (p_conn->State) {
          case HTTPs_CONN_STATE_REQ_INIT:                       // Receive and parse request.
          case HTTPs_CONN_STATE_REQ_PARSE_METHOD:
          case HTTPs_CONN_STATE_REQ_PARSE_URI:
          case HTTPs_CONN_STATE_REQ_PARSE_QUERY_STRING:
          case HTTPs_CONN_STATE_REQ_PARSE_PROTOCOL_VERSION:
          case HTTPs_CONN_STATE_REQ_PARSE_HDR:
          case HTTPs_CONN_STATE_REQ_LINE_HDR_HOOK:
            HTTPsReq_Handler(p_instance, p_conn);
            break;

          case HTTPs_CONN_STATE_REQ_BODY_INIT:                  // Process request body.
          case HTTPs_CONN_STATE_REQ_BODY_DATA:
          case HTTPs_CONN_STATE_REQ_BODY_FLUSH_DATA:
          case HTTPs_CONN_STATE_REQ_BODY_FORM_APP_PARSE:
          case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_INIT:
          case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_PARSE:
          case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_OPEN:
          case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_WR:
            HTTPsReq_Body(p_instance, p_conn);
            break;

          //                                                       Prepare response.
          case HTTPs_CONN_STATE_REQ_READY_SIGNAL:
          case HTTPs_CONN_STATE_REQ_READY_POLL:
            done = HTTPsReq_RdySignal(p_instance, p_conn);
            if (done == DEF_YES) {
              p_conn->State = HTTPs_CONN_STATE_RESP_PREPARE;
            }
            break;

          case HTTPs_CONN_STATE_RESP_PREPARE:
            done = HTTPsResp_Prepare(p_instance, p_conn);
            if (done == DEF_YES) {
              p_conn->State = HTTPs_CONN_STATE_RESP_INIT;
            }
            break;

          case HTTPs_CONN_STATE_RESP_INIT:                      // Build and transmit response.
          case HTTPs_CONN_STATE_RESP_TOKEN:
          case HTTPs_CONN_STATE_RESP_STATUS_LINE:
          case HTTPs_CONN_STATE_RESP_HDR:
          case HTTPs_CONN_STATE_RESP_HDR_CONTENT_TYPE:
          case HTTPs_CONN_STATE_RESP_HDR_FILE_TRANSFER:
          case HTTPs_CONN_STATE_RESP_HDR_LOCATION:
          case HTTPs_CONN_STATE_RESP_HDR_CONN:
          case HTTPs_CONN_STATE_RESP_HDR_LIST:
          case HTTPs_CONN_STATE_RESP_HDR_TX:
          case HTTPs_CONN_STATE_RESP_HDR_END:
          case HTTPs_CONN_STATE_RESP_FILE_STD:
          case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED:
          case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_TX_TOKEN:
          case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_TX_LAST_CHUNK:
          case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_HOOK:
          case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_FINALIZE:
          case HTTPs_CONN_STATE_RESP_COMPLETED:
            done = HTTPsResp_Handle(p_instance, p_conn);
            if (done == DEF_YES) {
              p_conn->State = HTTPs_CONN_STATE_COMPLETED;
            }
            break;

          case HTTPs_CONN_STATE_COMPLETED:                      // Transaction completed.
#if (HTTPs_CFG_PERSISTENT_CONN_EN == DEF_ENABLED)
            persistent = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_CONN_PERSISTENT);
            if ((p_cfg->ConnPersistentEn == DEF_ENABLED)
                && (persistent == DEF_YES)    ) {
              HTTPsMem_ConnClr(p_instance, p_conn);
              p_conn->SockState = HTTPs_SOCK_STATE_RX;
              p_conn->State = HTTPs_CONN_STATE_REQ_INIT;
            } else {
              p_conn->SockState = HTTPs_SOCK_STATE_CLOSE;
            }
#else
            p_conn->SockState = HTTPs_SOCK_STATE_CLOSE;
#endif
            hook_def = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnTransCompleteHook);
            if (hook_def == DEF_YES) {
              p_cfg->HooksPtr->OnTransCompleteHook(p_instance, p_conn, p_cfg->Hooks_CfgPtr);
            }
            break;

          case HTTPs_CONN_STATE_ERR_INTERNAL:
            HTTPsConn_ErrInternal(p_instance, p_conn);
            break;

          case HTTPs_CONN_STATE_UNKNOWN:
            p_conn->State = HTTPs_CONN_STATE_ERR_FATAL;
            p_conn->ErrCode = HTTPs_ERR_STATE_UNKNOWN;
            break;

          case HTTPs_CONN_STATE_ERR_FATAL:                      // Fatal err.
          default:
            p_conn->SockState = HTTPs_SOCK_STATE_CLOSE;
            break;
        }
      }
    }

    p_conn = p_conn_next;                                       // Move to next accepted conn.
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           HTTPsConn_Close()
 *
 * @brief    (1) Close connection.
 *               - (a) Close socket.
 *               - (b) Close timer
 *               - (c) Release connection structure.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *******************************************************************************************************/
static void HTTPsConn_Close(HTTPs_INSTANCE *p_instance,
                            HTTPs_CONN     *p_conn)
{
  const HTTPs_CFG     *p_cfg;
  CPU_BOOLEAN         hook_def;
  HTTPs_INSTANCE_ERRS *p_ctr_err = DEF_NULL;

  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
  if (p_conn->TokenCtrlPtr != DEF_NULL) {
    HTTPsMem_TokenRelease(p_instance, p_conn);
    HTTPs_ERR_INC(p_ctr_err->Resp_ErrTokenCloseNotEmptyCtr);
  }
#endif

#if (HTTPs_CFG_QUERY_STR_EN == DEF_ENABLED)
  HTTPsMem_QueryStrKeyValBlkReleaseAll(p_instance, p_conn);
#endif

#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
  HTTPsMem_FormKeyValBlkReleaseAll(p_instance, p_conn);
#endif

  p_cfg = p_instance->CfgPtr;

  hook_def = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnConnCloseHook);
  if (hook_def == DEF_YES) {
    p_cfg->HooksPtr->OnConnCloseHook(p_instance,
                                     p_conn,
                                     p_cfg->Hooks_CfgPtr);
  }

  //                                                               -------------------- CLOSE SOCK --------------------
  HTTPsSock_ConnClose(p_instance, p_conn);

  //                                                               ------------------ CLOSE FS FILE -------------------
  HTTPsResp_DataComplete(p_instance, p_conn);

#if ((HTTPs_CFG_HDR_RX_EN == DEF_ENABLED) \
  || (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED))

  switch (p_conn->HdrType) {
    case HTTPs_HDR_TYPE_REQ:
      //                                                           ---------- VALIDATE REQ HDR POOL IS EMPTY ----------
#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
      while (p_conn->HdrListPtr != DEF_NULL) {
        HTTPsMem_ReqHdrRelease(p_instance,
                               p_conn);
      }
#endif
      break;

    case HTTPs_HDR_TYPE_RESP:
      //                                                           --------- VALIDATE RESP HDR POOL IS EMPTY ----------
#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
      while (p_conn->HdrListPtr != DEF_NULL) {
        HTTPsMem_RespHdrRelease(p_instance,
                                p_conn);
      }
#endif
      break;

    default:
      HTTPs_ERR_INC(p_ctr_err->Conn_ErrHdrTypeInvalidCtr);
      break;
  }

#endif

  PP_UNUSED_PARAM(p_ctr_err);

  //                                                               ------------------- RELEASE CONN -------------------
  HTTPsMem_ConnRelease(p_instance, p_conn);
}

/****************************************************************************************************//**
 *                                          HTTPsConn_ErrInternal()
 *
 * @brief    (1) Internal error handler
 *               - (a) Make sure that all data has been read
 *               - (b) Update connection states and status code
 *               - (c) Notify application about the internal error.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *******************************************************************************************************/
static void HTTPsConn_ErrInternal(HTTPs_INSTANCE *p_instance,
                                  HTTPs_CONN     *p_conn)
{
  const HTTPs_CFG     *p_cfg;
  HTTPs_INSTANCE_ERRS *p_ctr_err = DEF_NULL;
  CPU_BOOLEAN         result;

  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  p_cfg = p_instance->CfgPtr;

  if (p_conn->ReqContentLen > 0) {
    //                                                             -------------- READ ALL RECEIVED DATA --------------
    p_conn->ReqContentLenRxd += p_conn->RxBufLenRem;
    p_conn->RxBufLenRem = 0u;

    if (p_conn->ReqContentLenRxd < p_conn->ReqContentLen) {     // If not received all data.
      p_conn->SockState = HTTPs_SOCK_STATE_RX;
      return;
    }
  }

  //                                                               ----------------- SET CONN STATES ------------------
  p_conn->State = HTTPs_CONN_STATE_RESP_PREPARE;
  p_conn->SockState = HTTPs_SOCK_STATE_NONE;

  switch (p_conn->ErrCode) {
    case HTTPs_ERR_REQ_METHOD_NOT_SUPPORTED:
      HTTPs_ERR_INC(p_ctr_err->ErrInternal_ReqMethodNotSupported);
      p_conn->StatusCode = HTTP_STATUS_NOT_IMPLEMENTED;
      break;

    case HTTPs_ERR_REQ_FORMAT_INVALID:
      HTTPs_ERR_INC(p_ctr_err->ErrInternal_ReqFormatInvalid);
      p_conn->StatusCode = HTTP_STATUS_BAD_REQUEST;
      break;

    case HTTPs_ERR_REQ_URI_LEN:
      HTTPs_ERR_INC(p_ctr_err->ErrInternal_ReqURI_Len);
      p_conn->StatusCode = HTTP_STATUS_REQUEST_URI_TOO_LONG;
      break;

    case HTTPs_ERR_REQ_PROTOCOL_VER_NOT_SUPPORTED:
      HTTPs_ERR_INC(p_ctr_err->ErrInternal_ReqProtocolNotSupported);
      p_conn->StatusCode = HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED;
      break;

    case HTTPs_ERR_REQ_MORE_DATA_REQUIRED:
      HTTPs_ERR_INC(p_ctr_err->ErrInternal_ReqMoreDataRequired);
      p_conn->StatusCode = HTTP_STATUS_BAD_REQUEST;
      break;

    case HTTPs_ERR_REQ_HDR_OVERFLOW:
      HTTPs_ERR_INC(p_ctr_err->ErrInternal_ReqHdrOverflow);
      p_conn->StatusCode = HTTP_STATUS_INTERNAL_SERVER_ERR;
      break;

    case HTTPs_ERR_FORM_FORMAT_INV:
      HTTPs_ERR_INC(p_ctr_err->ErrInternal_ReqBodyFormFormatInvalid);
      p_conn->StatusCode = HTTP_STATUS_INTERNAL_SERVER_ERR;
      break;

    case HTTPs_ERR_FORM_FILE_UPLOAD_OPEN:
      HTTPs_ERR_INC(p_ctr_err->ErrInternal_ReqBodyFormFileUploadOpen);
      p_conn->StatusCode = HTTP_STATUS_INTERNAL_SERVER_ERR;
      break;

    case HTTPs_ERR_KEY_VAL_CFG_POOL_SIZE_INV:
      HTTPs_ERR_INC(p_ctr_err->ErrInternal_ReqKeyValPoolSizeInvalid);
      p_conn->StatusCode = HTTP_STATUS_INTERNAL_SERVER_ERR;
      break;

    case HTTPs_ERR_STATE_UNKNOWN:
      HTTPs_ERR_INC(p_ctr_err->ErrInternal_StateUnknown);
      p_conn->StatusCode = HTTP_STATUS_INTERNAL_SERVER_ERR;
      break;

    default:
      HTTPs_ERR_INC(p_ctr_err->ErrInternal_Unknown);
      p_conn->StatusCode = HTTP_STATUS_INTERNAL_SERVER_ERR;
      break;
  }

  //                                                               --------------- NOTIFY APP ABOUT ERR ---------------
  result = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnErrHook);
  if (result == DEF_TRUE) {                                     // If err handler fnct is not null ...
    p_cfg->HooksPtr->OnErrHook(p_instance,                      // ... call cfg err fnct handler.
                               p_conn,
                               p_cfg->Hooks_CfgPtr,
                               p_conn->ErrCode);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
