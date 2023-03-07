/***************************************************************************//**
 * @file
 * @brief http server without filesystem
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "webserver.h"

#include <rtos_description.h>
#include <common/include/lib_mem.h>
#include <common/include/lib_str.h>
#include <common/include/rtos_path.h>
#include <common/include/rtos_utils.h>
#include <net/include/http_server.h>
#include <net/include/net_cfg_net.h>
#include <net/source/http/server/http_server_priv.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "images.h"
#include "common_declarations.h"

// -----------------------------------------------------------------------------
// Defines

#define  DFLT_URL_PATH          "/WebMic"
#define  AUDIO_URL              "/audio.opus"
#define  PCM_URL                "/audio.pcm"

#define  FIRST_ADR_INDEX        1u
#define  SECON_ADR_INDEX        3u
#define  THIRD_ADR_INDEX        4u
#define  FOURT_ADR_INDEX        5u

// -----------------------------------------------------------------------------
// Function prototypes

static CPU_BOOLEAN HTTPServerNoFSReqHook(const HTTPs_INSTANCE *p_instance,
                                         HTTPs_CONN           *p_conn,
                                         const void           *p_hook_cfg);

static CPU_BOOLEAN HTTPServerNoFSRespHdrTxHook(const HTTPs_INSTANCE *p_instance,
                                               HTTPs_CONN           *p_conn,
                                               const void           *p_hook_cfg);

static CPU_BOOLEAN HTTPServerNoFSRespChunkDataGetHook(const HTTPs_INSTANCE *p_instance,
                                                      HTTPs_CONN           *p_conn,
                                                      const void           *p_hook_cfg,
                                                      void                 *p_buf,
                                                      CPU_SIZE_T            buf_free_lim,
                                                      CPU_SIZE_T           *len_to_tx);

static void HTTPSeverNoFSCloseHook(const HTTPs_INSTANCE *p_instance,
                                   HTTPs_CONN           *p_conn,
                                   const void           *p_hook_cfg);

// -----------------------------------------------------------------------------
// File scope variables

static OGG_PageHeader_TypeDef idPacket_oggHeader = {
  .magic_num = "OggS",
  .type = 0x02, // first page in a logical bitstream (b_o_s)
  .serial = SERIALNO,
  .segments = 1, // send nothing more than an opus ID header
  .size = sizeof(OPUS_IdHeader_TypeDef), // size of payload
};

static OPUS_IdHeader_TypeDef idPacket_oggBody = {
  .signature = "OpusHead",
  .version = 1, // Must be 1. See IETF rfc7845
  .channels = CHANNELS,
  .preskip = PRE_SKIP,
  .samplerate = EX_SAMPLE_FREQ,
};

static OGG_PageHeader_TypeDef commentPacket_oggHeader = {
  .magic_num = "OggS",
  .type = 0x00, // neither first, last nor continuation page
  .serial = SERIALNO,
  .sequence = 1, // second page in stream
  .segments = 1, // just sending one packet
  .size = sizeof(OPUS_CommentHeader_TypeDef), // size of payload
};

static OPUS_CommentHeader_TypeDef commentPacket_oggBody = {
  .signature = "OpusTags",
};

static CPU_INT08U index_html[] =
  "<!DOCTYPE html>"
  "<html lang='en'>"
  "<head>"
  "<title>WebMic Example</title>"
  "<link href='styles.css' rel='stylesheet'>"
  "<link href='https://www.silabs.com/etc.clientlibs/siliconlabs/clientlibs/clientlib-global/resources/images/favicon.ico' rel='icon'>"
  "</head>"
  "<body>"
  "<header>"
  "<div class='logo'>"
  "<img src='/logo.png'>"
  "</div>"
  "<h1>Welcome to your Silicon Labs development kit!</h1>"
  "</header>"
  "<div class='player'>"
  "<audio controls preload='none'>"
  "<source src='/audio.opus' type='audio/ogg; codecs=opus'>"
  "<p>Your browser doesn't support HTML audio.</p>"
  "</audio>"
  "</div>"
  "<div class='intro'>"
  "<div class='container text-center'>"
  "<p class='lead'>This webpage is served by a simple web server running on your development kit using the "
  "<a href='https://doc.micrium.com/display/OSUM50900/HTTP+Server+Module'>Micrium</a>"
  " TCP/IP stack and HTTP server."
  "</p>"
  "<a href='http://www.silabs.com' class='btn btn-white'>Learn More</a>"
  "</div>"
  "</div>"
  "</body>"
  "</html>";

static CPU_INT08U styles_css[] =
  "html{font-family:sans-serif;-ms-text-size-adjust:100%;-webkit-text-size-adjust:100%}"
  "body{margin:0}"
  "header{display:block}"
  "a{background-color:transparent}"
  "a:active,a:hover{outline:0}"
  "h1{margin:.67em 0}"
  "img{border:0}"
  "button::-moz-focus-inner,input::-moz-focus-inner{border:0;padding:0}"
  "h1{font-family:inherit;font-weight:500;line-height:1.1;color:inherit;margin-top:20px;margin-bottom:10px;font-size:36px}"
  "p{margin:0 0 10px}"
  ".lead{margin-bottom:20px;font-size:16px;font-weight:300;line-height:1.4}"
  "@media (min-width:768px){.lead{font-size:21px}}"
  ".text-center,header,.btn{text-align:center}"
  ".btn{display:inline-block;margin-bottom:0;font-weight:400;vertical-align:middle;touch-action:manipulation;cursor:pointer;background-image:none;border:1px solid transparent;white-space:nowrap;padding:6px 12px;font-size:14px;line-height:1.42857;border-radius:4px;-webkit-user-select:none;-moz-user-select:none;-ms-user-select:none;user-select:none}"
  ".btn:focus,.btn:active:focus{outline:thin dotted;outline:5px auto -webkit-focus-ring-color;outline-offset:-2px}"
  ".btn:hover,.btn:focus{color:#333;text-decoration:none}"
  ".btn:active{outline:0;background-image:none;-webkit-box-shadow:inset 0 3px 5px rgba(0,0,0,.125);box-shadow:inset 0 3px 5px rgba(0,0,0,.125)}"
  "header{padding:60px 0}"
  ".logo img{width:200px}"
  ".intro{padding:60px 0;background-color:#d91e2a;color:#fff}"
  ".intro a:not(.btn){color:rgba(255,255,255,.7)}"
  ".btn-white{color:#ac1821;background-color:#fff;border-color:#ac1821}"
  ".btn-white:hover,.btn-white:focus,.btn-white:active{color:#ac1821;background-color:#e6e6e6;border-color:#761017}"
  ".btn-white:active{background-image:none}"
  ".player{padding:20px 0 80px 0;text-align:center}";

static CPU_INT08U not_found_html[] = "<h1>404 not found :(</h1>";

static CPU_INT32U clients[NR_CLIENT_MAX] = {0};
static bool newConnection[NR_CLIENT_MAX] = {0};

// -----------------------------------------------------------------------------
// Global constants

/***************************************************************************//**
 * HTTP SERVER HOOK CONFIGURATION
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
 ******************************************************************************/
