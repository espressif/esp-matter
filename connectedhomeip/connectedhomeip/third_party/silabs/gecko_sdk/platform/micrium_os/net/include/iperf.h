/***************************************************************************//**
 * @file
 * @brief Network - Iperf Module
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

/****************************************************************************************************//**
 * @defgroup NET_IPERF IPerf API
 * @ingroup  NET
 * @brief    IPerf API
 *
 * @addtogroup NET_IPERF
 * @{
 ********************************************************************************************************
 * @note     (1) Micrium IPerf is designed to work  with NLANR IPerf 2.0.2 or higher.
 *               Micrium IPerf should be compatible with kPerf or jPerf using IPerf 2.0.2 or higher.
 *               (a) Supports NLANR IPerf with the following restrictions/constraints :
 *                   - TCP:
 *                      - (A) Multi-threaded                                NOT supported on both   mode
 *                      - (B) Measure bandwith                                  Supported on both   mode
 *                      - (C) Report MSS/MTU size & observed read sizes     NOT supported on both   mode
 *                      - (D) Support for TCP window size via socket buffer     Supported on server mode
 *                   - UDP:
 *                     - (A) Multi-threaded                                NOT supported on both   mode
 *                     - (B) Create UDP streams of specified bandwidth     NOT supported on client mode
 *                     - (C) Measure packet loss                               Supported on server mode
 *                     - (D) Measure delay jitter                          NOT supported on both   mode
 *                     - (E) Multicast capable                             NOT supported on both   mode
 *               More information about NLANR IPerf can be obtained online at
 *                   http://www.onl.wustl.edu/restricted/iperf.html.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _IPERF_H_
#define  _IPERF_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef   IPERF_MODULE
#define  IPERF_EXT
#else
#define  IPERF_EXT  extern
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <common/include/rtos_types.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_def.h>
#include  <common/include/lib_str.h>

#include  <iperf_cfg.h>

#include  <net/include/net.h>
#include  <net/include/net_tcp.h>
#include  <net/include/net_app.h>
#include  <net/include/net_ascii.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                        MODULE CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#if     (IPERF_CFG_SERVER_EN == DEF_ENABLED)
#define  IPERF_SERVER_MODULE_PRESENT
#endif

#if     (IPERF_CFG_CLIENT_EN == DEF_ENABLED)
#define  IPERF_CLIENT_MODULE_PRESENT
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  IPERF_CFG_BUF_LEN_DFLT                                 8192u
#define  IPERF_CFG_TEST_NBR_MAX_DFLT                            2u
#define  IPERF_CFG_SERVER_ACCEPT_DLY_MAX_MS_DFLT                500u
#define  IPERF_CFG_SERVER_ACCEPT_RETRY_NBR_MAX_DFLT             10u
#define  IPERF_CFG_SERVER_ACCEPT_TIMEOUT_MAX_MS_DFLT            5000u
#define  IPERF_CFG_SERVER_TCP_RX_TIMEOUT_MAX_MS_DFLT            5000u
#define  IPERF_CFG_SERVER_UDP_RX_TIEMOUT_MAX_MS_DFLT            5000u
#define  IPERF_CFG_CLIENT_CONN_DLY_MAX_MS_DFLT                  500u
#define  IPERF_CFG_CLIENT_CONN_RETRY_NBR_MAX_DFLT               10u
#define  IPERF_CFG_CLIENT_CONN_TIMEOUT_MAX_MS_DFLT              5000u
#define  IPERF_CFG_CLIENT_TCP_TX_TIMEOUT_MAX_MS_DFLT            5000u

#define  IPERF_TASK_CFG_STK_SIZE_ELEMENTS_DFLT                  512u
#define  IPERF_TASK_CFG_STK_PTR_DFLT                            DEF_NULL

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  enum  iperf_rep_err {
  IPERF_REP_ERR_NONE,
  IPERF_REP_ERR_SERVER_SOCK_BIND,
  IPERF_REP_ERR_SERVER_SOCK_OPEN,
  IPERF_REP_ERR_SERVER_SOCK_CLOSE,
  IPERF_REP_ERR_SERVER_SOCK_LISTEN,
  IPERF_REP_ERR_SERVER_SOCK_ACCEPT,
  IPERF_REP_ERR_SERVER_SOCK_RX,
  IPERF_REP_ERR_SERVER_SOCK_WIN_SIZE,
  IPERF_REP_ERR_CLIENT_SOCK_OPEN,
  IPERF_REP_ERR_CLIENT_SOCK_BIND,
  IPERF_REP_ERR_CLIENT_SOCK_CONN,
  IPERF_REP_ERR_CLIENT_SOCK_TX,
  IPERF_REP_ERR_CLIENT_SOCK_TX_INV_ARG,
  IPERF_REP_ERR_CLIENT_SOCK_CLOSE,
}IPERF_REP_ERR;

/********************************************************************************************************
 *                                         IPERF MODE DATA TYPE
 * @brief IPerf mode
 *******************************************************************************************************/

