/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Socket Module
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

#include  "http_server_priv.h"

#include  <net/include/net_ipv4.h>
#include  <net/include/net_ipv6.h>
#include  <net/include/net_tcp.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_app.h>

#include  <net/source/tcpip/net_type_priv.h>

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (NET, HTTP)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_NET

#define  HTTPs_SOCK_SEL_TIMEOUT_MS                          1u

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void HTTPsSock_ConnAccept(HTTPs_INSTANCE *p_instance,
                                 NET_SOCK_ID    sock_listen_id);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           HTTPsSock_ListenInit()
 *
 * @brief    (1) Initialize listen socket:
 *               - (a) Open socket.
 *               - (b) Configure socket secure option.
 *               - (c) Bind socket.
 *               - (d) Configure socket for listen.
 *               - (e) Configure socket blocking option.
 *
 * @param    p_cfg   Pointer to the instance configuration structure.
 *
 * @param    -----   Argument validated in HTTPs_InstanceStart().
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Socket descriptor/handle identifier for the listen, if NO error(s).
 *           NET_SOCK_BSD_ERR_OPEN,  otherwise.
 *******************************************************************************************************/
NET_SOCK_ID HTTPsSock_ListenInit(const HTTPs_CFG          *p_cfg,
                                 NET_SOCK_PROTOCOL_FAMILY family,
                                 RTOS_ERR                 *p_err)
{
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR net_ipv4_addr_any;
#endif
  NET_SOCK_ID       sock_id;
  NET_SOCK_ADDR     sock_addr;
  NET_SOCK_ADDR_LEN sock_addr_len;
  CPU_INT08U        *p_addr = DEF_NULL;
  NET_SOCK_ADDR_LEN addr_len = 0u;
  RTOS_ERR          local_err;

  //                                                               -------------------- OPEN SOCK ---------------------
  sock_id = NetSock_Open(family,
                         NET_SOCK_TYPE_STREAM,
                         NET_SOCK_PROTOCOL_TCP,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (NET_SOCK_BSD_ERR_OPEN);
  }

  //                                                               --------------- CFG SOCK SECURE OPT-----------------
  if (p_cfg->SecurePtr != DEF_NULL) {
#ifndef NET_SECURE_MODULE_EN                                    // Set or clear socket secure mode.
    (void)NetSock_Close(sock_id, &local_err);
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, NET_SOCK_BSD_ERR_OPEN);
#else

    (void)NetSock_CfgSecure(sock_id,
                            DEF_YES,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      (void)NetSock_Close(sock_id, &local_err);
      return (NET_SOCK_BSD_ERR_OPEN);
    }

    (void)NetSock_CfgSecureServerCertKeyInstall(sock_id,
                                                p_cfg->SecurePtr->CertPtr,
                                                p_cfg->SecurePtr->CertLen,
                                                p_cfg->SecurePtr->KeyPtr,
                                                p_cfg->SecurePtr->KeyLen,
                                                p_cfg->SecurePtr->Fmt,
                                                p_cfg->SecurePtr->CertChain,
                                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      (void)NetSock_Close(sock_id, &local_err);
      return (NET_SOCK_BSD_ERR_OPEN);
    }
#endif
  }

  //                                                               -------------------- BIND SOCK ---------------------
#if !defined(NET_IPv4_MODULE_EN)
  if (family == NET_SOCK_ADDR_FAMILY_IP_V4) {
    (void)NetSock_Close(sock_id, &local_err);
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, NET_SOCK_BSD_ERR_OPEN);
  }
#endif

#if !defined(NET_IPv6_MODULE_EN)
  if (family == NET_SOCK_ADDR_FAMILY_IP_V6) {
    (void)NetSock_Close(sock_id, &local_err);
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, NET_SOCK_BSD_ERR_OPEN);
  }
