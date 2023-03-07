/***************************************************************************//**
 * @file
 * @brief Network - IPerf Client
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

#if (defined(RTOS_MODULE_NET_IPERF_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error IPerf Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

#include  "iperf_priv.h"
#ifdef  IPERF_CLIENT_MODULE_PRESENT

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <net/include/iperf.h>
#include  <net/include/net_util.h>

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET, IPERF)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void IPerf_ClientSocketInit(IPERF_TEST *p_test,
                                   RTOS_ERR   *p_err);

static void IPerf_ClientTCP(IPERF_TEST *p_test,
                            RTOS_ERR   *p_err);

static void IPerf_ClientUDP(IPERF_TEST *p_test,
                            RTOS_ERR   *p_err);

static void IPerf_ClientTxUDP_FIN(IPERF_TEST *p_test,
                                  CPU_CHAR   *p_data_buf);

static CPU_BOOLEAN IPerf_ClientTx(IPERF_TEST *p_test,
                                  CPU_CHAR   *p_data_buf,
                                  CPU_INT16U retry_max,
                                  CPU_INT32U time_dly_ms,
                                  RTOS_ERR   *p_err);

static void IPerf_ClientPattern(CPU_CHAR   *cp,
                                CPU_INT16U cnt);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           IPerf_ClientStart()
 *
 * @brief    (1) Process IPerf as a client :
 *               - (a) Initialize socket
 *               - (b) Run TCP or UDP transmitter
 *               - (c) Close used socket
 *
 * @param    p_test  Pointer to a test.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void IPerf_ClientStart(IPERF_TEST *p_test,
                       RTOS_ERR   *p_err)
{
  IPERF_OPT                *p_opt;
  IPERF_CONN               *p_conn;
  NET_SOCK_TYPE            sock_type;
  NET_SOCK_PROTOCOL        sock_protocol;
  NET_SOCK_PROTOCOL_FAMILY addr_family;
  RTOS_ERR                 local_err;

  p_conn = &p_test->Conn;
  p_opt = &p_test->Opt;

  LOG_VRB(("\n\r-------------IPerf DBG CLIENT START-------------\n\r"));
  LOG_VRB(("Init Client socket.\n\r"));

  KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

  addr_family = ((p_opt->IPv4 == DEF_YES) ? (NET_SOCK_PROTOCOL_FAMILY_IP_V4) : (NET_SOCK_PROTOCOL_FAMILY_IP_V6));

  //                                                               -------------------- OPEN SOCK ---------------------
  LOG_VRB(("Server SockOpen "));
  if (p_opt->Protocol == IPERF_PROTOCOL_UDP) {
    LOG_VRB(("UDP ... "));
    sock_type = NET_SOCK_TYPE_DATAGRAM;
    sock_protocol = NET_SOCK_PROTOCOL_UDP;
  } else {
    LOG_VRB(("TCP ... "));
    sock_type = NET_SOCK_TYPE_STREAM;
    sock_protocol = NET_SOCK_PROTOCOL_TCP;
  }

  p_conn->SockID = NetApp_SockOpen(addr_family,
                                   sock_type,
                                   sock_protocol,
                                   IPERF_OPEN_MAX_RETRY,
                                   IPERF_OPEN_MAX_DLY_MS,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_test->Err = IPERF_REP_ERR_CLIENT_SOCK_OPEN;
    goto exit_release;
  }

  LOG_VRB(("done.\n\r"));

  IPerf_ClientSocketInit(p_test, p_err);                        // -------------------- INIT SOCK ---------------------
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  KAL_LockRelease(IPerf_LockHandle, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

  //                                                               --------------- TCP/UDP TRANSMITTER ----------------
  if (p_opt->Protocol == IPERF_PROTOCOL_TCP) {
    IPerf_ClientTCP(p_test, p_err);
  } else if (p_opt->Protocol == IPERF_PROTOCOL_UDP) {
    IPerf_ClientUDP(p_test, p_err);
  }

  goto exit_close;

  //                                                               -------------------- CLOSE SOCK --------------------
exit_release:
  KAL_LockRelease(IPerf_LockHandle, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

exit_close:
  LOG_VRB(("Closing socket.\n\r"));

  if (p_conn->SockID != NET_SOCK_ID_NONE) {
    (void)NetApp_SockClose(p_conn->SockID, 0u, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_VRB(("Sock close error : %u\n\r", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(local_err))));
    }
  }

  goto exit;

exit:
  LOG_VRB(("\n\r-------------IPerf DBG CLIENT END-------------\n\r"));
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           IPerf_ClientSocketInit()
 *
 * @brief    (1) Initialize one socket for client use :
 *               - (a) Open a socket
 *               - (b) If bind client is enabled , bind the socket on local address & the same port as server
 *               - (c) Connect to remote addr & port
 *
 * @param    p_test  Pointer to a test.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void IPerf_ClientSocketInit(IPERF_TEST *p_test,
                                   RTOS_ERR   *p_err)
{
  IPERF_OPT            *p_opt;
  IPERF_CONN           *p_conn;
  NET_SOCK_ADDR_FAMILY addr_family;
  CPU_INT16U           server_port;
  CPU_BOOLEAN          cfg_succeed;
#if (IPERF_CFG_CLIENT_BIND_EN == DEF_ENABLED)
  NET_IF_NBR       if_nbr;
  NET_IP_ADDRS_QTY addr_tbl_size;
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR addr_ipv4;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_IPv6_ADDR addr_ipv6;
#endif
#endif
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR remote_addr_ipv4;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_IPv6_ADDR remote_addr_ipv6;
#endif
  CPU_INT16U retry;
  CPU_INT32U timeout;
  CPU_INT32U dly;
  CORE_DECLARE_IRQ_STATE;

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;

  CORE_ENTER_ATOMIC();
  retry = IPerf_CfgPtr->Client.ConnMaxRetry;
  timeout = IPerf_CfgPtr->Client.ConnMaxTimeoutMs;
  dly = IPerf_CfgPtr->Client.ConnMaxDlyMs;
  CORE_EXIT_ATOMIC();

  server_port = p_opt->Port;
  addr_family = ((p_opt->IPv4 == DEF_YES) ? (NET_SOCK_ADDR_FAMILY_IP_V4) : (NET_SOCK_ADDR_FAMILY_IP_V6));

  switch (addr_family) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V4:
      remote_addr_ipv4 = NetASCII_Str_to_IPv4(p_opt->IP_AddrRemote, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        p_test->Err = IPERF_REP_ERR_CLIENT_SOCK_OPEN;
        goto exit;
      }

      NetApp_SetSockAddr(&p_conn->ServerAddrPort,
                         NET_SOCK_ADDR_FAMILY_IP_V4,
                         server_port,
                         (CPU_INT08U *)&remote_addr_ipv4,
                         sizeof(remote_addr_ipv4),
                         p_err);
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V6:
      remote_addr_ipv6 = NetASCII_Str_to_IPv6(p_opt->IP_AddrRemote, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        p_test->Err = IPERF_REP_ERR_CLIENT_SOCK_OPEN;
        goto exit;
      }

      NetApp_SetSockAddr(&p_conn->ServerAddrPort,
                         NET_SOCK_ADDR_FAMILY_IP_V6,
                         server_port,
                         (CPU_INT08U *)&remote_addr_ipv6,
                         sizeof(remote_addr_ipv6),
                         p_err);
      break;
#endif
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_test->Err = IPERF_REP_ERR_CLIENT_SOCK_OPEN;
    goto exit;
  }

  cfg_succeed = NetSock_CfgBlock(p_conn->SockID, NET_SOCK_BLOCK_SEL_BLOCK, p_err);
  if (cfg_succeed != DEF_OK) {
    p_test->Err = IPERF_REP_ERR_CLIENT_SOCK_OPEN;
    goto exit;
  }

#if (IPERF_CFG_CLIENT_BIND_EN == DEF_ENABLED)

  if_nbr = p_conn->IF_Nbr;
  //                                                               -------------------- BIND SOCK ---------------------
  Mem_Clr((void *)&p_conn->ClientAddrPort,
          (CPU_SIZE_T) NET_SOCK_ADDR_SIZE);

  switch (addr_family) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V4:
      addr_tbl_size = 1;

      (void)NetIPv4_GetAddrHost(if_nbr,
                                &addr_ipv4,
                                &addr_tbl_size,
                                p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        p_test->Err = IPERF_REP_ERR_CLIENT_SOCK_OPEN;
        goto exit;
      }
      NetApp_SetSockAddr(&p_conn->ClientAddrPort,
                         NET_SOCK_ADDR_FAMILY_IP_V4,
                         server_port,
                         (CPU_INT08U *)&addr_ipv4,
                         NET_IPv4_ADDR_SIZE,
                         p_err);
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V6:
      addr_tbl_size = 1;
      (void)NetIPv6_GetAddrHost(if_nbr,
                                &addr_ipv6,
                                &addr_tbl_size,
                                p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        p_test->Err = IPERF_REP_ERR_CLIENT_SOCK_OPEN;
        goto exit;
      }
      NetApp_SetSockAddr(&p_conn->ClientAddrPort,
                         NET_SOCK_ADDR_FAMILY_IP_V6,
                         server_port,
                         (CPU_INT08U *)&addr_ipv6,
                         NET_IPv6_ADDR_SIZE,
                         p_err);
      break;
#endif
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  LOG_VRB(("Socket Bind ... "));
  (void)NetApp_SockBind(p_conn->SockID,
                        &p_conn->ClientAddrPort,
                        NET_SOCK_ADDR_SIZE,
                        IPERF_BIND_MAX_RETRY,
                        IPERF_BIND_MAX_DLY_MS,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_test->Err = IPERF_REP_ERR_CLIENT_SOCK_BIND;
    goto exit;
  }
  LOG_VRB(("done\r\n\r"));
#endif

  //                                                               ---------------- REMOTE CONNECTION -----------------
  //                                                               Remote IP addr for sock conn.
  LOG_VRB(("Socket Conn ... "));
  (void)NetApp_SockConn(p_conn->SockID,
                        &p_conn->ServerAddrPort,
                        NET_SOCK_ADDR_SIZE,
                        retry,
                        timeout,
                        dly,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_test->Err = IPERF_REP_ERR_CLIENT_SOCK_CONN;
    goto exit;
  }
  LOG_VRB(("Done\n\r"));
  LOG_VRB(("Connected to : ", (s)p_opt->IP_AddrRemote, ", port: ", (u)p_opt->Port, "\r\n\r"));

exit:
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR err = *p_err;

    PP_UNUSED_PARAM(err);
    LOG_VRB(("Fail error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err)), ".\n\r"));
  }
}

/****************************************************************************************************//**
 *                                               IPerf_ClientTCP()
 *
 * @brief    (1) IPerf TCP client (tranmitter):
 *               - (a) Initialize buffer
 *               - (b) Transmit data until the end of test is reached
 *
 * @param    p_test  Pointer to a test.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void IPerf_ClientTCP(IPERF_TEST *p_test,
                            RTOS_ERR   *p_err)
{
  IPERF_OPT   *p_opt;
  IPERF_CONN  *p_conn;
  IPERF_STATS *p_stats;
  CPU_CHAR    *p_data_buf;
  CPU_BOOLEAN tx_done;
#if (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED)
  CPU_INT16U cpu_usage;
#endif
#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
  IPERF_TS_MS ts_ms_prev;
  CPU_INT32U  tx_bytes_prev;
#endif
  NET_SOCK_ID sock_id;
  CPU_INT32U  tx_timeout;
  RTOS_ERR    local_err;
  CORE_DECLARE_IRQ_STATE;

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;
  p_stats = &p_test->Stats;
  tx_done = DEF_NO;

  CORE_ENTER_ATOMIC();
  tx_timeout = IPerf_CfgPtr->Client.TCP_TxMaxTimeoutMs;
  sock_id = p_conn->SockID;
  CORE_EXIT_ATOMIC();

  //                                                               ----------------- INIT BUF PATTERN -----------------
  p_data_buf = IPerf_BufPtr;

  NetSock_CfgTimeoutTxQ_Set(sock_id, tx_timeout, &local_err);

  KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

  IPerf_ClientPattern(p_data_buf, p_opt->BufLen);

  //                                                               --------------------- TX DATA ----------------------
  LOG_VRB(("------------------- TCP START SENDING -------------------\n\r"));
  p_stats->TS_Start_ms = IPerf_Get_TS_ms();
#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
  ts_ms_prev = p_stats->TS_Start_ms;
  tx_bytes_prev = 0u;
#endif
  p_conn->Run = DEF_YES;

  while (tx_done == DEF_NO) {                                   // Loop until the end of sending process.
    KAL_LockRelease(IPerf_LockHandle, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

    tx_done = IPerf_ClientTx(p_test,
                             p_data_buf,
                             IPERF_CLIENT_TCP_TX_MAX_RETRY,
                             IPERF_CLIENT_TCP_TX_MAX_DLY_MS,
                             p_err);

    KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

#if (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED)
    cpu_usage = KAL_CPU_UsageGet();
    p_stats->CPU_UsageAvg += cpu_usage;
    p_stats->CPU_CalcNbr++;
    if (cpu_usage > p_stats->CPU_UsageMax) {
      p_stats->CPU_UsageMax = cpu_usage;
    }

#endif

#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
    IPerf_UpdateBandwidth(p_test, &ts_ms_prev, &tx_bytes_prev);
#endif
  }

  p_conn->Run = DEF_NO;

  KAL_LockRelease(IPerf_LockHandle, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    LOG_VRB(("*************** CLIENT TCP RESULT ***************\n\r"));
    LOG_VRB(("Tx Call count  = %u \n\r", (u)p_stats->NbrCalls));
    LOG_VRB(("Tx Err  count  = %u \n\r", (u)p_stats->Errs));
    LOG_VRB(("------------------- END SENDING -------------------\n\r"));
  }
}

/****************************************************************************************************//**
 *                                               IPerf_ClientUDP()
 *
 * @brief    (1) IPerf UDP client :
 *               - (a) Initialize test performance statistics & buffer
 *               - (b) Transmit data until the end of test is reached
 *               - (c) Transmit UDP FIN datagram to finish UDP test
 *
 * @param    p_test  Pointer to a test.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void IPerf_ClientUDP(IPERF_TEST *p_test,
                            RTOS_ERR   *p_err)
{
  IPERF_OPT          *p_opt;
  IPERF_CONN         *p_conn;
  IPERF_STATS        *p_stats;
  IPERF_UDP_DATAGRAM *p_buf;                                    // UDP datagram buf ptr.
  CPU_CHAR           *p_data_buf;
  CPU_INT32S         pkt_id;
  IPERF_TS_MS        ts_cur_ms;
  CPU_INT32U         tv_sec;
  CPU_INT32U         tv_usec;
  CPU_BOOLEAN        tx_done;
#if (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED)
  CPU_INT16U cpu_usage;
#endif
#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
  IPERF_TS_MS ts_ms_prev;
  CPU_INT32U  tx_bytes_prev;
#endif

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;
  p_stats = &p_test->Stats;
  tx_done = DEF_NO;
  pkt_id = 0u;

  KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

  //                                                               ----------------- INIT BUF PATTERN -----------------
  p_data_buf = IPerf_BufPtr;

  IPerf_ClientPattern(p_data_buf, p_opt->BufLen);
  p_buf = (IPERF_UDP_DATAGRAM *)p_data_buf;
  p_conn->Run = DEF_YES;

  //                                                               --------------------- TX DATA ----------------------
  LOG_VRB(("------------------- UDP START SENDING -------------------\n\r"));
  p_stats->TS_Start_ms = IPerf_Get_TS_ms();
#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
  ts_ms_prev = p_stats->TS_Start_ms;
  tx_bytes_prev = 0u;
#endif

  while (tx_done == DEF_NO) {
    ts_cur_ms = IPerf_Get_TS_ms();
    tv_sec = ts_cur_ms /  DEF_TIME_NBR_mS_PER_SEC;
    tv_usec = (ts_cur_ms - (tv_sec * DEF_TIME_NBR_mS_PER_SEC)) * (DEF_TIME_NBR_uS_PER_SEC / DEF_TIME_NBR_mS_PER_SEC);
    p_buf->ID = NET_UTIL_HOST_TO_NET_32(pkt_id);                 // Set UDP datagram to send.
    p_buf->TimeVar_sec = NET_UTIL_HOST_TO_NET_32(tv_sec);
    p_buf->TimeVar_usec = NET_UTIL_HOST_TO_NET_32(tv_usec);

    KAL_LockRelease(IPerf_LockHandle, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

    tx_done = IPerf_ClientTx((IPERF_TEST *)p_test,
                             (CPU_CHAR *)p_data_buf,
                             (CPU_INT16U)IPERF_CLIENT_UDP_TX_MAX_RETRY,
                             (CPU_INT32U)IPERF_CLIENT_UDP_TX_MAX_DLY_MS,
                             (RTOS_ERR *)p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      pkt_id++;
    }

    KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

#if (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED)
    cpu_usage = KAL_CPU_UsageGet();
    p_stats->CPU_UsageAvg += cpu_usage;
    p_stats->CPU_CalcNbr++;
    if (cpu_usage > p_stats->CPU_UsageMax) {
      p_stats->CPU_UsageMax = cpu_usage;
    }

#endif

#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
    IPerf_UpdateBandwidth(p_test, &ts_ms_prev, &tx_bytes_prev);
#endif
  }

  //                                                               -------------------- TX UDP FIN --------------------
  ts_cur_ms = IPerf_Get_TS_ms();
  tv_sec = ts_cur_ms /  DEF_TIME_NBR_mS_PER_SEC;
  tv_usec = (ts_cur_ms - (tv_sec * DEF_TIME_NBR_mS_PER_SEC)) * (DEF_TIME_NBR_uS_PER_SEC / DEF_TIME_NBR_mS_PER_SEC);
  p_buf->ID = NET_UTIL_HOST_TO_NET_32(-pkt_id);                 // Prepare UDP datagram FIN to send.
  p_buf->TimeVar_sec = NET_UTIL_HOST_TO_NET_32(tv_sec);
  p_buf->TimeVar_usec = NET_UTIL_HOST_TO_NET_32(tv_usec);
  p_conn->Run = DEF_NO;
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    IPerf_ClientTxUDP_FIN(p_test, p_data_buf);

    LOG_VRB(("*************** CLIENT UDP RESULT ***************\n\r"));
    LOG_VRB(("Tx Call count  = %u \n\r", (u)p_stats->NbrCalls));
    LOG_VRB(("Tx Err  count  = %u \n\r", (u)p_stats->Errs));
    LOG_VRB(("*************************************************\n\r"));
    LOG_VRB(("------------------- END SENDING -------------------\n\r"));
  }

#if (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED)
  KAL_Dly(IPERF_UDP_CPU_USAGE_CALC_DLY_MS);                 // dly is needed to update task stats.
  p_stats->CPU_UsageMax = KAL_CPU_UsageGet();
  p_stats->CPU_UsageAvg = KAL_CPU_UsageGet();
  p_stats->CPU_CalcNbr++;
#endif

  KAL_LockRelease(IPerf_LockHandle, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
}

/****************************************************************************************************//**
 *                                           IPerf_ClientTxUDP_FIN()
 *
 * @brief    (1) Transmit UDP datagram FIN (end of UDP test) & receive UDP FINACK from server :
 *               - (a) Transmit UDP client datagram header FIN
 *               - (b) Receive  UDP FINACK from server
 *
 * @param    p_test      Pointer to a test.
 *
 * @param    p_data_buf  Pointer to data to transmit.
 *
 * @note     (1) $$$$ Server send UDP statistic into the FINACK paket (bytes reveived, stop time,
 *               lost count, out of order count, last paket id received and jitter). Presently,
 *               this data is not saved into the test statistics.
 *******************************************************************************************************/
