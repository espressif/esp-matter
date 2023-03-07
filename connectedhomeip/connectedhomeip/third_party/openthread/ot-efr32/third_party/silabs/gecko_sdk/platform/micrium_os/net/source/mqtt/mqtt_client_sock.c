/***************************************************************************//**
 * @file
 * @brief Network - MQTT Client
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

#if (defined(RTOS_MODULE_NET_MQTT_CLIENT_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error TFTP Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_err.h>

#include  <net/include/net_sock.h>
#include  <net/include/net_app.h>
#include  <net/source/tcpip/net_sock_priv.h>
#include  <net/source/mqtt/mqtt_client_sock_priv.h>

#include  <common/source/rtos/rtos_utils_priv.h>

#include  "mqtt_client_sock_priv.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  MQTTc_SOCK_SEL_FLAG_DESC_MSK              (DEF_BIT_00 | DEF_BIT_01 | DEF_BIT_02)
#define  MQTTc_SOCK_SEL_FLAG_DESC_RD                DEF_BIT_00
#define  MQTTc_SOCK_SEL_FLAG_DESC_WR                DEF_BIT_01
#define  MQTTc_SOCK_SEL_FLAG_DESC_ERR               DEF_BIT_02

#define  MQTTc_NET_SOCK_SEL_TIMEOUT_us              1000u

#define  LOG_DFLT_CH                               (NET, MQTT)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_SOCK_DESC MQTTc_NetSockDescRd;
static NET_SOCK_DESC MQTTc_NetSockDescWr;
static NET_SOCK_DESC MQTTc_NetSockDescErr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           MQTTc_SockConnOpen()
 *
 * @brief    Open socket for MQTTc module.
 *
 * @param    p_conn  Pointer to MQTTc_CONN to open.
 *
 * @param    p_err   Pointer to variable that will receive error code from this function.
 *******************************************************************************************************/
