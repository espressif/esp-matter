/***************************************************************************//**
 * @file
 * @brief Network - HTTP Client Response Module
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

#include  <net/include/http_client.h>
#include  <net/include/http.h>

#ifdef  HTTPc_WEBSOCK_MODULE_EN
#include  "http_client_websock_priv.h"
#endif

#include  <cpu/include/cpu.h>
#include  <common/include/lib_str.h>
#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void HTTPcResp_ParseStatusLine(HTTPc_CONN   *p_conn,
                                      HTTPc_STATUS *p_status,
                                      RTOS_ERR     *p_err);

static HTTP_PROTOCOL_VER HTTPcResp_ParseProtocolVer(const CPU_CHAR *p_resp_line,
                                                    CPU_INT32U     str_len);

static HTTP_STATUS_CODE HTTPcResp_ParseStatusCode(const CPU_CHAR *p_resp_line,
                                                  CPU_INT32U     str_len);

static const CPU_CHAR *HTTPcResp_ParseReasonPhrase(const CPU_CHAR   *p_resp_line,
                                                   CPU_INT32U       str_len,
                                                   HTTP_STATUS_CODE status_code);

static void HTTPcResp_ParseHdr(HTTPc_CONN   *p_conn,
                               HTTPc_STATUS *p_status,
                               RTOS_ERR     *p_err);

static void HTTPcResp_Body(HTTPc_CONN   *p_conn,
                           HTTPc_STATUS *p_status,
                           RTOS_ERR     *p_err);

static void HTTPcResp_BodyStd(HTTPc_CONN   *p_conn,
                              HTTPc_STATUS *p_status,
                              RTOS_ERR     *p_err);

static void HTTPcResp_BodyChunk(HTTPc_CONN   *p_conn,
                                HTTPc_STATUS *p_status,
                                RTOS_ERR     *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                             HTTPcResp()
 *
 * @brief    Main HTTP response processing function.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if response processing is complete successfully.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPcResp(HTTPc_CONN *p_conn,
                      RTOS_ERR   *p_err)
{
  CPU_BOOLEAN  rx_data;
  CPU_BOOLEAN  process;
  CPU_BOOLEAN  sock_rdy_rd;
  CPU_BOOLEAN  sock_rdy_err;
  CPU_BOOLEAN  done;
  HTTPc_STATUS rtn_status = HTTPc_STATUS_NONE;

  done = DEF_NO;
  process = DEF_NO;
  rx_data = DEF_BIT_IS_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_RX_MORE_DATA);

  if (rx_data == DEF_YES) {
    sock_rdy_rd = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPc_FLAG_SOCK_RDY_RD);
    sock_rdy_err = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPc_FLAG_SOCK_RDY_ERR);

    if ((sock_rdy_rd == DEF_YES)
        || (sock_rdy_err == DEF_YES)) {
      process = HTTPcSock_ConnDataRx(p_conn, DEF_NULL, p_err);
      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_NONE:
        case RTOS_ERR_WOULD_BLOCK:
        case RTOS_ERR_TIMEOUT:
          break;

        default:
          goto exit;
      }
    }
  }

  switch (p_conn->State) {
    case HTTPc_CONN_STATE_RESP_INIT:
      DEF_BIT_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_RX_MORE_DATA);
      if (process == DEF_YES) {
        DEF_BIT_CLR(p_conn->RespFlags, (HTTPc_FLAGS)HTTPc_FLAG_RESP_RX_MORE_DATA);
        p_conn->State = HTTPc_CONN_STATE_RESP_STATUS_LINE;
      }
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case HTTPc_CONN_STATE_RESP_STATUS_LINE:
      HTTPcResp_ParseStatusLine(p_conn, &rtn_status, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        break;
      }
      switch (rtn_status) {
        case HTTPc_STATUS_NONE:
          DEF_BIT_CLR(p_conn->RespFlags, (HTTPc_FLAGS)HTTPc_FLAG_RESP_RX_MORE_DATA);
          p_conn->State = HTTPc_CONN_STATE_RESP_HDR;
          break;

        case HTTPc_STATUS_NEED_MORE_DATA:
          DEF_BIT_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_RX_MORE_DATA);
          break;

        default:
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_YES);
      }
      break;

    case HTTPc_CONN_STATE_RESP_HDR:
      HTTPcResp_ParseHdr(p_conn, &rtn_status, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        break;
      }
      switch (rtn_status) {
        case HTTPc_STATUS_NONE:
          DEF_BIT_CLR(p_conn->RespFlags, (HTTPc_FLAGS)HTTPc_FLAG_RESP_RX_MORE_DATA);
          p_conn->State = HTTPc_CONN_STATE_RESP_BODY;
          break;

        case HTTPc_STATUS_NEED_MORE_DATA:
          DEF_BIT_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_RX_MORE_DATA);
          break;

        default:
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_YES);
      }
      break;

    case HTTPc_CONN_STATE_RESP_BODY:
    case HTTPc_CONN_STATE_RESP_BODY_CHUNK_SIZE:
    case HTTPc_CONN_STATE_RESP_BODY_CHUNK_DATA:
    case HTTPc_CONN_STATE_RESP_BODY_CHUNK_CRLF:
    case HTTPc_CONN_STATE_RESP_BODY_CHUNK_LAST:
      HTTPcResp_Body(p_conn, &rtn_status, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        break;
      }
      switch (rtn_status) {
        case HTTPc_STATUS_NONE:
          DEF_BIT_CLR(p_conn->RespFlags, (HTTPc_FLAGS)HTTPc_FLAG_RESP_RX_MORE_DATA);
          p_conn->State = HTTPc_CONN_STATE_RESP_COMPLETED;
          break;

        case HTTPc_STATUS_NEED_MORE_DATA:
          DEF_BIT_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_RX_MORE_DATA);
          break;

        default:
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_YES);
      }
      break;

    case HTTPc_CONN_STATE_RESP_COMPLETED:
      done = DEF_YES;
      p_conn->State = HTTPc_CONN_STATE_COMPLETED;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE, DEF_NO);
  }

exit:
  return (done);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                      HTTPcResp_ParseStatusLine()
 *
 * @brief    (1) HTTP Response status line processing :
 *               - (a) Parse HTTP protocol version received.
 *               - (b) Parse Status Code received.
 *               - (c) Parse Reason phrase received.
 *
 * @param    p_conn      Pointer to current HTTPc Connection.
 *
 * @param    p_status    Pointer to variable that will received the transaction status.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void HTTPcResp_ParseStatusLine(HTTPc_CONN   *p_conn,
                                      HTTPc_STATUS *p_status,
                                      RTOS_ERR     *p_err)
{
  HTTPc_REQ  *p_req;
  HTTPc_RESP *p_resp;
  CPU_CHAR   *p_resp_line_start;
  CPU_CHAR   *p_resp_line_end;
  CPU_SIZE_T len;

  p_req = p_conn->ReqListHeadPtr;
  p_resp = p_req->RespPtr;

  len = p_conn->RxDataLenRem;

  if (len <= sizeof(HTTP_STR_METHOD_GET)) {
    *p_status = HTTPc_STATUS_NEED_MORE_DATA;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  //                                                               ------------ RETRIEVE RESP STATUS LINE -------------
  //                                                               Search beginning and remove white spaces before Resp.
  p_resp_line_start = HTTP_StrGraphSrchFirst(p_conn->RxBufPtr, len);
  if (p_resp_line_start == DEF_NULL) {
    *p_status = HTTPc_STATUS_NEED_MORE_DATA;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return;
  }

  //                                                               Find end of resp status line.
  len -= (p_resp_line_start - p_conn->BufPtr);
  p_resp_line_end = Str_Str_N(p_resp_line_start, STR_CR_LF, len);
  if (p_resp_line_end == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    return;
  }

  len = p_resp_line_end - p_resp_line_start;
  //                                                               ---------- PARSE RESPONSE PROTOCOL VERSION ---------
  p_resp->ProtocolVer = HTTPcResp_ParseProtocolVer(p_resp_line_start, len);
  switch (p_resp->ProtocolVer) {
    case HTTP_PROTOCOL_VER_1_1:
      break;

    case HTTP_PROTOCOL_VER_1_0:
    case HTTP_PROTOCOL_VER_0_9:
    case HTTP_PROTOCOL_VER_UNKNOWN:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      return;
  }

  //                                                               ------------ FIND SPACE IN STATUS LINE -------------
  p_resp_line_start = Str_Char_N(p_resp_line_start, len, ASCII_CHAR_SPACE);
  if (p_resp_line_start == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    return;
  }
  p_resp_line_start++;
  len = p_resp_line_end - p_resp_line_start;
  //                                                               ------------ PARSE RESPONSE STATUS CODE ------------
  p_resp->StatusCode = HTTPcResp_ParseStatusCode(p_resp_line_start, len);
  switch (p_resp->StatusCode) {
    case  HTTP_STATUS_OK:
    case  HTTP_STATUS_CREATED:
    case  HTTP_STATUS_ACCEPTED:
    case  HTTP_STATUS_NO_CONTENT:
    case  HTTP_STATUS_RESET_CONTENT:
    case  HTTP_STATUS_MOVED_PERMANENTLY:
    case  HTTP_STATUS_FOUND:
    case  HTTP_STATUS_SEE_OTHER:
    case  HTTP_STATUS_NOT_MODIFIED:
    case  HTTP_STATUS_USE_PROXY:
    case  HTTP_STATUS_TEMPORARY_REDIRECT:
    case  HTTP_STATUS_BAD_REQUEST:
    case  HTTP_STATUS_UNAUTHORIZED:
    case  HTTP_STATUS_FORBIDDEN:
    case  HTTP_STATUS_NOT_FOUND:
    case  HTTP_STATUS_METHOD_NOT_ALLOWED:
    case  HTTP_STATUS_NOT_ACCEPTABLE:
    case  HTTP_STATUS_REQUEST_TIMEOUT:
    case  HTTP_STATUS_CONFLICT:
    case  HTTP_STATUS_GONE:
    case  HTTP_STATUS_LENGTH_REQUIRED:
    case  HTTP_STATUS_PRECONDITION_FAILED:
    case  HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE:
    case  HTTP_STATUS_REQUEST_URI_TOO_LONG:
    case  HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE:
    case  HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE:
    case  HTTP_STATUS_EXPECTATION_FAILED:
    case  HTTP_STATUS_INTERNAL_SERVER_ERR:
    case  HTTP_STATUS_NOT_IMPLEMENTED:
    case  HTTP_STATUS_SERVICE_UNAVAILABLE:
    case  HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED:
    case  HTTP_STATUS_SWITCHING_PROTOCOLS:
      break;

    case  HTTP_STATUS_UNKOWN:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      return;
  }

  //                                                               ------------ FIND SPACE IN STATUS LINE -------------
  p_resp_line_start = Str_Char_N(p_resp_line_start, len, ASCII_CHAR_SPACE);
  if (p_resp_line_start == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    return;
  }
  p_resp_line_start++;
  len = p_resp_line_end - p_resp_line_start;

  //                                                               ----------- PARSE RESPONSE REASON PHRASE -----------
  p_resp->ReasonPhrasePtr = HTTPcResp_ParseReasonPhrase(p_resp_line_start, len, p_resp->StatusCode);
  if (p_resp->ReasonPhrasePtr == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    return;
  }

  //                                                               -------------- UPDATE CONN RX PARAMS ---------------
  p_resp_line_end += STR_CR_LF_LEN;
  p_conn->RxDataLenRem -= (p_resp_line_end - p_conn->RxBufPtr);
  p_conn->RxBufPtr = p_resp_line_end;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                     HTTPcResp_ParseProtocolVer()
 *
 * @brief    Find HTTP Protocol version in the HTTP response received.
 *
 * @param    p_resp_line     Pointer to Response status line received.
 *
 * @param    str_len         Length of the response status line.
 *
 * @return   HTTP protocol version.
 *******************************************************************************************************/
