/***************************************************************************//**
 * @file
 * @brief Network Application Programming Interface (Api) Layer
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

#if (defined(RTOS_MODULE_NET_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>

#include  <net/include/net_app.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_util.h>
#include  <net/include/net_cfg_net.h>

#include  "net_app_priv.h"

#ifdef  NET_DNS_CLIENT_MODULE_EN
#include  <net/include/dns_client.h>
#endif

#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetApp_SockOpen()
 *
 * @brief     Opens an application socket, with error handling.
 *
 * @param     protocol_family     Socket protocol family :
 *
 * @param     sock_type           Socket type :
 *                                    - NET_SOCK_TYPE_DATAGRAM
 *                                    - NET_SOCK_TYPE_STREAM
 *
 * @param     protocol            Socket protocol :
 *                                    - NET_SOCK_PROTOCOL_TCP
 *                                    - NET_SOCK_PROTOCOL_UDP
 *
 * @param     retry_max           Maximum number of consecutive socket open retries.
 *
 * @param     time_dly_ms         Transitory socket open delay value, in milliseconds.
 *
 * @param     p_err               Pointer to the variable that will receive one of the following
 *                                error code(s) from this function:
 *                                    - RTOS_ERR_NONE
 *                                    - RTOS_ERR_POOL_EMPTY
 *
 * @return    Socket descriptor/handle identifier, if NO error(s).
 *            NET_SOCK_BSD_ERR_OPEN,               otherwise.
 *
 * @note     (1) Socket arguments and/or operations are validated in network socket handler functions.
 *               Some arguments validated only if the validation code is enabled
 *               (i.e. RTOS_ARG_CHK_EXT_EN is DEF_ENABLED in 'rtos_cfg.h').
 *
 * @note     (2) If a non-zero number of retries is requested, a non-zero time delay SHOULD also
 *               be requested; otherwise, all retries will most likely fail immediately since no time
 *               will have elapsed to allow socket operation(s) to successfully complete.
 *******************************************************************************************************/
NET_SOCK_ID NetApp_SockOpen(NET_SOCK_PROTOCOL_FAMILY protocol_family,
                            NET_SOCK_TYPE            sock_type,
                            NET_SOCK_PROTOCOL        protocol,
                            CPU_INT16U               retry_max,
                            CPU_INT32U               time_dly_ms,
                            RTOS_ERR                 *p_err)
{
  NET_SOCK_ID sock_id;
  CPU_INT16U  retry_cnt;
  CPU_BOOLEAN done;
  CPU_BOOLEAN dly;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------------ OPEN APP SOCK -------------------
  retry_cnt = 0u;
  done = DEF_NO;
  dly = DEF_NO;
  sock_id = NET_SOCK_BSD_ERR_OPEN;

  while ((retry_cnt <= retry_max)                               // While open retry <= max retry ...
         && (done == DEF_NO)) {                                 // ... & open NOT done,          ...
    if (dly == DEF_YES) {                                       // Dly open, on retries.
      RTOS_ERR local_err;

      NetApp_TimeDly_ms(time_dly_ms, &local_err);

      PP_UNUSED_PARAM(local_err);
    }
    //                                                             ... open sock.
    sock_id = NetSock_Open(protocol_family,
                           sock_type,
                           protocol,
                           p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        done = DEF_YES;
        break;

      case RTOS_ERR_NOT_INIT:
      case RTOS_ERR_POOL_EMPTY:                                 // If transitory open err(s), ...
        retry_cnt++;
        dly = DEF_YES;                                          // ... dly retry.
        break;

      default:
        done = DEF_YES;
        break;
    }
  }

  return (sock_id);
}

/****************************************************************************************************//**
 *                                           NetApp_SockClose()
 *
 * @brief    (1) Closes an application socket, with error handling :
 *               - (a) Configure close timeout, if any
 *               - (b) Close application socket
 *
 * @param    sock_id     Socket descriptor/handle identifier of application socket to close.
 *
 * @param    timeout_ms  Socket close timeout value.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following
 *                       error code(s) from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_TYPE
 *                           - RTOS_ERR_NET_RETRY_MAX
 *                           - RTOS_ERR_NET_SOCK_CLOSED
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_NOT_SUPPORTED
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_NET_IF_LINK_DOWN
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_NET_OP_IN_PROGRESS
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_NET_INVALID_CONN
 *                           - RTOS_ERR_RX
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_NET_NEXT_HOP
 *
 * @return   DEF_OK,   application socket successfully closed.
 *           DEF_FAIL, otherwise.
 *
 * @note     (2) Socket arguments and/or operations are validated in network socket handler functions.
 *               Some arguments are validated only if the validation code is enabled (i.e.
 *               RTOS_ARG_CHK_EXT_EN is DEF_ENABLED in 'net_cfg.h').
 *
 * @note     (3) Once an application closes its socket, NO further operations on the socket are
 *               allowed and the application MUST NOT continue to access the socket.
 *               NO error is returned for any internal error while closing the socket.
 *******************************************************************************************************/
