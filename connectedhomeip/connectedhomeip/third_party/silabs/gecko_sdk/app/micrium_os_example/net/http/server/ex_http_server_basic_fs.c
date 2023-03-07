/***************************************************************************//**
 * @file
 * @brief HTTP Server Instance Basic With FS Example
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
 * @note     (2) This example requires Micrium OS File System module.
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

#if (defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL) \
  && defined(RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_utils.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_path.h>
#include  <net/include/http_server.h>
#include  <net/include/net_fs.h>
#include  <net/include/net_cfg_net.h>

#include  <fs/include/fs_ramdisk.h>
#include  <fs/include/fs_core_cache.h>
#include  <fs/include/fs_core_file.h>
#include  <fs/include/fs_core_vol.h>
#include  <fs/include/fs_core_partition.h>
#include  <fs/include/fs_media.h>
#include  <fs/include/fs_fat.h>

#include  "files/_404_html.h"
#include  "files/form_html.h"
#include  "files/index_html.h"
#include  "files/list_html.h"
#include  "files/login_html.h"
#include  "files/logo_gif.h"
#include  "files/uc_style_css.h"

#include  "ex_http_server.h"
#include  "ex_http_server_hooks.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  EX_HTTP_SERVER_RAMDISK_SEC_SIZE
#define  EX_HTTP_SERVER_RAMDISK_SEC_SIZE                    512u
#endif

#ifndef  EX_HTTP_SERVER_RAMDISK_SEC_NBR
#define  EX_HTTP_SERVER_RAMDISK_SEC_NBR                     104u
#endif

#ifndef  EX_HTTP_SERVER_FILE_RAM_MEDIA_NAME
#define  EX_HTTP_SERVER_FILE_RAM_MEDIA_NAME                 "ram_https"
#endif

#ifndef  EX_HTTP_SERVER_FILE_VOL_NAME
#define  EX_HTTP_SERVER_FILE_VOL_NAME                       "ram_https"
#endif

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

const HTTPs_HOOK_CFG Ex_HTTP_Server_Hooks_Basic =
{
  .OnInstanceInitHook = Ex_HTTP_Server_InstanceInitHook,        // .OnInstanceInitHook    See Note #1.
  .OnReqHdrRxHook = Ex_HTTP_Server_ReqHdrRxHook,                // .OnReqHdrRxHook        See Note #2.
  .OnReqHook = Ex_HTTP_Server_ReqHookNoREST,                    // .OnReqHook             See Note #3.
  .OnReqBodyRxHook = Ex_HTTP_Server_ReqBodyRxHook,              // .OnReqBodyRxHook       See Note #4.
  .OnReqRdySignalHook = Ex_HTTP_Server_ReqRdySignalHook,        // .OnReqRdySignalHook    See Note #5.
  .OnReqRdyPollHook = Ex_HTTP_Server_ReqRdyPollHook,            // .OnReqRdyPollHook      See Note #6.
  .OnRespHdrTxHook = Ex_HTTP_Server_RespHdrTxHook,              // .OnRespHdrTxHook       See Note #7.
  .OnRespTokenHook = Ex_HTTP_Server_RespTokenValGetHook,        // .OnRespTokenHook       See Note #8.
  .OnRespChunkHook = Ex_HTTP_Server_RespChunkDataGetHook,       // .OnRespChunkHook       See Note #9.
  .OnTransCompleteHook = Ex_HTTP_Server_TransCompleteHook,      // .OnTransCompleteHook   See Note #10.
  .OnErrHook = Ex_HTTP_Server_ErrHook,                          // .OnErrHook             See Note #11.
  .OnErrFileGetHook = Ex_HTTP_Server_ErrFileGetHook,            // .OnErrFileGetHook      See Note #12.
  .OnConnCloseHook = Ex_HTTP_Server_ConnCloseHook               // .OnConnCloseHook       See Note #13.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static RTOS_TASK_CFG Ex_HTTP_ServerTaskCfg = {  .Prio = 21u,
                                                .StkSizeElements = 1024u,
                                                .StkPtr = DEF_NULL };

static HTTPs_CFG_FS_DYN Ex_HTTP_ServerFS_Dyn = {  .FS_API_Ptr = &NetFS_API_Native,
                                                  .WorkingFolderNamePtr = EX_HTTP_SERVER_FILE_VOL_NAME };

static HTTPs_HDR_RX_CFG Ex_HTTP_ServerHdrRx = {  .NbrPerConnMax = 15u,
                                                 .DataLenMax = 128u };

static HTTPs_HDR_TX_CFG Ex_HTTP_ServerHdrTx = {  .NbrPerConnMax = 15u,
                                                 .DataLenMax = 128u };

static HTTPs_QUERY_STR_CFG Ex_HTTP_ServerQueryStr = {  .NbrPerConnMax = 5u,
                                                       .KeyLenMax = 15u,
                                                       .ValLenMax = 20u };

static HTTPs_FORM_CFG Ex_HTTP_ServerForm = {  .KeyLenMax = 10u,
                                              .NbrPerConnMax = 5u,
                                              .ValLenMax = 12u,
                                              .MultipartEn = DEF_ENABLED,
                                              .MultipartFileUploadEn = DEF_ENABLED,
                                              .MultipartFileUploadOverWrEn = DEF_ENABLED,
                                              .MultipartFileUploadFolderPtr = "/" };

static HTTPs_TOKEN_CFG Ex_HTTP_ServerToken = {  .NbrPerConnMax = 5u,
                                                .ValLenMax = 12u };

static HTTPs_CFG Ex_HTTP_ServerCfg = {  .OS_TaskDly_ms = 0u,
                                        .SockSel = HTTPs_SOCK_SEL_IPv4_IPv6,
                                        .SecurePtr = DEF_NULL,
                                        .Port = 80u,
                                        .ConnNbrMax = 15u,
                                        .ConnInactivityTimeout_s = 15u,
                                        .BufLen = 1460u,
                                        .ConnPersistentEn = DEF_ENABLED,
                                        .FS_Type = HTTPs_FS_TYPE_DYN,
                                        .PathLenMax = 255u,
                                        .DfltResourceNamePtr = "/index.html",
                                        .HostNameLenMax = 128u,
                                        .HooksPtr = &Ex_HTTP_Server_Hooks_Basic,
                                        .Hooks_CfgPtr = DEF_NULL,
                                        .FS_CfgPtr = &Ex_HTTP_ServerFS_Dyn,
                                        .HdrRxCfgPtr = &Ex_HTTP_ServerHdrRx,
                                        .HdrTxCfgPtr = &Ex_HTTP_ServerHdrTx,
                                        .QueryStrCfgPtr = &Ex_HTTP_ServerQueryStr,
                                        .FormCfgPtr = &Ex_HTTP_ServerForm,
                                        .TokenCfgPtr = &Ex_HTTP_ServerToken };

static CPU_INT08U Ex_HTTP_Server_RAMDisk[EX_HTTP_SERVER_RAMDISK_SEC_SIZE * EX_HTTP_SERVER_RAMDISK_SEC_NBR];
static FS_CACHE   *Ex_HTTP_Server_CachePtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_HTTP_Server_FS_MediaPrepare(void);

static void Ex_HTTP_Server_FS_FileAdd(FS_WRK_DIR_HANDLE wrk_dir_handle,
                                      CPU_CHAR          *p_file_name,
                                      CPU_CHAR          *p_file_content,
                                      CPU_SIZE_T        file_len);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                     Ex_HTTP_Server_InstanceCreate()
 *
 * @brief  Initializes and starts a basic web server instance. Retrieves WebServer files using
 *         Micrium OS File System.
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
void Ex_HTTP_Server_InstanceCreateBasic(void)
{
  HTTPs_INSTANCE *p_instance;
  RTOS_ERR       err;

  Ex_HTTP_Server_FS_MediaPrepare();                             // Prepare FS media that will contain the web files.

  p_instance = HTTPs_InstanceInit(&Ex_HTTP_ServerCfg,           // Instance configuration. See Note #3a.
                                  &Ex_HTTP_ServerTaskCfg,       // Instance task configuration. See Note #3b.
                                  &err);

  Str_Copy_N(&Ex_HTTPs_Name[0], "Basic with Native FS", 60);
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
 *                                    Ex_HTTP_Server_FS_MediaPrepare()
 *
 * @brief  Example of Filse System media preparation to contain web files.
 *******************************************************************************************************/
