/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Control Layer Add-On - REST Configuration
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
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error HTTP Server Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "http_server_rest_priv.h"

#include  <net/include/http_server_addon_ctrl_layer.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

const HTTPs_CTRL_LAYER_APP_HOOKS HTTPsCtrlLayer_REST_App = {
  HTTPsREST_Init,
  HTTPsREST_RxHeader,
  HTTPsREST_Authenticate,
  HTTPsREST_RxBody,
  HTTPsREST_ReqRdySignal,
  DEF_NULL,                                                     // Poll not used by REST. Same mechanism replaced by GET_CHUNK and RX_BODY
  DEF_NULL,                                                     // Headers will be added before the chunk call
  DEF_NULL,                                                     // No token replacement for REST
  HTTPsREST_GetChunk,
  HTTPsREST_OnTransComplete,
  DEF_NULL,                                                     // If there is a connection error, it is most likely not recoverable.
  HTTPsREST_OnConnClosed
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