CPU_BOOLEAN NetApp_SockClose(NET_SOCK_ID sock_id,
                             CPU_INT32U  timeout_ms,
                             RTOS_ERR    *p_err)
{
  NET_SOCK_RTN_CODE rtn_code;
  CPU_BOOLEAN       rtn_status;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ---------------- CFG CLOSE TIMEOUT -----------------
  if (timeout_ms > 0) {                                         // If timeout avail, ...
                                                                // ... cfg close timeout.
    NetSock_CfgTimeoutConnCloseSet(sock_id, timeout_ms, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  }
  //                                                               ------------------ CLOSE APP SOCK ------------------
  rtn_code = NetSock_Close(sock_id, p_err);

  rtn_status = (rtn_code == NET_SOCK_BSD_ERR_NONE) ? DEF_OK : DEF_FAIL;

  return (rtn_status);
}

/****************************************************************************************************//**
 *                                               NetApp_SockBind()
 *
 * @brief    Binds an application socket to a local address, with error handling.
 *
 * @param    sock_id         Socket descriptor/handle identifier of the application to which to bind
 *                           the socket.
 *
 * @param    p_addr_local    Pointer to socket address structure.
 *
 * @param    addr_len        Length of socket address structure (in octets).
 *
 * @param    retry_max       Maximum number of consecutive socket bind retries).
 *
 * @param    time_dly_ms     Transitory socket bind delay value, in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following
 *                           error code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_TYPE
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_ALREADY_EXISTS
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_NET_INVALID_CONN
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *
 * @return   DEF_OK,   application socket is successfully bound to a local address.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) Socket arguments and/or operations are validated in network socket handler functions.
 *               Some arguments are validated only if the validation code is enabled (i.e.
 *               RTOS_ARG_CHK_EXT_EN is DEF_ENABLED in 'rtos_cfg.h').
 *
 * @note     (2) Socket address structure 'AddrFamily' member MUST be configured in host-order and
 *               MUST NOT be converted to/from network-order.
 *
 *               Socket address structure addresses MUST be configured/converted from host-order
 *               to network-order.
 *
 * @note     (3) If a non-zero number of retries is requested, a non-zero time delay SHOULD also be
 *               requested; otherwise, all retries will most likely fail immediately since no time will
 *               have elapsed to allow socket operation(s) to successfully complete.
 *******************************************************************************************************/
CPU_BOOLEAN NetApp_SockBind(NET_SOCK_ID       sock_id,
                            NET_SOCK_ADDR     *p_addr_local,
                            NET_SOCK_ADDR_LEN addr_len,
                            CPU_INT16U        retry_max,
                            CPU_INT32U        time_dly_ms,
                            RTOS_ERR          *p_err)
{
  NET_SOCK_RTN_CODE rtn_code;
  CPU_BOOLEAN       rtn_status;
  CPU_BOOLEAN       done;
  CPU_BOOLEAN       dly;
  CPU_INT16U        retry_cnt;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------------ BIND APP SOCK -------------------
  retry_cnt = 0u;
  done = DEF_NO;
  dly = DEF_NO;
  rtn_code = NET_SOCK_BSD_ERR_BIND;

  while ((retry_cnt <= retry_max)                               // While bind retry <= max retry ...
         && (done == DEF_NO)) {                                 // ... & bind NOT done,          ...
    if (dly == DEF_YES) {                                       // Dly bind, on retries.
      RTOS_ERR local_err;

      NetApp_TimeDly_ms(time_dly_ms, &local_err);

      PP_UNUSED_PARAM(local_err);
    }
    //                                                             ... bind sock.
    rtn_code = NetSock_Bind(sock_id,
                            p_addr_local,
                            addr_len,
                            p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        done = DEF_YES;
        break;

      case RTOS_ERR_NOT_INIT:
      case RTOS_ERR_ALREADY_EXISTS:                             // If transitory bind err(s), ...
      case RTOS_ERR_POOL_EMPTY:
        retry_cnt++;
        dly = DEF_YES;                                          // ... dly retry.
        break;

      default:
        done = DEF_YES;
        break;
    }
  }

  rtn_status = (rtn_code == NET_SOCK_BSD_ERR_NONE) ? DEF_OK : DEF_FAIL;

  return (rtn_status);
}

/****************************************************************************************************//**
 *                                               NetApp_SockConn()
 *
 * @brief    (1) Connects an application socket to a remote address, with error handling :
 *               - (a) Configure connect timeout, if any
 *               - (b) Connect application socket to remote address
 *               - (c) Restore connect timeout, if necessary
 *
 * @param    sock_id         Socket descriptor/handle identifier of application socket to which to
 *                           connect.
 *
 * @param    p_addr_remote   Pointer to socket address structure.
 *
 * @param    addr_len        Length of socket address structure (in octets).
 *
 * @param    retry_max       Maximum number of consecutive socket connect retries.
 *
 * @param    timeout_ms      Socket connect timeout value per attempt/retry.
 *
 * @param    time_dly_ms     Transitory socket connect delay value, in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following
 *                           error code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_TYPE
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_NOT_SUPPORTED
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_FAIL
 *                               - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_NET_IF_LINK_DOWN
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_NET_OP_IN_PROGRESS
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_ALREADY_EXISTS
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_NET_INVALID_CONN
 *                               - RTOS_ERR_RX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_NET_NEXT_HOP
 *                               - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *
 * @return   DEF_OK,   application socket has successfully connected to a remote address.
 *           DEF_FAIL, otherwise.
 *
 * @note     (2) Socket arguments and/or operations are validated in network socket handler functions.
 *               Some arguments are validated only if the validation code is enabled (i.e.
 *               RTOS_ARG_CHK_EXT_EN is DEF_ENABLED in 'rtos_cfg.h').
 *
 * @note     (3) Socket address structure 'AddrFamily' member MUST be configured in host-order and
 *               MUST NOT be converted to/from network-order.
 *               Socket address structure addresses MUST be configured/converted from host-order
 *               to network-order.
 *
 * @note     (4) If a non-zero number of retries is requested AND
 *               global socket blocking ('NET_SOCK_CFG_BLOCK_SEL') is configured
 *               for non-blocking operation ('NET_SOCK_BLOCK_SEL_NO_BLOCK')...
 *               ...then one or more of the following SHOULD also be requested; otherwise, all
 *               retries will most likely fail immediately since no time will have elapsed to
 *               allow socket operation(s) to successfully complete :
 *               - (a) A non-zero timeout
 *               - (b) A non-zero time delay
 *******************************************************************************************************/
CPU_BOOLEAN NetApp_SockConn(NET_SOCK_ID       sock_id,
                            NET_SOCK_ADDR     *p_addr_remote,
                            NET_SOCK_ADDR_LEN addr_len,
                            CPU_INT16U        retry_max,
                            CPU_INT32U        timeout_ms,
                            CPU_INT32U        time_dly_ms,
                            RTOS_ERR          *p_err)
{
  NET_SOCK_RTN_CODE rtn_code;
  CPU_BOOLEAN       rtn_status;
  CPU_INT16U        retry_cnt;
  CPU_INT32U        timeout_ms_cfgd;
  CPU_BOOLEAN       timeout_cfgd;
  CPU_BOOLEAN       done;
  CPU_BOOLEAN       dly;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- CFG CONN TIMEOUT -----------------
  if (timeout_ms > 0) {                                         // If timeout avail,         ...
                                                                // ... save cfg'd conn timeout ...
    timeout_ms_cfgd = NetSock_CfgTimeoutConnReqGet_ms(sock_id, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
    //                                                             ... & cfg temp conn timeout.
    NetSock_CfgTimeoutConnReqSet(sock_id, timeout_ms, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
    timeout_cfgd = DEF_YES;
  } else {
    timeout_cfgd = DEF_NO;
  }

  //                                                               ------------------ CONN APP SOCK -------------------
  retry_cnt = 0u;
  done = DEF_NO;
  dly = DEF_NO;
  rtn_code = NET_SOCK_BSD_ERR_CONN;

  while ((retry_cnt <= retry_max)                               // While conn retry <= max retry ...
         && (done == DEF_NO)) {                                 // ... & conn NOT done,          ...
    if (dly == DEF_YES) {                                       // Dly conn, on retries.
      RTOS_ERR local_err;

      NetApp_TimeDly_ms(time_dly_ms, &local_err);

      PP_UNUSED_PARAM(local_err);
    }
    //                                                             ... conn sock.
    rtn_code = NetSock_Conn(sock_id,
                            p_addr_remote,
                            addr_len,
                            p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        done = DEF_YES;
        break;

      case RTOS_ERR_ALREADY_EXISTS:                             // If transitory conn err(s), ...
      case RTOS_ERR_NET_OP_IN_PROGRESS:
      case RTOS_ERR_TIMEOUT:
      case RTOS_ERR_POOL_EMPTY:
      case RTOS_ERR_NET_IF_LINK_DOWN:
        retry_cnt++;
        dly = DEF_YES;                                          // ... dly retry.
        break;

      default:
        done = DEF_YES;
        break;
    }
  }

  //                                                               --------- RESTORE PREV CFG'D CONN TIMEOUT ----------
  if (timeout_cfgd == DEF_YES) {                                // If timeout cfg'd, ...
                                                                // ... restore prev'ly cfg'd conn timeout.
    NetSock_CfgTimeoutConnReqSet(sock_id, timeout_ms_cfgd, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  }

  rtn_status = (rtn_code == NET_SOCK_BSD_ERR_NONE) ? DEF_OK : DEF_FAIL;

  return (rtn_status);
}

/****************************************************************************************************//**
 *                                           NetApp_SockListen()
 *
 * @brief    Sets an application socket to listen for connection requests, with error handling.
 *
 * @param    sock_id         Socket descriptor/handle identifier of socket to listen.
 *
 * @param    sock_q_size     Maximum number of connection requests to accept & queue on listen socket.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following
 *                           error code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_TYPE
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_NET_INVALID_CONN
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *
 * @return   DEF_OK,   application socket is successfully set to listen.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) Socket arguments and/or operations are validated in network socket handler functions.
 *               Some arguments are validated only if the validation code is enabled (i.e.
 *               RTOS_ARG_CHK_EXT_EN is DEF_ENABLED in 'net_cfg.h').
 *
 * @note     (2) Socket listen operation is only valid for stream-type sockets.
 *******************************************************************************************************/
#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
CPU_BOOLEAN NetApp_SockListen(NET_SOCK_ID     sock_id,
                              NET_SOCK_Q_SIZE sock_q_size,
                              RTOS_ERR        *p_err)
{
  NET_SOCK_RTN_CODE rtn_code;
  CPU_BOOLEAN       rtn_status;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               -------------- SET APP SOCK TO LISTEN --------------
  rtn_code = NetSock_Listen(sock_id, sock_q_size, p_err);

  rtn_status = (rtn_code == NET_SOCK_BSD_ERR_NONE) ? DEF_OK : DEF_FAIL;

  return (rtn_status);
}
#endif

/****************************************************************************************************//**
 *                                           NetApp_SockAccept()
 *
 * @brief    (1) Returns a new application socket accepted from a listen application socket, with
 *               error handling :
 *               - (a) Configure the accept timeout, if any
 *               - (b) Wait for the  accept socket
 *               - (c) Restore the   accept timeout, if necessary
 *
 * @param    sock_id         Socket descriptor/handle identifier of listen socket.
 *
 * @param    p_addr_remote   Pointer to an address buffer that receives the socket address
 *                           structure.
 *
 * @param    p_addr_len      Pointer to a variable to pass the size of the socket address structure
 *                           and that will received the size of the accepted connection   s socket
 *                           address structure, if no errors, else a 0 size will be returned.
 *
 * @param    retry_max       Maximum number of consecutive socket accept retries.
 *
 * @param    timeout_ms      Socket accept timeout value per attempt/retry.
 *
 * @param    time_dly_ms     Transitory socket accept delay value, in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_TYPE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_NET_INVALID_CONN
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @return   Socket descriptor/handle identifier of new accepted socket, if NO error(s).
 *           NET_SOCK_BSD_ERR_ACCEPT,  otherwise.
 *
 * @note     (2) Socket arguments and/or operations are validated in network socket handler functions.
 *               Some arguments validated only if the validation code is enabled (i.e.
 *               RTOS_ARG_CHK_EXT_EN is DEF_ENABLED in 'rtos_cfg.h').
 *
 * @note     (3) Socket accept operation valid for stream-type sockets only.
 *
 * @note     (4) Socket address structure 'AddrFamily' member returned in host-order and SHOULD
 *               NOT be converted to network-order.
 *
 *               Socket address structure addresses returned in network-order and SHOULD be
 *               converted from network-order to host-order.
 *
 * @note     (5) If a non-zero number of retries is requested and the global socket blocking
 *               ('NET_SOCK_CFG_BLOCK_SEL') is configured for non-blocking operation
 *               ('NET_SOCK_BLOCK_SEL_NO_BLOCK'); then one or more of the following SHOULD also be
 *               requested; otherwise, all retries will most likely fail immediately since no time
 *               will elapse to wait for & allow socket operation(s) to successfully complete :
 *               - (a) One or more of the following SHOULD also be requested; otherwise, all
 *                     retries will most likely fail immediately since no time will have elapsed to
 *                     allow socket operation(s) to successfully complete :
 *                   - (1) A non-zero timeout
 *                   - (2) A non-zero time delay
 *
 * @internal
 * @note     (6) [INTERNAL] Pointers to variables that return values MUST be initialized PRIOR to all
 *               other validation or function handling in case of any error(s).
 * @endinternal
 *******************************************************************************************************/
#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
NET_SOCK_ID NetApp_SockAccept(NET_SOCK_ID       sock_id,
                              NET_SOCK_ADDR     *p_addr_remote,
                              NET_SOCK_ADDR_LEN *p_addr_len,
                              CPU_INT16U        retry_max,
                              CPU_INT32U        timeout_ms,
                              CPU_INT32U        time_dly_ms,
                              RTOS_ERR          *p_err)
{
  NET_SOCK_ADDR_LEN addr_len;
  NET_SOCK_ADDR_LEN addr_len_unused;
  NET_SOCK_ID       sock_id_accept;
  CPU_INT16U        retry_cnt;
  CPU_INT32U        timeout_ms_cfgd;
  CPU_BOOLEAN       timeout_cfgd;
  CPU_BOOLEAN       done;
  CPU_BOOLEAN       dly;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------------ VALIDATE ADDR -------------------
  if (p_addr_len != (NET_SOCK_ADDR_LEN *) 0) {                  // If avail,               ...
    addr_len = *p_addr_len;                                     // ... save init addr len; ...
  } else {
    p_addr_len = (NET_SOCK_ADDR_LEN *)&addr_len_unused;         // ... else re-cfg NULL rtn ptr to unused local var.
    addr_len = 0;
    PP_UNUSED_PARAM(addr_len_unused);                           // Prevent possible 'variable unused' warning.
  }
  *p_addr_len = 0;                                              // Cfg dflt addr len for err (see Note #6).

  //                                                               ---------------- CFG ACCEPT TIMEOUT ----------------
  if (timeout_ms > 0) {                                         // If timeout avail,         ...
                                                                // ... save cfg'd accept timeout ...
    timeout_ms_cfgd = NetSock_CfgTimeoutConnAcceptGet_ms(sock_id, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (NET_SOCK_BSD_ERR_ACCEPT);
    }
    //                                                             ... & cfg temp accept timeout.
    NetSock_CfgTimeoutConnAcceptSet(sock_id, timeout_ms, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (NET_SOCK_BSD_ERR_ACCEPT);
    }
  } else {
    timeout_cfgd = DEF_NO;
  }

  //                                                               ------------- WAIT FOR APP ACCEPT SOCK -------------
  retry_cnt = 0u;
  done = DEF_NO;
  dly = DEF_NO;
  sock_id_accept = NET_SOCK_BSD_ERR_ACCEPT;

  while ((retry_cnt <= retry_max)                               // While accept retry <= max retry ...
         && (done == DEF_NO)) {                                 // ... & accept NOT done,          ...
    if (dly == DEF_YES) {                                       // Dly accept, on retries.
      RTOS_ERR local_err;

      NetApp_TimeDly_ms(time_dly_ms, &local_err);

      PP_UNUSED_PARAM(local_err);
    }
    //                                                             ... wait for accept sock.
    *p_addr_len = addr_len;
    sock_id_accept = NetSock_Accept(sock_id,
                                    p_addr_remote,
                                    p_addr_len,
                                    p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        done = DEF_YES;
        break;

      case RTOS_ERR_POOL_EMPTY:                                 // If transitory accept err(s), ...
      case RTOS_ERR_TIMEOUT:
      case RTOS_ERR_WOULD_BLOCK:
        retry_cnt++;
        dly = DEF_YES;                                          // ... dly retry.
        break;

      default:
        done = DEF_YES;
        break;
    }
  }

  //                                                               -------- RESTORE PREV CFG'D ACCEPT TIMEOUT ---------
  if (timeout_cfgd == DEF_YES) {                                // If timeout cfg'd, ...
                                                                // ... restore prev'ly cfg'd accept timeout.
    NetSock_CfgTimeoutConnAcceptSet(sock_id, timeout_ms_cfgd, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (NET_SOCK_BSD_ERR_ACCEPT);
    }
  }

  return (sock_id_accept);
}
#endif

/****************************************************************************************************//**
 *                                               NetApp_SockRx()
 *
 * @brief    (1) Receive application data via socket, with error handling :
 *               - (a) Validate  receive arguments
 *               - (b) Configure receive timeout, if any
 *               - (c) Receive   application data via socket
 *               - (d) Restore   receive timeout, if necessary
 *
 * @param    sock_id         Socket descriptor/handle identifier of socket to receive application data.
 *
 * @param    p_data_buf      Pointer to an application data buffer that will  receive application data.
 *
 * @param    data_buf_len    Size of the   application data buffer (in octets).
 *
 * @param    data_rx_th      Application data receive threshold :
 *
 * @param    flags           Flags to select receive options; bit-field flags logically OR'd :
 *
 * @param    p_addr_remote   Pointer to an address buffer that receives the socket address
 *                           structure
 *
 * @param    p_addr_len      Pointer to a variable to pass the size of the socket address structure and that
 *                           will receive the size of the accepted connection's socket address structure,
 *                           if no errors, else a 0 size will be returned.
 *
 * @param    retry_max       Maximum number of consecutive socket receive retries.
 *
 * @param    timeout_ms      Socket receive timeout value per attempt/retry.
 *
 * @param    time_dly_ms     Transitory socket receive delay value, in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_NET_INVALID_CONN
 *                               - RTOS_ERR_NET_CONN_CLOSE_RX
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @return   Number of positive data octets received, if NO error(s).
 *           0, otherwise.
 *
 * @note     (2) Socket arguments and/or operations are validated in network socket handler functions.
 *               Some arguments are validated only if the validation code is enabled (i.e.
 *               RTOS_ARG_CHK_EXT_EN is DEF_ENABLED in 'rtos_cfg.h').
 *
 * @note     (3) Socket address structure 'AddrFamily' member returned in host-order and SHOULD NOT
 *               be converted to network-order.
 *
 *               Socket address structure addresses returned in network-order and SHOULD be
 *               converted from network-order to host-order.
 *
 * @note     (4) Datagram-type sockets transmit & receive all data atomically -- i.e. every
 *               single, complete datagram transmitted MUST be received as a single,
 *               complete datagram.
 *               @n
 *               IEEE Std 1003.1, 2004 Edition, Section 'recvfrom() : DESCRIPTION'
 *               summarizes that "for message-based sockets, such as ... SOCK_DGRAM ...
 *               the entire message shall be read in a single operation.  If a message is
 *               too long to fit in the supplied buffer, and MSG_PEEK is not set in the
 *               flags argument, the excess bytes shall be discarded".
 *               @n
 *               Thus if the socket's type is datagram & the receive data buffer size is
 *               NOT large enough for the received data, the receive data buffer is maximally
 *               filled with receive data but the remaining data octets are discarded &
 *               RTOS_ERR_WOULD_OVF error is returned.
 *
 * @note     (5) Stream-type sockets transmit & receive all data octets in one or more
 *               non-distinct packets.  In other words, the application data is NOT
 *               bounded by any specific packet(s); rather, it is contiguous & sequenced
 *               from one packet to the next.
 *               @n
 *               IEEE Std 1003.1, 2004 Edition, Section 'recv() : DESCRIPTION' summarizes
 *               that "for stream-based sockets, such as SOCK_STREAM, message boundaries
 *               shall be ignored.  In this case, data shall be returned to the user as
 *               soon as it becomes available, and no data shall be discarded".
 *               @n
 *               If the socket's type is stream and the receive data buffer size is NOT
 *               large enough for the received data, the receive data buffer is maximally
 *               filled with receive data & the remaining data octets remain queued for
 *               later application receives.
 *
 * @note     (6) If a non-zero number of retries is requested and one of the following conditions...
 *               - (a) global socket blocking ('NET_SOCK_CFG_BLOCK_SEL') is configured
 *                     for non-blocking operation ('NET_SOCK_BLOCK_SEL_NO_BLOCK'), OR
 *               - (b) socket 'flags' argument set to 'NET_SOCK_FLAG_RX_BLOCK';
 *                     @n
 *                     ... then one or more of the following SHOULD also be requested; otherwise, all
 *                     retries will most likely fail immediately since no time will have elapsed to
 *                     allow the socket operation(s) to successfully complete :
 *               - (c) A non-zero timeout
 *               - (d) A non-zero time delay
 *
 * @internal
 * @note     (7) [INTERNAL] Pointers to variables that return values MUST be initialized PRIOR to all
 *               other validation or function handling in case of any error(s).
 * @endinternal
 *******************************************************************************************************/
CPU_INT16U NetApp_SockRx(NET_SOCK_ID        sock_id,
                         void               *p_data_buf,
                         CPU_INT16U         data_buf_len,
                         CPU_INT16U         data_rx_th,
                         NET_SOCK_API_FLAGS flags,
                         NET_SOCK_ADDR      *p_addr_remote,
                         NET_SOCK_ADDR_LEN  *p_addr_len,
                         CPU_INT16U         retry_max,
                         CPU_INT32U         timeout_ms,
                         CPU_INT32U         time_dly_ms,
                         RTOS_ERR           *p_err)
{
  NET_SOCK_ADDR_LEN *p_addr_len_init = (NET_SOCK_ADDR_LEN *) p_addr_len;
  NET_SOCK_ADDR_LEN addr_len;
  NET_SOCK_ADDR_LEN addr_len_unused;
  NET_SOCK_ADDR_LEN addr_len_temp;
  NET_SOCK_ADDR     addr_temp;
  CPU_INT08U        *p_data_buf_rem;
  CPU_INT16U        data_buf_len_rem;
  CPU_INT16S        rx_len;
  CPU_INT16U        rx_len_tot;
  CPU_INT16U        rx_th_actual;
  CPU_INT16U        retry_cnt;
  CPU_INT32U        timeout_ms_cfgd;
  CPU_BOOLEAN       timeout_cfgd;
  CPU_BOOLEAN       done;
  CPU_BOOLEAN       dly;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- VALIDATE RX ADDR -----------------
  if (p_addr_len != DEF_NULL) {                                 // If avail,               ...
    addr_len = *p_addr_len;                                     // ... save init addr len; ...
  } else {
    p_addr_len = &addr_len_unused;                              // ... else re-cfg NULL rtn ptr to unused local var.
    addr_len = 0;
    PP_UNUSED_PARAM(addr_len_unused);                           // Prevent possible 'variable unused' warning.
  }
  *p_addr_len = 0;                                              // Cfg dflt addr len for err (see Note #6).

  RTOS_ASSERT_DBG_ERR_SET((((p_addr_remote == DEF_NULL)  && (p_addr_len_init == DEF_NULL))
                           || ((p_addr_remote != DEF_NULL)  && (p_addr_len_init != DEF_NULL))), *p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);

  if (p_addr_remote == DEF_NULL) {                              // If remote addr/addr len NOT avail, ...
    p_addr_remote = &addr_temp;                                 // ...   use temp addr                ...
    p_addr_len = &addr_len_temp;                                // ...     & temp addr len.
    addr_len = (NET_SOCK_ADDR_LEN) sizeof(addr_temp);           // Save init temp addr len.
  }

  //                                                               --------------- VALIDATE DATA RX TH ----------------
  if (data_rx_th < 1) {
    rx_th_actual = 1u;                                          // Lim rx th to at least 1 octet ...
  } else if (data_rx_th > data_buf_len) {
    rx_th_actual = data_buf_len;                                // ... & max of app buf data len.
  } else {
    rx_th_actual = data_rx_th;
  }

  //                                                               ------------------ CFG RX TIMEOUT ------------------
  if (timeout_ms > 0) {                                         // If timeout avail,         ...
                                                                // ... save cfg'd rx timeout ...
    RTOS_ERR local_err;

    timeout_ms_cfgd = NetSock_CfgTimeoutRxQ_Get_ms(sock_id, &local_err);

    PP_UNUSED_PARAM(local_err);
    //                                                             ... & cfg temp rx timeout.
    NetSock_CfgTimeoutRxQ_Set(sock_id, timeout_ms, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      timeout_cfgd = DEF_YES;
    } else {
      return (DEF_FAIL);
    }
  } else {
    timeout_cfgd = DEF_NO;
  }

  //                                                               ------------------- RX APP DATA --------------------
  rx_len_tot = 0u;
  retry_cnt = 0u;
  done = DEF_NO;
  dly = DEF_NO;

  while ((rx_len_tot < rx_th_actual)                            // While rx tot len <  rx th     ...
         && (retry_cnt <= retry_max)                            // ... & rx retry   <= max retry ...
         && (done == DEF_NO)) {                                 // ... & rx NOT done,            ...
    if (dly == DEF_YES) {                                       // Dly rx, on retries.
      RTOS_ERR local_err;

      NetApp_TimeDly_ms(time_dly_ms, &local_err);
      PP_UNUSED_PARAM(local_err);
    }
    //                                                             ... rx app data.
    *p_addr_len = addr_len;
    p_data_buf_rem = (CPU_INT08U *)p_data_buf    + rx_len_tot;
    data_buf_len_rem = (CPU_INT16U)(data_buf_len - rx_len_tot);
    rx_len = (CPU_INT16S) NetSock_RxDataFrom(sock_id,
                                             p_data_buf_rem,
                                             data_buf_len_rem,
                                             flags,
                                             p_addr_remote,
                                             p_addr_len,
                                             DEF_NULL,
                                             0u,
                                             DEF_NULL,
                                             p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        if (rx_len > 0) {                                       // If          rx len > 0, ...
          rx_len_tot += (CPU_INT16U)rx_len;                     // ... inc tot rx len.
        }
        retry_cnt = 0u;
        dly = DEF_NO;
        break;

      case RTOS_ERR_WOULD_OVF:
        if (rx_len > 0) {                                       // If          rx len > 0, ...
          rx_len_tot += (CPU_INT16U)rx_len;                     // ... inc tot rx len.
        }
        //                                                         If app data buf NOT large enough for all rx'd data,
        done = DEF_YES;
        //                                                         .. rtn data buf ovf err (see Note #4).
        break;

      //                                                           If transitory rx err(s), ...
      case RTOS_ERR_WOULD_BLOCK:
      case RTOS_ERR_TIMEOUT:
        retry_cnt++;
        dly = DEF_YES;                                          // ... dly next rx.
        break;

      default:
        done = DEF_YES;
        break;
    }
  }

  //                                                               ---------- RESTORE PREV CFG'D RX TIMEOUT -----------
  if (timeout_cfgd == DEF_YES) {                                // If timeout cfg'd, ...
                                                                // ... restore prev'ly cfg'd rx timeout.
    RTOS_ERR local_err;

    NetSock_CfgTimeoutRxQ_Set(sock_id, timeout_ms_cfgd, &local_err);

    PP_UNUSED_PARAM(local_err);
  }

  return (rx_len_tot);
}

/****************************************************************************************************//**
 *                                               NetApp_SockTx()
 *
 * @brief    (1) Transmit application data via socket, with error handling :
 *               - (a) Configure transmit timeout, if any
 *               - (b) Transmit  application data via socket
 *               - (c) Restore   transmit timeout, if necessary
 *
 * @param    sock_id         Socket descriptor/handle identifier of socket to transmit application data.
 *
 * @param    p_data          Pointer to application data to transmit.
 *
 * @param    data_len        Length  of application data to transmit (in octets).
 *
 * @param    flags           Flags to select transmit options; bit-field flags logically OR'd :
 *                           NET_SOCK_FLAG_NONE
 *                           NET_SOCK_FLAG_TX_NO_BLOCK
 *
 * @param    p_addr_remote   Pointer to destination address buffer.
 *
 * @param    addr_len        Length of  destination address buffer (in octets).
 *
 * @param    retry_max       Maximum number of consecutive socket transmit retries.
 *
 * @param    timeout_ms      Socket transmit timeout value per attempt/retry.
 *
 * @param    time_dly_ms     Transitory socket transmit delay value, in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_TYPE
 *                               - RTOS_ERR_NET_RETRY_MAX
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_NOT_SUPPORTED
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_FAIL
 *                               - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_NET_IF_LINK_DOWN
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_ALREADY_EXISTS
 *                               - RTOS_ERR_NET_INVALID_CONN
 *                               - RTOS_ERR_RX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_NET_NEXT_HOP
 *                               - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *
 * @return   Number of positive data octets transmitted, if NO error(s).
 *           0, otherwise.
 *
 * @note     (2) Socket arguments and/or operations are validated in network socket handler functions.
 *               Some arguments are validated only if the validation code is enabled (i.e.
 *               RTOS_ARG_CHK_EXT_EN is DEF_ENABLED in 'rtos_cfg.h').
 *
 * @note     (3) Socket address structure 'AddrFamily' member MUST be configured in host-order and
 *               MUST NOT be converted to/from network-order.
 *               Socket address structure addresses MUST be configured/converted from host-order
 *               to network-order.
 *
 * @note     (4) If a non-zero number of retries is requested and the global socket blocking
 *               ('NET_SOCK_CFG_BLOCK_SEL') is configured for non-blocking operation
 *               ('NET_SOCK_BLOCK_SEL_NO_BLOCK'); then one or more of the following SHOULD also be
 *               requested; otherwise, all retries will most likely fail immediately since no time
 *               will elapse to wait for & allow socket operation(s) to successfully complete :
 *                   - (a) A non-zero timeout
 *                   - (b) A non-zero time delay
 *
 * @note     (5) Datagram sockets NOT currently blocked during transmit and therefore require NO
 *               transmit timeout.
 *******************************************************************************************************/
CPU_INT16U NetApp_SockTx(NET_SOCK_ID        sock_id,
                         void               *p_data,
                         CPU_INT16U         data_len,
                         NET_SOCK_API_FLAGS flags,
                         NET_SOCK_ADDR      *p_addr_remote,
                         NET_SOCK_ADDR_LEN  addr_len,
                         CPU_INT16U         retry_max,
                         CPU_INT32U         timeout_ms,
                         CPU_INT32U         time_dly_ms,
                         RTOS_ERR           *p_err)
{
  CPU_INT08U  *p_data_buf;
  CPU_INT16U  data_buf_len;
  CPU_INT16S  tx_len;
  CPU_INT16U  tx_len_tot;
  CPU_INT16U  retry_cnt;
  CPU_INT32U  timeout_ms_cfgd;
  CPU_BOOLEAN timeout_cfgd;
  CPU_BOOLEAN done;
  CPU_BOOLEAN dly;
  RTOS_ERR    local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------------ CFG TX TIMEOUT ------------------
  if (timeout_ms > 0) {                                         // If timeout avail,         ...
                                                                // ... save cfg'd tx timeout ...
    timeout_ms_cfgd = NetSock_CfgTimeoutTxQ_Get_ms(sock_id, &local_err);
    //                                                             ... & cfg temp tx timeout.
    NetSock_CfgTimeoutTxQ_Set(sock_id, timeout_ms, p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        timeout_cfgd = DEF_YES;
        break;

      case RTOS_ERR_INVALID_TYPE:                               // Datagram sock timeout NOT avail (see Note #5).
        timeout_cfgd = DEF_NO;
        break;

      default:
        return (DEF_FAIL);
    }
  } else {
    timeout_cfgd = DEF_NO;
  }

  //                                                               ------------------- TX APP DATA --------------------
  tx_len_tot = 0u;
  retry_cnt = 0u;
  done = DEF_NO;
  dly = DEF_NO;

  while ((tx_len_tot < data_len)                                // While tx tot len <  app data len ...
         && (retry_cnt <= retry_max)                            // ... & tx retry   <= max retry    ...
         && (done == DEF_NO)) {                                 // ... & tx NOT done,               ...
    if (dly == DEF_YES) {                                       // Dly tx, on retries.
      NetApp_TimeDly_ms(time_dly_ms, &local_err);
    }
    //                                                             ... tx app data.
    p_data_buf = (CPU_INT08U *)p_data     + tx_len_tot;
    data_buf_len = (CPU_INT16U)(data_len - tx_len_tot);
    tx_len = (CPU_INT16S)NetSock_TxDataTo(sock_id,
                                          p_data_buf,
                                          data_buf_len,
                                          flags,
                                          p_addr_remote,
                                          addr_len,
                                          p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        if (tx_len > 0) {                                       // If          tx len > 0, ...
          tx_len_tot += (CPU_INT16U)tx_len;                     // ... inc tot tx len.
        }
        retry_cnt = 0u;
        dly = DEF_NO;
        break;

      case RTOS_ERR_NET_IF_LINK_DOWN:                           // If transitory tx err(s), ...
      case RTOS_ERR_POOL_EMPTY:
      case RTOS_ERR_TIMEOUT:
      case RTOS_ERR_WOULD_BLOCK:
        retry_cnt++;
        dly = DEF_YES;                                          // ... dly next tx.
        break;

      default:
        done = DEF_YES;
        break;
    }
  }

  //                                                               ---------- RESTORE PREV CFG'D TX TIMEOUT -----------
  if (timeout_cfgd == DEF_YES) {                                // If timeout cfg'd, ...
                                                                // ... restore prev'ly cfg'd tx timeout.
    NetSock_CfgTimeoutTxQ_Set(sock_id, timeout_ms_cfgd, &local_err);
  }

  PP_UNUSED_PARAM(local_err);

  return (tx_len_tot);
}

/****************************************************************************************************//**
 *                                           NetApp_SetSockAddr()
 *
 * @brief    Sets up a socket address from an IPv4 or an IPv6 address.
 *
 * @param    p_sock_addr     Pointer to the socket address that will be configure by this function.
 *
 * @param    addr_family     IP address family to configure, possible values:
 *                           NET_SOCK_ADDR_FAMILY_IP_V4
 *                           NET_SOCK_ADDR_FAMILY_IP_V6
 *
 * @param    port_nbr        Port number.
 *
 * @param    p_addr          Pointer to IP address to use.
 *
 * @param    addr_len        Length of the IP address to use.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *******************************************************************************************************/
void NetApp_SetSockAddr(NET_SOCK_ADDR        *p_sock_addr,
                        NET_SOCK_ADDR_FAMILY addr_family,
                        NET_PORT_NBR         port_nbr,
                        CPU_INT08U           *p_addr,
                        NET_IP_ADDR_LEN      addr_len,
                        RTOS_ERR             *p_err)
{
#ifdef  NET_IPv4_MODULE_EN
  NET_SOCK_ADDR_IPv4 *p_addr_ipv4;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_SOCK_ADDR_IPv6 *p_addr_ipv6;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_sock_addr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
#if !defined(NET_IPv4_MODULE_EN)
  if (addr_family == NET_SOCK_ADDR_FAMILY_IP_V4) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
  }
#endif

#if !defined(NET_IPv6_MODULE_EN)
  if (addr_family == NET_SOCK_ADDR_FAMILY_IP_V6) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
  }
#endif

  switch (addr_family) {
    case NET_SOCK_ADDR_FAMILY_IP_V4:
#ifdef  NET_IPv4_MODULE_EN
      RTOS_ASSERT_DBG_ERR_SET((addr_len == NET_IPv4_ADDR_SIZE), *p_err, RTOS_ERR_INVALID_ARG,; );
      Mem_Clr(p_sock_addr, NET_SOCK_ADDR_IPv4_SIZE);

      p_addr_ipv4 = (NET_SOCK_ADDR_IPv4 *)p_sock_addr;
      p_addr_ipv4->AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
      p_addr_ipv4->Port = NET_UTIL_HOST_TO_NET_16(port_nbr);

      NET_UTIL_VAL_COPY_GET_NET_32(&p_addr_ipv4->Addr, p_addr);
      break;
#endif

    case NET_SOCK_ADDR_FAMILY_IP_V6:
#ifdef  NET_IPv6_MODULE_EN
      RTOS_ASSERT_DBG_ERR_SET((addr_len == NET_IPv6_ADDR_SIZE), *p_err, RTOS_ERR_INVALID_ARG,; );

      Mem_Clr(p_sock_addr, NET_SOCK_ADDR_IPv6_SIZE);
      p_addr_ipv6 = (NET_SOCK_ADDR_IPv6 *)p_sock_addr;
      p_addr_ipv6->AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V6;
      p_addr_ipv6->Port = NET_UTIL_HOST_TO_NET_16(port_nbr);

      Mem_Copy(&p_addr_ipv6->Addr,
               p_addr,
               addr_len);
      break;
#endif

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }
}

/****************************************************************************************************//**
 *                                   NetApp_ClientStreamOpenByHostname()
 *
 * @brief    (1) Connects a client to a server using its host name with a stream (TCP) socket
 *               (select IP address automatically See Note #2) by following these steps :
 *               - (a) Get the IP address of the remote host from a string that contains either the IP
 *                     address or the host name that will be resolved using DNS (See Note #2).
 *               - (b) Open a stream socket.
 *               - (c) Connect a stream socket.
 *
 * @param    p_sock_id           Pointer to a variable that receives the socket ID opened from this
 *                               function.
 *
 * @param    p_remote_host_name  Pointer to a string that contains the remote host name to resolve.
 *
 * @param    remote_port_nbr     Port of the remote host.
 *
 * @param    p_sock_addr         Pointer to a variable that receives the socket address of the
 *                               remote host.
 *
 * @param    p_secure_cfg        Pointer to the secure configuration (TLS/SSL), if needed.
 *
 * @param    req_timeout_ms      Connection timeout in ms.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_INVALID_TYPE
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_NOT_SUPPORTED
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_FAIL
 *                                   - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                                   - RTOS_ERR_WOULD_OVF
 *                                   - RTOS_ERR_NET_IF_LINK_DOWN
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_NET_OP_IN_PROGRESS
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_ALREADY_EXISTS
 *                                   - RTOS_ERR_NOT_FOUND
 *                                   - RTOS_ERR_NET_INVALID_CONN
 *                                   - RTOS_ERR_RX
 *                                   - RTOS_ERR_NOT_READY
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NET_ADDR_UNRESOLVED
 *                                   - RTOS_ERR_NET_NEXT_HOP
 *                                   - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *
 * @return   NET_IP_ADDR_FAMILY_IPv4, if connected successfully using an IPv4 address.
 *           NET_IP_ADDR_FAMILY_IPv6, if connected successfully using an IPv6 address.
 *           NET_IP_ADDR_FAMILY_UNKNOWN, otherwise.
 *
 * @note     (2) When a host name is passed to the remote host name parameter, this function tries
 *               to resolve the address of the remote host using DNS.
 *               - (a) The DNS must be present and enabled in the project for the resolve to be possible.
 *               - (b) If an IPv6 and an IPv4 address are found for the remote host. This function will
 *                     first try to connect to the remote host using the IPv6 address. If the connection
 *                     fails using IPv6, a connection retry will occur using the IPv4 address.
 *               - (c) This function always blocks and the fail timeout depends on the DNS resolution
 *                     timeout, the number of remote addresses found, and the connection timeout parameter.
 *
 * @note     (3) This function is in blocking mode, which means that at the end of the function, the
 *               socket will have succeeded or failed to connect to the remote host.
 *******************************************************************************************************/
NET_IP_ADDR_FAMILY NetApp_ClientStreamOpenByHostname(NET_SOCK_ID             *p_sock_id,
                                                     CPU_CHAR                *p_remote_host_name,
                                                     NET_PORT_NBR            remote_port_nbr,
                                                     NET_SOCK_ADDR           *p_sock_addr,
                                                     NET_APP_SOCK_SECURE_CFG *p_secure_cfg,
                                                     CPU_INT32U              req_timeout_ms,
                                                     RTOS_ERR                *p_err)
{
  NET_IP_ADDR_FAMILY addr_rtn = NET_IP_ADDR_FAMILY_UNKNOWN;
  CPU_BOOLEAN        done = DEF_NO;
  CPU_INT08U         addr[NET_SOCK_BSD_ADDR_LEN_MAX];
  NET_IP_ADDR_FAMILY addr_family;
  NET_SOCK_ADDR      sock_addr_local;
  NET_SOCK_ADDR      *p_sock_addr_local;
#ifdef  NET_DNS_CLIENT_MODULE_EN
#if (defined(NET_IPv4_MODULE_EN) \
  & defined(NET_IPv6_MODULE_EN))
  CPU_BOOLEAN switch_addr_type = DEF_NO;
#endif
  DNSc_STATUS status;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_IP_ADDR_FAMILY_UNKNOWN);
  RTOS_ASSERT_DBG_ERR_SET((p_sock_id != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IP_ADDR_FAMILY_UNKNOWN);
  RTOS_ASSERT_DBG_ERR_SET((p_remote_host_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IP_ADDR_FAMILY_UNKNOWN);
#ifndef  NET_SECURE_MODULE_EN
  RTOS_ASSERT_DBG_ERR_SET((p_secure_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_AVAIL, NET_IP_ADDR_FAMILY_UNKNOWN);
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               -------------- SET ADDRESS IP FAMILY ---------------
#ifdef  NET_IPv6_MODULE_EN
  addr_family = NET_IP_ADDR_FAMILY_IPv6;                        // First try with IPv6 if enabled.
#else
  addr_family = NET_IP_ADDR_FAMILY_IPv4;
#endif

  //                                                               ------------- SET SOCK ADDRESS POINTER -------------
  if (p_sock_addr == DEF_NULL) {
    p_sock_addr_local = &sock_addr_local;
  } else {
    p_sock_addr_local = p_sock_addr;
  }

  //                                                               ------- RESOLVE ADDR, OPEN & CONNECT SOCKET --------
  while (done == DEF_NO) {
    CPU_BOOLEAN connect;
#ifdef  NET_DNS_CLIENT_MODULE_EN
    NET_IP_ADDR_OBJ addr_dns;
    DNSc_FLAGS      flags = DNSc_FLAG_NONE;
    CPU_INT08U      addr_nbr = 1u;
    RTOS_ERR        err;

    RTOS_ERR_SET(err, RTOS_ERR_NONE);

    //                                                             ------------- RESOLVE REMOTE HOST ADDR -------------
    switch (addr_family) {                                      // Set DNS Parameters
      case NET_IP_ADDR_FAMILY_IPv6:
        DEF_BIT_SET(flags, DNSc_FLAG_IPv6_ONLY);                // Get IPv6 address only.
        break;

      case NET_IP_ADDR_FAMILY_IPv4:
        DEF_BIT_SET(flags, DNSc_FLAG_IPv4_ONLY);                // Get IPv4 address only.
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_IP_ADDR_FAMILY_UNKNOWN);
    }

    //                                                             ------------------ DNS RESOLUTION ------------------
    status = DNSc_GetHost(p_remote_host_name, &addr_dns, &addr_nbr, flags, DEF_NULL, &err);
    switch (status) {
      case DNSc_STATUS_RESOLVED:
        if (addr_nbr != 0) {
          Mem_Copy(addr, addr_dns.Addr.Array, addr_dns.AddrLen);
          connect = DEF_YES;
          switch (addr_dns.AddrLen) {
            case NET_IPv4_ADDR_LEN:
              addr_family = NET_IP_ADDR_FAMILY_IPv4;
              break;

            case NET_IPv6_ADDR_LEN:
              addr_family = NET_IP_ADDR_FAMILY_IPv6;
              break;

            default:
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_IP_ADDR_FAMILY_UNKNOWN);
          }
        } else {
          connect = DEF_NO;
          if (addr_family == NET_IP_ADDR_FAMILY_IPv6) {
            addr_family = NET_IP_ADDR_FAMILY_IPv4;
          } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_NET_ADDR_UNRESOLVED);
            goto exit;
          }
        }
        break;

      case DNSc_STATUS_FAILED:
      case DNSc_STATUS_PENDING:
      default:
        connect = DEF_NO;
        if (addr_family == NET_IP_ADDR_FAMILY_IPv6) {
          addr_family = NET_IP_ADDR_FAMILY_IPv4;
        } else {
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_ADDR_UNRESOLVED);
          goto exit;
        }
        break;
    }

