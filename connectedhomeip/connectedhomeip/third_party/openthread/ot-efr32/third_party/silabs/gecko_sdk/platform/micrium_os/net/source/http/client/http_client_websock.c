/***************************************************************************//**
 * @file
 * @brief Network - HTTP Client Websocket Module
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

#include  "http_client_priv.h"
#include  "http_client_websock_priv.h"

#include  "../../tcpip/net_util_priv.h"
#include  "../../util/net_base64_priv.h"

#include  <common/include/lib_str.h>
#include  <common/include/lib_math.h>
#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  HTTPc_WEBSOCK_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

#define  HTTPc_WEBSOCK_TX_REQ_Q_NAME              "WebSocket Tx Req Q name"

#define  HTTPc_WEBSOCK_PROTOCOL_ERR_MSG           "\x03\xEAProtocolError"

#define  HTTPc_WEBSOCK_MIN_HEADER_LEN             2u
#define  HTTPc_WEBSOCK_MAX_HEADER_LEN             14u

#define  HTTPc_WEBSOCK_FIN_BIT                    DEF_BIT_07
#define  HTTPc_WEBSOCK_MASK_BIT                   DEF_BIT_07
#define  HTTPc_WEBSOCK_OPCODE_MASK                DEF_NIBBLE_MASK
#define  HTTPc_WEBSOCK_PAYLOAD_MASK               0x7F

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#define  HTTPc_WEBSOCK_GET_TX_BUF_SIZE(p_conn)   (p_conn->BufLen - p_conn->TxDataLen - (p_conn->TxBufPtr - p_conn->BufPtr))

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void HTTPcWebSock_TX(HTTPc_CONN *p_conn);

static void HTTPcWebSock_RX(HTTPc_CONN *p_conn);

static CPU_INT32U HTTPcWebSock_GetMsgHdr(HTTPc_CONN *p_conn,
                                         RTOS_ERR   *p_err);

static void HTTPcWebSock_GetPayload(HTTPc_CONN *p_conn);

static void HTTPcWebSock_SetTxBuf(HTTPc_WEBSOCK_MSG *p_msg,
                                  CPU_CHAR          *p_dst_buf,
                                  CPU_CHAR          *p_src_buf,
                                  CPU_INT32U        len);

static void HTTPcWebSock_SetTxBufHdr(HTTPc_CONN        *p_conn,
                                     HTTPc_WEBSOCK_MSG *p_tx_msg,
                                     CPU_CHAR          *p_tx_buf);

static void HTTPcWebSock_SetCtrlResponse(HTTPc_CONN           *p_conn,
                                         HTTPc_WEBSOCK_OPCODE opcode,
                                         CPU_CHAR             *p_data,
                                         CPU_INT32U           msg_len);

static void HTTPcWebSock_TxMsgRemove(HTTPc_CONN *p_conn);

static void HTTPcWebSock_TxMsgCleanList(HTTPc_CONN *p_conn);

static CPU_INT16U HTTPcWebSock_ComputeHdrLen(HTTPc_WEBSOCK_MSG *p_msg);

static CPU_BOOLEAN HTTPcWebSock_IsProtocolError(RTOS_ERR err);

static CPU_BOOLEAN HTTPcWebSock_IsStandardCloseCode(HTTPc_WEBSOCK_CLOSE_CODE close_code);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                      HTTPcWebSock_Process()
 *
 * @brief    Process all the states for a HTTP connection that has been upgraded to a WebSocket.
 *
 * @param    p_conn  Pointer to current HTTPc Connection (WebSocket Upgraded).
 *******************************************************************************************************/
void HTTPcWebSock_Process(HTTPc_CONN *p_conn)
{
  HTTPc_CONN_OBJ *p_conn_const;
  HTTPc_WEBSOCK  *p_ws;
  CPU_BOOLEAN    is_protocol_err;

  p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
  p_ws = p_conn->WebSockPtr;

  switch (p_conn->State) {
    case HTTPc_CONN_STATE_WEBSOCK_INIT:
      //                                                           WebSocket OnOpen Hook.
      if (p_ws->OnOpen != DEF_NULL) {
        p_ws->OnOpen(p_conn_const);
      }
      //                                                           Set the WebSocket initial state.
      p_ws->TxState = HTTPc_WEBSOCK_TX_STATE_MSG_INIT;
      p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_WAIT;
      p_conn->State = HTTPc_CONN_STATE_WEBSOCK_RXTX;
      break;

    case HTTPc_CONN_STATE_WEBSOCK_RXTX:

      HTTPcWebSock_TX(p_conn);                                  // Process Tx states of the WebSocket.
      HTTPcWebSock_RX(p_conn);                                  // Process Rx states of the WebSocket.
      break;

    case HTTPc_CONN_STATE_WEBSOCK_ERR:
      //                                                           WebSocket OnErr hook.
      if (p_ws->OnErr != DEF_NULL) {
        p_ws->OnErr(p_conn_const, p_conn->ErrCode);
      }

      is_protocol_err = HTTPcWebSock_IsProtocolError(p_conn->ErrCode);
      if (is_protocol_err == DEF_YES) {                         // Send a Close Frame if the Err Code a Protocol Error.
        p_ws->CloseCode = HTTPc_WEBSOCK_CLOSE_CODE_PROTOCOL_ERR;

        if (p_ws->Flags.IsTxMsgCtrlUsed == DEF_NO) {
          HTTPcWebSock_SetCtrlResponse(p_conn,
                                       HTTPc_WEBSOCK_OPCODE_CLOSE,
                                       HTTPc_WEBSOCK_PROTOCOL_ERR_MSG,
                                       sizeof(HTTPc_WEBSOCK_PROTOCOL_ERR_MSG));

          p_ws->Flags.IsCloseStarted = DEF_YES;
          p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_WAIT;
          p_conn->State = HTTPc_CONN_STATE_WEBSOCK_RXTX;
        } else {                                                // If the ctrl msg is already used, close the conn.
          p_conn->State = HTTPc_CONN_STATE_WEBSOCK_CLOSE;
        }
      } else {
        p_conn->State = HTTPc_CONN_STATE_WEBSOCK_CLOSE;
      }
      break;

    case HTTPc_CONN_STATE_WEBSOCK_CLOSE:
      //                                                           WebSocket OnClose hook.
      if (p_ws->OnClose != DEF_NULL) {
        p_ws->OnClose(p_conn_const, p_ws->CloseCode, &p_ws->CloseReason);
      }

      HTTPcWebSock_TxMsgCleanList(p_conn);                      // Empty the Tx Msg request pending list.
                                                                // Change state to remove and close the conn.
      p_conn->State = HTTPc_CONN_STATE_CLOSE;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
  }
}

/****************************************************************************************************//**
 *                                       HTTPcWebSock_InitReqObj()
 *
 * @brief    Initialize the WebSocket Request object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   If the operation is successful, pointer to the WebSocket Request object to initialize.
 *           If the operation has failed, DEF_NULL;
 *******************************************************************************************************/
