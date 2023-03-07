/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Request Module
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

#include  <cpu/include/cpu.h>
#include  <common/include/lib_ascii.h>
#include  <common/include/lib_str.h>

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

/********************************************************************************************************
 *                                               FORM DEFINES
 *******************************************************************************************************/

#define  HTTPs_STR_MULTIPART_CTRL_END_SEC                   STR_CR_LF STR_CR_LF
#define  HTTPs_STR_MULTIPART_CTRL_END_SEC_LEN               (sizeof(HTTPs_STR_MULTIPART_CTRL_END_SEC) - 1)

#define  HTTPs_STR_MULTIPART_DATA_START                     "--"
#define  HTTPs_STR_MULTIPART_DATA_START_LEN                 (sizeof(HTTPs_STR_MULTIPART_DATA_START) - 1)

#define  HTTPs_STR_MULTIPART_DATA_END                       STR_CR_LF "--"
#define  HTTPs_STR_MULTIPART_DATA_END_LEN                   (sizeof(HTTPs_STR_MULTIPART_DATA_END) - 1)

#define  HTTPs_STR_MULTIPART_LAST                           "--" STR_CR_LF
#define  HTTPs_STR_MULTIPART_LAST_LEN                       (sizeof(HTTPs_STR_MULTIPART_LAST) - 1)

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void HTTPsReq_MethodParse(HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN     *p_conn,
                                 HTTPs_ERR      *p_err);

static CPU_BOOLEAN HTTPsReq_URI_Parse(HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN     *p_conn,
                                      HTTPs_ERR      *p_err);

static void HTTPsReq_QueryStrParse(HTTPs_INSTANCE *p_instance,
                                   HTTPs_CONN     *p_conn,
                                   HTTPs_ERR      *p_err);

#if (HTTPs_CFG_QUERY_STR_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsReq_QueryStrKeyValBlkAdd(HTTPs_INSTANCE *p_instance,
                                                 HTTPs_CONN     *p_conn,
                                                 CPU_CHAR       *p_str,
                                                 CPU_SIZE_T     str_len,
                                                 HTTPs_ERR      *p_err);
#endif

static void HTTPsReq_ProtocolVerParse(HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN     *p_conn,
                                      HTTPs_ERR      *p_err);

static void HTTPsReq_HdrParse(HTTPs_INSTANCE *p_instance,
                              HTTPs_CONN     *p_conn,
                              HTTPs_ERR      *p_err);

static CPU_CHAR *HTTPsReq_HdrParseValGet(CPU_CHAR   *p_field,
                                         CPU_INT16U field_len,
                                         CPU_CHAR   *p_field_end,
                                         CPU_INT16U *p_len_rem);

#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsReq_BodyForm(HTTPs_INSTANCE *p_instance,
                                     HTTPs_CONN     *p_conn,
                                     HTTPs_ERR      *p_err);

static CPU_BOOLEAN HTTPsReq_BodyFormAppParse(HTTPs_INSTANCE *p_instance,
                                             HTTPs_CONN     *p_connn,
                                             HTTPs_ERR      *p_err);

static CPU_BOOLEAN HTTPsReq_BodyFormAppKeyValBlkAdd(HTTPs_INSTANCE *p_instance,
                                                    HTTPs_CONN     *p_conn,
                                                    CPU_CHAR       *p_str,
                                                    CPU_SIZE_T     str_len,
                                                    HTTPs_ERR      *p_err);

#if (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsReq_BodyFormMultipartParse(HTTPs_INSTANCE *p_instance,
                                                   HTTPs_CONN     *p_conn,
                                                   HTTPs_ERR      *p_err);

static CPU_CHAR *HTTPsReq_BodyFormMultipartBoundarySrch(CPU_CHAR   *p_boundary,
                                                        CPU_INT08U boundary_len,
                                                        CPU_CHAR   *p_buf,
                                                        CPU_INT16U buf_len,
                                                        CPU_CHAR   **p_boundary_sep);

static CPU_BOOLEAN HTTPsReq_BodyFormMultipartCtrlParse(HTTPs_INSTANCE *p_instance,
                                                       HTTPs_CONN     *p_conn,
                                                       HTTPs_ERR      *p_err);

static CPU_BOOLEAN HTTPsReq_BodyFormMultipartFileWr(HTTPs_INSTANCE *p_instance,
                                                    HTTPs_CONN     *p_conn,
                                                    HTTPs_ERR      *p_err);
#endif
#endif

#if ((HTTPs_CFG_QUERY_STR_EN == DEF_ENABLED) \
  || ((HTTPs_CFG_FORM_EN == DEF_ENABLED)))
static CPU_BOOLEAN HTTPsReq_URL_EncodeStrParse(HTTPs_INSTANCE *p_instance,
                                               HTTPs_CONN     *p_conn,
                                               HTTPs_KEY_VAL  *p_key_val,
                                               CPU_BOOLEAN    from_query,
                                               CPU_CHAR       *p_str,
                                               CPU_SIZE_T     str_len);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           HTTPsReq_Handler()
 *
 * @brief    (1) Parse request:
 *               - (a) Parse request method
 *               - (b) Parse request URI
 *               - (c) Parse request query string (if necessary)
 *               - (d) Parse request protocol version
 *               - (e) Parse request headers
 *               - (f) (HOOK) Authentication
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @return   HTTPs_CONN_PROCESS_CONTINUE, connection successfully updated and the process can continue.
 *           HTTPs_CONN_PROCESS_RX_TX,    connection requires more data to complete the parse process.
 *
 * @note     (2) RFC #2616, Section 5 'Request' specifies how a request message must be structured:
 **              A request message from a client to a server includes, within the first line of that message,
 *               the method to be applied to the resource, the identifier of the resource, and the protocol
 *               version in use.
 *
 *                   Request = Request-Line
 *                               *(( general-header
 *                               | request-header
 *                               | entity-header ) CRLF)
 *                           CRLF
 *                           [ message-body ]
 *******************************************************************************************************/
void HTTPsReq_Handler(HTTPs_INSTANCE *p_instance,
                      HTTPs_CONN     *p_conn)
{
  const HTTPs_CFG      *p_cfg;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
  HTTPs_INSTANCE_ERRS  *p_ctr_err = DEF_NULL;
  CPU_BOOLEAN          accepted;
  CPU_BOOLEAN          done;
  CPU_BOOLEAN          hook_def;
  CPU_BOOLEAN          is_query_str_found;
  HTTPs_ERR            err_http = HTTPs_ERR_NONE;

  p_cfg = (HTTPs_CFG *)p_instance->CfgPtr;

  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);
  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  done = DEF_NO;
  while (done != DEF_YES) {
    switch (p_conn->State) {
      case HTTPs_CONN_STATE_REQ_INIT:
        LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - New Request received"));
        HTTPs_STATS_INC(p_ctr_stats->Req_StatRxdCtr);
#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
        p_conn->HdrType = HTTPs_HDR_TYPE_REQ;
#endif
        p_conn->State = HTTPs_CONN_STATE_REQ_PARSE_METHOD;
        break;

      //                                                           ---------------- PARSE REQ METHOD ------------------
      case HTTPs_CONN_STATE_REQ_PARSE_METHOD:
        HTTPsReq_MethodParse(p_instance, p_conn, &err_http);
        if (err_http != HTTPs_ERR_NONE) {                                   // If the Method parsing has failed...
          HTTPs_ERR_INC(p_ctr_err->Req_ErrInvalidCtr);                      // ...generate an error...
          p_conn->ErrCode = err_http;
          p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
          p_conn->SockState = HTTPs_SOCK_STATE_NONE;
          done = DEF_YES;                                                   // ...and exit the state machine.
          break;
        }
        //                                                         If the Method parsing is successful...
        p_conn->State = HTTPs_CONN_STATE_REQ_PARSE_URI;                     // ...go to the next step.
        break;

      //                                                           ------------------ PARSE REQ URI -------------------
      case HTTPs_CONN_STATE_REQ_PARSE_URI:
        is_query_str_found = HTTPsReq_URI_Parse(p_instance, p_conn, &err_http);
        switch (err_http) {
          case HTTPs_ERR_NONE:                                       // If the URI parsing is successful...
            if (is_query_str_found == DEF_YES) {                     // ...check if query string need to be parse.
              p_conn->State = HTTPs_CONN_STATE_REQ_PARSE_QUERY_STRING;
            } else {
              p_conn->State = HTTPs_CONN_STATE_REQ_PARSE_PROTOCOL_VERSION;
            }
            break;

          case HTTPs_ERR_REQ_MORE_DATA_REQUIRED:                     // If more data is required to complete the...
            p_conn->SockState = HTTPs_SOCK_STATE_RX;                 // ...URI Parsing, exit the state machine.
            done = DEF_YES;
            break;

          default:                                                   // If the URI parsing has failed...
            HTTPs_ERR_INC(p_ctr_err->Req_ErrInvalidCtr);               // ...generate an error...
            p_conn->SockState = HTTPs_SOCK_STATE_NONE;
            p_conn->ErrCode = err_http;
            p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
            done = DEF_YES;                                          // ...and exit the state machine.
            break;
        }
        break;

      //                                                           --------------- PARSE REQ QUERY STR ----------------
      case HTTPs_CONN_STATE_REQ_PARSE_QUERY_STRING:
        HTTPsReq_QueryStrParse(p_instance, p_conn, &err_http);
        switch (err_http) {
          case HTTPs_ERR_NONE:                                       // If the Query Str parsing is successful...
                                                                     // ...go to the next step.
            p_conn->State = HTTPs_CONN_STATE_REQ_PARSE_PROTOCOL_VERSION;
            break;

          case HTTPs_ERR_REQ_MORE_DATA_REQUIRED:                     // If more data is required to complete the...
            p_conn->SockState = HTTPs_SOCK_STATE_RX;                 // ...Query Str Parsing, exit the state machine.
            done = DEF_YES;
            break;

          default:                                                   // If the Query Str parsing has failed...
            HTTPs_ERR_INC(p_ctr_err->Req_ErrInvalidCtr);               // ...generate an error...
            p_conn->SockState = HTTPs_SOCK_STATE_NONE;
            p_conn->ErrCode = err_http;
            p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
            done = DEF_YES;                                          // ...and exit the state machine.
            break;
        }
        break;

      //                                                           -------------- PARSE REQ PROTOCOL VER --------------
      case HTTPs_CONN_STATE_REQ_PARSE_PROTOCOL_VERSION:
        HTTPsReq_ProtocolVerParse(p_instance, p_conn, &err_http);
        switch (err_http) {
          case HTTPs_ERR_NONE:                                          // If the Protocol Ver parsing is successful...
                                                                        // ...go to the next step.
            p_conn->State = HTTPs_CONN_STATE_REQ_PARSE_HDR;
            p_conn->SockState = HTTPs_SOCK_STATE_NONE;
            DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_LOCATION);
            break;

          case HTTPs_ERR_REQ_MORE_DATA_REQUIRED:                        // If more data is required to complete the...
            p_conn->SockState = HTTPs_SOCK_STATE_RX;                    // ...Protocol Ver parsing, exit the state...
            done = DEF_YES;                                             // ...machine.
            break;

          default:                                                    // If the Protocol Ver parsing has failed...
            HTTPs_ERR_INC(p_ctr_err->Req_ErrInvalidCtr);                  // ...generate an error...
            p_conn->SockState = HTTPs_SOCK_STATE_NONE;
            p_conn->ErrCode = err_http;
            p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
            done = DEF_YES;                                             // ...and exit the state machine.
            break;
        }
        break;

      //                                                           ------------------ PARSE REQ HDR -------------------
      case HTTPs_CONN_STATE_REQ_PARSE_HDR:
        HTTPsReq_HdrParse(p_instance, p_conn, &err_http);          // See Note #2.
        switch (err_http) {
          case HTTPs_ERR_NONE:                                  // If the Protocol Ver parsing is successful...
                                                                // ...go to the next step.
            p_conn->State = HTTPs_CONN_STATE_REQ_LINE_HDR_HOOK;
            p_conn->SockState = HTTPs_SOCK_STATE_NONE;
            HTTPs_STATS_INC(p_ctr_stats->Req_StatProcessedCtr);
            break;

          case HTTPs_ERR_REQ_MORE_DATA_REQUIRED:                // If more data is required to complete the...
            p_conn->SockState = HTTPs_SOCK_STATE_RX;               // ...Protocol Ver parsing, exit the state...
            done = DEF_YES;                                     // ...machine.
            break;

          default:                                               // If the Header parsing has failed...
            HTTPs_ERR_INC(p_ctr_err->Req_ErrInvalidCtr);              // ...generate an error...
            p_conn->ErrCode = err_http;
            p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
            done = DEF_YES;                                      // ...and exit the state machine.
            break;
        }
        break;

      //                                                           --------------- CONN REQ EXT PROCESS ---------------
      case HTTPs_CONN_STATE_REQ_LINE_HDR_HOOK:
        hook_def = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnReqHook);
        if (hook_def == DEF_YES) {
          accepted = p_cfg->HooksPtr->OnReqHook(p_instance,
                                                p_conn,
                                                p_cfg->Hooks_CfgPtr);
          if (accepted != DEF_YES) {
            //                                                     If the connection is not authorized ...
            if (p_conn->StatusCode == HTTP_STATUS_OK) {
              p_conn->StatusCode = HTTP_STATUS_UNAUTHORIZED;
            }
            DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_REQ_FLUSH);
            p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FLUSH_DATA;
          }
        }
        //                                                         Otherwise, receive the body.
        p_conn->State = HTTPs_CONN_STATE_REQ_BODY_INIT;
        done = DEF_YES;                                         // ... exit the state machine.
        break;

      default:
        HTTPs_ERR_INC(p_ctr_err->Req_ErrStateUnkownCtr);
        p_conn->ErrCode = HTTPs_ERR_STATE_UNKNOWN;
        p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
        done = DEF_YES;
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
    }
  }
}

