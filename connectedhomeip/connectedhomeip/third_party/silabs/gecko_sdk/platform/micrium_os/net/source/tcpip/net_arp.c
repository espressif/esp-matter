/***************************************************************************//**
 * @file
 * @brief Network Arp Layer - (Address Resolution Protocol)
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_cfg_net.h"

#ifdef  NET_ARP_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#define    NET_ARP_MODULE

#include  "../../include/net_arp.h"
#include  "../../include/net_util.h"

#include  "net_arp_priv.h"
#include  "net_if_priv.h"
#include  "net_ip_priv.h"
#include  "net_priv.h"
#include  "net_ctr_priv.h"

#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                 (NET)
#define  RTOS_MODULE_CUR                              RTOS_CFG_MODULE_NET

#define  NET_ARP_PROTOCOL_TYPE_IP_V4                  0x0800u
#define  NET_ARP_HW_TYPE_ETHER                        0x0001u

#define  NET_ARP_PROTOCOL_ADDR_NONE                   0u

/********************************************************************************************************
 *                                           ARP CACHE DEFINES
 *
 * Note(s) : (1) (a) RFC #1122, Section 2.3.2.2 states that "the link layer SHOULD save (rather than
 *                   discard) at least one ... packet of each set of packets destined to the same
 *                   unresolved IP address, and transmit the saved packet when the address has been
 *                   resolved."
 *
 *               (b) However, in order to avoid excessive discards, it seems reasonable that at least
 *                   two transmit packet buffers should be queued to a pending ARP cache.
 *******************************************************************************************************/

#define  NET_ARP_CACHE_TIMEOUT_MIN_SEC           (1  *  DEF_TIME_NBR_SEC_PER_MIN)   // Cache timeout min  =  1 min
#define  NET_ARP_CACHE_TIMEOUT_MAX_SEC           (10  *  DEF_TIME_NBR_SEC_PER_MIN)  // Cache timeout max  = 10 min
#define  NET_ARP_CACHE_TIMEOUT_DFLT_SEC          (10  *  DEF_TIME_NBR_SEC_PER_MIN)  // Cache timeout dflt = 10 min

#define  NET_ARP_CACHE_TX_Q_TH_MIN                         0
#define  NET_ARP_CACHE_TX_Q_TH_MAX                       NET_BUF_NBR_MAX
#define  NET_ARP_CACHE_TX_Q_TH_DFLT                        2                        // See Note #1b.

#define  NET_ARP_RENEW_REQ_RETRY_DFLT                      5

/********************************************************************************************************
 *                                       ARP HEADER / MESSAGE DEFINES
 *
 * Note(s) : (1) See RFC #826, Section 'Packet Format' for ARP packet header format.
 *
 *               (a) ARP header includes two pairs of hardware & protocol type addresses--one each for the
 *                   sender & the target.
 *******************************************************************************************************/

#define  NET_ARP_HDR_OP_REQ                           0x0001u
#define  NET_ARP_HDR_OP_REPLY                         0x0002u

#define  NET_ARP_MSG_LEN                                 NET_ARP_HDR_SIZE
#define  NET_ARP_MSG_LEN_DATA                              0

/********************************************************************************************************
 *                                           ARP REQUEST DEFINES
 *
 * Note(s) : (1) RFC #1122, Section 2.3.2.1 states that "a mechanism to prevent ARP flooding (repeatedly
 *               sending an ARP Request for the same IP address, at a high rate) MUST be included.  The
 *               recommended maximum rate is 1 per second per destination".
 *******************************************************************************************************/

#define  NET_ARP_REQ_RETRY_MIN                             0
#define  NET_ARP_REQ_RETRY_MAX                             5
//                                                                 ARP req retry timeouts (see Note #1).
#define  NET_ARP_REQ_RETRY_TIMEOUT_MIN_SEC                 1    // ARP req retry timeout min  =  1 sec
#define  NET_ARP_REQ_RETRY_TIMEOUT_MAX_SEC                10    // ARP req retry timeout max  = 10 sec

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               ARP HEADER
 *
 * Note(s) : (1) See RFC #826, Section 'Packet Format' for ARP packet header format.
 *
 *           (2) See 'ARP HEADER / MESSAGE DEFINES'.
 *******************************************************************************************************/

//                                                                 --------------- NET ARP HDR ----------------
typedef  struct  net_arp_hdr {
  CPU_INT16U AddrHW_Type;                                               // ARP pkt hw       type.
  CPU_INT16U AddrProtocolType;                                          // ARP pkt protocol type.

  CPU_INT08U AddrHW_Len;                                                // ARP pkt hw       addr len (in octets).
  CPU_INT08U AddrProtocolLen;                                           // ARP pkt protocol addr len (in octets).

  CPU_INT16U OpCode;                                                    // ARP op code (see Note #2).

  CPU_INT08U AddrHW_Sender[NET_IF_HW_ADDR_LEN_MAX];                     // Sender hw       addr.

  //                                                               Sender protocol addr.
  CPU_INT08U AddrProtocolSender[NET_IPv4_ADDR_SIZE];

  CPU_INT08U AddrHW_Target[NET_IF_HW_ADDR_LEN_MAX];                     // Target hw       addr.

  //                                                               Target protocol addr.
  CPU_INT08U AddrProtocolTarget[NET_IPv4_ADDR_SIZE];
} NET_ARP_HDR;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_INT16U NetARP_CacheAccessedTh_nbr;                          // Nbr successful srch's to promote ARP cache.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN NetARP_AddrFltrEn;

static NET_ARP_CACHE NetARP_CacheTbl[NET_ARP_CFG_CACHE_NBR];

static NET_BUF_QTY NetARP_CacheTxQ_MaxTh_nbr;                   // Max nbr tx bufs to enqueue    on ARP cache.

static CPU_INT32U NetARP_CacheTimeout_ms;                       // ARP cache timeout value.

static CPU_INT08U NetARP_ReqMaxAttemptsPend_nbr;                // ARP req max nbr of attempts in pend  state.
static CPU_INT32U NetARP_ReqTimeoutPend_ms;                     // ARP req wait-for-reply timeout (in secs ).

static CPU_INT08U NetARP_ReqMaxAttemptsRenew_nbr;               // ARP req max nbr of attempts in renew state.
static CPU_INT32U NetARP_ReqTimeoutRenew_ms;                    // ARP req wait-for-reply timeout (in secs ).

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------------- RX FNCTS ---------------
static void NetARP_RxPktValidate(NET_BUF_HDR *p_buf_hdr,
                                 NET_ARP_HDR *p_arp_hdr,
                                 RTOS_ERR    *p_err);

static void NetARP_RxPktCacheUpdate(NET_IF_NBR  if_nbr,
                                    NET_ARP_HDR *p_arp_hdr,
                                    RTOS_ERR    *p_err);

static void NetARP_RxPktReply(NET_IF_NBR  if_nbr,
                              NET_ARP_HDR *p_arp_hdr,
                              RTOS_ERR    *p_err);

static CPU_BOOLEAN NetARP_RxPktIsTargetThisHost(NET_IF_NBR  if_nbr,
                                                NET_ARP_HDR *p_arp_hdr,
                                                RTOS_ERR    *p_err);

static void NetARP_RxPktFree(NET_BUF *p_buf);

//                                                                 --------------- TX FNCTS ---------------

static void NetARP_Tx(NET_IF_NBR if_nbr,
                      CPU_INT08U *p_addr_hw_sender,
                      CPU_INT08U *p_addr_hw_target,
                      CPU_INT08U *p_addr_protocol_sender,
                      CPU_INT08U *p_addr_protocol_target,
                      CPU_INT16U op_code,
                      RTOS_ERR   *p_err);

static void NetARP_TxReply(NET_IF_NBR  if_nbr,
                           NET_ARP_HDR *p_arp_hdr);

static void NetARP_TxPktPrepareHdr(NET_BUF    *p_buf,
                                   CPU_INT16U msg_ix,
                                   CPU_INT08U *p_addr_hw_sender,
                                   CPU_INT08U *p_addr_hw_target,
                                   CPU_INT08U *p_addr_protocol_sender,
                                   CPU_INT08U *p_addr_protocol_target,
                                   CPU_INT16U op_code);

static void NetARP_TxIxDataGet(NET_IF_NBR if_nbr,
                               CPU_INT32U data_len,
                               CPU_INT16U *p_ix,
                               RTOS_ERR   *p_err);

static void NetARP_TxPktFree(NET_BUF *p_buf);

static void NetARP_TxPktDiscard(NET_BUF *p_buf);

//                                                                 ----------- ARP CACHE FNCTS ------------

static void NetARP_CacheReqTimeout(void *p_cache_timeout);

static void NetARP_CacheTimeout(void *p_cache_timeout);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetARP_CfgAddrFilterEn()
 *
 * @brief    Configures the ARP address filter feature.
 *
 * @param    en  Set Filter enabled or disabled:
 *                   - DEF_ENABLED     Enable  filtering feature.
 *                   - DEF_DISABLED    Disable filtering feature.
 *******************************************************************************************************/
