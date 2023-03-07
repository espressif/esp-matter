/***************************************************************************//**
 * @file
 * @brief Network Mldp Layer - (Multicast Listener Discovery Protocol)
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

#include  <net/include/net_cfg_net.h>

#ifdef  NET_MLDP_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "net_mldp_priv.h"
#include  "net_ipv6_priv.h"
#include  "net_icmpv6_priv.h"
#include  "net_tmr_priv.h"
#include  "net_priv.h"
#include  "net_if_priv.h"

#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 *                                           MLDP REPORT DEFINES
 *
 * Note(s) : (1) RFC #2710,  Section 4 'Protocol Description' states that :
 *
 *               (a) "When a node starts listening to a multicast address on an interface, it should
 *                       immediately transmit an unsolicited Report for that address on that interface,
 *                       in case it is the first listener on the link. To cover the possibility of the initial
 *                       Report being lost or damaged, it is recommended that it be repeated once or twice
 *                       after short delays."
 *
 *                       The delay between the report transmissions is set to 2 seconds in this implementation.
 *
 *               (b) "When a node receives a Multicast-Address-Specific Query, if it is listening to the
 *                       queried Multicast Address on the interface from which the Query was received, it
 *                       sets a delay timer for that address to a random value selected from the range
 *                       [0, Maximum Response Delay]."
 *
 *           (2) When a transmit error occurs when attempting to transmit an MLDP report, a new timer
 *               is set with a delay of NET_MLDP_HOST_GRP_REPORT_DLY_RETRY_SEC seconds to retransmit
 *               the report.
 *******************************************************************************************************/

#define  NET_MLDP_HOST_GRP_REPORT_DLY_JOIN_SEC             2    // See Note #1a.
                                                                // See Note #1b.
#define  NET_MLDP_HOST_GRP_REPORT_DLY_MIN_SEC              0
#define  NET_MLDP_HOST_GRP_REPORT_DLY_MAX_SEC             10

#define  NET_MLDP_HOST_GRP_REPORT_DLY_RETRY_SEC            2    // See Note #2.

/********************************************************************************************************
 *                                           MLDP FLAG DEFINES
 *******************************************************************************************************/
//                                                                 ------------------ NET MLDP FLAGS ------------------
#define  NET_MLDP_FLAG_NONE                              DEF_BIT_NONE
#define  NET_MLDP_FLAG_USED                              DEF_BIT_00    // MLDP host grp cur used; i.e. NOT in free pool.

/********************************************************************************************************
 *                                   MLDP HOP BY HOP EXT HDR SIZE DEFINES
 *******************************************************************************************************/

#define  NET_MLDP_OPT_HDR_SIZE                             8

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       MLDP MODULE DATA DATA TYPE
 *******************************************************************************************************/

typedef  struct  net_mldp_data {
  MEM_DYN_POOL  HostGrpPool;
  SLIST_MEMBER  *HostGrpListPtr;
#if (NET_STAT_POOL_MLDP_EN == DEF_ENABLED)
  NET_STAT_POOL HostGrpPoolStat;
#endif
  RAND_NBR      RandSeed;
} NET_MLDP_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_MLDP_DATA *NetMLDP_DataPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_MLDP_MSG_TYPE NetMLDP_RxPktValidate(NET_BUF         *p_buf,
                                               NET_BUF_HDR     *p_buf_hdr,
                                               NET_MLDP_V1_HDR *p_mldp_hdr,
                                               RTOS_ERR        *p_err);

static void NetMLDP_RxQuery(NET_IF_NBR          if_nbr,
                            const NET_IPv6_ADDR *p_addr_dest,
                            NET_MLDP_V1_HDR     *p_mldp_hdr,
                            RTOS_ERR            *p_err);

static void NetMLDP_RxReport(NET_IF_NBR          if_nbr,
                             const NET_IPv6_ADDR *p_addr_dest,
                             NET_MLDP_V1_HDR     *p_mldp_hdr,
                             RTOS_ERR            *p_err);

static void NetMLDP_TxAdvertiseMembership(NET_MLDP_HOST_GRP *p_grp,
                                          RTOS_ERR          *p_err);

static void NetMLDP_TxReport(NET_IF_NBR    if_nbr,
                             NET_IPv6_ADDR *p_addr_mcast_dest,
                             RTOS_ERR      *p_err);

static void NetMLDP_TxMsgDone(NET_IF_NBR    if_nbr,
                              NET_IPv6_ADDR *p_addr_mcast_dest,
                              RTOS_ERR      *p_err);

static NET_MLDP_HOST_GRP *NetMLDP_HostGrpSrch(NET_IF_NBR          if_nbr,
                                              const NET_IPv6_ADDR *p_addr);

static NET_MLDP_HOST_GRP *NetMLDP_HostGrpSrchIF(NET_IF_NBR if_nbr);

static NET_MLDP_HOST_GRP *NetMLDP_HostGrpAdd(NET_IF_NBR          if_nbr,
                                             const NET_IPv6_ADDR *p_addr,
                                             RTOS_ERR            *p_err);

static void NetMLDP_HostGrpRemove(NET_MLDP_HOST_GRP *p_host_grp);

static void NetMLDP_HostGrpClr(NET_MLDP_HOST_GRP *p_host_grp);

static void NetMLDP_HostGrpReportDlyTimeout(void *p_host_grp_timeout);

static void NetMLDP_LinkStateNotification(NET_IF_NBR        if_nbr,
                                          NET_IF_LINK_STATE link_state);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetMLDP_HostGrpJoin()
 *
 * @brief     Join a IPv6 MLDP group associated with a multicast address.
 *
 * @param    if_nbr  Interface number associated with the MDLP host group.
 *
 * @param    p_addr  Pointer to IPv6 address of host group to join.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_TYPE
 *                       - RTOS_ERR_NOT_INIT
 *                       - RTOS_ERR_TX
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_RX
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_NOT_SUPPORTED
 *                       - RTOS_ERR_NOT_READY
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_NET_IF_LINK_DOWN
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_NET_NEXT_HOP
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @return   Pointer to MLDP Host Group object added to the MLDP list.
 *           DEF_NULL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetMLDP_HostGrpJoin(NET_IF_NBR    if_nbr,
                                NET_IPv6_ADDR *p_addr,
                                RTOS_ERR      *p_err)
{
  NET_MLDP_HOST_GRP *p_grp = DEF_NULL;
  CPU_BOOLEAN       result = DEF_FAIL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_addr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);

  Net_GlobalLockAcquire((void *)NetMLDP_HostGrpJoin);

  //                                                               Join host grp.
  p_grp = NetMLDP_HostGrpJoinHandler(if_nbr,
                                     p_addr,
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  result = (p_grp != DEF_NULL) ? DEF_OK : DEF_FAIL;

exit_release:
  Net_GlobalLockRelease();

  return (result);
}