/****************************************************************************************************//**
 *                                               HTTPsReq_Body()
 *
 * @brief    Process Body received in HTTP request.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @note     (2) RFC #2616, Section 9 'Method Definitions' describes methods for HTTP/1.1:
 *               - (a) RFC #2616, Section 9.3 'GET' describes GET method:
 *
 *                       The GET method means retrieve whatever information (in the form of an
 *                       entity) is identified by the Request-URI. If the Request-URI refers
 *                       to a data-producing process, it is the produced data which shall be
 *                       returned as the entity in the response and not the source text of the
 *                       process, unless that text happens to be the output of the process.
 *
 *                       The semantics of the GET method change to a "conditional GET" if the
 *                       request message includes an If-Modified-Since, If-Unmodified-Since,
 *                       If-Match, If-None-Match, or If-Range header field. A conditional GET
 *                       method requests that the entity be transferred only under the
 *                       circumstances described by the conditional header field(s). The
 *                       conditional GET method is intended to reduce unnecessary network
 *                       usage by allowing cached entities to be refreshed without requiring
 *                       multiple requests or transferring data already held by the client.
 *
 *                       The semantics of the GET method change to a "partial GET" if the
 *                       request message includes a Range header field. A partial GET requests
 *                       that only part of the entity be transferred, as described in section
 *                       14.35. The partial GET method is intended to reduce unnecessary
 *                       network usage by allowing partially-retrieved entities to be
 *                       completed without transferring data already held by the client.
 *
 *                       The response to a GET request is cacheable if and only if it meets
 *                       the requirements for HTTP caching described in section 13.
 *
 *                       See section 15.1.3 for security considerations when used for forms.
 *
 *               - (b) RFC #2616, Section 9.4 'HEAD' describes HEAD method:
 *
 *                       The HEAD method is identical to GET except that the server MUST NOT
 *                       return a message-body in the response. The metainformation contained
 *                       in the HTTP headers in response to a HEAD request SHOULD be identical
 *                       to the information sent in response to a GET request. This method can
 *                       be used for obtaining metainformation about the entity implied by the
 *                       request without transferring the entity-body itself. This method is
 *                       often used for testing hypertext links for validity, accessibility,
 *                       and recent modification.
 *
 *               - (c) RFC #2616, Section 9.5 'POST' describes POST method:
 *
 *                       The POST method is used to request that the origin server accept the entity
 *                       enclosed in the request as a new subordinate of the resource identified by
 *                       the Request-URI in the Request-Line. POST is designed to allow a uniform
 *                       method to cover the following functions:
 *
 *                           - Annotation of existing resources;
 *                           - Posting a message to a bulletin board, newsgroup, mailing list,
 *                       or similar group of articles;
 *                           - Providing a block of data, such as the result of submitting a
 *                       form, to a data-handling process;
 *                           - Extending a database through an append operation.
 *
 *                       The actual function performed by the POST method is determined by the server
 *                       and is usually dependent on the Request-URI. The posted entity is subordinate
 *                       to that URI in the same way that a file is subordinate to a directory
 *                       containing it, a news article is subordinate to a newsgroup to which it is
 *                       posted, or a record is subordinate to a database.
 *
 *                       The action performed by the POST method might not result in a resource that
 *                       can be identified by a URI. In this case, either 200 (OK) or 204 (No Content)
 *                       is the appropriate response status, depending on whether or not the response
 *                       includes an entity that describes the result.
 *
 *                       If a resource has been created on the origin server, the response SHOULD be 201
 *                       (Created) and contain an entity which describes the status of the request and
 *                       refers to the new resource, and a Location header (see section 14.30).
 *
 *                       Responses to this method are not cacheable, unless the response includes
 *                       appropriate Cache-Control or Expires header fields. However, the 303 (See Other)
 *                       response can be used to direct the user agent to retrieve a cacheable resource.
 *
 *                       POST requests MUST obey the message transmission requirements set out in
 *                       section 8.2.
 *
 *                       See section 15.1.3 for security considerations.
 *******************************************************************************************************/
void HTTPsReq_Body(HTTPs_INSTANCE *p_instance,
                   HTTPs_CONN     *p_conn)
{
  const HTTPs_CFG      *p_cfg;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
  HTTPs_INSTANCE_ERRS  *p_ctr_err = DEF_NULL;
  CPU_SIZE_T           data_len_rd;
  CPU_BOOLEAN          hook_continue;
  CPU_BOOLEAN          done;
  CPU_BOOLEAN          body_hook_def;
  CPU_BOOLEAN          req_flushed;
#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
  CPU_BOOLEAN parse_done;
  HTTPs_ERR   local_err = HTTPs_ERR_NONE;
#endif

  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);
  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  p_cfg = p_instance->CfgPtr;

  done = DEF_NO;
  while (done == DEF_NO) {
    switch (p_conn->State) {
      //                                                           ------------- PREPARE FOR BODY PARSING -------------
      case HTTPs_CONN_STATE_REQ_BODY_INIT:
        p_conn->ReqContentLenRxd = 0u;                          // Clear the length of data received variable.

        //                                                         SET CONN STATE AND SOCKET STATE FOR BODY PARSING.
        if (p_conn->ReqContentLen == 0) {                       // If all data received (no body) ...
                                                                // ... jump to response preparation.
          p_conn->SockState = HTTPs_SOCK_STATE_NONE;
          p_conn->State = HTTPs_CONN_STATE_REQ_READY_SIGNAL;
          p_conn->RxBufLenRem = 0;
          done = DEF_YES;
        } else {                                                // If a body is present in the request received:
          switch (p_conn->Method) {                             // (1) Set the conn state for the parsing.
            case HTTP_METHOD_GET:
              p_conn->State = HTTPs_CONN_STATE_REQ_BODY_DATA;
              HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodGetProcessedCtr);
              break;

            case HTTP_METHOD_HEAD:
              p_conn->State = HTTPs_CONN_STATE_REQ_BODY_DATA;
              HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodHeadProcessedCtr);
              break;

            case HTTP_METHOD_DELETE:
              p_conn->State = HTTPs_CONN_STATE_REQ_BODY_DATA;
              HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodDeleteProcessedCtr);
              break;

            case HTTP_METHOD_PUT:
              p_conn->State = HTTPs_CONN_STATE_REQ_BODY_DATA;
              HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodPutProcessedCtr);
              break;

            case HTTP_METHOD_POST:
              //                                                   Check if the POST Content-Type matches with those ...
              //                                                   ... the server core can parse.
#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
              if (p_conn->ReqContentType == HTTP_CONTENT_TYPE_APP_FORM) {
                p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FORM_APP_PARSE;
              } else if (p_conn->ReqContentType == HTTP_CONTENT_TYPE_MULTIPART_FORM) {
                p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_INIT;
              } else {
                p_conn->State = HTTPs_CONN_STATE_REQ_BODY_DATA;
              }
#else
              HTTPs_ERR_INC(p_ctr_err->Req_ErrBodyFormNotEn);
              p_conn->State = HTTPs_CONN_STATE_REQ_BODY_DATA;
#endif
              HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodPostProcessedCtr);
              break;

            default:
              HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodUnsupportedProcessedCtr);
              p_conn->ErrCode = HTTPs_ERR_REQ_METHOD_NOT_SUPPORTED;
              p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
              goto exit;
          }

          if (p_conn->RxBufLenRem == 0) {                       // (2) Set the socket state.
            p_conn->SockState = HTTPs_SOCK_STATE_RX;               // Need to Rx more data for parsing step.
            done = DEF_YES;
          } else {
            p_conn->SockState = HTTPs_SOCK_STATE_NONE;
          }
        }
        break;

      //                                                           ----- NOTIFY UPPER APP THAT DATA IS AVAILABLE ------
      case HTTPs_CONN_STATE_REQ_BODY_DATA:
        body_hook_def = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnReqBodyRxHook);
        //                                                         If the hook for the body is defined.
        if (body_hook_def == DEF_TRUE) {
          //                                                       Call the hook function.
          hook_continue = p_cfg->HooksPtr->OnReqBodyRxHook(p_instance,
                                                           p_conn,
                                                           p_cfg->Hooks_CfgPtr,
                                                           p_conn->RxBufPtr,
                                                           p_conn->RxBufLenRem,
                                                           &data_len_rd);

          if ((data_len_rd > p_conn->RxBufLenRem)
              && (p_conn->RxBufLenRem > 0)                   ) {          // Fatal error.
            p_conn->ErrCode = HTTPs_ERR_REQ_BODY_FAULT;
            p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
            goto exit;
          }

          //                                                       Update length of data consumed.
          p_conn->RxBufLenRem -= data_len_rd;
          p_conn->ReqContentLenRxd += data_len_rd;
          p_conn->RxBufPtr += data_len_rd;

          if (hook_continue == DEF_NO) {                        // Case when the App doesn't want to rx more data.
                                                                // While there is data to receive, flush it.
            p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FLUSH_DATA;
            p_conn->SockState = HTTPs_SOCK_STATE_RX;
          } else {
            //                                                     If there is more data to read
            if (p_conn->ReqContentLenRxd < p_conn->ReqContentLen) {
              //                                                   Ask for more data.
              p_conn->State = HTTPs_CONN_STATE_REQ_BODY_DATA;
              p_conn->SockState = HTTPs_SOCK_STATE_RX;
            } else {
              //                                                   Otherwise, prepare the request response body.
              p_conn->State = HTTPs_CONN_STATE_REQ_READY_SIGNAL;
              p_conn->SockState = HTTPs_SOCK_STATE_NONE;
            }
          }
        } else {                                                // Case hook is not defined: flush data rx.
          p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FLUSH_DATA;
          p_conn->SockState = HTTPs_SOCK_STATE_RX;
        }
        done = DEF_YES;
        break;

      //                                                           ----------- PARSE FORM RECEIVED IN POST ------------
      case HTTPs_CONN_STATE_REQ_BODY_FORM_APP_PARSE:
      case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_INIT:
      case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_PARSE:
      case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_OPEN:
      case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_WR:
#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
        parse_done = HTTPsReq_BodyForm(p_instance, p_conn, &local_err);
        switch (local_err) {
          case HTTPs_ERR_NONE:
            if (parse_done == DEF_YES) {
              p_conn->State = HTTPs_CONN_STATE_REQ_READY_SIGNAL;
              p_conn->SockState = HTTPs_SOCK_STATE_NONE;
              done = DEF_YES;
            }
            break;

          case HTTPs_ERR_REQ_MORE_DATA_REQUIRED:
            done = DEF_YES;
            p_conn->SockState = HTTPs_SOCK_STATE_RX;
            break;

          default:
            done = DEF_YES;
            p_conn->ErrCode = local_err;
            p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
            p_conn->SockState = HTTPs_SOCK_STATE_NONE;
            HTTPs_ERR_INC(p_ctr_err->Req_ErrBodyPostFormCtr);
            break;
        }
#else
        HTTPs_ERR_INC(p_ctr_err->Req_ErrBodyFormNotEn);
        p_conn->ErrCode = HTTPs_ERR_CFG_INVALID_FORM_EN;
        p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
        p_conn->SockState = HTTPs_SOCK_STATE_NONE;