#endif

  switch (family) {
    case NET_SOCK_ADDR_FAMILY_IP_V4:
#ifdef  NET_IPv4_MODULE_EN
      net_ipv4_addr_any = NET_IPv4_ADDR_ANY;
      p_addr = (CPU_INT08U *)&net_ipv4_addr_any;
      addr_len = NET_IPv4_ADDR_SIZE;
      sock_addr_len = sizeof(sock_addr);
      break;
#endif

    case NET_SOCK_ADDR_FAMILY_IP_V6:
#ifdef  NET_IPv6_MODULE_EN
      p_addr = (CPU_INT08U *)&NetIPv6_AddrAny;
      addr_len = NET_IPv6_ADDR_SIZE;
      sock_addr_len = sizeof(sock_addr);
      break;
#endif

    default:
      (void)NetSock_Close(sock_id, &local_err);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_SOCK_BSD_ERR_OPEN);
  }

  NetApp_SetSockAddr(&sock_addr,
                     family,
                     p_cfg->Port,
                     p_addr,
                     addr_len,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    (void)NetSock_Close(sock_id, &local_err);
    return (NET_SOCK_BSD_ERR_OPEN);
  }

  (void)NetSock_Bind(sock_id,
                     &sock_addr,
                     sock_addr_len,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    (void)NetSock_Close(sock_id, &local_err);
    return (NET_SOCK_BSD_ERR_OPEN);
  }

  //                                                               ------------------- LISTEN SOCK --------------------
  (void)NetSock_Listen(sock_id, p_cfg->ConnNbrMax, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    (void)NetSock_Close(sock_id, &local_err);
    return (NET_SOCK_BSD_ERR_OPEN);
  }

  //                                                               ---------------- CFG SOCK BLOCK OPT ----------------
  (void)NetSock_CfgBlock(sock_id, NET_SOCK_BLOCK_SEL_NO_BLOCK, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    (void)NetSock_Close(sock_id, &local_err);
    return (NET_SOCK_BSD_ERR_OPEN);
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (sock_id);
}

/****************************************************************************************************//**
 *                                           HTTPsSock_ListenClose()
 *
 * @brief    (1) Close listen socket.
 *
 * @param    p_instance      Pointer to the instance.
 *
 * @param    ----------      Argument validated in HTTPs_InstanceStart().
 *
 * @param    sock_listen_id  Socket ID for listen socket.
 *******************************************************************************************************/
void HTTPsSock_ListenClose(HTTPs_INSTANCE *p_instance,
                           NET_SOCK_ID    sock_listen_id)
{
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
  HTTPs_INSTANCE_ERRS  *p_ctr_err = DEF_NULL;
  CPU_BOOLEAN          done;
  RTOS_ERR             local_err;

#if  ((HTTPs_CFG_CTR_STAT_EN == DEF_DISABLED) \
  && (HTTPs_CFG_CTR_ERR_EN == DEF_DISABLED))
  PP_UNUSED_PARAM(p_instance);
#endif
  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);
  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  done = DEF_NO;
  while (done != DEF_YES) {
    (void)NetSock_Close(sock_listen_id, &local_err);
    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:
      case RTOS_ERR_NET_OP_IN_PROGRESS:
        done = DEF_YES;
        HTTPs_STATS_INC(p_ctr_stats->Sock_StatListenCloseCtr);
        break;

      default:
        done = DEF_YES;
        HTTPs_ERR_INC(p_ctr_err->Sock_ErrListenCloseCtr);
        break;
    }
  }
}

/****************************************************************************************************//**
 *                                           HTTPsSock_ConnSel()
 *
 * @brief    (1) Update connections that are ready to be processed.
 *               - (a) Prepare socket descriptor
 *               - (b) Socket select
 *               - (c) Update connection states
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *
 * @param    accept      DEF_YES, if server needs to check for incoming connections to accept.
 *                       DEF_NO, otherwise
 *
 * @return   Number of connection ready to be processed.
 *******************************************************************************************************/