/****************************************************************************************************//**
 *                                           NetMLDP_HostGrpLeave()
 *
 * @brief    Leave MDLP group associated with the received IPv6 multicast address.
 *
 * @param    if_nbr  Interface number associated with host group.
 *
 * @param    p_addr  Pointer to IPv6 address of host group to leave.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_TYPE
 *                       - RTOS_ERR_NOT_INIT
 *                       - RTOS_ERR_TX
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_RX
 *                       - RTOS_ERR_NOT_SUPPORTED
 *                       - RTOS_ERR_NOT_READY
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_NET_IF_LINK_DOWN
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_NET_NEXT_HOP
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @return   DEF_OK,   if host group successfully left.
 *           DEF_FAIL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetMLDP_HostGrpLeave(NET_IF_NBR    if_nbr,
                                 NET_IPv6_ADDR *p_addr,
                                 RTOS_ERR      *p_err)
{
  CPU_BOOLEAN host_grp_leave = DEF_FAIL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_addr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);

  //                                                               Acquire net lock (see Note #1b).
  Net_GlobalLockAcquire((void *)NetMLDP_HostGrpLeave);

  //                                                               Leave host grp.
  host_grp_leave = NetMLDP_HostGrpLeaveHandler(if_nbr, p_addr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit_release:
  Net_GlobalLockRelease();                                      // Release net lock.

  return (host_grp_leave);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetMLDP_Init()
 *
 * @brief    (1) Initialize Multicast Listener Discovery Layer :
 *               - (a) Initialize MLDP host group pool
 *               - (b) Initialize MLDP host group table
 *               - (c) Initialize MLDP Host Group List pointer
 *               - (d) Initialize MLDP random seed
 *
 * @param    p_mem_seg   Pointer to memory segment for dynamic allocation.
 *
 *
 * Argument(s) : p_mem_seg   Pointer to memory segment for dynamic allocation.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) MLDP host group pool MUST be initialized PRIOR to initializing the pool with pointers
 *                   to MLDP host group.
 *******************************************************************************************************/
void NetMLDP_Init(MEM_SEG  *p_mem_seg,
                  RTOS_ERR *p_err)
{
  //                                                               ----- INITIALIZE MEMORY SEGMENT FOR MLDP DATA ------
  NetMLDP_DataPtr = (NET_MLDP_DATA *)Mem_SegAlloc("MLDP Data Segment",
                                                  p_mem_seg,
                                                  sizeof(NET_MLDP_DATA),
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ---------- INIT MLDP HOST GRP POOL/STATS -----------
#if (NET_STAT_POOL_MLDP_EN == DEF_ENABLED)
  NetStat_PoolInit(&NetMLDP_DataPtr->HostGrpPoolStat,
                   NET_MCAST_CFG_HOST_GRP_NBR_MAX);
#endif

  //                                                               --------- CREATE POOL FOR MLDP HOST GROUP ----------
  Mem_DynPoolCreate("MLDP Host Group Pool",
                    &NetMLDP_DataPtr->HostGrpPool,
                    p_mem_seg,
                    sizeof(NET_MLDP_HOST_GRP),
                    sizeof(CPU_ALIGN),
                    1,
                    NET_MCAST_CFG_HOST_GRP_NBR_MAX,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ----------- INIT MLDP HOST GRP LIST PTR ------------
  SList_Init(&NetMLDP_DataPtr->HostGrpListPtr);

  //                                                               ------------------ INIT RAND SEED ------------------
  NetMLDP_DataPtr->RandSeed = 1u;                               // See 'lib_math.c  Math_Init()  Note #2'.
}

/****************************************************************************************************//**
 *                                       NetMLDP_HostGrpJoinHandler()
 *
 * @brief    (1) Join a IPv6 MLDP group associated with a multicast address:
 *               - (a) Validate interface number
 *               - (b) Validate multicast group address
 *               - (c) Search MLDP Host Group List for host group with corresponding address
 *                     & interface number
 *               - (d) If host group NOT found, allocate new host group.
 *               - (e) Advertise membership to multicast router(s)
 *
 * @param    if_nbr  Interface number associated with the MDLP host group.
 *
 * @param    p_addr  Pointer to IPv6 address of host group to join.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to MLDP Host Group object added to the MLDP list.
 *           DEF_NULL, otherwise.
 *
 * @note     (2) NetMLDP_HostGrpJoinHandler() is called by network protocol suite function(s) & MUST
 *               be called with the global network lock already acquired.
 *******************************************************************************************************/
NET_MLDP_HOST_GRP *NetMLDP_HostGrpJoinHandler(NET_IF_NBR          if_nbr,
                                              const NET_IPv6_ADDR *p_addr,
                                              RTOS_ERR            *p_err)
{
  NET_MLDP_HOST_GRP *p_grp = DEF_NULL;
  CPU_BOOLEAN       is_mcast = DEF_NO;
  CPU_BOOLEAN       is_valid = DEF_NO;

  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }

  //                                                               -------------- VALIDATE HOST GRP ADDR --------------
  is_mcast = NetIPv6_IsAddrMcast(p_addr);
  RTOS_ASSERT_DBG_ERR_SET((is_mcast == DEF_YES), *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);

  //                                                               ---------------- SRCH HOST GRP LIST ----------------
  p_grp = NetMLDP_HostGrpSrch(if_nbr, p_addr);
  if (p_grp != DEF_NULL) {                                      // If host grp found, ...
    p_grp->RefCtr++;
    goto exit;
  }

  p_grp = NetMLDP_HostGrpAdd(if_nbr, p_addr, p_err);            // Add new host grp into Host Grp List.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (p_grp);
}

/****************************************************************************************************//**
 *                                       NetMLDP_HostGrpLeaveHandler()
 *
 * @brief    (1) Leave MDLP group associated with the received IPv6 multicast address :
 *               - (a) Search MLDP Host Group List for host group with corresponding address
 *                     & interface number
 *               - (b) If host group found, remove host group from MLDP Host Group List.
 *               - (c) Advertise end of Membership if host is last member of the group.
 *
 * @param    if_nbr  Interface number associated with host group.
 *
 * @param    p_addr  Pointer to IPv6 address of host group to leave.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if host group successfully left.
 *           DEF_FAIL, otherwise.
 *
 * @note     (2) NetMLDP_HostGrpLeaveHandler() is called by network protocol suite function(s) & MUST
 *               be called with the global network lock already acquired.
 *******************************************************************************************************/
CPU_BOOLEAN NetMLDP_HostGrpLeaveHandler(NET_IF_NBR          if_nbr,
                                        const NET_IPv6_ADDR *p_addr,
                                        RTOS_ERR            *p_err)
{
  NET_MLDP_HOST_GRP *p_host_grp;
  CPU_BOOLEAN       is_mcast_allnodes;
  CPU_BOOLEAN       is_mcast;
  CPU_BOOLEAN       is_valid;
  CPU_INT08U        scope;
  CPU_BOOLEAN       result = DEF_FAIL;

  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }

  //                                                               -------------- VALIDATE HOST GRP ADDR --------------
  is_mcast = NetIPv6_IsAddrMcast(p_addr);
  RTOS_ASSERT_DBG_ERR_SET((is_mcast == DEF_YES), *p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);

  //                                                               ---------------- SRCH HOST GRP LIST ----------------
  p_host_grp = NetMLDP_HostGrpSrch(if_nbr, p_addr);
  if (p_host_grp == DEF_NULL) {                                 // If host grp NOT found, ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);                       // ... rtn err.
    goto exit;
  }

  p_host_grp->RefCtr--;                                         // Dec ref ctr.

  //                                                               ----------- ADVERTISE END OF MEMBERSHIP ------------
  is_mcast_allnodes = NetIPv6_IsAddrMcastAllNodes(&p_host_grp->AddrGrp);

  scope = NetIPv6_GetAddrScope(&p_host_grp->AddrGrp);

  if ((is_mcast_allnodes == DEF_NO)
      && ((scope != NET_IPv6_ADDR_SCOPE_RESERVED)
          && (scope != NET_IPv6_ADDR_SCOPE_IF_LOCAL))) {
    if (p_host_grp->RefCtr < 1) {
      NetMLDP_TxMsgDone(if_nbr,
                        &p_host_grp->AddrGrp,
                        p_err);
      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_NONE:
        case RTOS_ERR_NET_IF_LINK_DOWN:
          result = DEF_OK;
          break;

        default:
          goto exit;
      }
    }
  }

  //                                                               -------- REMOVE HOST GRP FROM HOST GRP LIST --------
  if (p_host_grp->RefCtr < 1) {
    NetMLDP_HostGrpRemove(p_host_grp);
  }

