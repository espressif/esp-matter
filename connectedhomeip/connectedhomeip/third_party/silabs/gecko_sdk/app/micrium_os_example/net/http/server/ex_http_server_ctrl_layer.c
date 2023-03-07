/***************************************************************************//**
 * @file
 * @brief HTTP Server Instance With Ctrl Layer Example
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
 *               server instance and start it. It regroups, using the control layer, a login layer,
 *               a default page app layer and a REST app layer.
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
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/auth.h>
#include  <common/include/lib_str.h>
#include  <common/include/rtos_utils.h>

#include  <net/include/net.h>
#include  <net/include/http_server.h>
#include  <net/include/http_server_fs_port_static.h>
#include  <net/include/http_server_addon_rest.h>
#include  <net/include/http_server_addon_auth.h>
#include  <net/include/http_server_addon_ctrl_layer.h>

#include  "ex_http_server.h"
#include  "ex_http_server_hooks.h"
#include  "ex_http_server_rest_hooks.h"
#include  "files/_404_html.h"
#include  "files/form_html.h"
#include  "files/index_html.h"
#include  "files/list_html.h"
#include  "files/login_html.h"
#include  "files/logo_gif.h"
#include  "files/uc_style_css.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 *
 * Notes: (1) This is the definition of different authentication rights.
 *            The are 28 AUTH_RIGHTs available to be mapped.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  HTTP_USER_ACCESS               AUTH_RIGHT_2

#define  LOGIN_PAGE_URL                "/login.html"
#define  MICRIUM_LOGO_URL              "/logo.gif"
#define  MICRIUM_CSS_URL               "/uc_style.css"
#define  INDEX_PAGE_URL                "/index.html"

#define  LOGIN_PAGE_CMD                "/logme"
#define  LOGOUT_PAGE_CMD               "/logout"
#define  FORM_PAGE_CMD                 "/form_submit"

#define  FORM_USERNAME_FIELD_NAME      "username"
#define  FORM_PASSWORD_FIELD_NAME      "password"

#define  FORM_LOGOUT_FIELD_NAME        "Log out"

#define  USER_NBR_MAX                   10u
#define  USERNAME_LEN_MAX               32u
#define  PASSWORD_LEN_MAX               32u

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_HTTP_ServerUsersInit(void);

//                                                                 -------------------- AUTH HOOKS --------------------
static AUTH_RIGHT Ex_HTTP_Server_AuthRequiredRightsGet(const HTTPs_INSTANCE *p_instance,
                                                       const HTTPs_CONN     *p_conn);

static CPU_BOOLEAN Ex_HTTP_Server_AuthLoginParse(const HTTPs_INSTANCE *p_instance,
                                                 const HTTPs_CONN     *p_conn,
                                                 HTTPs_AUTH_STATE     state,
                                                 HTTPs_AUTH_RESULT    *p_result);

static CPU_BOOLEAN Ex_HTTP_Server_AuthLogoutParse(const HTTPs_INSTANCE *p_instance,
                                                  const HTTPs_CONN     *p_conn,
                                                  HTTPs_AUTH_STATE     state);

static AUTH_RIGHT Ex_HTTP_Server_AuthRequiredRightsGet(const HTTPs_INSTANCE *p_instance,
                                                       const HTTPs_CONN     *p_conn);

static void Ex_HTTP_Server_StaticFS_Prepare(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                      CONTROL LAYER VARIABLES
 *******************************************************************************************************/

//                                                                 ----------- AUTHENTICATION CONFIGURATION -----------
//                                                                 Set Authorization Configuration (Access Right).
HTTPs_AUTHORIZATION_CFG Ex_HTTP_Server_AuthInstCfg = {
  Ex_HTTP_Server_AuthRequiredRightsGet                          // .GetRequiredRights: Authorization check function.
};

