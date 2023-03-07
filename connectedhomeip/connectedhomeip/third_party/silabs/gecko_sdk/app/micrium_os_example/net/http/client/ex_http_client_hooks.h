/***************************************************************************//**
 * @file
 * @brief Example - HTTP Client Hook Functions File
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

#ifndef  EX_HTTP_CLIENT_HOOKS_H
#define  EX_HTTP_CLIENT_HOOKS_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/http_client.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

CPU_BOOLEAN Ex_HTTP_Client_ReqQueryStrHook(HTTPc_CONN_OBJ *p_conn,
                                           HTTPc_REQ_OBJ  *p_req,
                                           HTTPc_KEY_VAL  **p_key_val);

CPU_BOOLEAN Ex_HTTP_Client_ReqHdrHook(HTTPc_CONN_OBJ *p_conn,
                                      HTTPc_REQ_OBJ  *p_req,
                                      HTTPc_HDR      **p_hdr);

CPU_BOOLEAN Ex_HTTP_Client_ReqBodyHook(HTTPc_CONN_OBJ *p_conn,
                                       HTTPc_REQ_OBJ  *p_req,
                                       void           **p_data,
                                       CPU_CHAR       *p_buf,
                                       CPU_INT16U     buf_len,
                                       CPU_INT16U     *p_data_len);

void Ex_HTTP_Client_RespHdrHook(HTTPc_CONN_OBJ *p_conn,
                                HTTPc_REQ_OBJ  *p_req,
                                HTTP_HDR_FIELD hdr_field,
                                CPU_CHAR       *p_hdr_val,
                                CPU_INT16U     val_len);

CPU_INT32U Ex_HTTP_Client_RespBodyHook(HTTPc_CONN_OBJ    *p_conn,
                                       HTTPc_REQ_OBJ     *p_req,
                                       HTTP_CONTENT_TYPE content_type,
                                       void              *p_data,
                                       CPU_INT16U        data_len,
                                       CPU_BOOLEAN       last_chunk);

CPU_BOOLEAN Ex_HTTP_Client_FormMultipartHook(HTTPc_CONN_OBJ    *p_conn,
                                             HTTPc_REQ_OBJ     *p_req,
                                             HTTPc_KEY_VAL_EXT *p_key_val_obj,
                                             CPU_CHAR          *p_buf,
                                             CPU_INT16U        buf_len,
                                             CPU_INT16U        *p_len_wr);

#ifdef  RTOS_MODULE_FS_AVAIL
CPU_BOOLEAN Ex_HTTP_Client_FormMultipartFileHook(HTTPc_CONN_OBJ       *p_conn,
                                                 HTTPc_REQ_OBJ        *p_req,
                                                 HTTPc_MULTIPART_FILE *p_file_obj,
                                                 CPU_CHAR             *p_buf,
                                                 CPU_INT16U           buf_len,
                                                 CPU_INT16U           *p_len_wr);
#endif

#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)
void Ex_HTTP_Client_ConnConnectCallback(HTTPc_CONN_OBJ *p_conn,
                                        CPU_BOOLEAN    open_status);

void Ex_HTTP_Client_ConnCloseCallback(HTTPc_CONN_OBJ          *p_conn,
                                      HTTPc_CONN_CLOSE_STATUS close_status,
                                      RTOS_ERR                err);

void Ex_HTTP_Client_TransDoneCallback(HTTPc_CONN_OBJ *p_conn,
                                      HTTPc_REQ_OBJ  *p_req,
                                      HTTPc_RESP_OBJ *p_resp,
                                      CPU_BOOLEAN    status);

void Ex_HTTP_Client_TransErrCallback(HTTPc_CONN_OBJ *p_conn,
                                     HTTPc_REQ_OBJ  *p_req,
                                     RTOS_ERR       err_code);
#endif

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                              MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // EX_HTTP_CLIENT_HOOKS_H
