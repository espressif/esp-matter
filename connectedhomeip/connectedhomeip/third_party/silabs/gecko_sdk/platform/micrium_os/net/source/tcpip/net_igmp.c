/***************************************************************************//**
 * @file
 * @brief Network Igmp Layer - (Internet Group Management Protocol)
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

#ifdef  NET_IGMP_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_util.h>
#include  <net/include/net_ipv4.h>

#include  "net_igmp_priv.h"
#include  "net_ipv4_priv.h"
#include  "net_buf_priv.h"
#include  "net_priv.h"
#include  "net_if_priv.h"
#include  "net_util_priv.h"

#include  <common/include/lib_math.h>
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
 *                                           IGMP HEADER DEFINES
 *******************************************************************************************************/

#define  NET_IGMP_HDR_VER_MASK                          0xF0u
#define  NET_IGMP_HDR_VER_SHIFT                            4u
#define  NET_IGMP_HDR_VER                                  1u   // Supports IGMPv1 ONLY (see 'net_igmp.h  Note #2').

#define  NET_IGMP_HDR_TYPE_MASK                         0x0Fu

/********************************************************************************************************
 *                                       IGMP MESSAGE SIZE DEFINES
 *
 * Note(s) : (1) RFC #1112, Appendix I, Section 'State Transition Diagram' states that "to be valid, the
 *               ... [received] message[s] must be at least 8 octets long".
 *******************************************************************************************************/

#define  NET_IGMP_MSG_LEN                  NET_IGMP_HDR_SIZE
#define  NET_IGMP_MSG_LEN_DATA                             0

/********************************************************************************************************
 *                                       IGMP MESSAGE TYPE DEFINES
 *
 * Note(s) : (1) RFC #1112, Appendix I, Section 'Type' states that "there are two types of IGMP message[s]
 *               ... to hosts" :
 *
 *               (a) 1 = Host Membership Query
 *               (b) 2 = Host Membership Report
 *******************************************************************************************************/

#define  NET_IGMP_MSG_TYPE_NONE                            0u
#define  NET_IGMP_MSG_TYPE_QUERY                           1u   // See Note #1a.
#define  NET_IGMP_MSG_TYPE_REPORT                          2u   // See Note #1b.

/********************************************************************************************************
 *                                       IGMP HOST GROUP STATES
 *
 *                                        -------------------
 *                                        |                 |
 *                                        |                 |
 *                                        |                 |
 *                                        |                 |
 *                           ------------>|      FREE       |<------------
 *                           |            |                 |            |
 *                           |            |                 |            |
 *                           |            |                 |            |
 *                           |            |                 |            |
 *                           |            -------------------            | (1e) LEAVE GROUP
 *                           |                     |                     |
 *                           | (1e) LEAVE GROUP    | (1a) JOIN GROUP     |
 *                           |                     |                     |
 *                  -------------------            |            -------------------
 *                   |                 |<------------            |                 |
 *                   |                 |                         |                 |
 *                   |                 |<------------------------|                 |
 *                   |                 |  (1c) QUERY  RECEIVED   |                 |
 *                   |    DELAYING     |                         |      IDLE       |
 *                   |                 |------------------------>|                 |
 *                   |                 |  (1b) REPORT RECEIVED   |                 |
 *                   |                 |                         |                 |
 *                   |                 |------------------------>|                 |
 *                   -------------------  (1d) TIMER  EXPIRED    -------------------
 *
 * Note(s) : (1) RFC #1112, Appendix I, Sections 'Informal Protocol Description' & 'State Transition Diagram'
 *               outline the IGMP state diagram :
 *
 *               (a) An application performs a request to join a multicast group.  A new IGMP host group
 *                   entry is allocated from the IGMP host group pool & inserted into the IGMP Host Group
 *                   List in the 'DELAYING' state.  A timer is started to transmit a report to inform the
 *                   IGMP enabled router.
 *
 *               (b) The host receives a valid IGMP Host Membership Report message, on the interface the
 *                   host has joined the group on.  The timer is stopped & the host group transitions
 *                   into the 'IDLE' state.
 *
 *               (c) A query is received for that IGMP group.  The host group transitions into the 'DELAYING'
 *                   state & a timer is started to transmit a report to inform the IGMP router.
 *
 *               (d) The report delay timer expires for the group & a report for that group is transmitted.
 *                   The host group then transitions into the 'IDLE' state.
 *
 *               (e) The application leaves the group on the interface; the IGMP host group is then freed.
 *
 *           (2) RFC #1112, Section 7.2 states that "to support IGMP, every level 2 host must join the
 *               all-hosts group (address 224.0.0.1) ... and must remain a member for as long as the
 *               host is active".
 *
 *               (a) Therefore, the group 224.0.0.1 is considered a special group, & is always in the
 *                   'STATIC' state, meaning it neither can be deleted, nor be put in the 'IDLE' or
 *                   'DELAYING' state.
 *
 *               (b) However, since network interfaces are not yet enabled at IGMP initialization time,
 *                   the host delays joining the "all-hosts" group on an interface until the first group
 *                   membership is requested on an interface.
 *******************************************************************************************************/

#define  NET_IGMP_HOST_GRP_STATE_NONE                      0u
#define  NET_IGMP_HOST_GRP_STATE_FREE                      1u
#define  NET_IGMP_HOST_GRP_STATE_DELAYING                  2u
#define  NET_IGMP_HOST_GRP_STATE_IDLE                      3u

#define  NET_IGMP_HOST_GRP_STATE_STATIC                   10u   // See Note #2.

/********************************************************************************************************
 *                                           IGMP REPORT DEFINES
 *
 * Note(s) : (1) RFC #1112, Appendix I, Section 'Informal Protocol Description' states that :
 *
 *               (a) "When a host joins a new group, it should immediately transmit a Report for that
 *                       group [...].  To cover the possibility of the initial Report being lost or damaged,
 *                       it is recommended that it be repeated once or twice after short delays."
 *
 *                       The delay between the report transmissions is set to 2 seconds in this implementation.
 *
 *               (b) "When a host receives a Query [...] it starts a report delay timer for each of its
 *                       group memberships on the network interface of the incoming Query.  Each timer is
 *                       set to a different, randomly-chosen value between zero and [10] seconds."
 *
 *           (2) When a transmit error occurs when attempting to transmit an IGMP report, a new timer
 *               is set with a delay of NET_IGMP_HOST_GRP_REPORT_DLY_RETRY_SEC seconds to retransmit
 *               the report.
 *******************************************************************************************************/

#define  NET_IGMP_HOST_GRP_REPORT_DLY_JOIN_SEC             2    // See Note #1a.
                                                                // See Note #1b.
#define  NET_IGMP_HOST_GRP_REPORT_DLY_MIN_SEC              0
#define  NET_IGMP_HOST_GRP_REPORT_DLY_MAX_SEC             10

#define  NET_IGMP_HOST_GRP_REPORT_DLY_RETRY_SEC            2    // See Note #2.

/********************************************************************************************************
 *                                           IGMP FLAG DEFINES
 *******************************************************************************************************/

//                                                                 ------------------ NET IGMP FLAGS ------------------
#define  NET_IGMP_FLAG_NONE                       DEF_BIT_NONE
#define  NET_IGMP_FLAG_USED                       DEF_BIT_00    // IGMP host grp cur used; i.e. NOT in free pool.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   IGMP HOST GROUP QUANTITY DATA TYPE
 *
 * Note(s) : (1) NET_IGMP_HOST_GRP_NBR_MAX  SHOULD be #define'd based on 'NET_IGMP_HOST_GRP_QTY' data type
 *               declared.
 *******************************************************************************************************/

