/***************************************************************************//**
 * @file
 * @brief Example - HTTP Client Application Functions File
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

#if (defined(RTOS_MODULE_NET_HTTP_CLIENT_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <ex_description.h>

#include  "ex_http_client.h"
#include  "ex_http_client_hooks.h"
#include  "ex_http_client_files.h"

#include  <common/include/rtos_path.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/lib_str.h>
#include  <common/include/lib_utils.h>

#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
#include  <fs/include/fs_core.h>
#include  <fs/include/fs_core_cache.h>
#include  <fs/include/fs_core_file.h>
#include  <fs/include/fs_core_partition.h>
#include  <fs/include/fs_core_vol.h>
#include  <fs/include/fs_fat.h>
#include  <fs/include/fs_ramdisk.h>
#endif

#include  <net/include/net_app.h>

#include  <http_client_cfg.h>

#ifdef RTOS_MODULE_NET_SSL_TLS_AVAIL
#include  "../../ssl_tls/ex_ssl_certificates.h"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  EX_HTTP_CLIENT_RAMDISK_SEC_SIZE
#define  EX_HTTP_CLIENT_RAMDISK_SEC_SIZE                        512u
#endif

#ifndef  EX_HTTP_CLIENT_RAMDISK_SEC_NBR
#define  EX_HTTP_CLIENT_RAMDISK_SEC_NBR                         60u
#endif

#ifndef  EX_HTTP_CLIENT_FILE_RAM_MEDIA_NAME
#define  EX_HTTP_CLIENT_FILE_RAM_MEDIA_NAME                    "ram_httpc"
#endif

#ifndef  EX_HTTP_CLIENT_FILE_VOL_NAME
#define  EX_HTTP_CLIENT_FILE_VOL_NAME                          "ram_httpc"
#endif

#ifndef  EX_HTTP_CLIENT_WRK_DIR
#define  EX_HTTP_CLIENT_WRK_DIR                                 EX_HTTP_CLIENT_FILE_VOL_NAME
#endif

#ifndef  EX_HTTP_CLIENT_FILE_NAME
#define  EX_HTTP_CLIENT_FILE_NAME                              "index.html"
#endif

#ifndef  EX_HTTP_CLIENT_CLIENT_HOSTNAME
#define  EX_HTTP_CLIENT_CLIENT_HOSTNAME                        "httpbin.org"
#endif

#ifndef  EX_HTTP_CLIENT_CONN_NBR_MAX
#define  EX_HTTP_CLIENT_CONN_NBR_MAX                            5u
#endif

#ifndef  EX_HTTP_CLIENT_REQ_NBR_MAX
#define  EX_HTTP_CLIENT_REQ_NBR_MAX                             5u
#endif

#ifndef  EX_HTTP_CLIENT_CONN_BUF_SIZE
#define  EX_HTTP_CLIENT_CONN_BUF_SIZE                           512u
#endif

/********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 *******************************************************************************************************/

#ifndef EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

HTTPc_HDR Ex_HTTP_Client_RespHdrTbl[EX_HTTP_CLIENT_CFG_HDR_NBR_MAX];
CPU_CHAR  Ex_HTTP_Client_RespHdrValStrTbl[EX_HTTP_CLIENT_CFG_HDR_NBR_MAX][EX_HTTP_CLIENT_CFG_HDR_VAL_LEN_MAX];

#if (HTTPc_CFG_USER_DATA_EN == DEF_ENABLED)
static EX_HTTP_CLIENT_REQ_DATA Ex_HTTP_Client_Data[EX_HTTP_CLIENT_REQ_NBR_MAX];
#endif

static HTTPc_CONN_OBJ Ex_HTTP_Client_ConnTbl[EX_HTTP_CLIENT_CONN_NBR_MAX];
static HTTPc_REQ_OBJ  Ex_HTTP_Client_ReqTbl[EX_HTTP_CLIENT_REQ_NBR_MAX];
static HTTPc_RESP_OBJ Ex_HTTP_Client_RespTbl[EX_HTTP_CLIENT_REQ_NBR_MAX];

static CPU_CHAR Ex_HTTP_Client_ConnBufTbl[EX_HTTP_CLIENT_CONN_NBR_MAX][EX_HTTP_CLIENT_CONN_BUF_SIZE];

#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
static HTTPc_FORM_TBL_FIELD Ex_HTTP_Client_FormTbl[EX_HTTP_CLIENT_CFG_FORM_FIELD_NBR_MAX];

static HTTPc_KEY_VAL     Ex_HTTP_Client_FormKeyValTbl[EX_HTTP_CLIENT_CFG_FORM_FIELD_NBR_MAX];
static HTTPc_KEY_VAL_EXT Ex_HTTP_Client_FormKeyValExtTbl[EX_HTTP_CLIENT_CFG_FORM_FIELD_NBR_MAX];
#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
static HTTPc_MULTIPART_FILE Ex_HTTP_Client_FormMultipartFileTbl[EX_HTTP_CLIENT_CFG_FORM_FIELD_NBR_MAX];
#endif
static CPU_CHAR Ex_HTTP_Client_FormKeyStrTbl[2 * EX_HTTP_CLIENT_CFG_FORM_FIELD_NBR_MAX][EX_HTTP_CLIENT_CFG_FORM_FIELD_KEY_LEN_MAX];
#endif

HTTPc_KEY_VAL Ex_HTTP_Client_ReqQueryStrTbl[EX_HTTP_CLIENT_CFG_QUERY_STR_NBR_MAX];
CPU_CHAR      Ex_HTTP_Client_ReqQueryStrKeyTbl[EX_HTTP_CLIENT_CFG_QUERY_STR_NBR_MAX][EX_HTTP_CLIENT_CFG_QUERY_STR_KEY_LEN_MAX];
CPU_CHAR      Ex_HTTP_Client_ReqQueryStrValTbl[EX_HTTP_CLIENT_CFG_QUERY_STR_NBR_MAX][EX_HTTP_CLIENT_CFG_QUERY_STR_VAL_LEN_MAX];

HTTPc_HDR Ex_HTTP_Client_ReqHdrTbl[EX_HTTP_CLIENT_CFG_HDR_NBR_MAX];
CPU_CHAR  Ex_HTTP_Client_ReqHdrValStrTbl[EX_HTTP_CLIENT_CFG_HDR_NBR_MAX][EX_HTTP_CLIENT_CFG_HDR_VAL_LEN_MAX];

CPU_CHAR Ex_HTTP_Client_FormValStrTbl[2 * EX_HTTP_CLIENT_CFG_FORM_FIELD_NBR_MAX][EX_HTTP_CLIENT_CFG_FORM_FIELD_VAL_LEN_MAX];
#if (defined(RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL) && (HTTPc_CFG_FORM_EN == DEF_ENABLED))
static CPU_CHAR Ex_HTTP_Client_FormMultipartNameStrTbl[EX_HTTP_CLIENT_CFG_FORM_FIELD_NBR_MAX][EX_HTTP_CLIENT_CFG_FORM_MULTIPART_NAME_LEN_MAX];
static CPU_CHAR Ex_HTTP_Client_FormMultipartFileNameStrTbl[EX_HTTP_CLIENT_CFG_FORM_FIELD_NBR_MAX][EX_HTTP_CLIENT_CFG_FORM_MULTIPART_FILENAME_LEN_MAX];
#endif

