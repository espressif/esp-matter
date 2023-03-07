/***************************************************************************//**
 * @file
 * @brief Network - SNTPc Client Module
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

#if (defined(RTOS_MODULE_NET_SNTP_CLIENT_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error SNTP Client Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>

#include  <net/include/net_sock.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_app.h>
#include  <net/include/net_util.h>

#include  <net/include/sntp_client.h>

#include  <net/source/tcpip/net_sock_priv.h>
#include  <net/source/cmd/sntp_client_cmd_priv.h>

#include  <common/source/kal/kal_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SNTP_TS_SEC_FRAC_SIZE      4294967296u                   // Equivalent to 2^32.

#define  SNTP_MS_NBR_PER_SEC        1000u                         // Nb of ms in a second.

#define  LOG_DFLT_CH               (NET, SNTP)
#define  RTOS_MODULE_CUR            RTOS_CFG_MODULE_NET

/********************************************************************************************************
 *                                           SNTPc MESSAGE DEFINES
 *
 * Note(s) : (1) See 'SNTPc MESSAGE DATA TYPE  Note #2' for flag fields.
 *******************************************************************************************************/

#define  SNTPc_MSG_FLAG_SHIFT                             24u

#define  SNTPc_MSG_FLAG_LI_SHIFT                           6u
#define  SNTPc_MSG_FLAG_VN_SHIFT                           3u

/********************************************************************************************************
 *                                   SNTPc MESSAGE LEAP INDICATOR DEFINES
 *******************************************************************************************************/

#define  SNTPc_MSG_LI_NO_WARNING                           0u
#define  SNTPc_MSG_LI_LAST_MIN_61                          1u
#define  SNTPc_MSG_LI_LAST_MIN_59                          2u
#define  SNTPc_MSG_LI_ALARM_CONDITION                      3u

/********************************************************************************************************
 *                                           SNTPc MESSAGE VERSION
 *******************************************************************************************************/

#define  SNTPc_MSG_VER_4                                   4u

/********************************************************************************************************
 *                                           SNTPc MESSAGE MODE
 *******************************************************************************************************/

#define  SNTPc_MSG_MODE_RESERVED                           0u
#define  SNTPc_MSG_MODE_SYM_ACT                            1u
#define  SNTPc_MSG_MODE_SYM_PAS                            2u
#define  SNTPc_MSG_MODE_CLIENT                             3u
#define  SNTPc_MSG_MODE_SERVER                             4u
#define  SNTPc_MSG_MODE_BROADCAST                          5u
#define  SNTPc_MSG_MODE_RESERVED_NTP_CTRL_MSG              6u
#define  SNTPc_MSG_MODE_RESERVED_PRIVATE                   7u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  sntp {
  NET_PORT_NBR       ServerPortNbrDflt;
  NET_IP_ADDR_FAMILY ServerAddrFamilyDflt;
  CPU_INT32U         ReqRxTimeout_msDflt;
  KAL_LOCK_HANDLE    LockHandle;
} SNTPc;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SNTPc *SNTPc_Ptr = DEF_NULL;

