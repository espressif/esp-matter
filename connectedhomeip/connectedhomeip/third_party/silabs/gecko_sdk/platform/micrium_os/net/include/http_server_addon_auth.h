/***************************************************************************//**
 * @file
 * @brief Network - Http Server Authentication Add-On
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

#ifndef _HTTP_SERVER_ADDON_AUTH_H_
#define _HTTP_SERVER_ADDON_AUTH_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/http_server.h>
#include  <net/include/http_server_addon_ctrl_layer.h>

#include  <cpu/include/cpu.h>
#include  <common/include/auth.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef enum https_auth_state {
  HTTPs_AUTH_STATE_REQ_URL,
  HTTPs_AUTH_STATE_REQ_COMPLETE
} HTTPs_AUTH_STATE;

typedef  struct  https_auth_result {
  CPU_CHAR *RedirectPathOnValidCredPtr;
  CPU_CHAR *RedirectPathOnInvalidCredPtr;
  CPU_CHAR *RedirectPathOnNoCredPtr;
  CPU_CHAR *UsernamePtr;
  CPU_CHAR *PasswordPtr;
}  HTTPs_AUTH_RESULT;

typedef  CPU_BOOLEAN (*HTTPs_AUTH_PARSE_LOGIN_FNCT)            (const HTTPs_INSTANCE *p_inst,
                                                                const HTTPs_CONN     *p_conn,
                                                                HTTPs_AUTH_STATE     state,
                                                                HTTPs_AUTH_RESULT    *p_result);

typedef  CPU_BOOLEAN (*HTTPs_AUTH_PARSE_LOGOUT_FNCT)           (const HTTPs_INSTANCE *p_inst,
                                                                const HTTPs_CONN     *p_conn,
                                                                HTTPs_AUTH_STATE     state);

typedef  AUTH_RIGHT (*HTTPs_AUTH_GET_REQUIRED_RIGHT_FNCT)     (const HTTPs_INSTANCE *p_inst,
                                                               const HTTPs_CONN     *p_conn);

typedef  struct  HTTPs_Authentication_Cfg {
  HTTPs_AUTH_PARSE_LOGIN_FNCT  ParseLogin;
  HTTPs_AUTH_PARSE_LOGOUT_FNCT ParseLogout;
} HTTPs_AUTH_CFG;

typedef  struct  HTTPs_Authorization_Cfg {
  HTTPs_AUTH_GET_REQUIRED_RIGHT_FNCT GetRequiredRights;
} HTTPs_AUTHORIZATION_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern HTTPs_CTRL_LAYER_AUTH_HOOKS HTTPsAuth_CookieHooksCfg;
extern HTTPs_CTRL_LAYER_APP_HOOKS  HTTPsAuth_AppUnprotectedCookieHooksCfg;
extern HTTPs_CTRL_LAYER_APP_HOOKS  HTTPsAuth_AppProtectedCookieHooksCfg;

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _HTTP_SERVER_ADDON_AUTH_H_