#endif
        break;

      //                                                           ---------- FLUSH ALL REMAINING BODY DATA -----------
      case HTTPs_CONN_STATE_REQ_BODY_FLUSH_DATA:
        p_conn->ReqContentLenRxd += p_conn->RxBufLenRem;
        p_conn->RxBufLenRem = 0;

        if ((p_conn->ReqContentLen == 0)
            || (p_conn->ReqContentLenRxd >= p_conn->ReqContentLen)) {
          //                                                       If there is no more data to receive. Process status.
          req_flushed = DEF_BIT_IS_SET(p_conn->Flags, HTTPs_FLAG_REQ_FLUSH);
          if (req_flushed == DEF_YES) {
            p_conn->State = HTTPs_CONN_STATE_RESP_PREPARE;
          } else {
            p_conn->State = HTTPs_CONN_STATE_REQ_READY_SIGNAL;
          }
          p_conn->SockState = HTTPs_SOCK_STATE_NONE;
        } else {
          //                                                       While there is data to receive, flush it.
          p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FLUSH_DATA;
          p_conn->SockState = HTTPs_SOCK_STATE_RX;
        }
        done = DEF_YES;
        break;

      default:
        HTTPs_ERR_INC(p_ctr_err->Req_ErrBodyStateUnknownCtr);
        p_conn->ErrCode = HTTPs_ERR_STATE_UNKNOWN;
        p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           HTTPsReq_RdySignal()
 *
 * @brief    Signal the upper application that the request was received completely and that it CAN
 *           start the request processing.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @return   DEF_YES, if upper application has finish the request processing.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) If the request processing by the application is not completed the poll hook will be called
 *               until it's done.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsReq_RdySignal(HTTPs_INSTANCE *p_instance,
                               HTTPs_CONN     *p_conn)
{
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
  HTTPs_KEY_VAL   *p_key_val = DEF_NULL;
  CPU_BOOLEAN     hook_def = DEF_NO;
  CPU_BOOLEAN     process_done = DEF_NO;

#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
  p_key_val = p_conn->FormDataListPtr;
#endif

  switch (p_conn->State) {
    //                                                             ------------ POST DATA RX TO USER APP --------------
    case HTTPs_CONN_STATE_REQ_READY_SIGNAL:
      hook_def = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnReqRdySignalHook);
      if (hook_def == DEF_YES) {
        //                                                         Callback fnct process data.
        process_done = p_cfg->HooksPtr->OnReqRdySignalHook(p_instance,
                                                           p_conn,
                                                           p_cfg->Hooks_CfgPtr,
                                                           (const HTTPs_KEY_VAL *)p_key_val);

        if (process_done != DEF_YES) {
          p_conn->State = HTTPs_CONN_STATE_REQ_READY_POLL;
        }
      } else {
        process_done = DEF_YES;
      }
      break;

    //                                                             ----------- WAIT END OF DATA PROCESSING ------------
    case HTTPs_CONN_STATE_REQ_READY_POLL:
      hook_def = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnReqRdyPollHook);
      if (hook_def == DEF_YES) {
        //                                                         Wait until data processing is completed.
        process_done = p_cfg->HooksPtr->OnReqRdyPollHook(p_instance,
                                                         p_conn,
                                                         p_cfg->Hooks_CfgPtr);
      } else {
        process_done = DEF_YES;
      }
      break;

    default:
      process_done = DEF_YES;
      p_conn->ErrCode = HTTPs_ERR_STATE_UNKNOWN;
      p_conn->State = HTTPs_CONN_STATE_ERR_INTERNAL;
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE, process_done);
  }

  return (process_done);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           HTTPsReq_MethodParse()
 *
 * @brief    Parse request Method
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) RFC #2616, Section 5.1.1 'Method' defines request line methods:
 *                       The Method token indicates the method to be performed on the resource identified by the
 *                       Request-URI. The method is case-sensitive.
 *
 *                       Method         = "OPTIONS"                ; Section 9.2
 *                       | "GET"                    ; Section 9.3
 *                       | "HEAD"                   ; Section 9.4
 *                       | "POST"                   ; Section 9.5
 *                       | "PUT"                    ; Section 9.6
 *                       | "DELETE"                 ; Section 9.7
 *                       | "TRACE"                  ; Section 9.8
 *                       | "CONNECT"                ; Section 9.9
 *                       | extension-method
 *                       extension-method = token
 *
 *                       The list of methods allowed by a resource can be specified in an Allow header field
 *                       (section 14.7). The return code of the response always notifies the client whether a
 *                       method is currently allowed on a resource, since the set of allowed methods can change
 *                       dynamically. An origin server SHOULD return the status code 405 (Method Not Allowed)
 *                       if the method is known by the origin server but not allowed for the requested resource,
 *                       and 501 (Not Implemented) if the method is unrecognized or not implemented by the origin
 *                       server. The methods GET and HEAD MUST be supported by all general-purpose servers. All
 *                       other methods are OPTIONAL; however, if the above methods are implemented, they MUST be
 *                       implemented with the same semantics as those specified in section 9.
 *******************************************************************************************************/
static void HTTPsReq_MethodParse(HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN     *p_conn,
                                 HTTPs_ERR      *p_err)
{
  CPU_CHAR             *p_request_method_start;
  CPU_CHAR             *p_request_method_end;
  CPU_SIZE_T           len;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
  CPU_INT32U           method;

  PP_UNUSED_PARAM(p_instance);
  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);

  len = p_conn->RxBufLenRem;

  if (len <= sizeof(HTTP_STR_METHOD_GET)) {                     // Check minimum length of RxBuf.
    *p_err = HTTPs_ERR_REQ_FORMAT_INVALID;
    return;
  }
  //                                                               Move the start ptr to the first meaningful char.
  p_request_method_start = HTTP_StrGraphSrchFirst(p_conn->RxBufPtr, len);
  if (p_request_method_start == DEF_NULL) {
    *p_err = HTTPs_ERR_REQ_FORMAT_INVALID;
    return;
  }
  len -= p_request_method_start - p_conn->RxBufPtr;
  //                                                               Find the end of method string.
  p_request_method_end = Str_Char_N(p_request_method_start, len, ASCII_CHAR_SPACE);
  if (p_request_method_end == DEF_NULL) {
    *p_err = HTTPs_ERR_REQ_FORMAT_INVALID;
    return;
  }
  len = p_request_method_end - p_request_method_start;
  //                                                               Try to match the Method str received.
  method = HTTP_Dict_KeyGet(HTTP_Dict_ReqMethod,
                            HTTP_Dict_ReqMethodSize,
                            p_request_method_start,
                            DEF_YES,
                            len);
  //                                                               Validate the DictionaryKey search results
  if (method == HTTP_DICT_KEY_INVALID) {
    p_conn->Method = HTTP_METHOD_UNKNOWN;
  } else {
    p_conn->Method = (HTTP_METHOD)method;
  }

  switch (p_conn->Method) {
    case HTTP_METHOD_GET:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Method = GET"));
      p_conn->RespBodyDataType = HTTPs_BODY_DATA_TYPE_FILE;
      HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodGetRxdCtr);
      break;

    case HTTP_METHOD_HEAD:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Method = HEAD"));
      p_conn->RespBodyDataType = HTTPs_BODY_DATA_TYPE_FILE;
      HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodHeadRxdCtr);
      break;

    case HTTP_METHOD_POST:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Method = POST"));
      p_conn->RespBodyDataType = HTTPs_BODY_DATA_TYPE_NONE;
      HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodPostRxdCtr);
      break;

    case HTTP_METHOD_DELETE:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Method = DELETE"));
      p_conn->RespBodyDataType = HTTPs_BODY_DATA_TYPE_NONE;
      HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodDeleteRxdCtr);
      break;

    case HTTP_METHOD_PUT:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Method = PUT"));
      p_conn->RespBodyDataType = HTTPs_BODY_DATA_TYPE_NONE;
      HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodPutRxdCtr);
      break;

    case HTTP_METHOD_TRACE:
    case HTTP_METHOD_CONNECT:
    case HTTP_METHOD_UNKNOWN:
    default:
      LOG_VRB(("HTTPs - SockID: ", (u)p_conn->SockID, " - Method = Unsupported"));
      HTTPs_STATS_INC(p_ctr_stats->Req_StatMethodUnsupportedRxdCtr);
      *p_err = HTTPs_ERR_REQ_METHOD_NOT_SUPPORTED;
      return;
  }
  //                                                               Update the RxBuf ptr.
  p_conn->RxBufLenRem -= len;
  p_conn->RxBufPtr = p_request_method_end;

  *p_err = HTTPs_ERR_NONE;
}

/****************************************************************************************************//**
 *                                           HTTPsReq_URI_Parse()
 *
 * @brief    Parse request URI and check for potential query string.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    -----       Argument validated in HTTPs_InstanceStart().
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES,    if potential query string is found.
 *           DEF_NO,     if NO potential query string is found.
 *
 * @note     (1) The Request-URI is a Uniform Resource Identifier (section 3.2) and identifies the resource
 *               upon which to apply the request.
 *
 *                   Request-URI = "*" | absoluteURI | abs_path | authority
 *
 *           - (a) The asterisk "*" means that the request does not apply to a particular resource, but to
 *                 the server itself, and is only allowed when the method used does not necessarily apply
 *                 to a resource. One example would be
 *
 *                       OPTIONS * HTTP/1.1
 *
 *           - (b) The absoluteURI form is REQUIRED when the request is being made to a proxy. The proxy is
 *                 requested to forward the request or service it from a valid cache, and return the response.
 *                 Note that the proxy MAY forward the request on to another proxy or directly to the server
 *                 specified by the absoluteURI. In order to avoid request loops, a proxy MUST be able to
 *                 recognize all of its server names, including any aliases, local variations, and the numeric
 *                 IP address. An example Request-Line would be:
 *
 *                       GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1
 *
 *                 To allow for transition to absoluteURIs in all requests in future versions of HTTP, all
 *                 HTTP/1.1 servers MUST accept the absoluteURI form in requests, even though HTTP/1.1 clients
 *                 will only generate them in requests to proxies.
 *
 *           - (c) The authority form is only used by the CONNECT method (section 9.9).
 *
 *           - (d) The most common form of Request-URI is that used to identify a resource on an origin server or
 *                 gateway. In this case the absolute path of the URI MUST be transmitted (see section 3.2.1,
 *                 abs_path) as the Request-URI, and the network location of the URI (authority) MUST be transmitted
 *                 in a Host header field. For example, a client wishing to retrieve the resource above directly
 *                 from the origin server would create a TCP connection to port 80 of the host "www.w3.org" and send
 *                 the lines:
 *
 *                       GET /pub/WWW/TheProject.html HTTP/1.1
 *                       Host: www.w3.org
 *
 *                 followed by the remainder of the Request. Note that the absolute path cannot be empty; if none
 *                 is present in the original URI, it MUST be given as "/" (the server root).
 *
 *           - (e) The Request-URI is transmitted in the format specified in section 3.2.1. If the Request-URI is
 *                 encoded using the "% HEX HEX" encoding [42], the origin server MUST decode the Request-URI in
 *                 order to properly interpret the request. Servers SHOULD respond to invalid Request-URIs with an
 *                 appropriate status code.
 *******************************************************************************************************/
static CPU_BOOLEAN HTTPsReq_URI_Parse(HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN     *p_conn,
                                      HTTPs_ERR      *p_err)
{
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;;
  CPU_CHAR        *p_request_uri_start = DEF_NULL;
  CPU_CHAR        *p_request_uri_end = DEF_NULL;
  CPU_CHAR        *p_request_uri_separator = DEF_NULL;
  CPU_CHAR        *p_str = DEF_NULL;
  CPU_INT32U      len = p_conn->RxBufLenRem;;
  CPU_SIZE_T      len_decoded = 0u;
  CPU_BOOLEAN     is_query_found = DEF_NO;

  //                                                               Move the start ptr to the first meaningful char.
  p_request_uri_start = HTTP_StrGraphSrchFirst(p_conn->RxBufPtr, len);
  if (p_request_uri_start == DEF_NULL) {
    *p_err = HTTPs_ERR_REQ_FORMAT_INVALID;
    return (is_query_found);
  }
  //                                                               Find the end of the URI including the potential...
  //                                                               ...query str.
  len -= p_request_uri_start - p_conn->RxBufPtr;
  p_request_uri_end = Str_Char_N(p_request_uri_start, len, ASCII_CHAR_SPACE);
  if (p_request_uri_end != DEF_NULL) {
    len = p_request_uri_end - p_request_uri_start;              // Recalculate the len to narrow the search.
  }
  //                                                               Try to find a '?' for query string.
  p_request_uri_separator = Str_Char_N(p_request_uri_start, len, ASCII_CHAR_QUESTION_MARK);
  if (p_request_uri_separator == DEF_NULL) {                    // If no query string or a full    URI is found...
    if (p_request_uri_end == DEF_NULL) {
      if (p_conn->RxBufPtr != p_conn->BufPtr) {                 // ...and if the buffer is not full...
                                                                // ... get more data.
        *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
      } else {
        //                                                         ... but if the buffer is full, generate an error.
        *p_err = HTTPs_ERR_REQ_FORMAT_INVALID;
      }
      return (is_query_found);
    }
  } else {
    is_query_found = DEF_YES;
    len = p_request_uri_separator - p_request_uri_start;
  }

  if (len > p_conn->PathLenMax) {                               // If unable to store req'd URI.
    *p_err = HTTPs_ERR_REQ_URI_LEN;                             // Resp with internal err page.
    return (is_query_found);
  }

  p_request_uri_start[len] = ASCII_CHAR_NULL;                   // Replace the char at the end of the URI by NULL.

  if (len > 1) {                                                // Req'd URI is not the default.
                                                                // Copy req'd URI.
    len_decoded = len;

    HTTP_URL_DecodeStr(p_request_uri_start,
                       p_conn->PathPtr,
                       &len_decoded);

    p_conn->PathPtr[len_decoded] = ASCII_CHAR_NULL;
  } else {                                                      // Default page req'd.
    p_str = p_conn->PathPtr;

    if ((p_cfg->DfltResourceNamePtr[0] != HTTPs_PATH_SEP_CHAR_DFLT)
        && (p_cfg->DfltResourceNamePtr[0] != ASCII_CHAR_REVERSE_SOLIDUS)) {
      p_str[0] = HTTPs_PATH_SEP_CHAR_DFLT;
      p_str++;
    }
    //                                                             Copy dflt file path.
    Str_Copy_N(p_str,
               p_cfg->DfltResourceNamePtr,
               p_conn->PathLenMax);

    Str_Char_Replace_N(p_conn->PathPtr,
                       ASCII_CHAR_REVERSE_SOLIDUS,
                       HTTPs_PATH_SEP_CHAR_DFLT,
                       p_conn->PathLenMax);
  }
  //                                                               Update rem len avail in the rx buf.
  p_conn->RxBufLenRem -= (&p_request_uri_start[len] - p_conn->RxBufPtr) + 1;
  p_conn->RxBufPtr = &p_request_uri_start[len] + 1;

  LOG_VRB(("HTTPs - SockId: ", (u)p_conn->SockID, "\r\n    URI =", (s)p_conn->PathPtr));

  *p_err = HTTPs_ERR_NONE;

  return (is_query_found);
}