typedef  CPU_INT16U NET_IGMP_HOST_GRP_QTY;                      // Defines max qty of IGMP host groups to support.

/********************************************************************************************************
 *                                   IGMP HOST GROUP STATE DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IGMP_HOST_GRP_STATE;

/********************************************************************************************************
 *                                           IGMP FLAGS DATA TYPE
 *******************************************************************************************************/

typedef  NET_FLAGS NET_IGMP_FLAGS;

/********************************************************************************************************
 *                                               IGMP HEADER
 *
 * Note(s) : (1) See RFC #1112, Appendix I for IGMP packet header format.
 *
 *           (2) IGMP Version Number & Message Type are encoded in the first octet of an IGMP header as follows :
 *
 *                           7 6 5 4   3 2 1 0
 *                       ---------------------
 *                       |  V E R  | T Y P E |
 *                       ---------------------
 *
 *                   where
 *                           VER         IGMP version; currently 1 (see 'net_igmp.h  Note #2')
 *                           TYPE        IGMP message type         (see 'net_igmp.h  IGMP MESSAGE TYPE DEFINES)
 *******************************************************************************************************/

//                                                                 ------------------- NET IGMP HDR -------------------
typedef  struct  net_igmp_hdr {
  CPU_INT08U    Ver_Type;                                       // IGMP pkt  ver/type (see Note #2).
  CPU_INT08U    Unused;
  CPU_INT16U    ChkSum;                                         // IGMP pkt  chk sum.
  NET_IPv4_ADDR AddrGrp;                                        // IPv4   host grp addr.
} NET_IGMP_HDR;

/********************************************************************************************************
 *                                       IGMP HOST GROUP DATA TYPES
 *
 *                                           NET_IGMP_HOST_GRP
 *                                           |-----------------|
 *                                           | Host Group Type |
 *                             Previous      |-----------------|
 *                           Host Group <------------O         |
 *                                           |-----------------|        Next
 *                                           |        O------------> Host Group
 *                                           |-----------------|
 *                                           |        O------------> Host Group
 *                                           |-----------------|        Timer
 *                                           |    Interface    |
 *                                           |     Number      |
 *                                           |-----------------|
 *                                           |   IP Address    |
 *                                           |-----------------|
 *                                           |      State      |
 *                                           |-----------------|
 *                                           |    Reference    |
 *                                           |     Counter     |
 *                                           |-----------------|
 *******************************************************************************************************/

//                                                                 ---------------- NET IGMP HOST GRP -----------------
typedef  struct  net_igmp_host_grp NET_IGMP_HOST_GRP;

struct  net_igmp_host_grp {
  NET_IGMP_HOST_GRP       *PrevPtr;                             // Ptr to PREV IGMP host grp.
  NET_IGMP_HOST_GRP       *NextPtr;                             // Ptr to NEXT IGMP host grp.

  NET_TMR                 *TmrPtr;                              // Ptr to host grp TMR.

  NET_IF_NBR              IF_Nbr;                               // IGMP   host grp IF nbr.
  NET_IPv4_ADDR           AddrGrp;                              // IGMP   host grp IPv4 addr.

  NET_IGMP_HOST_GRP_STATE State;                                // IGMP   host grp state.
  CPU_INT16U              RefCtr;                               // IGMP   host grp ref ctr.

  NET_IGMP_FLAGS          Flags;                                // IGMP   host grp flags.

  SLIST_MEMBER            ListNode;
};

/********************************************************************************************************
 *                                       IGMP MODULE DATA DATA TYPE
 *******************************************************************************************************/

typedef  struct  net_igmp_data {
  MEM_DYN_POOL  HostGrpPool;
  SLIST_MEMBER  *HostGrpListPtr;
#if (NET_STAT_POOL_IGMP_EN == DEF_ENABLED)
  NET_STAT_POOL HostGrpPoolStat;
#endif
  RAND_NBR      RandSeed;
  CPU_BOOLEAN   *AllHostsJoinedOnIF_Tbl;
} NET_IGMP_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_IGMP_DATA *NetIGMP_DataPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------- RX FNCTS -------
static CPU_INT08U NetIGMP_RxPktValidate(NET_BUF      *p_buf,
                                        NET_BUF_HDR  *p_buf_hdr,
                                        NET_IGMP_HDR *p_igmp_hdr,
                                        RTOS_ERR     *p_err);

static void NetIGMP_RxMsgQuery(NET_IF_NBR   if_nbr,
                               NET_IGMP_HDR *p_igmp_hdr);

static void NetIGMP_RxMsgReport(NET_IF_NBR   if_nbr,
                                NET_IGMP_HDR *p_igmp_hdr);

static void NetIGMP_RxPktFree(NET_BUF *p_buf);

//                                                                 ------- TX FNCTS -------
static void NetIGMP_TxMsg(NET_IF_NBR    if_nbr,
                          NET_IPv4_ADDR addr_src,
                          NET_IPv4_ADDR addr_grp,
                          CPU_INT08U    type,
                          RTOS_ERR      *p_err);

static void NetIGMP_TxMsgReport(NET_IGMP_HOST_GRP *p_host_grp,
                                RTOS_ERR          *p_err);

static void NetIGMP_TxIxDataGet(NET_IF_NBR if_nbr,
                                CPU_INT16U data_len,
                                CPU_INT16U *p_ix);

static void NetIGMP_TxPktFree(NET_BUF *p_buf);

static void NetIGMP_TxPktDiscard(NET_BUF *p_buf);

//                                                                 -- IGMP HOST GRP FNCTS -
static NET_IGMP_HOST_GRP *NetIGMP_HostGrpSrch(NET_IF_NBR    if_nbr,
                                              NET_IPv4_ADDR addr_grp);

static NET_IGMP_HOST_GRP *NetIGMP_HostGrpAdd(NET_IF_NBR    if_nbr,
                                             NET_IPv4_ADDR addr_grp,
                                             RTOS_ERR      *p_err);

static void NetIGMP_HostGrpRemove(NET_IGMP_HOST_GRP *p_host_grp);

static void NetIGMP_HostGrpReportDlyTimeout(void *p_host_grp_timeout);

static void NetIGMP_HostGrpClr(NET_IGMP_HOST_GRP *p_host_grp);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetIGMP_HostGrpJoin()
 *
 * @brief    Join a host group.
 *
 * @param    if_nbr      Interface number to join host group.
 *
 * @param    addr_grp    IP address of host group to join.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_OK,   if host group successfully joined.
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
 *
 * @note     (2) IP host group address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIGMP_HostGrpJoin(NET_IF_NBR    if_nbr,
                                NET_IPv4_ADDR addr_grp,
                                RTOS_ERR      *p_err)
{
  CPU_BOOLEAN host_grp_join = DEF_FAIL;

  //                                                               --------------- VALIDATE RTN ERR PTR ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Net_GlobalLockAcquire((void *)NetIGMP_HostGrpJoin);          // Acquire net lock (see Note #1b).

  //                                                               Join host grp.
  host_grp_join = NetIGMP_HostGrpJoinHandler(if_nbr, addr_grp, p_err);

  Net_GlobalLockRelease();                                      // Release net lock.

  return (host_grp_join);
}