static void Ex_HTTP_Server_FS_MediaPrepare(void)
{
  FS_RAM_DISK_CFG   ramdisk_cfg;
  FS_BLK_DEV_HANDLE dev_handle;
  FS_MEDIA_HANDLE   media_handle;
  FS_WRK_DIR_HANDLE wrk_dir_handle;
  FS_CACHE_CFG      cache_cfg;
  RTOS_ERR          err;

  ramdisk_cfg.DiskPtr = Ex_HTTP_Server_RAMDisk;                 // Create a RAMDisk that will contain the files.
  ramdisk_cfg.LbCnt = EX_HTTP_SERVER_RAMDISK_SEC_NBR;
  ramdisk_cfg.LbSize = EX_HTTP_SERVER_RAMDISK_SEC_SIZE;

  FS_RAM_Disk_Add(EX_HTTP_SERVER_FILE_RAM_MEDIA_NAME,
                  &ramdisk_cfg,
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  cache_cfg.Align = sizeof(CPU_ALIGN);                          // Create cache for RAMDisk block device.
  cache_cfg.BlkMemSegPtr = DEF_NULL;
  cache_cfg.MaxLbSize = 512u;
  cache_cfg.MinLbSize = 512u;
  cache_cfg.MinBlkCnt = 1u;

  Ex_HTTP_Server_CachePtr = FSCache_Create(&cache_cfg,
                                           &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Open block device.
  media_handle = FSMedia_Get(EX_HTTP_SERVER_FILE_RAM_MEDIA_NAME);
  APP_RTOS_ASSERT_CRITICAL(!FS_MEDIA_HANDLE_IS_NULL(media_handle),; );

  dev_handle = FSBlkDev_Open(media_handle, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FSCache_Assign(dev_handle,                                    // Assign cache to block device.
                 Ex_HTTP_Server_CachePtr,
                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FS_FAT_Fmt(dev_handle,                                        // Format RAMDisk in FAT format.
             FS_PARTITION_NBR_VOID,
             DEF_NULL,
             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  (void)FSVol_Open(dev_handle,                                  // Open volume and call it "ram_https".
                   1u,
                   EX_HTTP_SERVER_FILE_VOL_NAME,
                   FS_VOL_OPT_DFLT,
                   &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  wrk_dir_handle = FSWrkDir_Open(FSWrkDir_NullHandle,           // Create and open a working directory at the root of vol.
                                 EX_HTTP_SERVER_FILE_VOL_NAME,
                                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  Ex_HTTP_Server_FS_FileAdd(wrk_dir_handle,                           // Create and copy content of all the files to RAMDisk.
                            _404_HTML_NAME,
                            _404_HTML_CONTENT,
                            _404_HTML_SIZE);

  Ex_HTTP_Server_FS_FileAdd(wrk_dir_handle,
                            FORM_HTML_NAME,
                            FORM_HTML_CONTENT,
                            FORM_HTML_SIZE);

  Ex_HTTP_Server_FS_FileAdd(wrk_dir_handle,
                            INDEX_HTML_NAME,
                            INDEX_HTML_CONTENT,
                            INDEX_HTML_SIZE);

  Ex_HTTP_Server_FS_FileAdd(wrk_dir_handle,
                            LIST_HTML_NAME,
                            LIST_HTML_CONTENT,
                            LIST_HTML_SIZE);

  Ex_HTTP_Server_FS_FileAdd(wrk_dir_handle,
                            LOGIN_HTML_NAME,
                            LOGIN_HTML_CONTENT,
                            LOGIN_HTML_SIZE);

  Ex_HTTP_Server_FS_FileAdd(wrk_dir_handle,
                            LOGO_GIF_NAME,
                            LOGO_GIF_CONTENT,
                            LOGO_GIF_SIZE);

  Ex_HTTP_Server_FS_FileAdd(wrk_dir_handle,
                            UC_STYLE_CSS_NAME,
                            UC_STYLE_CSS_CONTENT,
                            UC_STYLE_CSS_SIZE);

  FSWrkDir_Close(wrk_dir_handle, &err);                         // Close working directory.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/****************************************************************************************************//**
 *                                          Ex_HTTPs_FS_FileAdd()
 *
 * @brief  Create a files and copy contenmt to it from C array.
 *
 * @param  wrk_dir_handle  Handle on working directory.
 *
 * @param  p_file_name     String containing name of the file.
 *
 * @param  p_file_content  Pointer to content of file.
 *
 * @param  file_len        Length of file, in octets.
 *******************************************************************************************************/
static void Ex_HTTP_Server_FS_FileAdd(FS_WRK_DIR_HANDLE wrk_dir_handle,
                                      CPU_CHAR          *p_file_name,
                                      CPU_CHAR          *p_file_content,
                                      CPU_SIZE_T        file_len)
{
  CPU_SIZE_T     rem_size = file_len;
  CPU_SIZE_T     size_wr;
  FS_FILE_HANDLE file_handle;
  RTOS_ERR       err;

  file_handle = FSFile_Open(wrk_dir_handle,
                            p_file_name,
                            (FS_FILE_ACCESS_MODE_CREATE | FS_FILE_ACCESS_MODE_WR),
                            &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  while (rem_size > 0) {
    size_wr = FSFile_Wr(file_handle,
                        p_file_content,
                        file_len,
                        &err);

    rem_size -= size_wr;
  }

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FSFile_Close(file_handle, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL && RTOS_MODULE_FS_AVAIL
