/***************************************************************************//**
 * @file
 * @brief HTTP Server Instance Hooks Example
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/lib_str.h>
#include  <net/include/http_server.h>
#include  <net/include/net_cfg_net.h>

#include  "ex_http_server.h"
#include  "ex_http_server_hooks.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  ERR_404_STR_FILE           "404.html"

#define  INDEX_PAGE_URL             "/index.html"
#define  REST_PAGE_URL              "/list.html"
#define  FORM_SUBMIT_URL            "/form_submit"

#define  FORM_LOGOUT_FIELD_NAME     "Log out"

/********************************************************************************************************
 ********************************************************************************************************
 *                                        GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Counter of error.
static CPU_INT32U Ex_HTTP_Server_HookErrCtr = 0u;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                    Ex_HTTP_Server_InstanceInitHook()
 *
 * @brief  Called to initialized the instance connection objects;
 *         Examples of behaviors that could be implemented :
 *           - (a) Session connections handling initialization:
 *               - (1) Initialize the memory pool and chained list for session connection objects.
 *               - (2) Initialize a periodic timer which check for expired session and release them if
 *                     it is the case.
 *           - (b) Back-end Application Request processing task initialization.
 *
 * @param  p_instance  Pointer to the HTTPs instance object.
 *
 * @param  p_hook_cfg  Pointer to hook configuration object.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_HTTP_Server_InstanceInitHook(const HTTPs_INSTANCE *p_instance,
                                            const void           *p_hook_cfg)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_hook_cfg);

  //                                                               Nothing to do for this example.

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                      Ex_HTTP_Server_ReqHdrRxHook()
 *
 * @brief  Called each time a header field is parsed in a request message. Allows to choose which
 *         additional header field(s) need to be processed by the upper application.
 *
 * @param  p_instance  Pointer to the HTTPs instance object.
 *
 * @param  p_conn      Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg  Pointer to hook configuration object.
 *
 * @param  hdr_field   Type of the header field received.
 *                         See the HTTPs_HDR_FIELD declaration in http-s.h file for all the header types supported.
 *
 * @return  DEF_YES,   If the header field needs to be process.
 *          DEF_NO,    Otherwise.
 *
 * @note   (1) The instance structure is for read-only. It MUST NOT be modified.
 *
 * @note   (2) The connection structure SHOULD NOT be modified. It should be only read to determine if the header
 *             type must be stored.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_HTTP_Server_ReqHdrRxHook(const HTTPs_INSTANCE *p_instance,
                                        const HTTPs_CONN     *p_conn,
                                        const void           *p_hook_cfg,
                                        HTTP_HDR_FIELD       hdr_field)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_hook_cfg);

#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
  switch (hdr_field) {
    case HTTP_HDR_FIELD_COOKIE:
    case HTTP_HDR_FIELD_COOKIE2:
      return(DEF_YES);

    default:
      return(DEF_NO);
  }
#else
  PP_UNUSED_PARAM(hdr_field);

  return (DEF_NO);
#endif
}

/****************************************************************************************************//**
 *                                        Ex_HTTP_Server_ReqHook()
 *
 * @brief  Called after the parsing of an HTTP request message's first line and header(s).
 *         Allows the application to process the information received in the request message.
 *         Examples of behaviors that could be implemented :
 *       - (a) Analyze the Request-URI and validate that the client has the permission to access
 *             the resource. If not, change the Response Status Code to 403 (Forbidden) or 401
 *             (Unauthorized) if an Authentication technique is implemented. In case of a 401
 *             Status, a "WWW-Authenticate" header needs to be added to the response message
 *             (See HTTPs_InstanceRespHdrTx() function)
 *       - (b) Depending on whether the header feature is enabled and which header fields have been
 *             chosen for use (see HTTPs_ReqHdrRxHook() function), different behaviors
 *             are possible. Here are some examples :
 *               - (1) A "Cookie" header is received. The default html page is modified to include
 *                     personalized features for the client.
 *               - (2) An "Authorization" header is received. This validates that the client login is good and
 *                     changes permanently its' access to the folder/file.
 *               - (3) An "If-Modified-Since" header is received. It then validates whether or not the resource
 *                     has been modified since the 'HTTP-date' received with the header. If it was, continue
 *                     with the request processing normally, else change the Status Code to 304 (Not Modified).
 *
 * @param  p_instance  Pointer to the HTTPs instance object.
 *
 * @param  p_conn      Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg  Pointer to hook configuration object.
 *
 * @return  DEF_OK    if the application allows the request to be continue.
 *          DEF_FAIL  otherwise.
 *                          Status code will be set automatically to HTTPs_STATUS_UNAUTHORIZED
 *
 * @note   (1) The instance structure is for read-only. It must not be modified at any point in this hook function.
 *
 * @note   (2) The following connection attributes can be accessed to analyze the connection:
 *           - (a) 'ClientAddr'
 *                     This connection parameter contains the IP address and port used by the remote client to access the
 *                     server instance.
 *           - (b) 'Method'
 *                     - HTTPs_METHOD_GET        Get  request
 *                     - HTTPs_METHOD_POST       Post request
 *                     - HTTPs_METHOD_HEAD       Head request
 *           - (c) 'PathPtr'
 *                     This is a pointer to the string that contains the name of the file requested.
 *           - (d) 'HdrCtr'
 *                     This parameter is a counter of the number of header field that has been stored.
 *           - (e) 'HdrListPtr'
 *                     This parameter is a pointer to the first header field stored. A linked list is created with
 *                     all header field stored.
 *
 * @note   (3) In this hook function, only the under-mentioned connection parameters are allowed
 *             to be modified :
 *           - (a) 'StatusCode'
 *                     See HTTPs_STATUS_CODE declaration in http-s.h for all the status code supported.
 *           - (b) 'PathPtr'
 *                     This is a pointer to the string that contains the name of the file requested. You can change
 *                     the name of the requested file to send another file instead without error.
 *           - (c) 'DataPtr'
 *                     This is a pointer to the data file or data to transmit. This parameter should be null when calling
 *                     this function. If data from memory has to be sent instead of a file, this pointer must be set
 *                     to the location of the data.
 *           - (d) 'RespBodyDataType'
 *                     - HTTPs_BODY_DATA_TYPE_FILE              Open and transmit a file. Value by default.
 *                     - HTTPs_BODY_DATA_TYPE_STATIC_DATA       Transmit data from the memory. Must be set by the hook function.
 *                     - HTTPs_BODY_DATA_TYPE_NONE              No body in response.
 *           - (e) 'DataLen'
 *                     0,                              Default value, will be set when the file is opened.
 *                     Data length,                    Must be set by the data length when transmitting data from
 *                                                     the memory
 *           - (f) 'ConnDataPtr'
 *                     This is a pointer available for the upper application when memory block must be allocated
 *                     to process the connection request. If memory is allocated by the upper application, the memory
 *                     space can be deallocated into another hook function.
 *
 * @note   (4) When the Location of the requested file has changed, besides the Status Code to change (3xx),
 *             the 'PathPtr' parameter needs to be updated. A "Location" header will be added automatically in
 *             the response by the HTTP Server module core with the new location.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_HTTP_Server_ReqHookNoREST(const HTTPs_INSTANCE *p_instance,
                                         HTTPs_CONN           *p_conn,
                                         const void           *p_hook_cfg)
{
  CPU_INT16S str_cmp;
  RTOS_ERR   err_local;

  //                                                               Redirect the REST page example to the default page.
  str_cmp = Str_Cmp_N(p_conn->PathPtr, REST_PAGE_URL, p_conn->PathLenMax);
  if (str_cmp == 0) {
    p_conn->StatusCode = HTTP_STATUS_SEE_OTHER;

    HTTPs_RespBodySetParamNoBody(p_instance, p_conn, &err_local);
    if (err_local.Code != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }

#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
    Str_Copy(p_conn->HostPtr, "");
    Str_Copy(p_conn->PathPtr, INDEX_PAGE_URL);
#else
    Str_Copy(p_conn->PathPtr, INDEX_PAGE_URL);
#endif
  }

  Ex_HTTP_Server_ReqHook(p_instance,
                         p_conn,
                         p_hook_cfg);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                        AppGlobal_Basic_ReqHook()
 *
 * @brief  Called when a HTTP request is received.
 *         Allows to authorized/denied the request based on the URL.
 *
 * @param  p_instance  Pointer to the HTTPs instance object.
 *
 * @param  p_conn      Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg  Pointer to hook configuration object.
 *
 * @return  DEF_OK,   if the request is allowed.
 *          DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_HTTP_Server_ReqHook(const HTTPs_INSTANCE *p_instance,
                                   HTTPs_CONN           *p_conn,
                                   const void           *p_hook_cfg)
{
  CPU_INT16S str_cmp;
  RTOS_ERR   err;

  PP_UNUSED_PARAM(p_hook_cfg);

  str_cmp = Str_Cmp_N(p_conn->PathPtr, FORM_SUBMIT_URL, p_conn->PathLenMax);
  if (str_cmp == 0u) {
    //                                                             Set Parameters to tx response body in chunk.
    HTTPs_RespBodySetParamStaticData(p_instance,
                                     p_conn,
                                     HTTP_CONTENT_TYPE_JSON,
                                     DEF_NULL,
                                     0,
                                     DEF_NO,
                                     &err);
    if (err.Code != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                     Ex_HTTP_Server_ReqBodyRxHook()
 *
 * @brief  Called when body data is received by the HTTPs core. Allows the application to retrieve
 *         body data.
 *
 * @param  p_instance       Pointer to the HTTPs instance object.
 *
 * @param  p_conn           Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg       Pointer to hook configuration object.
 *
 * @param  p_buf            Pointer to the data buffer.
 *
 * @param  buf_size         Size of the data rx available inside the buffer.
 *
 * @param  p_buf_size_used  Pointer to the variable that will received the length of the data consumed by the app.
 *
 * @return  DEF_YES    To continue with the data reception.
 *          DEF_NO     If the application doesn't want to rx data anymore.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_HTTP_Server_ReqBodyRxHook(const HTTPs_INSTANCE *p_instance,
                                         HTTPs_CONN           *p_conn,
                                         const void           *p_hook_cfg,
                                         void                 *p_buf,
                                         const CPU_SIZE_T     buf_size,
                                         CPU_SIZE_T           *p_buf_size_used)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_hook_cfg);
  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(buf_size);
  PP_UNUSED_PARAM(p_buf_size_used);

  //                                                               Nothing to do for this example.

  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                    Ex_HTTP_Server_ReqRdySignalHook()
 *
 * @brief  If defined, this hook function is called after the request has been completely received and
 *         parse by the HTTP server core.
 *
 * @param  p_instance  Pointer to the HTTPs instance object.
 *
 * @param  p_conn      Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg  Pointer to hook configuration object.
 *
 * @param  p_data      Pointer to the first key-value pair received in a form if any.
 *                     DEF_NULL otherwise.
 *
 * @return  DEF_YES, if the response can be sent.
 *          DEF_NO,  if the response cannot be sent after this call and the Poll function MUST be called before
 *                   sending the response (see note #3).
 *
 * @note   (1) This callback function SHOULD NOT be blocking and SHOULD return quickly. A time consuming
 *             function will block the processing of other connections and reduce the HTTP server performance.
 *
 * @note   (2) If the request data received take a while to be processed:
 *           - (a) the processing SHOULD be done in a separate task and not in this callback function to avoid
 *                 blocking other connections.
 *           - (b) the poll callback function SHOULD be used to allow the connection to poll periodically the
 *                 upper application and verify if the request data processing has been completed.
 *                 The 'ConnDataPtr' attribute inside HTTPs_CONN structure can be used to store a
 *                 semaphore pointer related to the completion of the request data processing.
 *                 See 'HTTPs_ReqRdyPollHook()' for more details on poll function.
 *
 * @note   (3) The following connection attributes can be accessed to analyze the connection:
 *           - (a) 'ClientAddr'
 *                     This connection parameter contains the IP address and port used by the remote client to access the
 *                     server instance.
 *           - (b) 'Method'
 *                     - HTTPs_METHOD_GET        Get  request
 *                     - HTTPs_METHOD_POST       Post request
 *                     - HTTPs_METHOD_HEAD       Head request
 *           - (c) 'PathPtr'
 *                     This is a pointer to the string that contains the name of the file requested.
 *           - (d) 'HdrCtr'
 *                     This parameter is a counter of the number of header field that has been stored.
 *           - (e) 'HdrListPtr'
 *                     This parameter is a pointer to the first header field stored. A linked list is created with
 *                     all header field stored.
 *           - (f) 'ConnDataPtr'
 *                     This is a pointer available for the upper application when memory block must be allocated
 *                     to process the connection request. If memory is allocated by the upper application, the memory
 *                     space can be deallocated into another hook function.
 *
 * @note   (4) In this hook function, only the under-mentioned connection parameters are allowed
 *             to be modified :
 *           - (a) 'StatusCode'
 *                     See HTTPs_STATUS_CODE declaration in http-s.h for all the status code supported.
 *           - (b) 'PathPtr'
 *                     This is a pointer to the string that contains the name of the file requested. You can change
 *                     the name of the requested file to send another file instead without error.
 *           - (c) 'DataPtr'
 *                     This is a pointer to the data file or data to transmit. This parameter should be null when calling
 *                     this function. If data from memory has to be sent instead of a file, this pointer must be set
 *                     to the location of the data.
 *           - (d) 'RespBodyDataType'
 *                     - HTTPs_BODY_DATA_TYPE_FILE              Open and transmit a file. Value by default.
 *                     - HTTPs_BODY_DATA_TYPE_STATIC_DATA       Transmit data from the memory. Must be set by the hook function.
 *                     - HTTPs_BODY_DATA_TYPE_NONE              No body in response.
 *           - (e) 'DataLen'
 *                     0,                              Default value, will be set when the file is opened.
 *                     Data length,                    Must be set to the data length when transmitting data from
 *                                                     the memory
 *
 * @note   (5) When the Location of the requested file has change, besides the Status Code to change (3xx),
 *             the 'PathPtr' parameter needs to be update. A "Location" header will be added automatically in
 *             the response by the HTTP Server module core with the new location.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_HTTP_Server_ReqRdySignalHook(const HTTPs_INSTANCE *p_instance,
                                            HTTPs_CONN           *p_conn,
                                            const void           *p_hook_cfg,
                                            const HTTPs_KEY_VAL  *p_data)
{
  HTTPs_KEY_VAL *p_ctrl_var = (HTTPs_KEY_VAL *)p_data;
  CPU_INT16S    str_cmp;
  RTOS_ERR      err_local;

  PP_UNUSED_PARAM(p_hook_cfg);

  while (p_ctrl_var != DEF_NULL) {
    //                                                             -------------- RECEIVED KEY-VALUE TYPE -------------
    if (p_ctrl_var->DataType == HTTPs_KEY_VAL_TYPE_PAIR) {
      str_cmp = Str_Cmp_N(p_ctrl_var->KeyPtr, FORM_LOGOUT_FIELD_NAME, p_ctrl_var->KeyLen);
      if (str_cmp == 0) {
        p_conn->StatusCode = HTTP_STATUS_SEE_OTHER;             // Redirect the page...

        HTTPs_RespBodySetParamNoBody(p_instance, p_conn, &err_local);
        if (err_local.Code != RTOS_ERR_NONE) {
          return (DEF_YES);
        }

#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
        Str_Copy(p_conn->HostPtr, "");
        Str_Copy(p_conn->PathPtr, INDEX_PAGE_URL);
#else
        Str_Copy(p_conn->PathPtr, INDEX_PAGE_URL);
#endif
      }
    } else if (p_ctrl_var->DataType == HTTPs_KEY_VAL_TYPE_FILE) {
      //                                                           Send back in response last file received in post.
      HTTPs_RespBodySetParamFile(p_instance,
                                 p_conn,
                                 p_ctrl_var->ValPtr,
                                 HTTP_CONTENT_TYPE_UNKNOWN,
                                 DEF_NO,
                                 &err_local);
      if (err_local.Code != RTOS_ERR_NONE) {
        return (DEF_YES);
      }
    }

    p_ctrl_var = p_ctrl_var->NextPtr;
  }

  (void)&p_instance;                                            // Prevent 'variable unused' compiler warning.

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                     Ex_HTTP_Server_ReqRdyPollHook()
 *
 * @brief  Called periodically by a connection waiting for the upper application to complete the
 *         request processing.
 *
 * @param  p_instance  Pointer to the HTTPs instance object.
 *
 * @param  p_conn      Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg  Pointer to hook configuration object.
 *
 * @return  DEF_YES, if the response can be sent.
 *          DEF_NO,  if the response cannot be sent after this call and the Poll function MUST be called again
 *                   before sending the response (see note #2).
 *
 * @note   (1) This callback function SHOULD NOT be blocking and SHOULD return quickly. A time consuming
 *             function will block the processing of other connections and reduce the HTTP server performance.
 *             This function will be called periodically by the connection until DEF_YES is returned.
 *
 * @note   (2) The poll callback function SHOULD be used when the request processing takes a while to
 *             be completed. It will allow the server to periodically poll the upper application to verify
 *             if the request processing has finished.
 *             The 'ConnDataPtr' attribute inside the HTTP_CONN structure can be used to store a
 *             semaphore pointer related to the completion of the request processing.
 *             See 'HTTPs_InstanceReqRdySignal()' for more details on post/poll functionality.
 *
 * @note   (3) The following connection attributes can be accessed to analyze the connection:
 *           - (a) 'ClientAddr'
 *                     This connection parameter contains the IP address and port used by the remote client to access the
 *                     server instance.
 *           - (b) 'Method'
 *                     HTTPs_METHOD_GET        Get  request
 *                     HTTPs_METHOD_POST       Post request
 *                     HTTPs_METHOD_HEAD       Head request
 *           - (c) 'PathPtr'
 *                     This is a pointer to the string that contains the name of the file requested.
 *           - (d) 'HdrCtr'
 *                     This parameter is a counter of the number of header field that has been stored.
 *           - (e) 'HdrListPtr'
 *                     This parameter is a pointer to the first header field stored. A linked list is created with
 *                     all header field stored.
 *           - (f) 'ConnDataPtr'
 *                     This is a pointer available for the upper application when memory block must be allocated
 *                     to process the connection request. If memory is allocated by the upper application, the memory
 *                     space can be deallocated into another hook function.
 *
 * @note   (4) In this hook function, only the under-mentioned connection parameters are allowed
 *             to be modified :
 *           - (a) 'StatusCode'
 *                     See HTTPs_STATUS_CODE declaration in http-s.h for all the status code supported.
 *           - (b) 'PathPtr'
 *                     This is a pointer to the string that contains the name of the file requested. You can change
 *                     the name of the requested file to send another file instead without error.
 *           - (c) 'DataPtr'
 *                     This is a pointer to the data file or data to transmit. This parameter should be null when calling
 *                     this function. If data from memory has to be sent instead of a file, this pointer must be set
 *                     to the location of the data.
 *           - (d) 'RespBodyDataType'
 *                     - HTTPs_BODY_DATA_TYPE_FILE              Open and transmit a file. Value by default.
 *                     - HTTPs_BODY_DATA_TYPE_STATIC_DATA       Transmit data from the memory. Must be set by the hook function.
 *                     - HTTPs_BODY_DATA_TYPE_NONE              No body in response.
 *           - (e) 'DataLen'
 *                     0,                              Default value, will be set when the file is opened.
 *                     Data length,                    Must be set to the data length when transmitting data from
 *                                                     the memory
 *
 * @note   (5) When the Location of the requested file has change, besides the Status Code to change (3xx),
 *             the 'PathPtr' parameter needs to be update. A "Location" header will be added automatically in
 *             the response by the HTTP Server module core with the new location.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_HTTP_Server_ReqRdyPollHook(const HTTPs_INSTANCE *p_instance,
                                          HTTPs_CONN           *p_conn,
                                          const void           *p_hook_cfg)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_hook_cfg);

  //                                                               Nothing to do for this example.

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                     Ex_HTTP_Server_RespHdrTxHook()
 *
 * @brief  Called each time the HTTP server is building a response message. Allows for adding header
 *         fields to the response message according to the application needs.
 *
 * @param  p_instance  Pointer to the HTTPs instance object.
 *
 * @param  p_conn      Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg  Pointer to hook configuration object.
 *
 * @return  DEF_YES,    if the header fields are added without running into a error.
 *          DEF_NO,     otherwise.
 *
 * @note   (1) The instance structure MUST NOT be modified.
 *
 * @note   (2) The connection structure MUST NOT be modified manually since the response is about to be
 *             transmitted at this point. The only change to the connection structure should be the
 *             addition of header fields for the response message through the function HTTPs_RespHdrGet().
 *******************************************************************************************************/