typedef  enum  iperf_mode {
  IPERF_MODE_SERVER = 0u,                                       ///< Server IPerf mode.
  IPERF_MODE_CLIENT = 1u                                        ///< Client IPerf mode.
} IPERF_MODE;

/********************************************************************************************************
 *                                      IPERF PROTOCOL DATA TYPE
 * @brief IPerf protocol
 *******************************************************************************************************/

typedef  enum  iperf_protocol {
  IPERF_PROTOCOL_TCP = 0u,                                      ///< IPerf opt protocol TCP.
  IPERF_PROTOCOL_UDP = 1u                                       ///< IPerf opt protocol UDP.
} IPERF_PROTOCOL;

/********************************************************************************************************
 *                                       IPERF TEST ID DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT16U IPERF_TEST_ID;

#define  IPERF_TEST_ID_NONE                                0u   // Null IPerf test ID.
#define  IPERF_TEST_ID_INIT                                1u   // First test ID, set at init.

/********************************************************************************************************
 *                                     IPERF TEST STATUS DATA TYPE
 * @brief IPerf test status
 *******************************************************************************************************/

typedef  enum  iperf_status {
  IPERF_TEST_STATUS_FREE = 0u,                                  ///< Test unused.
  IPERF_TEST_STATUS_QUEUED = 1u,                                ///< Test Q'd
  IPERF_TEST_STATUS_RUNNING = 2u,                               ///< Test currently run.
  IPERF_TEST_STATUS_DONE = 3u,                                  ///< Test done      with no err.
  IPERF_TEST_STATUS_ERR = 4u,                                   ///< Test done      with    err.
} IPERF_TEST_STATUS;

/********************************************************************************************************
 *                                      IPERF TIMESTAMP DATA TYPE
 *******************************************************************************************************/

typedef  CPU_TS32 IPERF_TS_MS;

#define  IPERF_TS_MS_MAX_VAL  DEF_INT_32U_MAX_VAL

/********************************************************************************************************
 *                                  IPERF DATA FORMAT UNIT DATA TYPE
 *******************************************************************************************************/

typedef  CPU_CHAR IPERF_FMT;

/********************************************************************************************************
 *                                  IPERF COMMAND PARAMETER DATA TYPE
 * @brief IPerf command parameter
 *
 * @note (1) This structure is used to pass additional parameters to the output function.
 *
 * @note (2) This variable is not used by IPerf, this variable is present to be compliant with shell.
 *******************************************************************************************************/

typedef  struct  IPERF_OUT_PARAM {
  void        *p_cur_working_dir;                               ///< Cur working dir ptr.
  void        *p_out_opt;                                       ///< Output opt      ptr.
  CPU_BOOLEAN *p_session_active;                                ///< Session status flag (see Note #2).
} IPERF_OUT_PARAM;

/********************************************************************************************************
 *                              IPERF COMMAND FUNCTION POINTER DATA TYPE
 *******************************************************************************************************/

typedef  void (*IPERF_OUT_FNCT)(CPU_CHAR *,
                                IPERF_OUT_PARAM *);

/********************************************************************************************************
 *                                     IPERF TEST OPTION DATA TYPE
 * @brief IPerf test option
 *******************************************************************************************************/

typedef  struct  iperf_opt {
  IPERF_MODE     Mode;                                                  ///< Server or client mode.
  IPERF_PROTOCOL Protocol;                                              ///< UDP    or TCP protocol.
  CPU_INT16U     Port;                                                  ///< Server or client port.
  CPU_BOOLEAN    IPv4;                                                  ///< IPv4 or IPv6  protocol.
  CPU_CHAR       IP_AddrRemote[NET_ASCII_LEN_MAX_ADDR_IP + 1u];         ///< IP Addr Remote to tx.
  CPU_INT16U     BytesNbr;                                              ///< Nbr of bytes   to tx.
  CPU_INT16U     BufLen;                                                ///< Buf len        to tx or rx.
  CPU_INT16U     Duration_ms;                                           ///< Time in sec    to tx.
  CPU_INT16U     WinSize;                                               ///< Win size       to tx or rx.
  CPU_BOOLEAN    Persistent;                                            ///< Server in persistent mode.
  IPERF_FMT      Fmt;                                                   ///< Result rate fmt.
  CPU_INT16U     Interval_ms;                                           ///< Interval (ms) between bandwidth update.
} IPERF_OPT;

/********************************************************************************************************
 *                                     IPERF STATISTICS DATA TYPE
 * @brief IPerf statistics
 *******************************************************************************************************/

