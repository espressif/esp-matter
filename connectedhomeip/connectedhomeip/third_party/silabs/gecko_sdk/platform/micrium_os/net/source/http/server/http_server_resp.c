/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Response Module
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

#include  "../http_priv.h"
#include  "../http_dict_priv.h"

#include  <common/include/lib_ascii.h>
#include  <common/include/lib_str.h>
#include  <common/include/lib_utils.h>

#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

/********************************************************************************************************
 *                                               TOKEN DEFINES
 *******************************************************************************************************/

#define  HTTPs_TOKEN_EXTENAL_CHAR_START                     ASCII_CHAR_DOLLAR_SIGN
#define  HTTPs_TOKEN_INTERNAL_CHAR_START                    ASCII_CHAR_NUMBER_SIGN
#define  HTTPs_TOKEN_CHAR_VAR_SEP_START                     ASCII_CHAR_LEFT_CURLY_BRACKET
#define  HTTPs_TOKEN_CHAR_VAR_SEP_END                       ASCII_CHAR_RIGHT_CURLY_BRACKET
#define  HTTPs_TOKEN_CHAR_OFFSET_LEN                        2u                              // Length of '${'.
#define  HTTPs_TOKEN_CHAR_DFLT_VAL                          ASCII_CHAR_TILDE

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN HTTPsResp_PrepareStatusCode(HTTPs_INSTANCE *p_instance,
                                               HTTPs_CONN     *p_conn);

static CPU_BOOLEAN HTTPsResp_PrepareBodyData(HTTPs_INSTANCE *p_instance,
                                             HTTPs_CONN     *p_conn);

static void HTTPsResp_StatusLine(HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN     *p_conn,
                                 HTTPs_ERR      *p_err);

static void HTTPsResp_Hdr(HTTPs_INSTANCE *p_instance,
                          HTTPs_CONN     *p_conn,
                          HTTPs_ERR      *p_err);

static CPU_CHAR *HTTPsResp_HdrFieldAdd(HTTPs_INSTANCE *p_instance,
                                       HTTPs_CONN     *p_conn,
                                       CPU_CHAR       *p_buf,
                                       CPU_SIZE_T     buf_len,
                                       HTTP_HDR_FIELD field_type,
                                       CPU_CHAR       *p_val,
                                       CPU_SIZE_T     val_len,
                                       HTTPs_ERR      *p_err);

static CPU_BOOLEAN HTTPsResp_DataTransferStd(HTTPs_INSTANCE *p_instance,
                                             HTTPs_CONN     *p_conn,
                                             HTTPs_ERR      *p_err);

static CPU_BOOLEAN HTTPsResp_DataTransferChunkedWithHook(HTTPs_INSTANCE *p_instance,
                                                         HTTPs_CONN     *p_conn,
                                                         HTTPs_ERR      *p_err);

static CPU_SIZE_T HTTPsResp_WrChunkedToBuf(CPU_CHAR   *p_buf,
                                           CPU_SIZE_T buf_len,
                                           CPU_SIZE_T data_len,
                                           CPU_SIZE_T len_dig_str_len);

#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsResp_DataTransferChunked(HTTPs_INSTANCE *p_instance,
                                                 HTTPs_CONN     *p_conn,
                                                 HTTPs_ERR      *p_err);

static CPU_INT16U HTTPsResp_TokenFinder(CPU_CHAR         *p_buf,
                                        CPU_INT16U       buf_len,
                                        CPU_CHAR         **p_str_token,
                                        HTTPs_TOKEN_TYPE *p_token_type,
                                        HTTPs_ERR        *p_err);

static CPU_BOOLEAN HTTPsResp_TokenValSet(HTTPs_INSTANCE *p_instance,
                                         HTTPs_CONN     *p_conn,
                                         const CPU_CHAR *p_token,
                                         CPU_INT16U     token_len,
                                         CPU_CHAR       *p_val,
                                         CPU_INT16U     val_len_max);
#endif

static void HTTPsResp_DfltErrPageSet(HTTPs_INSTANCE *p_instance,
                                     HTTPs_CONN     *p_conn);

#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsResp_FileOpen(HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN     *p_conn);
#endif

static CPU_SIZE_T HTTPsResp_DataRd(HTTPs_INSTANCE *p_instance,
                                   HTTPs_CONN     *p_conn,
                                   CPU_CHAR       *p_dst,
                                   CPU_SIZE_T     dst_len_max);

#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
static void HTTPsResp_DataSetPos(HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN     *p_conn,
                                 CPU_INT32S     offset,
                                 CPU_INT08U     origin);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           HTTPsResp_Prepare()
 *
 * @brief    (1) Prepare server response
 *               - (a) (HOOK) Signal Request Ready to be answered.
 *               - (b) (HOOK) Poll for an answer to the request.
 *               - (c) Prepare the status code.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @return   DEF_YES, if preparation is done.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsResp_Prepare(HTTPs_INSTANCE *p_instance,
                              HTTPs_CONN     *p_conn)
{
  const HTTPs_CFG     *p_cfg = p_instance->CfgPtr;
  HTTPs_INSTANCE_ERRS *p_ctr_err = DEF_NULL;
  CPU_BOOLEAN         is_err = DEF_NO;
  CPU_BOOLEAN         result = DEF_NO;
  CPU_BOOLEAN         done = DEF_NO;

  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  //                                                               --------------- VALIDATE STATUS CODE ---------------
  is_err = HTTPsResp_PrepareStatusCode(p_instance, p_conn);

  if (is_err == DEF_NO) {
    result = HTTPsResp_PrepareBodyData(p_instance, p_conn);
    if (result == DEF_FAIL) {
      HTTPs_ERR_INC(p_ctr_err->Resp_ErrPrepareCtr);
      p_conn->State = HTTPs_CONN_STATE_RESP_PREPARE;
      goto exit;
    }
  } else {
    result = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnErrFileGetHook);
    if (result == DEF_YES) {
      p_cfg->HooksPtr->OnErrFileGetHook(p_cfg->Hooks_CfgPtr,
                                        p_conn->StatusCode,
                                        p_conn->PathPtr,
                                        p_conn->PathLenMax,
                                        &p_conn->RespBodyDataType,
                                        &p_conn->RespContentType,
                                        &p_conn->DataPtr,
                                        &p_conn->DataLen);

      result = HTTPsResp_PrepareBodyData(p_instance, p_conn);
      if (result == DEF_FAIL) {
        HTTPs_ERR_INC(p_ctr_err->Resp_ErrPrepareErrPageCtr);
        HTTPsResp_DfltErrPageSet(p_instance, p_conn);
      }
    } else {
      HTTPsResp_DfltErrPageSet(p_instance, p_conn);
    }
  }

  //                                                               ------------ RELEASE HDR USE BY REQUEST ------------
#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
  while (p_conn->HdrListPtr != DEF_NULL) {
    HTTPsMem_ReqHdrRelease(p_instance,
                           p_conn);
  }
#endif

#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
  p_conn->HdrType = HTTPs_HDR_TYPE_RESP;
#endif

  //                                                               ---------- INITIALIZE RESPONSE PARAMETERS ----------
  p_conn->DataTxdLen = 0u;

  if ((p_conn->Method != HTTP_METHOD_HEAD)
      && (p_conn->RespBodyDataType != HTTPs_BODY_DATA_TYPE_NONE)) {
    DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_RESP_BODY_PRESENT);
  } else {
    DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_BODY_PRESENT);
  }

  done = DEF_YES;

exit:
  return (done);
}

/****************************************************************************************************//**
 *                                           HTTPsResp_Handle()
 *
 * @brief    (1) Prepare connection to transmit the request response:
 *               - (a) Get file content type and update response state accordingly.
 *               - (b) Parse file token and get their values.
 *               - (b) Prepare response:
 *                   - (A) Prepare response line.
 *                   - (B) Prepare response header section.
 *                   - (C) Update connection parameters to transmit response.
 *                   - (D) Update response state to transmit file or to close the connection.
 *               - (c) Transmit file:
 *                   - (A) Read file.
 *                   - (B) Parse and replace token from the connection buffer.
 *                   - (C) Validate that the file is completely transmitted.
 *               - (d) Close file.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @return   DEF_YES, if response handling is completed.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsResp_Handle(HTTPs_INSTANCE *p_instance,
                             HTTPs_CONN     *p_conn)
{
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
  HTTPs_INSTANCE_ERRS  *p_ctr_err = DEF_NULL;
  CPU_BOOLEAN          done = DEF_NO;
  CPU_BOOLEAN          body_done = DEF_NO;
  CPU_BOOLEAN          body_present = DEF_NO;
  CPU_BOOLEAN          chunk_en = DEF_NO;
  CPU_BOOLEAN          chunk_hook_en = DEF_NO;
  HTTPs_ERR            err;

  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);
  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  switch (p_conn->State) {
    case HTTPs_CONN_STATE_RESP_INIT:
    case HTTPs_CONN_STATE_RESP_STATUS_LINE:
      //                                                           ------------- PREPARE RESP STATUS LINE -------------
      HTTPsResp_StatusLine(p_instance, p_conn, &err);
      if (err != HTTPs_ERR_NONE) {
        HTTPs_ERR_INC(p_ctr_err->Resp_ErrStatusLineCtr);
        p_conn->SockState = HTTPs_SOCK_STATE_NONE;
        p_conn->State = HTTPs_CONN_STATE_ERR_FATAL;
        p_conn->ErrCode = err;
        break;
      }
      p_conn->State = HTTPs_CONN_STATE_RESP_HDR;
    //                                                             'break' intentionally omitted; MUST execute the ...
    //                                                             ... following case :                            ...
    //                                                             ... 'HTTPs_CONN_STATE_RESP_HDR'.
    //                                                             fallthrough
    case HTTPs_CONN_STATE_RESP_HDR:
    case HTTPs_CONN_STATE_RESP_HDR_CONTENT_TYPE:
    case HTTPs_CONN_STATE_RESP_HDR_FILE_TRANSFER:
    case HTTPs_CONN_STATE_RESP_HDR_LOCATION:
    case HTTPs_CONN_STATE_RESP_HDR_CONN:
    case HTTPs_CONN_STATE_RESP_HDR_LIST:
    case HTTPs_CONN_STATE_RESP_HDR_TX:
    case HTTPs_CONN_STATE_RESP_HDR_END:
      //                                                           ----------------- PREPARE RESP HDR -----------------
      HTTPsResp_Hdr(p_instance, p_conn, &err);
      switch (err) {
        case HTTPs_ERR_NONE:
          body_present = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_RESP_BODY_PRESENT);
          chunk_en = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED);
          chunk_hook_en = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED_HOOK);
          if (body_present == DEF_NO) {
            p_conn->State = HTTPs_CONN_STATE_RESP_COMPLETED;
          } else {
            if (chunk_hook_en == DEF_YES) {
              p_conn->State = HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_HOOK;
            } else {
              if (chunk_en == DEF_YES) {
                p_conn->State = HTTPs_CONN_STATE_RESP_DATA_CHUNCKED;
              } else {
                p_conn->State = HTTPs_CONN_STATE_RESP_FILE_STD;
              }
            }
          }
          p_conn->SockState = HTTPs_SOCK_STATE_TX;
          break;

        case HTTPs_ERR_RESP_BUF_NO_MORE_SPACE:
          p_conn->SockState = HTTPs_SOCK_STATE_TX;
          break;

        default:
          HTTPs_ERR_INC(p_ctr_err->Resp_ErrHdrCtr);
          p_conn->ErrCode = err;
          p_conn->State = HTTPs_CONN_STATE_ERR_FATAL;
          p_conn->SockState = HTTPs_SOCK_STATE_NONE;
          goto exit;
      }
      break;

    case HTTPs_CONN_STATE_RESP_FILE_STD:
      //                                                           ---------------- STANDARD TRANSFER -----------------
      body_done = HTTPsResp_DataTransferStd(p_instance, p_conn, &err);
      switch (err) {
        case HTTPs_ERR_NONE:
          if (body_done == DEF_YES) {
            p_conn->State = HTTPs_CONN_STATE_RESP_COMPLETED;
          }
          if (p_conn->TxDataLen > 0) {
            p_conn->SockState = HTTPs_SOCK_STATE_TX;
          } else {
            p_conn->SockState = HTTPs_SOCK_STATE_NONE;
          }
          break;

        default:
          HTTPsResp_DataComplete(p_instance, p_conn);
          HTTPs_ERR_INC(p_ctr_err->Resp_ErrTransferStdCtr);
          p_conn->ErrCode = err;
          p_conn->State = HTTPs_CONN_STATE_ERR_FATAL;
          p_conn->SockState = HTTPs_SOCK_STATE_NONE;
          goto exit;
      }
      break;

    case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED:
    case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_TX_TOKEN:
    case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_TX_LAST_CHUNK:
      //                                                           ----------------- CHUNKED TRANSFER -----------------
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
      body_done = HTTPsResp_DataTransferChunked(p_instance, p_conn, &err);
      switch (err) {
        case HTTPs_ERR_NONE:
          if (body_done == DEF_YES) {
            p_conn->State = HTTPs_CONN_STATE_RESP_COMPLETED;
          }
          if (p_conn->TxDataLen > 0) {
            p_conn->SockState = HTTPs_SOCK_STATE_TX;
          } else {
            p_conn->SockState = HTTPs_SOCK_STATE_NONE;
          }
          break;

        case HTTPs_ERR_TOKEN_POOL_GET:
          p_conn->SockState = HTTPs_SOCK_STATE_NONE;
          break;

        default:
          HTTPsResp_DataComplete(p_instance, p_conn);
          HTTPs_ERR_INC(p_ctr_err->Resp_ErrTransferChunkedCtr);
          p_conn->ErrCode = err;
          p_conn->State = HTTPs_CONN_STATE_ERR_FATAL;
          p_conn->SockState = HTTPs_SOCK_STATE_NONE;
          goto exit;
      }
#endif
      break;

    case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_HOOK:
    case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_FINALIZE:
      //                                                           ------------- CHUNK TRANSFER WITH HOOK -------------
      body_done = HTTPsResp_DataTransferChunkedWithHook(p_instance, p_conn, &err);
      switch (err) {
        case HTTPs_ERR_NONE:
          if (body_done == DEF_YES) {
            p_conn->State = HTTPs_CONN_STATE_RESP_COMPLETED;
          }
          p_conn->SockState = HTTPs_SOCK_STATE_TX;
          break;

        default:
          HTTPs_ERR_INC(p_ctr_err->Resp_ErrTransferChunkedHookCtr);
          p_conn->ErrCode = err;
          p_conn->State = HTTPs_CONN_STATE_ERR_FATAL;
          p_conn->SockState = HTTPs_SOCK_STATE_NONE;
          goto exit;
      }
      break;

    case HTTPs_CONN_STATE_RESP_COMPLETED:
      //                                                           -------------------- CLOSE FILE --------------------
      if (p_conn->TxDataLen == 0) {                             // No more data to tx.
        HTTPsResp_DataComplete(p_instance, p_conn);             // Close file.
                                                                // Update conn state to close the conn.
        p_conn->SockState = HTTPs_SOCK_STATE_NONE;
        p_conn->State = HTTPs_CONN_STATE_COMPLETED;
        done = DEF_YES;

        if ((p_conn->Method != HTTP_METHOD_HEAD)
            && (p_conn->RespBodyDataType != HTTPs_BODY_DATA_TYPE_NONE)) {
          HTTPs_STATS_INC(p_ctr_stats->Resp_StatBodyTxdCtr);
        }
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
        if (p_conn->TokenCtrlPtr != DEF_NULL) {
          HTTPsMem_TokenRelease(p_instance, p_conn);
        }
#endif
      } else {                                                  // Complete tx before closing.
        p_conn->SockState = HTTPs_SOCK_STATE_TX;
      }
      break;

    default:
      HTTPs_ERR_INC(p_ctr_err->Resp_ErrStateUnknownCtr);
      p_conn->State = HTTPs_CONN_STATE_ERR_FATAL;
      p_conn->SockState = HTTPs_SOCK_STATE_NONE;                // Conn must be closed.
      p_conn->ErrCode = HTTPs_ERR_STATE_UNKNOWN;
      break;
  }

exit:
  return (done);
}

/****************************************************************************************************//**
 *                                           HTTPsResp_DataComplete()
 *
 * @brief    (1) Terminate the body data stage:
 *               - (a) Close a file if FS is present.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *******************************************************************************************************/