CPU_BOOLEAN Ex_HTTP_Server_RespHdrTxHook(const HTTPs_INSTANCE *p_instance,
                                         HTTPs_CONN           *p_conn,
                                         const void           *p_hook_cfg)
{
#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
  HTTPs_HDR_BLK   *p_resp_hdr_blk;
  const HTTPs_CFG *p_cfg;
  CPU_CHAR        *str_data;
  CPU_SIZE_T      str_len;
  RTOS_ERR        err_local;

  PP_UNUSED_PARAM(p_hook_cfg);

  p_cfg = p_instance->CfgPtr;

  if (p_cfg->HdrTxCfgPtr == DEF_NULL) {
    return (DEF_NO);
  }

  switch (p_conn->StatusCode) {
    case HTTP_STATUS_OK:

      if (p_conn->ReqContentType == HTTP_CONTENT_TYPE_HTML) {
        //                                                         --------------- ADD SERVER HDR FIELD ---------------
        //                                                         Get and add header block to the connection.
        p_resp_hdr_blk = HTTPs_RespHdrGet(p_instance,
                                          p_conn,
                                          HTTP_HDR_FIELD_SERVER,
                                          HTTPs_HDR_VAL_TYPE_STR_DYN,
                                          &err_local);
        if (p_resp_hdr_blk == DEF_NULL) {
          return(DEF_FAIL);
        }

        str_data = "uC-HTTP-server";                            // Build Server string value.

        str_len = Str_Len_N(str_data, p_cfg->HdrTxCfgPtr->DataLenMax);

        //                                                         update hdr blk parameter.
        Str_Copy_N(p_resp_hdr_blk->ValPtr,
                   str_data,
                   str_len);

        p_resp_hdr_blk->ValLen = str_len;
      }
      break;

    default:
      break;
  }
#else
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_hook_cfg);
#endif

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                  Ex_HTTP_Server_RespTokenValGetHook()
 *
 * @brief  Called for each ${TEXT_STRING} embedded variable found in a HTML document.
 *
 * @param  p_instance   Pointer to the HTTPs instance object.
 *
 * @param  p_conn       Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg   Pointer to hook configuration object.
 *
 * @param  p_token      Pointer to the string that contains the value of the HTML embedded token.
 *
 * @param  token_len    Length of the embedded token.
 *
 * @param  p_val        Pointer to which buffer token value is copied to.
 *
 * @param  val_len_max  Maximum buffer length.
 *
 * @return  DEF_OK,   if token value copied successfully.
 *          DEF_FAIL, otherwise (see Note #3).
 *
 * @note   (1) The instance structure MUST NOT be modified.
 *
 * @note   (2) The connection structure MUST NOT be modified manually since the response is about to be
 *             transmitted at this point. The only change to the connection structure should be the
 *             addition of header fields for the response message through the function HTTPs_RespHdrGet().
 *
 * @note   (3) If the token replacement failed, the token will be replaced by a line of tilde (~) of
 *             length equal to val_len_max.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_HTTP_Server_RespTokenValGetHook(const HTTPs_INSTANCE *p_instance,
                                               HTTPs_CONN           *p_conn,
                                               const void           *p_hook_cfg,
                                               const CPU_CHAR       *p_token,
                                               CPU_INT16U           token_len,
                                               CPU_CHAR             *p_val,
                                               CPU_INT16U           val_len_max)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_hook_cfg);
  PP_UNUSED_PARAM(token_len);

  if (Str_Cmp_N(p_token, "TEST_NAME", 9) == 0) {
    Str_Copy_N(p_val, &Ex_HTTPs_Name[0], val_len_max);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                  Ex_HTTP_Server_RespChunkDataGetHook()
 *
 * @brief  Called to get the application data to put in the body when transferring in chunk.
 *
 * @param  p_instance   Pointer to the HTTPs instance object.
 *
 * @param  p_conn       Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg   Pointer to hook configuration object.
 *
 * @param  p_buf        Pointer to the buffer to fill.
 *
 * @param  buf_len_max  Maximum length the buffer can contain.
 *
 * @param  p_tx_len     Variable that will received the length written in the buffer.
 *
 * @return  DEF_YES      if there is no more data to send.
 *          DEF_NO       otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN Ex_HTTP_Server_RespChunkDataGetHook(const HTTPs_INSTANCE *p_instance,
                                                HTTPs_CONN           *p_conn,
                                                const void           *p_hook_cfg,
                                                void                 *p_buf,
                                                CPU_SIZE_T           buf_len_max,
                                                CPU_SIZE_T           *p_tx_len)
{
  PP_UNUSED_PARAM(p_hook_cfg);
  PP_UNUSED_PARAM(buf_len_max);

#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
  HTTPs_KEY_VAL   *p_key_val;
  CPU_INT16S      str_cmp;

  str_cmp = Str_Cmp_N(p_conn->PathPtr, FORM_SUBMIT_URL, p_conn->PathLenMax);
  if (str_cmp == 0) {
    //                                                             Construct JSON for user
    Str_Copy(p_buf, "{\"user\": {\"first name\": \"");          // Add First Name field.
    p_key_val = p_conn->FormDataListPtr;
    while (p_key_val != DEF_NULL) {
      str_cmp = Str_Cmp_N(p_key_val->KeyPtr, "firstname", p_cfg->FormCfgPtr->KeyLenMax);
      if (str_cmp == 0) {
        Str_Cat_N(p_buf, p_key_val->ValPtr, p_key_val->ValLen);
        break;
      }
      p_key_val = p_key_val->NextPtr;
    }

    Str_Cat(p_buf, "\", \"last name\":\"");                     // Add Last Name field.
    p_key_val = p_conn->FormDataListPtr;
    while (p_key_val != DEF_NULL) {
      str_cmp = Str_Cmp_N(p_key_val->KeyPtr, "lastname", p_cfg->FormCfgPtr->KeyLenMax);
      if (str_cmp == 0) {
        Str_Cat_N(p_buf, p_key_val->ValPtr, p_key_val->ValLen);
        break;
      }
      p_key_val = p_key_val->NextPtr;
    }
    Str_Cat(p_buf, "\"}}");
  }

  *p_tx_len = Str_Len_N(p_buf, p_cfg->BufLen);
#else
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(p_tx_len);

  CPU_SW_EXCEPTION(; );
#endif

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                   Ex_HTTP_Server_TransCompleteHook()
 *
 * @brief  Called each time an HTTP Transaction has been completed. Allows the upper application
 *         to free some previously allocated memory associated with a request.
 *
 * @param  p_instance  Pointer to the HTTPs instance object.
 *
 * @param  p_conn      Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg  Pointer to hook configuration object.
 *******************************************************************************************************/
void Ex_HTTP_Server_TransCompleteHook(const HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN           *p_conn,
                                      const void           *p_hook_cfg)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_hook_cfg);

  //                                                               Nothing to do for this example.
}

