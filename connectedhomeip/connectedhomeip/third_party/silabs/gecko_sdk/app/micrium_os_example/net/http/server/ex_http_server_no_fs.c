/***************************************************************************//**
 * @file
 * @brief HTTP Server Instance Without FS Example
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

/****************************************************************************************************//**
 * @note     (1) This example shows how to initialize Micrium OS HTTP Server, initialize a web
 *               server instance and start it.
 *
 * @note     (2) This example does not require a File System.
 *
 * @note     (3) This file is an example about how to use the HTTP Server, It may not cover all case
 *               needed by a real application. Also some modification might be needed, insert the
 *               code to perform the stated actions wherever 'TODO' comments are found.
 *              - (a) For instance this example doesn't manage the link state (plugs and unplugs),
 *                    this can be a problem when switching from a network to another network.
 *              - (b) This example is not fully tested, so it is not guaranteed that all cases are
 *                    covered properly.
 *******************************************************************************************************/

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

#include  <common/include/rtos_utils.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>
#include  <common/include/rtos_path.h>

#include  <net/include/http_server.h>
#include  <net/include/net_cfg_net.h>

#include  "ex_http_server.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  DFLT_PAGE_URL             "/hello_world"
#define  DFLT_PAGE_DATA            "hello world!"

/********************************************************************************************************
 ********************************************************************************************************
 *                                        FUNCTIONS PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN Ex_HTTP_Server_NoFS_ReqHook(const HTTPs_INSTANCE *p_instance,
                                               HTTPs_CONN           *p_conn,
                                               const void           *p_hook_cfg);

static CPU_BOOLEAN Ex_HTTP_Server_NoFS_RespHdrTxHook(const HTTPs_INSTANCE *p_instance,
                                                     HTTPs_CONN           *p_conn,
                                                     const void           *p_hook_cfg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                      HTTP SERVER HOOK CONFIGURATION
 *
 * Note(s): (1)  When the instance is created, an hook function can be called to initialize connection objects used by the instance.
 *               If the hook is not required by the upper application, it can be set as DEF_NULL and no function will be called.
 *               See HTTPs_InstanceInitHook() function for further details.
 *
 *          (2)  Each time a header field other than the default one is received, a hook function is called
 *               allowing to choose which header field(s) to keep for further processing.
 *               If the hook is not required by the upper application, it can be set as DEF_NULL and no function will be called.
 *               See HTTPs_ReqHdrRxHook() function for further details.
 *
 *          (3)  For each new incoming connection request a hook function can be called by the web server to authenticate
 *               the remote connection to accept or reject it. This function can have access to allow stored request header
 *               field.
 *               If the hook is not required by the upper application, it can be set as DEF_NULL and no function will be called.
 *               See HTTPs_ReqHook() function for further details.
 *
 *          (4)  If the upper application want to parse the data received in the request body itself, a hook function is available.
 *               It will be called each time new data are received. The exception is when a POST request with a form is
 *               received. In that case, the HTTP server core will parse the body and saved the data into Key-Value blocks.
 *               If the hook is not required by the upper application, it can be set as DEF_NULL and no function will be called.
 *               See HTTPs_ReqBodyRxHook() function for further details.
 *
 *          (5)  The Signal hook function occurs after the HTTP request has been completely received.
 *               The hook function SHOULD NOT be blocking and SHOULD return quickly. A time consuming function will
 *               block the processing of the other connections and reduce the HTTP server performance.
 *               In case the request processing is time consuming, the Poll hook function SHOULD be enabled to
 *               allow the server to periodically verify if the upper application has finished the request processing.
 *               If the hook is not required by the upper application, it can be set as DEF_NULL and no function will be called.
 *               See HTTPs_ReqRdySignalHook() function for further details.
 *
 *          (6)  The Poll hook function SHOULD be enable in case the request processing require lots of time. It
 *               allows the HTTP server to periodically poll the upper application and verify if the request processing
 *               has finished.
 *               If the Poll feature is not required, this field SHOULD be set as DEF_NULL.
 *               See HTTPs_ReqRdyPollHook() function for further details.
 *
 *          (7)  Before an HTTP response message is transmitted, a hook function is called to enable adding header field(s) to
 *               the message before it is sent.
 *               The Header Module must be enabled for this hook to be called. See HTTPs_CFG_HDR in http-s_cfg.h.
 *               If the hook is not required by the upper application, it can be set as DEF_NULL and no function will be called.
 *               See HTTPs_RespHdrTxHook() function for further details.
 *
 *          (8)  The hook function is called by the web server when a token is found. This means the hook
 *               function must fill a buffer with the value of the instance token to be sent.
 *               If the feature is not enabled, this field is not used and can be set as DEF_NULL.
 *               See 'HTTPs_RespTokenValGetHook' for further information.
 *
 *          (9)  To allow the upper application to transmit data with the Chunked Transfer Encoding, a hook function is
 *               available. If defined, it will be called at the moment of the Response body transfer, and it will be called
 *               until the application has transfer all its data.
 *               If the hook is not required by the upper application, it can be set as DEF_NULL and no function will be called.
 *               See HTTPs_RespChunkDataGetHook() function for further details.
 *
 *          (10) Once an HTTP transaction is completed, a hook function can be called to notify the upper application that the
 *               transaction is done. This hook function could be used to free some previously allocated memory.
 *               If the hook is not required by the upper application, it can be set as DEF_NULL and no function will be called.
 *               See HTTPs_TransCompleteHook() function for further details.
 *
 *          (11) When an internal error occurs during the processing of a connection a hook function can be called to
 *               notify the application of the error and to change the behavior such as the status code and the page returned.
 *               If the hook is not required by the upper application, it can be set as DEF_NULL and no function will be called.
 *               See HTTPs_ErrHook() function for further details.
 *
 *          (12) Get error file hook can be called every time an error has occurred when processing a connection.
 *               This function can set the web page that should be transmit instead of the default error page defined
 *               in http-s_cfg.h.
 *               If set to DEF_NULL the default error page will be used for every error.
 *               See HTTPs_ErrFileGetHook() function for further details.
 *
 *          (13) Once a connection is closed a hook function can be called to notify the upper application that a connection
 *               is no more active. This hook function could be used to free some previously allocated memory.
 *               If the hook is not required by the upper application, it can be set as DEF_NULL and no function will be called.
 *               See HTTPs_ConnCloseHook() function for further details.
 *******************************************************************************************************/