typedef  struct  iperf_stats {
  CPU_INT32U  NbrCalls;                                         ///< Nbr of I/O sys calls.

  CPU_INT32U  Bytes;                                            ///< Nbr of bytes rx'd or tx'd on net.
  CPU_INT32U  Errs;                                             ///< Nbr of       rx   or tx  errs.
  CPU_INT32U  TransitoryErrCnts;                                ///< Nbr of transitory err.

  CPU_INT32S  UDP_RxLastPkt;                                    ///< Prev         pkt ID rx'd
  CPU_INT32U  UDP_LostPkt;                                      ///< Nbr  of  UDP pkt lost
  CPU_INT32U  UDP_OutOfOrder;                                   ///< Nbr  of      pkt    rx'd out of order.
  CPU_INT32U  UDP_DupPkt;                                       ///< Nbr  of      pkt ID rx'd more than once.
  CPU_BOOLEAN UDP_AsyncErr;                                     ///< First    UDP pkt    rx'd.
  CPU_BOOLEAN UDP_EndErr;                                       ///< Err with UDP FIN or FINACK.

  IPERF_TS_MS TS_Start_ms;                                      ///< Start timestamp (ms).
  IPERF_TS_MS TS_End_ms;                                        ///< End   timestamp (ms).

#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
  CPU_INT32U  Bandwidth;                                        ///< Rx or Tx cur bandwidth.
#endif

#if (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED)
  CPU_INT32U CPU_UsageMax;                                      ///< Max CPU usage reached.
  CPU_INT32U CPU_UsageAvg;
  CPU_INT32U CPU_CalcNbr;
#endif
} IPERF_STATS;

/********************************************************************************************************
 *                              IPERF LOCAL & REMOTE CONNECTION DATA TYPE
 * @brief IPerf connection
 *******************************************************************************************************/

typedef  struct  iperf_conn {
  NET_SOCK_ID   SockID;                                         ///< Sock          used by server and client.
  NET_SOCK_ID   SockID_TCP_Server;                              ///< Accepted sock used by TCP server to rx.
  NET_SOCK_ADDR ServerAddrPort;                                 ///< Server sock addr IP.
  NET_SOCK_ADDR ClientAddrPort;                                 ///< Client sock addr IP.
  NET_IF_NBR    IF_Nbr;                                         ///< Local IF to tx or rx.
  CPU_BOOLEAN   Run;                                            ///< Server (rx'd) or client (tx'd) started.
} IPERF_CONN;

/*
 ********************************************************************************************************
 *                                        IPERF TEST DATA TYPE
 *******************************************************************************************************/

typedef  struct  iperf_test IPERF_TEST;

struct  iperf_test {
  IPERF_TEST_ID     TestID;                                     ///< IPerf test         ID.
  IPERF_TEST_STATUS Status;                                     ///< IPerf cur          status.
  IPERF_REP_ERR     Err;                                        ///< IPerf err          storage.
  IPERF_OPT         Opt;                                        ///< IPerf test         opt data.
  IPERF_STATS       Stats;                                      ///< IPerf stats/result     data.
  IPERF_CONN        Conn;                                       ///< IPerf conn             data.
  IPERF_TEST        *PrevPtr;                                   ///< Ptr to PREV IPerf test.
  IPERF_TEST        *NextPtr;                                   ///< Ptr to NEXT IPerf test.
};

typedef  struct  iperf_server_cfg {
  CPU_INT16U AcceptMaxRetry;                                    ///< Configure server maximum of    retries   on accept.
  CPU_INT32U AcceptMaxDlyMs;                                    ///< Configure server delay between retries   on accept.
  CPU_INT32U AcceptMaxTimeoutMs;                                ///< Configure server maximum inactivity time on accept.
  CPU_INT32U TCP_RxMaxTimeoutMs;                                ///< Configure server maximum inactivity time on TCP Rx.
  CPU_INT32U UDP_RxMaxTimeoutMs;                                ///< Configure server maximum inactivity time on UDP Rx.
} IPERF_SERVER_CFG;

typedef  struct  iperf_client_cfg {
  CPU_INT16U ConnMaxRetry;                                      ///< Configure client maximum of    retries   on connect.
  CPU_INT32U ConnMaxDlyMs;                                      ///< Configure client delay between retries   on connect.
  CPU_INT32U ConnMaxTimeoutMs;                                  ///< Configure client maximum inactivity time on connect.
  CPU_INT32U TCP_TxMaxTimeoutMs;                                ///< Configure client maximum inactivity time on TCP Tx.
} IPERF_CLIENT_CFG;