/****************************************************************************************************//**
 *                                     Ex_HTTP_Server_ErrFileGetHook()
 *
 * @brief  (1) Called when the response status code has been changed to an error status code. The
 *             change could be the result of the request processing in the HTTPs_ReqHook()
 *             callback function or the result of an internal error in the HTTP Server core.
 *       - (2) This function is intended to set the name of the file which will be sent with the response message.
 *             If no file is set, a default status page will be sent including the status code number and the
 *             reason phrase.
 *
 * @param  p_hook_cfg      Pointer to hook configuration object.
 *
 * @param  status_code     Status code, number of the response message.
 *
 * @param  p_file_str      Pointer to the buffer where the filename string must be copied.
 *
 * @param  file_len_max    Maximum length of the filename.
 *
 * @param  p_file_type     Pointer to the variable where the file type must be copied:
 *                             HTTPs_FILE_TYPE_FS,           when file is include in a File System.
 *                             HTTPs_FILE_TYPE_STATIC_DATA,  when file is a simple data stream inside a memory
 *                                                           block.
 *
 * @param  p_content_type  Content type of the body.
 *                         If the data is a File. the content type doesn't need to be set. It will be
 *                         set according to the file extension.
 *                         If the data is Static Data, the parameter MUST be set.
 *
 * @param  p_data          Pointer to the data memory block, if file type is HTTPs_FILE_TYPE_STATIC_DATA.
 *                         DEF_NULL,                         otherwise
 *
 * @param  p_data_len      Pointer to variable holding
 *                         the length of the data,           if file type is HTTPs_FILE_TYPE_STATIC_DATA.
 *                         DEF_NULL,                         otherwise
 *
 * @note   (1) If the configured file doesn't exist the instance will transmit the default web page instead,
 *             defined by HTTPs_CFG_HTML_DFLT_ERR_PAGE in http-s_cfg.h
 *******************************************************************************************************/