HTTPc_WEBSOCK_REQ *HTTPcWebSock_InitReqObj(RTOS_ERR *p_err)
{
  CPU_INT32U        val;
  CPU_CHAR          tmp_buf[HTTPc_WEBSOCK_KEY_PRE_HASH_LEN];
  CPU_INT08U        i;
  NET_SHA1_CTX      sha1_ctx;
  HTTPc_WEBSOCK_REQ *p_ws_req;

  //                                                               ------------- CREATE RAMDOM 16 BYTES ---------------
  //                                                               Reset the WebSocket handshake obj.
  p_ws_req = HTTPc_Mem_WebSockReqGet(p_err);
  if (p_ws_req == DEF_NULL) {
    goto exit;
  }
  //                                                               Generate a Random Seed.

  val = NetUtil_RandomRangeGet(1u, DEF_INT_32U_MAX_VAL);
  //                                                               Create the 16-byte random-key.
  for (i = 0; i < HTTPc_WEBSOCK_KEY_LEN; i += sizeof(CPU_INT32U)) {
    val = Math_RandSeed(val);
    Mem_Copy(&tmp_buf[i], &val, sizeof(CPU_INT32U));
  }

  //                                                               --------- PROCESS SEC-WEBSOCKET-KEY VALUE ----------
  NetBase64_Encode(tmp_buf,
                   HTTPc_WEBSOCK_KEY_LEN,
                   p_ws_req->Key,
                   HTTPc_WEBSOCK_KEY_ENCODED_LEN,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }
  //                                                               -------- PROCESS SEC-WEBSOCKET-ACCEPT VALUE --------
  Str_Copy(tmp_buf, p_ws_req->Key);
  Str_Cat(tmp_buf, HTTPc_WEBSOCK_GUID_STRING);

  NetSHA1_Reset(&sha1_ctx,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  NetSHA1_Input(&sha1_ctx,
                tmp_buf,
                HTTPc_WEBSOCK_KEY_PRE_HASH_LEN,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  NetSHA1_Result(&sha1_ctx,
                 tmp_buf,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  NetBase64_Encode(tmp_buf,
                   HTTPc_WEBSOCK_KEY_HASH_LEN,
                   p_ws_req->Accept,
                   HTTPc_WEBSOCK_KEY_HASH_ENCODED_LEN,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit:
  return (p_ws_req);

exit_release:
  HTTPc_Mem_WebSockReqRelease(p_ws_req);
  return (DEF_NULL);
}

/****************************************************************************************************//**
 *                                        HTTPcWebSock_TxMsgAdd()
 *
 * @brief    Add a message in the TX message queue.
 *
 * @param    p_msg   Pointer to the message to queue
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void HTTPcWebSock_TxMsgAdd(HTTPc_WEBSOCK_MSG *p_msg,
                           RTOS_ERR          *p_err)
{
  HTTPc_CONN        *p_conn;
  HTTPc_WEBSOCK     *p_ws;
  HTTPc_WEBSOCK_MSG *p_msg_item;
  CPU_BOOLEAN       in_use;

  p_conn = p_msg->ConnPtr;
  p_ws = p_conn->WebSockPtr;

  in_use = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_IN_USE);

  if (in_use == DEF_YES) {
    if (p_ws->TxMsgListHeadPtr == DEF_NULL) {
      p_ws->TxMsgListHeadPtr = p_msg;
      p_ws->TxMsgListEndPtr = p_msg;
      p_msg->NextPtr = 0;
    } else {
      p_msg_item = p_ws->TxMsgListEndPtr;
      p_msg_item->NextPtr = p_msg;
      p_ws->TxMsgListEndPtr = p_msg;
      p_msg->NextPtr = 0;
    }

    p_msg->Flags.IsUsed = DEF_YES;

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_INVALID_CONN);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          HTTPcWebSock_TX()
 *
 * @brief    Process the WebSocket states TX.
 *
 * @param    p_conn  Pointer to current HTTPc Connection (WebSocket Upgraded).
 *******************************************************************************************************/
static void HTTPcWebSock_TX(HTTPc_CONN *p_conn)
{
  HTTPc_WEBSOCK            *p_ws;
  HTTPc_WEBSOCK_MSG        *p_msg;
  CPU_BOOLEAN              done;
  HTTPc_CONN_OBJ           *p_conn_const;
  HTTPc_WEBSOCK_MSG_OBJ    *p_msg_const;
  CPU_INT16U               hdr_len;
  CPU_INT16U               buf_size;
  CPU_INT32U               len;
  CPU_INT32U               len_rem;
  CPU_CHAR                 *p_src_buf;
  CPU_BOOLEAN              sock_rdy_wr;
  CPU_BOOLEAN              sock_rdy_err;
  CPU_CHAR                 *p_tx_buf;
  HTTPc_WEBSOCK_CLOSE_CODE *p_close_code;
  RTOS_ERR                 local_err;

  //                                                               ------------------ INITIALIZAITON ------------------
  p_ws = p_conn->WebSockPtr;
  p_msg = p_ws->TxMsgListHeadPtr;

  if (p_conn->State != HTTPc_CONN_STATE_WEBSOCK_RXTX) {
    return;
  }

  if (p_msg != DEF_NULL) {
    do {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      done = DEF_YES;                                           // Always set to avoid unhandled state.
      switch (p_ws->TxState) {
        case HTTPc_WEBSOCK_TX_STATE_MSG_INIT:
          //                                                       On Msg Tx Initialization Hook.
          if (p_msg->OnMsgTxInit != DEF_NULL) {
            p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
            p_msg_const = (HTTPc_WEBSOCK_MSG_OBJ *)p_msg;
            p_msg->Len = p_msg->OnMsgTxInit(p_conn_const, p_msg_const);
          }
          done = DEF_NO;
          p_ws->TxState = HTTPc_WEBSOCK_TX_STATE_SET_BUF;
          break;

        case HTTPc_WEBSOCK_TX_STATE_SET_BUF:
          //                                                       Get hdr space needed.
          if (p_msg->Flags.IsHdrSet == DEF_NO) {
            hdr_len = HTTPcWebSock_ComputeHdrLen(p_msg);
            p_msg->MskKey = NetUtil_RandomRangeGet(1u, DEF_INT_32U_MAX_VAL);
          } else {
            hdr_len = 0;
          }
          //                                                       Compute buffer space available.
          buf_size = HTTPc_WEBSOCK_GET_TX_BUF_SIZE(p_conn);
          if (buf_size > hdr_len) {
            buf_size -= hdr_len;
          } else {
            done = DEF_YES;
            break;
          }
          //                                                       Check if sock rdy to wr to avoid using the conn...
          //                                                       ...buf unnecessarily.
          sock_rdy_wr = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPc_FLAG_SOCK_RDY_WR);
          if (sock_rdy_wr == DEF_NO) {
            done = DEF_YES;
            break;
          }
          //                                                       Check if On Msg Tx Data Hook is initialized.
          if (p_msg->OnMsgTxData != DEF_NULL) {
            p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
            p_msg_const = (HTTPc_WEBSOCK_MSG_OBJ *)p_msg;
            p_msg->DataLen = p_msg->OnMsgTxData(p_conn_const,
                                                p_msg_const,
                                                p_conn->TxBufPtr,
                                                buf_size);

            if (p_msg->Len == HTTPc_WEBSOCK_TX_MSG_LEN_NOT_DEFINED) {
              if (p_msg->DataLen == 0) {
                p_msg->Flags.IsFin = DEF_YES;
              } else {
                p_msg->Flags.IsFin = DEF_NO;
              }
              p_msg->Len = p_msg->DataLen;
            }
            p_src_buf = p_conn->TxBufPtr;
          } else {
            //                                                     Otherwise, use DataPtr.
            p_src_buf = (CPU_CHAR *)p_msg->DataPtr + p_msg->LenSent;
          }
          //                                                       Compute chunk len and TxDataPtr.
          len_rem = p_msg->Len - p_msg->LenSent;
          len = DEF_MIN(len_rem, buf_size);
          p_conn->TxDataPtr = p_conn->BufPtr + p_conn->BufLen - len;

          //                                                       Mask data and set Tx Buffer
          HTTPcWebSock_SetTxBuf(p_msg, (CPU_CHAR *)p_conn->TxDataPtr, p_src_buf, len);

          //                                                       Set the Hdr if needed.
          if (p_msg->Flags.IsHdrSet == DEF_NO) {
            hdr_len = HTTPcWebSock_ComputeHdrLen(p_msg);
            p_tx_buf = (CPU_CHAR *)p_conn->TxDataPtr;
            p_tx_buf -= hdr_len;
            HTTPcWebSock_SetTxBufHdr(p_conn,
                                     p_msg,
                                     p_tx_buf);
            p_conn->TxDataPtr = (void *)p_tx_buf;
            p_msg->Flags.IsHdrSet = DEF_YES;
          }

          p_msg->LenSent += len;
          p_conn->TxDataLen = len + hdr_len;
          p_ws->TxState = HTTPc_WEBSOCK_TX_STATE_SEND_BUF;
          done = DEF_NO;
          break;

        case HTTPc_WEBSOCK_TX_STATE_SEND_BUF:
          //                                                       Send Data available in the buffer.
          sock_rdy_wr = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPc_FLAG_SOCK_RDY_WR);
          sock_rdy_err = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPc_FLAG_SOCK_RDY_ERR);
          if ((sock_rdy_wr == DEF_YES)
              || (sock_rdy_err == DEF_YES)) {
            (void)HTTPcSock_ConnDataTx(p_conn, &local_err);
            switch (RTOS_ERR_CODE_GET(local_err)) {
              case RTOS_ERR_NONE:
              case RTOS_ERR_POOL_EMPTY:
              case RTOS_ERR_NET_IF_LINK_DOWN:
              case RTOS_ERR_TIMEOUT:
              case RTOS_ERR_WOULD_BLOCK:
                break;

              default:
                goto exit_err;
            }
          }

          if (p_conn->TxDataLen == 0u) {                        // If all the data available in the buf has been sent.
            if (p_msg->LenSent < p_msg->Len) {                  // Rtn to prev state if the frame is not completed.
              p_ws->TxState = HTTPc_WEBSOCK_TX_STATE_SET_BUF;
            } else if (p_msg->LenSent == p_msg->Len) {
              //                                                   Validate if it was the last frame of the message.
              if (p_msg->Flags.IsFin == DEF_YES) {
                p_ws->TxState = HTTPc_WEBSOCK_TX_STATE_COMPLETE;
                done = DEF_NO;
              } else {
                p_msg->OpCode = HTTPc_WEBSOCK_OPCODE_CONTINUATION_FRAME;
                p_msg->Flags.IsHdrSet = DEF_NO;
                p_msg->Len = HTTPc_WEBSOCK_TX_MSG_LEN_NOT_DEFINED;
                p_msg->LenSent = 0;
                p_ws->TxState = HTTPc_WEBSOCK_TX_STATE_SET_BUF;
              }
            } else {
              p_conn->ErrCode = local_err;
              p_conn->State = HTTPc_CONN_STATE_WEBSOCK_ERR;
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
            }
          }
          break;

        case HTTPc_WEBSOCK_TX_STATE_COMPLETE:

          p_ws->TxState = HTTPc_WEBSOCK_TX_STATE_MSG_INIT;
          p_msg->Flags.IsCompleted = DEF_YES;

          switch (p_msg->OpCode) {                              // Refresh WebSock states according to the msg types.
            case HTTPc_WEBSOCK_OPCODE_CONTINUATION_FRAME:
            case HTTPc_WEBSOCK_OPCODE_TXT_FRAME:
            case HTTPc_WEBSOCK_OPCODE_BIN_FRAME:
            case HTTPc_WEBSOCK_OPCODE_PING:
              break;

            case HTTPc_WEBSOCK_OPCODE_CLOSE:
              //                                                   Check if it was initiated by the server.
              if (p_ws->Flags.IsCloseStarted == DEF_YES) {
                //                                                 Set Close information.
                if (p_msg->Len >= sizeof(CPU_INT16U)) {
                  p_close_code = (CPU_INT16U *)p_msg->DataPtr;
                  p_ws->CloseCode = MEM_VAL_GET_INT16U_BIG(p_close_code);
                  p_ws->CloseReason.DataPtr = ((CPU_CHAR *)p_msg->DataPtr) + sizeof(CPU_INT16U);
                  p_ws->CloseReason.Len = p_msg->Len - HTTPc_WEBSOCK_CLOSE_CODE_LEN;
                } else {
                  p_ws->CloseCode = HTTPc_WEBSOCK_CLOSE_CODE_NONE;
                  p_ws->CloseReason.DataPtr = DEF_NULL;
                  p_ws->CloseReason.Len = 0u;
                }
                //                                                 Restore the Rx msg cached space.
                if (p_ws->Flags.IsRxDataCached == DEF_YES) {
                  p_conn->TxBufPtr = p_conn->BufPtr;
                  p_ws->Flags.IsRxDataCached = DEF_NO;
                }
                //                                                 Closing Handshake is completed.
                p_conn->State = HTTPc_CONN_STATE_WEBSOCK_CLOSE;
                p_ws->Flags.IsCloseStarted = DEF_NO;
              } else {                                          // Otherwise, wait for the close response from server.
                p_ws->Flags.IsCloseStarted = DEF_YES;
              }
              break;

            case HTTPc_WEBSOCK_OPCODE_PONG:
              //                                                   Check if the Pong msg was initiated by the server.
              if (p_ws->Flags.IsPongStarted == DEF_YES) {
                //                                                 Restore the Rx msg cached space.
                if (p_ws->Flags.IsRxDataCached == DEF_YES) {
                  p_conn->TxBufPtr = p_conn->BufPtr;
                  p_ws->Flags.IsRxDataCached = DEF_NO;
                }
                p_ws->Flags.IsPongStarted = DEF_NO;
              }
              break;

            default:
              p_conn->ErrCode = local_err;
              p_conn->State = HTTPc_CONN_STATE_WEBSOCK_ERR;
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
          }

          if (p_msg == &p_ws->TxMsgCtrl) {                      // Check If Tx Msg object is the internal obj for ctrl.
            p_ws->Flags.IsTxMsgCtrlUsed = DEF_NO;
            HTTPcWebSock_TxMsgRemove(p_conn);                   // Remove the Tx Msg obj in the list.
          } else {
            //                                                     On Msg Tx Complete Hook.
            if (p_msg->OnMsgTxComplete != DEF_NULL) {
              p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
              p_msg_const = (HTTPc_WEBSOCK_MSG_OBJ *)p_msg;
              p_msg->OnMsgTxComplete(p_conn_const, p_msg_const, DEF_OK);
            }

            HTTPcWebSock_TxMsgRemove(p_conn);                   // Remove the Tx Msg obj in the list.

#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
            //                                                     Signal that TX transaction is done.
            if (p_conn->TransDoneSignalPtr != DEF_NULL) {
              HTTPcTask_TransDoneSignal(p_conn);
            }
#endif
          }
          break;

        default:
          p_conn->ErrCode = local_err;
          p_conn->State = HTTPc_CONN_STATE_WEBSOCK_ERR;
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
      }
    } while (done == DEF_NO);
  }
  return;

exit_err:
  p_conn->ErrCode = local_err;
  p_conn->State = HTTPc_CONN_STATE_WEBSOCK_ERR;
}

/****************************************************************************************************//**
 *                                      HTTPcWebSock_RX()
 *
 * @brief    Process the WebSocket states for RX.
 *
 * @param    p_conn  Pointer to current HTTPc Connection (WebSocket Upgraded).
 *******************************************************************************************************/
static void HTTPcWebSock_RX(HTTPc_CONN *p_conn)
{
  HTTPc_WEBSOCK  *p_ws;
  CPU_BOOLEAN    sock_rdy_rd;
  CPU_BOOLEAN    sock_rdy_err;
  CPU_BOOLEAN    done;
  CPU_BOOLEAN    result;
  CPU_INT32U     rem_rx_len;
  CPU_INT32U     rx_len;
  HTTPc_CONN_OBJ *p_conn_const;
  CPU_INT32U     msg_len;
  CPU_BOOLEAN    is_std;
  CPU_INT16U     *p_close_code;
  RTOS_ERR       local_err;

  //                                                               ------------------ INITIALIZAITON ------------------
  p_ws = p_conn->WebSockPtr;

  if (p_conn->State != HTTPc_CONN_STATE_WEBSOCK_RXTX) {
    return;
  }

  do {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    done = DEF_YES;                                             // Always set to avoid unhandled state.
    rem_rx_len = 0u;
    //                                                             --------------- RX STATES PROCESSING ---------------
    switch (p_ws->RxState) {
      case HTTPc_WEBSOCK_RX_STATE_WAIT:

        if (p_conn->RxDataLenRem != 0) {                        // If there's data to process...
          p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_HDR;
          p_ws->RxMsgLenRead = 0u;
          p_conn->RxDataLen = 0u;
          done = DEF_NO;
          DEF_BIT_CLR(p_conn->RespFlags, (HTTPc_FLAGS)HTTPc_FLAG_RESP_RX_MORE_DATA);
        } else {                                                // Otherwise, try to rx WebSocket header.
          DEF_BIT_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_RX_MORE_DATA);
          rem_rx_len = HTTPc_WEBSOCK_MAX_HEADER_LEN;
        }
        break;

      case HTTPc_WEBSOCK_RX_STATE_HDR:                          // Process the msg header.

        rem_rx_len = HTTPcWebSock_GetMsgHdr(p_conn, &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          goto exit_err;
        }

        if (rem_rx_len == 0u) {                                 // Proceed to the next step if successful.
          p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_PREPARE;
          done = DEF_NO;
          DEF_BIT_CLR(p_conn->RespFlags, (HTTPc_FLAGS)HTTPc_FLAG_RESP_RX_MORE_DATA);
          //                                                       On Msg Rx Initialization Hook.
          if (p_ws->OnMsgRxInit != DEF_NULL) {
            if ((p_ws->RxMsgOpCode == HTTPc_WEBSOCK_OPCODE_TXT_FRAME)
                || (p_ws->RxMsgOpCode == HTTPc_WEBSOCK_OPCODE_BIN_FRAME)) {
              //                                                   Check if the msg is fragmented.
              if (p_ws->RxMsgFlags.IsFin == DEF_NO) {
                msg_len = HTTPc_WEBSOCK_TX_MSG_LEN_NOT_DEFINED;
              } else {
                msg_len = p_ws->RxMsgLen;
              }
              p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
              p_ws->RxMsgDataPtr = DEF_NULL;
              p_ws->OnMsgRxInit(p_conn_const, (HTTPc_WEBSOCK_MSG_TYPE)p_ws->RxMsgOpCode, msg_len, (void **)&(p_ws->RxMsgDataPtr));
            }
          }
        } else {                                                // If there is no enough data rx to process the hdr.
          DEF_BIT_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_RX_MORE_DATA);
        }
        break;

      case HTTPc_WEBSOCK_RX_STATE_PREPARE:

        rx_len = p_ws->RxMsgLenRead + p_conn->RxDataLenRem;

        if (rx_len < p_ws->RxMsgLen) {                          // Compute to nb of bytes to rx if needed.
          rem_rx_len = p_ws->RxMsgLen - rx_len;
          DEF_BIT_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_RX_MORE_DATA);
        } else {                                                // If the msg is already all rxd, process the payload.
          done = DEF_NO;
          DEF_BIT_CLR(p_conn->RespFlags, (HTTPc_FLAGS)HTTPc_FLAG_RESP_RX_MORE_DATA);
        }
        p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_PAYLOAD;
        break;

      case HTTPc_WEBSOCK_RX_STATE_PAYLOAD:                      // Process the rxd payload data.

        HTTPcWebSock_GetPayload(p_conn);

        DEF_BIT_CLR(p_conn->RespFlags, (HTTPc_FLAGS)HTTPc_FLAG_RESP_RX_MORE_DATA);
        if (p_ws->RxMsgLenRead < p_ws->RxMsgLen) {              // If the msg is not rxed completely.
          p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_PREPARE;
          //                                                       Continue to process if Ctrl msg.
          if ((p_ws->RxMsgOpCode == HTTPc_WEBSOCK_OPCODE_CLOSE)
              || (p_ws->RxMsgOpCode == HTTPc_WEBSOCK_OPCODE_PING)
              || (p_ws->RxMsgOpCode == HTTPc_WEBSOCK_OPCODE_PONG)) {
            done = DEF_NO;
          } else {                                              // Otherwise, save the remaining rx data and exit.
            p_conn->TxBufPtr = p_conn->RxBufPtr + p_conn->RxDataLenRem;
            done = DEF_YES;
          }
          //                                                       If the msg is rxed completely.
        } else if (p_ws->RxMsgLenRead == p_ws->RxMsgLen) {
          if (p_ws->Flags.IsRxDataCached == DEF_NO) {
            p_conn->TxBufPtr = p_conn->BufPtr;                  // Reset TxBufPtr.
          }
          p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_COMPLETE;
          done = DEF_NO;
        } else {
          p_conn->ErrCode = local_err;
          p_conn->State = HTTPc_CONN_STATE_WEBSOCK_ERR;
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        }
        break;

      case HTTPc_WEBSOCK_RX_STATE_COMPLETE:

        switch (p_ws->RxMsgOpCode) {
          case HTTPc_WEBSOCK_OPCODE_CONTINUATION_FRAME:
          case HTTPc_WEBSOCK_OPCODE_TXT_FRAME:
          case HTTPc_WEBSOCK_OPCODE_BIN_FRAME:

            if (p_ws->RxMsgFlags.IsFin == DEF_YES) {
              p_ws->OrigOpCode = HTTPc_WEBSOCK_OPCODE_NONE;
              p_ws->TotalMsgLen = 0u;
              //                                                   On Msg Rx Complete Hook.
              if (p_ws->OnMsgRxComplete != DEF_NULL) {
                p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
                p_ws->OnMsgRxComplete(p_conn_const);
              }
            }
            p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_WAIT;
            break;

          case HTTPc_WEBSOCK_OPCODE_PING:                      // Send a Pong Msg.

            if (p_ws->Flags.IsTxMsgCtrlUsed == DEF_NO) {
              HTTPcWebSock_SetCtrlResponse(p_conn, HTTPc_WEBSOCK_OPCODE_PONG, p_conn->RxBufPtr, p_ws->RxMsgLen);
              p_conn->RxBufPtr += p_ws->RxMsgLen;
              p_ws->Flags.IsPongStarted = DEF_YES;
              p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_WAIT;
            }
            break;

          case HTTPc_WEBSOCK_OPCODE_CLOSE:                     // Process the closing handshake

            if (p_ws->RxMsgLen >= sizeof(CPU_INT16U)) {
              p_close_code = (CPU_INT16U *)p_conn->RxBufPtr;
              p_ws->CloseCode = MEM_VAL_GET_INT16U_BIG(p_close_code);
              p_ws->CloseReason.DataPtr = p_conn->RxBufPtr + HTTPc_WEBSOCK_CLOSE_CODE_LEN;
              p_ws->CloseReason.Len = p_ws->RxMsgLen   - HTTPc_WEBSOCK_CLOSE_CODE_LEN;
            } else {
              p_ws->CloseCode = HTTPc_WEBSOCK_CLOSE_CODE_NORMAL;
              p_ws->CloseReason.DataPtr = DEF_NULL;
              p_ws->CloseReason.Len = 0u;
            }
            //                                                     Check if initiated by the application.
            if (p_ws->Flags.IsCloseStarted == DEF_YES) {
              p_conn->State = HTTPc_CONN_STATE_WEBSOCK_CLOSE;
              p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_WAIT;
              p_ws->Flags.IsCloseStarted = DEF_NO;
            } else {
              //                                                   Otherwise, initiated by the server.
              if (p_ws->Flags.IsTxMsgCtrlUsed == DEF_NO) {
                //                                                 Send a close response.
                is_std = HTTPcWebSock_IsStandardCloseCode(p_ws->CloseCode);
                if (is_std == DEF_YES) {
                  HTTPcWebSock_SetCtrlResponse(p_conn, HTTPc_WEBSOCK_OPCODE_CLOSE, p_conn->RxBufPtr, p_ws->RxMsgLen);
                  p_ws->Flags.IsCloseStarted = DEF_YES;
                } else {
                  RTOS_ERR_SET(local_err, RTOS_ERR_RX);
                  goto exit_err;
                }
              }
              p_conn->RxBufPtr += p_ws->RxMsgLen;
              p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_WAIT;
            }
            break;

          case HTTPc_WEBSOCK_OPCODE_PONG:

            //                                                     On Pong Hook.
            if (p_ws->OnPong != DEF_NULL) {
              p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
              p_ws->OnPong(p_conn_const, p_conn->RxBufPtr, p_ws->RxMsgLen);
            }

            p_conn->RxBufPtr += p_ws->RxMsgLen;
            p_ws->RxState = HTTPc_WEBSOCK_RX_STATE_WAIT;
            break;

          default:
            RTOS_ERR_SET(local_err, RTOS_ERR_RX);
            goto exit_err;
        }

        if ((p_conn->RxDataLenRem == 0u)
            || (p_ws->Flags.IsTxMsgCtrlUsed == DEF_YES)) {
          done = DEF_YES;
        } else {
          done = DEF_NO;
        }
        break;

      default:
        p_conn->ErrCode = local_err;
        p_conn->State = HTTPc_CONN_STATE_WEBSOCK_ERR;
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
    }
    //                                                             --------------------- RX DATA ----------------------
    result = DEF_BIT_IS_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_RX_MORE_DATA);

    if (result == DEF_YES) {                                    // If more data is needed...
      if (p_ws->Flags.IsRxDataCached == DEF_NO) {               // ...and If there is no rx data cached for ctrl msg...
        sock_rdy_rd = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPc_FLAG_SOCK_RDY_RD);
        sock_rdy_err = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPc_FLAG_SOCK_RDY_ERR);

        if ((sock_rdy_rd == DEF_YES)                            // ...and if the sock is ready....
            || (sock_rdy_err == DEF_YES)) {
          //                                                       ...then Rx data.
          result = HTTPcSock_ConnDataRx(p_conn, rem_rx_len, &local_err);
          switch (RTOS_ERR_CODE_GET(local_err)) {
            case RTOS_ERR_NONE:
            case RTOS_ERR_WOULD_BLOCK:
            case RTOS_ERR_TIMEOUT:
              break;

            case RTOS_ERR_NET_CONN_CLOSED_FAULT:
            case RTOS_ERR_NET_CONN_CLOSE_RX:
              RTOS_ERR_SET(p_conn->ErrCode, RTOS_ERR_NONE);
              p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_SERVER;
              p_conn->State = HTTPc_CONN_STATE_WEBSOCK_CLOSE;
              break;

            default:
              goto exit_err;
          }

          if (result == DEF_OK) {
            if (p_conn->RxDataLenRem == 0u) {
              done = DEF_YES;
            } else {
              done = DEF_NO;
            }
          } else {
            if (p_conn->RxDataLenRem == 0u) {
              done = DEF_YES;
            }
          }
        } else {
          done = DEF_YES;
        }
      }
    }
  } while (done == DEF_NO);

  return;