NET_SOCK_QTY HTTPsSock_ConnSel(HTTPs_INSTANCE *p_instance,
                               CPU_BOOLEAN    accept)
{
  const HTTPs_CFG     *p_cfg;
  HTTPs_CONN          *p_conn;
  HTTPs_INSTANCE_ERRS *p_ctr_err = DEF_NULL;
  NET_SOCK_TIMEOUT    *p_sock_timeout;
  NET_SOCK_TIMEOUT    sock_timeout;
  NET_SOCK_QTY        sock_nbr_rdy;
  NET_SOCK_DESC       sock_desc_rd;
  NET_SOCK_DESC       sock_desc_wr;
  NET_SOCK_DESC       sock_desc_err;
  NET_SOCK_QTY        sock_nbr_max;
  NET_SOCK_RTN_CODE   sel_rtn_code;
  CPU_BOOLEAN         child_present;
  RTOS_ERR            local_err;

  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);
  p_cfg = p_instance->CfgPtr;

  child_present = DEF_NO;

  //                                                               ---------------- PREPARE SOCK DESC -----------------
  NET_SOCK_DESC_INIT(&sock_desc_rd);
  NET_SOCK_DESC_INIT(&sock_desc_wr);
  NET_SOCK_DESC_INIT(&sock_desc_err);

  sock_nbr_rdy = 0;
  sock_nbr_max = 0;

  if (accept == DEF_YES) {
    switch (p_cfg->SockSel) {
      case HTTPs_SOCK_SEL_IPv4:
#ifdef NET_IPv4_MODULE_EN
        NET_SOCK_DESC_SET(p_instance->SockListenID_IPv4, &sock_desc_rd);
        sock_nbr_max = p_instance->SockListenID_IPv4 + 1;
#endif
        break;

      case HTTPs_SOCK_SEL_IPv6:
#ifdef NET_IPv6_MODULE_EN
        NET_SOCK_DESC_SET(p_instance->SockListenID_IPv6, &sock_desc_rd);
        sock_nbr_max = p_instance->SockListenID_IPv6 + 1;
#endif
        break;

      case HTTPs_SOCK_SEL_IPv4_IPv6:
#ifdef NET_IPv4_MODULE_EN
        NET_SOCK_DESC_SET(p_instance->SockListenID_IPv4, &sock_desc_rd);
        sock_nbr_max = p_instance->SockListenID_IPv4 + 1;
#endif
#ifdef NET_IPv6_MODULE_EN
        NET_SOCK_DESC_SET(p_instance->SockListenID_IPv6, &sock_desc_rd);
        if (sock_nbr_max < p_instance->SockListenID_IPv6) {
          sock_nbr_max = p_instance->SockListenID_IPv6 + 1;
        } else if ((sock_nbr_max == p_instance->SockListenID_IPv6)
                   && (p_instance->SockListenID_IPv6 == 0)                            ) {
          sock_nbr_max = p_instance->SockListenID_IPv6 + 1;
        }
#endif
        break;

      default:
        break;
    }
  }

  p_conn = p_instance->ConnFirstPtr;
  while (p_conn != DEF_NULL) {
    child_present = DEF_YES;
    switch (p_conn->SockState) {
      case HTTPs_SOCK_STATE_RX:                                 // Conn rdy to receive data.
        NET_SOCK_DESC_SET(p_conn->SockID, &sock_desc_rd);
        NET_SOCK_DESC_SET(p_conn->SockID, &sock_desc_err);
        break;

      case HTTPs_SOCK_STATE_TX:                                 // Conn rdy to tx data.
        NET_SOCK_DESC_SET(p_conn->SockID, &sock_desc_wr);
        NET_SOCK_DESC_SET(p_conn->SockID, &sock_desc_err);
        break;

      case HTTPs_SOCK_STATE_ERR:                                // Conn is wainting for sock err.
      case HTTPs_SOCK_STATE_CLOSE:
      case HTTPs_SOCK_STATE_NONE:
        NET_SOCK_DESC_SET(p_conn->SockID, &sock_desc_err);
        sock_nbr_rdy++;                                         // Socket close completed in HTTPsConn_Close().
        break;

      default:
        break;
    }

    if (sock_nbr_max <= p_conn->SockID) {                       // Update highest sock nbr.
      sock_nbr_max = p_conn->SockID + 1;
    } else if ((sock_nbr_max == p_conn->SockID)
               && (p_conn->SockID == 0)             ) {
      sock_nbr_max = p_conn->SockID + 1;
    }

    p_conn = p_conn->ConnNextPtr;
  }

  //                                                               -------------------- SOCK SEL ----------------------
  if ((accept == DEF_YES)
      && (child_present == DEF_NO) ) {
    p_sock_timeout = DEF_NULL;
  } else {
    sock_timeout.timeout_sec = 0;
    sock_timeout.timeout_us = HTTPs_SOCK_SEL_TIMEOUT_MS * DEF_TIME_NBR_uS_PER_SEC / DEF_TIME_NBR_mS_PER_SEC;
    p_sock_timeout = &sock_timeout;
  }

  sel_rtn_code = NetSock_Sel(sock_nbr_max,
                             &sock_desc_rd,
                             &sock_desc_wr,
                             &sock_desc_err,
                             p_sock_timeout,
                             &local_err);
  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:
    case RTOS_ERR_TIMEOUT:
      sock_nbr_rdy += sel_rtn_code;
      break;

    default:
      HTTPs_ERR_INC(p_ctr_err->Sock_ErrSelCtr);
      return (sock_nbr_rdy);
  }

  //                                                               -------------- ACCEPT NEW CONNECTIONS --------------
  if (accept == DEF_YES) {
    switch (p_cfg->SockSel) {
      case HTTPs_SOCK_SEL_IPv4:
#ifdef NET_IPv4_MODULE_EN
        if (NET_SOCK_DESC_IS_SET(p_instance->SockListenID_IPv4, &sock_desc_rd)) {
          HTTPsSock_ConnAccept(p_instance, p_instance->SockListenID_IPv4);
        }
#endif
        break;

      case HTTPs_SOCK_SEL_IPv6:
#ifdef NET_IPv6_MODULE_EN
        if (NET_SOCK_DESC_IS_SET(p_instance->SockListenID_IPv6, &sock_desc_rd)) {
          HTTPsSock_ConnAccept(p_instance, p_instance->SockListenID_IPv6);
        }
#endif
        break;

      case HTTPs_SOCK_SEL_IPv4_IPv6:
#ifdef NET_IPv4_MODULE_EN
        if (NET_SOCK_DESC_IS_SET(p_instance->SockListenID_IPv4, &sock_desc_rd)) {
          HTTPsSock_ConnAccept(p_instance, p_instance->SockListenID_IPv4);
        }
#endif
#ifdef NET_IPv6_MODULE_EN
        if (NET_SOCK_DESC_IS_SET(p_instance->SockListenID_IPv6, &sock_desc_rd)) {
          HTTPsSock_ConnAccept(p_instance, p_instance->SockListenID_IPv6);
        }
#endif
        break;

      default:
        break;
    }
  }

  //                                                               ------------ UPDATE CONN SOCK STATE  ---------------
  p_conn = p_instance->ConnFirstPtr;
  while (p_conn != DEF_NULL) {
    switch (p_conn->SockState) {
      case HTTPs_SOCK_STATE_RX:                                 // Conn rdy to receive data.
        if (NET_SOCK_DESC_IS_SET(p_conn->SockID, &sock_desc_rd)) {
          DEF_BIT_SET(p_conn->SockFlags, HTTPs_FLAG_SOCK_RDY_RD);               // Sock rdy.
        }

        if (NET_SOCK_DESC_IS_SET(p_conn->SockID, &sock_desc_err)) {
          DEF_BIT_SET(p_conn->SockFlags, HTTPs_FLAG_SOCK_RDY_ERR);              // Sock Err is pending.
          p_conn->SockState = HTTPs_SOCK_STATE_ERR;
        }
        break;

      case HTTPs_SOCK_STATE_TX:
        if (NET_SOCK_DESC_IS_SET(p_conn->SockID, &sock_desc_wr)) {
          DEF_BIT_SET(p_conn->SockFlags, HTTPs_FLAG_SOCK_RDY_WR);               // Sock rdy.
        }

        if (NET_SOCK_DESC_IS_SET(p_conn->SockID, &sock_desc_err)) {
          DEF_BIT_SET(p_conn->SockFlags, HTTPs_FLAG_SOCK_RDY_ERR);              // Sock Err is pending.
          p_conn->SockState = HTTPs_SOCK_STATE_ERR;
        }
        break;

      case HTTPs_SOCK_STATE_ERR:
      case HTTPs_SOCK_STATE_NONE:                               // Conn needs to be processed.
        if (NET_SOCK_DESC_IS_SET(p_conn->SockID, &sock_desc_err)) {
          DEF_BIT_SET(p_conn->SockFlags, HTTPs_FLAG_SOCK_RDY_ERR);              // Sock Err is pending.
          p_conn->SockState = HTTPs_SOCK_STATE_ERR;
        }
        sock_nbr_rdy++;
        break;

      default:
        break;
    }

    p_conn = p_conn->ConnNextPtr;
  }

  return (sock_nbr_rdy);
}