void Ex_HTTP_Server_ErrFileGetHook(const void           *p_hook_cfg,
                                   HTTP_STATUS_CODE     status_code,
                                   CPU_CHAR             *p_file_str,
                                   CPU_INT32U           file_len_max,
                                   HTTPs_BODY_DATA_TYPE *p_file_type,
                                   HTTP_CONTENT_TYPE    *p_content_type,
                                   void                 **p_data,
                                   CPU_INT32U           *p_data_len)
{
  PP_UNUSED_PARAM(p_hook_cfg);

  switch (status_code) {
    case HTTP_STATUS_NOT_FOUND:
      Str_Copy_N(p_file_str, ERR_404_STR_FILE, file_len_max);
      *p_file_type = HTTPs_BODY_DATA_TYPE_FILE;
      return;

    default:
      Str_Copy_N(p_file_str, HTTPs_HTML_DLFT_ERR_STR_NAME, file_len_max);
      *p_data = HTTPs_CFG_HTML_DFLT_ERR_PAGE;
      *p_data_len = HTTPs_HTML_DLFT_ERR_LEN;
      *p_file_type = HTTPs_BODY_DATA_TYPE_STATIC_DATA;
      *p_content_type = HTTP_CONTENT_TYPE_HTML;
      return;
  }
}