void HTTPsResp_DataComplete(HTTPs_INSTANCE *p_instance,
                            HTTPs_CONN     *p_conn)
{
  const HTTPs_CFG      *p_cfg = p_instance->CfgPtr;
  const NET_FS_API     *p_fs_api;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
  HTTPs_INSTANCE_ERRS  *p_ctr_errs = DEF_NULL;

  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);
  HTTPs_SET_PTR_ERRS(p_ctr_errs, p_instance);

  switch (p_cfg->FS_Type) {
    case HTTPs_FS_TYPE_STATIC:
      p_fs_api = ((HTTPs_CFG_FS_STATIC *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      break;

    case HTTPs_FS_TYPE_DYN:
      p_fs_api = ((HTTPs_CFG_FS_DYN *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      break;

    case HTTPs_FS_TYPE_NONE:
      p_fs_api = DEF_NULL;
      break;

    default:
      HTTPs_ERR_INC(p_ctr_errs->FS_ErrTypeInvalidCtr);
      return;
  }

  if (p_conn->DataPtr == DEF_NULL) {
    return;
  }

  switch (p_conn->RespBodyDataType) {
    case HTTPs_BODY_DATA_TYPE_FILE:                             // File type is from FS.
      HTTPs_STATS_INC(p_ctr_stats->FS_StatClosedCtr);

      if (p_fs_api != DEF_NULL) {
        p_fs_api->Close(p_conn->DataPtr);
      } else {
        HTTPs_ERR_INC(p_ctr_errs->File_ErrCloseNoFS_Ctr);
      }
      p_conn->DataPtr = DEF_NULL;
      break;

    case HTTPs_BODY_DATA_TYPE_NONE:
    case HTTPs_BODY_DATA_TYPE_STATIC_DATA:
    default:
      break;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       HTTPsResp_PrepareStatusCode()
 *
 * @brief    (1) Prepare connection following the status code
 *               - (a) (HOOK) Get error page, if required.
 *               - (b) Validate file presence, if required.
 *               - (c) Increment the appropriate status code processed counter.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @return   DEF_YES, if status code is related to an error.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN HTTPsResp_PrepareStatusCode(HTTPs_INSTANCE *p_instance,
                                               HTTPs_CONN     *p_conn)
{
  CPU_BOOLEAN          is_err;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;

#if  (HTTPs_CFG_CTR_STAT_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_instance);
#endif
  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);

  switch (p_conn->StatusCode) {
    //                                                             Success status codes.
    case HTTP_STATUS_OK:                                        // 200
    case HTTP_STATUS_CREATED:                                   // 201
    case HTTP_STATUS_ACCEPTED:                                  // 202
    case HTTP_STATUS_NO_CONTENT:                                // 204
    case HTTP_STATUS_RESET_CONTENT:                             // 205
      DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_LOCATION);
      is_err = DEF_NO;
      break;

    //                                                             Relocation status codes.
    case HTTP_STATUS_MOVED_PERMANENTLY:                         // 301
    case HTTP_STATUS_FOUND:                                     // 302
    case HTTP_STATUS_SEE_OTHER:                                 // 303
    case HTTP_STATUS_NOT_MODIFIED:                              // 304
    case HTTP_STATUS_USE_PROXY:                                 // 305
    case HTTP_STATUS_TEMPORARY_REDIRECT:                        // 307
      DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_CONN_PERSISTENT);
      DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_RESP_LOCATION);
      is_err = DEF_NO;
      break;

    //                                                             Error status codes.
    case HTTP_STATUS_BAD_REQUEST:                               // 400
    case HTTP_STATUS_UNAUTHORIZED:                              // 401
    case HTTP_STATUS_FORBIDDEN:                                 // 403
    case HTTP_STATUS_NOT_FOUND:                                 // 404
    case HTTP_STATUS_METHOD_NOT_ALLOWED:                        // 405
    case HTTP_STATUS_NOT_ACCEPTABLE:                            // 406
    case HTTP_STATUS_REQUEST_TIMEOUT:                           // 408
    case HTTP_STATUS_CONFLICT:                                  // 409
    case HTTP_STATUS_GONE:                                      // 410
    case HTTP_STATUS_LENGTH_REQUIRED:                           // 411
    case HTTP_STATUS_PRECONDITION_FAILED:                       // 412
    case HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE:                  // 413
    case HTTP_STATUS_REQUEST_URI_TOO_LONG:                      // 414
    case HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE:                    // 415
    case HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE:           // 416
    case HTTP_STATUS_EXPECTATION_FAILED:                        // 417

    //                                                             Server failed status codes.
    case HTTP_STATUS_INTERNAL_SERVER_ERR:                       // 500
    case HTTP_STATUS_NOT_IMPLEMENTED:                           // 501
    case HTTP_STATUS_SERVICE_UNAVAILABLE:                       // 503
    case HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED:                // 505
    default:
      DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_LOCATION);
      is_err = DEF_YES;
      break;
  }

  switch (p_conn->StatusCode) {
    case HTTP_STATUS_OK:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_OK"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeOKCtr);
      break;

    case HTTP_STATUS_MOVED_PERMANENTLY:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_MOVED_PERMANENTLY"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeMovedPermanentlyCtr);
      break;

    case HTTP_STATUS_FOUND:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_FOUND"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeFoundCtr);
      break;

    case HTTP_STATUS_SEE_OTHER:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_SEE_OTHER"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeSeeOtherCtr);
      break;

    case HTTP_STATUS_NOT_MODIFIED:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_NOT_MODIFIED"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeNotModifiedCtr);
      break;

    case HTTP_STATUS_USE_PROXY:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_USE_PROXY"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeUseProxyCtr);
      break;

    case HTTP_STATUS_TEMPORARY_REDIRECT:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_TEMPORARY_REDIRECT"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeTemporaryredirectCtr);
      break;

    case HTTP_STATUS_CREATED:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_CREATED"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeCreatedCtr);
      break;

    case HTTP_STATUS_ACCEPTED:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_ACCEPTED"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeAcceptedCtr);
      break;

    case HTTP_STATUS_NO_CONTENT:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_NO_CONTENT"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeNoContentCtr);
      break;

    case HTTP_STATUS_RESET_CONTENT:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_RESET_CONTENT"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeResetContentCtr);
      break;

    case HTTP_STATUS_BAD_REQUEST:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_BAD_REQUEST"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeBadRequestCtr);
      break;

    case HTTP_STATUS_UNAUTHORIZED:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_UNAUTHORIZED"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeUnauthorizedCtr);
      break;

    case HTTP_STATUS_FORBIDDEN:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_FORBIDDEN"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeForbiddenCtr);
      break;

    case HTTP_STATUS_NOT_FOUND:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_NOT_FOUND"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeNotFoundCtr);
      break;

    case HTTP_STATUS_METHOD_NOT_ALLOWED:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_METHOD_NOT_ALLOWED"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeMethodNotAllowedCtr);
      break;

    case HTTP_STATUS_NOT_ACCEPTABLE:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_NOT_ACCEPTABLE"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeNotAcceptableCtr);
      break;

    case HTTP_STATUS_REQUEST_TIMEOUT:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_REQUEST_TIMEOUT"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeRequestTimeoutCtr);
      break;

    case HTTP_STATUS_CONFLICT:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_CONFLICT"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeConflitCtr);
      break;

    case HTTP_STATUS_GONE:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_GONE"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeGoneCtr);
      break;

    case HTTP_STATUS_LENGTH_REQUIRED:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_LENGTH_REQUIRED"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeLenRequiredCtr);
      break;

    case HTTP_STATUS_PRECONDITION_FAILED:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_PRECONDITION_FAILED"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeConditionFailedCtr);
      break;

    case HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeEntityTooLongCtr);
      break;

    case HTTP_STATUS_REQUEST_URI_TOO_LONG:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_REQUEST_URI_TOO_LONG"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeURI_TooLongCtr);
      break;

    case HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeUnsupportedTypeCtr);
      break;

    case HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeNotSatisfiableCtr);
      break;

    case HTTP_STATUS_EXPECTATION_FAILED:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_EXPECTATION_FAILED"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeExpectationFailedCtr);
      break;

    case HTTP_STATUS_INTERNAL_SERVER_ERR:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_INTERNAL_SERVER_ERR"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeInternalServerErrCtr);
      break;

    case HTTP_STATUS_NOT_IMPLEMENTED:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_NOT_IMPLEMENTED"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeNotImplementedCtr);
      break;

    case HTTP_STATUS_SERVICE_UNAVAILABLE:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_SERVICE_UNAVAILABLE"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeSerUnavailableCtr);
      break;

    case HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Status = HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED"));
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeVerNotSupportedCtr);
      break;

    default:
      HTTPs_STATS_INC(p_ctr_stats->Resp_StatStatusCodeUnknownCtr);
      break;
  }

  return (is_err);
}