/********************************************************************************************************
 *                                           CONFIGURATIONS
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const SNTPc_INIT_CFG  SNTPc_InitCfgDflt = {
  .MemSegPtr = DEF_NULL
};
static SNTPc_INIT_CFG SNTPc_InitCfg = {
  .MemSegPtr = DEF_NULL
};
#else
extern const SNTPc_INIT_CFG SNTPc_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void SNTPc_Rx(NET_SOCK_ID sock,
                     CPU_INT32U  timeout_ms,
                     SNTP_PKT    *ppkt,
                     RTOS_ERR    *p_err);

static void SNTPc_Tx(NET_SOCK_ID   sock,
                     NET_SOCK_ADDR *paddr,
                     RTOS_ERR      *p_err);

static void SNTPc_AcquireLock(void);

static void SNTPc_ReleaseLock(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           SNTPc_ConfigureMemSeg()
 *
 * @brief    Configures the memory segment to use when allocating control data and buffers.
 *
 * @param    p_mem_seg   Pointer to memory segment to use when allocating control data.
 *                       DEF_NULL means general purpose heap segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the SNTPc client is initialized via the
 *               SNTPc_Init() function.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SNTPc_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_CRITICAL((SNTPc_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  SNTPc_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                               SNTPc_Init()
 *
 * @brief    Initializes the SNTPc Module.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void SNTPc_Init(RTOS_ERR *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  SNTPc_Ptr = (SNTPc *)Mem_SegAlloc("SNTPc Root struct",
                                    SNTPc_InitCfg.MemSegPtr,
                                    sizeof(SNTPc),
                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Create the module's lock.
  SNTPc_Ptr->LockHandle = KAL_LockCreate("SNTPc Lock",
                                         DEF_NULL,
                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
  SNTPcCmd_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  //                                                               Prepare dflt cfg.
  SNTPc_Ptr->ServerPortNbrDflt = SNTP_CLIENT_CFG_SERVER_PORT_NBR_DFLT;
  SNTPc_Ptr->ServerAddrFamilyDflt = SNTP_CLIENT_CFG_SERVER_ADDR_FAMILY_DFLT;
  SNTPc_Ptr->ReqRxTimeout_msDflt = SNTP_CLIENT_CFG_REQ_RX_TIMEOUT_MS_DFLT;
}

/****************************************************************************************************//**
 *                                           SNTPc_DfltCfgSet()
 *
 * @brief    Sets the default server configurations.
 *
 * @param    port_nbr        Port number to use.
 *
 * @param    addr_family     Address family. Use NET_IP_ADDR_FAMILY_NONE if you are unsure.
 *
 * @param    rx_timeout_ms   Timeout, in milliseconds, for the reception.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *
 * @return   DEF_TRUE,  if the new default server configuration is successfully set.
 *           DEF_FALSE, otherwise.
 *******************************************************************************************************/
