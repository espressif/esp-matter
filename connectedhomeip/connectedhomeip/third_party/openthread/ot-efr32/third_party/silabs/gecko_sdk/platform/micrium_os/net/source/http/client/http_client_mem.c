/***************************************************************************//**
 * @file
 * @brief Network - HTTP Client Memory Module
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

#if (defined(RTOS_MODULE_NET_HTTP_CLIENT_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error HTTP Client Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "http_client_priv.h"

#ifdef HTTPc_WEBSOCK_MODULE_EN
#include  "http_client_websock_priv.h"
#endif

#include  <net/include/http_client.h>
#include  <net/include/http.h>

#include  <cpu/include/cpu.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern HTTPc_DATA *HTTPc_DataPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          HTTPc_Mem_MsgGet()
 *
 * @brief    Get a HTTPc Message object.
 *
 * @return   If operation is successful, Pointer to the allocated Message object.
 *           If operation has failed,    DEF_NULL.
 *******************************************************************************************************/
HTTPc_TASK_MSG *HTTPc_Mem_TaskMsgGet(RTOS_ERR *p_err)
{
  HTTPc_TASK_MSG *p_msg = DEF_NULL;

  p_msg = (HTTPc_TASK_MSG *) Mem_DynPoolBlkGet(&HTTPc_DataPtr->MsgPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_Clr(p_msg, sizeof(HTTPc_TASK_MSG));

exit:
  return (p_msg);
}

/****************************************************************************************************//**
 *                                        HTTPc_Mem_MsgRelease()
 *
 * @brief    Release an allocated HTTPc Message object.
 *
 * @param    p_msg   Pointer to the Message object to free.
 *******************************************************************************************************/
void HTTPc_Mem_TaskMsgRelease(HTTPc_TASK_MSG *p_msg)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  Mem_DynPoolBlkFree(&HTTPc_DataPtr->MsgPool,
                     p_msg,
                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                       HTTPc_Mem_WebSockReqGet()
 *
 * @brief    Get a WebSocket Request object.
 *
 * @return   If operation is successful, Pointer to the allocated WebSocket Request object.
 *           If operation has failed,    DEF_NULL.
 *******************************************************************************************************/
#ifdef  HTTPc_WEBSOCK_MODULE_EN
HTTPc_WEBSOCK_REQ *HTTPc_Mem_WebSockReqGet(RTOS_ERR *p_err)
{
  HTTPc_WEBSOCK_REQ *p_ws_req = DEF_NULL;

  p_ws_req = (HTTPc_WEBSOCK_REQ *) Mem_DynPoolBlkGet(&HTTPc_DataPtr->WebSockReqPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_Clr(p_ws_req, sizeof(HTTPc_WEBSOCK_REQ));

exit:
  return (p_ws_req);
}
#endif

/****************************************************************************************************//**
 *                                     HTTPc_Mem_WebSockReqRelease()
 *
 * @brief    Release an allocated WebSocket Request object.
 *
 * @param    p_ws_req    Pointer to the WebSocket Request object to free.
 *******************************************************************************************************/
#ifdef  HTTPc_WEBSOCK_MODULE_EN
void HTTPc_Mem_WebSockReqRelease(HTTPc_WEBSOCK_REQ *p_ws_req)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  Mem_DynPoolBlkFree(&HTTPc_DataPtr->WebSockReqPool,
                     p_ws_req,
                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_CLIENT_AVAIL