/****************************************************************************************************//**
 *                                       HTTPsResp_PrepareBodyData()
 *
 * @brief    Open file for the connection, if required.
 *           Validate null file pointer.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @return   DEF_OK    if the Body Data is ready to be processed.
 *           DEF_FAIL  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN HTTPsResp_PrepareBodyData(HTTPs_INSTANCE *p_instance,
                                             HTTPs_CONN     *p_conn)
{
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
  CPU_BOOLEAN     chunk_en = DEF_NO;
#endif
  CPU_BOOLEAN         is_body_data_rdy = DEF_NO;
  HTTPs_INSTANCE_ERRS *p_ctr_err = DEF_NULL;

#if ((HTTPs_CFG_CTR_ERR_EN == DEF_DISABLED) \
  && (HTTPs_CFG_FS_PRESENT_EN == DEF_DISABLED))
  PP_UNUSED_PARAM(p_instance);
#endif
  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  switch (p_conn->RespBodyDataType) {
    //                                                             ------------------ BODY IS A FILE ------------------
    case HTTPs_BODY_DATA_TYPE_FILE:
#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
      if (p_conn->PathPtr == DEF_NULL) {                        // Check that path is valid.
        HTTPs_ERR_INC(p_ctr_err->Resp_ErrPathInvalidCtr);
        is_body_data_rdy = DEF_NO;
        goto exit;
      }

      is_body_data_rdy = HTTPsResp_FileOpen(p_instance,         // Open file.
                                            p_conn);
      if (is_body_data_rdy != DEF_YES) {
        goto exit;
      }
      //                                                           Get Content Type from file extension.
      if (p_conn->RespContentType == HTTP_CONTENT_TYPE_UNKNOWN) {
        p_conn->RespContentType = HTTP_GetContentTypeFromFileExt(p_conn->PathPtr,
                                                                 p_conn->PathLenMax);
        if (p_conn->RespContentType == HTTP_CONTENT_TYPE_UNKNOWN) {
          HTTPs_ERR_INC(p_ctr_err->Resp_ErrContentTypeInvalidCtr);
          is_body_data_rdy = DEF_NO;
          goto exit;
        }
      }
      //                                                           Check if Chunked Transfer Encoding is needed.
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
      chunk_en = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED);
      if ( (p_conn->ProtocolVer == HTTP_PROTOCOL_VER_1_1)                    // If protocol ver is 1.1 ...
           && (p_cfg->TokenCfgPtr != DEF_NULL)                               // ..token parsing is enabled ..
           && (chunk_en == DEF_YES)
           && ((p_conn->RespContentType == HTTP_CONTENT_TYPE_HTML)          // ..content type is : html  ..
               || (p_conn->RespContentType == HTTP_CONTENT_TYPE_PLAIN))) {  // ..                  plain ..
        DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED);
      } else {
        DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
      }
#else
      DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
#endif
      break;
#else
      HTTPs_ERR_INC(p_ctr_err->File_ErrOpenNoFS_Ctr);
      is_body_data_rdy = DEF_NO;
      goto exit;
#endif

    //                                                             --------------- BODY IS STATIC DATA ----------------
    case HTTPs_BODY_DATA_TYPE_STATIC_DATA:
      //                                                           Validate Content Type of data.
      if (p_conn->RespContentType == HTTP_CONTENT_TYPE_UNKNOWN) {
        HTTPs_ERR_INC(p_ctr_err->Resp_ErrContentTypeInvalidCtr);
        is_body_data_rdy = DEF_NO;
        goto exit;
      }
      //                                                           Check if data pointer is defined.
      if (p_conn->DataPtr == DEF_NULL) {
        DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED_HOOK);
        is_body_data_rdy = DEF_YES;
      } else {
        DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED_HOOK);
        is_body_data_rdy = (p_conn->DataLen > 0) ? DEF_YES : DEF_NO;
      }

      //                                                           Check if Chunked Transfer Encoding is needed.
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
      chunk_en = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED);
      if ( (p_conn->ProtocolVer == HTTP_PROTOCOL_VER_1_1)                    // If protocol ver is 1.1 ...
           && (p_cfg->TokenCfgPtr != DEF_NULL)                               // ..token parsing is enabled ..
           && (chunk_en == DEF_YES)
           && ((p_conn->RespContentType == HTTP_CONTENT_TYPE_HTML)           // ..content type is : html  ..
               || (p_conn->RespContentType == HTTP_CONTENT_TYPE_PLAIN))) {   // ..                  plain ..
        DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED);
      } else {
        DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
      }
#else
      DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
#endif
      break;

    //                                                             ---------------- NO BODY IS PRESENT ----------------
    case HTTPs_BODY_DATA_TYPE_NONE:
      is_body_data_rdy = DEF_YES;
      break;

    default:
      HTTPs_ERR_INC(p_ctr_err->Resp_ErrBodyTypeInvalidCtr);
      is_body_data_rdy = DEF_NO;
      p_conn->ErrCode = HTTPs_ERR_RESP_BODY_DATA_TYPE_UNKNOWN;
      break;
  }

exit:
  if (is_body_data_rdy == DEF_NO) {
    p_conn->StatusCode = HTTP_STATUS_NOT_FOUND;
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           HTTPsResp_StatusLine()
 *
 * @brief    (1) Prepare response status line:
 *               - (a) Copy protocol version
 *               - (b) Copy status code
 *               - (c) Copy Reason phrase
 *               - (d) Copy end of status line
 *               - (e) Update connection parameters
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Connection buffer size must be configured to handle at least the entire status line.
 *
 * @note     (3) RFC #2616, Section 6 'Response' specifies how a response message must be structured:
 **              - (a) After receiving and interpreting a request message, a server responds with an HTTP
 *                     response message.
 *
 *                     Response      = Status-Line               ; Section 6.1
 *                     *(( general-header        ; Section 4.5
 *                      | response-header        ; Section 6.2
 *                      | entity-header ) CRLF)  ; Section 7.1
 *                     CRLF
 *                     [ message-body ]          ; Section 7.2
 *
 * @note     (4) RFC #2616, Section 6.1 'Status Line' specifies how the response status line message must be
 *               structured:
 *               - (a) 6.1 Status-Line
 *                     The first line of a Response message is the Status-Line, consisting of the protocol
 *                     version followed by a numeric status code and its associated textual phrase, with each
 *                     element separated by SP characters. No CR or LF is allowed except in the final CRLF
 *                     sequence.
 *                       @verbatim
 *                     Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
 *                       @endverbatim
 *               - (b) 6.1.1 Status Code and Reason Phrase
 *                     The Status-Code element is a 3-digit integer result code of the attempt to understand
 *                     and satisfy the request. These codes are fully defined in section 10. The Reason-Phrase
 *                     is intended to give a short textual description of the Status-Code. The Status-Code is
 *                     intended for use by automata and the Reason-Phrase is intended for the human user. The
 *                     client is not required to examine or display the Reason- Phrase.
 *
 *                     The first digit of the Status-Code defines the class of response. The last two digits
 *                     do not have any categorization role. There are 5 values for the first digit:
 *                     - 1xx: Informational - Request received, continuing process
 *                     - 2xx: Success - The action was successfully received,
 *                       understood, and accepted
 *                     - 3xx: Redirection - Further action must be taken in order to
 *                       complete the request
 *                     - 4xx: Client Error - The request contains bad syntax or cannot
 *                       be fulfilled
 *                     - 5xx: Server Error - The server failed to fulfill an apparently
 *                       valid request
 *******************************************************************************************************/
static void HTTPsResp_StatusLine(HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN     *p_conn,
                                 HTTPs_ERR      *p_err)
{
  CPU_CHAR   *p_buf;
  CPU_CHAR   *p_buf_wr;
  CPU_SIZE_T buf_len;

  PP_UNUSED_PARAM(p_instance);                                  // Prevent possible 'variable unused' warnings.

  p_buf = p_conn->TxBufPtr;
  p_buf += p_conn->TxDataLen;
  p_buf_wr = p_buf;
  buf_len = p_conn->BufLen - p_conn->TxDataLen;

  //                                                               ---------------- COPY PROTOCOL VER -----------------
  p_buf_wr = HTTP_Dict_ValCopy(HTTP_Dict_ProtocolVer,            // See Note #2a.
                               HTTP_Dict_ProtocolVerSize,
                               p_conn->ProtocolVer,
                               p_buf_wr,
                               buf_len);
  if (p_buf_wr == DEF_NULL) {
    *p_err = HTTPs_ERR_RESP_STATUS_LINE;
    return;
  }

  *p_buf_wr = ASCII_CHAR_SPACE;
  p_buf_wr++;

  //                                                               ----------------- COPY STATUS CODE -----------------
  buf_len = p_conn->BufLen - (p_buf_wr - p_buf);
  p_buf_wr = HTTP_Dict_ValCopy(HTTP_Dict_StatusCode,         // See Note #2b.
                               HTTP_Dict_StatusCodeSize,
                               p_conn->StatusCode,
                               p_buf_wr,
                               buf_len);
  if (p_buf_wr == DEF_NULL) {
    *p_err = HTTPs_ERR_RESP_STATUS_LINE;
    return;
  }

  *p_buf_wr = ASCII_CHAR_SPACE;
  p_buf_wr++;

  //                                                               ---------------- COPY REASON PHRASE ----------------
  buf_len = p_conn->BufLen - (p_buf_wr - p_buf);
  p_buf_wr = HTTP_Dict_ValCopy(HTTP_Dict_ReasonPhrase,       // See Note #2b.
                               HTTP_Dict_ReasonPhraseSize,
                               p_conn->StatusCode,
                               p_buf_wr,
                               buf_len);
  if (p_buf_wr == DEF_NULL) {
    *p_err = HTTPs_ERR_RESP_STATUS_LINE;
    return;
  }

  //                                                               ------------- COPY END OF STATUS LINE --------------
  buf_len = p_conn->BufLen - (p_buf_wr - p_buf);
  (void)Str_Copy_N(p_buf_wr, STR_CR_LF, buf_len);

  //                                                               ---------------- UPDATE CONN PARAM -----------------
  p_conn->TxDataLen += (p_buf_wr - p_buf) + STR_CR_LF_LEN;

  *p_err = HTTPs_ERR_NONE;
}