const  HTTPs_HOOK_CFG  Ex_HTTP_Server_HooksNoFS =
{
  .OnInstanceInitHook  = DEF_NULL, // See Note #1.
  .OnReqHdrRxHook      = DEF_NULL, // See Note #2.
  .OnReqHook           = HTTPServerNoFSReqHook, // See Note #3.
  .OnReqBodyRxHook     = DEF_NULL, // See Note #4.
  .OnReqRdySignalHook  = DEF_NULL, // See Note #5.
  .OnReqRdyPollHook    = DEF_NULL, // See Note #6.
  .OnRespHdrTxHook     = HTTPServerNoFSRespHdrTxHook, // See Note #7.
  .OnRespTokenHook     = DEF_NULL, // See Note #8.
  .OnRespChunkHook     = HTTPServerNoFSRespChunkDataGetHook, // See Note #9.
  .OnTransCompleteHook = DEF_NULL, // See Note #10.
  .OnErrHook           = DEF_NULL, // See Note #11.
  .OnErrFileGetHook    = DEF_NULL, // See Note #12.
  .OnConnCloseHook     = HTTPSeverNoFSCloseHook // See Note #13.
};

// -----------------------------------------------------------------------------
// Global functions

/***************************************************************************//**
 * Initialize the HTTP server.
 ******************************************************************************/
void Webserver_Init(void)
{
  RTOS_ERR  err;

  HTTPs_Init(&err); // Initialize the HTTP server
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), ;);
}