/****************************************************************************************************//**
 *                                           HTTPsReq_QueryStrParse()
 *
 * @brief    Parse request query string.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    _conn       Pointer to the connection.
 *
 * @param    _err        Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) RFC #3986, Section 3.4 "Query" state:
 *
 *                       The query component contains non-hierarchical data that, along with
 *                       data in the path component (Section 3.3), serves to identify a
 *                       resource within the scope of the URI's scheme and naming authority
 *                       (if any).  The query component is indicated by the first question
 *                       mark ("?") character and terminated by a number sign ("#") character
 *                       or by the end of the URI.
 *
 *               - (2) W3C Recommendations :
 *
 *                       http://www.w3.org/Addressing/URL/4_URI_Recommentations.html
 *
 *               - (2) Each query must be Key-Value Pair separated by a equals sign ('=').
 *
 *               - (3) Control names and values are escaped. Space characters are replaced by `+', and then
 *                       reserved characters are escaped as described in [RFC1738], section 2.2: Non-alphanumeric
 *                       characters are replaced by '%HH', a percent sign and two hexadecimal digits representing
 *                       the ASCII code of the character. Line breaks are represented as "CR LF" pairs
 *                       (i.e., '%0D%0A').
 *******************************************************************************************************/
static void HTTPsReq_QueryStrParse(HTTPs_INSTANCE *p_instance,
                                   HTTPs_CONN     *p_conn,
                                   HTTPs_ERR      *p_err)
{
  CPU_CHAR    *p_req_line_end = DEF_NULL;
  CPU_CHAR    *p_query_str_end = DEF_NULL;
  CPU_CHAR    *p_field_start = DEF_NULL;
  CPU_CHAR    *p_field_end = DEF_NULL;
  CPU_INT32U  len = p_conn->RxBufLenRem;
  CPU_INT32U  len_rd = 0;
  CPU_INT32U  key_val_pair_len = 0;
  CPU_BOOLEAN done = DEF_NO;
#if (HTTPs_CFG_QUERY_STR_EN == DEF_ENABLED)
  CPU_BOOLEAN success = DEF_NO;
#endif

#if (HTTPs_CFG_QUERY_STR_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_instance);
#endif
  //                                                               Check for the end of the request line.
  p_req_line_end = Str_Str_N(p_conn->RxBufPtr, STR_CR_LF, len);
  if (p_req_line_end != DEF_NULL) {
    len = p_req_line_end - p_conn->RxBufPtr;
  }

  //                                                               Check for the end of the query string.
  p_query_str_end = Str_Char_N(p_conn->RxBufPtr, len, ASCII_CHAR_SPACE);
  if (p_query_str_end != DEF_NULL) {
    len = p_query_str_end - p_conn->RxBufPtr + 1;               // If found, set the query length.
  }

  p_field_start = p_conn->RxBufPtr;

  while (done == DEF_NO) {
    //                                                             Search for the query string char separator (&).
    p_field_end = Str_Char_N(p_field_start, len, ASCII_CHAR_AMPERSAND);
    if (p_field_end == DEF_NULL) {                              // If not found, check if it's the last query string.
      if (p_query_str_end == DEF_NULL) {
        if (len == p_conn->BufLen) {
          *p_err = HTTPs_ERR_CFG_INVALID_BUF_LEN;
        } else {
          *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
        }
        goto exit_update;
      } else {
        done = DEF_YES;                                         // Last field of query string.
        p_field_end = p_query_str_end;
      }
    }

    //                                                             Get the length of the key-value pair found.
    key_val_pair_len = p_field_end - p_field_start;
    if (key_val_pair_len == 0u) {                               // In the case it's null, the query has been fully ...
      *p_err = HTTPs_ERR_NONE;                                  // ...parsed and it has finished by a '&'.
      goto exit_update;
    }

#if (HTTPs_CFG_QUERY_STR_EN == DEF_ENABLED)
    success = HTTPsReq_QueryStrKeyValBlkAdd(p_instance,
                                            p_conn,
                                            p_field_start,
                                            key_val_pair_len,
                                            p_err);
    if (success == DEF_FAIL) {
      goto exit_update;
    }
#endif

    p_field_start += (key_val_pair_len + 1);
    len -= (key_val_pair_len + 1);
    len_rd += (key_val_pair_len + 1);
  }

  *p_err = HTTPs_ERR_NONE;

exit_update:
  p_conn->RxBufLenRem -= len_rd;
  p_conn->RxBufPtr += len_rd;

  return;
}

/****************************************************************************************************//**
 *                                       HTTPsReq_QueryStrKeyValBlkAdd()
 *
 * @brief    Add query field received in the Query String to the query list.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_str       Pointer to start of query field string.
 *
 * @param    str_len     Length of query field.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if query field successfully added to the list.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
#if (HTTPs_CFG_QUERY_STR_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsReq_QueryStrKeyValBlkAdd(HTTPs_INSTANCE *p_instance,
                                                 HTTPs_CONN     *p_conn,
                                                 CPU_CHAR       *p_str,
                                                 CPU_SIZE_T     str_len,
                                                 HTTPs_ERR      *p_err)
{
  const HTTPs_CFG *p_cfg;
  HTTPs_KEY_VAL   *p_key_val;
  CPU_BOOLEAN     result;
  RTOS_ERR        local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_cfg = p_instance->CfgPtr;

  if (p_cfg->QueryStrCfgPtr != DEF_NULL) {
    if (p_cfg->QueryStrCfgPtr->NbrPerConnMax != LIB_MEM_BLK_QTY_UNLIMITED) {
      if (p_conn->QueryStrBlkAcquiredCtr >= p_cfg->QueryStrCfgPtr->NbrPerConnMax) {
        *p_err = HTTPs_ERR_KEY_VAL_CFG_POOL_SIZE_INV;
        return (DEF_FAIL);
      }
    }

    p_key_val = HTTPsMem_QueryStrKeyValBlkGet(p_instance,       // Acquire Key-Value block.
                                              p_conn,
                                              &local_err);
    if (p_key_val == DEF_NULL) {                                // If no Key-Value block available.
      *p_err = HTTPs_ERR_QUERY_STR_POOL_GET;
      return (DEF_FAIL);
    }

    result = HTTPsReq_URL_EncodeStrParse(p_instance,
                                         p_conn,
                                         p_key_val,
                                         DEF_YES,
                                         p_str,
                                         str_len);
    if (result == DEF_FAIL) {
      *p_err = HTTPs_ERR_QUERY_STR_PARSE_FAULT;
      return (DEF_FAIL);
    }

    if (p_conn->QueryStrListPtr == DEF_NULL) {
      p_key_val->NextPtr = DEF_NULL;
    } else {
      p_key_val->NextPtr = p_conn->QueryStrListPtr;
    }

    p_conn->QueryStrListPtr = p_key_val;
  }

  *p_err = HTTPs_ERR_NONE;

  return (DEF_OK);
}
#endif

/****************************************************************************************************//**
 *                                       HTTPsReq_ProtocolVerParse()
 *
 * @brief    Parse request protocol version
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note         (1) At the time of implementing this HTTP server version, only the following HTTP version
 *                       are known:
 *
 *                           - (a) HTTP/0.9
 *                           - (b) HTTP/1.0
 *                           - (c) HTTP/1.1
 *
 * @note         (2) RFC #2616, Section 19.6 "Compatibility with Previous Versions" state:
 *
 *                       It is beyond the scope of a protocol specification to mandate
 *                       compliance with previous versions. HTTP/1.1 was deliberately
 *                       designed, however, to make supporting previous versions easy. It is
 *                       worth noting that, at the time of composing this specification
 *                       (1996), we would expect commercial HTTP/1.1 servers to:
 *
 *                           - recognize the format of the Request-Line for HTTP/0.9, 1.0, and
 *                       1.1 requests;
 *                           - understand any valid request in the format of HTTP/0.9, 1.0, or
 *                       1.1;
 *                       And we would expect HTTP/1.1 clients to:
 *
 *                           - recognize the format of the Status-Line for HTTP/1.0 and 1.1
 *                       responses;
 *
 *                           - understand any valid response in the format of HTTP/0.9, 1.0, or
 *                       1.1.
 *
 *                       For most implementations of HTTP/1.0, each connection is established
 *                       by the client prior to the request and closed by the server after
 *                       sending the response. Some implementations implement the Keep-Alive
 *                       version of persistent connections described in section 19.7.1 of RFC
 *                       2068 [33].
 *******************************************************************************************************/
static void HTTPsReq_ProtocolVerParse(HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN     *p_conn,
                                      HTTPs_ERR      *p_err)
{
#if (HTTPs_CFG_PERSISTENT_CONN_EN == DEF_ENABLED)
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
#endif
  CPU_CHAR             *p_protocol_ver_start;
  CPU_CHAR             *p_protocol_ver_end;
  CPU_INT32U           len;
  CPU_INT32U           protocol_ver;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;

#if (HTTPs_CFG_PERSISTENT_CONN_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_instance);
#endif
  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);

  len = p_conn->RxBufLenRem;
  if (len == 0) {                                               // If there's no more remaining char and the last...
    *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
    return;
  }
  //                                                               Move the pointer to the next meaningful char.
  p_protocol_ver_start = HTTP_StrGraphSrchFirst(p_conn->RxBufPtr, len);
  if (p_protocol_ver_start == DEF_NULL) {
    *p_err = HTTPs_ERR_REQ_FORMAT_INVALID;
    return;
  }
  //                                                               Find the end of the request line.
  p_protocol_ver_end = Str_Str_N(p_protocol_ver_start, STR_CR_LF, len);
  if (p_protocol_ver_end == DEF_NULL) {                         // If not found, check to get more data.
    if (p_conn->RxBufPtr != p_conn->BufPtr) {
      *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
    } else {
      *p_err = HTTPs_ERR_REQ_FORMAT_INVALID;
    }
    return;
  }

  len = p_protocol_ver_end - p_protocol_ver_start;
  //                                                               Try to match the Protocol version string.
  protocol_ver = HTTP_Dict_KeyGet(HTTP_Dict_ProtocolVer,
                                  HTTP_Dict_ProtocolVerSize,
                                  p_protocol_ver_start,
                                  DEF_YES,
                                  len);

  //                                                               Validate the DictionaryKey search results
  if (protocol_ver == HTTP_DICT_KEY_INVALID) {
    p_conn->ProtocolVer = HTTP_PROTOCOL_VER_UNKNOWN;
  } else {
    p_conn->ProtocolVer = (HTTP_PROTOCOL_VER)protocol_ver;
  }

  switch (p_conn->ProtocolVer) {
    case HTTP_PROTOCOL_VER_0_9:
      HTTPs_STATS_INC(p_ctr_stats->Req_StatProtocolVer0_9Ctr);
      break;

    case HTTP_PROTOCOL_VER_1_0:
      HTTPs_STATS_INC(p_ctr_stats->Req_StatProtocolVer1_0Ctr);
      break;

    case HTTP_PROTOCOL_VER_1_1:
#if (HTTPs_CFG_PERSISTENT_CONN_EN == DEF_ENABLED)
      if (p_cfg->ConnPersistentEn == DEF_ENABLED) {
        DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_CONN_PERSISTENT);
      }
#endif
      HTTPs_STATS_INC(p_ctr_stats->Req_StatProtocolVer1_1Ctr);
      break;

    case HTTP_PROTOCOL_VER_UNKNOWN:
    default:
      HTTPs_STATS_INC(p_ctr_stats->Req_StatProtocolVerUnsupportedCtr);
      p_conn->ProtocolVer = HTTP_PROTOCOL_VER_1_1;
      *p_err = HTTPs_ERR_REQ_PROTOCOL_VER_NOT_SUPPORTED;
      return;
  }
  //                                                               Update the RxBuf ptr.
  p_conn->RxBufLenRem -= (p_protocol_ver_end - p_conn->RxBufPtr) + 2;
  p_conn->RxBufPtr = p_protocol_ver_end + 2;

  *p_err = HTTPs_ERR_NONE;
}

/****************************************************************************************************//**
 *                                           HTTPsReq_HdrParse()
 *
 * @brief    (1) Parse request headers:
 *               - (a) Analyze fields contained in the field header section
 *               - (b) Store field value:
 *                   - (1) Store Content type field
 *                       - (a) Content type application
 *                       - (b) Content type multipart
 *                   - (2) Content length*
 *                   - (3) Host
 *               - (c) Update receive connection parameters
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) RFC #2616, Section "5.3 Request Header Fields" describe the request header field definitions
 *
 *               - (a) The request-header fields allow the client to pass additional information about the
 *                       request, and about the client itself, to the server. These fields act as request
 *                       modifiers, with semantics equivalent to the parameters on a programming language
 *                       method invocation.
 *
 *                       request-header = Accept
 *                       | Accept-Charset
 *                       | Accept-Encoding
 *                       | Accept-Language
 *                       | Authorization
 *                       | Expect
 *                       | From
 *                       | Host
 *                       | If-Match
 *                       | If-Modified-Since
 *                       | If-None-Match
 *                       | If-Range
 *                       | If-Unmodified-Since
 *                       | Max-Forwards
 *                       | Proxy-Authorization
 *                       | Range
 *                       | Referer
 *                       | TE
 *                       | User-Agent
 *
 *                       Request-header field names can be extended reliably only in combination with a change
 *                       in the protocol version. However, new or experimental header fields MAY be given the
 *                       semantics of request- header fields if all parties in the communication recognize
 *                       them to be request-header fields. Unrecognized header fields are treated as
 *                       entity-header fields.
 *
 *                       @note         (3) HTML 4.01 Specification section "17.13 Form submission" explain how user agents submit
 *                       form data to form processing agents:
 *
 *                           - (a) The content type "application/x-www-form-urlencoded" is the default content type.
 *
 *                           - (b) The content type "multipart/form-data" should be used for submitting forms that contain files,
 *                       non-ASCII data, and binary data.
 *
 *                       The content "multipart/form-data" follows the rules of all multipart MIME data streams as
 *                       outlined in [RFC2045]. The definition of "multipart/form-data" is available at the [IANA]
 *                       registry.
 *
 *                       If the user selected a second (image) file "file2.gif", the user agent might construct the parts as follows:
 *
 *                       Content-Type: multipart/form-data; boundary=AaB03x
 *******************************************************************************************************/