void SNTPc_DfltCfgSet(NET_PORT_NBR       port_nbr,
                      NET_IP_ADDR_FAMILY addr_family,
                      CPU_INT32U         rx_timeout_ms,
                      RTOS_ERR           *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((addr_family != NET_IP_ADDR_FAMILY_UNKNOWN), *p_err, RTOS_ERR_INVALID_CFG,; );
  RTOS_ASSERT_CRITICAL((SNTPc_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  //                                                               Get SNTPc Lock.
  SNTPc_AcquireLock();

  //                                                               Set default configurations.
  SNTPc_Ptr->ServerPortNbrDflt = port_nbr;
  SNTPc_Ptr->ServerAddrFamilyDflt = addr_family;
  SNTPc_Ptr->ReqRxTimeout_msDflt = rx_timeout_ms;

  //                                                               Release SNTPc Lock.
  SNTPc_ReleaseLock();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           SNTPc_ReqRemoteTime()
 *
 * @brief    Send a request to an NTP server and receive an SNTPc packet to compute.
 *
 * @param    hostname    String that contains the NTP server hostname.
 *
 * @param    p_pkt       Pointer to an SNTP_PKT variable that will contain the received SNTPc packet.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_TYPE
 *                           - RTOS_ERR_NET_RETRY_MAX
 *                           - RTOS_ERR_NET_SOCK_CLOSED
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_NOT_SUPPORTED
 *                           - RTOS_ERR_NET_CONN_CLOSE_RX
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_FAIL
 *                           - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_NET_IF_LINK_DOWN
 *                           - RTOS_ERR_CODE_GET(err_rtn
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_NET_OP_IN_PROGRESS
 *                           - RTOS_ERR_TX
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_ALREADY_EXISTS
 *                           - RTOS_ERR_CODE_GET(local_err
 *                           - RTOS_ERR_NET_INVALID_CONN
 *                           - RTOS_ERR_NET_STR_ADDR_INVALID
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_NET_ADDR_UNRESOLVED
 *                           - RTOS_ERR_NET_NEXT_HOP
 *                           - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *******************************************************************************************************/
void SNTPc_ReqRemoteTime(CPU_CHAR *hostname,
                         SNTP_PKT *p_pkt,
                         RTOS_ERR *p_err)
{
  NET_SOCK_ID        sock;
  NET_IP_ADDR_FAMILY ip_family;
  NET_SOCK_ADDR      sock_addr;
  NET_TS_MS          timestamp;
  CPU_INT32U         second;
  CPU_INT32U         frac;
  CPU_FP64           frac_float;
  CPU_BOOLEAN        is_hostname;
  CPU_BOOLEAN        is_completed;
  CPU_BOOLEAN        is_retry_allowed;
  RTOS_ERR           local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_pkt != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               ------------- ACQUIRE SNTPc MODULE LOCK -------------
  SNTPc_AcquireLock();

  //                                                               ----------------- SELECT IP FAMILY -----------------
  ip_family = SNTPc_Ptr->ServerAddrFamilyDflt;
  if (ip_family == NET_IP_ADDR_FAMILY_NONE) {
    ip_family = NET_IP_ADDR_FAMILY_IPv6;                        // If the ip family is unknown, Try first with IPV6.
  }

  is_completed = DEF_NO;

  while (is_completed == DEF_NO) {
    //                                                             ------------- RESOLVE SERVER HOST NAME -------------
    (void)NetApp_ClientDatagramOpenByHostname(&sock,
                                              hostname,
                                              SNTPc_Ptr->ServerPortNbrDflt,
                                              ip_family,
                                              &sock_addr,
                                              &is_hostname,
                                              p_err);
    //                                                             Check if a retry in IPv4 is allowed in case of error.
    if ((is_hostname == DEF_YES)
        && (SNTPc_Ptr->ServerAddrFamilyDflt == NET_IP_ADDR_FAMILY_NONE)
        && (ip_family == NET_IP_ADDR_FAMILY_IPv6)) {
      is_retry_allowed = DEF_YES;
    } else {
      is_retry_allowed = DEF_NO;
    }
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      if (is_retry_allowed == DEF_YES) {
        ip_family = NET_IP_ADDR_FAMILY_IPv4;
        continue;
      }
      goto exit_release;
    }

    //                                                             ----------- SET SOCKET IN BLOCKING MODE ------------
    NetSock_CfgBlock(sock, NET_SOCK_BLOCK_SEL_BLOCK, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
    //                                                             ---------------------- TX REQ ----------------------
    SNTPc_Tx(sock, &sock_addr, p_err);                          // Send the SNTPc request to the NTP server.
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      NetSock_Close(sock, &local_err);
      if (is_retry_allowed == DEF_YES) {
        ip_family = NET_IP_ADDR_FAMILY_IPv4;
        continue;
      }
      goto exit_release;
    }

    //                                                             ---------------------- RX REP ----------------------
    SNTPc_Rx(sock,                                              // Pend and Receive the SNTPc packet.
             SNTPc_Ptr->ReqRxTimeout_msDflt,
             p_pkt,
             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      NetSock_Close(sock, &local_err);
      if (is_retry_allowed == DEF_YES) {
        ip_family = NET_IP_ADDR_FAMILY_IPv4;
        continue;
      }
      goto exit_release;
    }
    //                                                             ----------------- COMPUTE REF TIME -----------------

    timestamp = NetUtil_TS_Get_ms();
    second = (CPU_INT32U)(timestamp / SNTP_MS_NBR_PER_SEC);
    frac_float = ((CPU_FP64) (timestamp % SNTP_MS_NBR_PER_SEC)) / SNTP_MS_NBR_PER_SEC;
    frac = (CPU_INT32U)(frac_float * SNTP_TS_SEC_FRAC_SIZE);
    p_pkt->TS_Ref.Sec = NET_UTIL_HOST_TO_NET_32(second);
    p_pkt->TS_Ref.Frac = NET_UTIL_HOST_TO_NET_32(frac);

    NetSock_Close(sock, p_err);

    is_completed = DEF_YES;
  }

  //                                                               ------------- RELEASE SNTPc MODULE LOCK -------------
exit_release:
  SNTPc_ReleaseLock();

exit:
  return;
}

/****************************************************************************************************//**
 *                                           SNTPc_GetRemoteTime()
 *
 * @brief    Gets the remote time (NTP timestamp) from a received NTP packet.
 *
 * @param    p_pkt   Pointer to received SNTPc message packet.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *
 * @return   NTP timestamp.
 *******************************************************************************************************/
SNTP_TS SNTPc_GetRemoteTime(SNTP_PKT *p_pkt,
                            RTOS_ERR *p_err)
{
  CPU_FP64 ts_originate;
  CPU_FP64 ts_rx;
  CPU_FP64 ts_tx;
  CPU_FP64 ts_terminate;
  CPU_FP64 local_time_offset;
  CPU_FP64 local_time_float;
  SNTP_TS  local_time;

  local_time.Sec = 0u;
  local_time.Frac = 0u;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, local_time);
  RTOS_ASSERT_DBG_ERR_SET((p_pkt != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, local_time);

  //                                                               ------------- GET TIME VALUES FROM PKT -------------
  ts_originate = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Originate.Sec)
                 + (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Originate.Frac)
                 / (CPU_FP64)SNTP_TS_SEC_FRAC_SIZE;

  ts_rx = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Rx.Sec)
          + (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Rx.Frac)
          / (CPU_FP64)SNTP_TS_SEC_FRAC_SIZE;

  ts_tx = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Tx.Sec)
          + (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Tx.Frac)
          / (CPU_FP64)SNTP_TS_SEC_FRAC_SIZE;

  ts_terminate = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Ref.Sec)
                 + (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Ref.Frac)
                 / (CPU_FP64)SNTP_TS_SEC_FRAC_SIZE;

  //                                                               ------------------ CALCULATE TIME ------------------
  local_time_offset = ((ts_rx - ts_originate)                   // Calculate local time offset from the server.
                       + (ts_tx - ts_terminate)) / 2;

  //                                                               Apply offset to local time.
  local_time_float = (((CPU_FP64)NetUtil_TS_Get_ms()) / SNTP_MS_NBR_PER_SEC) + local_time_offset;
  local_time.Sec = (CPU_INT32U)local_time_float;
  local_time.Frac = (CPU_INT32U)((local_time_float - local_time.Sec) * SNTP_TS_SEC_FRAC_SIZE);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (local_time);
}