//                                                                 Set the Authentication Configuration.
HTTPs_AUTH_CFG Ex_HTTP_Server_AppInstAuthCfg = {
  Ex_HTTP_Server_AuthLoginParse,                                // .ParseLogin: Fnct to parse rx credentials in login.
  Ex_HTTP_Server_AuthLogoutParse                                // .ParseLogout: Fnct to check for rx logout.
};

//                                                                 ------------- CTRL LAYER AUTH INSTANCE -------------
//                                                                 Set Authentication Instance.
HTTPs_CTRL_LAYER_AUTH_INST Ex_HTTP_Server_AuthInst = {
  &HTTPsAuth_CookieHooksCfg,                                    // .HooksPtr: Authentication with Cookie Hooks Cfg.
  (void *)&Ex_HTTP_Server_AuthInstCfg                           // .HooksCfgPtr: Pass the Authorization function.
};

//                                                                 --- CTRL LAYER APP INSTANCE FOR AUTH UNPROTECTED ---
//                                                                 Set the App Instance for the Login part (Unprotected).
HTTPs_CTRL_LAYER_APP_INST Ex_HTTP_Server_AppInstAuthUnprotected = {
  &HTTPsAuth_AppUnprotectedCookieHooksCfg,                      // .HooksPtr: Login App with Cookie Hooks Cfg.
  (void *)&Ex_HTTP_Server_AppInstAuthCfg                        // .HooksCfgPtr: Pass the Application Authentication Cfg
};

//                                                                 ---- CTRL LAYER APP INSTANCE FOR AUTH PROTECTED ----
//                                                                 Set the App Instance for the Login part (Protected).
HTTPs_CTRL_LAYER_APP_INST Ex_HTTP_Server_AppInstAuthProtected = {
  &HTTPsAuth_AppProtectedCookieHooksCfg,                        // .HooksPtr: Login App with Cookie Hooks Cfg.
  (void *)&Ex_HTTP_Server_AppInstAuthCfg                        // .HooksCfgPtr: Pass the Application Authentication Cfg
};

//                                                                 --------- CTRL LAYER APP INSTANCE FOR REST ---------
HTTPs_REST_CFG Ex_HTTP_Server_RestCfg = {
  0u                                                            // Simply the list index REST is going to use.
};

//                                                                 Set the Application Instance for the REST part.
HTTPs_CTRL_LAYER_APP_INST Ex_HTTP_Server_AppInst_REST = {
  (HTTPs_CTRL_LAYER_APP_HOOKS *)&HTTPsCtrlLayer_REST_App,       // .HooksPtr: REST Application Hooks Cfg.
  (void *)&Ex_HTTP_Server_RestCfg                               // .HooksCfgPtr: Pass the REST  Cfg.
};

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
 *          (12) Once a connection is closed a hook function can be called to notify the upper application that a connection
 *               is no more active. This hook function could be used to free some previously allocated memory.
 *               If the hook is not required by the upper application, it can be set as DEF_NULL and no function will be called.
 *               See HTTPs_ConnCloseHook() function for further details.
 *******************************************************************************************************/

const HTTPs_CTRL_LAYER_APP_HOOKS Ex_HTTP_Server_CtrlLayerHooksBasic =
{
  .OnInstanceInit = DEF_NULL,                                   // .OnInstanceInitHook    See Note #1.
  .OnReqHdrRx = DEF_NULL,                                       // .OnReqHdrRxHook        See Note #2.
  .OnReq = Ex_HTTP_Server_ReqHook,                              // .OnReqHook             See Note #3.
  .OnReqBodyRx = DEF_NULL,                                      // .OnReqBodyRxHook       See Note #4.
  .OnReqSignal = DEF_NULL,                                      // .OnReqRdySignalHook    See Note #5.
  .OnReqPoll = DEF_NULL,                                        // .OnReqRdyPollHook      See Note #6.
  .OnRespHdrTx = DEF_NULL,                                      // .OnRespHdrTxHook       See Note #7.
  .OnRespToken = Ex_HTTP_Server_RespTokenValGetHook,            // .OnRespTokenHook       See Note #8.
  .OnRespChunk = Ex_HTTP_Server_RespChunkDataGetHook,           // .OnRespChunkHook       See Note #9.
  .OnTransComplete = DEF_NULL,                                  // .OnTransCompleteHook   See Note #10.
  .OnError = DEF_NULL,                                          // .OnErrHook             See Note #11.
  .OnConnClose = DEF_NULL                                       // .OnConnCloseHook       See Note #12.
};