static void HTTPsReq_HdrParse(HTTPs_INSTANCE *p_instance,
                              HTTPs_CONN     *p_conn,
                              HTTPs_ERR      *p_err)
{
  CPU_CHAR             *p_field;
  CPU_CHAR             *p_field_end;
  CPU_CHAR             *p_val;
  CPU_INT32U           field_key;
  HTTP_HDR_FIELD       field;
  CPU_INT16U           len;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
  CPU_CHAR           *p_str;
  HTTP_DICT          *p_field_dict_entry;
  HTTPs_HDR_BLK      *p_req_hdr_blk;
  HTTPs_HDR_VAL_TYPE val_type;
  CPU_BOOLEAN        keep;
#endif
#if (((HTTPs_CFG_FORM_EN == DEF_ENABLED)           \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED)) \
  || (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED))
  HTTPs_INSTANCE_ERRS *p_ctr_errs = DEF_NULL;
#endif

#if ((HTTPs_CFG_FORM_EN == DEF_ENABLED) \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED))
  const HTTP_DICT *p_dictionary;
#endif
#if ((HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)       \
  || (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED) \
  || ((HTTPs_CFG_FORM_EN == DEF_ENABLED)        \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED)))
  const HTTPs_CFG *p_cfg;
#endif

#if  (HTTPs_CFG_HDR_RX_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_instance);
#endif
  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);

#if (((HTTPs_CFG_FORM_EN == DEF_ENABLED)           \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED)) \
  || (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED))
  HTTPs_SET_PTR_ERRS(p_ctr_errs, p_instance);
#endif
#if ((HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)       \
  || (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED) \
  || ((HTTPs_CFG_FORM_EN == DEF_ENABLED)        \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED)))
  p_cfg = p_instance->CfgPtr;
#endif

  //                                                               ------------------- #### NET-35. -------------------

  p_field = p_conn->RxBufPtr;                                   // Ptr should be prev moved to beginning of req hdr.
  while (p_field != DEF_NULL) {                                 // Analyze each field in the req hdr section.
    p_field_end = Str_Str_N(p_field,                            // Find end of the field.
                            STR_CR_LF,
                            p_conn->RxBufLenRem);

    //                                                             ------------------ ANALYZE FIELD -------------------
    if ((p_field_end != DEF_NULL)                               // If the field and val are present.
        && (p_field_end > p_field) ) {
      len = p_field_end - p_field;
      //                                                           Get field key by comparing field name.
      field_key = HTTP_Dict_KeyGet(HTTP_Dict_HdrField,
                                   HTTP_Dict_HdrFieldSize,
                                   p_field,
                                   DEF_NO,
                                   len);
      if (field_key != HTTP_DICT_KEY_INVALID) {
        field = (HTTP_HDR_FIELD)field_key;
#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
        //                                                         Get field dictionary entry.
        p_field_dict_entry = HTTP_Dict_EntryGet(HTTP_Dict_HdrField,
                                                HTTP_Dict_HdrFieldSize,
                                                field);
#endif

        //                                                         ----------------- STO FIELD VALUE ------------------
        switch (field) {
          //                                                       ------------------- CONTENT TYPE -------------------
          case HTTP_HDR_FIELD_CONTENT_TYPE:
            //                                                     Get field val beginning.
            p_val = HTTPsReq_HdrParseValGet(p_field,
                                            HTTP_STR_HDR_FIELD_CONTENT_TYPE_LEN,
                                            p_field_end,
                                            &len);
            if (p_val != DEF_NULL) {
              len = p_field_end - p_val;
              //                                                   Get content type key by comparing field val name.
              field_key = HTTP_Dict_KeyGet(HTTP_Dict_ContentType,
                                           HTTP_Dict_ContentTypeSize,
                                           p_val,
                                           DEF_YES,
                                           len);
              if (field_key != HTTP_DICT_KEY_INVALID) {
                field = (HTTP_HDR_FIELD)field_key;
                p_conn->ReqContentType = (HTTP_CONTENT_TYPE)field;
                switch (field) {
                  //                                               ----------------- CONTENT TYPE APP -----------------
                  case HTTP_CONTENT_TYPE_APP_FORM:
                    HTTPs_STATS_INC(p_ctr_stats->Req_StatContentTypeFormAppRxdCtr);
                    //                                             Add content-type to Conn struct.
                    break;

                  //                                               -------------- CONTENT TYPE MULTIPART --------------
                  case HTTP_CONTENT_TYPE_MULTIPART_FORM:
                    HTTPs_STATS_INC(p_ctr_stats->Req_StatContentTypeFormMultipartRxdCtr);
                    //                                             Add content-type value to Conn struct.

#if ((HTTPs_CFG_FORM_EN == DEF_ENABLED) \
                    && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED))
                    if (p_cfg->FormCfgPtr != DEF_NULL) {
                      if (p_cfg->FormCfgPtr->MultipartEn == DEF_ENABLED) {
                        //                                         ----------------- STO BOUNDARY VAL -----------------
                        //                                         Boundary should be located after content type ...
                        //                                         val (see Note #3b).
                        //                                         Find end of content type val.
                        p_dictionary = HTTP_Dict_EntryGet(HTTP_Dict_ContentType,
                                                          HTTP_Dict_ContentTypeSize,
                                                          HTTP_CONTENT_TYPE_MULTIPART_FORM);
                        p_val = p_val + p_dictionary->StrLen + 1;
                        p_val = HTTP_StrGraphSrchFirst(p_val, len);
                        len = len - (p_val - p_field);

                        //                                         Find beginning of boundary token.
                        p_val = Str_Str_N(p_val,
                                          HTTP_STR_MULTIPART_BOUNDARY,
                                          sizeof(HTTP_STR_MULTIPART_BOUNDARY));

                        if (p_val == DEF_NULL) {
                          *p_err = HTTPs_ERR_REQ_FORMAT_INVALID;
                          return;
                        }

                        //                                         Boundary located after '='.
                        p_val = Str_Char_N(p_val, len, ASCII_CHAR_EQUALS_SIGN);
                        if (p_val == DEF_NULL) {
                          *p_err = HTTPs_ERR_REQ_FORMAT_INVALID;
                          return;
                        }
                        p_val++;                                // Remove space before boundary val.
                        p_val = HTTP_StrGraphSrchFirst(p_val,
                                                       len);
                        len = p_field_end - p_val;

                        if (len > HTTPs_FORM_BOUNDARY_STR_LEN_MAX) {
                          *p_err = HTTPs_ERR_REQ_FORMAT_INVALID;
                          return;
                        }

                        //                                         Copy boundary val to Conn struct.
                        Str_Copy_N(p_conn->FormBoundaryPtr,
                                   p_val,
                                   len);
                        //                                         Make sure to create a string.
                        p_conn->FormBoundaryPtr[len] = ASCII_CHAR_NULL;

                        p_conn->FormBoundaryLen = len;
                      }
                    } else {
                      HTTPs_ERR_INC(p_ctr_errs->Req_ErrBodyFormNotEn);
                    }
#endif
                    break;

                  case HTTP_CONTENT_TYPE_UNKNOWN:
                    HTTPs_STATS_INC(p_ctr_stats->Req_StatContentTypeUnknownRxdCtr);
                    break;

                  default:
                    HTTPs_STATS_INC(p_ctr_stats->Req_StatContentTypeOtherRxdCtr);
                    break;
                }
              }
            } else {                                            // Should not occurs.
              *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
              return;
            }
            break;

          //                                                       ------------------- CONTENT LEN --------------------
          case HTTP_HDR_FIELD_CONTENT_LEN:
            //                                                     Find content len strg val.
            p_val = HTTPsReq_HdrParseValGet(p_field,
                                            HTTP_STR_HDR_FIELD_CONTENT_LEN_LEN,
                                            p_field_end,
                                            &len);

            //                                                     Convert and copy val to Conn struct.
            p_conn->ReqContentLen = Str_ParseNbr_Int32U(p_val, 0, DEF_NBR_BASE_DEC);
            break;

          //                                                       ----------------------- HOST -----------------------
          case HTTP_HDR_FIELD_HOST:
#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
            //                                                     Find beginning of host string val.
            p_val = HTTPsReq_HdrParseValGet(p_field,
                                            HTTP_STR_HDR_FIELD_HOST_LEN,
                                            p_field_end,
                                            &len);

            len = DEF_MIN(len, p_cfg->HostNameLenMax);

            //                                                     Copy host name val in Conn struct.
            (void)Str_Copy_N(p_conn->HostPtr, p_val, len);
            //                                                     Make sure to create a string.
            p_conn->HostPtr[len] = ASCII_CHAR_NULL;
#endif
            break;

          //                                                       -------------------- CONNECTION --------------------
          case HTTP_HDR_FIELD_CONN:
            //                                                     Find beginning of connection hdr string val.
            p_val = HTTPsReq_HdrParseValGet(p_field,
                                            HTTP_STR_HDR_FIELD_CONN_LEN,
                                            p_field_end,
                                            &len);
            if (p_val != DEF_NULL) {
              len = p_field_end - p_val;
              //                                                   Get connection key by comparing field val name.
              field_key = HTTP_Dict_KeyGet(HTTP_Dict_HdrFieldConnVal,
                                           HTTP_Dict_HdrFieldConnValSize,
                                           p_val,
                                           DEF_NO,
                                           len);
              if (field_key != HTTP_DICT_KEY_INVALID) {
                switch (field_key) {
                  case HTTP_HDR_FIELD_CONN_CLOSE:
                    DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_CONN_PERSISTENT);
                    break;

                  case HTTP_HDR_FIELD_CONN_PERSISTENT:
                  default:
                    break;
                }
              }
            }
            break;

          default:
#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
            if ((p_cfg->HdrRxCfgPtr != DEF_NULL)
                && (p_cfg->HooksPtr != DEF_NULL)) {
              keep = p_cfg->HooksPtr->OnReqHdrRxHook(p_instance,
                                                     p_conn,
                                                     p_cfg->Hooks_CfgPtr,
                                                     field);

              if ((keep == DEF_YES)
                  && (p_cfg->HdrRxCfgPtr->NbrPerConnMax != LIB_MEM_BLK_QTY_UNLIMITED)) {
                if (p_conn->HdrCtr >= p_cfg->HdrRxCfgPtr->NbrPerConnMax) {
                  *p_err = HTTPs_ERR_REQ_HDR_OVERFLOW;
                  return;
                }
              }

              if (keep == DEF_YES) {
                RTOS_ERR local_err;
                switch (field) {
                  default:
                    val_type = HTTPs_HDR_VAL_TYPE_STR_DYN;                     // Only string data type supported.
                    break;
                }
                //                                                 -------------- GET REQ HDR FIELD BLK ---------------
                p_req_hdr_blk = HTTPsMem_ReqHdrGet(p_instance,
                                                   p_conn,
                                                   (HTTP_HDR_FIELD)field,
                                                   val_type,
                                                   &local_err);
                if (p_req_hdr_blk == DEF_NULL) {
                  *p_err = HTTPs_ERR_REQ_HDR_POOL_GET;
                  return;
                }

                p_val = HTTPsReq_HdrParseValGet(p_field,
                                                p_field_dict_entry->StrLen,
                                                p_field_end,
                                                &len);
                if (p_val != DEF_NULL) {
                  len = p_field_end - p_val;

                  if (len > p_cfg->HdrRxCfgPtr->DataLenMax) {
                    HTTPs_ERR_INC(p_ctr_errs->Req_ErrHdrDataLenInv);
                    *p_err = HTTPS_ERR_REQ_HDR_INVALID_VAL_LEN;
                    return;
                  }

                  //                                               ------------ UPDATE REQ HDR FIELD PARAM ------------
                  Mem_Copy((void *) p_req_hdr_blk->ValPtr,
                           (const void *) p_val,
                           (CPU_SIZE_T) len);
                  //                                               Store only string.
                  p_str = (CPU_CHAR *)p_req_hdr_blk->ValPtr + len;
                  *p_str = ASCII_CHAR_NULL;
                  p_req_hdr_blk->ValLen = len + 1;
                } else {
                  *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
                  return;
                }
              }
            }
#endif
            break;
        }
      }

      //                                                           --------------- UPDATE RX CONN PARAM ---------------
      p_field_end += STR_CR_LF_LEN;
      p_conn->RxBufLenRem -= p_field_end - p_field;
      p_field = p_field_end;
      p_conn->RxBufPtr = p_field;
    } else if (p_field_end == p_field) {                        // All field processed.
      p_conn->RxBufPtr += STR_CR_LF_LEN;
      p_conn->RxBufLenRem -= STR_CR_LF_LEN;
      *p_err = HTTPs_ERR_NONE;
      return;
    } else {                                                    // More data req'd to complete processing.
      if (p_conn->RxBufPtr != p_conn->BufPtr) {                 // Check if the buffer is not full.
        *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
      } else {
        *p_err = HTTPs_ERR_REQ_FORMAT_INVALID;
      }
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                       HTTPsReq_HdrParseFieldValGet()
 *
 * @brief    Get the beginning of a field value.
 *
 * @param    p_field         Pointer to the beginning of the field line.
 *
 * @param    field_len       Field length.
 *
 * @param    p_field_end     Pointer to the end of the field line.
 *
 * @param    p_len_rem       Pointer to a variable that will receive the remaining length.
 *
 * @return   Pointer to the beginning of the field value.
 *
 * @note     (1) RFC #2616, section "4.2 Message Headers" describe how Header field should be formated.
 *
 *               - (a) HTTP header fields, which include general-header (section 4.5), request-header
 *                   (section 5.3), response-header (section 6.2), and entity-header (section 7.1)
 *                   fields, follow the same generic format as that given in Section 3.1 of RFC 822 [9].
 *                   Each header field consists of a name followed by a colon (":") and the field value.
 *                   Field names are case-insensitive. The field value MAY be preceded by any amount of
 *                   LWS, though a single SP is preferred. Header fields can be extended over multiple
 *                   lines by preceding each extra line with at least one SP or HT. Applications ought
 *                   to follow "common form", where one is known or indicated, when generating HTTP
 *                   constructs, since there might exist some implementations that fail to accept
 *                   anything
 *
 *                   beyond the common forms.
 *
 *                           message-header = field-name ":" [ field-value ]
 *                           field-name     = token
 *                           field-value    = *( field-content | LWS )
 *                           field-content  = <the OCTETs making up the field-value
 *                                           and consisting of either *TEXT or combinations
 *                                           of token, separators, and quoted-string>
 *******************************************************************************************************/
static CPU_CHAR *HTTPsReq_HdrParseValGet(CPU_CHAR   *p_field,
                                         CPU_INT16U field_len,
                                         CPU_CHAR   *p_field_end,
                                         CPU_INT16U *p_len_rem)
{
  CPU_INT16U len;
  CPU_CHAR   *p_val;

  p_val = p_field + field_len;
  len = (p_field_end - p_val);

  p_val = Str_Char_N(p_val, len, ASCII_CHAR_COLON);             // Field val located after ':' (see Note #1a).
  p_val++;

  len = (p_field_end - p_val);
  p_val = HTTP_StrGraphSrchFirst(p_val, len);                   // Remove blank space before field value.

  *p_len_rem = (p_field_end - p_val);

  return (p_val);
}

/****************************************************************************************************//**
 *                                           HTTPsReq_BodyForm()
 *
 * @brief    (1) Receive and process post data inside received form:
 *               - (a) Initialize connection parameters to parse post data.
 *               - (b) Parse received data.
 *                   - (1) Parse application post data.
 *                   - (2) Parse multipart   post data.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if Form processing is done.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) The ReqContentType should be validated before this function. Otherwise, an INTERNAL_ERROR
 *               will be triggered.
 *               Valid Content-Type(s) are:
 *                   - HTTPs_CONTENT_TYPE_REQ_APP ||
 *                   - HTTPs_CONTENT_TYPE_REQ_MULTIPART
 *
 * @note     (2) Receive all post data is required before returning response since network buffers are not
 *               freed before reading it and the stack could come locked if it run out of free buffer.
 *******************************************************************************************************/
#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsReq_BodyForm(HTTPs_INSTANCE *p_instance,
                                     HTTPs_CONN     *p_conn,
                                     HTTPs_ERR      *p_err)
{
  HTTPs_INSTANCE_ERRS *p_ctr_err = DEF_NULL;
  CPU_BOOLEAN         done;

  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  done = DEF_YES;

  switch (p_conn->State) {
    //                                                             ----------------- PARSE DATA RX'D ------------------
    case HTTPs_CONN_STATE_REQ_BODY_FORM_APP_PARSE:
      //                                                           --------------- PARSE APP POST DATA ----------------
      done = HTTPsReq_BodyFormAppParse(p_instance, p_conn, p_err);
      break;

    case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_INIT:
    case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_PARSE:
    case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_OPEN:
    case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_WR:
      //                                                           ------------ PARSE MULTIPART POST DATA -------------
#if (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED)
      done = HTTPsReq_BodyFormMultipartParse(p_instance, p_conn, p_err);
#else
      HTTPs_ERR_INC(p_ctr_err->Req_ErrBodyFormMultipartNotEn);
      *p_err = HTTPs_ERR_CFG_INVALID_FORM_MULTIPART_EN;
#endif
      break;

    default:
      HTTPs_ERR_INC(p_ctr_err->Req_ErrBodyStateUnknownCtr);
      *p_err = HTTPs_ERR_STATE_UNKNOWN;
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE, DEF_NO);
      break;
  }

  return (done);
}
#endif

