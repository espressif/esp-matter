/***************************************************************************//**
 * @file
 * @brief Network Connection Management
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

#include  <em_core.h>

#include  <net/include/net_util.h>

#include  "net_conn_priv.h"
#include  <net/include/net_cfg_net.h>
#include  "net_priv.h"
#include  "net_tcp_priv.h"
#include  "net_if_priv.h"

#include  <common/include/lib_mem.h>
#include  <common/include/lib_utils.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_CONN *NetConn_ListSrch(NET_CONN_FAMILY family,
                                  NET_CONN        **p_conn_list,
                                  CPU_INT08U      *p_addr_local);

static NET_CONN *NetConn_ChainSrch(NET_CONN       **p_conn_list,
                                   NET_CONN       *p_conn_chain,
                                   CPU_INT08U     *p_addr_local,
                                   CPU_INT08U     *p_addr_wildcard,
                                   CPU_INT08U     *p_addr_remote,
                                   NET_CONN_STATE *p_conn_state);

static void NetConn_ChainInsert(NET_CONN **p_conn_list,
                                NET_CONN *p_conn_chain);

static void NetConn_ChainUnlink(NET_CONN **p_conn_list,
                                NET_CONN *p_conn_chain);

static void NetConn_Add(NET_CONN **p_conn_list,
                        NET_CONN *p_conn_chain,
                        NET_CONN *p_conn);

static void NetConn_Unlink(NET_CONN *p_conn);

static void NetConn_Close(NET_CONN *p_conn);

static void NetConn_CloseApp(NET_CONN *p_conn);

static void NetConn_CloseTransport(NET_CONN *p_conn);

static void NetConn_CloseAllConnsHandler(NET_IF_NBR          if_nbr,
                                         CPU_INT08U          *p_addr,
                                         NET_CONN_ADDR_LEN   addr_len,
                                         NET_CONN_CLOSE_CODE close_code);

static void NetConn_CloseAllConnsCloseConn(NET_CONN            *p_conn,
                                           NET_IF_NBR          if_nbr,
                                           CPU_INT08U          *p_addr,
                                           NET_CONN_ADDR_LEN   addr_len,
                                           NET_CONN_CLOSE_CODE close_code);

static void NetConn_FreeHandler(NET_CONN *p_conn);

static void NetConn_Clr(NET_CONN *p_conn);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

NET_CONN *NetConn_Tbl;

#if (NET_STAT_POOL_CONN_EN == DEF_ENABLED)
NET_STAT_POOL NetConn_PoolStat;
#endif

//                                                                 Array of ptrs to net conn lists.
NET_CONN *NetConn_ConnListHead[NET_CONN_PROTOCOL_NBR_MAX];

NET_CONN *NetConn_ConnListChainPtr;                                 // Ptr to cur  conn list chain to update.
NET_CONN *NetConn_ConnListConnPtr;                                  // Ptr to cur  conn list conn  to update.
NET_CONN *NetConn_ConnListNextChainPtr;                             // Ptr to next conn list chain to update.
NET_CONN *NetConn_ConnListNextConnPtr;                              // Ptr to next conn list conn  to update.

CPU_INT16U NetConn_AccessedTh_nbr;                                  // Nbr successful srch's to promote net conns.

//                                                                 Conn addr wildcard cfg.
CPU_BOOLEAN NetConn_AddrWildCardAvailv4;
#ifdef  NET_IPv4_MODULE_EN
CPU_INT08U NetConn_AddrWildCardv4[NET_CONN_ADDR_LEN_MAX];
#endif

CPU_BOOLEAN NetConn_AddrWildCardAvailv6;
#ifdef  NET_IPv6_MODULE_EN
CPU_INT08U NetConn_AddrWildCardv6[NET_CONN_ADDR_LEN_MAX];
#endif

static NET_CONN *NetConn_PoolPtr;                             // Ptr to pool of free net conns.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetConn_CfgAccessedTh()
 *
 * @brief    Configure network connection access promotion threshold.
 *
 * @param    nbr_access  Desired number of accesses before network connection is promoted.
 *
 * @return   DEF_OK,   network connection access promotion threshold configured.
 *           DEF_FAIL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] 'NetConn_AccessedTh_nbr' MUST ALWAYS be accessed exclusively in
 *               critical sections.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetConn_CfgAccessedTh(CPU_INT16U nbr_access)
{
  CORE_DECLARE_IRQ_STATE;

#if (NET_CONN_ACCESSED_TH_MIN > DEF_INT_16U_MIN_VAL)
  if (nbr_access < NET_CONN_ACCESSED_TH_MIN) {
    return (DEF_FAIL);
  }
#endif
#if (NET_CONN_ACCESSED_TH_MAX < DEF_INT_16U_MAX_VAL)
  if (nbr_access > NET_CONN_ACCESSED_TH_MAX) {
    return (DEF_FAIL);
  }
#endif

  CORE_ENTER_ATOMIC();
  NetConn_AccessedTh_nbr = nbr_access;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           NetConn_PoolStatGet()
 *
 * @brief    Get network connection statistics pool.
 *
 * @return   Network connection statistics pool, if NO error(s).
 *           NULL               statistics pool, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] 'NetConn_PoolStat' MUST ALWAYS be accessed exclusively in critical sections.
 * @endinternal
 *******************************************************************************************************/
NET_STAT_POOL NetConn_PoolStatGet(void)
{
  NET_STAT_POOL stat_pool;
#if (NET_STAT_POOL_CONN_EN == DEF_ENABLED)
  CORE_DECLARE_IRQ_STATE;
#endif

  NetStat_PoolClr(&stat_pool);                                  // Init rtn pool stat for err.

#if (NET_STAT_POOL_CONN_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  stat_pool = NetConn_PoolStat;
  CORE_EXIT_ATOMIC();
#endif

  return (stat_pool);
}

/****************************************************************************************************//**
 *                                       NetConn_PoolStatResetMaxUsed()
 *
 * @brief    Reset network connection statistics pool's maximum number of entries used.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               @n
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
void NetConn_PoolStatResetMaxUsed(void)
{
#if (NET_STAT_POOL_CONN_EN == DEF_ENABLED)
  //                                                               Acquire net lock.
  Net_GlobalLockAcquire((void *)NetConn_PoolStatResetMaxUsed);

  NetStat_PoolResetUsedMax(&NetConn_PoolStat);                  // Reset net conn stat pool.

  Net_GlobalLockRelease();                                      // Release net lock.
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetConn_Init()
 *
 * @brief    (1) Initialize Network Connection Management Module :
 *               - (a) Initialize network connection pool
 *               - (b) Initialize network connection table
 *               - (c) Initialize network connection lists
 *               - (d) Initialize network connection wildcard address(s)
 *
 * @note     (2) Network connection pool MUST be initialized PRIOR to initializing the pool with
 *               pointers to network connections.
 *
 * @note     (3) Network connection  addresses maintained in network-order.  Therefore, network
 *               connection wildcard address(s) MUST be configured in network-order.
 *******************************************************************************************************/
void NetConn_Init(MEM_SEG  *p_mem_seg,
                  RTOS_ERR *p_err)
{
  NET_CONN_ADDR_LEN addr_len_ip;
  NET_CONN_ADDR_LEN addr_len_wildcard;
  NET_CONN          *p_conn;
  NET_CONN          **p_conn_list;
  CPU_INT16U        i;
  NET_CONN_LIST_QTY j;

  //                                                               ----- GET MEMORY SEGMENT FOR CONNECTION TABLE ------
  NetConn_Tbl = (NET_CONN *)Mem_SegAlloc("Net Connection table",
                                         p_mem_seg,
                                         sizeof(NET_CONN) * NET_CONN_NBR_CONN,
                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------------- INIT NET CONN POOL/STATS -------------
  NetConn_PoolPtr = DEF_NULL;                                   // Init-clr net conn pool (see Note #2).

#if (NET_STAT_POOL_CONN_EN == DEF_ENABLED)
  NetStat_PoolInit(&NetConn_PoolStat,
                   NET_CONN_NBR_CONN);
#endif

  //                                                               ---------------- INIT NET CONN TBL -----------------
  for (i = 0; i < NET_CONN_NBR_CONN; i++) {
    p_conn = &NetConn_Tbl[i];
    p_conn->ID = (NET_CONN_ID)i;

    p_conn->Flags = NET_CONN_FLAG_NONE;                         // Init each net conn as NOT used.

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    NetConn_Clr(p_conn);
#endif

    p_conn->NextConnPtr = NetConn_PoolPtr;                      // Free each net conn to net conn pool (see Note #2).
    NetConn_PoolPtr = p_conn;
  }

  //                                                               --------------- INIT NET CONN LISTS ----------------
  //                                                               Init net conn lists.
  p_conn_list = &NetConn_ConnListHead[0];
  for (j = 0u; j < NET_CONN_PROTOCOL_NBR_MAX; j++) {
    *p_conn_list = DEF_NULL;
    p_conn_list++;
  }

  //                                                               Init net conn list ptrs.
  NetConn_ConnListChainPtr = DEF_NULL;
  NetConn_ConnListConnPtr = DEF_NULL;
  NetConn_ConnListNextChainPtr = DEF_NULL;
  NetConn_ConnListNextConnPtr = DEF_NULL;

  //                                                               ----------- INIT NET CONN WILDCARD ADDRS -----------
  //                                                               See Note #3.
#ifdef  NET_IPv4_MODULE_EN
  addr_len_wildcard = sizeof(NetConn_AddrWildCardv4);
  Mem_Clr(&NetConn_AddrWildCardv4[0],
          addr_len_wildcard);

  addr_len_ip = sizeof(NET_CONN_ADDR_IP_V4_WILDCARD);
  if (addr_len_ip <= addr_len_wildcard) {
    NET_UTIL_VAL_SET_NET_32(&NetConn_AddrWildCardv4[0], NET_CONN_ADDR_IP_V4_WILDCARD);
    NetConn_AddrWildCardAvailv4 = DEF_YES;
  } else {
    NetConn_AddrWildCardAvailv4 = DEF_NO;
  }

#else
  NetConn_AddrWildCardAvailv4 = DEF_NO;
#endif

#ifdef  NET_IPv6_MODULE_EN
  addr_len_wildcard = sizeof(NetConn_AddrWildCardv6);
  Mem_Clr((void *)&NetConn_AddrWildCardv6[0],
          (CPU_SIZE_T) addr_len_wildcard);

  addr_len_ip = sizeof(NetIPv6_AddrWildcard);
  if (addr_len_ip <= addr_len_wildcard) {
    Mem_Copy(&NetConn_AddrWildCardv6[0], &NetIPv6_AddrWildcard, addr_len_ip);
    NetConn_AddrWildCardAvailv6 = DEF_YES;
  } else {
    NetConn_AddrWildCardAvailv6 = DEF_NO;
  }

#else
  NetConn_AddrWildCardAvailv6 = DEF_NO;
#endif
}

/****************************************************************************************************//**
 *                                               NetConn_Get()
 *
 * @brief    (1) Allocate & initialize a network connection :
 *               - (a) Get      a network connection
 *               - (b) Validate   network connection
 *               - (c) Initialize network connection
 *               - (d) Update     network connection pool statistics
 *               - (e) Return network connection handle identifier
 *                       OR
 *                       Null identifier & error code, on failure
 *           - (2) The network connection pool is implemented as a stack :
 *               - (a) 'NetConn_PoolPtr' points to the head of the network connection pool.
 *               - (b) Connections' 'NextConnPtr's link each connection to form the connection pool stack.
 *               - (c) Connections are inserted & removed at the head of        the connection pool stack.
 *                     @verbatim
 *                                   Connections are
 *                                   inserted & removed
 *                                       at the head
 *                                       (see Note #2c)
 *
 *                                           |               NextConnPtr
 *                                           |             (see Note #2b)
 *                                           v                    |
 *                                                                |
 *                                        -------       -------   v   -------       -------
 *                   Connection Pool ---->|     |------>|     |------>|     |------>|     |
 *                       Pointer          |     |       |     |       |     |       |     |
 *                                        |     |       |     |       |     |       |     |
 *                    (see Note #2a)      -------       -------       -------       -------
 *
 *                                        |                                               |
 *                                        |<--------- Pool of Free Connections ---------->|
 *                                        |                (see Note #2)                  |
 *                     @verbatim
 *
 * @param    family          Network connection family type.
 *
 * @param    protocol_ix     Network connection protocol index.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Connection handle identifier, if NO error(s).
 *           NET_CONN_ID_NONE,             otherwise.
 *
 * @note     (3) Network connection pool is accessed by 'NetConn_PoolPtr' during execution of
 *               -   NetConn_Init()
 *               -   NetConn_Get()
 *               -   NetConn_FreeHandler()
 *               Since the primary tasks of the network protocol suite are prevented from running
 *               concurrently (see 'net.h  Note #3'), it is NOT necessary to protect the shared
 *               resources of the network connection pool since no asynchronous access from other
 *               network tasks is possible.
 *******************************************************************************************************/
NET_CONN_ID NetConn_Get(NET_CONN_FAMILY      family,
                        NET_CONN_PROTOCOL_IX protocol_ix,
                        RTOS_ERR             *p_err)
{
  NET_CONN    *p_conn = DEF_NULL;
  NET_CONN_ID conn_id = NET_CONN_ID_NONE;
  RTOS_ERR    local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  //                                                               -------------- VALIDATE NET CONN ARGS --------------
  switch (family) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_CONN_FAMILY_IP_V4_SOCK:
#endif
#ifdef  NET_IPv6_MODULE_EN
    case NET_CONN_FAMILY_IP_V6_SOCK:
#endif
    switch (protocol_ix) {
#ifdef  NET_IPv4_MODULE_EN
      case NET_CONN_PROTOCOL_IX_IP_V4_UDP:
#ifdef  NET_TCP_MODULE_EN
      case NET_CONN_PROTOCOL_IX_IP_V4_TCP:
#endif
#endif

#ifdef  NET_IPv6_MODULE_EN
      case NET_CONN_PROTOCOL_IX_IP_V6_UDP:
#ifdef  NET_TCP_MODULE_EN
      case NET_CONN_PROTOCOL_IX_IP_V6_TCP:
#endif
#endif
      break;

      default:
        NET_CTR_ERR_INC(Net_ErrCtrs.Conn.InvProtocolIxCtr);
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_CONN_ID_NONE);
    }
    break;

    case NET_CONN_FAMILY_NONE:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.Conn.InvFamilyCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_CONN_ID_NONE);
  }

  //                                                               ------------------- GET NET CONN -------------------
  if (NetConn_PoolPtr != DEF_NULL) {                            // If net conn pool NOT empty, get net conn from pool.
    p_conn = NetConn_PoolPtr;
    NetConn_PoolPtr = p_conn->NextConnPtr;
  } else {                                                      // If none avail, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.Conn.NoneAvailCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_POOL_EMPTY);
    goto exit;
  }

  //                                                               ------------------ INIT NET CONN -------------------
  NetConn_Clr(p_conn);
  DEF_BIT_SET(p_conn->Flags, NET_CONN_FLAG_USED);               // Set net conn as used.
  p_conn->Family = family;
  p_conn->ProtocolIx = protocol_ix;

  switch (protocol_ix) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_CONN_PROTOCOL_IX_IP_V4_UDP:
#ifdef  NET_TCP_MODULE_EN
    case NET_CONN_PROTOCOL_IX_IP_V4_TCP:
#endif
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_CONN_PROTOCOL_IX_IP_V6_UDP:
#ifdef  NET_TCP_MODULE_EN
    case NET_CONN_PROTOCOL_IX_IP_V6_TCP:
#endif
      DEF_BIT_SET(p_conn->TxIPv6Flags, NET_IPv6_FLAG);
      break;
#endif

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.Conn.InvProtocolIxCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_CONN_ID_NONE);
  }

#if (NET_STAT_POOL_CONN_EN == DEF_ENABLED)
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               ------------ UPDATE NET CONN POOL STATS ------------
  NetStat_PoolEntryUsedInc(&NetConn_PoolStat, &local_err);
#endif

  PP_UNUSED_PARAM(local_err);

  //                                                               ----------------- RTN NET CONN ID ------------------
  conn_id = p_conn->ID;

exit:
  return (conn_id);
}

/****************************************************************************************************//**
 *                                               NetConn_Copy()
 *
 * @brief    (1) Copy/clone a network connection :
 *               - (a) IP transmit parameters :
 *                   - (1) TOS
 *                   - (2) TTL
 *                   - (3) Flags
 *
 * @param    conn_id_dest    Handle identifier of destination network connection to copy configured
 *                           parameters to.
 *
 *
 * Argument(s) : conn_id_dest    Handle identifier of destination network connection to copy configured
 *                                   parameters to.
 *
 *               conn_id_src     Handle identifier of source      network connection to copy configured
 *                                   parameters from.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) The 'NET_CONN_CFG_FAMILY' pre-processor 'else'-conditional code will never be compiled/linked
 *                   since 'net_conn.h' ensures that the family type configuration constant (NET_CONN_CFG_FAMILY)
 *                   is configured with an appropriate family type value (see 'net_conn.h  CONFIGURATION ERRORS').
 *                   The 'else'-conditional code is included for completeness & as an extra precaution in case
 *                   'net_conn.h' is incorrectly modified.
 *******************************************************************************************************/
void NetConn_Copy(NET_CONN_ID conn_id_dest,
                  NET_CONN_ID conn_id_src)
{
  NET_CONN *p_conn_dest;
  NET_CONN *p_conn_src;

  //                                                               -------------- COPY NET CONN'S PARAMS --------------
  p_conn_dest = &NetConn_Tbl[conn_id_dest];
  p_conn_src = &NetConn_Tbl[conn_id_src];

#ifdef  NET_IPv4_MODULE_EN
  p_conn_dest->TxIPv4Flags = p_conn_src->TxIPv4Flags;
  p_conn_dest->TxIPv4TOS = p_conn_src->TxIPv4TOS;
  p_conn_dest->TxIPv4TTL = p_conn_src->TxIPv4TTL;
#ifdef  NET_MCAST_TX_MODULE_EN
  p_conn_dest->TxIPv4TTL_Multicast = p_conn_src->TxIPv4TTL_Multicast;
#endif
#endif

#ifdef  NET_IPv6_MODULE_EN
  p_conn_dest->TxIPv6TrafficClass = p_conn_src->TxIPv6TrafficClass;
  p_conn_dest->TxIPv6FlowLabel = p_conn_src->TxIPv6FlowLabel;
  p_conn_dest->TxIPv6HopLim = p_conn_src->TxIPv6HopLim;
  p_conn_dest->TxIPv6Flags = p_conn_src->TxIPv6Flags;
#ifdef  NET_MCAST_TX_MODULE_EN
  p_conn_dest->TxIPv6HopLimMulticast = p_conn_src->TxIPv6HopLimMulticast;
#endif
#endif

#if (!defined(NET_IPv4_MODULE_EN) \
  && !defined(NET_IPv6_MODULE_EN))                                 // See Note #2.
  PP_UNUSED_PARAM(p_conn_dest);                                    // Prevent possible 'variable unused' warnings.
  PP_UNUSED_PARAM(p_conn_src);
#endif
}

/****************************************************************************************************//**
 *                                           NetConn_CloseFromApp()
 *
 * @brief    (1) Close a network connection from application layer :
 *               - (a) Close transport connection, if requested                            See Note #3c
 *               - (b) Clear network   connection's reference to application connection    See Note #3c
 *               - (c) Free  network   connection, if necessary
 *
 * @param    conn_id                 Handle identifier of network connection to close.
 *
 *
 * Argument(s) : conn_id                 Handle identifier of network connection to close.
 *
 *               close_conn_transport    Indicate whether to close transport connection :
 *
 *                                           DEF_YES                    Close transport connection.
 *                                           DEF_NO              Do NOT close transport connection.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) #### To prevent closing a network connection already closed via previous network
 *                   connection close, NetConn_CloseFromApp() checks if the network connection is used
 *                   BEFORE closing the network connection.
 *
 *                   This prevention is only best-effort since any invalid duplicate network connection
 *                   closes MAY be asynchronous to potentially valid network connection gets.  Thus the
 *                   invalid network connection close(s) MAY corrupt the network connection's valid
 *                   operation(s).
 *
 *                   However, since the primary tasks of the network protocol suite are prevented from
 *                   running concurrently (see 'net.h  Note #3'), it is NOT necessary to protect network
 *                   connection resources from possible corruption since no asynchronous access from
 *                   other network tasks is possible.
 *
 *                   Network connection(s) MAY already be closed AFTER other network connection
 *                   close operations & MUST be validated as used BEFORE any subsequent network
 *                   connection close operation(s).
 *
 *               (3) Network connections are considered connected if any of the following network
 *                   connections are valid :
 *
 *                   (1) Application layer connection
 *                   (2) Transport   layer connection
 *
 *                           (A) Network connections which ONLY reference application layer clone
 *                               connection(s) are NOT considered connected since the actual non-
 *                               cloned application connection MAY or MAY NOT reference the cloned
 *                               network connection.
 *
 *                   Since NetConn_CloseFromApp() actively closes the application layer connection,
 *                   network connections need only validate the remaining transport layer connection
 *                   as connected.
 *
 *                   Since network connection(s) connection validation determines, in part, when to
 *                   close the network connection (see Note #3a), & since NetConn_CloseFromTransport()
 *                   may indirectly call NetConn_CloseFromApp(); clearing the network connection's
 *                   application layer connection handle identifier MUST follow the closing of the
 *                   transport   layer connection to prevent re-closing the network connection.
 *******************************************************************************************************/
void NetConn_CloseFromApp(NET_CONN_ID conn_id,
                          CPU_BOOLEAN close_conn_transport)
{
  CPU_BOOLEAN conn_connd;
  CPU_BOOLEAN conn_used;
  CPU_BOOLEAN conn_free;
  NET_CONN    *p_conn;

  //                                                               -------------- VALIDATE NET CONN ---------------
  if (conn_id == NET_CONN_ID_NONE) {
    return;
  }

  p_conn = &NetConn_Tbl[conn_id];

  //                                                               ------------- CLOSE TRANSPORT CONN -------------
  if (close_conn_transport == DEF_YES) {
    NetConn_CloseTransport(p_conn);                                 // Close transport conn, if req'd.
    conn_used = DEF_BIT_IS_SET(p_conn->Flags, NET_CONN_FLAG_USED);
    conn_free = (conn_used == DEF_YES) ? DEF_YES : DEF_NO;          // ... since app & transport conns closed.
  } else {                                                          // Else chk net conn conn'd (see Note #3b).
    conn_connd = (p_conn->ID_Transport != NET_CONN_ID_NONE) ? DEF_YES : DEF_NO;
    conn_free = (conn_connd != DEF_YES) ? DEF_YES : DEF_NO;         // Free net conn, if NOT conn'd.
  }

  //                                                               ---------------- CLOSE APP CONN ----------------
  NetConn_ID_AppSet(conn_id, NET_CONN_ID_NONE);                     // Clr net conn's app conn id (see Note #3c).

  //                                                               ---------------- FREE NET CONN -----------------
  if (conn_free == DEF_YES) {
    NetConn_FreeHandler(p_conn);                                    // Free net conn, if req'd.
  }
}

/****************************************************************************************************//**
 *                                       NetConn_CloseFromTransport()
 *
 * @brief    (1) Close a network connection from transport layer :
 *               - (a) Close application connection, if requested                          See Note #3c
 *               - (b) Clear network     connection's reference to transport connection    See Note #3c
 *               - (c) Free  network     connection, if necessary
 *
 * @param    conn_id         Handle identifier of network connection to close.
 *
 *
 * Argument(s) : conn_id             Handle identifier of network connection to close.
 *
 *               close_conn_app      Indicate whether to close application connection :
 *
 *                                       DEF_YES                        Close application connection.
 *                                       DEF_NO                  Do NOT close application connection.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) #### To prevent closing a network connection already closed via previous network
 *                   connection close, NetConn_CloseFromTransport() checks if the network connection
 *                   is used BEFORE closing the network connection.
 *
 *                   This prevention is only best-effort since any invalid duplicate network connection
 *                   closes MAY be asynchronous to potentially valid network connection gets.  Thus the
 *                   invalid network connection close(s) MAY corrupt the network connection's valid
 *                   operation(s).
 *
 *                   However, since the primary tasks of the network protocol suite are prevented from
 *                   running concurrently (see 'net.h  Note #3'), it is NOT necessary to protect network
 *                   connection resources from possible corruption since no asynchronous access from
 *                   other network tasks is possible.
 *
 *                   (a) Network connection(s) MAY already be closed AFTER other network connection
 *                       close operations & MUST be validated as used BEFORE any subsequent network
 *                       connection close operation(s).
 *
 *               (3) (a) Network connections are considered connected if any of the following network
 *                       connections are valid :
 *
 *                       (1) Application layer connection
 *                       (2) Transport   layer connection
 *
 *                               (A) Network connections which ONLY reference application layer clone
 *                                   connection(s) are NOT considered connected since the actual non-
 *                                   cloned application connection MAY or MAY NOT reference the cloned
 *                                   network connection.
 *
 *                   (b) Since NetConn_CloseFromTransport() actively closes the transport layer connection,
 *                       network connections need only validate the remaining application layer connection
 *                       as connected.
 *
 *                   (c) Since network connection(s) connection validation determines, in part, when to
 *                       close the network connection (see Note #3a), & since NetConn_CloseFromApp() may
 *                       indirectly call NetConn_CloseFromTransport(); clearing the network connection's
 *                       transport   layer connection handle identifier MUST follow the closing of the
 *                       application layer connection to prevent re-closing the network connection.
 *******************************************************************************************************/
void NetConn_CloseFromTransport(NET_CONN_ID conn_id,
                                CPU_BOOLEAN close_conn_app)
{
  CPU_BOOLEAN conn_used;
  CPU_BOOLEAN conn_connd;
  CPU_BOOLEAN conn_free;
  NET_CONN    *p_conn;

  //                                                               -------------- VALIDATE NET CONN ---------------
  if (conn_id == NET_CONN_ID_NONE) {
    return;
  }

  p_conn = &NetConn_Tbl[conn_id];

  //                                                               ---------------- CLOSE APP CONN ----------------
  if (close_conn_app == DEF_YES) {
    NetConn_CloseApp(p_conn);                                       // Close app conn, if req'd.
    conn_used = DEF_BIT_IS_SET(p_conn->Flags, NET_CONN_FLAG_USED);
    conn_free = (conn_used == DEF_YES) ? DEF_YES : DEF_NO;          // ... since app & transport conns closed.
  } else {                                                          // Else chk net conn conn'd (see Note #3b).
    conn_connd = (p_conn->ID_App != NET_CONN_ID_NONE) ? DEF_YES : DEF_NO;
    conn_free = (conn_connd != DEF_YES) ? DEF_YES : DEF_NO;         // Free net conn, if NOT conn'd.
  }

  //                                                               ------------- CLOSE TRANSPORT CONN -------------
  NetConn_ID_TransportSet(conn_id, NET_CONN_ID_NONE);               // Clr net conn's transport conn id (see Note #3c).

  //                                                               ---------------- FREE NET CONN -----------------
  if (conn_free == DEF_YES) {
    NetConn_FreeHandler(p_conn);                                    // Free net conn, if req'd.
  }
}

/****************************************************************************************************//**
 *                                           NetConn_CloseAllConns()
 *
 * @brief    Close ALL network connections.
 *
 * @note     (1) Certain circumstances may require that :
 *               - (a) ALL network protocol suite connections close; ...
 *               - (b) All pending network &/or application connection function(s) SHOULD :
 *                   - (1) Abort, immediately if possible; ...
 *                   - (2) Return appropriate closed error code(s).
 *
 * @note     (2) NetConn_CloseAllConns() is called by network protocol suite function(s) & MUST
 *               be called with the global network lock already acquired.
 *******************************************************************************************************/
void NetConn_CloseAllConns(void)
{
  //                                                               Close ALL net conns.
  NetConn_CloseAllConnsHandler(NET_IF_NBR_NONE,
                               DEF_NULL,
                               0u,
                               NET_CONN_CLOSE_ALL);
}

/****************************************************************************************************//**
 *                                       NetConn_CloseAllConnsByIF()
 *
 * @brief    Close ALL network connections for a specific interface.
 *
 * @param    if_nbr  Interface number to close connection :
 *                   Interface number        Close ALL connections on interface.
 *                   NET_IF_NBR_WILDCARD,    Close ALL connections with a wildcard
 *                   address.
 *
 * @note     (1) Certain circumstances may require that :
 *               - (a) ALL network protocol suite connections for a specific interface close; ...
 *               - (b) All pending network &/or application connection function(s) SHOULD :
 *               - (1) Abort, immediately if possible; ...
 *               - (2) Return appropriate closed error code(s).
 *
 * @note     (2) NetConn_CloseAllConnsByIF() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
void NetConn_CloseAllConnsByIF(NET_IF_NBR if_nbr)
{
  //                                                               Close ALL net conns for specific IF.
  NetConn_CloseAllConnsHandler(if_nbr,
                               DEF_NULL,
                               0u,
                               NET_CONN_CLOSE_BY_IF);
}

/****************************************************************************************************//**
 *                                       NetConn_CloseAllConnsByAddr()
 *
 * @brief    Close ALL network connections for a specific address.
 *
 * @param    p_addr      Pointer to protocol address to close connection (see Note #4).
 *
 * @param    addr_len    Length  of protocol address (in octets).
 *
 * @note     (1) (a) Certain circumstances may require that :
 *               - (1) ALL network protocol suite connections for a specific local address close; ...
 *               - (2) All pending network &/or application connection function(s) SHOULD :
 *                   - (A) Abort, immediately if possible; ...
 *                   - (B) Return appropriate closed error code(s).
 *           - (b) The following lists example(s) when to close all network connections for a given
 *                 local address :
 *               - (1) RFC #2131, Section 4.4.5 states that "if the [DHCP] client is given a new
 *                     network address, it MUST NOT continue using the previous network address
 *                     and SHOULD notify the local users of the problem".
 *                     Therefore, ALL network connections based on a previously configured local
 *                     address MUST be closed.
 *
 * @note     (2) NetConn_CloseAllConnsByAddr() is called by application function(s) & ... :
 *               - (a) MUST NOT be called with the global network lock already acquired; ...
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock (see 'net.h  Note #3').
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 *               See also 'NetConn_CloseAllConnsByAddrHandler()  Note #2'.
 *
 * @note     (3) NetConn_CloseAllConnsByAddr() blocked until network initialization completes.
 *               - (a) However, since all network connections are closed when network initialization
 *                     completes; NO error is returned.
 *
 * @note     (4) Protocol address MUST be in network-order.
 *******************************************************************************************************/
