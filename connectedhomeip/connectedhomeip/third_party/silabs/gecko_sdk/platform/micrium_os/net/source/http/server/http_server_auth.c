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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
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
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "http_server_auth_priv.h"

#include  <net/include/http_server_addon_auth.h>
#include  <net/include/http_server_addon_ctrl_layer.h>

#include  <common/include/lib_math.h>
#include  <common/include/lib_utils.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

#define  HTTPs_AUTH_SESSION_RELEASE_TIMEOUT_MS           10
#define  HTTPs_AUTH_SESSION_TIMEOUT_MIN                  1
#define  HTTPs_AUTH_SESSION_TIMEOUT_SEC             (HTTPs_AUTH_SESSION_TIMEOUT_MIN * DEF_TIME_NBR_SEC_PER_MIN)

#define  HTTPs_AUTH_USER_LOGGED_MAX_NBR                  3

#define  HTTPs_AUTH_COOKIE_TAG_NAME_SESSION_ID           "session_id"
#define  HTTPs_AUTH_COOKIE_TAG_NAME_MAX_VALUE            "Max-Value"

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

HTTPs_CTRL_LAYER_AUTH_HOOKS HTTPsAuth_CookieHooksCfg = {
  DEF_NULL,
  HTTPsAuth_OnHdrRx,
  HTTPsAuth_OnAuth,
  HTTPsAuth_OnHdrTx,
  HTTPsAuth_OnTransComplete,
  HTTPsAuth_OnConnClose
};

HTTPs_CTRL_LAYER_APP_HOOKS HTTPsAuth_AppUnprotectedCookieHooksCfg = {
  HTTPsAuth_InitSessionPool,
  DEF_NULL,
  HTTPsAuth_ReqUnprotected,
  DEF_NULL,
  HTTPsAuth_ReqRdyUnprotected,
  DEF_NULL,
  HTTPsAuth_OnHdrTx,
  DEF_NULL,
  DEF_NULL,
  HTTPsAuth_OnTransComplete,
  DEF_NULL,
  HTTPsAuth_OnConnClose
};