exit_err:
  p_conn->ErrCode = local_err;
  p_conn->State = HTTPc_CONN_STATE_WEBSOCK_ERR;
}

/****************************************************************************************************//**
 *                                       HTTPcWebSock_GetMsgHdr()
 *
 * @brief    Process the header of a WebSocket message.
 *
 * @param    p_conn  Pointer to current HTTPc Connection (WebSocket Upgraded).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Remaining number of bytes to parse the hdr.
 *******************************************************************************************************/
static CPU_INT32U HTTPcWebSock_GetMsgHdr(HTTPc_CONN *p_conn,
                                         RTOS_ERR   *p_err)
{
  CPU_SIZE_T    len;
  CPU_INT08U    hdr_1;
  CPU_INT08U    hdr_2;
  CPU_CHAR      *p_data;
  CPU_INT64U    payload;
  CPU_INT64U    rem_hdr;
  HTTPc_WEBSOCK *p_ws;
  CPU_BOOLEAN   rsvd_bit;

  //                                                               ------------------ INITIALIZATION ------------------
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  rem_hdr = 0u;

  len = p_conn->RxDataLenRem;
  p_data = p_conn->RxBufPtr;
  p_ws = p_conn->WebSockPtr;
  //                                                               ----------------- PROCESS HDR BYTES ----------------
  if (len < HTTPc_WEBSOCK_MIN_HEADER_LEN) {                     // Check if enough data to compute the first parts of...
    rem_hdr = HTTPc_WEBSOCK_MIN_HEADER_LEN;                     // ...the message hdr.
    goto exit;
  }
  //                                                               Get the 2 first bytes.
  hdr_1 = p_data[0];
  hdr_2 = p_data[1];
  p_data += 2;
  len -= 2;
  //                                                               Process msg OPCODE field.
  p_ws->RxMsgOpCode = hdr_1 & HTTPc_WEBSOCK_OPCODE_MASK;
  //                                                               Process FIN bit.
  p_ws->RxMsgFlags.IsFin = DEF_BIT_IS_SET(hdr_1, HTTPc_WEBSOCK_FIN_BIT);

  //                                                               Process RSVD bits.
  rsvd_bit = DEF_BIT_IS_SET(hdr_1, DEF_BIT_06);
  if (rsvd_bit == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  rsvd_bit = DEF_BIT_IS_SET(hdr_1, DEF_BIT_05);
  if (rsvd_bit == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  rsvd_bit = DEF_BIT_IS_SET(hdr_1, DEF_BIT_04);
  if (rsvd_bit == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
  //                                                               Process MASK bit.
  p_ws->RxMsgFlags.IsMasked = DEF_BIT_IS_SET(hdr_2, HTTPc_WEBSOCK_MASK_BIT);

  //                                                               ----------------- PROCESS PAYLOAD ------------------
  payload = hdr_2 & HTTPc_WEBSOCK_PAYLOAD_MASK;
  rem_hdr = 0u;
  //                                                               According for the 2 first bytes, check the rem...
  //                                                               ...size of the hdr.
  if (p_ws->RxMsgFlags.IsMasked == DEF_YES) {
    rem_hdr += sizeof(CPU_INT32U);
  }

  if (payload == HTTPc_WEBSOCK_NORMAL_FRAME_CODE) {
    rem_hdr += sizeof(CPU_INT16U);
  } else if (payload == HTTPc_WEBSOCK_LONG_FRAME_CODE) {
    rem_hdr += sizeof(CPU_INT64U);
  }
  //                                                               Check if enough data rxd data.
  if (len < rem_hdr ) {
    goto exit;
  }
  //                                                               Get the msg PAYLOAD length.
  if (payload <= HTTPc_WEBSOCK_SMALL_FRAME_MAX_LEN) {
    //                                                             Payload length is <= 125 .
    p_ws->RxMsgLen = payload;
  } else if (payload == HTTPc_WEBSOCK_NORMAL_FRAME_CODE) {
    //                                                             Payload length is > 125 and <= 65535.
    p_ws->RxMsgLen = MEM_VAL_GET_INT16U_BIG(p_data);
    p_data += sizeof(CPU_INT16U);
  } else if (payload == HTTPc_WEBSOCK_LONG_FRAME_CODE) {
    //                                                             Payload length is > 65535.
    payload = MEM_VAL_GET_INT32U_BIG(p_data);
    if (payload != 0u) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
    p_data += sizeof(CPU_INT32U);
    payload = MEM_VAL_GET_INT32U_BIG(p_data);
    Mem_Copy(&p_ws->RxMsgLen, &payload, sizeof(CPU_INT64U));
    p_data += sizeof(CPU_INT32U);
  }
  //                                                               ------------------ MSG VALIDATION ------------------
  switch (p_ws->RxMsgOpCode ) {
    case HTTPc_WEBSOCK_OPCODE_TXT_FRAME:
    case HTTPc_WEBSOCK_OPCODE_BIN_FRAME:
      //                                                           If not the first frame of a frag msg, must be cont.
      if (p_ws->TotalMsgLen != 0u) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      p_ws->OrigOpCode = p_ws->RxMsgOpCode;
      p_ws->TotalMsgLen = p_ws->RxMsgLen;
      break;

    case HTTPc_WEBSOCK_OPCODE_CONTINUATION_FRAME:

      if (p_ws->TotalMsgLen == 0u) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }

      p_ws->TotalMsgLen += p_ws->RxMsgLen;
      break;

    case HTTPc_WEBSOCK_OPCODE_CLOSE:
    case HTTPc_WEBSOCK_OPCODE_PING:
    case HTTPc_WEBSOCK_OPCODE_PONG:
      //                                                           Control message cannot be longer than 125 bytes.
      if (p_ws->RxMsgLen > HTTPc_WEBSOCK_MAX_CTRL_FRAME_LEN ) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      }
      //                                                           Control message cannot be fragmented.
      if (p_ws->RxMsgFlags.IsFin == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      }
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      break;
  }
  //                                                               --------------- PROCESS MASKING KEY ----------------
  if (p_ws->RxMsgFlags.IsMasked == DEF_YES) {
    Mem_Copy(&p_ws->RxMsgMskKey, p_data, sizeof(CPU_INT32U));
    p_data += sizeof(CPU_INT32U);
  }
  //                                                               -------------- UPDATE CONN RX PARAMS ---------------
  p_conn->RxDataLenRem -= (p_data - p_conn->RxBufPtr);
  p_conn->RxBufPtr = p_data;

  rem_hdr = 0u;

exit:
  return rem_hdr;
}

/****************************************************************************************************//**
 *                                       HTTPcWebSock_GetPayload()
 *
 * @brief    Get the payload of a WebSocket message.
 *
 * @param    p_conn  Pointer to current HTTPc Connection (WebSocket Upgraded).
 *******************************************************************************************************/
static void HTTPcWebSock_GetPayload(HTTPc_CONN *p_conn)
{
  CPU_CHAR               *p_buf;
  HTTPc_CONN_OBJ         *p_conn_const;
  HTTPc_WEBSOCK          *p_ws;
  CPU_INT32U             content_len;
  CPU_INT32U             msg_len_rem;
  CPU_INT32U             chunk_len;
  HTTPc_WEBSOCK_MSG_TYPE msg_type;
  CPU_INT32U             data_len_read;

  //                                                               ------------------ INTIALIZATION -------------------
  p_ws = p_conn->WebSockPtr;
  p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
  p_buf = p_conn->RxBufPtr;
  content_len = p_ws->RxMsgLen;

  msg_len_rem = p_ws->RxMsgLen - p_conn->RxDataLen;

  switch (p_ws->RxMsgOpCode) {
    //                                                             -------------------- DATA FRAME --------------------
    case HTTPc_WEBSOCK_OPCODE_CONTINUATION_FRAME:
    case HTTPc_WEBSOCK_OPCODE_TXT_FRAME:
    case HTTPc_WEBSOCK_OPCODE_BIN_FRAME:

      if (msg_len_rem > p_conn->RxDataLenRem) {
        chunk_len = p_conn->RxDataLenRem;
      } else {
        chunk_len = msg_len_rem;
      }
      if (p_conn->RxDataLen > content_len) {
        return;
      }

      if (p_ws->RxMsgOpCode == HTTPc_WEBSOCK_OPCODE_CONTINUATION_FRAME) {
        msg_type = (HTTPc_WEBSOCK_MSG_TYPE)p_ws->OrigOpCode;
      } else {
        msg_type = (HTTPc_WEBSOCK_MSG_TYPE)p_ws->RxMsgOpCode;
      }
      if (p_ws->RxMsgDataPtr != DEF_NULL) {
        Mem_Copy(&((p_ws->RxMsgDataPtr)[p_ws->RxMsgLenRead]), p_buf, chunk_len);
        data_len_read = chunk_len;
      } else if (p_ws->OnMsgRxData != DEF_NULL) {
        data_len_read = p_ws->OnMsgRxData(p_conn_const,
                                          msg_type,
                                          p_buf,
                                          chunk_len);
      } else {
        data_len_read = chunk_len;
      }

      p_conn->RxDataLen += data_len_read;
      p_ws->RxMsgLenRead += data_len_read;
      p_conn->RxBufPtr += data_len_read;
      p_conn->RxDataLenRem -= data_len_read;
      break;
    //                                                             -------------------- CTRL FRAME --------------------
    case HTTPc_WEBSOCK_OPCODE_PONG:
    case HTTPc_WEBSOCK_OPCODE_CLOSE:
    case HTTPc_WEBSOCK_OPCODE_PING:
      if (p_ws->RxMsgLen <= p_conn->RxDataLenRem) {
        p_conn->TxBufPtr = p_conn->RxBufPtr + p_conn->RxDataLenRem;
        p_conn->RxDataLenRem -= p_ws->RxMsgLen;
        p_ws->Flags.IsRxDataCached = DEF_YES;
        p_ws->RxMsgLenRead += p_ws->RxMsgLen;
      }
      break;

    default:
      break;
  }
}

/****************************************************************************************************//**
 *                                        HTTPcWebSock_SetTxBuf()
 *
 * @brief    Mask and copy a message into a buffer.
 *
 * @param    p_msg       Pointer to the WebSocket message object.
 *
 * @param    p_dst_buf   Pointer to the destination buffer.
 *
 * @param    p_src_buf   Pointer to the source buffer.
 *
 * @param    len         Length of the data to copy.
 *******************************************************************************************************/
static void HTTPcWebSock_SetTxBuf(HTTPc_WEBSOCK_MSG *p_msg,
                                  CPU_CHAR          *p_dst_buf,
                                  CPU_CHAR          *p_src_buf,
                                  CPU_INT32U        len)
{
  CPU_INT32U msk_idx;
  CPU_INT32U msg_idx;
  CPU_CHAR   msk_byte[4];

  if (len == 0u) {
    return;
  }

  msk_byte[3] = (CPU_CHAR) (p_msg->MskKey & 0x000000FF);
  msk_byte[2] = (CPU_CHAR)((p_msg->MskKey & 0x0000FF00) >> 8);
  msk_byte[1] = (CPU_CHAR)((p_msg->MskKey & 0x00FF0000) >> 16);
  msk_byte[0] = (CPU_CHAR)((p_msg->MskKey & 0xFF000000) >> 24);

  p_dst_buf += len;
  p_src_buf += len;
  msg_idx = p_msg->LenSent + len;

  while (p_msg->LenSent < msg_idx) {
    msg_idx--;
    p_src_buf--;
    p_dst_buf--;
    msk_idx = msg_idx % sizeof(CPU_INT32U);
    *p_dst_buf = *p_src_buf ^ msk_byte[msk_idx];
  }
}

/****************************************************************************************************//**
 *                                    HTTPc_WebSock_PrepareTxBufHdr()
 *
 * @brief    Set the message header into the TX buffer.
 *
 * @param    p_conn      Pointer to current HTTPc Connection (WebSocket Upgraded).
 *
 * @param    p_tx_msg    Pointer to the WebSocket message object to send.
 *
 * @param    p_tx_buf    Pointer to Tx buffer.
 *******************************************************************************************************/
static void HTTPcWebSock_SetTxBufHdr(HTTPc_CONN        *p_conn,
                                     HTTPc_WEBSOCK_MSG *p_tx_msg,
                                     CPU_CHAR          *p_tx_buf)
{
  CPU_CHAR    *p_hdr;
  CPU_INT32U  tmp_len;
  CPU_INT32U  tmp_mask;
  CPU_BOOLEAN fin;

  PP_UNUSED_PARAM(p_conn);

  //                                                               ------------------ INITIALIZATION ------------------
  p_hdr = p_tx_buf;
  //                                                               Set the first to Byte of the WebSocket hdr.
  fin = p_tx_msg->Flags.IsFin;
  *p_hdr = p_tx_msg->OpCode + ((fin == DEF_YES) ? HTTPc_WEBSOCK_FIN_BIT : 0u);
  p_hdr++;
  //                                                               Set the Masking Key.
  *p_hdr = HTTPc_WEBSOCK_MASK_BIT;
  //                                                               ----------------- SET PAYLOAD LEN ------------------
  if (p_tx_msg->Len >= HTTPc_WEBSOCK_LONG_FRAME_MIN_LEN ) {     // Use the extended 64-bits payload len.
    *p_hdr += HTTPc_WEBSOCK_LONG_FRAME_CODE;
    p_hdr++;

    Mem_Clr(p_hdr, sizeof(CPU_INT32U));                         // Maximum msg lenght is 32-bit.
    p_hdr += sizeof(CPU_INT32U);

    tmp_len = MEM_VAL_GET_INT32U_BIG(&p_tx_msg->Len);
    Mem_Copy(p_hdr, &tmp_len, sizeof(CPU_INT32U));
    p_hdr += sizeof(CPU_INT32U);
    //                                                             Use the extended 16-bits payload len.
  } else if (p_tx_msg->Len >= HTTPc_WEBSOCK_NORMAL_FRAME_MIN_LEN ) {
    *p_hdr += HTTPc_WEBSOCK_NORMAL_FRAME_CODE;
    p_hdr++;

    tmp_len = MEM_VAL_GET_INT16U_BIG(&p_tx_msg->Len);
    Mem_Copy(p_hdr, &tmp_len, sizeof(CPU_INT16U));
    p_hdr += sizeof(CPU_INT16U);
  } else {                                                      // Use the 7-bits payload len.
    *p_hdr += p_tx_msg->Len;
    p_hdr++;
  }
  //                                                               ----------------- SET MASKING KEY ------------------
  tmp_mask = MEM_VAL_GET_INT32U_BIG(&p_tx_msg->MskKey);
  Mem_Copy(p_hdr, &tmp_mask, sizeof(CPU_INT32U));
}

/************************************************************************************************
 *                                    HTTPcWebSock_SetCtrlResponse()
 *
 * @brief    Set an internal Control message response using cached Rx Data.
 *
 * @param    p_conn      Pointer to current HTTPc Connection (WebSocket Upgraded).
 *
 * @param    opcode      Type of control frame to send.
 *
 * @param    p_data      Pointer to data.
 *
 * @param    msg_len     Message length.
 *******************************************************************************************************/
static void HTTPcWebSock_SetCtrlResponse(HTTPc_CONN           *p_conn,
                                         HTTPc_WEBSOCK_OPCODE opcode,
                                         CPU_CHAR             *p_data,
                                         CPU_INT32U           msg_len)
{
  HTTPc_WEBSOCK     *p_ws;
  HTTPc_WEBSOCK_MSG *p_msg_tx;
  RTOS_ERR          local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               ------------------- GET MSG PTR --------------------
  p_ws = p_conn->WebSockPtr;
  p_msg_tx = &p_ws->TxMsgCtrl;

  //                                                               --------------- SET TX CTRL MSG OBJ ----------------
  p_msg_tx->Len = msg_len;
  p_msg_tx->LenSent = 0u;
  p_msg_tx->DataPtr = p_data;
  p_msg_tx->OpCode = opcode;
  p_msg_tx->ConnPtr = p_conn;
  p_msg_tx->Flags.IsFin = DEF_YES;
  p_msg_tx->Flags.IsMasked = DEF_YES;
  p_msg_tx->Flags.IsHdrSet = DEF_NO;

  p_ws->Flags.IsTxMsgCtrlUsed = DEF_YES;

  HTTPcWebSock_TxMsgAdd(p_msg_tx, &local_err);

  PP_UNUSED_PARAM(local_err);
}

/****************************************************************************************************//**
 *                                      HTTPcWebSock_TxMsgRemove()
 *
 * @brief    Remove the first message in the TX message queue.
 *
 * @param    p_conn  Pointer to current HTTPc Connection (WebSocket Upgraded).
 *******************************************************************************************************/
static void HTTPcWebSock_TxMsgRemove(HTTPc_CONN *p_conn)
{
  HTTPc_WEBSOCK     *p_ws;
  HTTPc_WEBSOCK_MSG *p_msg_item;

  p_ws = p_conn->WebSockPtr;
  p_msg_item = p_ws->TxMsgListHeadPtr;

  if (p_msg_item == DEF_NULL) {
    p_ws->TxMsgListEndPtr = DEF_NULL;
  } else if (p_msg_item->NextPtr == DEF_NULL) {
    p_ws->TxMsgListHeadPtr = DEF_NULL;
    p_ws->TxMsgListEndPtr = DEF_NULL;
    p_msg_item->NextPtr = 0;
  } else {
    p_ws->TxMsgListHeadPtr = p_msg_item->NextPtr;
  }
  if (p_msg_item != DEF_NULL) {
    p_msg_item->Flags.IsUsed = DEF_NO;
  }
}

/****************************************************************************************************//**
 *                                     HTTPcWebSock_TxMsgCleanList()
 *
 * @brief    Remove all the pending WebSocket message.
 *
 * @param    p_conn  Pointer to current HTTPc Connection (WebSocket Upgraded).
 *******************************************************************************************************/
static void HTTPcWebSock_TxMsgCleanList(HTTPc_CONN *p_conn)
{
  HTTPc_WEBSOCK         *p_ws;
  HTTPc_CONN_OBJ        *p_conn_const;
  HTTPc_WEBSOCK_MSG     *p_msg;
  HTTPc_WEBSOCK_MSG_OBJ *p_msg_const;

  p_ws = p_conn->WebSockPtr;

  while (p_ws->TxMsgListHeadPtr != DEF_NULL) {
    p_msg = (HTTPc_WEBSOCK_MSG *)p_ws->TxMsgListHeadPtr;
    //                                                             Call On Msg Tx Complete Callback if set.
    if (p_msg->OnMsgTxComplete != DEF_NULL) {
      p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
      p_msg_const = (HTTPc_WEBSOCK_MSG_OBJ *)p_ws->TxMsgListHeadPtr;
      p_msg->OnMsgTxComplete(p_conn_const, p_msg_const, DEF_FAIL);
    }
    HTTPcWebSock_TxMsgRemove(p_conn);
  }
}

/****************************************************************************************************//**
 *                                     HTTPcWebSock_ComputeHdrLen()
 *
 * @brief    Compute the header length of a WebSocket Msg.
 *
 * @param    p_msg   Pointer to a WebSocket Msg obj.
 *
 * @return   Length of the header.
 *******************************************************************************************************/
static CPU_INT16U HTTPcWebSock_ComputeHdrLen(HTTPc_WEBSOCK_MSG *p_msg)
{
  CPU_INT16U hdr_len;

  hdr_len = 2;                                                  // Minimum Header Len.

  if (p_msg->Len >= HTTPc_WEBSOCK_LONG_FRAME_MIN_LEN ) {
    //                                                             Use the extended 64-bits payload len.
    hdr_len += sizeof(CPU_INT64U);
  } else if (p_msg->Len >= HTTPc_WEBSOCK_NORMAL_FRAME_MIN_LEN ) {
    //                                                             Use the extended 16-bits payload len.
    hdr_len += sizeof(CPU_INT16U);
  }

  if (p_msg->Flags.IsMasked == DEF_YES) {
    hdr_len += sizeof(CPU_INT32U);
  }

  return (hdr_len);
}

/****************************************************************************************************//**
 *                                    HTTPcWebSock_IsProtocolError()
 *
 * @brief    Check if the error code is related to a WebSocket protocol error.
 *
 * @param    err     Error code to check.
 *
 * @return   DEF_YES, if the error code is a protocol error.
 *           DEF_NO,  if the error code is NOT a protocol error.
 *******************************************************************************************************/
static CPU_BOOLEAN HTTPcWebSock_IsProtocolError(RTOS_ERR err)
{
  switch (RTOS_ERR_CODE_GET(err)) {
    case RTOS_ERR_RX:
      return (DEF_YES);

    default:
      return (DEF_NO);
  }
}

/****************************************************************************************************//**
 *                                  HTTPcWebSock_IsStandardCloseCode()
 *
 * @brief    Check if the Close code value is known and standarized.
 *
 * @param    close_code  Close code value to check.
 *
 * @return   DEF_YES, if the close code is standard.
 *           DEF_NO,  if the close code is NOT standard.
 *******************************************************************************************************/
static CPU_BOOLEAN HTTPcWebSock_IsStandardCloseCode(HTTPc_WEBSOCK_CLOSE_CODE close_code)
{
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
      return DEF_YES;

    default:
      break;
  }
  if ((close_code >= 3000) && (close_code <= 4999)) {
    return DEF_YES;
  }
  return DEF_NO;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // HTTPc_WEBSOCK_MODULE_EN
#endif // RTOS_MODULE_NET_HTTP_CLIENT_AVAIL