/****************************************************************************************************//**
 *                                       SNTPc_GetRoundTripDly_us()
 *
 * @brief    Gets SNTPc packet round trip delay from a received SNTPc message packet.
 *
 * @param    p_kt    Pointer to received SNTPc message packet.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *
 * @return   SNTPc packet round trip delay in microseconds.
 *
 * @note     (1) If the round trip delay is faster than the precision of the system clock, then the
 *               round trip delay is approximated to 0.
 *
 * @note     (2) Only the integer part of the round trip delay is returned.
 *******************************************************************************************************/
CPU_INT32U SNTPc_GetRoundTripDly_us(SNTP_PKT *p_pkt,
                                    RTOS_ERR *p_err)
{
  CPU_FP64   ts_originate;
  CPU_FP64   ts_rx;
  CPU_FP64   ts_tx;
  CPU_FP64   ts_terminate;
  CPU_FP64   round_trip_dly;
  CPU_INT32U round_trip_dly_32;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_pkt != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  round_trip_dly_32 = 0u;

  //                                                               ------------- GET TIME VALUES FROM PKT -------------
  ts_originate = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Originate.Sec)
                 + (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Originate.Frac)
                 / (CPU_FP64)SNTP_TS_SEC_FRAC_SIZE;

  ts_rx = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Rx.Sec)
          + (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Rx.Frac)
          / (CPU_FP64)SNTP_TS_SEC_FRAC_SIZE;

  ts_tx = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Tx.Sec)
          + (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Tx.Frac)
          / (CPU_FP64)SNTP_TS_SEC_FRAC_SIZE;

  ts_terminate = (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Ref.Sec)
                 + (CPU_FP64)NET_UTIL_NET_TO_HOST_32(p_pkt->TS_Ref.Frac)
                 / (CPU_FP64)SNTP_TS_SEC_FRAC_SIZE;

  //                                                               ------------- CALCULATE ROUND TRIP DLY -------------
  round_trip_dly = ((ts_terminate - ts_originate)
                    - (ts_tx - ts_rx)) * 1000000;

  round_trip_dly_32 = (CPU_INT32U)round_trip_dly;               // See Note #2.

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (round_trip_dly_32);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               SNTPc_Rx()
 *
 * @brief    Receive a NTP packet from server.
 *
 * @param    sock        Socket to receive NTP message packet from.
 *
 * @param    timeout_ms  Timeout value for the the reception.
 *
 * @param    p_pkt       Pointer to allocated SNTPc message packet.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_TRUE,  if packet successfully received.
 *           DEF_FALSE, otherwise.
 *******************************************************************************************************/
