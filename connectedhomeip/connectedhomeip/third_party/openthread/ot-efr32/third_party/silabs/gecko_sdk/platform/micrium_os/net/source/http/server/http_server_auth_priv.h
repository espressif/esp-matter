/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Authentication Add-On
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

#ifndef _HTTP_SERVER_AUTH_PRIV_H_
#define _HTTP_SERVER_AUTH_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "http_server_ctrl_layer_priv.h"

#include  "../../../include/http_server_addon_auth.h"

#include  <common/include/auth.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_BOOLEAN HTTPsAuth_InitSessionPool(const HTTPs_INSTANCE *p_instance,
                                      const void           *p_hook_cfg);

CPU_BOOLEAN HTTPsAuth_OnHdrRx(const HTTPs_INSTANCE *p_instance,
                              const HTTPs_CONN     *p_conn,
                              const void           *p_hook_cfg,
                              HTTP_HDR_FIELD       hdr_field);

CPU_BOOLEAN HTTPsAuth_OnHdrTx(const HTTPs_INSTANCE *p_instance,
                              HTTPs_CONN           *p_conn,
                              const void           *p_cfg);

CPU_BOOLEAN HTTPsAuth_OnAuth(const HTTPs_INSTANCE *p_instance,
                             HTTPs_CONN           *p_conn,
                             const void           *p_hook_cfg);

void HTTPsAuth_OnTransComplete(const HTTPs_INSTANCE *p_instance,
                               HTTPs_CONN           *p_conn,
                               const void           *p_hook_cfg);

void HTTPsAuth_OnConnClose(const HTTPs_INSTANCE *p_instance,
                           HTTPs_CONN           *p_conn,
                           const void           *p_hook_cfg);

CPU_BOOLEAN HTTPsAuth_ProcessSession(const HTTPs_INSTANCE *p_instance,
                                     HTTPs_CONN           *p_conn,
                                     const void           *p_hook_cfg,
                                     const HTTPs_KEY_VAL  *p_data);

CPU_BOOLEAN HTTPsAuth_ReqUnprotected(const HTTPs_INSTANCE *p_instance,
                                     HTTPs_CONN           *p_conn,
                                     const void           *p_hook_cfg);

CPU_BOOLEAN HTTPsAuth_ReqProtected(const HTTPs_INSTANCE *p_instance,
                                   HTTPs_CONN           *p_conn,
                                   const void           *p_hook_cfg);

CPU_BOOLEAN HTTPsAuth_ReqRdyUnprotected(const HTTPs_INSTANCE *p_instance,
                                        HTTPs_CONN           *p_conn,
                                        const void           *p_hook_cfg,
                                        const HTTPs_KEY_VAL  *p_data);

CPU_BOOLEAN HTTPsAuth_ReqRdyProtected(const HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN           *p_conn,
                                      const void           *p_hook_cfg,
                                      const HTTPs_KEY_VAL  *p_data);

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _HTTP_SERVER_AUTH_PRIV_H_