exit:
  return (result);
}

/****************************************************************************************************//**
 *                                           NetMLDP_IsGrpJoinedOnIF()
 *
 * @brief    Check for joined host group on specified interface.
 *
 * @param    if_nbr      Interface number to search.
 *
 * @param    p_addr_grp  Pointer to IPv6 address of host group.
 *
 * @return   DEF_YES, if host group address joined on interface.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetMLDP_IsGrpJoinedOnIF(NET_IF_NBR          if_nbr,
                                    const NET_IPv6_ADDR *p_addr_grp)
{
  NET_MLDP_HOST_GRP *p_host_grp;
  CPU_BOOLEAN       grp_joined;

  p_host_grp = NetMLDP_HostGrpSrch(if_nbr, p_addr_grp);
  grp_joined = (p_host_grp != DEF_NULL) ? DEF_YES : DEF_NO;

  return (grp_joined);
}

/****************************************************************************************************//**
 *                                           NetMLDP_HopByHopHdr()
 *
 * @brief    Callback function called by IP layer when Extension headers are added to Tx buffer.
 *
 * @param    p_ext_hdr_arg   Pointer to list of arguments.
 *******************************************************************************************************/
void NetMLDP_PrepareHopByHopHdr(void *p_ext_hdr_arg)
{
  NET_IPv6_EXT_HDR_ARG_GENERIC *p_hop_hdr_arg;
  NET_IPv6_OPT_HDR             *p_hop_hdr;
  NET_IPv6_EXT_HDR_TLV         *p_tlv;
  NET_BUF                      *p_buf;
  CPU_INT16U                   hop_hdr_ix;

  p_hop_hdr_arg = (NET_IPv6_EXT_HDR_ARG_GENERIC *)p_ext_hdr_arg;

  p_buf = p_hop_hdr_arg->BufPtr;
  hop_hdr_ix = p_hop_hdr_arg->BufIx;

  p_hop_hdr = (NET_IPv6_OPT_HDR *)&p_buf->DataPtr[hop_hdr_ix];
  p_hop_hdr->NextHdr = p_hop_hdr_arg->NextHdr;
  p_hop_hdr->HdrLen = 0;
  p_tlv = (NET_IPv6_EXT_HDR_TLV *)&p_hop_hdr->Opt[0];
  p_tlv->Type = NET_IPv6_EH_TYPE_ROUTER_ALERT;
  p_tlv->Len = 2;
  p_tlv->Val[0] = 0;
  p_tlv->Val[1] = 0;
  p_tlv = (NET_IPv6_EXT_HDR_TLV *)(&p_hop_hdr->Opt[0] + 4);
  p_tlv->Type = NET_IPv6_EH_TYPE_PADN;
  p_tlv->Len = 0;
}