static HTTP_PROTOCOL_VER HTTPcResp_ParseProtocolVer(const CPU_CHAR *p_resp_line,
                                                    CPU_INT32U     str_len)
{
  HTTP_PROTOCOL_VER ver;

  ver = (HTTP_PROTOCOL_VER)HTTP_Dict_KeyGet(HTTP_Dict_ProtocolVer,
                                            HTTP_Dict_ProtocolVerSize,
                                            p_resp_line,
                                            DEF_YES,
                                            str_len);
  switch (ver) {
    case HTTP_PROTOCOL_VER_0_9:
    case HTTP_PROTOCOL_VER_1_0:
    case HTTP_PROTOCOL_VER_1_1:
      break;

    default:
      return (HTTP_PROTOCOL_VER_UNKNOWN);
  }

  return ((HTTP_PROTOCOL_VER)ver);
}

/****************************************************************************************************//**
 *                                      HTTPcResp_ParseStatusCode()
 *
 * @brief    Find HTTP Status Code in the HTTP Response received.
 *
 * @param    p_resp_line     Pointer to the HTTP Response status line received.
 *
 * @param    str_len         Length of the Response status line.
 *
 * @return   HTTP status code.
 *******************************************************************************************************/
static HTTP_STATUS_CODE HTTPcResp_ParseStatusCode(const CPU_CHAR *p_resp_line,
                                                  CPU_INT32U     str_len)
{
  HTTP_STATUS_CODE status_code;

  status_code = (HTTP_STATUS_CODE)HTTP_Dict_KeyGet(HTTP_Dict_StatusCode,
                                                   HTTP_Dict_StatusCodeSize,
                                                   p_resp_line,
                                                   DEF_YES,
                                                   str_len);

  if (status_code >= HTTP_STATUS_UNKOWN) {
    return (HTTP_STATUS_UNKOWN);
  }

  return (status_code);
}