const HTTPs_HOOK_CFG Ex_HTTP_Server_HooksNoFS =
{
  .OnInstanceInitHook = DEF_NULL,                               ///< .OnInstanceInitHook    See Note #1.
  .OnReqHdrRxHook = DEF_NULL,                                   ///< .OnReqHdrRxHook        See Note #2.
  .OnReqHook = Ex_HTTP_Server_NoFS_ReqHook,                     ///< .OnReqHook             See Note #3.
  .OnReqBodyRxHook = DEF_NULL,                                  ///< .OnReqBodyRxHook       See Note #4.
  .OnReqRdySignalHook = DEF_NULL,                               ///< .OnReqRdySignalHook    See Note #5.
  .OnReqRdyPollHook = DEF_NULL,                                 ///< .OnReqRdyPollHook      See Note #6.
  .OnRespHdrTxHook = Ex_HTTP_Server_NoFS_RespHdrTxHook,         ///< .OnRespHdrTxHook       See Note #7.
  .OnRespTokenHook = DEF_NULL,                                  ///< .OnRespTokenHook       See Note #8.
  .OnRespChunkHook = DEF_NULL,                                  ///< .OnRespChunkHook       See Note #9.
  .OnTransCompleteHook = DEF_NULL,                              ///< .OnTransCompleteHook   See Note #10.
  .OnErrHook = DEF_NULL,                                        ///< .OnErrHook             See Note #11.
  .OnErrFileGetHook = DEF_NULL,                                 ///< .OnErrFileGetHook      See Note #12.
  .OnConnCloseHook = DEF_NULL                                   ///< .OnConnCloseHook       See Note #13.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         HTTP_ServerBasicInit()
 *
 * @brief  Initializes and starts a basic web server instance. Retrieves WebServer files built-in
 *         static file system.
 *
 * @note   (1) HTTP server instance configurations content MUST remain persistent. You can also use
 *             global variable or constants.
 *
 * @note   (2) Prior to do any call to HTTP module, it must be initialized. This is done by
 *             calling HTTPs_Init(). If the process is successful, the Web server internal data structures are
 *             initialized.
 *
 * @note   (3) Each web server must be initialized before it can be started or stopped. HTTPs_InstanceInit()
 *             is responsible to allocate memory for the instance, initialize internal data structure and
 *             create the web server instance's task.
 *           - (a) The first argument is the instance configuration, which should be modified following you
 *                 requirements. The intance's configuration set the server's port, the number of connection that
 *                 can be accepted, the hooks functions, etc.
 *           - (b) The second argument is the pointer to the instance task configuration. It sets the task priority,
 *                 the stack size of the task, etc.
 *
 * @note   (4) Once a web server instance is initialized, it can be started using HTTPs_InstanceStart() to
 *             become come accessible. This function starts the web server instance's task. Each instance has
 *             is own task and all accepted connection is processed with this single task.
 *             At this point you should be able to access your web server instance using the following
 *             address in your favorite web browser:
 *                 http://<target_ip_address>
 *******************************************************************************************************/
void Ex_HTTP_Server_InstanceCreateNoFS(void)
{
  RTOS_TASK_CFG  *p_cfg_task;
  HTTPs_CFG      *p_cfg_http;
  HTTPs_INSTANCE *p_instance;
  RTOS_ERR       err;

  //                                                               ---------- ALLOC CFG STRUCT (SEE NOTE 1) -----------
  p_cfg_task = (RTOS_TASK_CFG *)Mem_SegAlloc("Ex HTTP Server task cfg struct",
                                             DEF_NULL,
                                             sizeof(RTOS_TASK_CFG),
                                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  p_cfg_http = (HTTPs_CFG *)Mem_SegAlloc("Ex HTTP Server cfg struct",
                                         DEF_NULL,
                                         sizeof(HTTPs_CFG),
                                         &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  p_cfg_http->HdrTxCfgPtr = (HTTPs_HDR_TX_CFG *)Mem_SegAlloc("Ex HTTP Server hdr tx cfg struct",
                                                             DEF_NULL,
                                                             sizeof(HTTPs_HDR_TX_CFG),
                                                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ---------------- PREPARE CFG STRUCT ----------------
  //                                                               Server instance task cfg.
  p_cfg_task->Prio = 21u;
  p_cfg_task->StkSizeElements = 1024u;
  p_cfg_task->StkPtr = DEF_NULL;

  //                                                               Server instance cfg.
  ((HTTPs_HDR_TX_CFG *)p_cfg_http->HdrTxCfgPtr)->NbrPerConnMax = 2u;
  ((HTTPs_HDR_TX_CFG *)p_cfg_http->HdrTxCfgPtr)->DataLenMax = 128u;
  p_cfg_http->HdrRxCfgPtr = DEF_NULL;
  p_cfg_http->QueryStrCfgPtr = DEF_NULL;
  p_cfg_http->FormCfgPtr = DEF_NULL;
  p_cfg_http->TokenCfgPtr = DEF_NULL;
  p_cfg_http->OS_TaskDly_ms = 2u;
  p_cfg_http->SockSel = HTTPs_SOCK_SEL_IPv4;
  p_cfg_http->SecurePtr = DEF_NULL;
  p_cfg_http->Port = 80u;
  p_cfg_http->ConnNbrMax = 15u;
  p_cfg_http->ConnInactivityTimeout_s = 15u;
  p_cfg_http->BufLen = 1460u;
  p_cfg_http->ConnPersistentEn = DEF_ENABLED;
  p_cfg_http->FS_Type = HTTPs_FS_TYPE_NONE;
  p_cfg_http->PathLenMax = 128u;
  p_cfg_http->DfltResourceNamePtr = DFLT_PAGE_URL;
  p_cfg_http->HostNameLenMax = 128u;
  p_cfg_http->HooksPtr = &Ex_HTTP_Server_HooksNoFS;
  p_cfg_http->Hooks_CfgPtr = DEF_NULL;

  Str_Copy_N(&Ex_HTTPs_Name[0], "Simple with no FS", 60);

  p_instance = HTTPs_InstanceInit(p_cfg_http,                   // Instance configuration. See Note #3a.
                                  p_cfg_task,                   // Instance task configuration. See Note #3b.
                                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------ START WEB SERVER INSTANCE -------------
  HTTPs_InstanceStart(p_instance,                               // Instance handle. See Note #4.
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                      Ex_HTTP_Server_NoFS_ReqHook()
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
 *                    Status code will be set automatically to HTTPs_STATUS_UNAUTHORIZED
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
 *             the response by the HTTP Server core with the new location.
 *******************************************************************************************************/
static CPU_BOOLEAN Ex_HTTP_Server_NoFS_ReqHook(const HTTPs_INSTANCE *p_instance,
                                               HTTPs_CONN           *p_conn,
                                               const void           *p_hook_cfg)
{
  CPU_INT16S str_cmp;
  RTOS_ERR   err;

  PP_UNUSED_PARAM(p_hook_cfg);

  //                                                               Set the HTTP response body.
  str_cmp = Str_Cmp_N(p_conn->PathPtr, DFLT_PAGE_URL, p_conn->PathLenMax);
  if (str_cmp == 0) {
    HTTPs_RespBodySetParamStaticData(p_instance,
                                     p_conn,
                                     HTTP_CONTENT_TYPE_HTML,
                                     DFLT_PAGE_DATA,
                                     sizeof(DFLT_PAGE_DATA),
                                     DEF_NO,
                                     &err);
    if (err.Code != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                   Ex_HTTP_Server_NoFS_RespHdrTxHook()
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
static CPU_BOOLEAN Ex_HTTP_Server_NoFS_RespHdrTxHook(const HTTPs_INSTANCE *p_instance,
                                                     HTTPs_CONN           *p_conn,
                                                     const void           *p_hook_cfg)
{
  PP_UNUSED_PARAM(p_hook_cfg);

#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
  HTTPs_HDR_BLK   *p_resp_hdr_blk;
  const HTTPs_CFG *p_cfg;
  CPU_CHAR        *str_data;
  CPU_SIZE_T      str_len;
  RTOS_ERR        err;

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
                                          &err);
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
#endif

  return (DEF_YES);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