void NetARP_CfgAddrFilterEn(CPU_BOOLEAN en)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  NetARP_AddrFltrEn = en;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           NetARP_TxReqGratuitous()
 *
 * @brief    (1) Prepares and transmits a gratuitous ARP Request onto the local network by following
 *               these steps :
 *
 *           - (a) Acquire  network lock.
 *           - (b) Get      network interface's hardware address.
 *           - (c) Prepare  ARP Request packet :
 *               - (1) Configure sender's hardware address as this interface's hardware address
 *               - (2) Configure target's hardware address as NULL
 *               - (3) Configure sender's protocol address as this interface's protocol address
 *               - (4) Configure target's protocol address as this interface's protocol address
 *                       (See Note #6a)
 *               - (5) Configure ARP operation as ARP Request
 *           - (d) Transmit ARP Request.
 *           - (e) Release  network lock.
 *
 *       - (2) NetARP_TxReqGratuitous() COULD be used in conjunction with
 *               NetARP_IsAddrProtocolConflict()to determine if the host's protocol address is already
 *               present on the local network :
 *
 *           - (a) After successfully transmitting a gratuitous ARP Request onto the local
 *                   network and ...
 *           - (b) After some time delay(s) [on the order of ARP Request timeouts & retries]; ...
 *           - (c) Check this host's ARP protocol address conflict flag to see if any other host(s)
 *                   are configured with this host's ARP protocol address.
 *
 * @param    protocol_type       Address protocol type.
 *
 * @param    p_addr_protocol     Pointer to protocol address used to transmit gratuitous request
 *                               (see Note #5).
 *
 * @param    addr_protocol_len   Length  of protocol address (in octets).
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_INVALID_TYPE
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_NOT_FOUND
 *                                   - RTOS_ERR_RX
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_WOULD_OVF
 *                                   - RTOS_ERR_NET_IF_LINK_DOWN
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_TIMEOUT
 *
 * @internal
 * @note     (3) [INTERNAL] NetARP_TxReqGratuitous() is called by application function(s) and :
 *           - (a) MUST NOT be called with the global network lock already acquired;
 *           - (b) MUST block ALL other network protocol tasks by pending on and acquiring the global
 *                   network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (4) 'p_addr_protocol' MUST point to a valid protocol address in network-order.
 *
 * @note     (5) RFC #3927, Section 2.4 states that one purpose for transmitting a gratuitous ARP
 *               Request is for a host to "announce its [claim] ... [on] a unique address ... by
 *               broadcasting ... an ARP Request ... to make sure that other hosts on the link do not
 *               have stale ARP cache entries left over from some other host that may previously have
 *               been using the same address".
 *           - (a) "The ... ARP Request ... announcement ... sender and target IP addresses are both
 *                   set to the host's newly selected ... address."
 *******************************************************************************************************/
void NetARP_TxReqGratuitous(NET_PROTOCOL_TYPE  protocol_type,
                            CPU_INT08U         *p_addr_protocol,
                            NET_CACHE_ADDR_LEN addr_protocol_len,
                            RTOS_ERR           *p_err)
{
  NET_IF_NBR         if_nbr;
  NET_IP_ADDR_FAMILY ip_type;
  CPU_INT08U         addr_hw_len;
  CPU_INT08U         addr_hw_sender[NET_IF_HW_ADDR_LEN_MAX];
  CPU_INT08U         *p_addr_hw_sender;
  CPU_INT08U         *p_addr_hw_target;
  CPU_INT08U         *p_addr_protocol_sender;
  CPU_INT08U         *p_addr_protocol_target;
  CPU_INT16U         op_code;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_protocol != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #3b.
  Net_GlobalLockAcquire((void *)NetARP_TxReqGratuitous);

  switch (protocol_type) {
    case NET_PROTOCOL_TYPE_IP_V4:
      ip_type = NET_IP_ADDR_FAMILY_IPv4;
      break;

    case NET_PROTOCOL_TYPE_IP_V6:
      ip_type = NET_IP_ADDR_FAMILY_IPv6;
      break;

    default:
      ip_type = NET_IP_ADDR_FAMILY_NONE;
      break;
  }

  //                                                               ------------- GET PROTOCOL & HW ADDRS --------------
  if_nbr = NetIP_GetHostAddrIF_Nbr(ip_type,
                                   p_addr_protocol,
                                   addr_protocol_len,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  addr_hw_len = NET_IF_HW_ADDR_LEN_MAX;
  NetIF_AddrHW_GetHandler(if_nbr,
                          &addr_hw_sender[0],
                          &addr_hw_len,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               --------------- PREPARE ARP REQ PKT ----------------
  //                                                               See Note #1c.
  p_addr_hw_sender = &addr_hw_sender[0];
  p_addr_hw_target = DEF_NULL;
  p_addr_protocol_sender = p_addr_protocol;
  p_addr_protocol_target = p_addr_protocol;

  op_code = NET_ARP_HDR_OP_REQ;

  //                                                               -------------------- TX ARP REQ --------------------
  NetARP_Tx(if_nbr,
            p_addr_hw_sender,
            p_addr_hw_target,
            p_addr_protocol_sender,
            p_addr_protocol_target,
            op_code,
            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.ARP.TxMsgReqCtr);

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return;
}

/****************************************************************************************************//**
 *                                           NetARP_CfgCacheTimeout()
 *
 * @brief    Configures the ARP cache timeout from ARP Cache List.
 *
 * @param    timeout_sec     Desired value for the ARP cache timeout (in seconds).
 *
 * @return   DEF_OK,   ARP cache timeout configured.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) RFC #1122, Section 2.3.2.1 states that "an implementation of the Address Resolution
 *               Protocol (ARP) ... MUST provide a mechanism to flush out-of-date cache entries.  If
 *               this mechanism involves a timeout, it SHOULD be possible to configure the timeout
 *               value".
 *
 * @note     (2) Configured timeout does NOT reschedule any current ARP cache timeout in progress, but
 *               becomes effective the next time an ARP cache sets its timeout.
 *
 * @internal
 * @note     (3) [INTERNAL] 'NetARP_CacheTimeout_ms' variables MUST ALWAYS be accessed exclusively in
 *               critical sections.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetARP_CfgCacheTimeout(CPU_INT16U timeout_sec)
{
  CORE_DECLARE_IRQ_STATE;

#if (NET_ARP_CACHE_TIMEOUT_MIN_SEC > DEF_INT_16U_MIN_VAL)
  if (timeout_sec < NET_ARP_CACHE_TIMEOUT_MIN_SEC) {
    return (DEF_FAIL);
  }
#endif
#if (NET_ARP_CACHE_TIMEOUT_MAX_SEC < DEF_INT_16U_MAX_VAL)
  if (timeout_sec > NET_ARP_CACHE_TIMEOUT_MAX_SEC) {
    return (DEF_FAIL);
  }
#endif

  CORE_ENTER_ATOMIC();
  NetARP_CacheTimeout_ms = timeout_sec * DEF_TIME_NBR_mS_PER_SEC;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       NetARP_CfgCacheTxQ_MaxTh()
 *
 * @brief    Configures the ARP cache maximum number of transmit packet buffers to enqueue.
 *
 * @param    nbr_buf_max     Desired maximum number of transmit packet buffers to enqueue onto an
 *
 * @return   DEF_OK,   ARP cache transmit packet buffer threshold configured.
 *           DEF_FAIL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] 'NetARP_CacheTxQ_MaxTh_nbr' MUST ALWAYS be accessed exclusively in
 *               critical sections.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetARP_CfgCacheTxQ_MaxTh(NET_BUF_QTY nbr_buf_max)
{
  CORE_DECLARE_IRQ_STATE;

#if (NET_ARP_CACHE_TX_Q_TH_MIN > DEF_INT_16U_MIN_VAL)
  if (nbr_buf_max < NET_ARP_CACHE_TX_Q_TH_MIN) {
    return (DEF_FAIL);
  }
#endif
#if (NET_ARP_CACHE_TX_Q_TH_MAX < DEF_INT_16U_MAX_VAL)
  if (nbr_buf_max > NET_ARP_CACHE_TX_Q_TH_MAX) {
    return (DEF_FAIL);
  }
#endif

  CORE_ENTER_ATOMIC();
  NetARP_CacheTxQ_MaxTh_nbr = nbr_buf_max;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       NetARP_CfgCacheAccessedTh()
 *
 * @brief    Configures the ARP cache access promotion threshold.
 *
 * @param    nbr_access  Desired number of ARP cache accesses before ARP cache is promoted.
 *
 * @return   DEF_OK,   ARP cache access promotion threshold configured.
 *           DEF_FAIL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] 'NetARP_CacheAccessedTh_nbr' MUST ALWAYS be accessed exclusively in
 *               critical sections.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetARP_CfgCacheAccessedTh(CPU_INT16U nbr_access)
{
  CORE_DECLARE_IRQ_STATE;

#if (NET_ARP_CACHE_ACCESSED_TH_MIN > DEF_INT_16U_MIN_VAL)
  if (nbr_access < NET_ARP_CACHE_ACCESSED_TH_MIN) {
    return (DEF_FAIL);
  }
#endif
#if (NET_ARP_CACHE_ACCESSED_TH_MAX < DEF_INT_16U_MAX_VAL)
  if (nbr_access > NET_ARP_CACHE_ACCESSED_TH_MAX) {
    return (DEF_FAIL);
  }
#endif

  CORE_ENTER_ATOMIC();
  NetARP_CacheAccessedTh_nbr = nbr_access;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       NetARP_CfgPendReqTimeout()
 *
 * @brief    Configures the timeout between ARP Request retries in the ARP cache PEND state.
 *
 * @param    timeout_sec     Desired value for ARP Request pending ARP Reply timeout (in seconds).
 *
 * @return   DEF_OK,   ARP Request timeout configured.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) Configured timeout does NOT reschedule any current ARP Request timeout in progress but
 *               becomes effective the next time an ARP Request is transmitted with timeout.
 *
 * @internal
 * @note     (2) [INTERNAL] 'NetARP_ReqTimeoutPend_ms' variables MUST ALWAYS be accessed exclusively in
 *               critical sections.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetARP_CfgPendReqTimeout(CPU_INT08U timeout_sec)
{
  CORE_DECLARE_IRQ_STATE;

  if (timeout_sec < NET_ARP_REQ_RETRY_TIMEOUT_MIN_SEC) {
    return (DEF_FAIL);
  }
  if (timeout_sec > NET_ARP_REQ_RETRY_TIMEOUT_MAX_SEC) {
    return (DEF_FAIL);
  }

  CORE_ENTER_ATOMIC();
  NetARP_ReqTimeoutPend_ms = timeout_sec * DEF_TIME_NBR_mS_PER_SEC;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       NetARP_CfgPendReqMaxRetries()
 *
 * @brief    Configures the ARP Request maximum number of requests in the ARP cache PEND state.
 *
 * @param    max_nbr_retries     Desired maximum number of ARP Request attempts.
 *
 * @return   DEF_OK,   ARP Request maximum number of request attempts configured.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) An ARP cache monitors the number of ARP Requests transmitted before receiving an ARP
 *               Reply. In other words, an ARP cache monitors the number of ARP Request attempts.
 *               @n
 *               However, the maximum number of ARP Requests that each ARP cache is allowed to transmit
 *               is configured in terms of retries. The total number of attempts is equal to the
 *               configured number of retries, plus one (1).
 *
 * @internal
 * @note     (2) [INTERNAL] 'NetARP_ReqMaxAttemptsPend_nbr' MUST ALWAYS be accessed exclusively in
 *               critical sections.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetARP_CfgPendReqMaxRetries(CPU_INT08U max_nbr_retries)
{
  CORE_DECLARE_IRQ_STATE;

#if (NET_ARP_REQ_RETRY_MIN > DEF_INT_08U_MIN_VAL)
  if (max_nbr_retries < NET_ARP_REQ_RETRY_MIN) {
    return (DEF_FAIL);
  }
#endif
#if (NET_ARP_REQ_RETRY_MAX < DEF_INT_08U_MAX_VAL)
  if (max_nbr_retries > NET_ARP_REQ_RETRY_MAX) {
    return (DEF_FAIL);
  }
#endif

  CORE_ENTER_ATOMIC();
  NetARP_ReqMaxAttemptsPend_nbr = max_nbr_retries + 1u;         // Set max attempts as max retries + 1 (see Note #1).
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       NetARP_CfgRenewReqTimeout()
 *
 * @brief    Configures the timeout between ARP Request retries in the ARP cache RENEW state.
 *
 * @param    timeout_sec     Desired value for ARP Request pending ARP Reply timeout (in seconds).
 *
 * @return   DEF_OK,   ARP Request timeout configured.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) Configured timeout does NOT reschedule any current ARP Request timeout in progress but
 *               becomes effective the next time an ARP Request is transmitted with timeout.
 *
 * @internal
 * @note     (2) [INTERNAL] 'NetARP_ReqTimeoutRenew_ms' variables MUST ALWAYS be accessed exclusively in
 *               critical sections.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetARP_CfgRenewReqTimeout(CPU_INT08U timeout_sec)
{
  CORE_DECLARE_IRQ_STATE;

  if (timeout_sec < NET_ARP_REQ_RETRY_TIMEOUT_MIN_SEC) {
    return (DEF_FAIL);
  }
  if (timeout_sec > NET_ARP_REQ_RETRY_TIMEOUT_MAX_SEC) {
    return (DEF_FAIL);
  }

  CORE_ENTER_ATOMIC();
  NetARP_ReqTimeoutRenew_ms = timeout_sec * DEF_TIME_NBR_mS_PER_SEC;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       NetARP_CfgRenewReqMaxRetries()
 *
 * @brief    Configures the ARP Request maximum number of requests in the ARP cache RENEW state.
 *
 * @param    max_nbr_retries     Desired maximum number of ARP Request attempts.
 *
 * @return   DEF_OK,   ARP Request maximum number of request attempts configured.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) An ARP cache monitors the number of ARP Requests transmitted before receiving an ARP
 *               Reply. In other words, an ARP cache monitors the number of ARP Request attempts.
 *               @n
 *               However, the maximum number of ARP Requests that each ARP cache is allowed to transmit
 *               is configured in terms of retries. The total number of attempts is equal to the
 *               configured number of retries, plus one (1).
 *
 * @internal
 * @note     (2) [INTERNAL] 'NetARP_ReqMaxAttemptsRenew_nbr' MUST ALWAYS be accessed exclusively in
 *               critical sections.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetARP_CfgRenewReqMaxRetries(CPU_INT08U max_nbr_retries)
{
  CORE_DECLARE_IRQ_STATE;

#if (NET_ARP_REQ_RETRY_MIN > DEF_INT_08U_MIN_VAL)
  if (max_nbr_retries < NET_ARP_REQ_RETRY_MIN) {
    return (DEF_FAIL);
  }
#endif
#if (NET_ARP_REQ_RETRY_MAX < DEF_INT_08U_MAX_VAL)
  if (max_nbr_retries > NET_ARP_REQ_RETRY_MAX) {
    return (DEF_FAIL);
  }
#endif

  CORE_ENTER_ATOMIC();
  NetARP_ReqMaxAttemptsRenew_nbr = max_nbr_retries + 1u;        // Set max attempts as max retries + 1 (see Note #1).
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       NetARP_IsAddrProtocolConflict()
 *
 * @brief    Gets the interface's protocol address conflict status between this interface's ARP host
 *           protocol address(s) and the other host(s) on the local network.
 *
 * @param    if_nbr  Interface number to get protocol address conflict status.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_YES, if the address conflict is detected.
 *           DEF_NO,  otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] NetARP_IsAddrProtocolConflict() is called by the application function(s)
 *               and ... :
 *               - (a) MUST NOT be called with the global network lock already acquired; ...
 *               - (b) MUST block ALL other network protocol tasks by pending on and acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) RFC #3927, Section 2.5 states that :
 *               - (a) "If a host receives an ARP packet (request *or* reply) on an interface where
 *                     the 'sender hardware address' does not match the hardware address of that
 *                     interface, but the 'sender IP address' is a IP address the host has configured
 *                     for that interface, then this is a conflicting ARP packet, indicating an address
 *                     conflict."
 *******************************************************************************************************/
CPU_BOOLEAN NetARP_IsAddrProtocolConflict(NET_IF_NBR if_nbr,
                                          RTOS_ERR   *p_err)
{
  CPU_BOOLEAN addr_conflict = DEF_NO;
  CPU_BOOLEAN is_valid = DEF_NO;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #1b.
  Net_GlobalLockAcquire((void *)NetARP_IsAddrProtocolConflict);

  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit_release;
  }

  //                                                               ------------ CHK PROTOCOL ADDR CONFLICT ------------
  addr_conflict = NetIP_IsAddrConflict(if_nbr);

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (addr_conflict);
}

/****************************************************************************************************//**
 *                                           NetARP_CacheGetAddrHW()
 *
 * @brief     (1) Gets the hardware address that corresponds to a specific ARP cache's
 *                protocol address by following these steps :
 *                - (a) Acquire the network lock.
 *                - (b) Search the ARP Cache List for ARP cache with the desired    protocol address.
 *                - (c) If the corresponding ARP cache is found, get/return the     hardware address.
 *                - (d) Release network lock.
 *
 * @param     if_nbr              Network interface number.
 *
 * @param     p_addr_hw           Pointer to a memory buffer that will receive the hardware address :
 *
 * @param     addr_hw_len_buf     Length of hardware address memory buffer (in octets) [see Note #4a1].
 *
 * @param     p_addr_protocol     Pointer to the specific protocol address to search for corresponding
 *
 * @param     addr_protocol_len   Length of protocol addresses (in octets) [see Note #4b].
 *
 * @param     p_err               Pointer to the variable that will receive one of the following error
 *                                code(s) from this function:
 *                                    - RTOS_ERR_NONE
 *                                    - RTOS_ERR_NOT_FOUND
 *                                    - RTOS_ERR_NET_ADDR_UNRESOLVED
 *                                    - RTOS_ERR_INVALID_STATE
 *
 * @return    Length of returned hardware address (see Note #4a2), if available.
 *            0, otherwise.
 * @internal
 * @note     (1) [INTERNAL] NetARP_CacheGetAddrHW() is called by application function(s) & ... :
 *               - (a) MUST NOT be called with the global network lock already acquired; ...
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *                This is required since an application's network protocol suite API function access is
 *                asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) ARP addresses handled in network-order :
 *               - (a) 'p_addr_hw'            returns  a hardware address in network-order.
 *               - (b) 'p_addr_protocol' MUST points to a protocol address in network-order.
 *
 * @note     (3) While an ARP cache is in 'PENDING' state, the hardware address is NOT yet resolved,
 *               but it MAY be resolved in the near future by an awaited ARP Reply.
 *******************************************************************************************************/
NET_CACHE_ADDR_LEN NetARP_CacheGetAddrHW(NET_IF_NBR         if_nbr,
                                         CPU_INT08U         *p_addr_hw,
                                         NET_CACHE_ADDR_LEN addr_hw_len_buf,
                                         CPU_INT08U         *p_addr_protocol,
                                         NET_CACHE_ADDR_LEN addr_protocol_len,
                                         RTOS_ERR           *p_err)
{
  NET_CACHE_ADDR_ARP *p_cache_addr_arp = DEF_NULL;
  NET_ARP_CACHE      *p_cache = DEF_NULL;
  NET_CACHE_ADDR_LEN addr_hw_len = 0;
  NET_CACHE_ADDR_LEN rtn_val = 0;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_hw != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_protocol != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0);
  RTOS_ASSERT_DBG_ERR_SET((addr_protocol_len == NET_IPv4_ADDR_SIZE), *p_err, RTOS_ERR_INVALID_ARG, 0);
  RTOS_ASSERT_DBG_ERR_SET((addr_hw_len_buf >= NET_IF_HW_ADDR_LEN_MAX), *p_err, RTOS_ERR_INVALID_ARG, 0);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Mem_Clr(p_addr_hw, addr_hw_len_buf);                          // Clr hw addr

  addr_hw_len = NET_IF_HW_ADDR_LEN_MAX;                         // Cfg hw addr     len

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #2b.
  Net_GlobalLockAcquire((void *)NetARP_CacheGetAddrHW);

  //                                                               -------------------- SRCH CACHE --------------------
  p_cache_addr_arp = (NET_CACHE_ADDR_ARP *)NetCache_AddrSrch(NET_CACHE_TYPE_ARP,
                                                             if_nbr,
                                                             p_addr_protocol,
                                                             NET_IPv4_ADDR_SIZE);

  if (p_cache_addr_arp != DEF_NULL) {
    p_cache = (NET_ARP_CACHE *)p_cache_addr_arp->ParentPtr;
    switch (p_cache->State) {
      case NET_ARP_CACHE_STATE_RESOLVED:
        Mem_Copy(p_addr_hw,
                 p_cache_addr_arp->AddrHW,
                 NET_IF_HW_ADDR_LEN_MAX);
        rtn_val = addr_hw_len;
        break;

      case NET_ARP_CACHE_STATE_PEND:
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_ADDR_UNRESOLVED);
        goto exit_release;

      default:
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
        goto exit_release;
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit_release;
  }

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (rtn_val);
}

/****************************************************************************************************//**
 *                                       NetARP_CacheProbeAddrOnNet()
 *
 * @brief    - (1) Transmits an ARP Request to probe the local network for a specific protocol address :
 *               - (a) Acquire network lock.
 *               - (b) Remove the ARP cache with desired protocol address from ARP Cache List,
 *                     if available.
 *               - (c) Configure the ARP cache :
 *                   - (1) Get the default-configured ARP cache
 *                   - (2) ARP cache state
 *               - (d) Transmit the ARP Request to probe local network for desired protocol address.
 *               - (e) Release the network lock.
 *           - (2) NetARP_CacheProbeAddrOnNet() SHOULD be used in conjunction with
 *                 NetARP_CacheGetAddrHW() to determine if a specific protocol address is available on
 *                 the local network :
 *               - (a) After successfully transmitting an ARP Request to probe the local network.
 *               - (b) After some time delay(s) [on the order of ARP Request timeouts & retries].
 *               - (c) Check ARP Cache for the hardware address of a host on the local network that
 *                     corresponds to the desired protocol address.
 *
 * @param    protocol_type           Address protocol type.
 *
 * @param    p_addr_protocol_sender  Pointer to the protocol address to send the probe from
 *
 * @param    p_addr_protocol_target  Pointer to the protocol address to probe the local network
 *
 * @param    addr_protocol_len       Length of the protocol address (in octets)
 *
 * @param    p_err                   Pointer to the variable that will receive one of the following
 *                                   error code(s) from this function :
 *                                       - RTOS_ERR_NONE
 *                                       - RTOS_ERR_POOL_EMPTY
 *                                       - RTOS_ERR_NOT_FOUND
 *                                       - RTOS_ERR_SEG_OVF
 *
 * @internal
 * @note     (3) [INTERNAL] NetARP_CacheProbeAddrOnNet() is called by application function(s) & ... :
 *               - (a) MUST NOT be called with the global network lock already acquired; ...
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (4) 'p_addr_protocol_sender' MUST point to a valid protocol address in
 *               network-order, configured on a valid interface.
 *
 * @note     (5) 'p_addr_protocol_target' MUST point to a valid protocol address in
 *               network-order.
 *
 * @note     (6) The length of the protocol address MUST be equal to NET_IPv4_ADDR_SIZE
 *               and is included for correctness & completeness.
 *******************************************************************************************************/
void NetARP_CacheProbeAddrOnNet(NET_PROTOCOL_TYPE  protocol_type,
                                CPU_INT08U         *p_addr_protocol_sender,
                                CPU_INT08U         *p_addr_protocol_target,
                                NET_CACHE_ADDR_LEN addr_protocol_len,
                                RTOS_ERR           *p_err)
{
  NET_IF_NBR         if_nbr;
  NET_CACHE_ADDR_ARP *p_cache_addr_arp;
  NET_ARP_CACHE      *p_cache_arp;
  NET_IP_ADDR_FAMILY ip_type;
  CPU_INT32U         timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_protocol_sender != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_protocol_target != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((addr_protocol_len == NET_IPv4_ADDR_SIZE), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #3b.
  Net_GlobalLockAcquire((void *)NetARP_CacheProbeAddrOnNet);

  switch (protocol_type) {
    case NET_PROTOCOL_TYPE_IP_V4:
      ip_type = NET_IP_ADDR_FAMILY_IPv4;
      break;

    case NET_PROTOCOL_TYPE_IP_V6:
      ip_type = NET_IP_ADDR_FAMILY_IPv6;
      break;

    default:
      ip_type = NET_IP_ADDR_FAMILY_NONE;
      break;
  }

  //                                                               -------------------- GET IF NBR --------------------
  if_nbr = NetIP_GetHostAddrIF_Nbr(ip_type,
                                   p_addr_protocol_sender,
                                   addr_protocol_len,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               --------- REMOVE PROTOCOL ADDR'S ARP CACHE ---------
  p_cache_addr_arp = (NET_CACHE_ADDR_ARP *)NetCache_AddrSrch(NET_CACHE_TYPE_ARP,
                                                             if_nbr,
                                                             p_addr_protocol_target,
                                                             NET_IPv4_ADDR_SIZE);

  if (p_cache_addr_arp != DEF_NULL) {                           // If protocol addr's ARP cache avail, ...
    NetCache_Remove((NET_CACHE_ADDR *)p_cache_addr_arp,         // ... remove from ARP Cache List (see Note #1b).
                    DEF_YES);
  }

  //                                                               ------------------ CFG ARP CACHE -------------------
  CORE_ENTER_ATOMIC();
  timeout_ms = NetARP_ReqTimeoutPend_ms;
  CORE_EXIT_ATOMIC();

  p_cache_addr_arp = (NET_CACHE_ADDR_ARP *)NetCache_CfgAddrs(NET_CACHE_TYPE_ARP,
                                                             if_nbr,
                                                             0,
                                                             NET_IF_HW_ADDR_LEN_MAX,
                                                             p_addr_protocol_target,
                                                             p_addr_protocol_sender,
                                                             NET_IPv4_ADDR_SIZE,
                                                             DEF_YES,
                                                             NetARP_CacheReqTimeout,
                                                             timeout_ms,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_cache_arp = (NET_ARP_CACHE *)p_cache_addr_arp->ParentPtr;
  RTOS_ASSERT_CRITICAL((p_cache_arp != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_cache_arp->State = NET_ARP_CACHE_STATE_PEND;

  //                                                               ------- INSERT ARP CACHE INTO ARP CACHE LIST -------
  NetCache_Insert((NET_CACHE_ADDR *)p_cache_addr_arp);

  //                                                               -------------------- TX ARP REQ --------------------

  NetARP_TxReq(p_cache_addr_arp);                               // Tx ARP req to resolve ARP cache.

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetARP_CachePoolStatGet()
 *
 * @brief    Gets the ARP statistics pool.
 *
 * @return   ARP  statistics pool, if NO error(s).
 *           NULL statistics pool, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] 'NetARP_CachePoolStat' MUST ALWAYS be accessed exclusively in critical
 *               sections.
 * @endinternal
 *******************************************************************************************************/
NET_STAT_POOL NetARP_CachePoolStatGet(void)
{
  NET_STAT_POOL stat_pool;
#if (NET_STAT_POOL_ARP_EN == DEF_ENABLED)
  CORE_DECLARE_IRQ_STATE;
#endif

  NetStat_PoolClr(&stat_pool);                                  // Init rtn pool stat for err.

#if (NET_STAT_POOL_ARP_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  stat_pool = NetCache_AddrARP_PoolStat;
  CORE_EXIT_ATOMIC();
#endif

  return (stat_pool);
}

/****************************************************************************************************//**
 *                                   NetARP_CachePoolStatResetMaxUsed()
 *
 * @brief    Resets ARP statistics pool's maximum number of entries used.
 *
 * @internal
 * @note     (1) [INTERNAL] NetARP_CachePoolStatResetMaxUsed() is called by the application function(s)
 *               and ... :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on and acquiring the global
 *                       network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
void NetARP_CachePoolStatResetMaxUsed(void)
{
#if (NET_STAT_POOL_ARP_EN == DEF_ENABLED)
  //                                                               Acquire net lock.
  Net_GlobalLockAcquire((void *)NetARP_CachePoolStatResetMaxUsed);

  NetStat_PoolResetUsedMax(&NetCache_AddrARP_PoolStat);         // Reset ARP cache stat pool.

  Net_GlobalLockRelease();                                      // Release net lock.
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetARP_Init()
 *
 * @brief    (1) Initialize Address Resolution Protocol Layer :
 *               - (a) Initialize ARP cache pool
 *               - (b) Initialize ARP cache table
 *               - (c) Initialize ARP cache list pointers
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) ARP cache pool MUST be initialized PRIOR to initializing the pool with pointers to
 *               ARP caches.
 *******************************************************************************************************/
void NetARP_Init(RTOS_ERR *p_err)
{
  NET_ARP_CACHE     *p_cache;
  NET_ARP_CACHE_QTY i;

  //                                                               -------------- CFG ARP INIT DFLT VALS --------------
  (void)NetARP_CfgCacheTimeout(NET_ARP_CACHE_TIMEOUT_DFLT_SEC);
  (void)NetARP_CfgCacheTxQ_MaxTh(NET_ARP_CACHE_TX_Q_TH_DFLT);
  (void)NetARP_CfgCacheAccessedTh(NET_ARP_CACHE_ACCESSED_TH_DFLT);
  (void)NetARP_CfgPendReqTimeout(NET_ARP_REQ_RETRY_TIMEOUT_DFLT_SEC);
  (void)NetARP_CfgPendReqMaxRetries(NET_ARP_REQ_RETRY_DFLT);
  (void)NetARP_CfgRenewReqTimeout(NET_ARP_REQ_RETRY_TIMEOUT_DFLT_SEC);
  (void)NetARP_CfgRenewReqMaxRetries(NET_ARP_RENEW_REQ_RETRY_DFLT);

  //                                                               ------------ INIT ARP CACHE POOL/STATS -------------
  NetCache_AddrARP_PoolPtr = DEF_NULL;                          // Init-clr ARP cache pool (see Note #2).

#if (NET_STAT_POOL_ARP_EN == DEF_ENABLED)
  NetStat_PoolInit(&NetCache_AddrARP_PoolStat,
                   NET_ARP_CFG_CACHE_NBR);
#endif

  //                                                               ---------------- INIT ARP CACHE TBL ----------------
  p_cache = &NetARP_CacheTbl[0];
  for (i = 0u; i < NET_ARP_CFG_CACHE_NBR; i++) {
    //                                                             Init each ARP      cache type/addr len--NEVER modify.
    p_cache->Type = NET_CACHE_TYPE_ARP;
    p_cache->CacheAddrPtr = &NetCache_AddrARP_Tbl[i];           // Init each ARP addr cache ptr.

    p_cache->ReqAttemptsCtr = 0u;

    p_cache->State = NET_ARP_CACHE_STATE_FREE;                  // Init each ARP      cache as free/NOT used.
    p_cache->Flags = NET_CACHE_FLAG_NONE;

    p_cache->TmrPtr = DEF_NULL;

    NetCache_Init((NET_CACHE_ADDR *) p_cache,                   // Init each ARP addr cache.
                  (NET_CACHE_ADDR *) p_cache->CacheAddrPtr,
                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_cache++;
  }

  NetARP_AddrFltrEn = DEF_ENABLED;
  NetARP_ReqMaxAttemptsRenew_nbr = NET_ARP_RENEW_REQ_RETRY_DFLT + 1u;

  //                                                               ------------- INIT ARP CACHE LIST PTRS -------------
  NetCache_AddrListHead[NET_CACHE_ADDR_LIST_IX_ARP] = DEF_NULL;
  NetCache_AddrListTail[NET_CACHE_ADDR_LIST_IX_ARP] = DEF_NULL;
}

/****************************************************************************************************//**
 *                                           NetARP_CacheHandler()
 *
 * @brief    - (1) Resolve destination hardware address using ARP for transmit data packet :
 *               - (a) Search ARP Cache List for ARP cache with corresponding protocol address
 *               - (b) If ARP cache     found, handle packet based on ARP cache state :
 *                   - (1) PENDING  -> Enqueue transmit packet buffer to ARP cache
 *                   - (2) RESOLVED -> Copy ARP cache's hardware address to data packet;
 *                                     Return to Network Interface to transmit data packet
 *               - (c) If ARP cache NOT found, allocate new ARP cache in 'PENDING' state (see Note #1b1)
 *               See 'net_cache.h  CACHE STATES' for cache state diagram.
 *           - (2) This ARP cache handler function assumes the following :
 *               - (a) ALL ARP caches in the ARP Cache List are valid [validated by NetCache_AddrGet()]
 *               - (b) ANY ARP cache in the 'PENDING' state MAY have already enqueued at LEAST one
 *                     transmit packet buffer when ARP cache allocated [see NetCache_AddrGet()]
 *               - (c) ALL ARP caches in the 'RESOLVED' state have valid hardware addresses
 *               - (d) ALL transmit buffers enqueued on any ARP cache are valid
 *               - (e) Buffer's ARP address pointers pre-configured by Network Interface to point to :
 *                   - (1) 'ARP_AddrProtocolPtr'               Pointer to the protocol address used to
 *                                                             resolve the hardware address
 *                   - (2) 'ARP_AddrHW_Ptr'                    Pointer to memory buffer to return the
 *                                                             resolved hardware address
 *                   - (3) ARP addresses                       Which MUST be in network-order
 *
 * @param    p_buf   Pointer to network buffer to transmit.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (4) See RFC #1122, Section 2.3.2.2.
 *
 * @note     (5) Since ARP Layer is the last layer to handle & queue the transmit network
 *               buffer, it is NOT necessary to increment the network buffer's reference
 *               counter to include the pending ARP cache buffer queue as a new reference
 *               to the network buffer.
 *
 * @note     (6) Some buffer controls were previously initialized in NetBuf_Get() when the packet
 *               was received at the network interface layer.  These buffer controls do NOT need
 *               to be re-initialized but are shown for completeness.
 *
 * @note     (7) A resolved multicast address still remains resolved even if any error(s) occur
 *               while adding it to the ARP cache.
 *******************************************************************************************************/
NET_CACHE_STATE NetARP_CacheHandler(NET_BUF  *p_buf,
                                    RTOS_ERR *p_err)
{
#ifdef  NET_MCAST_MODULE_EN
  NET_PROTOCOL_TYPE  protocol_type_net;
  NET_IP_ADDR_FAMILY ip_type;
  CPU_BOOLEAN        addr_protocol_multicast;
  CPU_INT08U         addr_hw_len;
#endif
  NET_IF_NBR         if_nbr;
  CPU_INT08U         *p_addr_hw;
  CPU_INT08U         *p_addr_protocol;
  NET_BUF_HDR        *p_buf_hdr;
  NET_BUF_HDR        *ptail_buf_hdr;
  NET_BUF            *ptail_buf;
  NET_CACHE_ADDR_ARP *p_cache_addr_arp;
  NET_ARP_CACHE      *p_cache;
  NET_CACHE_STATE    cache_state = NET_CACHE_STATE_NONE;
  NET_BUF_QTY        buf_max_th;
  CORE_DECLARE_IRQ_STATE;

  p_buf_hdr = &p_buf->Hdr;
  p_addr_hw = p_buf_hdr->ARP_AddrHW_Ptr;
  p_addr_protocol = p_buf_hdr->ARP_AddrProtocolPtr;

  if_nbr = p_buf_hdr->IF_Nbr;
  //                                                               ------------------ SRCH ARP CACHE ------------------
  p_cache_addr_arp = (NET_CACHE_ADDR_ARP *)NetCache_AddrSrch(NET_CACHE_TYPE_ARP,
                                                             if_nbr,
                                                             p_addr_protocol,
                                                             NET_IPv4_ADDR_SIZE);

  if (p_cache_addr_arp != DEF_NULL) {
    p_cache = (NET_ARP_CACHE *)p_cache_addr_arp->ParentPtr;
    switch (p_cache->State) {
      case NET_ARP_CACHE_STATE_RENEW:
      case NET_ARP_CACHE_STATE_RESOLVED:                        // If ARP cache resolved, copy hw addr.
        Mem_Copy(p_addr_hw,
                 p_cache_addr_arp->AddrHW,
                 NET_IF_HW_ADDR_LEN_MAX);
        cache_state = NET_CACHE_STATE_RESOLVED;
        break;

      case NET_ARP_CACHE_STATE_PEND:                            // If ARP cache pend, append buf into Q (see Note #4).
        CORE_ENTER_ATOMIC();
        buf_max_th = NetARP_CacheTxQ_MaxTh_nbr;
        CORE_EXIT_ATOMIC();

        if (p_cache_addr_arp->TxQ_Nbr >= buf_max_th) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_ADDR_UNRESOLVED);
          goto exit;
        }

        ptail_buf = p_cache_addr_arp->TxQ_Tail;
        if (ptail_buf != DEF_NULL) {                            // If Q NOT empty,    append buf @ Q tail.
          ptail_buf_hdr = &ptail_buf->Hdr;
          ptail_buf_hdr->NextSecListPtr = p_buf;
          p_buf_hdr->PrevSecListPtr = ptail_buf;
          p_cache_addr_arp->TxQ_Tail = p_buf;
        } else {                                                // Else add buf as first q'd buf.
          p_cache_addr_arp->TxQ_Head = p_buf;
          p_cache_addr_arp->TxQ_Tail = p_buf;
#if 0                                                           // Init'd in NetBuf_Get() [see Note #6].
          p_buf_hdr->PrevSecListPtr = DEF_NULL;
          p_buf_hdr->NextSecListPtr = DEF_NULL;
#endif
        }

        p_cache_addr_arp->TxQ_Nbr++;
        //                                                         Cfg buf's unlink fnct/obj to ARP cache.
        p_buf_hdr->UnlinkFnctPtr = (NET_BUF_FNCT)&NetCache_UnlinkBuf;
        p_buf_hdr->UnlinkObjPtr = (void *) p_cache_addr_arp;
        cache_state = NET_CACHE_STATE_PEND;
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE, NET_CACHE_STATE_NONE);
    }
  } else {
#ifdef  NET_MCAST_MODULE_EN
    protocol_type_net = p_buf_hdr->ProtocolHdrTypeNet;

    switch (protocol_type_net) {
      case NET_PROTOCOL_TYPE_IP_V4:
        ip_type = NET_IP_ADDR_FAMILY_IPv4;
        break;

      case NET_PROTOCOL_TYPE_IP_V6:
        ip_type = NET_IP_ADDR_FAMILY_IPv6;
        break;

      default:
        ip_type = NET_IP_ADDR_FAMILY_NONE;
        break;
    }

    addr_protocol_multicast = NetIP_IsAddrMulticast(ip_type,
                                                    p_addr_protocol,
                                                    NET_IPv4_ADDR_SIZE);

    if (addr_protocol_multicast == DEF_YES) {                   // If multicast protocol addr,      ...
      RTOS_ERR local_err;

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      PP_UNUSED_PARAM(local_err);

      if_nbr = p_buf_hdr->IF_Nbr;
      addr_hw_len = NET_IF_HW_ADDR_LEN_MAX;
      //                                                           ... convert to multicast hw addr ...
      NetIF_AddrMulticastProtocolToHW(if_nbr,
                                      p_addr_protocol,
                                      NET_IPv4_ADDR_SIZE,
                                      protocol_type_net,
                                      p_addr_hw,
                                      &addr_hw_len,
                                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      //                                                           ... & add as resolved ARP cache.
      NetCache_AddResolved(if_nbr,
                           p_addr_hw,
                           p_addr_protocol,
                           NET_CACHE_TYPE_ARP,
                           NetARP_CacheTimeout,
                           NetARP_CacheTimeout_ms,
                           &local_err);

      cache_state = NET_CACHE_STATE_RESOLVED;                   // Rtn resolved multicast hw addr (see Note #7).
      goto exit;
    }
#endif

    NetARP_CacheAddPend(p_buf, p_buf_hdr, p_addr_protocol, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
    cache_state = NET_CACHE_STATE_PEND;
  }

exit:
  return (cache_state);
}

/****************************************************************************************************//**
 *                                               NetARP_Rx()
 *
 * @brief    (1) Process received ARP packets & update ARP Cache List :
 *               - (a) Validate ARP packet
 *               - (b) Update   ARP cache
 *               - (c) Prepare & transmit an ARP Reply for a received ARP Request
 *               - (d) Free ARP packet
 *               - (e) Update receive statistics
 *
 * @param    p_buf   Pointer to network buffer that received ARP packet.
 *
 *
 * Argument(s) : p_buf   Pointer to network buffer that received ARP packet.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
void NetARP_Rx(NET_BUF  *p_buf,
               RTOS_ERR *p_err)
{
  NET_BUF_HDR *p_buf_hdr;
  NET_ARP_HDR *p_arp_hdr;
  NET_IF_NBR  if_nbr;

  NET_CTR_STAT_INC(Net_StatCtrs.ARP.RxPktCtr);

  //                                                               -------------- VALIDATE RX'D ARP PKT ---------------
  p_buf_hdr = &p_buf->Hdr;

  p_arp_hdr = (NET_ARP_HDR *)&p_buf->DataPtr[p_buf_hdr->ARP_MsgIx];
  NetARP_RxPktValidate(p_buf_hdr, p_arp_hdr, p_err);            // Validate rx'd pkt.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               ----------------- UPDATE ARP CACHE -----------------
  if_nbr = p_buf_hdr->IF_Nbr;
  NetARP_RxPktCacheUpdate(if_nbr, p_arp_hdr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               ------------------- TX ARP REPLY -------------------
  NetARP_RxPktReply(if_nbr, p_arp_hdr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               ---------- FREE ARP PKT / UPDATE RX STATS ----------
  NetARP_RxPktFree(p_buf);

  NET_CTR_STAT_INC(Net_StatCtrs.ARP.RxMsgCompCtr);

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxPktDisCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetARP_TxReq()
 *
 * @brief    (1) Prepare & transmit an ARP Request to resolve a pending ARP cache :
 *               - (a) Get network interface's hardware & protocol addresses
 *               - (b) Prepare  ARP Request packet :
 *                   - (1) Configure sender's hardware address as this interface's hardware address
 *                   - (2) Configure target's hardware address as NULL since unknown
 *                   - (3) Configure sender's protocol address as this interface's protocol address
 *                   - (4) Configure target's protocol address as the protocol address listed in the ARP cache
 *                   - (5) Configure ARP operation as ARP Request
 *
 * @param    p_cache     Pointer to an ARP cache.
 *
 * @note     (2) Do NOT need to verify success of ARP Request since failure will cause timeouts & retries.
 *******************************************************************************************************/
void NetARP_TxReq(NET_CACHE_ADDR_ARP *p_cache_addr_arp)
{
  NET_ARP_CACHE *p_cache_arp;
  CPU_INT08U    addr_hw_len;
  CPU_INT08U    addr_hw_sender[NET_IF_HW_ADDR_LEN_MAX];
  CPU_INT08U    *p_addr_hw_target;
  CPU_INT08U    *p_addr_protocol_target;
  CPU_INT08U    *p_addr_protocol_sender;
  NET_IF_NBR    if_nbr;
  CPU_INT16U    op_code;
  RTOS_ERR      local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               ------------ GET HW & PROTOCOL IF ADDRS ------------
  addr_hw_len = NET_IF_HW_ADDR_LEN_MAX;
  NetIF_AddrHW_GetHandler(p_cache_addr_arp->IF_Nbr,
                          &addr_hw_sender[0],
                          &addr_hw_len,
                          &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               --------- CFG ARP REQ FROM ARP CACHE DATA ----------
  //                                                               See Note #1b.
  if_nbr = p_cache_addr_arp->IF_Nbr;
  p_addr_protocol_target = &p_cache_addr_arp->AddrProtocol[0];
  p_addr_protocol_sender = &p_cache_addr_arp->AddrProtocolSender[0];
  op_code = NET_ARP_HDR_OP_REQ;

  p_cache_arp = (NET_ARP_CACHE *)p_cache_addr_arp->ParentPtr;
  if (p_cache_arp->State != NET_ARP_CACHE_STATE_RENEW) {
    p_addr_hw_target = DEF_NULL;
  } else {
    p_addr_hw_target = p_cache_addr_arp->AddrHW;
  }

  NetARP_Tx(if_nbr,
            &addr_hw_sender[0],
            p_addr_hw_target,
            p_addr_protocol_sender,
            p_addr_protocol_target,
            op_code,
            &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.ARP.TxMsgReqCtr);

  p_cache_arp = (NET_ARP_CACHE *)p_cache_addr_arp->ParentPtr;
  RTOS_ASSERT_CRITICAL((p_cache_arp != DEF_NULL), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_cache_arp->ReqAttemptsCtr++;                                // Inc req attempts ctr.

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetARP_CacheAddPend()
 *
 * @brief    (1) Add a 'PENDING' ARP cache into the ARP Cache List & transmit an ARP Request :
 *               - (a) Configure ARP cache :
 *                   - (1) Get sender protocol sender
 *                   - (2) Get default-configured ARP cache
 *                   - (3) ARP cache state
 *                   - (4) Enqueue transmit buffer to ARP cache queue
 *               - (b) Insert   ARP cache into ARP Cache List
 *               - (c) Transmit ARP Request to resolve ARP cache
 *
 * @param    p_buf               Pointer to network buffer to transmit.
 *
 * @param    p_buf_hdr           Pointer to network buffer header.
 *
 * @param    p_addr_protocol     Pointer to protocol address (see Note #2).
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) 'p_addr_protocol' MUST point to a valid protocol address in network-order.
 *               See also 'NetARP_CacheHandler()  Note #2e3'.
 *
 * @note     (3) See RFC #1122, Section 2.3.2.2.
 *
 * @note     (4) Since ARP Layer is the last layer to handle & queue the transmit network
 *               buffer, it is NOT necessary to increment the network buffer's reference
 *               counter to include the pending ARP cache buffer queue as a new reference
 *               to the network buffer.
 *
 * @note     (5) Some buffer controls were previously initialized in NetBuf_Get() when the packet
 *               was received at the network interface layer.  These buffer controls do NOT need
 *               to be re-initialized but are shown for completeness.
 *******************************************************************************************************/
void NetARP_CacheAddPend(NET_BUF     *p_buf,
                         NET_BUF_HDR *p_buf_hdr,
                         CPU_INT08U  *p_addr_protocol,
                         RTOS_ERR    *p_err)
{
  NET_CACHE_ADDR_ARP *p_cache_addr_arp;
  NET_ARP_CACHE      *p_cache_arp;
  CPU_INT08U         addr_protocol_sender[NET_IPv4_ADDR_SIZE];
  NET_IF_NBR         if_nbr;
  CPU_INT32U         timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ------------------ CFG ARP CACHE -------------------
  //                                                               Copy sender protocol addr to net order.
  //                                                               Cfg protocol addr generically from IP addr.
  NET_UTIL_VAL_COPY_SET_NET_32(&addr_protocol_sender[0], &p_buf_hdr->IP_AddrSrc);

  if_nbr = p_buf_hdr->IF_Nbr;
  CORE_ENTER_ATOMIC();
  timeout_ms = NetARP_ReqTimeoutPend_ms;
  CORE_EXIT_ATOMIC();

  p_cache_addr_arp = (NET_CACHE_ADDR_ARP *)NetCache_CfgAddrs(NET_CACHE_TYPE_ARP,
                                                             if_nbr,
                                                             DEF_NULL,
                                                             NET_IF_HW_ADDR_LEN_MAX,
                                                             p_addr_protocol,
                                                             &addr_protocol_sender[0],
                                                             NET_IPv4_ADDR_SIZE,
                                                             DEF_YES,
                                                             NetARP_CacheReqTimeout,
                                                             timeout_ms,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               Cfg buf's unlink fnct/obj to ARP cache.
  p_buf_hdr->UnlinkFnctPtr = NetCache_UnlinkBuf;
  p_buf_hdr->UnlinkObjPtr = p_cache_addr_arp;

  //                                                               Q buf to ARP cache (see Note #3).
  p_cache_addr_arp->TxQ_Head = p_buf;
  p_cache_addr_arp->TxQ_Tail = p_buf;
  p_cache_addr_arp->TxQ_Nbr++;

  p_cache_arp = (NET_ARP_CACHE *)p_cache_addr_arp->ParentPtr;

  p_cache_arp->State = NET_ARP_CACHE_STATE_PEND;

  //                                                               ------- INSERT ARP CACHE INTO ARP CACHE LIST -------
  NetCache_Insert((NET_CACHE_ADDR *)p_cache_addr_arp);
  //                                                               -------------------- TX ARP REQ --------------------
  NetARP_TxReq(p_cache_addr_arp);                               // Tx ARP req to resolve ARP cache.

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetARP_CacheRemoveEntry()
 *
 * @brief    Remove an entry in the ARP cache.
 *
 * @param    p_cache     Pointer to the ARP entry to remove.
 *
 * @param    tmr_free    Indicate if the neighbor cache timer must be freed:
 *                       DEF_YES  Free timer.
 *                       DEF_NO   Do not free timer.
 *******************************************************************************************************/
void NetARP_CacheRemoveEntry(NET_ARP_CACHE *p_cache,
                             CPU_BOOLEAN   tmr_free)
{
  NET_CACHE_ADDR_ARP *p_cache_addr_arp;

  p_cache_addr_arp = p_cache->CacheAddrPtr;

  NetCache_Remove((NET_CACHE_ADDR *)p_cache_addr_arp,           // Clr Addr Cache and free tmr if specified.
                  tmr_free);

  p_cache->TmrPtr = DEF_NULL;
  p_cache->ReqAttemptsCtr = 0u;
  p_cache->State = NET_ARP_CACHE_STATE_FREE;
  p_cache->Flags = NET_CACHE_FLAG_NONE;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetARP_RxPktValidate()
 *
 * @brief    (1) Validate received ARP packet :
 *               - (a) Validate the received packet's following ARP header fields :
 *                   - (1) Hardware  Type
 *                   - (2) Hardware  Address Length
 *                   - (3) Hardware  Address : Sender's
 *                   - (4) Protocol  Type
 *                   - (5) Protocol  Address Length
 *                   - (6) Protocol  Address : Sender's
 *                   - (7) Operation Code
 *               - (b) Convert the following ARP header fields from network-order to host-order :
 *                   - (1) Hardware  Type
 *                   - (2) Protocol  Type
 *                   - (3) Operation Code                                          See Note #1bB
 *                       - (A) These fields are NOT converted directly in the received packet buffer's
 *                             data area but are converted in local or network buffer variables ONLY.
 *                       - (B) To avoid storing the ARP operation code in a network buffer variable &
 *                             passing an additional pointer to the network buffer header that received
 *                             ARP packet, ARP operation code is converted in the following functions :
 *                           - (1) NetARP_RxPktValidate()
 *                           - (2) NetARP_RxPktCacheUpdate()
 *                           - (3) NetARP_RxPktReply()
 *                           - (4) NetARP_RxPktIsTargetThisHost()
 *                       - (C) Hardware & Protocol Addresses are NOT converted from network-order to
 *                             host-order & MUST be accessed as multi-octet arrays.
 *               - (c) Validate the received packet's following ARP packet controls :
 *                   - (1) ARP message length                                      See Note #4
 *
 * @param    p_buf_hdr   Pointer to network buffer header that received ARP packet.
 *
 * @param    p_arp_hdr   Pointer to received packet's ARP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See RFC #826, Section 'Packet Format' for ARP packet header format.
 *
 * @note     (3) See RFC #826, Section 'Packet Generation' & Section 'Packet Reception'
 *
 * @note     (4) See RFC #894,  Section 'Frame Format'
 *                   RFC #1042, Section 'Frame Format and MAC Level Issues : For all hardware types'
 *                   RFC #893,  Section 'Introduction'
 *                   RFC #1122, Section 2.3.1
 *******************************************************************************************************/
static void NetARP_RxPktValidate(NET_BUF_HDR *p_buf_hdr,
                                 NET_ARP_HDR *p_arp_hdr,
                                 RTOS_ERR    *p_err)
{
  CPU_INT08U         addr_hw_target[NET_IF_HW_ADDR_LEN_MAX];
  CPU_INT08U         addr_hw_len;
  CPU_INT08U         *p_addr_target_hw;
  CPU_BOOLEAN        target_hw_verifd;
  CPU_INT16U         hw_type;
  CPU_INT16U         protocol_type_arp;
  NET_IP_ADDR_FAMILY ip_type;
  CPU_INT16U         op_code;
  CPU_BOOLEAN        rx_broadcast;
  CPU_BOOLEAN        valid;
  NET_IPv4_ADDR      addr_ip;
  RTOS_ERR           local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  PP_UNUSED_PARAM(local_err);

  //                                                               ------------ VALIDATE ARP HW TYPE/ADDR -------------
  NET_UTIL_VAL_COPY_GET_NET_16(&hw_type, &p_arp_hdr->AddrHW_Type);
  if (hw_type != NET_ADDR_HW_TYPE_802x) {
    NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvHW_TypeCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (p_arp_hdr->AddrHW_Len != NET_IF_HW_ADDR_LEN_MAX) {
    NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvHW_AddrLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  valid = NetIF_AddrHW_IsValidHandler(p_buf_hdr->IF_Nbr,
                                      &p_arp_hdr->AddrHW_Sender[0],
                                      &local_err);
  if (valid != DEF_YES) {
    NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvHW_AddrCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               --------- VALIDATE ARP PROTOCOL TYPE/ADDR ----------
  NET_UTIL_VAL_COPY_GET_NET_16(&protocol_type_arp, &p_arp_hdr->AddrProtocolType);
  if (protocol_type_arp != NET_ARP_PROTOCOL_TYPE_IP_V4) {
    NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvProtocolTypeCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (p_arp_hdr->AddrProtocolLen != NET_IPv4_ADDR_SIZE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               Get net protocol type.
  switch (protocol_type_arp) {
    case NET_ARP_PROTOCOL_TYPE_IP_V4:
      ip_type = NET_IP_ADDR_FAMILY_IPv4;
      break;

    default:
      ip_type = NET_IP_ADDR_FAMILY_NONE;
      break;
  }

  NET_UTIL_VAL_COPY_GET_NET_32(&addr_ip, &p_arp_hdr->AddrProtocolSender[0]);
  if (addr_ip == NET_ARP_PROTOCOL_ADDR_NONE) {
    valid = DEF_YES;
  } else {
    valid = NetIP_IsAddrValid(ip_type,
                              &p_arp_hdr->AddrProtocolSender[0],
                              NET_IPv4_ADDR_SIZE);
    if (valid != DEF_YES) {
      NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvProtocolAddrCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  }

  //                                                               --------------- VALIDATE ARP OP CODE ---------------
  NET_UTIL_VAL_COPY_GET_NET_16(&op_code, &p_arp_hdr->OpCode);
  rx_broadcast = DEF_BIT_IS_SET(p_buf_hdr->Flags, NET_BUF_FLAG_RX_BROADCAST);
  switch (op_code) {
    case NET_ARP_HDR_OP_REQ:
      if (NetARP_AddrFltrEn == DEF_YES) {
        if (rx_broadcast != DEF_YES) {                          // If rx'd ARP Req NOT broadcast (see Note #3)  ...
          addr_hw_len = NET_IF_HW_ADDR_LEN_MAX;
          NetIF_AddrHW_GetHandler(p_buf_hdr->IF_Nbr,
                                  &addr_hw_target[0],
                                  &addr_hw_len,
                                  p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvOpAddrCtr);
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          }

          p_addr_target_hw = p_arp_hdr->AddrHW_Target;
          target_hw_verifd = Mem_Cmp(p_addr_target_hw,
                                     &addr_hw_target[0],
                                     NET_IF_HW_ADDR_LEN_MAX);
          if (target_hw_verifd != DEF_YES) {                    // ... & NOT addr'd to this host (see Note #3), ...
            NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvOpAddrCtr);
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                  // ... rtn err / discard pkt (see Note #3).
            goto exit;
          }
        }
      }
      break;

    case NET_ARP_HDR_OP_REPLY:
      break;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvOpCodeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  //                                                               --------------- VALIDATE ARP MSG LEN ---------------
  p_buf_hdr->ARP_MsgLen = NET_ARP_HDR_SIZE;
  if (p_buf_hdr->ARP_MsgLen > p_buf_hdr->DataLen) {             // If ARP msg len > rem pkt data len, ...
    NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvMsgLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                          // ... rtn err (see Note #4).
    goto exit;
  }

  p_buf_hdr->DataLen = (NET_BUF_SIZE)p_buf_hdr->ARP_MsgLen;     // Trunc pkt data len to ARP msg len (see Note #4).

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetARP_RxPktCacheUpdate()
 *
 * @brief    (1) Update an ARP cache based on received ARP packet's sender addresses :
 *               - (a) Verify ARP message's intended target address is this host
 *               - (b) Search ARP Cache List
 *               - (c) Update ARP cache
 *
 * @param    if_nbr      Interface number the packet was received from.
 *
 * @param    p_arp_hdr   Pointer to received packet's ARP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See RFC #826, Section 'Packet Reception'
 *
 * @note     (3) A configurable ARP address filtering feature is provided to selectively filter & discard
 *               ALL misdirected or incorrectly received ARP messages (see 'net_cfg.h  ADDRESS RESOLUTION
 *               PROTOCOL LAYER CONFIGURATION  Note #3') :
 *               - (a) When ENABLED,  the ARP address filter discards :
 *                   - (1) ALL misdirected, broadcasted, or incorrectly received ARP messages.
 *                   - (2) Any ARP Reply received for this host for which NO corresponding ARP cache currently
 *                         exists.  Note that such an ARP Reply may be a legitimate, yet late, ARP Reply to a
 *                         pending ARP Request that has timed-out & been removed from the ARP Cache List.
 *               - (b) When DISABLED, the ARP address filter discards :
 *                   - (1) Any misdirected or incorrectly received ARP messages if the sender's address(s)
 *                         are NOT already cached.
 *
 * @note     (4) See RFC # 826, Section 'Related issue'
 *               RFC #1122, Section 2.3.2.1
 *
 * @note     (5) However, Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 4.5 'ARP Examples :
 *               ARP Cache Timeout' adds that "the Host Requirements RFC [#1122] says that this timeout
 *               should occur even if the entry is in use, but most Berkeley-derived implementations do
 *               not do this -- they restart the timeout each time the entry is referenced".
 *******************************************************************************************************/
static void NetARP_RxPktCacheUpdate(NET_IF_NBR  if_nbr,
                                    NET_ARP_HDR *p_arp_hdr,
                                    RTOS_ERR    *p_err)
{
  CPU_INT16U         op_code = 0u;
  CPU_BOOLEAN        cache_add = DEF_NO;
  CPU_INT08U         *p_addr_sender_hw = DEF_NULL;
  CPU_INT08U         *p_addr_sender_protocol = DEF_NULL;
  NET_CACHE_ADDR_ARP *p_cache_addr_arp = DEF_NULL;
  NET_ARP_CACHE      *p_cache_arp = DEF_NULL;
  NET_BUF            *p_buf_head = DEF_NULL;
  CPU_INT32U         timeout_ms = 0u;
  CPU_BOOLEAN        dest_is_host = DEF_NO;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ----------------- CHK TARGET ADDR ------------------
  dest_is_host = NetARP_RxPktIsTargetThisHost(if_nbr, p_arp_hdr, p_err);
  if (NetARP_AddrFltrEn == DEF_YES) {
    if (dest_is_host != DEF_YES) {                              // Fltr misdirected rx'd ARP msgs (see Note #3b1).
      NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxPktInvDest);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  } else {
    cache_add = dest_is_host;
  }

  //                                                               ------------------ SRCH ARP CACHE ------------------
  p_addr_sender_hw = p_arp_hdr->AddrHW_Sender;
  p_addr_sender_protocol = p_arp_hdr->AddrProtocolSender;

  p_cache_addr_arp = (NET_CACHE_ADDR_ARP *)NetCache_AddrSrch(NET_CACHE_TYPE_ARP,
                                                             if_nbr,
                                                             p_addr_sender_protocol,
                                                             NET_IPv4_ADDR_SIZE);

  //                                                               ----------------- UPDATE ARP CACHE -----------------
  if (p_cache_addr_arp != DEF_NULL) {                           // If ARP cache found, chk state.
    p_cache_arp = (NET_ARP_CACHE *)p_cache_addr_arp->ParentPtr;
    switch (p_cache_arp->State) {
      case NET_ARP_CACHE_STATE_RENEW:
      case NET_ARP_CACHE_STATE_RESOLVED:                        // If ARP cache resolved, update sender's hw addr.
        Mem_Copy(&p_cache_addr_arp->AddrHW[0],
                 p_addr_sender_hw,
                 NET_IF_HW_ADDR_LEN_MAX);
        //                                                         Reset ARP cache tmr (see Note #4).
        CORE_ENTER_ATOMIC();
        timeout_ms = NetARP_CacheTimeout_ms;
        CORE_EXIT_ATOMIC();
        if (p_cache_arp->TmrPtr != DEF_NULL) {
          NetTmr_Set(p_cache_arp->TmrPtr,
                     NetARP_CacheTimeout,
                     timeout_ms);
        } else {
          p_cache_arp->TmrPtr = NetTmr_Get(NetARP_CacheTimeout,
                                           p_cache_arp,
                                           timeout_ms,
                                           NET_TMR_OPT_NONE,
                                           p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
        }
        p_cache_arp->ReqAttemptsCtr = 0;
        p_cache_arp->State = NET_ARP_CACHE_STATE_RESOLVED;
        break;

      case NET_ARP_CACHE_STATE_PEND:                           // If ARP cache pend, add sender's hw addr, ...
        Mem_Copy(&p_cache_addr_arp->AddrHW[0],
                 p_addr_sender_hw,
                 NET_IF_HW_ADDR_LEN_MAX);

        p_cache_addr_arp->AddrHW_Valid = DEF_YES;
        //                                                         Reset ARP cache tmr (see Note #4).
        CORE_ENTER_ATOMIC();
        timeout_ms = NetARP_CacheTimeout_ms;
        CORE_EXIT_ATOMIC();
        if (p_cache_arp->TmrPtr != DEF_NULL) {
          NetTmr_Set(p_cache_arp->TmrPtr,
                     NetARP_CacheTimeout,
                     timeout_ms);
        } else {
          p_cache_arp->TmrPtr = NetTmr_Get(NetARP_CacheTimeout,
                                           p_cache_arp,
                                           timeout_ms,
                                           NET_TMR_OPT_NONE,
                                           p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
        }

        p_buf_head = p_cache_addr_arp->TxQ_Head;
        p_cache_addr_arp->TxQ_Head = DEF_NULL;
        p_cache_addr_arp->TxQ_Tail = DEF_NULL;
        p_cache_addr_arp->TxQ_Nbr = 0;

        if (p_buf_head != DEF_NULL) {
          NetCache_TxPktHandler(NET_PROTOCOL_TYPE_ARP,
                                p_buf_head,                     // ... & handle/tx cache's buf Q.
                                p_addr_sender_hw);
        }
        p_cache_arp->ReqAttemptsCtr = 0;
        p_cache_arp->State = NET_ARP_CACHE_STATE_RESOLVED;
        break;

      default:
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
        goto exit;
    }
  } else {
    //                                                             Else add new ARP cache into ARP Cache List.
    if (NetARP_AddrFltrEn == DEF_YES) {                         // If ARP addr fltr en'd          ..
      NET_UTIL_VAL_COPY_GET_NET_16(&op_code, &p_arp_hdr->OpCode);
      if (op_code != NET_ARP_HDR_OP_REQ) {                      // .. but ARP pkt NOT an ARP Req, ..
                                                                // .. do  NOT add new ARP cache (see Note #3a2).
        NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxPktTargetReplyCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
    } else {
      //                                                           If ARP addr fltr dis'd            ..
      if (cache_add != DEF_YES) {                               // .. &   ARP pkt NOT for this host, ..
                                                                // .. do  NOT add new ARP cache (see Note #3b1).
        NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxPktInvDest);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;                                              // Err rtn'd by NetARP_RxPktIsTargetThisHost().
      }
    }

    NetCache_AddResolved(if_nbr,
                         p_addr_sender_hw,
                         p_addr_sender_protocol,
                         NET_CACHE_TYPE_ARP,
                         NetARP_CacheTimeout,
                         NetARP_CacheTimeout_ms,
                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetARP_RxPktReply()
 *
 * @brief    Reply to received ARP message, if necessary.
 *
 * @param    if_nbr      Interface number the packet was received from.
 *
 * @param    p_arp_hdr   Pointer to received packet's ARP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) ARP Reply already transmitted the ARP cache's transmit buffer queue, if any, in
 *               NetARP_RxPktCacheUpdate(); no further action required.
 *
 * @note     (2) Default case already invalidated in NetARP_RxPktValidate().  However, the default
 *               case is included as an extra precaution in case 'OpCode' is incorrectly modified.
 *******************************************************************************************************/
static void NetARP_RxPktReply(NET_IF_NBR  if_nbr,
                              NET_ARP_HDR *p_arp_hdr,
                              RTOS_ERR    *p_err)
{
  CPU_INT16U op_code;

  NET_UTIL_VAL_COPY_GET_NET_16(&op_code, &p_arp_hdr->OpCode);

  switch (op_code) {
    case NET_ARP_HDR_OP_REQ:                                    // Use rx'd ARP Req to tx ARP Reply.
      NetARP_TxReply(if_nbr, p_arp_hdr);
      NET_CTR_STAT_INC(Net_StatCtrs.ARP.RxMsgReqCompCtr);
      break;

    case NET_ARP_HDR_OP_REPLY:                                  // See Note #1.
      NET_CTR_STAT_INC(Net_StatCtrs.ARP.RxMsgReplyCompCtr);
      break;

    default:                                                    // See Note #2.
      NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvOpCodeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetARP_RxPktIsTargetThisHost()
 *
 * @brief    (1) Determine if this host is the intended target of the received ARP message :
 *               - (a) Validate interface
 *               - (b) Get    target hardware address
 *               - (c) Verify target hardware address
 *               - (d) Get    target protocol address
 *               - (e) Verify target protocol address :
 *                   - (1) Check for protocol initialization address
 *                   - (2) Check for protocol address conflict
 *               - (f) Return target validation
 *
 * @param    if_nbr      Interface number the packet was received from.
 *
 * @param    p_arp_hdr   Pointer to received packet's ARP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (3) Default case already invalidated in NetARP_RxPktValidate().  However, the default case
 *               is included as an extra precaution in case 'OpCode' is incorrectly modified.
 *
 * @note     (4) See RFC #3927, Section 2.5
 *               See also 'NetARP_IsAddrProtocolConflict()  Note #3'.
 *******************************************************************************************************/
static CPU_BOOLEAN NetARP_RxPktIsTargetThisHost(NET_IF_NBR  if_nbr,
                                                NET_ARP_HDR *p_arp_hdr,
                                                RTOS_ERR    *p_err)
{
  CPU_INT32U         addr_protocol;
  CPU_INT16U         op_code;
  CPU_INT08U         *p_addr_target_hw;
  CPU_INT08U         *p_addr_sender_hw;
  CPU_INT08U         addr_if_hw[NET_IF_HW_ADDR_LEN_MAX];
  CPU_INT08U         addr_hw_len;
  CPU_INT16U         protocol_type_arp;
  NET_IP_ADDR_FAMILY ip_type;
  CPU_BOOLEAN        target_hw_verifd;
  CPU_BOOLEAN        sender_hw_verifd;
  CPU_BOOLEAN        is_cfgd;
  CPU_BOOLEAN        this_host = DEF_NO;
  RTOS_ERR           local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  PP_UNUSED_PARAM(local_err);

  //                                                               ------------------- GET HW ADDR --------------------
  addr_hw_len = NET_IF_HW_ADDR_LEN_MAX;
  NetIF_AddrHW_GetHandler(if_nbr,
                          &addr_if_hw[0],
                          &addr_hw_len,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               -------------- VERIFY TARGET HW ADDR ---------------
  if (NetARP_AddrFltrEn == DEF_YES) {
    NET_UTIL_VAL_COPY_GET_NET_16(&op_code, &p_arp_hdr->OpCode);
    switch (op_code) {
      case NET_ARP_HDR_OP_REQ:
        target_hw_verifd = DEF_YES;
        break;

      case NET_ARP_HDR_OP_REPLY:
        p_addr_target_hw = p_arp_hdr->AddrHW_Target;
        target_hw_verifd = Mem_Cmp(p_addr_target_hw,
                                   &addr_if_hw[0],
                                   NET_IF_HW_ADDR_LEN_MAX);
        break;

      default:                                                  // See Note #3.
        NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvOpCodeCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
    }
  } else {
    target_hw_verifd = DEF_YES;
  }

  //                                                               ---------------- GET PROTOCOL ADDR -----------------
  NET_UTIL_VAL_COPY_GET_NET_16(&protocol_type_arp, &p_arp_hdr->AddrProtocolType);
  switch (protocol_type_arp) {
    case NET_ARP_PROTOCOL_TYPE_IP_V4:
      ip_type = NET_IP_ADDR_FAMILY_IPv4;
      break;

    default:
      ip_type = NET_IP_ADDR_FAMILY_NONE;
      break;
  }

  NET_UTIL_VAL_COPY_GET_NET_32((&addr_protocol), (p_arp_hdr->AddrProtocolTarget));
  is_cfgd = NetIP_IsAddrCfgd(if_nbr,
                             (NET_IP_ADDR *)&addr_protocol,
                             ip_type,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (is_cfgd == DEF_YES) {
    //                                                             ------------ CHK PROTOCOL ADDR CONFLICT ------------
    p_addr_sender_hw = p_arp_hdr->AddrHW_Sender;                // Cmp sender's hw addr           (see Note #4).
    sender_hw_verifd = Mem_Cmp(p_addr_sender_hw,
                               &addr_if_hw[0],
                               NET_IF_HW_ADDR_LEN_MAX);

    if (sender_hw_verifd == DEF_YES) {                          // If sender hw addr is the same, ...
                                                                // ... invalidate configured addr (see Note #4).
      NetIP_InvalidateAddr(if_nbr,
                           (NET_IP_ADDR *)&p_arp_hdr->AddrProtocolTarget[0],
                           ip_type,
                           &local_err);
    }
  }

  //                                                               ------- CHK FOR DYNAMIC ADDRESS IN PROGRESS --------
  switch (op_code) {
    case NET_ARP_HDR_OP_REQ:
      break;

    case NET_ARP_HDR_OP_REPLY:
      is_cfgd |= NetIP_IsDynAddrCfgd(if_nbr, ip_type, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.ARP.RxInvOpCodeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  //                                                               -------------- RTN TARGET VALIDATION ---------------
  this_host = ((target_hw_verifd == DEF_YES)
               && (is_cfgd == DEF_YES)) ? DEF_YES : DEF_NO;

exit:
  return (this_host);
}

/****************************************************************************************************//**
 *                                           NetARP_RxPktFree()
 *
 * @brief    Free network buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *******************************************************************************************************/
static void NetARP_RxPktFree(NET_BUF *p_buf)
{
  (void)NetBuf_FreeBuf(p_buf, DEF_NULL);
}

/****************************************************************************************************//**
 *                                               NetARP_Tx()
 *
 * @brief    (1) Prepare & transmit an ARP Request or ARP Reply :
 *               - (a) Get network buffer for ARP transmit packet
 *               - (b) Prepare & transmit packet
 *               - (c) Free      transmit packet buffer(s)
 *               - (d) Update    transmit statistics
 *
 * @param    if_nbr                  Interface number to transmit ARP Request.
 *
 * @param    p_addr_hw_sender        Pointer to sender's hardware address (see Note #2).
 *
 * @param    p_addr_hw_target        Pointer to target's hardware address (see Note #2).
 *
 * @param    p_addr_protocol_sender  Pointer to sender's protocol address (see Note #2).
 *
 * @param    p_addr_protocol_target  Pointer to target's protocol address (see Note #2).
 *
 * @param    op_code                 ARP operation : Request or Reply.
 *
 * @param    p_err                   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) ARP addresses MUST be in network-order.
 *
 * @note     (3) Assumes network buffer's protocol header size is large enough to accommodate ARP header
 *               size (see 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1').
 *
 * @note     (4) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
static void NetARP_Tx(NET_IF_NBR if_nbr,
                      CPU_INT08U *p_addr_hw_sender,
                      CPU_INT08U *p_addr_hw_target,
                      CPU_INT08U *p_addr_protocol_sender,
                      CPU_INT08U *p_addr_protocol_target,
                      CPU_INT16U op_code,
                      RTOS_ERR   *p_err)
{
  CPU_INT16U msg_ix;
  CPU_INT16U msg_ix_offset;
  NET_BUF    *p_buf;

  //                                                               --------------------- GET BUF ----------------------
  msg_ix = 0u;
  NetARP_TxIxDataGet(if_nbr,
                     NET_ARP_MSG_LEN_DATA,
                     &msg_ix,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_buf = NetBuf_Get(if_nbr,
                     NET_TRANSACTION_TX,
                     NET_ARP_MSG_LEN_DATA,
                     msg_ix,
                     &msg_ix_offset,
                     NET_BUF_FLAG_NONE,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  msg_ix += msg_ix_offset;

  //                                                               ---------------- PREPARE/TX ARP PKT ----------------
  NetARP_TxPktPrepareHdr(p_buf,
                         msg_ix,
                         p_addr_hw_sender,
                         p_addr_hw_target,
                         p_addr_protocol_sender,
                         p_addr_protocol_target,
                         op_code);

  NetIF_Tx(p_buf, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  NetARP_TxPktFree(p_buf);

  NET_CTR_STAT_INC(Net_StatCtrs.ARP.TxMsgCtr);

  goto exit;

exit_discard:
  NetARP_TxPktDiscard(p_buf);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetARP_TxReply()
 *
 * @brief    (1) Prepare & transmit an ARP Reply in response to an ARP Request :
 *               - (a) Configure sender's hardware address as this interface's           hardware address
 *               - (b) Configure target's hardware address from the ARP Request's sender hardware address
 *               - (c) Configure sender's protocol address from the ARP Request's target protocol address
 *               - (d) Configure target's protocol address from the ARP Request's sender protocol address
 *               - (e) Configure ARP operation as ARP Reply
 *
 * @param    if_nbr      Interface number to transmit ARP Reply.
 *
 *
 * Argument(s) : if_nbr      Interface number to transmit ARP Reply.
 *
 *               p_arp_hdr   Pointer to a packet's ARP header.
 *
 * Return(s)   : None.
 *
 * Note(s)     : (2) Do NOT need to verify success of ARP Reply since failure will cause timeouts & retries.
 *******************************************************************************************************/
static void NetARP_TxReply(NET_IF_NBR  if_nbr,
                           NET_ARP_HDR *p_arp_hdr)
{
  CPU_INT08U addr_hw_sender[NET_IF_HW_ADDR_LEN_MAX];
  CPU_INT08U *p_addr_hw_target;
  CPU_INT08U *p_addr_protocol_sender;
  CPU_INT08U *p_addr_protocol_target;
  CPU_INT08U addr_hw_len;
  CPU_INT16U op_code;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               Cfg ARP Reply from ARP Req (see Note #1).
  addr_hw_len = NET_IF_HW_ADDR_LEN_MAX;
  NetIF_AddrHW_GetHandler(if_nbr,
                          &addr_hw_sender[0],
                          &addr_hw_len,
                          &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_addr_hw_target = &p_arp_hdr->AddrHW_Sender[0];
  p_addr_protocol_sender = &p_arp_hdr->AddrProtocolTarget[0];
  p_addr_protocol_target = &p_arp_hdr->AddrProtocolSender[0];

  op_code = NET_ARP_HDR_OP_REPLY;

  NetARP_Tx(if_nbr,
            &addr_hw_sender[0],
            p_addr_hw_target,
            p_addr_protocol_sender,
            p_addr_protocol_target,
            op_code,
            &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.ARP.TxMsgReplyCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetARP_TxPktPrepareHdr()
 *
 * @brief    (1) Prepare ARP packet header :
 *               - (a) Update network buffer's index & length controls
 *               - (b) Prepare the transmit packet's following ARP header fields :
 *                   - (1) Hardware  Type
 *                   - (2) Protocol  Type
 *                   - (3) Hardware  Address Length
 *                   - (4) Protocol  Address Length
 *                   - (5) Operation Code
 *                   - (6) Sender's  Hardware Address
 *                   - (7) Sender's  Protocol Address
 *                   - (8) Target's  Hardware Address
 *                   - (9) Target's  Protocol Address
 *               - (c) Convert the following ARP header fields from host-order to network-order :
 *                   - (1) Hardware  Type
 *                   - (2) Protocol  Type
 *                   - (3) Operation Code
 *               - (d) Configure ARP protocol address pointer                      See Note #2
 *
 * @param    p_buf                   Pointer to network buffer to prepare ARP packet.
 *
 * @param    msg_ix                  Buffer index to prepare ARP packet.
 *
 * @param    p_addr_hw_sender        Pointer to sender's hardware address (see Note #2).
 *
 * @param    p_addr_hw_target        Pointer to target's hardware address (see Note #2).
 *
 * @param    p_addr_protocol_sender  Pointer to sender's protocol address (see Note #2).
 *
 * @param    p_addr_protocol_target  Pointer to target's protocol address (see Note #2).
 *
 * @param    op_code                 ARP operation : Request or Reply.
 *
 * @param    p_err                   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) ARP addresses MUST be in network-order.
 *
 * @note     (3) Some buffer controls were previously initialized in NetBuf_Get() when the buffer was
 *               allocated.  These buffer controls do NOT need to be re-initialized but are shown for
 *               completeness.
 *******************************************************************************************************/
static void NetARP_TxPktPrepareHdr(NET_BUF    *p_buf,
                                   CPU_INT16U msg_ix,
                                   CPU_INT08U *p_addr_hw_sender,
                                   CPU_INT08U *p_addr_hw_target,
                                   CPU_INT08U *p_addr_protocol_sender,
                                   CPU_INT08U *p_addr_protocol_target,
                                   CPU_INT16U op_code)
{
  NET_BUF_HDR *p_buf_hdr;
  NET_ARP_HDR *p_arp_hdr;

  //                                                               ----------------- UPDATE BUF CTRLS -----------------
  p_buf_hdr = &p_buf->Hdr;
  p_buf_hdr->ARP_MsgIx = msg_ix;
  p_buf_hdr->ARP_MsgLen = NET_ARP_HDR_SIZE;
  p_buf_hdr->TotLen = (NET_BUF_SIZE)p_buf_hdr->ARP_MsgLen;
  p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_ARP;
  p_buf_hdr->ProtocolHdrTypeIF_Sub = NET_PROTOCOL_TYPE_ARP;

  //                                                               ----------------- PREPARE ARP HDR ------------------
  p_arp_hdr = (NET_ARP_HDR *)&p_buf->DataPtr[p_buf_hdr->ARP_MsgIx];

  //                                                               ---------- PREPARE ARP HW/PROTOCOL TYPES -----------
  NET_UTIL_VAL_SET_NET_16(&p_arp_hdr->AddrHW_Type, NET_ARP_HW_TYPE_ETHER);
  NET_UTIL_VAL_SET_NET_16(&p_arp_hdr->AddrProtocolType, NET_ARP_PROTOCOL_TYPE_IP_V4);

  //                                                               -------- PREPARE ARP HW/PROTOCOL ADDR LENS ---------
  p_arp_hdr->AddrHW_Len = NET_IF_HW_ADDR_LEN_MAX;
  p_arp_hdr->AddrProtocolLen = NET_IPv4_ADDR_SIZE;

  //                                                               --------------- PREPARE ARP OP CODE ----------------
  NET_UTIL_VAL_COPY_SET_NET_16(&p_arp_hdr->OpCode, &op_code);

  //                                                               ------- PREPARE ARP HW/PROTOCOL SENDER ADDRS -------
  Mem_Copy(&p_arp_hdr->AddrHW_Sender[0],
           p_addr_hw_sender,
           NET_IF_HW_ADDR_LEN_MAX);

  Mem_Copy(&p_arp_hdr->AddrProtocolSender[0],
           p_addr_protocol_sender,
           NET_IPv4_ADDR_SIZE);

  //                                                               ------- PREPARE ARP HW/PROTOCOL TARGET ADDRS -------
  if (p_addr_hw_target == DEF_NULL) {                           // If ARP target hw addr NULL for ARP Req, ...
    Mem_Clr(&p_arp_hdr->AddrHW_Target[0],                       // .. clr target hw addr octets.
            NET_IF_HW_ADDR_LEN_MAX);

    DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_TX_BROADCAST);     // ARP Req broadcast to ALL hosts on local net.
  } else {                                                      // Else copy target hw addr for ARP Reply.
    Mem_Copy(&p_arp_hdr->AddrHW_Target[0],
             p_addr_hw_target,
             NET_IF_HW_ADDR_LEN_MAX);
    //                                                             ARP Reply tx'd directly to target host.
  }

  Mem_Copy(&p_arp_hdr->AddrProtocolTarget[0],
           p_addr_protocol_target,
           NET_IPv4_ADDR_SIZE);

  //                                                               ------------ CFG ARP PROTOCOL ADDR PTR -------------
  p_buf_hdr->ARP_AddrProtocolPtr = &p_arp_hdr->AddrProtocolTarget[0];
}

/****************************************************************************************************//**
 *                                           NetARP_TxIxDataGet()
 *
 * @brief    - (1) Solves the starting index of the ARP data from the data buffer beginning.
 *           - (2) Starting index if found by adding up the header sizes of the lower-level
 *                 protocol headers.
 *
 * @param    if_nbr      Network interface number to transmit data.
 *
 * @param    hdr_len     Length of the ARP header.
 *
 * @param    data_len    Length of the ARP payload.
 *
 * @param    p_ix        Pointer to the current protocol index.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetARP_TxIxDataGet(NET_IF_NBR if_nbr,
                               CPU_INT32U data_len,
                               CPU_INT16U *p_ix,
                               RTOS_ERR   *p_err)
{
  NET_MTU mtu;

  mtu = NetIF_MTU_GetProtocol(if_nbr, NET_PROTOCOL_TYPE_ARP, NET_IF_FLAG_NONE);
  if (data_len > mtu) {
    NET_CTR_ERR_INC(Net_ErrCtrs.ARP.TxInvLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);                          // See Note #2.
    goto exit;
  }

  //                                                               Add the lower-level hdr offsets.
  NetIF_TxIxDataGet(if_nbr, data_len, p_ix);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetARP_TxPktFree()
 *
 * @brief    Free network buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *
 * @note     (1) Although ARP Transmit initially requests the network buffer for transmit,
 *               the ARP layer does NOT maintain a reference to the buffer.
 *
 * @note     (2) Also, since the network interface deallocation task frees ALL unreferenced buffers
 *               after successful transmission, the ARP layer must NOT free the transmit buffer.
 *               See also 'net_if.c  NetIF_TxDeallocTaskHandler()  Note #1c'.
 *******************************************************************************************************/
static void NetARP_TxPktFree(NET_BUF *p_buf)
{
  PP_UNUSED_PARAM(p_buf);                                       // Prevent 'variable unused' warning (see Note #1).
}

/****************************************************************************************************//**
 *                                           NetARP_TxPktDiscard()
 *
 * @brief    On any ARP transmit handler error(s), discard packet & buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetARP_TxPktDiscard(NET_BUF *p_buf)
{
  NET_CTR *p_ctr;

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  p_ctr = (NET_CTR *)&Net_ErrCtrs.ARP.TxPktDisCtr;
#else
  p_ctr = DEF_NULL;
#endif
  (void)NetBuf_FreeBuf(p_buf, p_ctr);
}

/****************************************************************************************************//**
 *                                           NetARP_CacheReqTimeout()
 *
 * @brief    Retry ARP Request to resolve an ARP cache in the 'PENDING' state on ARP Request timeout.
 *
 * @param    p_cache_timeout     Pointer to an ARP cache (see Note #1b).
 *
 * @note     (1) This function is a network timer callback function :
 *               - (a) Clear the timer pointer ... :
 *                   - (1) Cleared in NetCache_AddrFree() via NetCache_Remove(); or
 *                   - (2) Reset   by NetTmr_Get().
 *               - (b) but do NOT re-free the timer.
 *******************************************************************************************************/
static void NetARP_CacheReqTimeout(void *p_cache_timeout)
{
  NET_ARP_CACHE      *p_cache;
  NET_CACHE_ADDR_ARP *p_cache_addr_arp;
  CPU_INT32U         timeout_ms;
  CPU_INT08U         th_max;
  RTOS_ERR           local_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_cache = (NET_ARP_CACHE *)p_cache_timeout;
  p_cache_addr_arp = (NET_CACHE_ADDR_ARP *)p_cache->CacheAddrPtr;

  p_cache->TmrPtr = DEF_NULL;

  CORE_ENTER_ATOMIC();
  switch (p_cache->State) {
    case NET_ARP_CACHE_STATE_RENEW:
      th_max = NetARP_ReqMaxAttemptsRenew_nbr;
      timeout_ms = NetARP_ReqTimeoutRenew_ms;
      break;

    case NET_ARP_CACHE_STATE_PEND:
    default:
      th_max = NetARP_ReqMaxAttemptsPend_nbr;
      timeout_ms = NetARP_ReqTimeoutPend_ms;
      break;
  }
  CORE_EXIT_ATOMIC();

  if (p_cache->ReqAttemptsCtr >= th_max) {                     // If nbr attempts >= max, free ARP cache.
                                                               //  Clr but do NOT free tmr.
    NetCache_Remove((NET_CACHE_ADDR *)p_cache_addr_arp, DEF_NO);
    goto exit;
  }

  //                                                               ------------------ RETRY ARP REQ -------------------
  p_cache->TmrPtr = NetTmr_Get(NetARP_CacheReqTimeout,
                               p_cache,
                               timeout_ms,
                               NET_TMR_OPT_NONE,
                               &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // If tmr unavail, free ARP cache.
    NetCache_Remove((NET_CACHE_ADDR *)p_cache_addr_arp, DEF_NO);
    goto exit;
  }

  //                                                               ------------------ RE-TX ARP REQ -------------------
  NetARP_TxReq(p_cache_addr_arp);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetARP_CacheTimeout()
 *
 * @brief    Callback function when cache timed out. Fall in Renew state.
 *
 * @param    p_cache_timeout     Pointer to an ARP cache.
 *
 * @note     (1) RFC #1122, Section 2.3.2.1 states that "an implementation of the Address Resolution
 *               Protocol (ARP) ... MUST provide a mechanism to flush out-of-date cache entries".
 *
 * @note     (2) This function is a network timer callback function :
 *               - (a) Clear the timer pointer ... :
 *                   - (1) Cleared in NetCache_AddrFree() via NetCache_Remove().
 *               - (b) but do NOT re-free the timer.
 *******************************************************************************************************/
static void NetARP_CacheTimeout(void *p_cache_timeout)
{
  NET_ARP_CACHE      *p_cache = DEF_NULL;
  NET_CACHE_ADDR_ARP *p_arp = DEF_NULL;
  CPU_INT32U         timeout_ms = 0u;
  RTOS_ERR           local_err;
  CORE_DECLARE_IRQ_STATE;

  p_cache = (NET_ARP_CACHE *)p_cache_timeout;

  p_cache->TmrPtr = DEF_NULL;

  p_arp = (NET_CACHE_ADDR_ARP *)p_cache->CacheAddrPtr;

  if ((CPU_INT32U)p_arp->AddrProtocolSender[0] == NET_ARP_PROTOCOL_ADDR_NONE) {
    NetCache_Remove((NET_CACHE_ADDR *)p_arp, DEF_NO);
    return;
  }

  CORE_ENTER_ATOMIC();
  timeout_ms = NetARP_ReqTimeoutRenew_ms;
  CORE_EXIT_ATOMIC();

  //                                                               -------------- SET TIMER FOR RETRIES ---------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  p_cache->TmrPtr = NetTmr_Get(NetARP_CacheReqTimeout,
                               p_cache,
                               timeout_ms,
                               NET_TMR_OPT_NONE,
                               &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    //                                                             Clr but do NOT free tmr.
    NetCache_Remove((NET_CACHE_ADDR *)p_arp, DEF_NO);
    return;
  }

  //                                                               ------------- SET CACHE STATE TO RENEW -------------
  p_cache->State = NET_ARP_CACHE_STATE_RENEW;

  //                                                               -------------------- TX ARP REQ --------------------
  NetARP_TxReq(p_arp);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_ARP_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