/****************************************************************************************************//**
 *                                     HTTPcResp_ParseReasonPhrase()
 *
 * @brief    Find HTTP Reason Phrase in the HTTP Response received.
 *
 * @param    p_resp_line     Pointer to the HTTP Response status line received.
 *
 * @param    str_len         Length of the Response status line.
 *
 * @param    status_code     Status code number received.
 *
 * @return   Pointer to the Reason phrase associated with the status code saved in the HTTP libraries.
 *******************************************************************************************************/
static const CPU_CHAR *HTTPcResp_ParseReasonPhrase(const CPU_CHAR   *p_resp_line,
                                                   CPU_INT32U       str_len,
                                                   HTTP_STATUS_CODE status_code)
{
  HTTP_DICT *p_entry;

  PP_UNUSED_PARAM(p_resp_line);
  PP_UNUSED_PARAM(str_len);

  p_entry = HTTP_Dict_EntryGet(HTTP_Dict_ReasonPhrase,
                               HTTP_Dict_ReasonPhraseSize,
                               (CPU_INT32U)status_code);

  return (p_entry->StrPtr);
}

/****************************************************************************************************//**
 *                                         HTTPcResp_ParseHdr()
 *
 * @brief    Parse all the headers received in the HTTP response.
 *
 * @param    p_conn      Pointer to the current HTTPc Connection.
 *
 * @param    p_status    Pointer to variable that will received the transaction status.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void HTTPcResp_ParseHdr(HTTPc_CONN   *p_conn,
                               HTTPc_STATUS *p_status,
                               RTOS_ERR     *p_err)
{
  HTTPc_REQ                    *p_req;
  HTTPc_RESP                   *p_resp;
  CPU_CHAR                     *p_field_start;
  CPU_CHAR                     *p_field_end;
  CPU_CHAR                     *p_val;
  HTTP_HDR_FIELD               hdr_field;
  HTTP_HDR_FIELD_CONN_VAL      hdr_con_val;
  HTTP_CONTENT_TYPE            content_type;
  CPU_INT32U                   content_len;
  HTTP_HDR_FIELD_TRANSFER_TYPE transfer_type;
  CPU_INT16U                   len;
#if (HTTPc_CFG_HDR_RX_EN == DEF_ENABLED)
  HTTPc_CONN_OBJ *p_conn_const;
  HTTPc_REQ_OBJ  *p_req_const;
  HTTP_DICT      *p_entry;
#endif
#ifdef  HTTPc_WEBSOCK_MODULE_EN
  HTTPc_WEBSOCK_REQ *p_ws_req;
  CPU_BOOLEAN       is_matched;
  CPU_INT32U        version;
#endif

  p_req = p_conn->ReqListHeadPtr;
  p_resp = p_req->RespPtr;

  p_field_start = p_conn->RxBufPtr;
#ifdef  HTTPc_WEBSOCK_MODULE_EN
  p_ws_req = p_req->WebSockPtr;
#endif
  while (DEF_TRUE) {
    p_field_end = Str_Str_N(p_field_start,                      // Find end of header field.
                            STR_CR_LF,
                            p_conn->RxDataLenRem);

    if ((p_field_end != DEF_NULL)                               // If the field and value are present.
        && (p_field_end > p_field_start)) {
      len = p_field_end - p_field_start;

      hdr_field = (HTTP_HDR_FIELD)HTTP_Dict_KeyGet(HTTP_Dict_HdrField,
                                                   HTTP_Dict_HdrFieldSize,
                                                   p_field_start,
                                                   DEF_YES,
                                                   len);

      switch (hdr_field) {
        case HTTP_HDR_FIELD_CONN:
          //                                                       Get field val beginning.
          p_val = HTTP_HdrParseFieldValueGet(p_field_start,
                                             HTTP_STR_HDR_FIELD_CONN_LEN,
                                             p_field_end,
                                             &len);
          if (p_val == DEF_NULL) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          }

          len = p_field_end - p_val;

          hdr_con_val = (HTTP_HDR_FIELD_CONN_VAL)HTTP_Dict_KeyGet(HTTP_Dict_HdrFieldConnVal,
                                                                  HTTP_Dict_HdrFieldConnValSize,
                                                                  p_val,
                                                                  DEF_NO,
                                                                  len);
          switch (hdr_con_val) {
            case HTTP_HDR_FIELD_CONN_CLOSE:
              DEF_BIT_SET(p_conn->Flags, HTTPc_FLAG_CONN_TO_CLOSE);
              p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_NO_PERSISTENT;
              break;

            case HTTP_HDR_FIELD_CONN_PERSISTENT:
              break;
#ifdef  HTTPc_WEBSOCK_MODULE_EN
            case HTTP_HDR_FIELD_CONN_UPGRADE:
              DEF_BIT_SET(p_ws_req->Flags, HTTPc_FLAG_WEBSOCK_REQ_CONN_UPGRADE);
              break;
#endif
            case HTTP_HDR_FIELD_CONN_UNKNOWN:
            default:
              RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
              goto exit;
          }
          break;

        case HTTP_HDR_FIELD_CONTENT_TYPE:
          //                                                       Get field val beginning.
          p_val = HTTP_HdrParseFieldValueGet(p_field_start,
                                             HTTP_STR_HDR_FIELD_CONTENT_TYPE_LEN,
                                             p_field_end,
                                             &len);
          if (p_val == DEF_NULL) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          }

          len = p_field_end - p_val;

          content_type = (HTTP_CONTENT_TYPE)HTTP_Dict_KeyGet(HTTP_Dict_ContentType,
                                                             HTTP_Dict_ContentTypeSize,
                                                             p_val,
                                                             DEF_NO,
                                                             len);
          if (content_type == HTTP_CONTENT_TYPE_UNKNOWN) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          }

          p_resp->ContentType = content_type;
          break;

        case HTTP_HDR_FIELD_CONTENT_LEN:
          //                                                       Get field val beginning.
          p_val = HTTP_HdrParseFieldValueGet(p_field_start,
                                             HTTP_STR_HDR_FIELD_CONTENT_LEN_LEN,
                                             p_field_end,
                                             &len);
          if (p_val == DEF_NULL) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          }

          content_len = Str_ParseNbr_Int32U(p_val, 0, DEF_NBR_BASE_DEC);

          if (content_len <= 0u) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          }

          p_resp->ContentLen = content_len;
          break;

        case HTTP_HDR_FIELD_TRANSFER_ENCODING:
          //                                                       Get field val beginning.
          p_val = HTTP_HdrParseFieldValueGet(p_field_start,
                                             HTTP_STR_HDR_FIELD_TRANSFER_ENCODING_LEN,
                                             p_field_end,
                                             &len);
          if (p_val == DEF_NULL) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          }

          len = p_field_end - p_val;

          transfer_type = (HTTP_HDR_FIELD_TRANSFER_TYPE)HTTP_Dict_KeyGet(HTTP_Dict_HdrFieldTransferTypeVal,
                                                                         HTTP_Dict_HdrFieldTransferTypeValSize,
                                                                         p_val,
                                                                         DEF_NO,
                                                                         len);
          if (transfer_type != HTTP_HDR_FIELD_TRANSFER_TYPE_CHUNCKED) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          }
          DEF_BIT_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_BODY_CHUNK_TRANSFER);
          break;

#ifdef  HTTPc_WEBSOCK_MODULE_EN
        case HTTP_HDR_FIELD_UPGRADE:
          //                                                       Get field val beginning.
          p_val = HTTP_HdrParseFieldValueGet(p_field_start,
                                             HTTP_STR_HDR_FIELD_UPGRADE_LEN,
                                             p_field_end,
                                             &len);
          if (p_val == DEF_NULL) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          }

          len = p_field_end - p_val;

          hdr_con_val = (HTTP_HDR_FIELD_CONN_VAL)HTTP_Dict_KeyGet(HTTP_Dict_HdrFieldUpgradeVal,
                                                                  HTTP_Dict_HdrFieldUpgradeValSize,
                                                                  p_val,
                                                                  DEF_NO,
                                                                  len);
          switch (hdr_con_val) {
            case HTTP_HDR_FIELD_UPGRADE_WEBSOCKET:
              DEF_BIT_SET(p_ws_req->Flags, HTTPc_FLAG_WEBSOCK_REQ_UPGRADE_WEBSOCKET);
              break;

            default:
              RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
              goto exit;
          }
          break;

        case HTTP_HDR_FIELD_WEBSOCKET_ACCEPT:
          //                                                       Get field val beginning.
          p_val = HTTP_HdrParseFieldValueGet(p_field_start,
                                             HTTP_STR_HDR_FIELD_WEBSOCKET_ACCEPT_LEN,
                                             p_field_end,
                                             &len);
          if (p_val == DEF_NULL) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          }

          len = p_field_end - p_val;
          //                                                       Validate the websocket field.

          is_matched = Mem_Cmp(p_ws_req->Accept, p_val, HTTPc_WEBSOCK_KEY_HASH_ENCODED_LEN - 1);

          if (is_matched == DEF_YES) {
            DEF_BIT_SET(p_ws_req->Flags, HTTPc_FLAG_WEBSOCK_REQ_ACCEPT_VALIDATED);
          }
          break;

        case HTTP_HDR_FIELD_WEBSOCKET_VERSION:
          //                                                       Get field val beginning.
          p_val = HTTP_HdrParseFieldValueGet(p_field_start,
                                             HTTP_STR_HDR_FIELD_WEBSOCKET_VERSION_LEN,
                                             p_field_end,
                                             &len);
          if (p_val == DEF_NULL) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          }
          version = Str_ParseNbr_Int32U(p_val, DEF_NULL, DEF_NBR_BASE_DEC);

          if (version == HTTPc_WEBSOCK_PROTOCOL_VERSION_13) {
            DEF_BIT_SET(p_ws_req->Flags, HTTPc_FLAG_WEBSOCK_REQ_VERSION_VALIDATED);
          }
          break;
#endif

        default:
#if (HTTPc_CFG_HDR_RX_EN == DEF_ENABLED)
          p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
          p_req_const = (HTTPc_REQ_OBJ *)p_req;

          p_entry = HTTP_Dict_EntryGet(HTTP_Dict_HdrField,
                                       HTTP_Dict_HdrFieldSize,
                                       hdr_field);
          if (p_entry != DEF_NULL) {
            //                                                     Get field val beginning.
            p_val = HTTP_HdrParseFieldValueGet(p_field_start,
                                               p_entry->StrLen,
                                               p_field_end,
                                               &len);
            if (p_val == DEF_NULL) {
              RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
              goto exit;
            }

            len = p_field_end - p_val;

            if (p_req->OnHdrRx != DEF_NULL) {
              p_req->OnHdrRx(p_conn_const, p_req_const, hdr_field, p_val, len);
            }
          }
#endif
          break;
      }

      //                                                           --------------- UPDATE RX CONN PARAM ---------------
      p_field_end += STR_CR_LF_LEN;
      p_conn->RxDataLenRem -= p_field_end - p_field_start;
      p_field_start = p_field_end;
      p_conn->RxBufPtr = p_field_start;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    } else if (p_field_end == p_field_start) {                  // All field processed.
      p_conn->RxBufPtr += STR_CR_LF_LEN;
      p_conn->RxDataLenRem -= STR_CR_LF_LEN;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit;
    } else {                                                    // More data req'd to complete processing.
      *p_status = HTTPc_STATUS_NEED_MORE_DATA;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit;
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           HTTPcResp_Body()
 *
 * @brief    Process Response body received.
 *
 * @param    p_conn      Pointer to current HTTPc Connection.
 *
 * @param    p_status    Pointer to variable that will received the transaction status.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void HTTPcResp_Body(HTTPc_CONN   *p_conn,
                           HTTPc_STATUS *p_status,
                           RTOS_ERR     *p_err)
{
  CPU_BOOLEAN chunk_transfer;

  chunk_transfer = DEF_BIT_IS_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_BODY_CHUNK_TRANSFER);

  if (chunk_transfer == DEF_YES) {
    HTTPcResp_BodyChunk(p_conn, p_status, p_err);
  } else {
    HTTPcResp_BodyStd(p_conn, p_status, p_err);
  }
}

/****************************************************************************************************//**
 *                                          HTTPcResp_BodyStd()
 *
 * @brief    Process HTTP response body received with the Content-Length header.
 *
 * @param    p_conn      Pointer to current HTTPc Connection.
 *
 * @param    p_status    Pointer to variable that will received the transaction status.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void HTTPcResp_BodyStd(HTTPc_CONN   *p_conn,
                              HTTPc_STATUS *p_status,
                              RTOS_ERR     *p_err)
{
  HTTPc_CONN_OBJ *p_conn_const;
  HTTPc_REQ      *p_req;
  HTTPc_REQ_OBJ  *p_req_const;
  HTTPc_RESP     *p_resp;
  CPU_CHAR       *p_buf;
  CPU_INT32U     content_len;
  CPU_INT32U     content_len_rem;
  CPU_INT32U     content_len_chunk;
  CPU_INT32U     data_len_read;
  CPU_BOOLEAN    last_chunk;

  p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
  p_req = p_conn->ReqListHeadPtr;
  p_req_const = (HTTPc_REQ_OBJ *)p_req;
  p_resp = p_req->RespPtr;
  content_len = p_resp->ContentLen;
  p_buf = p_conn->RxBufPtr;
  content_len_rem = content_len - p_conn->RxDataLen;

  if (content_len_rem >= p_conn->RxDataLenRem) {
    content_len_chunk = p_conn->RxDataLenRem;
  } else {
    content_len_chunk = content_len_rem;
  }
  p_conn->RxDataLen += content_len_chunk;

  last_chunk = DEF_NO;
  if (p_conn->RxDataLen == content_len) {
    last_chunk = DEF_YES;
  } else {
    *p_status = HTTPc_STATUS_NEED_MORE_DATA;
  }

  if (p_req->OnBodyRx != DEF_NULL) {
    data_len_read = p_req->OnBodyRx(p_conn_const,
                                    p_req_const,
                                    p_resp->ContentType,
                                    p_buf,
                                    content_len_chunk,
                                    last_chunk);
    p_conn->RxDataLen -= content_len_chunk - data_len_read;
    p_conn->RxBufPtr += data_len_read;
    p_conn->RxDataLenRem -= data_len_read;
  } else {
    p_conn->RxBufPtr += content_len_chunk;
    p_conn->RxDataLenRem -= content_len_chunk;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                         HTTPcResp_BodyChunk()
 *
 * @brief    Process Response body received with the chunked transfer encoding.
 *
 * @param    p_conn      Pointer to current HTTPc Connection.
 *
 * @param    p_status    Pointer to variable that will received the transaction status.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Chunk Extensions located after chunk size are not supported.
 *******************************************************************************************************/
