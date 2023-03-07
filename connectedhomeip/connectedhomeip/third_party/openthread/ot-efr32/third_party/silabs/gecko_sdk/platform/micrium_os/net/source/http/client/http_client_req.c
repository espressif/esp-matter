/***************************************************************************//**
 * @file
 * @brief Network - HTTP Client Request Module
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

#ifdef  HTTPc_WEBSOCK_MODULE_EN
#include  "http_client_websock_priv.h"
#endif

#include  <net/include/http_client.h>
#include  <net/include/http.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_ascii.h>
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

#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
static CPU_SIZE_T HTTPcReq_PrepareFormAppLen(HTTPc_CONN *p_conn,
                                             RTOS_ERR   *p_err);

static CPU_SIZE_T HTTPcReq_PrepareFormMultipartLen(HTTPc_CONN *p_conn,
                                                   RTOS_ERR   *p_err);
#endif

static void HTTPcReq_Line(HTTPc_CONN *p_conn,
                          RTOS_ERR   *p_err);

#if (HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED)
static CPU_CHAR *HTTPcReq_QueryStrTbl(HTTPc_CONN   *p_conn,
                                      CPU_CHAR     *p_buf,
                                      CPU_INT16U   buf_len_rem,
                                      HTTPc_STATUS *p_status,
                                      RTOS_ERR     *p_err);

static CPU_CHAR *HTTPcReq_QueryStrHook(HTTPc_CONN   *p_conn,
                                       CPU_CHAR     *p_buf,
                                       CPU_INT16U   buf_len_rem,
                                       HTTPc_STATUS *p_status,
                                       RTOS_ERR     *p_err);
#endif

static void HTTPcReq_Hdr(HTTPc_CONN *p_conn,
                         RTOS_ERR   *p_err);

#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
static CPU_CHAR *HTTPcReq_HdrExtTbl(HTTPc_CONN *p_conn,
                                    CPU_CHAR   *p_buf,
                                    CPU_INT16U buf_len_rem,
                                    RTOS_ERR   *p_err);

static CPU_CHAR *HTTPcReq_HdrExtHook(HTTPc_CONN *p_conn,
                                     CPU_CHAR   *p_buf,
                                     CPU_INT16U buf_len_rem,
                                     RTOS_ERR   *p_err);

static HTTPc_HDR *HTTPcReq_HdrSearch(HTTPc_CONN     *p_conn,
                                     HTTP_HDR_FIELD hdr_field);
#endif

static void HTTPcReq_Body(HTTPc_CONN *p_conn,
                          RTOS_ERR   *p_err);
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
static void HTTPcReq_FormApp(HTTPc_CONN *p_conn,
                             RTOS_ERR   *p_err);

static void HTTPcReq_FormMultipart(HTTPc_CONN *p_conn,
                                   RTOS_ERR   *p_err);
#endif

static void HTTPcReq_BodyData(HTTPc_CONN *p_conn,
                              RTOS_ERR   *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          HTTPcReq_Prepare()
 *
 * @brief    - (1) Additional preparation before request processing. Sets which HTTP mandatory headers to
 *                 include:
 *                 - (a) Host
 *                 - (b) Connection
 *                 - (c) Content-Type
 *                 - (d) Content-Length
 *                 - (e) Transfer-Encoding
 *
 *           - (2) In case form table is present, calculate form size for Content-Length header.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 *
 * Argument(s) : p_conn      Pointer to current HTTPc Connection.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : None.
 *
 * Note(s)     : (1) The following HTTP headers must not be passed inside the extra header table. They
 *                   will be set by the HTTP client core according to the arguments passed by the API
 *                   functions :
 *                       (a) Host
 *                       (b) Connection
 *                       (c) Content-Type
 *                       (d) Content-Length
 *                       (e) Transfer-Encoding
 *
 *               (2) The Host Header is mandatory for HTTP/1.1 protocol version communication.
 *
 *               (3) If a body is present in the request, one of the bellow header MUST be present :
 *                       (a) Transfer-Encoding
 *                       (b) Content-Length
 *
 *               (4) Chunk Transfer-Encoding is only available in HTTP/1.1.
 *******************************************************************************************************/
void HTTPcReq_Prepare(HTTPc_CONN *p_conn,
                      RTOS_ERR   *p_err)
{
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
  HTTPc_HDR *p_hdr;
#endif
  HTTPc_REQ   *p_req;
  HTTP_DICT   *p_entry;
  CPU_BOOLEAN persistent;
  CPU_BOOLEAN chunk_en;
  CPU_BOOLEAN body_present;
  CPU_BOOLEAN form_present;
#ifdef HTTPc_TASK_MODULE_EN
  CPU_BOOLEAN no_block;
#endif
#ifdef  HTTPc_WEBSOCK_MODULE_EN
  CPU_BOOLEAN websocket;
#endif

  p_req = p_conn->ReqListHeadPtr;

  persistent = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_PERSISTENT);
  chunk_en = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_BODY_CHUNK_TRANSFER);
  body_present = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_BODY_PRESENT);
  form_present = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_FORM_PRESENT);
#ifdef  HTTPc_WEBSOCK_MODULE_EN
  websocket = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_UPGRADE_WEBSOCKET);
#endif
  //                                                               -------------- VALIDATE QUERY STRING ---------------
#if (HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED)

  if ((p_req->QueryStrNbr > 0)
      && (p_req->QueryStrTbl == DEF_NULL)) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
  }
#endif

  //                                                               ----------- VALIDATE ADDITIONAL HEADERS ------------
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
  if ((p_req->HdrNbr > 0)
      && (p_req->HdrTbl == DEF_NULL)) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
  }
#endif

#if  (HTTPc_CFG_FORM_EN == DEF_ENABLED)
  if ((p_req->DataPtr != DEF_NULL)                              // Cannot have a body pointer and a form pointer at ...
      && (p_req->FormFieldTbl != DEF_NULL)) {                   // ... same time.
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
  }