static void IPerf_ClientTxUDP_FIN(IPERF_TEST *p_test,
                                  CPU_CHAR   *p_data_buf)
{
  IPERF_CONN        *p_conn;
  IPERF_STATS       *p_stats;
  CPU_INT08U        data_len;
  CPU_INT08U        tx_ctr;
  CPU_INT16U        tx_err_ctr;
  CPU_INT32S        data_received;
  CPU_BOOLEAN       done;
  NET_SOCK_ADDR_LEN addr_len_server;
  RTOS_ERR          local_err;

  p_conn = &p_test->Conn;
  p_stats = &p_test->Stats;
  data_len = 128u;
  tx_ctr = 0u;
  tx_err_ctr = 0u;
  done = DEF_NO;

  LOG_VRB(("Sending FIN ACK:"));

  while (done == DEF_NO) {                                      // Loop until rx'd UDP FINACK or tx'd 10 time.
                                                                // ------------------ TX CLIENT HDR -------------------
    addr_len_server = sizeof(p_conn->ServerAddrPort);
    (void)NetApp_SockTx(p_conn->SockID,
                        p_data_buf,
                        data_len,
                        NET_SOCK_FLAG_NONE,
                        &p_conn->ServerAddrPort,
                        addr_len_server,
                        IPERF_CLIENT_UDP_TX_MAX_RETRY,
                        0,
                        IPERF_CLIENT_UDP_TX_MAX_DLY_MS,
                        &local_err);
    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
      tx_ctr++;                                                   // Inc tx cnt.
      LOG_VRB(("."));
    } else {
      LOG_VRB(("Tx error : %u\n\r", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(local_err))));
      tx_err_ctr++;
    }
    //                                                             -------------- RX SERVER UDP FIN ACK ---------------
    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
      data_received = NetApp_SockRx(p_conn->SockID,
                                    p_data_buf,
                                    IPERF_UDP_BUF_LEN_MAX,
                                    0u,
                                    NET_SOCK_FLAG_NONE,
                                    &p_conn->ServerAddrPort,
                                    &addr_len_server,
                                    IPERF_RX_UDP_FINACK_MAX_RETRY,
                                    IPERF_RX_UDP_FINACK_MAX_TIMEOUT_MS,
                                    IPERF_RX_UDP_FINACK_MAX_DLY_MS,
                                    &local_err);
      if (data_received > 0) {
        p_stats->UDP_EndErr = DEF_NO;
        LOG_VRB(("\n\rReceived UDP FINACK from server.\n\r"));
        done = DEF_YES;                                         // Rx'd server UDP FINACK, UDP test done.
        continue;                                               // $$$$ Sto rx'd stats of server (see Note #1).
      }
    }

    if (tx_ctr == IPERF_SERVER_UDP_TX_FINACK_COUNT) {
      p_stats->UDP_EndErr = DEF_YES;
      LOG_VRB(("\n\rSent %u UDP FIN and did not receive UDP FINACK from server.\n\r", (u)IPERF_SERVER_UDP_TX_FINACK_COUNT));
      done = DEF_YES;
    } else if (tx_err_ctr > IPERF_SERVER_UDP_TX_FINACK_ERR_MAX) {
      p_stats->UDP_EndErr = DEF_YES;
      LOG_VRB(("\n\rTx errors exceed maximum, %u FIN ACK sent\n\r", (u)tx_ctr));
      done = DEF_YES;
    }
  }
}