static void HTTPcResp_BodyChunk(HTTPc_CONN   *p_conn,
                                HTTPc_STATUS *p_status,
                                RTOS_ERR     *p_err)
{
  HTTPc_CONN_OBJ *p_conn_const;
  HTTPc_REQ      *p_req;
  HTTPc_REQ_OBJ  *p_req_const;
  HTTPc_RESP     *p_resp;
  CPU_CHAR       *p_size_start;
  CPU_CHAR       *p_size_end;
  CPU_CHAR       *p_chunk_start;
  CPU_CHAR       *p_str;
  CPU_INT32U     chunk_len;
  CPU_INT32U     buf_len;
  CPU_INT32U     data_len;
  CPU_INT32U     data_len_read = 0;

  p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
  p_req = p_conn->ReqListHeadPtr;
  p_req_const = (HTTPc_REQ_OBJ *)p_req;
  p_resp = p_req->RespPtr;

  buf_len = p_conn->BufLen - p_conn->RxDataLenRem;

  while (DEF_TRUE) {
    switch (p_conn->State) {
      case HTTPc_CONN_STATE_RESP_BODY:
      case HTTPc_CONN_STATE_RESP_BODY_CHUNK_SIZE:
        p_size_start = p_conn->RxBufPtr;

        p_size_end = Str_Str_N(p_size_start,                    // Find end of size of chunk.
                               STR_CR_LF,
                               p_conn->RxDataLenRem);
        if (p_size_end == DEF_NULL) {
          *p_status = HTTPc_STATUS_NEED_MORE_DATA;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          goto exit;
        }
        //                                                         Convert the hexadecimal string to 32 bits integer.
        chunk_len = Str_ParseNbr_Int32U(p_size_start, &p_str, 16);
        if (p_str == p_size_start) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }

        p_conn->RxDataLen = chunk_len;

        p_size_end += STR_CR_LF_LEN;
        p_conn->RxDataLenRem -= p_size_end - p_size_start;
        p_conn->RxBufPtr = p_size_end;

        if (chunk_len == 0) {
          p_conn->State = HTTPc_CONN_STATE_RESP_BODY_CHUNK_LAST;
        } else {
          p_conn->State = HTTPc_CONN_STATE_RESP_BODY_CHUNK_DATA;
        }
        break;

      case HTTPc_CONN_STATE_RESP_BODY_CHUNK_DATA:
        p_chunk_start = p_conn->RxBufPtr;

        //                                                         If data in the buffer is less than the chunk length.
        if (p_conn->RxDataLenRem < p_conn->RxDataLen) {
          if (buf_len > 0) {
            *p_status = HTTPc_STATUS_NEED_MORE_DATA;
            RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                // buffer not full, go get more data.
            goto exit;
          } else {
            //                                                     Part of chunk received in buffer.
            data_len = p_conn->RxDataLenRem;
          }
        } else {
          data_len = p_conn->RxDataLen;                         // End of chunk received in buffer.
        }

        if (p_req->OnBodyRx != DEF_NULL) {
          //                                                       Pass data rx to application.
          data_len_read = p_req->OnBodyRx(p_conn_const,
                                          p_req_const,
                                          p_resp->ContentType,
                                          p_conn->RxBufPtr,
                                          data_len,
                                          DEF_NO);
        } else {
          data_len_read = data_len;
        }

        p_conn->RxDataLenRem -= data_len_read;
        p_conn->RxBufPtr = p_chunk_start + data_len_read;
        p_conn->RxDataLen -= data_len_read;
        if (p_conn->RxDataLenRem >= p_conn->RxDataLen) {
          if (data_len_read >= data_len) {
            p_conn->State = HTTPc_CONN_STATE_RESP_BODY_CHUNK_CRLF;
          }
        } else {
          *p_status = HTTPc_STATUS_NEED_MORE_DATA;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                  // Go get more data.
          goto exit;
        }
        break;

      case HTTPc_CONN_STATE_RESP_BODY_CHUNK_CRLF:
        if (p_conn->RxDataLenRem < STR_CR_LF_LEN) {
          *p_status = HTTPc_STATUS_NEED_MORE_DATA;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          goto exit;
        }

        p_str = Str_Str_N(p_conn->RxBufPtr,                   // Find end of header field.
                          STR_CR_LF,
                          p_conn->RxDataLenRem);
        if (p_str == DEF_NULL) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }

        p_conn->RxDataLenRem -= STR_CR_LF_LEN;
        p_conn->RxBufPtr += STR_CR_LF_LEN;

        p_conn->State = HTTPc_CONN_STATE_RESP_BODY_CHUNK_SIZE;
        break;

      case HTTPc_CONN_STATE_RESP_BODY_CHUNK_LAST:
        if (p_req->OnBodyRx != DEF_NULL) {
          (void)p_req->OnBodyRx(p_conn_const,
                                p_req_const,
                                p_resp->ContentType,
                                DEF_NULL,
                                0,
                                DEF_YES);
        }
        p_conn->State = HTTPc_CONN_STATE_RESP_COMPLETED;
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        goto exit;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
    }
  }

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_CLIENT_AVAIL