static void SNTPc_Rx(NET_SOCK_ID sock,
                     CPU_INT32U  timeout_ms,
                     SNTP_PKT    *p_pkt,
                     RTOS_ERR    *p_err)
{
  NET_SOCK_ADDR     remote_addr;
  NET_SOCK_ADDR_LEN remote_addr_size;

  NetSock_CfgTimeoutRxQ_Set(sock,                               // Set the Rx timeout timer.
                            timeout_ms,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  remote_addr_size = sizeof(remote_addr);

  //                                                               ---------------------- RX PKT ----------------------
  (void)NetSock_RxDataFrom(sock,
                           p_pkt,
                           sizeof(SNTP_PKT),
                           NET_SOCK_FLAG_NONE,
                           (NET_SOCK_ADDR *)&remote_addr,
                           (NET_SOCK_ADDR_LEN *)&remote_addr_size,
                           DEF_NULL,
                           0,
                           DEF_NULL,
                           p_err);
}

/****************************************************************************************************//**
 *                                               SNTPc_Tx()
 *
 * @brief    Send NTP packet to server.
 *
 * @param    sock    Socket to sent NTP message packet to.
 *
 * @param    p_addr  Pointer to SNTPc server sockaddr_in.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_TRUE,  if packet successfully sent.
 *           DEF_FALSE, otherwise.
 *
 * @note     (1) RFC # 2030, Section 5 'SNTPc Client Operations' states that "[For client operations],
 *               all of the NTP header fields [...] can be set to 0, except the first octet and
 *               (optional) Transmit Timestamp fields.  In the first octet, the LI field is set to 0
 *               (no warning) and the Mode field is set to 3 (client).  The VN field must agree with
 *               the version number of the NTP/SNTPc server".
 *******************************************************************************************************/
static void SNTPc_Tx(NET_SOCK_ID   sock,
                     NET_SOCK_ADDR *p_addr,
                     RTOS_ERR      *p_err)
{
  CPU_INT32U cw;
  SNTP_PKT   pkt;
  CPU_INT08U li;
  CPU_INT08U vn;
  CPU_INT08U mode;
  NET_TS_MS  timestamp;
  CPU_INT32U second;
  CPU_INT32U frac;
  CPU_FP64   frac_float;

  Mem_Clr(&pkt, sizeof(pkt));                                   // Clr SNTPc msg pkt.

  //                                                               --------------------- INIT MSG ---------------------
  //                                                               See Note #1.
  li = SNTPc_MSG_LI_NO_WARNING;                                 // Set flags.
  li <<= SNTPc_MSG_FLAG_LI_SHIFT;

  vn = SNTPc_MSG_VER_4;
  vn <<= SNTPc_MSG_FLAG_VN_SHIFT;

  mode = SNTPc_MSG_MODE_CLIENT;

  cw = li | vn | mode;
  cw <<= SNTPc_MSG_FLAG_SHIFT;

  pkt.CW = NET_UTIL_HOST_TO_NET_32(cw);

  //                                                               Set tx timestamp.
  timestamp = NetUtil_TS_Get_ms();
  second = (CPU_INT32U)(timestamp / SNTP_MS_NBR_PER_SEC);
  frac_float = ((CPU_FP64) (timestamp % SNTP_MS_NBR_PER_SEC)) / SNTP_MS_NBR_PER_SEC;
  frac = (CPU_INT32U)(frac_float * SNTP_TS_SEC_FRAC_SIZE);
  pkt.TS_Tx.Sec = NET_UTIL_HOST_TO_NET_32(second);
  pkt.TS_Tx.Frac = NET_UTIL_HOST_TO_NET_32(frac);

  //                                                               ---------------------- TX PKT ----------------------
  (void)NetSock_TxDataTo(sock,
                         &pkt,
                         sizeof(SNTP_PKT),
                         NET_SOCK_FLAG_SOCK_NONE,
                         p_addr,
                         sizeof(NET_SOCK_ADDR),
                         p_err);
}

/****************************************************************************************************//**
 *                                           SNTPc_AcquireLock()
 *
 * @brief    Acquire the module lock.
 *******************************************************************************************************/
static void SNTPc_AcquireLock(void)
{
  RTOS_ERR local_err;

  KAL_LockAcquire(SNTPc_Ptr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  KAL_TIMEOUT_INFINITE,
                  &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           SNTPc_ReleaseLock()
 *
 * @brief    Release the module lock.
 *******************************************************************************************************/
static void SNTPc_ReleaseLock(void)
{
  RTOS_ERR local_err;

  KAL_LockRelease(SNTPc_Ptr->LockHandle,
                  &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_SNTP_CLIENT_AVAIL
