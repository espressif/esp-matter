/***************************************************************************//**
 * @file
 * @brief Network - HTTP Client Socket Module
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

#include  <common/include/lib_def.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  "http_client_priv.h"

#include  <net/include/http_client.h>
#include  <net/include/http.h>
#include  <net/include/net_app.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_tcp.h>

#ifdef  HTTPc_WEBSOCK_MODULE_EN
#include  "http_client_websock_priv.h"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

#define  HTTPc_SOCK_TIMEOUT_CONNECT_MS_DFLT               2000u
#define  HTTPc_SOCK_TIMEOUT_CONNECT_MS_MAX               10000u
#define  HTTPc_SOCK_TIMEOUT_CONNECT_MS_MIN                1000u

#define  HTTPc_SOCK_TIMEOUT_INACTIVITY_S_DFLT               60u
#define  HTTPc_SOCK_TIMEOUT_INACTIVITY_S_MAX               255u
#define  HTTPc_SOCK_TIMEOUT_INACTIVITY_S_MIN                 1u

#define  HTTPc_SOCK_SEL_TIMEOUT_MS                           1u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         HTTPcSock_Connect()
 *
 * @brief    - (1) Open Socket.
 *           - (2) Connect to Server.
 *           - (3) Set Inactivity Connection Timeout.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void HTTPcSock_Connect(HTTPc_CONN *p_conn,
                       RTOS_ERR   *p_err)
{
  NET_APP_SOCK_SECURE_CFG *p_secure_cfg;
  NET_CONN_ID             conn_id_tcp;;
  CPU_BOOLEAN             no_delay;

#ifdef NET_SECURE_MODULE_EN
  p_secure_cfg = p_conn->SockSecureCfgPtr;
  if (p_secure_cfg != DEF_NULL) {
    RTOS_ASSERT_DBG_ERR_SET((p_secure_cfg->TrustCallback != DEF_NULL), *p_err, RTOS_ERR_INVALID_CFG,; );
    if (p_secure_cfg->CommonName == DEF_NULL) {
      p_secure_cfg->CommonName = p_conn->HostNamePtr;
    }
    if (p_conn->ServerPort == HTTP_DFLT_PORT_NBR) {
      p_conn->ServerPort = HTTP_DFLT_PORT_NBR_SECURE;
    }
  }
#else
  p_secure_cfg = DEF_NULL;
#endif

  RTOS_ASSERT_DBG_ERR_SET((p_conn->ConnectTimeout_ms > 0u), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn->InactivityTimeout_s > 0u), *p_err, RTOS_ERR_INVALID_CFG,; );

  //                                                               ---------------- RESOLVE HOST NAME -----------------
  NetApp_ClientStreamOpenByHostname(&p_conn->SockID,
                                    p_conn->HostNamePtr,
                                    p_conn->ServerPort,
                                    &p_conn->ServerSockAddr,
                                    p_secure_cfg,
                                    p_conn->ConnectTimeout_ms,
                                    p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
    case RTOS_ERR_NET_OP_IN_PROGRESS:
      break;

    default:
      p_conn->SockID = NET_SOCK_ID_NONE;
      goto exit;
  }
  //                                                               ---------------- CFG SOCK BLOCK OPT ----------------
  (void)NetSock_CfgBlock(p_conn->SockID, NET_SOCK_BLOCK_SEL_NO_BLOCK, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  //                                                               ----- SET SOCKET CONNECTION INACTIVITY TIMEOUT -----
  (void)NetSock_OptSet(p_conn->SockID,
                       NET_SOCK_PROTOCOL_TCP,
                       NET_SOCK_OPT_TCP_KEEP_IDLE,
                       (void *)&p_conn->InactivityTimeout_s,
                       sizeof(p_conn->InactivityTimeout_s),
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  //                                                               --------------- SET NO DELAY FEATURE ---------------
  no_delay = DEF_FALSE;                                         // Set to false to disable the naggle algorithm.
  (void)NetSock_OptSet(p_conn->SockID,
                       NET_SOCK_PROTOCOL_TCP,
                       NET_SOCK_OPT_TCP_NO_DELAY,
                       (void *)&no_delay,
                       sizeof(no_delay),
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }
  //                                                               -------------- SET TCP CONNECTION MSL --------------
  conn_id_tcp = NetSock_GetConnTransportID(p_conn->SockID, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  (void)NetTCP_ConnCfgMSL_Timeout(conn_id_tcp, 0u, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  goto exit;

exit_close:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetSock_Close(p_conn->SockID, &local_err);
    p_conn->SockID = NET_SOCK_ID_NONE;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                        HTTPcSock_ConnDataRx()
 *
 * @brief    Receive data from the network and update connection.
 *
 * @param    p_conn      Pointer to current HTTPc Connection.
 *
 * @param    max_len     Specify the maximum length to get from the socket.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,     if data received successfully.
 *           DEF_FAIL,   otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPcSock_ConnDataRx(HTTPc_CONN *p_conn,
                                 CPU_INT32U max_len,
                                 RTOS_ERR   *p_err)
{
  CPU_CHAR          *p_buf;
  NET_SOCK_ADDR_LEN addr_len_server;
  CPU_INT16S        rx_len;
  CPU_INT32U        buf_len;
  CPU_BOOLEAN       rtn_val;
  RTOS_ERR          local_err;

  if (p_conn->RxDataLenRem > 0) {                               // If data is still present in the rx buf.
                                                                // Move rem data to the beginning of the rx buf.
    Mem_Copy(p_conn->BufPtr, p_conn->RxBufPtr, p_conn->RxDataLenRem);
  }
  p_conn->RxBufPtr = p_conn->BufPtr;

  p_buf = p_conn->BufPtr + p_conn->RxDataLenRem;
  buf_len = p_conn->BufLen - (p_conn->RxDataLenRem + p_conn->TxDataLen);

  if (buf_len == 0) {
    rtn_val = DEF_OK;                                           // HTTP buffer already full of data rx.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }
  if (max_len != 0) {
    buf_len = DEF_MIN(buf_len, max_len);
  }
  addr_len_server = sizeof(p_conn->ServerSockAddr);

  //                                                               ------------------ RECEIVED DATA -------------------
  rx_len = NetSock_RxDataFrom(p_conn->SockID,
                              (void *)p_buf,
                              buf_len,
                              NET_SOCK_FLAG_NO_BLOCK,
                              &p_conn->ServerSockAddr,
                              &addr_len_server,
                              DEF_NULL,
                              DEF_NULL,
                              DEF_NULL,
                              p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:                                         // Data received.
      p_conn->RxDataLenRem += rx_len;
      break;

    case RTOS_ERR_WOULD_BLOCK:                                  // Transitory rx err(s).
    case RTOS_ERR_TIMEOUT:
      rtn_val = DEF_FAIL;
      goto exit;

    case RTOS_ERR_NET_CONN_CLOSED_FAULT:                        // Conn closed by peer.
    case RTOS_ERR_NET_CONN_CLOSE_RX:
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetSock_Close(p_conn->SockID, &local_err);
      rtn_val = DEF_FAIL;
      goto exit;

    default:                                                    // Fatal err.
      rtn_val = DEF_FAIL;
      goto exit;
  }

  rtn_val = DEF_OK;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                        HTTPcSock_ConnDataTx()
 *
 * @brief    Transmit data on the network and update HTTPc connection parameters.
 *
 * @param    p_conn  Pointer to the current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if entire data transmitted successfully.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPcSock_ConnDataTx(HTTPc_CONN *p_conn,
                                 RTOS_ERR   *p_err)
{
  NET_SOCK_ADDR_LEN addr_len_server;
  CPU_INT16U        tx_len;
  CPU_BOOLEAN       rtn_val;
  RTOS_ERR          local_err;

  addr_len_server = sizeof(p_conn->ServerSockAddr);

  tx_len = NetSock_TxDataTo(p_conn->SockID,
                            p_conn->TxDataPtr,
                            p_conn->TxDataLen,
                            NET_SOCK_FLAG_NO_BLOCK,
                            &p_conn->ServerSockAddr,
                            addr_len_server,
                            p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:                                         // Data transmitted.
      p_conn->TxDataLen -= tx_len;
      if (p_conn->TxDataLen > 0u) {                             // If data is not entirely transmitted.
        p_conn->TxDataPtr = (CPU_CHAR *)p_conn->TxDataPtr + tx_len;
        rtn_val = DEF_FAIL;
        goto exit;
      }
      break;

    case RTOS_ERR_POOL_EMPTY:                                   // Transitory Errors
    case RTOS_ERR_NET_IF_LINK_DOWN:
    case RTOS_ERR_TIMEOUT:
    case RTOS_ERR_WOULD_BLOCK:
      rtn_val = DEF_FAIL;
      goto exit;

    case RTOS_ERR_NET_CONN_CLOSE_RX:                            // Conn closed by peer.
    case RTOS_ERR_NET_CONN_CLOSED_FAULT:
      rtn_val = DEF_FAIL;
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetSock_Close(p_conn->SockID, &local_err);
      goto exit;

    default:                                                    // Fatal err.
      rtn_val = DEF_FAIL;
      goto exit;
  }

  rtn_val = DEF_OK;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                           HTTPcSock_Close()
 *
 * @brief    Close Socket Connection with Server.
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void HTTPcSock_Close(HTTPc_CONN *p_conn,
                     RTOS_ERR   *p_err)
{
  CPU_BOOLEAN done = DEF_NO;

  while (done != DEF_YES) {
    NetSock_Close(p_conn->SockID, p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
      case RTOS_ERR_NET_OP_IN_PROGRESS:
        done = DEF_YES;
        p_conn->SockID = NET_SOCK_ID_NONE;
        break;

      default:
        done = DEF_YES;
        goto exit;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;
}

/****************************************************************************************************//**
 *                                            HTTPcSock_Sel()
 *
 * @brief    - (1) Check all HTTPc Connection sockets for available resources &/or operations.
 *           - (2) Set each HTTPc Connection descriptors.
 *
 * @param    p_conn  Pointer to fist item in Connection list or Single Connection pointer.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void HTTPcSock_Sel(HTTPc_CONN *p_conn,
                   RTOS_ERR   *p_err)
{
  HTTPc_CONN       *p_conn_tmp;
  NET_SOCK_TIMEOUT sock_timeout;
  NET_SOCK_TIMEOUT *p_sock_timeout;
  NET_SOCK_DESC    sock_desc_rd;
  NET_SOCK_DESC    sock_desc_wr;
  NET_SOCK_DESC    sock_desc_err;
  NET_SOCK_QTY     sock_nbr_max;
  NET_SOCK_QTY     sock_nbr_rdy;
  CPU_INT08U       state_family;
  CPU_BOOLEAN      rx_data;
#ifdef  HTTPc_WEBSOCK_MODULE_EN
  HTTPc_WEBSOCK *p_ws;
#endif

  //                                                               ---------------- PREPARE SOCK DESC -----------------
  NET_SOCK_DESC_INIT(&sock_desc_rd);
  NET_SOCK_DESC_INIT(&sock_desc_wr);
  NET_SOCK_DESC_INIT(&sock_desc_err);

  sock_nbr_max = 0;
  p_conn_tmp = p_conn;
  p_sock_timeout = DEF_NULL;
  sock_timeout.timeout_sec = 0;
  sock_timeout.timeout_us = HTTPc_SOCK_SEL_TIMEOUT_MS * DEF_TIME_NBR_uS_PER_SEC / DEF_TIME_NBR_mS_PER_SEC;

  while (p_conn_tmp != DEF_NULL) {
    if (p_conn_tmp->SockID != NET_SOCK_ID_NONE) {
      state_family = HTTPc_CONN_STATE_FAMILY_MASK & p_conn_tmp->State;
      switch (state_family) {
        case HTTPc_CONN_STATE_FLOW_FAMILY:
          p_sock_timeout = &sock_timeout;
          NET_SOCK_DESC_SET(p_conn_tmp->SockID, &sock_desc_err);
          break;

        case HTTPc_CONN_STATE_REQ_FAMILY:
          NET_SOCK_DESC_SET(p_conn_tmp->SockID, &sock_desc_wr);
          NET_SOCK_DESC_SET(p_conn_tmp->SockID, &sock_desc_err);
          break;

        case HTTPc_CONN_STATE_RESP_FAMILY:
          rx_data = DEF_BIT_IS_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_RX_MORE_DATA);
          if (rx_data == DEF_NO) {
            p_sock_timeout = &sock_timeout;
          }

          NET_SOCK_DESC_SET(p_conn_tmp->SockID, &sock_desc_rd);
          NET_SOCK_DESC_SET(p_conn_tmp->SockID, &sock_desc_err);
          break;
#ifdef  HTTPc_WEBSOCK_MODULE_EN
        case HTTPc_CONN_STATE_WEBSOCK_FAMILY:
          p_ws = p_conn_tmp->WebSockPtr;
          if (p_ws->TxMsgListHeadPtr != DEF_NULL) {
            NET_SOCK_DESC_SET(p_conn_tmp->SockID, &sock_desc_wr);
          }

          rx_data = DEF_BIT_IS_SET(p_conn->RespFlags, HTTPc_FLAG_RESP_RX_MORE_DATA);
          if (rx_data == DEF_NO) {
            p_sock_timeout = &sock_timeout;
          }

          NET_SOCK_DESC_SET(p_conn_tmp->SockID, &sock_desc_rd);
          NET_SOCK_DESC_SET(p_conn_tmp->SockID, &sock_desc_err);
          break;
#endif
        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
          goto exit;
      }
      p_conn_tmp->SockFlags = HTTPc_FLAG_NONE;
      if (sock_nbr_max <= p_conn_tmp->SockID) {                 // Update highest sock nbr.
        sock_nbr_max = p_conn_tmp->SockID + 1;
      }
    }
    p_conn_tmp = p_conn_tmp->NextPtr;
  }

  if (sock_nbr_max == 0u) {
    p_sock_timeout = &sock_timeout;
  }
  //                                                               -------------------- SOCK SEL ----------------------
  sock_nbr_rdy = NetSock_Sel(sock_nbr_max,
                             &sock_desc_rd,
                             &sock_desc_wr,
                             &sock_desc_err,
                             p_sock_timeout,
                             p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
    case RTOS_ERR_TIMEOUT:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    default:
      goto exit;
  }

  if (sock_nbr_rdy > 0) {
    p_conn_tmp = p_conn;
    while (p_conn_tmp != DEF_NULL) {
      if (p_conn_tmp->SockID != NET_SOCK_ID_NONE) {
        state_family = HTTPc_CONN_STATE_FAMILY_MASK & p_conn_tmp->State;
        switch (state_family) {
          case HTTPc_CONN_STATE_FLOW_FAMILY:
            if (NET_SOCK_DESC_IS_SET(p_conn_tmp->SockID, &sock_desc_err)) {
              DEF_BIT_SET(p_conn_tmp->SockFlags, HTTPc_FLAG_SOCK_RDY_ERR);
            }
            break;

          case HTTPc_CONN_STATE_REQ_FAMILY:
            if (NET_SOCK_DESC_IS_SET(p_conn_tmp->SockID, &sock_desc_wr)) {
              DEF_BIT_SET(p_conn_tmp->SockFlags, HTTPc_FLAG_SOCK_RDY_WR);
            }
            if (NET_SOCK_DESC_IS_SET(p_conn_tmp->SockID, &sock_desc_err)) {
              DEF_BIT_SET(p_conn_tmp->SockFlags, HTTPc_FLAG_SOCK_RDY_ERR);
            }
            break;

          case HTTPc_CONN_STATE_RESP_FAMILY:
            if (NET_SOCK_DESC_IS_SET(p_conn_tmp->SockID, &sock_desc_rd)) {
              DEF_BIT_SET(p_conn_tmp->SockFlags, HTTPc_FLAG_SOCK_RDY_RD);
            }
            if (NET_SOCK_DESC_IS_SET(p_conn_tmp->SockID, &sock_desc_err)) {
              DEF_BIT_SET(p_conn_tmp->SockFlags, HTTPc_FLAG_SOCK_RDY_ERR);
            }
            break;
#ifdef  HTTPc_WEBSOCK_MODULE_EN
          case HTTPc_CONN_STATE_WEBSOCK_FAMILY:
            if (NET_SOCK_DESC_IS_SET(p_conn_tmp->SockID, &sock_desc_wr)) {
              DEF_BIT_SET(p_conn_tmp->SockFlags, HTTPc_FLAG_SOCK_RDY_WR);
            }
            if (NET_SOCK_DESC_IS_SET(p_conn_tmp->SockID, &sock_desc_rd)) {
              DEF_BIT_SET(p_conn_tmp->SockFlags, HTTPc_FLAG_SOCK_RDY_RD);
            }
            if (NET_SOCK_DESC_IS_SET(p_conn_tmp->SockID, &sock_desc_err)) {
              DEF_BIT_SET(p_conn_tmp->SockFlags, HTTPc_FLAG_SOCK_RDY_ERR);
            }
            break;
#endif
          default:
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
            goto exit;
        }
      }
      p_conn_tmp = p_conn_tmp->NextPtr;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;
}

/****************************************************************************************************//**
 *                                        HTTPcSock_IsRxClosed()
 *
 * @brief    Check if the TCP Connection is half closed (FIN flag was received from Server).
 *
 * @param    p_conn  Pointer to current HTTPc Connection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if TCP connection is half-closed.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPcSock_IsRxClosed(HTTPc_CONN *p_conn,
                                 RTOS_ERR   *p_err)
{
  NET_TCP_CONN_ID    conn_id_tcp;
  NET_TCP_CONN_STATE state;
  CPU_BOOLEAN        result = DEF_NO;

  conn_id_tcp = NetSock_GetConnTransportID(p_conn->SockID, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    result = DEF_YES;
    goto exit;
  }

  state = NetTCP_ConnStateGet(conn_id_tcp);

  if (state == NET_TCP_CONN_STATE_CLOSE_WAIT) {
    result = DEF_YES;
  }

exit:
  return (result);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_CLIENT_AVAIL