//                                                                 Set the Application Instance for the other features.
HTTPs_CTRL_LAYER_APP_INST Ex_HTTP_Server_AppInst_Basic = {
  (HTTPs_CTRL_LAYER_APP_HOOKS *)&Ex_HTTP_Server_CtrlLayerHooksBasic,
  DEF_NULL
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                 Ex_HTTP_ServerCtrlLayerInstanceCreate()
 *
 * @brief  Initialize HTTPs REST Example application.
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
void Ex_HTTP_Server_InstanceCreateCtrlLayer(void)
{
  RTOS_TASK_CFG              *p_cfg_task;
  HTTPs_INSTANCE             *p_instance;
  HTTPs_CFG                  *p_cfg_http;
  HTTPs_CTRL_LAYER_AUTH_INST **auth_filters_tbl;
  HTTPs_CTRL_LAYER_APP_INST  **authentified_services_tbl;
  HTTPs_CTRL_LAYER_APP_INST  **auth_apps_tbl;
  HTTPs_CTRL_LAYER_CFG       *p_protected_services;
  HTTPs_CTRL_LAYER_CFG       *p_unprotected_services;
  HTTPs_CTRL_LAYER_CFG_LIST  *p_ctrl_layer_cfg_list;
  HTTPs_CTRL_LAYER_CFG       **ctrl_layer_cfgs_tbl;
  RTOS_ERR                   err;

  Ex_HTTP_ServerUsersInit();                                    // Initialize HTTP Server users.

  Ex_HTTP_Server_REST_MemInit();                                // Initialize REST hooks memory.

  Ex_HTTP_Server_StaticFS_Prepare();                            // Add files to the static file system.

  //                                                               -------- ALLOC HTTP CFG STRUCT (SEE NOTE 1) --------
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

  p_cfg_http->FS_CfgPtr = Mem_SegAlloc("Ex HTTP Server FS static cfg struct",
                                       DEF_NULL,
                                       sizeof(HTTPs_CFG_FS_STATIC),
                                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  p_cfg_http->HdrRxCfgPtr = (HTTPs_HDR_RX_CFG *)Mem_SegAlloc("Ex HTTP Server hdr rx cfg struct",
                                                             DEF_NULL,
                                                             sizeof(HTTPs_HDR_RX_CFG),
                                                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  p_cfg_http->HdrTxCfgPtr = (HTTPs_HDR_TX_CFG *)Mem_SegAlloc("Ex HTTP Server hdr tx cfg struct",
                                                             DEF_NULL,
                                                             sizeof(HTTPs_HDR_TX_CFG),
                                                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  p_cfg_http->QueryStrCfgPtr = (HTTPs_QUERY_STR_CFG *)Mem_SegAlloc("Ex HTTP Server query str cfg struct",
                                                                   DEF_NULL,
                                                                   sizeof(HTTPs_QUERY_STR_CFG),
                                                                   &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  p_cfg_http->FormCfgPtr = (HTTPs_FORM_CFG *)Mem_SegAlloc("Ex HTTP Server form cfg struct",
                                                          DEF_NULL,
                                                          sizeof(HTTPs_FORM_CFG),
                                                          &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  p_cfg_http->TokenCfgPtr = (HTTPs_TOKEN_CFG *)Mem_SegAlloc("Ex HTTP Server token cfg struct",
                                                            DEF_NULL,
                                                            sizeof(HTTPs_TOKEN_CFG),
                                                            &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ----- ALLOC CTRL LAYER CFG STRUCT (SEE NOTE 1) -----
  auth_filters_tbl = (HTTPs_CTRL_LAYER_AUTH_INST **)Mem_SegAlloc("Ex HTTP Server auth filters tbl",
                                                                 DEF_NULL,
                                                                 (sizeof(HTTPs_CTRL_LAYER_AUTH_INST *) * 1u),
                                                                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  authentified_services_tbl = (HTTPs_CTRL_LAYER_APP_INST **)Mem_SegAlloc("Ex HTTP Server authentified services tbl",
                                                                         DEF_NULL,
                                                                         (sizeof(HTTPs_CTRL_LAYER_APP_INST *) * 3u),
                                                                         &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  auth_apps_tbl = (HTTPs_CTRL_LAYER_APP_INST **)Mem_SegAlloc("Ex HTTP Server auth filters",
                                                             DEF_NULL,
                                                             (sizeof(HTTPs_CTRL_LAYER_APP_INST *) * 1u),
                                                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  p_protected_services = (HTTPs_CTRL_LAYER_CFG *)Mem_SegAlloc("Ex HTTP Server protected services",
                                                              DEF_NULL,
                                                              sizeof(HTTPs_CTRL_LAYER_CFG),
                                                              &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  p_unprotected_services = (HTTPs_CTRL_LAYER_CFG *)Mem_SegAlloc("Ex HTTP Server unprotected services",
                                                                DEF_NULL,
                                                                sizeof(HTTPs_CTRL_LAYER_CFG),
                                                                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  ctrl_layer_cfgs_tbl = (HTTPs_CTRL_LAYER_CFG **)Mem_SegAlloc("Ex HTTP Server ctrl layer cfgs tbl",
                                                              DEF_NULL,
                                                              sizeof(HTTPs_CTRL_LAYER_CFG *) * 2u,
                                                              &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  p_ctrl_layer_cfg_list = (HTTPs_CTRL_LAYER_CFG_LIST *)Mem_SegAlloc("Ex HTTP Server ctrl layer cfg list",
                                                                    DEF_NULL,
                                                                    sizeof(HTTPs_CTRL_LAYER_CFG_LIST),
                                                                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ---------------- PREPARE CFG STRUCT ----------------
  //                                                               Ctrl layer cfg.
  auth_filters_tbl[0u] = &Ex_HTTP_Server_AuthInst;

  authentified_services_tbl[0u] = &Ex_HTTP_Server_AppInstAuthProtected;
  authentified_services_tbl[1u] = &Ex_HTTP_Server_AppInst_REST;
  authentified_services_tbl[2u] = &Ex_HTTP_Server_AppInst_Basic;

  auth_apps_tbl[0u] = &Ex_HTTP_Server_AppInstAuthUnprotected;

  p_protected_services->AuthInstsPtr = auth_filters_tbl;
  p_protected_services->AuthInstsNbr = 1u;
  p_protected_services->AppInstsPtr = authentified_services_tbl;
  p_protected_services->AppInstsNbr = 3u;

  p_unprotected_services->AuthInstsPtr = DEF_NULL;
  p_unprotected_services->AuthInstsNbr = 0u;
  p_unprotected_services->AppInstsPtr = auth_apps_tbl;
  p_unprotected_services->AppInstsNbr = 1u;

  ctrl_layer_cfgs_tbl[0u] = p_protected_services;
  ctrl_layer_cfgs_tbl[1u] = p_unprotected_services;

  p_ctrl_layer_cfg_list->CfgsPtr = ctrl_layer_cfgs_tbl;
  p_ctrl_layer_cfg_list->Size = 2u;

  //                                                               Server instance task cfg.
  p_cfg_task->Prio = 21u;
  p_cfg_task->StkSizeElements = 1024u;
  p_cfg_task->StkPtr = DEF_NULL;

  //                                                               Server instance cfg.
  ((HTTPs_HDR_RX_CFG *)p_cfg_http->HdrRxCfgPtr)->NbrPerConnMax = 15u;
  ((HTTPs_HDR_RX_CFG *)p_cfg_http->HdrRxCfgPtr)->DataLenMax = 128u;
  ((HTTPs_HDR_TX_CFG *)p_cfg_http->HdrTxCfgPtr)->NbrPerConnMax = 15u;
  ((HTTPs_HDR_TX_CFG *)p_cfg_http->HdrTxCfgPtr)->DataLenMax = 128u;
  ((HTTPs_QUERY_STR_CFG *)p_cfg_http->QueryStrCfgPtr)->NbrPerConnMax = 5u;
  ((HTTPs_QUERY_STR_CFG *)p_cfg_http->QueryStrCfgPtr)->KeyLenMax = 15u;
  ((HTTPs_QUERY_STR_CFG *)p_cfg_http->QueryStrCfgPtr)->ValLenMax = 20u;
  ((HTTPs_FORM_CFG *)p_cfg_http->FormCfgPtr)->NbrPerConnMax = 15u;
  ((HTTPs_FORM_CFG *)p_cfg_http->FormCfgPtr)->KeyLenMax = 10u;
  ((HTTPs_FORM_CFG *)p_cfg_http->FormCfgPtr)->ValLenMax = 48u;
  ((HTTPs_FORM_CFG *)p_cfg_http->FormCfgPtr)->MultipartEn = DEF_ENABLED;
  ((HTTPs_FORM_CFG *)p_cfg_http->FormCfgPtr)->MultipartFileUploadEn = DEF_DISABLED;
  ((HTTPs_FORM_CFG *)p_cfg_http->FormCfgPtr)->MultipartFileUploadOverWrEn = DEF_DISABLED;
  ((HTTPs_FORM_CFG *)p_cfg_http->FormCfgPtr)->MultipartFileUploadFolderPtr = DEF_NULL;
  ((HTTPs_TOKEN_CFG *)p_cfg_http->TokenCfgPtr)->NbrPerConnMax = 2u;
  ((HTTPs_TOKEN_CFG *)p_cfg_http->TokenCfgPtr)->ValLenMax = 60u;
  ((HTTPs_CFG_FS_STATIC *)p_cfg_http->FS_CfgPtr)->FS_API_Ptr = &HTTPs_FS_API_Static;
  p_cfg_http->OS_TaskDly_ms = 0u;
  p_cfg_http->SockSel = HTTPs_SOCK_SEL_IPv4_IPv6;
  p_cfg_http->SecurePtr = DEF_NULL;
  p_cfg_http->Port = 80u;
  p_cfg_http->ConnNbrMax = 6u;
  p_cfg_http->ConnInactivityTimeout_s = 10u;
  p_cfg_http->BufLen = 1460u;
  p_cfg_http->ConnPersistentEn = DEF_ENABLED;
  p_cfg_http->PathLenMax = 255u;
  p_cfg_http->DfltResourceNamePtr = "/index.html";
  p_cfg_http->HostNameLenMax = 128u;
  p_cfg_http->HooksPtr = &HTTPsCtrlLayer_HookCfg;
  p_cfg_http->Hooks_CfgPtr = p_ctrl_layer_cfg_list;
  p_cfg_http->FS_Type = HTTPs_FS_TYPE_STATIC;

  Str_Copy_N(&Ex_HTTPs_Name[0], "Control Layer with Native FS", 60);

  //                                                               ------------ INITIALIZE HTTPS INSTANCE -------------
  p_instance = HTTPs_InstanceInit(p_cfg_http,
                                  p_cfg_task,
                                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ---------------- ADD REST RESOURCES ----------------
  HTTPsREST_Publish(&Ex_HTTP_Server_REST_ListResource,          // User list rsrc.
                    0u,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPsREST_Publish(&Ex_HTTP_Server_REST_UserResource,          // User info rsrc.
                    0u,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               --------------- START HTTPS INSTANCE ---------------
  HTTPs_InstanceStart(p_instance, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                 Ex_HTTP_Server_AuthRequiredRightsGet()
 *
 * @brief  Returns the rights required to fulfill the needs of a given request.
 *
 * @param  p_instance  Pointer to the HTTP server instance object.
 *
 * @param  p_conn      Pointer to the HTTP connection object.
 *
 * @return  returns the authorization right level for this example application.
 *******************************************************************************************************/
static AUTH_RIGHT Ex_HTTP_Server_AuthRequiredRightsGet(const HTTPs_INSTANCE *p_instance,
                                                       const HTTPs_CONN     *p_conn)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);

  return (HTTP_USER_ACCESS);
}

/****************************************************************************************************//**
 *                                   Ex_HTTP_Server_AuthLoginHookParse()
 *
 * @brief  (1) Check all the HTTP requests received to see if they are related to resources of the login page.
 *           - (a) Check if the POST login is received.
 *           - (b) For each request set the redirect paths on no, invalid & valid credentials.
 *           - (c) Parse the form fields received in the body for the user and password.
 *
 * @param  p_instance  Pointer to the HTTP server instance object.
 *
 * @param  p_conn      Pointer to the HTTP connection object.
 *
 * @param  state       State of the Authentication module:
 *                         - HTTPs_AUTH_STATE_REQ_URL:      The url and the headers were received and parse.
 *                         - HTTPs_AUTH_STATE_REQ_COMPLETE: All the request (url + headers + body) was received and parse.
 *
 * @param  p_result    Pointer to the authentication result structure to fill.
 *
 * @return  DEF_YES, if the request is the POST login.
 *          DEF_NO,  otherwise.
 *
 *
 * @note   (2) This hook will be called twice for a request processed by the Authentication module:
 *           - (a) When the Start line of the request (with the url) and the headers have been
 *                 received and parse -> HTTPs_AUTH_STATE_REQ_URL state.
 *           - (b) When all the request has been completely received and parse including the body
 *                 -> HTTPs_AUTH_STATE_REQ_COMPLETE state.
 *
 * @note   (3) for each request received the redirect paths is set as follow:
 *           - (a) RedirectPath_OnValidCred    INDEX_PAGE_URL
 *           - (b) RedirectPath_OnInvalidCred  LOGIN_PAGE_URL
 *           - (c) RedirectPath_OnNoCred
 *               - (1) if the path is an unprotected path, let it go. (DEF_NULL) (i.e. the logo)
 *               - (2) otherwise               LOGIN_PAGE_URL
 *******************************************************************************************************/
static CPU_BOOLEAN Ex_HTTP_Server_AuthLoginParse(const HTTPs_INSTANCE *p_instance,
                                                 const HTTPs_CONN     *p_conn,
                                                 HTTPs_AUTH_STATE     state,
                                                 HTTPs_AUTH_RESULT    *p_result)
{
  CPU_INT16S  cmp_val;
  CPU_BOOLEAN is_login = DEF_NO;
#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
  HTTPs_KEY_VAL *p_current;
  CPU_INT16S    cmp_val_username;
  CPU_INT16S    cmp_val_password;
#endif

  PP_UNUSED_PARAM(p_instance);

  p_result->UsernamePtr = DEF_NULL;
  p_result->PasswordPtr = DEF_NULL;
  //                                                               Set redirect paths for each requests.
  p_result->RedirectPathOnInvalidCredPtr = LOGIN_PAGE_URL;
  p_result->RedirectPathOnValidCredPtr = INDEX_PAGE_URL;

  switch (state) {
    //                                                             --------------- REQUEST URL RECEIVED ---------------
    case HTTPs_AUTH_STATE_REQ_URL:
      //                                                           Set redirect paths for each requests.
      cmp_val = Str_Cmp(p_conn->PathPtr, LOGIN_PAGE_URL);
      if (cmp_val != 0) {
        cmp_val = Str_Cmp(p_conn->PathPtr, MICRIUM_LOGO_URL);
        if (cmp_val != 0) {
          cmp_val = Str_Cmp(p_conn->PathPtr, MICRIUM_CSS_URL);
        }
      }
      p_result->RedirectPathOnNoCredPtr = (cmp_val == 0) ? DEF_NULL : LOGIN_PAGE_URL;
      //                                                           Check if POST login received.
      cmp_val = Str_Cmp(p_conn->PathPtr, LOGIN_PAGE_CMD);
      if (cmp_val == 0) {
        is_login = DEF_YES;
      }
      break;

    //                                                             -------------- REQUEST BODY RECEIVED ---------------
    case HTTPs_AUTH_STATE_REQ_COMPLETE:
#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
      //                                                           Parse form fields received for user/password.
      p_current = p_conn->FormDataListPtr;

      while ((p_current != DEF_NULL)
             && ((p_result->UsernamePtr == DEF_NULL)
                 || (p_result->PasswordPtr == DEF_NULL))) {
        if (p_current->DataType == HTTPs_KEY_VAL_TYPE_PAIR) {
          cmp_val_username = Str_CmpIgnoreCase_N(p_current->KeyPtr,
                                                 FORM_USERNAME_FIELD_NAME,
                                                 p_current->KeyLen);

          cmp_val_password = Str_CmpIgnoreCase_N(p_current->KeyPtr,
                                                 FORM_PASSWORD_FIELD_NAME,
                                                 p_current->KeyLen);

          if (cmp_val_username == 0) {
            p_result->UsernamePtr = p_current->ValPtr;
            is_login = DEF_YES;
          } else if (cmp_val_password == 0) {
            p_result->PasswordPtr = p_current->ValPtr;
          }
        }

        p_current = p_current->NextPtr;
      }
#endif
      break;

    default:
      break;
  }

  return (is_login);
}

/****************************************************************************************************//**
 *                                  Ex_HTTP_Server_AuthLogoutHookParse()
 *
 * @brief  Parse requests received for logout URL and form data logout info.
 *
 * @param  p_instance  Pointer to HTTPs instance object.
 *
 * @param  p_conn      Pointer to HTTPs connection object.
 *
 * @param  state       State of the Authentication module:
 *                         HTTPs_AUTH_STATE_REQ_URL:      The url and the headers were received and parse.
 *                         HTTPs_AUTH_STATE_REQ_COMPLETE: All the request (url + headers + body) was received and parse.
 *
 * @return  DEF_YES, if Logout received.
 *          DEF_NO,  otherwise.
 *
 * @note   (1) This hook will be called twice for a request processed by the Authentication module:
 *           - (a) When the Start line of the request (with the url) and the headers have been
 *                 received and parse -> HTTPs_AUTH_STATE_REQ_URL state.
 *           - (b) When all the request has been completely received and parse including the body
 *                 -> HTTPs_AUTH_STATE_REQ_COMPLETE state.
 *******************************************************************************************************/
static CPU_BOOLEAN Ex_HTTP_Server_AuthLogoutParse(const HTTPs_INSTANCE *p_instance,
                                                  const HTTPs_CONN     *p_conn,
                                                  HTTPs_AUTH_STATE     state)
{
  CPU_BOOLEAN is_logout = DEF_NO;
#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
  HTTPs_KEY_VAL *p_current;
#endif
  CPU_INT16S cmp_val;

  PP_UNUSED_PARAM(p_instance);

  switch (state) {
    //                                                             --------------- REQUEST URL RECEIVED ---------------
    case HTTPs_AUTH_STATE_REQ_URL:
      //                                                           Check if POST logout received.
      cmp_val = Str_Cmp(p_conn->PathPtr, LOGOUT_PAGE_CMD);
      if (cmp_val == 0) {
        is_logout = DEF_YES;
      }
      break;

    //                                                             -------------- REQUEST BODY RECEIVED ---------------
    case HTTPs_AUTH_STATE_REQ_COMPLETE:
#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
      //                                                           Parse form fields received for logout.
      p_current = p_conn->FormDataListPtr;

      while (p_current != DEF_NULL) {
        if (p_current->DataType == HTTPs_KEY_VAL_TYPE_PAIR) {
          cmp_val = Str_CmpIgnoreCase_N(p_current->KeyPtr,
                                        FORM_LOGOUT_FIELD_NAME,
                                        p_current->KeyLen);
          if (cmp_val == 0) {
            is_logout = DEF_YES;
            break;
          }
        }

        p_current = p_current->NextPtr;
      }
#endif
      break;

    default:
      break;
  }

  return (is_logout);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        Ex_HTTP_ServerUsersInit()
 *
 * @brief  Adds three users to the authentication system.
 *       - (1) Username:   admin
 *             Password:   password
 *             Rights:     Manager of HTTP user access right
 *       - (2) Username:   user0
 *             Password:
 *             Rights:     HTTP user
 *       - (3) Username:   user1
 *             Password:   user1
 *             Rights:     HTTP user
 *******************************************************************************************************/
static void Ex_HTTP_ServerUsersInit(void)
{
  AUTH_USER_HANDLE user_handle;
  RTOS_ERR         err;

  user_handle = Auth_CreateUser("user0",
                                "",
                                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  Auth_GrantRight(HTTP_USER_ACCESS,
                  user_handle,
                  Auth_RootUserHandle,
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  user_handle = Auth_CreateUser("user1",
                                "user1",
                                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  Auth_GrantRight(HTTP_USER_ACCESS,
                  user_handle,
                  Auth_RootUserHandle,
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/****************************************************************************************************//**
 *                                    Ex_HTTP_Server_StaticFS_Prepare()
 *
 * @brief  Example of web files add to static file system.
 *******************************************************************************************************/
static void Ex_HTTP_Server_StaticFS_Prepare(void)
{
  CPU_BOOLEAN success;

  success = HTTPs_FS_Init();                                    // Initialize static FS.
  APP_RTOS_ASSERT_DBG((success == DEF_OK),; );

  //                                                               Add files to static FS.
  success = HTTPs_FS_AddFile(_404_HTML_NAME,
                             _404_HTML_CONTENT,
                             _404_HTML_SIZE);
  APP_RTOS_ASSERT_CRITICAL(success == DEF_OK,; );

  success = HTTPs_FS_AddFile(FORM_HTML_NAME,
                             FORM_HTML_CONTENT,
                             FORM_HTML_SIZE);
  APP_RTOS_ASSERT_CRITICAL(success == DEF_OK,; );

  success = HTTPs_FS_AddFile(INDEX_HTML_NAME,
                             INDEX_HTML_CONTENT,
                             INDEX_HTML_SIZE);
  APP_RTOS_ASSERT_CRITICAL(success == DEF_OK,; );

  success = HTTPs_FS_AddFile(LIST_HTML_NAME,
                             LIST_HTML_CONTENT,
                             LIST_HTML_SIZE);
  APP_RTOS_ASSERT_CRITICAL(success == DEF_OK,; );

  success = HTTPs_FS_AddFile(LOGIN_HTML_NAME,
                             LOGIN_HTML_CONTENT,
                             LOGIN_HTML_SIZE);
  APP_RTOS_ASSERT_CRITICAL(success == DEF_OK,; );

  success = HTTPs_FS_AddFile(LOGO_GIF_NAME,
                             LOGO_GIF_CONTENT,
                             LOGO_GIF_SIZE);
  APP_RTOS_ASSERT_CRITICAL(success == DEF_OK,; );

  success = HTTPs_FS_AddFile(UC_STYLE_CSS_NAME,
                             UC_STYLE_CSS_CONTENT,
                             UC_STYLE_CSS_SIZE);
  APP_RTOS_ASSERT_CRITICAL(success == DEF_OK,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL && RTOS_MODULE_FS_AVAIL