/****************************************************************************************************//**
 *                                        Ex_HTTP_Server_ErrHook()
 *
 * @brief  Called each time an internal error occurs.
 *
 * @param  p_instance  Pointer to the HTTPs instance object.
 *
 * @param  p_conn      Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg  Pointer to hook configuration object.
 *
 * @param  err         Internal error that occurred:
 *                     See HTTPs_ERR declaration in http-s.h for all the error codes possible.
 *
 * @note   (1) The instance structure is for read-only. It must not be modified at any point in this hook function.
 *
 * @note   (2) The following connection attributes can be accessed to analyze the connection:
 *           - (a) 'ClientAddr'
 *                     This connection parameter contains the IP address and port used by the remote client to access the
 *                     server instance.
 *           - (b) 'Method'
 *                     - HTTPs_METHOD_GET        Get  request
 *                     - HTTPs_METHOD_POST       Post request
 *                     - HTTPs_METHOD_HEAD       Head request
 *           - (c) 'PathPtr'
 *                     This is a pointer to the string that contains the name of the file requested.
 *           - (d) 'HdrCtr'
 *                     This parameter is a counter of the number of header field that has been stored.
 *           - (e) 'HdrListPtr'
 *                     This parameter is a pointer to the first header field stored. A linked list is created with
 *                     all header field stored.
 *
 * @note   (3) In this hook function, only the under-mentioned connection parameters are allowed
 *             to be modified :
 *           - (a) 'StatusCode'
 *                     See HTTPs_STATUS_CODE declaration in http-s.h for all the status code supported.
 *           - (b) 'PathPtr'
 *                     This is a pointer to the string that contains the name of the file requested. You can change
 *                     the name of the requested file to send another file instead without error.
 *           - (c) 'DataPtr'
 *                     This is a pointer to the data file or data to transmit. This parameter should be null when calling
 *                     this function. If data from memory has to be sent instead of a file, this pointer must be set
 *                     to the location of the data.
 *           - (d) 'RespBodyDataType'
 *                     - HTTPs_BODY_DATA_TYPE_FILE              Open and transmit a file. Value by default.
 *                     - HTTPs_BODY_DATA_TYPE_STATIC_DATA       Transmit data from the memory. Must be set by the hook function.
 *                     - HTTPs_BODY_DATA_TYPE_NONE              No body in response.
 *           - (e) 'DataLen'
 *                     0,                              Default value, will be set when the file is opened.
 *                     Data length,                    Must be set to the data length when transmitting data from
 *                                                     the memory
 *           - (f) 'ConnDataPtr'
 *                     This is a pointer available for the upper application when memory block must be allocated
 *                     to process the connection request. If memory is allocated by the upper application, the memory
 *                     space can be deallocated into another hook function.
 *******************************************************************************************************/
void Ex_HTTP_Server_ErrHook(const HTTPs_INSTANCE *p_instance,
                            HTTPs_CONN           *p_conn,
                            const void           *p_hook_cfg,
                            HTTPs_ERR            err)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_hook_cfg);
  PP_UNUSED_PARAM(err);

  Ex_HTTP_Server_HookErrCtr++;
}

/****************************************************************************************************//**
 *                                     Ex_HTTP_Server_ConnCloseHook()
 *
 * @brief  Called each time a connection is being closed. Allows the upper application to free some
 *         previously allocated memory.
 *
 * @param  p_instance  Pointer to the HTTPs instance object.
 *
 * @param  p_conn      Pointer to the HTTPs connection object.
 *
 * @param  p_hook_cfg  Pointer to hook configuration object.
 *
 * @note   (1) The instance structure is for read-only. It MUST NOT be modified.
 *******************************************************************************************************/
void Ex_HTTP_Server_ConnCloseHook(const HTTPs_INSTANCE *p_instance,
                                  HTTPs_CONN           *p_conn,
                                  const void           *p_hook_cfg)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_hook_cfg);

  //                                                               Nothing to do for this example.
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
