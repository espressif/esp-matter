/***************************************************************************//**
 * @file
 * @brief Network - Http Server Control Layer Add-On
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

#ifndef _HTTP_SERVER_ADDON_CTRL_LAYER_H_
#define _HTTP_SERVER_ADDON_CTRL_LAYER_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/http_server.h>

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               CTRL LAYER AUTHENTICATION HOOKS DATA TYPE
 *******************************************************************************************************/

typedef  struct  https_ctrl_layer_auth_hooks {
  HTTPs_INSTANCE_INIT_HOOK  OnInstanceInit;
  HTTPs_REQ_HDR_RX_HOOK     OnReqHdrRx;
  HTTPs_REQ_HOOK            OnReqAuth;
  HTTPs_RESP_HDR_TX_HOOK    OnRespHdrTx;
  HTTPs_TRANS_COMPLETE_HOOK OnTransComplete;
  HTTPs_CONN_CLOSE_HOOK     OnConnClose;
} HTTPs_CTRL_LAYER_AUTH_HOOKS;

/********************************************************************************************************
 *                                CTRL LAYER APPLICATION HOOKS DATA TYPE
 *******************************************************************************************************/

typedef  struct  https_ctrl_layer_app_hooks {
  HTTPs_INSTANCE_INIT_HOOK  OnInstanceInit;
  HTTPs_REQ_HDR_RX_HOOK     OnReqHdrRx;
  HTTPs_REQ_HOOK            OnReq;
  HTTPs_REQ_BODY_RX_HOOK    OnReqBodyRx;
  HTTPs_REQ_RDY_SIGNAL_HOOK OnReqSignal;
  HTTPs_REQ_RDY_POLL_HOOK   OnReqPoll;
  HTTPs_RESP_HDR_TX_HOOK    OnRespHdrTx;
  HTTPs_RESP_TOKEN_HOOK     OnRespToken;
  HTTPs_RESP_CHUNK_HOOK     OnRespChunk;
  HTTPs_TRANS_COMPLETE_HOOK OnTransComplete;
  HTTPs_ERR_HOOK            OnError;
  HTTPs_CONN_CLOSE_HOOK     OnConnClose;
} HTTPs_CTRL_LAYER_APP_HOOKS;

/********************************************************************************************************
 *                                CTRL LAYER AUTHENTIFIACTION DATA TYPE
 *******************************************************************************************************/

typedef  struct  https_ctrl_layer_auth_inst {
  const HTTPs_CTRL_LAYER_AUTH_HOOKS *HooksPtr;
  void                              *HooksCfgPtr;
} HTTPs_CTRL_LAYER_AUTH_INST;

/********************************************************************************************************
 *                                CTRL LAYER APPLICATION DATA TYPE
 *******************************************************************************************************/

typedef  struct  https_ctrl_layer_app_inst {
  const HTTPs_CTRL_LAYER_APP_HOOKS *HooksPtr;
  void                             *HooksCfgPtr;
} HTTPs_CTRL_LAYER_APP_INST;

/********************************************************************************************************
 *                                CTRL LAYER CONFIGUATION DATA TYPE
 *******************************************************************************************************/

typedef  struct  https_ctrl_layer_cfg {
  HTTPs_CTRL_LAYER_AUTH_INST **AuthInstsPtr;
  CPU_SIZE_T                 AuthInstsNbr;
  HTTPs_CTRL_LAYER_APP_INST  **AppInstsPtr;
  CPU_SIZE_T                 AppInstsNbr;
} HTTPs_CTRL_LAYER_CFG;

/********************************************************************************************************
 *                               CTRL LAYER CONFIGURATION LIST DATA TYPE
 *******************************************************************************************************/

typedef  struct  https_ctrl_layer_cfg_List {
  HTTPs_CTRL_LAYER_CFG **CfgsPtr;
  CPU_SIZE_T           Size;
} HTTPs_CTRL_LAYER_CFG_LIST;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern const HTTPs_HOOK_CFG HTTPsCtrlLayer_HookCfg;

extern const HTTPs_CTRL_LAYER_APP_HOOKS HTTPsCtrlLayer_REST_App;

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _HTTP_SERVER_ADDON_CTRL_LAYER_H_
