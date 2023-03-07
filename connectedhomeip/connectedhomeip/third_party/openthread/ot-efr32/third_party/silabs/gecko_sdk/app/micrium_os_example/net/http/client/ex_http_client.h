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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  EX_HTTP_CLIENT_H
#define  EX_HTTP_CLIENT_H

/********************************************************************************************************
 *                                             FS MODULE
 *
 * Note(s): If the Micrium OS File System is present in the project, you can enable it for the example
 *          application.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <net/include/http_client.h>

#ifdef  RTOS_MODULE_FS_AVAIL
#include  <fs/include/fs_core_file.h>
#endif

#include  <rtos_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  EX_HTTP_CLIENT_CFG_QUERY_STR_NBR_MAX
#define  EX_HTTP_CLIENT_CFG_QUERY_STR_NBR_MAX                  6u
#endif

#ifndef  EX_HTTP_CLIENT_CFG_QUERY_STR_KEY_LEN_MAX
#define  EX_HTTP_CLIENT_CFG_QUERY_STR_KEY_LEN_MAX             20u
#endif

#ifndef  EX_HTTP_CLIENT_CFG_QUERY_STR_VAL_LEN_MAX
#define  EX_HTTP_CLIENT_CFG_QUERY_STR_VAL_LEN_MAX             50u
#endif

#ifndef  EX_HTTP_CLIENT_CFG_HDR_NBR_MAX
#define  EX_HTTP_CLIENT_CFG_HDR_NBR_MAX                        6u
#endif

#ifndef  EX_HTTP_CLIENT_CFG_HDR_VAL_LEN_MAX
#define  EX_HTTP_CLIENT_CFG_HDR_VAL_LEN_MAX                  100u
#endif

#ifndef  EX_HTTP_CLIENT_CFG_FORM_BUF_SIZE
#define  EX_HTTP_CLIENT_CFG_FORM_BUF_SIZE                    256u
#endif

#ifndef  EX_HTTP_CLIENT_CFG_FORM_FIELD_NBR_MAX
#define  EX_HTTP_CLIENT_CFG_FORM_FIELD_NBR_MAX                10u
#endif

#ifndef  EX_HTTP_CLIENT_CFG_FORM_FIELD_KEY_LEN_MAX
#define  EX_HTTP_CLIENT_CFG_FORM_FIELD_KEY_LEN_MAX           100u
#endif

#ifndef  EX_HTTP_CLIENT_CFG_FORM_FIELD_VAL_LEN_MAX
#define  EX_HTTP_CLIENT_CFG_FORM_FIELD_VAL_LEN_MAX           200u
#endif

#ifndef  EX_HTTP_CLIENT_CFG_FORM_MULTIPART_NAME_LEN_MAX
#define  EX_HTTP_CLIENT_CFG_FORM_MULTIPART_NAME_LEN_MAX      100u
#endif

#ifndef  EX_HTTP_CLIENT_CFG_FORM_MULTIPART_FILENAME_LEN_MAX
#define  EX_HTTP_CLIENT_CFG_FORM_MULTIPART_FILENAME_LEN_MAX  100u
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                          CONNECTION DATA TYPE
 *******************************************************************************************************/

typedef  struct  ex_httpc_conn_data {
  CPU_BOOLEAN Close;
} EX_HTTPc_CONN_DATA;

/********************************************************************************************************
 *                                           REQUEST DATA TYPE
 *******************************************************************************************************/

typedef  struct  ex_http_client_req_data {
  CPU_BOOLEAN    Done;
  CPU_INT08U     QueryStrIx;
  CPU_INT16U     FormIx;
#ifdef  RTOS_MODULE_FS_AVAIL
  CPU_CHAR       *WrkDirPtr;
  FS_FILE_HANDLE FileHandle;
#endif
} EX_HTTP_CLIENT_REQ_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void Ex_HTTP_Client_Init(void);

void Ex_HTTP_Client_ReqSendGet_NoTask(void);

void Ex_HTTP_Client_ReqSendGet(void);

#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
void Ex_HTTP_Client_ReqSendPost(void);

void Ex_HTTP_Client_ReqSendAppForm(void);

void Ex_HTTP_Client_ReqSendMultipartForm(CPU_CHAR *p_wrk_dir);
#endif

void Ex_HTTP_Client_ReqSendPut(void);

#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
void Ex_HTTP_Client_PersistentConn(void);

void Ex_HTTP_Client_MultiConn(void);
#endif

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                              MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // EX_HTTP_CLIENT_H