/****************************************************************************************************//**
 *                                               HTTPsResp_Hdr()
 *
 * @brief    (1) Prepare response headers:
 *               - (a) Add content type   header field
 *               - (b) Add content transfer header field
 *                   - (A) Content length
 *                   - (B) Transfer Encoding
 *
 *               - (c) Add location       header field
 *               - (d) Add connection     header field
 *               - (3) Copy end of response section
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) RFC #2616, Section 6 'Response' specifies how a response message must be structured:
 *               - (a) After receiving and interpreting a request message, a server responds with an HTTP
 *                     response message.
 *
 *                     Response      = Status-Line               ; Section 6.1
 *                     *(( general-header        ; Section 4.5
 *                      | response-header        ; Section 6.2
 *                      | entity-header ) CRLF)  ; Section 7.1
 *                      CRLF
 *                     [ message-body ]          ; Section 7.2
 *
 * @note     (3) RFC #2616, Section 6.2 'Response Header Fields' specifies how the response header fields
 *               message must be structured:
 *               - (a) 6.2 Response Header Fields
 *                     The response-header fields allow the server to pass additional information about the
 *                     response which cannot be placed in the Status- Line. These header fields give
 *                     information about the server and about further access to the resource identified by
 *                     the Request-URI.
 *                     response-header = Accept-Ranges           ; Section 14.5
 *                     | Age                     ; Section 14.6
 *                     | ETag                    ; Section 14.19
 *                     | Location                ; Section 14.30
 *                     | Proxy-Authenticate      ; Section 14.33
 *                     | Retry-After             ; Section 14.37
 *                     | Server                  ; Section 14.38
 *                     | Vary                    ; Section 14.44
 *                     | WWW-Authenticate        ; Section 14.47
 *
 *                     Response-header field names can be extended reliably only in combination with a change
 *                     in the protocol version. However, new or experimental header fields MAY be given the
 *                     semantics of response- header fields if all parties in the communication recognize them
 *                     to be response-header fields. Unrecognized header fields are treated as entity-header
 *                     fields.
 *******************************************************************************************************/
static void HTTPsResp_Hdr(HTTPs_INSTANCE *p_instance,
                          HTTPs_CONN     *p_conn,
                          HTTPs_ERR      *p_err)
{
  CPU_CHAR    *p_buf = p_conn->BufPtr + p_conn->TxDataLen;
  CPU_CHAR    *p_buf_wr = p_buf;
  CPU_CHAR    *p_str = DEF_NULL;
  CPU_SIZE_T  buf_len = 0;
  CPU_BOOLEAN chunk_en = DEF_NO;
  CPU_BOOLEAN chunk_hook_en = DEF_NO;
  CPU_BOOLEAN chuncked = DEF_NO;
  CPU_BOOLEAN done = DEF_NO;
  CPU_BOOLEAN location_needed = DEF_NO;
#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
  CPU_BOOLEAN     hook_done = DEF_NO;
  CPU_BOOLEAN     hook_def = DEF_NO;
#endif
#if (HTTPs_CFG_PERSISTENT_CONN_EN == DEF_ENABLED)
  CPU_BOOLEAN persistent = DEF_NO;
#endif
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;

  while (done != DEF_YES) {
    switch (p_conn->State) {
      case HTTPs_CONN_STATE_RESP_HDR:
        if (p_conn->RespBodyDataType == HTTPs_BODY_DATA_TYPE_NONE) {
          p_conn->State = HTTPs_CONN_STATE_RESP_HDR_LOCATION;
        } else {
          p_conn->State = HTTPs_CONN_STATE_RESP_HDR_CONTENT_TYPE;
        }
        break;

      case HTTPs_CONN_STATE_RESP_HDR_CONTENT_TYPE:
        buf_len = p_conn->BufLen - p_conn->TxDataLen - (p_buf_wr - p_buf);
        p_str = HTTPsResp_HdrFieldAdd(p_instance,
                                      p_conn,
                                      p_buf_wr,
                                      buf_len,
                                      HTTP_HDR_FIELD_CONTENT_TYPE,
                                      DEF_NULL,
                                      0,
                                      p_err);
        switch (*p_err) {
          case HTTPs_ERR_NONE:
            break;

          case HTTPs_ERR_RESP_BUF_NO_MORE_SPACE:
            p_conn->State = HTTPs_CONN_STATE_RESP_HDR_CONTENT_TYPE;
            goto exit;

          default:
            goto exit;
        }

        p_buf_wr = p_str;
        p_conn->State = HTTPs_CONN_STATE_RESP_HDR_FILE_TRANSFER;
        break;

      case HTTPs_CONN_STATE_RESP_HDR_FILE_TRANSFER:
        //                                                         ---------- ADD CONTENT TRANSFER HDR FIELD ----------
        buf_len = p_conn->BufLen - p_conn->TxDataLen - (p_buf_wr - p_buf);

        chunk_en = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED);
        chunk_hook_en = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED_HOOK);

        if ((chunk_en == DEF_YES)
            || (chunk_hook_en == DEF_YES)) {
          chuncked = DEF_YES;
        }

        if (chuncked == DEF_YES) {
          //                                                       --------- ADD TRANSFER ENCODING HDR FIELD ----------
          p_str = HTTPsResp_HdrFieldAdd(p_instance,
                                        p_conn,
                                        p_buf_wr,
                                        buf_len,
                                        HTTP_HDR_FIELD_TRANSFER_ENCODING,
                                        DEF_NULL,
                                        0,
                                        p_err);
          switch (*p_err) {
            case HTTPs_ERR_NONE:
              break;

            case HTTPs_ERR_RESP_BUF_NO_MORE_SPACE:
              p_conn->State = HTTPs_CONN_STATE_RESP_HDR_FILE_TRANSFER;
              goto exit;

            default:
              goto exit;
          }
        } else {
          //                                                       ------------ ADD CONTENT LEN HDR FIELD -------------
          p_str = HTTPsResp_HdrFieldAdd(p_instance,
                                        p_conn,
                                        p_buf_wr,
                                        buf_len,
                                        HTTP_HDR_FIELD_CONTENT_LEN,
                                        DEF_NULL,
                                        0,
                                        p_err);
          switch (*p_err) {
            case HTTPs_ERR_NONE:
              break;

            case HTTPs_ERR_RESP_BUF_NO_MORE_SPACE:
              p_conn->State = HTTPs_CONN_STATE_RESP_HDR_FILE_TRANSFER;
              goto exit;

            default:
              goto exit;
          }
        }
        p_buf_wr = p_str;
        p_conn->State = HTTPs_CONN_STATE_RESP_HDR_LOCATION;
        break;

      case HTTPs_CONN_STATE_RESP_HDR_LOCATION:
        //                                                         -------------- ADD LOCATION HDR FIELD --------------
        location_needed = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_RESP_LOCATION);
        if (location_needed == DEF_YES) {
          buf_len = p_conn->BufLen - p_conn->TxDataLen - (p_buf_wr - p_buf);
          p_str = HTTPsResp_HdrFieldAdd(p_instance,
                                        p_conn,
                                        p_buf_wr,
                                        buf_len,
                                        HTTP_HDR_FIELD_LOCATION,
                                        DEF_NULL,
                                        0,
                                        p_err);
          switch (*p_err) {
            case HTTPs_ERR_NONE:
              break;

            case HTTPs_ERR_RESP_BUF_NO_MORE_SPACE:
              p_conn->State = HTTPs_CONN_STATE_RESP_HDR_LOCATION;
              goto exit;

            default:
              goto exit;
          }

          p_buf_wr = p_str;
        }
        p_conn->State = HTTPs_CONN_STATE_RESP_HDR_CONN;
        break;

      case HTTPs_CONN_STATE_RESP_HDR_CONN:
#if (HTTPs_CFG_PERSISTENT_CONN_EN == DEF_ENABLED)
        persistent = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_CONN_PERSISTENT);
        if (persistent == DEF_NO) {
#endif
        //                                                         ---------------- ADD CONN HDR FIELD ----------------
        buf_len = p_conn->BufLen - p_conn->TxDataLen - (p_buf_wr - p_buf);
        p_str = HTTPsResp_HdrFieldAdd(p_instance,
                                      p_conn,
                                      p_buf_wr,
                                      buf_len,
                                      HTTP_HDR_FIELD_CONN,
                                      DEF_NULL,
                                      0,
                                      p_err);
        switch (*p_err) {
          case HTTPs_ERR_NONE:
            break;

          case HTTPs_ERR_RESP_BUF_NO_MORE_SPACE:
            p_conn->State = HTTPs_CONN_STATE_RESP_HDR_CONN;
            goto exit;

          default:
            goto exit;
        }

        p_buf_wr = p_str;
#if (HTTPs_CFG_PERSISTENT_CONN_EN == DEF_ENABLED)
    }
#endif
        p_conn->State = HTTPs_CONN_STATE_RESP_HDR_LIST;
        break;

      case HTTPs_CONN_STATE_RESP_HDR_LIST:
#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
        hook_def = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnRespHdrTxHook);
        if ((p_cfg->HdrTxCfgPtr != DEF_NULL)
            && (hook_def == DEF_YES)) {
          hook_done = p_cfg->HooksPtr->OnRespHdrTxHook(p_instance,
                                                       p_conn,
                                                       p_cfg->Hooks_CfgPtr);
          if (hook_done != DEF_YES) {
            *p_err = HTTPs_ERR_RESP_BUF_NO_MORE_SPACE;
            p_conn->State = HTTPs_CONN_STATE_RESP_HDR_LIST;
            goto exit;
          }
          p_conn->State = HTTPs_CONN_STATE_RESP_HDR_TX;
        } else {
          p_conn->State = HTTPs_CONN_STATE_RESP_HDR_END;
        }
#else
        p_conn->State = HTTPs_CONN_STATE_RESP_HDR_END;
#endif
        break;

      case HTTPs_CONN_STATE_RESP_HDR_TX:
#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
        while (p_conn->HdrListPtr != DEF_NULL) {
          buf_len = p_conn->BufLen - p_conn->TxDataLen - (p_buf_wr - p_buf);

          p_str = HTTPsResp_HdrFieldAdd(p_instance,
                                        p_conn,
                                        p_buf_wr,
                                        buf_len,
                                        p_conn->HdrListPtr->HdrField,
                                        (CPU_CHAR *)p_conn->HdrListPtr->ValPtr,
                                        p_conn->HdrListPtr->ValLen,
                                        p_err);
          switch (*p_err) {
            case HTTPs_ERR_NONE:
              break;

            case HTTPs_ERR_RESP_BUF_NO_MORE_SPACE:
              p_conn->State = HTTPs_CONN_STATE_RESP_HDR_TX;
              goto exit;

            default:
              goto exit;
          }

          p_buf_wr = p_str;

          HTTPsMem_RespHdrRelease(p_instance,                   // Release hdr blk once copied in conn buff.
                                  p_conn);
        }
#endif
        p_conn->State = HTTPs_CONN_STATE_RESP_HDR_END;
        break;

      case HTTPs_CONN_STATE_RESP_HDR_END:
        //                                                         --------------- COPY END OF RESP SEC ---------------
        buf_len = p_conn->BufLen - p_conn->TxDataLen - (p_buf_wr - p_buf);
        if (buf_len < STR_CR_LF_LEN) {
          p_conn->State = HTTPs_CONN_STATE_RESP_HDR_END;
          *p_err = HTTPs_ERR_RESP_BUF_NO_MORE_SPACE;
          goto exit;
        }

        (void)Str_Copy_N(p_buf_wr, STR_CR_LF, buf_len);
        p_buf_wr += STR_CR_LF_LEN;
        *p_buf_wr = ASCII_CHAR_NULL;

        done = DEF_YES;
        break;

      default:
        *p_err = HTTPs_ERR_STATE_UNKNOWN;
        goto exit;
    }
  }

  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);
  HTTPs_STATS_INC(p_ctr_stats->Resp_StatTxdCtr);

  *p_err = HTTPs_ERR_NONE;

exit:
  p_conn->TxDataLen += (p_buf_wr - p_buf);
  return;
}

/****************************************************************************************************//**
 *                                           HTTPsResp_HdrFieldAdd()
 *
 * @brief    (1) Add response header field:
 *               - (a) Copy header field title
 *               - (b) Copy header field value
 *               - (c) Copy end of header field
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    ------      Argument validated in HTTPs_InstanceStart().
 *
 * @param    p_buf       Pointer to the buffer where to copy header field.
 *
 * @param    -----       Argument validated in HTTPs_InstanceStart().
 *
 * @param    buf_len     Remaining length available in the buffer.
 *
 * @param    field_type  Header field type to add.
 *
 * @param    p_val       Pointer to value of header when an Hdr Fields list is present.
 *
 * @param    val_len     Length of the value.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to the end of the header field added, if field successfully copied.
 *           DEF_NULL, otherwise.
 *
 * @note     (2) RFC #2616, Section 6 'Response' specifies how a response message must be structured:
 *               - (a) After receiving and interpreting a request message, a server responds with an HTTP
 *                     response message.
 *
 *                       Response      = Status-Line               ; Section 6.1
 *                                       *(( general-header        ; Section 4.5
 *                                       | response-header        ; Section 6.2
 *                                       | entity-header ) CRLF)  ; Section 7.1
 *                                       CRLF
 *                                       [ message-body ]          ; Section 7.2
 *******************************************************************************************************/