#else
    //                                                             ------------ CONVERT STRING IP ADDRESS -------------
    addr_family = NetASCII_Str_to_IP(p_remote_host_name, addr, NET_SOCK_BSD_ADDR_LEN_MAX, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    switch (addr_family) {
      case NET_IP_ADDR_FAMILY_IPv6:
        connect = DEF_YES;
        break;

      case NET_IP_ADDR_FAMILY_IPv4:
        connect = DEF_YES;
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_IP_ADDR_FAMILY_UNKNOWN);
    }
#endif

    //                                                             ------------ CONNECT TO THE REMOTE HOST ------------
    if (connect == DEF_YES) {
      *p_sock_id = NetApp_ClientStreamOpen(addr,
                                           addr_family,
                                           remote_port_nbr,
                                           p_sock_addr_local,
                                           p_secure_cfg,
                                           req_timeout_ms,
                                           p_err);
      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_NONE:
        case RTOS_ERR_NET_OP_IN_PROGRESS:
          done = DEF_YES;
          addr_rtn = addr_family;
          goto exit;

        default:
#if (defined(NET_DNS_CLIENT_MODULE_EN)   \
          && defined(NET_IPv4_MODULE_EN) \
          && defined(NET_IPv6_MODULE_EN))
          switch_addr_type = DEF_YES;                           // then retry with other IP type
          break;