/****************************************************************************************************//**
 *                                           NetIGMP_HostGrpLeave()
 *
 * @brief    Leave a host group.
 *
 * @param    if_nbr      Interface number to leave host group.
 *
 * @param    addr_grp    IP address of host group to leave.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_FOUND
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
 *
 * @note     (2) IP host group address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIGMP_HostGrpLeave(NET_IF_NBR    if_nbr,
                                 NET_IPv4_ADDR addr_grp,
                                 RTOS_ERR      *p_err)
{
  CPU_BOOLEAN host_grp_leave = DEF_FAIL;

  //                                                               --------------- VALIDATE RTN ERR PTR ---------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               Acquire net lock (see Note #1b).
  Net_GlobalLockAcquire((void *)NetIGMP_HostGrpLeave);

  //                                                               Leave host grp.
  host_grp_leave = NetIGMP_HostGrpLeaveHandler(if_nbr, addr_grp, p_err);

  Net_GlobalLockRelease();                                      // Release net lock.

  return (host_grp_leave);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIGMP_Init()
 *
 * @brief    (1) Initialize Internet Group Management Protocol Layer :
 *               - (a) Initialize IGMP host group pool
 *               - (b) Initialize IGMP host group table
 *               - (c) Initialize IGMP Host Group List pointer
 *               - (d) Initialize IGMP all-hosts groups
 *               - (e) Initialize IGMP random seed
 *
 * @param    p_mem_seg   Pointer to memory segment for dynamic allocation.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) IGMP host group pool MUST be initialized PRIOR to initializing the pool with pointers
 *                       to IGMP host group.
 *
 * @note     (3) RFC #1112, Section 7.2 states that "every level 2 host must join the 'all-hosts'
 *               group ... on each network interface at initialization time and must remain a
 *               member for as long as the host is active".
 *               @n
 *               However, network interfaces are not enabled at IGMP initialization time &
 *               cannot be completely configured.  Therefore, joining the "all-hosts" group is
 *               postponed until the first group membership is requested on an interface. #### NET-802
 *               @n
 *               See also 'net_igmp.h  IGMP HOST GROUP STATES        Note #2b'
 *               & 'net_igmp.c  NetIGMP_HostGrpJoinHandler()  Note #7'.
 *******************************************************************************************************/
