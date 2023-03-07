/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Control Layer Add-On
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef _HTTP_SERVER_CTRL_LAYER_PRIV_H_
#define _HTTP_SERVER_CTRL_LAYER_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/http_server.h>
#include  <net/include/http_server_addon_ctrl_layer.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CTRL LAYER DATA ENTRY DATA TYPE
 *******************************************************************************************************/

typedef  struct  https_ctrl_layer_data_entry HTTPs_CTRL_LAYER_DATA_ENTRY;

struct  https_ctrl_layer_data_entry {
  CPU_INT32U                  OwnerId;
  void                        *DataPtr;
  HTTPs_CTRL_LAYER_DATA_ENTRY *NextPtr;
};

/********************************************************************************************************
 *                                   CTRL LAYER INSTANCE DATA TYPE
 *
 * Notes: (1) Structure of the memory management of the control layer.
 *******************************************************************************************************/

typedef  struct  https_ctrl_layer_inst_data {
  MEM_DYN_POOL                ConnDataPool;
  MEM_DYN_POOL                ConnDataEntryPool;
  MEM_DYN_POOL                InstDataEntryPool;
  HTTPs_CTRL_LAYER_DATA_ENTRY *InstDataHeadPtr;
} HTTPs_CTRL_LAYER_INST_DATA;

/********************************************************************************************************
 *                                   CTRL LAYER CONNECTION DATA TYPE
 *
 * Notes: (1) Structure for the ConnDataPtr substitution in the HTTPs_CONN
 *******************************************************************************************************/

typedef  struct  https_ctrl_layer_conn_data {
  HTTPs_CTRL_LAYER_CFG        *TargetCfgPtr;
  HTTPs_CTRL_LAYER_APP_INST   *TargetAppInstPtr;
  HTTPs_CTRL_LAYER_DATA_ENTRY *ConnDataHeadPtr;
} HTTPs_CTRL_LAYER_CONN_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               MEMORY MODULE
 *******************************************************************************************************/

HTTPs_CTRL_LAYER_INST_DATA *HTTPsCtrlLayerMem_InstDataAlloc(void);

CPU_BOOLEAN HTTPsCtrlLayerMem_InstDataPoolInit(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                               CPU_SIZE_T                 pool_size_max);

CPU_BOOLEAN HTTPsCtrlLayerMem_ConnDataPoolInit(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                               CPU_SIZE_T                 pool_size_max);

CPU_BOOLEAN HTTPsCtrlLayerMem_ConnDataEntryPoolInit(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                                    CPU_SIZE_T                 pool_size_max);

HTTPs_CTRL_LAYER_DATA_ENTRY *HTTPsCtrlLayerMem_InstDataEntryAlloc(HTTPs_CTRL_LAYER_INST_DATA *p_seg);

HTTPs_CTRL_LAYER_CONN_DATA *HTTPsCtrlLayerMem_ConnDataAlloc(HTTPs_CTRL_LAYER_INST_DATA *p_seg);

void HTTPsCtrlLayer_ConnDataFree(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                 HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data);

HTTPs_CTRL_LAYER_DATA_ENTRY *HTTPsCtrlLayer_ConnDataEntryAlloc(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                                               HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data);

void HTTPsCtrlLayer_ConnDataEntriesFree(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                        HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data);

/********************************************************************************************************
 *                                               CORE MODULE
 *******************************************************************************************************/

CPU_BOOLEAN HTTPsCtrlLayer_OnInstanceInit(const HTTPs_INSTANCE *p_instance,
                                          const void           *p_cfg);

CPU_BOOLEAN HTTPsCtrlLayer_OnReqRxHdr(const HTTPs_INSTANCE *p_instance,
                                      const HTTPs_CONN     *p_conn,
                                      const void           *p_cfg,
                                      HTTP_HDR_FIELD       hdr_field);

CPU_BOOLEAN HTTPsCtrlLayer_OnReq(const HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN           *p_conn,
                                 const void           *p_cfg);

CPU_BOOLEAN HTTPsCtrlLayer_OnReqRxBody(const HTTPs_INSTANCE *p_instance,
                                       HTTPs_CONN           *p_conn,
                                       const void           *p_cfg,
                                       void                 *p_buf,
                                       const CPU_SIZE_T     buf_size,
                                       CPU_SIZE_T           *p_buf_size_used);

CPU_BOOLEAN HTTPsCtrlLayer_OnReqRdySignal(const HTTPs_INSTANCE *p_instance,
                                          HTTPs_CONN           *p_conn,
                                          const void           *p_cfg,
                                          const HTTPs_KEY_VAL  *p_data);

CPU_BOOLEAN HTTPsCtrlLayer_OnReqRdyPoll(const HTTPs_INSTANCE *p_instance,
                                        HTTPs_CONN           *p_conn,
                                        const void           *p_cfg);

CPU_BOOLEAN HTTPsCtrlLayer_OnRespTxHdr(const HTTPs_INSTANCE *p_instance,
                                       HTTPs_CONN           *p_conn,
                                       const void           *p_cfg);

CPU_BOOLEAN HTTPsCtrlLayer_OnRespToken(const HTTPs_INSTANCE *p_instance,
                                       HTTPs_CONN           *p_conn,
                                       const void           *p_cfg,
                                       const CPU_CHAR       *p_token,
                                       CPU_INT16U           token_len,
                                       CPU_CHAR             *p_val,
                                       CPU_INT16U           val_len_max);

CPU_BOOLEAN HTTPsCtrlLayer_OnRespChunk(const HTTPs_INSTANCE *p_instance,
                                       HTTPs_CONN           *p_conn,
                                       const void           *p_cfg,
                                       void                 *p_buf,
                                       CPU_SIZE_T           buf_len_max,
                                       CPU_SIZE_T           *p_tx_len);

void HTTPsCtrlLayer_OnTransComplete(const HTTPs_INSTANCE *p_instance,
                                    HTTPs_CONN           *p_conn,
                                    const void           *p_cfg);

void HTTPsCtrlLayer_OnErr(const HTTPs_INSTANCE *p_instance,
                          HTTPs_CONN           *p_conn,
                          const void           *p_cfg,
                          HTTPs_ERR            err);

void HTTPsCtrlLayer_OnErrFileGet(const void           *p_cfg,
                                 HTTP_STATUS_CODE     status_code,
                                 CPU_CHAR             *p_file_str,
                                 CPU_INT32U           file_len_max,
                                 HTTPs_BODY_DATA_TYPE *p_file_type,
                                 HTTP_CONTENT_TYPE    *p_content_type,
                                 void                 **p_data,
                                 CPU_INT32U           *p_date_len);

void HTTPsCtrlLayer_OnConnClose(const HTTPs_INSTANCE *p_instance,
                                HTTPs_CONN           *p_conn,
                                const void           *p_cfg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _HTTP_SERVER_CTRL_LAYER_PRIV_H_