#else
          goto exit;
#endif
      }

#if (defined(NET_DNS_CLIENT_MODULE_EN) \
      && defined(NET_IPv4_MODULE_EN)   \
      && defined(NET_IPv6_MODULE_EN))
      if (switch_addr_type == DEF_YES) {
        if (addr_family == NET_IP_ADDR_FAMILY_IPv6) {
          addr_family = NET_IP_ADDR_FAMILY_IPv4;
        } else {
          goto exit;
        }
      }
#endif
    }
  }

exit:
  return (addr_rtn);
}

/****************************************************************************************************//**
 *                                   NetApp_ClientDatagramOpenByHostname()
 *
 * @brief    (1) Open a datagram type (UDP) socket to the server using its host name.
 *               (select IP address automatically See Note #2):
 *               - (a) Get IP address of the remote host from a string that contains either the IP
 *                     address or the host name that will be resolved using DNS (See Note #2).
 *               - (b) Open a datagram socket.
 *
 * @param    p_sock_id           Pointer to a variable that receives the socket ID opened from this
 *                               function.
 *
 * @param    p_remote_host_name  Pointer to a string that contains the remote host name to resolve.
 *
 * @param    remote_port_nbr     Port of the remote host.
 *
 * @param    ip_family           Select IP family of addresses returned by DNS resolution :
 *                                   - NET_IP_ADDR_FAMILY_IPv4
 *                                   - NET_IP_ADDR_FAMILY_IPv6
 *
 * @param    p_sock_addr         Pointer to a variable that receives the socket address of the
 *                               remote host.
 *
 * @param    p_is_hostname       Pointer to variable that receives the boolean to indicate if the
 *                               string passed in p_remote_host_name was a hostname or a IP address.
 *                               DEF_YES, hostname was received.
 *                               DEF_NO,  otherwise.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_NET_ADDR_UNRESOLVED
 *                                   - RTOS_ERR_NET_STR_ADDR_INVALID
 *
 * @return   NET_IP_ADDR_FAMILY_IPv4,    if the opening was successful using an IPv4 address.
 *           NET_IP_ADDR_FAMILY_IPv6,    if the opening was successful using an IPv6 address.
 *           NET_IP_ADDR_FAMILY_UNKNOWN, otherwise.
 *
 * @note     (2) When a host name is passed into the remote host name parameter, this function tries
 *               to resolve the address of the remote host using DNS.
 *              - (a) The DNS must be present and enabled in the project for the resolve to be possible.
 *              - (b) The ip_family argument let the application choose the IP family of the addresses
 *                    returned by the DNS resolution.
 *              - (c) This function always blocks and the fail timeout depends on the DNS resolution
 *                    timeout, the number of remote addresses found, and the connection timeout parameter.
 *******************************************************************************************************/