/***************************************************************************//**
 * Initializes and starts a basic web server instance.
 *
 * Note(s)     : (1) HTTP server instance configurations content MUST remain persistent. You can also use
 *                   global variable or constants.
 *
 *               (2) Prior to do any call to HTTP module, it must be initialized. This is done by
 *                   calling HTTPs_Init(). If the process is successful, the Web server internal data structures are
 *                   initialized.
 *
 *               (3) Each web server must be initialized before it can be started or stopped. HTTPs_InstanceInit()
 *                   is responsible to allocate memory for the instance, initialize internal data structure and
 *                   create the web server instance's task.
 *
 *                   (a) The first argument is the instance configuration, which should be modified following you
 *                       requirements. The intance's configuration set the server's port, the number of connection that
 *                       can be accepted, the hooks functions, etc.
 *
 *                   (b) The second argument is the pointer to the instance task configuration. It sets the task priority,
 *                       the stack size of the task, etc.
 *
 *               (4) Once a web server instance is initialized, it can be started using HTTPs_InstanceStart() to
 *                   become come accessible. This function starts the web server instance's task. Each instance has
 *                   is own task and all accepted connection is processed with this single task.
 *
 *                   At this point you should be able to access your web server instance using the following
 *                   address in your favorite web browser:
 *
 *                       http://<target_ip_address>
 ******************************************************************************/
