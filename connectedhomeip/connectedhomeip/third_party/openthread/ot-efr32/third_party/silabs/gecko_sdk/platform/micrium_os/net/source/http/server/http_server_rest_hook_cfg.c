/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Rest Module
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

#include "http_server_rest_priv.h"

#include  <net/include/http_server_addon_rest.h>

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           REST CONFIGURATION
 *
 * Note(s): The REST configuration in the REST resources list ID that the HTTP server will used for the
 *          REST application.
 *******************************************************************************************************/

const HTTPs_REST_CFG HTTPs_REST_Cfg = { 0 };

/********************************************************************************************************
 *                                         REST HOOK CONFIGURATION
 *******************************************************************************************************/

const HTTPs_HOOK_CFG HTTPs_REST_HookCfg = {
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
  DEF_NULL,
  DEF_NULL,                                                     // If there is a connection error, it is most likely not recoverable.
  HTTPsREST_OnConnClosed
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