static CPU_CHAR *HTTPsResp_HdrFieldAdd(HTTPs_INSTANCE *p_instance,
                                       HTTPs_CONN     *p_conn,
                                       CPU_CHAR       *p_buf,
                                       CPU_SIZE_T     buf_len,
                                       HTTP_HDR_FIELD field_type,
                                       CPU_CHAR       *p_val,
                                       CPU_SIZE_T     val_len,
                                       HTTPs_ERR      *p_err)
{
  HTTP_DICT *p_entry;
  CPU_SIZE_T len;
  CPU_SIZE_T len_value;
  CPU_CHAR *p_str;
  CPU_CHAR *p_path;
  CPU_BOOLEAN add_end_field = DEF_YES;;
#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
  CPU_SIZE_T len_host;
  CPU_SIZE_T len_tot;
#endif
#if ((HTTPs_CFG_HDR_TX_EN == DEF_ENABLED) \
  || (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED))
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
#endif

#if ((HTTPs_CFG_HDR_TX_EN == DEF_DISABLED)       \
  && (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_DISABLED) \
  && (HTTPs_CFG_FS_PRESENT_EN == DEF_DISABLED))
  PP_UNUSED_PARAM(p_instance);
#endif

#if (HTTPs_CFG_HDR_TX_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_val);
  PP_UNUSED_PARAM(val_len);
#endif
  //                                                               --------------- COPY HDR FIELD TITLE ---------------
  p_entry = HTTP_Dict_EntryGet(HTTP_Dict_HdrField,
                               HTTP_Dict_HdrFieldSize,
                               field_type);
  if (p_entry == DEF_NULL) {
    *p_err = HTTPs_ERR_HDR_FIELD_TYPE_UNKNOWN;
    return (DEF_NULL);
  }

  if (p_entry->StrLen > buf_len) {                              // Validate value len and buf len.
    if (buf_len == p_conn->BufLen) {
      *p_err = HTTPs_ERR_CFG_INVALID_BUF_LEN;
    } else {
      *p_err = HTTPs_ERR_RESP_BUF_NO_MORE_SPACE;
    }
    return (DEF_NULL);
  }

  (void)Str_Copy_N(p_buf, p_entry->StrPtr, p_entry->StrLen);    // Copy string to the buffer.
  p_str = p_buf + p_entry->StrLen;                              // Increment buffer pointer.

  *p_str = ASCII_CHAR_COLON;
  p_str++;
  *p_str = ASCII_CHAR_SPACE;
  p_str++;

  buf_len -= (p_buf - p_str);

  //                                                               ---------------- COPY HDR FIELD VAL ----------------
  switch (field_type) {
    case HTTP_HDR_FIELD_CONTENT_TYPE:
      p_entry = HTTP_Dict_EntryGet(HTTP_Dict_ContentType,
                                   HTTP_Dict_ContentTypeSize,
                                   p_conn->RespContentType);
      if (p_entry == DEF_NULL) {
        *p_err = HTTPs_ERR_HDR_FIELD_VAL_UNKNOWN;
        return (DEF_NULL);
      }

      if (p_entry->StrLen > buf_len) {                          // Validate value len and buf len.
        if (buf_len == p_conn->BufLen) {
          *p_err = HTTPs_ERR_CFG_INVALID_BUF_LEN;
        } else {
          *p_err = HTTPs_ERR_RESP_BUF_NO_MORE_SPACE;
        }
        return (DEF_NULL);
      }

      (void)Str_Copy_N(p_str, p_entry->StrPtr, p_entry->StrLen);       // Copy string to the buffer.
      p_str += p_entry->StrLen;                                        // Increment buffer pointer.
      break;

    case HTTP_HDR_FIELD_TRANSFER_ENCODING:
      p_entry = HTTP_Dict_EntryGet(HTTP_Dict_HdrFieldTransferTypeVal,
                                   HTTP_Dict_HdrFieldTransferTypeValSize,
                                   HTTP_HDR_FIELD_TRANSFER_TYPE_CHUNCKED);
      if (p_entry == DEF_NULL) {
        *p_err = HTTPs_ERR_HDR_FIELD_VAL_UNKNOWN;
        return (DEF_NULL);
      }

      if (p_entry->StrLen > buf_len) {                          // Validate value len and buf len.
        if (buf_len == p_conn->BufLen) {
          *p_err = HTTPs_ERR_CFG_INVALID_BUF_LEN;
        } else {
          *p_err = HTTPs_ERR_RESP_BUF_NO_MORE_SPACE;
        }
        return (DEF_NULL);
      }

      (void)Str_Copy_N(p_str, p_entry->StrPtr, p_entry->StrLen);       // Copy string to the buffer.
      p_str += p_entry->StrLen;                                        // Increment buffer pointer.
      break;

    case HTTP_HDR_FIELD_CONN:
      p_entry = HTTP_Dict_EntryGet(HTTP_Dict_HdrFieldConnVal,
                                   HTTP_Dict_HdrFieldConnValSize,
                                   HTTP_HDR_FIELD_CONN_CLOSE);
      if (p_entry == DEF_NULL) {
        *p_err = HTTPs_ERR_HDR_FIELD_VAL_UNKNOWN;
        return (DEF_NULL);
      }

      if (p_entry->StrLen > buf_len) {                          // Validate value len and buf len.
        if (buf_len == p_conn->BufLen) {
          *p_err = HTTPs_ERR_CFG_INVALID_BUF_LEN;
        } else {
          *p_err = HTTPs_ERR_RESP_BUF_NO_MORE_SPACE;
        }
        return (DEF_NULL);
      }

      (void)Str_Copy_N(p_str, p_entry->StrPtr, p_entry->StrLen);       // Copy string to the buffer.
      p_str += p_entry->StrLen;                                        // Increment buffer pointer.
      break;

    case HTTP_HDR_FIELD_CONTENT_LEN:
      p_str = Str_FmtNbr_Int32U(p_conn->DataLen,
                                DEF_INT_32U_NBR_DIG_MAX,
                                DEF_NBR_BASE_DEC,
                                ASCII_CHAR_NULL,
                                DEF_NO,
                                DEF_YES,
                                p_str);
      if (p_str == DEF_NULL) {
        return (DEF_NULL);
      }

      len = Str_Len_N(p_str, buf_len);
      p_str += len;
      break;

    case HTTP_HDR_FIELD_LOCATION:
#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
      p_path = HTTPs_StrPathGet(p_conn->PathPtr,
                                p_conn->PathLenMax,
                                p_conn->HostPtr,
                                p_cfg->HostNameLenMax,
                                DEF_NO);
#else
      p_path = p_conn->PathPtr;
#endif

      len_value = Str_Len_N(p_path, p_conn->PathLenMax);

#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
      len_host = Str_Len_N(p_conn->HostPtr, p_cfg->HostNameLenMax);
      len_tot = len_value + len_host;
      if (len_tot > buf_len) {
        return (DEF_NULL);
      }

      (void)Str_Copy_N(p_str, p_conn->HostPtr, len_host);
      p_str += len_host;
#else
      if (len_value > buf_len) {
        return (DEF_NULL);
      }
#endif

      Str_Copy_N(p_str, p_path, len_value);
#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
      if (p_instance->FS_PathSepChar != HTTPs_PATH_SEP_CHAR_DFLT) {
        (void)Str_Char_Replace_N(p_str,
                                 p_instance->FS_PathSepChar,
                                 HTTPs_PATH_SEP_CHAR_DFLT,
                                 buf_len);
      }
#endif

      p_str += len_value;
      break;

    default:
#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
      if (p_cfg->HdrTxCfgPtr != DEF_NULL) {
        //                                                         Validate that hdr len value has been set ...
        //                                                         ... correctly in callback fnct RespHdrListFnctPtr.
        if (val_len <= 0
            || val_len > p_cfg->HdrTxCfgPtr->DataLenMax) {
          break;
        }
        //                                                         Validate that conn buffer remaining len is enough ...
        //                                                         ... for the hdr field value.
        if (val_len > buf_len) {
          break;
        }

        Mem_Copy((void *)        p_str,                         // Copy hdr value in conn buff.
                 (const  void *) p_val,
                 (CPU_SIZE_T) val_len);

        p_str += val_len;                                       // Update conn buf ptr to end of data.
      }
      break;
#else
      add_end_field = DEF_NO;
      p_str = p_buf;
#endif
  }

  //                                                               -------------- COPY END OF HDR FIELD ---------------
  if (add_end_field == DEF_YES) {
    len = buf_len - (p_str - p_buf);
    (void)Str_Copy_N(p_str, STR_CR_LF, len);

    p_str += STR_CR_LF_LEN;
  }

  return (p_str);
}

/****************************************************************************************************//**
 *                                       HTTPsResp_DataTransferStd()
 *
 * @brief    Transfer message-body data with the standard way i.e with the Content-Length header and
 *           no body encoding.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if all data has been read.
 *           DEF_NO,  if data still need to be read.
 *
 * @note     (1) RFC #2616, Section 6 'Response' specifies how a response message must be structured:
 *               - (a) After receiving and interpreting a request message, a server responds with an HTTP
 *                     response message.
 *
 *                       Response      = Status-Line               ; Section 6.1
 *                                       *(( general-header        ; Section 4.5
 *                                       | response-header        ; Section 6.2
 *                                       | entity-header ) CRLF)  ; Section 7.1
 *                                       CRLF
 *                                       [ message-body ]          ; Section 7.2
 *******************************************************************************************************/
static CPU_BOOLEAN HTTPsResp_DataTransferStd(HTTPs_INSTANCE *p_instance,
                                             HTTPs_CONN     *p_conn,
                                             HTTPs_ERR      *p_err)
{
  void *p_buf = p_conn->TxBufPtr + p_conn->TxDataLen;
  CPU_SIZE_T buf_size = p_conn->BufLen   - p_conn->TxDataLen;
  CPU_SIZE_T size = 0;
  CPU_BOOLEAN done = DEF_NO;

  //                                                               ------------------- RD FILE DATA -------------------
  size = HTTPsResp_DataRd(p_instance,
                          p_conn,
                          (CPU_CHAR *)p_buf,
                          buf_size);
  if (size > 0) {                                               // File read successfully.
    p_conn->TxDataLen += size;
    p_conn->DataTxdLen += size;
  }
  //                                                               ------------ VALIDATE FILE RD COMPLETED ------------
  if (p_conn->DataTxdLen >= p_conn->DataLen) {                  // If file is completely read.
    done = DEF_YES;
  }

  *p_err = HTTPs_ERR_NONE;

  return (done);
}