#endif

  //                                                               -------------- VALIDATE REQUEST BODY ---------------
  if (body_present == DEF_YES) {
    p_entry = HTTP_Dict_EntryGet(HTTP_Dict_ContentType,
                                 HTTP_Dict_ContentTypeSize,
                                 p_req->ContentType);
    RTOS_ASSERT_DBG_ERR_SET((p_entry != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );

    if (form_present == DEF_NO) {
      if ((chunk_en == DEF_NO)
          && (p_req->ContentLen == 0u)) {
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
      }
    }
  }

  //                                                               -------------- VALIDATE REQUEST FORM ---------------
  if (form_present == DEF_YES) {
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
    switch (p_req->ContentType) {
      case HTTP_CONTENT_TYPE_APP_FORM:
      case HTTP_CONTENT_TYPE_MULTIPART_FORM:
        break;

      default:
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
    }

    if ((p_req->FormFieldNbr > 0)
        && (p_req->FormFieldTbl == DEF_NULL)) {
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
    }
#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
  }

  //                                                               ---------------- VALIDATE CALLBACKS ----------------
#ifdef HTTPc_TASK_MODULE_EN
  no_block = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_NO_BLOCK);
  if (no_block == DEF_YES) {
    RTOS_ASSERT_DBG_ERR_SET((p_req->OnTransComplete != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG,; );
    RTOS_ASSERT_DBG_ERR_SET((p_req->OnErr != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG,; );
  }
#endif

  //                                                               ------ VALIDATE HTTP HEADERS RELATED TO CONN -------
  //                                                               HOST HEADER CASE.
  //                                                               See Note #2.
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
  p_hdr = HTTPcReq_HdrSearch(p_conn,                            // Search Host header in header list.
                             HTTP_HDR_FIELD_HOST);
  RTOS_ASSERT_DBG_ERR_SET((p_hdr == DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif
  DEF_BIT_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_HOST_ADD);

  //                                                               WEBSOCKET CASE.
#ifdef  HTTPc_WEBSOCK_MODULE_EN
  if (websocket == DEF_YES) {
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
    p_hdr = HTTPcReq_HdrSearch(p_conn,                          // Search Connection header in header list.
                               HTTP_HDR_FIELD_CONN);
    RTOS_ASSERT_DBG_ERR_SET((p_hdr == DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

    DEF_BIT_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_CONNECTION_ADD);
    DEF_BIT_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_UPGRADE_ADD);
    DEF_BIT_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_WEBSOCKET_ADD);
  }
#endif

  //                                                               CONNECTION HEADER CASE.
#ifdef  HTTPc_WEBSOCK_MODULE_EN
  if ((persistent == DEF_NO) && (websocket == DEF_NO)) {
#else
  if (persistent == DEF_NO) {
#endif
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
    p_hdr = HTTPcReq_HdrSearch(p_conn,                          // Search Connection header in header list.
                               HTTP_HDR_FIELD_CONN);
    RTOS_ASSERT_DBG_ERR_SET((p_hdr == DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif
    DEF_BIT_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_CONNECTION_ADD);
    DEF_BIT_SET(p_conn->Flags, HTTPc_FLAG_CONN_TO_CLOSE);
    p_conn->CloseStatus = HTTPc_CONN_CLOSE_STATUS_NO_PERSISTENT;
  }

  //                                                               ------ VALIDATE HTTP HEADERS RELATED TO BODY -------
  if (body_present == DEF_YES) {
    //                                                             CONTENT-TYPE HEADER CASE.
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
    p_hdr = HTTPcReq_HdrSearch(p_conn,                          // Search Content-Type header in header list.
                               HTTP_HDR_FIELD_CONTENT_TYPE);
    RTOS_ASSERT_DBG_ERR_SET((p_hdr == DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

    DEF_BIT_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_CONTENT_TYPE_ADD);

    //                                                             TRANSFER-ENCODING HEADER CASE.
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
    p_hdr = HTTPcReq_HdrSearch(p_conn,                          // Search Transfer-Encoding header in header list.
                               HTTP_HDR_FIELD_TRANSFER_ENCODING);
    RTOS_ASSERT_DBG_ERR_SET((p_hdr == DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

    if (chunk_en == DEF_YES) {
      DEF_BIT_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_TRANSFER_ENCODE_ADD);
    }

    //                                                             CONTENT-LENGTH HEADER CASE.
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
    p_hdr = HTTPcReq_HdrSearch(p_conn,                          // Search Content-Length header in header list.
                               HTTP_HDR_FIELD_CONTENT_LEN);
    RTOS_ASSERT_DBG_ERR_SET((p_hdr == DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

    if (p_req->ContentLen > 0u) {
      DEF_BIT_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_CONTENT_LENGTH_ADD);
    }

#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
    //                                                             - CALCULATE FORM LENGTH FOR CONTENT-LENGTH HEADER --
    if (form_present == DEF_YES) {
      DEF_BIT_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_CONTENT_LENGTH_ADD);

      switch (p_req->ContentType) {
        case HTTP_CONTENT_TYPE_APP_FORM:
          p_req->ContentLen = HTTPcReq_PrepareFormAppLen(p_conn, p_err);
          break;

        case HTTP_CONTENT_TYPE_MULTIPART_FORM:
          p_req->ContentLen = HTTPcReq_PrepareFormMultipartLen(p_conn, p_err);
          break;

        default:
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
      }

      DEF_BIT_CLR(p_req->HdrFlags, (HTTPc_FLAGS)HTTPc_FLAG_REQ_HDR_TRANSFER_ENCODE_ADD);
    }
#endif
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                              HTTPcReq()
 *
 * @brief    - (1) Send data to TCP-IP stack if data is ready to be sent.
 *           - (2) State Machine for the Request processing:
 *               - (a) Prepare Request Line.
 *               - (b) Prepare Headers.
 *               - (c) Prepare Body.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *******************************************************************************************************/
CPU_BOOLEAN HTTPcReq(HTTPc_CONN *p_conn,
                     RTOS_ERR   *p_err)
{
  CPU_BOOLEAN process;
  CPU_BOOLEAN done;
  CPU_BOOLEAN sock_rdy_wr;
  CPU_BOOLEAN sock_rdy_err;

  done = DEF_NO;
  process = DEF_YES;
  //                                                               -------- PASS DATA TO SEND TO TCP-IP STACK ---------
  if (p_conn->TxDataLen > 0) {
    sock_rdy_wr = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPc_FLAG_SOCK_RDY_WR);
    sock_rdy_err = DEF_BIT_IS_SET(p_conn->SockFlags, HTTPc_FLAG_SOCK_RDY_ERR);

    if ((sock_rdy_wr == DEF_YES)
        || (sock_rdy_err == DEF_YES)) {
      process = HTTPcSock_ConnDataTx(p_conn, p_err);
      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_NONE:
        case RTOS_ERR_POOL_EMPTY:
        case RTOS_ERR_NET_IF_LINK_DOWN:
        case RTOS_ERR_TIMEOUT:
        case RTOS_ERR_WOULD_BLOCK:
          break;

        default:
          goto exit;
      }
    }
  }
  //                                                               -------- PROCESS INCOMING REQUEST FROM APP ---------
  if (process == DEF_YES) {
    switch (p_conn->State) {
      case HTTPc_CONN_STATE_REQ_LINE_METHOD:
      case HTTPc_CONN_STATE_REQ_LINE_URI:
      case HTTPc_CONN_STATE_REQ_LINE_QUERY_STR:
      case HTTPc_CONN_STATE_REQ_LINE_PROTO_VER:
        HTTPcReq_Line(p_conn, p_err);                            // Prepare Request Line.
        break;

      case HTTPc_CONN_STATE_REQ_HDR_HOST:
      case HTTPc_CONN_STATE_REQ_HDR_CONTENT_TYPE:
      case HTTPc_CONN_STATE_REQ_HDR_CONTENT_LEN:
      case HTTPc_CONN_STATE_REQ_HDR_TRANSFER_ENCODE:
      case HTTPc_CONN_STATE_REQ_HDR_CONN:
      case HTTPc_CONN_STATE_REQ_HDR_EXT:
      case HTTPc_CONN_STATE_REQ_HDR_LAST:
        HTTPcReq_Hdr(p_conn, p_err);                            // Prepare Request Headers.
        break;

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
        HTTPcReq_Body(p_conn, p_err);                           // Prepare Request Body.
        break;

      case HTTPc_CONN_STATE_REQ_END:
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        if (p_conn->TxDataLen == 0u) {                          // Validate Tx is completed before switch to Resp.
          done = DEF_YES;
          p_conn->State = HTTPc_CONN_STATE_RESP_INIT;
        }
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE, DEF_YES);
    }
  }

exit:
  return (done);
}

/****************************************************************************************************//**
 *                                        HTTPcReq_HdrCopyToBuf()
 *
 * @brief    Construct and Copy HTTP header to HTTP buffer.
 *
 * @param    p_buf       Pointer to HTTP buffer.
 *
 * @param    buf_len     Length remaining in HTTP buffer.
 *
 * @param    hdr_type    HTTP header type.
 *
 * @param    p_val       Pointer to HTTP header value.
 *
 * @param    val_len     HTTP header value length.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to location in HTTP buffer after data written.
 *******************************************************************************************************/
CPU_CHAR *HTTPcReq_HdrCopyToBuf(CPU_CHAR       *p_buf,
                                CPU_INT16U     buf_len,
                                CPU_SIZE_T     buf_len_rem,
                                HTTP_HDR_FIELD hdr_type,
                                const CPU_CHAR *p_val,
                                CPU_SIZE_T     val_len,
                                CPU_BOOLEAN    add_CRLF,
                                HTTPc_STATUS   *p_status,
                                RTOS_ERR       *p_err)
{
  HTTP_DICT  *p_entry;
  CPU_CHAR   *p_str;
  CPU_SIZE_T len_need;

  p_str = p_buf;

  //                                                               Find Header type in dictionary.
  p_entry = HTTP_Dict_EntryGet(HTTP_Dict_HdrField,
                               HTTP_Dict_HdrFieldSize,
                               hdr_type);
  if (p_entry == DEF_NULL) {
    p_str = DEF_NULL;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  len_need = p_entry->StrLen + val_len + 2;                     // Calculate length needed for header.
  if (add_CRLF == DEF_YES) {
    len_need += STR_CR_LF_LEN;
  }

  //                                                               Total len of hdr must be smaller than HTTP buf size.
  RTOS_ASSERT_DBG_ERR_SET((len_need <= buf_len), *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);

  if (len_need > buf_len_rem) {                                 // Missing space in HTTP buf. Data must be Tx.
    p_str = DEF_NULL;
    *p_status = HTTPc_STATUS_NO_MORE_SPACE;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }
  //                                                               Copy Header Type to buffer.
  p_str = Str_Copy_N(p_str, p_entry->StrPtr, p_entry->StrLen);
  RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

  p_str += p_entry->StrLen;

  *p_str = ASCII_CHAR_COLON;                                    // Copy ":" character to buffer.
  p_str++;
  *p_str = ASCII_CHAR_SPACE;                                    // Copy space character to buffer.
  p_str++;

  p_str = Str_Copy_N(p_str, p_val, val_len);                    // Copy header value to buffer.
  RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

  p_str += val_len;

  if (add_CRLF == DEF_YES) {
    //                                                             Copy end of line CRLN characters to buffer.
    p_str = Str_Copy_N(p_str, STR_CR_LF, STR_CR_LF_LEN);
    RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

    p_str += STR_CR_LF_LEN;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (p_str);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                     HTTPcReq_PrepareFormAppLen()
 *
 * @brief    Calculate length of Application form by parsing form table passed by application.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Length of the application form.
 *           Length 0 in case of error.
 *
 * @note     (1) In case form table is passed by application and Chunked Transfer is disabled, length
 *               of form must be calculate to put inside Content-Length header.
 *
 * @note     (2) Application type Form must be URL-Encode.
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
static CPU_SIZE_T HTTPcReq_PrepareFormAppLen(HTTPc_CONN *p_conn,
                                             RTOS_ERR   *p_err)
{
  HTTPc_REQ            *p_req;
  HTTPc_FORM_TBL_FIELD *p_form_tbl;
  HTTPc_FORM_TBL_FIELD *p_tbl_entry;
  HTTPc_KEY_VAL        *p_kvp;
  CPU_SIZE_T           total_len;
  CPU_INT16U           char_encode_nbr;
  CPU_INT16U           i;

  p_req = p_conn->ReqListHeadPtr;

  p_form_tbl = (HTTPc_FORM_TBL_FIELD *)p_req->FormFieldTbl;
  total_len = 0;

  //                                                               ----------- PARSE APPLICATION FORM TABLE -----------
  for (i = 0; i < p_req->FormFieldNbr; i++) {
    p_tbl_entry = &p_form_tbl[i];

    switch (p_tbl_entry->Type) {
      case HTTPc_FORM_FIELD_TYPE_KEY_VAL:
        break;

      case HTTPc_FORM_FIELD_TYPE_KEY_VAL_EXT:
      case HTTPc_FORM_FIELD_TYPE_FILE:
      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);
    }

    p_kvp = (HTTPc_KEY_VAL *)p_tbl_entry->FieldObjPtr;

    char_encode_nbr = HTTP_URL_CharEncodeNbr(p_kvp->KeyPtr, p_kvp->KeyLen);

    total_len += (p_kvp->KeyLen + char_encode_nbr * HTTP_URL_ENCODING_JUMP);

    char_encode_nbr = HTTP_URL_CharEncodeNbr(p_kvp->ValPtr, p_kvp->ValLen);

    total_len += (p_kvp->ValLen + char_encode_nbr * HTTP_URL_ENCODING_JUMP);
  }

  total_len += 2 * p_req->FormFieldNbr - 1;                     // Add length of all "=" and "&" char to include ...
                                                                // ... in the form.

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (total_len);
}
#endif

/****************************************************************************************************//**
 *                                  HTTPcReq_PrepareFormMultipartLen()
 *
 * @brief    Calculate length of Multipart form by parsing form table passed by application.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Length of the application form.
 *           Length 0 in case of error.
 *
 * @note     (1) In case form table is passed by application and Chunked Transfer is disabled, length
 *               of form must be calculate to put inside Content-Length header.
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
static CPU_SIZE_T HTTPcReq_PrepareFormMultipartLen(HTTPc_CONN *p_conn,
                                                   RTOS_ERR   *p_err)
{
  HTTPc_REQ            *p_req;
  HTTPc_FORM_TBL_FIELD *p_form_tbl;
  HTTPc_FORM_TBL_FIELD *p_tbl_entry;
  HTTPc_KEY_VAL        *p_kvp;
  HTTPc_KEY_VAL_EXT    *p_kvp_big;
  HTTPc_MULTIPART_FILE *p_multipart_file;
  HTTP_DICT            *p_entry;
  CPU_SIZE_T           total_len;
  CPU_INT16U           char_encode_nbr;
  CPU_INT16U           i;

  p_req = p_conn->ReqListHeadPtr;

  p_form_tbl = p_req->FormFieldTbl;

  total_len = 0;

  for (i = 0; i < p_req->FormFieldNbr; i++) {
    p_tbl_entry = &p_form_tbl[i];

    //                                                             ----------- ADD LENGTH OF START BOUNDARY -----------
    total_len += HTTPc_STR_BOUNDARY_START_LEN + STR_CR_LF_LEN;

    //                                                             ------ ADD LENGTH OF CONTENT-DISPOSITION HDR -------
    total_len += HTTP_STR_HDR_FIELD_CONTENT_DISPOSITION_LEN
                 + 2
                 + HTTP_STR_CONTENT_DISPOSITION_FORM_DATA_LEN
                 + 2
                 + HTTP_STR_MULTIPART_FIELD_NAME_LEN
                 + 3
                 + STR_CR_LF_LEN;

    switch (p_tbl_entry->Type) {
      case HTTPc_FORM_FIELD_TYPE_KEY_VAL:
        p_kvp = (HTTPc_KEY_VAL *)p_tbl_entry->FieldObjPtr;
        //                                                         Add length of Name field.
        char_encode_nbr = HTTP_URL_CharEncodeNbr(p_kvp->KeyPtr, p_kvp->KeyLen);
        total_len += (p_kvp->KeyLen + char_encode_nbr * HTTP_URL_ENCODING_JUMP);
        //                                                         Add length of data.
        total_len += p_kvp->ValLen + 2 * STR_CR_LF_LEN;
        break;

      case HTTPc_FORM_FIELD_TYPE_KEY_VAL_EXT:
        p_kvp_big = (HTTPc_KEY_VAL_EXT *)p_tbl_entry->FieldObjPtr;
        //                                                         Add length of Name field.
        char_encode_nbr = HTTP_URL_CharEncodeNbr(p_kvp_big->KeyPtr, p_kvp_big->KeyLen);
        total_len += (p_kvp_big->KeyLen + char_encode_nbr * HTTP_URL_ENCODING_JUMP);
        //                                                         Add length of data.
        total_len += p_kvp_big->ValLen + 2 * STR_CR_LF_LEN;
        break;

      case HTTPc_FORM_FIELD_TYPE_FILE:
        p_multipart_file = (HTTPc_MULTIPART_FILE *)p_tbl_entry->FieldObjPtr;
        //                                                         Add length of Name field.
        char_encode_nbr = HTTP_URL_CharEncodeNbr(p_multipart_file->NamePtr, p_multipart_file->NameLen);
        total_len += (p_multipart_file->NameLen + char_encode_nbr * HTTP_URL_ENCODING_JUMP);
        //                                                         Add length of File Name field.
        char_encode_nbr = HTTP_URL_CharEncodeNbr(p_multipart_file->FileNamePtr, p_multipart_file->FileNameLen);
        total_len += (p_multipart_file->FileNameLen + char_encode_nbr * HTTP_URL_ENCODING_JUMP);
        total_len += 2 + HTTP_STR_MULTIPART_FIELD_FILE_NAME_LEN + 3;
        //                                                         Add length of Content-Type Header.
        p_entry = HTTP_Dict_EntryGet(HTTP_Dict_ContentType,
                                     HTTP_Dict_ContentTypeSize,
                                     p_multipart_file->ContentType);
        if (p_entry != DEF_NULL) {
          total_len += HTTP_STR_HDR_FIELD_CONTENT_TYPE_LEN
                       + 2
                       + p_entry->StrLen
                       + STR_CR_LF_LEN;
        }
        //                                                         Add length of data.
        total_len += p_multipart_file->FileLen + 2 * STR_CR_LF_LEN;
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);
    }
  }

  //                                                               ------------ ADD LENGTH OF END BOUNDARY ------------
  total_len += HTTPc_STR_BOUNDARY_END_LEN + STR_CR_LF_LEN;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (total_len);
}
#endif

/****************************************************************************************************//**
 *                                           HTTPcReq_Line()
 *
 * @brief    - (1) Prepare and Copy HTTP Request line to HTTP Tx buffer.
 *           - (2) An HTTP line consist of :
 *               - (a) Method
 *               - (b) URI
 *               - (c) Query String
 *               - (d) Protocol Version
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (3) HTTP Line parameters previously validate in API function HTTPc_SetReqLineParam().
 *
 * @note     (4) The Request line processing is design as a state machine with 4 different states:
 *               - HTTPc_CONN_STATE_REQ_LINE_METHOD
 *               - HTTPc_CONN_STATE_REQ_LINE_URI
 *               - HTTPc_CONN_STATE_REQ_LINE_QUERY_STR
 *               - HTTPc_CONN_STATE_REQ_LINE_PROTO_VER
 *
 * @note     (5) The data length of each state MUST be smaller than the configured HTTPc buffer length.
 *               Else the process is aborted and the err code HTTPc_ERR_TRANS_BUF_LEN_INVALID is returned.
 *               The buffer length MUST be reconfigured and increased.
 *               Exception for the URI, see Note #7.
 *               Exception for the Query String, see Note #8.
 *
 * @note     (6) The function will not return until all the request line has been copied to the HTTP
 *               buffer or if the buffer is full before the end of the line processing. In that case,
 *               the state is saved and the function will exit to allow the data to be transmit.
 *               Next time around the process will resume at the saved state.
 *
 * @note     (7) The URI can be bigger than the HTTP buffer length. In that case, the URI is not copied
 *               into the HTTP buffer, the pointer to the URI will be passed directly to the TCPI-IP
 *               stack.
 *
 * @note     (8) See Note #1 of function HTTPcReq_QueryStrTbl() and HTTPcReq_QueryStrHook().
 *******************************************************************************************************/
static void HTTPcReq_Line(HTTPc_CONN *p_conn,
                          RTOS_ERR   *p_err)
{
  HTTPc_REQ  *p_req;
  HTTP_DICT  *p_entry;
  CPU_CHAR   *p_buf;
  CPU_CHAR   *p_buf_wr_start;
  CPU_CHAR   *p_buf_wr_end;
  CPU_SIZE_T buf_len;
  CPU_SIZE_T str_len;
#if (HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED)
  HTTPc_STATUS rtn_status = HTTPc_STATUS_NONE;
#endif

  p_req = p_conn->ReqListHeadPtr;

  p_buf = p_conn->TxBufPtr + p_conn->TxDataLen;
  buf_len = p_conn->BufLen   - p_conn->TxDataLen;
  p_buf_wr_start = p_buf;
  p_buf_wr_end = p_buf;

  switch (p_conn->State) {
    case HTTPc_CONN_STATE_REQ_LINE_METHOD:
      //                                                           -------------- PREPARE REQ LINE METHOD--------------
      //                                                           Find Method in HTTP dictionary.
      p_entry = HTTP_Dict_EntryGet(HTTP_Dict_ReqMethod,
                                   HTTP_Dict_ReqMethodSize,
                                   p_req->Method);
      //                                                           Should not occur. See note #1.
      RTOS_ASSERT_CRITICAL((p_entry != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      str_len = p_entry->StrLen + 1;                            // Length of method + length of space char.

      //                                                           Should not occur.
      RTOS_ASSERT_DBG_ERR_SET((str_len <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

      if (str_len > buf_len) {                                  // HTTP buf space not enough, return to Tx data.
        p_conn->TxDataLen += (p_buf_wr_start - p_buf);
        p_conn->TxDataPtr = p_conn->TxBufPtr;
        p_conn->State = HTTPc_CONN_STATE_REQ_LINE_METHOD;
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                        // Exit to transmit data in buffer.
        goto exit;
      }
      //                                                           Write Request method to buffer.
      p_buf_wr_end = Str_Copy_N(p_buf_wr_start, p_entry->StrPtr, p_entry->StrLen);
      RTOS_ASSERT_CRITICAL((p_buf_wr_end != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      p_buf_wr_end += p_entry->StrLen;

      *p_buf_wr_end = ASCII_CHAR_SPACE;                         // Write space character to buffer.
      p_buf_wr_end++;

      buf_len -= (p_buf_wr_end - p_buf_wr_start);               // Update buffer length remaining.
      p_buf_wr_start = p_buf_wr_end;
    //                                                             'break' intentionally omitted.
    //                                                             fallthrough

    case HTTPc_CONN_STATE_REQ_LINE_URI:
      //                                                           --------------- PREPARE REQ LINE URI----------------
      str_len = Str_Len_N(p_req->ResourcePathPtr, p_req->ResourcePathLen);

      if (str_len <= buf_len) {
        //                                                         IF HTTP buf space is enough, copy Resource path.
        p_buf_wr_end = Str_Copy_N(p_buf_wr_start, p_req->ResourcePathPtr, str_len);
        RTOS_ASSERT_CRITICAL((p_buf_wr_end != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_buf_wr_end += str_len;

        buf_len -= (p_buf_wr_end - p_buf_wr_start);                // Update buffer length remaining.
        p_buf_wr_start = p_buf_wr_end;
      } else {                                                  // ELSE (buffer space is not enough) ...
        switch (p_conn->State) {
          case HTTPc_CONN_STATE_REQ_LINE_METHOD:                // Start by exit function to allow remaining data in ...
                                                                // ... buffer to be transmit.
            p_conn->TxDataLen += (p_buf_wr_end - p_buf);
            p_conn->TxDataPtr = p_conn->TxBufPtr;
            p_conn->State = HTTPc_CONN_STATE_REQ_LINE_URI;
            break;

          case HTTPc_CONN_STATE_REQ_LINE_URI:                   // Second time around, set TxDataPtr to point to ...
                                                                // ... Resource Path ptr.
            p_conn->TxDataLen = str_len;
            p_conn->TxDataPtr = p_req->ResourcePathPtr;
            p_conn->State = HTTPc_CONN_STATE_REQ_LINE_QUERY_STR;
            break;

          default:
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
        }
        break;                                                  // break from upper HTTPc_CONN_STATE_REQ_LINE_URI case.
      }
    //                                                             'break' intentionally omitted.
    //                                                             fallthrough

    case HTTPc_CONN_STATE_REQ_LINE_QUERY_STR:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      //                                                           ---------- PREPARE REQ LINE QUERY STRINGS ----------
#if (HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED)
      if (p_req->QueryStrTbl != DEF_NULL) {
        //                                                         Write String Query to buffer.
        p_buf_wr_end = HTTPcReq_QueryStrTbl(p_conn,
                                            p_buf_wr_start,
                                            buf_len,
                                            &rtn_status,
                                            p_err);
      }
      if (p_req->OnQueryStrTx != DEF_NULL) {
        //                                                         Write String Query to buffer.
        p_buf_wr_end = HTTPcReq_QueryStrHook(p_conn,
                                             p_buf_wr_start,
                                             buf_len,
                                             &rtn_status,
                                             p_err);
      }
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        p_conn->TxDataLen = 0;
        goto exit;
      }
      switch (rtn_status) {
        case HTTPc_STATUS_NONE:                                 // All the Query Strings were copied to the HTTP buf.
          buf_len -= (p_buf_wr_end - p_buf_wr_start);
          p_buf_wr_start = p_buf_wr_end;
          break;

        case HTTPc_STATUS_NO_MORE_SPACE:                             // HTTP buf full before all Queries were copied.
          p_conn->TxDataLen += (p_buf_wr_end - p_buf);
          p_conn->TxDataPtr = p_conn->TxBufPtr;
          p_conn->State = HTTPc_CONN_STATE_REQ_LINE_QUERY_STR;
          goto exit;

        default:
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
#endif
      p_conn->TxDataPtr = p_conn->TxBufPtr;                     // Reset TxDataPtr to point to the HTTP buffer.
    //                                                             'break' intentionally omitted.
    //                                                             fallthrough

    case HTTPc_CONN_STATE_REQ_LINE_PROTO_VER:
      //                                                           -------- PREPARE REQ LINE PROTOCOL VERSION ---------
      //                                                           Find HTTP version in dictionary.
      p_entry = HTTP_Dict_EntryGet(HTTP_Dict_ProtocolVer,
                                   HTTP_Dict_ProtocolVerSize,
                                   HTTP_PROTOCOL_VER_1_1);
      //                                                           Should not occur. See note #1.
      RTOS_ASSERT_CRITICAL((p_entry != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      str_len = p_entry->StrLen + 1 + STR_CR_LF_LEN;            // total len = len of version + len of space +
                                                                // len of end of line.

      //                                                           See Note #5.
      RTOS_ASSERT_DBG_ERR_SET((str_len <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

      if (str_len > buf_len) {                                  // HTTP Buf space remaining not enough.
        p_conn->TxDataLen += (p_buf_wr_start - p_buf);
        p_conn->TxDataPtr = p_conn->TxBufPtr;
        p_conn->State = HTTPc_CONN_STATE_REQ_LINE_PROTO_VER;
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                        // Exit to transmit data in buffer.
        goto exit;
      }

      *p_buf_wr_end = ASCII_CHAR_SPACE;                         // Copy Space character.
      p_buf_wr_end++;

      //                                                           Copy HTTP Protocol version.
      p_buf_wr_end = Str_Copy_N(p_buf_wr_end, p_entry->StrPtr, p_entry->StrLen);
      RTOS_ASSERT_CRITICAL((p_buf_wr_end != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      p_buf_wr_end += p_entry->StrLen;

      //                                                           Copy end of line CRLF.
      p_buf_wr_end = Str_Copy_N(p_buf_wr_end, STR_CR_LF, buf_len);
      RTOS_ASSERT_CRITICAL((p_buf_wr_end != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      p_buf_wr_end += STR_CR_LF_LEN;

      p_conn->TxDataLen = (p_buf_wr_end - p_buf);               // Update data length to transmit.
      p_conn->TxDataPtr = p_conn->TxBufPtr;
      p_conn->State = HTTPc_CONN_STATE_REQ_HDR_HOST;            // Set next state to prepare request headers.
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;
}

/****************************************************************************************************//**
 *                                     HTTPcReq_QueryStrTbl()
 *
 * @brief    Parse Query String table and prepare the query string to be transmitted.
 *
 * @param    p_conn          Pointer to current HTTPc connection.
 *
 * @param    p_buf           Pointer to where the data must be copied in the HTTP buffer.
 *
 * @param    buf_len_rem     Buffer length remaining.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to location after the end of the data written.
 *
 * @note     (1) Since the Query Strings are passed by the App in the form of a table, the process will
 *               copy as much as queries possible to the HTTP buffer before returning. If not all the
 *               queries fit inside the buffer, the index of the table will be saved and the function
 *               will exit to allow transmission of data. Next time around the process will resume
 *               at the saved index.
 *
 * @note     (2) Note that each Query (Key-Value Pair) MUST individually fit inside the HTTP buffer
 *               since a query is not divisible.
 *******************************************************************************************************/
#if HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED
static CPU_CHAR *HTTPcReq_QueryStrTbl(HTTPc_CONN   *p_conn,
                                      CPU_CHAR     *p_buf,
                                      CPU_INT16U   buf_len_rem,
                                      HTTPc_STATUS *p_status,
                                      RTOS_ERR     *p_err)
{
  HTTPc_REQ     *p_req;
  HTTPc_KEY_VAL *p_kvp;
  CPU_CHAR      *p_buf_wr;
  CPU_SIZE_T    str_len_key;
  CPU_SIZE_T    str_len_val;
  CPU_SIZE_T    space_req;
  CPU_BOOLEAN   first;
  CPU_BOOLEAN   url_encode;
  CPU_INT16U    encode_nbr;
  CPU_INT08U    i;

  p_req = p_conn->ReqListHeadPtr;
  //                                                               ------------- PARSE QUERY STRING TABLE -------------
  p_buf_wr = p_buf;
  p_kvp = &p_req->QueryStrTbl[0];
  first = DEF_BIT_IS_CLR(p_conn->ReqFlags, HTTPc_FLAG_REQ_LINE_QUERY_STR_BEGIN);

  for (i = p_conn->ReqQueryStrTxIx; i < p_req->QueryStrNbr; i++) {
    if (first == DEF_YES) {                                     // Query String start with a ? after the URI.
      *p_buf_wr = ASCII_CHAR_QUESTION_MARK;
      p_buf_wr++;
      buf_len_rem--;
      DEF_BIT_SET(p_conn->ReqFlags, HTTPc_FLAG_REQ_LINE_QUERY_STR_BEGIN);
      first = DEF_NO;
    }

    //                                                             Calculate total length required by one key-val pair.
    space_req = 0;
    str_len_key = Str_Len_N(p_kvp->KeyPtr, p_kvp->KeyLen);
    encode_nbr = HTTP_URL_CharEncodeNbr(p_kvp->KeyPtr, str_len_key);
    space_req += (str_len_key + encode_nbr * HTTP_URL_ENCODING_JUMP);

    if (p_kvp->ValPtr != DEF_NULL) {
      str_len_val = Str_Len_N(p_kvp->ValPtr, p_kvp->ValLen);
      encode_nbr = HTTP_URL_CharEncodeNbr(p_kvp->ValPtr, str_len_val);
      space_req += (str_len_val + encode_nbr * HTTP_URL_ENCODING_JUMP);

      space_req += 1;                                           // Add length of "=" character.
    }

    space_req += 1;                                             // Add length of "&" character.

    //                                                             Return error if HTTP buf is to small for 1 pair.
    RTOS_ASSERT_DBG_ERR_SET((space_req <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG, p_buf_wr);

    if (space_req > buf_len_rem) {                              // HTTP buf remaining space not enough.
      p_conn->ReqQueryStrTxIx = i;                              // Save table index to resume at right place.
      *p_status = HTTPc_STATUS_NO_MORE_SPACE;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit;                                                // Exit to allow data to be transmitted.
    }

    url_encode = HTTP_URL_EncodeStr(p_kvp->KeyPtr,              // URL-encode and copy the query key.
                                    p_buf_wr,
                                    &str_len_key,
                                    buf_len_rem);
    RTOS_ASSERT_CRITICAL((url_encode == DEF_OK), RTOS_ERR_ASSERT_CRITICAL_FAIL, p_buf);

    p_buf_wr += str_len_key;
    buf_len_rem -= str_len_key;

    if (p_kvp->ValPtr != DEF_NULL) {
      *p_buf_wr = ASCII_CHAR_EQUALS_SIGN;                       // Copy "=" character between key and value.
      p_buf_wr++;
      buf_len_rem--;

      url_encode = HTTP_URL_EncodeStr(p_kvp->ValPtr,            // URL-encode and copy the query value.
                                      p_buf_wr,
                                      &str_len_val,
                                      buf_len_rem);
      RTOS_ASSERT_CRITICAL((url_encode == DEF_OK), RTOS_ERR_ASSERT_CRITICAL_FAIL, p_buf);

      p_buf_wr += str_len_val;
      buf_len_rem -= str_len_val;
    }

    if (i < (p_req->QueryStrNbr - 1)) {
      *p_buf_wr = ASCII_CHAR_AMPERSAND;                         // Copy "&" character if query pair is not the last.
      p_buf_wr++;
      buf_len_rem--;
    } else {
      p_conn->ReqQueryStrTxIx = 0u;
    }

    p_buf = p_buf_wr;

    p_kvp++;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (p_buf_wr);
}
#endif

/****************************************************************************************************//**
 *                                      HTTPcReq_QueryStrHook()
 *
 * @brief    - (1) Call hook function to recover each part of the query string.
 *           - (2) Prepare the query string to be transmitted.
 *
 * @param    p_conn          Pointer to current HTTPc Connection object.
 *
 * @param    p_buf           Pointer to where the data must be copied in the HTTP buffer.
 *
 * @param    buf_len_rem     Buffer length remaining.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to location after the end of the data written.
 *
 * @note     (1) Since the Query Strings are passed by the App hook function, the process will
 *               copy as much as queries possible to the HTTP buffer before returning. If not all the
 *               queries fit inside the buffer, a pointer to the last key-value pair will be saved and
 *               the function will exit to allow transmission of data. Next time around the process will resume
 *               at the saved pointer before calling the hook.
 *
 * @note     (2) Note that each Query (Key-Value Pair) MUST individually fit inside the HTTP buffer
 *               since a query is not divisible.
 *******************************************************************************************************/
#if HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED
static CPU_CHAR *HTTPcReq_QueryStrHook(HTTPc_CONN   *p_conn,
                                       CPU_CHAR     *p_buf,
                                       CPU_INT16U   buf_len_rem,
                                       HTTPc_STATUS *p_status,
                                       RTOS_ERR     *p_err)
{
  HTTPc_CONN_OBJ *p_conn_const;
  HTTPc_REQ      *p_req;
  HTTPc_REQ_OBJ  *p_req_const;
  HTTPc_KEY_VAL  *p_kvp;
  CPU_CHAR       *p_buf_wr;
  CPU_SIZE_T     str_len_key;
  CPU_SIZE_T     str_len_val;
  CPU_SIZE_T     space_req;
  CPU_BOOLEAN    first;
  CPU_BOOLEAN    done;
  CPU_BOOLEAN    url_encode;
  CPU_INT16U     encode_nbr;

  p_conn_const = (HTTPc_CONN_OBJ *)p_conn;

  p_req = p_conn->ReqListHeadPtr;
  p_req_const = (HTTPc_REQ_OBJ *)p_req;

  p_buf_wr = p_buf;
  done = DEF_NO;
  first = DEF_BIT_IS_CLR(p_conn->ReqFlags, HTTPc_FLAG_REQ_LINE_QUERY_STR_BEGIN);

  while (done == DEF_NO) {
    if (p_conn->ReqQueryStrTempPtr != DEF_NULL) {
      p_kvp = p_conn->ReqQueryStrTempPtr;
      done = DEF_BIT_IS_SET(p_conn->ReqFlags, HTTPc_FLAG_REQ_LINE_QUERY_STR_DONE);
      p_conn->ReqQueryStrTempPtr = DEF_NULL;
    } else {
      done = p_req->OnQueryStrTx(p_conn_const, p_req_const, &p_kvp);
      RTOS_ASSERT_DBG_ERR_SET((p_kvp != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);
    }

    if (first == DEF_YES) {                                     // Query String start with a ? after the URI.
      *p_buf_wr = ASCII_CHAR_QUESTION_MARK;
      p_buf_wr++;
      buf_len_rem--;
      DEF_BIT_SET(p_conn->ReqFlags, HTTPc_FLAG_REQ_LINE_QUERY_STR_BEGIN);
      first = DEF_NO;
    }

    //                                                             Calculate total length required by one key-val pair.
    space_req = 0;
    str_len_key = Str_Len_N(p_kvp->KeyPtr, p_kvp->KeyLen);
    encode_nbr = HTTP_URL_CharEncodeNbr(p_kvp->KeyPtr, str_len_key);
    space_req += (str_len_key + encode_nbr * HTTP_URL_ENCODING_JUMP);

    if (p_kvp->ValPtr != DEF_NULL) {
      str_len_val = Str_Len_N(p_kvp->ValPtr, p_kvp->ValLen);
      encode_nbr = HTTP_URL_CharEncodeNbr(p_kvp->ValPtr, str_len_val);
      space_req += (str_len_val + encode_nbr * HTTP_URL_ENCODING_JUMP);

      space_req += 1;                                           // Add length of "=" character.
    }

    space_req += 1;                                             // Add length of "&" character.

    //                                                             Return error if HTTP buf is to small for 1 pair.
    RTOS_ASSERT_DBG_ERR_SET((space_req <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG, p_buf_wr);

    if (space_req > buf_len_rem) {                              // HTTP buf remaining space not enough.
      p_conn->ReqQueryStrTempPtr = p_kvp;
      if (done == DEF_YES) {
        DEF_BIT_SET(p_conn->ReqFlags, HTTPc_FLAG_REQ_LINE_QUERY_STR_DONE);
      }
      *p_status = HTTPc_STATUS_NO_MORE_SPACE;                        // Exit to allow data to be transmitted.
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit;
    }

    url_encode = HTTP_URL_EncodeStr(p_kvp->KeyPtr,              // URL-encode and copy the query key.
                                    p_buf_wr,
                                    &str_len_key,
                                    buf_len_rem);
    RTOS_ASSERT_CRITICAL((url_encode == DEF_OK), RTOS_ERR_ASSERT_CRITICAL_FAIL, p_buf);

    p_buf_wr += str_len_key;
    buf_len_rem -= str_len_key;

    if (p_kvp->ValPtr != DEF_NULL) {
      *p_buf_wr = ASCII_CHAR_EQUALS_SIGN;                       // Copy "=" character between key and value.
      p_buf_wr++;
      buf_len_rem--;

      url_encode = HTTP_URL_EncodeStr(p_kvp->ValPtr,            // URL-encode and copy the query value.
                                      p_buf_wr,
                                      &str_len_val,
                                      buf_len_rem);
      RTOS_ASSERT_CRITICAL((url_encode == DEF_OK), RTOS_ERR_ASSERT_CRITICAL_FAIL, p_buf);

      p_buf_wr += str_len_val;
      buf_len_rem -= str_len_val;
    }

    if (done == DEF_NO) {
      *p_buf_wr = ASCII_CHAR_AMPERSAND;                         // Copy "&" character if query pair is not the last.
      p_buf_wr++;
      buf_len_rem--;
    }

    p_buf = p_buf_wr;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (p_buf_wr);
}
#endif

/****************************************************************************************************//**
 *                                            HTTPcReq_Hdr()
 *
 * @brief    (1) Prepare and Copy HTTP Request Headers to HTTP Tx buffer.
 *               - (a) Host, Connection, Content-Type, Content-Length and Transfer-Encoding headers
 *                     are processed by the HTTPc core.
 *               - (b) Extra Headers table given by application is parse to add headers to request.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) HTTP Header parameters previously validate in API function HTTPc_SetReqExtHdr()
 *               and in HTTPcReq_Prepare().
 *
 * @note     (3) The Request headers processing is design as a state machine with 4 different states:
 *               - HTTPc_CONN_STATE_REQ_HDR_HOST
 *               - HTTPc_CONN_STATE_REQ_HDR_CONN
 *               - HTTPc_CONN_STATE_REQ_HDR_CONTENT_TYPE
 *               - HTTPc_CONN_STATE_REQ_HDR_TRANSFER_ENCODE
 *               - HTTPc_CONN_STATE_REQ_HDR_CONTENT_LEN
 *               - HTTPc_CONN_STATE_REQ_HDR_EXT
 *
 * @note     (4) The data length of each state MUST be smaller than the configured HTTPc buffer length.
 *               Else the process is aborted and the err code HTTPc_ERR_TRANS_BUF_LEN_INVALID is returned.
 *               The buffer length MUST be reconfigured and increased.
 *
 * @note     (5) The function will not return until all the request headers has been copied to the HTTP
 *               buffer or if the buffer is full before the end of the header processing. In that case,
 *               the state is saved and the function will exit to allow the data to be transmit.
 *               Next time around the process will resume at the saved state.
 *******************************************************************************************************/
static void HTTPcReq_Hdr(HTTPc_CONN *p_conn,
                         RTOS_ERR   *p_err)
{
  HTTPc_REQ    *p_req;
  HTTP_DICT    *p_entry;
  CPU_CHAR     *p_str;
  CPU_CHAR     *p_buf;
  CPU_CHAR     *p_buf_wr;
  CPU_CHAR     content_len_str[DEF_INT_32U_NBR_DIG_MAX + 1];
  CPU_SIZE_T   buf_len;
  CPU_SIZE_T   len;
  CPU_BOOLEAN  flag_en;
  CPU_INT32U   key;
  HTTPc_STATUS rtn_status = HTTPc_STATUS_NONE;
#ifdef  HTTPc_WEBSOCK_MODULE_EN
  HTTPc_WEBSOCK_REQ *p_ws_req;
#endif

  p_req = p_conn->ReqListHeadPtr;

  p_buf = p_conn->TxBufPtr + p_conn->TxDataLen;
  buf_len = p_conn->BufLen   - p_conn->TxDataLen;
  p_buf_wr = p_buf;

  //                                                               --------------- ADD REQUIRED HEADERS ---------------
  switch (p_conn->State) {
    case HTTPc_CONN_STATE_REQ_HDR_HOST:
      //                                                           --------------- COPY HOST HDR FIELD ----------------
      flag_en = DEF_BIT_IS_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_HOST_ADD);
      if (flag_en == DEF_YES) {
        if ((p_conn->ServerPort == HTTP_DFLT_PORT_NBR)
            || (p_conn->ServerPort == HTTP_DFLT_PORT_NBR_SECURE)) {
          p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                        p_conn->BufLen,
                                        buf_len,
                                        HTTP_HDR_FIELD_HOST,
                                        p_conn->HostNamePtr,
                                        p_conn->HostNameLen,
                                        DEF_YES,
                                        &rtn_status,
                                        p_err);
          if (p_str == DEF_NULL) {
            goto exit;
          }

          buf_len -= (p_str - p_buf_wr);
          p_buf_wr = p_str;
        } else {
          //                                                       If not http dflt port, add the port nbr to host hdr.
          p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                        p_conn->BufLen,
                                        buf_len,
                                        HTTP_HDR_FIELD_HOST,
                                        p_conn->HostNamePtr,
                                        p_conn->HostNameLen,
                                        DEF_NO,
                                        &rtn_status,
                                        p_err);
          if (p_str == DEF_NULL) {
            goto exit;
          }

          buf_len -= (p_str - p_buf_wr);
          p_buf_wr = p_str;

          if (buf_len >= 1) {
            *p_str = ASCII_CHAR_COLON;
            p_str++;
          } else {
            RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
          }

          buf_len -= (p_str - p_buf_wr);
          p_buf_wr = p_str;

          p_str = Str_FmtNbr_Int32U(p_conn->ServerPort,
                                    5u,
                                    DEF_NBR_BASE_DEC,
                                    DEF_NULL,
                                    DEF_NO,
                                    DEF_YES,
                                    p_buf_wr);
          RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

          p_str += Str_Len(p_str);

          buf_len -= (p_str - p_buf_wr);
          p_buf_wr = p_str;

          p_str = Str_Copy_N(p_str, STR_CR_LF, buf_len);
          RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

          p_str += STR_CR_LF_LEN;
          buf_len -= p_str - p_buf_wr;
          p_buf_wr = p_str;
        }
      }
      p_conn->State = HTTPc_CONN_STATE_REQ_HDR_CONN;
    //                                                             'break' intentionally omitted.
    //                                                             fallthrough

    case HTTPc_CONN_STATE_REQ_HDR_CONN:
      //                                                           ------------- COPY CONNECTION HDR FIELD -------------
      key = HTTP_DICT_KEY_INVALID;
      flag_en = DEF_BIT_IS_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_CONNECTION_ADD);
      if (flag_en == DEF_YES) {
        //                                                         Check the value for the connection hdr field.
        flag_en = DEF_BIT_IS_SET(p_conn->Flags, HTTPc_FLAG_CONN_TO_CLOSE);
        if (flag_en == DEF_YES) {
          key = HTTP_HDR_FIELD_CONN_CLOSE;
        } else {
#ifdef  HTTPc_WEBSOCK_MODULE_EN
          flag_en = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_UPGRADE_WEBSOCKET);
          if (flag_en == DEF_YES) {
            key = HTTP_HDR_FIELD_CONN_UPGRADE;
          }
#endif
        }
        p_entry = HTTP_Dict_EntryGet(HTTP_Dict_HdrFieldConnVal,
                                     HTTP_Dict_HdrFieldConnValSize,
                                     key);
        RTOS_ASSERT_CRITICAL((p_entry != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                      p_conn->BufLen,
                                      buf_len,
                                      HTTP_HDR_FIELD_CONN,
                                      p_entry->StrPtr,
                                      p_entry->StrLen,
                                      DEF_YES,
                                      &rtn_status,
                                      p_err);
        if (p_str == DEF_NULL) {
          goto exit;
        }

        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;
      }

      p_conn->State = HTTPc_CONN_STATE_REQ_HDR_CONTENT_TYPE;
    //                                                             'break' intentionally omitted.
    //                                                             fallthrough

    case HTTPc_CONN_STATE_REQ_HDR_CONTENT_TYPE:
      //                                                           ----------- COPY CONTENT-TYPE HDR FIELD ------------
      flag_en = DEF_BIT_IS_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_CONTENT_TYPE_ADD);
      if (flag_en == DEF_YES) {
        p_entry = HTTP_Dict_EntryGet(HTTP_Dict_ContentType,
                                     HTTP_Dict_ContentTypeSize,
                                     p_req->ContentType);
        RTOS_ASSERT_CRITICAL((p_entry != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        if (p_req->ContentType != HTTP_CONTENT_TYPE_MULTIPART_FORM) {
          p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                        p_conn->BufLen,
                                        buf_len,
                                        HTTP_HDR_FIELD_CONTENT_TYPE,
                                        p_entry->StrPtr,
                                        p_entry->StrLen,
                                        DEF_YES,
                                        &rtn_status,
                                        p_err);
          if (p_str == DEF_NULL) {
            goto exit;
          }
        } else {
          len = HTTP_STR_HDR_FIELD_CONTENT_TYPE_LEN
                + 2
                + p_entry->StrLen
                + 2
                + HTTP_STR_MULTIPART_BOUNDARY_LEN
                + 1
                + HTTPc_STR_BOUNDARY_LEN
                + STR_CR_LF_LEN;

          RTOS_ASSERT_DBG_ERR_SET((len <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

          if (len > buf_len) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                    // Exit to transmit data in buffer.
            goto exit;
          }

          p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                        p_conn->BufLen,
                                        buf_len,
                                        HTTP_HDR_FIELD_CONTENT_TYPE,
                                        p_entry->StrPtr,
                                        p_entry->StrLen,
                                        DEF_NO,
                                        &rtn_status,
                                        p_err);
          if (p_str == DEF_NULL) {
            goto exit;
          }

          buf_len -= (p_str - p_buf_wr);
          p_buf_wr = p_str;

          *p_str = ASCII_CHAR_SEMICOLON;
          p_str++;
          *p_str = ASCII_CHAR_SPACE;
          p_str++;

          p_str = Str_Copy_N(p_str, HTTP_STR_MULTIPART_BOUNDARY, HTTP_STR_MULTIPART_BOUNDARY_LEN);
          RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

          p_str += HTTP_STR_MULTIPART_BOUNDARY_LEN;
          *p_str = ASCII_CHAR_EQUALS_SIGN;
          p_str++;

          p_str = Str_Copy_N(p_str, HTTPc_STR_BOUNDARY, HTTPc_STR_BOUNDARY_LEN);
          RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

          p_str += HTTPc_STR_BOUNDARY_LEN;

          p_str = Str_Copy_N(p_str, STR_CR_LF, STR_CR_LF_LEN);
          RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

          p_str += STR_CR_LF_LEN;
        }

        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;
      }
      p_conn->State = HTTPc_CONN_STATE_REQ_HDR_TRANSFER_ENCODE;
    //                                                             'break' intentionally omitted.
    //                                                             fallthrough

    case HTTPc_CONN_STATE_REQ_HDR_TRANSFER_ENCODE:
      //                                                           --------- COPY TRANSFER-ENCODING HDR FIELD ---------
      flag_en = DEF_BIT_IS_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_TRANSFER_ENCODE_ADD);
      if (flag_en == DEF_YES) {
        p_entry = HTTP_Dict_EntryGet(HTTP_Dict_HdrFieldTransferTypeVal,
                                     HTTP_Dict_HdrFieldTransferTypeValSize,
                                     HTTP_HDR_FIELD_TRANSFER_TYPE_CHUNCKED);
        RTOS_ASSERT_CRITICAL((p_entry != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                      p_conn->BufLen,
                                      buf_len,
                                      HTTP_HDR_FIELD_TRANSFER_ENCODING,
                                      p_entry->StrPtr,
                                      p_entry->StrLen,
                                      DEF_YES,
                                      &rtn_status,
                                      p_err);
        if (p_str == DEF_NULL) {
          goto exit;
        }

        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;
      }
      p_conn->State = HTTPc_CONN_STATE_REQ_HDR_CONTENT_LEN;
    //                                                             'break' intentionally omitted.
    //                                                             fallthrough

    case HTTPc_CONN_STATE_REQ_HDR_CONTENT_LEN:
      //                                                           ---------- COPY CONTENT-LENGTH HDR FIELD -----------
      flag_en = DEF_BIT_IS_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_CONTENT_LENGTH_ADD);
      if (flag_en == DEF_YES) {
        p_str = Str_FmtNbr_Int32U(p_req->ContentLen,
                                  DEF_INT_32U_NBR_DIG_MAX,
                                  DEF_NBR_BASE_DEC,
                                  '\0',
                                  DEF_NO,
                                  DEF_YES,
                                  content_len_str);
        RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        len = Str_Len_N(p_str, (DEF_INT_32U_NBR_DIG_MAX + 1));

        p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                      p_conn->BufLen,
                                      buf_len,
                                      HTTP_HDR_FIELD_CONTENT_LEN,
                                      p_str,
                                      len,
                                      DEF_YES,
                                      &rtn_status,
                                      p_err);
        if (p_str == DEF_NULL) {
          goto exit;
        }

        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;
      }
      p_conn->State = HTTPc_CONN_STATE_REQ_HDR_UPGRADE;
    //                                                             'break' intentionally omitted.
    //                                                             fallthrough

    case HTTPc_CONN_STATE_REQ_HDR_UPGRADE:

#ifdef  HTTPc_WEBSOCK_MODULE_EN
      flag_en = DEF_BIT_IS_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_UPGRADE_ADD);
      if (flag_en == DEF_YES) {
        p_entry = HTTP_Dict_EntryGet(HTTP_Dict_HdrFieldUpgradeVal,
                                     HTTP_Dict_HdrFieldUpgradeValSize,
                                     HTTP_HDR_FIELD_UPGRADE_WEBSOCKET);
        RTOS_ASSERT_CRITICAL((p_entry != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                      p_conn->BufLen,
                                      buf_len,
                                      HTTP_HDR_FIELD_UPGRADE,
                                      p_entry->StrPtr,
                                      p_entry->StrLen,
                                      DEF_YES,
                                      &rtn_status,
                                      p_err);
        if (p_str == DEF_NULL) {
          goto exit;
        }

        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;
      }
#endif
      p_conn->State = HTTPc_CONN_STATE_REQ_HDR_WEBSOCKET;
    //                                                             'break' intentionally omitted.
    //                                                             fallthrough

    case HTTPc_CONN_STATE_REQ_HDR_WEBSOCKET:
#ifdef  HTTPc_WEBSOCK_MODULE_EN
      flag_en = DEF_BIT_IS_SET(p_req->HdrFlags, HTTPc_FLAG_REQ_HDR_UPGRADE_ADD);
      if (flag_en == DEF_YES) {
        p_ws_req = p_req->WebSockPtr;

        p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                      p_conn->BufLen,
                                      buf_len,
                                      HTTP_HDR_FIELD_WEBSOCKET_KEY,
                                      p_ws_req->Key,
                                      HTTPc_WEBSOCK_KEY_ENCODED_LEN - 1,
                                      DEF_YES,
                                      &rtn_status,
                                      p_err);
        if (p_str == DEF_NULL) {
          goto exit;
        }

        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;

        p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                      p_conn->BufLen,
                                      buf_len,
                                      HTTP_HDR_FIELD_WEBSOCKET_VERSION,
                                      HTTPc_WEBSOCK_PROTOCOL_VERSION_13_STR,
                                      HTTPc_WEBSOCK_PROTOCOL_VERSION_13_STR_LEN,
                                      DEF_YES,
                                      &rtn_status,
                                      p_err);
        if (p_str == DEF_NULL) {
          goto exit;
        }

        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;
      }
#endif
      p_conn->State = HTTPc_CONN_STATE_REQ_HDR_EXT;
    //                                                             'break' intentionally omitted.
    //                                                             fallthrough

    case HTTPc_CONN_STATE_REQ_HDR_EXT:
      //                                                           -------------- COPY EXTRA HDR FIELDS ---------------
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
      if (p_req->HdrTbl != DEF_NULL) {
        p_buf_wr = HTTPcReq_HdrExtTbl(p_conn, p_buf_wr, buf_len, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
      }
      if (p_req->OnHdrTx != DEF_NULL) {
        p_buf_wr = HTTPcReq_HdrExtHook(p_conn, p_buf_wr, buf_len, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
      }
#endif
      p_conn->State = HTTPc_CONN_STATE_REQ_HDR_LAST;
    //                                                             'break' intentionally omitted.
    //                                                             fallthrough

    case HTTPc_CONN_STATE_REQ_HDR_LAST:
      //                                                           ------------- WRITE CRLF AFTER HEADERS -------------
      if (buf_len < STR_CR_LF_LEN) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                        // Exit to transmit data in buffer.
        goto exit;
      }

      p_str = Str_Copy_N(p_buf_wr, STR_CR_LF, buf_len);
      RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

      p_buf_wr += STR_CR_LF_LEN;

      p_conn->State = HTTPc_CONN_STATE_REQ_BODY;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  p_conn->TxDataLen += (p_buf_wr - p_buf);                      // Update Data Length to transmit.
  return;
}

/****************************************************************************************************//**
 *                                         HTTPcReq_HdrExtTbl()
 *
 * @brief    Add Additional headers include in the table to the HTTP Request.
 *
 * @param    p_conn          Pointer to the current HTTPc Connection.
 *
 * @param    p_buf           Pointer to where the data must be copied in the HTTP buffer.
 *
 * @param    buf_len_rem     Buffer length remaining.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to location in buffer after the end of the data written.
 *
 * @note     (1) Since the additional headers are passed by the App in the form of a table, the process will
 *               copy as much as headers possible to the HTTP buffer before returning. If not all the
 *               headers fit inside the buffer, the index of the table will be saved and the function
 *               will exit to allow transmission of data. Next time around the process will resume
 *               at the saved index.
 *
 * @note     (2) Note that each header MUST individually fit inside the HTTP buffer
 *               since a header field is not divisible.
 *******************************************************************************************************/
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
static CPU_CHAR *HTTPcReq_HdrExtTbl(HTTPc_CONN *p_conn,
                                    CPU_CHAR   *p_buf,
                                    CPU_INT16U buf_len_rem,
                                    RTOS_ERR   *p_err)
{
  HTTPc_REQ    *p_req;
  HTTPc_HDR    *p_hdr;
  CPU_CHAR     *p_str;
  CPU_INT08U   ix;
  CPU_INT08U   i;
  HTTPc_STATUS rtn_status = HTTPc_STATUS_NONE;

  p_req = p_conn->ReqListHeadPtr;

  ix = p_conn->ReqHdrTxIx;
  p_hdr = &p_req->HdrTbl[ix];
  for (i = ix; i < p_req->HdrNbr; i++) {
    p_str = HTTPcReq_HdrCopyToBuf(p_buf,
                                  p_conn->BufLen,
                                  buf_len_rem,
                                  p_hdr->HdrField,
                                  p_hdr->ValPtr,
                                  p_hdr->ValLen,
                                  DEF_YES,
                                  &rtn_status,
                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    switch (rtn_status) {
      case HTTPc_STATUS_NONE:
        break;

      case HTTPc_STATUS_NO_MORE_SPACE:
        p_conn->ReqHdrTxIx = i;
        goto exit;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, p_buf);
    }

    buf_len_rem -= (p_str - p_buf);
    p_buf = p_str;

    if (i == (p_req->HdrNbr - 1)) {
      p_conn->ReqHdrTxIx = 0u;
    }

    p_hdr++;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (p_buf);
}
#endif

/****************************************************************************************************//**
 *                                         HTTPcReq_HdrExtHook()
 *
 * @brief    Call request Header hook function to add additional headers to the request.
 *
 * @param    p_conn          Pointer to current HTTPc Connection.
 *
 * @param    p_buf           Pointer to where the data must be copied in the HTTP buffer.
 *
 * @param    buf_len_rem     Buffer length remaining.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to location in buffer after the end of the data written.
 *
 * @note     (1) Since the Additional Headers are passed by the App hook function, the process will
 *               copy as much as headers possible to the HTTP buffer before returning. If not all the
 *               headers fit inside the buffer, a pointer to the header field will be saved and
 *               the function will exit to allow transmission of data. Next time around the process will
 *               resume at the saved pointer before calling the hook.
 *
 * @note     (2) Note that each header MUST individually fit inside the HTTP buffer
 *               since a header field is not divisible.
 *******************************************************************************************************/
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
static CPU_CHAR *HTTPcReq_HdrExtHook(HTTPc_CONN *p_conn,
                                     CPU_CHAR   *p_buf,
                                     CPU_INT16U buf_len_rem,
                                     RTOS_ERR   *p_err)
{
  HTTPc_CONN_OBJ *p_conn_const;
  HTTPc_REQ      *p_req;
  HTTPc_REQ_OBJ  *p_req_const;
  HTTPc_HDR      *p_hdr;
  CPU_CHAR       *p_str;
  CPU_BOOLEAN    done;
  HTTPc_STATUS   rtn_status = HTTPc_STATUS_NONE;

  p_conn_const = (HTTPc_CONN_OBJ *)p_conn;

  p_req = p_conn->ReqListHeadPtr;
  p_req_const = (HTTPc_REQ_OBJ *)p_req;

  done = DEF_NO;
  while (done != DEF_YES) {
    if (p_conn->ReqHdrTempPtr != DEF_NULL) {
      p_hdr = p_conn->ReqHdrTempPtr;
      done = DEF_BIT_IS_SET(p_conn->ReqFlags, HTTPc_FLAG_REQ_HDR_DONE);
      p_conn->ReqHdrTempPtr = DEF_NULL;
    } else {
      done = p_req->OnHdrTx(p_conn_const, p_req_const, &p_hdr);
      RTOS_ASSERT_DBG_ERR_SET((p_hdr != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);
    }

    p_str = HTTPcReq_HdrCopyToBuf(p_buf,
                                  p_conn->BufLen,
                                  buf_len_rem,
                                  p_hdr->HdrField,
                                  p_hdr->ValPtr,
                                  p_hdr->ValLen,
                                  DEF_YES,
                                  &rtn_status,
                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    switch (rtn_status) {
      case HTTPc_STATUS_NONE:
        break;

      case HTTPc_STATUS_NO_MORE_SPACE:
        p_conn->ReqHdrTempPtr = p_hdr;
        if (done == DEF_YES) {
          DEF_BIT_SET(p_conn->ReqFlags, HTTPc_FLAG_REQ_HDR_DONE);
        }
        goto exit;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, p_buf);
    }

    buf_len_rem -= (p_str - p_buf);
    p_buf = p_str;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (p_buf);
}
#endif

/****************************************************************************************************//**
 *                                         HTTPcReq_HdrSearch()
 *
 * @brief    Search for a HTTP header type inside the extra header table passed by the upper application.
 *
 * @param    p_conn      Pointer to current HTTPc Connection.
 *
 * @param    hdr_field   HTTP header type to search for in table.
 *
 * @return   Pointer to HTTP header object found.
 *           DEF_NULL if no HTTP header found.
 *******************************************************************************************************/
#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
static HTTPc_HDR *HTTPcReq_HdrSearch(HTTPc_CONN     *p_conn,
                                     HTTP_HDR_FIELD hdr_field)
{
  HTTPc_REQ   *p_req;
  HTTPc_HDR   *p_hdr;
  CPU_BOOLEAN found;
  CPU_INT08U  i;

  p_req = p_conn->ReqListHeadPtr;
  p_hdr = p_req->HdrTbl;

  if (p_hdr == DEF_NULL) {
    return (DEF_NULL);
  }

  found = DEF_NO;
  for (i = 0u; i < p_req->HdrNbr; i++) {
    if (p_hdr->HdrField == hdr_field) {
      found = DEF_YES;
      break;
    }

    p_hdr++;
  }

  if (found == DEF_YES) {
    return (p_hdr);
  } else {
    return (DEF_NULL);
  }
}
#endif

/****************************************************************************************************//**
 *                                          HTTPcReq_Body()
 *
 * @brief    Prepare and Copy/Transmit HTTP Request Body.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void HTTPcReq_Body(HTTPc_CONN *p_conn,
                          RTOS_ERR   *p_err)
{
  HTTPc_REQ         *p_req;
  HTTP_METHOD       method;
  HTTP_CONTENT_TYPE content_type;
  CPU_BOOLEAN       body_present;

  p_req = p_conn->ReqListHeadPtr;
  method = p_req->Method;
  content_type = p_req->ContentType;

  body_present = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_BODY_PRESENT);

  switch (p_conn->State) {
    case HTTPc_CONN_STATE_REQ_BODY:
      switch (method) {
        case HTTP_METHOD_GET:
        case HTTP_METHOD_HEAD:
        case HTTP_METHOD_DELETE:
        case HTTP_METHOD_OPTIONS:
          if (body_present == DEF_NO) {
            p_conn->State = HTTPc_CONN_STATE_REQ_END;
          } else {
            p_conn->State = HTTPc_CONN_STATE_REQ_BODY_DATA;
          }
          break;

        case HTTP_METHOD_TRACE:
        case HTTP_METHOD_CONNECT:
          p_conn->State = HTTPc_CONN_STATE_REQ_END;
          break;

        case HTTP_METHOD_POST:
          if (body_present == DEF_NO) {
            p_conn->State = HTTPc_CONN_STATE_REQ_END;
            break;
          }
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
          if (p_req->FormFieldTbl != DEF_NULL) {
            switch (content_type) {
              case HTTP_CONTENT_TYPE_APP_FORM:
                p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_APP;
                break;

              case HTTP_CONTENT_TYPE_MULTIPART_FORM:
                p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY;
                break;

              default:
                RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
            }
          } else {
            p_conn->State = HTTPc_CONN_STATE_REQ_BODY_DATA;
          }
#else
          PP_UNUSED_PARAM(content_type);
          p_conn->State = HTTPc_CONN_STATE_REQ_BODY_DATA;
#endif
          break;

        case HTTP_METHOD_PUT:
          if (body_present == DEF_NO) {
            p_conn->State = HTTPc_CONN_STATE_REQ_END;
            break;
          }
          p_conn->State = HTTPc_CONN_STATE_REQ_BODY_DATA;
          break;

        case HTTP_METHOD_UNKNOWN:
        default:
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
      }
      break;

    case HTTPc_CONN_STATE_REQ_BODY_DATA:
    case HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_SIZE:
    case HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_DATA:
    case HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_END:
      HTTPcReq_BodyData(p_conn, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
    case HTTPc_CONN_STATE_REQ_BODY_FORM_APP:
      HTTPcReq_FormApp(p_conn, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      p_conn->State = HTTPc_CONN_STATE_REQ_END;
      break;
#endif

#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
    case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY:
    case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_HDR_CONTENT_DISPO:
    case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_HDR_CONTENT_TYPE:
    case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA:
    case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA_END:
    case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY_END:
      HTTPcReq_FormMultipart(p_conn, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;
}

/****************************************************************************************************//**
 *                                          HTTPcReq_FormApp()
 *
 * @brief    Prepare the application type form and copy it to HTTP buffer.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) The application-form is created with the table form passed by the upper application.
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
static void HTTPcReq_FormApp(HTTPc_CONN *p_conn,
                             RTOS_ERR   *p_err)
{
  HTTPc_REQ            *p_req;
  CPU_CHAR             *p_buf;
  CPU_CHAR             *p_buf_wr;
  HTTPc_FORM_TBL_FIELD *p_form_tbl;
  HTTPc_FORM_TBL_FIELD *p_tbl_entry;
  HTTPc_KEY_VAL        *p_form_field;
  CPU_INT16U           key_char_encode_nbr;
  CPU_INT16U           val_char_encode_nbr;
  CPU_SIZE_T           buf_len;
  CPU_SIZE_T           str_len_key;
  CPU_SIZE_T           str_len_val;
  CPU_SIZE_T           data_size = 0;
  CPU_BOOLEAN          url_encode;
  CPU_INT16U           i;

  p_req = p_conn->ReqListHeadPtr;

  p_buf = p_conn->TxBufPtr + p_conn->TxDataLen;                 // Point to beginning of free space in Tx buffer.
  buf_len = p_conn->BufLen   - p_conn->TxDataLen;
  p_buf_wr = p_buf;

  p_form_tbl = (HTTPc_FORM_TBL_FIELD *)p_req->FormFieldTbl;

  for (i = p_conn->ReqFormDataTxIx; i < p_req->FormFieldNbr; i++) {
    p_tbl_entry = &p_form_tbl[i];

    p_form_field = (HTTPc_KEY_VAL *)p_tbl_entry->FieldObjPtr;

    //                                                             Calculate length of key and value without encoding.
    str_len_key = p_form_field->KeyLen;
    str_len_val = p_form_field->ValLen;

    //                                                             Found number of character needing URL encoding.
    key_char_encode_nbr = HTTP_URL_CharEncodeNbr(p_form_field->KeyPtr, str_len_key);
    val_char_encode_nbr = HTTP_URL_CharEncodeNbr(p_form_field->ValPtr, str_len_val);

    //                                                             Calculate total size needed for key & value encoded.
    data_size = str_len_key
                + str_len_val
                + HTTP_URL_ENCODING_JUMP * key_char_encode_nbr
                + HTTP_URL_ENCODING_JUMP * val_char_encode_nbr
                + 1;                                            // + 1 for the "=" between key & value.

    if (i < (p_req->FormFieldNbr - 1)) {
      data_size++;                                              // + 1 for the "&" between each key&value pair.
    }

    //                                                             Return if error if conn buf is to small for 1 pair.
    RTOS_ASSERT_DBG_ERR_SET((data_size <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

    if (data_size > buf_len) {                                  // Return if no more space in buf. Need to Tx.
      p_conn->ReqFormDataTxIx = i;                              // Save the tbl index to start from there next time.
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                          // Exit to transmit data in buffer.
      goto exit;
    }

    url_encode = HTTP_URL_EncodeStr(p_form_field->KeyPtr,       // Encode and Write to buffer the Key.
                                    p_buf_wr,
                                    &str_len_key,
                                    buf_len);
    RTOS_ASSERT_CRITICAL((url_encode == DEF_OK), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    p_buf_wr += str_len_key;
    buf_len -= str_len_key;

    *p_buf_wr = ASCII_CHAR_EQUALS_SIGN;                         // Write the "=" sign.
    p_buf_wr++;
    buf_len--;

    url_encode = HTTP_URL_EncodeStr(p_form_field->ValPtr,       // Encode and Write to buffer the Value.
                                    p_buf_wr,
                                    &str_len_val,
                                    buf_len);
    RTOS_ASSERT_CRITICAL((url_encode == DEF_OK), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    p_buf_wr += str_len_val;
    buf_len -= str_len_val;

    if (i < (p_req->FormFieldNbr - 1)) {
      *p_buf_wr = ASCII_CHAR_AMPERSAND;                         // Write the "&" sign between pairs.
      p_buf_wr++;
      buf_len--;
    } else {
      p_conn->ReqFormDataTxIx = 0;
    }

    p_form_field++;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  p_conn->TxDataLen += (p_buf_wr - p_buf);                      // Update Data Length to transmit.
  return;
}
#endif

/****************************************************************************************************//**
 *                                      HTTPcReq_FormMultipart()
 *
 * @brief    Prepare the multipart type form and copy it to HTTP buffer.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note         (1) Do not support Compression, Encryption and Encoding inside multipart.
 *                   Only header Content-Disposition will be added with parameter "name". For file
 *                   upload, parameter "filename" will be added also.
 *                   Header Content-Type will be added if content-type passed is recognized by client.
 *
 * @note         (2) Only support 1 level of multipart.
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
static void HTTPcReq_FormMultipart(HTTPc_CONN *p_conn,
                                   RTOS_ERR   *p_err)
{
  HTTPc_CONN_OBJ       *p_conn_const;
  HTTPc_REQ            *p_req;
  HTTPc_REQ_OBJ        *p_req_const;
  CPU_CHAR             *p_buf;
  CPU_CHAR             *p_buf_wr;
  CPU_CHAR             *p_str = DEF_NULL;
  CPU_CHAR             *p_name = DEF_NULL;
  CPU_CHAR             *p_filename = DEF_NULL;
  HTTPc_FORM_TBL_FIELD *p_form_tbl;
  HTTPc_FORM_TBL_FIELD *p_tbl_entry;
  HTTPc_KEY_VAL        *p_kvp;
  HTTPc_KEY_VAL_EXT    *p_kvp_big;
  HTTPc_MULTIPART_FILE *p_multipart_file = DEF_NULL;
  HTTP_DICT            *p_entry;
  CPU_SIZE_T           name_field_len;
  CPU_SIZE_T           filename_field_len;
  CPU_SIZE_T           buf_len;
  CPU_SIZE_T           data_size;
  CPU_INT16U           data_len_wr;
  CPU_INT16U           name_char_encode_nbr = 0u;
  CPU_INT16U           filename_char_encode_nbr;
  CPU_INT16U           i;
  CPU_BOOLEAN          url_encode;
  CPU_BOOLEAN          done;
  HTTPc_STATUS         rtn_status = HTTPc_STATUS_NONE;

  p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
  p_req = p_conn->ReqListHeadPtr;
  p_req_const = (HTTPc_REQ_OBJ *)p_req;

  p_buf = p_conn->TxBufPtr + p_conn->TxDataLen;                 // Point to beginning of free space in Tx buffer.
  buf_len = p_conn->BufLen   - p_conn->TxDataLen;
  p_buf_wr = p_buf;
  data_size = 0;

  p_form_tbl = p_req->FormFieldTbl;
  for (i = p_conn->ReqFormDataTxIx; i < p_req->FormFieldNbr; i++) {
    p_tbl_entry = &p_form_tbl[i];

    switch (p_conn->State) {
      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY:
        //                                                         Calculate size of needed for data to write.
        data_size = HTTPc_STR_BOUNDARY_START_LEN + STR_CR_LF_LEN;

        //                                                         Validate that buffer size is enough big.
        RTOS_ASSERT_DBG_ERR_SET((data_size <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

        if (data_size > buf_len) {                                 // Validate that remaining space in buffer is enough.
          p_conn->TxDataLen += (p_buf_wr - p_buf);                 // Update Data Length to transmit.
          p_conn->ReqFormDataTxIx = i;                             // Save the tbl index to start from there next time.
          p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                      // Exit to transmit data in buffer.
          goto exit;
        }

        //                                                         Write start of boundary.
        p_str = Str_Copy_N(p_buf_wr, HTTPc_STR_BOUNDARY_START, data_size);
        RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_str += HTTPc_STR_BOUNDARY_START_LEN;

        //                                                         Write CRLF after boundary.
        p_str = Str_Copy_N(p_str, STR_CR_LF, STR_CR_LF_LEN);
        RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_str += STR_CR_LF_LEN;
        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;
      //                                                           'break' intentionally omitted.
      //                                                           fallthrough

      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_HDR_CONTENT_DISPO:
        switch (p_tbl_entry->Type) {
          case HTTPc_FORM_FIELD_TYPE_KEY_VAL:
            p_kvp = (HTTPc_KEY_VAL *)p_tbl_entry->FieldObjPtr;
            //                                                     Found nbr of chars to encode in name.
            name_field_len = p_kvp->KeyLen;
            name_char_encode_nbr = HTTP_URL_CharEncodeNbr(p_kvp->KeyPtr, name_field_len);
            p_name = p_kvp->KeyPtr;
            p_filename = DEF_NULL;
            break;

          case HTTPc_FORM_FIELD_TYPE_KEY_VAL_EXT:
            p_kvp_big = (HTTPc_KEY_VAL_EXT *)p_tbl_entry->FieldObjPtr;
            //                                                     Found nbr of chars to encode in name.
            name_field_len = p_kvp_big->KeyLen;
            name_char_encode_nbr = HTTP_URL_CharEncodeNbr(p_kvp_big->KeyPtr, name_field_len);
            p_name = p_kvp_big->KeyPtr;
            p_filename = DEF_NULL;
            break;

          case HTTPc_FORM_FIELD_TYPE_FILE:
            p_multipart_file = (HTTPc_MULTIPART_FILE *)p_tbl_entry->FieldObjPtr;
            //                                                     Found nbr of chars to encode in name.
            name_field_len = p_multipart_file->NameLen;
            name_char_encode_nbr = HTTP_URL_CharEncodeNbr(p_multipart_file->NamePtr, name_field_len);
            p_name = p_multipart_file->NamePtr;
            //                                                     Found nbr of chars to encode in filename.
            filename_field_len = p_multipart_file->FileNameLen;
            filename_char_encode_nbr = HTTP_URL_CharEncodeNbr(p_multipart_file->FileNamePtr, filename_field_len);
            p_filename = p_multipart_file->FileNamePtr;
            data_size += (2                                                                // "; "
                          + HTTP_STR_MULTIPART_FIELD_FILE_NAME_LEN                         // "filename"
                          + 3                                                              // "=\"\""
                          + filename_field_len                                             // "filename_rx_in_form"
                          + HTTP_URL_ENCODING_JUMP * filename_char_encode_nbr);            // nbr of char to encode.
            break;

          default:
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        }

        //                                                         Calculate size needed for data to write.
        data_size += HTTP_STR_HDR_FIELD_CONTENT_DISPOSITION_LEN                     // "Content-Disposition"
                     + 2                                                            // ": "
                     + HTTP_STR_CONTENT_DISPOSITION_FORM_DATA_LEN                   // "form-data"
                     + 2                                                            // "; "
                     + HTTP_STR_MULTIPART_FIELD_NAME_LEN                            // "name"
                     + 3                                                            // "=\"\""
                     + name_field_len                                               // "name_rx_in_form"
                     + HTTP_URL_ENCODING_JUMP * name_char_encode_nbr                // nbr of char to encode.
                     + STR_CR_LF_LEN;                                               // "\r\n"

        //                                                         Validate that buffer size is enough big.
        RTOS_ASSERT_DBG_ERR_SET((data_size <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

        if (data_size > buf_len) {                              // Validate that remaining space in buffer is enough.
          p_conn->TxDataLen += (p_buf_wr - p_buf);              // Update Data Length to transmit.
          p_conn->ReqFormDataTxIx = i;                          // Save the tbl index to start from there next time.
          p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_HDR_CONTENT_DISPO;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                      // Exit to transmit data in buffer.
          goto exit;
        }
        //                                                         Write Content-Disposition with name (and filename).
        p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                      p_conn->BufLen,
                                      buf_len,
                                      HTTP_HDR_FIELD_CONTENT_DISPOSITION,
                                      HTTP_STR_CONTENT_DISPOSITION_FORM_DATA,
                                      HTTP_STR_CONTENT_DISPOSITION_FORM_DATA_LEN,
                                      DEF_NO,
                                      &rtn_status,
                                      p_err);
        if (p_str == DEF_NULL) {
          goto exit;
        }

        *p_str = ASCII_CHAR_SEMICOLON;
        p_str++;
        *p_str = ASCII_CHAR_SPACE;
        p_str++;

        p_str = Str_Copy_N(p_str, HTTP_STR_MULTIPART_FIELD_NAME, HTTP_STR_MULTIPART_FIELD_NAME_LEN);
        RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_str += HTTP_STR_MULTIPART_FIELD_NAME_LEN;
        *p_str = ASCII_CHAR_EQUALS_SIGN;
        p_str++;
        *p_str = ASCII_CHAR_QUOTATION_MARK;
        p_str++;

        url_encode = HTTP_URL_EncodeStr(p_name,
                                        p_str,
                                        &name_field_len,
                                        buf_len);
        RTOS_ASSERT_CRITICAL((url_encode == DEF_OK), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_str += name_field_len;
        *p_str = ASCII_CHAR_QUOTATION_MARK;
        p_str++;

        if (p_filename != DEF_NULL) {
          *p_str = ASCII_CHAR_SEMICOLON;
          p_str++;
          *p_str = ASCII_CHAR_SPACE;
          p_str++;

          p_str = Str_Copy_N(p_str, HTTP_STR_MULTIPART_FIELD_FILE_NAME, HTTP_STR_MULTIPART_FIELD_FILE_NAME_LEN);
          RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

          p_str += HTTP_STR_MULTIPART_FIELD_FILE_NAME_LEN;
          *p_str = ASCII_CHAR_EQUALS_SIGN;
          p_str++;
          *p_str = ASCII_CHAR_QUOTATION_MARK;
          p_str++;

          url_encode = HTTP_URL_EncodeStr(p_filename,
                                          p_str,
                                          &filename_field_len,
                                          buf_len);
          RTOS_ASSERT_CRITICAL((url_encode == DEF_OK), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

          p_str += filename_field_len;
          *p_str = ASCII_CHAR_QUOTATION_MARK;
          p_str++;
        }

        p_str = Str_Copy_N(p_str, STR_CR_LF, STR_CR_LF_LEN);
        RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_str += STR_CR_LF_LEN;

        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;
      //                                                           'break' intentionally omitted.
      //                                                           fallthrough

      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_HDR_CONTENT_TYPE:
        switch (p_tbl_entry->Type) {
          case HTTPc_FORM_FIELD_TYPE_KEY_VAL:
          case HTTPc_FORM_FIELD_TYPE_KEY_VAL_EXT:
            p_multipart_file = DEF_NULL;
            break;

          case HTTPc_FORM_FIELD_TYPE_FILE:
            p_multipart_file = (HTTPc_MULTIPART_FILE *)p_tbl_entry->FieldObjPtr;
            break;

          default:
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        }

        if (p_multipart_file != DEF_NULL) {
          p_entry = HTTP_Dict_EntryGet(HTTP_Dict_ContentType,
                                       HTTP_Dict_ContentTypeSize,
                                       p_multipart_file->ContentType);
          if (p_entry != DEF_NULL) {
            p_str = HTTPcReq_HdrCopyToBuf(p_buf_wr,
                                          p_conn->BufLen,
                                          buf_len,
                                          HTTP_HDR_FIELD_CONTENT_TYPE,
                                          p_entry->StrPtr,
                                          p_entry->StrLen,
                                          DEF_YES,
                                          &rtn_status,
                                          p_err);
            if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
              goto exit;
            }

            switch (rtn_status) {
              case HTTPc_STATUS_NONE:
                break;

              case HTTPc_STATUS_NO_MORE_SPACE:
                //                                                 Update Data Length to transmit.
                p_conn->TxDataLen += (p_buf_wr - p_buf);
                p_conn->ReqFormDataTxIx = i;                    // Save the tbl index to start from there next time.
                p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_HDR_CONTENT_TYPE;
                goto exit;

              default:
                RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
            }
          }
        }

        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;

        //                                                         Validate that buffer size is enough big.
        RTOS_ASSERT_DBG_ERR_SET((STR_CR_LF_LEN <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

        if (STR_CR_LF_LEN > buf_len) {                          // Validate that remaining space in buf is enough.
          p_conn->TxDataLen += (p_buf_wr - p_buf);                   // Update Data Length to transmit.
          p_conn->ReqFormDataTxIx = i;                          // Save the tbl index to start from there next time.
          p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_HDR_CONTENT_TYPE;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                      // Exit to transmit data in buffer.
          goto exit;
        }

        p_str = Str_Copy_N(p_str, STR_CR_LF, STR_CR_LF_LEN);
        RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_str += STR_CR_LF_LEN;
        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;
      //                                                           'break' intentionally omitted.
      //                                                           fallthrough

      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA:
        switch (p_tbl_entry->Type) {
          case HTTPc_FORM_FIELD_TYPE_KEY_VAL:
            p_kvp = (HTTPc_KEY_VAL *)p_tbl_entry->FieldObjPtr;

            //                                                     Validate that buffer size is enough big.
            RTOS_ASSERT_DBG_ERR_SET((p_kvp->ValLen <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

            if (p_kvp->ValLen > buf_len) {                                  // Validate that remaining space in buf is enough.
              p_conn->TxDataLen += (p_buf_wr - p_buf);                      // Update Data Length to transmit.
              p_conn->ReqFormDataTxIx = i;                                  // Save the tbl index to start from there next time.
              p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA;
              RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                  // Exit to transmit data in buffer.
              goto exit;
            }

            Mem_Copy(p_buf_wr, p_kvp->ValPtr, p_kvp->ValLen);

            buf_len -= p_kvp->ValLen;;
            p_buf_wr += p_kvp->ValLen;
            break;

          case HTTPc_FORM_FIELD_TYPE_KEY_VAL_EXT:
            p_kvp_big = (HTTPc_KEY_VAL_EXT *)p_tbl_entry->FieldObjPtr;

            done = p_kvp_big->OnValTx(p_conn_const,
                                      p_req_const,
                                      p_kvp_big,
                                      p_buf_wr,
                                      buf_len,
                                      &data_len_wr);

            p_conn->ReqDataOffset += data_len_wr;

            if (data_len_wr > buf_len) {
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
            }

            if (p_conn->ReqDataOffset > p_kvp_big->ValLen) {
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
            }

            if ((done == DEF_YES)
                && (p_conn->ReqDataOffset < p_kvp_big->ValLen)) {
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
            }

            buf_len -= data_len_wr;
            p_buf_wr += data_len_wr;

            if (done == DEF_NO) {
              p_conn->TxDataLen += (p_buf_wr - p_buf);
              p_conn->ReqFormDataTxIx = i;                                  // Save the tbl index to start from there next time.
              p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA;
              RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                              // Exit to transmit data in buffer.
              goto exit;
            } else {
              p_conn->ReqDataOffset = 0;
            }
            break;

          case HTTPc_FORM_FIELD_TYPE_FILE:
            p_multipart_file = (HTTPc_MULTIPART_FILE *)p_tbl_entry->FieldObjPtr;

            done = p_multipart_file->OnFileTx(p_conn_const,
                                              p_req_const,
                                              p_multipart_file,
                                              p_buf_wr,
                                              buf_len,
                                              &data_len_wr);

            p_conn->ReqDataOffset += data_len_wr;

            if (data_len_wr > buf_len) {
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
            }

            if (p_conn->ReqDataOffset > p_multipart_file->FileLen) {
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
            }

            if ((done == DEF_YES)
                && (p_conn->ReqDataOffset < p_multipart_file->FileLen)) {
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
            }

            buf_len -= data_len_wr;
            p_buf_wr += data_len_wr;

            if (done == DEF_NO) {
              p_conn->TxDataLen += (p_buf_wr - p_buf);
              p_conn->ReqFormDataTxIx = i;                      // Save the tbl index to start from there next time.
              p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA;
              RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                  // Exit to transmit data in buffer.
              goto exit;
            } else {
              p_conn->ReqDataOffset = 0;
            }
            break;

          default:
            goto exit_critical;
        }
      //                                                           'break' intentionally omitted.
      //                                                           fallthrough

      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA_END:

        p_conn->TxDataPtr = p_conn->TxBufPtr;                   // Reset TxDataPtr to the HTTP buffer pointer.

        //                                                         Validate that buffer size is enough big.
        RTOS_ASSERT_DBG_ERR_SET((STR_CR_LF_LEN <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

        if (STR_CR_LF_LEN > buf_len) {                          // Validate that remaining space in buffer is enough.
          p_conn->TxDataLen += (p_buf_wr - p_buf);              // Update Data Length to transmit.
          p_conn->ReqFormDataTxIx = i;                          // Save the tbl index to start from there next time.
          p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA_END;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                      // Exit to transmit data in buffer.
          goto exit;
        }

        p_str = Str_Copy_N(p_buf_wr, STR_CR_LF, STR_CR_LF_LEN);
        RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_str += STR_CR_LF_LEN;
        buf_len -= (p_str - p_buf_wr);
        p_buf_wr = p_str;
      //                                                           'break' intentionally omitted.
      //                                                           fallthrough

      case HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY_END:
        if (i == (p_req->FormFieldNbr - 1)) {
          //                                                       ----------- WRITE END OF BOUNDARY + CRLF -----------
          //                                                       Calculate size of needed for data to write.
          data_size = Str_Len(HTTPc_STR_BOUNDARY_END) + STR_CR_LF_LEN;

          //                                                       Validate that buffer size is enough big.
          RTOS_ASSERT_DBG_ERR_SET((data_size <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

          if (data_size > buf_len) {
            p_conn->TxDataLen += (p_buf_wr - p_buf);
            p_conn->ReqFormDataTxIx = i;
            p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY_END;
            RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                     // Exit to transmit data in buffer.
            goto exit;
          }

          //                                                       Write end of boundary.
          p_str = Str_Copy_N(p_buf_wr, HTTPc_STR_BOUNDARY_END, HTTPc_STR_BOUNDARY_END_LEN);
          RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

          p_str += HTTPc_STR_BOUNDARY_END_LEN;
          //                                                       Write CRLF after last boundary.
          p_str = Str_Copy_N(p_str, STR_CR_LF, STR_CR_LF_LEN);
          RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

          p_str += STR_CR_LF_LEN;
          buf_len -= (p_str - p_buf_wr);
          p_buf_wr = p_str;
        }
        p_conn->State = HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY;
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
    }
  }

  p_conn->TxDataLen += (p_buf_wr - p_buf);

  p_conn->ReqFormDataTxIx = 0;
  p_conn->State = HTTPc_CONN_STATE_REQ_END;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;

exit_critical:
  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}
#endif

/****************************************************************************************************//**
 *                                        HTTPcReq_BodyData()
 *
 * @brief    Prepare and transmit Data passed by the application.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Data can be send with the chunked transfer-encoding or with header Content-Length
 *               when data length is known.
 *******************************************************************************************************/
static void HTTPcReq_BodyData(HTTPc_CONN *p_conn,
                              RTOS_ERR   *p_err)
{
  HTTPc_REQ      *p_req;
  CPU_SIZE_T     buf_len;
  CPU_BOOLEAN    chunk_en;
  HTTPc_CONN_OBJ *p_conn_const;
  HTTPc_REQ_OBJ  *p_req_const;
  CPU_CHAR       *p_buf;
  CPU_CHAR       *p_buf_wr;
#if (HTTPc_CFG_CHUNK_TX_EN == DEF_ENABLED)
  CPU_SIZE_T  nbr_dig_max;
  CPU_SIZE_T  data_size;
  CPU_CHAR    *p_str;
  CPU_BOOLEAN last_chunk;
#endif

  p_req = p_conn->ReqListHeadPtr;
  p_conn_const = (HTTPc_CONN_OBJ *)p_conn;
  p_req_const = (HTTPc_REQ_OBJ *)p_req;

  p_buf = p_conn->TxBufPtr + p_conn->TxDataLen;                 // Point to beginning of free space in Tx buffer.
  buf_len = p_conn->BufLen - p_conn->TxDataLen;
  p_buf_wr = p_buf;

  chunk_en = DEF_BIT_IS_SET(p_req->Flags, HTTPc_FLAG_REQ_BODY_CHUNK_TRANSFER);

  if ((p_conn->TxDataLen != 0u)
      && (p_req->DataPtr != DEF_NULL)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }

  //                                                               -------------- STANDARD TRANSFER MODE --------------
  if (chunk_en == DEF_NO) {
    //                                                             Call hook fnct to setup data pointer.
    (void)p_req->OnBodyTx(p_conn_const,
                          p_req_const,
                          &p_req->DataPtr,
                          p_buf_wr,
                          buf_len,
                          &p_conn->ReqDataOffset);

    if (p_conn->ReqDataOffset == 0) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit;
    }

    if (p_req->DataPtr == DEF_NULL) {                           // Case 1: Data was copied in Tx buffer.
      if (p_conn->ReqDataOffset > buf_len) {
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
      p_conn->TxDataPtr = p_conn->TxBufPtr;                    // Set TxDataPtr to HTTP buffer ptr.
    } else {                                                    // Case 2: Application Data Pointer was passed.
      p_conn->TxDataPtr = p_req->DataPtr;                       // Set pointer to data to Tx.
    }

    p_conn->TxDataLen += p_conn->ReqDataOffset;                 // Update Data Length to transmit.

    if (p_conn->ReqDataOffset > p_req->ContentLen) {
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    p_req->ContentLen -= p_conn->ReqDataOffset;                 // Calculate size of data remaining to Tx.

    if (p_req->ContentLen <= 0) {                               // Return if all data sent.
      p_conn->State = HTTPc_CONN_STATE_REQ_END;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit;
    }

    //                                                             --------------- CHUNK TRANSFER MODE ----------------
  } else {
#if (HTTPc_CFG_CHUNK_TX_EN == DEF_ENABLED)

    switch (p_conn->State) {
      case HTTPc_CONN_STATE_REQ_BODY_DATA:
        nbr_dig_max = HTTP_StrSizeHexDigReq(p_conn->BufLen);
        data_size = nbr_dig_max + STR_CR_LF_LEN;

        RTOS_ASSERT_DBG_ERR_SET((data_size < p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

        if (data_size >= buf_len) {                              // Exit to transmit data in buffer.
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          goto exit;
        }

        p_buf_wr += data_size;
        buf_len -= data_size;

        //                                                         Call hook fnct to setup data pointer.
        last_chunk = p_req->OnBodyTx(p_conn_const,
                                     p_req_const,
                                     &p_req->DataPtr,
                                     p_buf_wr,
                                     buf_len,
                                     (CPU_INT16U *)&p_req->ContentLen); // Use ContentLen to store length of chunk.

        if ((p_req->ContentLen == 0)
            && (last_chunk == DEF_NO)) {
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        }

        if (p_req->DataPtr == DEF_NULL) {                       // Case 1: Data was copied in Tx buffer.
          if (p_req->ContentLen > buf_len) {
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
          }

          p_conn->TxDataPtr = p_conn->TxBufPtr;                 // Set TxDataPtr to HTTP buffer ptr.
        } else {                                                // Case 2: Application data pointer was passed.
          p_conn->TxDataPtr = p_req->DataPtr;
        }

        if (last_chunk == DEF_YES) {
          DEF_BIT_SET(p_conn->ReqFlags, HTTPc_FLAG_REQ_BODY_CHUNK_LAST);
        } else {
          DEF_BIT_CLR(p_conn->ReqFlags, (HTTPc_FLAGS)HTTPc_FLAG_REQ_BODY_CHUNK_LAST);
        }

        p_conn->State = HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_SIZE;
      //                                                           'break' intentionally omitted.
      //                                                           fallthrough

      case HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_SIZE:
        nbr_dig_max = HTTP_StrSizeHexDigReq(p_conn->BufLen);
        data_size = nbr_dig_max + STR_CR_LF_LEN;
        p_buf_wr -= data_size;
        buf_len = data_size;

        p_str = HTTP_ChunkTransferWrSize(p_buf_wr, buf_len, nbr_dig_max, p_req->ContentLen);
        RTOS_ASSERT_DBG_ERR_SET((p_str != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG,; );

        //                                                         Update Data Length to transmit.
        if (p_conn->TxDataPtr == p_conn->TxBufPtr) {
          p_conn->TxDataLen += (p_str - p_buf_wr) + p_req->ContentLen;
          p_conn->State = HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_END;
        } else {
          p_conn->TxDataLen += (p_str - p_buf_wr);
          p_conn->State = HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_DATA;
        }

        p_conn->TxDataPtr = p_conn->TxBufPtr;                   // Set TxDataPtr to HTTP buffer ptr.
        break;

      case HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_DATA:
        p_conn->TxDataLen += p_req->ContentLen;                 // Update Data Length to transmit.
        p_conn->TxDataPtr = p_req->DataPtr;                     // Set TxDataPtr to App data ptr.
        p_conn->State = HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_END;
        break;

      case HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_END:
        nbr_dig_max = HTTP_StrSizeHexDigReq(p_conn->BufLen);
        data_size = nbr_dig_max + 3 * STR_CR_LF_LEN;

        RTOS_ASSERT_DBG_ERR_SET((data_size <= p_conn->BufLen), *p_err, RTOS_ERR_INVALID_CFG,; );

        if (data_size > buf_len) {                              // Exit to transmit data in buffer.
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          goto exit;
        }

        p_str = Str_Copy_N(p_buf_wr, STR_CR_LF, STR_CR_LF_LEN);
        RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_str += STR_CR_LF_LEN;
        buf_len -= STR_CR_LF_LEN;
        p_buf_wr = p_str;

        p_conn->TxDataPtr = p_conn->TxBufPtr;                   // Set TxDataPtr to internal buffer.

        p_conn->TxDataLen += STR_CR_LF_LEN;                     // Update Data Length to transmit.

        last_chunk = DEF_BIT_IS_SET(p_conn->ReqFlags, HTTPc_FLAG_REQ_BODY_CHUNK_LAST);
        if (last_chunk == DEF_YES) {
          p_str = HTTP_ChunkTransferWrSize(p_buf_wr, buf_len, nbr_dig_max, 0);
          RTOS_ASSERT_DBG_ERR_SET((p_str != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG,; );

          p_conn->TxDataLen += (p_str - p_buf_wr);              // Update Data Length to transmit.
          p_buf_wr = p_str;

          p_str = Str_Copy_N(p_buf_wr, STR_CR_LF, STR_CR_LF_LEN);
          RTOS_ASSERT_CRITICAL((p_str != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

          p_str += STR_CR_LF_LEN;

          p_conn->TxDataLen += (p_str - p_buf_wr);              // Update Data Length to transmit.
          p_conn->State = HTTPc_CONN_STATE_REQ_END;
        } else {
          p_conn->State = HTTPc_CONN_STATE_REQ_BODY_DATA;
        }
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
    }
#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_CLIENT_AVAIL