/****************************************************************************************************//**
 *                                           HTTPsSock_ConnDataRx()
 *
 * @brief    Receive data from the network and update connection.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    ------      Argument validated in HTTPs_InstanceStart().
 *
 * @return   DEF_OK,   if data received successfully.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsSock_ConnDataRx(HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN     *p_conn)
{
  CPU_CHAR             *p_buf;
  NET_SOCK_ADDR_LEN    addr_len_client;
  CPU_INT16U           rx_len;
  CPU_INT32U           buf_len;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
  HTTPs_INSTANCE_ERRS  *p_ctr_err = DEF_NULL;
  CPU_BOOLEAN          rtn_val;
  RTOS_ERR             local_err;

#if  ((HTTPs_CFG_CTR_STAT_EN == DEF_DISABLED) \
  && (HTTPs_CFG_CTR_ERR_EN == DEF_DISABLED))
  PP_UNUSED_PARAM(p_instance);
#endif
  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);
  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  if ((p_conn->RxBufLenRem > 0)
      && (p_conn->RxBufPtr != p_conn->BufPtr)) {                // If data is still present in the rx buf.
                                                                // Move rem data to the beginning of the rx buf.
    Mem_Copy(p_conn->BufPtr, p_conn->RxBufPtr, p_conn->RxBufLenRem);
  }

  p_buf = p_conn->BufPtr + p_conn->RxBufLenRem;
  buf_len = p_conn->BufLen - p_conn->RxBufLenRem;

  if (buf_len == 0) {
    rtn_val = DEF_OK;
    goto exit;
  }

  addr_len_client = sizeof(p_conn->ClientAddr);
  rx_len = (CPU_INT16U)NetSock_RxDataFrom(p_conn->SockID,
                                          (void *)p_buf,
                                          buf_len,
                                          NET_SOCK_FLAG_NO_BLOCK,
                                          &p_conn->ClientAddr,
                                          &addr_len_client,
                                          DEF_NULL,
                                          DEF_NULL,
                                          DEF_NULL,
                                          &local_err);
  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:                                         // Data received.
      p_conn->RxBufPtr = p_conn->BufPtr;
      p_conn->RxBufLenRem += rx_len;
      HTTPs_STATS_OCTET_INC(p_ctr_stats->Sock_StatOctetRxdCtr, rx_len);
      break;

    case RTOS_ERR_WOULD_BLOCK:                                  // Transitory rx err(s).
    case RTOS_ERR_TIMEOUT:
      p_conn->RxBufPtr = p_conn->BufPtr;
      HTTPs_ERR_INC(p_ctr_err->Sock_ErrRxCtr);
      rtn_val = DEF_FAIL;
      goto exit;

    case RTOS_ERR_NET_CONN_CLOSED_FAULT:                        // Conn closed by peer.
    case RTOS_ERR_NET_CONN_CLOSE_RX:
      HTTPs_ERR_INC(p_ctr_err->Sock_ErrRxConnClosedCtr);
      p_conn->SockState = HTTPs_SOCK_STATE_CLOSE;
      rtn_val = DEF_FAIL;
      goto exit;

    default:                                                    // Fatal err.
      HTTPs_ERR_INC(p_ctr_err->Sock_ErrRxFaultCtr);
      p_conn->SockState = HTTPs_SOCK_STATE_ERR;
      rtn_val = DEF_FAIL;
      goto exit;
  }

  rtn_val = DEF_OK;

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                           HTTPsSock_ConnDataTx()
 *
 * @brief    Transmit data on the network and update connection parameters.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    ------      Argument validated in HTTPs_InstanceStart().
 *
 * @return   DEF_OK,   if data transmitted successfully.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsSock_ConnDataTx(HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN     *p_conn)
{
  NET_SOCK_ADDR_LEN    addr_len_client;
  NET_SOCK_RTN_CODE    tx_len;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
  HTTPs_INSTANCE_ERRS  *p_ctr_err = DEF_NULL;
  RTOS_ERR             local_err;

#if  ((HTTPs_CFG_CTR_STAT_EN == DEF_DISABLED) \
  && (HTTPs_CFG_CTR_ERR_EN == DEF_DISABLED))
  PP_UNUSED_PARAM(p_instance);
#endif
  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);
  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  addr_len_client = sizeof(p_conn->ClientAddr);

  tx_len = NetSock_TxDataTo(p_conn->SockID,
                            p_conn->TxBufPtr,
                            p_conn->TxDataLen,
                            NET_SOCK_FLAG_NO_BLOCK,
                            &p_conn->ClientAddr,
                            addr_len_client,
                            &local_err);
  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:
      HTTPs_STATS_OCTET_INC(p_ctr_stats->Sock_StatOctetTxdCtr, tx_len);
      p_conn->TxDataLen -= tx_len;
      if (p_conn->TxDataLen > 0u) {                             // If data is not entirely transmitted.
        p_conn->TxBufPtr = (CPU_CHAR *)p_conn->TxBufPtr + tx_len;
        return (DEF_FAIL);
      } else {
        p_conn->TxBufPtr = p_conn->BufPtr;
      }
      break;

    case RTOS_ERR_POOL_EMPTY:                                   // Transitory Errors
    case RTOS_ERR_NET_IF_LINK_DOWN:
    case RTOS_ERR_TIMEOUT:
    case RTOS_ERR_WOULD_BLOCK:
      return (DEF_FAIL);

    case RTOS_ERR_NET_CONN_CLOSE_RX:                            // Conn closed by peer.
    case RTOS_ERR_NET_CONN_CLOSED_FAULT:
      HTTPs_ERR_INC(p_ctr_err->Sock_ErrTxConnClosedCtr);
      p_conn->SockState = HTTPs_SOCK_STATE_CLOSE;
      return (DEF_FAIL);

    default:                                                    // Fatal err.
      HTTPs_ERR_INC(p_ctr_err->Sock_ErrTxFaultCtr);
      p_conn->SockState = HTTPs_SOCK_STATE_ERR;
      return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           HTTPsSock_ConnClose()
 *
 * @brief    Close connection socket.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    ------      Argument validated in HTTPs_InstanceStart().
 *******************************************************************************************************/