NET_IP_ADDR_FAMILY NetApp_ClientDatagramOpenByHostname(NET_SOCK_ID        *p_sock_id,
                                                       CPU_CHAR           *p_remote_host_name,
                                                       NET_PORT_NBR       remote_port_nbr,
                                                       NET_IP_ADDR_FAMILY ip_family,
                                                       NET_SOCK_ADDR      *p_sock_addr,
                                                       CPU_BOOLEAN        *p_is_hostname,
                                                       RTOS_ERR           *p_err)
{
  NET_SOCK_ADDR      *p_sock_addr_local;
  NET_SOCK_ADDR      sock_addr_local;
  NET_IP_ADDR_FAMILY addr_family;
  CPU_INT08U         addr[NET_SOCK_BSD_ADDR_LEN_MAX];
  CPU_BOOLEAN        do_dns;
#ifdef  NET_DNS_CLIENT_MODULE_EN
  NET_IP_ADDR_OBJ addr_dns;
  DNSc_STATUS     status;
  DNSc_FLAGS      flags = DNSc_FLAG_NONE;
  CPU_INT08U      addr_nbr = 1u;
  RTOS_ERR        err;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_IP_ADDR_FAMILY_UNKNOWN);
  RTOS_ASSERT_DBG_ERR_SET((p_sock_id != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IP_ADDR_FAMILY_UNKNOWN);
  RTOS_ASSERT_DBG_ERR_SET((p_remote_host_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IP_ADDR_FAMILY_UNKNOWN);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  PP_UNUSED_PARAM(ip_family);

  //                                                               ------------- SET SOCK ADDRESS POINTER -------------
  if (p_sock_addr == DEF_NULL) {
    p_sock_addr_local = &sock_addr_local;
  } else {
    p_sock_addr_local = p_sock_addr;
  }

  //                                                               ------------- RESOLVE REMOTE HOST ADDR -------------
  //                                                               PARSE STRING FOR IP ADDR FORMAT
  addr_family = NetASCII_Str_to_IP(p_remote_host_name,
                                   addr,
                                   NET_SOCK_BSD_ADDR_LEN_MAX,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
#ifdef  NET_DNS_CLIENT_MODULE_EN
    do_dns = DEF_YES;
#else
    addr_family = NET_IP_ADDR_FAMILY_UNKNOWN;
    *p_is_hostname = DEF_NO;
    goto exit;
#endif
  } else {
    switch (addr_family) {
      case NET_IP_ADDR_FAMILY_IPv4:
      case NET_IP_ADDR_FAMILY_IPv6:
        *p_is_hostname = DEF_NO;
        do_dns = DEF_NO;
        break;

      default:
#ifdef  NET_DNS_CLIENT_MODULE_EN
        do_dns = DEF_YES;
        break;
#else
        addr_family = NET_IP_ADDR_FAMILY_UNKNOWN;
        *p_is_hostname = DEF_NO;
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_IP_ADDR_FAMILY_UNKNOWN);
#endif
    }
  }

#ifdef  NET_DNS_CLIENT_MODULE_EN                                // ------------------ DNS RESOLUTION ------------------
  if (do_dns == DEF_YES) {
    //                                                             Set DNS Parameters
    DEF_BIT_SET(flags, DNSc_FLAG_FORCE_RESOLUTION);

    switch (ip_family) {
      case NET_IP_ADDR_FAMILY_IPv6:
        flags |= DNSc_FLAG_IPv6_ONLY;                           // Get IPv6 address only.
        break;

      case NET_IP_ADDR_FAMILY_IPv4:
        flags |= DNSc_FLAG_IPv4_ONLY;                           // Get IPv4 address only.
        break;

      default:
        addr_family = NET_IP_ADDR_FAMILY_UNKNOWN;
        *p_is_hostname = DEF_YES;
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_IP_ADDR_FAMILY_UNKNOWN);
    }

    status = DNSc_GetHost(p_remote_host_name, &addr_dns, &addr_nbr, flags, DEF_NULL, &err);
    switch (status) {
      case DNSc_STATUS_RESOLVED:
        if (addr_nbr != 0) {
          Mem_Copy(addr, addr_dns.Addr.Array, addr_dns.AddrLen);
          switch (addr_dns.AddrLen) {
            case NET_IPv4_ADDR_LEN:
            case NET_IPv6_ADDR_LEN:
              addr_family = ip_family;
              *p_is_hostname = DEF_YES;
              break;

            default:
              addr_family = NET_IP_ADDR_FAMILY_UNKNOWN;
              *p_is_hostname = DEF_YES;
              RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_IP_ADDR_FAMILY_UNKNOWN);
          }
        } else {
          addr_family = NET_IP_ADDR_FAMILY_UNKNOWN;
          *p_is_hostname = DEF_YES;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_ADDR_UNRESOLVED);
          goto exit;
        }
        break;

      case DNSc_STATUS_FAILED:
      case DNSc_STATUS_PENDING:
      default:
        addr_family = NET_IP_ADDR_FAMILY_UNKNOWN;
        *p_is_hostname = DEF_YES;
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_ADDR_UNRESOLVED);
        goto exit;
    }
  }
