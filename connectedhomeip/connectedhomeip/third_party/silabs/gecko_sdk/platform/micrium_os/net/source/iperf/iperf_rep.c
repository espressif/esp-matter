/***************************************************************************//**
 * @file
 * @brief Network - IPerf Reporter
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
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_IPERF_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error IPerf Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "iperf_priv.h"

#include  "../tcpip/net_conn_priv.h"

#include  <net/include/net_conn.h>
#include  <net/include/net_util.h>
#include  <net/include/net_ipv4.h>
#include  <net/include/net_ipv6.h>

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

#define  IPERF_REPORTER_STR_BUF_MAX_LEN      256u
#define  IPERF_REPORTER_MIN_DLY_MS           100u

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_CHAR str_buf[IPERF_REPORTER_STR_BUF_MAX_LEN];

CPU_BOOLEAN cnt_tmr;
CPU_INT08U  ts_tmr_rst_cnts;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void IPerf_ReporterPrintTestOpts(IPERF_TEST      *p_test,
                                        IPERF_OUT_FNCT  p_out_fnct,
                                        IPERF_OUT_PARAM *p_out_param);

static void IPerf_ReporterPrintTestConn(IPERF_TEST      *p_test,
                                        IPERF_OUT_FNCT  p_out_fnct,
                                        IPERF_OUT_PARAM *p_out_param);

static void IPerf_ReporterPrintTestResult(IPERF_TEST      *p_test,
                                          IPERF_TS_MS     *p_prev_ts_ms,
                                          IPERF_TS_MS     *p_interval_ts,
                                          CPU_INT32U      *p_data_bytes_prev,
                                          IPERF_OUT_FNCT  p_out_fnct,
                                          IPERF_OUT_PARAM *p_out_param);

static void IPerf_ReporterPrintTestEndStats(IPERF_TEST      *p_test,
                                            IPERF_OUT_FNCT  p_out_fnct,
                                            IPERF_OUT_PARAM *p_out_param);

static void IPerf_ReporterPrintFmt(CPU_CHAR        *p_fmt_char,
                                   IPERF_OUT_FNCT  p_out_fnct,
                                   IPERF_OUT_PARAM *p_out_param);

static void IPerf_ReporterPrintTS(IPERF_TS_MS     ts_ms,
                                  IPERF_OUT_FNCT  p_out_fnct,
                                  IPERF_OUT_PARAM *p_out_param);

static void IPerf_ReporterPrintErr(IPERF_REP_ERR   err,
                                   IPERF_OUT_FNCT  p_out_fnct,
                                   IPERF_OUT_PARAM *p_out_param);

static void IPerf_ReporterPrintBandwidth(IPERF_TEST      *p_test,
                                         IPERF_TS_MS     ts_ms_delta,
                                         CPU_INT32U      data_bytes,
                                         IPERF_OUT_FNCT  p_out_fnct,
                                         IPERF_OUT_PARAM *p_out_param);

static IPERF_TS_MS IPerf_ReporterGet_TS(IPERF_STATS *p_stats);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          IPerf_Reporter()
 *
 * Description : Prints (in a terminal) the IPerf test results before, during and after the
 *               performance test.
 *
 *
 * Argument(s) : test_id         Test ID of the test to print.
 *
 *               p_out_fnct      Pointer to the string output function.
 *
 *               p_out_param     Pointer to the output function parameters.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
void IPerf_Reporter(IPERF_TEST_ID   test_id,
                    IPERF_OUT_FNCT  p_out_fnct,
                    IPERF_OUT_PARAM *p_out_param)
{
  IPERF_TEST_STATUS status;
  IPERF_TEST        reported_test;
  IPERF_OPT         *p_opt;
  IPERF_CONN        *p_conn;
  IPERF_STATS       *p_stats;
  IPERF_TS_MS       prev_ts;
  IPERF_TS_MS       cur_ts;
  IPERF_TS_MS       delta_ts;
  CPU_INT32U        prev_data_bytes;
  CPU_BOOLEAN       wait;
  CPU_BOOLEAN       done;
  RTOS_ERR          local_err;

  if (p_out_fnct == DEF_NULL) {
    return;
  }

  if (test_id == IPERF_TEST_ID_NONE) {
    return;
  }

  status = IPerf_TestGetStatus(test_id, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    IPerf_TestRelease(test_id, &local_err);
    return;
  }

  while (status == IPERF_TEST_STATUS_QUEUED) {
    KAL_Dly(IPERF_REPORTER_MIN_DLY_MS);
    status = IPerf_TestGetStatus(test_id, &local_err);
  }

  IPerf_TestGetResults(test_id, &reported_test, &local_err);
  if (reported_test.Status == IPERF_TEST_STATUS_ERR) {
    IPerf_ReporterPrintErr(reported_test.Err, p_out_fnct, p_out_param);
    IPerf_TestRelease(test_id, &local_err);
    return;
  }

  IPerf_ReporterPrintTestOpts(&reported_test, p_out_fnct, p_out_param);
  wait = DEF_YES;
  ts_tmr_rst_cnts = 0u;
  p_conn = &reported_test.Conn;

  while (wait == DEF_YES) {
    while ((p_conn->Run != DEF_YES)
           && (reported_test.Status == IPERF_TEST_STATUS_RUNNING)) {
      KAL_Dly(IPERF_REPORTER_MIN_DLY_MS);
      IPerf_TestGetResults(test_id, &reported_test, &local_err);
      if ((reported_test.Status == IPERF_TEST_STATUS_ERR)
          || (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE)        ) {
        IPerf_ReporterPrintErr(reported_test.Err, p_out_fnct, p_out_param);
        IPerf_TestRelease(test_id, &local_err);
        return;
      }
    }

    IPerf_ReporterPrintTestConn(&reported_test, p_out_fnct, p_out_param);
    p_stats = &reported_test.Stats;
    p_opt = &reported_test.Opt;
    prev_ts = 0u;
    prev_data_bytes = 0u;
    done = DEF_NO;

    while (done == DEF_NO) {
      IPerf_TestGetResults(test_id, &reported_test, &local_err);

      if (p_conn->Run == DEF_YES) {
        cur_ts = IPerf_ReporterGet_TS(p_stats);
        delta_ts = cur_ts - prev_ts;

        if (delta_ts >= p_opt->Interval_ms) {
          IPerf_ReporterPrintTestResult(&reported_test,
                                        &prev_ts,
                                        &cur_ts,
                                        &prev_data_bytes,
                                        p_out_fnct,
                                        p_out_param);
        }
      }

      if (reported_test.Status == IPERF_TEST_STATUS_RUNNING) {
        KAL_Dly(IPERF_REPORTER_MIN_DLY_MS);
      } else {
        done = DEF_YES;
      }
    }

    if (reported_test.Status == IPERF_TEST_STATUS_ERR) {
      IPerf_ReporterPrintErr(reported_test.Err, p_out_fnct, p_out_param);
      IPerf_TestRelease(test_id, &local_err);
      return;
    }

    IPerf_ReporterPrintTestEndStats(&reported_test, p_out_fnct, p_out_param);

    if (p_opt->Persistent != DEF_YES) {
      wait = DEF_NO;
    }
  }
  IPerf_TestRelease(test_id, &local_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       IPerf_ReporterGet_TS()
 *
 * Description : Get current time stamp, if test not done else get test end time.
 *
 * Argument(s) : p_stats     Pointer to a test statistics.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static IPERF_TS_MS IPerf_ReporterGet_TS(IPERF_STATS *p_stats)
{
  IPERF_TS_MS cur_ts;
  IPERF_TS_MS ts_max;

  if (p_stats->TS_End_ms != 0u) {
    cur_ts = p_stats->TS_End_ms;
  } else {
    cur_ts = IPerf_Get_TS_ms();
  }

  if (cur_ts > p_stats->TS_Start_ms) {
    cur_ts -= p_stats->TS_Start_ms;
  } else {
    ts_max = IPerf_Get_TS_Max_ms();
    cur_ts += (ts_max - p_stats->TS_Start_ms);
  }

  return (cur_ts);
}

/****************************************************************************************************//**
 *                                    IPerf_ReporterPrintTestOpts()
 *
 * Description : Print test ID and test options.
 *
 * Argument(s) : p_test          Pointer to a test.
 *
 *               p_out_fnct      Pointer to string output function.
 *
 *               p_out_param     Pointer to  output function parameters.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void IPerf_ReporterPrintTestOpts(IPERF_TEST      *p_test,
                                        IPERF_OUT_FNCT  p_out_fnct,
                                        IPERF_OUT_PARAM *p_out_param)
{
  IPERF_OPT            *p_opt;
  IPERF_CONN           *p_conn;
  NET_IF_NBR           if_nbr;
  NET_IP_ADDRS_QTY     addr_tbl_size;
  NET_SOCK_ADDR_FAMILY addr_family;
  RTOS_ERR             local_err;
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR addr_ipv4;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_IPv6_ADDR addr_ipv6;
#endif

  if (p_test == DEF_NULL) {
    return;
  }

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;

  if_nbr = p_conn->IF_Nbr;
  addr_family = ((p_opt->IPv4 == DEF_YES) ? (NET_SOCK_ADDR_FAMILY_IP_V4) : (NET_SOCK_ADDR_FAMILY_IP_V6));

  p_out_fnct("------------------------------------------------------------\r\n", p_out_param);
  p_out_fnct("TEST ID : ", p_out_param);
  (void)Str_FmtNbr_Int32U(p_test->TestID, 3, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
  p_out_fnct(str_buf, p_out_param);
  p_out_fnct("\r\n************************************************************\r\n", p_out_param);
  switch (p_opt->Mode) {
    case IPERF_MODE_SERVER:
      p_opt->Protocol ? p_out_fnct("UDP ", p_out_param) : p_out_fnct("TCP ", p_out_param);

      p_out_fnct("Server listening on ", p_out_param);

      switch (addr_family) {
#ifdef  NET_IPv4_MODULE_EN
        case NET_SOCK_PROTOCOL_FAMILY_IP_V4:
          addr_tbl_size = 1;
          (void)NetIPv4_GetAddrHost(if_nbr,
                                    &addr_ipv4,
                                    &addr_tbl_size,
                                    &local_err);

          if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
            NetASCII_IPv4_to_Str(addr_ipv4,
                                 str_buf,
                                 DEF_NO,
                                 &local_err);
            p_out_fnct(str_buf, p_out_param);
          } else {
            p_out_fnct("?.?.?.?", p_out_param);
          }
          break;
#endif

#ifdef  NET_IPv6_MODULE_EN
        case NET_SOCK_PROTOCOL_FAMILY_IP_V6:
          addr_tbl_size = 1;
          (void)NetIPv6_GetAddrHost(if_nbr,
                                    &addr_ipv6,
                                    &addr_tbl_size,
                                    &local_err);
          if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
            NetASCII_IPv6_to_Str(&addr_ipv6,
                                 str_buf,
                                 DEF_NO,
                                 DEF_NO,
                                 &local_err);
            p_out_fnct(str_buf, p_out_param);
          } else {
            p_out_fnct("?.?.?.?", p_out_param);
          }

          break;
#endif

        default:
          LOG_VRB(("Fail, error: %u.\n\r", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(local_err))));
          return;
      }
      break;

    case IPERF_MODE_CLIENT:
      p_opt->Protocol ? p_out_fnct("UDP ", p_out_param) : p_out_fnct("TCP ", p_out_param);
      p_out_fnct("Client connecting to ", p_out_param);
      p_out_fnct(p_opt->IP_AddrRemote, p_out_param);
      break;

    default:
      break;
  }

  p_out_fnct(" Port ", p_out_param);

  (void)Str_FmtNbr_Int32U(p_opt->Port, 5, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
  p_out_fnct(str_buf, p_out_param);
  p_out_fnct("\r\n", p_out_param);

  switch (p_opt->Protocol) {
    case IPERF_PROTOCOL_TCP:
      p_out_fnct("Window size: ", p_out_param);
      (void)Str_FmtNbr_Int32U(p_opt->WinSize, 5, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
      p_out_fnct(str_buf, p_out_param);
      p_out_fnct(" bytes\n\r", p_out_param);

      p_out_fnct("Buffer size: ", p_out_param);
      (void)Str_FmtNbr_Int32U(p_opt->BufLen, 5, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
      p_out_fnct(str_buf, p_out_param);
      p_out_fnct(" bytes\n\r", p_out_param);
      break;

    case IPERF_PROTOCOL_UDP:
      p_opt->Mode ? p_out_fnct("Transferring ", p_out_param) : p_out_fnct("Receiving ", p_out_param);
      (void)Str_FmtNbr_Int32U(p_opt->BufLen, 5, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
      p_out_fnct(str_buf, p_out_param);
      p_out_fnct(" bytes datagram\n\r", p_out_param);
      break;

    default:
      break;
  }

  p_out_fnct("------------------------------------------------------------\r\n", p_out_param);
}

/****************************************************************************************************//**
 *                                    IPerf_ReporterPrintTestConn()
 *
 * Description : Print the real connection established.
 *
 * Argument(s) : p_test          Pointer to a test.
 *
 *               p_out_fnct      Pointer to string output function.
 *
 *               p_out_param     Pointer to output function parameters.
 *
 * Return(s)   : DEF_YES, IPerf currently     running.
 *               DEF_NO,  IPerf currently NOT running.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void IPerf_ReporterPrintTestConn(IPERF_TEST      *p_test,
                                        IPERF_OUT_FNCT  p_out_fnct,
                                        IPERF_OUT_PARAM *p_out_param)
{
  IPERF_OPT            *p_opt;
  IPERF_CONN           *p_conn;
  NET_IP_ADDRS_QTY     addr_tbl_size;
  NET_SOCK_ADDR_FAMILY addr_family;
  NET_IF_NBR           if_nbr;
  CPU_INT16U           client_port;
  CPU_INT16U           server_port;
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR addr_ipv4;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_IPv6_ADDR addr_ipv6;
#endif
  RTOS_ERR local_err;

  if (p_test == DEF_NULL) {
    return;
  }

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;

  p_out_fnct("Local ", p_out_param);

  if_nbr = p_conn->IF_Nbr;
  addr_family = ((p_opt->IPv4 == DEF_YES) ? (NET_SOCK_ADDR_FAMILY_IP_V4) : (NET_SOCK_ADDR_FAMILY_IP_V6));

  switch (addr_family) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V4:
      addr_tbl_size = 1;
      (void)NetIPv4_GetAddrHost(if_nbr,
                                &addr_ipv4,
                                &addr_tbl_size,
                                &local_err);
      if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
        NetASCII_IPv4_to_Str(addr_ipv4,
                             str_buf,
                             DEF_NO,
                             &local_err);
        p_out_fnct(str_buf, p_out_param);
      } else {
        p_out_fnct("?.?.?.?", p_out_param);
      }
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V6:
      addr_tbl_size = 1;
      (void)NetIPv6_GetAddrHost(if_nbr,
                                &addr_ipv6,
                                &addr_tbl_size,
                                &local_err);
      if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
        NetASCII_IPv6_to_Str(&addr_ipv6,
                             str_buf,
                             DEF_NO,
                             DEF_NO,
                             &local_err);
        p_out_fnct(str_buf, p_out_param);
      } else {
        p_out_fnct("?.?.?.?", p_out_param);
      }
      break;
#endif
    default:
      LOG_VRB(("Fail, invalid IP family.\n\r"));
      return;
  }

  p_out_fnct(" Port ", p_out_param);

  (void)Str_FmtNbr_Int32U(p_opt->Port, 5, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
  p_out_fnct(str_buf, p_out_param);
  p_out_fnct(" connected with ", p_out_param);

  switch (p_opt->Mode) {
    case IPERF_MODE_SERVER:
      switch (addr_family) {
 #ifdef  NET_IPv4_MODULE_EN
        case NET_SOCK_PROTOCOL_FAMILY_IP_V4:
          NET_UTIL_VAL_COPY_GET_NET_32(&addr_ipv4, &p_conn->ClientAddrPort.Addr[NET_CONN_ADDR_IP_V4_IX_ADDR]);
          NetASCII_IPv4_to_Str(addr_ipv4, str_buf, DEF_NO, &local_err);
          break;
#endif

#ifdef  NET_IPv6_MODULE_EN
        case NET_SOCK_PROTOCOL_FAMILY_IP_V6:
          NetASCII_IPv6_to_Str((NET_IPv6_ADDR *)&p_conn->ClientAddrPort.Addr[NET_CONN_ADDR_IP_V6_IX_ADDR],
                               str_buf,
                               DEF_NO,
                               DEF_NO,
                               &local_err);
          break;
#endif

        default:
          LOG_VRB(("Fail, invalid IP family.\n\r"));
          return;
      }

      p_out_fnct(str_buf, p_out_param);
      p_out_fnct(" Port ", p_out_param);

      NET_UTIL_VAL_COPY_GET_NET_16(&client_port, &p_conn->ClientAddrPort.Addr[NET_CONN_ADDR_IP_IX_PORT]);
      (void)Str_FmtNbr_Int32U((CPU_INT32U) client_port,
                              (CPU_INT08U) 5,
                              (CPU_INT08U) DEF_NBR_BASE_DEC,
                              (CPU_CHAR)' ',
                              (CPU_BOOLEAN) DEF_NO,
                              (CPU_BOOLEAN) DEF_YES,
                              (CPU_CHAR *)&str_buf[0]);
      p_out_fnct(str_buf, p_out_param);
      break;

    case IPERF_MODE_CLIENT:
      switch (addr_family) {
 #ifdef  NET_IPv4_MODULE_EN
        case NET_SOCK_PROTOCOL_FAMILY_IP_V4:
          NET_UTIL_VAL_COPY_GET_NET_32(&addr_ipv4, &p_conn->ServerAddrPort.Addr[NET_CONN_ADDR_IP_V4_IX_ADDR]);
          NetASCII_IPv4_to_Str(addr_ipv4, str_buf, DEF_NO, &local_err);
          break;
#endif

#ifdef  NET_IPv6_MODULE_EN
        case NET_SOCK_PROTOCOL_FAMILY_IP_V6:
          NetASCII_IPv6_to_Str((NET_IPv6_ADDR *)&p_conn->ServerAddrPort.Addr[NET_CONN_ADDR_IP_V6_IX_ADDR],
                               str_buf,
                               DEF_NO,
                               DEF_NO,
                               &local_err);
          break;
#endif

        default:
          LOG_VRB(("Fail, invalid IP family.\n\r"));
          return;
      }

      p_out_fnct(str_buf, p_out_param);
      p_out_fnct(" Port ", p_out_param);

      NET_UTIL_VAL_COPY_GET_NET_16(&server_port, &p_conn->ServerAddrPort.Addr[NET_CONN_ADDR_IP_IX_PORT]);
      (void)Str_FmtNbr_Int32U((CPU_INT32U) server_port,
                              (CPU_INT08U) 5,
                              (CPU_INT08U) DEF_NBR_BASE_DEC,
                              (CPU_CHAR)' ',
                              (CPU_BOOLEAN) DEF_NO,
                              (CPU_BOOLEAN) DEF_YES,
                              (CPU_CHAR *)&str_buf[0]);
      p_out_fnct(str_buf, p_out_param);
      break;

    default:
      break;
  }
  p_out_fnct("\r\n", p_out_param);
}

/****************************************************************************************************//**
 *                                   IPerf_ReporterPrintTestResult()
 *
 * Description : Print test result during the test is running.
 *
 * Argument(s) : p_test          Pointer to a test.
 *
 *               p_prev_ts_ms    Pointer to timestamp of the last print in miliseconds.
 *
 *               p_interval_ts   Pointer to interval time in miliseconds between the last print
 *                               and the current print.
 *
 *               p_out_fnct      Pointer to string output function.
 *
 *               p_out_param     Pointer to output function parameters.
 *
 *
 * Return(s)   : DEF_YES, IPerf currently     running.
 *               DEF_NO,  IPerf currently NOT running.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void IPerf_ReporterPrintTestResult(IPERF_TEST      *p_test,
                                          IPERF_TS_MS     *p_prev_ts_ms,
                                          IPERF_TS_MS     *p_cur_ts_ms,
                                          CPU_INT32U      *p_data_bytes_prev,
                                          IPERF_OUT_FNCT  p_out_fnct,
                                          IPERF_OUT_PARAM *p_out_param)
{
  IPERF_OPT   *p_opt;
  IPERF_CONN  *p_conn;
  IPERF_STATS *p_stats;
  IPERF_TS_MS ts_delta;
  CPU_INT32U  data_bytes_delta;

  if ((p_test == DEF_NULL)
      || (p_prev_ts_ms == DEF_NULL)
      || (p_cur_ts_ms == DEF_NULL)
      || (p_data_bytes_prev == DEF_NULL)) {
    return;
  }

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;
  p_stats = &p_test->Stats;

  switch (p_opt->Mode) {
    case IPERF_MODE_SERVER:
      if (p_opt->Protocol == IPERF_PROTOCOL_TCP) {
        p_out_fnct("L[", p_out_param);
        (void)Str_FmtNbr_Int32U((CPU_INT32U) p_conn->SockID,
                                (CPU_INT08U) 2,
                                (CPU_INT08U) DEF_NBR_BASE_DEC,
                                (CPU_CHAR)' ',
                                (CPU_BOOLEAN) DEF_NO,
                                (CPU_BOOLEAN) DEF_YES,
                                (CPU_CHAR *)&str_buf[0]);
        p_out_fnct(str_buf, p_out_param);
        p_out_fnct("] R[", p_out_param);
        (void)Str_FmtNbr_Int32U((CPU_INT32U) p_conn->SockID_TCP_Server,
                                (CPU_INT08U) 2,
                                (CPU_INT08U) DEF_NBR_BASE_DEC,
                                (CPU_CHAR)' ',
                                (CPU_BOOLEAN) DEF_NO,
                                (CPU_BOOLEAN) DEF_YES,
                                (CPU_CHAR *)&str_buf[0]);
        p_out_fnct(str_buf, p_out_param);
        p_out_fnct("] ", p_out_param);
      } else {
        p_out_fnct("R[", p_out_param);
        (void)Str_FmtNbr_Int32U((CPU_INT32U) p_conn->SockID,
                                (CPU_INT08U) 2,
                                (CPU_INT08U) DEF_NBR_BASE_DEC,
                                (CPU_CHAR)' ',
                                (CPU_BOOLEAN) DEF_NO,
                                (CPU_BOOLEAN) DEF_YES,
                                (CPU_CHAR *)&str_buf[0]);
        p_out_fnct(str_buf, p_out_param);
        p_out_fnct("] ", p_out_param);
      }

      break;

    case IPERF_MODE_CLIENT:
      p_out_fnct("T[", p_out_param);
      (void)Str_FmtNbr_Int32U((CPU_INT32U) p_conn->SockID,
                              (CPU_INT08U) 2,
                              (CPU_INT08U) DEF_NBR_BASE_DEC,
                              (CPU_CHAR)' ',
                              (CPU_BOOLEAN) DEF_NO,
                              (CPU_BOOLEAN) DEF_YES,
                              (CPU_CHAR *)&str_buf[0]);
      p_out_fnct(str_buf, p_out_param);
      p_out_fnct("] ", p_out_param);

      break;

    default:
      break;
  }

  IPerf_ReporterPrintTS(*p_prev_ts_ms, p_out_fnct, p_out_param);
  p_out_fnct(" - ", p_out_param);

  IPerf_ReporterPrintTS(*p_cur_ts_ms, p_out_fnct, p_out_param);
  p_out_fnct(" sec : ", p_out_param);

  data_bytes_delta = p_stats->Bytes - *p_data_bytes_prev;
  (void)Str_FmtNbr_Int32U(data_bytes_delta, 10, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
  (void)Str_FmtNbr_Int32U((CPU_INT32U) data_bytes_delta,
                          (CPU_INT08U) 10,
                          (CPU_INT08U) DEF_NBR_BASE_DEC,
                          (CPU_CHAR)' ',
                          (CPU_BOOLEAN) DEF_NO,
                          (CPU_BOOLEAN) DEF_YES,
                          (CPU_CHAR *)&str_buf[0]);
  p_out_fnct(str_buf, p_out_param);
  p_out_fnct(" Bytes, ", p_out_param);

#if (IPERF_CFG_BANDWIDTH_CALC_EN != DEF_ENABLED)
  ts_delta = *p_cur_ts_ms - *p_prev_ts_ms;
#else
  ts_delta = 0u;
#endif

  IPerf_ReporterPrintBandwidth(p_test, ts_delta, data_bytes_delta, p_out_fnct, p_out_param);
  p_out_fnct("\r\n", p_out_param);

  *p_data_bytes_prev = p_stats->Bytes;
  *p_prev_ts_ms = *p_cur_ts_ms;
}

/****************************************************************************************************//**
 *                                   IPerf_ReporterPrintTestStats()
 *
 * Description : Print end result of test.
 *
 * Argument(s) : p_test          Pointer to a test.
 *
 *               p_out_fnct      Pointer to string output function.
 *
 *               p_out_param     Pointer to output function parameters.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void IPerf_ReporterPrintTestEndStats(IPERF_TEST      *p_test,
                                            IPERF_OUT_FNCT  p_out_fnct,
                                            IPERF_OUT_PARAM *p_out_param)
{
  IPERF_OPT   *p_opt;
  IPERF_STATS *p_stats;
  IPERF_TS_MS duration;
  IPERF_TS_MS ts_max;

  if (p_test == DEF_NULL) {
    return;
  }

  p_opt = &p_test->Opt;
  p_stats = &p_test->Stats;

  p_out_fnct("****************** RESULT ******************\r\n", p_out_param);

  if (p_opt->Mode == IPERF_MODE_SERVER) {
    p_out_fnct("Bytes received            = ", p_out_param);
  } else {
    p_out_fnct("Bytes sent                = ", p_out_param);
  }

  (void)Str_FmtNbr_Int32U(p_stats->Bytes, 10, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
  p_out_fnct(str_buf, p_out_param);
  p_out_fnct("\r\n", p_out_param);

  if (p_stats->TS_End_ms > p_stats->TS_Start_ms) {
    duration = (p_stats->TS_End_ms - p_stats->TS_Start_ms);
  } else {
    ts_max = IPerf_Get_TS_Max_ms();
    duration = (p_stats->TS_End_ms + (ts_max - p_stats->TS_Start_ms));
  }

  p_out_fnct("Duration (sec)            =    ", p_out_param);
  IPerf_ReporterPrintTS(duration, p_out_fnct, p_out_param);
  p_out_fnct("\r\n", p_out_param);

  p_out_fnct("Socket Call         count = ", p_out_param);
  (void)Str_FmtNbr_Int32U(p_stats->NbrCalls, 10, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
  p_out_fnct(str_buf, p_out_param);
  p_out_fnct("\r\n", p_out_param);

  p_out_fnct("Transitory error    count = ", p_out_param);
  (void)Str_FmtNbr_Int32U(p_stats->TransitoryErrCnts, 10, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
  p_out_fnct(str_buf, p_out_param);
  p_out_fnct("\r\n", p_out_param);

  if ((p_opt->Mode == IPERF_MODE_SERVER)
      && (p_opt->Protocol == IPERF_PROTOCOL_UDP)) {
    p_out_fnct("Packet lost         count = ", p_out_param);
    (void)Str_FmtNbr_Int32U(p_stats->UDP_LostPkt, 10, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
    p_out_fnct(str_buf, p_out_param);
    p_out_fnct("\r\n", p_out_param);

    p_out_fnct("Out of order packet count = ", p_out_param);
    (void)Str_FmtNbr_Int32U(p_stats->UDP_OutOfOrder, 10, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
    p_out_fnct(str_buf, p_out_param);
    p_out_fnct("\r\n", p_out_param);

    p_out_fnct("Duplicate packet    count = ", p_out_param);
    (void)Str_FmtNbr_Int32U(p_stats->UDP_DupPkt, 10, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
    p_out_fnct(str_buf, p_out_param);
    p_out_fnct("\r\n", p_out_param);

    if (p_stats->UDP_AsyncErr == DEF_YES) {
      p_out_fnct("WARNING : first UDP packet not received\n\r", p_out_param);
    }
  }

#if (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED)
  p_out_fnct("CPU Usage Max             = ", p_out_param);
  (void)Str_FmtNbr_Int32U(p_stats->CPU_UsageMax / 100, 7, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
  p_out_fnct(str_buf, p_out_param);
  p_out_fnct(".", p_out_param);
  (void)Str_FmtNbr_Int32U(p_stats->CPU_UsageMax % 100, 2, DEF_NBR_BASE_DEC, '0', DEF_NO, DEF_YES, &str_buf[0]);
  p_out_fnct(str_buf, p_out_param);
  p_out_fnct(" %\r\n", p_out_param);
  p_out_fnct("CPU Usage Average         = ", p_out_param);
  (void)Str_FmtNbr_Int32U((p_stats->CPU_UsageAvg / p_stats->CPU_CalcNbr) / 100, 7, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
  p_out_fnct(str_buf, p_out_param);
  p_out_fnct(".", p_out_param);
  (void)Str_FmtNbr_Int32U((p_stats->CPU_UsageAvg / p_stats->CPU_CalcNbr) % 100, 2, DEF_NBR_BASE_DEC, '0', DEF_NO, DEF_YES, &str_buf[0]);
  p_out_fnct(str_buf, p_out_param);
  p_out_fnct(" %\r\n", p_out_param);
#endif

  p_out_fnct("Average speed             = ", p_out_param);
  IPerf_ReporterPrintBandwidth(p_test, duration, p_stats->Bytes, p_out_fnct, p_out_param);
  p_out_fnct("\r\n************************************************\r\n", p_out_param);
}

/****************************************************************************************************//**
 *                                      IPerf_ReporterPrintFmt()
 *
 * Description : Print bandwidth format.
 *
 * Argument(s) : p_fmt_char      Pointer to format character.
 *
 *               p_out_fnct      Pointer to string output function.
 *
 *               p_out_param     Pointer to output function parameters.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void IPerf_ReporterPrintFmt(CPU_CHAR        *p_fmt_char,
                                   IPERF_OUT_FNCT  p_out_fnct,
                                   IPERF_OUT_PARAM *p_out_param)
{
  if (p_fmt_char == DEF_NULL) {
    return;
  }

  switch (*p_fmt_char) {
    case IPERF_ASCII_FMT_BITS_SEC:
      p_out_fnct("bits/sec", p_out_param);
      break;

    case IPERF_ASCII_FMT_KBITS_SEC:
      p_out_fnct("Kbits/sec", p_out_param);
      break;

    case IPERF_ASCII_FMT_MBITS_SEC:
      p_out_fnct("Mbits/sec", p_out_param);
      break;

    case IPERF_ASCII_FMT_GBITS_SEC:
      p_out_fnct("Gbits/sec", p_out_param);
      break;

    case IPERF_ASCII_FMT_BYTES_SEC:
      p_out_fnct("bytes/sec", p_out_param);
      break;

    case IPERF_ASCII_FMT_KBYTES_SEC:
      p_out_fnct("Kbytes/sec", p_out_param);
      break;

    case IPERF_ASCII_FMT_MBYTES_SEC:
      p_out_fnct("Mbytes/sec", p_out_param);
      break;

    case IPERF_ASCII_FMT_GBYTES_SEC:
      p_out_fnct("Gbytes/sec", p_out_param);
      break;

    default:
      return;
  }
}

/****************************************************************************************************//**
 *                                       IPerf_ReporterPrintTS()
 *
 * Description : Print timestamp in second format.
 *
 * Argument(s) : p_ts_ms         Pointer to timestamp in millisecond to print.
 *
 *               p_out_fnct      Pointer to string output function.
 *
 *               p_out_param     Pointer to output function parameters.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void IPerf_ReporterPrintTS(IPERF_TS_MS     ts_ms,
                                  IPERF_OUT_FNCT  p_out_fnct,
                                  IPERF_OUT_PARAM *p_out_param)
{
  CPU_INT16U sec;

  sec = ts_ms / DEF_TIME_NBR_mS_PER_SEC;
  (void)Str_FmtNbr_Int32U(sec, 3, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_NO, &str_buf[0]);
  str_buf[3] = '.';

  sec = ts_ms % DEF_TIME_NBR_mS_PER_SEC;
  (void)Str_FmtNbr_Int32U(sec, 3, DEF_NBR_BASE_DEC, '0', DEF_NO, DEF_YES, &str_buf[4]);
  p_out_fnct(str_buf, p_out_param);
}

/****************************************************************************************************//**
 *                                      IPerf_ReporterPrintErr()
 *
 * Description : Print error message.
 *
 * Argument(s) : err             error code to print.
 *
 *               p_out_fnct      Pointer to string output function.
 *
 *               p_out_param     Pointer to output function parameters.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void IPerf_ReporterPrintErr(IPERF_REP_ERR   err,
                                   IPERF_OUT_FNCT  p_out_fnct,
                                   IPERF_OUT_PARAM *p_out_param)
{
  if (p_out_fnct == DEF_NULL) {
    return;
  }

  switch (err) {
    case IPERF_REP_ERR_SERVER_SOCK_OPEN:
      p_out_fnct("Server is unable to open a socket\n\r", p_out_param);
      break;

    case IPERF_REP_ERR_SERVER_SOCK_BIND:
      p_out_fnct("Server is unable to bind the socket\n\r", p_out_param);
      break;

    case IPERF_REP_ERR_SERVER_SOCK_LISTEN:
      p_out_fnct("Server is unabled to listen on a socket\n\r", p_out_param);
      break;

    case IPERF_REP_ERR_SERVER_SOCK_ACCEPT:
      p_out_fnct("Server didn't received client connection\n\r", p_out_param);
      break;

    case IPERF_REP_ERR_SERVER_SOCK_CLOSE:
      p_out_fnct("Server was unable to close a socket\n\r", p_out_param);
      break;

    case IPERF_REP_ERR_SERVER_SOCK_WIN_SIZE:
      p_out_fnct("Server was unable to set the window size\n\r", p_out_param);
      break;

    case IPERF_REP_ERR_CLIENT_SOCK_OPEN:
      p_out_fnct("Client is unable to open a socket\n\r", p_out_param);
      break;

    case IPERF_REP_ERR_CLIENT_SOCK_BIND:
      p_out_fnct("Client is unable to bind the socket\n\r", p_out_param);
      break;

    case IPERF_REP_ERR_CLIENT_SOCK_CONN:
      p_out_fnct("Client is unable to establish a connection with the server\n\r", p_out_param);
      break;

    case IPERF_REP_ERR_CLIENT_SOCK_TX:
      p_out_fnct("Client is unable to transmit\n\r", p_out_param);
      break;

    case IPERF_REP_ERR_CLIENT_SOCK_TX_INV_ARG:
      p_out_fnct("Invalid buffer length argument, make sure your Device "
                 "Large Tx buffer is larger or equal to : \r\n          "
                 "    (IPERF_UDP_BUF_LEN_MAX + NET_BUF_DATA_IX_TX)\r\n  ", p_out_param);
      break;

    case IPERF_REP_ERR_CLIENT_SOCK_CLOSE:
      p_out_fnct("Client was unable to close a socket\n\r", p_out_param);
      break;

    case IPERF_REP_ERR_SERVER_SOCK_RX:
      p_out_fnct("Server failed to received.\n\r", p_out_param);
      break;

    default:
      p_out_fnct("Unknow error : ", p_out_param);
      (void)Str_FmtNbr_Int32U(err, 5, DEF_NBR_BASE_DEC, '0', DEF_NO, DEF_YES, &str_buf[4]);
      p_out_fnct(str_buf, p_out_param);
      p_out_fnct("\r\n", p_out_param);
      break;
  }
}

/****************************************************************************************************//**
 *                                    IPerf_ReporterGetBandwidth()
 *
 * Description : Get bandwidth calculation.
 *
 * Argument(s) : p_fmt_char      Pointer to a compatible format char.
 *
 *               p_ts_ms_delta   Pointer to the delta time for the bandwidth calculation.
 *
 *               p_data_bytes    Pointer to the amount of data in bytes for the bandwidth calculation.
 *
 *               p_out_fnct      Pointer to string output function.
 *
 *               p_out_param     Pointer to output function parameters.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void IPerf_ReporterPrintBandwidth(IPERF_TEST      *p_test,
                                         IPERF_TS_MS     ts_ms_delta,
                                         CPU_INT32U      data_bytes,
                                         IPERF_OUT_FNCT  p_out_fnct,
                                         IPERF_OUT_PARAM *p_out_param)
{
  IPERF_OPT *p_opt;
#if (IPERF_CFG_BANDWIDTH_CALC_EN != DEF_ENABLED)
  CPU_INT32U data_fmtd;
  CPU_INT32U bandwidth;
  CPU_INT32U bandwidth_X;
  CPU_INT32U bandwidth_Y;
#else
  IPERF_STATS *p_stats;
#endif

  p_opt = &p_test->Opt;

#if (IPERF_CFG_BANDWIDTH_CALC_EN != DEF_ENABLED)
  //                                                               Conv data in bytes to the right fmt.
  switch (p_opt->Fmt) {
    case IPERF_ASCII_FMT_MBITS_SEC:
      data_fmtd = IPerf_GetDataFmtd(IPERF_ASCII_FMT_KBITS_SEC, data_bytes);
      break;

    case IPERF_ASCII_FMT_GBITS_SEC:
      data_fmtd = IPerf_GetDataFmtd(IPERF_ASCII_FMT_MBITS_SEC, data_bytes);
      break;

    case IPERF_ASCII_FMT_MBYTES_SEC:
      data_fmtd = IPerf_GetDataFmtd(IPERF_ASCII_FMT_KBYTES_SEC, data_bytes);
      break;

    case IPERF_ASCII_FMT_GBYTES_SEC:
      data_fmtd = IPerf_GetDataFmtd(IPERF_ASCII_FMT_MBYTES_SEC, data_bytes);
      break;

    case IPERF_ASCII_FMT_BITS_SEC:
    case IPERF_ASCII_FMT_KBITS_SEC:
    case IPERF_ASCII_FMT_BYTES_SEC:
    case IPERF_ASCII_FMT_KBYTES_SEC:
    default:
      data_fmtd = IPerf_GetDataFmtd(p_opt->Fmt, data_bytes);
      break;
  }

  switch (p_opt->Fmt) {
    case IPERF_ASCII_FMT_MBITS_SEC:
    case IPERF_ASCII_FMT_GBITS_SEC:
    case IPERF_ASCII_FMT_MBYTES_SEC:
    case IPERF_ASCII_FMT_GBYTES_SEC:
      if (ts_ms_delta != 0) {
        bandwidth = (data_fmtd * DEF_TIME_NBR_mS_PER_SEC) / ts_ms_delta;
      } else {
        bandwidth = 0;
      }

      bandwidth_X = bandwidth / 1000u;
      (void)Str_FmtNbr_Int32U(bandwidth_X, 6, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_NO, &str_buf[0]);
      str_buf[6] = '.';

      bandwidth_Y = bandwidth % 1000u;
      (void)Str_FmtNbr_Int32U(bandwidth_Y, 3, DEF_NBR_BASE_DEC, '0', DEF_NO, DEF_YES, &str_buf[7]);
      break;

    case IPERF_ASCII_FMT_BITS_SEC:
    case IPERF_ASCII_FMT_KBITS_SEC:
    case IPERF_ASCII_FMT_BYTES_SEC:
    case IPERF_ASCII_FMT_KBYTES_SEC:
    default:
      if (ts_ms_delta != 0) {
        bandwidth = (data_fmtd * DEF_TIME_NBR_mS_PER_SEC) / ts_ms_delta;
      } else {
        bandwidth = 0;
      }
      (void)Str_FmtNbr_Int32U(bandwidth, 10, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);
      break;
  }

#else
  p_stats = &p_test->Stats;
  (void)Str_FmtNbr_Int32U(p_stats->Bandwidth, 6, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_YES, &str_buf[0]);

  PP_UNUSED_PARAM(ts_ms_delta);                                 // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(data_bytes);
#endif

  p_out_fnct(str_buf, p_out_param);
  p_out_fnct(" ", p_out_param);
  IPerf_ReporterPrintFmt(&p_opt->Fmt, p_out_fnct, p_out_param);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_IPERF_AVAIL
