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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  EX_HTTP_SERVER_HOOKS_H
#define  EX_HTTP_SERVER_HOOKS_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/http_server.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

CPU_BOOLEAN Ex_HTTP_Server_InstanceInitHook(const HTTPs_INSTANCE *p_instance,
                                            const void           *p_hook_cfg);

CPU_BOOLEAN Ex_HTTP_Server_ReqHdrRxHook(const HTTPs_INSTANCE *p_instance,
                                        const HTTPs_CONN     *p_conn,
                                        const void           *p_hook_cfg,
                                        HTTP_HDR_FIELD       hdr_field);

CPU_BOOLEAN Ex_HTTP_Server_ReqHookNoREST(const HTTPs_INSTANCE *p_instance,
                                         HTTPs_CONN           *p_conn,
                                         const void           *p_hook_cfg);

CPU_BOOLEAN Ex_HTTP_Server_ReqHook(const HTTPs_INSTANCE *p_instance,
                                   HTTPs_CONN           *p_conn,
                                   const void           *p_hook_cfg);

CPU_BOOLEAN Ex_HTTP_Server_ReqBodyRxHook(const HTTPs_INSTANCE *p_instance,
                                         HTTPs_CONN           *p_conn,
                                         const void           *p_hook_cfg,
                                         void                 *p_buf,
                                         const CPU_SIZE_T     buf_size,
                                         CPU_SIZE_T           *p_buf_size_used);

CPU_BOOLEAN Ex_HTTP_Server_ReqRdySignalHook(const HTTPs_INSTANCE *p_instance,
                                            HTTPs_CONN           *p_conn,
                                            const void           *p_hook_cfg,
                                            const HTTPs_KEY_VAL  *p_data);

CPU_BOOLEAN Ex_HTTP_Server_ReqRdyPollHook(const HTTPs_INSTANCE *p_instance,
                                          HTTPs_CONN           *p_conn,
                                          const void           *p_hook_cfg);

CPU_BOOLEAN Ex_HTTP_Server_RespHdrTxHook(const HTTPs_INSTANCE *p_instance,
                                         HTTPs_CONN           *p_conn,
                                         const void           *p_hook_cfg);

CPU_BOOLEAN Ex_HTTP_Server_RespTokenValGetHook(const HTTPs_INSTANCE *p_instance,
                                               HTTPs_CONN           *p_conn,
                                               const void           *p_hook_cfg,
                                               const CPU_CHAR       *p_token,
                                               CPU_INT16U           token_len,
                                               CPU_CHAR             *p_val,
                                               CPU_INT16U           val_len_max);

CPU_BOOLEAN Ex_HTTP_Server_RespChunkDataGetHook(const HTTPs_INSTANCE *p_instance,
                                                HTTPs_CONN           *p_conn,
                                                const void           *p_hook_cfg,
                                                void                 *p_buf,
                                                CPU_SIZE_T           buf_len_max,
                                                CPU_SIZE_T           *p_tx_len);

void Ex_HTTP_Server_TransCompleteHook(const HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN           *p_conn,
                                      const void           *p_hook_cfg);

void Ex_HTTP_Server_ErrFileGetHook(const void           *p_hook_cfg,
                                   HTTP_STATUS_CODE     status_code,
                                   CPU_CHAR             *p_file_str,
                                   CPU_INT32U           file_len_max,
                                   HTTPs_BODY_DATA_TYPE *p_file_type,
                                   HTTP_CONTENT_TYPE    *p_content_type,
                                   void                 **p_data,
                                   CPU_INT32U           *p_data_len);

void Ex_HTTP_Server_ErrHook(const HTTPs_INSTANCE *p_instance,
                            HTTPs_CONN           *p_conn,
                            const void           *p_hook_cfg,
                            HTTPs_ERR            err);

void Ex_HTTP_Server_ConnCloseHook(const HTTPs_INSTANCE *p_instance,
                                  HTTPs_CONN           *p_conn,
                                  const void           *p_hook_cfg);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // EX_HTTP_SERVER_HOOKS_H