/****************************************************************************************************//**
 *                                       HTTPsResp_DataTransferChunked()
 *
 * @brief    Transfers the message-body using chunked transfer coding.
 *           Calls the hook to get the data.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static CPU_BOOLEAN HTTPsResp_DataTransferChunkedWithHook(HTTPs_INSTANCE *p_instance,
                                                         HTTPs_CONN     *p_conn,
                                                         HTTPs_ERR      *p_err)
{
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
  HTTPs_INSTANCE_ERRS *p_ctr_err = DEF_NULL;
  CPU_SIZE_T len_to_tx = 0;
  CPU_SIZE_T max_dig_str_len;
  CPU_SIZE_T max_hdr_size;
  CPU_SIZE_T max_buf_free;
  CPU_BOOLEAN chunk_hook_def = DEF_NO;
  CPU_BOOLEAN is_last_chunk = DEF_NO;
  CPU_BOOLEAN done = DEF_NO;

  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  if (p_conn->TxDataLen > 0u) {                                 // All data must be transmitted before processing.
    *p_err = HTTPs_ERR_NONE;
    goto exit;
  }

  max_dig_str_len = HTTP_StrSizeHexDigReq(p_conn->BufLen);
  max_hdr_size = max_dig_str_len + STR_CR_LF_LEN;
  max_buf_free = p_conn->BufLen - max_hdr_size - STR_CR_LF_LEN;

  switch (p_conn->State) {
    case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_HOOK:
      chunk_hook_def = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnRespChunkHook);
      if (chunk_hook_def != DEF_YES) {
        *p_err = HTTPs_ERR_RESP_DATA_CHUNKED_HOOK_UNDEFINED;
        goto exit;
      }
      //                                                           If the hook for the chunk is defined ...
      //                                                           ... call the hook function.
      is_last_chunk = p_cfg->HooksPtr->OnRespChunkHook(p_instance,
                                                       p_conn,
                                                       p_cfg->Hooks_CfgPtr,
                                                       p_conn->TxBufPtr + max_hdr_size,
                                                       max_buf_free,
                                                       &len_to_tx);
      if (len_to_tx > max_buf_free) {
        *p_err = HTTPs_ERR_RESP_DATA_CHUNKED_LENGTH_INVALID;
        goto exit;
      }

      //                                                           If the length is within limits ...
      if (is_last_chunk == DEF_TRUE) {                          // ... if it is the last chunk ...
                                                                // .. set the state so the last chunk can be sent.
        p_conn->State = HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_FINALIZE;
      } else {                                                  // ... else insure the state didn't changed in the hook.
        p_conn->State = HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_HOOK;
      }

      p_conn->TxDataLen = HTTPsResp_WrChunkedToBuf(p_conn->TxBufPtr,
                                                   p_conn->BufLen,
                                                   len_to_tx,
                                                   max_dig_str_len);
      break;

    case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_FINALIZE:
      //                                                           1 is for the str length taken by a 0u in hex
      p_conn->TxDataLen = HTTPsResp_WrChunkedToBuf(p_conn->TxBufPtr,
                                                   p_conn->BufLen,
                                                   0u,
                                                   1);
      done = DEF_YES;
      break;

    default:
      HTTPs_ERR_INC(p_ctr_err->Resp_ErrTransferChunckedHookStateInvCtr);
      *p_err = HTTPs_ERR_STATE_UNKNOWN;
      goto exit;
  }

  *p_err = HTTPs_ERR_NONE;

exit:
  return (done);
}

/****************************************************************************************************//**
 *                                       HTTPsResp_WrChunkedToBuf()
 *
 * @brief    Encapsulate the data into the chunked encoding format.
 *           In order to avoid memcopies, this function assumes that the following requirement
 *           have been fulfilled:
 *               - (1) The data in the buffer must be placed from bufStart + lenDigitStrLen + STR_CR_LF_LEN.
 *               - (2) The buffer size must be at least lenDigitStrLen + 2 * STR_CR_LF_LEN + dataLen.
 *           The data can be place before or after the function call.
 *
 * @param    p_buf               Pointer to start of buffer.
 *
 * @param    buf_len             Length of buffer.
 *
 * @param    data_len            Length of the data inside the buffer.
 *
 * @param    len_dig_str_len     Length of the hex formatted string length.
 *
 * @return   Return the length of the the encoded data.
 *******************************************************************************************************/
static CPU_SIZE_T HTTPsResp_WrChunkedToBuf(CPU_CHAR   *p_buf,
                                           CPU_SIZE_T buf_len,
                                           CPU_SIZE_T data_len,
                                           CPU_SIZE_T len_dig_str_len)
{
  //                                                               Write the length of the data.
  HTTP_ChunkTransferWrSize(p_buf, buf_len, len_dig_str_len, data_len);

  //                                                               Skip the data.
  data_len += len_dig_str_len + STR_CR_LF_LEN;

  //                                                               Write the end of the data.
  Str_Copy_N(p_buf + data_len,
             STR_CR_LF,
             STR_CR_LF_LEN);

  data_len += STR_CR_LF_LEN;

  return (data_len);
}

/****************************************************************************************************//**
 *                                       HTTPsResp_FileTransferChunked()
 *
 * @brief    Transfer message-body (html & text document) using chunked transfer encoding.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if all data has been read.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) RFC #2616, Section 6 'Response' specifies how a response message must be structured:
 *           - (a) After receiving and interpreting a request message, a server responds with an HTTP
 *                 response message.
 *
 *                       Response      = Status-Line               ; Section 6.1
 *                                       *(( general-header        ; Section 4.5
 *                                       | response-header        ; Section 6.2
 *                                       | entity-header ) CRLF)  ; Section 7.1
 *                                       CRLF
 *                                       [ message-body ]          ; Section 7.2
 *
 * @note     (2) RFC #2616, Section 3.631 'Chunked Transfer Coding' specifies how a message-body must
 *               be structured:
 *           - (a) The chunked encoding modifies the body of a message in order to transfer it as a series of chunks,
 *                 each with its own size indicator, followed by an OPTIONAL trailer containing entity-header fields.
 *                 This allows dynamically produced content to be transferred along with the information necessary
 *                 for the recipient to verify that it has received the full message.
 *
 *                       Chunked-Body   = *chunk
 *                                       last-chunk
 *                                       trailer
 *                                       CRLF
 *
 *                       chunk          = chunk-size [ chunk-extension ] CRLF
 *                                       chunk-data CRLF
 *                       chunk-size     = 1*HEX
 *                       last-chunk     = 1*("0") [ chunk-extension ] CRLF
 *
 *                       chunk-extension= *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
 *                       chunk-ext-name = token
 *                       chunk-ext-val  = token | quoted-string
 *                       chunk-data     = chunk-size(OCTET)
 *                       trailer        = *(entity-header CRLF)
 *
 *                   The chunk-size field is a string of hex digits indicating the size of the chunk. The chunked
 *                   encoding is ended by any chunk whose size is zero, followed by the trailer, which is terminated
 *                   by an empty line.
 *******************************************************************************************************/
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsResp_DataTransferChunked(HTTPs_INSTANCE *p_instance,
                                                 HTTPs_CONN     *p_conn,
                                                 HTTPs_ERR      *p_err)
{
  const HTTPs_CFG *p_cfg;
  CPU_CHAR *p_buf_hdr_start;
  CPU_CHAR *p_buf_hdr_end;
  CPU_CHAR *p_buf_data;
  CPU_CHAR *p_token_str;
  CPU_CHAR *p_token_val_start;
  CPU_CHAR *p_token_val_end;
  CPU_CHAR *p_token_val_data;
  CPU_CHAR *p_wr;
  HTTPs_INSTANCE_ERRS *p_ctr_err = DEF_NULL;
  HTTPs_INSTANCE_STATS *p_ctr_stat = DEF_NULL;
  CPU_SIZE_T size_buf_data;
  CPU_SIZE_T size_rd;
  CPU_INT08U nbr_dig;
  CPU_INT32S offset;
  CPU_BOOLEAN tx_buf;
  CPU_BOOLEAN tx_token;
  CPU_BOOLEAN result;
  CPU_BOOLEAN chunk_hook_def;
  CPU_BOOLEAN done = DEF_NO;
  HTTPs_TOKEN_TYPE token_type;
  HTTPs_ERR err;
  RTOS_ERR local_err;

  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);
  HTTPs_SET_PTR_STATS(p_ctr_stat, p_instance);

  p_cfg = p_instance->CfgPtr;

  if (p_cfg->TokenCfgPtr == DEF_NULL) {
    *p_err = HTTPs_ERR_CFG_NULL_PTR_TOKEN;
    goto exit;
  }

  if (p_conn->TxDataLen > 0u) {                                 // All data must be transmitted before processing.
    *p_err = HTTPs_ERR_NONE;
    goto exit;
  }

  token_type = HTTPs_TOKEN_TYPE_NONE;

  switch (p_conn->State) {
    case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED:
      tx_token = DEF_NO;
      tx_buf = DEF_YES;

      //                                                           ------------------- RD FILE DATA -------------------
      if (p_conn->TokenBufRemLen == 0u) {
        p_buf_hdr_start = p_conn->TxBufPtr;
        p_buf_data = p_buf_hdr_start + HTTP_STR_BUF_TOP_SPACE_REQ_MIN;
        size_buf_data = p_conn->BufLen - (HTTP_STR_BUF_TOP_SPACE_REQ_MIN + HTTP_STR_BUF_END_SPACE_REQ_MIN);

        size_rd = HTTPsResp_DataRd(p_instance,
                                   p_conn,
                                   p_buf_data,
                                   size_buf_data);

        size_buf_data = size_rd;

        if (size_buf_data == 0u) {
          tx_buf = DEF_NO;
        }

        p_conn->TokenBufRemLen = size_buf_data;

        //                                                         ---------------- AQUIRING TOKEN BLK ----------------
      } else if (p_conn->TokenCtrlPtr == DEF_NULL) {
        (void)HTTPsMem_TokenGet(p_instance, p_conn, &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          *p_err = HTTPs_ERR_TOKEN_POOL_GET;
          goto exit;
        }

        p_buf_hdr_start = p_conn->TxBufPtr;
        p_buf_data = p_buf_hdr_start + HTTP_STR_BUF_TOP_SPACE_REQ_MIN;
        size_buf_data = p_conn->TokenBufRemLen;
        tx_token = DEF_YES;

        //                                                         --------------- PROCESSING REM DATA ----------------
      } else {
        p_buf_data = p_conn->TokenPtr + p_conn->TokenLen;
        size_buf_data = p_conn->TokenBufRemLen;
        p_buf_hdr_start = p_buf_data - HTTP_STR_BUF_TOP_SPACE_REQ_MIN;
      }

      //                                                           -------------------- FIND TOKEN --------------------
      if ((tx_token == DEF_NO)
          && (size_buf_data > 0u)    ) {
        p_conn->TokenLen = HTTPsResp_TokenFinder(p_buf_data,
                                                 size_buf_data,
                                                 &p_conn->TokenPtr,
                                                 &token_type,
                                                 &err);
        switch (err) {
          case HTTPs_ERR_NONE:                                  // Token found.
                                                                // If token blk not already aquired...
            if (p_conn->TokenCtrlPtr == DEF_NULL) {
              //                                                   ... acquire token blk.
              (void)HTTPsMem_TokenGet(p_instance, p_conn, &local_err);
              if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
                *p_err = HTTPs_ERR_TOKEN_POOL_GET;
                goto exit;
              }
            }
            HTTPs_STATS_INC(p_ctr_stat->Resp_StatTokenFoundCtr);
            tx_token = DEF_YES;                                 // Let prepare token chunk.
            break;

          case HTTPs_ERR_TOKEN_MORE_DATA_REQ:                   // Half of token found.
            offset = 0 - (size_buf_data - (p_conn->TokenPtr - p_buf_data));
            size_buf_data += offset;
            p_conn->TokenBufRemLen = 0;
            HTTPsResp_DataSetPos(p_instance,
                                 p_conn,
                                 offset,
                                 NET_FS_SEEK_ORIGIN_CUR);
            break;

          case HTTPs_ERR_TOKEN_NO_TOKEN_FOUND:
          default:
            p_conn->TokenBufRemLen = 0;
            break;
        }
      }

      //                                                           --------------- PREPARE TOKEN CHUNK ----------------
      if (tx_token == DEF_YES) {
        p_token_str = p_conn->TokenPtr             + HTTPs_TOKEN_CHAR_OFFSET_LEN;
        p_token_val_start = p_conn->TokenCtrlPtr->ValPtr;
        p_token_val_data = p_conn->TokenCtrlPtr->ValPtr + HTTP_STR_BUF_TOP_SPACE_REQ_MIN;

        switch (token_type) {
          case HTTPs_TOKEN_TYPE_EXTERNAL:
            chunk_hook_def = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnRespTokenHook);
            if (chunk_hook_def == DEF_YES) {
              result = p_cfg->HooksPtr->OnRespTokenHook(p_instance,
                                                        p_conn,
                                                        p_cfg->Hooks_CfgPtr,
                                                        p_token_str,
                                                        p_conn->TokenLen,
                                                        p_token_val_data,
                                                        p_cfg->TokenCfgPtr->ValLenMax);
            } else {
              HTTPs_ERR_INC(p_ctr_err->Resp_ErrTokenPtrNullCtr);
              result = DEF_FAIL;
            }
            break;

          case HTTPs_TOKEN_TYPE_INTERNAL:
            result = HTTPsResp_TokenValSet(p_instance,
                                           p_conn,
                                           p_token_str,
                                           p_conn->TokenLen,
                                           p_token_val_data,
                                           p_cfg->TokenCfgPtr->ValLenMax);
            break;

          default:
            HTTPs_ERR_INC(p_ctr_err->Resp_ErrTokenTypeInvalidCtr);
            result = DEF_FAIL;
            break;
        }

        if (result == DEF_OK) {
          p_conn->TokenCtrlPtr->ValLen = Str_Len_N(p_token_val_data, p_cfg->TokenCfgPtr->ValLenMax);
          if (p_conn->TokenCtrlPtr->ValLen == 0u) {
            tx_token = DEF_NO;
          }
        } else {
          Mem_Set(p_token_val_data, HTTPs_TOKEN_CHAR_DFLT_VAL, p_cfg->TokenCfgPtr->ValLenMax);
          p_conn->TokenCtrlPtr->ValLen = p_cfg->TokenCfgPtr->ValLenMax;
        }

        if (p_buf_data != p_conn->TokenPtr) {
          size_buf_data = p_conn->TokenPtr - p_buf_data;
          p_conn->TokenBufRemLen -= (size_buf_data    + p_conn->TokenLen);
        } else {
          tx_buf = DEF_NO;
          p_conn->TokenBufRemLen -= p_conn->TokenLen;

          if (tx_token == DEF_NO) {                             // Check if token at beginning of sector is empty
            *p_err = HTTPs_ERR_NONE;
            goto exit;
          }
        }

        p_conn->DataTxdLen += p_conn->TokenCtrlPtr->ValLen;
        nbr_dig = HTTP_StrSizeHexDigReq(p_conn->TokenCtrlPtr->ValLen);
        p_token_val_start += HTTP_STR_BUF_TOP_SPACE_REQ_MIN - nbr_dig - STR_CR_LF_LEN;
        p_wr = p_token_val_start;
        p_conn->TokenCtrlPtr->TxPtr = p_wr;

        Str_FmtNbr_Int32U(p_conn->TokenCtrlPtr->ValLen, nbr_dig, DEF_NBR_BASE_HEX, DEF_NO, DEF_YES, DEF_NO, p_wr);
        p_wr += nbr_dig;
        Str_Copy_N(p_wr, STR_CR_LF, STR_CR_LF_LEN);

        p_token_val_end = p_token_val_data + p_conn->TokenCtrlPtr->ValLen;
        Str_Copy_N(p_token_val_end, STR_CR_LF, STR_CR_LF_LEN);

        p_conn->TokenCtrlPtr->TxLen = (p_token_val_end + STR_CR_LF_LEN) - p_token_val_start;
      }

      //                                                           ------------- PREPARE FILE DATA CHUNK --------------
      if (tx_buf == DEF_YES) {
        nbr_dig = HTTP_StrSizeHexDigReq(size_buf_data);
        p_buf_hdr_end = p_buf_data + size_buf_data;
        p_buf_hdr_start += HTTP_STR_BUF_TOP_SPACE_REQ_MIN - nbr_dig - STR_CR_LF_LEN;
        p_wr = p_buf_hdr_start;
        p_conn->TxBufPtr = p_wr;

        Str_FmtNbr_Int32U(size_buf_data, nbr_dig, DEF_NBR_BASE_HEX, DEF_NO, DEF_YES, DEF_NO, p_wr);
        p_wr += nbr_dig;
        Str_Copy_N(p_wr, STR_CR_LF, STR_CR_LF_LEN);

        Str_Copy_N(p_buf_hdr_end, STR_CR_LF, STR_CR_LF_LEN);

        p_conn->TxDataLen = (p_buf_hdr_end + STR_CR_LF_LEN) - p_buf_hdr_start;

        p_conn->DataTxdLen += size_buf_data;
      }

      //                                                           ---------------- UPDATE CONN STATES ----------------
      p_conn->SockState = HTTPs_SOCK_STATE_TX;

      if ((tx_buf == DEF_YES)                                   // Tx only file data chunk.
          && (tx_token == DEF_NO) ) {
        p_conn->State = HTTPs_CONN_STATE_RESP_DATA_CHUNCKED;
      } else if ((tx_buf == DEF_YES)                            // Tx file data chunk & token data chunk.
                 && (tx_token == DEF_YES)) {
        p_conn->State = HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_TX_TOKEN;
      } else if ((tx_buf == DEF_NO)                             // Tx only token data chunk.
                 && (tx_token == DEF_YES)) {
        p_conn->TxBufPtr = p_conn->TokenCtrlPtr->TxPtr;
        p_conn->TxDataLen = p_conn->TokenCtrlPtr->TxLen;
        p_conn->State = HTTPs_CONN_STATE_RESP_DATA_CHUNCKED;
      } else {                                                  // Tx last chunk.
        p_conn->State = HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_TX_LAST_CHUNK;
        p_wr = p_conn->TxBufPtr;
        Str_FmtNbr_Int32U(0, 1, DEF_NBR_BASE_HEX, DEF_NO, DEF_YES, DEF_NO, p_wr);
        p_wr++;
        Str_Copy_N(p_wr, STR_CR_LF, STR_CR_LF_LEN);

        p_wr += 2;
        Str_Copy_N(p_wr, STR_CR_LF, STR_CR_LF_LEN);
        p_conn->TxDataLen = 5;
      }
      break;

    //                                                             ---------- UPDATE CONN STATES TO TX TOKEN ----------
    case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_TX_TOKEN:
      p_conn->TxBufPtr = p_conn->TokenCtrlPtr->TxPtr;
      p_conn->TxDataLen = p_conn->TokenCtrlPtr->TxLen;
      p_conn->State = HTTPs_CONN_STATE_RESP_DATA_CHUNCKED;
      break;

    //                                                             ------- UPDATE CONN STATES TO COMPLETE RESP --------
    case HTTPs_CONN_STATE_RESP_DATA_CHUNCKED_TX_LAST_CHUNK:
      done = DEF_YES;
      break;

    default:                                                    // Inv state.
      HTTPs_ERR_INC(p_ctr_err->Resp_ErrTransferChunckedStateInvCtr);
      *p_err = HTTPs_ERR_STATE_UNKNOWN;
      goto exit;
  }

  *p_err = HTTPs_ERR_NONE;