CPU_CHAR Ex_HTTP_Client_FormBuf[EX_HTTP_CLIENT_CFG_FORM_BUF_SIZE];

#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
static CPU_INT08U Ex_HTTP_Client_RAMDisk[EX_HTTP_CLIENT_RAMDISK_SEC_SIZE * EX_HTTP_CLIENT_RAMDISK_SEC_NBR];
static FS_CACHE   *Ex_HTTP_Client_CachePtr;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
static void Ex_HTTP_Client_FS_MediaPrepare(void);

static void Ex_HTTP_Client_FS_FileAdd(FS_WRK_DIR_HANDLE wrk_dir_handle,
                                      CPU_CHAR          *p_file_name,
                                      CPU_CHAR          *p_file_content,
                                      CPU_SIZE_T        file_len);
#endif

static CPU_INT08U Ex_HTTP_Client_ReqPrepareQueryStr(HTTPc_KEY_VAL **p_tbl);

static CPU_INT08U Ex_HTTP_Client_ReqPrepareHdrs(HTTPc_HDR **p_hdr_tbl);

#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
static CPU_INT08U Ex_HTTP_Client_ReqPrepareFormApp(HTTPc_FORM_TBL_FIELD **p_form_tbl);

static CPU_INT08U Ex_HTTP_Client_ReqPrepareFormMultipart(CPU_CHAR             *p_wrk_dir,
                                                         HTTPc_FORM_TBL_FIELD **p_form_tbl);

#ifdef RTOS_MODULE_NET_SSL_TLS_AVAIL
static CPU_BOOLEAN Ex_HTTP_Client_Secure_TrustCallback(void                             *p_cert_dn,
                                                       NET_SOCK_SECURE_UNTRUSTED_REASON reason);
#endif
#endif

/****************************************************************************************************//**
 *                                          Ex_HTTP_Client_Init()
 *
 * @brief  Initialize the HTTP Client stack for the example application.
 *******************************************************************************************************/
