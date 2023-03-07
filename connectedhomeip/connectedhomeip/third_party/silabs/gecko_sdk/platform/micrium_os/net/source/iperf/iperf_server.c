/***************************************************************************//**
 * @file
 * @brief Network - IPerf Server
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

#ifdef  IPERF_SERVER_MODULE_PRESENT

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <net/include/net_cfg_net.h>

#ifdef  NET_IPv4_MODULE_EN
#include  <net/include/net_ipv4.h>
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  <net/include/net_ipv6.h>
#endif

#include  <net/include/net_sock.h>
#include  <net/include/net_tcp.h>
#include  <net/include/net_util.h>

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void IPerf_ServerSockInit(IPERF_TEST *p_test,
                                 RTOS_ERR   *p_err);

static void IPerf_ServerTCP(IPERF_TEST *p_test,
                            RTOS_ERR   *p_err);

static void IPerf_ServerRxWinSizeSet(NET_SOCK_ID sock_id,
                                     CPU_INT16U  win_size,
                                     RTOS_ERR    *p_err);

static void IPerf_ServerUDP(IPERF_TEST *p_test,
                            RTOS_ERR   *p_err);

static void IPerf_ServerUDP_FINACK(IPERF_TEST *p_test,
                                   CPU_CHAR   *p_data_buf);

static CPU_BOOLEAN IPerf_ServerRxPkt(IPERF_TEST  *p_test,
                                     NET_SOCK_ID sock_id,
                                     CPU_CHAR    *p_data_buf,
                                     CPU_INT16U  retry_max,
                                     RTOS_ERR    *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           IPerf_ServerStart()
 *
 * @brief    (1) IPerf as a server main loop :
 *               - (a) Initialize socket
 *               - (b) Run IPerf as TCP/UDP server
 *               - (c) Close socket used
 *
 * @param    p_test  Pointer to a test.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void IPerf_ServerStart(IPERF_TEST *p_test,
                       RTOS_ERR   *p_err)
{
  IPERF_OPT                *p_opt;
  IPERF_CONN               *p_conn;
  CPU_BOOLEAN              run;
  NET_SOCK_TYPE            sock_type;
  NET_SOCK_PROTOCOL        sock_protocol;
  NET_SOCK_PROTOCOL_FAMILY addr_family;
  RTOS_ERR                 local_err;

  LOG_VRB(("\n\r------------- IPerf SERVER START -------------\n\r"));
  LOG_VRB(("Init server socket\n\r"));

  KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;

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
    p_test->Err = IPERF_REP_ERR_SERVER_SOCK_OPEN;
    goto exit;
  }
  LOG_VRB(("done.\n\r"));
  //                                                               -------------------- INIT SOCK ----------------------
  IPerf_ServerSockInit(p_test, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    LOG_VRB(("IPerf Server Task : \n\r"));
    run = DEF_YES;
  } else {
    run = DEF_NO;
  }

  //                                                               --------------- RUN IPERF TCP/UDP RX ---------------
  while (run == DEF_YES) {
    p_test->Status = IPERF_TEST_STATUS_RUNNING;

    KAL_LockRelease(IPerf_LockHandle, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
    if (p_opt->Protocol == IPERF_PROTOCOL_TCP) {
      IPerf_ServerTCP(p_test, p_err);
    } else {
      IPerf_ServerUDP(p_test, p_err);
    }

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      run = DEF_NO;
    }

    KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

    if (p_opt->Persistent == DEF_DISABLED) {
      run = DEF_NO;
    }
  }

  //                                                               -------------------- CLOSE SOCK --------------------
  LOG_VRB(("Closing socket.\n\r"));

  if (p_conn->SockID != NET_SOCK_ID_NONE) {
    (void)NetApp_SockClose(p_conn->SockID, 0u, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_VRB(("Sock close error : %u\n\r", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(local_err))));
    }
  }

  KAL_LockRelease(IPerf_LockHandle, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

exit:
  LOG_VRB(("\n\r------------- IPerf SERVER ENDED -------------\n\r"));
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           IPerf_ServerSockInit()
 *
 * @brief    (1) Initialize one socket for sever use :
 *               - (a) Open socket for incoming connection
 *               - (b) Bind socket on any address & current test's options port
 *               - (c) If TCP server, do a socket listen.
 *
 * @param    p_test  Pointer to a test.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void IPerf_ServerSockInit(IPERF_TEST *p_test,
                                 RTOS_ERR   *p_err)
{
  IPERF_OPT            *p_opt;
  IPERF_CONN           *p_conn;
  NET_SOCK_ADDR        sock_addr_server;
  NET_SOCK_ADDR_FAMILY addr_family;
  CPU_INT16U           server_port;
  CPU_BOOLEAN          cfg_succeed;
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR addr_any_ipv4;
#endif

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;

  server_port = p_opt->Port;
  addr_family = ((p_opt->IPv4 == DEF_YES) ? (NET_SOCK_ADDR_FAMILY_IP_V4) : (NET_SOCK_ADDR_FAMILY_IP_V6));

  cfg_succeed = NetSock_CfgBlock(p_conn->SockID, NET_SOCK_BLOCK_SEL_BLOCK, p_err);
  if (cfg_succeed != DEF_OK) {
    p_test->Err = IPERF_REP_ERR_SERVER_SOCK_BIND;
    goto exit;
  }

  //                                                               -------------------- BIND SOCK ---------------------
  Mem_Clr((void *)&sock_addr_server,
          (CPU_SIZE_T) NET_SOCK_ADDR_SIZE);

  switch (addr_family) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V4:
      addr_any_ipv4 = NET_IPv4_ADDR_NONE;
      NetApp_SetSockAddr(&sock_addr_server,
                         NET_SOCK_ADDR_FAMILY_IP_V4,
                         server_port,
                         (CPU_INT08U *)&addr_any_ipv4,
                         NET_IPv4_ADDR_SIZE,
                         p_err);
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V6:

      NetApp_SetSockAddr(&sock_addr_server,
                         NET_SOCK_ADDR_FAMILY_IP_V6,
                         server_port,
                         (CPU_INT08U *)&NetIPv6_AddrAny,
                         NET_IPv6_ADDR_SIZE,
                         p_err);
      break;
#endif
    default:
      p_test->Err = IPERF_REP_ERR_SERVER_SOCK_BIND;
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_test->Err = IPERF_REP_ERR_SERVER_SOCK_BIND;
    goto exit;
  }

  LOG_VRB(("Server Sock Bind ... "));
  (void)NetApp_SockBind(p_conn->SockID,
                        &sock_addr_server,
                        NET_SOCK_ADDR_SIZE,
                        IPERF_BIND_MAX_RETRY,
                        IPERF_BIND_MAX_DLY_MS,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_test->Err = IPERF_REP_ERR_SERVER_SOCK_BIND;
    goto exit;
  }
  Mem_Copy(&p_conn->ServerAddrPort, &sock_addr_server, NET_SOCK_ADDR_SIZE);
  LOG_VRB(("done.\n\r"));

  if (p_opt->Protocol == IPERF_PROTOCOL_TCP) {                  // Only TCP req a sock listen & a TCP Rx window size.
    LOG_VRB(("Server listen ... "));                    // ------------------- SOCK LISTEN --------------------
    (void)NetApp_SockListen(p_conn->SockID,
                            IPERF_SERVER_TCP_CONN_Q_SIZE,
                            p_err);

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      p_test->Err = IPERF_REP_ERR_SERVER_SOCK_LISTEN;
      goto exit;
    }
    //                                                             ----------------- SET WINDOW SIZE ------------------
    LOG_VRB(("Set accepted socket window size... "));
    IPerf_ServerRxWinSizeSet(p_conn->SockID, p_opt->WinSize, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      p_test->Err = IPERF_REP_ERR_SERVER_SOCK_WIN_SIZE;
      goto exit;
    }
    LOG_VRB(("Done\n\r"));
  }

exit:
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_VRB(("Fail error : %u.\n\r", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
  }
  return;
}

/****************************************************************************************************//**
 *                                               IPerf_ServerTCP()
 *
 * @brief    (1) IPerf TCP server :
 *               - (a) Wait for client connection (socket accept incoming connection)
 *               - (b) Set rx window size
 *               - (c) Receive packet until socket close received from client
 *               - (d) Close accepted socket
 *
 * @param    p_test  Pointer to a test.
 *
 *
 * Argument(s) : p_test      Pointer to a test.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
static void IPerf_ServerTCP(IPERF_TEST *p_test,
                            RTOS_ERR   *p_err)
{
  IPERF_CONN        *p_conn;
  CPU_CHAR          *p_data_buf;
  CPU_BOOLEAN       rx_done;
  NET_SOCK_ID       sock_id;
  NET_SOCK_ADDR_LEN addr_len_client;
#if (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED)
  CPU_INT16U cpu_usage;
#endif
#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
  CPU_BOOLEAN rx_started;
  IPERF_TS_MS ts_ms_prev;
  CPU_INT32U  rx_bytes_prev;
#endif
#if ((IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED) \
  || (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED))
  IPERF_STATS *p_stats;
#endif
  CPU_INT16U retry;
  CPU_INT32U timeout;
  CPU_INT32U dly;
  CPU_INT32U rx_timeout;
  RTOS_ERR   local_err;
  CORE_DECLARE_IRQ_STATE;

  KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

  p_conn = &p_test->Conn;
  p_conn->Run = DEF_NO;

  CORE_ENTER_ATOMIC();
  retry = IPerf_CfgPtr->Server.AcceptMaxRetry;
  timeout = IPerf_CfgPtr->Server.AcceptMaxTimeoutMs;
  dly = IPerf_CfgPtr->Server.AcceptMaxDlyMs;
  rx_timeout = IPerf_CfgPtr->Server.TCP_RxMaxTimeoutMs;
  CORE_EXIT_ATOMIC();
  //                                                               ------------------- SOCK ACCEPT --------------------
  LOG_VRB(("TCP Server Accept ... "));

  p_test->Status = IPERF_TEST_STATUS_RUNNING;
  addr_len_client = sizeof(p_conn->ClientAddrPort);
  sock_id = NetApp_SockAccept(p_conn->SockID,
                              &p_conn->ClientAddrPort,
                              &addr_len_client,
                              retry,
                              timeout,
                              dly,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_test->Err = IPERF_REP_ERR_SERVER_SOCK_ACCEPT;
    LOG_VRB(("Error : %u.\n\r", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
    KAL_LockRelease(IPerf_LockHandle, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
    goto exit_close;
  }

  p_conn->SockID_TCP_Server = sock_id;
  LOG_VRB(("Done.\n\r"));

  KAL_LockRelease(IPerf_LockHandle, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

  //                                                               --------------------- RX PKTS ----------------------
  p_data_buf = IPerf_BufPtr;

  NetSock_CfgTimeoutRxQ_Set(sock_id, rx_timeout, &local_err);

#if ((IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED) \
  || (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED))
  p_stats = &p_test->Stats;
#endif

#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
  rx_started = DEF_NO;
#endif
  rx_done = DEF_NO;

  LOG_VRB(("TCP Socket Received start... \n\r"));
  while (rx_done == DEF_NO) {                                   // Loop until sock is closed by the client.
    rx_done = IPerf_ServerRxPkt(p_test,
                                sock_id,
                                p_data_buf,
                                IPERF_SERVER_TCP_RX_MAX_RETRY,
                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_ERR(("IPerf: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
      goto exit_close;
    }

#if ((IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED) \
    || (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED))
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
    if (rx_started == DEF_NO) {
      ts_ms_prev = p_stats->TS_Start_ms;
      rx_bytes_prev = 0u;
      rx_started = DEF_YES;
    }
    IPerf_UpdateBandwidth(p_test, &ts_ms_prev, &rx_bytes_prev);
#endif

    KAL_LockRelease(IPerf_LockHandle, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
#endif
  }

  CORE_ENTER_ATOMIC();
  p_conn->Run = DEF_NO;
  CORE_EXIT_ATOMIC();
  goto exit_close;

exit_close:
  (void)NetApp_SockClose(sock_id, 0u, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    LOG_VRB(("Sock close error : %u\n\r", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(local_err))));
  }

  LOG_VRB(("TCP Socket Received Done\n\rClose socket accepted..."));

  LOG_VRB(("Done\n\r"));;

  return;
}

/****************************************************************************************************//**
 *                                       IPerf_ServerRxWinSizeSet()
 *
 * @brief    (1) Configure receive window size :
 *               - (a) Get socket connection id
 *               - (b) Configure connection receive window size
 *
 * @param    sock_id     Windows size socket ID to change.
 *
 * @param    win_size    New windows size.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void IPerf_ServerRxWinSizeSet(NET_SOCK_ID sock_id,
                                     CPU_INT16U  win_size,
                                     RTOS_ERR    *p_err)
{
  NET_CONN_ID conn_id;

  //                                                               ------------------- GET CONN ID --------------------
  conn_id = NetSock_GetConnTransportID(sock_id, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_VRB(("Setting window size fail with error : %u\n\r", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
    return;
  }
  LOG_VRB(("Setting window size of connection ID : %d\n\r", (d)conn_id));

  //                                                               ------------------- SET WIN SIZE -------------------
  (void)NetTCP_ConnCfgRxWinSize(conn_id, win_size, p_err);
}

/****************************************************************************************************//**
 *                                               IPerf_ServerUDP()
 *
 * @brief    (1) IPerf UDP server :
 *               - (a) Receive UDP packet
 *               - (b) Decode packet & update UDP stat
 *
 * @param    p_test  Pointer to a test.
 *
 *
 * Argument(s) : p_test      Pointer to a test.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
static void IPerf_ServerUDP(IPERF_TEST *p_test,
                            RTOS_ERR   *p_err)
{
  IPERF_STATS        *p_stats;
  IPERF_CONN         *p_conn;
  CPU_CHAR           *p_data_buf;
  IPERF_UDP_DATAGRAM *p_buf;                                    // Ptr to cast the buf on UDP Datagram.
  CPU_INT32U         pkt_ctr;
  CPU_INT32S         pkt_id;
  CPU_BOOLEAN        rx_done;
#if (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED)
  CPU_INT16U cpu_usage;
#endif
#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
  IPERF_TS_MS ts_ms_prev;
  CPU_INT32U  rx_bytes_prev;
#endif
  RTOS_ERR   err;
  CPU_INT32U rx_timeout;
  CORE_DECLARE_IRQ_STATE;

  p_conn = &p_test->Conn;
  p_stats = &p_test->Stats;
  pkt_ctr = 0u;
  pkt_id = 0u;
  p_conn->Run = DEF_NO;
  rx_done = DEF_NO;

  //                                                               ------------------- RX UDP PKTS --------------------

  CORE_ENTER_ATOMIC();
  rx_timeout = IPerf_CfgPtr->Server.UDP_RxMaxTimeoutMs;
  CORE_EXIT_ATOMIC();

  p_data_buf = IPerf_BufPtr;

  NetSock_CfgTimeoutRxQ_Set(p_conn->SockID, rx_timeout, &err);

  p_buf = (IPERF_UDP_DATAGRAM *)p_data_buf;

  LOG_VRB(("UDP Socket Received start\n\r"));          // Loop until end pkt rx'd or sock close from client.
  while (rx_done == DEF_NO) {
    rx_done = IPerf_ServerRxPkt((IPERF_TEST *)p_test,
                                (NET_SOCK_ID)p_conn->SockID,
                                (CPU_CHAR *)p_data_buf,
                                (CPU_INT16U)IPERF_SERVER_UDP_RX_MAX_RETRY,
                                (RTOS_ERR *)p_err);

    KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, &err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

    //                                                             --------- DECODE RXD PKT & UPDATE UDP STATS ---------
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      pkt_ctr++;
      NET_UTIL_VAL_COPY_GET_NET_32(&pkt_id, &p_buf->ID);        // Copy ID from datagram to pkt ID.
      if (pkt_ctr == 1u && pkt_id >= 0) {                       // First pkt rx'd.
        LOG_VRB(("UDP First packet received :  start udp process.\n\r"));
        if (pkt_id > 0) {
          LOG_VRB(("Packet ID not synchronized (first packet not received).\n\r"));
          p_stats->UDP_AsyncErr = DEF_YES;                      // First UDP pkt from client not rx'd.
          p_stats->UDP_LostPkt++;
        }

        p_stats->TS_Start_ms = IPerf_Get_TS_ms();
#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
        ts_ms_prev = p_stats->TS_Start_ms;
        rx_bytes_prev = 0u;
#endif
        p_stats->UDP_RxLastPkt = pkt_id;
      } else if ((pkt_ctr == 1u)
                 && (pkt_id < 0)) {                             // Rx'd old udp fin ack ...
        pkt_ctr = 0u;                                           // ... discard pkt.
        p_conn->Run = DEF_NO;
      } else if (pkt_id < 0) {                                  // Rx'd end pkt.
        p_stats->TS_End_ms = IPerf_Get_TS_ms();
        p_conn->Run = DEF_NO;
        rx_done = DEF_YES;
        IPerf_ServerUDP_FINACK(p_test, p_data_buf);             // Send FINACK.
        LOG_VRB(("UDP Socket Received done\n\r"));

        //                                                         Rx'd pkt id not expected ...
      } else if ((pkt_id != (p_stats->UDP_RxLastPkt + 1))
                 && (pkt_ctr > 1u)                          ) {
        if (pkt_id < p_stats->UDP_RxLastPkt + 1) {              // Pkt out of order; ....
          p_stats->UDP_OutOfOrder++;
        } else {                                                // ... pkts lost.
          p_stats->UDP_LostPkt += (pkt_id - (p_stats->UDP_RxLastPkt + 1u));
        }
      } else if ((pkt_id == p_stats->UDP_AsyncErr)              // Rx'd dup pkt.
                 && (pkt_ctr > 1u)                   ) {
        p_stats->UDP_DupPkt++;
      }

      if ((pkt_ctr > 1u)                                        // Sto pkt id for next pkt decode.
          && (pkt_id > p_stats->UDP_RxLastPkt)) {
        p_stats->UDP_RxLastPkt = pkt_id;
      }
    }

#if (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED)
    cpu_usage = KAL_CPU_UsageGet();
    p_stats->CPU_UsageAvg += cpu_usage;
    p_stats->CPU_CalcNbr++;
    if (cpu_usage > p_stats->CPU_UsageMax) {
      p_stats->CPU_UsageMax = cpu_usage;
    }
#endif

#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
    if (pkt_ctr > 1u) {
      IPerf_UpdateBandwidth(p_test, &ts_ms_prev, &rx_bytes_prev);
    }
#endif

    KAL_LockRelease(IPerf_LockHandle, &err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
  }

  goto exit;

exit:
  return;
}

/****************************************************************************************************//**
 *                                           IPerf_ServerUDP_FINACK()
 *
 * @brief    (1) Server UDP FINACK send loop :
 *               - (a) Set  UDP FINACK server header
 *               - (b) Send UDP FINACK for 10 times or until received UDP FIN
 *               - (c) Try to receive UDP FIN from client
 *
 * @param    p_test      Pointer to a test.
 *
 * @param    p_data_buf  Pointer to data to transmit.
 *******************************************************************************************************/