#endif

  PP_UNUSED_PARAM(do_dns);

  //                                                               ------------ OPEN TO THE REMOTE HOST ------------
  *p_sock_id = NetApp_ClientDatagramOpen(addr,
                                         addr_family,
                                         remote_port_nbr,
                                         p_sock_addr_local,
                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    addr_family = NET_IP_ADDR_FAMILY_UNKNOWN;
    goto exit;
  }

exit:
  return (addr_family);
}

/****************************************************************************************************//**
 *                                           NetApp_ClientStreamOpen()
 *
 * @brief    (1) Connects a client to a server using an IP address (IPv4 or IPv6) with a stream socket
 *               by following these steps :
 *               - (a) Open a stream socket.
 *               - (b) Set Security parameter (TLS/SSL) if required.
 *               - (c) Set connection timeout.
 *               - (d) Connect to the remote host.
 *
 * @param    p_addr              Pointer to IP address.
 *
 * @param    addr_family         IP family of the address.
 *
 * @param    remote_port_nbr     Port of the remote host.
 *
 * @param    p_sock_addr         Pointer to a variable that receives the socket address of the
 *                               remote host.
 *
 * @param    p_secure_cfg        Pointer to the secure configuration (TLS/SSL), if needed.
 *
 * @param    req_timeout_ms      Connection timeout in milliseconds.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_INVALID_TYPE
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_NOT_SUPPORTED
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_FAIL
 *                                   - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                                   - RTOS_ERR_WOULD_OVF
 *                                   - RTOS_ERR_NET_IF_LINK_DOWN
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_NET_OP_IN_PROGRESS
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_ALREADY_EXISTS
 *                                   - RTOS_ERR_NOT_FOUND
 *                                   - RTOS_ERR_NET_INVALID_CONN
 *                                   - RTOS_ERR_RX
 *                                   - RTOS_ERR_NOT_READY
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NET_NEXT_HOP
 *                                   - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *
 * @return   Socket ID, if no error.
 *           NET_SOCK_ID_NONE, otherwise.
 *
 * @note     (1) This function is in blocking mode, which mean that at the end of the function, the
 *               socket will have succeeded or failed to connect to the remote host.
 *******************************************************************************************************/