/****************************************************************************************************//**
 *                                               NetMLDP_Rx()
 *
 * @brief    (1) Process received MLDP packets & update host group status :
 *               - (a) Validate MLDP packet
 *               - (b) Update   MLDP host group status
 *               - (c) Free     MLDP packet
 *               - (d) Update receive statistics
 *
 * @param    p_buf       Pointer to network buffer that received ICMP packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_mldp_hdr  Pointer to received packet's MLDP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetMLDP_Rx(NET_BUF         *p_buf,
                NET_BUF_HDR     *p_buf_hdr,
                NET_MLDP_V1_HDR *p_mldp_hdr,
                RTOS_ERR        *p_err)
{
  NET_MLDP_MSG_TYPE msg_type;
  NET_IF_NBR        if_nbr;
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  NET_CTR *p_ctr;
#endif

  if_nbr = p_buf_hdr->IF_Nbr;

  //                                                               -------------- VALIDATE RX'D MLDP PKT --------------
  msg_type = NetMLDP_RxPktValidate(p_buf,
                                   p_buf_hdr,
                                   p_mldp_hdr,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------------ DEMUX MLDP MSG ------------------
  switch (msg_type) {
    case NET_MLDP_MSG_TYPE_QUERY:
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
      p_ctr = (NET_CTR *)&Net_StatCtrs.MLDP.RxMsgQueryCtr;
#endif
      NetMLDP_RxQuery(if_nbr,
                      &p_buf_hdr->IPv6_AddrDest,
                      p_mldp_hdr,
                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

    case NET_MLDP_MSG_TYPE_REPORT:
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
      p_ctr = (NET_CTR *)&Net_StatCtrs.MLDP.RxMsgReportCtr;
#endif
      NetMLDP_RxReport(if_nbr,
                       &p_buf_hdr->IPv6_AddrDest,
                       p_mldp_hdr,
                       p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      goto exit;
  }

  //                                                               ------------------ UPDATE RX STATS -----------------
  NET_CTR_STAT_INC(Net_StatCtrs.MLDP.RxMsgCompCtr);
  NET_CTR_STAT_INC(*p_ctr);

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetMLDP_RxPktValidate()
 *
 * @brief    (1) Validate received MLDP packet :
 *               - (a) Validate Hop Limit of packet.
 *               - (b) Validate Router Alert Option in Hop-By-Hop IPv6 Extension Header.
 *               - (c) Validate Rx Source address as a link-local address
 *               - (d) Validate Rx Destination address as multicast
 *               - (e) Validate Rx Source address is not configured on host.
 *               - (f) Validate MDLP message length.
 *               - (g) Validate MDLP message type.
 *
 * @param    p_buf       Pointer to network buffer that received ICMP packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_mldp_hdr  Pointer to received packet's NDP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static NET_MLDP_MSG_TYPE NetMLDP_RxPktValidate(NET_BUF         *p_buf,
                                               NET_BUF_HDR     *p_buf_hdr,
                                               NET_MLDP_V1_HDR *p_mldp_hdr,
                                               RTOS_ERR        *p_err)
{
  NET_IPv6_HDR         *p_ip_hdr;
  NET_IPv6_OPT_HDR     *p_opt_hdr;
  CPU_INT08U           *p_data;
  NET_IPv6_EXT_HDR_TLV *p_tlv;
  NET_MLDP_MSG_TYPE    msg_type = NET_MLDP_MSG_TYPE_NONE;
  NET_IPv6_OPT_TYPE    opt_type;
  NET_IF_NBR           if_nbr;
  NET_IF_NBR           if_nbr_found;
  CPU_INT16U           mldp_msg_len;
  CPU_INT16U           ext_hdr_ix;
  CPU_INT16U           eh_len;
  CPU_INT16U           next_tlv_offset;
  CPU_INT08U           mldp_type;
  CPU_INT08U           hop_limit_ip;
  CPU_BOOLEAN          is_addr_mcast;
  CPU_BOOLEAN          is_addr_linklocal;
  CPU_BOOLEAN          rtr_alert;

  PP_UNUSED_PARAM(p_buf);                                       // Prevent 'variable unused' warning (see Note #3b).

  if_nbr = p_buf_hdr->IF_Nbr;

  //                                                               -------------- VALIDATE HOP LIMIT VALUE ------------
  p_ip_hdr = (NET_IPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];
  hop_limit_ip = p_ip_hdr->HopLim;
  if (hop_limit_ip != NET_IPv6_HDR_HOP_LIM_MIN) {
    NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.RxInvalidHopLimit);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
  //                                                               ---------- VALIDATE HOP BY HOP IPv6 HEADER ---------
  ext_hdr_ix = p_buf_hdr->IPv6_HopByHopHdrIx;
  p_data = p_buf->DataPtr;
  //                                                               Get opt hdr from ext hdr data space.
  p_opt_hdr = (NET_IPv6_OPT_HDR *)&p_data[ext_hdr_ix];

  eh_len = (p_opt_hdr->HdrLen + 1) * NET_IPv6_EH_ALIGN_SIZE;
  next_tlv_offset = 0u;
  rtr_alert = DEF_NO;

  eh_len -= 2u;
  while (next_tlv_offset < eh_len) {
    p_tlv = (NET_IPv6_EXT_HDR_TLV *)&p_opt_hdr->Opt[next_tlv_offset];
    opt_type = p_tlv->Type & NET_IPv6_EH_TLV_TYPE_OPT_MASK;

    switch (opt_type) {
      case NET_IPv6_EH_TYPE_ROUTER_ALERT:
        rtr_alert = DEF_YES;
        break;

      case NET_IPv6_EH_TYPE_PAD1:
      case NET_IPv6_EH_TYPE_PADN:
      default:
        break;
    }

    if (opt_type == NET_IPv6_EH_TYPE_PAD1) {                    // The format of the Pad1 opt is a special case, it ...
      next_tlv_offset++;                                        // ... doesn't have len and value fields.
    } else {
      next_tlv_offset += p_tlv->Len + 2u;
    }
  }

  if (rtr_alert == DEF_NO) {
    NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.RxInvalidHdr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------------- VALIDATE MLDP RX SRC ADDR ------------
  is_addr_linklocal = NetIPv6_IsAddrLinkLocal(&p_buf_hdr->IPv6_AddrSrc);
  if (is_addr_linklocal == DEF_NO) {
    NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.RxInvalidAddrSrc);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------------- VALIDATE MLDP RX DEST ADDR ------------
  is_addr_mcast = NetIPv6_IsAddrMcast(&p_buf_hdr->IPv6_AddrDest);
  if (is_addr_mcast == DEF_NO) {
    NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.RxInvalidAddrDest);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------- VALIDATE SRC ADDR IS NOT LOCAL ADDR --------
  if_nbr_found = NetIPv6_GetAddrHostIF_Nbr(&p_buf_hdr->IPv6_AddrSrc);
  if (if_nbr == if_nbr_found) {
    NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.RxInvalidAddrSrc);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------------- VALIDATE MLDP RX MSG LEN -------------
  mldp_msg_len = p_buf_hdr->IP_DatagramLen;
  p_buf_hdr->MLDP_MsgLen = mldp_msg_len;
  if (mldp_msg_len < NET_MLDP_MSG_SIZE_MIN) {                   // If msg len < min msg len, rtn err (see Note #4a).
    NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.RxInvalidLen);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------------ VALIDATE MLDP MESSAGE TYPE -----------
  mldp_type = p_mldp_hdr->Type;
  switch (mldp_type) {
    case NET_ICMPv6_MSG_TYPE_MLDP_QUERY:
      msg_type = NET_MLDP_MSG_TYPE_QUERY;
      break;

    case NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1:
      msg_type = NET_MLDP_MSG_TYPE_REPORT;
      break;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.RxInvalidType);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

exit:
  return (msg_type);
}

/****************************************************************************************************//**
 *                                               NetMLDP_RxQuery()
 *
 * @brief    (1) Receive Multicast Listener Query message :
 *               - (a) Find if the query is a general query or a specific query.
 *               - (b) Send Report Message if case apply.
 *
 * @param    if_nbr          Interface number on which message was received.
 *
 * @param    p_addr_dest     Pointer to Destination address of Rx buffer.
 *
 * @param    p_mldp_hdr      Pointer to received packet's MLDP header.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) RFC #2710 Section 5 page 9 :
 *               - (a) The link-scope all-nodes address (FF02::1) is handled as a special
 *                     case. The node starts in Idle Listener state for that address on
 *                     every interface, never transitions to another state, and never sends
 *                     a Report or Done for that address
 *
 *               - (b) MLD messages are never sent for multicast addresses whose scope is 0
 *                     (reserved) or 1 (node-local).
 *******************************************************************************************************/