void MQTTc_SockConnOpen(MQTTc_CONN *p_conn,
                        RTOS_ERR   *p_err)
{
  CPU_BOOLEAN flag = DEF_TRUE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn->BrokerNamePtr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  NetApp_ClientStreamOpenByHostname(&p_conn->SockId,
                                    p_conn->BrokerNamePtr,
                                    p_conn->BrokerPortNbr,
                                    DEF_NULL,
                                    p_conn->SecureCfgPtr,
                                    p_conn->TimeoutMs,
                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_conn->SockId = NET_SOCK_ID_NONE;
    goto exit;
  }

  NetSock_CfgBlock(p_conn->SockId, NET_SOCK_BLOCK_SEL_NO_BLOCK, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_err;
  }

  (void)NetSock_OptSet(p_conn->SockId,                          // Set NO DELAY option.
                       NET_SOCK_PROTOCOL_TCP,
                       NET_SOCK_OPT_TCP_NO_DELAY,
                       (void *)&flag,
                       sizeof(flag),
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_err;
  }

  //                                                               Set sock conn inactivity timeout.
  (void)NetSock_OptSet(p_conn->SockId,
                       NET_SOCK_PROTOCOL_TCP,
                       NET_SOCK_OPT_TCP_KEEP_IDLE,
                       (void *)&p_conn->InactivityTimeout_s,
                       sizeof(p_conn->InactivityTimeout_s),
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_err;
  }

  goto exit;

end_err:
  {
    RTOS_ERR local_err;

    NetSock_Close(p_conn->SockId, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    p_conn->SockId = NET_SOCK_ID_NONE;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_SockConnClose()
 *
 * @brief    Close socket for MQTTc module.
 *
 * @param    p_conn  Pointer to MQTTc_CONN to close.
 *
 * @param    p_err   Pointer to variable that will receive error code from this function.
 *******************************************************************************************************/
void MQTTc_SockConnClose(MQTTc_CONN *p_conn,
                         RTOS_ERR   *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  NetSock_Close(p_conn->SockId, p_err);
}

/****************************************************************************************************//**
 *                                               MQTTc_SockTx()
 *
 * @brief    Transmit data on given connection's socket.
 *
 * @param    p_conn      Pointer to MQTTc_CONN that needs to transmit.
 *
 * @param    p_buf       Pointer to start of buffer to transmit.
 *
 * @param    buf_len     Length, in bytes, to transmit.
 *
 * @param    p_err       Pointer to variable that will receive error code from this function.
 *******************************************************************************************************/
CPU_INT32U MQTTc_SockTx(MQTTc_CONN *p_conn,
                        CPU_INT08U *p_buf,
                        CPU_INT32U buf_len,
                        RTOS_ERR   *p_err)
{
  NET_SOCK_RTN_CODE ret_val;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);                  // Validate err ptr.

  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  ret_val = NetSock_TxData(p_conn->SockId,
                           (void *)p_buf,
                           buf_len,
                           NET_SOCK_FLAG_TX_NO_BLOCK,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    ret_val = 0u;
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                               MQTTc_SockRx()
 *
 * @brief    Receive data on given connection's socket.
 *
 * @param    p_conn      Pointer to MQTTc_CONN that needs to receive.
 *
 * @param    p_buf       Pointer to start of buffer in which received data will be put.
 *
 * @param    buf_len     Length, in bytes, of receive buffer.
 *
 * @param    p_err       Pointer to variable that will receive error code from this function.
 *
 * @return   Number of bytes received, if NO error(s),
 *           0,                        otherwise.
 *******************************************************************************************************/
CPU_INT32U MQTTc_SockRx(MQTTc_CONN *p_conn,
                        CPU_INT08U *p_buf,
                        CPU_INT32U buf_len,
                        RTOS_ERR   *p_err)
{
  NET_SOCK_RTN_CODE ret_val;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);                  // Validate err ptr.

  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  ret_val = NetSock_RxData(p_conn->SockId,
                           p_buf,
                           buf_len,
                           NET_SOCK_FLAG_NONE,
                           p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    ret_val = 0u;
  }
  return (ret_val);
}

/****************************************************************************************************//**
 *                                           MQTTc_SockSelDescSet()
 *
 * @brief    Set select descriptor type for given connection.
 *
 * @param    p_conn          Pointer to MQTTc_CONN for which to set its descriptor.
 *
 * @param    sel_desc_type   Select descriptor type to set.
 *******************************************************************************************************/
void MQTTc_SockSelDescSet(MQTTc_CONN          *p_conn,
                          MQTTc_SEL_DESC_TYPE sel_desc_type)
{
  RTOS_ERR local_err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  switch (sel_desc_type) {
    case MQTTc_SEL_DESC_TYPE_RD:
      DEF_BIT_SET(p_conn->SockSelFlags, MQTTc_SOCK_SEL_FLAG_DESC_RD);
      break;

    case MQTTc_SEL_DESC_TYPE_WR:
      DEF_BIT_SET(p_conn->SockSelFlags, MQTTc_SOCK_SEL_FLAG_DESC_WR);
      break;

    case MQTTc_SEL_DESC_TYPE_ERR:
    default:
      DEF_BIT_SET(p_conn->SockSelFlags, MQTTc_SOCK_SEL_FLAG_DESC_ERR);
      break;
  }
  CORE_EXIT_ATOMIC();

  NetSock_SelAbort(p_conn->SockId, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_SockSelDescClr()
 *
 * @brief    Clear select descriptor type for given connection.
 *
 * @param    p_conn          Pointer to MQTTc_CONN for which to clear its descriptor.
 *
 * @param    sel_desc_type   Select descriptor type to clear.
 *******************************************************************************************************/
void MQTTc_SockSelDescClr(MQTTc_CONN          *p_conn,
                          MQTTc_SEL_DESC_TYPE sel_desc_type)
{
  switch (sel_desc_type) {
    case MQTTc_SEL_DESC_TYPE_RD:
      DEF_BIT_CLR(p_conn->SockSelFlags, (CPU_INT08U)MQTTc_SOCK_SEL_FLAG_DESC_RD);
      break;

    case MQTTc_SEL_DESC_TYPE_WR:
      DEF_BIT_CLR(p_conn->SockSelFlags, (CPU_INT08U)MQTTc_SOCK_SEL_FLAG_DESC_WR);
      break;

    case MQTTc_SEL_DESC_TYPE_ERR:
    default:
      DEF_BIT_CLR(p_conn->SockSelFlags, (CPU_INT08U)MQTTc_SOCK_SEL_FLAG_DESC_ERR);
      break;
  }

  return;
}

/****************************************************************************************************//**
 *                                           MQTTc_SockSelDescProc()
 *
 * @brief    Process select descriptor type for given connection.
 *
 * @param    p_conn          Pointer to MQTTc_CONN on which to process select descriptor.
 *
 * @param    sel_desc_type   Select descriptor type to process.
 *
 * @return   DEF_YES, if given descriptor is set,
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN MQTTc_SockSelDescProc(MQTTc_CONN          *p_conn,
                                  MQTTc_SEL_DESC_TYPE sel_desc_type)
{
  NET_SOCK_DESC *p_net_sock_desc;
  CPU_BOOLEAN   ret_val = DEF_NO;

  switch (sel_desc_type) {
    case MQTTc_SEL_DESC_TYPE_RD:
      p_net_sock_desc = &MQTTc_NetSockDescRd;
      break;

    case MQTTc_SEL_DESC_TYPE_WR:
      p_net_sock_desc = &MQTTc_NetSockDescWr;
      break;

    case MQTTc_SEL_DESC_TYPE_ERR:
    default:
      p_net_sock_desc = &MQTTc_NetSockDescErr;
      break;
  }

  ret_val = NET_SOCK_DESC_IS_SET(p_conn->SockId, p_net_sock_desc);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                               MQTTc_SockSel()
 *
 * @brief    Execute select operation for selected connections.
 *
 * @param    p_head_conn     Pointer to head of MQTTc Connection object list.
 *
 * @param    p_err           Pointer to variable that will receive error code from this function.
 *******************************************************************************************************/
void MQTTc_SockSel(MQTTc_CONN *p_head_conn,
                   RTOS_ERR   *p_err)
{
  MQTTc_CONN  *p_conn_iter;
  CPU_BOOLEAN must_call_sel = DEF_NO;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );                   // Validate err ptr.

  NET_SOCK_DESC_INIT(&MQTTc_NetSockDescRd);
  NET_SOCK_DESC_INIT(&MQTTc_NetSockDescWr);
  NET_SOCK_DESC_INIT(&MQTTc_NetSockDescErr);

  p_conn_iter = p_head_conn;
  while (p_conn_iter != DEF_NULL) {
    if ((p_conn_iter->SockId != NET_SOCK_ID_NONE)
        && (DEF_BIT_IS_SET_ANY(p_conn_iter->SockSelFlags, MQTTc_SOCK_SEL_FLAG_DESC_MSK) == DEF_YES)) {
      must_call_sel = DEF_YES;
      if (DEF_BIT_IS_SET(p_conn_iter->SockSelFlags, MQTTc_SOCK_SEL_FLAG_DESC_RD) == DEF_YES) {
        NET_SOCK_DESC_SET(p_conn_iter->SockId, &MQTTc_NetSockDescRd);
      }
      if (DEF_BIT_IS_SET(p_conn_iter->SockSelFlags, MQTTc_SOCK_SEL_FLAG_DESC_WR) == DEF_YES) {
        NET_SOCK_DESC_SET(p_conn_iter->SockId, &MQTTc_NetSockDescWr);
      }
      if (DEF_BIT_IS_SET(p_conn_iter->SockSelFlags, MQTTc_SOCK_SEL_FLAG_DESC_ERR) == DEF_YES) {
        NET_SOCK_DESC_SET(p_conn_iter->SockId, &MQTTc_NetSockDescErr);
      }
    }
    p_conn_iter = p_conn_iter->NextPtr;
  }

  if (must_call_sel == DEF_YES) {
    (void)NetSock_Sel(NET_SOCK_NBR_SOCK,
                      &MQTTc_NetSockDescRd,
                      &MQTTc_NetSockDescWr,
                      &MQTTc_NetSockDescErr,
                      DEF_NULL,
                      p_err);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }

  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_MQTT_CLIENT_AVAIL