void NetConn_CloseAllConnsByAddr(CPU_INT08U        *p_addr,
                                 NET_CONN_ADDR_LEN addr_len)
{
  //                                                               Acquire net lock (see Note #2b).
  Net_GlobalLockAcquire((void *)NetConn_CloseAllConnsByAddr);

  NetConn_CloseAllConnsByAddrHandler(p_addr, addr_len);         // Close ALL net conns for specific addr.

  Net_GlobalLockRelease();                                      // Release net lock.
}

/****************************************************************************************************//**
 *                                   NetConn_CloseAllConnsByAddrHandler()
 *
 * @brief    Close ALL network connections for a specific address.
 *
 * @param    p_addr      Pointer to protocol address to close connection (see Note #3).
 *
 * @param    addr_len    Length  of protocol address (in octets).
 *
 * @note     (1) (a) Certain circumstances may require that :
 *               - (1) ALL network protocol suite connections for a specific local address close; ...
 *               - (2) All pending network &/or application connection function(s) SHOULD :
 *                   - (A) Abort, immediately if possible; ...
 *                   - (B) Return appropriate closed error code(s).
 *           - (b) The following lists example(s) when to close all network connections for a given
 *                 local address :
 *               - (1) RFC #2131, Section 4.4.5 states that "if the [DHCP] client is given a new
 *                     network address, it MUST NOT continue using the previous network address
 *                     and SHOULD notify the local users of the problem".
 *                     Therefore, ALL network connections based on a previously configured local
 *                     address MUST be closed.
 *
 * @note     (2) NetConn_CloseAllConnsByAddrHandler() is called by network protocol suite function(s)
 *               & MUST be called with the global network lock already acquired.
 *
 *               See also 'NetConn_CloseAllConnsByAddr()  Note #2'.
 *
 * @note         (3) Protocol address MUST be in network-order.
 *******************************************************************************************************/
void NetConn_CloseAllConnsByAddrHandler(CPU_INT08U        *p_addr,
                                        NET_CONN_ADDR_LEN addr_len)
{
  //                                                               Close ALL net conns for specific addr.
  NetConn_CloseAllConnsHandler(NET_IF_NBR_NONE,
                               p_addr,
                               addr_len,
                               NET_CONN_CLOSE_BY_ADDR);
}

/****************************************************************************************************//**
 *                                           NetConn_IF_NbrGet()
 *
 * @brief    Get network connection's interface number.
 *
 * @param    conn_id     Handle identifier of network connection to get interface number.
 *
 * @return   Network connection's interface number, if NO error(s).
 *           NET_IF_NBR_NONE, otherwise.
 *******************************************************************************************************/
NET_IF_NBR NetConn_IF_NbrGet(NET_CONN_ID conn_id)
{
  NET_CONN   *p_conn = DEF_NULL;
  NET_IF_NBR if_nbr = NET_IF_NBR_NONE;

  //                                                               ---------------- VALIDATE NET CONN -----------------
  if (conn_id == NET_CONN_ID_NONE) {
    goto exit;
  }

  //                                                               -------------- GET NET CONN'S IF NBR ---------------
  p_conn = &NetConn_Tbl[conn_id];
  if_nbr = p_conn->IF_Nbr;

exit:
  return (if_nbr);
}

/****************************************************************************************************//**
 *                                           NetConn_ID_AppGet()
 *
 * @brief    Get network connection's application layer handle identifier.
 *
 * @param    conn_id     Handle identifier of network connection to get application layer handle identifier.
 *
 * @return   Network connection's application layer handle identifier, if NO error(s).
 *           NET_CONN_ID_NONE, otherwise.
 *******************************************************************************************************/
NET_CONN_ID NetConn_ID_AppGet(NET_CONN_ID conn_id)
{
  NET_CONN    *p_conn = DEF_NULL;
  NET_CONN_ID conn_id_app = NET_CONN_ID_NONE;

  //                                                               ---------------- VALIDATE NET CONN -----------------
  if (conn_id == NET_CONN_ID_NONE) {
    conn_id_app = NET_CONN_ID_NONE;
    goto exit;
  }

  //                                                               -------------- GET NET CONN'S APP ID ---------------
  p_conn = &NetConn_Tbl[conn_id];
  conn_id_app = p_conn->ID_App;

exit:
  return (conn_id_app);
}

/****************************************************************************************************//**
 *                                           NetConn_ID_AppSet()
 *
 * @brief    Set network connection's application layer handle identifier.
 *
 * @param    conn_id         Handle identifier of network connection to set application layer handle
 *                           identifier.
 *
 * @param    conn_id_app     Connection's application layer handle identifier.
 *******************************************************************************************************/
void NetConn_ID_AppSet(NET_CONN_ID conn_id,
                       NET_CONN_ID conn_id_app)
{
  NET_CONN *p_conn = DEF_NULL;

  //                                                               -------------- SET NET CONN'S APP ID ---------------
  p_conn = &NetConn_Tbl[conn_id];
  p_conn->ID_App = conn_id_app;
}

/****************************************************************************************************//**
 *                                           NetConn_ID_AppCloneGet()
 *
 * @brief    Get network connection's application layer clone handle identifier.
 *
 * @param    conn_id     Handle identifier of network connection to get application layer clone handle
 *                       identifier.
 *
 * @return   Network connection's application layer clone handle identifier, if NO error(s).
 *           NET_CONN_ID_NONE, otherwise.
 *******************************************************************************************************/
NET_CONN_ID NetConn_ID_AppCloneGet(NET_CONN_ID conn_id)
{
  NET_CONN    *p_conn = DEF_NULL;
  NET_CONN_ID conn_id_app_clone = NET_CONN_ID_NONE;

  //                                                               ---------------- VALIDATE NET CONN -----------------
  if (conn_id == NET_CONN_ID_NONE) {
    goto exit;
  }

  //                                                               ----------- GET NET CONN'S APP CLONE ID ------------
  p_conn = &NetConn_Tbl[conn_id];
  conn_id_app_clone = p_conn->ID_AppClone;

exit:
  return (conn_id_app_clone);
}

/****************************************************************************************************//**
 *                                           NetConn_ID_AppCloneSet()
 *
 * @brief    Set network connection's application layer clone handle identifier.
 *
 * @param    conn_id         Handle identifier of network connection to set application layer clone
 *                           handle identifier.
 *
 * @param    conn_id_app     Connection's application layer handle identifier.
 *******************************************************************************************************/
void NetConn_ID_AppCloneSet(NET_CONN_ID conn_id,
                            NET_CONN_ID conn_id_app)
{
  NET_CONN *p_conn;

  //                                                               ----------- SET NET CONN'S APP CLONE ID ------------
  p_conn = &NetConn_Tbl[conn_id];
  p_conn->ID_AppClone = conn_id_app;
}

/****************************************************************************************************//**
 *                                           NetConn_ID_TransportGet()
 *
 * @brief    Get network connection's transport layer handle identifier.
 *
 * @param    conn_id     Handle identifier of network connection to get transport layer handle identifier.
 *
 * @return   Network connection's transport layer handle identifier, if NO error(s).
 *           NET_CONN_ID_NONE,                                       otherwise.
 *******************************************************************************************************/
NET_CONN_ID NetConn_ID_TransportGet(NET_CONN_ID conn_id)
{
  NET_CONN    *p_conn = DEF_NULL;
  NET_CONN_ID conn_id_transport = NET_CONN_ID_NONE;

  //                                                               ---------------- VALIDATE NET CONN -----------------
  if (conn_id == NET_CONN_ID_NONE) {
    goto exit;
  }

  //                                                               ----------- GET NET CONN'S TRANSPORT ID ------------
  p_conn = &NetConn_Tbl[conn_id];
  conn_id_transport = p_conn->ID_Transport;

exit:
  return (conn_id_transport);
}

/****************************************************************************************************//**
 *                                           NetConn_ID_TransportSet()
 *
 * @brief    Set network connection's transport layer handle identifier.
 *
 * @param    conn_id             Handle identifier of network connection to set transport layer handle
 *                               identifier.
 *
 * @param    conn_id_transport   Connection's transport layer handle identifier.
 *******************************************************************************************************/
void NetConn_ID_TransportSet(NET_CONN_ID conn_id,
                             NET_CONN_ID conn_id_transport)
{
  NET_CONN *p_conn = DEF_NULL;

  //                                                               ----------- SET NET CONN'S TRANSPORT ID ------------
  p_conn = &NetConn_Tbl[conn_id];
  p_conn->ID_Transport = conn_id_transport;
}

/****************************************************************************************************//**
 *                                           NetConn_AddrLocalGet()
 *
 * @brief    Get network connection's local address.
 *
 * @param    conn_id         Handle identifier of network connection to get local address.
 *
 * @param    p_addr_local    Pointer to variable that will receive the return  local address (see Note #1),
 *                           if NO error(s).
 *
 * @param    p_addr_len      Pointer to a variable to ... :
 *                               - (a) Pass the size of the address buffer pointed to by 'p_addr_local'.
 *                               - (b) Return the actual local address length, if NO error(s);
 *                               - (c) Return 0,                               otherwise.
 *                           See also Note #2.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Network connection addresses maintained in network-order.
 *
 * @note     (2) Since 'p_addr_len' argument is both an input & output argument (see 'Argument(s) :
 *               p_addr_len'), ... :
 *               - (a) Its input value SHOULD be validated prior to use; ...
 *                   - (1) In the case that the 'p_addr_len' argument is passed a null pointer,
 *                         NO input value is validated or used.
 *               - (b) While its output value MUST be initially configured to return a default value
 *                     PRIOR to all other validation or function handling in case of any error(s).
 *******************************************************************************************************/