/****************************************************************************************************//**
 *                                               IPerf_ClientTx()
 *
 * @brief    Transmit data to socket until the end of test or entire buffer is sent.
 *           - (a) Handle the end of test.
 *           - (b) If test not done, transmit data
 *
 * @param    p_test      Pointer to a test.
 *
 * @param    p_data_buf  Pointer to data to transmit.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES       test     completed,
 *           DEF_NO        test NOT completed.
 *******************************************************************************************************/
static CPU_BOOLEAN IPerf_ClientTx(IPERF_TEST *p_test,
                                  CPU_CHAR   *p_data_buf,
                                  CPU_INT16U retry_max,
                                  CPU_INT32U time_dly_ms,
                                  RTOS_ERR   *p_err)
{
  IPERF_OPT         *p_opt;
  IPERF_CONN        *p_conn;
  IPERF_STATS       *p_stats;
  CPU_INT16U        tx_buf_len;
  CPU_INT16U        tx_len;
  CPU_INT16U        tx_len_tot;
  CPU_INT16U        buf_len;
  CPU_BOOLEAN       test_done;
  IPERF_TS_MS       ts_cur;
  IPERF_TS_MS       ts_max;
  IPERF_TS_MS       ts_ms_delta;
  NET_SOCK_ADDR_LEN addr_len_server;

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;
  p_stats = &p_test->Stats;
  tx_len_tot = 0u;
  test_done = DEF_NO;
  buf_len = p_opt->BufLen;
  addr_len_server = sizeof(p_conn->ServerAddrPort);

  KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

  while ((tx_len_tot < buf_len)                                 // Loop until tx tot len < buf len ...
         && (test_done == DEF_NO) ) {                           // ... & test NOT done.
                                                                // --------------- HANDLING END OF TEST ---------------
    if (p_opt->Duration_ms > 0u) {
      ts_cur = IPerf_Get_TS_ms();                               // Tx until time duration is not reached.
      ts_max = IPerf_Get_TS_Max_ms();
      if (ts_cur >= p_stats->TS_Start_ms) {
        ts_ms_delta = ts_cur - p_stats->TS_Start_ms;
      } else {
        ts_ms_delta = ts_cur + (ts_max - p_stats->TS_Start_ms);
      }

      if ((ts_ms_delta >= (ts_max - 100u))
          || (ts_ms_delta >= p_opt->Duration_ms)) {
        p_stats->TS_End_ms = ts_cur;
        test_done = DEF_YES;
      }
    } else if (p_stats->Bytes >= p_opt->BytesNbr) {
      p_stats->TS_End_ms = IPerf_Get_TS_ms();
      test_done = DEF_YES;
    }

    //                                                             --------------------- TX DATA ----------------------
    if (test_done == DEF_NO) {
      tx_buf_len = buf_len - tx_len_tot;
      p_stats->NbrCalls++;
      tx_len = NetApp_SockTx(p_conn->SockID,
                             p_data_buf,
                             tx_buf_len,
                             NET_SOCK_FLAG_NONE,
                             &p_conn->ServerAddrPort,
                             addr_len_server,
                             retry_max,
                             0,
                             time_dly_ms,
                             p_err);
      if (tx_len > 0) {                                         // If          tx len > 0, ...
        tx_len_tot += tx_len;                                   // ... inc tot tx len & bytes tx'd.
        p_stats->Bytes += tx_len;
      }

      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_NONE:
          break;

        case RTOS_ERR_POOL_EMPTY:
        case RTOS_ERR_NET_IF_LINK_DOWN:
        case RTOS_ERR_TIMEOUT:
        case RTOS_ERR_WOULD_BLOCK:                              // If transitory tx err, ...
          p_stats->TransitoryErrCnts++;                         // ... inc tot TransitoryErrCnts.
          break;

        default:
          p_stats->Errs++;
          test_done = DEF_YES;
          LOG_VRB(("Tx fatal Err : %u\n\r", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
          break;
      }
    }
  }

  KAL_LockRelease(IPerf_LockHandle, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

  return (test_done);
}

/****************************************************************************************************//**
 *                                           IPerf_ClientPattern()
 *
 * @brief    Fill a buffer with standard IPerf buffer pattern.
 *
 * @param    p_buf       Pointer to a buffer to fill.
 *
 * @param    buf_len     Buffer length.
 *******************************************************************************************************/
static void IPerf_ClientPattern(CPU_CHAR   *p_buf,
                                CPU_INT16U buf_len)
{
  CPU_CHAR *p_buf_wr;

  p_buf_wr = p_buf;
  while (buf_len-- > 0u) {
    *p_buf_wr = (CPU_CHAR)((buf_len % 10u) + '0');
    p_buf_wr++;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // IPERF_CLIENT_MODULE_PRESENT
#endif // RTOS_MODULE_NET_IPERF_AVAIL