static void NetMLDP_RxQuery(NET_IF_NBR          if_nbr,
                            const NET_IPv6_ADDR *p_addr_dest,
                            NET_MLDP_V1_HDR     *p_mldp_hdr,
                            RTOS_ERR            *p_err)
{
  NET_MLDP_HOST_GRP       *p_mldp_grp;
  NET_IPv6_ADDR           *p_mldp_grp_addr;
  NET_MLDP_HOST_GRP_STATE mldp_grp_state;
  CPU_INT32U              mldp_grp_delay;
  CPU_INT32U              timeout_ms;
  CPU_INT16U              timeout_sec;
  CPU_INT32U              resp_delay_ms;
  CPU_INT08U              scope;
  CPU_BOOLEAN             is_addr_unspecified;
  CPU_BOOLEAN             is_addr_mcast;
  CPU_BOOLEAN             is_mcast_allnodes;
  CPU_BOOLEAN             if_grp_list;

  p_mldp_grp_addr = &p_mldp_hdr->McastAddr;

  is_addr_unspecified = NetIPv6_IsAddrUnspecified(p_mldp_grp_addr);
  is_addr_mcast = NetIPv6_IsAddrMcast(p_mldp_grp_addr);

  //                                                               -------------- VALIDATE ADDRESS GROUP --------------
  if ((is_addr_unspecified == DEF_NO)
      && (is_addr_mcast == DEF_NO)) {
    NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.RxInvalidAddrGrp);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ----------------- SET DELAY VALUE ------------------
  resp_delay_ms = p_mldp_hdr->MaxResponseDly;
  if (resp_delay_ms != 0) {
    timeout_ms = resp_delay_ms;
  } else {
    timeout_ms = 0;
  }

  //                                                               ---------------- GET TYPE OF QUERY -----------------
  if (is_addr_unspecified == DEF_YES) {                         // Received a General Query ...
    if_grp_list = DEF_YES;

    p_mldp_grp = NetMLDP_HostGrpSrchIF(if_nbr);                 // ... Find all MLDP groups related to IF.
  } else {                                                      // Received a Mulitcast-Address-Specific Query ...
    if_grp_list = DEF_NO;
    //                                                             ... search MLD grp List for multicast addr.
    p_mldp_grp = NetMLDP_HostGrpSrch(if_nbr, p_addr_dest);
    if (p_mldp_grp == DEF_NULL) {
      NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.RxGrpNoFound);
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
      goto exit;
    }
  }

  //                                                               ------------- SEND MLDP REPORT MESSAGE -------------
  while (p_mldp_grp != DEF_NULL) {
    mldp_grp_state = p_mldp_grp->State;
    mldp_grp_delay = p_mldp_grp->Delay_ms;

    is_mcast_allnodes = NetIPv6_IsAddrMcastAllNodes(&p_mldp_grp->AddrGrp);
    scope = NetIPv6_GetAddrScope(&p_mldp_grp->AddrGrp);

    if ((is_mcast_allnodes == DEF_NO)                           // See Note #2.
        && ((scope != NET_IPv6_ADDR_SCOPE_RESERVED)
            && (scope != NET_IPv6_ADDR_SCOPE_IF_LOCAL))) {
      if (timeout_ms == 0) {                                    // If delay received is null, ...
        NetMLDP_TxReport(if_nbr,                                // ... send report right away.
                         &p_mldp_grp->AddrGrp,
                         p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.TxPktDisCtr);
        }
      } else if ((mldp_grp_state == NET_MLDP_HOST_GRP_STATE_IDLE)
                 || (mldp_grp_delay > resp_delay_ms)) {
        //                                                         Set Delay timer.
        NetMLDP_DataPtr->RandSeed = Math_RandSeed(NetMLDP_DataPtr->RandSeed);
        timeout_sec = NetMLDP_DataPtr->RandSeed % (CPU_INT16U)(NET_MLDP_HOST_GRP_REPORT_DLY_MAX_SEC + 1);
        timeout_ms = (timeout_sec * 1000);

        p_mldp_grp->TmrPtr = NetTmr_Get((CPU_FNCT_PTR)NetMLDP_HostGrpReportDlyTimeout,
                                        p_mldp_grp,
                                        timeout_ms,
                                        NET_TMR_OPT_NONE,
                                        p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {         // If err setting tmr,     ...
          NetMLDP_TxReport(if_nbr,
                           &p_mldp_grp->AddrGrp,
                           p_err);                              // ... tx report immediately; ...
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.TxPktDisCtr);
          }
        } else {                                                // ... else set host grp state to DELAYING.
          p_mldp_grp->State = NET_MLDP_HOST_GRP_STATE_DELAYING;
        }
      }
    } else {
      p_mldp_grp->TmrPtr = DEF_NULL;
      p_mldp_grp->State = NET_MLDP_HOST_GRP_STATE_IDLE;
      p_mldp_grp->Delay_ms = 0;
    }

    if (if_grp_list == DEF_NO) {                                // For a specific query, send only the report for ...
      break;                                                    // ... the specific grp.
    }

    p_mldp_grp = p_mldp_grp->NextIF_ListPtr;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetMLDP_RxReport()
 *
 * @brief    (1) Receive Multicast Listener Report message :
 *
 *           - (a) Find if IF is listening to the multicast address.
 *           - (b) If the group address is one the IF listen to :
 *                       i)   remove delay timer.
 *                       ii)  Increase the listener count.
 *                       iii) Change the group state to IDLE.
 *
 * @param    if_nbr          Interface number on which message was received.
 *
 * @param    p_addr_dest     Pointer to Destination address of Rx buf.
 *
 * @param    p_mldp_hdr      Pointer to received packet's MLDP header.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetMLDP_RxReport(NET_IF_NBR          if_nbr,
                             const NET_IPv6_ADDR *p_addr_dest,
                             NET_MLDP_V1_HDR     *p_mldp_hdr,
                             RTOS_ERR            *p_err)
{
  NET_MLDP_HOST_GRP *p_mldp_grp;
  NET_IPv6_ADDR     *p_mldp_grp_addr;
  CPU_BOOLEAN       is_addr_mcast;

  p_mldp_grp_addr = &p_mldp_hdr->McastAddr;

  is_addr_mcast = NetIPv6_IsAddrMcast(p_mldp_grp_addr);

  //                                                               ------- VALIDATE RX MULTICAST GROUP ADDRESS --------
  if (is_addr_mcast == DEF_NO) {
    NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.RxInvalidAddrGrp);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------- SEARCH FOR GRP ADDR IN MLDP GRP LIST -------
  p_mldp_grp = NetMLDP_HostGrpSrch(if_nbr, p_addr_dest);
  if (p_mldp_grp == DEF_NULL) {
    NET_CTR_ERR_INC(Net_ErrCtrs.MLDP.RxGrpNoFound);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  p_mldp_grp->RefCtr++;                                         // Increase Listener counter.

  p_mldp_grp->State = NET_MLDP_HOST_GRP_STATE_IDLE;             // Set state to IDLE.

  //                                                               Free timer.
  if (p_mldp_grp->TmrPtr != DEF_NULL) {
    NetTmr_Free(p_mldp_grp->TmrPtr);
    p_mldp_grp->TmrPtr = DEF_NULL;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetMLDP_TxAdvertiseMembership()
 *
 * @brief    Transmit a MLDP multicast membership advertisement.
 *
 * @param    p_grp   Pointer to the MLDP host group.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetMLDP_TxAdvertiseMembership(NET_MLDP_HOST_GRP *p_grp,
                                          RTOS_ERR          *p_err)
{
  CPU_INT32U  timeout_ms;
  CPU_BOOLEAN is_mcast_allnodes;
  CPU_INT08U  scope;

  is_mcast_allnodes = NetIPv6_IsAddrMcastAllNodes(&p_grp->AddrGrp);
  scope = NetIPv6_GetAddrScope(&p_grp->AddrGrp);

  if ((is_mcast_allnodes == DEF_NO)
      && ((scope != NET_IPv6_ADDR_SCOPE_RESERVED)
          && (scope != NET_IPv6_ADDR_SCOPE_IF_LOCAL))) {
    NET_IF_LINK_STATE link_state;

    link_state = NetIF_LinkStateGetHandler(p_grp->IF_Nbr, p_err);
    if (link_state != NET_IF_LINK_UP) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_IF_LINK_DOWN);
      goto exit;
    }

    timeout_ms = (NET_MLDP_HOST_GRP_REPORT_DLY_JOIN_SEC * 1000);
    p_grp->TmrPtr = NetTmr_Get(NetMLDP_HostGrpReportDlyTimeout,
                               p_grp,
                               timeout_ms,
                               NET_TMR_OPT_NONE,
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      p_grp->State = NET_MLDP_HOST_GRP_STATE_DELAYING;
      p_grp->Delay_ms = NET_MLDP_HOST_GRP_REPORT_DLY_RETRY_SEC * 1000;
    } else {                                                    // If no timer available tx once.
      p_grp->State = NET_MLDP_HOST_GRP_STATE_IDLE;
      p_grp->Delay_ms = 0u;
    }

    NetMLDP_TxReport(p_grp->IF_Nbr,
                     &p_grp->AddrGrp,
                     p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
      case RTOS_ERR_POOL_EMPTY:
        break;

      case RTOS_ERR_NET_IF_LINK_DOWN:
        NetTmr_Free(p_grp->TmrPtr);
        p_grp->TmrPtr = DEF_NULL;
        break;

      default:
        goto exit;
    }
  } else {
    p_grp->TmrPtr = DEF_NULL;
    p_grp->State = NET_MLDP_HOST_GRP_STATE_IDLE;
    p_grp->Delay_ms = 0;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetMLDP_TxReport()
 *
 * @brief    Transmit Multicast Listener Report message.
 *
 * @param    if_nbr              Network interface number to transmit Multicast Listener Report message.
 *
 * @param    p_addr_mcast_dest   Pointer to IPv6 multicast group address.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetMLDP_TxReport(NET_IF_NBR    if_nbr,
                             NET_IPv6_ADDR *p_addr_mcast_dest,
                             RTOS_ERR      *p_err)
{
  NET_IPv6_ADDR    addr_unspecified;
  NET_IPv6_ADDR    *p_addr_dest;
  NET_IPv6_ADDR    *p_addr_src;
  NET_IPv6_EXT_HDR hop_hdr;
  NET_IPv6_EXT_HDR *p_ext_hdr_head;

  //                                                               Take the multicast address being reported as dest.
  p_addr_dest = p_addr_mcast_dest;

  //                                                               Take a Link-Local address cfgd on IF as src addr.
  p_addr_src = NetIPv6_GetAddrLinkLocalCfgd(if_nbr);
  if (p_addr_src == DEF_NULL) {
    NetIPv6_AddrUnspecifiedSet(&addr_unspecified);
    p_addr_src = &addr_unspecified;
  }

  //                                                               ---------------- ADD HOP-HOP HEADER ----------------
  p_ext_hdr_head = DEF_NULL;

  p_ext_hdr_head = NetIPv6_ExtHdrAddToList(p_ext_hdr_head,
                                           &hop_hdr,
                                           NET_IP_HDR_PROTOCOL_EXT_HOP_BY_HOP,
                                           NET_MLDP_OPT_HDR_SIZE,
                                           NetMLDP_PrepareHopByHopHdr,
                                           NET_IPv6_EXT_HDR_KEY_HOP_BY_HOP,
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ---------------- TX MLDP REPORT MSG ----------------
  (void)NetICMPv6_TxMsgReqHandler(if_nbr,
                                  NET_ICMPv6_MSG_TYPE_MLDP_REPORT_V1,
                                  NET_ICMPv6_MSG_CODE_MLDP_REPORT,
                                  0u,
                                  p_addr_src,
                                  p_addr_dest,
                                  NET_IPv6_HDR_HOP_LIM_MIN,
                                  DEF_NO,
                                  p_ext_hdr_head,
                                  p_addr_mcast_dest,
                                  NET_IPv6_ADDR_SIZE,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetMLDP_TxDone()
 *
 * @brief    (1) Transmit Multicast Listener Done message.
 *
 * @param    if_nbr              Network interface number to transmit Multicast Listener Done message.
 *
 * @param    p_addr_mcast_dest   Pointer to multicast IPv6 address.
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
static void NetMLDP_TxMsgDone(NET_IF_NBR    if_nbr,
                              NET_IPv6_ADDR *p_addr_mcast_dest,
                              RTOS_ERR      *p_err)
{
  NET_IPv6_ADDR    addr_unspecified;
  NET_IPv6_ADDR    addr_mcast_all_routers;
  NET_IPv6_ADDR    *p_addr_src;
  NET_IPv6_EXT_HDR hop_hdr;
  NET_IPv6_EXT_HDR *p_ext_hdr_head;

  //                                                               Take the multicast all routers addr as dest addr.
  NetIPv6_AddrMcastAllRoutersSet(&addr_mcast_all_routers, DEF_NO);

  //                                                               Take a Link-Local address cfgd on IF as src addr.
  p_addr_src = NetIPv6_GetAddrLinkLocalCfgd(if_nbr);
  if (p_addr_src == DEF_NULL) {
    NetIPv6_AddrUnspecifiedSet(&addr_unspecified);
    p_addr_src = &addr_unspecified;
  }

  //                                                               ---------------- ADD HOP-HOP HEADER ----------------
  p_ext_hdr_head = DEF_NULL;

  p_ext_hdr_head = NetIPv6_ExtHdrAddToList(p_ext_hdr_head,
                                           &hop_hdr,
                                           NET_IP_HDR_PROTOCOL_EXT_HOP_BY_HOP,
                                           NET_MLDP_OPT_HDR_SIZE,
                                           NetMLDP_PrepareHopByHopHdr,
                                           NET_IPv6_EXT_HDR_KEY_HOP_BY_HOP,
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ----------------- TX MLDP DONE MSG -----------------
  (void) NetICMPv6_TxMsgReqHandler(if_nbr,
                                   NET_ICMPv6_MSG_TYPE_MLDP_DONE,
                                   NET_ICMPv6_MSG_CODE_MLDP_DONE,
                                   0u,
                                   p_addr_src,
                                   &addr_mcast_all_routers,
                                   NET_IPv6_HDR_HOP_LIM_MIN,
                                   DEF_NO,
                                   p_ext_hdr_head,
                                   p_addr_mcast_dest,
                                   NET_IPv6_ADDR_SIZE,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetMLDP_HostGrpSrch()
 *
 * @brief    Search MLDP Host Group List for host group with specific address & interface number.
 *
 *       - (1) MLDP host groups are linked to form an MLDP Host Group List.
 *
 *           - (a) In the diagram below, ... :
 *
 *               - (1) The horizontal row represents the list of MLDP host groups.
 *
 *               - (2) 'NetMLDP_HostGrpListHead' points to the head of the MLDP Host Group List.
 *
 *               - (3) MLDP host groups' 'PrevListPtr' & 'NextListPtr' doubly-link each host group to
 *                       form the MLDP Host Group List.
 *
 *           - (b) (1) For any MLDP Host Group List lookup, all MLDP host groups are searched in order
 *                       to find the host group with the appropriate host group address on the specified
 *                       interface.
 *
 *               - (2) To expedite faster MLDP Host Group List lookup :
 *
 *                   - (A) (1) (a) MLDP host groups are added at;            ...
 *                           - (b) MLDP host groups are searched starting at ...
 *                       - (2) ... the head of the MLDP Host Group List.
 *
 *                   - (B) As MLDP host groups are added into the list, older MLDP host groups migrate
 *                           to the tail of the MLDP Host Group List.   Once an MLDP host group is left,
 *                           it is removed from the MLDP Host Group List.
 *
 *                                   |                                               |
 *                                   |<---------- List of MLDP Host Groups --------->|
 *                                   |                (see Note #1a1)                |
 *
 *                           New MLDP host groups                      Oldest MLDP host group
 *                               inserted at head                        in MLDP Host Group List
 *                               (see Note #1b2A2)                           (see Note #1b2B)
 *
 *                                       |                 NextPtr                 |
 *                                       |             (see Note #1a3)             |
 *                                       v                    |                    v
 *                                                            |
 *               Head of MLDP         -------       -------   v   -------       -------
 *              Host Group List  ---->|     |------>|     |------>|     |------>|     |
 *                                    |     |       |     |       |     |       |     |        Tail of MLDP
 *              (see Note #1a2)       |     |<------|     |<------|     |<------|     |<----  Host Group List
 *                                    -------       -------   ^   -------       -------
 *                                                            |
 *                                                            |
 *                                                        PrevPtr
 *                                                    (see Note #1a3)
 *
 * @param    if_nbr      Interface number to search for host group.
 *
 * @param    addr_grp    IP address of host group to search (see Note #2).
 *
 * @return   Pointer to MLDP host group with specific IP group address & interface, if found.
 *           Pointer to NULL, otherwise.
 *******************************************************************************************************/
static NET_MLDP_HOST_GRP *NetMLDP_HostGrpSrch(NET_IF_NBR          if_nbr,
                                              const NET_IPv6_ADDR *p_addr)
{
  NET_MLDP_HOST_GRP *p_host_grp = DEF_NULL;
  CPU_INT08U        match_bit;
  CPU_BOOLEAN       found = DEF_NO;
  RTOS_ERR          local_err;

  SLIST_FOR_EACH_ENTRY(NetMLDP_DataPtr->HostGrpListPtr, p_host_grp, NET_MLDP_HOST_GRP, ListNode) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    match_bit = NetIPv6_GetAddrMatchingLen(&p_host_grp->AddrGrp, p_addr, &local_err);

    found = ((p_host_grp->IF_Nbr == if_nbr)                     // Cmp IF nbr & grp addr.
             && (match_bit == 128))  ? DEF_YES : DEF_NO;

    if (found == DEF_YES) {                                     // If NOT found, adv to next MLDP host grp.
      break;
    }
  }

  if (found != DEF_YES) {
    p_host_grp = DEF_NULL;
  }

  return (p_host_grp);
}

/****************************************************************************************************//**
 *                                           NetMLDP_HostGrpSrchIF()
 *
 * @brief    Search MLDP Host Group List for all host groups attached to the same interface number.
 *
 * @param    if_nbr  Interface number to search for host group.
 *
 * @return   Pointer to the MDLP host group at the head of the list containing all the host groups
 *               for the given Interface number.
 *******************************************************************************************************/
static NET_MLDP_HOST_GRP *NetMLDP_HostGrpSrchIF(NET_IF_NBR if_nbr)
{
  NET_MLDP_HOST_GRP *p_host_grp = DEF_NULL;
  NET_MLDP_HOST_GRP *p_if_grp_head = DEF_NULL;

  SLIST_FOR_EACH_ENTRY(NetMLDP_DataPtr->HostGrpListPtr, p_host_grp, NET_MLDP_HOST_GRP, ListNode) {
    //                                                             Srch MLDP Host Grp List ...
    //                                                             ... for all grp attached to IF.
    if (p_host_grp->IF_Nbr == if_nbr) {
      if (p_if_grp_head == DEF_NULL) {
        p_host_grp->PrevIF_ListPtr = DEF_NULL;
        p_host_grp->NextIF_ListPtr = DEF_NULL;
      } else {
        p_if_grp_head->PrevIF_ListPtr = p_host_grp;
        p_host_grp->NextIF_ListPtr = p_if_grp_head;
      }
      p_if_grp_head = p_host_grp;
    }
  }

  return (p_if_grp_head);
}

/****************************************************************************************************//**
 *                                           NetMLDP_HostGrpAdd()
 *
 * @brief    (1) Add a host group to the MLDP Host Group List :
 *
 *           - (a) Get a     host group from      host group pool
 *           - (b) Configure host group
 *           - (c) Insert    host group into MLDP Host Group List
 *           - (d) Configure interface for multicast address
 *
 * @param    if_nbr  Interface number to add host group.
 *
 * @param    p_addr  Pointer to IPv6 address of host group to add.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to host group, if NO error(s).
 *           Pointer to NULL, otherwise.
 *******************************************************************************************************/
static NET_MLDP_HOST_GRP *NetMLDP_HostGrpAdd(NET_IF_NBR          if_nbr,
                                             const NET_IPv6_ADDR *p_addr,
                                             RTOS_ERR            *p_err)
{
  NET_MLDP_HOST_GRP *p_host_grp = DEF_NULL;
  NET_PROTOCOL_TYPE addr_protocol_type;
  CPU_INT08U        *p_addr_protocol;
  CPU_INT08U        addr_protocol_len;
  RTOS_ERR          local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               ------------------- GET HOST GRP -------------------
  p_host_grp = (NET_MLDP_HOST_GRP *)Mem_DynPoolBlkGet(&NetMLDP_DataPtr->HostGrpPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------------ INIT HOST GRP -------------------
  NetMLDP_HostGrpClr(p_host_grp);
  DEF_BIT_SET(p_host_grp->Flags, NET_MLDP_FLAG_USED);           // Set host grp as used.

#if (NET_STAT_POOL_MLDP_EN == DEF_ENABLED)
  //                                                               ------------ UPDATE HOST GRP POOL STATS ------------
  NetStat_PoolEntryUsedInc(&NetMLDP_DataPtr->HostGrpPoolStat, &local_err);
#endif

  //                                                               ------------------- CFG HOST GRP -------------------
  Mem_Copy(&p_host_grp->AddrGrp, p_addr, sizeof(p_host_grp->AddrGrp));
  p_host_grp->IF_Nbr = if_nbr;
  p_host_grp->RefCtr = 1u;

  //                                                               Set host grp state.
  p_host_grp->State = NET_MLDP_HOST_GRP_STATE_IDLE;

  //                                                               -------- INSERT HOST GRP INTO HOST GRP LIST --------
  SList_Push(&NetMLDP_DataPtr->HostGrpListPtr, &p_host_grp->ListNode);

  //                                                               ------------ CFG IF FOR MULTICAST ADDR -------------
  addr_protocol_type = NET_PROTOCOL_TYPE_IP_V6;
  p_addr_protocol = (CPU_INT08U *)p_addr;
  addr_protocol_len = NET_IPv6_ADDR_SIZE;

  NetIF_AddrMulticastAdd(if_nbr,
                         p_addr_protocol,
                         addr_protocol_len,
                         addr_protocol_type,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_remove;
  }

  //                                                               ------------ ADVERTISE MCAST MEMBERSHIP ------------
  NetMLDP_TxAdvertiseMembership(p_host_grp, p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
    case RTOS_ERR_POOL_EMPTY:
    case RTOS_ERR_NET_IF_LINK_DOWN:
      break;

    default:
      goto exit;
  }

  NetIF_LinkStateSubscribeHandler(if_nbr, &NetMLDP_LinkStateNotification, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_remove;
  }

  PP_UNUSED_PARAM(local_err);

  goto exit;

exit_remove:
  NetMLDP_HostGrpRemove(p_host_grp);
  p_host_grp = DEF_NULL;

exit:
  return (p_host_grp);
}

/****************************************************************************************************//**
 *                                           NetMLDP_HostGrpRemove()
 *
 * @brief    Remove a host group from the MLDP Host Group List :
 *
 *           - (a) Remove host group from MLDP Host Group List
 *           - (b) Free   host group back to   host group pool
 *           - (c) Remove multicast address from interface
 *
 * @param    p_host_grp  Pointer to a host group.
 *******************************************************************************************************/
static void NetMLDP_HostGrpRemove(NET_MLDP_HOST_GRP *p_host_grp)
{
  NET_IF_NBR        if_nbr;
  NET_PROTOCOL_TYPE addr_protocol_type;
  NET_IPv6_ADDR     *p_addr_grp;
  CPU_INT08U        *p_addr_protocol;
  CPU_INT08U        addr_protocol_len;
  RTOS_ERR          local_err;

  if_nbr = p_host_grp->IF_Nbr;
  p_addr_grp = &p_host_grp->AddrGrp;

  //                                                               -------- REMOVE HOST GRP FROM HOST GRP LIST --------
  SList_Rem(&NetMLDP_DataPtr->HostGrpListPtr, &p_host_grp->ListNode);

  //                                                               ---------------- FREE HOST GRP TMR -----------------
  if (p_host_grp->TmrPtr != DEF_NULL) {
    NetTmr_Free(p_host_grp->TmrPtr);
    p_host_grp->TmrPtr = DEF_NULL;
  }
  //                                                               ------------------- CLR HOST GRP -------------------
  p_host_grp->State = NET_MLDP_HOST_GRP_STATE_FREE;             // Set host grp as freed/NOT used.
  DEF_BIT_CLR(p_host_grp->Flags, (CPU_INT16U)NET_MLDP_FLAG_USED);
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
  NetMLDP_HostGrpClr(p_host_grp);
#endif

#if (NET_STAT_POOL_MLDP_EN == DEF_ENABLED)
  //                                                               ------------ UPDATE HOST GRP POOL STATS ------------
  NetStat_PoolEntryUsedDec(&NetMLDP_DataPtr->HostGrpPoolStat, &local_err);
#endif

  //                                                               ------------------ FREE HOST GRP -------------------
  Mem_DynPoolBlkFree(&NetMLDP_DataPtr->HostGrpPool, p_host_grp, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  //                                                               ---------- REMOVE MULTICAST ADDR FROM IF -----------
  addr_protocol_type = NET_PROTOCOL_TYPE_IP_V6;
  p_addr_protocol = (CPU_INT08U *)p_addr_grp;
  addr_protocol_len = NET_IPv6_ADDR_SIZE;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetIF_AddrMulticastRemove(if_nbr,
                            p_addr_protocol,
                            addr_protocol_len,
                            addr_protocol_type,
                            &local_err);

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetIF_LinkStateUnSubscribeHandler(if_nbr, &NetMLDP_LinkStateNotification, &local_err);

  PP_UNUSED_PARAM(local_err);
}

/****************************************************************************************************//**
 *                                           NetMLDP_HostGrpClr()
 *
 * @brief    Clear MLDP host group controls.
 *
 * @param    p_host_grp  Pointer to a MLDP host group.
 *******************************************************************************************************/
static void NetMLDP_HostGrpClr(NET_MLDP_HOST_GRP *p_host_grp)
{
  p_host_grp->PrevIF_ListPtr = DEF_NULL;
  p_host_grp->NextIF_ListPtr = DEF_NULL;

  p_host_grp->TmrPtr = DEF_NULL;
  p_host_grp->Delay_ms = 0u;

  p_host_grp->IF_Nbr = NET_IF_NBR_NONE;

  Mem_Copy(&p_host_grp->AddrGrp, &NetIPv6_AddrNone, sizeof(p_host_grp->AddrGrp));

  p_host_grp->State = NET_MLDP_HOST_GRP_STATE_FREE;
  p_host_grp->RefCtr = 0u;
  p_host_grp->Flags = NET_MLDP_FLAG_NONE;
}

/****************************************************************************************************//**
 *                                       NetMLDP_HostGrpReportDlyTimeout()
 *
 * @brief    Transmit an MLDP report on MLDP Delay timeout.
 *
 * @param    p_host_grp_timeout  Pointer to a host group (see Note #1b).
 *
 * @note     (1) Ideally, network timer callback functions could be defined as '[(void) (OBJECT *)]'
 *                               type functions -- even though network timer API functions cast callback functions
 *                               to generic 'CPU_FNCT_PTR' type (i.e. '[(void) (void *)]').
 *
 *                                   - (a) (1) Unfortunately, ISO/IEC 9899:TC2, Section 6.3.2.3.(7) states that "a pointer
 *                               to an object ... may be converted to a pointer to a different object ...
 *                               [but] if the resulting pointer is not correctly aligned ... the behavior
 *                               is undefined".
 *
 *                               And since compilers may NOT correctly convert 'void' pointers to non-'void'
 *                               pointer arguments, network timer callback functions MUST avoid incorrect
 *                               pointer conversion behavior between 'void' pointer parameters & non-'void'
 *                               pointer arguments & therefore CANNOT be defined as '[(void) (OBJECT *)]'.
 *
 *                                   - (2) However, Section 6.3.2.3.(1) states that "a pointer to void may be converted
 *                               to or from a pointer to any ... object ... A pointer to any ... object ...
 *                               may be converted to a pointer to void and back again; the result shall
 *                               compare equal to the original pointer".
 *
 *                                   - (b) Therefore, to correctly convert 'void' pointer objects back to appropriate
 *                               network object pointer objects, network timer callback functions MUST :
 *
 *                                   - (1) Be defined as 'CPU_FNCT_PTR' type (i.e. '[(void) (void *)]'); &       ...
 *                                   - (2) Explicitly cast 'void' pointer arguments to specific object pointers; ...
 *                                   - (A) in this case, a 'NET_MLDP_HOST_GRP' pointer.
 *
 *                               See also 'net_tmr.c  NetTmr_Get()  Note #3'.
 *
 * @note     (2) This function is a network timer callback function :
 *
 *                                   - (a) Clear the timer pointer,
 *                                   - (b) but do NOT re-free the timer.
 *
 * @note     (3) In case of a transmit error :
 *
 *                                   - (a) Configure a timer to attempt retransmission of the MLDP report, if the error
 *                               is transitory.
 *
 *                                   - (b) Revert to 'IDLE' state, if the error is permanent.
 *******************************************************************************************************/
static void NetMLDP_HostGrpReportDlyTimeout(void *p_host_grp_timeout)
{
  NET_MLDP_HOST_GRP *p_host_grp;
  CPU_INT32U        timeout_ms;
  RTOS_ERR          local_err;

  p_host_grp = (NET_MLDP_HOST_GRP *)p_host_grp_timeout;         // See Note #1b2A.

  p_host_grp->TmrPtr = DEF_NULL;                                // Clear tmr (see Note #2).

  //                                                               ------------------ TX MLDP REPORT ------------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetMLDP_TxReport(p_host_grp->IF_Nbr,
                   &p_host_grp->AddrGrp,
                   &local_err);
  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:                                         // If NO err, ...
    case RTOS_ERR_NET_IF_LINK_DOWN:
      p_host_grp->State = NET_MLDP_HOST_GRP_STATE_IDLE;         // ... set state to 'IDLE'.
      break;

    case RTOS_ERR_POOL_EMPTY:                                   // If tx err, ...
                                                                // ... cfg new tmr (see Note #3a).
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      timeout_ms = (NET_MLDP_HOST_GRP_REPORT_DLY_RETRY_SEC * 1000);
      p_host_grp->TmrPtr = NetTmr_Get(NetMLDP_HostGrpReportDlyTimeout,
                                      p_host_grp,
                                      timeout_ms,
                                      NET_TMR_OPT_NONE,
                                      &local_err);
      if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
        p_host_grp->State = NET_MLDP_HOST_GRP_STATE_DELAYING;
        p_host_grp->Delay_ms = NET_MLDP_HOST_GRP_REPORT_DLY_RETRY_SEC * 1000;
      } else {
        p_host_grp->State = NET_MLDP_HOST_GRP_STATE_IDLE;
        p_host_grp->Delay_ms = 0;
      }
      break;

    default:                                                    // On all other errs, ...
      p_host_grp->State = NET_MLDP_HOST_GRP_STATE_IDLE;         // ... set state to 'IDLE'.
      break;
  }
}

/****************************************************************************************************//**
 *                                       NetMLDP_LinkStateNotification()
 *
 * @brief    Callback function called when the Interface link state has changed and the MDLP module
 *           needs to update its memberships on the Interface.
 *
 * @param    if_nbr      Interface number associated with the link status notification.
 *
 * @param    link_state  Current IF link state :
 *                       NET_IF_LINK_UP
 *                       NET_IF_LINK_DOWN
 *******************************************************************************************************/
static void NetMLDP_LinkStateNotification(NET_IF_NBR        if_nbr,
                                          NET_IF_LINK_STATE link_state)
{
  NET_MLDP_HOST_GRP *p_host_grp;
  RTOS_ERR          local_err;

  //                                                               ----------- ACQUIRE NETWORK GLOBAL LOCK ------------
  Net_GlobalLockAcquire((void *)NetMLDP_LinkStateNotification);

  switch (link_state) {
    case NET_IF_LINK_UP:
      SLIST_FOR_EACH_ENTRY(NetMLDP_DataPtr->HostGrpListPtr, p_host_grp, NET_MLDP_HOST_GRP, ListNode) {
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

        if (p_host_grp->IF_Nbr == if_nbr) {
          NetMLDP_TxAdvertiseMembership(p_host_grp, &local_err);
        }

        PP_UNUSED_PARAM(local_err);
      }
      break;

    case NET_IF_LINK_DOWN:
    default:
      break;
  }

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_MLDP_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