exit:
  return (done);
}
#endif

/****************************************************************************************************//**
 *                                           HTTPsResp_TokenFinder()
 *
 * @brief    Search for token to be parsed or replaced in a sting buffer.
 *
 * @param    p_buf           Pointer to the buffer that contains the string to search within.
 *
 * @param    buf_len         Length of the buffer.
 *
 * @param    p_str_token     Pointer to pointer that will set to the beginning of the token found.
 *
 * @param    p_token_type    Pointer to variable that will receive the token type found.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Length of token found (including start char and end char of the token), if token found and validated.
 *           0, otherwise.
 *******************************************************************************************************/
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
static CPU_INT16U HTTPsResp_TokenFinder(CPU_CHAR         *p_buf,
                                        CPU_INT16U       buf_len,
                                        CPU_CHAR         **p_str_token,
                                        HTTPs_TOKEN_TYPE *p_token_type,
                                        HTTPs_ERR        *p_err)
{
  CPU_CHAR *p_srch;
  CPU_CHAR *p_token_start;
  CPU_CHAR *p_token_external_start;
  CPU_CHAR *p_token_internal_start;
  CPU_CHAR *p_token_end;
  CPU_INT16U len;
  CPU_BOOLEAN done;

  p_srch = p_buf;
  len = buf_len;
  done = DEF_NO;

  while (done != DEF_YES) {
    p_token_external_start = Str_Char_N(p_srch,                 // Srch External token start char in the buf
                                        len,
                                        HTTPs_TOKEN_EXTENAL_CHAR_START);

    p_token_internal_start = Str_Char_N(p_srch,                 // Srch Internal token start char in the buf
                                        len,
                                        HTTPs_TOKEN_INTERNAL_CHAR_START);

    //                                                             Determine which type of token is the first find.
    if ((p_token_external_start != DEF_NULL)
        && (p_token_internal_start != DEF_NULL)
        && (p_token_external_start < p_token_internal_start)) {
      *p_token_type = HTTPs_TOKEN_TYPE_EXTERNAL;
      p_token_start = p_token_external_start;
    } else if (p_token_external_start != DEF_NULL) {
      *p_token_type = HTTPs_TOKEN_TYPE_EXTERNAL;
      p_token_start = p_token_external_start;
    } else if (p_token_internal_start != DEF_NULL) {
      *p_token_type = HTTPs_TOKEN_TYPE_INTERNAL;
      p_token_start = p_token_internal_start;
    } else {                                                    // If token start char not found ...
      *p_token_type = HTTPs_TOKEN_TYPE_NONE;
      *p_err = HTTPs_ERR_TOKEN_NO_TOKEN_FOUND;                  // ... buf doesn't contains token.
      return (0u);                                              // ... not token validated.
    }

    len = len - (p_token_start - p_buf) - 1;                    // Calculate remaining space after start token char.
    if (len == 0) {                                             // If start token char is the last char of the buf ...
      *p_str_token = p_token_start;                             // ... set the token start pointer ...
      *p_err = HTTPs_ERR_TOKEN_MORE_DATA_REQ;                   // ... more data is required to process the token.
      return (0u);                                              // ... token can not be validated.
    }

    *p_str_token = p_token_start;
    p_srch = p_token_start + 1;                                 // Move to the next buf char.

    if (*p_srch == HTTPs_TOKEN_CHAR_VAR_SEP_START) {            // Is it a char token start var sep ...
                                                                // ... real token found ...
      p_token_end = Str_Char_N(p_srch,                          // ... srch the end of the token...
                               len,
                               HTTPs_TOKEN_CHAR_VAR_SEP_END);
      if (p_token_end == DEF_NULL) {                            // If end of token not found ...
        *p_err = HTTPs_ERR_TOKEN_MORE_DATA_REQ;                 // ... more data is required to process the token.
        return (0u);                                            // ... token can not be validated.
      }

      done = DEF_YES;                                           // Token found and validated, can return it.
      *p_token_end = ASCII_CHAR_NULL;                           // Create a string.
    }
  }

  len = (p_token_end - p_token_start) + 1;                      // Calculate the length of the token validated.

  *p_err = HTTPs_ERR_NONE;                                      // No err, token can be processed.

  return (len);
}
#endif