static void IPerf_ServerUDP_FINACK(IPERF_TEST *p_test,
                                   CPU_CHAR   *p_data_buf)
{
  IPERF_STATS          *p_stats;
  IPERF_CONN           *p_conn;
  IPERF_OPT            *p_opt;
  IPERF_UDP_DATAGRAM   *p_datagram;                             // Ptr to set UDP pkt datagram hdr.
  IPERF_SERVER_UDP_HDR *p_hdr;                                  // Ptr to set UDP pkt server   hdr.
  CPU_INT08U           buf_len;
  CPU_INT08U           tx_ctr;
  CPU_INT16U           tx_err_ctr;
  CPU_INT32S           bytes_received;
  CPU_BOOLEAN          done;
  NET_SOCK_ADDR_LEN    addr_len_client;
  RTOS_ERR             local_err;

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;
  p_stats = &p_test->Stats;

  //                                                               ------------------ SET SERVER HDR ------------------
  p_datagram = (IPERF_UDP_DATAGRAM *) p_data_buf;
  p_hdr = (IPERF_SERVER_UDP_HDR *)p_datagram + 1u;
  p_hdr->Flags = NET_UTIL_HOST_TO_NET_32(IPERF_SERVER_UDP_HEADER_VERSION1);
  p_hdr->TotLen_Hi = 0u;
  p_hdr->TotLen_Lo = NET_UTIL_HOST_TO_NET_32(p_stats->Bytes &  0xFFFFFFFF);
  p_hdr->Stop_sec = NET_UTIL_HOST_TO_NET_32(p_stats->TS_End_ms * 1000u);
  p_hdr->Stop_usec = NET_UTIL_HOST_TO_NET_32(p_stats->TS_End_ms / 1000u);
  p_hdr->LostPkt_ctr = NET_UTIL_HOST_TO_NET_32(p_stats->UDP_LostPkt);
  p_hdr->OutOfOrder_ctr = NET_UTIL_HOST_TO_NET_32(p_stats->UDP_OutOfOrder);
  p_hdr->RxLastPkt = NET_UTIL_HOST_TO_NET_32(p_stats->UDP_RxLastPkt);
  p_hdr->Jitter_Hi = 0u;
  p_hdr->Jitter_Lo = 0u;
  addr_len_client = sizeof(p_conn->ClientAddrPort);
  buf_len = 128u;
  tx_ctr = 0u;
  tx_err_ctr = 0u;
  done = DEF_NO;

  //                                                               ----------------- SEND UDP FINACK ------------------
  LOG_VRB(("Sending FIN ACK :"));
  while (done == DEF_NO) {
    addr_len_client = sizeof(p_conn->ClientAddrPort);

    (void)NetApp_SockTx(p_conn->SockID,
                        p_data_buf,
                        buf_len,
                        NET_SOCK_FLAG_NONE,
                        &p_conn->ClientAddrPort,
                        addr_len_client,
                        IPERF_SERVER_UDP_TX_MAX_RETRY,
                        0,
                        IPERF_SERVER_UDP_TX_MAX_DLY_MS,
                        &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_VRB(("Tx error : %u\n\r", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(local_err))));
      tx_err_ctr++;
      if (tx_err_ctr > IPERF_SERVER_UDP_TX_FINACK_ERR_MAX) {
        p_stats->UDP_EndErr = DEF_YES;
        LOG_VRB(("Tx errors exceed maximum, %u FIN ACK was send\n\r", (u)tx_ctr));
        done = DEF_YES;
      }
      continue;
    }

    tx_ctr++;                                          // Inc tx cnt.
    if (tx_ctr == IPERF_SERVER_UDP_TX_FINACK_COUNT) {
      done = DEF_YES;
    }
    LOG_VRB(("."));
    //                                                             ------------- TRY TO RX CLIENT UDP FIN -------------
    bytes_received = NetApp_SockRx(p_conn->SockID,
                                   p_data_buf,
                                   p_opt->BufLen,
                                   0u,
                                   NET_SOCK_FLAG_NONE,
                                   &p_conn->ClientAddrPort,
                                   &addr_len_client,
                                   0u,
                                   IPERF_RX_UDP_FINACK_MAX_TIMEOUT_MS,
                                   0u,
                                   &local_err);

    if ((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE)
        && (bytes_received > 0)) {
      bytes_received = NetApp_SockRx(p_conn->SockID,
                                     p_data_buf,
                                     p_opt->BufLen,
                                     0u,
                                     NET_SOCK_FLAG_NONE,
                                     &p_conn->ClientAddrPort,
                                     &addr_len_client,
                                     0u,
                                     IPERF_RX_UDP_FINACK_MAX_TIMEOUT_MS,
                                     0u,
                                     &local_err);
      if (bytes_received <= 0) {
        done = DEF_YES;                                         // Conn closed or sock err.
      }
    } else if ((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_RX)
               && (bytes_received == 0u) ) {
      done = DEF_YES;
    }
  }
  LOG_VRB(("End UDP receive process\n\r\n\r"));
}