NET_SOCK_ID NetApp_ClientStreamOpen(CPU_INT08U              *p_addr,
                                    NET_IP_ADDR_FAMILY      addr_family,
                                    NET_PORT_NBR            remote_port_nbr,
                                    NET_SOCK_ADDR           *p_sock_addr,
                                    NET_APP_SOCK_SECURE_CFG *p_secure_cfg,
                                    CPU_INT32U              req_timeout_ms,
                                    RTOS_ERR                *p_err)
{
  NET_SOCK_ID              sock_id = NET_SOCK_ID_NONE;
  CPU_INT08U               addr_len = 0u;
  NET_SOCK_ADDR            sock_addr;
  NET_SOCK_ADDR            *p_sock_addr_local;
  NET_SOCK_PROTOCOL_FAMILY protocol_family = NET_SOCK_PROTOCOL_FAMILY_NONE;
  NET_SOCK_ADDR_FAMILY     sock_addr_family = NET_SOCK_ADDR_FAMILY_IP_V4;
  CPU_INT08U               block_state;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_addr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_SOCK_ID_NONE);
#ifndef  NET_SECURE_MODULE_EN
  RTOS_ASSERT_DBG_ERR_SET((p_secure_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_AVAIL, NET_SOCK_ID_NONE);
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (p_sock_addr == DEF_NULL) {
    p_sock_addr_local = &sock_addr;
  } else {
    p_sock_addr_local = p_sock_addr;
  }

  //                                                               ---------- PREPARE PARAMETERS TO CONNECT -----------
  switch (addr_family) {
    case NET_IP_ADDR_FAMILY_IPv4:
      protocol_family = NET_SOCK_PROTOCOL_FAMILY_IP_V4;
      sock_addr_family = NET_SOCK_ADDR_FAMILY_IP_V4;
      addr_len = NET_IPv4_ADDR_LEN;
      break;

    case NET_IP_ADDR_FAMILY_IPv6:
      protocol_family = NET_SOCK_PROTOCOL_FAMILY_IP_V6;
      sock_addr_family = NET_SOCK_ADDR_FAMILY_IP_V6;
      addr_len = NET_IPv6_ADDR_LEN;
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, NET_SOCK_ID_NONE);
  }

  //                                                               ------------------ SET SOCK ADDR -------------------
  NetApp_SetSockAddr(p_sock_addr_local, sock_addr_family, remote_port_nbr, p_addr, addr_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               -------------------- OPEN SOCK ---------------------
  sock_id = NetSock_Open(protocol_family, NET_SOCK_TYPE_STREAM, NET_SOCK_PROTOCOL_DFLT, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ---------------- CFG SOCK BLOCK OPT ----------------
  block_state = NetSock_BlockGet(sock_id, p_err);               // retrieve blocking mode of current socket.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  //                                                               Set mode to blocking for connect operation.
  (void)NetSock_CfgBlock(sock_id, NET_SOCK_BLOCK_SEL_BLOCK, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  //                                                               ----------------- SET CONN TIMEOUT -----------------
  NetSock_CfgTimeoutConnReqSet(sock_id,
                               req_timeout_ms,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  //                                                               ----------- SET SECURITY CONN PARAMETERS -----------
#if  (defined(NET_SECURE_MODULE_EN) \
  && NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT > 0)
  if (p_secure_cfg != DEF_NULL) {
    (void)NetSock_CfgSecure(sock_id,
                            DEF_YES,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_close;
    }

    NetSock_CfgSecureClientCommonName(sock_id,
                                      (CPU_CHAR *)p_secure_cfg->CommonName,
                                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_close;
    }

    NetSock_CfgSecureClientTrustCallBack(sock_id,
                                         p_secure_cfg->TrustCallback,
                                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_close;
    }

    if (p_secure_cfg->CertPtr != DEF_NULL) {
      NetSock_CfgSecureClientCertKeyInstall(sock_id,
                                            p_secure_cfg->CertPtr,
                                            p_secure_cfg->CertSize,
                                            p_secure_cfg->KeyPtr,
                                            p_secure_cfg->KeySize,
                                            p_secure_cfg->CertFmt,
                                            p_secure_cfg->CertChain,
                                            p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_close;
      }
    }
  }
#endif

  //                                                               ------------- CONN TO THE REMOTE HOST --------------
  NetSock_Conn(sock_id, p_sock_addr_local, addr_len, p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
    case RTOS_ERR_NET_OP_IN_PROGRESS:
      break;

    case RTOS_ERR_TIMEOUT:
    default:
      goto exit_close;
  }

  //                                                               ---------------- CFG SOCK BLOCK OPT ----------------
  (void)NetSock_CfgBlock(sock_id, block_state, p_err);          // re-configure blocking mode to one before connect.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_close;
  }

  PP_UNUSED_PARAM(p_secure_cfg);                                // Prevent 'variable unused' compiler warning.

  goto exit;

exit_close:
  {
    RTOS_ERR local_err;

    NetSock_Close(sock_id, &local_err);
    PP_UNUSED_PARAM(local_err);                                 // Prevent 'variable unused' compiler warning.
    sock_id = NET_SOCK_ID_NONE;
  }

exit:
  return (sock_id);
}

/****************************************************************************************************//**
 *                                       NetApp_ClientDatagramOpen()
 *
 * @brief    (1) Connects a client to a server using an IP address (IPv4 or IPv6) with a datagram
 *               socket by following these steps :
 *               - (a) Open a datagram socket.
 *               - (b) Set connection timeout.
 *
 * @param    p_addr              Pointer to IP address.
 *
 * @param    addr_family         IP family of the address.
 *
 * @param    remote_port_nbr     Port of the remote host.
 *
 * @param    p_sock_addr         Pointer to a variable that receives the socket address of the
 *                               remote host.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_POOL_EMPTY
 *
 * @return   Socket ID, if no errors,
 *           NET_SOCK_ID_NONE, otherwise.
 *******************************************************************************************************/
NET_SOCK_ID NetApp_ClientDatagramOpen(CPU_INT08U         *p_addr,
                                      NET_IP_ADDR_FAMILY addr_family,
                                      NET_PORT_NBR       remote_port_nbr,
                                      NET_SOCK_ADDR      *p_sock_addr,
                                      RTOS_ERR           *p_err)
{
  NET_SOCK_ID              sock_id = NET_SOCK_ID_NONE;
  CPU_INT08U               addr_len = 0u;
  NET_SOCK_ADDR            sock_addr;
  NET_SOCK_ADDR            *p_sock_addr_local;
  NET_SOCK_PROTOCOL_FAMILY protocol_family = NET_SOCK_PROTOCOL_FAMILY_NONE;
  NET_SOCK_ADDR_FAMILY     sock_addr_family = NET_SOCK_ADDR_FAMILY_IP_V4;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_addr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_SOCK_ID_NONE);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (p_sock_addr == DEF_NULL) {
    p_sock_addr_local = &sock_addr;
  } else {
    p_sock_addr_local = p_sock_addr;
  }

  //                                                               ---------- PREPARE PARAMETERS TO CONNECT -----------
  switch (addr_family) {
    case NET_IP_ADDR_FAMILY_IPv4:
      protocol_family = NET_SOCK_PROTOCOL_FAMILY_IP_V4;
      sock_addr_family = NET_SOCK_ADDR_FAMILY_IP_V4;
      addr_len = NET_IPv4_ADDR_LEN;
      break;

    case NET_IP_ADDR_FAMILY_IPv6:
      protocol_family = NET_SOCK_PROTOCOL_FAMILY_IP_V6;
      sock_addr_family = NET_SOCK_ADDR_FAMILY_IP_V6;
      addr_len = NET_IPv6_ADDR_LEN;
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, sock_id);
  }

  //                                                               ------------------ SET SOCK ADDR -------------------
  NetApp_SetSockAddr(p_sock_addr_local, sock_addr_family, remote_port_nbr, p_addr, addr_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               -------------------- OPEN SOCK ---------------------
  sock_id = NetSock_Open(protocol_family, NET_SOCK_TYPE_DATAGRAM, NET_SOCK_PROTOCOL_DFLT, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (sock_id);
}

/****************************************************************************************************//**
 *                                           NetApp_TimeDly_ms()
 *
 * @brief    Delay for specified time, in milliseconds.
 *
 * @param    time_dly_ms     Time delay value, in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *
 * @note     (1) Time delay of 0 milliseconds allowed.
 *               @n
 *               Time delay limited to the maximum possible OS time delay
 *               if greater than the maximum possible OS time delay.
 *******************************************************************************************************/
void NetApp_TimeDly_ms(CPU_INT32U time_dly_ms,
                       RTOS_ERR   *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  KAL_Dly(time_dly_ms);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