void NetConn_AddrLocalGet(NET_CONN_ID       conn_id,
                          CPU_INT08U        *p_addr_local,
                          NET_CONN_ADDR_LEN *p_addr_len,
                          RTOS_ERR          *p_err)
{
  NET_CONN    *p_conn;
  CPU_BOOLEAN is_used;

  *p_addr_len = 0u;                                             // Cfg dflt addr len for err (see Note #2b).

  //                                                               -------------- VALIDATE NET CONN USED --------------
  is_used = NetConn_IsUsed(conn_id);
  if (is_used != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }

  p_conn = &NetConn_Tbl[conn_id];

  //                                                               -------------- VALIDATE NET CONN ADDR --------------
  if (p_conn->AddrLocalValid != DEF_YES) {                      // If net conn local addr NOT avail, rtn err.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  //                                                               ------------ GET NET CONN'S LOCAL ADDR -------------
  NET_UTIL_VAL_COPY(p_addr_local,                               // Copy & rtn net conn local addr.
                    &p_conn->AddrLocal[0],
                    NET_CONN_ADDR_LEN_MAX);

  *p_addr_len = NET_CONN_ADDR_LEN_MAX;

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetConn_AddrLocalSet()
 *
 * @brief    Set network connection's local address.
 *
 * @param    conn_id         Handle identifier of network connection to set local address.
 *
 * @param    if_nbr          Interface number  of network connection with   local address.
 *
 * @param    p_addr_local    Pointer to local address (see Note #1).
 *
 * @param    addr_len        Length  of local address (in octets).
 *
 * @param    addr_over_wr    Allow      local address overwrite :
 *                           DEF_NO                          Do NOT overwrite local address.
 *                           DEF_YES                                Overwrite local address.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Network connection addresses maintained in network-order.
 *******************************************************************************************************/
void NetConn_AddrLocalSet(NET_CONN_ID       conn_id,
                          NET_IF_NBR        if_nbr,
                          CPU_INT08U        *p_addr_local,
                          NET_CONN_ADDR_LEN addr_len,
                          CPU_BOOLEAN       addr_over_wr,
                          RTOS_ERR          *p_err)
{
  NET_CONN *p_conn = &NetConn_Tbl[conn_id];;

  if (addr_over_wr != DEF_YES) {                                // If addr over-wr NOT req'd ...
    if (p_conn->AddrLocalValid != DEF_NO) {                     // ... & local addr valid,   ...
      NET_CTR_ERR_INC(Net_ErrCtrs.Conn.InvConnAddrInUseCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);                // ... rtn err.
      goto exit;
    }
  }

  //                                                               ------------ SET NET CONN'S LOCAL ADDR -------------
  Mem_Clr(&p_conn->AddrLocal[0],
          NET_CONN_ADDR_LEN_MAX);

  NET_UTIL_VAL_COPY(&p_conn->AddrLocal[0],                      // Copy local addr to net conn addr.
                    p_addr_local,
                    NET_CONN_ADDR_LEN_MAX);

  p_conn->AddrLocalValid = DEF_YES;
  p_conn->IF_Nbr = if_nbr;                                      // Set IF nbr.

  PP_UNUSED_PARAM(addr_len);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetConn_AddrRemoteGet()
 *
 * @brief    Get network connection's remote address.
 *
 * @param    conn_id         Handle identifier of network connection to get remote address.
 *
 * @param    p_addr_remote   Pointer to variable that will receive the return  remote address (see Note #1),
 *                           if NO error(s).
 *
 * @param    p_addr_len      Pointer to a variable to ... :
 *                               - (a) Pass the size of the address buffer pointed to by 'p_addr_remote'.
 *                               - (b) Return the actual local address length, if NO error(s);
 *                               - (c) Return 0,                               otherwise.
 *                           See also Note #2.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Network connection addresses maintained in network-order.
 *
 * @note     (2) Since 'p_addr_len' argument is both an input & output argument (see 'Argument(s) :
 *               p_addr_len'), ... :
 *               - (a) Its input value SHOULD be validated prior to use; ...
 *                   - (1) In the case that the 'p_addr_len' argument is passed a null pointer,
 *                         NO input value is validated or used.
 *               - (b) While its output value MUST be initially configured to return a default value
 *                     PRIOR to all other validation or function handling in case of any error(s).
 *******************************************************************************************************/
void NetConn_AddrRemoteGet(NET_CONN_ID       conn_id,
                           CPU_INT08U        *p_addr_remote,
                           NET_CONN_ADDR_LEN *p_addr_len,
                           RTOS_ERR          *p_err)
{
  NET_CONN    *p_conn;
  CPU_BOOLEAN is_used;

  *p_addr_len = 0u;                                             // Cfg dflt addr len for err (see Note #2b).

  //                                                               -------------- VALIDATE NET CONN USED --------------
  is_used = NetConn_IsUsed(conn_id);
  if (is_used != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }

  p_conn = &NetConn_Tbl[conn_id];

  //                                                               -------------- VALIDATE NET CONN ADDR --------------
  if (p_conn->AddrRemoteValid != DEF_YES) {                     // If net conn remote addr NOT avail, rtn err.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  //                                                               ------------ SET NET CONN'S REMOTE ADDR ------------
  NET_UTIL_VAL_COPY(p_addr_remote,                              // Copy & rtn net conn remote addr.
                    &p_conn->AddrRemote[0],
                    NET_CONN_ADDR_LEN_MAX);

  *p_addr_len = NET_CONN_ADDR_LEN_MAX;

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetConn_AddrRemoteSet()
 *
 * @brief    Set network connection's remote address.
 *
 * @param    conn_id         Handle identifier of network connection to set remote address.
 *
 * @param    p_addr_remote   Pointer to remote address (see Note #1).
 *
 * @param    addr_len        Length  of remote address (in octets).
 *
 * @param    addr_over_wr    Allow      remote address overwrite :
 *                           DEF_NO                          Do NOT overwrite remote address.
 *                           DEF_YES                                Overwrite remote address.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Network connection addresses maintained in network-order.
 *******************************************************************************************************/
void NetConn_AddrRemoteSet(NET_CONN_ID       conn_id,
                           CPU_INT08U        *p_addr_remote,
                           NET_CONN_ADDR_LEN addr_len,
                           CPU_BOOLEAN       addr_over_wr,
                           RTOS_ERR          *p_err)
{
  NET_CONN    *p_conn;
  CPU_BOOLEAN is_used;

  //                                                               -------------- VALIDATE NET CONN USED --------------
  is_used = NetConn_IsUsed(conn_id);
  if (is_used != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }

  p_conn = &NetConn_Tbl[conn_id];

  //                                                               ------------------ VALIDATE ADDR -------------------
  if (addr_over_wr != DEF_YES) {                                // If addr over-wr NOT req'd ...
    if (p_conn->AddrRemoteValid == DEF_YES) {                   // ... & remote addr valid,  ...
      NET_CTR_ERR_INC(Net_ErrCtrs.Conn.InvConnAddrInUseCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);                // ... rtn err.
      goto exit;
    }
  }

  //                                                               ------------ GET NET CONN'S REMOTE ADDR ------------
  NET_UTIL_VAL_COPY(&p_conn->AddrRemote[0],                     // Copy remote addr to net conn addr.
                    p_addr_remote,
                    NET_CONN_ADDR_LEN_MAX);

  p_conn->AddrRemoteValid = DEF_YES;

  PP_UNUSED_PARAM(addr_len);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetConn_AddrRemoteCmp()
 *
 * @brief    Compare an address to a network connection's remote address.
 *
 * @param    conn_id         Handle identifier of connection to compare.
 *
 * @param    p_addr_remote   Pointer to remote address to compare.
 *
 * @param    addr_len        Length  of remote address (in octets).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if addresses successfully compare.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetConn_AddrRemoteCmp(NET_CONN_ID       conn_id,
                                  CPU_INT08U        *p_addr_remote,
                                  NET_CONN_ADDR_LEN addr_len,
                                  RTOS_ERR          *p_err)
{
  NET_CONN    *p_conn = DEF_NULL;
  CPU_BOOLEAN cmp = DEF_NO;

  p_conn = &NetConn_Tbl[conn_id];

  //                                                               -------------- VALIDATE NET CONN ADDR --------------
  if (p_conn->AddrRemoteValid != DEF_YES) {                     // If conn local addr NOT avail, rtn err.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  //                                                               ------------ VALIDATE NET CONN ADDR LEN -------------
  switch (p_conn->Family) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_CONN_FAMILY_IP_V4_SOCK:
      if (addr_len != NET_SOCK_ADDR_IP_V4_LEN_PORT_ADDR) {
        NET_CTR_ERR_INC(Net_ErrCtrs.Conn.InvConnAddrLenCtr);
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, DEF_NO);
      }
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_CONN_FAMILY_IP_V6_SOCK:
      if (addr_len != NET_SOCK_ADDR_IP_V6_LEN_PORT_ADDR) {
        NET_CTR_ERR_INC(Net_ErrCtrs.Conn.InvConnAddrLenCtr);
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, DEF_NO);
      }
      break;
#endif

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.Conn.InvFamilyCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
      goto exit;
  }

  cmp = Mem_Cmp(p_addr_remote,                                  // Cmp remote addr to conn addr.
                &p_conn->AddrRemote[0],
                addr_len);

exit:
  return (cmp);
}

/****************************************************************************************************//**
 *                                           NetConn_IPv4TxParamsGet()
 *
 * @brief    (1) Get network connection's configured transmit IPv4 parameters :
 *               - (a) TOS
 *               - (b) TTL
 *               - (c) Flags
 *
 * @param    conn_id     Handle identifier of network connection to get configured transmit IPv4 parameters.
 *
 * @param    p_ip_flags  Pointer to variable that will receive the return transmit IPv4 flags, if NO error(s).
 *
 * @param    p_ip_tos    Pointer to variable that will receive the return transmit IPv4 TOS,   if NO error(s).
 *
 * @param    p_ip_ttl    Pointer to variable that will receive the return transmit IPv4 TTL,   if NO error(s).
 *******************************************************************************************************/

#ifdef  NET_IPv4_MODULE_EN
void NetConn_IPv4TxParamsGet(NET_CONN_ID    conn_id,
                             NET_IPv4_FLAGS *p_ip_flags,
                             NET_IPv4_TOS   *p_ip_tos,
                             NET_IPv4_TTL   *p_ip_ttl)
{
  NET_CONN *p_conn;

  //                                                               ----------- GET NET CONN'S IP TX PARAMS ------------
  p_conn = &NetConn_Tbl[conn_id];
  *p_ip_flags = p_conn->TxIPv4Flags;
  *p_ip_tos = p_conn->TxIPv4TOS;
  *p_ip_ttl = p_conn->TxIPv4TTL;
}
#endif

/****************************************************************************************************//**
 *                                           NetConn_IPv4TxTOS_Set()
 *
 * @brief    Set network connection's configured transmit IPv4 TOS.
 *
 * @param    conn_id     Handle identifier of network connection to set configured transmit IPv4 TOS.
 *
 * @param    ip_tos      Desired transmit IPv4 TOS (see Note #1).
 *
 * @note     (1) (a) RFC #1122, Section 4.1.4 states that "an application-layer program MUST be
 *                   able to set the ... [IP] TOS ... for sending a ... datagram, [which] must
 *                   be passed transparently to the IP layer".
 *               - (b) RFC #1122, Section 4.2.4.2 reiterates that :
 *                   - (1) "The application layer MUST be able to specify the [IP] Type-of-Service
 *                          (TOS) for [packets] that are sent on a connection."
 *                   - (2) "It not required [sic], but the application SHOULD be able to change
 *                          the [IP] TOS during the connection lifetime."
 *
 *             See also 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES'.*********************************************************************************************************
 *******************************************************************************************************/

#ifdef  NET_IPv4_MODULE_EN
void NetConn_IPv4TxTOS_Set(NET_CONN_ID  conn_id,
                           NET_IPv4_TOS ip_tos)
{
  NET_CONN    *p_conn;
  CPU_BOOLEAN valid;

  //                                                               ----------------- VALIDATE IP TOS ------------------
  valid = NetIPv4_IsValidTOS(ip_tos);
  RTOS_ASSERT_DBG((valid == DEF_YES), RTOS_ERR_INVALID_ARG,; );

  //                                                               ------------- SET NET CONN'S IP TX TOS -------------
  p_conn = &NetConn_Tbl[conn_id];
  p_conn->TxIPv4TOS = ip_tos;
}
#endif

/****************************************************************************************************//**
 *                                           NetConn_IPv4TxTTL_Set()
 *
 * @brief    Set network connection's configured transmit IPv4 TTL.
 *
 * @param    conn_id     Handle identifier of network connection to set configured transmit IPv4 TTL.
 *
 * @param    ip_ttl      Desired transmit IPv4 TTL (see Note #1) :
 *                           - NET_IPv4_TTL_MIN                  Minimum TTL transmit value   (1)
 *                           - NET_IPv4_TTL_MAX                  Maximum TTL transmit value (255)
 *                           - NET_IPv4_TTL_DFLT                 Default TTL transmit value (128)
 *                           - NET_IPv4_TTL_NONE                 Replace with default TTL
 *
 * @note     (1) (a) RFC #1122, Section 4.1.4 states that "an application-layer program MUST
 *                   be able to set the [IP] TTL ... for sending a ... datagram, [which] must
 *                   be passed transparently to the IP layer".
 *               - (b) RFC #1122, Section 4.2.2.19 reiterates that "the [IP] TTL value used to
 *                     send ... [packets] MUST be configurable".
 *                     See also 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES'.
 *******************************************************************************************************/

#ifdef  NET_IPv4_MODULE_EN
void NetConn_IPv4TxTTL_Set(NET_CONN_ID  conn_id,
                           NET_IPv4_TTL ip_ttl)
{
  NET_CONN    *p_conn;
  CPU_BOOLEAN valid;

  //                                                               ----------------- VALIDATE IP TTL ------------------
  valid = NetIPv4_IsValidTTL(ip_ttl);
  RTOS_ASSERT_DBG((valid == DEF_YES), RTOS_ERR_INVALID_ARG,; );

  //                                                               ------------- SET NET CONN'S IP TX TTL -------------
  p_conn = &NetConn_Tbl[conn_id];
  p_conn->TxIPv4TTL = ip_ttl;
}
#endif

/****************************************************************************************************//**
 *                                       NetConn_IPv4TxTTL_MulticastGet()
 *
 * @brief    Get network connection's configured transmit IPv4 multicast TTL.
 *
 * @param    conn_id     Handle identifier of network connection to get configured transmit IPv4 multicast
 *                       TTL.
 *
 * @return   Network connection's configured transmit IP multicast TTL, if NO error(s).
 *           NET_IPv4_TTL_NONE,  otherwise.
 *******************************************************************************************************/

#if ((defined(NET_IPv4_MODULE_EN)) \
  && (defined(NET_MCAST_TX_MODULE_EN)))
NET_IPv4_TTL NetConn_IPv4TxTTL_MulticastGet(NET_CONN_ID conn_id)
{
  NET_CONN     *p_conn = DEF_NULL;
  NET_IPv4_TTL ip_ttl = NET_IPv4_TTL_NONE;

  //                                                               ---------------- VALIDATE NET CONN -----------------
  if (conn_id == NET_CONN_ID_NONE) {
    goto exit;
  }

  //                                                               -------- GET NET CONN'S IP TX MULTICAST TTL --------
  p_conn = &NetConn_Tbl[conn_id];
  ip_ttl = p_conn->TxIPv4TTL_Multicast;

exit:
  return (ip_ttl);
}
#endif

/****************************************************************************************************//**
 *                                       NetConn_IPv4TxTTL_MulticastSet()
 *
 * @brief    Set network connection's configured transmit IPv4 multicast TTL.
 *
 * @param    conn_id     Handle identifier of network connection to set configured transmit IPv4 multicast
 *                       TTL.
 *
 * @param    ip_ttl      Desired transmit IP multicast TTL (see Note #1) :
 *                           - NET_IPv4_TTL_MIN                  Minimum TTL transmit value   (1)
 *                           - NET_IPv4_TTL_MAX                  Maximum TTL transmit value (255)
 *                           - NET_IPv4_TTL_DFLT                 Default TTL transmit value   (1)
 *                           - NET_IPv4_TTL_NONE                 Replace with default TTL
 *
 * @note     (1) RFC #1112, Section 6.1 states that "the service interface should provide a way for the
 *               upper-layer protocol to specify the IP time-to-live of an outgoing multicast datagram".
 *
 *               See also 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES'.
 *******************************************************************************************************/

#if ((defined(NET_IPv4_MODULE_EN)) \
  && (defined(NET_MCAST_TX_MODULE_EN)))
void NetConn_IPv4TxTTL_MulticastSet(NET_CONN_ID  conn_id,
                                    NET_IPv4_TTL ip_ttl)
{
  NET_CONN    *p_conn;
  CPU_BOOLEAN valid;

  //                                                               ------------ VALIDATE IP MULTICAST TTL -------------
  valid = NetIPv4_IsValidTTL(ip_ttl);
  RTOS_ASSERT_DBG((valid == DEF_YES), RTOS_ERR_INVALID_ARG,; );

  //                                                               -------- SET NET CONN'S IP TX MULTICAST TTL --------
  p_conn = &NetConn_Tbl[conn_id];
  p_conn->TxIPv4TTL_Multicast = ip_ttl;
}
#endif

/****************************************************************************************************//**
 *                                           NetConn_IPv6TxParamsGet()
 *
 * @brief    (1) Get network connection's configured transmit IPv6 parameters :
 *               - (a) Traffic Class
 *               - (b) Flow Label
 *               - (c) Hop Limit
 *               - (d) IPv6 Flags
 *
 * @param    conn_id             Handle identifier of network connection to get configured transmit IPv4 parameters.
 *
 * @param    p_ip_traffic_class  Pointer to variable that will receive the return transmit IPv4 flags, if NO error(s).
 *
 * @param    p_ip_flow_label     Pointer to variable that will receive the return transmit IPv4 TOS,   if NO error(s).
 *
 * @param    p_ip_hop_lim        Pointer to variable that will receive the return transmit IPv4 TTL,   if NO error(s).
 *
 * @param    p_ip_flags          Pointer to variable that will receive the return transmit IPv4 TTL,   if NO error(s).
 *******************************************************************************************************/

#ifdef  NET_IPv6_MODULE_EN
void NetConn_IPv6TxParamsGet(NET_CONN_ID            conn_id,
                             NET_IPv6_TRAFFIC_CLASS *p_ip_traffic_class,
                             NET_IPv6_FLOW_LABEL    *p_ip_flow_label,
                             NET_IPv6_HOP_LIM       *p_ip_hop_lim,
                             NET_IPv6_FLAGS         *p_ip_flags)
{
  NET_CONN *p_conn;

  //                                                               ----------- GET NET CONN'S IP TX PARAMS ------------
  p_conn = &NetConn_Tbl[conn_id];

  if (p_ip_traffic_class != DEF_NULL) {
    *p_ip_traffic_class = p_conn->TxIPv6TrafficClass;
  }
  if (p_ip_flow_label != DEF_NULL) {
    *p_ip_flow_label = p_conn->TxIPv6FlowLabel;
  }
  if (p_ip_hop_lim != DEF_NULL) {
    *p_ip_hop_lim = p_conn->TxIPv6HopLim;
  }
  if (p_ip_flags != DEF_NULL) {
    *p_ip_flags = p_conn->TxIPv6Flags;
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetConn_IPv6TxHopLimMcastGet()
 *
 * @brief    Get network connection's configured transmit IPv6 multicast hop limit.
 *
 * @param    conn_id     Handle identifier of network connection to get configured transmit IPv6 multicast
 *                       hop limit.
 *
 * @return   Network connection's configured transmit IP multicast hop limit, if NO error(s).
 *           NET_IPv6_HOP_LIM_NONE, otherwise.
 *******************************************************************************************************/
#if ((defined(NET_IPv6_MODULE_EN)) \
  && (defined(NET_MCAST_TX_MODULE_EN)))
NET_IPv6_HOP_LIM NetConn_IPv6TxHopLimMcastGet(NET_CONN_ID conn_id)
{
  NET_CONN         *p_conn = DEF_NULL;
  NET_IPv6_HOP_LIM ip_hop_lim = NET_IPv6_HOP_LIM_NONE;

  if (conn_id == NET_CONN_ID_NONE) {
    goto exit;
  }

  //                                                               ------ GET NET CONN'S IP TX MULTICAST HOP LIM ------
  p_conn = &NetConn_Tbl[conn_id];
  ip_hop_lim = p_conn->TxIPv6HopLimMulticast;

exit:
  return (ip_hop_lim);
}
#endif

/****************************************************************************************************//**
 *                                       NetConn_IPv6TxHopLimMcastSet()
 *
 * @brief    Set network connection's configured transmit IPv6 multicast hop limit.
 *
 * @param    conn_id     Handle identifier of network connection to set configured transmit IPv6 multicast
 *                       hop limit.
 *
 * @param    ip_hop_lim  Desired transmit IP multicast TTL (see Note #1) :
 *                           - NET_IPv6_HOP_LIM_MIN            Minimum hop limit transmit value   (1)
 *                           - NET_IPv6_HOP_LIM_MAX            Maximum hop limit transmit value (255)
 *                           - NET_IPv6_HOP_LIM_DFLT           Default hop limit transmit value   (1)
 *                           - NET_IPv6_HOP_LIM_NONE           Replace with default hop limit
 *
 * @note     (1) RFC #1112, Section 6.1 states that "the service interface should provide a way for the
 *               upper-layer protocol to specify the IP time-to-live of an outgoing multicast datagram".
 *
 *               See also 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES'.
 *******************************************************************************************************/
#if 0
#if ((defined(NET_IPv6_MODULE_EN)) \
  && (defined(NET_MCAST_TX_MODULE_EN)))
void NetConn_IPv6TxHopLimMcastSet(NET_CONN_ID      conn_id,
                                  NET_IPv6_HOP_LIM ip_hop_lim)
{
  NET_CONN    *p_conn;
  CPU_BOOLEAN valid;

  //                                                               ------------ VALIDATE IP MULTICAST TTL -------------
  valid = NetIPv6_IsValidHopLim(ip_hop_lim);
  RTOS_ASSERT_DBG_ERR_SET((valid == DEF_YES), *p_err, RTOS_ERR_INVALID_ARG,; );

  //                                                               -------- SET NET CONN'S IP TX MULTICAST TTL --------
  p_conn = &NetConn_Tbl[conn_id];
  p_conn->TxIPv6HopLimMulticast = ip_hop_lim;
}
#endif
#endif

/****************************************************************************************************//**
 *                                               NetConn_IsUsed()
 *
 * @brief    Validate network connection in use.
 *
 * @param    conn_id     Handle identifier of network connection to validate.
 *
 * @return   DEF_YES, network connection   valid &      in use.
 *           DEF_NO,  network connection invalid or NOT in use.
 *
 * @note     (1) NetConn_IsUsed() MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
CPU_BOOLEAN NetConn_IsUsed(NET_CONN_ID conn_id)
{
  NET_CONN    *p_conn;
  CPU_BOOLEAN used;

  if (conn_id == NET_CONN_ID_NONE) {
    return (DEF_NO);
  }
  //                                                               --------------- VALIDATE NET CONN ID ---------------
  RTOS_ASSERT_CRITICAL((conn_id >= NET_CONN_ID_MIN), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NO);
  RTOS_ASSERT_CRITICAL((conn_id <= (NET_CONN_ID)NET_CONN_ID_MAX), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NO);

  //                                                               -------------- VALIDATE NET CONN USED --------------
  p_conn = &NetConn_Tbl[conn_id];
  used = DEF_BIT_IS_SET(p_conn->Flags, NET_CONN_FLAG_USED);
  if (used != DEF_YES) {
    return (DEF_NO);
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                               NetConn_IsIPv6()
 *
 * @brief    Determine IP address used.
 *
 * @param    conn_id     Handle identifier of network connection to check.
 *
 * @return   DEF_YES, network connection is     IPv6.
 *           DEF_NO,  network connection is NOT IPv6.
 *
 * @note     (1) NetConn_IsIPv6() MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
CPU_BOOLEAN NetConn_IsIPv6(NET_CONN_ID conn_id)
{
  NET_CONN    *p_conn = DEF_NULL;
  CPU_BOOLEAN ipv6 = DEF_NO;

  p_conn = &NetConn_Tbl[conn_id];

  if (p_conn->Family == NET_CONN_FAMILY_IP_V6_SOCK) {
    ipv6 = DEF_YES;
  }

  return (ipv6);
}

/****************************************************************************************************//**
 *                                               NetConn_IsConn()
 *
 * @brief    Determine network connection status.
 *
 * @param    conn_id     Handle identifier of network connection to check.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) NetConn_IsConn() MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
CPU_INT08U NetConn_IsConn(NET_CONN_ID conn_id,
                          RTOS_ERR    *p_err)
{
  NET_CONN    *p_conn = DEF_NULL;
  CPU_INT08U  conn_type = NET_CONN_TYPE_CONN_NONE;
  CPU_BOOLEAN is_used = DEF_NO;

  //                                                               -------------- VALIDATE NET CONN USED --------------
  is_used = NetConn_IsUsed(conn_id);
  if (is_used != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }

  //                                                               ----------------- GET CONN STATUS ------------------
  p_conn = &NetConn_Tbl[conn_id];
  if (p_conn->AddrLocalValid == DEF_YES) {
    if (p_conn->AddrRemoteValid == DEF_YES) {
      conn_type = NET_CONN_TYPE_CONN_FULL;
    } else {
      conn_type = NET_CONN_TYPE_CONN_HALF;
    }
  } else {
    conn_type = NET_CONN_TYPE_CONN_NONE;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (conn_type);
}

/****************************************************************************************************//**
 *                                           NetConn_IsPortUsed()
 *
 * @brief    Check if the given port number is the one of an already used port.
 *
 * @param    port_nbr    Port number to check.
 *
 * @param    protocol    Network protocol type:
 *                           - NET_PROTOCOL_TYPE_UDP_V4
 *                           - NET_PROTOCOL_TYPE_TCP_V4
 *                           - NET_PROTOCOL_TYPE_UDP_V6
 *                           - NET_PROTOCOL_TYPE_TCP_V6
 *
 * @return   DEF_YES, if port is used,
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetConn_IsPortUsed(NET_PORT_NBR      port_nbr,
                               NET_PROTOCOL_TYPE protocol)
{
  NET_CONN             *p_conn = &NetConn_Tbl[0];
  NET_CONN_PROTOCOL_IX protocol_ix = DEF_NULL;
  CPU_INT32U           i;
  CPU_BOOLEAN          used = DEF_YES;

  switch (protocol) {
    case NET_PROTOCOL_TYPE_UDP_V4:
      protocol_ix = NET_CONN_PROTOCOL_IX_IP_V4_UDP;
      break;

    case NET_PROTOCOL_TYPE_TCP_V4:
      protocol_ix = NET_CONN_PROTOCOL_IX_IP_V4_TCP;
      break;

    case NET_PROTOCOL_TYPE_UDP_V6:
      protocol_ix = NET_CONN_PROTOCOL_IX_IP_V6_UDP;
      break;

    case NET_PROTOCOL_TYPE_TCP_V6:
      protocol_ix = NET_CONN_PROTOCOL_IX_IP_V6_TCP;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_YES);
  }

  for (i = 0; i < NET_CONN_NBR_CONN; i++) {
    if (p_conn->ProtocolIx == protocol_ix) {
      CPU_BOOLEAN  found;
      NET_PORT_NBR *p_port = (NET_PORT_NBR *)&p_conn->AddrLocal[0] + NET_CONN_ADDR_IP_IX_PORT;

      found = Mem_Cmp(&port_nbr,
                      p_port,
                      sizeof(port_nbr));
      if (found == DEF_YES) {
        used = DEF_YES;
        goto exit;
      }
    }
  }

  used = DEF_NO;

exit:
  return (used);
}

/****************************************************************************************************//**
 *                                               NetConn_Srch()
 *
 * @brief    (1) Search connection list for network connection with specific local &/or remote addresses :
 *               - (a) Get    network connection list  head pointer
 *               - (b) Search network connection list  for best-match  network connection chain
 *               - (c) Search network connection chain for best-match  network connection
 *               - (d) Return network connection handle identifier, if network connection     found
 *                     OR
 *                     Null identifier,                             if network connection NOT found
 *           - (2) Network connection are organized into connection lists & chains to expedite connection
 *                   searches.
 *               - (a) (1) For each connection family & protocol type, one connection list is maintained.
 *                   - (2) (A) Each connection list is maintained as a list of connection lists.  Each
 *                             connection list is uniquely organized by the local port number.
 *                             In other words, a connection list's top-level connections are each the heads
 *                             of their own connection list & each have a local port number that is unique
 *                             from all the other connection lists.
 *                       - (B) Each connection list maintained within a connection list is also referred to
 *                             as a connection chain.
 *                   - (3) Each connection chain is an organization of connections that all share the same
 *                         local port number but different local address &/or remote address.
 *                         In other words, connections in a connection chain share the same local port number;
 *                         however, no two connections in a connection chain share the same local AND remote
 *                         connection address(s).
 *                   - (4) In the diagram below, ... :
 *                       - (A) The top horizontal row  represents a   connection list's list of connection chains.
 *                       - (B) Each    vertical column represents the connections in unique     connection chains.
 *                       - (C) Each connection list is a pointer that points to the head of the connection list.
 *                       - (D) Connections' 'PrevChainPtr' & 'NextChainPtr' doubly-link each connection chain to
 *                             form the list of connection chains.
 *                       - (E) Connections' 'PrevConnPtr'  & 'NextConnPtr'  doubly-link each connection to form
 *                             a connection chain.
 *               - (b) (1) For any connection search, the specific family/protocol connection list is searched
 *                           primarily by the local port number to find the appropriate connection chain.  If a
 *                           connection chain with the local port number is found, then each connection in the
 *                           connection chain is searched in order to find the connection with the best match.
 *                   - (2) Network connection searches are resolved in order :
 *                       - (A) From greatest number of identical connection address fields ...
 *                       - (B)   to least    number of identical connection address fields.
 *                   - (3) To expedite faster connection searches for recently added (or recently promoted)
 *                           network connections :
 *                       - (A) (1) (a) Network connection chains are added at (or promoted to); ...
 *                               - (b) network connection chains are searched starting at       ...
 *                           - (2) ... the head of a network connection list.
 *                       - (B) (1) (a) Network connections       are added at (or promoted to); ...
 *                               - (b) network connections       are searched starting at       ...
 *                           - (2) ... the head of a network connection chain.
 *                     @verbatim
 *                       See also 'NetConn_Add()  Note #1'.
 *
 *                                       |                                                             |
 *                                       |<---------------- List of Connection Chains ---------------->|
 *                                       |                      (see Note #2a4A)                       |
 *
 *                       New connection chains inserted at head
 *                           of connection list (see Note #2b3A2);
 *                           new connections inserted at head of
 *                           connection chain (see Note #2b3B2)
 *
 *                                           |              NextChainPtr
 *                                           |            (see Note #2a4D)
 *                                           v                    |
 *                                                                |
 *                       Head of          -------       -------   v   -------       -------       -------
 *                   Connection List ---->|     |------>|     |------>|     |------>|     |------>|     |
 *                                        |     |       |     |       |     |       |     |       |     |
 *                  (see Note #2a4C)      |     |<------|     |<------|     |<------|     |<------|     |
 *                                        -------       -------   ^   -------       -------       -------
 *                                          | ^                   |                   | ^
 *                                          | |                   |                   | |
 *                                          v |             PrevChainPtr              v |
 *            ---                         -------         (see Note #2a4D)          -------
 *            ^                           |     |                                   |     |
 *            |                           |     |                                   |     |
 *            |                           |     |                                   |     |
 *            |                           -------                                   -------
 *            |                             | ^                                       | ^
 *            |           NextConnPtr ----> | |                                       | | <----  PrevConnPtr
 *            |        (see Note #2a4E)     v |                                       v |     (see Note #2a4E)
 *                                        -------                                   -------
 *            ns organized                |     |                                   |     |
 *            nection chain               |     |                                   |     |
 *            te #2a4B)                   |     |                                   |     |
 *                                        -------                                   -------
 *            |                             | ^
 *            |                             | |
 *            |                             v |
 *            |                           -------
 *            |                           |     |
 *            |                           |     |
 *            v                           |     |
 *            ---                         -------
 *                     @endverbatim
 * @param    family                  Network connection family type.
 *
 * @param    protocol_ix             Network connection protocol index.
 *
 * @param    p_addr_local            Pointer to local  address (see Note #3).
 *
 * @param    p_addr_remote           Pointer to remote address (see Note #3).
 *
 * @param    addr_len                Length  of search addresses (in octets).
 *
 * @param    p_conn_id_transport     Pointer to variable that will receive the returned network
 *                                   connection transport   handle identifier.
 *
 * @param    p_conn_id_app           Pointer to variable that will receive the returned network
 *                                   connection application handle identifier.
 *
 * @return   Handle identifier of network connection with specific local &/or remote address, if found.
 *           NET_CONN_ID_NONE, otherwise.
 *
 * @note     (3) Network connection addresses maintained in network-order.
 *
 * @note     (4) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
NET_CONN_ID NetConn_Srch(NET_CONN_FAMILY      family,
                         NET_CONN_PROTOCOL_IX protocol_ix,
                         CPU_INT08U           *p_addr_local,
                         CPU_INT08U           *p_addr_remote,
                         NET_CONN_ADDR_LEN    addr_len,
                         NET_CONN_ID          *p_conn_id_transport,
                         NET_CONN_ID          *p_conn_id_app,
                         NET_CONN_STATE       *p_conn_state)
{
  NET_CONN    **p_conn_list;
  NET_CONN    *p_conn_chain;
  NET_CONN    *p_conn;
  NET_CONN_ID conn_id = NET_CONN_ID_NONE;
  CPU_INT08U  addr_wildcard[NET_CONN_ADDR_LEN_MAX];
  CPU_INT08U  *p_addr_wildcard;

  *p_conn_state = NET_CONN_STATE_NONE;

  //                                                               Init conn id's for err or failed srch (see Note #4).
  if (p_conn_id_transport != DEF_NULL) {
    *p_conn_id_transport = NET_CONN_ID_NONE;
  }

  if (p_conn_id_app != DEF_NULL) {
    *p_conn_id_app = NET_CONN_ID_NONE;
  }

  //                                                               ------ VALIDATE/CFG CONN LIST FAMILY/PROTOCOL ------
  p_addr_wildcard = DEF_NULL;                                   // Clr wildcard addr.

  switch (family) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_CONN_FAMILY_IP_V4_SOCK:
#endif
#ifdef  NET_IPv6_MODULE_EN
    case NET_CONN_FAMILY_IP_V6_SOCK:
#endif
    if (NetConn_AddrWildCardAvailv4 == DEF_YES) {                 // Cfg wildcard addr.
#ifdef  NET_IPv4_MODULE_EN
      if (family == NET_CONN_FAMILY_IP_V4_SOCK) {
        Mem_Clr(&addr_wildcard, NET_SOCK_ADDR_LEN_MAX);

        Mem_Copy(&addr_wildcard[0],
                 p_addr_local,
                 NET_SOCK_ADDR_LEN_IP_V4);

        Mem_Copy(&addr_wildcard[NET_CONN_ADDR_IP_V4_IX_ADDR],
                 &NetConn_AddrWildCardv4[0],
                 NET_CONN_ADDR_IP_V4_LEN_ADDR);
      }
#endif
    }

    if (NetConn_AddrWildCardAvailv6 == DEF_YES) {                // Cfg wildcard addr.
#ifdef  NET_IPv6_MODULE_EN
      if (family == NET_CONN_FAMILY_IP_V6_SOCK) {
        Mem_Clr(&addr_wildcard, NET_SOCK_ADDR_LEN_MAX);

        Mem_Copy(&addr_wildcard[0],
                 p_addr_local,
                 NET_SOCK_ADDR_LEN_IP_V6);

        Mem_Copy(&addr_wildcard[NET_CONN_ADDR_IP_V6_IX_ADDR],
                 &NetConn_AddrWildCardv6[0],
                 NET_CONN_ADDR_IP_V6_LEN_ADDR);
      }
#endif
    }

    p_addr_wildcard = &addr_wildcard[0];
    break;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.Conn.InvFamilyCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_CONN_ID_NONE);
      conn_id = NET_CONN_ID_NONE;
  }

  //                                                               ---------------- SRCH NET CONN LIST ----------------
  p_conn_list = &NetConn_ConnListHead[protocol_ix];
  p_conn_chain = NetConn_ListSrch(family,
                                  p_conn_list,
                                  p_addr_local);
  if (p_conn_chain == DEF_NULL) {
    conn_id = NET_CONN_ID_NONE;
    goto exit;                                                  // NO net conn chain found.
  }

  //                                                               ---------------- SRCH NET CONN CHAIN ---------------
  p_conn = NetConn_ChainSrch(p_conn_list,
                             p_conn_chain,
                             p_addr_local,
                             p_addr_wildcard,
                             p_addr_remote,
                             p_conn_state);

  if (p_conn == DEF_NULL) {                                     // NO net conn       found.
    conn_id = NET_CONN_ID_NONE;
    goto exit;                                                  // Rtn err from NetConn_ChainSrch().
  }

  //                                                               If net conn       found, rtn conn id's.
  if (p_conn_id_transport != DEF_NULL) {
    *p_conn_id_transport = p_conn->ID_Transport;
  }

  if (p_conn_id_app != DEF_NULL) {
    *p_conn_id_app = p_conn->ID_App;
  }

  conn_id = p_conn->ID;

  PP_UNUSED_PARAM(addr_len);

exit:
  return (conn_id);
}

/****************************************************************************************************//**
 *                                               NetConn_ListAdd()
 *
 * @brief    (1) Add a network connection into a connection list :
 *               - (a) Get network connection's appropriate connection list
 *               - (b) Get network connection's appropriate connection chain
 *               - (c) Add network connection into          connection list
 *
 * @param    conn_id     Handle identifier of network connection to add.
 *
 * @note     (2) Network connection addresses maintained in network-order.
 *
 * @note     (3) Local address MUST be configured prior to network connection list add.
 *******************************************************************************************************/
void NetConn_ListAdd(NET_CONN_ID conn_id)
{
  NET_CONN             *p_conn;
  NET_CONN             *p_conn_chain;
  NET_CONN             **p_conn_list;
  NET_CONN_PROTOCOL_IX protocol_ix;

  p_conn = &NetConn_Tbl[conn_id];
  //                                                               ------ VALIDATE/CFG CONN LIST FAMILY/PROTOCOL ------
  switch (p_conn->Family) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_CONN_FAMILY_IP_V4_SOCK:
      break;
#endif
#ifdef  NET_IPv6_MODULE_EN
    case NET_CONN_FAMILY_IP_V6_SOCK:
      break;
#endif

    case NET_CONN_FAMILY_NONE:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.Conn.InvFamilyCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               ----------- VALIDATE NET CONN LOCAL ADDR -----------
  if (p_conn->AddrLocalValid != DEF_YES) {
    goto exit;
  }

  //                                                               ---------------- GET NET CONN LIST -----------------
  protocol_ix = p_conn->ProtocolIx;

  //                                                               ---------- GET NET CONN'S CONN LIST CHAIN ----------
  p_conn_list = &NetConn_ConnListHead[protocol_ix];
  p_conn_chain = NetConn_ListSrch(p_conn->Family,
                                  p_conn_list,
                                  &p_conn->AddrLocal[0]);

  //                                                               --------- ADD NET CONN INTO NET CONN LIST ----------
  NetConn_Add(p_conn_list, p_conn_chain, p_conn);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetConn_ListUnlink()
 *
 * @brief    Unlink a network connection from a connection list.
 *
 * @param    conn_id     Handle identifier of network connection to unlink.
 *******************************************************************************************************/
void NetConn_ListUnlink(NET_CONN_ID conn_id)
{
  NET_CONN *p_conn = &NetConn_Tbl[conn_id];

  NetConn_Unlink(p_conn);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetConn_ListSrch()
 *
 * @brief    Search a network connection list for network connection chain with specific local port.
 *
 * @param    family          Network connection family type.
 *
 * @param    p_conn_list     Pointer to a connection list.
 *
 * @param    p_addr_local    Pointer to local address.
 *
 * @return   Pointer to connection chain with specific local port, if found.
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) Network connection addresses maintained in network-order.
 *
 * @note     (2) (a) Assumes ALL connection lists' network connections' local addresses are valid.
 *
 *           - (b) Any connections whose local addresses are NOT valid are closed & unlinked
 *                   from their respective connection list.  Such connections may be accessed by
 *                   the application layer but will NOT correctly receive demultiplexed packets
 *                   due to invalid/incomplete connection address(s).
 *******************************************************************************************************/
static NET_CONN *NetConn_ListSrch(NET_CONN_FAMILY family,
                                  NET_CONN        **p_conn_list,
                                  CPU_INT08U      *p_addr_local)
{
  CPU_INT08U        *p_port_nbr_addr_local;
  CPU_INT08U        *p_port_nbr_conn_chain;
  NET_CONN_ADDR_LEN port_nbr_len;
  NET_CONN          *p_conn_chain;
  NET_CONN          *p_conn_chain_next;
  NET_CONN          *p_conn_next;
  CPU_BOOLEAN       found;
  CPU_INT16U        th;
  CORE_DECLARE_IRQ_STATE;

  switch (family) {                                                         // Get local port nbr to srch.
#ifdef  NET_IPv4_MODULE_EN
    case NET_CONN_FAMILY_IP_V4_SOCK:
      p_port_nbr_addr_local = p_addr_local + NET_CONN_ADDR_IP_IX_PORT;
      port_nbr_len = NET_CONN_ADDR_IP_LEN_PORT;
      break;
#endif
#ifdef  NET_IPv6_MODULE_EN
    case NET_CONN_FAMILY_IP_V6_SOCK:
      p_port_nbr_addr_local = p_addr_local + NET_CONN_ADDR_IP_IX_PORT;
      port_nbr_len = NET_CONN_ADDR_IP_LEN_PORT;
      break;
#endif
    case NET_CONN_FAMILY_NONE:                                              // See Note #3.
    default:
      return (DEF_NULL);
  }

  //                                                               ---------- SRCH NET CONN LIST ----------
  p_conn_chain = (NET_CONN *)*p_conn_list;                                  // Start @ list head (see Note #3a).
  found = DEF_NO;

  while ((p_conn_chain != DEF_NULL)                                         // Srch ALL net conn chains ..
         && (found == DEF_NO)  ) {                                          // .. until net conn chain found.
    p_conn_chain_next = p_conn_chain->NextChainPtr;

    if (p_conn_chain->AddrLocalValid == DEF_YES) {                          // If conn chain's local addr valid, ...
      switch (p_conn_chain->Family) {                                       // ... cmp local port nbrs.
#ifdef  NET_IPv4_MODULE_EN
        case NET_CONN_FAMILY_IP_V4_SOCK:
          p_port_nbr_conn_chain = &p_conn_chain->AddrLocal[0] + NET_CONN_ADDR_IP_IX_PORT;
          found = Mem_Cmp(p_port_nbr_addr_local,
                          p_port_nbr_conn_chain,
                          port_nbr_len);
          break;
#endif
#ifdef  NET_IPv6_MODULE_EN
        case NET_CONN_FAMILY_IP_V6_SOCK:
          p_port_nbr_conn_chain = &p_conn_chain->AddrLocal[0] + NET_CONN_ADDR_IP_IX_PORT;
          found = Mem_Cmp(p_port_nbr_addr_local,
                          p_port_nbr_conn_chain,
                          port_nbr_len);
          break;
#endif

        case NET_CONN_FAMILY_NONE:                                          // See Note #3.
        default:
          break;
      }

      if (found != DEF_YES) {                                               // If NOT found, ...
        p_conn_chain = p_conn_chain_next;                                   // ... adv to next conn chain.
      }
    } else {                                                                // If conn chain's local addr NOT valid, ...
      p_conn_next = p_conn_chain->PrevConnPtr;
      NetConn_Close(p_conn_chain);                                          // ... close conn (see Note #2b).

      if (p_conn_next != DEF_NULL) {                                        // If any conns rem in conn chain,     ...
        p_conn_chain = p_conn_next;                                         // ...      adv to next conn in chain; ...
      } else {
        p_conn_chain = p_conn_chain_next;                                   // ... else adv to next conn chain.
      }
    }
  }

  if (found == DEF_YES) {                                                   // If net conn chain found, ..
    p_conn_chain->ConnChainAccessedCtr++;                                   // .. inc conn chain access ctr.
    CORE_ENTER_ATOMIC();
    th = NetConn_AccessedTh_nbr;
    CORE_EXIT_ATOMIC();
    if (p_conn_chain->ConnChainAccessedCtr > th) {                          // If conn chain accessed > th           ..
      p_conn_chain->ConnChainAccessedCtr = 0u;
      if (p_conn_chain != *p_conn_list) {                                   // .. & conn chain NOT    @ conn list head,
        NetConn_ChainUnlink(p_conn_list, p_conn_chain);                     // .. promote conn chain to conn list head
        NetConn_ChainInsert(p_conn_list, p_conn_chain);                     // .. (see Note #3b).
      }
    }
  }

  return (p_conn_chain);
}

/****************************************************************************************************//**
 *                                           NetConn_ChainSrch()
 *
 * @brief    Search a network connection chain for network connection with specific local & remote
 *           addresses.
 *
 * @param    p_conn_list         Pointer to a connection list.
 *
 * @param    p_conn_chain        Pointer to a connection chain.
 *
 * @param    p_addr_local        Pointer to local          address.
 *
 * @param    p_addr_wildcard     Pointer to local wildcard address.
 *
 * @param    p_addr_remote       Pointer to remote         address.
 *
 * @return   Pointer to connection with specific local & remote address, if found.
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) Network connection addresses maintained in network-order.
 *
 * @note     (2) (a) Assumes ALL connection lists' network connections' local addresses are valid.
 *
 *           - (b) Any connections whose local addresses are NOT valid are closed & unlinked
 *                   from their respective connection list.  Such connections may be accessed by
 *                   the application layer but will NOT correctly receive demultiplexed packets
 *                   due to invalid/incomplete connection address(s).
 *
 * @note     (3) (a) See 'NetConn_Srch()  Note #2b3B1b'.
 *           - (b) See 'NetConn_Srch()  Note #2b3B1a'.
 *******************************************************************************************************/
static NET_CONN *NetConn_ChainSrch(NET_CONN       **p_conn_list,
                                   NET_CONN       *p_conn_chain,
                                   CPU_INT08U     *p_addr_local,
                                   CPU_INT08U     *p_addr_wildcard,
                                   CPU_INT08U     *p_addr_remote,
                                   NET_CONN_STATE *p_conn_state)
{
  NET_CONN    *p_conn;
  NET_CONN    *p_conn_next;
  NET_CONN    *p_conn_half;
  NET_CONN    *p_conn_half_wildcard;
  NET_CONN    *p_conn_full_wildcard;
  CPU_INT08U  *p_conn_addr_local;
  CPU_INT08U  *p_conn_addr_remote;
  CPU_INT16U  th;
  CPU_BOOLEAN found;
  CPU_BOOLEAN found_local;
  CPU_BOOLEAN found_local_wildcard;
  CPU_BOOLEAN found_remote;
  CPU_BOOLEAN addr_local_wildcard;
  CPU_SIZE_T  addr_len = 0u;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------- SRCH NET CONN CHAIN ----------
  p_conn = p_conn_chain;                                        // Start @ chain head (see Note #3a).
  p_conn_half = DEF_NULL;
  p_conn_half_wildcard = DEF_NULL;
  p_conn_full_wildcard = DEF_NULL;
  *p_conn_state = NET_CONN_STATE_NONE;
  found = DEF_NO;

  switch (p_conn->Family) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_CONN_FAMILY_IP_V4_SOCK:
      addr_len = (CPU_SIZE_T)NET_SOCK_ADDR_LEN_IP_V4;
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_CONN_FAMILY_IP_V6_SOCK:
      addr_len = (CPU_SIZE_T)NET_SOCK_ADDR_LEN_IP_V6;
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
  }

  addr_local_wildcard = Mem_Cmp(p_addr_local,
                                p_addr_wildcard,
                                addr_len);

  while ((p_conn != DEF_NULL)                                   // Srch ALL net conns in chain ..
         && (found == DEF_NO)) {                                // .. until net conn found.
    p_conn_next = p_conn->NextConnPtr;

    if (p_conn->AddrLocalValid == DEF_YES) {                    // If conn's local addr valid, ...
                                                                // ... cmp to conn addrs.
      p_conn_addr_local = &p_conn->AddrLocal[0];
      p_conn_addr_remote = &p_conn->AddrRemote[0];

      found_local = Mem_Cmp(p_addr_local,
                            p_conn_addr_local,
                            addr_len);
      found_remote = Mem_Cmp(p_addr_remote,
                             p_conn_addr_remote,
                             addr_len);

      if (found_local == DEF_YES) {                             // If local addrs match;                ...
        if ((p_addr_remote != DEF_NULL)                         // ... &   remote addrs avail,          ...
            && (p_conn->AddrRemoteValid == DEF_YES)) {
          found = found_remote;                                 // ... cmp remote addrs;                ...
        } else if (p_conn->AddrRemoteValid == DEF_NO) {         // ... else if conn remote addr NOT avail,
          p_conn_half = p_conn;                                 // ... save half conn.
        }
        //                                                         Else if local addrs do NOT match,    ...
      } else if ((p_addr_wildcard != DEF_NULL)                  // ... & wildcard addr      avail,      ...
                 && (addr_local_wildcard == DEF_NO)) {          // ... &   local  addr !=   wildcard addr;
                                                                // ... cmp local  addr with wildcard addr.
        found_local_wildcard = Mem_Cmp(p_addr_wildcard,
                                       p_conn_addr_local,
                                       addr_len);

        if (found_local_wildcard == DEF_YES) {                  // If local wildcard addrs match;       ...
          if ((p_addr_remote != DEF_NULL)
              && (p_conn->AddrRemoteValid == DEF_YES)) {
            if (found_remote == DEF_YES) {                      // ... & remote addrs avail & match,    ...
              p_conn_full_wildcard = p_conn;                    // ... save full-wildcard conn;         ...
            }
          } else if (p_conn->AddrRemoteValid == DEF_NO) {           // ... else if conn remote addr NOT avail,
            p_conn_half_wildcard = p_conn;                      // ... save half-wildcard conn.
          } else {
            ;
          }
        }
      } else {
        ;
      }
    } else {                                                    // If conn's local addr NOT valid, ...
      NetConn_Close(p_conn);                                    // ... close conn (see Note #2b).
    }

    if (found != DEF_YES) {                                     // If NOT found, ...
      p_conn = p_conn_next;                                     // ... adv to next conn.
    }
  }

  if (found == DEF_YES) {                                       // Full conn found.
    *p_conn_state = NET_CONN_STATE_FULL;
  } else if (p_conn_full_wildcard != DEF_NULL) {                // Full conn found with wildcard addr.
    *p_conn_state = NET_CONN_STATE_FULL_WILDCARD;
    p_conn = p_conn_full_wildcard;
  } else if (p_conn_half != DEF_NULL) {                         // Half conn found.
    *p_conn_state = NET_CONN_STATE_HALF;
    p_conn = p_conn_half;
  } else if (p_conn_half_wildcard != DEF_NULL) {                // Half conn found with wildcard addr.
    *p_conn_state = NET_CONN_STATE_HALF_WILDCARD;
    p_conn = p_conn_half_wildcard;
  } else {                                                      // NO   conn found.
    *p_conn_state = NET_CONN_STATE_NONE;
  }

  if (p_conn != DEF_NULL) {                                     // If net conn found, ..
    p_conn->ConnAccessedCtr++;                                  // .. inc conn access ctr.
    CORE_ENTER_ATOMIC();
    th = NetConn_AccessedTh_nbr;
    CORE_EXIT_ATOMIC();
    if (p_conn->ConnAccessedCtr > th) {                         // If conn accessed > th               ..
      p_conn->ConnAccessedCtr = 0u;
      if (p_conn != p_conn_chain) {                             // .. & conn  NOT   @ conn chain head, ..
        NetConn_Unlink(p_conn);                                 // .. promote conn to conn chain head  ..
        NetConn_Add(p_conn_list, p_conn_chain, p_conn);         // .. (see Note #3b).
      }
    }
  }

  return (p_conn);
}

/****************************************************************************************************//**
 *                                           NetConn_ChainInsert()
 *
 * @brief    (1) Insert a connection chain into a connection list :
 *
 *           - (a) Insert connection chain at the head of the connection list
 *           - (b) Set each chain connection's connection list
 *
 * @param    p_conn_list     Pointer to a connection list.
 *
 * @param    p_conn_chain    Pointer to a connection chain.
 *******************************************************************************************************/
static void NetConn_ChainInsert(NET_CONN **p_conn_list,
                                NET_CONN *p_conn_chain)
{
  NET_CONN *p_conn;
  NET_CONN *p_conn_next;

  //                                                               ------- INSERT CONN CHAIN AT CONN LIST HEAD --------
  p_conn_chain->PrevChainPtr = DEF_NULL;
  p_conn_chain->NextChainPtr = *p_conn_list;

  if (*p_conn_list != DEF_NULL) {                               // If conn list NOT empty,                         ...
    (*p_conn_list)->PrevChainPtr = p_conn_chain;                // ... insert conn chain before cur conn list head ...
  }
  *p_conn_list = p_conn_chain;                                  // Set    conn chain as     new conn list head.

  //                                                               ------------ SET CHAIN CONNS' CONN LIST ------------
  p_conn = p_conn_chain;
  while (p_conn != DEF_NULL) {
    p_conn->ConnList = p_conn_list;
    p_conn_next = p_conn->NextConnPtr;
    p_conn = p_conn_next;
  }
}

/****************************************************************************************************//**
 *                                           NetConn_ChainUnlink()
 *
 * @brief    (1) Unlink a connection chain from a connection list :
 *
 *           - (a) Unlink connection chain
 *           - (b) Clear each chain connection's connection list
 *
 * @param    p_conn_list     Pointer to a connection list.
 *
 * @param    p_conn_chain    Pointer to a connection chain.
 *
 * @note     (2) Since NetConn_ChainUnlink() called ONLY to remove & then re-link connection chains,
 *                           it is NOT necessary to clear the entry's previous & next chain pointers.  However,
 *                           pointers cleared to NULL shown for correctness & completeness.
 *******************************************************************************************************/
static void NetConn_ChainUnlink(NET_CONN **p_conn_list,
                                NET_CONN *p_conn_chain)
{
  NET_CONN *p_conn_chain_prev;
  NET_CONN *p_conn_chain_next;
  NET_CONN *p_conn;
  NET_CONN *p_conn_next;

  //                                                               --------- UNLINK CONN CHAIN FROM CONN LIST ---------
  p_conn_chain_prev = p_conn_chain->PrevChainPtr;
  p_conn_chain_next = p_conn_chain->NextChainPtr;
  //                                                               Point prev conn chain to next conn chain.
  if (p_conn_chain_prev != DEF_NULL) {
    p_conn_chain_prev->NextChainPtr = p_conn_chain_next;
  } else {
    *p_conn_list = p_conn_chain_next;
  }
  //                                                               Point next conn chain to prev conn chain.
  if (p_conn_chain_next != DEF_NULL) {
    p_conn_chain_next->PrevChainPtr = p_conn_chain_prev;
  }

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)                     // Clr conn chain's chain ptrs (see Note #2).
  p_conn_chain->PrevChainPtr = DEF_NULL;
  p_conn_chain->NextChainPtr = DEF_NULL;
#endif

  //                                                               ------------ CLR CHAIN CONNS' CONN LIST ------------
  p_conn = p_conn_chain;
  while (p_conn != DEF_NULL) {
    p_conn->ConnList = DEF_NULL;
    p_conn_next = p_conn->NextConnPtr;
    p_conn = p_conn_next;
  }
}

/****************************************************************************************************//**
 *                                               NetConn_Add()
 *
 * @brief    (1) Add a network connection into a network connection list :
 *
 *           - (a) Network connection chains are added at (or promoted to) the head of a
 *                   network connection list.
 *           - (b) Network connections       are added at (or promoted to) the head of a
 *                   network connection chain.
 *
 * @param    p_conn_list     Pointer to a network connection list.
 *
 * @param    p_conn_chain    Pointer to a network connection chain.
 *
 * @param    p_conn          _list     Pointer to a network connection list.
 *******************************************************************************************************/
static void NetConn_Add(NET_CONN **p_conn_list,
                        NET_CONN *p_conn_chain,
                        NET_CONN *p_conn)
{
  NET_CONN *p_conn_chain_prev;
  NET_CONN *p_conn_chain_next;

  if (p_conn_chain == DEF_NULL) {                                   // If conn chain empty, ...
                                                                    // ... insert conn as new conn chain (see Note #1a).
    p_conn->PrevChainPtr = DEF_NULL;
    p_conn->NextChainPtr = *p_conn_list;
    p_conn->PrevConnPtr = DEF_NULL;
    p_conn->NextConnPtr = DEF_NULL;

    if (*p_conn_list != DEF_NULL) {                                 // If conn list NOT empty,                   ...
      (*p_conn_list)->PrevChainPtr = p_conn;                        // ... insert conn before cur conn list head ...
    }
    *p_conn_list = p_conn;                                          // Set    conn as     new conn list head.
  } else {                                                          // Else add net conn into existing conn chain ...
                                                                    // ... as new conn chain head (see Note #1b).
    p_conn_chain_prev = p_conn_chain->PrevChainPtr;
    p_conn_chain_next = p_conn_chain->NextChainPtr;
    p_conn_chain->PrevChainPtr = DEF_NULL;
    p_conn_chain->NextChainPtr = DEF_NULL;
    p_conn_chain->PrevConnPtr = p_conn;

    p_conn->PrevConnPtr = DEF_NULL;
    p_conn->NextConnPtr = p_conn_chain;

    p_conn->PrevChainPtr = p_conn_chain_prev;
    if (p_conn_chain_prev != DEF_NULL) {
      p_conn_chain_prev->NextChainPtr = p_conn;
    } else {
      *p_conn_list = p_conn;
    }

    p_conn->NextChainPtr = p_conn_chain_next;
    if (p_conn_chain_next != DEF_NULL) {
      p_conn_chain_next->PrevChainPtr = p_conn;
    }
    //                                                             Inherit conn chain accessed ctr.
    p_conn->ConnChainAccessedCtr = p_conn_chain->ConnChainAccessedCtr;
    p_conn_chain->ConnChainAccessedCtr = 0u;
  }

  p_conn->ConnList = p_conn_list;                                   // Mark conn's conn list ownership.
}

/****************************************************************************************************//**
 *                                               NetConn_Unlink()
 *
 * @brief    Unlink a network connection from its network connection list.
 *
 * @param    p_conn  Pointer to a network connection.
 *
 * @note     (1) Since NetConn_Unlink() called ONLY to remove & then re-link or free network
 *                   connections, it is NOT necessary to clear the entry's previous & next chain/
 *                   connection pointers.  However, pointers cleared to NULL shown for correctness
 *                   & completeness.
 *******************************************************************************************************/
static void NetConn_Unlink(NET_CONN *p_conn)
{
  NET_CONN **p_conn_list;
  NET_CONN *p_conn_prev;
  NET_CONN *p_conn_next;
  NET_CONN *p_conn_chain_prev;
  NET_CONN *p_conn_chain_next;

  p_conn_list = p_conn->ConnList;
  if (p_conn_list == DEF_NULL) {                                        // If net conn NOT in conn list, ...
    return;                                                             // ... exit unlink.
  }

  //                                                               ----- UNLINK NET CONN FROM CONN CHAIN ------
  p_conn_prev = p_conn->PrevConnPtr;
  p_conn_next = p_conn->NextConnPtr;

  if (p_conn_prev != DEF_NULL) {                                        // If prev net conn non-NULL, ...
                                                                        // ... unlink conn from middle of conn chain.
    p_conn_prev->NextConnPtr = p_conn_next;                             // Point prev conn to next conn.

    if (p_conn_next != DEF_NULL) {                                      // If next net conn non-NULL, ...
      p_conn_next->PrevConnPtr = p_conn_prev;                           // ... point next conn to prev conn.
    }
  } else {                                                              // Else unlink conn chain head.
    p_conn_chain_prev = p_conn->PrevChainPtr;
    p_conn_chain_next = p_conn->NextChainPtr;

    if (p_conn_next != DEF_NULL) {                                      // If next conn in conn chain non-NULL, ...
                                                                        // ... promote next conn to conn chain head.
      p_conn_next->PrevChainPtr = p_conn_chain_prev;
      if (p_conn_chain_prev != DEF_NULL) {
        p_conn_chain_prev->NextChainPtr = p_conn_next;
      } else {
        *p_conn_list = p_conn_next;
      }

      p_conn_next->NextChainPtr = p_conn_chain_next;
      if (p_conn_chain_next != DEF_NULL) {
        p_conn_chain_next->PrevChainPtr = p_conn_next;
      }

      p_conn_next->PrevConnPtr = DEF_NULL;
      //                                                           Inherit conn chain accessed ctr.
      p_conn_next->ConnChainAccessedCtr = p_conn->ConnChainAccessedCtr;
      p_conn->ConnChainAccessedCtr = 0u;
    } else {                                                            // Else remove conn list entirely.
      if (p_conn_chain_prev != DEF_NULL) {
        p_conn_chain_prev->NextChainPtr = p_conn_chain_next;
      } else {
        *p_conn_list = p_conn_chain_next;
      }

      if (p_conn_chain_next != DEF_NULL) {
        p_conn_chain_next->PrevChainPtr = p_conn_chain_prev;
      }
    }
  }

  p_conn->ConnList = DEF_NULL;                                          // Clr net conn's conn list ownership.

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)                             // Clr net conn's chain/conn ptrs (see Note #1).
  p_conn->PrevChainPtr = DEF_NULL;
  p_conn->NextChainPtr = DEF_NULL;
  p_conn->PrevConnPtr = DEF_NULL;
  p_conn->NextConnPtr = DEF_NULL;
#endif
}

/****************************************************************************************************//**
 *                                               NetConn_Close()
 *
 * @brief    (1) Close a network connection :
 *
 *           - (a) Update network connection close statistic(s)
 *
 *           - (b) Close  network connection(s) :
 *               - (1) Close application connection
 *               - (2) Close transport   connection
 *
 *           - (c) Free   network connection, if necessary
 *
 *
 * Argument(s) : p_conn      Pointer to a network connection.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) Network connection's handle identifier MUST be obtained PRIOR to any network
 *                   connection validation &/or close operations.
 *
 * @note     (3) (a) Network connection(s) MAY  already be closed BEFORE any  network connection
 *                   close operations & MUST be validated as used BEFORE each network connection
 *                   close operation.
 *
 *                       - (b) Network connection SHOULD already be closed AFTER application & transport
 *                   layer connections have both closed.
 *
 *                   See also 'NetConn_CloseFromApp()        Note #1c'
 *                   & 'NetConn_CloseFromTransport()  Note #1c'.
 *******************************************************************************************************/
static void NetConn_Close(NET_CONN *p_conn)
{
  NET_CONN_ID conn_id;
  CPU_BOOLEAN used;

  //                                                               --------------- UPDATE CLOSE STATS -----------------
  NET_CTR_ERR_INC(Net_ErrCtrs.Conn.CloseCtr);

  //                                                               ------------------ CLOSE CONN(S) -------------------
  conn_id = p_conn->ID;                                         // Get net conn id (see Note #2).

  used = NetConn_IsUsed(conn_id);
  if (used == DEF_YES) {                                        // If net conn used (see Note #3a), ...
    NetConn_CloseApp(p_conn);                                   // ... close app conn.
  }

  used = NetConn_IsUsed(conn_id);
  if (used == DEF_YES) {                                        // If net conn used (see Note #3a), ...
    NetConn_CloseTransport(p_conn);                             // ... close transport conn.
  }

  //                                                               ------------------ FREE NET CONN -------------------
  used = NetConn_IsUsed(conn_id);
  if (used == DEF_YES) {                                        // If net conn used (see Note #3b), ...
    NetConn_FreeHandler(p_conn);                                // ... free net conn.
  }
}

/****************************************************************************************************//**
 *                                           NetConn_CloseApp()
 *
 * @brief    (1) Close a network connection's application connection :
 *
 *           - (a) Free  network     connection from application clone connection, if necessary
 *           - (b) Close application connection
 *
 *
 * Argument(s) : p_conn      Pointer to a network connection.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
static void NetConn_CloseApp(NET_CONN *p_conn)
{
  //                                                               ---------------- CLOSE APP CONN(S) -----------------
  NetSock_FreeConnFromSock(p_conn->ID_AppClone,                 // Free net conn from clone sock conn (see Note #1a).
                           p_conn->ID);

  NetSock_CloseFromConn((NET_SOCK_ID)p_conn->ID_App);           // Close sock app conn (see Note #1b).

  PP_UNUSED_PARAM(p_conn);                                      // Prevent possible 'variable unused' warning.
}

/****************************************************************************************************//**
 *                                           NetConn_CloseTransport()
 *
 * @brief    Close a network connection's transport connection.
 *
 * @param    p_conn  Pointer to a network connection.
 *******************************************************************************************************/
static void NetConn_CloseTransport(NET_CONN *p_conn)
{
  //                                                               --------------- CLOSE TRANSPORT CONN --------------
#ifdef  NET_TCP_MODULE_EN
  NetTCP_ConnCloseFromConn((NET_TCP_CONN_ID)p_conn->ID_Transport);
#endif

  PP_UNUSED_PARAM(p_conn);                                      // Prevent possible 'variable unused' warning.
}

/****************************************************************************************************//**
 *                                       NetConn_CloseAllConnsHandler()
 *
 * @brief    Close ALL network connections in ALL network connection lists for specified conditions.
 *
 * @param    if_nbr      Interface number to close connection :
 *                       Interface number        Close ALL connections on interface.
 *                       NET_IF_NBR_WILDCARD,    Close ALL connections with a wildcard
 *                       address.
 *
 * @param    p_addr      Pointer to protocol address to close connection (see Note #1).
 *
 * @param    addr_len    Length  of protocol address (in octets).
 *
 * @param    close_code  Select which close action to perform :
 *                       NET_CONN_CLOSE_ALL          Close all network connections.
 *                       NET_CONN_CLOSE_BY_IF        Close all network connections with
 *                       specified interface number.
 *                       NET_CONN_CLOSE_BY_ADDR      Close all network connections with
 *                       specified local protocol address.
 *
 * @note     (1) Protocol address MUST be in network-order.
 *
 * @note     (2) Since network connection close handlers execute asynchronously to NetConn_FreeHandler(),
 *                       network connection list pointers ('NetConn_ConnListChainPtr', 'NetConn_ConnListConnPtr',
 *                       etc.) MUST be coordinated with NetConn_FreeHandler() to avoid network connection list
 *                       corruption :
 *
 *                           - (a) (1) Network connection list pointers typically advance to the next network connection
 *                       in a network connection list.
 *
 *                           - (2) However, whenever a network connection list pointer connection is freed by an
 *                       asynchronous network connection close, the network connection list pointer(s)
 *                       MUST be advanced to the next valid & available network connection in the
 *                       network connection list.
 *
 *                       See also 'NetConn_FreeHandler()  Note #2a'.
 *
 *                           - (b) Network connection list pointers MUST be cleared after handling the network
 *                       connection list.
 *
 *                           - (1) However, network connection list pointers are implicitly cleared after
 *                       handling the network connection list.
 *
 * @note     (3) Invalid network connection(s) in the connection list MAY already be closed in other
 *                       validation functions.
 *******************************************************************************************************/
static void NetConn_CloseAllConnsHandler(NET_IF_NBR          if_nbr,
                                         CPU_INT08U          *p_addr,
                                         NET_CONN_ADDR_LEN   addr_len,
                                         NET_CONN_CLOSE_CODE close_code)
{
  NET_CONN          **p_conn_list;
  NET_CONN_LIST_QTY i;

  //                                                               ---- CLOSE CONNS IN ALL NET CONN LISTS -----
  p_conn_list = &NetConn_ConnListHead[0];
  for (i = 0u; i < NET_CONN_PROTOCOL_NBR_MAX; i++) {
    NetConn_ConnListChainPtr = *p_conn_list;                            // Start @ conn list head.
    while (NetConn_ConnListChainPtr != DEF_NULL) {                      // Close net conn chains ...
      NetConn_ConnListNextChainPtr = NetConn_ConnListChainPtr->NextChainPtr;
      NetConn_ConnListConnPtr = NetConn_ConnListChainPtr;

      while (NetConn_ConnListConnPtr != DEF_NULL) {                     // ... & net conns.
        NetConn_ConnListNextConnPtr = NetConn_ConnListConnPtr->NextConnPtr;
        //                                                         Close net conn, if req'd.
        NetConn_CloseAllConnsCloseConn(NetConn_ConnListConnPtr,
                                       if_nbr,
                                       p_addr,
                                       addr_len,
                                       close_code);

        NetConn_ConnListConnPtr = NetConn_ConnListNextConnPtr;          // Adv to next net conn       (see Note #2a1).
      }

      NetConn_ConnListChainPtr = NetConn_ConnListNextChainPtr;          // Adv to next net conn chain (see Note #2a1).
    }

#if 0                                                                   // Clr net conn list ptrs     (see Note #2b1).
    NetConn_ConnListChainPtr = DEF_NULL;
    NetConn_ConnListConnPtr = DEF_NULL;
    NetConn_ConnListNextChainPtr = DEF_NULL;
    NetConn_ConnListNextConnPtr = DEF_NULL;
#endif

    p_conn_list++;
  }
}

/****************************************************************************************************//**
 *                                       NetConn_CloseAllConnsCloseConn()
 *
 * @brief    Close network connection for specified conditions.
 *
 * @param    p_conn      Pointer to network connection to close.
 *
 * @param    if_nbr      Interface number to close connection :
 *                       Interface number        Close ALL connections on interface.
 *                       NET_IF_NBR_WILDCARD,    Close ALL connections with a wildcard
 *                       address.
 *
 * @param    p_addr      Pointer to protocol address to close connection (see Note #1).
 *
 * @param    addr_len    Length  of protocol address (in octets).
 *
 * @param    close_code  Select which close action to perform :
 *                       NET_CONN_CLOSE_ALL          Close all network connections.
 *                       NET_CONN_CLOSE_BY_IF        Close all network connections with
 *                       specified interface number.
 *                       NET_CONN_CLOSE_BY_ADDR      Close all network connections with
 *                       specified local protocol address.
 *
 * @note     (1) Protocol address MUST be in network-order.
 *******************************************************************************************************/
static void NetConn_CloseAllConnsCloseConn(NET_CONN            *p_conn,
                                           NET_IF_NBR          if_nbr,
                                           CPU_INT08U          *p_addr,
                                           NET_CONN_ADDR_LEN   addr_len,
                                           NET_CONN_CLOSE_CODE close_code)
{
  CPU_INT08U  *p_addr_conn;
  CPU_BOOLEAN close_conn;

  close_conn = DEF_NO;

  switch (close_code) {
    case NET_CONN_CLOSE_ALL:
      close_conn = DEF_YES;
      break;

    case NET_CONN_CLOSE_BY_IF:                                          // Cmp IF   to conn's IF.
      close_conn = (p_conn->IF_Nbr == if_nbr) ? DEF_YES : DEF_NO;
      break;

    case NET_CONN_CLOSE_BY_ADDR:
      switch (p_conn->Family) {
#ifdef  NET_IPv4_MODULE_EN
        case NET_CONN_FAMILY_IP_V4_SOCK:
          if (addr_len != NET_CONN_ADDR_IP_V4_LEN_ADDR) {
            break;
          }
          //                                                       Cmp addr to conn's addr.
          p_addr_conn = &p_conn->AddrLocal[0] + NET_CONN_ADDR_IP_V4_IX_ADDR;
          close_conn = Mem_Cmp(p_addr,
                               p_addr_conn,
                               addr_len);
          break;
#endif

#ifdef  NET_IPv6_MODULE_EN
        case NET_CONN_FAMILY_IP_V6_SOCK:
          if (addr_len != NET_CONN_ADDR_IP_V6_LEN_ADDR) {
            break;
          }
          //                                                       Cmp addr to conn's addr.
          p_addr_conn = &p_conn->AddrLocal[0] + NET_CONN_ADDR_IP_V6_IX_ADDR;
          close_conn = Mem_Cmp(p_addr,
                               p_addr_conn,
                               addr_len);
          break;
#endif

        case NET_CONN_FAMILY_NONE:
        default:
          break;
      }
      break;

    case NET_CONN_CLOSE_NONE:
    default:
      break;
  }

  if (close_conn == DEF_YES) {
    NetConn_Close(p_conn);
  }
}

/****************************************************************************************************//**
 *                                           NetConn_FreeHandler()
 *
 * @brief    (1) Free a network connection :
 *
 *           - (a) Remove network connection from a   network connection list :
 *               - (1) Update network connection list pointer(s)                       See Note #2
 *               - (2) Unlink network connection from network connection list
 *
 *           - (b) Clear  network connection values
 *           - (c) Free   network connection back to network connection pool
 *           - (d) Update network connection pool statistics
 *
 * @param    p_conn  Pointer to a network connection.
 *
 * @note     (2) Since network connection close handlers execute asynchronously to NetConn_FreeHandler(),
 *                   network connection list pointers ('NetConn_ConnListChainPtr', 'NetConn_ConnListConnPtr',
 *                   etc.) MUST be coordinated with NetConn_FreeHandler() to avoid network connection list
 *                   corruption :
 *
 *                       - (a) Whenever a network connection list pointer connection is freed, network connection
 *                   list pointers MUST be advanced to the next valid & available network connection in
 *                   the appropriate network connection list.
 *
 *                   See also 'NetConn_CloseAllConnsHandler()  Note #3'.
 *******************************************************************************************************/
static void NetConn_FreeHandler(NET_CONN *p_conn)
{
  NET_CONN *p_conn_next;
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  //                                                               -------- REMOVE NET CONN FROM NET CONN LIST --------
  //                                                               If net conn is next conn list conn to update, ...
  //                                                               ... adv to skip this net conn (see Note #2a).
  if (p_conn == NetConn_ConnListNextChainPtr) {
    p_conn_next = NetConn_ConnListNextChainPtr->NextChainPtr;
    NetConn_ConnListNextChainPtr = p_conn_next;
  }
  if (p_conn == NetConn_ConnListNextConnPtr) {
    p_conn_next = NetConn_ConnListNextConnPtr->NextConnPtr;
    NetConn_ConnListNextConnPtr = p_conn_next;
  }

  NetConn_Unlink(p_conn);                                       // Unlink net conn from net conn list.

  //                                                               ------------------- CLR NET CONN -------------------
  //                                                               Set net conn as NOT used.
  DEF_BIT_CLR(p_conn->Flags, (NET_CONN_FLAGS)NET_CONN_FLAG_USED);
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
  NetConn_Clr(p_conn);
#endif

  //                                                               ------------------ FREE NET CONN -------------------
  p_conn->NextConnPtr = NetConn_PoolPtr;
  NetConn_PoolPtr = p_conn;

#if (NET_STAT_POOL_CONN_EN == DEF_ENABLED)
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  //                                                               ------------ UPDATE NET CONN POOL STATS ------------
  NetStat_PoolEntryUsedDec(&NetConn_PoolStat, &local_err);
#endif

  PP_UNUSED_PARAM(local_err);
}

/****************************************************************************************************//**
 *                                               NetConn_Clr()
 *
 * @brief    Clear network connection controls.
 *
 * @param    p_conn  Pointer to a network connection.
 *******************************************************************************************************/
static void NetConn_Clr(NET_CONN *p_conn)
{
  p_conn->PrevChainPtr = DEF_NULL;
  p_conn->NextChainPtr = DEF_NULL;
  p_conn->PrevConnPtr = DEF_NULL;
  p_conn->NextConnPtr = DEF_NULL;
  p_conn->ConnList = DEF_NULL;
  p_conn->ID_AppClone = NET_CONN_ID_NONE;
  p_conn->ID_Transport = NET_CONN_ID_NONE;
  p_conn->IF_Nbr = NET_IF_NBR_NONE;
  p_conn->Family = NET_CONN_FAMILY_NONE;
  p_conn->ProtocolIx = NET_CONN_PROTOCOL_IX_NONE;
  p_conn->AddrLocalValid = DEF_NO;
  p_conn->AddrRemoteValid = DEF_NO;
  p_conn->ConnChainAccessedCtr = 0u;
  p_conn->ConnAccessedCtr = 0u;
  p_conn->Flags = NET_CONN_FLAG_NONE;

#ifdef  NET_IPv4_MODULE_EN
  p_conn->TxIPv4Flags = NET_IPv4_FLAG_NONE;
  p_conn->TxIPv4TOS = NET_IPv4_TOS_DFLT;
  p_conn->TxIPv4TTL = NET_IPv4_TTL_DFLT;
#ifdef  NET_MCAST_TX_MODULE_EN
  p_conn->TxIPv4TTL_Multicast = NET_IPv4_TTL_MULTICAST_DFLT;
#endif
#if 0
  p_conn->TxIP_Opts = DEF_NULL;
#endif
#endif

#ifdef  NET_IPv6_MODULE_EN
  p_conn->TxIPv6TrafficClass = NET_IPv6_TRAFFIC_CLASS_DFLT;
  p_conn->TxIPv6FlowLabel = NET_IPv6_FLOW_LABEL_DFLT;
  p_conn->TxIPv6HopLim = NET_IPv6_HDR_HOP_LIM_DFLT;
  p_conn->TxIPv6Flags = NET_IPv6_FLAG_NONE;
#ifdef  NET_MCAST_TX_MODULE_EN
  p_conn->TxIPv6HopLimMulticast = NET_IPv6_HOP_LIM_MULTICAST_DFLT;
#endif
#endif

  Mem_Clr(&p_conn->AddrLocal[0],
          NET_CONN_ADDR_LEN_MAX);
  Mem_Clr(&p_conn->AddrRemote[0],
          NET_CONN_ADDR_LEN_MAX);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