HTTPs_CTRL_LAYER_APP_HOOKS HTTPsAuth_AppProtectedCookieHooksCfg = {
  DEF_NULL,
  DEF_NULL,
  HTTPsAuth_ReqProtected,
  DEF_NULL,
  HTTPsAuth_ReqRdyProtected,
  DEF_NULL,
  HTTPsAuth_OnHdrTx,
  DEF_NULL,
  DEF_NULL,
  HTTPsAuth_OnTransComplete,
  DEF_NULL,
  HTTPsAuth_OnConnClose
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct https_auth_session HTTPs_AUTH_SESSION;

struct https_auth_session {
  CPU_INT16U         SessionID;
  AUTH_USER_HANDLE   UserHandle;
  sl_sleeptimer_timestamp_t ExpireTS;
  HTTPs_AUTH_RESULT  Result;
  HTTPs_AUTH_SESSION *NextPtr;
  HTTPs_AUTH_SESSION *PrevPtr;
};

typedef  struct https_auth_data {
  MEM_DYN_POOL       SessionPool;
  MEM_DYN_POOL       UserNamePool;
  HTTPs_AUTH_SESSION *SessionListPtr;
  KAL_TMR_HANDLE     SessionTmr;
} HTTPs_AUTH_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static HTTPs_AUTH_DATA *HTTPsAuth_DataPtr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/
#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
static CPU_INT16U HTTPsAuth_SetCookieStr(CPU_CHAR    *p_cookie_tag,
                                         CPU_CHAR    *p_cookie_value,
                                         CPU_CHAR    *p_buf,
                                         CPU_INT16U  buf_len,
                                         CPU_BOOLEAN nul);
#endif

static void HTTPsAuth_Redirect(const HTTPs_INSTANCE *p_instance,
                               HTTPs_CONN           *p_conn,
                               const CPU_CHAR       *p_uri);

static HTTPs_AUTH_SESSION *HTTPsAuth_SessionRetrieveFromHdr(const HTTPs_INSTANCE *p_instance,
                                                            HTTPs_CONN           *p_conn);

#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
static HTTPs_AUTH_SESSION *HTTPsAuth_SessionComputeFromHdr(HTTPs_HDR_BLK *p_cookie_blk);

static HTTPs_AUTH_SESSION *HTTPsAuth_SessionSrch(CPU_INT16U session_id);
#endif

static HTTPs_AUTH_SESSION *HTTPsAuth_SessionGet(void);

static void HTTPsAuth_SessionRelease(HTTPs_AUTH_SESSION *p_session);

static void HTTPsAuth_SessionReleaseTmr(void *p_arg);

static CPU_INT32U HTTPsAuth_SessionGenerateID(CPU_INT32U seed);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           HOOK FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                     HTTPsAuth_InitSessionPool()
 *
 * @brief    Initialize the session pool.
 *
 * @param    p_instance  Pointer to HTTPs Instance object.
 *
 * @param    p_hook_cfg  Pointer to hook configuration.
 *
 * @note     (1) This function is Control Layer hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsAuth_InitSessionPool(const HTTPs_INSTANCE *p_instance,
                                      const void           *p_hook_cfg)
{
  KAL_TMR_EXT_CFG tmr_cfg;
  RTOS_ERR        local_err;

  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_hook_cfg);

  HTTPsAuth_DataPtr = (HTTPs_AUTH_DATA *)Mem_SegAlloc("HTTPs Auth Module",
                                                      DEF_NULL,
                                                      sizeof(HTTPs_AUTH_DATA),
                                                      &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  //                                                               Create the session memory pool.
  Mem_DynPoolCreate("HTTPs Session Pool",
                    &HTTPsAuth_DataPtr->SessionPool,
                    DEF_NULL,
                    sizeof(HTTPs_AUTH_SESSION),
                    sizeof(CPU_ALIGN),
                    HTTPs_AUTH_USER_LOGGED_MAX_NBR,
                    HTTPs_AUTH_USER_LOGGED_MAX_NBR,
                    &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  //                                                               Set the first pointer to NULL which indicate ...
  HTTPsAuth_DataPtr->SessionListPtr = DEF_NULL;                 // there is no active session.

  //                                                               Create and Start the timer which check for ...
  //                                                               releasing expired session.

  tmr_cfg.Opt = KAL_OPT_TMR_NONE | KAL_OPT_TMR_PERIODIC;
  HTTPsAuth_DataPtr->SessionTmr = KAL_TmrCreate("HTTPs Auth Session release timer",
                                                HTTPsAuth_SessionReleaseTmr,
                                                DEF_NULL,
                                                HTTPs_AUTH_SESSION_RELEASE_TIMEOUT_MS,
                                                &tmr_cfg,
                                                &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  KAL_TmrStart(HTTPsAuth_DataPtr->SessionTmr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                         HTTPsAuth_OnHdrRx()
 *
 * @brief    Hook function called when HTTP header is received.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_hook_cfg  Pointer to hook configuration.
 *
 * @param    hdr_field   Header field received.
 *
 * @return   DEF_YES, to keep header.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) This function is Control Layer hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsAuth_OnHdrRx(const HTTPs_INSTANCE *p_instance,
                              const HTTPs_CONN     *p_conn,
                              const void           *p_hook_cfg,
                              HTTP_HDR_FIELD       hdr_field)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_hook_cfg);
  PP_UNUSED_PARAM(hdr_field);

#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
  switch (hdr_field) {
    case HTTP_HDR_FIELD_AUTHORIZATION:
      return (DEF_YES);

    case HTTP_HDR_FIELD_COOKIE:
    case HTTP_HDR_FIELD_COOKIE2:
      return (DEF_YES);

    case HTTP_HDR_FIELD_USER_AGENT:
      return (DEF_YES);

    default:
      return (DEF_NO);
  }
#else
  return (DEF_NO);
#endif
}

/****************************************************************************************************//**
 *                                         HTTPsAuth_OnHdrTx()
 *
 * @brief    Hook function called to add HTTP header to HTTP server response.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_cfg       Pointer to hook configuration.
 *
 * @return   DEF_OK
 *           DEF_FAIL
 *
 * @note     (1) This function is Control Layer hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsAuth_OnHdrTx(const HTTPs_INSTANCE *p_instance,
                              HTTPs_CONN           *p_conn,
                              const void           *p_cfg)
{
#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
  HTTPs_HDR_BLK      *p_resp_hdr_blk;
  const HTTPs_CFG    *p_http_cfg;
  HTTPs_AUTH_SESSION *p_session;
  sl_sleeptimer_timestamp_t ts_current;
  CPU_CHAR           *p_str;
  CPU_SIZE_T         str_len;
  CPU_SIZE_T         len;
  CPU_CHAR           value_str[DEF_INT_16U_NBR_DIG_MAX + 1];
  RTOS_ERR           err;

  p_http_cfg = p_instance->CfgPtr;

  switch (p_conn->StatusCode) {
    case HTTP_STATUS_OK:
    case HTTP_STATUS_SEE_OTHER:

      if (p_conn->ConnDataPtr != DEF_NULL) {
        p_session = (HTTPs_AUTH_SESSION *)p_conn->ConnDataPtr;
      } else {
        p_session = HTTPsAuth_SessionRetrieveFromHdr(p_instance, p_conn);
      }

      //                                                           Send back the cookie if there is a active session.
      if (p_session != DEF_NULL) {
        //                                                         Get and add header block to the connection.
        p_resp_hdr_blk = HTTPs_RespHdrGet((HTTPs_INSTANCE *)p_instance,
                                          p_conn,
                                          HTTP_HDR_FIELD_SET_COOKIE,
                                          HTTPs_HDR_VAL_TYPE_STR_DYN,
                                          &err);
        if (p_resp_hdr_blk == DEF_NULL) {
          return (DEF_FAIL);
        }

        //                                                         Get the data buf pointer and the buf_len in...
        //                                                         ...temporary variables.
        p_str = (CPU_CHAR *)p_resp_hdr_blk->ValPtr;
        str_len = p_http_cfg->HdrTxCfgPtr->DataLenMax;

        //                                                         Set the session_id cookie.
        Str_FmtNbr_Int32U(p_session->SessionID,
                          DEF_INT_16U_NBR_DIG_MAX,
                          DEF_NBR_BASE_DEC,
                          ASCII_CHAR_DIGIT_ZERO,
                          DEF_NO,
                          DEF_YES,
                          value_str);

        len = HTTPsAuth_SetCookieStr(HTTPs_AUTH_COOKIE_TAG_NAME_SESSION_ID,
                                     value_str,
                                     p_str,
                                     str_len,
                                     DEF_NO);

        //                                                         Refresh the pointer and the remaining buf_len.
        p_str += len;
        str_len -= len;

        //                                                         Set the Max_Value cookie.
        Str_FmtNbr_Int32U(HTTPs_AUTH_SESSION_TIMEOUT_SEC,
                          DEF_INT_16U_NBR_DIG_MAX,
                          DEF_NBR_BASE_DEC,
                          DEF_NULL,
                          DEF_NO,
                          DEF_YES,
                          value_str);

        (void)HTTPsAuth_SetCookieStr(HTTPs_AUTH_COOKIE_TAG_NAME_MAX_VALUE,
                                     value_str,
                                     p_str,
                                     str_len,
                                     DEF_YES);

        //                                                         Set the total header length.
        p_resp_hdr_blk->ValLen = Str_Len((CPU_CHAR *)p_resp_hdr_blk->ValPtr);

        // Refresh the expiration time of the session.
        ts_current = sl_sleeptimer_get_time();
        p_session->ExpireTS = ts_current + HTTPs_AUTH_SESSION_TIMEOUT_SEC;
      }
      break;

    default:
      break;
  }
#endif

  PP_UNUSED_PARAM(p_cfg);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_instance);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                          HTTPs_Auth_OnAuth()
 *
 * @brief    Upon Authorization request (When this module acts as a barrier for content).
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_hook_cfg  Pointer to hook configuration.
 *
 * @return   DEF_YES
 *           DEF_NO
 *
 * @note     (1) This function is Control Layer hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsAuth_OnAuth(const HTTPs_INSTANCE *p_instance,
                             HTTPs_CONN           *p_conn,
                             const void           *p_hook_cfg)
{
  HTTPs_AUTH_SESSION      *p_session;
  HTTPs_AUTHORIZATION_CFG *p_auth_cfg;
  AUTH_RIGHT              req_rights;
  AUTH_RIGHT              user_rights;
  RTOS_ERR                local_err;

  p_session = HTTPsAuth_SessionRetrieveFromHdr(p_instance, p_conn);

  if (p_session != DEF_NULL) {
    p_auth_cfg = (HTTPs_AUTHORIZATION_CFG *)p_hook_cfg;

    req_rights = p_auth_cfg->GetRequiredRights(p_instance, p_conn);

    user_rights = Auth_GetUserRight(p_session->UserHandle, &local_err);

    if (DEF_BIT_IS_SET(user_rights, req_rights)) {
      return (DEF_YES);
    }
  }

  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                      HTTPsAuth_OnTransComplete()
 *
 * @brief    Called when an HTTP Transaction has been completed.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_hook_cfg  Pointer to hook configuration.
 *
 * @note     (1) This function is Control Layer hook function.
 *******************************************************************************************************/
void HTTPsAuth_OnTransComplete(const HTTPs_INSTANCE *p_instance,
                               HTTPs_CONN           *p_conn,
                               const void           *p_hook_cfg)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_hook_cfg);

  p_conn->ConnDataPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                       HTTPs_Auth_OnConnClose()
 *
 * @brief    Called when any connection is closed.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_hook_cfg  Pointer to hook configuration.
 *
 * @note     (1) This function is Control Layer hook function.
 *******************************************************************************************************/
void HTTPsAuth_OnConnClose(const HTTPs_INSTANCE *p_instance,
                           HTTPs_CONN           *p_conn,
                           const void           *p_hook_cfg)
{
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_hook_cfg);

  p_conn->ConnDataPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                       HTTPsAuth_ReqUnprotected()
 *
 * @brief    This function is called when a unprotected request is received, i.e. the request received
 *           is not associated with a session. The upper application is called to ask what to do with
 *           the request, i.e. allow it or redirect it to another page.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_hook_cfg  Pointer to hook configuration.
 *
 * @return   DEF_YES
 *           DEF_NO
 *
 * @note     (1) This function is Control Layer hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsAuth_ReqUnprotected(const HTTPs_INSTANCE *p_instance,
                                     HTTPs_CONN           *p_conn,
                                     const void           *p_hook_cfg)
{
  const HTTPs_AUTH_CFG *p_cfg = (const HTTPs_AUTH_CFG *)p_hook_cfg;
  HTTPs_AUTH_RESULT    result;

  if (p_cfg->ParseLogin != DEF_NULL) {
    (void)p_cfg->ParseLogin(p_instance, p_conn, HTTPs_AUTH_STATE_REQ_URL, &result);

    if (result.RedirectPathOnNoCredPtr != DEF_NULL) {
      HTTPsAuth_Redirect(p_instance, p_conn, result.RedirectPathOnNoCredPtr);
    }
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                       HTTPsAuth_ReqProtected()
 *
 * @brief    This function is called when a request is received and the authentication and authorization
 *           have been accepted. We ask the upper application if the request has an URL related to the
 *           authentication module, i.e login or logout page, so that the authentication module can
 *           process those requests.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_hook_cfg  Pointer to hook configuration.
 *
 * @return   DEF_YES, if the received request must be process by the authentication module.
 *           DEF_NO,  if the request has nothing to do with the authentication module.
 *
 * @note     (1) This function is Control Layer hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsAuth_ReqProtected(const HTTPs_INSTANCE *p_instance,
                                   HTTPs_CONN           *p_conn,
                                   const void           *p_hook_cfg)
{
  HTTPs_AUTH_SESSION   *p_session;
  const HTTPs_AUTH_CFG *p_cfg = (const HTTPs_AUTH_CFG *)p_hook_cfg;
  CPU_INT16S           cmp_val;
  CPU_BOOLEAN          is_login;
  CPU_BOOLEAN          is_logout;
  CPU_BOOLEAN          val_return;

  p_session = HTTPsAuth_SessionRetrieveFromHdr(p_instance, p_conn);
  if (p_session == DEF_NULL) {                                  // Case when session timed out.
    p_conn->ConnDataPtr = DEF_NULL;
    val_return = HTTPsAuth_ReqUnprotected(p_instance, p_conn, p_hook_cfg);
    return (val_return);
  }

  if (p_cfg->ParseLogin != DEF_NULL) {                          // Check if POST login request received.
    is_login = p_cfg->ParseLogin(p_instance, p_conn, HTTPs_AUTH_STATE_REQ_URL, &p_session->Result);

    if (is_login == DEF_YES) {
      return (DEF_YES);
    }
  }

  if (p_cfg->ParseLogout != DEF_NULL) {                         // Check if POST logout request received.
    is_logout = p_cfg->ParseLogout(p_instance, p_conn, HTTPs_AUTH_STATE_REQ_URL);

    if (is_logout == DEF_YES) {
      return (DEF_YES);
    }
  }

  //                                                               Check if request if for login error/redirect pages.
  cmp_val = Str_Cmp(p_conn->PathPtr, p_session->Result.RedirectPathOnNoCredPtr);
  if (cmp_val != 0) {
    cmp_val = Str_Cmp(p_conn->PathPtr, p_session->Result.RedirectPathOnInvalidCredPtr);
  }
  if (cmp_val == 0) {
    HTTPsAuth_Redirect(p_instance, p_conn, p_session->Result.RedirectPathOnValidCredPtr);
    return (DEF_YES);
  }

  //                                                               Check if request has something to do with login.
  if (p_session->Result.RedirectPathOnNoCredPtr == DEF_NULL) {
    return (DEF_YES);
  }

  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                     HTTPsAuth_ReqRdyUnprotected()
 *
 * @brief    This function is called when a unprotected request (not yet login) is being process by the
 *           authentication module. The upper application is called to parse the info received in the
 *           request particularly the form fields, if any, for login/logout info.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_hook_cfg  Pointer to hook configuration.
 *
 * @param    p_data      Pointer to Form Key-Val data received in HTTP request if any.
 *
 * @return   DEF_YES, if the HTTP response can be send.
 *           DEF_NO,  if Poll Hook function must be called.
 *
 * @note     (1) This function is Control Layer hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsAuth_ReqRdyUnprotected(const HTTPs_INSTANCE *p_instance,
                                        HTTPs_CONN           *p_conn,
                                        const void           *p_hook_cfg,
                                        const HTTPs_KEY_VAL  *p_data)
{
  HTTPs_AUTH_SESSION   *p_session;
  AUTH_USER_HANDLE     user_handle;
  const HTTPs_AUTH_CFG *p_cfg = (const HTTPs_AUTH_CFG *)p_hook_cfg;
  const CPU_CHAR       *p_redirect_path = DEF_NULL;
  HTTPs_AUTH_RESULT    result;
  RTOS_ERR             local_err;

  PP_UNUSED_PARAM(p_data);

  if (p_cfg->ParseLogin != DEF_NULL) {
    (void)p_cfg->ParseLogin(p_instance, p_conn, HTTPs_AUTH_STATE_REQ_COMPLETE, &result);

    if ((result.UsernamePtr != DEF_NULL)
        && (result.PasswordPtr != DEF_NULL)    ) {
      p_session = HTTPsAuth_SessionGet();
      if (p_session == DEF_NULL) {
        return (DEF_YES);                                   // Internal error.
      }

      Mem_Copy(&p_session->Result, &result, sizeof(HTTPs_AUTH_RESULT));

      user_handle = Auth_ValidateCredentials(result.UsernamePtr,
                                             result.PasswordPtr,
                                             &local_err);
      switch (RTOS_ERR_CODE_GET(local_err)) {
        case RTOS_ERR_NONE:                                 // Success.
          p_conn->ConnDataPtr = p_session;
          p_session->UserHandle = user_handle;
          p_redirect_path = result.RedirectPathOnValidCredPtr;
          break;

        case RTOS_ERR_INVALID_CREDENTIALS:                  // Client error.
          p_redirect_path = result.RedirectPathOnInvalidCredPtr;
          HTTPsAuth_SessionRelease(p_session);
          break;

        default:                                            // Internal error.
          HTTPsAuth_SessionRelease(p_session);
          break;
      }
    }
  }

  if (p_redirect_path != DEF_NULL) {
    HTTPsAuth_Redirect(p_instance, p_conn, p_redirect_path);
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                      HTTPsAuth_ReqRdyProtected()
 *
 * @brief    This function is called when a protected request (already log in) is being process by the
 *           authentication module. The upper application is called to parse the info received in the
 *           request particularly the form fields, if any, for login/logout info.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_hook_cfg  Pointer to hook configuration.
 *
 * @param    p_data      Pointer to Form Key-Val data received in HTTP request if any.
 *
 * @return   DEF_YES, if the HTTP response can be send.
 *           DEF_NO,  if Poll Hook function must be called.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsAuth_ReqRdyProtected(const HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN           *p_conn,
                                      const void           *p_hook_cfg,
                                      const HTTPs_KEY_VAL  *p_data)
{
  HTTPs_AUTH_SESSION   *p_session;
  AUTH_USER_HANDLE     user_handle;
  const HTTPs_AUTH_CFG *p_cfg = (const HTTPs_AUTH_CFG *)p_hook_cfg;
  const CPU_CHAR       *p_redirect_path = DEF_NULL;
  HTTPs_AUTH_RESULT    *p_result;
  CPU_BOOLEAN          logout = DEF_NO;
  CPU_BOOLEAN          val_return;
  RTOS_ERR             local_err;

  p_session = HTTPsAuth_SessionRetrieveFromHdr(p_instance, p_conn);
  if (p_session == DEF_NULL) {                                  // Case when session timed out.
    p_conn->ConnDataPtr = DEF_NULL;
    val_return = HTTPsAuth_ReqRdyUnprotected(p_instance, p_conn, p_hook_cfg, p_data);
    return (val_return);
  }

  p_result = &p_session->Result;

  if (p_cfg->ParseLogin != DEF_NULL) {
    (void)p_cfg->ParseLogin(p_instance, p_conn, HTTPs_AUTH_STATE_REQ_COMPLETE, p_result);

    if ((p_result->UsernamePtr != DEF_NULL)
        && (p_result->PasswordPtr != DEF_NULL)) {
      user_handle = Auth_ValidateCredentials(p_result->UsernamePtr,
                                             p_result->PasswordPtr,
                                             &local_err);
      switch (RTOS_ERR_CODE_GET(local_err)) {
        case RTOS_ERR_NONE:                         // Success.
          p_session->UserHandle = user_handle;
          p_redirect_path = p_result->RedirectPathOnValidCredPtr;
          break;

        case RTOS_ERR_INVALID_CREDENTIALS:          // Client error.
          p_redirect_path = p_result->RedirectPathOnInvalidCredPtr;
          HTTPsAuth_SessionRelease(p_session);
          p_conn->ConnDataPtr = DEF_NULL;
          break;

        default:                                    // Internal error.
          HTTPsAuth_SessionRelease(p_session);
          p_conn->ConnDataPtr = DEF_NULL;
          break;
      }
    }
  }

  if (p_cfg->ParseLogout != DEF_NULL) {
    logout = p_cfg->ParseLogout(p_instance, p_conn, HTTPs_AUTH_STATE_REQ_COMPLETE);

    if (logout == DEF_YES) {
      p_redirect_path = p_session->Result.RedirectPathOnNoCredPtr;
      HTTPsAuth_SessionRelease(p_session);
      p_conn->ConnDataPtr = DEF_NULL;
    }
  }

  if (p_redirect_path != DEF_NULL) {
    HTTPsAuth_Redirect(p_instance, p_conn, p_redirect_path);
  }

  return (DEF_YES);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                     HTTPs_InstanceSetCookieStr()
 *
 * @brief    Format the cookie correctly in the transmit buffer.
 *
 * @param    p_cookie_tag    Pointer of char of the cookie tag (NULL terminated).
 *
 * @param    p_cookie_value  Pointer of char of the cookie value (NULL terminated).
 *
 * @param    p_buf           Destination buffer pointer of the formatted cookie.
 *
 * @param    buf_len         Maximum space available in the destination buffer.
 *
 * @return   0 to 65535      Total length used to set the formated cookie.
 *******************************************************************************************************/
#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
static CPU_INT16U HTTPsAuth_SetCookieStr(CPU_CHAR    *p_cookie_tag,
                                         CPU_CHAR    *p_cookie_value,
                                         CPU_CHAR    *p_buf,
                                         CPU_INT16U  buf_len,
                                         CPU_BOOLEAN nul)
{
  CPU_INT16U len;
  CPU_INT16U total_len;

  total_len = 0;
  //                                                               Cookie Tag.
  len = Str_Len(p_cookie_tag);
  if (len < buf_len) {
    Str_Copy_N(p_buf, p_cookie_tag, buf_len);
    buf_len -= len;
    p_buf += len;
    total_len = len;
  } else {
    return (total_len);
  }
  //                                                               '=' character delimiter.
  if (len < buf_len) {
    *p_buf = '=';
    buf_len--;
    p_buf++;
    total_len++;
  } else {
    return (total_len);
  }
  //                                                               Cookie Value.
  len = Str_Len(p_cookie_value);
  if (len < buf_len) {
    Str_Copy_N(p_buf, p_cookie_value, buf_len);
    buf_len -= len;
    p_buf += len;
    total_len += len;
  } else {
    return (total_len);
  }
  //                                                               ';' character delimiter.
  len = 1;
  if (len < buf_len) {
    *p_buf = ';';
    buf_len--;
    p_buf++;
    total_len++;
  }

  //                                                               Add null character at the end if required.
  if (nul == DEF_YES) {
    if (len < buf_len) {
      *p_buf = '\0';
      buf_len--;
      p_buf++;
      total_len++;
    }
  }

  return (total_len);
}
#endif

/****************************************************************************************************//**
 *                                            HTTPsAuth_Redirect()
 *
 * @brief    Called to redirect the client to a another page.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_uri       URI to the page to be redirected.
 *
 * @note     (1) This function modifies the following parameters of the
 *               HTTPs_CONN provided (p_conn):
 *               - (a) FileNamePtr
 *               - (b) FileType
 *               - (c) HostPtr
 *               - (d) StatusCode
 *
 * @note     (2) HTTPs_CFG_ABSOLUTE_URI_EN
 *               - Supports            DEF_DISABLED
 *               - Preferred state is  DEF_ENABLED
 *******************************************************************************************************/
static void HTTPsAuth_Redirect(const HTTPs_INSTANCE *p_instance,
                               HTTPs_CONN           *p_conn,
                               const CPU_CHAR       *p_uri)
{
  RTOS_ERR err;

  p_conn->StatusCode = HTTP_STATUS_SEE_OTHER;                   // Redirect the page...

  HTTPs_RespBodySetParamNoBody(p_instance, p_conn, &err);

#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
  Str_Copy(p_conn->HostPtr, "");
  Str_Copy(p_conn->PathPtr, p_uri);
#else
  Str_Copy(p_conn->PathPtr, p_uri);
#endif

  PP_UNUSED_PARAM(err);                                         // Prevent 'variable unused' compiler warning.
}

/****************************************************************************************************//**
 *                                        HTTPsAuth_GetSession()
 *
 * @brief    Gets the session from the current HTTPs connection received headers.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @return   Pointer to the Session object.
 *******************************************************************************************************/
static HTTPs_AUTH_SESSION *HTTPsAuth_SessionRetrieveFromHdr(const HTTPs_INSTANCE *p_instance,
                                                            HTTPs_CONN           *p_conn)
{
#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
  HTTPs_HDR_BLK      *p_req_hdr_blk;
  HTTPs_AUTH_SESSION *p_session;

  PP_UNUSED_PARAM(p_instance);

  p_req_hdr_blk = p_conn->HdrListPtr;
  //                                                               Browse the entire list of header field of the HTTP...
  //                                                               request.
  while (p_req_hdr_blk != (HTTPs_HDR_BLK *) 0) {
    switch (p_req_hdr_blk->HdrField) {
      case HTTP_HDR_FIELD_COOKIE:
        p_session = HTTPsAuth_SessionComputeFromHdr(p_req_hdr_blk);
        return (p_session);

      default:
        break;
    }

    p_req_hdr_blk = p_req_hdr_blk->NextPtr;
  }
#else
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
#endif

  return (DEF_NULL);
}

/****************************************************************************************************//**
 *                                       HTTPsAuth_ComputeSession()
 *
 * @brief    Computes the session from an header block
 *
 * @param    p_cookie_blk    Pointer to cookie header block.
 *
 * @return   Pointer to HTTP session.
 *******************************************************************************************************/
#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
static HTTPs_AUTH_SESSION *HTTPsAuth_SessionComputeFromHdr(HTTPs_HDR_BLK *p_cookie_blk)
{
  HTTPs_AUTH_SESSION *p_session;
  CPU_INT16U         session_id;
  CPU_CHAR           *p_str;
  CPU_CHAR           *p_end;

  p_str = (CPU_CHAR *)p_cookie_blk->ValPtr;
  p_end = p_str + p_cookie_blk->ValLen;

  while ((p_str != DEF_NULL)
         && (p_str < p_end)    ) {
    p_str = Str_Str_N(p_str,
                      HTTPs_AUTH_COOKIE_TAG_NAME_SESSION_ID,
                      p_end - p_str);

    if (p_str == DEF_NULL) {
      return DEF_NULL;
    }

    //                                                             Skip all the tag name plus the equal char.
    p_str += sizeof(HTTPs_AUTH_COOKIE_TAG_NAME_SESSION_ID);

    p_str[DEF_INT_16U_NBR_DIG_MAX] = ASCII_CHAR_NULL;

    session_id = (CPU_INT16U) Str_ParseNbr_Int32U(p_str,
                                                  DEF_NULL,
                                                  DEF_NBR_BASE_DEC);

    p_session = HTTPsAuth_SessionSrch(session_id);
    if (p_session != DEF_NULL) {
      return (p_session);
    } else {
      p_str += DEF_INT_16U_NBR_DIG_MAX + 1;
    }
  }

  return (DEF_NULL);
}
#endif

/****************************************************************************************************//**
 *                                      HTTPsAuth_SessionSrch()
 *
 * @brief    Find the corresponding session ID in the active session list.
 *
 * @param    session_id  The desired session ID number to be found in the active session list.
 *
 * @return   Pointer to the session object, if the session ID is in the session list.
 *           DEF_NULL,                      if the corresponding session ID is not found in the session list,
 *                                           or the session ID asked is invalid (in this case, session id = 0).
 *******************************************************************************************************/
#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
static HTTPs_AUTH_SESSION *HTTPsAuth_SessionSrch(CPU_INT16U session_id)
{
  HTTPs_AUTH_SESSION *p_session;

  if (session_id != 0) {
    //                                                             If the session id is valid.
    p_session = HTTPsAuth_DataPtr->SessionListPtr;
    while (p_session != DEF_NULL) {
      //                                                           Browse the active session list.
      if (p_session->SessionID == session_id) {
        break;                                          // If session ID match, the active session is find.
      }

      p_session = p_session->NextPtr;
    }
  } else {
    p_session = DEF_NULL;
  }

  return (p_session);
}
#endif

/****************************************************************************************************//**
 *                                      HTTPsAuth_SessionGet()
 *
 * @brief    Get the next session id structure available in the active session list.
 *
 * @return   Pointer to the next available session ID in the active session list.
 *           DEF_NULL if the list is full and the session cannot be allocated.
 *******************************************************************************************************/
static HTTPs_AUTH_SESSION *HTTPsAuth_SessionGet(void)
{
  HTTPs_AUTH_SESSION *p_session;
  HTTPs_AUTH_SESSION *p_last_session;
  RTOS_ERR           local_err;

  //                                                               Get a free session from the Session pool.
  p_session = (HTTPs_AUTH_SESSION *)Mem_DynPoolBlkGet(&HTTPsAuth_DataPtr->SessionPool,
                                                      &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);                                           // If Pool is empty or error no session are available.
  }

  if (HTTPsAuth_DataPtr->SessionListPtr == DEF_NULL) {
    HTTPsAuth_DataPtr->SessionListPtr = p_session;
    p_session->NextPtr = DEF_NULL;
    p_session->PrevPtr = DEF_NULL;
  } else {
    p_last_session = HTTPsAuth_DataPtr->SessionListPtr;
    while (p_last_session->NextPtr != DEF_NULL) {
      p_last_session = p_last_session->NextPtr;
    }
    p_last_session->NextPtr = p_session;
    p_session->PrevPtr = p_last_session;
    p_session->NextPtr = DEF_NULL;
  }

  p_session->SessionID = (CPU_INT16U)HTTPsAuth_SessionGenerateID((CPU_ADDR) p_session);

  return (p_session);                                           // Return the new session.
}

/****************************************************************************************************//**
 *                                     HTTPsAuth_SessionRelease()
 *
 * @brief    Release the session in the active session list.
 *
 * @param    p_session   Pointer on the session to be deleted form the active session list.
 *******************************************************************************************************/
static void HTTPsAuth_SessionRelease(HTTPs_AUTH_SESSION *p_session)
{
  HTTPs_AUTH_SESSION *p_session_next;
  HTTPs_AUTH_SESSION *p_session_prev;
  RTOS_ERR           local_err;

  //                                                               Update active session list by removing the session.
  if (p_session == HTTPsAuth_DataPtr->SessionListPtr) {
    if (p_session->NextPtr == DEF_NULL) {
      HTTPsAuth_DataPtr->SessionListPtr = DEF_NULL;
    } else {
      HTTPsAuth_DataPtr->SessionListPtr = p_session->NextPtr;
      HTTPsAuth_DataPtr->SessionListPtr->PrevPtr = DEF_NULL;
    }
  } else {
    p_session_prev = p_session->PrevPtr;
    p_session_next = p_session->NextPtr;

    if (p_session_prev != DEF_NULL) {
      p_session_prev->NextPtr = p_session_next;
    }

    if (p_session_next != DEF_NULL) {
      p_session_next->PrevPtr = p_session_prev;
    }
  }
  //                                                               Release the session to the pool.
  Mem_DynPoolBlkFree(&HTTPsAuth_DataPtr->SessionPool,
                     p_session,
                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                  HTTPsAuth_SessionReleaseTmr()
 *
 * @brief    Timer that check the expiration of session and release the expired session
 *           in the active session list.
 *
 * @param    p_tmr   Pointer to Timer object.
 *
 * @param    p_arg   Pointer to timer callback arguments.
 *******************************************************************************************************/
static void HTTPsAuth_SessionReleaseTmr(void *p_arg)
{
  HTTPs_AUTH_SESSION *p_session;
  sl_sleeptimer_timestamp_t time_now;

  PP_UNUSED_PARAM(p_arg);

  p_session = HTTPsAuth_DataPtr->SessionListPtr;

  time_now = sl_sleeptimer_get_time();
  while (p_session != DEF_NULL) {
    //                                                             If the session is expired...
    if (p_session->ExpireTS < time_now) {
      //                                                           ...release the session from the active session list.
      HTTPsAuth_SessionRelease(p_session);
    }
    //                                                             Move to the next session in the list.
    p_session = p_session->NextPtr;
  }
}

/****************************************************************************************************//**
 *                                   HTTPsAuth_SessionGenerateID()
 *
 * @brief    Generate a random number for the session ID
 *
 * @param    seed    Seed Value.
 *
 * @return   Random Session ID Value
 *
 * @note         (1) In this example, the value is simply random, not unique.
 *******************************************************************************************************/
static CPU_INT32U HTTPsAuth_SessionGenerateID(CPU_INT32U seed)
{
  CPU_INT32U rand;
  CPU_INT32U diff;
  CPU_INT32U val;
  RTOS_ERR   err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  Math_RandSetSeed(seed);
  rand = Math_Rand();

#if CPU_CFG_TS_32_EN == DEF_ENABLED
  rand += (CPU_INT32U)CPU_TS_Get32();
#else
  rand += (CPU_INT32U)KAL_TickGet(&err);
#endif

  Math_RandSetSeed(rand);
  rand = Math_Rand();

  diff = (DEF_INT_16U_MAX_VAL - 1) + 1;

  val = rand % diff + 1;

  PP_UNUSED_PARAM(err);

  return (val);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