void NetIGMP_Init(MEM_SEG  *p_mem_seg,
                  RTOS_ERR *p_err)
{
  NET_IF_NBR if_nbr;

  //                                                               ----------- INITIALIZE IGMP DATA SEGMENT -----------
  NetIGMP_DataPtr = (NET_IGMP_DATA *)Mem_SegAlloc("IGMP Data",
                                                  p_mem_seg,
                                                  sizeof(NET_IGMP_DATA),
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ---------- INIT IGMP HOST GRP POOL/STATS -----------
  //                                                               Init-clr IGMP host grp pool (see Note #2).
#if (NET_STAT_POOL_IGMP_EN == DEF_ENABLED)
  NetStat_PoolInit(&NetIGMP_DataPtr->HostGrpPoolStat,
                   NET_MCAST_CFG_HOST_GRP_NBR_MAX);
#endif

  //                                                               --------- CREATE POOL FOR IGMP HOST GROUP ----------
  Mem_DynPoolCreate("IGMP Host Group Pool",
                    &NetIGMP_DataPtr->HostGrpPool,
                    p_mem_seg,
                    sizeof(NET_IGMP_HOST_GRP),
                    sizeof(CPU_ALIGN),
                    1,
                    NET_MCAST_CFG_HOST_GRP_NBR_MAX,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ----------- INIT IGMP HOST GRP LIST PTR ------------
  SList_Init(&NetIGMP_DataPtr->HostGrpListPtr);

  //                                                               ---------------- INIT ALL-HOSTS GRP ----------------
  NetIGMP_DataPtr->AllHostsJoinedOnIF_Tbl = (CPU_BOOLEAN *)Mem_SegAlloc("All Hosts Joined On IF Table",
                                                                        p_mem_seg,
                                                                        sizeof(CPU_BOOLEAN) * Net_CoreDataPtr->IF_NbrTot,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (if_nbr = NET_IF_NBR_BASE_CFGD; if_nbr < Net_CoreDataPtr->IF_NbrTot; if_nbr++) {
    NetIGMP_DataPtr->AllHostsJoinedOnIF_Tbl[if_nbr] = DEF_NO;
  }

  //                                                               ------------------ INIT RAND SEED ------------------
  NetIGMP_DataPtr->RandSeed = 1u;                               // See 'lib_math.c  Math_Init()  Note #2'.
}

/****************************************************************************************************//**
 *                                       NetIGMP_HostGrpJoinHandler()
 *
 * @brief    (1) Join a host group :
 *               - (a) Validate interface number
 *               - (b) Validate internet group address
 *               - (c) Search IGMP Host Group List for host group with corresponding address
 *                     & interface number
 *               - (d) If host group NOT found, allocate new host group.
 *               - (e) Advertise membership to multicast router(s)
 *
 * @param    if_nbr      Interface number to join host group (see Note  #4).
 *
 * @param    addr_grp    IP address of host group to join    (see Notes #5 & #6).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if host group successfully joined.
 *           DEF_FAIL, otherwise.
 *
 * @note     (2) NetIGMP_HostGrpJoinHandler() is called by network protocol suite function(s) & MUST
 *               be called with the global network lock already acquired.
 *               See also 'NetIGMP_HostGrpJoin()  Note #1'.
 *
 * @note     (3) NetIGMP_HostGrpJoinHandler() blocked until network initialization completes.
 *
 * @note     (4) IGMP host groups can ONLY be joined on configured interface(s); NOT on any
 *               localhost/loopback interface(s).
 *
 * @note     (5) IP host group address MUST be in host-order.
 *
 * @note     (6) See RFC #1112, Section 4
 *
 * @note     (7) See RFC #1112, Section 7.2
 *
 * @note     (8) See RFC #1112, Section 7.1
 *
 * @note     (9) See RFC #1112, Appendix I, Section 'Informal Protocol Description'
 *******************************************************************************************************/
CPU_BOOLEAN NetIGMP_HostGrpJoinHandler(NET_IF_NBR    if_nbr,
                                       NET_IPv4_ADDR addr_grp,
                                       RTOS_ERR      *p_err)
{
  NET_IGMP_HOST_GRP *p_host_grp;
  CPU_INT32U        timeout_ms;
  CPU_BOOLEAN       is_en;
  CPU_BOOLEAN       rtn_val = DEF_FAIL;
  RTOS_ERR          local_err;

  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_en = NetIF_IsEnCfgdHandler(if_nbr, p_err);
  if (is_en != DEF_YES) {                                       // If cfg'd IF NOT en'd (see Note #4), ...
    goto exit;                                                  // ... rtn err.
  }

  //                                                               Chk all-hosts grp (see Note #7b).
  if (NetIGMP_DataPtr->AllHostsJoinedOnIF_Tbl[if_nbr] != DEF_YES) {
    NetIGMP_HostGrpAdd(if_nbr,
                       NET_IPv4_ADDR_MULTICAST_ALL_HOSTS,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    NetIGMP_DataPtr->AllHostsJoinedOnIF_Tbl[if_nbr] = DEF_YES;
  }

  //                                                               -------------- VALIDATE HOST GRP ADDR --------------
  if (addr_grp == NET_IPv4_ADDR_MULTICAST_ALL_HOSTS) {          // If host grp addr = all-hosts addr, ...
    rtn_val = DEF_OK;                                           // ... rtn (see Note #6).
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }

  if ((addr_grp < NET_IPv4_ADDR_MULTICAST_HOST_MIN)             // If host grp addr NOT valid multicast (see Note #6),
      || (addr_grp > NET_IPv4_ADDR_MULTICAST_HOST_MAX)) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);
  }

  //                                                               ---------------- SRCH HOST GRP LIST ----------------
  p_host_grp = NetIGMP_HostGrpSrch(if_nbr, addr_grp);
  if (p_host_grp != DEF_NULL) {                                 // If host grp found, ...
    p_host_grp->RefCtr++;                                       // ... inc ref ctr.
    rtn_val = DEF_OK;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit;
  }

  p_host_grp = NetIGMP_HostGrpAdd(if_nbr, addr_grp, p_err);     // Add new host grp into Host Grp List (see Note #8).
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  //                                                               --------------- ADVERTISE MEMBERSHIP ---------------
  NetIGMP_TxMsgReport(p_host_grp, &local_err);                  // See Note #9.

  //                                                               Cfg report timeout.
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  timeout_ms = (NET_IGMP_HOST_GRP_REPORT_DLY_JOIN_SEC * 1000);
  p_host_grp->TmrPtr = NetTmr_Get(NetIGMP_HostGrpReportDlyTimeout,
                                  p_host_grp,                   // See Note #7.
                                  timeout_ms,
                                  NET_TMR_OPT_NONE,
                                  &local_err);

  p_host_grp->State = (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) ? NET_IGMP_HOST_GRP_STATE_DELAYING
                      : NET_IGMP_HOST_GRP_STATE_IDLE;

  rtn_val = DEF_OK;

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                       NetIGMP_HostGrpLeaveHandler()
 *
 * @brief    (1) Leave a host group :
 *               - (a) Search IGMP Host Group List for host group with corresponding address
 *                     & interface number
 *               - (b) If host group found, remove host group from IGMP Host Group List
 *
 * @param    if_nbr      Interface number to leave host group.
 *
 * @param    addr_grp    IP address of host group to leave (see Note #4).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if host group successfully left.
 *           DEF_FAIL, otherwise.
 *
 * @note     (2) NetIGMP_HostGrpLeaveHandler() is called by network protocol suite function(s) & MUST
 *               be called with the global network lock already acquired.
 *               See also 'NetIGMP_HostGrpLeave()  Note #1'.
 *
 * @note     (3) NetIGMP_HostGrpLeaveHandler() blocked until network initialization completes.
 *
 * @note     (4) IP host group address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIGMP_HostGrpLeaveHandler(NET_IF_NBR    if_nbr,
                                        NET_IPv4_ADDR addr_grp,
                                        RTOS_ERR      *p_err)
{
  NET_IGMP_HOST_GRP *p_host_grp = DEF_NULL;
  CPU_BOOLEAN       rtn_val = DEF_FAIL;

  //                                                               ---------------- SRCH HOST GRP LIST ----------------
  p_host_grp = NetIGMP_HostGrpSrch(if_nbr, addr_grp);
  if (p_host_grp == DEF_NULL) {                                 // If host grp NOT found, ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);                       // ... rtn err.
    goto exit;
  }

  p_host_grp->RefCtr--;                                         // Dec ref ctr.
  if (p_host_grp->RefCtr < 1) {                                 // If last ref to host grp, ...
    NetIGMP_HostGrpRemove(p_host_grp);                          // ... remove host grp from Host Grp List.
  }

  rtn_val = DEF_OK;

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                           NetIGMP_IsGrpJoinedOnIF()
 *
 * @brief    Check for joined host group on specified interface.
 *
 * @param    if_nbr      Interface number to search.
 *
 * @param    addr_grp    IP address of host group.
 *
 * @return   DEF_YES, if host group address joined on interface.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIGMP_IsGrpJoinedOnIF(NET_IF_NBR    if_nbr,
                                    NET_IPv4_ADDR addr_grp)
{
  NET_IGMP_HOST_GRP *p_host_grp;
  CPU_BOOLEAN       grp_joined;

  p_host_grp = NetIGMP_HostGrpSrch(if_nbr, addr_grp);
  grp_joined = (p_host_grp != DEF_NULL) ? DEF_YES : DEF_NO;

  return (grp_joined);
}

/****************************************************************************************************//**
 *                                               NetIGMP_Rx()
 *
 * @brief    - (1) Process received IGMP packets & update host group status :
 *               - (a) Validate IGMP packet
 *               - (b) Update   IGMP host group status
 *               - (c) Free     IGMP packet
 *               - (d) Update receive statistics
 *           - (2) Although IGMP data units are typically referred to as 'messages' (see RFC #1112,
 *                   Appendix I), the term 'IGMP packet' (see RFC #1983, 'packet') is used for IGMP
 *                   Receive until the packet is validated as an IGMP message.
 *
 * @param    p_buf   Pointer to network buffer that received IGMP packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetIGMP_Rx(NET_BUF  *p_buf,
                RTOS_ERR *p_err)
{
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  NET_CTR *p_ctr;
#endif
  NET_BUF_HDR  *p_buf_hdr;
  NET_IGMP_HDR *p_igmp_hdr;
  NET_IF_NBR   if_nbr;
  CPU_INT08U   msg_type;

  NET_CTR_STAT_INC(Net_StatCtrs.IGMP.RxMsgCtr);

  p_buf_hdr = &p_buf->Hdr;

  p_igmp_hdr = (NET_IGMP_HDR *)&p_buf->DataPtr[p_buf_hdr->IGMP_MsgIx];
  if_nbr = p_buf_hdr->IF_Nbr;
  //                                                               Validate rx'd pkt.
  msg_type = NetIGMP_RxPktValidate(p_buf, p_buf_hdr, p_igmp_hdr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IGMP.RxPktDiscardedCtr);
    goto exit;
  }

  //                                                               ------------------ DEMUX IGMP MSG ------------------
  switch (msg_type) {
    case NET_IGMP_MSG_TYPE_QUERY:
      NetIGMP_RxMsgQuery(if_nbr, p_igmp_hdr);
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
      p_ctr = &Net_StatCtrs.IGMP.RxMsgQueryCtr;
#endif
      break;

    case NET_IGMP_MSG_TYPE_REPORT:
      NetIGMP_RxMsgReport(if_nbr, p_igmp_hdr);
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
      p_ctr = &Net_StatCtrs.IGMP.RxMsgReportCtr;
#endif
      break;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IGMP.RxHdrTypeCtr);
      NET_CTR_ERR_INC(Net_ErrCtrs.IGMP.RxPktDiscardedCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  NetIGMP_RxPktFree(p_buf);                                     // ------------------- FREE IGMP PKT ------------------

  //                                                               ------------------ UPDATE RX STATS -----------------
  NET_CTR_STAT_INC(Net_StatCtrs.IGMP.RxMsgCompCtr);
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
 *                                           NetIGMP_RxPktValidate()
 *
 * @brief    (1) Validate received IGMP packet :
 *           - (a) Validate the received message's length                          See Note #4
 *           - (b) (1) Validate the received packet's following IGMP header fields :
 *                   - (A) Version
 *                   - (B) Type
 *                   - (C) Checksum
 *               - (2) Validation ignores the following ICMP header fields :
 *                   - (A) Host Group Address
 *
 * @param    p_buf       Pointer to network buffer that received IGMP packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_igmp_hdr  Pointer to received packet's IGMP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See 'net_igmp.h  IGMP HEADER' for IGMP header format.
 *
 * @note     (3) See RFC #1112, Section 7.2
 *                       RFC #1122, Section 3.2.2
 *
 *                       'p_buf' NOT used to transmit ICMP error messages but is included for consistency.
 *
 * @note     (4) See RFC #1112, Appendix I, Section 'State Transition Diagram'
 *
 *                       Since IGMP message headers do NOT contain a message length field, the IGMP Message
 *                       Length is assumed to be the remaining IP Datagram Length.
 *******************************************************************************************************/
static CPU_INT08U NetIGMP_RxPktValidate(NET_BUF      *p_buf,
                                        NET_BUF_HDR  *p_buf_hdr,
                                        NET_IGMP_HDR *p_igmp_hdr,
                                        RTOS_ERR     *p_err)
{
  CPU_INT16U    igmp_msg_len;
  CPU_BOOLEAN   igmp_chk_sum_valid;
  CPU_INT08U    igmp_ver;
  CPU_INT08U    igmp_type = NET_IGMP_MSG_TYPE_NONE;
  NET_IPv4_ADDR igmp_grp_addr;

  PP_UNUSED_PARAM(p_buf);                                       // Prevent 'variable unused' warning (see Note #3).

  //                                                               ------------- VALIDATE IGMP RX MSG LEN -------------
  igmp_msg_len = p_buf_hdr->IP_DatagramLen;                     // See Note #3.
  p_buf_hdr->IGMP_MsgLen = igmp_msg_len;
  if (igmp_msg_len < NET_IGMP_MSG_SIZE_MIN) {                   // If msg len < min msg len, rtn err (see Note #4).
    NET_CTR_ERR_INC(Net_ErrCtrs.IGMP.RxHdrMsgLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ---------------- VALIDATE IGMP VER -----------------
  igmp_ver = p_igmp_hdr->Ver_Type & NET_IGMP_HDR_VER_MASK;      // See 'net_igmp.h  IGMP HEADER  Note #2'.
  igmp_ver >>= NET_IGMP_HDR_VER_SHIFT;
  if (igmp_ver != NET_IGMP_HDR_VER) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IGMP.RxHdrVerCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               -------------- VALIDATE IGMP MSG TYPE --------------
  NET_UTIL_VAL_COPY_GET_NET_32(&igmp_grp_addr, &p_igmp_hdr->AddrGrp);
  igmp_type = p_igmp_hdr->Ver_Type & NET_IGMP_HDR_TYPE_MASK;

  switch (igmp_type) {
    case NET_IGMP_MSG_TYPE_QUERY:
      if (p_buf_hdr->IP_AddrDest != NET_IPv4_ADDR_MULTICAST_ALL_HOSTS) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IGMP.RxPktInvalidAddrDestCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      break;

    case NET_IGMP_MSG_TYPE_REPORT:
      if (p_buf_hdr->IP_AddrDest != igmp_grp_addr) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IGMP.RxPktInvalidAddrDestCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      break;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IGMP.RxHdrTypeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  //                                                               -------------- VALIDATE IGMP CHK SUM ---------------
  igmp_chk_sum_valid = NetUtil_16BitOnesCplChkSumHdrVerify(p_igmp_hdr,
                                                           p_buf_hdr->IGMP_MsgLen);
  if (igmp_chk_sum_valid != DEF_OK) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IGMP.RxHdrChkSumCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

exit:
  return (igmp_type);
}

/****************************************************************************************************//**
 *                                           NetIGMP_RxMsgQuery()
 *
 * @brief    (1) Process IGMP received query :
 *           - (a) Search host group in IDLE     state
 *           - (b) Configure timer for host group
 *           - (c) Set    host group in DELAYING state
 *
 * @param    if_nbr      Interface number the packet was received from.
 *
 *
 * Argument(s) : if_nbr      Interface number the packet was received from.
 *
 *               p_igmp_hdr  Pointer to received packet's IGMP header.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) See 'net_igmp.h  IGMP HEADER' for IGMP header format.
 *
 *               (3) See 'net_igmp.h  IGMP HOST GROUP STATES  Note #1' for state transitions.
 *******************************************************************************************************/
static void NetIGMP_RxMsgQuery(NET_IF_NBR   if_nbr,
                               NET_IGMP_HDR *p_igmp_hdr)
{
  NET_IGMP_HOST_GRP *p_host_grp;
  CPU_INT16U        timeout_sec;
  CPU_INT32U        timeout_ms;
  RTOS_ERR          local_err;

  PP_UNUSED_PARAM(p_igmp_hdr);                                  // Prevent 'variable unused' compiler warning.

  SLIST_FOR_EACH_ENTRY(NetIGMP_DataPtr->HostGrpListPtr, p_host_grp, NET_IGMP_HOST_GRP, ListNode) {
    if (p_host_grp->IF_Nbr == if_nbr) {                         // If host grp IF nbr is query IF nbr & ..
                                                                // .. host grp state  is IDLE,          ..
      if (p_host_grp->State == NET_IGMP_HOST_GRP_STATE_IDLE) {
        //                                                         .. calc new rand timeout             ..
        NetIGMP_DataPtr->RandSeed = Math_RandSeed(NetIGMP_DataPtr->RandSeed);
        timeout_sec = NetIGMP_DataPtr->RandSeed % (CPU_INT16U)(NET_IGMP_HOST_GRP_REPORT_DLY_MAX_SEC + 1);
        timeout_ms = timeout_sec * 1000;

        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        //                                                         .. & set tmr.
        p_host_grp->TmrPtr = NetTmr_Get(NetIGMP_HostGrpReportDlyTimeout,
                                        p_host_grp,
                                        timeout_ms,
                                        NET_TMR_OPT_NONE,
                                        &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {         // If err setting tmr,     ...
          RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
          NetIGMP_TxMsgReport(p_host_grp, &local_err);          // ... tx report immed'ly; ...
        } else {                                                // ... else set host grp state to DELAYING.
          p_host_grp->State = NET_IGMP_HOST_GRP_STATE_DELAYING;
        }
      }
    }
  }
}

/****************************************************************************************************//**
 *                                           NetIGMP_RxMsgReport()
 *
 * @brief    (1) Process received IGMP report :
 *           - (a) Search host group in DELAYING state & targeted by report
 *           - (b) Free   timer
 *           - (c) Set    host group in IDLE     state
 *
 * @param    if_nbr      Interface number the packet was received from.
 *
 *
 * Argument(s) : if_nbr      Interface number the packet was received from.
 *
 *               p_igmp_hdr  Pointer to received packet's IGMP header.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) See 'net_igmp.h  IGMP HEADER' for IGMP header format.
 *
 *               (3) See 'net_igmp.h  IGMP HOST GROUP STATES  Note #1' for state transitions.
 *******************************************************************************************************/
static void NetIGMP_RxMsgReport(NET_IF_NBR   if_nbr,
                                NET_IGMP_HDR *p_igmp_hdr)
{
  NET_IPv4_ADDR     addr_grp;
  NET_IGMP_HOST_GRP *p_host_grp;

  NET_UTIL_VAL_COPY_GET_NET_32(&addr_grp, &p_igmp_hdr->AddrGrp);

  p_host_grp = NetIGMP_HostGrpSrch(if_nbr, addr_grp);

  if (p_host_grp != DEF_NULL) {                                      // If host grp            ...
    if (p_host_grp->State == NET_IGMP_HOST_GRP_STATE_DELAYING) {     // ... in DELAYING state, ...
      if (p_host_grp->TmrPtr != DEF_NULL) {
        NetTmr_Free(p_host_grp->TmrPtr);                             // ... free tmr if avail  ...
        p_host_grp->TmrPtr = DEF_NULL;
      }

      p_host_grp->State = NET_IGMP_HOST_GRP_STATE_IDLE;              // ... & set to IDLE state.
    }
  }
}

/****************************************************************************************************//**
 *                                           NetIGMP_RxPktFree()
 *
 * @brief    Free network buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *******************************************************************************************************/
static void NetIGMP_RxPktFree(NET_BUF *p_buf)
{
  (void)NetBuf_FreeBuf(p_buf, DEF_NULL);
}

/****************************************************************************************************//**
 *                                               NetIGMP_TxMsg()
 *
 * @brief    (1) Prepare & transmit an IGMP Report or IGMP Query :
 *           - (a) Validate IGMP Message Type          See 'net_igmp.h  IGMP MESSAGE TYPE DEFINES'
 *                                                       & 'net_icmp.c  Note #2c'
 *           - (b) Get buffer for IGMP Message :
 *               - (1) Initialize IGMP Message buffer controls
 *           - (c) Prepare IGMP Message :
 *               - (1) Type                            See  Note #1a
 *               - (2) Version                         See 'net_igmp.h  IGMP HEADER DEFINES'
 *               - (3) Group address
 *                   - (A) RFC #1112, Appendix I, Section 'Informal Protocl Description' states
 *                           that "a report is sent with" :
 *                       - (1) "an IP destination address equal to the host group address being
 *                                   reported" ...
 *                       - (2) "and with an IP time-to-live of 1, so that other members of the
 *                               same group can overhear the report."
 *               - (4) Check sum
 *           - (d) Transmit IGMP Message
 *           - (e) Free     IGMP Message buffer
 *           - (f) Update transmit statistics
 *
 * @param    if_nbr      Interface number to transmit IGMP message.
 *
 * @param    addr_src    Source     IP address (see Note #2).
 *
 * @param    addr_grp    Host group IP address (see Note #2).
 *
 * @param    type        IGMP message type (see Note #1c1) :
 *                       NET_IGMP_MSG_TYPE_QUERY         IGMP Message Query  type
 *                       NET_IGMP_MSG_TYPE_REPORT        IGMP Message Report type
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) IP addresses MUST be in host-order.
 *
 * @note     (3) Assumes network buffer's protocol header size is large enough to accommodate IGMP header
 *               size (see 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1a2').
 *
 * @note     (4) Some buffer controls were previously initialized in NetBuf_Get() when the buffer was
 *               allocated.  These buffer controls do NOT need to be re-initialized but are shown for
 *               completeness.
 *
 * @note     (5) IGMP header Check-Sum MUST be calculated AFTER the entire IGMP header has been
 *               prepared.  In addition, ALL multi-octet words are converted from host-order to
 *               network-order since "the sum of 16-bit integers can be computed in either byte
 *               order" [RFC #1071, Section 2.(B)].
 *
 *               IGMP header Check-Sum field MUST be cleared to '0' BEFORE the IGMP header Check-Sum
 *               is calculated (see RFC #1112, Appendix I, Section 'Checksum').
 *
 *               The IGMP header Check-Sum field is returned in network-order & MUST NOT be re-converted
 *               back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()  Note #3b').
 *
 * @note     (6) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
static void NetIGMP_TxMsg(NET_IF_NBR    if_nbr,
                          NET_IPv4_ADDR addr_src,
                          NET_IPv4_ADDR addr_grp,
                          CPU_INT08U    type,
                          RTOS_ERR      *p_err)
{
  NET_BUF      *p_msg_buf = DEF_NULL;
  NET_BUF_HDR  *pmsg_buf_hdr;
  NET_IGMP_HDR *p_igmp_hdr;
  CPU_INT16U   msg_ix;
  CPU_INT16U   msg_ix_offset;
  CPU_INT08U   igmp_ver;
  CPU_INT08U   igmp_type;
  CPU_INT16U   igmp_chk_sum;

  //                                                               -------------- VALIDATE IGMP MSG TYPE --------------
  switch (type) {
    case NET_IGMP_MSG_TYPE_REPORT:
      break;

    case NET_IGMP_MSG_TYPE_QUERY:                               // See 'net_icmp.c  Note #2c'.
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      goto exit_discard;
  }

  //                                                               ------------------ GET IGMP TX BUF -----------------
  msg_ix = 0;
  NetIGMP_TxIxDataGet(if_nbr, NET_IGMP_HDR_SIZE, &msg_ix);

  p_msg_buf = NetBuf_Get(if_nbr,
                         NET_TRANSACTION_TX,
                         NET_IGMP_MSG_LEN_DATA,
                         msg_ix,
                         &msg_ix_offset,
                         NET_BUF_FLAG_NONE,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  msg_ix += msg_ix_offset;

  //                                                               Init msg buf ctrls.
  pmsg_buf_hdr = &p_msg_buf->Hdr;
  pmsg_buf_hdr->IGMP_MsgIx = (CPU_INT16U)msg_ix;
  pmsg_buf_hdr->IGMP_MsgLen = (CPU_INT16U)NET_IGMP_HDR_SIZE;
  pmsg_buf_hdr->TotLen = (NET_BUF_SIZE)pmsg_buf_hdr->IGMP_MsgLen;
  pmsg_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IGMP;
  pmsg_buf_hdr->ProtocolHdrTypeNetSub = NET_PROTOCOL_TYPE_IGMP;

  //                                                               ----------------- PREPARE IGMP MSG -----------------
  p_igmp_hdr = (NET_IGMP_HDR *)&p_msg_buf->DataPtr[pmsg_buf_hdr->IGMP_MsgIx];

  //                                                               Prepare IGMP ver/type (see Note #1c1).
  igmp_ver = NET_IGMP_HDR_VER;
  igmp_ver <<= NET_IGMP_HDR_VER_SHIFT;

  igmp_type = type;
  igmp_type &= NET_IGMP_HDR_TYPE_MASK;

  p_igmp_hdr->Ver_Type = igmp_ver | igmp_type;

  NET_UTIL_VAL_SET_NET_16(&p_igmp_hdr->Unused, 0x0000u);        // Clr unused octets.

  //                                                               Prepare host grp addr (see Note #1c3).
  NET_UTIL_VAL_COPY_SET_NET_32(&p_igmp_hdr->AddrGrp, &addr_grp);

  //                                                               Prepare IGMP chk sum  (see Note #5).
  NET_UTIL_VAL_SET_NET_16(&p_igmp_hdr->ChkSum, 0x0000u);        // Clr  chk sum (see Note #5).
                                                                // Calc chk sum.
  igmp_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc(p_igmp_hdr,
                                                   NET_IGMP_HDR_SIZE);

  NET_UTIL_VAL_COPY_16(&p_igmp_hdr->ChkSum, &igmp_chk_sum);     // Copy chk sum in net order (see Note #5).

  //                                                               ------------------- TX IGMP MSG --------------------
  NetIPv4_Tx(p_msg_buf,
             addr_src,
             addr_grp,
             NET_IPv4_TOS_DFLT,
             NET_IPv4_TTL_MULTICAST_IGMP,
             NET_IPv4_FLAG_NONE,
             DEF_NULL,
             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               --------- FREE IGMP MSG / UPDATE TX STATS ----------
  NetIGMP_TxPktFree(p_msg_buf);

  NET_CTR_STAT_INC(Net_StatCtrs.IGMP.TxMsgCtr);

  goto exit;

exit_discard:
  NetIGMP_TxPktDiscard(p_msg_buf);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIGMP_TxMsgReport()
 *
 * @brief    (1) Prepare & transmit an IGMP Report :
 *           - (a) Prepare IGMP Report Message :
 *               - (1) Get host group's network interface number
 *               - (2) Get host group's source IP address
 *               - (3) Get host group's        IP address
 *
 *           - (b) Update transmit statistics
 *
 * @param    p_host_grp  Pointer to host group to transmit report.
 *
 *
 * Argument(s) : p_host_grp  Pointer to host group to transmit report.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
static void NetIGMP_TxMsgReport(NET_IGMP_HOST_GRP *p_host_grp,
                                RTOS_ERR          *p_err)
{
  NET_IF_NBR       if_nbr;
  NET_IP_ADDRS_QTY addr_ip_tbl_qty;
  NET_IPv4_ADDR    addr_src;
  NET_IPv4_ADDR    addr_grp;
  CPU_BOOLEAN      addr_avail;

  //                                                               ------------- PREPARE IGMP REPORT MSG --------------
  if_nbr = p_host_grp->IF_Nbr;                                  // Get IGMP host grp IF nbr.
  addr_ip_tbl_qty = 1;
  addr_avail = NetIPv4_GetAddrHostHandler(if_nbr,
                                          &addr_src,
                                          &addr_ip_tbl_qty,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (addr_avail == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_INVALID_ADDR_SRC);
    goto exit;
  }

  addr_grp = p_host_grp->AddrGrp;                               // Get IGMP host grp     addr.

  //                                                               ---------------- TX IGMP REPORT MSG ----------------
  NetIGMP_TxMsg(if_nbr,
                addr_src,
                addr_grp,
                NET_IGMP_MSG_TYPE_REPORT,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------------ UPDATE TX STATS -----------------
  NET_CTR_STAT_INC(Net_StatCtrs.IGMP.TxMsgReportCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIGMP_TxIxDataGet()
 *
 * @brief    Get the offset of a buffer at which the UDP data CAN be written.
 *
 * @param    if_nbr      Network interface number to transmit data.
 *
 * @param    data_len    Length of the IGMP payload.
 *
 * @param    p_ix        Pointer to the current protocol index.
 *******************************************************************************************************/
static void NetIGMP_TxIxDataGet(NET_IF_NBR if_nbr,
                                CPU_INT16U data_len,
                                CPU_INT16U *p_ix)
{
  NET_MTU mtu;

  mtu = NetIF_MTU_GetProtocol(if_nbr, NET_PROTOCOL_TYPE_IGMP, NET_IF_FLAG_NONE);

  NetIPv4_TxIxDataGet(if_nbr,
                      data_len,
                      mtu,
                      p_ix);
}

/****************************************************************************************************//**
 *                                           NetIGMP_TxPktFree()
 *
 * @brief    Free network buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *
 * @note     (1) Although IGMP Transmit initially requests the network buffer for transmit,
 *                   the IGMP layer does NOT maintain a reference to the buffer.
 *
 *                   Also, since the network interface layer frees ALL unreferenced buffers after
 *                   successful transmission, the IGMP layer MUST not free the transmit buffer.
 *******************************************************************************************************/
static void NetIGMP_TxPktFree(NET_BUF *p_buf)
{
  PP_UNUSED_PARAM(p_buf);                                       // Prevent 'variable unused' warning (see Note #1).
}

/****************************************************************************************************//**
 *                                           NetIGMP_TxPktDiscard()
 *
 * @brief    On any IGMP transmit packet error(s), discard packet & buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *******************************************************************************************************/
static void NetIGMP_TxPktDiscard(NET_BUF *p_buf)
{
  NET_CTR *p_ctr;

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  p_ctr = &Net_ErrCtrs.IGMP.TxPktDiscardedCtr;
#else
  p_ctr = DEF_NULL;
#endif
  (void)NetBuf_FreeBuf(p_buf, p_ctr);
}

/****************************************************************************************************//**
 *                                           NetIGMP_HostGrpSrch()
 *
 * @brief    Search IGMP Host Group List for host group with specific address & interface number.
 *
 *       - (1) IGMP host groups are linked to form an IGMP Host Group List.
 *
 *           - (a) In the diagram below, ... :
 *
 *               - (1) The horizontal row represents the list of IGMP host groups.
 *
 *               - (2) 'NetIGMP_HostGrpListHead' points to the head of the IGMP Host Group List.
 *
 *               - (3) IGMP host groups' 'PrevPtr' & 'NextPtr' doubly-link each host group to form the
 *                       IGMP Host Group List.
 *
 *           - (b) (1) For any IGMP Host Group List lookup, all IGMP host groups are searched in order
 *                       to find the host group with the appropriate host group address on the specified
 *                       interface.
 *
 *               - (2) To expedite faster IGMP Host Group List lookup :
 *
 *                   - (A) (1) (a) IGMP host groups are added at;            ...
 *                           - (b) IGMP host groups are searched starting at ...
 *                       - (2) ... the head of the IGMP Host Group List.
 *
 *                   - (B) As IGMP host groups are added into the list, older IGMP host groups migrate
 *                           to the tail of the IGMP Host Group List.   Once an IGMP host group is left,
 *                           it is removed from the IGMP Host Group List.
 *
 *                                   |                                               |
 *                                   |<---------- List of IGMP Host Groups --------->|
 *                                   |                (see Note #1a1)                |
 *
 *                           New IGMP host groups                      Oldest IGMP host group
 *                               inserted at head                        in IGMP Host Group List
 *                               (see Note #1b2A2)                           (see Note #1b2B)
 *
 *                                       |                 NextPtr                 |
 *                                       |             (see Note #1a3)             |
 *                                       v                    |                    v
 *                                                            |
 *              Head of IGMP          -------       -------   v   -------       -------
 *              Host Group List  ---->|     |------>|     |------>|     |------>|     |
 *                                    |     |       |     |       |     |       |     |        Tail of IGMP
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
 * @return   Pointer to IGMP host group with specific IP group address & interface, if found.
 *           Pointer to NULL, otherwise.
 *
 * @note     (2) IP host group address MUST be in host-order.
 *******************************************************************************************************/
static NET_IGMP_HOST_GRP *NetIGMP_HostGrpSrch(NET_IF_NBR    if_nbr,
                                              NET_IPv4_ADDR addr_grp)
{
  NET_IGMP_HOST_GRP *p_host_grp = DEF_NULL;
  CPU_BOOLEAN       found = DEF_NO;

  SLIST_FOR_EACH_ENTRY(NetIGMP_DataPtr->HostGrpListPtr, p_host_grp, NET_IGMP_HOST_GRP, ListNode) {
    found = ((p_host_grp->IF_Nbr == if_nbr)                     // Cmp IF nbr & grp addr.
             && (p_host_grp->AddrGrp == addr_grp)) ? DEF_YES : DEF_NO;

    if (found == DEF_YES) {
      break;
    }
  }

  if (found != DEF_YES) {
    p_host_grp = DEF_NULL;
  }

  return (p_host_grp);
}

/****************************************************************************************************//**
 *                                           NetIGMP_HostGrpAdd()
 *
 * @brief    (1) Add a host group to the IGMP Host Group List :
 *           - (a) Get a     host group from      host group pool
 *           - (b) Configure host group
 *           - (c) Insert    host group into IGMP Host Group List
 *           - (d) Configure interface for multicast address
 *
 * @param    if_nbr      Interface number to add host group.
 *
 * @param    addr_grp    IP address of host group to add.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to host group, if NO error(s).
 *           Pointer to NULL,       otherwise.
 *
 * @note     (2) IP host group address MUST be in host-order.
 *******************************************************************************************************/
static NET_IGMP_HOST_GRP *NetIGMP_HostGrpAdd(NET_IF_NBR    if_nbr,
                                             NET_IPv4_ADDR addr_grp,
                                             RTOS_ERR      *p_err)
{
  NET_IGMP_HOST_GRP *p_host_grp = DEF_NULL;
  NET_PROTOCOL_TYPE addr_protocol_type;
  NET_IPv4_ADDR     addr_grp_net;
  CPU_INT08U        *p_addr_protocol;
  CPU_INT08U        addr_protocol_len;
  RTOS_ERR          local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               ------------------- GET HOST GRP -------------------
  p_host_grp = (NET_IGMP_HOST_GRP *)Mem_DynPoolBlkGet(&NetIGMP_DataPtr->HostGrpPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------------ INIT HOST GRP -------------------
  NetIGMP_HostGrpClr(p_host_grp);
  DEF_BIT_SET(p_host_grp->Flags, NET_IGMP_FLAG_USED);           // Set host grp as used.

#if (NET_STAT_POOL_IGMP_EN == DEF_ENABLED)
  //                                                               ------------ UPDATE HOST GRP POOL STATS ------------
  NetStat_PoolEntryUsedInc(&NetIGMP_DataPtr->HostGrpPoolStat, &local_err);
#endif

  //                                                               ------------------- CFG HOST GRP -------------------
  p_host_grp->AddrGrp = addr_grp;
  p_host_grp->IF_Nbr = if_nbr;
  p_host_grp->RefCtr = 1u;
  //                                                               Set host grp state.
  //                                                               See 'net_igmp.h  IGMP HOST GROUP STATES  Note #2'.
  p_host_grp->State = (addr_grp == NET_IPv4_ADDR_MULTICAST_ALL_HOSTS) ? NET_IGMP_HOST_GRP_STATE_STATIC
                      : NET_IGMP_HOST_GRP_STATE_IDLE;

  //                                                               -------- INSERT HOST GRP INTO HOST GRP LIST --------
  SList_Push(&NetIGMP_DataPtr->HostGrpListPtr, &p_host_grp->ListNode);

  //                                                               ------------ CFG IF FOR MULTICAST ADDR -------------
  addr_grp_net = NET_UTIL_HOST_TO_NET_32(addr_grp);
  addr_protocol_type = NET_PROTOCOL_TYPE_IP_V4;
  p_addr_protocol = (CPU_INT08U *)&addr_grp_net;
  addr_protocol_len = sizeof(addr_grp_net);

  NetIF_AddrMulticastAdd(if_nbr,
                         p_addr_protocol,
                         addr_protocol_len,
                         addr_protocol_type,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_remove;
  }

  PP_UNUSED_PARAM(local_err);

  goto exit;

exit_remove:
  NetIGMP_HostGrpRemove(p_host_grp);
  p_host_grp = DEF_NULL;

exit:
  return (p_host_grp);
}

/****************************************************************************************************//**
 *                                           NetIGMP_HostGrpRemove()
 *
 * @brief    Remove a host group from the IGMP Host Group List :
 *           - (a) Remove host group from IGMP Host Group List
 *           - (b) Free   host group back to   host group pool
 *           - (c) Remove multicast address from interface
 *
 * @param    p_host_grp  Pointer to a host group.
 *******************************************************************************************************/
static void NetIGMP_HostGrpRemove(NET_IGMP_HOST_GRP *p_host_grp)
{
  NET_IF_NBR        if_nbr;
  NET_PROTOCOL_TYPE addr_protocol_type;
  NET_IPv4_ADDR     addr_grp;
  NET_IPv4_ADDR     addr_grp_net;
  CPU_INT08U        *p_addr_protocol;
  CPU_INT08U        addr_protocol_len;
  RTOS_ERR          local_err;

  if_nbr = p_host_grp->IF_Nbr;
  addr_grp = p_host_grp->AddrGrp;

  //                                                               -------- REMOVE HOST GRP FROM HOST GRP LIST --------
  SList_Rem(&NetIGMP_DataPtr->HostGrpListPtr, &p_host_grp->ListNode);

  //                                                               ---------------- FREE HOST GRP TMR -----------------
  if (p_host_grp->TmrPtr != DEF_NULL) {
    NetTmr_Free(p_host_grp->TmrPtr);
    p_host_grp->TmrPtr = DEF_NULL;
  }

  //                                                               ------------------- CLR HOST GRP -------------------
  p_host_grp->State = NET_IGMP_HOST_GRP_STATE_FREE;             // Set host grp as freed/NOT used.
  DEF_BIT_CLR(p_host_grp->Flags, (NET_IGMP_FLAGS)NET_IGMP_FLAG_USED);
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
  NetIGMP_HostGrpClr(p_host_grp);
#endif

  //                                                               ------------------ FREE HOST GRP -------------------
  Mem_DynPoolBlkFree(&NetIGMP_DataPtr->HostGrpPool, p_host_grp, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; )

#if (NET_STAT_POOL_IGMP_EN == DEF_ENABLED)
  //                                                               ------------ UPDATE HOST GRP POOL STATS ------------
  NetStat_PoolEntryUsedDec(&NetIGMP_DataPtr->HostGrpPoolStat, &local_err);
#endif

  //                                                               ---------- REMOVE MULTICAST ADDR FROM IF -----------
  addr_grp_net = NET_UTIL_HOST_TO_NET_32(addr_grp);
  addr_protocol_type = NET_PROTOCOL_TYPE_IP_V4;
  p_addr_protocol = (CPU_INT08U *)&addr_grp_net;
  addr_protocol_len = sizeof(addr_grp_net);

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetIF_AddrMulticastRemove(if_nbr,
                            p_addr_protocol,
                            addr_protocol_len,
                            addr_protocol_type,
                            &local_err);
}

/****************************************************************************************************//**
 *                                       NetIGMP_HostGrpReportDlyTimeout()
 *
 * @brief    Transmit an IGMP report on IGMP Query timeout.
 *
 * @param    p_host_grp_timeout  Pointer to a host group (see Note #1b).
 *
 * @note     (2) This function is a network timer callback function :
 *               - (a) Clear the timer pointer,
 *               - (b) but do NOT re-free the timer.
 *
 * @note     (3) In case of a transmit error :
 *               - (a) Configure a timer to attempt retransmission of the IGMP report, if the error
 *                     is transitory.
 *                     See also 'net_igmp.h  IGMP REPORT DEFINES  Note #2'.
 *               - (b) Revert to 'IDLE' state, if the error is permanent.
 *******************************************************************************************************/
static void NetIGMP_HostGrpReportDlyTimeout(void *p_host_grp_timeout)
{
  NET_IGMP_HOST_GRP *p_host_grp;
  CPU_INT32U        timeout_ms;
  RTOS_ERR          local_err;

  p_host_grp = (NET_IGMP_HOST_GRP *)p_host_grp_timeout;

  p_host_grp->TmrPtr = DEF_NULL;                                // Clear tmr (see Note #2).

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               ------------------ TX IGMP REPORT ------------------
  NetIGMP_TxMsgReport(p_host_grp, &local_err);
  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:                                         // If NO err, ...
      p_host_grp->State = NET_IGMP_HOST_GRP_STATE_IDLE;         // ... set state to 'IDLE'.
      break;

    //                                                             If tx err, ...
    case RTOS_ERR_NET_IF_LINK_DOWN:
      //                                                           ... cfg new tmr (see Note #3a).
      timeout_ms = NET_IGMP_HOST_GRP_REPORT_DLY_RETRY_SEC * 1000;
      p_host_grp->TmrPtr = NetTmr_Get(&NetIGMP_HostGrpReportDlyTimeout,
                                      p_host_grp,
                                      timeout_ms,
                                      NET_TMR_OPT_NONE,
                                      &local_err);

      p_host_grp->State = (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) ? NET_IGMP_HOST_GRP_STATE_DELAYING
                          : NET_IGMP_HOST_GRP_STATE_IDLE;
      break;

    default:                                                    // On all other errs, ...
      p_host_grp->State = NET_IGMP_HOST_GRP_STATE_IDLE;         // ... set state to 'IDLE'.
      break;
  }
}

/****************************************************************************************************//**
 *                                           NetIGMP_HostGrpClr()
 *
 * @brief    Clear IGMP host group controls.
 *
 * @param    p_host_grp  Pointer to an IGMP host group.
 *******************************************************************************************************/
static void NetIGMP_HostGrpClr(NET_IGMP_HOST_GRP *p_host_grp)
{
  p_host_grp->TmrPtr = DEF_NULL;

  p_host_grp->IF_Nbr = NET_IF_NBR_NONE;

  p_host_grp->State = NET_IGMP_HOST_GRP_STATE_FREE;
  p_host_grp->RefCtr = 0u;
  p_host_grp->Flags = NET_IGMP_FLAG_NONE;

  p_host_grp->AddrGrp = NET_IPv4_ADDR_NONE;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IGMP_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