void Webserver_Start(void)
{
  RTOS_TASK_CFG   *p_cfg_task;
  HTTPs_CFG       *p_cfg_http;
  HTTPs_INSTANCE  *p_instance;
  RTOS_ERR         err;

  // Alloc cfg struct (note 1)
  p_cfg_task = (RTOS_TASK_CFG *)Mem_SegAlloc("Ex HTTP Server task cfg struct",
                                             DEF_NULL,
                                             sizeof(RTOS_TASK_CFG),
                                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

  p_cfg_http = (HTTPs_CFG *)Mem_SegAlloc("Ex HTTP Server cfg struct",
                                         DEF_NULL,
                                         sizeof(HTTPs_CFG),
                                         &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

  p_cfg_http->HdrTxCfgPtr = (HTTPs_HDR_TX_CFG *)Mem_SegAlloc("Ex HTTP Server hdr tx cfg struct",
                                                             DEF_NULL,
                                                             sizeof(HTTPs_HDR_TX_CFG),
                                                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

  // Prepare for cfg struct
  // Server instance task cfg
  p_cfg_task->Prio            = HTTP_SERVER_PRIO;
  p_cfg_task->StkSizeElements = 1024u;
  p_cfg_task->StkPtr          = DEF_NULL;

  // Server instance cfg
  ((HTTPs_HDR_TX_CFG    *)p_cfg_http->HdrTxCfgPtr)->NbrPerConnMax =  2u;
  ((HTTPs_HDR_TX_CFG    *)p_cfg_http->HdrTxCfgPtr)->DataLenMax    =  128u;
  p_cfg_http->HdrRxCfgPtr                                         =  DEF_NULL;
  p_cfg_http->QueryStrCfgPtr                                      =  DEF_NULL;
  p_cfg_http->FormCfgPtr                                          =  DEF_NULL;
  p_cfg_http->TokenCfgPtr                                         =  DEF_NULL;
  p_cfg_http->OS_TaskDly_ms                                       =  0u;
  p_cfg_http->SockSel                                             =  HTTPs_SOCK_SEL_IPv4;
  p_cfg_http->SecurePtr                                           =  DEF_NULL;
  p_cfg_http->Port                                                =  80u;
  p_cfg_http->ConnNbrMax                                          =  NR_CLIENT_MAX;
  p_cfg_http->ConnInactivityTimeout_s                             =  15u;
  p_cfg_http->BufLen                                              =  5000u;
  p_cfg_http->ConnPersistentEn                                    =  DEF_ENABLED;
  p_cfg_http->FS_Type                                             =  HTTPs_FS_TYPE_NONE;
  p_cfg_http->PathLenMax                                          =  128u;
  p_cfg_http->DfltResourceNamePtr                                 =  DFLT_URL_PATH;
  p_cfg_http->HostNameLenMax                                      =  128u;
  p_cfg_http->HooksPtr                                            = &Ex_HTTP_Server_HooksNoFS;
  p_cfg_http->Hooks_CfgPtr                                        =  DEF_NULL;

  p_instance = HTTPs_InstanceInit(p_cfg_http, // Instance configuration. See Note #3a
                                  p_cfg_task, // Instance task configuration. See Note #3b
                                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

  // Start web server instance
  HTTPs_InstanceStart(p_instance, // Instance handle. See Note #4
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
}

// -----------------------------------------------------------------------------
// Local functions

/**************************************************************************//**
 * Brief description of function.
 *
 * @param p_instance Pointer to the HTTPs instance object.
 * @param p_conn Pointer to the HTTPs connection object.
 * @param p_hook_cfg Pointer to hook configuration object.
 * @returns DEF_OK if the application allows the request to be continue.
 *          DEF_FAIL  otherwise.
 *
 * Description : Called after the parsing of an HTTP request message's first line and header(s).
 *               Allows the application to process the information received in the request message.
 *               Examples of behaviors that could be implemented :
 *
 *               (a) Analyze the Request-URI and validate that the client has the permission to access
 *                   the resource. If not, change the Response Status Code to 403 (Forbidden) or 401
 *                   (Unauthorized) if an Authentication technique is implemented. In case of a 401
 *                   Status, a "WWW-Authenticate" header needs to be added to the response message
 *                   (See HTTPs_InstanceRespHdrTx() function)
 *
 *               (b) Depending on whether the header feature is enabled and which header fields have been
 *                   chosen for use (see HTTPs_ReqHdrRxHook() function), different behaviors
 *                   are possible. Here are some examples :
 *
 *                       (1) A "Cookie" header is received. The default html page is modified to include
 *                           personalized features for the client.
 *
 *                       (2) An "Authorization" header is received. This validates that the client login is good and
 *                           changes permanently its' access to the folder/file.
 *
 *                       (3) An "If-Modified-Since" header is received. It then validates whether or not the resource
 *                           has been modified since the 'HTTP-date' received with the header. If it was, continue
 *                           with the request processing normally, else change the Status Code to 304 (Not Modified).
 *
 * Note(s)     : (1) The instance structure is for read-only. It must not be modified at any point in this hook function.
 *
 *               (2) The following connection attributes can be accessed to analyze the connection:
 *
 *                   (a) 'ClientAddr'
 *                           This connection parameter contains the IP address and port used by the remote client to access the
 *                           server instance.
 *
 *                   (b) 'Method'
 *                           HTTPs_METHOD_GET        Get  request
 *                           HTTPs_METHOD_POST       Post request
 *                           HTTPs_METHOD_HEAD       Head request
 *
 *                   (c) 'PathPtr'
 *                           This is a pointer to the string that contains the name of the file requested.
 *
 *                   (d) 'HdrCtr'
 *                           This parameter is a counter of the number of header field that has been stored.
 *
 *                   (e) 'HdrListPtr'
 *                           This parameter is a pointer to the first header field stored. A linked list is created with
 *                           all header field stored.
 *
 *               (3) In this hook function, only the under-mentioned connection parameters are allowed
 *                   to be modified :
 *
 *                   (a) 'StatusCode'
 *
 *                           See HTTPs_STATUS_CODE declaration in http-s.h for all the status code supported.
 *
 *                   (b) 'PathPtr'
 *
 *                           This is a pointer to the string that contains the name of the file requested. You can change
 *                           the name of the requested file to send another file instead without error.
 *
 *                   (c) 'DataPtr'
 *
 *                           This is a pointer to the data file or data to transmit. This parameter should be null when calling
 *                           this function. If data from memory has to be sent instead of a file, this pointer must be set
 *                           to the location of the data.
 *
 *                   (d) 'RespBodyDataType'
 *
 *                           HTTPs_BODY_DATA_TYPE_FILE              Open and transmit a file. Value by default.
 *                           HTTPs_BODY_DATA_TYPE_STATIC_DATA       Transmit data from the memory. Must be set by the hook function.
 *                           HTTPs_BODY_DATA_TYPE_NONE              No body in response.
 *
 *                   (e) 'DataLen'
 *
 *                           0,                              Default value, will be set when the file is opened.
 *                           Data length,                    Must be set by the data length when transmitting data from
 *                                                           the memory
 *
 *                   (f) 'ConnDataPtr'
 *
 *                           This is a pointer available for the upper application when memory block must be allocated
 *                           to process the connection request. If memory is allocated by the upper application, the memory
 *                           space can be deallocated into another hook function.
 *
 *               (4) When the Location of the requested file has changed, besides the Status Code to change (3xx),
 *                   the 'PathPtr' parameter needs to be updated. A "Location" header will be added automatically in
 *                   the response by the HTTP Server core with the new location.
 ******************************************************************************/
static CPU_BOOLEAN HTTPServerNoFSReqHook(const HTTPs_INSTANCE *p_instance,
                                         HTTPs_CONN *p_conn,
                                         const void *p_hook_cfg)
{
  uint16_t i;
  RTOS_ERR err;
  CPU_INT32U clientAddr;
  CPU_INT08U *pData;
  CPU_INT32U pDataLen;
  HTTP_CONTENT_TYPE contentType;
  static bool totClientNrExceeded = false;

  PP_UNUSED_PARAM(p_hook_cfg);

  pData = not_found_html;
  pDataLen = sizeof(not_found_html);
  contentType = HTTP_CONTENT_TYPE_HTML;

  clientAddr = (p_conn->ClientAddr.Addr[FIRST_ADR_INDEX] << 24)
               | (p_conn->ClientAddr.Addr[SECON_ADR_INDEX] << 16)
               | (p_conn->ClientAddr.Addr[THIRD_ADR_INDEX] << 8)
               | (p_conn->ClientAddr.Addr[FOURT_ADR_INDEX] << 0);

  printf("request for: %s\n", p_conn->PathPtr);

  if (Str_Cmp_N(p_conn->PathPtr, DFLT_URL_PATH, p_conn->PathLenMax) == 0) {
    printf("Client IP address: %d.%d.%d.%d\n",
           p_instance->ConnLastPtr->ClientAddr.Addr[2],
           p_instance->ConnLastPtr->ClientAddr.Addr[3],
           p_instance->ConnLastPtr->ClientAddr.Addr[4],
           p_instance->ConnLastPtr->ClientAddr.Addr[5]);

    pData = index_html;
    pDataLen = sizeof(index_html);
    contentType = HTTP_CONTENT_TYPE_HTML;
  } else if (Str_Cmp_N(p_conn->PathPtr, "/styles.css", p_conn->PathLenMax) == 0) {
    pData = styles_css;
    pDataLen = sizeof(styles_css);
    contentType = HTTP_CONTENT_TYPE_CSS;
  } else if (Str_Cmp_N(p_conn->PathPtr, "/logo.png", p_conn->PathLenMax) == 0) {
    pData = img_logo_png;
    pDataLen = img_logo_png_len;
    contentType = HTTP_CONTENT_TYPE_PNG;
  } else if ((Str_Cmp_N(p_conn->PathPtr, AUDIO_URL, p_conn->PathLenMax) == 0)
             | (Str_Cmp_N(p_conn->PathPtr, PCM_URL, p_conn->PathLenMax) == 0))
  {
    totClientNrExceeded = true;
    for (i = 0; i < NR_CLIENT_MAX; i++) {
      if (clients[i] == clientAddr) {
        totClientNrExceeded = false;
        break;
      }

      if (clients[i] == 0) {
        clients[i] = clientAddr;
        newConnection[i] = true;
        totClientNrExceeded = false;
        break;
      }
    }

    if (!totClientNrExceeded) {
      pData = DEF_NULL;
      pDataLen = 0; // using chunks: size doesn't matter here
      contentType = HTTP_CONTENT_TYPE_PLAIN;
    }
  } else {
    pData = not_found_html;
    pDataLen = sizeof(not_found_html);
    contentType = HTTP_CONTENT_TYPE_HTML;
    printf("404 not found '%s'\n", p_conn->PathPtr);
  }

  HTTPs_RespBodySetParamStaticData(p_instance,
                                   p_conn,
                                   contentType,
                                   pData,
                                   pDataLen,
                                   DEF_NO,
                                   &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return DEF_FAIL;
  } else {
    return DEF_OK;
  }
}

/***************************************************************************//**
 * Called each time the HTTP server is building a response message.
 * Allows for adding header fields to the response message according to
 * the application needs.
 *
 * @param p_instance Pointer to the HTTPs instance object.
 * @param p_conn Pointer to the HTTPs connection object.
 * @param p_hook_cfg Pointer to hook configuration object.
 * @returns DEF_YES, if the header fields are added without running into a error.
 *          DEF_NO, otherwise.
 *
 * Note(s)     : (1) The instance structure MUST NOT be modified.
 *
 *               (2) The connection structure MUST NOT be modified manually since the response is about to be
 *                   transmitted at this point. The only change to the connection structure should be the
 *                   addition of header fields for the response message through the function HTTPs_RespHdrGet().
 ******************************************************************************/
static CPU_BOOLEAN HTTPServerNoFSRespHdrTxHook(const HTTPs_INSTANCE *p_instance,
                                               HTTPs_CONN *p_conn,
                                               const void *p_hook_cfg)
{
  PP_UNUSED_PARAM(p_hook_cfg);

#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
  HTTPs_HDR_BLK  *p_resp_hdr_blk;
  const  HTTPs_CFG      *p_cfg;
  CPU_CHAR       *str_data;
  CPU_SIZE_T      str_len;
  RTOS_ERR        err;

  p_cfg = p_instance->CfgPtr;

  if (p_cfg->HdrTxCfgPtr == DEF_NULL) {
    return (DEF_NO);
  }

  switch (p_conn->StatusCode) {
    case HTTP_STATUS_OK:

      if (p_conn->ReqContentType == HTTP_CONTENT_TYPE_HTML) {
        // Add server HDR field
        // Get and add header block to the connection
        p_resp_hdr_blk = HTTPs_RespHdrGet(p_instance,
                                          p_conn,
                                          HTTP_HDR_FIELD_SERVER,
                                          HTTPs_HDR_VAL_TYPE_STR_DYN,
                                          &err);
        if (p_resp_hdr_blk == DEF_NULL) {
          return(DEF_FAIL);
        }

        str_data = "uC-HTTP-server"; // Build Server string value

        str_len = Str_Len_N(str_data, p_cfg->HdrTxCfgPtr->DataLenMax);

        // update hdr blk parameter
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

/***************************************************************************//**
 * Called each time the HTTP server is trasmitting new chunk.
 *
 * @param p_instance Pointer to the HTTPs instance object.
 * @param p_conn Pointer to the HTTPs connection object.
 * @param p_hook_cfg Pointer to hook configuration object.
 * @param p_buf Pointer to the data to be trasmitted
 * @param buf_free_lim Available buffer size
 * @param len_to_tx Pointer to the size of the data
 * @returns DEF_YES, if there is no more chunks to be trasmitted.
 *          DEF_NO,  otherwise.
 *
 * Note(s): (1) The instance structure MUST NOT be modified.
 *
 *          (2) The connection structure MUST NOT be modified manually
 *              since a new chunk is about to be transmitted at this point.
 ******************************************************************************/
static  CPU_BOOLEAN  HTTPServerNoFSRespChunkDataGetHook(const  HTTPs_INSTANCE  *p_instance,
                                                        HTTPs_CONN      *p_conn,
                                                        const  void     *p_hook_cfg,
                                                        void            *p_buf,
                                                        CPU_SIZE_T      buf_free_lim,
                                                        CPU_SIZE_T      *len_to_tx)
{
  PP_UNUSED_PARAM(p_instance); // Prevent compiler warning
  PP_UNUSED_PARAM(p_hook_cfg);
  PP_UNUSED_PARAM(buf_free_lim);
  RTOS_ERR       err;
  CPU_TS         ts;
  ogg_page oggPage = { 0 };
  uint16_t clientNR;
  CPU_INT32U clientAddr;

  clientAddr = (p_conn->ClientAddr.Addr[FIRST_ADR_INDEX] << 24)
               | (p_conn->ClientAddr.Addr[SECON_ADR_INDEX] << 16)
               | (p_conn->ClientAddr.Addr[THIRD_ADR_INDEX] << 8)
               | (p_conn->ClientAddr.Addr[FOURT_ADR_INDEX] << 0);

  for (clientNR = 0; clientNR < NR_CLIENT_MAX; clientNR++) {
    if (clients[clientNR] == clientAddr) {
      break;
    }
  }

#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
  OSFlagPend(&micFlags,
             (OS_FLAGS)0x1 << clientNR,
             (OS_TICK )0,
             (OS_OPT)OS_OPT_PEND_FLAG_SET_ANY,
             &ts,
             &err);

  OSMutexPend(&audioMutex,
              0,
              OS_OPT_PEND_BLOCKING,
              (CPU_TS *)0,
              &err);

  if (Str_Cmp_N(p_conn->PathPtr, AUDIO_URL, p_conn->PathLenMax) == 0) {
    if (newConnection[clientNR]) {
    // Send ID Header
      oggPage.header = (unsigned char *)&idPacket_oggHeader;
      oggPage.header_len = sizeof(idPacket_oggHeader);
      oggPage.body = (unsigned char *)&idPacket_oggBody;
      oggPage.body_len = sizeof(idPacket_oggBody);
      ogg_page_checksum_set(&oggPage);
      Mem_Move(p_buf,
               oggPage.header,
               oggPage.header_len);
      Mem_Move((unsigned char *)p_buf + oggPage.header_len,
               oggPage.body,
               oggPage.body_len);
      *len_to_tx = oggPage.header_len + oggPage.body_len;

      // Send Comment Header
      oggPage.header = (unsigned char *)&commentPacket_oggHeader;
      oggPage.header_len = sizeof(commentPacket_oggHeader);
      oggPage.body = (unsigned char *)&commentPacket_oggBody;
      oggPage.body_len = sizeof(commentPacket_oggBody);
      ogg_page_checksum_set(&oggPage);
      Mem_Move((char *)p_buf + *len_to_tx,
               oggPage.header,
               oggPage.header_len);
      Mem_Move((unsigned char *)p_buf + oggPage.header_len + *len_to_tx,
               oggPage.body,
               oggPage.body_len);
      *len_to_tx += oggPage.header_len + oggPage.body_len;

      newConnection[clientNR] = false;
    } else {
      *len_to_tx = micData.oggPage->header_len + micData.oggPage->body_len;
      Mem_Move(p_buf,
               micData.oggPage->header,
               micData.oggPage->header_len);
      Mem_Move((unsigned char *)p_buf + micData.oggPage->header_len,
               micData.oggPage->body,
               micData.oggPage->body_len);
    }
  } else if (Str_Cmp_N(p_conn->PathPtr, PCM_URL, p_conn->PathLenMax) == 0) {
    *len_to_tx = SAMPLE_BUFFER_LEN*2;
    Mem_Move(p_buf,
             micData.pcmBuf,
             *len_to_tx);
  }

  OSMutexPost(&audioMutex,
              OS_OPT_POST_NONE,
              &err);

  OSFlagPost(&micFlags,
             (OS_FLAGS)0x1 << clientNR,
             (OS_OPT)OS_OPT_POST_FLAG_CLR,
             &err);

  return (DEF_NO);
#else
  CPU_SW_EXCEPTION(; );
  return (DEF_YES);
#endif
}

/***************************************************************************//**
 * Called each time the HTTP server connection is lost
 *
 * @param p_instance Pointer to the HTTPs instance object.
 * @param p_conn Pointer to the HTTPs connection object.
 * @param p_hook_cfg Pointer to hook configuration object.
 *
 * Note(s): (1) The instance structure MUST NOT be modified.
 *
 *          (2) The connection structure MUST NOT be modified manually
 ******************************************************************************/
static void HTTPSeverNoFSCloseHook(const  HTTPs_INSTANCE  *p_instance,
                                   HTTPs_CONN      *p_conn,
                                   const  void     *p_hook_cfg)
{
  PP_UNUSED_PARAM(p_hook_cfg);
  CPU_INT32U clientAddr;
  uint16_t i;

  if ((Str_Cmp_N(p_conn->PathPtr, AUDIO_URL, p_conn->PathLenMax) == 0)
     | (Str_Cmp_N(p_conn->PathPtr, PCM_URL, p_conn->PathLenMax) == 0))
  {
    clientAddr = (p_conn->ClientAddr.Addr[FIRST_ADR_INDEX] << 24)
                  | (p_conn->ClientAddr.Addr[SECON_ADR_INDEX] << 16)
                  | (p_conn->ClientAddr.Addr[THIRD_ADR_INDEX] << 8)
                  | (p_conn->ClientAddr.Addr[FOURT_ADR_INDEX] << 0);

    for (i = 0; i < NR_CLIENT_MAX; i++) {
      if (clients[i] == clientAddr) {
        clients[i] = 0;
      }
    }

    printf("Closing Client IP address: %d.%d.%d.%d\n",
           p_instance->ConnLastPtr->ClientAddr.Addr[2],
           p_instance->ConnLastPtr->ClientAddr.Addr[3],
           p_instance->ConnLastPtr->ClientAddr.Addr[4],
           p_instance->ConnLastPtr->ClientAddr.Addr[5]);
  }
}