/****************************************************************************************************//**
 *                                       HTTPsReq_BodyFormAppParse()
 *
 * @brief    (1) Parse form content types application:
 *               - (a) Remove undesirable charters
 *               - (b) Validate that the key pair value is completely present.
 *               - (c) Parse control key pair value.
 *               - (d) Update connection control parameters and state.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if Form parsing is finished.
 *           DEF_NO,  otherwise.
 *
 * @note     (2) HTML 4.01 Specification section "17.13.4 Form content types" describes the application Form
 *               content types:
 *
 *               application/x-www-form-urlencoded
 *
 *               This is the default content type. Forms submitted with this content type must be encoded
 *               as follows:
 *
 *               - (a) Control names and values are escaped. Space characters are replaced by `+', and then
 *                   reserved characters are escaped as described in [RFC1738], section 2.2: Non-alphanumeric
 *                   characters are replaced by '%HH', a percent sign and two hexadecimal digits representing
 *                   the ASCII code of the character. Line breaks are represented as "CR LF" pairs
 *                   (i.e., '%0D%0A').
 *
 *               - (b) The control names/values are listed in the order they appear in the document. The name
 *                   is separated from the value by '=' and name/value pairs are separated from each other
 *                   by '&'.
 *******************************************************************************************************/
#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsReq_BodyFormAppParse(HTTPs_INSTANCE *p_instance,
                                             HTTPs_CONN     *p_conn,
                                             HTTPs_ERR      *p_err)
{
  CPU_CHAR    *p_key_name;
  CPU_CHAR    *p_key_next;
  CPU_INT32U  len_content_rxd;
  CPU_INT16U  len_str;
  CPU_BOOLEAN done;
  CPU_BOOLEAN result;

  done = DEF_NO;
  //                                                               ------------- REMOVE UNDESIRABLE CHAR --------------
  //                                                               Remove possible blank char before first ctrl name.
  p_key_name = HTTP_StrGraphSrchFirst(p_conn->RxBufPtr,
                                      p_conn->RxBufLenRem);
  if (p_key_name == DEF_NULL) {
    *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
    goto exit;
  }

  p_conn->RxBufLenRem -= (p_key_name - p_conn->RxBufPtr);        // Update rem len avail in the rx buf.
  p_conn->ReqContentLenRxd += (p_key_name - p_conn->RxBufPtr);   // Update content len received.
  p_conn->RxBufPtr = p_key_name;                                 // Move rx buf ptr.

  while (done != DEF_YES) {
    //                                                             ----------- VALIDATE CUR KEY/VAL PAIRS -------------
    p_key_next = Str_Char_N(p_key_name,                         // Srch beginning of next key/val pairs.
                            p_conn->RxBufLenRem,
                            ASCII_CHAR_AMPERSAND);

    if (p_key_next == DEF_NULL) {                               // If next key/val pairs not found ...
                                                                // ... determine if all data are received or next ...
                                                                // ... key/val pairs are missing.
      len_content_rxd = p_conn->ReqContentLenRxd
                        + p_conn->RxBufLenRem;

      if (len_content_rxd < p_conn->ReqContentLen) {            // If data are missing ...
        *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;              // ... receive more data.
        goto exit;
      } else {                                                  // If all data received ...
        len_str = p_conn->RxBufLenRem;                          // ... last key/val pairs to parse.
      }
    } else {                                                    // Next key/val pairs found ...
      len_str = (p_key_next - p_key_name);                      // ... parse key/val pairs.
    }

    //                                                             Add key-Value block to list.
    result = HTTPsReq_BodyFormAppKeyValBlkAdd(p_instance,
                                              p_conn,
                                              p_key_name,
                                              len_str,
                                              p_err);
    if (result == DEF_FAIL) {
      goto exit;
    }

    //                                                             ------------ UPDATE CONN CTRLS & STATE -------------
    if (p_key_next != DEF_NULL) {                               // If not the last key/val pairs.
      len_str = p_key_next                                      // Calculate data parsed ...
                - p_key_name                                    // ... include key/val pairs separator.
                + 1;

      p_conn->ReqContentLenRxd += len_str;                      // Inc content len processed.
      p_conn->RxBufLenRem -= len_str;                           // Dec rem len available in rx buffer.
      p_key_name = p_key_next + 1;                              // Set cur key name to next key found.
      p_conn->RxBufPtr = p_key_name;                            // Update Rx buf ptr (i.e. if next key/val not found.)
    } else {                                                    // If last key/val pairs.
      p_conn->ReqContentLenRxd += p_conn->RxBufLenRem;          // Inc content len processed.
      p_conn->RxBufLenRem = 0u;                                 // No rem data avail in rx buffer.

      if (p_conn->ReqContentLenRxd >= p_conn->ReqContentLen) {
        //                                                         If all data received and processed ...
        done = DEF_YES;
      } else {                                                  // If data are missing ...
        *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;              // ... receive more data.
        goto exit;
      }
    }
  }

  *p_err = HTTPs_ERR_NONE;

exit:
  return (done);
}
#endif

/****************************************************************************************************//**
 *                                   HTTPsReq_BodyFormAppKeyValBlkAdd()
 *
 * @brief    Add received Application form entry to the Key-Value Form list.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_str       Pointer to start of string containing the key-value data.
 *
 * @param    str_len     Length of the string.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if key-value block successfully added to list.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
static CPU_BOOLEAN HTTPsReq_BodyFormAppKeyValBlkAdd(HTTPs_INSTANCE *p_instance,
                                                    HTTPs_CONN     *p_conn,
                                                    CPU_CHAR       *p_str,
                                                    CPU_SIZE_T     str_len,
                                                    HTTPs_ERR      *p_err)
{
  const HTTPs_CFG *p_cfg;
  HTTPs_KEY_VAL   *p_key_val;
  CPU_BOOLEAN     result;
  RTOS_ERR        local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_cfg = p_instance->CfgPtr;

  if (p_cfg->FormCfgPtr != DEF_NULL) {
    //                                                             Get the next key-value block available.
    if (p_cfg->FormCfgPtr->NbrPerConnMax != LIB_MEM_BLK_QTY_UNLIMITED) {
      if (p_conn->FormBlkAcquiredCtr >= p_cfg->FormCfgPtr->NbrPerConnMax) {
        *p_err = HTTPs_ERR_KEY_VAL_CFG_POOL_SIZE_INV;
        return (DEF_FAIL);
      }
    }

    p_key_val = HTTPsMem_FormKeyValBlkGet(p_instance, p_conn, &local_err);
    if (p_key_val == DEF_NULL) {                                // If no key-value block available.
      *p_err = HTTPs_ERR_FORM_POOL_GET;
      return (DEF_FAIL);
    }

    result = HTTPsReq_URL_EncodeStrParse(p_instance,
                                         p_conn,
                                         p_key_val,
                                         DEF_NO,
                                         p_str,
                                         str_len);
    if (result == DEF_FAIL) {
      *p_err = HTTPs_ERR_FORM_APP_PARSE_FAULT;
      return (DEF_FAIL);
    }

    if (p_conn->FormDataListPtr == DEF_NULL) {
      p_key_val->NextPtr = DEF_NULL;
    } else {
      p_key_val->NextPtr = p_conn->FormDataListPtr;
    }
    p_conn->FormDataListPtr = p_key_val;
  }

  *p_err = HTTPs_ERR_NONE;

  return (DEF_OK);
}
#endif

/****************************************************************************************************//**
 *                                       HTTPsReq_BodyFormMultipartParse()
 *
 * @brief    Parse form content types multipart.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if Form parsing is finished.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
#if ((HTTPs_CFG_FORM_EN == DEF_ENABLED) \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED))
static CPU_BOOLEAN HTTPsReq_BodyFormMultipartParse(HTTPs_INSTANCE *p_instance,
                                                   HTTPs_CONN     *p_conn,
                                                   HTTPs_ERR      *p_err)
{
  CPU_BOOLEAN done;
  CPU_BOOLEAN file_done;
  CPU_BOOLEAN is_file;

  done = DEF_NO;
  while (done != DEF_YES) {
    switch (p_conn->State) {
      case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_INIT:
      case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_PARSE:
        //                                                         Parse buffer for key-value pairs.
        is_file = HTTPsReq_BodyFormMultipartCtrlParse(p_instance,
                                                      p_conn,
                                                      p_err);
        switch (*p_err) {
          case HTTPs_ERR_NONE:
            if (is_file == DEF_YES) {
              p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_OPEN;
            } else {
              if (p_conn->ReqContentLenRxd >= p_conn->ReqContentLen) {
                done = DEF_YES;
              }
            }
            break;

          case HTTPs_ERR_REQ_MORE_DATA_REQUIRED:
            goto exit;

          default:
            goto exit;
        }
        break;

      case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_OPEN:
      case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_WR:
        file_done = HTTPsReq_BodyFormMultipartFileWr(p_instance,
                                                     p_conn,
                                                     p_err);
        switch (*p_err) {
          case HTTPs_ERR_NONE:
            if (file_done == DEF_YES) {
              p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_INIT;
            } else {
              p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_WR;
            }
            break;

          case HTTPs_ERR_REQ_MORE_DATA_REQUIRED:
            p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_WR;
            goto exit;

          default:
            goto exit;
        }
        break;

      default:
        *p_err = HTTPs_ERR_STATE_UNKNOWN;
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE, DEF_NO);
    }
  }

  *p_err = HTTPs_ERR_NONE;

exit:
  return (done);
}
#endif

/****************************************************************************************************//**
 *                                   HTTPsReq_BodyFormMultipartBoundarySrch()
 *
 * @brief    Search for first boundary in data buffer.
 *
 * @param    p_boundary      Pointer to string that contains the boundary to search for.
 *
 * @param    boundary_len    Boundary length.
 *
 * @param    p_buf           Pointer to buffer where to start to search in.
 *
 * @param    buf_len         Buffer length.
 *
 * @param    p_boundary_sep  Pointer that will be set if the buffer end with possible beginning of a boundary.
 *
 * @return   Pointer to the boundary, if successfully found.
 *           Null pointer, otherwise.
 *******************************************************************************************************/