void HTTPsSock_ConnClose(HTTPs_INSTANCE *p_instance,
                         HTTPs_CONN     *p_conn)
{
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
  HTTPs_INSTANCE_ERRS  *p_ctr_err = DEF_NULL;
  CPU_BOOLEAN          done;
  RTOS_ERR             local_err;

#if  ((HTTPs_CFG_CTR_STAT_EN == DEF_DISABLED) \
  && (HTTPs_CFG_CTR_ERR_EN == DEF_DISABLED))
  PP_UNUSED_PARAM(p_instance);
#endif
  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);
  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  done = DEF_NO;
  while (done != DEF_YES) {
    LOG_VRB(("HTTPs - Closing connection, SockID = ", (u)p_conn->SockID));
    (void)NetSock_Close(p_conn->SockID, &local_err);
    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:
      case RTOS_ERR_NET_OP_IN_PROGRESS:
        done = DEF_YES;
        HTTPs_STATS_INC(p_ctr_stats->Conn_StatClosedCtr);
        break;

      default:
        done = DEF_YES;
        HTTPs_ERR_INC(p_ctr_err->Sock_ErrCloseCtr);
        break;
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           HTTPsSock_ConnAccept()
 *
 * @brief    (1) Accept new incoming connection:
 *               - (a) Accept incoming connection.
 *               - (b) Configure TCP   connection MSL timeout.
 *               - (b) Acquire free    connection structure for the new accepted connection request.
 *
 * @param    p_instance      Pointer to the instance.
 *
 * @param    ----------      Argument validated in HTTPs_InstanceStart().
 *
 * @param    sock_listen_id  Socket ID for listen Socket.
 *******************************************************************************************************/
static void HTTPsSock_ConnAccept(HTTPs_INSTANCE *p_instance,
                                 NET_SOCK_ID    sock_listen_id)
{
  HTTPs_CONN           *p_conn;
  HTTPs_INSTANCE_ERRS  *p_ctr_err = DEF_NULL;
  HTTPs_INSTANCE_STATS *p_ctr_stats = DEF_NULL;
  const HTTPs_CFG      *p_cfg = p_instance->CfgPtr;
  NET_SOCK_ID          sock_id;
  NET_SOCK_ADDR        client_addr;
  NET_SOCK_ADDR_LEN    addr_len;
  CPU_BOOLEAN          done;
  NET_TCP_CONN_ID      conn_id_tcp;
  RTOS_ERR             local_err;

  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance);
  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);

  done = DEF_NO;
  while (done != DEF_YES) {
    CPU_BOOLEAN close_sock = DEF_NO;
    CPU_BOOLEAN flag = DEF_DISABLED;

    //                                                             ---------------- ACCEPT INCOMING REQ ---------------
    addr_len = sizeof(client_addr);

    sock_id = NetSock_Accept(sock_listen_id,
                             &client_addr,
                             &addr_len,
                             &local_err);
    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:                                       // New conn accepted.
        LOG_VRB(("HTTPs - Accepted new connection, SockID = ", (u)sock_id));
        HTTPs_STATS_INC(p_ctr_stats->Conn_StatAcceptedCtr);

        (void)NetSock_OptSet(sock_id,                           // Set inactivity timeout.
                             NET_SOCK_PROTOCOL_TCP,
                             NET_SOCK_OPT_TCP_KEEP_IDLE,
                             (void *)&p_cfg->ConnInactivityTimeout_s,
                             sizeof(p_cfg->ConnInactivityTimeout_s),
                             &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          HTTPs_ERR_INC(p_ctr_err->Conn_ErrTmrStartCtr);
          close_sock = DEF_YES;
          break;
        }

        (void)NetSock_OptSet(sock_id,                           // Set NO DELAY option.
                             NET_SOCK_PROTOCOL_TCP,
                             NET_SOCK_OPT_TCP_NO_DELAY,
                             (void *)&flag,
                             sizeof(flag),
                             &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          close_sock = DEF_YES;
          break;
        }

        //                                                         ----------------- CFG TCP CONN MSL -----------------
        conn_id_tcp = NetSock_GetConnTransportID(sock_id, &local_err);

        (void)NetTCP_ConnCfgMSL_Timeout(conn_id_tcp, 0u, &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          close_sock = DEF_YES;
          break;
        }

        //                                                         --------------- ACQUIRE CONN STRUCT ----------------
        p_conn = HTTPsMem_ConnGet(p_instance,
                                  sock_id,
                                  client_addr);
        if (p_conn != DEF_NULL) {
          p_conn->State = HTTPs_CONN_STATE_REQ_INIT;
        } else {                                                // If no free conn struct avail...
                                                                // ... close sock.
          HTTPs_ERR_INC(p_ctr_err->Conn_ErrNoneAvailCtr);
          close_sock = DEF_YES;
          break;
        }
        break;

      case RTOS_ERR_WOULD_BLOCK:
      case RTOS_ERR_TIMEOUT:
      case RTOS_ERR_POOL_EMPTY:
        done = DEF_YES;
        break;

      default:
        HTTPs_ERR_INC(p_ctr_err->Sock_ErrAcceptCtr);
        close_sock = DEF_YES;
        break;
    }

    if ((close_sock == DEF_YES)
        && (sock_id != NET_SOCK_ID_NONE)) {
      (void)NetSock_Close(sock_id, &local_err);
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