/****************************************************************************************************//**
 *                                           HTTPsResp_TokenValSet()
 *
 * @brief    Called for each token (${TEXT_STRING}) found in a HTML document.
 *
 * @param    p_instance      Pointer to the instance.
 *
 * @param    p_conn          Pointer to the connection.
 *
 * @param    p_token         Pointer to start of string containing the token
 *
 * @param    token_len       The Length of the token.
 *
 * @param    p_val           Pointer to buffer where to copy token value.
 *
 * @param    val_len_max     Value buffer length.
 *
 * @return   DEF_OK,   if token value copied successfully.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsResp_TokenValSet(HTTPs_INSTANCE *p_instance,
                                         HTTPs_CONN     *p_conn,
                                         const CPU_CHAR *p_token,
                                         CPU_INT16U     token_len,
                                         CPU_CHAR       *p_val,
                                         CPU_INT16U     val_len_max)
{
  CPU_INT32U token_key;
  HTTPs_TOKEN_INTERNAL token;
  HTTP_DICT *p_entry;
  HTTPs_INSTANCE_ERRS *p_ctr_err = DEF_NULL;

#if  (HTTPs_CFG_CTR_ERR_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_instance);
#endif
  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  token_key = HTTP_Dict_KeyGet(HTTPs_DictionaryTokenInternal,
                               HTTPs_DictionarySizeTokenInternal,
                               p_token,
                               DEF_YES,
                               token_len);
  if (token_key == HTTP_DICT_KEY_INVALID) {
    HTTPs_ERR_INC(p_ctr_err->Resp_ErrTokenInternalInvalidCtr);
    return (DEF_FAIL);
  }

  token = (HTTPs_TOKEN_INTERNAL)token_key;

  switch (token) {
    case HTTPs_TOKEN_INTERNAL_STATUS_CODE:
      p_entry = HTTP_Dict_EntryGet(HTTP_Dict_StatusCode,
                                   HTTP_Dict_StatusCodeSize,
                                   p_conn->StatusCode);
      if (p_entry != DEF_NULL) {
        Str_Copy_N(p_val, p_entry->StrPtr, val_len_max);
        return (DEF_OK);
      }
      HTTPs_ERR_INC(p_ctr_err->Resp_ErrTokenInternalStatusCodeInvalidCtr);
      break;

    case HTTPs_TOKEN_INTERNAL_REASON_PHRASE:
      p_entry = HTTP_Dict_EntryGet(HTTP_Dict_ReasonPhrase,
                                   HTTP_Dict_ReasonPhraseSize,
                                   p_conn->StatusCode);

      if (p_entry != DEF_NULL) {
        Str_Copy_N(p_val, p_entry->StrPtr, val_len_max);
        return (DEF_OK);
      }
      HTTPs_ERR_INC(p_ctr_err->Resp_ErrTokenInternalReasonPhraseInvalidCtr);
      break;

    default:
      HTTPs_ERR_INC(p_ctr_err->Resp_ErrTokenInternalInvalidCtr);
      break;
  }

  return (DEF_FAIL);
}
#endif

/****************************************************************************************************//**
 *                                       HTTPsResp_DfltErrPageSet()
 *
 * @brief    Configure the connection to transmit the static error page.
 *
 * @param    p_instance  Pointer to HTTP server instance object.
 *
 * @param    p_conn      Pointer to the connection.
 *******************************************************************************************************/
static void HTTPsResp_DfltErrPageSet(HTTPs_INSTANCE *p_instance,
                                     HTTPs_CONN     *p_conn)
{
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
  CPU_BOOLEAN chunk_en;
#else
  PP_UNUSED_PARAM(p_instance);
#endif

  p_conn->RespBodyDataType = HTTPs_BODY_DATA_TYPE_STATIC_DATA;
  p_conn->RespContentType = HTTP_CONTENT_TYPE_HTML;
  p_conn->DataLen = HTTPs_HTML_DLFT_ERR_LEN;
  p_conn->DataPtr = (void *)HTTPs_CFG_HTML_DFLT_ERR_PAGE;

  //                                                               Check if Chunked Transfer Encoding is needed.
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
  chunk_en = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED);
  if ((p_conn->ProtocolVer == HTTP_PROTOCOL_VER_1_1)            // If protocol ver is 1.1 ...
      && (p_cfg->TokenCfgPtr != DEF_NULL)                       // ..token parsing is enabled ..
      && (chunk_en == DEF_YES)              ) {
    DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED);
  } else {
    DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
  }
#else
  DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
#endif

  Str_Copy_N(p_conn->PathPtr,
             HTTPs_HTML_DLFT_ERR_STR_NAME,
             p_conn->PathLenMax);
}

/****************************************************************************************************//**
 *                                           HTTPsResp_FileOpen()
 *
 * @brief    Open file from the file system and the size.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *******************************************************************************************************/
#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsResp_FileOpen(HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN     *p_conn)
{
  const HTTPs_CFG *p_cfg;
  const NET_FS_API *p_fs_api;
  CPU_CHAR *p_file_path;
  CPU_BOOLEAN valid;
  HTTPs_INSTANCE_ERRS *p_ctr_errs = DEF_NULL;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
  CPU_BOOLEAN location_needed;
#endif

  HTTPs_SET_PTR_ERRS(p_ctr_errs, p_instance);
  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);

  p_cfg = p_instance->CfgPtr;

  switch (p_cfg->FS_Type) {
    case HTTPs_FS_TYPE_STATIC:
      p_fs_api = ((HTTPs_CFG_FS_STATIC *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      break;

    case HTTPs_FS_TYPE_DYN:
      p_fs_api = ((HTTPs_CFG_FS_DYN *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      break;

    case HTTPs_FS_TYPE_NONE:
    default:
      HTTPs_ERR_INC(p_ctr_errs->File_ErrOpenNoFS_Ctr);
      return (DEF_FAIL);
  }

  //                                                               Body is not a file: exit.
  if ((p_conn->RespBodyDataType == HTTPs_BODY_DATA_TYPE_STATIC_DATA)
      || (p_conn->RespBodyDataType == HTTPs_BODY_DATA_TYPE_NONE)       ) {
    return (DEF_OK);
  }

  if (p_fs_api == DEF_NULL) {
    HTTPs_ERR_INC(p_ctr_errs->File_ErrOpenNoFS_Ctr);
    return (DEF_FAIL);
  }

  //                                                               ------------------ SET FILE PATH -------------------
  //                                                               Translate the HTTP path into FS compatible path
  if (p_instance->FS_PathSepChar != HTTPs_PATH_SEP_CHAR_DFLT) {
    Str_Char_Replace_N(p_conn->PathPtr,
                       HTTPs_PATH_SEP_CHAR_DFLT,
                       p_instance->FS_PathSepChar,
                       p_conn->PathLenMax);
  }

  p_file_path = p_conn->PathPtr;

  while (*p_file_path == p_instance->FS_PathSepChar) {          // Skip the leading path separator.
    p_file_path++;
  }

#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
  p_file_path = HTTPs_StrPathGet(p_file_path,
                                 p_conn->PathLenMax - (p_file_path - p_conn->PathPtr),
                                 p_conn->HostPtr,
                                 p_cfg->HostNameLenMax,
                                 &location_needed);
  if (location_needed == DEF_YES) {
    DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_RESP_LOCATION);
  } else {
    DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_LOCATION);
  }
#endif

  //                                                               -------------------- OPEN FILE ---------------------
  p_conn->DataPtr = p_fs_api->Open(p_file_path,
                                   NET_FS_FILE_MODE_OPEN,
                                   NET_FS_FILE_ACCESS_RD);
  if (p_conn->DataPtr != DEF_NULL) {                            // If file opened successfully...
    HTTPs_STATS_INC(p_ctr_stats->FS_StatOpenedCtr);

    //                                                             ------------ GET FILE SIZE FOR THE RESP ------------
    valid = p_fs_api->GetSize(p_conn->DataPtr, &p_conn->DataLen);
    if (valid != DEF_YES) {                                     // If unable to get a valid size...
      HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);
      HTTPs_STATS_INC(p_ctr_stats->FS_StatClosedCtr);
      p_fs_api->Close(p_conn->DataPtr);                         // ... Close the file ...
      p_conn->DataPtr = DEF_NULL;

      return (DEF_FAIL);
    }
  } else {                                                      // If file not opened successfully.
    return (DEF_FAIL);
  }

  return (DEF_OK);
}
#endif

/****************************************************************************************************//**
 *                                           HTTPsResp_DataRd()
 *
 * @brief    Read file data.
 *
 * @param    p_instance      Pointer to the instance.
 *
 * @param    p_conn          Pointer to the connection.
 *
 * @param    p_dst           Pointer to buffer where to store read data.
 *
 * @param    dst_len_max     Maximum data that can be put in the destination.
 *
 * @return   Length of data read.
 *******************************************************************************************************/
static CPU_SIZE_T HTTPsResp_DataRd(HTTPs_INSTANCE *p_instance,
                                   HTTPs_CONN     *p_conn,
                                   CPU_CHAR       *p_dst,
                                   CPU_SIZE_T     dst_len_max)
{
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
  const NET_FS_API *p_fs_api;
  HTTPs_INSTANCE_ERRS *p_ctr_errs = DEF_NULL;
  CPU_CHAR *p_src;
  CPU_SIZE_T size;

  HTTPs_SET_PTR_ERRS(p_ctr_errs, p_instance);

  switch (p_cfg->FS_Type) {
    case HTTPs_FS_TYPE_STATIC:
      p_fs_api = ((HTTPs_CFG_FS_STATIC *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      break;

    case HTTPs_FS_TYPE_DYN:
      p_fs_api = ((HTTPs_CFG_FS_DYN *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      break;

    case HTTPs_FS_TYPE_NONE:
      p_fs_api = DEF_NULL;
      break;

    default:
      HTTPs_ERR_INC(p_ctr_errs->FS_ErrTypeInvalidCtr);
      return (0);
  }

  //                                                               ------------------- RD FILE DATA -------------------
  switch (p_conn->RespBodyDataType) {
    case HTTPs_BODY_DATA_TYPE_STATIC_DATA:                      // File type is fixed, internal err page.
      p_src = (CPU_CHAR *)p_conn->DataPtr + p_conn->DataFixPosCur;
      size = p_conn->DataLen - p_conn->DataFixPosCur;
      size = DEF_MIN(size, dst_len_max);
      Mem_Copy(p_dst, p_src, size);                             // Copy file data to the buf.
      p_conn->DataFixPosCur += size;
      break;

    case HTTPs_BODY_DATA_TYPE_NONE:
      size = 0;
      break;

    case HTTPs_BODY_DATA_TYPE_FILE:                             // File type is from FS.
      if (p_fs_api != DEF_NULL) {
        (void)p_fs_api->Rd(p_conn->DataPtr,                     // Read file from the FS.
                           p_dst,
                           dst_len_max,
                           &size);
      } else {
        HTTPs_ERR_INC(p_ctr_errs->File_ErrRdNoFS_Ctr);
        size = 0;
      }
      break;

    default:
      size = 0;
      break;
  }

  return (size);
}

/****************************************************************************************************//**
 *                                           HTTPsResp_DataSetPos()
 *
 * @brief    Set file position indicator.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    offset      Offset from the file position specified by 'origin'.
 *
 * @param    origin      Reference position for offset:
 *                       NET_FS_SEEK_ORIGIN_START    Offset is from the beginning of the file.
 *                       NET_FS_SEEK_ORIGIN_CUR      Offset is from current file position.
 *                       NET_FS_SEEK_ORIGIN_END      Offset is from the end       of the file.
 *******************************************************************************************************/
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
static void HTTPsResp_DataSetPos(HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN     *p_conn,
                                 CPU_INT32S     offset,
                                 CPU_INT08U     origin)
{
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
  const NET_FS_API *p_fs_api;
  HTTPs_INSTANCE_ERRS *p_ctr_errs = DEF_NULL;

  HTTPs_SET_PTR_ERRS(p_ctr_errs, p_instance);

  switch (p_cfg->FS_Type) {
    case HTTPs_FS_TYPE_STATIC:
      p_fs_api = ((HTTPs_CFG_FS_STATIC *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      break;

    case HTTPs_FS_TYPE_DYN:
      p_fs_api = ((HTTPs_CFG_FS_DYN *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      break;

    case HTTPs_FS_TYPE_NONE:
      p_fs_api = DEF_NULL;
      break;

    default:
      HTTPs_ERR_INC(p_ctr_errs->FS_ErrTypeInvalidCtr);
      return;
  }

  switch (p_conn->RespBodyDataType) {
    case HTTPs_BODY_DATA_TYPE_STATIC_DATA:                      // File type is fixed (err page).
      switch (origin) {
        case NET_FS_SEEK_ORIGIN_START:
          if (offset >= 0) {
            p_conn->DataFixPosCur = offset;
          }
          break;

        case NET_FS_SEEK_ORIGIN_CUR:
          p_conn->DataFixPosCur += offset;
          break;

        case NET_FS_SEEK_ORIGIN_END:
          if (offset <= 0) {
            p_conn->DataFixPosCur = p_conn->DataLen + offset;
          } else {
            p_conn->DataFixPosCur = p_conn->DataLen;
          }
          break;

        default:
          break;
      }
      break;

    case HTTPs_BODY_DATA_TYPE_FILE:                               // File type is from FS.
      if (p_fs_api != DEF_NULL) {
        (void)p_fs_api->SetPos(p_conn->DataPtr,
                               offset,
                               origin);
      } else {
        HTTPs_ERR_INC(p_ctr_errs->File_ErrSetPosNoFS_Ctr);
      }
      break;

    case HTTPs_BODY_DATA_TYPE_NONE:
    default:
      break;
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