#if ((HTTPs_CFG_FORM_EN == DEF_ENABLED) \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED))
static CPU_CHAR *HTTPsReq_BodyFormMultipartBoundarySrch(CPU_CHAR   *p_boundary,
                                                        CPU_INT08U boundary_len,
                                                        CPU_CHAR   *p_buf,
                                                        CPU_INT16U buf_len,
                                                        CPU_CHAR   **p_boundary_sep)
{
  CPU_CHAR   *p_boundary_found;
  CPU_CHAR   *p_str;
  CPU_INT16U len;

  PP_UNUSED_PARAM(p_boundary_sep);

  if (buf_len <= 0) {
    *p_boundary_sep = DEF_NULL;
    p_boundary_found = DEF_NULL;
    goto exit;
  }

  p_str = DEF_NULL;
  len = HTTPs_STR_MULTIPART_DATA_START_LEN;

  p_str = HTTPs_StrMemSrch(p_buf,                               // Search for boundary separator in the buffer.
                           buf_len,
                           HTTPs_STR_MULTIPART_DATA_START,
                           len);
  if (p_str == DEF_NULL) {
    if ((p_buf[buf_len - 1] == ASCII_CHAR_CARRIAGE_RETURN)
        || (p_buf[buf_len - 1] == ASCII_CHAR_LINE_FEED)
        || (p_buf[buf_len - 1] == ASCII_CHAR_HYPHEN_MINUS)   ) {
      *p_boundary_sep = p_buf + buf_len - 1;
      p_boundary_found = DEF_NULL;
      goto exit;
    } else {
      *p_boundary_sep = DEF_NULL;
      p_boundary_found = DEF_NULL;
      goto exit;
    }
  }

  *p_boundary_sep = p_str;

  len = buf_len - (p_str - p_buf);
  if (len > boundary_len) {
    p_boundary_found = HTTPs_StrMemSrch(p_str,                  // Search for boundary in the buffer.
                                        len,
                                        p_boundary,
                                        boundary_len);

    p_str = p_buf + buf_len - boundary_len - HTTPs_STR_MULTIPART_DATA_START_LEN;

    //                                                             Boundary is not found :
    if ((p_boundary_found == DEF_NULL)                          // Case when separator "--" is found inside data, ...
        && (*p_boundary_sep < p_str)  ) {                       // ... set separator before end of buffer in case ...
                                                                // ... start of boundary is at the end of buffer.
      *p_boundary_sep = p_str;
    }
    //                                                             Boundary is found :
    if (p_boundary_found != DEF_NULL) {                         // Insure that separator "--" is just before boundary...
      *p_boundary_sep = p_boundary_found - 2;                   // ... and not in data.
    }
  } else {
    p_boundary_found = DEF_NULL;
    goto exit;
  }

exit:
  return (p_boundary_found);
}
#endif