/****************************************************************************************************//**
 *                                           IPerf_ServerRxPkt()
 *
 * @brief    Receive packet through a socket.
 *
 * @param    p_test      Pointer to a test.
 *
 * @param    sock_id     Socket descriptor/handle identifier of socket to receive data.
 *
 * @param    p_data_buf  Pointer to data to transmit.
 *
 * @param    retry_max   Maximum number of consecutive socket receive retries.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES     Socket     ready to receive      paket,
 *           DEF_NO      Socket NOT ready to receive more paket.
 *******************************************************************************************************/
static CPU_BOOLEAN IPerf_ServerRxPkt(IPERF_TEST  *p_test,
                                     NET_SOCK_ID sock_id,
                                     CPU_CHAR    *p_data_buf,
                                     CPU_INT16U  retry_max,
                                     RTOS_ERR    *p_err)
{
  IPERF_OPT         *p_opt;
  IPERF_CONN        *p_conn;
  IPERF_STATS       *p_stats;
  NET_SOCK_ADDR_LEN addr_len_client;
  CPU_INT16S        rx_len;
  CPU_INT16U        rx_buf_len;
  CPU_BOOLEAN       rx_done;
  CPU_BOOLEAN       rx_server_done;
  RTOS_ERR          err;

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;
  p_stats = &p_test->Stats;
  rx_buf_len = p_opt->BufLen;
  rx_done = DEF_NO;
  rx_server_done = DEF_NO;
  rx_buf_len = p_opt->BufLen;

  while ((rx_done == DEF_NO)
         && (rx_server_done == DEF_NO)) {
    //                                                             --------------- RX PKT THROUGH SOCK ----------------
    addr_len_client = sizeof(p_conn->ClientAddrPort);
    rx_len = NetApp_SockRx(sock_id,
                           p_data_buf,
                           rx_buf_len,
                           0u,
                           NET_SOCK_FLAG_NONE,
                           &p_conn->ClientAddrPort,
                           &addr_len_client,
                           retry_max,
                           0,
                           0,
                           p_err);

    KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, &err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

    p_stats->NbrCalls++;

    p_stats->Bytes += rx_len;
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
      case RTOS_ERR_WOULD_OVF:
        rx_done = DEF_YES;
        if (p_conn->Run == DEF_NO) {
          IPerf_TestClrStats(p_stats);
          p_stats->TS_Start_ms = IPerf_Get_TS_ms();
          p_stats->TS_End_ms = 0u;
          p_conn->Run = DEF_YES;
        }
        break;

      case RTOS_ERR_WOULD_BLOCK:                                // Transitory rx err(s), ...
      case RTOS_ERR_TIMEOUT:
        p_stats->TransitoryErrCnts++;
        break;

      case RTOS_ERR_NET_CONN_CLOSE_RX:
      case RTOS_ERR_NET_CONN_CLOSED_FAULT:                      // Conn closed by peer.
        p_stats->TS_End_ms = IPerf_Get_TS_ms();
        rx_server_done = DEF_YES;
        break;

      default:
        p_stats->Errs++;
        rx_server_done = DEF_YES;
        p_test->Err = IPERF_REP_ERR_SERVER_SOCK_RX;             // Rtn fatal err(s).
        break;
    }

    KAL_LockRelease(IPerf_LockHandle, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
  }

  return (rx_server_done);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // IPERF_SERVER_MODULE_PRESENT
#endif // RTOS_MODULE_NET_IPERF_AVAIL