void Ex_HTTP_Client_Init(void)
{
  RTOS_ERR err;

#ifdef EX_HTTP_CLIENT_TASK_STK_SIZE
  HTTPc_ConfigureTaskStk(EX_HTTP_CLIENT_TASK_STK_SIZE, DEF_NULL);
#endif

  //                                                               ------------- INITIALIZE CLIENT SUITE --------------
  HTTPc_Init(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

#ifdef RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
  Ex_HTTP_Client_FS_MediaPrepare();
#endif
}

/****************************************************************************************************//**
 *                                    Ex_HTTP_Client_ReqSendGetNoTask()
 *
 * @brief  Send a GET request without using the HTTP client internal task and retrieve the HTTP
 *         response.
 *******************************************************************************************************/

#if (HTTPc_CFG_MODE_ASYNC_TASK_EN != DEF_ENABLED)
void Ex_HTTP_Client_ReqSendGet_NoTask(void)
{
  HTTPc_CONN_OBJ *p_conn;
  HTTPc_REQ_OBJ  *p_req;
  HTTPc_RESP_OBJ *p_resp;
  CPU_CHAR       *p_buf;
  CPU_SIZE_T     str_len;
  RTOS_ERR       err;

  p_conn = &Ex_HTTP_Client_ConnTbl[0u];
  p_req = &Ex_HTTP_Client_ReqTbl[0u];
  p_resp = &Ex_HTTP_Client_RespTbl[0u];
  p_buf = &Ex_HTTP_Client_ConnBufTbl[0u][0u];

  //                                                               ----------- INIT NEW CONNECTION & REQUEST ----------
  HTTPc_ConnClr(p_conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqClr(p_req, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------ SET REQ/RESP HOOK FUNCTIONS -----------
  HTTPc_ReqSetParam(p_req,                                      // Set hook to retrieve headers received in response.
                    HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
                    (void *)Ex_HTTP_Client_RespHdrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req,                                      // Set hook to retrieve the response body data.
                    HTTPc_PARAM_TYPE_RESP_BODY_HOOK,
                    (void *)Ex_HTTP_Client_RespBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ----------------- OPEN CONNECTION ------------------
  str_len = Str_Len(EX_HTTP_CLIENT_CLIENT_HOSTNAME);
  (void)HTTPc_ConnOpen(p_conn,
                       p_buf,
                       EX_HTTP_CLIENT_CONN_BUF_SIZE,
                       EX_HTTP_CLIENT_CLIENT_HOSTNAME,
                       str_len,
                       HTTPc_FLAG_NONE,
                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Connection to server succeeded.\n\r");

  //                                                               ---------------- SEND HTTP REQUEST -----------------
  str_len = Str_Len("/get");                                    // Send request to "www.httpbin.org/get"
  (void)HTTPc_ReqSend(p_conn,
                      p_req,
                      p_resp,
                      HTTP_METHOD_GET,
                      "/get",
                      str_len,
                      HTTPc_FLAG_NONE,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("%s\n\r", p_resp->ReasonPhrasePtr);
}
#endif

/****************************************************************************************************//**
 *                                       Ex_HTTP_Client_ReqSendGet()
 *
 * @brief  Send a GET request with a Query String and additional headers.
 *******************************************************************************************************/
void Ex_HTTP_Client_ReqSendGet(void)
{
#ifdef RTOS_MODULE_NET_SSL_TLS_AVAIL
  NET_APP_SOCK_SECURE_CFG secure_cfg;
#endif
  HTTPc_CONN_OBJ  *p_conn;
  HTTPc_REQ_OBJ   *p_req;
  HTTPc_RESP_OBJ  *p_resp;
  CPU_CHAR        *p_buf;
  HTTPc_KEY_VAL   *p_query_str_tbl;
  HTTPc_HDR       *p_hdr_tbl;
  HTTPc_PARAM_TBL tbl_obj;
  CPU_INT08U      query_nbr;
  CPU_INT08U      ext_hdr_nbr;
  CPU_SIZE_T      str_len;
  RTOS_ERR        err;

  p_conn = &Ex_HTTP_Client_ConnTbl[0u];
  p_req = &Ex_HTTP_Client_ReqTbl[0u];
  p_resp = &Ex_HTTP_Client_RespTbl[0u];
  p_buf = &Ex_HTTP_Client_ConnBufTbl[0u][0u];

#if (HTTPc_CFG_USER_DATA_EN == DEF_ENABLED)
  p_req->UserDataPtr = (void *)&Ex_HTTP_Client_Data[0u];
#else
  APP_RTOS_ASSERT_CRITICAL_FAIL(; );
#endif

  //                                                               -------------- SET STRING QUERY DATA ---------------
  query_nbr = Ex_HTTP_Client_ReqPrepareQueryStr(&p_query_str_tbl);
  if (query_nbr == 0u) {
    APP_RTOS_ASSERT_CRITICAL_FAIL(; );
  }

  //                                                               -------------- SET ADDITIONAL HEADERS --------------
  ext_hdr_nbr = Ex_HTTP_Client_ReqPrepareHdrs(&p_hdr_tbl);
  if (ext_hdr_nbr == 0u) {
    APP_RTOS_ASSERT_CRITICAL_FAIL(; );
  }

  //                                                               ----------- INIT NEW CONNECTION & REQUEST ----------
  HTTPc_ConnClr(p_conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqClr(p_req, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------- SET CONNECTION CALLBACKS -------------
#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
  HTTPc_ConnSetParam(p_conn,
                     HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK,
                     (void *)Ex_HTTP_Client_ConnCloseCallback,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  //                                                               -------------- SET SSL TRUST CALLBACK --------------
#ifdef RTOS_MODULE_NET_SSL_TLS_AVAIL
  //                                                               TODO: Assign your own client certificate/private key
  //                                                               if required by the server
#if  NET_SECURE_CFG_CLIENT_AUTH_EN == DEF_ENABLED
  secure_cfg.CertPtr = (CPU_CHAR *)SSL_CLIENT_CERT;
  secure_cfg.CertSize = SSL_ClientCertLen;
  secure_cfg.KeyPtr = (CPU_CHAR *)SSL_CLIENT_KEY;
  secure_cfg.KeySize = SSL_ClientKeyLen;
#else
  secure_cfg.CertPtr = DEF_NULL;
  secure_cfg.CertSize = 0;
  secure_cfg.KeyPtr = DEF_NULL;
  secure_cfg.KeySize = 0;
#endif
  secure_cfg.CertFmt = NET_SOCK_SECURE_CERT_KEY_FMT_PEM;
  secure_cfg.TrustCallback = Ex_HTTP_Client_Secure_TrustCallback;
  secure_cfg.CommonName = EX_HTTP_CLIENT_CLIENT_HOSTNAME;
  secure_cfg.CertChain = DEF_YES;

  HTTPc_ConnSetParam(p_conn,
                     HTTPc_PARAM_TYPE_SECURE_CFG_PTR,
                     (void *)&secure_cfg,
                     &err);
#endif

  //                                                               ------------ SET STRING QUERY PARAMETERS -----------
  tbl_obj.EntryNbr = query_nbr;
  tbl_obj.TblPtr = (void *)p_query_str_tbl;
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_QUERY_STR_TBL,
                    &tbl_obj,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ---------- SET REQUEST ADDITIONAL HEADERS ----------
  tbl_obj.EntryNbr = ext_hdr_nbr;
  tbl_obj.TblPtr = (void *)p_hdr_tbl;
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_HDR_TBL,
                    &tbl_obj,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------ SET REQ/RESP HOOK FUNCTIONS -----------
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
                    (void *)Ex_HTTP_Client_RespHdrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_RESP_BODY_HOOK,
                    (void *)Ex_HTTP_Client_RespBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               --------- SET REQ/RESP CALLBACK FUNCTIONS ----------
#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK,
                    (void *)Ex_HTTP_Client_TransErrCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  //                                                               ----------------- OPEN CONNECTION ------------------
  EX_TRACE(("Attempting to connect to HTTP server...\n\r"));

  str_len = Str_Len(EX_HTTP_CLIENT_CLIENT_HOSTNAME);
  (void)HTTPc_ConnOpen(p_conn,
                       p_buf,
                       EX_HTTP_CLIENT_CONN_BUF_SIZE,
                       EX_HTTP_CLIENT_CLIENT_HOSTNAME,
                       str_len,
                       HTTPc_FLAG_NONE,
                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE(("Connection to server succeeded.\n\r"));

  //                                                               ---------------- SEND HTTP REQUEST -----------------
  str_len = Str_Len("/get");
  (void)HTTPc_ReqSend(p_conn,
                      p_req,
                      p_resp,
                      HTTP_METHOD_GET,
                      "/get",
                      str_len,
                      HTTPc_FLAG_NONE,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("%s\n\r", p_resp->ReasonPhrasePtr);
}

/****************************************************************************************************//**
 *                                      Ex_HTTP_Client_ReqSendPost()
 *
 * @brief  Send a POST request with a pre-formatted form as body.
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
void Ex_HTTP_Client_ReqSendPost(void)
{
  HTTPc_CONN_OBJ       *p_conn;
  HTTPc_REQ_OBJ        *p_req;
  HTTPc_RESP_OBJ       *p_resp;
  CPU_CHAR             *p_buf;
  CPU_SIZE_T           str_len;
  HTTPc_FORM_TBL_FIELD *p_form_tbl;
  CPU_INT08U           form_nbr;
  CPU_INT32U           content_len;
  HTTP_CONTENT_TYPE    content_type;
  CPU_CHAR             *p_data;
  RTOS_ERR             err;

  p_conn = &Ex_HTTP_Client_ConnTbl[0u];
  p_req = &Ex_HTTP_Client_ReqTbl[0u];
  p_resp = &Ex_HTTP_Client_RespTbl[0u];
  p_buf = &Ex_HTTP_Client_ConnBufTbl[0u][0u];

#if (HTTPc_CFG_USER_DATA_EN == DEF_ENABLED)
  p_req->UserDataPtr = (void *)&Ex_HTTP_Client_Data[0u];
#else
  APP_RTOS_ASSERT_CRITICAL_FAIL(; );
#endif

  //                                                               ----------------- SET FORM TO SEND -----------------
  form_nbr = Ex_HTTP_Client_ReqPrepareFormApp(&p_form_tbl);
  if (form_nbr <= 0u) {
    APP_RTOS_ASSERT_CRITICAL_FAIL(; );
  }

  p_data = &Ex_HTTP_Client_FormBuf[0u];
  content_len = HTTPc_FormAppFmt(p_data,
                                 EX_HTTP_CLIENT_CFG_FORM_BUF_SIZE,
                                 p_form_tbl,
                                 form_nbr,
                                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ----------- INIT NEW CONNECTION & REQUEST ----------
  HTTPc_ConnClr(p_conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqClr(p_req, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------- SET CONNECTION CALLBACKS -------------
#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
  HTTPc_ConnSetParam(p_conn,
                     HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK,
                     (void *)Ex_HTTP_Client_ConnCloseCallback,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  //                                                               ------------ SET STRING QUERY PARAMETERS -----------
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_QUERY_STR_HOOK,
                    (void *)Ex_HTTP_Client_ReqQueryStrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  //                                                               ---------- SET REQUEST ADDITIONAL HEADERS ----------
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_HDR_HOOK,
                    (void *)Ex_HTTP_Client_ReqHdrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ----------- SET REQUEST BODY PARAMETERS ------------
  content_type = HTTP_CONTENT_TYPE_APP_FORM;
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_TYPE,
                    &content_type,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_LEN,
                    &content_len,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_BODY_HOOK,
                    (void *)Ex_HTTP_Client_ReqBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------ SET REQ/RESP HOOK FUNCTIONS -----------
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
                    (void *)Ex_HTTP_Client_RespHdrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_RESP_BODY_HOOK,
                    (void *)Ex_HTTP_Client_RespBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               --------- SET REQ/RESP CALLBACK FUNCTIONS ----------
#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK,
                    (void *)Ex_HTTP_Client_TransErrCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  //                                                               ----------------- OPEN CONNECTION ------------------
  str_len = Str_Len(EX_HTTP_CLIENT_CLIENT_HOSTNAME);
  (void)HTTPc_ConnOpen(p_conn,
                       p_buf,
                       EX_HTTP_CLIENT_CONN_BUF_SIZE,
                       EX_HTTP_CLIENT_CLIENT_HOSTNAME,
                       str_len,
                       HTTPc_FLAG_NONE,
                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE(("Connection to server succeeded.\n\r"));

  //                                                               ---------------- SEND HTTP REQUEST -----------------
  str_len = Str_Len("/post");
  (void)HTTPc_ReqSend(p_conn,
                      p_req,
                      p_resp,
                      HTTP_METHOD_POST,
                      "/post",
                      str_len,
                      HTTPc_FLAG_NONE,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("%s\n\r", p_resp->ReasonPhrasePtr);
}
#endif

/****************************************************************************************************//**
 *                                     Ex_HTTP_Client_ReqSendAppForm()
 *
 * @brief  Send an Application type form.
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
void Ex_HTTP_Client_ReqSendAppForm(void)
{
  HTTPc_CONN_OBJ       *p_conn;
  HTTPc_REQ_OBJ        *p_req;
  HTTPc_RESP_OBJ       *p_resp;
  CPU_CHAR             *p_buf;
  HTTPc_PARAM_TBL      tbl_obj;
  CPU_SIZE_T           str_len;
  HTTPc_FORM_TBL_FIELD *p_form_tbl;
  CPU_INT08U           form_nbr;
  HTTP_CONTENT_TYPE    content_type;
  RTOS_ERR             err;

  p_conn = &Ex_HTTP_Client_ConnTbl[0u];
  p_req = &Ex_HTTP_Client_ReqTbl[0u];
  p_resp = &Ex_HTTP_Client_RespTbl[0u];
  p_buf = &Ex_HTTP_Client_ConnBufTbl[0u][0u];

#if (HTTPc_CFG_USER_DATA_EN == DEF_ENABLED)
  p_req->UserDataPtr = (void *)&Ex_HTTP_Client_Data[0u];
#else
  APP_RTOS_ASSERT_CRITICAL_FAIL(; );
#endif

  //                                                               ----------------- SET FORM TO SEND -----------------
  form_nbr = Ex_HTTP_Client_ReqPrepareFormApp(&p_form_tbl);
  if (form_nbr <= 0u) {
    APP_RTOS_ASSERT_CRITICAL_FAIL(; );
  }

  //                                                               ----------- INIT NEW CONNECTION & REQUEST ----------
  HTTPc_ConnClr(p_conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqClr(p_req, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------- SET CONNECTION CALLBACKS -------------
#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
  HTTPc_ConnSetParam(p_conn,
                     HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK,
                     (void *)Ex_HTTP_Client_ConnCloseCallback,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  //                                                               ----------- SET REQUEST BODY PARAMETERS ------------
  content_type = HTTP_CONTENT_TYPE_APP_FORM;
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_TYPE,
                    &content_type,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  tbl_obj.EntryNbr = form_nbr;
  tbl_obj.TblPtr = (void *)p_form_tbl;
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_FORM_TBL,
                    &tbl_obj,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------ SET REQ/RESP HOOK FUNCTIONS -----------
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
                    (void *)Ex_HTTP_Client_RespHdrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_RESP_BODY_HOOK,
                    (void *)Ex_HTTP_Client_RespBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               --------- SET REQ/RESP CALLBACK FUNCTIONS ----------
#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK,
                    (void *)Ex_HTTP_Client_TransErrCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  //                                                               ----------------- OPEN CONNECTION ------------------
  str_len = Str_Len(EX_HTTP_CLIENT_CLIENT_HOSTNAME);
  (void)HTTPc_ConnOpen(p_conn,
                       p_buf,
                       EX_HTTP_CLIENT_CONN_BUF_SIZE,
                       EX_HTTP_CLIENT_CLIENT_HOSTNAME,
                       str_len,
                       HTTPc_FLAG_NONE,
                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE(("Connection to server succeeded.\n\r"));

  //                                                               ---------------- SEND HTTP REQUEST -----------------
  str_len = Str_Len("/post");
  (void)HTTPc_ReqSend(p_conn,
                      p_req,
                      p_resp,
                      HTTP_METHOD_POST,
                      "/post",
                      str_len,
                      HTTPc_FLAG_NONE,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("%s\n\r", p_resp->ReasonPhrasePtr);
}
#endif

/****************************************************************************************************//**
 *                                  Ex_HTTP_Client_ReqSendMultipartForm()
 *
 * @brief  Send a multipart type form.
 *
 * @param  p_wrk_dir  String that contains the directory where the index.html file to send as part
 *                    of the form is stored. Can be DEF_NULL if Micrium OS File System is not
 *                    present or if no file is desired as part of the form.
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
void Ex_HTTP_Client_ReqSendMultipartForm(CPU_CHAR *p_wrk_dir)
{
  HTTPc_CONN_OBJ       *p_conn;
  HTTPc_REQ_OBJ        *p_req;
  HTTPc_RESP_OBJ       *p_resp;
  CPU_CHAR             *p_buf;
  HTTPc_PARAM_TBL      tbl_obj;
  CPU_SIZE_T           str_len;
  HTTPc_FORM_TBL_FIELD *p_form_tbl;
  CPU_INT08U           form_nbr;
  HTTP_CONTENT_TYPE    content_type;
  RTOS_ERR             err;

  p_conn = &Ex_HTTP_Client_ConnTbl[0u];
  p_req = &Ex_HTTP_Client_ReqTbl[0u];
  p_resp = &Ex_HTTP_Client_RespTbl[0u];
  p_buf = &Ex_HTTP_Client_ConnBufTbl[0u][0u];

#if (HTTPc_CFG_USER_DATA_EN == DEF_ENABLED)
  p_req->UserDataPtr = (void *)&Ex_HTTP_Client_Data[0u];
#ifdef RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
  {
    EX_HTTP_CLIENT_REQ_DATA *p_req_data = (EX_HTTP_CLIENT_REQ_DATA *)p_req->UserDataPtr;

    p_req_data->WrkDirPtr = p_wrk_dir;
    p_req_data->FileHandle = FSFile_NullHandle;
  }
#endif
#else
  APP_RTOS_ASSERT_CRITICAL_FAIL(; );
#endif

  //                                                               ----------------- SET FORM TO SEND -----------------
  form_nbr = Ex_HTTP_Client_ReqPrepareFormMultipart(p_wrk_dir, &p_form_tbl);
  if (form_nbr <= 0u) {
    APP_RTOS_ASSERT_CRITICAL_FAIL(; );
  }

  //                                                               ----------- INIT NEW CONNECTION & REQUEST ----------
  HTTPc_ConnClr(p_conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqClr(p_req, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------- SET CONNECTION CALLBACKS -------------
#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
  HTTPc_ConnSetParam(p_conn,
                     HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK,
                     (void *)Ex_HTTP_Client_ConnCloseCallback,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  //                                                               ----------- SET REQUEST BODY PARAMETERS ------------
  content_type = HTTP_CONTENT_TYPE_MULTIPART_FORM;
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_TYPE,
                    &content_type,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  tbl_obj.EntryNbr = form_nbr;
  tbl_obj.TblPtr = (void *)p_form_tbl;
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_FORM_TBL,
                    &tbl_obj,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------ SET REQ/RESP HOOK FUNCTIONS -----------
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
                    (void *)Ex_HTTP_Client_RespHdrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_RESP_BODY_HOOK,
                    (void *)Ex_HTTP_Client_RespBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               --------- SET REQ/RESP CALLBACK FUNCTIONS ----------
#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK,
                    (void *)Ex_HTTP_Client_TransErrCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  //                                                               ----------------- OPEN CONNECTION ------------------
  str_len = Str_Len(EX_HTTP_CLIENT_CLIENT_HOSTNAME);
  (void)HTTPc_ConnOpen(p_conn,
                       p_buf,
                       EX_HTTP_CLIENT_CONN_BUF_SIZE,
                       EX_HTTP_CLIENT_CLIENT_HOSTNAME,
                       str_len,
                       HTTPc_FLAG_NONE,
                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE(("Connection to server succeeded.\n\r"));

  //                                                               ---------------- SEND HTTP REQUEST -----------------
  str_len = Str_Len("/post");
  (void)HTTPc_ReqSend(p_conn,
                      p_req,
                      p_resp,
                      HTTP_METHOD_POST,
                      "/post",
                      str_len,
                      HTTPc_FLAG_NONE,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("%s\n\r", p_resp->ReasonPhrasePtr);
}
#endif

/****************************************************************************************************//**
 *                                       Ex_HTTP_Client_ReqSendPut()
 *
 * @brief  Send PUT request.
 *******************************************************************************************************/
void Ex_HTTP_Client_ReqSendPut(void)
{
  HTTPc_CONN_OBJ    *p_conn;
  HTTPc_REQ_OBJ     *p_req;
  HTTPc_RESP_OBJ    *p_resp;
  CPU_CHAR          *p_buf;
  CPU_SIZE_T        str_len;
  CPU_SIZE_T        content_len;
  HTTP_CONTENT_TYPE content_type;
  RTOS_ERR          err;

  p_conn = &Ex_HTTP_Client_ConnTbl[0u];
  p_req = &Ex_HTTP_Client_ReqTbl[0u];
  p_resp = &Ex_HTTP_Client_RespTbl[0u];
  p_buf = &Ex_HTTP_Client_ConnBufTbl[0u][0u];

#if (HTTPc_CFG_USER_DATA_EN == DEF_ENABLED)
  p_req->UserDataPtr = (void *)&Ex_HTTP_Client_Data[0u];
#else
  APP_RTOS_ASSERT_CRITICAL_FAIL(; );
#endif

  //                                                               ----------- INIT NEW CONNECTION & REQUEST ----------
  HTTPc_ConnClr(p_conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqClr(p_req, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------- SET CONNECTION CALLBACKS -------------
#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
  HTTPc_ConnSetParam(p_conn,
                     HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK,
                     (void *)Ex_HTTP_Client_ConnCloseCallback,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  //                                                               ----------- SET REQUEST BODY PARAMETERS ------------
  content_type = HTTP_CONTENT_TYPE_GIF;
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_TYPE,
                    &content_type,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  content_len = STATIC_LOGO_GIF_LEN;
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_LEN,
                    &content_len,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_REQ_BODY_HOOK,
                    (void *)Ex_HTTP_Client_ReqBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------ SET REQ/RESP HOOK FUNCTIONS -----------
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
                    (void *)Ex_HTTP_Client_RespHdrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_RESP_BODY_HOOK,
                    (void *)Ex_HTTP_Client_RespBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               --------- SET REQ/RESP CALLBACK FUNCTIONS ----------
#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
  HTTPc_ReqSetParam(p_req,
                    HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK,
                    (void *)Ex_HTTP_Client_TransErrCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  //                                                               ----------------- OPEN CONNECTION ------------------
  str_len = Str_Len(EX_HTTP_CLIENT_CLIENT_HOSTNAME);
  (void)HTTPc_ConnOpen(p_conn,
                       p_buf,
                       EX_HTTP_CLIENT_CONN_BUF_SIZE,
                       EX_HTTP_CLIENT_CLIENT_HOSTNAME,
                       str_len,
                       HTTPc_FLAG_NONE,
                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE(("Connection to server succeeded.\n\r"));

  //                                                               ---------------- SEND HTTP REQUEST -----------------
  str_len = Str_Len("/post");
  (void)HTTPc_ReqSend(p_conn,
                      p_req,
                      p_resp,
                      HTTP_METHOD_PUT,
                      "/put",
                      str_len,
                      HTTPc_FLAG_NONE,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("%s\n\r", p_resp->ReasonPhrasePtr);
}

/****************************************************************************************************//**
 *                                     Ex_HTTP_Client_PersistentConn()
 *
 * @brief  Send multiple requests on same connection.
 *******************************************************************************************************/

#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED) \
  && (HTTPc_CFG_USER_DATA_EN == DEF_ENABLED)
void Ex_HTTP_Client_PersistentConn(void)
{
  EX_HTTP_CLIENT_REQ_DATA *p_data1;
  EX_HTTP_CLIENT_REQ_DATA *p_data2;
  HTTPc_CONN_OBJ          *p_conn;
  HTTPc_REQ_OBJ           *p_req1;
  HTTPc_RESP_OBJ          *p_resp1;
  HTTPc_REQ_OBJ           *p_req2;
  HTTPc_RESP_OBJ          *p_resp2;
  CPU_CHAR                *p_buf;
  CPU_SIZE_T              str_len;
  CPU_BOOLEAN             persistent;
  CPU_BOOLEAN             req1_done;
  CPU_BOOLEAN             req2_done;
  CPU_BOOLEAN             close;
  RTOS_ERR                err;
  CORE_DECLARE_IRQ_STATE;

  p_conn = &Ex_HTTP_Client_ConnTbl[0u];
  p_req1 = &Ex_HTTP_Client_ReqTbl[0u];
  p_resp1 = &Ex_HTTP_Client_RespTbl[0u];
  p_req2 = &Ex_HTTP_Client_ReqTbl[1u];
  p_resp2 = &Ex_HTTP_Client_RespTbl[1u];
  p_buf = &Ex_HTTP_Client_ConnBufTbl[0u][0u];

  p_req1->UserDataPtr = (void *)&Ex_HTTP_Client_Data[0u];
  p_req2->UserDataPtr = (void *)&Ex_HTTP_Client_Data[1u];

  p_data1 = (EX_HTTP_CLIENT_REQ_DATA *)p_req1->UserDataPtr;
  p_data2 = (EX_HTTP_CLIENT_REQ_DATA *)p_req2->UserDataPtr;

  close = DEF_NO;

  //                                                               ----------- INIT NEW CONNECTION & REQUEST ----------
  HTTPc_ConnClr(p_conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqClr(p_req1, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqClr(p_req2, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------- SET CONNECTION PARAMETERS ------------
  persistent = DEF_YES;
  HTTPc_ConnSetParam(p_conn,
                     HTTPc_PARAM_TYPE_PERSISTENT,
                     &persistent,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------- SET CONNECTION CALLBACKS -------------
  HTTPc_ConnSetParam(p_conn,
                     HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK,
                     (void *)Ex_HTTP_Client_ConnCloseCallback,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------ SET REQ/RESP HOOK FUNCTIONS -----------
  HTTPc_ReqSetParam(p_req1,
                    HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
                    (void *)Ex_HTTP_Client_RespHdrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req1,
                    HTTPc_PARAM_TYPE_RESP_BODY_HOOK,
                    (void *)Ex_HTTP_Client_RespBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req2,
                    HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
                    (void *)Ex_HTTP_Client_RespHdrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req2,
                    HTTPc_PARAM_TYPE_RESP_BODY_HOOK,
                    (void *)Ex_HTTP_Client_RespBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               --------- SET REQ/RESP CALLBACK FUNCTIONS ----------
  HTTPc_ReqSetParam(p_req1,
                    HTTPc_PARAM_TYPE_TRANS_COMPLETE_CALLBACK,
                    (void *)Ex_HTTP_Client_TransDoneCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req2,
                    HTTPc_PARAM_TYPE_TRANS_COMPLETE_CALLBACK,
                    (void *)Ex_HTTP_Client_TransDoneCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req1,
                    HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK,
                    (void *)Ex_HTTP_Client_TransErrCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req2,
                    HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK,
                    (void *)Ex_HTTP_Client_TransErrCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ----------------- OPEN CONNECTION ------------------
  str_len = Str_Len(EX_HTTP_CLIENT_CLIENT_HOSTNAME);
  (void)HTTPc_ConnOpen(p_conn,
                       p_buf,
                       EX_HTTP_CLIENT_CONN_BUF_SIZE,
                       EX_HTTP_CLIENT_CLIENT_HOSTNAME,
                       str_len,
                       HTTPc_FLAG_NONE,
                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE(("Connection to server succeeded.\n\r"));

  //                                                               ---------------- SEND HTTP REQUEST -----------------
  str_len = Str_Len("/get");
  (void)HTTPc_ReqSend(p_conn,
                      p_req1,
                      p_resp1,
                      HTTP_METHOD_GET,
                      "/get",
                      str_len,
                      HTTPc_FLAG_REQ_NO_BLOCK,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  (void)HTTPc_ReqSend(p_conn,
                      p_req2,
                      p_resp2,
                      HTTP_METHOD_GET,
                      "/get",
                      str_len,
                      HTTPc_FLAG_REQ_NO_BLOCK,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  do {
    CORE_ENTER_ATOMIC();
    req1_done = p_data1->Done;
    req2_done = p_data2->Done;
    CORE_EXIT_ATOMIC();

    if ((req1_done == DEF_YES)
        && (req2_done == DEF_YES)) {
      close = DEF_YES;
    }

    NetApp_TimeDly_ms(10u, &err);
  } while (close == DEF_NO);

  HTTPc_ConnClose(p_conn,
                  HTTPc_FLAG_NONE,
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}
#endif

/****************************************************************************************************//**
 *                                       Ex_HTTP_Client_MultiConn()
 *
 * @brief  Open multiple Connections to send HTTP requests in parallel.
 *******************************************************************************************************/

#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
void Ex_HTTP_Client_MultiConn(void)
{
  HTTPc_CONN_OBJ *p_conn1;
  HTTPc_CONN_OBJ *p_conn2;
  HTTPc_REQ_OBJ  *p_req1;
  HTTPc_RESP_OBJ *p_resp1;
  HTTPc_REQ_OBJ  *p_req2;
  HTTPc_RESP_OBJ *p_resp2;
  CPU_CHAR       *p_buf1;
  CPU_CHAR       *p_buf2;
  CPU_SIZE_T     str_len;
  RTOS_ERR       err;

  p_conn1 = &Ex_HTTP_Client_ConnTbl[0u];
  p_conn2 = &Ex_HTTP_Client_ConnTbl[1u];
  p_req1 = &Ex_HTTP_Client_ReqTbl[0u];
  p_resp1 = &Ex_HTTP_Client_RespTbl[0u];
  p_req2 = &Ex_HTTP_Client_ReqTbl[1u];
  p_resp2 = &Ex_HTTP_Client_RespTbl[1u];
  p_buf1 = &Ex_HTTP_Client_ConnBufTbl[0u][0u];
  p_buf2 = &Ex_HTTP_Client_ConnBufTbl[1u][0u];

#if (HTTPc_CFG_USER_DATA_EN == DEF_ENABLED)
  p_req1->UserDataPtr = (void *)&Ex_HTTP_Client_Data[0u];
  p_req2->UserDataPtr = (void *)&Ex_HTTP_Client_Data[1u];
#else
  APP_RTOS_ASSERT_CRITICAL_FAIL(; );
#endif

  //                                                               ----------- INIT NEW CONNECTION & REQUEST ----------
  HTTPc_ConnClr(p_conn1, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ConnClr(p_conn2, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqClr(p_req1, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqClr(p_req2, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------- SET CONNECTION CALLBACKS -------------
  HTTPc_ConnSetParam(p_conn1,
                     HTTPc_PARAM_TYPE_CONN_CONNECT_CALLBACK,
                     (void *)Ex_HTTP_Client_ConnConnectCallback,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ConnSetParam(p_conn2,
                     HTTPc_PARAM_TYPE_CONN_CONNECT_CALLBACK,
                     (void *)Ex_HTTP_Client_ConnConnectCallback,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ConnSetParam(p_conn1,
                     HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK,
                     (void *)Ex_HTTP_Client_ConnCloseCallback,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ConnSetParam(p_conn2,
                     HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK,
                     (void *)Ex_HTTP_Client_ConnCloseCallback,
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ------------ SET REQ/RESP HOOK FUNCTIONS -----------
  HTTPc_ReqSetParam(p_req1,
                    HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
                    (void *)Ex_HTTP_Client_RespHdrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req1,
                    HTTPc_PARAM_TYPE_RESP_BODY_HOOK,
                    (void *)Ex_HTTP_Client_RespBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req2,
                    HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
                    (void *)Ex_HTTP_Client_RespHdrHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req2,
                    HTTPc_PARAM_TYPE_RESP_BODY_HOOK,
                    (void *)Ex_HTTP_Client_RespBodyHook,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               --------- SET REQ/RESP CALLBACK FUNCTIONS ----------
  HTTPc_ReqSetParam(p_req1,
                    HTTPc_PARAM_TYPE_TRANS_COMPLETE_CALLBACK,
                    (void *)Ex_HTTP_Client_TransDoneCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req2,
                    HTTPc_PARAM_TYPE_TRANS_COMPLETE_CALLBACK,
                    (void *)Ex_HTTP_Client_TransDoneCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req1,
                    HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK,
                    (void *)Ex_HTTP_Client_TransErrCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  HTTPc_ReqSetParam(p_req2,
                    HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK,
                    (void *)Ex_HTTP_Client_TransErrCallback,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ----------------- OPEN CONNECTION ------------------
  str_len = Str_Len(EX_HTTP_CLIENT_CLIENT_HOSTNAME);
  (void)HTTPc_ConnOpen(p_conn1,
                       p_buf1,
                       EX_HTTP_CLIENT_CONN_BUF_SIZE,
                       EX_HTTP_CLIENT_CLIENT_HOSTNAME,
                       str_len,
                       HTTPc_FLAG_CONN_NO_BLOCK,
                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  (void)HTTPc_ConnOpen(p_conn2,
                       p_buf2,
                       EX_HTTP_CLIENT_CONN_BUF_SIZE,
                       EX_HTTP_CLIENT_CLIENT_HOSTNAME,
                       str_len,
                       HTTPc_FLAG_CONN_NO_BLOCK,
                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               ---------------- SEND HTTP REQUEST -----------------
  str_len = Str_Len("/get");
  (void)HTTPc_ReqSend(p_conn1,
                      p_req1,
                      p_resp1,
                      HTTP_METHOD_GET,
                      "/get",
                      str_len,
                      HTTPc_FLAG_REQ_NO_BLOCK,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  (void)HTTPc_ReqSend(p_conn2,
                      p_req2,
                      p_resp2,
                      HTTP_METHOD_GET,
                      "/get",
                      str_len,
                      HTTPc_FLAG_REQ_NO_BLOCK,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                    Ex_HTTP_Client_FS_MediaPrepare()
 *
 * @brief  Example of File System media preparation to contain HTTP Client example files.
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
static void Ex_HTTP_Client_FS_MediaPrepare(void)
{
  FS_RAM_DISK_CFG   ramdisk_cfg;
  FS_BLK_DEV_HANDLE dev_handle;
  FS_MEDIA_HANDLE   media_handle;
  FS_WRK_DIR_HANDLE wrk_dir_handle;
  FS_CACHE_CFG      cache_cfg;
  RTOS_ERR          err;

  ramdisk_cfg.DiskPtr = Ex_HTTP_Client_RAMDisk;                 // Create a RAMDisk that will contain the files.
  ramdisk_cfg.LbCnt = EX_HTTP_CLIENT_RAMDISK_SEC_NBR;
  ramdisk_cfg.LbSize = EX_HTTP_CLIENT_RAMDISK_SEC_SIZE;

  FS_RAM_Disk_Add(EX_HTTP_CLIENT_FILE_RAM_MEDIA_NAME,
                  &ramdisk_cfg,
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  cache_cfg.Align = sizeof(CPU_ALIGN);                          // Create cache for RAMDisk block device.
  cache_cfg.BlkMemSegPtr = DEF_NULL;
  cache_cfg.MaxLbSize = 512u;
  cache_cfg.MinLbSize = 512u;
  cache_cfg.MinBlkCnt = 1u;

  Ex_HTTP_Client_CachePtr = FSCache_Create(&cache_cfg,
                                           &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Open block device.
  media_handle = FSMedia_Get(EX_HTTP_CLIENT_FILE_RAM_MEDIA_NAME);
  APP_RTOS_ASSERT_CRITICAL(!FS_MEDIA_HANDLE_IS_NULL(media_handle),; );

  dev_handle = FSBlkDev_Open(media_handle, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FSCache_Assign(dev_handle,                                    // Assign cache to block device.
                 Ex_HTTP_Client_CachePtr,
                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  FS_FAT_Fmt(dev_handle,                                        // Format RAMDisk in FAT format.
             FS_PARTITION_NBR_VOID,
             DEF_NULL,
             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  (void)FSVol_Open(dev_handle,                                  // Open volume and call it "ram_httpc".
                   1u,
                   EX_HTTP_CLIENT_FILE_VOL_NAME,
                   FS_VOL_OPT_DFLT,
                   &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  wrk_dir_handle = FSWrkDir_Open(FSWrkDir_NullHandle,           // Create and open a working directory at the root of vol.
                                 EX_HTTP_CLIENT_WRK_DIR,
                                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  Ex_HTTP_Client_FS_FileAdd(wrk_dir_handle,                     // Create and copy content of all the files to RAMDisk.
                            EX_HTTP_CLIENT_FILE_NAME,
                            (CPU_CHAR *)Ex_HTTP_Client_IndexHtml,
                            STATIC_INDEX_HTML_LEN);

  FSWrkDir_Close(wrk_dir_handle, &err);                         // Close working directory.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}
#endif

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

#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
static void Ex_HTTP_Client_FS_FileAdd(FS_WRK_DIR_HANDLE wrk_dir_handle,
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

  while (rem_size > 0 && err.Code == RTOS_ERR_NONE) {
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
#endif

/****************************************************************************************************//**
 *                                   Ex_HTTP_Client_ReqPrepareQueryStr()
 *
 * @brief  Configure the Query String table.
 *
 * @param  p_tbl  Variable that will received the pointer to the Query String Table.
 *
 * @return  Number of fields in the table.
 *******************************************************************************************************/
static CPU_INT08U Ex_HTTP_Client_ReqPrepareQueryStr(HTTPc_KEY_VAL **p_tbl)
{
  HTTPc_KEY_VAL *p_kvp;

  //                                                               ----------------- SET FIRST QUERY ------------------
  p_kvp = &Ex_HTTP_Client_ReqQueryStrTbl[0u];
  p_kvp->KeyPtr = &Ex_HTTP_Client_ReqQueryStrKeyTbl[0u][0u];
  p_kvp->ValPtr = &Ex_HTTP_Client_ReqQueryStrValTbl[0u][0u];

  (void)Str_Copy_N(p_kvp->KeyPtr, "Name", EX_HTTP_CLIENT_CFG_QUERY_STR_KEY_LEN_MAX);
  (void)Str_Copy_N(p_kvp->ValPtr, "John Smith", EX_HTTP_CLIENT_CFG_QUERY_STR_VAL_LEN_MAX);

  p_kvp->KeyLen = Str_Len("Name");
  p_kvp->ValLen = Str_Len("John Smith");

  //                                                               ---------------- SET SECOND QUERY ------------------
  p_kvp++;

  p_kvp->KeyPtr = &Ex_HTTP_Client_ReqQueryStrKeyTbl[1][0];
  p_kvp->ValPtr = DEF_NULL;

  (void)Str_Copy_N(p_kvp->KeyPtr, "Active", EX_HTTP_CLIENT_CFG_QUERY_STR_KEY_LEN_MAX);

  p_kvp->KeyLen = Str_Len("Active");
  p_kvp->ValLen = 0;

  *p_tbl = &Ex_HTTP_Client_ReqQueryStrTbl[0];

  return (2);
}

/****************************************************************************************************//**
 *                                     Ex_HTTP_Client_ReqPrepareHdrs()
 *
 * @brief  Configure the Header Fields Table
 *
 * @param  p_hdr_tbl  Variable that will received the pointer to the Header Fields table.
 *
 * @return  Number of fields in the table.
 *******************************************************************************************************/
static CPU_INT08U Ex_HTTP_Client_ReqPrepareHdrs(HTTPc_HDR **p_hdr_tbl)
{
  HTTPc_HDR *p_hdr;

  //                                                               ---------------- ADD ACCEPT HEADER -----------------
  p_hdr = &Ex_HTTP_Client_ReqHdrTbl[0u];
  p_hdr->ValPtr = &Ex_HTTP_Client_ReqHdrValStrTbl[0u][0u];
  p_hdr->HdrField = HTTP_HDR_FIELD_ACCEPT;

  Str_Copy_N(p_hdr->ValPtr, "text/*", EX_HTTP_CLIENT_CFG_HDR_VAL_LEN_MAX);

  p_hdr->ValLen = Str_Len("text/*");

  *p_hdr_tbl = &Ex_HTTP_Client_ReqHdrTbl[0];

  return (1);
}

/****************************************************************************************************//**
 *                                   Ex_HTTP_Client_ReqPrepareFormApp()
 *
 * @brief  Configure the application type form table.
 *
 * @param  p_form_tbl  Variable that will received the pointer to the form table.
 *
 * @return  Number of fields in the form table.
 *******************************************************************************************************/
#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
static CPU_INT08U Ex_HTTP_Client_ReqPrepareFormApp(HTTPc_FORM_TBL_FIELD **p_form_tbl)
{
  HTTPc_FORM_TBL_FIELD *p_tbl;
  HTTPc_KEY_VAL        *p_kvp;
  RTOS_ERR             err;

  p_tbl = &Ex_HTTP_Client_FormTbl[0];

  //                                                               -------------- ADD FIRST FORM FIELD ----------------
  p_kvp = &Ex_HTTP_Client_FormKeyValTbl[0u];
  p_kvp->KeyPtr = &Ex_HTTP_Client_FormKeyStrTbl[0u][0u];
  p_kvp->ValPtr = &Ex_HTTP_Client_FormValStrTbl[0u][0u];

  p_kvp->KeyLen = Str_Len("Age");
  p_kvp->ValLen = Str_Len("32");

  (void)Str_Copy_N(p_kvp->KeyPtr, "Age", p_kvp->KeyLen);
  (void)Str_Copy_N(p_kvp->ValPtr, "32", p_kvp->ValLen);

  HTTPc_FormAddKeyVal(p_tbl, p_kvp, &err);

  //                                                               -------------- ADD SECOND FORM FIELD ---------------
  p_tbl++;
  p_kvp++;

  p_kvp->KeyPtr = &Ex_HTTP_Client_FormKeyStrTbl[1u][0u];
  p_kvp->ValPtr = &Ex_HTTP_Client_FormValStrTbl[1u][0u];

  p_kvp->KeyLen = Str_Len("book");
  p_kvp->ValLen = Str_Len("Implementing IPv6 Second Edition");

  (void)Str_Copy_N(p_kvp->KeyPtr, "book", p_kvp->KeyLen);
  (void)Str_Copy_N(p_kvp->ValPtr, "Implementing IPv6 Second Edition", p_kvp->ValLen);

  HTTPc_FormAddKeyVal(p_tbl, p_kvp, &err);

  *p_form_tbl = &Ex_HTTP_Client_FormTbl[0];

  return (2);
}
#endif

/****************************************************************************************************//**
 *                                Ex_HTTP_Client_ReqPrepareFormMultipart()
 *
 * @brief  Configure the multipart type form table.
 *
 * @param  p_wrk_dir   String that contains the directory where the index.html file to send as part
 *                     of the form is stored. Can be DEF_NULL if Micrium OS File System is not
 *                     present or if no file is desired as part of the form.
 *
 * @param  p_form_tbl  Variable that will received the pointer to the form table.
 *
 * @return  Number of fields in the form table.
 *******************************************************************************************************/

#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
static CPU_INT08U Ex_HTTP_Client_ReqPrepareFormMultipart(CPU_CHAR             *p_wrk_dir,
                                                         HTTPc_FORM_TBL_FIELD **p_form_tbl)
{
  HTTPc_FORM_TBL_FIELD *p_tbl;
  HTTPc_KEY_VAL        *p_kvp;
  HTTPc_KEY_VAL_EXT    *p_kvp_big;
  RTOS_ERR             err;
#ifdef RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
  HTTPc_MULTIPART_FILE *p_form_file;
  FS_FILE_HANDLE       file_handle;
  FS_ENTRY_INFO        file_info;
#endif

  PP_UNUSED_PARAM(p_wrk_dir);

  p_tbl = &Ex_HTTP_Client_FormTbl[0u];

  //                                                               -------------- ADD FIRST FORM FIELD ----------------
  p_kvp = &Ex_HTTP_Client_FormKeyValTbl[0u];
  p_kvp->KeyPtr = &Ex_HTTP_Client_FormKeyStrTbl[0u][0u];
  p_kvp->ValPtr = &Ex_HTTP_Client_FormValStrTbl[0u][0u];

  p_kvp->KeyLen = Str_Len("LED1");
  p_kvp->ValLen = Str_Len("ON");

  (void)Str_Copy_N(p_kvp->KeyPtr, "LED1", p_kvp->KeyLen);
  (void)Str_Copy_N(p_kvp->ValPtr, "ON", p_kvp->ValLen);

  HTTPc_FormAddKeyVal(p_tbl,
                      p_kvp,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               -------------- ADD SECOND FORM FIELD ---------------
  p_tbl++;

  p_kvp_big = &Ex_HTTP_Client_FormKeyValExtTbl[0u];
  p_kvp_big->KeyPtr = &Ex_HTTP_Client_FormKeyStrTbl[1u][0u];

  p_kvp_big->KeyLen = Str_Len("Text");
  p_kvp_big->ValLen = Str_Len("The Hypertext Transfer Protocol (HTTP) is a stateless application-level protocol for distributed, collaborative, hypertext information systems.");

  (void)Str_Copy_N(p_kvp_big->KeyPtr, "Text", p_kvp_big->KeyLen);
  (void)Str_Copy_N(&Ex_HTTP_Client_FormValStrTbl[1][0], "The Hypertext Transfer Protocol (HTTP) is a stateless application-level protocol for distributed, collaborative, hypertext information systems.", p_kvp_big->ValLen);

  p_kvp_big->OnValTx = &Ex_HTTP_Client_FormMultipartHook;

  HTTPc_FormAddKeyValExt(p_tbl, p_kvp_big, &err);

#ifdef RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
  if (p_wrk_dir != DEF_NULL) {
    FS_WRK_DIR_HANDLE wrk_dir_handle;

    //                                                             -------------- ADD THIRD FORM FIELD ----------------
    p_tbl++;

    p_form_file = &Ex_HTTP_Client_FormMultipartFileTbl[0u];
    p_form_file->NamePtr = &Ex_HTTP_Client_FormMultipartNameStrTbl[0u][0u];
    p_form_file->FileNamePtr = &Ex_HTTP_Client_FormMultipartFileNameStrTbl[0u][0u];
    p_form_file->ContentType = HTTP_CONTENT_TYPE_HTML;

    Str_Copy(p_form_file->NamePtr, "File");
    Str_Copy(p_form_file->FileNamePtr, "index.html");

    p_form_file->NameLen = Str_Len("File");
    p_form_file->FileNameLen = Str_Len("index.html");

    //                                                             ------------ SET FS WORKING DIRECTORY --------------
    wrk_dir_handle = FSWrkDir_Open(FSWrkDir_NullHandle,
                                   p_wrk_dir,
                                   &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    file_handle = FSFile_Open(wrk_dir_handle,
                              "/index.html",
                              FS_FILE_ACCESS_MODE_RD,
                              &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    FSFile_Query(file_handle,
                 &file_info,
                 &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    p_form_file->FileLen = file_info.Size;
    p_form_file->OnFileTx = &Ex_HTTP_Client_FormMultipartFileHook;

    HTTPc_FormAddFile(p_tbl, p_form_file, &err);

    *p_form_tbl = p_tbl;

    FSFile_Close(file_handle, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    FSWrkDir_Close(wrk_dir_handle, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

    *p_form_tbl = &Ex_HTTP_Client_FormTbl[0u];

    return (3u);
  } else {
    *p_form_tbl = &Ex_HTTP_Client_FormTbl[0u];

    return (2u);
  }
#else

  *p_form_tbl = &Ex_HTTP_Client_FormTbl[0u];

  return (2u);
#endif
}
#endif

/****************************************************************************************************//**
 *                                  Ex_HTTP_Client_Secure_TrustCallback()
 *
 * @brief  Hook function called by the TLS layer to let client decide to enforce or ignore server
 *         certificate validation errors.
 *
 * @param  p_cert_dn  Port-specific representation of the certificate distinguished name.
 *
 * @param  reason     Reason why the connection is not secure.
 *
 * @return  DEF_OK,   if non-secure connection should be allowed.
 *          DEF_FAIL, otherwise.
 *******************************************************************************************************/

#ifdef RTOS_MODULE_NET_SSL_TLS_AVAIL
CPU_BOOLEAN Ex_HTTP_Client_Secure_TrustCallback(void                             *p_cert_dn,
                                                NET_SOCK_SECURE_UNTRUSTED_REASON reason)
{
  PP_UNUSED_PARAM(p_cert_dn);
  PP_UNUSED_PARAM(reason);
  return (DEF_FAIL);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_CLIENT_AVAIL