/****************************************************************************************************//**
 *                                   HTTPsReq_BodyFormMultipartCtrlParse()
 *
 * @brief    (1) Parse and store multipart key-value pair data:
 *               - (a) Found Boundary in buffer.
 *               - (b) Found if multipart field is a file.
 *               - (c) Found the filename if it's a file or the key string if it's a key-value pair.
 *               - (d) Acquire Form Key-Value block.
 *               - (c) Store key string or filename string.
 *               - (d) Store value string.
 *
 * @param    p_instance  Pointer to current HTTP server instance.
 *
 * @param    p_conn      Pointer to HTTP server current connection object.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if the current multipart field contains a file.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
#if ((HTTPs_CFG_FORM_EN == DEF_ENABLED) \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED))
static CPU_BOOLEAN HTTPsReq_BodyFormMultipartCtrlParse(HTTPs_INSTANCE *p_instance,
                                                       HTTPs_CONN     *p_conn,
                                                       HTTPs_ERR      *p_err)
{
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
  CPU_CHAR        *p_buf = p_conn->RxBufPtr;
  CPU_CHAR        *p_boundary = DEF_NULL;
  CPU_CHAR        *p_boundary_sep = DEF_NULL;
  CPU_CHAR        *p_key = DEF_NULL;
  CPU_CHAR        *p_filename = DEF_NULL;
  CPU_CHAR        *p_filename_end = DEF_NULL;
  CPU_CHAR        *p_end = DEF_NULL;
  CPU_CHAR        *p_str = DEF_NULL;
  CPU_CHAR        *p_data = DEF_NULL;
  HTTPs_KEY_VAL   *p_key_val = DEF_NULL;
  CPU_CHAR        path_sep;
  CPU_SIZE_T      len = 0;
  CPU_BOOLEAN     is_file = DEF_NO;
  RTOS_ERR        local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  if (p_cfg->FormCfgPtr == DEF_NULL) {
    *p_err = HTTPs_ERR_CFG_NULL_PTR_FORM;
    return (DEF_YES);
  }

  switch (p_conn->State) {
    //                                                             ---- FOUND BOUNDARY IN DATA RECEIVED IN BUFFER -----
    case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_INIT:

      //                                                           Search for boundary token in the buffer.
      p_boundary = HTTPsReq_BodyFormMultipartBoundarySrch(p_conn->FormBoundaryPtr,
                                                          p_conn->FormBoundaryLen,
                                                          p_conn->RxBufPtr,
                                                          p_conn->RxBufLenRem,
                                                          &p_boundary_sep);
      if (p_boundary == DEF_NULL) {
        if (p_conn->RxBufLenRem == p_conn->BufLen) {
          *p_err = HTTPs_ERR_CFG_INVALID_BUF_LEN;
        } else {
          *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
        }
        goto exit;
      }

      if ((p_conn->FormDataListPtr != DEF_NULL)                 // Copy previous value to last key-val block.
          && (p_boundary_sep != p_buf)   ) {
        p_key_val = p_conn->FormDataListPtr;
        if (p_key_val->DataType != HTTPs_KEY_VAL_TYPE_FILE) {
          len = (p_boundary - p_buf) - HTTPs_STR_MULTIPART_DATA_START_LEN - STR_CR_LF_LEN;
          len = DEF_MIN(len, (CPU_SIZE_T)p_cfg->FormCfgPtr->ValLenMax - 1);
          Str_Copy_N(p_key_val->ValPtr, p_buf, len);
          p_key_val->ValPtr[len] = ASCII_CHAR_NULL;
          p_key_val->ValLen = len;
        }
      }

      //                                                           Check if there is data after boundary in buffer.
      len = (p_conn->RxBufPtr + p_conn->RxBufLenRem) - p_boundary;
      if (len < (p_conn->FormBoundaryLen + HTTPs_STR_MULTIPART_LAST_LEN)) {
        p_conn->ReqContentLenRxd += (p_boundary_sep - p_buf);
        p_conn->RxBufLenRem -= (p_boundary_sep - p_buf);
        p_conn->RxBufPtr += (p_boundary_sep - p_buf);
        *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
        goto exit;
      }

      p_str = p_boundary + p_conn->FormBoundaryLen;

      p_str = Str_Str_N(p_str,                                  // Find last boundary of multipart section.
                        HTTPs_STR_MULTIPART_LAST,
                        p_conn->RxBufLenRem);
      if ((p_str != DEF_NULL)
          && (p_str == (p_boundary + p_conn->FormBoundaryLen))) {
        p_conn->ReqContentLenRxd += p_conn->RxBufLenRem;
        p_conn->RxBufPtr += p_conn->RxBufLenRem;
        p_conn->RxBufLenRem = 0;
        *p_err = HTTPs_ERR_NONE;
        goto exit;
      }

      p_str = p_boundary + p_conn->FormBoundaryLen;

      p_str = Str_Str_N(p_str,                                  // Find end of boundary (CRLF).
                        STR_CR_LF,                              // CRLF found must be just after the boundary.
                        p_conn->RxBufLenRem);
      if ((p_str == DEF_NULL)
          || (p_str != (p_boundary + p_conn->FormBoundaryLen))) {
        *p_err = HTTPs_ERR_FORM_FORMAT_INV;
        goto exit;
      }

      p_str += STR_CR_LF_LEN;

      //                                                           Update Connection parameters.
      p_conn->ReqContentLenRxd += (p_str - p_buf);
      p_conn->RxBufLenRem -= (p_str - p_buf);
      p_conn->RxBufPtr += (p_str - p_buf);

      p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_PARSE;
      break;

    //                                                             -------- FOUND KEY STRING IN DATA RECEIVED ---------
    case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_PARSE:
      p_data = Str_Str_N(p_buf,                                 // Find end of key section.
                         HTTPs_STR_MULTIPART_CTRL_END_SEC,
                         p_conn->RxBufLenRem);
      if (p_data == DEF_NULL) {
        if (p_conn->RxBufLenRem == p_conn->BufLen) {
          *p_err = HTTPs_ERR_CFG_INVALID_BUF_LEN;
        } else {
          *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
        }
        goto exit;
      }

      p_data += HTTPs_STR_MULTIPART_CTRL_END_SEC_LEN;           // Beginning of data.

      len = p_data - p_buf;

      //                                                           Determine if it's a file.
      p_filename = HTTP_Dict_StrKeySrch(HTTP_Dict_MultipartField,
                                        HTTP_Dict_MultipartFieldSize,
                                        HTTP_MULTIPART_FIELD_FILE_NAME,
                                        p_conn->RxBufPtr,
                                        len);
      if (p_filename != DEF_NULL) {
        is_file = DEF_YES;

        len = p_data - p_filename;
        //                                                         Find beginning of filename.
        p_filename = Str_Char_N(p_filename, len, ASCII_CHAR_QUOTATION_MARK);
        if (p_filename == DEF_NULL) {
          *p_err = HTTPs_ERR_FORM_FORMAT_INV;
          goto exit;
        }
        p_filename++;
        //                                                         Find end of filename.
        p_filename_end = Str_Char_N(p_filename, len, ASCII_CHAR_QUOTATION_MARK);
        if (p_filename_end == DEF_NULL) {
          *p_err = HTTPs_ERR_FORM_FORMAT_INV;
          goto exit;
        }
        *p_filename_end = ASCII_CHAR_NULL;
      }

      //                                                           Find key name.
      p_key = HTTP_Dict_StrKeySrch(HTTP_Dict_MultipartField,
                                   HTTP_Dict_MultipartFieldSize,
                                   HTTP_MULTIPART_FIELD_NAME,
                                   p_conn->RxBufPtr,
                                   p_conn->RxBufLenRem);
      if (p_key != DEF_NULL) {
        len = p_data - p_key;
        //                                                         Find beginning of key.
        p_key = Str_Char_N(p_key, len, ASCII_CHAR_QUOTATION_MARK);
        if (p_key == DEF_NULL) {
          *p_err = HTTPs_ERR_FORM_FORMAT_INV;
          goto exit;
        }
        p_key++;
        //                                                         Find end of key name.
        p_end = Str_Char_N(p_key, len, ASCII_CHAR_QUOTATION_MARK);
        if (p_end == DEF_NULL) {
          *p_err = HTTPs_ERR_FORM_FORMAT_INV;
          goto exit;
        }
      }

      if ((p_filename == DEF_NULL)                              // 'filename' or 'name' string MUST be found.
          && (p_key == DEF_NULL)) {
        *p_err = HTTPs_ERR_FORM_FORMAT_INV;
        goto exit;
      }

      //                                                           --------------- ACQUIRE KEY-VALUE BLK --------------
      if (p_cfg->FormCfgPtr->NbrPerConnMax != LIB_MEM_BLK_QTY_UNLIMITED) {
        if (p_conn->FormBlkAcquiredCtr >= p_cfg->FormCfgPtr->NbrPerConnMax) {
          *p_err = HTTPs_ERR_KEY_VAL_CFG_POOL_SIZE_INV;
          goto exit;
        }
      }

      p_key_val = HTTPsMem_FormKeyValBlkGet(p_instance,
                                            p_conn,
                                            &local_err);
      if (p_key_val == DEF_NULL) {                              // If no Key-Value block available.
        *p_err = HTTPs_ERR_FORM_POOL_GET;
        goto exit;                                              // Returns with p_err from previous fnct call.
      } else {
        if (p_conn->FormDataListPtr == DEF_NULL) {
          p_key_val->NextPtr = DEF_NULL;
        } else {
          p_key_val->NextPtr = p_conn->FormDataListPtr;
        }
        p_conn->FormDataListPtr = p_key_val;
      }

      if (p_key != DEF_NULL) {                                  // Copy Key string found to key-value block.
        len = p_end - p_key;
        len = DEF_MIN(len, (CPU_SIZE_T)p_cfg->FormCfgPtr->KeyLenMax - 1);
        Str_Copy_N(p_key_val->KeyPtr, p_key, len);
        p_key_val->KeyPtr[len] = ASCII_CHAR_NULL;
        p_key_val->KeyLen = len;

        if (p_filename == DEF_NULL) {
          p_key_val->DataType = HTTPs_KEY_VAL_TYPE_PAIR;
        }

        p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_INIT;
      }

      if (p_filename != DEF_NULL) {
        p_key_val->DataType = HTTPs_KEY_VAL_TYPE_FILE;

        if (p_key == DEF_NULL) {
          p_key_val->KeyPtr[0] = ASCII_CHAR_NULL;
        }

        switch (p_cfg->FS_Type) {
          case HTTPs_FS_TYPE_NONE:
            path_sep = HTTPs_PATH_SEP_CHAR_DFLT;
            break;

          case HTTPs_FS_TYPE_STATIC:
          case HTTPs_FS_TYPE_DYN:
#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
            path_sep = p_instance->FS_PathSepChar;
            break;
#else
            *p_err = HTTPs_ERR_CFG_INVALID_FS_EN;
            goto exit;
#endif

          default:
            *p_err = HTTPs_ERR_CFG_INVALID_FS_TYPE;
            goto exit;
        }

        HTTPs_StrPathFormat(p_filename,                         // Format file path and store in value string.
                            p_cfg->FormCfgPtr->MultipartFileUploadFolderPtr,
                            p_key_val->ValPtr,
                            p_cfg->FormCfgPtr->ValLenMax,
                            path_sep);

        len = Str_Len_N(p_key_val->ValPtr, p_cfg->FormCfgPtr->ValLenMax);

        p_key_val->ValLen = len;

        p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_OPEN;
      }

      p_conn->ReqContentLenRxd += (p_data - p_buf);
      p_conn->RxBufPtr += (p_data - p_buf);
      p_conn->RxBufLenRem -= (p_data - p_buf);
      break;

    default:
      *p_err = HTTPs_ERR_STATE_UNKNOWN;
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE, DEF_NO);
  }

  *p_err = HTTPs_ERR_NONE;

exit:
  return (is_file);
}
#endif

/****************************************************************************************************//**
 *                                   HTTPsReq_BodyFormMultipartFileWr()
 *
 * @brief    (1) File received in multipart form processing:
 *               - (a) Open file
 *               - (b) Write data to file until boundary is found in buffer.
 *               - (c) Close file, if all file data is received or in case of an error.
 *
 * @param    p_instance  Pointer to current HTTP server instance.
 *
 * @param    p_conn      Pointer to HTTP server current connection object.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if file file writing is done and file is closed.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
#if ((HTTPs_CFG_FORM_EN == DEF_ENABLED) \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED))
static CPU_BOOLEAN HTTPsReq_BodyFormMultipartFileWr(HTTPs_INSTANCE *p_instance,
                                                    HTTPs_CONN     *p_conn,
                                                    HTTPs_ERR      *p_err)
{
  const HTTPs_CFG      *p_cfg = p_instance->CfgPtr;
  const NET_FS_API     *p_fs_api;
  CPU_CHAR             *p_boundary;
  CPU_CHAR             *p_boundary_sep;
  CPU_SIZE_T           len;
  CPU_SIZE_T           tmp;
  CPU_SIZE_T           len_wr;
  CPU_SIZE_T           len_wr_tot;
  CPU_BOOLEAN          fs_op;
  CPU_BOOLEAN          done = DEF_NO;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;

  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);

  switch (p_cfg->FS_Type) {
    case HTTPs_FS_TYPE_NONE:
      p_fs_api = DEF_NULL;
      break;

    case HTTPs_FS_TYPE_STATIC:
#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
      p_fs_api = ((HTTPs_CFG_FS_STATIC *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
#else
      p_fs_api = DEF_NULL;
#endif
      break;

    case HTTPs_FS_TYPE_DYN:
#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
      p_fs_api = ((HTTPs_CFG_FS_DYN *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
#else
      p_fs_api = DEF_NULL;
#endif
      break;

    default:
      *p_err = HTTPs_ERR_CFG_INVALID_FS_TYPE;
      goto exit;
  }

  done = DEF_NO;

  if (p_fs_api == DEF_NULL) {
    HTTPs_STATS_INC(p_ctr_stats->Req_StatFormFileUploadNoFS_Ctr);
  }

  switch (p_conn->State) {
    case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_OPEN:
      if ((p_fs_api != DEF_NULL)
          && (p_cfg->FormCfgPtr->MultipartFileUploadEn == DEF_ENABLED)) {
        if (p_cfg->FormCfgPtr->MultipartFileUploadOverWrEn == DEF_ENABLED) {
          p_conn->DataPtr = p_fs_api->Open(p_conn->FormDataListPtr->ValPtr,
                                           NET_FS_FILE_MODE_CREATE,
                                           NET_FS_FILE_ACCESS_WR);
          if (p_conn->DataPtr == DEF_NULL) {
            *p_err = HTTPs_ERR_FORM_FILE_UPLOAD_OPEN;
            goto exit;
          }
          HTTPs_STATS_INC(p_ctr_stats->Req_StatFormFileUploadOpenedCtr);
        } else {
          p_conn->DataPtr = p_fs_api->Open(p_conn->FormDataListPtr->ValPtr,
                                           NET_FS_FILE_MODE_CREATE_NEW,
                                           NET_FS_FILE_ACCESS_WR);
        }
      }
      p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_WR;
      break;

    case HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_WR:
      p_boundary_sep = DEF_NULL;
      //                                                           Search for boundary token in the buffer.
      p_boundary = HTTPsReq_BodyFormMultipartBoundarySrch(p_conn->FormBoundaryPtr,
                                                          p_conn->FormBoundaryLen,
                                                          p_conn->RxBufPtr,
                                                          p_conn->RxBufLenRem,
                                                          &p_boundary_sep);

      if (p_boundary_sep == DEF_NULL) {                         // Calculate length of data to write in file.
        len = p_conn->RxBufLenRem;
      } else {
        tmp = p_boundary_sep - p_conn->RxBufPtr;
        if (tmp < STR_CR_LF_LEN) {
          len = 0;
        } else {
          len = tmp - STR_CR_LF_LEN;
        }
      }
      //                                                           Write data to the file.
      if ((p_fs_api != DEF_NULL)
          && (p_cfg->FormCfgPtr->MultipartFileUploadEn == DEF_ENABLED)
          && (p_conn->DataPtr != DEF_NULL)   ) {
        len_wr_tot = 0;
        fs_op = DEF_OK;
        while ((fs_op != DEF_FAIL)
               && (len_wr_tot < len)     ) {
          fs_op = p_fs_api->Wr(p_conn->DataPtr,
                               (CPU_INT08U *)p_conn->RxBufPtr,
                               len,
                               &len_wr);

          len_wr_tot += len_wr;
        }

        if (fs_op == DEF_FAIL) {                                // If write operation fault exit with error.
          p_conn->ReqContentLenRxd += p_conn->RxBufLenRem;
          p_conn->RxBufPtr += p_conn->RxBufLenRem;
          p_conn->RxBufLenRem = 0;

          HTTPs_STATS_INC(p_ctr_stats->Req_StatFormFileUploadClosedCtr);
          p_fs_api->Close(p_conn->DataPtr);
          p_conn->DataPtr = DEF_NULL;

          done = DEF_YES;
          *p_err = HTTPs_ERR_FILE_WR_FAULT;
          goto exit;
        }
      } else {
        len_wr_tot = len;
      }

      //                                                           Update Connection parameters.
      p_conn->RxBufPtr += len_wr_tot;
      p_conn->RxBufLenRem -= len_wr_tot;
      p_conn->ReqContentLenRxd += len_wr_tot;

      if (p_boundary == DEF_NULL) {
        p_conn->State = HTTPs_CONN_STATE_REQ_BODY_FORM_MULTIPART_FILE_WR;
        *p_err = HTTPs_ERR_REQ_MORE_DATA_REQUIRED;
        goto exit;
      } else {
        p_conn->RxBufPtr += STR_CR_LF_LEN;
        p_conn->RxBufLenRem -= STR_CR_LF_LEN;
        p_conn->ReqContentLenRxd += STR_CR_LF_LEN;

        if ((p_fs_api != DEF_NULL)
            && (p_cfg->FormCfgPtr->MultipartFileUploadEn == DEF_ENABLED)
            && (p_conn->DataPtr != DEF_NULL)   ) {
          HTTPs_STATS_INC(p_ctr_stats->Req_StatFormFileUploadClosedCtr);
          p_fs_api->Close(p_conn->DataPtr);
        }
        p_conn->DataPtr = DEF_NULL;

        done = DEF_YES;
      }
      break;

    default:
      *p_err = HTTPs_ERR_STATE_UNKNOWN;
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE, DEF_NO);
  }

  *p_err = HTTPs_ERR_NONE;

exit:
  return (done);
}
#endif

/****************************************************************************************************//**
 *                                       HTTPsReq_URL_EncodeStrParse()
 *
 * @brief    Populate a new Key-Value block from a URL Encoded String.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_key_val   Pointer to the Key-Value block to populate.
 *
 * @param    from_query  DEF_YES, if parsing a Query String.
 *                       DEF_NO,  if parsing a HTTP Form.
 *
 * @param    p_str       Pointer to start of the URL encoded string.
 *
 * @param    str_len     Length of the URL encoded string.
 *
 * @return   DEF_OK,   if parsing is successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
#if ((HTTPs_CFG_QUERY_STR_EN == DEF_ENABLED) \
  || ((HTTPs_CFG_FORM_EN == DEF_ENABLED)))
static CPU_BOOLEAN HTTPsReq_URL_EncodeStrParse(HTTPs_INSTANCE *p_instance,
                                               HTTPs_CONN     *p_conn,
                                               HTTPs_KEY_VAL  *p_key_val,
                                               CPU_BOOLEAN    from_query,
                                               CPU_CHAR       *p_str,
                                               CPU_SIZE_T     str_len)
{
  const HTTPs_CFG *p_cfg;
  CPU_CHAR        *p_key;
  CPU_CHAR        *p_val;
  CPU_CHAR        *p_str_sep;
  CPU_SIZE_T      key_len;
  CPU_SIZE_T      val_len;
  CPU_SIZE_T      key_len_cfg;
  CPU_SIZE_T      val_len_cfg;
  CPU_BOOLEAN     result;

  p_cfg = p_instance->CfgPtr;

  if (from_query == DEF_YES) {
    if (p_cfg->QueryStrCfgPtr != DEF_NULL) {
      key_len_cfg = p_cfg->QueryStrCfgPtr->KeyLenMax - 1;
      val_len_cfg = p_cfg->QueryStrCfgPtr->ValLenMax - 1;
    } else {
      p_conn->ErrCode = HTTPs_ERR_CFG_NULL_PTR_QUERY_STR;
      return (DEF_FAIL);
    }
  } else {
    if (p_cfg->FormCfgPtr != DEF_NULL) {
      key_len_cfg = p_cfg->FormCfgPtr->KeyLenMax - 1;
      val_len_cfg = p_cfg->FormCfgPtr->ValLenMax - 1;
    } else {
      p_conn->ErrCode = HTTPs_ERR_CFG_NULL_PTR_FORM;
      return (DEF_FAIL);
    }
  }

  //                                                               Find separator "=".
  p_str_sep = Str_Char_N(p_str, str_len, ASCII_CHAR_EQUALS_SIGN);

  p_str[str_len] = ASCII_CHAR_NULL;

  if (p_str_sep != DEF_NULL) {
    p_val = p_str_sep + 1;
    p_key = p_str;
  } else {
    p_val = p_str_sep;
    p_key = DEF_NULL;
  }

  //                                                               --------------- COPY CTRL NAME & VAL ---------------
  //                                                               Get and copy the ctrl name.
  if (p_key != DEF_NULL) {
    key_len = p_str_sep - p_key;
    key_len = DEF_MIN(key_len, key_len_cfg);

    Str_Copy_N(p_key_val->KeyPtr,
               p_key,
               key_len);

    p_key_val->DataType = HTTPs_KEY_VAL_TYPE_PAIR;
  } else {
    key_len = 0;
    p_key_val->DataType = HTTPs_KEY_VAL_TYPE_VAL;
  }

  p_key_val->KeyPtr[key_len] = ASCII_CHAR_NULL;
  p_key_val->KeyLen = key_len;

  //                                                               Get and copy the value.
  val_len = p_str + str_len - p_val;
  val_len = DEF_MIN(val_len, val_len_cfg);

  Str_Copy_N(p_key_val->ValPtr,
             p_val,
             val_len);

  p_key_val->ValPtr[val_len] = ASCII_CHAR_NULL;
  p_key_val->ValLen = val_len;

  result = HTTP_URL_DecodeReplaceStr(p_key_val->KeyPtr,
                                     &p_key_val->KeyLen);
  if (result == DEF_FAIL) {
    return (DEF_FAIL);
  }

  result = HTTP_URL_DecodeReplaceStr(p_key_val->ValPtr,
                                     &p_key_val->ValLen);
  if (result == DEF_FAIL) {
    return (DEF_FAIL);
  }

  return (DEF_OK);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