typedef  struct  iperf_cfg {
  CPU_INT32U       BufLen;                                      ///< Configure maximum buffer size used to send/receive.
  CPU_INT16U       TestNbrMax;                                  ///< Configure the maximum number of tests (See Note #2).

  IPERF_SERVER_CFG Server;

  IPERF_CLIENT_CFG Client;
} IPERF_CFG;

typedef  struct  iperf_init_cfg {
  MEM_SEG *MemSegPtr;                                           ///< Pointer to the IPerf server memory segment.
} IPERF_INIT_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const IPERF_INIT_CFG IPerf_InitCfgDflt;
#endif

extern const IPERF_CFG     IPerf_CfgDflt;
extern const RTOS_TASK_CFG IPerf_TaskCfgDflt;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void IPerf_ConfigureMemSeg(MEM_SEG *p_mem_seg);

void IPerf_ConfigureCfg(const IPERF_CFG *p_cfg);

void IPerf_ConfigureTaskStk(CPU_STK_SIZE stk_size_elements,
                            void         *p_stk_base);

void IPerf_TaskPrioSet(RTOS_TASK_PRIO prio,
                       RTOS_ERR       *p_err);
#endif

void IPerf_Init(RTOS_ERR *p_err);

IPERF_TEST_ID IPerf_TestStart(CPU_CHAR        *argv,
                              IPERF_OUT_FNCT  p_out_fnct,
                              IPERF_OUT_PARAM *p_out_opt,
                              RTOS_ERR        *p_err);

void IPerf_TestRelease(IPERF_TEST_ID test_id,
                       RTOS_ERR      *p_err);

IPERF_TEST_STATUS IPerf_TestGetStatus(IPERF_TEST_ID test_id,
                                      RTOS_ERR      *p_err);

void IPerf_TestGetResults(IPERF_TEST_ID test_id,
                          IPERF_TEST    *p_test_result,
                          RTOS_ERR      *p_err);

void IPerf_Reporter(IPERF_TEST_ID   test_id,
                    IPERF_OUT_FNCT  p_out_fnct,
                    IPERF_OUT_PARAM *p_out_opt);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                     IPERF CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  IPERF_CFG_SERVER_EN
    #error  "IPERF_CFG_SERVER_EN not #define'd in 'iperf_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"
#elif  ((IPERF_CFG_SERVER_EN != DEF_ENABLED) \
  && (IPERF_CFG_SERVER_EN != DEF_DISABLED))
    #error  "IPERF_CFG_SERVER_EN illegally #define'd in 'iperf_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED] "
#endif

#ifndef  IPERF_CFG_CLIENT_EN
    #error  "IPERF_CFG_CLIENT_EN not #define'd in 'iperf_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"
#elif  ((IPERF_CFG_CLIENT_EN != DEF_ENABLED) \
  && (IPERF_CFG_CLIENT_EN != DEF_DISABLED))
    #error  "IPERF_CFG_CLIENT_EN illegally #define'd in 'iperf_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"
#endif

#if    ((IPERF_CFG_SERVER_EN != DEF_ENABLED) \
  && (IPERF_CFG_CLIENT_EN != DEF_ENABLED))
    #error  "IPERF_CFG_SERVER_EN and/or IPERF_CFG_CLIENT_EN illegally #define'd in 'iperf_cfg.h'."
#endif

#ifndef  IPERF_CFG_BANDWIDTH_CALC_EN
    #error  "IPERF_CFG_BANDWIDTH_CALC_EN not #define'd in 'iperf_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"
#elif  ((IPERF_CFG_BANDWIDTH_CALC_EN != DEF_ENABLED) \
  && (IPERF_CFG_BANDWIDTH_CALC_EN != DEF_DISABLED))
    #error  "IPERF_CFG_BANDWIDTH_CALC_EN illegally #define'd in 'iperf_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"
#endif

#ifndef  IPERF_CFG_CPU_USAGE_MAX_CALC_EN
    #error  "IPERF_CFG_CPU_USAGE_MAX_CALC_EN not #define'd in 'iperf_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"
#elif  ((IPERF_CFG_CPU_USAGE_MAX_CALC_EN != DEF_ENABLED) \
  && (IPERF_CFG_CPU_USAGE_MAX_CALC_EN != DEF_DISABLED))
    #error  "IPERF_CFG_CPU_USAGE_MAX_CALC_EN illegally #define'd in 'iperf_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"
#endif

/********************************************************************************************************
 *                                      CPU CONFIGURATION ERRORS
 *******************************************************************************************************/

#if     (CPU_CFG_TS_EN != DEF_ENABLED)
    #error  "CPU_CFG_TS_32_EN || CPU_CFG_TS_64_EN illegally #define'd in 'cpu_cfg.h' [MUST be DEF_ENABLED]"
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _IPERF_H_
